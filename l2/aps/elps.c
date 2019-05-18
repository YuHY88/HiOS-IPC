#include <string.h>
#include <stdlib.h>
#include <lib/hash1.h>
#include <lib/memtypes.h>
#include <lib/errcode.h>
#include <lib/types.h>
#include <lib/memory.h>
#include <lib/module_id.h>
#include <lib/msg_ipc.h>
#include <lib/msg_ipc_n.h>
#include <lib/log.h>
#include <lib/ifm_common.h>
#include <lib/alarm.h>
#include <lib/gpnSocket/socketComm/gpnGlobalPortIndexDef.h>
#include <lib/gpnSocket/socketComm/gpnSockMsgDef.h>



#include "aps/elps.h"
#include "aps/elps_cmd.h"
#include "aps/elps_pkt.h"
#include "cfm/cfm_session.h"
#include "l2_msg.h"

void elps_init(void)
{
	elps_sess_table_init(ELPS_NUM);
	aps_pkt_register();
	elps_cmd_init();
}

void elps_vlan_map_set(unsigned char *bitmap, uint16_t vlan_start,uint16_t vlan_end)
{
	uint16_t vlan= 0;
	
	ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);

	for(vlan = vlan_start; vlan < vlan_end+1; vlan++)
	{
		bitmap[vlan / 8] |= (1 << (vlan % 8));
	}
	
    	return ;
}

void elps_vlan_map_unset(unsigned char *bitmap, uint16_t vlan_start,uint16_t vlan_end)
{
	uint16_t vlan = 0;
	
	ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
	
	for(vlan=vlan_start; vlan<vlan_end+1; vlan++)
	{
		bitmap[vlan / 8] &= ~(1 << (vlan % 8));
	}
 
    	return ;
}

int elps_vlan_map_isset(unsigned char *bitmap, uint16_t vlan_start,uint16_t vlan_end)
{
	uint16_t vlan = 0;

	ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
	
	for(vlan = vlan_start; vlan<vlan_end+1; vlan++)
	{
		if((bitmap[vlan/8] & (1 <<(vlan%8))))
		{
			return vlan;
		}
	}
	
	return 0;
}
static unsigned int elps_compute_sess(void *hash_key)
{
    if ( NULL == hash_key )
    {
        return 0;
    }
    return ( ( unsigned int ) hash_key );
}

static int elps_compare_sess(void *item, void *hash_key)
{    
    if((NULL == item) || (NULL == hash_key))
    {
	return ERRNO_PARAM_ILLEGAL;
    }
    return ((struct hash_bucket *)item)->hash_key == hash_key ? 0:1;
}

void elps_sess_table_init ( unsigned int size )

{
    if (0 == size)
    {
        ELPS_LOG_DBG("size must great than or equal to zero!\n");
        return ;
    }   
    hios_hash_init(&elps_session_table, size, elps_compute_sess, elps_compare_sess);
    return ;
	
}


struct elps_sess *elps_sess_create(uint16_t sess_id)/* 创建一个 elps session */
{
	struct elps_sess * psess;
	
	ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);

	psess = (struct elps_sess *)XMALLOC(MTYPE_ELPS_SESS, sizeof(struct elps_sess));
	
	if(NULL == psess)
	{
		return NULL;
	}
	memset(psess,0,sizeof(struct elps_sess));

	psess->info.sess_id = sess_id;
	psess->info.keepalive = 5;
	psess->info.priority = 6; 
	psess->info.wtr = 5;
	psess->info.failback = FAILBACK_ENABLE;
	psess->pholdoff_timer = 0;
	psess->pkeepalive_timer = 0;
	psess->pwtr_timer = 0;
	return psess;

}


int 	elps_sess_add(struct elps_sess *psess) /* session 添加到 hash 表 */
{
	struct hash_bucket *sess_item = NULL;
	int ret;

	ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
	
	//new
	sess_item = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if(NULL == sess_item)
	{
		return ERRNO_MALLOC;
	}
	
	memset(sess_item, 0, sizeof(struct hash_bucket));
	
	sess_item->data = psess;
	sess_item->hash_key = (void *)(uint32_t)psess->info.sess_id;
	
	ret = hios_hash_add(&elps_session_table, sess_item);
	
	if(ret != 0)
	{
		XFREE(MTYPE_HASH_BACKET, sess_item);
		return ERRNO_OVERSIZE;
	}

	return ERRNO_SUCCESS;
	
}

int 	elps_sess_delete(uint16_t sess_id) /* 删除一个 elps session */
{
	struct hash_bucket *sess_item = NULL;
	struct elps_sess *sess_node = NULL;
    struct cfm_sess *unbind_cfm_sess = NULL;

	ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);


	sess_item = hios_hash_find(&elps_session_table,(void *)(uint32_t)sess_id);

	if(sess_item)
	{
		sess_node = sess_item->data;
        if(sess_node)
        {
            if(sess_node->info.master_cfm_session)
            {
                unbind_cfm_sess = cfm_session_unbind_elps(sess_node->info.master_cfm_session);
                if(unbind_cfm_sess == NULL)
                {
                    return ERRNO_FAIL;
                }
            }
            if(sess_node->info.backup_cfm_session)
            {
                unbind_cfm_sess = cfm_session_unbind_elps(sess_node->info.backup_cfm_session);
                if(unbind_cfm_sess == NULL)
                {
                    return ERRNO_FAIL;
                }
            }
    		XFREE(MTYPE_ELPS_SESS, sess_node);
        }
        
        hios_hash_delete(&elps_session_table, sess_item);
        XFREE(MTYPE_HASH_BACKET, sess_item);

	}	
    else
    {
        return ERRNO_NOT_FOUND;
    }

	return ERRNO_SUCCESS;
	//delet sesslist

}


struct elps_sess *elps_sess_lookup(uint16_t sess_id) /* 查找一个elps session */
{
	struct hash_bucket *sess_item = NULL;
	struct elps_sess*sess_node = NULL;

	if(sess_id<1 || sess_id>ELPS_NUM)
	{
		return NULL;
	}

	sess_item = hios_hash_find(&elps_session_table,(void  *)(uint32_t)sess_id);

	if(sess_item)
	{
		sess_node = sess_item->data;

		return sess_node;		
	}	

	return NULL;

}



int elps_sess_enable(struct elps_sess *psess)

{
    int ret = 0;
	
    if((NULL == psess) || (psess->info.status == APS_STATUS_ENABLE))
    {   
        return ERRNO_SUCCESS;
    }

    psess->info.status = APS_STATUS_ENABLE;
    
    /* send sess-enable to hal */	
    ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct elps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ELPS, OAM_SUBTYPE_ELPS_SESSION, IPC_OPCODE_ADD, psess->info.sess_id);
    if(ret != ERRNO_SUCCESS)
    {
		psess->info.status = APS_STATUS_DISABLE;
		ELPS_LOG_DBG("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
		return ERRNO_IPC;
    }
	/* modify for ipran by lipf, 2018/4/25 */
	struct ifm_usp usp;
	struct gpnPortInfo gPortInfo;
	
	memset(&usp, 0, sizeof(struct ifm_usp));	
	usp.type = IFNET_TYPE_MAX;
	usp.unit = 1;
	usp.slot = 0;
	usp.port = psess->info.backup_port;	
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
	gPortInfo.iIfindex = ifm_get_ifindex_by_usp(&usp);
	ipran_alarm_port_register(&gPortInfo);
	
    return ERRNO_SUCCESS;	
}

int elps_sess_disable(struct elps_sess *psess)

{
	
	int ret = 0;

	if((NULL == psess) || (psess->info.status == APS_STATUS_DISABLE))
	{
    		return  ERRNO_SUCCESS;
	}

	if(psess->info.current_status != ELPS_NO_REQUEST)
	{
		ELPS_LOG_DBG("%-15s[Func:%s]:The elps session current status is not NO request.--Line:%d",__FILE__, __FUNCTION__, __LINE__);
	}
    
	/* send sess-disable to hal */
	ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct elps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ELPS, OAM_SUBTYPE_ELPS_SESSION, IPC_OPCODE_DELETE, psess->info.sess_id);
	if(ret != ERRNO_SUCCESS)
	{
		zlog_err("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
		return ERRNO_IPC;
	}
	psess->info.status = APS_STATUS_DISABLE;
	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
	gPortInfo.iIfindex = psess->info.backup_port;
	ipran_alarm_port_unregister(&gPortInfo);
	return ERRNO_SUCCESS;
	
}


int elps_sess_forceswitch(struct elps_sess *psess, int is_creat)
{
	int ret;
	if (psess)
	  {
		if(is_creat)
		{
			if((psess->info.current_status == ELPS_LOCKOUT_OF_PROTECTION)||
			(psess->info.current_status == ELPS_SIGNAL_FAIL_FOR_PROTECTION))
			{
				return ELPS_LOWER_PRIO_CMD;
			}
			else if(psess->info.current_status ==ELPS_FORCED_SWITCH)
			{
				return ELPS_IS_ALREADY_FS;
			}
			psess->info.current_event = LOCAL_ELPS_FS;
		}
		else
		{
			if(psess->info.current_status != ELPS_FORCED_SWITCH)
			{
				return ELPS_IS_NOT_FS;		
			}
			psess->info.current_event = LOCAL_ELPS_CLEAR;
		}
				
		ret = elps_fsm(psess, NULL,psess->info.current_event);
		if(ret)
		{
			ELPS_LOG_DBG("[Func:%s]:elps_fsm return error %d.--Line:%d", __FUNCTION__, ret ,__LINE__);
			return ret;
		}
		
		return ERRNO_SUCCESS;
	  }
	
	 return ERRNO_FAIL;
}

int elps_sess_manualswitch(struct elps_sess *psess, int is_creat)
{
	int ret;
	if(psess)
	{
		if(is_creat)
		{
			if((psess->info.current_status == ELPS_LOCKOUT_OF_PROTECTION)||
			(psess->info.current_status == ELPS_SIGNAL_FAIL_FOR_PROTECTION)||
			(psess->info.current_status == ELPS_FORCED_SWITCH)||
			(psess->info.current_status == ELPS_SIGNAL_FAIL_FOR_WORKING))
			{
				return ELPS_LOWER_PRIO_CMD;
			}
			if(psess->info.current_status == ELPS_MANUAL_SWITCH)
			{
				return ELPS_IS_ALREADY_MS;
			}
			psess->info.current_event = LOCAL_ELPS_MS;
		}
		else
		{
			if(psess->info.current_status != ELPS_MANUAL_SWITCH)
			{
				return ELPS_IS_NOT_MS;
			}
			psess->info.current_event = LOCAL_ELPS_CLEAR;
		}		
		ret = elps_fsm(psess,NULL, psess->info.current_event);	
		if(ret)
		{
			ELPS_LOG_DBG("[Func:%s]:elps_fsm return error %d.--Line:%d", __FUNCTION__, ret ,__LINE__);
			return ret;
		}
		return ERRNO_SUCCESS;
	}
	return ERRNO_FAIL;
}

int elps_sess_lock(struct elps_sess *psess, int is_creat)
{	
	int ret;
	if(psess)
	{
		if(is_creat)
		{
			if(psess->info.current_status == ELPS_LOCKOUT_OF_PROTECTION)
			{
				return ELPS_IS_ALREADY_LOP;
			}
			psess->info.current_event = LOCAL_ELPS_LOP;
		}
		else
		{
			if(psess->info.current_status != ELPS_LOCKOUT_OF_PROTECTION)
			{
				return ELPS_IS_NOT_LOP;
			}
			psess->info.current_event = LOCAL_ELPS_CLEAR;
		}
		
		ret = elps_fsm(psess, NULL,psess->info.current_event);	
		if(ret)
		{
			ELPS_LOG_DBG("[Func:%s]:elps_fsm return error %d.--Line:%d", __FUNCTION__, ret ,__LINE__);
			return ret;
		}
		return ERRNO_SUCCESS;
	}
	return ERRNO_FAIL;
}

int elps_get_bulk(struct hhrelps_snmp *elps_buf, uint32_t session_id, uint32_t elps_max)
{
    struct hash_bucket *pbucket = NULL;
    struct elps_sess *psess = NULL;
    uint32_t elps_cnt = 0;
	int cursor = 0;
   	char ifname[NAME_STRING_LEN];
	int flag = 0;
	ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);

	if(0 == session_id)
	{
		HASH_BUCKET_LOOP(pbucket, cursor, elps_session_table)
		{
			psess = (struct elps_sess *)pbucket->data;
	        if (NULL == psess)
	        {
				continue;
	        }

        	memcpy((elps_buf + elps_cnt), &(psess->info), sizeof(struct elps_info));
			if(0 != psess->info.master_port)
			{
				memset(ifname, 0, NAME_STRING_LEN);
				ifm_get_name_by_ifindex(psess->info.master_port, ifname);
				memcpy((elps_buf + elps_cnt)->master_name, ifname, NAME_STRING_LEN);
			}
			if(0 != psess->info.backup_port)
			{
				memset(ifname, 0, NAME_STRING_LEN);
				ifm_get_name_by_ifindex(psess->info.backup_port, ifname);
				memcpy((elps_buf + elps_cnt)->backup_name, ifname, NAME_STRING_LEN);
			}
			elps_cnt++;
        	if (elps_cnt == elps_max)
        	{
            	return elps_cnt;
        	}
	    }
		ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d, elps_cnt %d",__FILE__,__func__,__LINE__, elps_cnt);

		return elps_cnt;
	}
	else
	{
		HASH_BUCKET_LOOP(pbucket, cursor, elps_session_table)
		{
			psess = (struct elps_sess *)pbucket->data;
	        if (NULL == psess)
	        {
				continue;
	        }

			if(0 == flag)
			{
				if(session_id == psess->info.sess_id)
				{
					flag = 1;
				}
				continue;
			}
			else
			{
				memcpy((elps_buf + elps_cnt), &(psess->info), sizeof(struct elps_info));
				if(0 != psess->info.master_port)
				{
					memset(ifname, 0, NAME_STRING_LEN);
					ifm_get_name_by_ifindex(psess->info.master_port, ifname);
					memcpy((elps_buf + elps_cnt)->master_name, ifname, NAME_STRING_LEN);
				}
				if(0 != psess->info.backup_port)
				{
					memset(ifname, 0, NAME_STRING_LEN);
					ifm_get_name_by_ifindex(psess->info.backup_port, ifname);
					memcpy((elps_buf + elps_cnt)->backup_name, ifname, NAME_STRING_LEN);
				}
				elps_cnt++;
	        	if (elps_cnt == elps_max)
	        	{
					flag = 0;
	            	return elps_cnt;
	        	}
			}
		}
		flag = 0;
		return elps_cnt;
	}
		
}

int elps_msg_rcv_get_bulk(struct ipc_msghdr_n *pmsghdr, void *pdata)
{
	uint32_t elps_max = IPC_MSG_LEN/sizeof(struct hhrelps_snmp);
    struct hhrelps_snmp elps_buf[elps_max];
	uint32_t elps_cnt = 0;
	int ret = 0;

	ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
	memset(elps_buf, 0, elps_max * sizeof(struct hhrelps_snmp));
    elps_cnt = elps_get_bulk(elps_buf, pmsghdr->msg_index, elps_max);
	if (elps_cnt > 0)
	{
		ret = ipc_send_reply_n2(elps_buf, elps_cnt * sizeof(struct hhrelps_snmp), elps_cnt, pmsghdr->sender_id,
                             MODULE_ID_L2, IPC_TYPE_ELPS, pmsghdr->msg_subtype, 0,pmsghdr->msg_index, IPC_OPCODE_REPLY);
	}
	else
	{
		// ret = l2_msg_send_noack(ERRNO_NOT_FOUND, pmsghdr, pmsghdr->msg_index);
		ret = ipc_send_reply_n2(NULL,0,0, pmsghdr->sender_id, MODULE_ID_L2, IPC_TYPE_ELPS, pmsghdr->msg_subtype, 0, pmsghdr->msg_index, IPC_OPCODE_NACK);
	}

	return ret;
}


