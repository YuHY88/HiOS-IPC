
#include <zebra.h>
#include "command.h"
#include "vtysh.h"

void
vtysh_init_cmd ()
{
    vtysh_init_ifm_cmd ();
    vtysh_init_devm_cmd ();
    vtysh_init_l2_cmd();
    vtysh_init_lldp_cmd ();
    vtysh_init_mstp_cmd();
    vtysh_init_l2if_efm_cmd ();
    vtysh_init_mpls_cmd ();
    vtysh_init_route_cmd ();
    vtysh_init_arp_cmd ();
    vtysh_init_ndp_cmd();
    vtysh_init_ospf_cmd ();
    vtysh_init_ospf6_cmd ();
    if(vtysh_is_app_connect("isisd"))
    {
        vtysh_init_isis_cmd ();
    }
    if(vtysh_is_app_connect("ripd"))
    {
        vtysh_init_ripd_cmd ();
    }

    vtysh_init_ripngd_cmd();
    vtysh_init_dhcp_cmd ();
    vtysh_init_qos_cmd ();
    vtysh_init_filem_cmd ();
    vtysh_init_syslog_cmd ();
    vtysh_init_ftm_cmd();
    vtysh_init_ipmc_cmd();
    vtysh_init_ntp_cmd ();

	vtysh_init_sdhvcg_cmd ();   // priority vtysh_init_hal_cmd, because sdhvcg install_node VCG_NODE

    vtysh_init_hal_cmd ();
    vtysh_init_ping_cmd ();
    vtysh_init_ces_cmd ();
    vtysh_init_aaa_cmd ();
    vtysh_init_statis_cmd ();
    vtysh_init_web_cmd();
    vtysh_init_system_cmd();
    if(vtysh_is_app_connect("bgpd"))
    {
        vtysh_init_bgp_cmd();
    }
    vtysh_init_clock_cmd();
    vtysh_init_synce_cmd();
    vtysh_init_snmpd_cmd ();
    vtysh_init_rmond_cmd ();
    vtysh_init_alarm_cmd();
    vtysh_init_dhcpv6_cmd();
    vtysh_init_qos_h3c_cmd ();
    vtysh_init_mpls_cmd_h3c();

    vtysh_init_sdhmgt_cmd ();
}

