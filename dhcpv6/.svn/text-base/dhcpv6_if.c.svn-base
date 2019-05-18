#include <sys/types.h>
#include <sys/socket.h>
#include <sys/queue.h>
#include <net/if.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include <assert.h>
#include <string.h>

#include <lib/queue.h>
#include <lib/thread.h>
#include <lib/command.h>
#include <lib/log.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/linklist.h>
#include <lib/syslog.h>
#include <lib/errcode.h>
#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>


#include "dhcp6.h"
#include "dhcpv6_if.h"
#include "dhcpv6_msg.h"
struct list dhcpv6_if_list = {0, 0, 0, 0, 0};
extern int dhcpv6_client_fsm(uint32_t ifindex, struct pkt_buffer *pkt, enum DHCPV6_EVENT type, uint32_t state, iatype_t ia_type);
int dhcpv6_if_add(struct dhcpv6_if *pif)
{
	DHCPV6_LOG_DEBUG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);

	if (NULL == pif)					//预防操作空指针
	{
		return 1;
	}

	listnode_add(&dhcpv6_if_list,  pif);
	
	return 0;
}

int dhcpv6_if_delete(uint32_t ifindex)
{
	DHCPV6_LOG_DEBUG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
	struct dhcpv6_if *pif = NULL;
	struct listnode *node;

	if(0 == ifindex)					
	{
		return 1;
	}
	
    for(node = dhcpv6_if_list.head ; node; node = node->next ,pif=NULL)
    {
		pif = listgetdata(node);

		if(pif && pif->ifindex == ifindex)
		{
			//pif->status = DISABLE;
			usleep(50);
			listnode_delete(&dhcpv6_if_list,pif);
			XFREE(MTYPE_IF,pif);	
			pif = NULL;
			break;
		}
	}
	
	DHCPV6_LOG_DEBUG("%s:leave the function of '%s',--the line of %d",__FILE__,__func__,__LINE__);

	return 0;
}

struct dhcpv6_if *dhcpv6_if_lookup(uint32_t ifindex)
{
	struct dhcpv6_if *pif = NULL;
	struct listnode *node;
	
	DHCPV6_LOG_DEBUG("%s:Entering the function of '%s',ifindex  %08X",__FILE__,__func__,ifindex);

    for(node = dhcpv6_if_list.head ; node; node = node->next,pif=NULL)
    {
		pif=listgetdata(node);
		if(pif && pif->ifindex == ifindex)
		{
			break;
		}
	}

	if(NULL == pif)
	{
		DHCPV6_LOG_DEBUG("%s:%s:fail to find dhcpv6_if entry according to ifindex of %08X",__FILE__,__func__,ifindex);
		return NULL;
	}
	else
	{
		DHCPV6_LOG_DEBUG("%s:%s:ok to find dhcpv6_if entry according to ifindex of %08X",__FILE__,__func__,ifindex);
		return pif;
	}
}

int dhcpv6_client_enable(int ifindex)
{
	struct dhcpv6_if *ifp = NULL;
	ifp = XMALLOC(MTYPE_IF, sizeof(struct dhcpv6_if));
	if(ifp == NULL)
	{
		DHCPV6_LOG_ERR("XMALLOC dhcpv6_if is failed!\n");
		return -1;
	}
	memset(ifp, 0, sizeof(struct dhcpv6_if));
	ifp->ifindex = ifindex;
	ifp->event.state = DHCP6S_INIT;
	ifp->ia.ia_type = IATYPE_NA;
	
	if(dhcpv6_if_add(ifp)){
		DHCPV6_LOG_ERR("This interface ifindex 0x%x add is failed!\n", ifindex);
		return 1;
	}

	dhcpv6_client_fsm(ifindex, NULL, DHCPV6_EVENT_FROM_CMD, DHCPV6_COMMAND_START, IATYPE_NA);

	DHCPV6_LOG_DEBUG("Info: interface dhcpv6 client enabled\n");
	
	return CMD_SUCCESS; 
}


int dhcpv6_client_disable(int ifindex)
{
	struct dhcpv6_if *ifp;
	DHCPV6_LOG_DEBUG("dhcpv6 client disable...\n");

	ifp = dhcpv6_if_lookup(ifindex);
	if (ifp == NULL) {
		DHCPV6_LOG_DEBUG("Can not find ifp by id!\n");
		return -1;
	}
	
	ifp->event.state = DHCP6S_RELEASE;
	dhcpv6_client_fsm(ifindex, NULL, DHCPV6_EVENT_FROM_CMD, DHCPV6_COMMAND_STOP, IATYPE_NA);
	dhcpv6_if_delete(ifindex);
	DHCPV6_LOG_DEBUG("Info: interface dhcpv6 client disabled\n");
	
	return CMD_SUCCESS; 
}

int dhcpv6_client_pd_enable(int ifindex)
{
	struct dhcpv6_if *ifp = NULL;
	ifp = XMALLOC(MTYPE_IF, sizeof(struct dhcpv6_if));
	if(ifp == NULL)
	{
		DHCPV6_LOG_ERR("XMALLOC dhcpv6_if is failed!\n");
		return -1;
	}
	memset(ifp, 0, sizeof(struct dhcpv6_if));
	ifp->ifindex = ifindex;
	ifp->event.state = DHCP6S_INIT;
	ifp->ia.ia_type = IATYPE_PD;
	
	if(dhcpv6_if_add(ifp)){
		DHCPV6_LOG_ERR("This interface ifindex 0x%x add is failed!\n", ifindex);
		return 1;
	}

	dhcpv6_client_fsm(ifindex, NULL, DHCPV6_EVENT_FROM_CMD, DHCPV6_COMMAND_START, IATYPE_PD);

	DHCPV6_LOG_DEBUG("Info: interface dhcpv6 client enabled\n");
	
	return CMD_SUCCESS; 
}

int dhcpv6_client_pd_disable(int ifindex)
{
	struct dhcpv6_if *ifp;
	DHCPV6_LOG_DEBUG("dhcpv6 client disable...\n");

	ifp = dhcpv6_if_lookup(ifindex);
	if (ifp == NULL) {
		DHCPV6_LOG_DEBUG("Can not find ifp by id!\n");
		return -1;
	}
	
	ifp->event.state = DHCP6S_RELEASE;
	dhcpv6_client_fsm(ifindex, NULL, DHCPV6_EVENT_FROM_CMD, DHCPV6_COMMAND_STOP, IATYPE_PD);
	dhcpv6_if_delete(ifindex);
	DHCPV6_LOG_DEBUG("Info: interface dhcpv6 client disabled\n");
	
	return CMD_SUCCESS; 
}

