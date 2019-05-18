
#include <string.h>
#include <unistd.h>
#include <lib/memory.h>
#include <lib/vty.h>
#include <lib/log.h>
#include <lib/errcode.h>
#include <lib/zassert.h>
#include <lib/route_com.h>
#include <lib/hash1.h>
#include <lib/ifm_common.h>

#include "ldp_lsptrigger.h"
#include "ldp.h"
struct hash_table ldp_filter_tabel;

/**
* @brief      <+ hash key compute +>
* @param[in ] <+hash_key: hash key+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
static unsigned int ldp_filter_compute_hash(void *hash_key)
{
    uint32_t hash_value;

    hash_value = *((int *)hash_key);
    return hash_value;
}

/**
* @brief      <+ ldp filter compare +>
* @param[in ] <+none+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
static int ldp_filter_compare(void *item, void *hash_key)
{
    if ((NULL == item) || (NULL == hash_key))
    {
        return ERRNO_FAIL;
    }
    if (*((int *)hash_key) == *((int *)(((struct hash_bucket *)item)->hash_key)))
    {
        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}

/**
* @brief      <+ ldp filter hash tabel initialize +>
* @param[in ] <+size: 1024+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
void ldp_filter_table_init(unsigned int size)
{
    hios_hash_init(&ldp_filter_tabel, size, ldp_filter_compute_hash, ldp_filter_compare);
}

/**
* @brief      <+ LDP filter node create +>
* @param[in ] <+prefix:地址前缀+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
struct ldp_filter *ldp_filter_create(struct inet_prefix *prefix)
{
    struct ldp_filter *pfilter = NULL;
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "malloc for new ip prefix %s.\n", ldp_ipv4_to_str(prefix->addr.ipv4));
    pfilter = (struct ldp_filter *)XCALLOC(MTYPE_LDP_ENTRY, sizeof(struct ldp_filter));
    if (NULL == pfilter)
    {
        return NULL;
    }
    pfilter->key        = prefix->addr.ipv4;
    pfilter->prefix.type       = prefix->type;
    pfilter->prefix.prefixlen  = prefix->prefixlen;
    pfilter->prefix.addr.ipv4  = prefix->addr.ipv4;

    ldp_filter_add(pfilter);

    return pfilter;
}

/**
* @brief      <+LDP filter node add to hash table +>
* @param[in ] <+pfilter:前缀结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_filter_add(struct ldp_filter *pfilter)
{
    struct hash_bucket *pitem = NULL;
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "\n");

    if (ldp_filter_tabel.hash_size == ldp_filter_tabel.num_entries)
    {
        return ERRNO_OVERSIZE;
    }

    pitem = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
    if (NULL == pitem)
    {
        return ERRNO_MALLOC;
    }

    pitem->hash_key = &(pfilter->key);
    pitem->data = pfilter;

    hios_hash_add(&ldp_filter_tabel, pitem);

    return ERRNO_SUCCESS;

}

/**
* @brief      <+LDP filter node delete from the hash table +>
* @param[in ] <+pfilter:前缀结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_filter_del(struct ldp_filter *pfilter)
{
    struct hash_bucket *pitem = NULL;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "\n");
    pitem = hios_hash_find(&ldp_filter_tabel, &pfilter->key);
    if (NULL == pitem)
    {
        MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "\n");
        return ERRNO_SUCCESS;
    }

    hios_hash_delete(&ldp_filter_tabel, pitem);

    XFREE(MTYPE_LDP_ENTRY, pfilter);
    XFREE(MTYPE_HASH_BACKET, pitem);

    return ERRNO_SUCCESS;
}

/**
* @brief      <+Clear ldp filter hash table +>
* @param[in ] <+none+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
void ldp_filter_clear(void)
{
    struct ldp_filter   *pfilter      = NULL;
    struct hash_bucket  *pbucket      = NULL;
    struct hash_bucket  *pbucket_temp = NULL;
    int cursor = 0;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "\n");
    HASH_BUCKET_LOOPW(pbucket, cursor, ldp_filter_tabel)
    {
        pbucket_temp = pbucket;
        pbucket = pbucket->next;
        pfilter = (struct ldp_filter *)pbucket_temp->data;
        if (pfilter)
        {
            ldp_filter_del(pfilter);
        }
    }
}

/**
* @brief      <+ LDP filter node lookup by ip prefix +>
* @param[in ] <+prefix:前缀结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
struct ldp_filter *ldp_filter_lookup(struct inet_prefix *prefix)
{
    uint32_t key = 0;
    struct hash_bucket *pitem = NULL;

    if (NULL == prefix)
    {
        return NULL;
    }
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "\n");
    key = prefix->addr.ipv4;
    pitem = hios_hash_find(&ldp_filter_tabel, &key);
    if (NULL == pitem)
    {
        return NULL;
    }

    return (struct ldp_filter *)pitem->data;
}

/**
* @brief      <+ 判断本设备是否是某一条路由的EGRESS 节点 +>
* @param[in ] <+proute:路由结构+>
* @param[out] <+none+>
* @return     <+LDP_ROUTE_EGRESS_NODE_NO or LDP_ROUTE_EGRESS_NODE_YES+>
* @author     wus
* @date       2018/3/1
* @note       <+check if the lsr is the egress node  of the route , return LDP_ROUTE_EGRESS_NODE_YES.+>
*/
int is_ldp_route_egress_node(struct route_entry *proute)
{
    switch (gldp.policy)
    {
    case LABEL_POLICY_PREFIX:
        if (NULL != ldp_filter_lookup(&proute->prefix))
        {
            MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "a egress node.\n");
            return LDP_ROUTE_EGRESS_NODE_YES;
        }
        break;
    case LABEL_POLICY_ALL:
        if (((ROUTE_PROTO_CONNECT == proute->nhp[0].protocol)
                && ((IFM_TYPE_IS_LOOPBCK(proute->nhp[0].ifindex))
                    || (32 != proute->prefix.prefixlen)))
                || (((ROUTE_PROTO_STATIC == proute->nhp[0].protocol)
                     || (ROUTE_PROTO_OSPF == proute->nhp[0].protocol))
                    && (gldp.proxy_egress)))
        {
            MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "a egress node.\n");
            return LDP_ROUTE_EGRESS_NODE_YES;
        }
        break;
    case LABEL_POLICY_HOST:
        if ((ROUTE_PROTO_CONNECT == proute->nhp[0].protocol)
                && (IFM_TYPE_IS_LOOPBCK(proute->nhp[0].ifindex))
                && (32 == proute->prefix.prefixlen))
        {
            MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "a egress node.\n");
            return LDP_ROUTE_EGRESS_NODE_YES;
        }
        break;
    case LABEL_POLICY_NONE:
        break;

    default:
        break;
    }
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "not a egress node.\n");
    return LDP_ROUTE_EGRESS_NODE_NO;

}

/**
* @brief      <+Save ldp filter configuration +>
* @param[in ] <+vty:命令行结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
void ldp_filter_config_write(struct vty *vty)
{
    struct ldp_filter   *pfilter = NULL;
    struct hash_bucket  *pbucket = NULL;
    char buf[128]  = "";
    char dip[INET_ADDRSTRLEN]    = "";
    int cursor = 0;

    HASH_BUCKET_LOOP(pbucket, cursor, ldp_filter_tabel)
    {
        pfilter = (struct ldp_filter *)pbucket->data;
        if (pfilter)
        {
            inet_ipv4tostr(pfilter->prefix.addr.ipv4, dip);
            sprintf(buf, "%s%s/%d", "mpls ldp ip-prefix ", dip, pfilter->prefix.prefixlen);
            vty_out(vty, " %s%s", buf, VTY_NEWLINE);
        }
    }
}
