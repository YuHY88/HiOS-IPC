#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <lib/types.h>
#include <lib/command.h>
#include <lib/inet_ip.h>
#include <lib/ether.h>
#include <lib/vty.h>
#include <lib/log.h>
#include <lib/module_id.h>
#include <lib/msg_ipc.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/prefix.h>
#include <lib/ifm_common.h>
#include <lib/mpls_common.h>
#include <lib/errcode.h>
#include "ftm/pkt_icmp.h"
#include "mpls_ping.h"
#include "mpls_ping_cmd.h"
#include "pw.h"
#include "lsp_static.h"
#include "tunnel.h"
#include "lspm.h"
#include "lsp_fec.h"


struct lsp_entry * lookup_lsp_by_tunnel_index(uint32_t lsp_index)
{
	struct lsp_entry *lsp_entry = NULL;
	
	lsp_entry = mpls_lsp_lookup(lsp_index);
	if(NULL == lsp_entry)
	{
		return NULL;		
	}
	return lsp_entry ;
}

uint32_t ping_select_ldp_lsp(struct inet_prefix *destprefix, struct inet_addr *nhpaddr)
{
	struct lsp_fec_t *pfec = NULL;
	struct lsp_nhp_t *pnhp = NULL;
	struct listnode  *pnode = NULL;

	if(NULL == destprefix || NULL == nhpaddr)
	{
		return 0;
	}

	pfec = lsp_fec_match(destprefix);
	if(NULL == pfec)
	{	
		zlog_err("%s[%s]:Can't find lsp fec.--line:%d",
			__FILE__, __FUNCTION__, __LINE__);
		return 0;
	}
	else if(pfec->nhplist.count == 0)
	{
		zlog_err("%s[%s]:Lsp fec has no nhp.--line:%d",
			__FILE__, __FUNCTION__, __LINE__);
		return 0;
	}

	for(ALL_LIST_ELEMENTS_RO(&pfec->nhplist, pnode, pnhp))
	{
		if((NULL == pnhp) || (0 == pnhp->ingress_lsp)
			|| (pnhp->nexthop.addr.ipv4 != nhpaddr->addr.ipv4))
		{
			continue;
		}
		return pnhp->ingress_lsp;
	}
		
	return 0;
}


DEFUN(ping_tunnel,
	ping_tunnel_cmd,
	"ping tunnel USP",
	"Ping\n"
	CLI_INTERFACE_TUNNEL_STR
	CLI_INTERFACE_TUNNEL_VHELP_STR
	)
{
	struct ping_config ping_info;
	uint32_t ifindex = 0;
	struct tunnel_if *pif = NULL;
	struct lsp_entry *lsp_entry = NULL;
	int ret = 0;

    ifindex = ifm_get_ifindex_by_name ( "tunnel", ( char * ) argv[0] );	
	pif = tunnel_if_lookup(ifindex);
	if(pif == NULL)
	{
        vty_error_out(vty, "%%Tunnel %s is not exit or no protocol!%s", ( char * ) argv[0],  VTY_NEWLINE );
		return CMD_WARNING;
	}
	
	
	if(LINK_DOWN == pif->tunnel.down_flag)
	{
		vty_error_out(vty, "The specified tunnel is down.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(TUNNEL_PRO_MPLSTP != pif->tunnel.protocol)
	{
		vty_error_out(vty, "The specified tunnel protocol is not support.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	if(pif->tunnel.backup_status == TUNNEL_STATUS_MASTER)
	{
		lsp_entry = lookup_lsp_by_tunnel_index(pif->tunnel.master_index);
	}
	else if(pif->tunnel.backup_status == TUNNEL_STATUS_BACKUP)
	{
		lsp_entry = lookup_lsp_by_tunnel_index(pif->tunnel.backup_index);
	}
	
	if(lsp_entry == NULL)
	{
		return CMD_WARNING;
	}

    /* ����ping ����*/
	memset(&ping_info, 0, sizeof(struct ping_config));	
	ping_info.type = PING_TYPE_TUNNEL;
	ping_info.tlv_type = MPLS_PING_TLV_PAD;
	ping_info.ifindex = ifindex;
	ping_info.timeout = 5000;  /* 5s */
	ping_info.size = 100;	
	ping_info.count = 1;
	ping_info.ttl = 255;
	ping_info.pvty = vty;
	ping_info.ipv4_prefix = lsp_entry->destip.addr.ipv4;
	ping_info.nexthop = lsp_entry->nexthop.addr.ipv4;
	ret = mpls_ping_send_request( &ping_info );
	if(ret == ERRNO_FAIL)
	{
        vty_error_out(vty, "Mpls pkt send failed!%s", VTY_NEWLINE );
	}
	
	vty_out(vty, "Ping Tunnel %s : %d data bytes, press CTRL_C to break%s", ( char * ) argv[0], ping_info.size, VTY_NEWLINE);
	vtysh_flush(vty);
	
    return CMD_WAIT;	
}


#if 0
DEFUN(ping_lsp,
	ping_lsp_cmd,
	"ping lsp ip A.B.C.D/M nexthop A.B.C.D",
	"Ping\n"
	"Lsp\n"
	"Specify IPv4 LSP's destination\n"
	"IPv4 LSP's destination\n"
	"Next hop\n"
	"Next hop address\n")
{
	struct lsp_entry *ldp_lsp = NULL;
	struct prefix_ipv4 destip;
	struct prefix_ipv4 nexthop;
	struct inet_prefix prefix;//host addr
	struct inet_addr   nhpaddr;
	uint32_t host_ipaddr = 0;
	uint32_t nhp     = 0;
	uint32_t lspindex = 0;
	int ret = 0;

	ret = str2prefix_ipv4(argv[0], &destip);
	if(0 == ret)
	{
		vty_error_out(vty, "Invalid ip address.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	host_ipaddr = ipv4_network_addr(destip.prefix.s_addr, destip.prefixlen);
	host_ipaddr = ntohl(host_ipaddr);
	ret = inet_valid_network(host_ipaddr);
	if(FALSE == ret)
	{
		vty_error_out(vty, "Invalid dest ip addr.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	prefix.addr.ipv4 = host_ipaddr;
	prefix.type      = destip.family;
	prefix.prefixlen = destip.prefixlen;

	
	ret = str2prefix_ipv4(argv[1], &nexthop);
	if (0 == ret)
	{
		vty_error_out(vty, "The specified nexthop address is invalid.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	nhp = ntohl(nexthop.prefix.s_addr);
	ret = inet_valid_network(nhp);
	if(FALSE == ret)
	{
		vty_error_out(vty, "Invalid nexthop ip addr.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	nhpaddr.addr.ipv4 = nhp;

	lspindex = ping_select_ldp_lsp(&prefix, &nhpaddr);
	if(0 == lspindex)
	{
		vty_error_out(vty, "Can't find the lsp for destip:%s, nexthop:%s.%s",
				argv[0], argv[1], VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	ldp_lsp = mpls_lsp_lookup(lspindex);
	if(NULL == ldp_lsp)
	{
		vty_error_out(vty, "The lsp of destip:%s, nexthop:%s is not exist.%s",
				argv[0], argv[1], VTY_NEWLINE);
		return CMD_WARNING;
	}
	
}
#endif
DEFUN(ping_lsp,
	ping_lsp_cmd,
	"ping lsp {name LSPNAME | ip A.B.C.D/M | nexthop A.B.C.D}",
	"Ping\n"
	"Lsp\n"
	"Lspname"
	"The name for lsp -- max.31\n"
	"Specify IPv4 LSP's destination\n"
	"IPv4 LSP's destination\n"
	"Next hop\n"
	"Next hop address\n")
{
	struct ping_config ping_info;
	struct static_lsp *plsp = NULL;
	struct lsp_entry  *lspm = NULL;
	struct prefix_ipv4 destip;
	struct prefix_ipv4 nexthop;
	struct inet_prefix dsprefix;//host addr
	struct inet_addr nhpaddr;
	uint32_t host_ipaddr = 0;
	uint32_t nhp         = 0;
	uint32_t lspindex    = 0;
	int ret              = 0;
	
	if(NULL != argv[0])
	{
		plsp = static_lsp_lookup((uchar *)argv[0]);
		if (NULL == plsp)
		{
			vty_error_out(vty, "The specified Lsp does not exist.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		lspindex = plsp->lsp_index;
	}

	if(argv[1] != NULL && argv[2] != NULL)
	{
		memset(&destip, 0, sizeof(struct prefix_ipv4));
		memset(&nexthop, 0, sizeof(struct prefix_ipv4));
		memset(&dsprefix, 0, sizeof(struct inet_prefix));
		memset(&nhpaddr, 0, sizeof(struct inet_addr));
		
		/* destination addr,struct in_addr is network byte order:big indian */
		ret = str2prefix_ipv4(argv[1], &destip);
		if(0 == ret)
		{
			vty_error_out(vty, "Invalid ip address.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}

		host_ipaddr = ipv4_network_addr(destip.prefix.s_addr, destip.prefixlen);
		host_ipaddr = ntohl(host_ipaddr);
		ret = inet_valid_network(host_ipaddr);
		if(FALSE == ret)
		{
			vty_error_out(vty, "Invalid dest ip.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		
		dsprefix.addr.ipv4 = host_ipaddr;
		dsprefix.type      = destip.family;
		dsprefix.prefixlen = destip.prefixlen;

		/* nexthop addr */
		ret = str2prefix_ipv4(argv[2], &nexthop);
		if (0 == ret)
		{
			vty_error_out(vty, "The specified nexthop address is invalid.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}

		nhp = ntohl(nexthop.prefix.s_addr);
		ret = inet_valid_network(nhp);
		if(FALSE == ret)
		{
			vty_error_out(vty, "Invalid nexthop ip addr.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		nhpaddr.addr.ipv4 = nhp;

		lspindex = ping_select_ldp_lsp(&dsprefix, &nhpaddr);
	}
	
	if(0 == lspindex)
	{
		vty_error_out(vty, "The specified Lsp does not exist.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	lspm = mpls_lsp_lookup(lspindex);
	if(NULL == lspm)
	{
		vty_error_out(vty, "The specified Lsp does not exist.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	if(LSP_DIRECTION_INGRESS != lspm->direction)
	{
		vty_error_out(vty, "The direction of Lsp does not suport.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(LINK_DOWN == lspm->down_flag)
	{
		vty_error_out(vty, "The specified lsp is down.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	/* ����ping ����*/
	memset(&ping_info, 0, sizeof(struct ping_config));	
	ping_info.type = PING_TYPE_LSP;
	ping_info.tlv_type = MPLS_PING_TLV_PAD;
	ping_info.lsp_index = lspm->lsp_index;
	ping_info.timeout = 5000;  /* 5s */
	ping_info.size = 100;	
	ping_info.count = 1;
	ping_info.ttl = 255;
	ping_info.pvty = vty;
	ping_info.ipv4_prefix = lspm->destip.addr.ipv4;
	ping_info.nexthop = lspm->nexthop.addr.ipv4;
	mpls_ping_send_request( &ping_info );
	
	if(argv[0] != NULL)
	{
		vty_out(vty, "Ping lsp: %s : %d data bytes, press CTRL_C to break%s",
			( char * ) argv[0], ping_info.size, VTY_NEWLINE);
	}
	if(argv[1] != NULL)
	{
		vty_out(vty, "Ping lsp: %s, nexthop: %s : %d data bytes, press CTRL_C to break%s",
			( char * ) argv[1], (char *)argv[2], ping_info.size, VTY_NEWLINE);
	}
	
	vtysh_flush(vty);

    return CMD_WAIT;	
}


DEFUN(ping_pw,
	ping_pw_cmd,
    "ping pw NAME  {remote A.B.C.D | pw-id <1-4294967295> | -r (2|4)}",
    "Ping\n"
    "Pw\n"
    "The name for pw -- max.31\n"
    "Remote PW information\n"
	"Remote ip:X.X.X.X\n"
	"Remote pw id\n"
	"Range of remote pw id:<1-4294967295>\n"
	"Reply mode\n"
	"Reply via an IPv4/IPv6 UDP packet\n"
	"Reply via application level control channel\n")
{
	struct ping_config ping_info;
	struct l2vc_entry *l2vc_entry= NULL;
	int ret = 0;
	uint8_t reply_mode=PING_PW_REPLY_BY_UDP;
	
	l2vc_entry = l2vc_lookup((uchar *)argv[0]);
	if (NULL == l2vc_entry)
	{
		vty_error_out(vty, "The specified l2vc_entry does not exist.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	if(LINK_DOWN == l2vc_entry->pwinfo.up_flag)
	{
		vty_error_out(vty, "The specified pw is down.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/* ����ping ����*/
	memset(&ping_info, 0, sizeof(struct ping_config));
	ping_info.type = PING_TYPE_PW;
	ping_info.tlv_type = MPLS_PING_TLV_PAD;
	if(l2vc_entry->pswitch_pw == NULL)
	{
		ping_info.pw_index = l2vc_entry->pwinfo.pwindex;
	}
	else
	{
		/* ��ftm���ͻ��ƣ�ָ����ʵ�ʷ��ͷ����뷴��pw */
		ping_info.pw_index = l2vc_entry->pswitch_pw->pwinfo.pwindex;
	}
	
	ping_info.timeout = 5000;  /* 5s */
	ping_info.size = 100;	
	ping_info.count = 1;
	ping_info.ttl = 255;
	ping_info.pvty = vty;
	if(argv[1] != NULL)
	{
		ping_info.ipv4_prefix = inet_strtoipv4((char *)argv[1]);
	}
	else
	{
		ping_info.ipv4_prefix = l2vc_entry->peerip.addr.ipv4;
	}
	
	if(argv[2] != NULL)
	{
		ping_info.pw_id = atoi(argv[2]);
	}
	else
	{
		ping_info.pw_id = l2vc_entry->pwinfo.vcid;
	}

	if(l2vc_entry->pwinfo.ac_type == AC_TYPE_ETH)
	{
		if(l2vc_entry->pwinfo.tag_flag == 1)
		{
			ping_info.pw_type = 4;
		}
		else
		{
			ping_info.pw_type = 5;
		}
	}
	else if(l2vc_entry->pwinfo.ac_type == AC_TYPE_TDM)
	{
		ping_info.pw_type = 17;//tdm
	}

	if(argv[3]!=NULL && !strncmp(argv[3],"4",1))
	{
		reply_mode = PING_PW_REPLY_BY_PW;
	}
	
	ping_info.reply_mode = reply_mode;
	
	ret = mpls_ping_send_request( &ping_info );
	if(ret == ERRNO_FAIL)
	{
        vty_error_out(vty, "Mpls pkt send failed!%s", VTY_NEWLINE );
	}
	
	vty_out(vty, "Ping pw %s, %d data bytes,reply mode %s, press CTRL_C to break%s", ( char * ) argv[0], ping_info.size, (reply_mode==PING_PW_REPLY_BY_UDP)?"by udp":"by pw",VTY_NEWLINE);
	vtysh_flush(vty);
	
    return CMD_WAIT;	
}


DEFUN(tracert_tunnel,
	tracert_tunnel_cmd,
	"tracert tunnel USP -t <1-255>",
	"Tracert\n"
	CLI_INTERFACE_TUNNEL_STR
	CLI_INTERFACE_TUNNEL_VHELP_STR
	"Ttl\n"
	"<1-255>\n"
	)
{
	struct ping_config ping_info;
	uint32_t ifindex = 0;
	struct tunnel_if *pif = NULL;
	struct lsp_entry *lsp_entry = NULL; 
	int ret = 0;

    ifindex = ifm_get_ifindex_by_name ( "tunnel", ( char * ) argv[0] );	
	pif = tunnel_if_lookup(ifindex);
	if(pif == NULL)
	{
        vty_error_out(vty, "%%Tunnel %s is not exit or no protocol!%s", ( char * ) argv[0],  VTY_NEWLINE );
		return CMD_WARNING;
	}
	
	if(LINK_DOWN == pif->tunnel.down_flag)
	{
		vty_error_out(vty, "The specified tunnel is down.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(TUNNEL_PRO_MPLSTP != pif->tunnel.protocol)
	{
		vty_error_out(vty, "The specified tunnel protocol is not support.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	if(pif->tunnel.backup_status == TUNNEL_STATUS_MASTER)
	{
		lsp_entry = lookup_lsp_by_tunnel_index(pif->tunnel.master_index);
	}
	else if(pif->tunnel.backup_status == TUNNEL_STATUS_BACKUP)
	{
		lsp_entry = lookup_lsp_by_tunnel_index(pif->tunnel.backup_index);
	}

	if(lsp_entry == NULL)
	{
		return CMD_WARNING;
	}

    /* ����ping ����*/
	memset(&ping_info, 0, sizeof(struct ping_config));	
	ping_info.type = TRACERT_TYPE_TUNNEL;
	ping_info.tlv_type = MPLS_PING_TLV_DS_SINGLE;
	ping_info.ifindex = ifindex;
	ping_info.timeout = 5000;  /* 5s */
	ping_info.size = 100;	
	ping_info.count = 1;
	ping_info.ttl_curr = 1;
	ping_info.ttl = (uint8_t)atoi(argv[1]);
	ping_info.pvty = vty;
	ping_info.ipv4_prefix = lsp_entry->destip.addr.ipv4;
	ping_info.nexthop = lsp_entry->nexthop.addr.ipv4;
	ret = mpls_ping_send_request( &ping_info );
	if(ret == ERRNO_FAIL)
	{
        vty_error_out(vty, "Mpls pkt send failed!%s", VTY_NEWLINE );
	}
	
	vty_out(vty, "Tracert Tunnel %s : %d data bytes, press CTRL_C to break%s", ( char * ) argv[0], ping_info.size, VTY_NEWLINE);
	vty_out(vty, "%-8s%-17s%-17s%-10s%s", "TTL", "Replier", "Type", "Time", VTY_NEWLINE);
	vtysh_flush(vty);
	
    return CMD_WAIT;	
}

DEFUN(tracert_lsp,
	tracert_lsp_cmd,
	"tracert lsp {name NAME | ip A.B.C.D/M | nexthop A.B.C.D} -t <1-255>",
	"Tracert\n"
	"Lsp\n"
	"Lspname\n"
	"The name -- max.31\n"
	"Specify IPv4 LSP's destination\n"
	"IPv4 LSP's destination\n"
	"Next hop\n"
	"Next hop address\n"
	"Ttl\n"
	"<1-255>\n"
	)
{
	struct ping_config ping_info;
	struct static_lsp *plsp = NULL;
	struct lsp_entry  *lspm = NULL;
	struct prefix_ipv4 destip;
	struct prefix_ipv4 nexthop;
	struct inet_prefix dsprefix;//host addr
	struct inet_addr nhpaddr;
	uint32_t host_ipaddr = 0;
	uint32_t nhp         = 0;
	uint32_t lspindex = 0;
	int ret = 0;
	
	/* ����ping ����*/
	memset(&ping_info, 0, sizeof(struct ping_config));
	
	if(argv[0] != NULL)
	{
		plsp = static_lsp_lookup((uchar *)argv[0]);
		if (NULL == plsp)
		{
			vty_error_out(vty, "The specified static Lsp does not exist.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		lspindex = plsp->lsp_index;
	}
	if(argv[1] != NULL && argv[2] != NULL)
	{
		memset(&destip, 0, sizeof(struct prefix_ipv4));
		memset(&nexthop, 0, sizeof(struct prefix_ipv4));
		memset(&dsprefix, 0, sizeof(struct inet_prefix));
		memset(&nhpaddr, 0, sizeof(struct inet_addr));
		
		/* destination addr,struct in_addr is network byte order:big indian */
		ret = str2prefix_ipv4(argv[1], &destip);
		if(0 == ret)
		{
			vty_error_out(vty, "Invalid ip address.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}

		host_ipaddr = ipv4_network_addr(destip.prefix.s_addr, destip.prefixlen);
		host_ipaddr = ntohl(host_ipaddr);
		ret = inet_valid_network(host_ipaddr);
		if(FALSE == ret)
		{
			vty_error_out(vty, "Invalid dest ip.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		
		dsprefix.addr.ipv4 = host_ipaddr;
		dsprefix.type      = destip.family;
		dsprefix.prefixlen = destip.prefixlen;

		/* nexthop addr */
		ret = str2prefix_ipv4(argv[2], &nexthop);
		if (0 == ret)
		{
			vty_error_out(vty, "The specified nexthop address is invalid.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}

		nhp = ntohl(nexthop.prefix.s_addr);
		ret = inet_valid_network(nhp);
		if(FALSE == ret)
		{
			vty_error_out(vty, "Invalid nexthop ip addr.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		nhpaddr.addr.ipv4 = nhp;

		lspindex = ping_select_ldp_lsp(&dsprefix, &nhpaddr);
	}
	
	if(0 == lspindex)
	{
		vty_error_out(vty, "The specified Lsp does not exist.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	lspm = mpls_lsp_lookup(lspindex);
	if(NULL == lspm)
	{
		vty_error_out(vty, "The specified Lsp does not exist.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(LSP_DIRECTION_INGRESS != lspm->direction)
	{
		vty_error_out(vty, "The direction of static Lsp does not suport.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(LINK_DOWN == lspm->down_flag)
	{
		vty_error_out(vty, "The specified lsp is down.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	ping_info.type = TRACERT_TYPE_LSP;
	ping_info.tlv_type = MPLS_PING_TLV_DS_SINGLE;
	ping_info.lsp_index = lspm->lsp_index;
	ping_info.ipv4_prefix = lspm->destip.addr.ipv4;
	ping_info.nexthop = lspm->nexthop.addr.ipv4;
	ping_info.lsp_label = lspm->outlabel;
	ping_info.timeout = 5000;  /* 5s */
	ping_info.size = 100;	
	ping_info.count = 1;
	ping_info.ttl_curr = 1;
	ping_info.ttl = (uint8_t)atoi(argv[3]);
	ping_info.pvty = vty;
	ret = mpls_ping_send_request( &ping_info );
	if(ret == ERRNO_FAIL)
	{
        vty_error_out(vty, "Mpls pkt send failed!%s", VTY_NEWLINE );
	}
	
	vty_out(vty, "Tracert mpls lsp: %d data bytes, press CTRL_C to break%s", ping_info.size, VTY_NEWLINE);
	vty_out(vty, "%-8s%-17s%-17s%-10s%s", "TTL", "Replier", "Type", "Time", VTY_NEWLINE);
	vtysh_flush(vty);
	
    return CMD_WAIT;	
}

DEFUN(tracert_pw,
	tracert_pw_cmd,
	"tracert pw NAME -t <1-255> {remote A.B.C.D}",
	"Tracert\n"
	"Pw\n"
	"The name -- max.31\n"
	"Ttl\n"
	"<1-255>\n"
	"Remote ip\n"
	"IP address format A.B.C.D\n"
	)
{
	struct ping_config ping_info;
	struct l2vc_entry *l2vc_entry= NULL;
	struct tunnel_if *tunif = NULL;
	int ret = 0;
	
	/* ����ping ����*/
	memset(&ping_info, 0, sizeof(struct ping_config));
	
	l2vc_entry = l2vc_lookup((uchar *)argv[0]);
	if (NULL == l2vc_entry)
	{
		vty_error_out(vty, "The specified l2vc_entry does not exist.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(l2vc_entry->pswitch_pw != NULL)
	{
		vty_error_out(vty, "Switch-pw can't support tracert function.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(LINK_DOWN == l2vc_entry->pwinfo.up_flag)
	{
		vty_error_out(vty, "The specified pw is down.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	ping_info.nexthop = l2vc_entry->peerip.addr.ipv4;
	ping_info.type = TRACERT_TYPE_PW;
	ping_info.pw_index = l2vc_entry->pwinfo.pwindex;
	if(argv[2] != NULL)
	{
		ping_info.ipv4_prefix = inet_strtoipv4((char *)argv[2]);
	}
	else
	{
		ping_info.ipv4_prefix = l2vc_entry->peerip.addr.ipv4;
	}
	ping_info.pw_label = l2vc_entry->outlabel;

	if(l2vc_entry->pwinfo.nhp_type == NHP_TYPE_TUNNEL)
	{
		tunif = tunnel_if_lookup(l2vc_entry->pwinfo.nhp_index);
		if(NULL == tunif)
		{
			vty_error_out(vty, "Can't find tunnel.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		
		if(tunif->tunnel.protocol == TUNNEL_PRO_GRE)
		{
			ping_info.gre_pw = 1;
			ping_info.tlv_type = MPLS_PING_TLV_DS_SINGLE;
		}
		else
		{
			ping_info.gre_pw = 0;
			ping_info.tlv_type = MPLS_PING_TLV_DS_DOUBLE;
		}
	}

	ping_info.pw_id = l2vc_entry->pwinfo.vcid;
	if(l2vc_entry->pwinfo.ac_type == AC_TYPE_ETH)
	{
		if(l2vc_entry->pwinfo.tag_flag == 1)
		{
			ping_info.pw_type = 4;
		}
		else
		{
			ping_info.pw_type = 5;
		}
	}
	else if(l2vc_entry->pwinfo.ac_type == AC_TYPE_TDM)
	{
		ping_info.pw_type = 17;
	}
	
	ping_info.timeout = 5000;  /* 5s */
	ping_info.size = 100;	
	ping_info.count = 1;
	ping_info.ttl_curr = 1;
	ping_info.ttl = (uint8_t)atoi(argv[1]);
	ping_info.pvty = vty;
	ret = mpls_ping_send_request( &ping_info );
	if(ret == ERRNO_FAIL)
	{
        vty_error_out(vty, "Mpls pkt send failed!%s", VTY_NEWLINE );
	}
	
	vty_out(vty, "Tracert mpls pw: %d data bytes, press CTRL_C to break%s", ping_info.size, VTY_NEWLINE);
	vty_out(vty, "%-8s%-17s%-17s%-10s%s", "TTL", "Replier", "Type", "Time", VTY_NEWLINE);
	vtysh_flush(vty);
	
    return CMD_WAIT;	
}

void mpls_ping_CmdInit(void)
{
	install_element (CONFIG_NODE, &ping_lsp_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &ping_pw_cmd, CMD_LOCAL);	
	install_element (CONFIG_NODE, &ping_tunnel_cmd, CMD_LOCAL);	
	install_element (CONFIG_NODE, &tracert_lsp_cmd, CMD_LOCAL);	
	install_element (CONFIG_NODE, &tracert_pw_cmd, CMD_LOCAL);	
	install_element (CONFIG_NODE, &tracert_tunnel_cmd, CMD_LOCAL);	
	return;
}

