/***********************************************************************
*
* pkt_arp.c
*
* manage the arp packet
*
***********************************************************************/

#include <string.h>
#include <lib/pkt_buffer.h>
#include <lib/pkt_type.h>
#include <lib/ether.h>
#include <lib/log.h>
#include <lib/ifm_common.h>
#include <lib/memtypes.h>
#include <lib/memory.h>
#include <lib/errcode.h>
#include <lib/inet_ip.h>
#include <lib/zassert.h>
#include <lib/alarm.h>
#include "lib/devm_com.h"
#include <route/arp_cmd.h>
#include "ftm_debug.h"
#include "ftm_ifm.h"
#include "ftm_nhp.h"
#include "ftm_fib.h"
#include "pkt_ip.h"
#include "pkt_eth.h"
#include "ftm_pkt.h"
#include "ftm_ifm.h"
#include "ftm.h"
#include "pkt_arp.h"
#include "ftm_arp.h"


/*arp 报文合法性检查函数*/
static uint8_t arp_pkt_check(struct arphdr *parph)
{
	if(ntohs(parph->hw_type) != 1) 
    {
        FTM_ARP_ZLOG_ERR("Arp packet hw_type check fail!\n");
        return ERRNO_FAIL;
    }

    if(ntohs(parph->pro_type) != ETH_P_IP)
    {
        FTM_ARP_ZLOG_ERR("Arp packet pro_type check fail!\n");
        return ERRNO_FAIL;
    }

    if(parph->hw_len != MAC_LEN)
    {
        FTM_ARP_ZLOG_ERR("Arp pcaket hw_len check fail!\n");
        return ERRNO_FAIL;
    }

    if(parph->pro_len != 4)
    {
        FTM_ARP_ZLOG_ERR("Arp pcaket pro_len check fail!\n");
        return ERRNO_FAIL;
    }

    if(ntohs(parph->opcode) > 4 || ntohs(parph->opcode) < 1)
    {
        FTM_ARP_ZLOG_ERR("Arp packet opcode check fail!\n");
        return ERRNO_FAIL;
    }

    if(!ipv4_is_valid(ntohl(parph->dip)))
    {
        FTM_ARP_ZLOG_ERR("Arp packet dip check fail!\n");
        return ERRNO_FAIL;
    }
    
    return ERRNO_SUCCESS;
}

/*添加半连接arp 表项*/
static void arp_add_incomplete(uint32_t ipaddr,uint16_t vpn,uint32_t ifindex,struct arp_entry *parp)
{
	int gratuious = 0;
	
	if (!parp) return;
	
	if(parp->gratuious)
		gratuious = parp->gratuious;

    memset(parp, 0, sizeof(struct arp_entry));
    parp->key.ipaddr = ipaddr;
    parp->key.vpnid = vpn;
	parp->ifindex = ifindex;						
	parp->status = ARP_STATUS_INCOMPLETE;
	parp->time = garp.fake_expire_time;
	parp->count = 0;
	parp->gratuious = gratuious;
	memset(parp->mac, 0, MAC_LEN);
    
	arp_add(parp, ARP_MASTER);	 
}

/*ip 冲突处理函数*/
uint8_t arp_ip_conflict(struct ftm_ifm *pif,struct arp_entry *parp)
{
    struct arp_entry *carp = NULL;
	uint8_t i = 0;
    
	struct gpnPortInfo gPortInfo;
    if(!pif || !parp)
        return ERRNO_FAIL;
    
	if (pif->pl3if->ipv4[0].addr == parp->key.ipaddr)
	{	
		i = 0;	
	}
	else if (pif->pl3if->ipv4[1].addr == parp->key.ipaddr)
	{	
		i = 1;	
	}
	else
	{	
		FTM_ARP_ZLOG_DEBUG("IP conflict! ip:0x%x ifidx:0x%x\n",parp->key.ipaddr,pif->ifm.ifindex);
		return ERRNO_FAIL;
    }

	FTM_ARP_ZLOG_DEBUG("conflict_ip:0x%x i:%d flag:%d\n",parp->key.ipaddr, i, pif->pl3if->conflict_flag[i]);
	
    /*第一次收到ip 冲突报文*/
    if((pif->pl3if->conflict_flag[i]) == 0)
    {
        FTM_ARP_ZLOG_DEBUG("Found ip conflict!ip:0x%x\n",parp->key.ipaddr);

        carp = XCALLOC(MTYPE_ARP_ENTRY,sizeof(struct arp_entry));
        if(!carp) return ERRNO_FAIL;  

		memcpy(carp,parp,sizeof(struct arp_entry));
        /*冲突标志位置位并启动冲突处理定时器*/
        pif->pl3if->conflict_flag[i] = 1;
        arp_add_incomplete(parp->key.ipaddr,parp->key.vpnid,pif->ifm.ifindex,carp);
        arp_ip_conflict_process((void *)carp);
        //alarm_report(IPC_OPCODE_ADD,MODULE_ID_FTM,ALM_CODE_SFW_IP_COLLISION,carp->ifindex,0,0,0);//上报告警

		if(IFM_TYPE_IS_ETHERNET_PHYSICAL(carp->ifindex))
		{
			/*ipran_alarm_port_register(IFM_FUN_ETH_TYPE, IFM_SLOT_ID_GET(carp->ifindex),
				IFM_PORT_ID_GET(carp->ifindex), 0, 0);
				
			ipran_alarm_report(IFM_FUN_ETH_TYPE, IFM_SLOT_ID_GET(carp->ifindex),
				IFM_PORT_ID_GET(carp->ifindex), 0, 0, 
				GPN_ALM_TYPE_FUN_ETH_IP_COL, GPN_SOCK_MSG_OPT_RISE);*/
				
			memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
			gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
			gPortInfo.iIfindex = carp->ifindex;
			ipran_alarm_port_register(&gPortInfo);
			
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_ETH_IP_COL, GPN_SOCK_MSG_OPT_RISE);
		}
		else if(IFM_TYPE_IS_ETHERNET_SUBPORT(carp->ifindex))
		{
			/*ipran_alarm_port_register(IFM_FUN_ETH_SUB_TYPE, IFM_SLOT_ID_GET(carp->ifindex),
				IFM_PORT_ID_GET(carp->ifindex), 0, 0);

			ipran_alarm_report(IFM_FUN_ETH_SUB_TYPE, IFM_SLOT_ID_GET(carp->ifindex),
				IFM_PORT_ID_GET(carp->ifindex), 0, 0, 
				GPN_ALM_TYPE_FUN_ETH_SUB_IP_COL, GPN_SOCK_MSG_OPT_RISE);*/

			memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
			gPortInfo.iAlarmPort = IFM_FUN_ETH_SUB_TYPE;
			gPortInfo.iIfindex = carp->ifindex;
			ipran_alarm_port_register(&gPortInfo);
			
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_ETH_SUB_IP_COL, GPN_SOCK_MSG_OPT_RISE);
		}
		else if(IFM_TYPE_IS_TRUNK_PHYSICAL(carp->ifindex))
		{
			/*ipran_alarm_port_register(IFM_TRUNK_TYPE, IFM_SLOT_ID_GET(carp->ifindex),
				IFM_PORT_ID_GET(carp->ifindex), 0, 0);

			ipran_alarm_report(IFM_TRUNK_TYPE, IFM_SLOT_ID_GET(carp->ifindex),
				IFM_PORT_ID_GET(carp->ifindex), 0, 0, 
				GPN_ALM_TYPE_FUN_ETH_LAG_IP_COL, GPN_SOCK_MSG_OPT_RISE);*/

			memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
			gPortInfo.iAlarmPort = IFM_TRUNK_TYPE;
			gPortInfo.iIfindex = carp->ifindex;
			ipran_alarm_port_register(&gPortInfo);
			
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_ETH_LAG_IP_COL, GPN_SOCK_MSG_OPT_RISE);
		}
		else if(IFM_TYPE_IS_TRUNK_SUBPORT(carp->ifindex))
		{
			/*ipran_alarm_port_register(IFM_TRUNK_SUBIF_TYPE, IFM_SLOT_ID_GET(carp->ifindex),
				IFM_PORT_ID_GET(carp->ifindex), IFM_SUBPORT_ID_GET(carp->ifindex), 0);

			ipran_alarm_report(IFM_TRUNK_SUBIF_TYPE, IFM_SLOT_ID_GET(carp->ifindex),
				IFM_PORT_ID_GET(carp->ifindex), IFM_SUBPORT_ID_GET(carp->ifindex), 0, 
				GPN_ALM_TYPE_FUN_ETH_LAG_SUB_IP_COL, GPN_SOCK_MSG_OPT_RISE);*/

			memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
			gPortInfo.iAlarmPort = IFM_TRUNK_SUBIF_TYPE;
			gPortInfo.iIfindex = carp->ifindex;
			ipran_alarm_port_register(&gPortInfo);
			
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_ETH_LAG_SUB_IP_COL, GPN_SOCK_MSG_OPT_RISE);
		}
		else if(IFM_TYPE_IS_VLANIF(carp->ifindex))
		{
			/*ipran_alarm_port_register(IFM_VLAN_OAM_TYPE, IFM_SLOT_ID_GET(carp->ifindex),
				IFM_PORT_ID_GET(carp->ifindex), 0, 0);

			ipran_alarm_report(IFM_FUN_ETH_TYPE, IFM_SLOT_ID_GET(carp->ifindex),
				IFM_PORT_ID_GET(carp->ifindex), 0, 0, 
				GPN_ALM_TYPE_FUN_VLANIF_IP_COL, GPN_SOCK_MSG_OPT_RISE);*/
				
			memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
			gPortInfo.iAlarmPort = IFM_VLAN_OAM_TYPE;
			gPortInfo.iIfindex = carp->ifindex;
			ipran_alarm_port_register(&gPortInfo);
			
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_VLANIF_IP_COL, GPN_SOCK_MSG_OPT_RISE);
		}
		else if(IFM_TYPE_IS_TUNNEL(carp->ifindex))
		{
			/*ipran_alarm_port_register(IFM_TUNNEL_TYPE, IFM_SLOT_ID_GET(carp->ifindex),
				IFM_PORT_ID_GET(carp->ifindex), 0, 0);

			ipran_alarm_report(IFM_TUNNEL_TYPE, IFM_SLOT_ID_GET(carp->ifindex),
				IFM_PORT_ID_GET(carp->ifindex), 0, 0, 
				GPN_ALM_TYPE_TUNNEL_IP_COL, GPN_SOCK_MSG_OPT_RISE);*/
				
			memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
			gPortInfo.iAlarmPort = IFM_TUNNEL_TYPE;
			gPortInfo.iIfindex = carp->ifindex;
			ipran_alarm_port_register(&gPortInfo);
			
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_TUNNEL_IP_COL, GPN_SOCK_MSG_OPT_RISE);
		}
    }
    else
    {
        /*重复收到冲突ip 的免费arp 报文时，复位冲突免费arp发送计数*/
        carp = arp_lookup(parp->key.ipaddr,parp->key.vpnid); 
        if(!carp) return ERRNO_FAIL;
        carp->time = ARP_TIMER*3;
        carp->count = 0;
		carp->gratuious_count++;
    }

    return ERRNO_SUCCESS;
}


/*ip冲突加定时器以5s 间隔发送免费arp 冲突报文，同时冲突标志位置位、上报告警
5 个周期内没收到免费arp 则冲突消除；冲突标志位复位告警消除定时器取消。*/
int arp_ip_conflict_process(void *arg)
{
    struct ftm_ifm *pifm = NULL;
    struct arp_entry *carp = NULL;
    struct arp_entry *parp = NULL;//hash存储的半连接表项
    uint32_t cip = 0;
    uint8_t smac[6];
    uint8_t flag = 3;

    carp = (struct arp_entry *)arg;
	pifm = ftm_ifm_lookup(carp->ifindex);
    if(!pifm || !(pifm->pl3if))
    {
    	goto CLR;
		//return;
    }
	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	cip = carp->key.ipaddr;
	if (ipv4_is_same_subnet(pifm->pl3if->ipv4[0].addr, cip, pifm->pl3if->ipv4[0].prefixlen))
	{  flag = 0;  }
	else if (ipv4_is_same_subnet(pifm->pl3if->ipv4[1].addr, cip, pifm->pl3if->ipv4[1].prefixlen))
	{  flag = 1;  }
	else
	{	FTM_ARP_ZLOG_DEBUG("IP conflict process! ip:0x%x ifidx:0x%x\n",carp->key.ipaddr,pifm->ifm.ifindex);
		goto CLR;
		//return;
    };
	
    /*发送冲突ip 免费arp*/
    FTM_IFM_GET_MAC(pifm, smac);
	arp_send(carp, ARP_OPCODE_GRATUITOUS_REPLY, carp->key.ipaddr, smac);
	//arp_send(carp, ARP_OPCODE_REQUEST, carp->key.ipaddr, smac);
    
    /*3个周期内没有收到冲突ip 的免费arp则冲突消除*/
    parp = arp_lookup(carp->key.ipaddr,carp->key.vpnid);
    if(parp && (parp->count < 5) && (pifm->pl3if->ipv4[flag].addr == carp->key.ipaddr))
    { 
        parp->count ++;
        FTM_ARP_ZLOG_DEBUG("parp->key.ipaddr:0x%x parp->count:%d\n",parp->key.ipaddr,parp->count);

		if(parp->gratuious_count < 5)
			high_pre_timer_add("ArpConflictTimer",LIB_TIMER_TYPE_NOLOOP,arp_ip_conflict_process,(void *)carp,1*1000);
		else
			high_pre_timer_add("ArpConflictTimer",LIB_TIMER_TYPE_NOLOOP,arp_ip_conflict_process,(void *)carp,5*1000);
    }
    else
    {
CLR:    
        FTM_ARP_ZLOG_DEBUG("Conflict clear! ip:0x%x\n",carp->key.ipaddr);
        //alarm_report(IPC_OPCODE_CLEAR,MODULE_ID_FTM,ALM_CODE_SFW_IP_COLLISION,carp->ifindex,0,0,0);//告警消除
        
		if(IFM_TYPE_IS_ETHERNET_PHYSICAL(carp->ifindex))
		{
			/*ipran_alarm_report(IFM_FUN_ETH_TYPE, IFM_SLOT_ID_GET(carp->ifindex),
				IFM_PORT_ID_GET(carp->ifindex), 0, 0, 
				GPN_ALM_TYPE_FUN_ETH_IP_COL, GPN_SOCK_MSG_OPT_CLEAN);

			ipran_alarm_port_unregister(IFM_FUN_ETH_TYPE, IFM_SLOT_ID_GET(carp->ifindex),
				IFM_PORT_ID_GET(carp->ifindex), 0, 0);*/

			memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
			gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
			gPortInfo.iIfindex = carp->ifindex;			
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_ETH_IP_COL, GPN_SOCK_MSG_OPT_CLEAN);
			
			//ipran_alarm_port_unregister(&gPortInfo);
		}
		else if(IFM_TYPE_IS_ETHERNET_SUBPORT(carp->ifindex))
		{
			/*ipran_alarm_report(IFM_FUN_ETH_SUB_TYPE, IFM_SLOT_ID_GET(carp->ifindex),
				IFM_PORT_ID_GET(carp->ifindex), 0, 0, 
				GPN_ALM_TYPE_FUN_ETH_SUB_IP_COL, GPN_SOCK_MSG_OPT_CLEAN);
			ipran_alarm_port_unregister(IFM_FUN_ETH_SUB_TYPE, IFM_SLOT_ID_GET(carp->ifindex),
				IFM_PORT_ID_GET(carp->ifindex), 0, 0);*/

			memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
			gPortInfo.iAlarmPort = IFM_FUN_ETH_SUB_TYPE;
			gPortInfo.iIfindex = carp->ifindex;			
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_ETH_SUB_IP_COL, GPN_SOCK_MSG_OPT_CLEAN);			
			//ipran_alarm_port_unregister(&gPortInfo);			
		}
		else if(IFM_TYPE_IS_TRUNK_PHYSICAL(carp->ifindex))
		{
			/*ipran_alarm_report(IFM_TRUNK_TYPE, IFM_SLOT_ID_GET(carp->ifindex),
				IFM_PORT_ID_GET(carp->ifindex), 0, 0, 
				GPN_ALM_TYPE_FUN_ETH_LAG_IP_COL, GPN_SOCK_MSG_OPT_CLEAN);
			ipran_alarm_port_unregister(IFM_TRUNK_TYPE, IFM_SLOT_ID_GET(carp->ifindex),
				IFM_PORT_ID_GET(carp->ifindex), 0, 0);	*/

			memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
			gPortInfo.iAlarmPort = IFM_TRUNK_TYPE;
			gPortInfo.iIfindex = carp->ifindex;			
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_ETH_LAG_IP_COL, GPN_SOCK_MSG_OPT_CLEAN);
			//ipran_alarm_port_unregister(&gPortInfo);			
		}
		else if(IFM_TYPE_IS_TRUNK_SUBPORT(carp->ifindex))
		{
			/*ipran_alarm_report(IFM_TRUNK_SUBIF_TYPE, IFM_SLOT_ID_GET(carp->ifindex),
				IFM_PORT_ID_GET(carp->ifindex), IFM_SUBPORT_ID_GET(carp->ifindex), 0, 
				GPN_ALM_TYPE_FUN_ETH_LAG_SUB_IP_COL, GPN_SOCK_MSG_OPT_CLEAN);
			ipran_alarm_port_unregister(IFM_TRUNK_SUBIF_TYPE, IFM_SLOT_ID_GET(carp->ifindex),
				IFM_PORT_ID_GET(carp->ifindex), IFM_SUBPORT_ID_GET(carp->ifindex), 0);*/

			memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
			gPortInfo.iAlarmPort = IFM_TRUNK_SUBIF_TYPE;
			gPortInfo.iIfindex = carp->ifindex;			
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_ETH_LAG_SUB_IP_COL, GPN_SOCK_MSG_OPT_CLEAN);
			//ipran_alarm_port_unregister(&gPortInfo);						
		}
		else if(IFM_TYPE_IS_VLANIF(carp->ifindex))
		{
			/*ipran_alarm_report(IFM_FUN_ETH_TYPE, IFM_SLOT_ID_GET(carp->ifindex),
				IFM_PORT_ID_GET(carp->ifindex), 0, 0, 
				GPN_ALM_TYPE_FUN_VLANIF_IP_COL, GPN_SOCK_MSG_OPT_CLEAN);
			ipran_alarm_port_unregister(IFM_VLAN_OAM_TYPE, IFM_SLOT_ID_GET(carp->ifindex),
				IFM_PORT_ID_GET(carp->ifindex), 0, 0);*/

			memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
			gPortInfo.iAlarmPort = IFM_VLAN_OAM_TYPE;
			gPortInfo.iIfindex = carp->ifindex;			
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_VLANIF_IP_COL, GPN_SOCK_MSG_OPT_CLEAN);
			//ipran_alarm_port_unregister(&gPortInfo);						
		}
		else if(IFM_TYPE_IS_TUNNEL(carp->ifindex))
		{
			/*ipran_alarm_report(IFM_TUNNEL_TYPE, IFM_SLOT_ID_GET(carp->ifindex),
				IFM_PORT_ID_GET(carp->ifindex), 0, 0, 
				GPN_ALM_TYPE_TUNNEL_IP_COL, GPN_SOCK_MSG_OPT_CLEAN);
			ipran_alarm_port_unregister(IFM_TUNNEL_TYPE, IFM_SLOT_ID_GET(carp->ifindex),
				IFM_PORT_ID_GET(carp->ifindex), 0, 0);*/

			memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
			gPortInfo.iAlarmPort = IFM_TUNNEL_TYPE;
			gPortInfo.iIfindex = carp->ifindex;			
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_TUNNEL_IP_COL, GPN_SOCK_MSG_OPT_CLEAN);
			//ipran_alarm_port_unregister(&gPortInfo);						
		}
		
		if(pifm != NULL && pifm->pl3if != NULL)
		{
        	pifm->pl3if->conflict_flag[flag] = 0;
		}	
        arp_delete(carp->key.ipaddr,carp->key.vpnid, ARP_MASTER);
		
        XFREE(MTYPE_ARP_ENTRY,carp);
    }
	return 0;
}


/*arp 收包函数*/
int arp_rcv(struct pkt_buffer *pkt, struct ftm_ifm *pifm)
{
    struct eth_control *pethcb = NULL;
    struct arphdr *parph = NULL;
	struct arp_entry src_arp;
	uint32_t ifindex = 0;
	uint32_t port = 0;	
    uint8_t  event_type = 0;

	if((pifm == NULL) || (NULL == pifm->parp))
		goto drop;
    
    FTM_ARP_ZLOG_DEBUG("in_ifindex:0x%x port:0x%x pifm->ifm.ifindex:0x%x\n",
                        pkt->in_ifindex,pkt->in_port,pifm->ifm.ifindex);
    
    if(pkt->data_len < sizeof(struct arphdr)) 
        goto drop;

    /*if physical intf recv packet with svlan,then drop it.*/
    pethcb = (struct eth_control *)(&(pkt->cb));
    if(IFM_TYPE_IS_PHYSICAL(pkt->in_ifindex) 
        && (pethcb && (pethcb->svlan != 0)))
    {
        FTM_PKT_ZLOG_DBG("pethcb->svlan:%d pifm->vlan_start:%d\n",
                pethcb->svlan,(pifm)?(pifm->ifm.encap.svlan.vlan_start):0);
        goto drop;
    }

    ftm_pkt_dump(pkt->data, pkt->data_len, PKT_ACTION_RECV, PKT_TYPE_ETH);

	/* 判断是否开启 arp 学习 */
	if ((pifm->parp == NULL)||(pifm->parp->arp_disable == 0))
		goto drop;

	/* 报文字段合法性检查 */
    parph = (struct arphdr *)(pkt->data);
	if(arp_pkt_check(parph) == ERRNO_FAIL)
		goto drop;

    /*报文事件类型获取*/
    if (ntohs(parph->opcode) == ARP_OPCODE_REQUEST)
    {
    	if(parph->dip == parph->sip)
			event_type = ARP_RCV_GRATUITOUS;
		else
        	event_type = ARP_RCV_REQUEST;
    }
    else if (ntohs(parph->opcode) == ARP_OPCODE_REPLY)
    {
    	if(parph->dip == parph->sip)
			event_type = ARP_RCV_GRATUITOUS;
		else
        event_type = ARP_RCV_REPLY;
    }

	/* 获取 arp 的入接口, vlanif 学习物理成员接口 */
	ifindex = pkt->in_ifindex;
 	if(IFM_TYPE_IS_VLANIF(ifindex))
		port = pkt->in_port;

    memset(&src_arp, 0, sizeof(struct arp_entry));
	src_arp.ifindex = ifindex;	
	src_arp.port = port;
	src_arp.key.ipaddr = ntohl(parph->sip);
	src_arp.key.vpnid = pkt->vpn;
    memcpy(src_arp.mac, parph->smac, 6);
	if(event_type == ARP_RCV_GRATUITOUS)
		src_arp.gratuious = 1;

    arp_fsm(&src_arp,ntohl(parph->dip),event_type,pifm);

	pkt_free(pkt);
    return NET_SUCCESS;
    
drop:	
	FTM_ARP_ZLOG_DEBUG("Arp Rcv failed, drop the packet!pkt->in_ifindex:0x%x pkt->in_port:0x%x\n",pkt->in_ifindex,pkt->in_port);	
	ftm_pkt_cnt_add(PKT_ACTION_DROP, PKT_TYPE_ETH);
	pkt_free(pkt);
	return NET_FAILED;
}

/*recv gratuitous arp*/
uint8_t arp_rcv_gratuitous(struct ftm_ifm *pif,struct arp_entry *psrc_arp)
{
    struct arp_entry parp1;
    struct arp_entry *parp = NULL;
    struct ifm_l3 *l3if = NULL;
	uint32_t garp_ip = 0;
	uint8_t smac[6];
	uint8_t i = 0,flag = IFM_IP_NUM;
	
	if (!pif || !(pif->pl3if) || !psrc_arp)
		return ERRNO_FAIL;

    /*find intf's samesubnet ip*/
    l3if = pif->pl3if;
	garp_ip = psrc_arp->key.ipaddr;
    for (i = 0; i < IFM_IP_NUM; i++)
    {   
        if ((l3if->ipv4[i].addr) && 
            ipv4_is_same_subnet(l3if->ipv4[i].addr, garp_ip, l3if->ipv4[i].prefixlen))
        {
            flag = i;
            break;
        }

        /*there is no intf's ip samesubnet to gratuiatous arp*/
        if((IFM_IP_NUM - 1) == i)
        {
            FTM_ARP_ZLOG_DEBUG("Not this intf's gratuiatous arp!Drop it!\n");
            return ERRNO_FAIL;
        }
    }
    
    /*flag never equal to IFM_IP_NUM ;but......*/
    if(flag == IFM_IP_NUM)
    {
        FTM_ARP_ZLOG_ERR();
        return ERRNO_FAIL;
    }

	memset(&parp1, 0, sizeof(struct arp_entry));
    FTM_IFM_GET_MAC(pif,smac);
	
	/*if find arp entry,maybe peer's mac update,so send arp request;
     * if not ,add incomplete entry and send request to learn arp */
    parp = arp_lookup(psrc_arp->key.ipaddr,psrc_arp->key.vpnid);
    if(!parp)
    {
        arp_add_incomplete(psrc_arp->key.ipaddr,psrc_arp->key.vpnid,pif->ifm.ifindex,&parp1);
        arp_send(&parp1, ARP_OPCODE_REQUEST, pif->pl3if->ipv4[flag].addr, smac); 
    }
    else
    {
        arp_send(parp, ARP_OPCODE_REQUEST, pif->pl3if->ipv4[flag].addr, smac);     
    }

   /*recv peer's gratuitous arp,if local's releated arp is 
    * NUll or incomplete,response local intf's gratuitous arp*/
    if(!parp || (parp && parp->status == ARP_STATUS_INCOMPLETE))
    {
        /*avoid endless loop with peer*/
    	if ((pif->parp->arp_num_max == 0) || (garp.num_limit == 0)) 
			return ERRNO_FAIL;
		
        /*send gratuitous arp*/
        psrc_arp->key.ipaddr = pif->pl3if->ipv4[flag].addr;
        return arp_send(psrc_arp,ARP_OPCODE_REQUEST,pif->pl3if->ipv4[flag].addr,smac); 
    }

    return ERRNO_SUCCESS;

}


static uint8_t arp_fixed_check(void *parpold, void *parpnew)
{
    struct arp_entry *parp_old = parpold;
    struct arp_entry *parp_new = parpnew;
    if (parp_old->status == ARP_STATUS_INCOMPLETE)
        return ERRNO_SUCCESS;
    
    if (garp.fixed_flag == ARP_FIXED_ALL)
    {
        FTM_ARP_ZLOG_DEBUG();
        if (memcmp(parp_old->mac, parp_new->mac, MAC_LEN))
            return ERRNO_FAIL;
        if (parp_old->ifindex != parp_new->ifindex)
            return ERRNO_FAIL;
        if (parp_old->port != parp_new->port)
            return ERRNO_FAIL;
        
        FTM_ARP_ZLOG_DEBUG();
        return ERRNO_SUCCESS;
    }
    else if(garp.fixed_flag == ARP_FIXED_MAC)
    {
        FTM_ARP_ZLOG_DEBUG();
        if (memcmp(parp_old->mac, parp_new->mac, MAC_LEN))
            return ERRNO_FAIL;

        FTM_ARP_ZLOG_DEBUG();
        return ERRNO_SUCCESS;
    }
    else if(garp.fixed_flag == ARP_FIXED_SENDACK)
    {
         FTM_ARP_ZLOG_DEBUG();
    }
	else if(garp.fixed_flag == 0)
	{
		FTM_ARP_ZLOG_DEBUG("Arp entry fix is disable!\n");
	}

    return ERRNO_SUCCESS;
}



int arp_send(struct arp_entry *parp, uint8_t opcode, uint32_t sip, uchar *smac)
{
    struct  arphdr      *parph = NULL;
    struct  pkt_buffer  *pkt = NULL;
	struct  eth_control *p_ethcb = NULL;
    uint8_t mac_str[32];
    uint8_t ret = 0;

	pkt = pkt_alloc(100);
	if(!pkt) return 1;

	ret = pkt_push(pkt, sizeof(struct arphdr)+30);	
	if(ret) goto drop;

    FTM_ARP_ZLOG_DEBUG("ifindex:0x%x opcode:%d sip:0x%x dip:0x%x smac:%s\n",parp->ifindex,
                        opcode,sip,parp->key.ipaddr,ether_mac_to_string((char *)smac, mac_str));
    
	/* set arp header */
	parph = (struct arphdr *)pkt->data;	
    parph->hw_type = htons(1);
	parph->hw_len = 6;
	parph->pro_type = htons(ETH_P_IP);
	parph->pro_len = 4;
	if(opcode == ARP_OPCODE_GRATUITOUS_REPLY)
		parph->opcode = htons(ARP_OPCODE_REPLY);
	else
		parph->opcode = htons(opcode);
    parph->dip = htonl(parp->key.ipaddr);
	parph->sip = htonl(sip);	   
	memcpy(parph->smac, smac, 6);

    /* set ethcb */
    p_ethcb = (struct eth_control *)(&(pkt->cb));
	memset(p_ethcb, 0, PKT_CB_SIZE);
	pkt->cb_type = PKT_TYPE_ETH;
    p_ethcb->ethtype = ETH_P_ARP;
	p_ethcb->cos = 2;   /* arp 的优先级为 2 */
	if(parp->port)		/* vlanif 接口 */
	{
	    p_ethcb->svlan = IFM_VLANIF_ID_GET(parp->ifindex);
		p_ethcb->ifindex = parp->port;
	}
	else
	{
		p_ethcb->ifindex = parp->ifindex;
	}
	p_ethcb->is_changed = 1;
    
	/* 设置 dmac */
	switch(opcode)
	{
		case ARP_OPCODE_GRATUITOUS_REPLY:
		case ARP_OPCODE_REQUEST:
			memcpy(parph->dmac, ether_get_zero_mac(), 6); /* 请求的 dmac 为 0 */
			memcpy(p_ethcb->dmac, ether_get_broadcast_mac(), 6); /* 目的 mac 是广播 mac */
			break;
		case ARP_OPCODE_REPLY:
			memcpy(parph->dmac, parp->mac, 6); 
			memcpy(p_ethcb->dmac, parp->mac, 6);
			break;
		case RARP_OPCODE_REQUEST:
			break;
		case RARP_OPCODE_REPLY:
			break;
		default:
        {
           goto drop;
        }
	}

	ftm_pkt_dump(pkt->data, pkt->data_len, PKT_ACTION_SEND, PKT_TYPE_ETH);
	return eth_forward(pkt);
	
drop:	
	FTM_ARP_ZLOG_ERR("Arp send failed, drop the packet!\n");
	pkt_free(pkt);
	return NET_FAILED;
}


/*免费arp 发送函数*/
int arp_send_gratuitous(void *arg)
{
	struct ftm_ifm *pifm = NULL;	
    uint32_t ifindex = (uint32_t)(arg);
    uint8_t  i = 0;
	
    FTM_ARP_ZLOG_DEBUG("ifindex:0x%x\n",ifindex);
    
	pifm = ftm_ifm_lookup(ifindex);
	if((pifm == NULL) || (NULL == pifm->parp))
		return ERRNO_FAIL;
	if(pifm->ifm.status != IFNET_LINKUP)
		return ERRNO_FAIL;
	
	for(i = 0; i < IFM_IP_NUM; i++)
	{
		if (pifm->pl3if->ipv4[i].addr)
		{
			arp_send_if_addr(i, pifm);
		}
	}

	pifm->parp->retry_cnt++;
	if(pifm->parp->retry_cnt < ARP_RETRY_NUM)
		high_pre_timer_add("ArpGratuitousTimer", LIB_TIMER_TYPE_NOLOOP, arp_send_gratuitous, (void *)ifindex, 1*1000);
	
	return ERRNO_SUCCESS;
}


/*接口up  发送3 次免费arp*/
void arp_send_if_linkup(struct ftm_ifm *pifm)
{
	int i = 0;

	if((pifm == NULL) || (NULL == pifm->parp) || (pifm->pl3if == NULL))
		return;
    
    FTM_ARP_ZLOG_DEBUG("pfim:%p,pifm->parp:%p pifm->pl3if:%p\n",pifm,pifm->parp,pifm->pl3if);
    
	pifm->parp->retry_cnt = 0;
	for(i = 0; i < IFM_IP_NUM; i++)
	{
		if (pifm->pl3if->ipv4[i].addr)
		{
			arp_send_if_addr(i, pifm);
		}
	}

	pifm->parp->retry_cnt++;
	//thread_add_timer(ftm_master, arp_send_gratuitous, (void *)(pifm->ifm.ifindex), 1);
	high_pre_timer_add("ArpGratuitousTimer", LIB_TIMER_TYPE_NOLOOP, arp_send_gratuitous, (void *)(pifm->ifm.ifindex), 1*1000);
	
	return;
}


/*接口添加ip 发送一次免费arp*/
void arp_send_if_addr(uint8_t flag, struct ftm_ifm *pifm)
{
	struct arp_entry src_arp;
	uint8_t smac[6];

    memset(&src_arp, 0, sizeof(struct arp_entry));
	src_arp.ifindex = pifm->ifm.ifindex;						
	src_arp.status = ARP_STATUS_INCOMPLETE;
	FTM_IFM_GET_MAC(pifm, smac);
		
	if(flag < IFM_IP_NUM)
	{		
		src_arp.key.ipaddr = pifm->pl3if->ipv4[flag].addr;/* 免费 arp 的源 ip 和 目的 ip 相同 */
		arp_send(&src_arp, ARP_OPCODE_REQUEST, pifm->pl3if->ipv4[flag].addr, smac);
	}

	return;
}


static unsigned int arp_miss_anti_hash(void *pkey)
{
    struct arp_anti_key *key = pkey;
	uint32_t hash_value = 0;

	hash_value = key->sip_vlan_ifidx + key->type;

	return hash_value;
}

static int arp_miss_anti_compare(void *item, void *hash_key)
{
    struct arp_anti_key *pkey = NULL;
    struct arp_anti_key *hkey = NULL;

    if ((NULL == item) || (NULL == hash_key))
    {
        return ERRNO_FAIL;
    }

    FTM_ARP_ZLOG_DEBUG();

    hkey = hash_key;
    pkey = ((struct hash_bucket *)item)->hash_key;
        
    if (NULL == pkey)
    {
        return ERRNO_FAIL;
    }
    
    if ((pkey->sip_vlan_ifidx == hkey->sip_vlan_ifidx) &&
        (pkey->type == hkey->type))
    {
        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}

uint8_t arp_miss_anti_cnt_add(struct arp_anti_key* sip_key, uint32_t maxnum)
{
    struct arp_miss_anti *miss_anti = NULL;
    struct hash_bucket *pbucket = NULL;
    uint8_t ret = 0;

    pbucket = hios_hash_find(&amiss_anti, sip_key);
    if (pbucket)
    {
        miss_anti = pbucket->data;
        miss_anti->cnt ++;
        FTM_ARP_ZLOG_DEBUG("sip:0x%x cnt:%d\n", sip_key->sip_vlan_ifidx, miss_anti->cnt);
    }
    else if (gmiss_anti.gsip_limit || gmiss_anti.sip_flag ||
             gmiss_anti.vlan_flag  || gmiss_anti.ifidx_flag)
    {
        FTM_ARP_ZLOG_DEBUG("sip:0x%x\n",sip_key->sip_vlan_ifidx);

        pbucket = XCALLOC(MTYPE_ARP_ANTI, sizeof(struct hash_bucket));
        if (!pbucket) return ERRNO_FAIL;
        miss_anti = XCALLOC(MTYPE_ARP_ANTI, sizeof(struct arp_miss_anti));
        if (!miss_anti) return ERRNO_FAIL;

        miss_anti->num_limit = maxnum;
        miss_anti->timer = 1;
        memcpy(&(miss_anti->key), sip_key, sizeof(struct arp_anti_key));
        pbucket->hash_key = &(miss_anti->key);
        pbucket->data = miss_anti;
        ret = hios_hash_add(&amiss_anti, pbucket);
        if (ret)
        {
            FTM_ARP_ZLOG_DEBUG();
            XFREE(MTYPE_ARP_ANTI, pbucket);
            XFREE(MTYPE_ARP_ANTI, miss_anti);
            return ERRNO_FAIL;
        }
    }

    return ERRNO_SUCCESS;
}

void arp_miss_anti_cnt_del(struct arp_anti_key *key)
{
    struct hash_bucket *pbucket = NULL;
    
    pbucket = hios_hash_find(&amiss_anti, key);
    if (pbucket)
    {
        FTM_ARP_ZLOG_DEBUG("value:0x%x type:%d\n", key->sip_vlan_ifidx, key->type);
        hios_hash_delete(&amiss_anti, pbucket);
        XFREE(MTYPE_ARP_ANTI, pbucket->data);
        XFREE(MTYPE_ARP_ANTI, pbucket);
    }
}

void arp_miss_anti_cnt(struct pkt_buffer *pkt)
{
    struct ip_control *ipcb = (struct ip_control *)(&(pkt->cb));
    struct arp_anti_key key;
    struct ftm_ifm *pifm = NULL;

    FTM_ARP_ZLOG_DEBUG();
    if (!(pkt->in_port)) return ;

    if (gmiss_anti.gsip_limit || gmiss_anti.sip_flag)
    {
        key.sip_vlan_ifidx = ipcb->sip;
        key.type = ARP_ANTI_SIP;
        arp_miss_anti_cnt_add(&key, 0);
    }

    if (gmiss_anti.ifidx_flag)  
    {
        key.sip_vlan_ifidx = pkt->in_port;
        key.type = ARP_ANTI_IFIDX;
        arp_miss_anti_cnt_add(&key, 0);
    }
    if (gmiss_anti.vlan_flag)
    {
        key.type = ARP_ANTI_VLAN;

        pifm = ftm_ifm_lookup(pkt->in_ifindex);
        if (!pifm) return;

        key.sip_vlan_ifidx = pifm->ifm.encap.svlan.vlan_start;
        
        FTM_ARP_ZLOG_ERR("ifindex:0x%x vlan:0x%x\n",pkt->in_ifindex, key.sip_vlan_ifidx);
        arp_miss_anti_cnt_add(&key, 0);
    }

}

static void arp_miss_anti_acl_add(uint32_t value, uint8_t type)
{
    struct arp_anti_key anti_key;
    int8_t ret = 0;
    
    FTM_ARP_ZLOG_DEBUG("value:0x%x type:%d\n", value, type);
    anti_key.sip_vlan_ifidx = value;
    anti_key.type = type;

    ret = ftm_msg_send_to_hal(&anti_key, sizeof(struct arp_anti_key), 1,
                            IPC_TYPE_ARP, ARP_INFO_HAL_ANTI, IPC_OPCODE_ADD, 0);
    if(-1 == ret)
    {
        FTM_ARP_ZLOG_DEBUG("Notify hal add arp failed!\n");
        return;
    }
}



static int arp_miss_anti_timer()
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pbucket_temp = NULL;
    struct arp_miss_anti *panti = NULL;
    uint32_t cursor = 0;

    HASH_BUCKET_LOOPW(pbucket, cursor, amiss_anti)
    {
        pbucket_temp = pbucket;
        pbucket = pbucket_temp->next;

        panti = pbucket_temp->data;
        if (panti)
        {
            FTM_ARP_ZLOG_DEBUG("sip:0x%x cnt:%d limit:%d glimit:%d\n",panti->key.sip_vlan_ifidx, panti->cnt,panti->num_limit,gmiss_anti.gsip_limit);
            if (((panti->num_limit)&&(panti->cnt > panti->num_limit)) ||
                ((gmiss_anti.gsip_limit)&&(panti->cnt > gmiss_anti.gsip_limit)))
            {
                FTM_ARP_ZLOG_DEBUG("Found arp-miss attack!\n");
                arp_miss_anti_acl_add(panti->key.sip_vlan_ifidx, panti->key.type);
                panti->cnt = 0;
                panti->timer = 1;
            }
            else
            {
                if ((panti->num_limit)) continue;
                
                hios_hash_delete(&amiss_anti, pbucket_temp);
                XFREE(MTYPE_ARP_ANTI, pbucket_temp->data);
                XFREE(MTYPE_ARP_ANTI, pbucket_temp);
            }
        }
        
    }


    //thread_add_timer(ftm_master, arp_miss_anti_timer, 0, 1);

    return ERRNO_SUCCESS;
}
int arp_miss(uint32_t ipaddr, uint16_t vpn)
{
	struct nhp_entry *pnhp = NULL;
	struct ftm_ifm *pif = NULL;
	struct arp_entry src_arp;	
	struct fib_entry *pfib = NULL;
	uint8_t  smac[MAC_LEN];
	uint32_t addr = 0;

    FTM_ARP_ZLOG_DEBUG("ipaddr:0x%x vpn:0x%x\n",ipaddr,vpn);
    memset(&src_arp, 0, sizeof(struct arp_entry));
	/* 通过查路由，找到发送 ARP 的接口 */
	pfib = ftm_fib_match(ipaddr, 32, vpn);	
	if(NULL == pfib)
		return ERRNO_FAIL;
	if((pfib->ipaddr == 0)&&(pfib->masklen == 0))
		return ERRNO_FAIL;
	pnhp = ftm_nhp_lookup(pfib->nhp.nhp_index);
	if(NULL == pnhp)
		return ERRNO_FAIL;
	pif = ftm_ifm_lookup(pnhp->ifindex);
	if((NULL == pif) || (NULL == pif->pl3if))
		return ERRNO_FAIL;	
	if((NULL == pif->parp) || (0 == pif->parp->arp_disable))/* 接口没有开启 ARP 学习 */
		return ERRNO_FAIL;

    /* 找到同网段的接口 IP */
	addr = ftm_ifm_get_l3if_ipv4(pif->pl3if, ipaddr);
	if((addr) || (pif->parp->arp_proxy))
    {   
        /*添加半连接arp表项，发送arp 请求*/
        arp_add_incomplete(ipaddr,vpn,pif->ifm.ifindex,&src_arp);
		FTM_IFM_GET_MAC(pif, smac);
		return arp_send(&src_arp, ARP_OPCODE_REQUEST, addr, smac); 
	}
	
	return 0;
}

uint8_t arp_to_static_check( struct arp_entry *psrc_arp )
{
	struct arp_key *parp_key = NULL;
	struct listnode *p_listnode = NULL;
	
	for(ALL_LIST_ELEMENTS_RO(garp_to_static_list.arp_to_static_list, p_listnode, parp_key))
	{  
		if(psrc_arp->key.ipaddr == parp_key->ipaddr && psrc_arp->key.vpnid == parp_key->vpnid)		
		{
			return 1;
		}
	}
	return 0;
}

/*arp 状态机函数*/
int arp_fsm(struct arp_entry *psrc_arp, uint32_t dip, uint8_t event_type, struct ftm_ifm *pif)
{
	struct arp_entry *parp = NULL;	
	struct fib_entry *pfib = NULL;
    uint8_t  smac[6];
	uint32_t addr = 0;

	if((pif == NULL) || (NULL == pif->pl3if) || (psrc_arp == NULL))
		return ERRNO_FAIL;
	
	switch(event_type)
	{
		case ARP_RCV_GRATUITOUS:
		{	
			addr = ftm_ifm_get_l3if_ipv4(pif->pl3if, dip);
			if(addr == dip)  /*ip 冲突时回应免费arp*/
			{
				return arp_ip_conflict(pif, psrc_arp);
			}
			else if(addr)
			{
				FTM_ARP_ZLOG_DEBUG("Rcv gratuitous arp!dip:0x%x\n", dip);
				return arp_rcv_gratuitous(pif, psrc_arp);
			}
			break;
		}
		case ARP_RCV_REQUEST:
		{
            FTM_ARP_ZLOG_DEBUG("Rcv arp request!dip:0x%x\n",dip);
			
			addr = ftm_ifm_get_l3if_ipv4(pif->pl3if, dip);
			if(addr == dip)  /* 如果请求的是本接口地址，则回复 reply */				
			{
				FTM_IFM_GET_MAC(pif,smac);
				return arp_send(psrc_arp,ARP_OPCODE_REPLY,addr,smac);
			}
			
			/* arp 代理使能，对于存在 dip 路由的都回复应答*/
			if(pif->parp->arp_proxy)
			{
				pfib = ftm_fib_match(dip, 32, psrc_arp->key.vpnid);
				if(pfib)
				{	
					FTM_ARP_ZLOG_DEBUG("Arp proxy!\n");
                    
					FTM_IFM_GET_MAC(pif, smac);
					return arp_send(psrc_arp, ARP_OPCODE_REPLY, dip, smac);
				}
			}
			break;
		}
		case ARP_RCV_REPLY:
		{
			// 如果不是自己发送请求的应答，则不处理
			parp = arp_lookup(psrc_arp->key.ipaddr, psrc_arp->key.vpnid);
			/*if((parp) && (parp->status != ARP_STATUS_STATIC))*/
			if(parp)
			{	
                if (arp_fixed_check(parp, psrc_arp) == ERRNO_FAIL)
                    return ERRNO_FAIL;
                
                FTM_ARP_ZLOG_DEBUG();
				if(garp_to_static_list.status == ARP_TO_STATIC_ALL)
				{
					psrc_arp->status = ARP_STATUS_STATIC;
					psrc_arp->status_old = ARP_STATUS_COMPLETE;
				}
				else if(garp_to_static_list.status == ARP_TO_STATIC_PART && arp_to_static_check(psrc_arp))
				{
					psrc_arp->status = ARP_STATUS_STATIC;
					psrc_arp->status_old = ARP_STATUS_COMPLETE;
				}
				else
                	psrc_arp->status = ARP_STATUS_COMPLETE;
                arp_update(psrc_arp, parp);
			}
			break;
		}
        default:
        {
            break;
        }
	}
	
	return ERRNO_FAIL;
}


/*arp 收发包初始化函数*/
uint8_t pkt_arp_init(void)
{
 	struct eth_proto ethreg;
	uint8_t ret = 0;

    /*注册收发包*/
    memset(&ethreg, 0 ,sizeof(struct eth_proto));
	ethreg.ethtype = ETH_P_ARP;
	ethreg.dmac_valid = 1;

	ret = devm_comm_get_mac ( 1, 0, MODULE_ID_ARP , ethreg.dmac );
	if (ret)
	{
		zlog_err( "%s[%d]:Arp get smac from devm error,mac == NULL\n", __FILE__, __LINE__);
	}
	
	ethreg.dmac_mask[0] = 0xff;
	ethreg.dmac_mask[1] = 0xff;
	ethreg.dmac_mask[2] = 0xff;
	ethreg.dmac_mask[3] = 0x00;
	ethreg.dmac_mask[4] = 0x00;
	ethreg.dmac_mask[5] = 0x00;
	ret = ftm_msg_send_to_hal((void *)&ethreg, sizeof(struct eth_proto), 1,
                    IPC_TYPE_PROTO, PROTO_TYPE_ETH, IPC_OPCODE_REGISTER, 0);

	memset(&ethreg, 0 ,sizeof(struct eth_proto));
	ethreg.ethtype = ETH_P_ARP;
	ethreg.dmac_valid = 1;

	ethreg.dmac[0] = 0xff;
	ethreg.dmac[1] = 0xff;
	ethreg.dmac[2] = 0xff;
	ethreg.dmac[3] = 0xff;
	ethreg.dmac[4] = 0xff;
	ethreg.dmac[5] = 0xff;
	
	ethreg.dmac_mask[0] = 0xff;
	ethreg.dmac_mask[1] = 0xff;
	ethreg.dmac_mask[2] = 0xff;
	ethreg.dmac_mask[3] = 0xff;
	ethreg.dmac_mask[4] = 0xff;
	ethreg.dmac_mask[5] = 0xff;
	
	ret = ftm_msg_send_to_hal((void *)&ethreg, sizeof(struct eth_proto), 1,
					IPC_TYPE_PROTO, PROTO_TYPE_ETH, IPC_OPCODE_REGISTER, 0);
    hios_hash_init(&amiss_anti, 1024, arp_miss_anti_hash, arp_miss_anti_compare);
    memset(&gmiss_anti, 0, sizeof(struct garp_miss_anti));
    high_pre_timer_add("ArpMissAntiTimer",LIB_TIMER_TYPE_LOOP, arp_miss_anti_timer, NULL, 1*1000);
	return ret;	   
}


