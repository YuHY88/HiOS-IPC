#include <stdlib.h>
#include <string.h>
#include <lib/thread.h>
#include <lib/memory.h>
#include <lib/hash1.h>
#include <lib/index.h>
#include <lib/zassert.h>
#include <lib/log.h>
#include <lib/types.h>
#include <lib/module_id.h>
#include <lib/msg_ipc_n.h>
#include <lib/errcode.h>
#include <lib/mpls_common.h>
#include <lib/ifm_common.h>
#include "lspm.h"
#include "bfd.h"
#include "bfd_session.h"
#include "bfd_packet.h"
#include "pw.h"
#include "tunnel.h"
#include "lsp_static.h"
#include "ldp/ldp_session.h"
#include "ldp/ldp_fsm.h"
#include <lib/alarm.h>
#include "l2vpn_h3c.h"

struct hash_table bfd_session_table;		/* bfd 会话表 */
struct hash_table bfd_template_table;		/* bfd 模板*/
struct list *local_id_list;

static uint32_t bfd_session_compute_hash(void *hash_key)
{
	if(NULL == hash_key)
	{
		return 0;
	}

	return (uint32_t)hash_key;
}

static int bfd_session_compare(void *item, void *hash_key)
{
    struct hash_bucket *pbucket = item;

    if ( NULL == item || NULL == hash_key )
    {
        return ERRNO_FAIL;
    }

    if ( pbucket->hash_key == hash_key )
    {
        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}

void bfd_session_table_init ( unsigned int size )
{
	BFD_DEBUG("--->income\n");

	hios_hash_init(&bfd_session_table, size, bfd_session_compute_hash, bfd_session_compare);
	/*会话标识索引初始化*/
	index_register(INDEX_TYPE_OAM, BFD_SESS_DISC_MAX);

	local_id_list = list_new(); //用于存放bfd session local_id
}

struct bfd_sess *bfd_session_create(uint16_t session_id)
{
	struct bfd_sess *sess_new = NULL;
	struct hash_bucket *pitem = NULL;
	struct gpnPortInfo gPortInfo;
	uint32_t sess_id = 0;

	BFD_DEBUG("--->session_id = %d\n", session_id);

	if (bfd_session_table.hash_size == bfd_session_table.num_entries)
	{
		zlog_err("%s[%d]: In function '%s' The entry number exceed limit %d.\n",
							__FILE__, __LINE__, __func__, BFD_NUM_MAX);
		return NULL;
	}

	sess_new = (struct bfd_sess *)XCALLOC(MTYPE_BFD_SESS_ENTRY, sizeof(struct bfd_sess));
	if (NULL == sess_new)
	{
		zlog_err("%s[%d]: In function '%s' Memory alloc failed !\n",
						__FILE__, __LINE__, __func__);
		return NULL;
	}

	pitem = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if (NULL == pitem)
	{
		XFREE(MTYPE_BFD_SESS_ENTRY, sess_new);
		zlog_err("%s[%d]: In function '%s' Memory alloc failed !\n",
							__FILE__, __LINE__, __func__);
		return NULL;
	}
	memset(sess_new, 0, sizeof(struct bfd_sess));
	sess_new->type = BFD_TYPE_INVALID;
	sess_new->status = BFD_STATUS_ADMINDOWN;
	sess_new->bfd_event = BFD_EVENT_MAX;
	sess_new->poll_event = BFD_EVENT_MAX;
	sess_new->wtr = BFD_WTR_INTERVAL;
	sess_new->init_interval = BFD_NEGO_INTERVAL;
	sess_new->cc_interval_recv = gbfd.cc_interval;
	sess_new->cc_interval_send = gbfd.cc_interval;
	sess_new->tdm_pw_label = 0;

	/*loca_id为0表示需要动态分配本端标识符*/
	if(session_id == 0)
	{
		/*按照标识符的序号来分配*/
		sess_new->session_id= index_alloc(INDEX_TYPE_OAM);
	}
	else
	{
		/*静态指定本端标识符，对端标识符可以配置或者学习*/
		index_get(INDEX_TYPE_OAM, session_id);
		sess_new->session_id_cfg = session_id;
	}
	
	pitem->hash_key = (void *)(sess_new->session_id_cfg ? sess_new->session_id_cfg : sess_new->session_id);
	pitem->data = sess_new;
	hios_hash_add(&bfd_session_table, pitem);
	sess_id = sess_new->session_id_cfg ? sess_new->session_id_cfg : sess_new->session_id;

	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_BFD_TYPE;
	gPortInfo.iMsgPara1 = sess_id;
	ipran_alarm_port_register(&gPortInfo);
	
	return sess_new;
}

int bfd_local_id_create(struct bfd_sess *psess, struct list *list, uint32_t local_id)
{
	struct bfd_local_id *data_bfd = NULL;
	struct listnode *next_node = NULL;
	struct listnode *node = NULL;
	uint16_t sess_id = 0;
	uint32_t i;
	int flag = 0;

    struct hash_bucket *pbucket   = NULL;
	struct bfd_sess    *ppsess    = NULL;
    int                 cursor    = 0;

	BFD_DEBUG();

	if(NULL == psess)
	{
		zlog_err("%s[%d]: In function '%s' pointer NULL\n",
							__FILE__, __LINE__, __func__);
		return ERRNO_FAIL;
	}
	
	sess_id = psess->session_id_cfg ? psess->session_id_cfg : psess->session_id;
	BFD_DEBUG("--->local_id = %d\n", local_id);

	if(0 == local_id)
	{
		for(i = 1; i < BFD_SESS_LOCAL_ID_MAX; i++)
		{
			for( ALL_LIST_ELEMENTS ( list, node, next_node, data_bfd ) )
			{
				if(i == data_bfd->local_id)
				{
					flag = 1;
					break;
				}
			}

			if(flag)
			{
				flag = 0;
				continue;
			}
			else
				break;

		}

		if(BFD_SESS_LOCAL_ID_MAX == i)
		{
			return ERRNO_FAIL;
		}

		data_bfd = (struct bfd_local_id *) XCALLOC ( MTYPE_BFD_SESS_ENTRY, sizeof (struct bfd_local_id) );
		if(NULL == data_bfd)
		{
			BFD_DEBUG("XCALLOC fail !\n");
			return ERRNO_FAIL;
		}
		data_bfd->local_id = i;
		data_bfd->sess_id = sess_id;
		psess->local_id = i;
		i = 0;
		listnode_add (list, (void *)(data_bfd));
	}
	else
	{	
#if 0
		if(local_id < BFD_SESS_LOCAL_ID_MAX)
		{
			return ERRNO_FAIL;
		}
	    HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
	    {
			ppsess = (struct bfd_sess *)pbucket->data;
			if(NULL == ppsess)
			{
				continue;
			}

			if(local_id == ppsess->local_id_cfg)
			{
				return ERRNO_FAIL;
			}
		}
		psess->local_id_cfg = psess->local_id = local_id;
#else	
		for( ALL_LIST_ELEMENTS ( list, node, next_node, data_bfd ) )
		{
			if(local_id == data_bfd->local_id)
			{
				return ERRNO_FAIL;
			}
		}
		for( ALL_LIST_ELEMENTS ( list, node, next_node, data_bfd ) )
		{
			if(sess_id == data_bfd->sess_id)
			{
				data_bfd->local_id = local_id;
				psess->local_id_cfg = local_id;
				return ERRNO_SUCCESS;
			}
		}
		
		data_bfd = (struct bfd_local_id *) XCALLOC ( MTYPE_BFD_SESS_ENTRY, sizeof (struct bfd_local_id) );
		if(NULL == data_bfd)
		{
			BFD_DEBUG("%s[%d] XCALLOC fail !\n");
			return ERRNO_FAIL;
		}
		data_bfd->sess_id = sess_id;
		data_bfd->local_id = local_id;
		psess->local_id_cfg = local_id;
		listnode_add (list, (void *)(data_bfd));
#endif
	}

	return ERRNO_SUCCESS;
}

struct bfd_sess *bfd_local_id_lookup(struct list *list, uint32_t local_id)
{
	struct bfd_local_id *data_bfd = NULL;
	struct listnode *next_node = NULL;
	struct listnode *node = NULL;
	struct bfd_sess *psess = NULL;

	BFD_DEBUG("--->local_id = %d\n", local_id);
	for( ALL_LIST_ELEMENTS ( list, node, next_node, data_bfd ) )
	{
		if(local_id == data_bfd->local_id)
		{
			psess = bfd_session_lookup(data_bfd->sess_id);
			if(NULL == psess)
			{
				BFD_DEBUG("--->no sess_id:%d\n", data_bfd->sess_id);
				return NULL;
			}

			return psess;
		}
	}

	return NULL;
}

int bfd_local_id_delete(struct bfd_sess *psess, struct list *list)
{
	struct bfd_local_id *data_bfd = NULL;
	struct listnode *next_node = NULL;
	struct listnode *node = NULL;
	uint16_t sess_id = 0;

	BFD_DEBUG();

	if(NULL == psess)
	{
		zlog_err("%s[%d]: In function '%s' pointer NULL\n",
							__FILE__, __LINE__, __func__);
		return ERRNO_FAIL;
	}
	
	sess_id = psess->session_id_cfg ? psess->session_id_cfg : psess->session_id;
	BFD_DEBUG("--->sess_id = %d\n", sess_id);

	for( ALL_LIST_ELEMENTS ( list, node, next_node, data_bfd ) )
	{
		if(sess_id == data_bfd->sess_id)
		{
			XFREE(MTYPE_BFD_SESS_ENTRY, node->data);
			node->data = NULL;
			list_delete_node (list, node);
			node= NULL; 
			//listnode_delete(list, (void *)data_bfd);
			psess->local_id_cfg = psess->local_id = 0;
			break;
		}
	}

	return ERRNO_SUCCESS;
}

/*create与add关联，若add失败，已创建的psess应该删除*/
void bfd_session_disable(struct bfd_sess *psess)
{
	
    struct l2vc_entry *pl2vc = NULL;
	struct static_lsp *pslsp = NULL;
	struct lsp_entry  *plsp  = NULL;
	uint16_t sess_id = 0;

	BFD_DEBUG();

	if(NULL == psess)
	{
		zlog_err("%s[%d]: In function '%s' pointer NULL\n",
							__FILE__, __LINE__, __func__);
		return;
	}

	sess_id = psess->session_id_cfg ? psess->session_id_cfg : psess->session_id;
	BFD_DEBUG("--->sess_id = %d\n", sess_id);

	if(psess->type == BFD_TYPE_INTERFACE)
	{
		mpls_if_unbind_bfd_sess(psess->index);
	}
	else if(psess->type == BFD_TYPE_LSP)
	{
		/*解除lsp绑定*/
		plsp = mpls_lsp_lookup(psess->index);
        pslsp = static_lsp_lookup((uchar *)(plsp->name));
		if(pslsp != NULL)
		{
			pslsp->bfd_id = 0;
		}
	}
	else if(psess->type == BFD_TYPE_PW)
	{
		/*解除pw绑定*/
		pl2vc = l2vc_lookup(pw_get_name(psess->index));
		if(pl2vc != NULL)
		{
			pl2vc->bfd_id = 0;
		}
	}

	BFD_THREAD_TIMER_OFF(psess->cc_timer); 			
	BFD_THREAD_TIMER_OFF(psess->retry_timer); 			

	psess->without_ip = 0;
	psess->tdm_pw_label = 0;
	psess->index = 0;
	psess->remote_id = 0;
	psess->remote_recv_interval = 0;
	psess->remote_send_interval = 0;
	psess->remote_multiplier = 0;
	psess->srcip = 0;
	psess->poll = 0;
	psess->final = 0;
	psess->type = BFD_TYPE_INVALID;
	psess->status = BFD_STATUS_ADMINDOWN;
	psess->bfd_event = BFD_EVENT_MAX;
	psess->poll_event = BFD_EVENT_MAX;
	psess->wtr = BFD_WTR_INTERVAL;
	psess->init_interval = BFD_NEGO_INTERVAL;
	psess->cc_interval_send = BFD_CC_INTERVAL;
	psess->cc_interval_recv = BFD_CC_INTERVAL;
	psess->retry_num = 0;
	psess->up_count = 0;
	psess->down_count = 0;
	psess->recv_count = 0;
	psess->send_count = 0;
}

int bfd_session_delete(uint16_t sess_id)
{
	struct hash_bucket *pitem = NULL;
	struct bfd_sess    *sess  = NULL;
    struct l2vc_entry  *pl2vc = NULL;
	struct static_lsp  *pslsp = NULL;
	struct lsp_entry   *plsp  = NULL;
	struct gpnPortInfo gPortInfo;
	uint32_t local_id = 0;

	BFD_DEBUG("--->sess_id = %d\n", sess_id);

	pitem = hios_hash_find(&bfd_session_table, (void *)(uint32_t)sess_id);
	if (NULL == pitem)
	{
		zlog_info("%s[%d]: In function '%s' find sess_id %d not exist.\n",
							__FILE__, __LINE__, __func__, sess_id);
		return ERRNO_SUCCESS;
	}
	sess = (struct bfd_sess *)pitem->data;

	BFD_THREAD_TIMER_OFF(sess->cc_timer); 			
	BFD_THREAD_TIMER_OFF(sess->retry_timer);

	/*删除会话时解除绑定*/
	if(sess->type == BFD_TYPE_INTERFACE)
	{
		mpls_if_unbind_bfd_sess(sess->index);
	}
	else if(sess->type == BFD_TYPE_LSP)
	{
		/*解除lsp绑定*/
		plsp = mpls_lsp_lookup(sess->index);
        pslsp = static_lsp_lookup((uchar *)(plsp->name));
		if(pslsp != NULL)
		{
			pslsp->bfd_id = 0;
		}
	}
	else if(sess->type == BFD_TYPE_PW)
	{
		/*解除pw绑定*/
		pl2vc = l2vc_lookup(pw_get_name(sess->index));
		if(pl2vc != NULL)
		{
			pl2vc->bfd_id = 0;
		}
	}

	local_id = sess->local_id_cfg ? sess->local_id_cfg : sess->local_id;
	if( local_id )
	{
		bfd_local_id_delete(sess, local_id_list);
	}
	
	hios_hash_delete(&bfd_session_table, pitem);
	index_free(INDEX_TYPE_OAM, (uint32_t)sess_id);
	XFREE(MTYPE_BFD_SESS_ENTRY, sess);
    pitem->prev = NULL;
    pitem->next = NULL;
	XFREE(MTYPE_HASH_BACKET, pitem);
	
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_BFD_TYPE;
	gPortInfo.iMsgPara1 = sess_id;
	ipran_alarm_port_unregister(&gPortInfo);
	
	return ERRNO_SUCCESS;
}

struct bfd_sess *bfd_session_lookup(uint16_t sess_id)
{
	struct hash_bucket *pitem = NULL;

	BFD_DEBUG("--->sess_id:%d.\n", sess_id);

	pitem = hios_hash_find(&bfd_session_table, (void *)(uint32_t)sess_id);
	if (NULL == pitem)
	{
	    zlog_err("%s[%d]: In function '%s',bfd_session_lookup fail! sess_id:%d.\n", __FILE__, __LINE__, __func__, sess_id);
		return NULL;
	}

	return (struct bfd_sess *)pitem->data;
}

int bfd_session_ifname(uint16_t sess_id, char *ifname)
{
    struct hash_bucket  *pitem = NULL;
    struct bfd_sess     *psession;
    struct lsp_entry    *plsp = NULL;

	BFD_DEBUG("--->sess_id:%d.\n", sess_id);

    if (NULL == ifname)
    {
        return 0;
    }

    pitem = hios_hash_find(&bfd_session_table, (void *)(uint32_t)sess_id);
    if (NULL == pitem)
    {
        return 0;
    }

    psession = (struct bfd_sess *)pitem->data;
    if (BFD_TYPE_INTERFACE == psession->type)
    {
        ifm_get_name_by_ifindex(psession->index, ifname);
    }
    else if (BFD_TYPE_PW == psession->type)
    {
        snprintf(ifname, NAME_STRING_LEN, "%s", (char *)(pw_get_name(psession->index)));
    }
    else if (BFD_TYPE_LSP == psession->type)
    {
        plsp = mpls_lsp_lookup(psession->index);
        if (NULL != plsp)
        {
            snprintf(ifname, NAME_STRING_LEN, "%s", (char *)plsp->name);
        }
    }
    else if (BFD_TYPE_TUNNEL == psession->type)
    {
        ifm_get_name_by_ifindex(psession->index, ifname);
    }
    else
    {
        return 0;
    }

    return 1;
}

void bfd_global_disable(void)
{
	uint32_t size;
    struct hash_bucket *pbucket = NULL;
	struct bfd_sess *sess = NULL;

	/*遍历删除所有会话*/
	for(size = 0; size < HASHTAB_SIZE; size++)
	{
		pbucket = bfd_session_table.buckets[size];
		while(pbucket != NULL)
		{
			sess = pbucket->data;
			/*删除一个会话，对应删除该哈希节点*/
			bfd_fsm(BFD_EVENT_SESSION_DELETE, sess);
			pbucket = bfd_session_table.buckets[size];
		}
	}
	bfd_pkt_unregister();
}


int bfd_bind_for_lsp_pw(enum BFD_TYPE type, uint16_t sess_id, uint32_t index, uint8_t ip_flag)
{
	/*type:for if/for lsp/for pw*/
	/*local id */
	struct pw_info *ppw = NULL;
	struct l2vc_entry *lpw = NULL;
	struct bfd_sess *psess = NULL;
	int ret = 0;

	BFD_DEBUG("--->**BIND**index=0x%x.sess_id=%d\n", index, sess_id);

	if(0 == sess_id)
	{
		if(BFD_TYPE_PW == type)
		{
			/*表示新创建的会话，若绑定失败，删除会话*/
			psess = bfd_session_create(0);
			if(psess == NULL)
			{
				BFD_DEBUG("Create BFD session	failed\n" );
				return CMD_WARNING;
			}
			
			/*表示创建会话的local_id，若失败，返回 */
			bfd_local_id_create(psess, local_id_list, 0);
			if(0 == psess->local_id)
			{
				bfd_session_delete(psess->session_id);
				BFD_DEBUG("Create BFD local_id  failed\n");
				return CMD_WARNING;
			}
		}
		else
		{
			return CMD_WARNING;
		}
	}
	else
	{
		psess = bfd_session_lookup(sess_id);
		if(psess == NULL)
		{
			return ERRNO_NOT_FOUND;
		}
		else
		{
			/*只能是静态指定的bfd，可以不指定远端标识符*/
			if(psess->local_id_cfg == 0)
			{
				return ERRNO_CONFIG_INCOMPLETE;
			}
		}	 
	}

	if(psess->type != BFD_TYPE_INVALID)
	{
		return ERRNO_EXISTED;
	}

	psess->index = index;

	if(type == BFD_TYPE_LSP)
	{
		psess->bfd_event = BFD_EVENT_BIND_LSP;
	}
	else if(type == BFD_TYPE_PW)
	{
		ppw = pw_lookup(index);
		if(NULL == ppw)
		{
			return ERRNO_FAIL;
		}
		
		lpw = pw_get_l2vc(ppw);
		if(NULL == lpw)
		{
			return ERRNO_FAIL;
		}
		lpw->bfd_id = psess->session_id_cfg ? psess->session_id_cfg : psess->session_id;
		psess->bfd_event = BFD_EVENT_BIND_PW;
		psess->without_ip = ip_flag;
	}
	else if(type == BFD_TYPE_TUNNEL)
	{
		/*暂时不支持tunnel*/
		return ERRNO_SUCCESS;
	}

	bfd_fsm(psess->bfd_event,psess);

    //通知适配下表
    ret = bfd_send_hal(psess, IPC_OPCODE_ADD, 0);
	if(0 < ret)
	{
		return ret;
	}
	
	return ERRNO_SUCCESS;
}

int bfd_unbind_for_lsp_pw(enum BFD_TYPE type, uint16_t sess_id, uint32_t index)
{
	/*type:for if/for lsp/for pw*/
	/*local id */
	struct bfd_sess *psess = NULL;
    struct l2vc_entry *pl2vc = NULL;
	struct static_lsp *pslsp	= NULL;
	struct lsp_entry *plsp = NULL;
	struct pw_info *ppw = NULL;
    struct bfd_sess sessdel;

	/*为pw的bfd_id置0*/
	BFD_DEBUG("--->**UNBIND**index=0x%x.sess_id=%d\n", index, sess_id);

    memset ( &sessdel, 0, sizeof ( struct bfd_sess ) );

	psess = bfd_session_lookup(sess_id);
	if(psess == NULL)
	{
		return ERRNO_SUCCESS;
	}
	else
	{
		if(psess->type != type || psess->index != index)
		{
			return ERRNO_FAIL;
		}
	}
	if(psess->type == BFD_TYPE_PW)
	{
		pl2vc = l2vc_lookup(pw_get_name(index));
		if(pl2vc != NULL)
		{
			pl2vc->bfd_id = 0;
			ppw = pw_lookup(index);
			if(NULL != ppw)
			{
				if(AC_TYPE_TDM == ppw->ac_type)
				{
					sessdel.tdm_pw_label = pl2vc->inlabel;
				}
			}
		}
	}
	else if(psess->type == BFD_TYPE_LSP)
	{
		plsp = mpls_lsp_lookup(index);
        pslsp = static_lsp_lookup((uchar *)(plsp->name));
		if(pslsp != NULL)
		{
			pslsp->bfd_id = 0;
		}
	}
	else if(psess->type == BFD_TYPE_TUNNEL)
	{
		/*暂时不支持tunnel*/
		return ERRNO_SUCCESS;
	}

    sessdel.index = psess->index;
    sessdel.type = psess->type;
    sessdel.session_id = sess_id;

	if(0 != psess->session_id)
	{
		psess->bfd_event = BFD_EVENT_SESSION_DELETE;
		bfd_fsm(psess->bfd_event, psess);
	}
	else
	{
		psess->bfd_event = BFD_EVENT_DISABLE;
		bfd_fsm(psess->bfd_event, psess);
	}

    //配置删除
    bfd_send_hal(&sessdel, IPC_OPCODE_DELETE, 0);

	return ERRNO_SUCCESS;
}

int bfd_state_notify(enum BFD_STATUS state_now, struct bfd_sess *psess)
{
    struct pw_info *pwinfo = NULL;
    struct ldp_sess *pldp = NULL;

	if(NULL == psess)
	{
		return ERRNO_NOT_FOUND;
	}

    BFD_DEBUG("--->state:%d, type:%d.\n", state_now, psess->type);

    switch(psess->type)
    {
    case BFD_TYPE_INTERFACE:
        if(BFD_STATUS_DOWN == state_now)
        {
        }
        else if(BFD_STATUS_UP == state_now)
        {
        }
        break;
    case BFD_TYPE_IP:
        if((BFD_STATUS_DOWN == state_now) && (0 != psess->ldp_flag))
        {
            pldp = ldp_session_lookup(psess->srcip, psess->dstip);
            if(NULL != pldp)
            {
                snprintf(pldp->reason, LDP_SESS_REASON_MAX_LEN, " ldp session down for bfd down\n" );
                ldp_fsm_goto_initialization(pldp);
            }
        }
        else if(BFD_STATUS_UP == state_now)
        {
        }
        break;
    case BFD_TYPE_LSP:
        if(BFD_STATUS_DOWN == state_now)
        {
            mpls_lsp_down(psess->index);
        }
        else if(BFD_STATUS_UP == state_now)
        {
            mpls_lsp_up(psess->index);
        }
        break;
    case BFD_TYPE_PW:
        pwinfo = pw_lookup(psess->index);
        if(NULL == pwinfo)
        {
            return ERRNO_NOT_FOUND;
        }

        if(BFD_STATUS_DOWN == state_now)
        {
            pw_detection_protocal_down(pwinfo);
        }
        else if(BFD_STATUS_UP == state_now)
        {
            pw_detection_protocal_up(pwinfo);
        }
        break;
    default:
        break;
    }

    return ERRNO_SUCCESS;
}

int bfd_for_ldp_bind_unbind(enum BFD_SUBTYPE type, void *pdata)
{
	struct ldp_sess *ldpsess = NULL;
	struct bfd_sess *psess = NULL;
	struct bfd_sess sessdel;
	int ret = 0;

	if(NULL == pdata)
	{
		BFD_DEBUG("---> pdata == NULL.\n");
		return ERRNO_FAIL;
	}
	ldpsess = (struct ldp_sess *)pdata;

	switch(type)
	{
		case BFD_SUBTYPE_BIND_LDP:
			if(0 != ldpsess->bfd_sess)
			{
				return ERRNO_FAIL;
			}
			psess = bfd_session_create(0);
			if(NULL == psess)
			{
				BFD_DEBUG("--->psess == NULL.\n");
				return ERRNO_FAIL;
			}
			
			ret = bfd_local_id_create(psess, local_id_list, 0);
			if(0 != ret)
			{
				BFD_DEBUG("--->creat local_id fail.\n");
				bfd_session_delete(psess->session_id);
				return ERRNO_FAIL;
			}

			psess->srcip = ldpsess->key.local_lsrid;
			psess->dstip = ldpsess->key.peer_lsrid;
			psess->ldp_flag = 1;
			psess->bfd_event = BFD_EVENT_BIND_IF_IP;

			BFD_DEBUG("--->psess->srcip = %u psess->dstip = %u\n", psess->srcip, psess->dstip);
			bfd_fsm(BFD_EVENT_BIND_IF_IP, psess);
			ret = bfd_send_hal(psess, IPC_OPCODE_ADD, 0);
			if(0 < ret)
			{
				return ERRNO_FAIL;
			}
			ldpsess->bfd_sess = psess->session_id;
			
			break;
		case BFD_SUBTYPE_UNBIND_LDP:
			if(0 == ldpsess->bfd_sess)
			{
				return ERRNO_FAIL;
			}
			psess = bfd_session_lookup(ldpsess->bfd_sess);
			if(NULL == psess)
			{
				return ERRNO_FAIL;
			}
			
			memset ( &sessdel, 0, sizeof ( struct bfd_sess ) );
			sessdel.session_id = psess->session_id;
			sessdel.type = BFD_TYPE_IP;
			sessdel.local_id = psess->local_id;
			bfd_fsm(BFD_EVENT_SESSION_DELETE, psess);
			bfd_send_hal(&sessdel, IPC_OPCODE_DELETE, 0);
			ldpsess->bfd_sess = 0;
			
			break;
		case BFD_SUBTYPE_CFG:
			break;

		default:
			break;
	}

	return 0;
}

static unsigned int template_name_compute ( void *hash_key )
{
    return hios_hash_string ( hash_key );
}

/* 模板名称作为hash key */
static int template_name_compare ( void *pitem, void *hash_key )
{
    struct hash_bucket *pbucket = ( struct hash_bucket * ) pitem;

    if ( pitem == NULL || hash_key == NULL )
    {
        return ERRNO_FAIL;
    }

    if ( strlen ( pbucket->hash_key ) != strlen ( hash_key ) )
    {
        return ERRNO_FAIL;
    }

    if ( 0 == memcmp ( pbucket->hash_key, hash_key, strlen ( hash_key ) ) )
    {
        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}

void bfd_template_table_init(unsigned int size)
{
    hios_hash_init(&bfd_template_table, size, template_name_compute, template_name_compare);
}


struct bfd_template *bfd_template_create()
{
	struct bfd_template *ptemplate = NULL;

	if(bfd_template_table.hash_size == bfd_template_table.num_entries)
	{
		zlog_err("%s[%d]: In function '%s' The entry number exceed limit %d.\n",
							__FILE__, __LINE__, __func__, BFD_NUM_MAX);
		return NULL;
	}

	ptemplate = (struct bfd_template *)XCALLOC(MTYPE_BFD_TEMPLATE, sizeof(struct bfd_template));
	if(NULL == ptemplate)
	{
		zlog_err("%s[%d]: In function '%s' Memory alloc failed !\n",
						__FILE__, __LINE__, __func__);
		return NULL;
	}
	memset(ptemplate, 0, sizeof(struct bfd_template));
	
	ptemplate->bfdindex = index_alloc(INDEX_TYPE_OAM);
	ptemplate->auth_mode = 0;
	ptemplate->detect_mult = BFD_DETECT_MULTIPLIER;
	ptemplate->cc_rx_interval = BFD_CC_INTERVAL;
	ptemplate->cc_tx_interval = BFD_CC_INTERVAL;

	return ptemplate;
}

struct bfd_template *bfd_template_lookup(uchar *pname)
{
    struct hash_bucket *pitem = NULL;

    if (pname == NULL)
    {
        return NULL;
    }

    pitem = hios_hash_find(&bfd_template_table, pname);
    if (pitem == NULL)
    {
        return NULL;
    }

    return (struct bfd_template *)pitem->data;
}

int bfd_template_add(struct bfd_template *ptemplate)
{
    struct hash_bucket *pitem = NULL;

	BFD_DEBUG("--->bfd_template_add\n");

    if (NULL == ptemplate)
    {
    	zlog_err("--->%s %d ----bfd_template is NULL------", __FUNCTION__, __LINE__);
        return ERRNO_PARAM_ILLEGAL;
    }

    if (bfd_template_table.hash_size == bfd_template_table.num_entries)
    {
        return ERRNO_OVERSIZE;
    }

    HASH_BUCKET_CREATE(pitem);
    if (NULL == pitem)
    {
        return ERRNO_MALLOC;;
    }

    pitem->hash_key = ptemplate->template_name;
    pitem->data     = ptemplate;
    hios_hash_add(&bfd_template_table, pitem);

	return ERRNO_SUCCESS;
}

int bfd_template_delete(uchar *pname)
{
    struct hash_bucket *pitem = NULL;
    struct bfd_template  *ptemplate = NULL;

    if ( NULL == pname)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pitem = hios_hash_find(&bfd_template_table, pname);
    if (NULL == pitem)
    {
        return ERRNO_SUCCESS;
    }
	ptemplate = (struct bfd_template *)pitem->data;

	
	index_free(INDEX_TYPE_OAM, (uint32_t)ptemplate->bfdindex);
	XFREE(MTYPE_BFD_TEMPLATE, ptemplate);
    hios_hash_delete(&bfd_template_table, pitem);
    HASH_BUCKET_DESTROY(pitem);

	return ERRNO_SUCCESS;
}

int bfd_bind_for_lsp_pw_h3c(enum BFD_TYPE type, struct nni_info *nni)
{
	struct bfd_template *ptem = NULL;
	struct bfd_sess *psess = NULL;
	int ret = ERRNO_SUCCESS;

	if(NULL == nni)
	{
		return ERRNO_SUCCESS;
	}

	if(ENABLE != nni->pw_class.bfd_flag)
	{
		return ERRNO_SUCCESS;
	}

	psess = bfd_session_create(0);
	if(NULL == psess)
	{
		return ERRNO_FAIL;
	}
	bfd_local_id_create(psess, local_id_list, 0);
	if(0 == psess->local_id)
	{
		bfd_session_delete(psess->session_id);
		return ERRNO_FAIL;
	}

	psess->index = nni->pwid;
	if(ENABLE == nni->pw_class.bfd_chtype)
	{
		psess->without_ip = 1;
	}

	memcpy(psess->pwclass_name, nni->pw_class.name, NAME_STRING_LEN);

	if('\0' != nni->pw_class.bfd_temp[0])
	{
		//索引模板，使用模板参数；
		ptem = bfd_template_lookup((uchar *)(nni->pw_class.bfd_temp));
		if(NULL == ptem)
		{
			//使用全局配置；
		}
		else
		{
			//使用指定模板配置；
			psess->ptem = ptem;
			psess->cc_interval_cfg = ptem->cc_rx_interval;
			psess->cc_multiplier_cfg = ptem->detect_mult;
		}
	}
	
	//使能bfd for pw；
	psess->type = BFD_TYPE_PW;
	psess->bfd_event = BFD_EVENT_BIND_PW;
	bfd_fsm(psess->bfd_event,psess);
	
	ret = bfd_send_hal(psess, IPC_OPCODE_ADD, 0);
	if(0 < ret)
	{
		return ret;
	}
	nni->bfd_id = psess->session_id;

	return ERRNO_SUCCESS;
}

int bfd_unbind_for_lsp_pw_h3c(uint16_t sess_id)
{
	struct bfd_sess *psess = NULL;
	struct bfd_sess sessdel;

    memset (&sessdel, 0, sizeof(struct bfd_sess));

	psess = bfd_session_lookup(sess_id);
	if(NULL == psess)
	{
		return ERRNO_SUCCESS;
	}
    sessdel.index = psess->index;
    sessdel.type = BFD_TYPE_PW;
	psess->ptem = NULL;
	
	if(psess->session_id)
	{
		sessdel.session_id = psess->session_id;
	    sessdel.local_id = psess->local_id;
		bfd_fsm(BFD_EVENT_SESSION_DELETE, psess);
	}

    //通知适配删表
    bfd_send_hal(&sessdel, IPC_OPCODE_DELETE, 0);

	return ERRNO_SUCCESS;
}

int bfd_update_sess_when_del_template(struct bfd_template *ptem)
{
	enum BFD_EVENT event = BFD_EVENT_MAX;
	struct hash_bucket *pbucket = NULL;
	struct bfd_sess *psess = NULL;
	uint16_t cursor = 0;

	if(NULL == ptem)
	{
		return ERRNO_SUCCESS;
	}

	HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
	{
		if(pbucket && pbucket->data)
		{
			psess = (struct bfd_sess *)pbucket->data;
			if(NULL != psess->ptem)
			{
				if(0 == strcmp((char *)ptem->template_name, (char *)psess->ptem->template_name))
				{
					if(gbfd.cc_interval > psess->cc_interval_cfg)
					{
						event = BFD_EVENT_INTERVAL_INC;
					}
					else
					{
						event = BFD_EVENT_INTERVAL_DEC;
					}

					psess->ptem = NULL;
					psess->cc_interval_cfg = 0;
					psess->cc_multiplier_cfg = 0;
					bfd_send_poll(psess, event);
					
				}
			}
	
		}
	}
	
	return ERRNO_SUCCESS;
}

int pw_class_change_bfd_template(struct pw_class *pclass)
{
	enum BFD_EVENT event = BFD_EVENT_MAX;
    struct bfd_template  *ptem = NULL;
	struct hash_bucket *pbucket = NULL;
	struct bfd_sess *psess = NULL;
	uint16_t cursor = 0;
	uint32_t cc_interval = 0;

	if(NULL == pclass)
	{
		return ERRNO_SUCCESS;
	}

	/*pclass->bfd_flag=0：去使能模板下的bfd；pclass->bfd_flag=1：更新模板下的bfd会话*/
	if(DISABLE == pclass->bfd_flag)
	{
		HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
		{
			if(pbucket && pbucket->data)
			{
				psess = (struct bfd_sess *)pbucket->data;
				if(0 == strcmp(pclass->name, (char *)psess->pwclass_name))
				{
					bfd_unbind_for_lsp_pw_h3c(psess->session_id);
				}
		
			}
		}
	}
	else
	{
		ptem = bfd_template_lookup((uchar *)(pclass->bfd_temp));
		if(NULL == ptem)
		{
			//更新会话，使用全局配置；
			HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
			{
				if(pbucket && pbucket->data)
				{
					psess = (struct bfd_sess *)pbucket->data;
					cc_interval = psess->cc_interval_cfg ? psess->cc_interval_cfg : gbfd.cc_interval;
					
					if(0 == strcmp(pclass->name, (char *)psess->pwclass_name))
					{
						psess->ptem = NULL;
						psess->cc_interval_cfg = 0;
						psess->cc_multiplier_cfg = 0;
					
						if(pclass->bfd_chtype != psess->without_ip)
						{
							psess->without_ip = pclass->bfd_chtype;
							bfd_fsm(BFD_EVENT_RECV_DOWN, psess);
						}
						else
						{
							if(gbfd.cc_interval > cc_interval)
							{
								event = BFD_EVENT_INTERVAL_INC;
							}
							else
							{
								event = BFD_EVENT_INTERVAL_DEC;
							}
							bfd_send_poll(psess, event);
						}
						
					}
			
				}
			}
		}
		else
		{
			//使用新指定的模板配置更新会话；
			HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
			{
				if(pbucket && pbucket->data)
				{
					psess = (struct bfd_sess *)pbucket->data;
					cc_interval = psess->cc_interval_cfg ? psess->cc_interval_cfg : gbfd.cc_interval;
					
					if(0 == strcmp(pclass->name, (char *)psess->pwclass_name))
					{
						psess->ptem = ptem;
						psess->cc_interval_cfg = ptem->cc_rx_interval;
						psess->cc_multiplier_cfg = ptem->detect_mult;
						
						if(pclass->bfd_chtype != psess->without_ip)
						{
							psess->without_ip = pclass->bfd_chtype;
							bfd_fsm(BFD_EVENT_RECV_DOWN, psess);
						}
						else
						{
							if(psess->cc_interval_cfg > cc_interval)
							{
								event = BFD_EVENT_INTERVAL_INC;
							}
							else
							{
								event = BFD_EVENT_INTERVAL_DEC;
							}
							bfd_send_poll(psess, event);
						}
						
					}
			
				}
			}
		}
	}

	return ERRNO_SUCCESS;
}


