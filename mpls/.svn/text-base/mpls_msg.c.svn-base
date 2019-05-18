/**
 * @file      : mpls_msg.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月8日 15:08:20
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <sys/msg.h>
#include <lib/zassert.h>
#include <lib/memory.h>
#include <lib/log.h>
#include <lib/errcode.h>
#include <lib/ifm_common.h>
#include <lib/devm_com.h>
#include <ftm/pkt_tcp.h>
#include <sys/prctl.h>
#include "mpls_main.h"
#include "mpls.h"
#include "lsp_static.h"
#include "pw.h"
#include "vpls.h"
#include "tunnel.h"
#include "mpls_ping.h"
#include "l3vpn.h"
#include "mpls_register.h"
#include "mpls_msg.h"
#include "ldp/ldp_packet.h"
#include "ldp/ldp_fec.h"
#include "ldp/ldp_address.h"
#include "labelm.h"
#include "lspm.h"
#include "bfd/bfd_packet.h"
#include "mpls_oam/mpls_oam.h"
#include "mpls_oam/mpls_oam_pkt.h"
#include "mpls_aps/mpls_aps_fsm.h"
#include "mpls_oam/mpls_oam_lck.h"
#include "mpls_oam/mpls_oam_csf.h"


extern int vsi_get_vsi_black_white_mac_bulk(struct mpls_mac *pindex, uint32_t data_len, struct mpls_mac*mac_buf);
extern int vsi_get_pw_l2vc_bulk(uint32_t vsi_id, struct l2vc_entry *pindex, uint32_t data_len, struct l2vc_entry *l2vc_buf);
extern int vsi_get_smac_bulk(struct mpls_smac *pindex, uint32_t data_len, struct mpls_smac*mac_buf);
extern int l2vpn_pw_class_get_bulk(struct pw_class *pindex, int index_flag, struct pw_class *pbuff);
extern int l2vpn_service_instance_get_bulk(struct serv_ins_mib *pindex, int index_flag, struct serv_ins_mib *pbuff);
extern int l2vpn_xc_group_get_bulk(struct xc_group_mib *pindex, int index_flag, struct xc_group_mib *pbuff);
extern int mpls_service_get_bulk(struct mpls_service *pindex, uint32_t index_flag, struct mpls_service *pbuf);
extern int label_table_get_bulk(struct label_info *pindex, uint32_t index_flag, struct label_info *pbuf);

/**
 * @brief      : mpls 处理接口消息
 * @param[in ] : pmsghdr - ipc 消息�?
 * @param[in ] : pdata   - ipc 消息内容
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018�?3�?8�? 15:30:07
 * @note       :
 */
static void mpls_ctr_msg_ifevent_process(struct ipc_msghdr_n *pmsghdr, void *pdata)
{
    struct ifm_event *pevent = NULL;
    uint32_t ifindex;

    if (pmsghdr->opcode == IPC_OPCODE_EVENT)
    {
        pevent  = (struct ifm_event *)pdata;
        ifindex = pevent->ifindex;

        if (pmsghdr->msg_subtype == IFNET_EVENT_DOWN)
        {
            MPLS_LOG_DEBUG("recv event notify ifindex 0x%0x down\n", ifindex);

            mpls_if_down(ifindex, pevent->mode);
        }
        else if (pmsghdr->msg_subtype == IFNET_EVENT_UP)
        {
            MPLS_LOG_DEBUG("recv event notify ifindex 0x%0x up\n", ifindex);

            mpls_if_up(ifindex, pevent->mode);
        }
        else if (pmsghdr->msg_subtype == IFNET_EVENT_IF_DELETE)
        {
            MPLS_LOG_DEBUG("recv event notify ifindex 0x%0x delete\n", ifindex);

            mpls_if_delete(ifindex);
        }
        else if (pmsghdr->msg_subtype == IFNET_EVENT_MODE_CHANGE)
        {
            MPLS_LOG_DEBUG("recv event notify ifindex 0x%0x change mode %d\n", ifindex, pevent->mode);

            mpls_if_mode_change(ifindex, pevent->mode);
        }
        else if (pmsghdr->msg_subtype == IFNET_EVENT_IP_ADD)
        {
            MPLS_LOG_DEBUG("recv event notify ifindex 0x%0x addr add %d\n", ifindex, pevent->mode);

            ldp_respond_ifaddr_event(ifindex, pevent, IFNET_EVENT_IP_ADD);
        }
        else if (pmsghdr->msg_subtype == IFNET_EVENT_IP_DELETE)
        {
            MPLS_LOG_DEBUG("recv event notify ifindex 0x%0x addr delete %d\n", ifindex, pevent->mode);

            ldp_respond_ifaddr_event(ifindex, pevent, IFNET_EVENT_IP_DELETE);
        }
    }

    return;
}


/* 接收 bfd 消息 */
static int mpls_msg_rcv_get_bfd(struct ipc_msghdr_n *pmsghdr, void *pdata)
{
    struct bfd_sess *psess    = NULL;
    char            ifname[NAME_STRING_LEN];
    int             ret = 0;

	BFD_DEBUG();
	if( (NULL == pmsghdr)||(NULL == pdata) )
	{
		BFD_DEBUG("pointer is NULL\n");
		return ERRNO_FAIL;
	}

    uint32_t *sessid = (uint32_t *)pdata;

    if(BFD_SUBTYPE_SESSION == pmsghdr->msg_subtype)
    {
        psess = bfd_session_lookup(pmsghdr->msg_index);
        if (NULL == psess)
        {
            ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS,
                        IPC_TYPE_BFD, pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
        }
        else
        {
            ret = ipc_send_reply_n2(psess, sizeof(struct bfd_sess), 1, pmsghdr->sender_id, MODULE_ID_MPLS,
                        IPC_TYPE_BFD, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
        }
    }
    else if(BFD_SUBTYPE_GLOBAL == pmsghdr->msg_subtype)
    {
            ret = ipc_send_reply_n2(&gbfd, sizeof(struct bfd_global), 1, pmsghdr->sender_id, MODULE_ID_MPLS,
                        IPC_TYPE_BFD, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
    }
    else if(BFD_SUBTYPE_IFNAME == pmsghdr->msg_subtype)
    {
        memset (ifname, 0, NAME_STRING_LEN);
        //ret = bfd_session_ifname(pmsghdr->msg_index, ifname);


        ret = bfd_session_ifname(*sessid, ifname);
        if (0 == ret)
        {
            ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS,
                        IPC_TYPE_BFD, pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
        }
        else
        {
            ret = ipc_send_reply_n2(ifname, NAME_STRING_LEN, 1, pmsghdr->sender_id, MODULE_ID_MPLS,
                        IPC_TYPE_BFD, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
        }
    }

    return ret;
}

static int mpls_get_bulk_bfd_template(struct bfd_template *meg_buf, uint32_t bfdindex, uint32_t meg_max)

{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct bfd_template *temp   = NULL;
	uchar pname[NAME_STRING_LEN] = {0};
    uint32_t meg_cnt          	= 0;
	int cursor                  = 0;
    uint32_t val 				= 0;

	BFD_DEBUG();
	
	if(NULL == meg_buf)
	{
		BFD_DEBUG("meg_buf is NULL\n");
        return 0;
	}

	if(0 == bfdindex)
	{
		HASH_BUCKET_LOOP(pbucket, cursor, bfd_template_table)
		{
			temp = (struct bfd_template *)pbucket->data;
			if (NULL == temp)
			{
				continue;
			}
			memcpy((meg_buf + meg_cnt), temp, sizeof(struct bfd_template));
			meg_cnt++;
			if (meg_cnt == meg_max)
			{
				return meg_cnt;
			}
		}
	}
	else
	{
		HASH_BUCKET_LOOP(pbucket, cursor, bfd_template_table)
		{
			temp = (struct bfd_template *)pbucket->data;
			if (NULL == temp)
			{
				continue;
			}

			if(bfdindex == temp->bfdindex)
			{
				memcpy(pname, temp->template_name, NAME_STRING_LEN);
			}
		}

        pbucket = hios_hash_find(&bfd_template_table, (void *)pname);
        if (NULL == pbucket)
        {
            val = bfd_template_table.compute_hash((void *)pname)%HASHTAB_SIZE;

            if (NULL != bfd_template_table.buckets[val])
            {
                pbucket = bfd_template_table.buckets[val];
            }
            else
            {
                for (++val; val<HASHTAB_SIZE; ++val)
                {
                    if (NULL != bfd_template_table.buckets[val])
                    {
                        pbucket = bfd_template_table.buckets[val];
                        break;
                    }
                }
            }
        }

		while (pbucket)
        {
            pnext = hios_hash_next_cursor(&bfd_template_table, pbucket);
            if ((NULL == pnext) || (NULL == pnext->data))
            {
                break;
            }
            pbucket = pnext;
            temp = (struct bfd_template *)(pnext->data);

			memcpy((meg_buf + meg_cnt), temp, sizeof(struct bfd_template));
			meg_cnt++;
            if (meg_cnt == meg_max)
            {
                return meg_cnt;
            }
        }

	}

	return meg_cnt;
}

static int mpls_get_bulk_bfd(struct bfd_sess *bfd_buf, uint32_t session_id, uint32_t bfd_max)
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct bfd_sess    *psess   = NULL;
	uint32_t bfd_cnt            = 0;
	int cursor                  = 0;
    int val 					= 0;

	BFD_DEBUG();
	if(NULL == bfd_buf)
	{
		BFD_DEBUG("bfd_buf is NULL\n");
		return 0;
	}
	BFD_DEBUG("sess_id = %d bfd_max = %d", session_id, bfd_max);

	if (0 == session_id)   /* 初次获取 */
	{
		HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table)
		{
			psess = (struct bfd_sess *)pbucket->data;
			if (NULL == psess)
			{
				continue;
			}

        	memcpy(&bfd_buf[bfd_cnt], psess, sizeof(struct bfd_sess));
			bfd_buf[bfd_cnt].session_id = psess->session_id_cfg ?  psess->session_id_cfg : psess->session_id;
			bfd_buf[bfd_cnt].local_id = psess->local_id_cfg ? psess->local_id_cfg : psess->local_id;
			bfd_buf[bfd_cnt].remote_id = psess->remote_id_cfg ?  psess->remote_id_cfg : psess->remote_id;

			bfd_cnt++;
        	if (bfd_cnt == bfd_max)
        	{
            	return bfd_cnt;
        	}
		}
	}
	else				/* 找到 session_id后面第一个数据 */
	{
		pbucket = hios_hash_find(&bfd_session_table, (void *)session_id);
		if (NULL == pbucket)	/* 未找到 session_id 对应的数据 */
		{
			/* 查找失败返回到当前哈希桶的链表头部 */
			val = bfd_session_table.compute_hash((void *)session_id)%HASHTAB_SIZE;

			if (NULL != bfd_session_table.buckets[val]) 		/* 当前哈希桶链表不为空 */
			{
				pbucket = bfd_session_table.buckets[val];
			}
			else										/* 当前哈希桶链表为空 */
			{
				for (++val; val<HASHTAB_SIZE; ++val)
				{
					if (NULL != bfd_session_table.buckets[val])
					{
						pbucket = bfd_session_table.buckets[val];
						break;
					}
				}
			}
		}

		if (NULL != pbucket)
		{
			for (bfd_cnt = 0; bfd_cnt < bfd_max; bfd_cnt++)
			{
				pnext = hios_hash_next_cursor(&bfd_session_table, pbucket);
				if ((NULL == pnext) || (NULL == pnext->data))
				{
					break;
				}
				psess = pnext->data;

	        	memcpy(&bfd_buf[bfd_cnt], psess, sizeof(struct bfd_sess));
				bfd_buf[bfd_cnt].session_id = psess->session_id_cfg ?  psess->session_id_cfg : psess->session_id;
				bfd_buf[bfd_cnt].local_id = psess->local_id_cfg ? psess->local_id_cfg : psess->local_id;
				bfd_buf[bfd_cnt].remote_id = psess->remote_id_cfg ?  psess->remote_id_cfg : psess->remote_id;

				pbucket = pnext;
			}
		}
	}

	return bfd_cnt;
}

/* 接收 bfd 消息 */
static int mpls_msg_rcv_get_bulk_bfd(struct ipc_msghdr_n  *pmsghdr, void *pdata)
{
	uint32_t bfd_max            = IPC_MSG_LEN/sizeof(struct bfd_sess);
	uint32_t temp_max			= IPC_MSG_LEN/sizeof(struct bfd_template);
    struct bfd_sess bfd_buf[bfd_max];
	struct bfd_template temp_buf[temp_max];
	uint32_t bfd_cnt            = 0;
	int ret                     = 0;

	BFD_DEBUG();
	if(NULL == pmsghdr)
	{
		BFD_DEBUG("pointer = NULL\n");
		return ERRNO_FAIL;
	}
	memset(bfd_buf, 0, bfd_max * sizeof(struct bfd_sess));
	memset(temp_buf, 0, temp_max * sizeof(struct bfd_template));

	BFD_DEBUG("subtype = %d\n", pmsghdr->msg_subtype);
	switch(pmsghdr->msg_subtype)
	{
		case BFD_SUBTYPE_TEMPLATE:
			bfd_cnt = mpls_get_bulk_bfd_template(temp_buf, pmsghdr->msg_index, temp_max);
			if(bfd_cnt > 0){
				ret = ipc_send_reply_n2(temp_buf, bfd_cnt * sizeof(struct bfd_template), bfd_cnt, pmsghdr->sender_id,
									 MODULE_ID_MPLS, IPC_TYPE_BFD, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
			}
			else{
				ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS, IPC_TYPE_BFD,
									 pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
			}
			
			break;
		case BFD_SUBTYPE_DETAIL:
			bfd_cnt = mpls_get_bulk_bfd(bfd_buf, pmsghdr->msg_index, bfd_max);
			if (bfd_cnt > 0){
				ret = ipc_send_reply_n2(bfd_buf, bfd_cnt * sizeof(struct bfd_sess), bfd_cnt, pmsghdr->sender_id,
									 MODULE_ID_MPLS, IPC_TYPE_BFD, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
			}
			else{
				ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS, IPC_TYPE_BFD,
									 pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
			}
			
			break;
		default:
			bfd_cnt = mpls_get_bulk_bfd(bfd_buf, pmsghdr->msg_index, bfd_max);
			if (bfd_cnt > 0){
				ret = ipc_send_reply_n2(bfd_buf, bfd_cnt * sizeof(struct bfd_sess), bfd_cnt, pmsghdr->sender_id,
									 MODULE_ID_MPLS, IPC_TYPE_BFD, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
			}
			else{
				ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS, IPC_TYPE_BFD,
									 pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
			}
			
			break;
	}

	return ret;
}

static int bfd_add_session_for_route(struct bfd_sess *rsess, struct list *list, int module_id, uint16_t flag)
{
	struct bfd_sess *psess = NULL;
	struct bfd_ip_map *bfd_ip = NULL;
	struct listnode *bfd_next_node = NULL;
	struct listnode *bfd_node = NULL;
	char ifname[INTERFACE_NAMSIZE+1] = "";
	
    uint32_t ret = 0;

	BFD_DEBUG();
	if( (NULL == rsess)||(NULL == list) )
	{
		BFD_DEBUG("pointer NULL\n");
		return ERRNO_FAIL;
	}

	inet_ipv4tostr(rsess->srcip, ifname);
	rsess->srcip = ntohl(rsess->srcip);
	inet_ipv4tostr(rsess->srcip, ifname);

	inet_ipv4tostr(rsess->dstip, ifname);
	rsess->dstip = ntohl(rsess->dstip);
	inet_ipv4tostr(rsess->dstip, ifname);

	if(1 == flag)
	{
		for(ALL_LIST_ELEMENTS (list, bfd_node, bfd_next_node, bfd_ip))
		{
			BFD_DEBUG("for running  #################!\n");
			if(rsess->dstip == bfd_ip->dest_ip)
			{
				BFD_DEBUG("abfd_ip_dest = %d !\n", bfd_ip->dest_ip);
				psess = bfd_session_lookup(bfd_ip->sess_id);
				ret = ipc_send_msg_n2( psess, sizeof ( struct bfd_sess ), 1, module_id, MODULE_ID_MPLS, IPC_TYPE_BFD, BFD_SUBTYPE_REPLY, IPC_OPCODE_UPDATE, 0);
				if(0 < ret)
				{
					BFD_DEBUG("BFD_SUBTYPE_REPLY bfd fail !\n");
					return ERRNO_FAIL;
				}

				return ERRNO_SUCCESS;
			}
			BFD_DEBUG("all_list_element ###### !\n");
		}
	}

	bfd_ip = (struct bfd_ip_map *) XCALLOC ( MTYPE_BFD_SESS_ENTRY, sizeof (struct bfd_ip_map) );
	if(NULL == bfd_ip)
	{
		BFD_DEBUG("XCALLOC fail !\n");
		return ERRNO_FAIL;
	}

	psess = bfd_session_create(0);
	if(psess == NULL)
	{
		BFD_DEBUG("Create bfd session fail !\n");
		return ERRNO_FAIL;
	}
	bfd_local_id_create(psess, local_id_list, 0);
	if(0 == psess->local_id)
	{
		bfd_session_delete(psess->session_id);
		BFD_DEBUG("Create bfd session local_id fail !\n");
		return ERRNO_FAIL;
	}

	psess->index = rsess->index;
	psess->srcip = rsess->srcip;
	psess->dstip = rsess->dstip;
	psess->cc_interval_cfg = rsess->cc_interval_cfg;
	psess->cc_multiplier_cfg = rsess->cc_multiplier_cfg;
	bfd_ip->dest_ip = psess->dstip;
	bfd_ip->sess_id = psess->session_id;

	bfd_fsm(BFD_EVENT_BIND_IF_IP, psess);

	//通知适配下表
	ret = bfd_send_hal(psess, IPC_OPCODE_ADD, 0);
	if(0 < ret)
	{
		BFD_DEBUG("IPC_OPCODE_ADD bfd fail !\n");
		return ERRNO_FAIL;
	}

	listnode_add (list, (void *)(bfd_ip));

	ret = ipc_send_msg_n2 ( psess, sizeof ( struct bfd_sess ), 1, module_id, MODULE_ID_MPLS, IPC_TYPE_BFD, BFD_SUBTYPE_REPLY, IPC_OPCODE_UPDATE, 0);
	if(0 < ret)
	{
		BFD_DEBUG("BFD_SUBTYPE_REPLY bfd fail !\n");
		return ERRNO_FAIL;
	}

	return ERRNO_SUCCESS;
}

static int bfd_del_session_for_route(struct bfd_sess *rsess, struct list *list, int module_id, uint8_t flag)
{
	struct bfd_sess *psess = NULL;
	struct bfd_ip_map *bfd_ip = NULL;
	struct listnode *bfd_next_node = NULL;
	struct listnode *bfd_node = NULL;
	struct bfd_sess sessdel;
	uint32_t ret = 0;

	BFD_DEBUG();
	if( (0 == flag)||(NULL == rsess) )
	{
		BFD_DEBUG("pif->bfd has NULL !\n");
		return ERRNO_FAIL;
	}

	for(ALL_LIST_ELEMENTS (list, bfd_node, bfd_next_node, bfd_ip))
	{
		psess = bfd_session_lookup(bfd_ip->sess_id);
		if(NULL == psess)
		{
			BFD_DEBUG("psess is not find !\n");
			continue;
		}

		memset (&sessdel, 0, sizeof ( struct bfd_sess ));
		sessdel.index = psess->index;
		sessdel.type = psess->type;
		sessdel.session_id = psess->session_id;
		sessdel.local_id = psess->local_id;
		bfd_fsm(BFD_EVENT_SESSION_DELETE, psess);

		ret = bfd_send_hal(&sessdel, IPC_OPCODE_DELETE, 0);
		if(0 < ret)
		{
			BFD_DEBUG("IPC_OPCODE_DELETE bfd fail !\n");
			return ERRNO_FAIL;
		}
	}

	list_delete_all_node (list);

	return ERRNO_SUCCESS;
}

static int mpls_msg_rcv_add_bfd(struct ipc_msghdr_n *pmsghdr, void *pdata)
{
	struct mpls_if *pif = NULL;
	struct bfd_sess *rsess = NULL;

	BFD_DEBUG();
	if( (NULL == pmsghdr)||(NULL == pdata) )
	{
		BFD_DEBUG("Pointer NULL !\n");
		return ERRNO_FAIL;
	}
	rsess = (struct bfd_sess *)pdata;

	pif = mpls_if_get(rsess->index);
	if(NULL == pif)
	{
		BFD_DEBUG("pif Pointer NULL !\n");
		return ERRNO_FAIL;
	}

	if(NULL == pif->list)
	{
		pif->list = list_new();			//创建list
		if(NULL == pif->list)
		{
			BFD_DEBUG("malloc list fial !\n");
			return ERRNO_FAIL;
		}
	}

	switch (pmsghdr->msg_subtype)
	{
		case BFD_SUBTYPE_BIND_OSPF:
			bfd_add_session_for_route(rsess, pif->list, MODULE_ID_OSPF, pif->ospf_flag);
			pif->ospf_flag = 1;
			break;
		case BFD_SUBTYPE_BIND_ISIS:
			bfd_add_session_for_route(rsess, pif->list, MODULE_ID_ISIS, pif->isis_flag);
			pif->isis_flag = 1;
			break;
		case BFD_SUBTYPE_BIND_RIP:
			bfd_add_session_for_route(rsess, pif->list, MODULE_ID_RIP, pif->rip_flag);
			pif->rip_flag = 1;
			break;
		case BFD_SUBTYPE_BIND_BGP:
			bfd_add_session_for_route(rsess, pif->list, MODULE_ID_BGP, pif->bgp_flag);
			pif->bgp_flag = 1;
			break;
		case BFD_SUBTYPE_CFG:
			break;
		default:
			break;
	}

	return ERRNO_SUCCESS;
}

static int mpls_msg_recv_update_bfd(struct ipc_msghdr_n *pmsghdr, void *pdata)
{
	enum BFD_EVENT event = BFD_EVENT_CHANGE_PARA;
	uint32_t interval = 0;
	uint16_t multiplier = 0;
	uint16_t sess_id = 0;
	struct bfd_sess *psess = NULL;

	BFD_DEBUG();
	if( (NULL == pmsghdr)||(NULL == pdata) )
	{
		BFD_DEBUG("Pointer NULL !\n");
		return ERRNO_FAIL;
	}
	struct bfd_sess *rsess = (struct bfd_sess *)pdata;

	sess_id = (rsess->session_id_cfg ? rsess->session_id_cfg : rsess->session_id);

	psess = bfd_session_lookup(sess_id);
	if(NULL == psess)
	{
		BFD_DEBUG("psess Pointer NULL !\n");
		return ERRNO_FAIL;
	}
	interval = (psess->cc_interval_cfg ? psess->cc_interval_cfg : gbfd.cc_interval);
	multiplier = (psess->cc_multiplier_cfg ? psess->cc_multiplier_cfg : gbfd.multiplier);
    if((0 != rsess->cc_interval_cfg) && (interval != rsess->cc_interval_cfg))
    {
        if(rsess->cc_interval_cfg > interval)
        {
            event = BFD_EVENT_INTERVAL_INC;
        }
        else
        {
            event = BFD_EVENT_INTERVAL_DEC;
        }
        psess->cc_interval_cfg = rsess->cc_interval_cfg;
        bfd_send_poll(psess, event);
    }
    if((0 != rsess->cc_multiplier_cfg) && (multiplier != rsess->cc_multiplier_cfg))
    {
        psess->cc_multiplier_cfg = rsess->cc_multiplier_cfg;
        event = BFD_EVENT_CHANGE_PARA;
        bfd_send_poll(psess, event);
    }

	return ERRNO_SUCCESS;
}
static int mpls_msg_rcv_del_bfd(struct ipc_msghdr_n *pmsghdr, void *pdata)
{
	struct mpls_if *pif = NULL;
	struct bfd_sess *rsess = NULL;

	BFD_DEBUG();
	if( (NULL == pmsghdr)||(NULL == pdata) )
	{
		BFD_DEBUG("Pointer NULL !\n");
		return ERRNO_FAIL;
	}
	rsess = (struct bfd_sess *)pdata;
	
	pif = mpls_if_lookup(rsess->index);
	if(NULL == pif)
	{
		BFD_DEBUG("pif Pointer NULL !\n");
		return ERRNO_FAIL;
	}

	if(NULL == pif->list)
	{
		BFD_DEBUG("list bfd has free over !\n");
		return ERRNO_FAIL;
	}

	BFD_DEBUG("subtype = %d\n", pmsghdr->msg_subtype);
	switch (pmsghdr->msg_subtype)
	{
		case BFD_SUBTYPE_UNBIND_OSPF:
			if((pif->isis_flag) || (pif->rip_flag) || (pif->bgp_flag))
			{
				pif->ospf_flag = 0;
				return ERRNO_SUCCESS;
			}
			bfd_del_session_for_route(rsess, pif->list, MODULE_ID_OSPF, pif->ospf_flag);
			pif->ospf_flag = 0;
			break;
		case BFD_SUBTYPE_UNBIND_ISIS:
			if((pif->ospf_flag) || (pif->rip_flag) || (pif->bgp_flag))
			{
				pif->isis_flag = 0;
				return ERRNO_SUCCESS;
			}
			bfd_del_session_for_route(rsess, pif->list, MODULE_ID_ISIS, pif->isis_flag);
			pif->isis_flag = 0;
			break;
		case BFD_SUBTYPE_UNBIND_RIP:
			if((pif->isis_flag) || (pif->ospf_flag) || (pif->bgp_flag))
			{
				pif->rip_flag = 0;
				return ERRNO_SUCCESS;
			}
			bfd_del_session_for_route(rsess, pif->list, MODULE_ID_RIP, pif->rip_flag);
			pif->rip_flag = 0;
			break;
		case BFD_SUBTYPE_UNBIND_BGP:
			if((pif->isis_flag) || (pif->rip_flag) || (pif->ospf_flag))
			{
				pif->bgp_flag = 0;
				return ERRNO_SUCCESS;
			}
			bfd_del_session_for_route(rsess, pif->list, MODULE_ID_BGP, pif->bgp_flag);
			pif->bgp_flag = 0;
			break;
		default:
			break;
	}

	return ERRNO_SUCCESS;
}

/* 接收 bfd 消息 */
static void mpls_msg_rcv_bfd(struct ipc_msghdr_n  *pmsghdr, void *pdata)
{
    struct bfd_info *bfdevent = NULL;
    struct bfd_sess *psess    = NULL;

	BFD_DEBUG();
	if( (NULL == pmsghdr)||(NULL == pdata) )
	{
		BFD_DEBUG("pointer is NULL\n");
		return;
	}

	if (pmsghdr->opcode == IPC_OPCODE_EVENT)
	{
		bfdevent = (struct bfd_info *)pdata;
		if ((psess = bfd_session_lookup(bfdevent->sess_id)) == NULL)
		{
			return;
		}
		BFD_DEBUG("local_id=%d,	bfd_event=%d !!!!\n", bfdevent->local_id, pmsghdr->msg_subtype);

		if (pmsghdr->msg_subtype == BFD_DIAG_NEIGHBOR_DOWN)
		{
			psess->bfd_event = BFD_EVENT_RECV_DOWN;
			bfd_fsm(psess->bfd_event,psess);
		}
		else if (pmsghdr->msg_subtype == BFD_DIAG_DETECT_EXPIRE)
		{
			psess->bfd_event = BFD_EVENT_TIMEOUT;
			bfd_fsm(psess->bfd_event,psess);
		}
		else if (pmsghdr->msg_subtype == BFD_DIAG_ADMIN_DOWN)
		{
			psess->bfd_event = BFD_EVENT_RECV_ADMINDOWN;
			bfd_fsm(psess->bfd_event,psess);
		}
	}

	return;
}

static int mpls_get_bulk_tpoam(struct snmp_oam *tpoam_buf, uint32_t session_id, uint32_t tpoam_max)
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct oam_session *psess   = NULL;
    uint32_t tpoam_cnt          = 0;
	int cursor                  = 0;
    int val 					= 0;

	OAM_DEBUG();
	if(NULL == tpoam_buf)
	{
		OAM_ERROR("tpoam_buf is NULL\n");
		return ERRNO_FAIL;
	}
	OAM_DEBUG("sess_id = %d; tpoam_max = %d\n", session_id, tpoam_max);

	if (0 == session_id)   /* 初次获取 */
	{
		HASH_BUCKET_LOOP(pbucket, cursor, mplsoam_session_table)
		{
			psess = (struct oam_session *)pbucket->data;
			if (NULL == psess)
			{
				continue;
			}

        	memcpy((tpoam_buf + tpoam_cnt), &(psess->info), sizeof(struct oam_info));
			(tpoam_buf + tpoam_cnt)->alarm_rpt = psess->alarm_rpt;
			(tpoam_buf + tpoam_cnt)->fwd_rmep = psess->fwd_rmep;
			(tpoam_buf + tpoam_cnt)->rev_rmep = psess->rev_rmep;
			(tpoam_buf + tpoam_cnt)->ais_enable = psess->ais_enable;
			(tpoam_buf + tpoam_cnt)->lck_enable = psess->lck_enable;
			(tpoam_buf + tpoam_cnt)->csf_enable = psess->csf_enable;
			if(NULL != psess->plck)
			{
				(tpoam_buf + tpoam_cnt)->lck_level = psess->plck->level;
				(tpoam_buf + tpoam_cnt)->lck_interval = psess->plck->lck_interval;
			}
			if(NULL != psess->pais)
			{
				(tpoam_buf + tpoam_cnt)->ais_level = psess->pais->level;
				(tpoam_buf + tpoam_cnt)->ais_interval = psess->pais->ais_interval;
			}
			if(NULL != psess->pcsf)
			{
				(tpoam_buf + tpoam_cnt)->csf_level = psess->pcsf->level;
				(tpoam_buf + tpoam_cnt)->csf_interval = psess->pcsf->csf_interval;
			}
			if(NULL != psess->fwd_lsp)
			{
				memcpy((tpoam_buf + tpoam_cnt)->fwd_lsp_name, psess->fwd_lsp->name, NAME_STRING_LEN);
			}
			if(NULL != psess->rev_lsp)
			{
				memcpy((tpoam_buf + tpoam_cnt)->rsv_lsp_name, psess->rev_lsp->name, NAME_STRING_LEN);
			}
			if(NULL != psess->sw_pw)
			{
				memcpy((tpoam_buf + tpoam_cnt)->pw_name, psess->sw_pw->name, NAME_STRING_LEN);
			}

			tpoam_cnt++;
        	if (tpoam_cnt == tpoam_max)
        	{
            	return tpoam_cnt;
        	}
		}
	}
	else				/* 找到 session_id后面第一个数据 */
	{
		pbucket = hios_hash_find(&mplsoam_session_table, (void *)session_id);
		if (NULL == pbucket)	/* 未找到 session_id 对应的数据 */
		{
			/* 查找失败返回到当前哈希桶的链表头部 */
			val = mplsoam_session_table.compute_hash((void *)session_id)%HASHTAB_SIZE;

			if (NULL != mplsoam_session_table.buckets[val]) 		/* 当前哈希桶链表不为空 */
			{
				pbucket = mplsoam_session_table.buckets[val];
			}
			else										/* 当前哈希桶链表为空 */
			{
				for (++val; val<HASHTAB_SIZE; ++val)
				{
					if (NULL != mplsoam_session_table.buckets[val])
					{
						pbucket = mplsoam_session_table.buckets[val];
						break;
					}
				}
			}
		}

		if (NULL != pbucket)
		{
			for (tpoam_cnt = 0; tpoam_cnt < tpoam_max; tpoam_cnt++)
			{
				pnext = hios_hash_next_cursor(&mplsoam_session_table, pbucket);
				if ((NULL == pnext) || (NULL == pnext->data))
				{
					break;
				}
				psess = pnext->data;

				memcpy((tpoam_buf + tpoam_cnt), &(psess->info), sizeof(struct oam_info));
				(tpoam_buf + tpoam_cnt)->alarm_rpt = psess->alarm_rpt;
				(tpoam_buf + tpoam_cnt)->fwd_rmep = psess->fwd_rmep;
				(tpoam_buf + tpoam_cnt)->rev_rmep = psess->rev_rmep;
				(tpoam_buf + tpoam_cnt)->ais_enable = psess->ais_enable;
				(tpoam_buf + tpoam_cnt)->lck_enable = psess->lck_enable;
				(tpoam_buf + tpoam_cnt)->csf_enable = psess->csf_enable;
				if(NULL != psess->plck)
				{
					(tpoam_buf + tpoam_cnt)->lck_level = psess->plck->level;
					(tpoam_buf + tpoam_cnt)->lck_interval = psess->plck->lck_interval;
				}
				if(NULL != psess->pais)
				{
					(tpoam_buf + tpoam_cnt)->ais_level = psess->pais->level;
					(tpoam_buf + tpoam_cnt)->ais_interval = psess->pais->ais_interval;
				}
				if(NULL != psess->pcsf)
				{
					(tpoam_buf + tpoam_cnt)->csf_level = psess->pcsf->level;
					(tpoam_buf + tpoam_cnt)->csf_interval = psess->pcsf->csf_interval;
				}
				if(NULL != psess->fwd_lsp)
				{
					memcpy((tpoam_buf + tpoam_cnt)->fwd_lsp_name, psess->fwd_lsp->name, NAME_STRING_LEN);
				}
				if(NULL != psess->rev_lsp)
				{
					memcpy((tpoam_buf + tpoam_cnt)->rsv_lsp_name, psess->rev_lsp->name, NAME_STRING_LEN);
				}
				if(NULL != psess->sw_pw)
				{
					memcpy((tpoam_buf + tpoam_cnt)->pw_name, psess->sw_pw->name, NAME_STRING_LEN);
				}

				pbucket = pnext;
			}
		}
	}

	return tpoam_cnt;
}

static int mpls_get_bulk_oam_meg(struct oam_meg *meg_buf, uint32_t data_len, uchar *pname, uint32_t meg_max)
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct oam_meg *psess   	= NULL;
    uint32_t meg_cnt          	= 0;
	int cursor                  = 0;
    uint32_t val 				= 0;

	OAM_DEBUG();
	if(NULL == meg_buf)
	{
		OAM_ERROR("pname is NULL\n");
        return 0;
	}

	if(0 == data_len)
	{
		HASH_BUCKET_LOOP(pbucket, cursor, mplsoam_meg_table)
		{
			psess = (struct oam_meg *)pbucket->data;
			if (NULL == psess)
			{
				continue;
			}
			memcpy((meg_buf + meg_cnt)->name, psess->name, NAME_STRING_LEN);
			(meg_buf + meg_cnt)->level = psess->level;
			meg_cnt++;
			if (meg_cnt == meg_max)
			{
				return meg_cnt;
			}
		}
	}
	else
	{
        pbucket = hios_hash_find(&mplsoam_meg_table, (void *)pname);
        if (NULL == pbucket)
        {
            val = mplsoam_meg_table.compute_hash((void *)pname)%HASHTAB_SIZE;

            if (NULL != mplsoam_meg_table.buckets[val])
            {
                pbucket = mplsoam_meg_table.buckets[val];
            }
            else
            {
                for (++val; val<HASHTAB_SIZE; ++val)
                {
                    if (NULL != mplsoam_meg_table.buckets[val])
                    {
                        pbucket = mplsoam_meg_table.buckets[val];
                        break;
                    }
                }
            }
        }

		while (pbucket)
        {
            pnext = hios_hash_next_cursor(&mplsoam_meg_table, pbucket);
            if ((NULL == pnext) || (NULL == pnext->data))
            {
                break;
            }
            pbucket = pnext;
            psess = (struct oam_meg *)(pnext->data);

			memcpy((meg_buf + meg_cnt)->name, psess->name, NAME_STRING_LEN);
			(meg_buf + meg_cnt)->level = psess->level;
			meg_cnt++;
            if (meg_cnt == meg_max)
            {
                return meg_cnt;
            }
        }

	}

	return meg_cnt;
}


static int mpls_msg_rcv_get_bulk_oam_meg(struct ipc_msghdr_n  *pmsghdr, void *pdata)
{
	uint32_t meg_max            = IPC_MSG_LEN/sizeof(struct oam_meg);
    struct oam_meg meg_buf[meg_max];
	uint32_t meg_cnt            = 0;
	int ret                     = 0;

	OAM_DEBUG();
	if(NULL == pmsghdr)
	{
		OAM_ERROR("pointer is NULL\n");
		return ERRNO_FAIL;
	}
	uchar *pname = (uchar *)pdata;

	memset (meg_buf, 0, meg_max * sizeof(struct oam_meg));

    meg_cnt = mpls_get_bulk_oam_meg(meg_buf, pmsghdr->data_len, pname, meg_max);
	if (meg_cnt > 0)
	{
		ret = ipc_send_reply_n2(meg_buf, meg_cnt * sizeof(struct oam_meg), meg_cnt, pmsghdr->sender_id,
                             MODULE_ID_MPLS, IPC_TYPE_MPLSOAM, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
	}
	else
	{
		ret = ipc_send_reply_n2(NULL, 0,0, pmsghdr->sender_id, MODULE_ID_MPLS, IPC_TYPE_MPLSOAM,
                             pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
	}

	return ret;
}

static int mpls_msg_rcv_get_bulk_tpoam(struct ipc_msghdr_n  *pmsghdr, void *pdata)
{
	uint32_t tpoam_max            = IPC_MSG_LEN/sizeof(struct snmp_oam);
    struct snmp_oam tpoam_buf[tpoam_max];
	uint32_t tpoam_cnt            = 0;
	int ret                     = 0;

	OAM_DEBUG();
	if(NULL == pmsghdr)
	{
		OAM_ERROR("pointer is NULL\n");
		return ERRNO_FAIL;
	}
	memset (tpoam_buf, 0, tpoam_max * sizeof(struct snmp_oam));

    tpoam_cnt = mpls_get_bulk_tpoam(tpoam_buf, pmsghdr->msg_index, tpoam_max);
	if (tpoam_cnt > 0)
	{
		ret = ipc_send_reply_n2(tpoam_buf, tpoam_cnt * sizeof(struct snmp_oam), tpoam_cnt, pmsghdr->sender_id,
                             MODULE_ID_MPLS, IPC_TYPE_MPLSOAM, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
	}
	else
	{
		ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS, IPC_TYPE_MPLSOAM,
                             pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
	}

	return ret;
}

static int mpls_get_bulk_aps(struct snmp_aps_sess *aps_buf, uint32_t session_id, uint32_t aps_max)
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct aps_sess *psess      = NULL;
    uint32_t aps_cnt            = 0;
	int cursor                  = 0;
    int val 					= 0;
   	char ifname[NAME_STRING_LEN];

	OAM_DEBUG();
	if(NULL == aps_buf)
	{
		OAM_ERROR("pointer is NULL\n");
		return ERRNO_FAIL;
	}

	if (0 == session_id)   /* 初次获取 */
	{
		HASH_BUCKET_LOOP(pbucket, cursor, mplsaps_session_table)
		{
			psess = (struct aps_sess *)pbucket->data;
			if (NULL == psess)
			{
				continue;
			}

        	memcpy((aps_buf + aps_cnt), &(psess->info), sizeof(struct mplsaps_info));
			if(0 != psess->info.master_index)
			{
				if(APS_TYPE_PW == psess->info.type)
				{
					memcpy((aps_buf + aps_cnt)->master_name, pw_get_name(psess->info.master_index), NAME_STRING_LEN);
				}
				else if(APS_TYPE_LSP == psess->info.type)
				{
					memset(ifname, 0, NAME_STRING_LEN);
					ifm_get_name_by_ifindex(psess->info.master_index, ifname);
					memcpy((aps_buf + aps_cnt)->master_name, ifname, NAME_STRING_LEN);
				}
			}
			if(0 != psess->info.backup_index)
			{
				if(APS_TYPE_PW == psess->info.type)
				{
					memcpy((aps_buf + aps_cnt)->backup_name, pw_get_name(psess->info.backup_index), NAME_STRING_LEN);
				}
				else if(APS_TYPE_LSP == psess->info.type)
				{
					memset(ifname, 0, NAME_STRING_LEN);
					ifm_get_name_by_ifindex(psess->info.backup_index, ifname);
					memcpy((aps_buf + aps_cnt)->backup_name, ifname, NAME_STRING_LEN);
				}
			}

			aps_cnt++;
        	if (aps_cnt == aps_max)
        	{
            	return aps_cnt;
        	}
		}
	}
	else				/* 找到 session_id后面第一个数据 */
	{
		pbucket = hios_hash_find(&mplsaps_session_table, (void *)session_id);
		if (NULL == pbucket)	/* 未找到 session_id 对应的数据 */
		{
			/* 查找失败返回到当前哈希桶的链表头部 */
			val = mplsaps_session_table.compute_hash((void *)session_id)%HASHTAB_SIZE;

			if (NULL != mplsaps_session_table.buckets[val]) 		/* 当前哈希桶链表不为空 */
			{
				pbucket = mplsaps_session_table.buckets[val];
			}
			else										/* 当前哈希桶链表为空 */
			{
				for (++val; val<HASHTAB_SIZE; ++val)
				{
					if (NULL != mplsaps_session_table.buckets[val])
					{
						pbucket = mplsaps_session_table.buckets[val];
						break;
					}
				}
			}
		}

		if (NULL != pbucket)
		{
			for (aps_cnt = 0; aps_cnt < aps_max; aps_cnt++)
			{
				pnext = hios_hash_next_cursor(&mplsaps_session_table, pbucket);
				if ((NULL == pnext) || (NULL == pnext->data))
				{
					break;
				}
				psess = pnext->data;

				memcpy((aps_buf + aps_cnt), &(psess->info), sizeof(struct mplsaps_info));
				if(0 != psess->info.master_index)
				{
					if(APS_TYPE_PW == psess->info.type)
					{
						memcpy((aps_buf + aps_cnt)->master_name, pw_get_name(psess->info.master_index), NAME_STRING_LEN);
					}
					else if(APS_TYPE_LSP == psess->info.type)
					{
						memset(ifname, 0, NAME_STRING_LEN);
						ifm_get_name_by_ifindex(psess->info.master_index, ifname);
						memcpy((aps_buf + aps_cnt)->master_name, ifname, NAME_STRING_LEN);
					}
				}
				if(0 != psess->info.backup_index)
				{
					if(APS_TYPE_PW == psess->info.type)
					{
						memcpy((aps_buf + aps_cnt)->backup_name, pw_get_name(psess->info.backup_index), NAME_STRING_LEN);
					}
					else if(APS_TYPE_LSP == psess->info.type)
					{
						memset(ifname, 0, NAME_STRING_LEN);
						ifm_get_name_by_ifindex(psess->info.backup_index, ifname);
						memcpy((aps_buf + aps_cnt)->backup_name, ifname, NAME_STRING_LEN);
					}
				}
				pbucket = pnext;

			}
		}
	}

	return aps_cnt;
}

static int mpls_msg_rcv_get_bulk_aps(struct ipc_msghdr_n  *pmsghdr, void *pdata)
{
	uint32_t aps_max            = IPC_MSG_LEN/sizeof(struct snmp_aps_sess);
    struct snmp_aps_sess aps_buf[aps_max];
	uint32_t aps_cnt            = 0;
	int ret                     = 0;

	OAM_DEBUG();
	if(NULL == pmsghdr)
	{
		OAM_ERROR("pointer is NULL\n");
		return ERRNO_FAIL;
	}

	memset(aps_buf, 0, aps_max * sizeof(struct snmp_aps_sess));
    aps_cnt = mpls_get_bulk_aps(aps_buf, pmsghdr->msg_index, aps_max);
	if (aps_cnt > 0)
	{
		ret = ipc_send_reply_n2(aps_buf, aps_cnt * sizeof(struct snmp_aps_sess), aps_cnt, pmsghdr->sender_id,
                             MODULE_ID_MPLS, IPC_TYPE_MPLSAPS, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
	}
	else
	{
		ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS, IPC_TYPE_MPLSAPS,
                             pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
	}

	return ret;
}

static void mpls_msg_rcv_get_tpoam(struct ipc_msghdr_n  *pmsghdr, void *pdata)
{
    char ifname[NAME_STRING_LEN];
    uint32_t session_id = 0;
    int  ret = 0;

	OAM_DEBUG();
    if(NULL == pmsghdr)
    {
		OAM_ERROR("pointer is NULL\n");
        return ;
    }

    if(NULL == pdata)
    {
        ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS,
                        IPC_TYPE_MPLSOAM, pmsghdr->msg_subtype, 1, 0,IPC_OPCODE_NACK);
    }

    if (OAM_SUBTYPE_IFNAME == pmsghdr->msg_subtype)
    {
        session_id = *(uint32_t *)pdata;
        memset (ifname, 0, NAME_STRING_LEN);
        ret = mplsoam_session_ifname(session_id, ifname);
		OAM_DEBUG("local_id=%d,	ifname[%s] \n", session_id, ifname);
        if (0 == ret)
        {
            ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS,
                        IPC_TYPE_MPLSOAM, pmsghdr->msg_subtype, 1, 0,IPC_OPCODE_NACK);
        }
        else
        {
            ret = ipc_send_reply_n2(ifname, NAME_STRING_LEN, 1, pmsghdr->sender_id, MODULE_ID_MPLS,
                        IPC_TYPE_MPLSOAM, pmsghdr->msg_subtype, 1, pmsghdr->msg_index,IPC_OPCODE_REPLY);
        }
    }

    return;
}

/* 接收 tpoam 消息 */
static void mpls_msg_rcv_tpoam(struct ipc_msghdr_n  *pmsghdr, void *pdata)
{
    struct oam_session *psess      = NULL;
    uint16_t            session_id = 0;

	OAM_DEBUG();
    if((pmsghdr == NULL) || (pdata == NULL))
    {
		OAM_ERROR("pointer is NULL\n");
        return ;
    }

    session_id = pmsghdr->msg_index;
    psess = mplsoam_session_lookup(session_id);
    if((NULL == psess) || (psess->info.type == OAM_TYPE_INVALID))
    {
    	OAM_ERROR("pointer is NULL!\n");
        return ;
    }

	switch (pmsghdr->opcode)
	{
		case IPC_OPCODE_UP:       /* session up */
		{
			OAM_DEBUG("rcv up.session:%d.\n", session_id);
			mplsoam_session_up(session_id);
            break;
        }
        case IPC_OPCODE_DOWN:     /* session down */
        {
			OAM_DEBUG("rcv down.session:%d.\n", session_id);
			mplsoam_session_down(session_id);
            break;
		}
		case IPC_OPCODE_ENABLE:   /* 收到 性能测试结果 */
		{
            if(pmsghdr->msg_subtype == OAM_SUBTYPE_LM)
            {
				OAM_DEBUG("rcv lm.session:%d.\n", session_id);
			    mplsoam_session_set_lm(session_id, pdata);
            }
            else if(pmsghdr->msg_subtype == OAM_SUBTYPE_DM)
            {
				OAM_DEBUG("rcv dm.session:%d.\n", session_id);
                mplsoam_session_set_dm(session_id, pdata);
            }
			break;
		}
		case IPC_OPCODE_ADD:      /* add alarm */
		{
			OAM_DEBUG("rcv add alarm:%d.\n", pmsghdr->msg_subtype);
            mplsoam_session_alarm_process(session_id, IPC_OPCODE_ADD, pmsghdr->msg_subtype);
			break;
		}
        case IPC_OPCODE_CLEAR:    /* clear alarm */
        {
        	OAM_DEBUG("rcv clear alarm:%d.\n", pmsghdr->msg_subtype);
            mplsoam_session_alarm_process(session_id, IPC_OPCODE_CLEAR, pmsghdr->msg_subtype);
			break;
        }

		default:
			break;
	}

	return;
}


/* 接收报文 */
static int mpls_msg_rcv_pkt(struct ipc_mesg_n *pmesg)
{
    struct pkt_buffer *ppkt = pkt_rcv_n(pmesg);
    struct ip_control *ipcb;
    struct mpls_control *mplscb;
    struct eth_control *ethcb;
	uint8_t oam_opcode = 0;

    switch (ppkt->cb_type)
    {
        case PKT_TYPE_IP:
        {
            ipcb = (struct ip_control *)&(ppkt->cb);
            break;
        }
        case PKT_TYPE_UDP:
        {
            ipcb = (struct ip_control *)&(ppkt->cb);
            if(ipcb->dport == UDP_PORT_BFD)
                bfd_recv(ppkt);
            else if((ipcb->dport == UDP_PORT_LSPING_REQ) || (ipcb->dport == UDP_PORT_LSPING_REPLY))
                mpls_ping_recv(ppkt);
            else if(ipcb->dport == TCP_PORT_LDP)
            {
                ldp_recv_pkt(ppkt, LDP_PKT_TYPE_UDP);
            }
            break;
        }
        case PKT_TYPE_TCP:
        {
            ipcb = (struct ip_control *)&(ppkt->cb);
            if((ipcb->dport == TCP_PORT_LDP) || (ipcb->sport == TCP_PORT_LDP))
            {
                ldp_recv_pkt(ppkt, LDP_PKT_TYPE_TCP);
            }
            break;
        }
        case PKT_TYPE_MPLS:
        {
            mplscb = (struct mpls_control *)&(ppkt->cb);
    
            OAM_DEBUG("mplscb->chtype=%d\n", mplscb->chtype);
            if((UDP_PORT_BFD == mplscb->dport) || (MPLS_CTLWORD_WITHOUT_IP == mplscb->chtype))
            {
                bfd_recv(ppkt);
            }
            else if((mplscb->dport == UDP_PORT_LSPING_REQ) || (mplscb->dport == UDP_PORT_LSPING_REPLY))
            {
                zlog_info("%s[%s]:MPLS PING RCV:--line:%d.",
                    __FILE__, __FUNCTION__, __LINE__);
                mpls_ping_recv(ppkt);
            }
            else if(mplscb->chtype == MPLS_CTLWORD_CHTYPE_8902 || mplscb->chtype == MPLS_CTLWORD_CHTYPE_7FFA)
            {
                oam_opcode = *((uint8_t *)ppkt->data + 1);
    
                OAM_DEBUG("oam_opcode=%d\n", oam_opcode);
                if((oam_opcode == CFM_OPCODE_LBM) || (oam_opcode == CFM_OPCODE_LBR))
                {
                    OAM_DEBUG("rcv lb:opcode = %d.\n", oam_opcode);
                    mplsoam_recv_lb(ppkt);
                }
                else if(oam_opcode == CFM_OPCODE_APS)
                {
                    OAM_DEBUG("aps receive mpls packet pkt: %s", pkt_dump(ppkt->data, ppkt->data_len));
                    mpls_aps_pkt_recv(ppkt);
                }
                else if(oam_opcode == CFM_OPCODE_AIS)
                {
                    OAM_DEBUG("rcv ais:opcode = %d.\n", oam_opcode);
                    mplsoam_ais_rcv_pkt(ppkt);
                }
                else if(oam_opcode == CFM_OPCODE_LCK)
                {
                    OAM_DEBUG("rcv lck:opcode = %d.\n", oam_opcode);
                    mplsoam_lck_rcv_pkt(ppkt);
                }
            }
            else if((mplscb->dport == TCP_PORT_LDP) || (mplscb->sport == TCP_PORT_LDP))
            {
                ldp_recv_pkt(ppkt, LDP_PKT_TYPE_TCP);
            }
            break;
        }
        case PKT_TYPE_ETH:
        {
            ethcb = (struct eth_control *)&(ppkt->cb);
            if(ethcb->ethtype == ETH_P_CFM) /* cfm */
            {
    
            }
            break;
        }
        default:
            break;
    }
    return ERRNO_SUCCESS;
}



/**
 * @brief      : 从控制信息链表中取数据执�?
 * @param[in ] : pmesg - ipc 消息结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : 
 * @date       : 2018�?3�?8�? 15:33:18
 * @note       :
 */
int mpls_ipc_msg_process(struct ipc_mesg_n *pmesg, int imlen)
{
    struct ipc_msghdr_n *pmsghdr   = NULL;
    void *pdata = NULL;
    int ret   = 0;
    struct route_entry *proute    = NULL;
    struct l3vpn_route *pvroute   = NULL;
    int i;
    int revln = 0;
    if(NULL == pmesg)
    {
        return 0;
    }

    revln = (int)pmesg->msghdr.data_len + IPC_HEADER_LEN_N; 

    if(revln <= imlen)
    {
        pmsghdr = &(pmesg->msghdr);
        pdata   = pmesg->msg_data;

        MPLS_LOG_DEBUG("msg_type = %d, opcode = %d, subtype = %d\n", pmsghdr->msg_type, pmsghdr->opcode, pmsghdr->msg_subtype);

        if(IPC_TYPE_DEVM == pmsghdr->msg_type && pmsghdr->opcode == IPC_OPCODE_EVENT)
        {
            if(DEV_EVENT_HA_BOOTSTATE_CHANGE == pmsghdr->msg_subtype)
            {
                mpls_brard_status_change_process((struct devm_com *)pdata);
            }
        }
        else if (pmsghdr->msg_type == IPC_TYPE_MPLS)
        {
            if (pmsghdr->opcode == IPC_OPCODE_GET_BULK)
            {
                if(pmsghdr->msg_subtype == MPLS_GET_LABEL_POOL_BULK)
                {
                    char buf[IPC_MSG_LEN];
                    memset(buf, 0, IPC_MSG_LEN);
                    ret = label_table_get_bulk((struct label_info *)pdata, pmsghdr->msg_index, (struct label_info *)buf);
                    if (ret > 0)
                    {
                        /* 因为返回数据结构中使用了零长数组，数据长度直接填IPC的最大长度 */
                        ret = ipc_send_reply_n2(buf, ret * sizeof(struct label_info), ret, pmsghdr->sender_id,
                                                MODULE_ID_MPLS, IPC_TYPE_MPLS, pmsghdr->msg_subtype,1,  pmsghdr->msg_index, IPC_OPCODE_REPLY);
                    }
                    else
                    {
                        ret = ipc_send_reply_n2(NULL, 0, 0,pmsghdr->sender_id, MODULE_ID_MPLS, IPC_TYPE_MPLS,
                                                pmsghdr->msg_subtype, 1, pmsghdr->msg_index,IPC_OPCODE_NACK);
                    }
                }
            }
        }
        else if(pmsghdr->msg_type == IPC_TYPE_PACKET)
        {
            mpls_msg_rcv_pkt(pmesg);
        }
        else if (pmsghdr->msg_type == IPC_TYPE_ROUTE)
        {
            proute = (struct route_entry *)pdata;
        
            for (i=0; i<pmsghdr->data_num; i++)
            {
                ldp_respond_route_event(proute, pmsghdr->opcode);
        
                proute += 1;
            }
        }
        else if (pmsghdr->msg_type == IPC_TYPE_IFM)
        {
            mpls_ctr_msg_ifevent_process(pmsghdr, pdata);
        }
        else if(pmsghdr->msg_type == IPC_TYPE_LDP)
        {
            if (pmsghdr->opcode == IPC_OPCODE_GET_BULK)
            {
                if(pmsghdr->msg_subtype == MPLS_GET_LDP_PREFIX_BULK)
                {
                    int msg_len = IPC_MSG_LEN/sizeof(struct ldp_ipprefix_mib);
		            struct ldp_ipprefix_mib ldprefix_buf[msg_len];
		            uint32_t index = pmsghdr->msg_index;

		            memset(ldprefix_buf, 0, msg_len*sizeof(struct ldp_ipprefix_mib));

		            ret = mpls_ldp_get_prefix(index, (uchar *)(&pmsghdr->msg_index), pmsghdr->data_len, (char *)ldprefix_buf);
		            if (ret > 0)
		            {
		                ret = ipc_send_reply_n2(ldprefix_buf, ret*sizeof(struct ldp_ipprefix_mib), ret, pmsghdr->sender_id,
		                                        MODULE_ID_MPLS, IPC_TYPE_LDP, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
		            }
		            else
		            {
		                ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS, IPC_TYPE_LDP,
		                                        pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
		            }
                }
                else if(pmsghdr->msg_subtype == MPLS_GET_LDP_LOCALIF_BULK)
                {
                    int msg_len = IPC_MSG_LEN/sizeof(struct ldp_localif_mib);
		            struct ldp_localif_mib ldpif_buf[msg_len];
		            uint32_t index = pmsghdr->msg_index;

		            memset(ldpif_buf, 0, msg_len*sizeof(struct ldp_localif_mib));

		            ret = mpls_ldp_get_localif(index, pmsghdr->data_len, (char *)ldpif_buf);
		            if (ret > 0)
		            {
		                ret = ipc_send_reply_n2(ldpif_buf, ret*sizeof(struct ldp_localif_mib), ret, pmsghdr->sender_id,
		                                        MODULE_ID_MPLS, IPC_TYPE_LDP, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
		            }
		            else
		            {
		                ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS, IPC_TYPE_LDP,
		                                        pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
		            }
                }
                else if(pmsghdr->msg_subtype == MPLS_GET_LDP_REMOT_BULK)
                {
                    int msg_len = IPC_MSG_LEN/sizeof(struct ldp_remoteconf_mib);
		            struct ldp_remoteconf_mib remot_buf[msg_len];
		            uint32_t index = pmsghdr->msg_index;

		            memset(remot_buf, 0, msg_len*sizeof(struct ldp_remoteconf_mib));

		            ret = mpls_get_remot_conf(index, pmsghdr->data_len, (char *)remot_buf);
		            if (ret > 0)
		            {
		                ret = ipc_send_reply_n2(remot_buf, ret*sizeof(struct ldp_remoteconf_mib), ret, pmsghdr->sender_id,
		                                        MODULE_ID_MPLS, IPC_TYPE_LDP, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
		            }
		            else
		            {
		                ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS, IPC_TYPE_LDP,
		                                        pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
		            }
                }
                else if(pmsghdr->msg_subtype == MPLS_GET_LDP_CONF_GLOBAL)
                {
                    int msg_len = IPC_MSG_LEN/sizeof(struct ldp_config_global_mib);
		            struct ldp_config_global_mib conf_buf[msg_len];
		            uint32_t index = pmsghdr->msg_index;

		            memset(conf_buf, 0, msg_len*sizeof(struct ldp_config_global_mib));

		            ret = mpls_ldp_get_global_conf(index, pmsghdr->data_len, (char *)conf_buf);
		            if (ret > 0)
		            {
		                ret = ipc_send_reply_n2(conf_buf, ret*sizeof(struct ldp_config_global_mib), ret, pmsghdr->sender_id,
		                                        MODULE_ID_MPLS, IPC_TYPE_LDP, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
		            }
		            else
		            {
		                ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS, IPC_TYPE_LDP,
		                                        pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
		            }

                }
                else if(pmsghdr->msg_subtype == MPLS_GET_LDP_SESS_BULK)
                {
                    int msg_len = IPC_MSG_LEN/sizeof(struct ldp_session_mib);
		            struct ldp_session_mib sess_buf[msg_len];
		            uint32_t index = pmsghdr->msg_index;

		            memset(sess_buf, 0, msg_len*sizeof(struct ldp_session_mib));

		            ret = mpls_ldp_get_sess(index, pmsghdr->data_len, (char *)sess_buf);
		            if (ret > 0)
		            {
		                ret = ipc_send_reply_n2(sess_buf, ret*sizeof(struct ldp_session_mib), ret, pmsghdr->sender_id,
		                                        MODULE_ID_MPLS, IPC_TYPE_LDP, pmsghdr->msg_subtype,1,  pmsghdr->msg_index, IPC_OPCODE_REPLY);
		            }
		            else
		            {
		                ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS, IPC_TYPE_LDP,
		                                        pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
		            }

                }
                else if(pmsghdr->msg_subtype == MPLS_GET_LDP_LSP_BULK)
                {
                    int msg_len = IPC_MSG_LEN/sizeof(struct ldp_lsp_mib);
		            struct ldp_lsp_mib lsp_buf[msg_len];
		            uint32_t index =pmsghdr->msg_index;
		            memset(lsp_buf, 0, msg_len*sizeof(struct ldp_lsp_mib));

		            ret = mpls_ldp_get_lsp(index, pmsghdr->data_len,(char *)lsp_buf);
		            if (ret > 0)
		            {
		                ret = ipc_send_reply_n2(lsp_buf, ret*sizeof(struct ldp_lsp_mib), ret, pmsghdr->sender_id,
		                                        MODULE_ID_MPLS, IPC_TYPE_LDP, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
		            }
		            else
		            {
		                ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS, IPC_TYPE_LDP,
		                                        pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
		            }
                }
            }

        }
        else if (pmsghdr->msg_type == IPC_TYPE_BFD) /* bfd 消息 */
		{
			if (pmsghdr->opcode == IPC_OPCODE_EVENT)
			{
				mpls_msg_rcv_bfd(pmsghdr, pdata);
			}
            else if (pmsghdr->opcode == IPC_OPCODE_GET)
			{
				ret = mpls_msg_rcv_get_bfd(pmsghdr, pdata);
			}
            else if (pmsghdr->opcode == IPC_OPCODE_GET_BULK)
			{
				ret = mpls_msg_rcv_get_bulk_bfd(pmsghdr, pdata);
			}
			else if (pmsghdr->opcode == IPC_OPCODE_ADD)
			{
				ret = mpls_msg_rcv_add_bfd(pmsghdr, pdata);
			}
			else if (pmsghdr->opcode == IPC_OPCODE_UPDATE)
			{
				ret = mpls_msg_recv_update_bfd(pmsghdr, pdata);
			}
			else if (pmsghdr->opcode == IPC_OPCODE_DELETE)
			{
				ret = mpls_msg_rcv_del_bfd(pmsghdr, pdata);
			}
		}
        else if (pmsghdr->msg_type == IPC_TYPE_MPLSOAM) /* mpls-tp oam 消息 */
        {
            if(pmsghdr->opcode == IPC_OPCODE_GET_BULK)
            {
            	if(OAM_SUBTYPE_MEG == pmsghdr->msg_subtype)
            	{
					mpls_msg_rcv_get_bulk_oam_meg(pmsghdr, pdata);
				}
                else
				{
					/* 批量获取*/
					mpls_msg_rcv_get_bulk_tpoam(pmsghdr, pdata);
				}
            }
            else if(pmsghdr->opcode == IPC_OPCODE_GET)
            {
                /* 获取oam绑定的业务的名称或单个session�?*/
                mpls_msg_rcv_get_tpoam(pmsghdr, pdata);
            }
            else
            {
			    mpls_msg_rcv_tpoam(pmsghdr, pdata);
            }
        }
		else if (pmsghdr->msg_type == IPC_TYPE_MPLSAPS)
		{
			if(IPC_OPCODE_GET_BULK == pmsghdr->opcode)
			{
				mpls_msg_rcv_get_bulk_aps(pmsghdr, pdata);
			}
		}
        else if (pmsghdr->msg_type == IPC_TYPE_LSP)
        {
            if (pmsghdr->opcode == IPC_OPCODE_GET)
            {
                ret = ipc_send_reply_n2(&gmpls.lsr_id, 4, 1, pmsghdr->sender_id, MODULE_ID_MPLS,
                                        IPC_TYPE_LSP, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
            }
            else if (pmsghdr->opcode == IPC_OPCODE_GET_BULK)
            {
                int msg_len = IPC_MSG_LEN/sizeof(struct static_lsp);
                struct static_lsp slsp_buff[msg_len];
                uchar *pname = (uchar *)pdata;

                memset(slsp_buff, 0, msg_len*sizeof(struct static_lsp));

                ret = static_lsp_get_bulk(pname, pmsghdr->data_len, slsp_buff);
                if (ret > 0)
                {
                    ret = ipc_send_reply_n2(slsp_buff, ret*sizeof(struct static_lsp), ret, pmsghdr->sender_id,
                                            MODULE_ID_MPLS, IPC_TYPE_LSP, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
                }
                else
                {
                    ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS, IPC_TYPE_LSP,
                                            pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
                }
            }
        }
        else if (pmsghdr->msg_type == IPC_TYPE_PW)
        {
            if (pmsghdr->opcode == IPC_OPCODE_GET)
            {
                struct counter_t *pcounter = NULL;
                uchar *pname = (uchar *)pdata;

                pcounter = l2vc_get_counter(pname);
                if (NULL == pcounter)
                {
                    ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS,
                                pmsghdr->msg_type, pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
                }
                else
                {
                    ipc_send_reply_n2(pcounter, sizeof(struct counter_t), 1, pmsghdr->sender_id,
                                MODULE_ID_MPLS, pmsghdr->msg_type, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
                    mem_share_free_bydata(pcounter, MODULE_ID_MPLS);
                }
            }
            else if (pmsghdr->opcode == IPC_OPCODE_GET_BULK)
            {
                if (MPLS_GET_L2VC_BULK == pmsghdr->msg_subtype)
                {
                    int msg_len = IPC_MSG_LEN/sizeof(struct l2vc_entry);
                    struct l2vc_entry l2vc_buff[msg_len];
                    uchar *pname = (uchar *)pdata;

                    memset(l2vc_buff, 0, msg_len*sizeof(struct l2vc_entry));

		            ret = l2vc_get_bulk(pname, pmsghdr->data_len, l2vc_buff);
		            if (ret > 0)
		            {
		                ret = ipc_send_reply_n2(l2vc_buff, ret*sizeof(struct l2vc_entry), ret, pmsghdr->sender_id,
		                                        MODULE_ID_MPLS, IPC_TYPE_PW, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
		            }
		            else
		            {
		                ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS, IPC_TYPE_PW,
		                                        pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
		            }
            	}
            	else if (MPLS_GET_PW_HQOS_BULK == pmsghdr->msg_subtype)
            	{
            		int msg_len = IPC_MSG_LEN/sizeof(struct hqos_pw);
            		struct hqos_pw hqos_array[msg_len];
            		uchar *pname = (uchar *)pdata;

            		memset(hqos_array, 0, msg_len * sizeof(struct hqos_pw));

            		ret = pw_get_hqos_bulk(pname, pmsghdr->data_len, hqos_array);
            		if(ret > 0)
            		{
            			ret = ipc_send_reply_n2(hqos_array, ret*sizeof(struct hqos_pw), ret, pmsghdr->sender_id,
            									MODULE_ID_MPLS, IPC_TYPE_PW, pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_REPLY);
            		}
            		else
            		{
            			ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS,
            									IPC_TYPE_PW, pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
            		}
            	}
                else if(MPLS_GET_PW_CLASS_BULK == pmsghdr->msg_subtype)
                {
                    char buf[IPC_MSG_LEN];
                    memset(buf, 0, IPC_MSG_LEN);
                    ret = l2vpn_pw_class_get_bulk((struct pw_class *)pdata, pmsghdr->msg_index, (struct pw_class *)buf);
                    if (ret > 0)
                    {
                        ret = ipc_send_reply_n2(buf, ret*sizeof(struct pw_class), ret, pmsghdr->sender_id,
                                                MODULE_ID_MPLS, IPC_TYPE_PW, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
                    }
                    else
                    {
                        ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS, IPC_TYPE_PW,
                                                pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
                    }

                }
                else if(MPLS_GET_SERV_INS_BULK == pmsghdr->msg_subtype)
                {
                    char buf[IPC_MSG_LEN];
                    memset(buf, 0, IPC_MSG_LEN);
                    ret = l2vpn_service_instance_get_bulk((struct serv_ins_mib *)pdata, pmsghdr->msg_index, (struct serv_ins_mib *)buf);
                    if (ret > 0)
                    {
                        ret = ipc_send_reply_n2(buf, ret*sizeof(struct serv_ins_mib), ret, pmsghdr->sender_id,
                                                MODULE_ID_MPLS, IPC_TYPE_PW, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
                    }
                    else
                    {
                        ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS, IPC_TYPE_PW,
                                                pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
                    }

                }
                else if(MPLS_GET_XC_GROUP_BULK == pmsghdr->msg_subtype)
                {
                    char buf[IPC_MSG_LEN];
                    memset(buf, 0, IPC_MSG_LEN);
                    ret = l2vpn_xc_group_get_bulk((struct xc_group_mib *)pdata, pmsghdr->msg_index, (struct xc_group_mib *)buf);
                    if (ret > 0)
                    {
                        /* 因为返回数据结构中使用了零长数组，数据长度直接填IPC的最大长�? */
                        ret = ipc_send_reply_n2(buf, IPC_MSG_LEN, ret, pmsghdr->sender_id,
                                                MODULE_ID_MPLS, IPC_TYPE_PW, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
                    }
                    else
                    {
                        ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS, IPC_TYPE_PW,
                                                pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
                    }

                }

                else if(MPLS_GET_MPLS_SERV_BULK == pmsghdr->msg_subtype)
                {
                    char buf[IPC_MSG_LEN];
                    memset(buf, 0, IPC_MSG_LEN);
                    ret = mpls_service_get_bulk((struct mpls_service *)pdata, pmsghdr->msg_index, (struct mpls_service *)buf);
                    if (ret > 0)
                    {
                        /* 因为返回数据结构中使用了零长数组，数据长度直接填IPC的最大长�? */
                        ret = ipc_send_reply_n2(buf, ret * sizeof(struct mpls_service), ret, pmsghdr->sender_id,
                                                MODULE_ID_MPLS, IPC_TYPE_PW, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
                    }
                    else
                    {
                        ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS, IPC_TYPE_PW,
                                                pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
                    }
                }
            }
        }
        else if (pmsghdr->msg_type == IPC_TYPE_VSI)
        {
            if (pmsghdr->opcode == IPC_OPCODE_GET_BULK)
            {
                if (MPLS_GET_VSI_BULK == pmsghdr->msg_subtype)
                {
                    int msg_len = IPC_MSG_LEN/sizeof(struct vsi_entry);
                    struct vsi_entry vsi_buff[msg_len];
                    uint32_t vsi_id = *((uint32_t *)pdata);

                    memset(vsi_buff, 0, msg_len*sizeof(struct vsi_entry));

                    ret = vsi_get_bulk(vsi_id, vsi_buff);
                    if (ret > 0)
                    {
                        ret = ipc_send_reply_n2(vsi_buff, ret*sizeof(struct vsi_entry), ret,
                                                pmsghdr->sender_id, MODULE_ID_MPLS,IPC_TYPE_VSI,
                                                pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
                    }
                    else
                    {
                        ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS,
                                                IPC_TYPE_VSI, pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
                    }
                }
                else if (MPLS_GET_VSIIF_BULK == pmsghdr->msg_subtype)
                {
                    int msg_len = IPC_MSG_LEN/sizeof(uint32_t);
                    uint32_t if_buff[msg_len];
                    uint32_t vsi_id = *((uint32_t *)pdata);

                    memset(if_buff, 0, msg_len*sizeof(uint32_t));

                    ret = vsi_get_vsiif_bulk(vsi_id, pmsghdr->msg_index, if_buff);
                    if (ret > 0)
                    {
                        ret = ipc_send_reply_n2(if_buff, ret*sizeof(uint32_t), ret,
                                                pmsghdr->sender_id, MODULE_ID_MPLS,IPC_TYPE_VSI,
                                                pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
                    }
                    else
                    {
                        ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS,
                                                IPC_TYPE_VSI, pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
                    }
                }
                else if (MPLS_GET_VSI_BLACKMAC_BULK == pmsghdr->msg_subtype)
                 {
                     uint8_t mac_buff[IPC_MSG_LEN];
                     memset(mac_buff, 0, IPC_MSG_LEN);
                
                     ret = vsi_get_vsi_black_white_mac_bulk(pdata, pmsghdr->data_len, (struct mpls_mac*)mac_buff);
                     if (ret > 0)
                     {
                         ret = ipc_send_reply_n2(mac_buff, ret*sizeof(struct mpls_mac), ret,
                                                 pmsghdr->sender_id, MODULE_ID_MPLS,IPC_TYPE_VSI,
                                                 pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
                     }
                     else
                     {
                         ret = ipc_send_reply_n2(NULL,0, 0, pmsghdr->sender_id, MODULE_ID_MPLS,
                                                 IPC_TYPE_VSI, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_NACK);
                     }
                 }

            }
            else if(pmsghdr->msg_subtype == MPLS_GET_VSI_STATIC_MAC_BULK)
            {
                uint8_t mac_buff[IPC_MSG_LEN];
                memset(mac_buff, 0, IPC_MSG_LEN);
                
                ret = vsi_get_smac_bulk(pdata, pmsghdr->data_len, (struct mpls_smac*)mac_buff);
                if (ret > 0)
                {
                    ret = ipc_send_reply_n2(mac_buff, ret*sizeof(struct mpls_smac), ret,
                                            pmsghdr->sender_id, MODULE_ID_MPLS,IPC_TYPE_VSI,
                                            pmsghdr->msg_subtype,1,  pmsghdr->msg_index, IPC_OPCODE_REPLY);
                }
                else
                {
                    ret = ipc_send_reply_n2(NULL,0, 0, pmsghdr->sender_id, MODULE_ID_MPLS,
                                            IPC_TYPE_VSI, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_NACK);
                }
            }

        }
        else if (pmsghdr->msg_type == IPC_TYPE_TUNNEL)
        {
            if (pmsghdr->opcode == IPC_OPCODE_GET)
            {
                if (MPLS_GET_TUNNEL_COUNTER == pmsghdr->msg_subtype)
                {
                    struct counter_t *pcounter = NULL;
                    uint32_t ifindex = pmsghdr->msg_index;

                    pcounter = tunnel_if_get_counter(ifindex);
                    if (NULL == pcounter)
                    {
                        ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS,
                                    pmsghdr->msg_type, pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
                    }
                    else
                    {
                        ipc_send_reply_n2(pcounter, sizeof(struct counter_t), 1, pmsghdr->sender_id,
                                    MODULE_ID_MPLS, pmsghdr->msg_type, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
                        mem_share_free_bydata(pcounter, MODULE_ID_MPLS);
                    }
                }
                else if (MPLS_GET_TUNNEL == pmsghdr->msg_subtype)
                {
                    struct tunnel_if *pif = NULL;
                    uint32_t ifindex = pmsghdr->msg_index;

                    pif = tunnel_if_lookup(ifindex);
                    if (NULL == pif)
                    {
                        ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS,
                                    pmsghdr->msg_type, pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
                    }
                    else
                    {
                        ipc_send_reply_n2(pif, sizeof(struct tunnel_if), 1, pmsghdr->sender_id,
                                    MODULE_ID_MPLS, pmsghdr->msg_type, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
                    }
                }
            }
            else if (pmsghdr->opcode == IPC_OPCODE_GET_BULK)
            {
                if (MPLS_GET_TUNNEL_BULK == pmsghdr->msg_subtype)
                {
                    int msg_len = IPC_MSG_LEN/sizeof(struct tunnel_if);
                    struct tunnel_if tunnel_buff[msg_len];
                    uint32_t ifindex = pmsghdr->msg_index;

                    memset(tunnel_buff, 0, msg_len*sizeof(struct tunnel_if));

	                ret = tunnel_if_get_bulk(ifindex, tunnel_buff);
	                if (ret > 0)
	                {
	                    ret = ipc_send_reply_n2(tunnel_buff, ret*sizeof(struct tunnel_if), ret, pmsghdr->sender_id,
	                                            MODULE_ID_MPLS, IPC_TYPE_TUNNEL, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
	                }
	                else
	                {
	                    ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS, IPC_TYPE_TUNNEL,
	                                            pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
	                }
            	}
            	else if (MPLS_GET_TUNNEL_HQOS_BULK == pmsghdr->msg_subtype)
            	{
            		int msg_len = IPC_MSG_LEN/sizeof(struct hqos_tunnel);
	                struct hqos_tunnel hqos_array[msg_len];
	                uint32_t ifindex = pmsghdr->msg_index;

	                memset(hqos_array, 0, msg_len*sizeof(struct hqos_tunnel));

	                ret = tunnel_if_get_hqos_bulk(ifindex, hqos_array);
	                if (ret > 0)
	                {
	                    ret = ipc_send_reply_n2(hqos_array, ret*sizeof(struct hqos_tunnel), ret, pmsghdr->sender_id,
	                                            MODULE_ID_MPLS, IPC_TYPE_TUNNEL, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
	                }
	                else
	                {
	                    ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS, IPC_TYPE_TUNNEL,
	                                            pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
	                }
            	}
            }
        }
        else if (pmsghdr->msg_type == IPC_TYPE_L3VPN)
        {
            if (pmsghdr->opcode == IPC_OPCODE_REGISTER)
            {
                l3vpn_register_msg(pmsghdr->sender_id);
            }
            else if (pmsghdr->opcode == IPC_OPCODE_DELETE)
            {
                if (pmsghdr->msg_subtype == L3VPN_SUBTYPE_LABEL)
                {
                    struct l3vpn_entry *pl3vpn = NULL;
                    uint32_t label  = *(uint32_t *)pdata;
                    uint32_t vrf_id = pmsghdr->msg_index;

                    pl3vpn = mpls_l3vpn_lookup((uint16_t)vrf_id);
                    if ((NULL != pl3vpn) && (LABEL_APPLY_PER_VPN == pl3vpn->label_apply))
                    {
                        mpls_l3vpn_delete_lsp(pl3vpn);
                    }

                    label_free(label);
                }
            }
            else if (pmsghdr->opcode == IPC_OPCODE_GET)
            {
                if (pmsghdr->msg_subtype == L3VPN_SUBTYPE_LABEL)
                {
                    int ret = 0;

                    ret = mpls_l3vpn_route_get_label(pmsghdr->msg_index);
                    if (ret > 0)
                    {
                        ipc_send_reply_n2(&ret, sizeof(uint32_t), 1, pmsghdr->sender_id, MODULE_ID_MPLS,
                                    IPC_TYPE_L3VPN, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
                    }
                    else
                    {
                        ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS,
                                    IPC_TYPE_L3VPN, pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_REPLY);
                    }
                }
                else if (pmsghdr->msg_subtype == L3VPN_SUBTYPE_INSTANCE)
                {
                    struct l3vpn_entry *pl3vpn = NULL;
                    uint16_t vrf_id = *((uint16_t *)pdata);

                    pl3vpn = mpls_l3vpn_lookup(vrf_id);
                    if (NULL != pl3vpn)
                    {
                        ipc_send_reply_n2(pl3vpn, sizeof(struct l3vpn_entry), 1, pmsghdr->sender_id, MODULE_ID_MPLS,
                                    IPC_TYPE_L3VPN, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
                    }
                    else
                    {
                        ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS,
                                    IPC_TYPE_L3VPN, pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
                    }
                }
            }
            else if (pmsghdr->opcode == IPC_OPCODE_GET_BULK)
            {
                if (pmsghdr->msg_subtype == L3VPN_SUBTYPE_INSTANCE)
                {
                    int msg_len = IPC_MSG_LEN/sizeof(struct l3vpn_entry);
	                struct l3vpn_entry l3vpn_entry[msg_len];
	                uint32_t vrf_id = *((uint32_t *)pdata);

	                memset(l3vpn_entry, 0, msg_len*sizeof(struct l3vpn_entry));

	                ret = mpls_l3vpn_get_instance_bulk(vrf_id, l3vpn_entry);
	                if (ret > 0)
	                {
	                    ret = ipc_send_reply_n2(l3vpn_entry, ret*sizeof(struct l3vpn_entry), ret, pmsghdr->sender_id,
	                                            MODULE_ID_MPLS, IPC_TYPE_L3VPN, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
	                }
	                else
	                {
	                    ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS, IPC_TYPE_L3VPN,
	                                            pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
	                }
                }
                else if ((pmsghdr->msg_subtype == L3VPN_SUBTYPE_IMPORT)
                    || (pmsghdr->msg_subtype == L3VPN_SUBTYPE_EXPORT))
                {
                    int msg_len = IPC_MSG_LEN/sizeof(struct l3vpn_target);
                    struct l3vpn_target target[msg_len];
                    uchar *ptarget = ((uchar *)pdata);
                    uint32_t vrf_id = pmsghdr->msg_index;

                    memset(target, 0, msg_len*sizeof(struct l3vpn_target));

                    ret = mpls_l3vpn_get_target_bulk(vrf_id, ptarget, pmsghdr->data_len, pmsghdr->msg_subtype, target);
                    if (ret > 0)
                    {
                        ret = ipc_send_reply_n2(target, ret*sizeof(struct l3vpn_target), ret, pmsghdr->sender_id,
                                                MODULE_ID_MPLS, IPC_TYPE_L3VPN, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);
                    }
                    else
                    {
                        ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_MPLS, IPC_TYPE_L3VPN,
                                                pmsghdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);
                    }
                }
            }
            else
            {
                pvroute = (struct l3vpn_route *)pdata;
                
                for (i=0; i<pmsghdr->data_num; i++)
                {
                    if (pmsghdr->opcode == IPC_OPCODE_ADD)
                    {
                        mpls_l3vpn_route_node_add(pvroute);
                    }
                    else if (pmsghdr->opcode == IPC_OPCODE_DELETE)
                    {
                        mpls_l3vpn_route_node_delete(pvroute);
                    }
                    pvroute += 1;
                }
            }
        }
        else if (pmsghdr->msg_type == IPC_TYPE_NHLFE)
        {
            uint32_t lsp_index = *(uint32_t *)pdata;
            enum OPCODE_E opcode = OPCODE_DOWN;

            if (pmsghdr->msg_subtype == LSP_SUBTYPE_UP)
            {
                opcode = OPCODE_UP;
            }

            static_lsp_process_arp(lsp_index, opcode, pmsghdr->msg_index);
        }
        else if(pmsghdr->msg_type == IPC_TYPE_FTM)
        {
            ldp_sess_process_tcp_event(pdata, pmsghdr->opcode);
        }
    }

    mem_share_free(pmesg, MODULE_ID_MPLS);

    return ret;
}

int mpls_bfd_ipc_msg_process(struct ipc_mesg_n *pmesg, int imlen)
{
    struct ipc_msghdr_n *pmsghdr  = NULL;
    struct pkt_buffer *ppkt = NULL;
    struct ip_control *ipcb = NULL;
	struct mpls_control *mplscb = NULL;
    void *pdata = NULL;
    int ret   = 0;
    int revln = 0;

	BFD_DEBUG();

    if(NULL == pmesg)
    {
		BFD_DEBUG("pmesg is NULL\n");
        return 0;
    }

    revln = (int)pmesg->msghdr.data_len + IPC_HEADER_LEN_N; 

    if(revln <= imlen)
    {
        pmsghdr = &(pmesg->msghdr);
        pdata   = pmesg->msg_data;

        BFD_DEBUG("--->msg_type = %d, opcode = %d, subtype = %d\n", pmsghdr->msg_type, pmsghdr->opcode, pmsghdr->msg_subtype);
        if(pmsghdr->msg_type == IPC_TYPE_PACKET)
        {
			//ppkt = pmesg->msg_data;
			ppkt = pkt_rcv_n(pmesg);
			
			switch (ppkt->cb_type)
			{
 				case PKT_TYPE_UDP:
				{
					ipcb = (struct ip_control *)&(ppkt->cb);

					BFD_DEBUG("-->ppkt->cb_type %d", ppkt->cb_type);
					if(ipcb->dport == UDP_PORT_BFD)
						bfd_recv(ppkt);
 					break;
				}
 				case PKT_TYPE_MPLS:
				{
					mplscb = (struct mpls_control *)&(ppkt->cb);
			
					BFD_DEBUG("mplscb->chtype %d", mplscb->chtype);
					if((UDP_PORT_BFD == mplscb->dport) || (MPLS_CTLWORD_WITHOUT_IP == mplscb->chtype))
					{
						bfd_recv(ppkt);
					}
 					break;
				}
 				default:
					BFD_DEBUG("--->con`t hunter !! unknown pkt");
					break;
			}
			
        }
		
        if (pmsghdr->msg_type == IPC_TYPE_BFD) /* bfd 消息 */
		{
			if (pmsghdr->opcode == IPC_OPCODE_EVENT)
			{
				mpls_msg_rcv_bfd(pmsghdr, pdata);
			}
            else if (pmsghdr->opcode == IPC_OPCODE_GET)
			{
				ret = mpls_msg_rcv_get_bfd(pmsghdr, pdata);
			}
            else if (pmsghdr->opcode == IPC_OPCODE_GET_BULK)
			{
				ret = mpls_msg_rcv_get_bulk_bfd(pmsghdr, pdata);
			}
			else if (pmsghdr->opcode == IPC_OPCODE_ADD)
			{
				ret = mpls_msg_rcv_add_bfd(pmsghdr, pdata);
			}
			else if (pmsghdr->opcode == IPC_OPCODE_UPDATE)
			{
				ret = mpls_msg_recv_update_bfd(pmsghdr, pdata);
			}
			else if (pmsghdr->opcode == IPC_OPCODE_DELETE)
			{
				ret = mpls_msg_rcv_del_bfd(pmsghdr, pdata);
			}
		}
    }

    mem_share_free(pmesg, MODULE_ID_BFD);

    return ret;
}

