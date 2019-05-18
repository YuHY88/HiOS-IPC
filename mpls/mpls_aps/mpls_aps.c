#include <stdlib.h>
#include <lib/hash1.h>
#include <lib/memtypes.h>
#include <lib/errcode.h>
#include <lib/types.h>
#include <lib/memory.h>
#include <lib/module_id.h>
#include <lib/msg_ipc_n.h>
#include <lib/log.h>
#include <lib/alarm.h>
#include <lib/gpnSocket/socketComm/gpnGlobalPortIndexDef.h>
#include <lib/gpnSocket/socketComm/gpnSockMsgDef.h>


#include "mpls_aps/mpls_aps.h"
#include "mpls_aps/mpls_aps_cmd.h"
#include "mpls_aps/mpls_aps_fsm.h"



void mplsaps_init(void)
{
	mplsaps_sess_table_init(MPLSAPS_NUM);
	mplsaps_pkt_register();
	mplsaps_cmd_init();
}


static unsigned int mplsaps_compute_sess(void *hash_key)
{
	OAM_DEBUG();

    if ( NULL == hash_key )
    {
        return 0;
    }
	
    return ( ( unsigned int ) hash_key );
}

static int mplsaps_compare_sess(void *item, void *hash_key)
{    
	OAM_DEBUG();

	if((NULL == item) || (NULL == hash_key))
    {
		return ERRNO_PARAM_ILLEGAL;
    }

    return ((struct hash_bucket *)item)->hash_key == hash_key ? 0:1;
}

void mplsaps_sess_table_init ( unsigned int size )
{
	OAM_DEBUG();

    if (0 == size)
    {
        return ;
    }   
    hios_hash_init(&mplsaps_session_table, size, mplsaps_compute_sess, mplsaps_compare_sess);

    return ;
}


struct aps_sess *mplsaps_sess_create(uint16_t sess_id)/* 创建一个aps session */
{
	struct aps_sess * psess;
	
	OAM_DEBUG();

	psess = (struct aps_sess *)XMALLOC(MTYPE_MPLSAPS_SESS, sizeof(struct aps_sess));
	
	if(NULL == psess)
	{
		OAM_ERROR("malloc fial!\n");
		return NULL;
	}
	memset(psess,0,sizeof(struct aps_sess));
	
	psess->info.sess_id = sess_id;
	psess->info.keepalive = 5;
	psess->info.priority = 6; 
	psess->info.wtr = 300;
	psess->info.backup = BACKUP_STATUS_SLAVE;
	psess->info.failback = FAILBACK_ENABLE;
	psess->info.type = APS_TYPE_MAX;
	return psess;

}


int mplsaps_sess_add(struct aps_sess *psess) /* session 添加到 hash 表 */
{
	struct hash_bucket *sess_item = NULL;
	int ret;

	OAM_DEBUG();
	
	//new
	sess_item = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if(NULL == sess_item)
	{
		OAM_ERROR("malloc fial!\n");
		return ERRNO_MALLOC;
	}
	
	memset(sess_item, 0, sizeof(struct hash_bucket));
	
	sess_item->data = psess;
	sess_item->hash_key = (void *)(uint32_t)psess->info.sess_id;
	
	ret = hios_hash_add(&mplsaps_session_table, sess_item);
	
	if(ret != 0)
	{
		XFREE(MTYPE_HASH_BACKET, sess_item);
		
		OAM_ERROR("add hash fial!\n");
		return ERRNO_OVERSIZE;
	}

	return ERRNO_SUCCESS;
	
}

int mplsaps_sess_delete(uint16_t sess_id) /* 删除一个 aps session */
{
	struct hash_bucket *sess_item = NULL;
	struct aps_sess *sess_node = NULL;

	OAM_DEBUG();

	sess_item = hios_hash_find(&mplsaps_session_table, (void  *)(uint32_t)sess_id);


	if(sess_item)
	{
		hios_hash_delete(&mplsaps_session_table, sess_item);

		sess_node = sess_item->data;

		XFREE(MTYPE_MPLSAPS_SESS, sess_node);
		XFREE(MTYPE_HASH_BACKET, sess_item);

	}	

	return ERRNO_SUCCESS; 

}


struct aps_sess *mplsaps_sess_lookup(uint16_t sess_id) /* 查找一个aps session */
{
	struct hash_bucket *sess_item = NULL;
	struct aps_sess *sess_node = NULL;

	OAM_DEBUG();

	if(sess_id < 1 || sess_id > MPLSAPS_NUM)
	{
		OAM_ERROR("sess_id is wrong!\n");
		return NULL;
	}

	sess_item = hios_hash_find(&mplsaps_session_table, (void  *)(uint32_t)sess_id);

	if(sess_item)
	{
		sess_node = sess_item->data;

		return sess_node;		
	}	

	return NULL;

}



int mplsaps_sess_enable(struct aps_sess *psess)
{
    int ret = 0;
	struct gpnPortInfo gPortInfo;

	OAM_DEBUG();
	
    if((NULL == psess) || (psess->info.status == MPLS_APS_STATUS_ENABLE))
    {   
		OAM_ERROR("pointer is NULL\n");
        return ERRNO_SUCCESS;
    }

   /* send sess-enable to hal */	
    ret = ipc_send_msg_n2(psess, sizeof(struct aps_sess), 1, MODULE_ID_HAL, MODULE_ID_MPLS,
    					IPC_TYPE_MPLSAPS, OAM_SUBTYPE_SESSION, IPC_OPCODE_ADD, psess->info.sess_id);
    if(ret != ERRNO_SUCCESS)
    {
		psess->info.status = MPLS_APS_STATUS_DISABLE;
		
		OAM_ERROR("send to hal fail!\n");
		return ERRNO_IPC;
    }

    psess->info.status = MPLS_APS_STATUS_ENABLE;
    if(psess->info.type == APS_TYPE_LSP)
    {
		/* modify for ipran by lipf, 2018/4/25 */		
		memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
		gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
		gPortInfo.iMsgPara1 = psess->info.backup_index;
		ipran_alarm_port_register(&gPortInfo);
    }
	else if(psess->info.type == APS_TYPE_PW)
	{
		/* modify for ipran by lipf, 2018/4/25 */		
		memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
		gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
		gPortInfo.iMsgPara1 = psess->info.backup_index;
		ipran_alarm_port_register(&gPortInfo);
	}
    return ERRNO_SUCCESS;	
}

int mplsaps_sess_disable(struct aps_sess *psess)
{
	int ret = 0;

	OAM_DEBUG();

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	
	if((NULL == psess) || (psess->info.status == MPLS_APS_STATUS_DISABLE))
	{
		OAM_ERROR("pointer is NULL\n");
    	return  ERRNO_SUCCESS;
	}

	gmplsaps.sess_enable--;
	
	if(psess->info.current_status != APS_STATE_NR)
	{
		OAM_DEBUG("sess_id==%d current_status==%d \n", psess->info.sess_id, psess->info.current_status);
	}
	/* send sess-enable to hal */	
	ret = ipc_send_msg_n2(psess, sizeof(struct aps_sess), 1, MODULE_ID_HAL, MODULE_ID_MPLS,
					IPC_TYPE_MPLSAPS, OAM_SUBTYPE_SESSION, IPC_OPCODE_DELETE, psess->info.sess_id);
	if(ret != ERRNO_SUCCESS)
	{
		psess->info.status = MPLS_APS_STATUS_DISABLE;
		
		OAM_ERROR("send to hal fail!\n");
		return ERRNO_IPC;
	}
    
    psess->info.status = MPLS_APS_STATUS_DISABLE;
	if(psess->info.type == APS_TYPE_LSP)
    {
		gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
		gPortInfo.iMsgPara1 = psess->info.backup_index;
		ipran_alarm_port_unregister(&gPortInfo);
    }
	else if(psess->info.type == APS_TYPE_PW)
	{
		gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
		gPortInfo.iMsgPara1 = psess->info.backup_index;
		ipran_alarm_port_unregister(&gPortInfo);
	}
	return ERRNO_SUCCESS;
	
}


int mplsaps_sess_forceswitch(struct aps_sess *psess, int is_creat)
{
	int ret;
	
	OAM_DEBUG();
	
	if(NULL == psess)
	{
		OAM_ERROR("pointer is NULL\n");
		return ERRNO_FAIL;
	}
	
	OAM_DEBUG("is_creat %d ", is_creat);
  	if(is_creat)
  	{
  		if((psess->info.current_status == APS_STATE_LO)||
  		(psess->info.current_status == APS_STATE_SF_P))
  		{
  			return MPLSAPS_LOWER_PRIO_CMD;
  		}
  		else if(psess->info.current_status == APS_STATE_FS)
  		{
  			return MPLSAPS_IS_ALREADY_FS;
  		}
  		psess->info.current_event = APS_EVENT_LOCAL_FS;
  	}
  	else
  	{
		
  		if(psess->info.current_status != APS_STATE_FS)
  		{	
  			return MPLSAPS_IS_NOT_FS;		
  		}
  		psess->info.current_event = APS_EVENT_LOCAL_CLEAR;
  	}
	OAM_DEBUG("psess->info.current_event %d ", psess->info.current_event);	

  	ret = mplsaps_fsm(psess,NULL, psess->info.current_event);
  	if(ret)
  	{
  		OAM_ERROR("mplsaps_fsm return error %d.\n", ret);
  		return ret;
  	}
  	
  	return ERRNO_SUCCESS;
}

int mplsaps_sess_manualswitch(struct aps_sess *psess, int is_creat)
{
	int ret;

	OAM_DEBUG();

	if(NULL == psess)
	{
		OAM_ERROR("pointer is NULL\n");
		return ERRNO_FAIL;
	}
	
	OAM_DEBUG("is_creat %d ", is_creat);
	if(is_creat)
	{
		if((psess->info.current_status == APS_STATE_LO)||
		(psess->info.current_status == APS_STATE_SF_P)||
		(psess->info.current_status == APS_STATE_FS)||
		(psess->info.current_status == APS_STATE_SF_W))
		{
			return MPLSAPS_LOWER_PRIO_CMD;
		}
		if(psess->info.current_status == APS_STATE_MS)
		{
			return MPLSAPS_IS_ALREADY_MS;
		}
		psess->info.current_event = APS_EVENT_LOCAL_MS;
	}
	else
	{
		if(psess->info.current_status != APS_STATE_MS)
		{
			return MPLSAPS_IS_NOT_MS;
		}
		psess->info.current_event = APS_EVENT_LOCAL_CLEAR;
	}		
	OAM_DEBUG("psess->info.current_event %d ", psess->info.current_event);	

	ret = mplsaps_fsm(psess, NULL,psess->info.current_event);	
	if(ret)
	{
  		OAM_ERROR("mplsaps_fsm return error %d.\n", ret);
		return ret;
	}

	return ERRNO_SUCCESS;	
}

int mplsaps_sess_lock(struct aps_sess *psess, int is_creat)
{	
	int ret;

	OAM_DEBUG();
	
	if(NULL == psess)
	{
		OAM_ERROR("pointer is NULL\n");
		return ERRNO_FAIL;
	}
	
	OAM_DEBUG("is_creat %d ", is_creat);
	if(is_creat)
	{
		if(psess->info.current_status == APS_STATE_LO)
		{
			return MPLSAPS_IS_ALREADY_LOP;
		}
		psess->info.current_event = APS_EVENT_LOCAL_LO;
	}
	else
	{
		
		if(psess->info.current_status != APS_STATE_LO)
		{
			return MPLSAPS_IS_NOT_LOP;
		}
		psess->info.current_event = APS_EVENT_LOCAL_CLEAR;
	}
	OAM_DEBUG("psess->info.current_event %d ", psess->info.current_event);	

	ret = mplsaps_fsm(psess, NULL,psess->info.current_event);	
	if(ret)
	{
  		OAM_ERROR("mplsaps_fsm return error %d.\n", ret);
		return ret;
	}
	
	return ERRNO_SUCCESS;
}

void mplsaps_unbind_lsp_pw(uint16_t sess_id)
{ 
    struct aps_sess * psess = NULL;

	OAM_DEBUG();
	
    psess = mplsaps_sess_lookup(sess_id);
	if(NULL == psess)
	{
		OAM_ERROR("pointer is NULL\n");
		return;
	}

    if(psess->info.type == APS_TYPE_LSP)
    {
        if(psess->tunnel_pif)
        {
            psess->tunnel_pif->tunnel.aps_id = 0;
            if(psess->tunnel_pif->p_mplstp != NULL && psess->tunnel_pif->p_mplstp->ingress_lsp != NULL)
            {
        	   psess->tunnel_pif->p_mplstp->ingress_lsp->aps_id = 0;
            }
            if(psess->tunnel_pif->p_mplstp != NULL && psess->tunnel_pif->p_mplstp->backup_ingress_lsp != NULL)
            {
        	   psess->tunnel_pif->p_mplstp->backup_ingress_lsp->aps_id = 0;
            }
            if(psess->tunnel_pif->p_mplstp != NULL && psess->tunnel_pif->p_mplstp->egress_lsp!= NULL)
            {
        	   psess->tunnel_pif->p_mplstp->egress_lsp->aps_id = 0;
            }
            if(psess->tunnel_pif->p_mplstp != NULL && psess->tunnel_pif->p_mplstp->backup_egress_lsp!= NULL)
            {
        	   psess->tunnel_pif->p_mplstp->backup_egress_lsp->aps_id = 0;
            }
        }
		
        psess->tunnel_pif = NULL;
        psess->info.master_index = 0;
        psess->info.backup_index =0;
        psess->info.type = APS_TYPE_MAX;
    }
    else if(psess->info.type == APS_TYPE_PW)
    {
        if(psess->pif)
        {     
        	psess->pif->aps_sessid = 0;
        	if(psess->pif->ppw_master != NULL)
        	{
        		psess->pif->ppw_master->aps_id = 0;
        	}
        	if(psess->pif->ppw_slave != NULL)
        	{
        		psess->pif->ppw_slave->aps_id = 0;
        	}
			
        	psess->pif = NULL;
        	psess->info.master_index = 0;
        	psess->info.backup_index =0;
        	psess->info.type = APS_TYPE_MAX;
        }
    }
   
    mplsaps_sess_disable(psess);
}

