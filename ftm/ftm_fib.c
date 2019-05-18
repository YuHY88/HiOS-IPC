/*
*   manage fib ptree
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/ptree.h>
#include <lib/log.h>
#include "ftm.h"
#include "ftm_fib.h"
#include "ftm_nhp.h"
#include "ftm_ndp.h"
#include "ftm_arp.h"
#include <route/route.h>

uint32_t fib_num = 0;
uint32_t fibv6_num = 0;

struct fib_global g_fib;  /* fib 的全局数据结构 */

/* fib 模块初始化 */
void ftm_fib_init()
{
	memset(&g_fib, 0, sizeof(struct fib_global));
	
    ftm_fib_tree_init();
    ftm_nhp_table_init(NHP_NUM_MAX);
    ftm_ecmp_table_init(ECMP_NUM_MAX);	
	ftm_arp_init();
	
    ftm_fibv6_tree_init();
    ftm_ndp_init();
}


int ftm_fib_tree_init()
{
    int vrf = 0;

    zlog_debug(FTM_DBG_L3, "%s:Initialize fib tree.\n", __func__);

    for(vrf = 0; vrf <= L3VPN_SIZE; vrf++)
    {
		fib_tree[vrf] = ptree_init(IP_MASK_LEN);
    }

    return 0;
}

void ftm_fib_tree_delete(uint16_t vrf)
{
    zlog_debug(FTM_DBG_L3, "%s:To delete fib tree, vrf=%d.\n", __func__, vrf);
	ptree_free(fib_tree[vrf]);
}

void ftm_fib_tree_clear()
{
    int vrf = 0;

    zlog_debug(FTM_DBG_L3, "%s:Empty fib tree.\n", __func__);

    for(vrf = 0; vrf <= L3VPN_SIZE; vrf++)
    {
		ptree_free(fib_tree[vrf]);
    }
}

int ftm_fib_add(struct fib_entry *pfib)
{
    int ret = 0;
    struct ptree_node *node = NULL;
	struct fib_entry *pfib_new = NULL;
    void *pnhp = NULL;

    zlog_debug(FTM_DBG_L3, "%s:Add fib tree node, ipaddr=%#X masklen=%d vpn=%d nhp_index=%d nhp_type=%d, fib_num=%d.\n",
                __func__, pfib->ipaddr, pfib->masklen, pfib->vpn, pfib->nhp.nhp_index, pfib->nhp.nhp_type, fib_num);

    node = ptree_node_get(fib_tree[pfib->vpn], (u_char *)&pfib->ipaddr, pfib->masklen);
    if(NULL == node)
    {
        zlog_err("%s:Failed to create fib tree node!\n", __func__);
        return 1;
    }

    if(NULL != node->info)
    {
        zlog_debug(FTM_DBG_L3, "%s:The fib tree node information is not empty, now update this node.\n", __func__);

        ptree_unlock_node(node);
        ret = ftm_fib_update((struct fib_entry *)node->info, pfib);
        return ret;
    }

    pnhp = nhp_lookup(pfib->nhp.nhp_index, pfib->nhp.nhp_type);
    if(NULL == pnhp)
    {
        zlog_err("%s:Failed to find nhp information, nhp_index=%d nhp_type=%d.\n",
                    __func__, pfib->nhp.nhp_index, pfib->nhp.nhp_type);
        node->lock = 0;
        node->info = NULL;
        ptree_node_delete(node);

        return 1;
    }

	pfib_new = (struct fib_entry *)XMALLOC(MTYPE_FIB_ENTRY, sizeof(struct fib_entry));
	if(NULL == pfib_new)
	{
		zlog_err("%s:Failed to allocate memory for fib entry.\n", __func__);
		node->lock = 0;
		node->info = NULL;
		ptree_node_delete(node);

		return 1;
	}

	memcpy(pfib_new, pfib, sizeof(struct fib_entry));
    pfib_new->nhp.pnhp = pnhp;
    node->info = (void *)pfib_new;

    ret = ftm_msg_send_to_hal(pfib_new, sizeof(struct fib_entry), 1, IPC_TYPE_FIB, 0, IPC_OPCODE_ADD, 0);
    if(-1 == ret)
    {
        zlog_err("%s:Send fib tree node add message to hal failure, ipaddr=%#X masklen=%d vpn=%d\n",
                    __func__, pfib_new->ipaddr, pfib_new->masklen, pfib_new->vpn);
        node->lock = 0;
        node->info = NULL;
        ptree_node_delete(node);
		XFREE(MTYPE_FIB_ENTRY, pfib_new);

        return 1;
    }

	fib_num += 1;

    return 0;
}

int ftm_fib_update(struct fib_entry *pfib_old, struct fib_entry *pfib_new)
{
    int ret = 0;
    void *pnhp_new = NULL;

    if(NULL == pfib_old || NULL == pfib_new)
    {
        zlog_err("%s:Abnormal function parameter.\n", __func__);
        return 1;
    }

    zlog_debug(FTM_DBG_L3, "%s:Update fib tree node information, ipaddr=%#X masklen=%d vpn=%d nhp_index=%d nhp_type=%d.\n",
                __func__, pfib_new->ipaddr, pfib_new->masklen, pfib_new->vpn,
                pfib_new->nhp.nhp_index, pfib_new->nhp.nhp_type);

    pnhp_new = nhp_lookup(pfib_new->nhp.nhp_index, pfib_new->nhp.nhp_type);
    if(NULL == pnhp_new)
    {
        zlog_err("%s:Failed to find nhp information, nhp_index=%d nhp_type=%d.\n",
                    __func__, pfib_new->nhp.nhp_index, pfib_new->nhp.nhp_type);
        return 1;
    }

    pfib_new->nhp.pnhp = pnhp_new;

    ret = ftm_msg_send_to_hal(pfib_new, sizeof(struct fib_entry), 1, IPC_TYPE_FIB, 0, IPC_OPCODE_UPDATE, 0);
    if(-1 == ret)
    {
        zlog_err("%s:Send fib tree node update message to hal failure, ipaddr=%#X masklen=%d vpn=%d nhp_index=%d nhp_type=%d.\n",
                    __func__, pfib_new->ipaddr, pfib_new->masklen, pfib_new->vpn,
                    pfib_new->nhp.nhp_index, pfib_new->nhp.nhp_type);
        return 1;
    }

    memcpy(&pfib_old->nhp, &pfib_new->nhp, sizeof(struct nhp_info));

    return 0;
}

int ftm_fib_delete(struct fib_entry *pfib)
{
    int ret = 0;
    struct ptree_node *node = NULL;
    struct fib_entry *pfib_info = NULL;

    zlog_debug(FTM_DBG_L3, "%s:To delete fib tree node, ipaddr=%#X masklen=%d vrf=%d.\n",
                __func__, pfib->ipaddr, pfib->masklen, pfib->vpn);

	node = ptree_node_lookup(fib_tree[pfib->vpn], (u_char *)&(pfib->ipaddr), pfib->masklen);
    if(NULL == node)
    {
        zlog_err("%s:Failed to find fib tree node, ipaddr=%#X masklen=%d vrf=%d.\n",
                    __func__, pfib->ipaddr, pfib->masklen, pfib->vpn);
		return 1;
    }

    if(NULL == node->info)
    {
        zlog_err("%s:The fib tree node information is empty, ipaddr=%#X masklen=%d vrf=%d.\n",
                    __func__, pfib->ipaddr, pfib->masklen, pfib->vpn);
        return 1;
    }

    pfib_info = (struct fib_entry *)node->info;

    ret = ftm_msg_send_to_hal(pfib_info, sizeof(struct fib_entry), 1, IPC_TYPE_FIB, 0, IPC_OPCODE_DELETE, 0);
    if(-1 == ret)
    {
        zlog_err("%s:Send fib tree node delete message to hal failure, ipaddr=%#X masklen=%d vrf=%d.\n",
                    __func__, pfib_info->ipaddr, pfib_info->masklen, pfib_info->vpn);
        return 1;
    }

    node->lock = 0;
    node->info = NULL;
    ptree_node_delete(node);
	XFREE(MTYPE_FIB_ENTRY, pfib_info);

	fib_num -= 1;

    return 0;
}

/* 返回精确匹配的路由 */
struct fib_entry *ftm_fib_lookup(uint32_t ipaddr, uchar masklen, uint16_t vrf)
{
    struct ptree_node *node = NULL;

    zlog_debug(FTM_DBG_L3, "%s:To find fib tree node information, ipaddr=%#X masklen=%d vrf=%d.\n",
                __func__, ipaddr, masklen, vrf);

	if(vrf > 1024)
	{
		return NULL;
	}

	node = ptree_node_lookup(fib_tree[vrf], (u_char *)&ipaddr, masklen);
    if(!node)
    {
		return NULL;
    }

    ptree_unlock_node(node);

    return (struct fib_entry *)node->info;
}


/* 返回最长匹配的路由 */
struct fib_entry *ftm_fib_match(uint32_t ipaddr, uchar masklen, uint16_t vrf)
{
    struct ptree_node *node = NULL;
    struct fib_entry *pfib = NULL;

    zlog_debug(FTM_DBG_L3, "%s:The fib tree longest matching mask, ipaddr=%#X masklen=%d vrf=%d.\n",
                __func__, ipaddr, masklen, vrf);

	if(vrf > 1024)
	{
		return NULL;
	}

	node = ptree_node_match(fib_tree[vrf], (u_char *)&ipaddr, masklen);
    if(NULL == node)
    {
		return NULL;
    }

    ptree_unlock_node(node);

    if(NULL == node->info)
    {
        return NULL;
    }

    pfib = (struct fib_entry *)node->info;

    return pfib;
}

int ftm_fibv6_tree_init(void)
{
    int vrf = 0;

    zlog_debug(FTM_DBG_L3, "%s:Initialize fibv6 tree.\n", __func__);

    for(vrf = 0; vrf <= L3VPN_SIZE; vrf++)
    {
		fibv6_tree[vrf] = ptree_init(IPV6_MASK_LEN);
    }

    return 0;
}

void ftm_fibv6_tree_delete(uint16_t vrf)
{
    zlog_debug(FTM_DBG_L3, "%s:To delete fibv6 tree, vrf=%d.\n", __func__, vrf);
	ptree_free(fibv6_tree[vrf]);
}

void ftm_fibv6_tree_clear(void)
{
    int vrf = 0;

    zlog_debug(FTM_DBG_L3, "%s:Empty fibv6 tree.\n", __func__);

    for(vrf = 0; vrf <= L3VPN_SIZE; vrf++)
    {
		ptree_free(fibv6_tree[vrf]);
    }
}

int ftm_fibv6_add(struct fibv6_entry *pfib)
{
    int ret = 0;
    struct ptree_node *node = NULL;
	struct fibv6_entry *pfib_new = NULL;
    void *pnhp = NULL;
	char ip_str[IPV6_ADDR_STRLEN];

    zlog_debug(FTM_DBG_L3, "%s:Add fibv6 tree node, ipaddr=%s masklen=%d vpn=%d nhp_index=%d nhp_type=%d.\n",
				__func__, inet_ntop(AF_INET6, pfib->ipaddr, ip_str, IPV6_ADDR_STRLEN), pfib->masklen,
				pfib->vpn, pfib->nhp.nhp_index, pfib->nhp.nhp_type);

    node = ptree_node_get(fibv6_tree[pfib->vpn], pfib->ipaddr, pfib->masklen);
    if(NULL == node)
    {
        zlog_err("%s:Failed to create fibv6 tree node!\n", __func__);
        return 1;
    }

    if(NULL != node->info)
    {
        zlog_debug(FTM_DBG_L3, "%s:The fibv6 tree node information is not empty, now update this node.\n", __func__);

        ptree_unlock_node(node);
        ret = ftm_fibv6_update((struct fibv6_entry *)node->info, pfib);
        return ret;
    }

    pnhp = nhp_lookup(pfib->nhp.nhp_index, pfib->nhp.nhp_type);
    if(NULL == pnhp)
    {
        zlog_err("%s:Failed to find nhp information, nhp_index=%d nhp_type=%d.\n",
                    __func__, pfib->nhp.nhp_index, pfib->nhp.nhp_type);
        node->lock = 0;
        node->info = NULL;
        ptree_node_delete(node);

        return 1;
    }

	pfib_new = (struct fibv6_entry *)XMALLOC(MTYPE_FIB_ENTRY, sizeof(struct fibv6_entry));
	if(NULL == pfib_new)
	{
		zlog_err("%s:Failed to allocate memory for fibv6 entry.\n", __func__);
		node->lock = 0;
		node->info = NULL;
		ptree_node_delete(node);

		return 1;
	}

	memcpy(pfib_new, pfib, sizeof(struct fibv6_entry));
    pfib_new->nhp.pnhp = pnhp;
    node->info = (void *)pfib_new;

	//subtype=1代表ipv6消息
    ret = ftm_msg_send_to_hal(pfib_new, sizeof(struct fibv6_entry), 1, IPC_TYPE_FIB, 1, IPC_OPCODE_ADD, 0);
    if(-1 == ret)
    {
        zlog_err("%s:Send fibv6 tree node add message to hal failure, ipaddr=%s masklen=%d vpn=%d\n",
                    __func__, inet_ntop(AF_INET6, pfib_new->ipaddr, ip_str, IPV6_ADDR_STRLEN), pfib_new->masklen, pfib_new->vpn);
        node->lock = 0;
        node->info = NULL;
        ptree_node_delete(node);
		XFREE(MTYPE_FIB_ENTRY, pfib_new);

        return 1;
    }

	fibv6_num += 1;

    return 0;
}

int ftm_fibv6_update(struct fibv6_entry *pfib_old, struct fibv6_entry *pfib_new)
{
    int ret = 0;
    void *pnhp_new = NULL;
	char ip_str[IPV6_ADDR_STRLEN];

    if(NULL == pfib_old || NULL == pfib_new)
    {
        zlog_err("%s:Abnormal function parameter.\n", __func__);
        return 1;
    }

    zlog_debug(FTM_DBG_L3, "%s:Update fibv6 tree node information, ipaddr=%s masklen=%d vpn=%d nhp_index=%d nhp_type=%d.\n",
                __func__, inet_ntop(AF_INET6, pfib_new->ipaddr, ip_str, IPV6_ADDR_STRLEN), pfib_new->masklen, pfib_new->vpn,
                pfib_new->nhp.nhp_index, pfib_new->nhp.nhp_type);

    pnhp_new = nhp_lookup(pfib_new->nhp.nhp_index, pfib_new->nhp.nhp_type);
    if(NULL == pnhp_new)
    {
        zlog_err("%s:Failed to find nhp information, nhp_index=%d nhp_type=%d.\n",
                    __func__, pfib_new->nhp.nhp_index, pfib_new->nhp.nhp_type);
        return 1;
    }

    pfib_new->nhp.pnhp = pnhp_new;

    ret = ftm_msg_send_to_hal(pfib_new, sizeof(struct fibv6_entry), 1, IPC_TYPE_FIB, 1, IPC_OPCODE_UPDATE, 0);
    if(-1 == ret)
    {
        zlog_err("%s:Send fibv6 tree node update message to hal failure, ipaddr=%s masklen=%d vpn=%d nhp_index=%d nhp_type=%d.\n",
                    __func__, inet_ntop(AF_INET6, pfib_new->ipaddr, ip_str, IPV6_ADDR_STRLEN), pfib_new->masklen, pfib_new->vpn,
                    pfib_new->nhp.nhp_index, pfib_new->nhp.nhp_type);
        return 1;
    }

    memcpy(&pfib_old->nhp, &pfib_new->nhp, sizeof(struct nhp_info));

    return 0;
}

int ftm_fibv6_delete(struct fibv6_entry *pfib)
{
    int ret = 0;
    struct ptree_node *node = NULL;
    struct fibv6_entry *pfib_info = NULL;
	char ip_str[IPV6_ADDR_STRLEN];

    zlog_debug(FTM_DBG_L3, "%s:To delete fibv6 tree node, ipaddr=%s masklen=%d vrf=%d.\n",
                __func__, inet_ntop(AF_INET6, pfib->ipaddr, ip_str, IPV6_ADDR_STRLEN), pfib->masklen, pfib->vpn);

	node = ptree_node_lookup(fibv6_tree[pfib->vpn], pfib->ipaddr, pfib->masklen);
    if(NULL == node)
    {
        zlog_err("%s:Failed to find fibv6 tree node, ipaddr=%s masklen=%d vrf=%d.\n",
                    __func__, inet_ntop(AF_INET6, pfib->ipaddr, ip_str, IPV6_ADDR_STRLEN), pfib->masklen, pfib->vpn);
		return 1;
    }

    if(NULL == node->info)
    {
        zlog_err("%s:The fibv6 tree node information is empty, ipaddr=%s masklen=%d vrf=%d.\n",
                    __func__, inet_ntop(AF_INET6, pfib->ipaddr, ip_str, IPV6_ADDR_STRLEN), pfib->masklen, pfib->vpn);
        return 1;
    }

    pfib_info = (struct fibv6_entry *)node->info;

    ret = ftm_msg_send_to_hal(pfib_info, sizeof(struct fibv6_entry), 1, IPC_TYPE_FIB, 1, IPC_OPCODE_DELETE, 0);
    if(-1 == ret)
    {
        zlog_err("%s:Send fibv6 tree node delete message to hal failure, ipaddr=%s masklen=%d vrf=%d.\n",
                    __func__, inet_ntop(AF_INET6, pfib_info->ipaddr, ip_str, IPV6_ADDR_STRLEN), pfib_info->masklen, pfib_info->vpn);
        return 1;
    }

    node->lock = 0;
    node->info = NULL;
    ptree_node_delete(node);
	XFREE(MTYPE_FIB_ENTRY, pfib_info);

	fibv6_num -= 1;

    return 0;
}

struct fibv6_entry *ftm_fibv6_lookup(struct ipv6_addr *paddr, uchar masklen, uint16_t vrf)
{
    struct ptree_node *node = NULL;
	char ip_str[IPV6_ADDR_STRLEN];

    zlog_debug(FTM_DBG_L3, "%s:To find fibv6 tree node information, ipaddr=%s masklen=%d vrf=%d.\n",
                __func__, inet_ntop(AF_INET6, paddr->ipv6, ip_str, IPV6_ADDR_STRLEN), masklen, vrf);

	if(vrf > 1024)
	{
		return NULL;
	}

	node = ptree_node_lookup(fibv6_tree[vrf], paddr->ipv6, masklen);
    if(!node)
    {
		return NULL;
    }

    ptree_unlock_node(node);

    return (struct fibv6_entry *)node->info;
}

struct fibv6_entry *ftm_fibv6_match(struct ipv6_addr *paddr, uchar masklen, uint16_t vrf)
{
    struct ptree_node *node = NULL;
    struct fibv6_entry *pfib = NULL;
	char ip_str[IPV6_ADDR_STRLEN];

    zlog_debug(FTM_DBG_L3, "%s:The fibv6 tree longest matching mask, ipaddr=%s masklen=%d vrf=%d.\n",
                __func__, inet_ntop(AF_INET6, paddr->ipv6, ip_str, IPV6_ADDR_STRLEN), masklen, vrf);

	if(vrf > 1024)
	{
		return NULL;
	}

	node = ptree_node_match(fibv6_tree[vrf], paddr->ipv6, masklen);
    if(NULL == node)
    {
		return NULL;
    }

    ptree_unlock_node(node);

    if(NULL == node->info)
    {
        return NULL;
    }

    pfib = (struct fibv6_entry *)node->info;

    return pfib;
}

/* 处理 fib 消息 */
int ftm_fib_msg(void *pdata, int data_len, int data_num, uint8_t subtype, enum IPC_OPCODE opcode)
{
	int i = 0;
	struct route_entry *proute = NULL;
	struct fib_entry *pfib = NULL;
	struct fib_entry fib;
	struct fibv6_entry *pfibv6 = NULL;
	struct fibv6_entry fibv6;
	char ip_str[IPV6_ADDR_STRLEN];

    if(NULL == pdata)
    {
        zlog_err("%s:Abnormal function parameter.\n", __func__);
        return 1;
    }

    zlog_debug(FTM_DBG_L3, "%s:Value of function parameters, data_len=%d data_num=%d subtype=%d opcode=%d.\n",
                __func__, data_len, data_num, subtype, opcode);

    for(i = 0; i < data_num; i++)
    {
		proute = (struct route_entry *)pdata;

        /* 处理 router-id */
        if (ROUTE_SUBTYPE_ROUTER_ID == subtype)
        {
            g_fib.router_id = *(uint32_t *)pdata;
            return 0;
        }

        if (ROUTE_SUBTYPE_ROUTERV6_ID == subtype)
        {
            memcpy(g_fib.routerv6_id.ipv6, pdata, IPV6_ADDR_LEN);
            return 0;
        }

		if(INET_FAMILY_IPV4 == proute->prefix.type)
		{
			memset(&fib, 0, sizeof(struct fib_entry));
			fib.ipaddr = proute->prefix.addr.ipv4;
			fib.masklen = proute->prefix.prefixlen;
			fib.vpn = proute->vpn;
			memcpy(&fib.nhp, &proute->nhpinfo, sizeof(struct nhp_info));

			zlog_debug(FTM_DBG_L3, "%s:Fib entry information, ipaddr=%#X masklen=%d vpn=%d.\n",
							__func__, fib.ipaddr, fib.masklen, fib.vpn);

			if(IPC_OPCODE_ADD == opcode)
			{
				ftm_fib_add(&fib);
			}
			else if(IPC_OPCODE_UPDATE == opcode)
			{
				pfib = ftm_fib_lookup(fib.ipaddr, fib.masklen, fib.vpn);
				if(NULL == pfib)
				{
					zlog_err("%s:Failed to lookup fib entry, ipaddr=%#X masklen=%d vpn=%d.\n",
								__func__, fib.ipaddr, fib.masklen, fib.vpn);
				}
				else
				{
					ftm_fib_update(pfib, &fib);
				}
			}
			else if(IPC_OPCODE_DELETE == opcode)
			{
				ftm_fib_delete(&fib);
			}
		}
		else if(INET_FAMILY_IPV6 == proute->prefix.type)
		{
			memset(&fibv6, 0, sizeof(struct fibv6_entry));
			memcpy(fibv6.ipaddr, proute->prefix.addr.ipv6, sizeof(fibv6.ipaddr));
			fibv6.masklen = proute->prefix.prefixlen;
			fibv6.vpn = proute->vpn;
			memcpy(&fibv6.nhp, &proute->nhpinfo, sizeof(struct nhp_info));

			zlog_debug(FTM_DBG_L3, "%s:Fibv6 entry information, ipaddr=%s masklen=%d vpn=%d.\n",
							__func__, inet_ntop(AF_INET6, fibv6.ipaddr, ip_str, IPV6_ADDR_STRLEN), fibv6.masklen, fibv6.vpn);

			if(IPC_OPCODE_ADD == opcode)
			{
				ftm_fibv6_add(&fibv6);
			}
			else if(IPC_OPCODE_UPDATE == opcode)
			{
				pfibv6 = ftm_fibv6_lookup((struct ipv6_addr *)fibv6.ipaddr, fibv6.masklen, fibv6.vpn);
				if(NULL == pfibv6)
				{
					zlog_err("%s:Failed to lookup fibv6 entry, ipaddr=%s masklen=%d vpn=%d.\n",
								__func__, inet_ntop(AF_INET6, fibv6.ipaddr, ip_str, IPV6_ADDR_STRLEN), fibv6.masklen, fibv6.vpn);
				}
				else
				{
					ftm_fibv6_update(pfibv6, &fibv6);
				}
			}
			else if(IPC_OPCODE_DELETE == opcode)
			{
				ftm_fibv6_delete(&fibv6);
			}
		}

		proute += sizeof(struct route_entry);
    }

	return 0;
}

