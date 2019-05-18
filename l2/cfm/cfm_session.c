/**
 * @file      : cfm_session.c
 * @brief     : define of 802.1ag and Y.1731 mep session
 * @details   : 
 * @author    : huoqq
 * @date      : 2018年3月23日 14:34:04
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
#include <lib/log.h>

#include "cfm_session.h"
#include "l2_if.h"

#include "cfm.h"
#include "cfm_lb.h"
#include "cfm_lt.h"
#include "cfm_test.h"
#include "aps/elps_pkt.h"
#include "aps/elps.h"
#include "raps/erps.h"
#include "raps/erps_fsm.h"

#include <lib/zassert.h>
#include "../vlan.h"

extern struct thread_master *l2_master;

struct hash_table cfm_sess_table;     /* CFM session hash 表，以 local_mep 为 hash key */
struct hash_table cfm_sess_table_new;     /* CFM session hash 表，以 session id 为 hash key */

#define CFM_IF_INIT_FLAG      0
#define CFM_IF_DEL_FLAG       1
#define CFM_IF_ADD_FLAG       2
#define CFM_IF_RECONFIG_FLAG  3

/**
 * @brief      : session哈希表key值计算
 * @param[in ] : hash_key, ma key值
 * @param[out] : 
 * @return     : 哈希key
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

static unsigned int compute_sess(void *hash_key)
{
    if ( NULL == hash_key )
    {
        return 0;
    }
    return ( ( unsigned int ) hash_key );
}

/**
 * @brief      : session哈希表key值比较
 * @param[in ] : item, 哈希节点 
 * @param[in ] : hash_key, session key值
 * @param[out] : 
 * @return     : 比较相等返回0，不相等返回1
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

static int compare_sess(void *item, void *hash_key)
{    
    struct hash_bucket *pbucket = item;

    if ( NULL == pbucket || NULL == hash_key )
    {
        return 1;
    }

    if ( pbucket->hash_key == hash_key )
    {
        return 0;
    }
    else
    { 
    	return 1; 
	}
}

/**
 * @brief      : session哈希表初始化
 * @param[in ] : uiSize, 哈希节点最大个数 
 * @param[out] : 
 * @return     : 
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

void 	cfm_session_table_init(unsigned int uiSize)
{
    if (0 == uiSize)
    {
        zlog_err("size must great than or equal to zero!\n");
        return ;
    }   
    hios_hash_init(&cfm_sess_table, uiSize, compute_sess, compare_sess);
    hios_hash_init(&cfm_sess_table_new, uiSize, compute_sess, compare_sess);
	
    return ;

}

/**
 * @brief      : 创建一个 session
 * @param[in ] : sess_id, session索引 
 * @param[out] : 
 * @return     : 成功返回新创建的session指针，失败返回NULL
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

struct cfm_sess 	*cfm_session_create(uint16_t sess_id)
{
	struct cfm_sess *psess;
	
	zlog_debug(CFM_DBG_COMMN,"%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);

	psess = (struct cfm_sess *)XMALLOC(MTYPE_CFM_SESS, sizeof(struct cfm_sess));
	if(NULL == psess)
	{
		return NULL;
	}
	memset(psess,0,sizeof(struct cfm_sess));

	psess->sess_id= sess_id;
	psess->direct   = MEP_DIRECT_DOWN;
	
	return psess;

}

/**
 * @brief      : 创建一个 session sla
 * @param[in ] : sess_id, session索引 
 * @param[out] : 
 * @return     : 成功返回新创建的session sla指针，失败返回NULL
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

struct sla_measure *cfm_session_create_sla(struct cfm_sess *psess)
{
    if((NULL == psess) || (psess->psla != NULL))
    {
        return NULL;
    }

    psess->psla = (struct sla_measure *)XMALLOC(MTYPE_CFM_SESS, sizeof(struct sla_measure));
    if(NULL == psess->psla)
    {   
        return NULL;
    }
    memset(psess->psla, 0, sizeof(struct sla_measure));
	psess->psla->sess_id   = psess->sess_id;
	psess->psla->ma_index  = psess->ma_index;
	psess->psla->local_mep = psess->local_mep;

    return psess->psla;
}

/**
 * @brief      : 添加 session 到 hash 表，与hal交互
 * @param[in ] : psess, session指针 
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

int 	cfm_session_add(struct cfm_sess *psess) 
{
	struct hash_bucket *sess_item = NULL;
	int 				ret;

	zlog_debug(CFM_DBG_COMMN,"%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
	
	//new
	sess_item = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if(NULL == sess_item)
	{
		return ERRNO_MALLOC;
	}
	
	memset(sess_item, 0, sizeof(struct hash_bucket));
	
	sess_item->data 	= psess;
	sess_item->hash_key = (void *)(psess->ma_index<<16 | psess->local_mep);
	
	ret = hios_hash_add(&cfm_sess_table, sess_item);
	if(ret != 0)
	{
		XFREE(MTYPE_HASH_BACKET, sess_item);
		return ERRNO_OVERSIZE;
	}

	//ipc_send_hal( psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, CFM_INFO_SESS_ADD, 0, psess->local_mep);

	return ERRNO_SUCCESS;

}

/**
 * @brief      : 添加 session 到 hash 表，与命令行交互
 * @param[in ] : psess, session指针 
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

int 	cfm_session_add_new(struct cfm_sess *psess)
{
	struct hash_bucket *sess_item = NULL;
	int 				ret;

	zlog_debug(CFM_DBG_COMMN,"%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
	
	//new
	sess_item = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if(NULL == sess_item)
	{
		return ERRNO_MALLOC;
	}
	
	memset(sess_item, 0, sizeof(struct hash_bucket));
	
	sess_item->data 	= psess;
	sess_item->hash_key = (void *)(uint32_t)psess->sess_id;
	
	ret = hios_hash_add(&cfm_sess_table_new, sess_item);
	if(ret != 0)
	{
		XFREE(MTYPE_HASH_BACKET, sess_item);
		return ERRNO_OVERSIZE;
	}

	//ipran_alarm_port_register(IFM_CFM_MEP_TYPE,0,0,psess->sess_id,0);
	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_CFM_MEP_TYPE;
	gPortInfo.iMsgPara1 = psess->sess_id;
	ipran_alarm_port_register(&gPortInfo);
	

	//ipc_send_hal( psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, CFM_INFO_SESS_ADD, 0, psess->local_mep);

	return ERRNO_SUCCESS;

}

/**
 * @brief      : 删除一个 session，与hal交互的哈希表
 * @param[in ] : key, key值 
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

int 	cfm_session_delete(uint32_t key)
{
	struct hash_bucket 		*sess_item = NULL;
	struct cfm_sess			*sess_node = NULL;

	zlog_debug(CFM_DBG_COMMN,"%s:Entering the function of '%s',key:%08X",__FILE__,__func__,key);


	sess_item = hios_hash_find(&cfm_sess_table, (void *)key);


	if(sess_item)
	{	
		sess_node = sess_item->data;
	
		hios_hash_delete(&cfm_sess_table, sess_item);
		XFREE(MTYPE_HASH_BACKET, sess_item);
		
		//elps_state_update(sess_node->ifindex,sess_node->local_mep,CC_STATE_UP);
		
		cfm_session_clear_alarm(sess_node);		

	}	

	return ERRNO_SUCCESS;

}

/**
 * @brief      : 删除一个 session，与命令行交互的哈希表
 * @param[in ] : sess_id，session id
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

int 	cfm_session_delete_new(uint16_t sess_id) 
{
	struct hash_bucket 		*sess_item_new = NULL;
	struct cfm_sess			*sess_node = NULL;
	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));

	zlog_debug(CFM_DBG_COMMN,"%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);


	sess_item_new = hios_hash_find(&cfm_sess_table_new, (void *)(uint32_t)sess_id);


	if(sess_item_new)
	{	
		sess_node = sess_item_new->data;
	
	    cfm_session_lm_disable(sess_node);
        cfm_session_dm_disable(sess_node);
		hios_hash_delete(&cfm_sess_table_new, sess_item_new);

		cfm_session_delete(sess_node->ma_index<<16|sess_node->local_mep);//delete session item in cfm_sess_table
		
		if(gelps.sess_enable)
		{
			elps_state_update(sess_node->ifindex,sess_node->sess_id,CC_STATE_UP);
		}
        
		if(gerps.sess_enable)
		{
			erps_state_update(sess_node->ifindex,sess_node->sess_id,ERPS_CC_DELETE);
		}
		
		if (sess_node->lb_enable)
		{
			cfm_lb_stop(sess_node);
		}

		if (sess_node->lt_enable)
		{
			cfm_lt_stop(sess_node);
		}

		if(sess_node->ais_enable)
		{
			cfm_ais_stop(sess_node);			
		}

		if(sess_node->tst_enable)
		{
			cfm_test_stop(sess_node);			
		}
			
		cfm_session_clear_alarm(sess_node);

		//cfm_send_session_to_hal(sess_node,0,0);
		if(sess_node->psla)
		{
			XFREE(MTYPE_CFM_SESS, sess_node->psla);
		}
		
		gPortInfo.iAlarmPort = IFM_CFM_MEP_TYPE;
		gPortInfo.iMsgPara1 = sess_node->sess_id;
		ipran_alarm_port_unregister(&gPortInfo);
		XFREE(MTYPE_CFM_SESS, sess_node);
		XFREE(MTYPE_HASH_BACKET, sess_item_new);

	}	

	return ERRNO_SUCCESS;
	//delet sesslist

}

/**
 * @brief      : 查找一个 session，与hal交互的哈希表
 * @param[in ] : key, key值 
 * @param[out] : 
 * @return     : 成功返回查找到的session指针，失败返回NULL
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

struct cfm_sess		*cfm_session_lookup(uint32_t key)
{
	struct hash_bucket 		*sess_item = NULL;
	struct cfm_sess			*sess_node = NULL;

	
	sess_item = hios_hash_find(&cfm_sess_table, (void *)key);

	if(sess_item)
	{
		sess_node = sess_item->data;

		return sess_node;		
	}	

	return NULL;

}

/**
 * @brief      : 查找一个 session，与命令行交互的哈希表
 * @param[in ] : sess_id，session id
 * @param[out] : 
 * @return     : 成功返回查找到的session指针，失败返回NULL
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

struct cfm_sess		*cfm_session_lookup_new(uint16_t sess_id)
{
	struct hash_bucket 		*sess_item = NULL;
	struct cfm_sess			*sess_node = NULL;

	
	sess_item = hios_hash_find(&cfm_sess_table_new, (void *)(uint32_t)sess_id);

	if(sess_item)
	{
		sess_node = sess_item->data;

		return sess_node;		
	}	

	return NULL;

}

/**
 * @brief      : 将session的告警清空
 * @param[in ] : psess, session指针 
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

int 	cfm_session_clear_alarm(struct cfm_sess	 *psess)
{
	if(NULL == psess)
		return ERRNO_FAIL;

	zlog_debug(CFM_DBG_COMMN,"%s:Entering the function of '%s'",__FILE__,__func__);

	
	if(psess->alarm.alarm_ma)
	{
		cfm_session_alarm_report(psess,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_CRE);
	}

	if(psess->alarm.alarm_error)
	{
		cfm_session_alarm_report(psess,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_FRE);		
	}

	if(psess->alarm.alarm_loc)
	{
		cfm_session_alarm_report(psess,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_LOC);		
	}

	if(psess->alarm.alarm_rdi)
	{
		cfm_session_alarm_report(psess,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_RDI);		
	}

	//ais
	if(psess->alarm.alarm_ais)
	{
		cfm_session_alarm_report(psess,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_AIS);		
	}

	//y.1731
	if(psess->alarm.alarm_level)
	{
		cfm_session_alarm_report(psess,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_MML);		
	}

	if(psess->alarm.alarm_meg)
	{
		cfm_session_alarm_report(psess,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_MMG);		
	}

	if(psess->alarm.alarm_mep)
	{
		cfm_session_alarm_report(psess,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_UNM);		
	}

	if(psess->alarm.alarm_cc)
	{
		cfm_session_alarm_report(psess,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_UNP);		
	}

	memset(&psess->alarm,0,sizeof(struct oam_alarm_t));
		


	return ERRNO_SUCCESS;

}

/**
 * @brief      : 处理会话up事件
 * @param[in ] :key,哈希key值 
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

int 	cfm_session_up(uint32_t key)
{
	struct hash_bucket 		*sess_item = NULL;
	struct cfm_sess			*sess_node = NULL;

	zlog_notice("%s:Entering the function of '%s'--key 0x%08X",__FILE__,__func__,key);

	
	sess_item = hios_hash_find(&cfm_sess_table, (void *)key);

	if(sess_item)
	{
		sess_node = sess_item->data;
		sess_node ->state = OAM_STATUS_UP;

		if(gelps.sess_enable)
		{
			elps_state_update(sess_node->ifindex,sess_node->sess_id,CC_STATE_UP);
		}
       
		if(gerps.sess_enable)
		{
			erps_state_update(sess_node->ifindex,sess_node->sess_id,ERPS_CC_UP);
		}
        
		if(sess_node->alarm.alarm_ma)
		{

			cfm_session_alarm_report(sess_node,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_CRE);

			sess_node->alarm.alarm_ma = 0;
		}

		if(sess_node->alarm.alarm_error)
		{
			
			cfm_session_alarm_report(sess_node,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_FRE);

			sess_node->alarm.alarm_error = 0;
		}

		if(sess_node->alarm.alarm_loc)
		{

			cfm_session_alarm_report(sess_node,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_LOC);

			sess_node->alarm.alarm_loc = 0;

		}

		//ais
		if(sess_node->alarm.alarm_ais)
		{
			cfm_session_alarm_report(sess_node,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_AIS);

			sess_node->alarm.alarm_ais = 0;
		}
	
		//y.1731
		if(sess_node->alarm.alarm_level)
		{
			cfm_session_alarm_report(sess_node,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_MML);

			sess_node->alarm.alarm_level = 0;
		}
	
		if(sess_node->alarm.alarm_meg)
		{
			cfm_session_alarm_report(sess_node,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_MMG);

			sess_node->alarm.alarm_meg = 0;
		}
	
		if(sess_node->alarm.alarm_mep)
		{
			cfm_session_alarm_report(sess_node,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_UNM);

			sess_node->alarm.alarm_mep = 0;
		}
	
		if(sess_node->alarm.alarm_cc)
		{
			cfm_session_alarm_report(sess_node,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_UNP);

			sess_node->alarm.alarm_cc = 0;
		}

		/*if(sess_node->alarm.alarm_rdi)
		{
			alarm_report(IPC_OPCODE_CLEAR,MODULE_ID_L2,ALM_CODE_OAM_CFM_RDI,
				sess_node->local_mep,sess_node->remote_mep,0,0);		
		}*/

		//memset(&sess_node->alarm,0,sizeof(struct oam_alarm_t));
		

	}	

	return ERRNO_SUCCESS;

}

/**
 * @brief      : 处理会话down事件
 * @param[in ] :key,哈希key值 
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

int 	cfm_session_down(uint32_t key)
{
	struct hash_bucket 		*sess_item = NULL;
	struct cfm_sess			*sess_node = NULL;

	zlog_notice("%s:Entering the function of '%s'--key %08x",__FILE__,__func__,key);

	
	sess_item = hios_hash_find(&cfm_sess_table, (void *)key);

	if(sess_item)
	{
		sess_node = sess_item->data;
		sess_node ->state = OAM_STATUS_DOWN;
		sess_node->alarm.alarm_loc	= 1;		

		cfm_session_alarm_report(sess_node,IPC_OPCODE_ADD,GPN_ALM_TYPE_FUN_CFM_LOC);
		
		zlog_debug(CFM_DBG_COMMN,"%s:Entering the function of '%s'--the line of %d sess_node->ifindex %d sess_node->local_mep %d sess_node->sess_id %d",
			__FILE__,__func__,__LINE__,sess_node->ifindex,sess_node->local_mep,sess_node->sess_id);	
		if(gelps.sess_enable)
		{
			elps_state_update(sess_node->ifindex,sess_node->sess_id,CC_STATE_DOWN);
		}
        
		if(gerps.sess_enable)
		{
			erps_state_update(sess_node->ifindex,sess_node->sess_id,ERPS_CC_DOWN);
		}	
		
	}	

	return ERRNO_SUCCESS;
}

/**
 * @brief      : 将告警上报网管
 * @param[in ] :psess,session指针 
 * @param[in ] :opcode,操作码 
 * @param[in ] :almcode，告警编码
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

int 	cfm_session_alarm_report(struct cfm_sess *psess,int opcode, unsigned int almcode)
{

	zlog_debug(CFM_DBG_COMMN,"%s:Entering the function of '%s'",__FILE__,__func__);

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_CFM_MEP_TYPE;
	gPortInfo.iMsgPara1 = psess->sess_id;
	if(psess)
	{	
		//alarm_report(opcode,MODULE_ID_L2,almcode,
			//psess->local_mep,psess->remote_mep,0,0);	

		if(opcode == IPC_OPCODE_ADD)
		{
			ipran_alarm_report(&gPortInfo,almcode,GPN_SOCK_MSG_OPT_RISE);
		}
		else if(opcode == IPC_OPCODE_CLEAR)
		{
			ipran_alarm_report(&gPortInfo,almcode,GPN_SOCK_MSG_OPT_CLEAN);
		}
	}	

	return ERRNO_SUCCESS;

}

/**
 * @brief      : 处理来自hal的告警事件
 * @param[in ] :key,哈希key值
 * @param[in ] :opcode,操作码 
 * @param[in ] :alarm_type，告警类型
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

int 	cfm_session_alarm(uint32_t key, uint16_t opcode, enum OAM_ALARM alarm_type)
{
	struct hash_bucket 		*sess_item  = NULL;
	struct cfm_sess			*psess 		= NULL;
	uint16_t				 ma_index   = key;
	struct cfm_ma			*pma 		= NULL;
	struct listnode 		*pnode;
	
	zlog_debug(CFM_DBG_COMMN,"%s:Entering the function of '%s'--key %08X,opcode:%d, alarm_type:%d",__FILE__,__func__,key,opcode,alarm_type);

	if(alarm_type == OAM_ALARM_MA || alarm_type == OAM_ALARM_ERROR)
	{
		sess_item = hios_hash_find(&cfm_ma_table, (void *)(uint32_t)ma_index);
		if(NULL == sess_item)
		{
			return ERRNO_NOT_FOUND;
		}
		pma = sess_item->data;

		pnode	= pma->sess_list.head;

		while(pnode!=NULL)
		{
			psess	= pnode->data;		
			pnode 	= pnode->next;

			if(opcode == IPC_OPCODE_ADD)
			{
				switch(alarm_type)
				{
					case OAM_ALARM_MA:
						psess->alarm.alarm_ma = 1;
						cfm_session_alarm_report(psess,opcode,GPN_ALM_TYPE_FUN_CFM_CRE);
						break;
					case OAM_ALARM_ERROR:
						psess->alarm.alarm_error = 1;
						cfm_session_alarm_report(psess,opcode,GPN_ALM_TYPE_FUN_CFM_FRE);						
						break;
					default:
						break;
				}
			}
			else if(IPC_OPCODE_CLEAR)
			{
				elps_state_update(psess->ifindex,psess->sess_id,CC_STATE_UP);
				switch(alarm_type)
				{
					case OAM_ALARM_MA:
						psess->alarm.alarm_ma = 0;
						cfm_session_alarm_report(psess,opcode,GPN_ALM_TYPE_FUN_CFM_CRE);						
						break;
					case OAM_ALARM_ERROR:
						psess->alarm.alarm_error = 0;
						cfm_session_alarm_report(psess,opcode,GPN_ALM_TYPE_FUN_CFM_FRE);						
						break;
					default:
						break;						
				}
			}
			
		}

		return ERRNO_SUCCESS;
	}


	
	sess_item = hios_hash_find(&cfm_sess_table, (void *)key);

    if(NULL == sess_item)
    {
        return ERRNO_NOT_FOUND;
    }

	psess = sess_item->data;

	if(opcode == IPC_OPCODE_ADD)
	{
			switch(alarm_type)
			{
				case OAM_ALARM_PORT_BLOCK:	
					psess->alarm.alarm_port_block= 1;
					break;
					
				//case CFM_ALARM_REMOTE_CCM:	
					//psess->alarm.alarm_ncc = 1;
					//break;
					
				case OAM_ALARM_PORT_DOWN:	
					psess->alarm.alarm_port_down= 1;
					break;
					
				case OAM_ALARM_RDI:	
					psess->alarm.alarm_rdi = 1;
					cfm_session_alarm_report(psess,opcode,GPN_ALM_TYPE_FUN_CFM_RDI);					
					break;
				//y.1731
				case OAM_ALARM_MEG: 
					psess->alarm.alarm_meg = 1;
					cfm_session_alarm_report(psess,opcode,GPN_ALM_TYPE_FUN_CFM_MMG);					
					break;
				case OAM_ALARM_MEP: 
					psess->alarm.alarm_mep = 1;
					cfm_session_alarm_report(psess,opcode,GPN_ALM_TYPE_FUN_CFM_UNM);					
					break;
				case OAM_ALARM_LEVEL: 
					psess->alarm.alarm_level= 1;
					cfm_session_alarm_report(psess,opcode,GPN_ALM_TYPE_FUN_CFM_MML);					
					break;
				case OAM_ALARM_INTERVAL: 
					psess->alarm.alarm_cc = 1;
					cfm_session_alarm_report(psess,opcode,GPN_ALM_TYPE_FUN_CFM_UNP);					
					break;
					
				default:
					break;
				
			}
		}
	else if(opcode == IPC_OPCODE_CLEAR)
	{
		switch(alarm_type)
		{
			case OAM_ALARM_PORT_BLOCK:		
				psess->alarm.alarm_port_block= 0;
				break;
				
			//case CFM_ALARM_REMOTE_CCM:		
				//psess->alarm.alarm_ncc = 0;
				//break;
				
			case OAM_ALARM_PORT_DOWN:	
				psess->alarm.alarm_port_down= 0;
				break;
				
				
			case OAM_ALARM_RDI:		
				psess->alarm.alarm_rdi = 0;
				cfm_session_alarm_report(psess,opcode,GPN_ALM_TYPE_FUN_CFM_RDI);				
				break;

			//y.1731
			case OAM_ALARM_MEG: 
				psess->alarm.alarm_meg = 0;
				cfm_session_alarm_report(psess,opcode,GPN_ALM_TYPE_FUN_CFM_MMG);					
				break;
			case OAM_ALARM_MEP: 
				psess->alarm.alarm_mep = 0;
				cfm_session_alarm_report(psess,opcode,GPN_ALM_TYPE_FUN_CFM_UNM);					
				break;
			case OAM_ALARM_LEVEL: 
				psess->alarm.alarm_level= 0;
				cfm_session_alarm_report(psess,opcode,GPN_ALM_TYPE_FUN_CFM_MML);					
				break;
			case OAM_ALARM_INTERVAL: 
				psess->alarm.alarm_cc = 0;
				cfm_session_alarm_report(psess,opcode,GPN_ALM_TYPE_FUN_CFM_UNP);					
				break;
			case OAM_ALARM_ALL: 
				cfm_session_clear_alarm(psess);	
				break;
				
			default:
				break;
			
		}
				
	}

	return ERRNO_SUCCESS;
}

/**
 * @brief      : lm使能
 * @param[in ] :psess,session指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

int cfm_session_lm_enable(struct cfm_sess *psess)
{
    int ret = 0;

    if((NULL == psess) || (psess->lm_enable == 1))
    {   
        return ERRNO_FAIL;
    }

    psess->lm_enable = OAM_STATUS_ENABLE;


    /*printf ( "%s[%d] : local_mep = %u\n", __FUNCTION__, __LINE__, psess->local_mep );
    printf ( "%s[%d] : remote_mep = %u\n", __FUNCTION__, __LINE__, psess->remote_mep );
    printf ( "%s[%d] : cc_interval = %u\n", __FUNCTION__, __LINE__, psess->cc_interval );
    printf ( "%s[%d] : cc_enable = %u\n", __FUNCTION__, __LINE__, psess->cc_enable );
    printf ( "%s[%d] : lm_enable = %u\n", __FUNCTION__, __LINE__, psess->lm_enable);
    printf ( "%s[%d] : lm_interval = %u\n", __FUNCTION__, __LINE__, psess->lm_interval);*/
    
    /* send lm-enable to hal */
#if 0
    ret = ipc_send_hal(psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
    					IPC_TYPE_CFM, OAM_SUBTYPE_LM, IPC_OPCODE_ENABLE, psess->local_mep);
#endif

    ret = cfm_send_hal(psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
    					IPC_TYPE_CFM, OAM_SUBTYPE_LM, IPC_OPCODE_ENABLE, psess->local_mep);
    if(ret != ERRNO_SUCCESS)
    {
        psess->lm_enable = OAM_STATUS_DISABLE;
        psess->lm_interval = 0;
        zlog_err("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
        return ERRNO_IPC;
    }

    return ERRNO_SUCCESS;
}

/**
 * @brief      : lm禁止
 * @param[in ] :psess,session指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

int cfm_session_lm_disable(struct cfm_sess *psess)
{
    int ret = 0;

    if((NULL == psess) || (psess->lm_enable != OAM_STATUS_ENABLE))
    {
        return  ERRNO_FAIL;
    }
    
    /* send lm-disable to hal */
#if 0
    ret = ipc_send_hal(psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
    					IPC_TYPE_CFM, OAM_SUBTYPE_LM, IPC_OPCODE_DISABLE, psess->local_mep);
#endif

	ret = cfm_send_hal(psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
    					IPC_TYPE_CFM, OAM_SUBTYPE_LM, IPC_OPCODE_DISABLE, psess->local_mep);
    if(ret != ERRNO_SUCCESS)
    {
        zlog_err("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
        return ERRNO_IPC;
    }
    psess->lm_enable 	= OAM_STATUS_DISABLE;
    psess->lm_interval  = 0;
	memset(psess->lm_dmac,0,6);

    return ERRNO_SUCCESS;
}

/**
 * @brief      : dm使能
 * @param[in ] :psess,session指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

int cfm_session_dm_enable(struct cfm_sess *psess)
{
    int ret = 0;

    if((NULL == psess) || (psess->dm_enable == 1))
    {   
        return ERRNO_FAIL;
    }

    psess->dm_enable = OAM_STATUS_ENABLE;
    
    /* send dm-enable to hal */
#if 0
    ret = ipc_send_hal(psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
    					IPC_TYPE_CFM, OAM_SUBTYPE_DM, IPC_OPCODE_ENABLE, psess->local_mep);
#endif
	ret = cfm_send_hal(psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
    					IPC_TYPE_CFM, OAM_SUBTYPE_DM, IPC_OPCODE_ENABLE, psess->local_mep);
    if(ret != ERRNO_SUCCESS)
    {
        psess->dm_enable 	= OAM_STATUS_DISABLE;
        psess->dm_interval  = 0;
        zlog_err("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
        return ERRNO_IPC;
    }

    return ERRNO_SUCCESS;
}

/**
 * @brief      : dm禁止
 * @param[in ] :psess,session指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

int cfm_session_dm_disable(struct cfm_sess *psess)
{
    int ret = 0;

    if((NULL == psess) || (psess->dm_enable != OAM_STATUS_ENABLE))
    {
        return  ERRNO_FAIL;
    }
    
    /* send lm-disable to hal */
#if 0
    ret = ipc_send_hal(psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
    					IPC_TYPE_CFM, OAM_SUBTYPE_DM, IPC_OPCODE_DISABLE, psess->local_mep);
#endif

	ret = cfm_send_hal(psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
    					IPC_TYPE_CFM, OAM_SUBTYPE_DM, IPC_OPCODE_DISABLE, psess->local_mep);
    if(ret != ERRNO_SUCCESS)
    {
        zlog_err("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
        return ERRNO_IPC;
    }
    psess->dm_enable 	= OAM_STATUS_DISABLE;
    psess->dm_interval 	= 0;
	memset(psess->dm_dmac,0,6);
	
    return ERRNO_SUCCESS;
}

/**
 * @brief      : 接收来自hal的dm测算结果
 * @param[in ] :psess,session指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

int cfm_session_set_dm(uint32_t key, struct sla_measure *sla)
{
    struct cfm_sess *psess = NULL;
    
    if(NULL == sla)
    {
        return ERRNO_FAIL;
    }

    psess = cfm_session_lookup(key);
    if((NULL == psess) || (NULL == psess->psla))
    {
        return ERRNO_FAIL;
    }
    
    psess->dm_enable 		 = OAM_STATUS_FINISH;
    psess->psla->dm_max 	 = sla->dm_max;
    psess->psla->dm_min 	 = sla->dm_min;
    psess->psla->dm_mean 	 = sla->dm_mean;
    psess->psla->jitter_max  = sla->jitter_max;
    psess->psla->jitter_min  = sla->jitter_min;
    psess->psla->jitter_mean = sla->jitter_mean;

	if(psess->perfm_enable)
	{
		cfm_session_dm_enable(psess);
	}
        
    return ERRNO_SUCCESS;
}

/**
 * @brief      : 接收来自hal的lm测算结果
 * @param[in ] :psess,session指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

int cfm_session_set_lm(uint32_t key, struct sla_measure *sla)
{
    struct cfm_sess *psess = NULL;

    if(NULL == sla)
    {
        return ERRNO_FAIL;
    }
    
    psess = cfm_session_lookup(key);
    if((NULL == psess) || (NULL == psess->psla))
    {
        return ERRNO_FAIL;
    }
    
    psess->lm_enable 		  = OAM_STATUS_FINISH;
    
    /* set near-end lm */
    psess->psla->lm_max 	  = sla->lm_max;
    psess->psla->lm_min 	  = sla->lm_min;
    psess->psla->lm_mean 	  = sla->lm_mean;

    /* set far-end lm */
    psess->psla->lm_max_peer  = sla->lm_max_peer;
    psess->psla->lm_min_peer  = sla->lm_min_peer;
    psess->psla->lm_mean_peer = sla->lm_mean_peer;

	if(psess->perfm_enable)
	{
		cfm_session_lm_enable(psess);
	}
		
    return ERRNO_SUCCESS;
}



/**
 * @brief      :ais报文超时接收定时器
 * @param[in ] : thread，定时器指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */
#if 0
static int cfm_ais_timer(struct thread *thread)
{
	uint32_t key;
	struct cfm_sess *psess;
	
	key = (uint32_t)THREAD_ARG(thread);		
	psess = cfm_session_lookup(key);
		
    if(NULL == psess)
    {       	
		zlog_err("%s: %s, pointer error, psess:%p,return!!\n",__FILE__,__func__,psess);
        return ERRNO_FAIL;
    }

	if(psess->ais_rcv_interval == 0)//ais disappear
	{
		psess->alarm.alarm_ais = 0;
		psess->pais_timer = NULL;
	}
	else//ais exist
	{
		psess->pais_timer = thread_add_timer(l2_master, cfm_ais_timer, (void *)(psess->local_mep|(psess->ma_index<<16)), psess->ais_rcv_interval);
		psess->ais_rcv_interval = 0;		
	}
	
	
	return 0;
}
#endif

static int cfm_ais_rsv_timer_out(void *para)
{
	uint32_t key;
	struct cfm_sess *psess = NULL;

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

	if(psess->ais_rcv_interval == 0)//ais disappear
	{
		psess->alarm.alarm_ais = 0;
		//psess->pais_timer = NULL;
		cfm_session_alarm_report(psess,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_AIS);
		high_pre_timer_delete(psess->pais_timer);
		psess->pais_timer = 0;
	}
	else//ais exist
	{
		//psess->pais_timer = thread_add_timer(l2_master, cfm_ais_timer, (void *)(psess->local_mep|(psess->ma_index<<16)), psess->ais_rcv_interval);
		psess->ais_rcv_interval = 0;		
	}
	
	
	return ERRNO_SUCCESS;
}


/**
 * @brief      : ais报文发送函数
 * @param[in ] : thread，发送定时器指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */
#if 0
static int cfm_send_ais(struct thread *thread)
{
	struct cfm_ais	   *pais;
	struct cfm_ais_pdu *pais_pdu;
	uint32_t   	  		key;
	uint8_t		  		i;
	struct raw_control  rawcb;
	uint8_t				mac[6] 		= {0x01, 0x80, 0xC2, 0x00, 0x00, 0x30};
	struct listnode    *p_listnode 	= NULL;
	void 			   *data 		= NULL;	
	struct cfm_ma 	   *pma 		= NULL;
	struct cfm_sess    *psess 		= NULL;	
	uint8_t 		    p_mode 		= 0;	
    int 				ret 		= 0;
	
	key 	= (uint32_t)THREAD_ARG(thread);		
	psess 	= cfm_session_lookup(key);
		
    if((NULL == psess) || (NULL == psess->pais))
    {       	
		zlog_err("%s: %s, pointer error, psess:%p, psess->pais:%p,return!!\n",__FILE__,__func__,psess,psess->pais);
        return ERRNO_FAIL;
    }

	memset (&rawcb, 0, sizeof(struct raw_control));

	pais 		= psess->pais;
	pais_pdu 	= &pais->ais_pdu;


	if(!psess->alarm.alarm_loc)
	{
		goto exit;
	}

	
	if(ifm_get_mode(psess->ifindex,MODULE_ID_L2, &p_mode) != 0)
	{
		zlog_err("%s: %s,get the interface mode timeout",__FILE__,__func__);		
		goto exit;
	}	

	pma = cfm_ma_lookup(psess->ma_index);
	if(!pma)
        return ERRNO_FAIL;

	rawcb.ethtype	 = 0x8902;
	rawcb.ttl		 = 64;
	rawcb.priority	 = pma->priority;

	for(i=0;i < 8;i++)
	{
		if(psess->ais_send_level & (0x01<<i))
		{
			pais_pdu->cfm_header.level			 = i;//level		
			
			mac[5] = mac[5] | i;  //level
			memcpy(pais_pdu->ethhdr.h_dest,  mac, 6);				
			
			if(psess->direct == MEP_DIRECT_DOWN)//back direction
			{
				if(p_mode != IFNET_MODE_L2)
				{
					//for switch
					for(ALL_LIST_ELEMENTS_RO(&(vlan_table[psess->vlan]->portlist),p_listnode,data))
					{
						if(((uint32_t)data) != psess->ifindex)
						{				
							rawcb.out_ifindex = (uint32_t)data;
							ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, pais_pdu, sizeof(struct cfm_ais_pdu)); 	
						}
					}
				}
				else
				{
					rawcb.in_ifindex = psess->ifindex;
					ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, pais_pdu, sizeof(struct cfm_ais_pdu)); 	
				}
			
			}
			else//positive direction
			{
				rawcb.out_ifindex = psess->ifindex;
				ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, pais_pdu, sizeof(struct cfm_ais_pdu)); 	
			}
		}		

	}

	

	exit:	
	pais->pais_timer = thread_add_timer(l2_master, cfm_send_ais, (void *)(psess->local_mep|(psess->ma_index<<16)), psess->ais_send_interval);

	return ret;
}
#endif

static int cfm_ais_snd_timer_out(void *para)
{
	struct cfm_ais	   *pais;
	struct cfm_ais_pdu *pais_pdu;
	uint8_t		  		i;
	struct raw_control  rawcb;
	uint8_t				mac[6] 		= {0x01, 0x80, 0xC2, 0x00, 0x00, 0x30};
	struct listnode    *p_listnode 	= NULL;
	void 			   *data 		= NULL;	
	struct cfm_ma 	   *pma 		= NULL;
	struct cfm_sess    *psess 		= NULL;	
	uint8_t 		    p_mode 		= 0;	
    int 				ret 		= 0;
	uint32_t 			key;

	if (NULL == para)
	{
		 return ERRNO_FAIL;
	}

	key   = (uint32_t)para;	
	psess 	= cfm_session_lookup(key);	
    if((NULL == psess) || (NULL == psess->pais))
    {       	
		zlog_err("%s: %s, pointer error, psess:%p, psess->pais:%p,return!!\n",__FILE__,__func__,psess,psess->pais);
        return ERRNO_FAIL;
    }

	memset (&rawcb, 0, sizeof(struct raw_control));

	pais 		= psess->pais;
	pais_pdu 	= &pais->ais_pdu;


	if(!psess->alarm.alarm_loc)
	{
		return ret;
	}

	if(ifm_get_mode(psess->ifindex,MODULE_ID_L2, &p_mode) != 0)
	{
		zlog_err("%s: %s,get the interface mode timeout",__FILE__,__func__);		
		return ret;
	}	

	pma = cfm_ma_lookup(psess->ma_index);
	if(!pma)
	{
        return ERRNO_FAIL;
	}

	rawcb.ethtype	 = 0x8902;
	rawcb.ttl		 = 64;
	rawcb.priority	 = pma->priority;

	for(i=0;i < 8;i++)
	{
		if(psess->ais_send_level & (0x01<<i))
		{
			pais_pdu->cfm_header.level			 = i;//level		
			
			mac[5] = mac[5] | i;  //level
			memcpy(pais_pdu->ethhdr.h_dest,  mac, 6);				
			
			if(psess->direct == MEP_DIRECT_DOWN)//back direction
			{
				if(p_mode != IFNET_MODE_L2)
				{
					//for switch
					for(ALL_LIST_ELEMENTS_RO(&(vlan_table[psess->vlan]->portlist),p_listnode,data))
					{
						if(((uint32_t)data) != psess->ifindex)
						{				
							rawcb.out_ifindex = (uint32_t)data;
							ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, pais_pdu, sizeof(struct cfm_ais_pdu)); 	
						}
					}
				}
				else
				{
					rawcb.in_ifindex = psess->ifindex;
					ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, pais_pdu, sizeof(struct cfm_ais_pdu)); 	
				}
			
			}
			else//positive direction
			{
				rawcb.out_ifindex = psess->ifindex;
				ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, pais_pdu, sizeof(struct cfm_ais_pdu)); 	
			}
		}		
	}

	return ret;
}

/**
 * @brief      : ais报文接收处理函数
 * @param[in ] : pkt，接收报文指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

int cfm_rcv_ais(struct pkt_buffer *pkt)
{
	struct cfm_ais_pdu  ais_pdu;
	char 				name[50];
	struct cfm_ma 		*pma 		= NULL;
	struct cfm_md 		*pmd 		= NULL;
	struct listnode 	*p_listnode = NULL, *p_listnode_sess = NULL;
	void 				*data 		= NULL, *data_sess 		 = NULL;
    uint8_t        	 	interval; 
	struct cfm_sess 	*psess;
	uint8_t 			dmac_multi[6]={0x01,0x80,0xc2,0x00,0x00,0x30};

	char timerName[20] = "CfmAisRsvTimer";
	
	zlog_debug(CFM_DBG_COMMN,"%s:Entering the function of '%s'",__FILE__,__func__);

	if(NULL == pkt)
	{
		return ERRNO_PARAM_ILLEGAL;
	}

	ifm_get_name_by_ifindex(pkt->in_ifindex,name);
	zlog_debug(CFM_DBG_COMMN,"receive cfm ais from interface:%s, vpn:%d \n",name,pkt->vpn);

	ifm_get_name_by_ifindex(pkt->in_port,name);
	zlog_debug(CFM_DBG_COMMN,"physical port is:%s\n",name);


	memset (&ais_pdu, 0, sizeof(struct cfm_ais_pdu));
	memcpy (&ais_pdu.cfm_header, pkt->data,pkt->data_len);

	interval = (ais_pdu.cfm_header.flag & 0x02)?60:1;
	interval = (3*interval)+(interval>>1); //3.5倍检测周期

	pmd = cfm_md_lookup_by_level(ais_pdu.cfm_header.level);
	if(!pmd)
	{
		zlog_err("%s: %s,cfm_md_lookup_by_level fail,level:%d",__FILE__,__func__,ais_pdu.cfm_header.level);
		
		return ERRNO_FAIL;
	}

	//check multicast mac
	dmac_multi[5] += pmd->level;	
	if(pkt->cb.ethcb.dmac[0]&0x01)
	{
		if(memcmp(pkt->cb.ethcb.dmac,dmac_multi,6))
		{
			zlog_debug(CFM_DBG_COMMN,"target mac:%02X:%02X:%02X:%02X:%02X:%02X\n",pkt->cb.ethcb.dmac[0],
				pkt->cb.ethcb.dmac[1],
				pkt->cb.ethcb.dmac[2],
				pkt->cb.ethcb.dmac[3],
				pkt->cb.ethcb.dmac[4],
				pkt->cb.ethcb.dmac[5]);
			
			zlog_debug(CFM_DBG_COMMN,"multicast dmac:%02X:%02X:%02X:%02X:%02X:%02X\n",dmac_multi[0],
				dmac_multi[1],
				dmac_multi[2],
				dmac_multi[3],
				dmac_multi[4],
				dmac_multi[5]);
		
			zlog_err("%s:md %d receive ais but not match",__func__,pmd->md_index);
			return ERRNO_UNMATCH;		
		}
	}

	for(ALL_LIST_ELEMENTS_RO(&pmd->ma_list,p_listnode,data))
	{
		pma = (struct cfm_ma *)data;			
		for(ALL_LIST_ELEMENTS_RO(&pma->sess_list,p_listnode_sess,data_sess))
		{
			psess = (struct cfm_sess *)data_sess;

			//check uniticast mac
			if(!(pkt->cb.ethcb.dmac[0]&0x01))
			{
				if(memcmp(pkt->cb.ethcb.dmac,psess->smac,6))
				{	
					zlog_debug(CFM_DBG_COMMN,"target mac:%02X:%02X:%02X:%02X:%02X:%02X\n",pkt->cb.ethcb.dmac[0],
						pkt->cb.ethcb.dmac[1],
						pkt->cb.ethcb.dmac[2],
						pkt->cb.ethcb.dmac[3],
						pkt->cb.ethcb.dmac[4],
						pkt->cb.ethcb.dmac[5]);
					
					zlog_debug(CFM_DBG_COMMN,"smac:%02X:%02X:%02X:%02X:%02X:%02X\n",psess->smac[0],
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
					zlog_err("%s:session %d receive ais but interface not match",__func__,psess->sess_id);
					continue;		
				}
			}
			else
			{
				if(pkt->in_ifindex == psess->ifindex)
				{
					zlog_err("%s:normal mode, session %d receive ais but interface same",__func__,psess->sess_id);
					continue;		
				}
			}

			if (0 == psess->alarm.alarm_ais)
			{
				psess->alarm.alarm_ais  = 1;
				cfm_session_alarm_report(psess,IPC_OPCODE_ADD,GPN_ALM_TYPE_FUN_CFM_AIS);
			}
			
			psess->ais_rcv_interval = interval;
#if 0
			if(NULL == psess->pais_timer)
			{
				psess->pais_timer = thread_add_timer(l2_master, cfm_ais_timer, (void *)(psess->local_mep|(psess->ma_index<<16)), interval);
			}
#endif
			if (!psess->pais_timer)
			{
				psess->pais_timer = high_pre_timer_add(timerName, LIB_TIMER_TYPE_LOOP, cfm_ais_rsv_timer_out, (void *)(psess->local_mep|(psess->ma_index<<16)), interval * 1000);
			}
			
			zlog_debug(CFM_DBG_COMMN,"session %d in ma %d ,ais set \n",psess->sess_id,psess->ma_index);				
			
		}
	}

	return ERRNO_SUCCESS;
}

/**
 * @brief      : ais发起函数
 * @param[in ] : psess，session指针； 
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

int cfm_ais_start(struct cfm_sess *psess)
{
	struct cfm_ais 		*pais;
	struct cfm_ais_pdu 	*pais_pdu;
	struct cfm_ma  		*pma;

	char timerName[20] = "CfmAisSndTimer";
		
    if((NULL == psess) || (NULL == psess->pais))
    {   
        return ERRNO_FAIL;
    }

	pma 		= cfm_ma_lookup(psess->ma_index);
	if(!pma)
        return ERRNO_FAIL;

	pais 		= psess->pais;
	pais_pdu 	= &pais->ais_pdu;


	memset(pais_pdu,0,sizeof(struct cfm_ais_pdu));
	
	pais_pdu->end_tlv  = 0;

	//pais_pdu->cfm_header.level			 = 0;//fix
	pais_pdu->cfm_header.version		 	= 0;
	pais_pdu->cfm_header.opcode			 	= CFM_AIS_MESSAGE_OPCODE;
	pais_pdu->cfm_header.first_tlv_offset  	= 0;

	if(psess->ais_send_interval == 1)
	{
		pais_pdu->cfm_header.flag = 0x04;
	}
	else
	{
		pais_pdu->cfm_header.flag = 0x06;
	}


	memcpy(pais_pdu->ethhdr.h_source, psess->smac, 6);

	pais_pdu->ethhdr.h_tpid	   	 = htons(0x8100);
	pais_pdu->ethhdr.h_vlan_tci  = htons((pma->priority << 13) | psess->vlan);
	pais_pdu->ethhdr.h_ethtype   = htons(0x8902);

	//pais->pais_timer = thread_add_timer(l2_master, cfm_send_ais, (void *)(psess->local_mep|(psess->ma_index<<16)), psess->ais_send_interval);

	pais->pais_timer = high_pre_timer_add(timerName, LIB_TIMER_TYPE_LOOP, cfm_ais_snd_timer_out, (void *)(psess->local_mep|(psess->ma_index<<16)), psess->ais_send_interval * 1000);

	zlog_debug(CFM_DBG_COMMN, "%s:'%s', aisTimer(%lu) add\n",  __FILE__, __func__, pais->pais_timer);
	
	return ERRNO_SUCCESS;

}

/**
 * @brief      : ais结束函数
 * @param[in ] : psess，session指针； 
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

int cfm_ais_stop(struct cfm_sess *psess)
{
	struct cfm_ais *pais;

		
    if(NULL == psess)
    {   
        return ERRNO_FAIL;
    }

	pais = psess->pais;

	if (pais )
	{
#if 0
		if(pais->pais_timer)//cancle the timer
		{
			THREAD_TIMER_OFF(pais->pais_timer);
			pais->pais_timer = NULL;
		}
#endif

		if(pais->pais_timer)//cancle the timer
		{
			zlog_debug(CFM_DBG_COMMN, "%s:'%s', aisTimer(%lu) delete\n", 
					__FILE__, __func__, pais->pais_timer);
					
			high_pre_timer_delete(pais->pais_timer);
			pais->pais_timer = 0;
		}
		
		XFREE (MTYPE_TMP, pais);
		psess->pais = NULL;
		
	}

	return ERRNO_SUCCESS;

}

/**
 * @brief      : 读完配置文件配置下发到hal
 * @param[in ] : 
 * @param[out] : 
 * @return     : 
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

void  config_finish_func(void * a)
{
	struct cfm_sess 	*psess=NULL;
    struct hash_bucket  *pbucket = NULL;
    int 				cursor;
	int 				ret;
	uint8_t				pmac[6] = {0};

	zlog_notice ("config_finish_func,%s,%s ,session num:%d \n",__FILE__,__FUNCTION__,cfm_sess_table.num_entries);

	HASH_BUCKET_LOOP(pbucket,cursor,cfm_sess_table)
	{
		psess = pbucket->data;

		if(!psess->local_mep || !psess->ma_index || !psess->remote_mep || !psess->ifindex)
		{
			zlog_err ("Error: session id:%d ,local_mep:%d, ma_index:%d,remote_mep:%d \n",psess->sess_id ,psess->local_mep,psess->ma_index,psess->remote_mep);
		
			continue;
		}

		if(ifm_get_mac(psess->ifindex, MODULE_ID_L2, pmac) == 0)
		{
			memcpy(psess->smac , pmac , 6);
		}
		else 
		{
			zlog_err ("Error:can not get mac on the interface,func:%s,session:%d \n",__FUNCTION__,psess->sess_id); 
			psess->ifindex = 0;
			psess->state = OAM_STATUS_DISABLE;			
			psess->cc_enable = 0;
			psess->cc_interval = 0;
			if(psess->ais_enable)
			{
				psess->ais_enable = 0;
				psess->ais_send_interval = 0;
				psess->ais_send_level = 0;
				cfm_ais_stop(psess);
			}
			continue; 		
		}

#if 0		
		ret = ipc_send_hal_wait_ack( psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_SESSION, IPC_OPCODE_ADD, 0);
#endif

		ret = cfm_ipc_send_wait_ack(psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_SESSION, IPC_OPCODE_ADD, 0);
		if(ret != 0)
		{
			zlog_err ( "%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret );
			
			psess->ifindex = 0;
			psess->state = OAM_STATUS_DISABLE;			
			psess->cc_enable = 0;
			psess->cc_interval = 0;
			if(psess->ais_enable)
			{
				psess->ais_enable = 0;
				psess->ais_send_interval = 0;
				psess->ais_send_level = 0;
				cfm_ais_stop(psess);
			}
			
			/*ht201:128  ht2200:1024 */
			if(ret == ERRNO_OVERSIZE)
			{
				zlog_err ("Error: oversize,from session id:%d \n",psess->sess_id);
				return ;			
			}
			else
			{
				zlog_err ("Error: set error,session id:%d \n",psess->sess_id);				
				continue;		
			}			
			
		}
		
	}

	return ;

}

/**
 * @brief      :elps绑定会话时在cfm中增加绑定个数
 * @param[in ] : sess_id，session id
 * @param[out] : 
 * @return     : 成功返回相关的session指针，失败返回NULL
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

struct cfm_sess		*cfm_session_bind_elps(uint16_t sess_id) 
{
	struct hash_bucket 		*sess_item = NULL;
	struct cfm_sess			*sess_node = NULL;
	struct cfm_md			*pmd = NULL;
	struct cfm_ma			*pma = NULL;
	
	sess_item = hios_hash_find(&cfm_sess_table_new, (void *)(uint32_t)sess_id);

	if(sess_item)
	{
		sess_node = sess_item->data;

		sess_node->elps_bind++;
		zlog_debug(CFM_DBG_COMMN,"%s,sess_node->elps_bind:%d \n",__FUNCTION__,sess_node->elps_bind);					

		pmd = cfm_md_lookup(sess_node->md_index);
		if(pmd)
		{
			pmd->elps_bind++;
			zlog_debug(CFM_DBG_COMMN,"%s,pmd->elps_bind:%d \n",__FUNCTION__,pmd->elps_bind); 			
			
		}

		pma = cfm_ma_lookup(sess_node->ma_index);
		if(pma)
		{
			pma->elps_bind++;
			zlog_debug(CFM_DBG_COMMN,"%s,pma->elps_bind:%d \n",__FUNCTION__,pma->elps_bind); 					
		}
		
		return sess_node;		
	}	
	else
	{
		zlog_err ("%s,sess_id[%d] not find\n",__FUNCTION__,sess_id);
		return NULL;
	}

}

/**
 * @brief      :elps解绑会话时在cfm中减少绑定个数
 * @param[in ] : sess_id，session id
 * @param[out] : 
 * @return     : 成功返回相关的session指针，失败返回NULL
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

struct cfm_sess		*cfm_session_unbind_elps(uint16_t sess_id) 
{
	struct hash_bucket 		*sess_item = NULL;
	struct cfm_sess			*sess_node = NULL;
	struct cfm_md			*pmd = NULL;
	struct cfm_ma			*pma = NULL;
	
	sess_item = hios_hash_find(&cfm_sess_table_new, (void *)(uint32_t)sess_id);

	if(sess_item)
	{
		sess_node = sess_item->data;
		
		sess_node->elps_bind--;			
		zlog_debug(CFM_DBG_COMMN,"%s,sess_node->elps_bind:%d \n",__FUNCTION__,sess_node->elps_bind);					

		pmd = cfm_md_lookup(sess_node->md_index);
		
		if(pmd)
		{
			pmd->elps_bind--;
			zlog_debug(CFM_DBG_COMMN,"%s,pmd->elps_bind:%d \n",__FUNCTION__,pmd->elps_bind); 			
		}

		pma = cfm_ma_lookup(sess_node->ma_index);		
		if(pma)
		{
			pma->elps_bind--;
			zlog_debug(CFM_DBG_COMMN,"%s,pma->elps_bind:%d \n",__FUNCTION__,pma->elps_bind); 					
		}

		
		return sess_node;		
		
	}	
	else
	{
		zlog_err ("%s,sess_id[%d] not find\n",__FUNCTION__,sess_id);
		return NULL;
	}


}


/**
 * @brief      :2200设备物理口删除时通知hal删除会话，包括所有相关子接口
 * @param[in ] : ifindex，接口索引
 * @param[out] : 
 * @return     : 
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

void cfm_if_delete(uint32_t ifindex)
{
	struct cfm_sess 	*psess = NULL;
    struct hash_bucket  *pbucket = NULL;
    int 				 cursor;
	int 				 ret;
	char 				 str1[50];
	struct ifm_usp usp;
	 
	zlog_debug(CFM_DBG_COMMN,"%s:Entering the function of '%s',ifindex:0x%x",__FILE__,__func__,ifindex);

	HASH_BUCKET_LOOP(pbucket,cursor,cfm_sess_table_new)
	{
		psess = pbucket->data;
		ifm_get_name_by_ifindex(psess->ifindex,str1);
		
		ifm_get_usp_by_ifindex(ifindex,&usp);

		
		if((IFM_PARENT_IFINDEX_GET(psess->ifindex) == ifindex) && (psess->reconfig_flag==CFM_IF_INIT_FLAG))
		{
			zlog_debug(CFM_DBG_COMMN,"'%s',find ifindex:0x%x,psess->ifindex:0x%x",__func__,ifindex,psess->ifindex);

			if(usp.type != IFNET_TYPE_TRUNK)
				psess->reconfig_flag = CFM_IF_DEL_FLAG;

#if 0			
			ret = ipc_send_hal_wait_ack( psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_SESSION, IPC_OPCODE_DELETE, 0);
#endif
			ret = cfm_ipc_send_wait_ack( psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_SESSION, IPC_OPCODE_DELETE, 0);
			if(ret == 0)
			{			
				zlog_debug(CFM_DBG_COMMN,"send delete session msg to hal ok,%s,session %d",str1,psess->sess_id); 	

				psess->ifindex_del = psess->ifindex;
				psess->ifindex = 0;
				psess->state = OAM_STATUS_DISABLE;			
			}
			else
			{
				zlog_err("send delete session msg to hal fail,%s,session %d",str1,psess->sess_id); 	
			}
		}
	}

	return;
}

/**
 * @brief      :2200设备物理口添加时只做标记，包括所有相关子接口
 * @param[in ] : ifindex，接口索引
 * @param[out] : 
 * @return     : 
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

void cfm_if_add(uint32_t ifindex)
{
	struct cfm_sess 	*psess=NULL;
    struct hash_bucket  *pbucket = NULL;
    int 				 cursor;
	char 				 str1[50];
	struct ifm_usp usp;	
	
	zlog_debug(CFM_DBG_COMMN,"%s:Entering the function of '%s',ifindex:0x%x",__FILE__,__func__,ifindex);

	ifm_get_usp_by_ifindex(ifindex,&usp);

	
	if(usp.type == IFNET_TYPE_TRUNK)
		return;

	HASH_BUCKET_LOOP(pbucket,cursor,cfm_sess_table_new)
	{
		psess = pbucket->data;
		ifm_get_name_by_ifindex(psess->ifindex,str1);
		
		if((IFM_PARENT_IFINDEX_GET(psess->ifindex_del) == ifindex) && (psess->reconfig_flag ==CFM_IF_DEL_FLAG))
		{		
			zlog_debug(CFM_DBG_COMMN,"'%s',find ifindex:0x%x,psess->ifindex:0x%x,session:%d,reconfig_flag set 1",__func__,
				ifindex,psess->ifindex,psess->sess_id);

			psess->reconfig_flag = CFM_IF_ADD_FLAG;

			/*if(usp.type == IFNET_TYPE_TRUNK)
			{
				if(psess->ma_index && psess->local_mep && psess->remote_mep)
				{
					psess->ifindex = psess->ifindex_del;
					psess->ifindex_del = 0;
					psess->reconfig_flag = CFM_IF_INIT_FLAG;			
					psess->state = OAM_STATUS_UP;
					cfm_session_clear_alarm(psess);
					
					ret = ipc_send_hal_wait_ack( psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_SESSION, IPC_OPCODE_ADD, 0);
					if(ret == 0)
					{			
						zlog_debug("send add session msg to hal ok,%s,session %d",str1,psess->sess_id); 	
					}
					else
					{
						zlog_err("send add session msg to hal fail,%s,session %d",str1,psess->sess_id); 	
					}
				}
				
			}*/
		}
	}

}

/**
 * @brief      :2200设备物理口添加并将接口下所有配置恢复完成后，再将cfm配置下发，包括所有相关子接口
 * @param[in ] : ifindex，接口索引
 * @param[out] : 
 * @return     : 
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

void cfm_session_reconfig(uint32_t ifindex)
{
	struct cfm_sess 	*psess=NULL;
    struct hash_bucket  *pbucket = NULL;
    int 				 cursor;
	int 				 ret;
	char 				 str1[50];
	
	zlog_debug(CFM_DBG_COMMN,"%s:Entering the function of '%s'",__FILE__,__func__);

	HASH_BUCKET_LOOP(pbucket,cursor,cfm_sess_table_new)
	{
		psess = pbucket->data;
		ifm_get_name_by_ifindex(psess->ifindex,str1);
		
		if((IFM_PARENT_IFINDEX_GET(psess->ifindex_del) == ifindex) && (psess->reconfig_flag==CFM_IF_ADD_FLAG))
		{		
			zlog_debug(CFM_DBG_COMMN,"'%s',ifindex:0x%x, psess->ifindex:0x%x",__func__,ifindex,psess->ifindex);
			
			if(psess->ma_index && psess->local_mep && psess->remote_mep)
			{
				psess->ifindex = psess->ifindex_del;
				psess->ifindex_del = 0;
				psess->reconfig_flag = CFM_IF_INIT_FLAG;			
				psess->state = OAM_STATUS_UP;
				cfm_session_clear_alarm(psess);
#if 0
				ret = ipc_send_hal_wait_ack( psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_SESSION, IPC_OPCODE_ADD, 0);
#endif
				ret = cfm_ipc_send_wait_ack(psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_SESSION, IPC_OPCODE_ADD, 0);
				if(ret == 0)
				{			
					zlog_debug(CFM_DBG_COMMN,"send add session msg to hal ok,%s,session %d",str1,psess->sess_id); 	
				}
				else
				{
					zlog_err("send add session msg to hal fail,%s,session %d",str1,psess->sess_id); 	
				}
			}
		}
	}

}

int  cfm_session_perfm_enable(gpnSockMsg *pgnNsmMsgSp)/* ´´½¨Ò»¸ö Mep session */
{
	struct cfm_sess *psess=NULL;
	uchar dmac[6]={0x0,0x0,0x0,0x0,0x0,0x0};
	uint16_t sess_id;
	uint16_t interval;
	UINT32 enStat=0;
	UINT32 scanType;
	//objLogicDesc portIndex;
	optObjOrient portIndex;
	zlog_notice("Entering %s ,pgnNsmMsgSp:%p",__func__,pgnNsmMsgSp);

	if(pgnNsmMsgSp == NULL)
	{
		zlog_err("pgnNsmMsgSp is NULL");
		goto exit;
	}
	
	sess_id  = pgnNsmMsgSp->iMsgPara2&0xffff;
	interval = pgnNsmMsgSp->iMsgPara8;

	zlog_debug(CFM_DBG_COMMN,"sess_id %d,interval:%d",sess_id,interval);

	psess = cfm_session_lookup_new(sess_id);

	if(psess == NULL)
	{
		zlog_err("session %d not exist",sess_id);
		goto exit;
	}

	if(interval<5 || interval>3600)
	{
		zlog_err("invalid interval:%d",interval);
		goto exit;
	}

	
    if(psess->state == OAM_STATUS_DISABLE)
    {   
        zlog_err("CFM-session is not enable");
		goto exit;
    }
    if(psess->lm_enable == OAM_STATUS_ENABLE || psess->dm_enable == OAM_STATUS_ENABLE)
    {
        zlog_err("LM or DM is running,lm_enable:%d,dm_enable:%d",psess->lm_enable,psess->dm_enable);
		goto exit;
    }

    if(psess->psla == NULL)
    {
        psess->psla = cfm_session_create_sla(psess);
        if(psess->psla == NULL)
        {
            zlog_err("Malloc fail in cfm_session_create_sla()");
			goto exit;
        }
    }

	if(psess->dmac_valid)
	{
		memcpy(dmac,psess->dmac,6);
	}
	else
	{
		zlog_err("remote mep mac not exist");
		goto exit;
	}

	enStat = 1;
	psess->perfm_enable = 1;
    psess->lm_interval  = interval;
    psess->dm_interval  = interval;
	memcpy(psess->lm_dmac,dmac,6);
	memcpy(psess->dm_dmac,dmac,6);
	
    cfm_session_lm_enable(psess);
    cfm_session_dm_enable(psess);

exit:
	memcpy(&(portIndex.devIndex), &(pgnNsmMsgSp->iMsgPara1), sizeof(objLogicDesc));
	scanType = pgnNsmMsgSp->iMsgPara7;

	gpnSockStatMsgApiPortStatMonEnableRsp(&portIndex, scanType, enStat);
	
	return ERRNO_SUCCESS;

}



int  cfm_session_perfm_disable(gpnSockMsg *pgnNsmMsgSp)/* ´´½¨Ò»¸ö Mep session */
{
	struct cfm_sess *psess=NULL;
	uchar dmac[6]={0x0,0x0,0x0,0x0,0x0,0x0};
	uint16_t sess_id;
	UINT32 enStat=0;
	UINT32 scanType;
	//objLogicDesc portIndex;
	optObjOrient portIndex;
	
	zlog_notice("Entering %s ,pgnNsmMsgSp:%p",__func__,pgnNsmMsgSp);

	if(pgnNsmMsgSp == NULL)
	{
		zlog_err("pgnNsmMsgSp is NULL");
		goto exit;
	}
	
	sess_id  = pgnNsmMsgSp->iMsgPara2&0xffff;

	zlog_debug(CFM_DBG_COMMN,"sess_id %d",sess_id);

	psess = cfm_session_lookup_new(sess_id);

	if(psess == NULL)
	{
		zlog_err("session %d not exist",sess_id);
		goto exit;
	}

	
    if(psess->lm_enable != OAM_STATUS_ENABLE && psess->dm_enable != OAM_STATUS_ENABLE)
    {
        zlog_err("LM and DM is not running,lm_enable:%d,dm_enable:%d",psess->lm_enable,psess->dm_enable);
		goto exit;
    }

	enStat = 1;
	psess->perfm_enable = 0;
    psess->lm_interval  = 0;
    psess->dm_interval  = 0;
	memcpy(psess->lm_dmac,dmac,6);
	memcpy(psess->dm_dmac,dmac,6);
	
    cfm_session_lm_disable(psess);
    cfm_session_dm_disable(psess);

exit:
	memcpy(&(portIndex.devIndex), &(pgnNsmMsgSp->iMsgPara1), sizeof(objLogicDesc));
	scanType = pgnNsmMsgSp->iMsgPara7;
	
	gpnSockStatMsgApiPortStatMonDisableRsp(&portIndex, scanType, enStat);
	
	return ERRNO_SUCCESS;

}




int  cfm_session_perfm_get(gpnSockMsg *pgnNsmMsgSp)/* ´´½¨Ò»¸ö Mep session */
{
	struct cfm_sess *psess=NULL;
	gpnStatPtnMepMData mepdata;   
	struct sla_measure *psla;
	//objLogicDesc portIndex;
	optObjOrient portIndex;
	uint16_t sess_id;

	zlog_notice("Entering %s ,pgnNsmMsgSp:%p",__func__,pgnNsmMsgSp);

	if(pgnNsmMsgSp == NULL)
	{
		zlog_err("pgnNsmMsgSp is NULL");
		return ERRNO_FAIL;
	}
	
	sess_id  = pgnNsmMsgSp->iMsgPara2&0xffff;

	zlog_debug(CFM_DBG_COMMN,"sess_id %d",sess_id);

	psess = cfm_session_lookup_new(sess_id);

	if(psess == NULL)
	{
		zlog_err("session %d not exist",sess_id);
		return ERRNO_FAIL;
	}

	psla = psess->psla;

	if(psla)
	{
		mepdata.statPtnMepLDROPRATA = psla->lm_mean;
		mepdata.statPtnMepRDROPRATA = psla->lm_mean_peer;
		mepdata.statPtnMepDELAY = psla->dm_mean;
		mepdata.statPtnMepDELAYCHG = psla->jitter_mean;
	}


	memcpy(&(portIndex.devIndex), &(pgnNsmMsgSp->iMsgPara1), sizeof(objLogicDesc));

	gpnSockStatMsgApiTxPtnMepMStatGetRsp(&portIndex, GPN_STAT_T_PTN_MEP_M_TYPE, \
						&mepdata, GPN_STAT_PARA_PTN_MEP_M_STAT_DATA_SIZE);

	
	return ERRNO_SUCCESS;

}
