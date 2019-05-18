/* dhcpclient.c
 *
 * udhcp DHCP client
 *
 * Russ Dill <Russ.Dill@asu.edu> July 2001
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
 
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#include <sys/time.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/sysinfo.h>

#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/linklist.h>
#include <lib/ifm_common.h>
#include <lib/module_id.h>
#include <lib/msg_ipc.h>
#include <lib/route_com.h>
#include <lib/prefix.h>
#include <lib/thread.h>
#include <lib/ifm_common.h>
#include <lib/errcode.h>
#include <lib/hptimer.h>


#include "dhcp_server.h"
#include "options.h"
#include "clientpacket.h"
#include "dhcp.h"
#include "dhcp_api.h"
#include "dhcp_snmp.h"
#include "dhcp_client.h"

extern struct thread_master *dhcp_master;
extern pthread_mutex_t dhcp_lock;

long uptime(void)
{
	struct sysinfo info;
	sysinfo(&info);
	return info.uptime;
}

//process sub-message in option 43 if needed (TLV format);
//NM IP 43/81 4 byte
//NM Trap Port	43/82 2 byte
//NM VLAN ID	43/83 2 byte
//NM VLAN  43/84 1byte 
struct vendorsub_message *get_vendor_suboption(uint8_t *message, int code)
{
 	struct vendorsub_message *sub_message;
	int i = 0;
	sub_message = XCALLOC(MTYPE_IF, sizeof(struct vendorsub_message));
	if(sub_message == NULL)
	{
		DHCP_LOG_ERROR("Error: There is no ram space\n");		  
		return NULL;
	}
	while( (message[i] == DHCP_VERDOR_SPEC_TRAPIP) || (message[i] == DHCP_VERDOR_SPEC_TRAPPORT)
	|| (message[i] == DHCP_VERDOR_SPEC_MGMTVLAN) || (message[i] == DHCP_VERDOR_SPEC_MGMTVLAN_VALID))
	{
		sub_message->optionptr =  message + i + 2;
		sub_message->length = message[i+1];
		if(message[i] == code)
		{
			break;
		}
		i = i + 1 + 1 + sub_message->length;
	}
	
	return sub_message;
 }

/*
 *函数：int dhcp_ip_address_set(uint32_t ifindex,uint32_t ip_addr,uint32_t mask,uchar del_flag, uchar zero)
 *参数说明：
 		   ifindex:接口索引
 		   ip_addr:配置ip地址
 		   mask:设置掩码
 		   del_flag:是否删除地址(是:TRUE 否:FALSE)
 		   zero:是否置零配置标志(是:TRUE 否:FALSE)
*/
int dhcp_ip_address_set(uint32_t ifindex,uint32_t ip_addr,uint32_t mask,uchar del_flag, uchar zero)
{
	struct ifm_l3 l3 ;	
	enum IFNET_EVENT event;
	int ret=0;
	struct in_addr mask_addr;	
	
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "dhcp_ip_address_set, ifindex:%d ,ip:%08X ,mask:%08X,del_flag:%d\n",ifindex ,ip_addr,mask,del_flag);

	memset(&l3,0,sizeof(struct ifm_l3));
	event = IFNET_EVENT_IP_DELETE;

	if(zero)
	{
		if(del_flag)
		{
			l3.ipv4_flag = IP_TYPE_INVALID;
		}
		else
		{
			l3.ipv4_flag = IP_TYPE_DHCP_ZERO;
		}
	}
	else
	{
		l3.ipv4_flag = IP_TYPE_DHCP;
	}
	
	if(!del_flag)
	{
		mask_addr.s_addr = mask;
		l3.ipv4[0].addr = ntohl(ip_addr);
		l3.ipv4[0].prefixlen = ip_masklen(mask_addr);
		//l3.ipv4_flag = IP_TYPE_DHCP;
		l3.vpn = 0;
		
		event = IFNET_EVENT_IP_ADD;
	}
	if(event == IFNET_EVENT_IP_DELETE) 
	{
		ret = ipc_send_msg_n2(&l3, sizeof(struct ifm_l3), 1, MODULE_ID_ROUTE, MODULE_ID_DHCP,
					IPC_TYPE_L3IF, event, IPC_OPCODE_EVENT, ifindex);
	}
	else 
	{
		//ret = ipc_send_reply_n2(&l3 ,sizeof(struct ifm_l3),1,MODULE_ID_ROUTE,MODULE_ID_DHCP,
		//	IPC_TYPE_L3IF,event, 1,ifindex, IPC_OPCODE_REPLY);
		
		ret = ipc_send_msg_n2(&l3, sizeof(struct ifm_l3), 1, MODULE_ID_ROUTE, MODULE_ID_DHCP,
					IPC_TYPE_L3IF, event, IPC_OPCODE_EVENT, ifindex);
	}

	if(ret < 0)
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "dhcp_ip_address_set error!! ifindex:%d ,ip:%08X ,mask:%08X,del_flag:%d, ret: %d\n",ifindex ,ip_addr,mask,del_flag,ret);
	}	
	return ret;
}

static int dhcp_route_msg_send(uint32_t gateway,uint32_t nm_ip,uint32_t mask, int del)
{
	struct route_entry route;
	struct in_addr netmask;
	int ret;

	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "%s:Entering the function of '%s', gateway %08X nm_ip %08X mask %08X",__FILE__,__func__, gateway, nm_ip, mask);

	netmask.s_addr = mask;

	route.prefix.type = INET_FAMILY_IPV4;
	route.prefix.addr.ipv4 = ntohl(nm_ip);
	route.prefix.prefixlen = ip_masklen(netmask);

	route.nhp_num = 1;
	route.nhp[0].nexthop.addr.ipv4 = ntohl(gateway);
	route.nhp[0].nhp_type = NHP_TYPE_IP;
	route.nhp[0].action =  NHP_ACTION_FORWARD;
	route.nhp[0].protocol =  ROUTE_PROTO_STATIC ;
	route.nhp[0].distance =  ROUTE_METRIC_STATIC;
	route.nhp[0].down_flag = LINK_DOWN;

	if(del == 1)
	{
		ret = ipc_send_msg_n2(&route, sizeof(struct route_entry), 1, MODULE_ID_ROUTE, MODULE_ID_DHCP,
								IPC_TYPE_ROUTE, 0, IPC_OPCODE_DELETE, 0);
	} 
	else 
	{
		ret = ipc_send_msg_n2(&route, sizeof(struct route_entry), 1, MODULE_ID_ROUTE, MODULE_ID_DHCP,
								IPC_TYPE_ROUTE, 0, IPC_OPCODE_ADD, 0);
	}
	
	return ret;
}
int dhcp_pkt_send_timer(void *arg)
{
	uint32_t ifindex;
	struct if_dhcp *pif=NULL;

	ifindex = (uint32_t)arg;
	
	pif = dhcp_if_lookup(ifindex);

	if(pif)
	{
		dhcp_client_fsm(pif, NULL, DHCP_EVENT_FROM_TIMER);
	}
	return 0;
}
void dhcp_state_to_str(int state,char *str)
{

	if(!str)
	{
		return;
	}
	
	switch(state)
	{
		case INIT_SELECTING:
			strcpy(str,"INIT_SELECTING");
		    break;			
		case REQUESTING:
			strcpy(str,"REQUESTING");
		    break;
		case BOUND:
			strcpy(str,"BOUND");
			break;
		case RENEWING:
			strcpy(str,"RENEWING");
			break;
		case REBINDING:
			strcpy(str,"REBINDING");
			break;
		default:
			break;
	}
}

int dhcp_trap(void *arg)
{
	struct DHCP_TRAP trap;
	uint8_t online;
	struct if_dhcp *pif;
	uint32_t ifindex;
	
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "%s:Entering the function of '%s'",__FILE__,__func__);
	
	ifindex = (uint32_t)arg;
	pif = dhcp_if_lookup(ifindex);

	if(!pif || !pif->c_ctrl)
	{
		return 1;
	}
	
	online = pif->c_ctrl->online;

	trap.nm_info.nm_ip = pif->c_ctrl->trap_ip;
	trap.nm_info.nm_port = pif->c_ctrl->trap_port;
	trap.trap_info.series_id = 0x00;
	trap.trap_info.model_id  = 0x00;
	memcpy(trap.trap_info.mac,pif->mac,6);
	trap.trap_info.ip = pif->c_ctrl->ip;
	trap.trap_info.mask = pif->c_ctrl->mask;
	trap.trap_info.gateway = pif->c_ctrl->gate;
	
	if(trap.nm_info.nm_ip && trap.nm_info.nm_port)
	{
		if(online)
			ipc_send_msg_n2(&trap, sizeof(struct DHCP_TRAP), 1, MODULE_ID_SNMPD, MODULE_ID_DHCP,
								IPC_TYPE_SNMP_TRAP, 0, IPC_OPCODE_ADD, 0);
		else
			ipc_send_msg_n2(&trap, sizeof(struct DHCP_TRAP), 1, MODULE_ID_SNMPD, MODULE_ID_DHCP,
								IPC_TYPE_SNMP_TRAP, 0, IPC_OPCODE_DELETE, 0);
	}

	if(!online)		
		memset(pif->c_ctrl,0,sizeof(struct client_ctrl));

	return 0;

}

void *dhcp_zero_send(void *arg)
{
	uint32_t ifindex = 0;
	struct if_dhcp *pif = NULL;
	struct option124_vlan_ifvalid option124_vlan;
	uint32_t i = 0;
	uint32_t packet_num = 0;

	/* 设置本线程对信号的反应,收到cancel信号马上设置退出状态 */
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	/* 只有在PTHREAD_CANCEL_ENABLE状态下有效,PTHREAD_CANCEL_ASYNCHRONOUS 立即执行取消信号 */
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
	
	ifindex = (uint32_t)arg;
	pif = dhcp_if_lookup(ifindex);
	if (pif == NULL || (pif != NULL && pif->c_ctrl == NULL)) {
		zlog_err("%s:%s find ifindex is failed or pif->c_ctrl is NULL!", __FILE__, __FUNCTION__);
		return NULL;
	}
	while(1) {
		DHCP_LOG_ERROR("dhcp send unvlan packet");
		for (packet_num = 0; packet_num < 3; packet_num++) {
			if (pif == NULL || (pif != NULL && pif->c_ctrl == NULL)) {
				zlog_err("%s:%s pif or pif->c_ctrl is NULL!", __FILE__, __FUNCTION__);
				return NULL;
			}
			if (packet_num == 0) {
				pif->c_ctrl->xid = random_xid();
			}
			/* send discover packet */
			option124_vlan.vlanid  = 0;
			option124_vlan.ifvalid = 1;
			
			send_discover(pif, pif->c_ctrl->xid, 0, option124_vlan); /* broadcast */
			usleep(3000);
		} 
		sleep(5);
		DHCP_LOG_ERROR("dhcp send vlan packet");
		for (i = DHCP_MIN_VLAN; i <= DHCP_MAX_VLAN; i++) {
			option124_vlan.vlanid  = i;
			option124_vlan.ifvalid = 0;
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "%s:%s vlanid %d!", __FILE__, __FUNCTION__, i);
			if (pif == NULL || (pif != NULL && pif->c_ctrl == NULL)) {
				zlog_err("%s:%s pif or pif->c_ctrl is NULL!", __FILE__, __FUNCTION__);
				return NULL;
			}
			send_discover(pif, pif->c_ctrl->xid, 0, option124_vlan);
			usleep(3000);
			//sleep(5);
		}
		if (i > DHCP_MAX_VLAN) {
			sleep(3600);
		}
	}
}

pthread_mutex_t dhcp_zero_lock;
pthread_t pthread_zero;

uint32_t dhcp_cancel_all_if_zero_pthread(void)
{
	struct if_dhcp *pif = NULL;
	struct listnode *node;
	uint32_t ifindex_sub = 0;
	
    for(node = dhcp_if_list.head ; node; node = node->next,pif=NULL)
    {
		pif=listgetdata(node);
		if(pif && pif->ifindex && pif->c_ctrl && pif->pthread_zero)
		{
			pthread_cancel(pif->pthread_zero);
			/* dhcp进程挂起删除曾经创建的子接口 */
			if(pif->c_ctrl->ifvalid == 1 && pif->c_ctrl->vlanid)
			{
				ifindex_sub = IFM_SUBIFINDEX_GET(pif->ifindex, pif->c_ctrl->vlanid);
				if(ifindex_sub)
				{
					delete_zero_if(ifindex_sub);
				}
			}
		}
	}

	return 0;
}


/* SIGHUP handler. */
static void sighup_zero (int sig)
{
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "SIGHUP received");
	dhcp_cancel_all_if_zero_pthread();
}
int dhcp_zero(uint32_t ifindex)
{
    prctl(PR_SET_NAME, "dhcp_zero"); 

	int err = 0;
	struct if_dhcp *pif = NULL;
		
    pthread_mutex_init(&dhcp_zero_lock, NULL);

	pif = dhcp_if_lookup(ifindex);
	if(pif == NULL)
	{	
		DHCP_LOG_ERROR("find ifindex:%x is failed!", ifindex);	
		return 1;
	}
	
	/* 物理接口也需把信息发给route,但只发送zero标签 */
	
	if(dhcp_ip_address_set(pif->ifindex, 0, 0, 0, 1))
	{
		DHCP_LOG_ERROR("set ifindex:%x dhcp zero flag fail!", ifindex);	
	}
	err = pthread_create(&(pif->pthread_zero), NULL, dhcp_zero_send, (void *)ifindex);
	if(err != 0)
	{
		perror("write pthread_create fail:");
		exit(-1);
	}
	/* 主进程退出后零配置子线程退出 */
	signal(SIGHUP, sighup_zero);
    pthread_detach(pif->pthread_zero);
	return 0;
}

int dhcp_zero_cancel(uint32_t ifindex)
{
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "Info: cancel dhcp zero thread!\n");
	struct if_dhcp *pif = NULL;
		
	pif = dhcp_if_lookup(ifindex);
	if(pif == NULL)
	{	
		DHCP_LOG_ERROR("find ifindex:%x is failed!", ifindex);	
		return 1;
	}
	if (pif->pthread_zero == 0) {
		DHCP_LOG_ERROR("pthread_zero is not create\n");
		return 1;
	}
	#if 0
	int kill_rc = pthread_kill(pthread_zero,0);

	if(kill_rc == ESRCH)//线程不存在
	{
		LOG (LOG_ERR, "the specified thread did not exists or already quit/n");
		return 1;
	}else if(kill_rc == EINVAL) {//信号不合法
		LOG (LOG_ERR, "signal is invalid/n");
		return 1;
	}else{
		LOG (LOG_ERR, "the specified thread is alive/n");
	}
	#endif
	
	pthread_cancel(pif->pthread_zero);
	pthread_join(pif->pthread_zero,NULL);
	return 0;
}

int delete_zero_if(uint32_t ifindex)
{
	int ret = -1;
	ret = ipc_send_msg_n2(NULL, 0, 0, MODULE_ID_IFM, MODULE_ID_DHCP,
					IPC_TYPE_IFM, 0, IPC_OPCODE_DELETE, ifindex);
	return ret;
}
int dhcp_client_zero_disable(int ifindex)
{
	struct if_dhcp *pif=NULL;
	//enum DHCP_ROLE role;
	//int zero_status = -1;
	uint32_t zero_index = 0;
	struct if_dhcp *pif_save = NULL;

	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "Info: interface dhcp client zero disabled\n");
	
	pif_save = dhcp_if_save_lookup(ifindex);
	if(pif_save){
		DHCP_LOG_ERROR("Info: interface address already save\n");
		return ERRNO_EXISTED;
	}
	pif = dhcp_if_lookup(ifindex);
	
	if (pif == NULL) {
		DHCP_LOG_ERROR("This interface 0x%x is not alive", ifindex);
		return ERRNO_NOT_FOUND;
	}
	
	//zero_status = dhcp_zero_cancel();
	dhcp_zero_cancel(ifindex);
	/*
	if (zero_status) {
		LOG (LOG_ERR, "cancel dhcp zero is failed!\n");
		dhcp_if_delete(ifindex);
		return 1;
	}
	*/
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "Info: pthread_zero is exit\n");
	if(pif->c_ctrl)
	{
		if(pif->c_ctrl->gate != 0)
		{
			dhcp_route_msg_send(pif->c_ctrl->gate,0,0,1);
		}
		if((pif->c_ctrl->server_addr != 0) && (pif->c_ctrl->ip != 0))
		{
			send_release(pif, pif->c_ctrl->server_addr, pif->c_ctrl->ip);
		}
		/* delete vlanid interface ip address */
		dhcp_ip_address_set(pif->ifindex,pif->c_ctrl->ip,pif->c_ctrl->mask, TRUE, TRUE);

		/* delete vlanid interface */
		if (pif->c_ctrl->vlanid) 
		{
			zero_index = IFM_SUBIFINDEX_GET(ifindex, pif->c_ctrl->vlanid);
			if(zero_index)
			{
				delete_zero_if(zero_index);
			}
		}
		XFREE(MTYPE_IF,pif->c_ctrl);
		pif->c_ctrl = NULL;
	}
	
	pthread_mutex_lock(&dhcp_lock);
	dhcp_if_delete(ifindex);
	pthread_mutex_unlock(&dhcp_lock);
	usleep(1000);

	return ERRNO_SUCCESS;
}

int dhcp_client_fsm(struct if_dhcp *pif, struct pkt_buffer *pkt, enum DHCP_EVENT enType)
{
	uint8_t *temp, *pmsg_type,*pvalue;
	uint32_t *t1 = NULL, *t2 = NULL, *xid = NULL;
	uint32_t *start = NULL, *lease=NULL;
	struct dhcpMessage *pdhcp_msg=NULL;
	long now;
	int flag = DISABLE;/* 地址set是否成功 */

	struct option124_vlan_ifvalid option124_vlan;

	static int *state;
	static uint32_t *requested_ip; 
	static uint32_t *tmp_pointer; 
	static uint16_t *tmp_pointer_u16; 
	static uint32_t *server_addr;
	static uint32_t *packet_num; 
	struct timeval *timeout;	

	//uint32_t tmp_ip;
	//struct thread t;
	
	int request_flag = DISABLE; 
	uint32_t router = 0; 
	//uint8_t netmasklen;
	//struct in_addr netmask;
	struct if_dhcp *pif_save = NULL;

	uint32_t ip_save = 0;
	uint32_t mask_save = 0;
	uint32_t ifindex_save = 0;
	uint32_t vlanid_save = 0;
	int ripc_ret = 0;
	if(!pif||pif->status == 0 ||pif->mode !=IFNET_MODE_L3)
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "%s:leave the function of '%s' because of status or mode",__FILE__,__func__);
	
		return 0;
	}
	zlog_err("%s:Entering the function of '%s',ifindex:0x%x",__FILE__,__func__, pif->ifindex);
	pif_save = dhcp_if_save_lookup(pif->ifindex);
	
	if(pif_save != NULL && (pif->dhcp_role == E_DHCP_CLIENT || pif->dhcp_role == E_DHCP_ZERO))
	{
		DHCP_LOG_ERROR("This ifindex 0x%x is always add dhcp_if_save_list\n", pif->ifindex);
		if(pif_save->c_ctrl)
		{
			ifindex_save = pif->ifindex;
			vlanid_save = pif_save->c_ctrl->vlanid;
			if(pif->c_ctrl == NULL)
			{
				/* 重启后dhcp_if_list链表中没有pif信息为空,需赋值show client才可以显示信息,才可以发送release*/
				/* 重启后pif_save无需强制转化大小端 */
				ip_save = ntohl(pif_save->c_ctrl->ip);
				mask_save = ntohl(pif_save->c_ctrl->mask);

				pif->c_ctrl = (struct client_ctrl*)XCALLOC(MTYPE_IF, sizeof(struct client_ctrl));
				if(NULL == pif->c_ctrl) 
				{
					DHCP_LOG_ERROR("Error: There is no ram space\n");		  
				
					return 0;
				}
				/* use to send release packet */
				state = &(pif->c_ctrl->state);
				(*state) = BOUND;
				pif->c_ctrl->ip = ntohl(pif_save->c_ctrl->ip);
				pif->c_ctrl->mask = ntohl(pif_save->c_ctrl->mask);
				pif->c_ctrl->server_addr = ntohl(pif_save->c_ctrl->server_addr);
				pif->c_ctrl->gate = ntohl(pif_save->c_ctrl->gate);
				pif->c_ctrl->dns = ntohl(pif_save->c_ctrl->dns);
				pif->c_ctrl->vlanid = pif_save->c_ctrl->vlanid;
				if(pif->c_ctrl->vlanid != 0)
				{
					pif->c_ctrl->ifvalid = 1;
				}
			}
			else
			{
				ip_save = pif_save->c_ctrl->ip;
				mask_save = pif_save->c_ctrl->mask;
			}
			if(pif->zero == ENABLE && vlanid_save)
			{
				/* 物理接口也需把信息发给route,但只发送zero标签 */
				if(dhcp_ip_address_set(ifindex_save, 0, 0, FALSE, TRUE))
				{
					DHCP_LOG_ERROR("set ifindex:%x dhcp zero flag fail!", ifindex_save);	
				}
				//add subif
				int zero_ret = -1;
				uint32_t zero_index = 0;
				zero_index = IFM_SUBIFINDEX_GET(ifindex_save, vlanid_save);
				if(zero_index)
				{
					zero_ret = ifm_set_subif(zero_index, vlanid_save, 0, MODULE_ID_DHCP);
					//set subif address
					if(zero_ret == 0)
					{
						dhcp_ip_address_set(zero_index, ip_save, mask_save, FALSE, TRUE);
					}
				}
			}
			else 
			{
				/* 如为零配置需添加dhcp标签 */
				if(pif->zero == ENABLE)
				{
					dhcp_ip_address_set(ifindex_save, ip_save, mask_save, FALSE, TRUE);
				}
				else
				{
					dhcp_ip_address_set(ifindex_save, ip_save, mask_save, FALSE, FALSE);
				}
			}
		}
		return 0;
	}
	if(pkt)
	{
		pdhcp_msg = pkt->data;		
	}

	if(pif->dhcp_role == E_DHCP_CLIENT || pif->dhcp_role == E_DHCP_ZERO)
	{	
		if(pif->c_ctrl==NULL)
		{
			pif->c_ctrl = (struct client_ctrl*)XCALLOC(MTYPE_IF, sizeof(struct client_ctrl));
			if(NULL == pif->c_ctrl) 
			{
				DHCP_LOG_ERROR("There is no ram space to pif->c_ctrl\n");	  
			
				return 0;
			}
			
			pif->c_ctrl->clientid[OPT_CODE] = DHCP_CLIENT_ID;
			pif->c_ctrl->clientid[OPT_LEN] = 7;
			pif->c_ctrl->clientid[OPT_DATA] = 1;

			pif->c_ctrl->hostname[OPT_CODE] = DHCP_HOST_NAME;
			pif->c_ctrl->hostname[OPT_LEN] = 5;
			memcpy(pif->c_ctrl->hostname + 2,"ipran", 5);

			//pif->c_ctrl->timer_thread = NULL;
			pif->c_ctrl->timer = 0;

			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "allocate pif->c_ctrl success");			
			
		}				

		state = &(pif->c_ctrl->state);
		requested_ip = &(pif->c_ctrl->ip);
		server_addr  = &(pif->c_ctrl->server_addr);
		timeout = &(pif->c_ctrl->timeout);
		packet_num = &(pif->c_ctrl->packet_num);
		t1 = &(pif->c_ctrl->t1);
		t2 = &(pif->c_ctrl->t2);
		xid = &(pif->c_ctrl->xid);
		start = &(pif->c_ctrl->start);
		lease = &(pif->c_ctrl->lease);			
	}
	else if(pif->dhcp_role == E_DHCP_NOTHING)
	{
		if(pif->c_ctrl)
		{
			//dhcp_ip_address_set(pif->ifindex , 0,0,1);
			state = &(pif->c_ctrl->state);
			requested_ip = &(pif->c_ctrl->ip);
			server_addr  = &(pif->c_ctrl->server_addr);
			//uint8_t route_netmasklen;
			//struct in_addr route_netmask;
			//route_netmask.s_addr = pif->c_ctrl->mask;
			//route_netmasklen = ip_masklen(route_netmask);

			//zlog_err("%s:%s server_addr %x server_addr %x\n", __FILE__, __FUNCTION__, *server_addr, *requested_ip);
			//if(!ipv4_is_same_subnet(ntohl(requested_ip),ntohl(server_addr),route_netmasklen)) {
			//	zlog_err("%s:%s server_addr %x server_addr %x delete route\n", __FILE__, __FUNCTION__, *server_addr, *requested_ip);

				ripc_ret = dhcp_route_msg_send(pif->c_ctrl->gate,0,0,1);
				if(ripc_ret < 0)
				{
					DHCP_LOG_ERROR("dhcp send ipc to route message failed!\n");
				}
			//}
			if(*state == BOUND)
			{
				send_release(pif, *server_addr,*requested_ip);
			}
			
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "free pif->c_ctrl success");
			
			XFREE(MTYPE_IF,pif->c_ctrl);
			pif->c_ctrl = NULL;
		}
		return 0;
	}			
	else
	{
		return 0 ;
	}

	now = uptime();
	if(pdhcp_msg == NULL) 
	{
		if(enType != DHCP_EVENT_FROM_CMD && ((*timeout).tv_sec > now))
		{
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "time not enough, return");
		
			goto TIMER;
		}		
		
		switch((*state)) 
		{
			case INIT_SELECTING:
				if (pif->zero == ENABLE) {
					dhcp_zero(pif->ifindex);
					//pif->c_ctrl->xid = zero_xid;
					return 0;
				} else {
					if ((*packet_num) < 3) //old 3 aaaaaaaaaaaaaaaaafix
					{
						if ((*packet_num) == 0)
						{
							(*xid) = random_xid();
						}
						/* send discover packet */
						option124_vlan.vlanid  = 0;
						option124_vlan.ifvalid = 0;
						send_discover(pif, (*xid), (*requested_ip), option124_vlan); /* broadcast */
						//printf("send_discover utag\n");
						(*timeout).tv_sec = now + 5; // old 4:2 aaaaaaaaaaafix
						(*timeout).tv_usec = 0;
						(*packet_num)++;
						
					} 
					else 
					{
						/* wait to try again */
						(*packet_num) = 0;
						(*timeout).tv_sec = now + 60;
						(*timeout).tv_usec = 0;	
					}
				}
				break;				
			case RENEW_REQUESTED:
			case REQUESTING:	
				if((*packet_num) < 3) 
				{
					/* send request packet */
					if (pif->zero == ENABLE)
					{
						//如放在pif中会不会有问题
						if(pif->c_ctrl != NULL)
						{
							if (pif->c_ctrl->ifvalid == 0 && pif->c_ctrl->vlanid != 0)
							{
								option124_vlan.vlanid = pif->c_ctrl->vlanid;           //aaaaaaaaaaa,add by hqq
								option124_vlan.ifvalid = 0;
							} else {
								option124_vlan.vlanid	= 0;           //aaaaaaaaaaa,add by hqq
								option124_vlan.ifvalid	= 1;
							}
						}
					} else {
						option124_vlan.vlanid	= 0;           //aaaaaaaaaaa,add by hqq
						option124_vlan.ifvalid	= 0;
					}	
					send_selecting(pif, (*xid), (*server_addr), (*requested_ip), option124_vlan); /* broadcast */

					(*timeout).tv_sec = now + 5;  //old 10:2 aaaaaaaaaaafix
					(*timeout).tv_usec = 0;
					(*packet_num)++;
				} 
				else 
				{
					/* timed out, go back to init *state */
					(*state) = INIT_SELECTING;
					(*timeout).tv_sec = now;
					(*timeout).tv_usec = 0;
					(*packet_num) = 0;
				}
				break;
			case BOUND:
				/* Lease is starting to run out, time to enter renewing (*state) */
				(*state) = RENEWING;
				DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "Entering renew state");
				/* fall right through */
			case RENEWING:
				if(now >= (long)((*start)+(*t2)-2)) //time reach to t2
				{
					/* timed out, enter rebinding *state */
					(*state) = REBINDING;
					(*timeout).tv_sec = now;
					(*timeout).tv_usec = 0;
					DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "Entering rebinding state");
				} 
				else 
				{					
					send_renew(pif, (*xid), (*server_addr), (*requested_ip)); /* unicast */

					(*t1) = ((*t2) -(now-*start)) / 2 + (*t1);//((*t2) - (*t1)) / 2 + (*t1)
					(*timeout).tv_sec = (*t1) + (*start);
					(*timeout).tv_usec = 0;
					
				}
				break;
			case REBINDING:
				if(now >= (long)((*start)+(*lease)-2)) //time reach to lease
				{		
					if(pif->zero == ENABLE)
					{
						dhcp_ip_address_set(pif->ifindex , 0, 0, TRUE, TRUE); 	
					}
					else
					{
						dhcp_ip_address_set(pif->ifindex , 0, 0, TRUE, FALSE); 
					}
					//memset(pif->c_ctrl,0,sizeof(struct client_ctrl));
					
					/* timed out, enter init *state */
					//code maybe no use

					pif->c_ctrl->online = 0;
					dhcp_trap((void *)pif->ifindex);

					(*state) = INIT_SELECTING;
					(*timeout).tv_sec = now;
					(*timeout).tv_usec = 0;
					(*packet_num) = 0;	
					
					DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "Lease lost, entering init state");				
				} 
				else 
				{
					/* send a request packet */
					
					send_renew(pif, (*xid), 0, (*requested_ip)); /* broadcast */

					(*t2) = ((*lease) -(now-*start)) / 2 + (*t2);//((*lease) - (*t2)) / 2 + (*t2)
					(*timeout).tv_sec = (*t2) + (*start);
					(*timeout).tv_usec = 0;	
				}
				break;
			case RELEASED:
				/* yah, I know, *you* say it would never happen */
				(*timeout).tv_sec = 0x7fffffff;
				(*timeout).tv_usec = 0;
				break;
			default:
				DHCP_LOG_ERROR("This is unknown state!");
				return 0;
		}

		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "after send [ *state:%d ,timeout.tv_sec-now :%ld ] \n",(*state),(*timeout).tv_sec-now);
	} 
	else
	{
	
		if((pmsg_type = get_option(pdhcp_msg, DHCP_MESSAGE_TYPE)) == NULL) 
		{
			DHCP_LOG_ERROR("couldnt get option from packet -- ignoring");
			return 0;
		}

		if(pdhcp_msg->xid != (*xid)) 
		{
			DHCP_LOG_ERROR("Ignoring XID %lx (our xid is %x)",
				(unsigned long) pdhcp_msg->xid, (*xid));
			return 0;
		}

		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "dhcpc got a right dhcp packet,pmsg_type[%d]\n",*pmsg_type);

		switch((*state)) 
		{
			case INIT_SELECTING:
				/* Must be a DHCPOFFER to one of our xid's */
				if(*pmsg_type == DHCPOFFER) 
				{
					DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, " Received DHCP OFFER pkt->svlan %d", pkt->svlan);
				//if (pif->zero == 1 && pif->c_ctrl->vlanid == pkt->cb->ethcb.svlan) {
					if(pif->zero == ENABLE)
					{
						pthread_cancel(pif->pthread_zero);
						pthread_join(pif->pthread_zero,NULL);
						/* ???回来的vlan信息是否会被剥掉??? */
						pif->c_ctrl->vlanid = pkt->svlan;
						pif->c_ctrl->ifvalid = 0;
					}
					if((temp = get_option(pdhcp_msg, DHCP_SERVER_ID))) 
					{
						if(pif->c_ctrl && pif->c_ctrl->timer != 0)
						{
							//thread_cancel(pif->c_ctrl->timer_thread);
							high_pre_timer_delete(pif->c_ctrl->timer);
						}
						DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "dhcpc get server %d.%d.%d.%d\n",temp[0],temp[1],temp[2],temp[3]);
						memcpy(server_addr, temp, 4);
						(*xid) = pdhcp_msg->xid;
						(*requested_ip) = pdhcp_msg->yiaddr;

						/* enter requesting *state */
						(*state) = REQUESTING;
						(*timeout).tv_sec = now;
					    (*timeout).tv_usec = 0;
						(*packet_num) = 0;
					} 
					else 
					{
						DHCP_LOG_ERROR("No server ID in message");
						return 0;
					}
				}	/* end if *pmsg_type == DHCPOFFER */			
				else
				{
					return 0;
				}
				break;
			case RENEW_REQUESTED:
			case REQUESTING:
			case RENEWING:
			case REBINDING:
				if(*pmsg_type == DHCPACK) 
				{
					if(pif->c_ctrl && pif->c_ctrl->timer != 0)
					{
						//thread_cancel(pif->c_ctrl->timer_thread);	
						high_pre_timer_delete(pif->c_ctrl->timer);
					}
					if (!(temp = get_option(pdhcp_msg, DHCP_LEASE_TIME))) 
					{
						DHCP_LOG_ERROR("No lease time with ACK, using 1 hour lease");
						(*lease) = 60 * 60;
					} 
					else 
					{
						memcpy(lease, temp, 4);
						(*lease) = ntohl((*lease));
						//(*lease) = 30; //aaaaaaaaaaaaaaaaaaafix
						DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "lease time :%d s \n",(*lease));
					}

					/* enter bound *state */
					(*t1) = (*lease) / 2;

					/* little fixed point for n * .875 */
					(*t2) = ((*lease) * 0x7) >> 3;
					(*start) = now;
					(*timeout).tv_sec = (*t1) + (*start);
					(*timeout).tv_usec = 0;
					(*packet_num) = 0;	
					if((*requested_ip) != pdhcp_msg->yiaddr)
					{
						request_flag = ENABLE;
					}
					(*requested_ip) = pdhcp_msg->yiaddr;

					//trap info
					tmp_pointer = &(pif->c_ctrl->trap_ip); 		
					if(!(temp = get_option(pdhcp_msg, DHCP_VENDOR_SPEC_INFO))) 
					{						
						pif->c_ctrl->trap_ip 	= 0;
						pif->c_ctrl->trap_port 	= 0;
					} 
					else 
					{
						memcpy(tmp_pointer, temp+2, 4); //trap ip						
						
						tmp_pointer_u16 = &(pif->c_ctrl->trap_port);	
						memcpy(tmp_pointer_u16, temp+8, 2); //trap port

						pvalue = (uint8_t *)&(pif->c_ctrl->trap_ip);		
						DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "trap ip :%d.%d.%d.%d ,trap port:%d  \n",
							pvalue[0],pvalue[1],pvalue[2],pvalue[3],pif->c_ctrl->trap_port);						
						struct vendorsub_message *sub_message;
						uint16_t	  mgmtVid = 0;
						uint8_t 	  vlanifvalid = 0;

						sub_message = get_vendor_suboption(temp, DHCP_VERDOR_SPEC_MGMTVLAN);
						if(sub_message && sub_message->optionptr)
						{
							uint16_t vlanid = 0;
							memcpy(&vlanid, sub_message->optionptr, sub_message->length);
							mgmtVid = htons(vlanid);

							if(mgmtVid < DHCP_MIN_VLAN || mgmtVid > DHCP_MAX_VLAN)
							{
								DHCP_LOG_ERROR(" option43 vlanid is less than 1 or greater than 4094 ");
								mgmtVid = 1;
							}
							pif->c_ctrl->vlanid = mgmtVid;
						}
						sub_message = get_vendor_suboption(temp, DHCP_VERDOR_SPEC_MGMTVLAN_VALID);
						if(sub_message && sub_message->optionptr)
						{
							memcpy(&vlanifvalid, sub_message->optionptr, sub_message->length);
							pif->c_ctrl->ifvalid = vlanifvalid;
						}
						DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "vlanid :%d ,ifvalid:%d	\n", mgmtVid, vlanifvalid);
					}
					/* 自家设备上无option43 通过ack包vlan判断 */
					if(pif->c_ctrl->vlanid != 0 && pif->c_ctrl->ifvalid == 0)
					{
						if (pkt->svlan != 0 && pif->zero == ENABLE)
						{
							pif->c_ctrl->vlanid = pkt->svlan;
							pif->c_ctrl->ifvalid = 0;
						}
					}

					tmp_pointer = &(pif->c_ctrl->mask); 		
					if(!(temp = get_option(pdhcp_msg, DHCP_SUBNET))) 
					{
						DHCP_LOG_ERROR("No subnet with ACK, using  255.255.255.0");
						
						(*tmp_pointer)=0xFFFFFF00;
					} 
					else 
					{
						if(memcmp(tmp_pointer, temp, 4) != 0)
						{
							request_flag = ENABLE;
						}
						memcpy(tmp_pointer, temp, 4);
						DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "requested_mask :0X%08X  \n",ntohl(*tmp_pointer));
					}
					tmp_pointer =	&(pif->c_ctrl->gate); 		
					if(!(temp = get_option(pdhcp_msg, DHCP_ROUTER))) 
					{
						DHCP_LOG_ERROR("No gateway with ACK");
						
						(*tmp_pointer)=htonl((ntohl(*requested_ip) 
											& ntohl(pif->c_ctrl->mask)) +1);
					} 
					else 
					{
						memcpy(tmp_pointer, temp, 4);
						DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "requested_gate :0X%08X  \n",ntohl(*tmp_pointer));
						router = *tmp_pointer;
						DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "router :0X%08X  \n",ntohl(router));
					}

					tmp_pointer =	&(pif->c_ctrl->dns); 		
					if(!(temp = get_option(pdhcp_msg, DHCP_DNS_SERVER))) 
					{
						DHCP_LOG_ERROR("No dns with ACK");					
					} 
					else 
					{
						memcpy(tmp_pointer, temp, 4);
						DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "requested_dns :0X%08X  \n",ntohl(*tmp_pointer));
					}					
					/* 
					 *request_flag被赋值为1,(1)offer包与ack包中request_ip不同时,(2)pif->c_ctrl->mask
					 *只会在收到ack时被赋值，如pif->c_ctrl->mask与收到ack中掩码option不同会被赋值
					 */
					if(request_flag == ENABLE)
					{	
						if(pif->c_ctrl->vlanid != 0 && pif->c_ctrl->ifvalid != 1 && pif->zero == ENABLE) 
						{
							//add subif
							int zero_ret = -1;
							uint32_t zero_index = 0;
							zero_index = IFM_SUBIFINDEX_GET(pif->ifindex, pif->c_ctrl->vlanid);
							if(zero_index)
							{
								zero_ret = ifm_set_subif(zero_index, pif->c_ctrl->vlanid, 0, MODULE_ID_DHCP);
								//set subif address
								if(zero_ret == 0)
								{
									//flag = dhcp_ip_address_set_static(zero_index,(*requested_ip),(pif->c_ctrl->mask));
									flag = dhcp_ip_address_set(zero_index,(*requested_ip),(pif->c_ctrl->mask),FALSE, TRUE);
								}
							}
						}
						else
						{
							if(pif->zero)
							{
								flag = dhcp_ip_address_set(pif->ifindex, (*requested_ip), pif->c_ctrl->mask, FALSE, TRUE);	
							}
							else
							{
								flag = dhcp_ip_address_set(pif->ifindex,(*requested_ip),pif->c_ctrl->mask, FALSE, FALSE);
							}
						}
						if(flag == ENABLE)
						{
							send_release(pif, *server_addr, *requested_ip);
							/* return to init *state */
							if(pif->c_ctrl != NULL)
							{
								memset(pif->c_ctrl,0,sizeof(struct client_ctrl));
							}
							else
							{
								DHCP_LOG_ERROR("pif->c_ctr is null");
							}
							(*state) = INIT_SELECTING;
							(*timeout).tv_sec = now+5;
						}
					}/* end if request_flag == ENABLE */
					if(flag == DISABLE)
					{	
						//add static route to server if there is  giaddr
						if(pdhcp_msg->giaddr || ntohl(router))
						{
							//netmask.s_addr = pif->c_ctrl->mask;
							//netmasklen = ip_masklen(netmask);

							//tmp_ip = pif->c_ctrl->trap_ip?pif->c_ctrl->trap_ip:pif->c_ctrl->server_addr;
							//if(!ipv4_is_same_subnet(ntohl(router),ntohl(tmp_ip),netmasklen))
							{
								//dhcp_route_msg_send(pif->c_ctrl->gate,tmp_ip,pif->c_ctrl->mask);
								ripc_ret = 0;
								ripc_ret = dhcp_route_msg_send(pif->c_ctrl->gate,0,0,0);
								if(ripc_ret < 0)
								{
									DHCP_LOG_ERROR("dhcp send ipc message to route failed!\n");
								}
							}
						}
					
						(*state) = BOUND;
						DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "Entering bind state");

						//trap if ip from  NM
						if(pif->c_ctrl->trap_ip)
						{
							pif->c_ctrl->online = 1;
							high_pre_timer_add((char *)"DhcpSocketRcvTimer", LIB_TIMER_TYPE_LOOP, dhcp_trap, (void *)pif->ifindex,4);
						}
						
					}/* end if !flag*/
				}/* end if *pmsg_type == DHCPACK*/
				else if(*pmsg_type == DHCPNAK)
				{				
					if(pif->c_ctrl && pif->c_ctrl->timer != 0)
					{
						//thread_cancel(pif->c_ctrl->timer_thread);
						high_pre_timer_delete(pif->c_ctrl->timer);
					}
					/* return to init *state */
					DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "Received DHCP NAK");
					(*state) = INIT_SELECTING;
					(*timeout).tv_sec = now;
					(*timeout).tv_usec = 0;
					(*requested_ip) = 0;
					(*packet_num) = 0;
					sleep(3); /* avoid excessive network traffic */
				}
				else
				{
					return 0;
				}
				break;				
			default:
				DHCP_LOG_ERROR("This is unknown state!");
				return 0;
			/* case BOUND, RELEASED: - ignore all packets */
		}/* end switch(*state) */
	} /* end else */
TIMER:
	if((*timeout).tv_sec == now)
	{
	
    	//thread_add_event(dhcp_master, dhcp_pkt_send_timer, pif->ifindex, 0);
    	dhcp_client_fsm(pif,NULL,DHCP_EVENT_FROM_TIMER);
	}
	else if((*timeout).tv_sec > now)
	{
	
		pif->c_ctrl->timer = high_pre_timer_add((char *)"DhcpClientTimer", LIB_TIMER_TYPE_NOLOOP, &dhcp_pkt_send_timer,   (void *)(pif->ifindex), ((*timeout).tv_sec - now) * 1000);
		//(*timeout).tv_sec = (*timeout).tv_sec - now;

		//pif->c_ctrl->timer_thread = thread_add_timer(dhcp_master, dhcp_pkt_send_timer, (void *)pif->ifindex, (*timeout).tv_sec-now); 
	}
	return 0;
}
