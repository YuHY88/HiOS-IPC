/* OSPF VTY interface.
 * Copyright (C) 2005 6WIND <alain.ritoux@6wind.com>
 * Copyright (C) 2000 Toshiaki Takada
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <zebra.h>
#include "lib/memory.h"
#include "lib/thread.h"
#include "lib/prefix.h"
#include "lib/table.h"
#include "lib/vty.h"
#include "lib/command.h"
#include "lib/plist.h"
#include "lib/log.h"
#include "lib/prefix.h"
#include "lib/errcode.h"
#include "lib/timer.h"
#include "lib/route_types.h"
#include "lib/module_id.h"
#include "lib/mpls_common.h"
#include "lib/ospf_common.h"
#include "lib/pkt_buffer.h"


#include "ospfd/ospfd.h"
#include "ospfd/ospf_asbr.h"
#include "ospfd/ospf_lsa.h"
#include "ospfd/ospf_lsdb.h"
#include "ospfd/ospf_ism.h"
#include "ospfd/ospf_interface.h"
#include "ospfd/ospf_nsm.h"
#include "ospfd/ospf_neighbor.h"
#include "ospfd/ospf_flood.h"
#include "ospfd/ospf_abr.h"
#include "ospfd/ospf_spf.h"
#include "ospfd/ospf_route.h"
#include "ospfd/ospf_zebra.h"
#include "ospfd/ospf_vty.h"
#include "ospfd/ospf_dump.h"
#include "ospfd/ospf_ipc.h"
#include "ospfd/ospf_packet.h"
#include "ospfd/ospf_bfd.h"


//extern struct ospf_statics ospf_sta;

static const char *ospf_network_type_str[] =
{
    "Null",
    "P2P",
    "BROADCAST",
    "NBMA",
    "P2MP",
    "VIRTUALLINK",
    "LOOPBACK"
};


/* Utility functions. */
int
ospf_str2area_id(const char *str, struct in_addr *area_id, int *format)
{
    char *endptr = NULL;
    unsigned long ret = 0;

    /* match "A.B.C.D". */
    if (strchr(str, '.') != NULL)
    {
        ret = inet_aton(str, area_id);

        if (!ret)
        {
            return -1;
        }

        *format = OSPF_AREA_ID_FORMAT_ADDRESS;
    }
    /* match "<0-4294967295>". */
    else
    {
        if (*str == '-')
        {
            return -1;
        }

        errno = 0;
        ret = strtoul(str, &endptr, 10);

        if (*endptr != '\0' || errno || ret > UINT32_MAX)
        {
            return -1;
        }

        area_id->s_addr = htonl(ret);
        *format = OSPF_AREA_ID_FORMAT_DECIMAL;
    }

    return 0;
}


static int
str2metric(const char *str, int *metric)
{
    /* Sanity check. */
    if (str == NULL)
    {
        return 0;
    }

    *metric = strtol(str, NULL, 10);

    if (*metric < 0 || *metric > 16777214)
    {
        /* vty_out (vty, "OSPF metric value is invalid%s", VTY_NEWLINE); */
        return 0;
    }

    return 1;
}

static int
str2metric_type(const char *str, int *metric_type_value)
{
    /* Sanity check. */
    if (str == NULL)
    {
        return 0;
    }

    if (strncmp(str, "1", 1) == 0)
    {
        *metric_type_value = EXTERNAL_METRIC_TYPE_1;
    }
    else if (strncmp(str, "2", 1) == 0)
    {
        *metric_type_value = EXTERNAL_METRIC_TYPE_2;
    }
    else
    {
        return 0;
    }

    return 1;
}

int
ospf_oi_count(struct interface *ifp)
{
    struct route_node *rn = NULL;
    int i = 0;

    for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
    {
        if (rn->info)
        {
            i++;
        }
    }

    return i;
}

static struct interface *
ospf_find_interface(uint32_t ifindex)
{
    struct interface *ifp = NULL;
    vrf_id_t vrf_id = VRF_DEFAULT;
    uint8_t pdown_flag = 0;
    struct ifm_event pevent;
    pevent.ifindex = ifindex;

    ifp = if_lookup_by_index_vrf(pevent.ifindex, vrf_id);

    if (ifp != NULL)
    {
        return ifp;
    }

    /* get link */

    if (ifm_get_link(ifindex, MODULE_ID_OSPF, &pdown_flag) != 0)
    {
        pevent.up_flag = IFNET_LINKDOWN;
    }
    else
    {
        pevent.up_flag = pdown_flag ;
    }

    /* create the struct pointer ifp */
    ifp = ospf_interface_add(ZEBRA_INTERFACE_ADDRESS_ADD, pevent, vrf_id);
    return ifp;
}

static int 
ip_address_str_format_2_prefix(struct vty *vty, struct prefix_ipv4 *target_p, const char *ip_addr, const char *netmask)
{
	int ret = 0;
	u_char mask_len = 0;
	struct in_addr temp_mask;
	
	//get ip
	VTY_CHECK_OSPF_ADDRESS("ip address", ip_addr);			/*check A.B.C.D*/
	VTY_GET_IPV4_ADDRESS("ip address", target_p->prefix,ip_addr);	/* Change format */
	
	//get mask
	if(strlen(netmask) > 2)//if A.B.C.D format
	{
		ret = inet_aton(netmask, &temp_mask);
		if (! ret)
		{
			vty_error_out(vty, "Please specify valid netmask as a.b.c.d or 0~32%s",
					VTY_NEWLINE);
			return CMD_WARNING;
		}
		mask_len = ip_masklen(temp_mask);
	}
	else//0~32 format
	{
		VTY_GET_INTEGER_RANGE("ip netmask", mask_len, netmask, 0, IPV4_MAX_PREFIXLEN);
	}
	
	if(mask_len > IPV4_MAX_PREFIXLEN)
	{
		vty_error_out(vty, "Please specify valid netmask length.%s",
			VTY_NEWLINE);
		return CMD_WARNING;
	}
	target_p->prefixlen = mask_len;
	target_p->family = AF_INET;

	return CMD_SUCCESS;
}

DEFUN(router_ospf,
      router_ospf_cmd,
      "ospf instance <1-255> {vpn-instance <1-127>}",
      "Enable a OSPF instance\n"
      "Start OSPF configuration\n"
      "OSPF instance number\n"
      "ospf vpn\n"
      "ospf Vpnid format <1-127> value\n")
{
    int ospf_id;
    uint16_t vpnid;
    struct ospf *ospf = NULL;
    uint16_t l3vpn_id = 0;

    ospf_id = OSPF_DEFAULT_PROCESS_ID;

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0]
                              , OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID);
    }


    if (argv[1] != NULL)
    {

        VTY_GET_INTEGER_RANGE("ospf vpn", vpnid, argv[1], 1, 127);

        if ((ospf = ospf_lookup_id(ospf_id)) == NULL)
        {
            if (vty->config_read_flag != 1)
            {
                l3vpn_id = mpls_com_get_l3vpn_instance2(vpnid, MODULE_ID_OSPF);
                if (0 == l3vpn_id)
                {
                    vty_error_out(vty, "The VPN-Instance %d does not exist.%s", vpnid, VTY_NEWLINE);
                    return CMD_WARNING;
                }
            }

            ospf = ospf_get(ospf_id);

            ospf->vpn = vpnid;
        }
        else
        {

            if (ospf->vpn != vpnid)
            {
                vty_error_out(vty, "The specified Instance ID has been used another vpn instance.%s", VTY_NEWLINE);
                return CMD_WARNING;
            }
        }

    }
    else
    {
        ospf = ospf_get(ospf_id);
    }


    vty->node = OSPF_NODE;
    vty->index = ospf;
    return CMD_SUCCESS;
}

DEFUN(router_ospf_h3c,
	router_ospf_cmd_h3c,
	"ospf <1-65535> {router-id A.B.C.D | vpn-instance STRING}",
	"Enable a OSPF instance\n"
	"OSPF instance number\n"
	"router-id for the OSPF instance\n"
    "OSPF router-id in IP address format\n"
	"ospf vpn\n"
	"VPN instance name\n")
{
	int ospf_id;
	uint16_t vpnid;
	struct in_addr router_id;
	struct ospf *ospf = NULL;
    uint16_t l3vpn_id = 0;
	
	ospf_id = OSPF_DEFAULT_PROCESS_ID;
	
	if (argv[0] != NULL)
	{
		VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0]
							  , OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID_H3C);
	}

	if (argv[2] != NULL)
	{
		OSPF_LOG_DEBUG(" argv[2] = %s\n", argv[2]);
		VTY_GET_INTEGER_RANGE("ospf vpn", vpnid, argv[2], 1, 127);

        if ((ospf = ospf_lookup_id(ospf_id)) == NULL)
        {
            if (vty->config_read_flag != 1)
            {
                l3vpn_id = mpls_com_get_l3vpn_instance2(vpnid, MODULE_ID_OSPF);
                if (0 == l3vpn_id)
                {
                    vty_error_out(vty, "The VPN-Instance %d does not exist.%s", vpnid, VTY_NEWLINE);
                    return CMD_WARNING;
                }
            }

            ospf = ospf_get(ospf_id);

            ospf->vpn = vpnid;
        }
        else
        {

            if (ospf->vpn != vpnid)
            {
                vty_error_out(vty, "The specified Instance ID has been used another vpn instance.%s", VTY_NEWLINE);
                return CMD_WARNING;
            }
        }
	}
	else
    {
        ospf = ospf_get(ospf_id);
    }
	
	if (argv[1] != NULL)
	{
		OSPF_LOG_DEBUG(" argv[1] = %s\n", argv[1]);

		/*check A.B.C.D*/
	    VTY_CHECK_OSPF_ADDRESS("Router ID", argv[1]);
	    /* Change route_id format */
	    VTY_GET_IPV4_ADDRESS("Router ID", router_id, argv[1]);

		/* check route_id 0.0.0.0 */
	    if (router_id.s_addr == 0)
	    {
	        vty_error_out(vty, "Cannot set router id 0.0.0.0%s", VTY_NEWLINE);
	        return CMD_WARNING;
	    }

	    if (ospf->router_id.s_addr == router_id.s_addr)
	    {
	        vty_warning_out(vty, "The router id has alreadly taken effect.%s", VTY_NEWLINE);
	        ospf->router_id_static = router_id;
	        return CMD_SUCCESS;
	    }

	    /*check old router id*/
	    if (ospf->router_id_static.s_addr == router_id.s_addr)
	    {
	        vty_warning_out(vty, "The router id is same as now configure.%s", VTY_NEWLINE);
	        return CMD_WARNING;
	    }

	    ospf->router_id_static = router_id;

	    if (ospf->router_id.s_addr == 0)
	    {
	        ospf_router_id_update(ospf);
	    }
	    else
	    {
	        //Not the first time, You need to restart the OSPF instance to validate the new router ID.
			ospf_reset(ospf);
	    }
	}

	vty->node = OSPF_NODE;
    vty->index = ospf;
    return CMD_SUCCESS;
}


DEFUN(no_router_ospf,
      no_router_ospf_cmd,
      "no ospf instance <1-255>",
      NO_STR
      "Enable a OSPF instance\n"
      "Start OSPF configuration\n"
      "OSPF instance number\n")
{
    struct ospf *ospf = NULL;
    int ospf_id;
    ospf_id = OSPF_DEFAULT_PROCESS_ID;

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0]
                              , OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID_H3C);
    }

    ospf = ospf_lookup_id(ospf_id);

    if (ospf == NULL)
    {
        VTY_OSPF_INSTANCE_ERR
    }
	
	if((ospf_id == OSPF_MAX_PROCESS_ID) && (ospf->dcn_enable == OSPF_DCN_ENABLE))
	{
		vty_warning_out(vty, "When ospf-dcn enabled, ospf instance 255 can not be deleted. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
    ospf_finish (ospf);
    return CMD_SUCCESS;
}

ALIAS(no_router_ospf,
	undo_router_ospf_h3c_cmd,
	"undo ospf [<1-65535>]",
	"Cancel current configuration\n"
	"Enable a OSPF instance\n"
	"OSPF instance number\n")


DEFUN(area_ospf,
      area_ospf_cmd,
      "area <0-255>",
      "enter area node\n"
      "OSPF area ID as a decimal value\n")
{
    vty->node = AREA_NODE;
    struct in_addr area_id;
    struct ospf *ospf = (struct ospf*)(vty->index);
    struct ospf_area* area = NULL;
    int format = 1;

    VTY_GET_OSPF_AREA_ID(area_id, format, argv[0]);

    area = ospf_area_get(ospf, area_id, format);

    if (area != NULL)
    {
        vty->index_sub = area;
    }
    else
    {
        vty_error_out(vty, "Can not inter area node%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN(area_ospf_h3c,
	area_ospf_address_format_cmd,
	"area A.B.C.D",
	"enter area node\n"
	"OSPF area ID as a ip address format\n")
{
	vty->node = AREA_NODE;
	struct in_addr area_id;
	struct ospf *ospf = (struct ospf*)(vty->index);
	struct ospf_area* area = NULL;
	int format = 1;

	VTY_GET_OSPF_AREA_ID(area_id, format, argv[0]);

	area = ospf_area_get(ospf, area_id, format);

	if (area != NULL)
	{
		vty->index_sub = area;
	}
	else
	{
		vty_error_out(vty, "Can not inter area node%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}



static
struct ospf_network *
ospf_look_up_area_network(struct ospf_area* area)
{
    struct route_node *rn = NULL;
    struct ospf_network *network = NULL;

    for (rn = route_top(area->ospf->networks); rn; rn = route_next(rn))
    {
        if ((network = (struct ospf_network *)rn->info) != NULL
                && !(memcmp(&network->area_id , &area->area_id, sizeof(struct in_addr))))
        {
            return network;
        }
    }

    return NULL;
}

DEFUN(no_area_ospf,
      no_area_ospf_cmd,
      "no area <0-255>",
      NO_STR
      "disable area node\n"
      "OSPF area ID as a decimal value\n")
{
	int format = -1;
    struct in_addr area_id;
    struct ospf* ospf = vty->index;
    struct ospf_area* area = NULL;
    struct ospf_network *network = NULL;
	
    VTY_GET_OSPF_AREA_ID(area_id, format, argv[0]);
    area = ospf_area_lookup_by_area_id(ospf, area_id);

    if (area)
    {
    	//modify by zzl 2019.1.24 for bug#58509
		if(area_id.s_addr == 0)
		{
			if(listcount (ospf->vlinks) > 0)
			{
				vty_error_out(vty, " Failed to delete the backbone area because the virtual-link exists.%s", VTY_NEWLINE);
            	return CMD_WARNING;
			}
		}
		
        network = ospf_look_up_area_network(area);

        if (network)
        {
            vty_warning_out(vty, "Please delete the network command(s) of the area first.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
        else
        {
            if (IS_OSPF_ABR(ospf) && IS_AREA_NSSA(area))
            {
                ospf->redistribute--;
            }

            if (IS_AREA_NSSA(area))
            {
                ospf->anyNSSA--;
            }

            if (IS_AREA_STUB(area))
            {
                ospf->anySTUB--;
            }

            ospf_asbr_status_update(ospf, ospf->redistribute);
            listnode_delete(ospf->areas, area);
            ospf_area_free(area);
			area = NULL;

            return CMD_SUCCESS;
        }
    }
    else
    {
        vty_error_out(vty, "The area does not exist.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
}


ALIAS(no_area_ospf,
	no_area_ospf_address_format_cmd,
	"no area A.B.C.D",
	NO_STR
	"disable area node\n"
	"OSPF area ID as a ip address format\n")


ALIAS(no_area_ospf,
	no_area_ospf_h3c_cmd,
	"undo area A.B.C.D",
	"Cancel current configuration\n"
	"disable area node\n"
	"OSPF area ID as a ip address format\n")



/* check Ip address validte*/
static
int ospf_check_input_ip_valid(int ip)
{
    unsigned int a1 = 0, a2 = 0, a3 = 0, a4 = 0;
    a1 = ((unsigned char *) & ip)[0];
    a2 = ((unsigned char *) & ip)[1];
    a3 = ((unsigned char *) & ip)[2];
    a4 = ((unsigned char *) & ip)[3];

    if (0 == a1)
    {
        if (0 != ip)
        {
            return ERRNO_CLI;
        }
    }
    else if (0 == a4)
    {
        return ERRNO_CLI;
    }
    else if (255 == a1 && 255 == a2 && 255 == a3 && 255 == a4)
    {
        return ERRNO_CLI;
    }
    else if (a1 >= 224 && a1 <= 239)
    {
        return ERRNO_CLI;
    }
    else if (a1 >= 240 && a1 <= 255)
    {
        return ERRNO_CLI;/*experimental address*/
    }
    else if (127 == a1)
    {
        return ERRNO_CLI;
    }

    return ERRNO_SUCCESS;
}



DEFUN(ospf_router_id,
      router_ospf_id_cmd,
      "router-id A.B.C.D",
      "router-id for the OSPF instance\n"
      "OSPF router-id in IP address format\n")
{
    struct ospf *ospf = vty->index;
    struct in_addr router_id;
	
    /*check A.B.C.D*/
    VTY_CHECK_OSPF_ADDRESS("Router ID", argv[0]);
    /* Change route_id format */
    VTY_GET_IPV4_ADDRESS("Router ID", router_id, argv[0]);
	
    /* check route_id 0.0.0.0 */
    if (router_id.s_addr == 0)
    {
        vty_error_out(vty, "Cannot set router id 0.0.0.0%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (ospf->router_id.s_addr == router_id.s_addr)
    {
        vty_warning_out(vty, "The router id has alreadly taken effect.%s", VTY_NEWLINE);
        ospf->router_id_static = router_id;
        return CMD_SUCCESS;
    }

    /*check old router id*/
    if (ospf->router_id_static.s_addr == router_id.s_addr)
    {
        vty_warning_out(vty, "The router id is same as now configure.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ospf->router_id_static = router_id;

    if (ospf->router_id.s_addr == 0)
    {
        ospf_router_id_update(ospf);
    }
    else
    {
        vty_info_out(vty, "Set router id succeeded. You need to restart the OSPF instance to validate the new router ID.%s", VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}

/********************************* DCN Command Line START ****************************************************/

/* 十六进制 字符串转为十进制数字*/
u_int32_t
ospf_hexstr2int ( char *str, int *ret )
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
	{ 
		return 0;
	}

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
		/*Not a hex number*/
		//return 0;//delete by zzl, Solve the situation when the parameter is not 0x.
	}

	if ( len < 1 || len > 10 )
	{ 
		return 0; 
	}

	for ( i = 0; i < len; i++ )
	{
		if ( *str >= '0' && *str <= '9' )
		{ 
			digit = *str++ - '0'; 
		}
		else if ( *str >= 'A' && *str <= 'F' )
		{ 
			digit = *str++ - 'A' + 10; 
		}
		else if ( *str >= 'a' && *str <= 'f' )
		{ 
			digit = *str++ - 'a' + 10; 
		}
		else
		{ 
			return 0; 
		}

		if ( total  > UINT32_MAX )
		{ 
			return 0; 
		}

		powv = 1;
		for ( j = 0; j < len - ( i + 1 ); j++ )
		{ 
			powv = powv * 16; 
		}

		//total = total + (digit * pow(16,(len-(i+1))));
		total = total + ( digit * powv );
	}

	*ret = 0;
	return total;
}


static int check_if_ip_conflict(struct prefix_ipv4 *target_ip)
{
	struct interface *ifp = NULL;
    struct listnode *node = NULL;
	struct listnode *cnode = NULL;
    struct connected *co = NULL;

	if(target_ip != NULL)
	{
		if(target_ip->prefix.s_addr == om->ne_ip_static.s_addr)
		{
			if(IS_DEBUG_OSPF_DCN)
			{
				zlog_debug(OSPF_DBG_DCN, " The new ne-ip is same with old ne-ip!\n");
			}
			return 1;
		}
		
		for (ALL_LIST_ELEMENTS_RO (om->iflist, node, ifp))
	    {
	        for (ALL_LIST_ELEMENTS_RO (ifp->connected, cnode, co))
		    {
		    	if(IS_DEBUG_OSPF_DCN)
		    	{
					zlog_debug(OSPF_DBG_DCN, "IN FOR() ifp->name = %s\n", ifp->name);
		    		zlog_debug(OSPF_DBG_DCN, "IN FOR() co->address = %s/%d\n", inet_ntoa(co->address->u.prefix4), co->address->prefixlen);
					zlog_debug(OSPF_DBG_DCN, "IN FOR() target_ip = %s/%d\n", inet_ntoa(target_ip->prefix), target_ip->prefixlen);
		    	}

				if((prefix_match(co->address, (struct prefix *)target_ip))\
					|| (prefix_match((struct prefix *)target_ip, co->address)))
				{
					if((co->address->u.prefix4.s_addr != 0) && (target_ip->prefix.s_addr != 0))
					{
						if(IS_DEBUG_OSPF_DCN)
						{
							zlog_debug(OSPF_DBG_DCN, "prefix_match ret = 1\n");
						}
						return 1;
					}
				}
		    }
	    }
	}
	return 0;
}

/**
 * @brief      : 检查 ip 与掩码组合的合法性
 * @param[in ] : ip   - ip 地址
 * @param[in ] : mask - 掩码
 * @param[out] :
 * @return     : 合法返回 1，否则返回 0
 * @author     : Zhangzl
 * @date       : 2018年7月18日 14:27:38
 * @note       :
 */
static int check_valid_ip_mask(int ip, int mask)
{
    struct prefix_ipv4 ip_old;
    struct prefix_ipv4 ip_new;
    struct prefix_ipv4 ip_check;
    struct in_addr     mask_addr;

    ip_old.prefix.s_addr = ip;
    ip_old.prefixlen     = mask;
    ip_new.prefix.s_addr = ip;
    ip_new.prefixlen     = mask;

    apply_mask_ipv4(&ip_new);

    /* 网络号为 0，非法 ip */
    if (ip_new.prefix.s_addr == 0)
    {
        return FALSE;
    }

    /* 主机号为 0，非法 ip */
    if (ip_new.prefix.s_addr == ip_old.prefix.s_addr)
    {
        return FALSE;
    }

    /* 主机号全为 1，非法 ip */
    ip_check.prefix.s_addr = ip;
    ip_check.prefixlen     = mask;

    masklen2ip(ip_check.prefixlen, &mask_addr);
    ip_check.prefix.s_addr &= ~mask_addr.s_addr;

    if (ip_check.prefix.s_addr == ~mask_addr.s_addr)
    {
        return FALSE;
    }

    return TRUE;
}

static uint32_t creat_target_if_by_if_name(const char *type, const char *if_name)
{	
	int ret = 0;
	int if_type = 0;
	uint32_t ifindex = 0;

	ifindex = ifm_get_ifindex_by_name ( type, if_name);
	if ( ifindex == 0 )
	{
		OSPF_LOG_DEBUG("Get %s %s ifindex fail!!  \n", type, if_name);
		return 0;
	}

	if_type = ifm_get_typenum ( ( char * ) type );
	//Notify IFM to create target loopback.
	ret = ipc_send_common_wait_ack_n ( NULL, 0, 1 , MODULE_ID_IFM, MODULE_ID_OSPF,
                                      IPC_TYPE_IFM, if_type, IPC_OPCODE_ADD, ifindex );
	if ( ret < 0 )
	{
		return 0;
	}
	return ifindex;
}


/*********************************************************************/
DEFUN (dcn_u0_test,
		dcn_u0_test_cmd,
		"u0 add A.B.C.D ifindex HEX<0x1000001-0xFFFFFFFF>",
		"device name\n"
		"Add action\n"
		"New device ne-ip\n"
		"U0 ifindex\n"
		"ifindex\n")
{
	u_int32_t u0_ifindex = 0;
	struct in_addr new_ne_ip;
	struct u0_device_info u0_temp;
	
	memset(&u0_temp, 0, sizeof(struct u0_device_info));

	VTY_GET_IPV4_ADDRESS("New U0 device", new_ne_ip, argv[0]);
	OSPF_GET_INTEGER_RANGE_HEX("u0 ifindex", u0_ifindex, (char *)argv[1], 16777217, 805306368);

	u0_temp.ne_ip.s_addr = new_ne_ip.s_addr;
	u0_temp.ne_id.s_addr = (new_ne_ip.s_addr & htonl(0xffffff));
	memcpy(u0_temp.dcn_ne_device_type, "HT201", strlen("HT201"));
	memcpy(u0_temp.dcn_ne_vendor, "HUAHUAN", strlen("HUAHUAN"));
	u0_temp.mac[0] = 0x00;
	u0_temp.mac[1] = 0x1d;
	u0_temp.mac[2] = 0x80;
	u0_temp.mac[3] = ((ntohl(new_ne_ip.s_addr) >> 16) & 0xff);
	u0_temp.mac[4] = ((ntohl(new_ne_ip.s_addr) >> 8) & 0xff);
	u0_temp.mac[5] = (ntohl(new_ne_ip.s_addr) & 0xff);
	u0_temp.ifindex = u0_ifindex;

	u0_device_add_handle(&u0_temp);
	
	return TRUE;
}

DEFUN (dcn_u0_test_show,
		dcn_u0_test_show_cmd,
		"show u0",
		"Show running system information\n"
		"U0 device info\n")
{
	uint32_t ne_count = 0;
	struct listnode *node = NULL;
    char dcn_ne_mac_temp[32] = "";
	struct u0_device_info_local *u0_device = NULL;

	vty_out(vty, "%s          U0 Device Elements Information%s", VTY_NEWLINE, VTY_NEWLINE);
	
	if(listcount(om->u0_list) > 0)
	{
		vty_out(vty, "%s%-10s %-15s %-18s %-18s %-10s%s",
                    VTY_NEWLINE, "NE ID", "NE IP", "Device Type", "NE Mac", "Interface", VTY_NEWLINE);
		for (ALL_LIST_ELEMENTS_RO(om->u0_list, node, u0_device))
		{
			ne_count++;
            vty_out(vty, "0x%-8x ", ntohl(u0_device->u0_info.ne_id.s_addr));
            vty_out(vty, "%-15s ", inet_ntoa(u0_device->u0_info.ne_ip));
            vty_out(vty, "%-18s ", u0_device->u0_info.dcn_ne_device_type);
            memset(dcn_ne_mac_temp, 0, sizeof(dcn_ne_mac_temp));
            sprintf(dcn_ne_mac_temp, "%02X:%02X:%02X:%02X:%02X:%02X", u0_device->u0_info.mac[0], u0_device->u0_info.mac[1],
                    u0_device->u0_info.mac[2], u0_device->u0_info.mac[3], u0_device->u0_info.mac[4], u0_device->u0_info.mac[5]);
            vty_out(vty, "%-18s %s", dcn_ne_mac_temp, VTY_NEWLINE);
			vty_out(vty, "0x%-10x %s", u0_device->u0_info.ifindex, VTY_NEWLINE);
		}
		vty_out(vty, "%su0 count:%d%s", VTY_NEWLINE, ne_count, VTY_NEWLINE);
	}
	return TRUE;
}
		
/*********************************************************************/
DEFUN (dcn_ospf,
		dcn_ospf_cmd,
		"dcn",
		"Enter Data Communication Network and enable\n")
{
	int ret = 0;
	int ospf_id = 0;
	int ifindex = 0;
	uint16_t vpnid = 0;
	struct in_addr area_id;
	struct prefix_ipv4 p;
	struct ifm_l3 l3if;
	struct ospf *ospf = NULL;

	uint16_t l3vpn_id = 0;


	ospf_id = OSPF_MAX_PROCESS_ID;
	vpnid = OSPF_DCN_VRF_ID;

	//First: check DCN_VPN whether exist.
	if (vty->config_read_flag != 1)
    {
        l3vpn_id = mpls_com_get_l3vpn_instance2(vpnid, MODULE_ID_OSPF);
        if (0 == l3vpn_id)
        {
            vty_error_out(vty, "The DCN_VPN %d does not exist.%s", vpnid, VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
	
	//Second: creat ospf instance and bind to DCN_VPN 
	ospf = ospf_lookup_id(ospf_id);
	if(NULL == ospf)
	{
	    ospf = ospf_get(ospf_id);
		if(ospf == NULL)
	    {
	        vty_error_out(vty,"DCN fails to enable because OSPF instance 255 creat fail.%s", VTY_NEWLINE);
	        return CMD_WARNING;
	    }
		ospf->vpn = OSPF_DCN_VRF_ID;
	}
	else if(OSPF_DCN_VRF_ID != ospf->vpn)
	{
		vty_error_out(vty, "The Instance ID 255 used for DCN is not bound to vpn instance id 127.%s", VTY_NEWLINE);
        return CMD_WARNING;
	}

	if(ospf->dcn_enable != OSPF_DCN_ENABLE)
    {
	    ospf->dcn_enable = OSPF_DCN_ENABLE;
		
		/* Turn on the "master switch" of opaque-lsa capability. */
	    if (!CHECK_FLAG (ospf->config, OSPF_OPAQUE_CAPABLE))
	    {
			if(IS_DEBUG_OSPF_DCN)
			{
				zlog_debug(OSPF_DBG_DCN, "%s[%d]: In function '%s': Opaque capability: OFF -> ON\n",\
									__FILE__, __LINE__, __func__);
			}
	        SET_FLAG (ospf->config, OSPF_OPAQUE_CAPABLE);
	        ospf_renegotiate_optional_capabilities (ospf);
	    }

		memset(&p, 0, sizeof(struct prefix_ipv4));
	    p.family = AF_INET;
		if(om->ne_ip_static.s_addr == 0)
		{
	    	p.prefix = om->ne_ip_dynamic;
			p.prefixlen = om->ne_ip_dynamic_prefixlen;
		}
		else
		{
			p.prefix = om->ne_ip_static;
			p.prefixlen = om->ne_ip_static_prefixlen;
		}

		//config loop128 ip address
		//if (vty->config_read_flag != 1)
		{
			memset(&l3if, 0, sizeof(struct ifm_l3));
			l3if.vpn = OSPF_DCN_VRF_ID;
			l3if.ipv4_flag = IP_TYPE_STATIC;
			l3if.ipv4[0].addr = ntohl(p.prefix.s_addr);
			l3if.ipv4[0].prefixlen = IPV4_MAX_BITLEN;
			
			//ifindex = ifm_get_ifindex_by_name((char *)"loopback", (char *)"128");
			ifindex = creat_target_if_by_if_name((char *)"loopback", (char *)"128");
			if(0 == ifindex)
			{
				zlog_err("[%s %d]ERROR:  type : %s OR pt_name : %s error!!  \n", __FUNCTION__, __LINE__, "loopback","128");
				return CMD_WARNING;
			}
			ret = ifm_set_l3if(ifindex, &l3if, IFNET_EVENT_IP_ADD, MODULE_ID_OSPF);

			/* modify for Bug #50645: Warning user configuration error.*/
		    if(ret == ERRNO_EXISTED_IP)
		   	{
				/*dcn status send to ifm and lldp*/
				send_dcn_singal_to_target_module(ospf->dcn_enable, MODULE_ID_IFM, IPC_TYPE_IFM, OSPF_DCN_ENABLE, 0);
				send_dcn_singal_to_target_module(ospf->dcn_enable, MODULE_ID_L2, \
												IPC_TYPE_OSPF_DCN, IPC_SUB_TYPE_DCN_STATUS, IPC_OPCODE_UPDATE);
				/* Used to notify EFM to trigger the U0 device add operation.*/
				send_dcn_singal_to_target_module(ospf->dcn_enable, MODULE_ID_L2, \
												IPC_TYPE_EFM, IPC_SUB_TYPE_DCN_STATUS, IPC_OPCODE_ENABLE);
				vty_error_out(vty,"The IP address for ospf DCN have already existed.%s", VTY_NEWLINE);
		        return CMD_WARNING;
		    }
			else if(ret == ERRNO_CONFLICT_IP)
			{
				vty_error_out(vty,"The IP address for ospf DCN conflicts with another address !%s", VTY_NEWLINE);
		        return CMD_WARNING;
			}
			else if(ret == ERRNO_SUBNET_SIMILAR)
			{
				vty_error_out(vty,"The IP address for ospf DCN A similar IP subnet already exists !%s", VTY_NEWLINE);
		        return CMD_WARNING;
			}
			else if(ret == -1)
			{
				vty_error_out(vty,"The IP address for ospf DCN set failed!%s", VTY_NEWLINE);
		        return CMD_WARNING;
			}
		}

		ospf->router_id_static = p.prefix;
	    if((ospf->router_id.s_addr == 0) || (p.prefix.s_addr != ospf->router_id.s_addr))
	    {
	        ospf_router_id_update (ospf);
	    }
	    area_id.s_addr = 0;
	    ospf_network_set (ospf, &p, area_id);

		interface_cmd_config_for_dcn(ospf, &p);
		
	}

	if(IS_DEBUG_OSPF_DCN)
	{
		zlog_debug(OSPF_DBG_DCN, "%s[%d]: In function '%s': hello: DCN node create success.\n",\
									__FILE__, __LINE__, __func__);
	}
	
	vty->node = OSPF_DCN_NODE;
	vty->index = ospf;
	return CMD_SUCCESS;
}


DEFUN (no_ospf_dcn,
       no_dcn_ospf_cmd,
	   "no dcn",
	   NO_STR
	   "OSPF DCN node\n")
{
    struct ospf *ospf = NULL;
	int ospf_id = OSPF_MAX_PROCESS_ID;
	int ifindex = 0;
    struct prefix_ipv4 p;
	struct interface *loop0_ifp = NULL;
	struct listnode *cnode = NULL;
	struct connected *co = NULL;
    struct in_addr area_id;
	struct ifm_l3 l3if;
	char ip_str[255];
	int ret = 0;

	ospf = ospf_lookup_id(ospf_id);
    if(ospf == NULL)
    {
        vty_error_out(vty,"DCN has been disabled or OSPF instance 255 has been deleted.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if(ospf->dcn_enable == OSPF_DCN_DISABLE)
    {
        vty_warning_out(vty,"DCN has been disabled.%s", VTY_NEWLINE);
    
        return CMD_SUCCESS;
    }
    ospf->dcn_enable = OSPF_DCN_DISABLE;
	
	//ifindex = ifm_get_ifindex_by_name((char *)"loopback", (char *)"0");
	ifindex = ifm_get_ifindex_by_name((char *)"loopback", (char *)"128");
	if(0 == ifindex)
	{
		zlog_err("[%s %d]ERROR:  type : %s OR pt_name : %s error!!  \n", __FUNCTION__, __LINE__, "loopback","128");
		return CMD_WARNING;
	}
	loop0_ifp = ospf_find_interface(ifindex);

	memset(&p, 0 ,sizeof(struct prefix_ipv4));
	for (ALL_LIST_ELEMENTS_RO (loop0_ifp->connected, cnode, co))
	{
		p.prefix = co->address->u.prefix4;
		p.prefixlen = co->address->prefixlen;
	}
    p.family = AF_INET;
    
    area_id.s_addr = 0;
    ospf_network_unset (ospf, &p, area_id);
    ospf_area_check_free(ospf,area_id);

	memset(&l3if, 0, sizeof(struct ifm_l3));
	l3if.vpn = OSPF_DCN_VRF_ID;
	l3if.ipv4_flag = IP_TYPE_STATIC;
	l3if.ipv4[0].addr = ntohl(p.prefix.s_addr);
	l3if.ipv4[0].prefixlen = IPV4_MAX_BITLEN;
	if(IS_DEBUG_OSPF_DCN)
	{
		OSPF_LOG_DEBUG("Notify route delete loop128 address is:%s\n", inet_ntoa(p.prefix));
	}

	ret = ifm_set_l3if(ifindex, &l3if, IFNET_EVENT_IP_DELETE, MODULE_ID_OSPF);
	if(ret == -1)
	{
		vty_error_out(vty,"The IP address for ospf DCN set failed!%s", VTY_NEWLINE);
	}

	interface_cmd_config_for_dcn(ospf, &p);

	ospf_finish (ospf);

	//when disable dcn, reset dcn static data
	om->ne_ip_static.s_addr = 0;
	om->ne_ip_static_prefixlen = 0;
	om->ne_id_static.s_addr = 0;

	memset (ip_str, 0, sizeof (ip_str));
	sprintf(ip_str, "%d.%d.%d.%d/32", OSPF_DCN_NE_IP_FREFIX, om->mac[3], om->mac[4],om->mac[5]);
	
	memset(&p, 0 ,sizeof(struct prefix_ipv4));
	str2prefix_ipv4 (ip_str, &p);
	om->ne_ip_dynamic.s_addr = p.prefix.s_addr;
	om->ne_ip_dynamic_prefixlen = p.prefixlen;
		
    return CMD_SUCCESS;
}


ALIAS (no_ospf_dcn,
	  undo_dcn_ospf_cmd,
	  "undo dcn",
	  "Cancel current setting\n"
	  "OSPF DCN node\n")



DEFUN (dcn_ne_id_set,
  dcn_ne_id_set_cmd,
  "ne-id HEX<0x10001-0xFEFFFE>",
  "Set NE ID\n"
  "Net-Element ID value, ne-id=subnetid<<16+baseid,subnetid:0x01~0xfe, baseid:0x0001~0xfffe. SAMPLE: 90001\n")
{
	int ifindex = 0;
	struct ifm_l3 l3if;
	struct in_addr area_id;
	u_int32_t ne_id = 0;
	u_int32_t ne_ip_by_ne_id = 0;
	char ip_temp[32] = "";
	struct prefix_ipv4 p;
	struct prefix_ipv4 temp_p;
	struct ospf *ospf = NULL;
	struct listnode *cnode = NULL;
	struct connected *co = NULL;
	struct u0_device_info_local *u0_info_local = NULL;
	struct u0_device_info_local u0_info_temp;
	int ospf_id = OSPF_MAX_PROCESS_ID;
	struct interface *loop0_ifp = NULL;
	int ret = 0;


	OSPF_GET_INTEGER_RANGE_HEX("dcn ne-id", ne_id, (char *)argv[0], 65537, 16711678);

	ne_ip_by_ne_id = ne_id + (0x88 << 24);

	memset(&p, 0, sizeof(struct prefix_ipv4));
	p.family = AF_INET;
	p.prefix.s_addr = htonl(ne_ip_by_ne_id);
	p.prefixlen = 32;

	//check ip valid
	if (p.prefixlen == 32)
    {
        ret = inet_valid_network(htonl(p.prefix.s_addr));
        if ((!ret) || (p.prefix.s_addr == 0))
        {
            vty_error_out(vty, "The ne-id generate NE IP address is invalid.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
	}
	else
	{
		ret = inet_valid_ipv4(htonl(p.prefix.s_addr));
	    if (!ret)
	    {
	        vty_error_out(vty, "The ne-id generate NE IP is invalid.%s", VTY_NEWLINE);
	        return CMD_WARNING;
	    }
		
		ret = check_valid_ip_mask(htonl(p.prefix.s_addr), p.prefixlen);
		if (!ret)
	    {
	        vty_error_out(vty, "The ne-id generate NE IP is invalid.%s", VTY_NEWLINE);
	        return CMD_WARNING;
	    }
	}
	
	if((htonl(ne_id) == om->ne_id_static.s_addr) || check_if_ip_conflict(&p))
	{
		vty_error_out(vty,"The ne-id generate IP conflict with IP address existed OR repeat config.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	ospf = ospf_lookup_id(ospf_id);
	if(ospf == NULL || ospf->dcn_enable != OSPF_DCN_ENABLE)
	{
		vty_error_out(vty,"DCN is not enabled or OSPF instance 255 has been deleted.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(OSPF_DCN_VRF_ID != ospf->vpn)
	{
		vty_error_out(vty, "The Instance ID 255 used for DCN is not bound to vpn instance id 127.%s", VTY_NEWLINE);
        return CMD_WARNING;
	}

	memset(ip_temp, 0, sizeof(ip_temp));
	OSPF_LOG_DEBUG("Current vty session target ip:%s ", inet_ipv4tostr(vty->server_ip, ip_temp));
	//Check if it is a command to configure U0 by comparing login ip.
	u0_info_local = u0_repeat_detect_by_ne_ip(htonl(vty->server_ip));
	if(u0_info_local)
	{	
		memset(&u0_info_temp, 0, sizeof(struct u0_device_info_local));
		memcpy(&u0_info_temp, u0_info_local, sizeof(struct u0_device_info_local));
		//u0_info_temp.u0_info.ne_ip = p.prefix;
		/* For U0, ne-id and ne-ip are not modified synchronously.*/
		u0_info_temp.u0_info.ne_id.s_addr = htonl(ne_id);
		u0_info_temp.u0_info.ifindex = u0_info_temp.u0_info.ifindex - 0xffe;
		ret = u0_device_update_handle(&(u0_info_temp.u0_info));
		if(ret < 0)
		{
			vty_error_out(vty, "Modify ne-ip fail!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		
		u0_map_info_send_to_target_process(&(u0_info_temp.u0_info), MODULE_ID_L2, IPC_OPCODE_UPDATE);
		return CMD_SUCCESS;
	}
	
	om->ne_id_static.s_addr = htonl(ne_id);
	if(IS_DEBUG_OSPF_DCN)
	{
		OSPF_LOG_DEBUG("om->ne_id_static = %s\n", inet_ntoa(om->ne_id_static));
	}

	if(om->ne_ip_static.s_addr == 0)
	{
		om->ne_ip_dynamic.s_addr = htonl(ne_ip_by_ne_id);
		om->ne_ip_dynamic_prefixlen = 32;
	}
	if(IS_DEBUG_OSPF_DCN)
	{
		OSPF_LOG_DEBUG("ne ip by ne id = %s\n", inet_ntoa(om->ne_ip_dynamic));
	}
  
	memset(&p, 0, sizeof(struct prefix_ipv4));
    p.family = AF_INET;
	if(om->ne_ip_static.s_addr == 0)
	{
    	p.prefix = om->ne_ip_dynamic;
		p.prefixlen = om->ne_ip_dynamic_prefixlen;
	}
	else
	{
		p.prefix = om->ne_ip_static;
		p.prefixlen = om->ne_ip_static_prefixlen;
	}

	if(om->ne_ip_static.s_addr == 0)
	{
		ifindex = ifm_get_ifindex_by_name((char *)"loopback", (char *)"128");
		if(0 == ifindex)
		{
			zlog_err("[%s %d]ERROR:  type : %s OR pt_name : %s error!!  \n", __FUNCTION__, __LINE__, "loopback","128");
			return CMD_WARNING;
		}
		loop0_ifp = ospf_find_interface(ifindex);

		memset(&temp_p, 0 ,sizeof(struct prefix_ipv4));
		for (ALL_LIST_ELEMENTS_RO (loop0_ifp->connected, cnode, co))
		{
			temp_p.prefix = co->address->u.prefix4;
			temp_p.prefixlen = co->address->prefixlen;
		}
	    temp_p.family = AF_INET;
		
		//delete loop128 old ip address
		memset(&l3if, 0, sizeof(struct ifm_l3));
		l3if.vpn = OSPF_DCN_VRF_ID;
		l3if.ipv4_flag = IP_TYPE_STATIC;
		l3if.ipv4[0].addr = ntohl(temp_p.prefix.s_addr);
		l3if.ipv4[0].prefixlen = IPV4_MAX_BITLEN;
		if(IS_DEBUG_OSPF_DCN)
		{
			OSPF_LOG_DEBUG("Notify route delete loop128 address is:%s\n", inet_ntoa(temp_p.prefix));
		}
		ret = ifm_set_l3if(ifindex, &l3if, IFNET_EVENT_IP_DELETE, MODULE_ID_OSPF);
		if(ret == -1)
		{
			vty_error_out(vty,"The IP address for ospf DCN set failed!%s", VTY_NEWLINE);
		}

		//add loop128 new ip address
		memset(&l3if, 0, sizeof(struct ifm_l3));
		l3if.vpn = OSPF_DCN_VRF_ID;
		l3if.ipv4_flag = IP_TYPE_STATIC;
		l3if.ipv4[0].addr = ntohl(p.prefix.s_addr);
		l3if.ipv4[0].prefixlen = IPV4_MAX_BITLEN;

		ret = ifm_set_l3if(ifindex, &l3if, IFNET_EVENT_IP_ADD, MODULE_ID_OSPF);
	    if(ret == ERRNO_EXISTED_IP)
	   	{
			vty_error_out(vty,"The IP address for ospf DCN have already existed.%s", VTY_NEWLINE);
	        return CMD_WARNING;
	    }
		else if(ret == ERRNO_CONFLICT_IP)
		{
			vty_error_out(vty,"The IP address for ospf DCN conflicts with another address !%s", VTY_NEWLINE);
	        return CMD_WARNING;
		}
		else if(ret == ERRNO_SUBNET_SIMILAR)
		{
			vty_error_out(vty,"The IP address for ospf DCN A similar IP subnet already exists !%s", VTY_NEWLINE);
	        return CMD_WARNING;
		}
		else if(ret == -1)
		{
			vty_error_out(vty,"The IP address for ospf DCN set failed!%s", VTY_NEWLINE);
	        return CMD_WARNING;
		}

		/*send update ne-ip ro snmpd*/
		//send_ne_ip_singal_to_snmpd(ntohl(p.prefix.s_addr), IPC_OPCODE_UPDATE);
		
		area_id.s_addr = 0;
		//clean old network
	    ospf_network_unset (ospf, &temp_p, area_id);
	    ospf_network_set (ospf, &p, area_id);
		
		ospf->router_id_static = p.prefix;
	  	ospf_router_id_update (ospf);

	}
	
	ospf_reset(ospf);
	

	return CMD_SUCCESS;
}



DEFUN (dcn_ne_ip_set,
	dcn_ne_ip_set_cmd,
	"ne-ip A.B.C.D/M",
	"Set NE IP address\n"
	"OSPF network prefix\n")
{
	int ifindex = 0;
	u_char mask_len = 0;
	char ip_temp[32] = "";
	struct ifm_l3 l3if;
	struct in_addr area_id;
	struct in_addr temp_mask;
	struct ospf *ospf = NULL;
	struct prefix_ipv4 p;
	struct prefix_ipv4 temp_p;
	struct prefix_ipv4 old_network;
	struct listnode *cnode = NULL;
	struct connected *co = NULL;
	struct interface *loop0_ifp = NULL;
	struct u0_device_info_local *u0_info_local = NULL;
	struct u0_device_info_local u0_info_temp;
	int ospf_id = OSPF_MAX_PROCESS_ID;
	int ret = 0;

	/* Get ne-ip prefix. */
	memset(&p, 0, sizeof(struct prefix_ipv4));
	if(argc <= 1)
	{
		VTY_GET_IPV4_PREFIX ("ne-ip prefix", p, argv[0]);
	}
	else
	{
		//get ip
	    VTY_CHECK_OSPF_ADDRESS("Ne-ip", argv[0]);			/*check A.B.C.D*/
	    VTY_GET_IPV4_ADDRESS("Ne-ip", p.prefix, argv[0]);	/* Change route_id format */
		
		//get mask
		if(strlen(argv[1]) > 2)//if A.B.C.D format
		{
			ret = inet_aton(argv[1], &temp_mask);
			if (! ret)
		    {
		        vty_error_out(vty, "Please specify valid netmask as a.b.c.d or 0~32%s",
		                VTY_NEWLINE);
		        return CMD_WARNING;
		    }
			mask_len = ip_masklen(temp_mask);
		}
		else//0~32 format
		{
			VTY_GET_INTEGER_RANGE("ne-ip netmask", mask_len, argv[1], 0, IPV4_MAX_PREFIXLEN);
		}
		
		if(mask_len > IPV4_MAX_PREFIXLEN)
		{
			vty_error_out(vty, "Please specify valid netmask length.%s",
                VTY_NEWLINE);
        	return CMD_WARNING;
		}
		p.prefixlen = mask_len;
		p.family = AF_INET;
	}
	
	//check ip valid
	if (p.prefixlen == 32)
    {
        ret = inet_valid_network(htonl(p.prefix.s_addr));
        if ((!ret) || (p.prefix.s_addr == 0))
        {
            vty_error_out(vty, "The specified NE IP address is invalid.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
	}
	else
	{
		ret = inet_valid_ipv4(htonl(p.prefix.s_addr));
	    if (!ret)
	    {
	        vty_error_out(vty, "The NE IP address is invalid.%s", VTY_NEWLINE);
	        return CMD_WARNING;
	    }
		
		ret = check_valid_ip_mask(htonl(p.prefix.s_addr), p.prefixlen);
		if (!ret)
	    {
	        vty_error_out(vty, "The NE IP address is invalid.%s", VTY_NEWLINE);
	        return CMD_WARNING;
	    }
	}
	
	if(check_if_ip_conflict(&p))
	{
		vty_error_out(vty,"The ne-ip conflict with IP address existed OR repeat config.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	ospf = ospf_lookup_id(ospf_id);
	if(ospf == NULL || ospf->dcn_enable != OSPF_DCN_ENABLE)
	{
		vty_error_out(vty,"DCN is not enabled or OSPF instance 255 has been deleted.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(OSPF_DCN_VRF_ID != ospf->vpn)
	{
		vty_error_out(vty, "The Instance ID 255 used for DCN is not bound to vpn instance id 127.%s", VTY_NEWLINE);
        return CMD_WARNING;
	}

	memset(ip_temp, 0, sizeof(ip_temp));
	OSPF_LOG_DEBUG("Current vty session target ip:%s ", inet_ipv4tostr(vty->server_ip, ip_temp));

	//printf("%s[%d]: #####################session ip:0x%x  ----htonl:0x%x\n", __FUNCTION__, __LINE__, vty->server_ip, htonl(vty->server_ip));
	//Check if it is a command to configure U0 by comparing login ip.
	u0_info_local = u0_repeat_detect_by_ne_ip(htonl(vty->server_ip));
	if(u0_info_local)
	{	
		//printf("%s[%d]: #####################U0 handle\n", __FUNCTION__, __LINE__);
		memset(&u0_info_temp, 0, sizeof(struct u0_device_info_local));
		memcpy(&u0_info_temp, u0_info_local, sizeof(struct u0_device_info_local));
		u0_info_temp.u0_info.ne_ip = p.prefix;
		u0_info_temp.u0_info.ifindex = u0_info_temp.u0_info.ifindex - 0xffe;
		ret = u0_device_update_handle(&(u0_info_temp.u0_info));
		if(ret < 0)
		{
			vty_error_out(vty, "Modify ne-ip fail!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		
		u0_map_info_send_to_target_process(&(u0_info_temp.u0_info), MODULE_ID_L2, IPC_OPCODE_UPDATE);
		return CMD_SUCCESS;
	}
	
	//To prevent the netmask of ne-ip is not 32, Can not clear the original network problem.
	old_network.family = AF_INET;
	old_network.prefix = om->ne_ip_static;
	old_network.prefixlen = om->ne_ip_static_prefixlen;
	
	om->ne_ip_static = p.prefix;
	om->ne_ip_static_prefixlen = p.prefixlen;
	
	ifindex = ifm_get_ifindex_by_name((char *)"loopback", (char *)"128");
	if(0 == ifindex)
	{
		zlog_err("[%s %d]ERROR:  type : %s OR pt_name : %s error!!  \n", __FUNCTION__, __LINE__, "loopback","128");
		return CMD_WARNING;
	}
	loop0_ifp = ospf_find_interface(ifindex);

	memset(&temp_p, 0 ,sizeof(struct prefix_ipv4));
	for (ALL_LIST_ELEMENTS_RO (loop0_ifp->connected, cnode, co))
	{
		temp_p.prefix = co->address->u.prefix4;
		temp_p.prefixlen = co->address->prefixlen;
	}
    temp_p.family = AF_INET;
	
	//delete loop128 old ip address
	memset(&l3if, 0, sizeof(struct ifm_l3));
	l3if.vpn = OSPF_DCN_VRF_ID;
	l3if.ipv4_flag = IP_TYPE_STATIC;
	l3if.ipv4[0].addr = ntohl(temp_p.prefix.s_addr);
	l3if.ipv4[0].prefixlen = IPV4_MAX_BITLEN;
	if(IS_DEBUG_OSPF_DCN)
	{
		OSPF_LOG_DEBUG("Notify route delete loop128 address is:%s\n", inet_ntoa(temp_p.prefix));
	}
	ret = ifm_set_l3if(ifindex, &l3if, IFNET_EVENT_IP_DELETE, MODULE_ID_OSPF);
	if(ret == -1)
	{
		vty_error_out(vty,"The IP address for ospf DCN set failed!%s", VTY_NEWLINE);
	}

	//add loop128 new ip address
	memset(&l3if, 0, sizeof(struct ifm_l3));
	l3if.vpn = OSPF_DCN_VRF_ID;
	l3if.ipv4_flag = IP_TYPE_STATIC;
	l3if.ipv4[0].addr = ntohl(p.prefix.s_addr);
	l3if.ipv4[0].prefixlen = IPV4_MAX_BITLEN;

	ret = ifm_set_l3if(ifindex, &l3if, IFNET_EVENT_IP_ADD, MODULE_ID_OSPF);
    if(ret == ERRNO_EXISTED_IP)
   	{
		vty_error_out(vty,"The IP address for ospf DCN have already existed.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
	else if(ret == ERRNO_CONFLICT_IP)
	{
		vty_error_out(vty,"The IP address for ospf DCN conflicts with another address !%s", VTY_NEWLINE);
        return CMD_WARNING;
	}
	else if(ret == ERRNO_SUBNET_SIMILAR)
	{
		vty_error_out(vty,"The IP address for ospf DCN A similar IP subnet already exists !%s", VTY_NEWLINE);
        return CMD_WARNING;
	}
	else if(ret == -1)
	{
		vty_error_out(vty,"The IP address for ospf DCN set failed!%s", VTY_NEWLINE);
        return CMD_WARNING;
	}

	/*send update ne-ip ro snmpd*/
	//send_ne_ip_singal_to_snmpd(ntohl(p.prefix.s_addr), IPC_OPCODE_UPDATE);

	area_id.s_addr = 0;
	//clean old network, To prevent the netmask of ne-ip is not 32, clear 2 times.
    ospf_network_unset (ospf, &temp_p, area_id);
	ospf_network_unset (ospf, &old_network, area_id);
    ospf_network_set (ospf, &p, area_id);
	
	ospf->router_id_static = p.prefix;
  	ospf_router_id_update (ospf);

	ospf_reset(ospf);
	
	return CMD_SUCCESS;
}


ALIAS(dcn_ne_ip_set,
	dcn_ne_ip_set_same_h3c_cmd,
	"ne-ip A.B.C.D (<0-32>|A.B.C.D)",
	"Set NE IP address\n"
	"Ne_ip prefix\n"
	"Ne_ip netmask length(0~32)\n"
	"Ne_ip netmask\n")



DEFUN (dcn_trap_report,
       dcn_trap_report_cmd,
       "auto-report",
       "Enable network element info trap-report\n")
{   
    struct ospf *ospf = NULL;
	int ospf_id = OSPF_MAX_PROCESS_ID;
	struct listnode *node = NULL;
	struct ospf_area *area = NULL;
	struct ospf_lsa *lsa = NULL;
	struct route_node *rn = NULL;
	struct ospf_dcn_ne_info s_ospf_dcn_ne_info;
	
	ospf = ospf_lookup_id(ospf_id);
    if(ospf == NULL)
    {
        vty_warning_out(vty,"The dcn is not enable.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
	
    if(ospf->dcn_report == OSPF_DCN_REPORT_ENABLE)
    {
        vty_warning_out(vty,"This DCN have already configured DCN TRAP REPORT.%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    
    ospf->dcn_report = OSPF_DCN_REPORT_ENABLE;

	if(ospf->dcn_enable)
	{
    	for (ALL_LIST_ELEMENTS_RO (ospf->areas, node, area))
		{
			if (ospf_lsdb_count_self (area->lsdb, OSPF_OPAQUE_AREA_LSA) > 0 ||
	        	(ospf_lsdb_count (area->lsdb, OSPF_OPAQUE_AREA_LSA) > 0))
			{
				LSDB_LOOP (OPAQUE_AREA_LSDB(area), rn, lsa)
				{
					memset(&s_ospf_dcn_ne_info, 0, sizeof(struct ospf_dcn_ne_info));
					if(!get_dcn_ne_info_from_tlv(lsa, ospf, &s_ospf_dcn_ne_info))
					{
						ospf_dcn_ne_info_trap(&s_ospf_dcn_ne_info, OSPF_DCN_NE_ONLINE);
					}

				}
			}
		}
	}
    
    return CMD_SUCCESS;
}

DEFUN (no_dcn_trap_report,
       no_dcn_trap_report_cmd,
       "no auto-report",
       NO_STR
       "network element info trap-report\n")
{
    struct ospf *ospf = NULL;
	int ospf_id = OSPF_MAX_PROCESS_ID;
	
 	ospf = ospf_lookup_id(ospf_id);
    if(ospf == NULL)
    {
        vty_warning_out(vty,"The DCN is not enable.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
	
    if(ospf->dcn_report == OSPF_DCN_REPORT_DISABLE)
    {
        vty_warning_out(vty,"The DCN is not configured DCN TRAP REPORT.%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    ospf->dcn_report = OSPF_DCN_REPORT_DISABLE;
	
    return CMD_SUCCESS;
}

ALIAS (no_dcn_trap_report,
       undo_dcn_trap_report_cmd,
       "undo auto-report",
       "Cancel current setting\n"
       "network element info trap-report\n")


DEFUN (show_ip_ospf_dcn,
       show_ip_ospf_dcn_cmd,
       "show dcn (self|ne-info)",
       SHOW_STR
       "OSPF DCN\n"
       "self DCN brief information\n"
       "ne information")
{
    struct ospf *ospf;
    int ospf_id = OSPF_MAX_PROCESS_ID;
    struct route_node *rn;
	struct ospf_lsa *lsa;
	struct ospf_area *area;
	struct listnode *node;
	struct ospf_dcn_ne_info ne_info;
	//char dcn_ne_id_temp[32] = "";
	char dcn_ne_ip_temp[32] = "";
	char dcn_ne_mac_temp[32] = "";
	int ne_count = 0;

    ospf = ospf_lookup_id(ospf_id);
    if(ospf == NULL)
    {
        vty_warning_out(vty, "No DCN NE information now.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (ospf->dcn_enable == OSPF_DCN_DISABLE)
    {
        vty_warning_out(vty, "The DCN is not enable.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }



    if (strncmp(argv[0], "se", 2) == 0)
    {
        vty_out(vty, "%s          DCN Brief Information%s", VTY_NEWLINE, VTY_NEWLINE);
		
        vty_out(vty, "NE ID           : 0x%06x%s", (om->ne_id_static.s_addr == 0) \
        											? ntohl(om->ne_id_dynamic.s_addr) \
        											: ntohl(om->ne_id_static.s_addr),  VTY_NEWLINE);

        vty_out(vty, "NE IP           : %s/%d%s", (om->ne_ip_static.s_addr == 0) \
        										? inet_ntoa(om->ne_ip_dynamic) \
        										: inet_ntoa(om->ne_ip_static), \
        										(om->ne_ip_static.s_addr == 0) \
        										? om->ne_ip_dynamic_prefixlen \
        										: om->ne_ip_static_prefixlen, VTY_NEWLINE);
        vty_out(vty, "DCN interface   : %s%s", "loopback 128",  VTY_NEWLINE);
        vty_out(vty, "DCN trap report : %s%s", (ospf->dcn_report ? "enable" : "disable"),  VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "%s          DCN Network Elements Information%s", VTY_NEWLINE, VTY_NEWLINE);

        for (ALL_LIST_ELEMENTS_RO(ospf->areas, node, area))
        {
            vty_out(vty, "%s                 Area %s",
                    VTY_NEWLINE, ospf_area_desc_string(area));
            vty_out(vty, "%s%-18s %-16s %-20s %-18s %s",
                    VTY_NEWLINE, "NE ID", "NE IP", "Device Type", "NE Mac", VTY_NEWLINE);

            if (ospf_lsdb_count_self(area->lsdb, OSPF_OPAQUE_AREA_LSA) > 0 ||
                    (ospf_lsdb_count(area->lsdb, OSPF_OPAQUE_AREA_LSA) > 0))
            {
                LSDB_LOOP(OPAQUE_AREA_LSDB(area), rn, lsa)
                {
                    memset(&ne_info, 0, sizeof(ne_info));

                    if (!get_dcn_ne_info_from_tlv(lsa, ospf, &ne_info))
                    {
                        ne_count++;
                        //vty_out(vty, "%-16s ", inet_ipv4tostr(htonl(ne_info.dcn_ne_id), dcn_ne_id_temp));
                        vty_out(vty, "0x%-16x ", ntohl(ne_info.dcn_ne_id));
                        vty_out(vty, "%-16s ", inet_ipv4tostr(htonl(ne_info.dcn_ne_ip), dcn_ne_ip_temp));
                        //vty_out (vty, "%-6d ", ne_info.dcn_ne_metric);
                        vty_out(vty, "%-20s ", ne_info.dcn_ne_device_type);
                        memset(dcn_ne_mac_temp, 0, sizeof(dcn_ne_mac_temp));
                        sprintf(dcn_ne_mac_temp, "%02X:%02X:%02X:%02X:%02X:%02X", ne_info.mac[0], ne_info.mac[1],
                                ne_info.mac[2], ne_info.mac[3], ne_info.mac[4], ne_info.mac[5]);
                        vty_out(vty, "%-18s %s", dcn_ne_mac_temp, VTY_NEWLINE);
                    }
                }
            }
        }

        vty_out(vty, "%sTotal number :%d %s", VTY_NEWLINE, ne_count, VTY_NEWLINE);
    }


    return CMD_SUCCESS;

}

ALIAS (show_ip_ospf_dcn,
	  display_ip_ospf_dcn_cmd,
	  "display dcn (self|ne-info)",
	  SHOW_STR
	  "OSPF DCN\n"
	  "self DCN brief information\n"
	  "ne information")


/********************************* DCN Command Line END ****************************************************/

/********************************* OSPF BFD Command Line START *********************************************/

DEFUN(ospf_bfd,
      ospf_bfd_cmd,
      "ip ospf bfd enable",
      "IP Information\n"
      "OSPF interface commands\n"
      "Enable bfd on this interface\n"
      "Enable BFD\n")
{
    struct ospf_interface * oi = NULL;
    struct interface *ifp = NULL;
    struct route_node *rn = NULL;
    struct ospf_if_params *params = NULL;
    uint32_t ifindex = 0;

    /*??è??ó?ú?÷òy*/
    ifindex = (uint32_t)vty->index;

    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    if (params->bfd_flag)
    {
        vty_warning_out(vty, "This interface have already configured OSPF BFD ENABLE.%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }

	SET_IF_PARAM(params, bfd_flag);
    params->bfd_flag = OSPF_IF_BFD_ENABLE;
	
    for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
    {
        if ((oi = rn->info))
        {
            ospf_bfd_if_update(oi);
        }
    }
	
    return CMD_SUCCESS;
}

ALIAS(ospf_bfd,
	ospf_bfd_h3c_cmd,
	"ospf bfd enable [echo]",
	"OSPF interface commands\n"
	"Specify BFD configuration\n"
	"Enable BFD for OSPF\n"
	"Echo packet mode\n")


DEFUN(no_ospf_bfd,
      no_ospf_bfd_cmd,
      "no ip ospf bfd enable",
      NO_STR
      "IP information\n"
      "OSPF interface commands\n"
      "Enable bfd on this interface\n"
      "Enable BFD\n")
{
    struct ospf_interface * oi = NULL;
    struct interface *ifp = NULL;
    struct route_node *rn = NULL;
    struct ospf_if_params *params = NULL;
    uint32_t ifindex = 0;

    /*??è??ó?ú?÷òy*/
    ifindex = (uint32_t)vty->index;

    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    if (!params->bfd_flag)
    {
        vty_warning_out(vty, "This interface never configured OSPF BFD ENABLE.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	UNSET_IF_PARAM(params, bfd_flag);
    params->bfd_flag = OSPF_IF_BFD_DISABLE;

    for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
    {
        if ((oi = rn->info))
        {
            ospf_bfd_if_update(oi);
        }
    }

    return CMD_SUCCESS;
}


ALIAS(no_ospf_bfd,
	no_ospf_bfd_h3c_cmd,
	"undo ospf bfd enable",
	"Cancel the current setting\n"
	"OSPF interface commands\n"
	"Specify BFD configuration\n"
	"Enable BFD for OSPF\n")
			

DEFUN(ospf_bfd_sess_cc_interval_set,
      ospf_bfd_sess_cc_interval_set_cmd,
      "ip ospf bfd cc-interval (3 | 10 | 20 | 100 | 300 | 1000)",
      "IP information\n"
      "OSPF interface commands\n"
      BFD_STR
      "Set BFD session minimum receive interval\n"
      "3.3ms\n"
      "10ms, default is 10ms\n"
      "20ms\n"
      "100ms\n"
      "300ms\n"
      "1000ms\n")
{
    uint32_t interval = 0;
    uint32_t ifindex = 0;
    struct interface *ifp = NULL;
    struct route_node *rn = NULL;
    struct ospf_interface * oi = NULL;
    struct ospf_if_params *params = NULL;

    ifindex = (uint32_t)vty->index;

    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    /* 更改间隔时间*/
    if ((strncmp(argv[0], "3", 1) == 0) ||
            (strncmp(argv[0], "10", 2) == 0) ||
            (strncmp(argv[0], "20", 2) == 0) ||
            (strncmp(argv[0], "100", 3) == 0) ||
            (strncmp(argv[0], "300", 3) == 0) ||
            (strncmp(argv[0], "1000", 4) == 0))
    {
        interval = (atoi(argv[0])) * 1000;
    }

    SET_IF_PARAM(IF_DEF_PARAMS(ifp), if_bfd_recv_interval);
    IF_DEF_PARAMS(ifp)->if_bfd_recv_interval = interval;

    if (!(params->bfd_flag))
    {
        vty_warning_out(vty, "Please enable BFD on this interface first.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
    {
        if ((oi = rn->info))
        {
            ospf_bfd_modify_session_by_interface(oi);
        }
    }

    return CMD_SUCCESS;
}

DEFUN(ospf_bfd_sess_cc_interval_reset,
      ospf_bfd_sess_cc_interval_reset_cmd,
      "no ip ospf bfd cc-interval",
      NO_STR
      "IP information\n"
      "OSPF interface commands\n"
      BFD_STR
      "Set BFD session minimum receive interval\n")
{
    uint32_t ifindex = 0;
    struct interface *ifp = NULL;
    struct route_node *rn = NULL;
    struct ospf_interface * oi = NULL;
    struct ospf_if_params *params = NULL;

    ifindex = (uint32_t)vty->index;

    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    UNSET_IF_PARAM(IF_DEF_PARAMS(ifp), if_bfd_recv_interval);
    IF_DEF_PARAMS(ifp)->if_bfd_recv_interval = 10 * 1000;//default is 10ms

    if (!(params->bfd_flag))
    {
        vty_warning_out(vty, "This interface has not enable OSPF BFD.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
    {
        if ((oi = rn->info))
        {
            ospf_bfd_modify_session_by_interface(oi);
        }
    }

    return CMD_SUCCESS;
}

DEFUN(ospf_bfd_sess_detect_multiplier_set,
      ospf_bfd_sess_detect_multiplier_set_cmd,
      "ip ospf bfd detect-multiplier <3-10>",
      "IP information\n"
      "OSPF interface commands\n"
      BFD_STR
      "Set BFD session detect multiplier\n"
      "Detect multiplier, default is 3\n")
{
    uint16_t multiplier = 0;
    uint32_t ifindex = 0;
    struct interface *ifp = NULL;
    struct route_node *rn = NULL;
    struct ospf_interface * oi = NULL;
    struct ospf_if_params *params = NULL;

    /*判断输入参数是否合法*/
    VTY_GET_INTEGER_RANGE("detect-multiplier", multiplier, argv[0], 3, 10);

    ifindex = (uint32_t)vty->index;

    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    if (multiplier == IF_DEF_PARAMS(ifp)->if_bfd_detect_multiplier)
    {
        return CMD_SUCCESS;
    }

    SET_IF_PARAM(IF_DEF_PARAMS(ifp), if_bfd_detect_multiplier);
    IF_DEF_PARAMS(ifp)->if_bfd_detect_multiplier = multiplier;


    if (!(params->bfd_flag))
    {
        vty_warning_out(vty, "Please enable BFD on this interface first.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
    {
        if ((oi = rn->info))
        {
            ospf_bfd_modify_session_by_interface(oi);
        }
    }

    return CMD_SUCCESS;
}

DEFUN(ospf_bfd_sess_detect_multiplier_reset,
      ospf_bfd_sess_detect_multiplier_reset_cmd,
      "no ip ospf bfd detect-multiplier",
      NO_STR
      "IP information\n"
      "OSPF interface commands\n"
      BFD_STR
      "Set BFD session detect multiplier\n")
{
    uint32_t ifindex = 0;
    struct interface *ifp = NULL;
    struct route_node *rn = NULL;
    struct ospf_interface * oi = NULL;
    struct ospf_if_params *params = NULL;

    ifindex = (uint32_t)vty->index;

    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    if (IF_DEF_PARAMS(ifp)->if_bfd_detect_multiplier == 3)
    {
        UNSET_IF_PARAM(IF_DEF_PARAMS(ifp), if_bfd_detect_multiplier);
        return CMD_SUCCESS;
    }

    UNSET_IF_PARAM(IF_DEF_PARAMS(ifp), if_bfd_detect_multiplier);
    IF_DEF_PARAMS(ifp)->if_bfd_detect_multiplier = 3; //default is 10ms

    if (!(params->bfd_flag))
    {
        vty_warning_out(vty, "This interface has not enable OSPF BFD.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
    {
        if ((oi = rn->info))
        {
            ospf_bfd_modify_session_by_interface(oi);
        }
    }

    return CMD_SUCCESS;
}

static void
show_ospf_bfd_session_info(struct vty *vty, struct ospf_interface *oi)
{
	if(IS_DEBUG_OSPF_BFD)
	{
		zlog_debug(OSPF_DBG_BFD, "%s[%d]: In function '%s' ", __FILE__, __LINE__, __func__);
	}
    struct route_node *rn = NULL;
    struct ospf_neighbor *nbr = NULL;

    for (rn = route_top(oi->nbrs); rn; rn = route_next(rn))
    {
        if ((nbr = rn->info))
        {
            /* Do not show myself. */
            if (nbr != oi->nbr_self)
            {
                /* Down state is not shown. */
                if (nbr->state != NSM_Down)
                {
                    vty_out(vty, " NeighborID:%-18s ", inet_ntoa(nbr->router_id));
                    vty_out(vty, " AreaID:%-20s ", inet_ntoa(oi->area->area_id));
                    vty_out(vty, " Interface:%-20s %s", oi->ifp->name, VTY_NEWLINE);

                    vty_out(vty, " BFDState:%-20s ", (nbr->bfd_session_info.status == OSPF_BFD_STATUS_UP) ? "up" : "down");
                    vty_out(vty, " rx(ms):%-20d ", nbr->bfd_session_info.cc_interval_recv);
                    vty_out(vty, " tx(ms):%-16d %s", nbr->bfd_session_info.cc_interval_send, VTY_NEWLINE);

                    vty_out(vty, " Multiplier:%-18d ", nbr->bfd_session_info.cc_multiplier);
                    vty_out(vty, " BFD local Dis:%-13d ", nbr->bfd_session_info.local_id);
                    vty_out(vty, " LocalIpAdd:%-20s %s", inet_ntoa(oi->address->u.prefix4), VTY_NEWLINE);

                    vty_out(vty, " RemoteIpAdd:%-17s ", inet_ntoa(nbr->address.u.prefix4));
					vty_out(vty, " Session-id:%-16d ", nbr->bfd_session_info.session_id);
                    vty_out(vty, " Diagnostic Info:%-40s %s%s", ospf_bfd_diag_string[nbr->bfd_session_info.diag], VTY_NEWLINE, VTY_NEWLINE);

                }
            }
        }
    }
}

DEFUN(ospf_bfd_info_show,
      ospf_bfd_info_show_cmd,
      "show ip ospf [<1-255>] bfd ( session | statistics | config )",
      SHOW_STR
      IP_STR
      "OSPF information\n"
      "OSPF instance number\n"
      BFD_STR
      "Display session information\n"
      "Display the statistics information of session\n"
      "Display configuration view information\n")
{
    struct ospf *ospf;
    struct ospf_interface *oi;
    struct listnode *node;
    int ospf_id = OSPF_DEFAULT_PROCESS_ID;

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID);
    }

    ospf = ospf_lookup_id(ospf_id);

    if (ospf == NULL)
    {
        VTY_OSPF_INSTANCE_ERR
    }

    if (strncmp(argv[1], "session", 7) == 0)
    {
        vty_out(vty, "%s		OSPF Instance %d with Router ID %s %s",
                VTY_NEWLINE, ospf_id, inet_ntoa(ospf->router_id), VTY_NEWLINE);

        for (ALL_LIST_ELEMENTS_RO(ospf->oiflist, node, oi))
        {
            if (OSPF_IF_PARAM(oi, bfd_flag))
            {
                vty_out(vty, "%s      Area %s ",
                        VTY_NEWLINE, inet_ntoa(oi->area->area_id));
                vty_out(vty, "interface %s(%s)'s BFD Sessions %s",
                        inet_ntoa(oi->address->u.prefix4), oi->ifp->name, VTY_NEWLINE);

                show_ospf_bfd_session_info(vty, oi);
            }
        }
    }
    else if (strncmp(argv[1], "statistics", 9) == 0)
    {

    }
    else if (strncmp(argv[1], "config", 6) == 0)
    {

    }

    return CMD_SUCCESS;
}

static void display_ospf_bfd_info(struct vty *vty, char **argv, struct ospf *ospf_p)
{
	struct listnode *node = NULL;
	struct ospf_interface *oi = NULL;
	
	if (strncmp(argv[1], "session", 7) == 0)
    {
        vty_out(vty, "%s		OSPF Instance %d with Router ID %s %s",
                VTY_NEWLINE, ospf_p->ospf_id, inet_ntoa(ospf_p->router_id), VTY_NEWLINE);

        for (ALL_LIST_ELEMENTS_RO(ospf_p->oiflist, node, oi))
        {
            if (OSPF_IF_PARAM(oi, bfd_flag))
            {
                vty_out(vty, "%s      Area %s ",
                        VTY_NEWLINE, inet_ntoa(oi->area->area_id));
                vty_out(vty, "interface %s(%s)'s BFD Sessions %s",
                        inet_ntoa(oi->address->u.prefix4), oi->ifp->name, VTY_NEWLINE);

                show_ospf_bfd_session_info(vty, oi);
            }
        }
    }
    else if (strncmp(argv[1], "statistics", 9) == 0)
    {

    }
    else if (strncmp(argv[1], "config", 6) == 0)
    {

    }
	
	return;
}

DEFUN(display_ospf_bfd_info_h3c,
	display_ospf_bfd_info_h3c_cmd,
	"display ospf [ <1-65535> ] bfd ( session | statistics | config )",
	"Display current system information\n"
	"OSPF information\n"
	"OSPF instance number\n"
	BFD_STR
	"Display session information\n"
	"Display the statistics information of session\n"
	"Display configuration view information\n")
{
	struct ospf *ospf = NULL;
    struct listnode *node = NULL;
    int ospf_id = OSPF_DEFAULT_PROCESS_ID;

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID_H3C);
    
	    ospf = ospf_lookup_id(ospf_id);
	    if (ospf == NULL)
	    {
	        VTY_OSPF_INSTANCE_ERR
	    }

		display_ospf_bfd_info(vty, (char **)argv, ospf);
	}
	else
	{
		for (ALL_LIST_ELEMENTS_RO (om->ospf, node, ospf))
		{
			display_ospf_bfd_info(vty, (char **)argv, ospf);
			vty_out(vty, " ***************************************************************************%s",
                        			VTY_NEWLINE);
		}
	}

    return CMD_SUCCESS;
}

/********************************* OSPF BFD Command Line END *********************************************/


DEFUN(ospf_reset_instance,
      ospf_reset_instance_cmd,
      "reset ospf instance <1-255>",
      "Reset operation\n"
      "OSPF information\n"
      "OSPF instance\n"
      "OSPF instance number\n")
{
    struct ospf *ospf = NULL;
    int ospf_id;
    ospf_id = OSPF_DEFAULT_PROCESS_ID;

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID);
    }

    ospf = ospf_lookup_id(ospf_id);

    if (ospf == NULL)
    {
        VTY_OSPF_INSTANCE_ERR
    }

    if (ospf->router_id.s_addr == 0)
    {
        vty_error_out(vty, "The ospf instance fails to obtain a router ID.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ospf_reset(ospf);
    return CMD_SUCCESS;
}


DEFUN(ospf_reset_instance_h3c,
	ospf_reset_instance_h3c_cmd,
	"reset ospf [ <1-65535> ] process [ graceful-restart ]",
	"Reset operation\n"
	"OSPF information\n"
	"OSPF instance number\n"
	"OSPF instance\n"
	"Restart with graceful-restart way\n")
{
	
    int ospf_id;
	struct ospf *ospf = NULL;
	struct listnode *node = NULL;
	
    ospf_id = OSPF_DEFAULT_PROCESS_ID;

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID_H3C);

	    ospf = ospf_lookup_id(ospf_id);
	    if (ospf == NULL)
	    {
	        VTY_OSPF_INSTANCE_ERR
	    }
		
		if (ospf->router_id.s_addr == 0)
	    {
	        vty_error_out(vty, "The ospf instance fails to obtain a router ID.%s", VTY_NEWLINE);
	        return CMD_WARNING;
	    }

   		ospf_reset(ospf);
	}
	else
	{
		for (ALL_LIST_ELEMENTS_RO (om->ospf, node, ospf))
		{
			if (ospf->router_id.s_addr == 0)
		    {
		        vty_error_out(vty, "The ospf instance fails to obtain a router ID.%s", VTY_NEWLINE);
		        return CMD_WARNING;
		    }

	   		ospf_reset(ospf);
		}
	}
    
    return CMD_SUCCESS;
}

static void
ospf_passive_interface_default(struct ospf *ospf, u_char newval)
{
    struct listnode *ln = NULL;
    struct interface *ifp = NULL;
    struct ospf_interface *oi = NULL;
    ospf->passive_interface_default = newval;

    for (ALL_LIST_ELEMENTS_RO(om->iflist, ln, ifp))
    {
        if (ifp &&
                OSPF_IF_PARAM_CONFIGURED(IF_DEF_PARAMS(ifp), passive_interface))
        {
            UNSET_IF_PARAM(IF_DEF_PARAMS(ifp), passive_interface);
        }
    }

    for (ALL_LIST_ELEMENTS_RO(ospf->oiflist, ln, oi))
    {
        if (OSPF_IF_PARAM_CONFIGURED(oi->params, passive_interface))
        {
            UNSET_IF_PARAM(oi->params, passive_interface);
        }

        /* update multicast memberships */
        //ospf_if_set_multicast(oi);
    }
}

static void
ospf_passive_interface_update_addr(struct ospf *ospf, struct interface *ifp,
                                   struct ospf_if_params *params, u_char value,
                                   struct in_addr addr)
{
    u_char dflt;
    params->passive_interface = value;

    if (params != IF_DEF_PARAMS(ifp))
    {
        if (OSPF_IF_PARAM_CONFIGURED(IF_DEF_PARAMS(ifp), passive_interface))
        {
            dflt = IF_DEF_PARAMS(ifp)->passive_interface;
        }
        else
        {
            dflt = ospf->passive_interface_default;
        }

        if (value != dflt)
        {
            SET_IF_PARAM(params, passive_interface);
        }
        else
        {
            UNSET_IF_PARAM(params, passive_interface);
        }

        ospf_free_if_params(ifp, addr);
        ospf_if_update_params(ifp, addr);
    }
}

static void
ospf_passive_interface_update(struct interface *ifp,
                              struct ospf_if_params *params, u_char value)
{
    params->passive_interface = value;

    if (params == IF_DEF_PARAMS(ifp))
    {
        if (value == OSPF_IF_ACTIVE)
        {
            SET_IF_PARAM(params, passive_interface);
        }
        else
        {
            UNSET_IF_PARAM(params, passive_interface);
        }
    }
}

/* get the appropriate ospf parameters structure, checking if
 * there's a valid interface address at the argi'th argv index
 */
enum
{
    VTY_SET = 0,
    VTY_UNSET,
};
#define OSPF_VTY_GET_IF_PARAMS(ifp,params,argi,addr,set) \
  (params) = IF_DEF_PARAMS ((ifp));           \
                                              \
  if (argc == (argi) + 1)                     \
    {                                         \
      int ret = inet_aton(argv[(argi)], &(addr)); \
      if (!ret)                               \
    {                                     \
      vty_warning_out (vty, "Please specify interface address by A.B.C.D%s", \
           VTY_NEWLINE);              \
      return CMD_WARNING;                 \
    }                                     \
      (params) = ospf_get_if_params ((ifp), (addr)); \
                                              \
      if (set)                                \
        ospf_if_update_params ((ifp), (addr));  \
      else if ((params) == NULL)              \
        return CMD_SUCCESS;                   \
    }

#define OSPF_VTY_PARAM_UNSET(params,var,ifp,addr) \
  UNSET_IF_PARAM ((params), var);               \
    if ((params) != IF_DEF_PARAMS ((ifp)))        \
    {                                             \
      ospf_free_if_params ((ifp), (addr));        \
      ospf_if_update_params ((ifp), (addr));      \
    }

DEFUN(ospf_passive_interface,
      ospf_passive_interface_addr_cmd,
      "passive-interface IFNAME A.B.C.D",
      "Suppress routing updates on an interface\n"
      "Interface's name\n")
{
    struct interface *ifp;
    struct in_addr addr;
    int ret;
    struct ospf_if_params *params;
    struct ospf *ospf = vty->index;

    if (argc == 0)
    {
        ospf_passive_interface_default(ospf, OSPF_IF_PASSIVE);
        return CMD_SUCCESS;
    }

    ifp = if_get_by_name(argv[0]);
    params = IF_DEF_PARAMS(ifp);
	
	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (argc == 2)
    {
        ret = inet_aton(argv[1], &addr);

        if (!ret)
        {
            vty_warning_out(vty, "Waring: Please specify interface address by A.B.C.D%s",
                    VTY_NEWLINE);
            return CMD_WARNING;
        }

        params = ospf_get_if_params(ifp, addr);
        ospf_if_update_params(ifp, addr);
        ospf_passive_interface_update_addr(ospf, ifp, params,
                                           OSPF_IF_PASSIVE, addr);
    }

    //ospf_passive_interface_update (ospf, ifp, params, OSPF_IF_PASSIVE);
    /* XXX We should call ospf_if_set_multicast on exactly those
     * interfaces for which the passive property changed.  It is too much
     * work to determine this set, so we do this for every interface.
     * This is safe and reasonable because ospf_if_set_multicast uses a
     * record of joined groups to avoid systems calls if the desired
     * memberships match the current memership.
     */
    return CMD_SUCCESS;
}

ALIAS(ospf_passive_interface,
      ospf_passive_interface_cmd,
      "passive-interface IFNAME",
      "Suppress routing updates on an interface\n"
      "Interface's name\n")

ALIAS(ospf_passive_interface,
      ospf_passive_interface_default_cmd,
      "passive-interface default",
      "Suppress routing updates on an interface\n"
      "Suppress routing updates on interfaces by default\n")

DEFUN(no_ospf_passive_interface,
      no_ospf_passive_interface_addr_cmd,
      "no passive-interface IFNAME A.B.C.D",
      NO_STR
      "Allow routing updates on an interface\n"
      "Interface's name\n")
{
    struct interface *ifp;
    struct in_addr addr;
    struct ospf_if_params *params;
    int ret;
    //struct route_node *rn;
    struct ospf *ospf = vty->index;

    if (argc == 0)
    {
        ospf_passive_interface_default(ospf, OSPF_IF_ACTIVE);
        return CMD_SUCCESS;
    }

    ifp = if_get_by_name(argv[0]);
    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (argc == 2)
    {
        ret = inet_aton(argv[1], &addr);

        if (!ret)
        {
            vty_warning_out(vty, "Please specify interface address by A.B.C.D%s",
                    VTY_NEWLINE);
            return CMD_WARNING;
        }

        params = ospf_lookup_if_params(ifp, addr);

        if (params == NULL)
        {
            return CMD_SUCCESS;
        }

        ospf_passive_interface_update_addr(ospf, ifp, params, OSPF_IF_ACTIVE,
                                           addr);
    }

    //ospf_passive_interface_update (ospf, ifp, params, OSPF_IF_ACTIVE);
    /* XXX We should call ospf_if_set_multicast on exactly those
     * interfaces for which the passive property changed.  It is too much
     * work to determine this set, so we do this for every interface.
     * This is safe and reasonable because ospf_if_set_multicast uses a
     * record of joined groups to avoid systems calls if the desired
     * memberships match the current memership.
     */
    return CMD_SUCCESS;
}

ALIAS(no_ospf_passive_interface,
      no_ospf_passive_interface_cmd,
      "no passive-interface IFNAME",
      NO_STR
      "Allow routing updates on an interface\n"
      "Interface's name\n")

ALIAS(no_ospf_passive_interface,
      no_ospf_passive_interface_default_cmd,
      "no passive-interface default",
      NO_STR
      "Allow routing updates on an interface\n"
      "Allow routing updates on interfaces by default\n")

DEFUN(ospf_network_area,
      ospf_network_area_cmd,
      "network A.B.C.D/M ",
      "Enable routing on an IP network\n"
      "OSPF network prefix\n")
{
    struct ospf *ospf = vty->index;
    struct prefix_ipv4 p;
    struct ospf_area *area = (struct ospf_area *)(vty->index_sub);
    int ret = -1;

    if (!area)
    {
        VTY_CHECK_OSPF_AREA
    }

	memset(&p, 0, sizeof(struct prefix_ipv4));
	
    /* Get network prefix and Area ID. */
    VTY_GET_IPV4_PREFIX("network prefix", p, argv[0]);

    if (ospf->router_id.s_addr == 0)
    {
        vty_warning_out(vty, "Please first config router ID.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ret = ospf_network_set(ospf, &p, area->area_id);

    if (ret == ERRNO_FAIL)
    {
        vty_error_out(vty, "The network statement is already configure other instance.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    else if (ret == ERRNO_EXISTED)
    {
        vty_error_out(vty, "There is already same network statement.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN(ospf_network_area_h3c,
	ospf_network_area_h3c_cmd,
	"network A.B.C.D A.B.C.D",
	"Enable routing on an IP network\n"
	"OSPF network prefix\n"
	"wildcard-mask\n")
{
	struct prefix_ipv4 p;
	struct in_addr temp_mask;
	struct ospf *ospf = vty->index;
	struct ospf_area *area = (struct ospf_area *)(vty->index_sub);
	int ret = -1;

	if (!area)
	{
		VTY_CHECK_OSPF_AREA
	}

	memset(&p, 0, sizeof(struct prefix_ipv4));
	
	//get network ip
    VTY_CHECK_OSPF_ADDRESS("network prefix", argv[0]);			/*check A.B.C.D*/
    VTY_GET_IPV4_ADDRESS("network prefix", p.prefix, argv[0]);	/* Change route_id format */
	
	//get mask
	ret = inet_aton(argv[1], &temp_mask);
	if (! ret)
    {
        vty_error_out(vty, "Please specify valid netmask as A.B.C.D(Note:wildcard-mask)%s",
                VTY_NEWLINE);
        return CMD_WARNING;
    }
	temp_mask.s_addr = ~(temp_mask.s_addr);//turn wildcard mask to normal masks.
	p.prefixlen = ip_masklen(temp_mask);
	zlog_debug(OSPF_DBG_OTHER, "wildcard mask turn to mask:%s, mask length = %d\n", inet_ntoa(temp_mask), p.prefixlen);

	//check router id if config
	if (ospf->router_id.s_addr == 0)
	{
		vty_warning_out(vty, "Please first config router ID.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	//config network
	ret = ospf_network_set(ospf, &p, area->area_id);
	if (ret == ERRNO_FAIL)
	{
		vty_error_out(vty, "The network statement is already configure other instance.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	else if (ret == ERRNO_EXISTED)
	{
		vty_error_out(vty, "There is already same network statement.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}


DEFUN(no_ospf_network_area,
      no_ospf_network_area_cmd,
      "no network A.B.C.D/M ",
      NO_STR
      "Enable routing on an IP network\n"
      "OSPF network prefix\n")
{
    struct ospf *ospf = vty->index;
    struct prefix_ipv4 p;
    int ret;
    struct ospf_area *area = (struct ospf_area *)(vty->index_sub);

    if (!area)
    {
        VTY_CHECK_OSPF_AREA
    }

    /* Get network prefix and Area ID. */
    VTY_GET_IPV4_PREFIX("network prefix", p, argv[0]);
    ret = ospf_network_unset(ospf, &p, area->area_id);

    if (ret == 0)
    {
        vty_error_out(vty, "Can't find specified network area configuration.%s",
                VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(no_ospf_network_area_h3c,
	no_ospf_network_area_h3c_cmd,
	"undo network A.B.C.D A.B.C.D",
	"Clean current config\n"
	"Enable routing on an IP network\n"
	"OSPF network prefix\n"
	"wildcard-mask\n")
{	
	int ret = 0;
	struct prefix_ipv4 p;
	struct in_addr temp_mask;
	struct ospf *ospf = vty->index;
	struct ospf_area *area = (struct ospf_area *)(vty->index_sub);

	if (!area)
	{
		VTY_CHECK_OSPF_AREA
	}

	memset(&p, 0, sizeof(struct prefix_ipv4));
	
	//get network ip
	VTY_CHECK_OSPF_ADDRESS("network prefix", argv[0]);			/*check A.B.C.D*/
	VTY_GET_IPV4_ADDRESS("network prefix", p.prefix, argv[0]);	/* Change route_id format */
	
	//get mask
	ret = inet_aton(argv[1], &temp_mask);
	if (! ret)
	{
		vty_error_out(vty, "Please specify valid netmask as A.B.C.D(Note:wildcard-mask)%s",
				VTY_NEWLINE);
		return CMD_WARNING;
	}
	temp_mask.s_addr = ~(temp_mask.s_addr);//turn wildcard mask to normal masks.
	p.prefixlen = ip_masklen(temp_mask);
	OSPF_LOG_DEBUG("wildcard mask turn to mask:%s, mask length = %d\n", inet_ntoa(temp_mask), p.prefixlen);

	ret = ospf_network_unset(ospf, &p, area->area_id);
	if (ret == 0)
	{
		vty_error_out(vty, "Can't find specified network area configuration.%s",
			  VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}


DEFUN(ospf_area_summary,
      ospf_area_summary_cmd,
      "abr-summary A.B.C.D/M { not-advertise | cost <0-16777214> }",
      "Summarize routes matching address/mask (border routers only)\n"
      "Area summary prefix\n"
      "DoNotAdvertise this range\n"
      "User specified metric for this range\n"
      "Advertised metric for this range\n")
{
    struct ospf *ospf = vty->index;
    struct prefix_ipv4 p;
    u_int32_t cost;
    struct ospf_area *area = (struct ospf_area *)(vty->index_sub);

    if (!area)
    {
        VTY_CHECK_OSPF_AREA
    }

    VTY_GET_IPV4_PREFIX("area range", p, argv[0]);

    if (argv[0] != NULL && argv[1] != NULL)
    {
        ospf_area_range_set(ospf, area->area_id, &p, 0);
    }
    else if (argv[0] != NULL)
    {
        ospf_area_range_set(ospf, area->area_id, &p, OSPF_AREA_RANGE_ADVERTISE);
    }

    if (argv[2] != NULL)
    {
        VTY_GET_INTEGER("summary cost", cost, argv[2]);
        ospf_area_range_cost_set(ospf, area->area_id, &p, cost);
    }
    else
    {
        ospf_area_range_cost_set(ospf, area->area_id, &p, OSPF_AREA_RANGE_COST_UNSPEC);
    }

    return CMD_SUCCESS;
}


DEFUN(ospf_area_summary_h3c,
	ospf_area_summary_h3c_cmd,
	"abr-summary A.B.C.D ( <0-32> | A.B.C.D ) { not-advertise | cost <0-16777214> }",
	"Summarize routes matching address/mask (border routers only)\n"
	"Area summary prefix\n"
	"netmask length(0~32)\n"
	"netmask format: A.B.C.D\n"
	"DoNotAdvertise this range\n"
	"User specified metric for this range\n"
	"Advertised metric for this range\n")
{
	int ret = 0;
	u_int32_t cost;
	struct prefix_ipv4 p;
	u_char mask_len = 0;
	struct in_addr temp_mask;
	struct ospf *ospf = vty->index;
	struct ospf_area *area = (struct ospf_area *)(vty->index_sub);

	if (!area)
	{
		VTY_CHECK_OSPF_AREA
	}

	//get ip
	VTY_CHECK_OSPF_ADDRESS("Summary ip", argv[0]);			/*check A.B.C.D*/
	VTY_GET_IPV4_ADDRESS("Summary ip", p.prefix, argv[0]);	/* Change format */
	
	//get mask
	if(strlen(argv[1]) > 2)//if A.B.C.D format
	{
		ret = inet_aton(argv[1], &temp_mask);
		if (! ret)
		{
			vty_error_out(vty, "Please specify valid netmask as a.b.c.d or 0~32%s",
					VTY_NEWLINE);
			return CMD_WARNING;
		}
		mask_len = ip_masklen(temp_mask);
	}
	else//0~32 format
	{
		VTY_GET_INTEGER_RANGE("Summary ip netmask", mask_len, argv[1], 0, IPV4_MAX_PREFIXLEN);
	}
	
	if(mask_len > IPV4_MAX_PREFIXLEN)
	{
		vty_error_out(vty, "Please specify valid netmask length.%s",
			VTY_NEWLINE);
		return CMD_WARNING;
	}
	p.prefixlen = mask_len;
	p.family = AF_INET;
	

	if (argv[0] != NULL && argv[1] != NULL && argv[2] != NULL)
	{
		OSPF_LOG_DEBUG(" argv[2] = %s\n", argv[2]);
		ospf_area_range_set(ospf, area->area_id, &p, 0);
	}
	else if (argv[0] != NULL && argv[1] != NULL)
	{
		ospf_area_range_set(ospf, area->area_id, &p, OSPF_AREA_RANGE_ADVERTISE);
	}

	if (argv[3] != NULL)
	{
		VTY_GET_INTEGER("summary cost", cost, argv[3]);
		ospf_area_range_cost_set(ospf, area->area_id, &p, cost);
	}
	else
	{
		ospf_area_range_cost_set(ospf, area->area_id, &p, OSPF_AREA_RANGE_COST_UNSPEC);
	}

  return CMD_SUCCESS;
}


DEFUN(ospf_area_summary_format2_h3c,
	ospf_area_summary_format2_h3c_cmd,
	"abr-summary A.B.C.D ( <0-32> | A.B.C.D ) advertise { cost <0-16777214> }",
	"Summarize routes matching address/mask (border routers only)\n"
	"Area summary prefix\n"
	"netmask length(0~32)\n"
	"netmask format: A.B.C.D\n"
	"Advertise this range(default)\n"
	"User specified metric for this range\n"
	"Advertised metric for this range\n")
{
	int ret = 0;
	u_int32_t cost;
	struct prefix_ipv4 p;
	u_char mask_len = 0;
	struct in_addr temp_mask;
	struct ospf *ospf = vty->index;
	struct ospf_area *area = (struct ospf_area *)(vty->index_sub);

	if (!area)
	{
		VTY_CHECK_OSPF_AREA
	}

	//get ip
	VTY_CHECK_OSPF_ADDRESS("Summary ip", argv[0]);			/*check A.B.C.D*/
	VTY_GET_IPV4_ADDRESS("Summary ip", p.prefix, argv[0]);	/* Change format */
	
	//get mask
	if(strlen(argv[1]) > 2)//if A.B.C.D format
	{
		ret = inet_aton(argv[1], &temp_mask);
		if (! ret)
		{
			vty_error_out(vty, "Please specify valid netmask as a.b.c.d or 0~32%s",
					VTY_NEWLINE);
			return CMD_WARNING;
		}
		mask_len = ip_masklen(temp_mask);
	}
	else//0~32 format
	{
		VTY_GET_INTEGER_RANGE("Summary ip netmask", mask_len, argv[1], 0, IPV4_MAX_PREFIXLEN);
	}
	
	if(mask_len > IPV4_MAX_PREFIXLEN)
	{
		vty_error_out(vty, "Please specify valid netmask length.%s",
			VTY_NEWLINE);
		return CMD_WARNING;
	}
	p.prefixlen = mask_len;
	p.family = AF_INET;
	
	if (argv[0] != NULL && argv[1] != NULL)
	{
		ospf_area_range_set(ospf, area->area_id, &p, OSPF_AREA_RANGE_ADVERTISE);
	}

	if (argv[2] != NULL)
	{
		VTY_GET_INTEGER("summary cost", cost, argv[2]);
		ospf_area_range_cost_set(ospf, area->area_id, &p, cost);
	}
	else
	{
		ospf_area_range_cost_set(ospf, area->area_id, &p, OSPF_AREA_RANGE_COST_UNSPEC);
	}

  return CMD_SUCCESS;
}	


DEFUN(no_ospf_area_summary,
      no_ospf_area_summary_cmd,
      "no abr-summary A.B.C.D/M",
      NO_STR
      "Summarize routes matching address/mask (border routers only)\n"
      "Area summary prefix\n")
{
    struct ospf *ospf = vty->index;
    struct prefix_ipv4 p;
    struct ospf_area *area = (struct ospf_area *)(vty->index_sub);

    if (!area)
    {
        VTY_CHECK_OSPF_AREA
    }

    VTY_GET_IPV4_PREFIX("summary", p, argv[0]);
    ospf_area_range_unset(ospf, area->area_id, &p);
    return CMD_SUCCESS;
}



DEFUN(no_ospf_area_summary_h3c,
	no_ospf_area_summary_h3c_cmd,
	"undo abr-summary A.B.C.D ( <0-32> | A.B.C.D )",
	"Cancel the current configuration\n"
	"Summarize routes matching address/mask (border routers only)\n"
	"Area summary prefix\n"
	"netmask length(0~32)\n"
	"netmask format: A.B.C.D\n")
{
	int ret = 0;
	struct prefix_ipv4 p;
	struct ospf *ospf = vty->index;
	struct ospf_area *area = (struct ospf_area *)(vty->index_sub);

	if (!area)
	{
		VTY_CHECK_OSPF_AREA
	}

	//get ip address and netmask
	ret = ip_address_str_format_2_prefix(vty, &p, (char *)argv[0], (char *)argv[1]);
	if(ret != CMD_SUCCESS)
	{
		return CMD_WARNING;
	}

	ospf_area_range_unset(ospf, area->area_id, &p);
	return CMD_SUCCESS;
}


DEFUN(ospf_asbr_summary,
      ospf_asbr_summary_cmd,
      "asbr-summary A.B.C.D/M { not-advertise | cost <0-16777214> }",
      "Summarize routes matching address/mask (AS border routers only)\n"
      "Summary prefix\n"
      "DoNotAdvertise this range\n"
      "User specified metric for this range\n"
      "Advertised metric for this range\n")
{
    struct ospf *ospf = vty->index;
    struct prefix_ipv4 p;
    u_int32_t cost;
    VTY_GET_IPV4_PREFIX("ospf summary", p, argv[0]);

    if (argv[0] != NULL && argv[1] != NULL)
    {
        ospf_summary_set(ospf, &p, 0);
    }
    else if (argv[0] != NULL)
    {
        ospf_summary_set(ospf, &p, OSPF_SUMMARY_ADVERTISE);
    }

    if (argv[2] != NULL)
    {
        VTY_GET_INTEGER("summary cost", cost, argv[2]);
        ospf_summary_cost_set(ospf, &p, cost);
    }
    else
    {
        ospf_summary_cost_set(ospf, &p, OSPF_SUMMARY_COST_UNSPEC);
    }

    return CMD_SUCCESS;
}


DEFUN(ospf_asbr_summary_h3c,
	ospf_asbr_summary_h3c_cmd,
	"asbr-summary A.B.C.D ( <0-32> | A.B.C.D ) { not-advertise | cost <0-16777214> | nssa-only | tag <0-4294967295> }",
	"Summarize routes matching address/mask (AS border routers only)\n"
	"Summary prefix\n"
	"netmask length(0~32)\n"
	"netmask format: A.B.C.D\n"
	"DoNotAdvertise this range\n"
	"User specified metric for this range\n"
	"Advertised metric for this range\n"
	"Forbidden peer device convert Type7-lsa to Type5-lsa\n"
	"Set tag of the aggregation route\n"
	"32-bit tag value\n")
{
	int ret = -1;
	u_int32_t cost;
	struct prefix_ipv4 p;
	struct ospf *ospf = vty->index;

	//get ip address and netmask
	ret = ip_address_str_format_2_prefix(vty, &p, (char *)argv[0], (char *)argv[1]);
	if(ret != CMD_SUCCESS)
	{
		return CMD_WARNING;
	}

	if (argv[0] != NULL && argv[1] != NULL && argv[2] != NULL)
	{
	  ospf_summary_set(ospf, &p, 0);
	}
	else if (argv[0] != NULL)
	{
	  ospf_summary_set(ospf, &p, OSPF_SUMMARY_ADVERTISE);
	}

	if (argv[3] != NULL)
	{
	  VTY_GET_INTEGER("summary cost", cost, argv[3]);
	  ospf_summary_cost_set(ospf, &p, cost);
	}
	else
	{
	  ospf_summary_cost_set(ospf, &p, OSPF_SUMMARY_COST_UNSPEC);
	}

	//argv[4],argv[5]: Not supported for the time being.
	
	return CMD_SUCCESS;
}



DEFUN(no_ospf_asbr_summary,
      no_ospf_asbr_summary_cmd,
      "no asbr-summary A.B.C.D/M ",
      NO_STR
      "Summarize routes matching address/mask (AS border routers only)\n"
      "Summary prefix\n")
{
    struct ospf *ospf = vty->index;
    struct prefix_ipv4 p;
    VTY_GET_IPV4_PREFIX("ospf summary", p, argv[0]);
    ospf_summary_unset(ospf, &p);
    return CMD_SUCCESS;
}

DEFUN(no_ospf_asbr_summary_h3c,
	no_ospf_asbr_summary_h3c_cmd,
	"undo asbr-summary A.B.C.D ( <0-32> | A.B.C.D )",
	"Cancel the current configuration\n"
	"Summarize routes matching address/mask (AS border routers only)\n"
	"Asbr summary prefix\n"
	"Netmask length(0~32)\n"
	"Netmask format: A.B.C.D\n")
{
	int ret = -1;
	struct prefix_ipv4 p;
	struct ospf *ospf = vty->index;
	
	//get ip address and netmask
	ret = ip_address_str_format_2_prefix(vty, &p, (char *)argv[0], (char *)argv[1]);
	if(ret != CMD_SUCCESS)
	{
		return CMD_WARNING;
	}
	ospf_summary_unset(ospf, &p);
	return CMD_SUCCESS;
}


/* Command Handler Logic in VLink stuff is delicate!!

    ALTER AT YOUR OWN RISK!!!!

    Various dummy values are used to represent 'NoChange' state for
    VLink configuration NOT being changed by a VLink command, and
    special syntax is used within the command strings so that the
    typed in command verbs can be seen in the configuration command
    bacckend handler.  This is to drastically reduce the verbeage
    required to coe up with a reasonably compatible Cisco VLink command

    - Matthew Grant <grantma@anathoth.gen.nz>
    Wed, 21 Feb 2001 15:13:52 +1300
 */


/* Configuration data for virtual links
 */
struct ospf_vl_config_data
{
    struct vty *vty;      /* vty stuff */
    struct in_addr area_id;   /* area ID from command line */
    int format;           /* command line area ID format */
    struct in_addr vl_peer;   /* command line vl_peer */
    int auth_type;        /* Authehntication type, if given */
    char *auth_key;       /* simple password if present */
    int crypto_key_id;        /* Cryptographic key ID */
    char *md5_key;        /* MD5 authentication key */
    int hello_interval;           /* Obvious what these are... */
    int retransmit_interval;
    int transmit_delay;
    int dead_interval;
};

static void
ospf_vl_config_data_init(struct ospf_vl_config_data *vl_config,
                         struct vty *vty)
{
    memset(vl_config, 0, sizeof(struct ospf_vl_config_data));
    vl_config->auth_type = OSPF_AUTH_CMD_NOTSEEN;
    vl_config->vty = vty;
}

static struct ospf_vl_data *
ospf_find_vl_data(struct ospf *ospf, struct ospf_vl_config_data *vl_config)
{
	OSPF_LOG_DEBUG();
    struct ospf_area *area;
    struct ospf_vl_data *vl_data;
    struct vty *vty;
    struct in_addr area_id;

    vty = vl_config->vty;
    area_id = vl_config->area_id;

    if (area_id.s_addr == OSPF_AREA_BACKBONE)
    {
        vty_error_out(vty,
                "Configuring VLs over the backbone is not allowed%s",
                VTY_NEWLINE);
        return NULL;
    }

    area = ospf_area_get(ospf, area_id, vl_config->format);

    if (area->external_routing != OSPF_AREA_DEFAULT)
    {
        if (vl_config->format == OSPF_AREA_ID_FORMAT_ADDRESS)
            vty_out(vty, "Area %s is %s%s",
                    inet_ntoa(area_id),
                    area->external_routing == OSPF_AREA_NSSA ? "nssa" : "stub",
                    VTY_NEWLINE);
        else
            vty_out(vty, "Area %ld is %s%s",
                    (u_long)ntohl(area_id.s_addr),
                    area->external_routing == OSPF_AREA_NSSA ? "nssa" : "stub",
                    VTY_NEWLINE);

        return NULL;
    }

    if ((vl_data = ospf_vl_lookup(ospf, area, vl_config->vl_peer)) == NULL)
    {
        vl_data = ospf_vl_data_new(area, vl_config->vl_peer);

        if (vl_data->vl_oi == NULL)
        {
            vl_data->vl_oi = ospf_vl_new(ospf, vl_data);
            ospf_vl_add(ospf, vl_data);
            ospf_spf_calculate_schedule(ospf, SPF_FLAG_CONFIG_CHANGE);
        }
    }

    return vl_data;
}


static int
ospf_vl_set_security(struct ospf_vl_data *vl_data,
                     struct ospf_vl_config_data *vl_config)
{
    struct crypt_key *ck;
    struct vty *vty;
    struct interface *ifp = vl_data->vl_oi->ifp;
    vty = vl_config->vty;

    if (vl_config->auth_type != OSPF_AUTH_CMD_NOTSEEN)
    {
		if((IF_DEF_PARAMS(ifp)->auth_type != vl_config->auth_type)\
			&& (vl_config->auth_type != OSPF_AUTH_NOTSET))
    	{
    		if((IF_DEF_PARAMS(ifp)->auth_type == OSPF_AUTH_SIMPLE))
    		{
    			//clean has existed simple auth
				memset(IF_DEF_PARAMS(ifp)->auth_simple, 0, OSPF_AUTH_SIMPLE_SIZE + 1);
				IF_DEF_PARAMS(ifp)->auth_type = OSPF_AUTH_NOTSET;
    		}
			else if((IF_DEF_PARAMS(ifp)->auth_type == OSPF_AUTH_CRYPTOGRAPHIC))
			{
				//clean all has existed md5 key
    			ospf_auth_md5_delete_delete_all(IF_DEF_PARAMS(ifp));
				IF_DEF_PARAMS(ifp)->auth_type = OSPF_AUTH_NOTSET;
			}
		}
		
        SET_IF_PARAM(IF_DEF_PARAMS(ifp), auth_type);
        IF_DEF_PARAMS(ifp)->auth_type = vl_config->auth_type;
    }

    if (vl_config->auth_key)
    {
        memset(IF_DEF_PARAMS(ifp)->auth_simple, 0, OSPF_AUTH_SIMPLE_SIZE + 1);
        strncpy((char *) IF_DEF_PARAMS(ifp)->auth_simple, vl_config->auth_key,
                OSPF_AUTH_SIMPLE_SIZE);
    }
    else if (vl_config->md5_key)
    {
    	/*Note here: Source code can add more than one MD5 implementation.*/
        if (ospf_crypt_key_lookup(IF_DEF_PARAMS(ifp)->auth_crypt, vl_config->crypto_key_id)
                != NULL)
        {
            vty_error_out(vty, "OSPF: Key %d already exists%s",
                    vl_config->crypto_key_id, VTY_NEWLINE);
            return CMD_WARNING;
        }

        ck = ospf_crypt_key_new();
        ck->key_id = vl_config->crypto_key_id;
        memset(ck->auth_key, 0, OSPF_AUTH_MD5_SIZE + 1);
        strncpy((char *) ck->auth_key, vl_config->md5_key, OSPF_AUTH_MD5_SIZE);
		ck->auth_key_len = strlen(vl_config->md5_key);
		if (ck->auth_key_len < OSPF_MD5_SIZE)
	    {
	        ck->auth_key_len = OSPF_MD5_SIZE;
	    }
		
        ospf_crypt_key_add(IF_DEF_PARAMS(ifp)->auth_crypt, ck);
    }
    else if (vl_config->crypto_key_id != 0)
    {
        /* Delete a key */
        if (ospf_crypt_key_lookup(IF_DEF_PARAMS(ifp)->auth_crypt,
                                  vl_config->crypto_key_id) == NULL)
        {
            vty_error_out(vty, "OSPF: Key %d does not exist%s",
                    vl_config->crypto_key_id, VTY_NEWLINE);
            return CMD_WARNING;
        }

        ospf_crypt_key_delete(IF_DEF_PARAMS(ifp)->auth_crypt, vl_config->crypto_key_id);
    }

    return CMD_SUCCESS;
}

static int
ospf_vl_set_timers(struct ospf_vl_data *vl_data,
                   struct ospf_vl_config_data *vl_config)
{
    struct interface *ifp = vl_data->vl_oi->ifp;

    /* Virtual Link data initialised to defaults, so only set
       if a value given */
    if (vl_config->hello_interval)
    {
        SET_IF_PARAM(IF_DEF_PARAMS(ifp), v_hello);
        IF_DEF_PARAMS(ifp)->v_hello = vl_config->hello_interval;

		if(OSPF_IF_PARAM_IS_SET(vl_data->vl_oi, v_wait))
		{
			if (IS_DEBUG_OSPF_EVENT)
			{
				OSPF_LOG_DEBUG(" dead interval is configed\n");
			}
			if((u_int32_t)(vl_config->hello_interval) > IF_DEF_PARAMS(ifp)->v_wait)
			{
				if (IS_DEBUG_OSPF_EVENT)
				{
					OSPF_LOG_DEBUG(" dead interval change to 4 times of hello\n");
				}
				IF_DEF_PARAMS(ifp)->v_wait = 4 * vl_config->hello_interval;
			}
		}
		else
		{
			if (IS_DEBUG_OSPF_EVENT)
			{
				OSPF_LOG_DEBUG(" dead interval is not configed: dead interval change to 4 times of hello\n");
			}
			IF_DEF_PARAMS(ifp)->v_wait = 4 * vl_config->hello_interval;
		}
    }

    if (vl_config->dead_interval)
    {
    	if((u_int32_t)(vl_config->dead_interval) <= IF_DEF_PARAMS(ifp)->v_hello)
    	{

			vty_info_out(vl_config->vty, "Hello interval is greater than Dead interval, \
				now Dead interval is forced to adjust to 4 times of new Hello interval \n");
    		vl_config->dead_interval = 4 * IF_DEF_PARAMS(ifp)->v_hello;
		}
		
        SET_IF_PARAM(IF_DEF_PARAMS(ifp), v_wait);
        IF_DEF_PARAMS(ifp)->v_wait = vl_config->dead_interval;
    }

    if (vl_config->retransmit_interval)
    {
        SET_IF_PARAM(IF_DEF_PARAMS(ifp), retransmit_interval);
        IF_DEF_PARAMS(ifp)->retransmit_interval = vl_config->retransmit_interval;
    }

    if (vl_config->transmit_delay)
    {
        SET_IF_PARAM(IF_DEF_PARAMS(ifp), transmit_delay);
        IF_DEF_PARAMS(ifp)->transmit_delay = vl_config->transmit_delay;
    }

    return CMD_SUCCESS;
}



/* The business end of all of the above */
static int
ospf_vl_set(struct ospf *ospf, struct ospf_vl_config_data *vl_config)
{
	OSPF_LOG_DEBUG();
    struct ospf_vl_data *vl_data;
    int ret;
	
    vl_data = ospf_find_vl_data(ospf, vl_config);
    if (!vl_data)
    {
        return CMD_WARNING;
    }

    /* Process this one first as it can have a fatal result, which can
       only logically occur if the virtual link exists already
       Thus a command error does not result in a change to the
       running configuration such as unexpectedly altered timer
       values etc.*/
    ret = ospf_vl_set_security(vl_data, vl_config);
    if (ret != CMD_SUCCESS)
    {
        return ret;
    }

    /* Set any time based parameters, these area already range checked */
    ret = ospf_vl_set_timers(vl_data, vl_config);
    if (ret != CMD_SUCCESS)
    {
        return ret;
    }

    return CMD_SUCCESS;
}

/* This stuff exists to make specifying all the alias commands A LOT simpler
 */
#define VLINK_HELPSTR_IPADDR \
       "Configure a virtual link\n" \
       "Router ID of the remote ABR\n"

#define VLINK_HELPSTR_AUTHTYPE_SIMPLE \
       "Enable authentication on this virtual link\n" \
       "dummy string \n"

#define VLINK_HELPSTR_AUTHTYPE_ALL \
       VLINK_HELPSTR_AUTHTYPE_SIMPLE \
       "Use null authentication\n" \
       "Use message-digest authentication\n"

#define VLINK_HELPSTR_RETRANS_INTERVAL_PARAM_NOSECS \
	   "Time between retransmitting lost link state advertisements\n" \
	   "Seconds\n"

#define VLINK_HELPSTR_TRANSMIT_DELAY_PARAM_NOSECS \
	   "Link state transmit delay\n" \
	   "Seconds\n"

#define VLINK_HELPSTR_TIME_PARAM_NOSECS \
       "Time between HELLO packets\n" \
       "Interval after which a neighbor is declared dead\n"

#define VLINK_HELPSTR_TIME_PARAM \
       VLINK_HELPSTR_TIME_PARAM_NOSECS \
       "Seconds\n"

#define VLINK_HELPSTR_AUTH_SIMPLE \
       "Authentication simple password (key)\n" \
       "dummy string \n" \
       "The OSPF password (password length 1~8)"

#define VLINK_HELPSTR_AUTH_MD5 \
       "Message digest authentication (MD5) password (key)\n" \
       "dummy string \n" \
       "Key ID\n" \
       "Use MD5 algorithm\n" \
       "The OSPF password (password length 1~256)"

#if 0
DEFUN(ospf_area_vlink,
      ospf_area_vlink_cmd,
      "vlink-peer A.B.C.D",
      VLINK_HELPSTR_IPADDR)
{
    int i = 0;
    int ret = 0;
	int h_flag = 0;
	int d_flag = 0;
    struct ospf *ospf = vty->index;
    struct ospf_vl_config_data vl_config;
    char auth_key[OSPF_AUTH_SIMPLE_SIZE+1];
    char md5_key[OSPF_AUTH_MD5_SIZE+1];
    struct ospf_area *area = (struct ospf_area *)(vty->index_sub);

    if (!area)
    {
        VTY_CHECK_OSPF_AREA
    }
	
	ospf_vl_config_data_init(&vl_config, vty);
	vl_config.area_id = area->area_id;
	vl_config.format = OSPF_AREA_ID_FORMAT_ADDRESS;

    /* Read off first parameters and check them */
    ret = inet_aton(argv[0], &vl_config.vl_peer);
    if (! ret)
    {
        vty_error_out(vty, "Please specify valid Router ID as a.b.c.d%s",
                VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (argc <= 1)
    {
        /* Thats all folks! - BUGS B. strikes again!!!*/
        return  ospf_vl_set(ospf, &vl_config);
    }

    /* Deal with other parameters */
    for (i = 1; i < argc; i++)
    {
        /* vty_out (vty, "argv[%d] - %s%s", i, argv[i], VTY_NEWLINE); */
        switch (argv[i][0])
        {
            case 'a':

                if (strncmp(argv[i], "auth-simple", 11) == 0)
                {
                    /* authentication-key - this option can occur anywhere on
                                        command line.  At start of command line
                            must check for authentication option. */
                    vl_config.auth_type = OSPF_AUTH_SIMPLE;

					if (strlen(argv[i+1]) > OSPF_AUTH_SIMPLE_SIZE)
				    {
				        vty_error_out(vty, " OSPF auth simple PASSWORD length 1~8 bytes.%s", VTY_NEWLINE);
				        return CMD_WARNING;
				    }					
                    memset(auth_key, 0, OSPF_AUTH_SIMPLE_SIZE + 1);
                    strncpy(auth_key, argv[i+1], OSPF_AUTH_SIMPLE_SIZE);
                    vl_config.auth_key = auth_key;
                    i++;
                }

                break;
            case 'm':
                /* message-digest-key */
				if (strncmp(argv[i], "message-digest", 14) == 0)
				{
	                i++;
					vl_config.auth_type = OSPF_AUTH_CRYPTOGRAPHIC;
	                vl_config.crypto_key_id = strtol(argv[i], NULL, 10);

	                if (vl_config.crypto_key_id < 0)
	                {
	                    return CMD_WARNING;
	                }

	                i++;
					if (strlen(argv[i]) > OSPF_AUTH_MD5_SIZE)
				    {
				        vty_error_out(vty, " OSPF auth md5 PASSWORD length 1~256 bytes.%s", VTY_NEWLINE);
				        return CMD_WARNING;
				    }
	                memset(md5_key, 0, OSPF_AUTH_MD5_SIZE + 1);
	                strncpy(md5_key, argv[i], OSPF_AUTH_MD5_SIZE);
	                vl_config.md5_key = md5_key;
				}
                break;
            case 'h':
                /* Hello interval */
                i++;
                vl_config.hello_interval = strtol(argv[i], NULL, 10);

                if ((vl_config.hello_interval < 1) || (vl_config.hello_interval > 65535))
                {
                	vty_error_out(vty, "Hello Interval is invalid%s", VTY_NEWLINE);
                    return CMD_WARNING;
                }
				h_flag++;
				if(d_flag > 0)
				{
					if(vl_config.hello_interval >= vl_config.dead_interval)
					{
						vty_error_out(vty, "Hello interval is greater than Dead interval.%s", VTY_NEWLINE);
						return CMD_WARNING;
					}
				}				
				
                break;
            case 'r':
                /* Retransmit Interval */
                i++;
                vl_config.retransmit_interval = strtol(argv[i], NULL, 10);

                if ((vl_config.retransmit_interval < 1) || (vl_config.retransmit_interval > 3600))
                {
                	vty_error_out(vty, "Retransmit Interval is invalid%s", VTY_NEWLINE);
                    return CMD_WARNING;
                }

                break;
            case 't':
                /* Transmit Delay */
                i++;
                vl_config.transmit_delay = strtol(argv[i], NULL, 10);

                if ((vl_config.transmit_delay < 1) || (vl_config.transmit_delay > 3600))
                {
                	vty_error_out(vty, "Transmit Delay is invalid%s", VTY_NEWLINE);
                    return CMD_WARNING;
                }

                break;
            case 'd':
                /* Dead Interval */
                i++;
                vl_config.dead_interval = strtol(argv[i], NULL, 10);

                if ((vl_config.dead_interval < 1) || (vl_config.dead_interval > 65535))
                {
                	vty_error_out(vty, "Dead Interval is invalid%s", VTY_NEWLINE);
                    return CMD_WARNING;
                }
				d_flag++;
				if(h_flag > 0)
				{
					if(vl_config.hello_interval >= vl_config.dead_interval)
					{
						vty_error_out(vty, "Hello interval is greater than Dead interval.%s", VTY_NEWLINE);
						return CMD_WARNING;
					}
				}
				
                break;
			default:
				break;
        }
    }

    /* Action configuration */
    return ospf_vl_set(ospf, &vl_config);
}

DEFUN(no_ospf_area_vlink,
      no_ospf_area_vlink_cmd,
      "no vlink-peer A.B.C.D",
      NO_STR
      VLINK_HELPSTR_IPADDR)
{
    struct ospf *ospf = vty->index;
    struct ospf_vl_config_data vl_config;
    struct ospf_vl_data *vl_data = NULL;
    char auth_key[OSPF_AUTH_SIMPLE_SIZE+1];
    int i;
    int ret = -1;
	
    struct ospf_area *area = (struct ospf_area *)(vty->index_sub);
	if (!area)
    {
        VTY_CHECK_OSPF_AREA
    }
    
    ospf_vl_config_data_init(&vl_config, vty);
	vl_config.area_id.s_addr = area->area_id.s_addr;
	
    area = ospf_area_lookup_by_area_id(ospf, vl_config.area_id);
    if (!area)
    {
        vty_error_out(vty, "Area does not exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ret = inet_aton(argv[0], &vl_config.vl_peer);
    if (! ret)
    {
        vty_error_out(vty, "Please specify valid Router ID as a.b.c.d%s",
                VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (argc <= 1)
    {
        /* Basic VLink no command */
        /* Thats all folks! - BUGS B. strikes again!!!*/
        if ((vl_data = ospf_vl_lookup(ospf, area, vl_config.vl_peer)))
        {
            ospf_vl_delete(ospf, vl_data);
        }
		vl_data = NULL;

        ospf_area_check_free(ospf, vl_config.area_id);
        return CMD_SUCCESS;
    }

    /* If we are down here, we are reseting parameters */

    /* Deal with other parameters */
    for (i = 1; i < argc; i++)
    {
        /* vty_out (vty, "argv[%d] - %s%s", i, argv[i], VTY_NEWLINE); */
        switch (argv[i][0])
        {
            case 'a':
				if (strncmp(argv[i], "auth-simple", 11) == 0)
                {
                    /* authentication  - this option can only occur at start
                                     of command line */
                    vl_config.auth_type = OSPF_AUTH_NOTSET;
					memset(auth_key, 0, OSPF_AUTH_SIMPLE_SIZE + 1);
                    vl_config.auth_key = auth_key;
                }

                break;
            case 'm':
                /* message-digest-key */
                /* Delete one key */
				if (strncmp(argv[i], "message-digest", 14) == 0)
				{
	                i++;
	                vl_config.crypto_key_id = strtol(argv[i], NULL, 10);

	                if (vl_config.crypto_key_id < 0)
	                {
	                    return CMD_WARNING;
	                }
					vl_config.auth_type = OSPF_AUTH_NOTSET;
	                vl_config.md5_key = NULL;
				}
                break;
            case 'h':
                /* Hello interval */
                vl_config.hello_interval = OSPF_HELLO_INTERVAL_DEFAULT;
                break;
            case 'r':
                /* Retransmit Interval */
                vl_config.retransmit_interval = OSPF_RETRANSMIT_INTERVAL_DEFAULT;
                break;
            case 't':
                /* Transmit Delay */
                vl_config.transmit_delay = OSPF_TRANSMIT_DELAY_DEFAULT;
                break;
            case 'd':
                /* Dead Interval */
                i++;
                vl_config.dead_interval = OSPF_ROUTER_DEAD_INTERVAL_DEFAULT;
                break;
			default:
				break;
        }
    }

    /* Action configuration */
    return ospf_vl_set(ospf, &vl_config);
}

#endif
static
void show_ip_ospf_vlinks_info(struct vty *vty, struct ospf_vl_data *vl_data)
{
    struct ospf_neighbor *nbr = NULL;
	struct ospf_interface *oi = NULL;
	char msgbuf[255];
	
	if (vl_data != NULL)
	{
		if((oi = vl_data->vl_oi))
		{
			memset(msgbuf, 0, sizeof(msgbuf));
			if((nbr = ospf_nbr_lookup_by_routerid(oi->nbrs, &vl_data->vl_peer)))
			{
				snprintf(msgbuf, sizeof(msgbuf), LOOKUP(ospf_nsm_state_msg, nbr->state));
			}
			else
			{
				snprintf(msgbuf, sizeof(msgbuf), "Down");
			}
			vty_out(vty, "  Virtual-link Neighbor-id ->  %s, Neighbor-State: %s %s",
	                        inet_ntoa(vl_data->vl_peer), msgbuf, VTY_NEWLINE);
			
			vty_out(vty, "  Interface: %s %s", IF_NAME(vl_data->nexthop.oi), VTY_NEWLINE);
			vty_out(vty, "  Cost: %d      State: %s      Type: %s %s", oi->output_cost, \
				LOOKUP(ospf_ism_state_msg, oi->state), ospf_network_type_str[oi->type], VTY_NEWLINE);
			vty_out(vty, "  Transit Area: %s %s", inet_ntoa(vl_data->vl_area_id), VTY_NEWLINE);
			vty_out(vty, "%s", VTY_NEWLINE);
	        vty_out(vty, "  Timer intervals configured,%s", VTY_NEWLINE);
	        vty_out(vty, "  Hello: ");

	        if (OSPF_IF_PARAM(oi, fast_hello) == 0)
	        {
	            vty_out(vty, "%ds   ", OSPF_IF_PARAM(oi, v_hello));
	        }
	        else
	        {
	            vty_out(vty, "%dms   ", 1000 / OSPF_IF_PARAM(oi, fast_hello));
	        }

	        vty_out(vty, " Dead: %ds   Retransmit: %d   Transmit : %d%s",
	                OSPF_IF_PARAM(oi, v_wait),
	                OSPF_IF_PARAM(oi, retransmit_interval), OSPF_IF_PARAM(oi, transmit_delay),
	                VTY_NEWLINE);

	        if (OSPF_IF_PASSIVE_STATUS(oi) == OSPF_IF_ACTIVE)
	        {
	            char timebuf[OSPF_TIME_DUMP_SIZE];
	            /*vty_out(vty, "  Hello due in: %s%s",
	                    ospf_timer_dump(oi->t_hello, timebuf, sizeof(timebuf)),
	                    VTY_NEWLINE);*/
				vty_out(vty, "  Hello due in: %s%s",
	                    ospf_timer_dump_new(oi->t_hello, OSPF_IF_PARAM(oi, v_hello), timebuf, sizeof(timebuf)),
	                    VTY_NEWLINE);
	        }
	        else /* passive-interface is set */
	        {
	            vty_out(vty, "  No Hellos (Passive interface)%s", VTY_NEWLINE);
	        }

			vty_out(vty, "  *********************************************************** %s%s", VTY_NEWLINE, VTY_NEWLINE);
		}
	}
}
	  
DEFUN(show_ip_ospf_vlink,
	  show_ip_ospf_vlink_cmd,
	  "show ip ospf [<1-255>] vlink",
	  SHOW_STR
	  IP_STR
	  "OSPF information\n"
	  "OSPF instance number\n"
	  "Virtual link information\n")
{
	struct ospf *ospf = NULL;
	struct listnode *node = NULL;
	int ospf_id = OSPF_DEFAULT_PROCESS_ID;
	struct ospf_vl_data *vl_data = NULL;

	if (argv[0] != NULL)
	{
		VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID);
	}

	ospf = ospf_lookup_id(ospf_id);

	if (ospf == NULL)
	{
		VTY_OSPF_INSTANCE_ERR
	}

	vty_out(vty, "%s          OSPF Instance %d with Router ID %s%s", VTY_NEWLINE, ospf_id, \
															  inet_ntoa(ospf->router_id),VTY_NEWLINE);
	vty_out(vty, "                    Virtual Links%s", VTY_NEWLINE);
	vty_out(vty, "%s", VTY_NEWLINE);
	
	/* Show All vlink. */
	for (ALL_LIST_ELEMENTS_RO(ospf->vlinks, node, vl_data))
	{
		if(vl_data != NULL)
		{
		  show_ip_ospf_vlinks_info(vty, vl_data);
		}
	}

	return CMD_SUCCESS;
}

DEFUN(show_ip_ospf_vlink_h3c,
	show_ip_ospf_vlink_h3c_cmd,
	"display ospf [<1-65535>] vlink",
	SHOW_STR
	"OSPF information\n"
	"OSPF instance number\n"
	"Virtual link information\n")
{
	struct ospf *ospf = NULL;
	struct listnode *node = NULL;
	struct listnode *node1 = NULL;
	struct listnode *nnode = NULL;
	int ospf_id = OSPF_DEFAULT_PROCESS_ID;
	struct ospf_vl_data *vl_data = NULL;

	if (argv[0] != NULL)
	{
		VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID_H3C);
	
		ospf = ospf_lookup_id(ospf_id);

		if (ospf == NULL)
		{
			VTY_OSPF_INSTANCE_ERR
		}

		vty_out(vty, "%s          OSPF Instance %d with Router ID %s%s", VTY_NEWLINE, ospf_id, \
																  inet_ntoa(ospf->router_id),VTY_NEWLINE);
		vty_out(vty, "                    Virtual Links%s", VTY_NEWLINE);
		vty_out(vty, "%s", VTY_NEWLINE);
		
		/* Show All vlink. */
		for (ALL_LIST_ELEMENTS_RO(ospf->vlinks, node, vl_data))
		{
			if(vl_data != NULL)
			{
			  show_ip_ospf_vlinks_info(vty, vl_data);
			}
		}
	}
	else
	{
		for (ALL_LIST_ELEMENTS (om->ospf, node1, nnode, ospf))
		{
			if(listcount(ospf->vlinks) > 0)
			{
				vty_out(vty, "%s          OSPF Instance %d with Router ID %s%s", VTY_NEWLINE, ospf->ospf_id, \
																	  inet_ntoa(ospf->router_id),VTY_NEWLINE);
				vty_out(vty, "                    Virtual Links%s", VTY_NEWLINE);
				vty_out(vty, "%s", VTY_NEWLINE);

				/* Show All vlink. */
				for (ALL_LIST_ELEMENTS_RO(ospf->vlinks, node, vl_data))
				{
					if(vl_data != NULL)
					{
					  show_ip_ospf_vlinks_info(vty, vl_data);
					}
				}
			}
		}
	}

	return CMD_SUCCESS;
}

#if 0
ALIAS(ospf_area_vlink,
      ospf_area_vlink_param1_cmd,
      "vlink-peer A.B.C.D "
      "(hello-interval|dead-interval) <1-65535>",
      VLINK_HELPSTR_IPADDR
      VLINK_HELPSTR_TIME_PARAM)

ALIAS(no_ospf_area_vlink,
      no_ospf_area_vlink_param1_cmd,
      "no vlink-peer A.B.C.D "
      "(hello-interval|dead-interval)",
      NO_STR
      VLINK_HELPSTR_IPADDR
      VLINK_HELPSTR_TIME_PARAM)

ALIAS(ospf_area_vlink,
      ospf_area_vlink_param2_cmd,
      "vlink-peer A.B.C.D "
      "(hello-interval|dead-interval) <1-65535> "
      "(hello-interval|dead-interval) <1-65535>",
      VLINK_HELPSTR_IPADDR
      VLINK_HELPSTR_TIME_PARAM
      VLINK_HELPSTR_TIME_PARAM)

ALIAS(no_ospf_area_vlink,
      no_ospf_area_vlink_param2_cmd,
      "no vlink-peer A.B.C.D "
      "(hello-interval|dead-interval) "
      "(hello-interval|dead-interval)",
      NO_STR
      VLINK_HELPSTR_IPADDR
      VLINK_HELPSTR_TIME_PARAM
      VLINK_HELPSTR_TIME_PARAM)

ALIAS(ospf_area_vlink,
      ospf_area_vlink_retrans_interval_cmd,
      "vlink-peer A.B.C.D "
      "(retransmit-interval) <1-3600>",
      VLINK_HELPSTR_IPADDR
      VLINK_HELPSTR_RETRANS_INTERVAL_PARAM_NOSECS)

ALIAS(no_ospf_area_vlink,
      no_ospf_area_vlink_retrans_interval_cmd,
      "no vlink-peer A.B.C.D "
      "(retransmit-interval)",
      NO_STR
      VLINK_HELPSTR_IPADDR
      VLINK_HELPSTR_RETRANS_INTERVAL_PARAM_NOSECS)

ALIAS(ospf_area_vlink,
      ospf_area_vlink_trans_delay_cmd,
      "vlink-peer A.B.C.D "
      "(transmit-delay) <1-3600>",
      VLINK_HELPSTR_IPADDR
      VLINK_HELPSTR_TRANSMIT_DELAY_PARAM_NOSECS)

ALIAS(no_ospf_area_vlink,
      no_ospf_area_vlink_trans_delay_cmd,
      "no vlink-peer A.B.C.D "
      "(transmit-delay)",
      NO_STR
      VLINK_HELPSTR_IPADDR
      VLINK_HELPSTR_TRANSMIT_DELAY_PARAM_NOSECS)


ALIAS(ospf_area_vlink,
      ospf_area_vlink_md5_cmd,
      "vlink-peer A.B.C.D "
      "(message-digest-md5-key|) <1-255> md5 KEY",
      VLINK_HELPSTR_IPADDR
      VLINK_HELPSTR_AUTH_MD5)

ALIAS(no_ospf_area_vlink,
      no_ospf_area_vlink_md5_cmd,
      "no vlink-peer A.B.C.D "
      "(message-digest-md5-key|) <1-255>",
      NO_STR
      VLINK_HELPSTR_IPADDR
      VLINK_HELPSTR_AUTH_MD5)

ALIAS(ospf_area_vlink,
      ospf_area_vlink_authkey_cmd,
      "vlink-peer A.B.C.D "
      "(auth-simple-key|) AUTH_KEY",
      VLINK_HELPSTR_IPADDR
      VLINK_HELPSTR_AUTH_SIMPLE)

ALIAS(no_ospf_area_vlink,
      no_ospf_area_vlink_authkey_cmd,
      "no vlink-peer A.B.C.D "
      "(auth-simple-key|)",
      NO_STR
      VLINK_HELPSTR_IPADDR
      VLINK_HELPSTR_AUTH_SIMPLE)
#endif

DEFUN(ospf_area_vlink_h3c,
	ospf_area_vlink_h3c_cmd,
	"vlink-peer A.B.C.D {dead <1-32768>|hello <1-8192>|retransmit <1-3600>|trans-delay <1-3600>}",
	VLINK_HELPSTR_IPADDR
    "Interval after which a neighbor is declared dead\n"
    "Seconds\n"
    "Time between HELLO packets\n"
    "Seconds\n"
    "Time between retransmitting lost link state advertisements\n"
    "Seconds\n"
    "Link state transmit delay\n"
    "Seconds\n")
{
	int ret = 0;
	struct ospf *ospf = vty->index;
    struct ospf_vl_config_data vl_config;
	//char auth_key[OSPF_AUTH_SIMPLE_SIZE+1];
    //char md5_key[OSPF_AUTH_MD5_SIZE+1];
	struct ospf_area *area = (struct ospf_area *)(vty->index_sub);

	if (!area)
	{
		VTY_CHECK_OSPF_AREA
	}
	
	ospf_vl_config_data_init(&vl_config, vty);
	vl_config.area_id = area->area_id;
	vl_config.format = OSPF_AREA_ID_FORMAT_ADDRESS;

	/* Read off first parameters and check them */
	ret = inet_aton(argv[0], &vl_config.vl_peer);
	if (!ret)
	{
		vty_error_out(vty, "Please specify valid Router ID as a.b.c.d%s",
				VTY_NEWLINE);
		return CMD_WARNING;
	}

	//get hello interval
	if((argv[2] != NULL) && (argv[1] != NULL))
	{
		//get hello interval
		vl_config.hello_interval = strtol(argv[2], NULL, 10);

	    if ((vl_config.hello_interval < 1) || (vl_config.hello_interval > 8192))
	    {
	    	vty_error_out(vty, "Hello Interval is invalid%s", VTY_NEWLINE);
	        return CMD_WARNING;
	    }

		vl_config.dead_interval = strtol(argv[1], NULL, 10);

        if ((vl_config.dead_interval < 1) || (vl_config.dead_interval > 32768))
        {
        	vty_error_out(vty, "Dead Interval is invalid%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
		
		if(vl_config.hello_interval >= vl_config.dead_interval)
		{
			vty_error_out(vty, "Hello interval is greater than Dead interval.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	//get dead interval
	else if(argv[1] != NULL)
	{
		vl_config.dead_interval = strtol(argv[1], NULL, 10);

        if ((vl_config.dead_interval < 1) || (vl_config.dead_interval > 32768))
        {
        	vty_error_out(vty, "Dead Interval is invalid%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
	}
	//get hello interval
	else if(argv[2] != NULL)
	{
		vl_config.hello_interval = strtol(argv[2], NULL, 10);

	    if ((vl_config.hello_interval < 1) || (vl_config.hello_interval > 8192))
	    {
	    	vty_error_out(vty, "Hello Interval is invalid%s", VTY_NEWLINE);
	        return CMD_WARNING;
	    }
	}

	//get retransmit interval
	if(argv[3] != NULL)
	{
		vl_config.retransmit_interval = strtol(argv[3], NULL, 10);

        if ((vl_config.retransmit_interval < 1) || (vl_config.retransmit_interval > 3600))
        {
        	vty_error_out(vty, "Retransmit Interval is invalid%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
	}
	
	//get trans-delay
	if(argv[4] != NULL)
	{
		vl_config.transmit_delay = strtol(argv[4], NULL, 10);

	    if ((vl_config.transmit_delay < 1) || (vl_config.transmit_delay > 3600))
	    {
	    	vty_error_out(vty, "Transmit Delay is invalid%s", VTY_NEWLINE);
	        return CMD_WARNING;
	    }
	}

	/* Action configuration */
    return ospf_vl_set(ospf, &vl_config);
}


DEFUN(no_ospf_area_vlink_h3c,
		no_ospf_area_vlink_h3c_cmd,
		"undo vlink-peer A.B.C.D {dead|hello|retransmit|trans-delay|simple}",
		"Cancel current setting\n"
		VLINK_HELPSTR_IPADDR
		"Interval after which a neighbor is declared dead\n"
		"Time between HELLO packets\n"
		"Time between retransmitting lost link state advertisements\n"
		"Link state transmit delay\n"
		"Simple authentication mode\n")
{
	int ret = 0;
	int flag = 0;
	struct ospf *ospf = vty->index;
	struct ospf_vl_data *vl_data = NULL;
    struct ospf_vl_config_data vl_config;
	char auth_key[OSPF_AUTH_SIMPLE_SIZE+1];
    //char md5_key[OSPF_AUTH_MD5_SIZE+1];
	struct ospf_area *area = (struct ospf_area *)(vty->index_sub);

	if (!area)
    {
        VTY_CHECK_OSPF_AREA
    }
	ospf_vl_config_data_init(&vl_config, vty);
	vl_config.area_id.s_addr = area->area_id.s_addr;
	
    area = ospf_area_lookup_by_area_id(ospf, vl_config.area_id);
    if (!area)
    {
        vty_error_out(vty, "Area does not exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ret = inet_aton(argv[0], &vl_config.vl_peer);
    if (! ret)
    {
        vty_error_out(vty, "Please specify valid Router ID as a.b.c.d%s",
                VTY_NEWLINE);
        return CMD_WARNING;
    }

	/* If we are down here, we are reseting parameters */
	/* Dead Interval */
	if(argv[1] != NULL)
	{
		vl_config.dead_interval = OSPF_ROUTER_DEAD_INTERVAL_DEFAULT;
		flag++;
	}

	/* Hello interval */
	if(argv[2] != NULL)
	{
		vl_config.hello_interval = OSPF_HELLO_INTERVAL_DEFAULT;
		flag++;
	}
	
	/* Retransmit Interval */
	if(argv[3] != NULL)
	{
		vl_config.retransmit_interval = OSPF_RETRANSMIT_INTERVAL_DEFAULT;
		flag++;
	}

	/* Transmit Delay */
	if(argv[4] != NULL)
	{
		vl_config.transmit_delay = OSPF_TRANSMIT_DELAY_DEFAULT;
		flag++;
	}
	
	/* auth simple */
	if(argv[5] != NULL)
	{
		vl_config.auth_type = OSPF_AUTH_NOTSET;
		memset(auth_key, 0, OSPF_AUTH_SIMPLE_SIZE + 1);
        vl_config.auth_key = auth_key;
		flag++;
	}

	if (!flag)
	{
		/* Basic VLink no command */
		/* Thats all folks! - BUGS B. strikes again!!!*/
		if ((vl_data = ospf_vl_lookup(ospf, area, vl_config.vl_peer)))
		{
			ospf_vl_delete(ospf, vl_data);
		}
		vl_data = NULL;

		ospf_area_check_free(ospf, vl_config.area_id);
		return CMD_SUCCESS;
	}

	/* Action configuration */
    return ospf_vl_set(ospf, &vl_config);
}


ALIAS(no_ospf_area_vlink_h3c,
		no_ospf_area_vlink_cmd,
		"no vlink-peer A.B.C.D {dead|hello|retransmit|trans-delay|simple}",
		NO_STR
		VLINK_HELPSTR_IPADDR
		"Interval after which a neighbor is declared dead\n"
		"Time between HELLO packets\n"
		"Time between retransmitting lost link state advertisements\n"
		"Link state transmit delay\n"
		"Simple authentication mode\n")


DEFUN(ospf_area_vlink_auth_md5_h3c,
		ospf_area_vlink_auth_md5_h3c_cmd,
		"vlink-peer A.B.C.D (hmac-md5|md5) <1-255> (cipher|plain) PASSWORD",
		VLINK_HELPSTR_IPADDR
		"Use HMAC-MD5 algorithm\n"
		"Use MD5 algorithm\n"
		"Key ID\n"
		"Encryption type (Cryptogram)\n"
		"Encryption type (Plain text)\n"
		"The password (length 1-255 bytes)\n")
{
	int ret = 0;
	struct ospf *ospf = vty->index;
    struct ospf_vl_config_data vl_config;
    char md5_key[OSPF_AUTH_MD5_SIZE+1];
	struct ospf_area *area = (struct ospf_area *)(vty->index_sub);

	if (!area)
	{
		VTY_CHECK_OSPF_AREA
	}
	
	ospf_vl_config_data_init(&vl_config, vty);
	vl_config.area_id = area->area_id;
	vl_config.format = OSPF_AREA_ID_FORMAT_ADDRESS;

	/* Read off first parameters and check them */
	ret = inet_aton(argv[0], &vl_config.vl_peer);
	if (!ret)
	{
		vty_error_out(vty, "Please specify valid Router ID as a.b.c.d%s",
				VTY_NEWLINE);
		return CMD_WARNING;
	}

	vl_config.auth_type = OSPF_AUTH_CRYPTOGRAPHIC;
    vl_config.crypto_key_id = strtol(argv[2], NULL, 10);

    if (vl_config.crypto_key_id < 0 || vl_config.crypto_key_id > 255)
    {
        return CMD_WARNING;
    }

	if (strlen(argv[4]) > OSPF_AUTH_MD5_SIZE)
    {
        vty_error_out(vty, " OSPF auth md5 PASSWORD length 1~256 bytes.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    memset(md5_key, 0, OSPF_AUTH_MD5_SIZE + 1);
    strncpy(md5_key, argv[4], OSPF_AUTH_MD5_SIZE);
    vl_config.md5_key = md5_key;
	
	/* Action configuration */
    return ospf_vl_set(ospf, &vl_config);
}


DEFUN(ospf_area_vlink_auth_simple_h3c,
		ospf_area_vlink_auth_simple_h3c_cmd,
		"vlink-peer A.B.C.D simple (cipher|plain) PASSWORD",
		VLINK_HELPSTR_IPADDR
		"Simple authentication mode\n"
		"Encryption type (Cryptogram)\n"
		"Encryption type (Plain text)\n"
		"The password (length 1-8 bytes)\n")
{
	int ret = 0;
	struct ospf *ospf = vty->index;
    struct ospf_vl_config_data vl_config;
	char auth_key[OSPF_AUTH_SIMPLE_SIZE+1];
	struct ospf_area *area = (struct ospf_area *)(vty->index_sub);

	if (!area)
	{
		VTY_CHECK_OSPF_AREA
	}
	
	ospf_vl_config_data_init(&vl_config, vty);
	vl_config.area_id = area->area_id;
	vl_config.format = OSPF_AREA_ID_FORMAT_ADDRESS;

	/* Read off first parameters and check them */
	ret = inet_aton(argv[0], &vl_config.vl_peer);
	if (!ret)
	{
		vty_error_out(vty, "Please specify valid Router ID as a.b.c.d%s",
				VTY_NEWLINE);
		return CMD_WARNING;
	}

	vl_config.auth_type = OSPF_AUTH_SIMPLE;

	if (strlen(argv[2]) > OSPF_AUTH_SIMPLE_SIZE)
    {
        vty_error_out(vty, " OSPF auth simple PASSWORD length 1~8 bytes.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }					
    memset(auth_key, 0, OSPF_AUTH_SIMPLE_SIZE + 1);
    strncpy(auth_key, argv[2], OSPF_AUTH_SIMPLE_SIZE);
    vl_config.auth_key = auth_key;

	/* Action configuration */
    return ospf_vl_set(ospf, &vl_config);
}

DEFUN(no_ospf_area_vlink_auth_md5_h3c,
		no_ospf_area_vlink_auth_md5_h3c_cmd,
		"undo vlink-peer A.B.C.D (hmac-md5|md5) <1-255>",
		"Cancel current setting\n"
		VLINK_HELPSTR_IPADDR
		"Use HMAC-MD5 algorithm\n"
		"Use MD5 algorithm\n"
		"Key ID\n")
{
	int ret = 0;
	struct ospf *ospf = vty->index;
	//struct ospf_vl_data *vl_data = NULL;
	struct ospf_vl_config_data vl_config;
	//char auth_key[OSPF_AUTH_SIMPLE_SIZE+1];
	//char md5_key[OSPF_AUTH_MD5_SIZE+1];
	struct ospf_area *area = (struct ospf_area *)(vty->index_sub);

	if (!area)
	{
		VTY_CHECK_OSPF_AREA
	}
	ospf_vl_config_data_init(&vl_config, vty);
	vl_config.area_id.s_addr = area->area_id.s_addr;

	area = ospf_area_lookup_by_area_id(ospf, vl_config.area_id);
	if (!area)
	{
		vty_error_out(vty, "Area does not exist%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	ret = inet_aton(argv[0], &vl_config.vl_peer);
	if (! ret)
	{
		vty_error_out(vty, "Please specify valid Router ID as a.b.c.d%s",
				VTY_NEWLINE);
		return CMD_WARNING;
	}

	vl_config.crypto_key_id = strtol(argv[2], NULL, 10);

    if (vl_config.crypto_key_id < 0 || vl_config.crypto_key_id > 255)
    {
        return CMD_WARNING;
    }
	vl_config.auth_type = OSPF_AUTH_NOTSET;
    vl_config.md5_key = NULL;

	/* Action configuration */
    return ospf_vl_set(ospf, &vl_config);
}


ALIAS(no_ospf_area_vlink_auth_md5_h3c,
		no_ospf_area_vlink_auth_md5_cmd,
		"no vlink-peer A.B.C.D (hmac-md5|md5) <1-255>",
		NO_STR
		VLINK_HELPSTR_IPADDR
		"Use HMAC-MD5 algorithm\n"
		"Use MD5 algorithm\n"
		"Key ID\n")


DEFUN(ospf_area_shortcut,
      ospf_area_shortcut_cmd,
      "area shortcut (default|enable|disable)",
      "OSPF area parameters\n"
      "Configure the area's shortcutting mode\n"
      "Set default shortcutting behavior\n"
      "Enable shortcutting through the area\n"
      "Disable shortcutting through the area\n")
{
    struct ospf *ospf = vty->index;
    int mode;
    struct ospf_area *area = (struct ospf_area *)(vty->index_sub);

    if (!area)
    {
        VTY_CHECK_OSPF_AREA
    }

    //area = ospf_area_get (ospf, area->area_id, format);
    if (strncmp(argv[0], "de", 2) == 0)
    {
        mode = OSPF_SHORTCUT_DEFAULT;
    }
    else if (strncmp(argv[0], "di", 2) == 0)
    {
        mode = OSPF_SHORTCUT_DISABLE;
    }
    else if (strncmp(argv[0], "e", 1) == 0)
    {
        mode = OSPF_SHORTCUT_ENABLE;
    }
    else
    {
        return CMD_WARNING;
    }

    ospf_area_shortcut_set(ospf, area, mode);

    if (ospf->abr_type != OSPF_ABR_SHORTCUT)
        vty_out(vty, "Shortcut area setting will take effect "
                "only when the router is configured as Shortcut ABR%s",
                VTY_NEWLINE);

    return CMD_SUCCESS;
}

DEFUN(no_ospf_area_shortcut,
      no_ospf_area_shortcut_cmd,
      "no area shortcut (enable|disable)",
      NO_STR
      "OSPF area parameters\n"
      "Deconfigure the area's shortcutting mode\n"
      "Deconfigure enabled shortcutting through the area\n"
      "Deconfigure disabled shortcutting through the area\n")
{
    struct ospf *ospf = vty->index;
    struct ospf_area *area = (struct ospf_area *)(vty->index_sub);

    if (!area)
    {
        return CMD_SUCCESS;
    }

    ospf_area_shortcut_unset(ospf, area);
    return CMD_SUCCESS;
}


DEFUN(no_ospf_area_stub,
      no_ospf_area_stub_cmd,
      "no area stub",
      NO_STR
      "OSPF area parameters\n"
      "Configure OSPF area as stub\n")
{
    //no used
    struct ospf *ospf = vty->index;
    struct ospf_area *area = (struct ospf_area *)(vty->index_sub);
    ospf_area_stub_unset(ospf, area->area_id);
    ospf_area_no_summary_unset(ospf, area->area_id);
    return CMD_SUCCESS;
}

DEFUN(no_ospf_area_stub_no_summary,
      no_ospf_area_stub_no_summary_cmd,
      "no area stub no-summary",
      NO_STR
      "OSPF area parameters\n"
      "Configure OSPF area as stub\n"
      "Do not inject inter-area routes into area\n")
{
    struct ospf *ospf = vty->index;
    struct ospf_area *area = (struct ospf_area *)(vty->index_sub);
    ospf_area_no_summary_unset(ospf, area->area_id);
    return CMD_SUCCESS;
}


static int
ospf_stub_or_nssa_cmd_handler(struct vty *vty, int area_summary, int area_type)
{
    struct ospf *ospf = vty->index;
    int ret ;
    struct ospf_area *area = (struct ospf_area *)(vty->index_sub);
    int format;

    if (!area)
    {
        VTY_CHECK_OSPF_AREA
    }

    if (area_type)/* nssa */
    {
        VTY_GET_OSPF_AREA_ID_NO_BB("NSSA", area->area_id, format, inet_ntoa(area->area_id));

        /* check area if stub */
        if (IS_AREA_STUB(area))
        {
            vty_error_out(vty, "This area has been configured as STUB.%s",
                    VTY_NEWLINE);
            return CMD_WARNING;
        }

        ret = ospf_area_nssa_set(ospf, area->area_id, area_summary);

        if (ret == 0)
        {
            vty_error_out(vty, "Area cannot be nssa as it contains a virtual link%s",
                    VTY_NEWLINE);
            return CMD_WARNING;
        }

        if (area_summary)   /* total */
        {
            ospf_area_no_summary_set(ospf, area->area_id);
        }
        else
        {
            ospf_area_no_summary_unset(ospf, area->area_id);
        }
    }
    else   /* stub */
    {
        VTY_GET_OSPF_AREA_ID_NO_BB("stub", area->area_id, format, inet_ntoa(area->area_id));

        /* check area if nssa */
        if (IS_AREA_NSSA(area))
        {
            vty_error_out(vty, "This area has been configured as NSSA%s",
                    VTY_NEWLINE);
            return CMD_WARNING;
        }

        ret = ospf_area_stub_set(ospf, area->area_id, area_summary);

        if (ret == 0)
        {
            vty_warning_out(vty, "First deconfigure all virtual link through this area%s",
                    VTY_NEWLINE);
            return CMD_WARNING;
        }

        if (area_summary)   /* total */
        {
            ospf_area_no_summary_set(ospf, area->area_id);
        }
        else
        {
            ospf_area_no_summary_unset(ospf, area->area_id);
        }
    }

    return CMD_SUCCESS;
}

static int
ospf_no_stub_or_nssa_cmd_handler(struct vty *vty, int area_summary, int area_type)
{
    struct ospf *ospf = vty->index;
    struct ospf_area *area = (struct ospf_area *)(vty->index_sub);
    int format;

    if (!area)
    {
        return CMD_SUCCESS;
    }

    if (area_type)/*no nssa */
    {
        VTY_GET_OSPF_AREA_ID_NO_BB("NSSA", area->area_id, format, inet_ntoa(area->area_id));

        if (!IS_AREA_NSSA(area))
        {
            vty_error_out(vty, "This area is not a NSSA area or the NSSA function is disabled.%s",
                    VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    else /*stub*/
    {
        /*check area if backbone*/
        VTY_GET_OSPF_AREA_ID_NO_BB("stub", area->area_id, format, inet_ntoa(area->area_id));

        /* check area if nssa */
        if (!IS_AREA_STUB(area))
        {
            vty_error_out(vty, "The area has not been set as STUB area.%s",
                    VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    if (area_summary == 0)
    {
        if (area_type)/*no nssa */
        {
            ospf_area_nssa_unset(ospf, area->area_id);
        }
        else   /*stub*/
        {
            ospf_area_stub_unset(ospf, area->area_id);
        }

        //ospf_area_no_summary_set (ospf, area->area_id);
    }
    else
    {
        ospf_area_no_summary_unset(ospf, area->area_id);
    }

    return CMD_SUCCESS;
}


DEFUN(ospf_area_nssa,
      ospf_area_nssa_cmd,
      "nssa {no-summary}",
      "Configure OSPF area as nssa\n"
      "Do not inject inter-area routes into nssa\n")
{
    int area_type = 1;//init to nssa
	int area_summary = 0;

    if (argv[0] != NULL)
    {
        area_summary = 1;
    }
    else
    {
        area_summary = 0;
    }

    return ospf_stub_or_nssa_cmd_handler(vty, area_summary, area_type);
}

DEFUN(ospf_area_stub_h3c,
	ospf_area_stub_h3c_cmd,
	"stub { default-route-advertise-always | no-summary }",
	"Configure OSPF area as stub\n"
	"Always originate type 3 default LSA into stub area\n"
	"Do not inject inter-area routes into stub\n")
{
	int area_type = 0;//init to stub
	int area_summary = 0;

	if (argv[0] != NULL)
    {
    	;//In order to be adapt with H3C, There is actually no such feature.
    }

	
	if (argv[1] != NULL)
	{
		area_summary = 1;
	}
	else
	{
		area_summary = 0;
	}

    return ospf_stub_or_nssa_cmd_handler(vty, area_summary, area_type);
}



DEFUN(no_ospf_area_stub_nssa,
      no_ospf_area_stub_nssa_cmd,
      "no (stub|nssa) ",
      NO_STR
      "Configure OSPF area as stub\n"
      "Configure OSPF area as nssa\n")
{
    int area_type, area_summary = 0;

    if (strncmp(argv[0], "s", 1) == 0)
    {
        area_type = 0;
    }
    else if (strncmp(argv[0], "n", 1) == 0)
    {
        area_type = 1;
    }
    else
    {
        return CMD_WARNING;
    }

    return ospf_no_stub_or_nssa_cmd_handler(vty, area_summary, area_type);
}

ALIAS(no_ospf_area_stub_nssa,
      no_ospf_area_stub_nssa_h3c_cmd,
      "undo (stub|nssa) ",
      "Cancel current configuration\n"
      "Configure OSPF area as stub\n"
      "Configure OSPF area as nssa\n")


DEFUN(ospf_area_name,
      ospf_area_name_cmd,
      "name STRING",
      "Configure OSPF area name\n"
      "OSPF area name string\n")
{
    struct ospf_area *area = (struct ospf_area *)(vty->index_sub);

    if (!area)
    {
        VTY_CHECK_OSPF_AREA
    }

    if (strlen(argv[0]) > 100)
    {
        vty_error_out(vty, "The description is too long,Please input length smaller than 100%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ospf_name_set(area, argv[0]);
    
    return CMD_SUCCESS;
}


ALIAS(ospf_area_name,
      ospf_area_name_h3c_cmd,
      "description STRING",
      "Specify OSPF area description\n"
      "OSPF area description (no more than 100 characters)\n")


DEFUN(no_ospf_area_name,
      no_ospf_area_name_cmd,
      "no name",
      NO_STR
      "Configure OSPF area name\n")
{
    struct ospf_area *area = (struct ospf_area *)(vty->index_sub);

    if (!area)
    {
        VTY_CHECK_OSPF_AREA
    }

    ospf_name_unset(area);
    return CMD_SUCCESS;
}


ALIAS(no_ospf_area_name,
	no_ospf_area_name_h3c_cmd,
	"undo description",
	"Cancel the current configuration\n"
	"Specify OSPF area description\n")


DEFUN(ospf_area_default_cost,
      ospf_area_default_cost_cmd,
      "default-cost <0-16777214>",
      "Set the summary-default cost of a NSSA or stub area\n"
      "Stub's advertised default summary cost\n")
{
    struct ospf_area *area = (struct ospf_area *)(vty->index_sub);
    u_int32_t cost;
    struct prefix_ipv4 p;

    if (!area)
    {
        VTY_CHECK_OSPF_AREA
    }

    VTY_GET_INTEGER_RANGE("stub default cost", cost, argv[0], 0, 16777214);

    if (area->external_routing == OSPF_AREA_DEFAULT)
    {
        vty_error_out(vty, "The area is neither stub, nor NSSA%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    area->default_cost = cost;

    if (IS_AREA_NSSA(area) && area->no_summary == 0)
    {
        return CMD_SUCCESS;
    }

    p.family = AF_INET;
    p.prefix.s_addr = OSPF_DEFAULT_DESTINATION;
    p.prefixlen = 0;

    if (IS_DEBUG_OSPF_EVENT)
        zlog_debug(OSPF_DBG_EVENT, "ospf_abr_announce_stub_defaults(): "
                   "announcing 0.0.0.0/0 to area %s",
                   inet_ntoa(area->area_id));

    ospf_abr_announce_network_to_area(&p, area->default_cost, area);
    return CMD_SUCCESS;
}

DEFUN(no_ospf_area_default_cost,
      no_ospf_area_default_cost_cmd,
      "no default-cost",
      NO_STR
      "Set the summary-default cost of a NSSA or stub area\n")
{
    struct ospf *ospf = vty->index;
    struct ospf_area *area = (struct ospf_area *)(vty->index_sub);
    struct prefix_ipv4 p;

    if (area == NULL)
    {
        return CMD_SUCCESS;
    }

    if (area->external_routing == OSPF_AREA_DEFAULT)
    {
        vty_error_out(vty, "The area is neither stub, nor NSSA%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    area->default_cost = 1;

    if (IS_AREA_NSSA(area) && area->no_summary == 0)
    {
        return CMD_SUCCESS;
    }

    p.family = AF_INET;
    p.prefix.s_addr = OSPF_DEFAULT_DESTINATION;
    p.prefixlen = 0;

    if (IS_DEBUG_OSPF_EVENT)
        zlog_debug(OSPF_DBG_EVENT, "ospf_abr_announce_stub_defaults(): "
                   "announcing 0.0.0.0/0 to area %s",
                   inet_ntoa(area->area_id));

    ospf_abr_announce_network_to_area(&p, area->default_cost, area);
    ospf_area_check_free(ospf, area->area_id);
    return CMD_SUCCESS;
}


ALIAS(no_ospf_area_default_cost,
      no_ospf_area_default_cost_h3c_cmd,
      "undo default-cost",
      "Cancel current configuration\n"
      "Set the summary-default cost of a NSSA or stub area\n")


DEFUN(ospf_area_export_list,
      ospf_area_export_list_cmd,
      "area export-list NAME",
      "OSPF area parameters\n"
      "Set the filter for networks announced to other areas\n"
      "Name of the access-list\n")
{
    struct ospf *ospf = vty->index;
    struct ospf_area *area = (struct ospf_area *)(vty->index_sub);

    if (!area)
    {
        VTY_CHECK_OSPF_AREA
    }

    ospf_area_export_list_set(ospf, area, argv[0]);
    return CMD_SUCCESS;
}

DEFUN(no_ospf_area_export_list,
      no_ospf_area_export_list_cmd,
      "no area export-list NAME",
      NO_STR
      "OSPF area parameters\n"
      "Unset the filter for networks announced to other areas\n"
      "Name of the access-list\n")
{
    struct ospf *ospf = vty->index;
    struct ospf_area *area = (struct ospf_area *)(vty->index_sub);

    if (area == NULL)
    {
        return CMD_WARNING;
    }

    ospf_area_export_list_unset(ospf, area);
    return CMD_SUCCESS;
}


DEFUN(ospf_area_import_list,
      ospf_area_import_list_cmd,
      "area import-list NAME",
      "OSPF area parameters\n"
      "Set the filter for networks from other areas announced to the specified one\n"
      "Name of the access-list\n")
{
    struct ospf *ospf = vty->index;
    struct ospf_area *area = (struct ospf_area *)(vty->index_sub);

    if (!area)
    {
        VTY_CHECK_OSPF_AREA
    }

    ospf_area_import_list_set(ospf, area, argv[0]);
    return CMD_SUCCESS;
}

DEFUN(no_ospf_area_import_list,
      no_ospf_area_import_list_cmd,
      "no area import-list NAME",
      NO_STR
      "OSPF area parameters\n"
      "Unset the filter for networks announced to other areas\n"
      "Name of the access-list\n")
{
    struct ospf *ospf = vty->index;
    struct ospf_area *area = (struct ospf_area *)(vty->index_sub);

    if (area == NULL)
    {
        return CMD_WARNING;
    }

    ospf_area_import_list_unset(ospf, area);
    return CMD_SUCCESS;
}

DEFUN(ospf_area_filter_list,
      ospf_area_filter_list_cmd,
      "area filter-list prefix WORD (in|out)",
      "OSPF area parameters\n"
      "Filter networks between OSPF areas\n"
      "Filter prefixes between OSPF areas\n"
      "Name of an IP prefix-list\n"
      "Filter networks sent to this area\n"
      "Filter networks sent from this area\n")
{
    struct ospf *ospf = vty->index;
    struct ospf_area *area = (struct ospf_area *)(vty->index_sub);
    struct prefix_list *plist = NULL;

    if (!area)
    {
        VTY_CHECK_OSPF_AREA
    }

    plist = prefix_list_lookup(AFI_IP, argv[0]);

    if (strncmp(argv[1], "in", 2) == 0)
    {
        PREFIX_LIST_IN(area) = plist;

        if (PREFIX_NAME_IN(area))
        {
            free(PREFIX_NAME_IN(area));
        }

        PREFIX_NAME_IN(area) = strdup(argv[0]);
        ospf_schedule_abr_task(ospf);
    }
    else
    {
        PREFIX_LIST_OUT(area) = plist;

        if (PREFIX_NAME_OUT(area))
        {
            free(PREFIX_NAME_OUT(area));
        }

        PREFIX_NAME_OUT(area) = strdup(argv[0]);
        ospf_schedule_abr_task(ospf);
    }

    return CMD_SUCCESS;
}

DEFUN(no_ospf_area_filter_list,
      no_ospf_area_filter_list_cmd,
      "no area filter-list prefix WORD (in|out)",
      NO_STR
      "OSPF area parameters\n"
      "Filter networks between OSPF areas\n"
      "Filter prefixes between OSPF areas\n"
      "Name of an IP prefix-list\n"
      "Filter networks sent to this area\n"
      "Filter networks sent from this area\n")
{
    struct ospf *ospf = vty->index;
    struct ospf_area *area = (struct ospf_area *)(vty->index_sub);

    if (area  == NULL)
    {
        return CMD_WARNING;
    }

    if (strncmp(argv[1], "in", 2) == 0)
    {
        if (PREFIX_NAME_IN(area))
            if (strcmp(PREFIX_NAME_IN(area), argv[0]) != 0)
            {
                return CMD_SUCCESS;
            }

        PREFIX_LIST_IN(area) = NULL;

        if (PREFIX_NAME_IN(area))
        {
            free(PREFIX_NAME_IN(area));
        }

        PREFIX_NAME_IN(area) = NULL;
        ospf_schedule_abr_task(ospf);
    }
    else
    {
        if (PREFIX_NAME_OUT(area))
            if (strcmp(PREFIX_NAME_OUT(area), argv[0]) != 0)
            {
                return CMD_SUCCESS;
            }

        PREFIX_LIST_OUT(area) = NULL;

        if (PREFIX_NAME_OUT(area))
        {
            free(PREFIX_NAME_OUT(area));
        }

        PREFIX_NAME_OUT(area) = NULL;
        ospf_schedule_abr_task(ospf);
    }

    return CMD_SUCCESS;
}


DEFUN(ospf_area_authentication_message_digest,
      ospf_area_authentication_message_digest_cmd,
      "authentication-mode (hmac-md5|md5) <1-255> (cipher|plain) PASSWORD",
      "Specify an authentication mode\n"
      "Use HMAC-MD5 algorithm\n"
      "Use MD5 algorithm\n"
	  "Key ID\n"
      "Encryption type (Cryptogram)\n"
	  "Encryption type (Plain text)\n"
	  "The password (length 1~255 bytes)\n")
{
	u_char key_id = 0;
	struct crypt_key *ck;
    struct ospf_area *area = NULL;
	
	area = (struct ospf_area *)(vty->index_sub);

    if (!area)
    {
        VTY_CHECK_OSPF_AREA
    }

	if (strlen(argv[3]) > 255)
    {
        vty_error_out(vty, "OSPF md5 auth PASSWORD length must be 1~255 bytes%s",
                VTY_NEWLINE);
        return CMD_WARNING;
    }
	
	//clean old auth
	ospf_area_auth_md5_delete_all(area);
    memset(area->auth_simple, 0, OSPF_AUTH_SIMPLE_SIZE + 1);
    area->auth_simple__config = AREA_AUTH_PARAM_CLEAN;
	area->auth_type = OSPF_AUTH_NULL;
    area->auth_type__config = AREA_AUTH_PARAM_CLEAN;

	//new auth
    area->auth_type = OSPF_AUTH_CRYPTOGRAPHIC;
	area->auth_type__config = AREA_AUTH_PARAM_CONFIG;

	//get key_id
	key_id = strtol(argv[1], NULL, 10);
	if (ospf_crypt_key_lookup(area->auth_crypt, key_id) != NULL)
    {
        return CMD_SUCCESS;
    }

	//creat md5 struct
	ck = ospf_crypt_key_new();
    ck->key_id = (u_char) key_id;
    memset(ck->auth_key, 0, OSPF_AUTH_MD5_SIZE + 1);
	strncpy((char *) ck->auth_key, argv[3], OSPF_AUTH_MD5_SIZE);
	ck->auth_key_len = strlen(argv[3]);

    if (ck->auth_key_len < OSPF_MD5_SIZE)
    {
        ck->auth_key_len = OSPF_MD5_SIZE;
    }

    ospf_crypt_key_add(area->auth_crypt, ck);
    area->auth_crypt__config = AREA_AUTH_PARAM_CONFIG;
    return CMD_SUCCESS;
}


DEFUN(ospf_area_authentication,
      ospf_area_authentication_cmd,
      "authentication-mode simple (cipher|plain) PASSWORD",
      "Specify an authentication mode\n"
      "Simple authentication mode\n"
	  "Encryption type (Cryptogram)\n"
	  "Encryption type (Plain text)\n"
	  "The password (length 1~8 bytes)\n")
{
    struct ospf_area *area = (struct ospf_area *)(vty->index_sub);

    if (!area)
    {
        VTY_CHECK_OSPF_AREA
    }
	
	if (strlen(argv[1]) > 8)
    {
        vty_error_out(vty, "OSPF auth PASSWORD length must be 1~8 bytes%s",
                VTY_NEWLINE);
        return CMD_WARNING;
    }

	ospf_area_auth_md5_delete_all(area);
    area->auth_type = OSPF_AUTH_SIMPLE;
	area->auth_type__config = AREA_AUTH_PARAM_CONFIG;
	memset(area->auth_simple, 0, OSPF_AUTH_SIMPLE_SIZE + 1);
	strncpy((char *) area->auth_simple, argv[1], OSPF_AUTH_SIMPLE_SIZE);
    area->auth_simple__config = AREA_AUTH_PARAM_CONFIG;
    return CMD_SUCCESS;
}

DEFUN(no_ospf_area_authentication,
      no_ospf_area_authentication_cmd,
      "no authentication-mode",
      NO_STR
      "Specify an authentication mode\n")
{
    struct ospf_area *area = (struct ospf_area *)(vty->index_sub);

    if (!area)
    {
        VTY_CHECK_OSPF_AREA
    }

	/*delete all md5 key*/
    ospf_area_auth_md5_delete_all(area);
    area->auth_type = OSPF_AUTH_NULL;
    area->auth_type__config = AREA_AUTH_PARAM_CLEAN;
    memset(area->auth_simple, 0, OSPF_AUTH_SIMPLE_SIZE + 1);
    area->auth_simple__config = AREA_AUTH_PARAM_CLEAN;

    ospf_area_check_free(area->ospf, area->area_id);
    return CMD_SUCCESS;
}

ALIAS(no_ospf_area_authentication,
      undo_ospf_area_simple_authentication_cmd,
	"undo authentication-mode",
	"Cancel current configuration\n"
	"Specify an authentication mode\n")


ALIAS(no_ospf_area_authentication,
	undo_ospf_area_md5_authentication_cmd,
	"undo authentication-mode (hmac-md5|md5) <1-255>",
	"Cancel current configuration\n"
	"Specify an authentication mode\n"
	"Use HMAC-MD5 algorithm\n"
	"Use MD5 algorithm\n"
	"Key ID\n")


DEFUN(ospf_abr_type,
      ospf_abr_type_cmd,
      "ospf abr-type (cisco|ibm|shortcut|standard)",
      "OSPF specific commands\n"
      "Set OSPF ABR type\n"
      "Alternative ABR, cisco implementation\n"
      "Alternative ABR, IBM implementation\n"
      "Shortcut ABR\n"
      "Standard behavior (RFC2328)\n")
{
    struct ospf *ospf = vty->index;
    u_char abr_type = OSPF_ABR_UNKNOWN;

    if (strncmp(argv[0], "c", 1) == 0)
    {
        abr_type = OSPF_ABR_CISCO;
    }
    else if (strncmp(argv[0], "i", 1) == 0)
    {
        abr_type = OSPF_ABR_IBM;
    }
    else if (strncmp(argv[0], "sh", 2) == 0)
    {
        abr_type = OSPF_ABR_SHORTCUT;
    }
    else if (strncmp(argv[0], "st", 2) == 0)
    {
        abr_type = OSPF_ABR_STAND;
    }
    else
    {
        return CMD_WARNING;
    }

    /* If ABR type value is changed, schedule ABR task. */
    if (ospf->abr_type != abr_type)
    {
        ospf->abr_type = abr_type;
        ospf_schedule_abr_task(ospf);
    }

    return CMD_SUCCESS;
}

DEFUN(no_ospf_abr_type,
      no_ospf_abr_type_cmd,
      "no ospf abr-type (cisco|ibm|shortcut|standard)",
      NO_STR
      "OSPF specific commands\n"
      "Set OSPF ABR type\n"
      "Alternative ABR, cisco implementation\n"
      "Alternative ABR, IBM implementation\n"
      "Shortcut ABR\n")
{
    struct ospf *ospf = vty->index;
    u_char abr_type = OSPF_ABR_UNKNOWN;

    if (strncmp(argv[0], "c", 1) == 0)
    {
        abr_type = OSPF_ABR_CISCO;
    }
    else if (strncmp(argv[0], "i", 1) == 0)
    {
        abr_type = OSPF_ABR_IBM;
    }
    else if (strncmp(argv[0], "sh", 2) == 0)
    {
        abr_type = OSPF_ABR_SHORTCUT;
    }
    else if (strncmp(argv[0], "st", 2) == 0)
    {
        abr_type = OSPF_ABR_STAND;
    }
    else
    {
        return CMD_WARNING;
    }

    /* If ABR type value is changed, schedule ABR task. */
    if (ospf->abr_type == abr_type)
    {
        ospf->abr_type = OSPF_ABR_DEFAULT;
        ospf_schedule_abr_task(ospf);
    }

    return CMD_SUCCESS;
}

DEFUN(ospf_log_adjacency_changes,
      ospf_log_adjacency_changes_cmd,
      "log-adjacency-changes",
      "Log changes in adjacency state\n")
{
    struct ospf *ospf = vty->index;
    SET_FLAG(ospf->config, OSPF_LOG_ADJACENCY_CHANGES);
    return CMD_SUCCESS;
}

DEFUN(ospf_log_adjacency_changes_detail,
      ospf_log_adjacency_changes_detail_cmd,
      "log-adjacency-changes detail",
      "Log changes in adjacency state\n"
      "Log all state changes\n")
{
    struct ospf *ospf = vty->index;
    SET_FLAG(ospf->config, OSPF_LOG_ADJACENCY_CHANGES);
    SET_FLAG(ospf->config, OSPF_LOG_ADJACENCY_DETAIL);
    return CMD_SUCCESS;
}

DEFUN(no_ospf_log_adjacency_changes,
      no_ospf_log_adjacency_changes_cmd,
      "no log-adjacency-changes",
      NO_STR
      "Log changes in adjacency state\n")
{
    struct ospf *ospf = vty->index;
    UNSET_FLAG(ospf->config, OSPF_LOG_ADJACENCY_DETAIL);
    UNSET_FLAG(ospf->config, OSPF_LOG_ADJACENCY_CHANGES);
    return CMD_SUCCESS;
}

DEFUN(no_ospf_log_adjacency_changes_detail,
      no_ospf_log_adjacency_changes_detail_cmd,
      "no log-adjacency-changes detail",
      NO_STR
      "Log changes in adjacency state\n"
      "Log all state changes\n")
{
    struct ospf *ospf = vty->index;
    UNSET_FLAG(ospf->config, OSPF_LOG_ADJACENCY_DETAIL);
    return CMD_SUCCESS;
}

DEFUN(ospf_compatible_rfc1583,
      ospf_compatible_rfc1583_cmd,
      "compatible rfc1583",
      "OSPF compatibility list\n"
      "compatible with RFC 1583\n")
{
    struct ospf *ospf = vty->index;

    if (!CHECK_FLAG(ospf->config, OSPF_RFC1583_COMPATIBLE))
    {
        SET_FLAG(ospf->config, OSPF_RFC1583_COMPATIBLE);
        ospf_spf_calculate_schedule(ospf, SPF_FLAG_CONFIG_CHANGE);
    }

    return CMD_SUCCESS;
}


ALIAS(ospf_compatible_rfc1583,
      ospf_compatible_rfc1583_h3c_cmd,
      "rfc1583 compatible",
      "compatible with RFC 1583\n"
      "OSPF compatibility list\n")


DEFUN(no_ospf_compatible_rfc1583,
      no_ospf_compatible_rfc1583_cmd,
      "no compatible rfc1583",
      NO_STR
      "OSPF compatibility list\n"
      "compatible with RFC 1583\n")
{
    struct ospf *ospf = vty->index;

    if (CHECK_FLAG(ospf->config, OSPF_RFC1583_COMPATIBLE))
    {
        UNSET_FLAG(ospf->config, OSPF_RFC1583_COMPATIBLE);
        ospf_spf_calculate_schedule(ospf, SPF_FLAG_CONFIG_CHANGE);
    }

    return CMD_SUCCESS;
}


ALIAS(no_ospf_compatible_rfc1583,
      no_ospf_compatible_rfc1583_h3c_cmd,
      "undo rfc1583 compatible",
      "Cancel the current setting\n"
      "compatible with RFC 1583\n"
      "OSPF compatibility list\n")



static int
ospf_timers_spf_set(struct vty *vty, unsigned int delay,
                    unsigned int hold,
                    unsigned int max)
{
    struct ospf *ospf = vty->index;
    ospf->spf_delay = delay;
    ospf->spf_holdtime = hold;
    ospf->spf_max_holdtime = max;
    return CMD_SUCCESS;
}

DEFUN(ospf_timers_min_ls_interval,
      ospf_timers_min_ls_interval_cmd,
      "timers throttle lsa all <0-5000>",
      "Adjust routing timers\n"
      "Throttling adaptive timer\n"
      "LSA delay between transmissions\n"
      NO_STR
      "Delay (msec) between sending LSAs\n")
{
    struct ospf *ospf = vty->index;
    unsigned int interval;

    if (argc != 1)
    {
        vty_error_out(vty, "Insufficient arguments%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    VTY_GET_INTEGER("LSA interval", interval, argv[0]);
    ospf->min_ls_interval = interval;
    return CMD_SUCCESS;
}

DEFUN(no_ospf_timers_min_ls_interval,
      no_ospf_timers_min_ls_interval_cmd,
      "no timers throttle lsa all",
      NO_STR
      "Adjust routing timers\n"
      "Throttling adaptive timer\n"
      "LSA delay between transmissions\n")
{
    struct ospf *ospf = vty->index;
    ospf->min_ls_interval = OSPF_MIN_LS_INTERVAL;
    return CMD_SUCCESS;
}

DEFUN(ospf_timers_min_ls_arrival,
      ospf_timers_min_ls_arrival_cmd,
      "timers lsa arrival <0-1000>",
      "Adjust routing timers\n"
      "Throttling link state advertisement delays\n"
      "OSPF minimum arrival interval delay\n"
      "Delay (msec) between accepted LSAs\n")
{
    struct ospf *ospf = vty->index;
    unsigned int arrival;

    if (argc != 1)
    {
        vty_error_out(vty, "Insufficient arguments%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    VTY_GET_INTEGER_RANGE("minimum LSA inter-arrival time", arrival, argv[0], 0, 1000);
    ospf->min_ls_arrival = arrival;
    return CMD_SUCCESS;
}

DEFUN(no_ospf_timers_min_ls_arrival,
      no_ospf_timers_min_ls_arrival_cmd,
      "no timers lsa arrival",
      NO_STR
      "Adjust routing timers\n"
      "Throttling link state advertisement delays\n"
      "OSPF minimum arrival interval delay\n")
{
    struct ospf *ospf = vty->index;
    ospf->min_ls_arrival = OSPF_MIN_LS_ARRIVAL;
    return CMD_SUCCESS;
}

DEFUN(ospf_timers_throttle_spf,
      ospf_timers_throttle_spf_cmd,
      "timers throttle spf <0-600000> <0-600000> <0-600000>",
      "Adjust routing timers\n"
      "Throttling adaptive timer\n"
      "OSPF SPF timers\n"
      "Delay (msec) from first change received till SPF calculation\n"
      "Initial hold time (msec) between consecutive SPF calculations\n"
      "Maximum hold time (msec)\n")
{
    unsigned int delay, hold, max;

    if (argc != 3)
    {
        vty_error_out(vty, "Insufficient arguments%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    VTY_GET_INTEGER_RANGE("SPF delay timer", delay, argv[0], 0, 600000);
    VTY_GET_INTEGER_RANGE("SPF hold timer", hold, argv[1], 0, 600000);
    VTY_GET_INTEGER_RANGE("SPF max-hold timer", max, argv[2], 0, 600000);
    return ospf_timers_spf_set(vty, delay, hold, max);
}

DEFUN_DEPRECATED(ospf_timers_spf,
                 ospf_timers_spf_cmd,
                 "timers spf <0-4294967295> <0-4294967295>",
                 "Adjust routing timers\n"
                 "OSPF SPF timers\n"
                 "Delay (s) between receiving a change to SPF calculation\n"
                 "Hold time (s) between consecutive SPF calculations\n")
{
    unsigned int delay, hold;

    if (argc != 2)
    {
        vty_error_out(vty, "Insufficient number of arguments%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    VTY_GET_INTEGER("SPF delay timer", delay, argv[0]);
    VTY_GET_INTEGER("SPF hold timer", hold, argv[1]);

    /* truncate down the second values if they're greater than 600000ms */
    if (delay > (600000 / 1000))
    {
        delay = 600000;
    }
    else if (delay == 0)
        /* 0s delay was probably specified because of lack of ms resolution */
    {
        delay = OSPF_SPF_DELAY_DEFAULT;
    }

    if (hold > (600000 / 1000))
    {
        hold = 600000;
    }

    return ospf_timers_spf_set(vty, delay * 1000, hold * 1000, hold * 1000);
}

DEFUN(no_ospf_timers_throttle_spf,
      no_ospf_timers_throttle_spf_cmd,
      "no timers throttle spf",
      NO_STR
      "Adjust routing timers\n"
      "Throttling adaptive timer\n"
      "OSPF SPF timers\n")
{
    return ospf_timers_spf_set(vty,
                               OSPF_SPF_DELAY_DEFAULT,
                               OSPF_SPF_HOLDTIME_DEFAULT,
                               OSPF_SPF_MAX_HOLDTIME_DEFAULT);
}

ALIAS_DEPRECATED(no_ospf_timers_throttle_spf,
                 no_ospf_timers_spf_cmd,
                 "no timers spf",
                 NO_STR
                 "Adjust routing timers\n"
                 "OSPF SPF timers\n")


DEFUN(ospf_neighbor_poll_interval,
      ospf_neighbor_poll_interval_cmd,
      "neighbor A.B.C.D {poll-interval <1-65535>}",
      NEIGHBOR_STR
      "Neighbor IP address\n"
      "Dead Neighbor Polling interval\n"
      "Seconds\n")
{
    struct ospf *ospf = vty->index;
    struct in_addr nbr_addr;
    unsigned int interval = OSPF_POLL_INTERVAL_DEFAULT;
    int ret;

    /*check A.B.C.D*/
    VTY_CHECK_OSPF_ADDRESS("neighbor address", argv[0]);
    /* Change address format */
    VTY_GET_IPV4_ADDRESS("neighbor address", nbr_addr, argv[0]);
    /* check address */
    ret = ospf_check_input_ip_valid(nbr_addr.s_addr);

    if (ERRNO_SUCCESS != ret || 0 == nbr_addr.s_addr)
    {
        vty_error_out(vty, "The IP address can not be configured as the address of the OSPF neighbor%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (argv[1])
    {
        VTY_GET_INTEGER_RANGE("poll interval", interval, argv[1], 1, 65535);
    }

    ospf_nbr_nbma_set(ospf, nbr_addr);

    if (argv[1])
    {
        ospf_nbr_nbma_poll_interval_set(ospf, nbr_addr, interval);
    }

    return CMD_SUCCESS;
}

ALIAS(ospf_neighbor_poll_interval,
	ospf_neighbor_poll_interval_h3c_cmd,
	"peer A.B.C.D {poll-interval <1-65535>}",
	"Specify a neighbor router\n"
	"Neighbor IP address\n"
	"Dead Neighbor Polling interval\n"
    "Seconds\n")


DEFUN(ospf_neighbor_cost_h3c,
	ospf_neighbor_cost_h3c_cmd,
	"peer A.B.C.D cost <1-65535>",
	"Specify a neighbor router\n"
	"Neighbor IP address\n"
	"OSPF cost for point-to-multipoint neighbor\n"
	"Neighbor cost\n")
{
    int ret = -1;
	int cost_val = 0;
    struct in_addr nbr_addr;
	struct ospf *ospf = vty->index;

    /*check A.B.C.D*/
    VTY_CHECK_OSPF_ADDRESS("neighbor address", argv[0]);
    /* Change address format */
    VTY_GET_IPV4_ADDRESS("neighbor address", nbr_addr, argv[0]);
    /* check address */
    ret = ospf_check_input_ip_valid(nbr_addr.s_addr);

    if (ERRNO_SUCCESS != ret || 0 == nbr_addr.s_addr)
    {
        vty_error_out(vty, "The IP address can not be configured as the address of the OSPF neighbor%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	if (argv[1] != NULL)
	{
        VTY_GET_INTEGER_RANGE("peer dr-priority", cost_val, argv[1], 1, 65535);
		cost_val = cost_val;//Only to clear compilation alarms
	}
	
    ospf_nbr_nbma_set(ospf, nbr_addr);

    return CMD_SUCCESS;
}


DEFUN(ospf_neighbor_dr_priority_h3c,
	ospf_neighbor_dr_priority_h3c_cmd,
	"peer A.B.C.D dr-priority <0-255>",
	"Specify a neighbor router\n"
	"Neighbor IP address\n"
	"OSPF cost for point-to-multipoint neighbor\n"
	"Neighbor cost\n"
	"Router priority\n"
	"Router priority value\n")
{
	int ret = -1;
	int priority_val = 0;
	struct in_addr nbr_addr;
	struct ospf *ospf = vty->index;

	/*check A.B.C.D*/
	VTY_CHECK_OSPF_ADDRESS("neighbor address", argv[0]);
	/* Change address format */
	VTY_GET_IPV4_ADDRESS("neighbor address", nbr_addr, argv[0]);
	/* check address */
	ret = ospf_check_input_ip_valid(nbr_addr.s_addr);

	if (ERRNO_SUCCESS != ret || 0 == nbr_addr.s_addr)
	{
		vty_error_out(vty, "The IP address can not be configured as the address of the OSPF neighbor%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if (argv[1] != NULL)
    {
        VTY_GET_INTEGER_RANGE("peer dr-priority", priority_val, argv[1], 0, 255);
		priority_val = priority_val;//Only to clear compilation alarms
    }
	
	ospf_nbr_nbma_set(ospf, nbr_addr);

	return CMD_SUCCESS;
}



DEFUN(no_ospf_neighbor,
      no_ospf_neighbor_cmd,
      "no neighbor A.B.C.D",
      NO_STR
      NEIGHBOR_STR
      "Neighbor IP address\n")
{
    struct ospf *ospf = vty->index;
    struct in_addr nbr_addr;
    int ret;
    /*check A.B.C.D*/
    VTY_CHECK_OSPF_ADDRESS("neighbor address", argv[0]);
    /* Change address format */
    VTY_GET_IPV4_ADDRESS("neighbor address", nbr_addr, argv[0]);
    /* check address */
    ret = ospf_check_input_ip_valid(nbr_addr.s_addr);

    if (ERRNO_SUCCESS != ret || 0 == nbr_addr.s_addr)
    {
        vty_warning_out(vty, "Please specify neighbor address by A.B.C.D%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ret = ospf_nbr_nbma_unset(ospf, nbr_addr);

    if (ret == ERRNO_FAIL)
    {
        vty_error_out(vty, "This neighbor does not exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


ALIAS(no_ospf_neighbor,
	no_ospf_neighbor_h3c_cmd,
	"undo peer A.B.C.D",
	"Cancel the current setting\n"
	"Specify a neighbor router\n"
	"Neighbor IP address\n")


DEFUN(ospf_refresh_interval, ospf_refresh_interval_cmd,
      "refresh interval <10-1800>",
      "Adjust refresh parameters\n"
      "Set refresh parameter\n"
      "Refresh value in seconds\n")
{
    struct ospf *ospf = vty->index;
    unsigned int interval;

    VTY_GET_INTEGER_RANGE("refresh interval", interval, argv[0], 10, 1800);
    interval = (interval / 10) * 10;
    ospf_refresh_interval_set(ospf, interval);
    return CMD_SUCCESS;
}

DEFUN(no_ospf_refresh_interval, no_ospf_refresh_interval_cmd,
      "no refresh interval",
      NO_STR
      "Adjust refresh parameters\n"
      "Set refresh default parameter\n")
{
    struct ospf *ospf = vty->index;
    ospf->refresh_interval = OSPF_LS_REFRESH_TIME;
    return CMD_SUCCESS;
}



DEFUN(ospf_refresh_timer, ospf_refresh_timer_cmd,
      "refresh timer <10-1800>",
      "Adjust refresh parameters\n"
      "Set refresh timer\n"
      "Timer value in seconds\n")
{
    struct ospf *ospf = vty->index;
    unsigned int interval;
    VTY_GET_INTEGER_RANGE("refresh timer", interval, argv[0], 10, 1800);
    interval = (interval / 10) * 10;
    ospf_timers_refresh_set(ospf, interval);
    return CMD_SUCCESS;
}

DEFUN(no_ospf_refresh_timer, no_ospf_refresh_timer_val_cmd,
      "no refresh timer <10-1800>",
      "Adjust refresh parameters\n"
      "Unset refresh timer\n"
      "Timer value in seconds\n")
{
    struct ospf *ospf = vty->index;
    unsigned int interval;

    if (argc == 1)
    {
        VTY_GET_INTEGER_RANGE("refresh timer", interval, argv[0], 10, 1800);

        if (ospf->lsa_refresh_interval != interval ||
                interval == OSPF_LSA_REFRESH_INTERVAL_DEFAULT)
        {
            return CMD_SUCCESS;
        }
    }

    ospf_timers_refresh_unset(ospf);
    return CMD_SUCCESS;
}

ALIAS(no_ospf_refresh_timer,
      no_ospf_refresh_timer_cmd,
      "no refresh timer",
      "Adjust refresh parameters\n"
      "Unset refresh timer\n")

DEFUN(ospf_auto_cost_reference_bandwidth,
      ospf_auto_cost_reference_bandwidth_cmd,
      "reference-bandwidth <1-4294967>",
      "Use reference bandwidth method to assign OSPF cost\n"
      "The reference bandwidth in terms of Mbits per second\n")
{
    struct ospf *ospf = vty->index;
    u_int32_t refbw = 0;
	u_int32_t newcost = 0;
    struct listnode *node = NULL;
	struct listnode *nnode = NULL;
    //struct interface *ifp = NULL;
	struct ospf_interface *oi = NULL;
	
    refbw = strtol(argv[0], NULL, 10);

    if (refbw < 1 || refbw > 4294967)
    {
        vty_error_out(vty, "reference-bandwidth value is invalid%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /* If reference bandwidth is changed. */
    if ((refbw * 100) == ospf->ref_bandwidth)
    {
        return CMD_SUCCESS;
    }

    ospf->ref_bandwidth = refbw * 100;

	/*del by zzl 2019.2.28, because the logic is wrong*/
    /*for (ALL_LIST_ELEMENTS_RO(om->iflist, node, ifp))
    {
        ospf_if_recalculate_output_cost(ifp);
    }*/

	for (ALL_LIST_ELEMENTS(ospf->oiflist, node, nnode, oi))
    {
        newcost = ospf_if_get_output_cost (oi);
        /* Is actual output cost changed? */
        if (oi->output_cost != newcost)
        {
            oi->output_cost = newcost;
            ospf_router_lsa_update_area (oi->area);
        }
    }

    return CMD_SUCCESS;
}


ALIAS(ospf_auto_cost_reference_bandwidth,
      ospf_auto_cost_reference_bandwidth_h3c_cmd,
      "bandwidth-reference <1-4294967>",
      "Use reference bandwidth method to assign OSPF cost\n"
      "The reference bandwidth in terms of Mbits per second\n")


DEFUN(no_ospf_auto_cost_reference_bandwidth,
      no_ospf_auto_cost_reference_bandwidth_cmd,
      "no reference-bandwidth",
      NO_STR
      "Use reference bandwidth method to assign OSPF cost\n")
{
    struct ospf *ospf = vty->index;
	u_int32_t newcost = 0;
    struct listnode *node = NULL;
	struct listnode *nnode = NULL;
    //struct interface *ifp = NULL;
	struct ospf_interface *oi = NULL;

    if (ospf->ref_bandwidth == OSPF_DEFAULT_REF_BANDWIDTH)
    {
        return CMD_SUCCESS;
    }

    ospf->ref_bandwidth = OSPF_DEFAULT_REF_BANDWIDTH;
    vty_out(vty, "OSPF: Reference bandwidth is changed.%s", VTY_NEWLINE);
    vty_out(vty, "        Please ensure reference bandwidth is consistent across all routers%s", VTY_NEWLINE);

    /*for (ALL_LIST_ELEMENTS(om->iflist, node, nnode, ifp))
    {
        ospf_if_recalculate_output_cost(ifp);
    }*/
    for (ALL_LIST_ELEMENTS(ospf->oiflist, node, nnode, oi))
    {
        newcost = ospf_if_get_output_cost (oi);
        /* Is actual output cost changed? */
        if (oi->output_cost != newcost)
        {
            oi->output_cost = newcost;
            ospf_router_lsa_update_area (oi->area);
        }
    }

    return CMD_SUCCESS;
}


ALIAS(no_ospf_auto_cost_reference_bandwidth,
	no_ospf_auto_cost_reference_bandwidth_h3c_cmd,
	"undo bandwidth-reference",
	"Cancel the current setting\n"
	"Use reference bandwidth method to assign OSPF cost\n")


const char *ospf_abr_type_descr_str[] =
{
    "Unknown",
    "Standard (RFC2328)",
    "Alternative IBM",
    "Alternative Cisco",
    "Alternative Shortcut"
};

const char *ospf_shortcut_mode_descr_str[] =
{
    "Default",
    "Enabled",
    "Disabled"
};



static void
show_ip_ospf_area(struct vty *vty, struct ospf_area *area)
{
    /* Show Area ID. */
    vty_out(vty, " Area ID: %s", inet_ntoa(area->area_id));

    /* Show Area type/mode. */
    if (OSPF_IS_AREA_BACKBONE(area))
    {
        vty_out(vty, " (Backbone)%s", VTY_NEWLINE);
    }
    else
    {
        if (area->external_routing == OSPF_AREA_STUB)
            vty_out(vty, " (Stub%s%s)",
                    area->no_summary ? ", no summary" : "",
                    area->shortcut_configured ? "; " : "");
        else if (area->external_routing == OSPF_AREA_NSSA)
            vty_out(vty, " (NSSA%s%s)",
                    area->no_summary ? ", no summary" : "",
                    area->shortcut_configured ? "; " : "");

        vty_out(vty, "%s", VTY_NEWLINE);
        vty_out(vty, "   Shortcutting mode: %s",
                ospf_shortcut_mode_descr_str[area->shortcut_configured]);
        vty_out(vty, ", S-bit consensus: %s%s",
                area->shortcut_capability ? "ok" : "no", VTY_NEWLINE);
    }

    if (area->area_name != NULL)
    {
        vty_out(vty, "   Area Name: %s %s", area->area_name, VTY_NEWLINE);
    }

    /* Show number of interfaces. */
    vty_out(vty, "   Number of interfaces in this area: Total: %d, "
            "Active: %d%s", listcount(area->oiflist),
            area->act_ints, VTY_NEWLINE);

    if (area->external_routing == OSPF_AREA_NSSA)
    {
        vty_out(vty, "   It is an NSSA configuration. %s   Elected NSSA/ABR performs type-7/type-5 LSA translation. %s", VTY_NEWLINE, VTY_NEWLINE);

        if (! IS_OSPF_ABR(area->ospf))
            vty_out(vty, "   It is not ABR, therefore not Translator. %s",
                    VTY_NEWLINE);
        else if (area->NSSATranslatorState)
        {
            vty_out(vty, "   We are an ABR and ");

            if (area->NSSATranslatorRole == OSPF_NSSA_ROLE_CANDIDATE)
                vty_out(vty, "the NSSA Elected Translator. %s",
                        VTY_NEWLINE);
            else if (area->NSSATranslatorRole == OSPF_NSSA_ROLE_ALWAYS)
                vty_out(vty, "always an NSSA Translator. %s",
                        VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, "   We are an ABR, but ");

            if (area->NSSATranslatorRole == OSPF_NSSA_ROLE_CANDIDATE)
                vty_out(vty, "not the NSSA Elected Translator. %s",
                        VTY_NEWLINE);
            else
                vty_out(vty, "never an NSSA Translator. %s",
                        VTY_NEWLINE);
        }
    }

    /* Stub-router state for this area */
    if (CHECK_FLAG(area->stub_router_state, OSPF_AREA_IS_STUB_ROUTED))
    {
        char timebuf[OSPF_TIME_DUMP_SIZE];
        vty_out(vty, "   Originating stub / maximum-distance Router-LSA%s",
                VTY_NEWLINE);

        if (CHECK_FLAG(area->stub_router_state, OSPF_AREA_ADMIN_STUB_ROUTED))
            vty_out(vty, "     Administratively activated (indefinitely)%s",
                    VTY_NEWLINE);

        if (area->t_stub_router)
        {
            /*vty_out(vty, "     Active from startup, %s remaining%s",
                    ospf_timer_dump(area->t_stub_router, timebuf,
                                    sizeof(timebuf)), VTY_NEWLINE);*/
			vty_out(vty, "     Active from startup, %s remaining%s",
                    ospf_timer_dump_new(area->t_stub_router, area->ospf->stub_router_startup_time,
				                    timebuf,sizeof(timebuf)), VTY_NEWLINE);
        }
    }

    /* Show number of fully adjacent neighbors. */
    vty_out(vty, "   Number of fully adjacent neighbors in this area:"
            " %d%s", area->full_nbrs, VTY_NEWLINE);
    /* Show authentication type. */
    vty_out(vty, "   Area has ");

    if (area->auth_type == OSPF_AUTH_NULL)
    {
        vty_out(vty, "no authentication%s", VTY_NEWLINE);
    }
    else if (area->auth_type == OSPF_AUTH_SIMPLE)
    {
        vty_out(vty, "simple password authentication%s", VTY_NEWLINE);
    }
    else if (area->auth_type == OSPF_AUTH_CRYPTOGRAPHIC)
    {
        vty_out(vty, "message digest authentication%s", VTY_NEWLINE);
    }

    if (!OSPF_IS_AREA_BACKBONE(area))
        vty_out(vty, "   Number of full virtual adjacencies going through"
                " this area: %d%s", area->full_vls, VTY_NEWLINE);

    /* Show SPF calculation times. */
    vty_out(vty, "   SPF algorithm executed %d times%s",
            area->spf_calculation, VTY_NEWLINE);
    /* Show number of LSA. */
    vty_out(vty, "   Number of LSA %ld%s", area->lsdb->total, VTY_NEWLINE);
    vty_out(vty, "   Number of router LSA %ld.%s",
            ospf_lsdb_count(area->lsdb, OSPF_ROUTER_LSA),
            VTY_NEWLINE);
    vty_out(vty, "   Number of network LSA %ld.%s",
            ospf_lsdb_count(area->lsdb, OSPF_NETWORK_LSA),
            VTY_NEWLINE);
    vty_out(vty, "   Number of summary LSA %ld.%s",
            ospf_lsdb_count(area->lsdb, OSPF_SUMMARY_LSA),
            VTY_NEWLINE);
    vty_out(vty, "   Number of ASBR summary LSA %ld.%s",
            ospf_lsdb_count(area->lsdb, OSPF_ASBR_SUMMARY_LSA),
            VTY_NEWLINE);
    vty_out(vty, "   Number of NSSA LSA %ld.%s",
            ospf_lsdb_count(area->lsdb, OSPF_AS_NSSA_LSA),
            VTY_NEWLINE);
    vty_out(vty, "   Number of opaque link LSA %ld.%s",
            ospf_lsdb_count(area->lsdb, OSPF_OPAQUE_LINK_LSA),
            VTY_NEWLINE);
    vty_out(vty, "   Number of opaque area LSA %ld.%s",
            ospf_lsdb_count(area->lsdb, OSPF_OPAQUE_AREA_LSA),
            VTY_NEWLINE);
    vty_out(vty, "%s", VTY_NEWLINE);
}


DEFUN(show_ip_ospf_refesh_list,
      show_ip_ospf_refesh_list_cmd,
      "show ip ospf [<1-255>] refresh-list",
      SHOW_STR
      IP_STR
      "OSPF information\n"
      "OSPF instance number\n"
      "refresh-list")
{
    struct ospf *ospf;
    int refresh_index;
    struct list *refresh_list;
    struct listnode *node, *nnode;
    struct route_node *rn;
    struct ospf_lsa *lsa;
    /* Check OSPF is enable. */
    int ospf_id;
    ospf_id = OSPF_DEFAULT_PROCESS_ID;

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID);
    }

    ospf = ospf_lookup_id(ospf_id);

    if (ospf == NULL)
    {
        VTY_OSPF_INSTANCE_ERR
    }

    vty_out(vty, "%-6s %-15s %-5s %-15s %-5s %-15s%s", "index", "lsa id", "type", "adv id", "age", "area", VTY_NEWLINE);

    for (refresh_index = 0; refresh_index < OSPF_LSA_REFRESHER_SLOTS; refresh_index ++)
    {
        refresh_list = ospf->lsa_refresh_queue.qs [refresh_index];

        if (refresh_list)
        {
            for (ALL_LIST_ELEMENTS(refresh_list, node, nnode, lsa))
            {
                if (lsa)
                {
                    vty_out(vty, "%-6d %-15s %-5d ", refresh_index, inet_ntoa(lsa->data->id),
                            lsa->data->type);

                    if (lsa->area == NULL)
                    {
                        vty_out(vty, "%-15s %-5d - %s", inet_ntoa(lsa->data->adv_router), LS_AGE(lsa), VTY_NEWLINE);
                    }
                    else
                    {
                        vty_out(vty, "%-15s %-5d ", inet_ntoa(lsa->data->adv_router), LS_AGE(lsa));
                        vty_out(vty, "%-15s %s", inet_ntoa(lsa->area->area_id), VTY_NEWLINE);
                    }
                }
            }
        }
    }

    vty_out(vty, "refresh %-5d %s", ospf->lsa_refresh_queue.index, VTY_NEWLINE);
    vty_out(vty, "MAXAGE %s", VTY_NEWLINE);

    for (rn = route_top(ospf->maxage_lsa); rn; rn = route_next(rn))
    {
        if ((lsa = rn->info) == NULL)
        {
            continue;
        }

        vty_out(vty, "%-6d %-15s %-5d ",CMD_SUCCESS, inet_ntoa(lsa->data->id), lsa->data->type);
		if (lsa->area == NULL)
		{
        	vty_out(vty, "%-15s %-5d - %s", inet_ntoa(lsa->data->adv_router), LS_AGE(lsa), VTY_NEWLINE);
		}
		else
		{
			vty_out(vty, "%-15s %-5d ", inet_ntoa(lsa->data->adv_router), LS_AGE(lsa));
			vty_out(vty, "%-15s %s", inet_ntoa(lsa->area->area_id), VTY_NEWLINE);
		}
    }

    return CMD_SUCCESS;
}

static void display_ospf_lsa_refresh_list_info(struct vty *vty, char **argv, struct ospf *ospf_p)
{
	int refresh_index = 0;
	struct ospf_lsa *lsa = NULL;
	struct route_node *rn = NULL;
	struct listnode *node = NULL;
	struct listnode *nnode = NULL;
	struct list *refresh_list = NULL;
	
	vty_out(vty, "%-6s %-15s %-5s %-15s %-5s %-15s%s",\
				"index", "lsa id", "type", "adv id", "age", "area", VTY_NEWLINE);

    for (refresh_index = 0; refresh_index < OSPF_LSA_REFRESHER_SLOTS; refresh_index ++)
    {
        refresh_list = ospf_p->lsa_refresh_queue.qs [refresh_index];

        if (refresh_list)
        {
            for (ALL_LIST_ELEMENTS(refresh_list, node, nnode, lsa))
            {
                if (lsa)
                {
                    vty_out(vty, "%-6d %-15s %-5d ", refresh_index, inet_ntoa(lsa->data->id),
                            lsa->data->type);

                    if (lsa->area == NULL)
                    {
                        vty_out(vty, "%-15s %-5d - %s", inet_ntoa(lsa->data->adv_router), LS_AGE(lsa), VTY_NEWLINE);
                    }
                    else
                    {
                        vty_out(vty, "%-15s %-5d ", inet_ntoa(lsa->data->adv_router), LS_AGE(lsa));
                        vty_out(vty, "%-15s %s", inet_ntoa(lsa->area->area_id), VTY_NEWLINE);
                    }
                }
            }
        }
    }

    vty_out(vty, "refresh %-5d %s", ospf_p->lsa_refresh_queue.index, VTY_NEWLINE);
    vty_out(vty, "MAXAGE %s", VTY_NEWLINE);

    for (rn = route_top(ospf_p->maxage_lsa); rn; rn = route_next(rn))
    {
        if ((lsa = rn->info) == NULL)
        {
            continue;
        }

        vty_out(vty, "%-6d %-15s %-5d ",CMD_SUCCESS, inet_ntoa(lsa->data->id), lsa->data->type);
		if (lsa->area == NULL)
		{
        	vty_out(vty, "%-15s %-5d - %s", inet_ntoa(lsa->data->adv_router), LS_AGE(lsa), VTY_NEWLINE);
		}
		else
		{
			vty_out(vty, "%-15s %-5d ", inet_ntoa(lsa->data->adv_router), LS_AGE(lsa));
			vty_out(vty, "%-15s %s", inet_ntoa(lsa->area->area_id), VTY_NEWLINE);
		}
    }
	return;
}


DEFUN(display_ospf_refesh_list,
	display_ospf_refesh_list_h3c_cmd,
	"display ospf [ <1-65535> ] refresh-list",
	"Display current system information\n"
	"OSPF information\n"
	"OSPF instance number\n"
	"Lsa refresh-list")
{
    int ospf_id;
	struct ospf *ospf = NULL;
	struct listnode *node = NULL;
	
    ospf_id = OSPF_DEFAULT_PROCESS_ID;

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID_H3C);
    
	    ospf = ospf_lookup_id(ospf_id);
	    if (ospf == NULL)
	    {
	        VTY_OSPF_INSTANCE_ERR
	    }

		display_ospf_lsa_refresh_list_info(vty, (char **)argv, ospf);
	}
	else
	{
		for (ALL_LIST_ELEMENTS_RO (om->ospf, node, ospf))
		{
			vty_out(vty, "%s%-10s OSPF Instance %d with Router ID %s%s",
            			VTY_NEWLINE, " ", ospf->ospf_id, inet_ntoa(ospf->router_id), VTY_NEWLINE);
			vty_out(vty, "%-15s LSA Refresh-list%s", " ", VTY_NEWLINE);
			
			display_ospf_lsa_refresh_list_info(vty, (char **)argv, ospf);
			vty_out(vty, " ************************************************************%s", VTY_NEWLINE);
		}
	}
	
    return CMD_SUCCESS;
}

	
DEFUN(show_ip_ospf,
      show_ip_ospf_cmd,
      "show ip ospf instance [<1-255>]",
      SHOW_STR
      IP_STR
      "OSPF information\n"
      "OSPF instance\n"
      "OSPF instance number\n")
{
    struct listnode *node = NULL, *nnode = NULL;
    struct ospf_area * area = NULL;
    struct ospf *ospf = NULL;
    struct timeval result;
    char timebuf[OSPF_TIME_DUMP_SIZE];
    int ospf_id;
    ospf_id = OSPF_DEFAULT_PROCESS_ID;

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID);
    }

    ospf = ospf_lookup_id(ospf_id);

    if (ospf == NULL)
    {
        VTY_OSPF_INSTANCE_ERR
    }

    /* Show Router ID. */
    vty_out(vty, " OSPF Instance: %d, Router ID: %s, vpn: %d%s", ospf->ospf_id,
            inet_ntoa(ospf->router_id), ospf->vpn,
            VTY_NEWLINE);


    vty_out(vty, " OSPF DCN is %s%s",
            ospf->dcn_enable ?
            "enabled" : "disabled", VTY_NEWLINE);

    vty_out(vty, " OSPF DCN trap report is %s%s",
            ospf->dcn_report ?
            "enabled" : "disabled", VTY_NEWLINE);

    /* Graceful shutdown */
    if (ospf->t_deferred_shutdown)
    {
    	memset(timebuf, 0, sizeof(timebuf));
        /*vty_out(vty, " Deferred shutdown in intance, %s remaining%s",
                ospf_timer_dump(ospf->t_deferred_shutdown,
                                timebuf, sizeof(timebuf)), VTY_NEWLINE);*/
		vty_out(vty, " Deferred shutdown in intance, %s remaining%s",
                ospf_timer_dump_new(ospf->t_deferred_shutdown, ospf->stub_router_shutdown_time,
                                timebuf, sizeof(timebuf)), VTY_NEWLINE);
    }

    /* Show capability. */
    vty_out(vty, " Supports only single TOS (TOS0) routes%s", VTY_NEWLINE);
    vty_out(vty, " This implementation conforms to RFC2328%s", VTY_NEWLINE);
    vty_out(vty, " RFC1583Compatibility flag is %s%s",
            CHECK_FLAG(ospf->config, OSPF_RFC1583_COMPATIBLE) ?
            "enabled" : "disabled", VTY_NEWLINE);
    vty_out(vty, " OpaqueCapability flag is %s%s",
            CHECK_FLAG(ospf->config, OSPF_OPAQUE_CAPABLE) ?
            "enabled" : "disabled",
            VTY_NEWLINE);

    /* Show stub-router configuration */
    if (ospf->stub_router_startup_time != OSPF_STUB_ROUTER_UNCONFIGURED
            || ospf->stub_router_shutdown_time != OSPF_STUB_ROUTER_UNCONFIGURED)
    {
        vty_out(vty, " Stub router advertisement is configured%s",
                VTY_NEWLINE);

        if (ospf->stub_router_startup_time != OSPF_STUB_ROUTER_UNCONFIGURED)
            vty_out(vty, "   Enabled for %us after start-up%s",
                    ospf->stub_router_startup_time, VTY_NEWLINE);

        if (ospf->stub_router_shutdown_time != OSPF_STUB_ROUTER_UNCONFIGURED)
            vty_out(vty, "   Enabled for %us prior to full shutdown%s",
                    ospf->stub_router_shutdown_time, VTY_NEWLINE);
    }

    /* Show SPF timers. */
    vty_out(vty, " Initial SPF scheduling delay %d millisec(s)%s"
            " Minimum hold time between consecutive SPFs %d millisec(s)%s"
            " Maximum hold time between consecutive SPFs %d millisec(s)%s"
            " Hold time multiplier is currently %d%s",
            ospf->spf_delay, VTY_NEWLINE,
            ospf->spf_holdtime, VTY_NEWLINE,
            ospf->spf_max_holdtime, VTY_NEWLINE,
            ospf->spf_hold_multiplier, VTY_NEWLINE);
    vty_out(vty, " SPF algorithm ");

    if (ospf->ts_spf.tv_sec || ospf->ts_spf.tv_usec)
    {
        result = tv_sub(time_get_recent_relative_time(), ospf->ts_spf);
        vty_out(vty, "last executed %s ago%s",
                ospf_timeval_dump(&result, timebuf, sizeof(timebuf)),
                VTY_NEWLINE);
        vty_out(vty, " Last SPF duration %s%s",
                ospf_timeval_dump(&ospf->ts_spf_duration, timebuf, sizeof(timebuf)),
                VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "has not been run%s", VTY_NEWLINE);
        /*vty_out(vty, " SPF timer %s%s%s",
                (ospf->t_spf_calc ? "due in " : "is "),
                ospf_timer_dump(ospf->t_spf_calc, timebuf, sizeof(timebuf)),
                VTY_NEWLINE);*/
		vty_out(vty, " SPF timer %s%s%s",
                (ospf->t_spf_calc ? "due in " : "is "),
                ospf_timer_dump_new(ospf->t_spf_calc, 0, timebuf, sizeof(timebuf)),
                VTY_NEWLINE);
    }

    /* Show refresh parameters. */
    vty_out(vty, " Refresh timer %d secs%s",
            ospf->lsa_refresh_interval, VTY_NEWLINE);

    /* Show ABR/ASBR flags. */
    if (CHECK_FLAG(ospf->flags, OSPF_FLAG_ABR))
        vty_out(vty, " This router is an ABR, ABR type is: %s%s",
                ospf_abr_type_descr_str[ospf->abr_type], VTY_NEWLINE);

    if (CHECK_FLAG(ospf->flags, OSPF_FLAG_ASBR))
        vty_out(vty, " This router is an ASBR "
                "(injecting external routing information)%s", VTY_NEWLINE);

    /* Show Number of AS-external-LSAs. */
    if ((listcount(ospf->areas) - ospf->anyNSSA - ospf->anySTUB) == 0)
    {
        vty_out(vty, " Number of external LSA %ld.%s",
                ospf_lsdb_count(ospf->lsdb, OSPF_AS_EXTERNAL_LSA) - ospf_lsdb_count_self(ospf->lsdb, OSPF_AS_EXTERNAL_LSA),
                VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, " Number of external LSA %ld.%s",
                ospf_lsdb_count(ospf->lsdb, OSPF_AS_EXTERNAL_LSA),
                VTY_NEWLINE);
    }

    vty_out(vty, " Number of opaque AS LSA %ld.%s",
            ospf_lsdb_count(ospf->lsdb, OSPF_OPAQUE_AS_LSA), VTY_NEWLINE);
    /* Show number of areas attached. */
    vty_out(vty, " Number of areas attached to this router: %d%s",
            listcount(ospf->areas), VTY_NEWLINE);

    if (CHECK_FLAG(ospf->config, OSPF_LOG_ADJACENCY_CHANGES))
    {
        if (CHECK_FLAG(ospf->config, OSPF_LOG_ADJACENCY_DETAIL))
        {
            vty_out(vty, " All adjacency changes are logged%s", VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, " Adjacency changes are logged%s", VTY_NEWLINE);
        }
    }

    vty_out(vty, "%s", VTY_NEWLINE);

    /* Show each area status. */
    for (ALL_LIST_ELEMENTS(ospf->areas, node, nnode, area))
    {
        show_ip_ospf_area(vty, area);
    }

    return CMD_SUCCESS;
}

static void display_ospf_instance_info(struct vty *vty, char **argv, struct ospf *ospf_p)
{
	struct timeval result;
	struct listnode *node = NULL;
	struct listnode *nnode = NULL;
	struct ospf_area *area = NULL;
	char timebuf[OSPF_TIME_DUMP_SIZE];
	
	/* Show Router ID. */
	vty_out(vty, "OSPF Instance: %d, Router ID: %s, vpn: %d%s", ospf_p->ospf_id,
				inet_ntoa(ospf_p->router_id), ospf_p->vpn, VTY_NEWLINE);

	vty_out(vty, " OSPF DCN is %s%s",
				ospf_p->dcn_enable ?
				"enabled" : "disabled", VTY_NEWLINE);

	vty_out(vty, " OSPF DCN trap report is %s%s",
				ospf_p->dcn_report ?
				"enabled" : "disabled", VTY_NEWLINE);

	/* Graceful shutdown */
	if (ospf_p->t_deferred_shutdown)
	{
		memset(timebuf, 0, sizeof(timebuf));
		/*vty_out(vty, " Deferred shutdown in intance, %s remaining%s",
					ospf_timer_dump(ospf_p->t_deferred_shutdown,
							  timebuf, sizeof(timebuf)), VTY_NEWLINE);*/
		vty_out(vty, " Deferred shutdown in intance, %s remaining%s",
                ospf_timer_dump_new(ospf_p->t_deferred_shutdown, ospf_p->stub_router_shutdown_time,
                                timebuf, sizeof(timebuf)), VTY_NEWLINE);
	}

	/* Show capability. */
	vty_out(vty, " Supports only single TOS (TOS0) routes%s", VTY_NEWLINE);
	vty_out(vty, " This implementation conforms to RFC2328%s", VTY_NEWLINE);
	vty_out(vty, " RFC1583Compatibility flag is %s%s",
				CHECK_FLAG(ospf_p->config, OSPF_RFC1583_COMPATIBLE) ?
				"enabled" : "disabled", VTY_NEWLINE);
	vty_out(vty, " OpaqueCapability flag is %s%s",
				CHECK_FLAG(ospf_p->config, OSPF_OPAQUE_CAPABLE) ?
				"enabled" : "disabled",
				VTY_NEWLINE);

	/* Show stub-router configuration */
	if (ospf_p->stub_router_startup_time != OSPF_STUB_ROUTER_UNCONFIGURED
		|| ospf_p->stub_router_shutdown_time != OSPF_STUB_ROUTER_UNCONFIGURED)
	{
		vty_out(vty, " Stub router advertisement is configured%s", VTY_NEWLINE);

		if (ospf_p->stub_router_startup_time != OSPF_STUB_ROUTER_UNCONFIGURED)
		{
			vty_out(vty, "   Enabled for %us after start-up%s",
							ospf_p->stub_router_startup_time, VTY_NEWLINE);
		}

		if (ospf_p->stub_router_shutdown_time != OSPF_STUB_ROUTER_UNCONFIGURED)
		{
			vty_out(vty, "   Enabled for %us prior to full shutdown%s",
							ospf_p->stub_router_shutdown_time, VTY_NEWLINE);
		}
	}

	/* Show SPF timers. */
	vty_out(vty, " Initial SPF scheduling delay %d millisec(s)%s"
				" Minimum hold time between consecutive SPFs %d millisec(s)%s"
				" Maximum hold time between consecutive SPFs %d millisec(s)%s"
				" Hold time multiplier is currently %d%s",
				ospf_p->spf_delay, VTY_NEWLINE,
				ospf_p->spf_holdtime, VTY_NEWLINE,
				ospf_p->spf_max_holdtime, VTY_NEWLINE,
				ospf_p->spf_hold_multiplier, VTY_NEWLINE);
	vty_out(vty, " SPF algorithm ");

	if (ospf_p->ts_spf.tv_sec || ospf_p->ts_spf.tv_usec)
	{
		result = tv_sub(time_get_recent_relative_time(), ospf_p->ts_spf);
		vty_out(vty, "last executed %s ago%s",
					ospf_timeval_dump(&result, timebuf, sizeof(timebuf)),
					VTY_NEWLINE);
		vty_out(vty, " Last SPF duration %s%s",
					ospf_timeval_dump(&ospf_p->ts_spf_duration, timebuf, sizeof(timebuf)),
					VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "has not been run%s", VTY_NEWLINE);
		/*vty_out(vty, " SPF timer %s%s%s",
					(ospf_p->t_spf_calc ? "due in " : "is "),
					ospf_timer_dump(ospf_p->t_spf_calc, timebuf, sizeof(timebuf)),
					VTY_NEWLINE);*/
		vty_out(vty, " SPF timer %s%s%s",
                (ospf_p->t_spf_calc ? "due in " : "is "),
                ospf_timer_dump_new(ospf_p->t_spf_calc, 0, timebuf, sizeof(timebuf)),
                VTY_NEWLINE);
	}

	/* Show refresh parameters. */
	vty_out(vty, " Refresh timer %d secs%s",
					ospf_p->lsa_refresh_interval, VTY_NEWLINE);

	/* Show ABR/ASBR flags. */
	if (CHECK_FLAG(ospf_p->flags, OSPF_FLAG_ABR))
	{
		vty_out(vty, " This router is an ABR, ABR type is: %s%s",
					ospf_abr_type_descr_str[ospf_p->abr_type], VTY_NEWLINE);
	}

	if (CHECK_FLAG(ospf_p->flags, OSPF_FLAG_ASBR))
	{
		vty_out(vty, " This router is an ASBR "
					"(injecting external routing information)%s", VTY_NEWLINE);
	}

	/* Show Number of AS-external-LSAs. */
	if ((listcount(ospf_p->areas) - ospf_p->anyNSSA - ospf_p->anySTUB) == 0)
	{
		vty_out(vty, " Number of external LSA %ld.%s",
		ospf_lsdb_count(ospf_p->lsdb, OSPF_AS_EXTERNAL_LSA) - ospf_lsdb_count_self(ospf_p->lsdb, OSPF_AS_EXTERNAL_LSA),
		VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, " Number of external LSA %ld.%s",
					ospf_lsdb_count(ospf_p->lsdb, OSPF_AS_EXTERNAL_LSA),VTY_NEWLINE);
	}

	vty_out(vty, " Number of opaque AS LSA %ld.%s",
						ospf_lsdb_count(ospf_p->lsdb, OSPF_OPAQUE_AS_LSA), VTY_NEWLINE);
	/* Show number of areas attached. */
	vty_out(vty, " Number of areas attached to this router: %d%s",
						listcount(ospf_p->areas), VTY_NEWLINE);

	if (CHECK_FLAG(ospf_p->config, OSPF_LOG_ADJACENCY_CHANGES))
	{
		if (CHECK_FLAG(ospf_p->config, OSPF_LOG_ADJACENCY_DETAIL))
		{
			vty_out(vty, " All adjacency changes are logged%s", VTY_NEWLINE);
		}
		else
		{
			vty_out(vty, " Adjacency changes are logged%s", VTY_NEWLINE);
		}
	}

	vty_out(vty, "%s", VTY_NEWLINE);

	/* Show each area status. */
	for (ALL_LIST_ELEMENTS(ospf_p->areas, node, nnode, area))
	{
		show_ip_ospf_area(vty, area);
	}
	
	return;
}

DEFUN(show_ip_ospf_h3c,
	show_ip_ospf_h3c_cmd,
	"display ospf [<1-65535>] verbose",
	"Display current system information\n"
	"OSPF information\n"
	"Specify OSPF instance number\n"
	"Detail information about specify OSPF instance\n")
{
	int ospf_id;
	struct ospf *ospf = NULL;
	struct listnode *node = NULL;
	
	ospf_id = OSPF_DEFAULT_PROCESS_ID;

	if (argv[0] != NULL)
	{
		VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID_H3C);
	

	  	ospf = ospf_lookup_id(ospf_id);
		if (ospf == NULL)
		{
			VTY_OSPF_INSTANCE_ERR
		}
		
		display_ospf_instance_info(vty, (char **)argv, ospf);
	}
	else
	{
		vty_out(vty, "%s", VTY_NEWLINE);
		for (ALL_LIST_ELEMENTS_RO (om->ospf, node, ospf))
		{
			display_ospf_instance_info(vty, (char **)argv, ospf);
			vty_out(vty, " ************************************************************************%s",
							VTY_NEWLINE);
		}
	}

	return CMD_SUCCESS;
}


ALIAS(show_ip_ospf_h3c,
	show_ip_ospf_h3c_format_cmd,
	"display ospf [<1-65535>] ",
	"Display current system information\n"
	"OSPF information\n"
	"Specify OSPF instance number\n")


static void
show_ip_ospf_interface_sub(struct vty *vty, struct ospf *ospf,
                           struct ospf_interface *oi)
{
    struct interface *ifp = oi->ifp;
    int is_up;
    struct ospf_neighbor *nbr;

    if (ifp->ifindex)
    {
        if (CHECK_FLAG(oi->connected->flags, ZEBRA_IFA_SLAVE))
        {
            return;
        }

        vty_out(vty, "%s is %s%s", ifp->name,
                ((is_up = if_is_operative(ifp)) ? "up" : "down"), VTY_NEWLINE);
        vty_out(vty, "  ifindex: %0x    MTU: %u bytes    BW: %u Kbit %s%s",
                ifp->ifindex, ifp->mtu, ifp->bandwidth, if_flag_dump(ifp->flags),
                VTY_NEWLINE);
        /* Show OSPF interface information. */
        vty_out(vty, "  Internet Address: %s/%d    ",
                inet_ntoa(oi->address->u.prefix4), oi->address->prefixlen);

        if (oi->connected->destination || oi->type == OSPF_IFTYPE_VIRTUALLINK)
        {
            struct in_addr *dest;
            const char *dstr;

            if (CONNECTED_PEER(oi->connected)
                    || oi->type == OSPF_IFTYPE_VIRTUALLINK)
            {
                dstr = "Peer";
            }
            else
            {
                dstr = "Broadcast";
            }

            /* For Vlinks, showing the peer address is probably more
                * informative than the local interface that is being used
                */
            if (oi->type == OSPF_IFTYPE_VIRTUALLINK)
            {
                dest = &oi->vl_data->peer_addr;
            }
            else
            {
                dest = &oi->connected->destination->u.prefix4;
            }

            vty_out(vty, " %s %s,", dstr, inet_ntoa(*dest));
        }

        vty_out(vty, " Area: %s%s", ospf_area_desc_string(oi->area),
                VTY_NEWLINE);
        vty_out(vty, "  MTU mismatch detection :%s%s",
                OSPF_IF_PARAM(oi, mtu_ignore) ? "disabled" : "enabled", VTY_NEWLINE);
        vty_out(vty, "  Router ID: %s   Type: %s   Cost: %d   State: %s   Priority: %d%s",
                inet_ntoa(ospf->router_id), ospf_network_type_str[oi->type],
                oi->output_cost, LOOKUP(ospf_ism_state_msg, oi->state),
                PRIORITY(oi), VTY_NEWLINE);

        /* Show DR information. */
        if (DR(oi).s_addr == 0)
        {
            vty_out(vty, "  No designated router on this network%s", VTY_NEWLINE);
        }
        else
        {
            nbr = ospf_nbr_lookup_by_addr(oi->nbrs, &DR(oi));

            if (nbr == NULL)
            {
                vty_out(vty, "  No designated router on this network%s", VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, "  Designated Router (ID): %s   ",
                        inet_ntoa(nbr->router_id));
                vty_out(vty, " Interface Address: %s%s",
                        inet_ntoa(nbr->address.u.prefix4), VTY_NEWLINE);
            }
        }

        /* Show BDR information. */
        if (BDR(oi).s_addr == 0)
            vty_out(vty, "  No backup designated router on this network%s",
                    VTY_NEWLINE);
        else
        {
            nbr = ospf_nbr_lookup_by_addr(oi->nbrs, &BDR(oi));

            if (nbr == NULL)
                vty_out(vty, "  No backup designated router on this network%s",
                        VTY_NEWLINE);
            else
            {
                vty_out(vty, "  Backup Designated Router (ID): %s  ",
                        inet_ntoa(nbr->router_id));
                vty_out(vty, " Interface Address: %s%s",
                        inet_ntoa(nbr->address.u.prefix4), VTY_NEWLINE);
            }
        }

        vty_out(vty, "%s", VTY_NEWLINE);
        vty_out(vty, "  Timer intervals configured,%s", VTY_NEWLINE);
        vty_out(vty, "  Hello: ");

        if (OSPF_IF_PARAM(oi, fast_hello) == 0)
        {
            vty_out(vty, "%ds   ", OSPF_IF_PARAM(oi, v_hello));
        }
        else
        {
            vty_out(vty, "%dms   ", 1000 / OSPF_IF_PARAM(oi, fast_hello));
        }

        vty_out(vty, " Dead: %ds   Retransmit: %d   Transmit : %d%s",
                OSPF_IF_PARAM(oi, v_wait),
                OSPF_IF_PARAM(oi, retransmit_interval), OSPF_IF_PARAM(oi, transmit_delay),
                VTY_NEWLINE);

        if (OSPF_IF_PASSIVE_STATUS(oi) == OSPF_IF_ACTIVE)
        {
            char timebuf[OSPF_TIME_DUMP_SIZE];
            /*vty_out(vty, "  Hello due in: %s%s",
                    ospf_timer_dump(oi->t_hello, timebuf, sizeof(timebuf)),
                    VTY_NEWLINE);*/
			vty_out(vty, "  Hello due in: %s%s",
                    ospf_timer_dump_new(oi->t_hello, OSPF_IF_PARAM(oi, v_hello), timebuf, sizeof(timebuf)),
                    VTY_NEWLINE);
        }
        else /* passive-interface is set */
        {
            vty_out(vty, "    No Hellos (Passive interface)%s", VTY_NEWLINE);
        }

        vty_out(vty, "  Neighbor Count: %d   Adjacent neighbor count: %d%s",
                ospf_nbr_count(oi, 0), ospf_nbr_count(oi, NSM_Full),
                VTY_NEWLINE);
    }
}


DEFUN(show_ip_ospf_interface,
      show_ip_ospf_interface_cmd,
      "show ip ospf [<1-255>] interface",
      SHOW_STR
      IP_STR
      "OSPF information\n"
      "OSPF instance number\n"
      "Interface information\n")
{
    struct interface *ifp = NULL;
    struct ospf *ospf = NULL;
    struct listnode *node = NULL;
    uint32_t ifindex = 0;
    char if_name[IFNET_NAMESIZE];
    int ospf_id;
    ospf_id = OSPF_DEFAULT_PROCESS_ID;
    struct ospf_interface * oi = NULL;
    int ret = -1;
    struct route_node *rn = NULL;

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID);
    }

    ospf = ospf_lookup_id(ospf_id);

    if (ospf == NULL)
    {
        VTY_OSPF_INSTANCE_ERR
    }

    /* Interface name is specified. */
    if(argc == 3)
    {
        ifindex = ifm_get_ifindex_by_name((char *)argv[1], (char *) argv[2]);
        if (ifindex == 0)
        {
            vty_error_out(vty, "Command incomplete ,please check out%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        /*get the name of the interface*/
        ret = ifm_get_name_by_ifindex(ifindex , if_name);

        if (ret < 0)
        {
            vty_error_out(vty, "Failed to get interface information.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        if ((ifp = if_lookup_by_name(if_name)) == NULL)
        {
            vty_error_out(vty, "No such interface name%s", VTY_NEWLINE);
        }
        else
        {
	        for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
	        {
	            oi = rn->info;

	            if (!oi)
	            {
	                continue;
	            }
				if(oi->ospf == ospf)
				{
					show_ip_ospf_interface_sub(vty, ospf, oi);
				}
				else
				{
					vty_info_out(vty, "The specified interface is not enabled in the current OSPF instance%s", VTY_NEWLINE);
					return CMD_WARNING;
				}
	        }
            /*for (ALL_LIST_ELEMENTS_RO(ospf->oiflist, node, oi))
            {
                if (oi->ifp == ifp)
                {
                    show_ip_ospf_interface_sub(vty, ospf, oi);
                }
            }*/
        }
    }    
    /* Show All Interfaces. */
    else
    {
        for (ALL_LIST_ELEMENTS_RO(ospf->oiflist, node, oi))
        {
            vty_out(vty, "%s", VTY_NEWLINE);
            show_ip_ospf_interface_sub(vty, ospf, oi);
        }
    }

    return CMD_SUCCESS;
}

ALIAS (show_ip_ospf_interface,
        show_ip_ospf_interface_ifname_cmd,
        "show ip ospf [<1-255>] interface (ethernet |gigabitethernet |xgigabitethernet ) USP",
        SHOW_STR
        IP_STR
        "OSPF information\n"
        "OSPF instance number\n"
        "Interface information\n"
        "Ethernet interface type\n"
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        "The port/subport of the interface, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n")



static int display_ospf_interface_info(struct vty *vty, int argc, char **argv, struct ospf *ospf_p)
{
	int ret = -1;
	uint32_t ifindex = 0;
    struct route_node *rn = NULL;
	char if_name[IFNET_NAMESIZE];
	struct listnode *node = NULL;
	struct interface *ifp = NULL;
    struct ospf_interface * oi = NULL;
	
	/* Interface name is specified. */
	if(argc == 3)
	{
		ifindex = ifm_get_ifindex_by_name((char *)argv[1], (char *) argv[2]);
		if (ifindex == 0)
		{
			vty_error_out(vty, "Command incomplete ,please check out%s", VTY_NEWLINE);
			return CMD_WARNING;
		}

		/*get the name of the interface*/
		ret = ifm_get_name_by_ifindex(ifindex , if_name);
		if (ret < 0)
		{
			vty_error_out(vty, "Failed to get interface information.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}

		if ((ifp = if_lookup_by_name(if_name)) == NULL)
		{
			vty_error_out(vty, "No such interface name%s", VTY_NEWLINE);
		}
		else
		{
			for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
			{
				oi = rn->info;

				if (!oi)
				{
					continue;
				}
				if(oi->ospf == ospf_p)
				{
					show_ip_ospf_interface_sub(vty, ospf_p, oi);
				}
				else if(argv[0] != NULL)
				{
					vty_info_out(vty, "The specified interface is not enabled in the current OSPF instance%s", VTY_NEWLINE);
					return CMD_WARNING;
				}
			}
		}
	}	 
	/* Show All Interfaces. */
	else
	{
		for (ALL_LIST_ELEMENTS_RO(ospf_p->oiflist, node, oi))
		{
			vty_out(vty, "%s", VTY_NEWLINE);
			show_ip_ospf_interface_sub(vty, ospf_p, oi);
		}
	}
	
	return CMD_SUCCESS;
}

DEFUN (show_ip_ospf_interface_h3c,
		display_ospf_interface_h3c_cmd,
		"display ospf [ <1-65535> ] interface { verbose }",
		"Display current system information\n"
		"OSPF information\n"
		"Specify OSPF instance number\n"
		"Interface information\n"
		"Detail information about Interfaces\n")
{
    int ospf_id;
	int ret = CMD_WARNING;
	struct ospf *ospf = NULL;
	struct listnode *node = NULL;

	ospf_id = OSPF_DEFAULT_PROCESS_ID;
	
    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID_H3C);
    
	    ospf = ospf_lookup_id(ospf_id);
	    if (ospf == NULL)
	    {
	        VTY_OSPF_INSTANCE_ERR
	    }

		ret = display_ospf_interface_info(vty, argc, (char **)argv, ospf);
	}
	else
	{
		for (ALL_LIST_ELEMENTS_RO (om->ospf, node, ospf))
		{
			vty_out(vty, "%s%-10s OSPF Instance %d with Router ID %s%s",
            				VTY_NEWLINE, " ", ospf->ospf_id, inet_ntoa(ospf->router_id), VTY_NEWLINE);
			vty_out(vty, "%-15s Interfaces%s", " ", VTY_NEWLINE);
			ret = display_ospf_interface_info(vty, argc, (char **)argv, ospf);
		}
	}

    return ret;
}


ALIAS (show_ip_ospf_interface_h3c,
		display_ospf_interface_ifname_h3c_cmd,
		"display ospf [ <1-65535> ] interface (ethernet |gigabitethernet |xgigabitethernet ) USP",
		"Display current system information\n"
		"OSPF information\n"
		"Specify OSPF instance number\n"
		"Interface information\n"
		"Ethernet interface type\n"
		"Gigabit Ethernet interface\n"
		"10Gigabit Ethernet interface\n"
		"The port/subport of the interface, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n")


ALIAS(show_ip_ospf_interface_h3c,
		display_ospf_if_trunk_h3c_cmd, 
		"display ospf [ <1-65535> ] interface (trunk) TRUNK",
		"Display current system information\n"
		"OSPF information\n"
		"Specify OSPF instance number\n"
		"Interface information\n"
		"Trunk interface\n"
		"The port/subport of trunk, format: <1-128>[.<1-4095>]\n")


ALIAS(show_ip_ospf_interface_h3c,
		display_ospf_if_vlanif_h3c_cmd, 
		"display ospf [ <1-65535> ] interface (vlanif) <1-4094>",
		"Display current system information\n"
		"OSPF information\n"
		"Specify OSPF instance number\n"
		"Interface information\n"
		"Vlan interface\n"
		"VLAN interface number\n")


ALIAS(show_ip_ospf_interface_h3c,
		display_ospf_if_looplack_h3c_cmd, 
		"display ospf [ <1-65535> ] interface (loopback) <0-128>",
		"Display current system information\n"
		"OSPF information\n"
		"Specify OSPF instance number\n"
		"Interface information\n"
		"LoopBack interface\n"
		"LoopBack interface number\n")


DEFUN(show_ip_ospf_interface_trunk,
	  show_ip_ospf_interface_trunk_cmd, 
	  "show ip ospf [<1-255>] interface trunk TRUNK",
 	  SHOW_STR
 	  IP_STR
 	  "OSPF information\n"
 	  "OSPF instance number\n"
 	  INTERFACE_STR  
	  CLI_INTERFACE_TRUNK_STR
      CLI_INTERFACE_TRUNK_VHELP_STR)
{
	struct interface *ifp = NULL;
	struct ospf *ospf = NULL;
	//struct listnode *node = NULL;
	uint32_t ifindex = 0;
	char if_name[IFNET_NAMESIZE];
	int ospf_id;
	ospf_id = OSPF_DEFAULT_PROCESS_ID;
	//struct ospf_interface * oi = NULL;
	int ret = -1;
    struct route_node *rn = NULL;

	if (argv[0] != NULL)
	{
		VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID);
	}

	ospf = ospf_lookup_id(ospf_id);

	if (ospf == NULL)
	{
		VTY_OSPF_INSTANCE_ERR
	}

	/* Interface name is specified. */
	ifindex = ifm_get_ifindex_by_name((char *)"trunk", (char *) argv[1]);
	if (ifindex == 0)
	{
		vty_error_out(vty, "Command incomplete ,please check out%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*get the name of the interface*/
	ret = ifm_get_name_by_ifindex(ifindex , if_name);

	if (ret < 0)
	{
		vty_error_out(vty, "Failed to get interface information.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if ((ifp = if_lookup_by_name(if_name)) == NULL)
	{
		vty_error_out(vty, "No such interface name%s", VTY_NEWLINE);
	}
	else
	{
		for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
        {
            struct ospf_interface *oi = rn->info;

            if (!oi)
            {
                continue;
            }
			if(oi->ospf == ospf)
			{
				show_ip_ospf_interface_sub(vty, ospf, oi);
			}
			else
			{
				vty_info_out(vty, "The specified interface is not enabled in the current OSPF instance%s", VTY_NEWLINE);
				return CMD_WARNING;
			}
        }
		
		/*for (ALL_LIST_ELEMENTS_RO(ospf->oiflist, node, oi))
		{
			if (oi->ifp == ifp)
			{
			 	show_ip_ospf_interface_sub(vty, ospf, oi);
			}
		}*/
	}
	
    return CMD_SUCCESS;

}

DEFUN(show_ip_ospf_interface_vlanif,
	  show_ip_ospf_interface_vlanif_cmd, 
	  "show ip ospf [<1-255>] interface vlanif <1-4094>",
   	  SHOW_STR
   	  IP_STR
   	  "OSPF information\n"
   	  "OSPF instance number\n"
   	  INTERFACE_STR       
	  CLI_INTERFACE_VLANIF_STR
      CLI_INTERFACE_VLANIF_VHELP_STR)
{
	struct interface *ifp = NULL;
	struct ospf *ospf = NULL;
	//struct listnode *node = NULL;
	uint32_t ifindex = 0;
	char if_name[IFNET_NAMESIZE];
	int ospf_id;
	ospf_id = OSPF_DEFAULT_PROCESS_ID;
	//struct ospf_interface * oi = NULL;
	int ret = -1;
    struct route_node *rn = NULL;

	if (argv[0] != NULL)
	{
		VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID);
	}

	ospf = ospf_lookup_id(ospf_id);

	if (ospf == NULL)
	{
		VTY_OSPF_INSTANCE_ERR
	}

	/* Interface name is specified. */
	ifindex = ifm_get_ifindex_by_name((char *)"vlanif", (char *) argv[1]);
	if (ifindex == 0)
	{
		vty_error_out(vty, "Command incomplete ,please check out%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*get the name of the interface*/
	ret = ifm_get_name_by_ifindex(ifindex , if_name);

	if (ret < 0)
	{
		vty_error_out(vty, "Failed to get interface information.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if ((ifp = if_lookup_by_name(if_name)) == NULL)
	{
		vty_error_out(vty, "No such interface name%s", VTY_NEWLINE);
	}
	else
	{
		for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
        {
            struct ospf_interface *oi = rn->info;

            if (!oi)
            {
                continue;
            }
			if(oi->ospf == ospf)
			{
				show_ip_ospf_interface_sub(vty, ospf, oi);
			}
			else
			{
				vty_info_out(vty, "The specified interface is not enabled in the current OSPF instance%s", VTY_NEWLINE);
				return CMD_WARNING;
			}
        }
		/*for (ALL_LIST_ELEMENTS_RO(ospf->oiflist, node, oi))
		{
			if (oi->ifp == ifp)
			{
				show_ip_ospf_interface_sub(vty, ospf, oi);
			}
		}*/
	}
	
	return CMD_SUCCESS;

}


DEFUN(show_ip_ospf_interface_loopback,
	  show_ip_ospf_interface_loopback_cmd, 
	  "show ip ospf [<1-255>] interface loopback <0-128>",
  	  SHOW_STR
  	  IP_STR
  	  "OSPF information\n"
  	  "OSPF instance number\n"
  	  INTERFACE_STR  
	  CLI_INTERFACE_LOOPBACK_STR
      CLI_INTERFACE_LOOPBACK_VHELP_STR)
{
	struct interface *ifp = NULL;
	struct ospf *ospf = NULL;
	//struct listnode *node = NULL;
	uint32_t ifindex = 0;
	char if_name[IFNET_NAMESIZE];
	int ospf_id;
	ospf_id = OSPF_DEFAULT_PROCESS_ID;
	//struct ospf_interface * oi = NULL;
	int ret = -1;
    struct route_node *rn = NULL;

	if (argv[0] != NULL)
	{
		VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID);
	}

	ospf = ospf_lookup_id(ospf_id);

	if (ospf == NULL)
	{
		VTY_OSPF_INSTANCE_ERR
	}

	/* Interface name is specified. */
	ifindex = ifm_get_ifindex_by_name((char *)"loopback", (char *) argv[1]);
	if (ifindex == 0)
	{
		vty_error_out(vty, "Command incomplete ,please check out%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*get the name of the interface*/
	ret = ifm_get_name_by_ifindex(ifindex , if_name);

	if (ret < 0)
	{
		vty_error_out(vty, "Failed to get interface information.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if ((ifp = if_lookup_by_name(if_name)) == NULL)
	{
		vty_error_out(vty, "No such interface name%s", VTY_NEWLINE);
	}
	else
	{
		for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
        {
            struct ospf_interface *oi = rn->info;

            if (!oi)
            {
                continue;
            }
			if(oi->ospf == ospf)
			{
				show_ip_ospf_interface_sub(vty, ospf, oi);
			}
			else
			{
				vty_info_out(vty, "The specified interface is not enabled in the current OSPF instance%s", VTY_NEWLINE);
				return CMD_WARNING;
			}
        }
		/*for (ALL_LIST_ELEMENTS_RO(ospf->oiflist, node, oi))
		{
			if (oi->ifp == ifp)
			{
				show_ip_ospf_interface_sub(vty, ospf, oi);
			}
		}*/
	}
	
	return CMD_SUCCESS;

}


static void
show_ip_ospf_neighbour_header(struct vty *vty)
{
    vty_out(vty, "%s%-15s %-15s %-5s %-10s %-20s%s",
            VTY_NEWLINE, "Area ID", "Router ID", "Pri", "State",
            "Interface",
            VTY_NEWLINE);
}

static void 
ospf_interface_name_simplify(char *name ,char if_name[])
{
    char *p = NULL;
    if(!strncmp(name, "e", 1))
    {
        sprintf(if_name, "%s", "Eth");
    }
    else if(!strncmp(name, "g", 1))
    {
        sprintf(if_name, "%s", "Ge");
    }
    else if(!strncmp(name, "x", 1))
    {
        sprintf(if_name, "%s", "XGE");
    }
    else if(!strncmp(name, "l", 1))
    {        
        sprintf(if_name, "%s", "Lo");
    }
    else if(!strncmp(name, "t", 1))
    {
        sprintf(if_name, "%s", "Tru");
    }
	else if(!strncmp(name, "v", 1))
    {
        sprintf(if_name, "%s", "vlanif");
    }
    else
    {
        sprintf(if_name, "%s", "Unknown");
    }
    p = strchr(name,' ');
    if(p != 0)
       strcat(if_name, p+1);
}

static void
show_ip_ospf_neighbor_sub(struct vty *vty, struct ospf_interface *oi)
{
    struct route_node *rn = NULL;
    struct ospf_neighbor *nbr = NULL;
    char msgbuf[16];

	//add oi->type by zzl, Reference Huawei.
    if (CHECK_FLAG(oi->connected->flags, ZEBRA_IFA_SLAVE) || (oi->type == OSPF_IFTYPE_VIRTUALLINK))
    {
        return;
    }

    for (rn = route_top(oi->nbrs); rn; rn = route_next(rn))
    {
        if ((nbr = rn->info))
        {
            /* Do not show myself. */
            if (nbr != oi->nbr_self)
            {
                /* Down state is not shown. */
                if (nbr->state != NSM_Down)
                {
                    vty_out(vty, "%-15s ", ospf_area_desc_string(oi->area));
                    ospf_nbr_state_message(nbr, msgbuf, 16);

                    if (nbr->state == NSM_Attempt && nbr->router_id.s_addr == 0)
                    {
                        vty_out(vty, "%-15s %-5d %-10s ", "-", nbr->priority, msgbuf);
                    }
                    else
                        vty_out(vty, "%-15s %-5d %-10s ", inet_ntoa(nbr->router_id)
                                , nbr->priority, msgbuf);
                    char if_name[IFNET_NAMESIZE] = "";
                    ospf_interface_name_simplify(oi->ifp->name, if_name);

                    vty_out(vty, "%-20s %s", if_name, VTY_NEWLINE);
                }
            }
        }
    }
}


static void
show_ip_ospf_neighbor_detail_sub(struct vty *vty, struct ospf_interface *oi,
							   struct ospf_neighbor *nbr);

static void
show_ip_ospf_nbr_nbma_detail_sub(struct vty *vty, struct ospf_interface *oi,
								struct ospf_nbr_nbma *nbr_nbma);

static void display_ospf_neighbour_info(struct vty *vty, char **argv, struct ospf *ospf_p)
{
	struct listnode *nd = NULL;
	struct listnode *node = NULL;
	struct route_node *rn = NULL;
	struct ospf_neighbor *nbr = NULL;
	struct ospf_interface *oi = NULL;
	struct ospf_nbr_nbma *nbr_nbma = NULL;
	
	if(argv[1] == NULL)
	{
		show_ip_ospf_neighbour_header(vty);
	
		for (ALL_LIST_ELEMENTS_RO(ospf_p->oiflist, node, oi))
		{
			if (CHECK_FLAG(oi->connected->flags, ZEBRA_IFA_SLAVE))
			{
				continue;
			}

			show_ip_ospf_neighbor_sub(vty, oi);
			
			if((argv[2] != NULL)  && (oi->type == OSPF_IFTYPE_NBMA))
			{
				/* print Down neighbor status */
				for (ALL_LIST_ELEMENTS_RO(oi->nbr_nbma, nd, nbr_nbma))
				{
					if (nbr_nbma == NULL)
					{
#ifdef DEBUG_ROBUST
						zlog_err("%s():%d: ospfd terniated!", __FUNCTION__, __LINE__);
#endif
						continue;
					}

					if (nbr_nbma->nbr == NULL
					  || nbr_nbma->nbr->state == NSM_Down)
					{
						vty_out(vty, "%-15s %-15s %-5d %-10s ", "-",
							  "-", nbr_nbma->priority, "Down");
						char if_name[IFNET_NAMESIZE] = "";
						ospf_interface_name_simplify(oi->ifp->name, if_name);
						vty_out(vty, "%-15s%s", if_name, VTY_NEWLINE);
					}
				}
			}
		}
	}
	else
	{
		for (ALL_LIST_ELEMENTS_RO(ospf_p->oiflist, node, oi))
        {
            for (rn = route_top(oi->nbrs); rn; rn = route_next(rn))
            {
                if ((nbr = rn->info))
                {
                    if (nbr != oi->nbr_self)
                    {
                        if (nbr->state != NSM_Down)
                        {
                            show_ip_ospf_neighbor_detail_sub(vty, oi, nbr);
                        }
                    }
                }
            }
				
            if ((argv[2] != NULL) && (oi->type == OSPF_IFTYPE_NBMA))
            {
                

                for (ALL_LIST_ELEMENTS_RO(oi->nbr_nbma, nd, nbr_nbma))
                {
                    if (nbr_nbma->nbr == NULL
                            || nbr_nbma->nbr->state == NSM_Down)
                    {
                        show_ip_ospf_nbr_nbma_detail_sub(vty, oi, nbr_nbma);
                    }
                }
            }
        }
	}
	return;
}

DEFUN(show_ip_ospf_neighbor_h3c,
	show_ip_ospf_neighbor_h3c_cmd,
	"display ospf [<1-65535>] peer { verbose | all }",
	"Display current system information\n"
	"OSPF information\n"
	"OSPF instance number\n"
	"Specify a neighbor router\n"
	"Detail information about neighbors\n"
	"include down status neighbor\n")
{
	int ospf_id;
	struct ospf *ospf = NULL;
	struct listnode *node = NULL;

	
	ospf_id = OSPF_DEFAULT_PROCESS_ID;

	if (argv[0] != NULL)
	{
		VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID_H3C);
		
		ospf = ospf_lookup_id(ospf_id);

		if (ospf == NULL)
		{
			VTY_OSPF_INSTANCE_ERR
		}
		display_ospf_neighbour_info(vty, (char **)argv, ospf);
	}
	else
	{
		for (ALL_LIST_ELEMENTS_RO (om->ospf, node, ospf))
		{
			vty_out(vty, "%s%-10s OSPF Instance %d with Router ID %s%s",
            VTY_NEWLINE, " ", ospf->ospf_id, inet_ntoa(ospf->router_id), VTY_NEWLINE);
			vty_out(vty, "%-15s Neighbors%s", " ", VTY_NEWLINE);

			display_ospf_neighbour_info(vty, (char **)argv, ospf);
		}
	}
	return CMD_SUCCESS;
}


ALIAS(show_ip_ospf_neighbor_h3c,
	  show_ip_ospf_neighbor_cmd,
	  "show ip ospf [<1-255>] neighbor { detail | all}",
	  SHOW_STR
	  IP_STR
	  "OSPF information\n"
	  "OSPF instance number\n"
	  "Neighbor list\n"
	  "detail of all neighbors\n"
	  "include down status neighbor\n")



static void
show_ip_ospf_nbr_nbma_detail_sub(struct vty *vty, struct ospf_interface *oi,
                                 struct ospf_nbr_nbma *nbr_nbma)
{
    char timebuf[OSPF_TIME_DUMP_SIZE];

    if (CHECK_FLAG(oi->connected->flags, ZEBRA_IFA_SLAVE))
    {
        return;
    }

    /* Show neighbor ID. */
    vty_out(vty, " Neighbor %s,", "-");
    /* Show interface address. */
    vty_out(vty, " interface address %s%s",
            inet_ntoa(nbr_nbma->addr), VTY_NEWLINE);
    /* Show Area ID. */
    vty_out(vty, "    In the area %s via interface %s%s",
            ospf_area_desc_string(oi->area), IF_NAME(oi), VTY_NEWLINE);
    /* Show neighbor priority and state. */
    vty_out(vty, "    Neighbor priority is %d, State is %s,",
            nbr_nbma->priority, "Down");
    /* Show state changes. */
    vty_out(vty, " %d state changes%s", nbr_nbma->state_change, VTY_NEWLINE);
    /* Show PollInterval */
    vty_out(vty, "    Poll interval %d%s", nbr_nbma->v_poll, VTY_NEWLINE);
    /* Show poll-interval timer. */
    /*vty_out(vty, "    Poll timer due in %s%s",
            ospf_timer_dump(nbr_nbma->t_poll, timebuf, sizeof(timebuf)),
            VTY_NEWLINE);*/
	vty_out(vty, "    Poll timer due in %s%s",
            ospf_timer_dump_new(nbr_nbma->t_poll, nbr_nbma->v_poll, timebuf, sizeof(timebuf)),
            VTY_NEWLINE);
    /* Show poll-interval timer thread. */
    vty_out(vty, "    Thread Poll Timer %s%s",
            nbr_nbma->t_poll != 0 ? "on" : "off", VTY_NEWLINE);
}

static void
show_ip_ospf_neighbor_detail_sub(struct vty *vty, struct ospf_interface *oi,
                                 struct ospf_neighbor *nbr)
{
    char timebuf[OSPF_TIME_DUMP_SIZE];

	//add oi->type by zzl, Reference Huawei.
    if (CHECK_FLAG(oi->connected->flags, ZEBRA_IFA_SLAVE) || (oi->type == OSPF_IFTYPE_VIRTUALLINK))
    {
        return;
    }

    /* Show neighbor ID. */
    if (nbr->state == NSM_Attempt && nbr->router_id.s_addr == 0)
    {
        vty_out(vty, " Neighbor %s,", "-");
    }
    else
    {
        vty_out(vty, " Neighbor %s,", inet_ntoa(nbr->router_id));
    }

    /* Show interface address. */
    vty_out(vty, " interface address %s%s",
            inet_ntoa(nbr->address.u.prefix4), VTY_NEWLINE);
    /* Show Area ID. */
    vty_out(vty, "    In the area %s via interface %s%s",
            ospf_area_desc_string(oi->area), oi->ifp->name, VTY_NEWLINE);
    /* Show neighbor priority and state. */
    vty_out(vty, "    Priority: %d,   State: %s,",
            nbr->priority, LOOKUP(ospf_nsm_state_msg, nbr->state));
    /* Show state changes. */
    vty_out(vty, " %d state changes%s", nbr->state_change, VTY_NEWLINE);

    if (!IS_SET_DD_MS(nbr->dd_flags))
    {
        vty_out(vty, "    Mode : Master%s", VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "    Mode : Slave%s", VTY_NEWLINE);
    }

    if (nbr->ts_last_progress.tv_sec || nbr->ts_last_progress.tv_usec)
    {
        struct timeval res
        = tv_sub(time_get_recent_relative_time(), nbr->ts_last_progress);
        vty_out(vty, "    Most recent state change statistics:%s",
                VTY_NEWLINE);
        vty_out(vty, "      Progressive change %s ago%s",
                ospf_timeval_dump(&res, timebuf, sizeof(timebuf)),
                VTY_NEWLINE);
    }

    if (nbr->ts_last_regress.tv_sec || nbr->ts_last_regress.tv_usec)
    {
        struct timeval res
        = tv_sub(time_get_recent_relative_time(), nbr->ts_last_regress);
        vty_out(vty, "      Regressive change %s ago, due to %s%s",
                ospf_timeval_dump(&res, timebuf, sizeof(timebuf)),
                (nbr->last_regress_str ? nbr->last_regress_str : "??"),
                VTY_NEWLINE);
    }

    /* Show Designated Rotuer ID. */
    vty_out(vty, "    DR: %s,  ", inet_ntoa(nbr->d_router));
    /* Show Backup Designated Rotuer ID. */
    vty_out(vty, " BDR: %s%s", inet_ntoa(nbr->bd_router), VTY_NEWLINE);
    /* Show options. */
    vty_out(vty, "    Options %d %s%s", nbr->options,
            ospf_options_dump(nbr->options), VTY_NEWLINE);
    /* Show Router Dead interval timer. */
    vty_out(vty, "    Dead timer due in %s%s",
            //ospf_timer_dump(nbr->t_inactivity, timebuf, sizeof(timebuf)),
            ospf_timer_dump_new(nbr->t_inactivity, nbr->v_inactivity, timebuf, sizeof(timebuf)),
            VTY_NEWLINE);
    /* Show Database Summary list. */
    vty_out(vty, "    Database Summary List:   %d%s",
            ospf_db_summary_count(nbr), VTY_NEWLINE);
    /* Show Link State Request list. */
    vty_out(vty, "    Link State Request List:   %ld%s",
            ospf_ls_request_count(nbr), VTY_NEWLINE);
    /* Show Link State Retransmission list. */
    vty_out(vty, "    Link State Retransmission List:  %ld%s",
            ospf_ls_retransmit_count(nbr), VTY_NEWLINE);
    /* Show inactivity timer thread. */
    vty_out(vty, "    Thread Inactivity Timer %s%s",
            nbr->t_inactivity != 0 ? "on" : "off", VTY_NEWLINE);
    /* Show Database Description retransmission thread. */
    vty_out(vty, "    Thread Database Description Retransmision %s%s",
            nbr->t_db_desc != 0 ? "on" : "off", VTY_NEWLINE);
    /* Show Link State Request Retransmission thread. */
    vty_out(vty, "    Thread Link State Request Retransmission %s%s",
            nbr->t_ls_req != 0 ? "on" : "off", VTY_NEWLINE);
    /* Show Link State Update Retransmission thread. */
    vty_out(vty, "    Thread Link State Update Retransmission %s%s%s",
            nbr->t_ls_upd != 0 ? "on" : "off", VTY_NEWLINE, VTY_NEWLINE);
}

DEFUN(show_ip_ospf_neighbor_id,
      show_ip_ospf_neighbor_id_cmd,
      "show ip ospf [<1-255>] neighbor A.B.C.D",
      SHOW_STR
      IP_STR
      "OSPF information\n"
      "OSPF instance number\n"
      "Neighbor list\n"
      "Neighbor router ID\n")
{
    struct ospf *ospf;
    struct listnode *node;
    struct ospf_neighbor *nbr;
    struct ospf_interface *oi;
    struct in_addr router_id;
    int ret;
    int ospf_id;
    ospf_id = OSPF_DEFAULT_PROCESS_ID;

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID);
    }

    ospf = ospf_lookup_id(ospf_id);

    if (ospf == NULL)
    {
        VTY_OSPF_INSTANCE_ERR
    }

    ret = inet_aton(argv[1], &router_id);

    if (!ret)
    {
        vty_error_out(vty, "Please specify Neighbor ID by A.B.C.D%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    for (ALL_LIST_ELEMENTS_RO(ospf->oiflist, node, oi))
    {
        if ((nbr = ospf_nbr_lookup_by_routerid(oi->nbrs, &router_id)))
        {
            show_ip_ospf_neighbor_detail_sub(vty, oi, nbr);
        }
    }

    return CMD_SUCCESS;
}

DEFUN(show_ip_ospf_neighbor_id_h3c,
	show_ip_ospf_neighbor_id_h3c_cmd,
	"display ospf [ <1-65535> ] peer A.B.C.D",
	"Display current system information\n"
	"OSPF information\n"
	"OSPF instance number\n"
	"Specify a neighbor router\n"
	"Neighbor router ID\n")
{
	int ret;
	int ospf_id;
	struct ospf *ospf = NULL;
	struct listnode *node = NULL;
	struct ospf_neighbor *nbr = NULL;
	struct ospf_interface *oi = NULL;
	struct in_addr router_id;
	
	ospf_id = OSPF_DEFAULT_PROCESS_ID;

	ret = inet_aton(argv[1], &router_id);
	if (!ret)
	{
	  vty_error_out(vty, "Please specify Neighbor ID by A.B.C.D%s", VTY_NEWLINE);
	  return CMD_WARNING;
	}
	
	if (argv[0] != NULL)
	{
		VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID_H3C);

		ospf = ospf_lookup_id(ospf_id);

		if (ospf == NULL)
		{
			VTY_OSPF_INSTANCE_ERR
		}

		for (ALL_LIST_ELEMENTS_RO(ospf->oiflist, node, oi))
		{
			if ((nbr = ospf_nbr_lookup_by_routerid(oi->nbrs, &router_id)))
			{
				show_ip_ospf_neighbor_detail_sub(vty, oi, nbr);
			}
		}
	}
	else
	{
		for (ALL_LIST_ELEMENTS_RO (om->ospf, node, ospf))
		{
			for (ALL_LIST_ELEMENTS_RO(ospf->oiflist, node, oi))
			{
				if ((nbr = ospf_nbr_lookup_by_routerid(oi->nbrs, &router_id)))
				{
					show_ip_ospf_neighbor_detail_sub(vty, oi, nbr);
				}
			}
		}
	}

  return CMD_SUCCESS;
}


DEFUN(show_ip_ospf_neighbor_int_detail,
      show_ip_ospf_neighbor_int_detail_cmd,
      "show ip ospf [<1-255>] neighbor ethernet USP {detail}",
      SHOW_STR
      IP_STR
      "OSPF information\n"
      "OSPF instance number\n"
      "Neighbor list\n"
      "Ethernet interface type\n"
      CLI_INTERFACE_GIGABIT_ETHERNET_STR
      "detail of neighbors")
{
    struct ospf *ospf;
    struct ospf_interface *oi;
    struct interface *ifp;
    struct route_node *rn, *nrn;
    struct ospf_neighbor *nbr;
    uint32_t ifindex = 0;
    char if_name[128];
    int ret = -1;
    /* Check OSPF is enable. */
    int ospf_id;
    ospf_id = OSPF_DEFAULT_PROCESS_ID;

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID);
    }

    ospf = ospf_lookup_id(ospf_id);

    if (ospf == NULL)
    {
        VTY_OSPF_INSTANCE_ERR
    }

    ifindex = ifm_get_ifindex_by_name("ethernet", (char *) argv[1]);

    if (ifindex == 0)
    {
        vty_error_out(vty, "Command incomplete ,please check out%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*get the name of the interface*/
    ret = ifm_get_name_by_ifindex(ifindex , if_name);

    if (ret < 0)
    {
        vty_error_out(vty, "Failed to get interface information.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if ((ifp = if_lookup_by_name(if_name)) == NULL)
    {
        vty_error_out(vty, "No such interface.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (argv[2] == NULL)
    {

        show_ip_ospf_neighbour_header(vty);

        for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
        {
            oi = rn->info;

            if (oi == NULL)
            {
                continue;
            }

            show_ip_ospf_neighbor_sub(vty, oi);
        }
    }
    else
    {
        for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
        {
            if ((oi = rn->info))
            {
                for (nrn = route_top(oi->nbrs); nrn; nrn = route_next(nrn))
                {
                    if ((nbr = nrn->info))
                    {
                        if (nbr != oi->nbr_self)
                        {
                            if (nbr->state != NSM_Down)
                            {
                                show_ip_ospf_neighbor_detail_sub(vty, oi, nbr);
                            }
                        }
                    }
                }
            }
        }
    }

    return CMD_SUCCESS;
}


DEFUN(show_ip_ospf_neighbor_int_gigabitethernet_detail,
      show_ip_ospf_neighbor_int_gigabitethernet_detail_cmd,
      "show ip ospf [<1-255>] neighbor gigabitethernet USP {detail}",
      SHOW_STR
      IP_STR
      "OSPF information\n"
      "OSPF instance number\n"
      "Neighbor list\n"
      CLI_INTERFACE_GIGABIT_ETHERNET_STR
      CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
      "detail of neighbors")
{
    struct ospf *ospf;
    struct ospf_interface *oi;
    struct interface *ifp;
    struct route_node *rn, *nrn;
    struct ospf_neighbor *nbr;
    uint32_t ifindex = 0;
    char if_name[128];
    int ret = -1;
    /* Check OSPF is enable. */
    int ospf_id;
    ospf_id = OSPF_DEFAULT_PROCESS_ID;

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID);
    }

    ospf = ospf_lookup_id(ospf_id);

    if (ospf == NULL)
    {
        VTY_OSPF_INSTANCE_ERR
    }

    ifindex = ifm_get_ifindex_by_name("gigabitethernet", (char *) argv[1]);

    if (ifindex == 0)
    {
        vty_error_out(vty, "Command incomplete ,please check out%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*get the name of the interface*/
    ret = ifm_get_name_by_ifindex(ifindex , if_name);

    if (ret < 0)
    {
        vty_error_out(vty, "Failed to get interface information.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if ((ifp = if_lookup_by_name(if_name)) == NULL)
    {
        vty_error_out(vty, "No such interface.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (argv[2] == NULL)
    {
        show_ip_ospf_neighbour_header(vty);

        for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
        {
            oi = rn->info;

            if (oi == NULL)
            {
                continue;
            }

            show_ip_ospf_neighbor_sub(vty, oi);
        }
    }
    else
    {
        for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
        {
            if ((oi = rn->info))
            {
                for (nrn = route_top(oi->nbrs); nrn; nrn = route_next(nrn))
                {
                    if ((nbr = nrn->info))
                    {
                        if (nbr != oi->nbr_self)
                        {
                            if (nbr->state != NSM_Down)
                            {
                                show_ip_ospf_neighbor_detail_sub(vty, oi, nbr);
                            }
                        }
                    }
                }
            }
        }
    }

    return CMD_SUCCESS;
}

DEFUN(show_ip_ospf_neighbor_int_xgigabitethernet_detail,
      show_ip_ospf_neighbor_int_xgigabitethernet_detail_cmd,
      "show ip ospf [<1-255>] neighbor xgigabitethernet USP {detail}",
      SHOW_STR
      IP_STR
      "OSPF information\n"
      "OSPF instance number\n"
      "Neighbor list\n"
      CLI_INTERFACE_XGIGABIT_ETHERNET_STR
      CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
      "detail of neighbors")
{
    struct ospf *ospf;
    struct ospf_interface *oi;
    struct interface *ifp;
    struct route_node *rn, *nrn;
    struct ospf_neighbor *nbr;
    uint32_t ifindex = 0;
    char if_name[128];
    int ret = -1;
    /* Check OSPF is enable. */
    int ospf_id;
    ospf_id = OSPF_DEFAULT_PROCESS_ID;

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID);
    }

    ospf = ospf_lookup_id(ospf_id);

    if (ospf == NULL)
    {
        VTY_OSPF_INSTANCE_ERR
    }


    ifindex = ifm_get_ifindex_by_name("xgigabitethernet", (char *) argv[1]);

    if (ifindex == 0)
    {
        vty_error_out(vty, "Command incomplete ,please check out%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*get the name of the interface*/
    ret = ifm_get_name_by_ifindex(ifindex , if_name);

    if (ret < 0)
    {
        vty_error_out(vty, "Failed to get interface information.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if ((ifp = if_lookup_by_name(if_name)) == NULL)
    {
        vty_error_out(vty, "No such interface.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (argv[2] == NULL)
    {
        show_ip_ospf_neighbour_header(vty);

        for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
        {
            oi = rn->info;

            if (oi == NULL)
            {
                continue;
            }

            show_ip_ospf_neighbor_sub(vty, oi);
        }
    }
    else
    {
        for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
        {
            if ((oi = rn->info))
            {
                for (nrn = route_top(oi->nbrs); nrn; nrn = route_next(nrn))
                {
                    if ((nbr = nrn->info))
                    {
                        if (nbr != oi->nbr_self)
                        {
                            if (nbr->state != NSM_Down)
                            {
                                show_ip_ospf_neighbor_detail_sub(vty, oi, nbr);
                            }
                        }
                    }
                }
            }
        }
    }

    return CMD_SUCCESS;
}


DEFUN(show_ip_ospf_neighbor_by_intface_detail,
	show_ip_ospf_neighbor_by_intface_detail_h3c_cmd,
	"display ospf [ <1-65535> ] peer ( ethernet | gigabitethernet | xgigabitethernet ) USP { verbose }",
	"Display current system information\n"
	"OSPF information\n"
	"OSPF instance number\n"
	"Neighbor router information\n"
	"Ethernet interface type\n"
	"Gigabit Ethernet interface\n"
	"10Gigabit Ethernet interface\n"
	"The port/subport of the interface, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
	"Detail information about neighbors\n")
{
	int ospf_id;
	int ret = -1;
	uint32_t ifindex = 0;
	char if_name[128] = "";
	struct ospf *ospf = NULL;;
	struct interface *ifp = NULL;
	struct route_node *rn = NULL;
	struct route_node *nrn = NULL;
	struct ospf_interface *oi = NULL;
	struct ospf_neighbor *nbr = NULL;
	
	ospf_id = OSPF_DEFAULT_PROCESS_ID;
	if (argv[0] != NULL)
	{
		VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID_H3C);
		
		ospf = ospf_lookup_id(ospf_id);
		if (ospf == NULL)
		{
			VTY_OSPF_INSTANCE_ERR
		}
	}

	OSPF_LOG_DEBUG(" Interface name:%s %s", argv[1], argv[2]);
	ifindex = ifm_get_ifindex_by_name((char *) argv[1], (char *) argv[2]);

	if (ifindex == 0)
	{
		vty_error_out(vty, "Command incomplete ,please check out%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*get the name of the interface*/
	ret = ifm_get_name_by_ifindex(ifindex , if_name);
	if (ret < 0)
	{
		vty_error_out(vty, "Failed to get interface information.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if ((ifp = if_lookup_by_name(if_name)) == NULL)
	{
		vty_error_out(vty, "No such interface.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if (argv[3] == NULL)
    {
		show_ip_ospf_neighbour_header(vty);

	    for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
	    {
	        oi = rn->info;
			
	        if (oi == NULL)
	        {
	            continue;
	        }

			if ((argv[0] != NULL) && (ospf->ospf_id != oi->ospf->ospf_id))
			{
				vty_error_out(vty, "The interface is not in specific ospf instance.%s", VTY_NEWLINE);
			}
			
	        show_ip_ospf_neighbor_sub(vty, oi);
	    }
	}
	else
	{
		for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
		{
			if ((oi = rn->info))
			{
				if ((argv[0] != NULL) && (ospf->ospf_id != oi->ospf->ospf_id))
				{
					vty_error_out(vty, "The interface is not in specific ospf instance.%s", VTY_NEWLINE);
				}
				
				for (nrn = route_top(oi->nbrs); nrn; nrn = route_next(nrn))
				{
					if ((nbr = nrn->info))
					{
						if (nbr != oi->nbr_self)
						{
							if (nbr->state != NSM_Down)
							{
								show_ip_ospf_neighbor_detail_sub(vty, oi, nbr);
							}
						}
					}
				}
			}
		}
	}
	
	return CMD_SUCCESS;
}


static const char *show_database_desc[] =
{
    "unknown",
    "Router",
    "Network",
    "Sum-Net",
    "ASBR-Sum",
    "External",
    "Gro-LSA",
    "NSSA",
    "T-8 LSA",
    "Opq-Link",
    "Opq-Area",
    "Opq-As",
};


/* Show functions */
static int
show_lsa_summary(struct vty *vty, struct ospf_lsa *lsa, int self, int type, struct ospf *ospf)
{
    struct router_lsa *rl = NULL;
    struct summary_lsa *sl = NULL;
    struct as_external_lsa *al = NULL;
    u_int32_t metric;

    if (lsa != NULL)

        /* If self option is set, check LSA self flag. */
        if (self == 0 || IS_LSA_SELF(lsa))
        {
            /*when router lsa's link count is 0,do not show this lsa*/
            if (lsa->data->type == OSPF_ROUTER_LSA)
            {
                rl = (struct router_lsa *) lsa->data;

                if (rl->links == 0)
                {
                    return -1;
                }

                metric = ntohs(rl->link[0].metric);
            }
            else if (lsa->data->type == OSPF_SUMMARY_LSA || lsa->data->type == OSPF_ASBR_SUMMARY_LSA)
            {
                sl = (struct summary_lsa *) lsa->data;
                metric = GET_METRIC(sl->metric);
            }
            else if (lsa->data->type == OSPF_AS_NSSA_LSA || lsa->data->type == OSPF_AS_EXTERNAL_LSA)
            {
                al = (struct as_external_lsa *) lsa->data;
                metric = GET_METRIC(al->e[0].metric);
            }
            else
            {
                metric = 0;
            }

            vty_out(vty, "%-8s ", show_database_desc[type]);
            /* LSA common part show. */
            vty_out(vty, "%-15s ", inet_ntoa(lsa->data->id));
            vty_out(vty, "%-15s %-5d 0x%-9lx ",
                    inet_ntoa(lsa->data->adv_router), LS_AGE(lsa),
                    (u_long)ntohl(lsa->data->ls_seqnum));
            vty_out(vty, "%-3d ", ntohs(lsa->data->length));
            vty_out(vty, "%-d %s", metric, VTY_NEWLINE);
        }

    return 0;
}

static void
show_ip_ospf_database_header(struct vty *vty, struct ospf_lsa *lsa)
{
    struct router_lsa *rlsa = (struct router_lsa*) lsa->data;
    vty_out(vty, "  %-12s: %s%s", "LS Type",
            LOOKUP(ospf_lsa_type_msg, lsa->data->type), VTY_NEWLINE);
    vty_out(vty, "  %-12s: %s %s%s", "LS ID", inet_ntoa(lsa->data->id),
            LOOKUP(ospf_link_state_id_type_msg, lsa->data->type), VTY_NEWLINE);
    vty_out(vty, "  %-12s: %s%s", "Adv Router",
            inet_ntoa(lsa->data->adv_router), VTY_NEWLINE);
    vty_out(vty, "  %-12s: %d%s", "LS age", LS_AGE(lsa), VTY_NEWLINE);
    vty_out(vty, "  %-12s: %d%s", "Length", ntohs(lsa->data->length), VTY_NEWLINE);
    vty_out(vty, "  %-12s: 0x%-2x : %s%s", "Options",
            lsa->data->options,
            ospf_options_dump(lsa->data->options),
            VTY_NEWLINE);

    if (lsa->data->type == OSPF_ROUTER_LSA)
    {
        vty_out(vty, "  %-12s: 0x%x" , "Flags", rlsa->flags);

        if (rlsa->flags)
            vty_out(vty, " :%s%s%s%s",
                    IS_ROUTER_LSA_BORDER(rlsa) ? " ABR" : "",
                    IS_ROUTER_LSA_EXTERNAL(rlsa) ? " ASBR" : "",
                    IS_ROUTER_LSA_VIRTUAL(rlsa) ? " VL-endpoint" : "",
                    IS_ROUTER_LSA_SHORTCUT(rlsa) ? " Shortcut" : "");

        vty_out(vty, "%s", VTY_NEWLINE);
    }

    vty_out(vty, "  %-12s: %08lx%s", "Seq Number", (u_long)ntohl(lsa->data->ls_seqnum),
            VTY_NEWLINE);
    vty_out(vty, "  %-12s: 0x%04x%s", "Checksum", ntohs(lsa->data->checksum),
            VTY_NEWLINE);
}

const char *link_type_desc[] =
{
    "(null)",
    "another Router (point-to-point)",
    "TransNet",
    "StubNet",
    "Virtual Link",
};

const char *link_id_desc[] =
{
    "(null)",
    "Neighboring Router ID",
    "DR address",
    "Net",
    "Neighboring Router ID",
};

const char *link_data_desc[] =
{
    "(null)",
    "Router Interface address",
    "Router Interface address",
    "Net Mask",
    "Router Interface address",
};

/* Show router-LSA each Link information. */
static void
show_ip_ospf_database_router_links(struct vty *vty,
                                   struct router_lsa *rl)
{
    int len, type;
    unsigned int i;
    len = ntohs(rl->header.length) - 4;

    for (i = 0; i < ntohs(rl->links) && len > 0; len -= 12, i++)
    {
        type = rl->link[i].type;
        vty_out(vty, "   %-12s: %s (%s)%s", "Link ID", inet_ntoa(rl->link[i].link_id),
                link_id_desc[type], VTY_NEWLINE);
        vty_out(vty, "   %-12s: %s (%s)%s", "Link Data", inet_ntoa(rl->link[i].link_data),
                link_data_desc[type], VTY_NEWLINE);
        vty_out(vty, "   %-12s: %s%s", "Link Type",
                link_type_desc[type], VTY_NEWLINE);
        vty_out(vty, "      Number of TOS metrics: 0%s", VTY_NEWLINE);
        vty_out(vty, "       TOS 0 Metric: %d%s",
                ntohs(rl->link[i].metric), VTY_NEWLINE);
        vty_out(vty, "%s", VTY_NEWLINE);
    }
}

/* Show router-LSA detail information. */
static int
show_router_lsa_detail(struct vty *vty, struct ospf_lsa *lsa , struct ospf *ospf)
{
    if (lsa != NULL)
    {
        struct router_lsa *rl = (struct router_lsa *) lsa->data;

        if (rl->links != 0)
        {
            show_ip_ospf_database_header(vty, lsa);
            vty_out(vty, "  %-12s: %d%s", "*Link count", ntohs(rl->links),
                    VTY_NEWLINE);
            show_ip_ospf_database_router_links(vty, rl);
            vty_out(vty, "%s", VTY_NEWLINE);
        }
    }

    return 0;
}

/* Show network-LSA detail information. */
static int
show_network_lsa_detail(struct vty *vty, struct ospf_lsa *lsa, struct ospf *ospf)
{
    int length, i;

    if (lsa != NULL)
    {
        struct network_lsa *nl = (struct network_lsa *) lsa->data;
        show_ip_ospf_database_header(vty, lsa);
        vty_out(vty, "  %-10s: /%d%s", "Net Mask",
                ip_masklen(nl->mask), VTY_NEWLINE);
        length = ntohs(lsa->data->length) - OSPF_LSA_HEADER_SIZE - 4;

        for (i = 0; length > 0; i++, length -= 4)
        {
            vty_out(vty, "        Attached Router: %s%s",
                    inet_ntoa(nl->routers[i]), VTY_NEWLINE);
        }

        vty_out(vty, "%s", VTY_NEWLINE);
    }

    return 0;
}

/* Show summary-LSA detail information. */
static int
show_summary_lsa_detail(struct vty *vty, struct ospf_lsa *lsa, struct ospf *ospf)
{
    if (lsa != NULL)
    {
        struct summary_lsa *sl = (struct summary_lsa *) lsa->data;
        show_ip_ospf_database_header(vty, lsa);
        vty_out(vty, "  %-10s: /%d%s", "Net Mask", ip_masklen(sl->mask),
                VTY_NEWLINE);
        vty_out(vty, "        TOS: 0  Metric: %d%s", GET_METRIC(sl->metric),
                VTY_NEWLINE);
        vty_out(vty, "%s", VTY_NEWLINE);
    }

    return 0;
}

/* Show summary-ASBR-LSA detail information. */
static int
show_summary_asbr_lsa_detail(struct vty *vty, struct ospf_lsa *lsa, struct ospf *ospf)
{
    if (lsa != NULL)
    {
        struct summary_lsa *sl = (struct summary_lsa *) lsa->data;
        show_ip_ospf_database_header(vty, lsa);
        vty_out(vty, "  %-10s: /%d%s", "Net Mask",
                ip_masklen(sl->mask), VTY_NEWLINE);
        vty_out(vty, "        TOS: 0  Metric: %d%s", GET_METRIC(sl->metric),
                VTY_NEWLINE);
        vty_out(vty, "%s", VTY_NEWLINE);
    }

    return 0;
}

/* Show AS-external-LSA detail information. */
static int
show_as_external_lsa_detail(struct vty *vty, struct ospf_lsa *lsa , struct ospf* ospf)
{
    if (lsa != NULL)
    {
        /*when area's count is 1,external lsa do not show this lsa*/
        if (lsa->data->type == OSPF_AS_EXTERNAL_LSA && IS_LSA_SELF(lsa))
        {
            if ((listcount(ospf->areas) - ospf->anyNSSA - ospf->anySTUB) == 0)
            {
                return -1;
            }
        }

        struct as_external_lsa *al = (struct as_external_lsa *) lsa->data;

        show_ip_ospf_database_header(vty, lsa);

        vty_out(vty, "  %-10s: /%d%s", "Net Mask",
                ip_masklen(al->mask), VTY_NEWLINE);

        vty_out(vty, "        Metric Type: %s%s",
                IS_EXTERNAL_METRIC(al->e[0].tos) ?
                "2 (Larger than any link state path)" : "1", VTY_NEWLINE);

        vty_out(vty, "        TOS: 0%s", VTY_NEWLINE);

        vty_out(vty, "        Metric: %d%s",
                GET_METRIC(al->e[0].metric), VTY_NEWLINE);

        vty_out(vty, "        Forward Address: %s%s",
                inet_ntoa(al->e[0].fwd_addr), VTY_NEWLINE);

        vty_out(vty, "        External Route Tag: %lu%s%s",
                (u_long)ntohl(al->e[0].route_tag), VTY_NEWLINE, VTY_NEWLINE);
    }

    return 0;
}


/* Show AS-NSSA-LSA detail information. */
static int
show_as_nssa_lsa_detail(struct vty *vty, struct ospf_lsa *lsa, struct ospf *ospf)
{
    if (lsa != NULL)
    {
        struct as_external_lsa *al = (struct as_external_lsa *) lsa->data;
        show_ip_ospf_database_header(vty, lsa);
        vty_out(vty, "  %-10s: /%d%s", "Net Mask",
                ip_masklen(al->mask), VTY_NEWLINE);
        vty_out(vty, "        Metric Type: %s%s",
                IS_EXTERNAL_METRIC(al->e[0].tos) ?
                "2 (Larger than any link state path)" : "1", VTY_NEWLINE);
        vty_out(vty, "        TOS: 0%s", VTY_NEWLINE);
        vty_out(vty, "        Metric: %d%s",
                GET_METRIC(al->e[0].metric), VTY_NEWLINE);
        vty_out(vty, "        NSSA: Forward Address: %s%s",
                inet_ntoa(al->e[0].fwd_addr), VTY_NEWLINE);
        vty_out(vty, "        External Route Tag: %u%s%s",
                ntohl(al->e[0].route_tag), VTY_NEWLINE, VTY_NEWLINE);
    }

    return 0;
}

static int
show_func_dummy(struct vty *vty, struct ospf_lsa *lsa, struct ospf *ospf)
{
    return 0;
}

static int
show_opaque_lsa_detail(struct vty *vty, struct ospf_lsa *lsa, struct ospf *ospf)
{
    if (lsa != NULL)
    {
        show_ip_ospf_database_header(vty, lsa);
        show_opaque_info_detail(vty, lsa);
        vty_out(vty, "%s", VTY_NEWLINE);
    }

    return 0;
}

int (*show_function[])(struct vty *, struct ospf_lsa *, struct ospf *) =
{
    NULL,
    show_router_lsa_detail,
    show_network_lsa_detail,
    show_summary_lsa_detail,
    show_summary_asbr_lsa_detail,
    show_as_external_lsa_detail,
    show_func_dummy,
    show_as_nssa_lsa_detail,  /* almost same as external */
    NULL,             /* type-8 */
    show_opaque_lsa_detail,
    show_opaque_lsa_detail,
    show_opaque_lsa_detail,
};

static void
show_lsa_prefix_set(struct vty *vty, struct prefix_ls *lp, struct in_addr *id,
                    struct in_addr *adv_router)
{
    memset(lp, 0, sizeof(struct prefix_ls));
    lp->family = 0;

    if (id == NULL)
    {
        lp->prefixlen = 0;
    }
    else if (adv_router == NULL)
    {
        lp->prefixlen = 32;
        lp->id = *id;
    }
    else
    {
        lp->prefixlen = 64;
        lp->id = *id;
        lp->adv_router = *adv_router;
    }
}

static void
show_lsa_detail_proc(struct vty *vty, struct route_table *rt,
                     struct in_addr *id, struct in_addr *adv_router, struct ospf* ospf)
{
    struct prefix_ls lp;
    struct route_node *rn, *start;
    struct ospf_lsa *lsa;
    show_lsa_prefix_set(vty, &lp, id, adv_router);
    start = route_node_get(rt, (struct prefix *) & lp);

    if (start)
    {
        route_lock_node(start);

        for (rn = start; rn; rn = route_next_until(rn, start))
        {
            if ((lsa = rn->info))
            {
                if (show_function[lsa->data->type] != NULL)
                {
                    show_function[lsa->data->type](vty, lsa, ospf);
                }
            }
        }

        route_unlock_node(start);
    }
}

/* Show detail LSA information
   -- if id is NULL then show all LSAs. */
static void
show_lsa_detail(struct vty *vty, struct ospf *ospf, int type,
                struct in_addr *id, struct in_addr *adv_router)
{
    struct listnode *node;
    struct ospf_area *area;

    switch (type)
    {
        case OSPF_AS_EXTERNAL_LSA:
        case OSPF_OPAQUE_AS_LSA:
            vty_out(vty, "                %s %s%s",
                    show_database_desc[type],
                    VTY_NEWLINE, VTY_NEWLINE);
            show_lsa_detail_proc(vty, AS_LSDB(ospf, type), id, adv_router, ospf);
            break;
        default:

            for (ALL_LIST_ELEMENTS_RO(ospf->areas, node, area))
            {
                vty_out(vty, "%s                %s (Area %s)%s%s",
                        VTY_NEWLINE, show_database_desc[type],
                        ospf_area_desc_string(area), VTY_NEWLINE, VTY_NEWLINE);
                show_lsa_detail_proc(vty, AREA_LSDB(area, type), id, adv_router, ospf);
            }

            break;
    }
}

static void
show_lsa_detail_adv_router_proc(struct vty *vty, struct route_table *rt,
                                struct in_addr *adv_router, struct ospf *ospf)
{
    struct route_node *rn;
    struct ospf_lsa *lsa;

    for (rn = route_top(rt); rn; rn = route_next(rn))
    {
        if ((lsa = rn->info))
        {
            if (IPV4_ADDR_SAME(adv_router, &lsa->data->adv_router))
            {
                if (CHECK_FLAG(lsa->flags, OSPF_LSA_LOCAL_XLT))
                {
                    continue;
                }

                if (show_function[lsa->data->type] != NULL)
                {
                    show_function[lsa->data->type](vty, lsa, ospf);
                }
            }
        }
    }
}

/* Show detail LSA information. */
static void
show_lsa_detail_adv_router(struct vty *vty, struct ospf *ospf, int type,
                           struct in_addr *adv_router)
{
    struct listnode *node;
    struct ospf_area *area;

    switch (type)
    {
        case OSPF_AS_EXTERNAL_LSA:
        case OSPF_OPAQUE_AS_LSA:
            vty_out(vty, "                %s %s%s",
                    show_database_desc[type],
                    VTY_NEWLINE, VTY_NEWLINE);
            show_lsa_detail_adv_router_proc(vty, AS_LSDB(ospf, type),
                                            adv_router, ospf);
            break;
        default:

            for (ALL_LIST_ELEMENTS_RO(ospf->areas, node, area))
            {
                vty_out(vty, "%s                %s (Area %s)%s%s",
                        VTY_NEWLINE, show_database_desc[type],
                        ospf_area_desc_string(area), VTY_NEWLINE, VTY_NEWLINE);
                show_lsa_detail_adv_router_proc(vty, AREA_LSDB(area, type),
                                                adv_router, ospf);
            }

            break;
    }
}

static void
show_ip_ospf_lsdb_header(struct vty *vty)
{
    vty_out(vty, "%s%-8s %-15s %-15s %-5s %-11s %-3s %-s%s",
            VTY_NEWLINE, "type", "LinkState ID", "ADV Router", "Age", "Seq#",
            "Len", "Metric", VTY_NEWLINE);
}


static void
show_ip_ospf_database_summary(struct vty *vty, struct ospf *ospf, int self)
{
    struct ospf_lsa *lsa;
    struct route_node *rn;
    struct ospf_area *area;
    struct listnode *node;
    int type;

    for (ALL_LIST_ELEMENTS_RO(ospf->areas, node, area))
    {
        vty_out(vty, "%s                 Area %s",
                VTY_NEWLINE, ospf_area_desc_string(area));
        show_ip_ospf_lsdb_header(vty);

        for (type = OSPF_MIN_LSA; type < OSPF_MAX_LSA; type++)
        {
            switch (type)
            {
                case OSPF_AS_EXTERNAL_LSA:
                case OSPF_OPAQUE_AS_LSA:
                    continue;
                default:
                    break;
            }

            if (ospf_lsdb_count_self(area->lsdb, type) > 0 ||
                    (!self && ospf_lsdb_count(area->lsdb, type) > 0))
            {
                LSDB_LOOP(AREA_LSDB(area, type), rn, lsa)
                {
                    show_lsa_summary(vty, lsa, self, type, ospf);
                }
            }
        }
    }

    for (type = OSPF_MIN_LSA; type < OSPF_MAX_LSA; type++)
    {
        switch (type)
        {
            case OSPF_AS_EXTERNAL_LSA:
            case OSPF_OPAQUE_AS_LSA:
                break;
            default:
                continue;
        }

        if (ospf_lsdb_count_self(ospf->lsdb, type) ||
                (!self && ospf_lsdb_count(ospf->lsdb, type)))
        {
            /*If all areas are nssa or stub area in current instances, mean no External lsa*/
            if ((listcount(ospf->areas) - ospf->anyNSSA - ospf->anySTUB) == 0)
            {
                return;
            }

            vty_out(vty, "%s", VTY_NEWLINE);
            vty_out(vty, "                 AS External Database %s", VTY_NEWLINE);
            LSDB_LOOP(AS_LSDB(ospf, type), rn, lsa)
            {
                show_lsa_summary(vty, lsa, self, type, ospf);
            }
        }
    }

    vty_out(vty, "%s", VTY_NEWLINE);
}

static void
show_ip_ospf_database_maxage(struct vty *vty, struct ospf *ospf)
{
    struct route_node *rn;
    vty_out(vty, "%s                MaxAge Link States:%s%s",
            VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE);

    for (rn = route_top(ospf->maxage_lsa); rn; rn = route_next(rn))
    {
        struct ospf_lsa *lsa;

        if ((lsa = rn->info) != NULL)
        {
            /*when area's count is 1,external lsa do not show this lsa*/
            if (lsa->data->type == OSPF_AS_EXTERNAL_LSA && IS_LSA_SELF(lsa))
            {
                if ((listcount(ospf->areas) - ospf->anyNSSA - ospf->anySTUB) == 0)
                {
                    return ;
                }
            }

            vty_out(vty, "Link type: %d%s", lsa->data->type, VTY_NEWLINE);
            vty_out(vty, "Link State ID: %s%s",
                    inet_ntoa(lsa->data->id), VTY_NEWLINE);
            vty_out(vty, "Advertising Router: %s%s",
                    inet_ntoa(lsa->data->adv_router), VTY_NEWLINE);
            vty_out(vty, "LSA lock count: %d%s", lsa->lock, VTY_NEWLINE);
            vty_out(vty, "%s", VTY_NEWLINE);
        }
    }
}

#define OSPF_LSA_TYPE_NSSA_DESC      "NSSA external link state\n"
#define OSPF_LSA_TYPE_NSSA_CMD_STR   "|nssa-external"

#define OSPF_LSA_TYPE_OPAQUE_LINK_DESC "Link local Opaque-LSA\n"
#define OSPF_LSA_TYPE_OPAQUE_AREA_DESC "Link area Opaque-LSA\n"
#define OSPF_LSA_TYPE_OPAQUE_AS_DESC   "Link AS Opaque-LSA\n"
#define OSPF_LSA_TYPE_OPAQUE_CMD_STR   "|opaque-link|opaque-area|opaque-as"

#define OSPF_LSA_TYPES_CMD_STR                                                \
    "asbr-summary|external|network|router|summary"                            \
    OSPF_LSA_TYPE_NSSA_CMD_STR                                                \
    OSPF_LSA_TYPE_OPAQUE_CMD_STR

#define OSPF_LSA_TYPES_DESC                                                   \
   "ASBR summary link states\n"                                               \
   "External link states\n"                                                   \
   "Network link states\n"                                                    \
   "Router link states\n"                                                     \
   "Network summary link states\n"                                            \
   OSPF_LSA_TYPE_NSSA_DESC                                                    \
   OSPF_LSA_TYPE_OPAQUE_LINK_DESC                                             \
   OSPF_LSA_TYPE_OPAQUE_AREA_DESC                                             \
   OSPF_LSA_TYPE_OPAQUE_AS_DESC

static int
ospf_get_lsa_num(struct ospf* ospf)
{
    int num = 0;
    struct listnode *node, *nnode;
    struct ospf_area *area = NULL;

    if (list_isempty(ospf->areas))
    {
        return 0;
    }

    for (ALL_LIST_ELEMENTS(ospf->areas, node, nnode, area))
    {
        num += area->lsdb->total;
    }

    if ((listcount(ospf->areas) - ospf->anyNSSA - ospf->anySTUB) == 0)
    {
        num = num + (ospf_lsdb_count_all(ospf->lsdb) - ospf_lsdb_count_self(ospf->lsdb, OSPF_AS_EXTERNAL_LSA));
    }
    else
    {
        num = num + (ospf_lsdb_count_all(ospf->lsdb));
    }

    return num;
}
static int
ospf_get_route_num(struct ospf* ospf)
{
    int num = 0;
    struct route_node *rn1 = NULL;
    struct route_node *rn2 = NULL;
    struct ospf_route *or = NULL;

    if (ospf->new_table != NULL)
    {
        for (rn1 = route_top(ospf->new_table); rn1; rn1 = route_next(rn1))
        {
            if ((or = rn1->info) != NULL)
            {
                num += or->paths->count;
            }
        }
    }

    if (ospf->old_external_route != NULL)
    {
        for (rn2 = route_top(ospf->old_external_route); rn2; rn2 = route_next(rn2))
        {
            if ((or = rn2->info) != NULL)
            {
                num += or->paths->count;
            }
        }
    }

    return num;
}

DEFUN(show_ip_ospf_statics,
      show_ip_ospf_statics_cmd,
      "show ip ospf statistics ",
      SHOW_STR
      IP_STR
      "OSPF information\n"
      "ospf packet,lsa,route statics\n")
{
    struct ospf *top = NULL;
    struct listnode *node = NULL;

    if (listcount(om->ospf) == 0)
    {
        vty_error_out(vty, "There is no active ospf instance %s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }

    vty_out(vty, "%s", VTY_NEWLINE);
    vty_out(vty, "--------------------ospf packet---------------------%s", VTY_NEWLINE);
    vty_out(vty, "     send success:%-12d  send error:%-d%s", ospf_sta.send_statics,
            ospf_sta.send_error, VTY_NEWLINE);
    vty_out(vty, "     recv success:%-12d  recv error:%-d%s", ospf_sta.recv_statics,
            ospf_sta.recv_all - ospf_sta.recv_statics, VTY_NEWLINE);
    vty_out(vty, "     hello,send  :%-12d  recv:%-d%s", ospf_sta.s_hel, ospf_sta.r_hel, VTY_NEWLINE);
    vty_out(vty, "     DD,   send  :%-12d  recv:%-d%s", ospf_sta.s_dd, ospf_sta.r_dd, VTY_NEWLINE);
    vty_out(vty, "     LSR,  send  :%-12d  recv:%-d%s", ospf_sta.s_lsr, ospf_sta.r_lsr, VTY_NEWLINE);
    vty_out(vty, "     LSU,  send  :%-12d  recv:%-d%s", ospf_sta.s_lsu, ospf_sta.r_lsu, VTY_NEWLINE);
    vty_out(vty, "     LSA,  send  :%-12d  recv:%-d%s", ospf_sta.s_lsa, ospf_sta.r_lsa, VTY_NEWLINE);
    vty_out(vty, "%s", VTY_NEWLINE);

    for (ALL_LIST_ELEMENTS_RO(om->ospf, node, top))
    {
        if (top != NULL)
        {
            int lsa_num = ospf_get_lsa_num(top);
            int route_num = ospf_get_route_num(top);
            vty_out(vty, "------------instance %d total lsa and route-----------%s%s", top->ospf_id, VTY_NEWLINE, VTY_NEWLINE);
            vty_out(vty, "     lsa total   :%-12d", lsa_num);
            vty_out(vty, "  route total:%-d   %s%s", route_num, VTY_NEWLINE, VTY_NEWLINE);
            vty_out(vty, "%s", VTY_NEWLINE);
        }
    }

    return CMD_SUCCESS;
}

ALIAS(show_ip_ospf_statics,
	display_ospf_statics_h3c_cmd,
	"display ospf statistics",
	"Display current system information\n"
	"OSPF information\n"
	"ospf packet,lsa,route statics\n")


DEFUN(show_ip_ospf_lsdb,
      show_ip_ospf_lsdb_all_cmd,
      "show ip ospf [<1-255>] lsdb {self-originate-all|max-age}",
      SHOW_STR
      IP_STR
      "OSPF information\n"
      "OSPF instance number\n"
      "Database summary\n"
      "self originate lsa all\n"
      "LSAs in MaxAge list\n")
{
    struct ospf *ospf = NULL;
    /* Check OSPF is enable. */
    int ospf_id;
    ospf_id = OSPF_DEFAULT_PROCESS_ID;

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID);
    }

    ospf = ospf_lookup_id(ospf_id);

    if (ospf == NULL)
    {
        VTY_OSPF_INSTANCE_ERR
    }

    vty_out(vty, "%s         OSPF Instance %d with Router ID (%s)%s%s", VTY_NEWLINE,
            ospf->ospf_id, inet_ntoa(ospf->router_id), VTY_NEWLINE, VTY_NEWLINE);

    /* Show all LSA. */
    if (!argv[1] && !argv[2])
    {
        show_ip_ospf_database_summary(vty, ospf, 0);
        return CMD_SUCCESS;
    }
    /* Show all LSA sel-originate. */
    else if (argv[1])
    {
        show_ip_ospf_database_summary(vty, ospf, 1);
    }
    else if (argv[2])
    {
        show_ip_ospf_database_maxage(vty, ospf);
        return CMD_SUCCESS;
    }

    return CMD_SUCCESS;
}


static int display_ospf_lsdb_brief_info(struct vty *vty, int argc, char **argv, struct ospf *ospf_p)
{
	/* Show all LSA. */
    if (!argv[1])
    {
        show_ip_ospf_database_summary(vty, ospf_p, 0);
    }
    /* Show all LSA sel-originate. */
    else if (argv[1] && (strncmp(argv[1], "sel", 3) == 0))
    {
        show_ip_ospf_database_summary(vty, ospf_p, 1);
    }
    else if (argv[1] && (strncmp(argv[1], "max", 3) == 0))
    {
        show_ip_ospf_database_maxage(vty, ospf_p);
    }
	return CMD_SUCCESS;
}

DEFUN(display_ospf_lsdb_h3c,
	display_ospf_lsdb_all_h3c_cmd,
	"display ospf [ <1-65535> ] lsdb { self-originate | brief }",
	"Display current system information\n"
	"OSPF information\n"
	"OSPF instance number\n"
	"Database summary\n"
	"Self originate lsa all\n"
	"Display in brief format\n")
{
	int ospf_id;
    struct ospf *ospf = NULL;
    struct listnode *node = NULL;
    ospf_id = OSPF_DEFAULT_PROCESS_ID;

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID_H3C);
   
	    ospf = ospf_lookup_id(ospf_id);

	    if (ospf == NULL)
	    {
	        VTY_OSPF_INSTANCE_ERR
	    }

		vty_out(vty, "%s         OSPF Instance %d with Router ID (%s)%s%s", VTY_NEWLINE,
            ospf->ospf_id, inet_ntoa(ospf->router_id), VTY_NEWLINE, VTY_NEWLINE);

		display_ospf_lsdb_brief_info(vty, argc, (char **)argv, ospf);
	}
	else
	{
		for (ALL_LIST_ELEMENTS_RO (om->ospf, node, ospf))
		{
    		vty_out(vty, "%s         OSPF Instance %d with Router ID (%s)%s%s", VTY_NEWLINE,
            	ospf->ospf_id, inet_ntoa(ospf->router_id), VTY_NEWLINE, VTY_NEWLINE);
			
			display_ospf_lsdb_brief_info(vty, argc, (char **)argv, ospf);
			vty_out(vty, " **********************************************************************%s", VTY_NEWLINE);
		}
	}
	
    return CMD_SUCCESS;
}

ALIAS(display_ospf_lsdb_h3c,
	display_ospf_lsdb_all_h3c_format2_cmd,
	"display ospf [ <1-65535> ] lsdb (max-age)",
	"Display current system information\n"
	"OSPF information\n"
	"OSPF instance number\n"
	"Database summary\n"
	"LSAs in MaxAge list\n")


DEFUN(show_ip_ospf_lsdb_self,
      show_ip_ospf_lsdb_self_cmd,
      "show ip ospf [<1-255>] lsdb (" OSPF_LSA_TYPES_CMD_STR ") {self-originate-single}",
      SHOW_STR
      IP_STR
      "OSPF information\n"
      "OSPF instance number\n"
      "Database summary\n"
      OSPF_LSA_TYPES_DESC
      "Self-originated link states\n")
{
    struct ospf *ospf;
    int type;
    struct in_addr adv_router;
    /* Check OSPF is enable. */
    int ospf_id;
    ospf_id = OSPF_DEFAULT_PROCESS_ID;

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID);
    }

    ospf = ospf_lookup_id(ospf_id);

    if (ospf == NULL)
    {
        VTY_OSPF_INSTANCE_ERR
    }

    vty_out(vty, "%s		OSPF Router with ID (%s)%s%s", VTY_NEWLINE,
            inet_ntoa(ospf->router_id), VTY_NEWLINE, VTY_NEWLINE);

    /* Set database type to show. */
    if (strncmp(argv[1], "r", 1) == 0)
    {
        type = OSPF_ROUTER_LSA;
    }
    else if (strncmp(argv[1], "ne", 2) == 0)
    {
        type = OSPF_NETWORK_LSA;
    }
    else if (strncmp(argv[1], "ns", 2) == 0)
    {
        type = OSPF_AS_NSSA_LSA;
    }
    else if (strncmp(argv[1], "s", 1) == 0)
    {
        type = OSPF_SUMMARY_LSA;
    }
    else if (strncmp(argv[1], "a", 1) == 0)
    {
        type = OSPF_ASBR_SUMMARY_LSA;
    }
    else if (strncmp(argv[1], "e", 1) == 0)
    {
        type = OSPF_AS_EXTERNAL_LSA;
    }
    else if (strncmp(argv[1], "opaque-l", 8) == 0)
    {
        type = OSPF_OPAQUE_LINK_LSA;
    }
    else if (strncmp(argv[1], "opaque-ar", 9) == 0)
    {
        type = OSPF_OPAQUE_AREA_LSA;
    }
    else if (strncmp(argv[1], "opaque-as", 9) == 0)
    {
        type = OSPF_OPAQUE_AS_LSA;
    }
    else
    {
        return CMD_WARNING;
    }

    /* `show self originate LSA . */
    if (argv[2])
    {
        adv_router = ospf->router_id;
        show_lsa_detail_adv_router(vty, ospf, type, &adv_router);
    }
    else
    {
        show_lsa_detail(vty, ospf, type, NULL, NULL);
    }

    return CMD_SUCCESS;
}

static int display_ospf_lsdb_detail_info(struct vty *vty, int argc, char **argv, struct ospf *ospf_p)
{
	int type = 0;
	struct in_addr adv_router;
	
	/* Set database type to show. */
    if (strncmp(argv[1], "r", 1) == 0)
    {
        type = OSPF_ROUTER_LSA;
    }
    else if (strncmp(argv[1], "ne", 2) == 0)
    {
        type = OSPF_NETWORK_LSA;
    }
    else if (strncmp(argv[1], "ns", 2) == 0)
    {
        type = OSPF_AS_NSSA_LSA;
    }
    else if (strncmp(argv[1], "s", 1) == 0)
    {
        type = OSPF_SUMMARY_LSA;
    }
    else if (strncmp(argv[1], "asb", 3) == 0)
    {
        type = OSPF_ASBR_SUMMARY_LSA;
    }
    else if (strncmp(argv[1], "ase", 3) == 0)
    {
        type = OSPF_AS_EXTERNAL_LSA;
    }
    else if (strncmp(argv[1], "opaque-l", 8) == 0)
    {
        type = OSPF_OPAQUE_LINK_LSA;
    }
    else if (strncmp(argv[1], "opaque-ar", 9) == 0)
    {
        type = OSPF_OPAQUE_AREA_LSA;
    }
    else if (strncmp(argv[1], "opaque-as", 9) == 0)
    {
        type = OSPF_OPAQUE_AS_LSA;
    }
    else
    {
        return CMD_WARNING;
    }

    /* `show self originate LSA . */
    if (argv[2])
    {
        adv_router = ospf_p->router_id;
        show_lsa_detail_adv_router(vty, ospf_p, type, &adv_router);
    }
    else
    {
        show_lsa_detail(vty, ospf_p, type, NULL, NULL);
    }
	
	return CMD_SUCCESS;
}

DEFUN(display_ospf_lsdb_detail_h3c,
	display_ospf_lsdb_detail_h3c_cmd,
	"display ospf [ <1-65535> ] lsdb (router | network | summary | asbr | ase | nssa | opaque-link "\
	"| opaque-area | opaque-as) {self-originate}",
	"Display current system information\n"
	"OSPF information\n"
	"OSPF instance number\n"
	"Database summary\n"
	"Router link states\n"
	"Network link states\n"  
	"Network summary link states\n"
	"ASBR summary link states\n"
	"External link states\n"
	"NSSA external link state\n"
	"Link local Opaque-LSA\n"
	"Link area Opaque-LSA\n"
	"Link AS Opaque-LSA\n"
	"Self-originated link states\n")
{
    int ospf_id;
	int ret = CMD_WARNING;
	struct ospf *ospf = NULL;
	struct listnode *node = NULL;
	
    ospf_id = OSPF_DEFAULT_PROCESS_ID;

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID_H3C);
    
	    ospf = ospf_lookup_id(ospf_id);
	    if (ospf == NULL)
	    {
	        VTY_OSPF_INSTANCE_ERR
	    }

	    vty_out(vty, "%s		OSPF Router with ID (%s)%s%s", VTY_NEWLINE,
	            inet_ntoa(ospf->router_id), VTY_NEWLINE, VTY_NEWLINE);

		ret = display_ospf_lsdb_detail_info(vty, argc, (char **)argv, ospf);
	}
	else
	{
		for (ALL_LIST_ELEMENTS_RO (om->ospf, node, ospf))
		{
			vty_out(vty, "%s	OSPF Router with ID (%s) in OSPF Instance %d%s%s", VTY_NEWLINE,
	            inet_ntoa(ospf->router_id), ospf->ospf_id, VTY_NEWLINE, VTY_NEWLINE);

			ret = display_ospf_lsdb_detail_info(vty, argc, (char **)argv, ospf);
			vty_out(vty, " **********************************************************************%s", VTY_NEWLINE);	
		}
	}
	
	return ret;
}

DEFUN(ip_ospf_auth_simple,
      ip_ospf_auth_simple_cmd,
      "ip ospf auth simple PASSWORD",
      "IP Information\n"
      "OSPF interface commands\n"
      "Enable authentication on this interface\n"
      "Use simple authentication on this interface\n"
      "The simple authentication password <1-8>\n")
{
    struct interface *ifp = NULL;
    struct ospf_if_params *params = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    if (strlen(argv[0]) > 8)
    {
        vty_error_out(vty, "OSPF auth PASSWORD must be shorter than 8%s",
                VTY_NEWLINE);
        return CMD_WARNING;
    }

    ospf_auth_md5_delete_delete_all(params);
    SET_IF_PARAM(params, auth_type);
    params->auth_type = OSPF_AUTH_SIMPLE;
    memset(params->auth_simple, 0, OSPF_AUTH_SIMPLE_SIZE + 1);
    strncpy((char *) params->auth_simple, argv[0], OSPF_AUTH_SIMPLE_SIZE);
    SET_IF_PARAM(params, auth_simple);
    return CMD_SUCCESS;
}


	
DEFUN(ospf_interface_authentication,
	ospf_interface_authentication_h3c_cmd,
	"ospf authentication-mode simple (cipher|plain) PASSWORD",
	"OSPF interface commands\n"
	"Specify an authentication mode\n"
	"Simple authentication mode\n"
	"Encryption type (Cryptogram)\n"
	"Encryption type (Plain text)\n"
	"The password (length 1~8 bytes)\n")
{
	struct interface *ifp = NULL;
	struct ospf_if_params *params = NULL;
	uint32_t ifindex = (uint32_t)vty->index;
	
	VTY_CHECK_NM_INTERFACE(ifindex);
	ifp = ospf_find_interface(ifindex);
	params = IF_DEF_PARAMS(ifp);

 	if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }
  
	if (strlen(argv[1]) > 8)
	{
		vty_error_out(vty, "OSPF auth PASSWORD length must be 1~8 bytes%s",
			  VTY_NEWLINE);
		return CMD_WARNING;
	}

	//clean old auth
	ospf_auth_md5_delete_delete_all(params);

	//creat new auth
    SET_IF_PARAM(params, auth_type);
    params->auth_type = OSPF_AUTH_SIMPLE;
    memset(params->auth_simple, 0, OSPF_AUTH_SIMPLE_SIZE + 1);
    strncpy((char *) params->auth_simple, argv[1], OSPF_AUTH_SIMPLE_SIZE);
    SET_IF_PARAM(params, auth_simple);
	
	return CMD_SUCCESS;
}


DEFUN(ospf_interface_authentication_md5,
	  ospf_interface_authentication_md5_h3c_cmd,
	  "ospf authentication-mode (hmac-md5|md5) <1-255> (cipher|plain) PASSWORD",
	  "OSPF interface commands\n"
	  "Specify an authentication mode\n"
	  "Use HMAC-MD5 algorithm\n"
	  "Use MD5 algorithm\n"
	  "Key ID\n"
	  "Encryption type (Cryptogram)\n"
	  "Encryption type (Plain text)\n"
	  "The password (length 1~255 bytes)\n")
{
	u_char key_id;
    struct crypt_key *ck;
	struct interface *ifp = NULL;
    struct ospf_if_params *params = NULL;
    //char plaintext_key[512];
    uint32_t ifindex = (uint32_t)vty->index;

	
    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

	if (strlen(argv[3]) > 255)
	{
		vty_error_out(vty, "OSPF md5 auth PASSWORD length must be 1~255 bytes%s",
				VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	//clean old auth
    ospf_auth_md5_delete_delete_all(params);
	params->auth_type = OSPF_AUTH_NOTSET;
    UNSET_IF_PARAM(params, auth_type);
    memset(params->auth_simple, 0, OSPF_AUTH_SIMPLE_SIZE + 1);
    UNSET_IF_PARAM(params, auth_simple);

	//creat new auth
    SET_IF_PARAM(params, auth_type);
    params->auth_type = OSPF_AUTH_CRYPTOGRAPHIC;

	//get key_id
    key_id = strtol(argv[1], NULL, 10);

    if (ospf_crypt_key_lookup(params->auth_crypt, key_id) != NULL)
    {
        return CMD_SUCCESS;
    }

	//creat md5 struct
	ck = ospf_crypt_key_new();
	ck->key_id = (u_char) key_id;
	memset(ck->auth_key, 0, OSPF_AUTH_MD5_SIZE + 1);
	strncpy((char *) ck->auth_key, argv[3], OSPF_AUTH_MD5_SIZE);
	ck->auth_key_len = strlen(argv[3]);

	if (ck->auth_key_len < OSPF_MD5_SIZE)
	{
		ck->auth_key_len = OSPF_MD5_SIZE;
	}

	ospf_crypt_key_add(params->auth_crypt, ck);
    SET_IF_PARAM(params, auth_crypt);
	
	return CMD_SUCCESS;
}

DEFUN(no_ospf_interface_authentication,
	no_ospf_interface_authentication_h3c_cmd,
	"undo ospf authentication-mode",
	"Cancel the current setting\n"
	"OSPF interface commands\n"
	"Specify an authentication mode\n")
{
    struct in_addr addr;
	struct interface *ifp = NULL;
    struct ospf_if_params *params = NULL;
    uint32_t ifindex = (uint32_t)vty->index;

	
    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

	/*delete all md5 key*/
	ospf_auth_md5_delete_delete_all(params);
	params->auth_type = OSPF_AUTH_NOTSET;
    UNSET_IF_PARAM(params, auth_type);
	//clean simple auth
	memset(params->auth_simple, 0, OSPF_AUTH_SIMPLE_SIZE);
    UNSET_IF_PARAM(params, auth_simple);

	if (params != IF_DEF_PARAMS(ifp))
    {
        ospf_free_if_params(ifp, addr);
        ospf_if_update_params(ifp, addr);
    }
	
	return CMD_SUCCESS;
}


ALIAS(no_ospf_interface_authentication,
	undo_ospf_interface_simple_authentication_h3c_cmd,
	"undo ospf authentication-mode simple",
	"Cancel current configuration\n"
	"OSPF interface commands\n"
	"Specify an authentication mode\n"
	"Simple authentication mode\n")


ALIAS(no_ospf_interface_authentication,
	undo_ospf_interface_md5_authentication_h3c_cmd,
	"undo ospf authentication-mode (hmac-md5|md5) <1-255>",
	"Cancel current configuration\n"
	"OSPF interface commands\n"
	"Specify an authentication mode\n"
	"Use HMAC-MD5 algorithm\n"
	"Use MD5 algorithm\n"
	"Key ID\n")


ALIAS(no_ospf_interface_authentication,
	undo_ospf_interface_keychain_authentication_h3c_cmd,
	"undo ospf authentication-mode keychain",
	"Cancel current configuration\n"
	"OSPF interface commands\n"
	"Specify an authentication mode\n"
	"Use a keychain for authentication\n")


DEFUN(no_ip_ospf_auth,
      no_ip_ospf_auth_cmd,
      "no ip ospf auth",
      NO_STR
      "IP Information\n"
      "OSPF interface commands\n"
      "Disable authentication on this interface\n")
{
    struct interface *ifp = NULL;
    struct in_addr addr;
    struct ospf_if_params *params = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    /*delete all md5 key*/
    ospf_auth_md5_delete_delete_all(params);
    params->auth_type = OSPF_AUTH_NOTSET;
    UNSET_IF_PARAM(params, auth_type);
    memset(params->auth_simple, 0, OSPF_AUTH_SIMPLE_SIZE);
    UNSET_IF_PARAM(params, auth_simple);

    if (params != IF_DEF_PARAMS(ifp))
    {
        ospf_free_if_params(ifp, addr);
        ospf_if_update_params(ifp, addr);
    }

    return CMD_SUCCESS;
}

DEFUN(ip_ospf_auth_md5,
      ip_ospf_auth_md5_cmd,
      "ip ospf auth md5 <1-255> PASSWORD",
      "IP Information\n"
      "OSPF interface commands\n"
      "Enable authentication on this interface\n"
      "Use MD5 algorithm\n"
      "Key ID\n"
      "The OSPF password (key)<1-255>")
{
    struct interface *ifp = NULL;
    struct crypt_key *ck;
    u_char key_id;
    struct ospf_if_params *params = NULL;
    //char plaintext_key[512];
    uint32_t ifindex = (uint32_t)vty->index;
    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    if (strlen(argv[1]) > 255)
    {
        vty_error_out(vty, "OSPF auth PASSWORD must be shorter than 255%s",
                VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*delete all md5 key*/
    ospf_auth_md5_delete_delete_all(params);
    SET_IF_PARAM(params, auth_type);
    params->auth_type = OSPF_AUTH_CRYPTOGRAPHIC;
    key_id = strtol(argv[0], NULL, 10);

    if (ospf_crypt_key_lookup(params->auth_crypt, key_id) != NULL)
    {
        return CMD_SUCCESS;
    }

    ck = ospf_crypt_key_new();
    ck->key_id = (u_char) key_id;
    memset(ck->auth_key, 0, OSPF_AUTH_MD5_SIZE + 1);
    strncpy((char *) ck->auth_key, argv[1], OSPF_AUTH_MD5_SIZE);
    ck->auth_key_len = strlen(argv[1]);

    if (ck->auth_key_len < OSPF_MD5_SIZE)
    {
        ck->auth_key_len = OSPF_MD5_SIZE;
    }

    ospf_crypt_key_add(params->auth_crypt, ck);
    SET_IF_PARAM(params, auth_crypt);
    return CMD_SUCCESS;
}

DEFUN(no_ip_ospf_auth_md5,
      no_ip_ospf_auth_md5_cmd,
      "no ip ospf auth md5 <1-255>",
      NO_STR
      "IP Information\n"
      "OSPF interface commands\n"
      "Disable authentication on this interface\n"
      "Use MD5 algorithm\n"
      "Key ID\n")
{
    struct interface *ifp = NULL;
    struct crypt_key *ck = NULL;
    int key_id;
    struct in_addr addr;
    struct ospf_if_params *params = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    params->auth_type = OSPF_AUTH_NOTSET;
    UNSET_IF_PARAM(params, auth_type);
    key_id = strtol(argv[0], NULL, 10);
    ck = ospf_crypt_key_lookup(params->auth_crypt, key_id);

    if (ck == NULL)
    {
        vty_error_out(vty, "Key %d does not exist%s", key_id, VTY_NEWLINE);
        return CMD_WARNING;
    }

    ospf_crypt_key_delete(params->auth_crypt, key_id);

    if (params != IF_DEF_PARAMS(ifp))
    {
        ospf_free_if_params(ifp, addr);
        ospf_if_update_params(ifp, addr);
    }

    return CMD_SUCCESS;
}


DEFUN(ip_ospf_cost,
      ip_ospf_cost_u32_cmd,
      "ip ospf cost <1-65535>",
      "IP Information\n"
      "OSPF interface commands\n"
      "Interface cost\n"
      "Cost")
{
    struct interface *ifp = NULL;
    u_int32_t cost;
    struct ospf_if_params *params = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    cost = strtol(argv[0], NULL, 10);

    /* cost range is <1-65535>. */
    if (cost < 1 || cost > 65535)
    {
        vty_error_out(vty, "Interface output cost is invalid%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    SET_IF_PARAM(params, output_cost_cmd);
    params->output_cost_cmd = cost;
    ospf_if_recalculate_output_cost(ifp);
    return CMD_SUCCESS;
}


ALIAS(ip_ospf_cost,
	ip_ospf_cost_u32_h3c_cmd,
	"ospf cost <1-65535>",
	"OSPF interface commands\n"
	"Interface cost\n"
	"Cost value\n")


DEFUN(no_ip_ospf_cost,
      no_ip_ospf_cost_cmd,
      "no ip ospf cost",
      NO_STR
      "IP Information\n"
      "OSPF interface commands\n"
      "Interface cost\n")
{
    struct interface *ifp = NULL;
    struct in_addr addr;
    struct ospf_if_params *params = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    UNSET_IF_PARAM(params, output_cost_cmd);

    if (params != IF_DEF_PARAMS(ifp))
    {
        ospf_free_if_params(ifp, addr);
        ospf_if_update_params(ifp, addr);
    }

    ospf_if_recalculate_output_cost(ifp);
    return CMD_SUCCESS;
}


ALIAS(no_ip_ospf_cost,
	no_ip_ospf_cost_h3c_cmd,
	"undo ospf cost",
	"Cancel the current setting\n"
	"OSPF interface commands\n"
	"Interface cost\n")


static void
ospf_nbr_timer_update(struct ospf_interface *oi)
{
    struct route_node *rn = NULL;
    struct ospf_neighbor *nbr = NULL;

    for (rn = route_top(oi->nbrs); rn; rn = route_next(rn))
    {
        if ((nbr = rn->info))
        {
            nbr->v_inactivity = OSPF_IF_PARAM(oi, v_wait);
            nbr->v_db_desc = OSPF_IF_PARAM(oi, retransmit_interval);
            nbr->v_ls_req = OSPF_IF_PARAM(oi, retransmit_interval);
            nbr->v_ls_upd = OSPF_IF_PARAM(oi, retransmit_interval);

            if (nbr != oi->nbr_self)
                ospf_inactive_timer_restart(nbr);
        }
    }
}

static int
ospf_vty_dead_interval_set(struct vty *vty, const char *interval_str,
                           const char *nbr_str,
                           const char *fast_hello_str)
{
    struct ospf *ospf;
    struct interface *ifp = NULL;
    u_int32_t seconds;
    u_char hellomult;
    struct in_addr addr;
    int ret;
    struct ospf_if_params *params = NULL;
    struct ospf_interface *oi = NULL;
    struct route_node *rn = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    u_int16_t ospf_id;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ospf_id = OSPF_DEFAULT_PROCESS_ID;

    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    if (nbr_str)
    {
        ret = inet_aton(nbr_str, &addr);

        if (!ret)
        {
            vty_error_out(vty, "Please specify interface address by A.B.C.D%s",
                    VTY_NEWLINE);
            return CMD_WARNING;
        }

        params = ospf_get_if_params(ifp, addr);
        ospf_if_update_params(ifp, addr);
    }

    if (interval_str)
    {
        /*VTY_GET_INTEGER_RANGE("Router Dead Interval", seconds, interval_str,
                              1, 65535);*/
        //Modify by zzl for adapt h3c command parameter range
		VTY_GET_INTEGER_RANGE("Router Dead Interval", seconds, interval_str,
                              1, 2147483647);

        if (params->v_hello >= seconds)
        {
            vty_error_out(vty, "The dead interval must greater than hello interval%s",
                    VTY_NEWLINE);
            return CMD_WARNING;
        }

        /* reset fast_hello too, just to be sure */
        UNSET_IF_PARAM(params, fast_hello);
        params->fast_hello = OSPF_FAST_HELLO_DEFAULT;
    }
    else if (fast_hello_str)
    {
        VTY_GET_INTEGER_RANGE("Hello Multiplier", hellomult, fast_hello_str,
                              1, 10);
        /* 1s dead-interval with sub-second hellos desired */
        seconds = OSPF_ROUTER_DEAD_INTERVAL_MINIMAL;
        SET_IF_PARAM(params, fast_hello);
        params->fast_hello = hellomult;
    }
    else
    {
        vty_warning_out(vty, "Please specify dead-interval or hello-multiplier%s",
                VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (params->v_wait == seconds)
    {
        return CMD_SUCCESS;
    }

    SET_IF_PARAM(params, v_wait);
    params->v_wait = seconds;

    /* Update timer values in neighbor structure. */
    if (nbr_str)
    {
        if ((ospf = ospf_lookup_id(ospf_id)))
        {
            oi = ospf_if_lookup_by_local_addr(ospf, ifp, addr);

            if (oi)
            {
                ospf_nbr_timer_update(oi);
            }
        }
    }
    else
    {
        for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
        {
            if ((oi = rn->info))
            {
                ospf_nbr_timer_update(oi);
            }
        }
    }

    return CMD_SUCCESS;
}

static void ospf_interface_init(uint32_t ifindex, vrf_id_t vrf_id)
{
    int    ret = 0;
    struct ifm_l3  l3 = {0};
    struct ifm_l3  l3_unnum = {0};
    struct ifm_event pevent;
    memset(&pevent, 0, sizeof(struct ifm_event));
	uint8_t pflag = 1;
    uint8_t pdown_flag = 0;
    char ip[INET_ADDRSTRLEN] = "";
	int num = 5;
	
    /* get link */
    /*
	while(num > 0)
	{
    	pdown_flag = ifm_get_link(ifindex, MODULE_ID_OSPF);
		if(NULL != pdown_flag)
			break;
		usleep(5000);
		num --;
	}
	*/
    if (ifm_get_link(ifindex, MODULE_ID_OSPF, &pdown_flag) != 0)
    {
        zlog_err("%-15s[%d]: OSPF get ifindex %0x link status time out", __FUNCTION__, __LINE__, ifindex);
        pflag = IFNET_LINKDOWN;
    }
    else
    {
        pflag = pdown_flag;
        zlog_debug(OSPF_DBG_OTHER, "%-15s[%d]: OSPF get link ifindex %0x %d", __FUNCTION__, __LINE__, ifindex, pevent.up_flag);
    }
	
	num = 5;
	while(num > 0)
	{
    	ret = ifm_get_l3if(ifindex, MODULE_ID_OSPF, &l3);
        
		if(ret == 0)	break;
		
		usleep(5000);
		num --;
	}

    if (ret == 0)
    {
        if (l3.ipv4_flag == IP_TYPE_UNNUMBERED)
        {
			//if(l3_unnum == NULL)
		 	//{
		 	//	zlog_err("%-15s[%d]: OSPF get ifindex %0x l3 time out", __FUNCTION__, __LINE__, l3.unnumbered_if);
		 	//}

            if (ifm_get_l3if(l3.unnumbered_if, MODULE_ID_OSPF, &l3_unnum) == 0)
            {

                if (l3_unnum.ipv4[0].addr != 0)
                {
                    pevent.ifindex = ifindex;
                    pevent.mode = IFNET_MODE_L3;
                    pevent.ipaddr.type = INET_FAMILY_IPV4;
                    pevent.ipaddr.addr.ipv4 = l3_unnum.ipv4[0].addr;
                    pevent.ipaddr.prefixlen = l3_unnum.ipv4[0].prefixlen;
                    pevent.ipflag = l3.ipv4_flag;
                    pevent.up_flag = pflag;
					pevent.vpn= l3.vpn;
                    zlog_debug(OSPF_DBG_OTHER, "%-15s[%d]: OSPF get ip  ifindex %0x:[%s],mask :%d", __func__, __LINE__, ifindex, inet_ipv4tostr(l3_unnum.ipv4[0].addr, ip) , \
                               l3_unnum.ipv4[0].prefixlen);
                    ospf_interface_address_add(pevent, vrf_id);

                    if (l3_unnum.ipv4[1].addr != 0)
                    {
                        memset(&pevent, 0, sizeof(struct ifm_event));
                        pevent.ifindex = ifindex;
                        pevent.ipaddr.type = INET_FAMILY_IPV4;
                        pevent.ipaddr.addr.ipv4 = l3_unnum.ipv4[1].addr;
                        pevent.ipaddr.prefixlen = l3_unnum.ipv4[1].prefixlen;
                        pevent.ipflag = IP_TYPE_SLAVE;
                        pevent.up_flag = pflag;
						pevent.vpn= l3.vpn;
                        ospf_interface_address_add(pevent, vrf_id);
                    }
                }
            }
        }
        else
        {

            if (l3.ipv4[0].addr != 0)
            {
                pevent.ifindex = ifindex;
                pevent.mode = IFNET_MODE_L3;
                pevent.ipaddr.type = INET_FAMILY_IPV4;
                pevent.ipaddr.addr.ipv4 = l3.ipv4[0].addr;
                pevent.ipaddr.prefixlen = l3.ipv4[0].prefixlen;
                pevent.ipflag = l3.ipv4_flag;
				pevent.up_flag = pflag;
				pevent.vpn= l3.vpn;
                zlog_debug(OSPF_DBG_OTHER, "%-15s[%d]: OSPF get ip  ifindex %0x:[%s],mask :%d", __func__, __LINE__, ifindex, inet_ipv4tostr(l3.ipv4[0].addr, ip) , \
                           l3.ipv4[0].prefixlen);
                ospf_interface_address_add(pevent, vrf_id);

                if (l3.ipv4[1].addr != 0)
                {
                    memset(&pevent, 0, sizeof(struct ifm_event));
                    pevent.ifindex = ifindex;
                    pevent.ipaddr.type = INET_FAMILY_IPV4;
                    pevent.ipaddr.addr.ipv4 = l3.ipv4[1].addr;
                    pevent.ipaddr.prefixlen = l3.ipv4[1].prefixlen;
                    pevent.ipflag = IP_TYPE_SLAVE;
                    pevent.up_flag = pflag;
					pevent.vpn= l3.vpn;
                    ospf_interface_address_add(pevent, vrf_id);
                }
            }
        }

    }
    else
    {
        if (pflag  == IFNET_LINKDOWN)
        {
            ospf_interface_state_down(pevent, vrf_id);
        }
        else
        {
            ospf_interface_state_up(pevent, vrf_id);
        }

        zlog_warn("%-15s[%d]: OSPF get ifindex %0x ip error", __func__, __LINE__, ifindex);
    }
}

static struct interface *
ospf_enable_interface_init(uint32_t ifindex)
{
    struct interface *ifp = NULL;
    vrf_id_t vrf_id = VRF_DEFAULT;

    /*according to the interface name look up the struct pointer ifp */
    ifp = if_lookup_by_index_vrf(ifindex, vrf_id);

    if (ifp)/*if the if exit,first must delete the address*/
    {
        //struct ifm_event pevent;
        //pevent.ifindex = ifindex;
        struct connected *ifc = NULL;
        struct listnode *node = NULL;
        struct listnode *next = NULL;

        for (node = listhead(ifp->connected); node; node = next)
        {
            ifc = listgetdata(node);
            next = node->next;

            if (ifc != NULL)
            {
                connected_delete_by_prefix(ifp, ifc->address);
            }
        }
    }

    /*if the interface exist,reget the ifp*/
    ospf_interface_init(ifindex, vrf_id);

    if (ifp == NULL)
    {
        ifp = if_lookup_by_index_vrf(ifindex, vrf_id);
    }

    return ifp;
}


DEFUN(ip_ospf_dead_interval,
      ip_ospf_dead_interval_cmd,
      "ip ospf dead-interval <1-65535>",
      "IP Information\n"
      "OSPF interface commands\n"
      "Interval after which a neighbor is declared dead\n"
      "Seconds\n")
{
    return ospf_vty_dead_interval_set(vty, argv[0], NULL, NULL);
}


ALIAS(ip_ospf_dead_interval,
	ip_ospf_dead_interval_h3c_cmd,
	"ospf timer dead <1-2147483647>",
	"OSPF interface commands\n"
	"Specify timer interval\n"
	"Specify the interval after which a neighbor is declared dead\n"
	"Seconds\n")


DEFUN(ip_ospf_dead_interval_minimal,
      ip_ospf_dead_interval_minimal_addr_cmd,
      "ip ospf dead-interval minimal hello-multiplier <1-10> A.B.C.D",
      "IP Information\n"
      "OSPF interface commands\n"
      "Interval after which a neighbor is declared dead\n"
      "Minimal 1s dead-interval with fast sub-second hellos\n"
      "Hello multiplier factor\n"
      "Number of Hellos to send each second\n"
      "Address of interface\n")
{
    if (argc == 2)
    {
        return ospf_vty_dead_interval_set(vty, NULL, argv[1], argv[0]);
    }
    else
    {
        return ospf_vty_dead_interval_set(vty, NULL, NULL, argv[0]);
    }
}

ALIAS(ip_ospf_dead_interval_minimal,
      ip_ospf_dead_interval_minimal_cmd,
      "ip ospf dead-interval minimal hello-multiplier <1-10>",
      "IP Information\n"
      "OSPF interface commands\n"
      "Interval after which a neighbor is declared dead\n"
      "Minimal 1s dead-interval with fast sub-second hellos\n"
      "Hello multiplier factor\n"
      "Number of Hellos to send each second\n")

DEFUN(no_ip_ospf_dead_interval,
      no_ip_ospf_dead_interval_cmd,
      "no ip ospf dead-interval",
      NO_STR
      "IP Information\n"
      "OSPF interface commands\n"
      "Interval after which a neighbor is declared dead\n")
{
    struct interface *ifp = NULL;
    struct in_addr addr;
    struct ospf_if_params *params = NULL;
    struct ospf_interface *oi = NULL;
    struct route_node *rn = NULL;
    uint32_t ifindex = (uint32_t)vty->index;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);
	
	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    UNSET_IF_PARAM(params, v_wait);

    if (OSPF_IF_PARAM_CONFIGURED(params, v_hello) &&
            params->v_hello == OSPF_HELLO_INTERVAL_DEFAULT)
    {
        if ((params->type == OSPF_IFTYPE_BROADCAST) || (params->type == OSPF_IFTYPE_POINTOPOINT))
        {
            params->v_wait = OSPF_ROUTER_DEAD_INTERVAL_DEFAULT;
        }
        else
        {
            params->v_wait = OSPF_ROUTER_DEAD_INTERVAL_NBMA_DEFAULT;
        }
    }
    else
    {
        params->v_wait = 4 * params->v_hello;
    }

    UNSET_IF_PARAM(params, fast_hello);
    params->fast_hello = OSPF_FAST_HELLO_DEFAULT;

    if (params != IF_DEF_PARAMS(ifp))
    {
        ospf_free_if_params(ifp, addr);
        ospf_if_update_params(ifp, addr);
    }

    /* Update timer values in neighbor structure. */
    for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
    {
        if ((oi = rn->info))
        {
            ospf_nbr_timer_update(oi);
        }
    }

    return CMD_SUCCESS;
}


ALIAS(no_ip_ospf_dead_interval,
	no_ip_ospf_dead_interval_h3c_cmd,
	"undo ospf timer dead",
	"Cancel the current setting\n"
	"OSPF interface commands\n"
	"Specify timer interval\n"
	"Interval after which a neighbor is declared dead\n")


DEFUN(ip_ospf_hello_interval,
      ip_ospf_hello_interval_cmd,
      "ip ospf hello-interval <1-65535>",
      "IP Information\n"
      "OSPF interface commands\n"
      "Time between HELLO packets\n"
      "Seconds\n")
{
    struct interface *ifp = NULL;
    u_int32_t seconds;
    struct ospf_if_params *params = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    struct ospf_interface *oi = NULL;
    struct route_node *rn = NULL;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    seconds = strtol(argv[0], NULL, 10);

    /* HelloInterval range is <1-65535>. */
    if (seconds < 1 || seconds > 65535)
    {
        vty_error_out(vty, "Hello Interval is invalid%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    SET_IF_PARAM(params, v_hello);
    /* Then change the hello interval */
    params->v_hello = seconds;

    /* first adjust dead interval */
    if (OSPF_IF_PARAM_CONFIGURED(params, v_wait))  /*config the dead interval*/
    {
        if (IS_DEBUG_OSPF_EVENT)
        {
            zlog_debug(OSPF_DBG_EVENT, "%s %d dead interval is configed \n", __func__, __LINE__);
        }

        if (seconds >= params->v_wait)  /*hello interval is bigger than dead interval*/
        {

            if (IS_DEBUG_OSPF_EVENT)
            {
                zlog_debug(OSPF_DBG_EVENT, "%s %d dead interval change to 4 times of hello \n", __func__, __LINE__);
            }

            params->v_wait = seconds * 4;

            for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
            {
                if ((oi = rn->info))
                {
                    ospf_nbr_timer_update(oi);

                    OSPF_ISM_TIMER_OFF(oi->t_hello);//update hello timer
                    OSPF_HELLO_TIMER_ON(oi);
                }
            }

            vty_info_out(vty, "Hello interval is greater than Dead interval,"
                    "now Dead interval is forced to adjust to 4 times of new Hello interval %s", VTY_NEWLINE);
        }
        else
        {
            for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
            {
                if ((oi = rn->info))
                {
                    OSPF_ISM_TIMER_OFF(oi->t_hello);//update hello timer
                    OSPF_HELLO_TIMER_ON(oi);
                }
            }
        }
    }
    else /*do not config the dead interval, dead interval adjust to 4 times ospf hello interval*/
    {
        if (IS_DEBUG_OSPF_EVENT)
        {
            zlog_debug(OSPF_DBG_EVENT, "%s %d dead interval is not configed: dead interval change to 4 times of hello \n", __func__, __LINE__);
        }

        params->v_wait = seconds * 4;

        for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
        {

            if ((oi = rn->info))
            {
                ospf_nbr_timer_update(oi);

                OSPF_ISM_TIMER_OFF(oi->t_hello);//update hello timer
                OSPF_HELLO_TIMER_ON(oi);
            }
        }

    }

    return CMD_SUCCESS;
}


ALIAS(ip_ospf_hello_interval,
	ip_ospf_hello_interval_h3c_cmd,
	"ospf timer hello <1-65535>",
	"OSPF interface commands\n"
	"Specify timer interval\n"
	"Specify the interval at which the interface sends hello packets\n"
	"Seconds\n")


DEFUN(no_ip_ospf_hello_interval,
      no_ip_ospf_hello_interval_cmd,
      "no ip ospf hello-interval",
      NO_STR
      "IP Information\n"
      "OSPF interface commands\n"
      "Time between HELLO packets\n")
{
    struct interface *ifp = NULL;
    struct in_addr addr;
    struct ospf_if_params *params = NULL;
    struct ospf_interface *oi = NULL;
    struct route_node *rn = NULL;
    uint32_t ifindex = (uint32_t)vty->index;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    /* First ,If the dead interval is not configured ,adjust the dead interval into default values*/
    if (!OSPF_IF_PARAM_CONFIGURED(params, v_wait))
    {
        if (IS_DEBUG_OSPF_EVENT)
        {
            zlog_debug(OSPF_DBG_EVENT, "%s %d dead interval is configed \n", __func__, __LINE__);
        }

        //SET_IF_PARAM (params, v_wait);
        if ((params->type == OSPF_IFTYPE_BROADCAST) || (params->type == OSPF_IFTYPE_POINTOPOINT))
        {
            params->v_wait = OSPF_ROUTER_DEAD_INTERVAL_DEFAULT;
        }
        else
        {
            params->v_wait = OSPF_ROUTER_DEAD_INTERVAL_NBMA_DEFAULT;
        }

        for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
        {
            if ((oi = rn->info))
            {
                ospf_nbr_timer_update(oi);
            }
        }
    }
	else
	{
		if ((params->type == OSPF_IFTYPE_BROADCAST) || (params->type == OSPF_IFTYPE_POINTOPOINT))
        {
            if(params->v_wait == OSPF_ROUTER_DEAD_INTERVAL_DEFAULT)
           	{
				UNSET_IF_PARAM (params, v_wait);
			}
        }
        else
        {
            if(params->v_wait == OSPF_ROUTER_DEAD_INTERVAL_NBMA_DEFAULT)
            {
				UNSET_IF_PARAM (params, v_wait);
			}
        }
	}

    /* Then, change the hello interval into default values */
    UNSET_IF_PARAM(params, v_hello);

    if ((params->type == OSPF_IFTYPE_BROADCAST) || (params->type == OSPF_IFTYPE_POINTOPOINT))
    {
        params->v_hello = OSPF_HELLO_INTERVAL_DEFAULT;
    }
    else
    {
        params->v_hello = OSPF_HELLO_INTERVAL_NBMA_DEFAULT;
    }

    for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
    {
        if ((oi = rn->info))
        {
            OSPF_ISM_TIMER_OFF(oi->t_hello);//update hello timer
            OSPF_HELLO_TIMER_ON(oi);
        }
    }

    if (params != IF_DEF_PARAMS(ifp))
    {
        ospf_free_if_params(ifp, addr);
        ospf_if_update_params(ifp, addr);
    }

    return CMD_SUCCESS;
}


ALIAS(no_ip_ospf_hello_interval,
	no_ip_ospf_hello_interval_h3c_cmd,
	"undo ospf timer hello",
	"Cancel the current setting\n"
	"OSPF interface commands\n"
	"Specify timer interval\n"
	"Specify the interval at which the interface sends hello packets\n")


static
void ospf_nbma_hello_change(struct interface *ifp, int old_type)
{
    int config = 0;
    struct ospf_if_params *params = NULL;
    params = IF_DEF_PARAMS(ifp);
    int new_type = IF_DEF_PARAMS(ifp)->type;

	if(params == NULL)
	{
		zlog_warn("%s %d ifp params is NULL",__func__,__LINE__);
		return ;
	}

    if ((new_type == OSPF_IFTYPE_NBMA || new_type == OSPF_IFTYPE_POINTOMULTIPOINT)
            && (old_type == OSPF_IFTYPE_POINTOPOINT || old_type == OSPF_IFTYPE_BROADCAST))
    {
        if (params->v_hello == OSPF_HELLO_INTERVAL_DEFAULT
                && params->v_wait == OSPF_ROUTER_DEAD_INTERVAL_DEFAULT)
        {
            config = 1;
        }
        else
        {
            config = 2;
        }
    }
    else if ((new_type == OSPF_IFTYPE_POINTOPOINT || new_type == OSPF_IFTYPE_BROADCAST)
             && (old_type == OSPF_IFTYPE_NBMA || old_type == OSPF_IFTYPE_POINTOMULTIPOINT))
    {
        if (params->v_hello ==  OSPF_HELLO_INTERVAL_NBMA_DEFAULT
                && params->v_wait == OSPF_ROUTER_DEAD_INTERVAL_NBMA_DEFAULT)
        {
            config = 3;
        }
        else
        {
            config = 4;
        }
    }

    /* nbma set default hello value 30s,*/
    if (config == 1)
    {
        SET_IF_PARAM(params, v_hello);
        params->v_hello = OSPF_HELLO_INTERVAL_NBMA_DEFAULT;
        //SET_IF_PARAM(params, v_wait);
        params->v_wait = OSPF_ROUTER_DEAD_INTERVAL_NBMA_DEFAULT;
    }
    else if (config == 3)
    {
        SET_IF_PARAM(params, v_hello);
        params->v_hello = OSPF_HELLO_INTERVAL_DEFAULT;
        //SET_IF_PARAM(params, v_wait);
        params->v_wait = OSPF_ROUTER_DEAD_INTERVAL_DEFAULT;
    }
}


DEFUN(ip_ospf_network,
      ip_ospf_network_cmd,
      "ip ospf network (broadcast|nbma|p2mp|p2p)",
      "IP Information\n"
      "OSPF interface commands\n"
      "Network type\n"
      "Specify OSPF broadcast multi-access network\n"
      "Specify OSPF NBMA network\n"
      "Specify OSPF point-to-multipoint network\n"
      "Specify OSPF point-to-point network\n")
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct route_node *rn = NULL;
    struct interface *ifp = NULL;
    struct ospf_if_params *params = NULL;
    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    /* set the network type */
    int old_type = IF_DEF_PARAMS(ifp)->type;

    if (strncmp(argv[0], "b", 1) == 0)
    {
        IF_DEF_PARAMS(ifp)->type = OSPF_IFTYPE_BROADCAST;
    }
    else if (strncmp(argv[0], "n", 1) == 0)
    {
        IF_DEF_PARAMS(ifp)->type = OSPF_IFTYPE_NBMA;
    }
    else if (strncmp(argv[0], "p2mp", 4) == 0)
    {
        IF_DEF_PARAMS(ifp)->type = OSPF_IFTYPE_POINTOMULTIPOINT;
    }
    else if (strncmp(argv[0], "p2p", 3) == 0)
    {
        IF_DEF_PARAMS(ifp)->type = OSPF_IFTYPE_POINTOPOINT;
    }

    /* enable ospf,can send hello packet */
    if (OSPF_IF_PASSIVE == params->passive_interface)
    {
        ospf_passive_interface_update(ifp, params, OSPF_IF_ACTIVE);
        ifp = ospf_enable_interface_init(ifindex);
    }

    if (IF_DEF_PARAMS(ifp)->type == old_type)
    {
        for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
        {
            struct ospf_interface *oi = rn->info;

            if (!oi)
            {
                continue;
            }

            ospf_external_lsa_fw_update(oi->ospf);
        }

        return CMD_SUCCESS;
    }

    /* set the hello interval into default */
    ospf_nbma_hello_change(ifp, old_type);
    SET_IF_PARAM(IF_DEF_PARAMS(ifp), type);

    for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
    {
        struct ospf_interface *oi = rn->info;
        u_char orig_ism_state;

        if (!oi)
        {
            continue;
        }


        orig_ism_state = oi->state;
        OSPF_ISM_EVENT_EXECUTE(oi, ISM_InterfaceDown);


        oi->type = IF_DEF_PARAMS(ifp)->type;

        if (orig_ism_state > ISM_Down)
        {
            OSPF_ISM_EVENT_EXECUTE(oi, ISM_InterfaceUp);
        }

        ospf_nbr_timer_update(oi);
        ospf_external_lsa_fw_update(oi->ospf);
    }

    return CMD_SUCCESS;
}


ALIAS(ip_ospf_network,
	ip_ospf_network_h3c_cmd,
	"ospf network-type (broadcast|nbma)",
	"OSPF interface commands\n"
	"Specify OSPF network type\n"
	"Specify OSPF broadcast multi-access network\n"
	"Specify OSPF NBMA network\n")


DEFUN(ip_ospf_network_p2mp_p2p,
	ip_ospf_network_p2mp_h3c_cmd,
	"ospf network-type (p2mp) [unicast]",
	"OSPF interface commands\n"
	"Specify OSPF network type\n"
	"Specify OSPF point-to-multipoint network\n"
	"Specify unicast point-to-multipoint network\n")
{
	uint32_t ifindex = (uint32_t)vty->index;
    struct route_node *rn = NULL;
    struct interface *ifp = NULL;
    struct ospf_if_params *params = NULL;
    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    /* set the network type */
    int old_type = IF_DEF_PARAMS(ifp)->type;

	if (strncmp(argv[0], "p2mp", 4) == 0)
    {
        IF_DEF_PARAMS(ifp)->type = OSPF_IFTYPE_POINTOMULTIPOINT;
		if(argv[1] != NULL)
		{
			//printf("func:%s line:%d argv[1] = %s\n", __FUNCTION__, __LINE__, argv[1]);
		}
    }
    else if (strncmp(argv[0], "p2p", 3) == 0)
    {
        IF_DEF_PARAMS(ifp)->type = OSPF_IFTYPE_POINTOPOINT;
		if(argv[1] != NULL)
		{
			//printf("func:%s line:%d argv[1] = %s\n", __FUNCTION__, __LINE__, argv[1]);
		}
    }

    /* enable ospf,can send hello packet */
    if (OSPF_IF_PASSIVE == params->passive_interface)
    {
        ospf_passive_interface_update(ifp, params, OSPF_IF_ACTIVE);
        ifp = ospf_enable_interface_init(ifindex);
    }

    if (IF_DEF_PARAMS(ifp)->type == old_type)
    {
        for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
        {
            struct ospf_interface *oi = rn->info;

            if (!oi)
            {
                continue;
            }

            ospf_external_lsa_fw_update(oi->ospf);
        }

        return CMD_SUCCESS;
    }

    /* set the hello interval into default */
    ospf_nbma_hello_change(ifp, old_type);
    SET_IF_PARAM(IF_DEF_PARAMS(ifp), type);

    for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
    {
        struct ospf_interface *oi = rn->info;
        u_char orig_ism_state;

        if (!oi)
        {
            continue;
        }


        orig_ism_state = oi->state;
        OSPF_ISM_EVENT_EXECUTE(oi, ISM_InterfaceDown);


        oi->type = IF_DEF_PARAMS(ifp)->type;

        if (orig_ism_state > ISM_Down)
        {
            OSPF_ISM_EVENT_EXECUTE(oi, ISM_InterfaceUp);
        }

        ospf_nbr_timer_update(oi);
        ospf_external_lsa_fw_update(oi->ospf);
    }

    return CMD_SUCCESS;
}


ALIAS(ip_ospf_network_p2mp_p2p,
	ip_ospf_network_p2p_h3c_cmd,
	"ospf network-type (p2p) [peer-address-check]",
	"OSPF interface commands\n"
	"Specify OSPF network type\n"
	"Specify OSPF point-to-point network\n"
	"Specify check for nbr on the same network\n")


	
DEFUN(no_ip_ospf_network,
      no_ip_ospf_network_cmd,
      "no ip ospf network",
      NO_STR
      "IP Information\n"
      "OSPF interface commands\n"
      "Network type\n")
{
    struct interface *ifp = NULL;
    struct route_node *rn = NULL;
    struct ospf_if_params *params = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);

    if (ifp == NULL)
    {
        vty_error_out(vty, "Cannot find the interface.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }
    

    int old_type = IF_DEF_PARAMS(ifp)->type;
    IF_DEF_PARAMS(ifp)->type = ospf_default_iftype(ifp);

    /*enable ospf,can send hello packet*/
    if (params != NULL)
    {
        ospf_passive_interface_update(ifp, params, OSPF_IF_PASSIVE);
    }

    if (IF_DEF_PARAMS(ifp)->type == old_type)
    {
        for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
        {
            struct ospf_interface *oi = rn->info;

            if (!oi)
            {
                continue;
            }

            ospf_external_lsa_fw_update(oi->ospf);
        }

        return CMD_SUCCESS;
    }

    ospf_nbma_hello_change(ifp, old_type);

    for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
    {
        struct ospf_interface *oi = rn->info;
        u_char orig_ism_state;

        if (!oi)
        {
            continue;
        }

        orig_ism_state = oi->state;
        OSPF_ISM_EVENT_EXECUTE(oi, ISM_InterfaceDown);


        oi->type = IF_DEF_PARAMS(ifp)->type;

        if (orig_ism_state > ISM_Down)
        {
            OSPF_ISM_EVENT_EXECUTE(oi, ISM_InterfaceUp);
        }

        ospf_nbr_timer_update(oi);
        ospf_external_lsa_fw_update(oi->ospf);
    }

    return CMD_SUCCESS;
}


ALIAS(no_ip_ospf_network,
	no_ip_ospf_network_h3c_cmd,
	"undo ospf network-type",
	"Cancel the current setting\n"
	"OSPF interface commands\n"
	"Specify OSPF network type\n")

	  

DEFUN(ip_ospf_priority,
      ip_ospf_priority_cmd,
      "ip ospf priority <0-255>",
      "IP Information\n"
      "OSPF interface commands\n"
      "Router priority\n"
      "Priority\n")
{
    struct interface *ifp = NULL;
    long priority;
    struct route_node *rn = NULL;
    struct ospf_if_params *params = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    priority = strtol(argv[0], NULL, 10);

    /* Router Priority range is <0-255>. */
    if (priority < 0 || priority > 255)
    {
        vty_error_out(vty, "Router Priority is invalid%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    SET_IF_PARAM(params, priority);
    params->priority = priority;

    for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
    {
        struct ospf_interface *oi = rn->info;

        if (!oi)
        {
            continue;
        }

        if (PRIORITY(oi) != OSPF_IF_PARAM(oi, priority))
        {
            PRIORITY(oi) = OSPF_IF_PARAM(oi, priority);
            OSPF_ISM_EVENT_SCHEDULE(oi, ISM_NeighborChange);
        }
    }

    return CMD_SUCCESS;
}


ALIAS(ip_ospf_priority,
	ip_ospf_priority_h3c_cmd,
	"ospf dr-priority <0-255>",
	"OSPF interface commands\n"
	"Router priority\n"
	"Router Priority value\n")


DEFUN(no_ip_ospf_priority,
      no_ip_ospf_priority_cmd,
      "no ip ospf priority",
      NO_STR
      "IP Information\n"
      "OSPF interface commands\n"
      "Router priority\n")
{
    struct interface *ifp = NULL;
    struct route_node *rn = NULL;
    struct in_addr addr;
    struct ospf_if_params *params = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    UNSET_IF_PARAM(params, priority);
    params->priority = OSPF_ROUTER_PRIORITY_DEFAULT;

    if (params != IF_DEF_PARAMS(ifp))
    {
        ospf_free_if_params(ifp, addr);
        ospf_if_update_params(ifp, addr);
    }

    for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
    {
        struct ospf_interface *oi = rn->info;

        if (!oi)
        {
            continue;
        }

        if (PRIORITY(oi) != OSPF_IF_PARAM(oi, priority))
        {
            PRIORITY(oi) = OSPF_IF_PARAM(oi, priority);
            OSPF_ISM_EVENT_SCHEDULE(oi, ISM_NeighborChange);
        }
    }

    return CMD_SUCCESS;
}


ALIAS(no_ip_ospf_priority,
	no_ip_ospf_priority_h3c_cmd,
	"undo ospf dr-priority",
	"Cancel the current setting\n"
	"OSPF interface commands\n"
	"Router priority\n")


DEFUN(ip_ospf_retransmit_interval,
      ip_ospf_retransmit_interval_cmd,
      "ip ospf retransmit-interval <1-3600>",
      "IP Information\n"
      "OSPF interface commands\n"
      "Time between retransmitting lost link state advertisements\n"
      "Seconds\n")
{
    struct interface *ifp = NULL;
    u_int32_t seconds;
    //struct in_addr addr;
    //int ret;
    struct ospf_if_params *params = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    seconds = strtol(argv[0], NULL, 10);

    /* Retransmit Interval range is <3-65535>. */
    if (seconds < 1 || seconds > 3600)
    {
        vty_error_out(vty, "Retransmit Interval is invalid%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    SET_IF_PARAM(params, retransmit_interval);
    params->retransmit_interval = seconds;
    return CMD_SUCCESS;
}


ALIAS(ip_ospf_retransmit_interval,
	ip_ospf_retransmit_interval_h3c_cmd,
	"ospf timer retransmit <1-3600>",
	"OSPF interface commands\n"
	"Specify timer interval\n"
	"Specify the interval at which the interface retransmits LSAs\n"
	"Seconds\n")


DEFUN(no_ip_ospf_retransmit_interval,
      no_ip_ospf_retransmit_interval_cmd,
      "no ip ospf retransmit-interval",
      NO_STR
      "IP Information\n"
      "OSPF interface commands\n"
      "Time between retransmitting lost link state advertisements\n")
{
    struct interface *ifp = NULL;
    struct in_addr addr;
    //int ret;
    struct ospf_if_params *params = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    UNSET_IF_PARAM(params, retransmit_interval);
    params->retransmit_interval = OSPF_RETRANSMIT_INTERVAL_DEFAULT;

    if (params != IF_DEF_PARAMS(ifp))
    {
        ospf_free_if_params(ifp, addr);
        ospf_if_update_params(ifp, addr);
    }

    return CMD_SUCCESS;
}


ALIAS(no_ip_ospf_retransmit_interval,
	no_ip_ospf_retransmit_interval_h3c_cmd,
	"undo ospf timer retransmit",
	"Cancel the current setting\n"
	"OSPF interface commands\n"
	"Specify timer interval\n"
	"Specify the interval at which the interface retransmits LSAs\n")


DEFUN(ip_ospf_transmit_delay,
      ip_ospf_transmit_delay_cmd,
      "ip ospf transmit-delay <1-3600>",
      "IP Information\n"
      "OSPF interface commands\n"
      "Link state transmit delay\n"
      "Seconds\n")
{
    struct interface *ifp = NULL;
    u_int32_t seconds;
    //struct in_addr addr;
    //int ret;
    struct ospf_if_params *params = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    seconds = strtol(argv[0], NULL, 10);

    /* Transmit Delay range is <1-3600>. */
    if (seconds < 1 || seconds > 3600)
    {
        vty_error_out(vty, "Transmit Delay is invalid%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    SET_IF_PARAM(params, transmit_delay);
    params->transmit_delay = seconds;
    return CMD_SUCCESS;
}


DEFUN(no_ip_ospf_transmit_delay,
      no_ip_ospf_transmit_delay_cmd,
      "no ip ospf transmit-delay",
      NO_STR
      "IP Information\n"
      "OSPF interface commands\n"
      "Link state transmit delay\n")
{
    struct interface *ifp = NULL;
    struct in_addr addr;
    //int ret;
    struct ospf_if_params *params = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);
	
	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    UNSET_IF_PARAM(params, transmit_delay);
    params->transmit_delay = OSPF_TRANSMIT_DELAY_DEFAULT;

    if (params != IF_DEF_PARAMS(ifp))
    {
        ospf_free_if_params(ifp, addr);
        ospf_if_update_params(ifp, addr);
    }

    return CMD_SUCCESS;
}


DEFUN(ip_ospf_area,
      ip_ospf_area_cmd,
      "ip ospf area (A.B.C.D|<0-4294967295>) [A.B.C.D]",
      "IP Information\n"
      "OSPF interface commands\n"
      "Enable OSPF on this interface\n"
      "OSPF area ID in IP address format\n"
      "OSPF area ID as a decimal value\n"
      "Address of interface\n")
{
    struct interface *ifp = NULL;
    struct in_addr area_id;
    struct in_addr addr;
    int format;
    struct ospf_if_params *params = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    VTY_GET_OSPF_AREA_ID(area_id, format, argv[0]);
    OSPF_VTY_GET_IF_PARAMS(ifp, params, 1, addr, VTY_SET);

    if (OSPF_IF_PARAM_CONFIGURED(params, if_area))
    {
        vty_error_out(vty, "There is already an interface area statement.%s",
                VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (memcmp(ifp->name, "VLINK", 5) == 0)
    {
        vty_error_out(vty, "Cannot enable OSPF on a virtual link.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    SET_IF_PARAM(params, if_area);
    params->if_area = area_id;
    //ospf_interface_area_set (ifp);/*ospf_get*/
    return CMD_SUCCESS;
}

DEFUN(no_ip_ospf_area,
      no_ip_ospf_area_cmd,
      "no ip ospf area [A.B.C.D]",
      NO_STR
      "IP Information\n"
      "OSPF interface commands\n"
      "Disable OSPF on this interface\n"
      "Address of interface\n")
{
    struct interface *ifp = NULL;
    struct ospf_if_params *params = NULL;
    struct in_addr addr;
    uint32_t ifindex = (uint32_t)vty->index;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    OSPF_VTY_GET_IF_PARAMS(ifp, params, 0, addr, VTY_UNSET);

    if (!OSPF_IF_PARAM_CONFIGURED(params, if_area))
    {
        return CMD_SUCCESS;
    }

    OSPF_VTY_PARAM_UNSET(params, if_area, ifp, addr);
    ospf_interface_area_unset(ifp);
    return CMD_SUCCESS;
}

/********************************* OSPF LDP Command Line START **********************************************/
DEFUN(ip_ospf_ldp_sync,
      ip_ospf_ldp_sync_cmd,
      "ip ospf ldp-sync",
      "IP Information\n"
      "OSPF interface commands\n"
      "Enable LDP-OSPF synchronization\n")
{
    struct interface *ifp = NULL;
    struct ospf_if_params *params = NULL;
    struct route_node *rn = NULL;
    uint32_t ifindex = (uint32_t)vty->index;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    if (IF_DEF_PARAMS(ifp)->ldp_enable == OSPF_IF_LDP_SYNC_ENABLE)
    {
        return CMD_SUCCESS;
    }

    IF_DEF_PARAMS(ifp)->ldp_enable = OSPF_IF_LDP_SYNC_ENABLE;

    for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
    {
        struct ospf_interface *oi = rn->info;

        if (!oi)
        {
            continue;
        }

        oi->ldp_enable = IF_DEF_PARAMS(ifp)->ldp_enable;
    }

    return CMD_SUCCESS;
}

DEFUN(no_ip_ospf_ldp_sync,
      no_ip_ospf_ldp_sync_cmd,
      "no ip ospf ldp-sync",
      NO_STR
      "IP Information\n"
      "OSPF interface commands\n"
      "Enable LDP-OSPF synchronization\n")
{
    struct interface *ifp = NULL;
    struct ospf_if_params *params = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    struct route_node *rn = NULL;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    if (IF_DEF_PARAMS(ifp)->ldp_enable == OSPF_IF_LDP_SYNC_DISABLE)
    {
        return CMD_SUCCESS;
    }

    IF_DEF_PARAMS(ifp)->ldp_enable = OSPF_IF_LDP_SYNC_DISABLE;

    for (rn = route_top(IF_OIFS(ifp)); rn; rn = route_next(rn))
    {
        struct ospf_interface *oi = rn->info;

        if (!oi)
        {
            continue;
        }

        oi->ldp_enable = IF_DEF_PARAMS(ifp)->ldp_enable;
    }

    return CMD_SUCCESS;
}

DEFUN(ip_ospf_ldp_hold_down,
      ip_ospf_ldp_hold_down_cmd,
      "ip ospf ldp-sync hold-down interval <0-65535>",
      "IP Information\n"
      "OSPF interface commands\n"
      "Specify LDP-OSPF synchronization timer interval\n"
      "HoldDown timer\n"
      "HoldDown interval\n"
      "Second(s)\n")
{
    struct interface *ifp = NULL;
    struct ospf_if_params *params = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    u_int32_t seconds;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    seconds = strtol(argv[0], NULL, 10);

    /* range is <0-65535>. */
    if (seconds > 65535)
    {
        vty_error_out(vty, "hold-down interval is invalid%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (params->v_holddown == seconds)
    {
        return CMD_SUCCESS;
    }

    SET_IF_PARAM(params, v_holddown);
    params->v_holddown = seconds;
    return CMD_SUCCESS;
}


DEFUN(no_ip_ospf_ldp_hold_down,
      no_ip_ospf_ldp_hold_down_cmd,
      "no ip ospf ldp-sync hold-down interval",
      NO_STR
      "IP Information\n"
      "OSPF interface commands\n"
      "Specify LDP-OSPF synchronization timer interval\n"
      "HoldDown timer\n"
      "HoldDown interval\n")
{
    struct interface *ifp = NULL;
    struct ospf_if_params *params = NULL;
    uint32_t ifindex = (uint32_t)vty->index;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    UNSET_IF_PARAM(params, v_holddown);
    params->v_holddown = OSPF_SYNC_LDP_HOLD_DOWN_INTERVAL;
    return CMD_SUCCESS;
}

DEFUN(ip_ospf_ldp_max_cost,
      ip_ospf_ldp_max_cost_cmd,
      "ip ospf ldp-sync hold-max-cost interval <0-65535>",
      "IP Information\n"
      "OSPF interface commands\n"
      "Specify LDP-OSPF synchronization timer interval\n"
      "HoldMaxCost timer\n"
      "HoldMaxCost interval\n"
      "Second(s)\n")
{
    struct interface *ifp = NULL;
    struct ospf_if_params *params = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    u_int32_t seconds;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    seconds = strtol(argv[0], NULL, 10);

    /* range is <0-65535>. */
    if (seconds > 65535)
    {
        vty_error_out(vty, "hold-max-cost interval is invalid%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (params->v_maxcost == seconds)
    {
        return CMD_SUCCESS;
    }

    SET_IF_PARAM(params, v_maxcost);
    params->v_maxcost = seconds;
    return CMD_SUCCESS;
}

DEFUN(ip_ospf_ldp_max_cost_infinite,
      ip_ospf_ldp_max_cost_infinite_cmd,
      "ip ospf ldp-sync hold-max-cost interval infinite",
      "IP Information\n"
      "OSPF interface commands\n"
      "Specify LDP-OSPF synchronization timer interval\n"
      "HoldMaxCost timer\n"
      "HoldMaxCost interval\n"
      "Always advertise the maximum cost\n")
{
    struct interface *ifp = NULL;
    struct ospf_if_params *params = NULL;
    uint32_t ifindex = (uint32_t)vty->index;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);
	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    return CMD_SUCCESS;
}



DEFUN(no_ip_ospf_ldp_max_cost,
      no_ip_ospf_ldp_max_cost_cmd,
      "no ip ospf ldp-sync hold-max-cost interval",
      NO_STR
      "IP Information\n"
      "OSPF interface commands\n"
      "Specify LDP-OSPF synchronization timer interval\n"
      "HoldMaxCost timer\n"
      "HoldMaxCost interval\n")
{
    struct interface *ifp = NULL;
    struct ospf_if_params *params = NULL;
    uint32_t ifindex = (uint32_t)vty->index;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR
    }

    UNSET_IF_PARAM(params, v_maxcost);
    params->v_maxcost = OSPF_SYNC_LDP_MAX_COST_INTERVAL;

    return CMD_SUCCESS;
}

/********************************* OSPF LDP Command Line END **********************************************/



DEFUN(ospf_redistribute_direct,
      ospf_redistribute_direct_cmd,
      "redistribute connected route-policy (ethernet|gigabitethernet|xgigabitethernet|loopback) IFNAME",
      REDIST_STR
      "Connected routes (directly attached subnet or host)\n"
      "route policy\n"
      "interface type :ethernet\n"
      "interface type :gigabitethernet\n"
      "interface type :xgigabitethernet\n"
      "interface type :loopback\n"
      "interface name\n"
     )
{
    uint32_t ifindex = 0;
    struct ifm_info pifm = {0};
    struct ospf *ospf = vty->index;
    char if_name[IFNET_NAMESIZE];
    struct interface *ifp = NULL;
    struct ospf_route_redist *ospf_route_redist = NULL;
    int ret = -1;
	
	if(ospf == NULL)
	{
		VTY_OSPF_INSTANCE_ERR
	}
	
    if (! strcmp(argv[0], "ethernet"))
    {
        ifindex = ifm_get_ifindex_by_name((char *)"ethernet", (char *)argv[1]);
    }
    else if (! strcmp(argv[0], "gigabitethernet"))
    {
        ifindex = ifm_get_ifindex_by_name((char *)"gigabitethernet", (char *)argv[1]);
    }
    else if (! strcmp(argv[0], "xgigabitethernet"))
    {
        ifindex = ifm_get_ifindex_by_name((char *)"xgigabitethernet", (char *)argv[1]);
    }
    else if (! strcmp(argv[0], "loopback"))
    {
        ifindex = ifm_get_ifindex_by_name((char *)"loopback", (char *)argv[1]);
    }
    else
    {
        return CMD_WARNING;
    }

    if (ifindex == 0)
    {
        vty_error_out(vty, "Command incomplete ,please check out%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (vty->config_read_flag != 1)
    {
        /*check if the interface exist*/
        if (ifm_get_all_info(ifindex, MODULE_ID_OSPF,&pifm) != 0)
        {
            vty_error_out(vty, "This interface not exist%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    /*get the name of the interface*/
    ret = ifm_get_name_by_ifindex(ifindex , if_name);

    if (ret < 0)
    {
        vty_error_out(vty, "Failed to get interface information.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }


    /* find/create ospf6 interface */
    ifp = if_get_by_name(if_name);

    if (ifp == NULL)
    {
        vty_error_out(vty, "This interface not exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if ((ospf_route_redist = ospf_route_policy_look(ospf, ifindex)) != NULL)
    {
        vty_error_out(vty, "OSPF have already redistribute connect route policy %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ospf_route_policy_get(ospf, ifindex, if_name);

    ospf_redistribute_connect_set(ospf, ROUTE_PROTO_CONNECT, -1, -1, 0);

    return CMD_SUCCESS;
}

DEFUN(no_ospf_redistribute_direct,
      no_ospf_redistribute_direct_cmd,
      "no redistribute connected route-policy (ethernet|gigabitethernet|xgigabitethernet|loopback) IFNAME",
      NO_STR
      REDIST_STR
      "Connected routes (directly attached subnet or host)\n"
      "route policy\n"
      "interface type :ethernet\n"
      "interface type :gigabitethernet\n"
      "interface type :xgigabitethernet\n"
      "interface type :loopback\n"
      "interface name\n"
     )
{
    uint32_t ifindex = 0;
    struct ifm_info pifm = {0};
    struct ospf *ospf = vty->index;
    char if_name[IFNET_NAMESIZE];
    struct interface *ifp = NULL;
    struct ospf_route_redist *ospf_route_redist = NULL;
    int ret = -1;
	
    if(ospf == NULL)
	{
		VTY_OSPF_INSTANCE_ERR
	}

    if (! strcmp(argv[0], "ethernet"))
    {
        ifindex = ifm_get_ifindex_by_name((char *)"ethernet", (char *)argv[1]);
    }
    else if (! strcmp(argv[0], "gigabitethernet"))
    {
        ifindex = ifm_get_ifindex_by_name((char *)"gigabitethernet", (char *)argv[1]);
    }
    else if (! strcmp(argv[0], "xgigabitethernet"))
    {
        ifindex = ifm_get_ifindex_by_name((char *)"xgigabitethernet", (char *)argv[1]);
    }
    else if (! strcmp(argv[0], "loopback"))
    {
        ifindex = ifm_get_ifindex_by_name((char *)"loopback", (char *)argv[1]);
    }
    else
    {
        return CMD_WARNING;
    }

    if (ifindex == 0)
    {
        vty_error_out(vty, "Command incomplete ,please check out%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (vty->config_read_flag != 1)
    {
        /*check if the interface exist*/
        if (ifm_get_all_info(ifindex, MODULE_ID_OSPF,&pifm) != 0)
        {
            vty_error_out(vty, "This interface not exist%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    /*get the name of the interface*/
    ret = ifm_get_name_by_ifindex(ifindex , if_name);

    if (ret < 0)
    {
        vty_error_out(vty, "Failed to get interface information.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }


    /* find/create ospf6 interface */
    ifp = if_get_by_name(if_name);

    if (ifp == NULL)
    {
        vty_error_out(vty, "This interface not exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if ((ospf_route_redist = ospf_route_policy_look(ospf, ifindex)) == NULL)
    {
        vty_error_out(vty, "OSPF do not redistribute connect route policy %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    listnode_delete(ospf->redist_name, ospf_route_redist);
    ospf_route_policy_free(ospf_route_redist);
	ospf_route_redist = NULL;

    ospf_redistribute_connect_unset(ospf, ROUTE_PROTO_CONNECT, 0);
    return CMD_SUCCESS;
}



DEFUN(ospf_redistribute_source,
      ospf_redistribute_source_cmd,
      "redistribute " QUAGGA_REDIST_STR_OSPFD
      " {metric <0-16777214>|metric-type (1|2)}",
      REDIST_STR
      QUAGGA_REDIST_HELP_STR_OSPFD
      "Metric for redistributed routes\n"
      "OSPF default metric\n"
      "OSPF exterior metric type for redistributed routes\n"
      "Set OSPF External Type 1 metrics\n"
      "Set OSPF External Type 2 metrics\n")
{
    struct ospf *ospf = vty->index ;
    int source;
    int type = -1;
    int metric = -1;

    if (ospf->router_id.s_addr == 0)
    {
        vty_error_out(vty, "Please first config router ID.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /* Get distribute source. */
    source = proto_redistnum(AFI_IP, argv[0]);

    if (source < 0)
    {
        return CMD_WARNING;
    }

    /* Get metric value. */
    if (argv[1] != NULL)
    {
        if (!str2metric(argv[1], &metric))
        {
            return CMD_WARNING;
        }
    }

    /* Get metric type. */
    if (argv[2] != NULL)
    {
        if (!str2metric_type(argv[2], &type))
        {
            return CMD_WARNING;
        }
    }


    ospf->route_policy_flag = 1;

    return ospf_redistribute_set(ospf, source, type, metric , 0);
}


DEFUN(ospf_redistribute_source_h3c,
	ospf_redistribute_source_h3c_cmd,
	"import-route (direct|static) {cost <0-16777214>|type (1|2)|tag <0-4294967295>|route-policy STRING|nssa-only}",
	"Import routes from other protocols into OSPF\n"
	"Direct routes\n"
	"Static routes\n"
	"Metric for imported route\n"
	"Value of metric\n"
	"Type value\n"
	"Set OSPF External Type 1 metrics\n"
    "Set OSPF External Type 2 metrics\n"
    "Specify route tag\n"
    "Value of tag\n"
    "Apply the specified routing policy to filter routes\n"
    "Name of the routing policy(string length:<1-63>)\n"
    "Limit redistributed routes to NSSA areas\n")
{
    int source = -1;
    int type = -1;
    int metric = -1;
	int len = 0;
	unsigned long tag_value = 0;
	struct ospf *ospf = vty->index ;

    if (ospf->router_id.s_addr == 0)
    {
        vty_error_out(vty, "Please first config router ID.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /* Get distribute source. */
    source = proto_redistnum(AFI_IP, argv[0]);
    if (source < 0)
    {
    	if (strncmp ((char *)argv[0], "b", 1) == 0)
	 	{
			source = ROUTE_PROTO_EBGP;
	 	}
		else
		{
        	return CMD_WARNING;
		}
    }

    /* Get metric value. */
    if (argv[1] != NULL)
    {
        if (!str2metric(argv[1], &metric))
        {
            return CMD_WARNING;
        }
    }

    /* Get metric type. */
    if (argv[2] != NULL)
    {
        if (!str2metric_type(argv[2], &type))
        {
            return CMD_WARNING;
        }
    }

	/* Get tag value. */
    if (argv[3] != NULL)
    {
        VTY_GET_INTEGER("Value of tag", tag_value, argv[3]);
		tag_value = tag_value;//Only to clear compilation alarms
    }

	/* Get route-policy name. */
    if (argv[4] != NULL)
    {
    	len = strlen(argv[4]);
        if(len < 1 && len > 63)
       	{
			vty_error_out(vty, "The STRING length is not in the range of 1 and 63.%s", VTY_NEWLINE);
        	return CMD_WARNING;
		}
    }

	/* Get nssa-only. */
    /*if (argv[5] != NULL)
    {
        ;
    }*/
	
    ospf->route_policy_flag = 1;
	OSPF_LOG_DEBUG("Get protol type:%s	instance num:%d  metric_type:%d  metric:%d", \
					ospf_redist_string(source), 0, type, metric);

    return ospf_redistribute_set(ospf, source, type, metric , 0);
}


DEFUN(ospf_redistribute_source_h3c_bgp,
	ospf_redistribute_source_h3c_bgp_cmd,
	"import-route (bgp) <1-4294967295> allow-ibgp "
	"{cost <0-16777214>|type (1|2)|tag <0-4294967295>|route-policy STRING|nssa-only}",
	"Import routes from other protocols into OSPF\n"
	"BGP routes\n"
	"Autonomous system number\n"
	"Import Border Gateway Protocol (IBGP)\n"
	"Metric for imported route\n"
	"Value of metric\n"
	"Type value\n"
	"Set OSPF External Type 1 metrics\n"
	"Set OSPF External Type 2 metrics\n"
	"Specify route tag\n"
	"Value of tag\n"
	"Apply the specified routing policy to filter routes\n"
	"Name of the routing policy(string length:<1-63>)\n"
	"Limit redistributed routes to NSSA areas\n")
{
	int source = -1;
    int type = -1;
    int metric = -1;
	int len = 0;
	int instance = 0;
	unsigned long tag_value = 0;
	struct ospf *ospf = vty->index ;

    if (ospf->router_id.s_addr == 0)
    {
        vty_error_out(vty, "Please first config router ID.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /* Get distribute source. */
    source = proto_redistnum(AFI_IP, argv[0]);
    if (source < 0)
    {
    	if (strncmp ((char *)argv[0], "b", 1) == 0)
	 	{
			source = ROUTE_PROTO_IBGP;
	 	}
		else
		{
        	return CMD_WARNING;
		}
    }
	
	/* Get Autonomous system number*/
	if (argv[1] != NULL)
	{
		VTY_GET_INTEGER(" BGP Autonomous system number", instance, argv[1]);
		instance = instance;//Only to clear compilation alarms
		//For adapt h3c can Specify Autonomous system number: instance no use
	}

    /* Get metric value. */
    if (argv[2] != NULL)
    {
        if (!str2metric(argv[2], &metric))
        {
            return CMD_WARNING;
        }
    }

    /* Get metric type. */
    if (argv[3] != NULL)
    {
        if (!str2metric_type(argv[3], &type))
        {
            return CMD_WARNING;
        }
    }

	/* Get tag value. */
    if (argv[4] != NULL)
    {
        VTY_GET_INTEGER("Value of tag", tag_value, argv[4]);
		tag_value = tag_value;//Only to clear compilation alarms
    }

	/* Get route-policy name. */
    if (argv[5] != NULL)
    {
    	len = strlen(argv[5]);
        if(len < 1 && len > 63)
       	{
			vty_error_out(vty, "The STRING length is not in the range of 1 and 63.%s", VTY_NEWLINE);
        	return CMD_WARNING;
		}
		//len = len;//Only to clear compilation alarms
    }

	/* Get nssa-only. */
    /*if (argv[6] != NULL)
    {
        ;
    }*/
	
    ospf->route_policy_flag = 1;
	OSPF_LOG_DEBUG("Get protol type:%s  instance num:%d  metric_type:%d  metric:%d", \
				ospf_redist_string(source), 0, type, metric);
	
    return ospf_redistribute_set(ospf, source, type, metric , 0);
}


DEFUN(ospf_redistribute_source_h3c_bgp_format2,
	ospf_redistribute_source_h3c_bgp_format2_cmd,
	"import-route (bgp) (<1-4294967295> | allow-ibgp) "
	"{cost <0-16777214>|type (1|2)|tag <0-4294967295>|route-policy STRING|nssa-only}",
	"Import routes from other protocols into OSPF\n"
	"BGP routes\n"
	"Autonomous system number\n"
	"Import Border Gateway Protocol (IBGP)\n"
	"Metric for imported route\n"
	"Value of metric\n"
	"Type value\n"
	"Set OSPF External Type 1 metrics\n"
	"Set OSPF External Type 2 metrics\n"
	"Specify route tag\n"
	"Value of tag\n"
	"Apply the specified routing policy to filter routes\n"
	"Name of the routing policy(string length:<1-63>)\n"
	"Limit redistributed routes to NSSA areas\n")
{
	int source = -1;
	int type = -1;
	int metric = -1;
	int len = 0;
	int instance = 0;
	unsigned long tag_value = 0;
	struct ospf *ospf = vty->index ;

	if (ospf->router_id.s_addr == 0)
	{
		vty_error_out(vty, "Please first config router ID.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/* Get distribute source. */
	source = proto_redistnum(AFI_IP, argv[0]);
	if (source < 0)
	{
		if (strncmp ((char *)argv[0], "b", 1) == 0)
		{
			if(strncmp ((char *)argv[1], "all", 3) == 0)
			{
				source = ROUTE_PROTO_IBGP;
			}
			else
			{
				source = ROUTE_PROTO_EBGP;

				/* Get Autonomous system number*/
				VTY_GET_INTEGER(" BGP Autonomous system number", instance, argv[1]);
				instance = instance;//Only to clear compilation alarms
				//For adapt h3c can Specify Autonomous system number: instance no use
			}
		}
		else
		{
			return CMD_WARNING;
		}
	}

	/* Get metric value. */
	if (argv[2] != NULL)
	{
		if (!str2metric(argv[2], &metric))
		{
			return CMD_WARNING;
		}
	}

	/* Get metric type. */
	if (argv[3] != NULL)
	{
		if (!str2metric_type(argv[3], &type))
		{
			return CMD_WARNING;
		}
	}

	/* Get tag value. */
	if (argv[4] != NULL)
	{
		VTY_GET_INTEGER("Value of tag", tag_value, argv[4]);
		tag_value = tag_value;//Only to clear compilation alarms
	}

	/* Get route-policy name. */
	if (argv[5] != NULL)
	{
		len = strlen(argv[5]);
		if(len < 1 && len > 63)
		{
			vty_error_out(vty, "The STRING length is not in the range of 1 and 63.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}

	/* Get nssa-only. */
	/*if (argv[6] != NULL)
	{
		;
	}*/
	
	ospf->route_policy_flag = 1;
	OSPF_LOG_DEBUG("Get protol type:%s	instance num:%d  metric_type:%d  metric:%d", \
				ospf_redist_string(source), 0, type, metric);
	
	return ospf_redistribute_set(ospf, source, type, metric , 0);
}


DEFUN(ospf_redistribute_source_h3c_bgp_format3,
	ospf_redistribute_source_h3c_bgp_format3_cmd,
	"import-route (bgp) "
	"{cost <0-16777214>|type (1|2)|tag <0-4294967295>|route-policy STRING|nssa-only}",
	"Import routes from other protocols into OSPF\n"
	"BGP routes\n"
	"Metric for imported route\n"
	"Value of metric\n"
	"Type value\n"
	"Set OSPF External Type 1 metrics\n"
	"Set OSPF External Type 2 metrics\n"
	"Specify route tag\n"
	"Value of tag\n"
	"Apply the specified routing policy to filter routes\n"
	"Name of the routing policy(string length:<1-63>)\n"
	"Limit redistributed routes to NSSA areas\n")
{
	int source = -1;
	int type = -1;
	int metric = -1;
	int len = 0;
	//int instance = 0;
	unsigned long tag_value = 0;
	struct ospf *ospf = vty->index ;

	if (ospf->router_id.s_addr == 0)
	{
		vty_error_out(vty, "Please first config router ID.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/* Get distribute source. */
	source = proto_redistnum(AFI_IP, argv[0]);
	if (source < 0)
	{
		if (strncmp ((char *)argv[0], "b", 1) == 0)
		{
			source = ROUTE_PROTO_EBGP;
		}
		else
		{
			return CMD_WARNING;
		}
	}

	/* Get metric value. */
	if (argv[1] != NULL)
	{
		if (!str2metric(argv[1], &metric))
		{
			return CMD_WARNING;
		}
	}

	/* Get metric type. */
	if (argv[2] != NULL)
	{
		if (!str2metric_type(argv[2], &type))
		{
			return CMD_WARNING;
		}
	}

	/* Get tag value. */
	if (argv[3] != NULL)
	{
		VTY_GET_INTEGER("Value of tag", tag_value, argv[3]);
		tag_value = tag_value;//Only to clear compilation alarms
	}

	/* Get route-policy name. */
	if (argv[4] != NULL)
	{
		len = strlen(argv[4]);
		if(len < 1 && len > 63)
		{
			vty_error_out(vty, "The STRING length is not in the range of 1 and 63.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}

	/* Get nssa-only. */
	/*if (argv[5] != NULL)
	{
		;
	}*/
	
	ospf->route_policy_flag = 1;
	OSPF_LOG_DEBUG("Get protol type:%s	instance num:%d  metric_type:%d  metric:%d", \
				ospf_redist_string(source), 0, type, metric);
	
	return ospf_redistribute_set(ospf, source, type, metric , 0);
}


DEFUN(ospf_redistribute_ri_source,
      ospf_redistribute_ri_source_cmd,
      "redistribute " QUAGGA_REDIST_STR_OSPFD_RI "<1-255>"
      " {metric <0-16777214>|metric-type (1|2)}",
      REDIST_STR
      QUAGGA_REDIST_HELP_STR_OSPFD_RI
      "instance id\n"
      "Metric for redistributed routes\n"
      "OSPF default metric\n"
      "OSPF exterior metric type for redistributed routes\n"
      "Set OSPF External Type 1 metrics\n"
      "Set OSPF External Type 2 metrics\n")
{
    int type = -1;
    int source = -1;
    int metric = -1;
    int instance = 0;
    struct ospf *ospf = vty->index ;

    if (ospf->router_id.s_addr == 0)
    {
        vty_error_out(vty, "Please first config router ID.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /* Get distribute source. */
    source = proto_redistnum(AFI_IP, argv[0]);

    if (source < 0)
    {
        return CMD_WARNING;
    }

    VTY_GET_INTEGER("redistribute instance", instance, argv[1]);

    if (ROUTE_PROTO_OSPF == source  && instance == ospf->ospf_id)
    {
        vty_error_out(vty, "The instance can not redistribute the routes of its own.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /* Get metric value. */
    if (argv[2] != NULL)
	{
	    if (!str2metric(argv[2], &metric))
	    {
	        return CMD_WARNING;
	    }
	}
    /* Get metric type. */
    if (argv[3] != NULL)
	{
	    if (!str2metric_type(argv[3], &type))
	    {
	        return CMD_WARNING;
	    }
	}

    return ospf_redistribute_set(ospf, source, type, metric, instance);
}


DEFUN(ospf_redistribute_ri_source_h3c,
	ospf_redistribute_ri_source_h3c_cmd,
	"import-route (rip|isis|ospf) (<1-65535> | all-processes) "
	"{cost <0-16777214>|type (1|2)|tag <0-4294967295>|route-policy STRING|nssa-only|allow-direct}",
	"Import routes from other protocols into OSPF\n"
	"RIP routes\n"
	"IS-IS routes\n"
	"OSPF routes\n"
	"Process ID\n"
	"Redistribute all processes\n"
	"Metric for imported route\n"
	"Value of metric\n"
	"Type value\n"
	"Set OSPF External Type 1 metrics\n"
	"Set OSPF External Type 2 metrics\n"
	"Specify route tag\n"
	"Value of tag\n"
	"Apply the specified routing policy to filter routes\n"
	"Name of the routing policy(string length:<1-63>)\n"
	"Limit redistributed routes to NSSA areas\n"
	"Include direct routes\n")
{
	int len = 0;
	int type = -1;
    int source = -1;
    int metric = -1;
    int instance = 0;
	unsigned long tag_value = 0;
    struct ospf *ospf = vty->index ;

    if (ospf->router_id.s_addr == 0)
    {
        vty_error_out(vty, "Please first config router ID.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /* Get distribute source. */
    source = proto_redistnum(AFI_IP, argv[0]);
    if (source < 0)
    {
        if (strncmp ((char *)argv[0], "b", 1) == 0)
	 	{
			source = ROUTE_PROTO_EBGP;
	 	}
		else
		{
        	return CMD_WARNING;
		}
    }

	//Get instance number
	/*
	Note: In order to comply with the command format "all-processes" of H3C, in fact, our ROUTER module
	does not support the import-route of all instances at one time. If "all-processes" is configured, 
	we will only import protocol routes with instance number 1 by default.
	*/
	if(argv[1] != NULL)
	{
		if(strncmp ((char *)argv[1], "all", 3) == 0)
		{
			//For adapt h3c's all-processes parameter 
			instance = 1;
		}
		else
		{
    		VTY_GET_INTEGER_RANGE("import instance", instance, argv[1], 0, OSPF_MAX_PROCESS_ID_H3C);
		}
	}
	else
	{
		instance = 1;
	}
	OSPF_LOG_DEBUG("Get protol type:%s  instance num: %d", ospf_redist_string(source), instance);
	
    if (ROUTE_PROTO_OSPF == source  && instance == ospf->ospf_id)
    {
        vty_error_out(vty, "The instance can not redistribute the routes of its own.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /* Get metric value. */
    if (argv[2] != NULL)
    {
        if (!str2metric(argv[2], &metric))
        {
            return CMD_WARNING;
        }
    }

    /* Get metric type. */
    if (argv[3] != NULL)
    {
        if (!str2metric_type(argv[3], &type))
        {
            return CMD_WARNING;
        }
    }

	/* Get tag value. */
    if (argv[4] != NULL)
    {
        VTY_GET_INTEGER("Value of tag", tag_value, argv[4]);
		tag_value = tag_value;//Only to clear compilation alarms
    }

	/* Get route-policy name. */
    if (argv[5] != NULL)
    {
    	len = strlen(argv[5]);
        if(len < 1 && len > 63)
       	{
			vty_error_out(vty, "The STRING length is not in the range of 1 and 63.%s", VTY_NEWLINE);
        	return CMD_WARNING;
		}
    }

	/* Get nssa-only. */
    /*if (argv[6] != NULL)
    {
        ;
    }*/

	/* Get allow-direct. */
	/*if (argv[7] != NULL)
    {
        ;
    }*/
	OSPF_LOG_DEBUG("Get protol type:%s  instance num:%d  metric_type:%d  metric:%d", \
				ospf_redist_string(source), instance, type, metric);
	
    return ospf_redistribute_set(ospf, source, type, metric, instance);
}


DEFUN(ospf_redistribute_ri_source_h3c_format2,
	ospf_redistribute_ri_source_h3c_format2_cmd,
	"import-route (rip|isis|ospf) "
	"{cost <0-16777214>|type (1|2)|tag <0-4294967295>|route-policy STRING|nssa-only|allow-direct}",
	"Import routes from other protocols into OSPF\n"
	"RIP routes\n"
	"IS-IS routes\n"
	"OSPF routes\n"
	"Metric for imported route\n"
	"Value of metric\n"
	"Type value\n"
	"Set OSPF External Type 1 metrics\n"
	"Set OSPF External Type 2 metrics\n"
	"Specify route tag\n"
	"Value of tag\n"
	"Apply the specified routing policy to filter routes\n"
	"Name of the routing policy(string length:<1-63>)\n"
	"Limit redistributed routes to NSSA areas\n"
	"Include direct routes\n")
{
	int len = 0;
	int type = -1;
	int source = -1;
	int metric = -1;
	int instance = 0;
	unsigned long tag_value = 0;
	struct ospf *ospf = vty->index ;

	if (ospf->router_id.s_addr == 0)
	{
		vty_error_out(vty, "Please first config router ID.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/* Get distribute source. */
	source = proto_redistnum(AFI_IP, argv[0]);
	if (source < 0)
	{
		if (strncmp ((char *)argv[0], "b", 1) == 0)
		{
			source = ROUTE_PROTO_EBGP;
		}
		else
		{
			return CMD_WARNING;
		}
	}

	//Get instance number
	/*
	Note: In order to comply with the command format "all-processes" of H3C, in fact, our ROUTER module
	does not support the import-route of all instances at one time. If "all-processes" is configured, 
	we will only import protocol routes with instance number 1 by default.
	*/
	instance = 1;
	OSPF_LOG_DEBUG("Get protol type:%s	instance num: %d", ospf_redist_string(source), instance);
	
	if (ROUTE_PROTO_OSPF == source	&& instance == ospf->ospf_id)
	{
		vty_error_out(vty, "The instance can not redistribute the routes of its own.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/* Get metric value. */
	if (argv[1] != NULL)
	{
		if (!str2metric(argv[1], &metric))
		{
			return CMD_WARNING;
		}
	}

	/* Get metric type. */
	if (argv[2] != NULL)
	{
		if (!str2metric_type(argv[2], &type))
		{
			return CMD_WARNING;
		}
	}

	/* Get tag value. */
	if (argv[3] != NULL)
	{
		VTY_GET_INTEGER("Value of tag", tag_value, argv[3]);
		tag_value = tag_value;//Only to clear compilation alarms
	}

	/* Get route-policy name. */
	if (argv[4] != NULL)
	{
		len = strlen(argv[4]);
		if(len < 1 && len > 63)
		{
			vty_error_out(vty, "The STRING length is not in the range of 1 and 63.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}

	/* Get nssa-only. */
	/*if (argv[5] != NULL)
	{
		;
	}*/

	/* Get allow-direct. */
	/*if (argv[6] != NULL)
	{
		;
	}*/
	OSPF_LOG_DEBUG("Get protol type:%s	instance num:%d  metric_type:%d  metric:%d", \
				ospf_redist_string(source), instance, type, metric);
	
	return ospf_redistribute_set(ospf, source, type, metric, instance);
}


DEFUN(no_ospf_redistribute_source,
      no_ospf_redistribute_source_cmd,
      "no redistribute " QUAGGA_REDIST_STR_OSPFD,
      NO_STR
      REDIST_STR
      QUAGGA_REDIST_HELP_STR_OSPFD)
{
    struct ospf *ospf = vty->index;
    int source;
    source = proto_redistnum(AFI_IP, argv[0]);

    if (source < 0 || source == ROUTE_PROTO_OSPF)
    {
        return CMD_WARNING;
    }

    ospf->route_policy_flag = 0;

    return ospf_redistribute_unset(ospf, source, 0);
}


DEFUN(no_ospf_redistribute_source_h3c,
	no_ospf_redistribute_source_h3c_cmd,
	"undo import-route (direct|static|bgp)",
	"Cancel the current setting\n"
	"Import routes from other protocols into OSPF\n"
	"Direct routes\n"
	"Static routes\n"
	"BGP routes\n")
{
	int source = -1;
	int source1 = -1;
	struct ospf *ospf = vty->index;
	
	source = proto_redistnum(AFI_IP, argv[0]);
	if (source < 0 || source == ROUTE_PROTO_OSPF)
	{
		if (strncmp ((char *)argv[0], "b", 1) == 0)
	 	{
			source = ROUTE_PROTO_EBGP;
			source1 = ROUTE_PROTO_IBGP;
	 	}
		else
		{
        	return CMD_WARNING;
		}
	}

	ospf->route_policy_flag = 0;

	OSPF_LOG_DEBUG("Get protol type:%s", ospf_redist_string(source));
	
	if(source1 > 0)
	{
		ospf_redistribute_unset(ospf, source1, 0);//clean ibgp
	}
		
	return ospf_redistribute_unset(ospf, source, 0);//clean ebgp
}


DEFUN(no_ospf_redistribute_ri_source,
      no_ospf_redistribute_ri_source_cmd,
      "no redistribute " QUAGGA_REDIST_STR_OSPFD_RI "<1-255>",
      NO_STR
      REDIST_STR
      QUAGGA_REDIST_HELP_STR_OSPFD_RI
      "instance id\n")
{
    struct ospf *ospf = vty->index;
    int source = -1;
    int instance = 0;
    source = proto_redistnum(AFI_IP, argv[0]);

    if (source < 0)
    {
        return CMD_WARNING;
    }

    VTY_GET_INTEGER("redistribute instance", instance, argv[1]);

    if (ROUTE_PROTO_OSPF == source  && instance == ospf->ospf_id)
    {
        vty_error_out(vty, "The instance can not redistribute the routes of its own.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return ospf_redistribute_unset(ospf, source, instance);
}


DEFUN(no_ospf_redistribute_ri_source_h3c,
	no_ospf_redistribute_ri_source_h3c_cmd,
	"undo import-route (rip |isis |ospf) (<1-65535>|all-processes)",
	"Cancel the current setting\n"
	"Import routes from other protocols into OSPF\n"
	"RIP routes\n"
	"IS-IS routes\n"
	"OSPF routes\n"
	"Process ID\n"
	"Redistribute all processes\n")
{
	int source = -1;
	int source1 = -1;
	int instance = 0;
	struct ospf *ospf = vty->index;
	source = proto_redistnum(AFI_IP, argv[0]);
	if (source < 0)
	{
		if (strncmp ((char *)argv[0], "b", 1) == 0)
	 	{
			source = ROUTE_PROTO_EBGP;
			source1 = ROUTE_PROTO_IBGP;
			source1 = source1;//Only to clear compilation alarms
	 	}
		else
		{
        	return CMD_WARNING;
		}
	}

	if(argv[1] != NULL)
	{
		if(strncmp ((char *)argv[1], "all", 3) == 0)
		{
			instance = 1;
		}
		else
		{
			VTY_GET_INTEGER_RANGE("import instance", instance, argv[1], 1, OSPF_MAX_PROCESS_ID_H3C);
		}
	}
	
	if (ROUTE_PROTO_OSPF == source  && instance == ospf->ospf_id)
	{
		vty_error_out(vty, "The instance can not redistribute the routes of its own.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	OSPF_LOG_DEBUG("Get protol type:%s  instance num: %d", ospf_redist_string(source), instance);
	
	return ospf_redistribute_unset(ospf, source, instance);
}


DEFUN(no_ospf_redistribute_ri_source_format2_h3c,
	no_ospf_redistribute_ri_source_format2_h3c_cmd,
	"undo import-route (rip |isis |ospf)",
	"Cancel the current setting\n"
	"Import routes from other protocols into OSPF\n"
	"RIP routes\n"
	"IS-IS routes\n"
	"OSPF routes\n")
{
	int source = -1;
	int source1 = -1;
	int instance = 0;
	struct ospf *ospf = vty->index;
	source = proto_redistnum(AFI_IP, argv[0]);
	if (source < 0)
	{
		if (strncmp ((char *)argv[0], "b", 1) == 0)
		{
			source = ROUTE_PROTO_EBGP;
			source1 = ROUTE_PROTO_IBGP;
			source1 = source1;//Only to clear compilation alarms
		}
		else
		{
			return CMD_WARNING;
		}
	}

	instance = 1;
	
	if (ROUTE_PROTO_OSPF == source	&& instance == ospf->ospf_id)
	{
		vty_error_out(vty, "The instance can not redistribute the routes of its own.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	OSPF_LOG_DEBUG("Get protol type:%s	instance num: %d", ospf_redist_string(source), instance);
	
	return ospf_redistribute_unset(ospf, source, instance);
}



DEFUN(ospf_distribute_list_out,
      ospf_distribute_list_out_cmd,
      "distribute-list WORD out " QUAGGA_REDIST_STR_OSPFD,
      "Filter networks in routing updates\n"
      "Access-list name\n"
      OUT_STR
      QUAGGA_REDIST_HELP_STR_OSPFD)
{
    struct ospf *ospf = vty->index;
    int source;
    /* Get distribute source. */
    source = proto_redistnum(AFI_IP, argv[1]);

    if (source < 0 || source == ROUTE_PROTO_OSPF)
    {
        return CMD_WARNING;
    }

    return ospf_distribute_list_out_set(ospf, source, argv[0]);
}

DEFUN(no_ospf_distribute_list_out,
      no_ospf_distribute_list_out_cmd,
      "no distribute-list WORD out " QUAGGA_REDIST_STR_OSPFD,
      NO_STR
      "Filter networks in routing updates\n"
      "Access-list name\n"
      OUT_STR
      QUAGGA_REDIST_HELP_STR_OSPFD)
{
    struct ospf *ospf = vty->index;
    int source;
    source = proto_redistnum(AFI_IP, argv[1]);

    if (source < 0 || source == ROUTE_PROTO_OSPF)
    {
        return CMD_WARNING;
    }

    return ospf_distribute_list_out_unset(ospf, source, argv[0]);
}

/* Default information originate. */
DEFUN(ospf_default_information_originate,
      ospf_default_information_originate_cmd,
      "default-information originate "
      "{always|metric <0-16777214>|metric-type (1|2)}",
      "Control distribution of default information\n"
      "Distribute a default route\n"
      "Always advertise default route\n"
      "OSPF default metric\n"
      "OSPF metric\n"
      "OSPF metric type for default routes\n"
      "Set OSPF External Type 1 metrics\n"
      "Set OSPF External Type 2 metrics\n")
{
    struct ospf *ospf = vty->index;
    int default_originate = DEFAULT_ORIGINATE_ZEBRA;
    int type = -1;
    int metric = -1;

    if (argc < 3)
    {
        return CMD_WARNING;    /* this should not happen */
    }

    /* Check whether "always" was specified */
    if (argv[0] != NULL)
    {
        default_originate = DEFAULT_ORIGINATE_ALWAYS;
    }

    /* Get metric value. */
    if (argv[1] != NULL)
        if (!str2metric(argv[1], &metric))
        {
            return CMD_WARNING;
        }

    /* Get metric type. */
    if (argv[2] != NULL)
        if (!str2metric_type(argv[2], &type))
        {
            return CMD_WARNING;
        }

    return ospf_redistribute_default_set(ospf, default_originate,
                                         type, metric);
}

static
void ospf_distance_update(struct route_table *rt, struct ospf *ospf)
{
    struct ospf_route *or = NULL;
    //struct prefix_ipv4 *p;
    struct route_node *rn = NULL;

    for (rn = route_top(rt); rn; rn = route_next(rn))
    {
        if ((or = rn->info) == NULL)
        {
            continue;
        }

        ospf_zebra_distance_update((struct prefix_ipv4 *) &rn->p, or, ospf);
    }
}


DEFUN(no_ospf_default_information_originate,
      no_ospf_default_information_originate_cmd,
      "no default-information originate",
      NO_STR
      "Control distribution of default information\n"
      "Distribute a default route\n")
{
    struct ospf *ospf = vty->index;
    struct prefix_ipv4 p;
    p.family = AF_INET;
    p.prefix.s_addr = 0;
    p.prefixlen = 0;
    ospf_external_lsa_flush(ospf, DEFAULT_ROUTE, &p, 0);

    if (EXTERNAL_INFO(ospf->vpn, DEFAULT_ROUTE))
    {
        //ospf_external_info_delete (DEFAULT_ROUTE,0, p);
        route_table_finish(EXTERNAL_INFO(ospf->vpn, DEFAULT_ROUTE));
        EXTERNAL_INFO(ospf->vpn, DEFAULT_ROUTE) = NULL;
    }

    //ospf_routemap_unset (ospf, DEFAULT_ROUTE);
    return ospf_redistribute_default_unset(ospf);
}

DEFUN(ospf_default_metric,
      ospf_default_metric_cmd,
      "default-metric <0-16777214>",
      "Set metric of redistributed routes\n"
      "Default metric\n")
{
    struct ospf *ospf = vty->index;
    int metric = -1;

    if (!str2metric(argv[0], &metric))
    {
        return CMD_WARNING;
    }

    ospf->default_metric = metric;
    return CMD_SUCCESS;
}

DEFUN(no_ospf_default_metric,
      no_ospf_default_metric_cmd,
      "no default-metric",
      NO_STR
      "Set metric of redistributed routes\n")
{
    struct ospf *ospf = vty->index;
    ospf->default_metric = -1;
    return CMD_SUCCESS;
}


DEFUN(distance_ospf,
      distance_ospf_cmd,
      "distance {external} <1-255>",
      "Define an administrative distance\n"
      "Distance for external routes\n"
      "Distance value\n")
{
    struct ospf *ospf = vty->index;

    if (argv[0] != NULL)
    {
        ospf->distance_external = atoi(argv[1]);

        if (ospf->new_external_route != NULL)
        {
            ospf_distance_update(ospf->old_external_route, ospf);
        }
    }
    else
    {
        ospf->distance_all = atoi(argv[1]);

        if (ospf->new_external_route != NULL)
        {
            ospf_distance_update(ospf->old_external_route, ospf);
        }

        if (ospf->new_table != NULL)
        {
            ospf_distance_update(ospf->new_table, ospf);
        }
    }

    return CMD_SUCCESS;
}


DEFUN(distance_ospf_h3c,
	distance_ospf_h3c_cmd,
	"preference {ase} <1-255> {route-policy STRING}",
	"Specify the preference for OSPF routes\n"
	"Preference for ASE routes\n"
	"Preference value\n"
	"Specify the routing policy\n"
	"Name of the routing policy(string length <1-63>)\n")
{
	int str_len = 0;
	struct ospf *ospf = vty->index;

	if (argv[0] != NULL)
	{
		/*if((argv[1] == NULL) && (argv[2] == NULL))
		{
			vty_warning_out(vty, "Command incomplete.");
        	return CMD_WARNING;
		}*/

		if(argv[2] != NULL)
		{
			str_len = strlen(argv[2]);
			if(str_len > 63 || str_len < 1)
			{
				vty_error_out(vty, "THe name length of the routing policy is 1-63 bytes!");
        		return CMD_WARNING;
			}
		}

		if(argv[1] != NULL)
		{
			ospf->distance_external = atoi(argv[1]);

			if (ospf->new_external_route != NULL)
			{
			ospf_distance_update(ospf->old_external_route, ospf);
			}
		}
	}
	else
	{
		/*if((argv[1] == NULL) && (argv[2] == NULL))
		{
			vty_warning_out(vty, "Command incomplete.");
        	return CMD_WARNING;
		}*/
		if(argv[2] != NULL)
		{
			str_len = strlen(argv[2]);
			if(str_len > 63 || str_len < 1)
			{
				vty_error_out(vty, "THe name length of the routing policy is 1-63 bytes!");
        		return CMD_WARNING;
			}
		}

		if(argv[1] != NULL)
		{
			ospf->distance_all = atoi(argv[1]);

			if (ospf->new_external_route != NULL)
			{
				ospf_distance_update(ospf->old_external_route, ospf);
			}

			if (ospf->new_table != NULL)
			{
				ospf_distance_update(ospf->new_table, ospf);
			}
		}
	}

  return CMD_SUCCESS;
}


DEFUN(distance_ospf_h3c_adapt_format1,
	distance_ospf_h3c_adapt_format1_cmd,
	"preference {ase} route-policy STRING [<1-255>]",
	"Specify the preference for OSPF routes\n"
	"Preference for ASE routes\n"
	"Specify the routing policy\n"
	"Name of the routing policy(string length <1-63>)\n"
	"Preference value\n")
{
	int str_len = 0;
	struct ospf *ospf = vty->index;

	if (argv[0] != NULL)
	{
		if(argv[1] != NULL)
		{
			OSPF_LOG_DEBUG(" ase argv[1] = %s\n", argv[1]);
			str_len = strlen(argv[1]);
			if(str_len > 63 || str_len < 1)
			{
				vty_error_out(vty, "THe name length of the routing policy is 1-63 bytes!");
				return CMD_WARNING;
			}
		}

		if(argv[2] != NULL)
		{
			OSPF_LOG_DEBUG(" ase argv[2] = %s\n", argv[2]);
			ospf->distance_external = atoi(argv[2]);

			if (ospf->new_external_route != NULL)
			{
			ospf_distance_update(ospf->old_external_route, ospf);
			}
		}
	}
	else
	{
		if(argv[1] != NULL)
		{
			OSPF_LOG_DEBUG(" no ase argv[1] = %s\n", argv[1]);
			str_len = strlen(argv[1]);
			if(str_len > 63 || str_len < 1)
			{
				vty_error_out(vty, "THe name length of the routing policy is 1-63 bytes!");
				return CMD_WARNING;
			}
		}

		if(argv[2] != NULL)
		{
			OSPF_LOG_DEBUG(" no ase argv[2] = %s\n", argv[2]);
			ospf->distance_all = atoi(argv[2]);

			if (ospf->new_external_route != NULL)
			{
				ospf_distance_update(ospf->old_external_route, ospf);
			}

			if (ospf->new_table != NULL)
			{
				ospf_distance_update(ospf->new_table, ospf);
			}
		}
	}

	return CMD_SUCCESS;
}


DEFUN(no_distance_ospf,
      no_distance_ospf_cmd,
      "no distance {external}",
      NO_STR
      "Define an administrative distance\n"
      "External routes\n")
{
    struct ospf *ospf = vty->index;

    if (argv[0] != NULL)
    {
        ospf->distance_external = 0;

        if (ospf->new_external_route != NULL)
        {
            ospf_distance_update(ospf->old_external_route, ospf);
        }
    }
    else
    {
        ospf->distance_all = ROUTE_METRIC_OSPF;

        if (ospf->new_external_route != NULL)
        {
            ospf_distance_update(ospf->old_external_route, ospf);
        }

        if (ospf->new_table != NULL)
        {
            ospf_distance_update(ospf->new_table, ospf);
        }
    }

    return CMD_SUCCESS;
}

ALIAS(no_distance_ospf,
      no_distance_ospf_h3c_cmd,
      "undo preference {ase}",
      "Cancel the current setting\n"
      "Specify the preference for OSPF routes\n"
	  "Preference for ASE routes\n")


DEFUN(ip_ospf_mtu_ignore,
      ip_ospf_mtu_ignore_cmd,
      "ip ospf mtu-enable",
      "IP Information\n"
      "OSPF interface commands\n"
      "Enable mtu mismatch detection\n")
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct interface *ifp = NULL;
    struct in_addr addr;
    //int ret;
    struct ospf_if_params *params = NULL;
    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
    }

    params->mtu_ignore = 0;

    if (params->mtu_ignore != OSPF_MTU_IGNORE_DEFAULT)
    {
        SET_IF_PARAM(params, mtu_ignore);
    }
    else
    {
        UNSET_IF_PARAM(params, mtu_ignore);

        if (params != IF_DEF_PARAMS(ifp))
        {
            ospf_free_if_params(ifp, addr);
            ospf_if_update_params(ifp, addr);
        }
    }

    return CMD_SUCCESS;
}


ALIAS(ip_ospf_mtu_ignore,
	ip_ospf_mtu_ignore_h3c_cmd,
	"ospf mtu-enable",
	"OSPF interface commands\n"
	"Enable mtu mismatch detection\n")


DEFUN(no_ip_ospf_mtu_ignore,
      no_ip_ospf_mtu_ignore_cmd,
      "no ip ospf mtu-enable",
      "IP Information\n"
      "OSPF interface commands\n"
      "Enable mtu mismatch detection\n")
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct interface *ifp = NULL;
    struct in_addr addr;
    //int ret;
    struct ospf_if_params *params = NULL;
    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = ospf_find_interface(ifindex);
    params = IF_DEF_PARAMS(ifp);

	if(params == NULL)
	{
		VTY_INTERFACE_PARAMETER_ERR
	}

    if (params->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
    }

    params->mtu_ignore = 1;

    if (params->mtu_ignore != OSPF_MTU_IGNORE_DEFAULT)
    {
        SET_IF_PARAM(params, mtu_ignore);
    }
    else
    {
        UNSET_IF_PARAM(params, mtu_ignore);

        if (params != IF_DEF_PARAMS(ifp))
        {
            ospf_free_if_params(ifp, addr);
            ospf_if_update_params(ifp, addr);
        }
    }

    return CMD_SUCCESS;
}


ALIAS(no_ip_ospf_mtu_ignore,
	no_ip_ospf_mtu_ignore_h3c_cmd,
	"undo ospf mtu-enable",
	"Cancel the current setting\n"
	"OSPF interface commands\n"
	"Enable mtu mismatch detection\n")
			

DEFUN(ospf_max_metric_router_lsa_admin,
      ospf_max_metric_router_lsa_admin_cmd,
      "max-metric router-lsa administrative",
      "OSPF maximum / infinite-distance metric\n"
      "Advertise own Router-LSA with infinite distance (stub router)\n"
      "Administratively applied, for an indefinite period\n")
{
    struct listnode *ln;
    struct ospf_area *area;
    struct ospf *ospf = vty->index;

    for (ALL_LIST_ELEMENTS_RO(ospf->areas, ln, area))
    {
        SET_FLAG(area->stub_router_state, OSPF_AREA_ADMIN_STUB_ROUTED);

        if (!CHECK_FLAG(area->stub_router_state, OSPF_AREA_IS_STUB_ROUTED))
        {
            ospf_router_lsa_update_area(area);
        }
    }

    /* Allows for areas configured later to get the property */
    ospf->stub_router_admin_set = OSPF_STUB_ROUTER_ADMINISTRATIVE_SET;
    return CMD_SUCCESS;
}

DEFUN(no_ospf_max_metric_router_lsa_admin,
      no_ospf_max_metric_router_lsa_admin_cmd,
      "no max-metric router-lsa administrative",
      NO_STR
      "OSPF maximum / infinite-distance metric\n"
      "Advertise own Router-LSA with infinite distance (stub router)\n"
      "Administratively applied, for an indefinite period\n")
{
    struct listnode *ln;
    struct ospf_area *area;
    struct ospf *ospf = vty->index;

    for (ALL_LIST_ELEMENTS_RO(ospf->areas, ln, area))
    {
        UNSET_FLAG(area->stub_router_state, OSPF_AREA_ADMIN_STUB_ROUTED);

        /* Don't trample on the start-up stub timer */
        if (CHECK_FLAG(area->stub_router_state, OSPF_AREA_IS_STUB_ROUTED)
                && !area->t_stub_router)
        {
            UNSET_FLAG(area->stub_router_state, OSPF_AREA_IS_STUB_ROUTED);
            ospf_router_lsa_update_area(area);
        }
    }

    ospf->stub_router_admin_set = OSPF_STUB_ROUTER_ADMINISTRATIVE_UNSET;
    return CMD_SUCCESS;
}

DEFUN(ospf_max_metric_router_lsa_startup,
      ospf_max_metric_router_lsa_startup_cmd,
      "max-metric router-lsa on-startup <5-86400>",
      "OSPF maximum / infinite-distance metric\n"
      "Advertise own Router-LSA with infinite distance (stub router)\n"
      "Automatically advertise stub Router-LSA on startup of OSPF\n"
      "Time (seconds) to advertise self as stub-router\n")
{
    unsigned int seconds;
    struct ospf *ospf = vty->index;

    if (argc != 1)
    {
        vty_error_out(vty, "Must supply stub-router period");
        return CMD_WARNING;
    }

    VTY_GET_INTEGER("stub-router startup period", seconds, argv[0]);
    ospf->stub_router_startup_time = seconds;
    return CMD_SUCCESS;
}

DEFUN(no_ospf_max_metric_router_lsa_startup,
      no_ospf_max_metric_router_lsa_startup_cmd,
      "no max-metric router-lsa on-startup",
      NO_STR
      "OSPF maximum / infinite-distance metric\n"
      "Advertise own Router-LSA with infinite distance (stub router)\n"
      "Automatically advertise stub Router-LSA on startup of OSPF\n")
{
    struct listnode *ln;
    struct ospf_area *area;
    struct ospf *ospf = vty->index;
    ospf->stub_router_startup_time = OSPF_STUB_ROUTER_UNCONFIGURED;

    for (ALL_LIST_ELEMENTS_RO(ospf->areas, ln, area))
    {
        SET_FLAG(area->stub_router_state, OSPF_AREA_WAS_START_STUB_ROUTED);
        OSPF_TIMER_OFF(area->t_stub_router);

        /* Don't trample on admin stub routed */
        if (!CHECK_FLAG(area->stub_router_state, OSPF_AREA_ADMIN_STUB_ROUTED))
        {
            UNSET_FLAG(area->stub_router_state, OSPF_AREA_IS_STUB_ROUTED);
            ospf_router_lsa_update_area(area);
        }
    }

    return CMD_SUCCESS;
}

DEFUN(ospf_max_metric_router_lsa_shutdown,
      ospf_max_metric_router_lsa_shutdown_cmd,
      "max-metric router-lsa on-shutdown <5-86400>",
      "OSPF maximum / infinite-distance metric\n"
      "Advertise own Router-LSA with infinite distance (stub router)\n"
      "Advertise stub-router prior to full shutdown of OSPF\n"
      "Time (seconds) to wait till full shutdown\n")
{
    unsigned int seconds;
    struct ospf *ospf = vty->index;

    if (argc != 1)
    {
        vty_error_out(vty, "Must supply stub-router shutdown period");
        return CMD_WARNING;
    }

    VTY_GET_INTEGER("stub-router shutdown wait period", seconds, argv[0]);
    ospf->stub_router_shutdown_time = seconds;
    return CMD_SUCCESS;
}

DEFUN(no_ospf_max_metric_router_lsa_shutdown,
      no_ospf_max_metric_router_lsa_shutdown_cmd,
      "no max-metric router-lsa on-shutdown",
      NO_STR
      "OSPF maximum / infinite-distance metric\n"
      "Advertise own Router-LSA with infinite distance (stub router)\n"
      "Advertise stub-router prior to full shutdown of OSPF\n")
{
    struct ospf *ospf = vty->index;
    ospf->stub_router_shutdown_time = OSPF_STUB_ROUTER_UNCONFIGURED;
    return CMD_SUCCESS;
}

static void
config_write_stub_router(struct vty *vty, struct ospf *ospf)
{
    struct listnode *ln;
    struct ospf_area *area;

    if (ospf->stub_router_startup_time != OSPF_STUB_ROUTER_UNCONFIGURED)
        vty_out(vty, " max-metric router-lsa on-startup %u%s",
                ospf->stub_router_startup_time, VTY_NEWLINE);

    if (ospf->stub_router_shutdown_time != OSPF_STUB_ROUTER_UNCONFIGURED)
        vty_out(vty, " max-metric router-lsa on-shutdown %u%s",
                ospf->stub_router_shutdown_time, VTY_NEWLINE);

    for (ALL_LIST_ELEMENTS_RO(ospf->areas, ln, area))
    {
        if (CHECK_FLAG(area->stub_router_state, OSPF_AREA_ADMIN_STUB_ROUTED))
        {
            vty_out(vty, " max-metric router-lsa administrative%s",
                    VTY_NEWLINE);
            break;
        }
    }

    return;
}

static void
show_ip_ospf_route_network(struct vty *vty, struct route_table *rt)
{
    struct route_node *rn;
    struct ospf_route *or;
    struct listnode *pnode, *pnnode;
    struct ospf_path *path;
    vty_out(vty, "============ OSPF network routing table ============%s",
            VTY_NEWLINE);

    for (rn = route_top(rt); rn; rn = route_next(rn))
    {
        if ((or = rn->info) != NULL)
        {
            char buf1[19];
            snprintf(buf1, 19, "%s/%d",
                     inet_ntoa(rn->p.u.prefix4), rn->p.prefixlen);

            switch (or->path_type)
            {
                case OSPF_PATH_INTER_AREA:

                    if (or->type == OSPF_DESTINATION_NETWORK)
                        vty_out(vty, "N IA %-18s    cost:[%d] area: %s%s", buf1, or->cost,
                                inet_ntoa(or->u.std.area_id), VTY_NEWLINE);

                    //else if (or->type == OSPF_DESTINATION_DISCARD)
                    //vty_out (vty, "D IA %-18s    Discard entry%s", buf1, VTY_NEWLINE);
                    break;
                case OSPF_PATH_INTRA_AREA:
                    vty_out(vty, "N    %-18s    cost:[%d] area: %s%s", buf1, or->cost,
                            inet_ntoa(or->u.std.area_id), VTY_NEWLINE);
                    break;
                default:
                    break;
            }

            if (or->type == OSPF_DESTINATION_NETWORK)
                for (ALL_LIST_ELEMENTS(or->paths, pnode, pnnode, path))
                {
                    if (if_lookup_by_index(path->ifindex))
                    {
                        if (path->nexthop.s_addr == 0)
                            vty_out(vty, "%24s   directly attached to %s%s",
                                    "", ifindex2ifname(path->ifindex), VTY_NEWLINE);
                        else
                            vty_out(vty, "%24s   via %s, %s%s", "",
                                    inet_ntoa(path->nexthop),
                                    ifindex2ifname(path->ifindex), VTY_NEWLINE);
                    }
                }
        }
    }

    vty_out(vty, "%s", VTY_NEWLINE);
}

static void
show_ip_ospf_route_router(struct vty *vty, struct route_table *rtrs)
{
    struct route_node *rn;
    struct ospf_route *or;
    struct listnode *pnode;
    struct listnode *node;
    struct ospf_path *path;
    vty_out(vty, "============ OSPF router routing table =============%s",
            VTY_NEWLINE);

    for (rn = route_top(rtrs); rn; rn = route_next(rn))
    {
        if (rn->info)
        {
            int flag = 0;
            vty_out(vty, "R    %-15s    ", inet_ntoa(rn->p.u.prefix4));

            for (ALL_LIST_ELEMENTS_RO((struct list *)rn->info, node, or))
            {
                if (flag++)
                {
                    vty_out(vty, "%24s", "");
                }

                /* Show path. */
                vty_out(vty, "%s cost:[%d] area: %s",
                        (or->path_type == OSPF_PATH_INTER_AREA ? "IA" : "  "),
                        or->cost, inet_ntoa(or->u.std.area_id));
                /* Show flags. */
                vty_out(vty, "%s%s%s",
                        (or->u.std.flags & ROUTER_LSA_BORDER ? ", ABR" : ""),
                        (or->u.std.flags & ROUTER_LSA_EXTERNAL ? ", ASBR" : ""),
                        VTY_NEWLINE);

                for (ALL_LIST_ELEMENTS_RO(or->paths, pnode, path))
                {
                    if (if_lookup_by_index(path->ifindex))
                    {
                        if (path->nexthop.s_addr == 0)
                            vty_out(vty, "%24s   directly attached to %s%s",
                                    "", ifindex2ifname(path->ifindex),
                                    VTY_NEWLINE);
                        else
                            vty_out(vty, "%24s   via %s, %s%s", "",
                                    inet_ntoa(path->nexthop),
                                    ifindex2ifname(path->ifindex),
                                    VTY_NEWLINE);
                    }
                }
            }
        }
    }

    vty_out(vty, "%s", VTY_NEWLINE);
}

static void
show_ip_ospf_route_external(struct vty *vty, struct route_table *rt)
{
    struct route_node *rn = NULL;
    struct ospf_route *er = NULL;
    struct listnode *pnode = NULL, *pnnode = NULL;
    struct ospf_path *path;
    vty_out(vty, "============ OSPF external routing table ===========%s",
            VTY_NEWLINE);

    for (rn = route_top(rt); rn; rn = route_next(rn))
    {
        if ((er = rn->info) != NULL)
        {
            char buf1[19];
            snprintf(buf1, 19, "%s/%d",
                     inet_ntoa(rn->p.u.prefix4), rn->p.prefixlen);

            switch (er->path_type)
            {
                case OSPF_PATH_TYPE1_EXTERNAL:
                    vty_out(vty, "N E1 %-18s    cost:[%d] tag: %u%s", buf1,
                            er->cost, er->u.ext.tag, VTY_NEWLINE);
                    break;
                case OSPF_PATH_TYPE2_EXTERNAL:
                    vty_out(vty, "N E2 %-18s    cost:[%d/%d] tag: %u%s", buf1, er->cost,
                            er->u.ext.type2_cost, er->u.ext.tag, VTY_NEWLINE);
                    break;
            }

            for (ALL_LIST_ELEMENTS(er->paths, pnode, pnnode, path))
            {
                if (if_lookup_by_index(path->ifindex))
                {
                    if (path->nexthop.s_addr == 0)
                        vty_out(vty, "%24s   directly attached to %s%s",
                                "", ifindex2ifname(path->ifindex), VTY_NEWLINE);
                    else
                        vty_out(vty, "%24s   via %s, %s%s", "",
                                inet_ntoa(path->nexthop),
                                ifindex2ifname(path->ifindex),
                                VTY_NEWLINE);
                }
            }
        }
    }

    vty_out(vty, "%s", VTY_NEWLINE);
}

DEFUN(show_ip_ospf_border_routers,
      show_ip_ospf_border_routers_cmd,
      "show ip ospf [<1-255>] border-routers",
      SHOW_STR
      IP_STR
      "OSPF information\n"
      "OSPF instance number\n"
      "show all the ABR's and ASBR's\n")
{
    struct ospf *ospf;
    /* Check OSPF is enable. */
    int ospf_id;
    ospf_id = OSPF_DEFAULT_PROCESS_ID;

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], 1, 255);
    }

    ospf = ospf_lookup_id(ospf_id);

    if (ospf == NULL)
    {
        VTY_OSPF_INSTANCE_ERR
    }

    if (ospf->new_table == NULL)
    {
        vty_error_out(vty, "No OSPF routing information exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /* Show Network routes.
    show_ip_ospf_route_network (vty, ospf->new_table);   */
    /* Show Router routes. */
    show_ip_ospf_route_router(vty, ospf->new_rtrs);
    return CMD_SUCCESS;
}

DEFUN(show_ip_ospf_route,
      show_ip_ospf_route_cmd,
      "show ip ospf [<1-255>] route",
      SHOW_STR
      IP_STR
      "OSPF information\n"
      "OSPF instance number\n"
      "OSPF routing table\n")
{
    struct ospf *ospf = NULL;
    /* Check OSPF is enable. */
    int ospf_id;
    ospf_id = OSPF_DEFAULT_PROCESS_ID;

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID);
    }

    ospf = ospf_lookup_id(ospf_id);

    if (ospf == NULL)
    {
        VTY_OSPF_INSTANCE_ERR
    }

    if (ospf->new_table == NULL)
    {
        vty_error_out(vty, "No OSPF routing information exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /* Show Network routes. */
    show_ip_ospf_route_network(vty, ospf->new_table);
    /* Show Router routes. */
    //show_ip_ospf_route_router (vty, ospf->new_rtrs);
    /* Show AS External routes. */
    show_ip_ospf_route_external(vty, ospf->old_external_route);
    return CMD_SUCCESS;
}


DEFUN(display_ospf_route_h3c,
	display_ospf_route_h3c_cmd,
	"display ospf [ <1-65535> ] routing",
	"Display current system information\n"
	"OSPF information\n"
	"OSPF instance number\n"
	"OSPF routing table\n")
{
    int ospf_id;
	struct ospf *ospf = NULL;
	struct listnode *node = NULL;
    ospf_id = OSPF_DEFAULT_PROCESS_ID;

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID_H3C);
    
	    ospf = ospf_lookup_id(ospf_id);
	    if (ospf == NULL)
	    {
	        VTY_OSPF_INSTANCE_ERR
	    }

		if (ospf->new_table == NULL)
	    {
	        vty_error_out(vty, "No OSPF routing information exist%s", VTY_NEWLINE);
	        return CMD_WARNING;
	    }

		/* Show Network routes. */
	    show_ip_ospf_route_network(vty, ospf->new_table);
	    /* Show Router routes. */
	    //show_ip_ospf_route_router (vty, ospf->new_rtrs);
	    /* Show AS External routes. */
	    show_ip_ospf_route_external(vty, ospf->old_external_route);
	}
	else
	{
		for (ALL_LIST_ELEMENTS_RO (om->ospf, node, ospf))
		{
			vty_out(vty, "%s%-10s OSPF Instance %d with Router ID %s%s",
            	VTY_NEWLINE, " ", ospf->ospf_id, inet_ntoa(ospf->router_id), VTY_NEWLINE);
			
			if (ospf->new_table == NULL)
		    {
		        vty_error_out(vty, "No OSPF routing information exist%s", VTY_NEWLINE);
				vty_out(vty, " *******************************************************************%s", VTY_NEWLINE);
		        continue;
		    }

		    /* Show Network routes. */
		    show_ip_ospf_route_network(vty, ospf->new_table);
		    /* Show Router routes. */
		    //show_ip_ospf_route_router (vty, ospf->new_rtrs);
		    /* Show AS External routes. */
		    show_ip_ospf_route_external(vty, ospf->old_external_route);
			vty_out(vty, " *******************************************************************%s", VTY_NEWLINE);
		}
	}
    
    return CMD_SUCCESS;
}

const char *ospf_err_str[] =
{
    "IP: received my own packet",
    "Bad packet",
    "Bad version",
    "Bad checksum",
    "Bad area id ",
    "Drop on unnumbered interface",
    "Bad virtual link",
    "Bad authentication type",
    "Bad authentication key",
    "Packet too small",
    "Packet size > ip length ",
    "Transmit error",
    "Interface down",
    "Unknown neighbor",
    /* hello */
    "Netmask mismatch",
    "Hello timer mismatch",
    "Dead timer mismatch",
    "Extern option mismatch",  
    "Virtual neighbor unknown",
    "Router ID confusion",
    "NBMA neighbor unknown ",
    /* dd */
    "Neighbor state low",
    "Router ID confusion",
    "Extern option mismatch", 
    "Unknown LSA type", 
    "MTU option mismatch",  
    /* lsack */
    "Neighbor state low",
    "Unknown LSA Type", 
    /* lsr */
    "Neighbor state low",
    "Bad request",
    /* lsu */ 
    "Neighbor state low",
    "LSA checksum bad",
    "Unknown LSA Type",  
    "Received less recent LSA",
    
};

static void
show_ip_ospf_err(struct vty *vty, struct ospf *ospf)
{
    vty_out(vty, "    		OSPF Instance %d Router ID %s%s", ospf->ospf_id,
            inet_ntoa(ospf->router_id), VTY_NEWLINE);
    vty_out(vty, "       			OSPF Error statictics%s%s", VTY_NEWLINE, VTY_NEWLINE);
    

    vty_out(vty, "General packet errors:%s", VTY_NEWLINE);
    vty_out(vty, " %-4d : %-30s  %-4d : %-30s%s", ospf->o_err.receive_own_count, ospf_err_str[0],
            ospf->o_err.receive_bad_pkt_count, ospf_err_str[1], VTY_NEWLINE);
    vty_out(vty, " %-4d : %-30s  %-4d : %-30s%s", ospf->o_err.receive_bad_version_count, ospf_err_str[2],
            ospf->o_err.receive_bad_check_count, ospf_err_str[3], VTY_NEWLINE);
    vty_out(vty, " %-4d : %-30s  %-4d : %-30s%s", ospf->o_err.receive_bad_area_count, ospf_err_str[4],
            ospf->o_err.receive_unnumberd_count, ospf_err_str[5], VTY_NEWLINE);
    vty_out(vty, " %-4d : %-30s  %-4d : %-30s%s", ospf->o_err.receive_bad_vertual_count, ospf_err_str[6],
            ospf->o_err.receive_auth_type_count, ospf_err_str[7], VTY_NEWLINE);
    vty_out(vty, " %-4d : %-30s  %-4d : %-30s%s", ospf->o_err.receive_auth_key_count, ospf_err_str[8],
            ospf->o_err.receive_pkt_small_count, ospf_err_str[9], VTY_NEWLINE);
    vty_out(vty, " %-4d : %-30s  %-4d : %-30s%s", ospf->o_err.receive_pkt_large_count, ospf_err_str[10],
            ospf->o_err.receive_transmit_count, ospf_err_str[11], VTY_NEWLINE);
    vty_out(vty, " %-4d : %-30s  %-4d : %-30s%s", ospf->o_err.receive_down_count, ospf_err_str[12],
            ospf->o_err.receive_neighbor_count, ospf_err_str[13], VTY_NEWLINE);

    /* hello */
    vty_out(vty, "%s", VTY_NEWLINE);
    vty_out(vty, "HELLO packet errors:%s", VTY_NEWLINE);
    vty_out(vty, " %-4d : %-30s  %-4d : %-30s%s", ospf->o_err.receive_mask_mismatch_count, ospf_err_str[14],
            ospf->o_err.receive_hello_mismatch_count, ospf_err_str[15], VTY_NEWLINE);
    vty_out(vty, " %-4d : %-30s  %-4d : %-30s%s", ospf->o_err.receive_dead_mismatch_count, ospf_err_str[16],
            ospf->o_err.receive_option_mismatch_count, ospf_err_str[17], VTY_NEWLINE);
    vty_out(vty, " %-4d : %-30s  %-4d : %-30s%s", ospf->o_err.receive_virnei_unknown_count, ospf_err_str[18],
        ospf->o_err.receive_hello_routerid_mismatch_count, ospf_err_str[19], VTY_NEWLINE);
    vty_out(vty, " %-4d : %-30s%s", ospf->o_err.receive_nbmanei_unknown_count, ospf_err_str[20], VTY_NEWLINE);

    /* dd */
    vty_out(vty, "%s", VTY_NEWLINE);
    vty_out(vty, "DD packet errors:%s", VTY_NEWLINE);
    vty_out(vty, " %-4d : %-30s  %-4d : %-30s%s", ospf->o_err.receive_dd_neighbor_low_count, ospf_err_str[21],
            ospf->o_err.receive_dd_routerid_mismatch_count, ospf_err_str[22], VTY_NEWLINE);
    vty_out(vty, " %-4d : %-30s  %-4d : %-30s%s", ospf->o_err.receive_extern_option_count, ospf_err_str[23],
        ospf->o_err.receive_dd_unlsa_count, ospf_err_str[24], VTY_NEWLINE);
    vty_out(vty, " %-4d : %-30s%s", ospf->o_err.receive_mtu_mismatch_count, ospf_err_str[25], VTY_NEWLINE);

    /* ls ack */
    vty_out(vty, "%s", VTY_NEWLINE);
    vty_out(vty, "LS ACK packet errors:%s", VTY_NEWLINE);    
    vty_out(vty, " %-4d : %-30s  %-4d : %-30s%s", ospf->o_err.receive_lsack_neighbor_low_count, ospf_err_str[26],
        ospf->o_err.receive_lsack_neighbor_low_count, ospf_err_str[27], VTY_NEWLINE);

    /* lsr */
    vty_out(vty, "%s", VTY_NEWLINE);
    vty_out(vty, "LS REQ packet errors:%s", VTY_NEWLINE);
    vty_out(vty, " %-4d : %-30s  %-4d : %-30s%s", ospf->o_err.receive_lsr_neighbor_low_count, ospf_err_str[28],
        ospf->o_err.receive_bad_req_count, ospf_err_str[29], VTY_NEWLINE);
    /* lsu */
    vty_out(vty, "%s", VTY_NEWLINE);
    vty_out(vty, "LS UPD packet errors:%s", VTY_NEWLINE);
    vty_out(vty, " %-4d : %-30s  %-4d : %-30s%s", ospf->o_err.receive_lsu_neighbor_low_count, ospf_err_str[30],
        ospf->o_err.receive_lsu_lsa_check_count, ospf_err_str[31], VTY_NEWLINE);
    vty_out(vty, " %-4d : %-30s  %-4d : %-30s%s", ospf->o_err.receive_lsu_unlsa_count, ospf_err_str[32],
        ospf->o_err.receive_less_recetlsa_count, ospf_err_str[33], VTY_NEWLINE);

    
}



DEFUN(show_ip_ospf_error,
      show_ip_ospf_error_cmd,
      "show ip ospf [<1-255>] error",
      SHOW_STR
      IP_STR
      "OSPF information\n"
      "OSPF instance number\n"
      "Error information\n")
{
    struct ospf *ospf = NULL;
    /* Check OSPF is enable. */
    int ospf_id;
    ospf_id = OSPF_DEFAULT_PROCESS_ID;

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID);
    }

    ospf = ospf_lookup_id(ospf_id);

    if (ospf == NULL)
    {
        VTY_OSPF_INSTANCE_ERR
    }

    show_ip_ospf_err(vty, ospf);
    return CMD_SUCCESS;
}


DEFUN(display_ospf_statistics_error,
	display_ospf_statistics_error_h3c_cmd,
	"display ospf [<1-65535>] statistics error",
	"Display current system information\n"
	"OSPF information\n"
	"OSPF instance number\n"
	"OSPF info statistics\n"
	"Error information\n")
{
    int ospf_id;
	struct ospf *ospf = NULL;
	struct listnode *node = NULL;
	
    ospf_id = OSPF_DEFAULT_PROCESS_ID;

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf instance", ospf_id, argv[0], OSPF_DEFAULT_PROCESS_ID, OSPF_MAX_PROCESS_ID_H3C);
    
	    ospf = ospf_lookup_id(ospf_id);
	    if (ospf == NULL)
	    {
	        VTY_OSPF_INSTANCE_ERR
	    }

	    show_ip_ospf_err(vty, ospf);
	}
	else
	{
		for (ALL_LIST_ELEMENTS_RO (om->ospf, node, ospf))
		{
			show_ip_ospf_err(vty, ospf);
			vty_out(vty, " ********************************************************************%s%s", 
							VTY_NEWLINE, VTY_NEWLINE);
		}
	}
    return CMD_SUCCESS;
}


const char *ospf_abr_type_str[] =
{
    "unknown",
    "standard",
    "ibm",
    "cisco",
    "shortcut"
};

const char *ospf_shortcut_mode_str[] =
{
    "default",
    "enable",
    "disable"
};


static void
area_id2str(char *buf, int length, struct ospf_area *area)
{
    memset(buf, 0, length);

    if (area->format == OSPF_AREA_ID_FORMAT_ADDRESS)
    {
        strncpy(buf, inet_ntoa(area->area_id), length);
    }
    else
    {
        sprintf(buf, "%lu", (unsigned long) ntohl(area->area_id.s_addr));
    }
}


const char *ospf_int_type_str[] =
{
    "unknown",        /* should never be used. */
    "p2p",
    "broadcast",
    "nbma",
    "p2mp",
    "virtual-link",   /* should never be used. */
    "loopback"
};

static int
config_write_ospf_nbr_nbma(struct vty *vty, struct ospf *ospf)
{
    struct ospf_nbr_nbma *nbr_nbma;
    struct route_node *rn;

    /* Static Neighbor configuration print. */
    for (rn = route_top(ospf->nbr_nbma); rn; rn = route_next(rn))
    {
        if ((nbr_nbma = rn->info))
        {
            vty_out(vty, " neighbor %s", inet_ntoa(nbr_nbma->addr));

            if (nbr_nbma->priority != OSPF_NEIGHBOR_PRIORITY_DEFAULT)
            {
                vty_out(vty, " priority %d", nbr_nbma->priority);
            }

            if (nbr_nbma->v_poll != OSPF_POLL_INTERVAL_DEFAULT)
            {
                vty_out(vty, " poll-interval %d", nbr_nbma->v_poll);
            }

            vty_out(vty, "%s", VTY_NEWLINE);
        }
    }

    return 0;
}

static int
config_write_virtual_link(struct vty *vty, struct ospf *ospf, struct ospf_area* area_buf)
{
	int16_t flag = 0;//用来决定命令的显示格式
	char vty_out_buf[256] = "";
    struct listnode *node = NULL;
    struct ospf_vl_data *vl_data = NULL;
	struct ospf_if_params *params = NULL;
    u_char buf[INET_ADDRSTRLEN];

    /* Virtual-Link print */
    for (ALL_LIST_ELEMENTS_RO(ospf->vlinks, node, vl_data))
    {
        struct listnode *n2;
        struct crypt_key *ck;
        struct ospf_interface *oi;

		flag = 0;
        if ((vl_data != NULL) && (vl_data->vl_area_id.s_addr == area_buf->area_id.s_addr))
        {
            memset(buf, 0, INET_ADDRSTRLEN);

            if (vl_data->format == OSPF_AREA_ID_FORMAT_ADDRESS)
            {
                strncpy((char *) buf, inet_ntoa(vl_data->vl_area_id), INET_ADDRSTRLEN);
            }
            else
			{
                sprintf((char *) buf, "%lu",
                        (unsigned long int) ntohl(vl_data->vl_area_id.s_addr));
			}
		
            oi = vl_data->vl_oi;
			params = IF_DEF_PARAMS(vl_data->vl_oi->ifp);
			
            /* timers */
            if (OSPF_IF_PARAM(oi, v_hello) != OSPF_HELLO_INTERVAL_DEFAULT ||
                    OSPF_IF_PARAM(oi, v_wait) != OSPF_ROUTER_DEAD_INTERVAL_DEFAULT ||
                    OSPF_IF_PARAM(oi, retransmit_interval) != OSPF_RETRANSMIT_INTERVAL_DEFAULT ||
                    OSPF_IF_PARAM(oi, transmit_delay) != OSPF_TRANSMIT_DELAY_DEFAULT)
			{
            	memset(vty_out_buf, 0, sizeof(vty_out_buf));
				sprintf(vty_out_buf, "  vlink-peer %s", inet_ntoa(vl_data->vl_peer));
            	if(OSPF_IF_PARAM(oi, v_hello) != OSPF_HELLO_INTERVAL_DEFAULT)
           		{
					sprintf((vty_out_buf + strlen(vty_out_buf)), " hello %d",
															OSPF_IF_PARAM(oi, v_hello));
				}

				if(OSPF_IF_PARAM(oi, v_wait) != OSPF_ROUTER_DEAD_INTERVAL_DEFAULT)
           		{
					sprintf((vty_out_buf + strlen(vty_out_buf)), " dead %d",
															OSPF_IF_PARAM(oi, v_wait));
				}
				/*if((OSPF_IF_PARAM(oi, v_hello) != OSPF_HELLO_INTERVAL_DEFAULT)\
					|| (OSPF_IF_PARAM(oi, v_wait) != OSPF_ROUTER_DEAD_INTERVAL_DEFAULT))
				{
                	vty_out(vty, "%s%s", vty_out_buf, VTY_NEWLINE);
				}*/
				
				if(OSPF_IF_PARAM(oi, retransmit_interval) != OSPF_RETRANSMIT_INTERVAL_DEFAULT)
           		{
           			//memset(vty_out_buf, 0, sizeof(vty_out_buf));
					//sprintf(vty_out_buf, "  vlink-peer %s", inet_ntoa(vl_data->vl_peer));
					sprintf((vty_out_buf + strlen(vty_out_buf)), " retransmit %d",
															OSPF_IF_PARAM(oi, retransmit_interval));
					//vty_out(vty, "%s%s", vty_out_buf, VTY_NEWLINE);
				}
				
				if(OSPF_IF_PARAM(oi, transmit_delay) != OSPF_TRANSMIT_DELAY_DEFAULT)
           		{
           			//memset(vty_out_buf, 0, sizeof(vty_out_buf));
					//sprintf(vty_out_buf, "  vlink-peer %s", inet_ntoa(vl_data->vl_peer));
					sprintf((vty_out_buf + strlen(vty_out_buf)), " transmit-delay %d",
															OSPF_IF_PARAM(oi, transmit_delay));
				}
				
				vty_out(vty, "%s%s", vty_out_buf, VTY_NEWLINE);
				flag++;
            }

			/* vlink authentication print */
			if (OSPF_IF_PARAM_CONFIGURED(params, auth_type)
					&& (params->auth_type != OSPF_AUTH_NOTSET))
            {
                /* Translation tables are not that much help here due to syntax
                   of the simple option */
                switch (params->auth_type)
                {
                    case OSPF_AUTH_NULL:
                        break;
                    case OSPF_AUTH_SIMPLE:
                        /* Simple Authentication Password print. */
                        if (OSPF_IF_PARAM_CONFIGURED(params, auth_simple) &&
                                params->auth_simple[0] != '\0')
                        {
                            vty_out(vty, "  vlink-peer %s simple plain %s%s",
				                        inet_ntoa(vl_data->vl_peer),
				                        params->auth_simple,
				                        VTY_NEWLINE);
                        }
                        break;
								
                    case OSPF_AUTH_CRYPTOGRAPHIC:
                        /* Cryptographic Authentication Key print. */
                        for (ALL_LIST_ELEMENTS_RO(params->auth_crypt, n2, ck))
                        {
                           vty_out(vty, "  vlink-peer %s"
				                        " md5 %d plain %s%s",
				                        inet_ntoa(vl_data->vl_peer),
				                        ck->key_id, ck->auth_key, VTY_NEWLINE);
                        }
                        break;
						
                    default:
                        break;
                }
				flag++;
            }

			if(!flag)
			{
                vty_out(vty, "  vlink-peer %s%s",
                        inet_ntoa(vl_data->vl_peer), VTY_NEWLINE);
			}
        }
    }

    return 0;
}


/* Configuration write function for ospfd. */
static int
ospf_config_write_interface(struct vty *vty)
{
    struct listnode *n1, *n2;
    struct interface *ifp;
    struct crypt_key *ck;
    int writeq = 0;
    struct route_node *rn = NULL;
    struct ospf_if_params *params;

    for (ALL_LIST_ELEMENTS_RO(iflist, n1, ifp))
    {
        if (memcmp(ifp->name, "VLINK", 5) == 0)
        {
            continue;
        }

        if (ifp->ifindex)
        {
            vty_out(vty, "!%s", VTY_NEWLINE);
            vty_out(vty, "interface %s%s", ifp->name,
                    VTY_NEWLINE);

            if (ifp->desc)
                vty_out(vty, " description %s%s", ifp->desc,
                        VTY_NEWLINE);

            writeq++;
            params = IF_DEF_PARAMS(ifp);

			if(params == NULL)
			{
				continue;
			}

            do
            {
                /* Interface Network print. */
                if (OSPF_IF_PARAM_CONFIGURED(params, type) &&
                        params->type != OSPF_IFTYPE_LOOPBACK)
                {
                    if (params->type != ospf_default_iftype(ifp))
                    {
                        vty_out(vty, " ip ospf network %s%s",
                                ospf_int_type_str[params->type], VTY_NEWLINE);
                    }

                    if (params->type == ospf_default_iftype(ifp))
                    {
                        if (OSPF_IF_PARAM_CONFIGURED(params, passive_interface))
                        {
                            vty_out(vty, " ip ospf network %s%s",
                                    ospf_int_type_str[params->type], VTY_NEWLINE);
                        }
                    }
                }

				/* Interface ospf bfd config print. */
				if (OSPF_IF_PARAM_CONFIGURED(params, bfd_flag))
				{
					if(params->bfd_flag == OSPF_IF_BFD_ENABLE)
					{
						vty_out (vty, " ip ospf bfd enable%s", VTY_NEWLINE);
					}
				}

				if (OSPF_IF_PARAM_CONFIGURED(params, if_bfd_send_interval))
				{
					vty_out (vty, " ip ospf bfd cc-interval %d%s", params->if_bfd_send_interval, VTY_NEWLINE);
				}

				if (OSPF_IF_PARAM_CONFIGURED(params, if_bfd_detect_multiplier))
				{
					vty_out (vty, " ip ospf bfd detect-multiplier %d%s", params->if_bfd_detect_multiplier, VTY_NEWLINE);
				}
				
                /* OSPF interface authentication print */
                if (OSPF_IF_PARAM_CONFIGURED(params, auth_type) &&
                        params->auth_type != OSPF_AUTH_NOTSET)
                {
                    /* Translation tables are not that much help here due to syntax
                       of the simple option */
                    switch (params->auth_type)
                    {
                        case OSPF_AUTH_NULL:
                            break;
                        case OSPF_AUTH_SIMPLE:
                            /* Simple Authentication Password print. */
                            if (OSPF_IF_PARAM_CONFIGURED(params, auth_simple) &&
                                    params->auth_simple[0] != '\0')
                            {
                                vty_out(vty, " ip ospf auth simple %s",
                                        params->auth_simple);
                                vty_out(vty, "%s", VTY_NEWLINE);
                            }

                            break;
                        case OSPF_AUTH_CRYPTOGRAPHIC:
                            /* Cryptographic Authentication Key print. */
                            for (ALL_LIST_ELEMENTS_RO(params->auth_crypt, n2, ck))
                            {
                                vty_out(vty, " ip ospf auth md5 %d %s",
                                        ck->key_id, ck->auth_key);
                                vty_out(vty, "%s", VTY_NEWLINE);
                            }

                            break;
                        default:
                            break;
                    }
                }

                /* Interface Output Cost print. */
                if (OSPF_IF_PARAM_CONFIGURED(params, output_cost_cmd))
                {
                    vty_out(vty, " ip ospf cost %u", params->output_cost_cmd);
                    vty_out(vty, "%s", VTY_NEWLINE);
                }

                if ((params->type == OSPF_IFTYPE_BROADCAST) || (params->type == OSPF_IFTYPE_POINTOPOINT)
                        || (params->type == OSPF_IFTYPE_LOOPBACK) || (params->type == OSPF_IFTYPE_POINTOMULTIPOINT))
                {
                    /* Hello Interval print. */
                    if (OSPF_IF_PARAM_CONFIGURED(params, v_hello) &&
                            params->v_hello != OSPF_HELLO_INTERVAL_DEFAULT)
                    {
                        vty_out(vty, " ip ospf hello-interval %u", params->v_hello);
                        vty_out(vty, "%s", VTY_NEWLINE);
                    }

                    /* Router Dead Interval print. */
                    if (OSPF_IF_PARAM_CONFIGURED(params, v_wait))
                    {
                        if (params->v_wait != 4*params->v_hello)
                        {
                            vty_out(vty, " ip ospf dead-interval ");
                            vty_out(vty, "%u", params->v_wait);
                            vty_out(vty, "%s", VTY_NEWLINE);
                        }
                    }
                }
                else if(params->type == OSPF_IFTYPE_NBMA)
                {
                    /* Hello Interval print. */
                    if (OSPF_IF_PARAM_CONFIGURED(params, v_hello) &&
                            params->v_hello != OSPF_HELLO_INTERVAL_NBMA_DEFAULT)
                    {
                        vty_out(vty, " ip ospf hello-interval %u", params->v_hello);
                        vty_out(vty, "%s", VTY_NEWLINE);
                    }

                    /* Router Dead Interval print. */
                    if (OSPF_IF_PARAM_CONFIGURED(params, v_wait))
                    {
                        if (params->v_wait != 4*params->v_hello)
                        {
                            vty_out(vty, " ip ospf dead-interval ");
                            vty_out(vty, "%u", params->v_wait);
                            vty_out(vty, "%s", VTY_NEWLINE);
                        }
                    }
                }

                /* Router Priority print. */
                if (OSPF_IF_PARAM_CONFIGURED(params, priority) &&
                        params->priority != OSPF_ROUTER_PRIORITY_DEFAULT)
                {
                    vty_out(vty, " ip ospf priority %u", params->priority);
                    vty_out(vty, "%s", VTY_NEWLINE);
                }

                /* Retransmit Interval print. */
                if (OSPF_IF_PARAM_CONFIGURED(params, retransmit_interval) &&
                        params->retransmit_interval != OSPF_RETRANSMIT_INTERVAL_DEFAULT)
                {
                    vty_out(vty, " ip ospf retransmit-interval %u",
                            params->retransmit_interval);
                    vty_out(vty, "%s", VTY_NEWLINE);
                }

                /* Transmit Delay print. */
                if (OSPF_IF_PARAM_CONFIGURED(params, transmit_delay) &&
                        params->transmit_delay != OSPF_TRANSMIT_DELAY_DEFAULT)
                {
                    vty_out(vty, " ip ospf transmit-delay %u", params->transmit_delay);
                    vty_out(vty, "%s", VTY_NEWLINE);
                }

                /* Area  print. */
                if (OSPF_IF_PARAM_CONFIGURED(params, if_area))
                {
                    vty_out(vty, " ip ospf area %s", inet_ntoa(params->if_area));
                    vty_out(vty, "%s", VTY_NEWLINE);
                }

                /* MTU enable print. */
                if (OSPF_IF_PARAM_CONFIGURED(params, mtu_ignore) &&
                        params->mtu_ignore != OSPF_MTU_IGNORE_DEFAULT)
                {
                    if (params->mtu_ignore == 0)
                    {
                        vty_out(vty, " ip ospf mtu-enable");
                    }
                    else
                    {
                        vty_out(vty, " no ip ospf mtu-enable");
                    }

                    vty_out(vty, "%s", VTY_NEWLINE);
                }

                if (params->ldp_enable == OSPF_IF_LDP_SYNC_ENABLE)
                {
                    vty_out(vty, " ip ospf ldp-sync%s", VTY_NEWLINE);
                }

                /* hold down interval print. */
                if (OSPF_IF_PARAM_CONFIGURED(params, v_holddown) &&
                        params->v_holddown != OSPF_SYNC_LDP_HOLD_DOWN_INTERVAL)
                {
                    vty_out(vty, " ip ospf ldp-sync hold-down interval %u", params->v_holddown);
                    vty_out(vty, "%s", VTY_NEWLINE);
                }

                /* hold max cost interval print. */
                if (OSPF_IF_PARAM_CONFIGURED(params, v_maxcost) &&
                        params->v_maxcost != OSPF_SYNC_LDP_MAX_COST_INTERVAL)
                {
                    vty_out(vty, " ip ospf ldp-sync hold-max-cost interval %u", params->v_maxcost);
                    vty_out(vty, "%s", VTY_NEWLINE);
                }

                /* static neighbor print. */

                //config_write_ospf_nbr_nbma (vty, ospf);
                while (1)
                {
                    if (rn == NULL)
                    {
                        rn = route_top(IF_OIFS_PARAMS(ifp));
                    }
                    else
                    {
                        rn = route_next(rn);
                    }

                    if (rn == NULL)
                    {
                        break;
                    }

                    params = rn->info;

                    if (params != NULL)
                    {
                        break;
                    }
                }
            }
            while (rn);

            ospf_opaque_config_write_if(vty, ifp);
        }
    }

    return writeq;
}

static int
config_write_network_area(struct vty *vty, struct ospf *ospf, struct ospf_area* area_buf)
{
    struct route_node *rn;
    u_char buf[INET_ADDRSTRLEN];

    /* `network area' print. */
    for (rn = route_top(ospf->networks); rn; rn = route_next(rn))
    {
        if (rn->info)
        {
            struct ospf_network *n = rn->info;
            memset(buf, 0, INET_ADDRSTRLEN);

            /* Create Area ID string by specified Area ID format. */
            if (n->format == OSPF_AREA_ID_FORMAT_ADDRESS)
            {
                strncpy((char *) buf, inet_ntoa(n->area_id), INET_ADDRSTRLEN);
            }
            else
                sprintf((char *) buf, "%lu",
                        (unsigned long int) ntohl(n->area_id.s_addr));

            /* Network print. */

            //if(inet_addr(buf) == inet_addr(area_buf))
            if (area_buf->area_id.s_addr == n->area_id.s_addr)
                vty_out(vty, "  network %s/%d %s",
                        inet_ntoa(rn->p.u.prefix4), rn->p.prefixlen,
                        VTY_NEWLINE);
        }
    }

    return 0;
}


static int
config_write_ospf_area(struct vty *vty, struct ospf *ospf)
{
    struct listnode *node;
    struct ospf_area *area;
    u_char buf[INET_ADDRSTRLEN];
	struct listnode *n2;
    struct crypt_key *ck;

    /* Area configuration print. */
    for (ALL_LIST_ELEMENTS_RO(ospf->areas, node, area))
    {
        struct route_node *rn1;
        area_id2str((char *) buf, INET_ADDRSTRLEN, area);
        vty_out(vty, " area %s%s", inet_ntoa(area->area_id), VTY_NEWLINE);
        /* Network area print. */
        config_write_network_area(vty, ospf, area);

		/* vlink config print. */
		config_write_virtual_link(vty, ospf, area);//add by zzl
		
        if (area->area_name != NULL)
        {
            vty_out(vty, "  name %s%s", area->area_name, VTY_NEWLINE);
        }

        if ((area->auth_type != OSPF_AUTH_NULL)
			&& (area->auth_type != OSPF_AUTH_NOTSET)
			&& (area->auth_type__config == AREA_AUTH_PARAM_CONFIG))
        {
        	switch (area->auth_type)
        	{
				case OSPF_AUTH_NULL:
					break;
					
                case OSPF_AUTH_SIMPLE:
                    /* Simple Authentication Password print. */
                    if ((area->auth_simple__config) &&
                            (area->auth_simple[0] != '\0'))
		            {
		                vty_out(vty, " authentication-mode simple plain %s%s", area->auth_simple, VTY_NEWLINE);
		            }
					break;
							
	            case OSPF_AUTH_CRYPTOGRAPHIC:
	        		/* Cryptographic Authentication Key print. */
	                for (ALL_LIST_ELEMENTS_RO(area->auth_crypt, n2, ck))
	                {
	                    vty_out(vty, " authentication-mode md5 %d plain %s%s",
	                    ck->key_id, ck->auth_key, VTY_NEWLINE);
	                }
					break;
					
				default:
					break;
			}
        }

        if (area->shortcut_configured != OSPF_SHORTCUT_DEFAULT)
            vty_out(vty, " area %s shortcut %s%s", buf,
                    ospf_shortcut_mode_str[area->shortcut_configured],
                    VTY_NEWLINE);

        if ((area->external_routing == OSPF_AREA_STUB)
                || (area->external_routing == OSPF_AREA_NSSA))
        {
            if (area->external_routing == OSPF_AREA_STUB)
            {
                vty_out(vty, "  stub");
            }
            else if (area->external_routing == OSPF_AREA_NSSA)
            {
                vty_out(vty, "  nssa");
            }

            if (area->no_summary)
            {
                vty_out(vty, " no-summary");
            }

            vty_out(vty, "%s", VTY_NEWLINE);

            if (area->default_cost != 1)
                vty_out(vty, "  default-cost %d%s",
                        area->default_cost, VTY_NEWLINE);
        }

        for (rn1 = route_top(area->ranges); rn1; rn1 = route_next(rn1))
        {
            if (rn1->info)
            {
                struct ospf_area_range *range = rn1->info;
                vty_out(vty, "  abr-summary %s/%d",
                        inet_ntoa(rn1->p.u.prefix4), rn1->p.prefixlen);

                if (range->cost_config != OSPF_AREA_RANGE_COST_UNSPEC)
                {
                    vty_out(vty, " cost %d", range->cost_config);
                }

                if (!CHECK_FLAG(range->flags, OSPF_AREA_RANGE_ADVERTISE))
                {
                    vty_out(vty, " not-advertise");
                }

                if (CHECK_FLAG(range->flags, OSPF_AREA_RANGE_SUBSTITUTE))
                    vty_out(vty, " substitute %s/%d",
                            inet_ntoa(range->subst_addr), range->subst_masklen);

                vty_out(vty, "%s", VTY_NEWLINE);
            }
        }
    }

    return 0;
}

static int
config_write_ospf_redistribute(struct vty *vty, struct ospf *ospf)
{
    int type, instance;
    struct ospf_route_redist *ospf_route_redist = NULL;
    struct listnode *node = NULL;

    /* redistribute print. */
    for (type = 0; type < ROUTE_PROTO_MAX; type++)
    {
        for (instance = 0; instance <= OSPF_MAX_PROCESS_ID; instance++)
        {
            if (type == ROUTE_PROTO_OSPF && instance == ospf->ospf_id)
            {
                continue;
            }

            if (vrf_bitmap_check(ospf->dmetric[type][instance].redist, VRF_DEFAULT))
            {
                if ((type == ROUTE_PROTO_CONNECT && ospf->route_policy_flag == 1)
                        || type != ROUTE_PROTO_CONNECT)
                {
                    vty_out(vty, " redistribute %s", zebra_route_string(type));

                    if (instance != 0)
                    {
                        vty_out(vty, " %d", instance);
                    }

                    if (ospf->dmetric[type][instance].value >= 0)
                    {
                        vty_out(vty, " metric %d", ospf->dmetric[type][instance].value);
                    }

                    if (ospf->dmetric[type][instance].type == EXTERNAL_METRIC_TYPE_1)
                    {
                        vty_out(vty, " metric-type 1");
                    }

                    vty_out(vty, "%s", VTY_NEWLINE);
                }

                if (type == ROUTE_PROTO_CONNECT)
                {
                    for (ALL_LIST_ELEMENTS_RO(ospf->redist_name, node, ospf_route_redist))
                    {
                        vty_out(vty, " redistribute connected route-policy %s %s",
                                ospf_route_redist->if_name, VTY_NEWLINE);
                    }
                }


            }
        }
    }


    return 0;
}

static int
config_write_ospf_default_metric(struct vty *vty, struct ospf *ospf)
{
    if (ospf->default_metric != -1)
        vty_out(vty, " default-metric %d%s", ospf->default_metric,
                VTY_NEWLINE);

    return 0;
}

static int
config_write_ospf_distribute(struct vty *vty, struct ospf *ospf)
{
    int type;

    if (ospf)
    {
        /* distribute-list print. */
        for (type = 0; type < ROUTE_PROTO_MAX; type++)
        {
            if (DISTRIBUTE_NAME(ospf, type))
            {
                vty_out(vty, " distribute-list %s out %s%s",
                        DISTRIBUTE_NAME(ospf, type),
                        zebra_route_string(type), VTY_NEWLINE);
            }
        }

        /* default-information print. */
        if (ospf->default_originate != DEFAULT_ORIGINATE_NONE)
        {
            vty_out(vty, " default-information originate");

            if (ospf->default_originate == DEFAULT_ORIGINATE_ALWAYS)
            {
                vty_out(vty, " always");
            }

            if (ospf->dmetric[DEFAULT_ROUTE][0].value >= 0)
                vty_out(vty, " metric %d",
                        ospf->dmetric[DEFAULT_ROUTE][0].value);

            if (ospf->dmetric[DEFAULT_ROUTE][0].type == EXTERNAL_METRIC_TYPE_1)
            {
                vty_out(vty, " metric-type 1");
            }

            if (ROUTEMAP_NAME(ospf, DEFAULT_ROUTE))
                vty_out(vty, " route-map %s",
                        ROUTEMAP_NAME(ospf, DEFAULT_ROUTE));

            vty_out(vty, "%s", VTY_NEWLINE);
        }
    }

    return 0;
}

static int
config_write_ospf_distance(struct vty *vty, struct ospf *ospf)
{
    struct route_node *rn;
    struct ospf_distance *odistance;

    if (ospf->distance_all != ROUTE_METRIC_OSPF)
    {
        vty_out(vty, " distance %d%s", ospf->distance_all, VTY_NEWLINE);
    }

    if (ospf->distance_intra
            || ospf->distance_inter
            || ospf->distance_external)
    {
        vty_out(vty, " distance");

        if (ospf->distance_intra)
        {
            vty_out(vty, " intra-area %d", ospf->distance_intra);
        }

        if (ospf->distance_inter)
        {
            vty_out(vty, " inter-area %d", ospf->distance_inter);
        }

        if (ospf->distance_external)
        {
            vty_out(vty, " external %d", ospf->distance_external);
        }

        vty_out(vty, "%s", VTY_NEWLINE);
    }

    for (rn = route_top(ospf->distance_table); rn; rn = route_next(rn))
    {
        if ((odistance = rn->info) != NULL)
        {
            vty_out(vty, " distance %d %s/%d %s%s", odistance->distance,
                    inet_ntoa(rn->p.u.prefix4), rn->p.prefixlen,
                    odistance->access_list ? odistance->access_list : "",
                    VTY_NEWLINE);
        }
    }

    return 0;
}

/* OSPF configuration write function. */
static int
ospf_config_write(struct vty *vty)
{
    struct ospf *ospf;
    struct listnode *node, *nnode;
    int writeq = 0;
    struct route_node *rn1;

    for (ALL_LIST_ELEMENTS(om->ospf, node, nnode, ospf))
    {
        if (ospf != NULL && ospf->ospf_id != 0)
        {
            /* `router ospf' print. */
            if (ospf->vpn == 0)
                vty_out(vty, "ospf instance %d %s", ospf->ospf_id, VTY_NEWLINE);
            else
                vty_out(vty, "ospf instance %d vpn-instance %d %s", ospf->ospf_id, ospf->vpn, VTY_NEWLINE);

            writeq++;

            if (!ospf->networks)
            {
                return writeq;
            }

            /* Router ID print. */
            if (ospf->router_id_static.s_addr != 0)
                vty_out(vty, " router-id %s%s",
                        inet_ntoa(ospf->router_id_static), VTY_NEWLINE);

            /* ABR type print. */
            if (ospf->abr_type != OSPF_ABR_DEFAULT)
                vty_out(vty, " ospf abr-type %s%s",
                        ospf_abr_type_str[ospf->abr_type], VTY_NEWLINE);

            /* log-adjacency-changes flag print. */
            if (CHECK_FLAG(ospf->config, OSPF_LOG_ADJACENCY_CHANGES))
            {
                vty_out(vty, " log-adjacency-changes");

                if (CHECK_FLAG(ospf->config, OSPF_LOG_ADJACENCY_DETAIL))
                {
                    vty_out(vty, " detail");
                }

                vty_out(vty, "%s", VTY_NEWLINE);
            }

            /* RFC1583 compatibility flag print -- Compatible with CISCO 12.1. */
            if (!CHECK_FLAG(ospf->config, OSPF_RFC1583_COMPATIBLE))
            {
                vty_out(vty, " no compatible rfc1583%s", VTY_NEWLINE);
            }

            /* auto-cost reference-bandwidth configuration.  */
            if (ospf->ref_bandwidth != OSPF_DEFAULT_REF_BANDWIDTH)
            {
                vty_out(vty, "! Important: ensure reference bandwidth "
                        "is consistent across all routers%s", VTY_NEWLINE);
                vty_out(vty, " reference-bandwidth %d%s",
                        ospf->ref_bandwidth / 1000, VTY_NEWLINE);
            }

            /* LSA timers */
            if (ospf->min_ls_interval != OSPF_MIN_LS_INTERVAL)
                vty_out(vty, " timers throttle lsa all %d%s",
                        ospf->min_ls_interval, VTY_NEWLINE);

            if (ospf->min_ls_arrival != OSPF_MIN_LS_ARRIVAL)
                vty_out(vty, " timers lsa arrival %d%s",
                        ospf->min_ls_arrival, VTY_NEWLINE);

            /* SPF timers print. */
            if (ospf->spf_delay != OSPF_SPF_DELAY_DEFAULT ||
                    ospf->spf_holdtime != OSPF_SPF_HOLDTIME_DEFAULT ||
                    ospf->spf_max_holdtime != OSPF_SPF_MAX_HOLDTIME_DEFAULT)
                vty_out(vty, " timers throttle spf %d %d %d%s",
                        ospf->spf_delay, ospf->spf_holdtime,
                        ospf->spf_max_holdtime, VTY_NEWLINE);

            /* Max-metric router-lsa print */
            config_write_stub_router(vty, ospf);

            /* SPF refresh parameters print. */
            if (ospf->refresh_interval != OSPF_LS_REFRESH_TIME)
                vty_out(vty, " refresh interval %d%s",
                        ospf->refresh_interval, VTY_NEWLINE);

            /* Redistribute information print. */
            config_write_ospf_redistribute(vty, ospf);

            for (rn1 = route_top(ospf->summary); rn1; rn1 = route_next(rn1))
            {
                if (rn1->info)
                {
                    struct ospf_summary *summary = rn1->info;
                    vty_out(vty, " asbr-summary %s/%d",
                            inet_ntoa(rn1->p.u.prefix4), rn1->p.prefixlen);

                    if (summary->cost_config != OSPF_SUMMARY_COST_UNSPEC)
                    {
                        vty_out(vty, " cost %d", summary->cost_config);
                    }

                    if (!CHECK_FLAG(summary->flags, OSPF_SUMMARY_ADVERTISE))
                    {
                        vty_out(vty, " not-advertise");
                    }

                    vty_out(vty, "%s", VTY_NEWLINE);
                }
            }
          
			/* ospf bfd all-interfaces config print.*/
			if(ospf->all_interfaces_bfd_enable == OSPF_ALL_IF_BFD_ENABLE)
			{
				vty_out (vty, " bfd all-interfaces enable%s", VTY_NEWLINE);
			}
            /* Network area print. */
            //config_write_network_area (vty, ospf);
            /* static neighbor print. */
            config_write_ospf_nbr_nbma(vty, ospf);
            /* Virtual-Link print. */
            //config_write_virtual_link(vty, ospf);//move to config_write_ospf_area()
            /* Default metric configuration.  */
            config_write_ospf_default_metric(vty, ospf);
            /* Distribute-list and default-information print. */
            config_write_ospf_distribute(vty, ospf);
            /* Distance configuration. */
            config_write_ospf_distance(vty, ospf);
            ospf_opaque_config_write_router(vty, ospf);
            /* Area config print. */
            config_write_ospf_area(vty, ospf);
        }
    }

    return writeq;
}

static int
ospf_config_write_for_factory(struct vty *vty)
{
    struct ospf *ospf;
    struct listnode *node, *nnode;
    int writeq = 0;
    struct route_node *rn1;

    for (ALL_LIST_ELEMENTS(om->ospf, node, nnode, ospf))
    {
        if (ospf != NULL && ospf->ospf_id == OSPF_MAX_PROCESS_ID && ospf->dcn_enable)
        {
            /* `router ospf' print. */
            if (ospf->vpn == 0)
                vty_out(vty, "ospf instance %d %s", ospf->ospf_id, VTY_NEWLINE);
            else
                vty_out(vty, "ospf instance %d vpn-instance %d %s", ospf->ospf_id, ospf->vpn, VTY_NEWLINE);

            writeq++;

            if (!ospf->networks)
            {
                return writeq;
            }

            /* Router ID print. */
            if (ospf->router_id_static.s_addr != 0)
                vty_out(vty, " router-id %s%s",
                        inet_ntoa(ospf->router_id_static), VTY_NEWLINE);

            /* ABR type print. */
            if (ospf->abr_type != OSPF_ABR_DEFAULT)
                vty_out(vty, " ospf abr-type %s%s",
                        ospf_abr_type_str[ospf->abr_type], VTY_NEWLINE);

            /* log-adjacency-changes flag print. */
            if (CHECK_FLAG(ospf->config, OSPF_LOG_ADJACENCY_CHANGES))
            {
                vty_out(vty, " log-adjacency-changes");

                if (CHECK_FLAG(ospf->config, OSPF_LOG_ADJACENCY_DETAIL))
                {
                    vty_out(vty, " detail");
                }

                vty_out(vty, "%s", VTY_NEWLINE);
            }

            /* RFC1583 compatibility flag print -- Compatible with CISCO 12.1. */
            if (!CHECK_FLAG(ospf->config, OSPF_RFC1583_COMPATIBLE))
            {
                vty_out(vty, " no compatible rfc1583%s", VTY_NEWLINE);
            }

            /* auto-cost reference-bandwidth configuration.  */
            if (ospf->ref_bandwidth != OSPF_DEFAULT_REF_BANDWIDTH)
            {
                vty_out(vty, "! Important: ensure reference bandwidth "
                        "is consistent across all routers%s", VTY_NEWLINE);
                vty_out(vty, " reference-bandwidth %d%s",
                        ospf->ref_bandwidth / 1000, VTY_NEWLINE);
            }

            /* LSA timers */
            if (ospf->min_ls_interval != OSPF_MIN_LS_INTERVAL)
                vty_out(vty, " timers throttle lsa all %d%s",
                        ospf->min_ls_interval, VTY_NEWLINE);

            if (ospf->min_ls_arrival != OSPF_MIN_LS_ARRIVAL)
                vty_out(vty, " timers lsa arrival %d%s",
                        ospf->min_ls_arrival, VTY_NEWLINE);

            /* SPF timers print. */
            if (ospf->spf_delay != OSPF_SPF_DELAY_DEFAULT ||
                    ospf->spf_holdtime != OSPF_SPF_HOLDTIME_DEFAULT ||
                    ospf->spf_max_holdtime != OSPF_SPF_MAX_HOLDTIME_DEFAULT)
                vty_out(vty, " timers throttle spf %d %d %d%s",
                        ospf->spf_delay, ospf->spf_holdtime,
                        ospf->spf_max_holdtime, VTY_NEWLINE);

            /* Max-metric router-lsa print */
            config_write_stub_router(vty, ospf);

            /* SPF refresh parameters print. */
            if (ospf->refresh_interval != OSPF_LS_REFRESH_TIME)
                vty_out(vty, " refresh interval %d%s",
                        ospf->refresh_interval, VTY_NEWLINE);

            /* Redistribute information print. */
            config_write_ospf_redistribute(vty, ospf);

            for (rn1 = route_top(ospf->summary); rn1; rn1 = route_next(rn1))
            {
                if (rn1->info)
                {
                    struct ospf_summary *summary = rn1->info;
                    vty_out(vty, " asbr-summary %s/%d",
                            inet_ntoa(rn1->p.u.prefix4), rn1->p.prefixlen);

                    if (summary->cost_config != OSPF_SUMMARY_COST_UNSPEC)
                    {
                        vty_out(vty, " cost %d", summary->cost_config);
                    }

                    if (!CHECK_FLAG(summary->flags, OSPF_SUMMARY_ADVERTISE))
                    {
                        vty_out(vty, " not-advertise");
                    }

                    vty_out(vty, "%s", VTY_NEWLINE);
                }
            }
          
			/* ospf bfd all-interfaces config print.*/
			if(ospf->all_interfaces_bfd_enable == OSPF_ALL_IF_BFD_ENABLE)
			{
				vty_out (vty, " bfd all-interfaces enable%s", VTY_NEWLINE);
			}
            /* Network area print. */
            //config_write_network_area (vty, ospf);
            /* static neighbor print. */
            config_write_ospf_nbr_nbma(vty, ospf);
            /* Virtual-Link print. */
            //config_write_virtual_link(vty, ospf);//move to config_write_ospf_area()
            /* Default metric configuration.  */
            config_write_ospf_default_metric(vty, ospf);
            /* Distribute-list and default-information print. */
            config_write_ospf_distribute(vty, ospf);
            /* Distance configuration. */
            config_write_ospf_distance(vty, ospf);
            ospf_opaque_config_write_router(vty, ospf);
            /* Area config print. */
            config_write_ospf_area(vty, ospf);
        }
    }

    return writeq;
}


/* OSPF dcn configuration write function. */
static int
ospf_dcn_config_write(struct vty *vty)
{
    struct ospf *ospf = NULL;
	int ospf_id = OSPF_MAX_PROCESS_ID;
    int writeq = 0;

	ospf = ospf_lookup_id(ospf_id);
    if(ospf != NULL)
    {
		if(ospf->dcn_enable == OSPF_DCN_ENABLE)
		{
			vty_out(vty, "dcn %s", VTY_NEWLINE);
			
			writeq++;

			//不要改变这里的命令顺序，不然设备启动时恢复配置有问题。因为有依赖关系。
			if(om->ne_id_static.s_addr != 0)
			{
				vty_out(vty, " ne-id 0x%x %s", ntohl(om->ne_id_static.s_addr), VTY_NEWLINE);
			}
			if(om->ne_ip_static.s_addr != 0)
			{
				vty_out(vty, " ne-ip %s/%d %s", inet_ntoa(om->ne_ip_static), om->ne_ip_static_prefixlen, VTY_NEWLINE);	
			}
						
			if(ospf->dcn_report == OSPF_DCN_REPORT_ENABLE)
			{
				vty_out(vty, " auto-report %s", VTY_NEWLINE);
			}

		}
    }

    return writeq;
}


void
ospf_vty_show_init(void)
{
    /* "show ip ospf refresh list" commands. */
    install_element(OSPF_NODE, &show_ip_ospf_refesh_list_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ip_ospf_refesh_list_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &show_ip_ospf_refesh_list_cmd, CMD_LOCAL);

	/* "show ip ospf refresh list" commands for h3c. */
    install_element(OSPF_NODE, &display_ospf_refesh_list_h3c_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &display_ospf_refesh_list_h3c_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &display_ospf_refesh_list_h3c_cmd, CMD_LOCAL);
	
    /* "show ip ospf" commands. */
    install_element(OSPF_NODE, &show_ip_ospf_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ip_ospf_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &show_ip_ospf_cmd, CMD_LOCAL);
    install_element(OSPF_NODE, &show_ip_ospf_lsdb_all_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ip_ospf_lsdb_all_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &show_ip_ospf_lsdb_all_cmd, CMD_LOCAL);
    install_element(OSPF_NODE, &show_ip_ospf_lsdb_self_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ip_ospf_lsdb_self_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &show_ip_ospf_lsdb_self_cmd, CMD_LOCAL);

	/* "show ip ospf instance info" commands for h3c. */
	install_element(OSPF_NODE, &show_ip_ospf_h3c_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ip_ospf_h3c_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &show_ip_ospf_h3c_cmd, CMD_LOCAL);
	install_element(OSPF_NODE, &show_ip_ospf_h3c_format_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ip_ospf_h3c_format_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &show_ip_ospf_h3c_format_cmd, CMD_LOCAL);

	/* "show ip ospf lsdb info" commands for h3c. */
	install_element(OSPF_NODE, &display_ospf_lsdb_all_h3c_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &display_ospf_lsdb_all_h3c_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &display_ospf_lsdb_all_h3c_cmd, CMD_LOCAL);
	install_element(OSPF_NODE, &display_ospf_lsdb_all_h3c_format2_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &display_ospf_lsdb_all_h3c_format2_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &display_ospf_lsdb_all_h3c_format2_cmd, CMD_LOCAL);
	install_element(OSPF_NODE, &display_ospf_lsdb_detail_h3c_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &display_ospf_lsdb_detail_h3c_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &display_ospf_lsdb_detail_h3c_cmd, CMD_LOCAL);
	
    /* "show ip ospf interface" commands. */
    install_element(OSPF_NODE, &show_ip_ospf_interface_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ip_ospf_interface_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &show_ip_ospf_interface_cmd, CMD_LOCAL);    
    install_element(OSPF_NODE, &show_ip_ospf_interface_ifname_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ip_ospf_interface_ifname_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &show_ip_ospf_interface_ifname_cmd, CMD_LOCAL);
	install_element(OSPF_NODE, &show_ip_ospf_interface_trunk_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ip_ospf_interface_trunk_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &show_ip_ospf_interface_trunk_cmd, CMD_LOCAL);
	install_element(OSPF_NODE, &show_ip_ospf_interface_vlanif_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ip_ospf_interface_vlanif_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &show_ip_ospf_interface_vlanif_cmd, CMD_LOCAL);
	install_element(OSPF_NODE, &show_ip_ospf_interface_loopback_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ip_ospf_interface_loopback_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &show_ip_ospf_interface_loopback_cmd, CMD_LOCAL);

	/* "show ip ospf interface" commands for h3c. */
    install_element(OSPF_NODE, &display_ospf_interface_h3c_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &display_ospf_interface_h3c_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &display_ospf_interface_h3c_cmd, CMD_LOCAL);
	install_element(OSPF_NODE, &display_ospf_interface_ifname_h3c_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &display_ospf_interface_ifname_h3c_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &display_ospf_interface_ifname_h3c_cmd, CMD_LOCAL);
	install_element(OSPF_NODE, &display_ospf_if_trunk_h3c_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &display_ospf_if_trunk_h3c_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &display_ospf_if_trunk_h3c_cmd, CMD_LOCAL);
	install_element(OSPF_NODE, &display_ospf_if_vlanif_h3c_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &display_ospf_if_vlanif_h3c_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &display_ospf_if_vlanif_h3c_cmd, CMD_LOCAL);
	install_element(OSPF_NODE, &display_ospf_if_looplack_h3c_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &display_ospf_if_looplack_h3c_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &display_ospf_if_looplack_h3c_cmd, CMD_LOCAL);
	
    /* "show ip ospf neighbor" commands. */
    install_element(OSPF_NODE, &show_ip_ospf_neighbor_int_detail_cmd, CMD_LOCAL);
    install_element(OSPF_NODE, &show_ip_ospf_neighbor_int_gigabitethernet_detail_cmd, CMD_LOCAL);
    install_element(OSPF_NODE, &show_ip_ospf_neighbor_int_xgigabitethernet_detail_cmd, CMD_LOCAL);
    install_element(OSPF_NODE, &show_ip_ospf_neighbor_id_cmd, CMD_LOCAL);
    install_element(OSPF_NODE, &show_ip_ospf_neighbor_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ip_ospf_neighbor_int_detail_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ip_ospf_neighbor_int_gigabitethernet_detail_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ip_ospf_neighbor_int_xgigabitethernet_detail_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ip_ospf_neighbor_id_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ip_ospf_neighbor_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &show_ip_ospf_neighbor_int_detail_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &show_ip_ospf_neighbor_int_gigabitethernet_detail_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &show_ip_ospf_neighbor_int_xgigabitethernet_detail_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &show_ip_ospf_neighbor_id_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &show_ip_ospf_neighbor_cmd, CMD_LOCAL);
	

	/* "show ip ospf neighbor" commands for h3c. */
	install_element(OSPF_NODE, &show_ip_ospf_neighbor_h3c_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &show_ip_ospf_neighbor_h3c_cmd, CMD_LOCAL);
	install_element(AREA_NODE, &show_ip_ospf_neighbor_h3c_cmd, CMD_LOCAL);

	install_element(OSPF_NODE, &show_ip_ospf_neighbor_id_h3c_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &show_ip_ospf_neighbor_id_h3c_cmd, CMD_LOCAL);
	install_element(AREA_NODE, &show_ip_ospf_neighbor_id_h3c_cmd, CMD_LOCAL);
	
	install_element(OSPF_NODE, &show_ip_ospf_neighbor_by_intface_detail_h3c_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &show_ip_ospf_neighbor_by_intface_detail_h3c_cmd, CMD_LOCAL);
	install_element(AREA_NODE, &show_ip_ospf_neighbor_by_intface_detail_h3c_cmd, CMD_LOCAL);
	
    /* "show ip ospf route" commands. */
    install_element(OSPF_NODE, &show_ip_ospf_route_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ip_ospf_route_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &show_ip_ospf_route_cmd, CMD_LOCAL);
    install_element(OSPF_NODE, &show_ip_ospf_border_routers_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ip_ospf_border_routers_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &show_ip_ospf_border_routers_cmd, CMD_LOCAL);
    install_element(OSPF_NODE, &show_ip_ospf_statics_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ip_ospf_statics_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &show_ip_ospf_statics_cmd, CMD_LOCAL);

	/* "show ip ospf route" commands for h3c. */
	install_element(OSPF_NODE, &display_ospf_route_h3c_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &display_ospf_route_h3c_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &display_ospf_route_h3c_cmd, CMD_LOCAL);

	/* "show ip ospf statics" commands for h3c. */
	install_element(OSPF_NODE, &display_ospf_statics_h3c_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &display_ospf_statics_h3c_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &display_ospf_statics_h3c_cmd, CMD_LOCAL);
	
    /* "show ospf dcn info" commands. */
    install_element(OSPF_NODE, &show_ip_ospf_dcn_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ip_ospf_dcn_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &show_ip_ospf_dcn_cmd, CMD_LOCAL);
	install_element (OSPF_DCN_NODE, &show_ip_ospf_dcn_cmd, CMD_LOCAL);

	install_element(CONFIG_NODE, &dcn_u0_test_show_cmd, CMD_LOCAL);

	/* "show ospf dcn info" commands for h3c. */
	install_element(OSPF_NODE, &display_ip_ospf_dcn_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &display_ip_ospf_dcn_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &display_ip_ospf_dcn_cmd, CMD_LOCAL);
	install_element (OSPF_DCN_NODE, &display_ip_ospf_dcn_cmd, CMD_LOCAL);
	
    /* "show ip ospf err" */
    install_element(OSPF_NODE, &show_ip_ospf_error_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ip_ospf_error_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &show_ip_ospf_error_cmd, CMD_LOCAL);

	/* "show ip ospf err" for h3c*/
    install_element(OSPF_NODE, &display_ospf_statistics_error_h3c_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &display_ospf_statistics_error_h3c_cmd, CMD_LOCAL);
    install_element(AREA_NODE, &display_ospf_statistics_error_h3c_cmd, CMD_LOCAL);
	
	/* "show ospf bfd info" */
	install_element(CONFIG_NODE, &ospf_bfd_info_show_cmd, CMD_LOCAL);
	install_element(OSPF_NODE, &ospf_bfd_info_show_cmd, CMD_LOCAL);
	install_element(AREA_NODE, &ospf_bfd_info_show_cmd, CMD_LOCAL);

	/* "show ospf bfd info for h3c" */
	install_element(CONFIG_NODE, &display_ospf_bfd_info_h3c_cmd, CMD_LOCAL);
	install_element(OSPF_NODE, &display_ospf_bfd_info_h3c_cmd, CMD_LOCAL);
	install_element(AREA_NODE, &display_ospf_bfd_info_h3c_cmd, CMD_LOCAL);
	
	/* "show ospf vlink info" */
	install_element(CONFIG_NODE, &show_ip_ospf_vlink_cmd, CMD_LOCAL);
	install_element(OSPF_NODE, &show_ip_ospf_vlink_cmd, CMD_LOCAL);
	install_element(AREA_NODE, &show_ip_ospf_vlink_cmd, CMD_LOCAL);
	
	/* "show ospf vlink info" for h3c */
	install_element(CONFIG_NODE, &show_ip_ospf_vlink_h3c_cmd, CMD_LOCAL);
	install_element(OSPF_NODE, &show_ip_ospf_vlink_h3c_cmd, CMD_LOCAL);
	install_element(AREA_NODE, &show_ip_ospf_vlink_h3c_cmd, CMD_LOCAL);

}


/*ospf enable cmd install in L3if NODE */
static void ospf_l3if_init(void)
{
    ifm_l3if_init(ospf_config_write_interface);
    /* "ip ospf network" commands. */
    install_element(PHYSICAL_SUBIF_NODE, &ip_ospf_network_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ip_ospf_network_cmd, CMD_SYNC);
	/* "ip ospf network" commands for h3c. */
    install_element(PHYSICAL_SUBIF_NODE, &ip_ospf_network_h3c_cmd, CMD_SYNC);
	install_element(PHYSICAL_SUBIF_NODE, &ip_ospf_network_p2mp_h3c_cmd, CMD_SYNC);
	install_element(PHYSICAL_SUBIF_NODE, &ip_ospf_network_p2p_h3c_cmd, CMD_SYNC);
	install_element(PHYSICAL_SUBIF_NODE, &no_ip_ospf_network_h3c_cmd, CMD_SYNC);
    /* "ip ospf mtu-ignore" commands. */
    install_element(PHYSICAL_SUBIF_NODE, &ip_ospf_mtu_ignore_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ip_ospf_mtu_ignore_cmd, CMD_SYNC);
	/* "ip ospf mtu-ignore" commands for h3c. */
    install_element(PHYSICAL_SUBIF_NODE, &ip_ospf_mtu_ignore_h3c_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ip_ospf_mtu_ignore_h3c_cmd, CMD_SYNC);
    /* "description" commands. */
    //install_element (PHYSICAL_SUBIF_NODE, &interface_desc_cmd, CMD_SYNC);
    //install_element (PHYSICAL_SUBIF_NODE, &no_interface_desc_cmd, CMD_SYNC);
    /* "ip ospf authentication simple" commands. */
    install_element(PHYSICAL_SUBIF_NODE, &ip_ospf_auth_simple_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ip_ospf_auth_cmd, CMD_SYNC);
    /* "ip ospf auth md5" commands. */
    install_element(PHYSICAL_SUBIF_NODE, &ip_ospf_auth_md5_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ip_ospf_auth_md5_cmd, CMD_SYNC);
	/* "ip ospf auth" commands for h3c. */
    install_element(PHYSICAL_SUBIF_NODE, &ospf_interface_authentication_h3c_cmd, CMD_SYNC);
	install_element(PHYSICAL_SUBIF_NODE, &ospf_interface_authentication_md5_h3c_cmd, CMD_SYNC);
	install_element(PHYSICAL_SUBIF_NODE, &no_ospf_interface_authentication_h3c_cmd, CMD_SYNC);
	install_element(PHYSICAL_SUBIF_NODE, &undo_ospf_interface_simple_authentication_h3c_cmd, CMD_SYNC);
	install_element(PHYSICAL_SUBIF_NODE, &undo_ospf_interface_md5_authentication_h3c_cmd, CMD_SYNC);
	install_element(PHYSICAL_SUBIF_NODE, &undo_ospf_interface_keychain_authentication_h3c_cmd, CMD_SYNC);

    /* "ip ospf cost" commands. */
    install_element(PHYSICAL_SUBIF_NODE, &ip_ospf_cost_u32_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ip_ospf_cost_cmd, CMD_SYNC);
	/* "ip ospf cost" commands for h3c. */
	install_element(PHYSICAL_SUBIF_NODE, &ip_ospf_cost_u32_h3c_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ip_ospf_cost_h3c_cmd, CMD_SYNC);
    /* "ip ospf dead-interval" commands. */
    install_element(PHYSICAL_SUBIF_NODE, &ip_ospf_dead_interval_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ip_ospf_dead_interval_cmd, CMD_SYNC);
	/* "ip ospf dead-interval" commands for h3c. */
    install_element(PHYSICAL_SUBIF_NODE, &ip_ospf_dead_interval_h3c_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ip_ospf_dead_interval_h3c_cmd, CMD_SYNC);
    /* "ip ospf hello-interval" commands. */
    install_element(PHYSICAL_SUBIF_NODE, &ip_ospf_hello_interval_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ip_ospf_hello_interval_cmd, CMD_SYNC);
	/* "ip ospf hello-interval" commands for h3c. */
    install_element(PHYSICAL_SUBIF_NODE, &ip_ospf_hello_interval_h3c_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ip_ospf_hello_interval_h3c_cmd, CMD_SYNC);
    /* "ip ospf priority" commands. */
    install_element(PHYSICAL_SUBIF_NODE, &ip_ospf_priority_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ip_ospf_priority_cmd, CMD_SYNC);
	/* "ip ospf priority" commands for h3c. */
    install_element(PHYSICAL_SUBIF_NODE, &ip_ospf_priority_h3c_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ip_ospf_priority_h3c_cmd, CMD_SYNC);
    /* "ip ospf retransmit-interval" commands. */
    install_element(PHYSICAL_SUBIF_NODE, &ip_ospf_retransmit_interval_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ip_ospf_retransmit_interval_cmd, CMD_SYNC);
	/* "ip ospf retransmit-interval" commands for h3c. */
    install_element(PHYSICAL_SUBIF_NODE, &ip_ospf_retransmit_interval_h3c_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ip_ospf_retransmit_interval_h3c_cmd, CMD_SYNC);
    /* "ip ospf transmit-delay" commands. */
    install_element(PHYSICAL_SUBIF_NODE, &ip_ospf_transmit_delay_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ip_ospf_transmit_delay_cmd, CMD_SYNC);
    /* "ip ospf area" commands. */
    install_element(PHYSICAL_SUBIF_NODE, &ip_ospf_area_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ip_ospf_area_cmd, CMD_SYNC);
    /* "ip ospf ldp-sync" commands. */
    install_element(PHYSICAL_SUBIF_NODE, &ip_ospf_ldp_sync_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ip_ospf_ldp_sync_cmd, CMD_SYNC);
    /* "ip ospf ldp-sync hold down" commands. */
    install_element(PHYSICAL_SUBIF_NODE, &ip_ospf_ldp_hold_down_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ip_ospf_ldp_hold_down_cmd, CMD_SYNC);
    /* "ip ospf ldp-sync hold-max-cost" commands. */
    install_element(PHYSICAL_SUBIF_NODE, &ip_ospf_ldp_max_cost_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &ip_ospf_ldp_max_cost_infinite_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ip_ospf_ldp_max_cost_cmd, CMD_SYNC);
	/* "ospf bfd" command. */
	install_element(PHYSICAL_SUBIF_NODE, &ospf_bfd_cmd, CMD_SYNC);
	install_element(PHYSICAL_SUBIF_NODE, &no_ospf_bfd_cmd, CMD_SYNC);
	
	install_element(PHYSICAL_SUBIF_NODE, &ospf_bfd_sess_cc_interval_set_cmd, CMD_SYNC);
	install_element(PHYSICAL_SUBIF_NODE, &ospf_bfd_sess_cc_interval_reset_cmd, CMD_SYNC);
	
	install_element(PHYSICAL_SUBIF_NODE, &ospf_bfd_sess_detect_multiplier_set_cmd, CMD_SYNC);
	install_element(PHYSICAL_SUBIF_NODE, &ospf_bfd_sess_detect_multiplier_reset_cmd, CMD_SYNC);

	/* "ospf bfd" command for h3c. */
	install_element(PHYSICAL_SUBIF_NODE, &ospf_bfd_h3c_cmd, CMD_SYNC);
	install_element(PHYSICAL_SUBIF_NODE, &no_ospf_bfd_h3c_cmd, CMD_SYNC);
}

/*ospf enable cmd install in L2if NODE */
static void ospf_l2if_init(void)
{
    /* "ip ospf network" commands. */
    install_element(PHYSICAL_IF_NODE, &ip_ospf_network_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ip_ospf_network_cmd, CMD_SYNC);
	/* "ip ospf network" commands for h3c. */
    install_element(PHYSICAL_IF_NODE, &ip_ospf_network_h3c_cmd, CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &ip_ospf_network_p2mp_h3c_cmd, CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &ip_ospf_network_p2p_h3c_cmd, CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &no_ip_ospf_network_h3c_cmd, CMD_SYNC);
    /* "ip ospf mtu-ignore" commands. */
    install_element(PHYSICAL_IF_NODE, &ip_ospf_mtu_ignore_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ip_ospf_mtu_ignore_cmd, CMD_SYNC);
	/* "ip ospf mtu-ignore" commands for h3c. */
    install_element(PHYSICAL_IF_NODE, &ip_ospf_mtu_ignore_h3c_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ip_ospf_mtu_ignore_h3c_cmd, CMD_SYNC);
    /* "description" commands. */
    //install_element (PHYSICAL_SUBIF_NODE, &interface_desc_cmd, CMD_SYNC);
    //install_element (PHYSICAL_SUBIF_NODE, &no_interface_desc_cmd, CMD_SYNC);
    /* "ip ospf auth simple" commands. */
    install_element(PHYSICAL_IF_NODE, &ip_ospf_auth_simple_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ip_ospf_auth_cmd, CMD_SYNC);
    /* "ip ospf auth md5" commands. */
    install_element(PHYSICAL_IF_NODE, &ip_ospf_auth_md5_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ip_ospf_auth_md5_cmd, CMD_SYNC);
	/* "ip ospf auth" commands for h3c. */
    install_element(PHYSICAL_IF_NODE, &ospf_interface_authentication_h3c_cmd, CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &ospf_interface_authentication_md5_h3c_cmd, CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &no_ospf_interface_authentication_h3c_cmd, CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &undo_ospf_interface_simple_authentication_h3c_cmd, CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &undo_ospf_interface_md5_authentication_h3c_cmd, CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &undo_ospf_interface_keychain_authentication_h3c_cmd, CMD_SYNC);
    /* "ip ospf cost" commands. */
    install_element(PHYSICAL_IF_NODE, &ip_ospf_cost_u32_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ip_ospf_cost_cmd, CMD_SYNC);
	/* "ip ospf cost" commands for h3c. */
	install_element(PHYSICAL_IF_NODE, &ip_ospf_cost_u32_h3c_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ip_ospf_cost_h3c_cmd, CMD_SYNC);
    /* "ip ospf dead-interval" commands. */
    install_element(PHYSICAL_IF_NODE, &ip_ospf_dead_interval_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ip_ospf_dead_interval_cmd, CMD_SYNC);
	/* "ip ospf dead-interval" commands for h3c. */
    install_element(PHYSICAL_IF_NODE, &ip_ospf_dead_interval_h3c_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ip_ospf_dead_interval_h3c_cmd, CMD_SYNC);
    /* "ip ospf hello-interval" commands. */
    install_element(PHYSICAL_IF_NODE, &ip_ospf_hello_interval_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ip_ospf_hello_interval_cmd, CMD_SYNC);
	/* "ip ospf hello-interval" commands for h3c. */
    install_element(PHYSICAL_IF_NODE, &ip_ospf_hello_interval_h3c_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ip_ospf_hello_interval_h3c_cmd, CMD_SYNC);
    /* "ip ospf priority" commands. */
    install_element(PHYSICAL_IF_NODE, &ip_ospf_priority_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ip_ospf_priority_cmd, CMD_SYNC);
	/* "ip ospf priority" commands for h3c. */
    install_element(PHYSICAL_IF_NODE, &ip_ospf_priority_h3c_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ip_ospf_priority_h3c_cmd, CMD_SYNC);
    /* "ip ospf retransmit-interval" commands. */
    install_element(PHYSICAL_IF_NODE, &ip_ospf_retransmit_interval_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ip_ospf_retransmit_interval_cmd, CMD_SYNC);
	/* "ip ospf retransmit-interval" commands for h3c. */
    install_element(PHYSICAL_IF_NODE, &ip_ospf_retransmit_interval_h3c_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ip_ospf_retransmit_interval_h3c_cmd, CMD_SYNC);
    /* "ip ospf transmit-delay" commands. */
    install_element(PHYSICAL_IF_NODE, &ip_ospf_transmit_delay_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ip_ospf_transmit_delay_cmd, CMD_SYNC);
    /* "ip ospf area" commands. */
    install_element(PHYSICAL_IF_NODE, &ip_ospf_area_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ip_ospf_area_cmd, CMD_SYNC);
    /* "ip ospf ldp-sync" commands. */
    install_element(PHYSICAL_IF_NODE, &ip_ospf_ldp_sync_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ip_ospf_ldp_sync_cmd, CMD_SYNC);
    /* "ip ospf ldp-sync hold down" commands. */
    install_element(PHYSICAL_IF_NODE, &ip_ospf_ldp_hold_down_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ip_ospf_ldp_hold_down_cmd, CMD_SYNC);
    /* "ip ospf ldp-sync hold-max-cost" commands. */
    install_element(PHYSICAL_IF_NODE, &ip_ospf_ldp_max_cost_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &ip_ospf_ldp_max_cost_infinite_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ip_ospf_ldp_max_cost_cmd, CMD_SYNC);
	/* "ospf bfd" command. */
	install_element(PHYSICAL_IF_NODE, &ospf_bfd_cmd, CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &no_ospf_bfd_cmd, CMD_SYNC);
	
	install_element(PHYSICAL_IF_NODE, &ospf_bfd_sess_cc_interval_set_cmd, CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &ospf_bfd_sess_cc_interval_reset_cmd, CMD_SYNC);
	
	install_element(PHYSICAL_IF_NODE, &ospf_bfd_sess_detect_multiplier_set_cmd, CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &ospf_bfd_sess_detect_multiplier_reset_cmd, CMD_SYNC);

	/* "ospf bfd" command for h3c. */
	install_element(PHYSICAL_IF_NODE, &ospf_bfd_h3c_cmd, CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &no_ospf_bfd_h3c_cmd, CMD_SYNC);
}

static void ospf_l2trunk_init(void)
{
    /* "ip ospf network" commands. */
    install_element(TRUNK_IF_NODE, &ip_ospf_network_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ip_ospf_network_cmd, CMD_SYNC);
	/* "ip ospf network" commands for h3c. */
	install_element(TRUNK_IF_NODE, &ip_ospf_network_h3c_cmd, CMD_SYNC);
	install_element(TRUNK_IF_NODE, &ip_ospf_network_p2mp_h3c_cmd, CMD_SYNC);
	install_element(TRUNK_IF_NODE, &ip_ospf_network_p2p_h3c_cmd, CMD_SYNC);
	install_element(TRUNK_IF_NODE, &no_ip_ospf_network_h3c_cmd, CMD_SYNC);

    /* "ip ospf mtu-ignore" commands. */
    install_element(TRUNK_IF_NODE, &ip_ospf_mtu_ignore_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ip_ospf_mtu_ignore_cmd, CMD_SYNC);
	/* "ip ospf mtu-ignore" commands for h3c. */
    install_element(TRUNK_IF_NODE, &ip_ospf_mtu_ignore_h3c_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ip_ospf_mtu_ignore_h3c_cmd, CMD_SYNC);
    /* "description" commands. */
    //install_element (PHYSICAL_SUBIF_NODE, &interface_desc_cmd, CMD_SYNC);
    //install_element (PHYSICAL_SUBIF_NODE, &no_interface_desc_cmd, CMD_SYNC);
    /* "ip ospf auth simple" commands. */
    install_element(TRUNK_IF_NODE, &ip_ospf_auth_simple_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ip_ospf_auth_cmd, CMD_SYNC);
    /* "ip ospf auth md5" commands. */
    install_element(TRUNK_IF_NODE, &ip_ospf_auth_md5_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ip_ospf_auth_md5_cmd, CMD_SYNC);
	/* "ip ospf auth" commands for h3c. */
    install_element(TRUNK_IF_NODE, &ospf_interface_authentication_h3c_cmd, CMD_SYNC);
	install_element(TRUNK_IF_NODE, &ospf_interface_authentication_md5_h3c_cmd, CMD_SYNC);
	install_element(TRUNK_IF_NODE, &no_ospf_interface_authentication_h3c_cmd, CMD_SYNC);
	install_element(TRUNK_IF_NODE, &undo_ospf_interface_simple_authentication_h3c_cmd, CMD_SYNC);
	install_element(TRUNK_IF_NODE, &undo_ospf_interface_md5_authentication_h3c_cmd, CMD_SYNC);
	install_element(TRUNK_IF_NODE, &undo_ospf_interface_keychain_authentication_h3c_cmd, CMD_SYNC);
    /* "ip ospf cost" commands. */
    install_element(TRUNK_IF_NODE, &ip_ospf_cost_u32_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ip_ospf_cost_cmd, CMD_SYNC);
	/* "ip ospf cost" commands for h3c. */
	install_element(TRUNK_IF_NODE, &ip_ospf_cost_u32_h3c_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ip_ospf_cost_h3c_cmd, CMD_SYNC);
    /* "ip ospf dead-interval" commands. */
    install_element(TRUNK_IF_NODE, &ip_ospf_dead_interval_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ip_ospf_dead_interval_cmd, CMD_SYNC);
	/* "ip ospf dead-interval" commands for h3c. */
    install_element(TRUNK_IF_NODE, &ip_ospf_dead_interval_h3c_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ip_ospf_dead_interval_h3c_cmd, CMD_SYNC);
    /* "ip ospf hello-interval" commands. */
    install_element(TRUNK_IF_NODE, &ip_ospf_hello_interval_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ip_ospf_hello_interval_cmd, CMD_SYNC);
	/* "ip ospf hello-interval" commands for h3c. */
    install_element(TRUNK_IF_NODE, &ip_ospf_hello_interval_h3c_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ip_ospf_hello_interval_h3c_cmd, CMD_SYNC);
    /* "ip ospf priority" commands. */
    install_element(TRUNK_IF_NODE, &ip_ospf_priority_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ip_ospf_priority_cmd, CMD_SYNC);
	/* "ip ospf priority" commands for h3c. */
    install_element(TRUNK_IF_NODE, &ip_ospf_priority_h3c_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ip_ospf_priority_h3c_cmd, CMD_SYNC);
    /* "ip ospf retransmit-interval" commands. */
    install_element(TRUNK_IF_NODE, &ip_ospf_retransmit_interval_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ip_ospf_retransmit_interval_cmd, CMD_SYNC);
	/* "ip ospf retransmit-interval" commands for h3c. */
    install_element(TRUNK_IF_NODE, &ip_ospf_retransmit_interval_h3c_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ip_ospf_retransmit_interval_h3c_cmd, CMD_SYNC);
    /* "ip ospf transmit-delay" commands. */
    install_element(TRUNK_IF_NODE, &ip_ospf_transmit_delay_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ip_ospf_transmit_delay_cmd, CMD_SYNC);
    /* "ip ospf area" commands. */
    install_element(TRUNK_IF_NODE, &ip_ospf_area_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ip_ospf_area_cmd, CMD_SYNC);
    /* "ip ospf ldp-sync" commands. */
    install_element(TRUNK_IF_NODE, &ip_ospf_ldp_sync_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ip_ospf_ldp_sync_cmd, CMD_SYNC);
    /* "ip ospf ldp-sync hold down" commands. */
    install_element(TRUNK_IF_NODE, &ip_ospf_ldp_hold_down_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ip_ospf_ldp_hold_down_cmd, CMD_SYNC);
    /* "ip ospf ldp-sync hold-max-cost" commands. */
    install_element(TRUNK_IF_NODE, &ip_ospf_ldp_max_cost_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &ip_ospf_ldp_max_cost_infinite_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ip_ospf_ldp_max_cost_cmd, CMD_SYNC);
	/* "ospf bfd" command. */
	install_element(TRUNK_IF_NODE, &ospf_bfd_cmd, CMD_SYNC);
	install_element(TRUNK_IF_NODE, &no_ospf_bfd_cmd, CMD_SYNC);
	
	install_element(TRUNK_IF_NODE, &ospf_bfd_sess_cc_interval_set_cmd, CMD_SYNC);
	install_element(TRUNK_IF_NODE, &ospf_bfd_sess_cc_interval_reset_cmd, CMD_SYNC);
	
	install_element(TRUNK_IF_NODE, &ospf_bfd_sess_detect_multiplier_set_cmd, CMD_SYNC);
	install_element(TRUNK_IF_NODE, &ospf_bfd_sess_detect_multiplier_reset_cmd, CMD_SYNC);

	/* "ospf bfd" command for h3c. */
	install_element(TRUNK_IF_NODE, &ospf_bfd_h3c_cmd, CMD_SYNC);
	install_element(TRUNK_IF_NODE, &no_ospf_bfd_h3c_cmd, CMD_SYNC);
}


static void ospf_l3trunk_init(void)
{
    /* "ip ospf network" commands. */
    install_element(TRUNK_SUBIF_NODE, &ip_ospf_network_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ip_ospf_network_cmd, CMD_SYNC);
	/* "ip ospf network" commands for h3c. */
	install_element(TRUNK_SUBIF_NODE, &ip_ospf_network_h3c_cmd, CMD_SYNC);
	install_element(TRUNK_SUBIF_NODE, &ip_ospf_network_p2mp_h3c_cmd, CMD_SYNC);
	install_element(TRUNK_SUBIF_NODE, &ip_ospf_network_p2p_h3c_cmd, CMD_SYNC);
	install_element(TRUNK_SUBIF_NODE, &no_ip_ospf_network_h3c_cmd, CMD_SYNC);
    /* "ip ospf mtu-ignore" commands. */
    install_element(TRUNK_SUBIF_NODE, &ip_ospf_mtu_ignore_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ip_ospf_mtu_ignore_cmd, CMD_SYNC);
	/* "ip ospf mtu-ignore" commands for h3c. */
    install_element(TRUNK_SUBIF_NODE, &ip_ospf_mtu_ignore_h3c_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ip_ospf_mtu_ignore_h3c_cmd, CMD_SYNC);
    /* "description" commands. */
    //install_element (PHYSICAL_SUBIF_NODE, &interface_desc_cmd, CMD_SYNC);
    //install_element (PHYSICAL_SUBIF_NODE, &no_interface_desc_cmd, CMD_SYNC);
    /* "ip ospf auth simple" commands. */
    install_element(TRUNK_SUBIF_NODE, &ip_ospf_auth_simple_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ip_ospf_auth_cmd, CMD_SYNC);
    /* "ip ospf auth md5" commands. */
    install_element(TRUNK_SUBIF_NODE, &ip_ospf_auth_md5_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ip_ospf_auth_md5_cmd, CMD_SYNC);
	/* "ip ospf auth" commands for h3c. */
    install_element(TRUNK_SUBIF_NODE, &ospf_interface_authentication_h3c_cmd, CMD_SYNC);
	install_element(TRUNK_SUBIF_NODE, &ospf_interface_authentication_md5_h3c_cmd, CMD_SYNC);
	install_element(TRUNK_SUBIF_NODE, &no_ospf_interface_authentication_h3c_cmd, CMD_SYNC);
	install_element(TRUNK_SUBIF_NODE, &undo_ospf_interface_simple_authentication_h3c_cmd, CMD_SYNC);
	install_element(TRUNK_SUBIF_NODE, &undo_ospf_interface_md5_authentication_h3c_cmd, CMD_SYNC);
	install_element(TRUNK_SUBIF_NODE, &undo_ospf_interface_keychain_authentication_h3c_cmd, CMD_SYNC);
    /* "ip ospf cost" commands. */
    install_element(TRUNK_SUBIF_NODE, &ip_ospf_cost_u32_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ip_ospf_cost_cmd, CMD_SYNC);
	/* "ip ospf cost" commands for h3c. */
	install_element(TRUNK_SUBIF_NODE, &ip_ospf_cost_u32_h3c_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ip_ospf_cost_h3c_cmd, CMD_SYNC);
    /* "ip ospf dead-interval" commands. */
    install_element(TRUNK_SUBIF_NODE, &ip_ospf_dead_interval_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ip_ospf_dead_interval_cmd, CMD_SYNC);
	/* "ip ospf dead-interval" commands for h3c. */
    install_element(TRUNK_SUBIF_NODE, &ip_ospf_dead_interval_h3c_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ip_ospf_dead_interval_h3c_cmd, CMD_SYNC);
    /* "ip ospf hello-interval" commands. */
    install_element(TRUNK_SUBIF_NODE, &ip_ospf_hello_interval_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ip_ospf_hello_interval_cmd, CMD_SYNC);
	/* "ip ospf hello-interval" commands for h3c. */
    install_element(TRUNK_SUBIF_NODE, &ip_ospf_hello_interval_h3c_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ip_ospf_hello_interval_h3c_cmd, CMD_SYNC);
    /* "ip ospf priority" commands. */
    install_element(TRUNK_SUBIF_NODE, &ip_ospf_priority_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ip_ospf_priority_cmd, CMD_SYNC);
	/* "ip ospf priority" commands for h3c. */
    install_element(TRUNK_SUBIF_NODE, &ip_ospf_priority_h3c_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ip_ospf_priority_h3c_cmd, CMD_SYNC);
    /* "ip ospf retransmit-interval" commands. */
    install_element(TRUNK_SUBIF_NODE, &ip_ospf_retransmit_interval_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ip_ospf_retransmit_interval_cmd, CMD_SYNC);
	/* "ip ospf retransmit-interval" commands for h3c. */
    install_element(TRUNK_SUBIF_NODE, &ip_ospf_retransmit_interval_h3c_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ip_ospf_retransmit_interval_h3c_cmd, CMD_SYNC);
    /* "ip ospf transmit-delay" commands. */
    install_element(TRUNK_SUBIF_NODE, &ip_ospf_transmit_delay_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ip_ospf_transmit_delay_cmd, CMD_SYNC);
    /* "ip ospf area" commands. */
    install_element(TRUNK_SUBIF_NODE, &ip_ospf_area_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ip_ospf_area_cmd, CMD_SYNC);
    /* "ip ospf ldp-sync" commands. */
    install_element(TRUNK_SUBIF_NODE, &ip_ospf_ldp_sync_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ip_ospf_ldp_sync_cmd, CMD_SYNC);
    /* "ip ospf ldp-sync hold down" commands. */
    install_element(TRUNK_SUBIF_NODE, &ip_ospf_ldp_hold_down_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ip_ospf_ldp_hold_down_cmd, CMD_SYNC);
    /* "ip ospf ldp-sync hold-max-cost" commands. */
    install_element(TRUNK_SUBIF_NODE, &ip_ospf_ldp_max_cost_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &ip_ospf_ldp_max_cost_infinite_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ip_ospf_ldp_max_cost_cmd, CMD_SYNC);
	/* "ospf bfd" command. */
	install_element(TRUNK_SUBIF_NODE, &ospf_bfd_cmd, CMD_SYNC);
	install_element(TRUNK_SUBIF_NODE, &no_ospf_bfd_cmd, CMD_SYNC);
	
	install_element(TRUNK_SUBIF_NODE, &ospf_bfd_sess_cc_interval_set_cmd, CMD_SYNC);
	install_element(TRUNK_SUBIF_NODE, &ospf_bfd_sess_cc_interval_reset_cmd, CMD_SYNC);
	
	install_element(TRUNK_SUBIF_NODE, &ospf_bfd_sess_detect_multiplier_set_cmd, CMD_SYNC);
	install_element(TRUNK_SUBIF_NODE, &ospf_bfd_sess_detect_multiplier_reset_cmd, CMD_SYNC);

	/* "ospf bfd" command for h3c. */
	install_element(TRUNK_SUBIF_NODE, &ospf_bfd_h3c_cmd, CMD_SYNC);
	install_element(TRUNK_SUBIF_NODE, &no_ospf_bfd_h3c_cmd, CMD_SYNC);
}

static void ospf_vlanif_init(void)
{
    /* "ip ospf network" commands. */
    install_element(VLANIF_NODE, &ip_ospf_network_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ip_ospf_network_cmd, CMD_SYNC);
	/* "ip ospf network" commands for h3c. */
	install_element(VLANIF_NODE, &ip_ospf_network_h3c_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &ip_ospf_network_p2mp_h3c_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &ip_ospf_network_p2p_h3c_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &no_ip_ospf_network_h3c_cmd, CMD_SYNC);
    /* "ip ospf mtu-ignore" commands. */
    install_element(VLANIF_NODE, &ip_ospf_mtu_ignore_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ip_ospf_mtu_ignore_cmd, CMD_SYNC);
	/* "ip ospf mtu-ignore" commands for h3c. */
    install_element(VLANIF_NODE, &ip_ospf_mtu_ignore_h3c_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ip_ospf_mtu_ignore_h3c_cmd, CMD_SYNC);
    /* "description" commands. */
    //install_element (VLANIF_NODE, &interface_desc_cmd, CMD_SYNC);
    //install_element (VLANIF_NODE, &no_interface_desc_cmd, CMD_SYNC);
    /* "ip ospf auth simple" commands. */
    install_element(VLANIF_NODE, &ip_ospf_auth_simple_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ip_ospf_auth_cmd, CMD_SYNC);
    /* "ip ospf auth md5" commands. */
    install_element(VLANIF_NODE, &ip_ospf_auth_md5_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ip_ospf_auth_md5_cmd, CMD_SYNC);
	/* "ip ospf auth" commands for h3c. */
    install_element(VLANIF_NODE, &ospf_interface_authentication_h3c_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &ospf_interface_authentication_md5_h3c_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &no_ospf_interface_authentication_h3c_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &undo_ospf_interface_simple_authentication_h3c_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &undo_ospf_interface_md5_authentication_h3c_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &undo_ospf_interface_keychain_authentication_h3c_cmd, CMD_SYNC);
    /* "ip ospf cost" commands. */
    install_element(VLANIF_NODE, &ip_ospf_cost_u32_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ip_ospf_cost_cmd, CMD_SYNC);
	/* "ip ospf cost" commands for h3c. */
	install_element(VLANIF_NODE, &ip_ospf_cost_u32_h3c_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ip_ospf_cost_h3c_cmd, CMD_SYNC);
    /* "ip ospf dead-interval" commands. */
    install_element(VLANIF_NODE, &ip_ospf_dead_interval_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ip_ospf_dead_interval_cmd, CMD_SYNC);
	/* "ip ospf dead-interval" commands for h3c. */
    install_element(VLANIF_NODE, &ip_ospf_dead_interval_h3c_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ip_ospf_dead_interval_h3c_cmd, CMD_SYNC);
    /* "ip ospf hello-interval" commands. */
    install_element(VLANIF_NODE, &ip_ospf_hello_interval_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ip_ospf_hello_interval_cmd, CMD_SYNC);
	/* "ip ospf hello-interval" commands for h3c. */
    install_element(VLANIF_NODE, &ip_ospf_hello_interval_h3c_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ip_ospf_hello_interval_h3c_cmd, CMD_SYNC);
    /* "ip ospf priority" commands. */
    install_element(VLANIF_NODE, &ip_ospf_priority_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ip_ospf_priority_cmd, CMD_SYNC);
	/* "ip ospf priority" commands for h3c. */
    install_element(VLANIF_NODE, &ip_ospf_priority_h3c_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ip_ospf_priority_h3c_cmd, CMD_SYNC);
    /* "ip ospf retransmit-interval" commands. */
    install_element(VLANIF_NODE, &ip_ospf_retransmit_interval_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ip_ospf_retransmit_interval_cmd, CMD_SYNC);
	/* "ip ospf retransmit-interval" commands for h3c. */
    install_element(VLANIF_NODE, &ip_ospf_retransmit_interval_h3c_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ip_ospf_retransmit_interval_h3c_cmd, CMD_SYNC);
    /* "ip ospf transmit-delay" commands. */
    install_element(VLANIF_NODE, &ip_ospf_transmit_delay_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ip_ospf_transmit_delay_cmd, CMD_SYNC);
    /* "ip ospf area" commands. */
    install_element(VLANIF_NODE, &ip_ospf_area_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ip_ospf_area_cmd, CMD_SYNC);
    /* "ip ospf ldp-sync" commands. */
    install_element(VLANIF_NODE, &ip_ospf_ldp_sync_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ip_ospf_ldp_sync_cmd, CMD_SYNC);
    /* "ip ospf ldp-sync hold down" commands. */
    install_element(VLANIF_NODE, &ip_ospf_ldp_hold_down_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ip_ospf_ldp_hold_down_cmd, CMD_SYNC);
    /* "ip ospf ldp-sync hold-max-cost" commands. */
    install_element(VLANIF_NODE, &ip_ospf_ldp_max_cost_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &ip_ospf_ldp_max_cost_infinite_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ip_ospf_ldp_max_cost_cmd, CMD_SYNC);
	/* "ospf bfd" command. */
	install_element(VLANIF_NODE, &ospf_bfd_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &no_ospf_bfd_cmd, CMD_SYNC);
	
	install_element(VLANIF_NODE, &ospf_bfd_sess_cc_interval_set_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &ospf_bfd_sess_cc_interval_reset_cmd, CMD_SYNC);
	
	install_element(VLANIF_NODE, &ospf_bfd_sess_detect_multiplier_set_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &ospf_bfd_sess_detect_multiplier_reset_cmd, CMD_SYNC);

	/* "ospf bfd" command for h3c. */
	install_element(VLANIF_NODE, &ospf_bfd_h3c_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &no_ospf_bfd_h3c_cmd, CMD_SYNC);
}


static struct cmd_node area_node =
{
    AREA_NODE,
    "%s(config-ospf-area)# ",
    1
};


static void
ospf_vty_area_init(void)
{
    install_node(&area_node, NULL);
    install_default(AREA_NODE);
    /*area node commands*/
    install_element(OSPF_NODE, &area_ospf_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &no_area_ospf_cmd, CMD_SYNC);

	install_element(OSPF_NODE, &area_ospf_address_format_cmd, CMD_SYNC);
	install_element(OSPF_NODE, &no_area_ospf_address_format_cmd, CMD_SYNC);
	install_element(OSPF_NODE, &no_area_ospf_h3c_cmd, CMD_SYNC);
	
    /* "network area" commands. */
    install_element(AREA_NODE, &ospf_network_area_cmd, CMD_SYNC);
    install_element(AREA_NODE, &no_ospf_network_area_cmd, CMD_SYNC);

	install_element(AREA_NODE, &ospf_network_area_h3c_cmd, CMD_SYNC);
    install_element(AREA_NODE, &no_ospf_network_area_h3c_cmd, CMD_SYNC);
    /* "area stub or nssa no_summury" commands. */
    install_element(AREA_NODE, &ospf_area_nssa_cmd, CMD_SYNC);
    install_element(AREA_NODE, &no_ospf_area_stub_nssa_cmd, CMD_SYNC);

	install_element(AREA_NODE, &ospf_area_stub_h3c_cmd, CMD_SYNC);
	install_element(AREA_NODE, &no_ospf_area_stub_nssa_h3c_cmd, CMD_SYNC);
    /* "area name" commands.  */
    install_element(AREA_NODE, &ospf_area_name_cmd, CMD_SYNC);
    install_element(AREA_NODE, &no_ospf_area_name_cmd, CMD_SYNC);

	install_element(AREA_NODE, &ospf_area_name_h3c_cmd, CMD_SYNC);
    install_element(AREA_NODE, &no_ospf_area_name_h3c_cmd, CMD_SYNC);
    /* "area summary" commands.  */
    install_element(AREA_NODE, &ospf_area_summary_cmd, CMD_SYNC);
    install_element(AREA_NODE, &no_ospf_area_summary_cmd, CMD_SYNC);

	/* "area summary" commands for h3c. */
	install_element(AREA_NODE, &ospf_area_summary_h3c_cmd, CMD_SYNC);
	install_element(AREA_NODE, &ospf_area_summary_format2_h3c_cmd, CMD_SYNC);
	install_element(AREA_NODE, &no_ospf_area_summary_h3c_cmd, CMD_SYNC);
	
    /* "area default cost" commands.  */
    install_element(AREA_NODE, &ospf_area_default_cost_cmd, CMD_SYNC);
    install_element(AREA_NODE, &no_ospf_area_default_cost_cmd, CMD_SYNC);
    install_element(AREA_NODE, &no_ospf_area_default_cost_h3c_cmd, CMD_SYNC);
	/* "area virtual-link" commands. */
	/*install_element (AREA_NODE, &ospf_area_vlink_cmd, CMD_SYNC);
	install_element (AREA_NODE, &no_ospf_area_vlink_cmd, CMD_SYNC);

	install_element (AREA_NODE, &ospf_area_vlink_param1_cmd, CMD_SYNC);
	install_element (AREA_NODE, &no_ospf_area_vlink_param1_cmd, CMD_SYNC);

	install_element (AREA_NODE, &ospf_area_vlink_param2_cmd, CMD_SYNC);
	install_element (AREA_NODE, &no_ospf_area_vlink_param2_cmd, CMD_SYNC);

	install_element (AREA_NODE, &ospf_area_vlink_retrans_interval_cmd, CMD_SYNC);
	install_element (AREA_NODE, &no_ospf_area_vlink_retrans_interval_cmd, CMD_SYNC);

	install_element (AREA_NODE, &ospf_area_vlink_trans_delay_cmd, CMD_SYNC);
	install_element (AREA_NODE, &no_ospf_area_vlink_trans_delay_cmd, CMD_SYNC);

	install_element (AREA_NODE, &ospf_area_vlink_md5_cmd, CMD_SYNC);
	install_element (AREA_NODE, &no_ospf_area_vlink_md5_cmd, CMD_SYNC);

	install_element (AREA_NODE, &ospf_area_vlink_authkey_cmd, CMD_SYNC);
	install_element (AREA_NODE, &no_ospf_area_vlink_authkey_cmd, CMD_SYNC);
	*/
/* "area virtual-link" commands for h3c. */
	install_element (AREA_NODE, &ospf_area_vlink_h3c_cmd, CMD_SYNC);
	install_element (AREA_NODE, &no_ospf_area_vlink_h3c_cmd, CMD_SYNC);
	install_element (AREA_NODE, &no_ospf_area_vlink_cmd, CMD_SYNC);
	install_element (AREA_NODE, &ospf_area_vlink_auth_md5_h3c_cmd, CMD_SYNC);
	install_element (AREA_NODE, &ospf_area_vlink_auth_simple_h3c_cmd, CMD_SYNC);
	install_element (AREA_NODE, &no_ospf_area_vlink_auth_md5_h3c_cmd, CMD_SYNC);
	install_element (AREA_NODE, &no_ospf_area_vlink_auth_md5_cmd, CMD_SYNC);
	/* "area authentication" commands. */
	install_element (AREA_NODE, &ospf_area_authentication_message_digest_cmd, CMD_SYNC);
	install_element (AREA_NODE, &ospf_area_authentication_cmd, CMD_SYNC);
	install_element (AREA_NODE, &no_ospf_area_authentication_cmd, CMD_SYNC);
	install_element (AREA_NODE, &undo_ospf_area_simple_authentication_cmd, CMD_SYNC);
	install_element (AREA_NODE, &undo_ospf_area_md5_authentication_cmd, CMD_SYNC);

}



/* ospfd's interface node. */
static struct cmd_node interface_node =
{
    INTERFACE_NODE,
    "%s(config-if)# ",
    1
};

/* Initialization of OSPF interface. */
static void
ospf_vty_if_init(void)
{
    /* Install interface node. */
    install_node(&interface_node, NULL);
    install_element(CONFIG_NODE, &interface_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &no_interface_cmd, CMD_SYNC);
    install_default(INTERFACE_NODE);
}

static void
ospf_vty_zebra_init(void)
{
    install_element(OSPF_NODE, &ospf_redistribute_source_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &no_ospf_redistribute_source_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &ospf_redistribute_ri_source_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &no_ospf_redistribute_ri_source_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &ospf_distribute_list_out_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &no_ospf_distribute_list_out_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &ospf_default_information_originate_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &no_ospf_default_information_originate_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &ospf_default_metric_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &no_ospf_default_metric_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &ospf_redistribute_direct_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &no_ospf_redistribute_direct_cmd, CMD_SYNC);

	/* "ospf import-route" commands for h3c. */
	install_element(OSPF_NODE, &ospf_redistribute_source_h3c_cmd, CMD_SYNC);
	install_element(OSPF_NODE, &ospf_redistribute_source_h3c_bgp_cmd, CMD_SYNC);
	install_element(OSPF_NODE, &ospf_redistribute_source_h3c_bgp_format2_cmd, CMD_SYNC);
	install_element(OSPF_NODE, &ospf_redistribute_source_h3c_bgp_format3_cmd, CMD_SYNC);
	install_element(OSPF_NODE, &ospf_redistribute_ri_source_h3c_cmd, CMD_SYNC);
	install_element(OSPF_NODE, &ospf_redistribute_ri_source_h3c_format2_cmd, CMD_SYNC);
	install_element(OSPF_NODE, &no_ospf_redistribute_source_h3c_cmd, CMD_SYNC);
	install_element(OSPF_NODE, &no_ospf_redistribute_ri_source_h3c_cmd, CMD_SYNC);
	install_element(OSPF_NODE, &no_ospf_redistribute_ri_source_format2_h3c_cmd, CMD_SYNC);
	
    /* "ospf distance" commands. */
    install_element(OSPF_NODE, &no_distance_ospf_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &distance_ospf_cmd, CMD_SYNC);

	/* "ospf distance" commands for h3c */
    install_element(OSPF_NODE, &distance_ospf_h3c_cmd, CMD_SYNC);
	install_element(OSPF_NODE, &distance_ospf_h3c_adapt_format1_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &no_distance_ospf_h3c_cmd, CMD_SYNC);
}

static struct cmd_node ospf_node =
{
    OSPF_NODE,
    "%s(config-ospf)# ",
    1
};


static struct cmd_node dcn_node =
{
    OSPF_DCN_NODE,
    "%s(config-dcn)# ",
    1
};


/* Install OSPF related vty commands. */
void
ospf_vty_init(void)
{
    /* Install ospf top node. */
    install_node(&ospf_node, ospf_config_write);
    install_node_factory(&ospf_node, ospf_config_write_for_factory);
    install_default(OSPF_NODE);

	 /* Install dcn ospf top node. */
    install_node (&dcn_node, ospf_dcn_config_write);
    install_node_factory(&dcn_node, ospf_dcn_config_write);
    install_default (OSPF_DCN_NODE);

	install_element (CONFIG_NODE, &dcn_u0_test_cmd, CMD_SYNC);

	/* "dcn ospf node" commands*/
	install_element (CONFIG_NODE, &dcn_ospf_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &no_dcn_ospf_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &undo_dcn_ospf_cmd, CMD_SYNC);
    /* "ospf instance" commands. */
    install_element(CONFIG_NODE, &router_ospf_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &router_ospf_cmd_h3c, CMD_SYNC);
    install_element(CONFIG_NODE, &no_router_ospf_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &undo_router_ospf_h3c_cmd, CMD_SYNC);
    /* "ospf restart" commands. */
    install_element(CONFIG_NODE, &ospf_reset_instance_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &ospf_reset_instance_h3c_cmd, CMD_SYNC);
    /* "ospf router-id" commands. */
    install_element(OSPF_NODE, &router_ospf_id_cmd, CMD_SYNC);

    /* "ospf rfc1583-compatible" commands. */
    install_element(OSPF_NODE, &ospf_compatible_rfc1583_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &no_ospf_compatible_rfc1583_cmd, CMD_SYNC);

	/* "ospf rfc1583-compatible" commands for h3c*/
    install_element(OSPF_NODE, &ospf_compatible_rfc1583_h3c_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &no_ospf_compatible_rfc1583_h3c_cmd, CMD_SYNC);
    /* reference bandwidth commands */
    install_element(OSPF_NODE, &ospf_auto_cost_reference_bandwidth_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &no_ospf_auto_cost_reference_bandwidth_cmd, CMD_SYNC);
	
	/* reference bandwidth commands for h3c*/
	install_element(OSPF_NODE, &ospf_auto_cost_reference_bandwidth_h3c_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &no_ospf_auto_cost_reference_bandwidth_h3c_cmd, CMD_SYNC);
	
    /* "passive-interface" commands. */
    install_element(OSPF_NODE, &ospf_passive_interface_addr_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &ospf_passive_interface_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &ospf_passive_interface_default_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &no_ospf_passive_interface_addr_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &no_ospf_passive_interface_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &no_ospf_passive_interface_default_cmd, CMD_SYNC);
    /* "neighbor" commands. */
    install_element(OSPF_NODE, &ospf_neighbor_poll_interval_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &no_ospf_neighbor_cmd, CMD_SYNC);

	/* "neighbor" commands for h3c. */
	install_element(OSPF_NODE, &ospf_neighbor_poll_interval_h3c_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &ospf_neighbor_cost_h3c_cmd, CMD_SYNC);
	install_element(OSPF_NODE, &ospf_neighbor_dr_priority_h3c_cmd, CMD_SYNC);
	install_element(OSPF_NODE, &no_ospf_neighbor_h3c_cmd, CMD_SYNC);
	
    /*asbr summary command*/
    install_element(OSPF_NODE, &ospf_asbr_summary_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &no_ospf_asbr_summary_cmd, CMD_SYNC);

	install_element(OSPF_NODE, &ospf_asbr_summary_h3c_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &no_ospf_asbr_summary_h3c_cmd, CMD_SYNC);
	
    install_element(OSPF_NODE, &ospf_refresh_interval_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &no_ospf_refresh_interval_cmd, CMD_SYNC);
    /*refresh interval command*/
    install_element(OSPF_NODE, &ospf_refresh_timer_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &no_ospf_refresh_timer_cmd, CMD_SYNC);

	/* min_ls_arrival */
	install_element(OSPF_NODE, &ospf_timers_min_ls_arrival_cmd, CMD_SYNC);
    install_element(OSPF_NODE, &no_ospf_timers_min_ls_arrival_cmd, CMD_SYNC);
	
    /* "ospf dcn enable" commands. */
	install_element (OSPF_DCN_NODE, &dcn_trap_report_cmd, CMD_SYNC);
    install_element (OSPF_DCN_NODE, &no_dcn_trap_report_cmd, CMD_SYNC);
	install_element (OSPF_DCN_NODE, &undo_dcn_trap_report_cmd, CMD_SYNC);
	install_element (OSPF_DCN_NODE, &dcn_ne_ip_set_cmd, CMD_SYNC);
	install_element (OSPF_DCN_NODE, &dcn_ne_ip_set_same_h3c_cmd, CMD_SYNC);
	install_element (OSPF_DCN_NODE, &dcn_ne_id_set_cmd, CMD_SYNC);
	
	/* "ospf bfd" command. */
	
    
    /* Init interface related vty commands. */
    ospf_vty_if_init ();
    /*l3if init */
    ospf_l3if_init();
    /*l2if init */
    ospf_l2if_init();
    /*l2trunk init*/
    ospf_l2trunk_init();
    /*l3trunk init*/
    ospf_l3trunk_init();
	/*vlanif init*/
	ospf_vlanif_init();
    /* Init zebra related vty commands. */
    ospf_vty_zebra_init ();
    /*Init area vty commands*/
    ospf_vty_area_init();
}


