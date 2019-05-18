#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lib/types.h>
#include <lib/command.h>
#include <lib/ether.h>
#include <lib/vty.h>
#include <lib/module_id.h>
#include <lib/msg_ipc_n.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/prefix.h>
#include <lib/inet_ip.h>
#include <lib/ifm_common.h>
#include <lib/errcode.h>
#include <lib/mpls_common.h>
#include <lib/log.h>

#include "arp.h"
#include "arp_cmd.h"
#include "arp_static.h"
#include "route_if.h"
#include "route_main.h"


uint16_t slot_numlimit[8]={ARP_NUM_MAX_DEF};



/*判断-之间有多少各字符*/
#define ARP_GET_ONE_COLONOFMAC(s,e) do{\
		int  __len = (e) - (s) + 1;\
		if(__len > 4)\
			return (FALSE);\
}while(0)

/*判断是否时合法字符*/
#define ARP_CHAR_IS_MAC_DIGITAL(c) ((((c) >= '0') && ((c) <= '9')) || \
								(((c) >= 'a') && ((c) <= 'f')) || \
								(((c) >= 'A') && ((c) <= 'F')))


#define ARP_NOT_MAC_DIGITAL_CHK(__pc) do{\
		if((__pc == NULL) || !ARP_CHAR_IS_MAC_DIGITAL(*(__pc))) return (FALSE);\
}while(0)

static int8_t arp_valid_mac(uint8_t *str)
{
    int8_t  hyphen     = 0;
	int8_t  legal_len  = 0;
	int8_t  actual_len = 0;
	uint8_t *prev = NULL;
	uint8_t *smac = NULL;
	uint8_t *str_tmp = str;

	if(str == NULL)
	{
        return ERRNO_FAIL;
    }

	legal_len = strlen("FFFF-FFFF-FFFF");
	actual_len = strlen((const char*)str_tmp);

	if (actual_len != legal_len)
	{
        return ERRNO_FAIL;
	}

	if(!strcmp((const char*)str,"0000-0000-0000"))
	{
		return ERRNO_FAIL;
	}

	while (1)
	{
		if (ARP_CHAR_IS_MAC_DIGITAL(*str_tmp))
		{
			if (smac == NULL)
			{
				smac = str_tmp;
			}
		}
		else if (*str_tmp == '-')
		{
			ARP_NOT_MAC_DIGITAL_CHK(prev);
			ARP_GET_ONE_COLONOFMAC(smac, str_tmp-1);

            smac = NULL;

			hyphen++;
			if (hyphen > 2)
			{
				return ERRNO_FAIL;
			}
		}
		else if (*str_tmp == '\0')
		{
			if (hyphen != 2)
			{
				return ERRNO_FAIL;
			}

			ARP_NOT_MAC_DIGITAL_CHK(prev);
			ARP_GET_ONE_COLONOFMAC(smac, str_tmp-1);

			break;
		}
		else
		{
			return ERRNO_FAIL;
		}

		prev = str_tmp;
		str_tmp = str_tmp + 1;
	}

    return ERRNO_SUCCESS;
}

#define ARP_HEX_TO_DIGIT(x)  (((x) >= 'A') ? ((x) - 'A' + 10) : ((x) - '0'))
/*string format H-H-H convert to H-H-H*/
static uint8_t *arp_ether_string_to_mac (uint8_t *str, uint8_t *mac)
{      
    int8_t i = 0, j = 0, c = 0;
    uint8_t b1 = 0, b2 = 0;

    for (i = 0; i < 6; i++)
    {
        c = str[j++];
        if (c >= 'a' && c <= 'f')
            c &= 0x0DF;           /* Force to Uppercase. */
        b1 = ARP_HEX_TO_DIGIT(c);
        
        c = str[j++];
        if (c >= 'a' && c <= 'f')
            c &= 0x0DF;           /* Force to Uppercase. */
        b2 = ARP_HEX_TO_DIGIT(c);
        mac[i] = b1 * 16 + b2;

        if (1 == i%2)
            j++;                    /* Skip a hyphen. */
    }

    return mac;
}



/*h3c_cmd add short static arp entry*/
DEFUN(configure_h3c_short_static_arp,
	configure_h3c_short_static_arp_cmd,
	"arp static X.X.X.X H-H-H {vpn-instance <1-128>}",
	"Address Resolution Protocol (ARP) module\n"
	"Static ARP entry\n"
	"Specify the IP address\n"
	"MAC address\n"
	"Specify the Vpn-instance\n"
	"STRING<1-128>\n") 
{
    uint32_t ipaddr = 0;
    uint16_t vpnid  = 0;
    uint8_t  mac[6] = {0};
    int8_t   ret    = 0;
    struct arp_entry sarp;
    struct l3vpn_entry *l3vpn = NULL;

    memset(&sarp, 0, sizeof(struct arp_entry));
    ipaddr = inet_strtoipv4((char *)argv[0]);
    if(TRUE != inet_valid_ipv4(ipaddr))
    {
        vty_error_out(vty,"Invalid IP address.%s",VTY_NEWLINE);
        return CMD_WARNING;
    }
    
	ret = arp_valid_mac((uint8_t *)argv[1]);
	if (ERRNO_FAIL == ret)
	{
		vty_error_out(vty, "Wrong parameter found at mac position.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
   
    arp_ether_string_to_mac((uint8_t *)argv[1], mac);
    if (!ether_is_broadcast_mac(mac) || !ether_is_muticast_mac(mac))
    {
        vty_error_out(vty,"Invalid MAC address.%s",VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    if (NULL != argv[2])
    {
        vpnid = (uint16_t)atoi((char *)argv[2]);
        if( 0 == mpls_com_get_l3vpn_instance2(vpnid, MODULE_ID_ROUTE))
        {
            vty_error_out(vty, "VPN-Instance does not exist.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    /*set arp entry*/
    sarp.key.ipaddr = ipaddr;
    sarp.key.vpnid  = vpnid;
    sarp.status = ARP_STATUS_STATIC;
    memcpy(sarp.mac, mac, MAC_LEN);
    /*find samesubnet ip of outif*/
    if (ERRNO_FAIL == arp_static_get_outif(&sarp))
    {
        vty_warning_out(vty,"Add static arp outif resolve fail!Just save config!%s",VTY_NEWLINE);
        vty->config_read_flag = 1;/*get ourif error.just save config!*/
    }

    ret = arp_static_add(&sarp, vty->config_read_flag);
	if(ret == ARP_ENTRY_IS_NULL)
	{
		vty_error_out(vty, "This is impossiable.%s", VTY_NEWLINE);
        return CMD_WARNING;
	}
	else if((ret == ARP_OVER_STATIC_NUM) || (ret == ARP_HASH_BUCKET_FULL))
	{
		vty_error_out(vty, "Maximum number of static ARP entries already reached.%s", VTY_NEWLINE);
        return CMD_WARNING;
	}
	else if (ret == ERRNO_IPC)
	{
		vty_error_out(vty, "Static arp add notify ftm add error!. %s", VTY_NEWLINE);
        return CMD_WARNING;
	}

    return CMD_SUCCESS;    
}


/*添加vlanif接口的静态arp表项*/
DEFUN(configure_h3c_long_static_arp,
	configure_h3c_long_static_arp_cmd,
	"arp static X.X.X.X H-H-H <1-4094> gigabitethernet USP {vpn-instance <1-128>}",
	"Address Resolution Protocol (ARP) module\n"
	"Static ARP entry\n"
	"Specify the IP address\n"
	"MAC address\n"
    "INTEGER<1-4094>\n"
    "GigabitEthernet interface\n"
	CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
	"Specify the Vpn-instance\n"
	"STRING<1-128>\n")
{
    uint32_t ipaddr = 0;
    uint16_t vpnid  = 0;
    uint8_t  mac[6] = {0};
    int8_t   ret    = 0;
    uint32_t ifindex = 0;
    uint32_t port = 0;
    int8_t downto_ftm_flag = 0;
    struct arp_entry sarp;
    struct l3vpn_entry *l3vpn = NULL;
    struct route_if *rvlanif = NULL;
    
    downto_ftm_flag = vty->config_read_flag;
    memset(&sarp, 0, sizeof(struct arp_entry));
    ipaddr = inet_strtoipv4((char *)argv[0]);
    if(TRUE != inet_valid_ipv4(ipaddr))
    {
        vty_error_out(vty,"Invalid IP address.%s",VTY_NEWLINE);
        return CMD_WARNING;
    }
    
	ret = arp_valid_mac((uint8_t *)argv[1]);
	if (ERRNO_FAIL == ret)
	{
		vty_error_out(vty, "Wrong parameter found at mac position.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
    arp_ether_string_to_mac((uint8_t *)argv[1], mac);
    if (!ether_is_broadcast_mac(mac) || !ether_is_muticast_mac(mac))
    {
        vty_error_out(vty,"Invalid MAC address.%s",VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*check vlanif exist.if not,add static arp error*/
    ifindex = ifm_get_ifindex_by_name ("vlanif", (char *)argv[2]);
    rvlanif = route_if_lookup(ifindex);
    if((ifindex == 0) || (NULL == rvlanif))
    {
        vty_error_out(vty,"Please create the corresponding VLAN interface first.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	port = ifm_get_ifindex_by_name("gigabitethernet", (char *)argv[3]);
	if(port == 0)
	{
		vty_error_out(vty, "Please check out interface format.%s", VTY_NEWLINE);
		return CMD_WARNING;	
    }
    
    if (NULL != argv[4])
    {
        vpnid = (uint16_t)atoi((char *)argv[4]);
        if( 0 == mpls_com_get_l3vpn_instance2(vpnid, MODULE_ID_ROUTE))
        {
            vty_error_out(vty, "VPN-Instance does not exist.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    
    /*long static arp:if vlanif and member port exist download to ftm */
    downto_ftm_flag = 0;

    /*set arp entry*/
    sarp.key.ipaddr = ipaddr;
    sarp.key.vpnid  = vpnid;
    sarp.status = ARP_STATUS_STATIC;
    memcpy(sarp.mac, mac, MAC_LEN);
    sarp.ifindex = ifindex;
    sarp.port = port;

    ret = arp_static_add(&sarp, downto_ftm_flag);
	if(ret == ARP_ENTRY_IS_NULL)
	{
		vty_error_out(vty, "This is impossiable.%s", VTY_NEWLINE);
        return CMD_WARNING;
	}
	else if((ret == ARP_OVER_STATIC_NUM) || (ret == ARP_HASH_BUCKET_FULL))
	{
		vty_error_out(vty, "Maximum number of static ARP entries already reached.%s", VTY_NEWLINE);
        return CMD_WARNING;
	}
	else if (ret == ERRNO_IPC)
	{
		vty_error_out(vty, "Static arp add notify ftm add error!. %s", VTY_NEWLINE);
        return CMD_WARNING;
	}

    return CMD_SUCCESS;    
}


DEFUN(h3c_undo_arp,
    h3c_undo_arp_cmd,
    "undo arp X.X.X.X {<1-128>}",
    "Cancel current setting\n"
    "Address Resolution Protocol (ARP) module\n"
    "Specify the IP address\n"
    "Vpn-instance name\n"
    )
{
    uint32_t ipaddr = 0;
    uint16_t vpnid = 0;
    int8_t ret = 0;
    struct arp_entry arp;
    struct l3vpn_entry *l3vpn = NULL;
    
    ipaddr = inet_strtoipv4((char *)argv[0]);
    if(TRUE != inet_valid_ipv4(ipaddr))
    {
        vty_error_out(vty,"Invalid IP address.%s",VTY_NEWLINE);
        return CMD_WARNING;
    }
  
    if (NULL != argv[1])
    {
        vpnid = (uint16_t)atoi((char *)argv[1]);
        if( 0 == mpls_com_get_l3vpn_instance2(vpnid, MODULE_ID_ROUTE))
        {
            vty_error_out(vty, "VPN-Instance does not exist.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    if (NULL != arp_static_lookup(ipaddr, vpnid))
    {
        arp_static_delete(ipaddr, vpnid);
    }
    else
    {
        memset(&arp, 0, sizeof(struct arp_entry));
        ret = ipc_send_msg_n2(&arp, sizeof(struct arp_entry), 1, MODULE_ID_FTM,MODULE_ID_ARP,
		    							IPC_TYPE_ARP, 0, IPC_OPCODE_DELETE, 0);
        if (0 > ret)
        {
            vty_warning_out(vty,"Route module notify ftm delete dynamic arp error.%s",VTY_NEWLINE);    
        } 
    }

    return CMD_SUCCESS;
}


DEFUN(h3c_reset_arp,
      h3c_reset_arp_cmd,
      "reset arp ( all | dynamic | multiport | static )",
      "Reset operation\n"
      "Address Resolution Protocol (ARP) module\n"
      "Reset all ARP entry\n"
      "Reset dynamic ARP entry\n"
      "Specify the interface\n"
      "Clear multiport ARP entries\n"
      "Reset static ARP entry\n"    
    )
{
    uint32_t backup_numlimit = 0;
    if (!strcmp(argv[0],"all"))
    {
        arp_static_delete_all();
        ROUTE_ARP_DELETE_ALL_DYNAMIC();
    }
    else if (!strcmp(argv[0],"dynamic"))
    {
        ROUTE_ARP_DELETE_ALL_DYNAMIC();
    }
    else if (!strcmp(argv[0],"multiport"))
    {

    }
    else if (!strcmp(argv[0],"static"))
    { 
        arp_static_delete_all();
    }

    return CMD_SUCCESS;
}

DEFUN(h3c_reset_intf,
      h3c_reset_intf_cmd,
      "reset arp interface gigabitethernet USP",
      "Reset operation\n"
      "Address Resolution Protocol (ARP) module\n"
      "Specify the interface\n"
	  CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
    )
{
    struct ifm_arp *arpif_ctrl = NULL;
    struct route_if *rif = NULL;
    uint32_t ifindex = 0;
    uint32_t numlimit_backup = 0;
    
    ifindex = ifm_get_ifindex_by_name("gigabitethernet", (char *)argv[0]);
    rif = route_if_lookup(ifindex);
    if (!rif)
    {
        return CMD_SUCCESS;
    }

    arpif_ctrl = &(rif->arp);

    /*del intf's dynamic arp*/
    numlimit_backup = arpif_ctrl->arp_num_max;
    arpif_ctrl->arp_num_max = 0; 
    ipc_send_msg_n2(arpif_ctrl, sizeof(struct ifm_arp), 1, MODULE_ID_FTM, MODULE_ID_ARP,IPC_TYPE_ARPIF, ARP_INFO_NUM_LIMIT, IPC_OPCODE_UPDATE, ifindex);
    arpif_ctrl->arp_num_max = numlimit_backup;
    ipc_send_msg_n2(arpif_ctrl, sizeof(struct ifm_arp), 1, MODULE_ID_FTM, MODULE_ID_ARP,IPC_TYPE_ARPIF, ARP_INFO_NUM_LIMIT, IPC_OPCODE_UPDATE, ifindex);

    /*del intf's static arp*/
    arp_static_delete_by_interface(ifindex); 

    return CMD_SUCCESS;
}

DEFUN(h3c_reset_slot,
      h3c_reset_slot_cmd,
      "reset arp slot <1-8>",
      "Reset operation\n"
      "Address Resolution Protocol (ARP) module\n"
      "Specify the slot number\n"
      "Slot number\n"
     )
{
    uint8_t slot_num = 0;

    slot_num = (uint8_t)atoi(argv[0]);
    
    arp_static_delete_by_slot(slot_num);
    ipc_send_msg_n2(&slot_num, sizeof(slot_num), 1, MODULE_ID_FTM, MODULE_ID_ARP, IPC_TYPE_ARP, ARP_INFO_RESET_SLOT, 0, 0);

    return CMD_SUCCESS;
}


DEFUN(h3c_gdynamic_arp_maxnum,
      h3c_gdynamic_arp_maxnum_cmd,
      "arp max-learning-num <0-4096> slot <1-8>",
      "Address Resolution Protocol (ARP) module\n"
      "Set the maximum number of dynamic ARP entries that can be learned\n"
      "Value for the maximum number\n"
      "Specify the slot number\n"
      "Slot number\n"
     )
{
    uint16_t num_limit = 0;
    uint16_t slot_num = 0;
    uint32_t slot_num_limit = 0;
    int8_t ret = 0;

    num_limit = (uint32_t)atoi(argv[0]);
    slot_num = (uint8_t)atoi(argv[1]);

    /*slot and num_limit bit calc*/
    slot_num_limit = slot_num << 16 | num_limit;

    slot_numlimit[slot_num] = num_limit;
    ret = ipc_send_msg_n2(&slot_num_limit, 4, 1, MODULE_ID_FTM, MODULE_ID_ARP, IPC_TYPE_ARP,ARP_INFO_SLOT_NUM_LIMIT, 0, 0);
    if(0 != ret)
    {
        vty_error_out(vty,"Send global arp num-limit fail!%s",VTY_NEWLINE);	
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN(h3c_intf_dynamic_arp_maxnum,
      h3c_intf_dynamic_arp_maxnum_cmd,
      "arp max-learning-num <0-4096>",
      "Address Resolution Protocol (ARP) module\n"
      "Set the maximum number of dynamic ARP entries that can be learned\n"
      "Value for the maximum number\n"
      "Specify the slot number\n"
      "Slot number\n"
     )
{
    int8_t ret = 0;
    uint32_t ifindex = 0;
    uint16_t if_arp_num_max = 0;
    struct route_if *pif = NULL;
    struct ifm_arp pif_arp;

    if_arp_num_max = (uint32_t)atoi(argv[0]);
    if (vty->node != CONFIG_NODE)
    {
        ifindex = (uint32_t)(vty->index);
        pif = route_if_lookup(ifindex);
        if (NULL == pif || pif->intf.ipv4_flag == IP_TYPE_INVALID)
        {
            vty_error_out(vty,"Arp num-limit set fail,please check whether the l3if IP is configured!%s",VTY_NEWLINE);
            return CMD_WARNING;
        }


        memset(&pif_arp, 0, sizeof(struct ifm_arp));
        pif_arp.ifindex = ifindex;
        pif_arp.arp_num_max = if_arp_num_max;
        ret = arp_if_update(&pif_arp ,ARP_INFO_NUM_LIMIT );
        if (1 == ret)
        {
            vty_error_out(vty,"Set interface arp num-limit fail!%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    else
    {
        g_arp.num_limit = if_arp_num_max;
        ret = ipc_send_msg_n2(&g_arp, sizeof(struct arp_global), 1, MODULE_ID_FTM,MODULE_ID_ARP, IPC_TYPE_ARP,ARP_INFO_NUM_LIMIT, 0, 0);
		if(0 != ret)
		{
			vty_error_out(vty,"Send global arp num-limit fail!%s",VTY_NEWLINE);	
			return CMD_WARNING;
		}

    }

    return CMD_SUCCESS;
}


DEFUN(h3c_arp_timer_age,
      h3c_arp_timer_age_cmd,
      "arp timer aging <1-1440>",
      "Address Resolution Protocol (ARP) module\n"
      "Specify ARP timer\n"
      "Specify ARP aging timer in minutes\n"
      "The value of timer(minutes)\n"
        )
{
	int8_t ret = 0;

    g_arp.age_time = (uint32_t)atoi(argv[0]) * 60;

    ret = ipc_send_msg_n2(&g_arp, sizeof(struct arp_global), 1, MODULE_ID_FTM,
                        MODULE_ID_ARP, IPC_TYPE_ARP,ARP_INFO_AGE_TIME, 0, 0);
	if(0 != ret)
	{
		vty_error_out(vty,"Send global arp age-time fail.%s",VTY_NEWLINE);	
		return CMD_WARNING;
	}
    
	return CMD_SUCCESS;

}




static struct cmd_node arp_node =
{ 
	ARP_NODE,  
	"%s(config-arp)# ",  
	1, 
};


/*进入arp 视图*/
DEFUN (arp_mode,
	arp_mode_cmd,
	"arp",
	"Arp command\n")
{
	vty->node = ARP_NODE;
	
	return CMD_SUCCESS;
}

/*配置物理口及trunk 接口的静态arp*/
DEFUN(configure_static_arp,
	configure_static_arp_cmd,
	"arp static ip A.B.C.D mac XX:XX:XX:XX:XX:XX interface {ethernet USP |gigabitethernet USP |xgigabitethernet USP | trunk TRUNK} {l3vpn <1-1024>}",
	"Arp\n"
	"Static arp \n"
	"IP address\n"
	"IP format A.B.C.D\n"
	"Mac address\n"
	"Mac format XX:XX:XX:XX:XX:XX\n"
	"Interface\n"
	CLI_INTERFACE_ETHERNET_STR
	CLI_INTERFACE_ETHERNET_VHELP_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
	CLI_INTERFACE_XGIGABIT_ETHERNET_STR
	CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
	"Trunk interface type\n"
	"Trunk interface number <1-128>[.<1-4095>]\n"
	"L3vpn \n"
	"L3vpn value <1-1024>\n")
{
	int ret = 0;
    uint32_t ip = 0;
    uint32_t vpnid = 0;
    uint8_t mac[6];
    uint32_t ifindex = 0;
	struct arp_entry arp_entry;


    /*参数获取及合法性检查*/
    ip = inet_strtoipv4((char *)argv[0]);
    ret = inet_valid_ipv4(ip);
	if (1 != ret)
	{
		vty_error_out(vty, "The ip is invalid.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
    
	ret = ether_valid_mac((char *)argv[1]);
	if (1 == ret)
	{
		vty_error_out(vty, "Please check out mac format.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
    ether_string_to_mac((char *)argv[1], (uchar *)mac);
	if (!ether_is_broadcast_mac(mac))
    {
	    vty_error_out(vty, "Can't set broadcast mac.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
	else if (mac[0] & 0x01)
    {
		vty_error_out(vty, "Can't set multicast mac.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    else if (!ether_is_zero_mac(mac))
    {
		vty_error_out(vty, "Input mac is invalid.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (NULL != argv[2] && NULL == argv[3] &&NULL == argv[4] &&NULL == argv[5])
    {
	    ifindex = ifm_get_ifindex_by_name("ethernet", (char *)argv[2]);
    }
	else if (NULL == argv[2] && NULL != argv[3] &&NULL == argv[4] &&NULL == argv[5])
    {
	    ifindex = ifm_get_ifindex_by_name("gigabitethernet", (char *)argv[3]);
    }
	else if (NULL == argv[2] && NULL == argv[3] &&NULL != argv[4] &&NULL == argv[5])
    {
	    ifindex = ifm_get_ifindex_by_name("xgigabitethernet", (char *)argv[4]);
    }
    else if (NULL == argv[2] && NULL == argv[3] &&NULL == argv[4] &&NULL != argv[5])
    {
        ifindex = ifm_get_ifindex_by_name("trunk", (char *)argv[5]);
    }
	else
	{
		vty_error_out(vty, "Please check out interface format.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	if(ifindex == 0)
	{
		vty_error_out(vty, "Please check out interface format.%s", VTY_NEWLINE );
		return CMD_WARNING;	
	}	
    
    if (NULL != argv[6])
    {
        vpnid = (uint16_t)atoi(argv[6]);
    }

    if (vpnid > VPN_SUPPORT )
    {
        vty_error_out(vty, "This device support vpn range <1-%d>!%s",VPN_SUPPORT,VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    
    /*参数赋值*/
    memset(&arp_entry, 0, sizeof(struct arp_entry));
    arp_entry.status = ARP_STATUS_STATIC;
	arp_entry.key.ipaddr = ip;
    arp_entry.key.vpnid = vpnid;
    memcpy(arp_entry.mac,mac,6);
    arp_entry.ifindex = ifindex;

    //非配置恢复检查l3if 是否存在，不存在则添加失败
    if(1 != vty->config_read_flag)
    {
        if(!route_if_lookup(ifindex))
        {
    		vty_error_out(vty,"Static arp add fail,please add l3if first!%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    
    /*静态arp 添加*/
    ret = arp_static_add(&arp_entry,vty->config_read_flag);
	if(ret == ARP_ENTRY_IS_NULL)
	{
		vty_error_out(vty, "Arp entry is NULL.%s", VTY_NEWLINE);
        return CMD_WARNING;
	}
	else if(ret == ARP_OVER_STATIC_NUM)
	{
		vty_error_out(vty, "Static arp num greater than the limited.%s", VTY_NEWLINE);
        return CMD_WARNING;
	}
	else if(ret == ARP_OVER_TOTAL_SUM)
	{
		vty_error_out(vty, "Total arp num great than limit.%s", VTY_NEWLINE);
        return CMD_WARNING;
	}
	else if(ret == ARP_HASH_BUCKET_FULL)
	{
		vty_error_out(vty, "Add static arp fail,static hash bucket is full.%s", VTY_NEWLINE);
        return CMD_WARNING;
	}
	
	return CMD_SUCCESS;
}



/*配置vlanif 接口的静态arp*/
DEFUN(configure_vlanif_static_arp,
	configure_vlanif_static_arp_cmd,
	"arp static ip A.B.C.D mac XX:XX:XX:XX:XX:XX interface vlanif <1-4094> port {ethernet USP|gigabitethernet USP|xgigabitethernet USP} {l3vpn <1-1024>}",
	"Arp\n"
	"Static arp \n"
	"IP address\n"
	"IP format A.B.C.D\n"
	"Mac address\n"
	"Mac format XX:XX:XX:XX:XX:XX\n"
	"Interface\n"
	"Vlanif interface\n"
	"Vlanif interface number\n"
    "The physical member port\n"
    "Ethernet interface type\n"
	"The subport of the interface, format: <0-7>/<0-31>/<1-255>\n"
	CLI_INTERFACE_GIGABIT_ETHERNET_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
	CLI_INTERFACE_XGIGABIT_ETHERNET_STR
	CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
    "L3vpn \n"
	"L3vpn value <1-1024>\n")
{
	int ret = 0;
    uint32_t ip = 0;
    uint32_t vpnid = 0;
    uint8_t mac[6];
    uint32_t ifindex = 0;
    uint32_t port = 0;
	struct arp_entry arp_entry;


    /*参数获取及合法性检查*/
    ip = inet_strtoipv4((char *)argv[0]);
    ret = inet_valid_ipv4(ip);
	if (1 != ret)
	{
		vty_error_out(vty, "The ip is invalid.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
    
	ret = ether_valid_mac((char *)argv[1]);
	if (1 == ret)
	{
		vty_error_out ( vty, "Please check out mac format.%s", VTY_NEWLINE );
		return CMD_WARNING;
	}
    ether_string_to_mac((char *)argv[1], (uchar *)mac);
	if (!ether_is_broadcast_mac(mac))
    {
        vty_error_out ( vty, "Can't set broadcast mac.%s", VTY_NEWLINE );
        return CMD_WARNING;
    }
	else if (mac[0] & 0x01)
    {
        vty_error_out ( vty, "Can't set multicast mac.%s", VTY_NEWLINE );
        return CMD_WARNING;
    }
    else if (!ether_is_zero_mac(mac))
    {
        vty_error_out ( vty, "Input mac is invalid.%s", VTY_NEWLINE );
        return CMD_WARNING;
    }

    if (NULL != argv[2])
    {
	    ifindex = ifm_get_ifindex_by_name("vlanif", (char *)argv[2]);          
    }
    if (ifindex == 0)
    {
        vty_error_out ( vty, "Please check out interface format.%s", VTY_NEWLINE );
        return CMD_WARNING; 
    }

	if (NULL != argv[3] && NULL == argv[4] && NULL == argv[5] )
    {
	    port = ifm_get_ifindex_by_name("ethernet",(char *)argv[3]);
    }
	else if (NULL == argv[3] && NULL != argv[4] && NULL == argv[5] )
    {
	    port = ifm_get_ifindex_by_name("gigabitethernet",(char *)argv[4]);
    }
	else if (NULL == argv[3] && NULL == argv[4] && NULL != argv[5] )
    {
	    port = ifm_get_ifindex_by_name("xgigabitethernet",(char *)argv[5]);
    }
	else
	{
		vty_error_out ( vty, "Please check out port format.%s", VTY_NEWLINE );
		return CMD_WARNING; 
	}
    if (port == 0)
    {
		vty_error_out ( vty, "Please check out port format.%s", VTY_NEWLINE );
		return CMD_WARNING;	
	}
    else if(IFM_IS_SUBPORT(port))
    {   
    	vty_error_out ( vty, "The vlanif physical member port can't be subport.%s", VTY_NEWLINE );
		return CMD_WARNING;	
    }
    
    if (NULL != argv[6])
    {
        vpnid = (uint16_t)atoi(argv[6]);
    }
    
    if (vpnid > VPN_SUPPORT )
    {
        vty_error_out(vty, "This device support vpn range <1-%d>!%s",VPN_SUPPORT,VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    
    /*参数赋值*/
    memset(&arp_entry, 0, sizeof(struct arp_entry));
    arp_entry.status = ARP_STATUS_STATIC;
	arp_entry.key.ipaddr = ip;
    arp_entry.key.vpnid = vpnid;
    memcpy(arp_entry.mac,mac,6);
    arp_entry.ifindex = ifindex;
    arp_entry.port = port;

    //非配置恢复检查l3if 是否存在，不存在则添加失败
    if(1 != vty->config_read_flag)
    {
        if(!route_if_lookup(ifindex))
        {
    		vty_error_out(vty,"Static arp add fail,please add l3if first!%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    /*静态arp 添加*/
    ret = arp_static_add(&arp_entry,vty->config_read_flag);
	if(ret == ARP_ENTRY_IS_NULL)
	{
		vty_error_out(vty, "Arp entry is NULL.%s", VTY_NEWLINE);
        return CMD_WARNING;
	}
	else if(ret == ARP_OVER_STATIC_NUM)
	{
		vty_error_out(vty, "Static arp num greater than the limited.%s", VTY_NEWLINE);
        return CMD_WARNING;
	}
	else if(ret == ARP_OVER_TOTAL_SUM)
	{
		vty_error_out(vty, "Total arp num great than limit.%s", VTY_NEWLINE);
        return CMD_WARNING;
	}
	else if(ret == ARP_HASH_BUCKET_FULL)
	{
		vty_error_out(vty, "Add static arp fail,static hash bucket is full.%s", VTY_NEWLINE);
        return CMD_WARNING;
	}
    
	return CMD_SUCCESS;
}


/*删除指定静态arp*/
DEFUN(undo_configure_static_arp,
	undo_configure_static_arp_cmd,
	"no arp static ip A.B.C.D {l3vpn <1-1024>}",
	"Delete\n"
	"Arp\n"
	"Static arp\n"
	"IP address\n"
	"IP format A.B.C.D\n"
	"L3vpn\n"
	"L3vpn value <1-1024>\n"
	)
{
    uint32_t ipaddr = 0;
	uint16_t vpnid = 0;
	int ret = 0;


    /*参数获取及合法性检查*/
    ipaddr = inet_strtoipv4((char *)argv[0]);	
	ret = inet_valid_ipv4(ipaddr);
	if (1 != ret)
	{
		vty_error_out(vty, "The ip is invalid.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
    
    if (0 != argv[1])
    {
        vpnid = (uint16_t)atoi(argv[1]);
    }

    if (vpnid > VPN_SUPPORT )
    {
        vty_error_out(vty, "This device support vpn range <1-%d>!%s",VPN_SUPPORT,VTY_NEWLINE);
        return CMD_SUCCESS;
    }

    /*静态arp 删除*/
    ret = arp_static_delete(ipaddr, vpnid); 
    if ( 0 != ret)
    {
        vty_error_out(vty, "Can not find this static arp  %s",VTY_NEWLINE) ;
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/*显示指定静态arp*/
DEFUN(show_arp_static,
	show_arp_static_cmd,
	"show arp static ip A.B.C.D {l3vpn <1-1024>}",
	"Show running arp information\n"
	"Arp\n"
	"Static arp\n"
	"IP address\n"
	"IP format A.B.C.D\n"
	"L3vpn\n"
	"L3vpn value <1-1024>\n")
{
	uint32_t ipaddr = 0;
	uint16_t vpnid = 0;
	int ret = 0;
    struct arp_entry *arp = NULL;
    char ifname[IFNET_NAMESIZE];
    char ipv4_addr[20];
	char mac[32];


    /*参数获取及合法性检查*/
    ipaddr = inet_strtoipv4((char *)argv[0]);	
	ret = inet_valid_ipv4(ipaddr);
	if (1 != ret)
	{
		vty_error_out(vty, "The ip is invalid.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
    
    if ( NULL != argv[1])
    {
       vpnid = (uint16_t)atoi(argv[1]);
    }
    
    if (vpnid > VPN_SUPPORT )
    {
        vty_error_out(vty, "This device support vpn range <1-%d>!%s",VPN_SUPPORT,VTY_NEWLINE);
        return CMD_SUCCESS;
    }


    /*静态arp 查找*/
    arp = arp_static_lookup(ipaddr, vpnid);
    if ( NULL == arp)
    {
        vty_error_out(vty, "Can not find this static arp.%s",VTY_NEWLINE) ;
        return CMD_WARNING;
    }
    else
    {
    	vty_out (vty, "%-15s ", "IP ADDRESS");
		vty_out (vty, "%-18s ", "MAC ADDRESS");
		vty_out (vty, "%-28s ", "INTERFACE");
		vty_out (vty, "%-5s%s", "VPN",VTY_NEWLINE);

        inet_ipv4tostr(arp->key.ipaddr,ipv4_addr);
		ifm_get_name_by_ifindex ( arp->ifindex ,ifname);
    	sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", arp->mac[0], arp->mac[1], 
                            arp->mac[2], arp->mac[3], arp->mac[4], arp->mac[5]);

        vty_out(vty, "%-15s %-18s %-28s %-5d %s", 
                ipv4_addr, mac,ifname,arp->key.vpnid,VTY_NEWLINE);
    }
		
    return CMD_SUCCESS;
}



/*显示所有静态arp*/
DEFUN(show_all_static_arp,
	show_all_static_arp_cmd,
	"show arp static",
	"Show running arp information\n"
	"Arp\n"
	"Static arp\n")
{
	int cursor = 0;
	int cnt = 0;
    struct arp_entry *arp = NULL;
    struct hash_bucket *pbucket = NULL;
    char ipv4_addr[20];
    char ifname[IFNET_NAMESIZE];
	char mac[32];

    vty_out (vty, "%-15s ", "IP ADDRESS");
	vty_out (vty, "%-18s ", "MAC ADDRESS");
	vty_out (vty, "%-28s ", "INTERFACE");
	vty_out (vty, "%-5s%s", "VPN",VTY_NEWLINE);
	
    HASH_BUCKET_LOOP(pbucket, cursor,static_arp_table)
    {
        arp = (struct arp_entry *)pbucket->data;
        if(NULL != arp)
        {
            inet_ipv4tostr(arp->key.ipaddr,ipv4_addr);
			ifm_get_name_by_ifindex ( arp->ifindex ,ifname);
    		sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", arp->mac[0], arp->mac[1],
                                arp->mac[2], arp->mac[3], arp->mac[4], arp->mac[5]);

            vty_out(vty, "%-15s %-18s %-28s %-5d %s", 
                    ipv4_addr, mac,ifname,arp->key.vpnid,VTY_NEWLINE);
    		
    		cnt++;
    	}
    }
    
	vty_out (vty, "STATIC NUM:%d %s",cnt, VTY_NEWLINE);	
    return CMD_SUCCESS;
}


/*接口arp 学习使能*/
DEFUN(arp_interface_enable,
		arp_interface_enable_cmd,
       "arp enable",
       "Arp command\n"
	   "Enable dynamic arp learning\n")
{
	uint32_t ifindex = 0;
	struct ifm_arp ifm_arp;
    struct route_if *pif = NULL;
	struct ifm_arp *parp = NULL;
	int ret = 0;

	ifindex = (uint32_t)(vty->index);
	pif = route_if_lookup(ifindex);
    if ( NULL == pif || pif->intf.ipv4_flag == IP_TYPE_INVALID)
    {
		vty_error_out(vty,"Arp enable fail,please check whether the l3if IP is configured!%s",VTY_NEWLINE);
        return CMD_WARNING;   
    }

    parp = (struct ifm_arp *)(&(pif->arp));
	if (1 == parp->arp_disable)
	{
		vty_error_out(vty,"This l3 interface dynamic arp is enable already!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	memset(&ifm_arp, 0, sizeof(struct ifm_arp));
    ifm_arp.ifindex = ifindex;
	ifm_arp.arp_disable = 1;
	ret = arp_if_update(&ifm_arp, ARP_INFO_LEARN_LIMIT);
	if (0 != ret)
	{
		vty_error_out(vty,"Arp enable fail,please check arp_if_update!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
    
	return CMD_SUCCESS;
}


/*接口arp 学习去使能*/
DEFUN(arp_interface_disable,
		arp_interface_disable_cmd,
       "arp disable",
       "Arp command\n"
	   "Disable dynamic arp learning\n")
{
	uint32_t ifindex = 0;
	struct ifm_arp ifm_arp;
    struct route_if *pif = NULL;
	struct ifm_arp *parp = NULL;
	int ret = 0;

	ifindex = (uint32_t)(vty->index);
    pif = route_if_lookup(ifindex);
    if ( NULL == pif || pif->intf.ipv4_flag == IP_TYPE_INVALID)
    {
		vty_error_out(vty,"Arp disable fail,please check whether the l3if IP is configured!%s",VTY_NEWLINE);
        return CMD_WARNING;   
    }

    parp = (struct ifm_arp *)(&(pif->arp));
	if (0 == parp->arp_disable)
	{
		vty_error_out(vty,"This l3 interface dynamic arp is disable already!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	memset(&ifm_arp, 0, sizeof(struct ifm_arp));
    ifm_arp.ifindex = ifindex;
	ifm_arp.arp_disable = 0;
	ret = arp_if_update(&ifm_arp, ARP_INFO_LEARN_LIMIT);
	if (0 != ret)
	{
		vty_error_out(vty,"Arp disable fail,please check arp_if_update!%s", VTY_NEWLINE);		
		return CMD_WARNING;
	}
    
	return CMD_SUCCESS;
}


/*接口arp 代理使能*/
DEFUN (arp_proxy,                                                         
        arp_proxy_cmd,                                                     
        "arp proxy ",                                                                                                                
        "Arp command\n"
        "Arp proxy\n" 
		)
{  
    uint32_t ifindex = 0;
	struct ifm_arp ifm_arp;
    struct route_if *pif = NULL;
	struct ifm_arp *parp = NULL;
	int ret = 0;

	ifindex = (uint32_t)(vty->index);
	pif = route_if_lookup(ifindex);
	if (NULL == pif || pif->intf.ipv4_flag == IP_TYPE_INVALID)
	{
		vty_error_out(vty,"Arp proxy set,please check whether the l3if IP is configured!%s",VTY_NEWLINE);
		return CMD_WARNING;
	}

    parp = (struct ifm_arp *)(&(pif->arp));
	if (0 == parp->arp_disable)
	{
		vty_error_out(vty,"Arp proxy set fail,please set arp enable frist!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if (1 == parp->arp_proxy )
	{
		vty_error_out(vty,"Arp proxy already set !%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	memset(&ifm_arp, 0, sizeof(struct ifm_arp));
    ifm_arp.ifindex = ifindex;
	ifm_arp.arp_proxy = 1;
	ret = arp_if_update(&ifm_arp , ARP_INFO_AGENT);
	if (0 != ret)
	{
		vty_error_out(vty,"Arp proxy set enable fail,please check arp_if_update!%s", VTY_NEWLINE);		
		return CMD_WARNING;
	}
    
	return CMD_SUCCESS;
}


/*接口arp 代理去使能*/
DEFUN (no_arp_proxy,                                                         
        no_arp_proxy_cmd,                                                     
        "no arp proxy",                                                                                                                
        "Negate a command or set its defaults\n"
        "Arp\n" 
        "Arp proxy\n" 
		)
{  
    uint32_t ifindex = 0;
	struct ifm_arp ifm_arp;
    struct route_if *pif = NULL;
	struct ifm_arp *parp = NULL;
	int ret = 0;

	ifindex = (uint32_t)(vty->index);
	pif = route_if_lookup(ifindex);
	if (NULL == pif || pif->intf.ipv4_flag == IP_TYPE_INVALID)
	{
		vty_error_out(vty,"Arp proxy set fail,please check whether the l3if IP is configured!%s",VTY_NEWLINE);
		return CMD_WARNING;
	}

    parp = (struct ifm_arp *)(&(pif->arp));
	if (0 == parp->arp_disable)
	{
		vty_error_out(vty,"Arp proxy set fail,please set arp enable frist!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if (0 == parp->arp_proxy )
	{
		vty_error_out(vty,"Arp proxy already disable !%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
    
	memset(&ifm_arp, 0, sizeof(struct ifm_arp));
    ifm_arp.ifindex = ifindex;
	ifm_arp.arp_proxy = 0;
	ret = arp_if_update(&ifm_arp , ARP_INFO_AGENT);
	if (0 != ret)
	{        
		vty_error_out(vty,"Arp proxy set disable fail,please check arp_if_update!%s", VTY_NEWLINE);		
		return CMD_WARNING;
	}
    
	return CMD_SUCCESS;
}



/*配置全局arp老化时间*/
DEFUN (arp_age_time,                                                         
        arp_age_time_cmd,                                                     
        "arp age-time <180-65535>",
        "Arp \n"
        "Arp age-time\n"
        "Arp age-time number(in seconds)\n" 
		)
{
	int ret = 0;
	
	g_arp.age_time = (uint32_t)atoi(argv[0]);

    ret = ipc_send_msg_n2(&g_arp, sizeof(struct arp_global), 1, MODULE_ID_FTM,
                        MODULE_ID_ARP, IPC_TYPE_ARP,ARP_INFO_AGE_TIME, 0, 0);
	if(0 != ret)
	{
		vty_error_out(vty,"Send global arp age-time fail.%s",VTY_NEWLINE);	
		return CMD_WARNING;
	}
    
	return CMD_SUCCESS;
}


/*配置接口或全局arp 学习数量限制*/
DEFUN (arp_num_limit,                                                         
        arp_num_limit_cmd,                                                     
        "arp num-limit <0-4096>",                                                                                                                     
        "Arp \n"
        "Arp num-limit\n"
        "Arp num-limit number\n"
		)
{	
	struct ifm_arp ifm_arp;
    struct route_if *pif = NULL;
	struct ifm_arp *parp = NULL;
	uint32_t ifindex = 0;
	uint32_t global_num_limit = 0;
	uint32_t if_arp_num_max = 0;
	int ret = 0;

	if(vty->node ==ARP_NODE)
	{
		global_num_limit = (uint32_t)atoi(argv[0]);
		g_arp.num_limit = global_num_limit;
        ret = ipc_send_msg_n2(&g_arp, sizeof(struct arp_global), 1, MODULE_ID_FTM,
                            MODULE_ID_ARP, IPC_TYPE_ARP,ARP_INFO_NUM_LIMIT, 0, 0);
		if(0 != ret)
		{
			vty_error_out(vty,"Send global arp num-limit fail!%s",VTY_NEWLINE);	
			return CMD_WARNING;
		}
	}
	else
	{	
    	if_arp_num_max = (uint32_t)atoi(argv[0]);
		ifindex = (uint32_t)(vty->index);
        pif = route_if_lookup(ifindex);
		if (NULL == pif || pif->intf.ipv4_flag == IP_TYPE_INVALID)
		{
			vty_error_out(vty,"Arp num-limit set fail,please check whether the l3if IP is configured!%s",VTY_NEWLINE);
			return CMD_WARNING;
		}

        parp = (struct ifm_arp *)(&(pif->arp));
		if (0 == parp->arp_disable)
		{
			vty_error_out(vty,"Arp num-limit set fail,please set arp enable frist!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}

		memset(&ifm_arp, 0, sizeof(struct ifm_arp));
	    ifm_arp.ifindex = ifindex;
		ifm_arp.arp_num_max = if_arp_num_max;
		ret = arp_if_update(&ifm_arp ,ARP_INFO_NUM_LIMIT );
		if (1 == ret)
		{
			vty_error_out(vty,"Set interface arp num-limit fail!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}	
    
	return CMD_SUCCESS;
}


/*显示接口或全局arp 配置*/
DEFUN (show_arp_config,                                                         
        show_arp_config_cmd,                                                     
        "show arp config",                                                       
        SHOW_STR                                                                
        "Arp information\n"
        "Arp config information\n" 
		)
{
    struct ifm_arp *parp = NULL;
    struct route_if *pif = NULL;
	char ifname[IFNET_NAMESIZE];
	uint32_t ifindex;
	struct hash_bucket *pbucket = NULL;
	int cursor = 0;

	if(vty->node == ARP_NODE )
	{
		vty_out(vty,"Global arp config%s",VTY_NEWLINE);
		vty_out (vty, " arp age-time %d%s",g_arp.age_time, VTY_NEWLINE);
		vty_out (vty, " arp num-limit %d%s",g_arp.num_limit, VTY_NEWLINE);
	
		HASH_BUCKET_LOOP( pbucket, cursor,route_if_table)
		{	
			if(pbucket->data)
			{	
				pif = pbucket->data;
                parp = (struct ifm_arp *)(&(pif->arp));
				ifindex = parp->ifindex;
                
                if (0 != ifindex)
                {
                    /*loopback 接口、tunnel 接口不存在arp配置信息*/
                    if(IFM_TYPE_IS_LOOPBCK(ifindex) || IFM_TYPE_IS_TUNNEL(ifindex))
                        continue;
                    
				    ifm_get_name_by_ifindex(ifindex, ifname);
			
				    vty_out ( vty, "interface %s%s", ifname, VTY_NEWLINE );
				    if(parp->arp_disable !=  ARP_DISABLE_DEF)
					    vty_out ( vty, " arp disable %s", VTY_NEWLINE );
    				else
	    				vty_out ( vty, " arp enable %s", VTY_NEWLINE );

		    		if(parp->arp_proxy != ARP_PROXY_DEF)
			    		vty_out ( vty, " arp proxy %s", VTY_NEWLINE );
				    else
					    vty_out ( vty, " arp proxy disable%s", VTY_NEWLINE );
			
    				vty_out ( vty, " arp num-limit %d%s",parp->arp_num_max, VTY_NEWLINE );
                }
            }
		}
	}
	else
	{
        ifindex = (uint32_t)(vty->index);
		pif = route_if_lookup(ifindex);

        //route_if 接口数据结构为空时，命令执行失败
        if (NULL == pif)
        {
            vty_error_out(vty,"Show arp config fail,please check whether the l3if IP is configured!%s",VTY_NEWLINE);
            return CMD_ERR_INCOMPLETE;
        }
        
        if (pif && (pif->intf.ipv4_flag != IP_TYPE_INVALID))
		{
		    parp = (struct ifm_arp *)(&(pif->arp));
			ifindex = parp->ifindex;
			
            if (0 != ifindex)
            {
			    ifm_get_name_by_ifindex(ifindex, ifname);

                vty_out ( vty, "interface %s%s", ifname, VTY_NEWLINE );
			    if(parp->arp_disable !=  ARP_DISABLE_DEF)
				    vty_out ( vty, " arp disable %s", VTY_NEWLINE );
    			else
	    			vty_out ( vty, " arp enable %s", VTY_NEWLINE );

		    	if(parp->arp_proxy != ARP_PROXY_DEF)
			    	vty_out ( vty, " arp proxy %s", VTY_NEWLINE );
    			else
	    			vty_out ( vty, " arp proxy disable%s", VTY_NEWLINE );
			
			    vty_out ( vty, " arp num-limit %d%s",parp->arp_num_max, VTY_NEWLINE );
		    }
        }
	}

    return CMD_SUCCESS;
}


DEFUN (config_arp_fake_expire_time,
      config_arp_fake_expire_time_cmd,
      "arp-fake expire-time <1-30>",
      "Fake arp entry\n"
      "Arp entry expire time\n"
      "The range of expire time\n"
    )
{
    struct arp_global garp;
    uint8_t time = 0;
    int8_t ret = 0;

    memset(&garp, 0, sizeof(struct arp_global));
    time = (uint8_t)atoi(argv[0]);
    garp.fake_expire_time = time;

    ret = ipc_send_msg_n2(&garp, sizeof(struct arp_global), 1, MODULE_ID_FTM, MODULE_ID_ROUTE, 
								        IPC_TYPE_ARP, ARP_INFO_FAKE_TIME, IPC_OPCODE_UPDATE, 0);
    if(ret == -1)
    {
        vty_out(vty, "Error:ipc send to ftm fail!%s",VTY_NEWLINE);
    }

    return CMD_SUCCESS;
    
    
    
}


DEFUN (config_arp_entry_fixed_all,
      config_arp_entry_fixed_all_cmd,
      "arp anti-attack entry-check fixed-all",
      "Arp\n"
      "Arp attack anti\n"
      "Arp entry check\n"
      "Arp entry fixed all\n"
)
{
    struct arp_global garp;
    int8_t ret = 0;

    memset(&garp, 0, sizeof(struct arp_global));
    garp.fixed_flag = ARP_FIXED_ALL;
    ret = ipc_send_msg_n2(&garp, sizeof(struct arp_global), 1, MODULE_ID_FTM, MODULE_ID_ROUTE, 
								        IPC_TYPE_ARP, ARP_INFO_FIXED, IPC_OPCODE_UPDATE, 0);
    if(ret == -1)
    {
        vty_out(vty, "Error:ipc send to ftm fail!%s",VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}


DEFUN (no_config_arp_entry_fixed,
      no_config_arp_entry_fixed_cmd,
      "no arp anti-attack entry-check",
      "No\n"
      "Arp\n"
      "Arp attack anti\n"
      "Arp entry check\n"
)
{
    struct arp_global garp;
    int8_t ret = 0;

    memset(&garp, 0, sizeof(struct arp_global));
    garp.fixed_flag = 0;
    ret = ipc_send_msg_n2(&garp, sizeof(struct arp_global), 1, MODULE_ID_FTM, MODULE_ID_ROUTE, 
								        IPC_TYPE_ARP, ARP_INFO_FIXED, IPC_OPCODE_UPDATE, 0);
    if(ret == -1)
    {
        vty_out(vty, "Error:ipc send to ftm fail!%s",VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}



DEFUN (config_arp_entry_fixed_mac,
      config_arp_entry_fixed_mac_cmd,
      "arp anti-attack entry-check fixed-mac",
      "Arp\n"
      "Arp attack anti\n"
      "Arp entry check\n"
      "Arp entry fixed mac\n"
)
{
    struct arp_global garp;
    int8_t ret = 0;

    memset(&garp, 0, sizeof(struct arp_global));
    garp.fixed_flag = ARP_FIXED_MAC;
    ret = ipc_send_msg_n2(&garp, sizeof(struct arp_global), 1, MODULE_ID_FTM, MODULE_ID_ROUTE, 
								        IPC_TYPE_ARP, ARP_INFO_FIXED, IPC_OPCODE_UPDATE, 0);
    if(ret == -1)
    {
        vty_out(vty, "Error:ipc send to ftm fail!%s",VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}

DEFUN (config_arp_entry_fixed_sendack_mac,
    config_arp_entry_fixed_sendack_cmd,
    "arp anti-attack entry-check send-ack",
    "Arp\n"
    "Arp attack anti\n"
    "Arp entry check\n"
    "Arp entry send ack\n"        
)
{
    struct arp_global garp;
    int8_t ret = 0;

    memset(&garp, 0, sizeof(struct arp_global));
    garp.fixed_flag = ARP_FIXED_SENDACK;
    ret = ipc_send_msg_n2(&garp, sizeof(struct arp_global), 1, MODULE_ID_FTM, MODULE_ID_ROUTE, 
								        IPC_TYPE_ARP, ARP_INFO_FIXED, IPC_OPCODE_UPDATE, 0);
    if(ret == -1)
    {
        vty_out(vty, "Error:ipc send to ftm fail!%s",VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}

DEFUN (arp_conf_debug,
       arp_conf_debug_cmd,
       "debug arp (enable|disable) (all|arp|other)",
       "Debug config\n"
       "Arp config\n"
       "Arp debug enable\n"
       "Arp debug disable\n"
       "Arp debug type all\n"
       "Arp debug type arp\n"
       "Arp debug type other\n")
{
    int enable = 0;
    unsigned int type = 0;

    if(argv[0][0] == 'e') enable = 1;

    if(strcmp(argv[1],"other") == 0)
        type = ARP_DEBUG_OTHER;
    else if(strcmp(argv[1],"arp") == 0)
        type = ARP_DEBUG_LOG;
    else type = ARP_DEBUG_ALL;

    zlog_debug_set(vty, type,  enable);

    return(CMD_SUCCESS);
}





/* 静态 arp 的配置保存 */
static int arp_static_config_write(struct vty *vty)
{    
 	int cursor = 0;
    struct arp_entry *arp = NULL;
    struct hash_bucket *pbucket = NULL;
    char ipv4_addr[20];
    char ifname[IFNET_NAMESIZE];

	//配置恢复时进入arp视图
	vty_out(vty, "arp%s", VTY_NEWLINE);
	if(g_arp.age_time != 1800)
	{
		vty_out ( vty, " arp age-time %d%s",g_arp.age_time, VTY_NEWLINE );
	}

	if(g_arp.num_limit != 4096)
	{
		vty_out ( vty, " arp num-limit %d%s",g_arp.num_limit, VTY_NEWLINE );
	}

    HASH_BUCKET_LOOP(pbucket, cursor,static_arp_table)
    {
    	if(pbucket->data)
    	{
			arp = (struct arp_entry *)pbucket->data;
			
            inet_ipv4tostr(arp->key.ipaddr,ipv4_addr);
			ifm_get_name_by_ifindex ( arp->ifindex ,ifname);
	      
	        vty_out(vty, " arp static ip %s mac %02x:%02x:%02x:%02x:%02x:%02x interface %s",ipv4_addr,
                    arp->mac[0], arp->mac[1], arp->mac[2], arp->mac[3], arp->mac[4], arp->mac[5],ifname);

            if(IFM_TYPE_IS_VLANIF(arp->ifindex))
            {
                ifm_get_name_by_ifindex(arp->port,ifname);
                vty_out(vty," port %s",ifname);
            }
            
	        if(0 !=  arp->key.vpnid)
	        {
	        	vty_out(vty, " l3vpn %d ",arp->key.vpnid);
	        }
	        vty_out (vty, "%s", VTY_NEWLINE);
		}      	  
	}
    
    return 0;
}


/* 静态 arp 的命令初始 */
void arp_static_cmd_init(void)
{
    install_node(&arp_node, arp_static_config_write);

	install_default (ARP_NODE);

	install_element (CONFIG_NODE, &arp_mode_cmd , CMD_SYNC);
	install_element (ARP_NODE, &configure_static_arp_cmd, CMD_SYNC);
    install_element (ARP_NODE, &configure_vlanif_static_arp_cmd, CMD_SYNC);
    install_element (ARP_NODE, &undo_configure_static_arp_cmd, CMD_SYNC);
	install_element (ARP_NODE, &show_arp_static_cmd, CMD_LOCAL);
	install_element (ARP_NODE, &show_all_static_arp_cmd, CMD_LOCAL);

	install_element (ARP_NODE, &show_arp_config_cmd, CMD_LOCAL);
	install_element (ARP_NODE, &arp_num_limit_cmd, CMD_SYNC);
	install_element (ARP_NODE, &arp_age_time_cmd, CMD_SYNC);
}


/* arp 配置命令初始化 */
void arp_cmd_init(void)
{
    arp_static_cmd_init();

	install_element (PHYSICAL_IF_NODE, &arp_interface_enable_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &arp_interface_disable_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &arp_num_limit_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &arp_proxy_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_arp_proxy_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &show_arp_config_cmd, CMD_LOCAL);

	install_element (PHYSICAL_SUBIF_NODE, &arp_interface_enable_cmd, CMD_SYNC);
	install_element (PHYSICAL_SUBIF_NODE, &arp_interface_disable_cmd, CMD_SYNC);
	install_element (PHYSICAL_SUBIF_NODE, &arp_num_limit_cmd, CMD_SYNC);
	install_element (PHYSICAL_SUBIF_NODE, &arp_proxy_cmd, CMD_SYNC);
	install_element (PHYSICAL_SUBIF_NODE, &no_arp_proxy_cmd, CMD_SYNC);
	install_element (PHYSICAL_SUBIF_NODE, &show_arp_config_cmd, CMD_LOCAL);

	install_element (TRUNK_IF_NODE, &arp_interface_enable_cmd, CMD_SYNC);
	install_element (TRUNK_IF_NODE, &arp_interface_disable_cmd, CMD_SYNC);
	install_element (TRUNK_IF_NODE, &arp_num_limit_cmd, CMD_SYNC);
	install_element (TRUNK_IF_NODE, &arp_proxy_cmd, CMD_SYNC);
	install_element (TRUNK_IF_NODE, &no_arp_proxy_cmd, CMD_SYNC);
	install_element (TRUNK_IF_NODE, &show_arp_config_cmd, CMD_LOCAL);

	install_element (TRUNK_SUBIF_NODE, &arp_interface_enable_cmd, CMD_SYNC);
	install_element (TRUNK_SUBIF_NODE, &arp_interface_disable_cmd, CMD_SYNC);
	install_element (TRUNK_SUBIF_NODE, &arp_num_limit_cmd, CMD_SYNC);
	install_element (TRUNK_SUBIF_NODE, &arp_proxy_cmd, CMD_SYNC);
	install_element (TRUNK_SUBIF_NODE, &no_arp_proxy_cmd, CMD_SYNC);
	install_element (TRUNK_SUBIF_NODE, &show_arp_config_cmd, CMD_LOCAL);


	install_element (VLANIF_NODE, &arp_interface_enable_cmd, CMD_SYNC);
	install_element (VLANIF_NODE, &arp_interface_disable_cmd, CMD_SYNC);
	install_element (VLANIF_NODE, &arp_num_limit_cmd, CMD_SYNC);
	install_element (VLANIF_NODE, &arp_proxy_cmd, CMD_SYNC);
	install_element (VLANIF_NODE, &no_arp_proxy_cmd, CMD_SYNC);
	install_element (VLANIF_NODE, &show_arp_config_cmd, CMD_SYNC);
    install_element (ARP_NODE, &config_arp_fake_expire_time_cmd, CMD_SYNC);
    install_element (ARP_NODE, &config_arp_entry_fixed_all_cmd, CMD_SYNC);
    install_element (ARP_NODE, &config_arp_entry_fixed_mac_cmd, CMD_SYNC);
    install_element (ARP_NODE, &config_arp_entry_fixed_sendack_cmd, CMD_SYNC);
	install_element (ARP_NODE, &no_config_arp_entry_fixed_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &arp_conf_debug_cmd, CMD_LOCAL);

#if 0
    /*for h3c cmd*/
    install_element (CONFIG_NODE,&configure_h3c_short_static_arp_cmd);
    install_element (CONFIG_NODE,&configure_h3c_long_static_arp_cmd);
    install_element (CONFIG_NODE,&h3c_undo_arp_cmd);
    install_element (CONFIG_NODE,&h3c_reset_arp_cmd);
    install_element (CONFIG_NODE,&h3c_reset_intf_cmd);
    install_element (CONFIG_NODE,&h3c_reset_slot_cmd);
    install_element (CONFIG_NODE,&h3c_gdynamic_arp_maxnum_cmd);
    install_element (CONFIG_NODE,&h3c_intf_dynamic_arp_maxnum_cmd);
    install_element (CONFIG_NODE,&h3c_arp_timer_age_cmd);
#endif
}


