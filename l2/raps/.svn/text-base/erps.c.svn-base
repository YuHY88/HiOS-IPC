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
#include <lib/oam_common.h>
#include <lib/log.h>


#include "raps/erps.h"
#include "raps/erps_cmd.h"
#include "raps/erps_pkt.h"
#include "l2_msg.h"



struct erps_global gerps;              /* erps 全局数据结构 */
struct hash_table erps_session_table; /* erps 会话 hash 表，用 sess_id 作为 key */

void erps_init(void)
{
    erps_sess_table_init(ERPS_NUM);
    erps_cmd_init();
}

void erps_vlan_map_set(unsigned char *bitmap, uint16_t vlan_start, uint16_t vlan_end)
{
    uint16_t vlan = 0;

    ERPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d", __FILE__, __func__, __LINE__);

    for(vlan = vlan_start; vlan < vlan_end + 1; vlan++)
    {
        bitmap[vlan / 8] |= (1 << (vlan % 8));
    }

    return ;
}

void erps_vlan_map_unset(unsigned char *bitmap, uint16_t vlan_start, uint16_t vlan_end)
{
    uint16_t vlan = 0;

    ERPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d", __FILE__, __func__, __LINE__);

    for(vlan = vlan_start; vlan < vlan_end + 1; vlan++)
    {
        bitmap[vlan / 8] &= ~(1 << (vlan % 8));
    }

    return ;
}

int erps_vlan_map_isset(unsigned char *bitmap, uint16_t vlan_start, uint16_t vlan_end)
{
    uint16_t vlan = 0;

    ERPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d", __FILE__, __func__, __LINE__);

    for(vlan = vlan_start; vlan < vlan_end + 1; vlan++)
    {
        if((bitmap[vlan / 8] & (1 << (vlan % 8))))
        {
            return vlan;
        }
    }

    return 0;
}

static unsigned int erps_compute_sess(void *hash_key)
{
    if(NULL == hash_key)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    return ((unsigned int) hash_key);
}

static int erps_compare_sess(void *item, void *hash_key)
{
    if((NULL == item) || (NULL == hash_key))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    return ((struct hash_bucket *)item)->hash_key == hash_key ? 0 : 1;
}

void erps_sess_table_init(unsigned int size)

{
    if(0 == size)
    {
        ERPS_LOG_DBG("size must great than or equal to zero!\n");
        return ;
    }

    hios_hash_init(&erps_session_table, size, erps_compute_sess, erps_compare_sess);
    return ;

}


struct erps_sess *erps_sess_create(uint16_t sess_id)/* 创建一个 erps session */
{
    struct erps_sess *psess;

    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);

    psess = (struct erps_sess *)XMALLOC(MTYPE_ERPS_SESS, sizeof(struct erps_sess));

    if(NULL == psess)
    {
        return NULL;
    }

    memset(psess, 0, sizeof(struct erps_sess));

    psess->info.sess_id = sess_id;
    psess->info.priority = 6;
    psess->info.wtr = 5;         /* unit:ms */
    psess->info.guardtimer = 500;/* unit:ms */
    psess->info.wtb = 5500;      /* unit:ms */
    psess->info.keepalive = 5;   /* unit:s */
    psess->info.failback = FAILBACK_ENABLE;
    psess->info.current_status = ERPS_STATE_INIT;
    psess->info.level = 7;

    return psess;

}


int erps_sess_add(struct erps_sess *psess) /* session 添加到 hash 表 */
{
    struct hash_bucket *sess_item = NULL;
    int ret;

    if(NULL == psess)
    {
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);

    //new
    sess_item = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));

    if(NULL == sess_item)
    {
        return ERRNO_MALLOC;
    }

    memset(sess_item, 0, sizeof(struct hash_bucket));

    sess_item->data = psess;
    sess_item->hash_key = (void *)(uint32_t)psess->info.sess_id;

    ret = hios_hash_add(&erps_session_table, sess_item);

    if(ret)
    {
        XFREE(MTYPE_HASH_BACKET, sess_item);
        return ERRNO_OVERSIZE;
    }

    return ERRNO_SUCCESS;

}

int erps_sess_delete(uint16_t sess_id) /* 删除一个 erps session */
{
    struct hash_bucket *sess_item = NULL;
    struct erps_sess *sess_node = NULL;
    struct cfm_sess *unbind_cfm_sess = NULL;

    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);


    sess_item = hios_hash_find(&erps_session_table, (void *)(uint32_t)sess_id);


    if(sess_item)
    {
        sess_node = sess_item->data;

        if(sess_node)
        {
            if(sess_node->info.east_cfm_session_id)
            {
                unbind_cfm_sess = cfm_session_unbind_elps(sess_node->info.east_cfm_session_id);

                if(NULL == unbind_cfm_sess)
                {
                    return ERRNO_FAIL;
                }
            }

            if(sess_node->info.west_cfm_session_id)
            {
                unbind_cfm_sess = cfm_session_unbind_elps(sess_node->info.west_cfm_session_id);

                if(NULL == unbind_cfm_sess)
                {
                    return ERRNO_FAIL;
                }
            }            
            XFREE(MTYPE_ERPS_SESS, sess_node);
        }
        
        hios_hash_delete(&erps_session_table, sess_item);
        
        XFREE(MTYPE_HASH_BACKET, sess_item);
    }

    return ERRNO_SUCCESS;
    //delet sesslist

}


struct erps_sess *erps_sess_lookup(uint16_t sess_id) /* 查找一个erps session */
{
    struct hash_bucket *sess_item = NULL;
    struct erps_sess *sess_node = NULL;

    if(sess_id < 1 || sess_id > ERPS_NUM)
    {
        return NULL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);

    sess_item = hios_hash_find(&erps_session_table, (void *)(uint32_t)sess_id);

    if(sess_item)
    {
        sess_node = sess_item->data;

        return sess_node;
    }

    return NULL;

}

int erps_sess_update(struct erps_sess *psess)
{
    int ret = ERRNO_FAIL;

    if(NULL == psess)
    {
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);

    if(psess)
    {
        /* send sess-enable to hal */
        ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                           IPC_TYPE_ERPS, OAM_SUBTYPE_SESSION, IPC_OPCODE_UPDATE, psess->info.sess_id);

        if(ret != ERRNO_SUCCESS)
        {
            psess->info.status = SESSION_STATUS_DISABLE;
            ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
            return ERRNO_IPC;
        }
    }

    return ret;
}
int erps_sess_enable(struct erps_sess *psess)

{
    int ret = ERRNO_FAIL;

    if((NULL == psess) || (SESSION_STATUS_ENABLE == psess->info.status))
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);

    psess->info.status = SESSION_STATUS_ENABLE;

    /* send sess-enable to hal */
    ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                       IPC_TYPE_ERPS, OAM_SUBTYPE_SESSION, IPC_OPCODE_ADD, psess->info.sess_id);

    if(ret != ERRNO_SUCCESS)
    {
        psess->info.status = SESSION_STATUS_DISABLE;
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_IPC;
    }

    return ERRNO_SUCCESS;
}

int erps_sess_disable(struct erps_sess *psess)

{
    int ret = 0;

    if(NULL == psess)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    if((NULL == psess) || (SESSION_STATUS_DISABLE == psess->info.status))
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_SUCCESS;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);

    if(psess->info.current_status != ERPS_STATE_IDLE)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
    }

    /* send sess-disable to hal */
    ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                       IPC_TYPE_ERPS, OAM_SUBTYPE_SESSION, IPC_OPCODE_DELETE, psess->info.sess_id);

    if(ret != ERRNO_SUCCESS)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_IPC;
    }

	psess->info.current_status = ERPS_STATE_INIT;
    psess->info.status = SESSION_STATUS_DISABLE;
    return ERRNO_SUCCESS;

}

int erps_get_bulk(struct erps_info *erps_buf, uint32_t session_id, uint32_t erps_max)
{
    struct hash_bucket *pbucket = NULL;
    struct erps_sess *psess = NULL;
    uint32_t erps_cnt = 0;
	int cursor = 0;
	int flag = 0;
	ERPS_LOG_DBG( "%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);

	if(0 == session_id)
	{
		HASH_BUCKET_LOOP(pbucket, cursor, erps_session_table)
		{
			psess = (struct erps_sess *)pbucket->data;
	        if (NULL == psess)
	        {
				continue;
	        }

        	memcpy((erps_buf + erps_cnt), &(psess->info), sizeof(struct erps_info));
		
			erps_cnt++;
        	if (erps_cnt == erps_max)
        	{
            	return erps_cnt;
        	}
	    }
		ERPS_LOG_DBG( "%s:Entering the function of '%s'--the line of %d, erps_cnt %d",__FILE__,__func__,__LINE__, erps_cnt);

		return erps_cnt;
	}
	else
	{
		HASH_BUCKET_LOOP(pbucket, cursor, erps_session_table)
		{
			psess = (struct erps_sess *)pbucket->data;
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
				memcpy((erps_buf + erps_cnt), &(psess->info), sizeof(struct erps_info));
				
				erps_cnt++;
	        	if (erps_cnt == erps_max)
	        	{
					flag = 0;
	            	return erps_cnt;
	        	}
			}
		}
		flag = 0;
		return erps_cnt;
	}
		
}

int erps_msg_rcv_get_bulk(struct ipc_msghdr_n  *pmsghdr, void *pdata)
{
	uint32_t erps_num = IPC_MSG_LEN/sizeof(struct erps_info);
    struct erps_info erps_buf[erps_num];
	uint32_t erps_cnt = 0;
	int ret = 0;
	
	ERPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
	memset(erps_buf, 0, erps_num * sizeof(struct erps_info));
    erps_cnt = erps_get_bulk(erps_buf, pmsghdr->msg_index, erps_num);
	if (erps_cnt > 0)
	{
		ret = ipc_send_reply_n2(erps_buf, erps_cnt * sizeof(struct erps_info), erps_cnt, pmsghdr->sender_id,
                             MODULE_ID_L2, IPC_TYPE_ERPS, pmsghdr->msg_subtype, 0, pmsghdr->msg_index, IPC_OPCODE_REPLY);
	}
	else
	{
	    //  ret = l2_msg_send_noack(ERRNO_NOT_FOUND, pmsghdr, pmsghdr->msg_index);
		ret = ipc_send_reply_n2(NULL,0,0, pmsghdr->sender_id, MODULE_ID_L2, IPC_TYPE_ERPS, pmsghdr->msg_subtype, 0, pmsghdr->msg_index, IPC_OPCODE_NACK);
	}

	return ret;
}

int erps_msg_rcv_get_global(struct ipc_msghdr_n  *pmsghdr, void *pdata)
{
	int ret = 0;
	
	ERPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
	ret = ipc_send_reply_n2(&gerps, sizeof(struct erps_global), 1, pmsghdr->sender_id, MODULE_ID_L2,
                        IPC_TYPE_ERPS, pmsghdr->msg_subtype, 0, pmsghdr->msg_index,IPC_OPCODE_REPLY);

	return ret;
}


