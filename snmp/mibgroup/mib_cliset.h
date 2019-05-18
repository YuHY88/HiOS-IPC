/*
 *  cli_set group interface - ipran_cli_set.h
 *
 */
#ifndef _IPRAN_CLI_SET_H
#define _IPRAN_CLI_SET_H


#define CMDS_MAX_LEN		(512 - 1)
#define CMD_MAX_LEN			128
#define CMD_ERR_MSG_MAX_LEN	128


void init_ipran_cli_set(void);

#define HUAHUAN_CLI_CMD_SET 1,3,6,1,4,1,9966,5,30

#define cmdlineString                        1
#define cmdlineAckErrorCode                  2
#define cmdlineAckErrorSet                   3
#define cmdlineAckErrorInfo                  4

#define cmdlineStringView                    5
#define cmdlineStringCmd                     6
#define cmdlineAckErrorCode2                 7
#define cmdlineAckErrorSet2                  8
#define cmdlineAckErrorInfo2                 9

#define cmdTransactionID                     1
#define cmdTransactionPktID                  2
#define cmdTransactionStatus                 3
#define cmdTransactionTotalPktCount          4
#define cmdTransactionTotalCMDCount          5
#define cmdTransactionCmdCount               6
#define cmdTransactionCmds                   7
#define cmdlineTransactionTableRowStatus     8

#define cmdTransactionStatusID               1
#define cmdTransactionStatusxecuteStatus     2
#define cmdTransactionStatusFailedPktID      3
#define cmdTransactionStatusFailedCmdID      4
#define cmdTransactionStatusFailedErrorCode  5
#define cmdTransactionStatusFailedCmd        6
#define cmdTransactionStatusFailedMsg        7

//#define MAX_CLI_LENGTH  255
//#define MAX_PDU_LENGTH  (0xffff-28)

enum CLI_EXECUTE_STATUS
{
    CLI_SUCCESS = 0,
    CLI_EXECUTING,
    CLI_TIMEOUT,
    CLI_CMDSIZEOUT ,
    CLI_CMD_EXECUTE_FAILED
};

typedef struct _CLI_CMDS
{
    int cmd_status;
    int error_cmd_index;
    int error_code;

	size_t error_msg_len;
	size_t cmd_str_len;
	
    char error_msg[CMD_ERR_MSG_MAX_LEN];
    char cmd_str[CMD_MAX_LEN];
	
} CLI_CMDS;

void init_mib_cliset(void);

int snmpcli_parser_cmd(CLI_CMDS *pcmds_info_send);

int snmpcli_init_cmds(void);

#endif   /* _IPRAN_CLI_SET_H */
