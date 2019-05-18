/*
*       manage the nhp table
*
*/

#include <string.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/index.h>
#include <lib/linklist.h>
#include <lib/zassert.h>
#include <lib/thread.h>
#include <lib/mpls_common.h>
#include <lib/hptimer.h>
#include <route/ndp_cmd.h>
#include <route/arp_cmd.h>
#include "ftm_nhp.h"
#include "ftm_nhlfe.h"
#include "ftm_fib.h"
#include "ftm_ifm.h"
#include "ftm.h"
#include "log.h"


struct list nhp_list; /* 需要学习 arp 的 nhp 链表*/


static unsigned int ftm_hash(void *data)
{
	if(NULL == data)
		return 0;

	return *(unsigned int *)data;
}

static int ftm_compare(void *item, void *hash_key)
{
    struct hash_bucket *b = NULL;

	if((NULL == item) || (NULL == hash_key))
		return 1;

	b = (struct hash_bucket *)item;

	return *(unsigned int *)b->hash_key == *(unsigned int *)hash_key ? 0 : 1;
}

void ftm_nhp_table_init(unsigned int size)
{
    zlog_debug(FTM_DBG_L3, "%s:Initialize the next-hop table size, size=%d.\n",
                __func__, size);
	hios_hash_init(&nhp_table, size, ftm_hash, ftm_compare);
}

int ftm_nhp_add(struct nhp_entry *nhp)
{
	int ret = 0;
    int flag = 0;
    struct nhp_entry *pnhp = NULL;
    struct hash_bucket *item = NULL;
    struct arp_entry *parp = NULL;
	struct ndp_neighbor *pndp = NULL;
	char ip_str[IPV6_ADDR_STRLEN];

    if(NULL == nhp)
    {
        zlog_err("%s:Abnormal function parameter.\n", __func__);
        return 1;
    }

    zlog_debug(FTM_DBG_L3, "%s:Value of function parameters, nhp_index=%d nexthop=%s ifindex=%d vpn=%d nhp_type=%d nhp_action=%d.\n",
                __func__, nhp->nhp_index, inet_addrtostr(&nhp->nexthop, ip_str, IPV6_ADDR_STRLEN), nhp->ifindex, nhp->vpn, nhp->nhp_type, nhp->action);
    pnhp = ftm_nhp_lookup(nhp->nhp_index);
    if(NULL != pnhp)
    {
        zlog_debug(FTM_DBG_L3, "%s:Next hop table entry already exists, now update this table entry, nhp_index=%d.\n",
                    __func__, nhp->nhp_index);

        flag = 1;
        memcpy(pnhp, nhp, sizeof(struct nhp_entry));
    }
    else
    {
        pnhp = (struct nhp_entry *)XMALLOC(MTYPE_NHP_ENTRY, sizeof(struct nhp_entry));
        if(NULL == pnhp)
        {
            zlog_err("%s:Failed to allocate memory for next hop table entry.\n", __func__);
            return 1;
        }

        memcpy(pnhp, nhp, sizeof(struct nhp_entry));

        item = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
        if(NULL == item)
        {
            zlog_err("%s:Failed to allocate memory for hash bucket.\n", __func__);
            XFREE(MTYPE_NHP_ENTRY, pnhp);
            return 1;
        }

        memset(item, 0, sizeof(struct hash_bucket));
        item->hash_key = (void *)&pnhp->nhp_index;
        item->data = (void *)pnhp;
        ret = hios_hash_add(&nhp_table, item);
        if(-1 == ret)
        {
            zlog_err("%s:Failed to add next hop table entry, nhp_index=%d.\n",
                        __func__, pnhp->nhp_index);
            XFREE(MTYPE_HASH_BACKET, item);
            XFREE(MTYPE_NHP_ENTRY, pnhp);

            return 1;
        }
    }

    /* get arp and send to hal */
    memset(pnhp->mac, 0, sizeof(pnhp->mac));
	if(INET_FAMILY_IPV4 == pnhp->nexthop_connect.type)
	{
		if((0 != pnhp->nexthop_connect.addr.ipv4) && (INADDR_LOOPBACK != pnhp->nexthop_connect.addr.ipv4) && 
					(0 == pnhp->arp_flag) && (NHP_TYPE_CONNECT != pnhp->nhp_type) && (NHP_TYPE_LSP != pnhp->nhp_type))
		{
			zlog_debug(FTM_DBG_L3, "%s:To find next-hop arp information, nexthop=%s vpn=%d.\n",
						__func__, inet_addrtostr(&pnhp->nexthop_connect, ip_str, IPV6_ADDR_STRLEN), pnhp->vpn);

			parp = arp_lookup_active(pnhp->nexthop_connect.addr.ipv4, pnhp->vpn);
			if(NULL != parp && pnhp->ifindex == parp->ifindex)
			{
				zlog_debug(FTM_DBG_L3, "%s:Success to find next-hop arp information, nexthop=%s mac=%02X:%02X:%02X:%02X:%02X:%02X\n",
							__func__, ip_str, parp->mac[0], parp->mac[1], parp->mac[2], parp->mac[3], parp->mac[4], parp->mac[5]);


				pnhp->port = parp->port;
				memcpy(pnhp->mac, parp->mac, 6);
				pnhp->arp_flag = 1;
			}
			else
			{
				zlog_debug(FTM_DBG_L3, "%s:Failed to find next-hop arp information, the next-hop action would be set as DROP, nexthop=%s vpn=%d.\n",
							__func__, ip_str, pnhp->vpn);

				pnhp->action = NHP_ACTION_DROP;
				pnhp->port = 0;
				ftm_nhp_list_add_node(pnhp);
			}
		}
	}
	else if(INET_FAMILY_IPV6 == pnhp->nexthop_connect.type)
	{
		if((0 == pnhp->arp_flag) && (NHP_TYPE_CONNECT != pnhp->nhp_type) && (NHP_TYPE_LSP != pnhp->nhp_type))
		{
			zlog_debug(FTM_DBG_L3, "%s:To find next-hop arp information, nexthop=%s vpn=%d.\n",
						__func__, inet_addrtostr(&pnhp->nexthop_connect, ip_str, IPV6_ADDR_STRLEN), pnhp->vpn);

			pndp = ndp_lookup_active((struct ipv6_addr *)pnhp->nexthop_connect.addr.ipv6, pnhp->vpn, pnhp->ifindex);
			if(NULL != pndp && pnhp->ifindex == pndp->key.ifindex)
			{
				zlog_debug(FTM_DBG_L3, "%s:Success to find next-hop ndp information, nexthop=%s mac=%02X:%02X:%02X:%02X:%02X:%02X\n",
							__func__, ip_str, pndp->mac[0], pndp->mac[1], pndp->mac[2], pndp->mac[3], pndp->mac[4], pndp->mac[5]);

				pnhp->port = pndp->port;
				memcpy(pnhp->mac, pndp->mac, MAC_LEN);
				pnhp->arp_flag = 1;
			}
			else
			{
				zlog_debug(FTM_DBG_L3, "%s:Failed to find next-hop arp information, the next-hop action would be set as DROP, nexthop=%s vpn=%d.\n",
							__func__, ip_str, pnhp->vpn);

				pnhp->action = NHP_ACTION_DROP;
				pnhp->port = 0;
				ftm_nhp_list_add_node(pnhp);
			}
		}
	}

    if(1 == flag)
    {
        ret = ftm_msg_send_to_hal(pnhp, sizeof(struct nhp_entry), 1, IPC_TYPE_NHP, pnhp->nhp_type, IPC_OPCODE_UPDATE, 0);
        if(-1 == ret)
        {
            zlog_err("%s:Send next hop table entry update message to hal failure, nhp_index=%d.\n",
                        __func__, pnhp->nhp_index);

            ftm_nhp_delete(pnhp->nhp_index);

            return 1;
        }
    }
    else
    {
        ret = ftm_msg_send_to_hal(pnhp, sizeof(struct nhp_entry), 1, IPC_TYPE_NHP, pnhp->nhp_type, IPC_OPCODE_ADD, 0);
        if(-1 == ret)
        {
            zlog_err("%s:Send next hop table entry add message to hal failure, nhp_index=%d.\n",
                        __func__, pnhp->nhp_index);

			ftm_nhp_list_delete_node(pnhp);

            hios_hash_delete(&nhp_table, item);
            item->prev = NULL;
            item->next = NULL;
            XFREE(MTYPE_HASH_BACKET, item);
            XFREE(MTYPE_NHP_ENTRY, pnhp);

            return 1;
        }
    }

	return 0;
}

int ftm_nhp_update(struct nhp_entry *nhp)
{
    int ret = 0;
    struct nhp_entry *pnhp = NULL;
	char ip_str[IPV6_ADDR_STRLEN];

    if(NULL == nhp)
    {
        zlog_err("%s:Abnormal function parameter.\n", __func__);
        return 1;
    }

    zlog_debug(FTM_DBG_L3, "%s:Value of function parameters, nhp_index=%d nexthop=%s ifindex=%d vpn=%d nhp_type=%d protocol=%d distance=%d.\n",
                __func__, nhp->nhp_index, inet_addrtostr(&nhp->nexthop, ip_str, IPV6_ADDR_STRLEN), nhp->ifindex, nhp->vpn, 
				nhp->nhp_type, nhp->protocol, nhp->distance);

    pnhp = ftm_nhp_lookup(nhp->nhp_index);
    if(NULL == pnhp)
    {
        zlog_err("%s:Failed to find next-hop table entry, nhp_index=%d.\n",
                    __func__, nhp->nhp_index);
        return 1;
    }

    zlog_debug(FTM_DBG_L3, "%s:Now update the next-hop table entry related information, nhp_index=%d nhp_type=%d protocol=%d distance=%d.\n",
                __func__, pnhp->nhp_index, pnhp->nhp_type, pnhp->protocol, pnhp->distance);

    pnhp->nhp_type = nhp->nhp_type;
    pnhp->protocol = nhp->protocol;
    pnhp->distance = nhp->distance;
    pnhp->cost = nhp->cost;

    ret = ftm_msg_send_to_hal(pnhp, sizeof(struct nhp_entry), 1, IPC_TYPE_NHP, pnhp->nhp_type, IPC_OPCODE_UPDATE, 0);
    if(-1 == ret)
    {
        zlog_err("%s:Send next hop table entry update message to hal failure, nhp_index=%d.\n",
                    __func__, pnhp->nhp_index);
        return 1;
    }

    return 0;
}

int ftm_nhp_delete(uint32_t nhp_index)
{
    int ret = 0;
    struct hash_bucket *item = NULL;

    zlog_debug(FTM_DBG_L3, "%s:To delete next hop table entry, nhp_index=%d.\n",
                __func__, nhp_index);

	item = hios_hash_find(&nhp_table, &nhp_index);
	if((NULL == item) || (NULL == item->data))
    {
        zlog_err("%s:Failed to find next hop table entry, nhp_index=%d.\n",
                    __func__, nhp_index);
		return 1;
    }

    ret = ftm_msg_send_to_hal(&nhp_index, sizeof(nhp_index), 1, IPC_TYPE_NHP, ((struct nhp_entry *)(item->data))->nhp_type, IPC_OPCODE_DELETE, 0);
    if(-1 == ret)
    {
        zlog_err("%s:Send next hop table entry delete message to hal failure, nhp_index=%d.\n",
                    __func__, nhp_index);
        return 1;
    }

    ftm_nhp_list_delete_node((struct nhp_entry *)item->data);

	hios_hash_delete(&nhp_table, item);

    XFREE(MTYPE_NHP_ENTRY, item->data);
    item->prev = NULL;
    item->next = NULL;
    XFREE(MTYPE_HASH_BACKET, item);

	return 0;
}

struct nhp_entry *ftm_nhp_lookup(uint32_t nhp_index)
{
    struct hash_bucket *item = NULL;
    struct nhp_entry *pnhp = NULL;
	char ip_str[IPV6_ADDR_STRLEN] = "";

    zlog_debug(FTM_DBG_L3, "%s:To find next hop table entry, nhp_index=%d.\n",
                __func__, nhp_index);

	item = hios_hash_find(&nhp_table, &nhp_index);
	if(NULL == item)
    {
		return NULL;
    }

    pnhp = (struct nhp_entry *)item->data;
    if(NULL != pnhp)
    {
        zlog_debug(FTM_DBG_L3, "%s:Next hop table entry information, nhp_index=%d nexthop=%s ifindex=%d vpn=%d nhp_type=%d nhp_action=%d.\n",
                    __func__, pnhp->nhp_index, inet_addrtostr(&pnhp->nexthop, ip_str, IPV6_ADDR_STRLEN), pnhp->ifindex, pnhp->vpn, 
					pnhp->nhp_type, pnhp->action);        
		return pnhp;
    }
    else
    {
	    return NULL;
    }
}

int ftm_ecmp_update_nhp(struct nhp_entry *pnhp)
{
    int i = 0;
    int j = 0;
    int ret = 0;
    struct hash_bucket *bucket = NULL;
    struct ecmp_group *ecmp = NULL;

    zlog_debug(FTM_DBG_L3, "%s:Update ecmp table entry nhp information, nhp_index=%d.\n",
                __func__, pnhp->nhp_index);

    HASH_BUCKET_LOOP(bucket, i, ecmp_table)
    {
        ecmp = (struct ecmp_group *)bucket->data;
        for(j = 0; j < ecmp->nhp_num; j++)
        {
            if(ecmp->nhp[j].nhp_index == pnhp->nhp_index)
            {
                ret = ftm_msg_send_to_hal(ecmp, sizeof(struct ecmp_group), 1, IPC_TYPE_NHP, NHP_TYPE_ECMP, IPC_OPCODE_UPDATE, 0);
                if(-1 == ret)
                {
                    zlog_err("%s:Send ecmp table entry update message to hal failure, ecmp_index=%d.\n",
                                __func__, ecmp->group_index);
                }

                break;
            }
        }
    }

    return 0;
}

/* 更新 nhp 的 arp 信息*/
int ftm_nhp_update_arp(struct arp_entry *parp, int opcode)
{
    int i = 0;
    int ret = 0;
    struct hash_bucket *bucket = NULL;
    struct nhp_entry *nhp = NULL;

    zlog_debug(FTM_DBG_L3, "%s:Value of function parameters, nexthop=%#X vpn=%d ifindex=%d opcode=%d mac=%02X:%02X:%02X:%02X:%02X:%02X.\n",
                __func__, parp->key.ipaddr, parp->key.vpnid, parp->ifindex, opcode, parp->mac[0],
                parp->mac[1], parp->mac[2], parp->mac[3], parp->mac[4], parp->mac[5]);

    if(1 == opcode) //update
    {
        HASH_BUCKET_LOOP(bucket, i, nhp_table)
        {
            nhp = (struct nhp_entry *)bucket->data;
            if((NULL != nhp) && (INET_FAMILY_IPV4 == nhp->nexthop_connect.type) && (nhp->nexthop_connect.addr.ipv4 == parp->key.ipaddr) 
					&& (nhp->vpn == parp->key.vpnid) && (nhp->ifindex == parp->ifindex) && (NHP_TYPE_CONNECT != nhp->nhp_type) 
					&& (NHP_TYPE_LSP != nhp->nhp_type))            
			{
                zlog_debug(FTM_DBG_L3, "%s:Update next hop table entry arp information, nhp_index=%d.\n",
                            __func__, nhp->nhp_index);

                nhp->port = parp->port;
                memcpy(nhp->mac, parp->mac, sizeof(parp->mac));
                if(ARP_STATUS_INCOMPLETE == parp->status)
                {
                    nhp->arp_flag = 0;
                    nhp->action = NHP_ACTION_DROP;
                    ftm_nhp_list_add_node(nhp);
                }
                else
                {
                    nhp->arp_flag = 1;
                    nhp->action = NHP_ACTION_FORWARD;
                    ftm_nhp_list_delete_node(nhp);
                }

                ret = ftm_msg_send_to_hal(nhp, sizeof(struct nhp_entry), 1, IPC_TYPE_NHP, nhp->nhp_type, IPC_OPCODE_UPDATE, 0);
                if(-1 == ret)
                {
                    zlog_err("%s:Send next hop table entry update message to hal failure, nhp_index=%d.\n",
                                __func__, nhp->nhp_index);
                }
                else
                {
                    ftm_ecmp_update_nhp(nhp);
                }
            }
        }
    }
    else if(0 == opcode) //delete
    {
        HASH_BUCKET_LOOP(bucket, i, nhp_table)
        {
            nhp = (struct nhp_entry *)bucket->data;
            if((NULL != nhp) && (INET_FAMILY_IPV4 == nhp->nexthop_connect.type) && (nhp->nexthop_connect.addr.ipv4 == parp->key.ipaddr) 
					&& (nhp->vpn == parp->key.vpnid) && (nhp->ifindex == parp->ifindex) && (NHP_TYPE_CONNECT != nhp->nhp_type) 
					&& (NHP_TYPE_LSP != nhp->nhp_type))            
			{
                zlog_debug(FTM_DBG_L3, "%s:Delete next hop table entry arp information, nhp_index=%d.\n",
                            __func__, nhp->nhp_index);

                memset(nhp->mac, 0, sizeof(nhp->mac));
                nhp->port = 0;
                nhp->arp_flag = 0;
                nhp->action = NHP_ACTION_DROP;

                ftm_nhp_list_add_node(nhp);

                ret = ftm_msg_send_to_hal(nhp, sizeof(struct nhp_entry), 1, IPC_TYPE_NHP, nhp->nhp_type, IPC_OPCODE_UPDATE, 0);
                if(-1 == ret)
                {
                    zlog_err("%s:Send next hop table entry update message to hal failure, nhp_index=%d.\n",
                                __func__, nhp->nhp_index);
                }
                else
                {
                    ftm_ecmp_update_nhp(nhp);
                }
            }
        }
    }

    return 0;
}

/* 更新 nhp 的 ndp 信息*/
int ftm_nhp_update_ndp(struct ndp_neighbor *pndp, int opcode)
{
    int i = 0;
    int ret = 0;
    struct hash_bucket *bucket = NULL;
    struct nhp_entry *nhp = NULL;
	char ip_str[IPV6_ADDR_STRLEN];

    zlog_debug(FTM_DBG_L3, "%s:Value of function parameters, nexthop=%s vpn=%d ifindex=%d opcode=%d mac=%02X:%02X:%02X:%02X:%02X:%02X.\n",
                __func__, inet_ipv6tostr((struct ipv6_addr *)pndp->key.ipv6_addr, ip_str, IPV6_ADDR_STRLEN), pndp->key.vpnid, pndp->key.ifindex, 
				opcode, pndp->mac[0], pndp->mac[1], pndp->mac[2], pndp->mac[3], pndp->mac[4], pndp->mac[5]);

    if(1 == opcode) //update
    {
        HASH_BUCKET_LOOP(bucket, i, nhp_table)
        {
            nhp = (struct nhp_entry *)bucket->data;
            if((NULL != nhp) && (INET_FAMILY_IPV6 == nhp->nexthop_connect.type) && !memcmp(nhp->nexthop_connect.addr.ipv6, pndp->key.ipv6_addr, IPV6_ADDR_LEN) 
					&& (nhp->vpn == pndp->key.vpnid) && (nhp->ifindex == pndp->key.ifindex) && (NHP_TYPE_CONNECT != nhp->nhp_type) 
					&& (NHP_TYPE_LSP != nhp->nhp_type))
            {
                zlog_debug(FTM_DBG_L3, "%s:Update next hop table entry ndp information, nhp_index=%d.\n",
                            __func__, nhp->nhp_index);

                nhp->port = pndp->port;
                memcpy(nhp->mac, pndp->mac, MAC_LEN); 
                if(NDP_STATUS_INCOMPLETE == pndp->status || NDP_STATUS_STALE == pndp->status || 
							NDP_STATUS_DELAY == pndp->status || NDP_STATUS_PROBE == pndp->status)
                {
                    nhp->arp_flag = 0;
                    nhp->action = NHP_ACTION_DROP;
                    ftm_nhp_list_add_node(nhp);
                }
                else
                {
                    nhp->arp_flag = 1;
                    nhp->action = NHP_ACTION_FORWARD;
                    ftm_nhp_list_delete_node(nhp);
                }

                ret = ftm_msg_send_to_hal(nhp, sizeof(struct nhp_entry), 1, IPC_TYPE_NHP, nhp->nhp_type, IPC_OPCODE_UPDATE, 0);
                if(-1 == ret)
                {
                    zlog_err("%s:Send next hop table entry update message to hal failure, nhp_index=%d.\n",
                                __func__, nhp->nhp_index);
                }
                else
                {
                    ftm_ecmp_update_nhp(nhp);
                }
            }
        }
    }
    else if(0 == opcode) //delete
    {
        HASH_BUCKET_LOOP(bucket, i, nhp_table)
        {
            nhp = (struct nhp_entry *)bucket->data;
            if((NULL != nhp) && (INET_FAMILY_IPV6 == nhp->nexthop_connect.type) && !memcmp(nhp->nexthop_connect.addr.ipv6, pndp->key.ipv6_addr, IPV6_ADDR_LEN) 
					&& (nhp->vpn == pndp->key.vpnid) && (nhp->ifindex == pndp->key.ifindex) && (NHP_TYPE_CONNECT != nhp->nhp_type) 
					&& (NHP_TYPE_LSP != nhp->nhp_type))
            {
                zlog_debug(FTM_DBG_L3, "%s:Delete next hop table entry arp information, nhp_index=%d.\n",
                            __func__, nhp->nhp_index);

                memset(nhp->mac, 0, sizeof(nhp->mac));
                nhp->port = 0;
                nhp->arp_flag = 0;
                nhp->action = NHP_ACTION_DROP;

                ftm_nhp_list_add_node(nhp);

                ret = ftm_msg_send_to_hal(nhp, sizeof(struct nhp_entry), 1, IPC_TYPE_NHP, nhp->nhp_type, IPC_OPCODE_UPDATE, 0);
                if(-1 == ret)
                {
                    zlog_err("%s:Send next hop table entry update message to hal failure, nhp_index=%d.\n",
                                __func__, nhp->nhp_index);
                }
                else
                {
                    ftm_ecmp_update_nhp(nhp);
                }
            }
        }
    }

    return 0;
}

void ftm_ecmp_table_init(unsigned int size)
{
    zlog_debug(FTM_DBG_L3, "%s:Initialize the ecmp table size, size=%d.\n",
                __func__, size);
	hios_hash_init(&ecmp_table, size, ftm_hash, ftm_compare);
}

int ftm_ecmp_add(struct ecmp_group *ecmp)
{
	int i = 0;
    int ret = 0;
    int flag = 0;
    int nhp_index = 0;
    struct ecmp_group *ecmp_info = NULL;
    struct nhp_entry *nhp_info = NULL;
    struct hash_bucket *item = NULL;

    if(NULL == ecmp)
    {
        zlog_err("%s:Abnormal function parameter.\n", __func__);
        return 1;
    }

    zlog_debug(FTM_DBG_L3, "%s:Value of function parameters, group_index=%d group_type=%d nhp_num=%d frr_flag=%d.\n",
                __func__, ecmp->group_index, ecmp->group_type, ecmp->nhp_num, ecmp->frr_flag);

    ecmp_info = ftm_ecmp_lookup(ecmp->group_index);
    if(NULL != ecmp_info)
    {
        zlog_debug(FTM_DBG_L3, "%s:Ecmp table entry already existed, now update this table entry, group_index=%d.\n",
                    __func__, ecmp->group_index);

        flag = 1;
        memcpy(ecmp_info, ecmp, sizeof(struct ecmp_group));
    }
    else
    {
        ecmp_info = (struct ecmp_group *)XMALLOC(MTYPE_ECMP_ENTRY, sizeof(struct ecmp_group));
        if(NULL == ecmp_info)
        {
            zlog_err("%s:Failed to allocate memory for ecmp table entry.\n", __func__);
            return 1;
        }

        memset(ecmp_info, 0,sizeof(struct ecmp_group));
        memcpy(ecmp_info, ecmp, sizeof(struct ecmp_group));

        item = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
        if(NULL == item)
        {
            zlog_err("%s:Failed to allocate memory for hash bucket.\n", __func__);
            XFREE(MTYPE_ECMP_ENTRY, ecmp_info);
            return 1;
        }

        memset(item, 0, sizeof(struct hash_bucket));
        item->hash_key = (void *)&ecmp_info->group_index;
        item->data = (void *)ecmp_info;
        ret = hios_hash_add(&ecmp_table, item);
        if(-1 == ret)
        {
            zlog_err("%s:Failed to add ecmp table entry, group_index=%d.\n",
                        __func__, ecmp_info->group_index);
            XFREE(MTYPE_HASH_BACKET, item);
            XFREE(MTYPE_ECMP_ENTRY, ecmp_info);

            return 1;
        }
    }

    for(i = 0; i < ecmp_info->nhp_num; i++)
    {
        nhp_index = ecmp_info->nhp[i].nhp_index;

        nhp_info = ftm_nhp_lookup(nhp_index);
        if(NULL == nhp_info)
        {
            zlog_err("%s:Failed to find next-hop table entry information, nhp_index=%d.\n",
                        __func__, nhp_index);
			if(1 == flag)
			{
				ftm_ecmp_delete(ecmp_info->group_index);
			}
			else
			{
            	hios_hash_delete(&ecmp_table, item);

            	item->prev = NULL;
            	item->next = NULL;
           	 	XFREE(MTYPE_HASH_BACKET, item);
            	XFREE(MTYPE_ECMP_ENTRY, ecmp_info);
			}

            return 1;
        }

        ecmp_info->nhp[i].pnhp = (void *)nhp_info;
    }

    if(1 == flag)
    {
        ret = ftm_msg_send_to_hal(ecmp_info, sizeof(struct ecmp_group), 1, IPC_TYPE_NHP, NHP_TYPE_ECMP, IPC_OPCODE_UPDATE, 0);
        if(-1 == ret)
        {
            zlog_err("%s:Send ecmp table entry update message to hal failure, group_index=%d.\n",
                        __func__, ecmp_info->group_index);

            ftm_ecmp_delete(ecmp_info->group_index);

            return 1;
        }
    }
    else
    {
        ret = ftm_msg_send_to_hal(ecmp_info, sizeof(struct ecmp_group), 1, IPC_TYPE_NHP, NHP_TYPE_ECMP, IPC_OPCODE_ADD, 0);
        if(-1 == ret)
        {
            zlog_err("%s:Send ecmp table entry add message to hal failure, group_index=%d.\n",
                        __func__, ecmp_info->group_index);
            hios_hash_delete(&ecmp_table, item);

            item->prev = NULL;
            item->next = NULL;
            XFREE(MTYPE_HASH_BACKET, item);
            XFREE(MTYPE_ECMP_ENTRY, ecmp_info);

            return 1;
        }
    }

	return 0;
}

int ftm_ecmp_delete(uint32_t ecmp_index)
{
    int ret = 0;
    struct hash_bucket *item = NULL;

    zlog_debug(FTM_DBG_L3, "%s:To delete ecmp table entry, group_index=%d.\n",
                __func__, ecmp_index);

	item = hios_hash_find(&ecmp_table, &ecmp_index);
	if((NULL == item) || (NULL == item->data))
    {
        zlog_err("%s:Failed to find ecmp table entry, group_index=%d.\n",
                    __func__, ecmp_index);
		return 1;
    }

    ret = ftm_msg_send_to_hal(&ecmp_index, sizeof(ecmp_index), 1, IPC_TYPE_NHP, NHP_TYPE_ECMP, IPC_OPCODE_DELETE, 0);
    if(-1 == ret)
    {
        zlog_err("%s:Send ecmp table entry delete message to hal failure, group_index=%d.\n",
                    __func__, ecmp_index);
        return 1;
    }

	hios_hash_delete(&ecmp_table, item);

    XFREE(MTYPE_ECMP_ENTRY, item->data);
    item->prev = NULL;
    item->next = NULL;
    XFREE(MTYPE_HASH_BACKET, item);

	return 0;
}

struct ecmp_group *ftm_ecmp_lookup(uint32_t ecmp_index)
{
    struct hash_bucket *item = NULL;
    struct ecmp_group *ecmp = NULL;

    zlog_debug(FTM_DBG_L3, "%s:To find ecmp table entry information, group_index=%d.\n",
                __func__, ecmp_index);

	item = hios_hash_find(&ecmp_table, &ecmp_index);
	if(!item)
    {
        zlog_err("%s:Failed to find ecmp table entry, group_index=%d.\n",
                    __func__, ecmp_index);
		return NULL;
    }

    ecmp = (struct ecmp_group *)item->data;
    if(NULL != ecmp)
    {
        zlog_debug(FTM_DBG_L3, "%s:Ecmp table entry information, group_index=%d group_type=%d nhp_num=%d frr_flag=%d.\n",
                    __func__, ecmp->group_index, ecmp->group_type, ecmp->nhp_num, ecmp->frr_flag);
        return ecmp;
    }
    else
    {
        zlog_err("%s:The ecmp table entry is empty, group_index=%d.\n",
                    __func__, ecmp_index);
        return NULL;
    }
}

void *nhp_lookup(uint32_t nhp_index, uint8_t nhp_type)
{
	switch (nhp_type)
	{
		case NHP_TYPE_ECMP:
		case NHP_TYPE_FRR:
			return ftm_ecmp_lookup(nhp_index);
		default:
			return ftm_nhp_lookup(nhp_index);
	}
}

int ftm_nhp_msg(void *pdata, int data_len, int data_num, uint8_t subtype, enum IPC_OPCODE opcode)
{
    int i = 0;
    struct nhp_entry *nhp = NULL;
    struct ecmp_group *ecmp = NULL;

    if(NULL == pdata)
    {
        zlog_err("%s:Abnormal function parameter.\n", __func__);
        return 1;
    }

    zlog_debug(FTM_DBG_L3, "%s:Value of function parameter, data_len=%d data_num=%d subtype=%d opcode=%d.\n",
                __func__, data_len, data_num, subtype, opcode);

    if(NHP_TYPE_CONNECT == subtype || NHP_TYPE_HOST == subtype || NHP_TYPE_IP == subtype || NHP_TYPE_TUNNEL == subtype ||
			NHP_TYPE_LSP == subtype)
    {
        for(i = 0; i < data_num; i++)
        {
            nhp = (struct nhp_entry *)pdata;

            if(IPC_OPCODE_ADD == opcode)
            {
                ftm_nhp_add(nhp);
            }
            else if(IPC_OPCODE_UPDATE == opcode)
            {
                ftm_nhp_update(nhp);
            }
            else if(IPC_OPCODE_DELETE == opcode)
            {
                ftm_nhp_delete(nhp->nhp_index);
            }

            nhp += sizeof(struct nhp_entry);
        }
    }
    else if(NHP_TYPE_ECMP == subtype || NHP_TYPE_FRR == subtype)
    {
        for(i = 0; i < data_num; i++)
        {
            ecmp = (struct ecmp_group *)pdata;

            if(IPC_OPCODE_ADD == opcode)
            {
                ftm_ecmp_add(ecmp);
            }
            else if(IPC_OPCODE_DELETE == opcode)
            {
                ftm_ecmp_delete(ecmp->group_index);
            }

            ecmp += sizeof(struct ecmp_group);
        }
    }

    return 0;
}

//int ftm_traverse_nhp_list(struct thread *thread)
int ftm_traverse_nhp_list(void *para)
{
    struct list *plist = &nhp_list;
    struct listnode *pnode = NULL;
    struct nhp_entry *pnhp = NULL;
    struct arp_entry *parp = NULL;

    zlog_debug(FTM_DBG_L3, "%s:Traverse nhp list, list_count=%d.\n", __func__, nhp_list.count);

    for(ALL_LIST_ELEMENTS_RO(plist, pnode, pnhp))
    {
        if(NULL != pnhp)
        {
			if(INET_FAMILY_IPV4 == pnhp->nexthop_connect.type)
			{
				parp = arp_lookup_active(pnhp->nexthop_connect.addr.ipv4, pnhp->vpn); 
                /*路由出接口更新，删除对应直连下一条arp表项重新学习*/
                if((parp) && (parp->status != ARP_STATUS_STATIC) && (parp->ifindex != pnhp->ifindex))
                {
                    FTM_ARP_ZLOG_DEBUG("ip:0x%x vpn:%d old_ifindex:0x%x newnhp->ip:0x%x newnhp_connect->ip:0x%x new_pnhp->ifindex:0x%x",
                                    parp->key.ipaddr, parp->key.vpnid, parp->ifindex,pnhp->nexthop.addr.ipv4,pnhp->nexthop_connect.addr.ipv4,pnhp->ifindex);
                    arp_delete(parp->key.ipaddr, parp->key.vpnid, ARP_MASTER);
                    arp_miss(parp->key.ipaddr,parp->key.vpnid);
                }
			}
			else if(INET_FAMILY_IPV6 == pnhp->nexthop_connect.type)
			{
				ndp_lookup_active((struct ipv6_addr *)pnhp->nexthop_connect.addr.ipv6, pnhp->vpn, pnhp->ifindex); 
			}        
		}
    }

	if(0 != nhp_list.count)
	{
        //MPLS_TIMER_ADD(ftm_traverse_nhp_list, NULL, TRAVERSE_NHP_LIST_TIME);
		high_pre_timer_add("NhpTimer", LIB_TIMER_TYPE_NOLOOP, ftm_traverse_nhp_list, NULL, TRAVERSE_NHP_LIST_TIME * 1000);
	}

    return 0;
}

void ftm_nhp_list_add_node(struct nhp_entry *pnhp)
{
    struct listnode *pnode = NULL;
	char ip_str[IPV6_ADDR_STRLEN];

   zlog_debug(FTM_DBG_L3, "%s:Add nhp list node, nhp_index=%d nexthop=%s.\n", 
				__func__, pnhp->nhp_index, inet_addrtostr(&pnhp->nexthop_connect, ip_str, IPV6_ADDR_STRLEN));

    pnode = listnode_lookup(&nhp_list, (void *)pnhp);
    if(NULL != pnode)
    {
        zlog_debug(FTM_DBG_L3, "%s:Node already exists in nhp list.\n", __func__);
        return ;
    }

    listnode_add(&nhp_list, (void *)pnhp);
	if(1 == nhp_list.count)
	{
        //MPLS_TIMER_ADD(ftm_traverse_nhp_list, NULL, TRAVERSE_NHP_LIST_TIME);
		high_pre_timer_add("NhpTimer", LIB_TIMER_TYPE_NOLOOP, ftm_traverse_nhp_list, NULL, TRAVERSE_NHP_LIST_TIME * 1000);
	}

    return ;
}


void ftm_nhp_list_delete_node(struct nhp_entry *pnhp)
{
    zlog_debug(FTM_DBG_L3, "%s:Delete nhp list node, nhp_index=%d.\n", __func__, pnhp->nhp_index);

    listnode_delete(&nhp_list, (void *)pnhp);
    return ;
}
