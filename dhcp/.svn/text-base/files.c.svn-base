
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <lib/hash1.h>
#include <lib/thread.h>
#include <lib/prefix.h>
#include <lib/inet_ip.h>

#include "dhcp_server.h"
#include "files.h"
#include "options.h"
#include "pool_address.h"

//extern struct hash_table ip_pool_table;
extern struct thread_master *dhcp_master;

/*
 * Domain names may have 254 chars, and string options can be 254
 * chars long. However, 80 bytes will be enough for most, and won't
 * hog up memory. If you have a special application, change it
 */


int dhcp_write_leases(void *msg)
{
	FILE *fp=NULL;
	char file[100];
	
	struct dhcpOfferedAddr *addr_node = NULL;
    struct hash_bucket *pbucket = NULL;
    int cursor;	
	//static uchar count=0;
	struct ip_pool *pool_node;
	uchar flag = DISABLE;/* 是否打开/data/dat/ip_pool.dat */
	struct in_addr addr;

	//zlog_debug("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
	
	sprintf(file,"%s","/data/dat/ip_pool.dat");

	//if (t==NULL || count >= 9)/* change for wumm, read file per 60s */ 
	{
		if(!(fp = fopen(file, "w")))
		{
			DHCP_LOG_ERROR("Unable to open %s for writing", file);
			return 1;
		}
		flag = ENABLE;	
	//	count = 0;		
	}


    HASH_BUCKET_LOOPW ( pbucket, cursor, dhcp_addr_table )
	{
		addr_node 		=  pbucket->data;
		pbucket		    =  pbucket->next;

		if((addr_node->expires > (unsigned long) time(0)))
		{
			if(flag == ENABLE)
			{
				//LOG(LOG_DEBUG, "write address , pool:%d, ip:%s \n",addr_node->pool_index, inet_ntoa((addr_node->yiaddr)));
				addr_node->now = time(0);
				fwrite(addr_node, sizeof(struct dhcpOfferedAddr), 1, fp);
			}
				
		}
		else
		{		
		
			addr.s_addr 	= addr_node->yiaddr;
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "del expired address , pool:%d, ip:%s \n",addr_node->pool_index, inet_ntoa((addr)));

			pool_node = dhcp_pool_lookup(addr_node->pool_index);
			dhcp_addr_del(addr_node->yiaddr,pool_node);
		}		
	}

	if(flag == ENABLE)
	{
		fclose(fp);
	}
	#if 0
	if(t)
	{
		count++;
		thread_add_timer(dhcp_master,dhcp_write_leases,NULL,60);
	}
	#endif
	return 0;
	/*if (server_config.notify_file) {
		sprintf(buf, "%s %s", server_config.notify_file, server_config.lease_file);
		system(buf);
	}*/
}

void dhcp_read_leases(struct ip_pool *pool_node)
{
	FILE *fp;
	unsigned int i = 0;
	struct dhcpOfferedAddr lease;
	char file[100];
	unsigned long max_leases;
	uint32_t start;		/* Start address of leases, network order */
	uint32_t end;			/* End of leases, network order */
	uint16_t pos,m,n;
	uint8_t  *indexd;
	
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "Entering the function of '%s'--the line of %d",__func__,__LINE__);
	
	indexd = pool_node->dhcp_pool.indexd; 	
	
	if(indexd == NULL)
	{
		zlog_err("indexs or indexd is  NULL");
		return;	
	}
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "Entering the function of '%s'--the line of %d",__func__,__LINE__);


	max_leases = pool_node->dhcp_pool.total;
	start = pool_node->dhcp_pool.start;
	end = pool_node->dhcp_pool.end;	
	
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "Entering the function of '%s'--the line of %d",__func__,__LINE__);
	sprintf(file,"%s","/data/dat/ip_pool.dat");
	
	if(file[0] == '\0')
	{
		return;
	}
	if(!(fp = fopen(file, "r")))
	{
		DHCP_LOG_ERROR("Unable to open %s for reading", file);
		return;
	}
	
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "Entering the function of '%s'--the line of %d",__func__,__LINE__);
	//LOG(LOG_DEBUG, "i :%d ,max_leases:%d ,file :%s\n",i, max_leases,file);
	while(fread(&lease, sizeof(struct dhcpOfferedAddr), 1, fp) == 1) 
	{
	
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "Entering the function of '%s'--the line of %d",__func__,__LINE__);
		/* ADDME: is it a static lease */
		//LOG(LOG_DEBUG, "read lease ,ip:%08X \n", lease.yiaddr);
		if((lease.pool_index == pool_node->pool_index) && ntohl(lease.yiaddr) >= ntohl(start) && ntohl(lease.yiaddr) <= ntohl(end)  ) 
		{		
			i++;
			if(i> max_leases)
			{			
				DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "loading address overflow, pool:%d, count:%d \n",pool_node->pool_index, i);
				break;
			}

			pos = ntohl(lease.yiaddr)-ntohl(start);
			m = pos/8;
			n = pos%8;
			indexd[m] &= ~(0x01<<n) ; //reverse  flag			
			
			dhcp_addr_add_by_node(&lease);
		}
	}
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "loading address , pool:%d, count:%d \n",pool_node->pool_index, i);
	fclose(fp);
}
