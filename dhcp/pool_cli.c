#include <string.h>
#include <stdlib.h>
#include <lib/linklist.h>
#include <lib/prefix.h>
#include <lib/vty.h>
#include <lib/ifm_common.h>
#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/command.h>
#include <lib/prefix.h>
#include <lib/inet_ip.h>
#include <lib/module_id.h>
#include <lib/msg_ipc.h>
#include <lib/memtypes.h>
#include <lib/memory.h>

#include "files.h"
#include "pool_cli.h"
#include "dhcp_if.h"
#include "pool_address.h"
#include "pool_static_address.h"

static int show_ip_pool ( struct vty *vty ,uint8_t pool_index);
static int show_ip_pool_address ( struct vty *vty ,uint8_t pool_index );
static int show_ip_pool_static_address ( struct vty *vty ,uint8_t pool_index );

//static int check_str(char * str,uint8_t max_len);

extern struct hash_table ip_pool_table;

struct cmd_node pool_node =
{
  POOL_NODE,
  "%s(config-ip-pool)# ",
  1,
};


DEFUN(pool_name,
		pool_name_cmd,
		"ip pool dhcp  <1-255>",
		"Specify IP configurations for the system\n"
		"Configure a IP pool or enter the IP pool view\n"
		"Dynamic host configure protocol\n"
		"Pool index,<1-255>\n")
{
	struct ip_pool *pool_node = NULL;
	
	pool_node = dhcp_pool_add(atoi(argv[0]));

	if(pool_node == NULL)
	{
		vty_error_out(vty, "please check out %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	vty_info_out(vty, "ip pool index, %d %s",pool_node->pool_index, VTY_NEWLINE);
	
	vty->index = (void *)pool_node;
	vty->node = POOL_NODE;	
	return CMD_SUCCESS;
}

DEFUN(no_pool_name,
		no_pool_name_cmd,
		"no ip pool dhcp <1-255>",
		NO_STR
		"Specify IP configurations for the system\n"
		"Configure a IP pool or enter the IP pool view\n"
		"Dynamic host configure protocol\n"		
		"Pool index\n")
{
	uint8_t ret = 0;
	ret = dhcp_pool_delete(atoi(argv[0]));
	if(ret)
	{
		vty_error_out(vty, "cannot find this pool group ,please check out %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	return CMD_SUCCESS;
}

DEFUN(show_pool_info,
	show_pool_info_cmd,
	"show ip pool {index <1-255>}",
	SHOW_STR
	"Specify IP configurations for the system\n"
	"Configure a IP pool or enter the IP pool view\n"	
	"index\n"
	"Pool index\n")
{
	int ret = CMD_SUCCESS;
	uint8_t pool_index = 0;

	if(argv[0])
		pool_index = (uint8_t)atoi(argv[0]);

	/*pool_node = ip_pool_lookup((uchar *)(argv[0]));
	if(NULL == pool_node)
	{
		vty_out(vty,"pool %s doesn't exist!%s",argv[0],VTY_NEWLINE);
		return CMD_WARNING;
	}*/
	ret = show_ip_pool(vty,pool_index);

	return ret;
}


DEFUN(show_pool_address_info,
	show_pool_address_info_cmd,
	"show ip pool address {index <1-255>}",
	SHOW_STR
	"Specify IP configurations for the system\n"
	"Configure a IP pool or enter the IP pool view\n"	
	"ip address\n"
	"index\n"
	"Pool index\n")
	
{
	int ret = CMD_SUCCESS;
	uint8_t pool_index=0;
	struct ip_pool *pool_node = NULL;
	
	if(argv[0])
	{
		pool_index = (uint8_t)atoi(argv[0]);

		pool_node = dhcp_pool_lookup(pool_index);
		if(NULL == pool_node)
		{
			vty_out(vty,"pool %s doesn't exist!%s",argv[0],VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	ret = show_ip_pool_address(vty,pool_index);
	ret = show_ip_pool_static_address(vty,pool_index);

	return ret;
}

DEFUN(gateway_list,
	gateway_list_cmd,
	"gateway A.B.C.D/<1-30>",
	"Configure the gateway\n"
	"Gateway's IP address/mask_len\n"
	)
{
	struct ip_pool *pool_node = NULL;
	struct prefix_ipv4 ipv4;
	u_int32 mask = 0,mask_host;
	uint32_t start;		/* Start address of leases, network order */
	uint32_t end;			/* End of leases, network order */
	uint32_t tmp;
	struct in_addr addr;
	
	pool_node = (struct ip_pool *)vty->index;

	if(pool_node->dhcp_pool.gateway)
	{
		vty_error_out(vty,"Please delete the gateway section first.%s",VTY_NEWLINE);
		return CMD_WARNING;		
	}	
	
	VTY_GET_IPV4_PREFIX("gateway", ipv4, argv[0]);

	if(ipv4.prefixlen >= 31 || ipv4.prefixlen == 0)
	{
		vty_error_out(vty,"Illegal mask value%s",VTY_NEWLINE);
		
		return CMD_WARNING;		

	}

	masklen2ip(ipv4.prefixlen,(struct in_addr *)&mask);
	if(!inet_valid_ipv4(ntohl(ipv4.prefix.s_addr)))
	{
		vty_error_out(vty,"Illegal gateway value%s",VTY_NEWLINE);		
		return CMD_WARNING;			
	}

	mask_host = ntohl(mask);

	tmp = ntohl(ipv4.prefix.s_addr)& mask_host;
	start	= htonl(tmp+1);
	end		= htonl((tmp | ~mask_host)-1);

	if(ntohl(ipv4.prefix.s_addr) < ntohl(start) || ntohl(ipv4.prefix.s_addr) > ntohl(end))
	{
		vty_error_out(vty,"Illegal gateway%s",VTY_NEWLINE);		
		return CMD_WARNING; 		
	}
			
	
	if(pool_node->dhcp_pool.start)
	{
		if(ntohl(pool_node ->dhcp_pool.start) < ntohl(start) 
			|| ntohl(pool_node ->dhcp_pool.start) > ntohl(end)
			||ntohl(pool_node ->dhcp_pool.end) > ntohl(end))
		{		
			vty_error_out(vty,"The address section should be within the subnet of the gate.%s",VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	
	pool_node ->dhcp_pool.gateway  = ipv4.prefix.s_addr;
	pool_node ->dhcp_pool.mask_len = ipv4.prefixlen ;
	pool_node ->dhcp_pool.mask = mask;	
	pool_node ->dhcp_pool.start_from_gate =	start;
	pool_node ->dhcp_pool.end_from_gate = end;

	
	//return CMD_SUCCESS;
	addr.s_addr = pool_node->dhcp_pool.gateway;
	vty_info_out(vty,"gateway is %s ,mask_len is %d%s",inet_ntoa(addr),pool_node->dhcp_pool.mask_len,VTY_NEWLINE);

	
	return CMD_SUCCESS;

}

DEFUN(no_gateway_list,
	no_gateway_list_cmd,
	//"no gateway A.B.C.D/<1-32>",	
	"no gateway",		
	NO_STR
	"Configure the gateway\n"
	//"Gateway's IP address/mask_len\n"
	)
{
	struct ip_pool *pool_node = NULL;

	pool_node = (struct ip_pool *)vty->index;

	if(!pool_node->dhcp_pool.gateway)
	{
		vty_error_out(vty,"The gateway does not exist.%s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	pool_node ->dhcp_pool.gateway = 0;
	pool_node ->dhcp_pool.mask_len = 0;
	
	vty_info_out(vty,"The gateway has been deleted %s",VTY_NEWLINE);

	
	return CMD_SUCCESS;

}

DEFUN(dns_list,
	dns_list_cmd,
	"dns-server A.B.C.D",
	"Configure DNS servers\n"
	"IP address\n")
{
	struct ip_pool *pool_node = NULL;
	struct prefix_ipv4 ipv4;
	struct in_addr inaddr;


	VTY_GET_IPV4_PREFIX("dns-server", ipv4, argv[0]);

	if(!inet_valid_ipv4(ntohl(ipv4.prefix.s_addr)))
	{
		vty_error_out(vty,"Invalid dns-server value%s",VTY_NEWLINE);
		return CMD_WARNING;			
	}

	pool_node = (struct ip_pool *)vty->index;
	
	if(pool_node ->dhcp_pool.dns_server  == ipv4.prefix.s_addr)
	{
		vty_info_out(vty,"Part of the domain-name-server IP has already exist.%s",VTY_NEWLINE);		
		return CMD_WARNING;
	}

	pool_node ->dhcp_pool.dns_server  = ipv4.prefix.s_addr;

	
	inaddr.s_addr		= pool_node ->dhcp_pool.dns_server;
	vty_info_out(vty,"dns-server is %s %s",inet_ntoa(inaddr),VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN(no_dns_list,
	no_dns_list_cmd,
	"no dns-server A.B.C.D",
	NO_STR
	"Delete primary and secondary DNS's IP address\n"
	"IP address\n")
{
	struct ip_pool *pool_node = NULL;
	struct prefix_ipv4 ipv4;

	pool_node = (struct ip_pool *)vty->index;
	
	VTY_GET_IPV4_PREFIX("dns-server", ipv4, argv[0]);
	if(pool_node ->dhcp_pool.dns_server  != ipv4.prefix.s_addr)
	{
		vty_error_out(vty,"The dns does not exist.%s",VTY_NEWLINE);
		return CMD_WARNING;		
	}

	pool_node ->dhcp_pool.dns_server = 0;

	vty_info_out(vty,"The dns has been deleted %s",VTY_NEWLINE);

	return CMD_SUCCESS;
}


DEFUN(pool_network,
	address_cmd,
	"address A.B.C.D <1-65535>",
	"Add a network\n"
	"Start ip address\n"
	"Ip number")
{
	struct ip_pool *pool_node = NULL;
	struct prefix_ipv4 ipv4;
    struct hash_bucket *pbucket = NULL;
    int cursor;	
	struct ip_pool *pool_node_new = NULL;
	uint32_t start;		
	uint32_t end;	
	uint32_t total;	
	uint16_t m;	
	uint16_t n;
	struct in_addr inaddr;

	VTY_GET_IPV4_PREFIX("address", ipv4, argv[0]);
	
	if(!inet_valid_ipv4(ntohl(ipv4.prefix.s_addr)))
	{
		vty_error_out(vty,"Invalid start address value%s",VTY_NEWLINE);
		return CMD_WARNING;			
	}

	pool_node = (struct ip_pool *)vty->index;

	if(pool_node ->dhcp_pool.start)
	{
		vty_error_out(vty,"Please delete the address section first.%s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	start = ipv4.prefix.s_addr;
	end = htonl(ntohl(ipv4.prefix.s_addr)+atoi(argv[1])-1);
	total = atoi(argv[1]) ;

	if(!start || !total)
	{
		vty_error_out(vty,"Illegal value%s",VTY_NEWLINE);
		
		return CMD_WARNING;		
	}

	if(pool_node ->dhcp_pool.gateway)
	{
		if(ntohl(start) < ntohl(pool_node ->dhcp_pool.start_from_gate) 
			|| ntohl(start) > ntohl(pool_node ->dhcp_pool.end_from_gate)
			||ntohl(end) > ntohl(pool_node ->dhcp_pool.end_from_gate))
		{
		
			vty_error_out(vty,"The address section should be within the subnet of the gate.%s",VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	
	HASH_BUCKET_LOOP(pbucket,cursor,ip_pool_table)
    {
        pool_node_new = pbucket->data;
		if((pool_node ->pool_index != pool_node_new->pool_index) && 
			(((ntohl(start) >= ntohl(pool_node_new->dhcp_pool.start)) &&
			(ntohl(start) <= ntohl(pool_node_new->dhcp_pool.end)))  ||
			((ntohl(end) >= ntohl(pool_node_new->dhcp_pool.start)) &&
			(ntohl(end) <= ntohl(pool_node_new->dhcp_pool.end))))	)
		{
		
			vty_error_out(vty,"Repeated address section.%s",VTY_NEWLINE);
			return CMD_WARNING;		
		}
    }

	pool_node ->dhcp_pool.start =  start;
	pool_node ->dhcp_pool.end   =  end;
	pool_node ->dhcp_pool.total =  total ;

	m = total/8;
	n = total%8;
	
	if(n)
	{
		m++;
	}
	if(pool_node ->dhcp_pool.indexd == NULL)
	{		
		pool_node ->dhcp_pool.indexd =(uint8_t *)xmalloc(m);
		memset(pool_node ->dhcp_pool.indexd, 0xff, m);
	}

	if(pool_node ->dhcp_pool.indexs == NULL)
	{		
		pool_node ->dhcp_pool.indexs =(uint8_t *)xmalloc(m);
		memset(pool_node ->dhcp_pool.indexs, 0xff, m);
	}

	
	dhcp_read_leases(pool_node);
	
	/*if(pool_node ->dhcp_pool.leases == NULL)
	{
		pool_node ->dhcp_pool.leases = xcalloc(pool_node ->dhcp_pool.total, sizeof(struct dhcpOfferedAddr));
		read_leases(pool_node);
	}
	else
		LOG(LOG_ERR,"pool name[%s] ,pool_node ->dhcp_pool.leases != NULL\n",pool_node ->name);*/

	inaddr.s_addr		= pool_node ->dhcp_pool.start;
	vty_info_out(vty,"start address %s, ",inet_ntoa(inaddr));	
	
	inaddr.s_addr		= pool_node ->dhcp_pool.end;
	vty_out(vty,"end address %s  %s",inet_ntoa(inaddr) ,VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN(no_pool_network,
	no_address_cmd,
	"no address",
	NO_STR
	"Delete network section\n")
{
	struct ip_pool *pool_node = NULL;
	
	pool_node = (struct ip_pool *)vty->index;

	/*if(pool_node ->dhcp_pool.used || pool_node ->dhcp_pool.static_num)//aaaaaaaaaaaafix
	{
		vty_out(vty,"Error: There are IP addresses allocated in the network section.%s",VTY_NEWLINE);
		return CMD_WARNING;

	}*/

	if(pool_node ->dhcp_pool.start)
	{
		//delete  address
		dhcp_addr_del_by_pool(pool_node); 
		dhcp_static_addr_del_by_pool(pool_node); 

		//pool_node ->dhcp_pool.leases = NULL;
		
		if(pool_node ->dhcp_pool.indexd)
		{
			free(pool_node ->dhcp_pool.indexd);
			pool_node ->dhcp_pool.indexd = NULL;
		}

		if(pool_node ->dhcp_pool.indexs)
		{
			free(pool_node ->dhcp_pool.indexs);
			pool_node ->dhcp_pool.indexs = NULL;
		}
		
		pool_node ->dhcp_pool.start = 0 ;
		pool_node ->dhcp_pool.end = 0;
		pool_node ->dhcp_pool.total = 0 ;
		pool_node ->dhcp_pool.used= 0 ;
		pool_node ->dhcp_pool.expired= 0 ;
		pool_node ->dhcp_pool.static_num = 0 ;

		dhcp_write_leases(NULL);		
		
		vty_info_out(vty,"The address section has been deleted%s",VTY_NEWLINE);
	}
	else	
	{
		vty_error_out(vty,"The address section does not exist.%s",VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	return CMD_SUCCESS;
}


DEFUN(leasetime,
	leasetime_cmd,
	"lease <0-65535>",
	"Configure the lease of the IP pool\n"
	"Minute, from 0 to 65535\n")
{
	struct ip_pool *pool_node = NULL;

	pool_node = (struct ip_pool *)vty->index;
	
	//pool_node ->dhcp_pool.leasetime = atoi(argv[0])*24*60*60 + atoi(argv[1])*60*60+atoi(argv[2])*60;
	pool_node ->dhcp_pool.leasetime = atoi(argv[0])*60;

	if(pool_node ->dhcp_pool.leasetime)
	{
		vty_info_out(vty,"Lease time  is %d minutes %s",pool_node ->dhcp_pool.leasetime/60,VTY_NEWLINE);
	}
	else
	{
		vty_info_out(vty,"Lease time  unlimited %s",VTY_NEWLINE);
	}	

	return CMD_SUCCESS;
}

DEFUN(no_leasetime,
	no_leasetime_cmd,
	"no lease",	
	NO_STR
	"Restore the default lease 1 day\n")
{
	struct ip_pool *pool_node = NULL;

	pool_node = (struct ip_pool *)vty->index;
	
	pool_node ->dhcp_pool.leasetime = 24*60*60 ;// 1440 minutes ,  1day

	vty_info_out(vty,"Lease time  restored 1 day %s",VTY_NEWLINE);

	return CMD_SUCCESS;
}


DEFUN(static_bind,
	static_bind_cmd,
	"static-bind ip A.B.C.D mac XX:XX:XX:XX:XX:XX",
	"Static bind\n"
	"IP address for static bind\n"
	"IP address\n"
	"MAC address for static bind\n"
	"MAC address\n")
{
	struct ip_pool *pool_node = NULL;
	uint32_t ip_addr,ip_addr_host;
	uint8_t  mac_addr[6];
	struct dhcpOfferedAddr *lease;

	uint16_t pos,m,n;
	uint8_t  *indexd,*indexs;
	struct in_addr inaddr;

	//check ip
	ip_addr_host = inet_strtoipv4((char *)argv[0]);
	ip_addr 	 = htonl(ip_addr_host);
	if(!inet_valid_ipv4(ip_addr_host))
	{
		vty_error_out(vty,"Invalid ip value%s",VTY_NEWLINE);
		return CMD_WARNING;			
	}

	//check mac
	if(ether_valid_mac((char *)argv[1]))
	{
		vty_error_out(vty,"Invalid mac value%s",VTY_NEWLINE);
		return CMD_WARNING;			
	}
	ether_string_to_mac((char *)argv[1], (uchar *)mac_addr);

	if(mac_addr[0]&0x01)
	{
		vty_error_out(vty,"Invalid mac value%s",VTY_NEWLINE);
		return CMD_WARNING;			
	}

	pool_node = (struct ip_pool *)vty->index;

	if(!pool_node ->dhcp_pool.start)
	{
		vty_error_out(vty,"Please config the address section first%s",VTY_NEWLINE);
		return CMD_WARNING;		
	}	

	if(ntohl(ip_addr) < ntohl(pool_node ->dhcp_pool.start) 
		|| ntohl(ip_addr) > ntohl(pool_node ->dhcp_pool.end))
	{
		vty_error_out(vty,"The address section should be within the pool.%s",VTY_NEWLINE);
		return CMD_WARNING;
	}	


	//interface ip check
	if(dhcp_if_lookup_by_ip(ip_addr,0))
	{
		vty_error_out(vty,"The address section conflict within interface ip.%s",VTY_NEWLINE);
		return CMD_WARNING;		

	}

	//gateway check
	if(ip_addr == pool_node->dhcp_pool.gateway)
	{
		vty_error_out(vty,"The address section cannot be gateway.%s",VTY_NEWLINE);
		return CMD_WARNING;	
	}

	//ip   check
	pos = ntohl(ip_addr)-ntohl(pool_node ->dhcp_pool.start);
	m = pos/8;
	n = pos%8;

	indexd	= pool_node->dhcp_pool.indexd;		
	indexs	= pool_node->dhcp_pool.indexs;	
	
	if(!indexd || !indexs )
	{
		zlog_err("static-bind() , index should not be NULL");
		return CMD_WARNING;		
	
	}
	if(!(indexd[m] & (0x01<<n)) ||  !(indexs[m] & (0x01<<n)))
	{
		vty_error_out(vty,"ip exist or conflict%s",VTY_NEWLINE); 	
		return CMD_WARNING;			
	}

	//mac check
	lease = dhcp_static_addr_lookup(mac_addr);
	if(lease)
	{
		vty_error_out(vty,"mac exist%s",VTY_NEWLINE); 	
		return CMD_WARNING; 	
	}

	lease = dhcp_addr_lookup_by_mac(mac_addr,pool_node);
	if(lease)
	{
		vty_error_out(vty,"mac exist%s",VTY_NEWLINE);  
		return CMD_WARNING;	 
	}

	//set static flag
	indexs[m] &= ~(0x01<<n) ;		
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "static-bind() , set static flag ,pos:%d , m:%d, n:%d",pos,m,n);			

	pool_node->dhcp_pool.static_num ++;

	lease = dhcp_static_addr_add(ip_addr,mac_addr);
	lease->type = e_type_static;
	lease->pool_index = pool_node ->pool_index ;
	lease->expires = 0x7fffffff;
	lease->state = e_state_nouse;
	
	inaddr.s_addr		= ip_addr;
	vty_info_out(vty,"static-bind,ip[%s] ,mac[%02X:%02X:%02X:%02X:%02X:%02X] %s",inet_ntoa(inaddr),
		mac_addr[0],mac_addr[1],mac_addr[2],mac_addr[3],mac_addr[4],mac_addr[5],VTY_NEWLINE);
	
	return CMD_SUCCESS;
}

DEFUN(no_static_bind_ip,
	no_static_bind_cmd_ip,
	"no static-bind ip A.B.C.D ",
	NO_STR
	"Static bind\n"
	"IP address for static bind\n"
	"IP address\n")
{
	struct ip_pool *pool_node = NULL;
	uint32_t ip_addr,ip_addr_host;
	uint16_t pos,m,n;
	uint8_t  *indexs;
	char *str;
	
	if (argv[0] == NULL)
	{
		vty_error_out(vty,"Input value is NULL%s",VTY_NEWLINE);
		return CMD_WARNING;			
	}
	str = (char*)XCALLOC(MTYPE_IF, 17);
	if(NULL == str) 
	{
		DHCP_LOG_ERROR("Error: There is no ram space\n");		  
	
		return CMD_WARNING;
	}
	memcpy(str, argv[0], 17);
	//check ip
	ip_addr_host = inet_strtoipv4(str);
	ip_addr	 = htonl(ip_addr_host);
	if(!inet_valid_ipv4(ip_addr_host))
	{
		vty_error_out(vty,"Invalid ip value%s",VTY_NEWLINE);
		
		XFREE(MTYPE_IF, str);
		str = NULL;
		return CMD_WARNING;			
	}

	pool_node = (struct ip_pool *)vty->index;
	if(!pool_node ->dhcp_pool.start)
	{
		vty_error_out(vty,"The address section does not exist.%s",VTY_NEWLINE);
		XFREE(MTYPE_IF, str);
		str = NULL;
		return CMD_WARNING;		
	}	

	if(ntohl(ip_addr) < ntohl(pool_node ->dhcp_pool.start) 
		|| ntohl(ip_addr) > ntohl(pool_node ->dhcp_pool.end))
	{
		vty_error_out(vty,"The ip is not in the address section%s",VTY_NEWLINE);
		XFREE(MTYPE_IF, str);		
		str = NULL;
		return CMD_WARNING;
	}	

	//static ip  check
	indexs = pool_node->dhcp_pool.indexs;
	pos = ntohl(ip_addr)-ntohl(pool_node ->dhcp_pool.start);
	m = pos/8;
	n = pos%8;

	indexs	= pool_node->dhcp_pool.indexs;	
	
	if(!indexs || (indexs[m] & (0x01<<n)))
	{
		vty_error_out(vty,"The static ip not exist%s",VTY_NEWLINE); 		
		XFREE(MTYPE_IF, str);		
		str = NULL;
		return CMD_WARNING;		
	}
	
	indexs[m] |= 0x01<<n;//set 1

	
	dhcp_static_addr_del(ip_addr);

	pool_node->dhcp_pool.static_num --;
	vty_info_out(vty,"Static ip del success.%s",VTY_NEWLINE);
	XFREE(MTYPE_IF, str);	
	str = NULL;
	return CMD_SUCCESS;		
}
#if 0
int check_str(char * str,uint8_t max_len)
{
	uint8_t len;
	uint8_t i;
	uint8_t flag;
	uint8_t cnt_1=0, cnt_2=0, cnt_3=0;

	
	len = strlen(str);

	if(str == NULL || len > max_len)
		return 0;

	for(i=0; i<len; i++)
	{
		flag = 0;
		
		if(str[i]>=0x41 && str[i]<=0x5a)// A-Z
		{
			flag=1 ;
		}
		else if(str[i]>=0x61 && str[i]<=0x7a)//a-z
		{
			flag=1 ;
		}
		else if(str[i]>=0x30 && str[i]<=0x39)//0-9
		{
			flag=1 ;
		}
		else if(str[i] == 0x5f)//_
		{
			flag=1 ;
			cnt_1 ++;
		}
		else if(str[i] == 0x2d)// -
		{
			flag=1 ;
			cnt_2 ++;			
		}
		else if(str[i] == 0x2e)// .
		{
			flag=1 ;
			cnt_3 ++;			
		}

		if(!flag)
			return 0;
		
	}

	if(cnt_1 == len || cnt_2 == len || cnt_3 == len)
		return 0;

	return 1;
}
#endif
int dhcp_pool_config_write ( struct vty *vty )
{
    struct ip_pool *pool_node = NULL;
    struct hash_bucket *pbucket = NULL;
    int cursor;
	uint32_t tmp;
	uchar   *mac = NULL;
	
    struct hash_bucket *pbucket1 = NULL;
    int cursor1;
	struct dhcpOfferedAddr *addr_node = NULL;

	struct in_addr inaddr;

	HASH_BUCKET_LOOP(pbucket,cursor,ip_pool_table)
    {
        pool_node = pbucket->data;
		
	    //vty_out (vty, "!%s", VTY_NEWLINE);	
		
      	vty_out(vty,"ip pool dhcp %d%s",pool_node->pool_index,VTY_NEWLINE);
		
		tmp = pool_node->dhcp_pool.start;		
		inaddr.s_addr		= tmp;
		if(tmp)
		{
      		vty_out(vty," address %s %d %s",inet_ntoa(inaddr),pool_node->dhcp_pool.total ,VTY_NEWLINE);
		}
		tmp = pool_node->dhcp_pool.dns_server;
		inaddr.s_addr		= tmp;		
		if(tmp)
		{
      		vty_out(vty," dns-server %s %s",inet_ntoa(inaddr),VTY_NEWLINE);
		}
	   	
		tmp = pool_node->dhcp_pool.gateway;
		inaddr.s_addr		= tmp;			
		if(tmp)
		{
      		vty_out(vty," gateway  %s/%d %s",inet_ntoa(inaddr),pool_node->dhcp_pool.mask_len,VTY_NEWLINE);
		}
		tmp = pool_node->dhcp_pool.leasetime;
		if(tmp!=86400)
		{
      		vty_out(vty," lease  %d %s",tmp/60,VTY_NEWLINE);
		}

		HASH_BUCKET_LOOP(pbucket1,cursor1,dhcp_static_addr_table)
		{
			addr_node = pbucket1->data;
			if(addr_node->pool_index == pool_node->pool_index)
			{
					tmp = addr_node->yiaddr; 
					mac = addr_node->chaddr;
					inaddr.s_addr		= tmp;					
					vty_out(vty," static-bind ip  %s  mac %02x:%02x:%02x:%02x:%02x:%02x %s",inet_ntoa(inaddr),\
						mac[0] ,mac[1] ,mac[2] ,mac[3] ,mac[4] ,mac[5] ,VTY_NEWLINE);
			}
		}

	    vty_out (vty, "!%s", VTY_NEWLINE);	    
    }
	return 0;
}

#if 0
void show_ip_pool_node_offered( struct vty *vty ,struct ip_pool *pool_node)
{
	uint32_t tmp;
	uchar   *mac = NULL;
	int i=0;
	int expires_time;
	uchar *p_ip=NULL;
	char ip_str[20];
	char time_str[100];


	//dynamic info

	if(!pool_node ->dhcp_pool.used)
		return ;

	for(i=0 ; i< pool_node ->dhcp_pool.total ;i++)
	{
		if(pool_node ->dhcp_pool.leases[i].yiaddr)
		{
			tmp = pool_node ->dhcp_pool.leases[i].yiaddr ;
			mac = pool_node ->dhcp_pool.leases[i].chaddr ;
			p_ip = &tmp;
			sprintf(ip_str,"%d.%d.%d.%d",p_ip[0],p_ip[1],p_ip[2],p_ip[3]);
			sprintf(time_str,"remaining time unlimited");				
			
			if(pool_node->dhcp_pool.leasetime)
			{
				expires_time = pool_node ->dhcp_pool.leases[i].expires-time(0) ; //seconds
				if(expires_time>=0)
				{
					expires_time = expires_time/60; //minutes
				}
				else
				{
					expires_time = 0 ;
				}
				sprintf(time_str,"remaining time %d minites",expires_time);
			}
			
			vty_out(vty,"%-15s: ip %-15s, mac %02x:%02x:%02x:%02x:%02x:%02x, %s %s",\
				"OfferedAddr",\
				ip_str,\
				mac[0] ,mac[1] ,mac[2] ,mac[3] ,mac[4] ,mac[5] ,\
				time_str,\
				VTY_NEWLINE);
			
		}
		
	}

}

#endif

void show_ip_pool_node ( struct vty *vty ,struct ip_pool *pool_node)
{
	uint32_t tmp;
    struct hash_bucket *pbucket = NULL;
    int cursor;
	struct dhcpOfferedAddr *addr_node = NULL;
	struct in_addr inaddr;
	int conflict_count = 0;
	
	if(!pool_node)
	{
		return ;
	}
  	vty_out(vty,"%-15s: %d%s","Pool-index",pool_node->pool_index,VTY_NEWLINE);
	
	tmp = pool_node->dhcp_pool.gateway;
	inaddr.s_addr		= tmp;	
	if(tmp)
	{
  		vty_out(vty,"%-15s: %s/%d %s","Gateway",inet_ntoa(inaddr),pool_node->dhcp_pool.mask_len,VTY_NEWLINE);
	}
	else
	{
  		vty_out(vty,"%-15s: %s %s","Gateway", "--", VTY_NEWLINE);
	}
	tmp = pool_node->dhcp_pool.start;
	inaddr.s_addr		= tmp;	
  	vty_out(vty,"%-15s: %s %s","Start-address",tmp?inet_ntoa(inaddr):"--",VTY_NEWLINE);
	
	tmp = pool_node->dhcp_pool.end;
	inaddr.s_addr		= tmp;		
  	vty_out(vty,"%-15s: %s %s","End-address",tmp?inet_ntoa(inaddr):"--",VTY_NEWLINE);

	tmp = pool_node->dhcp_pool.leasetime;
	if(tmp)
	{
		vty_out(vty,"%-15s: %d minutes %s","Lease",tmp/60,VTY_NEWLINE);			
	}
	else
	{
		vty_out(vty,"%-15s: %s%s","Lease","Unlimited",VTY_NEWLINE);	
	}
	tmp = pool_node->dhcp_pool.dns_server;	
	inaddr.s_addr		= tmp;	
	vty_out(vty,"%-15s: %s %s","DNS-server",tmp?inet_ntoa(inaddr):"--",VTY_NEWLINE);

	//total num
	tmp = pool_node->dhcp_pool.total;
  	vty_out(vty,"%-15s: %d %s","Total-num",tmp,VTY_NEWLINE);

	//static num
	tmp = pool_node->dhcp_pool.static_num;
  	vty_out(vty,"%-15s: %d %s","Static-num",tmp,VTY_NEWLINE);


	//dynamic num,expired num
	pool_node->dhcp_pool.used		 = 0;
	pool_node ->dhcp_pool.expired   = 0;

	HASH_BUCKET_LOOP(pbucket,cursor,dhcp_addr_table)
	{
		addr_node = pbucket->data;
		if(addr_node->pool_index == pool_node->pool_index)
		{
			pool_node ->dhcp_pool.used++;	
			if(addr_node->expires<= (uint32_t)time(0))
			{
				pool_node ->dhcp_pool.expired ++;	
			}	
		}
	}
	
	//dynamic num
	tmp = pool_node->dhcp_pool.used;
	vty_out(vty,"%-15s: %d %s","Dynamic-num",tmp,VTY_NEWLINE);
	
	tmp = pool_node ->dhcp_pool.expired;
	vty_out(vty,"%-15s: %d %s","Expired-num",tmp,VTY_NEWLINE);

	//remaining num
	tmp = pool_node->dhcp_pool.total + pool_node ->dhcp_pool.expired - pool_node->dhcp_pool.static_num - pool_node->dhcp_pool.used;

	//check gateway
	if (pool_node->dhcp_pool.start && pool_node->dhcp_pool.gateway && pool_node->dhcp_pool.end)
	{
		if (pool_node->dhcp_pool.gateway >= pool_node->dhcp_pool.start && pool_node->dhcp_pool.gateway <=pool_node->dhcp_pool.end)
		{
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "gateway is between start and end ip addresses");
			tmp--;
		}
	}

	//check interface conflict  ip address
	conflict_count = check_interface_ip_address(htonl(pool_node->dhcp_pool.start), htonl(pool_node->dhcp_pool.end), htonl(pool_node->dhcp_pool.gateway),0);
	if (conflict_count != 0)
	{
		tmp = tmp - conflict_count;
	}
	vty_out(vty,"%-15s: %d %s","Remaining-num",tmp,VTY_NEWLINE);

  	vty_out(vty,"%s",VTY_NEWLINE);
}

#if 0

void show_ip_pool_node_address ( struct vty *vty ,struct ip_pool *pool_node)
{
	uint32_t tmp;
	uchar   *mac = NULL;
	struct static_lease *cur=NULL;	
	int i=0;
    struct hash_bucket *pbucket = NULL;
    int cursor;
	struct dhcpOfferedAddr *addr_node = NULL;
	
	if(!pool_node)	
		return ;

  	vty_out(vty,"%-15s: %s%s","Pool-name",pool_node->name,VTY_NEWLINE);

	//total address
	tmp = pool_node->dhcp_pool.total;
	if(tmp)
  		vty_out(vty,"%-15s: %d %s","Address-num",tmp,VTY_NEWLINE);
	else
  		vty_out(vty,"%-15s: %s %s","Address-num","--",VTY_NEWLINE);

	//static num
	tmp = pool_node->dhcp_pool.static_num;
  	vty_out(vty,"%-15s: %d %s","Static-num",tmp,VTY_NEWLINE);

	//dynamic num
	tmp = pool_node->dhcp_pool.used;
	vty_out(vty,"%-15s: %d %s","Dynamic-num",tmp,VTY_NEWLINE);

	pool_node ->dhcp_pool.expired = 0;



	vty_out(vty,"%-15s: %d %s","ip",tmp,VTY_NEWLINE);
	DHCP_ADDR_LOOP(pbucket,cursor)
	{
		addr_node = pbucket->data;
		if(!strcmp(addr_node->pool_name, pool_node->name))
		{
			if(addr_node->type == e_type_dynamic && addr_node->expires<= time(0))
			{				
				pool_node ->dhcp_pool.expired ++;				
			}

		}
		addr_node = NULL;
	}
	




	/*pool_node ->dhcp_pool.used=0;
	for(i=0 ; i< pool_node ->dhcp_pool.total ;i++)
	{
		if(pool_node ->dhcp_pool.leases[i].yiaddr)
		{
			pool_node ->dhcp_pool.used ++;				
		}			
	}*/
	

	//expired num
	/*pool_node ->dhcp_pool.expired=0;
	if(pool_node ->dhcp_pool.leasetime)
	{
		for(i=0 ; i< pool_node ->dhcp_pool.total ;i++)
		{
			if(pool_node ->dhcp_pool.leases[i].yiaddr && pool_node ->dhcp_pool.leases[i].expires < time(0))
			{
				pool_node ->dhcp_pool.expired ++;				
			}			
		}

	}*/
	tmp = pool_node->dhcp_pool.expired;
	vty_out(vty,"%-15s: %d %s","Expired-num",tmp,VTY_NEWLINE);

	//remaining num
	tmp = pool_node->dhcp_pool.total + pool_node ->dhcp_pool.expired - pool_node->dhcp_pool.static_num - pool_node->dhcp_pool.used;
	vty_out(vty,"%-15s: %d %s","remaining-num",tmp,VTY_NEWLINE);


	//static info
	if(pool_node->dhcp_pool.static_num)
	{
		LOOP_STATIC_LEASE(pool_node->dhcp_pool.static_leases,cur)
		{			
			tmp = cur->ip ;	
			mac = cur->mac ;
			vty_out(vty,"%-15s: ip %-15s, mac %02x:%02x:%02x:%02x:%02x:%02x %s","Static-bind",inet_ntoa(tmp),\
				mac[0] ,mac[1] ,mac[2] ,mac[3] ,mac[4] ,mac[5] ,VTY_NEWLINE);
		}
	}

	//dynamic info
	show_ip_pool_node_offered(vty, pool_node);	

	
  	vty_out(vty,"%s",VTY_NEWLINE);
	

}

#endif

int show_ip_pool ( struct vty *vty ,uint8_t pool_index)
{
    struct ip_pool *pool_node = NULL;
    struct hash_bucket *pbucket = NULL;
    int cursor;	

	if(pool_index)
	{
		pool_node = dhcp_pool_lookup(pool_index);
		if(NULL == pool_node)
		{
			vty_out(vty,"pool %d doesn't exist!%s",pool_index,VTY_NEWLINE);
			return CMD_WARNING;
		}	
		else
		{			
			show_ip_pool_node(vty, pool_node);
			return CMD_SUCCESS;
		}
	}
	
	HASH_BUCKET_LOOP(pbucket,cursor,ip_pool_table)
    {
        pool_node = pbucket->data;
		
		show_ip_pool_node(vty, pool_node);
    }
	
	return CMD_SUCCESS;
}

int show_ip_pool_address ( struct vty *vty ,uint8_t pool_index )
{
    struct hash_bucket *pbucket = NULL;
    int cursor;	
	struct dhcpOfferedAddr *addr_node = NULL;
	uint32_t tmp;
	uint8_t *mac;
	char str[300];
	char str1[50];
	
	int expires_time;
	struct in_addr inaddr;
	
	if(dhcp_addr_table.num_entries || dhcp_static_addr_table.num_entries)
	{	
		vty_out(vty,"%-16s%-18s%-8s%-15s%-11s%-5s%s%s","ip","mac", "type","free-time","state","pool","interface",VTY_NEWLINE);
	}				//     1     2     3    4     5     6    7     ,1      2         3            4             5        6           7
					
	if(!dhcp_addr_table.num_entries)
	{	
		return 0;
	}
	HASH_BUCKET_LOOP(pbucket,cursor,dhcp_addr_table)
	{
		addr_node = pbucket->data;

		if(pool_index && addr_node->pool_index != pool_index)
		{
			continue;
		}		
		
		tmp = addr_node->yiaddr;//max 15	
		inaddr.s_addr		= tmp;		
		sprintf(str,"%-16s",inet_ntoa(inaddr));

		mac = addr_node->chaddr;//max 17
		sprintf(str1,"%02X:%02X:%02X:%02X:%02X:%02X",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
		sprintf(str+strlen(str),"%-18s",str1);

		//max 7
		if(addr_node->type == e_type_static)
		{
			sprintf(str1,"%s","static");
		}
		else
		{
			sprintf(str1,"%s","dynamic");
		}
		sprintf(str+strlen(str),"%-8s",str1);

		//max13
		if(addr_node->expires==0x7fffffff)//include static
		{
			sprintf(str1,"%s","unlimited");	
		}
		else
		{
			expires_time = addr_node->expires-time(0) ; //seconds
			if(expires_time>0)
			{
				if (expires_time > 86400 && addr_node->now > 0)
				{
					expires_time = addr_node->expires - addr_node->now;
				}
				expires_time = expires_time/60; //minutes
			}
			else
			{
				expires_time = 0 ;
				if(addr_node->state ==e_state_send_ack)
				{	
					addr_node->state	= e_state_expired;			
				}
			}
			sprintf(str1,"%d minutes",expires_time);

		}
		sprintf(str+strlen(str),"%-15s",str1);

		//state,max 10
		tmp = addr_node->state;
		if(tmp==e_state_nouse)
		{
			sprintf(str1,"%s","NOUSE");
		}
		else if(tmp==e_state_send_offer)
		{
			sprintf(str1,"%s","REQUESTING");
		}
		else if(tmp==e_state_send_ack)
		{
			sprintf(str1,"%s","BOUND");
		}
		else if(tmp==e_state_released)
		{
			sprintf(str1,"%s","RELEASED");	
		}
		else if(tmp==e_state_declined)
		{
			sprintf(str1,"%s","DECLINED");	
		}else if(tmp==e_state_expired)
		{
			sprintf(str1,"%s","EXPIRED");		
		}
		else
		{
			sprintf(str1,"%s","--");
		}
		sprintf(str+strlen(str),"%-11s",str1);

		//pool
		sprintf(str1,"%d",addr_node->pool_index);
		sprintf(str+strlen(str),"%-5s",str1);
		
		//ifname
		tmp = addr_node->ifindex;
		if(tmp)
		{
			ifm_get_name_by_ifindex(tmp,str1);
			sprintf(str+strlen(str),"%s",str1);			
		}
		else
		{
			sprintf(str+strlen(str),"%s","--");			
		}
		
		vty_out(vty,"%s%s",str,VTY_NEWLINE);		
	}
	
	return 0;
}



int show_ip_pool_static_address(struct vty *vty, uint8_t pool_index )
{
    struct hash_bucket *pbucket = NULL;
    int cursor;	
	struct dhcpOfferedAddr *addr_node = NULL;
	uint32_t tmp;
	uint8_t *mac;
	char str[300];
	char str1[50];
	int expires_time;
	struct in_addr inaddr;

	if(!dhcp_static_addr_table.num_entries)
	{
		return 0;
	}
	HASH_BUCKET_LOOP(pbucket, cursor, dhcp_static_addr_table)
	{
		addr_node = pbucket->data;

		if(pool_index && addr_node->pool_index != pool_index)
		{
			continue;
		}
		
		tmp = addr_node->yiaddr;//max 15	
		inaddr.s_addr		= tmp;		
		sprintf(str,"%-16s",inet_ntoa(inaddr));

		mac = addr_node->chaddr;//max 17
		sprintf(str1,"%02X:%02X:%02X:%02X:%02X:%02X",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
		sprintf(str+strlen(str),"%-18s",str1);

		//max 7
		if(addr_node->type == e_type_static)
		{
			sprintf(str1,"%s","static");
		}
		else
		{
			sprintf(str1,"%s","dynamic");
		}
		sprintf(str+strlen(str),"%-8s",str1);

		//max13
		if(addr_node->expires==0x7fffffff)//include static
		{
			sprintf(str1,"%s","unlimited");	
		}
		else
		{
			expires_time = addr_node->expires-time(0) ; //seconds
			if(expires_time>0)
			{
				expires_time = expires_time/60; //minutes
			}
			else
			{
				expires_time = 0 ;
				if(addr_node->state == e_state_send_ack)
				{	
					addr_node->state = e_state_expired;			
				}
			}
			sprintf(str1,"%d minutes",expires_time);
		}
		sprintf(str+strlen(str),"%-15s",str1);

		//state,max 10
		tmp = addr_node->state;
		if(tmp==e_state_nouse)
		{
			sprintf(str1,"%s","NOUSE");
		}
		else if(tmp==e_state_send_offer)
		{
			sprintf(str1,"%s","REQUESTING");
		}
		else if(tmp==e_state_send_ack)
		{
			sprintf(str1,"%s","BOUND");
		}
		else if(tmp==e_state_released)
		{
			sprintf(str1,"%s","RELEASED");	
		}
		else if(tmp==e_state_declined)
		{
			sprintf(str1,"%s","DECLINED");	
		}
		else if(tmp==e_state_expired)
		{
			sprintf(str1,"%s","EXPIRED");		
		}
		else
		{
			sprintf(str1,"%s","--");
		}
		sprintf(str+strlen(str),"%-11s",str1);

		//pool
		sprintf(str1,"%d",addr_node->pool_index);
		sprintf(str+strlen(str),"%-5s",str1);
		
		//ifname
		tmp = addr_node->ifindex;
		if(tmp)
		{
			ifm_get_name_by_ifindex(tmp,str1);
			sprintf(str+strlen(str),"%s",str1);			
		}
		else
		{
			sprintf(str+strlen(str),"%s","--");			
		}
		
		vty_out(vty,"%s%s",str,VTY_NEWLINE);				
	}
	
	return 0;
}


void dhcp_pool_cmd_init(void)
{
	install_node(&pool_node, dhcp_pool_config_write);
		
	install_default(POOL_NODE);

	install_element(CONFIG_NODE, &pool_name_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &no_pool_name_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &show_pool_info_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &show_pool_address_info_cmd, CMD_LOCAL);

	install_element(POOL_NODE, &gateway_list_cmd, CMD_SYNC);	
	install_element(POOL_NODE, &address_cmd, CMD_SYNC);	
	install_element(POOL_NODE, &dns_list_cmd, CMD_SYNC);	
	install_element(POOL_NODE, &leasetime_cmd, CMD_SYNC);	
	install_element(POOL_NODE, &static_bind_cmd, CMD_SYNC);

	install_element(POOL_NODE, &no_gateway_list_cmd, CMD_SYNC);
	install_element(POOL_NODE, &no_dns_list_cmd, CMD_SYNC);
	install_element(POOL_NODE, &no_address_cmd, CMD_SYNC);
	install_element(POOL_NODE, &no_static_bind_cmd_ip, CMD_SYNC);
	install_element(POOL_NODE, &no_leasetime_cmd, CMD_SYNC); 

	/*install_element (POOL_NODE, &pool_ip_rule_cmd);
	install_element (POOL_NODE, &no_pool_ip_rule_cmd);
	install_element (POOL_NODE, &pool_ip_extend_rule_cmd);
	install_element (POOL_NODE, &pool_ip_extend_2_rule_cmd);
	install_element (POOL_NODE, &pool_mac_rule_cmd);*/
}

