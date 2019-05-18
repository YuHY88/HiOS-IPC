/**
 * @file      : l2vpn_cmd.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年4月25日 14:13:53
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <lib/ifm_common.h>
#include <lib/command.h>
#include <lib/ether.h>
#include <lib/prefix.h>
#include <lib/errcode.h>
#include "mpls_main.h"
#include "mpls.h"
#include "labelm.h"
#include "lsp_static.h"
#include "lspm.h"
#include "pw.h"
#include "l2vpn_cmd_h3c.h"
#include "l2vpn_h3c.h"
#include "bfd/bfd_session.h"

extern int bfd_bind_for_lsp_pw_h3c(enum BFD_TYPE type, struct nni_info *nni);
extern int bfd_unbind_for_lsp_pw_h3c(uint16_t sess_id);
extern int pw_class_change_bfd_template(struct pw_class *pclass);
extern int ifm_unset_subif_l2 ( uint32_t ifindex , int module_id );
static struct cmd_node xc_group_node =
{
    XCONNECT_GROUP_NODE,
    "%s(xcg)# ",
    1
};


static struct cmd_node connection_node =
{
    CONNECTION_NODE,
    "%s(xcg)# ",
    1
};


static struct cmd_node pw_class_node =
{
    PW_CLASS_NODE,
    "%s(pw-class)# ",
    1
};


static struct cmd_node service_instance_node =
{
    SERVICE_INSTANCE_NODE,
    "%s(srv)# ",
    1
};


static struct cmd_node pw_master_node =
{
    PW_MASTER_NODE,
    "%s(xcg)# ",
    1
};


static struct cmd_node pw_backup_node =
{
    PW_BACKUP_NODE,
    "%s(xcg)# ",
    1
};


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月25日 14:55:13
 * @note       :
 */
DEFUN(l2vpn_enable_h3c,
    l2vpn_enable_cmd_h3c,
    "l2vpn enable",
    "Layer 2 Virtual Private Network (L2VPN) module\n"
    "Enable L2VPN\n")
{


    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月25日 14:55:23
 * @note       :
 */
DEFUN(no_l2vpn_enable_h3c,
    no_l2vpn_enable_cmd_h3c,
    "no l2vpn enable",
    NO_STR
    "Layer 2 Virtual Private Network (L2VPN) module\n"
    "Enable L2VPN\n")
{


    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月25日 14:57:34
 * @note       :
 */
DEFUN(l2vpn_xconnect_group_h3c,
    l2vpn_xconnect_group_cmd_h3c,
    "xconnect-group STRING<1-31>",
    "Configure an xconnect-group\n"
    "Xconnect-group name (Hyphen '-' not permitted)\n")
{
    struct xc_group *pgroup  = NULL;
    char            *pprompt = NULL;
    int ret = ERRNO_SUCCESS;

    pgroup = xc_group_lookup((uchar *)argv[0]);
    if (NULL == pgroup)
    {
        pgroup = xc_group_create();
        if (NULL == pgroup)
        {
            vty_error_out(vty, "Memory alloc failed !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        memcpy(pgroup->name, argv[0], strlen(argv[0]));

        ret = xc_group_add(pgroup);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

            xc_group_free(pgroup);

            return CMD_WARNING;
        }
    }

    vty->index = pgroup;
    vty->node  = XCONNECT_GROUP_NODE;

    pprompt = vty->change_prompt;
    if (NULL != pprompt)
    {
        snprintf(pprompt, VTY_BUFSIZ, "%%s(xcg-%s)# ", argv[0]);
    }

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月25日 16:06:16
 * @note       :
 */
DEFUN(no_l2vpn_xconnect_group_h3c,
    no_l2vpn_xconnect_group_cmd_h3c,
    "no xconnect-group STRING<1-31>",
    NO_STR
    "Configure an xconnect-group\n"
    "Xconnect-group name (Hyphen '-' not permitted)\n")
{
    struct xc_group *pgroup = NULL;

    pgroup = xc_group_lookup((uchar *)argv[0]);
    if (NULL == pgroup)
    {
        return CMD_SUCCESS;
    }

    xc_group_delete_connection(&pgroup->cont_list);

    xc_group_delete((uchar *)argv[0]);

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月26日 14:44:41
 * @note       :
 */
DEFUN(l2vpn_connection_h3c,
    l2vpn_connection_cmd_h3c,
    "connection STRING<1-19>",
    "Configure a cross connection\n"
    "Connection name (Hyphen '-' not permitted)\n")
{
    struct xc_group   *pgroup  = (struct xc_group *)vty->index;
    struct cont_entry *pcont   = NULL;
    struct list       *plist   = &pgroup->cont_list;
    int ret = ERRNO_SUCCESS;

    if (plist->count > CONNECTION_MAX_NUM)
    {
        vty_warning_out(vty, "The number of connections in the cross-connect group "
                            "has reached the limit.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    pcont = connection_lookup(plist, (uchar *)argv[0]);
    if (NULL == pcont)
    {
        pcont = connection_create();
        if (NULL == pcont)
        {
            vty_error_out(vty, "Memory alloc failed !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        memcpy(pcont->name, argv[0], strlen(argv[0]));

        ret = connection_add(plist, pcont);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

            connection_free(pcont);

            return CMD_WARNING;
        }
    }

    vty->index_sub = pcont;
    vty->node      = CONNECTION_NODE;

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月26日 14:45:52
 * @note       :
 */
DEFUN(no_l2vpn_connection_h3c,
    no_l2vpn_connection_cmd_h3c,
    "no connection STRING<1-19>",
    NO_STR
    "Configure a cross connection\n"
    "Connection name (Hyphen '-' not permitted)\n")
{
    struct xc_group   *pgroup = (struct xc_group *)vty->index;
    struct list       *plist  = &pgroup->cont_list;
    struct cont_entry *pcont  = NULL;

    pcont = connection_lookup(plist, (uchar *)argv[0]);
    if (NULL == pcont)
    {
        return CMD_SUCCESS;
    }

    connection_delete_uni(pcont);
    connection_delete_nni_backup(pcont);
    connection_delete_nni_master(pcont);
    connection_delete(plist, pcont);

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月26日 15:46:21
 * @note       :
 */
DEFUN(l2vpn_ac_interface_h3c,
    l2vpn_ac_interface_cmd_h3c,
    "ac interface {ethernet USP | gigabitethernet USP | xgigabitethernet USP} "
    "service-instance <1-4096> {access-mode (ethernet | vlan)}",
    "Configure an attachment circuit\n"
    "Specify the AC interface\n"
    CLI_INTERFACE_ETHERNET_STR
    CLI_INTERFACE_ETHERNET_VHELP_STR
    CLI_INTERFACE_GIGABIT_ETHERNET_STR
    CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
    CLI_INTERFACE_XGIGABIT_ETHERNET_STR
    CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
    "Specify a service instance\n"
    "Service instance ID\n"
    "Specify the access mode\n"
    "Use Ethernet access mode\n"
    "Use Vlan access mode\n")
{
    struct cont_entry *pcont     = (struct cont_entry *)vty->index_sub;
    struct mpls_if    *pif       = NULL;
    const char        *pintf     = NULL;
    const char        *pintf_num = NULL;
    struct serv_ins   *psi       = NULL;
    struct serv_ins    si;
    uint32_t ifindex = 0;

    if (FLAG_ISSET(pcont->flag, UNI_FLAG))
    {
        vty_error_out(vty, "The number of ACs or PWs in the connection has "
                            "reached the limit.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (NULL != argv[0])
    {
        pintf = "ethernet";
        pintf_num = argv[0];
    }
    else if (NULL != argv[1])
    {
        pintf = "gigabitethernet";
        pintf_num = argv[1];
    }
    else if (NULL != argv[2])
    {
        pintf = "xgigabitethernet";
        pintf_num = argv[2];
    }

    if (NULL != pintf)
    {
        ifindex = ifm_get_ifindex_by_name(pintf, pintf_num);
        if (ifindex == 0)
        {
            vty_error_out(vty, "Specify the illegal interface.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    si.instance = (uint16_t)atoi(argv[3]);

    si.ifindex = l2vpn_get_service_instance_ifindex(ifindex, si.instance);

    pif = mpls_if_get(si.ifindex);
    if (NULL == pif)
    {
        vty_error_out(vty, "Specify the interface dose not exist.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    psi = service_instance_lookup(si.ifindex);
    if (NULL == psi)
    {
        vty_error_out(vty, "The specified service instance doesn't exist.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (NULL != psi->pcont)
    {
        vty_error_out(vty, "The operation conflicts with some existing configurations.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (psi->en_type == ENCAP_TYPE_INVALID)
    {
        vty_error_out(vty, "Please configure the encapsulation first.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    pcont->uni.instance.instance = si.instance;
    pcont->uni.instance.ifindex  = si.ifindex;
    pcont->uni.ifindex           = ifindex;
    pcont->uni.mode              = ACCESS_MODE_VLAN;

    if (NULL != argv[4])
    {
        if (argv[4][0] == 'e')
        {
            pcont->uni.mode = ACCESS_MODE_ETH;
        }
    }

    psi->pcont = pcont;

    SET_FLAG(pcont->flag, UNI_FLAG);

    l2vpn_add_master_pw(pcont);
    l2vpn_add_backup_pw(pcont);

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月26日 16:40:53
 * @note       :
 */
DEFUN(no_l2vpn_ac_interface_h3c,
    no_l2vpn_ac_interface_cmd_h3c,
    "no ac interface {ethernet USP | gigabitethernet USP | xgigabitethernet USP} "
    "service-instance <1-4096>",
    NO_STR
    "Configure an attachment circuit\n"
    "Specify the AC interface\n"
    CLI_INTERFACE_ETHERNET_STR
    CLI_INTERFACE_ETHERNET_VHELP_STR
    CLI_INTERFACE_GIGABIT_ETHERNET_STR
    CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
    CLI_INTERFACE_XGIGABIT_ETHERNET_STR
    CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
    "Specify a service instance\n"
    "Service instance ID\n")
{
    struct cont_entry *pcont     = (struct cont_entry *)vty->index_sub;
    const char        *pintf     = NULL;
    const char        *pintf_num = NULL;
    struct serv_ins    si;
    uint32_t ifindex = 0;

    memset(&si, 0, sizeof(struct serv_ins));

    if (NULL != argv[0])
    {
        pintf = "ethernet";
        pintf_num = argv[0];
    }
    else if (NULL != argv[1])
    {
        pintf = "gigabitethernet";
        pintf_num = argv[1];
    }
    else if (NULL != argv[2])
    {
        pintf = "xgigabitethernet";
        pintf_num = argv[2];
    }

    if (NULL != pintf)
    {
        ifindex = ifm_get_ifindex_by_name(pintf, pintf_num);
        if (ifindex == 0)
        {
            vty_error_out(vty, "Specify the illegal interface.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    si.instance = (uint16_t)atoi(argv[3]);

    si.ifindex = l2vpn_get_service_instance_ifindex(ifindex, si.instance);

    if ((pcont->uni.ifindex != ifindex)
        || (pcont->uni.instance.ifindex != si.ifindex)
        || (pcont->uni.instance.instance != si.instance))
    {
        return CMD_SUCCESS;
    }

    connection_delete_uni(pcont);

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月26日 16:44:33
 * @note       :
 */
DEFUN(l2vpn_peer_h3c,
    l2vpn_peer_cmd_h3c,
    "peer A.B.C.D pw-id <1-4294967295> in-label <16-1048575> out-label <16-1048575> {pw-class STRING<1-19>}",
    "Configure a pseudowire to a peer\n"
    "Peer's LSR ID\n"
    "Specify a pseudowire ID\n"
    "Pseudowire ID\n"
    "Specify an incoming label\n"
    "Incoming label\n"
    "Specify an outgoing label\n"
    "Outgoing label\n"
    "Specify a pseudowire class\n"
    "Pseudowire class name\n")
{
    struct pw_class   *pclass  = NULL;
    struct cont_entry *pcont   = (struct cont_entry *)vty->index_sub;
    struct nni_info    nni;
    int ret = ERRNO_SUCCESS;

    memset(&nni, 0 ,sizeof(struct nni_info));

    nni.pw_class.ctrlword = DISABLE;
    nni.pw_class.pw_type  = PW_TYPE_VLAN;

    nni.peer = inet_strtoipv4((char *)argv[0]);
    if ((0 == nni.peer) || (FALSE == inet_valid_network(nni.peer)))
    {
        vty_error_out(vty, "The specified peer is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    nni.pwid = atoi(argv[1]);
    if (ERRNO_FAIL == pwid_check(nni.pwid))
    {
        vty_error_out(vty, "Pseudowire ID is already used.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if ((pcont->nni_master.peer == 0)
        && (pcont->nni_master.pwid == 0))
    {
        nni.inlabel = atoi(argv[2]);
        if (0 == label_get(nni.inlabel, MODULE_ID_L2VPN))
        {
            vty_error_out(vty, "The in label being used.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        nni.outlabel = atoi(argv[3]);

        if (NULL != argv[4])
        {
            pclass = pw_class_lookup((uchar *)argv[4]);
            if (NULL != pclass)
            {
                memcpy(&nni.pw_class, pclass, sizeof(struct pw_class));

				bfd_bind_for_lsp_pw_h3c(BFD_TYPE_PW, &nni);
            }

            memcpy(nni.pw_class.name, argv[4], strlen(argv[4]));
        }

        pcont->nni_master = nni;

        SET_FLAG(pcont->flag, NNI_MASTER_FLAG);

        sprintf(pcont->master_name, "pw%u", pcont->nni_master.pwid);

        ret = l2vpn_add_master_pw(pcont);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
    else
    {
        if ((pcont->nni_master.peer != nni.peer)
            || (pcont->nni_master.pwid != nni.pwid))
        {
            vty_error_out(vty, "The number of ACs or PWs in the connection has "
                                "reached the limit.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        nni.inlabel = atoi(argv[2]);
        nni.outlabel = atoi(argv[3]);

        if ((pcont->nni_master.inlabel != nni.inlabel)
            || (pcont->nni_master.outlabel != nni.outlabel))
        {
            vty_error_out(vty, "The PW already exists.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    vty->node = PW_MASTER_NODE;

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月26日 16:51:28
 * @note       :
 */
DEFUN(no_l2vpn_peer_h3c,
    no_l2vpn_peer_cmd_h3c,
    "no peer A.B.C.D pw-id <1-4294967295>",
    NO_STR
    "Configure a pseudowire to a peer\n"
    "Peer's LSR ID\n"
    "Specify a pseudowire ID\n"
    "Pseudowire ID\n")
{
    struct cont_entry *pcont  = (struct cont_entry *)vty->index_sub;
    struct nni_info nni;

    nni.peer = inet_strtoipv4((char *)argv[0]);
    if ((0 == nni.peer) || (FALSE == inet_valid_network(nni.peer)))
    {
        vty_error_out(vty, "The specified peer is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    nni.pwid = atoi(argv[1]);

    if ((pcont->nni_master.peer != nni.peer)
        || (pcont->nni_master.pwid != nni.pwid))
    {
        return CMD_SUCCESS;
    }

	bfd_unbind_for_lsp_pw_h3c(pcont->nni_backup.bfd_id);
	bfd_unbind_for_lsp_pw_h3c(pcont->nni_master.bfd_id);

    connection_delete_nni_backup(pcont);
    connection_delete_nni_master(pcont);

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月26日 17:11:52
 * @note       :
 */
DEFUN(l2vpn_backup_peer_h3c,
    l2vpn_backup_peer_cmd_h3c,
    "backup-peer A.B.C.D pw-id <1-4294967295> in-label <16-1048575> out-label <16-1048575> "
    "{pw-class STRING<1-19>}",
    "Configure a backup pseudowire to a peer\n"
    "Peer's LSR ID\n"
    "Specify a pseudowire ID\n"
    "Pseudowire ID\n"
    "Specify an incoming label\n"
    "Incoming label\n"
    "Specify an outgoing label\n"
    "Outgoing label\n"
    "Specify a pseudowire class\n"
    "Pseudowire class name\n")
{
    struct cont_entry *pcont   = (struct cont_entry *)vty->index_sub;
    struct pw_class   *pclass  = NULL;
    struct nni_info    nni;
    int ret = ERRNO_SUCCESS;

    memset(&nni, 0 ,sizeof(struct nni_info));

    nni.pw_class.ctrlword = DISABLE;
    nni.pw_class.pw_type  = PW_TYPE_VLAN;

    nni.peer = inet_strtoipv4((char *)argv[0]);
    if ((0 == nni.peer) || (FALSE == inet_valid_network(nni.peer)))
    {
        vty_error_out(vty, "The specified peer is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    nni.pwid = atoi(argv[1]);
    if (ERRNO_FAIL == pwid_check(nni.pwid))
    {
        vty_error_out(vty, "Pseudowire ID is already used.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if ((pcont->nni_backup.peer == 0)
        && (pcont->nni_backup.pwid == 0))
    {
        nni.inlabel = atoi(argv[2]);
        if (0 == label_get(nni.inlabel, MODULE_ID_L2VPN))
        {
            vty_error_out(vty, "The in label being used.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        nni.outlabel = atoi(argv[3]);

        if (NULL != argv[4])
        {
            pclass = pw_class_lookup((uchar *)argv[4]);
            if (NULL != pclass)
            {
                memcpy(&nni.pw_class, pclass, sizeof(struct pw_class));

				bfd_bind_for_lsp_pw_h3c(BFD_TYPE_PW, &nni);
            }

            memcpy(nni.pw_class.name, argv[4], strlen(argv[4]));
        }

        pcont->nni_backup = nni;

        SET_FLAG(pcont->flag, NNI_BACKUP_FLAG);

        sprintf(pcont->backup_name, "pw%u", pcont->nni_backup.pwid);

        ret = l2vpn_add_backup_pw(pcont);
        if (ERRNO_SUCCESS != ret)
        {
            connection_delete_nni_backup(pcont);

            vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
    else
    {
        if ((pcont->nni_backup.peer != nni.peer)
            || (pcont->nni_backup.pwid != nni.pwid))
        {
            vty_error_out(vty, "The number of ACs or PWs in the connection has "
                                "reached the limit.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        nni.inlabel = atoi(argv[2]);
        nni.outlabel = atoi(argv[3]);

        if ((pcont->nni_backup.inlabel != nni.inlabel)
            || (pcont->nni_backup.outlabel != nni.outlabel))
        {
            vty_error_out(vty, "The PW already exists.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    vty->node = PW_BACKUP_NODE;

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月26日 17:13:12
 * @note       :
 */
DEFUN(no_l2vpn_backup_peer_h3c,
    no_l2vpn_backup_peer_cmd_h3c,
    "no backup-peer A.B.C.D pw-id PWID",
    NO_STR
    "Configure a backup pseudowire to a peer\n"
    "Peer's LSR ID\n"
    "Specify a pseudowire ID\n"
    "Pseudowire ID\n")
{
    struct cont_entry *pcont  = (struct cont_entry *)vty->index_sub;
    struct nni_info nni;

    nni.peer = inet_strtoipv4((char *)argv[0]);
    if ((0 == nni.peer) || (FALSE == inet_valid_network(nni.peer)))
    {
        vty_error_out(vty, "The specified peer is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    nni.pwid = atoi(argv[1]);

    if ((pcont->nni_backup.peer != nni.peer)
        || (pcont->nni_backup.pwid != nni.peer))
    {
        return CMD_SUCCESS;
    }

	bfd_unbind_for_lsp_pw_h3c(pcont->nni_backup.bfd_id);

    connection_delete_nni_backup(pcont);

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月17日 10:32:48
 * @note       :
 */
DEFUN(l2vpn_pw_revertive_h3c,
    l2vpn_pw_revertive_cmd_h3c,
    "revertive { wtr <0-3600> | never }",
    "Configure revertible mode for pseudowire protection switching\n"
    "Set the wait-to-restore timer\n"
    "Wait-to-restore time in seconds\n"
    "Never switch back to the main PW\n")
{
    struct cont_entry *pcont = (struct cont_entry *)vty->index_sub;
    struct mpls_if    *pif   = NULL;

    if (NULL != argv[0])
    {
        pcont->failback = FAILBACK_ENABLE;
        pcont->wtr      = (uint16_t)atoi(argv[0]);
    }
    else if (NULL != argv[1])
    {
        pcont->failback = FAILBACK_DISABLE;
        pcont->wtr      = 0;
    }

    pif = mpls_if_lookup(pcont->uni.instance.ifindex);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    if (NULL != pif->ppw_master)
    {
        pif->ppw_master->failback = pcont->failback;
        pif->ppw_master->wtr      = pcont->wtr;

        pw_set_pw_status(pif);
    }

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月17日 11:03:06
 * @note       :
 */
DEFUN(no_l2vpn_pw_revertive_h3c,
    no_l2vpn_pw_revertive_cmd_h3c,
    "no revertive ( wtr | never )",
    NO_STR
    "Configure revertible mode for pseudowire protection switching\n"
    "Set the wait-to-restore timer\n"
    "Wait-to-restore time in seconds\n"
    "Never switch back to the main PW\n")
{
    struct cont_entry *pcont = (struct cont_entry *)vty->index_sub;
    struct mpls_if    *pif   = NULL;

    if (argv[0][0] == 'w')
    {
        if (pcont->failback != FAILBACK_ENABLE)
        {
            return CMD_SUCCESS;
        }

        pcont->failback = FAILBACK_ENABLE;
        pcont->wtr      = 0;
    }
    else if (argv[0][0] == 'n')
    {
        if (pcont->failback != FAILBACK_DISABLE)
        {
            return CMD_SUCCESS;
        }

        pcont->failback = FAILBACK_DISABLE;
        pcont->wtr      = 0;
    }

    pif = mpls_if_lookup(pcont->uni.instance.ifindex);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    if (NULL != pif->ppw_master)
    {
        pif->ppw_master->failback = pcont->failback;
        pif->ppw_master->wtr      = pcont->wtr;

        pw_set_pw_status(pif);
    }

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月26日 14:40:21
 * @note       :
 */
DEFUN(l2vpn_pw_class_h3c,
    l2vpn_pw_class_cmd_h3c,
    "pw-class STRING<1-19>",
    "Configure a pseudowire class\n"
    "Pseudowire class name\n")
{
    struct pw_class *pclass  = NULL;
    char            *pprompt = NULL;
    int ret = ERRNO_SUCCESS;

    pclass = pw_class_lookup((uchar *)argv[0]);
    if (NULL == pclass)
    {
        pclass = pw_class_create();
        if (NULL == pclass)
        {
            vty_error_out(vty, "Memory alloc failed !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        memcpy(pclass->name, argv[0], strlen(argv[0]));

        ret = pw_class_add(pclass);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

            pw_class_free(pclass);

            return CMD_WARNING;
        }
    }

    vty->index = pclass;
    vty->node  = PW_CLASS_NODE;

    pprompt = vty->change_prompt;
    if (NULL != pprompt)
    {
        snprintf(pprompt, VTY_BUFSIZ, "%%s(pw-class-%s)# ", argv[0]);
    }

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月26日 14:41:54
 * @note       :
 */
DEFUN(no_l2vpn_pw_class_h3c,
    no_l2vpn_pw_class_cmd_h3c,
    "no pw-class STRING<1-19>",
    NO_STR
    "Configure a pseudowire class\n"
    "Pseudowire class name\n")
{
    pw_class_delete((uchar *)argv[0]);

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月26日 15:03:21
 * @note       :
 */
DEFUN(l2vpn_control_word_h3c,
    l2vpn_control_word_cmd_h3c,
    "control-word enable",
    "Configure pseudowire control word\n"
    "Enable control word\n")
{
    struct pw_class *pclass = (struct pw_class *)vty->index;

    pclass->ctrlword = ENABLE;

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月26日 15:04:47
 * @note       :
 */
DEFUN(no_l2vpn_control_word_h3c,
    no_l2vpn_control_word_cmd_h3c,
    "no control-word enable",
    NO_STR
    "Configure pseudowire control word\n"
    "Enable control word\n")
{
    struct pw_class *pclass = (struct pw_class *)vty->index;

    pclass->ctrlword = DISABLE;

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月26日 15:07:39
 * @note       : 默认 pw 封装类型为 vlan
 */
DEFUN(l2vpn_pw_type_h3c,
    l2vpn_pw_type_cmd_h3c,
    "pw-type (ethernet | vlan)",
    "Configure the pseudowire encapsulation type\n"
    "Use Ethernet encapsulation\n"
    "Use VLAN encapsulation\n")
{
    struct pw_class *pclass = (struct pw_class *)vty->index;

    if (argv[0][0] == 'e')
    {
        pclass->pw_type = PW_TYPE_ETH;
    }
    else if (argv[0][0] == 'v')
    {
        pclass->pw_type = PW_TYPE_VLAN;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月26日 15:09:40
 * @note       :
 */
DEFUN(no_l2vpn_pw_type_h3c,
    no_l2vpn_pw_type_cmd_h3c,
    "no pw-type",
    NO_STR
    "Configure the pseudowire encapsulation type\n")
{
    struct pw_class *pclass = (struct pw_class *)vty->index;

    pclass->pw_type = PW_TYPE_VLAN;

    return CMD_SUCCESS;
}

/**
 * @brief	   :
 * @param[in ] :
 * @param[out] :
 * @return	   :
 * @author	   : jzs
 * @date	   : 2018年5月22日 15:09:40
 * @note	   :
 */
DEFUN (vccv_bfd_enable,
	vccv_bfd_enable_cmd,
	"vccv bfd [raw-bfd] [template NAME] ",
	BFD_STR
	"Enable vccv bfd function\n"
	"raw-bfd without IP/UDP Headers\n"
	"Name template_Name\n")
{
	struct pw_class *pclass = (struct pw_class *)vty->index;
	
	if(NULL != argv[0])
	{
		pclass->bfd_chtype = 1;
	}
	else
	{
		pclass->bfd_chtype = 0;
	}

	if(NULL != argv[1])
	{
        memcpy(pclass->bfd_temp, argv[1], strlen(argv[1]));
	}
	else
	{
		memset(pclass->bfd_temp, 0, NAME_STRING_LEN);
	}

	pclass->bfd_flag = ENABLE;

	pw_class_change_bfd_template(pclass);
	
	return CMD_SUCCESS;
}


/**
 * @brief	   :
 * @param[in ] :
 * @param[out] :
 * @return	   :
 * @author	   : jzs
 * @date	   : 2018年5月22日 15:09:40
 * @note	   :
 */
DEFUN (undo_vccv_bfd_enable,
	undo_vccv_bfd_enable_cmd,
	"undo vccv bfd",
	BFD_STR
	"Disable vccv bfd function\n")
{
	struct pw_class *pclass = (struct pw_class *)vty->index;

	pw_class_change_bfd_template(pclass);
	
	memset(pclass->bfd_temp, 0, NAME_STRING_LEN);
	pclass->bfd_chtype = 0;
	pclass->bfd_flag = DISABLE;
	
	return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月26日 15:16:39
 * @note       :
 */
DEFUN(l2vpn_service_instance_h3c,
    l2vpn_service_instance_cmd_h3c,
    "service-instance <1-4096>",
    "Configure a service instance\n"
    "Service instance ID\n")
{
    struct serv_ins *psi = NULL;
    struct serv_ins  si;
    int ret = ERRNO_SUCCESS;

    si.instance = (uint16_t)atoi(argv[0]);

    si.ifindex = l2vpn_get_service_instance_ifindex((uint32_t)vty->index, si.instance);

    psi = service_instance_lookup(si.ifindex);
    if (NULL == psi)
    {
        psi = service_instance_create();
        if (NULL == psi)
        {
            vty_error_out(vty, "Memory alloc failed !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        psi->ifindex  = si.ifindex;
        psi->instance = si.instance;

        ret = service_instance_add(psi);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

            service_instance_free(psi);

            return CMD_WARNING;
        }
    }

    vty->index_sub = psi;
    vty->node      = SERVICE_INSTANCE_NODE;

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月26日 15:17:18
 * @note       :
 */
DEFUN(no_l2vpn_service_instance_h3c,
    no_l2vpn_service_instance_cmd_h3c,
    "no service-instance <1-4096>",
    NO_STR
    "Configure a service instance\n"
    "Service instance ID\n")
{
    struct serv_ins *psi = NULL;
    struct serv_ins  si;

    si.instance = (uint16_t)atoi(argv[0]);

    si.ifindex = l2vpn_get_service_instance_ifindex((uint32_t)vty->index, si.instance);

    psi = service_instance_lookup(si.ifindex);
    if (NULL == psi)
    {
        return CMD_SUCCESS;
    }

    if (ENCAP_TYPE_INVALID != psi->en_type)
    {
        ifm_unset_subif_l2(psi->ifindex, MODULE_ID_MPLS);
    }

    if (NULL != psi->pcont)
    {
        connection_delete_uni((struct cont_entry *)psi->pcont);
    }

    service_instance_delete(si.ifindex);

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月27日 14:17:35
 * @note       :
 */
DEFUN(l2vpn_encapsulation_svid_h3c,
    l2vpn_encapsulation_svid_cmd_h3c,
    "encapsulation s-vid <1-4094>",
    "Configure a packet matching rule for the service instance\n"
    "Match service VLAN tags\n"
    "VLAN ID\n")
{
    struct serv_ins *psi = (struct serv_ins *)vty->index_sub;
    struct serv_ins si;
    int ret = ERRNO_SUCCESS;

    if (ENCAP_TYPE_INVALID != psi->en_type)
    {
        vty_error_out(vty, "Please delete the encapsulation first.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    si.en_type = ENCAP_TYPE_VID;
    si.vid     = (uint16_t)atoi(argv[0]);

    ret = l2vpn_check_service_instance_encap(psi->ifindex, si.en_type, si.vid);
    if (ERRNO_FAIL == ret)
    {
        vty_error_out(vty, "The operation conflicts with some existing configurations.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = ifm_set_subif_l2(psi->ifindex, si.vid, 0, MODULE_ID_MPLS);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "balabala.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    psi->en_type = si.en_type;
    psi->vid     = si.vid;

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月27日 14:19:01
 * @note       :
 */
DEFUN(l2vpn_encapsulation_h3c,
    l2vpn_encapsulation_cmd_h3c,
    "encapsulation (default|tagged|untagged)",
    "Configure a packet matching rule for the service instance\n"
    "Match the packets that unmatch with any other criteria\n"
    "Match tagged packets\n"
    "Match untagged packets\n")
{
    struct serv_ins *psi = (struct serv_ins *)vty->index_sub;
    enum encap_type type = ENCAP_TYPE_INVALID;
    int ret = ERRNO_SUCCESS;
    int svlan = 0;
    int cvlan = 0;

    if (ENCAP_TYPE_INVALID != psi->en_type)
    {
        vty_error_out(vty, "Please delete the encapsulation first.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (argv[0][0] == 'd')
    {
        type = ENCAP_TYPE_DEFAULT;

        vty_warning_out(vty, "Not support!%s", VTY_NEWLINE);

        return CMD_WARNING;
    }
    else if (argv[0][0] == 't')
    {
        type = ENCAP_TYPE_TAGGED;
    }
    else if (argv[0][0] == 'u')
    {
        type = ENCAP_TYPE_UNTAGGED;
    }

    ret = l2vpn_check_service_instance_encap(psi->ifindex, type, 0);
    if (ERRNO_FAIL == ret)
    {
        vty_error_out(vty, "The operation conflicts with some existing configurations.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = ifm_set_subif_l2(psi->ifindex, svlan, cvlan, MODULE_ID_MPLS);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "ifm_set_subif_l2 error.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    psi->en_type = type;

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月27日 14:20:06
 * @note       :
 */
DEFUN(no_l2vpn_encapsulation_h3c,
    no_l2vpn_encapsulation_cmd_h3c,
    "no encapsulation",
    NO_STR
    "Configure a packet matching rule for the service instance\n")
{
    struct serv_ins *psi = (struct serv_ins *)vty->index_sub;

    if (ENCAP_TYPE_INVALID == psi->en_type)
    {
        return CMD_SUCCESS;
    }

    ifm_unset_subif_l2(psi->ifindex, MODULE_ID_MPLS);

    if (NULL != psi->pcont)
    {
        connection_delete_uni((struct cont_entry *)psi->pcont);
    }

    psi->en_type = ENCAP_TYPE_INVALID;
    psi->vid     = 0;

    vty_info_out(vty, "The association between the service instance and the VSI/Xconnect group, "
                    "and global configuration associated with the service instance were deleted.%s",
                    VTY_NEWLINE);

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月10日 9:29:55
 * @note       :
 */
int l2vpn_connection_config_write(struct vty *vty, struct xc_group *pgroup)
{
    struct list       *plist = NULL;
    struct listnode   *pnode = NULL;
    struct cont_entry *pcont = NULL;
    char ifname[IFNET_NAMESIZE]  = "";
    char peerip[INET_ADDRSTRLEN] = "";

    vty_out(vty, "xconnect-group %s%s", pgroup->name, VTY_NEWLINE);

    plist = &pgroup->cont_list;

    for (ALL_LIST_ELEMENTS_RO(plist, pnode, pcont))
    {
        vty_out(vty, " connection %s%s", pcont->name, VTY_NEWLINE);

        if ((pcont->failback == FAILBACK_ENABLE) && (pcont->wtr != 0))
        {
            vty_out(vty, "  revertive wtr %u%s", pcont->wtr, VTY_NEWLINE);
        }
        else if (pcont->failback == FAILBACK_DISABLE)
        {
            vty_out(vty, "  revertive never%s", VTY_NEWLINE);
        }

        if (FLAG_ISSET(pcont->flag, UNI_FLAG))
        {
            ifm_get_name_by_ifindex(pcont->uni.ifindex, ifname);

            vty_out(vty, "  ac interface %s service-instance %u", ifname, pcont->uni.instance.instance);

            if (ACCESS_MODE_ETH == pcont->uni.mode)
            {
                vty_out(vty, " access-mode ethernet%s", VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, "%s", VTY_NEWLINE);
            }
        }

        if (FLAG_ISSET(pcont->flag, NNI_MASTER_FLAG))
        {
            inet_ipv4tostr(pcont->nni_master.peer, peerip);

            vty_out(vty, "  peer %s pw-id %u in-label %u out-label %u", peerip, pcont->nni_master.pwid,
                        pcont->nni_master.inlabel, pcont->nni_master.outlabel);

            if ('\0' != pcont->nni_master.pw_class.name[0])
            {
                vty_out(vty, " pw-class %s%s", pcont->nni_master.pw_class.name, VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, "%s", VTY_NEWLINE);
            }
        }

        if (FLAG_ISSET(pcont->flag, NNI_BACKUP_FLAG))
        {
            inet_ipv4tostr(pcont->nni_backup.peer, peerip);

            vty_out(vty, "   backup-peer %s pw-id %u in-label %u out-label %u", peerip, pcont->nni_backup.pwid,
                        pcont->nni_backup.inlabel, pcont->nni_backup.outlabel);

            if ('\0' != pcont->nni_backup.pw_class.name[0])
            {
                vty_out(vty, " pw-class %s%s", pcont->nni_backup.pw_class.name, VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, "%s", VTY_NEWLINE);
            }
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月10日 9:23:46
 * @note       :
 */
int l2vpn_xc_group_config_write(struct vty *vty)
{
    struct xc_group    *pgroup  = NULL;
    struct hash_bucket *pbucket = NULL;
    int cursor = 0;

    HASH_BUCKET_LOOP(pbucket, cursor, xc_group_table)
    {
        pgroup = (struct xc_group *)pbucket->data;
        if (NULL == pgroup)
        {
            continue;
        }

        l2vpn_connection_config_write(vty, pgroup);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月10日 9:46:29
 * @note       :
 */
int l2vpn_pw_class_config_write(struct vty *vty)
{
    struct pw_class    *pclass  = NULL;
    struct hash_bucket *pbucket = NULL;
    int cursor = 0;

    HASH_BUCKET_LOOP(pbucket, cursor, pw_class_table)
    {
        pclass = (struct pw_class *)pbucket->data;
        if (NULL == pclass)
        {
            continue;
        }

        vty_out(vty, "pw-class %s%s", pclass->name, VTY_NEWLINE);

        if (PW_TYPE_ETH == pclass->pw_type)
        {
            vty_out(vty, " pw-type ethernet%s", VTY_NEWLINE);
        }

        if (ENABLE == pclass->ctrlword)
        {
            vty_out(vty, " control-word enable%s", VTY_NEWLINE);
        }
		if(ENABLE == pclass->bfd_flag)
		{
			if(ENABLE == pclass->bfd_chtype)
			{
				if('\0' != pclass->bfd_temp[0])
				{
					vty_out(vty, " vccv bfd raw-bfd template %s%s", pclass->bfd_temp, VTY_NEWLINE);
				}
			}
			else
			{
				if('\0' != pclass->bfd_temp[0])
				{
					vty_out(vty, " vccv bfd template %s%s", pclass->bfd_temp, VTY_NEWLINE);
				}
				else
				{
					vty_out(vty, " vccv bfd%s", VTY_NEWLINE);
				}
			}
		}
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月10日 9:53:57
 * @note       :
 */
int l2vpn_service_instance_config_write(struct vty *vty)
{
    struct serv_ins    *psi     = NULL;
    struct hash_bucket *pbucket = NULL;
    char ifname[IFNET_NAMESIZE] = "";
    uint32_t ifindex = 0;
    int cursor = 0;

    HASH_BUCKET_LOOP(pbucket, cursor, service_instance_table)
    {
        psi = (struct serv_ins *)pbucket->data;
        if (NULL == psi)
        {
            continue;
        }

        memset(ifname, 0, IFNET_NAMESIZE);
        ifindex = IFM_PARENT_IFINDEX_GET(psi->ifindex);
        ifm_get_name_by_ifindex(ifindex, ifname);

        vty_out(vty, "interface %s%s", ifname, VTY_NEWLINE);

        vty_out(vty, " service-instance %u%s", psi->instance, VTY_NEWLINE);

        if (ENCAP_TYPE_VID == psi->en_type)
        {
            vty_out(vty, "  encapsulation s-vid %u%s", psi->vid, VTY_NEWLINE);
        }
        else if (ENCAP_TYPE_DEFAULT == psi->en_type)
        {
            vty_out(vty, "  encapsulation default%s", VTY_NEWLINE);
        }
        else if (ENCAP_TYPE_TAGGED == psi->en_type)
        {
            vty_out(vty, "  encapsulation tagged%s", VTY_NEWLINE);
        }
        else if (ENCAP_TYPE_UNTAGGED == psi->en_type)
        {
            vty_out(vty, "  encapsulation untagged%s", VTY_NEWLINE);
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月7日 10:51:15
 * @note       :
 */
void l2vpn_cmd_h3c_init(void)
{
    install_node(&xc_group_node, l2vpn_xc_group_config_write);
    install_node(&connection_node, NULL);
    install_node(&pw_class_node, l2vpn_pw_class_config_write);
    install_node(&service_instance_node, l2vpn_service_instance_config_write);
    install_node(&pw_master_node, NULL);
    install_node(&pw_backup_node, NULL);

    install_default(XCONNECT_GROUP_NODE);
    install_default(CONNECTION_NODE);
    install_default(PW_CLASS_NODE);
    install_default(SERVICE_INSTANCE_NODE);
    install_default(PW_MASTER_NODE);
    install_default(PW_BACKUP_NODE);

    install_element(CONFIG_NODE, &l2vpn_xconnect_group_cmd_h3c, CMD_SYNC);
    install_element(CONFIG_NODE, &no_l2vpn_xconnect_group_cmd_h3c, CMD_SYNC);
    install_element(CONFIG_NODE, &l2vpn_pw_class_cmd_h3c, CMD_SYNC);
    install_element(CONFIG_NODE, &no_l2vpn_pw_class_cmd_h3c, CMD_SYNC);

    install_element(XCONNECT_GROUP_NODE, &l2vpn_connection_cmd_h3c, CMD_SYNC);
    install_element(XCONNECT_GROUP_NODE, &no_l2vpn_connection_cmd_h3c, CMD_SYNC);

    install_element(CONNECTION_NODE, &l2vpn_pw_revertive_cmd_h3c, CMD_SYNC);
    install_element(CONNECTION_NODE, &no_l2vpn_pw_revertive_cmd_h3c, CMD_SYNC);
    install_element(CONNECTION_NODE, &l2vpn_ac_interface_cmd_h3c, CMD_SYNC);
    install_element(CONNECTION_NODE, &no_l2vpn_ac_interface_cmd_h3c, CMD_SYNC);
    install_element(CONNECTION_NODE, &l2vpn_peer_cmd_h3c, CMD_SYNC);
    install_element(CONNECTION_NODE, &no_l2vpn_peer_cmd_h3c, CMD_SYNC);

    install_element(PW_MASTER_NODE, &l2vpn_backup_peer_cmd_h3c, CMD_SYNC);
    install_element(PW_MASTER_NODE, &no_l2vpn_backup_peer_cmd_h3c, CMD_SYNC);

    install_element(PW_CLASS_NODE, &l2vpn_control_word_cmd_h3c, CMD_SYNC);
    install_element(PW_CLASS_NODE, &no_l2vpn_control_word_cmd_h3c, CMD_SYNC);
    install_element(PW_CLASS_NODE, &l2vpn_pw_type_cmd_h3c, CMD_SYNC);
    install_element(PW_CLASS_NODE, &no_l2vpn_pw_type_cmd_h3c, CMD_SYNC);
    install_element(PW_CLASS_NODE, &vccv_bfd_enable_cmd, CMD_SYNC);
    install_element(PW_CLASS_NODE, &undo_vccv_bfd_enable_cmd, CMD_SYNC);

    install_element(PHYSICAL_IF_NODE, &l2vpn_service_instance_cmd_h3c, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_l2vpn_service_instance_cmd_h3c, CMD_SYNC);

    install_element(SERVICE_INSTANCE_NODE, &l2vpn_encapsulation_svid_cmd_h3c, CMD_SYNC);
    install_element(SERVICE_INSTANCE_NODE, &l2vpn_encapsulation_cmd_h3c, CMD_SYNC);
    install_element(SERVICE_INSTANCE_NODE, &no_l2vpn_encapsulation_cmd_h3c, CMD_SYNC);

    return;
}


