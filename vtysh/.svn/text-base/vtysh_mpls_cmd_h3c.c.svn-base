/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月9日 9:09:19
 * @note       :
 */

#include <zebra.h>
#include "command.h"
#include "vtysh.h"
#include "ifm_common.h"


static struct cmd_node xc_group_node =
{
    XCONNECT_GROUP_NODE,
    "%s(xcg)# ",
};


static struct cmd_node connection_node =
{
    CONNECTION_NODE,
    "%s(xcg-connection)# ",
};


static struct cmd_node pw_class_node =
{
    PW_CLASS_NODE,
    "%s(pw-class)# ",
};


static struct cmd_node service_instance_node =
{
    SERVICE_INSTANCE_NODE,
    "%s(srv)# ",
};


static struct cmd_node pw_master_node =
{
    PW_MASTER_NODE,
    "%s(xcg-connection-master)# ",
};


static struct cmd_node pw_backup_node =
{
    PW_BACKUP_NODE,
    "%s(xcg-connection-backup)# ",
};


DEFUNSH(VTYSH_MPLS,
    vtysh_exit_xc_group,
    vtysh_exit_xc_group_cmd,
    "exit",
    "Exit current mode and down to previous mode\n")
{
    return vtysh_exit(vty);
}


ALIAS(vtysh_exit_xc_group,
    vtysh_quit_xc_group_cmd,
    "quit",
    "Exit current mode and down to previous mode\n")


DEFUNSH(VTYSH_MPLS,
    vtysh_exit_connection,
    vtysh_exit_connection_cmd,
    "exit",
    "Exit current mode and down to previous mode\n")
{
    return vtysh_exit(vty);
}


ALIAS(vtysh_exit_connection,
    vtysh_quit_connection_cmd,
    "quit",
    "Exit current mode and down to previous mode\n")


DEFUNSH(VTYSH_MPLS,
    vtysh_exit_pw_master,
    vtysh_exit_pw_master_cmd,
    "exit",
    "Exit current mode and down to previous mode\n")
{
    return vtysh_exit(vty);
}


ALIAS(vtysh_exit_pw_master,
    vtysh_quit_pw_master_cmd,
    "quit",
    "Exit current mode and down to previous mode\n")


DEFUNSH(VTYSH_MPLS,
    vtysh_exit_pw_backup,
    vtysh_exit_pw_backup_cmd,
    "exit",
    "Exit current mode and down to previous mode\n")
{
    return vtysh_exit(vty);
}


ALIAS(vtysh_exit_pw_backup,
    vtysh_quit_pw_backup_cmd,
    "quit",
    "Exit current mode and down to previous mode\n")


DEFUNSH(VTYSH_MPLS,
    vtysh_exit_pw_class,
    vtysh_exit_pw_class_cmd,
    "exit",
    "Exit current mode and down to previous mode\n")
{
    return vtysh_exit(vty);
}


ALIAS(vtysh_exit_pw_class,
    vtysh_quit_pw_class_cmd,
    "quit",
    "Exit current mode and down to previous mode\n")


DEFUNSH(VTYSH_MPLS,
    vtysh_exit_service_instance,
    vtysh_exit_service_instance_cmd,
    "exit",
    "Exit current mode and down to previous mode\n")
{
    return vtysh_exit(vty);
}


ALIAS(vtysh_exit_service_instance,
    vtysh_quit_service_instance_cmd,
    "quit",
    "Exit current mode and down to previous mode\n")


DEFSH(VTYSH_MPLS, static_lsp_ingress_nexthop_cmd_h3c_vtysh,
    "static-lsp ingress NAME destination A.B.C.D (A.B.C.D | <0-32>) nexthop A.B.C.D "
    "out-label <16-1048575>",
    "\n")


DEFSH(VTYSH_MPLS, static_lsp_transit_nexthop_cmd_h3c_vtysh,
    "static-lsp transit NAME in-label <16-1048575> nexthop A.B.C.D "
    "out-label <16-1048575>",
    "\n")


DEFSH(VTYSH_MPLS, static_lsp_egress_cmd_h3c_vtysh,
    "static-lsp egress NAME in-label <16-1048575>",
    "\n")


DEFSH(VTYSH_MPLS, no_static_lsp_cmd_h3c_vtysh,
    "no static-lsp (ingress | transit | egress) NAME",
    "\n")


DEFSH(VTYSH_MPLS, l2vpn_enable_cmd_h3c_vtysh,
    "l2vpn enable",
    "Layer 2 Virtual Private Network (L2VPN) module\n"
    "Enable L2VPN\n")


DEFSH(VTYSH_MPLS, no_l2vpn_enable_cmd_h3c_vtysh,
    "no l2vpn enable",
    NO_STR
    "Layer 2 Virtual Private Network (L2VPN) module\n"
    "Enable L2VPN\n")


DEFUNSH(VTYSH_MPLS,
    l2vpn_xconnect_group,
    l2vpn_xconnect_group_cmd_h3c_vtysh,
    "xconnect-group STRING<1-31>",
    "Configure an xconnect-group\n"
    "Xconnect-group name (Hyphen '-' not permitted)\n")
{
    static char index[VTY_BUFSIZ] = "";
    char *pprompt = NULL;

    pprompt = vty->change_prompt;
    if (NULL != pprompt)
    {
        snprintf(pprompt, VTY_BUFSIZ, "%%s(xcg-%s)# ", argv[0]);
    }

    memset(index, 0, VTY_BUFSIZ);
    memcpy(index, argv[0], VTY_BUFSIZ);

    vty->index = index;
    vty->node  = XCONNECT_GROUP_NODE;

    return CMD_SUCCESS;
}


DEFSH(VTYSH_MPLS, no_l2vpn_xconnect_group_cmd_h3c_vtysh,
    "no xconnect-group STRING<1-31>",
    NO_STR
    "Configure an xconnect-group\n"
    "Xconnect-group name (Hyphen '-' not permitted)\n")


DEFUNSH(VTYSH_MPLS,
    l2vpn_connection,
    l2vpn_connection_cmd_h3c_vtysh,
    "connection STRING<1-19>",
    "Configure a cross connection\n"
    "Connection name (Hyphen '-' not permitted)\n")
{
    vty->node  = CONNECTION_NODE;

    return CMD_SUCCESS;
}


DEFSH(VTYSH_MPLS, no_l2vpn_connection_cmd_h3c_vtysh,
    "no connection STRING<1-19>",
    NO_STR
    "Configure a cross connection\n"
    "Connection name (Hyphen '-' not permitted)\n")


DEFSH(VTYSH_MPLS, l2vpn_ac_interface_ethernet_cmd_h3c_vtysh,
    "ac interface ethernet USP service-instance <1-4096> {access-mode (ethernet | vlan)}",
    "Configure an attachment circuit\n"
    "Specify the AC interface\n"
    CLI_INTERFACE_ETHERNET_STR
    CLI_INTERFACE_ETHERNET_VHELP_STR
    "Specify a service instance\n"
    "Service instance ID\n"
    "Specify the access mode\n"
    "Use Ethernet access mode\n"
    "Use Vlan access mode\n")


DEFSH(VTYSH_MPLS, l2vpn_ac_interface_gigabitethernet_cmd_h3c_vtysh,
    "ac interface gigabitethernet USP "
    "service-instance <1-4096> {access-mode (ethernet | vlan)}",
    "Configure an attachment circuit\n"
    "Specify the AC interface\n"
    CLI_INTERFACE_GIGABIT_ETHERNET_STR
    CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
    "Specify a service instance\n"
    "Service instance ID\n"
    "Specify the access mode\n"
    "Use Ethernet access mode\n"
    "Use Vlan access mode\n")


DEFSH(VTYSH_MPLS, l2vpn_ac_interface_xgigabitethernet_cmd_h3c_vtysh,
    "ac interface xgigabitethernet USP "
    "service-instance <1-4096> {access-mode (ethernet | vlan)}",
    "Configure an attachment circuit\n"
    "Specify the AC interface\n"
    CLI_INTERFACE_XGIGABIT_ETHERNET_STR
    CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
    "Specify a service instance\n"
    "Service instance ID\n"
    "Specify the access mode\n"
    "Use Ethernet access mode\n"
    "Use Vlan access mode\n")


DEFSH(VTYSH_MPLS, no_l2vpn_ac_interface_ethernet_cmd_h3c_vtysh,
    "no ac interface ethernet USP service-instance <1-4096>",
    NO_STR
    "Configure an attachment circuit\n"
    "Specify the AC interface\n"
    CLI_INTERFACE_ETHERNET_STR
    CLI_INTERFACE_ETHERNET_VHELP_STR
    "Specify a service instance\n"
    "Service instance ID\n")


DEFSH(VTYSH_MPLS, no_l2vpn_ac_interface_gigabitethernet_cmd_h3c_vtysh,
    "no ac interface gigabitethernet USP service-instance <1-4096>",
    NO_STR
    "Configure an attachment circuit\n"
    "Specify the AC interface\n"
    CLI_INTERFACE_GIGABIT_ETHERNET_STR
    CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
    "Specify a service instance\n"
    "Service instance ID\n")


DEFSH(VTYSH_MPLS, no_l2vpn_ac_interface_xgigabitethernet_cmd_h3c_vtysh,
    "no ac interface xgigabitethernet USP service-instance <1-4096>",
    NO_STR
    "Configure an attachment circuit\n"
    "Specify the AC interface\n"
    CLI_INTERFACE_XGIGABIT_ETHERNET_STR
    CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
    "Specify a service instance\n"
    "Service instance ID\n")


DEFUNSH(VTYSH_MPLS,
    l2vpn_peer,
    l2vpn_peer_cmd_h3c_vtysh,
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
    vty->node = PW_MASTER_NODE;

    return CMD_SUCCESS;
}


DEFSH(VTYSH_MPLS, no_l2vpn_peer_cmd_h3c_vtysh,
    "no peer A.B.C.D pw-id <1-4294967295>",
    NO_STR
    "Configure a pseudowire to a peer\n"
    "Peer's LSR ID\n"
    "Specify a pseudowire ID\n"
    "Pseudowire ID\n")


DEFUNSH(VTYSH_MPLS,
    l2vpn_backup_peer,
    l2vpn_backup_peer_cmd_h3c_vtysh,
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
    vty->node = PW_BACKUP_NODE;

    return CMD_SUCCESS;
}


DEFSH(VTYSH_MPLS, no_l2vpn_backup_peer_cmd_h3c_vtysh,
    "no backup-peer A.B.C.D pw-id PWID",
    NO_STR
    "Configure a backup pseudowire to a peer\n"
    "Peer's LSR ID\n"
    "Specify a pseudowire ID\n"
    "Pseudowire ID\n")


DEFSH(VTYSH_MPLS,
    l2vpn_pw_revertive_wtr_cmd_h3c_vtysh,
    "revertive wtr <0-3600>",
    "Configure revertible mode for pseudowire protection switching\n"
    "Set the wait-to-restore timer\n"
    "Wait-to-restore time in seconds\n")


DEFSH(VTYSH_MPLS,
    l2vpn_pw_revertive_never_cmd_h3c_vtysh,
    "revertive never",
    "Configure revertible mode for pseudowire protection switching\n"
    "Never switch back to the main PW\n")


DEFSH(VTYSH_MPLS,
    no_l2vpn_pw_revertive_cmd_h3c_vtysh,
    "no revertive ( wtr | never )",
    NO_STR
    "Configure revertible mode for pseudowire protection switching\n"
    "Set the wait-to-restore timer\n"
    "Never switch back to the main PW\n")


DEFUNSH(VTYSH_MPLS,
    l2vpn_pw_class,
    l2vpn_pw_class_cmd_h3c_vtysh,
    "pw-class STRING<1-19>",
    "Configure a pseudowire class\n"
    "Pseudowire class name\n")
{
    char *pprompt = NULL;

    pprompt = vty->change_prompt;
    if (NULL != pprompt)
    {
        snprintf(pprompt, VTY_BUFSIZ, "%%s(pw-class-%s)# ", argv[0]);
    }

    vty->node = PW_CLASS_NODE;

    return CMD_SUCCESS;
}


DEFSH(VTYSH_MPLS, no_l2vpn_pw_class_cmd_h3c_vtysh,
    "no pw-class STRING<1-19>",
    NO_STR
    "Configure a pseudowire class\n"
    "Pseudowire class name\n")


DEFSH(VTYSH_MPLS, l2vpn_control_word_cmd_h3c_vtysh,
    "control-word enable",
    "Configure pseudowire control word\n"
    "Enable control word\n")


DEFSH(VTYSH_MPLS, no_l2vpn_control_word_cmd_h3c_vtysh,
    "no control-word enable",
    NO_STR
    "Configure pseudowire control word\n"
    "Enable control word\n")


DEFSH(VTYSH_MPLS, l2vpn_pw_type_cmd_h3c_vtysh,
    "pw-type (ethernet | vlan)",
    "Configure the pseudowire encapsulation type\n"
    "Use Ethernet encapsulation\n"
    "Use VLAN encapsulation\n")


DEFSH(VTYSH_MPLS, no_l2vpn_pw_type_cmd_h3c_vtysh,
    "no pw-type",
    NO_STR
    "Configure the pseudowire encapsulation type\n")


DEFSH (VTYSH_MPLS,
	vccv_bfd_enable_cmd_vtysh,
	"vccv bfd [raw-bfd] [template NAME] ",
	BFD_STR
	"Enable vccv bfd function\n"
	"raw-bfd without IP/UDP Headers\n"
	"Name template_Name\n")


DEFSH (VTYSH_MPLS,
	undo_vccv_bfd_enable_cmd_vtysh,
	"undo vccv bfd",
	BFD_STR
	"Disable vccv bfd function\n")


DEFUNSH(VTYSH_MPLS,
    l2vpn_service_instance,
    l2vpn_service_instance_cmd_h3c_vtysh,
    "service-instance <1-4096>",
    "Configure a service instance\n"
    "Service instance ID\n")
{
    vty->node = SERVICE_INSTANCE_NODE;

    return CMD_SUCCESS;
}


DEFSH(VTYSH_MPLS, no_l2vpn_service_instance_cmd_h3c_vtysh,
    "no service-instance <1-4096>",
    NO_STR
    "Configure a service instance\n"
    "Service instance ID\n")


DEFSH(VTYSH_MPLS, l2vpn_encapsulation_svid_cmd_h3c_vtysh,
    "encapsulation s-vid <1-4094>",
    "Configure a packet matching rule for the service instance\n"
    "Match service VLAN tags\n"
    "VLAN ID\n")


DEFSH(VTYSH_MPLS, l2vpn_encapsulation_cmd_h3c_vtysh,
    "encapsulation (default|tagged|untagged)",
    "Configure a packet matching rule for the service instance\n"
    "Match the packets that unmatch with any other criteria\n"
    "Match tagged packets\n"
    "Match untagged packets\n")


DEFSH(VTYSH_MPLS, no_l2vpn_encapsulation_cmd_h3c_vtysh,
    "no encapsulation",
    NO_STR
    "Configure a packet matching rule for the service instance\n")


void vtysh_init_mpls_cmd_h3c(void)
{
    install_node(&xc_group_node, NULL);
    install_node(&connection_node, NULL);
    install_node(&pw_class_node, NULL);
    install_node(&service_instance_node, NULL);
    install_node(&pw_master_node, NULL);
    install_node(&pw_backup_node, NULL);

    vtysh_install_default(XCONNECT_GROUP_NODE);
    vtysh_install_default(CONNECTION_NODE);
    vtysh_install_default(PW_CLASS_NODE);
    vtysh_install_default(SERVICE_INSTANCE_NODE);
    vtysh_install_default(PW_MASTER_NODE);
    vtysh_install_default(PW_BACKUP_NODE);

    install_element_level(XCONNECT_GROUP_NODE, &vtysh_exit_xc_group_cmd, VISIT_LEVE, CMD_SYNC);
    install_element_level(CONNECTION_NODE, &vtysh_exit_connection_cmd, VISIT_LEVE, CMD_SYNC);
    install_element_level(PW_CLASS_NODE, &vtysh_exit_pw_class_cmd, VISIT_LEVE, CMD_SYNC);
    install_element_level(SERVICE_INSTANCE_NODE, &vtysh_exit_service_instance_cmd, VISIT_LEVE, CMD_SYNC);
    install_element_level(PW_MASTER_NODE, &vtysh_exit_pw_master_cmd, VISIT_LEVE, CMD_SYNC);
    install_element_level(PW_BACKUP_NODE, &vtysh_exit_pw_backup_cmd, VISIT_LEVE, CMD_SYNC);

    install_element_level(CONFIG_NODE, &static_lsp_ingress_nexthop_cmd_h3c_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(CONFIG_NODE, &static_lsp_transit_nexthop_cmd_h3c_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(CONFIG_NODE, &static_lsp_egress_cmd_h3c_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(CONFIG_NODE, &no_static_lsp_cmd_h3c_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    install_element_level(CONFIG_NODE, &l2vpn_xconnect_group_cmd_h3c_vtysh, VISIT_LEVE, CMD_SYNC);
    install_element_level(CONFIG_NODE, &no_l2vpn_xconnect_group_cmd_h3c_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    install_element_level(XCONNECT_GROUP_NODE, &l2vpn_connection_cmd_h3c_vtysh, VISIT_LEVE, CMD_SYNC);
    install_element_level(XCONNECT_GROUP_NODE, &no_l2vpn_connection_cmd_h3c_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    install_element_level(CONNECTION_NODE, &l2vpn_pw_revertive_wtr_cmd_h3c_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(CONNECTION_NODE, &l2vpn_pw_revertive_never_cmd_h3c_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(CONNECTION_NODE, &no_l2vpn_pw_revertive_cmd_h3c_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(CONNECTION_NODE, &l2vpn_ac_interface_ethernet_cmd_h3c_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(CONNECTION_NODE, &l2vpn_ac_interface_gigabitethernet_cmd_h3c_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(CONNECTION_NODE, &l2vpn_ac_interface_xgigabitethernet_cmd_h3c_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(CONNECTION_NODE, &no_l2vpn_ac_interface_ethernet_cmd_h3c_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(CONNECTION_NODE, &no_l2vpn_ac_interface_gigabitethernet_cmd_h3c_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(CONNECTION_NODE, &no_l2vpn_ac_interface_xgigabitethernet_cmd_h3c_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(CONNECTION_NODE, &l2vpn_peer_cmd_h3c_vtysh, VISIT_LEVE, CMD_SYNC);
    install_element_level(CONNECTION_NODE, &no_l2vpn_peer_cmd_h3c_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    install_element_level(PW_MASTER_NODE, &l2vpn_backup_peer_cmd_h3c_vtysh, VISIT_LEVE, CMD_SYNC);
    install_element_level(PW_MASTER_NODE, &no_l2vpn_backup_peer_cmd_h3c_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    install_element_level(CONFIG_NODE, &l2vpn_pw_class_cmd_h3c_vtysh, VISIT_LEVE, CMD_SYNC);
    install_element_level(CONFIG_NODE, &no_l2vpn_pw_class_cmd_h3c_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    install_element_level(PW_CLASS_NODE, &l2vpn_control_word_cmd_h3c_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PW_CLASS_NODE, &no_l2vpn_control_word_cmd_h3c_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PW_CLASS_NODE, &l2vpn_pw_type_cmd_h3c_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PW_CLASS_NODE, &no_l2vpn_pw_type_cmd_h3c_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PW_CLASS_NODE, &vccv_bfd_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PW_CLASS_NODE, &undo_vccv_bfd_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    install_element_level(PHYSICAL_IF_NODE, &l2vpn_service_instance_cmd_h3c_vtysh, VISIT_LEVE, CMD_SYNC);
    install_element_level(PHYSICAL_IF_NODE, &no_l2vpn_service_instance_cmd_h3c_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    install_element_level(SERVICE_INSTANCE_NODE, &l2vpn_encapsulation_svid_cmd_h3c_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(SERVICE_INSTANCE_NODE, &l2vpn_encapsulation_cmd_h3c_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(SERVICE_INSTANCE_NODE, &no_l2vpn_encapsulation_cmd_h3c_vtysh, CONFIG_LEVE_5, CMD_SYNC);
}

