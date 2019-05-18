
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
//#include <netdb.h>
//#include <netinet/in.h>
//#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
//#include <sys/ioctl.h>
#include <time.h>
#include <sys/time.h>

#include <lib/ifm_common.h>
#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>
#include <lib/linklist.h>
#include <lib/inet_ip.h>

#include "dhcp_server.h"
#include "options.h"
#include "files.h"
#include "serverpacket.h"
#include "pool.h"
#include "pool_address.h"
#include "pool_static_address.h"

/* globals */
extern struct hash_table ip_pool_table;
struct if_dhcp *pif_server;
struct ip_pool *p_pool;
struct dhcpOfferedAddr *leases;

int dhcp_server_fsm(struct if_dhcp *pif, struct pkt_buffer *pkt)
{
	uint8_t *pmsg_type;
	uint8_t *pserver_id = NULL;
	uint8_t *prequested = NULL;
	uint32_t server_id = 0, requested = 0,ip_addr;
	struct dhcpOfferedAddr *lease = NULL;
	struct dhcpMessage *pdhcp_msg = NULL;
	struct dhcpOfferedAddr *lease_static = NULL;
    struct hash_bucket *pbucket = NULL;
    int cursor;
	struct ip_pool *pool_node = NULL;

	uint32_t yiaddr;
	uint8_t flag = DISABLE;/* 是否有可用IP地址 */
	struct in_addr addr;

	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "Entering the function of '%s'",__func__);

	if(!pif || pif->status == DISABLE || pif->mode !=IFNET_MODE_L3)
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "leave the function of '%s' because of status or mode",__func__);
	
		return 0;
	}

	pif_server = pif;

	if(!pif_server)
	{
		return 0;
	}
	if(pkt)
	{
		pdhcp_msg = pkt->data;
	}		
	if(pif_server->dhcp_role != E_DHCP_SERVER || !pif_server->ip_addr)
	{	
		DHCP_LOG_ERROR("dhcp_server_fsm() return ,ifindex:%08X, dhcp_role:%d, ip_addr:%08X ",pif_server->ifindex,pif_server->dhcp_role ,pif_server->ip_addr );
	
		return 0 ;
		//server_config.snmp_server= p_pool->dhcp_pool.snmp_server; //aaaaaaaaaaaaaaaaaafix		
	}

	if(!pdhcp_msg)
	{
		return 0;
	}
	if((pmsg_type = get_option(pdhcp_msg, DHCP_MESSAGE_TYPE)) == NULL) 
	{
		DHCP_LOG_ERROR("couldn't get option from pdhcp_msg, ignoring");
		return 0;
	}

	/* 包中携带的relay ip或server ip与地址池中网关地址一致，依据包中携带的client硬件地址，
	查询静态表，返回的变量的client ip与网关在同一网段内，直接分配给客户端这个ip地址 */
	HASH_BUCKET_LOOP(pbucket, cursor, ip_pool_table)
	{
		pool_node = pbucket->data;
		if(!pool_node->dhcp_pool.gateway || !pool_node->dhcp_pool.start)
		{
			continue;
		}
		ip_addr = pdhcp_msg->giaddr?pdhcp_msg->giaddr:pif_server->ip_addr;
		if(ipv4_is_same_subnet(ntohl(ip_addr),ntohl(pool_node->dhcp_pool.gateway),pool_node->dhcp_pool.mask_len))
		{		
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "find static ip pool successed!");

			lease_static = dhcp_static_addr_lookup(pdhcp_msg->chaddr);
			if(lease_static && lease_static->yiaddr)
			{
				DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "find ip pool yiaddr is 0x%x!", ntohl(lease_static->yiaddr));

				if(ipv4_is_same_subnet(ntohl(lease_static->yiaddr), ntohl(pool_node->dhcp_pool.gateway), pool_node->dhcp_pool.mask_len))
				{
					DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "yiaddr 0x%x and gateway 0x%x are equal subnet!", ntohl(lease_static->yiaddr), ntohl(pool_node->dhcp_pool.gateway));
					lease = lease_static;
				}
			}
		}
	}

	if(pmsg_type[0] != DHCPDISCOVER )
	{
		prequested = get_option(pdhcp_msg, DHCP_REQUESTED_IP);
		pserver_id = get_option(pdhcp_msg, DHCP_SERVER_ID);
		
		if(prequested) 
		{
			memcpy(&requested, prequested, 4);
		}
		
		if(pserver_id)
		{
			memcpy(&server_id, pserver_id, 4);
		}
		
		ip_addr = requested?requested:(pdhcp_msg->ciaddr);//0 ,or  requested ,or pdhcp_msg->ciaddr

		if(!lease)
		{
			lease = dhcp_addr_lookup(ip_addr);
		}
		if(!lease)	
		{
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "addr_lookup fail ,return");			
			return 0;
		}

		if(memcmp(lease->chaddr,pdhcp_msg->chaddr,6)) //should be equal
		{
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "mac not equal ,return");			
		
			return 0;
		}
	}

	if(lease)
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "addr_lookup ok ,type:%s",lease->type?"static":"dynamic");			

		p_pool =dhcp_pool_lookup(lease->pool_index);

		if(!p_pool)
		{
			return 0;
		}
	}
		
	switch(pmsg_type[0]) 
	{
		case DHCPDISCOVER:
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "received DISCOVER");			
			if(!lease)
			{
				HASH_BUCKET_LOOP(pbucket,cursor,ip_pool_table)
				{
					flag = DISABLE;
					pool_node = pbucket->data;
					
					if(!pool_node->dhcp_pool.gateway || !pool_node->dhcp_pool.start)
					{	
						continue;
					}
					ip_addr = pdhcp_msg->giaddr?pdhcp_msg->giaddr:pif_server->ip_addr;
						
					if(ipv4_is_same_subnet(ntohl(ip_addr),ntohl(pool_node->dhcp_pool.gateway),pool_node->dhcp_pool.mask_len))
					{					
						p_pool			= pool_node ;
						
						//find free address
						yiaddr = dhcp_find_address(0,pdhcp_msg->giaddr?0:1);

						//find  allocated address but expired
						if(!yiaddr)
						{
							flag = ENABLE;
							yiaddr = dhcp_find_address(1,pdhcp_msg->giaddr?0:1);							
						}
						
 						//delet lease of same mac
						lease = dhcp_addr_lookup_by_mac(pdhcp_msg->chaddr,p_pool);
						if(lease)
						{							
							addr.s_addr 	= lease->yiaddr;
							DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "delet lease of same mac,ip:%s",inet_ntoa(addr)); 		
							dhcp_addr_del(lease->yiaddr,p_pool);
						}
						
						if(yiaddr)
						{
							if(flag == ENABLE)
							{
								lease = dhcp_addr_lookup(yiaddr);

								if(!lease)
								{
									DHCP_LOG_ERROR("dhcp_addr_lookup failed");
									continue;									
								}
								memcpy(lease->chaddr,pdhcp_msg->chaddr,6); //update mac

							}
							else
							{
								lease = dhcp_addr_add(yiaddr,pdhcp_msg->chaddr);
								lease->type = e_type_dynamic; 						
								lease->pool_index = pool_node ->pool_index;								
							}								

							goto out;
						}
						
						DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "ifindex %08X match pool fail!! ,continue",pif_server->ifindex );

						//continue;
					}
				}	
				
				DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "ifindex %08X match pool fail!! ,return  ",pif_server->ifindex );
				
				return 0;
			}

			out:
				
			if(sendOffer(pdhcp_msg,lease) < 0) 
			{
				DHCP_LOG_ERROR("send OFFER failed");
			}
			break;
		case DHCPREQUEST:
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "received REQUEST");		
			if(lease) 
			{
				if(pserver_id) 
				{
					/* SELECTING State */
					DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "server_id = %08x", ntohl(server_id));
					if(server_id == pif_server->ip_addr && prequested &&
						requested == lease->yiaddr)
					{
						sendACK(pdhcp_msg, lease->yiaddr,lease);
					}
					else //add by hqq ,  not select me,release lease
					{
						if(lease->type == e_type_dynamic)//we do not change static band
						{
							dhcp_addr_del(lease->yiaddr,p_pool);
						}
					}
				} 
				else 
				{
					if(prequested) 
					{
						/* INIT-REBOOT State */
						if(lease->yiaddr == requested)
						{
							sendACK(pdhcp_msg, lease->yiaddr,lease);
						}
						else
						{
							sendNAK(pdhcp_msg);
						}
					}
					else 
					{
						/* RENEWING or REBINDING State */
						if(lease->yiaddr == pdhcp_msg->ciaddr)
						{
							sendACK(pdhcp_msg, lease->yiaddr,lease);
						}
						else 
						{
							/* don't know what to do!!!! */
							sendNAK(pdhcp_msg);
						}
					}
				}
	
			/* what to do if we have no record of the client */
			} 
			else if(pserver_id) 
			{
				/* SELECTING State */
	
			} 
			else if(prequested) 
			{
				/* INIT-REBOOT State */
				if((lease = dhcp_addr_lookup(requested))) 
				{
					if(lease_expired(lease)) 
					{
						/* probably best if we drop this lease */
						memset(lease->chaddr, 0, 16);
						/* make some contention for this address */
					} 
					else
					{
						sendNAK(pdhcp_msg);

					}
				}
				else if(requested < p_pool->dhcp_pool.start || requested > p_pool->dhcp_pool.end) 
				{
					sendNAK(pdhcp_msg);
				} /* else remain silent */
			} 
			else 
			{
				 /* RENEWING or REBINDING State */
			}
			break;
		case DHCPDECLINE:
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "received DECLINE");
			if (lease) 
			{
				//memset(lease->chaddr, 0, 16);
				//lease->expires = time(0) + 10*60; //10 minutes
				//lease->state = e_state_declined;	
				if(lease->type == e_type_dynamic)
				{
					dhcp_addr_del(lease->yiaddr,p_pool);					
				}
				else
				{
					lease->state = e_state_declined;	
				}
			}
			break;
		case DHCPRELEASE:
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "received RELEASE");
			if (lease) 
			{
				//lease->expires = time(0);
				//lease->state = e_state_released;
				if(lease->type == e_type_dynamic)
				{
					dhcp_addr_del(lease->yiaddr,p_pool);					
				}
				else
				{
					lease->state = e_state_released;
				}				
			}
			break;
		case DHCPINFORM:
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "received INFORM");
			send_inform(pdhcp_msg);
			break;
		default:
			DHCP_LOG_ERROR("unsupported DHCP message (%02x) -- ignoring", pmsg_type[0]);
	}
	return 0;
}
