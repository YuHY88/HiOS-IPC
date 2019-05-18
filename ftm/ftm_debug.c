/*
*        ftm debug fuction
*/


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <lib/zebra.h>
#include <lib/ptree.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/command.h>
#include <lib/sockunion.h>
#include <lib/prefix.h>
#include <lib/vty.h>
#include <lib/hash1.h>
#include <lib/errcode.h>
#include <lib/inet_ip.h>
#include <lib/ether.h>
#include <lib/ifm_common.h>
#include <route/ndp_cmd.h>


#include "ftm_nhp.h"
#include "ftm_fib.h"
#include "ftm_ilm.h"
#include "ftm_nhlfe.h"
#include "ftm_arp.h"
#include "ftm_ndp.h"
#include "ftm_ifm.h"
#include "ftm_pw.h"
#include "ftm_fib.h"
#include "ftm_debug.h"
#include "ftm_ifm.h"
#include "ftm_pkt.h"
#include "pkt_icmp.h"
#include "pkt_tcp.h"
#include "pkt_type.h"
#include "pkt_buffer.h"
#include "proto_reg.h"
#include "pkt_arp.h"




#define DECIMAL_STRLEN_MIN 1
#define DECIMAL_STRLEN_MAX 10

extern struct pkt_debug ftm_pkt_dbg;
extern struct pkt_count ftm_pkt_cnt;
extern void tcp_config_write ( struct vty *vty);

static unsigned int current_row = 0;


const struct message ftm_dbg_name[] = {
		{.key = FTM_DBG_PKT,	.str = "pkt"},
		{.key = FTM_DBG_IP,		.str = "ip" },
		{.key = FTM_DBG_TCP,	.str = "tcp"},
		{.key = FTM_DBG_UDP,	.str = "udp"},
		{.key = FTM_DBG_L3,		.str = "l3" },
		{.key = FTM_DBG_ARP,	.str = "arp" },
		{.key = FTM_DBG_NDP,	.str = "ndp" },
		{.key = FTM_DBG_ALL,	.str = "all"},
};

#define ROUTE_LINE_CHECK(line, line_num)\
	if ((line) == ((line_num)+60)) {(line)=0; return CMD_CONTINUE;}\
	if (((line)<((line_num)+60)) && ((++(line))>line_num))

/*
DEFUN (debug,
		debug_cmd,
		"debug",
		"access debug\n"
)
{
	vty->node = DEBUG_NODE;
//	vty->index =;

	return CMD_SUCCESS;
}
*/
static struct cmd_node tcp_node =
{
   TCP_NODE,
   "%s(config-tcp)# ",
   1,
};


static void convert_nhp_type_to_string(enum NHP_TYPE nhp_type, char *type)
{
    switch(nhp_type)
    {
        case NHP_TYPE_HOST:
            sprintf(type, "%s", "HOST");
            break;
        case NHP_TYPE_IP:
            sprintf(type, "%s", "IP");
            break;
        case NHP_TYPE_CONNECT:
            sprintf(type, "%s", "CONNECT");
            break;
        case NHP_TYPE_LSP:
            sprintf(type, "%s", "LSP");
            break;
        case NHP_TYPE_FRR:
            sprintf(type, "%s", "FRR");
            break;
        case NHP_TYPE_ECMP:
            sprintf(type, "%s", "ECMP");
            break;
        case NHP_TYPE_TUNNEL:
            sprintf(type, "%s", "TUNNEL");
            break;
        default:
            sprintf(type, "%s", "UNKNOWN");
            break;
    }

    return ;
}

#if 0
static void convert_nhp_action_to_string(enum NHP_ACTION nhp_action, char *action)
{
    switch(nhp_action)
    {
        case NHP_ACTION_FORWARD:
            sprintf(action, "%s", "FORWARD");
            break;
        case NHP_ACTION_DROP:
            sprintf(action, "%s", "DROP");
            break;
        case NHP_ACTION_TOCPU:
            sprintf(action, "%s", "TOCPU");
            break;
        default:
            sprintf(action, "%s", "UNKNOWN");
            break;
    }

    return ;
}
#endif

static int ftm_fib_entry_print(struct vty *vty, struct fib_entry *pfib, void *nhp_info, int print_hdr)
{
    int i;
    char buf[32], ip_mask[32], type[32];
    //char nexthop[32], action[32], ifname[32];
    char nexthop[32], ifname[32];
    struct ecmp_group *ecmp = NULL;
    struct nhp_entry *pnhp = NULL;

    if(NULL == pfib || NULL == nhp_info)
    {
        return 1;
    }

    if(print_hdr)
    {
        ROUTE_LINE_CHECK(current_row, vty->flush_cp)
        {
            vty_out(vty, "FIB Table:%s", VTY_NEWLINE);
        }

		if(2 != print_hdr)
		{
			ROUTE_LINE_CHECK(current_row, vty->flush_cp)
			{
				vty_out(vty, "Total number of routes:%d%s", fib_num, VTY_NEWLINE);
			}
		}

        ROUTE_LINE_CHECK(current_row, vty->flush_cp)
        {
            //vty_out(vty, "Destionation/Mask  VPN  Nexthop         Type    Action  Interface%s", VTY_NEWLINE);
            vty_out(vty, "Destionation/Mask  VPN  Nexthop         Type    Interface%s", VTY_NEWLINE);
        }
    }

    memset(buf, 0, sizeof(buf));
    memset(ip_mask, 0, sizeof(ip_mask));
    inet_ipv4tostr(pfib->ipaddr, buf);

    snprintf(ip_mask, 32, "%s/%d", buf, pfib->masklen);

    memset(type, 0, sizeof(type));
    convert_nhp_type_to_string(pfib->nhp.nhp_type, type);

    if(NHP_TYPE_ECMP == pfib->nhp.nhp_type || NHP_TYPE_FRR == pfib->nhp.nhp_type)
    {
        ecmp = (struct ecmp_group *)nhp_info;

        for(i = 0; i < ecmp->nhp_num; i++)
        {
            pnhp = ecmp->nhp[i].pnhp;

            memset(nexthop, 0, sizeof(nexthop));
            inet_ipv4tostr(pnhp->nexthop.addr.ipv4, nexthop);

            //memset(action, 0, sizeof(action));
            //convert_nhp_action_to_string(pnhp->action, action);

            memset(ifname, 0, sizeof(ifname));
            if(0 == pnhp->ifindex)
            {
                snprintf(ifname, 32, "%s", "---");
            }
            else
            {
                ifm_get_name_by_ifindex(pnhp->ifindex, ifname);
            }

            if(0 == i)
            {
                ROUTE_LINE_CHECK(current_row, vty->flush_cp)
                {
                    //vty_out(vty, "%-18s %-4d %-15s %-7s %-7s %s%s", ip_mask, pfib->vpn, nexthop, type, action, ifname, VTY_NEWLINE);
                    vty_out(vty, "%-18s %-4d %-15s %-7s %s%s", ip_mask, pfib->vpn, nexthop, type, ifname, VTY_NEWLINE);
                }
            }
            else
            {
                ROUTE_LINE_CHECK(current_row, vty->flush_cp)
                {
                    //vty_out(vty, "%-23s %-15s %-7s %-7s %s%s", " ", nexthop, type, action, ifname, VTY_NEWLINE);
                    vty_out(vty, "%-23s %-15s %-7s %s%s", " ", nexthop, type, ifname, VTY_NEWLINE);
                }
            }
        }
    }
    else
    {
        pnhp = (struct nhp_entry *)nhp_info;

        memset(nexthop, 0, sizeof(nexthop));
        inet_ipv4tostr(pnhp->nexthop.addr.ipv4, nexthop);

        //memset(action, 0, sizeof(action));
        //convert_nhp_action_to_string(pnhp->action, action);

        memset(ifname, 0, sizeof(ifname));
        if(0 == pnhp->ifindex || NHP_TYPE_LSP == pnhp->nhp_type)
        {
            snprintf(ifname, 32, "%s", "---");
        }
        else
        {
            ifm_get_name_by_ifindex(pnhp->ifindex, ifname);
        }

        ROUTE_LINE_CHECK(current_row, vty->flush_cp)
        {
            //vty_out(vty, "%-18s %-4d %-15s %-7s %-7s %s%s", ip_mask, pfib->vpn, nexthop, type, action, ifname, VTY_NEWLINE);
            vty_out(vty, "%-18s %-4d %-15s %-7s %s%s", ip_mask, pfib->vpn, nexthop, type, ifname, VTY_NEWLINE);
        }
    }

    return 0;
}

static int ftm_fibv6_entry_print(struct vty *vty, struct fibv6_entry *pfib, void *nhp_info, int print_hdr)
{
    int i;
    char ip_str[IPV6_ADDR_STRLEN], type[32];
    //char nexthop[IPV6_ADDR_STRLEN], action[32], ifname[32];
    char nexthop[IPV6_ADDR_STRLEN], ifname[32];
    struct ecmp_group *ecmp = NULL;
    struct nhp_entry *pnhp = NULL;

    if(NULL == pfib || NULL == nhp_info)
    {
        return 1;
    }

    if(print_hdr)
    {
        ROUTE_LINE_CHECK(current_row, vty->flush_cp)
        {
            vty_out(vty, "IPV6 FIB Table:%s", VTY_NEWLINE);
        }

		if(2 != print_hdr)
		{
			ROUTE_LINE_CHECK(current_row, vty->flush_cp)
			{
				vty_out(vty, "Total number of routes:%d%s", fibv6_num, VTY_NEWLINE);
			}
		}
    }

    memset(ip_str, 0, IPV6_ADDR_STRLEN);
	inet_ipv6tostr((struct ipv6_addr *)(pfib->ipaddr), ip_str, IPV6_ADDR_STRLEN);

	ROUTE_LINE_CHECK(current_row, vty->flush_cp)
	{
		vty_out(vty, "%sDestination:%-46s PrefixLength:%-7d%s", VTY_NEWLINE, ip_str, pfib->masklen, VTY_NEWLINE);
	}

    memset(type, 0, sizeof(type));
    convert_nhp_type_to_string(pfib->nhp.nhp_type, type);

	//ROUTE_LINE_CHECK(current_row, vty->flush_cp)
	//{
	//	vty_out(vty, "%-11s:%-46d %-12s:%-7s%s", "VPN", pfib->vpn, "Type", type, VTY_NEWLINE);
	//}

    if(NHP_TYPE_ECMP == pfib->nhp.nhp_type || NHP_TYPE_FRR == pfib->nhp.nhp_type)
    {
        ecmp = (struct ecmp_group *)nhp_info;

        for(i = 0; i < ecmp->nhp_num; i++)
        {
            pnhp = ecmp->nhp[i].pnhp;

            memset(nexthop, 0, sizeof(nexthop));
			if(INET_FAMILY_IPV4 == pnhp->nexthop.type)
			{
				inet_ipv4tostr(pnhp->nexthop.addr.ipv4, nexthop);
			}
			else if(INET_FAMILY_IPV6 == pnhp->nexthop.type)
			{
				inet_ipv6tostr((struct ipv6_addr *)(pnhp->nexthop.addr.ipv6), nexthop, IPV6_ADDR_STRLEN);
			}

            //memset(action, 0, sizeof(action));
            //convert_nhp_action_to_string(pnhp->action, action);

            memset(ifname, 0, sizeof(ifname));
            if(0 == pnhp->ifindex)
            {
                snprintf(ifname, 32, "%s", "---");
            }
            else
            {
                ifm_get_name_by_ifindex(pnhp->ifindex, ifname);
            }

			ROUTE_LINE_CHECK(current_row, vty->flush_cp)
			{
				//vty_out(vty, "%-11s:%-46s %-12s:%-7s%s", "Nexthop", nexthop, "Action", action, VTY_NEWLINE);
				vty_out(vty, "%-11s:%-46s %-12s:%-7d%s", "Nexthop", nexthop, "Vpn", pfib->vpn, VTY_NEWLINE);
			}

			ROUTE_LINE_CHECK(current_row, vty->flush_cp)
			{
				//vty_out(vty, "%-11s:%s%s", "Interface", ifname, VTY_NEWLINE);
				vty_out(vty, "%-11s:%-46s %-12s:%-7s%s", "Interface", ifname, "Type", type, VTY_NEWLINE);
			}
        }
    }
    else
    {
        pnhp = (struct nhp_entry *)nhp_info;

        memset(nexthop, 0, sizeof(nexthop));
		if(INET_FAMILY_IPV4 == pnhp->nexthop.type)
		{
			inet_ipv4tostr(pnhp->nexthop.addr.ipv4, nexthop);
		}
		else if(INET_FAMILY_IPV6 == pnhp->nexthop.type)
		{
			inet_ipv6tostr((struct ipv6_addr *)(pnhp->nexthop.addr.ipv6), nexthop, IPV6_ADDR_STRLEN);
		}

        //memset(action, 0, sizeof(action));
        //convert_nhp_action_to_string(pnhp->action, action);

        memset(ifname, 0, sizeof(ifname));
        if(0 == pnhp->ifindex || NHP_TYPE_LSP == pnhp->nhp_type)
        {
            snprintf(ifname, 32, "%s", "---");
        }
        else
        {
            ifm_get_name_by_ifindex(pnhp->ifindex, ifname);
        }

		ROUTE_LINE_CHECK(current_row, vty->flush_cp)
		{
			//vty_out(vty, "%-11s:%-46s %-12s:%-7s%s", "Nexthop", nexthop, "Action", action, VTY_NEWLINE);
			vty_out(vty, "%-11s:%-46s %-12s:%-7d%s", "Nexthop", nexthop, "Vpn", pfib->vpn, VTY_NEWLINE);
		}

		ROUTE_LINE_CHECK(current_row, vty->flush_cp)
		{
			//vty_out(vty, "%-11s:%s%s", "Interface", ifname, VTY_NEWLINE);
			vty_out(vty, "%-11s:%-46s %-12s:%-7s%s", "Interface", ifname, "Type", type, VTY_NEWLINE);
		}
    }

    return 0;
}

DEFUN(show_ftm_fib_all,
    show_ftm_fib_all_cmd,
    "show ftm fib",
    SHOW_STR
    "Ftm information\n"
    "Forwarding information base\n")
{
    int i, ret = 0, print_hdr = 1;
    char buf[32];
    struct ptree_node *node = NULL;
    struct fib_entry *pfib = NULL;
    void *nhp_info = NULL;

    if(0xFFFFFFFF == vty->flush_cp)
    {
        return CMD_SUCCESS;
    }

    for(i = 0; i < L3VPN_SIZE+1; i++)
    {
        PTREE_LOOP(fib_tree[i], pfib, node)
        {
            memset(buf, 0, sizeof(buf));
            convert_nhp_type_to_string(pfib->nhp.nhp_type, buf);

            nhp_info = nhp_lookup(pfib->nhp.nhp_index, pfib->nhp.nhp_type);
            if(NULL == nhp_info)
            {
                continue;
            }

            ret = ftm_fib_entry_print(vty, pfib, nhp_info, print_hdr);
            if(CMD_CONTINUE == ret)
            {
                return ret;
            }

            print_hdr = 0;
        }
    }

    current_row = 0;

    return CMD_SUCCESS;
}

DEFUN(show_ftm_fib,
    show_ftm_fib_cmd,
    "show ftm fib ip (A.B.C.D | A.B.C.D/<0-32>) {vpn <1-1024>}",
    SHOW_STR
    "Ftm information\n"
    "Forwarding information base\n"
    IP_STR
    "IP address\n"
    "IP address and Length of ip address mask\n"
    "Vpn information\n"
    "Vpn value\n")
{
    int ret = 0;
    int vpn = 0;
    char buf[32];
    struct prefix_ipv4 prefix;
    struct fib_entry *pfib = NULL;
    void *nhp_info = NULL;

    if(NULL == argv[0])
    {
        vty_error_out(vty, "IP address is required, please enter.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memset(&prefix, 0, sizeof(struct prefix_ipv4));
    ret = str2prefix_ipv4(argv[0], &prefix);
    if(0 == ret)
    {
        vty_error_out(vty, "The IP address you provided is malformed.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	prefix.prefix.s_addr = ntohl(prefix.prefix.s_addr);

    if(NULL != argv[1])
    {
        vpn = atoi(argv[1]);
    }

    if((0 > vpn) || (vpn > 1024))
    {
        vty_error_out(vty, "The vpn value you entered more than limit.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    pfib = ftm_fib_lookup(prefix.prefix.s_addr, prefix.prefixlen, vpn);
    if(NULL == pfib)
    {
        return CMD_WARNING;
    }

    memset(buf, 0, sizeof(buf));
    convert_nhp_type_to_string(pfib->nhp.nhp_type, buf);

    nhp_info = nhp_lookup(pfib->nhp.nhp_index, pfib->nhp.nhp_type);
    if(NULL == nhp_info)
    {
        vty_error_out(vty, "Failed to get nhp information, nhp_index=%d nhp_type=%s.%s", pfib->nhp.nhp_index, buf, VTY_NEWLINE);
        return CMD_WARNING;
    }

    ret = ftm_fib_entry_print(vty, pfib, nhp_info, 2);
    if(ret)
    {
        vty_error_out(vty, "Faild to print fib entry information.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(show_ftm_fibv6_all,
    show_ftm_fibv6_all_cmd,
    "show ftm ipv6 fib",
    SHOW_STR
    "Ftm information\n"
	"IPV6 configuration informaton\n"
    "Forwarding information base\n")
{
    int i, ret = 0, print_hdr = 1;
    struct ptree_node *node = NULL;
    struct fibv6_entry *pfib = NULL;
    void *nhp_info = NULL;

    if(0xFFFFFFFF == vty->flush_cp)
    {
        return CMD_SUCCESS;
    }

    for(i = 0; i < L3VPN_SIZE+1; i++)
    {
        PTREE_LOOP(fibv6_tree[i], pfib, node)
        {
            nhp_info = nhp_lookup(pfib->nhp.nhp_index, pfib->nhp.nhp_type);
            if(NULL == nhp_info)
            {
                continue;
            }

            ret = ftm_fibv6_entry_print(vty, pfib, nhp_info, print_hdr);
            if(CMD_CONTINUE == ret)
            {
                return ret;
            }

            print_hdr = 0;
        }
    }

    current_row = 0;

    return CMD_SUCCESS;
}

DEFUN(show_ftm_fibv6,
    show_ftm_fibv6_cmd,
    "show ftm ipv6 fib (X:X:X:X:X:X:X:X | X:X:X:X:X:X:X:X/<0-128>) {vpn <1-1024>}",
    SHOW_STR
    "Ftm information\n"
	"IPV6 configuration information\n"
    "Forwarding information base\n"
    "IPV6 address\n"
    "IPV6 address and Length of ip address mask\n"
    "Vpn information\n"
    "Vpn value\n")
{
    int ret = 0;
    int vpn = 0;
    char buf[32];
    struct prefix_ipv6 prefix;
    struct fibv6_entry *pfib = NULL;
    void *nhp_info = NULL;

    if(NULL == argv[0])
    {
        vty_error_out(vty, "IP address is required, please enter.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memset(&prefix, 0, sizeof(struct prefix_ipv6));
    ret = str2prefix_ipv6(argv[0], &prefix);
    if(0 == ret)
    {
        vty_error_out(vty, "The IP address you provided is malformed.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if(NULL != argv[1])
    {
        vpn = atoi(argv[1]);
    }

    if((0 > vpn) || (vpn > 1024))
    {
        vty_error_out(vty, "The vpn value you entered more than limit.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    pfib = ftm_fibv6_lookup((struct ipv6_addr *)prefix.prefix.s6_addr, prefix.prefixlen, vpn);
    if(NULL == pfib)
    {
        return CMD_WARNING;
    }

    memset(buf, 0, sizeof(buf));
    convert_nhp_type_to_string(pfib->nhp.nhp_type, buf);

    nhp_info = nhp_lookup(pfib->nhp.nhp_index, pfib->nhp.nhp_type);
    if(NULL == nhp_info)
    {
        vty_error_out(vty, "Failed to get nhp information, nhp_index=%d nhp_type=%s.%s", pfib->nhp.nhp_index, buf, VTY_NEWLINE);
        return CMD_WARNING;
    }

    ret = ftm_fibv6_entry_print(vty, pfib, nhp_info, 2);
    if(ret)
    {
        vty_error_out(vty, "Faild to print ipv6 fib entry information.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

static void ftm_pw_show(struct vty *vty, struct pw_info *ppwinfo)
{
    if (NULL == ppwinfo)
    {
        return;
    }

    vty_out(vty, "%-20s: %x%s", "ifindex", ppwinfo->ifindex, VTY_NEWLINE);
    vty_out(vty, "%-20s: %x%s", "nhp index", ppwinfo->nhp_index, VTY_NEWLINE);
    vty_out(vty, "%-20s: %s%s", "PW status", ppwinfo->up_flag == LINK_UP ? "up" :
                "down", VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "ms-pw inedx", ppwinfo->mspw_index, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "vsi id", ppwinfo->vpls.vsi_id, VTY_NEWLINE);
    vty_out(vty, "%-20s: %s%s", "protocol", ppwinfo->protocol == PW_PROTO_SVC ?
                "svc" : ppwinfo->protocol == PW_PROTO_MARTINI ? "martini" :
                "-", VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "VC ID", ppwinfo->vcid, VTY_NEWLINE);
    vty_out(vty, "%-20s: %s%s", "VC type", ppwinfo->ac_type == AC_TYPE_ETH ?
                "ethernet" : ppwinfo->ac_type == AC_TYPE_TDM ? "tdm" :
                "-", VTY_NEWLINE);
    vty_out(vty, "%-20s: %s%s", "encapsulate", ppwinfo->tag_flag == 0 ? "raw" :
                    "tag", VTY_NEWLINE);
    vty_out(vty, "%-20s: %#x%s", "tpid", ppwinfo->tpid, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "vlan", ppwinfo->vlan, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "mtu", ppwinfo->mtu, VTY_NEWLINE);
    vty_out(vty, "%-20s: %s%s", "ctrl-word", ppwinfo->ctrlword_flag == ENABLE ?
                "enable" : "disable", VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "ces frame-num", ppwinfo->ces.frame_num, VTY_NEWLINE);
    vty_out(vty, "%s", VTY_NEWLINE);
}


DEFUN(show_ftm_pw_index,
	show_ftm_pw_index_cmd,
	"show ftm pw {index <1-4294967295>}",
	SHOW_STR
	"Forward soft management\n"
	"Pseudo wire\n"
	"Index for PW\n"
	"<1-4294967295>\n")
{
	struct pw_info     *ppwinfo = NULL;
    struct hash_bucket *pbucket = NULL;
    int cursor;
    uint32_t pwindex = 0;

	if (NULL == argv[0])
	{
		HASH_BUCKET_LOOP(pbucket, cursor, ftm_pw_table)
		{
            ppwinfo = (struct pw_info *)pbucket->data;

            ftm_pw_show(vty, ppwinfo);
        }
	}
    else
    {
        pwindex = atoi(argv[0]);
        ppwinfo = ftm_pw_lookup(pwindex);
        ftm_pw_show(vty, ppwinfo);
    }

	return CMD_SUCCESS;
}


void ftm_ifm_config_show ( struct vty *vty,  struct ftm_ifm *p_ftm_ifm )
{
	char ifname[IFNET_NAMESIZE] = "";

	vty_out ( vty, "=============================================%s", VTY_NEWLINE );

	ifm_get_name_by_ifindex ( p_ftm_ifm->ifm.ifindex, ifname );

	vty_out ( vty, "Interface name: %s %s", ifname, VTY_NEWLINE );
	vty_out ( vty, "Ifindex 	  : 0x%0x %s", p_ftm_ifm->ifm.ifindex, VTY_NEWLINE );
	vty_out ( vty, "Parent ifindex: 0x%0x %s", p_ftm_ifm->ifm.parent, VTY_NEWLINE );
	vty_out ( vty, "Unit/Slot/Port/SubPort : (%d/%d/%d/%d) %s",
		p_ftm_ifm->ifm.unit, p_ftm_ifm->ifm.slot, p_ftm_ifm->ifm.port,
		p_ftm_ifm->ifm.sub_port, VTY_NEWLINE );

	vty_out ( vty, "Type	   : %d(%s)%s", p_ftm_ifm->ifm.type, ifm_get_typestr(p_ftm_ifm->ifm.type), VTY_NEWLINE );
	vty_out ( vty, "Sub type   : %d %s", p_ftm_ifm->ifm.sub_type, VTY_NEWLINE );
	vty_out ( vty, "Mode	   : %d(%s) %s", p_ftm_ifm->ifm.mode,
		p_ftm_ifm->ifm.mode == IFNET_MODE_L3 ? "L3" :
		(p_ftm_ifm->ifm.mode == IFNET_MODE_L2 ? "L2" :
		(p_ftm_ifm->ifm.mode == IFNET_MODE_SWITCH ? "switch" : 
		(p_ftm_ifm->ifm.mode == IFNET_MODE_PHYSICAL ? "physical" : "unkown"))), VTY_NEWLINE );
	vty_out ( vty, "Shutdown   : %d(%s) %s", p_ftm_ifm->ifm.shutdown,
		p_ftm_ifm->ifm.shutdown == IFNET_NO_SHUTDOWN ? "no shutdown":"shutdown", VTY_NEWLINE );
	vty_out ( vty, "LinkStatus : %d(%s) %s", p_ftm_ifm->ifm.status,
		p_ftm_ifm->ifm.status == IFNET_LINKUP ? "up":"down", VTY_NEWLINE );
	vty_out ( vty, "Statistics : %d(%s) %s", p_ftm_ifm->ifm.statistics,
		p_ftm_ifm->ifm.statistics == IFNET_STAT_DISABLE ? "disable" : "enable", VTY_NEWLINE );
	vty_out ( vty, "TPID	   : 0x%0x %s", p_ftm_ifm->ifm.tpid, VTY_NEWLINE );
	vty_out ( vty, "Encap type : %d(%s) %s", p_ftm_ifm->ifm.encap.type,
		p_ftm_ifm->ifm.encap.type == IFNET_ENCAP_UNTAG ?"untag":
		(p_ftm_ifm->ifm.encap.type == IFNET_ENCAP_DOT1Q ?"dot1q":
		(p_ftm_ifm->ifm.encap.type == IFNET_ENCAP_QINQ ?"qinq":
		(p_ftm_ifm->ifm.encap.type == IFNET_ENCAP_VLANIF ?"vlanif":"unkown"))), VTY_NEWLINE );
	vty_out ( vty, "MTU 	   : %d %s", p_ftm_ifm->ifm.mtu, VTY_NEWLINE );
	vty_out ( vty, "JUMBO	   : %d %s", p_ftm_ifm->ifm.jumbo, VTY_NEWLINE );
	vty_out ( vty, "CVLAN	   : (%d-%d) %s", p_ftm_ifm->ifm.encap.cvlan.vlan_start, p_ftm_ifm->ifm.encap.cvlan.vlan_end, VTY_NEWLINE );
	vty_out ( vty, "SVLAN	   : (%d-%d) %s", p_ftm_ifm->ifm.encap.svlan.vlan_start, p_ftm_ifm->ifm.encap.svlan.vlan_end, VTY_NEWLINE );
	vty_out ( vty, "CVLAN act  : %d %s", p_ftm_ifm->ifm.encap.cvlan_act, VTY_NEWLINE );
	vty_out ( vty, "SVLAN act  : %d %s", p_ftm_ifm->ifm.encap.svlan_act, VTY_NEWLINE );
	vty_out ( vty, "New CVLAN  : (%d-%d) %s", p_ftm_ifm->ifm.encap.cvlan_new.vlan_start, p_ftm_ifm->ifm.encap.cvlan_new.vlan_end, VTY_NEWLINE );
	vty_out ( vty, "New SVLAN  : (%d-%d) %s", p_ftm_ifm->ifm.encap.svlan_new.vlan_start, p_ftm_ifm->ifm.encap.svlan_new.vlan_end, VTY_NEWLINE );
	vty_out ( vty, "CVLAN cos  : %d %s", p_ftm_ifm->ifm.encap.cvlan_cos, VTY_NEWLINE );
	vty_out ( vty, "SVLAN cos  : %d %s", p_ftm_ifm->ifm.encap.svlan_cos, VTY_NEWLINE );
	vty_out ( vty, "Ethernet address: %02x:%02x:%02x:%02x:%02x:%02x  %s",
		  p_ftm_ifm->ifm.mac[0], p_ftm_ifm->ifm.mac[1],
		  p_ftm_ifm->ifm.mac[2], p_ftm_ifm->ifm.mac[3],
		  p_ftm_ifm->ifm.mac[4], p_ftm_ifm->ifm.mac[5], VTY_NEWLINE );
	vty_out ( vty, "=============================================%s", VTY_NEWLINE );
}



DEFUN ( show_ftm_if,
        show_ftm_if_cmd,
		"show ftm interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP|tdm USP|stm USP|vlanif <1-4094>| trunk TRUNK|loopback <0-128>|tunnel USP|clock <1-2>}",
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
    struct ftm_ifm *pifm = NULL;
    struct hash_bucket *pbucket = NULL;
    int cursor;

    if ( argv[0] != NULL )
    {
        if ( strncmp ( argv[0], "eth", 3 ) == 0 )
        {
            HASH_BUCKET_LOOP ( pbucket, cursor, ftm_ifm_table)
            {
                pifm = pbucket->data;
                if ( pifm->ifm.type == IFNET_TYPE_ETHERNET )
                { ftm_ifm_config_show ( vty, pifm ); }
            }
            return CMD_SUCCESS;

        }
		else if ( strncmp ( argv[0], "giga", 3 ) == 0 )
        {
            HASH_BUCKET_LOOP ( pbucket, cursor, ftm_ifm_table)
            {
                pifm = pbucket->data;
                if ( pifm->ifm.type == IFNET_TYPE_GIGABIT_ETHERNET )
                { ftm_ifm_config_show ( vty, pifm ); }
            }
            return CMD_SUCCESS;

        }
		else if ( strncmp ( argv[0], "xgiga", 3 ) == 0 )
        {
            HASH_BUCKET_LOOP ( pbucket, cursor, ftm_ifm_table)
            {
                pifm = pbucket->data;
                if ( pifm->ifm.type == IFNET_TYPE_XGIGABIT_ETHERNET )
                { ftm_ifm_config_show ( vty, pifm ); }
            }
            return CMD_SUCCESS;

        }
        else if ( strncmp ( argv[0], "tdm", 3 ) == 0 )
        {
            HASH_BUCKET_LOOP ( pbucket, cursor, ftm_ifm_table)
            {
                pifm = pbucket->data;
                if ( pifm->ifm.type == IFNET_TYPE_TDM )
                { ftm_ifm_config_show ( vty, pifm ); }
            }
            return CMD_SUCCESS;

        }
        else if ( strncmp ( argv[0], "stm", 3 ) == 0 )
        {
            HASH_BUCKET_LOOP ( pbucket, cursor, ftm_ifm_table)
            {
                pifm = pbucket->data;
                if ( pifm->ifm.type == IFNET_TYPE_STM )
                { ftm_ifm_config_show ( vty, pifm ); }
            }
            return CMD_SUCCESS;

        }
        else if ( strncmp ( argv[0], "loopback", 4 ) == 0 )
        {
            HASH_BUCKET_LOOP ( pbucket, cursor, ftm_ifm_table)
            {
                pifm = pbucket->data;
                if ( pifm->ifm.type == IFNET_TYPE_LOOPBACK )
                { ftm_ifm_config_show ( vty, pifm ); }
            }
            return CMD_SUCCESS;
        }
        else if ( strncmp ( argv[0], "trunk", 4 ) == 0 )
        {
            HASH_BUCKET_LOOP ( pbucket, cursor, ftm_ifm_table)
            {
                pifm = pbucket->data;
                if ( pifm->ifm.type == IFNET_TYPE_TRUNK )
                { ftm_ifm_config_show ( vty, pifm ); }
            }
            return CMD_SUCCESS;
        }
        else if ( strncmp ( argv[0], "vlanif", 4 ) == 0 )
        {
            HASH_BUCKET_LOOP ( pbucket, cursor, ftm_ifm_table)
            {
                pifm = pbucket->data;
                if ( pifm->ifm.type == IFNET_TYPE_VLANIF )
                { ftm_ifm_config_show ( vty, pifm ); }
            }
            return CMD_SUCCESS;
        }
        else if ( strncmp ( argv[0], "tunnel", 4 ) == 0 )
        {
            HASH_BUCKET_LOOP ( pbucket, cursor, ftm_ifm_table)
            {
                pifm = pbucket->data;
                if ( pifm->ifm.type == IFNET_TYPE_TUNNEL )
                { ftm_ifm_config_show ( vty, pifm ); }
            }
            return CMD_SUCCESS;
        }
        else if ( strncmp ( argv[0], "clock", 4 ) == 0 )
        {
            HASH_BUCKET_LOOP ( pbucket, cursor, ftm_ifm_table)
            {
                pifm = pbucket->data;
                if ( pifm->ifm.type == IFNET_TYPE_CLOCK )
                { ftm_ifm_config_show ( vty, pifm ); }
            }
            return CMD_SUCCESS;
        }

        ifindex = ifm_get_ifindex_by_name ( "ethernet", (char *)argv[0] );
    }
	else if ( argv[1] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "gigabitethernet", (char *)argv[1] );
    }
	else if ( argv[2] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "xgigabitethernet", (char *)argv[2] );
    }
    else if ( argv[3] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "tdm", (char *)argv[3] );
    }
    else if ( argv[4] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "stm", (char *)argv[4] );
    }
    else if ( argv[5] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "vlanif", (char *)argv[5] );
    }
    else if ( argv[6] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "trunk", (char *)argv[6] );
    }
    else if ( argv[7] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "loopback", (char *)argv[7] );
    }
    else if ( argv[8] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "tunnel", (char *)argv[8] );
    }
    else if ( argv[9] != NULL )
    {
        ifindex = ifm_get_ifindex_by_name ( "clock", (char *)argv[9] );
    }
    else
    {
        HASH_BUCKET_LOOP ( pbucket, cursor, ftm_ifm_table)
        {
            pifm = pbucket->data;
            ftm_ifm_config_show ( vty, pifm );
        }

        return CMD_SUCCESS;
    }

    pifm = ftm_ifm_lookup ( ifindex );
    if ( !pifm )
    {
        vty_out ( vty, "%%This interface not exist%s", VTY_NEWLINE );
        return IFM_ERR_NOT_FOUND;
    }

    ftm_ifm_config_show ( vty, pifm );

    return CMD_SUCCESS;
}

ALIAS ( show_ftm_if,
		show_ftm_if_all_cmd,
		"show ftm interface (ethernet|gigabitethernet|xgigabitethernet|tdm|stm|vlanif|trunk|loopback|tunnel|clock)",
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


static void ftm_ilm_show(struct vty *vty, struct ilm_entry *pilm)
{
    if (pilm == NULL)
    {
        return;
    }

    vty_out(vty, "%-20s: %u%s", "inlabel", pilm->inlabel, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "lsp index", pilm->lsp_index, VTY_NEWLINE);
    vty_out(vty, "%-20s: %x%s", "ac index", pilm->ac_index, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "pw index", pilm->pw_index, VTY_NEWLINE);
    vty_out(vty, "%-20s: %x%s", "group index", pilm->group_index, VTY_NEWLINE);
    vty_out(vty, "%-20s: %x%s", "nhp index", pilm->nhp_index, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "nhp type", pilm->nhp_type, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "lsp type", pilm->lsp_type, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "action", pilm->action, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "vpnid", pilm->vpnid, VTY_NEWLINE);

    if ((pilm->car_cir&CAR_VALUE_L1_FLAG) == CAR_VALUE_L1_FLAG)
    {
        vty_out(vty, "%-20s: %u%s", "car cir l1",
                    pilm->car_cir&(~CAR_VALUE_L1_FLAG), VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "%-20s: %u%s", "car cir", pilm->car_cir, VTY_NEWLINE);
    }
    
    vty_out(vty, "%-20s: %u%s", "car pir", pilm->car_pir, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "domain id", pilm->domain_id, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "statistics", pilm->statis_enable, VTY_NEWLINE);
    vty_out(vty, "%s", VTY_NEWLINE);
}


DEFUN(show_ftm_ilm,
	show_ftm_ilm_cmd,
	"show ftm ilm {label <16-1015807>}",
    SHOW_STR
	"Forward soft management\n"
	"Incoming label map\n"
	"Label for ILM\n"
	"<16-1015807>\n")
{
	struct ilm_entry   *pilm    = NULL;
    struct hash_bucket *pbucket = NULL;
    int cursor;
    uint32_t label = 0;

	if (NULL == argv[0])
	{
		HASH_BUCKET_LOOP(pbucket, cursor, ilm_table)
		{
            pilm = (struct ilm_entry *)pbucket->data;

            ftm_ilm_show(vty, pilm);
        }
	}
    else
    {
        label = atoi(argv[0]);
        pilm = ftm_ilm_lookup(label);
        ftm_ilm_show(vty, pilm);
    }

	return CMD_SUCCESS;
}


static void ftm_nhlfe_show(struct vty *vty, struct nhlfe_entry *pnhlfe)
{
    if (NULL == pnhlfe)
    {
        return ;
    }

    vty_out(vty, "%-20s: %u%s", "lsp index", pnhlfe->lsp_index, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "out label", pnhlfe->outlabel, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "pw index", pnhlfe->pw_index, VTY_NEWLINE);
    vty_out(vty, "%-20s: %x%s", "group index", pnhlfe->group_index, VTY_NEWLINE);
    vty_out(vty, "%-20s: %x%s", "nhp index", pnhlfe->nhp_index, VTY_NEWLINE);
    vty_out(vty, "%-20s: %x%s", "nexthop", pnhlfe->nexthop, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "nhp type", pnhlfe->nhp_type, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "lsp type", pnhlfe->lsp_type, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "action", pnhlfe->action, VTY_NEWLINE);
    vty_out(vty, "%-20s: %02x:%02x:%02x:%02x:%02x:%02x%s", "dmac",
                        pnhlfe->dmac[0], pnhlfe->dmac[1], pnhlfe->dmac[2],
                        pnhlfe->dmac[3], pnhlfe->dmac[4], pnhlfe->dmac[5], VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "mplstp oam id", pnhlfe->mplsoam_id, VTY_NEWLINE);

    if ((pnhlfe->car_cir & CAR_VALUE_L1_FLAG) == CAR_VALUE_L1_FLAG)
    {
        vty_out(vty, "%-20s: %u%s", "car cir l1",
                    pnhlfe->car_cir&(~CAR_VALUE_L1_FLAG), VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "%-20s: %u%s", "car cir", pnhlfe->car_cir, VTY_NEWLINE);
    }
    
    vty_out(vty, "%-20s: %u%s", "car pir", pnhlfe->car_pir, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "hqos id", pnhlfe->hqos_id, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "phb enable", pnhlfe->phb_enable, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "phb id", pnhlfe->phb_id, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "flag", pnhlfe->flag, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "down flag", pnhlfe->down_flag, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "statistics", pnhlfe->statis_enable, VTY_NEWLINE);
    vty_out(vty, "%s", VTY_NEWLINE);

}


DEFUN(show_ftm_nhlfe,
	show_ftm_nhlfe_cmd,
	"show ftm nhlfe {index <10001-15000>}",
	SHOW_STR
	"Forward soft management\n"
	"Next hop label forward entry\n"
	"Index for NHLFE\n"
	"<10001-15000>\n")
{
	struct nhlfe_entry *pnhlfe  = NULL;
    struct hash_bucket *pbucket = NULL;
    int cursor;
    uint32_t index = 0;

	if (NULL == argv[0])
	{
		HASH_BUCKET_LOOP(pbucket, cursor, nhlfe_table)
		{
            pnhlfe = (struct nhlfe_entry *)pbucket->data;

            ftm_nhlfe_show(vty, pnhlfe);
        }
	}
    else
    {
        index = atoi(argv[0]);
        pnhlfe = ftm_nhlfe_lookup(index);
        ftm_nhlfe_show(vty, pnhlfe);
    }

	return CMD_SUCCESS;
}


static void ftm_tunnel_show(struct vty *vty, struct tunnel_t *ptunnel)
{
    if (NULL == ptunnel)
    {
        return ;
    }

    vty_out(vty, "%-20s: %x%s", "ifindex", ptunnel->ifindex, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "protocol", ptunnel->protocol, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "backup status", ptunnel->backup_status, VTY_NEWLINE);
    vty_out(vty, "%-20s: %x%s", "sip", ptunnel->sip.addr.ipv4, VTY_NEWLINE);
    vty_out(vty, "%-20s: %x%s", "dip", ptunnel->dip.addr.ipv4, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "master index", ptunnel->master_index, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "backup index", ptunnel->backup_index, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "master egr index", ptunnel->master_egress_index, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "backup egr index", ptunnel->backup_egress_index, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "mplstp oam id", ptunnel->mplsoam_id, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "bfd id", ptunnel->bfd_id, VTY_NEWLINE);

    if ((ptunnel->car_cir[0]&CAR_VALUE_L1_FLAG) == CAR_VALUE_L1_FLAG)
    {
        vty_out(vty, "%-20s: %u%s", "ingress car cir l1",
                    ptunnel->car_cir[0]&(~CAR_VALUE_L1_FLAG), VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "%-20s: %u%s", "ingress car cir", ptunnel->car_cir[0], VTY_NEWLINE);
    }
    
    vty_out(vty, "%-20s: %u%s", "ingress car pir", ptunnel->car_pir[0], VTY_NEWLINE);

    if ((ptunnel->car_cir[1]&CAR_VALUE_L1_FLAG) == CAR_VALUE_L1_FLAG)
    {
        vty_out(vty, "%-20s: %u%s", "egress car cir l1",
                    ptunnel->car_cir[1]&(~CAR_VALUE_L1_FLAG), VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "%-20s: %u%s", "egress car cir", ptunnel->car_cir[1], VTY_NEWLINE);
    }
    
    vty_out(vty, "%-20s: %u%s", "egress car pir", ptunnel->car_pir[1], VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "hqos id", ptunnel->hqos_id, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "domain id", ptunnel->domain_id, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "phb enable", ptunnel->phb_enable, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "phb id", ptunnel->phb_id, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "down flag", ptunnel->down_flag, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "statistics", ptunnel->statis_enable, VTY_NEWLINE);
    vty_out(vty, "%-20s: %x%s", "nexthop", ptunnel->nexthop, VTY_NEWLINE);
    vty_out(vty, "%-20s: %x%s", "outif", ptunnel->outif, VTY_NEWLINE);
    vty_out(vty, "%s", VTY_NEWLINE);
}


DEFUN(show_ftm_tunnel,
	show_ftm_tunnel_cmd,
	"show ftm tunnel [USP]",
	SHOW_STR
	"Forward soft management\n"
	CLI_INTERFACE_TUNNEL_STR
	CLI_INTERFACE_TUNNEL_VHELP_STR)
{
    struct ftm_ifm     *pifm    = NULL;
    struct hash_bucket *pbucket = NULL;
    int cursor;
    uint32_t ifindex = 0;

    if (NULL == argv[0])
    {
        HASH_BUCKET_LOOP(pbucket, cursor, ftm_ifm_table)
        {
            pifm = (struct ftm_ifm *)pbucket->data;
            if (NULL == pifm)
            {
                continue;
            }

            ftm_tunnel_show(vty, pifm->ptunnel);
        }
    }
    else
    {
        ifindex = ifm_get_ifindex_by_name((char *)"tunnel", (char *)argv[0]);
        pifm = ftm_ifm_lookup(ifindex);
        if (pifm != NULL)
        {
            ftm_tunnel_show(vty, pifm->ptunnel);
        }
    }

    return CMD_SUCCESS;
}


DEFUN(debug_ftm_fib_add,
      debug_ftm_fib_add_cmd,
      "debug ftm fib add prefix (A.B.C.D | A.B.C.D/<0-32>) nhp <1-4096> {vpn <1-1024>}",
      "Debug command\n"
      "Ftm information\n"
      "Fib information\n"
      "Add command\n"
      "Prefix information\n"
      "Ip address\n"
      "Ip address and mask"
      "Nhp index\n"
      "<1-4096>"
      "Vpn\n"
      "<1-1024>\n"
      )
{
    int ret = 0;
    int nhp_index = 0;
    int vpn = 0;
    struct prefix_ipv4 p;
	struct fib_entry fib;
    struct nhp_entry *pnhp = NULL;

    if(NULL == argv[0])
    {
        vty_error_out(vty, "Prefix information is required, please enter.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if(NULL == argv[1])
    {
        vty_error_out(vty, "The nhp index is required, please enter.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memset(&p, 0, sizeof(struct prefix_ipv4));
    ret = str2prefix_ipv4(argv[0], &p);
    if(0 == ret)
    {
        vty_error_out(vty, "The prefix information you provided is malformed.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    nhp_index = atoi(argv[1]);
    if((1 > nhp_index) || (nhp_index > 4096))
    {
        vty_error_out(vty, "The nhp index you entered more than limit.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if(NULL != argv[2])
    {
        vpn = atoi(argv[2]);
    }

    if((0 > vpn) || (vpn > 1024))
    {
        vty_error_out(vty, "The vpn value you entered more than limit.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    pnhp = ftm_nhp_lookup(nhp_index);
    if(NULL == pnhp)
    {
        vty_error_out(vty, "Lookup nhp entry failure.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	memset(&fib, 0, sizeof(struct fib_entry));
	fib.ipaddr = p.prefix.s_addr;
	fib.masklen = p.prefixlen;
	fib.vpn = vpn;
	fib.nhp.nhp_index = pnhp->nhp_index;
	fib.nhp.nhp_type = pnhp->nhp_type;

    ret = ftm_fib_add(&fib);
    if(ret)
    {
        vty_error_out(vty, "Add fib entry failure.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    vty_out(vty, "Add fib entry successful.%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}

DEFUN(debug_ftm_fib_delete,
      debug_ftm_fib_delete_cmd,
      "debug ftm fib delete prefix (A.B.C.D | A.B.C.D/<0-32>) {vpn <1-1024>}",
      "Debug command\n"
      "Ftm information\n"
      "Fib information\n"
      "Delete command\n"
      "Prefix information\n"
      "Prefix ip adress\n"
      "Prefix ip address and mask\n"
      "Vpn\n"
      "<1-1024>\n"
      )
{
    int ret = 0;
    int vpn = 0;
    struct prefix_ipv4 p;
	struct fib_entry fib;

    if(NULL == argv[0])
    {
        vty_error_out(vty, "Prefix information is required, please enter.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memset(&p, 0, sizeof(struct prefix_ipv4));
    ret = str2prefix_ipv4(argv[0], &p);
    if(0 == ret)
    {
        vty_error_out(vty, "The prefix information you provided is malformed.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if(NULL != argv[1])
    {
        vpn = atoi(argv[1]);
    }

    if((0 > vpn) || (vpn > 1024))
    {
        vty_error_out(vty, "The vpn value you entered more than limit.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	memset(&fib, 0, sizeof(struct fib_entry));
	fib.ipaddr = p.prefix.s_addr;
	fib.masklen = p.prefixlen;
	fib.vpn = vpn;

	ret = ftm_fib_delete(&fib);
	if(ret)
    {
        vty_error_out(vty, "Delete fib entry failure.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    vty_out(vty, "Delete fib entry successful.%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}

DEFUN(debug_ftm_nhp_add,
      debug_ftm_nhp_add_cmd,
      "debug ftm nhp add index <1-4096> nexthop A.B.C.D ouif (ethernet|gigabitethernet|xgigabitethernet|tdm|stm) USP nhp_type <1-100> {vpn <1-1024>}",
      "Debug command\n"
      "Ftm information\n"
      "Nhp information\n"
      "Add command\n"
      "Nhp index\n"
      "<1-4096>\n"
      "Next hop information\n"
      "Next hop\n"
      "Interface information\n"
      "Ethernet interface type\n"
      CLI_INTERFACE_GIGABIT_ETHERNET_STR
      CLI_INTERFACE_XGIGABIT_ETHERNET_STR
      "Tdm iterface type\n"
      "Stm interface type\n"
      "Port/Subport of the interface,format <0-7>/<0-31>/<0-255>[.<0-4096>]\n"
      "Nhp type\n"
      "<1-100>\n"
      "Vpn\n"
      "<1-1024>\n")
{
    int ret = 0;
    int vpn = 0;
    struct nhp_entry nhp;
    struct nhp_entry *pnhp = NULL;
    struct in_addr addr;

    memset(&nhp, 0, sizeof(struct nhp_entry));

    nhp.nhp_index = atoi(argv[0]);
    if((1 > nhp.nhp_index) || (nhp.nhp_index > 4096))
    {
        vty_error_out(vty, "The nhp index value you entered more than limit.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    pnhp = ftm_nhp_lookup(nhp.nhp_index);
    if(NULL != pnhp)
    {
        vty_error_out(vty, "The nhp entry already exists.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memset(&addr, 0, sizeof(struct in_addr));
    ret = inet_pton(AF_INET, argv[1], &addr);
    if(0 >= ret)
    {
        vty_error_out(vty, "The next hop you provided is malformed.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
	nhp.nexthop.type = INET_FAMILY_IPV4;
    nhp.nexthop.addr.ipv4 = addr.s_addr;
    nhp.ifindex = ifm_get_ifindex_by_name(argv[2], (char *)argv[3]);
    if(0 == nhp.ifindex)
    {
        vty_error_out(vty, "Get interface index failure.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    nhp.nhp_type = atoi(argv[4]);
    if((1 > nhp.nhp_type) || (nhp.nhp_type > 100))
    {
        vty_error_out(vty, "The nhp type you entered more than limit.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if(NULL != argv[5])
    {
        vpn = atoi(argv[5]);
    }

    if((0 > vpn) || (vpn > 1024))
    {
        vty_error_out(vty, "The vpn value you entered more than limit.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    nhp.vpn = vpn;

    ret = ftm_nhp_add(&nhp);
    if(ret)
    {
        vty_error_out(vty, "Add nhp entry failure.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	vty_out(vty, "Add nhp entry successful.%s", VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN(debug_ftm_nhp_delete,
      debug_ftm_nhp_delete_cmd,
      "debug ftm nhp delete index <1-4096>",
      "Debug command\n"
      "Ftm information\n"
      "Nhp information\n"
      "Delete command\n"
      "Nhp index\n"
      "<1-4096>\n")
{
    int ret = 0;
    int nhp_index = 0;

    nhp_index = atoi(argv[0]);
    if((1 > nhp_index) || (nhp_index > 4096))
    {
        vty_error_out(vty, "The nhp index you entered more than limit.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ret = ftm_nhp_delete(nhp_index);
    if(ret)
    {
        vty_error_out(vty, "Delete nhp entry failure.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    vty_out(vty, "Delete nhp entry successful.%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}

#if 0
DEFUN (debug_ftm_nhlfe_add,
       debug_ftm_nhlfe_add_cmd,
       "debug ftm nhlfe add index <1-4096>  outif (ethernet|gigabitethernet|xgigabitethernet|tdm|stm) A/B/C.H dmac HHHH:HHHH:HHHH outlabel <1-4096>",
       "Index of ftm nhlfe\n"
       "ftm\n"
       "nhlfe\n"
       "add\n"
       "index value\n"
       "<1-4096>\n"
       "out interface\n"
       "ethernet:interface type\n"
       CLI_INTERFACE_GIGABIT_ETHERNET_STR
       CLI_INTERFACE_XGIGABIT_ETHERNET_STR
       "tdm:interface type\n"
       "stm:interface type\n"
       " <0-7>/<0-31>/<0-255>.<0~4096>\n"
       "dest mac\n"
       "format HHHH:HHHH:HHHH\n"
       "out label\n"
       "label number\n"
       )
{

	int index_tmp = 0;
	int label = -1;
	int ret	= 0;
	struct nhlfe_entry *p_nhlfe = NULL;
         uchar dmac[6] = { 0 };

	index_tmp = atoi(argv[0]);
	if(index_tmp < 1 || index_tmp > 4096 )
	{
		vty_out (vty, "%% Invalid FTM_NHP_INDEX value. Supported range is %d-%d%s", 1, 4096, VTY_NEWLINE);
          	return CMD_WARNING;
	}

	p_nhlfe = ftm_nhlfe_lookup(index_tmp);
	if(NULL != p_nhlfe)
	{
		vty_out (vty, "ftm nhlfe of index %d already exist%s", index_tmp, VTY_NEWLINE);
		return CMD_ERR_NOTHING_TODO;
	}

	p_nhlfe = (struct nhlfe_entry *)malloc(sizeof(struct nhlfe_entry));
	if(NULL == p_nhlfe)
	{
		return CMD_WARNING;
	}
	memset(p_nhlfe, 0, sizeof(struct nhlfe_entry));

	p_nhlfe->lsp_index = index_tmp;
	p_nhlfe->nhp_type = NHP_TYPE_CONNECT;

	 p_nhlfe->nhp_index = ifm_get_ifindex_by_name(argv[1], (char *)argv[2]);
	 vty_out(vty, "nhp_index =%d,  ifname=%s %s\n", p_nhlfe->nhp_index, argv[0], argv[2]);

	if( 0 == p_nhlfe->nhp_index)
	{
	    free(p_nhlfe);
	    p_nhlfe = NULL;
	    vty_out(vty, "Failed to get ifindex by ifname%s", VTY_NEWLINE);

	    return CMD_WARNING;
	}

	/*dmac*/
	if ( sscanf ( argv[3], "%4hx.%4hx.%4hx",
	              ( unsigned short * ) &dmac[0],
	              ( unsigned short * ) &dmac[2],
	              ( unsigned short * ) &dmac[4] ) != 3 )
	{
	    vty_out ( vty, "%%please check out mac format%s", VTY_NEWLINE );
	    return CMD_WARNING;
	}
	memcpy(p_nhlfe->dmac, dmac, 6);

	vty_out(vty, "%d %d %d %d %d %d \n", dmac[0], dmac[1], dmac[2], dmac[3],dmac[4],dmac[5]);

	label = atoi(argv[4]);
	if(label < 0 || label > 1024 )
	{
		free(p_nhlfe);
		p_nhlfe = NULL;
      		vty_out (vty, "%% Invalid FTM_nhlfe_VRF_INDEX value. Supported range is %d-%d%s", 0, 1024, VTY_NEWLINE);

	   	return CMD_WARNING;
	}
        	p_nhlfe->outlabel = label;

	ret = ftm_nhlfe_add(p_nhlfe);
	if(ret)
	{
		free(p_nhlfe);
		p_nhlfe = NULL;
		vty_out(vty, "%% Failed to add ftm nhlfe%s", VTY_NEWLINE);

		return CMD_WARNING;
	}
	vty_out(vty, "%% Success to add ftm nhlfe%s\n", VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN (debug_ftm_pw_nhlfe_add,
		debug_ftm_pw_nhlfe_add_cmd,
		"debug ftm pw nhlfe add lspindex <1-4096> outlabel LABEL pwindex <1-4096> nhpindex <1-4096>",
		"Debug\n"
		"Ftm\n"
		"Pw\n"
		"Nhlfe\n"
		"Add\n"
		"Private lspindex\n"
		"The range of private lspindex:<1-4096>\n"
		"Outlabel of VC\n"
		"Outlabel value\n"
		"Pw index\n"
		"The range of pwindex :<1-4096>\n"
		"public lspindex of pw's nhp\n"
		"The range of public lspindex:<1-4096>\n")
{
	struct nhlfe_entry *p_nhlfe = NULL;
	uint32_t lsp_index = 0;
	int ret = 0;

	lsp_index = atoi(argv[0]);
	p_nhlfe = ftm_nhlfe_lookup(lsp_index);
	if(p_nhlfe)
	{
		vty_out (vty, "ftm nhlfe of index %d already exist%s", lsp_index, VTY_NEWLINE);

		return CMD_WARNING;
	}
	p_nhlfe = (struct nhlfe_entry *)malloc(sizeof(struct nhlfe_entry));
	if(p_nhlfe == NULL)
	{
		return CMD_WARNING;
	}
	memset(p_nhlfe, 0, sizeof(struct nhlfe_entry));
	p_nhlfe->lsp_index = lsp_index;
	p_nhlfe->outlabel = atoi(argv[1]);
	p_nhlfe->pw_index = atoi(argv[2]);
	p_nhlfe->nhp_index= atoi(argv[3]);
	p_nhlfe->lsp_type = LSP_TYPE_L2VC;
	p_nhlfe->nhp_type = NHP_TYPE_LSP;


	ret = ftm_nhlfe_add(p_nhlfe);
	if(ret)
	{
		free(p_nhlfe);
		p_nhlfe = NULL;
		vty_out(vty, "%% Failed to add ftm nhlfe%s", VTY_NEWLINE);

		return CMD_WARNING;
	}
	vty_out(vty, "%% Success to add ftm nhlfe%s\n", VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN (debug_ftm_nhlfe_delete,
	       debug_ftm_nhlfe_delete_cmd,
	       "debug ftm nhlfe delete index <1-4096>",
	       "Index of ftm nhlfe\n"
	       "ftm nhlfe delete\n")
{
	int ret = 0;
	int index_tmp = 0;
	index_tmp = atoi(argv[0]);
	if(index_tmp < 1 || index_tmp > 4096)
	{
		vty_out (vty, "index value must be <1-4096>");
		return CMD_WARNING;
	}


	ret = ftm_nhlfe_delete(index_tmp);
	if(ret)
	{
		vty_out(vty, "Failed to delete ftm nhlfe%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	vty_out(vty, "Success to delete ftm nhlfe%s", VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN (debug_ftm_ilm_add,
       debug_ftm_ilm_add_cmd,
       "debug ftm ilm add index <1-4096>  inlabel <1-4096>",
       "Index of ftm ilm\n"
       "ftm\n"
       "ilm\n"
       "add\n"
       "lsp index value\n"
       "<1-4096>\n"
       "in label\n"
       "label number\n"
       )
{

	int index_tmp = 0;
	int label = -1;
	int ret	= 0;
	struct ilm_entry *p_ilm = NULL;

	index_tmp = atoi(argv[0]);
	if(index_tmp < 1 || index_tmp > 4096 )
	{
		vty_out (vty, "%% Invalid FTM_NHP_INDEX value. Supported range is %d-%d%s", 1, 4096, VTY_NEWLINE);
          	return CMD_WARNING;
	}

         label = atoi(argv[1]);
	p_ilm = ftm_ilm_lookup(label);
	if(NULL != p_ilm)
	{
		vty_out (vty, "ftm p_ilm of label %d already exist%s", label, VTY_NEWLINE);
		return CMD_ERR_NOTHING_TODO;
	}

	p_ilm = (struct ilm_entry *)malloc(sizeof(struct ilm_entry));
	if(NULL == p_ilm)
	{
		return CMD_WARNING;
	}
	memset(p_ilm, 0, sizeof(struct ilm_entry));

	p_ilm->lsp_index= index_tmp;
	p_ilm->nhp_type = NHP_TYPE_CONNECT;
         p_ilm->inlabel = label;

	ret = ftm_ilm_add(p_ilm);
	if(ret)
	{
		free(p_ilm);
		p_ilm = NULL;
		vty_out(vty, "%% Failed to add ftm ilm%s", VTY_NEWLINE);

		return CMD_WARNING;
	}
	vty_out(vty, "%% Success to add ftm ilm%s\n", VTY_NEWLINE);

	return CMD_SUCCESS;
}
DEFUN (debug_ftm_pw_ilm_add,
	debug_ftm_pw_ilm_add_cmd,
	"debug ftm pw ilm add lspindex <1-4096> inlabel <16-4095> pwindex <1-4096>",
	"Debug\n"
	"Ftm\n"
	"Pw\n"
	"ilm\n"
	"add\n"
	"private lspindex\n"
	"The range of lspindex :<1-4096>\n"
	"Inlabel of VC\n"
	"The range of VC_inlabel:<16-4095>\n"
	"Pwindex\n"
	"the range of pwindex:<1-4096>\n")
{
	struct ilm_entry *p_ilm = NULL;
	uint32_t inlabel = 0;
	int ret = 0;

	inlabel = atoi(argv[1]);
	p_ilm = ftm_ilm_lookup(inlabel);
	if(p_ilm)
	{
		vty_out (vty, "ftm p_ilm of label %d already exist%s", inlabel, VTY_NEWLINE);

		return CMD_WARNING;
	}
	p_ilm = (struct ilm_entry *)malloc(sizeof(struct ilm_entry));
	if(p_ilm)
	{
		return CMD_WARNING;
	}
	memset(p_ilm, 0, sizeof(struct ilm_entry));
	p_ilm->lsp_index = atoi(argv[0]);
	p_ilm->inlabel = inlabel;
	p_ilm->pw_index = atoi(argv[2]);
	p_ilm->lsp_type = LSP_TYPE_L2VC;

	ret = ftm_ilm_add(p_ilm);
	if(ret)
	{
		free(p_ilm);
		p_ilm = NULL;
		vty_out(vty, "%% Failed to add ftm ilm%s", VTY_NEWLINE);

		return CMD_WARNING;
	}
	vty_out(vty, "%% Success to add ftm ilm%s\n", VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN (debug_ftm_ilm_delete,
	       debug_ftm_ilm_delete_cmd,
	       "debug ftm ilm delete label <1-4096>",
	      "Index of ftm ilm\n"
               "ftm\n"
               "ilm\n"
               "del\n"
               "label value\n")
{
	int ret = 0;
	int label = 0;
	label = atoi(argv[0]);
	if(label < 1 || label > 4096)
	{
		vty_out (vty, "label value must be <1-4096>");
		return CMD_WARNING;
	}


	ret = ftm_ilm_delete(label);
	if(ret)
	{
		vty_out(vty, "Failed to delete ftm ilm%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	vty_out(vty, "Success to delete ftm ilm%s", VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN (defug_ftm_pw_add,
		debug_ftm_pw_add_cmd,
		"debug ftm pw add pwindex <0-1024> ifindex IF_INDEX ingress_lsp IN_LSP egress_lsp E_LSP \
inlabel LABEL tpid TPID",
		"index of ftm pw\n"
		"ftm\n"
		"pw\n"
		"ifindex value <0-1024>\n"
		"lspindex value\n"
		"vcid\n"
		"inlabel numble of vc\n"
		"outlabel numbleof vc\n"
		"tpid value\n")
{
	struct pw_info *p_pw = NULL;
	uint32_t pw_index = 0;
	int ret = 0;

	pw_index = atoi(argv[0]);

	p_pw = ftm_pw_lookup(pw_index);
	if(p_pw != NULL)
	{
		vty_out(vty,"ftm:this pw is already exist!%s",VTY_NEWLINE);

		return CMD_WARNING;
	}
	p_pw = (struct pw_info *)malloc(sizeof(struct pw_info));
	if(p_pw == NULL)
	{
		vty_out(vty,"fail to malloc for ftm pw!%s",VTY_NEWLINE);

		return CMD_WARNING;
	}
	memset(p_pw,0,sizeof(struct pw_info));

	p_pw->pwindex = pw_index;
	p_pw->ifindex = atoi(argv[1]);
	p_pw->ingress_lsp = atoi(argv[2]);
	p_pw->egress_lsp = atoi(argv[3]);
//	p_pw->inlabel = atoi(argv[4]);
	p_pw->tpid = atoi(argv[5]);

	ret = ftm_pw_add(p_pw);
	if(ret == 0)
	{
		vty_out(vty,"ftm pw:add pw successful!%s",VTY_NEWLINE);

		return CMD_SUCCESS;
	}
	free(p_pw);
	p_pw = NULL;

	vty_out(vty,"ftm pw:fail to add pw!%s",VTY_NEWLINE);

	return CMD_WARNING;
}

DEFUN (debug_ftm_pw_delete,
		debug_ftm_pw_delete_cmd,
		"debug ftm pw delete index <1-4096>",
		"index of ftm pw\n"
		"ftm\n"
		"pw\n"
		"ftm pw delete\n"
		"index of pw\n")
{
	int pw_index = 0;
	struct pw_info *ppw = NULL;
	int ret = 0;

	pw_index = atoi(argv[0]);

	if((pw_index < 1) || (pw_index > 4096))
	{
		vty_out(vty,"the value of pw_index must be <1-4096>");

		return CMD_WARNING;
	}
	ppw = ftm_pw_lookup(pw_index);
	if(!ppw)
	{
		vty_out(vty,"fail to find this pwindex in the ftm_pw table!%s",VTY_NEWLINE);

		return CMD_WARNING;
	}
	ret = ftm_pw_delete(pw_index);
	if(ret == 0)
	{
		vty_out(vty,"success to delete the ftm pw!%s",VTY_NEWLINE);

		return CMD_SUCCESS;
	}
}
#endif
DEFUN (show_ftm_nhp,
	       show_ftm_nhp_cmd,
	       "show ftm nhp index <1-8192>",
           SHOW_STR
	       "Ftm information\n"
	       "Nhp information\n"
	       "Index\n"
	       "<1-8192>\n")
{
    int ret = 0;
    int nhp_index = 0;
    struct nhp_entry *pnhp = NULL;
    char ifname[IFNET_NAMESIZE];
    char mac[32];
    char buf[32];
    char type[32];
    //char action[32];
    char arp_status[32];

    nhp_index = atoi(argv[0]);
    if((1 > nhp_index) || (nhp_index > 8192))
    {
        vty_error_out(vty, "The nhp index value you entered more than limit.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    pnhp = ftm_nhp_lookup(nhp_index);
    if(NULL == pnhp)
    {
        vty_error_out(vty, "There is no nhp entry, nhp_index=%d.%s", nhp_index, VTY_NEWLINE);
        return CMD_WARNING;
    }

    memset(ifname, 0, IFNET_NAMESIZE);
    if(0 == pnhp->ifindex)
    {
        sprintf(ifname, "%s", "   ---");
    }
    else
    {
        ret = ifm_get_name_by_ifindex(pnhp->ifindex, ifname);
        if(0 > ret)
        {
            vty_error_out(vty, "Failed to get interface information.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    memset(mac, 0, sizeof(mac));
    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", pnhp->mac[0], pnhp->mac[1], pnhp->mac[2], pnhp->mac[3], pnhp->mac[4], pnhp->mac[5]);
	inet_ntop(AF_INET, &pnhp->nexthop.addr.ipv4, buf, sizeof(buf));

    memset(type, 0, sizeof(type));
    convert_nhp_type_to_string(pnhp->nhp_type, type);

    //memset(action, 0, sizeof(action));
    //convert_nhp_action_to_string(pnhp->action, action);

    memset(arp_status, 0, sizeof(arp_status));
    if(1 == pnhp->arp_flag)
    {
        strcpy(arp_status, "COMPLETE");
    }
    else
    {
        strcpy(arp_status, "INCOMPLETE");
    }

	vty_out (vty, "%-12s", "Nhp index");
	vty_out (vty, "%-20s", "Next hop");
	vty_out (vty, "%-20s", "Interface");
	vty_out (vty, "%-8s", "Vpn");
	vty_out (vty, "%-15s", "Nhp type");
	//vty_out (vty, "%-15s", "Nhp action");
    vty_out (vty, "%-15s", "Arp status");
	vty_out (vty, "%-20s%s", "Mac", VTY_NEWLINE);

    vty_out(vty, "%-12d", pnhp->nhp_index);
    vty_out(vty, "%-20s", buf);
    vty_out(vty, "%-20s", ifname);
    vty_out(vty, "%-8d", pnhp->vpn);
    vty_out(vty, "%-15s", type);
    //vty_out(vty, "%-15s", action);
    vty_out(vty, "%-15s", arp_status);
    vty_out(vty, "%-20s%s", mac, VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN (show_ftm_ecmp,
	       show_ftm_ecmp_cmd,
	       "show ftm ecmp index <1-8192>",
           SHOW_STR
	       "Ftm information\n"
	       "Ecmp information\n"
	       "Index\n"
	       "<1-8192>\n")
{
    int i;
    int ecmp_index = 0;
    struct ecmp_group *ecmp = NULL;
    char buf[32];

    ecmp_index = atoi(argv[0]);
    if((1 > ecmp_index) || (ecmp_index > 8192))
    {
        vty_error_out(vty, "The nhp index value you entered more than limit.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ecmp = ftm_ecmp_lookup(ecmp_index);
    if(NULL == ecmp)
    {
        vty_error_out(vty, "There is no nhp entry, ecmp_index=%d.%s", ecmp_index, VTY_NEWLINE);
        return CMD_WARNING;
    }

	vty_out (vty, "%-12s", "Ecmp index");
	vty_out (vty, "%-12s", "Ecmp type");
    vty_out (vty, "%-12s", "Nhp number");
    for(i = 0; i < ecmp->nhp_num; i++)
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%s%d%s", "Nhp", i, " index");
        vty_out (vty, "%-15s", buf);
    }
	vty_out (vty, "%s",  VTY_NEWLINE);

    vty_out(vty, "%-12d", ecmp->group_index);
    vty_out(vty, "%-12d", ecmp->group_type);
    vty_out(vty, "%-12d", ecmp->nhp_num);
    for(i = 0; i < ecmp->nhp_num; i++)
    {
        vty_out (vty, "%-15d", ecmp->nhp[i].nhp_index);
    }
	vty_out (vty, "%s",  VTY_NEWLINE);

	return CMD_SUCCESS;
}






/*arp 状态字符串获取*/
static char* arp_status_parse_str(uint8_t status)
{
  switch(status)
  {
  	case 0:
		return (char *)"INCOMPLETE";
	case 1:
		return (char *)"COMPLETE";
	case 2:
		return (char *)"STATIC";
	case 3:
		return (char *)"AGED";
    case 4:
        return (char *)"RULE";
	default:
		return (char *)"UNKNOW";
  }
  return (char *)"UNKNOW";
}

/*显示指定接口下的arp 表项*/
DEFUN(show_arp_interface,
	show_arp_interface_cmd,
	"show arp interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP|vlanif <1-4094>|trunk TRUNK}",
	SHOW_STR
	"Show arp info\n"
	"Interface\n"
	"Ethernet interface type\n"
	CLI_INTERFACE_ETHERNET_VHELP_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
	CLI_INTERFACE_XGIGABIT_ETHERNET_STR
	CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
	)
{
	int cursor = 0;
	int flag = 0;
	uint32_t ifindex = 0;
    struct arp_entry *arp = NULL;
    struct hash_bucket *pbucket = NULL;
    char ipv4_addr[20];
    char ifname[IFNET_NAMESIZE];
	char mac[32];


    /*参数获取及合法性检查*/
    if(NULL != argv[0] && NULL == argv[1] && NULL == argv[2]&& NULL == argv[3] && NULL == argv[4])
    {
       ifindex = ifm_get_ifindex_by_name("ethernet", (char *)argv[0]);
    }
    else if(NULL == argv[0] && NULL != argv[1] && NULL == argv[2]&& NULL == argv[3] && NULL == argv[4])
    {
       ifindex = ifm_get_ifindex_by_name("gigabitethernet", (char *)argv[1]);
    }
    else if(NULL == argv[0] && NULL == argv[1] && NULL != argv[2]&& NULL == argv[3] && NULL == argv[4])
    {
       ifindex = ifm_get_ifindex_by_name("xgigabitethernet", (char *)argv[2]);
    }
	else if(NULL == argv[0] && NULL == argv[1] && NULL == argv[2]&& NULL != argv[3] && NULL == argv[4])
    {
       ifindex = ifm_get_ifindex_by_name("vlanif", (char *)argv[3]);
    }
	else if(NULL == argv[0] && NULL == argv[1] && NULL == argv[2]&& NULL == argv[3] && NULL != argv[4])
    {
       ifindex = ifm_get_ifindex_by_name("trunk", (char *)argv[4]);
    }
	if(ifindex == 0)
	{
		vty_error_out (vty, "Get ifindex by name fail ,please check out input!%s", VTY_NEWLINE );
		return CMD_WARNING;
	}

    /*显示指定接口的arp 表项*/
	HASH_BUCKET_LOOP ( pbucket, cursor, arp_table)
 	{
    	if(pbucket->data)
 		{
    		arp = (struct arp_entry *)pbucket->data;
			if(ifindex != arp->ifindex)
				continue;

			flag += 1;
            inet_ipv4tostr(arp->key.ipaddr,ipv4_addr);
			ifm_get_name_by_ifindex (arp->ifindex ,ifname);

			if(flag == 1)
			{
				vty_out (vty, "%-15s ", "IP ADDRESS");
				vty_out (vty, "%-18s ", "MAC ADDRESS");
				vty_out (vty, "%-28s ", "INTERFACE");
				vty_out (vty, "%-4s ",  "VPN");
				vty_out (vty, "%-8s%s","STATUS",VTY_NEWLINE);
			}

			sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", arp->mac[0], arp->mac[1], arp->mac[2], arp->mac[3], arp->mac[4], arp->mac[5]);
      	 	vty_out(vty, "%-15s %-18s %-28s %-4d %-8s %s",
        	    ipv4_addr, mac,ifname,arp->key.vpnid,arp_status_parse_str(arp->status),VTY_NEWLINE);
		}
    }

    return CMD_SUCCESS;
}


/*显示指定的arp表项*/
DEFUN(show_arp,
	show_arp_cmd,
	"show arp ip A.B.C.D {l3vpn <1-1024>}",
	"Show arp info\n"
	"Arp\n"
	"IP address\n"
	"IP format A.B.C.D \n"
	"L3vpn \n"
	"L3vpn value <1-1024> \n"
	)
{
	uint32_t ipaddr = 0;
	uint16_t vpnid = 0;
    char ifname[IFNET_NAMESIZE];
    char ipv4_addr[16];
	char mac[32];
    struct arp_entry *arp = NULL;
    struct hash_bucket *pbucket = NULL;
    uint32_t cursor = 0;
    uint8_t flag = 0;

    /*参数获取及合法性检查*/
    ipaddr = inet_strtoipv4((char *)argv[0]);
    if(argv[1])
    {
        vpnid = (uint16_t)atoi(argv[1]);
        if (vpnid > VPN_SUPPORT )
        {
            vty_error_out(vty, "This device support vpn range <1-%d>!%s",VPN_SUPPORT,VTY_NEWLINE);
            return CMD_SUCCESS;
        }

        /*arp 查找显示*/
        arp = arp_lookup(ipaddr, vpnid);
        if ( NULL != arp)
        {
        	vty_out (vty, "%-15s ", "IP ADDRESS");
    		vty_out (vty, "%-18s ", "MAC ADDRESS");
    		vty_out (vty, "%-28s ", "INTERFACE");
    		vty_out (vty, "%-4s ",  "VPN");
    		vty_out (vty, "%-8s%s","STATUS",VTY_NEWLINE);

            inet_ipv4tostr(ipaddr,ipv4_addr);
    		ifm_get_name_by_ifindex ( arp->ifindex ,ifname);
        	sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", arp->mac[0], arp->mac[1], arp->mac[2], arp->mac[3], arp->mac[4], arp->mac[5]);

            vty_out(vty, "%-15s %-18s %-28s %-4d %-8s %s",
    	            ipv4_addr, mac,ifname,arp->key.vpnid,arp_status_parse_str(arp->status),VTY_NEWLINE);
       }
    }
    else
    {
       	HASH_BUCKET_LOOP ( pbucket, cursor, arp_table)
     	{
        	if(pbucket->data)
     		{
        		arp = (struct arp_entry *)pbucket->data;
    			if(ipaddr != arp->key.ipaddr)
    				continue;

    			flag += 1;
                inet_ipv4tostr(arp->key.ipaddr,ipv4_addr);
    			ifm_get_name_by_ifindex (arp->ifindex ,ifname);

    			if(flag == 1)
    			{
    				vty_out (vty, "%-15s ", "IP ADDRESS");
    				vty_out (vty, "%-18s ", "MAC ADDRESS");
    				vty_out (vty, "%-28s ", "INTERFACE");
    				vty_out (vty, "%-4s ",  "VPN");
    				vty_out (vty, "%-8s%s","STATUS",VTY_NEWLINE);
    			}

    			sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", arp->mac[0], arp->mac[1], arp->mac[2], arp->mac[3], arp->mac[4], arp->mac[5]);
          	 	vty_out(vty, "%-15s %-18s %-28s %-4d %-8s %s",
            	    ipv4_addr, mac,ifname,arp->key.vpnid,arp_status_parse_str(arp->status),VTY_NEWLINE);
    		}
        }
    }

   return CMD_SUCCESS;
}


/*指定vpn 显示arp*/
DEFUN(show_arp_l3vpn,
	show_arp_l3vpn_cmd,
	"show arp l3vpn <1-1024>",
	"Show arp info\n"
	"Arp\n"
	"L3vpn \n"
	"L3vpn value <1-1024> \n"
	)
{
	int cursor = 0;
	int flag = 0;
	struct hash_bucket *pbucket = NULL;
	uint16_t vpnid = 0;
    struct arp_entry *arp = NULL;
    char ifname[IFNET_NAMESIZE];
    char ipv4_addr[16];
	char mac[32];

    /*参数获取*/
    vpnid = (uint16_t)atoi(argv[0]);
    if (vpnid > VPN_SUPPORT )
    {
        vty_error_out(vty, "This device support vpn range <1-%d>!%s",VPN_SUPPORT,VTY_NEWLINE);
        return CMD_SUCCESS;
    }

    /*轮询hash 表显示arp*/
  	HASH_BUCKET_LOOP ( pbucket, cursor, arp_table)
  	{
   	    if(pbucket->data)
     	{
        	arp = (struct arp_entry *)pbucket->data;

			if(vpnid != arp->key.vpnid)
				continue;

			flag += 1;
            inet_ipv4tostr(arp->key.ipaddr,ipv4_addr);
			ifm_get_name_by_ifindex ( arp->ifindex ,ifname);

			//只打印一次
			if(flag == 1)
			{
				vty_out (vty, "%-15s ", "IP ADDRESS");
				vty_out (vty, "%-18s ", "MAC ADDRESS");
				vty_out (vty, "%-28s ", "INTERFACE");
				vty_out (vty, "%-4s ",  "VPN");
				vty_out (vty, "%-8s%s", "STATUS",VTY_NEWLINE);
			}

    		sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", arp->mac[0], arp->mac[1], arp->mac[2], arp->mac[3], arp->mac[4], arp->mac[5]);
	      	vty_out(vty, "%-15s %-18s %-28s %-4d %-8s %s",
                    ipv4_addr, mac,ifname,arp->key.vpnid,arp_status_parse_str(arp->status),VTY_NEWLINE);
    	}
  	}

    return CMD_SUCCESS;
}


/*显示arp 统计信息*/
DEFUN(show_arp_statistics,
	show_arp_statistics_cmd,
	"show arp statistics",
	"Show running arp information\n"
	"Arp\n"
	"Arp statistics num\n")
{
	int cursor = 0;
	int arp_static_num = 0;
	int arp_complete_num = 0;
	int arp_incomplete_num = 0;
	int arp_aged_num = 0;
	int arp_total_num = 0;
	int arp_dynamic_num = 0;
    int arp_rule_num = 0;
    struct arp_entry *arp = NULL;
    struct hash_bucket *pbucket = NULL;

    /*轮询hash 表，进行arp 统计*/
    HASH_BUCKET_LOOP ( pbucket, cursor, arp_table)
    {
        if(pbucket->data)
        {
         	arp = (struct arp_entry *)pbucket->data;

			switch(arp->status)
			{
				case ARP_STATUS_STATIC:
					arp_static_num++;
					break;
				case ARP_STATUS_COMPLETE:
					arp_complete_num++;
					break;
				case ARP_STATUS_INCOMPLETE:
					arp_incomplete_num++;
					break;
				case ARP_STATUS_AGED:
					arp_aged_num++;
					break;
                case ARP_STATUS_LLDP:
                    arp_rule_num++;
                    break;
				default:
		        {
					vty_out ( vty, "%%Unkown arp status %s", VTY_NEWLINE );
		        	return CMD_WARNING;
		        }
			}

    	}
    }
	arp_dynamic_num = arp_complete_num +arp_incomplete_num +arp_aged_num ;
	arp_total_num = arp_static_num + arp_dynamic_num + arp_rule_num;

    /*显示arp 统计信息*/
	vty_out(vty, "----------arp statistics--------%s", VTY_NEWLINE);
	vty_out (vty, "Total arp number:     %d %s",arp_total_num ,VTY_NEWLINE);
	vty_out (vty, "Static arp number:    %d %s",arp_static_num, VTY_NEWLINE);
	vty_out (vty, "Complete arp  number: %d  %s",arp_complete_num ,VTY_NEWLINE);
	vty_out (vty, "Incomplete arp number:%d  %s",arp_incomplete_num, VTY_NEWLINE);
	vty_out (vty, "Aged arp  number :    %d %s",arp_aged_num, VTY_NEWLINE);
	vty_out (vty, "Rule arp  number :    %d %s",arp_rule_num, VTY_NEWLINE);

    return CMD_SUCCESS;
}


/*显示所有arp*/
DEFUN(show_all_arp,
	show_all_arp_cmd,
	"show arp",
	"Show running arp information\n"
	"Arp\n")
{
	int cursor = 0;
	int static_arp_num = 0;
	int dynamic_arp_num = 0;
    struct arp_entry *arp = NULL;
    struct hash_bucket *pbucket = NULL;
    char ipv4_addr[16];
    char ifname[IFNET_NAMESIZE];
	char mac[32];

    vty_out (vty, "%-15s ", "IP ADDRESS");
	vty_out (vty, "%-18s ", "MAC ADDRESS");
	vty_out (vty, "%-28s ", "INTERFACE");
	vty_out (vty, "%-4s ", "VPN");
	vty_out (vty, "%-8s%s", "STATUS",VTY_NEWLINE);

    HASH_BUCKET_LOOP ( pbucket, cursor, arp_table)
    {
        if(pbucket->data)
        {
        	arp = (struct arp_entry *)pbucket->data;
			if(arp->status == ARP_STATUS_STATIC)
        	{
        		static_arp_num++;
        	}
        	else
        	{
        		dynamic_arp_num++;
        	}
            inet_ipv4tostr(arp->key.ipaddr,ipv4_addr);
			ifm_get_name_by_ifindex ( arp->ifindex ,ifname);

    		sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", arp->mac[0], arp->mac[1], arp->mac[2], arp->mac[3], arp->mac[4], arp->mac[5]);
	        vty_out(vty, "%-15s %-18s %-28s %-4d %-8s%s",
	            ipv4_addr, mac,ifname,arp->key.vpnid,arp_status_parse_str(arp->status),VTY_NEWLINE);
    	}
    }
	vty_out (vty, "TOTAL NUM:%d     DYNAMIC NUM:%d        STATIC NUM:%d %s",static_arp_num+dynamic_arp_num,dynamic_arp_num ,static_arp_num, VTY_NEWLINE);

    return CMD_SUCCESS;
}


/*清除指定接口的动态arp*/
DEFUN (clear_arp_interface,
        clear_arp_interface_cmd,
        "clear arp interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP|vlanif <1-4094>|trunk TRUNK}",
		"Clear\n"
		"Arp\n"
		"Interface\n"
		"Ethernet interface type\n"
		CLI_INTERFACE_ETHERNET_VHELP_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
		"Vlanif interface type\n"
		CLI_INTERFACE_VLANIF_VHELP_STR
		"Trunk interface type"
		CLI_INTERFACE_TRUNK_VHELP_STR
		)
{
	uint32_t ifindex = 0;
	int num = 0;

    /*参数获取及合法性检查*/
	if(NULL != argv[0] && NULL == argv[1] && NULL == argv[2]&& NULL == argv[3] && NULL == argv[4])
    {
       ifindex = ifm_get_ifindex_by_name("ethernet", (char *)argv[0]);
    }
    else if(NULL == argv[0] && NULL != argv[1] && NULL == argv[2]&& NULL == argv[3] && NULL == argv[4])
    {
       ifindex = ifm_get_ifindex_by_name("gigabitethernet", (char *)argv[1]);
    }
    else if(NULL == argv[0] && NULL == argv[1] && NULL != argv[2]&& NULL == argv[3] && NULL == argv[4])
    {
       ifindex = ifm_get_ifindex_by_name("xgigabitethernet", (char *)argv[2]);
    }
	else if(NULL == argv[0] && NULL == argv[1] && NULL == argv[2]&& NULL != argv[3] && NULL == argv[4])
    {
       ifindex = ifm_get_ifindex_by_name("vlanif", (char *)argv[3]);
    }
	else if(NULL == argv[0] && NULL == argv[1] && NULL == argv[2]&& NULL == argv[3] && NULL != argv[4])
    {
       ifindex = ifm_get_ifindex_by_name("trunk", (char *)argv[4]);
    }
	if(0 == ifindex)
	{
		vty_error_out (vty, "Get interface index by name fail,please check out input!%s",VTY_NEWLINE);
		return CMD_WARNING;
	}

    /*删除指定arp*/
	num = arp_delete_interface(ifindex);
	if(num == 0)
	{
		vty_out (vty, "Can not find dynamic arp to clear%s",VTY_NEWLINE);
	}
	else
	{
		vty_out (vty, "%d dynamic arp wait immediate aged  %s", num, VTY_NEWLINE);
	}

    return CMD_SUCCESS;
}



/*清除指定ip的动态arp*/
DEFUN (clear_arp_ip,
        clear_arp_ip_cmd,
        "clear arp ip A.B.C.D",
		"Clear\n"
		"Arp\n"
		"IP address\n"
		"IP format A.B.C.D\n")
{
	uint32_t ipaddr = 0;
	int num = 0;

    ipaddr = inet_strtoipv4((char *)argv[0]);
	num = arp_delete_ip(ipaddr);
	if(num == 0)
	{
		vty_out (vty, "Can not find dynamic arp to clear%s", VTY_NEWLINE);
	}
	else
	{
		vty_out (vty, "%d dynamic arp wait immediate aged  %s", num, VTY_NEWLINE);
	}

    return CMD_SUCCESS;
}


/*清除指定vpn 的动态arp*/
DEFUN (clear_arp_l3vpn,
        clear_arp_l3vpn_cmd,
        "clear arp l3vpn <0-1024>",
		"Clear\n"
		"Arp\n"
		"L3vpn\n"
		"L3vpn value <1-1024>\n")
{
	uint16_t vpnid = 0;
	int num = 0;

	vpnid = (uint16_t)atoi(argv[0]);
    if (vpnid > VPN_SUPPORT )
    {
        vty_error_out(vty, "This device support vpn range <1-%d>!%s",VPN_SUPPORT,VTY_NEWLINE);
        return CMD_SUCCESS;
    }
	
    num = arp_delete_vpn( vpnid);
	if(num == 0)
	{
		vty_out (vty, "Can not find dynamic arp to clear%s", VTY_NEWLINE);
	}
	else
	{
		vty_out (vty, "%d dynamic arp wait immediate aged%s", num, VTY_NEWLINE);
	}

    return CMD_SUCCESS;
}


/*清除全局或接口下动态arp */
DEFUN (clear_arp,
        clear_arp_cmd,
        "clear arp ",
		"Clear\n"
		"Arp\n")
{
	uint32_t ifindex = 0 ;
	int num = 0;

    if(CONFIG_NODE == vty->node)
    {
        num = arp_delete_all();
    }
    else
    {
        ifindex = (uint32_t)vty->index;
	    num = arp_delete_interface(ifindex);
    }

	if(num == 0)
	{
		vty_out (vty, "Can not find dynamic arp to clear%s", VTY_NEWLINE);
	}
	else
	{
		vty_out (vty, "%d dynamic arp wait immediate aged%s", num, VTY_NEWLINE);
	}

    return CMD_SUCCESS;
}

DEFUN (config_arp_miss_anti_sip,
        config_arp_miss_anti_sip_cmd,
        "arp-miss speed-limit source-ip [A.B.C.D] maxinum <0-1500>",
        "Arp miss action\n"
        "Arp-miss speed limit\n"
        "Arp-miss packet source ip\n"
        "Ip format\n"
        "The time allowed\n"
        "The range of arp-miss count\n"
        )
{
    uint32_t sip = 0;
    uint32_t maxnum = 0;
    struct arp_anti_key key;

    memset(&key, 0, sizeof(struct arp_anti_key));
    if (argv[0] && argv[1])
    {
        sip = inet_strtoipv4((char *)argv[0]);
        maxnum = (uint32_t)atoi(argv[1]);

        key.sip_vlan_ifidx = sip;
        key.type = ARP_ANTI_SIP;
        gmiss_anti.sip_flag = 1;

        arp_miss_anti_cnt_add(&key, maxnum);
    }
    
    if (!argv[0] && argv[1])
    {
        maxnum = (uint32_t)atoi(argv[1]);
        gmiss_anti.gsip_limit = maxnum;
        
    }


    return CMD_SUCCESS;
    
}

DEFUN (undo_config_arp_miss_anti_sip,
        undo_config_arp_miss_anti_sip_cmd,
        "no arp-miss speed-limit source-ip A.B.C.D",
        "Undo\n"
        "Arp miss action\n"
        "Arp-miss speed limit\n"
        "Arp-miss packet source ip\n"
        "Ip format\n"
        )
{
    uint32_t sip = 0;
    struct arp_anti_key key;
    
    if (argv[0])
    {
        sip = inet_strtoipv4((char *)argv[0]);

        key.sip_vlan_ifidx = sip;
        key.type = ARP_ANTI_SIP;
        gmiss_anti.sip_flag = 0;
    }

    arp_miss_anti_cnt_del(&key);

   return CMD_SUCCESS; 
}

DEFUN (config_arp_miss_anti_vlan,
        config_arp_miss_anti_vlan_cmd,
        "arp-miss speed-limit vlan <1-4095> maxinum <0-1500>",
        "Arp miss action\n"
        "Arp-miss speed limit\n"
        "Vlan"
        "Vlan value\n"
        "The time allowed\n"
        "The range of arp-miss count\n"
        )
{
    uint32_t vlan = 0;
    uint32_t maxnum = 0;
    struct arp_anti_key key;
    
    if (argv[0] && argv[1])
    {
        vlan = (uint32_t)atoi(argv[0]);
        maxnum = (uint32_t)atoi(argv[1]);

        key.sip_vlan_ifidx = vlan;
        key.type = ARP_ANTI_VLAN;
        gmiss_anti.vlan_flag = 1;

        arp_miss_anti_cnt_add(&key, maxnum);
    }
    
    return CMD_SUCCESS;
}

DEFUN (undo_config_arp_miss_anti_vlan,
        undo_config_arp_miss_anti_vlan_cmd,
        "no arp-miss speed-limit vlan <1-4095>",
        "Undo\n"
        "Arp miss action\n"
        "Arp-miss speed limit\n"
        "Vlan\n"
        "Vlan value\n"
        )
{
    uint32_t vlan = 0;
    struct arp_anti_key key;
    
    if (argv[0])
    {
        vlan = (uint32_t)atoi(argv[0]);

        key.sip_vlan_ifidx = vlan;
        key.type = ARP_ANTI_VLAN;
        gmiss_anti.sip_flag = 0;
    }

    arp_miss_anti_cnt_del(&key);

    return CMD_SUCCESS;
}


DEFUN (config_arp_miss_anti_ifidx,
        config_arp_miss_anti_ifidx_cmd,
        "arp-miss speed-limit interface {ethernet USP |gigabitethernet USP |xgigabitethernet USP} maxinum <0-1500>",
        "Arp miss action\n"
        "Arp-miss speed limit\n"
        INTERFACE_STR
    	CLI_INTERFACE_ETHERNET_STR
    	CLI_INTERFACE_ETHERNET_VHELP_STR
    	CLI_INTERFACE_GIGABIT_ETHERNET_STR
    	CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
    	CLI_INTERFACE_XGIGABIT_ETHERNET_STR
    	CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
        "The time allowed\n"
        "The range of arp-miss count\n"
        )
{
    uint32_t ifindex = 0;
    uint32_t maxnum = 0;
    struct arp_anti_key key;

    if(argv[0] && !argv[1] && !argv[2])
    {
        ifindex = ifm_get_ifindex_by_name("ethernet", (char *)argv[0]);
    }
    else if(!argv[0] && argv[1] && !argv[2])
    {
        ifindex = ifm_get_ifindex_by_name("gigabitethernet", (char *)argv[1]);
    }
    else if(argv[0] && !argv[1] && argv[2])
    {
        ifindex = ifm_get_ifindex_by_name("xgigabitethernet", (char *)argv[2]);
    }
        
    maxnum = (uint32_t)atoi(argv[3]);
    key.sip_vlan_ifidx = ifindex;
    key.type = ARP_ANTI_IFIDX;

    gmiss_anti.ifidx_flag = 1;

    arp_miss_anti_cnt_add(&key, maxnum);

    return CMD_SUCCESS;
    
}


DEFUN  (undo_config_arp_miss_anti_ifidx,
        undo_config_arp_miss_anti_ifidx_cmd,
        "no arp-miss speed-limit interface {ethernet USP |gigabitethernet USP |xgigabitethernet USP}",
        "Undo\n"
        "Arp miss action\n"
        "Arp-miss speed limit\n"
        INTERFACE_STR
    	CLI_INTERFACE_ETHERNET_STR
    	CLI_INTERFACE_ETHERNET_VHELP_STR
    	CLI_INTERFACE_GIGABIT_ETHERNET_STR
    	CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
    	CLI_INTERFACE_XGIGABIT_ETHERNET_STR
    	CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
        )
{
    uint32_t ifindex = 0;
    struct arp_anti_key key;
    
    if(argv[0] && !argv[1] && !argv[2])
    {
        ifindex = ifm_get_ifindex_by_name("ethernet", (char *)argv[0]);
    }
    else if(!argv[0] && argv[1] && !argv[2])
    {
        ifindex = ifm_get_ifindex_by_name("gigabitethernet", (char *)argv[1]);
    }
    else if(argv[0] && !argv[1] && argv[2])
    {
        ifindex = ifm_get_ifindex_by_name("xgigabitethernet", (char *)argv[2]);
    }

    key.sip_vlan_ifidx = ifindex;
    key.type = ARP_ANTI_IFIDX;

    gmiss_anti.ifidx_flag = 0;
    
    arp_miss_anti_cnt_del(&key);
   
   return CMD_SUCCESS; 
}



static char *ndp_status_parse(uint8_t status)
{
    switch(status)
    {
        case 0:
            return (char *)"INVALID";
        case 1:
            return (char *)"INCOMPLETE";
        case 2:
            return (char *)"REACHABLE";
        case 3:
            return (char *)"STALE";
        case 4:
            return (char *)"DELAY";
        case 5:
            return (char *)"PROBE";
        case 6:
            return (char *)"PERMANENT";
        case 7:
            return (char *)"STATIC";
        default:
            return (char *)"UNKNOW";
    }

}

/*显示所有活跃ndp 表项*/
/*ipv6_str check*/
int ndp_ipv6_str_check(const char *src)
{
    int colon = 0;

    if (NULL == src)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    while(*src != '\0')
    {
        if((*src++ == ':') && (++colon > 7))
        {
            return ERRNO_FAIL;
        }
    }

    return ERRNO_SUCCESS;
}

DEFUN(show_ipv6_ndp,
      show_ipv6_ndp_cmd,
      "show ipv6 neighbor",
      SHOW_STR
      IPV6_STR
      "Ipv6 neighbor\n")
{
    struct hash_bucket  *pbucket = NULL;
    struct ndp_neighbor *pndp = NULL;
    uint32_t cursor = 0;
    uint8_t  ipv6_str[IPV6_ADDR_STRLEN];
    uint8_t  ifname[IFNET_NAMESIZE];
    uint8_t  mac[20];
    uint16_t static_num = 0;
    uint16_t dynamic_num = 0;


    HASH_BUCKET_LOOP(pbucket, cursor, ndp_table)
    {
        if(pbucket->data)
        {
            pndp = (struct ndp_neighbor *)(pbucket->data);

            if((static_num == 0) && (dynamic_num == 0))
                vty_out(vty,"-------------------------------------------------%s", VTY_NEWLINE);

            inet_ipv6tostr((struct ipv6_addr *)(pndp->key.ipv6_addr), (char *)ipv6_str, IPV6_ADDR_STRLEN);
            ether_mac_to_string((char *)(pndp->mac), (uchar *)mac);
            ifm_get_name_by_ifindex(pndp->key.ifindex, (char *)ifname);

            vty_out (vty, "%-41s ", "IPV6 ADDRESS");
            vty_out (vty, "%-8s %s",  "VPN", VTY_NEWLINE);
            vty_out (vty, "%-41s %-8d %s", ipv6_str, pndp->key.vpnid, VTY_NEWLINE);

            vty_out (vty, "%-20s ", "MAC ADDRESS");
            vty_out (vty, "%-20s ", "INTERFACE");
            vty_out (vty, "%-12s %s","STATUS",VTY_NEWLINE);
            vty_out(vty, "%-20s %-20s %-12s %s%s", mac, ifname, ndp_status_parse(pndp->status), VTY_NEWLINE, VTY_NEWLINE);

            if(pndp->status == NDP_STATUS_STATIC)
                static_num ++;
            else
                dynamic_num ++;

        }
    }

    if((static_num != 0) || (dynamic_num != 0))
    {
        vty_out (vty,"-------------------------------------------------%s", VTY_NEWLINE);
    	vty_out (vty, "TOTAL NUM:%d     DYNAMIC NUM:%d        STATIC NUM:%d %s",
                  static_num + dynamic_num,dynamic_num, static_num, VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}



/*显示指定的ndp表项*/
DEFUN(show_ipv6_ndp_ip,
    	show_ipv6_ndp_ip_cmd,
    	"show ipv6 neighbor X.X.X.X.X.X.X.X {l3vpn <1-1024>}",
        SHOW_STR
        IPV6_STR
        "Ipv6 neighbor\n"
        "Ipv6 address format X:X:X:X:X:X:X:X(X:X::X:X)\n"
    	"L3vpn \n"
    	"L3vpn value <1-1024> \n"
    	)
{
	int vpnid = -1;
    struct ipv6_addr ip6addr;
    struct ndp_neighbor *pndp = NULL;
    struct hash_bucket  *bucket = NULL;
    char ipv6_str[IPV6_ADDR_STRLEN];
    char ifname[IFNET_NAMESIZE];
	char mac[20];
    int ret = 0;
    int cursor = 0;

    /*参数获取及合法性检查*/
    ret = ndp_ipv6_str_check((char *)argv[0]);
	if (ERRNO_SUCCESS != ret)
	{
        vty_error_out(vty, "The specified IPV6 address is invalid.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
    
    ret = inet_strtoipv6((char *)argv[0], &ip6addr);
    if (ret <= 0)
    {
        vty_error_out(vty, "The specified IPV6 address is invalid.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if(argv[1])
    {
        vpnid = atoi(argv[1]);
    }

    HASH_BUCKET_LOOP(bucket, cursor, ndp_table)
    {
        if (NULL == bucket->data)
            continue;

        pndp = (struct ndp_neighbor *)bucket->data;
        if (vpnid >= 0)
        {
            if (!IPV6_ADDR_SAME(&ip6addr, pndp->key.ipv6_addr) ||
               !((uint32_t)vpnid == pndp->key.vpnid))
            {
                continue;
            }
        }
        else if (vpnid < 0)
        {
            if (!IPV6_ADDR_SAME(&ip6addr, pndp->key.ipv6_addr))
            {
                continue;
            }
        }
        else
        {
            FTM_NDP_ZLOG_DEBUG("This is impossible!ip6addr:%s vpn:%d\n",
                               inet_ipv6tostr(&ip6addr,(char *)ipv6_str, IPV6_ADDR_STRLEN), vpnid);
            continue;
        }
        
        inet_ipv6tostr((struct ipv6_addr *)(ip6addr.ipv6), (char *)ipv6_str, IPV6_ADDR_STRLEN);
        ether_mac_to_string((char *)(pndp->mac), (uchar *)mac);
        ifm_get_name_by_ifindex(pndp->key.ifindex, ifname);

        vty_out (vty, "%-41s ", "IPV6 ADDRESS");
        vty_out (vty, "%-8s %s ",  "VPN", VTY_NEWLINE);
        vty_out (vty, "%-41s %-8d %s", ipv6_str, pndp->key.vpnid, VTY_NEWLINE);

		vty_out (vty, "%-20s ", "MAC ADDRESS");
		vty_out (vty, "%-20s ", "INTERFACE");
		vty_out (vty, "%-12s %s","STATUS",VTY_NEWLINE);
        vty_out(vty, "%-20s %-20s %-12s %s",mac, ifname, ndp_status_parse(pndp->status), VTY_NEWLINE);
    }        

    return CMD_SUCCESS;
}


/*显示指定接口的nd 表项*/
DEFUN(show_ipv6_ndp_interface_ndp,
      show_ipv6_ndp_interface_cmd,
      "show ipv6 neighbor interface {ethernet USP |gigabitethernet USP |xgigabitethernet USP |trunk TRUNK | vlanif <1-4095>}",
      SHOW_STR
      IPV6_STR
      "Ipv6 neighbor\n"
      INTERFACE_STR
      CLI_INTERFACE_ETHERNET_STR
      CLI_INTERFACE_ETHERNET_VHELP_STR
      CLI_INTERFACE_GIGABIT_ETHERNET_STR
      CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
      CLI_INTERFACE_XGIGABIT_ETHERNET_STR
      CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
      CLI_INTERFACE_TRUNK_STR
      CLI_INTERFACE_TRUNK_VHELP_STR
      CLI_INTERFACE_VLANIF_STR
      CLI_INTERFACE_VLANIF_VHELP_STR
    )
{
    uint32_t ifindex = 0;
    char ifname[IFNET_NAMESIZE];
    uint8_t ipv6_str[IPV6_ADDR_STRLEN];
    char mac[20];
    struct ndp_neighbor *pndp = NULL;
    struct hash_bucket *pbucket = NULL;
    uint32_t cursor = 0;

    if(NULL != argv[0] && NULL == argv[1] && NULL == argv[2]&& NULL == argv[3] && NULL == argv[4])
    {
       ifindex = ifm_get_ifindex_by_name("ethernet", (char *)argv[0]);
    }
    else if(NULL == argv[0] && NULL != argv[1] && NULL == argv[2]&& NULL == argv[3] && NULL == argv[4])
    {
       ifindex = ifm_get_ifindex_by_name("gigabitethernet", (char *)argv[1]);
    }
    else if(NULL == argv[0] && NULL == argv[1] && NULL != argv[2]&& NULL == argv[3] && NULL == argv[4])
    {
       ifindex = ifm_get_ifindex_by_name("xgigabitethernet", (char *)argv[2]);
    }
	else if(NULL == argv[0] && NULL == argv[1] && NULL == argv[2]&& NULL != argv[3] && NULL == argv[4])
    {
       ifindex = ifm_get_ifindex_by_name("trunk", (char *)argv[3]);
    }
	else if(NULL == argv[0] && NULL == argv[1] && NULL == argv[2]&& NULL == argv[3] && NULL != argv[4])
    {
       ifindex = ifm_get_ifindex_by_name("vlanif", (char *)argv[4]);
    }

    if(ifindex == 0)
    {
        vty_error_out(vty,"Get ifindex fail!%s",VTY_NEWLINE);
        return CMD_ERR_INCOMPLETE;
    }

    HASH_BUCKET_LOOP ( pbucket, cursor, ndp_table)
    {
        if(pbucket->data)
        {
            pndp = (struct ndp_neighbor *)(pbucket->data);
            if(pndp->key.ifindex != ifindex)
                continue;

            inet_ipv6tostr((struct ipv6_addr *)(pndp->key.ipv6_addr), (char *)ipv6_str, IPV6_ADDR_STRLEN);
            ether_mac_to_string((char *)(pndp->mac), (uchar *)mac);
            ifm_get_name_by_ifindex(pndp->key.ifindex, (char *)ifname);

            vty_out (vty, "%-41s ", "IPV6 ADDRESS");
            vty_out (vty, "%-8s %s", "VPN", VTY_NEWLINE);
            vty_out (vty, "%-41s %-8d %s", ipv6_str, pndp->key.vpnid, VTY_NEWLINE);

    		vty_out (vty, "%-20s ", "MAC ADDRESS");
    		vty_out (vty, "%-20s ", "INTERFACE");
    		vty_out (vty, "%-12s %s","STATUS",VTY_NEWLINE);
            vty_out (vty, "%-20s %-20s %-12s %s%s", mac, ifname,
                     ndp_status_parse(pndp->status), VTY_NEWLINE, VTY_NEWLINE);

        }
    }

    return CMD_SUCCESS;
}

/*display ipv6 neighbor entry which l3vpn is specified*/
DEFUN(show_ipv6_ndp_l3vpn,
    	show_ipv6_ndp_l3vpn_cmd,
    	"show ipv6 neighbor l3vpn <1-128>",
        SHOW_STR
        IPV6_STR
        "Ipv6 neighbor\n"
    	"L3vpn \n"
    	"L3vpn value <1-128> \n"
    	)
{
    struct ndp_neighbor *pndp = NULL;
    struct hash_bucket *pbucket = NULL;
    char ipv6_str[IPV6_ADDR_STRLEN];
    char ifname[IFNET_NAMESIZE];
	char mac[20];
    uint32_t cursor = 0;
	uint16_t vpnid = 0;
    uint32_t flag_top = 0;
    uint32_t flag_bottom = 0;

    if(argv[0])
    {
        vpnid = (uint16_t)atoi(argv[0]);
    }

    HASH_BUCKET_LOOP(pbucket, cursor, ndp_table)
    {
        if (pbucket->data)
        {
            pndp = (struct ndp_neighbor *)pbucket->data;
            if (pndp->key.vpnid != vpnid)
                continue;

            if (flag_top == 0)
            {
                vty_out(vty,"-------------------------------------------------%s", VTY_NEWLINE);
                flag_top = 1;
                flag_bottom = 1;
            }
            
            inet_ipv6tostr((struct ipv6_addr *)(pndp->key.ipv6_addr), (char *)ipv6_str, IPV6_ADDR_STRLEN);
            ether_mac_to_string((char *)(pndp->mac), (uchar *)mac);
            ifm_get_name_by_ifindex(pndp->key.ifindex, (char *)ifname);

            vty_out (vty, "%-41s ", "IPV6 ADDRESS");
            vty_out (vty, "%-8s %s",  "VPN", VTY_NEWLINE);
            vty_out (vty, "%-41s %-8d %s", ipv6_str, pndp->key.vpnid, VTY_NEWLINE);

            vty_out (vty, "%-20s ", "MAC ADDRESS");
            vty_out (vty, "%-20s ", "INTERFACE");
            vty_out (vty, "%-12s %s","STATUS",VTY_NEWLINE);
            vty_out(vty, "%-20s %-20s %-12s %s%s", mac, ifname, ndp_status_parse(pndp->status), VTY_NEWLINE, VTY_NEWLINE);

        }
    }
    
    if(flag_bottom == 1)
        vty_out(vty,"-------------------------------------------------%s", VTY_NEWLINE);

   return CMD_SUCCESS;
}


DEFUN(clear_ipv6_ndp_interface,
        clear_ipv6_ndp_interface_cmd,
		"clear ipv6 neighbor interface {ethernet USP |gigabitethernet USP |xgigabitethernet USP | trunk TRUNK | vlanif <1-4095>}",
		CLEAR_STR
		IPV6_STR
		"Ipv6 neighbor\n"
		INTERFACE_STR
		CLI_INTERFACE_ETHERNET_STR
		CLI_INTERFACE_ETHERNET_VHELP_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
		CLI_INTERFACE_TRUNK_STR
		CLI_INTERFACE_TRUNK_VHELP_STR
		CLI_INTERFACE_VLANIF_STR
         CLI_INTERFACE_VLANIF_VHELP_STR
        )
{
    uint32_t ifindex = 0;
    uint32_t num = 0;

   if(NULL != argv[0] && NULL == argv[1] && NULL == argv[2]&& NULL == argv[3] && NULL == argv[4])
    {
       ifindex = ifm_get_ifindex_by_name("ethernet", (char *)argv[0]);
    }
    else if(NULL == argv[0] && NULL != argv[1] && NULL == argv[2]&& NULL == argv[3] && NULL == argv[4])
    {
       ifindex = ifm_get_ifindex_by_name("gigabitethernet", (char *)argv[1]);
    }
    else if(NULL == argv[0] && NULL == argv[1] && NULL != argv[2]&& NULL == argv[3] && NULL == argv[4])
    {
       ifindex = ifm_get_ifindex_by_name("xgigabitethernet", (char *)argv[2]);
    }
	else if(NULL == argv[0] && NULL == argv[1] && NULL == argv[2]&& NULL != argv[3] && NULL == argv[4])
    {
       ifindex = ifm_get_ifindex_by_name("trunk", (char *)argv[3]);
    }
	else if(NULL == argv[0] && NULL == argv[1] && NULL == argv[2]&& NULL == argv[3] && NULL != argv[4])
    {
       ifindex = ifm_get_ifindex_by_name("vlanif", (char *)argv[4]);
    }

    if(ifindex != 0)
    {
        num = ndp_delete_interface(ifindex);
    }

    vty_out(vty, "%d dynamic ndp wait aged!%s", num, VTY_NEWLINE);

    return CMD_SUCCESS;

}


DEFUN(clear_ipv6_ndp_all,
         clear_ipv6_ndp_all_cmd,
         "clear ipv6 neighbor [l3vpn <1-1024>]",
         CLEAR_STR
         IPV6_STR
         "Ipv6 neighbor\n"
    	 "L3vpn \n"
    	 "L3vpn value <1-1024> \n"
        )
{
    uint16_t vpn = 0;
    uint32_t num = 0;

    if(argv[0])
    {
        vpn = (uint16_t)atoi((char *)argv[0]);

        num = ndp_delete_vpn(vpn);
    }
    else
    {
        num = ndp_delete_all();
    }

    vty_out(vty, "%d dynamic ndp wait aged!%s", num, VTY_NEWLINE);

    return CMD_SUCCESS;
}


DEFUN(show_ipv6_ndp_statistics,
      show_ipv6_ndp_statistics_cmd,
      "show ipv6 neighbor statistics",
      SHOW_STR
      IPV6_STR
      "Ipv6 neighbor\n"
      "Ipv6 neighbor statistics\n")
{
    struct hash_bucket *pbucket = NULL;
    struct ndp_neighbor *pndp = NULL;
    uint32_t cursor = 0;
    uint32_t total_nd = 0;
    uint16_t static_nd = 0;
    uint16_t incm_nd = 0;
    uint16_t reach_nd = 0;
    uint16_t stale_nd = 0;
    uint16_t delay_nd = 0;
    uint16_t probe_nd = 0;

    HASH_BUCKET_LOOP(pbucket, cursor, ndp_table)
    {
        if (!(pbucket->data))
            continue;

        pndp = (struct ndp_neighbor *)pbucket->data;
        switch (pndp->status)
        {
            case NDP_STATUS_INCOMPLETE:
                incm_nd ++;
                break;
            case NDP_STATUS_REACHABLE:
                reach_nd ++;
                break;
            case NDP_STATUS_STALE:
                stale_nd ++;
                break;
            case NDP_STATUS_DELAY:
                delay_nd ++;
                break;
            case NDP_STATUS_PROBE:
                probe_nd ++;
                break;
            case NDP_STATUS_STATIC:
                static_nd ++;
                break;
            default:
                FTM_NDP_ZLOG_ERR("Unknown type\n");
                break;
        }
    }

    total_nd = incm_nd + reach_nd + stale_nd + delay_nd + probe_nd + static_nd;
    
    /*dispay ipv6 neighbor statistics*/
    vty_out(vty,"------------------Ipv6 neighbor statistics--------------------%s",VTY_NEWLINE);
    vty_out(vty,"Total ipv6 neighbor number:      %d%s",total_nd,VTY_NEWLINE);
    vty_out(vty,"Incomplete ipv6 neighbor number: %d%s",incm_nd,VTY_NEWLINE);
    vty_out(vty,"Reachable ipv6 neighbor number:  %d%s",reach_nd,VTY_NEWLINE);
    vty_out(vty,"Stale ipv6 neighbor number:      %d%s",stale_nd,VTY_NEWLINE);
    vty_out(vty,"Delay ipv6 neighbor number:      %d%s",delay_nd,VTY_NEWLINE);
    vty_out(vty,"Probe ipv6 neighbor number:      %d%s",probe_nd,VTY_NEWLINE);
    vty_out(vty,"Static ipv6 neighbor number:     %d%s",static_nd,VTY_NEWLINE);
    vty_out(vty,"---------------------------------------------------------------%s",VTY_NEWLINE);
    
    return CMD_SUCCESS;

}




static char* pkt_inif_type_parse_str(uint8_t type)
{
  switch(type)
  {
  	case 0:
		return (char *)"PORT";
	case 1:
		return (char *)"PW";
	case 2:
		return (char *)"LSP";
	case 3:
		return (char *)"TUNNEL";
	default:
		return (char *)"UNKNOW";
  }
  return (char *)"UNKNOW";
}


/*显示ftm protocol*/
DEFUN(show_ftm_protocol,
	show_ftm_protocol_cmd,
	"show ftm protocol (ip|eth|mpls)",
	SHOW_STR
	"Ftm \n"
	"Protocol\n"
	"Ip protocol info\n"
	"Eth protocol info\n"
	"Mpls protocol info\n")
{
    struct hash_bucket *pbucket = NULL;
	struct ip_proto *ip_pro = NULL;
	struct eth_proto *eth_pro = NULL;
	struct mpls_proto *mpls_pro = NULL;
	struct prefix_ipv4 dip;
    struct prefix_ipv4 sip;
	char dip_addr[16];
	char sip_addr[16];
	char dmac[32];
	char smac[32];
	int cursor = 0;
	int module_id = 0;
	int flag = 0;

	if(!strncmp(argv[0], "ip", 1))
	{
		HASH_BUCKET_LOOP ( pbucket, cursor, ip_proto_hash)
		{
			if(pbucket->data)
			{
				module_id 	= (int)pbucket->data;
				ip_pro 		= (struct ip_proto *)pbucket->hash_key;
				flag ++;

				if(flag == 1)
				{
					vty_out (vty, "%-10s ", "MODULE ID");
					vty_out (vty, "%-8s ", "ACL_INDEX");
					vty_out (vty, "%-8s ", "PROTOCOL");
					vty_out (vty, "%-6s ", "SPORT");
					vty_out (vty, "%-6s ",  "DPORT");
					vty_out (vty, "%-16s ",  "DIP");
					vty_out (vty, "%-16s%s","SIP",VTY_NEWLINE);
					vty_out(vty,  "-----------------------------------------------------------------%s",VTY_NEWLINE);
				}

				vty_out(vty, "%-10d ",module_id);

				if(ip_pro->acl_index == 0)
					vty_out(vty, "%-8s ","--");
				else
					vty_out(vty, "%-8d ",ip_pro->acl_index);

				if(ip_pro->protocol == 0)
					vty_out(vty, "%-8s ","--");
				else
					vty_out(vty, "%-8d ",ip_pro->protocol);

				if(ip_pro->sport == 0)
					vty_out(vty, "%-6s ","--");
				else
					vty_out(vty, "%-6d ",ip_pro->sport);

				if(ip_pro->dport == 0)
					vty_out(vty, "%-6s ","--");
				else
					vty_out(vty, "%-6d ",ip_pro->dport);

				if(ip_pro->dip == 0)
				{
					vty_out(vty, "%-16s ","--");
				}
				else
				{
					dip.prefix.s_addr = htonl(ip_pro->dip);
					inet_ntop ( AF_INET, ( void * ) &dip.prefix, dip_addr, 100 );
					vty_out(vty,"%-16s ",dip_addr);
				}

				if(ip_pro->sip == 0)
				{
					vty_out(vty, "%-16s ","--");
				}
				else
				{
					sip.prefix.s_addr = htonl(ip_pro->sip);
					inet_ntop ( AF_INET, ( void * ) &sip.prefix, sip_addr, 100 );
					vty_out(vty,"%-16s ",sip_addr);
				}

				vty_out(vty,  "%s",VTY_NEWLINE);
			}
		}
	}
	else if(!strncmp(argv[0], "eth", 1))
	{

		HASH_BUCKET_LOOP ( pbucket, cursor, eth_proto_hash)
		{
			if(pbucket->data)
			{
				module_id 	= (int)pbucket->data;
				eth_pro 		= (struct eth_proto *)pbucket->hash_key;
				flag ++;

				if(flag == 1)
				{
					vty_out (vty, "%-9s ", "MODULE ID");
					vty_out (vty, "%-8s ", "ACL_INDEX");
					vty_out (vty, "%-8s ", "ETHTYPE");
					vty_out (vty, "%-11s ", "SUB_ETHTYPE");
					vty_out (vty, "%-17s ",  "DMAC");
					vty_out (vty, "%-17s ", "SMAC");
					vty_out (vty, "%-4s ", "VLAN");
					vty_out (vty, "%-6s%s","OPCODE",VTY_NEWLINE);
					vty_out(vty,  "------------------------------------------------------------%s",VTY_NEWLINE);
				}

				vty_out(vty, "%-9d ",module_id);

				if(eth_pro->acl_index == 0)
					vty_out(vty, "%-8s ","--");
				else
					vty_out(vty, "%-8d ",eth_pro->acl_index);

				if(eth_pro->ethtype == 0)
					vty_out(vty, "%-8s ","--");
				else
					vty_out(vty, "0x%-8x ",eth_pro->ethtype);

				if(eth_pro->sub_ethtype == 0)
					vty_out(vty, "%-11s ","--");
				else
					vty_out(vty, "0x%-11x ",eth_pro->sub_ethtype);

				if(eth_pro->dmac_valid == 0)
					vty_out(vty, "%-17s ","--");
				else
				{
					sprintf(dmac, "%02X:%02X:%02X:%02X:%02X:%02X", eth_pro->dmac[0], eth_pro->dmac[1], eth_pro->dmac[2], eth_pro->dmac[3], eth_pro->dmac[4], eth_pro->dmac[5]);
					vty_out(vty, "%-17s ",dmac);
				}

				if(eth_pro->smac_valid == 0)
					vty_out(vty, "%-17s ","--");
				else
				{
					sprintf(smac, "%02X:%02X:%02X:%02X:%02X:%02X", eth_pro->smac[0], eth_pro->smac[1], eth_pro->smac[2], eth_pro->smac[3], eth_pro->smac[4], eth_pro->smac[5]);
					vty_out(vty, "%-17s ",smac);
				}

				if(eth_pro->vlan == 0)
					vty_out(vty, "%-4s ","--");
				else
					vty_out(vty, "%-4d ",eth_pro->vlan);

				if(eth_pro->oam_opcode == 0)
					vty_out(vty, "%-6s ","--");
				else
					vty_out(vty, "%-6d ",eth_pro->oam_opcode);

				vty_out(vty,  "%s",VTY_NEWLINE);
			}
		}
	}
	else if(!strncmp(argv[0], "mpls", 1))
	{
		HASH_BUCKET_LOOP ( pbucket, cursor, mpls_proto_hash)
		{
			if(pbucket->data)
			{
				module_id 	= (int)pbucket->data;
				mpls_pro 	= (struct mpls_proto *)pbucket->hash_key;
				flag ++;

				if(flag == 1)
				{
					vty_out (vty, "%-9s ", "MODULE ID");
					vty_out (vty, "%-8s ", "ACL_INDEX");
					vty_out (vty, "%-7s ", "IF_TYPE");
					vty_out (vty, "%-12s ", "CONTROL_TYPE");
					vty_out (vty, "%-6s ",  "DPORT");
					vty_out (vty, "%-8s ", "PROTOCOL");
					vty_out (vty, "%-6s ","OPCODE");
					vty_out (vty, "%-11s ","INLABLE_TTL");
					vty_out (vty, "%-16s%s",  "DIP",VTY_NEWLINE);
					vty_out(vty,  "------------------------------------------------------------%s",VTY_NEWLINE);
				}

				vty_out(vty, "%-9d ",module_id);

				if(mpls_pro->acl_index == 0)
					vty_out(vty, "%-8s ","--");
				else
					vty_out(vty, "%-8d ",mpls_pro->acl_index);

				vty_out(vty, "%-7s ",pkt_inif_type_parse_str(mpls_pro->if_type));

				if(mpls_pro->chtype == 0)
					vty_out(vty, "%-12s ","--");
				else
					vty_out(vty, "0x%-12x ",mpls_pro->chtype);

				if(mpls_pro->dport == 0)
					vty_out(vty, "%-6s ","--");
				else
					vty_out(vty, "%-6d ",mpls_pro->dport);

				if(mpls_pro->protocol == 0)
					vty_out(vty, "%-8s ","--");
				else
					vty_out(vty, "%-8d ",mpls_pro->protocol);

				if(mpls_pro->oam_opcode == 0)
					vty_out(vty, "%-6s ","--");
				else
					vty_out(vty, "%-6d ",mpls_pro->oam_opcode);

				if(mpls_pro->ttl == 0)
					vty_out(vty, "%-11s ","--");
				else
					vty_out(vty, "%-11d ",mpls_pro->ttl);

				if(mpls_pro->dip == 0)
				{
					vty_out(vty, "%-16s ","--");
				}
				else
				{
					dip.prefix.s_addr = htonl(mpls_pro->dip);
					inet_ntop ( AF_INET, ( void * ) &dip.prefix, dip_addr, 100 );
					vty_out(vty,"%-16s ",dip_addr);
				}

				vty_out(vty,  "%s",VTY_NEWLINE);
			}
		}
	}

    return CMD_SUCCESS;
}



/*Show ftm packet statistics*/
DEFUN (show_ftm_packet_stat,
       show_ftm_packet_stat_cmd,
       "show ftm packet statistics",
	   "show\n"
	   "Ftm\n"
	   "packet\n"
	   "statistics\n")
{
	vty_out(vty, "ftm packet statistics:%s", VTY_NEWLINE);
	vty_out(vty, "Eth:%s", VTY_NEWLINE);
	vty_out(vty, "%lld packets received%s", ftm_pkt_cnt.eth_recv_pkts,VTY_NEWLINE);
	vty_out(vty, "%lld packets forwarded%s", ftm_pkt_cnt.eth_send_pkts,VTY_NEWLINE);
	vty_out(vty, "%lld packets droped%s", ftm_pkt_cnt.eth_drop_pkts,VTY_NEWLINE);
	vty_out(vty, "Ip:%s", VTY_NEWLINE);
	vty_out(vty, "%lld packets received%s", ftm_pkt_cnt.ip_recv_pkts,VTY_NEWLINE);
	vty_out(vty, "%lld packets forwarded%s", ftm_pkt_cnt.ip_send_pkts,VTY_NEWLINE);
	vty_out(vty, "%lld packets droped%s", ftm_pkt_cnt.ip_drop_pkts,VTY_NEWLINE);
	vty_out(vty, "Udp:%s", VTY_NEWLINE);
	vty_out(vty, "%lld packets received%s", ftm_pkt_cnt.udp_recv_pkts,VTY_NEWLINE);
	vty_out(vty, "%lld packets forwarded%s", ftm_pkt_cnt.udp_send_pkts,VTY_NEWLINE);
	vty_out(vty, "%lld packets droped%s", ftm_pkt_cnt.udp_drop_pkts,VTY_NEWLINE);
	vty_out(vty, "Tcp:%s", VTY_NEWLINE);
	vty_out(vty, "%lld packets received%s", ftm_pkt_cnt.tcp_recv_pkts,VTY_NEWLINE);
	vty_out(vty, "%lld packets forwarded%s", ftm_pkt_cnt.tcp_send_pkts,VTY_NEWLINE);
	vty_out(vty, "%lld packets droped%s", ftm_pkt_cnt.tcp_drop_pkts,VTY_NEWLINE);
	vty_out(vty, "Mpls:%s", VTY_NEWLINE);
	vty_out(vty, "%lld packets received%s", ftm_pkt_cnt.mpls_recv_pkts,VTY_NEWLINE);
	vty_out(vty, "%lld packets forwarded%s", ftm_pkt_cnt.mpls_send_pkts,VTY_NEWLINE);
	vty_out(vty, "%lld packets droped%s", ftm_pkt_cnt.mpls_drop_pkts,VTY_NEWLINE);
	return CMD_SUCCESS;
}

/*clear ftm packet statistics*/
DEFUN (clear_ftm_packet_stat,
       clear_ftm_packet_stat_cmd,
       "clear ftm packet statistics",
	   "clear\n"
	   "Ftm\n"
	   "packet\n"
	   "statistics\n")
{
	memset(&ftm_pkt_cnt, 0, sizeof(struct pkt_count));

	return CMD_SUCCESS;
}


DEFUN (debug_ftm_packet,
       debug_ftm_packet_cmd,
       "debug ftm packet (receive|send) (arp|ip|eth|udp|tcp|mpls|all)",
	   "Debug\n"
	   "Ftm\n"
	   "packet\n"
	   "receive\n"
	   "send\n"
	   "arp\n"
	   "ip\n"
	   "eth\n"
	   "udp\n"
	   "tcp\n"
	   "mpls\n"
	   "all\n")
{
	if (strncmp (argv[0], "r", 1) == 0)
	{
		if (strncmp (argv[1], "arp", 3) == 0)
			ftm_pkt_dbg.arp_recv_dbg = 1;
		else if(strncmp (argv[1], "ip", 2) == 0)
			ftm_pkt_dbg.ip_recv_dbg = 1;
	 	else if (strncmp (argv[1], "eth", 3) == 0)
			ftm_pkt_dbg.eth_recv_dbg = 1;
	 	else if (strncmp (argv[1], "udp", 3) == 0)
			ftm_pkt_dbg.udp_recv_dbg = 1;
	 	else if (strncmp (argv[1], "tcp", 3) == 0)
			ftm_pkt_dbg.tcp_recv_dbg = 1;
		else if (strncmp (argv[1], "mpls", 3) == 0)
			ftm_pkt_dbg.mpls_recv_dbg = 1;
	 	else if (strncmp (argv[1], "all", 3) == 0)
	 	{
			ftm_pkt_dbg.arp_recv_dbg = 1;
			ftm_pkt_dbg.ip_recv_dbg = 1;
			ftm_pkt_dbg.eth_recv_dbg = 1;
			ftm_pkt_dbg.udp_recv_dbg = 1;
			ftm_pkt_dbg.tcp_recv_dbg = 1;
			ftm_pkt_dbg.mpls_recv_dbg = 1;
	 	}
		else
		{
			vty_out(vty, "Fail to debug on ftm packet!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
 	else if (strncmp (argv[0], "s", 1) == 0)
 	{
 		if (strncmp (argv[1], "arp", 3) == 0)
			ftm_pkt_dbg.arp_send_dbg = 1;
		else if(strncmp (argv[1], "ip", 2) == 0)
			ftm_pkt_dbg.ip_send_dbg = 1;
	 	else if (strncmp (argv[1], "eth", 3) == 0)
			ftm_pkt_dbg.eth_send_dbg = 1;
	 	else if (strncmp (argv[1], "udp", 3) == 0)
			ftm_pkt_dbg.udp_send_dbg = 1;
	 	else if (strncmp (argv[1], "tcp", 3) == 0)
			ftm_pkt_dbg.tcp_send_dbg = 1;
		else if (strncmp (argv[1], "mpls", 3) == 0)
			ftm_pkt_dbg.mpls_send_dbg = 1;
	 	else if (strncmp (argv[1], "all", 3) == 0)
	 	{
			ftm_pkt_dbg.arp_send_dbg = 1;
			ftm_pkt_dbg.tcp_send_dbg = 1;
			ftm_pkt_dbg.udp_send_dbg = 1;
			ftm_pkt_dbg.eth_send_dbg = 1;
			ftm_pkt_dbg.ip_send_dbg = 1;
			ftm_pkt_dbg.mpls_send_dbg = 1;
	 	}
		else
		{
			vty_out(vty, "Fail to debug on ftm packet!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
 	}
	else
	{
		vty_out(vty, "Fail to debug on ftm packet!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	vty_out(vty, "Success to debug ftm packet!%s", VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN (no_debug_ftm_packet,
       no_debug_ftm_packet_cmd,
       "no debug ftm packet (receive|send) (arp|ip|eth|udp|tcp|mpls|all)",
       "Delete command\n"
	   "Debug\n"
	   "Ftm\n"
	   "packet\n"
	   "receive\n"
	   "send\n"
	   "arp\n"
	   "ip\n"
	   "eth\n"
	   "udp\n"
	   "tcp\n"
	   "mpls\n"
	   "all\n")
{
	if (strncmp (argv[0], "r", 1) == 0)
	{
		if (strncmp (argv[1], "arp", 3) == 0)
			ftm_pkt_dbg.arp_recv_dbg = 0;
		else if(strncmp (argv[1], "ip", 2) == 0)
			ftm_pkt_dbg.ip_recv_dbg = 0;
	 	else if (strncmp (argv[1], "eth", 3) == 0)
			ftm_pkt_dbg.eth_recv_dbg = 0;
	 	else if (strncmp (argv[1], "udp", 3) == 0)
			ftm_pkt_dbg.udp_recv_dbg = 0;
	 	else if (strncmp (argv[1], "tcp", 3) == 0)
			ftm_pkt_dbg.tcp_recv_dbg = 0;
		else if (strncmp (argv[1], "mpls", 3) == 0)
			ftm_pkt_dbg.mpls_recv_dbg = 0;
	 	else if (strncmp (argv[1], "all", 3) == 0)
	 	{
			ftm_pkt_dbg.arp_recv_dbg = 0;
			ftm_pkt_dbg.ip_recv_dbg  = 0;
			ftm_pkt_dbg.eth_recv_dbg = 0;
			ftm_pkt_dbg.udp_recv_dbg = 0;
			ftm_pkt_dbg.tcp_recv_dbg = 0;
			ftm_pkt_dbg.mpls_recv_dbg = 0;
		}
		else
		{
			vty_out(vty, "Fail to debug on ftm packet!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
 	else if (strncmp (argv[0], "s", 1) == 0)
 	{
 		if (strncmp (argv[1], "arp", 3) == 0)
			ftm_pkt_dbg.arp_send_dbg = 0;
		else if(strncmp (argv[1], "ip", 2) == 0)
			ftm_pkt_dbg.ip_send_dbg = 0;
	 	else if (strncmp (argv[1], "eth", 3) == 0)
			ftm_pkt_dbg.eth_send_dbg = 0;
	 	else if (strncmp (argv[1], "udp", 3) == 0)
			ftm_pkt_dbg.udp_send_dbg = 0;
	 	else if (strncmp (argv[1], "tcp", 3) == 0)
			ftm_pkt_dbg.tcp_send_dbg = 0;
		else if (strncmp (argv[1], "mpls", 3) == 0)
			ftm_pkt_dbg.mpls_send_dbg = 0;
	 	else if (strncmp (argv[1], "all", 3) == 0)
	 	{
			ftm_pkt_dbg.arp_send_dbg = 0;
			ftm_pkt_dbg.tcp_send_dbg = 0;
			ftm_pkt_dbg.udp_send_dbg = 0;
			ftm_pkt_dbg.eth_send_dbg = 0;
			ftm_pkt_dbg.ip_send_dbg = 0;
			ftm_pkt_dbg.mpls_send_dbg = 0;
	 	}
		else
		{
			vty_out(vty, "Fail to debug on ftm packet!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
 	}
	else
	{
		vty_out(vty, "Fail to debug on ftm packet!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	vty_out(vty, "Success to debug ftm packet!%s", VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN (show_tcp_session,
       show_tcp_session_cmd,
       "show tcp session {ipv6}",
	   "show\n"
	   "tcp\n"
	   "session\n")
{
	struct hash_bucket *node = NULL;
	struct tcp_session *psess = NULL;
	struct prefix_ipv4 dip;
    struct prefix_ipv4 sip;
	char dip_addr[16];
	char sip_addr[16];
	char dipv6[INET6_ADDRSTRLEN] = "";
	char sipv6[INET6_ADDRSTRLEN] = "";
	int cursor = 0;
	int flag = 0;

	const char *sess_status[] = {
		"LISTENING",
		"SYN_SENT",
		"SYN_RECEIVED",
		"ESTABLISHED",
		"FIN_WAIT_1",
		"FIN_WAIT_2",
		"CLOSE_WAIT",
		"CLOSING",
		"LAST_ACK",
		"TIME_WAIT",
		"CLOSED",
		"UNKNOWN",
	};

	vty_out (vty, "%-16s:",  "Local Add");
	vty_out (vty, "%-8s ", "PORT");
	vty_out (vty, "%-16s:",  "Foreign Add");
	vty_out (vty, "%-8s ",  "PORT");
	vty_out (vty, "%-11s","VPN");
	vty_out (vty, "%-11s%s","SESS_STATUS",VTY_NEWLINE);
	vty_out(vty,  "-----------------------------------------------------------------%s",VTY_NEWLINE);

	HASH_BUCKET_LOOP(node, cursor, tcp_session_table)
	{
		if(node->data)
		{
			psess = (struct tcp_session *)(node->data);

			if(psess->v6session == TRUE)
			{
				if (NULL == argv[0])
		        {
		            continue;
		        }
				inet_ntop(AF_INET6, psess->sipv6.ipv6, sipv6, INET6_ADDRSTRLEN);

				inet_ntop(AF_INET6, psess->dipv6.ipv6, dipv6, INET6_ADDRSTRLEN);

				vty_out(vty,"%-16s   :%-8d  %-16s   :%-8d ",dipv6,psess->dport,sipv6,psess->sport);
				vty_out(vty, "%-11s%s", sess_status[psess->status], VTY_NEWLINE);
				flag ++;
			}
			else
			{
				if (NULL != argv[0])
		        {
		            continue;
		        }
				sip.prefix.s_addr = htonl(psess->sip);
				inet_ntop ( AF_INET, ( void * ) &sip.prefix, sip_addr, 100 );

				dip.prefix.s_addr = htonl(psess->dip);
				inet_ntop ( AF_INET, ( void * ) &dip.prefix, dip_addr, 100 );

				vty_out(vty,"%-16s:%-8d %-16s:%-8d ",dip_addr,psess->dport,sip_addr,psess->sport);
				vty_out(vty, "%-11d", psess->vpn);

				vty_out(vty, "%-11s%s", sess_status[psess->status], VTY_NEWLINE);
				flag ++;
			}

		}
	}

	vty_out(vty,  "%s-----------------------------------------------------------------%s",VTY_NEWLINE,VTY_NEWLINE);
	vty_out (vty, "%s tcp session total num is: %d  %s",(argv[0]==NULL?"IPv4":"Ipv6"),flag,VTY_NEWLINE);
	return CMD_SUCCESS;
}

DEFUN (show_udp_session,
	  show_udp_session_cmd,
	  "show udp session {ipv6}",
	  "show\n"
	  "udp\n"
	  "session\n")
{
   struct hash_bucket *node = NULL;
   struct udp_session *psess = NULL;
   struct prefix_ipv4 dip;
   struct prefix_ipv4 sip;
   char dip_addr[16];
   char sip_addr[16];
   char dipv6[INET6_ADDRSTRLEN] = "";
   char sipv6[INET6_ADDRSTRLEN] = "";
   int cursor = 0;
   int flag = 0;

 
   vty_out (vty, "%-16s:",	"Local Add");
   vty_out (vty, "%-8s ", "PORT");
   vty_out (vty, "%-16s:",	"Foreign Add");
   vty_out (vty, "%-8s ",  "PORT");
   vty_out (vty, "%-11s%s","VPN",VTY_NEWLINE);
   vty_out(vty,  "-----------------------------------------------------------------%s",VTY_NEWLINE);

   HASH_BUCKET_LOOP(node, cursor, udp_session_table)
   {
	   if(node->data)
	   {
		   psess = (struct udp_session *)(node->data);

		   if(psess->v6session == TRUE)
		   {
			   if (NULL == argv[0])
			   {
				   continue;
			   }
			   inet_ntop(AF_INET6, psess->sipv6.ipv6, sipv6, INET6_ADDRSTRLEN);

			   inet_ntop(AF_INET6, psess->dipv6.ipv6, dipv6, INET6_ADDRSTRLEN);

			   vty_out(vty,"%-16s	:%-8d  %-16s   :%-8d ",dipv6,psess->dport,sipv6,psess->sport);
			   vty_out(vty, "%-11d%s", psess->vpn, VTY_NEWLINE);
			   flag ++;
		   }
		   else
		   {
			   if (NULL != argv[0])
			   {
				   continue;
			   }
			   sip.prefix.s_addr = htonl(psess->sip);
			   inet_ntop ( AF_INET, ( void * ) &sip.prefix, sip_addr, 100 );

			   dip.prefix.s_addr = htonl(psess->dip);
			   inet_ntop ( AF_INET, ( void * ) &dip.prefix, dip_addr, 100 );

			   vty_out(vty,"%-16s:%-8d %-16s:%-8d ",dip_addr,psess->dport,sip_addr,psess->sport);

			   vty_out(vty, "%-11d%s", psess->vpn, VTY_NEWLINE);
			   flag ++;
		   }

	   }
   }

   vty_out(vty,  "%s-----------------------------------------------------------------%s",VTY_NEWLINE,VTY_NEWLINE);
   vty_out (vty, "%s udp session total num is: %d  %s",(argv[0]==NULL?"IPv4":"Ipv6"),flag,VTY_NEWLINE);
   return CMD_SUCCESS;
}


DEFUN (show_ipc_blockinfo,
       show_ipc_blockinfo_cmd,
       "show ipc <1-125> blockinfo",
	   "show\n"
	   "ipc\n"
	   "key value format<1-125>\n"
	   "blockinfo\n")
{
#if 0

	struct ipc_mesg_n mesg;
	int id = 0;
	uint32_t queue = 0;
	char *errmsg;

	const char *mod[] = {
			"MODULE_ID_INVALID",
			"MODULE_ID_IFM",
			"MODULE_ID_DEVM",
			"MODULE_ID_VTY",
			"MODULE_ID_MPLS",
			"MODULE_ID_FTM",
			"MODULE_ID_HAL",
			"MODULE_ID_HSL",
			"MODULE_ID_QOS",
			"MODULE_ID_OSPF",
			"MODULE_ID_ISIS",
			"MODULE_ID_RIP",
			"MODULE_ID_BGP",
			"MODULE_ID_ROUTE",
			"MODULE_ID_ARP",
			"MODULE_ID_LDP",
			"MODULE_ID_RSVPTE",
			"MODULE_ID_VRRP",
			"MODULE_ID_DHCP",
			"MODULE_ID_LACP",
			"MODULE_ID_L2",
			"MODULE_ID_SYSLOG",
			"MODULE_ID_FILE",
			"MODULE_ID_PING",
			"MODULE_ID_CES",
			"MODULE_ID_AAA",
			"MODULE_ID_SYSTEM",
			"MODULE_ID_BFD",
			"MODULE_ID_ALARM",	  /* 告警管理 */
			"MODULE_ID_NTP", /* NTP */
			"MODULE_ID_SNMPD",
			"MODULE_ID_SDHMGT",
			"MODULE_ID_RESV1",
			"MODULE_ID_WEB",
			"MODULE_ID_OPENFLOW", /* OpenFlow 模块*/
			"MODULE_ID_NETCONF", /* NETCONFIG 模块*/
			"MODULE_ID_IPMC", /* 组播 */
			"MODULE_ID_CLOCK", /* 时钟 */
			"MODULE_ID_HA", /* HA 模块 */
			"MODULE_ID_OSPF6",
			"MODULE_ID_DHCPV6",
			"MODULE_ID_SNMPD_TRAP"
    };
    const char *ipctype[] = {
		   "IPC_TYPE_INVALID",
		   "IPC_TYPE_LSP",	  /* mpls lsp */
		   "IPC_TYPE_ILM",	  /* mpls ilm */
		   "IPC_TYPE_NHLFE",  /* mpls nhlfe */
		   "IPC_TYPE_PW",	  /* mpls pw */
		   "IPC_TYPE_VSI",	  /* mpls vsi */
		   "IPC_TYPE_L3VPN",    /* mpls l3vpn */
		   "IPC_TYPE_MPLSOAM",/* mpls oam*/
		   "IPC_TYPE_ROUTE",  /* ip 路由 */
		   "IPC_TYPE_FIB",	  /* ip fib */
		   "IPC_TYPE_NHP",	  /* ip nhp */
		   "IPC_TYPE_L3IF",   /* ip l3if */
		   "IPC_TYPE_ARP",	  /* arp 全局信息*/
		   "IPC_TYPE_ARPIF",  /* arp 接口信息 */
		   "IPC_TYPE_NDP",	  /* ndp 全局信息*/
		   "IPC_TYPE_NDPIF",  /* ndp 接口信息*/
		   "IPC_TYPE_IFM",	  /* 接口管理 */
		   "IPC_TYPE_FTM",	  /* 软转发 */
		   "IPC_TYPE_VLAN",   /* vlan */
		   "IPC_TYPE_L2IF",   /* l2if */
		   "IPC_TYPE_MPLSIF", /* mpls if */
		   "IPC_TYPE_DEVM",   /* 设备管理*/
		   "IPC_TYPE_CES",	  /* ces */
		   "IPC_TYPE_ACL",	  /* qos acl */
		   "IPC_TYPE_CAR",	  /* qos car */
		   "IPC_TYPE_QOS",	  /* QOS 全局信息 */
		   "IPC_TYPE_HQOS",
		   "IPC_TYPE_QOSIF",  /* QOS 接口配置 */
		   "IPC_TYPE_CFM",	  /* CFM 消息 */
		   "IPC_TYPE_PROTO",  /* 协议注册*/
		   "IPC_TYPE_PACKET", /* 转发报文 */
		   "IPC_TYPE_VTY",	  /* VTY 消息 */
		   "IPC_TYPE_SYSLOG", /* syslog 消息 */
		   "IPC_TYPE_AAA",	  /* AAA 消息 */
		   "IPC_TYPE_BFD",	  /* BFD 消息 */
		   "IPC_TYPE_TRUNK",  /* trunk */
		   "IPC_TYPE_MAC",	  /* mac 地址消息 */
		   "IPC_TYPE_SYNCE",  /* synce 消息 */
		   "IPC_TYPE_TUNNEL", /* tunnel 消息 */
		   "IPC_TYPE_ALARM",  /* 告警消息 */
		   "IPC_TYPE_FILE", /* 文件管理消息 */
		   "IPC_TYPE_SNMP",   /* SNMP 消息 */
		   "IPC_TYPE_SLA",	/* SLA 消息 */
		   "IPC_TYPE_OSPF", /* OSPF 消息 */
		   "IPC_TYPE_RIP",	/* RIP 消息 */
		   "IPC_TYPE_ISIS", /* ISIS 消息 */
		   "IPC_TYPE_OFP",	  /* openflow 消息 */
		   "IPC_TYPE_ELPS",   /* ELPS 消息 */
		   "IPC_TYPE_BGP",	  /* BGP 消息 */
		   "IPC_TYPE_HA",	  /* HA 消息 */
           "IPC_TYPE_MSTP",   /* MSTP 消息 */ 
           "IPC_TYPE_SFP",    /* SFP消息 */
           "IPC_TYPE_IPMC",   /* IPMC message */
           "IPC_TYPE_MPLSAPS",/* mpls aps*/
           "IPC_TYPE_EFM",    /* efm message */
           "IPC_TYPE_SYSTEM", /* system message*/
           "IPC_TYPE_SYNC_DATA",
           "IPC_TYPE_LDP",    /* mpls ldp */
           "IPC_TYPE_SDHMGT", /* sdhmgt message */
           "IPC_TYPE_MAX"
    };

	if (NULL != argv[0])
	{
    	queue = (uint32_t)atoi(argv[0]);
	}

	id = ipc_connect(queue);

	if (id < 0)
	{
		vty_out (vty, "key %d connect failed!%s", queue, VTY_NEWLINE);
		return CMD_WARNING;
	}

	vty_out (vty, "key = %d, msqid = 0x%x%s", queue, id, VTY_NEWLINE);

	/* 读一次队列内容 */
	memset(&mesg, 0, sizeof(struct ipc_mesg_n));
	if (-1 == ipc_recv_pkt(id, &mesg, 0))
	{
		errmsg = strerror(errno);
		vty_out (vty, "Read fail!, %s\n",errmsg);
		return CMD_WARNING;
	}


	if(mesg.msghdr.msg_type < sizeof(ipctype)/sizeof(char *))
	{
		vty_out (vty, "IPC_TYPE = %s%s", ipctype[mesg.msghdr.msg_type], VTY_NEWLINE);
	}
	else
	{
		vty_out (vty, "IPC_TYPE = %d%s", mesg.msghdr.msg_type, VTY_NEWLINE);
	}
	if(mesg.msghdr.sender_id < sizeof(mod)/sizeof(char *))
	{
		vty_out (vty, "sender_id = %s%s", mod[mesg.msghdr.sender_id], VTY_NEWLINE);
	}
	else
	{
		vty_out (vty, "sender_id = %d%s", mesg.msghdr.sender_id, VTY_NEWLINE);
	}

	if((unsigned)mesg.msghdr.module_id < sizeof(mod)/sizeof(char *))
	{
		vty_out (vty, "module_id = %s%s", mod[mesg.msghdr.module_id], VTY_NEWLINE);
	}
	else
	{
		vty_out (vty, "module_id = %d%s", mesg.msghdr.module_id, VTY_NEWLINE);
	}

	vty_out (vty, "subtype = %d%s", mesg.msghdr.msg_subtype, VTY_NEWLINE);
	vty_out (vty, "opcode = %d%s", mesg.msghdr.opcode, VTY_NEWLINE);
	vty_out (vty, "data_len = %d%s", mesg.msghdr.data_len, VTY_NEWLINE);
#endif
	return CMD_SUCCESS;
}

DEFUN(tcp_mode,
   tcp_mode_cmd,
   "tcp",
   "TCP\n")
{
   vty->node = TCP_NODE;
   return CMD_SUCCESS;
}


DEFUN(tcp_keepalive_control_switch,
   tcp_keepalive_control_switch_cmd,
   "tcp keepalive (enable | disable)",
   "TCP\n"
   "Tcp keepalive\n"
   "enable\n"
   "disable\n")
{
	extern uint16_t tcp_keepalive_switch;

	if((memcmp(argv[0],"e",1)) == 0)
	{
	   tcp_keepalive_switch = 1;
	}
	else if((memcmp(argv[0],"d",1)) == 0)
	{
	   tcp_keepalive_switch = 0;
	}
	else
	{
	   vty_error_out(vty,"% %s[%d] : Wrong Format\n", __FUNCTION__, __LINE__);
	   return CMD_WARNING;
	}
	return CMD_SUCCESS;
}


DEFUN(tcp_keepalive_control,
   tcp_keepalive_time_control_cmd,
   "tcp keepalive-time <60-7200>",
   "TCP\n"
   "keepalive time\n"
   "<60-7200>more than 60s,less than 7200s(2h)\n")
{
   char *endptr = NULL;
   int alive_time = 0;
   extern uint16_t tcp_keepalive_time;
   extern uint16_t tcp_keepalive_switch;

   alive_time = strtoul (argv[0], &endptr, 10);
   if(( alive_time < 60 ) || ( alive_time > 7200 ))
   {
	   vty_error_out(vty,"%s[%d] : Wrong Format\n", __FUNCTION__, __LINE__);
	   return CMD_WARNING;
   }

   if(tcp_keepalive_switch == 0)
   {
	   vty_error_out(vty,"% %s[%d] : tcp keepalive is disable.\n", __FUNCTION__, __LINE__);
	   return CMD_WARNING;
   }
   /*tcp_keepalive_send浣跨敤鐨勬槸tcp_status_check涓殑瀹氭椂鍣紝璇ュ畾鏃跺櫒鏄?3绉掍竴娆★紝鎵?浠ヨ繖閲? /3*/
   tcp_keepalive_time = alive_time/3;

   return CMD_SUCCESS;
}

DEFUN(show_tcp_config,
   show_tcp_config_cmd,
   "show tcp config",
   "show\n"
   "TCP\n"
   "the config of TCP\n")
{
	extern uint16_t tcp_keepalive_time;
	extern uint16_t tcp_keepalive_switch;

	vty_out(vty, "TCP Configuration%s", VTY_NEWLINE);
	vty_out(vty, "    TCP keepalive status :%s%s",tcp_keepalive_switch?"enable":"disable" , VTY_NEWLINE);
	vty_out(vty, "    TCP keepalive-time :%d%s",tcp_keepalive_time*3 , VTY_NEWLINE);
	vty_out(vty, "%s", VTY_NEWLINE);
	return CMD_SUCCESS;
}

DEFUN (config_dynamic_arp_to_static,
  config_dynamic_arp_to_static_cmd,
  "arp to-static {ip A.B.C.D} {l3vpn <1-1024>}",
  "Arp\n"
  "Dynamic arp to static\n"
  "IP address\n"
  "IP format A.B.C.D\n"
  "L3vpn\n"
  "L3vpn value <1-1024>\n")
{
   uint32_t ipaddr = 0;
   uint16_t vpnid = 0;
   uint32_t cursor = 0;
   struct arp_entry *parp = NULL;
   struct hash_bucket *pbucket = NULL;
   struct arp_key *parp_key = NULL;
   struct listnode *p_listnode = NULL;
   int8_t ret = 0;

   
   /*参数获取及合法性检查*/
   if( NULL != argv[0] )
   {
	   ipaddr = inet_strtoipv4((char *)argv[0]);   
	   ret = inet_valid_ipv4(ipaddr);
	   if (1 != ret)
	   {
		   vty_error_out(vty, "The ip is invalid.%s", VTY_NEWLINE);
		   return CMD_WARNING;
	   }
   }

   if ( NULL != argv[1] )
   {
	   vpnid = (uint16_t)atoi(argv[1]);
   }

   if (vpnid > VPN_SUPPORT )
   {
	   vty_error_out(vty, "This device support vpn range <1-%d>!%s",VPN_SUPPORT,VTY_NEWLINE);
	   return CMD_SUCCESS;
   }

   if( ipaddr )
   {
	   parp = arp_lookup(ipaddr, vpnid);
	   if ( NULL != parp)
	   {
		   if(parp->status == ARP_STATUS_STATIC    || 
		   parp->status == ARP_STATUS_INCOMPLETE ||
		   parp->status == ARP_STATUS_LLDP)
		   {
			   vty_error_out(vty, "this arp entry is not dynamic.%s",VTY_NEWLINE) ;
			   return CMD_WARNING;
		   }
		   parp->status_old = parp->status;
		   parp->status = ARP_STATUS_STATIC;
	   }

	   for(ALL_LIST_ELEMENTS_RO(garp_to_static_list.arp_to_static_list, p_listnode, parp_key))
	   {  
		   if(ipaddr == parp_key->ipaddr && vpnid == parp_key->vpnid)
		   {
			   vty_error_out(vty, "The arp is already exist%s", VTY_NEWLINE);
			   return CMD_WARNING;
		   }
	   }
	   
	   parp_key = (struct arp_key*)XCALLOC(MTYPE_ARP_ENTRY, sizeof(struct arp_key));
	   if(NULL == parp_key)
	   {
		   zlog_err("%s[%d]:leave %s:error:fail to XCALLOC struct vlan_mapping\n",__FILE__,__LINE__,__func__);
		   return ERRNO_FAIL;
	   }

	   memset(parp_key,0,sizeof(struct arp_key));
	   parp_key->ipaddr = ipaddr;
	   parp_key->vpnid = vpnid;
	   listnode_add(garp_to_static_list.arp_to_static_list, (void *)parp_key);

	   if(garp_to_static_list.status == ARP_TO_STATIC_DIS)
		   garp_to_static_list.status = ARP_TO_STATIC_PART;
   }
   else
   {
	   HASH_BUCKET_LOOPW(pbucket, cursor, arp_table)
	   {
		   if(pbucket->data)
		   {
			   parp = (struct arp_entry *)pbucket->data;
			   pbucket = pbucket->next;

			   /*静态arp 不老化*/
			   if(parp->status == ARP_STATUS_STATIC    || 
				   parp->status == ARP_STATUS_INCOMPLETE ||
				   parp->status == ARP_STATUS_LLDP)
				   continue;

			   parp->status_old = parp->status;
			   parp->status = ARP_STATUS_STATIC;
		   }
	   }

	   if(garp_to_static_list.status != ARP_TO_STATIC_ALL)
		   garp_to_static_list.status = ARP_TO_STATIC_ALL;
   }

   return CMD_SUCCESS;
}

DEFUN (no_config_dynamic_arp_to_static,
  no_config_dynamic_arp_to_static_cmd,
  "no arp to-static {ip A.B.C.D} {l3vpn <1-1024>}",
  NO_STR
  "Arp\n"
  "Dynamic arp to static\n"
  "IP address\n"
  "IP format A.B.C.D\n"
  "L3vpn\n"
  "L3vpn value <1-1024>\n")
{
   uint32_t ipaddr = 0;
   uint16_t vpnid = 0;
   uint32_t cursor = 0;
   struct arp_entry *parp = NULL;
   struct hash_bucket *pbucket = NULL;
   struct arp_key *parp_key = NULL;
   struct listnode *p_listnode = NULL;
   int8_t ret = 0;

	
   /*参数获取及合法性检查*/
   if( NULL != argv[0] )
   {
	   ipaddr = inet_strtoipv4((char *)argv[0]);   
	   ret = inet_valid_ipv4(ipaddr);
	   if (1 != ret)
	   {
		   vty_error_out(vty, "The ip is invalid.%s", VTY_NEWLINE);
		   return CMD_WARNING;
	   }
   }

   if ( NULL != argv[1] )
   {
	   vpnid = (uint16_t)atoi(argv[1]);
   }

   if (vpnid > VPN_SUPPORT )
   {
	   vty_error_out(vty, "This device support vpn range <1-%d>!%s",VPN_SUPPORT,VTY_NEWLINE);
	   return CMD_SUCCESS;
   }

   if( ipaddr )
   {
	   parp = arp_lookup(ipaddr, vpnid);
	   if ( NULL != parp)
	   {
		   if(parp->status == ARP_STATUS_STATIC && parp->status_old != 0)
		   {
			   parp->status = parp->status_old;
			   parp->status_old = 0;
		   }
		   else
		   {
			   vty_error_out(vty, "this arp entry is not dynamic to static.%s",VTY_NEWLINE) ;
			   return CMD_WARNING;
		   }
	   }
	   
	   for(ALL_LIST_ELEMENTS_RO(garp_to_static_list.arp_to_static_list, p_listnode, parp_key))
	   {  
		   if(ipaddr == parp_key->ipaddr && vpnid == parp_key->vpnid)
		   {
			   XFREE(MTYPE_ARP_ENTRY, parp_key);
			   list_delete_node(garp_to_static_list.arp_to_static_list, p_listnode);
			   break;
		   }
	   }

	   if(list_isempty(garp_to_static_list.arp_to_static_list) && garp_to_static_list.status != ARP_TO_STATIC_ALL)
	   {
		   garp_to_static_list.status = ARP_TO_STATIC_DIS;
	   }
   }
   else
   {
	   HASH_BUCKET_LOOPW(pbucket, cursor, arp_table)
	   {
		   if(pbucket->data)
		   {
			   parp = (struct arp_entry *)pbucket->data;
			   pbucket = pbucket->next;

			   if(parp->status == ARP_STATUS_STATIC && parp->status_old != 0)
			   {
				   parp->status = parp->status_old;
				   parp->status_old = 0;
			   }			  
		   }
	   }

	   for(ALL_LIST_ELEMENTS_RO(garp_to_static_list.arp_to_static_list, p_listnode, parp_key))
	   {  
		   if(parp_key)
			   XFREE(MTYPE_ARP_ENTRY, parp_key);
	   }
	   list_delete_all_node(garp_to_static_list.arp_to_static_list);
	   garp_to_static_list.status = ARP_TO_STATIC_DIS;
   }

   return CMD_SUCCESS;
}

DEFUN (show_dynamic_arp_to_static,
 show_dynamic_arp_to_static_cmd,
 "show arp to-static status",
 SHOW_STR
 "Arp\n"
 "Dynamic arp to static\n"
 "status info\n")
{
   char ipv4_addr[20];
   struct arp_key *parp_key = NULL;
   struct listnode *p_listnode = NULL;


   if(garp_to_static_list.status == ARP_TO_STATIC_DIS)
   {
	   vty_out(vty, "arp to_static status: disable%s", VTY_NEWLINE);
	   return CMD_SUCCESS;
   }

   if(garp_to_static_list.status == ARP_TO_STATIC_ALL)
   {
	   vty_out(vty, "arp to_static status: enable (for all arp entry)%s", VTY_NEWLINE);
	   return CMD_SUCCESS;
   }

   vty_out(vty, "arp to_static status: enable (for some arp entry as follows:)%s", VTY_NEWLINE);

   for(ALL_LIST_ELEMENTS_RO(garp_to_static_list.arp_to_static_list, p_listnode, parp_key))
   {  
	   inet_ipv4tostr(parp_key->ipaddr,ipv4_addr);
	   vty_out(vty, "arp to_static ip %s vpnid %d%s", ipv4_addr, parp_key->vpnid, VTY_NEWLINE); 	   
   }

   return CMD_SUCCESS;
}

DEFUN (ftm_pkt_debug_fun,
   ftm_pkt_debug_fun_cmd,
   "debug ftm (pkt|ip|tcp|udp|l3|arp|ndp) (enable|disable)",
   "Debug information to moniter\n"
   "Programe name\n"
   "FTM packet info\n"
   "IP debug info\n"
   "TCP debug info\n"
   "UDP debug info\n"
   "L3 debug info\n"
   "Arp debug info\n"
   "Ndp debug info\n"
   "debug enable\n"
   "debug disable\n")
{
   uint32_t zlog_num = 0;

   for( ; zlog_num < array_size(ftm_dbg_name); zlog_num++)
   {
	   if(!strncmp(argv[0], ftm_dbg_name[zlog_num].str, 2))
	   {
		   zlog_debug_set( vty, ftm_dbg_name[zlog_num].key, !strncmp(argv[1], "enable", 3));
		   return CMD_SUCCESS;
	   }
   }

   vty_out (vty, "No debug typd find %s", VTY_NEWLINE);

   return CMD_SUCCESS;
}

DEFUN (show_ftm_pkt_debug_fun,
		show_ftm_pkt_debug_fun_cmd,
		"show ftm debug",
		SHOW_STR
		"Ftm"
		"Debug status\n")
{
	int type_num = 0;

	vty_out(vty, "debug type		 status %s", VTY_NEWLINE);

	for(type_num = 0; type_num < array_size(ftm_dbg_name); ++type_num)
	{
		vty_out(vty, "%-15s	 %-10s %s", ftm_dbg_name[type_num].str, 
		   !!(vty->monitor & (1 << type_num)) ? "enable" : "disable", VTY_NEWLINE);
	}

	return CMD_SUCCESS;
}


void
ftm_debug_init (void)
{
//	install_node (&debug_node, NULL);
//	install_element(DEBUG_NODE, &debug_cmd);
//	install_default(CONFIG_NODE);
	install_node(&tcp_node,tcp_config_write);
	install_default( TCP_NODE );
	install_element ( CONFIG_NODE, &tcp_mode_cmd,CMD_SYNC);
	install_element ( TCP_NODE, &tcp_keepalive_time_control_cmd,CMD_SYNC);
	install_element ( TCP_NODE, &tcp_keepalive_control_switch_cmd,CMD_SYNC);
	install_element ( CONFIG_NODE, &show_tcp_config_cmd,CMD_LOCAL);


	ifm_l3if_init(NULL);

	install_element (PHYSICAL_IF_NODE, &clear_arp_cmd, CMD_LOCAL);
	install_element (PHYSICAL_SUBIF_NODE, &clear_arp_cmd, CMD_LOCAL);
	install_element (TRUNK_IF_NODE, &clear_arp_cmd, CMD_LOCAL);
	install_element (VLANIF_NODE, &clear_arp_cmd, CMD_LOCAL);

    install_element (CONFIG_NODE, &clear_arp_cmd, CMD_LOCAL);
    install_element (CONFIG_NODE, &clear_arp_interface_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &clear_arp_l3vpn_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &clear_arp_ip_cmd, CMD_LOCAL);

	install_element (CONFIG_NODE, &show_arp_interface_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_arp_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_arp_l3vpn_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_all_arp_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_arp_statistics_cmd, CMD_LOCAL);
    install_element (CONFIG_NODE, &config_arp_miss_anti_sip_cmd, CMD_LOCAL);
    install_element (CONFIG_NODE, &undo_config_arp_miss_anti_sip_cmd, CMD_LOCAL);
    install_element (CONFIG_NODE, &config_arp_miss_anti_vlan_cmd, CMD_LOCAL);
    install_element (CONFIG_NODE, &undo_config_arp_miss_anti_vlan_cmd, CMD_LOCAL);
    install_element (CONFIG_NODE, &config_arp_miss_anti_ifidx_cmd, CMD_LOCAL);
    install_element (CONFIG_NODE, &undo_config_arp_miss_anti_ifidx_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &config_dynamic_arp_to_static_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &no_config_dynamic_arp_to_static_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_dynamic_arp_to_static_cmd, CMD_LOCAL);

    install_element (CONFIG_NODE, &show_ipv6_ndp_cmd, CMD_LOCAL);
    install_element (CONFIG_NODE, &show_ipv6_ndp_ip_cmd, CMD_LOCAL);
    install_element (CONFIG_NODE, &show_ipv6_ndp_interface_cmd, CMD_LOCAL);
    install_element (CONFIG_NODE, &show_ipv6_ndp_l3vpn_cmd, CMD_LOCAL);
    install_element (CONFIG_NODE, &clear_ipv6_ndp_interface_cmd, CMD_LOCAL);
    install_element (CONFIG_NODE, &clear_ipv6_ndp_all_cmd, CMD_LOCAL);
    install_element (CONFIG_NODE, &show_ipv6_ndp_statistics_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_ftm_fib_all_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_ftm_fib_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_ftm_fibv6_all_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_ftm_fibv6_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_ftm_pw_index_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_ftm_if_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_ftm_if_all_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_ftm_ilm_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_ftm_nhlfe_cmd, CMD_LOCAL);
    install_element (CONFIG_NODE, &show_ftm_tunnel_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &debug_ftm_fib_add_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &debug_ftm_fib_delete_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &debug_ftm_nhp_add_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &debug_ftm_nhp_delete_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_ftm_nhp_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_ftm_ecmp_cmd, CMD_LOCAL);

#if 0
    install_element (CONFIG_NODE, &debug_ftm_nhlfe_add_cmd);
	install_element (CONFIG_NODE, &debug_ftm_pw_nhlfe_add_cmd);
    install_element (CONFIG_NODE, &debug_ftm_nhlfe_delete_cmd);
    install_element (CONFIG_NODE, &debug_ftm_ilm_add_cmd);
	install_element (CONFIG_NODE, &debug_ftm_pw_ilm_add_cmd);
    install_element (CONFIG_NODE, &debug_ftm_ilm_delete_cmd);
	install_element (CONFIG_NODE, &debug_ftm_pw_add_cmd);
	install_element (CONFIG_NODE, &debug_ftm_pw_delete_cmd);
#endif

	install_element (CONFIG_NODE, &debug_ftm_packet_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &no_debug_ftm_packet_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_ftm_packet_stat_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &clear_ftm_packet_stat_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_tcp_session_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_udp_session_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_ipc_blockinfo_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_ftm_protocol_cmd, CMD_LOCAL);

	install_element (CONFIG_NODE, &ftm_pkt_debug_fun_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_ftm_pkt_debug_fun_cmd, CMD_LOCAL);	
}

int
cmd_str2int (char *str, int *ret)
{
	int i			= 0;
	int len			= 0;
	int digit		= 0;
	uint32_t limit	= 0;
	uint32_t remain	= 0;
	int minus		= 0;
	uint32_t max 	= UINT32_MAX;
	uint32_t total	= 0;

	/* Sanify check. */
	if (str == NULL || ret == NULL)
		return -1;

	/* First set return value as error. */
	*ret = -1;

	len = strlen (str);
	if (*str == '+')
	{
		str++;
		len--;
	}
	else if (*str == '-')
	{
		str++;
		len--;
		minus = 1;
		max = max / 2 + 1;
	}

	limit = max / 10;
	remain = max % 10;

	if (len < DECIMAL_STRLEN_MIN || len > DECIMAL_STRLEN_MAX)
		return -1;

	for (i = 0; i < len; i++)
	{
		if (*str < '0' || *str > '9')
			return -1;

		digit = *str++ - '0';

		if (total  > limit || (total == limit && digit > (int)remain))
			return -1;

		total = total * 10 + digit;
	}

	*ret = 0;

	if (minus && (total == 0))
	{
		return -1;
	}

	if (minus)
	{
		return - total;
	}
	else
	{
		return total;
	}
}

