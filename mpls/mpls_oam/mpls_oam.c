#include <lib/memory.h>
#include <lib/hash1.h>
#include <lib/index.h>
#include <lib/zassert.h>
#include <lib/log.h>
#include <lib/types.h>
#include <lib/module_id.h>
#include <lib/msg_ipc_n.h>
#include <lib/errcode.h>
#include <lib/command.h>
#include <lib/pkt_type.h>
#include <ftm/pkt_mpls.h>
#include <stdlib.h>
#include <lib/ifm_common.h>
#include "mpls_alarm.h"
#include "mpls_main.h"
#include "mpls_if.h"
#include "lsp_static.h"
#include "lspm.h"
#include "pw.h"
#include "mpls_oam.h"
#include "mpls_oam_ais.h"
#include "mpls_oam_lck.h"
#include "mpls_oam_csf.h"


struct oam_global gmplsoam; /* mpls oam 全局数据结构 */

struct hash_table mplsoam_session_table; /* oam 会话 hash 表，用 session_id 作为 key */
struct hash_table mplsoam_meg_table;     /* oam meg表项hash表，meg-name作为key*/


extern uchar *static_lsp_get_name(uint32_t index);


static unsigned int mplsoam_session_compute(void *hash_key)
{
	if(NULL == hash_key)
	{
		return 0;
	}
	
	return (unsigned int)hash_key;
}

static int mplsoam_session_compare(void *item, void *hash_key)
{
	if((NULL == item) || (NULL == hash_key))
	{
		return ERRNO_PARAM_ILLEGAL;
	}

	return ((struct hash_bucket *)item)->hash_key == hash_key ? 0:1;
}

static unsigned int mplsoam_meg_compute(void *hash_key)
{
	if(NULL == hash_key)
	{
		return 0;
	}
	return hios_hash_string(hash_key);
}

static int mplsoam_meg_compare(void *item, void *hash_key)
{
	struct hash_bucket *pbucket = NULL;
	
	if(NULL == item || NULL == hash_key)
	{
		return 1;
	}
	
	pbucket = (struct hash_bucket *)item;

	if(strlen(pbucket->hash_key) != strlen(hash_key))
	{
		return 1;
	}
	
	if(0 == memcmp(pbucket->hash_key, hash_key, strlen(hash_key)))
	{
		return 0;
	}
	
	return 1;
}


/* 初始化 hash 表 */
void mplsoam_session_table_init ( unsigned int size )
{
    memset(&gmplsoam, 0, sizeof(struct oam_global));
	gmplsoam.global_chtype = 0x8902;
	hios_hash_init(&mplsoam_session_table, size, mplsoam_session_compute, mplsoam_session_compare);
	hios_hash_init(&mplsoam_meg_table, size, mplsoam_meg_compute, mplsoam_meg_compare);
}

/* 创建一个会话 */
struct oam_session *mplsoam_session_create(uint16_t session_id)
{
	struct oam_session *psess = NULL;

	OAM_DEBUG();

	psess = (struct oam_session *)XMALLOC(MTYPE_MPLSOAM_SESS, sizeof(struct oam_session));
	if(NULL == psess)
	{
		OAM_DEBUG("malloc\n");
		return NULL;
	}
	memset(psess, 0, sizeof(struct oam_session));
	psess->client_list = list_new();
	
	/* 创建后先初始化*/
	psess->info.session_id = session_id;
	psess->info.priority = 5;
    psess->info.level = 7;

	if(gmplsoam.g_chtype_enable == 1)
	{
    	psess->info.channel_type = gmplsoam.global_chtype;
	}
	else if(gmplsoam.sess_total == 0)
	{
		psess->info.channel_type = MPLS_CTLWORD_CHTYPE_8902;
		gmplsoam.global_chtype = psess->info.channel_type;
	}
	else
	{
		psess->info.channel_type = gmplsoam.global_chtype;
	}
	
	return psess;
}


/* ��ӵ� hash �� */
int mplsoam_session_add(struct oam_session *psess) 
{
	struct hash_bucket *pbucket = NULL;

	OAM_DEBUG();
	
	if(NULL == psess)
	{
		OAM_DEBUG("pointer NULL\n");
		return ERRNO_PARAM_ILLEGAL;
	}

	if(mplsoam_session_table.hash_size == mplsoam_session_table.num_entries)
	{
		OAM_DEBUG("oversize\n");
		return ERRNO_OVERSIZE;
	}
	
	pbucket = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if(NULL == pbucket)
	{
		OAM_DEBUG("malloc\n");
		return ERRNO_MALLOC;
	}
	memset(pbucket, 0, sizeof(struct hash_bucket));
	
	pbucket->hash_key = (void *)((uint32_t)psess->info.session_id);
	pbucket->data = psess;
	
	hios_hash_add(&mplsoam_session_table, pbucket);

	return ERRNO_SUCCESS;
}


/* 删除一个会话 */
int mplsoam_session_delete(uint16_t session_id)     
{
	struct hash_bucket *pbucket = NULL;
	struct oam_session *psess = NULL;

	OAM_DEBUG();
	
	pbucket = hios_hash_find(&mplsoam_session_table, (void *)((uint32_t)session_id));
	if(NULL == pbucket)
	{
		return  ERRNO_NOT_FOUND;
	}

	/* delete ais */
	psess = (struct oam_session *)pbucket->data;
	if(psess->ais_enable == 1)
	{
		mplsoam_session_ais_disable(psess);
	}
	if(psess->lck_enable == 1)
	{
		mplsoam_session_lck_disable(psess);
	}
	if(psess->csf_enable == 1)
	{
		mplsoam_session_csf_disable(psess);
	}

	/* free client_list */
	if(psess->client_list != NULL)
	{
		if(list_isempty(psess->client_list))
		{
			list_free(psess->client_list);
		}
		else 
		{
			list_delete_all_node(psess->client_list);
			list_free(psess->client_list);
		}
	}

	/* unbind meg */
	if(psess->meg != NULL)
	{
		listnode_delete(psess->meg->session_list, psess);
	}

	/* delete session */
	hios_hash_delete(&mplsoam_session_table, pbucket);
	XFREE(MTYPE_MPLSOAM_SESS, pbucket->data);
	XFREE(MTYPE_HASH_BACKET, pbucket);

	return ERRNO_SUCCESS;
}


/* 查找一个会话 */
struct oam_session *mplsoam_session_lookup(uint16_t session_id) 
{
	struct hash_bucket *pbucket = NULL;

	OAM_DEBUG();

    if(!session_id)
    {
        return NULL;
    }
	pbucket = hios_hash_find(&mplsoam_session_table, (void *)((uint32_t)session_id));
	if(NULL == pbucket)
	{
		return NULL;
	}

	return (struct oam_session *)pbucket->data;
}


/* 会话 up */
void mplsoam_session_up(uint16_t session_id)
{
	struct oam_session *psess = NULL;
    struct pw_info *pwinfo = NULL;

	OAM_DEBUG();

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));

	psess = mplsoam_session_lookup(session_id);
	if((NULL == psess) || (psess->info.state == OAM_STATUS_UP))
	{
		return;
	}

	psess->info.state = OAM_STATUS_UP;

    switch(psess->info.type)
    {
    	case OAM_TYPE_INTERFACE:
			
		    if(psess->alarm_rpt == 1)
		    {
				gPortInfo.iAlarmPort = IFM_VS_2_TYPE;
				gPortInfo.iIfindex = psess->info.index;
				gPortInfo.iMsgPara1 = session_id;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_VS_LOC, GPN_SOCK_MSG_OPT_CLEAN);
		    }
			break;
        case OAM_TYPE_LSP:			
			if(psess->alarm_rpt == 1)
		    {
				gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
				gPortInfo.iMsgPara1 = psess->info.index;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_LSP_LOC, GPN_SOCK_MSG_OPT_CLEAN);
		    }
			mpls_lsp_up(psess->info.index);
            break;
        case OAM_TYPE_TUNNEL:
            break;
        case OAM_TYPE_PW:
			#if 1
            pwinfo = pw_lookup(psess->info.index);
            if(NULL == pwinfo)
            {
                break ;
            }
            pw_detection_protocal_up(pwinfo);
			#endif
			if(psess->alarm_rpt == 1)
		    {
				gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
				gPortInfo.iMsgPara1 = psess->info.index;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_PW_LOC, GPN_SOCK_MSG_OPT_CLEAN);
		    }
            break;

        default:
            break ;
    }
    psess->alarm.alarm_loc = 0;
    
	return;
}


/* 会话 down */
void mplsoam_session_down(uint16_t session_id)
{
	struct oam_session *psess = NULL;
    struct pw_info *pwinfo = NULL;

	OAM_DEBUG();

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));

	psess = mplsoam_session_lookup(session_id);
	if((NULL == psess) || (psess->info.state == OAM_STATUS_DOWN))
	{
		return;
	}

	psess->info.state = OAM_STATUS_DOWN;
	psess->down_cnt++;

    switch(psess->info.type)
    {
    	case OAM_TYPE_INTERFACE:
			if(psess->alarm_rpt == 1)
		    {
				gPortInfo.iAlarmPort = IFM_VS_2_TYPE;
				gPortInfo.iIfindex = psess->info.index;
				gPortInfo.iMsgPara1 = session_id;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_VS_LOC, GPN_SOCK_MSG_OPT_RISE);
		    }

			if(1 == psess->ais_enable)
			{
				mplsoam_ais_start(psess);
			}
			break;
        case OAM_TYPE_LSP:
			if(psess->alarm_rpt == 1)
		    {
				gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
				gPortInfo.iMsgPara1 = psess->info.index;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_LSP_LOC, GPN_SOCK_MSG_OPT_RISE);
		    }
			mpls_lsp_down(psess->info.index);

			if(1 == psess->ais_enable)
			{
				mplsoam_ais_start(psess);
			}
            break;
        case OAM_TYPE_TUNNEL:
            break;
        case OAM_TYPE_PW:
			#if 1
            pwinfo = pw_lookup(psess->info.index);
            if(NULL == pwinfo)
            {
                break ;
            }
            pw_detection_protocal_down(pwinfo);
			#endif
			if(psess->alarm_rpt == 1)
		    {
				gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
				gPortInfo.iMsgPara1 = psess->info.index;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_PW_LOC, GPN_SOCK_MSG_OPT_RISE);
		    }
            break;
        default:
            break ;
    }
    psess->alarm.alarm_loc = 1;

	return;
}


/* 会话使能，pw视图、tunnel视图、接口视图下使能时调用此接口*/
int mplsoam_session_enable(uint16_t session_id, uint32_t index, enum OAM_TYPE type)
{
	int ret;
	struct oam_session *psess = NULL;
	struct gpnPortInfo gPortInfo;

	OAM_DEBUG();

	psess = mplsoam_session_lookup(session_id);
    if(NULL == psess)
    {
		return ERRNO_NOT_FOUND;
    }

	if(psess->info.mp_type == MP_TYPE_MIP)
	{
		return ERRNO_FAIL;
	}
	if(psess->info.state != OAM_STATUS_DISABLE)
	{
        return ERRNO_ALREADY_ENABLE_OAM;
	}

	ret = mplsoam_session_config_complete(psess);
	if(ret)
    {   
		return ERRNO_CONFIG_INCOMPLETE;
    }
    
    psess->info.state = OAM_STATUS_ENABLE;
	psess->info.type = type;
	psess->info.index = index;

    ret = msg_sender_to_other_wait_ack(&(psess->info), sizeof(struct oam_info), 1, MODULE_ID_HAL, MODULE_ID_MPLS,
                        IPC_TYPE_MPLSOAM, OAM_SUBTYPE_SESSION, IPC_OPCODE_ADD, psess->info.session_id);
    if(ret < 0)
    {
	    psess->info.state = OAM_STATUS_DISABLE;
    	psess->info.type = OAM_TYPE_INVALID;
    	psess->info.index = 0;
        OAM_ERROR("Send to hal.\n");
        return ERRNO_IPC;
    }
    else if(ret)
    {
        mplsoam_session_delete(session_id);
        
        OAM_ERROR("oversize.\n");
        return ret;
    }
    gmplsoam.sess_enable++;

	if(psess->info.type == OAM_TYPE_INTERFACE)
	{
		/* modify for ipran by lipf, 2018/4/25 */
		memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));

		gPortInfo.iAlarmPort = IFM_VS_2_TYPE;
		gPortInfo.iIfindex = psess->info.index;
		gPortInfo.iMsgPara1 = psess->info.session_id;
		ipran_alarm_port_register(&gPortInfo);
	}
	return ERRNO_SUCCESS;
}


/* 会话去使能，pw视图、tunnel视图、接口视图下去使能时调用此接口*/
int mplsoam_session_disable(uint16_t session_id)
{
	struct oam_session *psess = NULL;
	int ret;

	OAM_DEBUG();

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));

	psess = mplsoam_session_lookup(session_id);
    if((NULL == psess) || (psess->info.state == OAM_STATUS_DISABLE))
    {
        return ERRNO_SUCCESS;
    }
    
    /* 禁止 oam 会话前先禁止性能测试 lm、dm */
    mplsoam_session_lm_disable(session_id);
    mplsoam_session_dm_disable(session_id);
	mplsoam_session_ais_disable(psess);
	mplsoam_session_lck_disable(psess);
	
	ret = ipc_send_msg_n2(NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_MPLS,
					IPC_TYPE_MPLSOAM, OAM_SUBTYPE_SESSION, IPC_OPCODE_DELETE, psess->info.session_id);
	if(ret != ERRNO_SUCCESS)
	{
	    OAM_ERROR("Send to hal fail!\n");
        return ERRNO_IPC;
	}

    /*清除此会话的所有告警*/
    if(psess->alarm_rpt == 1)
    {
        mplsoam_session_alarm_rpt_disable(psess);    
    }

    memset(&psess->alarm, 0, sizeof(struct oam_alarm_t));
	if(psess->info.type == OAM_TYPE_INTERFACE)
	{
		gPortInfo.iAlarmPort = IFM_VS_2_TYPE;
		gPortInfo.iIfindex = psess->info.index;
		gPortInfo.iMsgPara1 = psess->info.session_id;
		ipran_alarm_port_unregister(&gPortInfo);
	}
	
    /* oam ��lsp��pw��ӿڽ��������ϵ*/
    gmplsoam.sess_enable--;
    psess->info.state = OAM_STATUS_DISABLE;	
    psess->info.index = 0;
    psess->info.type = OAM_TYPE_INVALID;

	/* oam 与lsp、pw或接口解除绑定关系*/
    gmplsoam.sess_enable--;
    psess->info.state = OAM_STATUS_DISABLE;	
    psess->info.index = 0;
    psess->info.type = OAM_TYPE_INVALID;
   
	return ERRNO_SUCCESS;
}


int mplsoam_session_ais_enable(struct oam_session *psess, uint8_t level, uint16_t interval)
{
	OAM_DEBUG();

    if(NULL == psess)
    {
        return ERRNO_FAIL;
    }
    
    if(0 == psess->ais_enable)
    {
        return ERRNO_FAIL;
    }

    if(NULL == psess->pais)
    {
        psess->pais = (struct oam_ais *)malloc(sizeof(struct oam_ais));
        if(NULL == psess->pais)
        {
            return ERRNO_MALLOC;
        }
        memset(psess->pais, 0, sizeof(struct oam_ais));
		psess->pais->ais_interval = interval;
		psess->pais->level = level;
    }
	if(NULL == psess->pais->ais_transport)
	{
		psess->pais->ais_transport = list_new();
	}
	
#if 0
    if(psess->info.state != OAM_STATUS_DOWN)
    {
        return ERRNO_FAIL;
    }
#endif

    mplsoam_ais_pdu_construct(psess);

    mplsoam_ais_start(psess);
    
    return 0;
}

int mplsoam_session_ais_disable(struct oam_session *psess)
{
	OAM_DEBUG();

    if((NULL == psess) || (0 == psess->ais_enable))
    {
        return 1;
    }

    psess->ais_enable = 0;

    COM_THREAD_TIMER_OFF(psess->ais_send);

	if(psess->pais)
	{
		if(psess->pais->ais_transport != NULL)
		{
			if(list_isempty(psess->pais->ais_transport))
			{
				list_free(psess->pais->ais_transport);
			}
			else
			{
				list_delete_all_node(psess->pais->ais_transport);
				list_free(psess->pais->ais_transport);
			}
		}
		free(psess->pais);
		psess->pais = NULL;
	}

    return 0;
}

int mplsoam_session_lck_enable(struct oam_session *psess, uint8_t level, uint16_t interval)
{
	OAM_DEBUG();	

	if((interval != 1 && interval != 60) || NULL == psess)
	{
		return ERRNO_PARAM_ILLEGAL;
	}

	if(psess->lck_enable != 1)
	{
		return ERRNO_FAIL;
	}

	if(psess->plck == NULL)
	{
		psess->plck = (struct oam_lck *)malloc(sizeof(struct oam_lck));
		if(psess->plck == NULL)
		{
			OAM_ERROR("malloc err!\n");
			return ERRNO_FAIL;
		}
		memset(psess->plck, 0, sizeof(struct oam_lck));
		psess->plck->lck_transport = list_new();	
	}
	
	psess->plck->level = level;
	psess->plck->lck_interval = interval;

	mplsoam_lck_pdu_construct(psess);
	
	mplsoam_lck_start(psess);

	return ERRNO_SUCCESS;
}

int mplsoam_session_lck_disable(struct oam_session *psess)
{
	OAM_DEBUG();

	if((NULL == psess) || (0 == psess->lck_enable))
    {
        return ERRNO_SUCCESS;
    }

    psess->lck_enable = 0;

    COM_THREAD_TIMER_OFF(psess->lck_send);

	if(psess->plck)
	{
		if(psess->plck->lck_transport != NULL)
		{
			if(list_isempty(psess->plck->lck_transport))
			{
				list_free(psess->plck->lck_transport);
			}
			else
			{
				list_delete_all_node(psess->plck->lck_transport);
				list_free(psess->plck->lck_transport);
			}
		}
		free(psess->plck);
		psess->plck = NULL;
	}

    return ERRNO_SUCCESS;
}

int mpls_if_update_tpoam_csf(struct mpls_if *pif)
{
	OAM_DEBUG();

	struct oam_session *psess = NULL;

	if(NULL == pif)
	{
		return ERRNO_FAIL;
	}

	if(NULL != pif->ppw_slave)
	{
		if(0 != pif->ppw_slave->mplsoam_id)
		{
			psess = mplsoam_session_lookup(pif->ppw_slave->mplsoam_id);
			if(NULL == psess)
			{
				return ERRNO_SUCCESS;
			}

			if(0 == psess->csf_enable)
			{
				return ERRNO_SUCCESS;
			}

			if(LINK_UP == pif->ppw_slave->ac_flag)
			{
				COM_THREAD_TIMER_OFF(psess->csf_send);
			}
			else
			{
				if(0 == psess->csf_enable)
				{
					return ERRNO_SUCCESS;
				}
			
				if(NULL == psess->pcsf)
				{
					return ERRNO_FAIL;
				}
				mplsoam_session_csf_enable(psess, psess->pcsf->level, psess->pcsf->csf_interval);
			}
		}
	}

	if(NULL != pif->ppw_master)
	{
		if(0 != pif->ppw_master->mplsoam_id)
		{
			psess = mplsoam_session_lookup(pif->ppw_master->mplsoam_id);
			if(NULL == psess)
			{
				return ERRNO_SUCCESS;
			}

			if(0 == psess->csf_enable)
			{
				return ERRNO_SUCCESS;
			}

			if(LINK_UP == pif->ppw_master->ac_flag)
			{
				COM_THREAD_TIMER_OFF(psess->csf_send);
			}
			else
			{
				if(0 == psess->csf_enable)
				{
					return ERRNO_SUCCESS;
				}
			
				if(NULL == psess->pcsf)
				{
					return ERRNO_FAIL;
				}
				mplsoam_session_csf_enable(psess, psess->pcsf->level, psess->pcsf->csf_interval);
			}
		}
	}

	return ERRNO_SUCCESS;
}


int mplsoam_session_csf_enable(struct oam_session *psess, uint8_t level, uint16_t interval)
{
	OAM_DEBUG();

	if((interval != 1 && interval != 60) || NULL == psess)
	{
		OAM_ERROR("interval is wrong\n");
		return ERRNO_PARAM_ILLEGAL;
	}

	if(psess->csf_enable != 1)
	{
		OAM_ERROR("csf enable fail \n");
		return ERRNO_FAIL;
	}

	if(psess->pcsf == NULL)
	{
		psess->pcsf = (struct oam_csf *)malloc(sizeof(struct oam_csf));
		if(psess->pcsf == NULL)
		{
			OAM_ERROR("%s[%s]:malloc err.--line:%d", __FILE__, __FUNCTION__, __LINE__);
			return ERRNO_FAIL;
		}
		memset(psess->pcsf, 0, sizeof(struct oam_csf));
	}
	
	psess->pcsf->level = level;
	psess->pcsf->csf_interval = interval;

	mplsoam_csf_pdu_construct(psess);
	mplsoam_csf_start(psess);

	return ERRNO_SUCCESS;
}

int mplsoam_session_csf_disable(struct oam_session *psess)
{
	OAM_DEBUG();

	if((NULL == psess) || (0 == psess->csf_enable))
    {
        return ERRNO_SUCCESS;
    }

    COM_THREAD_TIMER_OFF(psess->csf_send);

	if(NULL != psess->pcsf)
	{
		free(psess->pcsf);
		psess->pcsf = NULL;
	}

    return ERRNO_SUCCESS;
}

int mplsoam_session_lm_enable(struct oam_session *psess)
{
    int ret = 0;

	OAM_DEBUG();
    if((NULL == psess) || (psess->info.lm_enable == 1))
    {  
        return ERRNO_SUCCESS;
    }

    psess->info.lm_enable = OAM_STATUS_ENABLE;
    
    /* send lm-enable to hal */
    ret = ipc_send_msg_n2(&(psess->info), sizeof(struct oam_info), 1, MODULE_ID_HAL, MODULE_ID_MPLS,
    					IPC_TYPE_MPLSOAM, OAM_SUBTYPE_LM, IPC_OPCODE_ENABLE, psess->info.session_id);
    if(ret != ERRNO_SUCCESS)
    {
        psess->info.lm_enable = OAM_STATUS_DISABLE;
        psess->info.lm_interval = 0;
        psess->info.lm_frequency = 0;
		
        OAM_ERROR("Send to hal fail.\n");
        return ERRNO_IPC;
    }

    return ERRNO_SUCCESS;
}

int mplsoam_session_lm_disable(uint16_t session_id)
{
    struct oam_session *psess = NULL;
    int ret = 0;

	OAM_DEBUG();

    psess = mplsoam_session_lookup(session_id);
    if((NULL == psess) || (0 == psess->info.lm_enable))
    {
        return  ERRNO_SUCCESS;
    }
    
    /* send lm-disable to hal */
    ret = ipc_send_msg_n2(NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_MPLS,
    					IPC_TYPE_MPLSOAM, OAM_SUBTYPE_LM, IPC_OPCODE_DISABLE, psess->info.session_id);
    if(ret != ERRNO_SUCCESS)
    {
        OAM_ERROR("Send to hal fail.\n");
        return ERRNO_IPC;
    }
    psess->info.lm_enable = OAM_STATUS_DISABLE;
    psess->info.lm_interval = 0;
    psess->info.lm_frequency = 0;

    return ERRNO_FAIL;
}

int mplsoam_session_dm_enable(struct oam_session *psess)
{
    int ret = 0;

	OAM_DEBUG();

    if((NULL == psess) || (psess->info.dm_enable == 1))
    {   
        return ERRNO_SUCCESS;
    }

    psess->info.dm_enable = OAM_STATUS_ENABLE;
    
    /* send dm-enable to hal */
    ret = ipc_send_msg_n2(&(psess->info), sizeof(struct oam_info), 1, MODULE_ID_HAL, MODULE_ID_MPLS,
    					IPC_TYPE_MPLSOAM, OAM_SUBTYPE_DM, IPC_OPCODE_ENABLE, psess->info.session_id);
    if(ret != ERRNO_SUCCESS)
    {
        psess->info.dm_enable = OAM_STATUS_DISABLE;
        psess->info.dm_interval = 0;
        psess->info.dm_frequency = 0;
		psess->info.dm_exp      = 0;
		psess->info.dm_tlv_len      = 0;
		
        OAM_ERROR("Send to hal fail.\n");
        return ERRNO_IPC;
    }

    return ERRNO_FAIL;
}

int mplsoam_session_dm_disable(uint16_t session_id)
{   
    struct oam_session *psess = NULL;
    int ret = 0;

	OAM_DEBUG();
    
    psess = mplsoam_session_lookup(session_id);
    if((NULL == psess) || (0 == psess->info.dm_enable))
    {
        return ERRNO_SUCCESS;
    }

     /* send dm-disable to hal */
    ret = ipc_send_msg_n2(NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_MPLS,
                        IPC_TYPE_MPLSOAM, OAM_SUBTYPE_DM, IPC_OPCODE_DISABLE, psess->info.session_id);
    if(ret != ERRNO_SUCCESS)
    {
        OAM_ERROR("Send to hal fail.\n");
        return ERRNO_IPC;
    }
    psess->info.dm_enable = OAM_STATUS_DISABLE;
    psess->info.dm_interval = 0;
    psess->info.dm_frequency = 0;
	psess->info.dm_exp = 0;
	psess->info.dm_tlv_len = 0;

    return ERRNO_SUCCESS;
}

int mplsoam_session_set_dm(uint16_t session_id, struct sla_measure *psla)
{
    struct oam_session *psess = NULL;

	OAM_DEBUG();

    if(NULL == psla)
    {
        return 1;
    }
    psess = mplsoam_session_lookup(session_id);
    if(NULL == psess)
    {
        return 1;
    }

    if(psla->state == OAM_STATUS_FINISH)
    {
        psess->info.dm_enable = OAM_STATUS_FINISH;
    }
    else if(psla->state == OAM_STATUS_ENABLE)
    {
        psess->info.dm_enable = OAM_STATUS_ENABLE;
    }
    
    psess->sla.dm_max = psla->dm_max;
    psess->sla.dm_min = psla->dm_min;
    psess->sla.dm_mean = psla->dm_mean;
    psess->sla.jitter_max = psla->jitter_max;
    psess->sla.jitter_min = psla->jitter_min;
    psess->sla.jitter_mean = psla->jitter_mean;
    
    return 0;
}

int mplsoam_session_set_lm(uint16_t session_id, struct sla_measure *psla)
{
    struct oam_session *psess = NULL;

	OAM_DEBUG();

    if(NULL == psla)
    {
        return 1;
    }
    psess = mplsoam_session_lookup(session_id);
    if(NULL == psess)
    {
        return 1;
    }

    if(psla->state == OAM_STATUS_FINISH)
    {
        psess->info.lm_enable = OAM_STATUS_FINISH;
    }
    else if(psla->state == OAM_STATUS_ENABLE)
    {
        psess->info.lm_enable = OAM_STATUS_ENABLE;
    }
    
    /* set near-end lm */
    psess->sla.lm_max = psla->lm_max;
    psess->sla.lm_min = psla->lm_min;
    psess->sla.lm_mean = psla->lm_mean;

    /* set far-end lm */
    psess->sla.lm_max_peer = psla->lm_max_peer;
    psess->sla.lm_min_peer = psla->lm_min_peer;
    psess->sla.lm_mean_peer = psla->lm_mean_peer;
   
    return 0;
}
void mplsoam_session_alarm_rpt_enable(struct oam_session *psess)
{
	uint32_t alm_unl = 0;
	uint32_t alm_mmg = 0;
	uint32_t alm_unm = 0;
	uint32_t alm_unp = 0;
	uint32_t alm_loc = 0;
	uint32_t alm_rdi = 0;
	uint32_t alm_ais = 0;
	uint32_t alm_lck = 0;
	uint32_t alm_csf = 0;
	uint32_t index = 0;
	uint8_t type = 0;
	uint32_t ifindex = 0;
	struct gpnPortInfo gPortInfo;

	OAM_DEBUG();
    
    if(NULL == psess)
    {
        return ;
    }
	
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));

	if(psess->info.type == OAM_TYPE_INTERFACE)
	{
		type = IFM_VS_2_TYPE;
		ifindex = psess->info.index;
		index = psess->info.session_id;
		
		alm_unl = GPN_ALM_TYPE_VS_UNL;
		alm_mmg = GPN_ALM_TYPE_VS_MMG;
		alm_unm = GPN_ALM_TYPE_VS_UNM;
		alm_unp = GPN_ALM_TYPE_VS_UNP;
		alm_loc = GPN_ALM_TYPE_VS_LOC;
		alm_rdi = GPN_ALM_TYPE_VS_RDI;
		alm_lck = GPN_ALM_TYPE_VS_LOK;
	}
	else if(psess->info.type == OAM_TYPE_LSP)
	{
		type = IFM_LSP_2_TYPE;
		index = psess->info.index;
	
		alm_unl = GPN_ALM_TYPE_LSP_UNL;
		alm_mmg = GPN_ALM_TYPE_LSP_MMG;
		alm_unm = GPN_ALM_TYPE_LSP_UNM;
		alm_unp = GPN_ALM_TYPE_LSP_UNP;
		alm_loc = GPN_ALM_TYPE_LSP_LOC;
		alm_rdi = GPN_ALM_TYPE_LSP_RDI;
		alm_ais = GPN_ALM_TYPE_LSP_AIS;
		alm_lck = GPN_ALM_TYPE_LSP_LOK;
	}
	else if(psess->info.type == OAM_TYPE_PW)
	{
		type = IFM_PW_2_TYPE;
		index = psess->info.index;
		
		alm_unl = GPN_ALM_TYPE_PW_UNL;
		alm_mmg = GPN_ALM_TYPE_PW_MMG;
		alm_unm = GPN_ALM_TYPE_PW_UNM;
		alm_unp = GPN_ALM_TYPE_PW_UNP;
		alm_loc = GPN_ALM_TYPE_PW_LOC;
		alm_rdi = GPN_ALM_TYPE_PW_RDI;
		alm_ais = GPN_ALM_TYPE_PW_AIS;
		alm_lck = GPN_ALM_TYPE_PW_LOK;
		alm_csf = GPN_ALM_TYPE_PW_CSF_LOS;
	}

	gPortInfo.iAlarmPort = type;
	gPortInfo.iIfindex = ifindex;
	gPortInfo.iMsgPara1 = index;
		
    if(psess->alarm.alarm_level == 1)
    {
		ipran_alarm_report(&gPortInfo, alm_unl, GPN_SOCK_MSG_OPT_RISE);
    }
    if(psess->alarm.alarm_meg == 1)
    {
		ipran_alarm_report(&gPortInfo, alm_mmg, GPN_SOCK_MSG_OPT_RISE);
    }
    if(psess->alarm.alarm_mep == 1)
    {
		ipran_alarm_report(&gPortInfo, alm_unm, GPN_SOCK_MSG_OPT_RISE);
    }
    if(psess->alarm.alarm_cc == 1)
    {
		ipran_alarm_report(&gPortInfo, alm_unp, GPN_SOCK_MSG_OPT_RISE);
    }
    if(psess->alarm.alarm_loc == 1)
    {
		ipran_alarm_report(&gPortInfo, alm_loc, GPN_SOCK_MSG_OPT_RISE);
    }
    if(psess->alarm.alarm_rdi == 1)
    {
		ipran_alarm_report(&gPortInfo, alm_rdi, GPN_SOCK_MSG_OPT_RISE);
    }
    if(psess->alarm.alarm_ais == 1)
    {
		ipran_alarm_report(&gPortInfo, alm_ais, GPN_SOCK_MSG_OPT_RISE);
    }
    if(psess->alarm.alarm_lck == 1)
    {
		ipran_alarm_report(&gPortInfo, alm_lck, GPN_SOCK_MSG_OPT_RISE);
    }
	if(psess->alarm.alarm_csf == 1)
	{
		ipran_alarm_report(&gPortInfo, alm_csf, GPN_SOCK_MSG_OPT_RISE);
	}
	
    return ;
}

void mplsoam_session_alarm_rpt_disable(struct oam_session *psess)
{
	uint32_t alm_unl = 0;
	uint32_t alm_mmg = 0;
	uint32_t alm_unm = 0;
	uint32_t alm_unp = 0;
	uint32_t alm_loc = 0;
	uint32_t alm_rdi = 0;
	uint32_t alm_ais = 0;
	uint32_t alm_lck = 0;
	uint32_t alm_csf = 0;
	uint32_t index = 0;
	uint8_t type = 0;
	uint32_t ifindex = 0;
	struct gpnPortInfo gPortInfo;

	OAM_DEBUG();
	
    if(NULL == psess)
    {
        return ;
    }

	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
    
	if(psess->info.type == OAM_TYPE_INTERFACE)
	{
		type = IFM_VS_2_TYPE;
		ifindex = psess->info.index;
		index = psess->info.session_id;
		
		alm_unl = GPN_ALM_TYPE_VS_UNL;
		alm_mmg = GPN_ALM_TYPE_VS_MMG;
		alm_unm = GPN_ALM_TYPE_VS_UNM;
		alm_unp = GPN_ALM_TYPE_VS_UNP;
		alm_loc = GPN_ALM_TYPE_VS_LOC;
		alm_rdi = GPN_ALM_TYPE_VS_RDI;
		alm_lck = GPN_ALM_TYPE_VS_LOK;
	}
	else if(psess->info.type == OAM_TYPE_LSP)
	{
		type = IFM_LSP_2_TYPE;
		index = psess->info.index;
	
		alm_unl = GPN_ALM_TYPE_LSP_UNL;
		alm_mmg = GPN_ALM_TYPE_LSP_MMG;
		alm_unm = GPN_ALM_TYPE_LSP_UNM;
		alm_unp = GPN_ALM_TYPE_LSP_UNP;
		alm_loc = GPN_ALM_TYPE_LSP_LOC;
		alm_rdi = GPN_ALM_TYPE_LSP_RDI;
		alm_ais = GPN_ALM_TYPE_LSP_AIS;
		alm_lck = GPN_ALM_TYPE_LSP_LOK;
	}
	else if(psess->info.type == OAM_TYPE_PW)
	{
		type = IFM_PW_2_TYPE;
		index = psess->info.index;
		
		alm_unl = GPN_ALM_TYPE_PW_UNL;
		alm_mmg = GPN_ALM_TYPE_PW_MMG;
		alm_unm = GPN_ALM_TYPE_PW_UNM;
		alm_unp = GPN_ALM_TYPE_PW_UNP;
		alm_loc = GPN_ALM_TYPE_PW_LOC;
		alm_rdi = GPN_ALM_TYPE_PW_RDI;
		alm_ais = GPN_ALM_TYPE_PW_AIS;
		alm_lck = GPN_ALM_TYPE_PW_LOK;
		alm_csf = GPN_ALM_TYPE_PW_CSF_LOS;
	}

	gPortInfo.iAlarmPort = type;
	gPortInfo.iIfindex = ifindex;
	gPortInfo.iMsgPara1 = index;
		
    if(psess->alarm.alarm_level == 1)
    {
		ipran_alarm_report(&gPortInfo, alm_unl, GPN_SOCK_MSG_OPT_CLEAN);
    }
    if(psess->alarm.alarm_meg == 1)
    {
		ipran_alarm_report(&gPortInfo, alm_mmg, GPN_SOCK_MSG_OPT_CLEAN);
    }
    if(psess->alarm.alarm_mep == 1)
    {
		ipran_alarm_report(&gPortInfo, alm_unm, GPN_SOCK_MSG_OPT_CLEAN);
    }
    if(psess->alarm.alarm_cc == 1)
    {
		ipran_alarm_report(&gPortInfo, alm_unp, GPN_SOCK_MSG_OPT_CLEAN);
    }
    if(psess->alarm.alarm_loc == 1)
    {
		ipran_alarm_report(&gPortInfo, alm_loc, GPN_SOCK_MSG_OPT_CLEAN);
    }
    if(psess->alarm.alarm_rdi == 1)
    {
		ipran_alarm_report(&gPortInfo, alm_rdi, GPN_SOCK_MSG_OPT_CLEAN);
    }
    if(psess->alarm.alarm_ais == 1)
    {
		ipran_alarm_report(&gPortInfo, alm_ais, GPN_SOCK_MSG_OPT_CLEAN);
    }
    if(psess->alarm.alarm_lck == 1)
    {
		ipran_alarm_report(&gPortInfo, alm_lck, GPN_SOCK_MSG_OPT_CLEAN);
    }
	if(psess->alarm.alarm_csf == 1)
	{
		ipran_alarm_report(&gPortInfo, alm_csf, GPN_SOCK_MSG_OPT_CLEAN);
	}
	
    return ;
}


#if 0
void mplsoam_session_alarm_rpt_disable(struct oam_session *psess)
{
    uint16_t session_id = 0;
    uint16_t remote_mep = 0;
    
    if(NULL == psess)
    {
        return ;
    }

    session_id = psess->info.session_id;
    remote_mep = psess->info.remote_mep;
    if(psess->alarm.alarm_level == 1)
    {
        alarm_report(IPC_OPCODE_CLEAR, MODULE_ID_MPLS, ALM_CODE_OAM_MPLSTP_UNL, session_id, remote_mep, 0, 0);
    }
    if(psess->alarm.alarm_meg == 1)
    {
        alarm_report(IPC_OPCODE_CLEAR, MODULE_ID_MPLS, ALM_CODE_OAM_MPLSTP_MMG, session_id, remote_mep, 0, 0);
    }
    if(psess->alarm.alarm_mep == 1)
    {
        alarm_report(IPC_OPCODE_CLEAR, MODULE_ID_MPLS, ALM_CODE_OAM_MPLSTP_UNM, session_id, remote_mep, 0, 0);
    }
    if(psess->alarm.alarm_cc == 1)
    {
        alarm_report(IPC_OPCODE_CLEAR, MODULE_ID_MPLS, ALM_CODE_OAM_MPLSTP_UNP, session_id, remote_mep, 0, 0);
    }
    if(psess->alarm.alarm_loc == 1)
    {
        alarm_report(IPC_OPCODE_CLEAR, MODULE_ID_MPLS, ALM_CODE_OAM_MPLSTP_LOC, session_id, remote_mep, 0, 0);
    }
    if(psess->alarm.alarm_rdi == 1)
    {
        alarm_report(IPC_OPCODE_CLEAR, MODULE_ID_MPLS, ALM_CODE_OAM_MPLSTP_RDI, session_id, remote_mep, 0, 0);
    }
    if(psess->alarm.alarm_ais == 1)
    {
        alarm_report(IPC_OPCODE_CLEAR, MODULE_ID_MPLS, ALM_CODE_OAM_MPLSTP_AIS, session_id, remote_mep, 0, 0);
    }
    if(psess->alarm.alarm_lck == 1)
    {
        alarm_report(IPC_OPCODE_CLEAR, MODULE_ID_MPLS, ALM_CODE_OAM_MPLSTP_LOK, session_id, remote_mep, 0, 0);
    }
	
    return ;
}
#endif

/* process alarm */
void mplsoam_session_alarm_process(uint16_t session_id, uint16_t state, enum OAM_ALARM alarm_type)
{
    struct oam_session *psess = NULL;
    unsigned int type = 0;
	uint32_t alm_unl = 0;
	uint32_t alm_mmg = 0;
	uint32_t alm_unm = 0;
	uint32_t alm_unp = 0;
	uint32_t alm_loc = 0;
	uint32_t alm_rdi = 0;
	uint32_t alm_ais = 0;
	uint32_t alm_lck = 0;
	uint32_t alm_csf = 0;
	uint32_t index = 0;
	uint8_t oam_type = 0;
	uint8_t oam_state = 0;
	uint32_t ifindex = 0;
	struct gpnPortInfo gPortInfo;

	OAM_DEBUG();
	
    psess = mplsoam_session_lookup(session_id);
    if(NULL == psess)
    {
        return ;
    }
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));

	if(psess->info.type == OAM_TYPE_INTERFACE)
	{
		oam_type = IFM_VS_2_TYPE;
		ifindex = psess->info.index;
		index = psess->info.session_id;
		
		alm_unl = GPN_ALM_TYPE_VS_UNL;
		alm_mmg = GPN_ALM_TYPE_VS_MMG;
		alm_unm = GPN_ALM_TYPE_VS_UNM;
		alm_unp = GPN_ALM_TYPE_VS_UNP;
		alm_loc = GPN_ALM_TYPE_VS_LOC;
		alm_rdi = GPN_ALM_TYPE_VS_RDI;
		alm_lck = GPN_ALM_TYPE_VS_LOK;
	}
	else if(psess->info.type == OAM_TYPE_LSP)
	{
		oam_type = IFM_LSP_2_TYPE;
		index = psess->info.index;
	
		alm_unl = GPN_ALM_TYPE_LSP_UNL;
		alm_mmg = GPN_ALM_TYPE_LSP_MMG;
		alm_unm = GPN_ALM_TYPE_LSP_UNM;
		alm_unp = GPN_ALM_TYPE_LSP_UNP;
		alm_loc = GPN_ALM_TYPE_LSP_LOC;
		alm_rdi = GPN_ALM_TYPE_LSP_RDI;
		alm_ais = GPN_ALM_TYPE_LSP_AIS;
		alm_lck = GPN_ALM_TYPE_LSP_LOK;
	}
	else if(psess->info.type == OAM_TYPE_PW)
	{
		oam_type = IFM_PW_2_TYPE;
		index = psess->info.index;
		
		alm_unl = GPN_ALM_TYPE_PW_UNL;
		alm_mmg = GPN_ALM_TYPE_PW_MMG;
		alm_unm = GPN_ALM_TYPE_PW_UNM;
		alm_unp = GPN_ALM_TYPE_PW_UNP;
		alm_loc = GPN_ALM_TYPE_PW_LOC;
		alm_rdi = GPN_ALM_TYPE_PW_RDI;
		alm_ais = GPN_ALM_TYPE_PW_AIS;
		alm_lck = GPN_ALM_TYPE_PW_LOK;
		alm_csf = GPN_ALM_TYPE_PW_CSF_LOS;
	}
	
    if(state == IPC_OPCODE_ADD)
    {
    	oam_state = GPN_SOCK_MSG_OPT_RISE;
        switch(alarm_type)
        {
            case OAM_ALARM_LEVEL:
                if(psess->alarm.alarm_level == 1)
                {
                    return ;
                }
                psess->alarm.alarm_level = 1;
                type = alm_unl;
                break;
            case OAM_ALARM_MEG:
                if(psess->alarm.alarm_meg == 1)
                {
                    return ;
                }
                psess->alarm.alarm_meg = 1;
                type = alm_mmg;
                break;
            case OAM_ALARM_MEP:
                if(psess->alarm.alarm_mep == 1)
                {
                    return ;
                }
                psess->alarm.alarm_mep = 1;
                type = alm_unm;
                break;
            case OAM_ALARM_INTERVAL:
                if(psess->alarm.alarm_cc == 1)
                {
                   return ;
                }
                psess->alarm.alarm_cc = 1;
                type = alm_unp;
                break;
            case OAM_ALARM_LOC:
                if(psess->alarm.alarm_loc == 1)
                {
                    return ;
                }
				
				#if 0
				if(psess->info.state == OAM_STATUS_ENABLE)
				{
					mplsoam_session_down(psess->info.session_id);
				}
				#endif
				
                psess->alarm.alarm_loc = 1;
                type = alm_loc;
                break;
            case OAM_ALARM_RDI:
                if(psess->alarm.alarm_rdi == 1)
                {
                    return ;
                }
                psess->alarm.alarm_rdi = 1;
                type = alm_rdi;
                break;
			case OAM_ALARM_AIS:
				if(psess->alarm.alarm_ais == 1)
                {
                    return ;
                }
                psess->alarm.alarm_ais = 1;
                type = alm_ais;
                break;
			case OAM_ALARM_LCK:
				if(psess->alarm.alarm_lck == 1)
                {
                    return ;
                }
                psess->alarm.alarm_lck = 1;
                type = alm_lck;
                break;
			case OAM_ALARM_CSF:
				if(psess->alarm.alarm_csf == 1)
				{
					return ;
				}
				psess->alarm.alarm_csf = 1;
				type = alm_csf;
				break;
            default:
                break;
            
        }
		if((psess->info.state == OAM_STATUS_ENABLE)
			&& (alarm_type == OAM_ALARM_LEVEL || alarm_type == OAM_ALARM_MEG
				|| alarm_type == OAM_ALARM_MEP || alarm_type == OAM_ALARM_INTERVAL
					|| alarm_type == OAM_ALARM_LOC))
		{
			if(psess->info.state == OAM_STATUS_ENABLE)
			{
				mplsoam_session_down(psess->info.session_id);
			}
		}
    }
    else if(state == IPC_OPCODE_CLEAR)
    {
    	oam_state = GPN_SOCK_MSG_OPT_CLEAN;
        switch(alarm_type)
        {
            case OAM_ALARM_LEVEL:
                if(psess->alarm.alarm_level == 0)
                {
                    return ;
                }
                psess->alarm.alarm_level = 0;
                type = alm_unl;
                break;
            case OAM_ALARM_MEG:
                if(psess->alarm.alarm_meg == 0)
                {
                    return ;
                }
                psess->alarm.alarm_meg = 0;
                type = alm_mmg;
                break;
            case OAM_ALARM_MEP:
                if(psess->alarm.alarm_mep == 0)
                {
                    return ;
                }
                psess->alarm.alarm_mep = 0;
                type = alm_unm;
                break;
            case OAM_ALARM_INTERVAL:
                if(psess->alarm.alarm_cc == 0)
                {
                   return ;
                }
                psess->alarm.alarm_cc = 0;
                type = alm_unp;
                break;
            case OAM_ALARM_LOC:
                if(psess->alarm.alarm_loc == 0)
                {
                    return ;
                }
                psess->alarm.alarm_loc = 0;
                type = alm_loc;
                break;
            case OAM_ALARM_RDI:
                if(psess->alarm.alarm_rdi == 0)
                {
                    return ;
                }
                psess->alarm.alarm_rdi = 0;
                type = alm_rdi;
                break;
			case OAM_ALARM_AIS:
				if(psess->alarm.alarm_ais == 0)
                {
                    return ;
                }
                psess->alarm.alarm_ais = 0;
                type = alm_ais;
                break;
			case OAM_ALARM_LCK:
				if(psess->alarm.alarm_lck == 0)
                {
                    return ;
                }
                psess->alarm.alarm_lck = 0;
                type = alm_lck;
                break;
			case OAM_ALARM_CSF:
				if(psess->alarm.alarm_csf == 0)
				{
					return ;
				}
				psess->alarm.alarm_csf = 0;
				type = alm_csf;
            default:
                break;
        }
    }

	gPortInfo.iAlarmPort = oam_type;	
	gPortInfo.iIfindex = ifindex;		
	gPortInfo.iMsgPara1 = index;		

    /* 判断是否向alarm模块上报告警*/
    if(psess->alarm_rpt == 1)
    {
		ipran_alarm_report(&gPortInfo, type, oam_state);
    }
    
    return ;
}

#if 0
/*session down ,清除比loc高的级别的告警*/
void mplsoam_session_down_clear_alarm(struct oam_session *psess)
{
    uint16_t session_id = 0;
    uint16_t remote_mep = 0;
    
    if(NULL == psess)
    {
        return ;
    }

    session_id = psess->info.session_id;
    remote_mep = psess->info.remote_mep;

    if(psess->alarm_rpt == 1)
    {   
        if(psess->alarm.alarm_level == 1)
        {
            alarm_report(IPC_OPCODE_CLEAR, MODULE_ID_MPLS, ALM_CODE_OAM_MPLSTP_UNL, session_id, remote_mep, 0, 0);
        }
        if(psess->alarm.alarm_meg == 1)
        {
            alarm_report(IPC_OPCODE_CLEAR, MODULE_ID_MPLS, ALM_CODE_OAM_MPLSTP_MMG, session_id, remote_mep, 0, 0);
        }
        if(psess->alarm.alarm_mep == 1)
        {
            alarm_report(IPC_OPCODE_CLEAR, MODULE_ID_MPLS, ALM_CODE_OAM_MPLSTP_UNM, session_id, remote_mep, 0, 0);
        }
        if(psess->alarm.alarm_cc == 1)
        {
            alarm_report(IPC_OPCODE_CLEAR, MODULE_ID_MPLS, ALM_CODE_OAM_MPLSTP_UNP, session_id, remote_mep, 0, 0);
        }
    }

    psess->alarm.alarm_level = 0;
    psess->alarm.alarm_meg = 0;
    psess->alarm.alarm_mep = 0;
    psess->alarm.alarm_cc = 0;
    
    return ;
}
#endif

/* 检查配置是否完整 */
int mplsoam_session_config_complete(struct oam_session *psess)
{
	OAM_DEBUG();

    if(NULL == psess)
    {
        return ERRNO_PARAM_ILLEGAL;
    }
    
    if(psess->info.cc_interval != 0 && psess->info.session_id != 0 && psess->info.remote_mep != 0
		&& psess->meg != NULL && psess->info.mp_id != 0 && psess->info.mp_type == MP_TYPE_MEP)
    {
        return ERRNO_SUCCESS;
    }
    return ERRNO_CONFIG_INCOMPLETE;
}

int mpls_pw_bind_oam_mip(uint16_t session_id, struct l2vc_entry *pl2vc)
{
	struct oam_session *psess = NULL;

	OAM_DEBUG();

	if(NULL == pl2vc)
	{
		return ERRNO_PARAM_ILLEGAL;
	}
	
	psess = mplsoam_session_lookup(session_id);
	if(NULL == psess)
	{
		OAM_ERROR("can't find session:%d.\n", session_id);
		return ERRNO_NOT_FOUND;
	}
	
	if(psess->info.mp_type != MP_TYPE_MIP)
	{
		OAM_ERROR("session:%d is not mip.\n", session_id);
		return ERRNO_UNMATCH;
	}
	
	if(psess->info.type != OAM_TYPE_INVALID)
	{
		return ERRNO_ALREADY_ENABLE_OAM;
	}
	
	if((0 == psess->fwd_rmep) || (0 == psess->rev_rmep) || (0 == psess->info.mp_id))
    {
		OAM_ERROR("Forward remote mep and reverse remote mep is not config.\n");
        return ERRNO_CONFIG_INCOMPLETE;
    }
	
	if(psess->meg == NULL)
	{
		OAM_ERROR("don't service meg.\n"); 
		return ERRNO_CONFIG_INCOMPLETE;
	}

    if(pl2vc->pswitch_pw == NULL)
    {
		OAM_ERROR("Pw:%s is not switch-pw.\n", pl2vc->name);
        return ERRNO_FAIL;
    }

	if((pl2vc->mplsoam_id == psess->info.mp_id 
		&& pl2vc->pswitch_pw->mplsoam_id == psess->info.mp_id)
			&& (psess->sw_pw == pl2vc || psess->sw_pw == pl2vc->pswitch_pw))
	{
		return ERRNO_SUCCESS;
	}
	
    if(pl2vc->mplsoam_id != 0 || pl2vc->pswitch_pw->mplsoam_id != 0)
    {
		OAM_ERROR("Pw:%s is already bind with other mip.\n", pl2vc->name);
        return ERRNO_ALREADY_BIND_TPOAM;
    }

    psess->sw_pw = pl2vc;
	psess->info.type = OAM_TYPE_PW;
    pl2vc->mplsoam_id = psess->info.session_id;
	pl2vc->pswitch_pw->mplsoam_id = psess->info.session_id;

	return ERRNO_SUCCESS;
}

int mpls_pw_unbind_oam_mip(uint16_t session_id, struct l2vc_entry *pl2vc)
{
	int ret = 0;

	OAM_DEBUG();
	
	if(NULL == pl2vc)
	{
		return ERRNO_PARAM_ILLEGAL;
	}
	if(pl2vc->pswitch_pw == NULL)
    {
		OAM_ERROR("Pw:%s is not switch pw.\n", pl2vc->name);
        return ERRNO_FAIL;
    }
	
	ret = mplsoam_session_unbind_service(session_id, pl2vc);

	return ret;
}


int mplsoam_session_unbind_service(uint16_t session_id, void *service)
{
	struct oam_session *psess = NULL;
	struct static_lsp *plsp = NULL;
	struct l2vc_entry *pl2vc = NULL;

	OAM_DEBUG();
	
	if(0 == session_id || NULL == service)
	{
		OAM_DEBUG("para is WRONG\n");
		return ERRNO_FAIL;
	}

	psess = mplsoam_session_lookup(session_id);
	if(NULL == psess)
	{
		return ERRNO_NOT_FOUND;
	}
	
	if(psess->info.type == OAM_TYPE_LSP)
	{
		plsp = (struct static_lsp *)service;
		if(plsp == psess->fwd_lsp)
		{
			psess->fwd_lsp = NULL;
			plsp->mplsoam_id = 0;
		}
		else if(plsp == psess->rev_lsp)
		{
			psess->rev_lsp = NULL;
			plsp->mplsoam_id = 0;
		}
		else
		{
			return ERRNO_UNMATCH;
		}
	}
	else if(psess->info.type == OAM_TYPE_PW)
	{
		pl2vc = (struct l2vc_entry *)service;
		if(pl2vc == psess->sw_pw || pl2vc->pswitch_pw == psess->sw_pw)
		{
			pl2vc->mplsoam_id = 0;
			pl2vc->pswitch_pw->mplsoam_id = 0;
			psess->sw_pw = NULL;
		}
		else
		{
			return ERRNO_UNMATCH;
		}
	}

	if(NULL == psess->fwd_lsp && NULL == psess->rev_lsp && NULL == psess->sw_pw)
	{
    	psess->info.type = OAM_TYPE_INVALID;
	}
	
	return ERRNO_SUCCESS;
}


int mplsoam_session_ifname(uint16_t session_id, char *ifname)
{
    struct oam_session *psession;
    struct lsp_entry   *plsp = NULL;

	OAM_DEBUG();

    if(NULL == ifname)
    {
		OAM_ERROR("pointer is NULL\n");
        return 0;
    }

    psession = mplsoam_session_lookup(session_id);
    if(NULL == psession)
    {
		OAM_ERROR("pointer is NULL\n");
        return 0;
    }

    if (OAM_TYPE_INTERFACE == psession->info.type)
    {
        ifm_get_name_by_ifindex(psession->info.index, ifname);
    }
    else if (OAM_TYPE_PW == psession->info.type)
    {
        snprintf(ifname, NAME_STRING_LEN, "%s", (uchar *)(pw_get_name(psession->info.index)));
    }
    else if (OAM_TYPE_LSP == psession->info.type)
    {
        plsp = mpls_lsp_lookup(psession->info.index);
        if (NULL != plsp)
        {
            snprintf(ifname, NAME_STRING_LEN, "%s", (char *)plsp->name);
        }
    }
    else if (OAM_TYPE_TUNNEL == psession->info.type)
    {
        ifm_get_name_by_ifindex(psession->info.index, ifname);
    }
    else
    {
		OAM_ERROR("type is NULL\n");
        return 0;
    }

    return 1;
}


/****************************oam meg****************************/


struct oam_meg *mplsoam_meg_create(uchar *name)
{
	struct oam_meg *meg = NULL;

	OAM_DEBUG();

	if(NULL == name)
	{
        OAM_ERROR("pointer is NULL\n");
		return NULL;
	}
	
    meg = (struct oam_meg *)XMALLOC(MTYPE_MPLSOAM_SESS, sizeof(struct oam_meg));
    if(NULL == meg)
    {
        OAM_ERROR("malloc fial!\n");
		return NULL;
    }
    memset(meg, 0, sizeof(struct oam_meg));

    memcpy(meg->name, name, strlen((char *)name));
    meg->level = 7;
	meg->session_list = list_new();
	
    return meg;
}


/* 添加meg节点*/
int mplsoam_meg_add(struct oam_meg *meg) 
{
	struct hash_bucket *pbucket = NULL;

	OAM_DEBUG();
	
	if(NULL == meg)
	{
        OAM_ERROR("pointer is NULL\n");
		return ERRNO_PARAM_ILLEGAL;
	}

	if(mplsoam_meg_table.hash_size == mplsoam_meg_table.num_entries)
	{
		OAM_ERROR("oversize\n");
		return ERRNO_OVERSIZE;
	}
	
	pbucket = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if(NULL == pbucket)
	{
		OAM_ERROR("malloc\n");
		return ERRNO_MALLOC;
	}
	memset(pbucket, 0, sizeof(struct hash_bucket));
	
	pbucket->hash_key = meg->name;
	pbucket->data = meg;
	
	hios_hash_add(&mplsoam_meg_table, pbucket);

	return ERRNO_SUCCESS;
}


/* 删除一个meg节点*/
int mplsoam_meg_delete(uchar *name)     
{
	struct hash_bucket *pbucket = NULL;

	pbucket = hios_hash_find(&mplsoam_meg_table, name);
	if(NULL == pbucket)
	{
		return  ERRNO_NOT_FOUND;
	}

	hios_hash_delete(&mplsoam_meg_table, pbucket);
	XFREE(MTYPE_MPLSOAM_SESS, pbucket->data);
	XFREE(MTYPE_HASH_BACKET, pbucket);

	return ERRNO_SUCCESS;
}

/* 查找一个meg*/
struct oam_meg *mplsoam_meg_lookup(uchar *name) 
{
	struct hash_bucket *pbucket = NULL;

    if(NULL == name)
    {
        return NULL;
    }
	pbucket = hios_hash_find(&mplsoam_meg_table, name);
	if(NULL == pbucket)
	{
		return NULL;
	}

	return (struct oam_meg *)pbucket->data;
}


