/**
 * @file      : cfm_test.c
 * @brief     : define of Y.1731 test,lck
 * @details   : 
 * @author    : huoqq
 * @date      : 2018年3月23日 14:34:30
 * @version   : 
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      : 
 */

#include <string.h>
#include <stdlib.h>
#include <lib/hash1.h>
#include <lib/types.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/thread.h>
#include <lib/linklist.h>
#include <lib/errcode.h>
#include "lib/msg_ipc.h"
#include <lib/alarm.h>
#include <lib/command.h>
#include <lib/vty.h>
#include <lib/ifm_common.h>
#include <lib/log.h>

#include "lib/pkt_buffer.h"

#include "cfm_session.h"
#include "l2_if.h"

#include "cfm.h"
#include "cfm_test.h"

#include <lib/zassert.h>
#include "../vlan.h"

//static int cfm_send_lck_timer(struct thread *thread);
static int cfm_send_lck(struct cfm_sess 	*psess);
//static int cfm_send_test_timer(struct thread *thread);
static int cfm_send_test(struct cfm_sess 	*psess);


extern struct thread_master *l2_master;
uint32_t crc32_table[256];

/**
 * @brief      : lck报文超时接收定时器
 * @param[in ] : thread，定时器指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */
#if 0
static int cfm_lck_timer(struct thread *thread)
{
	uint32_t 		 key;
	struct cfm_sess *psess;
	
	key   = (uint32_t)THREAD_ARG(thread);		
	psess = cfm_session_lookup(key);
		
    if(NULL == psess)
    {       	
		zlog_err("%s: %s, pointer error, psess:%p,return!!\n",__FILE__,__func__,psess);
        return ERRNO_FAIL;
    }

	if(psess->lck_rcv_interval == 0)//lck disappear
	{
		psess->alarm.alarm_lck 	= 0;
		psess->plck_timer 		= NULL;
		cfm_session_alarm_report(psess,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_LCK);
	}
	else//lck exist
	{
		psess->plck_timer 		= thread_add_timer(l2_master, cfm_lck_timer, (void *)(psess->local_mep|(psess->ma_index<<16)), psess->lck_rcv_interval);
		psess->lck_rcv_interval = 0;		
	}
	
	
	return ERRNO_SUCCESS;
}
#endif

static int cfm_lck_rcv_timer_out(void *para)
{
	uint32_t 		 key;
	struct cfm_sess *psess;
	char timerName[20] = "CfmLckRcvTimer";

	if (NULL == para)
	{
		 return ERRNO_FAIL;
	}
	
	key   = (uint32_t)para;	
	psess = cfm_session_lookup(key);
		
    if(NULL == psess)
    {       	
		zlog_err("%s: %s, pointer error, psess:%p,return!!\n",__FILE__,__func__,psess);
        return ERRNO_FAIL;
    }

	if(psess->lck_rcv_interval == 0)//lck disappear
	{
		psess->alarm.alarm_lck 	= 0;

		//high_pre_timer_delete(psess->plck_timer);
		psess->plck_timer 		= 0;
		
		cfm_session_alarm_report(psess,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_LCK);
	}
	else//lck exist
	{
		//psess->plck_timer 		= thread_add_timer(l2_master, cfm_lck_timer, (void *)(psess->local_mep|(psess->ma_index<<16)), psess->lck_rcv_interval);
		psess->plck_timer = high_pre_timer_add(timerName, LIB_TIMER_TYPE_NOLOOP, cfm_lck_rcv_timer_out, 
							(void *)(psess->local_mep|(psess->ma_index<<16)), psess->lck_rcv_interval * 1000);
		
		psess->lck_rcv_interval = 0;		
	}
	
	
	return ERRNO_SUCCESS;
}

/**
 * @brief      : lck报文发送函数,只发一次
 * @param[in ] : psess，session指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

static int cfm_send_lck_once(struct cfm_sess 	*psess)
{
	struct cfm_lck 		*plck;
	struct cfm_lck_pdu 	*plck_pdu;
	uint8_t		 		 i;
	struct raw_control   rawcb;
	uint8_t 			 mac[6] 	= {0x01, 0x80, 0xC2, 0x00, 0x00, 0x30};
	struct listnode  	*p_listnode = NULL;
	void 				*data 		= NULL;	
	struct cfm_ma 		*pma 		= NULL;
	uint8_t 			 p_mode 	= 0;	
    int 				 ret 		= 0;

	struct cfm_md		*lckmd = NULL;
	struct cfm_ma		*lckma = NULL;
	struct cfm_sess 	*lcksess = NULL;
	
	struct listnode 	*lck_listnode 	  = NULL; 
	struct listnode 	*lck_listnode_sess  = NULL;
	
	void				*lckma_data		= NULL;
	void				*lckdata_sess	= NULL;
	
		
    if((NULL == psess) || (NULL == psess->plck))
    {       	
		zlog_err("%s: %s, pointer error, psess:%p, psess->plck:%p,return!!\n",__FILE__,__func__,psess,psess->plck);
        return ERRNO_FAIL;
    }

	memset (&rawcb, 0, sizeof(struct raw_control));

	plck 		= psess->plck;
	plck_pdu 	= &plck->lck_pdu;

	if(ifm_get_mode(psess->ifindex,MODULE_ID_L2, &p_mode) != 0)
	{
		zlog_err("%s: %s,get the interface mode timeout",__FILE__,__func__);		
        return ERRNO_FAIL;
	}	

	pma = cfm_ma_lookup(psess->ma_index);
	if(!pma)
	{
		zlog_err("%s: %s,ma is null",__FILE__,__func__);		
        return ERRNO_FAIL;
	}
	
	rawcb.ethtype	 = 0x8902;
	rawcb.ttl		 = 64;
	rawcb.priority	 = pma->priority;

	//back direction
	for(i=0;i < 8;i++)
	{
		if(psess->lck_send_level & (0x01<<i))
		{
			plck_pdu->cfm_header.level = i;//level	
			
			zlog_debug(CFM_DBG_TEST, "%s:%d send lck level:%d\n\r", __FUNCTION__, __LINE__, i);
			
			mac[5] = 0x30 | i;  //level
			memcpy(plck_pdu->ethhdr.h_dest,  mac, 6);	

			lckmd = cfm_md_lookup_by_level(plck_pdu->cfm_header.level);
			if(!lckmd)
			{
				zlog_err("%s: %s,cfm_md_lookup_by_level fail,level:%d", __FILE__,__func__, plck_pdu->cfm_header.level);
				
				return ERRNO_FAIL;
			}
				
			for(ALL_LIST_ELEMENTS_RO(&lckmd->ma_list,lck_listnode,lckma_data))
			{
				lckma = (struct cfm_ma *)lckma_data;	
				if (NULL == lckma)
				{
					continue;
				}
				
				for(ALL_LIST_ELEMENTS_RO(&lckma->sess_list,lck_listnode_sess,lckdata_sess))
				{
					lcksess = (struct cfm_sess *)lckdata_sess;
					if (NULL == lcksess)
					{
						continue;
					}

					if(psess->direct != lcksess->direct)
					{
						continue;
					}
			
					if(lcksess->direct == MEP_DIRECT_DOWN)
					{
						if(ifm_get_mode(psess->ifindex,MODULE_ID_L2, &p_mode) != 0)
						{
							zlog_err("%s: %s,get the interface mode timeout",__FILE__,__func__);		
					        return ERRNO_FAIL;
						}	
	
						if(p_mode != IFNET_MODE_L2)
						{
							//for switch
							for(ALL_LIST_ELEMENTS_RO(&(vlan_table[lcksess->vlan]->portlist),p_listnode,data))
							{
								if(((uint32_t)data) != lcksess->ifindex)
								{				
									rawcb.out_ifindex = (uint32_t)data;
									rawcb.in_ifindex  = 0;							
									ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, plck_pdu, sizeof(struct cfm_lck_pdu)); 	
								}
							}
						}
						else
						{
							rawcb.in_ifindex  = lcksess->ifindex;
							rawcb.out_ifindex = 0;					
							ret	= pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, plck_pdu, sizeof(struct cfm_lck_pdu)); 	
						}
				
					}
					else
					{
						rawcb.out_ifindex = lcksess->ifindex;
						rawcb.in_ifindex  = 0;							
						ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, plck_pdu, sizeof(struct cfm_lck_pdu)); 	
					}
				}
			}
		}		
	}

	//same direction		
	for(i=0;i < 8;i++)
	{
		if(psess->lck_send_level & (0x01<<i))
		{
			plck_pdu->cfm_header.level	= i;//level		
			
			mac[5] = 0x30 | i;  //level
			memcpy(plck_pdu->ethhdr.h_dest,  mac, 6);

			lckmd = cfm_md_lookup_by_level(plck_pdu->cfm_header.level);
			if(!lckmd)
			{
				zlog_err("%s: %s,cfm_md_lookup_by_level fail,level:%d", __FILE__,__func__, plck_pdu->cfm_header.level);
				
				return ERRNO_FAIL;
			}
				
			for(ALL_LIST_ELEMENTS_RO(&lckmd->ma_list,lck_listnode,lckma_data))
			{
				lckma = (struct cfm_ma *)lckma_data;	
				if (NULL == lckma)
				{
					continue;
				}
				
				for(ALL_LIST_ELEMENTS_RO(&lckma->sess_list,lck_listnode_sess,lckdata_sess))
				{
					lcksess = (struct cfm_sess *)lckdata_sess;
					if (NULL == lcksess)
					{
						continue;
					}

					if(psess->direct != lcksess->direct)
					{
						continue;
					}			
			
					if (MEP_DIRECT_UP == lcksess->direct)
					{
						/* Only when for pw */
						if(lcksess->over_pw)
						{
							rawcb.in_ifindex  = lcksess->ifindex;
							rawcb.out_ifindex = 0;
							ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, &plck->lck_pdu, sizeof(struct cfm_lck_pdu));						
						}
						else
						{
							//for switch
							for(ALL_LIST_ELEMENTS_RO(&(vlan_table[lcksess->vlan]->portlist),p_listnode,data))
							{
								if(((uint32_t)data) != lcksess->ifindex)
								{				
									rawcb.out_ifindex = (uint32_t)data;
									rawcb.in_ifindex  = 0;							
									ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, &plck->lck_pdu, sizeof(struct cfm_lck_pdu));		
								}
							}
						}
					}
					else
					{
						rawcb.out_ifindex = lcksess->ifindex;
						rawcb.in_ifindex  = 0;				
						ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, &plck->lck_pdu, sizeof(struct cfm_lck_pdu));	
					}
				}
			}
		}
	}

	XFREE (MTYPE_TMP, plck);
	psess->plck = NULL;

	return ret;
}

/**
 * @brief      : lck发起函数,只发送一次
 * @param[in ] : sess，session指针； 
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */


static int cfm_lck_start_once(struct cfm_sess *psess)
{
	struct cfm_lck 		*plck;
	struct cfm_lck_pdu 	*plck_pdu;
	struct cfm_ma  		*pma;
		
    if(NULL == psess)
    {   
        return ERRNO_FAIL;
    }

	plck = psess->plck;
	if(plck == NULL)
	{
		plck = (struct cfm_lck *)XMALLOC(MTYPE_TMP, sizeof(struct cfm_lck));
		if (NULL == plck)
		{		
			return -1;
		}
		memset(plck,0,sizeof(struct cfm_lck));
		psess->plck = plck;
	}
	
	pma = cfm_ma_lookup(psess->ma_index);
	if(!pma)
        return ERRNO_FAIL;

	plck 	 = psess->plck;
	plck_pdu = &plck->lck_pdu;
	
	plck_pdu->end_tlv  = 0;

	//plck_pdu->cfm_header.level			 = 0;//fix
	plck_pdu->cfm_header.version			= 0;
	plck_pdu->cfm_header.opcode			 	= CFM_LCK_MESSAGE_OPCODE;
	plck_pdu->cfm_header.first_tlv_offset  	= 0;

	if(psess->lck_send_interval == 1)
	{
		plck_pdu->cfm_header.flag = 0x04;
	}
	else
	{
		plck_pdu->cfm_header.flag = 0x06;
	}


	memcpy(plck_pdu->ethhdr.h_source, psess->smac, 6);

	plck_pdu->ethhdr.h_tpid	   	 = htons(0x8100);
	plck_pdu->ethhdr.h_vlan_tci  = htons((pma->priority << 13) | psess->vlan);
	plck_pdu->ethhdr.h_ethtype   = htons(0x8902);

	cfm_send_lck_once(psess);

	return ERRNO_SUCCESS;

}

/**
 * @brief      : lck报文发送定时器
 * @param[in ] : thread，定时器指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

#if 0
int cfm_send_lck_timer(struct thread *thread)
{
	uint32_t   	  		 key;
	struct cfm_sess 	*psess;
	
	key 	= (uint32_t)THREAD_ARG(thread);		
	psess 	= cfm_session_lookup(key);

    if((NULL == psess) || (NULL == psess->plck))
    {       	
		zlog_err("%s: %s, pointer error, psess:%p, psess->plck:%p,return!!\n",__FILE__,__func__,psess,psess->plck);
        return ERRNO_FAIL;
    }

	cfm_send_lck(psess);
	
	return ERRNO_SUCCESS;
}
#endif

int cfm_send_lck_timer_out(void *para)
{
	uint32_t   	  		 key;
	struct cfm_sess 	*psess;
	
	key   = (uint32_t)para;		
	psess 	= cfm_session_lookup(key);

    if((NULL == psess) || (NULL == psess->plck))
    {       	
		zlog_err("%s: %s, pointer error, psess:%p, psess->plck:%p,return!!\n",__FILE__,__func__,psess,psess->plck);
        return ERRNO_FAIL;
    }

	cfm_send_lck(psess);
	
	return ERRNO_SUCCESS;
}

/**
 * @brief      : lck报文发送函数
 * @param[in ] : psess，session指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

int cfm_send_lck(struct cfm_sess 	*psess)
{
	struct cfm_lck 		*plck;
	struct cfm_lck_pdu 	*plck_pdu;
	struct raw_control   rawcb;
	uint8_t		 		 i;
	uint8_t 			 mac[6] 	= {0x01, 0x80, 0xC2, 0x00, 0x00, 0x30};
	struct listnode  	*p_listnode = NULL;
	void 				*data 		= NULL;	
	struct cfm_ma 		*pma 		= NULL;
	uint8_t 			 p_mode		= 0;	
    int 				 ret		= 0;

	struct cfm_md		*lckmd = NULL;
	struct cfm_ma		*lckma = NULL;
	struct cfm_sess 	*lcksess = NULL;
	
	struct listnode 	*lck_listnode 	  = NULL; 
	struct listnode 	*lck_listnode_sess  = NULL;
	
	void				*lckma_data		= NULL;
	void				*lckdata_sess	= NULL;
	
	char 				timerName[20] = "CfmLckSndTimer";
	
		
    if((NULL == psess) || (NULL == psess->plck))
    {       	
		zlog_err("%s: %s, pointer error, psess:%p, psess->plck:%p,return!!\n",__FILE__,__func__,psess,psess->plck);
        return ERRNO_FAIL;
    }

	memset (&rawcb, 0, sizeof(struct raw_control));

	plck 		= psess->plck;
	plck_pdu 	= &plck->lck_pdu;
	plck->plck_timer = 0;
	
	if(ifm_get_mode(psess->ifindex,MODULE_ID_L2, &p_mode) != 0)
	{
		zlog_err("%s: %s,get the interface mode timeout",__FILE__,__func__);		
		goto exit;
		//return ERRNO_FAIL;
	}	

	pma = cfm_ma_lookup(psess->ma_index);
	if(!pma)
	{
		zlog_err("%s: %s,ma is null",__FILE__,__func__);		
        return ERRNO_FAIL;
	}
	
	rawcb.ethtype	 = 0x8902;
	rawcb.ttl		 = 64;
	rawcb.priority	 = pma->priority;

	//back direction
	for(i=0;i < 8;i++)
	{
		if(psess->lck_send_level & (0x01<<i))
		{
			plck_pdu->cfm_header.level			 = i;//level		
			zlog_debug(CFM_DBG_TEST,"%s:%d send lck level:%d\n\r", __FUNCTION__, __LINE__, i);
			
			mac[5] = 0x30+i;  //level
			memcpy(plck_pdu->ethhdr.h_dest,  mac, 6);	

			/*bug56776*/
			lckmd = cfm_md_lookup_by_level(plck_pdu->cfm_header.level);
			if(!lckmd)
			{
				zlog_err("%s: %s,cfm_md_lookup_by_level fail,level:%d", __FILE__,__func__, plck_pdu->cfm_header.level);
				
				return ERRNO_FAIL;
			}
				
			for(ALL_LIST_ELEMENTS_RO(&lckmd->ma_list,lck_listnode,lckma_data))
			{
				lckma = (struct cfm_ma *)lckma_data;	
				if (NULL == lckma)
				{
					continue;
				}
				
				for(ALL_LIST_ELEMENTS_RO(&lckma->sess_list,lck_listnode_sess,lckdata_sess))
				{
					lcksess = (struct cfm_sess *)lckdata_sess;
					if (NULL == lcksess)
					{
						continue;
					}

					if(psess->direct != lcksess->direct)
					{
						continue;
					}

					if(lcksess->direct == MEP_DIRECT_DOWN)
					{
						if(ifm_get_mode(psess->ifindex,MODULE_ID_L2, &p_mode) != 0)
						{
							zlog_err("%s: %s,get the interface mode timeout",__FILE__,__func__);		
							goto exit;
						}	

						if(p_mode != IFNET_MODE_L2)
						{
							//for switch
							for(ALL_LIST_ELEMENTS_RO(&(vlan_table[lcksess->vlan]->portlist),p_listnode,data))
							{
								if(((uint32_t)data) != lcksess->ifindex)
								{				
									rawcb.out_ifindex = (uint32_t)data;
									rawcb.in_ifindex  = 0;							
									ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, plck_pdu, sizeof(struct cfm_lck_pdu)); 	
								}
							}
						}
						else
						{
							rawcb.in_ifindex  = lcksess->ifindex;
							rawcb.out_ifindex = 0;					
							ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, plck_pdu, sizeof(struct cfm_lck_pdu)); 	
						}
					
					}
					else
					{
						rawcb.out_ifindex = lcksess->ifindex;
						rawcb.in_ifindex  = 0;							
						ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, plck_pdu, sizeof(struct cfm_lck_pdu)); 	
					}
				}
			}
		}		
	}

	//same direction		
	for(i=0;i < 8;i++)
	{
		if(psess->lck_send_level & (0x01<<i))
		{
			plck_pdu->cfm_header.level	= i;//level		
			zlog_debug(CFM_DBG_TEST, "%s:%d send lck level:%d\n\r", __FUNCTION__, __LINE__, i);
			
			mac[5] = 0x30+i;  //level
			memcpy(plck_pdu->ethhdr.h_dest,  mac, 6);
			
			lckmd = cfm_md_lookup_by_level(plck_pdu->cfm_header.level);
			if(!lckmd)
			{
				zlog_err("%s: %s,cfm_md_lookup_by_level fail,level:%d", __FILE__,__func__, plck_pdu->cfm_header.level);
				
				return ERRNO_FAIL;
			}
				
			for(ALL_LIST_ELEMENTS_RO(&lckmd->ma_list,lck_listnode,lckma_data))
			{
				lckma = (struct cfm_ma *)lckma_data;	
				if (NULL == lckma)
				{
					continue;
				}
				
				for(ALL_LIST_ELEMENTS_RO(&lckma->sess_list,lck_listnode_sess,lckdata_sess))
				{
					lcksess = (struct cfm_sess *)lckdata_sess;
					if (NULL == lcksess)
					{
						continue;
					}

					if(psess->direct != lcksess->direct)
					{
						continue;
					}
					
					if (MEP_DIRECT_UP == lcksess->direct)
					{
						/* Only when for pw */
						if(lcksess->over_pw)
						{
							rawcb.in_ifindex  = lcksess->ifindex;
							rawcb.out_ifindex = 0;
							ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, &plck->lck_pdu, sizeof(struct cfm_lck_pdu));						
						}
						else
						{
							//for switch
							for(ALL_LIST_ELEMENTS_RO(&(vlan_table[lcksess->vlan]->portlist),p_listnode,data))
							{
								if(((uint32_t)data) != lcksess->ifindex)
								{				
									rawcb.out_ifindex = (uint32_t)data;
									rawcb.in_ifindex  = 0;							
									ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, &plck->lck_pdu, sizeof(struct cfm_lck_pdu));		
								}
							}
						}
					}
					else
					{
						rawcb.out_ifindex = lcksess->ifindex;
						rawcb.in_ifindex  = 0;				
						ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, &plck->lck_pdu, sizeof(struct cfm_lck_pdu));	
					}
				}
			}
		}
	}
	
	exit:	
	//plck->plck_timer = thread_add_timer(l2_master, cfm_send_lck_timer, (void *)(psess->local_mep|(psess->ma_index<<16)), psess->lck_send_interval);

	if (0 == plck->plck_timer)
	{
		plck->plck_timer = high_pre_timer_add(timerName, LIB_TIMER_TYPE_NOLOOP, cfm_send_lck_timer_out, 
				(void *)(psess->local_mep|(psess->ma_index<<16)), psess->lck_send_interval * 1000);

				
		zlog_debug(CFM_DBG_TEST, "%s:'%s', %s(%lu) add interval(%d)\n", 
					__FILE__, __func__, timerName, plck->plck_timer, psess->lck_send_interval);
	}
	
	return ret;
}

/**
 * @brief      : lck报文接收处理函数
 * @param[in ] : pkt，接收报文指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

int cfm_rcv_lck(struct pkt_buffer *pkt)
{
	struct cfm_lck_pdu  lck_pdu;
	char 				name[50];
    uint8_t         	interval; 
	struct cfm_sess 	*psess;
	struct cfm_ma 		*pma 		 = NULL;
	struct cfm_md 		*pmd 		 = NULL;
	struct listnode 	*p_listnode  = NULL, *p_listnode_sess   = NULL;
	void 				*data 		 = NULL, *data_sess 		= NULL;
	uint8_t 			dmac_multi[6]={0x01,0x80,0xc2,0x00,0x00,0x30};
	char timerName[20] = "CfmLckRcvTimer";
	uint8_t *p=NULL;
	int i;
	
	zlog_debug(CFM_DBG_TEST, "%s:Entering the function of '%s'",__FILE__,__func__);

	if(NULL == pkt)
	{
		return ERRNO_PARAM_ILLEGAL;
	}

	ifm_get_name_by_ifindex(pkt->in_ifindex,name);
	zlog_debug(CFM_DBG_TEST, "receive cfm lck from interface:%s, vpn:%d \n",name,pkt->vpn);

	ifm_get_name_by_ifindex(pkt->in_port,name);
	zlog_debug(CFM_DBG_TEST, "physical port is:%s\n",name);


	memset (&lck_pdu, 0, sizeof(struct cfm_lck_pdu));
	memcpy (&lck_pdu.cfm_header, pkt->data,pkt->data_len);

	p = (uint8_t *)pkt->data;
	zlog_debug(CFM_DBG_TEST, "receive data:");
	for (i = 0; i < 16; i++)
	{
		zlog_debug(CFM_DBG_TEST, "%02x", p[i]);
	}
	
	interval = (lck_pdu.cfm_header.flag & 0x02)?60:1;
	interval = (3*interval)+(interval>>1); //3.5倍检测周期
	
    zlog_debug(CFM_DBG_TEST, "receive lck level:%d\n\r", lck_pdu.cfm_header.level);
	
	pmd = cfm_md_lookup_by_level(lck_pdu.cfm_header.level);
	if(!pmd)
	{
		zlog_err("%s: %s,cfm_md_lookup_by_level fail,level:%d",__FILE__,__func__,lck_pdu.cfm_header.level);
		
		return ERRNO_FAIL;
	}

	//check multicast mac
	dmac_multi[5] += pmd->level;
	if(pkt->cb.ethcb.dmac[0]&0x01)
	{
		if(memcmp(pkt->cb.ethcb.dmac,dmac_multi,6))
		{
			zlog_debug(CFM_DBG_TEST, "target mac:%02X:%02X:%02X:%02X:%02X:%02X\n",pkt->cb.ethcb.dmac[0],
				pkt->cb.ethcb.dmac[1],
				pkt->cb.ethcb.dmac[2],
				pkt->cb.ethcb.dmac[3],
				pkt->cb.ethcb.dmac[4],
				pkt->cb.ethcb.dmac[5]);
			
			zlog_debug(CFM_DBG_TEST, "multicast dmac:%02X:%02X:%02X:%02X:%02X:%02X\n",dmac_multi[0],
				dmac_multi[1],
				dmac_multi[2],
				dmac_multi[3],
				dmac_multi[4],
				dmac_multi[5]);
		
			zlog_err("%s:md %d receive lck but not match",__func__,pmd->md_index);
			return ERRNO_UNMATCH;		
		}
	}

	for(ALL_LIST_ELEMENTS_RO(&pmd->ma_list,p_listnode,data))
	{
		pma = (struct cfm_ma *)data;			
		for(ALL_LIST_ELEMENTS_RO(&pma->sess_list,p_listnode_sess,data_sess))
		{
			psess = (struct cfm_sess *)data_sess;

			ifm_get_name_by_ifindex(psess->ifindex,name);
			zlog_debug(CFM_DBG_TEST, "session port:%s", name);
			
			//check unicast mac
			if(!(pkt->cb.ethcb.dmac[0]&0x01))
			{
				if(memcmp(pkt->cb.ethcb.dmac,psess->smac,6))
				{	
					zlog_debug(CFM_DBG_TEST, "target mac:%02X:%02X:%02X:%02X:%02X:%02X\n",pkt->cb.ethcb.dmac[0],
						pkt->cb.ethcb.dmac[1],
						pkt->cb.ethcb.dmac[2],
						pkt->cb.ethcb.dmac[3],
						pkt->cb.ethcb.dmac[4],
						pkt->cb.ethcb.dmac[5]);
					
					zlog_debug(CFM_DBG_TEST, "smac:%02X:%02X:%02X:%02X:%02X:%02X\n",psess->smac[0],
						psess->smac[1],
						psess->smac[2],
						psess->smac[3],
						psess->smac[4],
						psess->smac[5]);
				
					zlog_err("%s:session %d receive ais but not match",__func__,psess->sess_id);
					continue;		
				}
				
			}

			//check ifindex
			if(psess->direct == MEP_DIRECT_DOWN || (psess->direct == MEP_DIRECT_UP && psess->over_pw))
			{
				if(pkt->in_ifindex != psess->ifindex)
				{
					zlog_err("%s:session %d receive lck but interface not match",__func__,psess->sess_id);
					continue;		
				}
			}
			else
			{
				if(pkt->in_ifindex == psess->ifindex)
				{
					zlog_err("%s:normal mode, session %d receive lck but interface same",__func__,psess->sess_id);
					continue;		
				}
			}
			
		    if(psess->alarm.alarm_lck == 0)
			{
				psess->alarm.alarm_lck  = 1;
				cfm_session_alarm_report(psess,IPC_OPCODE_ADD,GPN_ALM_TYPE_FUN_CFM_LCK);
			}	
			
			psess->lck_rcv_interval = interval;
			if(0 == psess->plck_timer)
			{
				//psess->plck_timer = thread_add_timer(l2_master, cfm_lck_timer, (void *)(psess->local_mep|(psess->ma_index<<16)), interval);
				psess->plck_timer = high_pre_timer_add(timerName, LIB_TIMER_TYPE_NOLOOP, cfm_lck_rcv_timer_out, 
							(void *)(psess->local_mep|(psess->ma_index<<16)), interval * 1000);
			}

			zlog_debug(CFM_DBG_TEST, "session %d in ma %d,alarm lck set \n",psess->sess_id,psess->ma_index);	
			
		}
	}

	return ERRNO_SUCCESS;
}

/**
 * @brief      : lck发起函数,周期发送
 * @param[in ] : psess，session指针； 
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */


static int cfm_lck_start(struct cfm_sess *psess)
{
	struct cfm_lck 		*plck;
	struct cfm_lck_pdu  *plck_pdu;
	struct cfm_ma  		*pma;
		
    if(NULL == psess)
    {   
        return ERRNO_FAIL;
    }

	plck = psess->plck;
	if(plck == NULL)
	{
		plck = (struct cfm_lck *)XMALLOC(MTYPE_TMP, sizeof(struct cfm_lck));
		if (NULL == plck)
		{		
			return -1;
		}
		memset(plck,0,sizeof(struct cfm_lck));
		psess->plck = plck;
	}
	
	pma = cfm_ma_lookup(psess->ma_index);
	if(!pma)
        return ERRNO_FAIL;

	plck 	 = psess->plck;
	plck_pdu = &plck->lck_pdu;
	
	plck_pdu->end_tlv  = 0;

	//plck_pdu->cfm_header.level			 = 0;//fix
	plck_pdu->cfm_header.version		  = 0;
	plck_pdu->cfm_header.opcode			  = CFM_LCK_MESSAGE_OPCODE;
	plck_pdu->cfm_header.first_tlv_offset = 0;

	if(psess->lck_send_interval == 1)
	{
		plck_pdu->cfm_header.flag = 0x04;
	}
	else
	{
		plck_pdu->cfm_header.flag = 0x06;
	}


	memcpy(plck_pdu->ethhdr.h_source, psess->smac, 6);

	plck_pdu->ethhdr.h_tpid	     = htons(0x8100);
	plck_pdu->ethhdr.h_vlan_tci  = htons((pma->priority << 13) | psess->vlan);
	plck_pdu->ethhdr.h_ethtype   = htons(0x8902);

	cfm_send_lck(psess);
	
	return ERRNO_SUCCESS;

}

/**
 * @brief      : lck结束函数
 * @param[in ] : psess，session指针； 
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

static int cfm_lck_stop(struct cfm_sess *psess)
{
	struct cfm_lck *plck;

		
    if(NULL == psess)
    {   
        return ERRNO_FAIL;
    }

	plck = psess->plck;

	if (plck)
	{
#if 0
		if(plck->plck_timer)//cancle the timer
		{
			THREAD_TIMER_OFF(plck->plck_timer);
			plck->plck_timer = NULL;
		}
#endif

		if(plck->plck_timer)//cancle the timer
		{
			zlog_debug(CFM_DBG_TEST, "delete lck timer\n");
			high_pre_timer_delete(plck->plck_timer);
			plck->plck_timer = 0;
		}
		
		XFREE (MTYPE_TMP, plck);
		psess->plck = NULL;
		
	}

	return ERRNO_SUCCESS;

}

/**
 * @brief      : test报文发送定时器
 * @param[in ] : thread，定时器指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */
#if 0
int cfm_send_test_timer(struct thread *thread)
{
	uint32_t   	  	 key;
	struct cfm_sess *psess;
	
	key   = (uint32_t)THREAD_ARG(thread);		
	psess = cfm_session_lookup(key);

    if((NULL == psess) || (NULL == psess->ptest))
    {       	
		zlog_err("%s: %s, pointer error, psess:%p, psess->ptest:%p,return!!\n",__FILE__,__func__,psess,psess->ptest);
        return ERRNO_FAIL;
    }

	cfm_send_test(psess);

	return ERRNO_SUCCESS;
}
#endif

static int cfm_send_test_timer_out(void *para)
{
	uint32_t   	  	 key;
	struct cfm_sess *psess;
	
	key   = (uint32_t)para;		
	psess = cfm_session_lookup(key);

    if((NULL == psess) || (NULL == psess->ptest))
    {       	
		zlog_err("%s: %s, pointer error, psess:%p, psess->ptest:%p,return!!\n",__FILE__,__func__,psess,psess->ptest);
        return ERRNO_FAIL;
    }

	cfm_send_test(psess);

	return ERRNO_SUCCESS;
}

/**
 * @brief      : crc32 计算函数
 * @param[in ] : crc，校验码初始值
 * @param[in ] : pdata，待校验数据指针
 * @param[in ] : size，待校验数据长度
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */


static uint32_t make_crc(uint32_t crc, unsigned char *pdata, uint32_t size)  
{  
  
    while(size--)  
        crc = (crc >> 8)^(crc32_table[(crc ^ *pdata++)&0xff]);  
  
    return crc;  
}  

/**
 * @brief      : prbs31计算函数
 * @param[in ] : pdata，待计算数据指针
 * @param[in ] : num，待计算数据长度
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

static int prbs31_get(uchar *pdata,uint32_t num)
{
	unsigned int    i,N;
	unsigned char  *zita;
	unsigned char   ii=0;
	unsigned int   	init;//use only high 30 bits
	struct	timeval tv;



	if(pdata == NULL || num == 0)
	{
		zlog_err("pdata:%p,num:%d\n",pdata,num);
		return ERRNO_FAIL;
	}

	//prbs bits count
	N = num*8;

	//input a radom value
	gettimeofday(&tv,NULL);
	init = tv.tv_usec+1;

	zita = XCALLOC(MTYPE_TMP,N+1);
	if(zita==NULL)
	{
		zlog_err("zita is NULL\n");
		return ERRNO_FAIL;		
	}
	
	//init zita[i]  
	zita[0] = 0;//the 31st  of feedback shift registers
	for(i=1;i<=30;i++) 
	{	 
		//i=1 is the first send bit ,also the 30th of feedback shift registers,also highest bit of init value
	   zita[i] = (init>>(32-i))&0x01; 
	}
	
	for(i=31;i<=N;i++)
	{
		zita[i] = zita[i-31]^zita[i-28];//X31+X28+1
	}

	memset(pdata,0,num);

	//prbs bits to bytes
	for(i=0;i<num;i++)
	{
	   for(ii=1;ii<=8;ii++)
	   {		   
		   pdata[i]|=zita[i*8+ii]&0x01;

		   if(ii<8)
		   pdata[i]=pdata[i]<<1;		   
	   }
	}

	XFREE(MTYPE_TMP,zita);
	
	return ERRNO_SUCCESS;

}

/**
 * @brief      : prbs31校验函数
 * @param[in ] : pdata，待校验数据指针
 * @param[in ] : num，待校验数据长度
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

static int prbs31_check(uchar *pdata,uint32_t num)
{
	unsigned int 	i,N;
	unsigned char  *zita;
	unsigned char 	c1,c2;
	signed char 	ii   = 0;
	unsigned int 	init = 0;//use only high 30 bits

	zlog_debug(CFM_DBG_TEST, "%s:Entering the function of '%s',num:%d",__FILE__,__func__,num);

	if(pdata == NULL || num == 0)
	{
		zlog_err("pdata:%p,num:%d\n",pdata,num);
		return ERRNO_FAIL;
	}

	for(i=0;i<4;i++)
	{
		for(ii=7;ii>=0;ii--)
		{
			init = init<<1;
			init|= (pdata[i]>>ii)&0x01;
		}
	}

	zlog_debug(CFM_DBG_TEST, "init:0x%08x\n",init);

	//prbs bits count
	N    = num*8;

	zita = XCALLOC(MTYPE_TMP,N+1);
	if(zita == NULL)
	{
		zlog_err("zita is NULL\n");
		return ERRNO_FAIL;		
	}
	
	//init zita[i]  
	zita[0] = 0;//the 31st  of feedback shift registers
	for(i=1;i<=30;i++) 
	{	 
		//i=1 is the first send bit ,also the 30th of feedback shift registers,also highest bit of init value
	   zita[i] = (init>>(32-i))&0x01; 
	}
	
	for(i=31;i<=N;i++)
	{
		zita[i] = zita[i-31]^zita[i-28];//X31+X28+1
	}


	//compare
	for(i=0;i<num;i++)
	{
	   for(ii=1;ii<=8;ii++)
	   {		   
		   c1 = zita[i*8+ii]&0x01;
		   c2 = (pdata[i]>>(8-ii))&0x01;

		   if(c1 != c2)
		   {
			   zlog_err("i:%d,ii:%d,c1:%d,c2:%d\n",i,ii,c1,c2);
			   
			   XFREE(MTYPE_TMP,zita);
			   return ERRNO_FAIL;
		   }
	   }
	}

	XFREE(MTYPE_TMP,zita);

	return ERRNO_SUCCESS;

}

/**
 * @brief      : 全零码校验函数
 * @param[in ] : pdata，待校验数据指针
 * @param[in ] : num，待校验数据长度
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

static int zeros_check(uchar *pdata,uint32_t num)
{
	unsigned int i;

	zlog_debug(CFM_DBG_TEST, "%s:Entering the function of '%s',num:%d",__FILE__,__func__,num);

	if(pdata == NULL || num == 0)
	{
		zlog_err("pdata:%p,num:%d\n",pdata,num);
		return ERRNO_FAIL;
	}

	for(i=0; i<num; i++)
	{
		if(pdata[i] != 0)
		{
			zlog_err("i:%d,pdata[i]:%d\n",i,pdata[i]);
			return ERRNO_FAIL;			
		}
	}	

	return ERRNO_SUCCESS;

}

/**
 * @brief      : test报文发送函数
 * @param[in ] : psess，session指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

int cfm_send_test(struct cfm_sess 	*psess)
{
    int 				ret 		= 0;
	struct listnode  	*p_listnode = NULL;
	void 				*data 		= NULL;	
	struct cfm_ma 		*pma 		= NULL;
	uint8_t 			 p_mode		= 0;		
	unsigned char 		*p 			= NULL;
	unsigned int 		crc			= 0xffffffff;
	struct raw_control  rawcb;
	struct cfm_test 	*ptest;
	struct cfm_test_pdu *ptest_pdu;
	char timerName[20] = "CfmTestTimer";
		
    if((NULL == psess) || (NULL == psess->ptest))
    {       	
		zlog_err("%s: %s, pointer error, psess:%p, psess->ptest:%p,return!!\n",__FILE__,__func__,psess,psess->ptest);
        return ERRNO_FAIL;
    }

	memset (&rawcb, 0, sizeof(struct raw_control));

	ptest 		= psess->ptest;
	ptest_pdu 	= &ptest->test_pdu;
	ptest->ptest_timer	= 0;

	p    = ptest_pdu->tlvs;
	*p++ = 32; //test tlv
	*p++ = (ptest->tst_len+1)>>8; 
	*p++ = ptest->tst_len+1; 
	*p++ = ptest->tst_pattern; 
	if(ptest->tst_pattern == prbs_with_crc32)
	{
		prbs31_get(p,ptest->tst_len-4);	
		
		crc = make_crc(crc,p-4,ptest->tst_len);//begain from type,tst_len-4+4

		p+=ptest->tst_len-4;
		
		*p++=crc>>24;
		*p++=crc>>16;
		*p++=crc>>8;
		*p++=crc;
		
	}
	else if(ptest->tst_pattern == prbs_without_crc32)
	{
		prbs31_get(p,ptest->tst_len);	
		p+=ptest->tst_len;
	}
	else if(ptest->tst_pattern == zero_with_crc32)
	{	
		memset(p,0,ptest->tst_len-4);

		crc = make_crc(crc,p-4,ptest->tst_len);//begain from type,tst_len-4+4
		
		p+=ptest->tst_len-4;
		
		*p++=crc>>24;
		*p++=crc>>16;
		*p++=crc>>8;
		*p++=crc;
	}
	else//zero_without_crc32
	{	
		memset(p,0,ptest->tst_len);
		p+=ptest->tst_len;
	}
	
	zlog_debug(CFM_DBG_TEST, "crc:%08X,pattern:%d\n",crc,ptest->tst_pattern);

	
	//endtlv
	*p++ = 0;

	ptest->remain_len = ptest->test_pdu.tlvs+1500-p;

	if(ifm_get_mode(psess->ifindex,MODULE_ID_L2, &p_mode) != 0)
	{
		zlog_err("%s: %s,get the interface mode timeout",__FILE__,__func__);		
        return ERRNO_FAIL;
	}	

	pma = cfm_ma_lookup(psess->ma_index); 
	if(!pma)
	{
		zlog_err("%s: %s,ma is null",__FILE__,__func__);		
        return ERRNO_FAIL;
	}

	rawcb.ethtype	 = 0x8902;
	rawcb.ttl		 = 64;
	rawcb.priority	 = pma->priority;
	ptest_pdu->seq   = htonl(psess->tst_next_trans_id);
			
	if (MEP_DIRECT_UP == psess->direct)
	{
		/* Only when for pw */
		if(psess->over_pw)
		{
			rawcb.in_ifindex = psess->ifindex;
			ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, &ptest->test_pdu, sizeof(struct cfm_test_pdu) - ptest->remain_len);	
		}
		else
		{
			//for switch
			for(ALL_LIST_ELEMENTS_RO(&(vlan_table[psess->vlan]->portlist),p_listnode,data))
			{
				if(((uint32_t)data) != psess->ifindex)
				{				
					rawcb.out_ifindex = (uint32_t)data;
					ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, &ptest->test_pdu, sizeof(struct cfm_test_pdu) - ptest->remain_len);		
				}
			}
		}
	}
	else
	{
		rawcb.out_ifindex = psess->ifindex;
		ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, &ptest->test_pdu, sizeof(struct cfm_test_pdu) - ptest->remain_len);	
		
	}	

	ptest->tst_tx_count--;
	psess->tst_next_trans_id++;

	zlog_debug(CFM_DBG_TEST, "%s: pkt count(%d) not send\n", __func__, ptest->tst_tx_count);
	
	if(ptest->tst_tx_count == 0)
	{
	
		if(psess->tst_mode == 1)
		{
			cfm_lck_stop(psess);
		}
		
		psess->tst_enable	= 0;
		ptest->ptest_timer	= 0;
		
		XFREE (MTYPE_TMP, ptest);
		psess->ptest 	   = NULL;
		
		return ERRNO_SUCCESS;
	}
		
	//ptest->ptest_timer = thread_add_timer(l2_master, cfm_send_test_timer, (void *)(psess->local_mep|(psess->ma_index<<16)), ptest->tst_tx_interval);

	ptest->ptest_timer = high_pre_timer_add(timerName, LIB_TIMER_TYPE_NOLOOP, 
				cfm_send_test_timer_out, (void *)(psess->local_mep|(psess->ma_index<<16)), ptest->tst_tx_interval * 1000);
	
	zlog_debug(CFM_DBG_TEST, "%s:'%s', %s(%lu) add interval(%d)\n", 
				__FILE__, __func__, timerName, ptest->ptest_timer, psess->lck_send_interval);
	
	return ret;
}

/**
 * @brief      : test报文接收处理函数
 * @param[in ] : psess，session指针
 * @param[in ] : pkt，接收报文指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

int cfm_rcv_test(struct cfm_sess *psess,struct pkt_buffer *pkt)
{
	struct cfm_test_pdu  	test_pdu;
	char 				 	name[50];	
	enum cfm_tst_pattern 	tst_pattern;
	uint32_t        		tst_len; 
	uint8_t 				dmac_multi[6] = {0x01,0x80,0xc2,0x00,0x00,0x30};
	
	zlog_debug(CFM_DBG_TEST, "%s:Entering the function of '%s',psess:%p",__FILE__,__func__,psess);

	if(NULL == psess || NULL == pkt)
	{
		zlog_err("psess:%p,pkt:%p\n",psess,pkt);
		return ERRNO_PARAM_ILLEGAL;
	}

	//check mac
	dmac_multi[5] += psess->level;	
	if(pkt->cb.ethcb.dmac[0]&0x01)
	{
		if(memcmp(pkt->cb.ethcb.dmac,dmac_multi,6))
		{
			zlog_debug(CFM_DBG_TEST, "target mac:%02X:%02X:%02X:%02X:%02X:%02X\n",pkt->cb.ethcb.dmac[0],
				pkt->cb.ethcb.dmac[1],
				pkt->cb.ethcb.dmac[2],
				pkt->cb.ethcb.dmac[3],
				pkt->cb.ethcb.dmac[4],
				pkt->cb.ethcb.dmac[5]);
			
			zlog_debug(CFM_DBG_TEST, "multicast dmac:%02X:%02X:%02X:%02X:%02X:%02X\n",dmac_multi[0],
				dmac_multi[1],
				dmac_multi[2],
				dmac_multi[3],
				dmac_multi[4],
				dmac_multi[5]);
		
			zlog_err("%s:session %d receive test but not match",__func__,psess->sess_id);
			return ERRNO_UNMATCH;		
		}
	}
	else
	{
		if(memcmp(pkt->cb.ethcb.dmac,psess->smac,6))
		{	
			zlog_debug(CFM_DBG_TEST, "target mac:%02X:%02X:%02X:%02X:%02X:%02X\n",pkt->cb.ethcb.dmac[0],
				pkt->cb.ethcb.dmac[1],
				pkt->cb.ethcb.dmac[2],
				pkt->cb.ethcb.dmac[3],
				pkt->cb.ethcb.dmac[4],
				pkt->cb.ethcb.dmac[5]);
			
			zlog_debug(CFM_DBG_TEST, "smac:%02X:%02X:%02X:%02X:%02X:%02X\n",psess->smac[0],
				psess->smac[1],
				psess->smac[2],
				psess->smac[3],
				psess->smac[4],
				psess->smac[5]);
		
			zlog_err("%s:session %d receive test but not match",__func__,psess->sess_id);
			return ERRNO_UNMATCH;		
		}
	}


	ifm_get_name_by_ifindex(pkt->in_ifindex,name);
	zlog_debug(CFM_DBG_TEST, "receive cfm test from interface:%s, vpn:%d \n",name,pkt->vpn);

	ifm_get_name_by_ifindex(pkt->in_port,name);
	zlog_debug(CFM_DBG_TEST, "physical port is:%s\n",name);

	//check ifindex
	if(psess->direct == MEP_DIRECT_DOWN || (psess->direct == MEP_DIRECT_UP && psess->over_pw))
	{
		if(pkt->in_ifindex != psess->ifindex)
		{
			zlog_err("%s:session %d receive test but interface not match",__func__,psess->sess_id);
			return ERRNO_UNMATCH;		
		}
	}
	else
	{
		if(pkt->in_ifindex == psess->ifindex)
		{
			zlog_err("%s:normal mode, session %d receive test but interface same",__func__,psess->sess_id);
			return ERRNO_UNMATCH;		
		}
	}


	memset (&test_pdu, 0, sizeof(struct cfm_test_pdu));
	pkt->data = (char *)pkt + pkt->data_offset;
	memcpy (&test_pdu.cfm_header, pkt->data,pkt->data_len);

	test_pdu.seq = ntohl(test_pdu.seq);

	if(test_pdu.seq && (test_pdu.seq == psess->tst_lastid))
	{
		zlog_err("rcv test id not match,rx id:%d,last id:%d\n",test_pdu.seq,psess->tst_lastid);
		return ERRNO_FAIL;
	}

	psess->tst_rxnum++;
	
	if(psess->tst_mode == 1)
	{
		cfm_lck_start_once(psess);
	}

	tst_pattern = test_pdu.tlvs[3];
	tst_len 	= ((test_pdu.tlvs[1]<<8) | test_pdu.tlvs[2])-1;
	if(tst_pattern == prbs_without_crc32)
	{
		if(prbs31_check(test_pdu.tlvs+4,tst_len))
		{
			psess->tst_errnum++;
		}
	}
	else if(tst_pattern == prbs_with_crc32 )
	{
		if(prbs31_check(test_pdu.tlvs+4,tst_len-4))
		{
			psess->tst_errnum++;
		}			
	}
	else if(tst_pattern == zero_without_crc32)
	{
		if(zeros_check(test_pdu.tlvs+4,tst_len))
		{
			psess->tst_errnum++;
		}
	}
	else if(tst_pattern == zero_with_crc32)
	{
		if(zeros_check(test_pdu.tlvs+4,tst_len-4))
		{
			psess->tst_errnum++;
		}			
	}

	psess->tst_lastid = test_pdu.seq;

	zlog_debug(CFM_DBG_TEST, "session %d in ma %d,rcv test pdu,id:%d \n",psess->sess_id,psess->ma_index,test_pdu.seq);	
	
	return ERRNO_SUCCESS;	
}

/**
 * @brief      : test发起函数
 * @param[in ] : psess，session指针； 
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

int cfm_test_start(struct cfm_sess *psess)
{
	struct cfm_test 	*ptest;
	struct cfm_test_pdu *ptest_pdu;
	struct cfm_ma  		*pma;
	
    if((NULL == psess) || (NULL == psess->ptest))
    {   
		zlog_err("%s: %s, pointer error, psess:%p, psess->ptest:%p,return!!\n",__FILE__,__func__,psess,psess->ptest);
    
        return ERRNO_FAIL;
    }

	pma = cfm_ma_lookup(psess->ma_index);
	if(!pma)
	{
		zlog_err("%s: %s,ma is null",__FILE__,__func__);		
        return ERRNO_FAIL;
	}

	ptest 		= psess->ptest;
	ptest_pdu 	= &ptest->test_pdu;


	memset(ptest_pdu,0,sizeof(struct cfm_test_pdu));
	
	ptest_pdu->cfm_header.level			 	= psess->level;//fix
	ptest_pdu->cfm_header.version		 	= 0;
	ptest_pdu->cfm_header.opcode			= CFM_TEST_MESSAGE_OPCODE;
	ptest_pdu->cfm_header.flag 				= 0;	
	ptest_pdu->cfm_header.first_tlv_offset  = 4;


	memcpy(ptest_pdu->ethhdr.h_source, psess->smac, 6);
	memcpy(ptest_pdu->ethhdr.h_dest, ptest->tst_dmac, 6);

	ptest_pdu->ethhdr.h_tpid	   = htons(0x8100);
	ptest_pdu->ethhdr.h_vlan_tci   = htons((pma->priority << 13) | psess->vlan);
	
	ptest_pdu->ethhdr.h_ethtype    = htons(0x8902);

	cfm_send_test(psess);
	if(psess->tst_mode == 1)
	{
		cfm_lck_start(psess);
	}

	return ERRNO_SUCCESS;
}

/**
 * @brief      : test结束函数
 * @param[in ] : psess，session指针； 
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

int cfm_test_stop(struct cfm_sess *psess)
{
	struct cfm_test *ptest;

	zlog_info("%s: %s\n",__FILE__,__func__);

		
    if(NULL == psess)
    {   
        return ERRNO_FAIL;
    }

	if(psess->tst_mode == 1)
	{
		cfm_lck_stop(psess);
	}
	
	psess->tst_enable = 0;
	
	ptest = psess->ptest;
	if (ptest )
	{
#if 0
		if(ptest->ptest_timer)//cancle the timer
		{
			THREAD_TIMER_OFF(ptest->ptest_timer);
			ptest->ptest_timer = NULL;
		}
#endif

		if(ptest->ptest_timer)//cancle the timer
		{
			high_pre_timer_delete(ptest->ptest_timer);
			ptest->ptest_timer = 0;
		}

		XFREE (MTYPE_TMP, ptest);
		psess->ptest = NULL;
		
	}

	return ERRNO_SUCCESS;

}

/**
 * @brief      : crc32表格初始化
 * @param[in ] :  
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

int make_crc32_table()  
{  
    uint32_t c;  
    int 	 i   = 0;  
    int 	 bit = 0;  
      
    for(i = 0; i < 256; i++)  
    {  
        c  = (uint32_t)i;  
          
        for(bit = 0; bit < 8; bit++)  
        {  
            if(c&1)  
            {  
                c = (c >> 1)^(0xEDB88320);  
            }  
            else  
            {  
                c =  c >> 1;  
            }  
              
        }  
        crc32_table[i] = c;  
    }  
      
  return ERRNO_SUCCESS;
}  


