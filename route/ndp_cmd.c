
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/ifm_common.h>
#include <lib/errcode.h>
#include <lib/ether.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/log.h>
#include <lib/log.h>

#include "route_main.h"
#include "ndp_cmd.h"
#include "route_if.h"

/*NDP 视图节点*/
static struct cmd_node ndp_node =
{ 
	NDP_NODE,  
	"%s(config-ndp)# ",  
	1, 
};


/*进入ndp 视图*/
DEFUN(ndp_mode,
    	 ndp_mode_cmd,
	     "ndp",
	     "Ndp command\n")
{
	vty->node = NDP_NODE;
	
	return CMD_SUCCESS;
}


/*配置物理口及trunk 接口的静态ndp*/
DEFUN(configure_static_ndp,
    	 configure_static_ndp_cmd,
    	 "ipv6 neighbor X.X.X.X.X.X.X.X mac XX:XX:XX:XX:XX:XX interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP|trunk TRUNK} [l3vpn <1-1024>]",
    	 "Ipv6\n"
    	 "Ipv6 neighbor\n"
    	 "Ipv6 address format X:X:X:X:X:X:X:X\n"
    	 "Mac address\n"
    	 "Mac format XX:XX:XX:XX:XX:XX\n"
    	 INTERFACE_STR
    	 CLI_INTERFACE_ETHERNET_STR
    	 CLI_INTERFACE_ETHERNET_VHELP_STR
    	 CLI_INTERFACE_GIGABIT_ETHERNET_STR
    	 CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
    	 CLI_INTERFACE_XGIGABIT_ETHERNET_STR
    	 CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
    	 CLI_INTERFACE_TRUNK_STR
    	 CLI_INTERFACE_TRUNK_VHELP_STR
    	 "L3vpn \n"
    	 "L3vpn value <1-1024>\n")
{
    struct ndp_neighbor pndp;
    struct ndp_neighbor *rndp = NULL;
    struct route_if *pif = NULL;
    struct ipv6_addr    ipv6_add;
    uint32_t ifindex = 0;
    uint16_t vpnid = 0;
    uint8_t  mac[6];
	int ret = 0;

    /*参数获取及合法性检查*/
    ret = ndp_ipv6_str_check((char *)argv[0]);
	if (ERRNO_SUCCESS != ret)
	{
        vty_error_out(vty, "The specified IPV6 address is invalid.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
    
    memset(&ipv6_add, 0, sizeof(struct ipv6_addr));
    ret = inet_pton6((char *)argv[0], ipv6_add.ipv6);
    if(ret == 0)
    {
        vty_error_out(vty, "The specified IPV6 address is invalid.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }


	ret = ether_valid_mac((char *)argv[1]);
	if (1 == ret)
	{
		vty_error_out ( vty, "Please check out mac format.%s", VTY_NEWLINE );
		return CMD_WARNING;
	}
    ether_string_to_mac((char *)argv[1], mac);

    if (!ether_is_broadcast_mac(mac))
    {
        vty_error_out ( vty, "Can't set broadcast mac.%s", VTY_NEWLINE );
        return CMD_WARNING;
    }
	else if (mac[0] & 0x01)
    {
        vty_error_out ( vty, "Can't set multicast mac.%s", VTY_NEWLINE );
        return CMD_WARNING;
    }
    else if (!ether_is_zero_mac(mac))
    {
        vty_error_out ( vty, "Input mac is invalid.%s", VTY_NEWLINE );
        return CMD_WARNING;
    }

    if (NULL != argv[2] && NULL == argv[3] && NULL == argv[4] && NULL == argv[5])
    {
	    ifindex = ifm_get_ifindex_by_name("ethernet", (char *)argv[2]);
    }
    else if(NULL == argv[2] && NULL != argv[3] && NULL == argv[4] && NULL == argv[5])
    {
        ifindex = ifm_get_ifindex_by_name("gigabitethernet", (char *)argv[3]);
    }
	else if(NULL == argv[2] && NULL == argv[3] && NULL != argv[4] && NULL == argv[5])
    {
        ifindex = ifm_get_ifindex_by_name("xgigabitethernet", (char *)argv[4]);
    }
	else if(NULL == argv[2] && NULL == argv[3] && NULL == argv[4] && NULL != argv[5])
    {
        ifindex = ifm_get_ifindex_by_name("trunk", (char *)argv[5]);
    }
	if(ifindex == 0)
	{
		vty_error_out ( vty, "Please check out interface format.%s", VTY_NEWLINE );
		return CMD_WARNING;	
	}	
    
    if (NULL != argv[6])
    {
        vpnid = (uint16_t)atoi(argv[6]);
    }

    memset(&pndp, 0, sizeof(struct ndp_neighbor));
    memcpy(pndp.key.ipv6_addr, ipv6_add.ipv6, sizeof(struct ipv6_addr));
    pndp.key.vpnid = vpnid;
    pndp.key.ifindex = ifindex;
    memcpy(pndp.mac, mac, MAC_LEN);
    pndp.status  = NDP_STATUS_STATIC;

    //非配置恢复检查l3if 是否存在，不存在则添加失败
    if(1 != vty->config_read_flag)
    {
        pif = route_if_lookup(ifindex);
        if(!pif || !(NDP_IF_IPV6_EXIST(pif)))
        {
    		vty_error_out(vty,"Static ndp add fail,please add l3if ip first!%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    rndp = ndp_static_lookup((struct ipv6_addr *)(pndp.key.ipv6_addr), pndp.key.vpnid, pndp.key.ifindex);
    if (rndp)
    {
        vty_error_out(vty,"This ipv6 neighbor is exists!%s",VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    
    ret = ndp_static_add(&pndp);
    if (ret == ERRNO_OVERSIZE)
    {
        vty_error_out(vty,"Static nd add fail! Reason:Over limit!%s",VTY_NEWLINE);
        return CMD_WARNING;
    }
    else if (ret == ERRNO_PARAM_ILLEGAL)
    {
        vty_error_out(vty,"Static nd add fail!Reason:param illegal!%s",VTY_NEWLINE);
        return CMD_WARNING;
    }
    else if (ret == ERRNO_IPC)
    {
        vty_error_out(vty,"Static nd add maybe failed!Reason:Ipc send to ftm error!%s",VTY_NEWLINE);
        return CMD_WARNING;
    }
    else if (ret == ERRNO_FAIL)
    {
        vty_error_out(vty,"Static nd add fail!Reason:Hash add fail!%s",VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/*add vlanif ipv6 neighbor*/
DEFUN(configure_static_vlanif_ndp,
    	 configure_static_vlanif_ndp_cmd,
    	 "ipv6 neighbor X:X:X:X:X:X:X:X mac XX:XX:XX:XX:XX:XX interface vlanif <1-4094> port  {ethernet USP|gigabitethernet USP|xgigabitethernet USP} [l3vpn <1-1024>]",
    	 "Ipv6\n"
    	 "Ipv6 neighbor\n"
    	 "Ipv6 address format X:X:X:X:X:X:X:X\n"
    	 "Mac address\n"
    	 "Mac format XX:XX:XX:XX:XX:XX\n"
    	 INTERFACE_STR
    	 CLI_INTERFACE_VLANIF_STR
         CLI_INTERFACE_VLANIF_VHELP_STR
         "The physical member port\n"
         CLI_INTERFACE_ETHERNET_STR
    	 CLI_INTERFACE_ETHERNET_VHELP_STR
    	 CLI_INTERFACE_GIGABIT_ETHERNET_STR
    	 CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
    	 CLI_INTERFACE_XGIGABIT_ETHERNET_STR
    	 CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
    	 "L3vpn \n"
    	 "L3vpn value <1-1024>\n")
{
    struct ndp_neighbor pndp;
    struct ndp_neighbor *rndp = NULL;
    struct route_if *pif = NULL;
    struct ipv6_addr    ipv6_add;
    uint32_t ifindex = 0;
    uint32_t port = 0;
    uint16_t vpnid = 0;
    uint8_t  mac[6];
	int ret = 0;

    /*参数获取及合法性检查*/
    ret = ndp_ipv6_str_check((char *)argv[0]);
	if (ERRNO_SUCCESS != ret)
	{
        vty_error_out(vty, "The specified IPV6 address is invalid.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
    
    memset(&ipv6_add, 0, sizeof(struct ipv6_addr));
    ret = inet_pton6((char *)argv[0], ipv6_add.ipv6);
    if(ret == 0)
    {
        vty_error_out(vty, "The specified IPV6 address is invalid.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	ret = ether_valid_mac((char *)argv[1]);
	if (1 == ret)
	{
		vty_error_out ( vty, "Please check out mac format.%s", VTY_NEWLINE );
		return CMD_WARNING;
	}
    ether_string_to_mac((char *)argv[1], mac);

    if (!ether_is_broadcast_mac(mac))
    {
        vty_error_out ( vty, "Can't set broadcast mac.%s", VTY_NEWLINE );
        return CMD_WARNING;
    }
	else if (mac[0] & 0x01)
    {
        vty_error_out ( vty, "Can't set multicast mac.%s", VTY_NEWLINE );
        return CMD_WARNING;
    }
    else if (!ether_is_zero_mac(mac))
    {
        vty_error_out ( vty, "Input mac is invalid.%s", VTY_NEWLINE );
        return CMD_WARNING;
    }

    if (NULL != argv[2])
    {
        ifindex = ifm_get_ifindex_by_name("vlanif", (char *)argv[2]);
    }
	if(ifindex == 0)
	{
		vty_error_out ( vty, "Please check out interface format.%s", VTY_NEWLINE );
		return CMD_WARNING;	
	}	


    if (NULL != argv[3] && NULL == argv[4] && NULL == argv[5])
    {
	    port = ifm_get_ifindex_by_name("ethernet", (char *)argv[3]);
    }
    else if(NULL == argv[3] && NULL != argv[4] && NULL == argv[5])
    {
        port = ifm_get_ifindex_by_name("gigabitethernet", (char *)argv[4]);
    }
	else if(NULL == argv[3] && NULL == argv[4] && NULL != argv[5])
    {
        port = ifm_get_ifindex_by_name("xgigabitethernet", (char *)argv[5]);
    }
	if(port == 0)
	{
		vty_error_out ( vty, "Please check out interface format.%s", VTY_NEWLINE );
		return CMD_WARNING;	
	}	
    
    if (NULL != argv[6])
    {
        vpnid = (uint16_t)atoi(argv[6]);
    }

    memset(&pndp, 0, sizeof(struct ndp_neighbor));
    memcpy(pndp.key.ipv6_addr, ipv6_add.ipv6, sizeof(struct ipv6_addr));
    pndp.key.vpnid = vpnid;
    pndp.key.ifindex = ifindex;
    memcpy(pndp.mac, mac, MAC_LEN);
    pndp.status  = NDP_STATUS_STATIC;
    pndp.port = port;

    //非配置恢复检查l3if 是否存在，不存在则添加失败
    if(1 != vty->config_read_flag)
    {
        pif = route_if_lookup(ifindex);
        if(!pif || !(NDP_IF_IPV6_EXIST(pif)))
        {
    		vty_error_out(vty,"Static ndp add fail,please add l3if ip first!%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    rndp = ndp_static_lookup((struct ipv6_addr *)(pndp.key.ipv6_addr), pndp.key.vpnid, pndp.key.ifindex);
    if (rndp)
    {
        vty_error_out(vty,"This ipv6 neighbor is exists!%s",VTY_NEWLINE);
        return CMD_SUCCESS;
    }

    ret = ndp_static_add(&pndp);
    if (ret == ERRNO_OVERSIZE)
    {
        vty_error_out(vty,"Static nd add fail! Reason:Over limit!%s",VTY_NEWLINE);
        return CMD_WARNING;
    }
    else if (ret == ERRNO_PARAM_ILLEGAL)
    {
        vty_error_out(vty,"Static nd add fail!Reason:param illegal!%s",VTY_NEWLINE);
        return CMD_WARNING;
    }
    else if (ret == ERRNO_IPC)
    {
        vty_error_out(vty,"Static nd add maybe failed!Reason:Ipc send to ftm error!%s",VTY_NEWLINE);
        return CMD_WARNING;
    }
    else if (ret == ERRNO_FAIL)
    {
        vty_error_out(vty,"Static nd add fail!Reason:Hash add fail!%s",VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

/*删除配置的ipv6 静态邻居*/
DEFUN(undo_configure_static_ndp,
    	 undo_configure_static_ndp_cmd,
    	 "no ipv6 neighbor X.X.X.X.X.X.X.X [l3vpn <1-1024>]",
    	 "Delete\n"
    	 "Ipv6\n"
    	 "Ipv6 neighbor\n"
    	 "Ipv6 address format X:X:X:X:X:X:X:X\n"
     	 "L3vpn\n"
       	 "L3vpn value <1-1024>\n")
{
    struct ipv6_addr ipv6_add;
    uint16_t vpnid = 0;
    int ret = 0;

    /*参数获取及合法性检查*/
    ret = ndp_ipv6_str_check((char *)argv[0]);
	if (ERRNO_SUCCESS != ret)
	{
        vty_error_out(vty, "The specified IPV6 address is invalid.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
    
    memset(&ipv6_add, 0, sizeof(struct ipv6_addr));
    ret = inet_pton6((char *)argv[0], ipv6_add.ipv6);
    if(ret == 0)
    {
        vty_error_out(vty, "The specified IPV6 address is invalid.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if(NULL != argv[1])
    {
        vpnid = (uint16_t)atoi(argv[1]);
    }
    
    ndp_static_delete_by_ip_vpn(&ipv6_add, vpnid);

    return CMD_SUCCESS;
}



/*接口模式下使能nd 学习*/
DEFUN(ndp_interface_enable,
		 ndp_interface_enable_cmd,
         "ipv6 ndp (enable | disable)",
         "Ipv6\n"
         "ndp command\n"
	     "Enable dynamic ndp learning\n")
{
	struct ifm_arp  *pndp = NULL;
    struct route_if *pif = NULL;
	struct ifm_arp  ifm_ndp;
    uint32_t ifindex = 0;
	int ret = 0;


    /*接口相关信息检查*/
	ifindex = (uint32_t)(vty->index);
    if(!ifindex)
    {
        vty_error_out(vty, "Get ifindex error!%s",VTY_NEWLINE);
        return CMD_WARNING;
    }
	pif = route_if_lookup(ifindex);
    if (!pif || !(NDP_IF_IPV6_EXIST(pif)))
    {
		vty_error_out(vty,"Ndp enable/disable fail,please check whether l3if ip is configured!%s",VTY_NEWLINE);
        return CMD_WARNING;   
    }

    /*参数设置*/
    memset(&ifm_ndp, 0, sizeof(struct ifm_arp));
    pndp = (struct ifm_arp *)(&(pif->arp));
    ifm_ndp.ifindex = ifindex;
    if(!strcmp(argv[0], "enable"))
    {
        if(pndp->ndp_disable == NDP_ENABLE_DEF)
        {
            vty_error_out(vty,"This l3 interface ndp is enable already!%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
        
        ifm_ndp.ndp_disable = NDP_ENABLE_DEF;
    }
    else if(!strcmp(argv[0], "disable"))
    {
        if(pndp->ndp_disable == NDP_DISABLE_DEF)
        {
            vty_error_out(vty,"This l3 interface ndp is disable already!%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        ifm_ndp.ndp_disable = NDP_DISABLE_DEF; 
    }


    /*通知ftm 更新接口nd 控制信息*/
    ret = ndp_if_update(&ifm_ndp, NDP_INFO_LEARN_LIMIT);
    if(ret != ERRNO_SUCCESS)
    {
        vty_error_out(vty,"Ndp enable/disable fail,please check ndp_if_update!%s",VTY_NEWLINE);
        return CMD_WARNING;
    }

	return CMD_SUCCESS;
}


/*接口nd 代理使能*/
DEFUN(ndp_proxy,                                                         
         ndp_proxy_cmd,                                                     
         "ipv6 ndp proxy",                                                                                                                
         "Ipv6\n"
         "Ndp command\n"
         "Ndp proxy\n")
{  
    struct route_if *pif = NULL;
	struct ifm_arp *pndp = NULL;
	struct ifm_arp ifm_ndp;
    uint32_t ifindex = 0;
    int ret = 0;

    /*接口相关信息检查*/
	ifindex = (uint32_t)(vty->index);
	pif = route_if_lookup(ifindex);
    if(!pif || !NDP_IF_IPV6_EXIST(pif))
	{
		vty_error_out(vty,"Ndp proxy set fail,please check whether l3if ip is configured!%s",VTY_NEWLINE);
		return CMD_WARNING;
	}
    pndp = (struct ifm_arp *)(&(pif->arp));
	if(NDP_DISABLE_DEF == pndp->ndp_disable)
	{
		vty_error_out(vty,"Ndp proxy set fail,please set ndp enable frist!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(NDP_PROXY_DEF == pndp->ndp_proxy )
	{
		vty_error_out(vty,"Ndp proxy already set !%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

    /*通知ftm 更新接口控制信息*/
	memset(&ifm_ndp, 0, sizeof(struct ifm_arp));
    ifm_ndp.ifindex = ifindex;
	ifm_ndp.ndp_proxy = 1;
	ret = ndp_if_update(&ifm_ndp , NDP_INFO_AGENT);
	if(ret != ERRNO_SUCCESS)
	{
		vty_error_out(vty,"Ndp proxy set enable fail,please check ndp_if_update!%s", VTY_NEWLINE);		
		return CMD_WARNING;
	}
    
	return CMD_SUCCESS;
}

/*接口nd 代理去使能*/
DEFUN(no_ndp_proxy,                                                         
         no_ndp_proxy_cmd,                                                     
         "no ipv6 ndp proxy ",                                                                                                                
         "Cancel current setting\n"
         "Ipv6\n"
         "Ndp\n"
         "Ndp proxy\n")
{  
    struct route_if *pif = NULL;
	struct ifm_arp *pndp = NULL;
	struct ifm_arp ifm_ndp;
    uint32_t ifindex = 0;
    int ret = 0;

    /*接口相关信息检查*/
	ifindex = (uint32_t)(vty->index);
	pif = route_if_lookup(ifindex);
    if(!pif || !NDP_IF_IPV6_EXIST(pif))
	{
		vty_error_out(vty,"Ndp proxy set fail,please check whether the l3if ip is configured!%s",VTY_NEWLINE);
		return CMD_WARNING;
	}
    pndp = (struct ifm_arp *)(&(pif->arp));
	if(NDP_DISABLE_DEF == pndp->ndp_disable)
	{
		vty_error_out(vty,"Ndp proxy set fail,please set ndp enable frist!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(NDP_NO_PROXY_DEF == pndp->ndp_proxy )
	{
		vty_error_out(vty,"No ndp proxy already set!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}


    /*通知ftm 更新接口控制信息*/
	memset(&ifm_ndp, 0, sizeof(struct ifm_arp));
    ifm_ndp.ifindex = ifindex;
	ifm_ndp.ndp_proxy = 0;
	ret = ndp_if_update(&ifm_ndp, NDP_INFO_AGENT);
	if (ret != ERRNO_SUCCESS)
	{
		vty_error_out(vty,"Ndp proxy set enable fail,please check ndp_if_update!%s", VTY_NEWLINE);		
		return CMD_WARNING;
	}
    
	return CMD_SUCCESS;
}


/*配置接口或全局ndp 学习数量限制*/
DEFUN (ndp_num_limit,                                                         
          ndp_num_limit_cmd,                                                     
          "ipv6 neighbor num-limit <0-1024>",                                                                                                                     
          "Ipv6 \n"
          "Ipv6 neighbor\n"
          "Ndp num-limit number\n")
{    
	struct ifm_arp *pndp = NULL;
    struct route_if *pif = NULL;
	struct ifm_arp ifm_ndp;
    uint32_t ifindex = 0;
	int ret = 0;

	if(vty->node == NDP_NODE)
	{
		gndp.num_limit = (uint32_t)atoi(argv[0]);
        
	    /*通知ftm 更新全局ndp 控制信息*/
        ret = ipc_send_msg_n2(&gndp, sizeof(struct arp_global), 1, MODULE_ID_FTM,
                           MODULE_ID_ROUTE, IPC_TYPE_NDP, NDP_INFO_NUM_LIMIT, 0, 0);
		if(ret < 0)
		{
			vty_error_out(vty,"Set global ndp num-limit fail!%s",VTY_NEWLINE);	
			return CMD_WARNING;
		}
	}
	else
	{	
	    /*接口相关信息检查*/
		ifindex = (uint32_t)(vty->index);
        pif = route_if_lookup(ifindex);
        if(!pif || !NDP_IF_IPV6_EXIST(pif))
		{
			vty_error_out(vty,"Ipv6 neighbor num-limit set fail,please check whether l3if ip is configured!%s",VTY_NEWLINE);
			return CMD_WARNING;
		}
        pndp = (struct ifm_arp *)(&(pif->arp));
		if(NDP_DISABLE_DEF == pndp->ndp_disable)
		{
			vty_error_out(vty,"Ndp num-limit set fail,please set ndp enable frist!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}

        /*通知ftm 更新接口控制信息*/
		memset(&ifm_ndp, 0, sizeof(struct ifm_arp));
	    ifm_ndp.ifindex = ifindex;
		ifm_ndp.neighbor_limit = (uint32_t)atoi(argv[0]);
		ret = ndp_if_update(&ifm_ndp, NDP_INFO_NUM_LIMIT);
		if (ret < 0)
		{
			vty_error_out(vty,"Set interface ipv6 neighbor num-limit fail!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}	

    return CMD_SUCCESS;
}


/*配置全局nd 表项可达时间*/
DEFUN (ndp_reach_time,                                                         
          ndp_reach_time_cmd,                                                     
          "ipv6 neighbor reachable-time <60-65535>",
          "Ipv6\n"
          "Ipv6 neighbor\n"
          "Ndp reachable-time number(in seconds)\n")
{
	int ret = 0;
	
	gndp.reach_time = (uint32_t)atoi(argv[0]);

    /*通知ftm 更新全局ndp 控制信息*/
    ret = ipc_send_msg_n2(&gndp, sizeof(struct arp_global), 1, MODULE_ID_FTM,
                         MODULE_ID_ROUTE, IPC_TYPE_NDP, NDP_INFO_REACH_TIME, 0, 0);
	if(ret < 0)
	{
		vty_error_out(vty,"Set ipv6 neighbor reachable-time fail!%s", VTY_NEWLINE);	
		return CMD_WARNING;
	}
    
	return CMD_SUCCESS;
}


/*配置nd 表项无效状态等待时间*/
DEFUN (ndp_stale_time,                                                         
          ndp_stale_time_cmd,                                                     
          "ipv6 neighbor stale-time <60-65535>",
          "Ipv6\n"
          "Ipv6 neighbor\n"
          "Ndp stale-timeout number(in seconds)\n")
{
	int ret = 0;
	
	gndp.stale_time = (uint32_t)atoi(argv[0]);

    /*通知ftm 更新全局ndp 控制信息*/
    ret = ipc_send_msg_n2(&gndp, sizeof(struct arp_global), 1, MODULE_ID_FTM,
                         MODULE_ID_ROUTE, IPC_TYPE_NDP, NDP_INFO_STALE_TIME, 0, 0);
	if(ret < 0)
	{
		vty_error_out(vty,"Set ipv6 neighbor stale-timeout time fail!%s", VTY_NEWLINE);	
		return CMD_WARNING;
	}
    
	return CMD_SUCCESS;
}



/*显示接口或全局ndp 配置*/
DEFUN (show_ndp_config,                                                         
          show_ndp_config_cmd,                                                     
          "show ipv6 ndp config", 
          SHOW_STR                                                                
          IPV6_STR
          "Ipv6 neighbor\n" 
          "Ipv6 neighbor config\n"
          )
{
    struct hash_bucket *pbucket = NULL;
    struct ifm_arp *pndp = NULL;
    struct route_if *pif = NULL;
	char ifname[IFNET_NAMESIZE];
	uint32_t ifindex = 0;
    uint32_t cursor = 0;

	if (vty->node != NDP_NODE)
	{
        ifindex = (uint32_t)(vty->index);
		pif = route_if_lookup(ifindex);

        //route_if 接口数据结构为空时，命令执行失败
        if(!pif || !NDP_IF_IPV6_EXIST(pif))
        {
            vty_error_out(vty,"Show ipv6 ndp config fail,please check whether the l3if ip is configured!%s",VTY_NEWLINE);
            return CMD_WARNING;
        }
       
        if(NDP_IF_IPV6_EXIST(pif))
		{
		    pndp = (struct ifm_arp *)(&(pif->arp));
			ifindex = pndp->ifindex;

			if (ifindex != 0)
			{
	            ifm_get_name_by_ifindex(pndp->ifindex, ifname);

				vty_out (vty, "interface %s%s", ifname, VTY_NEWLINE );
	    	    if(pndp->ndp_disable !=  NDP_ENABLE_DEF)
	    		    vty_out ( vty, " ipv6 ndp disable %s", VTY_NEWLINE );
	    		else
	    			vty_out ( vty, " ipv6 ndp enable %s", VTY_NEWLINE );

	        	if(pndp->ndp_proxy != NDP_NO_PROXY_DEF)
	    	    	vty_out ( vty, " ipv6 ndp proxy %s", VTY_NEWLINE );
	    		else
	    			vty_out ( vty, " no ipv6 ndp proxy %s", VTY_NEWLINE );
	    	
	    	    vty_out ( vty, " ipv6 neighbor num-limit %d%s", pndp->neighbor_limit, VTY_NEWLINE );
			}
    	}

		return CMD_SUCCESS;
	}


    /*未指定接口时显示全局nd 配置*/
	vty_out (vty, "Global ndp config%s", VTY_NEWLINE);
	vty_out (vty, " ipv6 neighbor reach-time %d%s", gndp.reach_time, VTY_NEWLINE);
	vty_out (vty, " ipv6 neighbor stale-time %d%s", gndp.stale_time, VTY_NEWLINE);
	vty_out (vty, " ipv6 neighbor fake-time %d%s", gndp.fake_time, VTY_NEWLINE);
	vty_out (vty, " ipv6 neighbor num-limit %d%s", gndp.num_limit, VTY_NEWLINE);

    HASH_BUCKET_LOOP( pbucket, cursor, route_if_table)
    {   
        if(pbucket->data)
        {   
            pif = pbucket->data;

            //接口ip 未配置
            if(!pif || !NDP_IF_IPV6_EXIST(pif))
                continue;

            /*loopback 接口、tunnel 接口不存在ndp配置信息*/
            pndp = (struct ifm_arp *)(&(pif->arp));
            ifindex = pndp->ifindex;
            if(IFM_TYPE_IS_LOOPBCK(ifindex) || IFM_TYPE_IS_TUNNEL(ifindex))
                continue;
            
            ifm_get_name_by_ifindex(pndp->ifindex, ifname);
            vty_out (vty, "interface %s%s", ifname, VTY_NEWLINE );
    	    if(pndp->ndp_disable !=  NDP_ENABLE_DEF)
    		    vty_out ( vty, " ipv6 ndp disable %s", VTY_NEWLINE );
    		else
    			vty_out ( vty, " ipv6 ndp enable %s", VTY_NEWLINE );

        	if(pndp->ndp_proxy != NDP_NO_PROXY_DEF)
    	    	vty_out ( vty, " ipv6 ndp proxy %s", VTY_NEWLINE );
    		else
    			vty_out ( vty, " no ipv6 ndp proxy %s", VTY_NEWLINE );
    	
    	    vty_out ( vty, " ipv6 neighbor num-limit %d%s", pndp->neighbor_limit, VTY_NEWLINE );
        }
    }
    
    return CMD_SUCCESS;
}



/*显示静态nd 表项*/
DEFUN(show_ipv6_neighbor_static,
         show_ipv6_neighbor_static_cmd,
         "show ipv6 neighbor static",
         SHOW_STR
         IPV6_STR
         "Ipv6 neighbor\n"
         "static ipv6 neighbor\n")
{
    struct hash_bucket *pbucket = NULL;
    struct ndp_neighbor *pndp = NULL;
    uint8_t ipv6_str[IPV6_ADDR_STRLEN];
    uint8_t ifname[IFNET_NAMESIZE];
    uint8_t mac[32];
    uint32_t cursor = 0;
    uint16_t num = 0;

    HASH_BUCKET_LOOP(pbucket, cursor, static_ndp_table)
    {
        if(pbucket->data)
        {
            pndp = (struct ndp_neighbor *)(pbucket->data);
            if(pndp->status != NDP_STATUS_STATIC)
                continue;

            //首次打印表头
            if(num == 0)
                vty_out(vty,"-------------------------------------------------%s", VTY_NEWLINE);

            num ++;
    
            //输出参数转换为字符串类型
            inet_ipv6tostr((struct ipv6_addr *)(pndp->key.ipv6_addr), (char *)ipv6_str, IPV6_ADDR_STRLEN);
            ether_mac_to_string((char *)(pndp->mac), (uchar *)mac);
            ifm_get_name_by_ifindex(pndp->key.ifindex, (char *)ifname);

            /*vty 输出到终端*/
            vty_out (vty, "%-60s%s", "IPV6 ADDRESS",VTY_NEWLINE);
            vty_out (vty, "%-60s%s", ipv6_str, VTY_NEWLINE);
            vty_out (vty, "%-20s ", "MAC ADDRESS");
            vty_out (vty, "%-20s ", "INTERFACE");
            vty_out (vty, "%-8s ",  "VPN");
            vty_out (vty, "%s",VTY_NEWLINE);
            vty_out (vty, "%-20s %-20s %-8d %s%s", mac, ifname, pndp->key.vpnid, VTY_NEWLINE,VTY_NEWLINE);            

            memset(ipv6_str, 0, INET6_ADDRSTRLEN);
            memset(ifname, 0, IFNET_NAMESIZE);
            memset(mac, 0, 32);
        }
    }
    
    if(num != 0)
    {
        vty_out(vty,"-------------------------------------------------%s", VTY_NEWLINE);
    	vty_out(vty,"STATIC NUM:%d %s", num, VTY_NEWLINE);	
    }

    return CMD_SUCCESS;
}


/*set incomplete ndp entry age time*/
DEFUN(ndp_fake_time_set,
      ndp_fake_time_set_cmd,
      "ndp-fake expire-time <1-30>",
      "Incomplete ndp entry\n"
      "Expire time\n"
      "Expire time range\n"
      )
{
    uint16_t expire_time = 0;
    int8_t   ret = 0;

    expire_time = atoi((char *)argv[0]);
    gndp.fake_time = expire_time; 
    ret = ipc_send_msg_n2(&gndp, sizeof(struct ndp_global), 1, MODULE_ID_FTM, 
                       MODULE_ID_ROUTE,IPC_TYPE_NDP,NDP_INFO_FAKE_TIME,IPC_OPCODE_UPDATE,0);

    if(ret == -1)
    {
        vty_out(vty, "Error:ipc send to ftm fail!%s",VTY_NEWLINE);
    }
    
    return CMD_SUCCESS;
}

DEFUN (ndp_conf_debug,
       ndp_conf_debug_cmd,
       "debug ndp (enable|disable) (all|ndp|other)",
       "Debug config\n"
       "Ndp config\n"
       "Ndp debug enable\n"
       "Ndp debug disable\n"
       "Ndp debug type all\n"
       "Ndp debug type arp\n"
       "Ndp debug type other\n")
{
    int enable = 0;
    unsigned int type = 0;

    if(argv[0][0] == 'e') enable = 1;

    if(strcmp(argv[1],"other") == 0)
        type = NDP_DEBUG_OTHER;
    else if(strcmp(argv[1],"ndp") == 0)
        type = NDP_DEBUG_LOG;
    else type = NDP_DEBUG_ALL;

    zlog_debug_set(vty, type,  enable);

    return(CMD_SUCCESS);
}


/* 静态 ndp 的配置保存 */
static int ndp_static_config_write(struct vty *vty)
{	
    struct ndp_neighbor *pndp = NULL;
    struct hash_bucket  *pbucket = NULL;
    uint8_t ifname[IFNET_NAMESIZE];
    uint8_t ipv6_str[IPV6_ADDR_STRLEN];
    uint8_t mac[20];
    int cursor = 0;

    //配置恢复时进入ndp 视图
	vty_out(vty, "ndp%s", VTY_NEWLINE);
	if(gndp.reach_time != 1800)
	{
		vty_out(vty, " ipv6 neighbor reachable-time %d%s",gndp.reach_time, VTY_NEWLINE );
	}

    if(gndp.stale_time != 1200)
    {
    	vty_out(vty, " ipv6 neighbor stale-time %d%s",gndp.stale_time, VTY_NEWLINE );
    }
    
	if(gndp.num_limit != 1024)
	{
		vty_out ( vty, " ipv6 neighbor num-limit %d%s",gndp.num_limit, VTY_NEWLINE );
	}
    
    HASH_BUCKET_LOOP(pbucket, cursor,static_ndp_table)
    {
    	if(!(pbucket->data))
            continue;
	
		pndp = (struct ndp_neighbor *)(pbucket->data);
        inet_ipv6tostr((struct ipv6_addr *)(pndp->key.ipv6_addr), (char *)ipv6_str, IPV6_ADDR_STRLEN);
		ifm_get_name_by_ifindex (pndp->key.ifindex, (char *)ifname);
        ether_mac_to_string((char *)(pndp->mac), (uchar *)mac);

        vty_out(vty," ipv6 neighbor %s mac %s interface %s",ipv6_str, mac, ifname);
        if(IFM_TYPE_IS_VLANIF(pndp->key.ifindex))
        {
            ifm_get_name_by_ifindex(pndp->port, (char *)ifname);
            vty_out(vty," port %s",ifname);
        }
        
        if(0 !=  pndp->key.vpnid)
        {
        	vty_out(vty, " l3vpn %d ",pndp->key.vpnid);
        }
        vty_out (vty, "%s", VTY_NEWLINE);
    }

	return ERRNO_SUCCESS;
}

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



/* 静态 ndp 的命令初始化*/
void ndp_static_cmd_init(void)
{
    install_node(&ndp_node, ndp_static_config_write);

	install_default(NDP_NODE);
    install_element(CONFIG_NODE, &ndp_mode_cmd, CMD_SYNC);
    install_element(NDP_NODE, &show_ndp_config_cmd, CMD_SYNC);
    install_element(NDP_NODE, &show_ipv6_neighbor_static_cmd, CMD_SYNC);
    install_element(NDP_NODE, &configure_static_ndp_cmd, CMD_SYNC);
    install_element(NDP_NODE, &configure_static_vlanif_ndp_cmd, CMD_SYNC);
    install_element(NDP_NODE, &undo_configure_static_ndp_cmd, CMD_SYNC);
	install_element(NDP_NODE, &ndp_num_limit_cmd, CMD_SYNC);
    install_element(NDP_NODE, &ndp_reach_time_cmd, CMD_SYNC);
	install_element(NDP_NODE, &ndp_stale_time_cmd, CMD_SYNC);
    install_element(NDP_NODE, &ndp_fake_time_set_cmd, CMD_SYNC);
	install_element(NDP_NODE, &ndp_conf_debug_cmd, CMD_LOCAL);
}


/* ndp 配置命令初始化 */
void ndp_cmd_init(void)
{
    ndp_static_cmd_init();

#define NDP_INSTALL_ELEMENT_L3IF(cmd, flag)\
        install_element (PHYSICAL_IF_NODE, cmd, flag);\
        install_element (PHYSICAL_SUBIF_NODE, cmd, flag);\
        install_element (TRUNK_IF_NODE, cmd, flag);\
        install_element (TRUNK_SUBIF_NODE, cmd, flag);\
        install_element (VLANIF_NODE, cmd, flag);

    NDP_INSTALL_ELEMENT_L3IF(&ndp_interface_enable_cmd, CMD_SYNC);
    NDP_INSTALL_ELEMENT_L3IF(&ndp_num_limit_cmd, CMD_SYNC);
    NDP_INSTALL_ELEMENT_L3IF(&ndp_proxy_cmd, CMD_SYNC);
    NDP_INSTALL_ELEMENT_L3IF(&no_ndp_proxy_cmd, CMD_SYNC);
	NDP_INSTALL_ELEMENT_L3IF(&show_ndp_config_cmd, CMD_SYNC);
}


