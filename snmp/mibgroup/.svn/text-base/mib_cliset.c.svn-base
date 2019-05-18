/*
 *  cli command MIB group implementation - ipran_cli_set.c
 *
 */
/* Portions of this file are subject to the following copyright(s).  See
 * the Net-SNMP's COPYING file for more details and other copyrights
 * that may apply:
 */
/*
 * Portions of this file are copyrighted by:
 * Copyright ?2003 Sun Microsystems, Inc. All rights reserved.
 * Use is subject to license terms specified in the COPYING file
 * distributed with the Net-SNMP package.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "net-snmp-config.h"

#include "net-snmp-includes.h"
#include "net-snmp-agent-includes.h"

#include <lib/msg_ipc_n.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/pkt_buffer.h>
#include <lib/msg_ipc.h>
#include <lib/ether.h>
#include <lib/log.h>

//#include "lib/snmp_common.h"
#include "snmp_config_table.h"


#include "mib_cliset.h"

FindVarMethod       cmd_line_mode_get;
WriteMethod         cmd_line_mode_set;

CLI_CMDS            cli_cmd_info;

/*
 * define the structure we're going to ask the agent to register our
 * information at
 */
struct variable1  cmdlineSetMode_variables[] =
{
    {cmdlineString, ASN_OCTET_STR, RWRITE, cmd_line_mode_get, 1, {1}},
    {cmdlineAckErrorCode, ASN_INTEGER, RONLY, cmd_line_mode_get, 1, {2}},
    {cmdlineAckErrorSet, ASN_INTEGER, RONLY, cmd_line_mode_get, 1, {3}},
    {cmdlineAckErrorInfo, ASN_OCTET_STR, RONLY, cmd_line_mode_get, 1, {4}}
} ;

/*
 * Define the OID pointer to the top of the mib tree that we're
 * registering underneath
 */
oid cli_set_mode_mode_oid[] = { HUAHUAN_CLI_CMD_SET, 1};
int cli_set_mode_mode_oid_len = sizeof(cli_set_mode_mode_oid) / sizeof(oid);

void init_mib_cliset(void)
{
    REGISTER_MIB("ipran/ipran_cli_set_mode", cmdlineSetMode_variables, variable1,
                 cli_set_mode_mode_oid);

    snmpcli_init_cmds();
}

int snmpcli_init_cmds(void)
{
    memset(&cli_cmd_info, 0,  sizeof(struct _CLI_CMDS));
    cli_cmd_info.cmd_status = CLI_SUCCESS;
    cli_cmd_info.error_cmd_index = 0;
	cli_cmd_info.error_code = 0;

	memset(cli_cmd_info.cmd_str, 0, CMD_MAX_LEN);
	memset(cli_cmd_info.error_msg, 0, CMD_ERR_MSG_MAX_LEN);

    return 0;
}



#if 0
/* send message and returen errcode */
int ipc_send_common_wait_ack_cli_set(void *pdata, int data_len, int data_num, int module_id, int sender_id,
                                     enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
    static struct ipc_msghdr_n msghdr;
    static struct ipc_mesg_n msg;
    int ret, errcode;
    //int count = 1000*150;
    int count = 1000 * 30; // 30 seconds

    if (ipc_common_id < 0)
    {
        ipc_connect_common();

        if (ipc_common_id < 0)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: ipc connect common failed!\n", __FILE__, __LINE__);
            return -1;
        }
    }

    if (ipc_reply_id < 0)
    {
        ipc_reply_id = ipc_connect(IPC_MSG_REPLY);

        if (ipc_reply_id < 0)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
            return -1;
        }
    }

    msghdr.data_len = data_len;
    msghdr.module_id = module_id;
    msghdr.msg_type = msg_type;
    msghdr.msg_subtype = subtype;
    msghdr.msg_index = msg_index;
    msghdr.data_num = data_num;
    msghdr.opcode = opcode;
    msghdr.sender_id = sender_id;

    /*clear old reply*/
    while (ipc_recv(ipc_reply_id, (void *)&msg, sender_id) != -1)
    {
    }

    ret = ipc_send(ipc_common_id, &msghdr, pdata);

    if (ret == -1)
    {
        return -1;
    }

wait:
    usleep(1000);

    if (count)
    {
        count --;
        ret = ipc_recv(ipc_reply_id, (void *) & msg, sender_id);

        if (ret == -1)
        {
            goto wait;
        }

        if ((msg.msghdr.msg_type != msg_type)
                || (msg.msghdr.msg_subtype != subtype)
                || (msg.msghdr.msg_index != msg_index)
                || (msg.msghdr.sender_id != module_id))
        {
            goto wait;
        }

        if (msg.msghdr.opcode == IPC_OPCODE_NACK)
        {
            errcode = *(uint32_t *)msg.msg_data;
            return errcode;
        }
        else if (msg.msghdr.opcode == IPC_OPCODE_ACK)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        zlog_info("%s[%d] ipc ack time out \n", __FUNCTION__, __LINE__);
        return -1;
    }
}
#endif
									 


int snmpcli_parser_cmd(CLI_CMDS *pcmds_info_send)
{
    struct ipc_mesg_n *pmsg = ipc_sync_send_n2((void *)pcmds_info_send, sizeof(CLI_CMDS), 1, MODULE_ID_VTY, MODULE_ID_SNMPD, 
		IPC_TYPE_VTY, VTY_MSG_CMD, 0, 0, 1000*10);	

    if(pmsg)
    {
		CLI_CMDS *pcmds_info_recv = (CLI_CMDS *)pmsg->msg_data;

		memcpy(&cli_cmd_info, pcmds_info_recv, sizeof(CLI_CMDS));

		mem_share_free(pmsg, MODULE_ID_SNMPD);
	}

    return cli_cmd_info.error_code;
}

u_char *cmd_line_mode_get(struct variable *vp,
                          oid *name,
                          size_t *length,
                          int exact, size_t *var_len, WriteMethod **write_method)
{
    static int      intval;
    char            *error_info = {0};

    if (header_generic(vp, name, length, exact, var_len, write_method) ==
            MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case cmdlineString:
            *write_method = cmd_line_mode_set;
            *var_len = cli_cmd_info.cmd_str_len;
            return (u_char *)cli_cmd_info.cmd_str;

        case cmdlineAckErrorCode:
            *var_len = sizeof(int);
            intval = cli_cmd_info.cmd_status;
            return (u_char *)&intval;

        case cmdlineAckErrorSet:
            *var_len = sizeof(int);
            intval = cli_cmd_info.error_cmd_index;
            return (u_char *)&intval;

        case cmdlineAckErrorInfo:
            *var_len = cli_cmd_info.error_msg_len;
            return (u_char *)cli_cmd_info.error_msg;

        default :
            return NULL;
    }

    return NULL;
}

int cmd_line_mode_set(int action,
                      u_char *var_val,
                      u_char var_val_type,
                      size_t var_val_len,
                      u_char *statP, oid *name, size_t name_len)
{
    u_char    *cp;
    int       count, *setvar = NULL;
    int       ret = -1;
    char      buf[CMDS_MAX_LEN] = {0};

    switch (action)
    {
        case RESERVE1:
            return SNMP_ERR_NOERROR;

        case RESERVE2:
            return SNMP_ERR_NOERROR;

        case ACTION:
            return SNMP_ERR_NOERROR;

        case FREE:
            return SNMP_ERR_NOERROR;

        case UNDO:
            return SNMP_ERR_NOERROR;

        case COMMIT:

            switch ((char)name[name_len - 2])
            {
                case cmdlineString:

                    if (CLI_EXECUTING == cli_cmd_info.cmd_status)
                    {
                        return SNMP_ERR_GENERR;
                    }

					if(var_val_len > CMDS_MAX_LEN)
					{
						return SNMP_ERR_WRONGLENGTH;
					}
                    memset(buf, 0, CMDS_MAX_LEN);
                    memcpy(buf, var_val, var_val_len);

                    snmpcli_init_cmds();
                    cli_cmd_info.cmd_status = CLI_EXECUTING;
                    memcpy(cli_cmd_info.cmd_str, var_val, var_val_len);
                    //ret = snmpcli_parser_cmd(buf, cli_cmd_info);
                    ret = snmpcli_parser_cmd(&cli_cmd_info);

                    if (CLI_SUCCESS != ret)
                    {
                        cli_cmd_info.cmd_status = CLI_CMD_EXECUTE_FAILED ;
                        return SNMP_ERR_COMMITFAILED;
                    }

                    cli_cmd_info.cmd_status = CLI_SUCCESS ;
                    return (SNMP_ERR_NOERROR);

                default:
                    return SNMP_ERR_NOSUCHNAME;
            }

            return SNMP_ERR_NOERROR;
    }

    return SNMP_ERR_NOERROR;
}

