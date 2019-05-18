/*
*       manage the static mac table
*
*/
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <lib/types.h>
#include <lib/command.h>
#include <lib/module_id.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/prefix.h>
#include <lib/ifm_common.h>
#include <lib/vty.h>
#include <lib/log.h>
#include "mac_static.h"

#define MAC_STATIC_TBL_SHOW_HEAD(vty)   \
do{\
    vty_out ( vty, "Mac static address tables:%s", VTY_NEWLINE );\
    vty_out ( vty, "-------------------------------------------------------------------------------%s", VTY_NEWLINE );\
    vty_out ( vty, "%-21s %-7s %-7s %-31s %-9s%s", "MAC ADDRESS", "VLAN","VSI", "INTERFACE/PW_NAME", "TYPE", VTY_NEWLINE );\
    vty_out ( vty, "-------------------------------------------------------------------------------%s", VTY_NEWLINE );\
}while(0);

#define MAC_STATIC_TBL_SHOW_END(vty,count)   \
do{\
    vty_out ( vty, "-------------------------------------------------------------------------------%s", VTY_NEWLINE );\
    vty_out ( vty, "Interface total: %d%s", count, VTY_NEWLINE );\
} while ( 0 );


#define MAC_STATIC_VLAN_IS_INVALID(vty,vlanid)								\
do {															\
	if ((vlanid < 1) || (vlanid > 4094))							\
        {														\
            vty_error_out(vty, "Please check out vlan <1-4094>%s", VTY_NEWLINE);	\
            return CMD_WARNING;										\
        }														\
}while ( 0 );

#define GOT_ONE_COLONOFMAC(s,e,v) do{\
        int  __len = (e) - (s) + 1;\
        if(__len > 2)\
            return (int)PARSE_MALFORMED;\
        v = strtol(s, NULL, 16); \
}while(0)


#define CLI_IS_MAC_DIGITAL(c) ((((c) >= '0') && ((c) <= '9')) || \
                                (((c) >= 'a') && ((c) <= 'f')) || \
                                (((c) >= 'A') && ((c) <= 'F')))


#define NOT_MAC_DIGITAL_CHK_RET(__pc) do{\
        if((__pc == NULL) || !CLI_IS_MAC_DIGITAL(*(__pc))) return (int)PARSE_MALFORMED;\
}while(0)

#define MAC_TO_STRING(buff,mac) do{\
        sprintf(buff, "%02x:%02x:%02x:%02x:%02x:%02x",mac[0],mac[1], mac[2], mac[3], mac[4],mac[5]); \
}while(0)
 
uchar mac_move_status[2][10] = {"disable","enable"};
uchar mac_mac_limit_action[2][10] = {"discard","forward"};

static struct cmd_node mac_node =
{ 
    MAC_NODE,
    "%s(config-mac)# ",
    1,
};

DEFUN (mac_mode,
        mac_mode_cmd,
        "mac",
        "Mac command mode\n")
{
        vty->node = MAC_NODE;

        return CMD_SUCCESS;
}

int cli_parse_mac_addr(uchar *clistr, uchar *deststr)
{
    int colon = 0;
    int legal_len = 0;
    int actual_len = 0;
    char *prev = NULL;
    char *smac = NULL;
    char *str_tmp = (char *)clistr;

    if ((clistr == NULL) || (deststr == NULL))
        return (int)PARSE_COMMON_ERROR;

    legal_len = strlen("FF:FF:FF:FF:FF:FF");
    actual_len = strlen(str_tmp);

    if (actual_len > legal_len)
    {
        return (int)PARSE_MACLEN_LONG;
    }

    if (actual_len < legal_len)
    {
        return (int)PARSE_MACLEN_SHORT;
    }

    while (1)
    {
        if (CLI_IS_MAC_DIGITAL(*str_tmp))
        {
            if (smac == NULL)
            {
                smac = str_tmp;
            }
        }
        else if (*str_tmp == ':')
        {
            NOT_MAC_DIGITAL_CHK_RET(prev);
            if(NULL==smac)
            {
				return (int)PARSE_INVALID_CHAR;
            }
            GOT_ONE_COLONOFMAC(smac, str_tmp - 1, deststr[colon]);

            smac = NULL;

            colon++;

            if (colon > 5)
            {
                return (int)PARSE_MALFORMED;
            }
        }
        else if (*str_tmp == '\0')
        {
            if (colon != 5)
            {
                return (int)PARSE_MALFORMED;
            }

            NOT_MAC_DIGITAL_CHK_RET(prev);
            if(NULL==smac)
            {
				return (int)PARSE_INVALID_CHAR;
            }

            GOT_ONE_COLONOFMAC(smac, str_tmp - 1, deststr[colon]);

            break;
        }
        else
        {
            return (int)PARSE_INVALID_CHAR;
        }

        prev = str_tmp;
        str_tmp = str_tmp + 1;
    }

    return (int)PARSE_SUCCEED;
}

int cli_mac_form_dotstring(struct vty *vty, uchar *clistr, uchar *deststr)
{
    int retval;
    retval = cli_parse_mac_addr(clistr, deststr);

    if (retval == (int)PARSE_INVALID_CHAR)
    {
        vty_error_out(vty, "Input invalid charactor.%s", VTY_NEWLINE);
        return 1;
    }
    else if (retval == (int)PARSE_MACLEN_SHORT)
    {
        vty_error_out(vty, "Mac string too short.%s", VTY_NEWLINE);
        return 1;
    }
    else if (retval == (int)PARSE_MACLEN_LONG)
    {
        vty_error_out(vty, "Mac string too long.%s", VTY_NEWLINE);
        return 1;
    }
    else if (retval != 0)
    {
        vty_error_out(vty, "Input malformed mac address.%s", VTY_NEWLINE);
        return 1;
    }

    return 0;
}

/* vty out static mac */ 
int mac_static_print (struct vty *vty,struct mac_entry *pmac_entry)
{
	char print_mac[32];
	char ifname[IFNET_NAMESIZE];

	MAC_TO_STRING(print_mac,pmac_entry->key.mac);
    ifm_get_name_by_ifindex(pmac_entry->outif, ifname);
    if (pmac_entry->status == MAC_STATUS_STATIC)
    {
        vty_out(vty, "%-21s %-7d %-7s %-31s %-9s%s", print_mac, pmac_entry->key.vlanid,"-", ifname, "static", VTY_NEWLINE);
    }
    else if (pmac_entry->status == MAC_STATUS_BLACKHOLE)
    {
        vty_out(vty, "%-21s %-7d %-7s %-31s %-9s%s", print_mac, pmac_entry->key.vlanid,"-", ifname, "blackhole", VTY_NEWLINE);
    }
	return 0;
}

DEFUN(configure_ethernet_static_mac,
		configure_ethernet_static_mac_cmd,
		"mac static XX:XX:XX:XX:XX:XX interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP} vlan <1-4094> { blackhole}",
		"Mac\n"
		"Static mac \n"
		"Mac format XX:XX:XX:XX:XX:XX\n"
		"Interface \n"
		"Interface type ethernet\n"
		"The port/subport of the interface, format: <1-7>/<1-31>/<1-255>\n"
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
		"Vlan \n"
		"Vlan value <1-4094>\n"
		"Blackhole \n")
{
    int ret = 0;
    struct mac_entry *pmac_entry = NULL;
    uchar  key_mac[MAC_LEN];
    uint32_t  ifindex=0;
    uint16_t  key_vlanid;
    struct ifm_info pifm_info = {0};
    uint8_t  mode = 0;

    ret = cli_mac_form_dotstring(vty, (uchar *)argv[0], key_mac);

    if (1 == ret)
    {
        return CMD_WARNING;
    }

    if (key_mac[0] & 0x01)
    {
        vty_error_out(vty, "Multicast or broadcast MAC address does not support%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    else if (key_mac[0] == 0 && key_mac[1] == 0 && key_mac[2] == 0 && key_mac[3] == 0
             && key_mac[4] == 0 && key_mac[5] == 0)
    {
        vty_error_out(vty, "The MAC address is invalid %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	if(argv[1] != NULL && argv[2] == NULL && argv[3] == NULL)
    {
    	ifindex = ifm_get_ifindex_by_name ( "ethernet", ( char * ) argv[1] );
	}
	else if(argv[1] == NULL && argv[2] != NULL && argv[3] == NULL)
    {
    	ifindex = ifm_get_ifindex_by_name ( "gigabitethernet", ( char * ) argv[2] );
	}
	else if(argv[1] == NULL && argv[2] == NULL && argv[3] != NULL)
    {
    	ifindex = ifm_get_ifindex_by_name ( "xgigabitethernet", ( char * ) argv[3] );
	}
	
	if(IFM_IS_SUBPORT(ifindex))
	{
    		vty_error_out(vty, "Subinterfaces are not supported %s", VTY_NEWLINE);
    		return CMD_WARNING;
	}

    if (ifindex == 0)
    {
        vty_error_out(vty, "Please check out interface format: <1-7>/<1-31>/<1-255> %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*判断端口是否存在*/
    if(ifm_get_all_info(ifindex, MODULE_ID_L2, &pifm_info) != 0)
    {
        vty_error_out ( vty, "Failed to execute command,the port does not exist.%s", VTY_NEWLINE );
        return CMD_WARNING;   
    }

    if(1 != vty->config_read_flag)//非配置恢复，需检查接口是否是mode switch模式
    {
		/*检查mode是否是switch*/
	    ifm_get_mode(ifindex,MODULE_ID_L2, &mode);
	    if((uint8_t)IFNET_MODE_SWITCH != mode)
	    {
	        vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
	        return CMD_WARNING;
	    }
    }
    
    key_vlanid = (uint16_t)atoi(argv[4]);
    MAC_STATIC_VLAN_IS_INVALID(vty,key_vlanid);
    
    pmac_entry = (struct mac_entry *) XMALLOC(MTYPE_MAC_STATIC, sizeof(struct mac_entry));
    if(NULL==pmac_entry)
    {
    	vty_error_out(vty, "%s%s", "Configure static mac error!", VTY_NEWLINE);
        return CMD_WARNING; 
    }
    memset(pmac_entry, 0, sizeof(struct mac_entry));
    memcpy(pmac_entry->key.mac, key_mac, MAC_LEN);
    pmac_entry->outif = ifindex;
    pmac_entry->key.vlanid = key_vlanid;
    
    if (NULL != argv[5])
    {
        pmac_entry->status = MAC_STATUS_BLACKHOLE;
    }
    else 
    {	
        pmac_entry->status = MAC_STATUS_STATIC;
    }	

    ret = mac_static_add(pmac_entry);

    if (MAC_ERROR_OVERSIZE==ret)
    {
        vty_error_out(vty, "%s%s", "The total num of static mac is already equal to the limit num!", VTY_NEWLINE);
        return CMD_WARNING;
    }
    else if (MAC_ERROR_EXISTED==ret)
    {
        vty_error_out(vty, "%s%s", "This mac with vlan is already configure! ", VTY_NEWLINE);
        return CMD_WARNING; 
    }
    else if (MAC_ERROR_HAL_FAIL==ret)
    {
        vty_error_out(vty, "%s%s", "Configure static mac failed!", VTY_NEWLINE);
        return CMD_WARNING; 
    }
    return CMD_SUCCESS;
}

DEFUN(configure_trunk_static_mac,
      configure_trunk_static_mac_cmd,
      "mac static XX:XX:XX:XX:XX:XX interface trunk <1-128> vlan <1-4094> { blackhole}",
      "Mac\n"
      "Static mac \n"
      "Mac format XX:XX:XX:XX:XX:XX\n"
      "Interface \n"
      "Interface type trunk\n"
      "Trunk <1-128>\n"
      "Vlan \n"
      "Vlan value <1-4094>\n"
      "Blackhole \n")
{
    int ret = 0;
    struct mac_entry *pmac_entry = NULL;
    uchar  key_mac[MAC_LEN];
    uint32_t  ifindex;
    uint16_t  key_vlanid;
    struct ifm_info pifm_info = {0};
    uint8_t  mode = 0;

    ret = cli_mac_form_dotstring(vty, (uchar *)argv[0], key_mac);

    if (1 == ret)
    {
        return CMD_WARNING;
    }

    if (key_mac[0] & 0x01)
    {
        vty_error_out(vty, "Multicast or broadcast MAC address does not support%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    else if (key_mac[0] == 0 && key_mac[1] == 0 && key_mac[2] == 0 && key_mac[3] == 0
             && key_mac[4] == 0 && key_mac[5] == 0)
    {
        vty_error_out(vty, "The MAC address is invalid %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
        
    
    ifindex = ifm_get_ifindex_by_name ( (char *)"trunk", ( char * )argv[1] );
    if ( ifindex == 0 )
    {
        vty_error_out ( vty, "Failed to execute command,the trunk port does not exist.%s", VTY_NEWLINE );
        return CMD_WARNING;
    }

    if(1 != vty->config_read_flag)//非配置恢复，需检查接口是否存在，检查是否是mode switch模式
    {
	    /*判断trunk是否已经存在*/
	    if(ifm_get_all_info(ifindex, MODULE_ID_L2, &pifm_info) != 0)
	    {
	        vty_error_out ( vty, "Failed to execute command,the trunk port does not exist.%s", VTY_NEWLINE );
	        return CMD_WARNING;   
	    }

		/*检查mode是否是switch*/
	    ifm_get_mode(ifindex,MODULE_ID_L2, &mode);
	    if((uint8_t)IFNET_MODE_SWITCH != mode)
	    {
	        vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
	        return CMD_WARNING;
	    }
    }
	
    key_vlanid = (uint16_t)atoi(argv[2]);
    MAC_STATIC_VLAN_IS_INVALID(vty,key_vlanid);
    
    pmac_entry = (struct mac_entry *) XMALLOC(MTYPE_MAC_STATIC, sizeof(struct mac_entry));
    if(NULL==pmac_entry)
    {
    	vty_error_out(vty, "%s%s", "Configure static mac error!", VTY_NEWLINE);
        return CMD_WARNING; 
    }
    memset(pmac_entry, 0, sizeof(struct mac_entry));
    memcpy(pmac_entry->key.mac, key_mac, MAC_LEN);
    pmac_entry->outif = ifindex;
    pmac_entry->key.vlanid = key_vlanid;
    
    if (NULL != argv[3])
    {
        pmac_entry->status = MAC_STATUS_BLACKHOLE;
    }
    else 
    {	
        pmac_entry->status = MAC_STATUS_STATIC;
    }	

    if(1 == vty->config_read_flag)//配置恢复，基于trunk口的静态mac，先保存至本地，trunk创建后在下发配置
    {
        ret = mac_static_local_add(pmac_entry);		
    }
    else
    {
   	 ret = mac_static_add(pmac_entry);
    }
    
    if (MAC_ERROR_OVERSIZE==ret)
    {
        vty_error_out(vty, "%s%s", "The total num of static mac is already equal to the limit num!", VTY_NEWLINE);
        return CMD_WARNING;
    }
    else if (MAC_ERROR_EXISTED==ret)
    {
        vty_error_out(vty, "%s%s", "This mac with vlan is already configure! ", VTY_NEWLINE);
        return CMD_WARNING; 
    }
    else  if (MAC_ERROR_HAL_FAIL==ret)
    {
        vty_error_out(vty, "%s%s", "Configure static mac failed!", VTY_NEWLINE);
        return CMD_WARNING; 
    }
    return CMD_SUCCESS;
}

DEFUN(show_mac_static_all,
      show_mac_static_all_cmd,
      "show mac static",
      "Show static mac information\n"
      "Mac\n"
      "Static mac \n")
{
    int total=0;
    int cursor = 0;   
    struct hash_bucket *pbucket = NULL;
    struct mac_entry *pmac_entry = NULL;
	
    MAC_STATIC_TBL_SHOW_HEAD(vty);

    HASH_BUCKET_LOOP( pbucket, cursor, mac_static_table )
    {
        total++;
        pmac_entry = (struct mac_entry *)pbucket->data;
        mac_static_print(vty,pmac_entry);
    }

    MAC_STATIC_TBL_SHOW_END(vty,total);

    return CMD_SUCCESS;
}

DEFUN(show_mac_static_mac,
      show_mac_static_mac_cmd,
      "show mac static mac XX:XX:XX:XX:XX:XX ",
      "Show static mac information\n"
      "Mac\n"
      "Static mac \n"
      "Mac\n"
      "Mac format XX:XX:XX:XX:XX:XX\n")
{
    int ret = 0, total=0;
    int cursor = 0; 
    struct hash_bucket *pbucket = NULL;
    struct mac_entry *pmac_entry = NULL;
    uchar  key_mac[MAC_LEN];

    MAC_STATIC_TBL_SHOW_HEAD(vty);

    ret = cli_mac_form_dotstring(vty, (uchar *)argv[0], key_mac);

    if (1 == ret)
    {
        return CMD_WARNING;
    }

    //look_up   and vty_out
    HASH_BUCKET_LOOP( pbucket, cursor, mac_static_table )
    {
        pmac_entry = (struct mac_entry *)pbucket->data;

        if (memcmp(pmac_entry->key.mac, key_mac, MAC_LEN) == 0)
        {
            total++;
        	mac_static_print(vty,pmac_entry);
        }
    }
  
    MAC_STATIC_TBL_SHOW_END(vty,total);

    return CMD_SUCCESS;
}

DEFUN(show_mac_static_ethernet,
		show_mac_static_ethernet_cmd,
		"show mac static interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP} {vlan <1-4094> | blackhole}",
		"Show static mac information\n"
		"Mac\n"
		"Static mac \n"
		"Interface\n"
		"Interface type ethernet\n"
		"The port/subport of the interface, format: <1-7>/<1-31>/<1-255>\n"
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
		"Vlan \n"
		"Vlan value <1-4094>\n"
      "Blackhole \n")
{
    int total=0;
    int cursor = 0;   
    uint16_t  key_vlanid;
    struct hash_bucket *pbucket = NULL;
    struct mac_entry *pmac_entry = NULL;
    uint32_t  ifindex=0;

    MAC_STATIC_TBL_SHOW_HEAD(vty);
    
    if(argv[0] != NULL && argv[1] == NULL && argv[2] == NULL)
    {
    	ifindex = ifm_get_ifindex_by_name ( "ethernet", ( char * ) argv[0] );
	}
	else if(argv[0] == NULL && argv[1] != NULL && argv[2] == NULL)
    {
    	ifindex = ifm_get_ifindex_by_name ( "gigabitethernet", ( char * ) argv[1] );
	}
	else if(argv[0] == NULL && argv[1] == NULL && argv[2] != NULL)
    {
    	ifindex = ifm_get_ifindex_by_name ( "xgigabitethernet", ( char * ) argv[2] );
	}

	if(IFM_IS_SUBPORT(ifindex))
	{
    		vty_error_out(vty, "Subinterfaces are not supported %s", VTY_NEWLINE);
    		return CMD_WARNING;
	}

    if (ifindex == 0)
    {
        vty_error_out(vty, "%%Please check out interface format  %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    if (argv[3] == NULL && argv[4] == NULL )	
    {
        //look_up   and vty_out
        HASH_BUCKET_LOOP( pbucket, cursor, mac_static_table )
        {
            pmac_entry = (struct mac_entry *)pbucket->data;

            if (pmac_entry->outif == ifindex)
            {
                total++;
                mac_static_print(vty,pmac_entry);
            }
        }

    }
    else if (argv[3] != NULL && argv[4] == NULL )	
    {	
        key_vlanid = (uint16_t)atoi(argv[3]);
        MAC_STATIC_VLAN_IS_INVALID(vty,key_vlanid);

        //look_up   and vty_out
        HASH_BUCKET_LOOP( pbucket, cursor, mac_static_table )
        {
            pmac_entry = (struct mac_entry *)pbucket->data;

            if (pmac_entry->outif == ifindex)
            {
                if (pmac_entry->key.vlanid == key_vlanid)
                {
                    total++;
                    mac_static_print(vty,pmac_entry);
                }
            }
        }
        
        
    }
    else if(argv[3] == NULL && argv[4] != NULL )
    {
        //look_up   and vty_out
        HASH_BUCKET_LOOP( pbucket, cursor, mac_static_table )
        {
            pmac_entry = (struct mac_entry *)pbucket->data;

            if (pmac_entry->outif == ifindex)
            {
                if (pmac_entry->status == MAC_STATUS_BLACKHOLE)
                {
                    total++;
                    mac_static_print(vty,pmac_entry);
                }
            }
        }

    }
    else if(argv[3] != NULL && argv[4] != NULL )
    {
        key_vlanid = (uint16_t)atoi(argv[3]);

        MAC_STATIC_VLAN_IS_INVALID(vty,key_vlanid);

        //look_up   and vty_out
        HASH_BUCKET_LOOP( pbucket, cursor, mac_static_table )
        {
            pmac_entry = (struct mac_entry *)pbucket->data;

            if (pmac_entry->outif == ifindex)
            {
                if (pmac_entry->key.vlanid == key_vlanid)
                {
                    if (pmac_entry->status == MAC_STATUS_BLACKHOLE)
                    {
                        total++;
                        mac_static_print(vty,pmac_entry);
                    }
                }
            }
        }
    }
    
    MAC_STATIC_TBL_SHOW_END(vty,total);

    return CMD_SUCCESS;
}

DEFUN(show_mac_static_trunk,
      show_mac_static_trunk_cmd,
      "show mac static interface trunk <1-128> {vlan <1-4094> | blackhole}",
      "Show static mac information\n"
      "Mac\n"
      "Static mac \n"
      "Interface\n"
      "Interface type trunk\n"
      "Trunk <1-128>\n"
      "Vlan\n"
      "Vlan value <1-4094>\n"
      "Blackhole \n")      
{
    int total=0;
    int cursor = 0; 
    uint16_t  key_vlanid;
    struct hash_bucket *pbucket = NULL;
    struct mac_entry *pmac_entry = NULL;
    uint32_t  ifindex;

    MAC_STATIC_TBL_SHOW_HEAD(vty);
    
    ifindex = ifm_get_ifindex_by_name((char *)"trunk",(char *) argv[0]);
    if (ifindex == 0)
    {
        vty_error_out(vty, "Please check out interface format  %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    if (argv[1] == NULL && argv[2] == NULL )	
    {
        //look_up   and vty_out
        HASH_BUCKET_LOOP( pbucket, cursor, mac_static_table )
        {
            pmac_entry = (struct mac_entry *)pbucket->data;

            if (pmac_entry->outif == ifindex)
            {
                total++;
                mac_static_print(vty,pmac_entry);
            }
        }

    }
    else if (argv[1] != NULL && argv[2] == NULL )	
    {	
        key_vlanid = (uint16_t)atoi(argv[1]);
        MAC_STATIC_VLAN_IS_INVALID(vty,key_vlanid);

        //look_up   and vty_out
        HASH_BUCKET_LOOP( pbucket, cursor, mac_static_table )
        {
            pmac_entry = (struct mac_entry *)pbucket->data;

            if (pmac_entry->outif == ifindex)
            {
                if (pmac_entry->key.vlanid == key_vlanid)
                {
                    total++;
                    mac_static_print(vty,pmac_entry);
                }
            }
        }
        
        
    }
    else if(argv[1] == NULL && argv[2] != NULL )
    {
        //look_up   and vty_out
        HASH_BUCKET_LOOP( pbucket, cursor, mac_static_table )
        {
            pmac_entry = (struct mac_entry *)pbucket->data;

            if (pmac_entry->outif == ifindex)
            {
                if (pmac_entry->status == MAC_STATUS_BLACKHOLE)
                {
                    total++;
                    mac_static_print(vty,pmac_entry);
                }
            }
        }

    }
    else if(argv[1] != NULL && argv[2] != NULL )
    {
        key_vlanid = (uint16_t)atoi(argv[1]);
        MAC_STATIC_VLAN_IS_INVALID(vty,key_vlanid);

        //look_up   and vty_out
        HASH_BUCKET_LOOP( pbucket, cursor, mac_static_table )
        {
            pmac_entry = (struct mac_entry *)pbucket->data;

            if (pmac_entry->outif == ifindex)
            {
                if (pmac_entry->key.vlanid == key_vlanid)
                {
                    if (pmac_entry->status == MAC_STATUS_BLACKHOLE)
                    {
                        total++;
                        mac_static_print(vty,pmac_entry);
                    }
                }
            }
        }
    }
    
    MAC_STATIC_TBL_SHOW_END(vty,total);
    
    return CMD_SUCCESS;
}

DEFUN(show_mac_static_vlan,
      show_mac_static_vlan_cmd,
      "show mac static vlan <1-4094>",
      "Show static mac information\n"
      "Mac\n"
      "Static mac \n"
      "Vlan\n"
      "Vlan value <1-4094>\n")
{
    int total=0;
    int cursor = 0;  
    struct hash_bucket *pbucket = NULL;
    struct mac_entry *pmac_entry = NULL;
    uint16_t  key_vlanid;

    MAC_STATIC_TBL_SHOW_HEAD(vty);

    key_vlanid = (uint16_t)atoi(argv[0]);
    MAC_STATIC_VLAN_IS_INVALID(vty,key_vlanid);
    
    //look_up   and vty_out
    HASH_BUCKET_LOOP( pbucket, cursor, mac_static_table )
    {
        pmac_entry = (struct mac_entry *)pbucket->data;

        if (pmac_entry->key.vlanid == key_vlanid)
        {
            total++;
            mac_static_print(vty,pmac_entry);
        }
    }
    
    MAC_STATIC_TBL_SHOW_END(vty,total);

    return CMD_SUCCESS;
}

DEFUN(show_mac_static_blackhole,
      show_mac_static_blackhole_cmd,
      "show mac static blackhole",
      "Show static mac information\n"
      "Mac\n"
      "Static mac \n"
      "Blackhole mac\n")
{
    int total=0;
    int cursor = 0;
    struct hash_bucket *pbucket = NULL;
    struct mac_entry *pmac_entry = NULL;

    MAC_STATIC_TBL_SHOW_HEAD(vty);

    //look_up   and vty_out
    HASH_BUCKET_LOOP( pbucket, cursor, mac_static_table )
    {
        pmac_entry = (struct mac_entry *)pbucket->data;

        if (pmac_entry->status == MAC_STATUS_BLACKHOLE)
        {
            total++;
        	mac_static_print(vty,pmac_entry);
        }
    }
	
    MAC_STATIC_TBL_SHOW_END(vty,total);

    return CMD_SUCCESS;
}

DEFUN(undo_configure_static_mac,
      undo_configure_static_mac_cmd,
      "no mac XX:XX:XX:XX:XX:XX vlan <1-4094>",
      "Mac disable\n"
      "Mac\n"
      "Mac format XX:XX:XX:XX:XX:XX\n"
      "Vlan\n"
      "Vlan value <1-4094>\n")
{
    int ret = 0;
    uchar  key_mac[MAC_LEN];
    uint16_t  key_vlanid;

    ret = cli_mac_form_dotstring(vty, (uchar *)argv[0], key_mac);
    if (1 == ret)
    {
        return CMD_WARNING;
    }

    key_vlanid = (uint16_t)atoi(argv[1]);
    MAC_STATIC_VLAN_IS_INVALID(vty,key_vlanid);   

    ret = mac_static_delete(key_mac, key_vlanid);
    if (MAC_ERROR_NOT_EXISTED== ret)
    {
        vty_error_out(vty, "The mac does not exist ! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    else if(MAC_ERROR_MALLOC== ret)
    {
        vty_error_out(vty, "Malloc memory failed ! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    return CMD_SUCCESS;
}

DEFUN(configure_mac_ageing_time,
      configure_mac_ageing_time_cmd,
      "mac age-time <0-65535>",
      "Mac\n"
      "Dynamic mac ageing time \n"
      "Time value <0-65535> seconds and the default value is 300 seconds.\n")
{
    int ret = 0;
    int age_time;
	struct mac_configuration temp_config;

    age_time=atoi(argv[0]);
    if ((age_time < 0) || (age_time > 65535))
    {
        vty_error_out(vty, "Please check out ageing time <0-65535>%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	if((uint32_t)age_time==mac_config.age_time)
    {
        return CMD_SUCCESS;
    }
	
	memset(&temp_config,0,sizeof(struct mac_configuration));
	temp_config.age_time=age_time;

    ret = mac_ageing_time_set(&temp_config);
    if (ret)
    {
		MAC_LOG_DBG("%s[%d]:%s:mac age-time:%d fail\n",__FILE__,__LINE__,__FUNCTION__,age_time);
        vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    else
    {
		MAC_LOG_DBG("%s[%d]:%s:mac age-time:%d success\n",__FILE__,__LINE__,__FUNCTION__,age_time);
        mac_config.age_time=age_time;
    }
    return CMD_SUCCESS;
}

DEFUN(no_configure_mac_ageing_time,
      no_configure_mac_ageing_time_cmd,
      "no mac age-time",
      "Disable mac ageing time\n"
      "Mac\n"
      "Dynamic mac ageing time \n")
{
    int ret = 0;	
	struct mac_configuration temp_config;
	
	if(MAC_AGE_TIME==mac_config.age_time)
    {
        return CMD_SUCCESS;
    }
	
	memset(&temp_config,0,sizeof(struct mac_configuration));
	temp_config.age_time=MAC_AGE_TIME;
	
    ret = mac_ageing_time_set(&temp_config);
    if (0 != ret)
    {
		MAC_LOG_DBG("%s[%d]:%s:mac age-time:%d\n",__FILE__,__LINE__,__FUNCTION__,MAC_AGE_TIME);
        vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    else
    {
		MAC_LOG_DBG("%s[%d]:%s:mac age-time:%d success\n",__FILE__,__LINE__,__FUNCTION__,MAC_AGE_TIME);
	 	mac_config.age_time=MAC_AGE_TIME;
    }	
    return CMD_SUCCESS;	
}

DEFUN(show_mac_config,
      show_mac_config_cmd,
      "show mac config",
      "Show\n"
      "Mac\n"
      "Mac configuration \n")
{
    
	vty_out(vty,  "mac age-time   : %d seconds%s", mac_config.age_time,VTY_NEWLINE);
	vty_out(vty, "mac move status: %s%s", mac_move_status[mac_config.mac_move],VTY_NEWLINE);
	if(0!=mac_config.limit_num)
	{
		vty_out(vty, "mac limit      : %d%s", mac_config.limit_num,VTY_NEWLINE);
		vty_out(vty, "limit action   : %s%s", mac_mac_limit_action[mac_config.limit_action],VTY_NEWLINE);
	}
   	return CMD_SUCCESS;

}

DEFUN(config_mac_move,
      config_mac_move_cmd,
      "mac move (enable |disable)",
      "Mac\n"
      "Config mac move\n"
      "Mac move enable \n"
      "Mac move disable  \n")
{

	uint8_t mac_move= 0;
	struct mac_configuration temp_config;
	int ret = 0;	
	
	if(!strncmp("enable", argv[0], strlen("en")))
		mac_move = ENABLE;
	else if(!strncmp("disable", argv[0], strlen("dis")))
		mac_move = DISABLE;
	else
	{
		vty_error_out (vty, "Invalid input%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	if(mac_move==mac_config.mac_move)
    {
        return CMD_SUCCESS;
    }
	
	memset(&temp_config,0,sizeof(struct mac_configuration));
	temp_config.mac_move=mac_move;

	ret = mac_move_set(&temp_config);
	if (0 != ret)
    {
		MAC_LOG_DBG("%s[%d]:%s:mac_move:%s fail\n",__FILE__,__LINE__,__FUNCTION__,mac_move_status[mac_move]);
        vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
	else
	{
		MAC_LOG_DBG("%s[%d]:%s:mac_move:%s success\n",__FILE__,__LINE__,__FUNCTION__,mac_move_status[mac_move]);
	    mac_config.mac_move=mac_move;
	}	
	return CMD_SUCCESS;
}


DEFUN(mac_mac_limit,
	mac_mac_limit_cmd,
	"mac limit <1-16384> action (discard | forward)",
	"Mac\n"
	"Limit the count of mac learn\n"
	"Limit value <1-16384> and the default value is 0.\n"
	"Action when limit value is exceeded\n"
	"Discard the packet\n"
	"Forward the packet\n"
	)
{
	uint32_t limit_num = 0;
	uint8_t action=0;
	struct mac_configuration temp_config;
	int ret=0;
	
	limit_num= (s_int32_t)atoi(argv[0]);
	if ((limit_num < 1) || (limit_num > 16384))
	{
		vty_error_out(vty, "Please check out mac limit <0-16384>%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(!strncmp("forward", argv[1], strlen("for")))
		action = ENABLE;
	else if(!strncmp("discard", argv[1], strlen("dis")))
		action = DISABLE;

	if((limit_num==mac_config.limit_num)&&(action==mac_config.limit_action))
	{
		return CMD_SUCCESS;
	}

	memset(&temp_config,0,sizeof(struct mac_configuration));
	temp_config.limit_action=action;
	temp_config.limit_num=limit_num;

	ret = mac_limit_set(&temp_config);
	if (-1== ret)
	{
        vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	else
	{
	    mac_config.limit_num=limit_num;
	    mac_config.limit_action=action;
	}

	MAC_LOG_DBG("%s[%d]:%s:mac limit:%d,action:%s\n",__FILE__,__LINE__,__FUNCTION__,limit_num,mac_mac_limit_action[action]);
	return CMD_SUCCESS;
	
}

DEFUN(no_mac_mac_limit,
	no_mac_mac_limit_cmd,
	"no mac limit",
	"Clear config\n"
	"Mac\n"
	"Limit the count of mac learn\n")
{
	uint32_t limit_num = 0;
	struct mac_configuration temp_config;
	int ret=0;
	
	if(limit_num==mac_config.limit_num)
	{
		return CMD_SUCCESS;
	}
	
	memset(&temp_config,0,sizeof(struct mac_configuration));
	temp_config.limit_num=limit_num;

	ret = mac_limit_set(&temp_config);
	if (-1== ret)
	{
        vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	else
	{
		mac_config.limit_num=limit_num;
	}

	MAC_LOG_DBG("%s[%d]:%s:mac limit:%d success\n",__FILE__,__LINE__,__FUNCTION__,limit_num);
	return CMD_SUCCESS;

	
}


/* write the configuration of static mac */ 
int mac_static_config_write (struct vty *vty)
{
    struct mac_entry *pmac_entry= NULL;
    char ifname[IFNET_NAMESIZE];
    struct hash_bucket *pbucket = NULL;
    int cursor = 0;
    char print_mac[32];

    vty_out(vty, "mac%s",VTY_NEWLINE);	
   	vty_out(vty, " mac age-time %d%s", mac_config.age_time,VTY_NEWLINE);
   	vty_out(vty, " mac move %s%s", mac_move_status[mac_config.mac_move],VTY_NEWLINE);
	if(0!=mac_config.limit_num)
	{
		vty_out(vty, " mac limit %d action %s%s", mac_config.limit_num,mac_mac_limit_action[mac_config.limit_action],VTY_NEWLINE);
	}
	
    HASH_BUCKET_LOOP( pbucket, cursor, mac_static_table )
    {
        pmac_entry = (struct mac_entry *)pbucket->data;

        ifm_get_name_by_ifindex(pmac_entry->outif, ifname);
        MAC_TO_STRING(print_mac,pmac_entry->key.mac);

        if (pmac_entry->status == MAC_STATUS_STATIC)
        {
            vty_out ( vty, " mac static %s interface %s vlan %d %s",print_mac, ifname,pmac_entry->key.vlanid, VTY_NEWLINE );
        }
        else if (pmac_entry->status == MAC_STATUS_BLACKHOLE)
        {
            vty_out ( vty, " mac static %s interface %s vlan %d %s%s",print_mac, ifname,pmac_entry->key.vlanid, "blackhole", VTY_NEWLINE );
        }
    }
    return 0;
}

void mac_static_cmd_init(void)
{
    mac_config_init();
    install_node(&mac_node, mac_static_config_write);
    install_default (MAC_NODE);

    install_element(CONFIG_NODE, &mac_mode_cmd, CMD_SYNC);	
    install_element(MAC_NODE, &configure_mac_ageing_time_cmd, CMD_SYNC);    
    install_element(MAC_NODE, &no_configure_mac_ageing_time_cmd, CMD_SYNC);
    install_element(MAC_NODE, &config_mac_move_cmd, CMD_SYNC);
    install_element(MAC_NODE, &configure_ethernet_static_mac_cmd, CMD_SYNC);
    install_element(MAC_NODE, &configure_trunk_static_mac_cmd, CMD_SYNC);	
    install_element(MAC_NODE, &undo_configure_static_mac_cmd, CMD_SYNC);
    install_element(MAC_NODE, &mac_mac_limit_cmd, CMD_SYNC);
    install_element(MAC_NODE, &no_mac_mac_limit_cmd, CMD_SYNC);

	/* show 命令注册在 config 节点 */
    install_element(CONFIG_NODE, &show_mac_static_all_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_mac_static_mac_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_mac_static_ethernet_cmd, CMD_LOCAL);    
    install_element(CONFIG_NODE, &show_mac_static_trunk_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_mac_static_vlan_cmd, CMD_LOCAL); 
    install_element(CONFIG_NODE, &show_mac_static_blackhole_cmd, CMD_LOCAL); 
    install_element(CONFIG_NODE, &show_mac_config_cmd, CMD_LOCAL); 
	
}


