#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <lib/types.h>
#include <lib/command.h>
#include <lib/linklist.h>
#include <lib/prefix.h>
#include <lib/errcode.h>
#include <lib/ether.h>
#include <lib/ifm_common.h>

#include "qos_policy.h"
#include "qos_if.h"
#include "acl_cli.h"
#include "acl.h"


extern struct hash_table acl_group_table;


u_int32_t cmd_hexstr2int ( char *str, uint8_t max_len,uint8_t min_len, int *ret )
{
    int i;
    int j;
    int powv = 0;
    int len;
    int digit;
    u_int32_t total = 0;

    /* Sanify check. */
    if ( str == NULL || ret == NULL )
    { return -1; }

    /* First set return value as error. */
    *ret = -1;

    len = strlen ( str );
	if(*str != '0' )
	{
		return -1;
	}
	str ++;
	len --;
	if(*str != 'x' )
	{
		return -1;
	}
	str ++;
	len --;

	if((max_len == min_len)&&(len != max_len))
	{ return -1; }

	if((max_len > min_len)&&((len > max_len)||(len < min_len)))
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



const char* proto_type_parse_str(uint8_t proto_num)
{
  switch(proto_num)
  {
	case 1:
		return "icmp";
	case 2:
		return "igmp";
	case 4:
		return "ip";
	case 6:
		return "tcp";
	case 17:
		return "udp";
	default:
		return "unknown";
  }
}

void tcp_type_parse_str(uint8_t tcp_type,struct vty *vty)
{
	if(tcp_type&ACl_TCP_FLAG_FIN)
	{
		vty_out(vty, " %s", "fin");
	}
	if(tcp_type&ACl_TCP_FLAG_SYN)
	{
		vty_out(vty, " %s", "syn");
	}
	if(tcp_type&ACl_TCP_FLAG_RST)
	{
		vty_out(vty, " %s", "rst");
	}
	if(tcp_type&ACl_TCP_FLAG_PSH)
	{
		vty_out(vty, " %s", "psh");
	}
	if(tcp_type&ACl_TCP_FLAG_ACK)
	{
		vty_out(vty, " %s", "ack");
	}
	if(tcp_type&ACl_TCP_FLAG_URG)
	{
		vty_out(vty, " %s", "urg");
	}
}


static void acl_rule_mac_show ( struct vty *vty, struct mac_rule *mac_tmp, enum ACL_TYPE type)
{
	struct prefix_ipv4 dip;
    struct prefix_ipv4 sip;
	char dip_addr[16];
	char sip_addr[16];

	vty_out(vty," rule %d",mac_tmp->ruleid );

	if(type == ACL_TYPE_MAC)
	{
		vty_out(vty," mac");
	}
	else
	{
		vty_out(vty," mix");
	}

	if(mac_tmp->ethtype_mask == 1)
	{
		if(mac_tmp->ethtype == 0)
		{
			vty_out(vty," ethtype any");
		}
		else
		{
			vty_out(vty," ethtype 0x%04x",mac_tmp->ethtype );
		}
	}
	if(mac_tmp->smac_mask == 1)
	{
		vty_out(vty," smac %02x:%02x:%02x:%02x:%02x:%02x",
			mac_tmp->smac[0],mac_tmp->smac[1],mac_tmp->smac[2],mac_tmp->smac[3],mac_tmp->smac[4],mac_tmp->smac[5]);
	}
	if(mac_tmp->dmac_mask == 1)
	{
		vty_out(vty," dmac %02x:%02x:%02x:%02x:%02x:%02x",
			mac_tmp->dmac[0],mac_tmp->dmac[1],mac_tmp->dmac[2],mac_tmp->dmac[3],mac_tmp->dmac[4],mac_tmp->dmac[5]);
	}
	if(mac_tmp->untag_mask == 1)
	{
		vty_out(vty," %s","untag");
	}
	if(mac_tmp->vlan != ACl_DEF_SVLANID)
	{
		vty_out(vty," vlan %d",mac_tmp->vlan);
	}
	if(mac_tmp->cos != ACL_DEF_COS)
	{
		vty_out(vty," cos %d",mac_tmp->cos);
	}
	if(mac_tmp->cvlan != ACl_DEF_CVLANID)
	{
		vty_out(vty," cvlan %d",mac_tmp->cvlan);
	}
	if(mac_tmp->cvlan_cos != ACL_DEF_CLAN_COS)
	{
		vty_out(vty," cvlan-cos %d",mac_tmp->cvlan_cos);
	}
	if(mac_tmp->offset_mask == 1)
	{
		vty_out(vty," offset %d 0x%x",mac_tmp->offset_len,mac_tmp->offset_info);
	}

	if(mac_tmp->proto_mask == 1)
	{
		vty_out(vty," protocol 0x%02x",mac_tmp->protocol);
	}
	if(mac_tmp->sip_mask == 1)
	{
		sip.prefix.s_addr = htonl(mac_tmp->sip);
		inet_ntop ( AF_INET, ( void * ) &sip.prefix, sip_addr, 100 );
		vty_out(vty," sip %s/%d",sip_addr,mac_tmp->sip_masklen);
	}
	if(mac_tmp->dip_mask == 1)
	{
		dip.prefix.s_addr = htonl(mac_tmp->dip);
		inet_ntop ( AF_INET, ( void * ) &dip.prefix, dip_addr, 100 );
		vty_out(vty," dip %s/%d",dip_addr,mac_tmp->dip_masklen);
	}
	if(mac_tmp->vpn_mask == 1)
	{
		vty_out(vty," l3vpn %d",mac_tmp->vpnid);
	}
	if(mac_tmp->dscp != ACL_DEF_DSCP)
	{
		vty_out(vty," dscp %d",mac_tmp->dscp);
	}
	if(mac_tmp->tos != ACL_DEF_TOS)
	{
		vty_out(vty," tos %d",mac_tmp->tos);
	}
	if(mac_tmp->ttl_mask == 1)
	{
		vty_out(vty," ttl %d",mac_tmp->ttl);
	}

	vty_out(vty,"%s",VTY_NEWLINE);

}

static void acl_rule_ip_show ( struct vty *vty, struct ip_rule *rule_tmp)
{
	struct prefix_ipv4 ipv4;
	char ipv4_addr[16];

	vty_out(vty," rule %d ip",rule_tmp->ruleid );
	if(rule_tmp->ip_mask == 1)
	{
		if(rule_tmp->ip == 0)
		{
			vty_out(vty, " %s any",rule_tmp->ip_flag ? "dip" : "sip");
		}
		else
		{
			ipv4.prefix.s_addr = htonl(rule_tmp->ip);
			inet_ntop ( AF_INET, ( void * ) &ipv4.prefix, ipv4_addr, 100 );
			vty_out(vty, " %s %s/%d",rule_tmp->ip_flag ? "dip" : "sip",ipv4_addr,rule_tmp->ip_masklen);
		}
	}
	if(rule_tmp->vpn_mask == 1)
	{
		vty_out(vty, " l3vpn %d",rule_tmp->vpnid);
	}

	vty_out(vty,"%s",VTY_NEWLINE);
}

static void acl_rule_ipex_show ( struct vty *vty, struct ip_extend_rule *ip_extend_tmp)
{
    struct prefix_ipv4 dip;
    struct prefix_ipv4 sip;
	char dip_addr[16];
	char sip_addr[16];

	vty_out(vty," rule %d ip-extended",ip_extend_tmp->ruleid );
	if(ip_extend_tmp->proto_mask == 0)
	{
		vty_out(vty," protocol any");
	}
	else
	{
		if((ip_extend_tmp->protocol != 1)&&(ip_extend_tmp->protocol != 2)
			&&(ip_extend_tmp->protocol != 4)&&(ip_extend_tmp->protocol != 6)
			&&(ip_extend_tmp->protocol != 17))
		{
			vty_out(vty," protocol 0x%02x",ip_extend_tmp->protocol);
		}
		else
			vty_out(vty," protocol %s",proto_type_parse_str(ip_extend_tmp->protocol));

		if((ip_extend_tmp->protocol == 6)||(ip_extend_tmp->protocol == 17))
		{
			if(ip_extend_tmp->sport_mask == 1)
				vty_out(vty," sport %d",ip_extend_tmp->sport);
			if(ip_extend_tmp->dport_mask == 1)
				vty_out(vty," dport %d",ip_extend_tmp->dport);
		}
	}
	if(ip_extend_tmp->sip_mask == 1)
	{
		sip.prefix.s_addr = htonl(ip_extend_tmp->sip);
		inet_ntop ( AF_INET, ( void * ) &sip.prefix, sip_addr, 100 );
		vty_out(vty," sip %s/%d",sip_addr,ip_extend_tmp->sip_masklen);
	}
	if(ip_extend_tmp->dip_mask == 1)
	{
		dip.prefix.s_addr = htonl(ip_extend_tmp->dip);
		inet_ntop ( AF_INET, ( void * ) &dip.prefix, dip_addr, 100 );
		vty_out(vty," dip %s/%d",dip_addr,ip_extend_tmp->dip_masklen);
	}
	if(ip_extend_tmp->vpn_mask == 1)
	{
		vty_out(vty," l3vpn %d",ip_extend_tmp->vpnid);
	}
	if(ip_extend_tmp->dscp != ACL_DEF_DSCP)
	{
		vty_out(vty," dscp %d",ip_extend_tmp->dscp);
	}
	if(ip_extend_tmp->tos != ACL_DEF_TOS)
	{
		vty_out(vty," tos %d",ip_extend_tmp->tos);
	}
	if(ip_extend_tmp->ttl_mask == 1)
	{
		vty_out(vty," ttl %d",ip_extend_tmp->ttl);
	}
	if((ip_extend_tmp->proto_mask == 1)&&(ip_extend_tmp->protocol == 6)&&(ip_extend_tmp->tcp_type != 0))
	{
		vty_out(vty," tcp-flag");
		tcp_type_parse_str(ip_extend_tmp->tcp_type,vty);
	}
	vty_out (vty, "%s", VTY_NEWLINE);

}


static int acl_config_write ( struct vty *vty )
{
    struct acl_group *acl_node = NULL;
	struct listnode *node = NULL;
	struct rule_node rule_node;
    struct hash_bucket *pbucket = NULL;
    int cursor;

    HASH_BUCKET_LOOP ( pbucket, cursor, acl_group_table)
    {
    	if(NULL != pbucket->data)
    	{
    		acl_node = pbucket->data;
	      	vty_out(vty,"acl %d%s",acl_node->acl_num ,VTY_NEWLINE);

			if ( acl_node->name[0] != '\0' )
			{
				vty_out(vty," name %s%s",acl_node->name,VTY_NEWLINE);
			}
			if(acl_node->rulelist.head != NULL)
		   	{
			   	for (node = acl_node->rulelist.head ; node; node = node->next)
			   	{
					memcpy(&rule_node, listgetdata (node), sizeof(rule_node));

					if((rule_node.type == ACL_TYPE_MAC)||(rule_node.type == ACL_TYPE_MAC_IP_MIX))
					{
						acl_rule_mac_show ( vty, &rule_node.rule.mac_acl, rule_node.type);
					}
					if(rule_node.type == ACL_TYPE_IP)
					{
						acl_rule_ip_show ( vty, &rule_node.rule.ip_acl);
					}
					if(rule_node.type == ACL_TYPE_IP_EXTEND)
					{
						acl_rule_ipex_show ( vty, &rule_node.rule.ipex_acl);
					}
			   	}
		    }
		}
    }

	return ERRNO_SUCCESS;
}


struct cmd_node acl_node =
{
  ACL_NODE,
  "%s(config-acl)# ",
  1,
};


DEFUN(acl_num,
		acl_num_cmd,
		"acl <1-65535>",
		"Acl rule\n"
		"Acl num\n")
{
	uint8_t ret = 0;
	uint32_t acl_num = 0;

	acl_num =(uint32_t) atoi(argv[0]);

	ret = acl_group_add(acl_num);
	if(ret)
	{
		vty_error_out(vty, "please check out %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	vty->index = (void *)acl_num;
	vty->node = ACL_NODE;

	return CMD_SUCCESS;
}

DEFUN(no_acl_num,
		no_acl_num_cmd,
		"no acl <1-65535>",
		NO_STR
		"Acl rule\n"
		"Acl num\n")
{
	uint8_t ret = 0;
	uint32_t acl_num = 0;

	acl_num =(uint32_t) atoi(argv[0]);

	ret = acl_group_delete(acl_num);
	if(ret)
	{
		vty_error_out(vty, "cannot find this acl group ,please check out%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}



DEFUN(acl_name,
		acl_name_cmd,
		"name NAME",
		"Name\n"
		"Acl name\n")
{
	struct acl_group *acl_node = NULL;

	acl_node = acl_group_lookup((uint32_t)(vty->index));
	if(acl_node == NULL)
	{
		vty_error_out(vty, "can not find this acl_group%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	memcpy(&acl_node->name,argv[0], sizeof(uchar)* NAME_STRING_LEN );

	return CMD_SUCCESS;
}


DEFUN(no_acl_name,
		no_acl_name_cmd,
		"no name",
		NO_STR
		"Acl name\n")
{
	struct acl_group *acl_node = NULL;

	acl_node = acl_group_lookup( ( uint32_t )vty->index);
	if(acl_node == NULL)
	{
		vty_error_out(vty, "can not find this acl_group%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if (acl_node->name[0] != '\0' )
	{
		memset(acl_node->name, 0, sizeof(uchar)* NAME_STRING_LEN );
	}
	else
	{
		vty_error_out(vty, "this acl group has no name%s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}


DEFUN(acl_ip_rule,
	acl_ip_rule_cmd,
	"rule <1-65535> ip (sip|dip) (A.B.C.D/M|any) [l3vpn <1-128>]",
	"Acl rule\n"
	"Acl rule number\n"
	"Acl rule for ip\n"
	"Sip\n"
	"Dip\n"
	"Ip address:A.B.C.D/M\n"
	"Ip address:any\n"
	"l3vpn \n"
	"l3vpn Vpnid format<1-128>\n")
{
	uint8_t ret = 0;
	uint32_t acl_num = 0;
	struct ip_rule rule_tmp;
	struct prefix_ipv4 ipv4;

	acl_num = ( uint32_t )vty->index;
	memset(&rule_tmp, 0, sizeof(struct ip_rule));

	rule_tmp.ruleid = atoi(argv[0]);

	if(!strncmp(argv[1], "sip", 1))
	{
		rule_tmp.ip_mask = 1;
		rule_tmp.ip_flag = 0;
	}
	else if(!strncmp(argv[1], "dip", 1))
	{
		rule_tmp.ip_mask = 1;
		rule_tmp.ip_flag = 1;
	}

	if(!strncmp(argv[2], "any", 2))
	{
		rule_tmp.ip = 0;
		rule_tmp.ip_masklen = 0;
	}
	else
	{
		VTY_GET_IPV4_PREFIX("ip-address", ipv4, argv[2]);
		rule_tmp.ip = ntohl(ipv4.prefix.s_addr);
		rule_tmp.ip_masklen = ipv4.prefixlen;
	}

	if(NULL != argv[3])
	{
		rule_tmp.vpnid  = atoi(argv[3]);
		rule_tmp.vpn_mask = 1;
	}

	ret = acl_rule_add((struct ip_rule *) &rule_tmp, ACL_TYPE_IP, acl_num);
	if(ret)
	{
		vty_error_out(vty, "acl rule add fail %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}


DEFUN(no_acl_ip_rule,
	no_acl_ip_rule_cmd,
	"no rule <1-65535>",
	NO_STR
	"Acl ip rule\n"
	"Acl rule\n")
{

	uint8_t ret = 0;
	uint16_t ruleid = 0;
	uint32_t acl_num = 0;

	acl_num =  ( uint32_t )vty->index;

	if(NULL != argv[0])
	{
		ruleid =(uint16_t)atoi(argv[0]);
	}

	ret = acl_rule_delete(ruleid ,acl_num);
	if(ret)
	{
		vty_error_out(vty, "acl rule delete fail %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}

DEFUN(acl_ip_extend_rule,
	acl_ip_extend_rule_cmd,
	"rule <1-65535> ip-extended protocol (HH|icmp|igmp|ip|any) {sip A.B.C.D/M|dip A.B.C.D/M|l3vpn <1-128>|dscp <0-63>|tos <0-7>|ttl <1-255>}",
	"Acl rule\n"
	"Acl rule number\n"
	"Acl rule for ip-extended\n"
	"Acl ip-extended protocal\n"
	"Acl ip-extended protocal: <0x00-0xff>\n"
	"Acl ip-extended protocal: icmp\n"
	"Acl ip-extended protocal: igmp\n"
	"Acl ip-extended protocal: ip\n"
	"Acl ip-extended protocal: any protocal\n"
	"Acl ip-extended source ip\n"
	"Acl ip-extended source ip:A.B.C.D/M\n"
	"Acl ip-extended destination ip\n"
	"Acl ip-extended destination ip:A.B.C.D/M\n"
	"Acl ip-extended l3vpn\n"
	"Acl ip-extended l3vpn vpnid format<1-128>\n"
	"Acl ip-extended dscp\n"
	"Acl ip-extended dscp in the range from 0 to 63\n"
	"Acl ip-extended tos\n"
	"Acl ip-extended tos in the range from 0 to 7\n"
	"Acl ip-extended ttl\n"
	"Acl ip-extended ttl in the range from 1 to 255\n")
{
	uint8_t ret = 0;
	uint32_t acl_num = 0;
	struct prefix_ipv4 ipv4;
	struct prefix_ipv4 p;
	struct ip_extend_rule ip_extend_tmp;

	acl_num =  ( uint32_t )vty->index;
	memset(&ip_extend_tmp, 0, sizeof(struct ip_extend_rule));
	ip_extend_tmp.dscp = ACL_DEF_DSCP;
	ip_extend_tmp.tos = ACL_DEF_TOS;

	ip_extend_tmp.ruleid = atoi(argv[0]);
	/*IP协议号中规定: 1-----icmp;2------igmp;4------ip*/
	if(!strncmp(argv[1], "icmp", 2))
	{
		ip_extend_tmp.protocol = 1;
		ip_extend_tmp.proto_mask = 1;
	}
	else if(!strncmp(argv[1], "igmp", 2))
	{
		ip_extend_tmp.protocol = 2;
		ip_extend_tmp.proto_mask = 1;
	}
	else if(!strncmp(argv[1], "ip", 2))
	{
		ip_extend_tmp.protocol = 4;
		ip_extend_tmp.proto_mask = 1;
	}
	else if(!strncmp(argv[1], "any", 1))
	{
		ip_extend_tmp.proto_mask = 0;
	}
	else
	{
		ACL_GET_INTEGER_RANGE_HEX ( (char *)"protocol",&ip_extend_tmp.protocol, (char *)argv[1],2,2, ACL_IPEX_PROTOCOL_MAX );
		ip_extend_tmp.proto_mask = 1;
	}

	if(NULL != argv[2])
	{
		VTY_GET_IPV4_PREFIX("sip-address", ipv4, argv[2]);
		ip_extend_tmp.sip = ntohl(ipv4.prefix.s_addr);
		ip_extend_tmp.sip_masklen = ipv4.prefixlen;
		ip_extend_tmp.sip_mask = 1;
	}
	if(NULL != argv[3])
	{
		VTY_GET_IPV4_PREFIX("dip-address", p, argv[3]);
		ip_extend_tmp.dip = ntohl(p.prefix.s_addr);
		ip_extend_tmp.dip_masklen = p.prefixlen;
		ip_extend_tmp.dip_mask = 1;
	}
	if(NULL != argv[4])
	{
		ip_extend_tmp.vpnid = atoi(argv[4]);
		ip_extend_tmp.vpn_mask = 1;
	}
	if(NULL != argv[5])
	{
		ip_extend_tmp.dscp = atoi(argv[5]);
	}
	if(NULL != argv[6])
	{
		ip_extend_tmp.tos = atoi(argv[6]);
	}
	if(NULL != argv[7])
	{
		ip_extend_tmp.ttl = atoi(argv[7]);
		ip_extend_tmp.ttl_mask = 1;
	}

	ret = acl_rule_add((struct ip_extend_rule* )&ip_extend_tmp, ACL_TYPE_IP_EXTEND, acl_num);
	if(ret)
	{
		vty_error_out(vty, "acl rule add fail %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}


DEFUN(acl_ip_extend_2_rule,
	acl_ip_extend_2_rule_cmd,
	"rule <1-65535> ip-extended protocol (udp|tcp) {sport <1-65535>|dport <1-65535>|sip A.B.C.D/M|dip A.B.C.D/M|l3vpn <1-128>|dscp <0-63>|tos <0-7>|ttl <1-255>|tcp-flag |fin|syn|rst|psh|ack|urg}",
	"Acl rule\n"
	"Acl rule number\n"
	"Acl rule for ip-extended\n"
	"Acl ip-extended protocal\n"
	"Acl ip-extended protocal: udp\n"
	"Acl ip-extended protocal: tcp\n"
	"Source port\n"
	"Source port in the range from 1 to 65535\n"
	"Destination port\n"
	"Destination port in the range from 1 to 65535\n"
	"Acl ip-extended source ip\n"
	"Acl ip-extended source ip : A.B.C.D/M\n"
	"Acl ip-extended destination ip\n"
	"Acl ip-extended destination ip: A.B.C.D/M\n"
	"Acl ip-extended l3vpn\n"
	"Acl ip-extended l3vpn vpnid format<1-128>\n"
	"Acl ip-extended dscp\n"
	"Acl ip-extended dscp in the range from 0 to 63\n"
	"Acl ip-extended tos\n"
	"Acl ip-extended tos in the range from 0 to 7\n"
	"Acl ip-extended ttl\n"
	"Acl ip-extended ttl in the range from 1 to 255\n"
	"Acl ip-extended tcp type\n"
	"Acl ip-extended tcp type: fin (only when protocal is tcp)\n"
	"Acl ip-extended tcp type: syn (only when protocal is tcp)\n"
	"Acl ip-extended tcp type: rst (only when protocal is tcp)\n"
	"Acl ip-extended tcp type: psh (only when protocal is tcp)\n"
	"Acl ip-extended tcp type: ack (only when protocal is tcp)\n"
	"Acl ip-extended tcp type: urg (only when protocal is tcp)\n")
{
	uint8_t ret = 0;
	uint32_t acl_num = 0;
	struct prefix_ipv4 ipv4;
	struct prefix_ipv4 p;
	struct ip_extend_rule ip_extend_tmp;

	acl_num =  ( uint32_t )vty->index;

	memset(&ip_extend_tmp, 0, sizeof(struct ip_extend_rule));
	ip_extend_tmp.dscp = ACL_DEF_DSCP;
	ip_extend_tmp.tos = ACL_DEF_TOS;

	ip_extend_tmp.ruleid = atoi(argv[0]);
	/*protocol: 6-----tcp;17------udp*/
	if(!strncmp(argv[1], "tcp", 1))
	{
		ip_extend_tmp.protocol = 6;
		ip_extend_tmp.proto_mask = 1;
	}
	else if(!strncmp(argv[1], "udp", 1))
	{
		ip_extend_tmp.protocol = 17;
		ip_extend_tmp.proto_mask = 1;
	}
	if(NULL != argv[2])
	{
		ip_extend_tmp.sport = atoi(argv[2]);
		ip_extend_tmp.sport_mask = 1;
	}
	if(NULL != argv[3])
	{
		ip_extend_tmp.dport = atoi(argv[3]);
		ip_extend_tmp.dport_mask = 1;
	}

	if(NULL != argv[4])
	{
		VTY_GET_IPV4_PREFIX("sip-address", ipv4, argv[4]);
		ip_extend_tmp.sip = ntohl(ipv4.prefix.s_addr);
		ip_extend_tmp.sip_masklen = ipv4.prefixlen;
		ip_extend_tmp.sip_mask = 1;
	}
	if(NULL != argv[5])
	{
		VTY_GET_IPV4_PREFIX("dip-address", p, argv[5]);
		ip_extend_tmp.dip = ntohl(p.prefix.s_addr);
		ip_extend_tmp.dip_masklen = p.prefixlen;
		ip_extend_tmp.dip_mask = 1;
	}
	if(NULL != argv[6])
	{
		ip_extend_tmp.vpnid = atoi(argv[6]);
		ip_extend_tmp.vpn_mask = 1;
	}
	if(NULL != argv[7])
	{
		ip_extend_tmp.dscp = atoi(argv[7]);
	}
	if(NULL != argv[8])
	{
		ip_extend_tmp.tos = atoi(argv[8]);
	}
	if(NULL != argv[9])
	{
		ip_extend_tmp.ttl = atoi(argv[9]);
		ip_extend_tmp.ttl_mask = 1;
	}
	if(ip_extend_tmp.protocol == 6)
	{
		if(NULL != argv[10])
		{
			if(NULL != argv[11])
			{
				ip_extend_tmp.tcp_type += ACl_TCP_FLAG_FIN;
			}
			if(NULL != argv[12])
			{
				ip_extend_tmp.tcp_type += ACl_TCP_FLAG_SYN;
			}
			if(NULL != argv[13])
			{
				ip_extend_tmp.tcp_type += ACl_TCP_FLAG_RST;
			}
			if(NULL != argv[14])
			{
				ip_extend_tmp.tcp_type += ACl_TCP_FLAG_PSH;
			}
			if(NULL != argv[15])
			{
				ip_extend_tmp.tcp_type += ACl_TCP_FLAG_ACK;
			}
			if(NULL != argv[16])
			{
				ip_extend_tmp.tcp_type += ACl_TCP_FLAG_URG;
			}
		}
	}
	else
	{
		if((NULL != argv[10])||(NULL != argv[11])||(NULL != argv[12])
			||(NULL != argv[13])||(NULL != argv[14])||(NULL != argv[15])||(NULL != argv[16]))
		{
			vty_error_out(vty, "tcp-type flags fin/syn/rst/psh/ack/urg only set when protocal is tcp.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}

	ret = acl_rule_add((struct ip_extend_rule* )&ip_extend_tmp, ACL_TYPE_IP_EXTEND, acl_num);
	if(ret)
	{
		vty_error_out(vty, "acl rule add fail %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	return CMD_SUCCESS;
}



DEFUN(acl_mac_rule,
	acl_mac_rule_cmd,
	"rule <1-65535> mac ethtype (HHHH|any) {smac XX:XX:XX:XX:XX:XX|dmac XX:XX:XX:XX:XX:XX|untag|vlan <1-4095>|cos <0-7>|cvlan <1-4095>|cvlan-cos <0-7>|offset <0-3> STRING}",
	"Acl rule\n"
	"Acl rule number\n"
	"Acl rule for mac\n"
	"Ethernet type\n"
	"Ethernet type in the range <0x0600-0xffff>\n"
	"Ethernet type any\n"
	"Source mac address\n"
	"Source mac address:XX:XX:XX:XX:XX:XX\n"
	"Destination mac address\n"
	"Destination mac address:XX:XX:XX:XX:XX:XX\n"
	"Untag \n"
	"Vlan id\n"
	"Vlan id in the range from 1 to 4095\n"
	"Cos id\n"
	"Cos id in the range from 0 to 7\n"
	"Cvlan id\n"
	"Cvlan id in the range from 1 to 4095\n"
	"Cvlan-cos id\n"
	"Cvlan-cos id in the range from 0 to 7\n"
	"Offset: After ethernet header, custom matching message content\n"
	"Offset: Ethernet header offset 0 to 3 bytes\n"
	"Offset_info:Ethernet head offset and offset content not exceeding 4 bytes format <0x0-0xffffffff>\n")
{
	struct mac_rule rule_tmp;
	uint8_t ret = 0;
	uint8_t max_len = 0;
	uint32_t acl_num = 0;

	acl_num =  ( uint32_t )vty->index;
	memset(&rule_tmp, 0, sizeof(struct mac_rule));
	rule_tmp.vlan = ACl_DEF_SVLANID;
	rule_tmp.cvlan = ACl_DEF_CVLANID;
	rule_tmp.cos = ACL_DEF_COS;
	rule_tmp.cvlan_cos = ACL_DEF_CLAN_COS;
	rule_tmp.offset_len = ACL_DEF_OFFSET;
	rule_tmp.dscp = ACL_DEF_DSCP;
	rule_tmp.tos = ACL_DEF_TOS;

	rule_tmp.ruleid = atoi(argv[0]);
	if(!strncmp(argv[1], "any", 2))
	{
		rule_tmp.ethtype_mask = 1;
		rule_tmp.ethtype = 0;
	}
	else
	{
		ACL_GET_INTEGER_RANGE_HEX ( (char *)"ethtype",&rule_tmp.ethtype, (char *)argv[1],4,4,ACL_MAC_ETHTYPE_MAX );

		if(rule_tmp.ethtype < 0x0600)
		{
			vty_error_out (vty, "Ethtype value should be more than 0x600 %s", VTY_NEWLINE);
			return CMD_WARNING;
		}

		rule_tmp.ethtype_mask = 1;
	}
	if(NULL != argv[2])
	{
		ether_string_to_mac((char *)argv[2], (uchar *)rule_tmp.smac);
		rule_tmp.smac_mask = 1;
	}
	if(NULL != argv[3])
	{
		ether_string_to_mac((char *)argv[3], (uchar *)rule_tmp.dmac);
		rule_tmp.dmac_mask = 1;
	}
	if((NULL != argv[4])&&(NULL == argv[5])&&(NULL == argv[6])&&(NULL == argv[7])&&(NULL == argv[8])&&(NULL == argv[9]))
	{
		rule_tmp.untag_mask = 1;
	}
	else if ((NULL == argv[4])&&((NULL != argv[5])||(NULL != argv[6])||(NULL != argv[7])||(NULL != argv[8])||(NULL != argv[9])))
	{
		if(NULL != argv[5])
		{
			rule_tmp.vlan = atoi(argv[5]);
		}
		if(NULL != argv[6])
		{
			rule_tmp.cos = atoi(argv[6]);
		}
		if(NULL != argv[7])
		{
			rule_tmp.cvlan = atoi(argv[7]);
		}
		if(NULL != argv[8])
		{
			rule_tmp.cvlan_cos = atoi(argv[8]);
		}
		if(NULL != argv[9])
		{
			rule_tmp.offset_mask = 1;
			rule_tmp.offset_len = atoi(argv[9]);
			max_len = (4 - rule_tmp.offset_len)*2;
			ACL_GET_INTEGER_RANGE_HEX ( (char *)"offset_info",&rule_tmp.offset_info , (char *)argv[10],max_len,0, ACL_MAC_OFFSET_MAX );

		}
	}
	else if ((NULL != argv[4])&&((NULL != argv[5])||(NULL != argv[6])||(NULL != argv[7])||(NULL != argv[8])))
	{
		vty_error_out ( vty, "please configure untag or tag(included vlan/cos/cvlan/cvlan_cos)! %s", VTY_NEWLINE );
		return CMD_WARNING;
	}
	else if ((NULL != argv[4])&&(NULL != argv[9]))
	{
		vty_error_out ( vty, "configure info untag conflict with offset! %s", VTY_NEWLINE );
		return CMD_WARNING;
	}

	ret = acl_rule_add((struct mac_rule* )&rule_tmp ,ACL_TYPE_MAC, acl_num);
	if(ret)
	{
		vty_error_out(vty, "acl rule add error %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}


DEFUN(acl_mix_rule,
	acl_mix_rule_cmd,
	"rule <1-65535> mix {ethtype HHHH|smac XX:XX:XX:XX:XX:XX|dmac XX:XX:XX:XX:XX:XX | "
	"vlan <1-4095>|cos <0-7>|cvlan <1-4095>|cvlan-cos <0-7>|"
	"protocol HH|sip A.B.C.D/M|dip A.B.C.D/M|l3vpn <1-128>|dscp <0-63>|tos <0-7>|ttl <1-255>}",
	"Acl rule\n"
	"Acl rule number\n"
	"Acl mac-ip mix rule\n"
	"Ethernet type\n"
	"Ethernet type in the range <0x0600-0xffff>\n"
	"Source mac address\n"
	"Source mac address:XX:XX:XX:XX:XX:XX\n"
	"Destination mac address\n"
	"Destination mac address:XX:XX:XX:XX:XX:XX\n"
	"Vlan id\n"
	"Vlan id in the range from 1 to 4095\n"
	"Cos id\n"
	"Cos id in the range from 0 to 7\n"
	"Cvlan id\n"
	"Cvlan id in the range from 1 to 4095\n"
	"Cvlan-cos id\n"
	"Cvlan-cos id in the range from 0 to 7\n"
	"Acl ip-extended protocal\n"
	"Acl ip-extended protocal: <0x00-0xff>\n"
	"Acl ip-extended source ip\n"
	"Acl ip-extended source ip:A.B.C.D/M\n"
	"Acl ip-extended destination ip\n"
	"Acl ip-extended destination ip:A.B.C.D/M\n"
	"Acl ip-extended l3vpn\n"
	"Acl ip-extended l3vpn vpnid format<1-128>\n"
	"Acl ip-extended dscp\n"
	"Acl ip-extended dscp in the range from 0 to 63\n"
	"Acl ip-extended tos\n"
	"Acl ip-extended tos in the range from 0 to 7\n"
	"Acl ip-extended ttl\n"
	"Acl ip-extended ttl in the range from 1 to 255\n")
{
	struct mac_rule rule_tmp;
	uint8_t ret = 0;
	uint32_t acl_num = 0;
	struct prefix_ipv4 ipv4;
	struct prefix_ipv4 p;
	int flag = 0;

	for(int i = 1;i<argc; i++)
	{
		if(argv[i] != NULL)
		{
			flag = 1;
			break;
		}
	}

	if(flag == 0)
	{
		vty_error_out(vty, "Please set the matching rule%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	acl_num =  ( uint32_t )vty->index;
	memset(&rule_tmp, 0, sizeof(struct mac_rule));
	rule_tmp.vlan = ACl_DEF_SVLANID;
	rule_tmp.cvlan = ACl_DEF_CVLANID;
	rule_tmp.cos = ACL_DEF_COS;
	rule_tmp.cvlan_cos = ACL_DEF_CLAN_COS;
	rule_tmp.offset_len = ACL_DEF_OFFSET;
	rule_tmp.dscp = ACL_DEF_DSCP;
	rule_tmp.tos = ACL_DEF_TOS;

	rule_tmp.ruleid = atoi(argv[0]);

	if(NULL != argv[1])
	{
		ACL_GET_INTEGER_RANGE_HEX ( (char *)"ethtype",&rule_tmp.ethtype, (char *)argv[1],4,4,ACL_MAC_ETHTYPE_MAX );

		if(rule_tmp.ethtype < 0x0600)
		{
			vty_error_out (vty, "Ethtype value should be more than 0x600 %s", VTY_NEWLINE);
			return CMD_WARNING;
		}

		rule_tmp.ethtype_mask = 1;
	}
	if(NULL != argv[2])
	{
		ether_string_to_mac((char *)argv[2], (uchar *)rule_tmp.smac);
		rule_tmp.smac_mask = 1;
	}
	if(NULL != argv[3])
	{
		ether_string_to_mac((char *)argv[3], (uchar *)rule_tmp.dmac);
		rule_tmp.dmac_mask = 1;
	}

	if(NULL != argv[4])
	{
		rule_tmp.vlan = atoi(argv[4]);
	}
	if(NULL != argv[5])
	{
		rule_tmp.cos = atoi(argv[5]);
	}
	if(NULL != argv[6])
	{
		rule_tmp.cvlan = atoi(argv[6]);
	}
	if(NULL != argv[7])
	{
		rule_tmp.cvlan_cos = atoi(argv[7]);
	}

	if(NULL != argv[8])
	{
		ACL_GET_INTEGER_RANGE_HEX ( (char *)"protocol",&rule_tmp.protocol, (char *)argv[8],2,2,ACL_IPEX_PROTOCOL_MAX );
		rule_tmp.proto_mask = 1;
	}

	if(NULL != argv[9])
	{
		VTY_GET_IPV4_PREFIX("sip-address", ipv4, argv[9]);
		rule_tmp.sip = ntohl(ipv4.prefix.s_addr);
		rule_tmp.sip_masklen = ipv4.prefixlen;
		rule_tmp.sip_mask = 1;
	}
	if(NULL != argv[10])
	{
		VTY_GET_IPV4_PREFIX("dip-address", p, argv[10]);
		rule_tmp.dip = ntohl(p.prefix.s_addr);
		rule_tmp.dip_masklen = p.prefixlen;
		rule_tmp.dip_mask = 1;
	}

	if(NULL != argv[11])
	{
		rule_tmp.vpnid = atoi(argv[11]);
		rule_tmp.vpn_mask = 1;
	}
	if(NULL != argv[12])
	{
		rule_tmp.dscp = atoi(argv[12]);
	}
	if(NULL != argv[13])
	{
		rule_tmp.tos = atoi(argv[13]);
	}
	if(NULL != argv[14])
	{
		rule_tmp.ttl = atoi(argv[14]);
		rule_tmp.ttl_mask = 1;
	}

	ret = acl_rule_add((struct mac_rule* )&rule_tmp ,ACL_TYPE_MAC_IP_MIX, acl_num);
	if(ret)
	{
		vty_error_out(vty, "acl rule add error %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}



DEFUN(show_acl_info,
	show_acl_info_cmd,
	"show acl [<1-65535>]",
	"Show\n"
	"Acl rule\n"
	"Acl num\n")
{
	struct acl_group *acl_node = NULL;
	struct listnode *node = NULL;
	struct rule_node rule_node;

	if(NULL == argv[0])
	{
		acl_config_write ( vty );
		return CMD_SUCCESS;
	}

	acl_node = acl_group_lookup(atoi(argv[0]));
	if(NULL == acl_node)
	{
		vty_error_out(vty,"acl %d doesn't exist!%s",atoi(argv[0]),VTY_NEWLINE);
		return CMD_WARNING;
	}

	vty_out (vty, "=======================================================================%s", VTY_NEWLINE);

	vty_out(vty,"acl %d %s",acl_node->acl_num,VTY_NEWLINE);
	if ( acl_node->name[0] != '\0' )
	{
		vty_out(vty," name %s%s",acl_node->name,VTY_NEWLINE);
	}
	if(acl_node->rulelist.head != NULL)
   	{
	   	for (node = acl_node->rulelist.head ; node; node = node->next)
	   	{
			memcpy(&rule_node, listgetdata (node), sizeof(rule_node));

			if((rule_node.type == ACL_TYPE_MAC)||(rule_node.type == ACL_TYPE_MAC_IP_MIX))
			{
				acl_rule_mac_show ( vty, &rule_node.rule.mac_acl, rule_node.type);
			}
			if(rule_node.type == ACL_TYPE_IP)
			{
				acl_rule_ip_show ( vty, &rule_node.rule.ip_acl);
			}
			if(rule_node.type == ACL_TYPE_IP_EXTEND)
			{
				acl_rule_ipex_show ( vty, &rule_node.rule.ipex_acl);
			}
	   	}
    }

	vty_out (vty, "=======================================================================%s", VTY_NEWLINE);

	return CMD_SUCCESS;
}


DEFUN(show_acl_rule_info,
	show_acl_rule_info_cmd,
	"show acl <1-65535> rule <1-65535>",
	"Show\n"
	"Acl rule\n"
	"Acl num\n"
	"Show acl rule infomation\n"
	"Acl rule number\n")
{
	struct acl_group *acl_node = NULL;
	struct listnode *node = NULL;
	struct rule_node rule_node;
	uint16_t ruleid = 0;

	acl_node = acl_group_lookup(atoi(argv[0]));
	if(NULL == acl_node)
	{
		vty_error_out(vty,"acl %d doesn't exist!%s",atoi(argv[0]),VTY_NEWLINE);
		return CMD_WARNING;
	}

	ruleid = (uint16_t)atoi(argv[1]);
	if(acl_node->rulelist.head != NULL)
	{
   		for (node = acl_node->rulelist.head ; node; node = node->next)
   		{
			memcpy(&rule_node, listgetdata(node), sizeof(rule_node));

			if(ruleid == rule_node.rule.mac_acl.ruleid)
			{
				if((rule_node.type == ACL_TYPE_MAC)||(rule_node.type == ACL_TYPE_MAC_IP_MIX))
				{
					acl_rule_mac_show ( vty, &rule_node.rule.mac_acl, rule_node.type);
				}
				if(rule_node.type == ACL_TYPE_IP)
				{
					acl_rule_ip_show ( vty, &rule_node.rule.ip_acl);
				}
				if(rule_node.type == ACL_TYPE_IP_EXTEND)
				{
					acl_rule_ipex_show ( vty, &rule_node.rule.ipex_acl);
				}
			}
		}
	}

	return CMD_SUCCESS;
}



void acl_cli_init(void)
{
	install_node (&acl_node, acl_config_write);

	install_default (ACL_NODE);

	install_element (CONFIG_NODE, &acl_num_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &no_acl_num_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &show_acl_info_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_acl_rule_info_cmd, CMD_LOCAL);

	install_element (ACL_NODE, &acl_ip_rule_cmd, CMD_SYNC);
	install_element (ACL_NODE, &no_acl_ip_rule_cmd, CMD_SYNC);
	install_element (ACL_NODE, &acl_ip_extend_rule_cmd, CMD_SYNC);
	install_element (ACL_NODE, &acl_ip_extend_2_rule_cmd, CMD_SYNC);
	install_element (ACL_NODE, &acl_mac_rule_cmd, CMD_SYNC);
	install_element (ACL_NODE, &acl_mix_rule_cmd, CMD_SYNC);
	install_element (ACL_NODE, &acl_name_cmd, CMD_SYNC);
	install_element (ACL_NODE, &no_acl_name_cmd, CMD_SYNC);
}







