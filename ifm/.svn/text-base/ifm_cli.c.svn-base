
#include <zebra.h>
#include <lib/ether.h>
#include <lib/errcode.h>
#include <lib/log.h>
#include <lib/memory.h>
#include <lib/command.h>
#include "vty.h"
#include "sockunion.h"
#include "prefix.h"
#include "command.h"
#include "memory.h"
#include "log.h"
#include "linklist.h"
#include "hash1.h"
#include "ifm.h"
#include "ifm_init.h"
#include "ifm_cli.h"
#include "ifm_qos.h"
#include "ifm_message.h"
#include "devm_com.h"
#include "msg_ipc.h"

uint16_t default_vlan_table[9] = {4085,4086,4087,4088,4089,4090,4091,4092,4093};
const struct message ifm_dbg_name[] = {
	{.key = IFM_DBG_COMMON , .str = "common"},	
	{.key = IFM_DBG_ALL,     .str = "all"},
};

/* 十六进制 字符串转为十进制数字*/
u_int32_t
cmd_hexstr2int ( char *str, int *ret )
{
    int i;
    int j;
    int powv = 0;
    int len;
    int digit;
    u_int32_t total = 0;
    char *pnt;

    /* Sanify check. */
    if ( str == NULL || ret == NULL )
    { return -1; }

    /* First set return value as error. */
    *ret = -1;

    len = strlen ( str );
    pnt = strchr ( str, 'x' );
    if ( pnt )
    {
        str += 2;
        len -= 2;
    }
    else
    {
        /*Not a  hex number*/
        return -1;
    }

    if ( len < 1 || len > 10 )
    { return -1; }

    for ( i = 0; i < len; i++ )
    {
        if ( *str >= '0' && *str <= '9' )
        { digit = *str++ - '0'; }
        else if ( *str >= 'A' && *str <= 'F' )
        { digit = *str++ - 'A' + 10; }
        else if ( *str >= 'a' && *str <= 'f' )
        { digit = *str++ - 'a' + 10; }
        else
        { return -1; }

        if ( total  > UINT32_MAX )
        { return -1; }

        powv = 1;
        for ( j = 0; j < len - ( i + 1 ); j++ )
        { powv = powv * 16; }

        //total = total + (digit * pow(16,(len-(i+1))));
        total = total + ( digit * powv );
    }

    *ret = 0;
    return total;
}

/* MAC地址字符串是否合法*/
int cmd_check_macform ( char *mac_string )
{
    int len, i;
    int symbol_count = 0;
    int digit_count = 0;

    len = strlen ( mac_string );

    for ( i = 0; i < len; i++ )
    {
        if ( mac_string[i] == ':' )
        {
            if ( i == 0 || digit_count == 0 )
            {
                return -1;//erro form
            }
            symbol_count++;
            digit_count = 0;
        }
        else if ( ( mac_string[i] >= '0' && mac_string[i] <= '9' )
                  || ( mac_string[i] >= 'a' && mac_string[i] <= 'f' )
                  || ( mac_string[i] >= 'A' && mac_string[i] <= 'F' ) )
        {
            digit_count++;
            if ( ! ( digit_count > 0 && digit_count <= 2 ) )
            {
                return -1;//erro form
            }
        }
        else
        {
            return -1;//erro form
        }
    }

    if ( symbol_count != 5 )
    {
        return -1;//erro form
    }

    return 0;//right form
}

/*由接口子类型枚举值获取对应字符串*/
const char *ifm_get_sub_typestr ( enum IFNET_SUBTYPE subtype )
{
    switch ( subtype )
    {
        case IFNET_SUBTYPE_SUBPORT:
            return "Sub port";
        case IFNET_SUBTYPE_VPORT:
            return "Virtual port";
        case IFNET_SUBTYPE_FE:
            return "FE";
        case IFNET_SUBTYPE_GE:
        case IFNET_SUBTYPE_PHYSICAL:
            return "GE";
        case IFNET_SUBTYPE_10GE:
            return "10GE";
        case IFNET_SUBTYPE_40GE:
            return "40GE";
        case IFNET_SUBTYPE_100GE:
            return "100GE";
        case IFNET_SUBTYPE_COMBO:
            return "Combo";
        case IFNET_SUBTYPE_E1:
            return "E1";
        case IFNET_SUBTYPE_T1:
            return "T1";
        case IFNET_SUBTYPE_STM1:
            return "STM1";
        case IFNET_SUBTYPE_STM4:
            return "STM4";
        case IFNET_SUBTYPE_CLOCK2M:
            return "CLOCK2M";
		case IFNET_SUBTYPE_CE1:
			return "VX E1";
        default:
            return "unknown";
    }
    return "unknown";
}


DEFUN ( physical_ethernet_if,
        physical_ethernet_if_cmd,
        "interface ethernet USP",
        CLI_INTERFACE_STR
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR )
{
    struct ifm_entry *pifm = NULL;
    uint32_t ifindex = 0;
    int ret = 0;
    char *pprompt = NULL;

    ifindex = ifm_get_ifindex_by_name ( "ethernet", ( char * ) argv[0] );
    if ( ifindex == 0 )
    {
	    ifm_cli_parse_erro ( vty, IFM_ERR_PARAM_FORMAT );
        return CMD_WARNING;
    }

    /* 判断是子接口 */
    if ( IFM_IS_SUBPORT ( ifindex ) )
    {
        ret = ifm_create_sub_ethernet ( ifindex );
        if ( ret < 0 )
        {
            ifm_cli_parse_erro ( vty, ret );
            return CMD_WARNING;
        }

    }
    else
    {
        pifm = ifm_lookup ( ifindex );
        if ( !pifm )
        {
            ifm_cli_parse_erro ( vty, IFM_ERR_PORT_NOT_FOUND );
            return CMD_WARNING;
        }
    }

    /*change node*/
    if ( IFM_IS_SUBPORT ( ifindex ) )
    {
        vty->node  = PHYSICAL_SUBIF_NODE;
        pprompt = vty->change_prompt;
        if ( pprompt )
        {
            /* format the prompt */
            snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-ethernet%d/%d/%d.%d)#",
                       IFM_UNIT_ID_GET ( ifindex ),
                       IFM_SLOT_ID_GET ( ifindex ),
                       IFM_PORT_ID_GET ( ifindex ),
                       IFM_SUBPORT_ID_GET ( ifindex ) );

        }

    }
    else
    {
        vty->node  = PHYSICAL_IF_NODE;
        pprompt = vty->change_prompt;
        if ( pprompt )
        {
            /* format the prompt */
            snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-ethernet%d/%d/%d)#",
                       IFM_UNIT_ID_GET ( ifindex ),
                       IFM_SLOT_ID_GET ( ifindex ),
                       IFM_PORT_ID_GET ( ifindex ) );

        }
    }
    vty->index = ( void * ) ifindex;

    return CMD_SUCCESS;
}

DEFUN ( clock_if,
        clock_if_cmd,
        "interface clock <1-2>",
        CLI_INTERFACE_STR
        CLI_INTERFACE_CLOCK_STR
        CLI_INTERFACE_CLOCK_VHELP_STR )

{
    uint32_t ifindex = 0;
    struct ifm_entry *pifm = NULL;
    char *pprompt = NULL;

    ifindex = ifm_get_ifindex_by_name ( "clock", ( char * ) argv[0] );
    if ( ifindex == 0 )
    {
	    ifm_cli_parse_erro ( vty, IFM_ERR_PARAM_FORMAT );
        return CMD_WARNING;
    }

    pifm = ifm_lookup ( ifindex );
    if ( !pifm )
    {
	    ifm_cli_parse_erro ( vty, IFM_ERR_PORT_NOT_FOUND );
        return CMD_WARNING;
    }

    /*change node*/
    vty->node = CLOCK_NODE;
    pprompt = vty->change_prompt;
    if ( pprompt )
    {
        /* format the prompt */
        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-clock%d)#",
                   IFM_PORT_ID_GET ( ifindex ) );
    }

    vty->index = ( void * ) ifindex;

    return CMD_SUCCESS;
}


DEFUN ( physical_tdm_if,
        physical_tdm_if_cmd,
        "interface tdm USP",
        CLI_INTERFACE_STR
        CLI_INTERFACE_TDM_STR
        CLI_INTERFACE_TDM_VHELP_STR )

{
    struct ifm_entry *pifm = NULL;
    uint32_t ifindex = 0;
    int ret = 0;
    char *pprompt = NULL;

    ifindex = ifm_get_ifindex_by_name ( "tdm", ( char * ) argv[0] );
    if ( ifindex == 0 )
    {
	    ifm_cli_parse_erro ( vty, IFM_ERR_PARAM_FORMAT );
        return CMD_WARNING;
    }

    /* 判断是子接口 */
    if ( IFM_IS_SUBPORT ( ifindex ) )
    {
        ret = ifm_create_sub_tdm ( ifindex );
        if ( ret < 0 )
        {
            ifm_cli_parse_erro ( vty, ret );
            return CMD_WARNING;
        }

    }
    else
    {
        pifm = ifm_lookup ( ifindex );
        if ( !pifm )
        {
	        ifm_cli_parse_erro ( vty, IFM_ERR_PORT_NOT_FOUND );
            return CMD_WARNING;
        }
    }

    /*change node*/
    if ( IFM_IS_SUBPORT ( ifindex ) )
    {
        vty->node  = TDM_SUBIF_NODE;
        pprompt = vty->change_prompt;
        if ( pprompt )
        {
            /* format the prompt */
            snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-tdm%d/%d/%d.%d)#",
                       IFM_UNIT_ID_GET ( ifindex ),
                       IFM_SLOT_ID_GET ( ifindex ),
                       IFM_PORT_ID_GET ( ifindex ),
                       IFM_SUBPORT_ID_GET ( ifindex ) );

        }

    }
    else
    {
        vty->node  = TDM_IF_NODE;
        pprompt = vty->change_prompt;
        if ( pprompt )
        {
            /* format the prompt */
            snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-tdm%d/%d/%d)#",
                       IFM_UNIT_ID_GET ( ifindex ),
                       IFM_SLOT_ID_GET ( ifindex ),
                       IFM_PORT_ID_GET ( ifindex ) );

        }

    }
    vty->index = ( void * ) ifindex;

    return CMD_SUCCESS;
}

DEFUN ( physical_stm_if,
        physical_stm_if_cmd,
        "interface stm USP",
        CLI_INTERFACE_STR
        CLI_INTERFACE_STM_STR
        CLI_INTERFACE_STM_VHELP_STR )

{
    struct ifm_entry *pifm = NULL;
    uint32_t ifindex = 0;
    int ret = 0;
    char *pprompt = NULL;

    ifindex = ifm_get_ifindex_by_name ( "stm", ( char * ) argv[0] );
    if ( ifindex == 0 )
    {
	    ifm_cli_parse_erro ( vty, IFM_ERR_PARAM_FORMAT );
        return CMD_WARNING;
    }

    /* 判断是子接口 */
    if ( IFM_IS_SUBPORT ( ifindex ) )
    {
        ret = ifm_create_sub_stm ( ifindex );
        if ( ret < 0 )
        {
            ifm_cli_parse_erro ( vty, ret );
            return CMD_WARNING;
        }

    }
    else
    {
        pifm = ifm_lookup ( ifindex );
        if ( !pifm )
        {
	        ifm_cli_parse_erro ( vty, IFM_ERR_PORT_NOT_FOUND );
            return CMD_WARNING;
        }
    }

    /*change node*/
    if ( IFM_IS_SUBPORT ( ifindex ) )
    {
        vty->node  = STM_SUBIF_NODE;
        pprompt = vty->change_prompt;
        if ( pprompt )
        {
            /* format the prompt */
            snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-stm%d/%d/%d.%d)#",
                       IFM_UNIT_ID_GET ( ifindex ),
                       IFM_SLOT_ID_GET ( ifindex ),
                       IFM_PORT_ID_GET ( ifindex ),
                       IFM_SUBPORT_ID_GET ( ifindex ) );

        }

    }
    else
    {
        vty->node  = STM_IF_NODE;
        pprompt = vty->change_prompt;
        if ( pprompt )
        {
            /* format the prompt */
            snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-stm%d/%d/%d)#",
                       IFM_UNIT_ID_GET ( ifindex ),
                       IFM_SLOT_ID_GET ( ifindex ),
                       IFM_PORT_ID_GET ( ifindex ) );

        }

    }
    vty->index = ( void * ) ifindex;

    return CMD_SUCCESS;
}


DEFUN ( tunnel_if,
        tunnel_if_cmd,
        "interface tunnel USP",
        CLI_INTERFACE_STR
        CLI_INTERFACE_TUNNEL_STR
        CLI_INTERFACE_TUNNEL_VHELP_STR )

{
    int ret = 0;
	unsigned int devType = 0;
    uint32_t ifindex = 0;
    char *pprompt = NULL;
	struct ifm_usp usp;

	memset(&usp, 0, sizeof(struct ifm_usp));
	
    devm_comm_get_id(1, 0, MODULE_ID_IFM, &devType );

    ifindex = ifm_get_ifindex_by_name ( "tunnel", ( char * ) argv[0] );
    if ( ifindex == 0 )
    {
	    ifm_cli_parse_erro ( vty, IFM_ERR_PARAM_FORMAT );
        return CMD_WARNING;
    }
	ret = ifm_get_usp_by_ifindex(ifindex, &usp);
	if ( ret != 0 )
	{
		ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
	}
	if ( devType == ID_HT201  || devType == ID_HT201E )
	{
		if( usp.sub_port > 128 )
		{
			ifm_cli_parse_erro ( vty, IFM_ERR_PARAM_TUNNEL_RANGE_128 );
       		return CMD_WARNING;
		}
	}
	if( devType == ID_HT2200 || devType == ID_HT2200V2 || devType == ID_H9MOLMXE_VX )
	{
		if( usp.sub_port > 2000 )
		{
			ifm_cli_parse_erro ( vty, IFM_ERR_PARAM_TUNNEL_RANGE_2000 );
        	return CMD_WARNING;
		}
	}	

    ret  = ifm_create_tunnel ( ifindex );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    /*change node*/
    vty->node = TUNNEL_IF_NODE;
    pprompt = vty->change_prompt;
    if ( pprompt )
    {
        /* format the prompt */
        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-tunnel%d/%d/%d)#",
                   IFM_UNIT_ID_GET ( ifindex ),
                   IFM_SLOT_ID_GET ( ifindex ),
                   IFM_SUBPORT_ID_GET ( ifindex ) );
    }

    vty->index = ( void * ) ifindex;

    return CMD_SUCCESS;

}


DEFUN ( loopback_if,
        loopback_if_cmd,
        "interface loopback <0-128>",
        CLI_INTERFACE_STR
        CLI_INTERFACE_LOOPBACK_STR
        CLI_INTERFACE_LOOPBACK_VHELP_STR )

{
    int ret = 0;
    uint32_t ifindex = 0;
    char *pprompt = NULL;

    ifindex = ifm_get_ifindex_by_name ( "loopback", ( char * ) argv[0] );
    if ( ifindex == 0 )
    {
        ifm_cli_parse_erro ( vty, IFM_ERR_PARAM_FORMAT );
        return CMD_WARNING;
    }

    ret = ifm_create_loopback ( ifindex );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    /*change node*/
    vty->node = LOOPBACK_IF_NODE;
    pprompt = vty->change_prompt;
    if ( pprompt )
    {
        /* format the prompt */
        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-loopback%d)#",
                   IFM_SUBPORT_ID_GET ( ifindex ) );
    }

    vty->index = ( void * ) ifindex;

    return CMD_SUCCESS;
}


DEFUN ( trunk_if,
        trunk_if_cmd,
        "interface trunk TRUNK",
        CLI_INTERFACE_STR
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR )
{
    int ret = 0;
    uint32_t ifindex = 0;
    char *pprompt = NULL;

    ifindex = ifm_get_ifindex_by_name ( "trunk", ( char * ) argv[0] );
    if ( ifindex == 0 )
    {
        ifm_cli_parse_erro ( vty, IFM_ERR_PARAM_FORMAT );
        return CMD_WARNING;
    }

    /* 判断是子接口 */
    if ( IFM_IS_SUBPORT ( ifindex ) )
    {
        ret = ifm_create_sub_trunk ( ifindex );
        if ( ret < 0 )
        {
            ifm_cli_parse_erro ( vty, ret );
            return CMD_WARNING;
        }

    }
    else
    {

        ret = ifm_create_trunk ( ifindex );
        if ( ret < 0 )
        {
            ifm_cli_parse_erro ( vty, ret );
            return CMD_WARNING;
        }

    }
    /*change node*/
    if ( IFM_IS_SUBPORT ( ifindex ) )
    {

        vty->node = TRUNK_SUBIF_NODE;
        pprompt = vty->change_prompt;
        if ( pprompt )
        {
            /* format the prompt */
            snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-trunk%d.%d)#",
                       IFM_PORT_ID_GET ( ifindex ),
                       IFM_SUBPORT_ID_GET ( ifindex ) );
        }
    }
    else
    {
        vty->node = TRUNK_IF_NODE;
        pprompt = vty->change_prompt;
        if ( pprompt )
        {
            /* format the prompt */
            snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-trunk%d)#",
                       IFM_PORT_ID_GET ( ifindex ) );

        }
    }

    vty->index = ( void * ) ifindex;

    return CMD_SUCCESS;
}


DEFUN ( trunk_member_port_set,
        trunk_member_port_set_cmd,
        "trunk <1-128> {passive|priority <0-65535>}",
        "Set trunk member\n"
        "The value of trunk id\n"
        "Passive mode\n"
        "Port priority\n"
        "The value of port priority\n" )
{
    int ret = 0;
    uint32_t ifindex = 0;
    uint32_t tifindex = 0;
    struct ifm_entry *tpifm = NULL;
    struct ifm_entry *pifm = NULL;
    uint16_t id = 0;

    ifindex = ( uint32_t ) vty->index;
    VTY_GET_INTEGER_RANGE ( "trunkid", id, argv[0], 1, 128 );
    tifindex = ifm_get_ifindex_by_name ( "trunk", ( char * ) argv[0] );
    if ( tifindex == 0 )
    {
	    ifm_cli_parse_erro ( vty, IFM_ERR_PARAM_FORMAT );	
        return CMD_WARNING;
    }

    tpifm = ifm_lookup ( tifindex );
    if ( !tpifm )
    {
	    ifm_cli_parse_erro ( vty, IFM_ERR_TRUNK_NO_EXIST );
        return CMD_WARNING;
    }

    pifm = ifm_lookup ( ifindex );
    if ( !pifm )
    {
        ifm_cli_parse_erro ( vty, IFM_ERR_NOT_FOUND );
        return CMD_WARNING;
    }

    if ( pifm->ifm.parent == 0 )
    {
        ret = ifm_set_trunk ( ifindex, id, IFM_OPCODE_ADD );
        if ( ret < 0 )
        {
            ifm_cli_parse_erro ( vty, IFM_ERR_GENERAL );
            return CMD_WARNING;
        }
    }
    else
    {
        zlog_debug ( IFM_DBG_COMMON,"%s[%d] pifm->ifm.trunkid = %d\n", __FUNCTION__, __LINE__, pifm->ifm.trunkid );
    }

    return CMD_SUCCESS;

}

DEFUN ( no_trunk_member_port_set,
        no_trunk_member_port_set_cmd,
        "no trunk <1-128> {passive|priority}",
        NO_STR
        "Set trunk member\n"
        "The value of trunk id\n"
        "Passive mode\n"
        "Port priority\n" )
{
    int ret = 0;
    uint32_t tifindex = 0;
    uint32_t ifindex = 0;
    struct ifm_entry *tpifm = NULL;
    struct ifm_entry *pifm = NULL;

    zlog_debug ( IFM_DBG_COMMON,"%s[%d] argc %d \n", __FUNCTION__, __LINE__, argc );

    if ( argv[1] == NULL && argv[2] == NULL )
    {
        ifindex = ( uint32_t ) vty->index;
        tifindex = ifm_get_ifindex_by_name ( "trunk", ( char * ) argv[0] );
        if ( tifindex == 0 )
        {
            ifm_cli_parse_erro ( vty, IFM_ERR_PARAM_FORMAT );	
            return CMD_WARNING;
        }

        pifm = ifm_lookup ( ifindex );
        if ( !pifm )
        {
            ifm_cli_parse_erro ( vty, IFM_ERR_NOT_FOUND );
            return CMD_WARNING;
        }
        zlog_debug (IFM_DBG_COMMON, "%s[%d] pifm->ifm.parent = 0x%0x tifindex 0x%0x ifindex 0x%0x pifm->ifm.trunkid = %d trunkid = %d\n",
                     __FUNCTION__, __LINE__, pifm->ifm.parent, tifindex,
                     ifindex, pifm->ifm.trunkid, IFM_TRUNK_ID_GET ( tifindex ) );

        if ( pifm->ifm.parent == tifindex )
        {
            tpifm = ifm_lookup ( tifindex );
            if ( !tpifm )
            {
                ifm_cli_parse_erro ( vty, IFM_ERR_TRUNK_NO_EXIST );	
                return CMD_WARNING;
            }

            ret = ifm_set_trunk ( ifindex, IFM_TRUNK_ID_GET ( tifindex ), IFM_OPCODE_DELETE );
            if ( ret < 0 )
            {
                ifm_cli_parse_erro ( vty, IFM_ERR_GENERAL );
                return CMD_WARNING;
            }
        }
        else
        {
            zlog_debug ( IFM_DBG_COMMON,"%s[%d] pifm->ifm.trunkid = %d trunkid = %d\n", __FUNCTION__, __LINE__, pifm->ifm.trunkid,
                         IFM_TRUNK_ID_GET ( tifindex ) );
        }
    }

    return CMD_SUCCESS;
}


DEFUN ( vlanif_if,
        vlanif_if_cmd,
        "interface vlanif <1-4094>",
        CLI_INTERFACE_STR
        CLI_INTERFACE_VLANIF_STR
        CLI_INTERFACE_VLANIF_VHELP_STR )
{
    int ret = 0;
    uint32_t ifindex = 0;
    char *pprompt = NULL;
	unsigned int devType = 0;
	int vlan = 0;
	uint16_t *vlan_table = NULL;
	struct ipc_mesg_n *pRevmsg = NULL;
	
	
	devm_comm_get_id(1, 0, MODULE_ID_IFM, &devType );

	VTY_GET_INTEGER_RANGE ( "vlan", vlan, argv[0], IFM_VLAN_MIN, IFM_VLAN_MAX );
	
	ifindex = ifm_get_ifindex_by_name ( "vlanif", ( char * ) argv[0] );
    if ( ifindex == 0 )
    {
        ifm_cli_parse_erro ( vty, IFM_ERR_PARAM_FORMAT );
        return CMD_WARNING;
    }
	
	if( devType == ID_HT157 || devType == ID_HT158 )
	{
		//vlan_table = ( uint16_t * )ipc_send_common_wait_reply( NULL, 0, 1, MODULE_ID_L2, MODULE_ID_IFM,
		//	IPC_TYPE_RESERVE_VLAN, 0, IPC_OPCODE_GET, ifindex);
		pRevmsg = ipc_sync_send_n2(NULL, 0, 0, MODULE_ID_L2, MODULE_ID_IFM,
                               IPC_TYPE_RESERVE_VLAN, 0, IPC_OPCODE_GET, ifindex, 0);
		 if(pRevmsg)
	 	{
	 	    if(pRevmsg->msghdr.opcode == IPC_OPCODE_REPLY)
	        {  
	             ret = 0;
	             vlan_table = pRevmsg->msg_data;
	        }

	        mem_share_free(pRevmsg,  MODULE_ID_IFM);
		}
		
		
		if( vlan_table != NULL )
		{
			memcpy(default_vlan_table,vlan_table,9);			
			
			zlog_debug(IFM_DBG_COMMON,"%s[%d] vlan[1] %d vlan[2] %d vlan[3] %d vlan[4] %d vlan[5] %d vlan[6] %d vlan[7] %d vlan[8] %d vlan[9] %d\n",
				__FUNCTION__,__LINE__,default_vlan_table[0],default_vlan_table[1],default_vlan_table[2],
				default_vlan_table[3],default_vlan_table[4],default_vlan_table[5],default_vlan_table[6],
				default_vlan_table[7],default_vlan_table[8]);
		}
		if( ( vlan == default_vlan_table[0] ) || (  vlan == default_vlan_table[1] ) || ( vlan == default_vlan_table[2] ) 
			|| ( vlan == default_vlan_table[3] ) || ( vlan == default_vlan_table[4] ) || ( vlan == default_vlan_table[5] )
			|| ( vlan == default_vlan_table[6] ) || ( vlan == default_vlan_table[7] ) || ( vlan == default_vlan_table[8] ))
		{
			ifm_cli_parse_erro ( vty, IFM_ERR_VLANIF );
        	return CMD_WARNING;
			
		}
	}   

    ret = ifm_create_vlanif ( ifindex );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    /*change node*/
    vty->node = VLANIF_NODE;
    pprompt = vty->change_prompt;
    if ( pprompt )
    {
        /* format the prompt */
        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-vlanif%d)#",
                   IFM_SUBPORT_ID_GET ( ifindex ) );
    }
    vty->index = ( void * ) ifindex;

    return CMD_SUCCESS;

}

DEFUN ( e1_if,
        e1_if_cmd,
        "interface e1 USP",
        CLI_INTERFACE_STR
        CLI_INTERFACE_E1_STR
        CLI_INTERFACE_E1_VHELP_STR )

{
    uint32_t ifindex = 0;
    char *pprompt = NULL;
	struct ifm_entry *pifm = NULL;

    ifindex = ifm_get_ifindex_by_name ( "e1", ( char * ) argv[0] );
    if ( ifindex == 0 )
    {
	    ifm_cli_parse_erro ( vty, IFM_ERR_PARAM_FORMAT );
        return CMD_WARNING;
    }
	
	pifm = ifm_lookup ( ifindex );
	if ( !pifm )
	{		
		ifm_cli_parse_erro ( vty, IFM_ERR_PORT_NOT_FOUND );
		return CMD_WARNING;
	}

    /*change node*/
    vty->node = E1_IF_NODE;
    pprompt = vty->change_prompt;
    if ( pprompt )
    {
        /* format the prompt */
        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-e1%d/%d/%d)#",
                   IFM_UNIT_ID_GET ( ifindex ),
                   IFM_SLOT_ID_GET ( ifindex ),
                   IFM_PORT_ID_GET ( ifindex ) );
    }

    vty->index = ( void * ) ifindex;

    return CMD_SUCCESS;

}


DEFUN ( no_if,
        no_if_cmd,
        "no interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP|tdm USP|stm USP|vlanif <1-4094>| trunk TRUNK|loopback <1-128>|tunnel USP}",
        NO_STR
        CLI_INTERFACE_STR
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
        CLI_INTERFACE_TDM_STR
        CLI_INTERFACE_TDM_VHELP_STR
        CLI_INTERFACE_STM_STR
        CLI_INTERFACE_STM_VHELP_STR
        CLI_INTERFACE_VLANIF_STR
        CLI_INTERFACE_VLANIF_VHELP_STR
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR
        CLI_INTERFACE_LOOPBACK_STR
        CLI_INTERFACE_LOOPBACK_VHELP_STR
        CLI_INTERFACE_TUNNEL_STR
        CLI_INTERFACE_TUNNEL_VHELP_STR )
{
    int ret = 0;
    int ifindex = 0;

    if ( argv[0] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "ethernet", ( char * ) argv[0] );
        if ( !IFM_IS_SUBPORT ( ifindex ) )
        {
	        ifm_cli_parse_erro ( vty, IFM_ERR_PORT_NOSUPPORT_DELETE );
            return CMD_WARNING;
        }

    }
    else if ( argv[1] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "gigabitethernet", ( char * ) argv[1] );
        if ( !IFM_IS_SUBPORT ( ifindex ) )
        {
            ifm_cli_parse_erro ( vty, IFM_ERR_PORT_NOSUPPORT_DELETE );
            return CMD_WARNING;
        }

    }
	else if ( argv[2] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "xgigabitethernet", ( char * ) argv[2] );
        if ( !IFM_IS_SUBPORT ( ifindex ) )
        {
            ifm_cli_parse_erro ( vty, IFM_ERR_PORT_NOSUPPORT_DELETE );
            return CMD_WARNING;
        }

    }
    else if ( argv[3] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "tdm", ( char * ) argv[3] );
        if ( !IFM_IS_SUBPORT ( ifindex ) )
        {
            ifm_cli_parse_erro ( vty, IFM_ERR_PORT_NOSUPPORT_DELETE );
            return CMD_WARNING;
        }

    }
    else if ( argv[4] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "stm", ( char * ) argv[4] );
        if ( !IFM_IS_SUBPORT ( ifindex ) )
        {
            ifm_cli_parse_erro ( vty, IFM_ERR_PORT_NOSUPPORT_DELETE );
            return CMD_WARNING;
        }

    }
    else if ( argv[5] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "vlanif", ( char * ) argv[5] );
    }
    else if ( argv[6] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "trunk", ( char * ) argv[6] );
    }
    else if ( argv[7] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "loopback", ( char * ) argv[7] );
    }
    else if ( argv[8] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "tunnel", ( char * ) argv[8] );
    }

    if ( ifindex == 0 )
    {
        ifm_cli_parse_erro ( vty, IFM_ERR_PARAM_FORMAT );
        return CMD_WARNING;
    }

    ret = ifm_delete ( ifindex );
    if ( ret < 0 )
    {
         ifm_cli_parse_erro ( vty, IFM_ERR_GENERAL );
        return CMD_WARNING;
    }

    return CMD_SUCCESS;

}

DEFUN ( if_shutdown,
        if_shutdown_cmd,
        "shutdown",
        "Shutdown the selected interface\n" )
{
    int ret = 0;
    uint8_t shutdown = IFNET_SHUTDOWN;
    uint32_t ifindex;

    ifindex = ( uint32_t ) vty->index;

    ret = ifm_set_info ( ifindex, ( void * ) &shutdown, IFNET_INFO_SHUTDOWN, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN ( if_no_shutdown,
        if_no_shutdown_cmd,
        "no shutdown",
        NO_STR
        "Shutdown the selected interface\n" )
{
    int ret = 0;
    uint8_t shutdown = IFNET_NO_SHUTDOWN;
    uint32_t ifindex;

    ifindex = ( uint32_t ) vty->index;


    ret = ifm_set_info ( ifindex, ( void * ) &shutdown, IFNET_INFO_SHUTDOWN, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN ( if_mtu,
        if_mtu_cmd,
        "mtu <64-9550>",
        "Interface MTU\n"
        "The value of Interface MTU (bytes)\n")
{
    int ret = 0;
    uint16_t mtu = 0;
    uint32_t ifindex;

	VTY_GET_INTEGER_RANGE ( "mtu", mtu, argv[0], IFM_MTU_MIN, IFM_MTU_MAX );

	ifindex = ( uint32_t ) vty->index;
	ret = ifm_set_info ( ifindex, ( void * ) &mtu, IFNET_INFO_MTU, IFM_OPCODE_UPDATE );

	if ( ret < 0 )
	{
		ifm_cli_parse_erro ( vty, ret );
		return CMD_WARNING;
	}

    return CMD_SUCCESS;
}


DEFUN ( if_phy_mode,
        if_phy_mode_cmd,
        "phy mode (auto | master | slave)",
        "Phy set\n"
        "Phy mode set\n"
        "Phy auto\n"
        "Phy master\n"
        "Phy slave\n")
{
    int ret = 0;
    uint32_t ms = 0;
    uint32_t ifindex;

	if(argv[0][0] == 'a')
	{
        ms = PORT_MS_AUTO;
	}
	else if(argv[0][0] == 'm')
	{
        ms = PORT_MS_MASTER;
	}
	else
	{
        ms = PORT_MS_SLAVE;
	}

	ifindex = ( uint32_t ) vty->index;
	ret = ifm_set_info ( ifindex, ( void * ) &ms, IFNET_INFO_PHYMASTER, IFM_OPCODE_UPDATE );
    
	if ( ret < 0 )
	{
		ifm_cli_parse_erro ( vty, ret );
		return CMD_WARNING;
	}

//    printf("%s: if %x, ms = %d \n", __func__, ifindex, ms);
    return CMD_SUCCESS;
}



DEFUN ( no_if_mtu,
		no_if_mtu_cmd,
		"no mtu",
		NO_STR
		"The value of Interface MTU (bytes)\n")
{
	int ret = 0;
	uint16_t mtu = 0;
	uint32_t ifindex;

	mtu = IFM_DEF_MTU;
	ifindex = ( uint32_t ) vty->index;
	ret = ifm_set_info ( ifindex, ( void * ) &mtu, IFNET_INFO_MTU, IFM_OPCODE_UPDATE );

	if ( ret < 0 )
	{
		ifm_cli_parse_erro ( vty, ret );
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}

		
DEFUN ( if_jumboframe,
		if_jumboframe_cmd,
		"jumbo-frame <1518-12288>",
		"Interface jumbo frame\n"
		"The value of Interface jumbo frame (bytes)\n" )
{
	int ret = 0;
	uint16_t jumbo = 0;
	uint32_t ifindex;

	VTY_GET_INTEGER_RANGE ( "jumbo", jumbo, argv[0], IFM_JUMBO_MIN, IFM_JUMBO_MAX );
	ifindex = ( uint32_t ) vty->index;
	ret = ifm_set_info ( ifindex, ( void * ) &jumbo, IFNET_INFO_JUMBO, IFM_OPCODE_UPDATE );	

	if ( ret < 0 )
	{
		ifm_cli_parse_erro ( vty, ret );
		return CMD_WARNING;
	}

    return CMD_SUCCESS;
}

DEFUN ( no_if_jumboframe,
		no_if_jumboframe_cmd,
		"no jumbo-frame",
		NO_STR
		"The value of Interface jumbo frame (bytes)\n" )
{
	int ret = 0;
	uint16_t jumbo = 0;
	uint32_t ifindex;

	jumbo = IFM_DEF_JUMBO;
	ifindex = ( uint32_t ) vty->index;
	ret = ifm_set_info ( ifindex, ( void * ) &jumbo, IFNET_INFO_JUMBO, IFM_OPCODE_UPDATE );

	if ( ret < 0 )
	{
		ifm_cli_parse_erro ( vty, ret );
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}


DEFUN ( if_flapping_control,
        if_flapping_control_cmd,
        "flapping-control period <0-255>",
        "Interface flapping-control\n"
        "Period of Interface flapping-control\n"
        "Value of Interface flapping-control (seconds)\n" )
{
    int ret = 0;
    uint32_t ifindex = 0;
    uint8_t flap_period = 0;

    VTY_GET_INTEGER_RANGE ( "period", flap_period, argv[0], IFM_FLAP_PERIOD_MIN, IFM_FLAP_PERIOD_MAX );
    ifindex = ( uint32_t ) vty->index;
    ret = ifm_set_info ( ifindex, ( void * ) &flap_period, IFNET_INFO_FLAP, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN ( if_duplex ,
        if_duplex_cmd,
        "operation (duplex | simplex | auto)",
        "Operation Interface rate mode\n"
        "Duplex of Interface rate mode\n"
        "Simplex of Interface rate mode\n"
        "AutoNegotiation of Interface rate mode\n" )
{
    int ret = 0;
    struct ifm_port speed;
    uint32_t ifindex;

    ifindex = ( uint32_t ) vty->index;


    if ( strncmp ( argv[0], "duplex", 6 ) == 0 )
    {
        speed.duplex  = IFNET_DUPLEX_FULL;
        speed.autoneg = IFNET_SPEED_MODE_FORCE;
        speed.speed   = IFNET_SPEED_INVALID;
    }
    else if ( strncmp ( argv[0], "simplex", 7 ) == 0 )
    {
        speed.duplex  = IFNET_DUPLEX_HALF;
        speed.autoneg = IFNET_SPEED_MODE_FORCE;
        speed.speed   = IFNET_SPEED_INVALID;

    }
    else
    {
        speed.autoneg = IFNET_SPEED_MODE_AUTO;
        speed.duplex  = IFNET_SPEED_INVALID;
        speed.speed   = IFNET_SPEED_INVALID;

    }

    ret = ifm_set_info ( ifindex, ( void * ) &speed, IFNET_INFO_SPEED, IFM_OPCODE_UPDATE );

    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }


    return CMD_SUCCESS;
}
DEFUN ( if_combo ,
        if_combo_cmd,
        "combo enable (copper | fiber)",
        "Combo config set\n"
        "Enable copper or fiber\n"
        "Enable copper\n"
        "Enable fiber\n" )
{
    int ret = 0;
    uint8_t fiber_sub = 0;
    uint32_t ifindex;

    ifindex = ( uint32_t ) vty->index;


    if ( strncmp ( argv[0], "copper", 6 ) == 0 )
    {
        fiber_sub  = (uint8_t)IFNET_FIBERTYPE_COPPER;
    }
    else if ( strncmp ( argv[0], "fiber", 5 ) == 0 )
    {
    	fiber_sub  = (uint8_t)IFNET_FIBERTYPE_FIBER;
    }
	
    ret = ifm_set_info ( ifindex, ( void * ) &fiber_sub, IFNET_INFO_COMBO, IFM_OPCODE_UPDATE );

    if ( ret != 0 )
    {
    	 ifm_cli_parse_erro ( vty, ret );
		 return CMD_WARNING;
		//vty_error_out ( vty, "%s %s",errcode_get_string( ret), VTY_NEWLINE );
        //return ret ;
    }

    return CMD_SUCCESS;
}

/* 配置 GE 接口的速率模式*/
DEFUN ( if_speed,
        if_speed_cmd,
        "speed (100 | 1000 )",
        "Interface rate\n"
        "Fe rate type\n"
        "GE rate type\n"
        "10ge rate type\n"
        "Port self negotiation mode\n" )
{
    int ret = 0;
    uint32_t ifindex = 0;
    struct ifm_port speed;
    unsigned long  t_speed ;
    char *endptr = NULL ;
    ifindex = ( uint32_t ) vty->index;
    if ( argv[0] != NULL )
    {
        t_speed = strtoul ( argv[0], &endptr, 10 );
        if ( t_speed == 100 )
        {
            speed.duplex  = IFNET_DUPLEX_INVALID;
            speed.autoneg = IFNET_SPEED_MODE_INVALID;
            speed.speed = IFNET_SPEED_FE;
        }
        else if ( t_speed == 1000 )
        {
            speed.duplex  = IFNET_DUPLEX_INVALID;
            speed.autoneg = IFNET_SPEED_MODE_INVALID;
            speed.speed = IFNET_SPEED_GE;
        }
        else if ( t_speed == 10000 )
        {
            speed.duplex  = IFNET_DUPLEX_INVALID;
            speed.autoneg = IFNET_SPEED_MODE_INVALID;
            speed.speed = IFNET_SPEED_10GE;

        }
        ret = ifm_set_info ( ifindex, ( void * ) &speed, IFNET_INFO_SPEED, IFM_OPCODE_UPDATE );

        if ( ret < 0 )
        {
            ifm_cli_parse_erro ( vty, ret );
            return CMD_WARNING;
        }

    }
    else
    {
        zlog_err ( "%s[%d] the wrong speed format", __FUNCTION__, __LINE__ );
        return CMD_WARNING;
    }
    return CMD_SUCCESS;
}



//EtherType：以太网类型字段
DEFUN ( if_tpid,
        if_tpid_cmd,
        "tpid  (0x9100|0x9200|0x88a8)",
        "Interface tpid\n"
        "Tpid value\n" )
{
    int ret = 0;
    uint16_t tpid = 0;
    uint32_t ifindex;

    if ( strncmp ( argv[0], "0x9100", 6 ) == 0 )
    {
        tpid = 0x9100;
    }
    else if ( strncmp ( argv[0], "0x9200", 6 ) == 0 )
    {
        tpid = 0x9200;
    }
    else if ( strncmp ( argv[0], "0x88a8", 6 ) == 0 )
    {
        tpid = 0x88a8;
    }

    ifindex = ( uint32_t ) vty->index;

    ret = ifm_set_info ( ifindex, ( void * ) &tpid, IFNET_INFO_TPID, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN ( if_no_tpid,
        if_no_tpid_cmd,
        "no tpid",
        "Delete\n"
        "Interface tpid\n" )
{
    int ret = 0;
    uint16_t tpid = 0;
    uint32_t ifindex;

    tpid = IFM_DEF_TPID;

    ifindex = ( uint32_t ) vty->index;
    ret = ifm_set_info ( ifindex, ( void * ) &tpid, IFNET_INFO_TPID, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/*DEFUN ( if_mac,
        if_mac_cmd,
        "mac XX:XX:XX:XX:XX:XX",
        "Interface mac\n"
        "Mac address\n" )
{
    int ret = 0;
    uint32_t ifindex = 0;
    uint16_t mac[6] = {0};
    uchar cmac[6] = {0};
    int i;

    if ( cmd_check_macform ( ( char * ) argv[0] ) == -1 )
    {
	    ifm_cli_parse_erro( vty,  IFM_ERR_PARAM_MAC_INVALID);
        return CMD_WARNING;
    }

    if ( sscanf ( argv[0], "%hx:%hx:%hx:%hx:%hx:%hx",
                  &mac[0], &mac[1],
                  &mac[2], &mac[3],
                  &mac[4], &mac[5] ) != 6 )
    {
	    ifm_cli_parse_erro ( vty, IFM_ERR_PARAM_MAC_INVALID );	
        return CMD_WARNING;
    }

    for ( i = 0 ; i < 6; i++ )
    { cmac[i] = mac[i]; }

    ifindex = ( uint32_t ) vty->index;
    ret = ifm_set_info ( ifindex, ( void * ) cmac, IFNET_INFO_MAC, IFM_OPCODE_UPDATE );

    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}*/


DEFUN ( if_mode_change,
        if_mode_change_cmd,
        "mode (l2|l3|switch)",
        "Interface mode\n"
        "L2vpn mode\n"
        "L3 mode\n" )
{
    int ret = 0;
    uint8_t mode = IFNET_MODE_L3;
    uint32_t ifindex;

     /*恢复保存不需要用户确认*/
	if(vty->config_read_flag)
	{
		if ( strncmp ( argv[0], "l2", 2 ) == 0 )
		{
			mode = IFNET_MODE_L2;
		}
		else if ( strncmp ( argv[0], "switch", 2 ) == 0 )
		{
			mode = IFNET_MODE_SWITCH;
		}
		else
		{
			mode = IFNET_MODE_L3;
		}
		
		ifindex = ( uint32_t ) vty->index;
		ret = ifm_set_info ( ifindex, ( void * ) &mode, IFNET_INFO_MODE, IFM_OPCODE_UPDATE );
		if ( ret < 0 )
		{
			ifm_cli_parse_erro ( vty, ret );
			return CMD_WARNING;
		}
		vty_out ( vty, "%%Change the mode to %s success!%s", argv[0], VTY_NEWLINE );
		return CMD_SUCCESS;

	}
	else
	{	
    	    switch ( vty->cmd_replay )
	    {
	        case VTY_REPLAY_NONE:
	            vty_out ( vty, "%%Configuration based on original mode will be cleared,change the mode?(Y/N)%s", VTY_NEWLINE );
	            return CMD_REPLAY;

	        case VTY_REPLAY_YES:
	            if ( strncmp ( argv[0], "l2", 2 ) == 0 )
	            {
	                mode = IFNET_MODE_L2;
	            }
	            else if ( strncmp ( argv[0], "switch", 2 ) == 0 )
	            {
	                mode = IFNET_MODE_SWITCH;
	            }
	            else
	            {
	                mode = IFNET_MODE_L3;
	            }

	            ifindex = ( uint32_t ) vty->index;
	            ret = ifm_set_info ( ifindex, ( void * ) &mode, IFNET_INFO_MODE, IFM_OPCODE_UPDATE );
	            if ( ret < 0 )
	            {
	                ifm_cli_parse_erro ( vty, ret );
	                return CMD_WARNING;
	            }
	            vty_out ( vty, "%%Change the mode to %s success!%s", argv[0], VTY_NEWLINE );
	            return CMD_SUCCESS;

	        case VTY_REPLAY_NO:
	            vty_out ( vty, "%%No change the mode%s", VTY_NEWLINE );
	            return CMD_SUCCESS;
	    }
    }


    return CMD_SUCCESS;
}


DEFUN ( subif_encapsulate_untag,
        subif_encapsulate_untag_cmd,
        "encapsulate untag",
        "Subif encapsulate\n"
        "Untag of ethernet frame\n" )
{
    int ret = 0;
    uint32_t ifindex = 0;
    struct ifm_encap encap;

    memset ( &encap, 0, sizeof ( struct ifm_encap ) );
    ifindex = ( uint32_t ) vty->index;

    encap.type                  = IFNET_ENCAP_UNTAG;  // 0: untag, 1: dot1q, 2: qinq
    encap.svlan.vlan_start = IFM_DEF_SVLANID;
    ret = ifm_set_info ( ifindex, ( void * ) &encap, IFNET_INFO_ENCAP, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN ( subif_encapsulate_dot1q,
        subif_encapsulate_dot1q_cmd,
        "encapsulate dot1q <1-4094> {to <1-4094>}",
        "Subif encapsulate\n"
        "Dot1q vlan encapsulate type\n"
        "The value of vlan\n"
        "The range of vlan\n"
        "The value of end vlan\n" )
{
    int ret = 0;
    uint32_t ifindex = 0;
    struct ifm_encap encap;

    memset ( &encap, 0, sizeof ( struct ifm_encap ) );
    ifindex = ( uint32_t ) vty->index;
    encap.type = IFNET_ENCAP_DOT1Q;

    VTY_GET_INTEGER_RANGE ( "vlan", encap.svlan.vlan_start, argv[0], IFM_VLAN_MIN, IFM_VLAN_MAX );

    if ( NULL == argv[1] )
    {
        encap.svlan.vlan_end  = encap.svlan.vlan_start;
    }
    else
    {
        VTY_GET_INTEGER_RANGE ( "vlan_end", encap.svlan.vlan_end , argv[1], IFM_VLAN_MIN, IFM_VLAN_MAX );

        if ( encap.svlan.vlan_start >= encap.svlan.vlan_end )
        {
            ifm_cli_parse_erro ( vty, IFM_ERR_ENCAP_RANGE );
            return CMD_WARNING;
        }

    }

    ret = ifm_set_info ( ifindex, ( void * ) &encap, IFNET_INFO_ENCAP, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    return CMD_SUCCESS;

}


DEFUN ( subif_encapsulate_qinq,
        subif_encapsulate_qinq_cmd,
        "encapsulate qinq svlan <1-4094> cvlan <1-4094> {to <1-4094>} ",
        "Subif encapsulate\n"
        "Qinq vlan encapsulate type\n"
        "Service provider vlan\n"
        "The value of service provider vlan\n"
        "Customer vlan\n"
        "The value of customer vlan\n"
        "The range of customer vlan\n"
        "The value of end customer vlan\n" )
{
    int ret = 0;
    uint32_t ifindex = 0;
    struct ifm_encap encap;

    memset ( &encap, 0, sizeof ( struct ifm_encap ) );
    ifindex = ( uint32_t ) vty->index;
    encap.type = IFNET_ENCAP_QINQ;

    VTY_GET_INTEGER_RANGE ( "svlan", encap.svlan.vlan_start, argv[0], IFM_VLAN_MIN, IFM_VLAN_MAX );

    encap.svlan.vlan_end = encap.svlan.vlan_start;

    VTY_GET_INTEGER_RANGE ( "cvlan", encap.cvlan.vlan_start, argv[1], IFM_VLAN_MIN, IFM_VLAN_MAX );

    if ( NULL == argv[2] )
    {
        encap.cvlan.vlan_end = encap.cvlan.vlan_start;
    }
    else
    {
        VTY_GET_INTEGER_RANGE ( "cvlan", encap.cvlan.vlan_end, argv[2], IFM_VLAN_MIN, IFM_VLAN_MAX );
        if ( encap.cvlan.vlan_start >= encap.cvlan.vlan_end )
        {
            ifm_cli_parse_erro ( vty, IFM_ERR_ENCAP_RANGE );
            return CMD_WARNING;
        }

    }

    ret = ifm_set_info ( ifindex, ( void * ) &encap, IFNET_INFO_ENCAP, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN ( subif_encapsulate_untag_add,
        subif_encapsulate_untag_add_cmd,
        "encapsulate untag add vlan <1-4094> cos <0-7>",
        "Subif encapsulate\n"
        "Untag of ethernet frame\n"
        "Add vlan\n"
        "Vlan\n"
        "Vlan id value\n"
        "Add cos of vlan\n"
        "Cos value\n" )
{
    int ret = 0;
    uint32_t ifindex = 0;
    struct ifm_encap encap;

    ifindex = ( uint32_t ) vty->index;

    memset ( &encap, 0, sizeof ( struct ifm_encap ) );
    encap.svlan_cos = IFM_VLAN_COS_INVALID;
    encap.cvlan_cos = IFM_VLAN_COS_INVALID;


    encap.type = IFNET_ENCAP_UNTAG;  // 0: untag, 1: dot1q, 2: qinq
    encap.svlan.vlan_start  = IFM_DEF_SVLANID;

    VTY_GET_INTEGER_RANGE ( "vlan", encap.svlan_new.vlan_start, argv[0], IFM_VLAN_MIN, IFM_VLAN_MAX );

    encap.svlan_new.vlan_end = encap.svlan_new.vlan_start;

    VTY_GET_INTEGER_RANGE ( "cos", encap.svlan_cos, argv[1], 0, 7 );


    encap.cvlan_act = VLAN_ACTION_NO;
    encap.svlan_act = VLAN_ACTION_ADD;

    ret = ifm_set_info ( ifindex, ( void * ) &encap, IFNET_INFO_ENCAP, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}



DEFUN ( subif_encapsulate_dot1q_add,
        subif_encapsulate_dot1q_add_cmd,
        "encapsulate dot1q <1-4094> {to <1-4094>} add svlan <1-4094> {cos <0-7>}",
        "Subif encapsulate\n"
        "Dot1q vlan encapsulate type\n"
        "The value of vlan\n"
        "The range of vlan\n"
        "The value of end vlan\n"
        "Add service provider vlan\n"
        "Service provider vlan\n"
        "The value of service provider vlan\n"
        "Add cos of service provider vlan\n"
        "Cos value\n" )

{
    int ret = 0;
    uint32_t ifindex = 0;
    struct ifm_encap encap;

    memset ( &encap, 0, sizeof ( struct ifm_encap ) );
    encap.svlan_cos = IFM_VLAN_COS_INVALID;
    encap.cvlan_cos = IFM_VLAN_COS_INVALID;

    ifindex = ( uint32_t ) vty->index;

    encap.type = IFNET_ENCAP_DOT1Q;

    VTY_GET_INTEGER_RANGE ( "vlan", encap.svlan.vlan_start , argv[0], IFM_VLAN_MIN, IFM_VLAN_MAX );

    if ( NULL == argv[1] )
    {

        encap.svlan.vlan_end = encap.svlan.vlan_start;
    }
    else
    {
        VTY_GET_INTEGER_RANGE ( "vlan_end", encap.svlan.vlan_end, argv[1], IFM_VLAN_MIN, IFM_VLAN_MAX );

        if ( encap.svlan.vlan_start >= encap.svlan.vlan_end )
        {
            ifm_cli_parse_erro ( vty, IFM_ERR_ENCAP_RANGE );
            return CMD_WARNING;
        }

    }


    VTY_GET_INTEGER_RANGE ( "svlan", encap.svlan_new.vlan_start, argv[2], IFM_VLAN_MIN, IFM_VLAN_MAX );

    encap.svlan_new.vlan_end = encap.svlan_new.vlan_start;

    if ( NULL == argv[3] )
    {
        encap.svlan_cos = IFM_VLAN_COS_INVALID;
    }
    else
    {
        VTY_GET_INTEGER_RANGE ( "cos", encap.svlan_cos, argv[3], 0, 7 );
    }

    encap.cvlan_act = VLAN_ACTION_NO;
    encap.svlan_act = VLAN_ACTION_ADD;

    ret = ifm_set_info ( ifindex, ( void * ) &encap, IFNET_INFO_ENCAP, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN ( subif_encapsulate_dot1q_translate_single,
        subif_encapsulate_dot1q_translate_single_cmd,
        "encapsulate dot1q <1-4094> translate vlan <1-4094> {cos <0-7>}",
        "Subif encapsulate\n"
        "Dot1q vlan encapsulate type\n"
        "The value of vlan\n"        
        "Translate vlan \n"
        "Translate single vlan\n"
        "The value of vlan\n"       
        "Cos of vlan\n"
        "Cos value\n" )

{
    int ret = 0;
    uint32_t ifindex = 0;
    struct ifm_encap encap;

    memset ( &encap, 0, sizeof ( struct ifm_encap ) );
    encap.svlan_cos = IFM_VLAN_COS_INVALID;
    encap.cvlan_cos = IFM_VLAN_COS_INVALID;

    ifindex = ( uint32_t ) vty->index;

    encap.type = IFNET_ENCAP_DOT1Q;

    VTY_GET_INTEGER_RANGE ( "vlan", encap.svlan.vlan_start, argv[0], IFM_VLAN_MIN, IFM_VLAN_MAX );
    encap.svlan.vlan_end = encap.svlan.vlan_start;
    VTY_GET_INTEGER_RANGE ( "new vlan", encap.svlan_new.vlan_start, argv[1], IFM_VLAN_MIN, IFM_VLAN_MAX );
    encap.svlan_new.vlan_end = encap.svlan_new.vlan_start;

    if ( NULL == argv[2] )
    {
        encap.svlan_cos = IFM_VLAN_COS_INVALID;
    }
    else
    {
        VTY_GET_INTEGER_RANGE ( "cos", encap.svlan_cos, argv[2], 0, 7 );
    }

    encap.cvlan_act = VLAN_ACTION_NO;
    encap.svlan_act = VLAN_ACTION_TRANSLATE;

    ret = ifm_set_info ( ifindex, ( void * ) &encap, IFNET_INFO_ENCAP, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN ( subif_encapsulate_dot1q_translate_double,
        subif_encapsulate_dot1q_translate_double_cmd,
        "encapsulate dot1q <1-4094> translate svlan <1-4094> {cos <0-7>} cvlan <1-4094> {cos <0-7>}",
        "Subif encapsulate\n"
        "Dot1q vlan encapsulate type\n"
        "Vlan value\n"
        "Translate vlan \n"
        "Translate service provider vlan\n"
        "The value of service provider vlan\n"
        "Cos of service vlan\n"
        "Cos value\n"
        "Translate customer vlan \n"
        "The value of customer vlan\n"
        "Translate cos of customer vlan\n"
        "Cos value\n" )

{
    int ret = 0;
    uint32_t ifindex = 0;
    struct ifm_encap encap;

    memset ( &encap, 0, sizeof ( struct ifm_encap ) );
    encap.svlan_cos = IFM_VLAN_COS_INVALID;
    encap.cvlan_cos = IFM_VLAN_COS_INVALID;

    ifindex = ( uint32_t ) vty->index;

    encap.type = IFNET_ENCAP_DOT1Q;

    VTY_GET_INTEGER_RANGE ( "vlan", encap.svlan.vlan_start, argv[0], IFM_VLAN_MIN, IFM_VLAN_MAX );
    encap.svlan.vlan_end = encap.svlan.vlan_start;

    VTY_GET_INTEGER_RANGE ( "svlan", encap.svlan_new.vlan_start, argv[1], IFM_VLAN_MIN, IFM_VLAN_MAX );
    encap.svlan_new.vlan_end = encap.svlan_new.vlan_start;

    if ( NULL == argv[2] )
    {
        encap.svlan_cos = IFM_VLAN_COS_INVALID;
    }
    else
    {
        VTY_GET_INTEGER_RANGE ( "cos", encap.svlan_cos, argv[2], 0, 7 );
    }

    VTY_GET_INTEGER_RANGE ( "cvlan", encap.cvlan_new.vlan_start, argv[3], IFM_VLAN_MIN, IFM_VLAN_MAX );
    encap.cvlan_new.vlan_end = encap.cvlan_new.vlan_start;

    if ( NULL == argv[4] )
    {
        encap.cvlan_cos = IFM_VLAN_COS_INVALID;
    }
    else
    {
        VTY_GET_INTEGER_RANGE ( "cos", encap.cvlan_cos, argv[4], 0, 7 );
    }

    encap.cvlan_act = VLAN_ACTION_TRANSLATE;
    encap.svlan_act = VLAN_ACTION_ADD;

    ret = ifm_set_info ( ifindex, ( void * ) &encap, IFNET_INFO_ENCAP, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN ( subif_encapsulate_qinq_translate_single,
        subif_encapsulate_qinq_translate_single_cmd,
        "encapsulate qinq svlan <1-4094> cvlan <1-4094> translate vlan <1-4094> {cos <0-7>}",
        "Subif encapsulate\n"
        "Qinq vlan encapsulate type\n"
        "Service provider vlan\n"
        "The value of service vlan\n"
        "Customer vlan\n"
        "The value of customer vlan\n"
        "Translate vlan \n"
        "Translate single vlan \n"
        "Vlan value\n"
        "Cos of vlan\n"
        "Cos value\n" )

{
    int ret = 0;
    uint32_t ifindex = 0;
    struct ifm_encap encap;

    memset ( &encap, 0, sizeof ( struct ifm_encap ) );
    encap.svlan_cos = IFM_VLAN_COS_INVALID;
    encap.cvlan_cos = IFM_VLAN_COS_INVALID;

    ifindex = ( uint32_t ) vty->index;

    encap.type = IFNET_ENCAP_QINQ;

    VTY_GET_INTEGER_RANGE ( "svlan", encap.svlan.vlan_start, argv[0], IFM_VLAN_MIN, IFM_VLAN_MAX );
    encap.svlan.vlan_end = encap.svlan.vlan_start;
    VTY_GET_INTEGER_RANGE ( "cvlan", encap.cvlan.vlan_start, argv[1], IFM_VLAN_MIN, IFM_VLAN_MAX );
    encap.cvlan.vlan_end = encap.cvlan.vlan_start;

    VTY_GET_INTEGER_RANGE ( "vlan", encap.svlan_new.vlan_start, argv[2], IFM_VLAN_MIN, IFM_VLAN_MAX );
    encap.svlan_new.vlan_end = encap.svlan_new.vlan_start;

    if ( NULL == argv[3] )
    {
        encap.svlan_cos = IFM_VLAN_COS_INVALID;
    }
    else
    {
        VTY_GET_INTEGER_RANGE ( "cos", encap.svlan_cos, argv[3], 0, 7 );
    }

    encap.cvlan_act = VLAN_ACTION_DELETE;
    encap.svlan_act = VLAN_ACTION_TRANSLATE;

    ret = ifm_set_info ( ifindex, ( void * ) &encap, IFNET_INFO_ENCAP, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN ( subif_encapsulate_qinq_translate_double,
        subif_encapsulate_qinq_translate_double_cmd,
        "encapsulate qinq svlan <1-4094> cvlan <1-4094> translate svlan <1-4094> {cos <0-7>} cvlan <1-4094> {cos <0-7>}",
        "Subif encapsulate\n"
        "Qinq vlan encapsulate type\n"
        "Service provider vlan\n"
        "The value of service provider vlan\n"
        "Customer vlan\n"
        "The value of customer vlan\n"
        "Translate vlan \n"
        "Translate service provider vlan\n"
        "The value of service provider vlan\n"
        "Cos of service provider vlan\n"
        "Cos value\n"
        "Translate customer vlan \n"
        "The value of customer vlan\n"
        "Translate cos of customer vlan\n"
        "Cos value\n" )

{
    int ret = 0;
    uint32_t ifindex = 0;
    struct ifm_encap encap;

    memset ( &encap, 0, sizeof ( struct ifm_encap ) );
    encap.svlan_cos = IFM_VLAN_COS_INVALID;
    encap.cvlan_cos = IFM_VLAN_COS_INVALID;

    ifindex = ( uint32_t ) vty->index;

    encap.type = IFNET_ENCAP_QINQ;

    VTY_GET_INTEGER_RANGE ( "svlan", encap.svlan.vlan_start, argv[0], IFM_VLAN_MIN, IFM_VLAN_MAX );
    encap.svlan.vlan_end = encap.svlan.vlan_start;
    VTY_GET_INTEGER_RANGE ( "cvlan", encap.cvlan.vlan_start, argv[1], IFM_VLAN_MIN, IFM_VLAN_MAX );
    encap.cvlan.vlan_end = encap.cvlan.vlan_start;

    VTY_GET_INTEGER_RANGE ( "svlan", encap.svlan_new.vlan_start, argv[2], IFM_VLAN_MIN, IFM_VLAN_MAX );
    encap.svlan_new.vlan_end = encap.svlan_new.vlan_start;

    if ( NULL == argv[3] )
    {
        encap.svlan_cos = IFM_VLAN_COS_INVALID;
    }
    else
    {
        VTY_GET_INTEGER_RANGE ( "cos", encap.svlan_cos, argv[3], 0, 7 );
    }

    VTY_GET_INTEGER_RANGE ( "cvlan", encap.cvlan_new.vlan_start, argv[4], IFM_VLAN_MIN, IFM_VLAN_MAX );
    encap.cvlan_new.vlan_end = encap.cvlan_new.vlan_start;

    if ( NULL == argv[5] )
    {
        encap.cvlan_cos = IFM_VLAN_COS_INVALID;
    }
    else
    {
        VTY_GET_INTEGER_RANGE ( "cos", encap.cvlan_cos, argv[5], 0, 7 );
    }

    encap.cvlan_act = VLAN_ACTION_TRANSLATE;
    encap.svlan_act = VLAN_ACTION_TRANSLATE;

    ret = ifm_set_info ( ifindex, ( void * ) &encap, IFNET_INFO_ENCAP, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN ( subif_encapsulate_qinq_translate_outer,
        subif_encapsulate_qinq_translate_outer_cmd,
        "encapsulate qinq svlan <1-4094> cvlan <1-4094> translate svlan <1-4094> {cos <0-7>} cvlan *",
        "Subif encapsulate\n"
        "Qinq vlan encapsulate type\n"
        "Service provider vlan\n"
        "The value of svlan\n"
        "Customer vlan\n"
        "The value of customer vlan\n"
        "Translate vlan \n"
        "Only translate service provider vlan\n"
        "The value of service provider vlan\n"
        "Cos of service provider vlan\n"
        "Cos value\n"
        "Customer vlan\n"
        "Keep the customer vlan\n" )

{
    int ret = 0;
    uint32_t ifindex = 0;
    struct ifm_encap encap;

    memset ( &encap, 0, sizeof ( struct ifm_encap ) );
    encap.svlan_cos = IFM_VLAN_COS_INVALID;
    encap.cvlan_cos = IFM_VLAN_COS_INVALID;

    ifindex = ( uint32_t ) vty->index;

    encap.type = IFNET_ENCAP_QINQ;

    VTY_GET_INTEGER_RANGE ( "svlan", encap.svlan.vlan_start, argv[0], IFM_VLAN_MIN, IFM_VLAN_MAX );
    encap.svlan.vlan_end = encap.svlan.vlan_start;
    VTY_GET_INTEGER_RANGE ( "cvlan", encap.cvlan.vlan_start, argv[1], IFM_VLAN_MIN, IFM_VLAN_MAX );
    encap.cvlan.vlan_end = encap.cvlan.vlan_start;

    VTY_GET_INTEGER_RANGE ( "svlan", encap.svlan_new.vlan_start, argv[2], IFM_VLAN_MIN, IFM_VLAN_MAX );
    encap.svlan_new.vlan_end = encap.svlan_new.vlan_start;

    if ( NULL == argv[3] )
    {
        encap.svlan_cos = IFM_VLAN_COS_INVALID;
    }
    else
    {
        VTY_GET_INTEGER_RANGE ( "cos", encap.svlan_cos, argv[3], 0, 7 );
    }

    encap.cvlan_act = VLAN_ACTION_NO;
    encap.svlan_act = VLAN_ACTION_TRANSLATE;

    ret = ifm_set_info ( ifindex, ( void * ) &encap, IFNET_INFO_ENCAP, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN ( subif_encapsulate_qinq_translate_delete,
        subif_encapsulate_qinq_translate_delete_cmd,
        "encapsulate qinq svlan <1-4094> cvlan <1-4094> {to <1-4094>} delete svlan",
        "Subif encapsulate\n"
        "Qinq vlan encapsulate type\n"
        "Service provider VLAN\n"
        "The value of svlan\n"
        "Customer vlan\n"
        "The value of customer vlan\n"
        "The range of customer vlan\n"
        "The value of end customer vlan\n"
        "Delete vlan\n"
        "Delete service provider vlan\n" )

{
    int ret = 0;
    uint32_t ifindex = 0;
    struct ifm_encap encap;

    memset ( &encap, 0, sizeof ( struct ifm_encap ) );
    encap.svlan_cos = IFM_VLAN_COS_INVALID;
    encap.cvlan_cos = IFM_VLAN_COS_INVALID;

    ifindex = ( uint32_t ) vty->index;

    encap.type = IFNET_ENCAP_QINQ;


    VTY_GET_INTEGER_RANGE ( "svlan", encap.svlan.vlan_start, argv[0], IFM_VLAN_MIN, IFM_VLAN_MAX );
    encap.svlan.vlan_end = encap.svlan.vlan_start;

    VTY_GET_INTEGER_RANGE ( "cvlan", encap.cvlan.vlan_start, argv[1], IFM_VLAN_MIN, IFM_VLAN_MAX );
    if ( NULL == argv[2] )
    {
        encap.cvlan.vlan_end = encap.cvlan.vlan_start;
    }
    else
    {
        VTY_GET_INTEGER_RANGE ( "end cvlan", encap.cvlan.vlan_end, argv[2], IFM_VLAN_MIN, IFM_VLAN_MAX );

        if ( encap.cvlan.vlan_start >= encap.cvlan.vlan_end )
        {
            ifm_cli_parse_erro ( vty, IFM_ERR_ENCAP_RANGE );
            return CMD_WARNING;
        }

    }

    encap.cvlan_act = VLAN_ACTION_NO;
    encap.svlan_act = VLAN_ACTION_DELETE;

    ret = ifm_set_info ( ifindex, ( void * ) &encap, IFNET_INFO_ENCAP, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN ( no_subif_encapsulate,
        no_subif_encapsulate_cmd,
        "no encapsulate",
        NO_STR
        "Subif encapsulate\n" )
{
    int ret = 0;
    uint32_t ifindex = 0;
    struct ifm_encap encap;

    memset ( &encap, 0, sizeof ( struct ifm_encap ) );
    ifindex = ( uint32_t ) vty->index;

    encap.type             = IFNET_ENCAP_INVALID;
    ret = ifm_set_info ( ifindex, ( void * ) &encap, IFNET_INFO_ENCAP, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN ( if_alias,
        if_alias_cmd,
        "alias STRING",
        "Set an interface alias\n"
        "The alias of Interface(no more than 256 characters)\n" )
{
    int ret = 0;
    uint32_t ifindex = 0;
    const char *alias = argv[0];

    ifindex = ( uint32_t ) vty->index;
	
    if ( strlen ( alias ) > IFM_ALIAS_STRING_LEN )
    {
	    ifm_cli_parse_erro ( vty, IFM_ERR_STRING_OUT_RANGE);
        return CMD_WARNING;
    }

    ret = ifm_set_info ( ifindex, ( void * ) alias, IFNET_INFO_ALIAS, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN ( no_if_alias,
        no_if_alias_cmd,
        "no alias",
        NO_STR
        "Set an interface alias\n" )
{
    int ret = 0;
    uint32_t ifindex = 0;
    uchar alias[IFM_ALIAS_STRING_LEN] = {'\0'};

    ifindex = ( uint32_t ) vty->index;

    ret = ifm_set_info ( ifindex, ( void * ) alias, IFNET_INFO_ALIAS, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN ( statistics_enable,
        statistics_enable_cmd,
        "statistics enable",
        "Interface statistics\n"
        "Enable statistics of the interface \n" )
{
    int ret = 0;
    uint32_t ifindex = 0;
    uint8_t statistics = IFNET_STAT_ENABLE;

    ifindex = ( uint32_t ) vty->index;

    ret = ifm_set_info ( ifindex, ( void * ) &statistics, IFNET_INFO_STATISTICS, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN ( no_statistics_enable,
        no_statistics_enable_cmd,
        "no statistics enable",
        NO_STR
        "Interface statistics\n"
        "Enable statistics of the interface \n" )
{
    int ret = 0;
    uint32_t ifindex = 0;
    uint8_t statistics = IFNET_STAT_DISABLE;

    ifindex = ( uint32_t ) vty->index;

    ret = ifm_set_info ( ifindex, ( void * ) &statistics, IFNET_INFO_STATISTICS, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }


    return CMD_SUCCESS;
}


DEFUN ( clear_if_statistics,
        clear_if_statistics_cmd,
        "statistics clear interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP|tdm USP|stm USP|vlanif <1-4094>| trunk TRUNK|loopback <0-128>|tunnel USP}",
        "Interface statistics\n"
        "Clear	statistics\n"
        CLI_INTERFACE_STR
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
        CLI_INTERFACE_TDM_STR
        CLI_INTERFACE_TDM_VHELP_STR
        CLI_INTERFACE_STM_STR
        CLI_INTERFACE_STM_VHELP_STR
        CLI_INTERFACE_VLANIF_STR
        CLI_INTERFACE_VLANIF_VHELP_STR
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR
        CLI_INTERFACE_LOOPBACK_STR
        CLI_INTERFACE_LOOPBACK_VHELP_STR
        CLI_INTERFACE_TUNNEL_STR
        CLI_INTERFACE_TUNNEL_VHELP_STR )

{
    int ret = 0;
    uint32_t ifindex = 0;
    uint8_t statistics = IFNET_STAT_CLEAR;

    if ( argv[0] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "ethernet", ( char * ) argv[0] );
    }
	else if ( argv[1] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "gigabitethernet", ( char * ) argv[1] );
    }
	else if ( argv[2] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "xgigabitethernet", ( char * ) argv[2] );
    }
    else if ( argv[3] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "tdm", ( char * ) argv[3] );
    }
    else if ( argv[4] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "stm", ( char * ) argv[4] );
    }
    else if ( argv[5] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "vlanif", ( char * ) argv[5] );
    }
    else if ( argv[6] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "trunk", ( char * ) argv[6] );
    }
    else if ( argv[7] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "loopback", ( char * ) argv[7] );
    }
    else if ( argv[8] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "tunnel", ( char * ) argv[8] );
    }
    else
    {
	    ifm_cli_parse_erro ( vty, IFM_ERR_PARAM_FORMAT );
        return CMD_WARNING;
    }
    if ( ifindex == 0 )
    {
        ifm_cli_parse_erro ( vty, IFM_ERR_PARAM_FORMAT );
        return CMD_WARNING;
    }

    ret = ifm_set_info ( ifindex, ( void * ) &statistics, IFNET_INFO_STATISTICS, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }


    return CMD_SUCCESS;

}

DEFUN ( clear_statistics,
        clear_statistics_cmd,
        "statistics clear",
        "Interface statistics\n"
        "Clear	statistics\n" )

{
    int ret = 0;
    uint32_t ifindex = 0;
    uint8_t statistics = IFNET_STAT_CLEAR;

    ifindex = ( uint32_t ) vty->index;

    if ( ifindex == 0 )
    {
        ifm_cli_parse_erro ( vty, IFM_ERR_PARAM_FORMAT );
        return CMD_WARNING;
    }
    ret = ifm_set_info ( ifindex, ( void * ) &statistics, IFNET_INFO_STATISTICS, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN ( reflector,
        reflector_cmd,
        "reflector (external|internal) ( exchange-mac ){exchange-ip}",
        "Interface set reflector\n"
        "Set external reflector\n"
        "Set internal reflector\n" 
        "Exchange mac\n" 
        "Exchange ip\n" 
        )

{
    int ret = 0;
    uint32_t ifindex = 0;
	struct ifm_reflector reflector;
	unsigned int devtype = 0;
	struct ifm_usp usp;
	
	memset(&reflector, 0, sizeof(struct ifm_reflector));
	memset( &usp, 0, sizeof(struct ifm_usp ));
	devm_comm_get_id(1, 0, MODULE_ID_IFM, &devtype);
	
    ifindex = ( uint32_t ) vty->index;
    if ( ifindex == 0 )
    {
		ifm_cli_parse_erro ( vty , IFM_ERR_PARAM_FORMAT );
        return CMD_WARNING;
    }
	ret = ifm_get_usp_by_ifindex( ifindex, &usp );
	if ( ret != 0 )
	{
		ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
	}
	if( devtype == ID_HT158 )
	{
		if( usp.port == 1 )
		{
			ifm_cli_parse_erro( vty,  IFM_ERR_REFLECTOR );
        	return CMD_WARNING;			
		}
	}
	
    if ( is_sla_enable == 1 )
    {
		ifm_cli_parse_erro( vty,  IFM_ERR_SLA_IS_SETTED );
        return CMD_WARNING;
    }
    ret = ifm_get_info ( ifindex, &reflector, IFNET_INFO_LOOPBACK );
    if ( ret != 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    if ( reflector.reflector_flag & IFM_LOOPBACK_EXTERNEL)
    {
        ifm_cli_parse_erro ( vty, IFM_ERR_LOOPBACK_EX_SET );
        return CMD_WARNING;

    }
	
    if ( reflector.reflector_flag & IFM_LOOPBACK_INTERNAL)
    {
        ifm_cli_parse_erro ( vty, IFM_ERR_LOOPBACK_IN_SET );
		
        return CMD_WARNING;
    }
    if ( strncmp( argv[0], "external", 8 ) == 0 )
	{
		reflector.reflector_flag |= IFM_LOOPBACK_EXTERNEL;
	}
	if ( strncmp ( argv[0], "internal", 8 ) == 0 )
	{
		reflector.reflector_flag |= IFM_LOOPBACK_INTERNAL;
	}	
	/*设置exchange mac*/
    if ( argv[1] != NULL )
    {
        reflector.exchange_mac = 1;
    }
	/*设置exchange ip*/
    if ( argv[2] != NULL )
    {
        reflector.exchange_ip = 1;
    }
	reflector.set_flag = 64;
	
    ret = ifm_set_info ( ifindex, ( void * ) &reflector, IFNET_INFO_LOOPBACK, IFM_OPCODE_UPDATE );
    if ( ret != 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN ( no_reflector,
        no_reflector_cmd,
        "no reflector (external|internal)",
        NO_STR
        "Interface set reflector\n"
        "Set external reflector\n"
        "Set internal reflector\n" )
{
    int ret = 0;
    uint32_t ifindex = 0;
    uint8_t set_flag = 0;  
    struct ifm_loopback loop_info ;
    struct ifm_entry*pifm = NULL;
	struct ifm_reflector reflector;

	memset(&reflector, 0, sizeof(struct ifm_reflector));
    ifindex = ( uint32_t ) vty->index;

    if ( ifindex == 0 )
    {
        ifm_cli_parse_erro ( vty, IFM_ERR_PARAM_FORMAT );
        return CMD_WARNING;
    }
    pifm = ifm_lookup(  ifindex );
    if ( !pifm )
   {
   	zlog_err( "%s[%d] pifm is null,ifindex = 0x%0x\n",__FUNCTION__,__LINE__ ,ifindex );
	return CMD_WARNING;
   }
   loop_info = pifm->ifm.loop_info;  
   set_flag = loop_info.set_flag;    //vlan mac ip   
	if(64 == pifm->ifm.reflector.set_flag )  
	{
   		ret = ifm_get_info ( ifindex, &reflector, IFNET_INFO_LOOPBACK );
   	 	if ( ret != 0 )
    	{
        		ifm_cli_parse_erro ( vty, ret );
        		return CMD_WARNING;
    	}
  
    	if ( 0 == reflector.reflector_flag )
   	 {
		    ifm_cli_parse_erro( vty,  IFM_ERR_LOOPBACK_NO_SET );
    		return CMD_WARNING;
   	 }

    	if ( strncmp (argv[0], "external",8 ) == 0 )
    	{
        	reflector.reflector_flag &= ~IFM_LOOPBACK_EXTERNEL;
    	}
    	else if ( strncmp (argv[0], "internal",8 ) == 0  )
    	{
        	reflector.reflector_flag &= ~IFM_LOOPBACK_INTERNAL;
    	}
    	
		
		reflector.exchange_ip = 0;
		reflector.exchange_mac = 0;
	 	ret = ifm_set_info ( ifindex, ( void * ) &reflector, IFNET_INFO_LOOPBACK, IFM_OPCODE_UPDATE );
          if ( ret != 0 )
         {
          	ifm_cli_parse_erro ( vty, ret );
        	return CMD_WARNING;
         }
   }
	else 
	{
		if ( set_flag == IFM_LB_SVLAN_SET  )
		{
			loop_info.set_flag = set_flag;
			loop_info.reflector.exchange_mac = 0;
			loop_info.reflector.exchange_ip = 0;
			loop_info.svlan = 0;

			if( strncmp (argv[0],"external", 8 ) == 0 )
			{
				(loop_info.reflector.reflector_flag )&= ~IFM_LOOPBACK_EXTERNEL;
			}
			else if( strncmp (argv[0],"internal",8 ) == 0 )
			{
				(loop_info.reflector.reflector_flag )&= ~IFM_LOOPBACK_INTERNAL;
			}

			ret = ifm_set_info( ifindex, &loop_info,  IFNET_INFO_LOOPBACK_FUNC,  IFM_OPCODE_UPDATE );
			if ( ret < 0 )
			{
				ifm_cli_parse_erro ( vty, ret );
				return CMD_WARNING;
			}
		}
		else if ( set_flag == IFM_LB_SMAC_SET)
		{
			loop_info.set_flag = set_flag;
			loop_info.reflector.exchange_mac = 0;
			loop_info.reflector.exchange_ip = 0;
			memset(loop_info.smac, 0x00, 6);

			if( strncmp (argv[0],"external", 8 ) == 0 )
			{
				(loop_info.reflector.reflector_flag )&= ~IFM_LOOPBACK_EXTERNEL;
			}
			else if( strncmp (argv[0],"internal",8 ) == 0 )
			{
				(loop_info.reflector.reflector_flag )&= ~IFM_LOOPBACK_INTERNAL;
			}

			ret = ifm_set_info( ifindex, &loop_info,  IFNET_INFO_LOOPBACK_FUNC,  IFM_OPCODE_UPDATE );
			if ( ret < 0 )
			{
				ifm_cli_parse_erro ( vty, ret );
				return CMD_WARNING;
			}  	
		}
		else if ( set_flag == IFM_LB_DMAC_SET)
		{
			loop_info.set_flag = set_flag;
			loop_info.reflector.exchange_mac = 0;
			loop_info.reflector.exchange_ip = 0;
			memset(loop_info.dmac, 0x00, 6);

			if( strncmp (argv[0],"external", 8 ) == 0 )
			{
				(loop_info.reflector.reflector_flag )&= ~IFM_LOOPBACK_EXTERNEL;
			}
			else if( strncmp (argv[0],"internal",8 ) == 0 )
			{
				(loop_info.reflector.reflector_flag )&= ~IFM_LOOPBACK_INTERNAL;
			}

			ret = ifm_set_info(ifindex, &loop_info,  IFNET_INFO_LOOPBACK_FUNC,  IFM_OPCODE_UPDATE );
			if ( ret < 0 )
			{
				ifm_cli_parse_erro ( vty, ret );
				return CMD_WARNING;
			}  	
		}
		else if ( set_flag == IFM_LB_SIP_SET )
		{
			loop_info.set_flag = set_flag;
			loop_info.reflector.exchange_mac = 0;
			loop_info.reflector.exchange_ip = 0;
			loop_info.sip.addr = 0;

			if( strncmp (argv[0],"external", 8 ) == 0 )
			{
				(loop_info.reflector.reflector_flag )&= ~IFM_LOOPBACK_EXTERNEL;
			}
			else if( strncmp (argv[0],"internal",8 ) == 0 )
			{
				(loop_info.reflector.reflector_flag )&= ~IFM_LOOPBACK_INTERNAL;
			}

			ret = ifm_set_info(ifindex, &loop_info,  IFNET_INFO_LOOPBACK_FUNC,  IFM_OPCODE_UPDATE );
			if ( ret < 0 )
			{
				ifm_cli_parse_erro ( vty, ret );
				return CMD_WARNING;
			}  	
		}
		else if ( set_flag == IFM_LB_DIP_SET )
		{
			loop_info.set_flag = set_flag;
			loop_info.reflector.exchange_mac = 0;
			loop_info.reflector.exchange_ip = 0;
			loop_info.dip.addr = 0;
			if( strncmp (argv[0],"external", 8 ) == 0 )
			{
				(loop_info.reflector.reflector_flag )&= ~IFM_LOOPBACK_EXTERNEL;
			}
			else if( strncmp (argv[0],"internal",8 ) == 0 )
			{
				(loop_info.reflector.reflector_flag )&= ~IFM_LOOPBACK_INTERNAL;
			}

			ret = ifm_set_info( ifindex, &loop_info,  IFNET_INFO_LOOPBACK_FUNC,  IFM_OPCODE_UPDATE );
			if ( ret < 0 )
			{
				ifm_cli_parse_erro ( vty, ret );
				return CMD_WARNING;
			}  	
		}   
  	}
   
   
    return CMD_SUCCESS;
}

DEFUN ( sla_interface_session_enable,
        sla_interface_session_enable_cmd,
        "sla enable session <1-65535> {to <1-65535>}",
        SLA_STR
        "Enable\n"
        "Session\n"
        "Session <1-65535>\n"
        "To\n"
        "Session <1-65535>\n"
      )
{
	uint32_t ifindex = (uint32_t)vty->index;
	struct ifm_entry *pifm = NULL;
	
	pifm = ifm_lookup(ifindex);
	if(NULL == pifm)
	{
		return CMD_WARNING;
	}
	pifm->ifm.sla_flag = 1;
	
    is_sla_enable = 1;
    return CMD_SUCCESS;
}

/* 若同时使能多条，必须同时去使能*/

DEFUN ( no_sla_interface_session_enable,
        no_sla_interface_session_enable_cmd,
        "no sla enable session <1-65535> {to <1-65535>}",
        "No command\n"
        SLA_STR
        "Enable\n"
        "Session\n"
        "Session <1-65535>\n"
        "To\n"
        "Session <1-65535>\n"
      )
{
	uint32_t ifindex = (uint32_t)vty->index;
	struct ifm_entry *pifm = NULL;
	
	pifm = ifm_lookup(ifindex);
	if(NULL == pifm)
	{
		return CMD_WARNING;
	}
	pifm->ifm.sla_flag = 0;
    is_sla_enable = 0;
    return CMD_SUCCESS;
}


DEFUN ( ces_mode,
        ces_mode_cmd,
        "mode (unframed | framed | multiframed)",
        "Interface frame mode\n"
        "Unframed mode\n"
        "Framed mode\n"
		"Multiframed mode\n")
{
    int ret = 0;
    uint32_t ifindex = 0;

    ifindex = ( uint32_t ) vty->index;

    if ( strncmp ( argv[0], "unframed", 2 ) == 0 )
    {
        /*delete sons*/
        ret = ifm_delete_son_all ( ifindex );
        if ( ret < 0 )
        {
            ifm_cli_parse_erro ( vty, ret );
            return CMD_WARNING;

        }

    }

    return CMD_SUCCESS;

}

DEFUN ( no_ces_mode,
        no_ces_mode_cmd,
        "no mode",
        NO_STR
        "Interface frame mode\n" )
{
    int ret = 0;
    uint32_t ifindex = 0;

    ifindex = ( uint32_t ) vty->index;

    /*delete sons*/
    ret = ifm_delete_son_all ( ifindex );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;

    }



    return CMD_SUCCESS;

}

DEFUN ( show_ifm,
        show_ifm_cmd,
        "show interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP|tdm USP|stm USP|vlanif <1-4094>| trunk TRUNK|loopback <0-128>|tunnel USP|clock <1-2>}",
        SHOW_STR
        CLI_INTERFACE_STR
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
        CLI_INTERFACE_TDM_STR
        CLI_INTERFACE_TDM_VHELP_STR
        CLI_INTERFACE_STM_STR
        CLI_INTERFACE_STM_VHELP_STR
        CLI_INTERFACE_VLANIF_STR
        CLI_INTERFACE_VLANIF_VHELP_STR
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR
        CLI_INTERFACE_LOOPBACK_STR
        CLI_INTERFACE_LOOPBACK_VHELP_STR
        CLI_INTERFACE_TUNNEL_STR
        CLI_INTERFACE_TUNNEL_VHELP_STR
        CLI_INTERFACE_CLOCK_STR
        CLI_INTERFACE_CLOCK_VHELP_STR )
{
    uint32_t ifindex = 0;
    struct ifm_entry *pifm = NULL;

    if ( argv[0] != NULL )
    {
        if ( strncmp ( argv[0], "eth", 3 ) == 0 )
        {
            ifm_type_brief_show ( vty, IFNET_TYPE_ETHERNET );
            return CMD_SUCCESS;

        }
		else if ( strncmp ( argv[0], "gigabitethernet", 3 ) == 0 )
        {
            ifm_type_brief_show ( vty, IFNET_TYPE_GIGABIT_ETHERNET );
            return CMD_SUCCESS;

        }
		else if ( strncmp ( argv[0], "xgigabitethernet", 3 ) == 0 )
        {
            ifm_type_brief_show ( vty, IFNET_TYPE_XGIGABIT_ETHERNET );
            return CMD_SUCCESS;

        }
        else if ( strncmp ( argv[0], "tdm", 3 ) == 0 )
        {
            ifm_type_brief_show ( vty, IFNET_TYPE_TDM );
            return CMD_SUCCESS;

        }
        else if ( strncmp ( argv[0], "stm", 3 ) == 0 )
        {
            ifm_type_brief_show ( vty, IFNET_TYPE_STM );
            return CMD_SUCCESS;

        }
        else if ( strncmp ( argv[0], "loopback", 4 ) == 0 )
        {
            ifm_type_brief_show ( vty, IFNET_TYPE_LOOPBACK );
            return CMD_SUCCESS;
        }
        else if ( strncmp ( argv[0], "trunk", 4 ) == 0 )
        {
            ifm_type_brief_show ( vty, IFNET_TYPE_TRUNK );
            return CMD_SUCCESS;
        }
        else if ( strncmp ( argv[0], "vlanif", 4 ) == 0 )
        {
            ifm_type_brief_show ( vty, IFNET_TYPE_VLANIF );
            return CMD_SUCCESS;
        }
        else if ( strncmp ( argv[0], "tunnel", 4 ) == 0 )
        {
            ifm_type_brief_show ( vty, IFNET_TYPE_TUNNEL );
            return CMD_SUCCESS;
        }
        else if ( strncmp ( argv[0], "clock", 4 ) == 0 )
        {
            ifm_type_brief_show ( vty, IFNET_TYPE_CLOCK );
            return CMD_SUCCESS;
        }

        ifindex = ifm_get_ifindex_by_name ( "ethernet", ( char * ) argv[0] );
    }
    else if ( argv[1] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "gigabitethernet", ( char * ) argv[1] );
    }
	else if ( argv[2] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "xgigabitethernet", ( char * ) argv[2] );
    }
    else if ( argv[3] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "tdm", ( char * ) argv[3] );
    }
    else if ( argv[4] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "stm", ( char * ) argv[4] );
    }
    else if ( argv[5] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "vlanif", ( char * ) argv[5] );
    }
    else if ( argv[6] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "trunk", ( char * ) argv[6] );
    }
    else if ( argv[7] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "loopback", ( char * ) argv[7] );
    }
    else if ( argv[8] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "tunnel", ( char * ) argv[8] );
    }
    else if ( argv[9] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "clock", ( char * ) argv[9] );
    }
    else
    {
        ifm_all_brief_show ( vty );

        return CMD_SUCCESS;
    }
    if ( ifindex == 0 )
    {
        ifm_cli_parse_erro ( vty, IFM_ERR_PARAM_FORMAT );
        return CMD_WARNING;
    }

    pifm = ifm_lookup ( ifindex );
    if ( !pifm )
    {
	    ifm_cli_parse_erro ( vty, IFM_ERR_NOT_FOUND );
        return CMD_WARNING;
    }

    ifm_brief_show ( vty, pifm );

    return CMD_SUCCESS;
}

ALIAS ( show_ifm,
        show_ifm_type_cmd,
        "show interface (ethernet|gigabitethernet|xgigabitethernet|tdm|stm|vlanif|trunk|loopback|tunnel|clock)",
        SHOW_STR
        CLI_INTERFACE_STR
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        CLI_INTERFACE_TDM_STR
        CLI_INTERFACE_STM_STR
        CLI_INTERFACE_VLANIF_STR
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_LOOPBACK_STR
        CLI_INTERFACE_TUNNEL_STR
        CLI_INTERFACE_CLOCK_STR )


DEFUN ( show_config_ifm,
        show_ifm_config_cmd,
        "show interface config {ethernet USP|gigabitethernet USP|xgigabitethernet USP|tdm USP|stm USP|vlanif <1-4094>| trunk TRUNK|loopback <0-128>|tunnel USP|clock <1-2>}",
        SHOW_STR
        CLI_INTERFACE_STR
        "Interface all config\n"
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
        CLI_INTERFACE_TDM_STR
        CLI_INTERFACE_TDM_VHELP_STR
        CLI_INTERFACE_STM_STR
        CLI_INTERFACE_STM_VHELP_STR
        CLI_INTERFACE_VLANIF_STR
        CLI_INTERFACE_VLANIF_VHELP_STR
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR
        CLI_INTERFACE_LOOPBACK_STR
        CLI_INTERFACE_LOOPBACK_VHELP_STR
        CLI_INTERFACE_TUNNEL_STR
        CLI_INTERFACE_TUNNEL_VHELP_STR
        CLI_INTERFACE_CLOCK_STR
        CLI_INTERFACE_CLOCK_VHELP_STR )
{
    uint32_t ifindex = 0;
    struct ifm_entry *pifm = NULL;

    if ( argv[0] != NULL )
    {
        if ( strncmp ( argv[0], "eth", 3 ) == 0 )
        {
            ifm_type_config_show ( vty, IFNET_TYPE_ETHERNET );
            return CMD_SUCCESS;
        }
		else if ( strncmp ( argv[0], "gigabiteth", 3 ) == 0 )
        {
            ifm_type_config_show ( vty, IFNET_TYPE_GIGABIT_ETHERNET );
            return CMD_SUCCESS;
        }
		else if ( strncmp ( argv[0], "xgigabiteth", 3 ) == 0 )
        {
            ifm_type_config_show ( vty, IFNET_TYPE_XGIGABIT_ETHERNET );
            return CMD_SUCCESS;
        }
        else if ( strncmp ( argv[0], "tdm", 3 ) == 0 )
        {
            ifm_type_config_show ( vty, IFNET_TYPE_TDM );
            return CMD_SUCCESS;
        }
        else if ( strncmp ( argv[0], "stm", 3 ) == 0 )
        {
            ifm_type_config_show ( vty, IFNET_TYPE_STM );
            return CMD_SUCCESS;
        }
        else if ( strncmp ( argv[0], "loopback", 4 ) == 0 )
        {
            ifm_type_config_show ( vty, IFNET_TYPE_LOOPBACK );
            return CMD_SUCCESS;
        }
        else if ( strncmp ( argv[0], "trunk", 4 ) == 0 )
        {
            ifm_type_config_show ( vty, IFNET_TYPE_TRUNK );
            return CMD_SUCCESS;
        }
        else if ( strncmp ( argv[0], "vlanif", 4 ) == 0 )
        {
            ifm_type_config_show ( vty, IFNET_TYPE_VLANIF );
            return CMD_SUCCESS;
        }
        else if ( strncmp ( argv[0], "tunnel", 4 ) == 0 )
        {
            ifm_type_config_show ( vty, IFNET_TYPE_TUNNEL );
            return CMD_SUCCESS;
        }
        else if ( strncmp ( argv[0], "clock", 4 ) == 0 )
        {
            ifm_type_config_show ( vty, IFNET_TYPE_CLOCK );
            return CMD_SUCCESS;
        }

        ifindex = ifm_get_ifindex_by_name ( "ethernet", ( char * ) argv[0] );
    }
    else if ( argv[1] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "gigabitethernet", ( char * ) argv[1] );
    }
	else if ( argv[2] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "xgigabitethernet", ( char * ) argv[2] );
    }
	else if ( argv[3] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "tdm", ( char * ) argv[3] );
    }
    else if ( argv[4] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "stm", ( char * ) argv[4] );
    }
    else if ( argv[5] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "vlanif", ( char * ) argv[5] );
    }
    else if ( argv[6] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "trunk", ( char * ) argv[6] );
    }
    else if ( argv[7] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "loopback", ( char * ) argv[7] );
    }
    else if ( argv[8] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "tunnel", ( char * ) argv[8] );
    }
    else if ( argv[9] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "clock", ( char * ) argv[9] );
    }
    else
    {
        ifm_all_config_show ( vty );

        return CMD_SUCCESS;
    }

    if ( ifindex == 0 )
    {
	    ifm_cli_parse_erro ( vty, IFM_ERR_PARAM_FORMAT );
        return CMD_WARNING;
    }
    pifm = ifm_lookup ( ifindex );
    if ( !pifm )
    {
        ifm_cli_parse_erro ( vty, IFM_ERR_NOT_FOUND );
        return CMD_WARNING;
    }

    ifm_interface_config_show ( vty, pifm );

    return CMD_SUCCESS;
}

DEFUN ( sfp_config_als_off_time,
        sfp_config_als_off_time_cmd,
        "sfp-config als off-time <1-65535>",
        "interface set sfp_off\n"
        "sfp auto\n"
        "time of close\n"
        "value range\n" )
{
    int ret = 0;
    uint32_t ifindex ;
    uint32_t sfp_off;
    VTY_GET_INTEGER_RANGE ( "sfp_off", sfp_off, argv[0], 1, 65535 );
    ifindex = (uint32_t)vty->index;
    if ( ifindex == 0 )
    {
	    ifm_cli_parse_erro ( vty , IFM_ERR_PARAM_FORMAT );
        return CMD_WARNING;
    }
    ret = ifm_set_info ( ifindex, ( void * ) &sfp_off, IFNET_INFO_SFP_OFF, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty , ret );
        return CMD_WARNING;
    }
    return CMD_SUCCESS;
}
DEFUN ( sfp_config_als_on_time,
        sfp_config_als_on_time_cmd,
        "sfp-config als on-time <1-65535>",
        "interface set sfp_on\n"
        "sfp manual\n"
        "open time\n"
        "value range\n" )
{
    int ret = 0 ;
    uint32_t ifindex = 0 ;
    uint32_t sfp_on = 0 ;
    VTY_GET_INTEGER_RANGE ( "sfp_on", sfp_on, argv[0], 1, 65535 );
    ifindex = (uint32_t)vty->index;
    if ( ifindex == 0 )
    {
        ifm_cli_parse_erro ( vty , IFM_ERR_PARAM_FORMAT );
        return CMD_WARNING;
    }
    ret = ifm_set_info ( ifindex, ( void * ) &sfp_on, IFNET_INFO_SFP_ON, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty , ret );
        return CMD_WARNING;
    }
    return CMD_SUCCESS;

}
DEFUN ( sfp_config_als,
        sfp_config_als_cmd,
        "sfp-config als (enable|disable)",
        "interface set sfp_als\n"
        "sfp auto\n"
        "sfp enable\n"
        "sfp disable\n" )
{
    int ret = 0 ;
    uint8_t sfp_als = 0;
    uint32_t ifindex = 0 ;
    ifindex = (uint32_t)vty -> index;
    if ( ifindex == 0 )
    {
       ifm_cli_parse_erro ( vty , IFM_ERR_PARAM_FORMAT );
        return CMD_WARNING;
    }
    if ( strncmp ( argv[0], "enable", 6 ) == 0 )
    {
        sfp_als = IFM_SFP_ALS_ENABLE;
    }
    else
    {
        sfp_als = IFM_SFP_ALS_DISABLE;
    }
    ret = ifm_set_info ( ifindex, ( void * ) &sfp_als, IFNET_INFO_SFP_ALS, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }
    return IFM_SUCCESS;

}

DEFUN ( sfp_config_tx,
        sfp_config_tx_cmd,
        "sfp-config tx (enable|disable)",
        "interface set sfp\n"
        "sfp manual\n"
        "sfp tx enable\n"
        "sfp tx disable\n" )
{
    int ret = 0 ;
    uint32_t ifindex = 0 ;
    uint8_t sfp_tx = 0 ;
    if ( strncmp ( argv[0], "enable", 6 ) == 0 )
    {
        sfp_tx = IFM_SFP_TX_ENABLE;
    }
    else
    {
        sfp_tx = IFM_SFP_TX_DISABLE;
    }
    ifindex = (uint32_t)vty->index ;
    if ( ifindex == 0 )
    {
        ifm_cli_parse_erro ( vty , IFM_ERR_PARAM_FORMAT );
        return CMD_WARNING;
    }
    ret = ifm_set_info ( ifindex, ( void * ) & sfp_tx, IFNET_INFO_SFP_TX, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }
    return IFM_SUCCESS;
}

ALIAS ( show_config_ifm,
        show_ifm_type_config_cmd,
        "show interface config (ethernet|gigabitethernet|xgigabitethernet|tdm|stm|vlanif|trunk|loopback|tunnel|clock)",
        SHOW_STR
        CLI_INTERFACE_STR
        "Interface all config\n"
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        CLI_INTERFACE_TDM_STR
        CLI_INTERFACE_STM_STR
        CLI_INTERFACE_VLANIF_STR
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_LOOPBACK_STR
        CLI_INTERFACE_TUNNEL_STR
        CLI_INTERFACE_CLOCK_STR )

DEFUN ( debug_show_msg,
        debug_show_msg_cmd,
        "show ifm msg",
        "Show Interface Mangemet Msg\n"
        "Interface Mangemet\n"
        "Interface Mangemet MSG\n" )
{
    vty_out ( vty, "The Msg Rec  %-5d %s", ifm_rmsg_list.count, VTY_NEWLINE );
    vty_out ( vty, "The Msg Send to Common %-5d %s", ifm_smsg_comm_list.count, VTY_NEWLINE );
    vty_out ( vty, "The Msg Send to FTM    %-5d%s", ifm_smsg_ftm_list.count, VTY_NEWLINE );
    vty_out ( vty, "The Msg Send to HAL    %-5d%s", ifm_smsg_hal_list.count, VTY_NEWLINE );
	
    return CMD_SUCCESS;

}

DEFUN ( debug_show_interface_name,
        debug_show_interface_name_cmd,
        "show ifm interface namebyifindex HEX",
        "Show Interface Mangemet\n"
        "Interface Mangemet\n"
        "Show Interface NAME by ifindex\n"
        "Interface ifindex \n" )
{
    int ret = 0;
    int ifindex = 0;
    char ifname[IFNET_NAMESIZE] = "";

    ifindex = cmd_hexstr2int ( ( char * ) argv[0], &ret );

    ifm_get_name_by_ifindex ( ifindex, ifname );

    vty_out ( vty, "interface %s%s", ifname, VTY_NEWLINE );

    return CMD_SUCCESS;

}
DEFUN ( debug_down_flapping_control,
        debug_down_flapping_control_cmd,
        "debug down-flapping period <0-255>",
        DEBUG_STR
        "Interface flapping-control\n"
        "Period of Interface flapping-control\n"
        "Value of Interface flapping-control (seconds)\n" )
{
    int ret = 0;
    uint32_t ifindex = 0;
    uint8_t flap_period = 0;

    VTY_GET_INTEGER_RANGE ( "period", flap_period, argv[0], IFM_FLAP_PERIOD_MIN, IFM_FLAP_PERIOD_MAX );
    ifindex = ( uint32_t ) vty->index;
    ret = ifm_set_info ( ifindex, ( void * ) &flap_period, IFNET_INFO_DOWN_FLAP, IFM_OPCODE_UPDATE );
    if ( ret < 0 )
    {
        ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN ( physical_gigabit_ethernet_if,
		physical_gigabit_ethernet_if_cmd,
		"interface gigabitethernet USP",
		CLI_INTERFACE_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR )

{
	int ret = 0;
	uint32_t ifindex = 0;
	char *pprompt = NULL;
	struct ifm_entry *pifm = NULL;
	

	ifindex = ifm_get_ifindex_by_name ( "gigabitethernet", argv[0] );
    if ( ifindex == 0 )
    {
        ifm_cli_parse_erro ( vty , IFM_ERR_PARAM_FORMAT );
        return CMD_WARNING;
    }

	if ( IFM_IS_SUBPORT ( ifindex ) )
	{
		ret = ifm_create_sub_ethernet ( ifindex );
		if ( ret < 0 )
		{
			ifm_cli_parse_erro ( vty, ret );
			return CMD_WARNING;
		}
	}
	else
	{
		pifm = ifm_lookup ( ifindex );
		if ( !pifm )
		{
			ifm_cli_parse_erro ( vty , IFM_ERR_NOT_FOUND );
			return CMD_WARNING;
		}
	}
	/*change node*/
	if ( IFM_IS_SUBPORT ( ifindex ) )
	{
		vty->node  = PHYSICAL_SUBIF_NODE;
	    pprompt = vty->change_prompt;
	    if ( pprompt )
	    {
	        /* format the prompt */
	        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-gigabitethernet%d/%d/%d.%d)#",
	                   IFM_UNIT_ID_GET ( ifindex ),
	                   IFM_SLOT_ID_GET ( ifindex ),
	                   IFM_PORT_ID_GET ( ifindex ),
	                   IFM_SUBPORT_ID_GET ( ifindex ) );

	    }
	}
	else
	{
		vty->node  = PHYSICAL_IF_NODE;
		pprompt = vty->change_prompt;
		if ( pprompt )
		{
			/* format the prompt */
			snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-gigabitethernet%d/%d/%d)#",
					   IFM_UNIT_ID_GET ( ifindex ),
					   IFM_SLOT_ID_GET ( ifindex ),
					   IFM_PORT_ID_GET ( ifindex ) );
		}
	}
	vty->index  = ( void * )ifindex;

	return CMD_SUCCESS;
}

DEFUN ( physical_xgigabit_ethernet_if,
		physical_xgigabit_ethernet_if_cmd,
		"interface xgigabitethernet USP",
		CLI_INTERFACE_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR )
{
	int ret = 0;
	int ifindex = 0;
	char *pprompt = NULL;
	struct ifm_entry *pifm = NULL;
	

	ifindex = ifm_get_ifindex_by_name ( "xgigabitethernet", argv[0] );
	if ( ifindex == 0 )
    {
        ifm_cli_parse_erro ( vty , IFM_ERR_PARAM_FORMAT );
        return CMD_WARNING;
    }

	if ( IFM_IS_SUBPORT ( ifindex ) )
	{
		ret = ifm_create_sub_ethernet ( ifindex );
		if ( ret < 0 )
		{
			ifm_cli_parse_erro ( vty, ret );
			return CMD_WARNING;
		}
	}
	else
	{
		pifm = ifm_lookup ( ifindex );
		if ( !pifm )
		{
			ifm_cli_parse_erro ( vty , IFM_ERR_NOT_FOUND );
			return CMD_WARNING;
		}
	}

	/*change node*/
	if ( IFM_IS_SUBPORT ( ifindex ) )
	{
		vty->node  = PHYSICAL_SUBIF_NODE;
	    pprompt = vty->change_prompt;
	    if ( pprompt )
	    {
	        /* format the prompt */
	        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-xgigabitethernet%d/%d/%d.%d)#",
	                   IFM_UNIT_ID_GET ( ifindex ),
	                   IFM_SLOT_ID_GET ( ifindex ),
	                   IFM_PORT_ID_GET ( ifindex ),
	                   IFM_SUBPORT_ID_GET ( ifindex ) );

	    }
	}
	else
	{
		vty->node  = PHYSICAL_IF_NODE;
		pprompt = vty->change_prompt;
		if ( pprompt )
		{
			/* format the prompt */
			snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-xgigabitethernet%d/%d/%d)#",
					   IFM_UNIT_ID_GET ( ifindex ),
					   IFM_SLOT_ID_GET ( ifindex ),
					   IFM_PORT_ID_GET ( ifindex ) );
		}
	}
	vty->index  = (void*)ifindex;

	return CMD_SUCCESS;
}

DEFUN(reflector_vlan,
	reflector_vlan_cmd,
	"reflector vlan <1-4094> (external | internal) ( exchange-mac ){exchange-ip}",
	"Loopback\n"
	"Based on vlan\n"
	"Vlan range\n"
	"Flag of external\n"
	"Flag of internal\n"
	"Exchange mac\n"
	"Exchange ip\n")
{
	int ret = 0;
	uint32_t  ifindex = 0;
	struct ifm_loopback loop_info;
	struct ifm_entry * pifm = NULL;
	unsigned int devtype = 0;
	struct ifm_usp usp;
	
	memset(&usp,0,sizeof(struct ifm_usp));
	memset(&loop_info, 0, sizeof(struct ifm_loopback));
	devm_comm_get_id(1, 0, MODULE_ID_IFM, &devtype);
	
	ifindex = ( uint32_t )vty->index;
	if ( ifindex == 0)
	{
		ifm_cli_parse_erro ( vty , IFM_ERR_PARAM_FORMAT );
		return CMD_WARNING;	
	}
	ret = ifm_get_usp_by_ifindex(ifindex, &usp);
	if ( ret != 0 )
	{
		ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
	}
	if( devtype == ID_HT158 )
	{
		if( usp.port == 1 )
		{
			ifm_cli_parse_erro( vty,  IFM_ERR_REFLECTOR );
        	return CMD_WARNING;			
		}
	}
	pifm = ifm_lookup(  ifindex );
    if ( !pifm )
   	{
   		zlog_err( "%s[%d] pifm is null,ifindex = 0x%0x\n",__FUNCTION__,__LINE__ ,ifindex );
		return CMD_WARNING;
   	}

	loop_info = pifm->ifm.loop_info;
	
	if ( is_sla_enable == 1 )
    {
		ifm_cli_parse_erro( vty,  IFM_ERR_SLA_IS_SETTED );
        return CMD_WARNING;
    }
	if ( loop_info.reflector.reflector_flag & IFM_LOOPBACK_EXTERNEL )
	{
		ifm_cli_parse_erro( vty, IFM_ERR_LOOPBACK_EX_SET);
		return CMD_WARNING;
	}
	if ( loop_info.reflector.reflector_flag & IFM_LOOPBACK_INTERNAL )
	{
		ifm_cli_parse_erro( vty, IFM_ERR_LOOPBACK_IN_SET);
		return CMD_WARNING;
	}
	
	VTY_GET_INTEGER_RANGE ( "vlan", loop_info.svlan, argv[0], IFM_VLAN_MIN, IFM_VLAN_MAX );
	
	
	if ( strncmp( argv[1], "external", 8 ) == 0 )
	{
		loop_info.reflector.reflector_flag = IFM_LOOPBACK_EXTERNEL;	
	}
	else if ( strncmp( argv[1], "internal", 8 ) == 0 )
	{
		loop_info.reflector.reflector_flag = IFM_LOOPBACK_INTERNAL;
	}
	if ( argv[2] !=	NULL )
	{
		loop_info.reflector.exchange_mac = 1;
	}
	if ( argv[3] != NULL )
	{
		loop_info.reflector.exchange_ip = 1;
	}

	loop_info.set_flag = IFM_LB_SVLAN_SET;	
	ret = ifm_set_info( ifindex,  ( void* )&loop_info,  IFNET_INFO_LOOPBACK_FUNC, IFM_OPCODE_UPDATE );
	if ( ret < 0 )
	{
		ifm_cli_parse_erro( vty,  ret );
		return CMD_WARNING;
	}
	return CMD_SUCCESS;
}
DEFUN(reflector_mac,
	reflector_mac_cmd,
	"reflector (smac | dmac) XX:XX:XX:XX:XX:XX (external | internal) ( exchange-mac ){exchange-ip}",
	"Loopback\n"
	"Based on smac\n"
	"Based on dmac\n"
	"Mac value\n"
	"Flag of external\n"
	"Flag of internal\n"
	"Exchange mac\n"
	"Exchnage ip\n" )
{
	int ret = 0;
	uint32_t ifindex =  0;
	struct ifm_loopback loop_info;
	struct ifm_entry*pifm = NULL;
	unsigned int devtype = 0;
	struct ifm_usp usp;
	
	memset(&usp,0,sizeof(struct ifm_usp));
	memset(&loop_info, 0, sizeof(struct ifm_loopback));
	devm_comm_get_id(1, 0, MODULE_ID_IFM, &devtype);	
	ifindex = ( uint32_t ) vty->index;
	if ( ifindex==0 )
	{
		ifm_cli_parse_erro ( vty , IFM_ERR_PARAM_FORMAT );
        return CMD_WARNING;
	}
	ret = ifm_get_usp_by_ifindex(ifindex, &usp);
	if ( ret != 0 )
	{
		ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
	}
	if( devtype == ID_HT158 )
	{
		if( usp.port == 1 )
		{
			ifm_cli_parse_erro( vty,  IFM_ERR_REFLECTOR );
        	return CMD_WARNING;			
		}
	}
	pifm = ifm_lookup( ifindex );
	if ( !pifm )
	{
		zlog_err( "%s[%d] pifm is null,ifindex = 0x%0x\n",__FUNCTION__,__LINE__ ,ifindex );
		return CMD_WARNING;
	}
	
	loop_info = pifm->ifm.loop_info;
	if ( is_sla_enable == 1 )
    {
		ifm_cli_parse_erro( vty,  IFM_ERR_SLA_IS_SETTED );
        return CMD_WARNING;
    }
	if ( loop_info.reflector.reflector_flag & IFM_LOOPBACK_EXTERNEL )
	{
		ifm_cli_parse_erro( vty, IFM_ERR_LOOPBACK_EX_SET);
		return CMD_WARNING;
	}
	if ( loop_info.reflector.reflector_flag & IFM_LOOPBACK_INTERNAL )
	{
		ifm_cli_parse_erro( vty, IFM_ERR_LOOPBACK_IN_SET);
		return CMD_WARNING;
	}
	
	if ( strncmp( argv[ 0 ], "smac", 4) == 0 )
	{
		ret = ether_valid_mac(( char*)argv[1] );
		if ( ret == 1 )
		{
			ifm_cli_parse_erro( vty,  IFM_ERR_PARAM_MAC_INVALID );
			return CMD_WARNING;
		}
		ether_string_to_mac(( char*)argv[1], loop_info.smac );
		
		if ( !ether_is_zero_mac( loop_info.smac ) )
		{
			ifm_cli_parse_erro( vty,  IFM_ERR_PARAM_MAC_ZERO);
			return CMD_WARNING;
			
		}
		loop_info.set_flag = IFM_LB_SMAC_SET;
	}
	else if ( strncmp( argv[ 0 ], "dmac", 4) == 0 )
	{
		ret = ether_valid_mac(( char*)argv[1] );
		if ( ret == 1 )
		{
			ifm_cli_parse_erro( vty,  IFM_ERR_PARAM_MAC_INVALID );
			return CMD_WARNING;
		}
		ether_string_to_mac(( char*)argv[1], loop_info.dmac );

		if ( !ether_is_zero_mac( loop_info.dmac ) )
		{
			ifm_cli_parse_erro( vty,  IFM_ERR_PARAM_MAC_ZERO);
			return CMD_WARNING;
		}
		loop_info.set_flag = IFM_LB_DMAC_SET;		
	}
	if ( strncmp( argv[2],"external", 8 ) == 0 )
	{
		loop_info.reflector.reflector_flag = IFM_LOOPBACK_EXTERNEL;
	}
	else if ( strncmp( argv[2],"internal", 8 ) == 0 )
	{
		loop_info.reflector.reflector_flag = IFM_LOOPBACK_INTERNAL;
	}
	if ( argv[3] != NULL )
	{
		loop_info.reflector.exchange_mac = 1;
	}
	if ( argv[4] != NULL )
	{
		loop_info.reflector.exchange_ip = 1;
	}	
	ret = ifm_set_info(ifindex, ( void * ) &loop_info, IFNET_INFO_LOOPBACK_FUNC, IFM_OPCODE_UPDATE );
	if ( ret < 0 )
	{
		ifm_cli_parse_erro( vty,  ret );
		return CMD_WARNING;
	}
	return CMD_SUCCESS;
}
DEFUN(reflector_ip,
	reflector_ip_cmd,
	"reflector (sip | dip) A.B.C.D (external | internal) ( exchange-mac ){exchange-ip}",
	"Loopback\n"
	"Based on sip\n"
	"Based on dip\n"
	"Ip value\n"
	"Flag of external\n"
	"Flag of internal\n"
	"Exchange mac\n"
	"Exchange ip\n" )
{
	int ret = 0;
	uint32_t ifindex = 0;	
	struct ifm_loopback loop_info;
	struct ifm_entry * pifm = NULL;
	unsigned int devtype = 0;
	struct ifm_usp usp;
	
	memset(&usp,0,sizeof(struct ifm_usp));
	memset(&loop_info, 0, sizeof(struct ifm_loopback));
	devm_comm_get_id(1, 0, MODULE_ID_IFM, &devtype);

	ifindex = (uint32_t)vty->index;
	if ( ifindex == 0 )
	{
		ifm_cli_parse_erro ( vty , IFM_ERR_PARAM_FORMAT );
		return CMD_WARNING;
	}
	ret = ifm_get_usp_by_ifindex(ifindex, &usp);
	if ( ret != 0 )
	{
		ifm_cli_parse_erro ( vty, ret );
        return CMD_WARNING;
	}
	if( devtype == ID_HT158 )
	{
		if( usp.port == 1 )
		{
			ifm_cli_parse_erro( vty,  IFM_ERR_REFLECTOR );
        	return CMD_WARNING;			
		}
	}
	pifm = ifm_lookup(  ifindex );
    if ( !pifm )
   	{
   		zlog_err( "%s[%d] pifm is null,ifindex = 0x%0x\n",__FUNCTION__,__LINE__ ,ifindex );
		return CMD_WARNING;
   	}
	loop_info = pifm->ifm.loop_info;
	if ( is_sla_enable == 1 )
    {
		ifm_cli_parse_erro( vty,  IFM_ERR_SLA_IS_SETTED );
        return CMD_WARNING;
    }
	if ( loop_info.reflector.reflector_flag & IFM_LOOPBACK_EXTERNEL )
	{
		ifm_cli_parse_erro( vty, IFM_ERR_LOOPBACK_EX_SET);
		return CMD_WARNING;
	}
	if ( loop_info.reflector.reflector_flag & IFM_LOOPBACK_INTERNAL )
	{
		ifm_cli_parse_erro( vty, IFM_ERR_LOOPBACK_IN_SET);
		return CMD_WARNING;
	}
	if ( strncmp( argv[ 0 ], "sip", 3 ) == 0 )
	{
		loop_info.sip.addr = inet_strtoipv4( ( char* )argv[ 1 ] );
		ret = inet_valid_ipv4( loop_info.sip.addr );
		if ( ret != 1)
		{
			ifm_cli_parse_erro( vty,  IFM_ERR_PARAM_IP_INVALID );
			return CMD_WARNING;
		}
		loop_info.set_flag = IFM_LB_SIP_SET;	
	}
	else if ( strncmp( argv[0],"dip", 3 ) ==0 )
	{
		loop_info.dip.addr = inet_strtoipv4( ( char* )argv[ 1 ] );
		ret = inet_valid_ipv4( loop_info.dip.addr );
		if ( ret != 1)
		{
			ifm_cli_parse_erro( vty,  IFM_ERR_PARAM_IP_INVALID );
			return CMD_WARNING;
		}
		loop_info.set_flag = IFM_LB_DIP_SET;	
	}
	if ( strncmp( argv[2],"external",8 ) == 0 )
	{
		loop_info.reflector.reflector_flag = IFM_LOOPBACK_EXTERNEL;
	}
	else if ( strncmp ( argv[2],"internal",8 ) == 0 )
	{
		loop_info.reflector.reflector_flag = IFM_LOOPBACK_INTERNAL;	
	}
	if ( argv[3] != NULL )
	{
		loop_info.reflector.exchange_mac = 1;
	}
	if ( argv[4] != NULL )
	{
		loop_info.reflector.exchange_ip = 1;
	}
	
	ret = ifm_set_info( ifindex, ( void * ) &loop_info, IFNET_INFO_LOOPBACK_FUNC,  IFM_OPCODE_UPDATE );
	if ( ret < 0 )
	{
		ifm_cli_parse_erro( vty,  ret );
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}

DEFUN(sfp_config_als_time,
		sfp_config_als_time_cmd,
		"no sfp-config als ( on-time | off-time )",
		"No time set\n"
		"Sfp-config\n"
		"Sfp manual\n"
		"On-time\n"
		"Off-time\n" )
{
	uint32_t ifindex = 0;
	int ret = 0;
	uint32_t sfp_time = 0;

	ifindex = ( uint32_t )vty->index;

	if ( ifindex == 0 )
	{
		ifm_cli_parse_erro(vty, ret);
		return CMD_WARNING;
		//vty_error_out ( vty, "%s %s",errcode_get_string(IFM_ERR_PARAM_FORMAT), VTY_NEWLINE );
		//return IFM_ERR_PARAM_FORMAT;
	}

	if ( strncmp(argv[0], "on-time", 7 ) == 0 )
	{
		sfp_time = 2;
		ret = ifm_set_info( ifindex, (void *)&sfp_time, IFNET_INFO_SFP_ON, IFM_OPCODE_UPDATE );
		if ( ret != 0 )
		{
			ifm_cli_parse_erro(vty, ret);
			return CMD_WARNING;
			//vty_error_out ( vty, "%s %s",errcode_get_string(ret), VTY_NEWLINE );
			//return ret;
		}
	}
	else if ( strncmp( argv[0], "off-time",7 ) == 0 )
	{
		sfp_time = 6;
		ret = ifm_set_info( ifindex, (void*)&sfp_time, IFNET_INFO_SFP_OFF,IFM_OPCODE_UPDATE );
		if ( ret != 0 )
		{
			ifm_cli_parse_erro(vty, ret);
			//vty_error_out ( vty, "%s %s",errcode_get_string(ret), VTY_NEWLINE );
			//return ret;
			return CMD_WARNING;
		}
	}
	return CMD_SUCCESS;
}


DEFUN(flow_control,
    flow_control_cmd,
    "flow-control",
    "Enable flow-control both send and receive\n")
{
    int ret = 0;
    uint32_t ifindex = 0;
    struct ifm_entry *pifm = NULL;
    struct ifm_flow_control fc;

    memset(&fc, 0, sizeof(struct ifm_flow_control));

    ifindex = ( uint32_t ) vty->index;

    pifm = ifm_lookup2 ( ifindex );
    if ( !pifm )
    {
        vty_error_out ( vty, "%s %s",errcode_get_string(ERRNO_NOT_FOUND), VTY_NEWLINE );
        return CMD_WARNING;
    }
    
	if(pifm->ifm.mode != IFNET_MODE_SWITCH)
	{
		vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

    fc.status = IFM_FLOW_CONTROL_ENABLE;
    fc.direction = IFM_FLOW_CONTROL_RECEIVE_AND_SEND;

    ret = ifm_set_info(ifindex, (void *) &fc, IFNET_INFO_FLOW_CONTROL, IFM_OPCODE_UPDATE);
    if ( ret != 0 )
    {
        vty_error_out ( vty, "%s %s",errcode_get_string(ret), VTY_NEWLINE );
        return CMD_WARNING;
    }
    return CMD_SUCCESS;
}

DEFUN(undo_flow_control,
    undo_flow_control_cmd,
    "undo flow-control",
    NO_STR
    "Disable flow-control both send and receive\n")
{
    int ret = 0;
    uint32_t ifindex = 0;
    struct ifm_entry *pifm = NULL;
    struct ifm_flow_control fc;

    memset(&fc, 0, sizeof(struct ifm_flow_control));

    ifindex = (uint32_t)vty->index;
    
    pifm = ifm_lookup2 ( ifindex );
    if ( !pifm )
    {
        vty_error_out ( vty, "%s %s",errcode_get_string(ERRNO_NOT_FOUND), VTY_NEWLINE );
        return CMD_WARNING;
    }
    
    if(pifm->ifm.mode != IFNET_MODE_SWITCH)
    {
        vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    fc.status = IFM_FLOW_CONTROL_DISABLE;
    fc.direction = IFM_FLOW_CONTROL_RECEIVE | IFM_FLOW_CONTROL_SEND;

    ret = ifm_set_info(ifindex, (void *)&fc, IFNET_INFO_FLOW_CONTROL, IFM_OPCODE_UPDATE);
    if ( ret != 0 )
    {
        vty_error_out ( vty, "%s %s",errcode_get_string(ret), VTY_NEWLINE );
        return CMD_WARNING;
    }
    return CMD_SUCCESS;
}


DEFUN(flow_control_receive,
    flow_control_receive_cmd,
    "flow-control receive enable",
    "IEEE 802.3x Flow Control\n"
    "Direction receive\n"
    "Enable flow control receive\n")
{
    int ret = 0;
    uint32_t ifindex = 0;
    struct ifm_entry *pifm = NULL;
    struct ifm_flow_control fc;

    memset(&fc, 0, sizeof(struct ifm_flow_control));

    ifindex = (uint32_t)vty->index;
    
    pifm = ifm_lookup2 ( ifindex );
    if ( !pifm )
    {
        vty_error_out ( vty, "%s %s",errcode_get_string(ERRNO_NOT_FOUND), VTY_NEWLINE );
        return CMD_WARNING;
    }
    
    if(pifm->ifm.mode != IFNET_MODE_SWITCH)
    {
        vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
        return CMD_WARNING;
    }


    fc.status = IFM_FLOW_CONTROL_ENABLE;
    fc.direction = IFM_FLOW_CONTROL_RECEIVE;

    ret = ifm_set_info (ifindex, (void *)&fc, IFNET_INFO_FLOW_CONTROL, IFM_OPCODE_UPDATE);
    if ( ret != 0 )
    {
        vty_error_out ( vty, "%s %s",errcode_get_string(ret), VTY_NEWLINE );
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(flow_control_send,
    flow_control_send_cmd,
    "flow-control send enable",
    "IEEE 802.3x Flow Control\n"
    "Direction send\n"
    "Enable flow control send\n")
{
    int ret = 0;
    uint32_t ifindex = 0;
    struct ifm_entry *pifm = NULL;
    struct ifm_flow_control fc;

    memset(&fc, 0, sizeof(struct ifm_flow_control));

    ifindex = ( uint32_t ) vty->index;
    
    pifm = ifm_lookup2 ( ifindex );
    if ( !pifm )
    {
        vty_error_out ( vty, "%s %s",errcode_get_string(ERRNO_NOT_FOUND), VTY_NEWLINE );
        return CMD_WARNING;
    }
    
    if(pifm->ifm.mode != IFNET_MODE_SWITCH)
    {
        vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
        return CMD_WARNING;
    }


    fc.status = IFM_FLOW_CONTROL_ENABLE;
    fc.direction = IFM_FLOW_CONTROL_SEND;

    ret = ifm_set_info(ifindex, (void *)&fc, IFNET_INFO_FLOW_CONTROL, IFM_OPCODE_UPDATE);
    if ( ret != 0 )
    {
        vty_error_out ( vty, "%s %s",errcode_get_string(ret), VTY_NEWLINE );
        return CMD_WARNING;
    }
    return CMD_SUCCESS;
}

    
DEFUN(show_flow_control,
    show_flow_control_cmd,
    "show flow-control { interface (ethernet | gigabitethernet | xgigabitethernet) USP }",
    "Show Command\n"
    "Show flow control config\n"
    "Interface\n"
    CLI_INTERFACE_ETHERNET_STR
    CLI_INTERFACE_GIGABIT_ETHERNET_STR
    CLI_INTERFACE_XGIGABIT_ETHERNET_STR
    "The port of the interface, format: <0-7>/<0-31>/<1-255>\n")
{
    uint32_t ifindex = 0;

    FLOW_CONTROL_SHOW_HEAD(vty);

    if(argv[0] != NULL)
    {
        ifindex = ifm_get_ifindex_by_name ( argv[0], (char *)argv[1] );        
        flow_control_print(vty, ifindex);
    }
    else
    {
        ifm_flow_control_show(vty);
    }

    FLOW_CONTROL_SHOW_END(vty);
    
    return CMD_SUCCESS;
}
	
DEFUN(interface_sample_interval,
	interface_sample_interval_cmd,
	"sample interval <5-20>",	
	"Receive good packets\n"
	"Sample interval\n"
	"Value of interval\n" )
{
	int ret = 0;
	uint32_t ifindex = 0;
	int interval = 0;

	ifindex = (uint32_t)vty->index;
	
	VTY_GET_INTEGER_RANGE ( "interval", interval, argv[0], 5, 20);

	if( ifindex == 0 )
	{
		ifm_cli_parse_erro ( vty , IFM_ERR_PARAM_FORMAT );
        return CMD_WARNING;
	}
	ret = ifm_set_info(ifindex, (void *)&interval, IFNET_INFO_INTERVAL, IFM_OPCODE_UPDATE);
	if( ret )
	{
		ifm_cli_parse_erro ( vty , ret );
        return CMD_WARNING;
	}
	
	return CMD_SUCCESS;
}
DEFUN(ifm_show_timerinfo_fun,
	ifm_show_timerinfo_cmd,
	"show ifm timer",
	"Show Command\n"
    "Module id\n"
    "Timer\n")
{
	int iInLn = 1000;
	char bInfo[1000] = {0};

	iInLn = high_pre_timer_info(bInfo, iInLn);
	
	if(iInLn > 0)
	{
		vty_out(vty, "%s%s", bInfo, VTY_NEWLINE);
	}
         
    else 
	{
		vty_error_out(vty, "Show file module timer infor error. %s", VTY_NEWLINE);
	}

    return CMD_SUCCESS;
}
DEFUN( ifm_debug_fun,
	ifm_debug_fun_cmd,
	"debug ifm (enable |disable) (common|all)",
	"Debug information to monitor\n"
	"Programe name\n"
	"IFM debug enable\n"
	"IFM debug disable\n"
	"IFM debug type common\n"
	"IFM debug type all\n"	)
{
	int zlog_num = 0;
	for( zlog_num = 0; zlog_num < array_size(ifm_dbg_name);zlog_num++ )
	{
		if( !strncmp(argv[1],ifm_dbg_name[zlog_num].str,3))
		{
			zlog_debug_set(vty, ifm_dbg_name[zlog_num].key, !strncmp( argv[0], "enable", 3));
			return CMD_SUCCESS;
		}		
	}
	vty_out(vty,"No debug type find %s", VTY_NEWLINE);
	return CMD_SUCCESS;
}
DEFUN(show_ifm_debug_fun,
	show_ifm_debug_cmd,
	"show ifm debug",
	SHOW_STR
	"IFM\n"
	"Debug status\n" )
{
	int type_num = 0;

	vty_out(vty,"debug type           status %s", VTY_NEWLINE);
	for(type_num = 0; type_num < array_size(ifm_dbg_name); ++type_num)
	{
		vty_out(vty, "%-15s    %-10s %s", ifm_dbg_name[type_num].str, 
			!!(vty->monitor & (1 << type_num)) ? "enable" : "disable", VTY_NEWLINE);
	}
	return CMD_SUCCESS;
}

/*显示指定接口流控状态，added by liub 2018-4-25*/
int flow_control_print (struct vty *vty, uint32_t ifindex)
{
    int ret = 0;
    char ifname[IFNET_NAMESIZE] = "";
    struct ifm_flow_control *flow_ctrl = NULL;

    ret = ifm_get_name_by_ifindex(ifindex, ifname);
    if(ret != 0)
    {
        zlog_err( "%s[%d] ifm_get_name_by_ifindex failed, error=%d\n", __FUNCTION__, __LINE__, ret);
    }

    /*flow_ctrl = ifm_get_flow_control(ifindex);
    if(flow_ctrl == NULL)
    {
        zlog_err( "%s[%d] ifm_get_flow_control failed, error=%d\n", __FUNCTION__, __LINE__, ret);
        return -1;
    }*/
    if(ifm_get_flow_control(ifindex, flow_ctrl) != 0)
    {
        zlog_err( "%s[%d] ifm_get_flow_control failed, error=%d\n", __FUNCTION__, __LINE__, ret);
        return -1;
    }

    vty_out(vty, "%-25s %9s %10s %12lld %12lld %s", 
                    ifname, 
                    (flow_ctrl->direction & IFM_FLOW_CONTROL_SEND)?"enable":"disable",
                    (flow_ctrl->direction & IFM_FLOW_CONTROL_RECEIVE)?"enable":"disable", 
                    flow_ctrl->rx_pause, 
                    flow_ctrl->tx_pause, 
                    VTY_NEWLINE);

    return 0;
}

/*显示接口流控状态，added by liub 2018-4-25*/
void ifm_flow_control_show ( struct vty *vty )
{
    struct ifm_entry *pifm = NULL;
    struct hash_bucket *pbucket = NULL;
    uint32_t cursor;

    IFM_ENTRTY_LOOP ( pbucket, cursor )
    {
        pifm = pbucket->data;
        if ( pifm == NULL )
        {
            zlog_err ( "%s[%d] pifm == NULL\n", __FUNCTION__, __LINE__ );
            return;
        }
        
        if ( !((IFM_TYPE_IS_ETHERNET(pifm->ifm.ifindex))
               || (IFM_TYPE_IS_GIGABIT_ETHERNET(pifm->ifm.ifindex)) 
               || (IFM_TYPE_IS_XGIGABIT_ETHERNET(pifm->ifm.ifindex))) 
               || (IFM_TYPE_IS_OUTBAND(pifm->ifm.ifindex))
               || (IFM_TYPE_IS_SUBPORT(pifm->ifm.ifindex))
               || (IFM_TYPE_IS_TRUNK(pifm->ifm.ifindex)))
        {
            continue;
        }

        flow_control_print(vty, pifm->ifm.ifindex);

    }

    return;
}


/* 显示内容:
Physical status is down, administrator status is enabled.
 Hardware is Ethernet  Current HW addr: 001d.8011.2001
 Physical:001d.8011.2001  Logical:(not set)
Description: not set
Mtu 2000 bytes.
*/

/*
Ethernet0/0/1 current state : DOWN
Line protocol current state : DOWN
Description:
Route Port,The Maximum Transmit Unit is 1500
Internet protocol processing : disabled
IP Sending Frames' Format is PKTFMT_ETHNT_2, Hardware address is 5489-989f-6d6d
Last physical up time   : -
Last physical down time : 2016-05-06 17:57:12 UTC-08:00
Current system time: 2016-05-08 10:04:05-08:00
Hardware address is 5489-989f-6d6d
    Last 300 seconds input rate 0 bytes/sec, 0 packets/sec
    Last 300 seconds output rate 0 bytes/sec, 0 packets/sec
    Input: 0 bytes, 0 packets
    Output: 0 bytes, 0 packets
    Input:
      Unicast: 0 packets, Multicast: 0 packets
      Broadcast: 0 packets
    Output:
      Unicast: 0 packets, Multicast: 0 packets
      Broadcast: 0 packets
    Input bandwidth utilization  :    0%
    Output bandwidth utilization :    0%
*/
void ifm_brief_show ( struct vty *vty,  struct ifm_entry *pifm )
{
    struct ifm_entry *tpifm = NULL;
     struct ifm_port  pspeed = {0};
    struct ifm_counter  pcounter = {0};
    char ifname[IFNET_NAMESIZE] = "";
	int dev_id = 0;
	void *state = NULL;	
	struct ipc_mesg_n * pMsgRcv = NULL;

	devm_comm_get_id(1, 0, MODULE_ID_IFM, &dev_id );

    if ( pifm == NULL )
    { return; }

    ifm_get_name_by_ifindex ( pifm->ifm.ifindex, ifname );

    vty_out ( vty, "interface %s%s", ifname, VTY_NEWLINE );
    vty_out ( vty, " Alias: %s %s", pifm->alias, VTY_NEWLINE );

    if ( pifm->ifm.sub_type == IFNET_SUBTYPE_SUBPORT )
    {
        tpifm = ifm_lookup ( IFM_PARENT_IFINDEX_GET ( pifm->ifm.ifindex ) );
        if ( !tpifm )
        {
            return;
        }
    }
    else
    {
        tpifm = pifm;
    }

    vty_out ( vty, " Subtype: %s %s", ifm_get_sub_typestr ( pifm->ifm.sub_type ), VTY_NEWLINE );

    switch ( tpifm->ifm.type )
    {

        case IFNET_TYPE_ETHERNET:
		case IFNET_TYPE_GIGABIT_ETHERNET:
		case IFNET_TYPE_XGIGABIT_ETHERNET:
		    if(tpifm->ifm.type == IFNET_TYPE_GIGABIT_ETHERNET && tpifm->port_info.fiber == IFNET_FIBERTYPE_COPPER)
		    {
                uint32_t ms = PORT_MS_AUTO;
                char *msinfo[4] = {"auto", "master", "slave", "auto"};
                if(ifm_get_phymaster(tpifm->ifm.ifindex, &ms) == 0)
                {
                    vty_out ( vty, " Phy mode: %s%s",msinfo[ms], VTY_NEWLINE);
                }                
		    }
            vty_out ( vty, " Link state: %s, Administrator status: %s %s", pifm->ifm.status == IFNET_LINKUP ? "up" : "down",
                      pifm->ifm.shutdown == IFNET_NO_SHUTDOWN ? "enabled" : "disable", VTY_NEWLINE );
            if ( pifm->ifm.sub_type != IFNET_SUBTYPE_SUBPORT )
            {
                /*start show speed*/
                /*speed show logic:*/
                /*if (up && config auto) show all speed config from hw                         */
                /*else if(up && config no auto)                                       */
                /*                                    if bandwith is config show bandwith config from ifm config  */
                /*                                    if bandwith is no config show bandwith config from from hw  */
                /*else if(down && config auto) no show auto config have no bandwith and duplex config*/
                /*else if(down && config no auto)show duplex and bandwith config */
                if ( pifm->ifm.status == IFNET_LINKUP )
                {
                    if ( pifm->port_info.autoneg == IFNET_SPEED_MODE_AUTO )
                    {
                        if(ifm_get_speed ( tpifm->ifm.ifindex , &pspeed) == 0)
                        {
                            vty_out ( vty, " AutoNegotiation:  %s %s", pspeed.autoneg == IFNET_SPEED_MODE_FORCE ? "disable" : "enable", VTY_NEWLINE );
                            if ( tpifm->ifm.status == IFNET_LINKUP )
                            {
                                vty_out ( vty, " Duplex: %s, Bandwidth: %s %s",
                                          pspeed.duplex == IFNET_DUPLEX_FULL ? "full" : "half",
                                          ( pspeed.speed == IFNET_SPEED_GE ? "1000" :
                                            ( pspeed.speed == IFNET_SPEED_FE ? "100" :
                                              ( pspeed.speed == IFNET_SPEED_10GE ? "10000" :
						    (pspeed.speed == IFNET_SPEED_10M ? "10" :"--" )) ) ), VTY_NEWLINE );
                            }
                        }
                        else
                        {
                            zlog_err ( "%s[%d] no reply for get pspeed\n", __FUNCTION__, __LINE__ );
                        }
                    }
                    else
                    {
                        vty_out ( vty, " AutoNegotiation:  %s %s", pifm->port_info.autoneg == IFNET_SPEED_MODE_FORCE ? "disable" : "enable", VTY_NEWLINE );
                        if ( pifm->port_info.speed == IFNET_SPEED_INVALID )
                        {
                            
                            if(ifm_get_speed ( tpifm->ifm.ifindex , &pspeed) == 0)
                            {
                                vty_out ( vty, " Duplex: %s, Bandwidth: %s %s",
                                          pspeed.duplex == IFNET_DUPLEX_FULL ? "full" : "half",
                                          ( pspeed.speed == IFNET_SPEED_GE ? "1000" :
                                            ( pspeed.speed == IFNET_SPEED_FE ? "100" :
                                              ( pspeed.speed == IFNET_SPEED_10GE ? "10000" :
						    (pspeed.speed == IFNET_SPEED_10M ? "10" :"--" )) ) ), VTY_NEWLINE );

                            }
                            else
                            {
                                zlog_err ( "%s[%d] no reply for get pspeed\n", __FUNCTION__, __LINE__ );
                            }

                        }
                        else
                        {
                            vty_out ( vty, " Duplex: %s, Bandwidth: %s %s",
                                      pifm->port_info.duplex == IFNET_DUPLEX_FULL ? "full" : "half",
                                      ( pifm->port_info.speed == IFNET_SPEED_GE ? "1000" :
                                        ( pifm->port_info.speed == IFNET_SPEED_FE ? "100" :
                                              ( pifm->port_info.speed  == IFNET_SPEED_10GE ? "10000" :
						    ( pifm->port_info.speed  == IFNET_SPEED_10M ? "10" :"--" )) ) ), VTY_NEWLINE );
                        }
                    }
                }
                else if ( pifm->ifm.status == IFNET_LINKDOWN )
                {
                    vty_out ( vty, " AutoNegotiation:  %s %s", pifm->port_info.autoneg == IFNET_SPEED_MODE_FORCE ? "disable" : "enable", VTY_NEWLINE );
                    if ( pifm->port_info.autoneg == IFNET_SPEED_MODE_FORCE )
                    {

                        vty_out ( vty, " Duplex: %s, Bandwidth: %s %s",
                                  pifm->port_info.duplex == IFNET_DUPLEX_FULL ? "full" : "half",
                                  ( pifm->port_info.speed == IFNET_SPEED_GE ? "1000" :
                                    ( pifm->port_info.speed == IFNET_SPEED_FE ? "100" :
						( pifm->port_info.speed  == IFNET_SPEED_10GE ? "10000" :
						( pifm->port_info.speed  == IFNET_SPEED_10M ? "10" :"--" )) ) ), VTY_NEWLINE );

                    }

                }
                /*end show speed*/
                if(pifm->ifm.mode != IFNET_MODE_PHYSICAL )
                {
                    vty_out ( vty, " Jumbo : %d bytes	%s", pifm->ifm.jumbo, VTY_NEWLINE );
					vty_out ( vty, " Interval : %d      %s",pifm->ifm.interval, VTY_NEWLINE );					
					if(pifm->port_info.fiber != IFNET_FIBERTYPE_COMBO )
					{
						vty_out ( vty, " Media : %s     %s", pifm->port_info.fiber == IFNET_FIBERTYPE_FIBER ? "fiber" :
						(pifm->port_info.fiber == IFNET_FIBERTYPE_COPPER ? "copper" :
						(pifm->port_info.fiber == IFNET_FIBERTYPE_COMBO ? "combo" : "other" )), VTY_NEWLINE);
					}
					else
					{
						vty_out ( vty, " Media : %s(%s)    %s", "combo",
						(pifm->port_info.fiber_sub == IFNET_FIBERTYPE_COPPER ? "copper" : "fiber"), VTY_NEWLINE);
					}
				}
            }

			if(pifm->ifm.mode != IFNET_MODE_PHYSICAL )
			{
				vty_out ( vty, " Mtu : %d bytes	%s", pifm->ifm.mtu, VTY_NEWLINE );
			}

            vty_out ( vty, " Ethernet address: %02x:%02x:%02x:%02x:%02x:%02x  %s",
                      tpifm->ifm.mac[0], tpifm->ifm.mac[1],
                      tpifm->ifm.mac[2], tpifm->ifm.mac[3],
                      tpifm->ifm.mac[4], tpifm->ifm.mac[5], VTY_NEWLINE );

			if(pifm->ifm.mode != IFNET_MODE_PHYSICAL )
			{

			if ( pifm->ifm.statistics == IFNET_STAT_ENABLE )
            {
                if(ifm_get_counter(pifm->ifm.ifindex, &pcounter) == 0)
                {
                    if ( pifm->ifm.sub_type == IFNET_SUBTYPE_SUBPORT )
                    {
                        vty_out ( vty, " Input  Total: %llu bytes, %llu packets %s", pcounter.rx_bytes, pcounter.rx_packets, VTY_NEWLINE );
                        vty_out ( vty, " Output Total: %llu bytes, %llu packets %s", pcounter.tx_bytes, pcounter.tx_packets, VTY_NEWLINE );
                    }
                    else
                    {
                        if ( IFM_TYPE_IS_OUTBAND ( pifm->ifm.ifindex ) )
                        {
                            vty_out ( vty, " Input  Total: %llu bytes, %llu packets %s", pcounter.rx_bytes, pcounter.rx_packets, VTY_NEWLINE );
                            vty_out ( vty, " Output Total: %llu bytes, %llu packets %s", pcounter.tx_bytes, pcounter.tx_packets, VTY_NEWLINE );
                        }
                        else
                        {
                            vty_out ( vty, " Input  Total: %llu bytes, %llu packets, Drop: %llu packets %s",
                                      pcounter.rx_bytes, pcounter.rx_packets, pcounter.rx_dropped, VTY_NEWLINE );
                            vty_out ( vty, " Output Total: %llu bytes, %llu packets, Drop: %llu packets %s",
                                      pcounter.tx_bytes, pcounter.tx_packets, pcounter.tx_dropped, VTY_NEWLINE );
                            vty_out ( vty, " Input: %s", VTY_NEWLINE );

                            vty_out ( vty, " Multicast: %llu packets Broadcast: %llu packets Unicast: %llu packets %s",
                                      pcounter.rx_multicast, pcounter.rx_broadcast, pcounter.rx_ucast, VTY_NEWLINE );
                            vty_out ( vty, " CRC error: %llu packets UndersizeErrors: %llu packets OverSizeErrors: %llu packets %s",
                                      pcounter.bad_crc, pcounter.undersize_pkts, pcounter.oversize_pkts_rcv, VTY_NEWLINE );

                            vty_out ( vty, " Output: %s", VTY_NEWLINE );
                            vty_out ( vty, " Multicast: %llu packets Broadcast: %llu packets Unicast: %llu packets %s",
                                        pcounter.tx_multicast, pcounter.tx_broadcast, pcounter.tx_ucast, VTY_NEWLINE );

							if( dev_id == ID_HT157 || dev_id == ID_HT158 )
							{
								vty_out ( vty, " Input: %s", VTY_NEWLINE );
                            	vty_out ( vty, " Filtered: %llu packets %s", pcounter.rx_filtered, VTY_NEWLINE );
							}
                        }
                    }
                }
                else
                {
                    zlog_err ( "%s[%d] no reply for get counter\n", __FUNCTION__, __LINE__ );
                }
            }
            else
            {
                zlog_info ( "%s[%d] ifindex 0x%0x counter disable statistics=%d\n", __FUNCTION__, __LINE__ , pifm->ifm.ifindex, pifm->ifm.statistics );
            }

			}

            break;
        case IFNET_TYPE_TDM:
            vty_out ( vty, " Link state: %s, Administrator status: %s %s", pifm->ifm.status == IFNET_LINKUP ? "up" : "down",
                      pifm->ifm.shutdown == IFNET_NO_SHUTDOWN ? "enabled" : "disable", VTY_NEWLINE );

            vty_out ( vty, " Ethernet address: %02x:%02x:%02x:%02x:%02x:%02x  %s",
                      tpifm->ifm.mac[0], tpifm->ifm.mac[1],
                      tpifm->ifm.mac[2], tpifm->ifm.mac[3],
                      tpifm->ifm.mac[4], tpifm->ifm.mac[5], VTY_NEWLINE );
            if ( pifm->ifm.statistics == IFNET_STAT_ENABLE )
            {
                if ( ifm_get_counter ( pifm->ifm.ifindex, &pcounter) == 0)
                {
                    vty_out ( vty, " Loss: %llu packets %s", pcounter.rx_dropped, VTY_NEWLINE );

                }
                else
                {
                    zlog_err ( "%s[%d] no reply for get counter\n", __FUNCTION__, __LINE__ );
                }
            }
            else
            {
                zlog_info ( "%s[%d] ifindex 0x%0x counter disable statistics=%d\n", __FUNCTION__, __LINE__ , tpifm->ifm.ifindex, tpifm->ifm.statistics );
            }

            break;
        case IFNET_TYPE_STM:			
		
			//state = ipc_send_hal_wait_reply(NULL, 0, 1 , MODULE_ID_HAL, MODULE_ID_IFM, 
			//		IPC_TYPE_STM, IFNET_INFO_STATUS, IPC_OPCODE_GET, pifm->ifm.ifindex);
			pMsgRcv = ipc_sync_send_n2(NULL, 0, 0, MODULE_ID_HAL, MODULE_ID_IFM, 
							IPC_TYPE_STM, IFNET_INFO_STATUS, IPC_OPCODE_GET, pifm->ifm.ifindex, 0);
			if(pMsgRcv)
			{
				if(pMsgRcv->msghdr.opcode == IPC_OPCODE_REPLY)
		        {
		             pifm->ifm.status = *((uint8_t*)pMsgRcv->msg_data);					 
		        } 
				mem_share_free(pMsgRcv, MODULE_ID_IFM);	 
		            
			}
			

			//pifm->ifm.status = *((uint8_t* )state);
            vty_out ( vty, " Link state: %s, Administrator status: %s %s", pifm->ifm.status == IFNET_LINKUP ? "up" : "down",
                      pifm->ifm.shutdown == IFNET_NO_SHUTDOWN ? "enabled" : "disable", VTY_NEWLINE );

            vty_out ( vty, " Ethernet address: %02x:%02x:%02x:%02x:%02x:%02x  %s",
                      tpifm->ifm.mac[0], tpifm->ifm.mac[1],
                      tpifm->ifm.mac[2], tpifm->ifm.mac[3],
                      tpifm->ifm.mac[4], tpifm->ifm.mac[5], VTY_NEWLINE );

            if ( pifm->ifm.statistics == IFNET_STAT_ENABLE )
            {
//                pcounter = ifm_get_counter ( tpifm->ifm.ifindex );

//                if ( pcounter != NULL )
//                {
//                    //need add code
//                }
//                else
//                {
//                    zlog_err ( "%s[%d] no reply for get counter\n", __FUNCTION__, __LINE__ );
//                }
            }
            else
            {
                zlog_info ( "%s[%d] ifindex 0x%0x counter disable statistics=%d\n", __FUNCTION__, __LINE__ , pifm->ifm.ifindex, pifm->ifm.statistics );
            }

            break;
        case IFNET_TYPE_TRUNK:
            vty_out ( vty, " Link state: %s, Administrator status: %s %s", pifm->ifm.status == IFNET_LINKUP ? "up" : "down",
                      pifm->ifm.shutdown == IFNET_NO_SHUTDOWN ? "enabled" : "disable", VTY_NEWLINE );

            if ( pifm->ifm.sub_type != IFNET_SUBTYPE_SUBPORT )
            {
                vty_out ( vty, " Jumbo : %d bytes	%s", pifm->ifm.jumbo, VTY_NEWLINE );
            }
            vty_out ( vty, " Mtu : %d bytes	%s", pifm->ifm.mtu, VTY_NEWLINE );
            vty_out ( vty, " Ethernet address: %02x:%02x:%02x:%02x:%02x:%02x  %s",
                      tpifm->ifm.mac[0], tpifm->ifm.mac[1],
                      tpifm->ifm.mac[2], tpifm->ifm.mac[3],
                      tpifm->ifm.mac[4], tpifm->ifm.mac[5], VTY_NEWLINE );

            if ( pifm->ifm.statistics == IFNET_STAT_ENABLE )
            {
                if (ifm_get_counter ( pifm->ifm.ifindex, &pcounter) == 0)
                {
                    if ( pifm->ifm.sub_type == IFNET_SUBTYPE_SUBPORT )
                    {
                        vty_out ( vty, " Input  Total: %llu bytes, %llu packets %s", pcounter.rx_bytes, pcounter.rx_packets, VTY_NEWLINE );
                        vty_out ( vty, " Output Total: %llu bytes, %llu packets %s", pcounter.tx_bytes, pcounter.tx_packets, VTY_NEWLINE );
                    }
                    else
                    {
                        vty_out ( vty, " Input  Total: %llu bytes, %llu packets, Drop: %llu packets %s",
                                  pcounter.rx_bytes, pcounter.rx_packets, pcounter.rx_dropped, VTY_NEWLINE );
                        vty_out ( vty, " Output Total: %llu bytes, %llu packets, Drop: %llu packets %s",
                                  pcounter.tx_bytes, pcounter.tx_packets, pcounter.tx_dropped, VTY_NEWLINE );
                        vty_out ( vty, " Input: %s", VTY_NEWLINE );

                        vty_out ( vty, " Multicast: %llu packets Broadcast: %llu packets Unicast: %llu packets %s",
                                  pcounter.rx_multicast, pcounter.rx_broadcast, pcounter.rx_ucast, VTY_NEWLINE );
                        vty_out ( vty, " CRC error: %llu packets UndersizeErrors: %llu packets OverSizeErrors: %llu packets %s",
                                  pcounter.bad_crc, pcounter.undersize_pkts, pcounter.oversize_pkts_rcv, VTY_NEWLINE );

                        vty_out ( vty, " Output: %s", VTY_NEWLINE );
                        vty_out ( vty, " Multicast: %llu packets Broadcast: %llu packets Unicast: %llu packets %s",
                                   pcounter.tx_multicast, pcounter.tx_broadcast, pcounter.tx_ucast, VTY_NEWLINE );
						if( dev_id == ID_HT157 || dev_id == ID_HT158 )
						{
							vty_out ( vty, " Input: %s", VTY_NEWLINE );
                        	vty_out ( vty, " Filtered: %llu packets %s", pcounter.rx_filtered, VTY_NEWLINE );
						}
                    }
                }
                else
                {
                    zlog_err ( "%s[%d] no reply for get counter\n", __FUNCTION__, __LINE__ );
                }
            }
            else
            {
                zlog_info ( "%s[%d] ifindex 0x%0x counter disable statistics=%d\n", __FUNCTION__, __LINE__ , pifm->ifm.ifindex, pifm->ifm.statistics );
            }

            break;
        case IFNET_TYPE_VLANIF:
            vty_out ( vty, " Link state: %s, Administrator status: %s %s", pifm->ifm.status == IFNET_LINKUP ? "up" : "down",
                      pifm->ifm.shutdown == IFNET_NO_SHUTDOWN ? "enabled" : "disable", VTY_NEWLINE );

            vty_out ( vty, " Mtu : %d bytes.	%s", pifm->ifm.mtu, VTY_NEWLINE );
            vty_out ( vty, " Ethernet address: %02x:%02x:%02x:%02x:%02x:%02x  %s",
                      tpifm->ifm.mac[0], tpifm->ifm.mac[1],
                      tpifm->ifm.mac[2], tpifm->ifm.mac[3],
                      tpifm->ifm.mac[4], tpifm->ifm.mac[5], VTY_NEWLINE );
            if ( pifm->ifm.statistics == IFNET_STAT_ENABLE )
            {
                if ( ifm_get_counter ( pifm->ifm.ifindex, &pcounter) == 0 )
                {
                    vty_out ( vty, " Input	Total: %llu bytes, %llu packets %s", pcounter.rx_bytes, pcounter.rx_packets, VTY_NEWLINE );
                    vty_out ( vty, " Output Total: %llu bytes, %llu packets %s", pcounter.tx_bytes, pcounter.tx_packets, VTY_NEWLINE );

                }
                else
                {
                    zlog_err ( "%s[%d] no reply for get counter\n", __FUNCTION__, __LINE__ );
                }
            }
            else
            {
                zlog_info ( "%s[%d] ifindex 0x%0x counter disable statistics=%d\n", __FUNCTION__, __LINE__ , tpifm->ifm.ifindex, tpifm->ifm.statistics );
            }

            break;
        case IFNET_TYPE_LOOPBACK:
            vty_out ( vty, " Link state: %s, Administrator status: %s %s", pifm->ifm.status == IFNET_LINKUP ? "up" : "down",
                      pifm->ifm.shutdown == IFNET_NO_SHUTDOWN ? "enabled" : "disable", VTY_NEWLINE );

            vty_out ( vty, " Mtu : %d bytes.	%s", pifm->ifm.mtu, VTY_NEWLINE );
            vty_out ( vty, " Ethernet address: %02x:%02x:%02x:%02x:%02x:%02x  %s",
                      tpifm->ifm.mac[0], tpifm->ifm.mac[1],
                      tpifm->ifm.mac[2], tpifm->ifm.mac[3],
                      tpifm->ifm.mac[4], tpifm->ifm.mac[5], VTY_NEWLINE );
            if ( pifm->ifm.statistics == IFNET_STAT_ENABLE
                    && pifm->ifm.sub_port == 0 )
            {
                if ( ifm_get_counter ( pifm->ifm.ifindex, &pcounter) == 0)
                {
                    vty_out ( vty, " Input	Total: %llu bytes, %llu packets %s", pcounter.rx_bytes, pcounter.rx_packets, VTY_NEWLINE );
                    vty_out ( vty, " Output Total: %llu bytes, %llu packets %s", pcounter.tx_bytes, pcounter.tx_packets, VTY_NEWLINE );

                }
                else
                {
                    zlog_err ( "%s[%d] no reply for get counter\n", __FUNCTION__, __LINE__ );
                }

            }
            else
            {
                zlog_info ( "%s[%d] ifindex 0x%0x counter disable statistics=%d\n", __FUNCTION__, __LINE__ , pifm->ifm.ifindex, pifm->ifm.statistics );
            }

            break;
        case IFNET_TYPE_TUNNEL:
            vty_out ( vty, " Link state: %s, Administrator status: %s %s", pifm->ifm.status == IFNET_LINKUP ? "up" : "down",
                      pifm->ifm.shutdown == IFNET_NO_SHUTDOWN ? "enabled" : "disable", VTY_NEWLINE );

            break;
		case IFNET_TYPE_VCG:
			break;
		case IFNET_TYPE_E1:
			break;			
        default:
            break;

    }
}
/*根据接口类型显示 接口*/
void ifm_type_brief_show ( struct vty *vty,  enum IFNET_TYPE type )
{
    struct ifm_entry *pifm = NULL;
    struct hash_bucket *pbucket = NULL;
    uint32_t cursor;
    struct listnode *node     = NULL;
    struct listnode *nextnode = NULL;
    int *pdata = NULL;
    struct ifm_entry *tpifm = NULL;


    IFM_ENTRTY_LOOP ( pbucket, cursor )
    {
        pifm = pbucket->data;

        if ( pifm == NULL )
        {
            zlog_err ( "%s[%d] pifm == NULL\n", __FUNCTION__, __LINE__ );
            return;
        }

        if ( pifm->ifm.type == type )
        {
            /*son interface config follow parent interface*/
            if ( ( IFM_TYPE_IS_METHERNET(pifm->ifm.ifindex) && IFM_TYPE_IS_METHERNET(pifm->ifm.parent) )
                    || ( IFM_TYPE_IS_TRUNK ( pifm->ifm.ifindex ) && IFM_TYPE_IS_TRUNK ( pifm->ifm.parent ) )
                    || ( IFM_TYPE_IS_TDM ( pifm->ifm.ifindex ) && IFM_TYPE_IS_TDM ( pifm->ifm.parent ) )
                    || ( IFM_TYPE_IS_STM ( pifm->ifm.ifindex ) && IFM_TYPE_IS_STM ( pifm->ifm.parent ) ) )
            {
                continue;
            }

            /*interface config*/
            ifm_brief_show ( vty, pifm );

            /*son interface config*/
            if ( pifm->sonlist )
            {
                for ( ALL_LIST_ELEMENTS ( pifm->sonlist, node, nextnode, pdata ) )
                {
                    tpifm = ( struct ifm_entry * ) pdata;
                    if ( tpifm == NULL )
                    {
                        zlog_err ( "%s[%d] tpifm == NULL\n", __FUNCTION__, __LINE__ );
                        return;
                    }

                    ifm_brief_show ( vty, tpifm );
                }
            }

        }
    }

}
/*显示接口*/
void ifm_all_brief_show ( struct vty *vty )
{
    struct ifm_entry *pifm = NULL;
    struct hash_bucket *pbucket = NULL;
    uint32_t cursor;
    struct listnode *node     = NULL;
    struct listnode *nextnode = NULL;
    int *pdata = NULL;
    struct ifm_entry *tpifm = NULL;


    IFM_ENTRTY_LOOP ( pbucket, cursor )
    {
        pifm = pbucket->data;
        if ( pifm == NULL )
        {
            zlog_err ( "%s[%d] pifm == NULL\n", __FUNCTION__, __LINE__ );
            return;
        }

        /*son interface config follow parent interface*/
        if ( ( IFM_TYPE_IS_METHERNET(pifm->ifm.ifindex) && IFM_TYPE_IS_METHERNET(pifm->ifm.parent) )
                || ( IFM_TYPE_IS_TRUNK ( pifm->ifm.ifindex ) && IFM_TYPE_IS_TRUNK ( pifm->ifm.parent ) )
                || ( IFM_TYPE_IS_TDM ( pifm->ifm.ifindex ) && IFM_TYPE_IS_TDM ( pifm->ifm.parent ) )
                || ( IFM_TYPE_IS_STM ( pifm->ifm.ifindex ) && IFM_TYPE_IS_STM ( pifm->ifm.parent ) ) )
        {
            continue;
        }

        /*interface config*/
        ifm_brief_show ( vty, pifm );

        /*son interface config*/
        if ( pifm->sonlist )
        {
            for ( ALL_LIST_ELEMENTS ( pifm->sonlist, node, nextnode, pdata ) )
            {
                tpifm = ( struct ifm_entry * ) pdata;
                if ( tpifm == NULL )
                {
                    zlog_err ( "%s[%d] tpifm == NULL\n", __FUNCTION__, __LINE__ );
                    return;
                }

                ifm_brief_show ( vty, tpifm );
            }
        }
    }

}

/*显示接口封装配置*/
void ifm_encap_config_show ( struct vty *vty,  struct ifm_entry *pifm )
{
    if ( pifm == NULL )
    {
        zlog_err ( "%s[%d] pifm == NULL\n", __FUNCTION__, __LINE__ );
        return;
    }

    switch ( pifm->ifm.encap.type )
    {
        case IFNET_ENCAP_UNTAG:
            vty_out ( vty, " Encapsulate: untag%s", VTY_NEWLINE );
            if ( pifm->ifm.encap.svlan_act !=  VLAN_ACTION_NO )
            {
                if ( pifm->ifm.encap.svlan_act == VLAN_ACTION_ADD )
                {
                    if ( pifm->ifm.encap.svlan_cos == 8 )
                    {
                        vty_out ( vty, "  Vlan translate: add vlan %d%s",
                                  pifm->ifm.encap.svlan_new.vlan_start, VTY_NEWLINE );

                    }
                    else
                    {
                        vty_out ( vty, "  Vlan translate: add vlan %d cos %d%s",
                                  pifm->ifm.encap.svlan_new.vlan_start,
                                  pifm->ifm.encap.svlan_cos, VTY_NEWLINE );
                    }
                }

            }
            else
            {
                vty_out ( vty, "  Vlan translate: --%s", VTY_NEWLINE );
            }

            break;
        case IFNET_ENCAP_DOT1Q:
            vty_out ( vty, " Encapsulate: dot1q%s", VTY_NEWLINE );
            if ( pifm->ifm.encap.svlan.vlan_start == pifm->ifm.encap.svlan.vlan_end )
            {
                vty_out ( vty, "  Vlan: %d%s", pifm->ifm.encap.svlan.vlan_start, VTY_NEWLINE );

                if ( pifm->ifm.encap.svlan_act !=  VLAN_ACTION_NO )
                {
                    if ( pifm->ifm.encap.svlan_act == VLAN_ACTION_ADD
                            && pifm->ifm.encap.cvlan_act == VLAN_ACTION_NO )
                    {
                        if ( pifm->ifm.encap.svlan_cos == 8 )
                        {
                            vty_out ( vty, "  Vlan translate: add svlan %d%s",
                                      pifm->ifm.encap.svlan_new.vlan_start, VTY_NEWLINE );
                        }
                        else
                        {
                            vty_out ( vty, "  Vlan translate: add svlan %d cos %d%s",
                                      pifm->ifm.encap.svlan_new.vlan_start,
                                      pifm->ifm.encap.svlan_cos, VTY_NEWLINE );
                        }
                    }
                    else if ( pifm->ifm.encap.svlan_act == VLAN_ACTION_TRANSLATE
                              && pifm->ifm.encap.cvlan_act == VLAN_ACTION_NO )
                    {
                        if ( pifm->ifm.encap.svlan_cos == 8 )
                        {
                            vty_out ( vty, "  Vlan translate: translate vlan %d%s",
                                      pifm->ifm.encap.svlan_new.vlan_start, VTY_NEWLINE );
                        }
                        else
                        {
                            vty_out ( vty, "  Vlan translate: translate vlan %d cos %d%s",
                                      pifm->ifm.encap.svlan_new.vlan_start,
                                      pifm->ifm.encap.svlan_cos, VTY_NEWLINE );
                        }
                    }
                    else if ( pifm->ifm.encap.svlan_act == VLAN_ACTION_ADD
                              && pifm->ifm.encap.cvlan_act == VLAN_ACTION_TRANSLATE )
                    {
                        if ( pifm->ifm.encap.svlan_cos == 8
                                && pifm->ifm.encap.cvlan_cos == 8 )
                        {
                            vty_out ( vty, "  Vlan translate: translate svlan %d cvlan %d%s",
                                      pifm->ifm.encap.svlan_new.vlan_start,
                                      pifm->ifm.encap.cvlan_new.vlan_start, VTY_NEWLINE );
                        }
                        else if ( pifm->ifm.encap.cvlan_cos == 8 )
                        {
                            vty_out ( vty, "  Vlan translate: translate svlan %d cos %d cvlan %d%s",
                                      pifm->ifm.encap.svlan_new.vlan_start,
                                      pifm->ifm.encap.svlan_cos,
                                      pifm->ifm.encap.cvlan_new.vlan_start, VTY_NEWLINE );
                        }
                        else if ( pifm->ifm.encap.svlan_cos == 8 )
                        {
                            vty_out ( vty, "  Vlan translate: translate svlan %d cvlan %d cos %d%s",
                                      pifm->ifm.encap.svlan_new.vlan_start,
                                      pifm->ifm.encap.cvlan_new.vlan_start,
                                      pifm->ifm.encap.cvlan_cos, VTY_NEWLINE );
                        }
                        else
                        {
                            vty_out ( vty, "  Vlan translate: translate svlan %d cos %d cvlan %d cos %d%s",
                                      pifm->ifm.encap.svlan_new.vlan_start,
                                      pifm->ifm.encap.svlan_cos,
                                      pifm->ifm.encap.cvlan_new.vlan_start,
                                      pifm->ifm.encap.cvlan_cos, VTY_NEWLINE );
                        }

                    }

                }
                else
                {
                    vty_out ( vty, "  Vlan translate: --%s", VTY_NEWLINE );
                }
            }
            else
            {
                vty_out ( vty, "  Vlan Range: %d to %d%s", pifm->ifm.encap.svlan.vlan_start, pifm->ifm.encap.svlan.vlan_end, VTY_NEWLINE );
                if ( pifm->ifm.encap.svlan_act !=  VLAN_ACTION_NO )
                {
                    if ( pifm->ifm.encap.svlan_act == VLAN_ACTION_ADD )
                    {
                        if ( pifm->ifm.encap.svlan_cos == 8 )
                        {
                            vty_out ( vty, "  Vlan translate: add svlan %d%s",
                                      pifm->ifm.encap.svlan_new.vlan_start, VTY_NEWLINE );
                        }
                        else
                        {
                            vty_out ( vty, "  Vlan translate: add svlan %d cos %d%s",
                                      pifm->ifm.encap.svlan_new.vlan_start,
                                      pifm->ifm.encap.svlan_cos, VTY_NEWLINE );
                        }
                    }

                }
                else
                {
                    vty_out ( vty, "  Vlan translate: --%s", VTY_NEWLINE );
                }
            }
            break;
        case IFNET_ENCAP_QINQ:
            vty_out ( vty, " Encapsulate: qinq%s", VTY_NEWLINE );
            if ( pifm->ifm.encap.cvlan.vlan_start == pifm->ifm.encap.cvlan.vlan_end )
            {
                vty_out ( vty, "  Svlan: %d%s", pifm->ifm.encap.svlan.vlan_start, VTY_NEWLINE );
                vty_out ( vty, "  Cvlan: %d%s", pifm->ifm.encap.cvlan.vlan_start, VTY_NEWLINE );
                if ( pifm->ifm.encap.svlan_act !=  VLAN_ACTION_NO )
                {
                    if ( pifm->ifm.encap.svlan_act == VLAN_ACTION_TRANSLATE
                            && pifm->ifm.encap.cvlan_act == VLAN_ACTION_TRANSLATE )
                    {

                        if ( pifm->ifm.encap.svlan_cos == 8
                                && pifm->ifm.encap.cvlan_cos == 8 )
                        {
                            vty_out ( vty, "  Vlan translate: translate svlan %d cvlan %d%s",
                                      pifm->ifm.encap.svlan_new.vlan_start,
                                      pifm->ifm.encap.cvlan_new.vlan_start, VTY_NEWLINE );
                        }
                        else if ( pifm->ifm.encap.cvlan_cos == 8 )
                        {
                            vty_out ( vty, "  Vlan translate: translate svlan %d cos %d cvlan %d%s",
                                      pifm->ifm.encap.svlan_new.vlan_start,
                                      pifm->ifm.encap.svlan_cos,
                                      pifm->ifm.encap.cvlan_new.vlan_start, VTY_NEWLINE );

                        }
                        else if ( pifm->ifm.encap.svlan_cos == 8 )
                        {
                            vty_out ( vty, "  Vlan translate: translate svlan %d cvlan %d cos %d%s",
                                      pifm->ifm.encap.svlan_new.vlan_start,
                                      pifm->ifm.encap.cvlan_new.vlan_start,
                                      pifm->ifm.encap.cvlan_cos, VTY_NEWLINE );

                        }
                        else
                        {
                            vty_out ( vty, "  Vlan translate: translate svlan %d cos %d cvlan %d cos %d%s",
                                      pifm->ifm.encap.svlan_new.vlan_start,
                                      pifm->ifm.encap.svlan_cos,
                                      pifm->ifm.encap.cvlan_new.vlan_start,
                                      pifm->ifm.encap.cvlan_cos, VTY_NEWLINE );

                        }

                    }
                    else if ( pifm->ifm.encap.svlan_act == VLAN_ACTION_DELETE
                              && pifm->ifm.encap.cvlan_act == VLAN_ACTION_NO )
                    {
                        vty_out ( vty, "  Vlan translate: delete svlan %s", VTY_NEWLINE );

                    }
                    else if ( pifm->ifm.encap.svlan_act == VLAN_ACTION_TRANSLATE
                              && pifm->ifm.encap.cvlan_act == VLAN_ACTION_NO )
                    {
                        if ( pifm->ifm.encap.svlan_cos == 8 )
                        {
                            vty_out ( vty, "  Vlan translate: translate svlan %d%s",
                                      pifm->ifm.encap.svlan_new.vlan_start, VTY_NEWLINE );

                        }
                        else
                        {
                            vty_out ( vty, "  Vlan translate: translate svlan %d cos %d%s",
                                      pifm->ifm.encap.svlan_new.vlan_start,
                                      pifm->ifm.encap.svlan_cos, VTY_NEWLINE );
                        }
                    }
                    else if ( pifm->ifm.encap.svlan_act == VLAN_ACTION_TRANSLATE
                              && pifm->ifm.encap.cvlan_act == VLAN_ACTION_DELETE )
                    {
                        if ( pifm->ifm.encap.svlan_cos == 8 )
                        {
                            vty_out ( vty, "  Vlan translate: translate vlan %d%s",
                                      pifm->ifm.encap.svlan_new.vlan_start, VTY_NEWLINE );

                        }
                        else
                        {
                            vty_out ( vty, "  Vlan translate: translate vlan %d cos %d%s",
                                      pifm->ifm.encap.svlan_new.vlan_start,
                                      pifm->ifm.encap.svlan_cos, VTY_NEWLINE );
                        }
                    }

                }
                else
                {
                    vty_out ( vty, "  Vlan translate: --%s", VTY_NEWLINE );
                }

            }
            else
            {
                vty_out ( vty, "  Svlan: %d%s", pifm->ifm.encap.svlan.vlan_start, VTY_NEWLINE );
                vty_out ( vty, "  CVlan Range: %d to %d%s", pifm->ifm.encap.cvlan.vlan_start, pifm->ifm.encap.cvlan.vlan_end, VTY_NEWLINE );
                if ( pifm->ifm.encap.svlan_act == VLAN_ACTION_DELETE )
                {
                    vty_out ( vty, "  Vlan translate: delete svlan%s", VTY_NEWLINE );
                }
                else
                {
                    vty_out ( vty, "  Vlan translate: --%s", VTY_NEWLINE );
                }
            }
            break;
        default:
            vty_out ( vty, " Encapsulate: --%s", VTY_NEWLINE );
            break;
    }
}

/*显示接口配置*/
void ifm_interface_config_show ( struct vty *vty,  struct ifm_entry *pifm )
{
    struct ifm_entry *tpifm = NULL;
    char ifname[IFNET_NAMESIZE] = "";

    if ( pifm == NULL )
    {
        zlog_err ( "%s[%d] pifm == NULL\n", __FUNCTION__, __LINE__ );
        return;
    }


    ifm_get_name_by_ifindex ( pifm->ifm.ifindex, ifname );

    vty_out ( vty, "interface %s%s", ifname, VTY_NEWLINE );

    vty_out ( vty, " Alias : %s %s", pifm->alias, VTY_NEWLINE );
    vty_out ( vty, " Subtype : %s %s", ifm_get_sub_typestr ( pifm->ifm.sub_type ), VTY_NEWLINE );

    switch ( pifm->ifm.type )
    {

        case IFNET_TYPE_ETHERNET:
		case IFNET_TYPE_GIGABIT_ETHERNET:
		case IFNET_TYPE_XGIGABIT_ETHERNET:
            vty_out ( vty, " Administrator: %s%s", pifm->ifm.shutdown == IFNET_SHUTDOWN ? "shutdown" : "no shutdown", VTY_NEWLINE );
            if ( IFM_TYPE_IS_TRUNK ( pifm->ifm.parent ) )
            {
                vty_out ( vty, " AutoNegotiation:  %s %s", pifm->port_info.autoneg == IFNET_SPEED_MODE_FORCE ? "disable" : "enable", VTY_NEWLINE );
                if ( pifm->port_info.autoneg == IFNET_SPEED_MODE_FORCE )
                {
                    vty_out ( vty, " Duplex: %s, Bandwidth: %s %s",
                              pifm->port_info.duplex == IFNET_DUPLEX_FULL ? "full" : "half",
                              ( pifm->port_info.speed == IFNET_SPEED_GE ? "1000" :
                                ( pifm->port_info.speed == IFNET_SPEED_FE ? "100" :
                                  ( pifm->port_info.speed == IFNET_SPEED_10GE ? "10000" : "--" ) ) ), VTY_NEWLINE );
                }

            }
            else
            {
                /*son interface */
                if ( IFM_IS_SUBPORT ( pifm->ifm.ifindex ) )
                {
                    tpifm = ifm_lookup ( IFM_PARENT_IFINDEX_GET ( pifm->ifm.ifindex ) );
                    if ( !tpifm )
                    {
                        return;
                    }
                    vty_out ( vty, " Mode : %s%s", pifm->ifm.mode == IFNET_MODE_L2 ? "l2" :
                              ( pifm->ifm.mode == IFNET_MODE_L3 ? "l3" : "switch" ), VTY_NEWLINE );
                    vty_out ( vty, " Mtu : %d Bytes %s", pifm->ifm.mtu, VTY_NEWLINE );

                    vty_out ( vty, " Statistics : %s %s", pifm->ifm.statistics == IFNET_STAT_ENABLE ? "enable" : "disable", VTY_NEWLINE );

                    ifm_encap_config_show ( vty, pifm );


                }
                else
                {
                    vty_out ( vty, " AutoNegotiation:  %s %s", pifm->port_info.autoneg == IFNET_SPEED_MODE_FORCE ? "disable" : "enable", VTY_NEWLINE );
                    if ( pifm->port_info.autoneg == IFNET_SPEED_MODE_FORCE )
                    {
                        vty_out ( vty, " Duplex: %s, Bandwidth: %s %s",
                                  pifm->port_info.duplex == IFNET_DUPLEX_FULL ? "full" : "half",
                                  ( pifm->port_info.speed == IFNET_SPEED_GE ? "1000" :
                                    ( pifm->port_info.speed == IFNET_SPEED_FE ? "100" :
                                      ( pifm->port_info.speed == IFNET_SPEED_10GE ? "10000" : "--" ) ) ), VTY_NEWLINE );

                    }

                    // 2018-05-04, by caojt, add IFNET_MODE_PHYSICAL for L1 ethernet port
                    if (pifm->ifm.mode != IFNET_MODE_PHYSICAL)
                    {
                        vty_out ( vty, " Mode : %s%s", pifm->ifm.mode == IFNET_MODE_L2 ? "l2" :
                                    ( pifm->ifm.mode == IFNET_MODE_L3 ? "l3" : "switch" ), VTY_NEWLINE );

                        vty_out ( vty, " Mtu : %d Bytes %s", pifm->ifm.mtu, VTY_NEWLINE );
                        vty_out ( vty, " Jumbo : %d Bytes %s", pifm->ifm.jumbo, VTY_NEWLINE );
                        vty_out ( vty, " Sfp off : %d s%s", pifm->ifm.sfp_off, VTY_NEWLINE );
                        vty_out ( vty, " Sfp on : %d s%s", pifm->ifm.sfp_on, VTY_NEWLINE );
                        vty_out ( vty, " Sfp als : %s %s", pifm->ifm.sfp_als == IFM_SFP_ALS_ENABLE  ? "enable" : "disable", VTY_NEWLINE );
                        vty_out ( vty, " Sfp tx : %s %s", pifm->ifm.sfp_tx == IFM_SFP_TX_ENABLE   ? "enable" : "disable", VTY_NEWLINE );

                        if (!IFM_TYPE_IS_OUTBAND(pifm->ifm.ifindex))
                        {
                            vty_out ( vty, " Tpid : 0x%0x %s", pifm->ifm.tpid, VTY_NEWLINE ); 
                        }

                        if ( pifm->port_info.flap_period == 0 )
                        {
                            vty_out ( vty, " Flap : disable %s", VTY_NEWLINE );
                        }
                        else
                        {
                            vty_out ( vty, " Flap period : %d seconds %s", pifm->port_info.flap_period, VTY_NEWLINE );
                        }
                    }
                }
            }
            if ( pifm->ifm.reflector.reflector_flag )
            {
                vty_out ( vty, " LOOPBACK:" );
                if ( pifm->ifm.reflector.reflector_flag & IFM_LOOPBACK_INTERNAL )
                {
                    vty_out ( vty, "  internal %s", VTY_NEWLINE  );
                }
                if ( pifm->ifm.reflector.reflector_flag & IFM_LOOPBACK_EXTERNEL )
                {
                    vty_out ( vty, "  external %s", VTY_NEWLINE  );
                }

            }
            break;
        case IFNET_TYPE_TDM:
        case IFNET_TYPE_STM:
            vty_out ( vty, " Administrator: %s%s", pifm->ifm.shutdown == IFNET_SHUTDOWN ? "shutdown" : "no shutdown", VTY_NEWLINE );
            vty_out ( vty, " Statistics : %s %s", pifm->ifm.statistics == IFNET_STAT_ENABLE ? "enable" : "disable", VTY_NEWLINE );

            break;
        case IFNET_TYPE_TRUNK:
            if ( pifm->ifm.parent == 0 )
            {
                vty_out ( vty, " Mode : %s%s", pifm->ifm.mode == IFNET_MODE_L2 ? "l2" :
                          ( pifm->ifm.mode == IFNET_MODE_L3 ? "l3" : "switch" ), VTY_NEWLINE );

                vty_out ( vty, " Mtu : %d Bytes %s", pifm->ifm.mtu, VTY_NEWLINE );

                vty_out ( vty, " Statistics : %s %s", pifm->ifm.statistics == IFNET_STAT_ENABLE ? "enable" : "disable", VTY_NEWLINE );

                vty_out ( vty, " Jumbo : %d Bytes %s", pifm->ifm.jumbo, VTY_NEWLINE );
                if ( !IFM_TYPE_IS_OUTBAND ( pifm->ifm.ifindex ) )
                { vty_out ( vty, " Tpid : 0x%0x %s", pifm->ifm.tpid, VTY_NEWLINE ); }

            }
            else
            {
                vty_out ( vty, " Administrator: %s%s", pifm->ifm.shutdown == IFNET_SHUTDOWN ? "shutdown" : "no shutdown", VTY_NEWLINE );

                vty_out ( vty, " Mode : %s%s", pifm->ifm.mode == IFNET_MODE_L2 ? "l2" :
                          ( pifm->ifm.mode == IFNET_MODE_L3 ? "l3" : "switch" ), VTY_NEWLINE );

                vty_out ( vty, " Mtu : %d Bytes %s", pifm->ifm.mtu, VTY_NEWLINE );

                vty_out ( vty, " Statistics : %s %s", pifm->ifm.statistics == IFNET_STAT_ENABLE ? "enable" : "disable", VTY_NEWLINE );

                tpifm = ifm_lookup ( IFM_PARENT_IFINDEX_GET ( pifm->ifm.ifindex ) );
                if ( !tpifm )
                {
                    return;
                }

                ifm_encap_config_show ( vty, pifm );

            }

            break;

        case IFNET_TYPE_VLANIF:
            vty_out ( vty, " Mtu : %d Bytes %s", pifm->ifm.mtu, VTY_NEWLINE );
            vty_out ( vty, " Statistics : %s %s", pifm->ifm.statistics == IFNET_STAT_ENABLE ? "enable" : "disable", VTY_NEWLINE );

            break;
        case IFNET_TYPE_LOOPBACK:
            vty_out ( vty, " Mtu : %d Bytes %s", pifm->ifm.mtu, VTY_NEWLINE );
            vty_out ( vty, " Statistics : %s %s", pifm->ifm.statistics == IFNET_STAT_ENABLE ? "enable" : "disable", VTY_NEWLINE );
            break;
        case IFNET_TYPE_TUNNEL:

            break;
		case IFNET_TYPE_VCG:
			break;
		case IFNET_TYPE_E1:
			break;				
        default:
            break;
    }

}

/*显示接口配置*/
void ifm_all_config_show ( struct vty *vty )
{
    struct ifm_entry *pifm = NULL;
    struct hash_bucket *pbucket = NULL;
    uint32_t cursor;
    struct listnode *node     = NULL;
    struct listnode *nextnode = NULL;
    int *pdata = NULL;
    struct ifm_entry *tpifm = NULL;


    IFM_ENTRTY_LOOP ( pbucket, cursor )
    {
        pifm = pbucket->data;

        if ( pifm == NULL )
        {
            zlog_err ( "%s[%d] pifm == NULL\n", __FUNCTION__, __LINE__ );
            return;
        }

        /*son interface config follow parent interface*/
        if ( ( IFM_TYPE_IS_METHERNET(pifm->ifm.ifindex) && IFM_TYPE_IS_METHERNET(pifm->ifm.parent) )
                || ( IFM_TYPE_IS_TRUNK ( pifm->ifm.ifindex ) && IFM_TYPE_IS_TRUNK ( pifm->ifm.parent ) )
                || ( IFM_TYPE_IS_TDM ( pifm->ifm.ifindex ) && IFM_TYPE_IS_TDM ( pifm->ifm.parent ) )
                || ( IFM_TYPE_IS_STM ( pifm->ifm.ifindex ) && IFM_TYPE_IS_STM ( pifm->ifm.parent ) ) )
        {
            continue;
        }

        /*interface config*/
        ifm_interface_config_show ( vty, pifm );

        /*son interface config*/
        if ( pifm->sonlist )
        {
            for ( ALL_LIST_ELEMENTS ( pifm->sonlist, node, nextnode, pdata ) )
            {
                tpifm = ( struct ifm_entry * ) pdata;
                if ( tpifm == NULL )
                {
                    zlog_err ( "%s[%d] tpifm == NULL\n", __FUNCTION__, __LINE__ );
                    return;
                }

                ifm_interface_config_show ( vty, tpifm );
            }
        }

    }

}


/*显示特定接口类型的接口配置*/

void ifm_type_config_show ( struct vty *vty,  enum IFNET_TYPE type )
{
    struct ifm_entry *pifm = NULL;
    struct hash_bucket *pbucket = NULL;
    uint32_t cursor;
    struct listnode *node     = NULL;
    struct listnode *nextnode = NULL;
    int *pdata = NULL;
    struct ifm_entry *tpifm = NULL;


    IFM_ENTRTY_LOOP ( pbucket, cursor )
    {
        pifm = pbucket->data;
        if ( pifm == NULL )
        {
            zlog_err ( "%s[%d] pifm == NULL\n", __FUNCTION__, __LINE__ );
            return;
        }

        if ( pifm->ifm.type == type )
        {
            /*son interface config follow parent interface*/
            if ( ( IFM_TYPE_IS_METHERNET(pifm->ifm.ifindex)&& IFM_TYPE_IS_METHERNET(pifm->ifm.parent) )
                    || ( IFM_TYPE_IS_TRUNK ( pifm->ifm.ifindex ) && IFM_TYPE_IS_TRUNK ( pifm->ifm.parent ) )
                    || ( IFM_TYPE_IS_TDM ( pifm->ifm.ifindex ) && IFM_TYPE_IS_TDM ( pifm->ifm.parent ) )
                    || ( IFM_TYPE_IS_STM ( pifm->ifm.ifindex ) && IFM_TYPE_IS_STM ( pifm->ifm.parent ) ) )
            {
                continue;
            }

            /*interface config*/
            ifm_interface_config_show ( vty, pifm );

            /*son interface config*/
            if ( pifm->sonlist )
            {
                for ( ALL_LIST_ELEMENTS ( pifm->sonlist, node, nextnode, pdata ) )
                {
                    tpifm = ( struct ifm_entry * ) pdata;
                    if ( tpifm == NULL )
                    {
                        zlog_err ( "%s[%d] tpifm == NULL\n", __FUNCTION__, __LINE__ );
                        return;
                    }

                    ifm_interface_config_show ( vty, tpifm );
                }
            }

        }
    }

}
/*接口封装恢复保存*/
void ifm_encap_config_write ( struct vty *vty,  struct ifm_info ifmifo )
{
    if ( ifmifo.encap.type != IFM_DEF_ENCP )
    {
        switch ( ifmifo.encap.type )
        {
            case IFNET_ENCAP_UNTAG:

                if ( ifmifo.encap.svlan_act !=  VLAN_ACTION_NO )
                {
                    if ( ifmifo.encap.svlan_act == VLAN_ACTION_ADD )
                    {
                        if ( ifmifo.encap.svlan_cos == 8 )
                        {
                            vty_out ( vty, " encapsulate untag add vlan %d%s",
                                      ifmifo.encap.svlan_new.vlan_start, VTY_NEWLINE );

                        }
                        else
                        {
                            vty_out ( vty, " encapsulate untag add vlan %d cos %d%s",
                                      ifmifo.encap.svlan_new.vlan_start,
                                      ifmifo.encap.svlan_cos, VTY_NEWLINE );
                        }
                    }

                }
                else
                {
                    vty_out ( vty, " encapsulate untag%s", VTY_NEWLINE );
                }
                break;
            case IFNET_ENCAP_DOT1Q:
                if ( ifmifo.encap.svlan_act !=  VLAN_ACTION_NO )
                {
                    if ( ifmifo.encap.svlan.vlan_start == ifmifo.encap.svlan.vlan_end )
                    {
                        if ( ifmifo.encap.svlan_act == VLAN_ACTION_ADD
                                && ifmifo.encap.cvlan_act == VLAN_ACTION_NO )
                        {
                            if ( ifmifo.encap.svlan_cos == 8 )
                            {
                                vty_out ( vty, " encapsulate dot1q %d add svlan %d%s",
                                          ifmifo.encap.svlan.vlan_start,
                                          ifmifo.encap.svlan_new.vlan_start, VTY_NEWLINE );
                            }
                            else
                            {
                                vty_out ( vty, " encapsulate dot1q %d add svlan %d cos %d%s",
                                          ifmifo.encap.svlan.vlan_start,
                                          ifmifo.encap.svlan_new.vlan_start,
                                          ifmifo.encap.svlan_cos, VTY_NEWLINE );
                            }
                        }
                        else if ( ifmifo.encap.svlan_act == VLAN_ACTION_TRANSLATE
                                  && ifmifo.encap.cvlan_act == VLAN_ACTION_NO )
                        {
                            if ( ifmifo.encap.svlan_cos == 8 )
                            {
                                vty_out ( vty, " encapsulate dot1q %d translate vlan %d%s",
                                          ifmifo.encap.svlan.vlan_start,
                                          ifmifo.encap.svlan_new.vlan_start, VTY_NEWLINE );
                            }
                            else
                            {
                                vty_out ( vty, " encapsulate dot1q %d translate vlan %d cos %d%s",
                                          ifmifo.encap.svlan.vlan_start,
                                          ifmifo.encap.svlan_new.vlan_start,
                                          ifmifo.encap.svlan_cos, VTY_NEWLINE );
                            }
                        }
                        else if ( ifmifo.encap.svlan_act == VLAN_ACTION_ADD
                                  && ifmifo.encap.cvlan_act == VLAN_ACTION_TRANSLATE )
                        {
                            if ( ifmifo.encap.svlan_cos == 8
                                    && ifmifo.encap.cvlan_cos == 8 )
                            {
                                vty_out ( vty, " encapsulate dot1q %d translate svlan %d cvlan %d%s",
                                          ifmifo.encap.svlan.vlan_start,
                                          ifmifo.encap.svlan_new.vlan_start,
                                          ifmifo.encap.cvlan_new.vlan_start, VTY_NEWLINE );
                            }
                            else if ( ifmifo.encap.cvlan_cos == 8 )
                            {
                                vty_out ( vty, " encapsulate dot1q %d translate svlan %d cos %d cvlan %d%s",
                                          ifmifo.encap.svlan.vlan_start,
                                          ifmifo.encap.svlan_new.vlan_start,
                                          ifmifo.encap.svlan_cos,
                                          ifmifo.encap.cvlan_new.vlan_start, VTY_NEWLINE );
                            }
                            else if ( ifmifo.encap.svlan_cos == 8 )
                            {
                                vty_out ( vty, " encapsulate dot1q %d translate svlan %d cvlan %d cos %d%s",
                                          ifmifo.encap.svlan.vlan_start,
                                          ifmifo.encap.svlan_new.vlan_start,
                                          ifmifo.encap.cvlan_new.vlan_start,
                                          ifmifo.encap.cvlan_cos, VTY_NEWLINE );
                            }
                            else
                            {
                                vty_out ( vty, " encapsulate dot1q %d translate svlan %d cos %d cvlan %d cos %d%s",
                                          ifmifo.encap.svlan.vlan_start,
                                          ifmifo.encap.svlan_new.vlan_start,
                                          ifmifo.encap.svlan_cos,
                                          ifmifo.encap.cvlan_new.vlan_start,
                                          ifmifo.encap.cvlan_cos, VTY_NEWLINE );
                            }

                        }

                    }
                    else
                    {
                        if ( ifmifo.encap.svlan_act == VLAN_ACTION_ADD )
                        {
                            if ( ifmifo.encap.svlan_cos == 8 )
                            {
                                vty_out ( vty, " encapsulate dot1q %d to %d add svlan %d%s",
                                          ifmifo.encap.svlan.vlan_start,
                                          ifmifo.encap.svlan.vlan_end,
                                          ifmifo.encap.svlan_new.vlan_start, VTY_NEWLINE );
                            }
                            else
                            {
                                vty_out ( vty, " encapsulate dot1q %d to %d add svlan %d cos %d%s",
                                          ifmifo.encap.svlan.vlan_start,
                                          ifmifo.encap.svlan.vlan_end,
                                          ifmifo.encap.svlan_new.vlan_start,
                                          ifmifo.encap.svlan_cos, VTY_NEWLINE );
                            }
                        }
                    }

                }
                else
                {
                    if ( ifmifo.encap.svlan.vlan_start == ifmifo.encap.svlan.vlan_end )
                    {
                        vty_out ( vty, " encapsulate dot1q %d%s", ifmifo.encap.svlan.vlan_start, VTY_NEWLINE );
                    }
                    else
                    {
                        vty_out ( vty, " encapsulate dot1q %d to %d%s", ifmifo.encap.svlan.vlan_start, ifmifo.encap.svlan.vlan_end, VTY_NEWLINE );
                    }
                }
                break;
            case IFNET_ENCAP_QINQ:
                if ( ifmifo.encap.svlan_act !=  VLAN_ACTION_NO )
                {
                    if ( ifmifo.encap.cvlan.vlan_start == ifmifo.encap.cvlan.vlan_end )
                    {
                        if ( ifmifo.encap.svlan_act == VLAN_ACTION_TRANSLATE
                                && ifmifo.encap.cvlan_act == VLAN_ACTION_TRANSLATE )
                        {

                            if ( ifmifo.encap.svlan_cos == 8
                                    && ifmifo.encap.cvlan_cos == 8 )
                            {
                                vty_out ( vty, " encapsulate qinq svlan %d cvlan %d translate svlan %d cvlan %d%s",
                                          ifmifo.encap.svlan.vlan_start,
                                          ifmifo.encap.cvlan.vlan_start,
                                          ifmifo.encap.svlan_new.vlan_start,
                                          ifmifo.encap.cvlan_new.vlan_start, VTY_NEWLINE );
                            }
                            else if ( ifmifo.encap.cvlan_cos == 8 )
                            {
                                vty_out ( vty, " encapsulate qinq svlan %d cvlan %d translate svlan %d cos %d cvlan %d%s",
                                          ifmifo.encap.svlan.vlan_start,
                                          ifmifo.encap.cvlan.vlan_start,
                                          ifmifo.encap.svlan_new.vlan_start,
                                          ifmifo.encap.svlan_cos,
                                          ifmifo.encap.cvlan_new.vlan_start, VTY_NEWLINE );

                            }
                            else if ( ifmifo.encap.svlan_cos == 8 )
                            {
                                vty_out ( vty, " encapsulate qinq svlan %d cvlan %d translate svlan %d cvlan %d cos %d%s",
                                          ifmifo.encap.svlan.vlan_start,
                                          ifmifo.encap.cvlan.vlan_start,
                                          ifmifo.encap.svlan_new.vlan_start,
                                          ifmifo.encap.cvlan_new.vlan_start,
                                          ifmifo.encap.cvlan_cos, VTY_NEWLINE );

                            }
                            else
                            {
                                vty_out ( vty, " encapsulate qinq svlan %d cvlan %d translate svlan %d cos %d cvlan %d cos %d%s",
                                          ifmifo.encap.svlan.vlan_start,
                                          ifmifo.encap.cvlan.vlan_start,
                                          ifmifo.encap.svlan_new.vlan_start,
                                          ifmifo.encap.svlan_cos,
                                          ifmifo.encap.cvlan_new.vlan_start,
                                          ifmifo.encap.cvlan_cos, VTY_NEWLINE );

                            }

                        }
                        else if ( ifmifo.encap.svlan_act == VLAN_ACTION_DELETE
                                  && ifmifo.encap.cvlan_act == VLAN_ACTION_NO )
                        {
                            vty_out ( vty, " encapsulate qinq svlan %d cvlan %d delete svlan%s",
                                      ifmifo.encap.svlan.vlan_start,
                                      ifmifo.encap.cvlan.vlan_start, VTY_NEWLINE );

                        }
                        else if ( ifmifo.encap.svlan_act == VLAN_ACTION_TRANSLATE
                                  && ifmifo.encap.cvlan_act == VLAN_ACTION_NO )
                        {
                            if ( ifmifo.encap.svlan_cos == 8 )
                            {
                                vty_out ( vty, " encapsulate qinq svlan %d cvlan %d translate svlan %d cvlan *%s",
                                          ifmifo.encap.svlan.vlan_start,
                                          ifmifo.encap.cvlan.vlan_start,
                                          ifmifo.encap.svlan_new.vlan_start, VTY_NEWLINE );

                            }
                            else
                            {
                                vty_out ( vty, " encapsulate qinq svlan %d cvlan %d translate svlan %d cos %d cvlan *%s",
                                          ifmifo.encap.svlan.vlan_start,
                                          ifmifo.encap.cvlan.vlan_start,
                                          ifmifo.encap.svlan_new.vlan_start,
                                          ifmifo.encap.svlan_cos, VTY_NEWLINE );
                            }
                        }
                        else if ( ifmifo.encap.svlan_act == VLAN_ACTION_TRANSLATE
                                  && ifmifo.encap.cvlan_act == VLAN_ACTION_DELETE )
                        {
                            if ( ifmifo.encap.svlan_cos == 8 )
                            {
                                vty_out ( vty, " encapsulate qinq svlan %d cvlan %d translate vlan %d%s",
                                          ifmifo.encap.svlan.vlan_start,
                                          ifmifo.encap.cvlan.vlan_start,
                                          ifmifo.encap.svlan_new.vlan_start, VTY_NEWLINE );

                            }
                            else
                            {
                                vty_out ( vty, " encapsulate qinq svlan %d cvlan %d translate vlan %d cos %d%s",
                                          ifmifo.encap.svlan.vlan_start,
                                          ifmifo.encap.cvlan.vlan_start,
                                          ifmifo.encap.svlan_new.vlan_start,
                                          ifmifo.encap.svlan_cos, VTY_NEWLINE );
                            }
                        }

                    }
                    else
                    {
                        if ( ifmifo.encap.svlan_act == VLAN_ACTION_DELETE )
                        {
                            vty_out ( vty, " encapsulate qinq svlan %d cvlan %d to %d delete svlan%s",
                                      ifmifo.encap.svlan.vlan_start,
                                      ifmifo.encap.cvlan.vlan_start,
                                      ifmifo.encap.cvlan.vlan_end, VTY_NEWLINE );
                        }
                    }

                }
                else
                {
                    if ( ifmifo.encap.cvlan.vlan_start == ifmifo.encap.cvlan.vlan_end )
                    {
                        vty_out ( vty, " encapsulate qinq svlan %d cvlan %d%s", ifmifo.encap.svlan.vlan_start, ifmifo.encap.cvlan.vlan_start, VTY_NEWLINE );
                    }
                    else
                    {
                        vty_out ( vty, " encapsulate qinq svlan %d cvlan %d to %d%s", ifmifo.encap.svlan.vlan_start, ifmifo.encap.cvlan.vlan_start, ifmifo.encap.cvlan.vlan_end, VTY_NEWLINE );
                    }
                }
                break;
            default:
                break;
        }
    }

}

/*接口配置恢复保存*/
void ifm_interface_config_write ( struct vty *vty, struct ifm_entry *pifm )
{
    struct ifm_info ifmifo;
    char ifname[IFNET_NAMESIZE] = "";
    char ip[INET_ADDRSTRLEN] = "";
    if ( pifm == NULL )
    {
        zlog_err ( "%s[%d] pifm == NULL\n", __FUNCTION__, __LINE__ );
        return;
    }

    memcpy ( &ifmifo, &pifm->ifm, sizeof ( struct ifm_info ) );

    ifm_get_name_by_ifindex ( pifm->ifm.ifindex, ifname );

    vty_out ( vty, "interface %s%s", ifname, VTY_NEWLINE );


    if ( pifm->alias[0] != '\0' )
    { vty_out ( vty, " alias %s%s", pifm->alias, VTY_NEWLINE ); }
    if ( ifmifo.shutdown != IFM_DEF_SHUTDOWN )
    {
        vty_out ( vty, " %s%s", ifmifo.shutdown == IFNET_SHUTDOWN ? "shutdown" : "no shutdown", VTY_NEWLINE );
    }
    switch ( ifmifo.type )
    {
        case IFNET_TYPE_ETHERNET:
		case IFNET_TYPE_GIGABIT_ETHERNET:
		
            /*interface add trunk*/
            if ( IFM_TYPE_IS_TRUNK ( ifmifo.parent ) )
            {
                if ( pifm->port_info.autoneg == IFNET_SPEED_MODE_AUTO )
                {
                    vty_out ( vty, " operation auto%s", VTY_NEWLINE );
                }
                else
                {
                    vty_out ( vty, " operation %s%s", pifm->port_info.duplex == IFNET_DUPLEX_FULL ? "duplex" : "simplex", VTY_NEWLINE );
                    if ( pifm->port_info.speed != IFNET_SPEED_INVALID )
                    {
                        vty_out ( vty, " speed %s%s",
                                  ( pifm->port_info.speed == IFNET_SPEED_GE ? "1000" :
                                    ( pifm->port_info.speed == IFNET_SPEED_FE ? "100" : "10000" ) ), VTY_NEWLINE );

                    }
                }
			
				
				if ( pifm->port_info.fiber == IFNET_FIBERTYPE_COMBO )
				{
					if(pifm->port_info.fiber_sub != IFM_DEF_COMBO_TYPE)
					{
						vty_out ( vty, " combo enable %s%s", pifm->port_info.fiber_sub == IFNET_FIBERTYPE_COPPER ? 
							"copper":"fiber", VTY_NEWLINE );
					}
				}
				
				
            }
            else
            {
                /*son interface */
                if ( IFM_IS_SUBPORT ( pifm->ifm.ifindex ) )
                {
                    if ( ifmifo.mode != IFM_DEF_MODE )
                    {
                        vty_out ( vty, " mode %s%s", ifmifo.mode == IFNET_MODE_L2 ? "l2" :
                                  ( ifmifo.mode == IFNET_MODE_L3 ? "l3" : "switch" ), VTY_NEWLINE );
                    }

                    if ( ifmifo.mtu != IFM_DEF_MTU )
                    {
                        vty_out ( vty, " mtu %d%s", ifmifo.mtu, VTY_NEWLINE );
                    }

                    if ( ifmifo.statistics != IFNET_STAT_DISABLE )
                    { vty_out ( vty, " statistics %s%s", ifmifo.statistics == IFNET_STAT_ENABLE ? "enable" : "disable", VTY_NEWLINE ); }

                    ifm_encap_config_write ( vty, ifmifo );

                }
                else
                {
                    if(ifmifo.phymaster != PORT_MS_AUTO && pifm->port_info.fiber == IFNET_FIBERTYPE_COPPER 
                                && IFM_TYPE_IS_GIGABIT_ETHERNET(pifm->ifm.ifindex))
                    {
                        if(ifmifo.phymaster == PORT_MS_MASTER)
                        {
                            vty_out ( vty, " phy mode master%s", VTY_NEWLINE ); 
                        }
                        else
                        {
                            vty_out ( vty, " phy mode slave%s", VTY_NEWLINE );
                        }
                    }
                    
					if ( pifm->port_info.fiber == IFNET_FIBERTYPE_COMBO )
					{
						if(pifm->port_info.fiber_sub != IFM_DEF_COMBO_TYPE)
						{
							vty_out ( vty, " combo enable %s%s", pifm->port_info.fiber_sub == IFNET_FIBERTYPE_COPPER ? 
								"copper":"fiber", VTY_NEWLINE );
						}
					}
                    if ( pifm->port_info.autoneg == IFNET_SPEED_MODE_AUTO )
                    {
                        vty_out ( vty, " operation auto%s", VTY_NEWLINE );
                    }
                    else
                    {
                        vty_out ( vty, " operation %s%s", pifm->port_info.duplex == IFNET_DUPLEX_FULL ? "duplex" : "simplex", VTY_NEWLINE );
                        if ( pifm->port_info.speed != IFNET_SPEED_INVALID )
                        {
                            vty_out ( vty, " speed %s%s",
                                      ( pifm->port_info.speed == IFNET_SPEED_GE ? "1000" :
                                        ( pifm->port_info.speed == IFNET_SPEED_FE ? "100" : "10000" ) ), VTY_NEWLINE );
                        }
                    }
					if(pifm->ifm.mode != IFNET_MODE_PHYSICAL)
					{
						if ( ifmifo.mode != IFM_DEF_MODE )
	                    {
	                        vty_out ( vty, " mode %s%s", ifmifo.mode == IFNET_MODE_L2 ? "l2" :
	                                  ( ifmifo.mode == IFNET_MODE_L3 ? "l3" : "switch" ), VTY_NEWLINE );
	                    }
					}
                    if ( ifmifo.mtu != IFM_DEF_MTU )
                    {
                        vty_out ( vty, " mtu %d%s", ifmifo.mtu, VTY_NEWLINE );
                    }

                    if ( ifmifo.jumbo != IFM_DEF_JUMBO )
                    {
                        vty_out ( vty, " jumbo-frame %d%s", ifmifo.jumbo, VTY_NEWLINE );
                    }

                    if ( pifm->port_info.flap_period != IFM_DEF_FLAP )
                    { vty_out ( vty, " flapping-control period %d%s", pifm->port_info.flap_period, VTY_NEWLINE ); }
                    if ( ifmifo.sfp_off != IFM_DEF_SFP_OFF )
                    {
                        vty_out ( vty, " sfp-config als off-time %d%s", ifmifo.sfp_off, VTY_NEWLINE );
                    }
                    if ( ifmifo.sfp_on != IFM_DEF_SFP_ON )
                    {
                        vty_out ( vty, " sfp-config als on-time %d%s", ifmifo.sfp_on, VTY_NEWLINE );
                    }
                    if ( ifmifo.sfp_tx != IFM_DEF_SFP_TX )
                    {
                        vty_out ( vty, " sfp-config tx %s%s", ifmifo.sfp_tx == IFM_SFP_TX_DISABLE ? "disable" : "enable", VTY_NEWLINE );
                    }
                    if ( ifmifo.sfp_als != IFM_DEF_SFP_ALS )
                    {
                        vty_out ( vty, " sfp-config als %s%s", ifmifo.sfp_als == IFM_SFP_ALS_ENABLE ? "enable" : "disable", VTY_NEWLINE );
                    }
                    if ( ifmifo.tpid != IFM_DEF_TPID )
                    { 
                        vty_out ( vty, " tpid 0x%0x%s", ifmifo.tpid, VTY_NEWLINE ); 
                    }
										

                    /*added by liub 2018-4-25 for flow control */
                    if ( ifmifo.flow_ctrl.status != IFM_FLOW_CONTROL_DISABLE) 
                    {
                        if(ifmifo.flow_ctrl.direction == IFM_FLOW_CONTROL_RECEIVE)
                        {
                            vty_out ( vty, " flow-control receive enable %s", VTY_NEWLINE ); 
                        }
                        else if(ifmifo.flow_ctrl.direction == IFM_FLOW_CONTROL_SEND)
                        {
                            vty_out ( vty, " flow-control send enable %s", VTY_NEWLINE ); 
                        }
                        else if(ifmifo.flow_ctrl.direction == IFM_FLOW_CONTROL_RECEIVE_AND_SEND)
                        {
                            vty_out ( vty, " flow-control %s", VTY_NEWLINE ); 
                        }
                    }					
                    
                    if ( ifmifo.interval != IFM_DEF_INTERVAL )
					{
						vty_out ( vty, " sample interval %d     %s", ifmifo.interval, VTY_NEWLINE );
						
					}	
                }

            }
            if ( pifm->ifm.reflector.reflector_flag )
            {
                if ( pifm->ifm.reflector.reflector_flag & IFM_LOOPBACK_INTERNAL )
                {
                    vty_out ( vty, " reflector internal  %s %s", (pifm->ifm.reflector.exchange_mac == 0 ? "exchange-ip" :
						( pifm->ifm.reflector.exchange_ip == 0 ? "exchange-mac" : "exchange-mac exchange-ip")), VTY_NEWLINE );
                }
                if ( pifm->ifm.reflector.reflector_flag & IFM_LOOPBACK_EXTERNEL )
                {
                    vty_out ( vty, " reflector external %s %s", (pifm->ifm.reflector.exchange_mac == 0 ? "exchange-ip" :
						( pifm->ifm.reflector.exchange_ip == 0 ? "exchange-mac" : "exchange-mac exchange-ip")),VTY_NEWLINE );
                }

            }
	
	  if ( pifm->ifm.loop_info.reflector.reflector_flag )
  	 { 
  	 	if ( pifm->ifm.loop_info.reflector.reflector_flag & IFM_LOOPBACK_EXTERNEL)
 		{
 			if ( pifm->ifm.loop_info.set_flag & IFM_LB_SVLAN_SET )
 			{					
 				vty_out ( vty, " reflctor vlan %d %s %s %s", pifm->ifm.loop_info.svlan,
				pifm->ifm.loop_info.reflector.reflector_flag == IFM_LOOPBACK_EXTERNEL ? "external" : "internal",
				(pifm->ifm.loop_info.reflector.exchange_mac == 0 ? "exchange-ip" : ( pifm->ifm.loop_info.reflector.exchange_ip == 0 ? "exchange-mac" : "exchange-mac exchange-ip")),VTY_NEWLINE );
 			}
			if ( pifm->ifm.loop_info.set_flag & IFM_LB_SMAC_SET )
			{
				vty_out ( vty, " reflector smac %02x:%02x:%02x:%02x:%02x:%02x %s %s %s",
				pifm->ifm.loop_info.smac[0], pifm->ifm.loop_info.smac[1],
				pifm->ifm.loop_info.smac[2],pifm->ifm.loop_info.smac[3],
				pifm->ifm.loop_info.smac[4],pifm->ifm.loop_info.smac[5],
				pifm->ifm.loop_info.reflector.reflector_flag == IFM_LOOPBACK_EXTERNEL ? "external" : "internal",
				(pifm->ifm.loop_info.reflector.exchange_mac == 0 ? "exchange-ip" : ( pifm->ifm.loop_info.reflector.exchange_ip == 0 ? "exchange-mac" : "exchange-mac exchange-ip")),VTY_NEWLINE );
			
			}
			if ( pifm->ifm.loop_info.set_flag & IFM_LB_DMAC_SET )
			{
				vty_out ( vty, " reflector dmac %02x:%02x:%02x:%02x:%02x:%02x %s %s %s",
				pifm->ifm.loop_info.dmac[0], pifm->ifm.loop_info.dmac[1],
				pifm->ifm.loop_info.dmac[2],pifm->ifm.loop_info.dmac[3],
				pifm->ifm.loop_info.dmac[4],pifm->ifm.loop_info.dmac[5], 
				pifm->ifm.loop_info.reflector.reflector_flag == IFM_LOOPBACK_EXTERNEL ? "external" : "internal",
				(pifm->ifm.loop_info.reflector.exchange_mac == 0 ? "exchange-ip" : ( pifm->ifm.loop_info.reflector.exchange_ip == 0 ? "exchange-mac" : "exchange-mac exchange-ip")),VTY_NEWLINE );
		
			}
			if ( pifm->ifm.loop_info.set_flag & IFM_LB_SIP_SET )
			{
				inet_ipv4tostr( pifm->ifm.loop_info.sip.addr, ip);
				vty_out ( vty, " reflector sip %s %s %s %s",ip,  
				pifm->ifm.loop_info.reflector.reflector_flag == IFM_LOOPBACK_EXTERNEL ? "external" : "internal",
				(pifm->ifm.loop_info.reflector.exchange_mac == 0 ? "exchange-ip" : ( pifm->ifm.loop_info.reflector.exchange_ip == 0 ? "exchange-mac" : "exchange-mac exchange-ip")),VTY_NEWLINE );
		
			}
			if ( pifm->ifm.loop_info.set_flag & IFM_LB_DIP_SET )
			{	
				inet_ipv4tostr( pifm->ifm.loop_info.dip.addr, ip);
				vty_out ( vty, " reflector dip %s %s %s %s",ip,  
				pifm->ifm.loop_info.reflector.reflector_flag == IFM_LOOPBACK_EXTERNEL ? "external" : "internal",
				(pifm->ifm.loop_info.reflector.exchange_mac == 0 ? "exchange-ip" : ( pifm->ifm.loop_info.reflector.exchange_ip == 0 ? "exchange-mac" : "exchange-mac exchange-ip")),VTY_NEWLINE );
		
			}	  	
 			
 		}
		if ( pifm->ifm.loop_info.reflector.reflector_flag & IFM_LOOPBACK_INTERNAL )
 		{
 			if ( pifm->ifm.loop_info.set_flag & IFM_LB_SVLAN_SET )
 			{
 				vty_out ( vty, " reflector vlan %d %s %s %s", pifm->ifm.loop_info.svlan,
				pifm->ifm.loop_info.reflector.reflector_flag == IFM_LOOPBACK_EXTERNEL ? "external" : "internal",
				(pifm->ifm.loop_info.reflector.exchange_mac == 0 ? "exchange-ip" : ( pifm->ifm.loop_info.reflector.exchange_ip == 0 ? "exchange-mac" : "exchange-mac exchange-ip")),VTY_NEWLINE );
 			}
			if ( pifm->ifm.loop_info.set_flag & IFM_LB_SMAC_SET )
			{
				vty_out ( vty, " reflector smac %02x:%02x:%02x:%02x:%02x:%02x %s %s %s",
				pifm->ifm.loop_info.smac[0], pifm->ifm.loop_info.smac[1],
				pifm->ifm.loop_info.smac[2],pifm->ifm.loop_info.smac[3],
				pifm->ifm.loop_info.smac[4],pifm->ifm.loop_info.smac[5],
				pifm->ifm.loop_info.reflector.reflector_flag == IFM_LOOPBACK_EXTERNEL ? "external" : "internal",
				(pifm->ifm.loop_info.reflector.exchange_mac == 0 ? "exchange-ip" : ( pifm->ifm.loop_info.reflector.exchange_ip == 0 ? "exchange-mac" : "exchange-mac exchange-ip")),VTY_NEWLINE );
			
		}
		if ( pifm->ifm.loop_info.set_flag & IFM_LB_DMAC_SET )
		{
			vty_out ( vty, " reflector dmac %02x:%02x:%02x:%02x:%02x:%02x %s %s %s",
			pifm->ifm.loop_info.dmac[0], pifm->ifm.loop_info.dmac[1],
			pifm->ifm.loop_info.dmac[2],pifm->ifm.loop_info.dmac[3],
			pifm->ifm.loop_info.dmac[4],pifm->ifm.loop_info.dmac[5], 
			pifm->ifm.loop_info.reflector.reflector_flag == IFM_LOOPBACK_EXTERNEL ? "external" : "internal",
			(pifm->ifm.loop_info.reflector.exchange_mac == 0 ? "exchange-ip" : ( pifm->ifm.loop_info.reflector.exchange_ip == 0 ? "exchange-mac" : "exchange-mac exchange-ip")),VTY_NEWLINE );
		
		}
		if ( pifm->ifm.loop_info.set_flag & IFM_LB_SIP_SET )
		{
			inet_ipv4tostr( pifm->ifm.loop_info.sip.addr, ip);
			vty_out ( vty, " reflector sip %s %s %s %s",ip,  
			pifm->ifm.loop_info.reflector.reflector_flag == IFM_LOOPBACK_EXTERNEL ? "external" : "internal",
			(pifm->ifm.loop_info.reflector.exchange_mac == 0 ? "exchange-ip" : ( pifm->ifm.loop_info.reflector.exchange_ip == 0 ? "exchange-mac" : "exchange-mac exchange-ip")),VTY_NEWLINE );
		
		}
		if ( pifm->ifm.loop_info.set_flag & IFM_LB_DIP_SET )
		{	
			inet_ipv4tostr( pifm->ifm.loop_info.dip.addr, ip);
			vty_out ( vty, " reflector dip %s %s %s %s",ip,  
			pifm->ifm.loop_info.reflector.reflector_flag == IFM_LOOPBACK_EXTERNEL ? "external" : "internal",
			(pifm->ifm.loop_info.reflector.exchange_mac == 0 ? "exchange-ip" : ( pifm->ifm.loop_info.reflector.exchange_ip == 0 ? "exchange-mac" : "exchange-mac exchange-ip")),VTY_NEWLINE );
		
		}	  	
  	 }
	 
  	 }	 
	
            break;
	case IFNET_TYPE_XGIGABIT_ETHERNET:
		 if ( IFM_TYPE_IS_TRUNK ( ifmifo.parent ) )
            {
                if ( pifm->port_info.autoneg == IFNET_SPEED_MODE_AUTO )
                {
                    vty_out ( vty, " operation auto%s", VTY_NEWLINE );
                }
                else
                {
                    vty_out ( vty, " operation %s%s", pifm->port_info.duplex == IFNET_DUPLEX_FULL ? "duplex" : "simplex", VTY_NEWLINE );
                   
                }
            }
		 else
		 {
                /*son interface */
                if ( IFM_IS_SUBPORT ( pifm->ifm.ifindex ) )
                {
                    if ( ifmifo.mode != IFM_DEF_MODE )
                    {
                        vty_out ( vty, " mode %s%s", ifmifo.mode == IFNET_MODE_L2 ? "l2" :
                                  ( ifmifo.mode == IFNET_MODE_L3 ? "l3" : "switch" ), VTY_NEWLINE );
                    }

                    if ( ifmifo.mtu != IFM_DEF_MTU )
                    {
                        vty_out ( vty, " mtu %d%s", ifmifo.mtu, VTY_NEWLINE );
                    }

                    if ( ifmifo.statistics != IFNET_STAT_DISABLE )
                    { vty_out ( vty, " statistics %s%s", ifmifo.statistics == IFNET_STAT_ENABLE ? "enable" : "disable", VTY_NEWLINE ); }

                    ifm_encap_config_write ( vty, ifmifo );

                }
                else
                {
                    if ( pifm->port_info.autoneg == IFNET_SPEED_MODE_AUTO )
                    {
                        vty_out ( vty, " operation auto%s", VTY_NEWLINE );
                    }
                    else
                    {
                        vty_out ( vty, " operation %s%s", pifm->port_info.duplex == IFNET_DUPLEX_FULL ? "duplex" : "simplex", VTY_NEWLINE );
                        
                    }

                    // 2018-05-04, by caojt, add IFNET_MODE_PHYSICAL for L1 ethernet port
                    if ( ifmifo.mode != IFM_DEF_MODE && ifmifo.mode != IFNET_MODE_PHYSICAL)
                    {
                        vty_out ( vty, " mode %s%s", ifmifo.mode == IFNET_MODE_L2 ? "l2" :
                                  ( ifmifo.mode == IFNET_MODE_L3 ? "l3" : "switch" ), VTY_NEWLINE );
                    }

                    if ( ifmifo.mtu != IFM_DEF_MTU )
                    {
                        vty_out ( vty, " mtu %d%s", ifmifo.mtu, VTY_NEWLINE );
                    }

                    if ( ifmifo.jumbo != IFM_DEF_JUMBO )
                    {
                        vty_out ( vty, " jumbo-frame %d%s", ifmifo.jumbo, VTY_NEWLINE );
                    }

                    if ( pifm->port_info.flap_period != IFM_DEF_FLAP )
                    { vty_out ( vty, " flapping-control period %d%s", pifm->port_info.flap_period, VTY_NEWLINE ); }
                    if ( ifmifo.sfp_off != IFM_DEF_SFP_OFF )
                    {
                        vty_out ( vty, " sfp-config als off-time %d%s", ifmifo.sfp_off, VTY_NEWLINE );
                    }
                    if ( ifmifo.sfp_on != IFM_DEF_SFP_ON )
                    {
                        vty_out ( vty, " sfp-config als on-time %d%s", ifmifo.sfp_on, VTY_NEWLINE );
                    }
                    if ( ifmifo.sfp_tx != IFM_DEF_SFP_TX )
                    {
                        vty_out ( vty, " sfp-config tx %s%s", ifmifo.sfp_tx == IFM_SFP_TX_DISABLE ? "disable" : "enable", VTY_NEWLINE );
                    }
                    if ( ifmifo.sfp_als != IFM_DEF_SFP_ALS )
                    {
                        vty_out ( vty, " sfp-config als %s%s", ifmifo.sfp_als == IFM_SFP_ALS_ENABLE ? "enable" : "disable", VTY_NEWLINE );
                    }
                    if ( ifmifo.tpid != IFM_DEF_TPID )
                    { 
                        vty_out ( vty, " tpid 0x%0x%s", ifmifo.tpid, VTY_NEWLINE ); 
                    }

                    /*added by liub 2018-4-25 for flow control */
                    if ( ifmifo.flow_ctrl.status != IFM_FLOW_CONTROL_DISABLE) 
                    {
                        if(ifmifo.flow_ctrl.direction == IFM_FLOW_CONTROL_RECEIVE)
                        {
                            vty_out ( vty, "flow-control receive enable %s", VTY_NEWLINE ); 
                        }
                        else if(ifmifo.flow_ctrl.direction == IFM_FLOW_CONTROL_SEND)
                        {
                            vty_out ( vty, "flow-control send enable %s", VTY_NEWLINE ); 
                        }
                        else if(ifmifo.flow_ctrl.direction == IFM_FLOW_CONTROL_RECEIVE_AND_SEND)
                        {
                            vty_out ( vty, "flow-control %s", VTY_NEWLINE ); 
                        }
                    }
                    
                }

            }           
                   
		  if ( pifm->ifm.reflector.reflector_flag )
            {
                if ( pifm->ifm.reflector.reflector_flag & IFM_LOOPBACK_INTERNAL )
                {
                    vty_out ( vty, " reflector internal  %s %s", (pifm->ifm.reflector.exchange_mac == 0 ? "exchange-ip" :
						( pifm->ifm.reflector.exchange_ip == 0 ? "exchange-mac" : "exchange-mac exchange-ip")), VTY_NEWLINE );
                }
                if ( pifm->ifm.reflector.reflector_flag & IFM_LOOPBACK_EXTERNEL )
                {
                    vty_out ( vty, " reflector external %s %s", (pifm->ifm.reflector.exchange_mac == 0 ? "exchange-ip" :
						( pifm->ifm.reflector.exchange_ip == 0 ? "exchange-mac" : "exchange-mac exchange-ip")),VTY_NEWLINE );
                }

            }
	
		if ( pifm->ifm.loop_info.reflector.reflector_flag )
  	 { 
  	 	if ( pifm->ifm.loop_info.reflector.reflector_flag & IFM_LOOPBACK_EXTERNEL)
 		{
 			if ( pifm->ifm.loop_info.set_flag & IFM_LB_SVLAN_SET )
 			{					
 				vty_out ( vty, " reflector vlan %d %s %s %s", pifm->ifm.loop_info.svlan,
				pifm->ifm.loop_info.reflector.reflector_flag == IFM_LOOPBACK_EXTERNEL ? "external" : "internal",
				(pifm->ifm.loop_info.reflector.exchange_mac == 0 ? "exchange-ip" : ( pifm->ifm.loop_info.reflector.exchange_ip == 0 ? "exchange-mac" : "exchange-mac exchange-ip")),VTY_NEWLINE );
			}
			if ( pifm->ifm.loop_info.set_flag & IFM_LB_SMAC_SET )
			{
				vty_out ( vty, " reflector smac %02x:%02x:%02x:%02x:%02x:%02x %s %s %s",
				pifm->ifm.loop_info.smac[0], pifm->ifm.loop_info.smac[1],
				pifm->ifm.loop_info.smac[2],pifm->ifm.loop_info.smac[3],
				pifm->ifm.loop_info.smac[4],pifm->ifm.loop_info.smac[5],
				pifm->ifm.loop_info.reflector.reflector_flag == IFM_LOOPBACK_EXTERNEL ? "external" : "internal",
				(pifm->ifm.loop_info.reflector.exchange_mac == 0 ? "exchange-ip" : ( pifm->ifm.loop_info.reflector.exchange_ip == 0 ? "exchange-mac" : "exchange-mac exchange-ip")),VTY_NEWLINE );
			}
			if ( pifm->ifm.loop_info.set_flag & IFM_LB_DMAC_SET )
			{
				vty_out ( vty, " reflector dmac %02x:%02x:%02x:%02x:%02x:%02x %s %s %s",
				pifm->ifm.loop_info.dmac[0], pifm->ifm.loop_info.dmac[1],
				pifm->ifm.loop_info.dmac[2],pifm->ifm.loop_info.dmac[3],
				pifm->ifm.loop_info.dmac[4],pifm->ifm.loop_info.dmac[5], 
				pifm->ifm.loop_info.reflector.reflector_flag == IFM_LOOPBACK_EXTERNEL ? "external" : "internal",
				(pifm->ifm.loop_info.reflector.exchange_mac == 0 ? "exchange-ip" : ( pifm->ifm.loop_info.reflector.exchange_ip == 0 ? "exchange-mac" : "exchange-mac exchange-ip")),VTY_NEWLINE );
			}
			if ( pifm->ifm.loop_info.set_flag & IFM_LB_SIP_SET )
			{
				inet_ipv4tostr( pifm->ifm.loop_info.sip.addr, ip);
				vty_out ( vty, " reflector sip %s %s %s %s",ip,  
				pifm->ifm.loop_info.reflector.reflector_flag == IFM_LOOPBACK_EXTERNEL ? "external" : "internal",
				(pifm->ifm.loop_info.reflector.exchange_mac == 0 ? "exchange-ip" : ( pifm->ifm.loop_info.reflector.exchange_ip == 0 ? "exchange-mac" : "exchange-mac exchange-ip")),VTY_NEWLINE );
			}
			if ( pifm->ifm.loop_info.set_flag & IFM_LB_DIP_SET )
			{	
				inet_ipv4tostr( pifm->ifm.loop_info.dip.addr, ip);
				vty_out ( vty, " reflector dip %s %s %s %s",ip,  
				pifm->ifm.loop_info.reflector.reflector_flag == IFM_LOOPBACK_EXTERNEL ? "external" : "internal",
				(pifm->ifm.loop_info.reflector.exchange_mac == 0 ? "exchange-ip" : ( pifm->ifm.loop_info.reflector.exchange_ip == 0 ? "exchange-mac" : "exchange-mac exchange-ip")),VTY_NEWLINE );
			}	  	
 			
 		}
		if ( pifm->ifm.loop_info.reflector.reflector_flag & IFM_LOOPBACK_INTERNAL )
 		{
 			if ( pifm->ifm.loop_info.set_flag & IFM_LB_SVLAN_SET )
 			{
 				vty_out ( vty, " reflector vlan %d %s %s %s", pifm->ifm.loop_info.svlan,
				pifm->ifm.loop_info.reflector.reflector_flag == IFM_LOOPBACK_EXTERNEL ? "external" : "internal",
				(pifm->ifm.loop_info.reflector.exchange_mac == 0 ? "exchange-ip" : ( pifm->ifm.loop_info.reflector.exchange_ip == 0 ? "exchange-mac" : "exchange-mac exchange-ip")),VTY_NEWLINE );
 			}
			if ( pifm->ifm.loop_info.set_flag & IFM_LB_SMAC_SET )
			{
				vty_out ( vty, " reflector smac %02x:%02x:%02x:%02x:%02x:%02x %s %s %s",
				pifm->ifm.loop_info.smac[0], pifm->ifm.loop_info.smac[1],
				pifm->ifm.loop_info.smac[2],pifm->ifm.loop_info.smac[3],
				pifm->ifm.loop_info.smac[4],pifm->ifm.loop_info.smac[5],
				pifm->ifm.loop_info.reflector.reflector_flag == IFM_LOOPBACK_EXTERNEL ? "external" : "internal",
				(pifm->ifm.loop_info.reflector.exchange_mac == 0 ? "exchange-ip" : ( pifm->ifm.loop_info.reflector.exchange_ip == 0 ? "exchange-mac" : "exchange-mac exchange-ip")),VTY_NEWLINE );
			
			}
			if ( pifm->ifm.loop_info.set_flag & IFM_LB_DMAC_SET )
			{
				vty_out ( vty, " reflector dmac %02x:%02x:%02x:%02x:%02x:%02x %s %s %s",
				pifm->ifm.loop_info.dmac[0], pifm->ifm.loop_info.dmac[1],
				pifm->ifm.loop_info.dmac[2],pifm->ifm.loop_info.dmac[3],
				pifm->ifm.loop_info.dmac[4],pifm->ifm.loop_info.dmac[5], 
				pifm->ifm.loop_info.reflector.reflector_flag == IFM_LOOPBACK_EXTERNEL ? "external" : "internal",
				(pifm->ifm.loop_info.reflector.exchange_mac == 0 ? "exchange-ip" : ( pifm->ifm.loop_info.reflector.exchange_ip == 0 ? "exchange-mac" : "exchange-mac exchange-ip")),VTY_NEWLINE );
		
			}
			if ( pifm->ifm.loop_info.set_flag & IFM_LB_SIP_SET )
			{
				inet_ipv4tostr( pifm->ifm.loop_info.sip.addr, ip);
				vty_out ( vty, " reflector sip %s %s %s %s",ip,  
				pifm->ifm.loop_info.reflector.reflector_flag == IFM_LOOPBACK_EXTERNEL ? "external" : "internal",
				(pifm->ifm.loop_info.reflector.exchange_mac == 0 ? "exchange-ip" : ( pifm->ifm.loop_info.reflector.exchange_ip == 0 ? "exchange-mac" : "exchange-mac exchange-ip")),VTY_NEWLINE );
		
			}
			if ( pifm->ifm.loop_info.set_flag & IFM_LB_DIP_SET )
			{	
				inet_ipv4tostr( pifm->ifm.loop_info.dip.addr, ip);
				vty_out ( vty, " reflector dip %s %s %s %s",ip,  
				pifm->ifm.loop_info.reflector.reflector_flag == IFM_LOOPBACK_EXTERNEL ? "external" : "internal",
				(pifm->ifm.loop_info.reflector.exchange_mac == 0 ? "exchange-ip" : ( pifm->ifm.loop_info.reflector.exchange_ip == 0 ? "exchange-mac" : "exchange-mac exchange-ip")),VTY_NEWLINE );
		
			}	  			
 		} 	 	
  	 	
  	 }
	  
            break;			
		
        case IFNET_TYPE_TDM:
        case IFNET_TYPE_STM:
            if ( IFM_IS_SUBPORT ( pifm->ifm.ifindex ) )
            {
                if ( ifmifo.statistics != IFNET_STAT_DISABLE )
                { vty_out ( vty, " statistics %s%s", ifmifo.statistics == IFNET_STAT_ENABLE ? "enable" : "disable", VTY_NEWLINE ); }
            }
            break;
        case IFNET_TYPE_TRUNK:
            if ( ifmifo.mode != IFM_DEF_MODE )
            {
                vty_out ( vty, " mode %s%s", ifmifo.mode == IFNET_MODE_L2 ? "l2" :
                          ( ifmifo.mode == IFNET_MODE_L3 ? "l3" : "switch" ), VTY_NEWLINE );
            }
            /*son interface */
            if ( IFM_IS_SUBPORT ( pifm->ifm.ifindex ) )
            {
                if ( ifmifo.mtu != IFM_DEF_MTU )
                {
                    vty_out ( vty, " mtu %d%s", ifmifo.mtu, VTY_NEWLINE );
                }

                if ( ifmifo.statistics != IFNET_STAT_DISABLE )
                { vty_out ( vty, " statistics %s%s", ifmifo.statistics == IFNET_STAT_ENABLE ? "enable" : "disable", VTY_NEWLINE ); }

                ifm_encap_config_write ( vty, ifmifo );
            }
            else
            {
                if ( ifmifo.mtu != IFM_DEF_MTU )
                {
                    vty_out ( vty, " mtu %d%s", ifmifo.mtu, VTY_NEWLINE );
                }

                if ( ifmifo.jumbo != IFM_DEF_JUMBO )
                {
                    vty_out ( vty, " jumbo-frame %d%s", ifmifo.jumbo, VTY_NEWLINE );
                }

                if ( pifm->port_info.flap_period != IFM_DEF_FLAP )
                { vty_out ( vty, " flapping-control period %d%s", pifm->port_info.flap_period, VTY_NEWLINE ); }
                if ( ifmifo.tpid != IFM_DEF_TPID )
                { vty_out ( vty, " tpid 0x%0x%s", ifmifo.tpid, VTY_NEWLINE ); }

            }
            break;
        case IFNET_TYPE_VLANIF:
            if ( ifmifo.mtu != IFM_DEF_MTU )
            { vty_out ( vty, " mtu %d%s", ifmifo.mtu, VTY_NEWLINE ); }

            if ( ifmifo.statistics != IFNET_STAT_DISABLE )
            { vty_out ( vty, " statistics %s%s", ifmifo.statistics == IFNET_STAT_ENABLE ? "enable" : "disable", VTY_NEWLINE ); }

            break;
        case IFNET_TYPE_LOOPBACK:
            if ( ifmifo.mtu != IFM_DEF_MTU )
            { vty_out ( vty, " mtu %d%s", ifmifo.mtu, VTY_NEWLINE ); }

            if ( ifmifo.statistics != IFNET_STAT_DISABLE )
            { vty_out ( vty, " statistics %s%s", ifmifo.statistics == IFNET_STAT_ENABLE ? "enable" : "disable", VTY_NEWLINE ); }
            break;
        case IFNET_TYPE_TUNNEL:
            // if ( ifmifo.statistics != IFNET_STAT_DISABLE )
            //{ vty_out ( vty, " statistics %s%s", ifmifo.statistics == IFNET_STAT_ENABLE ? "enable" : "disable", VTY_NEWLINE ); }
            break;
		case IFNET_TYPE_VCG:
			break;
		case IFNET_TYPE_E1:
			break;	
        default:
            break;
    }

    /* 保存接口上的qos配置 */
    ifm_qos_config_write ( pifm, vty );

    vty_out ( vty, "!%s", VTY_NEWLINE );

}


/*接口配置恢复保存*/
int ifm_config_write ( struct vty *vty )
{
    struct ifm_entry *pifm = NULL;
    struct hash_bucket *pbucket = NULL;
    uint32_t cursor;
    struct listnode *node     = NULL;
    struct listnode *nextnode = NULL;
    int *pdata = NULL;
    struct ifm_entry *tpifm = NULL;

    IFM_ENTRTY_LOOP ( pbucket, cursor )
    {
        pifm = pbucket->data;
        if ( pifm == NULL )
        {
            zlog_err ( "%s[%d] pifm == NULL\n", __FUNCTION__, __LINE__ );
            return 0;
        }

        /*son interface write follow parent interface*/
        if ( ( IFM_TYPE_IS_METHERNET(pifm->ifm.ifindex) && IFM_TYPE_IS_METHERNET(pifm->ifm.parent) )
                || ( IFM_TYPE_IS_TRUNK ( pifm->ifm.ifindex ) && IFM_TYPE_IS_TRUNK ( pifm->ifm.parent ) )
                || ( IFM_TYPE_IS_TDM ( pifm->ifm.ifindex ) && IFM_TYPE_IS_TDM ( pifm->ifm.parent ) )
                || ( IFM_TYPE_IS_STM ( pifm->ifm.ifindex ) && IFM_TYPE_IS_STM ( pifm->ifm.parent ) ) )
        {
            continue;
        }
     	/*判断接口是否正在被devm 隐藏或删除如没有正在被操作则恢复*/
        if ( pifm->devm_flag == 0 )
        {
            /*interface write*/
            ifm_interface_config_write ( vty, pifm );

            /*son interface write*/
            if ( pifm->sonlist )
            {
                for ( ALL_LIST_ELEMENTS ( pifm->sonlist, node, nextnode, pdata ) )
                {
                    tpifm = ( struct ifm_entry * ) pdata;
                    if ( tpifm == NULL )
                    {
                        zlog_err ( "%s[%d] pifm == NULL\n", __FUNCTION__, __LINE__ );
                        return 0;
                    }

                    ifm_interface_config_write ( vty, tpifm );
                }
            }
        }
    }
	return 0;
}

/*接口命令行注册*/

void
ifm_cli_init ( void )
{
    install_node ( &physical_if_node, ifm_config_write );
    install_node ( &physical_subif_node, NULL );
    install_node ( &tdm_if_node, NULL );
    install_node ( &tdm_subif_node, NULL );
    install_node ( &stm_if_node, NULL );
    install_node ( &stm_subif_node, NULL );
    install_node ( &loopback_if_node, NULL );
    install_node ( &tunnel_if_node, NULL );
    install_node ( &trunk_if_node, NULL );
    install_node ( &trunk_subif_node, NULL );
    install_node ( &vlanif_node, NULL );
    install_node ( &clockif_node, NULL );
	install_node ( &e1if_node, NULL );

    install_default ( PHYSICAL_IF_NODE );
    install_default ( PHYSICAL_SUBIF_NODE );
    install_default ( TDM_IF_NODE );
    install_default ( TDM_SUBIF_NODE );
    install_default ( STM_IF_NODE );
    install_default ( STM_SUBIF_NODE );
    install_default ( LOOPBACK_IF_NODE );
    install_default ( TUNNEL_IF_NODE );
    install_default ( TRUNK_IF_NODE );
    install_default ( TRUNK_SUBIF_NODE );
    install_default ( VLANIF_NODE );
    install_default ( CLOCK_NODE );
	install_default ( E1_IF_NODE );

    install_element ( CONFIG_NODE, &physical_ethernet_if_cmd, CMD_SYNC );
	install_element ( CONFIG_NODE, &physical_gigabit_ethernet_if_cmd, CMD_SYNC );
	install_element ( CONFIG_NODE, &physical_xgigabit_ethernet_if_cmd, CMD_SYNC );
    install_element ( CONFIG_NODE, &physical_tdm_if_cmd, CMD_SYNC );
    install_element ( CONFIG_NODE, &physical_stm_if_cmd, CMD_SYNC );
    install_element ( CONFIG_NODE, &loopback_if_cmd, CMD_SYNC );
    install_element ( CONFIG_NODE, &tunnel_if_cmd, CMD_SYNC );
    install_element ( CONFIG_NODE, &trunk_if_cmd, CMD_SYNC );
    install_element ( CONFIG_NODE, &vlanif_if_cmd, CMD_SYNC );
    install_element ( CONFIG_NODE, &clock_if_cmd, CMD_SYNC );
	install_element ( CONFIG_NODE, &e1_if_cmd, CMD_SYNC);


    install_element ( CONFIG_NODE, &show_ifm_cmd, CMD_LOCAL );
    install_element ( CONFIG_NODE, &show_ifm_type_cmd, CMD_LOCAL );
    install_element ( CONFIG_NODE, &show_ifm_config_cmd, CMD_LOCAL );
    install_element ( CONFIG_NODE, &show_ifm_type_config_cmd, CMD_LOCAL );

    install_element ( CONFIG_NODE, &clear_if_statistics_cmd, CMD_SYNC );
    install_element ( CONFIG_NODE, &no_if_cmd, CMD_SYNC );
	
    install_element ( PHYSICAL_IF_NODE, &if_shutdown_cmd, CMD_SYNC );
    install_element ( PHYSICAL_IF_NODE, &if_no_shutdown_cmd, CMD_SYNC );
    install_element ( PHYSICAL_IF_NODE, &if_mtu_cmd, CMD_SYNC );
	install_element ( PHYSICAL_IF_NODE, &no_if_mtu_cmd, CMD_SYNC );
    install_element ( PHYSICAL_IF_NODE, &if_alias_cmd, CMD_SYNC );
    install_element ( PHYSICAL_IF_NODE, &no_if_alias_cmd, CMD_SYNC );
    install_element ( PHYSICAL_IF_NODE, &clear_statistics_cmd, CMD_SYNC );
    install_element ( PHYSICAL_IF_NODE, &if_jumboframe_cmd, CMD_SYNC );
	install_element ( PHYSICAL_IF_NODE, &no_if_jumboframe_cmd, CMD_SYNC );
    install_element ( PHYSICAL_IF_NODE, &if_flapping_control_cmd, CMD_SYNC );
    install_element ( PHYSICAL_IF_NODE, &if_duplex_cmd, CMD_SYNC );
	install_element ( PHYSICAL_IF_NODE, &if_combo_cmd,CMD_SYNC);
    install_element ( PHYSICAL_IF_NODE, &if_tpid_cmd, CMD_SYNC );
    install_element ( PHYSICAL_IF_NODE, &if_no_tpid_cmd, CMD_SYNC );
    //install_element ( PHYSICAL_IF_NODE, &if_mac_cmd, CMD_SYNC );
    install_element ( PHYSICAL_IF_NODE, &if_speed_cmd, CMD_SYNC );
    install_element ( PHYSICAL_IF_NODE, &if_mode_change_cmd, CMD_SYNC );
    install_element ( PHYSICAL_IF_NODE, &trunk_member_port_set_cmd, CMD_SYNC );
    install_element ( PHYSICAL_IF_NODE, &no_trunk_member_port_set_cmd, CMD_SYNC );
    install_element ( PHYSICAL_IF_NODE, &reflector_cmd, CMD_SYNC );
    install_element ( PHYSICAL_IF_NODE, &no_reflector_cmd, CMD_SYNC );
    install_element ( PHYSICAL_IF_NODE, &sla_interface_session_enable_cmd, CMD_SYNC );
    install_element ( PHYSICAL_IF_NODE, &no_sla_interface_session_enable_cmd, CMD_SYNC );

    install_element ( PHYSICAL_IF_NODE, &sfp_config_als_off_time_cmd, CMD_SYNC );
    install_element ( PHYSICAL_IF_NODE, &sfp_config_als_on_time_cmd, CMD_SYNC );
    install_element ( PHYSICAL_IF_NODE, &sfp_config_als_cmd, CMD_SYNC );
    install_element ( PHYSICAL_IF_NODE, &sfp_config_tx_cmd, CMD_SYNC );
    install_element ( PHYSICAL_IF_NODE, &reflector_vlan_cmd, CMD_SYNC );
    install_element ( PHYSICAL_IF_NODE, &reflector_mac_cmd, CMD_SYNC );
    install_element ( PHYSICAL_IF_NODE, &reflector_ip_cmd, CMD_SYNC );
	install_element ( PHYSICAL_IF_NODE, &sfp_config_als_time_cmd, CMD_SYNC );		
		
	install_element ( PHYSICAL_IF_NODE, &interface_sample_interval_cmd, CMD_SYNC );
    install_element ( PHYSICAL_IF_NODE, &if_phy_mode_cmd, CMD_SYNC );

    
    install_element ( PHYSICAL_SUBIF_NODE, &if_shutdown_cmd, CMD_SYNC );
    install_element ( PHYSICAL_SUBIF_NODE, &if_no_shutdown_cmd, CMD_SYNC );
    install_element ( PHYSICAL_SUBIF_NODE, &if_mtu_cmd, CMD_SYNC );
	install_element ( PHYSICAL_SUBIF_NODE, &no_if_mtu_cmd, CMD_SYNC );
    install_element ( PHYSICAL_SUBIF_NODE, &if_alias_cmd, CMD_SYNC );
    install_element ( PHYSICAL_SUBIF_NODE, &no_if_alias_cmd, CMD_SYNC );
    install_element ( PHYSICAL_SUBIF_NODE, &statistics_enable_cmd, CMD_SYNC );
    install_element ( PHYSICAL_SUBIF_NODE, &no_statistics_enable_cmd, CMD_SYNC );
    install_element ( PHYSICAL_SUBIF_NODE, &clear_statistics_cmd, CMD_SYNC );
    install_element ( PHYSICAL_SUBIF_NODE, &subif_encapsulate_untag_cmd, CMD_SYNC );
    install_element ( PHYSICAL_SUBIF_NODE, &subif_encapsulate_dot1q_cmd, CMD_SYNC );
    install_element ( PHYSICAL_SUBIF_NODE, &subif_encapsulate_qinq_cmd, CMD_SYNC );
    install_element ( PHYSICAL_SUBIF_NODE, &if_mode_change_cmd, CMD_SYNC );
    install_element ( PHYSICAL_SUBIF_NODE, &subif_encapsulate_untag_add_cmd, CMD_SYNC );
    install_element ( PHYSICAL_SUBIF_NODE, &subif_encapsulate_dot1q_add_cmd, CMD_SYNC );
    install_element ( PHYSICAL_SUBIF_NODE, &subif_encapsulate_dot1q_translate_single_cmd, CMD_SYNC );
    install_element ( PHYSICAL_SUBIF_NODE, &subif_encapsulate_dot1q_translate_double_cmd, CMD_SYNC );
    install_element ( PHYSICAL_SUBIF_NODE, &subif_encapsulate_qinq_translate_single_cmd, CMD_SYNC );
    install_element ( PHYSICAL_SUBIF_NODE, &subif_encapsulate_qinq_translate_double_cmd, CMD_SYNC );
    install_element ( PHYSICAL_SUBIF_NODE, &subif_encapsulate_qinq_translate_outer_cmd, CMD_SYNC );
    install_element ( PHYSICAL_SUBIF_NODE, &subif_encapsulate_qinq_translate_delete_cmd, CMD_SYNC );
    install_element ( PHYSICAL_SUBIF_NODE, &no_subif_encapsulate_cmd, CMD_SYNC );
    install_element ( PHYSICAL_SUBIF_NODE, &reflector_cmd, CMD_SYNC );
    install_element ( PHYSICAL_SUBIF_NODE,&no_reflector_cmd, CMD_SYNC );
    install_element ( PHYSICAL_SUBIF_NODE, &sla_interface_session_enable_cmd, CMD_SYNC );
    install_element ( PHYSICAL_SUBIF_NODE, &no_sla_interface_session_enable_cmd, CMD_SYNC );

    install_element ( TDM_IF_NODE, &if_shutdown_cmd, CMD_SYNC );
    install_element ( TDM_IF_NODE, &if_no_shutdown_cmd, CMD_SYNC );
    install_element ( TDM_IF_NODE, &if_alias_cmd, CMD_SYNC );
    install_element ( TDM_IF_NODE, &no_if_alias_cmd, CMD_SYNC );
    install_element ( TDM_IF_NODE, &clear_statistics_cmd, CMD_SYNC );
    install_element ( TDM_IF_NODE, &ces_mode_cmd, CMD_SYNC );
    install_element ( TDM_IF_NODE, &no_ces_mode_cmd, CMD_SYNC );

    install_element ( TDM_SUBIF_NODE, &if_shutdown_cmd, CMD_SYNC );
    install_element ( TDM_SUBIF_NODE, &if_no_shutdown_cmd, CMD_SYNC );
    install_element ( TDM_SUBIF_NODE, &if_alias_cmd, CMD_SYNC );
    install_element ( TDM_SUBIF_NODE, &no_if_alias_cmd, CMD_SYNC );
    install_element ( TDM_SUBIF_NODE, &clear_statistics_cmd, CMD_SYNC );
    install_element ( TDM_SUBIF_NODE, &statistics_enable_cmd, CMD_SYNC );
    install_element ( TDM_SUBIF_NODE, &no_statistics_enable_cmd, CMD_SYNC );

	install_element ( STM_IF_NODE, &sfp_config_als_off_time_cmd, CMD_SYNC );
    install_element ( STM_IF_NODE, &sfp_config_als_on_time_cmd, CMD_SYNC );
    install_element ( STM_IF_NODE, &sfp_config_als_cmd, CMD_SYNC );
    install_element ( STM_IF_NODE, &sfp_config_tx_cmd, CMD_SYNC );
	install_element ( STM_IF_NODE, &sfp_config_als_time_cmd, CMD_SYNC );
    install_element ( STM_IF_NODE, &clear_statistics_cmd, CMD_SYNC );

    install_element ( LOOPBACK_IF_NODE, &if_mtu_cmd, CMD_SYNC );
	install_element ( LOOPBACK_IF_NODE, &no_if_mtu_cmd, CMD_SYNC );
    install_element ( LOOPBACK_IF_NODE, &if_alias_cmd, CMD_SYNC );
    install_element ( LOOPBACK_IF_NODE, &no_if_alias_cmd, CMD_SYNC );
    install_element ( LOOPBACK_IF_NODE, &statistics_enable_cmd, CMD_SYNC );
    install_element ( LOOPBACK_IF_NODE, &no_statistics_enable_cmd, CMD_SYNC );
    install_element ( LOOPBACK_IF_NODE, &clear_statistics_cmd, CMD_SYNC );

    install_element ( TUNNEL_IF_NODE, &if_shutdown_cmd, CMD_SYNC );
    install_element ( TUNNEL_IF_NODE, &if_no_shutdown_cmd, CMD_SYNC );
    install_element ( TUNNEL_IF_NODE, &if_alias_cmd, CMD_SYNC );
    install_element ( TUNNEL_IF_NODE, &no_if_alias_cmd, CMD_SYNC );
    install_element ( TUNNEL_IF_NODE, &statistics_enable_cmd, CMD_SYNC );
    install_element ( TUNNEL_IF_NODE, &no_statistics_enable_cmd, CMD_SYNC );
    install_element ( TUNNEL_IF_NODE, &clear_statistics_cmd, CMD_SYNC );

    install_element ( TRUNK_IF_NODE, &if_mtu_cmd, CMD_SYNC );
    install_element ( TRUNK_IF_NODE, &no_if_mtu_cmd, CMD_SYNC );
    install_element ( TRUNK_IF_NODE, &if_alias_cmd, CMD_SYNC );
    install_element ( TRUNK_IF_NODE, &no_if_alias_cmd, CMD_SYNC );
    install_element ( TRUNK_IF_NODE, &clear_statistics_cmd, CMD_SYNC );
    install_element ( TRUNK_IF_NODE, &if_jumboframe_cmd, CMD_SYNC );
	install_element ( TRUNK_IF_NODE, &no_if_jumboframe_cmd, CMD_SYNC );
    install_element ( TRUNK_IF_NODE, &if_tpid_cmd, CMD_SYNC );
    install_element ( TRUNK_IF_NODE, &if_no_tpid_cmd, CMD_SYNC );
    install_element ( TRUNK_IF_NODE, &if_mode_change_cmd, CMD_SYNC );

    install_element ( TRUNK_SUBIF_NODE, &if_shutdown_cmd, CMD_SYNC );
    install_element ( TRUNK_SUBIF_NODE, &if_no_shutdown_cmd, CMD_SYNC );
    install_element ( TRUNK_SUBIF_NODE, &if_mtu_cmd, CMD_SYNC );
	install_element ( TRUNK_SUBIF_NODE, &no_if_mtu_cmd, CMD_SYNC );
    install_element ( TRUNK_SUBIF_NODE, &if_alias_cmd, CMD_SYNC );
    install_element ( TRUNK_SUBIF_NODE, &no_if_alias_cmd, CMD_SYNC );
    install_element ( TRUNK_SUBIF_NODE, &statistics_enable_cmd, CMD_SYNC );
    install_element ( TRUNK_SUBIF_NODE, &no_statistics_enable_cmd, CMD_SYNC );
    install_element ( TRUNK_SUBIF_NODE, &clear_statistics_cmd, CMD_SYNC );
    install_element ( TRUNK_SUBIF_NODE, &subif_encapsulate_untag_cmd, CMD_SYNC );
    install_element ( TRUNK_SUBIF_NODE, &subif_encapsulate_dot1q_cmd, CMD_SYNC );
    install_element ( TRUNK_SUBIF_NODE, &subif_encapsulate_qinq_cmd, CMD_SYNC );
    install_element ( TRUNK_SUBIF_NODE, &if_mode_change_cmd, CMD_SYNC );
    install_element ( TRUNK_SUBIF_NODE, &subif_encapsulate_untag_add_cmd, CMD_SYNC );
    install_element ( TRUNK_SUBIF_NODE, &subif_encapsulate_dot1q_add_cmd, CMD_SYNC );
    install_element ( TRUNK_SUBIF_NODE, &subif_encapsulate_dot1q_translate_single_cmd, CMD_SYNC );
    install_element ( TRUNK_SUBIF_NODE, &subif_encapsulate_dot1q_translate_double_cmd, CMD_SYNC );
    install_element ( TRUNK_SUBIF_NODE, &subif_encapsulate_qinq_translate_single_cmd, CMD_SYNC );
    install_element ( TRUNK_SUBIF_NODE, &subif_encapsulate_qinq_translate_double_cmd, CMD_SYNC );
    install_element ( TRUNK_SUBIF_NODE, &subif_encapsulate_qinq_translate_outer_cmd, CMD_SYNC );
    install_element ( TRUNK_SUBIF_NODE, &subif_encapsulate_qinq_translate_delete_cmd, CMD_SYNC );
    install_element ( TRUNK_SUBIF_NODE, &no_subif_encapsulate_cmd, CMD_SYNC );

    install_element ( VLANIF_NODE, &if_mtu_cmd, CMD_SYNC );
	install_element ( VLANIF_NODE, &no_if_mtu_cmd, CMD_SYNC );
    install_element ( VLANIF_NODE, &if_alias_cmd, CMD_SYNC );
    install_element ( VLANIF_NODE, &no_if_alias_cmd, CMD_SYNC );
    install_element ( VLANIF_NODE, &statistics_enable_cmd, CMD_SYNC );
    install_element ( VLANIF_NODE, &no_statistics_enable_cmd, CMD_SYNC );
    install_element ( VLANIF_NODE, &clear_statistics_cmd, CMD_SYNC );

    install_element ( CLOCK_NODE, &if_alias_cmd, CMD_SYNC );
    install_element ( CLOCK_NODE, &no_if_alias_cmd, CMD_SYNC );

    install_element ( E1_IF_NODE, &if_alias_cmd, CMD_SYNC );
    install_element ( E1_IF_NODE, &no_if_alias_cmd, CMD_SYNC );

    install_element ( CONFIG_NODE, &debug_show_msg_cmd, CMD_LOCAL );
    install_element ( CONFIG_NODE, &debug_show_interface_name_cmd, CMD_LOCAL );
	install_element ( PHYSICAL_IF_NODE, &debug_down_flapping_control_cmd, CMD_SYNC );

    /*start: added by liub 2018-4-23 for flow control*/
    install_element ( PHYSICAL_IF_NODE, &flow_control_cmd, CMD_SYNC);
    install_element ( PHYSICAL_IF_NODE, &undo_flow_control_cmd, CMD_SYNC);
    install_element ( PHYSICAL_IF_NODE, &flow_control_receive_cmd, CMD_SYNC);
    install_element ( PHYSICAL_IF_NODE, &flow_control_send_cmd, CMD_SYNC);
    install_element ( CONFIG_NODE, &show_flow_control_cmd, CMD_LOCAL);
	/*end: added by liub 2018-4-23 for flow control*/
	install_element(CONFIG_NODE, &ifm_show_timerinfo_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &ifm_debug_fun_cmd,CMD_LOCAL);
	install_element (CONFIG_NODE, &show_ifm_debug_cmd,CMD_LOCAL);
}

