#include "vtysh_telnet.h"
#include <zebra.h>
#include "command.h"
#include "vtysh.h"
#include <lib/memory.h>
#include <lib/memtypes.h>
#include "vtysh_session.h"
#include "vtysh_user.h"
#include <arpa/telnet.h>
#include "buffer.h"
#include <pthread.h>
#include "thread.h"
#include "ftm/pkt_tcp.h"
#include "sshd/channels.h"
#include "vtysh_sync.h"



#define CONTROL(X)  ((X) - '@')
#define VTY_NORMAL     0
#define VTY_PRE_ESCAPE 1
#define VTY_ESCAPE     2

static const char telnet_backward_char = 0x08;
static const char telnet_space_char = ' ';
static int g_telnet_server_enable = TELNET_SERVER_ENABLE;
//extern struct thread_master *vtysh_master;

static void vtysh_telnet_close_session(void)
{
    struct login_session *session;
    struct listnode *node, *nnode;
    
    pthread_mutex_lock(&session_delete_lock);

    for(ALL_LIST_ELEMENTS(user_session_list, node, nnode, session))
    {
        if(session->session_type == SESSION_TELNET)
        {
            session->session_status = SESSION_CLOSE;
        }
    }
    
    pthread_mutex_unlock(&session_delete_lock);
}
void vtysh_telnet_enable_set(int val)
{
	if(val == 1)
	{
		g_telnet_server_enable = TELNET_SERVER_ENABLE;
	}
	else
	{
		g_telnet_server_enable = TELNET_SERVER_DISABLE;
		vtysh_telnet_close_session();
	}
}

static int vtysh_telnet_enable_get(void)
{
	return g_telnet_server_enable;
}

/* Send WILL TELOPT_ECHO to remote server. */
void
vty_will_echo(struct vty *vty)
{
    unsigned char cmd[] = { IAC, WILL, TELOPT_ECHO, '\0' };
    vty_out(vty, "%s", cmd);
}

/* Make suppress Go-Ahead telnet option. */
void
vty_will_suppress_go_ahead(struct vty *vty)
{
    unsigned char cmd[] = { IAC, WILL, TELOPT_SGA, '\0' };
    vty_out(vty, "%s", cmd);
}

/* Make don't use linemode over telnet. */
static void
vty_dont_linemode(struct vty *vty)
{
    unsigned char cmd[] = { IAC, DONT, TELOPT_LINEMODE, '\0' };
    vty_out(vty, "%s", cmd);
}

/* Use window size. */
static void
vty_do_window_size(struct vty *vty)
{
    unsigned char cmd[] = { IAC, DO, TELOPT_NAWS, '\0' };
    vty_out(vty, "%s", cmd);
}

/* Basic function to write buffer to vty. */
static void
vty_write(struct vty *vty, const char *buf, size_t nbytes)
{
    struct login_session *session = (struct login_session *)vty->session;

    if(vty->node == AUTH_NODE && session->auth_prompt == 0)
    {
        return;
    }

    pthread_mutex_lock(&vty->obuf_lock);
    /* Should we do buffering here ?  And make vty_flush (vty) ? */
    buffer_put(vty->obuf, buf, nbytes);
    pthread_mutex_unlock(&vty->obuf_lock);
}

/* Basic function to insert character into vty. */
static void
vty_self_insert(struct vty *vty, char c)
{
    int i;
    int length;

    vty_ensure(vty, vty->length + 1);
    length = vty->length - vty->cp;
    memmove(&vty->buf[vty->cp + 1], &vty->buf[vty->cp], length);
    vty->buf[vty->cp] = c;

    vty_write(vty, &vty->buf[vty->cp], length + 1);

    for(i = 0; i < length; i++)
    {
        vty_write(vty, &telnet_backward_char, 1);
    }

    vty->cp++;
    vty->length++;
}

/* Self insert character 'c' in overwrite mode. */
static void
vty_self_insert_overwrite(struct vty *vty, char c)
{
    vty_ensure(vty, vty->length + 1);
    vty->buf[vty->cp++] = c;

    if(vty->cp > vty->length)
    {
        vty->length++;
    }

    if(vty->node == AUTH_NODE)
    {
        return;
    }

    vty_write(vty, &c, 1);
}

/* Insert a word into vty interface with overwrite mode. */
static void
vty_insert_word_overwrite(struct vty *vty, char *str)
{
    int len = strlen(str);
    vty_write(vty, str, len);
    strcpy(&vty->buf[vty->cp], str);
    vty->cp += len;
    vty->length = vty->cp;
}

/* Forward character. */
static void
vty_forward_char(struct vty *vty)
{
    if(vty->cp < vty->length)
    {
        vty_write(vty, &vty->buf[vty->cp], 1);
        vty->cp++;
    }
}

/* Backward character. */
void
vty_backward_char(struct vty *vty)
{
    if(vty->cp > 0)
    {
        vty->cp--;
        vty_write(vty, &telnet_backward_char, 1);
    }
}

/* Move to the beginning of the line. */
static void
vty_beginning_of_line(struct vty *vty)
{
    while(vty->cp)
    {
        vty_backward_char(vty);
    }
}

/* Move to the end of the line. */
static void
vty_end_of_line(struct vty *vty)
{
    while(vty->cp < vty->length)
    {
        vty_forward_char(vty);
    }
}

static void vty_kill_line_from_beginning(struct vty *);
static void vty_redraw_line(struct vty *);

/* Print command line history.  This function is called from
   vty_next_line and vty_previous_line. */
static void
vty_history_print(struct vty *vty)
{
    int length;

    vty_kill_line_from_beginning(vty);

    /* Get previous line from history buffer */
    length = strlen(vty->hist[vty->hp]);
    memcpy(vty->buf, vty->hist[vty->hp], length);
    vty->cp = vty->length = length;

    /* Redraw current line */
    vty_redraw_line(vty);
}

/* Show next command line history. */
static void
vty_next_line(struct vty *vty)
{
    int try_index;

    if(vty->hp == vty->hindex)
    {
        return;
    }

    /* Try is there history exist or not. */
    try_index = vty->hp;

    if(try_index == (VTY_MAXHIST - 1))
    {
        try_index = 0;
    }
    else
    {
        try_index++;
    }

    /* If there is not history return. */
    if(vty->hist[try_index] == NULL)
    {
        return;
    }
    else
    {
        vty->hp = try_index;
    }

    vty_history_print(vty);
}

/* Show previous command line history. */
static void
vty_previous_line(struct vty *vty)
{
    int try_index;

    try_index = vty->hp;

    if(try_index == 0)
    {
        try_index = VTY_MAXHIST - 1;
    }
    else
    {
        try_index--;
    }

    if(vty->hist[try_index] == NULL)
    {
        return;
    }
    else
    {
        vty->hp = try_index;
    }

    vty_history_print(vty);
}

/* This function redraw all of the command line character. */
static void
vty_redraw_line(struct vty *vty)
{
    vty_write(vty, vty->buf, vty->length);
    vty->cp = vty->length;
}

/* Forward word. */
static void
vty_forward_word(struct vty *vty)
{
    while(vty->cp != vty->length && vty->buf[vty->cp] != ' ')
    {
        vty_forward_char(vty);
    }

    while(vty->cp != vty->length && vty->buf[vty->cp] == ' ')
    {
        vty_forward_char(vty);
    }
}

/* Backward word without skipping training space. */
static void
vty_backward_pure_word(struct vty *vty)
{
    while(vty->cp > 0 && vty->buf[vty->cp - 1] != ' ')
    {
        vty_backward_char(vty);
    }
}

/* Backward word. */
static void
vty_backward_word(struct vty *vty)
{
    while(vty->cp > 0 && vty->buf[vty->cp - 1] == ' ')
    {
        vty_backward_char(vty);
    }

    while(vty->cp > 0 && vty->buf[vty->cp - 1] != ' ')
    {
        vty_backward_char(vty);
    }
}

/* When '^D' is typed at the beginning of the line we move to the down
   level. */
static void
vty_down_level(struct vty *vty)
{
    vty_out(vty, "%s", VTY_NEWLINE);
    (*config_exit_cmd.func)(NULL, vty, 0, NULL);
    vty_prompt(vty);
    vty->cp = 0;
}

/* Delete a charcter at the current point. */
static void
vty_delete_char(struct vty *vty)
{
    int i;
    int size;
    struct login_session *session = (struct login_session *)vty->session;

    if(vty->length == 0)
    {
        vty_down_level(vty);
        return;
    }

    if(vty->cp == vty->length)
    {
        return;    /* completion need here? */
    }

    size = vty->length - vty->cp;

    vty->length--;
    memmove(&vty->buf[vty->cp], &vty->buf[vty->cp + 1], size - 1);
    vty->buf[vty->length] = '\0';

    if(vty->node == AUTH_NODE  && session->auth_prompt == 0)
    {
        return;
    }

    vty_write(vty, &vty->buf[vty->cp], size - 1);
    vty_write(vty, &telnet_space_char, 1);

    for(i = 0; i < size; i++)
    {
        vty_write(vty, &telnet_backward_char, 1);
    }
}

/* Delete a character before the point. */
static void
vty_delete_backward_char(struct vty *vty)
{
    if(vty->cp == 0)
    {
        return;
    }

    vty_backward_char(vty);
    vty_delete_char(vty);
}

/* Kill rest of line from current point. */
static void
vty_kill_line(struct vty *vty)
{
    int i;
    int size;

    size = vty->length - vty->cp;

    if(size == 0)
    {
        return;
    }

    for(i = 0; i < size; i++)
    {
        vty_write(vty, &telnet_space_char, 1);
    }

    for(i = 0; i < size; i++)
    {
        vty_write(vty, &telnet_backward_char, 1);
    }

    memset(&vty->buf[vty->cp], 0, size);
    vty->length = vty->cp;
}

/* Kill line from the beginning. */
static void
vty_kill_line_from_beginning(struct vty *vty)
{
    vty_beginning_of_line(vty);
    vty_kill_line(vty);
}

/* Delete a word before the point. */
static void
vty_forward_kill_word(struct vty *vty)
{
    while(vty->cp != vty->length && vty->buf[vty->cp] == ' ')
    {
        vty_delete_char(vty);
    }

    while(vty->cp != vty->length && vty->buf[vty->cp] != ' ')
    {
        vty_delete_char(vty);
    }
}

/* Delete a word before the point. */
static void
vty_backward_kill_word(struct vty *vty)
{
    while(vty->cp > 0 && vty->buf[vty->cp - 1] == ' ')
    {
        vty_delete_backward_char(vty);
    }

    while(vty->cp > 0 && vty->buf[vty->cp - 1] != ' ')
    {
        vty_delete_backward_char(vty);
    }
}

/* Transpose chars before or at the point. */
static void
vty_transpose_chars(struct vty *vty)
{
    char c1, c2;

    /* If length is short or point is near by the beginning of line then
       return. */
    if(vty->length < 2 || vty->cp < 1)
    {
        return;
    }

    /* In case of point is located at the end of the line. */
    if(vty->cp == vty->length)
    {
        c1 = vty->buf[vty->cp - 1];
        c2 = vty->buf[vty->cp - 2];

        vty_backward_char(vty);
        vty_backward_char(vty);
        vty_self_insert_overwrite(vty, c1);
        vty_self_insert_overwrite(vty, c2);
    }
    else
    {
        c1 = vty->buf[vty->cp];
        c2 = vty->buf[vty->cp - 1];

        vty_backward_char(vty);
        vty_self_insert_overwrite(vty, c1);
        vty_self_insert_overwrite(vty, c2);
    }
}

/* When '^Z' is received from vty, move down to the enable mode. */
void
vty_end_config(struct vty *vty)
{
    vty_out(vty, "%s", VTY_NEWLINE);

    switch(vty->node)
    {
        case CONFIG_NODE:
            break;

        case USER_NODE:
        case INTERFACE_NODE:
        case RIP_NODE:
        case RIPNG_NODE:
        case BGP_NODE:
        case OSPF_NODE:
        case OSPF_DCN_NODE:
		case SDH_DXC_NODE:
        case ISIS_NODE:
//        case VTY_NODE:
        case PHYSICAL_IF_NODE:
        case PHYSICAL_SUBIF_NODE:
        case TDM_IF_NODE:
        case TDM_SUBIF_NODE:
        case STM_IF_NODE:
        case STM_SUBIF_NODE:
        case TUNNEL_IF_NODE:
        case LOOPBACK_IF_NODE:
        case TRUNK_IF_NODE:
        case TRUNK_SUBIF_NODE:
        case VLANIF_NODE:
        case CLOCK_NODE:
		case VCG_NODE:
		case E1_IF_NODE:	
        case OSPF6_NODE:
        case TCP_NODE:
        case SYSTEM_NODE:
//      vty_config_unlock (vty);
            vty->node = CONFIG_NODE;
            break;

        default:
            /* Unknown node, we have to ignore it. */
            break;
    }

    vty_prompt(vty);
    vty->cp = 0;
}

/* ^C stop current input and do not add command line to the history. */
void
vty_stop_input(struct vty *vty)
{
    vty->cp = vty->length = 0;
    vty_clear_buf(vty);
    vty_out(vty, "%s", VTY_NEWLINE);
#if 0

    switch(vty->node)
    {
        case ENABLE_NODE:
        case RESTRICTED_NODE:
            /* Nothing to do. */
            break;

        case CONFIG_NODE:
        case INTERFACE_NODE:
        case ZEBRA_NODE:
        case RIP_NODE:
        case RIPNG_NODE:
        case BABEL_NODE:
        case BGP_NODE:
        case RMAP_NODE:
        case OSPF_NODE:
        case OSPF6_NODE:
        case ISIS_NODE:
        case KEYCHAIN_NODE:
        case KEYCHAIN_KEY_NODE:
        case MASC_NODE:
        case PIM_NODE:
        case VTY_NODE:
        case PHYSICAL_IF_NODE:
        case PHYSICAL_SUBIF_NODE:
        case TUNNEL_IF_NODE:
        case LOOPBACK_IF_NODE:
        case TRUNK_IF_NODE:
        case VLANIF_NODE:
//      vty_config_unlock (vty);
            vty->node = ENABLE_NODE;
            break;

        default:
            /* Unknown node, we have to ignore it. */
            break;
    }

#endif
    vty_prompt(vty);

    /* Set history pointer to the latest one. */
    vty->hp = vty->hindex;
}

/* Execute current command line. */
int
vty_telnet_execute(struct vty *vty)
{
    int ret;
    struct login_session *session;

    ret = CMD_SUCCESS;
    session = (struct login_session *)vty->session;
    
	VTY_DEBUG(VD_TELNET, "node %d, ul->auth_mode %d", vty->node, 
										session->ul->auth_mode);
    switch(vty->node)
    {
        case AUTH_NODE:
            vty_auth(vty, vty->buf);
            break;

        default:
//            VTYSH_CMD_EXEC_LOCK;
            vtysh_execute(vty->buf, vty);
//            VTYSH_CMD_EXEC_UNLOCK;
            if(vty->type == VTY_TERM || vty->type == VTY_SSH)
            {
                vty_hist_add(vty);
            }

            break;
    }

    /* Clear command line buffer. */
    vty->cp = vty->length = 0;
    vty_clear_buf(vty);

    if(session != NULL && session->session_status != SESSION_CLOSE)
    {
        if(session->auth_prompt == 1 && vty->node == AUTH_NODE)
        {
            vty_out(vty, "Username: ");
        }
        else
        {
            vty_prompt(vty);
        }
    }

    return ret;
}

/* Do completion at vty interface. */
static void
vty_complete_command(struct vty *vty)
{
    int i;
    int ret;
    char **matched = NULL;
    vector vline;

    if(vty->node == AUTH_NODE)
    {
        return;
    }

    vline = cmd_make_strvec(vty->buf);

    if(vline == NULL)
    {
        return;
    }

    /* In case of 'help \t'. */
    if(isspace((int) vty->buf[vty->length - 1]))
    {
        vector_set(vline, NULL);
    }

    matched = cmd_complete_command_lib(vline, vty, &ret, 1);

    cmd_free_strvec(vline);

    vty_out(vty, "%s", VTY_NEWLINE);

    switch(ret)
    {
        case CMD_ERR_AMBIGUOUS:
            vty_out(vty, "%% Ambiguous command.%s", VTY_NEWLINE);
            vty_prompt(vty);
            vty_redraw_line(vty);
            break;

        case CMD_ERR_NO_MATCH:
            /* vty_out (vty, "%% There is no matched command.%s", VTY_NEWLINE); */
            vty_prompt(vty);
            vty_redraw_line(vty);
            break;

        case CMD_COMPLETE_FULL_MATCH:
            vty_prompt(vty);
            vty_redraw_line(vty);
            vty_backward_pure_word(vty);
            vty_insert_word_overwrite(vty, matched[0]);
            vty_self_insert(vty, ' ');
            XFREE(MTYPE_TMP, matched[0]);
            break;

        case CMD_COMPLETE_MATCH:
            vty_prompt(vty);
            vty_redraw_line(vty);
            vty_backward_pure_word(vty);
            vty_insert_word_overwrite(vty, matched[0]);
            XFREE(MTYPE_TMP, matched[0]);
            vector_only_index_free(matched);
            return;
            break;

        case CMD_COMPLETE_LIST_MATCH:
            for(i = 0; matched[i] != NULL; i++)
            {
                if(i != 0 && ((i % 6) == 0))
                {
                    vty_out(vty, "%s", VTY_NEWLINE);
                }

                vty_out(vty, "%-10s ", matched[i]);
                XFREE(MTYPE_TMP, matched[i]);
            }

            vty_out(vty, "%s", VTY_NEWLINE);

            vty_prompt(vty);
            vty_redraw_line(vty);
            break;

        case CMD_ERR_NOTHING_TODO:
            vty_prompt(vty);
            vty_redraw_line(vty);
            break;

        default:
            break;
    }

    if(matched)
    {
        vector_only_index_free(matched);
    }
}

static void
vty_describe_fold(struct vty *vty, int cmd_width,
                  unsigned int desc_width, struct cmd_token *token)
{
    char *buf;
    const char *cmd, *p;
    int pos;

    cmd = token->cmd[0] == '.' ? token->cmd + 1 : token->cmd;

    if(desc_width <= 0)
    {
        vty_out(vty, "  %-*s  %s%s", cmd_width, cmd, token->desc, VTY_NEWLINE);
        return;
    }

    buf = XCALLOC(MTYPE_TMP, strlen(token->desc) + 1);

    for(p = token->desc; strlen(p) > desc_width; p += pos + 1)
    {
        for(pos = desc_width; pos > 0; pos--)
            if(*(p + pos) == ' ')
            {
                break;
            }

        if(pos == 0)
        {
            break;
        }

        strncpy(buf, p, pos);
        buf[pos] = '\0';
        vty_out(vty, "  %-*s  %s%s", cmd_width, cmd, buf, VTY_NEWLINE);

        cmd = "";
    }

    vty_out(vty, "  %-*s  %s%s", cmd_width, cmd, p, VTY_NEWLINE);

    XFREE(MTYPE_TMP, buf);
}

/* Describe matched command function. */
static void
vty_describe_command(struct vty *vty)
{
    int ret;
    vector vline;
    vector describe;
    unsigned int i, width, desc_width;
    struct cmd_token *token, *token_cr = NULL;

    vline = cmd_make_strvec(vty->buf);

    /* In case of '> ?'. */
    if(vline == NULL)
    {
        vline = vector_init(1);
        vector_set(vline, NULL);
    }
    else if(isspace((int) vty->buf[vty->length - 1]))
    {
        vector_set(vline, NULL);
    }

    describe = cmd_describe_command(vline, vty, &ret);

    vty_out(vty, "%s", VTY_NEWLINE);

    /* Ambiguous error. */
    switch(ret)
    {
        case CMD_ERR_AMBIGUOUS:
            vty_out(vty, "%% Ambiguous command.%s", VTY_NEWLINE);
            goto out;
            break;

        case CMD_ERR_NO_MATCH:
            vty_out(vty, "%% There is no matched command.%s", VTY_NEWLINE);
            goto out;
            break;
    }

    /* Get width of command string. */
    width = 0;

    for(i = 0; i < vector_active(describe); i++)
        if((token = vector_slot(describe, i)) != NULL)
        {
            unsigned int len;

            if(token->cmd[0] == '\0')
            {
                continue;
            }

            len = strlen(token->cmd);

            if(token->cmd[0] == '.')
            {
                len--;
            }

            if(width < len)
            {
                width = len;
            }
        }

    /* Get width of description string. */
    desc_width = vty->width - (width + 6);

    /* Print out description. */
    for(i = 0; i < vector_active(describe); i++)
        if((token = vector_slot(describe, i)) != NULL)
        {
            if(token->cmd[0] == '\0')
            {
                continue;
            }

            if(strcmp(token->cmd, command_cr) == 0)
            {
                token_cr = token;
                continue;
            }

            if(!token->desc)
                vty_out(vty, "  %-s%s",
                        token->cmd[0] == '.' ? token->cmd + 1 : token->cmd,
                        VTY_NEWLINE);
            else if(desc_width >= strlen(token->desc))
                vty_out(vty, "  %-*s  %s%s", width,
                        token->cmd[0] == '.' ? token->cmd + 1 : token->cmd,
                        token->desc, VTY_NEWLINE);
            else
            {
                vty_describe_fold(vty, width, desc_width, token);
            }

#if 0
            vty_out(vty, "  %-*s %s%s", width
                    desc->cmd[0] == '.' ? desc->cmd + 1 : desc->cmd,
                    desc->str ? desc->str : "", VTY_NEWLINE);
#endif /* 0 */
        }

    if((token = token_cr))
    {
        if(!token->desc)
            vty_out(vty, "  %-s%s",
                    token->cmd[0] == '.' ? token->cmd + 1 : token->cmd,
                    VTY_NEWLINE);
        else if(desc_width >= strlen(token->desc))
            vty_out(vty, "  %-*s  %s%s", width,
                    token->cmd[0] == '.' ? token->cmd + 1 : token->cmd,
                    token->desc, VTY_NEWLINE);
        else
        {
            vty_describe_fold(vty, width, desc_width, token);
        }
    }

out:
    cmd_free_strvec(vline);

    if(describe)
    {
        vector_free(describe);
    }

    vty_prompt(vty);
    vty_redraw_line(vty);
}

/* Add current command line to the history buffer. */
void
vty_hist_add(struct vty *vty)
{
    int index;

    if(vty->length == 0 && (vty->type == VTY_TERM || vty->type == VTY_SSH))
    {
        return;
    }

    index = vty->hindex ? vty->hindex - 1 : VTY_MAXHIST - 1;

    /* Ignore the same string as previous one. */
    if(vty->hist[index])
        if(strcmp(vty->buf, vty->hist[index]) == 0)
        {
            vty->hp = vty->hindex;
            return;
        }

    /* Insert history entry. */
    if(vty->hist[vty->hindex])
    {
        XFREE(MTYPE_VTY_HIST, vty->hist[vty->hindex]);
    }

    vty->hist[vty->hindex] = XSTRDUP(MTYPE_VTY_HIST, vty->buf);

    /* History index rotation. */
    vty->hindex++;

    if(vty->hindex == VTY_MAXHIST)
    {
        vty->hindex = 0;
    }

    vty->hp = vty->hindex;
}

/* Get telnet window size. */
static int
vty_telnet_option(struct vty *vty, unsigned char *buf, int nbytes)
{
#ifdef TELNET_OPTION_DEBUG
    int i;

    for(i = 0; i < nbytes; i++)
    {
        switch(buf[i])
        {
            case IAC:
                vty_out(vty, "IAC ");
                break;

            case WILL:
                vty_out(vty, "WILL ");
                break;

            case WONT:
                vty_out(vty, "WONT ");
                break;

            case DO:
                vty_out(vty, "DO ");
                break;

            case DONT:
                vty_out(vty, "DONT ");
                break;

            case SB:
                vty_out(vty, "SB ");
                break;

            case SE:
                vty_out(vty, "SE ");
                break;

            case TELOPT_ECHO:
                vty_out(vty, "TELOPT_ECHO %s", VTY_NEWLINE);
                break;

            case TELOPT_SGA:
                vty_out(vty, "TELOPT_SGA %s", VTY_NEWLINE);
                break;

            case TELOPT_NAWS:
                vty_out(vty, "TELOPT_NAWS %s", VTY_NEWLINE);
                break;

            default:
                vty_out(vty, "%x ", buf[i]);
                break;
        }
    }

    vty_out(vty, "%s", VTY_NEWLINE);

#endif /* TELNET_OPTION_DEBUG */

    switch(buf[0])
    {
        case SB:
            vty->sb_len = 0;
            vty->iac_sb_in_progress = 1;
            return 0;
            break;

        case SE:
        {
            if(!vty->iac_sb_in_progress)
            {
                return 0;
            }

            if((vty->sb_len == 0) || (vty->sb_buf[0] == '\0'))
            {
                vty->iac_sb_in_progress = 0;
                return 0;
            }

            switch(vty->sb_buf[0])
            {
                case TELOPT_NAWS:
                    if(vty->sb_len != TELNET_NAWS_SB_LEN)
                        zlog_warn("RFC 1073 violation detected: telnet NAWS option "
                                  "should send %d characters, but we received %lu",
                                  TELNET_NAWS_SB_LEN, (u_long)vty->sb_len);
                    else if(sizeof(vty->sb_buf) < TELNET_NAWS_SB_LEN)
                        zlog_err("Bug detected: sizeof(vty->sb_buf) %lu < %d, "
                                 "too small to handle the telnet NAWS option",
                                 (u_long)sizeof(vty->sb_buf), TELNET_NAWS_SB_LEN);
                    else
                    {
                        vty->width = ((vty->sb_buf[1] << 8) | vty->sb_buf[2]);
                        vty->height = ((vty->sb_buf[3] << 8) | vty->sb_buf[4]);
#ifdef TELNET_OPTION_DEBUG
                        vty_out(vty, "TELNET NAWS window size negotiation completed: "
                                "width %d, height %d%s",
                                vty->width, vty->height, VTY_NEWLINE);
#endif
                    }

                    break;
            }

            vty->iac_sb_in_progress = 0;
            return 0;
            break;
        }

        default:
            break;
    }

    return 1;
}


void
vty_escape_map(unsigned char c, struct vty *vty)
{
    switch(c)
    {
        case('A'):
            vty_previous_line(vty);
            break;

        case('B'):
            vty_next_line(vty);
            break;

        case('C'):
            vty_forward_char(vty);
            break;

        case('D'):
            vty_backward_char(vty);
            break;

        default:
            break;
    }

    /* Go back to normal mode. */
    vty->escape = VTY_NORMAL;
}

/* #define TELNET_OPTION_DEBUG */

/* Quit print out to the buffer. */
static void
vty_buffer_reset(struct vty *vty)
{
    pthread_mutex_lock(&vty->obuf_lock);
    buffer_reset(vty->obuf);
    pthread_mutex_unlock(&vty->obuf_lock);

    vty_prompt(vty);
    vty_redraw_line(vty);
}

/* allocate and initialise vty */
struct vty *
vty_new_init(void)
{
    struct vty *vty;

    vty = vty_new();
//  vty->fd = vty_sock;
//  vty->wfd = vty_sock;
    vty->type = VTY_TERM;
    vty->node = AUTH_NODE;
    vty->fail = 0;
    vty->cp = 0;
    vty_clear_buf(vty);
    vty->length = 0;
    memset(vty->hist, 0, sizeof(vty->hist));
    vty->hp = 0;
    vty->hindex = 0;
    vector_set(vtyvec, vty);
//  vector_set_index (vtyvec, vty_sock, vty);
    vty->status = VTY_NORMAL;
    vty->lines = -1;
    vty->iac = 0;
    vty->iac_sb_in_progress = 0;
    vty->sb_len = 0;

    return vty;
}


void *
vty_telnet_read(void *arg)
{
    int i;
    int nbytes;
    int err;
    sigset_t oldmask;
    struct buffer_data *data;
    struct buffer *buffer;
//  struct buffer *buffer_tmp;
    unsigned char *buf;

    struct login_session *session = (struct login_session *) arg;
    struct vty *vty = session->vty;
    struct telnet_client *tc = NULL;
    buffer = session->buf_ipc_recv;

    pthread_detach(pthread_self());
    prctl(PR_SET_NAME, " vty_telnet_read");

    if(buffer == NULL)
    {
        return NULL;
    }

    if((err = pthread_sigmask(SIG_BLOCK, &sig_mask, &oldmask)) != 0)
    {
        perror("pthread_sigmask error:");
        exit(1);
    }

    while(1)
    {
        usleep(10000);

        /* Check status. */
        if(session->session_status == SESSION_CLOSE)
        {
            session->tid_r_exit = 1;
//            session->ul->islogin = USER_LINE_LOGIN_OUT;	
            break;
        }

        /*buffer非空且readcp指向的bufferdata非空*/
        if(session->bufdata_cnt == 0 && session->read_bufdata == NULL)
        {
            continue;
        }

        while(session->read_bufdata)
        {
            /* read指向recv正在处理的bufdata，等待recv处理完成*/
            if(session->recv_copy_busy &&   \
                    session->read_bufdata == session->recv_bufdata)
            {
                continue;
            }

            data = session->read_bufdata;
            /* bufferdata length */
            nbytes = data->cp;
            /* data of bufferdata */
            buf = data->data;

            if(session->client == CLIENT) // client role ,send user input data to server
            {
                tc = session->tc;
                telnetc_send_server(tc, buf, nbytes);
            }
            else
            {
            for(i = 0; i < nbytes; i++)
            {
                    if(buf[i] == 'C' - '@')
                    {
                        if(vty->node > AUTH_NODE)
                        {
                            vty->sig_int_recv = 1;
                            vtysh_execute_sigint(vty);
                            vty->sig_int_recv = 0;
                        }  
                        continue;
                    }
                if(buf[i] == IAC)
                {
                    if(!vty->iac)
                    {
                        vty->iac = 1;
                        continue;
                    }
                    else
                    {
                        vty->iac = 0;
                    }
                }

                if(vty->iac_sb_in_progress && !vty->iac)
                {

                    //IAC SB NAWS WIDTH[1] WIDTH[0] HEIGHT[1] HEIGHT[0] IAC SE
                    if(vty->sb_len < sizeof(vty->sb_buf))
                    {
                        vty->sb_buf[vty->sb_len] = buf[i];
                    }

                    vty->sb_len++;
                    continue;
                }

                if(vty->iac)
                {
                    /* In case of telnet command */
                    int ret = 0;

                    ret = vty_telnet_option(vty, buf + i, nbytes - i);
                    vty->iac = 0;
                    i += ret;
                    continue;
                }


                if(vty->status == VTY_MORE)
                {
                    switch(buf[i])
                    {
                        case CONTROL('C'):
                        case 'q':
                        case 'Q':
                            vty_buffer_reset(vty);
                            vty->continue_flash = 1;
                            break;

                        default:
                            //tell flush thread to flush continue
                            vty->continue_flash = 1;
                            break;
                    }

                    continue;
                }

                /* Escape character. */
                if(vty->escape == VTY_ESCAPE)
                {
                    vty_escape_map(buf[i], vty);
                    continue;
                }

                /* Pre-escape status. */
                if(vty->escape == VTY_PRE_ESCAPE)
                {
                    switch(buf[i])
                    {
                        case '[':
                            vty->escape = VTY_ESCAPE;
                            break;

                        case 'b':
                            vty_backward_word(vty);
                            vty->escape = VTY_NORMAL;
                            break;

                        case 'f':
                            vty_forward_word(vty);
                            vty->escape = VTY_NORMAL;
                            break;

                        case 'd':
                            vty_forward_kill_word(vty);
                            vty->escape = VTY_NORMAL;
                            break;

                        case CONTROL('H'):
                        case 0x7f:
                            vty_backward_kill_word(vty);
                            vty->escape = VTY_NORMAL;
                            break;

                        default:
                            vty->escape = VTY_NORMAL;
                            break;
                    }

                    continue;
                }

                switch(buf[i])
                {
                    case CONTROL('A'):
                        vty_beginning_of_line(vty);
                        break;

                    case CONTROL('B'):
                        vty_backward_char(vty);
                        break;

                    case CONTROL('C'):
                        vty_stop_input(vty);
                        break;

                    case CONTROL('D'):
                        vty_delete_char(vty);
                        break;

                    case CONTROL('E'):
                        vty_end_of_line(vty);
                        break;

                    case CONTROL('F'):
                        vty_forward_char(vty);
                        break;

                    case CONTROL('H'):
                    case 0x7f:
                        vty_delete_backward_char(vty);
                        break;

                    case CONTROL('K'):
                        vty_kill_line(vty);
                        break;

                    case CONTROL('N'):
                        vty_next_line(vty);
                        break;

                    case CONTROL('P'):
                        vty_previous_line(vty);
                        break;

                    case CONTROL('T'):
                        vty_transpose_chars(vty);
                        break;

                    case CONTROL('U'):
                        vty_kill_line_from_beginning(vty);
                        break;

                    case CONTROL('W'):
                        vty_backward_kill_word(vty);
                        break;

                    case CONTROL('Z'):
                        vty_end_config(vty);
                        break;

                    case '\r':

                        //  case '\r':
                        if(session->session_status == SESSION_CLOSE)
                        {
                            session->tid_r_exit = 1;
                            break;
                        }

                        vty_out(vty, "%s", VTY_NEWLINE);
                        vty_telnet_execute(vty);

                        /*接收到回车，跳过'\r'后面的'\n'，否则若回车后flush线程置位VTY_MORE，轮询\n时，会导致
                        连刷两页*/
                        if(i < nbytes && buf[i + 1] == '\n')
                        {
                            i++;
                        }

                        break;

                    case '\t':
                        vty_complete_command(vty);
                        break;

                    case '?':
                        if(vty->node == AUTH_NODE)
                        {
                            vty_self_insert(vty, buf[i]);
                        }
                        else
                        {
                            vty_describe_command(vty);
                        }

                        break;

                    case '\033':
                        if(i + 1 < nbytes && buf[i + 1] == '[')
                        {
                            vty->escape = VTY_ESCAPE;
                            i++;
                        }
                        else
                        {
                            vty->escape = VTY_PRE_ESCAPE;
                        }

                        break;

                    default:
                        if(buf[i] > 31 && buf[i] < 127)
                        {
                            vty_self_insert(vty, buf[i]);
                        }

                        break;
                }
            }
            }

            /*unlock session->*/
            pthread_mutex_lock(&session->bufdata_delete_lock);

            if(data->next)
            {
                zlog_debug(VD_TELNET, "##READ:we have something in next\n");
                session->read_bufdata = session->read_bufdata->next;
            }
            else
            {
                zlog_debug(VD_TELNET, "##READ:read up all bufdata\n");
                session->read_bufdata = session->recv_bufdata = NULL;
            }

            buffer_data_delete(buffer, data);
            session->bufdata_cnt--;
            zlog_debug(VD_TELNET, "##READ:after buffer_data_delete bufdata_cnt:%d\n", session->bufdata_cnt);

            pthread_mutex_unlock(&session->bufdata_delete_lock);
        }
    }

    return NULL;
}


void *
vty_telnet_write(void *arg)
{
    int erase;
    buffer_status_t flushrc;
    struct vty *vty;
    uint32_t send_ip;
    uint16_t send_port;
    int err;
    sigset_t oldmask;

	uint16_t vpn = 0;

    struct login_session *session = (struct login_session *) arg;
    vty = session->vty;
    send_ip = session->client_ip;
    send_port = session->client_port;

    pthread_detach(pthread_self());
    prctl(PR_SET_NAME, " vty_telnet_write");

    if((err = pthread_sigmask(SIG_BLOCK, &sig_mask, &oldmask)) != 0)
    {
        perror("pthread_sigmask error:");
        exit(1);
    }

    while(1)
    {
        usleep(10000);
        pthread_mutex_lock(&vty->obuf_lock);

        if(buffer_empty(vty->obuf))
        {
            pthread_mutex_unlock(&vty->obuf_lock);

            if(session->session_status == SESSION_CLOSE)
            {
                session->tid_w_exit = 1;
                break;
            }

            continue;
        }

        pthread_mutex_unlock(&vty->obuf_lock);

        if(vty->status == VTY_MORE)
        {
            if(session->tid_r_exit)
            {
                zlog_debug(VD_TELNET, "read thread has exit, write break here\n");
                session->tid_w_exit = 1;
                break;
            }

            if(vty->continue_flash == 0)
            {
                /*读线程在处理CMD_CONTINUE回显时固定回显60行，此时写线程more分页不再阻塞等待回车键，直接flush*/
                if(vty->flush_cp)
                {
                    vty->continue_flash = 0;
                }
                else
                {
                    continue;
                }
            }
            else if(vty->continue_flash == 1)
            {
                zlog_debug(VD_TELNET, "#WRITE:vty_tel_flush more status and continue_flash\n");
                vty->continue_flash = 0;
            }
        }

        /* Function execution continue. */
        erase = ((vty->status == VTY_MORE || vty->status == VTY_MORELINE));

        pthread_mutex_lock(&vty->obuf_lock);
#if 0

        /* N.B. if width is 0, that means we don't know the window size. */
        if((vty->lines == 0) || (vty->width == 0) || (vty->height == 0))
        {
            flushrc = buffer_flush_available_ipc(vty->obuf, send_ip, send_port);
        }
        else if(vty->status == VTY_MORELINE)
            flushrc = buffer_flush_window_ipc(vty->obuf, send_ip, send_port, vty->width,
                                              1, erase, 0);
        else
#endif
		vpn = vtysh_vpn_get();
        flushrc = buffer_flush_window_ipc(vty->obuf, send_ip, send_port, vpn, vty->width,
                                          vty->lines >= 0 ? vty->lines :
                                          vty->height,
                                          erase, 0);

        pthread_mutex_unlock(&vty->obuf_lock);

        switch(flushrc)
        {
            case BUFFER_ERROR:
                vty->monitor = 0; /* disable monitoring to avoid infinite recursion */
                zlog_warn("#WRITE:buffer_flush failed on vty client fd %d, closing",
                          vty->fd);
                pthread_mutex_lock(&vty->obuf_lock);
                buffer_reset(vty->obuf);
                pthread_mutex_unlock(&vty->obuf_lock);
                session->session_status = SESSION_CLOSE;
//              vty->status == VTY_CLOSE;
                return 0;

            case BUFFER_EMPTY:
                if(session->session_status == SESSION_CLOSE)
                {
                    break;
                }
//                  vty_close (vty);
                else
                {
                    vty->status = VTY_NORMAL;
                }

                break;

            case BUFFER_PENDING:
                /* There is more data waiting to be written. */
                vty->status = VTY_MORE;
                break;
        }

        if(session->session_status == SESSION_CLOSE)
        {
            zlog_debug(VD_TELNET, "write thread exit\n");
            session->tid_w_exit = 1;
            break;
        }
    }

    return NULL;
}

struct vty *
vty_telnet_create(struct login_session *session)
{
    struct vty *vty;
    vty = vty_new_init();

    memset(vty->ttyname, 0, 20);
    strcpy(vty->ttyname, session->address);

	vty->client_ip = session->client_ip;
	vty->client_port = session->client_port;
	vty->server_ip = session->server_ip;
	vty->server_port = session->server_port;

	memset(vty->hostname, 0, sizeof(vty->hostname));
	
	memcpy(vty->hostname, session->device_name, strlen(session->device_name));

    vty->session = session;
    vty->v_timeout = USER_IDLE_COUNTER;//add out timer

    if(pthread_mutex_init(&vty->obuf_lock, NULL) != 0)
    {
        perror("obuf_lock init failed\n");
        exit(1);
    }

    if(host.lines >= 0)
    {
        vty->lines = host.lines;
    }

    if(pthread_mutex_init(&vty->catch_buf_lock, NULL) != 0)
    {
        perror("snmp catch_buf_lock init failed\n");
        exit(1);
    }

    vty_will_echo(vty);
    vty_will_suppress_go_ahead(vty);
    vty_dont_linemode(vty);
    vty_do_window_size(vty);
    /* Say hello to the world. */
    vty_hello(vty);

    for(int i = 0; i < DAEMON_MAX; i++)
    {
        vty->connect_fd[i] = -1;
    }

    vty_out(vty, "%s%% User Access Verification%s%s", VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE);

    /* Setting up terminal. */

    /* vty_dont_lflow_ahead (vty); */

	if((session->ul->auth_mode == AUTH_MODE_SCHEME) && (host.login_cnt <= USER_LOGIN_MAX))
    {
		vty_out(vty, "Username:");
	}
	else if(session->ul->auth_mode == AUTH_MODE_PASSWORD)
    {
		vty_out(vty, "Password:");
    }

    /* Do not connect until we have passed authentication. */
    if(vtysh_connect_all(vty) <= 0)
    {
        fprintf(stderr, "%s[%d] : Exiting: failed to connect to any daemons.\n", __func__, __LINE__);
        exit(1);
    }

#if 0
    pthread_mutex_lock(&session_timer_lock);

    if(vty->v_timeout)
    {
        //vty->t_timeout = thread_add_timer(vtysh_master, vty_timeout, vty, vty->v_timeout);
    }

    pthread_mutex_unlock(&session_timer_lock);
#endif

    /*sync vty if master is at realtime_sync status*/
    if(syncvty.vty_slot == VTY_MAIN_SLOT
            && syncvty.self_status == VTYSH_REALTIME_SYNC_ENABLE)
    {
        vtysh_master_sync_vty(vty, VTY_SYNC_CREAT, NULL);
    }

    return vty;

}

struct login_session *
vty_telnet_thread_creat_connect(uint32_t client_ip, uint16_t client_port, uint32_t server_ip)
{
    struct login_session *session;
    uint32_t sip, dip;
    uint16_t sport;
//  int buf_len;
    pthread_attr_t attr;
    int err;

    if(vtysh_telnet_enable_get() != TELNET_SERVER_ENABLE)
    {
		printf("\n Telnet server is not enable!\n");
		return NULL;
    }    

    zlog_debug(VD_TELNET, "vty_telnet_thread_creat_connect\n");
    sip = client_ip;
    sport = client_port;
	dip = server_ip;

    //锟斤拷锟斤拷锟结话锟斤拷锟斤拷锟斤拷痈锟斤拷没锟斤拷亩锟叫达拷叱锟?
    session = vty_telnet_session_creat(sip, sport, dip);

    /*Creat write thread*/
    err = pthread_attr_init(&attr);

    if(err != 0)
    {
        perror("pthread_attr_init fail:");
        exit(-1);
    }

    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    if(err == 0)
    {
        err = pthread_create(&(session->tid_w), NULL, vty_telnet_write, (void *) session);
    }

    if(err != 0)
    {
        perror("write pthread_create fail:");
        exit(-1);
    }

    pthread_attr_destroy(&attr);

    /*Creat read thread*/
    err = pthread_attr_init(&attr);

    if(err != 0)
    {
        perror("can't creat thread:");
        exit(-1);
    }

    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    if(err == 0)
    {
        err = pthread_create(&(session->tid_r), NULL, vty_telnet_read, (void *) session);
    }

    if(err != 0)
    {
        perror("read pthread_create fail:");
        exit(-1);
    }

    pthread_attr_destroy(&attr);

    return session;

}

void
vty_term_close(struct vty *vty)
{
    unsigned int i;
    struct login_session *session, *s;
    struct vty *v;
	
	uint16_t vpn = vtysh_vpn_get();

    if(vty == NULL)
    {
        return;
    }

    session = (struct login_session *)vty->session;

    /* Cancel threads.*/
    if(vty->t_read)
    {
        thread_cancel(vty->t_read);
		vty->t_read = NULL;
    }

    if(vty->t_write)
    {
        thread_cancel(vty->t_write);
		vty->t_write = NULL;
    }

    if(vty->t_timeout)
    {
        //thread_cancel(vty->t_timeout);
        high_pre_timer_delete(vty->t_timeout);
        vty->t_timeout = 0;
    }

    if(vty->type == VTY_TERM)
    {
        vty_out(vty, "%s%% Log out from terminal%s%s", VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE);
        /* Flush buffer. */
        buffer_flush_all_ipc(vty->obuf, session->client_ip, session->client_port, vpn);
    }

    /* Free input buffer. */
    if(vty->obuf)
    {
        buffer_free(vty->obuf);
		vty->obuf = NULL;
    }

    for(i = 0; i < DAEMON_MAX; i++)
    {
        if(vty->connect_fd[i] != -1)
        {
            close(vty->connect_fd[i]);
        }
    }

    /* Free command history. */
    for(i = 0; i < VTY_MAXHIST; i++)
        if(vty->hist[i])
        {
            XFREE(MTYPE_VTY_HIST, vty->hist[i]);
			vty->hist[i] = NULL;
        }

    for(i = 0; i < vector_active(vtyvec); i++)
    {
        if((v = vector_slot(vtyvec, i)) != NULL && (v->type == VTY_TERM || v->type == VTY_SSH))
        {
            s = (struct login_session *)v->session;

            if(s->client_ip == session->client_ip && s->client_port == session->client_port)
            {
                vector_unset(vtyvec, i);
            }
        }
    }

    /* Close socket. */
    if(vty->fd > 0)
    {
        close(vty->fd);
    }
    else
    {
        vty_stdio_reset();
    }

//  sleep(3);
    if(vty->type == VTY_TERM)
    {
        vty_tcp_close(session->client_ip, session->client_port, TCP_PORT_TELNET);
    }
    else
    {
        vty_tcp_close(session->client_ip, session->client_port, TCP_PORT_SSH);
    }

    if(vty->buf)
    {
        XFREE(MTYPE_VTY, vty->buf);
		vty->buf = NULL;
    }

    if(vty->buf_tmp)
    {
        XFREE(MTYPE_VTY, vty->buf_tmp);
		vty->buf_tmp = NULL;
    }

    /* Check configure. */
//  vty_config_unlock (vty);
    pthread_mutex_destroy(&vty->obuf_lock);

    /* OK free vty and connect */
    XFREE(MTYPE_VTY, vty);

}

void
vty_consol_close(struct login_session *session)
{
    struct vty *vty;

    vty = session->vty;

    if(session->session_type != SESSION_CONSOLE)
    {
        return;
    }

    vty_out(vty, "user logout from consol%s%s", VTY_NEWLINE, VTY_NEWLINE);
    vty_out(vty, "%sUser Access Verification%s%s", VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE);

    vtysh_user_login_trap(session, 0);
    session->ul->idle_cnt_en = USER_LINE_IDLE_DIS;
    session->ul->islogin = USER_LINE_LOGIN_OUT;	
    
    if(session->user_name)
    {
        free(session->user_name);
        session->user_name = NULL;
    }

    /*cancle timer*/
    if(vty->t_timeout)
    {
        pthread_mutex_lock(&session_timer_lock);
        //thread_cancel(vty->t_timeout);
		high_pre_timer_delete(vty->t_timeout);
        vty->t_timeout = 0;
        pthread_mutex_unlock(&session_timer_lock);
    }

    /*reset consol timer counter*/
    session->timer_count = 0;
    
    vtysh_line_config_update(session);    
}

void *
vty_ssh_cmd(void *arg)
{
    Channel *c = (Channel *)arg;
    struct buffer_data *data;
    int nbytes, i;
    unsigned char *buf;
    struct vty *vty = c->vty;
    struct buffer *buffer = c->buf_ipc_recv;

	pthread_detach(pthread_self());
    prctl(PR_SET_NAME, " vty_ssh_cmd");

//  printf("vty_ssh_cmd\n");
    while(1)
    {
        usleep(100000);

        if(c->state == SSH_CHANNEL_CLOSED)
        {
            c->channel_exit_flag = 1;
            break;
        }

        /*buffer非空且readcp指向的bufferdata非空*/
        if(c->bufdata_cnt == 0 && c->read_bufdata == NULL)
        {
            continue;
        }

        while(c->read_bufdata)
        {
//          printf("##READ while(data && data->cp), cp:%d\n",session->read_bufdata->cp);

            /* read指向recv正在处理的bufdata，等待recv处理完成*/
            if(c->recv_copy_busy && \
                    c->read_bufdata == c->recv_bufdata)
            {
                continue;
            }

            data = c->read_bufdata;
            /* bufferdata length */
            nbytes = data->cp;
            /* data of bufferdata */
            buf = data->data;

            zlog_debug(VD_TELNET, "vty_ssh_cmd:nbytes:%d\n", nbytes);

            for(i = 0; i < nbytes; i++)
            {
                if(vty->status == VTY_MORE)
                {
                    switch(buf[i])
                    {
                        case CONTROL('C'):
                        case 'q':
                        case 'Q':
                            vty_buffer_reset(vty);
                            vty->continue_flash = 1;
                            break;

                        default:
                            //tell flush thread to flush continue
                            vty->continue_flash = 1;
                            break;
                    }

                    continue;
                }

                /* Escape character. */
                if(vty->escape == VTY_ESCAPE)
                {
                    vty_escape_map(buf[i], vty);
                    continue;
                }

                /* Pre-escape status. */
                if(vty->escape == VTY_PRE_ESCAPE)
                {
                    switch(buf[i])
                    {
                        case '[':
                            vty->escape = VTY_ESCAPE;
                            break;

                        case 'b':
                            vty_backward_word(vty);
                            vty->escape = VTY_NORMAL;
                            break;

                        case 'f':
                            vty_forward_word(vty);
                            vty->escape = VTY_NORMAL;
                            break;

                        case 'd':
                            vty_forward_kill_word(vty);
                            vty->escape = VTY_NORMAL;
                            break;

                        case CONTROL('H'):
                        case 0x7f:
                            vty_backward_kill_word(vty);
                            vty->escape = VTY_NORMAL;
                            break;

                        default:
                            vty->escape = VTY_NORMAL;
                            break;
                    }

                    continue;
                }

                switch(buf[i])
                {
                    case CONTROL('A'):
                        vty_beginning_of_line(vty);
                        break;

                    case CONTROL('B'):
                        vty_backward_char(vty);
                        break;

                    case CONTROL('C'):
                        vty_stop_input(vty);
                        break;

                    case CONTROL('D'):
                        vty_delete_char(vty);
                        break;

                    case CONTROL('E'):
                        vty_end_of_line(vty);
                        break;

                    case CONTROL('F'):
                        vty_forward_char(vty);
                        break;

                    case CONTROL('H'):
                    case 0x7f:
                        vty_delete_backward_char(vty);
                        break;

                    case CONTROL('K'):
                        vty_kill_line(vty);
                        break;

                    case CONTROL('N'):
                        vty_next_line(vty);
                        break;

                    case CONTROL('P'):
                        vty_previous_line(vty);
                        break;

                    case CONTROL('T'):
                        vty_transpose_chars(vty);
                        break;

                    case CONTROL('U'):
                        vty_kill_line_from_beginning(vty);
                        break;

                    case CONTROL('W'):
                        vty_backward_kill_word(vty);
                        break;

                    case CONTROL('Z'):
                        vty_end_config(vty);
                        break;

                    case '\r':
                    case '\n':
                        vty_out(vty, "%s", VTY_NEWLINE);
                        vty_telnet_execute(vty);

                        /*接收到回车，跳过'\r'后面的'\n'，否则若回车后flush线程置位VTY_MORE，轮询\n时，会导致
                        连刷两页*/
                        if(i < nbytes && buf[i + 1] == '\n')
                        {
                            i++;
                        }

                        break;

                    case '\t':
                        vty_complete_command(vty);
                        break;

                    case '?':
                        if(vty->node == AUTH_NODE)
                        {
                            vty_self_insert(vty, buf[i]);
                        }
                        else
                        {
                            vty_describe_command(vty);
                        }

                        break;

                    case '\033':
                        if(i + 1 < nbytes && buf[i + 1] == '[')
                        {
                            vty->escape = VTY_ESCAPE;
                            i++;
                        }
                        else
                        {
                            vty->escape = VTY_PRE_ESCAPE;
                        }

                        break;

                    default:
                        if(buf[i] > 31 && buf[i] < 127)
                        {
                            vty_self_insert(vty, buf[i]);
                        }

                        break;
                }
            }

            /*unlock session->*/
            pthread_mutex_lock(&c->bufdata_delete_lock);
            zlog_debug(VD_TELNET, "##READ:lock bufdata_delete_lock\n");

            /*下一个bufferdata中有数据要读? session->read_bufdata指向下一个bufferdata*/
            if(data->next)
            {
                zlog_debug(VD_TELNET, "##READ:we have something in next\n");
                c->read_bufdata = c->read_bufdata->next;
            }
            else //已读完所有bufferdata
            {
                zlog_debug(VD_TELNET, "##READ:read up all bufdata\n");
                c->read_bufdata = c->recv_bufdata = NULL;
            }

            buffer_data_delete(buffer, data);
            c->bufdata_cnt--;
            zlog_debug(VD_TELNET, "##READ:after buffer_data_delete bufdata_cnt:%d\n", c->bufdata_cnt);

            pthread_mutex_unlock(&c->bufdata_delete_lock);
            zlog_debug(VD_TELNET, "##READ:unlock bufdata_delete_lock\n");
        }
    }

    return NULL;
}

void
vty_sync_close(struct vty *vty)
{
    unsigned int i;
    struct vty *v;

    zlog_debug(VD_TELNET, "%s[%d]%s\n", __FILE__, __LINE__, __func__);
    assert(vty != NULL);

    /* Free input buffer. */
    if(vty->obuf)
    {
        buffer_free(vty->obuf);
    }

    for(int i = 0; i < DAEMON_MAX; i++)
    {
        if(vty->connect_fd[i] != -1)
        {
            close(vty->connect_fd[i]);
        }
    }

    /* Free command history. */
    for(i = 0; i < VTY_MAXHIST; i++)
        if(vty->hist[i])
        {
            XFREE(MTYPE_VTY_HIST, vty->hist[i]);
        }

    for(i = 0; i < vector_active(vtyvec_sync); i++)
    {
        if((v = vector_slot(vtyvec_sync, i)) != NULL && (v->type == VTY_SYNC))
        {
            if(strcmp(vty->ttyname, v->ttyname) == 0)
            {
                vector_unset(vtyvec_sync, i);
            }
        }
    }

    /* Close socket. */
    if(vty->fd > 0)
    {
        close(vty->fd);
    }
    else
    {
        vty_stdio_reset();
    }

    if(vty->buf)
    {
        XFREE(MTYPE_VTY, vty->buf);
    }

    if(vty->buf_tmp)
    {
        XFREE(MTYPE_VTY, vty->buf_tmp);
    }

    /* Check configure. */
    pthread_mutex_destroy(&vty->obuf_lock);

    /* OK free vty and connect */
    XFREE(MTYPE_VTY, vty);

}


