/**
 * @file      : mpls.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月7日 17:26:55
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <lib/command.h>
#include <lib/prefix.h>
#include <lib/errcode.h>
#include <lib/memtypes.h>
#include <lib/memory.h>
#include <lib/mpls_common.h>
#include <lib/log.h>
#include "mpls_main.h"
#include "mpls.h"
#include "lsp_cmd.h"
#include "lspm.h"
#include "lsp_fec.h"
#include "ldp/ldp_label.h"
#include "l3vpn_cmd.h"
#include "vpls_cmd.h"
#include "pw_cmd.h"
#include "labelm.h"
#include "ldp/ldp_lsptrigger.h"
#include "mpls_oam/mpls_oam.h"

struct mpls_service *mpls_service_create(struct mpls_service *pservice);
int mpls_service_add(struct mpls_service *pservice);
int mpls_service_del(uint32_t service_id);
struct mpls_service * mpls_service_lookup(uint32_t service_id);
void mpls_service_show(struct vty * vty);


/* 初始化 mpls node */
static struct cmd_node mpls_node =
{
    MPLS_NODE,
    "%s(config-mpls)# ",
    1
};

struct hash_table mpls_service_table;

static int mpls_global_config_write(struct vty *vty);
extern void mpls_ldp_remot_config_write(struct vty *vty);
extern int mpls_delete_local_ldp(void);
extern int mpls_delete_remot_ldp(void);
extern void mpls_service_config_write(struct vty * vty);
int mpls_service_get_bulk(struct mpls_service *pindex, uint32_t index_flag, struct mpls_service *pbuf);

/**
 * @brief      : lsr-id 生成
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月7日 17:28:44
 * @note       :
 */
void mpls_set_lsr_id(void)
{
    uint32_t         *prouter_id   = NULL;
    struct ipv6_addr *prouterv6_id = NULL;

    if (!ROUTE_IPV4_ADDR_EXIST(gmpls.lsr_id))
    {
        prouter_id = route_com_get_router_id(MODULE_ID_MPLS);
        if (NULL != prouter_id)
        {
            gmpls.lsr_id = *prouter_id;

            MPLS_IPC_SENDTO_FTM(&gmpls.lsr_id, IP_ADDR_LEN, 1, MODULE_ID_MPLS,
                            IPC_TYPE_LSP, IPC_OPCODE_ENABLE, LSP_SUBTYPE_LSR_ID, 0);
            mem_share_free_bydata(prouter_id, MODULE_ID_MPLS);
        }
    }

    if (!ROUTE_IPV6_ADDR_EXIST(gmpls.lsrv6_id.ipv6))
    {
        prouterv6_id = route_com_get_routerv6_id(MODULE_ID_MPLS);
        if (NULL != prouterv6_id)
        {
            memcpy(gmpls.lsrv6_id.ipv6, prouterv6_id->ipv6, IPV6_ADDR_LEN);

            MPLS_IPC_SENDTO_FTM(&gmpls.lsrv6_id.ipv6, IPV6_ADDR_LEN, 1, MODULE_ID_MPLS,
                        IPC_TYPE_LSP, IPC_OPCODE_ENABLE, LSP_SUBTYPE_LSRV6_ID, 0);
            mem_share_free_bydata(prouterv6_id, MODULE_ID_MPLS);
        }
    }

    return;
}


/*mpls debug cmd*/
const struct message mpls_dbg_name[] = {
        {.key = MPLS_DBG_BFD,	.str = "bfd"},
        {.key = MPLS_DBG_OAM,	.str = "oam"},
        {.key = MPLS_DBG_LSP,	.str = "lsp"},
        {.key = MPLS_DBG_L2VC,	.str = "l2vc"},
        {.key = MPLS_DBG_VPLS,	.str = "vpls"},
        {.key = MPLS_DBG_TUNNEL,.str = "tunnel"},
        {.key = MPLS_DBG_L3VPN,	.str = "l3vpn"},
        {.key = MPLS_DBG_LDP,	.str = "ldp"},
        {.key = MPLS_DBG_PING,	.str = "ping"},
};

DEFUN (mpls_debug_monitor,
	mpls_debug_monitor_cmd,
	"debug mpls (enable|disable) (bfd|oam|lsp|l2vc|vpls|tunnel|l3vpn|ldp|ping)",
	"Debug information to moniter\n"
	"Programe name mpls\n"
	"Enable debug\n"
	"Disable debug\n"
	"Debug bfd informations\n"
	"Debug oam informations\n"
    "Debug lsp informations\n"
    "Debug l2vc informations\n"
    "Debug vpls informations\n"
    "Debug tunnel informations\n"
    "Debug l3vpn informations\n"
    "Debug ldp informations\n"
    "Debug ping informations\n")
{
	uint32_t zlog_num;
	
	for(zlog_num = 0; zlog_num < array_size(mpls_dbg_name); zlog_num++)
	{
		if(strcmp(argv[1], mpls_dbg_name[zlog_num].str) == 0)
		{
			zlog_debug_set(vty, mpls_dbg_name[zlog_num].key, !strncmp(argv[0], "enable", 3));

			return CMD_SUCCESS;
		}
	}
	vty_out (vty, "No debug typd find %s", VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN (show_mpls_debug_monitor,
	show_mpls_debug_monitor_cmd,
	"show mpls debug",
	SHOW_STR
	"Mpls"
	"Debug status\n")
{
	uint32_t type_num;
	
	vty_out(vty, "debug type	status %s", VTY_NEWLINE);

	for(type_num = 0; type_num < array_size(mpls_dbg_name); ++type_num)
	{
		vty_out(vty, "%-15s    %-10s %s", mpls_dbg_name[type_num].str, 
			!!(vty->monitor & (1 << type_num)) ? "enable" : "disable", VTY_NEWLINE);
	}

	return CMD_SUCCESS;
}


/**
 * @brief      : 进入 mpls 节点命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 17:30:05
 * @note       :
 */
DEFUN(mpls,
    mpls_cmd,
    "mpls",
    MPLS_CLI_INFO)
{
    gmpls.enable = ENABLE;
    vty->node    = MPLS_NODE;

    return CMD_SUCCESS;
}


/**
 * @brief      : 配置 lsr-id 命令
 * @param[in ] : lsr-id A.B.C.D - lsr id
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 17:30:53
 * @note       :
 */
DEFUN(mpls_lsr_id,
    mpls_lsr_id_cmd,
    "mpls lsr-id A.B.C.D",
    MPLS_CLI_INFO)
{
    uint32_t lsr_id;
    int      ret;
    uint32_t old_lsrid = gmpls.lsr_id;

    lsr_id = inet_strtoipv4((char *)argv[0]);
    if (0 == lsr_id)
    {
        vty_out(vty, "Error: Invalid lsr-id !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (!inet_valid_network(lsr_id))
    {
        vty_out(vty, "Error: Invalid lsr-id !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (((old_lsrid != 0) && (old_lsrid != lsr_id)) && (ENABLE == gmpls.ldp_enable))
    {
        vty_out(vty, "The configurations of local ldp is cleared for the "
                    "lsr-id is changed! %s", VTY_NEWLINE);

        /* Delete all local ldp ,if gmpls.lsr_id moded */

        mpls_delete_local_ldp();

        mpls_delete_remot_ldp();

        gmpls.ldp_enable = DISABLE;
    }

    gmpls.lsr_id = lsr_id;

    ret = MPLS_IPC_SENDTO_FTM(&gmpls.lsr_id, IP_ADDR_LEN, 1, MODULE_ID_MPLS,
                        IPC_TYPE_LSP, IPC_OPCODE_ENABLE, LSP_SUBTYPE_LSR_ID, 0);
    if (ERRNO_SUCCESS != ret)
    {
        vty_out(vty, "Error: IPC send error !%s", VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 配置 IPv6 lsr-id 命令
 * @param[in ] : lsr-id X:X:X:X:X:X:X:X - lsr id
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 17:32:33
 * @note       :
 */
DEFUN(mpls_lsrv6_id,
    mpls_lsrv6_id_cmd,
    "mpls lsr-id X:X:X:X:X:X:X:X",
    MPLS_CLI_INFO)
{
    struct prefix_ipv6 lsrv6_id;
    int ret;

    ret = str2prefix_ipv6(argv[0], &lsrv6_id);
    if (0 == ret)
    {
        vty_out(vty, "Error: Invalid lsr-id !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* 未指定地址 */
    if (!ROUTE_IPV6_ADDR_EXIST(lsrv6_id.prefix.s6_addr))
    {
        vty_out(vty, "Error: Invalid lsr-id !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* loopback 地址 */
    if (IN6_IS_ADDR_LOOPBACK(&lsrv6_id.prefix))
    {
        vty_out(vty, "Error: Invalid lsr-id !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    memcpy(gmpls.lsrv6_id.ipv6, lsrv6_id.prefix.s6_addr, IPV6_ADDR_LEN);

    ret = MPLS_IPC_SENDTO_FTM(&gmpls.lsrv6_id.ipv6, IPV6_ADDR_LEN, 1, MODULE_ID_MPLS,
                    IPC_TYPE_LSP, IPC_OPCODE_ENABLE, LSP_SUBTYPE_LSRV6_ID, 0);
    if (ERRNO_SUCCESS != ret)
    {
        vty_out(vty, "Error: %s%s", errcode_get_string(ERRNO_IPC), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : ldp 使能命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 17:33:59
 * @note       :
 */
DEFUN(mpls_ldp_enable,
	mpls_ldp_enable_cmd,
    "mpls ldp enable",
	MPLS_CLI_INFO)
{
    MPLS_LSRID_CHECK(vty);

    if(ENABLE == gmpls.ldp_enable)
    {
        return CMD_SUCCESS;
    }

    gmpls.ldp_enable = ENABLE;

    ldp_pkt_register(); // 收报注册

    /* 增加pw的纠错能力 如第一次协商失败，在必要参数vcid mtu控制字一致情况下，可再协商三次 */
    //gldp.ppw_correct = thread_add_timer(mpls_master, ldp_sess_pws_recorect_timer, NULL, LDP_MARTINI_PW_RECORRECT_TIME);

    return CMD_SUCCESS;
}


/**
 * @brief      : 去使能 ldp 命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 17:34:53
 * @note       :
 */
DEFUN(no_mpls_ldp_enable,
	no_mpls_ldp_enable_cmd,
    "no mpls ldp enable",
	MPLS_CLI_INFO)
{
    MPLS_LSRID_CHECK(vty);

    if(DISABLE == gmpls.ldp_enable)
    {
        return CMD_SUCCESS;
    }

    gmpls.ldp_enable = DISABLE;
    if(NULL != gldp.ppw_correct)
    {
        //THREAD_OFF(gldp.ppw_correct);
    }

    mpls_delete_local_ldp();
    mpls_delete_remot_ldp();

    return CMD_SUCCESS;
}


/**
 * @brief      : 显示 mpls 节点配置命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 18:32:41
 * @note       :
 */
DEFUN(show_this_mpls,
    show_this_mpls_cmd,
    "show this",
    SHOW_STR
    "This\n")
{
    vty_out(vty, "#%s", VTY_NEWLINE);

    mpls_global_config_write(vty);

    vty_out(vty, " #%s", VTY_NEWLINE);

    mpls_l3vpn_config_show_this(vty);

    vty_out(vty, " #%s", VTY_NEWLINE);

    mpls_vsi_config_show_this(vty);

    vty_out(vty, " #%s", VTY_NEWLINE);

    mpls_pw_config_show_this(vty);

    vty_out(vty, "#%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}


static unsigned int mpls_service_compute_hash(void *hash_key)
{

    return *(int*)hash_key;
}

/**
* @brief      <+none+>
* @param[in ] <+none+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
static int mpls_service_compare(void *item, void *hash_key)
{
    uint32_t *pkey = NULL;
    uint32_t *pkey_old = NULL;

    if ((NULL == item) || (NULL == hash_key))
    {
        return ERRNO_FAIL;
    }

    pkey = (uint32_t *)hash_key;
    pkey_old = (uint32_t *)(((struct hash_bucket *)item)->hash_key);
    if (NULL == pkey_old)
    {
        return ERRNO_FAIL;
    }

    if (*pkey == *pkey_old)
    {
        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}

/**
* @brief      <+none+>
* @param[in ] <+none+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
void mpls_service_table_init(unsigned int size)
{
    hios_hash_init(&mpls_service_table, size, mpls_service_compute_hash, mpls_service_compare);
}

struct mpls_service *mpls_service_create(struct mpls_service *pservice)
{
    struct mpls_service *p = NULL;
    p = (struct mpls_service *)XCALLOC(MTYPE_LDP_ENTRY, sizeof(struct mpls_service));
    if(NULL == p)
    {
        return NULL;
    }
    memcpy(p, pservice, sizeof(struct mpls_service));
    if(ERRNO_SUCCESS != mpls_service_add(p))
    {
        XFREE(MTYPE_LDP_ENTRY, p);
        return NULL;
    }
    return p;
}

int mpls_service_add(struct mpls_service *pservice)
{
    struct hash_bucket *pitem = NULL;

    if (mpls_service_table.hash_size == mpls_service_table.num_entries)
    {
        MPLS_LOG_ERROR("add ldp session to hash failed for oversize\n");
        return ERRNO_OVERSIZE;
    }

    pitem = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
    if (NULL == pitem)
    {
        MPLS_LOG_ERROR("add ldp session to hash failed for malloc hash bucket failed\n");
        return ERRNO_MALLOC;
    }
    if(NULL == pitem->data)
    {
        pitem->hash_key = &(pservice->service_id);
        pitem->data = pservice;

        hios_hash_add(&mpls_service_table, pitem);
    }
    else
    {
        MPLS_LOG_ERROR("add ldp session to hash failed for get hashbacket data != NULL\n");
    }

    return ERRNO_SUCCESS;

}

int mpls_service_del(uint32_t service_id)
{
    struct hash_bucket *pitem = NULL;
    struct mpls_service *pservice = NULL;

    pitem = hios_hash_find(&mpls_service_table, &service_id);
    if ((NULL == pitem) || (NULL == pitem->data))
    {
        MPLS_LOG_ERROR("error delete ldp session for find pitem data failed\n");
        return ERRNO_NOT_FOUND;
    }
    pservice = pitem->data;

    hios_hash_delete(&mpls_service_table, pitem);

    XFREE(MTYPE_LDP_ENTRY, pservice);
    XFREE(MTYPE_HASH_BACKET, pitem);

    return ERRNO_SUCCESS;

}

struct mpls_service * mpls_service_lookup(uint32_t service_id)
{
    struct hash_bucket *pitem = NULL;
    struct mpls_service *pservice = NULL;

    pitem = hios_hash_find(&mpls_service_table, &service_id);
    if ((NULL == pitem) || (NULL == pitem->data))
    {
        MPLS_LOG_ERROR("error delete ldp session for find pitem data failed\n");
        return NULL;
    }
    
    pservice = pitem->data;
    return pservice;
}

void mpls_service_config_write(struct vty * vty)
{
    struct mpls_service *pservice   = NULL;
    struct hash_bucket  *pbucket    = NULL;
    struct hash_bucket  *pbucket_temp = NULL;
    int cursor = 0;

    HASH_BUCKET_LOOPW(pbucket, cursor, mpls_service_table)
    {
        pbucket_temp = pbucket;
        if(NULL == pbucket_temp)
        {
           continue;
        }
        pbucket = pbucket->next;
        pservice = (struct mpls_service *)pbucket_temp->data;
        if (pservice)
        {
            if(ifm_get_ifindex_by_name2(pservice->root_port))
            {
                vty_out(vty, " mpls-service service-id %d service-name %s service-type %s service-vpn %s %d root-port %s%s", 
                    pservice->service_id, pservice->name, ((pservice->service_type == E_TREE) ? "e-tree" :((pservice->service_type == E_LAN) ? "e-lan" : "e-line")),
                    pservice->vpn_type == MPLS_VPN_TYPE_VPLS ? "vsi" : "vc-id", pservice->vpn_id, pservice->root_port, VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, " mpls-service service-id %d service-name %s service-type %s service-vpn %s %d root-port pw-name %s%s", 
                    pservice->service_id, pservice->name, ((pservice->service_type == E_TREE) ? "e-tree" :((pservice->service_type == E_LAN) ? "e-lan" : "e-line")),
                    pservice->vpn_type == MPLS_VPN_TYPE_VPLS ? "vsi" : "vc-id", pservice->vpn_id, pservice->root_port, VTY_NEWLINE);
            }
        }
    }

}

void mpls_service_show(struct vty * vty)
{
    struct mpls_service *pservice   = NULL;
    struct hash_bucket  *pbucket    = NULL;
    struct hash_bucket  *pbucket_temp = NULL;
    int cursor = 0;

    vty_out(vty, "%-15s%-15s%-15s%-8s%-8s%-15s%s", 
        "Service ID", "Service Name" ,"Service Type", "VC-ID", "VPLS-ID", "Root-Port", VTY_NEWLINE);
    vty_out(vty, "%s%s", "-----------------------------------------------------------------------", VTY_NEWLINE);

    HASH_BUCKET_LOOPW(pbucket, cursor, mpls_service_table)
    {
        pbucket_temp = pbucket;
        if(NULL == pbucket_temp)
        {
            continue;
        }
        pbucket = pbucket->next;
        pservice = (struct mpls_service *)pbucket_temp->data;
        if (pservice)
        {
            vty_out(vty, "%-15d%-15s%-15s%-8d%-8d%-15s%s", pservice->service_id, pservice->name, 
                ((pservice->service_type == E_TREE) ? "e-tree" :((pservice->service_type == E_LAN) ? "e-lan" : "e-line")), 
                ((pservice->vpn_type == MPLS_VPN_TYPE_VC) ? pservice->vpn_id : 0), ((pservice->vpn_type == MPLS_VPN_TYPE_VPLS) ? pservice->vpn_id : 0),
                (char *)pservice->root_port, VTY_NEWLINE);
        }
    }

}

int mpls_service_get_bulk(struct mpls_service *pindex, uint32_t index_flag, struct mpls_service *pbuf)
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct mpls_service *pservice   = NULL;
    int msg_num  = IPC_MSG_LEN / sizeof(struct mpls_service);
    int cursor;
    int data_num = 0;

    if (0 == index_flag)
    {
        HASH_BUCKET_LOOP(pbucket, cursor, mpls_service_table)
        {
            pservice = (struct mpls_service *)pbucket->data;
            if (pservice == NULL)
            {
                continue;
            }
            memcpy(&pbuf[data_num], pservice, sizeof(struct mpls_service));
            if (++data_num == msg_num)
            {
                MPLS_LOG_DEBUG("data_num %d\n", data_num);
                return data_num;
            }
        }
    }
    else
    {
        pbucket = hios_hash_find(&mpls_service_table, &pindex->service_id);
        if (pbucket)
        {
            for (data_num = 0; data_num < msg_num; data_num++)
            {
                pnext = hios_hash_next_cursor(&mpls_service_table, pbucket);
                if ((NULL == pnext) || (NULL == pnext->data))
                {
                    break;
                }
                pservice = (struct mpls_service *)pnext->data;
                memcpy(&pbuf[data_num], pservice, sizeof(struct mpls_service));
                pbucket = pnext;
            }
        }
        else
        {

        }
    }
    MPLS_LOG_DEBUG("data_num %d\n", data_num);
    return data_num;

}

DEFUN (mpls_serive_id,
     mpls_service_id_cmd,
     "mpls-service service-id <1-4294967295> service-name NAME service-type (e-line|e-lan|e-tree) service-vpn {vc-id <1-4294967295> | vsi <1-1024>} "
     "root-port { ethernet USP | gigabitethernet USP | xgigabitethernet USP | tdm USP | trunk TRUNK}",

    MPLS_CLI_INFO)
{
    u_int32_t service_id = 0;
    struct mpls_service service_node;
    struct mpls_service *pservice = NULL;
    uint32_t ifindex = 0;

    VTY_GET_INTEGER_RANGE("serviec-id", service_id, argv[0], 1, 4294967295UL);

    service_node.service_id = service_id;
    
    if(NULL != argv[1])
    {
        strncpy(service_node.name, argv[1], NAME_STRING_LEN);
    }

    // service-type
    if(strncmp(argv[2], "e-line", 6) == 0)
    {
        service_node.service_type = E_LINE;
    }
    else if (strncmp(argv[2], "e-lan", 5) == 0)
    {
        service_node.service_type = E_LAN;
    }
    else if (strncmp(argv[2], "e-tree", 6) == 0)
    {
        service_node.service_type = E_TREE;
    }

    // service-vpn
    if (NULL != argv[3])
    {
        VTY_GET_INTEGER_RANGE ("vc-id", service_node.vpn_id, argv[3], 1, 4294967295UL);
        service_node.vpn_type = MPLS_VPN_TYPE_VC;

    }
    else if (NULL != argv[4])
    {
        VTY_GET_INTEGER_RANGE ("VPLS Identifier", service_node.vpn_id, argv[4], 1, 1024);
        service_node.vpn_type = MPLS_VPN_TYPE_VPLS;

    }

    // root port
    if (NULL != argv[5])
    {
        ifindex = ifm_get_ifindex_by_name("ethernet", argv[5]);
        if (0 == ifindex)
        {
            vty_error_out(vty, "Interface ethernet %s is Invalid.%s", argv[5], VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    else if (NULL != argv[6])
    {
        ifindex = ifm_get_ifindex_by_name("gigabitethernet", argv[6]);
        if (0 == ifindex)
        {
            vty_error_out(vty, "Interface gigabitethernet %s is Invalid.%s", argv[6], VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    else if (NULL != argv[7])
    {
        ifindex = ifm_get_ifindex_by_name("xgigabitethernet", argv[7]);
        if (0 == ifindex)
        {
            vty_error_out(vty, "Interface xgigabitethernet %s is Invalid.%s", argv[7], VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    else if (NULL != argv[8])
    {
        ifindex = ifm_get_ifindex_by_name("tdm", argv[8]);
        if (0 == ifindex)
        {
            vty_error_out(vty, "Interface tdm %s is Invalid.%s", argv[8], VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    else if (NULL != argv[9])
    {
        ifindex = ifm_get_ifindex_by_name("trunk", argv[9]);
        if (0 == ifindex)
        {
            vty_error_out(vty, "Interface trunk %s is Invalid.%s", argv[9], VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    ifm_get_name_by_ifindex(ifindex, service_node.root_port);

    pservice = mpls_service_lookup(service_id);
    if(pservice)
    {
        memcpy(pservice, &service_node, sizeof(struct mpls_service));
    }
    else
    {
        mpls_service_create(&service_node);
    }

    return CMD_SUCCESS;
}
     
DEFUN (mpls_serive_id2,
     mpls_service_id_cmd2,
     "mpls-service service-id <1-4294967295> service-name NAME service-type (e-line|e-lan|e-tree) service-vpn {vc-id <1-4294967295> | vsi <1-1024>} "
     "root-port pw-name NAME",

    MPLS_CLI_INFO)
{
    u_int32_t service_id = 0;
    struct mpls_service service_node;
    struct mpls_service *pservice = NULL;
    
    VTY_GET_INTEGER_RANGE("serviec-id", service_id, argv[0], 1, 4294967295UL);

    service_node.service_id = service_id;
    
    if(NULL != argv[1])
    {
        strncpy(service_node.name, argv[1], NAME_STRING_LEN);
    }

    // service-type
    if(strncmp(argv[2], "e-line", 6) == 0)
    {
        service_node.service_type = E_LINE;
    }
    else if (strncmp(argv[2], "e-lan", 5) == 0)
    {
        service_node.service_type = E_LAN;
    }
    else if (strncmp(argv[2], "e-tree", 6) == 0)
    {
        service_node.service_type = E_TREE;
    }

    // service-vpn
    if (NULL != argv[3])
    {
        VTY_GET_INTEGER_RANGE ("vc-id", service_node.vpn_id, argv[3], 1, 4294967295UL);
        service_node.vpn_type = MPLS_VPN_TYPE_VC;

    }
    else if (NULL != argv[4])
    {
        VTY_GET_INTEGER_RANGE ("VPLS Identifier", service_node.vpn_id, argv[4], 1, 1024);
        service_node.vpn_type = MPLS_VPN_TYPE_VPLS;

    }

    // root port
    if (NULL != argv[5])
    {
        strncpy(service_node.root_port, argv[5], NAME_STRING_LEN);
    }

    pservice = mpls_service_lookup(service_node.service_id);
    if(pservice)
    {
        memcpy(pservice, &service_node, sizeof(struct mpls_service));
    }
    else
    {
        mpls_service_create(&service_node);
    }

    return CMD_SUCCESS;
}



DEFUN (no_mpls_serive_id,
     no_mpls_serive_id_cmd,
     "no mpls-service service-id ID",
     MPLS_CLI_INFO)
{
    struct mpls_service *pservice = NULL;
    u_int32_t service_id = 0;
    
    VTY_GET_INTEGER_RANGE ("service-id", service_id, argv[0], 1, 4294967295UL);

    pservice = mpls_service_lookup(service_id);
    if(pservice)
    {
        mpls_service_del(service_id);
    }
    
    return CMD_SUCCESS;
}

DEFUN (show_mpls_service,
    show_mpls_service_cmd,
    "show mpls-service",
    MPLS_CLI_INFO)
{
    mpls_service_show(vty);
    return CMD_SUCCESS;
}


    
/**
 * @brief      : mpls 节点配置管理
 * @param[in ] : vty - vty 全局结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 18:35:05
 * @note       :
 */
static int mpls_global_config_write(struct vty *vty)
{
    char lsr_id[INET_ADDRSTRLEN]    = "";
    char lsrv6_id[INET6_ADDRSTRLEN] = "";

    if (ENABLE == gmpls.enable)
    {
        vty_out(vty, "mpls%s", VTY_NEWLINE);
    }

    if (0 != gmpls.lsr_id)
    {
        vty_out(vty, " mpls lsr-id %s%s", inet_ipv4tostr(gmpls.lsr_id, lsr_id), VTY_NEWLINE);
    }

    if (ROUTE_IPV6_ADDR_EXIST(gmpls.lsrv6_id.ipv6))
    {
        inet_ntop(AF_INET6, gmpls.lsrv6_id.ipv6, lsrv6_id, INET6_ADDRSTRLEN);

        vty_out(vty, " mpls lsr-id %s%s", lsrv6_id, VTY_NEWLINE);
    }


    mpls_service_config_write(vty);

    mpls_static_lsp_config_write(vty);

    ldp_filter_config_write(vty);

    /* ldp global conf write */
	if (ENABLE == gmpls.ldp_enable)
    {
    	vty_out(vty, " mpls ldp enable%s", VTY_NEWLINE);

        if (gldp.bfd_enable != MPLS_LDP_BFD_DISBALE)
        {
            vty_out(vty, " ldp bfd enable%s", VTY_NEWLINE);
        }

        if (gldp.advertise != LABEL_ADVERTISE_DU)
        {
            vty_out(vty, " mpls ldp advertise dod%s", VTY_NEWLINE);
        }

        if (gldp.policy != LABEL_POLICY_HOST)
        {
            if (gldp.policy == LABEL_POLICY_PREFIX)
            {
                vty_out(vty, " ldp label-policy %s%s","ip-prefix", VTY_NEWLINE);
            }
            else if (gldp.policy == LABEL_POLICY_ALL)
            {
                vty_out(vty, " ldp label-policy %s%s","all", VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, " ldp label-policy %s%s","none", VTY_NEWLINE);
            }
        }

        if (LDP_HELLO_INTERNAL_DEFAULT != gldp.hello_internal)
        {
            vty_out(vty, " mpls ldp hello %d%s", gldp.hello_internal, VTY_NEWLINE);
        }

        if (LDP_KEEP_ALIVE_TIME_DEFAULT != gldp.keepalive_internal)
        {
            vty_out(vty, " mpls ldp keepalive %d%s", gldp.keepalive_internal, VTY_NEWLINE);
        }

            mpls_ldp_remot_config_write(vty);
    }

	if(gmplsoam.g_chtype_enable == 1)
	{
		if(gmplsoam.global_chtype == 0x8902)
		{
			vty_out(vty, " oam global channel-type 0x8902%s", VTY_NEWLINE);
		}
		else if(gmplsoam.global_chtype == 0x7ffa)
		{
			vty_out(vty, " oam global channel-type 0x7FFA%s", VTY_NEWLINE);
		}
	}

    return CMD_SUCCESS;
}


/**
 * @brief      : mpls 节点下命令初始化
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月7日 18:36:21
 * @note       :
 */
void mpls_global_cmd_init(void)
{
    install_node(&mpls_node, mpls_global_config_write);
    install_default(MPLS_NODE);

    install_element (CONFIG_NODE, &mpls_cmd, CMD_SYNC);

    install_element (MPLS_NODE, &show_this_mpls_cmd, CMD_LOCAL);
    install_element (MPLS_NODE, &mpls_lsr_id_cmd, CMD_SYNC);
    install_element (MPLS_NODE, &mpls_lsrv6_id_cmd, CMD_SYNC);
	install_element (MPLS_NODE, &mpls_ldp_enable_cmd, CMD_SYNC);
	install_element (MPLS_NODE, &no_mpls_ldp_enable_cmd, CMD_SYNC);

    install_element (MPLS_NODE, &mpls_service_id_cmd, CMD_SYNC);
    install_element (MPLS_NODE, &mpls_service_id_cmd2, CMD_SYNC);
    install_element (MPLS_NODE, &no_mpls_serive_id_cmd, CMD_SYNC);
    install_element (MPLS_NODE, &show_mpls_service_cmd, CMD_LOCAL);
    install_element (CONFIG_NODE, &show_mpls_service_cmd, CMD_LOCAL);

    install_element (CONFIG_NODE, &mpls_debug_monitor_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_mpls_debug_monitor_cmd, CMD_LOCAL);
}


