

#include <zebra.h>
#include <lib/memory.h>
#include <pthread.h>
#include <termios.h>
#include "ftm/pkt_ip.h"
#include "vtysh.h"
#include "vtysh_telnet_client.h"
#include "vtysh_telnet_client_cmd.h"
#include "vtysh.h"


#define IAC         255  /* interpret as command: */
#define DONT        254  /* you are not to use option */
#define DO          253  /* please, you use option */
#define WONT        252  /* I won't use option */
#define WILL        251  /* I will use option */
#define SB          250  /* interpret as subnegotiation */
#define SE          240  /* end sub negotiation */

#define TELOPT_ECHO   1  /* echo */
#define TELOPT_SGA    3  /* suppress go ahead */
#define TELOPT_TTYPE 24  /* terminal type */
#define TELOPT_NAWS  31  /* window size */


extern struct thread_master *vtysh_master;

struct telnet_client telnetc[TELNET_CLIENT_MAX];


void telnetc_init(void)
{
    memset(telnetc, 0, sizeof(telnetc));

    telnetc_cmd_reg();
}

struct telnet_client *telnet_data_get(int32_t cl)
    {
    if(cl >= TELNET_CLIENT_MAX)  
        {
    
        return NULL;
}
    else
{
        return &telnetc[cl];
    }
}

/*

*/
static struct telnet_client *telnetc_idle_get(void)
{
    int i = 0;
    struct telnet_client *tc = NULL;

    for(i = 0; i < TELNET_CLIENT_MAX; i++)
    {
        tc = telnet_data_get(i);
        if(tc == NULL)
        {
            break;
        }
        if(tc->state == CLIENT_IDLE)
        {
            VTY_DEBUG(VD_CLIENT, "get idle index %d", i);
            return tc;
        }
    }

    return NULL;
}

/*

*/
static void telnetc_data_clear(struct telnet_client *tc)
{
    struct login_session *session = NULL;
    
    if(tc == NULL)
    {
        return;
    }
    
    VTY_DEBUG(VD_CLIENT, "sport %d", tc->sport); 

    session = tc->ss;
    session->client = SERVER;    

    if(tc->recv_buf)
    {
        XFREE(MTYPE_VTY, tc->recv_buf);
    }

    memset(tc, 0, sizeof(struct telnet_client));  
    tc->state = CLIENT_IDLE;
    tc->server_state = SERVER_NONE;
    tc->connect = TELNET_SERVER_START; 
    
}

/*

*/
struct telnet_client *telnetc_match(uint32_t sip, uint32_t dip, uint32_t sport)
{
    int i = 0;
    struct telnet_client *tc = NULL;
    
    for(i = 0; i < TELNET_CLIENT_MAX; i++)
    {      
        tc = telnet_data_get(i);
        if(tc == NULL)
        {
            break;
        }        
        if(tc->dip == dip && tc->sport == sport) 
        {
            return tc;
        }
    }

    VTY_DEBUG(VD_CLIENT, "no matched sip %x, dip %x, sport %d", sip, dip, sport);
    
    return NULL;   
}


/*
send packet to FTM
*/
static int32_t telnetc_pkt_send(uint32_t sip, uint32_t sport, uint32_t dip, uint32_t dport, uint16_t vpn, const uint8_t *buf, uint32_t len)
{
    union pkt_control pkt_ctrl;
    int ret = -1;
    
    memset(&pkt_ctrl, 0, sizeof(pkt_ctrl));
    pkt_ctrl.ipcb.dip = dip;
    pkt_ctrl.ipcb.sip = sip;
    pkt_ctrl.ipcb.protocol = IP_P_TCP;
    pkt_ctrl.ipcb.dport = dport;
    pkt_ctrl.ipcb.sport = sport;
    
    pkt_ctrl.ipcb.ttl   = 64;
    pkt_ctrl.ipcb.is_changed = 1;
    pkt_ctrl.ipcb.tos = 4;
    pkt_ctrl.ipcb.vpn = vpn;
    VTY_DEBUG(VD_CLIENT, "sip %x, sport %d, dip %x, dport %d, vpn %d; ptk len %d ", sip, sport,
                        dip, dport, vpn, len);

    ret = pkt_send(PKT_TYPE_TCP, &pkt_ctrl, (void *)buf, len);
    if(ret != 0)
    {
        zlog_err("pkt_send failed!\n");
    }
    
    return ret;
}


/*
telnet protocol command send
*/
static int32_t telnetc_cmd_send(struct telnet_client *tc, uint8_t cmd, uint8_t opt)
{
    uint8_t cmdbuf[3] = {0};
    uint32_t len = 0;
    int ret = -1;
    
    len = 3;
    cmdbuf[0] = IAC;
    cmdbuf[1] = cmd;
    cmdbuf[2] = opt;

    ret = telnetc_pkt_send(tc->sip, tc->sport, tc->dip, tc->dport, tc->vpn, cmdbuf, len);

    return ret;
}

/*
telnet protocol command parse
*/
static int32_t telnetc_telnet_cmd_parse(struct telnet_client *tc, const uint8_t *buf, uint32_t len)
{
    uint8_t ch = 0;
    uint8_t cmd = 0;
    uint8_t opt = 0;
    uint32_t cmdlen = 0;
          
    if(tc == NULL || buf == NULL)
    {
        return -1;
    } 

    ch = *buf;
    while(ch == IAC)  /* IAC 应该不会在报文中间 */
    {
        cmd = *(buf + 1);
        opt = *(buf + 2);

        VTY_DEBUG(VD_CLIENT, "cmd %d, opt %d", cmd, opt);
        if(cmd != SB && cmd != SE)
        {
            /* DO DONT WILL WONT  cmd*/
            
            /* 对端想让我执行的选项，我都不同意*/
            if(cmd == DO || cmd == DONT)
            {
                telnetc_cmd_send(tc, WONT, opt);
            }
            else if(cmd == WILL)
            {
                if(opt == TELOPT_ECHO) /* 对方（server） 回显，本端同意*/
                {
                    telnetc_cmd_send(tc, DO, TELOPT_ECHO);
                }
            }
            else if(cmd == WONT)    /* 对方禁止的选项，我都同意*/
            {
                telnetc_cmd_send(tc, DONT, opt);
            }
            
            buf = buf + 3;         
            cmdlen += 3;
        }
        else if(cmd == SB) /* 子选项 都不回应*/
        {
            buf = buf + 4;
            cmdlen += 4;
        }
        else if(cmd == SE)
        {
            buf = buf + 2;
            cmdlen += 2;            
        }

        if(cmdlen < len)
        {
            ch = *buf;
        }
        else    /* len 长度的信息已分析完*/
        {
            break;
        }
        
    }

    VTY_DEBUG(VD_CLIENT, "telcmd len %d", cmdlen);
    return cmdlen;
}

/*

*/
static int32_t telnetc_console_input(uint8_t *buf, uint32_t len)
{
    fd_set fdset;
    struct timeval tv;
    uint32_t num = 0;

    if(buf == NULL)
    {
        return -1;
    }

    FD_ZERO(&fdset);
    FD_SET(STDIN_FILENO, &fdset);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    if(select(STDIN_FILENO + 1, &fdset, NULL, NULL, &tv) > 0) 
    {
        num = read(STDIN_FILENO, (void *)buf, len);     
        VTY_DEBUG(VD_CLIENT, "len %d", num);
    }
    
    if(((num == 3) && (buf[0] == 0x1b) && (buf[1] == 0x4f)) || 
       ((num == 5) && (buf[0] == 0x1b) && (buf[1] == 0x5b)))
    {
        num = 0;
    }
    
    return num;
}


/*
某些情况下连接中断时，本地输入时user_input递增，因为无server的返回from_server 不变，
由此作为判断依据。
*/
static uint32_t telnetc_connect_is_interrupt(struct telnet_client *tc)
{
    struct login_session *session = NULL;

    session = tc->ss;
    if(session == NULL)
    {
        return 0;
    }

    if(tc->from_server > 0xfffffff0)
    {
        tc->from_server = 0;
        tc->user_input = 0;
    }

        /* from_server - 1 是因为在输入时判断，这个输入回车还没返回*/
    if(tc->user_input > tc->from_server + 1 && tc->from_server > 0)
    {
        zlog_err("connect interrupt:input %d, fsever %d \n", tc->user_input, tc->from_server + 1); 
        session->client = SERVER;
        tc->connect = TELNET_SERVER_FAIL;
        if(session->session_type == SESSION_TELNET)
        {
            telnetc_data_clear(tc);
        }    
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
*/
int32_t telnetc_send_server(struct telnet_client *tc, uint8_t *buf, uint32_t len)
{
    int ret = 0;
    uint8_t outbuf[CMD_BUF_LEN] = {0};
    uint8_t *dst = outbuf;
    uint8_t *bufend = buf + len;
    uint8_t c = 0;
    
    if(tc == NULL || buf == NULL)
    {
        return -1;
    }

    while(buf < bufend)
    {
        c = *buf++;

        if(c == '\r' || c == '\n') 
        {
            *dst = '\r';
            dst++;
            *dst = '\n';  

            tc->user_input++;
        }
        else
        {
            *dst = c;
        }

        dst++;
    }

    VTY_DEBUG(VD_CLIENT, "len %d\n",dst - outbuf);  

    ret = telnetc_pkt_send(tc->sip, tc->sport, tc->dip, tc->dport, tc->vpn, outbuf, dst - outbuf);
    
    return ret;
}


/*
telnet cmd
*/
static int32_t telnetc_connect_init(struct telnet_client *tc)
{
    int ret = -1;

    ret = telnetc_cmd_send(tc, WONT, TELOPT_ECHO);

    return ret;
}

/*

*/
int32_t telnetc_send_ctrlc(struct telnet_client *tc)
{
    uint8_t ctrlc = 0x3;  
    uint32_t len = 1;
    int ret = 0;

    ret = telnetc_send_server(tc, &ctrlc, len);

    return ret;
}

/*
send data to console terminal
*/
static int32_t telnetc_send_console_terminal(struct telnet_client *tc, const uint8_t *buf, uint32_t len)
{
    uint32_t buflen = 0;
    
    if(tc == NULL)
    {
        return -1;
    } 

    buflen = len > SERVER_PKT_LEN? SERVER_PKT_LEN : len;
    while(tc->len != 0) 
    {
        usleep(100);
    }
    memset(tc->recv_buf, 0, SERVER_PKT_LEN);
    memcpy(tc->recv_buf, buf, buflen);
    tc->len = buflen;    
    VTY_DEBUG(VD_CLIENT, "len %d",len); 
    
    return 0;
}


/*
act as server, and send data to client to display
*/
int32_t telnetc_recv_from_server(struct login_session *session, const uint8_t *buf, uint32_t len)
{
    int ret = -1;
    struct telnet_client *tc;
    uint32_t cmdlen = 0;
    uint32_t buflen = 0;

    if(session == NULL || buf == NULL)
    {
        return -1;
    }

    tc = session->tc;
    if(tc == NULL)
    {
        return -1;
    }

/* server返回，有回车记录*/
    if(buf[0] == '\r')
    {
        tc->from_server++;
    } 

    cmdlen = telnetc_telnet_cmd_parse(tc, buf, len);
    
/* 跳过telnet 协议命令字*/
    buf = buf + cmdlen;
    buflen = len - cmdlen;
        
    tc->connect = TELNET_SERVER_SUCCESS;
    if(session->session_type == SESSION_CONSOLE)
    {        
        ret = telnetc_send_console_terminal(tc, buf, buflen);   
    }
    else if(session->session_type == SESSION_TELNET)
    {
        /*ret = telnetc_pkt_send(session->server_ip, session->server_port, session->client_ip, 
                                    session->client_port, tc->vpn, buf, buflen);*/
		ret = telnetc_pkt_send(session->server_ip, session->server_port, session->client_ip, 
                                    session->client_port, vtysh_vpn_get(), buf, buflen);
    }

    if(ret != 0)
    {
        zlog_err("recv form server err, ret %d \n", ret);
    }
    
    return ret;
}



/*
telnet登录情况下再作为client 登录到server时，telnet退出， 同时将client退出
*/
void telnetc_server_exit(uint32_t sip, uint32_t dip, uint32_t sport)
{
    struct telnet_client *tc = NULL;
    struct login_session *session = NULL;
    
    tc = telnetc_match(sip, dip, sport);    
    if(tc == NULL)
    {
        return ;
    }
    
    tc->server_state = SERVER_CONNECT_EXIT;
    session = tc->ss;
    if(session == NULL)
    {
        return ;
    }
    
    VTY_DEBUG(VD_CLIENT, "sip %x, dip %x, sport %d", sip, dip, sport); 

    session->client = SERVER;

    if(session->session_type == SESSION_TELNET)
    {
        telnetc_data_clear(tc);
    }
    
}

/*
串口处理线程，telnet的在原有telnet server 里处理
*/
static void telnetc_thread_console(void *arg)
{
    uint8_t cmdbuf[CMD_BUF_LEN];
    int32_t len = 0;
    struct telnet_client *tc = NULL;
    struct vty *vty;
    struct termios term_new, term_init;
    int ret = -1;


    prctl(PR_SET_NAME, "telnet client");

    /* 自己不回显*/
    system("stty -echo");
    
    if(tcgetattr(STDIN_FILENO , &term_init) == -1)
    {
        perror("Cannot get standard input description");
        exit(1);
    }
    term_new = term_init;
    term_new.c_lflag &= ~(ICANON | ECHO); /* 禁止行缓冲*/
    term_new.c_cc[VMIN] = 1;
    term_new.c_cc[VTIME] = 0;
    ret = tcsetattr(STDIN_FILENO, TCSAFLUSH, &term_new);    
    
    tc = (struct telnet_client *)arg;
    vty = tc->ss->vty;

    while(1)
    {
        usleep(10000);
        if(tc->server_state == SERVER_CONNECT_EXIT)
        {            
            break;           
        }

        if(tc->connect == TELNET_SERVER_FAIL)
        {
            break;
        }

        if(vty->sig_int_recv == 1)
        {
            telnetc_send_ctrlc(tc);
            vty->sig_int_recv = 0;
        }

        len = telnetc_console_input(cmdbuf, CMD_BUF_LEN);
        if(len > 0)
        {
            telnetc_send_server(tc, cmdbuf, (uint32_t)len);
        }
       
        if(tc->len != 0)
        {
            VTY_DEBUG(VD_CLIENT, "display: len %d", tc->len); 
            write(STDOUT_FILENO, tc->recv_buf, tc->len);
            fflush(stdout);
            tc->len = 0;           
        }  
    }

    ret = tcsetattr(STDIN_FILENO, TCSAFLUSH, &term_init);
    VTY_DEBUG(VD_CLIENT, "tcsetattr ret %d\n", ret);
    system("stty echo");

    usleep(100000);
    printf("\r\n");
    telnetc_data_clear(tc);
    
    return ;
}

/*
定时时间到，检查是否与对端连接成功，即定时时间内是否已收到对端发来数据
没收到，相应状态清0
*/
static int32_t telnetc_connect_check(struct thread *thread)
{
    struct telnet_client *tc = NULL;
    struct login_session *session = NULL;
    struct vty *vty = NULL;

    tc = THREAD_ARG (thread);
    session = tc->ss;
    vty = session->vty;
    
    if(tc->connect == TELNET_SERVER_START)
    {
        tc->connect = TELNET_SERVER_FAIL;
        vty_out(vty, "Telnet failed !%s", VTY_NEWLINE);

        session->client = SERVER;
        if(session->session_type == SESSION_TELNET)
        {
            telnetc_data_clear(tc);
        }      
    }

    return 0;
}

/*

*/
int32_t telnetc_telnet_host(struct vty *vty, uint32_t sip, uint32_t dip, uint32_t dport, uint16_t vpn)
{
    struct telnet_client *tc = NULL;
    int ret = 0;
    struct login_session *session = NULL;
    pthread_t thread_id;
    pthread_attr_t attr;
    
    tc = telnetc_idle_get();
    if(tc == NULL)
    {
        vty_out(vty, "Reach max client %s", VTY_NEWLINE);
        return -1;
    }
    tc->dip = dip;
    tc->sip = sip;
    tc->dport = dport;

    tc->sport = pkt_port_alloc(MODULE_ID_VTY, PKT_TYPE_TCP);
    VTY_DEBUG(VD_CLIENT, "assigned sport %d", tc->sport); 
    tc->vpn = vpn;
    session = vty->session;
    tc->ss = session;

    if(session->session_type == SESSION_CONSOLE)
    {
        tc->recv_buf = (uint8_t *)XCALLOC(MTYPE_VTY, SERVER_PKT_LEN);
    }

    session->client = CLIENT; //act as server and client  

    ret = telnetc_connect_init(tc);
    if(ret != 0)
    {
        telnetc_data_clear(tc);
        
        return ret;        
    }

    tc->state = CLIENT_OCCUPY;
    session->tc = tc;  

    //thread_add_timer(vtysh_master, telnetc_connect_check, tc, TELENT_CONNECT_WAIT_TIME);     
	high_pre_timer_add("vtyshTelnetConnCheckTimer", LIB_TIMER_TYPE_NOLOOP, telnetc_connect_check, tc, TELENT_CONNECT_WAIT_TIME);
	
    if(vty->type == VTY_SHELL)
    {
        ret = pthread_attr_init(&attr);
        if(ret != 0)
        {
            zlog_err("pthread init failed !\n");
            telnetc_data_clear(tc);
            
            return ret;
        }

        ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        if(ret == 0)
        {
            ret = pthread_create(&thread_id, NULL, (void *)telnetc_thread_console, (void *)tc);
        }
        if(ret != 0)
        {
            zlog_err("telnetc_thread_console failed !\n");
            telnetc_data_clear(tc);
            
            return ret;
        }
        
        pthread_attr_destroy(&attr);         
    }

    return 0;
}

int32_t telnelc_debug_console_exit(void)
{
    int i = 0;
    struct telnet_client *tc;
    struct login_session *session = NULL;
    int ret = -1;
    for(i = 0; i < TELNET_CLIENT_MAX; i++)
    {  
        tc = telnet_data_get(i);
        if(tc == NULL)
        {
            break;
        }
        if(tc->recv_buf)
        {
            tc->connect = TELNET_SERVER_FAIL;
            ret = 0;
            session = tc->ss;
            session->client = SERVER;
            break;
        }
    }
    return ret;
}
