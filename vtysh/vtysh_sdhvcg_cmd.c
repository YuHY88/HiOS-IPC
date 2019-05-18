/**
 * \page cmds_ref_vcg VCG
 * - \subpage modify_log_vcg
 *
 */

/**
 * \page modify_log_vcg Modify Log
 * \section vcg-v001r001 HiOS-V001R001
 *  
 */

#include <zebra.h>
#include "command.h"
#include "vtysh.h"
#include "lib/ifm_common.h"

DEFUNSH ( VTYSH_INTERFACE_VCG,
           vcg_if,
           physical_vcg_if_cmd,
           "interface sdhvcg USP",
           CLI_INTERFACE_STR
           CLI_INTERFACE_VCG_STR
           CLI_INTERFACE_VCG_VHELP_STR )
{
    int ifindex = 0;
    char *pprompt = NULL;

    if ( argc > 0 )
    {
        ifindex = ifm_get_ifindex_by_name ( "sdhvcg", argv[0] );
        if ( ifindex == 0 )
        {
            return CMD_WARNING;
        }

        /*change node*/
        vty->node = VCG_NODE;
        pprompt = vty->change_prompt;
        if ( pprompt )
        {
            /* format the prompt */
            snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-sdhvcg%d/%d/%d)#",
                       IFM_UNIT_ID_GET ( ifindex ),
                       IFM_SLOT_ID_GET ( ifindex ),
                       IFM_PORT_ID_GET ( ifindex ) );
        }
        vty->index = ( void * )ifindex;

    }
    return CMD_SUCCESS;
}


DEFUNSH ( VTYSH_INTERFACE_VCG,
		vtysh_vcg_exit,
		vtysh_vcg_exit_cmd,
		"exit",
		"Exit current mode and down to previous mode\n" )
{
	return vtysh_exit ( vty );
}


/*绑定VC4级别的VCG*/
DEFSH( VTYSH_SDHVCG,
       binding_vcg4_cmd_vtysh,
	   "binding vc4 (up|down|both) vc4 <1-128> num <1-32>",
	   "binding vcg\n"
       "vc4 level\n"
       "up direction\n"
       "down direction\n"
       "both direction\n"
       "vc4 serial number\n"
       "the range of the vc4 serial number\n"
       "the numbers of binding\n"
       "the range of binding number\n")
	
/*绑定VC3级别的VCG*/
DEFSH ( VTYSH_SDHVCG, 
		binding_vcg3_cmd_vtysh,
		"binding vc3 (up|down|both) vc4 <1-128> vc3 <1-3> num <1-96>",
		"binding vcg\n"
        "vc3 level\n"
        "up direction\n"
        "down direction\n"
        "both direction\n"
        "vc4 serial number\n"
        "the range of the vc4 serial number\n"
        "vc3 serial number\n"
        "the range of the vc3 serial number\n"
        "the numbers of binding\n"
        "the range of binding number\n")
		  
/*绑定VC12级别的VCG*/
DEFSH( VTYSH_SDHVCG, 
	   binding_vcg12_cmd_vtysh,
	   "binding vc12 (up|down|both) vc4 <1-128> vc12 <1-63> num <1-63>",
	   "binding vcg\n"
       "vc12 level\n"
       "up direction\n"
       "down direction\n"
       "both direction\n"
       "vc4 serial number\n"
       "the range of the vc4 serial number\n"
       "vc12 serial number\n"
       "the range of the vc12 serial number\n"
       "the numbers of binding\n"
       "the range of binding number\n")

/*删除VCG绑定*/
DEFSH ( VTYSH_SDHVCG, 
		delete_vcg_binding_cmd_vtysh,
		"no binding",
		NO_STR
		"binding vcg\n")

/*显示VCG绑定信息*/
DEFSH ( VTYSH_SDHVCG, 
		show_vcg_binding_cmd_vtysh,
		"show binding",
		SHOW_STR
       "binding vcg\n")

/*设置当前VCG的J1、J2开销*/
DEFSH ( VTYSH_SDHVCG, 
	    set_vcg_jx_oh_cmd_vtysh,
	    "oh jx (transmit|expect) DISPLAYSTRING {padding_zero}",
	    "set transmit oh\n"
	    "set expect receive\n"
	    "displaystring\n")

/*设置上行加扰，下行解扰*/
DEFSH ( VTYSH_SDHVCG, 
	    set_scrambling_cmd_vtysh,
		"scrambling (up|down|both|none)",
	    "scrambling direction\n")

/*恢复默认上行加扰，下行解扰*/
DEFSH ( VTYSH_SDHVCG, 
	    recover_scrambling_cmd_vtysh,
	    "no scrambling",
	    NO_STR
		"default scrambling both\n")

/*上行fcs插入禁止/使能*/		
DEFSH ( VTYSH_SDHVCG, 
	    set_fcs_insert_cmd_vtysh,
	    "fcs insert (enable|disable)",
	    "enable up direction fcs insert\n"
	    "disable up direction fcs insert\n")

/*恢复默认fcs插入*/
DEFSH ( VTYSH_SDHVCG, 
	    recover_fcs_insert_cmd_vtysh,
	    "no fcs insert",
	    NO_STR
		"default enable up direction fcs insert\n")

/*lcas禁止/使能*/
DEFSH ( VTYSH_SDHVCG,
        set_lcas_cmd_vtysh,
        "lcas (enable|disable)",
        "enable lcas\n"
        "disable lcas\n")

/*恢复默认lcas*/
DEFSH ( VTYSH_SDHVCG,
        recover_lcas_cmd_vtysh,
        "no lcas",
        NO_STR
        "default enable lcas\n")

/*复位lcas*/
DEFSH ( VTYSH_SDHVCG,
        reset_lcas_cmd_vtysh,
        "lcas reset",
        "reset lcas\n")

/*查询当前配置信息*/
DEFSH ( VTYSH_SDHVCG,
        config_information_cmd_vtysh,
        "show config",
        SHOW_STR
        "config info\n")

DEFSH ( VTYSH_SDHVCG,
        show_sdhvcg_interface_config_cmd_vtysh,
        "show interface config sdhvcg [USP]",
        SHOW_STR
        CLI_INTERFACE_STR
        "config info\n"
        CLI_INTERFACE_VCG_STR
        CLI_INTERFACE_VCG_VHELP_STR )

DEFSH ( VTYSH_SDHVCG,
         vlan_cfg_cmd_vtysh,
        "vlan <1-4094> pri <0-7> {tpid TPID | upvlan-keep | downvlan-keep}",
        "vlan\n"
        "the range of vlan\n"
        "pri\n"
        "the range of priority\n"
        "tpid\n"
        "tpid value\n"
        "upvlan keep\n"
        "downvlan keep\n" )

DEFSH ( VTYSH_SDHVCG,
      switch_image_cmd_vtysh,
      "switch eos-image (vc4 | vc3 | vc12) slot <1-32>",
      "switch\n"
      "the image which needs switch\n"
      "vc4 image\n"
      "vc3 image\n"
      "vc12 image\n" 
      "slot number\n")

DEFSH (VTYSH_SDHVCG,
	 sdhvcg_conf_debug_cmd_vtysh,
	 "debug sdhvcg (enable|disable) all",
	 "Debug config\n"
	 "Sdhvcg config\n"
	 "Sdhvcg debug enable\n"
	 "Sdhvcg debug disable\n"
	 "Debug all\n"
	)


/* install all cmd */
void vtysh_init_sdhvcg_cmd(void)
{
    install_node ( &vcgif_node, NULL );
    vtysh_install_default (VCG_NODE);

    install_element_level (VCG_NODE, &vtysh_vcg_exit_cmd ,VISIT_LEVE, CMD_SYNC);

    install_element_level (VCG_NODE, &binding_vcg4_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VCG_NODE, &binding_vcg3_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VCG_NODE, &binding_vcg12_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VCG_NODE, &delete_vcg_binding_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VCG_NODE, &show_vcg_binding_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (VCG_NODE, &set_vcg_jx_oh_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VCG_NODE, &set_scrambling_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VCG_NODE, &recover_scrambling_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VCG_NODE, &set_fcs_insert_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VCG_NODE, &recover_fcs_insert_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VCG_NODE, &set_lcas_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VCG_NODE, &recover_lcas_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VCG_NODE, &reset_lcas_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VCG_NODE, &config_information_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL); 
    install_element_level (VCG_NODE, &vlan_cfg_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &switch_image_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &physical_vcg_if_cmd, VISIT_LEVE, CMD_SYNC);  
	install_element_level (CONFIG_NODE, &show_sdhvcg_interface_config_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &sdhvcg_conf_debug_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	
}



