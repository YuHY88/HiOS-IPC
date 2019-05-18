/*
*    ftm packet receive and send
*
*/
#include "config.h"
#include <stdio.h>
#include <sched.h>
#include <lib/linklist.h>
#include <lib/msg_ipc_n.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/thread.h>
#include <sys/prctl.h>
#include <lib/zassert.h>
#include <lib/errcode.h>
#include <unistd.h>
#include <pthread.h>
#include "ftm.h"
#include "ftm_ifm.h"
#include "ftm_fib.h"
#include "ftm_bgp.h"
#include "ftm_nhp.h"
#include "ftm_arp.h"
#include "ftm_tunnel.h"
#include "ftm_vlan.h"
#include "ftm_pkt.h"
#include "ftm_vpls.h"
#include "ftm_ndp.h"
#include "ftm.h"
#include "ftm_nhlfe.h"
#include "ftm_lsp.h"
#include "ftm_ldp.h"

struct list ftm_msg_list;
pthread_mutex_t ftm_msg_lock;
extern void ftm_msg_ipc_init(void);
extern void ftm_msg_ipc_close(void);
int ftm_msg_send_reply (void *pdata, int data_len,struct ipc_msghdr_n *rcvhdr,uint32_t msg_index);
int ftm_msg_send_ack (struct ipc_msghdr_n *rcvhdr,uint32_t msg_index);
int ftm_msg_send_noack (uint32_t errcode, struct ipc_msghdr_n *rcvhdr,uint32_t msg_index);
int ftm_msg_send_hal_wait_ack(void *pdata, uint32_t data_len, uint16_t data_num, int module_id, int sender_id,
		enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index);
int ftm_msg_send_hal_wait_reply(void *pdata,void *recvdata, uint32_t data_len, uint16_t data_num, int module_id, int sender_id,
		enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index);


/* init the packet ipc queue */
#if 0
void ftm_msg_ipc_init()
{
    int id = 0;

    /* ipc for send control message to hal */
	id = ipc_connect(IPC_MSG_HAL);
	if(id < 0)
	{
	    zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
		assert(0);

	}
	else
	{
		ipc_hal_id = id;
	}
	return;
}
#endif

/* close the ipc queue */
void ftm_msg_ipc_close()
{
;
}

#if 0
int ftm_msg_rcv_control(void)
{
	int repeat_flag = 0;
	struct ipc_mesg mesg;
	struct ipc_mesg *pmesg_new = NULL;
	struct ipc_mesg *pdata = NULL;
	struct listnode *pnode = NULL;
	struct listnode *pnext = NULL;
	struct sched_param param;
	cpu_set_t mask;

	prctl(PR_SET_NAME, "ftm_msg_control");

	CPU_ZERO(&mask);
	CPU_SET(0, &mask);
	sched_setaffinity(0, sizeof(mask), &mask);
	param.sched_priority = 50;

#ifndef HAVE_KERNEL_3_0	
	if(sched_setscheduler(0, SCHED_RR, &param))
	{
		zlog_err("%s:Set priority error.\n", __func__);
	}
#endif

	while(1)
	{
#if 0
		if(100 == token++)
		{
			token = 1;
			usleep(1000);
		}
#endif

		if(0 > ipc_ftm_id)
		{
			ipc_ftm_id = ipc_connect(IPC_MSG_FTM);
		}

		if(-1 == ipc_recv_block(ipc_ftm_id, &mesg, MODULE_ID_FTM))	
		{
			usleep(10000);
			continue;
		}

		pthread_mutex_lock(&ftm_msg_lock);
		for(ALL_LIST_ELEMENTS(&ftm_msg_list, pnode, pnext, pdata))
		{
			if(0 == memcmp(pdata, &mesg, sizeof(struct ipc_mesg)))
			{
				listnode_move_to_tail(&ftm_msg_list, pnode);
				repeat_flag = 1;
				break;
			}
		}

		if(0 == repeat_flag)
		{
			pmesg_new = (struct ipc_mesg *)malloc(sizeof(struct ipc_mesg));
			if(NULL == pmesg_new)
			{
				pthread_mutex_unlock(&ftm_msg_lock);
				continue;
			}

			memcpy(pmesg_new, &mesg, sizeof(struct ipc_mesg));
			listnode_add(&ftm_msg_list, (void *)pmesg_new);
		}

		repeat_flag = 0;
		pthread_mutex_unlock(&ftm_msg_lock);
	}

	return 0;
}
#endif


int ftm_msg_rcv_n(struct ipc_mesg_n *pmesg, int imlen)
{
	ftm_msg_rcv_msg_n(pmesg, imlen);
		
	mem_share_free(pmesg, MODULE_ID_FTM);
	return ERRNO_SUCCESS;
}

int ftm_msg_rcv_msg_n(struct ipc_mesg_n *pmesg, int imlen)
{
	int ret = 0;
	int revln = 0;
	struct ipc_msghdr_n *phdr = NULL;
    //void *pdata = NULL;

    revln = (int)pmesg->msghdr.data_len + IPC_HEADER_LEN_N; 
	if(revln <= imlen)
	{
		phdr = &(pmesg->msghdr);
		switch(phdr->msg_type)
		{
			case IPC_TYPE_LSP:
				ftm_lsp_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode);
				break;
			case IPC_TYPE_ARP:
				ftm_arp_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode, phdr->sender_id);
				break;
            case IPC_TYPE_NDP:
                ftm_ndp_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode);
                break;
			case IPC_TYPE_IFM:
				ftm_ifm_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode, phdr->msg_index);
				break;
			case IPC_TYPE_PW:
				ftm_pw_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode);
				break;
			case IPC_TYPE_VSI:
				ftm_vsi_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode, phdr->msg_index);
				break;
			case IPC_TYPE_MPLSIF:
				ftm_mplsif_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode, phdr->msg_index);
				break;
			case IPC_TYPE_L3IF:
				ftm_l3if_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode,phdr->msg_index);
				break;
			case IPC_TYPE_ARPIF:
				ftm_arpif_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode,phdr->msg_index);
				break;
            case IPC_TYPE_NDPIF:
                ftm_ndpif_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode,phdr->msg_index);
                break;
            case IPC_TYPE_FIB:
                ftm_fib_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode);
                break;
            case IPC_TYPE_NHP:
                ftm_nhp_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode);
                break;
			case IPC_TYPE_TUNNEL:
				ftm_tunnel_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode, phdr->msg_index);
				break;
			case IPC_TYPE_VLAN:
				ftm_vlan_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode, phdr->msg_index);
				break;
			case IPC_TYPE_PROTO:
				ftm_proto_msg(pmesg->msg_data, phdr->sender_id, phdr->msg_subtype, phdr->opcode);
				break;
			case IPC_TYPE_BGP:
				ftm_bgp_msg_proc(pmesg->msg_data, phdr->data_len, phdr->sender_id,phdr->msg_subtype);
                break;
            case IPC_TYPE_LDP:
                ftm_ldp_frr_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode);
                break;

			case IPC_TYPE_PACKET:	
				if( phdr->sender_id == MODULE_ID_HAL )
				{
					ftm_pkt_rcv_n(pmesg);
				}
				else
				{
					ret = ftm_pkt_msg_n(phdr,pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode, phdr->sender_id);
					if( ret != 0 )
					{
						ftm_pkt_rcv_from_app_n(pmesg);
					}
				}
				break;
			case IPC_TYPE_MPLSOAM:
                ftm_tpoam_msg(phdr,pmesg->msg_data, phdr->data_len, phdr->msg_subtype, phdr->opcode, phdr->msg_index);
                break;
            case IPC_TYPE_HA:
                if (phdr->msg_subtype == IPC_TYPE_ARP)
                {
                    ftm_arp_rcv_ha( pmesg );
                }
                break;
			case IPC_TYPE_DEVM:
				ftm_arp_devm_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode);
				break;
			default:
				break;
		}
	}

    return 0;
}

#if 0
/* receive control message from app */
int ftm_msg_rcv(struct ipc_mesg *pmesg)
{
	int token = 100;
	struct ipc_msghdr *phdr = NULL;
	struct ipc_mesg *pmesg_new = NULL;

	while(token)
	{
		token--;

		if(list_isempty(&ftm_msg_list))
		{
		    return(-1);
            
			//continue;
		}

		pthread_mutex_lock(&ftm_msg_lock);
		pmesg_new = listnode_head(&ftm_msg_list);	
		if(NULL != pmesg_new && NULL != pmesg)
		{
			memcpy(pmesg, pmesg_new, sizeof(struct ipc_mesg));
			listnode_delete(&ftm_msg_list, pmesg_new);

			free(pmesg_new);
			pmesg_new = NULL;
		}
		pthread_mutex_unlock(&ftm_msg_lock);

		phdr = &(pmesg->msghdr);
		switch(phdr->msg_type)
		{
			case IPC_TYPE_LSP:
				ftm_lsp_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode);
				break;
			case IPC_TYPE_ARP:
				ftm_arp_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode);
				break;
            case IPC_TYPE_NDP:
                ftm_ndp_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode);
                break;
			case IPC_TYPE_IFM:
				ftm_ifm_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode, phdr->msg_index);
				break;
			case IPC_TYPE_PW:
				ftm_pw_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode);
				break;
			case IPC_TYPE_VSI:
				ftm_vsi_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode, phdr->msg_index);
				break;
			case IPC_TYPE_MPLSIF:
				ftm_mplsif_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode, phdr->msg_index);
				break;
			case IPC_TYPE_L3IF:
				ftm_l3if_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode,phdr->msg_index);
				break;
			case IPC_TYPE_ARPIF:
				ftm_arpif_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode,phdr->msg_index);
				break;
            case IPC_TYPE_NDPIF:
                ftm_ndpif_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode,phdr->msg_index);
                break;
            case IPC_TYPE_FIB:
                ftm_fib_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode);
                break;
            case IPC_TYPE_NHP:
                ftm_nhp_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode);
                break;
			case IPC_TYPE_TUNNEL:
				ftm_tunnel_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode, phdr->msg_index);
				break;
			case IPC_TYPE_VLAN:
				ftm_vlan_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode, phdr->msg_index);
				break;
			case IPC_TYPE_PROTO:
				ftm_proto_msg(pmesg->msg_data, phdr->sender_id, phdr->msg_subtype, phdr->opcode);
				break;
			case IPC_TYPE_BGP:
				ftm_bgp_msg_proc(pmesg->msg_data, phdr->data_len, phdr->sender_id,phdr->msg_subtype);
                break;
            case IPC_TYPE_LDP:
                ftm_ldp_frr_msg(pmesg->msg_data, phdr->data_len, phdr->data_num, phdr->msg_subtype, phdr->opcode);
                break;
			default:
				break;
		}
	}

    return 0;
}
#endif

/* send msg to hal */
int ftm_msg_send_to_hal(void *pdata, int data_len, int data_num,
						  enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
	int ret = 0;
	struct ipc_mesg_n *pSndMsg = NULL;
	
	pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + data_len, MODULE_ID_FTM);
	if(pSndMsg != NULL)
	{
		pSndMsg->msghdr.data_len    = data_len;
		pSndMsg->msghdr.module_id   = MODULE_ID_HAL;
		pSndMsg->msghdr.sender_id   = MODULE_ID_FTM;
		pSndMsg->msghdr.msg_type    = msg_type;
		pSndMsg->msghdr.msg_subtype = subtype;
		pSndMsg->msghdr.msg_index   = msg_index;
		pSndMsg->msghdr.data_num    = data_num;
		pSndMsg->msghdr.opcode      = opcode;
			
		if(pdata)
		{
			memcpy(pSndMsg->msg_data, pdata, data_len);
		}
		
		/*send info*/
		ret = ipc_send_msg_n1(pSndMsg, sizeof(struct ipc_mesg_n) + data_len);
		if(ret)
		{
			zlog_err("%s[%d],ipc_send_msg_n1 failed\n",__FUNCTION__,__LINE__);
			ipc_msg_free(pSndMsg,MODULE_ID_FTM);
			return ret;
		}
	}
	else
	{
		zlog_err("%s[%d],mem_share_malloc failed\n",__FUNCTION__,__LINE__);
		return -1;
	}

	return ret;
}

#if 0
/* send msg to hal */
int ftm_msg_send_to_hal(void *pdata, int data_len, int data_num,
						  enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
	static struct ipc_msghdr msghdr;
	int ret = 0;

	if(ipc_hal_id < 0 )
    {
		ipc_hal_id = ipc_connect(IPC_MSG_HAL);

        if(ipc_hal_id < 0 )
        {
            zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
            return -1;
        }
    }

	msghdr.data_len = data_len;
	msghdr.module_id = MODULE_ID_HAL;
	msghdr.msg_type = msg_type;
	msghdr.msg_subtype = subtype;
	msghdr.msg_index = msg_index;
	msghdr.data_num = data_num;
	msghdr.opcode = opcode;
	msghdr.sender_id = MODULE_ID_FTM;
#if 0
	ret = ipc_send(ipc_hal_id, &msghdr, pdata);
    if(ret < 0)
		usleep(1000);   /* ·¢ËÍÊ§°ÜÈÃ cpu */
#endif
	ret = ipc_send_block(ipc_hal_id, &msghdr, pdata);

	return ret;
}
#endif 

static int ftm_get_bulk_lsp(uint32_t *lsp_buf, uint32_t ifindex, uint32_t lsp_max)
{
    struct hash_bucket *pbucket = NULL;
    struct nhlfe_entry *pnhlfe  = NULL;
    uint32_t lsp_cnt            = 0;
	int cursor                  = 0;

	HASH_BUCKET_LOOP(pbucket, cursor, nhlfe_table)
	{
		pnhlfe = (struct nhlfe_entry *)pbucket->data;
        if (NULL == pnhlfe)
        {
			continue;
        }

        if(pnhlfe->nhp_type == NHP_TYPE_CONNECT && pnhlfe->nhp_index == ifindex)
        {
            memcpy(&lsp_buf[lsp_cnt++], &pnhlfe->lsp_index, sizeof(uint32_t));
        	if (lsp_cnt == lsp_max)
        	{
            	return lsp_cnt;
        	}
        }
    }

	return lsp_cnt;
}

int ftm_msg_rcv_get_bulk_lsp(struct ipc_msghdr_n *phdr,uint32_t subtype, uint32_t ifindex)
{
	uint32_t lsp_max            = IPC_MSG_LEN/sizeof(uint32_t);
    uint32_t lsp_buf[lsp_max];
	uint32_t lsp_cnt            = 0;
	int ret                     = 0;
	struct ipc_mesg_n *pSndMsg = NULL;

	memset (lsp_buf, 0, lsp_max * sizeof(uint32_t));

    lsp_cnt = ftm_get_bulk_lsp(lsp_buf, ifindex, lsp_max);
	if (lsp_cnt > 0)
	{
		/*ret = ipc_send_reply_bulk(lsp_buf, lsp_cnt * sizeof(uint32_t), lsp_cnt, MODULE_ID_MPLS,
                             MODULE_ID_FTM, IPC_TYPE_MPLSOAM, subtype, ifindex);*/

		pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + lsp_cnt * sizeof(uint32_t), MODULE_ID_FTM);
		if(pSndMsg != NULL)
		{
			pSndMsg->msghdr.data_len    = lsp_cnt * sizeof(uint32_t);
			pSndMsg->msghdr.module_id   = MODULE_ID_MPLS;
			pSndMsg->msghdr.sender_id   = MODULE_ID_FTM;
			pSndMsg->msghdr.msg_type    = IPC_TYPE_MPLSOAM;
			pSndMsg->msghdr.msg_subtype = subtype;
			pSndMsg->msghdr.msg_index   = ifindex;
			pSndMsg->msghdr.data_num    = lsp_cnt;
			pSndMsg->msghdr.opcode      = IPC_OPCODE_REPLY;
				
			if(lsp_buf)
			{
				memcpy(pSndMsg->msg_data, lsp_buf, lsp_cnt * sizeof(uint32_t));
			}
			
			/*send info*/
			ret = ipc_send_msg_n1(pSndMsg, sizeof(struct ipc_mesg_n) + lsp_cnt * sizeof(uint32_t));
			if(ret)
			{
				zlog_err("%s[%d],ipc_send_msg_n1 failed\n",__FUNCTION__,__LINE__);
				ipc_msg_free(pSndMsg,MODULE_ID_FTM);
				return ret;
			}
		}							 
	}
	else
	{
		/*ret = ipc_send_noack(ERRNO_NOT_FOUND, MODULE_ID_MPLS, MODULE_ID_FTM, IPC_TYPE_MPLSOAM,
                             subtype, ifindex);*/
		
		ret = ftm_msg_send_noack(ERRNO_NOT_FOUND,&phdr,ifindex);
	}

	return ret;
}


int ftm_msg_send_reply (void *pdata, int data_len,struct ipc_msghdr_n *rcvhdr,uint32_t msg_index)
{
					
	struct ipc_mesg_n * repmesg = NULL;
	uint32_t msg_len = 0;
						
	msg_len = sizeof(struct ipc_msghdr_n);/*info len*/
								
	repmesg = ipc_msg_malloc(msg_len + data_len,MODULE_ID_FTM);
	if(NULL == repmesg)	
	{
		return ERRNO_FAIL;					
	}
					
	memset(repmesg,0,msg_len + data_len);
						
	repmesg->msghdr.data_len = data_len; 
	repmesg->msghdr.data_num = 0;                 
	repmesg->msghdr.msg_index = msg_index;
	repmesg->msghdr.opcode = IPC_OPCODE_REPLY;

	if(pdata)
	{
		memcpy(repmesg->msg_data,pdata,data_len);
	}

	/*send fail so free local malloc memory*/
	if(ipc_send_reply_n1(rcvhdr, repmesg, msg_len + data_len))
	{
		mem_share_free(repmesg, MODULE_ID_FTM);	
		return ERRNO_FAIL;
	}		
	return ERRNO_SUCCESS;
						
}

/************************************************************
* Function : ftm_msg_send_ack
* return   : 0 ok , 1 error   
* description: send errcode as reply because of no local data
************************************************************/
				
/*send ack as reply*/
int ftm_msg_send_ack (struct ipc_msghdr_n *rcvhdr,uint32_t msg_index)
{
					
	struct ipc_mesg_n * repmesg = NULL;
	uint32_t msg_len = 0;
						
	msg_len = sizeof(struct ipc_msghdr_n);/*info len*/
								
	repmesg = ipc_msg_malloc(msg_len,MODULE_ID_FTM);
	if(NULL == repmesg)	
	{
		return ERRNO_FAIL;
								
	}
					
	memset(repmesg,0,msg_len);
						
	repmesg->msghdr.data_len = 0; 
	repmesg->msghdr.data_num = 0;                 
	repmesg->msghdr.msg_index = msg_index;
	repmesg->msghdr.opcode = IPC_OPCODE_ACK;

	/*send fail so free local malloc memory*/
	if(ipc_send_reply_n1(rcvhdr, repmesg, msg_len))
	{
		mem_share_free(repmesg, MODULE_ID_FTM);	
		return ERRNO_FAIL;
	}
					
	return ERRNO_SUCCESS;
						
}

/************************************************************
 * Function : ftm_msg_send_noack
 * return   : 0 ok , 1 error   
 * description: send errcode as reply because of no local data
 ************************************************************/

/*send noack as reply*/
int ftm_msg_send_noack (uint32_t errcode, struct ipc_msghdr_n *rcvhdr,uint32_t msg_index)
{
	
		struct ipc_mesg_n * repmesg = NULL;
		uint32_t msg_len = 0;
		
		msg_len = sizeof(struct ipc_msghdr_n)+sizeof(uint32_t);
		
		repmesg = ipc_msg_malloc(msg_len,MODULE_ID_FTM);
		if(NULL == repmesg)	
		{
			return ERRNO_FAIL;
				
		}
	
		memset(repmesg,0,msg_len);
		
		memcpy(repmesg->msg_data,&errcode,4);
		
		repmesg->msghdr.data_len = 4;   /*sizeof(errcode)*/
		repmesg->msghdr.data_num = 1;                 
		repmesg->msghdr.msg_index = msg_index;
		repmesg->msghdr.opcode = IPC_OPCODE_NACK;

		if(ipc_send_reply_n1(rcvhdr, repmesg, msg_len))
		{
			mem_share_free(repmesg, MODULE_ID_FTM);	
				return ERRNO_FAIL;
		}
	
		return ERRNO_SUCCESS;
		
}
/* ***********************************************************
 *  Function : ftm_msg_send_hal_wait_ack
 *  return   : 0 ok, 1 error
 *  description: send set msg to hal and wait ack, used to replace "ipc_send_hal_wait_ack"
 * ***********************************************************/
/*send info confirm ack or noack*/
int ftm_msg_send_hal_wait_ack(void *pdata, uint32_t data_len, uint16_t data_num, int module_id, int sender_id,
		enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
	int ret = 0;
	int rcvlen = 0;
	struct ipc_mesg_n *pSndMsg = NULL;
	struct ipc_mesg_n * pRcvMsg = NULL;
	
	pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + data_len, MODULE_ID_FTM);
	if(pSndMsg != NULL)
	{
		pSndMsg->msghdr.data_len    = data_len;
		pSndMsg->msghdr.module_id   = module_id;
		pSndMsg->msghdr.sender_id   = sender_id;
		pSndMsg->msghdr.msg_type    = msg_type;
		pSndMsg->msghdr.msg_subtype = msg_subtype;
		pSndMsg->msghdr.msg_index   = msg_index;
		pSndMsg->msghdr.data_num    = data_num;
		pSndMsg->msghdr.opcode      = opcode;
			
		if(pdata)
		{
			memcpy(pSndMsg->msg_data, pdata, data_len);
		}
		
		/*send info*/
		ret = ipc_sync_send_n1(pSndMsg, sizeof(struct ipc_mesg_n) + data_len, &pRcvMsg, &rcvlen, 5000);
			
		switch(ret)
		{
			/*send fail*/
			case -1:
			{
				ipc_msg_free(pSndMsg,MODULE_ID_FTM);
				return -1;
			}
			case -2:
			{
				/*recv fail*/
				return -1;
							
			}
			case 0:
			{	/*recv success*/
				if(pRcvMsg != NULL)
				{	
					/*if noack return errcode*/
					if(IPC_OPCODE_NACK == pRcvMsg->msghdr.opcode)
					{
						memcpy(ret,pRcvMsg->msg_data,sizeof(ret));
					}
					/*if ack return 0*/
					else if(IPC_OPCODE_ACK == pRcvMsg->msghdr.opcode)
					{
						ret = 0;
					}
					else
					{
						ret = -1;
					}
					//ret = (int)pRcvMsg->msg_data;
					ipc_msg_free(pRcvMsg,MODULE_ID_FTM);
				}
				else
				{
					return -1;
				}
			}
			default:
				return -1;
						
		}
		
	}
	else
	{
		return -1;
	}
	return(ret);
}      




/* ***********************************************************
 *  Function : ftm_msg_send_hal_wait_reply
 *  return   : 0 ok , 1 error
 *  description: send get msg to hal and wait reply data, used to replace "ipc_send_hal_wait_reply"
 * ***********************************************************/

/*send info wait reply*/
int ftm_msg_send_hal_wait_reply(void *pdata,void *recvdata, uint32_t data_len, uint16_t data_num, int module_id, int sender_id,
		enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
		int    iRetv = 0;
		int    iRepL = 0;
		
		struct ipc_mesg_n *pMsgSnd = NULL;
		struct ipc_mesg_n *pMsgRcv = NULL;
		
		pMsgSnd = mem_share_malloc(sizeof(struct ipc_mesg_n), MODULE_ID_FTM);
		
		if(pMsgSnd == NULL) 
		{
			return ERRNO_FAIL;
		}
	
		memset(pMsgSnd, 0, sizeof(struct ipc_mesg_n));
		
		/*fill msg header*/
		pMsgSnd->msghdr.data_len    = data_len;   
		pMsgSnd->msghdr.data_num    = data_num;
		pMsgSnd->msghdr.module_id   = module_id;
		pMsgSnd->msghdr.sender_id   = sender_id;
		pMsgSnd->msghdr.msg_type    = msg_type;
		pMsgSnd->msghdr.msg_subtype = msg_subtype;
		pMsgSnd->msghdr.opcode      = opcode;
		pMsgSnd->msghdr.msg_index   = msg_index;

		if( NULL != pdata )
		{
			memcpy(pMsgSnd->msg_data,pdata,data_len);
		}
		
		/*send info send fail:-1 recv fail:-2 ok:0 */
		iRetv = ipc_sync_send_n1(pMsgSnd, sizeof(struct ipc_mesg_n), &pMsgRcv, &iRepL, 5000);
		
		switch(iRetv)
		{
			case -1:  
			{	/*send fail*/
				 mem_share_free(pMsgSnd, MODULE_ID_FTM);
				 return ERRNO_FAIL;
			}
			case -2:  
			{
				return ERRNO_FAIL;
			}
			case  0:
			{
				if(NULL == pMsgRcv) 
				{
					return ERRNO_FAIL;
				}
				else 
				{        
					memcpy(recvdata,pMsgRcv->msg_data,\
						pMsgRcv->msghdr.data_len >= data_len ? data_len : pMsgRcv->msghdr.data_len);
							
					/*recv success free memory*/
					mem_share_free(pMsgRcv, MODULE_ID_FTM);
					return ERRNO_SUCCESS;
				}
					
			}    
			default: 
				 return ERRNO_FAIL;
		}
}

