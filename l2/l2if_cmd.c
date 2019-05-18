#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <lib/command.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/hash1.h>
#include <lib/ifm_common.h>
#include <lib/linklist.h>
#include <lib/msg_ipc.h>
#include <lib/zassert.h>
#include <lib/log.h>
#include <lib/errcode.h>
#include <syslog/syslogd.h>
#include "vlan.h"
#include "l2_if.h"
#include "qinq.h"
#include "lldp/lldp.h"
#include "trunk.h"
#include "mac_static.h"
#include "loopd/loop_detect.h"
#include "mstp/mstp.h"
#include "mstp/mstp_port.h"
#include "mstp/mstp_bpdu.h"
#include "efm/efm.h"
#include "l2cp/l2cp.h"
#include "port_isolate.h"
#include "l2_msg.h"
#include "l2_msg.h"

extern lldp_global lldp_global_msg;

const struct message l2_dbg_name[] = {
         {.key = L2_DBG_ELPS,     .str = "elps"},
         {.key = L2_DBG_ERPS,     .str = "erps"},
         {.key = L2_DBG_LACP,     .str = "lacp"},
         {.key = L2_DBG_LLDP,     .str = "lldp"},
         {.key = L2_DBG_MSTP, 	  .str = "mstp"},
         {.key = L2_DBG_TRUNK,    .str = "trunk"},
         {.key = L2_DBG_MAC,      .str = "mac"},
         {.key = L2_DBG_QINQ,     .str = "qinq"},
         {.key = L2_DBG_VLAN,     .str = "vlan"},
         {.key = L2_DBG_EFM, 	  .str = "efm"},
         {.key = L2_DBG_COMMON,    .str = "common"},
         {.key = L2_DBG_ALL,      .str = "all"},
};


uchar l2if_a_mode_switch[3][25]=
{
    "hybrid"
    ,"trunk"
    ,"access"
};

uchar l2if_a_mac_learn[2][10] = {"enable","disable"};
uchar l2if_a_storm_suppress[2][10] = {"disable","enable"};

uchar mac_limit_action[2][10] = {"discard","forward"};
uchar switch_vlan_tag[2][10] = {"tagged","untagged"};

uchar l2if_a_vlan_action[4][25] =
{
    "NO"
    ,"ADD"
    ,"DELETE"
    ,"TRANSLATE"
};
void l2if_reserve_vlan_config_write(struct vty *vty,uint32_t ifindex);

int reserve_vlan_table[6] = {DEF_RESV_VLAN1,DEF_RESV_VLAN2,DEF_RESV_VLAN3,DEF_RESV_VLAN4,DEF_RESV_VLAN5,DEF_RESV_VLAN6};

//Edit by jhz,20180313,The vlan linked list shown is modified by vlan_list to vlan_tag_list,and it shows that the vlan is tagged or untagged.
DEFUN(show_switch_interface,
      show_switch_interface_cmd,
      "show switch interface",
      "Show command \n"
      "Mode switch \n"
      "Interface info\n"
     )
{
    uint16_t num = 0;
    uint16_t tag_num = 0;
    uint16_t untag_num = 0;
    uint32_t ifindex = 0;
    void *pdata = NULL;
    int i = 0;
    int j = 1;
    struct listnode *p_listnode = NULL;
    struct l2if *p_if_l2 = NULL;
    struct vlan_mapping *p_vlan_mapping = NULL;
    struct vlan_tagged *pvlan_tagged = NULL;
    uint8_t  mode = 0;
    uint32_t vlan_id=0;
    u_char switch_vlan_tag_count[4095];
    u_char switch_vlan_untag_count[4095];
    uint32_t switch_vlan_count[4094][3];
    uint32_t temp[3];
    uint16_t line_num = 0;
    uint8_t storm_suppress = 0;
    uint32_t storm_rate = 0;

    /*get ifindex from interface we use*/
    ifindex = (uint32_t)vty->index;

    /*检查mode是否是switch*/
    if(ifm_get_mode(ifindex,MODULE_ID_L2, &mode) != 0)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to get mode from ifm\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Fail to get interface mode%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    if((uint8_t)IFNET_MODE_SWITCH != mode)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s,mode != IFNET_MODE_SWITCH\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*在哈希表搜寻ifindex对应节点，如果没有则创建*/
    p_if_l2 = l2if_get(ifindex);
    if(NULL == p_if_l2)
    {
        vty_error_out (vty, "Fail to CALLOC if l2 table bucket %s", VTY_NEWLINE);
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to CALLOC if l2 table bucket\n",__FILE__,__LINE__,__FUNCTION__);
        return CMD_WARNING;
    }
    /*输出l2if配置信息*/
    vty_out (vty, "mode         : %s%s",l2if_a_mode_switch[p_if_l2->switch_info.mode], VTY_NEWLINE);
    if(p_if_l2->switch_info.limit_num!=0)
    {
        vty_out ( vty, "mac  limit   : %d%s", p_if_l2->switch_info.limit_num,VTY_NEWLINE );
        vty_out ( vty, "limit action : %s%s",mac_limit_action[p_if_l2->switch_info.limit_action], VTY_NEWLINE);
    }

    if(DEFAULT_VLAN_ID == p_if_l2->switch_info.access_vlan)
        vty_out (vty, "access_vlan  : %s%s","-", VTY_NEWLINE);
    else
        vty_out (vty, "access_vlan  : %d%s",p_if_l2->switch_info.access_vlan, VTY_NEWLINE);
    if(IFM_TYPE_IS_METHERNET(ifindex))
    {
        if(DEFAULT_VLAN_ID == p_if_l2->switch_info.dot1q_tunnel)
            vty_out (vty, "dot1q_tunnel : %s%s","-", VTY_NEWLINE);
        else
            vty_out (vty, "dot1q_tunnel : %d%s",p_if_l2->switch_info.dot1q_tunnel, VTY_NEWLINE);

        if(8 == p_if_l2->switch_info.dot1q_cos)
            vty_out (vty, "dot1q_cos    : %s%s","-", VTY_NEWLINE);
        else
            vty_out (vty, "dot1q_cos    : %d%s",p_if_l2->switch_info.dot1q_cos, VTY_NEWLINE);

        vty_out (vty, "----------------------------------------------------%s",VTY_NEWLINE);

        vty_out (vty, "%-12s  %-15s  %-6s%s","type", "storm-suppress","rate(kilobits/s)",VTY_NEWLINE);

        storm_suppress = p_if_l2->switch_info.storm_control.storm_unicast;
        storm_rate=p_if_l2->switch_info.storm_control.unicast_rate;
        if(ENABLE==storm_suppress)
        {
            vty_out (vty, "%-12s  %-15s  %-6d%s","unicast",  l2if_a_storm_suppress[storm_suppress],storm_rate,VTY_NEWLINE);
        }
        else
        {
            vty_out (vty, "%-12s  %-15s  %s","unicast",  l2if_a_storm_suppress[storm_suppress],VTY_NEWLINE);
        }

        storm_suppress = p_if_l2->switch_info.storm_control.storm_broadcast;
        storm_rate=p_if_l2->switch_info.storm_control.broadcast_rate;
        if(ENABLE==storm_suppress)
        {
            vty_out (vty, "%-12s  %-15s  %-6d%s","broadcast",  l2if_a_storm_suppress[storm_suppress],storm_rate,VTY_NEWLINE);
        }
        else
        {
            vty_out (vty, "%-12s  %-15s  %s","broadcast",  l2if_a_storm_suppress[storm_suppress],VTY_NEWLINE);
        }

        storm_suppress = p_if_l2->switch_info.storm_control.storm_multicast;
        storm_rate=p_if_l2->switch_info.storm_control.multicast_rate;
        if(ENABLE==storm_suppress)
        {
            vty_out (vty, "%-12s  %-15s  %-6d%s","multicast",  l2if_a_storm_suppress[storm_suppress],storm_rate,VTY_NEWLINE);
        }
        else
        {
            vty_out (vty, "%-12s  %-15s  %s","multicast",  l2if_a_storm_suppress[storm_suppress],VTY_NEWLINE);
        }

        vty_out (vty, "----------------------------------------------------%s",VTY_NEWLINE);
        /*计算qinq个数*/
        num = 0;
        for(ALL_LIST_ELEMENTS_RO(p_if_l2->switch_info.qinq_list, p_listnode, p_vlan_mapping))
            num++;

        vty_out (vty, "Total QinQ Num:%d%s", num,VTY_NEWLINE);
        /*输出qinq信息*/
        if(0 != num)
        {
            num = 1;
            //		vty_out (vty, "id     cvlan       cvlan_new   svlan       svlan_new   svlan_cos%s",VTY_NEWLINE);
            vty_out (vty, "id     svlan       cvlan       svlan_new   svlan_cos   cvlan_new%s",VTY_NEWLINE);
            for(ALL_LIST_ELEMENTS_RO(p_if_l2->switch_info.qinq_list, p_listnode, p_vlan_mapping))
            {
                /*显示id*/
                vty_out (vty, "%-7d",num);

                /*显示svlan*/
                if(0 == p_vlan_mapping->svlan.vlan_start)
                    vty_out (vty, "%-12s","-");
                else
                {
                    if(p_vlan_mapping->svlan.vlan_start == p_vlan_mapping->svlan.vlan_end)
                        vty_out (vty, "%-12d",p_vlan_mapping->svlan.vlan_start);
                    else
                        vty_out (vty, "%-4d-%-7d",p_vlan_mapping->svlan.vlan_start,p_vlan_mapping->svlan.vlan_end);
                }

                /*显示cvlan*/
                if(0 == p_vlan_mapping->cvlan.vlan_start)
                    vty_out (vty, "%-12s","-");
                else
                {
                    if(p_vlan_mapping->cvlan.vlan_start == p_vlan_mapping->cvlan.vlan_end)
                        vty_out (vty, "%-12d",p_vlan_mapping->cvlan.vlan_start);
                    else
                        vty_out (vty, "%-4d-%-7d",p_vlan_mapping->cvlan.vlan_start,p_vlan_mapping->cvlan.vlan_end);
                }

                /*显示svlan_new*/
                if(0 == p_vlan_mapping->svlan_new.vlan_start)
                    vty_out (vty, "%-12s","-");
                else
                {
                    if(p_vlan_mapping->svlan_new.vlan_start == p_vlan_mapping->svlan_new.vlan_end)
                        vty_out (vty, "%-12d",p_vlan_mapping->svlan_new.vlan_start);
                    else
                        vty_out (vty, "%-4d-%-7d",p_vlan_mapping->svlan_new.vlan_start,p_vlan_mapping->svlan_new.vlan_end);
                }

                /*显示svlan_new cos*/
                if(8 != p_vlan_mapping->svlan_cos)
                    vty_out (vty, "%-12d",p_vlan_mapping->svlan_cos);
                else
                    vty_out (vty, "%-12s","-");

                /*显示cvlan_new*/
                if(0 == p_vlan_mapping->cvlan_new.vlan_start)
                    vty_out (vty, "%-12s%s","-",VTY_NEWLINE);
                else
                {
                    if(p_vlan_mapping->cvlan_new.vlan_start == p_vlan_mapping->cvlan_new.vlan_end)
                        vty_out (vty, "%-12d%s",p_vlan_mapping->cvlan_new.vlan_start,VTY_NEWLINE);
                    else
                        vty_out (vty, "%-4d-%-7d%s",p_vlan_mapping->cvlan_new.vlan_start,p_vlan_mapping->cvlan_new.vlan_end,VTY_NEWLINE);
                }

                num ++;
            }
        }
    }
    vty_out (vty, "----------------------------------------------------%s",VTY_NEWLINE);

    /*count switch vlan*/
    memset(switch_vlan_tag_count,0,sizeof(switch_vlan_tag_count));
    memset(switch_vlan_untag_count,0,sizeof(switch_vlan_untag_count));

    if(listhead(p_if_l2->switch_info.vlan_tag_list) != NULL)
    {
        for(ALL_LIST_ELEMENTS_RO(p_if_l2->switch_info.vlan_tag_list, p_listnode, pdata))
        {
            pvlan_tagged=(struct vlan_tagged*)pdata;
            if(pvlan_tagged->tag==TAGGED)
            {
                switch_vlan_tag_count[pvlan_tagged->vlanid] =0x1;
                tag_num++;
            }
            else if(pvlan_tagged->tag==UNTAGGED)
            {
                switch_vlan_untag_count[pvlan_tagged->vlanid] =0x1;
                untag_num++;
            }
        }
    }
    if(DEFAULT_VLAN_ID != p_if_l2->switch_info.access_vlan)
    {
        vlan_id=p_if_l2->switch_info.access_vlan;
        switch_vlan_untag_count[vlan_id] =0x1;
        untag_num++;
    }

    vty_out (vty, "Total switch vlan Num:%d%s", tag_num+untag_num,VTY_NEWLINE);
    if(tag_num+untag_num)
    {
        vty_out (vty, "id   vlan_start   vlan_end   tagged/untagged%s",VTY_NEWLINE);
        /*tag vlan*/
        memset(switch_vlan_count, 0, sizeof(switch_vlan_count));
        if(tag_num)
        {
            for(vlan_id=0; vlan_id<4095; vlan_id++)
            {
                if(0x1==switch_vlan_tag_count[vlan_id])
                {
                    switch_vlan_count[line_num][0] = vlan_id;
                    switch_vlan_count[line_num][1] = vlan_id;
                    switch_vlan_count[line_num][2] = TAGGED;
                    break;
                }
            }
            for(vlan_id=0; vlan_id<4095; vlan_id++)
            {
                if(0x1==switch_vlan_tag_count[vlan_id])
                {
                    if(vlan_id == (switch_vlan_count[line_num][1] + 1))
                    {
                        switch_vlan_count[line_num][1] = vlan_id;
                    }
                    else if(vlan_id != switch_vlan_count[line_num][0])
                    {
                        line_num++;
                        switch_vlan_count[line_num][0] = vlan_id;
                        switch_vlan_count[line_num][1] = vlan_id;
                        switch_vlan_count[line_num][2] = TAGGED;
                    }
                }
            }
        }
        if(untag_num)
        {
            for(vlan_id=0; vlan_id<4095; vlan_id++)
            {
                if(0x1==switch_vlan_untag_count[vlan_id])
                {
                    if(tag_num)
                    {
                        line_num++;
                    }
                    switch_vlan_count[line_num][0] = vlan_id;
                    switch_vlan_count[line_num][1] = vlan_id;
                    switch_vlan_count[line_num][2] = UNTAGGED;
                    break;
                }
            }
            for(vlan_id=0; vlan_id<4095; vlan_id++)
            {
                if(0x1==switch_vlan_untag_count[vlan_id])
                {
                    if(vlan_id == (switch_vlan_count[line_num][1] + 1))
                    {
                        switch_vlan_count[line_num][1] = vlan_id;
                    }
                    else if(vlan_id != switch_vlan_count[line_num][0])
                    {
                        line_num++;
                        switch_vlan_count[line_num][0] = vlan_id;
                        switch_vlan_count[line_num][1] = vlan_id;
                        switch_vlan_count[line_num][2] = UNTAGGED;
                    }
                }
            }
        }

        /*排序switch vlan*/
        for(i=0; i<line_num; i++)
        {
            for(int j = 0; j<(line_num-i); j++) // j开始等于0，
            {
                if(switch_vlan_count[j][0] > switch_vlan_count[j+1][0])
                {
                    memcpy(temp,switch_vlan_count[j],sizeof(temp));
                    memcpy(switch_vlan_count[j],switch_vlan_count[j+1],sizeof(temp));
                    memcpy(switch_vlan_count[j+1],temp,sizeof(temp));
                }
            }
        }

        /*输出switch vlan*/
        for(i=0; i<=line_num; i++)
        {
            j=switch_vlan_count[i][2];
            vty_out(vty,"%-4d %-10d   %-10d %s%s",i+1, switch_vlan_count[i][0], switch_vlan_count[i][1],switch_vlan_tag[j],VTY_NEWLINE);
        }
    }
    vty_out (vty, "----------------------------------------------------%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}

DEFUN(l2_mode_switch,
      l2_mode_switch_cmd,
      "switch (hybrid | access | trunk )",
      "Choice mode\n"
      "Switch hybrid\n"
      "Switch access\n"
      "Switch trunk\n"
     )
{
    enum SWITCH_MODE switch_mode = SWITCH_MODE_HYBRID;
    uint32_t ifindex = 0;
    uchar mode_type[3][10] = {"hybrid","trunk","access"};
    uint8_t  mode = 0;
    struct l2if *p_l2if = NULL;

    /*get ifindex from interface we use*/
    ifindex = (uint32_t)vty->index;

    /*检查mode是否是switch*/
    if(ifm_get_mode(ifindex,MODULE_ID_L2, &mode) != 0)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to get mode from ifm\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Fail to get interface mode%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    if((uint8_t)IFNET_MODE_SWITCH != mode)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s,mode != IFNET_MODE_SWITCH\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if(1 == vty->config_read_flag)//配置恢复，trunk接口创建后切换switch mode时，下发静态mac至hal
    {
        if(IFM_TYPE_IS_TRUNK(ifindex))
        {
            mac_static_add_by_ifindex(ifindex);
            L2_COMMON_LOG_DBG("%s[%d] mac_static_add_by_ifindex \n", __FUNCTION__, __LINE__);
        }
    }

    /*在哈希表搜寻ifindex对应节点，如果没有则创建*/
    p_l2if = l2if_get(ifindex);
    if(NULL == p_l2if)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to CALLOC if l2 table bucket\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Fail to CALLOC if l2 table bucket %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if(0 == strncmp("hy", argv[0],strlen("hy")))
        switch_mode = SWITCH_MODE_HYBRID;
    else if(0 == strncmp("ac", argv[0],strlen("ac")))
        switch_mode = SWITCH_MODE_ACCESS;
    else if(0 == strncmp("tr", argv[0],strlen("tr")))
        switch_mode = SWITCH_MODE_TRUNK;
    else
    {
        vty_error_out (vty, "Invalid input mode type%s", VTY_NEWLINE);
        L2_COMMON_LOG_DBG("%s[%d]:%s:invalid input mode type\n",__FILE__,__LINE__,__FUNCTION__);
        return CMD_WARNING;
    }

    /*如果模式不发生切换，则不执行函数*/
    if(p_l2if->switch_info.mode != switch_mode)
    {
        if(ERRNO_FAIL == l2if_set_switch_mode(p_l2if, switch_mode))
        {
            vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
            L2_COMMON_LOG_DBG("%s[%d]:%s:fail to set switch mode\n",__FILE__,__LINE__,__FUNCTION__);
            return CMD_WARNING;
        }
    }

    L2_COMMON_LOG_DBG("%s[%d]:%s:sucessfully set switch mode = %s\n",__FILE__,__LINE__,__FUNCTION__,mode_type[switch_mode]);
    return CMD_SUCCESS;
}

DEFUN(no_switch_access_vlan,
      no_switch_access_vlan_cmd,
      "no switch access-vlan",
      "No command\n"
      "Mode switch \n"
      "Access vlanid\n"
     )
{
    uint32_t ifindex = 0;
    int ret = 0;
    uint8_t mode = 0;
    struct l2if *p_l2if = NULL;
	int i,port = 0;

    ifindex = (uint32_t)vty->index;

    if(ifm_get_mode(ifindex,MODULE_ID_L2, &mode) != 0)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to get mode from ifm\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Fail to get interface mode%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    if((uint8_t)IFNET_MODE_SWITCH != mode)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s,mode != IFNET_MODE_SWITCH\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    p_l2if = l2if_lookup(ifindex);
    if(NULL == p_l2if)
    {
        vty_error_out (vty, "Fail to set no access vlan,lookup error%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }

	if( devtype == ID_HT157 || devtype == ID_HT158 )
	{
		if(IFM_TYPE_IS_TRUNK(ifindex) && !(IFM_TYPE_IS_SUBPORT(ifindex)))
		{
			for(i = 0; i < 3; i++)
			{
				if(trunk_reserve_vlan_table[i].trunkid == IFM_TRUNK_ID_GET(ifindex))
				{
					if(trunk_reserve_vlan_table[i].reserve_vlan_bak != p_l2if->switch_info.access_vlan)
					{
						ret = l2if_delete_access_vlan(p_l2if);
				        if(ERRNO_FAIL == ret)
				        {
				            L2_COMMON_LOG_DBG("%s[%d]:%s:fail to set no access vlan\n",__FILE__,__LINE__,__FUNCTION__);
				            vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
				            return CMD_WARNING;
				        }
					}
					else
						vty_error_out (vty, "Failed:access-vlan already empty%s",VTY_NEWLINE);
				}
			}
		}
		else
		{
		    port = IFM_PORT_ID_GET ( ifindex );
		    if( port < 1 || port > 6 )
		    {
		        L2_COMMON_LOG_DBG ("Invalid port number!\n");
		        return -1;
		    }
			if((DEF_RESV_VLAN1 + port - 1) != p_l2if->switch_info.access_vlan)
			{
		        ret = l2if_delete_access_vlan(p_l2if);
		        if(ERRNO_FAIL == ret)
		        {
		            L2_COMMON_LOG_DBG("%s[%d]:%s:fail to set no access vlan\n",__FILE__,__LINE__,__FUNCTION__);
		            vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
		            return CMD_WARNING;
		        }
			}
			else
				vty_error_out (vty, "Failed:access-vlan already empty%s",VTY_NEWLINE);
		}
	}
	else
	{
	    if(DEFAULT_VLAN_ID != p_l2if->switch_info.access_vlan)
	    {
	        ret = l2if_delete_access_vlan(p_l2if);
	        if(ERRNO_FAIL == ret)
	        {
	            L2_COMMON_LOG_DBG("%s[%d]:%s:fail to set no access vlan\n",__FILE__,__LINE__,__FUNCTION__);
	            vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
	            return CMD_WARNING;
	        }
	    }
	    else
	        vty_error_out (vty, "Failed:access-vlan already empty%s",VTY_NEWLINE);
	}

    return CMD_SUCCESS;
}

/*Edit by jhz,20180313,When the interface mode is modified to switch mode, the switch access-vlan can be configured.*/
DEFUN(switch_access_vlan,
      switch_access_vlan_cmd,
      "switch access-vlan <1-4094>",
      "Mode switch\n"
      "Access-vlan\n"
      "Vlanid<1-4094>\n"
     )
{
    uint32_t ifindex = 0;
    uint16_t vlanid = 0;
    int ret = 0;
    uint8_t mode = 0;
    struct l2if *p_l2if = NULL;

    vlanid = atoi(argv[0]);

    ifindex = (uint32_t)vty->index;

    if(ifm_get_mode(ifindex,MODULE_ID_L2, &mode) != 0)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to get mode from ifm\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Fail to get interface mode%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    if((uint8_t)IFNET_MODE_SWITCH != mode)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s,mode != IFNET_MODE_SWITCH\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*在哈希表搜寻ifindex对应节点，如果没有则创建*/
    p_l2if = l2if_get(ifindex);
    if(NULL == p_l2if)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to CALLOC if l2 table bucket\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Fail to CALLOC if l2 table bucket %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*检查dot1q tunnel是否为空*/
    if(DEFAULT_VLAN_ID != p_l2if->switch_info.dot1q_tunnel)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error: dot1q tunnel != 0\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Set access vlan error:Please delete dot1q_tunnel first.%s",VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*access vlanid不能与vlanlist的id重复*/
    if(ERRNO_SUCCESS == check_input_in_vlanlist(p_l2if,vlanid))
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error: access vlanid must not include in vlanlist\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Set access vlan error:access vlanid must not include in vlanlist.%s",VTY_NEWLINE);
        return ERRNO_FAIL;
    }


    if(DISABLE==vlan_table[vlanid]->vlan.enable)
    {
        vty_error_out (vty, "Vlanid = %d is not created%s",vlanid, VTY_NEWLINE);
        return CMD_WARNING;
    }

    ret = l2if_add_access_vlan(p_l2if,vlanid);
    if(ERRNO_FAIL == ret)
    {
        L2_COMMON_LOG_DBG("%s[%d]:%s:fail to set access vlan\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    L2_COMMON_LOG_DBG("%s[%d]:%s:p_if_l2->access_vlan = %d \n",__FILE__,__LINE__,__FUNCTION__,vlanid);

    return CMD_SUCCESS;
}

DEFUN(no_switch_vlan_add,
      no_switch_vlan_add_cmd,
      "no switch vlan  <1-4094> {to <1-4094>}",
      "No command\n"
      "Mode switch\n"
      "Switch vlan range\n"
      "Vlanid start <1-4094>\n"
      "To\n"
      "Vlanid end <1-4094>\n"
     )
{
    uint32_t ifindex = 0;
    int ret = 0;
    uint16_t v_start = 0;
    uint16_t v_end = 0;
    struct vlan_range switch_vlan;
    struct l2if *p_l2if = NULL;
    uint8_t mode = 0;
    uint8_t flag=0;

    ifindex = (uint32_t)vty->index;

    /*检查mode是否是switch*/
    if(ifm_get_mode(ifindex,MODULE_ID_L2, &mode) != 0)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to get mode from ifm\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Fail to get interface mode%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    if((uint8_t)IFNET_MODE_SWITCH != mode)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s,mode != IFNET_MODE_SWITCH\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    p_l2if = l2if_lookup(ifindex);
    if(NULL == p_l2if)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:invalid ifindex\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "No switch vlan failed,lookup error%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    v_start = atoi(argv[0]);
    if(NULL == argv[1])
        v_end = v_start;
    else
        v_end = atoi(argv[1]);

    memset(&switch_vlan, 0, sizeof(struct vlan_range));
    switch_vlan.vlan_start = v_start;
    switch_vlan.vlan_end= v_end;

    if(v_start > v_end)
    {
        L2_COMMON_LOG_DBG("%s[%d]:%s,v_end(%d) must not less than v_start(%d)\n",__FILE__,__LINE__,__FUNCTION__, v_end, v_start);
        vty_error_out (vty, "Vlan input error,vlanid_end(%d) must not less than vlanid_start(%d) !%s",v_end,v_start, VTY_NEWLINE);
        return CMD_WARNING;
    }

    ret = l2if_delete_vlan(p_l2if,&switch_vlan,flag);
    if(ERRNO_NOT_FOUND == ret)
    {
        L2_COMMON_LOG_DBG("%s[%d]:%s:no switch vlan<%d-%d> failed\n",__FILE__,__LINE__,__FUNCTION__, v_start, v_end);
        vty_error_out (vty, "No switch vlan failed,vlan input error%s",VTY_NEWLINE);
        return CMD_WARNING;
    }
    else if(ERRNO_FAIL == ret)
    {
        L2_COMMON_LOG_DBG("%s[%d]:%s:fail to find switch vlan<%d-%d>\n",__FILE__,__LINE__,__FUNCTION__, v_start, v_end);
        vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    L2_COMMON_LOG_DBG("%s[%d]:%s:no switch vlan %d to %d,ifindex = %#x\n",__FILE__,__LINE__,__FUNCTION__, v_start, v_end,ifindex);
	mstp_port_vlan_change(p_l2if);

    return CMD_SUCCESS;
}

/*Edit by jhz,20180313,This command line applies only to the switch trunk mode, and when Hal configures a vlan, tagged tags are added.*/
DEFUN(switch_vlan_add,
      switch_vlan_add_cmd,
      "switch vlan  <1-4094> {to <1-4094>}",
      "Mode switch\n"
      "Switch vlan\n"
      "Vlanid start <1-4094>\n"
      "To\n"
      "Vlanid end <1-4094>\n"
     )
{
    uint32_t ifindex = 0;
    uint16_t v_start = 0;
    uint16_t v_end = 0;
    uint16_t repeat_v_start=0,repeat_v_end=0;
    uint16_t check_vlanid = 0;
    uint8_t flag = 0;
    int ret = 0;
    uint8_t mode = 0;
    struct vlan_range switch_vlan;
    struct l2if *p_l2if = NULL;
    struct listnode *p_listnode = NULL;
    struct vlan_tagged *pvlan_tagged = NULL;
    uint32_t switch_vlan_count[4094][2];
    uint16_t line_num = 0;
    int iRet = 0;
    int i = 0;
    uint16_t vlan_id;
    u_char vlan_send[4095];

    ifindex = (uint32_t)vty->index;

    /*检查mode是否为switch*/
    if(ifm_get_mode(ifindex,MODULE_ID_L2, &mode) != 0)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to get mode from ifm\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Fail to get interface mode%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    if((uint8_t)IFNET_MODE_SWITCH != mode)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s,mode != IFNET_MODE_SWITCH\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*在哈希表搜寻ifindex对应节点，如果没有则创建*/
    p_l2if = l2if_get(ifindex);
    if(NULL == p_l2if)
    {
        vty_error_out (vty, "Fail to CALLOC if l2 table bucket %s", VTY_NEWLINE);
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to CALLOC if l2 table bucket\n",__FILE__,__LINE__,__FUNCTION__);
        return CMD_WARNING;
    }

    /*check swtich mode*//*edit by jhz,20180313*/
    if(SWITCH_MODE_TRUNK != p_l2if->switch_info.mode)
    {
        vty_error_out (vty, "When there is no egress option,switch mode must be trunk%s", VTY_NEWLINE);
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:switch mode must be trunk\n",__FILE__,__LINE__,__FUNCTION__);
        return CMD_WARNING;
    }

    /*检查dot1q tunnel是否为空*/
    if(DEFAULT_VLAN_ID != p_l2if->switch_info.dot1q_tunnel)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error: dot1q tunnel != 0\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Set switch vlan error:Please delete dot1q_tunnel first.%s",VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*读取参数*/
    v_start = atoi(argv[0]);
    if(NULL == argv[1])
        v_end = v_start;
    else
        v_end = atoi(argv[1]);

    if(v_start > v_end)
    {
        L2_COMMON_LOG_DBG("%s[%d]:%s,v_end(%d) must not less than v_start(%d)\n",__FILE__,__LINE__,__FUNCTION__, v_end, v_start);
        vty_error_out (vty, "Vlan input error,vlanid_end(%d) must not less than vlanid_start(%d) !%s", v_end, v_start,VTY_NEWLINE);
        return CMD_WARNING;
    }

    memset(&switch_vlan, 0, sizeof(struct vlan_range));
    switch_vlan.vlan_start = v_start;
    switch_vlan.vlan_end = v_end;

    /*access vlanid 不能与vlanlist的id相同*/
    if(switch_vlan.vlan_start <= p_l2if->switch_info.access_vlan
            && p_l2if->switch_info.access_vlan <= switch_vlan.vlan_end)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error: hybrid vlanid must not include in vlanlist.\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Set switch vlan error:access vlanid must not include in vlanlist.%s",VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*判断是否创建全局vlan*/
    repeat_v_start=0;
    repeat_v_end =0;
    for(check_vlanid=v_start; check_vlanid<=v_end; ++check_vlanid)
    {
        if(DISABLE==vlan_table[check_vlanid]->vlan.enable)
        {
            flag = 1;
            if(repeat_v_start==0)
            {
                repeat_v_start=check_vlanid;
                repeat_v_end=check_vlanid;
                continue;
            }
            if(check_vlanid==(repeat_v_end+1))
                repeat_v_end=check_vlanid;
            else
            {
                if(repeat_v_start==repeat_v_end)
                    vty_error_out (vty, "Vlanid = %d is not created%s",repeat_v_start, VTY_NEWLINE);
                else
                    vty_error_out (vty, "Vlanid = <%d-%d> are not created%s",repeat_v_start,repeat_v_end, VTY_NEWLINE);
                repeat_v_start=check_vlanid;
                repeat_v_end=check_vlanid;
            }
        }
    }
    if((repeat_v_start==repeat_v_end)&&(repeat_v_start!=0))
        vty_error_out (vty, "Vlanid = %d is not created%s",repeat_v_start, VTY_NEWLINE);
    else if(repeat_v_start!=repeat_v_end)
        vty_error_out (vty, "Vlanid = <%d-%d> are not created%s",repeat_v_start,repeat_v_end, VTY_NEWLINE);
    if(1 == flag)
        return CMD_WARNING;

    //未添加的switch vlan
    memset(vlan_send, 0, sizeof(vlan_send));
    memset(switch_vlan_count, 0, sizeof(switch_vlan_count));
    line_num=0;

    for(vlan_id=v_start; vlan_id<=v_end; ++vlan_id)
    {
        vlan_send[vlan_id]=1;
    }

    if(listhead(p_l2if->switch_info.vlan_list) != NULL)
    {
        for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.vlan_tag_list, p_listnode, pvlan_tagged))
        {
            vlan_id =pvlan_tagged->vlanid;
            vlan_send[vlan_id]=0;
        }
    }

    for(vlan_id=v_start; vlan_id<=v_end; ++vlan_id)
    {
        if(vlan_send[vlan_id]==1)
        {
            switch_vlan_count[line_num][0] = vlan_id;
            switch_vlan_count[line_num][1] = vlan_id;
            iRet++;
            break;
        }
    }
    if(iRet)
    {
        for(vlan_id=v_start; vlan_id<=v_end; ++vlan_id)
        {
            if(vlan_send[vlan_id]==1)
            {
                if(vlan_id == (switch_vlan_count[line_num][1] + 1))
                {
                    switch_vlan_count[line_num][1] = vlan_id;
                }
                else if(vlan_id != switch_vlan_count[line_num][0])
                {
                    line_num++;
                    switch_vlan_count[line_num][0] = vlan_id;
                    switch_vlan_count[line_num][1] = vlan_id;
                }
            }
        }

        /*输出switch vlan*/
        for(i=0; i<=line_num; i++)
        {
            switch_vlan.vlan_start = switch_vlan_count[i][0];
            switch_vlan.vlan_end = switch_vlan_count[i][1];

            ret = l2if_add_vlan(p_l2if, &switch_vlan,TAGGED);
            if(ERRNO_FAIL == ret)
            {
                vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
                return CMD_WARNING;
            }
        }
    }
	mstp_port_vlan_change(p_l2if);

    L2_COMMON_LOG_DBG("%s[%d]:%s:switch vlan %d to %d,ifindex = %#x\n",__FILE__,__LINE__,__FUNCTION__, v_start, v_end,ifindex);

    return CMD_SUCCESS;
}

/*Edit by jhz,20180313,The new command line applies only to the switch hybrid mode, and when Hal configures a vlan, add tagged or untagged tags.*/
DEFUN(switch_vlan_tag_add,
      switch_vlan_tag_add_cmd,
      "switch vlan  <1-4094> {to <1-4094>} egress-tagged (enable | disable)",
      "Mode switch\n"
      "Switch vlan\n"
      "Vlanid start <1-4094>\n"
      "To\n"
      "Vlanid end <1-4094>\n"
      "Whether the egress message carries a vlan tag\n"
      "Enable\n"
      "Disable\n"
     )
{
    uint32_t ifindex = 0;
    uint16_t v_start = 0;
    uint16_t v_end = 0;
    uint16_t repeat_v_start=0,repeat_v_end=0;
    uint16_t check_vlanid = 0;
    uint8_t flag = 0;
    int ret = 0;
    uint8_t mode = 0;
    struct vlan_range switch_vlan;
    struct l2if *p_l2if = NULL;
    struct listnode *p_listnode = NULL;
    struct vlan_tagged *pvlan_tagged = NULL;
    uint32_t switch_vlan_count[4094][2];
    uint16_t line_num = 0;
    int iRet = 0;
    int i = 0;
    uint16_t vlan_id;
    u_char vlan_send[4095];
    uint8_t tag=0;

    ifindex = (uint32_t)vty->index;

    /*检查mode是否为switch*/
    if(ifm_get_mode(ifindex,MODULE_ID_L2, &mode) != 0)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to get mode from ifm\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Fail to get interface mode%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    if((uint8_t)IFNET_MODE_SWITCH != mode)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s,mode != IFNET_MODE_SWITCH\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*在哈希表搜寻ifindex对应节点，如果没有则创建*/
    p_l2if = l2if_get(ifindex);
    if(NULL == p_l2if)
    {
        vty_error_out (vty, "Fail to CALLOC if l2 table bucket %s", VTY_NEWLINE);
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to CALLOC if l2 table bucket\n",__FILE__,__LINE__,__FUNCTION__);
        return CMD_WARNING;
    }

    /*check swtich mode*/
    if(SWITCH_MODE_HYBRID != p_l2if->switch_info.mode)
    {
        vty_error_out (vty, "When there is an egress option,switch mode must be hybrid%s", VTY_NEWLINE);
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:switch mode must be hybrid\n",__FILE__,__LINE__,__FUNCTION__);
        return CMD_WARNING;
    }

    /*检查dot1q tunnel是否为空*/
    if(DEFAULT_VLAN_ID != p_l2if->switch_info.dot1q_tunnel)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error: dot1q tunnel != 0\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Set switch vlan error:Please delete dot1q_tunnel first.%s",VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*读取参数*/
    v_start = atoi(argv[0]);
    if(NULL == argv[1])
        v_end = v_start;
    else
        v_end = atoi(argv[1]);

    if(!strncmp("enable", argv[2], strlen("en")))
        tag =TAGGED;
    else if(!strncmp("disable", argv[2], strlen("dis")))
        tag =UNTAGGED;
    else
    {
        L2_COMMON_LOG_DBG("%s[%d]:%s:invalid input\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Invalid input%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if(v_start > v_end)
    {
        L2_COMMON_LOG_DBG("%s[%d]:%s,v_end(%d) must not less than v_start(%d)\n",__FILE__,__LINE__,__FUNCTION__, v_end, v_start);
        vty_error_out (vty, "Vlan input error,vlanid_end(%d) must not less than vlanid_start(%d) !%s", v_end, v_start,VTY_NEWLINE);
        return CMD_WARNING;
    }

    memset(&switch_vlan, 0, sizeof(struct vlan_range));
    switch_vlan.vlan_start = v_start;
    switch_vlan.vlan_end = v_end;

    /*access vlanid 不能与vlanlist的id相同*/
    if(v_start <= p_l2if->switch_info.access_vlan&& p_l2if->switch_info.access_vlan <= v_end)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error: hybrid vlanid must not include in vlanlist\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Set switch vlan error:access vlanid must not include in vlanlist.%s",VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*判断是否创建全局vlan*/
    repeat_v_start=0;
    repeat_v_end =0;
    for(check_vlanid=v_start; check_vlanid<=v_end; ++check_vlanid)
    {
        if(DISABLE==vlan_table[check_vlanid]->vlan.enable)
        {
            flag = 1;
            if(repeat_v_start==0)
            {
                repeat_v_start=check_vlanid;
                repeat_v_end=check_vlanid;
                continue;
            }
            if(check_vlanid==(repeat_v_end+1))
                repeat_v_end=check_vlanid;
            else
            {
                if(repeat_v_start==repeat_v_end)
                    vty_error_out (vty, "Vlanid = %d is not created%s",repeat_v_start, VTY_NEWLINE);
                else
                    vty_error_out (vty, "Vlanid = <%d-%d> are not created%s",repeat_v_start,repeat_v_end, VTY_NEWLINE);
                repeat_v_start=check_vlanid;
                repeat_v_end=check_vlanid;
            }
        }
    }
    if((repeat_v_start==repeat_v_end)&&(repeat_v_start!=0))
        vty_error_out (vty, "Vlanid = %d is not created%s",repeat_v_start, VTY_NEWLINE);
    else if(repeat_v_start!=repeat_v_end)
        vty_error_out (vty, "Vlanid = <%d-%d> are not created%s",repeat_v_start,repeat_v_end, VTY_NEWLINE);
    if(1 == flag)
        return CMD_WARNING;

    /*遍历vlan_tag_list,找出未添加的switch vlan和已添加但tag不同的switch vlan*/
    //未添加的switch vlan
    memset(vlan_send, 0, sizeof(vlan_send));
    memset(switch_vlan_count, 0, sizeof(switch_vlan_count));
    line_num=0;

    for(vlan_id=v_start; vlan_id<=v_end; ++vlan_id)
    {
        vlan_send[vlan_id]=1;
    }

    if(listhead(p_l2if->switch_info.vlan_list) != NULL)
    {
        for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.vlan_tag_list, p_listnode, pvlan_tagged))
        {
            vlan_id =pvlan_tagged->vlanid;
            vlan_send[vlan_id]=0;
        }
    }

    for(vlan_id=v_start; vlan_id<=v_end; ++vlan_id)
    {
        if(vlan_send[vlan_id]==1)
        {
            switch_vlan_count[line_num][0] = vlan_id;
            switch_vlan_count[line_num][1] = vlan_id;
            iRet++;
            break;
        }
    }
    if(iRet)
    {
        for(vlan_id=v_start; vlan_id<=v_end; ++vlan_id)
        {
            if(vlan_send[vlan_id]==1)
            {
                if(vlan_id == (switch_vlan_count[line_num][1] + 1))
                {
                    switch_vlan_count[line_num][1] = vlan_id;
                }
                else if(vlan_id != switch_vlan_count[line_num][0])
                {
                    line_num++;
                    switch_vlan_count[line_num][0] = vlan_id;
                    switch_vlan_count[line_num][1] = vlan_id;
                }
            }
        }

        /*输出switch vlan*/
        for(i=0; i<=line_num; i++)
        {
            switch_vlan.vlan_start = switch_vlan_count[i][0];
            switch_vlan.vlan_end = switch_vlan_count[i][1];

            ret = l2if_add_vlan(p_l2if, &switch_vlan,tag);
            if(ERRNO_FAIL == ret)
            {
                vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
                return CMD_WARNING;
            }
        }
    }

    /*已添加但tag不同的switch vlan*/
    memset(switch_vlan_count, 0, sizeof(switch_vlan_count));
    line_num=0;
    iRet=0;
    for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.vlan_tag_list, p_listnode, pvlan_tagged))
    {
        if((pvlan_tagged->vlanid >=v_start)&&(pvlan_tagged->vlanid <=v_end))
        {
            if(pvlan_tagged->tag!=tag)
            {
                switch_vlan_count[line_num][0] = pvlan_tagged->vlanid;
                switch_vlan_count[line_num][1] = pvlan_tagged->vlanid;
                iRet++;
                break;
            }
        }
    }
    if(iRet)
    {
        for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.vlan_tag_list, p_listnode, pvlan_tagged))
        {
            if((pvlan_tagged->vlanid >=v_start)&&(pvlan_tagged->vlanid <=v_end))
            {
                if(pvlan_tagged->tag!=tag)
                {
                    if(pvlan_tagged->vlanid == (switch_vlan_count[line_num][1] + 1))
                    {
                        switch_vlan_count[line_num][1] = pvlan_tagged->vlanid;
                    }
                    else if(pvlan_tagged->vlanid != switch_vlan_count[line_num][0])
                    {
                        line_num++;
                        switch_vlan_count[line_num][0] = pvlan_tagged->vlanid;
                        switch_vlan_count[line_num][1] = pvlan_tagged->vlanid;
                    }
                }
            }
        }

        ret=l2if_vlan_send_hal_up_tagged(switch_vlan_count,line_num,p_l2if,tag);
        if(ERRNO_NOT_FOUND == ret)
        {
            vty_error_out (vty, "Vlan update %s fail%s",argv[2],VTY_NEWLINE);
            return CMD_WARNING;
        }
        else if(ERRNO_FAIL == ret)
        {
            vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
	mstp_port_vlan_change(p_l2if);

    L2_COMMON_LOG_DBG("%s[%d]:%s:switch vlan %d to %d,ifindex = %#x\n",__FILE__,__LINE__,__FUNCTION__, v_start, v_end,ifindex);

    return CMD_SUCCESS;
}


DEFUN(l2if_mac_limit,
      l2if_mac_limit_cmd,
      "mac limit <1-16384> action (discard | forward)",
      "Mac\n"
      "Limit the count of mac learn\n"
      "Limit value <1-16384> and the default value is 0.\n"
      "Action when limit value is exceeded\n"
      "Discard the packet\n"
      "Forward the packet\n")
{
    uint32_t limit_num = 0;
    uint32_t ifindex=0;
    uint8_t action=0;
    uint8_t mode = 0;
    struct l2if *p_l2if = NULL;
    struct ifm_switch ifm_switch_t;
    int ret=0;

    limit_num= (s_int32_t)atoi(argv[0]);
    if ((limit_num < 1) || (limit_num > 16384))
    {
        vty_error_out(vty, "Please check out mac limit <0-16384>%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if(!strncmp("forward", argv[1], strlen("for")))
        action = ENABLE;
    else if(!strncmp("discard", argv[1], strlen("dis")))
        action = DISABLE;

    ifindex = (uint32_t)vty->index;

    /*检查mode是否为switch*/
    if(ifm_get_mode(ifindex,MODULE_ID_L2, &mode) != 0)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to get mode from ifm\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Fail to get interface mode%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    if((uint8_t)IFNET_MODE_SWITCH != mode)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s,mode != IFNET_MODE_SWITCH\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*在哈希表搜寻ifindex对应节点，如果没有则创建*/
    p_l2if = l2if_get(ifindex);
    if(NULL == p_l2if)
    {
        vty_error_out (vty, "Fail to CALLOC if l2 table bucket %s", VTY_NEWLINE);
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to CALLOC if l2 table bucket\n",__FILE__,__LINE__,__FUNCTION__);
        return CMD_WARNING;
    }

    if((limit_num==p_l2if->switch_info.limit_num)&&(action==p_l2if->switch_info.limit_action))
    {
        return CMD_SUCCESS;
    }

    memset(&ifm_switch_t,0,sizeof(struct ifm_switch));
    ifm_switch_t.limit_action=action;
    ifm_switch_t.limit_num=limit_num;


    L2_COMMON_LOG_DBG("before ipc_send_hal limit:%d-action:%d\n",ifm_switch_t.limit_num,ifm_switch_t.limit_action);

    ret=l2_msg_send_hal_wait_ack(&ifm_switch_t,sizeof(struct ifm_switch), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_MAC_LIMIT, IPC_OPCODE_UPDATE, ifindex);
    if(ret)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to send mac limit msg to hal\n",__FILE__,__LINE__,__func__);
        vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
//	ipc_send_hal(&ifm_switch_t,sizeof(struct ifm_switch), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_MAC_LIMIT, IPC_OPCODE_UPDATE, ifindex);
    p_l2if->switch_info.limit_num=limit_num;
    p_l2if->switch_info.limit_action=action;

    L2_COMMON_LOG_DBG("%s[%d]:%s:ifindex: %x,mac limit=%d, action %s success!\n",__FILE__,__LINE__,__FUNCTION__,ifindex,limit_num,mac_limit_action[action]);
    return CMD_SUCCESS;

}

DEFUN(no_l2if_mac_limit,
      no_l2if_mac_limit_cmd,
      "no mac limit",
      "Clear config\n"
      "Mac\n"
      "Limit the count of mac learn\n")
{
    uint32_t limit_num = 0;
    uint32_t ifindex=0;
    uint8_t mode = 0;
    struct l2if *p_l2if = NULL;
    struct ifm_switch ifm_switch_t;
    int ret=0;

    ifindex = (uint32_t)vty->index;

    /*检查mode是否为switch*/
    if(ifm_get_mode(ifindex,MODULE_ID_L2, &mode) != 0)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to get mode from ifm\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Fail to get interface mode%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    if((uint8_t)IFNET_MODE_SWITCH != mode)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s,mode != IFNET_MODE_SWITCH\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*在哈希表搜寻ifindex对应节点，如果没有则创建*/
    p_l2if = l2if_get(ifindex);
    if(NULL == p_l2if)
    {
        vty_error_out (vty, "Fail to CALLOC if l2 table bucket %s", VTY_NEWLINE);
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to CALLOC if l2 table bucket\n",__FILE__,__LINE__,__FUNCTION__);
        return CMD_WARNING;
    }

    if(limit_num==p_l2if->switch_info.limit_num)
    {
        return CMD_SUCCESS;
    }

    memset(&ifm_switch_t,0,sizeof(struct ifm_switch));
    ifm_switch_t.limit_num=limit_num;
    ifm_switch_t.limit_action = ENABLE;     //default enable forwarding

    L2_COMMON_LOG_DBG("before ipc_send_hal limit:%d, action=%d\n",ifm_switch_t.limit_num, ifm_switch_t.limit_action);
#if 1
    ret=l2_msg_send_hal_wait_ack(&ifm_switch_t,sizeof(struct ifm_switch), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_MAC_LIMIT, IPC_OPCODE_UPDATE, ifindex);
    if(ret)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to send mac limit msg to hal\n",__FILE__,__LINE__,__func__);
        vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
#endif
//	ipc_send_hal(&ifm_switch_t,sizeof(struct ifm_switch), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_MAC_LIMIT, IPC_OPCODE_UPDATE, ifindex);
    p_l2if->switch_info.limit_num=limit_num;

    L2_COMMON_LOG_DBG("%s[%d]:%s:ifindex: %x,mac limit=%d success!\n",__FILE__,__LINE__,__FUNCTION__,ifindex,limit_num);
    return CMD_SUCCESS;

}

DEFUN(no_l2if_storm_supress,
      no_l2if_storm_supress_cmd,
      "no storm-suppress {unicast | broadcast | multicast}",
      "Clear config\n"
      "Broadcast storm\n"
      "Storm-control\n"
      "Unicast\n"
      "Broadcast\n"
      "Multicast\n")
{
    uint32_t ifindex = 0;
    struct l2if *p_l2if = NULL;
    uint8_t mode = 0;
    struct ifm_switch ifm_switch_t;
    uint8_t unicast_num;
    uint8_t broadcast_num;
    uint8_t multicast_num;
    uint32_t unicast_rate;
    uint32_t broadcast_rate;
    uint32_t multicast_rate;
    uint32_t rate=0;
    int ret=0;

    ifindex = (uint32_t)vty->index;

    /*检查mode是否为switch*/
    if(ifm_get_mode(ifindex,MODULE_ID_L2, &mode) != 0)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to get mode from ifm\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Fail to get interface mode%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    if((uint8_t)IFNET_MODE_SWITCH != mode)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s,mode != IFNET_MODE_SWITCH\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*在哈希表搜寻ifindex对应节点，如果没有则创建*/
    p_l2if = l2if_get(ifindex);
    if(NULL == p_l2if)
    {
        vty_error_out (vty, "Fail to CALLOC if l2 table bucket %s", VTY_NEWLINE);
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to CALLOC if l2 table bucket\n",__FILE__,__LINE__,__FUNCTION__);
        return CMD_WARNING;
    }

    unicast_num=p_l2if->switch_info.storm_control.storm_unicast;
    broadcast_num=p_l2if->switch_info.storm_control.storm_broadcast;
    multicast_num=p_l2if->switch_info.storm_control.storm_multicast;

    unicast_rate=p_l2if->switch_info.storm_control.unicast_rate;
    broadcast_rate=p_l2if->switch_info.storm_control.broadcast_rate;
    multicast_rate=p_l2if->switch_info.storm_control.multicast_rate;

    if (argv[0] != NULL && argv[1] != NULL && argv[2] != NULL )
    {
        unicast_num=DISABLE;
        broadcast_num=DISABLE;
        multicast_num=DISABLE;
        unicast_rate=rate;
        broadcast_rate=rate;
        multicast_rate=rate;
    }
    else if (argv[0] != NULL && argv[1] != NULL && argv[2] == NULL )
    {
        unicast_num=DISABLE;
        broadcast_num=DISABLE;
        unicast_rate=rate;
        broadcast_rate=rate;
    }
    else if (argv[0] != NULL && argv[1] == NULL && argv[2] != NULL )
    {
        unicast_num=DISABLE;
        multicast_num=DISABLE;
        unicast_rate=rate;
        multicast_rate=rate;
    }
    else if (argv[0] != NULL && argv[1] == NULL && argv[2] == NULL )
    {
        unicast_num=DISABLE;
        unicast_rate=rate;
    }
    else if (argv[0] == NULL && argv[1] != NULL && argv[2] != NULL )
    {
        broadcast_num=DISABLE;
        multicast_num=DISABLE;
        broadcast_rate=rate;
        multicast_rate=rate;
    }
    else if (argv[0] == NULL && argv[1] != NULL && argv[2] == NULL )
    {
        broadcast_num=DISABLE;
        broadcast_rate=rate;
    }
    else if (argv[0] == NULL && argv[1] == NULL && argv[2] != NULL )
    {
        multicast_num=DISABLE;
        multicast_rate=rate;
    }
    else if (argv[0] == NULL && argv[1] == NULL && argv[2] == NULL )
    {
        unicast_num= DISABLE;
        broadcast_num= DISABLE;
        multicast_num=DISABLE;
        unicast_rate=rate;
        broadcast_rate=rate;
        multicast_rate=rate;
    }

    if((unicast_num==p_l2if->switch_info.storm_control.storm_unicast)&&
            (broadcast_num==p_l2if->switch_info.storm_control.storm_broadcast)&&
            (multicast_num==p_l2if->switch_info.storm_control.storm_multicast))
    {
        return CMD_SUCCESS;
    }

    L2_COMMON_LOG_DBG("[%d]:%s:set unicast=%s,broadcast=%s,multicast=%s,unicast_rate=%d,broadcast_rate=%d,multicast_rate=%d\n",__LINE__,__FUNCTION__,
               l2if_a_storm_suppress[unicast_num]	,l2if_a_storm_suppress[broadcast_num],l2if_a_storm_suppress[multicast_num],
               unicast_rate,broadcast_rate,multicast_rate);


    memset(&ifm_switch_t,0,sizeof(struct ifm_switch));
    ifm_switch_t.storm_control.storm_unicast=unicast_num;
    ifm_switch_t.storm_control.storm_broadcast=broadcast_num;
    ifm_switch_t.storm_control.storm_multicast=multicast_num;

    /*Hal requires all 0 */
    ifm_switch_t.storm_control.unicast_rate=0;
    ifm_switch_t.storm_control.broadcast_rate=0;
    ifm_switch_t.storm_control.multicast_rate=0;

    ret=l2_msg_send_hal_wait_ack(&ifm_switch_t,sizeof(struct ifm_switch), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_STORM_SUPRESS, IPC_OPCODE_UPDATE, ifindex);
    if(ret)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to send storm suppress msg to hal\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    p_l2if->switch_info.storm_control.storm_unicast=unicast_num;
    p_l2if->switch_info.storm_control.storm_broadcast=broadcast_num;
    p_l2if->switch_info.storm_control.storm_multicast=multicast_num;

    p_l2if->switch_info.storm_control.unicast_rate=unicast_rate;
    p_l2if->switch_info.storm_control.broadcast_rate=broadcast_rate;
    p_l2if->switch_info.storm_control.multicast_rate=multicast_rate;

    L2_COMMON_LOG_DBG("%s[%d]:%s:ifindex: %x;storm suppress success\n",__FILE__,__LINE__,__FUNCTION__,ifindex);
    return CMD_SUCCESS;
}

DEFUN(l2if_storm_supress,
      l2if_storm_supress_cmd,
      "storm-suppress {unicast | broadcast | multicast}  rate <64-50000>",
      "Broadcast storm\n"
      "Storm-control\n"
      "Unicast\n"
      "Broadcast\n"
      "Multicast\n"
      "Rate values in kilobits (1000 bits) per second\n"
      "Rate values format\n")
{
    uint32_t ifindex = 0;
    struct l2if *p_l2if = NULL;
    uint8_t mode = 0;
    struct ifm_switch ifm_switch_t;
    uint8_t unicast_num;
    uint8_t broadcast_num;
    uint8_t multicast_num;
    uint32_t unicast_rate;
    uint32_t broadcast_rate;
    uint32_t multicast_rate;
    uint32_t rate;
    int ret=0;

    ifindex = (uint32_t)vty->index;

    /*检查mode是否为switch*/
    if(ifm_get_mode(ifindex,MODULE_ID_L2, &mode) != 0)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to get mode from ifm\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Fail to get interface mode%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    if((uint8_t)IFNET_MODE_SWITCH != mode)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s,mode != IFNET_MODE_SWITCH\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*在哈希表搜寻ifindex对应节点，如果没有则创建*/
    p_l2if = l2if_get(ifindex);
    if(NULL == p_l2if)
    {
        vty_error_out (vty, "Fail to CALLOC if l2 table bucket %s", VTY_NEWLINE);
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to CALLOC if l2 table bucket\n",__FILE__,__LINE__,__FUNCTION__);
        return CMD_WARNING;
    }

    unicast_num=p_l2if->switch_info.storm_control.storm_unicast;
    broadcast_num=p_l2if->switch_info.storm_control.storm_broadcast;
    multicast_num=p_l2if->switch_info.storm_control.storm_multicast;

    unicast_rate=p_l2if->switch_info.storm_control.unicast_rate;
    broadcast_rate=p_l2if->switch_info.storm_control.broadcast_rate;
    multicast_rate=p_l2if->switch_info.storm_control.multicast_rate;

    rate=atoi(argv[3]);

    if (argv[0] != NULL && argv[1] != NULL && argv[2] != NULL )
    {
        unicast_num=ENABLE;
        broadcast_num=ENABLE;
        multicast_num=ENABLE;
        unicast_rate=rate;
        broadcast_rate=rate;
        multicast_rate=rate;
    }
    else if (argv[0] != NULL && argv[1] != NULL && argv[2] == NULL )
    {
        unicast_num=ENABLE;
        broadcast_num=ENABLE;
        unicast_rate=rate;
        broadcast_rate=rate;
    }
    else if (argv[0] != NULL && argv[1] == NULL && argv[2] != NULL )
    {
        unicast_num=ENABLE;
        multicast_num=ENABLE;
        unicast_rate=rate;
        multicast_rate=rate;
    }
    else if (argv[0] != NULL && argv[1] == NULL && argv[2] == NULL )
    {
        unicast_num=ENABLE;
        unicast_rate=rate;
    }
    else if (argv[0] == NULL && argv[1] != NULL && argv[2] != NULL )
    {
        broadcast_num=ENABLE;
        multicast_num=ENABLE;
        broadcast_rate=rate;
        multicast_rate=rate;
    }
    else if (argv[0] == NULL && argv[1] != NULL && argv[2] == NULL )
    {
        broadcast_num=ENABLE;
        broadcast_rate=rate;
    }
    else if (argv[0] == NULL && argv[1] == NULL && argv[2] != NULL )
    {
        multicast_num=ENABLE;
        multicast_rate=rate;
    }
    else if (argv[0] == NULL && argv[1] == NULL && argv[2] == NULL )
    {
        unicast_num=ENABLE;
        broadcast_num=ENABLE;
        multicast_num=ENABLE;
        unicast_rate=rate;
        broadcast_rate=rate;
        multicast_rate=rate;
    }

    if((unicast_num==p_l2if->switch_info.storm_control.storm_unicast)&&
            (broadcast_num==p_l2if->switch_info.storm_control.storm_broadcast)&&
            (multicast_num==p_l2if->switch_info.storm_control.storm_multicast)&&
            (unicast_rate==p_l2if->switch_info.storm_control.unicast_rate)&&
            (broadcast_rate==p_l2if->switch_info.storm_control.broadcast_rate)&&
            (multicast_rate==p_l2if->switch_info.storm_control.multicast_rate))
    {
        return CMD_SUCCESS;
    }

    L2_COMMON_LOG_DBG("[%d]:%s:set unicast=%s,broadcast=%s,multicast=%s,unicast_rate=%d,broadcast_rate=%d,multicast_rate=%d\n",__LINE__,__FUNCTION__,
               l2if_a_storm_suppress[unicast_num]	,l2if_a_storm_suppress[broadcast_num],l2if_a_storm_suppress[multicast_num],
               unicast_rate,broadcast_rate,multicast_rate);

    memset(&ifm_switch_t,0,sizeof(struct ifm_switch));
    ifm_switch_t.storm_control.storm_unicast=unicast_num;
    ifm_switch_t.storm_control.storm_broadcast=broadcast_num;
    ifm_switch_t.storm_control.storm_multicast=multicast_num;

    ifm_switch_t.storm_control.unicast_rate=unicast_rate;
    ifm_switch_t.storm_control.broadcast_rate=broadcast_rate;
    ifm_switch_t.storm_control.multicast_rate=multicast_rate;

    ret=l2_msg_send_hal_wait_ack(&ifm_switch_t,sizeof(struct ifm_switch), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_STORM_SUPRESS, IPC_OPCODE_UPDATE, ifindex);
    if(ret)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to send storm suppress msg to hal\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    p_l2if->switch_info.storm_control.storm_unicast=unicast_num;
    p_l2if->switch_info.storm_control.storm_broadcast=broadcast_num;
    p_l2if->switch_info.storm_control.storm_multicast=multicast_num;

    p_l2if->switch_info.storm_control.unicast_rate=unicast_rate;
    p_l2if->switch_info.storm_control.broadcast_rate=broadcast_rate;
    p_l2if->switch_info.storm_control.multicast_rate=multicast_rate;

    L2_COMMON_LOG_DBG("%s[%d]:%s:ifindex: %x;storm suppress success\n",__FILE__,__LINE__,__FUNCTION__,ifindex);
    return CMD_SUCCESS;
}

DEFUN(reserve_vlan_set,
      reserve_vlan_set_cmd,
      "reserve-vlan <1-4094>",
      "Reserve vlan:some vlan in used by sys\n"
      "Reserve vlan value <1-4094>\n")
{
    int ret = 0;
    int reserve_vlanid = 0;
    uint32_t ifindex = 0;
    uint8_t port = 0;
    uint8_t mode = 0;
    struct l2if *p_l2if = NULL;
	int i = 0;

    /*get ifindex from interface we use*/
    ifindex = (uint32_t)vty->index;

    /*检查mode是否是switch*/
    if(ifm_get_mode(ifindex,MODULE_ID_L2, &mode) != 0)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to get mode from ifm\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Fail to get interface mode%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    if((uint8_t)IFNET_MODE_SWITCH != mode)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s,mode != IFNET_MODE_L3\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Mode error,not L3if %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    p_l2if = l2if_get(ifindex);
    if(NULL == p_l2if)
    {
        vty_error_out (vty, "Fail to CALLOC if l2 table bucket %s", VTY_NEWLINE);
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to CALLOC if l2 table bucket\n",__FILE__,__LINE__,__FUNCTION__);
        return CMD_WARNING;
    }

    reserve_vlanid = (int)atoi(argv[0]);

	if(ENABLE==vlan_table[reserve_vlanid]->vlan.enable)
    {
        vty_error_out (vty, "Fail to set reserve vlan,vlan %d is useing, please no vlan %d first! %s", reserve_vlanid, reserve_vlanid,VTY_NEWLINE);
        return CMD_WARNING;
    }

	if(IFM_TYPE_IS_TRUNK(ifindex) && !(IFM_TYPE_IS_SUBPORT(ifindex)))
	{
		for(i = 0; i < 3; i++)
		{
			if( trunk_reserve_vlan_table[i].trunkid == IFM_TRUNK_ID_GET(ifindex) )
			{
				if(trunk_reserve_vlan_table[i].reserve_vlan == reserve_vlanid)
					return CMD_SUCCESS;
				break;
			}
		}
		
	}
	else
	{
		port = IFM_PORT_ID_GET ( ifindex );
	    if( port < 1 || port > 6 )
	    {
	        vty_error_out (vty, "Invalid port number! %s", VTY_NEWLINE);
	        return CMD_WARNING;
	    }
	    if( reserve_vlan_table[port-1] == reserve_vlanid )
	        return CMD_SUCCESS;
	}   

    ret=ipc_send_msg_n2(&reserve_vlanid,sizeof(struct ifm_switch), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_RESERVE_VLAN, IPC_OPCODE_UPDATE, ifindex);
    if(ret)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to send storm suppress msg to hal\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	if(IFM_TYPE_IS_TRUNK(ifindex) && !(IFM_TYPE_IS_SUBPORT(ifindex)))
	{
		trunk_reserve_vlan_table[i].reserve_vlan = reserve_vlanid;
	}
	else
	{
		reserve_vlan_table[port-1] = reserve_vlanid;
	} 
    

    L2_COMMON_LOG_DBG("%s[%d]:%s:reserve vlan set success\n",__FILE__,__LINE__,__FUNCTION__);
    return CMD_SUCCESS;
}

DEFUN(no_reserve_vlan_set,
      no_reserve_vlan_set_cmd,
      "no reserve-vlan",
      NO_STR
      "Hold vlan:some vlan in used by sys\n"
      "Hold vlan value <1-4094>\n")
{
    int ret = 0;
	int i = 0;
    int reserve_vlanid = 0;
    uint32_t ifindex = 0;
    uint8_t port = 0;
    uint8_t mode = 0;
    struct l2if *p_l2if = NULL;

    /*get ifindex from interface we use*/
    ifindex = (uint32_t)vty->index;

    /*检查mode是否是switch*/
    if(ifm_get_mode(ifindex,MODULE_ID_L2, &mode) != 0)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to get mode from ifm\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Fail to get interface mode%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
	if((uint8_t)IFNET_MODE_L3 != mode)
	{
		L2_COMMON_LOG_DBG("%s[%d]:leave %s,mode != IFNET_MODE_L3\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Mode error,not L3if %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

    p_l2if = l2if_get(ifindex);
    if(NULL == p_l2if)
    {
        vty_error_out (vty, "Fail to CALLOC if l2 table bucket %s", VTY_NEWLINE);
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to CALLOC if l2 table bucket\n",__FILE__,__LINE__,__FUNCTION__);
        return CMD_WARNING;
    }

	if(IFM_TYPE_IS_TRUNK(ifindex) && !(IFM_TYPE_IS_SUBPORT(ifindex)))
	{
		for(i = 0; i < 3; i++)
		{
			if( trunk_reserve_vlan_table[i].trunkid == IFM_TRUNK_ID_GET(ifindex) )
			{
				reserve_vlanid = trunk_reserve_vlan_table[i].reserve_vlan_bak;
				break;
			}
		}
		
	}
	else
	{
	    port = IFM_PORT_ID_GET ( ifindex );
	    if( port < 1 || port > 6 )
	    {
	        vty_error_out (vty, "Invalid port number! %s", VTY_NEWLINE);
	        return CMD_WARNING;
	    }

	    switch(port)
	    {
	    case 1:
	        reserve_vlanid = DEF_RESV_VLAN1;
	        break;
	    case 2:
	        reserve_vlanid = DEF_RESV_VLAN2;
	        break;
	    case 3:
	        reserve_vlanid = DEF_RESV_VLAN3;
	        break;
	    case 4:
	        reserve_vlanid = DEF_RESV_VLAN4;
	        break;
	    case 5:
	        reserve_vlanid = DEF_RESV_VLAN5;
	        break;
	    case 6:
	        reserve_vlanid = DEF_RESV_VLAN6;
	        break;
	    default:
	        break;

	    	}
		if( reserve_vlan_table[port-1] == reserve_vlanid )
	        return CMD_SUCCESS;
	}
    

    ret=ipc_send_msg_n2(&reserve_vlanid,sizeof(struct ifm_switch), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_RESERVE_VLAN, IPC_OPCODE_UPDATE, ifindex);
    if(ret)
    {
        L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to send storm suppress msg to hal\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
	
    if(IFM_TYPE_IS_TRUNK(ifindex) && !(IFM_TYPE_IS_SUBPORT(ifindex)))
	{
		trunk_reserve_vlan_table[i].reserve_vlan = reserve_vlanid;
	}
	else
	{
		reserve_vlan_table[port-1] = reserve_vlanid;
	} 

    L2_COMMON_LOG_DBG("%s[%d]:%s:reserve vlan set success\n",__FILE__,__LINE__,__FUNCTION__);
    return CMD_SUCCESS;
}


DEFUN(reserve_vlan_show,
      reserve_vlan_show_cmd,
      "show interface reserve-vlan",
      SHOW_STR
      CLI_INTERFACE_STR
      "Reserve vlan:some vlan in used by sys\n")
{
    int i = 0;

    vty_out(vty, " interface name 				   R-vlan-id%s", VTY_NEWLINE);
    for( ; i < 6; i++ )
    {
        vty_out(vty, "gigabitethernet 1/1/%d			      %d%s",i+1,reserve_vlan_table[i], VTY_NEWLINE);
    }

    for( i = 0; i < 3; i++ )
    {
    	if(trunk_reserve_vlan_table[i].flag == 0)
			continue;
        vty_out(vty, "trunk %d\t			      %d%s",trunk_reserve_vlan_table[i].trunkid,trunk_reserve_vlan_table[i].reserve_vlan, VTY_NEWLINE);
    }
    return CMD_SUCCESS;
}
extern void zlog_debug_set(struct vty *vty, unsigned int type, int enable);

DEFUN(l2_debug_fun,
	  l2_debug_cmd,
	  "debug l2 (enable|disable) (elps|erps|lacp|lldp|mstp|trunk|mac|qinq|vlan|efm|common|all)",
	  "Debug information to moniter\n"
	  "config l2 debug information \n"
	  "l2 debug enable\n"
	  "l2 debug disable\n"
	  "l2 debug type elps\n"
	  "l2 debug type erps\n"
	  "l2 debug type lacp\n"
	  "l2 debug type lldp\n"
	  "l2 debug type mstp\n"
	  "l2 debug type trunk\n"
	  "l2 debug type mac static\n"
	  "l2 debug type qinq\n"
	  "l2 debug type vlan\n"
	  "l2 debug type efm\n"
	  "l2 debug type common\n"
	  "l2 debug type all\n")
{
  unsigned int  zlog_num = 0;
  
  for( ; zlog_num < array_size(l2_dbg_name); zlog_num++)
  {
	  if(!strncmp(argv[1], l2_dbg_name[zlog_num].str, 3))
	  {
		  zlog_debug_set( vty, l2_dbg_name[zlog_num].key, !strncmp(argv[0], "enable", 3));

		  return CMD_SUCCESS;
	  }
  }

  vty_out (vty, "No debug typd find %s", VTY_NEWLINE);

  return CMD_SUCCESS;
}

DEFUN (show_l2_debug_fun,
	   show_l2_debug_cmd,
	   "show l2 debug",
	   SHOW_STR
	   "l2"
	   "Debug status\n")
{
	  unsigned  int type_num = 0;
	   int offset = 0;
	   int debugInfor = 0;

	   debugInfor = l2_dbg_name[0].key;

	   if(debugInfor & 0x01)
	   {
	   	  offset = 0;	
	   }
	   else
	   {
	   	   while(debugInfor)
		   {
		   		offset++;
				debugInfor >>= 1;
		   }
		   offset -= 1;
	   }
	   
	   vty_out(vty, "debug type      status %s", VTY_NEWLINE);
	   vty_out(vty, "-------------------------%s", VTY_NEWLINE);
	   
	   for(type_num = 0; type_num < array_size(l2_dbg_name); ++type_num)
	   {
			 vty_out(vty, "%-15s	%-10s %s", l2_dbg_name[type_num].str, 
				  !!(vty->monitor & (1 << (type_num+offset))) ? "enable" : "disable", VTY_NEWLINE);
	   }

	   return CMD_SUCCESS;
}

DEFUN (show_l2_timerinfo_fun,
       	 show_l2_timerinfo_cmd,
       	  "show l2 timer",
         SHOW_STR
       	  "L2  module information\n"
       	  "L2  module timer\n")
{
    int   iInLn = 1000;
    char  bInfo[1000] = {0};
    
    iInLn = high_pre_timer_info(bInfo, iInLn);

    if(iInLn > 0)
    {
    	vty_out(vty, "%s%s", bInfo, VTY_NEWLINE);
    }
    else
	{
		vty_error_out(vty, "Show l2 timer infor error. %s", VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}


/* 接口下的 qinq 配置 */
void l2if_qinq_config_write(struct vty *vty, struct list *list)
{
    struct vlan_mapping *qinq_data = NULL;
    struct listnode *node = NULL;
    uint8_t ic, is, inc, ins;
    enum VLAN_ACTION cvlan_act;  /* cvlan 的转换动作*/
    enum VLAN_ACTION svlan_act;  /* svlan 的转换动作*/

    for(ALL_LIST_ELEMENTS_RO(list,node,qinq_data))
    {
        ic = qinq_data->cvlan.vlan_start? 1:0;
        is = qinq_data->svlan.vlan_start? 1:0;
        inc = qinq_data->cvlan_new.vlan_start? 1:0;
        ins = qinq_data->svlan_new.vlan_start? 1:0;
        cvlan_act = qinq_data->cvlan_act;
        svlan_act = qinq_data->svlan_act;

        if((cvlan_act == VLAN_ACTION_NO) && (svlan_act == VLAN_ACTION_TRANSLATE))
        {
            if(0 == ic && 0 != is && 0 == inc && 0 != ins)
            {
                if(qinq_data->svlan.vlan_start==qinq_data->svlan.vlan_end)
                {
                    vty_out(vty," vlan-mapping dot1q %d translate %d%s", qinq_data->svlan.vlan_start,\
                            qinq_data->svlan_new.vlan_start,VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty," vlan-mapping dot1q %d to %d translate %d%s", qinq_data->svlan.vlan_start,\
                            qinq_data->svlan.vlan_end,qinq_data->svlan_new.vlan_start,VTY_NEWLINE);
                }
            }
            else if(0 != ic && 0 != is && 0 != inc && 0 != ins)
            {
                if(qinq_data->svlan_cos==8)
                {
                    vty_out(vty," vlan-mapping qinq svlan %d cvlan %d to %d translate svlan %d cvlan *%s", qinq_data->svlan.vlan_start, \
                            qinq_data->cvlan.vlan_start, qinq_data->cvlan.vlan_end, qinq_data->svlan_new.vlan_start,VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty," vlan-mapping qinq svlan %d cvlan %d to %d translate svlan %d cos %d cvlan *%s",qinq_data->svlan.vlan_start, \
                            qinq_data->cvlan.vlan_start, qinq_data->cvlan.vlan_end, qinq_data->svlan_new.vlan_start,qinq_data->svlan_cos,VTY_NEWLINE);
                }
            }
        }
        if((cvlan_act == VLAN_ACTION_NO) && (svlan_act == VLAN_ACTION_ADD))
        {
            if(qinq_data->svlan_cos==8)
            {
                if(qinq_data->svlan.vlan_start==qinq_data->svlan.vlan_end)
                {
                    vty_out(vty," vlan-mapping dot1q %d add svlan %d %s", qinq_data->svlan.vlan_start, \
                            qinq_data->svlan_new.vlan_start,VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty," vlan-mapping dot1q %d to %d add svlan %d %s", qinq_data->svlan.vlan_start, \
                            qinq_data->svlan.vlan_end,qinq_data->svlan_new.vlan_start,VTY_NEWLINE);
                }
            }
            else
            {
                if(qinq_data->svlan.vlan_start==qinq_data->svlan.vlan_end)
                {
                    vty_out(vty," vlan-mapping dot1q %d add svlan %d cos %d%s", qinq_data->svlan.vlan_start,\
                            qinq_data->svlan_new.vlan_start, qinq_data->svlan_cos,VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty," vlan-mapping dot1q %d to %d add svlan %d cos %d%s", qinq_data->svlan.vlan_start,\
                            qinq_data->svlan.vlan_end,qinq_data->svlan_new.vlan_start, qinq_data->svlan_cos,VTY_NEWLINE);
                }
            }
        }
        if((cvlan_act == VLAN_ACTION_ADD) && (svlan_act == VLAN_ACTION_TRANSLATE))
        {
            if(qinq_data->svlan_cos==8)
            {
                vty_out(vty," vlan-mapping dot1q %d translate svlan %d cvlan %d%s", qinq_data->svlan.vlan_start,\
                        qinq_data->svlan_new.vlan_start, qinq_data->cvlan_new.vlan_start,VTY_NEWLINE);
            }
            else
            {
                vty_out(vty," vlan-mapping dot1q %d translate svlan %d cos %d cvlan %d%s", qinq_data->svlan.vlan_start, \
                        qinq_data->svlan_new.vlan_start, qinq_data->svlan_cos, qinq_data->cvlan_new.vlan_start,VTY_NEWLINE);
            }
        }
        if((cvlan_act == VLAN_ACTION_TRANSLATE) && (svlan_act == VLAN_ACTION_TRANSLATE))
        {
            if(qinq_data->svlan_cos==8)
            {
                vty_out(vty," vlan-mapping qinq svlan %d cvlan %d translate svlan %d cvlan %d%s", qinq_data->svlan.vlan_start, \
                        qinq_data->cvlan.vlan_start, qinq_data->svlan_new.vlan_start, qinq_data->cvlan_new.vlan_start,VTY_NEWLINE);
            }
            else
            {
                vty_out(vty," vlan-mapping qinq svlan %d cvlan %d translate svlan %d cos %d cvlan %d%s", qinq_data->svlan.vlan_start, \
                        qinq_data->cvlan.vlan_start, qinq_data->svlan_new.vlan_start, qinq_data->svlan_cos, qinq_data->cvlan_new.vlan_start,VTY_NEWLINE);
            }
        }
        if((cvlan_act == VLAN_ACTION_NO) && (svlan_act == VLAN_ACTION_DELETE))
        {
            if(qinq_data->cvlan.vlan_start==qinq_data->cvlan.vlan_end)
            {
                vty_out(vty," vlan-mapping qinq svlan %d cvlan %d delete svlan%s", qinq_data->svlan.vlan_start, \
                        qinq_data->cvlan.vlan_start, VTY_NEWLINE);
            }
            else
            {
                vty_out(vty," vlan-mapping qinq svlan %d cvlan %d to %d delete svlan%s", qinq_data->svlan.vlan_start, \
                        qinq_data->cvlan.vlan_start, qinq_data->cvlan.vlan_end,VTY_NEWLINE);
            }
        }
    }

    return ;
}

/*Edit by jhz,20180313,It applies only to the switch trunk mode,displaying the information in vlan_tag_list*/
/* 接口下的 vlan 配置 */
void l2if_vlan_config_write(struct vty *vty, struct list *list)
{
    int i = 0;
    uint16_t line_num = 0;
    uint32_t switch_vlan_count[4094][2];
    struct listnode *p_listnode = NULL;
    struct vlan_tagged *pvlan_tagged = NULL;

    if(listhead(list) != NULL)
    {
        memset(switch_vlan_count, 0, sizeof(switch_vlan_count));
        pvlan_tagged=(struct vlan_tagged *)listgetdata(list->head);
        switch_vlan_count[line_num][0] = pvlan_tagged->vlanid;
        switch_vlan_count[line_num][1] = pvlan_tagged->vlanid;

        for(ALL_LIST_ELEMENTS_RO(list, p_listnode, pvlan_tagged))
        {
            if(pvlan_tagged->vlanid == (switch_vlan_count[line_num][1] + 1))
            {
                switch_vlan_count[line_num][1] = pvlan_tagged->vlanid;
            }
            else if(pvlan_tagged->vlanid != switch_vlan_count[line_num][0])
            {
                line_num++;
                switch_vlan_count[line_num][0] = pvlan_tagged->vlanid;
                switch_vlan_count[line_num][1] = pvlan_tagged->vlanid;
            }
        }

        /*输出switch vlan*/
        for(i=0; i<=line_num; i++)
        {
            if(switch_vlan_count[i][1]==switch_vlan_count[i][0])
                vty_out(vty," switch vlan %d%s", switch_vlan_count[i][1],VTY_NEWLINE);
            else
                vty_out(vty," switch vlan %d to %d%s", switch_vlan_count[i][0], switch_vlan_count[i][1],VTY_NEWLINE);
        }

    }
    return;
}

/*Edit by jhz,20180313,The new function applies only to the switch hybrid mode, displaying the information in vlan_tag_list.*/
/* 接口下的 vlan 配置 */
void l2if_vlan_tag_config_write(struct vty *vty, struct list *list)
{
    int i = 0;
    int j = 0;
    uint16_t line_num = 0;
    uint32_t switch_vlan_count[4094][3];
    uint32_t temp[3];
    struct listnode *p_listnode = NULL;
    struct vlan_tagged *pvlan_tagged = NULL;
    uint16_t tag_num = 0;
    uint16_t untag_num = 0;

    if(listhead(list) != NULL)
    {
        /*tag vlan*/
        memset(switch_vlan_count, 0, sizeof(switch_vlan_count));
        for(ALL_LIST_ELEMENTS_RO(list, p_listnode, pvlan_tagged))
        {
            if(pvlan_tagged->tag==TAGGED)
            {
                switch_vlan_count[line_num][0] = pvlan_tagged->vlanid;
                switch_vlan_count[line_num][1] = pvlan_tagged->vlanid;
                switch_vlan_count[line_num][2] = TAGGED;
                tag_num++;
                break;
            }
        }
        if(tag_num)
        {
            for(ALL_LIST_ELEMENTS_RO(list, p_listnode, pvlan_tagged))
            {
                if(pvlan_tagged->tag==TAGGED)
                {
                    if(pvlan_tagged->vlanid == (switch_vlan_count[line_num][1] + 1))
                    {
                        switch_vlan_count[line_num][1] = pvlan_tagged->vlanid;
                    }
                    else if(pvlan_tagged->vlanid != switch_vlan_count[line_num][0])
                    {
                        line_num++;
                        switch_vlan_count[line_num][0] = pvlan_tagged->vlanid;
                        switch_vlan_count[line_num][1] = pvlan_tagged->vlanid;
                        switch_vlan_count[line_num][2] = TAGGED;
                    }
                }
            }
        }

        /*untag vlan*/
        for(ALL_LIST_ELEMENTS_RO(list, p_listnode, pvlan_tagged))
        {
            if(pvlan_tagged->tag==UNTAGGED)
            {
                if(tag_num)
                {
                    line_num++;
                }
                switch_vlan_count[line_num][0] = pvlan_tagged->vlanid;
                switch_vlan_count[line_num][1] = pvlan_tagged->vlanid;
                switch_vlan_count[line_num][2] = UNTAGGED;
                untag_num++;
                break;
            }
        }
        if(untag_num)
        {
            for(ALL_LIST_ELEMENTS_RO(list, p_listnode, pvlan_tagged))
            {
                if(pvlan_tagged->tag==UNTAGGED)
                {
                    if(pvlan_tagged->vlanid == (switch_vlan_count[line_num][1] + 1))
                    {
                        switch_vlan_count[line_num][1] = pvlan_tagged->vlanid;
                    }
                    else if(pvlan_tagged->vlanid != switch_vlan_count[line_num][0])
                    {
                        line_num++;
                        switch_vlan_count[line_num][0] = pvlan_tagged->vlanid;
                        switch_vlan_count[line_num][1] = pvlan_tagged->vlanid;
                        switch_vlan_count[line_num][2] = UNTAGGED;
                    }
                }
            }
        }

        /*排序switch vlan*/
        for(i=0; i<line_num; i++)
        {
            for(int j = 0; j<(line_num-i); j++) // j开始等于0，
            {
                if(switch_vlan_count[j][0] > switch_vlan_count[j+1][0])
                {
                    memcpy(temp,switch_vlan_count[j],sizeof(temp));
                    memcpy(switch_vlan_count[j],switch_vlan_count[j+1],sizeof(temp));
                    memcpy(switch_vlan_count[j+1],temp,sizeof(temp));
                }
            }
        }

        /*输出switch vlan*/
        for(i=0; i<=line_num; i++)
        {
            j=switch_vlan_count[i][2];
            if(switch_vlan_count[i][1]==switch_vlan_count[i][0])
                vty_out(vty," switch vlan %d egress-tagged %s%s", switch_vlan_count[i][1],l2if_a_mac_learn[j],VTY_NEWLINE);
            else
                vty_out(vty," switch vlan %d to %d egress-tagged %s%s", switch_vlan_count[i][0], switch_vlan_count[i][1],l2if_a_mac_learn[j],VTY_NEWLINE);
        }
    }

    return;
}


/* 接口下的 lldp 配置 */
void l2if_lldp_config_write(struct vty *vty, struct lldp_port	*port)
{
    if(port == NULL)
        return;

    if(port->adminStatus)
    {
        vty_out(vty, " lldp enable %s", VTY_NEWLINE);

        switch(port->tlv_cfg.basic_tlv_tx_enable)
        {
        case 0x00:
            vty_out(vty, " no lldp tlv-enable basic-tlv all %s", VTY_NEWLINE);
            break;
        case 0x1F:
            /*this is default config , do nothing*/
            break;
        default:
            if(!CHECK_FLAG(port->tlv_cfg.basic_tlv_tx_enable,
                           PORT_DESCRIPTION_TLV_TX_ENABLE))
            {
                vty_out(vty, " no lldp tlv-enable basic-tlv port-description %s", VTY_NEWLINE);
            }

            if(!CHECK_FLAG(port->tlv_cfg.basic_tlv_tx_enable,
                           SYSTEM_NAME_TLV_TX_ENABLE))
            {
                vty_out(vty, " no lldp tlv-enable basic-tlv system-name %s", VTY_NEWLINE);
            }

            if(!CHECK_FLAG(port->tlv_cfg.basic_tlv_tx_enable,
                           SYSTEM_DESCRIPTION_TLV_TX_ENABLE))
            {
                vty_out(vty, " no lldp tlv-enable basic-tlv system-description %s", VTY_NEWLINE);
            }

            if(!CHECK_FLAG(port->tlv_cfg.basic_tlv_tx_enable,
                           SYSTEM_CAPABILITIES_TLV_TX_ENABLE))
            {
                vty_out(vty, " no lldp tlv-enable basic-tlv system-capability %s", VTY_NEWLINE);
            }

            if(!CHECK_FLAG(port->tlv_cfg.basic_tlv_tx_enable,
                           MANAGEMENT_ADDRESS_TLV_TX_ENABLE))
            {
                vty_out(vty, " no lldp tlv-enable basic-tlv management-address %s", VTY_NEWLINE);
            }

            break;
        }

        if (CHECK_FLAG(port->arp_nd_learning, LLDP_LEARNING_ARP))
        {
            if (!port->arp_nd_learning_vlan)
            {
                vty_out(vty, " lldp management-address arp-learning %s", VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, " lldp management-address arp-learning vlan %u%s",
                        port->arp_nd_learning_vlan, VTY_NEWLINE);
            }
        }
    }

    return;
}

void l2if_mstp_config_write(struct vty *vty, struct mstp_port	*port)
{
	unsigned int			instance_id = 0;

    if(port == NULL)
    {
        return;
    }

	/* stp enable */
	if(port->adminStatus)
	{
		vty_out(vty, " stp enable %s", VTY_NEWLINE);
	}

	/*stp priority <0-240>*/
	if(port->port_pri != DEF_PORT_PRIORITY)
	{
		vty_out(vty, " stp priority %d %s", port->port_pri, VTY_NEWLINE);
	}
	
	/*stp cost <1-200000000>*/
	if(port->port_path_cost != DEF_PORT_PATH_COST)
	{
		vty_out(vty, " stp cost %d %s", port->port_path_cost, VTY_NEWLINE);
	}

	/*stp instance <1-8> port-priority <0-240>
	**stp instance <1-8> cost <1-200000000>*/
	for(instance_id = 0; instance_id < MSTP_INSTANCE_MAX; instance_id++)
	{
		if(port->msti_port_pri[instance_id] != DEF_PORT_PRIORITY)
		{
			vty_out(vty, " stp instance %d port priority %d %s", instance_id+1, port->msti_port_pri[instance_id], VTY_NEWLINE);
		}
		
		if(port->msti_port_path_cost[instance_id] != DEF_PORT_PATH_COST)
		{
			vty_out(vty, " stp instance %d cost %d %s", instance_id+1, port->msti_port_path_cost[instance_id], VTY_NEWLINE);
		}
	}
    if(port->mstp_edge_port == MSTP_TRUE)
    {
        vty_out(vty, " stp edge-port %s", VTY_NEWLINE);
    }

	/*stp filter-port*/
    if(port->mstp_filter_port == MSTP_TRUE)
    {
        vty_out(vty, " stp filter-port %s", VTY_NEWLINE);
    }

	/*stp p2p*/
	if(port->mstp_p2p == MSTP_TRUE)
    {
        vty_out(vty, " stp p2p %s", VTY_NEWLINE);
    }

    return;
}

void l2if_reserve_vlan_config_write(struct vty *vty,uint32_t ifindex)
{
    uint8_t port = 0;
	int i = 0;
    uint16_t reserve_vlanid = 0;

	if(IFM_TYPE_IS_TRUNK(ifindex) && !(IFM_TYPE_IS_SUBPORT(ifindex)))
	{
		for(i = 0; i < 3; i++)
		{
			if(trunk_reserve_vlan_table[i].trunkid == IFM_TRUNK_ID_GET(ifindex))
			{
				if(trunk_reserve_vlan_table[i].reserve_vlan != trunk_reserve_vlan_table[i].reserve_vlan_bak)
				vty_out(vty, " reserve-vlan %d%s", reserve_vlanid, VTY_NEWLINE);
			}
		}
	}
	else
	{
	    port = IFM_PORT_ID_GET ( ifindex );
	    if( port < 1 || port > 6 )
	    {
	        vty_error_out (vty, "Invalid port number! %s", VTY_NEWLINE);
	        return ;
	    }

	    switch(port)
	    {
	    case 1:
	        reserve_vlanid = reserve_vlan_table[port-1];

	        if( reserve_vlanid != DEF_RESV_VLAN1 )
	        {
	            vty_out(vty, " reserve-vlan %d%s", reserve_vlanid, VTY_NEWLINE);
	        }
	        break;
	    case 2:
	        reserve_vlanid = reserve_vlan_table[port-1];

	        if( reserve_vlanid != DEF_RESV_VLAN2 )
	        {
	            vty_out(vty, " reserve-vlan %d%s", reserve_vlanid, VTY_NEWLINE);
	        }
	        break;
	    case 3:
	        reserve_vlanid = reserve_vlan_table[port-1];

	        if( reserve_vlanid != DEF_RESV_VLAN3 )
	        {
	            vty_out(vty, " reserve-vlan %d%s", reserve_vlanid, VTY_NEWLINE);
	        }
	        break;
	    case 4:
	        reserve_vlanid = reserve_vlan_table[port-1];

	        if( reserve_vlanid != DEF_RESV_VLAN4 )
	        {
	            vty_out(vty, " reserve-vlan %d%s", reserve_vlanid, VTY_NEWLINE);
	        }
	        break;
	    case 5:
	        reserve_vlanid = reserve_vlan_table[port-1];

	        if( reserve_vlanid != DEF_RESV_VLAN5 )
	        {
	            vty_out(vty, " reserve-vlan %d%s", reserve_vlanid, VTY_NEWLINE);
	        }
	        break;
	    case 6:
	        reserve_vlanid = reserve_vlan_table[port-1];

	        if( reserve_vlanid != DEF_RESV_VLAN6 )
	        {
	            vty_out(vty, " reserve-vlan %d%s", reserve_vlanid, VTY_NEWLINE);
	        }
	        break;
	    default:
	        break;
	    }
	}

    return ;
}
#if 0
/*
static void l2if_etree_config_write(struct vty *vty, struct l2if *pif)
{
	struct port_islt *pislt = NULL;
	struct listnode *vnode = NULL;
	struct listnode *node = NULL;
	void *vdata = NULL;
	void *data = NULL;

	if (NULL == pif)
	{
		return;
	}

	for (ALL_LIST_ELEMENTS_RO(pif->pislt, node, data))
	{
		pislt = (struct port_islt *)data;

		if (EVP_TREE == pislt->type)
		{
			if (NULL != listhead(pif->switch_info.vlan_list))
			{
				for (ALL_LIST_ELEMENTS_RO(pif->switch_info.vlan_list, vnode, vdata))
				{
					if (pislt->id == (uint32_t)vdata)
					{
						vty_out( vty, " port-isolation %s %d %s%s", "vlan", pislt->id,  (ROOT == pislt->role) ? "root" : "", VTY_NEWLINE);
						continue;
					}
				}
			}
		}
		else
		{
			vty_out( vty, " port-isolation %s %d %s%s", "group", pislt->id,  (ROOT == pislt->role) ? "root" : "", VTY_NEWLINE);
		}
	}	
}
*/
#endif
/* write the configuration of each interface */
int l2if_config_write (struct vty *vty)
{
    struct l2if *pif = NULL;
	uint8_t port = 0;
    char ifname[IFNET_NAMESIZE];
    uint32_t ifindex;
    struct hash_bucket *pbucket = NULL;
    int cursor;
	uint8_t mode = 0;
	int i = 0;
    uchar switch_info[3][10] = {"hybrid","trunk","access"};
	struct ifm_info pifm_info;

    HASH_BUCKET_LOOP( pbucket, cursor, l2if_table )
    {
        pif =(struct l2if *) pbucket->data;
        ifindex = pif->ifindex;

        if(ifm_get_all_info(ifindex, MODULE_ID_L2, &pifm_info) != 0)
        {
            L2_COMMON_LOG_DBG("%s[%d]:leave %s,The port does not exist.\n",__FILE__,__LINE__,__FUNCTION__);
            continue;
        }
        ifm_get_name_by_ifindex(ifindex, ifname);
        vty_out ( vty, "interface %s%s", ifname, VTY_NEWLINE );

        /* switch 接口的配置 */
        if(ifm_get_mode(ifindex,MODULE_ID_L2, &mode) != 0)
        {
            L2_COMMON_LOG_DBG("%s[%d]:leave %s,ifm_get_mode error.\n",__FILE__,__LINE__,__FUNCTION__);
            continue;
        }
        if((uint8_t)IFNET_MODE_SWITCH == mode)
    	{
    		if( pif->switch_info.mode !=  SWITCH_MODE_HYBRID )
    			vty_out ( vty, " switch %s%s", switch_info[pif->switch_info.mode],VTY_NEWLINE );
    	}

        if(pif->switch_info.limit_num!=0)
        {
            vty_out ( vty, " mac limit %d action %s%s", pif->switch_info.limit_num,mac_limit_action[pif->switch_info.limit_action],VTY_NEWLINE );
        }

        if((ENABLE==pif->switch_info.storm_control.storm_unicast)||(ENABLE==pif->switch_info.storm_control.storm_broadcast)||
                (ENABLE==pif->switch_info.storm_control.storm_multicast))
        {
            if(ENABLE==pif->switch_info.storm_control.storm_unicast)
            {
                vty_out ( vty, " storm-suppress unicast rate %d%s",pif->switch_info.storm_control.unicast_rate,VTY_NEWLINE);
            }
            if(ENABLE==pif->switch_info.storm_control.storm_broadcast)
            {
                vty_out ( vty, " storm suppress broadcast rate %d%s",pif->switch_info.storm_control.broadcast_rate,VTY_NEWLINE);
            }
            if(ENABLE==pif->switch_info.storm_control.storm_multicast)
            {
                vty_out ( vty, " storm suppress multicast rate %d%s",pif->switch_info.storm_control.multicast_rate,VTY_NEWLINE);
            }
        }

		if( devtype == ID_HT157 || devtype == ID_HT158 )
		{
			if(IFM_TYPE_IS_TRUNK(ifindex) && !(IFM_TYPE_IS_SUBPORT(ifindex)))
			{
				for(i = 0; i < 3; i++)
				{
					if(trunk_reserve_vlan_table[i].trunkid == IFM_TRUNK_ID_GET(ifindex))
					{
						if( pif->switch_info.access_vlan != trunk_reserve_vlan_table[i].reserve_vlan_bak )
							vty_out ( vty, " switch access-vlan %d%s", pif->switch_info.access_vlan,VTY_NEWLINE );
					}
				}
			}
			else
			{
			    port = IFM_PORT_ID_GET ( ifindex );
			    if( port < 1 || port > 6 )
			    {
			        vty_error_out (vty, "Invalid port number! %s", VTY_NEWLINE);
			        return CMD_WARNING;
			    }
				if(pif->switch_info.access_vlan != (DEF_RESV_VLAN1 + port-1))
	            	vty_out ( vty, " switch access-vlan %d%s", pif->switch_info.access_vlan,VTY_NEWLINE );
			}
		}
		else
		{
			if(pif->switch_info.access_vlan != DEFAULT_VLAN_ID)
            	vty_out ( vty, " switch access-vlan %d%s", pif->switch_info.access_vlan,VTY_NEWLINE );	
		}
        
        if(pif->switch_info.dot1q_tunnel != DEFAULT_VLAN_ID)
            vty_out ( vty, " dot1q-tunnel svlan %d cos %d%s", pif->switch_info.dot1q_tunnel,pif->switch_info.dot1q_cos,VTY_NEWLINE );

        if(NULL != pif->switch_info.vlan_tag_list->head)
        {
            if(pif->switch_info.mode==SWITCH_MODE_HYBRID)
            {
                l2if_vlan_tag_config_write(vty, pif->switch_info.vlan_tag_list);
            }
            else if(pif->switch_info.mode==SWITCH_MODE_TRUNK)
            {
                l2if_vlan_config_write(vty, pif->switch_info.vlan_tag_list);
            }
        }
        if(pif->switch_info.qinq_list)
        {
            l2if_qinq_config_write(vty, pif->switch_info.qinq_list);
        }
        /*lldp interface config*/
        if(pif->lldp_port_info)
        {
            l2if_lldp_config_write(vty, pif->lldp_port_info);
        }
        /*mstp interface config*/
        if(pif->mstp_port_info)
        {
            l2if_mstp_config_write(vty, pif->mstp_port_info);
        }

        if (pif->ploopdetect)
        {
            l2if_loopdetect_config_write(vty, pif->ploopdetect);
        }
        if(pif->pefm)
        {
            l2if_efm_config_write(vty, pif->pefm);
        }
        if(NULL != pif->l2cp_master)
        {
            l2if_l2cp_config_write(vty, pif->l2cp_master);
        }

		if( devtype == ID_HT157 || devtype == ID_HT158 )
		{
			 l2if_reserve_vlan_config_write(vty,pif->ifindex);
		}
    }

    /*trunk config*/
    trunk_config_write(vty);

    return ERRNO_SUCCESS;
}

void trunk_vlan_cli_init (void)
{
    install_element (TRUNK_IF_NODE, &l2_mode_switch_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &show_switch_interface_cmd, CMD_LOCAL);
    install_element (TRUNK_IF_NODE, &no_switch_access_vlan_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &switch_access_vlan_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &no_switch_vlan_add_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &switch_vlan_add_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &switch_vlan_tag_add_cmd, CMD_SYNC);
}

void l2if_vlan_cli_init (void)
{
    install_element (PHYSICAL_IF_NODE, &no_switch_access_vlan_cmd, CMD_SYNC);
    install_element (PHYSICAL_IF_NODE, &switch_access_vlan_cmd, CMD_SYNC);
    install_element (PHYSICAL_IF_NODE, &no_switch_vlan_add_cmd, CMD_SYNC);
    install_element (PHYSICAL_IF_NODE, &switch_vlan_add_cmd, CMD_SYNC);
    install_element (PHYSICAL_IF_NODE, &switch_vlan_tag_add_cmd, CMD_SYNC);
    install_element (PHYSICAL_IF_NODE, &l2if_mac_limit_cmd, CMD_SYNC);
    install_element (PHYSICAL_IF_NODE, &no_l2if_mac_limit_cmd, CMD_SYNC);
    install_element (PHYSICAL_IF_NODE, &no_l2if_storm_supress_cmd, CMD_SYNC);
    install_element (PHYSICAL_IF_NODE, &l2if_storm_supress_cmd, CMD_SYNC);
    install_element (PHYSICAL_IF_NODE, &reserve_vlan_set_cmd,CMD_SYNC);
    install_element (PHYSICAL_IF_NODE, &no_reserve_vlan_set_cmd,CMD_SYNC);
	
	install_element (CONFIG_NODE, &l2_debug_cmd,CMD_SYNC);
	install_element (CONFIG_NODE, &show_l2_debug_cmd,CMD_SYNC);
    install_element (CONFIG_NODE, &reserve_vlan_show_cmd,CMD_LOCAL);
	install_element (CONFIG_NODE, &show_l2_timerinfo_cmd,CMD_LOCAL);
}


/* 注册接口下的命令 */
void l2if_cli_init (void)
{
    ifm_port_init ( l2if_config_write);

    install_element (PHYSICAL_IF_NODE, &show_switch_interface_cmd, CMD_LOCAL);
    install_element (PHYSICAL_IF_NODE, &l2_mode_switch_cmd, CMD_SYNC);

    l2cp_cli_init();
    l2if_vlan_cli_init(); /* 注册接口下的 vlan 命令 */

    l2if_qinq_cli_init(); /* 注册接口下的 qinq 命令 */

    trunk_vlan_cli_init ();/* 注册trunk 口下的 vlan 命令 */
}
