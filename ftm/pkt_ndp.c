
#include <lib/inet_ip.h>
#include <lib/memtypes.h>
#include <lib/errcode.h>
#include <lib/pkt_buffer.h>
#include <lib/prefix.h>
#include <lib/route_com.h>
#include <lib/memory.h>
#include <lib/log.h>
#include <lib/checksum.h>
#include <route/ndp_cmd.h>

#include "ftm.h"
#include "pkt_ip.h"
#include "ftm_nhp.h"
#include "ftm_fib.h"
#include "ftm_ifm.h"
#include "ftm_ndp.h"
#include "pkt_ndp.h"
#include "pkt_icmp.h"
#include "pkt_type.h"

unsigned long  delay_timerId=0;
struct list    ndp_delay_list = {0, 0, 0, 0, 0};
static uint8_t ipv6_multiaddr[16] = {0xff,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};


/*添加nd 半连接表项*/
static void ndp_add_incomplete(struct ipv6_addr *paddr, uint16_t vpn, uint32_t ifindex, struct ndp_neighbor *nd_entry)
{
    memset(nd_entry, 0, sizeof(struct ndp_neighbor));
    nd_entry->key.ifindex = ifindex;
    nd_entry->isrouter = 1;//暂时置1
    memcpy(nd_entry->key.ipv6_addr, paddr, sizeof(struct ipv6_addr));
    nd_entry->key.vpnid = vpn;
    nd_entry->key.ifindex = ifindex;
    nd_entry->time = gndp.fake_time;
    nd_entry->status = NDP_STATUS_INCOMPLETE;
    nd_entry->count ++;

    ndp_add(nd_entry);
}

//nd 可达性确认事件接口
int ndp_reachable_confim(struct ipv6_addr *pdip, uint16_t vpn, uint32_t ifindex)
{
    struct ndp_neighbor *pndp = NULL;
    uint8_t  ip6str[INET6_ADDRSTRLEN];
    struct ipv6_addr dip;

    memset(&dip, 0, sizeof(struct ipv6_addr));
    inet_ntop6((uchar *)pdip, (char *)ip6str, sizeof(struct ipv6_addr));

    pndp = ndp_lookup(pdip->ipv6, vpn, ifindex);
    if( !pndp )
    {
        FTM_NDP_ZLOG_DEBUG("ndp entry ip:%s vpn:%d not exist!\n",ip6str, vpn);
        return ERRNO_FAIL;
    }

    return ndp_fsm(pndp, dip.ipv6, NDP_EVENT_REACHABLE_CONFIRM, 0);
}



//查找nd表项失败，触发ndp_miss添加INCOMPLETE表项
int ndp_miss(struct ipv6_addr *pdip, uint16_t vpn,uint32_t ifindex)
{
    struct ndp_neighbor pndp;
	struct ftm_ifm *pif = NULL;
    struct ipv6_addr *paddr = NULL;
    struct ipv6_addr ip6addr;
	uint8_t  smac[MAC_LEN];
    uint8_t  paddr_str[INET6_ADDRSTRLEN];

    inet_ntop6((uchar *)pdip, (char *)paddr_str, sizeof(struct ipv6_addr));
    FTM_NDP_ZLOG_DEBUG("ip6addr:%s vpn:0x%x\n", paddr_str, vpn);
    
	/* 通过查路由，找到发送 NDP 的接口 */
#if 0
	pfib = ftm_fibv6_match(pdip, 128, vpn);	
	if(NULL == pfib)
		return ERRNO_FAIL;
	pnhp = ftm_nhp_lookup(pfib->nhp.nhp_index);
	if(NULL == pnhp)
		return ERRNO_FAIL;
#endif
    if (ifindex == 0)
    {
        FTM_NDP_ZLOG_DEBUG();
        return ERRNO_FAIL;
    }

    
	pif = ftm_ifm_lookup(ifindex);
	if((NULL == pif) || (NULL == pif->pl3if))
		return ERRNO_FAIL;	
	if((NULL == pif->parp) || (0 == pif->parp->ndp_disable))/* 接口没有开启 NDP 学习 */
		return ERRNO_FAIL;

    /* 找到同网段的接口 IP ,找不到则填充routev6_id*/
    paddr = ftm_ifm_get_l3if_ipv6(pif->pl3if, pdip);
    if(paddr) 
        IPV6_ADDR_COPY(&ip6addr, paddr);       
    else
        IPV6_ADDR_COPY(&ip6addr, &(g_fib.routerv6_id));

    if(ipv6_is_valid(&ip6addr) || pif->parp->ndp_proxy)
    {
        /*添加半连接ndp表项，发送ndp 请求*/
        ndp_add_incomplete(pdip, vpn, pif->ifm.ifindex, &pndp);
		FTM_IFM_GET_MAC(pif, smac);
		return ndp_send(&pndp, NDP_OPCODE_NSOLICIT, ip6addr.ipv6, smac);
    }
    
	return ERRNO_SUCCESS;
}


/*被请求节点组播地址由前缀FF02::1:FF00:0/104和单播地址的最后24位组成*/
static void ndp_solicit_addr_get(uint8_t target[16],uint8_t multi_addr[16])
{
    memset(multi_addr, 0, 16);

    multi_addr[0]  = 0XFF;
    multi_addr[1]  = 0X02;
    multi_addr[11] = 0X01;
    multi_addr[12] = 0xFF;
    multi_addr[13] = target[13];
    multi_addr[14] = target[14];
    multi_addr[15] = target[15];
}   


/*ipv6 控制块填充*/
static void ndp_set_ipv6cb(struct ip_control *ip6cb, uint8_t sip[16], uint8_t dip[16])
{
    if(!sip || !dip || !ip6cb) 
    {
        zlog_err("%s:%d sip is NULL!\n",__func__,__LINE__);
        return;
    }

    ip6cb->is_changed = 1;
    ip6cb->ttl = 255; 
    ip6cb->protocol = IP_P_IPV6_ICMP;
    ip6cb->chsum_enable = 1;
    memcpy(ip6cb->sipv6.ipv6, sip, sizeof(struct ipv6_addr));
    memcpy(ip6cb->dipv6.ipv6, dip, sizeof(struct ipv6_addr));

}

/*icmpv6 报文填充*/
static void ndp_set_icmpv6(struct ndp_pkt *nd_pkt, uint8_t type ,uint8_t dip[16])
{

    memset(nd_pkt, 0, NDP_PKT_LEN);
    
    nd_pkt->nd_hdr.type = type;
    nd_pkt->nd_hdr.code = 0;
    nd_pkt->nd_hdr.checksum = 0;//csum在ip层计算

    //na应答报文填充R S O标志位
    if(type == NDP_NEIGHBOR_ADVERTISE)
    {
        //1:路由器  0:主机
        nd_pkt->router = 0;

        //1:为响应ns 发送的na , 0:组播或非请求单播
        nd_pkt->solicited = 1;
        
        /*重载标记。1:用目标链路层地址选项
                 中的链路层地址来更新邻居缓存表*/
        nd_pkt->override = 1;//如果是作为代理发送na应答则应置O 标志位为0
    }
    
    memcpy(nd_pkt->dip, dip, sizeof(struct ipv6_addr));

}




//发送ndp协议报文
int ndp_send(struct ndp_neighbor *pnd_entry,uint8_t opcode,uint8_t sip[16],uint8_t *smac)
{
    struct ndp_pkt    *nd_pkt = NULL;
    struct ndp_opt    *nd_opt = NULL;
    struct ip_control *ipv6cb = NULL;
    struct pkt_buffer *pkt = NULL;
    struct ndp_neighbor *pndp = NULL;
    uint8_t mc_addr[16];
    int ret = 0;
    
    FTM_NDP_ZLOG_DEBUG();
    pkt = pkt_alloc(100);
    if(!pkt)
    {
        FTM_NDP_ZLOG_ERR("Pkt alloc fail!\n");
        return ERRNO_FAIL;
    }
    
    ret = pkt_push(pkt, NDP_PKT_LEN);
    if(ret) goto drop;

    
    /*nd 请求应答报文均由icmpv6头 + 一个选项字段构成*/
    nd_pkt = pkt->data;
    nd_opt = (struct ndp_opt *)(nd_pkt + sizeof(struct icmpv6_hdr));
    ipv6cb = (struct ip_control *)(&(pkt->cb));
    
    switch(opcode)
    {
        case NDP_OPCODE_NSOLICIT:
            /*填充报文*/
            ndp_set_icmpv6(nd_pkt, NDP_NEIGHBOR_SOLICIT, pnd_entry->key.ipv6_addr);
            nd_opt = (struct ndp_opt *)(nd_pkt->option);
            nd_opt->type = NDP_OPT_SRC_LADDR;
            nd_opt->len  = sizeof(struct ndp_opt) / 8;
            memcpy(nd_opt->mac, smac, MAC_LEN);

            /*填充ipcb 控制块*/
            /*probe状态的nd 表项以单播方式发送nd 探测报文*/
            pndp = ndp_lookup(nd_pkt->dip, pnd_entry->key.vpnid, pnd_entry->key.ifindex);
            if(pndp && pndp->status == NDP_STATUS_PROBE)
            {
                pkt->cb_type = PKT_TYPE_IPV6;
                IPV6_ADDR_COPY(mc_addr, nd_pkt->dip);
                ipv6cb->pkt_type = PKT_TYPE_IPV6;
                if (ipv6_is_linklocal((struct ipv6_addr *)nd_pkt->dip))
                {
                    ipv6cb->ifindex = pnd_entry->key.ifindex;
                }
            }
            else
            {
            	pkt->cb_type = PKT_TYPE_IPV6MC;
                ndp_solicit_addr_get(nd_pkt->dip, mc_addr);
                ipv6cb->pkt_type = PKT_TYPE_IPV6MC;
                ipv6cb->ifindex = pnd_entry->key.ifindex;
            }
			ipv6cb->vpn = pnd_entry->key.vpnid;
            ndp_set_ipv6cb(ipv6cb, sip, mc_addr);
            break;
        case NDP_OPCODE_NADVERTISE:
            /*填充报文*/
            ndp_set_icmpv6(nd_pkt, NDP_NEIGHBOR_ADVERTISE, sip);
            nd_opt = (struct ndp_opt *)(nd_pkt->option);
            nd_opt->type = NDP_OPT_TARGET_LADDR;
            nd_opt->len  = sizeof(struct ndp_opt) / 8;
            memcpy(nd_opt->mac, smac, MAC_LEN);

            /*填充ipcb 控制块*/
        	pkt->cb_type = PKT_TYPE_IPV6;
            ipv6cb->pkt_type = PKT_TYPE_IPV6;
			ipv6cb->vpn = pnd_entry->key.vpnid;
            ndp_set_ipv6cb(ipv6cb, sip, pnd_entry->key.ipv6_addr);
            
            if (ipv6_is_linklocal((struct ipv6_addr *)(pnd_entry->key.ipv6_addr)))
            {
                ipv6cb->ifindex = pnd_entry->key.ifindex;
            }

            if(ipv6_is_zero(&(ipv6cb->dipv6)))
            {
                nd_pkt->solicited = 0;

                //设置dip 为所有节点多播地址FF02::1
                IPV6_ADDR_COPY(&(ipv6cb->dipv6), ipv6_multiaddr);
            }
            
            break;
        default:
                break;
    }

    return ip_forward(pkt);
    
drop:
    FTM_NDP_ZLOG_DEBUG("Ndp send error,drop it!\n");
    pkt_free(pkt);
    return NET_FAILED;
}

#if 0
static int ndp_pkt_check(struct ndp_pkt *nd_pkt)
{
    struct ndp_opt *nd_opt = NULL;

    nd_opt = (struct ndp_opt *)(nd_pkt->option);
    
    /*type字段检查*/
    if(nd_pkt->nd_hdr.type < NDP_ROUTER_SOLICI
       || nd_pkt->nd_hdr.type > NDP_REDIRECT)
    {
        FTM_NDP_ZLOG_ERR("nd_pkt type error!\n");
        return ERRNO_FAIL;
    }

    /*code 字段应为0*/
    if(nd_pkt->nd_hdr.code)
    {
        FTM_NDP_ZLOG_ERR("nd_pkt code not zero!\n");
        return ERRNO_FAIL;
    }

    /*check_sum 字段检查，需要ip 层配合生成伪首部来计算校验和*/
        
    /*icmp length 为24字节或更多字节*/

    /*nd 报文的ttl 应等于255*/


    /*Target address 不是多播地址*/
    if(ipv6_is_multicast((struct ipv6_addr *)(nd_pkt->dip)))
    {
        FTM_NDP_ZLOG_ERR("nd_pkt target address is multicast!\n");
        return ERRNO_FAIL;
    }
    
    /*选项字段类型检查*/
    if(nd_opt->type < NDP_OPT_SRC_LADDR || 
       nd_opt->type > NDP_OPT_MTU)
    {
        FTM_NDP_ZLOG_ERR("nd_opt type error!\n");
        return ERRNO_FAIL;
    }

    /*选项字段中length 字段大于0*/
    if(nd_opt->len <= 0)
    {
        FTM_NDP_ZLOG_ERR("nd_opt length error!\n");
        return ERRNO_FAIL;
    }

    return ERRNO_SUCCESS;
}
#endif


//接收ndp协议报文
int ndp_recv(struct pkt_buffer *pkt)
{
    struct ndp_neighbor  pndp;
    struct ip_control *ipv6cb = NULL;
    struct ndp_pkt *nd_pkt = NULL;
    struct ftm_ifm *pifm = NULL;
    struct ndp_opt *nd_opt = NULL;
    struct ipv6_addr dip;
    uint8_t rso_flag = 0;//存储R S O reserve字段，R ,S,O分别占一个bit位
    uint8_t event_type = 0;

    nd_pkt = pkt->data;
    ipv6cb = (struct ip_control *)(&(pkt->cb));
    nd_opt = (struct ndp_opt *)(nd_pkt->option);
    memset(&pndp, 0, sizeof(struct ndp_neighbor));

    FTM_NDP_ZLOG_DEBUG();

    if(!nd_pkt || !nd_opt)
        goto drop;
    
    //if(ERRNO_SUCCESS != ndp_pkt_check(nd_pkt))
    //    goto drop;

    pifm = ftm_ifm_lookup(pkt->in_ifindex);
    if(!pifm || !(pifm->parp))
    {
        FTM_NDP_ZLOG_ERR("Interface:0x%x pifm or pifm->parp is NULL!\n",pkt->in_ifindex);
        goto drop;
    }
    if(pifm->parp->ndp_disable == NDP_DISABLE_DEF)
    {
        FTM_NDP_ZLOG_ERR("Dynamic ndp learn disable, drop it!\n");
        goto drop;
    }

    if(nd_pkt->nd_hdr.type == NDP_NEIGHBOR_SOLICIT)
    {
        event_type = NDP_EVENT_RCV_NSOLICIT;
        if(nd_opt && (nd_opt->type == NDP_OPT_SRC_LADDR) && (nd_opt->len != 0))
        {
            memcpy(pndp.mac, nd_opt->mac, MAC_LEN);
        }

        /*收到 NS 请求保存发送端ip 、mac 及收包端口*/
        memcpy(pndp.key.ipv6_addr, ipv6cb->sipv6.ipv6, sizeof(struct ipv6_addr));
        pndp.key.vpnid = pkt->vpn;
        pndp.key.ifindex = pkt->in_ifindex;

		if(IFM_TYPE_IS_VLANIF(pkt->in_ifindex))
			pndp.port = pkt->in_port;
        
        memcpy(dip.ipv6, nd_pkt->dip, sizeof(struct ipv6_addr));
    }
    else if(nd_pkt->nd_hdr.type == NDP_NEIGHBOR_ADVERTISE)
    {
        event_type = NDP_EVENT_RCV_NADVERTISE;
        if(nd_opt && (nd_opt->type == NDP_OPT_TARGET_LADDR) && (nd_opt->len != 0))
        {
            memcpy(pndp.mac, nd_opt->mac, MAC_LEN);
        }

        FTM_NDP_ZLOG_DEBUG("R:%d S:%d O:%d\n",nd_pkt->router,nd_pkt->solicited,nd_pkt->override);
        
        /*设置R S O 标志位*/
        if(nd_pkt->router) rso_flag |= 0x80;
        if(nd_pkt->solicited) rso_flag |= 0x40;
        if(nd_pkt->override) rso_flag |= 0x20;

        FTM_NDP_ZLOG_DEBUG("rso_flag:%d\n",rso_flag);
        
        /*收到NA 应答报文生成nd 表项*/
        memcpy(pndp.key.ipv6_addr, nd_pkt->dip, sizeof(struct ipv6_addr));
        pndp.key.vpnid = pkt->vpn;
        pndp.key.ifindex = pkt->in_ifindex;

		if(IFM_TYPE_IS_VLANIF(pkt->in_ifindex))
			pndp.port = pkt->in_port;
                
    }
    else
    {
        FTM_NDP_ZLOG_ERR("Event type error!\n");
        goto drop;
    }
    
    ndp_fsm(&pndp, dip.ipv6, event_type, rso_flag);

    pkt_free(pkt);
    return NET_SUCCESS;

drop:
    FTM_NDP_ZLOG_ERR("Ndp rcv error,drop the pkt!\n");
    pkt_free(pkt);
    return NET_FAILED;
}


/*delay状态等待可达性确认定时器*/
int ndp_delay_status_timer( )
{
	struct listnode  *pnode  = NULL;
	struct listnode  *pnextnode = NULL; 
    struct ndp_neighbor *pndp = NULL;
	struct list *delay_list = NULL;//纯粹为了消告警啊

	delay_list = &ndp_delay_list;
    for (ALL_LIST_ELEMENTS(delay_list, pnode, pnextnode, pndp))
	{ 
        listnode_delete(delay_list, pndp);
        
        FTM_NDP_ZLOG_DEBUG("pndp->key.ip:%x %x\n",pndp->key.ipv6_addr[0],pndp->key.ipv6_addr[15]);        
        /*delay状态超时产生可达性确认超时事件*/
        ndp_fsm(pndp, pndp->key.ipv6_addr, NDP_EVENT_REACHCONFIRM_TIMEOUT, 0);
        
    }

    if(delay_list->count)
		delay_timerId = high_pre_timer_add("NdpDelayStatusTimer", LIB_TIMER_TYPE_NOLOOP,ndp_delay_status_timer, NULL, NDP_DELAY_FIRST_PROBE_TIME*1000);	 
    else
        delay_timerId = 0;

	return ERRNO_SUCCESS;
}


/*s标志  O标志
*  S            O
*  0            0    非请求na，O标志位为0则忽略此na并切换本地缓存表项到probe探测
*  0            1    非请求na，O标志位为1，更新mac并切换本地缓存状态到stale (邻居主动发出)
*  1            0    请求na，O 为0；有nd 且状态为reach时更新mac 并切状态到stale；
*                                                        有nd 为其它状态时忽略此na 报文
*  1            1    请求na，更新mac，并切换nd 状态到reachable
*/

/*NDP 状态机*/
int ndp_fsm(struct ndp_neighbor *pndp, uint8_t dip[16], enum NDP_EVENT event_type, uint8_t rso_flag)
{
    struct ftm_ifm *pifm = NULL;
    struct ndp_neighbor *ndp = NULL;
    struct fibv6_entry *pfib6 = NULL;
    struct ipv6_addr ip6_addr;
    struct nhp_entry *pnhp = NULL;
    uint8_t smac[MAC_LEN];
    uint8_t sflag = 0;
    uint8_t i = 0;

    FTM_NDP_ZLOG_DEBUG("pndp->ifindex:0x%x dip[0]:%x dip[15]:0x%x event_type:%d\n",
                                        pndp->key.ifindex, dip[0], dip[15], event_type);

    pifm = ftm_ifm_lookup(pndp->key.ifindex);
    if(!pifm)
        return ERRNO_FAIL;
    
    switch(event_type)
    {
        case NDP_EVENT_RCV_NSOLICIT:
            /*收到NS请求并验证报文合法性；如果本地没有选项字段中
                      *的<ip,mac> 对应关系，则根据报文的option 字段在本地生成源
                      *IP 的nd 表项并置为stale状态；如果本地存在option 中的nd表项
                      *但与option 中的mac 地址不同，则更新此nd 表项并置为stale 状态
                    */
            
            //收到请求本接口ip 的ns 请求回复na 应答
            for(i = 0;i < IFM_IP_NUM; i++)
            {
                FTM_NDP_ZLOG_DEBUG("sametoaddr:%d sametolinklocal:%d",IPV6_ADDR_SAME(dip, pifm->pl3if->ipv6[i].addr),
                                                            IPV6_ADDR_SAME(dip, pifm->pl3if->ipv6_link_local.addr));
                if (IPV6_ADDR_SAME(dip, pifm->pl3if->ipv6[i].addr) ||
                    IPV6_ADDR_SAME(dip, pifm->pl3if->ipv6_link_local.addr))
                {
                    sflag = 1;
                }
                if(sflag)
                {
                    pndp->status = NDP_STATUS_STALE;
                    pndp->time = gndp.stale_time;
                    ndp = ndp_lookup(pndp->key.ipv6_addr, pndp->key.vpnid, pndp->key.ifindex);
                    if(!ndp)
                    {
                        ndp_add( pndp );
                    }
                    else if(ndp->status == NDP_STATUS_INCOMPLETE || 
							ndp->status == NDP_STATUS_STALE      ||
							ndp->status == NDP_STATUS_DELAY      ||
							ndp->status == NDP_STATUS_PROBE )
                    {
                        ndp_delete((struct ipv6_addr *)(ndp->key.ipv6_addr), ndp->key.vpnid, ndp->key.ifindex);
                        ndp_add( pndp );
                    }

                    FTM_IFM_GET_MAC(pifm,smac);
                    return ndp_send(pndp, NDP_OPCODE_NADVERTISE, dip, smac);    
                }
            }

            /*代理开启时查询是否有到达dip 的路由，有则应答*/
            if(pifm->parp->ndp_proxy == NDP_PROXY_DEF)
            {
                memcpy(ip6_addr.ipv6, dip, sizeof(struct ipv6_addr));
				pfib6 = ftm_fibv6_match(&ip6_addr, 128, pndp->key.vpnid);
				if(NULL == pfib6)   return ERRNO_FAIL;
				pnhp = ftm_nhp_lookup(pfib6->nhp.nhp_index);
				if(NULL != pnhp)
				{	
					FTM_NDP_ZLOG_DEBUG("Ndp proxy!\n");
					FTM_IFM_GET_MAC(pifm, smac);
					return ndp_send(pndp, NDP_OPCODE_NADVERTISE, dip, smac);
				}
            }
            break;
        case NDP_EVENT_RCV_NADVERTISE:
            /*只处理主动发出nd的应答na 报文*/
            ndp = ndp_lookup(pndp->key.ipv6_addr, pndp->key.vpnid, pndp->key.ifindex);
            if(!ndp) return ERRNO_FAIL;

            /*R标志位置位*/
            pndp->isrouter = rso_flag & 0x80;

            /*S 置位与O标志位*/
            if((rso_flag & 0x40) && (rso_flag & 0x20))
            {
                FTM_NDP_ZLOG_DEBUG();
                /*更新mac并切换状态为RACHABLE*/
                pndp->status = NDP_STATUS_REACHABLE;
            }
            else if((rso_flag & 0x40) && !(rso_flag & 0x20))
            {           
                FTM_NDP_ZLOG_DEBUG();
                if(pndp->status == NDP_STATUS_REACHABLE)
                {
                    memcpy(pndp->mac, ndp->mac, 6);
                    pndp->status = NDP_STATUS_STALE;
                    pndp->time = gndp.stale_time;
                }
            }
            else if(!(rso_flag & 0x40) && (rso_flag & 0x20))
            {
                FTM_NDP_ZLOG_DEBUG();
                pndp->status = NDP_STATUS_STALE;
                pndp->time = gndp.stale_time;
            }
            else if(!(rso_flag & 0x40) && !(rso_flag & 0x20))
            {
                FTM_NDP_ZLOG_DEBUG();
                pndp->status = NDP_STATUS_PROBE;
            }

            return ndp_update(ndp, pndp);
            break;
        case NDP_EVENT_REACHABLE_TIMEOUT:
            if(pndp->status == NDP_STATUS_REACHABLE)
            {
                /*可达性时间到nd 表项切换到stale 状态
                              ，同时更新主机路由为drop  状态  */
               pndp->status = NDP_STATUS_STALE;
               pndp->time = gndp.stale_time;
               ftm_nhp_update_ndp(pndp, 1);//更新nhp 状态
               FTM_NDP_STATUS_UPDATE_NOTIFY_HAL(pndp);
            }
            break;
        case NDP_EVENT_PKT_SEND:
		case NDP_EVENT_STALE_TIMEOUT:
            if(pndp && pndp->status == NDP_STATUS_STALE)
            {
               pndp->status = NDP_STATUS_DELAY;
               pndp->time = NDP_DELAY_FIRST_PROBE_TIME;
			   FTM_NDP_STATUS_UPDATE_NOTIFY_HAL(pndp);
			   
               listnode_add(&ndp_delay_list, pndp);
               if(!delay_timerId)
                   delay_timerId = high_pre_timer_add("NdpDelayStatusTimer", LIB_TIMER_TYPE_NOLOOP,ndp_delay_status_timer, NULL, NDP_DELAY_FIRST_PROBE_TIME*1000);   
            }
            break;
        case NDP_EVENT_REACHABLE_CONFIRM:
            if(pndp->status == NDP_STATUS_DELAY)
            {
                pndp->status = NDP_STATUS_REACHABLE;
                pndp->time = NDP_REACHABLE_TIME;
                ftm_nhp_update_ndp(pndp, 1);//更新nhp 状态 

				FTM_NDP_STATUS_UPDATE_NOTIFY_HAL(pndp);
            }
            break;
        case NDP_EVENT_REACHCONFIRM_TIMEOUT:
            if(pndp->status == NDP_STATUS_DELAY)
            {
                pndp->status = NDP_STATUS_PROBE;
                pndp->time = NDP_PROBE_AGE_TIME;
                pndp->count = 0;
				FTM_NDP_STATUS_UPDATE_NOTIFY_HAL(pndp);
				
				ndp_probe_fast(pndp);
				pndp->count ++;
            }
            break;
        case NDP_EVENT_RCV_RSOLICI:
        case NDP_EVENT_RCV_RADVERTISE:
        case NDP_EVENT_RCV_REDIRECT:
            break;
        default:
            break;
    }

    return ERRNO_SUCCESS;
}


/*ndp 注册收发包*/
int ndp_register(void)
{
    struct ip_proto ip_pro;
	int ret = 0;

	/*注册收发包*/
    memset(&ip_pro, 0 ,sizeof(struct ip_proto));
	ip_pro.protocol = IP_P_IPV6_ICMP;
    ip_pro.icmpv6_type_valid = 1;
    ip_pro.icmpv6_type = NDP_NEIGHBOR_SOLICIT;

	ret = ftm_msg_send_to_hal((void *)&ip_pro, sizeof(struct ip_proto), 1,
                    IPC_TYPE_PROTO, PROTO_TYPE_IPV6, IPC_OPCODE_REGISTER, 0);

    return ret;
}


