/* file process command 
   name filem_fpga.c
   edit suxq
   date 2016/05/26
*/
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include "lib/memory.h"
#include "lib/module_id.h"
#include "lib/memshare.h"
#include "lib/msg_ipc_n.h"
#include "lib/log.h"


#include "filem.h"
#include "filem_tran.h"
#include "filem_cmd.h"
#include "filem_img.h"
#include "filem_oper.h"

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

#define FILE_UPDATE_SUCCESS 0//升级成功
#define FILE_UPDATE_FAIL    1//升级失败

#define FILE_CMD_INVALID    2//无意义  
#define FILE_CMD_PRINT      3//需要打印出的信息


/*---------------------------------------------------------*/
/* Name: filem_fpga_upgrade                                */
/* Retu: 0  1                                              */
/* Desc: FPGA升级                                          */
/*---------------------------------------------------------*/
int filem_fpga_upgrade(int slot, int index, char *filename)
{
    int  iRetVal = 0;
    int  iStrlen = 0;
    
    struct ipc_mesg_n *pSndMsg = NULL;
    struct ipc_mesg_n *pRcvMsg = NULL;

    pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + 200, MODULE_ID_FILE);
    
    if(pSndMsg == NULL) return(-FILEM_ERROR_NOMEM);

    memset(&pSndMsg->msghdr, 0, sizeof(struct ipc_msghdr_n));
    
    ipc_clear_que_n(MODULE_ID_REPLY_A, MODULE_ID_FILE);

    iStrlen = sprintf((char*)pSndMsg->msg_data, "%d_%d_%s", slot, index, filename);  //自定义消息格式，需与hal统一

    pSndMsg->msghdr.module_id   = MODULE_ID_HAL;
    pSndMsg->msghdr.sender_id   = MODULE_ID_FILE;
    pSndMsg->msghdr.msg_type    = IPC_TYPE_FILE;
    pSndMsg->msghdr.msg_subtype = FILE_CMD_INVALID;
    pSndMsg->msghdr.opcode      = IPC_OPCODE_EVENT;
    pSndMsg->msghdr.data_num    = 1;
    pSndMsg->msghdr.data_len    = (unsigned int)iStrlen;
  
    zlog_debug(FILEM_DEBUG_UPGRADE, "filem_fpga_upgrade send hal data=%s\r\n", pSndMsg->msg_data);

    iRetVal = ipc_send_msg_n1(pSndMsg, iStrlen + sizeof(struct ipc_msghdr_n));

    if(iRetVal == 0)
    {
        int  iTimeOut = 0;
        
        sleep(3);                                           /* 从发出升级命令到收到第一个包大概时间是20秒 */
                                                             /* 阻塞到 FPGA 升级完成 */
        while(1)
        {
            pRcvMsg = NULL;
            
            iRetVal = ipc_recv_reply_n(MODULE_ID_FILE, &pRcvMsg, &iStrlen, 1000);
                                      
            if(iRetVal == -1)
            {                
                if(++iTimeOut >= 90)               //90 s 收不到 hal 回传信息则退出
                {
                     zlog_debug(FILEM_DEBUG_UPGRADE, "filem dont get information form hal while upgrade fpga\r\n");
                     
                     zlog_debug(FILEM_DEBUG_UPGRADE, "upgrade fpga file name =%s time out\r\n", filename);
                     
                     iRetVal = -FILEM_ERROR_UPDATEFPGA;
                     break;
                }
            }
            else
            {
                if(pRcvMsg->msghdr.msg_type == IPC_TYPE_FILE)
				{
	                if(pRcvMsg->msghdr.msg_subtype == FILE_CMD_PRINT)//输出需要打印的消息
	                {
                         iTimeOut = 0;
	                     zlog_debug(FILEM_DEBUG_UPGRADE,"%s\n", pRcvMsg->msg_data);
			        }
					else if(pRcvMsg->msghdr.msg_subtype == FILE_CMD_INVALID)
                    {
                         iTimeOut = 0;
                         zlog_debug(FILEM_DEBUG_UPGRADE,"%s\n", pRcvMsg->msg_data);
	                }
	                else if(pRcvMsg->msghdr.msg_subtype == FILE_UPDATE_SUCCESS)
	                {
	                     iRetVal = 0;
	                     mem_share_free(pRcvMsg, MODULE_ID_FILE);
	                     break;
	                }
	                else if(pRcvMsg->msghdr.msg_subtype == FILE_UPDATE_FAIL)
	                {
	                     iRetVal = -FILEM_ERROR_UPDATEFPGA;
	                     mem_share_free(pRcvMsg, MODULE_ID_FILE);
	                     zlog_debug(FILEM_DEBUG_UPGRADE, "filem upgrade fpga error,slot = %d, index =%d \r\n", slot, index);
	                     break;
	                } 
	                else
	                {
	                     mem_share_free(pRcvMsg, MODULE_ID_FILE);
	                }  
				}  
			    mem_share_free(pRcvMsg, MODULE_ID_FILE);                
            }
        }
    }
    else 
    {
         iRetVal = -FILEM_ERROR_UPDATEFPGA;
         mem_share_free(pSndMsg, MODULE_ID_FILE);
    }
    return(iRetVal);
}



