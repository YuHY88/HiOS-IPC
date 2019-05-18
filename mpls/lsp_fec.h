/*
*       manage the lsp fec table
*
*/


#ifndef HIOS_LSP_FEC_H
#define HIOS_LSP_FEC_H

#include <lib/ptree.h>
#include <lib/inet_ip.h>
#include <lib/types.h>
#include <lib/linklist.h>
#include <lib/mpls_common.h>

extern struct ptree *lsp_fec_tree;

enum LDP_EGRESS_TYPE
{
    LDP_EGRESS_HOST = 1,    /* 本机直连路由需要下发EGRESS LSP */
    LDP_EGRESS_PROXY,       /* 本机需要代理EGRESS LSP 的需要下发EGRESS LSP */
    LDP_EGRESS_NONE         /* LDP 学习到的不下发EGRESS LSP */
};

/* lsp 的下一跳数据结构 */
struct lsp_nhp_t
{
    uint32_t         peer_lsrid;         /* down stream ldp session identifier */
    struct inet_addr nexthop;     /* 分配出标签的 lsrid */
    enum LSP_TYPE    type;
    uint32_t         outlabel;    /* lsr 邻居分配的出标签 */
    uint32_t         ingress_lsp; /* 出标签生成的 ingress lsp */
    uint32_t         transit_lsp; /* 出标签和入标签生成的 transit lsp*/
    uint8_t          distance;    /* lsp 路由的 metric */
    uint8_t          pad[3];
    uint32_t         frr_master;
    uint32_t         frr_index;
    void            *pnhp_frr;
};
/* lsp 的 fec 数据结构 */
struct lsp_fec_t
{
    struct inet_prefix prefix;      /* 路由前缀 */	
    struct inet_addr   nexthop;     /* 路由的 nexthop 或者 local lsrid */
    uint32_t           inlabel;     /* 本机分配的入标签 */
    uint32_t           outlabel;    /* nhp 使用的出标签*/
    struct list        nhplist;     /* 存储所有邻居分配的出标签信息，struct lsp_nhp_t */
    uint32_t           egress_lsp;  /* 入标签生成的 egress lsp */
    uint8_t            pad[3];
    uint8_t            is_connect;  /* 1: 是本机的直连路由 */
    enum LDP_EGRESS_TYPE egress_type;
};



/* fec func */
void lsp_fec_tree_init(unsigned int size);
void lsp_fec_tree_clear(void);

struct lsp_fec_t *lsp_fec_create(struct inet_prefix *pprefix, uint32_t nexthop, uint32_t flag);
struct lsp_fec_t * lsp_fec_add(struct lsp_fec_t *pfec); /* 添加一条 fec */
int lsp_fec_delete(struct lsp_fec_t *pfec); /* 删除一条 fec */
struct lsp_fec_t *lsp_fec_lookup(struct inet_prefix *pprefix); /* 精确查找一条 fec */
struct lsp_fec_t *lsp_fec_match(struct inet_prefix *pprefix);  /* 最长匹配查找一条 fec */

int lsp_fec_add_nhp(struct lsp_fec_t *pfec, struct lsp_nhp_t *pnhp);
int lsp_fec_delete_nhp(struct lsp_fec_t *pfec, struct lsp_nhp_t *pnhp);


int lsp_fec_add_com_nhp(struct lsp_fec_t *pfec, struct lsp_nhp_t *pnhp);  /* 指定 fec 添加一个 nhp */
struct lsp_nhp_t *lsp_fec_lookup_nhp(struct list *nhplist, struct lsp_nhp_t *pnhp);/* 指定 fec 查找一个 nhp */
int lsp_fec_del_com_nhp(struct lsp_fec_t *pfec, struct lsp_nhp_t *pnhp);/* 指定 fec 删除一个 nhp */
int lsp_fec_clear_nhp(struct lsp_fec_t *pfec); /* 删除 fec 的 nhplist */

int lsp_fec_add_frr(struct lsp_fec_t *pfec, struct lsp_nhp_t *pnhp);
int lsp_fec_del_frr(struct lsp_fec_t *pfec, struct lsp_nhp_t *pnhp);

int lsp_fec_send_frr_ftm(struct ldp_frr_t *ldp_frr, int opcode);
int lsp_fec_add_frr_nhp(struct lsp_fec_t *pfec, struct lsp_nhp_t *pnhp);
int lsp_fec_del_frr_nhp(struct lsp_fec_t *pfec, struct lsp_nhp_t *pnhp);

#endif

