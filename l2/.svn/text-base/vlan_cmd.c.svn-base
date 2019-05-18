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
#include <lib/errcode.h>
#include <lib/log.h>
#include <syslog/syslogd.h>
#include "vlan.h"
#include "qinq.h"
#include "l2_if.h"

uchar vlan_a_mac_learn[2][10] = {"disable","enable"};
uchar vlan_a_storm_suppress[2][10] = {"disable","enable"};
uchar vlan_mac_limit_action[2][10] = {"discard","forward"};
uchar vlan_tag[2][10] = {"tagged","untagged"};

DEFUN(show_vlan_summary,
      show_vlan_summary_cmd,
      "show vlan summary",
      "Show command\n"
      "Vlan node\n"
      "Vlan information summary\n"
     )
{

    uint16_t vlan_num = 0;
    uint16_t i = 0;

    for(i=1; i<4095; i++)
    {
        if(DISABLE != vlan_table[i]->vlan.enable)
            vlan_num++;
    }
    vty_out (vty, "The total number of vlans is : %d%s", vlan_num, VTY_NEWLINE);

    return CMD_SUCCESS;
}


/*Edit by jhz,20180313，When displaying vlan member ports, add tagged or untagged flags.*/
DEFUN(show_vlan,
      show_vlan_cmd,
      "show vlan  [<1-4094>]",
      "Show command\n"
      "Vlan node\n"
      "Vlanid <1-4094>\n"
     )
{

    uint16_t vlanid = 0;
    uint16_t i = 0;
    uint8_t mac_learn = 0;
    uint8_t storm_suppress = 0;
    uint16_t vlan_num = 0;
    uint8_t inter_num = 0;
    uchar test_name = '\0';
    char inter_name[32] = "\0";
    struct listnode *p_listnode = NULL;
    struct port_tagged *pvlan_tagged=NULL;

    if(NULL != argv[0])
    {
        vlanid = atoi(argv[0]);

        if(DISABLE != vlan_table[vlanid]->vlan.enable)
        {
            mac_learn = vlan_table[vlanid]->vlan.mac_learn;
            vty_out (vty, "vlan %d:%s", vlanid, VTY_NEWLINE);
            if(!strcmp((const char *)&test_name,(const char *)vlan_table[vlanid]->name))
            {
                vty_out ( vty, "     name        : %-32s%s", "-",VTY_NEWLINE);
            }
            else
            {
                vty_out (vty, "     name        : %-32s%s", vlan_table[vlanid]->name,VTY_NEWLINE);
            }
            vty_out (vty, "     mac learn   : %-15s%s", vlan_a_mac_learn[mac_learn],VTY_NEWLINE);
            if(vlan_table[vlanid]->vlan.limit_num!=0)
            {
                vty_out ( vty, "     mac limit   : %d%s", vlan_table[vlanid]->vlan.limit_num,VTY_NEWLINE );
                vty_out ( vty, "     limit action: %s%s", vlan_mac_limit_action[vlan_table[vlanid]->vlan.limit_action],VTY_NEWLINE );
            }
            storm_suppress = vlan_table[vlanid]->vlan.storm_unicast;
            vty_out (vty, "     unicast   storm suppress : %-15s%s", vlan_a_storm_suppress[storm_suppress],VTY_NEWLINE);
            storm_suppress = vlan_table[vlanid]->vlan.storm_broadcast;
            vty_out (vty, "     broadcast storm suppress : %-15s%s", vlan_a_storm_suppress[storm_suppress],VTY_NEWLINE);
            storm_suppress = vlan_table[vlanid]->vlan.storm_multicast;
            vty_out (vty, "     multicast storm suppress : %-15s%s", vlan_a_storm_suppress[storm_suppress],VTY_NEWLINE);

            for(ALL_LIST_ELEMENTS_RO(&(vlan_table[vlanid]->port_tag_list),p_listnode,pvlan_tagged))
            {
                if(pvlan_tagged->tag!=DOT1Q_TUNNEL)
                {
                    inter_num++;
                }
            }
            vty_out (vty, "     Total interface num : %d%s", inter_num,VTY_NEWLINE);
            for(ALL_LIST_ELEMENTS_RO(&(vlan_table[vlanid]->port_tag_list),p_listnode,pvlan_tagged))
            {
                ifm_get_name_by_ifindex ( pvlan_tagged->ifindex, inter_name);
                if(pvlan_tagged->tag!=DOT1Q_TUNNEL)
                {
                    vty_out (vty, "     %s  %s%s", inter_name,vlan_tag[pvlan_tagged->tag],VTY_NEWLINE);
                }
            }
            VLAN_LOG_DBG("%s[%d]:%s:vlanid: %d;vlan name:%s,vlan learn:%s \n",__FILE__,__LINE__,__FUNCTION__, vlanid,vlan_table[vlanid]->name, vlan_a_mac_learn[mac_learn]);
        }
        else
        {
            VLAN_LOG_DBG("%s[%d]:%s:vlan %d not create\n",__FILE__,__LINE__,__FUNCTION__,vlanid);
            vty_error_out (vty, "Vlan %d  not create%s",vlanid, VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    else
    {
        for(i=1; i<4095; i++)
        {
            if(DISABLE != vlan_table[i]->vlan.enable)
                vlan_num++;
        }
        vty_out (vty, "The total number of vlans is : %d%s", vlan_num, VTY_NEWLINE);


        for(i=0; i<4095; i++)
        {
            if(DISABLE != vlan_table[i]->vlan.enable)
            {
                inter_num = 0;
                mac_learn = vlan_table[i]->vlan.mac_learn;
                vty_out (vty, "vlan %d:%s", i, VTY_NEWLINE);
                if(!strcmp((const char *)&test_name,(const char *)vlan_table[i]->name))
                {
                    vty_out (vty, "     name        : %-32s%s", "-",VTY_NEWLINE);
                }
                else
                {
                    vty_out (vty, "     name        : %-32s%s", vlan_table[i]->name,VTY_NEWLINE);
                }
                vty_out (vty, "     mac learn   : %-15s%s", vlan_a_mac_learn[mac_learn],VTY_NEWLINE);
                if(vlan_table[i]->vlan.limit_num!=0)
                {
                    vty_out ( vty, "     mac limit   : %d%s", vlan_table[i]->vlan.limit_num,VTY_NEWLINE );
                    vty_out ( vty, "     limit action: %s%s", vlan_mac_limit_action[vlan_table[i]->vlan.limit_action],VTY_NEWLINE );
                }
                storm_suppress = vlan_table[i]->vlan.storm_unicast;
                vty_out (vty, "     unicast   storm suppress : %-15s%s", vlan_a_storm_suppress[storm_suppress],VTY_NEWLINE);
                storm_suppress = vlan_table[i]->vlan.storm_broadcast;
                vty_out (vty, "     broadcast storm suppress : %-15s%s", vlan_a_storm_suppress[storm_suppress],VTY_NEWLINE);
                storm_suppress = vlan_table[i]->vlan.storm_multicast;
                vty_out (vty, "     multicast storm suppress : %-15s%s", vlan_a_storm_suppress[storm_suppress],VTY_NEWLINE);

                for(ALL_LIST_ELEMENTS_RO(&(vlan_table[i]->port_tag_list),p_listnode,pvlan_tagged))
                {
                    if(pvlan_tagged->tag!=DOT1Q_TUNNEL)
                    {
                        inter_num++;
                    }
                }
                vty_out (vty, "     Total interface num : %d%s", inter_num,VTY_NEWLINE);
                for(ALL_LIST_ELEMENTS_RO(&(vlan_table[i]->port_tag_list),p_listnode,pvlan_tagged))
                {
                    ifm_get_name_by_ifindex ( pvlan_tagged->ifindex, inter_name);
                    if(pvlan_tagged->tag!=DOT1Q_TUNNEL)
                    {
                        vty_out (vty, "     %s  %s%s", inter_name,vlan_tag[pvlan_tagged->tag],VTY_NEWLINE);
                    }
                }
                VLAN_LOG_DBG("%s[%d]:%s:vlanid: %d;vlan name:%s,vlan learn:%s \n",__FILE__,__LINE__,__FUNCTION__, i,vlan_table[i]->name, vlan_a_mac_learn[mac_learn]);
            }

        }
    }

    return CMD_SUCCESS;
}

DEFUN(no_vlan_get_range,
      no_vlan_get_range_cmd,
      "no vlan <1-4094> {to <1-4094>}",
      "No command\n"
      "Vlan node\n"
      "Vlanid start <1-4094>\n"
      "To\n"
      "Vlanid end <1-4094>\n"
     )
{
    uint16_t v_start = 0;
    uint16_t v_end = 0;
    uint16_t vlanid = 0;
    int ret = 0;
    int i=0;
    uint8_t disable_flag = 1;
    uint8_t enable_flag = 0;

    v_start = atoi(argv[0]);
    if(NULL == argv[1])
        v_end = v_start;
    else
        v_end = atoi(argv[1]);

    if(v_end < v_start)
    {
        VLAN_LOG_DBG("%s[%d]:%s:error: %d less than %d\n",__FILE__,__LINE__,__FUNCTION__,v_end,v_start);
        vty_error_out (vty, "Vlan input error,vlanid_end(%d) must not less than vlanid_start(%d) !%s",v_end,v_start, VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*check if all vlan in the range is enable*/
    for(vlanid=v_start; vlanid<=v_end; ++vlanid)
    {
        if(DISABLE == vlan_table[vlanid]->vlan.enable)
        {
            disable_flag = 0;
        }
        else
        {
            enable_flag = 1;
        }

		if( devtype == ID_HT157 || devtype == ID_HT158 )
		{
			/*for( i=0; i<6; i++ )
	        {
	            if( vlanid == (DEF_RESV_VLAN1 + i) )
	            {
	                vty_error_out (vty, "Vlan %d is a reserve vlan,can not be use!%s",vlanid, VTY_NEWLINE);
	                return CMD_WARNING;
	            }
	        }*/
	        for( i=0; i<3; i++ )
		    {
		        if( vlanid == trunk_reserve_vlan_table[i].reserve_vlan )
		        {
		            vty_error_out (vty, "Vlan %d is a reserve vlan,can not be use.if you want to use this vlan,please change reserve vlan by 'reserve-vlan <1-4094> in mode switch' !%s", vlanid, VTY_NEWLINE);
		            return CMD_WARNING;
		        }
		    }
	        for( i=0; i<6; i++ )
	        {
	            if( vlanid == reserve_vlan_table[i] )
	            {
	                vty_error_out (vty, "Vlan %d is a reserve vlan,can not be use.if you want to use this vlan,please change reserve vlan by 'reserve-vlan <1-4094> in mode switch' !%s",vlanid, VTY_NEWLINE);
	                return CMD_WARNING;
	            }
	        }
		}
        
    }

    if(0 == disable_flag)
    {
        if(0 == enable_flag)   //全部未创建
        {
            if(v_end == v_start)
                vty_error_out (vty, "Delete vlan error,vlan %d is not exist%s",v_start, VTY_NEWLINE);
            else
                vty_error_out (vty, "Delete vlan error,vlan<%d-%d> are not exist%s",v_start, v_end, VTY_NEWLINE);
            return CMD_WARNING;
        }
        else   //部分成员未创建
        {
            vty_error_out (vty, "Delete vlan error,vlan<%d-%d> have members does not exist%s",v_start, v_end, VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    else
    {
        ret = vlan_delete(v_start,v_end);
        if(ret < 0)
        {
            VLAN_LOG_DBG("%s[%d]:%s:delete vlan error\n",__FILE__,__LINE__,__FUNCTION__);
            vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        VLAN_LOG_DBG("%s[%d]:%s:delete vlan<%d-%d>\n",__FILE__,__LINE__,__FUNCTION__,v_start,v_end);
        return CMD_SUCCESS;
    }
}

DEFUN(vlan_get_range,
      vlan_get_range_cmd,
      "vlan <1-4094> to <1-4094>",
      "Vlan node\n"
      "Vlanid start <1-4094>\n"
      "To\n"
      "Vlanid end <1-4094>\n"
     )
{
    uint16_t v_start = 0;
    uint16_t v_end = 0;
    int i = 0;
    int j = 0;
    int flag=0;
    uint16_t line_num = 0;
    uint16_t vlan_count[4094][2];

    v_start = atoi(argv[0]);
    v_end = atoi(argv[1]);

    if(v_end < v_start)
    {
        VLAN_LOG_DBG("%s[%d]:%s:error: %d less than %d\n",__FILE__,__LINE__,__FUNCTION__,v_end,v_start);
        vty_error_out (vty, "Vlan input error,vlanid_end(%d) must not less than vlanid_start(%d) !%s",v_end,v_start, VTY_NEWLINE);
        return CMD_WARNING;
    }

	if( devtype == ID_HT157 || devtype == ID_HT158 )
	{
	    for(i=v_start; i<=v_end; i++)
	    {
	        /*for( j=0; j<6; j++ )
	        {
	            if( i == (DEF_RESV_VLAN1 + j) )
	            {
	                vty_error_out (vty, "Vlan %d is a reserve vlan,can not be use!%s",i, VTY_NEWLINE);
	                return CMD_WARNING;
	            }
	        }*/
	        for( j=0; j<3; j++ )
		    {
		        if( i == trunk_reserve_vlan_table[j].reserve_vlan )
		        {
		            vty_error_out (vty, "Vlan %d is a reserve vlan,can not be use.if you want to use this vlan,please change reserve vlan by 'reserve-vlan <1-4094> in mode switch' !%s", i, VTY_NEWLINE);
		            return CMD_WARNING;
		        }
		    }
	        for( j=0; j<6; j++ )
	        {
	            if( i == reserve_vlan_table[j] )
	            {
	                vty_error_out (vty, "Vlan %d is a reserve vlan,can not be use.if you want to use this vlan,please change reserve vlan by 'reserve-vlan <1-4094> in mode switch' !%s",i, VTY_NEWLINE);
	                return CMD_WARNING;
	            }
	        }
	    }
	}

    memset(vlan_count, 0, sizeof(vlan_count));
    for(i=v_start; i<=v_end; i++)
    {
        if(DISABLE == vlan_table[i]->vlan.enable)
        {
            if(flag==0)
            {
                vlan_count[line_num][0] = i;
                vlan_count[line_num][1] =i;
                flag=1;
                continue;
            }

            if((vlan_count[line_num][1] + 1)==i)
            {
                vlan_count[line_num][1] = i;
            }
            else
            {
                line_num++;
                vlan_count[line_num][0] = i;
                vlan_count[line_num][1] = i;
            }
        }
    }

    if(flag==1)
    {
        for(i=0; i<=line_num; i++)
        {
            vlan_create(vlan_count[i][0], vlan_count[i][1]);
        }
    }
    VLAN_LOG_DBG("%s[%d]:%s:create vlan<%d-%d>\n",__FILE__,__LINE__,__FUNCTION__,v_start,v_end);
    return CMD_SUCCESS;
}


DEFUN(vlan_get,
      vlan_get_cmd,
      "vlan <1-4094>",
      "Vlan node\n"
      "Vlanid start <1-4094>\n"
     )
{
    int vlanid = 0;
    int ret = 0;
    int i = 0;

    vlanid = atoi(argv[0]);

	if( devtype == ID_HT157 || devtype == ID_HT158 )
	{
	    /*for( i=0; i<6; i++ )
	    {
	        if( vlanid == (DEF_RESV_VLAN1 + i) )
	        {
	            vty_error_out (vty, "Vlan %d is a reserve vlan,can not be use!%s",vlanid, VTY_NEWLINE);
	            return CMD_WARNING;
	        }
	    }*/

		for( i=0; i<3; i++ )
	    {
	        if( vlanid == trunk_reserve_vlan_table[i].reserve_vlan )
	        {
	            vty_error_out (vty, "Vlan %d is a reserve vlan,can not be use.if you want to use this vlan,please change reserve vlan by 'reserve-vlan <1-4094> in mode switch' !%s", vlanid, VTY_NEWLINE);
	            return CMD_WARNING;
	        }
	    }

	    for( i=0; i<6; i++ )
	    {
	        if( vlanid == reserve_vlan_table[i] )
	        {
	            vty_error_out (vty, "Vlan %d is a reserve vlan,can not be use.if you want to use this vlan,please change reserve vlan by 'reserve-vlan <1-4094> in mode switch' !%s", vlanid, VTY_NEWLINE);
	            return CMD_WARNING;
	        }
	    }
	}

    if(DISABLE == vlan_table[vlanid]->vlan.enable)
    {
        ret = vlan_create(vlanid,vlanid);
        if(ret < 0)
        {
            VLAN_LOG_DBG("%s[%d]:%s:create vlan error\n",__FILE__,__LINE__,__FUNCTION__);
            vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    vty->node =(int) VLAN_NODE;
    vty->index = (void*)vlanid;

    VLAN_LOG_DBG("%s[%d]:%s:create & inter vlan %d\n",__FILE__,__LINE__,__FUNCTION__,vlanid);
    return CMD_SUCCESS;
}

DEFUN(vlan_name,
      vlan_name_cmd,
      "name NAME",
      "Set vlan name\n"
      "Vlan name and length of name is <1-31>\n"
     )
{
    uint32_t vlanid = 0;
    uint8_t length = 0;

    length = strlen(argv[0]);
    if(NAME_STRING_LEN - 1 < length)
    {
        VLAN_LOG_DBG("%s[%d]:length of name is %d, out of <1-31>\n",__FILE__,__LINE__,length-1);
        vty_error_out (vty, "Length of name length error ,should be <1-31>%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    vlanid = (uint32_t)vty->index;

    memcpy(vlan_table[vlanid]->name, argv[0], length);
    vlan_table[vlanid]->name[length] = '\0';

    VLAN_LOG_DBG("%s[%d]:%s:vlanid =  %d,name = %s\n",__FILE__,__LINE__,__FUNCTION__,vlanid,vlan_table[vlanid]->name);
    return CMD_SUCCESS;
}

DEFUN(no_vlan_name,
      no_vlan_name_cmd,
      "no name",
      "Default vlan name\n"
      "Name\n"
     )
{
    uint32_t vlanid = 0;

    vlanid = (uint32_t)vty->index;
    vlan_table[vlanid]->name[0] = '\0';

    VLAN_LOG_DBG("%s[%d]:%s:vlanid =  %d,name = %s\n",__FILE__,__LINE__,__FUNCTION__,vlanid,vlan_table[vlanid]->name);
    return CMD_SUCCESS;
}


DEFUN(vlan_mac_learn,
      vlan_mac_learn_cmd,
      "mac learn (enable|disable)",
      "Mac\n"
      "Learn\n"
      "Enable\n"
      "Disable\n"
     )
{
    uint32_t vlanid = 0;
    uint8_t learn_num = 0;
    uint32_t limit_num = 0;
    uint8_t action=0;

    vlanid = (uint32_t)vty->index;

    if(!strncmp("enable", argv[0], strlen("en")))
        learn_num =ENABLE;
    else if(!strncmp("disable", argv[0], strlen("dis")))
        learn_num =DISABLE;
    else
    {
        VLAN_LOG_DBG("%s[%d]:%s:invalid input\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out (vty, "Invalid input%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if(learn_num==vlan_table[vlanid]->vlan.mac_learn)
    {
        return CMD_SUCCESS;
    }

    if(vlan_set_info(vlanid,vlanid, &learn_num, NULL,NULL,VLAN_INFO_MAC_LEARN))
    {
        VLAN_LOG_DBG("%s[%d]:%s:vlanid learn msg send error\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*先设置mac learn disable,然后设置mac limit_num会使能mac learn,
    将limit_num保存在本地,不下发至hal,当配置mac learn使能,配置mac limit*/
    if(learn_num ==ENABLE)
    {
        limit_num=vlan_table[vlanid]->vlan.limit_num;
        action=vlan_table[vlanid]->vlan.limit_action;
        if(vlan_set_info(vlanid, vlanid,&limit_num,&action,NULL, VLAN_INFO_MAC_LIMIT))	/*下发保存在本地的mac limit信息至hal*/
        {
            VLAN_LOG_DBG("%s[%d]:%s:vlanid limit msg send error\n",__FILE__,__LINE__,__FUNCTION__);
            vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    VLAN_LOG_DBG("%s[%d]:%s:vlanid: %d;vlan mac learn=%s\n",__FILE__,__LINE__,__FUNCTION__,vlanid,vlan_a_mac_learn[learn_num]);
    return CMD_SUCCESS;
}

DEFUN(vlan_mac_limit,
      vlan_mac_limit_cmd,
      "mac limit <1-16384> action (discard | forward)",
      "Mac\n"
      "Limit the count of mac learn\n"
      "Limit value <1-16384> and the default value is 0.\n"
      "Action when limit value is exceeded\n"
      "Discard the packet\n"
      "Forward the packet\n")
{
    uint32_t limit_num = 0;
    uint32_t vlanid = 0;
    uint8_t action=0;

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

    vlanid = (uint32_t)vty->index;

    if((limit_num==vlan_table[vlanid]->vlan.limit_num)&&(action==vlan_table[vlanid]->vlan.limit_action))
    {
        return CMD_SUCCESS;
    }

    /*先设置mac learn disable,然后设置mac limit_num会使能mac learn*/
    if(vlan_table[vlanid]->vlan.mac_learn==DISABLE)
    {
        /*保存至本地，不下发hal*/
        vlan_table[vlanid]->vlan.limit_num=limit_num;
        vlan_table[vlanid]->vlan.limit_action=action;
    }
    else
    {
        if(vlan_set_info(vlanid, vlanid,&limit_num,&action,NULL, VLAN_INFO_MAC_LIMIT))
        {
            VLAN_LOG_DBG("%s[%d]:%s:vlanid limit msg send error\n",__FILE__,__LINE__,__FUNCTION__);
            vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    VLAN_LOG_DBG("%s[%d]:%s:vlanid: %d;vlan mac limit=%d,action:%s\n",__FILE__,__LINE__,__FUNCTION__,vlanid,limit_num,vlan_mac_limit_action[action]);
    return CMD_SUCCESS;

}

DEFUN(no_vlan_mac_limit,
      no_vlan_mac_limit_cmd,
      "no mac limit",
      "Clear config\n"
      "Mac\n"
      "Limit the count of mac learn\n")
{
    uint32_t limit_num = 0;
    uint32_t vlanid = 0;
    uint8_t action=0;

    vlanid = (uint32_t)vty->index;

    if(limit_num==vlan_table[vlanid]->vlan.limit_num)
    {
        return CMD_SUCCESS;
    }

    /*先设置mac learn disable,然后设置mac limit_num会使能mac learn*/
    if(vlan_table[vlanid]->vlan.mac_learn==DISABLE)
    {
        /*保存至本地，不下发hal*/
        vlan_table[vlanid]->vlan.limit_num=limit_num;
    }
    else
    {
        action = ENABLE;    //default enable forwarding
        if(vlan_set_info(vlanid, vlanid,&limit_num,&action,NULL, VLAN_INFO_MAC_LIMIT))
        {
            VLAN_LOG_DBG("%s[%d]:%s:vlanid limit msg send error\n",__FILE__,__LINE__,__FUNCTION__);
            vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    VLAN_LOG_DBG("%s[%d]:%s:vlanid: %d;vlan mac limit=%d\n",__FILE__,__LINE__,__FUNCTION__,vlanid,limit_num);
    return CMD_SUCCESS;

}

DEFUN(no_vlan_storm_supress,
      no_vlan_storm_supress_cmd,
      "no storm suppress {unicast | broadcast | multicast}",
      "Clear config\n"
      "Broadcast storm\n"
      "Storm-control\n"
      "Unicast\n"
      "Broadcast\n"
      "Multicast\n")
{
    uint32_t vlanid = 0;
    uint8_t unicast_num;
    uint8_t broadcast_num;
    uint8_t multicast_num;

    vlanid = (uint32_t)vty->index;
    unicast_num=vlan_table[vlanid]->vlan.storm_unicast;
    broadcast_num=vlan_table[vlanid]->vlan.storm_broadcast;
    multicast_num=vlan_table[vlanid]->vlan.storm_multicast;

    if (argv[0] != NULL && argv[1] != NULL && argv[2] != NULL )
    {
        unicast_num=DISABLE;
        broadcast_num=DISABLE;
        multicast_num=DISABLE;
    }
    else if (argv[0] != NULL && argv[1] != NULL && argv[2] == NULL )
    {
        unicast_num=DISABLE;
        broadcast_num=DISABLE;
    }
    else if (argv[0] != NULL && argv[1] == NULL && argv[2] != NULL )
    {
        unicast_num=DISABLE;
        multicast_num=DISABLE;
    }
    else if (argv[0] != NULL && argv[1] == NULL && argv[2] == NULL )
    {
        unicast_num=DISABLE;
    }
    else if (argv[0] == NULL && argv[1] != NULL && argv[2] != NULL )
    {
        broadcast_num=DISABLE;
        multicast_num=DISABLE;
    }
    else if (argv[0] == NULL && argv[1] != NULL && argv[2] == NULL )
    {
        broadcast_num=DISABLE;
    }
    else if (argv[0] == NULL && argv[1] == NULL && argv[2] != NULL )
    {
        multicast_num=DISABLE;
    }
    else if (argv[0] == NULL && argv[1] == NULL && argv[2] == NULL )
    {
        unicast_num= DISABLE;
        broadcast_num= DISABLE;
        multicast_num=DISABLE;
    }

    if((unicast_num==vlan_table[vlanid]->vlan.storm_unicast)&&
            (broadcast_num==vlan_table[vlanid]->vlan.storm_broadcast)&&
            (multicast_num==vlan_table[vlanid]->vlan.storm_multicast))
    {
        return CMD_SUCCESS;
    }

    VLAN_LOG_DBG("[%d]:%s:set unicast=%s,broadcast=%s,multicast=%s\n",__LINE__,__FUNCTION__,
               vlan_a_storm_suppress[unicast_num]	,vlan_a_storm_suppress[broadcast_num],vlan_a_storm_suppress[multicast_num]);
    if(vlan_set_info(vlanid,vlanid,&unicast_num,&broadcast_num,&multicast_num,VLAN_INFO_STORM_SUPRESS))
    {
        vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    VLAN_LOG_DBG("%s[%d]:%s:vlanid: %d; no storm  suppress success\n",__FILE__,__LINE__,__FUNCTION__,vlanid);
    return CMD_SUCCESS;
}

DEFUN(vlan_storm_supress,
      vlan_storm_supress_cmd,
      "storm suppress {unicast | broadcast | multicast}",
      "Broadcast storm\n"
      "Storm-control\n"
      "Unicast\n"
      "Broadcast\n"
      "Multicast\n")
{
    uint32_t vlanid = 0;
    uint8_t unicast_num;
    uint8_t broadcast_num;
    uint8_t multicast_num;

    vlanid = (uint32_t)vty->index;
    unicast_num=vlan_table[vlanid]->vlan.storm_unicast;
    broadcast_num=vlan_table[vlanid]->vlan.storm_broadcast;
    multicast_num=vlan_table[vlanid]->vlan.storm_multicast;

    if (argv[0] != NULL && argv[1] != NULL && argv[2] != NULL )
    {
        unicast_num=ENABLE;
        broadcast_num=ENABLE;
        multicast_num=ENABLE;
    }
    else if (argv[0] != NULL && argv[1] != NULL && argv[2] == NULL )
    {
        unicast_num=ENABLE;
        broadcast_num=ENABLE;
    }
    else if (argv[0] != NULL && argv[1] == NULL && argv[2] != NULL )
    {
        unicast_num=ENABLE;
        multicast_num=ENABLE;
    }
    else if (argv[0] != NULL && argv[1] == NULL && argv[2] == NULL )
    {
        unicast_num=ENABLE;
    }
    else if (argv[0] == NULL && argv[1] != NULL && argv[2] != NULL )
    {
        broadcast_num=ENABLE;
        multicast_num=ENABLE;
    }
    else if (argv[0] == NULL && argv[1] != NULL && argv[2] == NULL )
    {
        broadcast_num=ENABLE;
    }
    else if (argv[0] == NULL && argv[1] == NULL && argv[2] != NULL )
    {
        multicast_num=ENABLE;
    }
    else if (argv[0] == NULL && argv[1] == NULL && argv[2] == NULL )
    {
        unicast_num=ENABLE;
        broadcast_num=ENABLE;
        multicast_num=ENABLE;
    }

    if((unicast_num==vlan_table[vlanid]->vlan.storm_unicast)&&
            (broadcast_num==vlan_table[vlanid]->vlan.storm_broadcast)&&
            (multicast_num==vlan_table[vlanid]->vlan.storm_multicast))
    {
        return CMD_SUCCESS;
    }

    VLAN_LOG_DBG("[%d]:%s:set unicast=%s,broadcast=%s,multicast=%s\n",__LINE__,__FUNCTION__,
               vlan_a_storm_suppress[unicast_num]	,vlan_a_storm_suppress[broadcast_num],vlan_a_storm_suppress[multicast_num]);
    if(vlan_set_info(vlanid,vlanid,&unicast_num,&broadcast_num,&multicast_num,VLAN_INFO_STORM_SUPRESS))
    {
        vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    VLAN_LOG_DBG("%s[%d]:%s:vlanid: %d;storm suppress success\n",__FILE__,__LINE__,__FUNCTION__,vlanid);
    return CMD_SUCCESS;
}

DEFUN(vlan_range_set,
	 vlan_range_set_cmd,
	 "vlan-range <1-8> VLAN_STRING",
	 "Vlan node\n"
	 "Vlan_STRING segment,1:1-512;2:513-1024...etc\n"
	 "VLAN_STRING,use string to set vlan(eg. F:vlan 1 to 4)\n"
	)
{
	int segment = 0;
	int ret = 0;
	int string_len = 0,offset = 0;
	int i = 0;
	char *vlan_string = NULL;
	int length = 0;
	int vlanid_str = 0;
	int vlanid = 0;
	char ch;
	int vlan_start = 0;
	char *endptr = NULL; 

	segment = atoi(argv[0]);
	vlan_string = ( char * ) argv[1];
	length = strlen(vlan_string);

	if( length > 128 )
	{
		vty_error_out(vty, "%s[%d]:vlan_string too long! %s", __FUNCTION__, __LINE__, VTY_NEWLINE);
		return CMD_WARNING;
	}

	if( segment == 1 )
		vlan_start = 1;
	else if( segment == 2 )
		vlan_start = 513;
	else if( segment == 3 )
		vlan_start = 1025;
	else if( segment == 4 )
		vlan_start = 1537;
	else if( segment == 5 )
		vlan_start = 2049;
	else if( segment == 6 )
		vlan_start = 2561;
	else if( segment == 7 )
		vlan_start = 3073;
	else if( segment == 8 )
		vlan_start = 3585;
	else
	{
		vty_error_out(vty, "%s[%d]:unkown segment :%d %s", __FUNCTION__, __LINE__, segment, VTY_NEWLINE);
		return CMD_WARNING;
	}

	for( string_len=0; string_len<length; string_len++ )
	{
		ch = vlan_string[string_len];
		vlanid_str = strtoul (&ch, &endptr, 16);
		for( offset=0; offset<4; offset++ )
		{
			if(!(vlanid_str & (0x8 >> offset)))
				continue;
			vlanid = string_len*4 + offset + vlan_start;
			
			if( vlanid > (vlan_start + 512) || vlanid > 4094 )
			{
				vty_error_out(vty, "%s[%d]:vlanid overflow, %d! %s", __FUNCTION__, __LINE__, vlanid, VTY_NEWLINE);
				return CMD_WARNING;
			}

			if( devtype == ID_HT157 || devtype == ID_HT158 )
			{
				for( i=0; i<3; i++ )
				{
					if( vlanid == trunk_reserve_vlan_table[i].reserve_vlan )
					{
						vty_error_out (vty, "Vlan %d is a reserve vlan,can not be use.if you want to use this vlan,please change reserve vlan by 'reserve-vlan <1-4094> in mode switch' !%s", vlanid, VTY_NEWLINE);
						return CMD_WARNING;
					}
				}

				for( i=0; i<6; i++ )
				{
					if( vlanid == reserve_vlan_table[i] )
					{
						vty_error_out (vty, "Vlan %d is a reserve vlan,can not be use.if you want to use this vlan,please change reserve vlan by 'reserve-vlan <1-4094> in mode switch' !%s", vlanid, VTY_NEWLINE);
						return CMD_WARNING;
					}
				}
			}

			if(DISABLE == vlan_table[vlanid]->vlan.enable)
			{
				ret = vlan_create(vlanid,vlanid);
				if(ret < 0)
				{
					VLAN_LOG_DBG("%s[%d]:%s:create vlan error\n",__FILE__,__LINE__,__FUNCTION__);
					vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
					return CMD_WARNING;
				}
			}			
		}
	}

	return CMD_SUCCESS;
}

DEFUN(no_vlan_range_set,
	no_vlan_range_set_cmd,
	"no vlan-range <1-8> VLAN_STRING",
	"No command\n"
	"Vlan node\n"
	"Vlan_STRING segment,1:1-512;2:513-1024...etc\n"	
	"VLAN_STRING,use string to set vlan(eg. F:vlan 1 to 4)\n"
  )
{
	int segment = 0;
	int ret = 0;
	int string_len = 0,offset = 0;
	int i = 0;
	char *vlan_string = NULL;
	int length = 0;
	int vlanid_str = 0;
	int vlanid = 0;
	char ch;
	char *endptr = NULL; 
	int vlan_start = 0;
    uint8_t enable_flag = 0;	

	segment = atoi(argv[0]);
	vlan_string = ( char * ) argv[1];
	length = strlen(vlan_string);

	if( length > 128 )
	{
		vty_error_out(vty, "%s[%d]:vlan_string too long! %s", __FUNCTION__, __LINE__, VTY_NEWLINE);
		return CMD_WARNING;
	}

	if( segment == 1 )
		vlan_start = 1;
	else if( segment == 2 )
		vlan_start = 513;
	else if( segment == 3 )
		vlan_start = 1025;
	else if( segment == 4 )
		vlan_start = 1537;
	else if( segment == 5 )
		vlan_start = 2049;
	else if( segment == 6 )
		vlan_start = 2561;
	else if( segment == 7 )
		vlan_start = 3073;
	else if( segment == 8 )
		vlan_start = 3585;
	else
		zlog_err("%s[%d]:unkown segment :%d\n",__FUNCTION__,__LINE__,segment);

	for( string_len=0; string_len<length; string_len++ )
	{
		ch = vlan_string[string_len];
		vlanid_str = strtoul (&ch, &endptr, 16);
		for( offset=0; offset<4; offset++ )
		{
			if(!(vlanid_str & (0x8 >> offset)))
				continue;
			
			vlanid = string_len*4 + offset + vlan_start;
			if(ENABLE == vlan_table[vlanid]->vlan.enable)
	        {
	            enable_flag = 1;
	        }

			if( vlanid > (vlan_start + 512)  || vlanid > 4094 )
			{
				vty_error_out(vty, "%s[%d]:vlanid overflow, %d! %s", __FUNCTION__, __LINE__, vlanid, VTY_NEWLINE);
				return CMD_WARNING;
			}

			if( devtype == ID_HT157 || devtype == ID_HT158 )
			{
		        for( i=0; i<3; i++ )
			    {
			        if( vlanid == trunk_reserve_vlan_table[i].reserve_vlan )
			        {
			            vty_error_out (vty, "Vlan %d is a reserve vlan,can not be use.if you want to use this vlan,please change reserve vlan by 'reserve-vlan <1-4094> in mode switch' !%s", vlanid, VTY_NEWLINE);
			            return CMD_WARNING;
			        }
			    }
		        for( i=0; i<6; i++ )
		        {
		            if( vlanid == reserve_vlan_table[i] )
		            {
		                vty_error_out (vty, "Vlan %d is a reserve vlan,can not be use.if you want to use this vlan,please change reserve vlan by 'reserve-vlan <1-4094> in mode switch' !%s",vlanid, VTY_NEWLINE);
		                return CMD_WARNING;
		            }
		        }
			}
			
	        if(0 == enable_flag)   //全部未创建
	        {
	            vty_error_out (vty, "Delete vlan error,vlan %d is not exist%s",vlanid, VTY_NEWLINE);
	            return CMD_WARNING;
	        }
		    else
		    {
		        ret = vlan_delete(vlanid,vlanid);
		        if(ret < 0)
		        {
		            VLAN_LOG_DBG("%s[%d]:%s:delete vlan error\n",__FILE__,__LINE__,__FUNCTION__);
		            vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
		            return CMD_WARNING;
		        }
		        VLAN_LOG_DBG("%s[%d]:%s:delete vlan<%d-%d>\n",__FILE__,__LINE__,__FUNCTION__,vlanid,vlanid);
		    }
	    }		
	}

	return CMD_SUCCESS;
}


static struct cmd_node vlan_node =
{
    VLAN_NODE,
    "%s(config-vlan)# ",
    1 /* VTYSH */
};


void vlan_cli_init (void)
{
    install_node (&vlan_node, vlan_config_write);
    install_default(VLAN_NODE);

    install_element(CONFIG_NODE, &vlan_get_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &show_vlan_summary_cmd, CMD_LOCAL);
    install_element (CONFIG_NODE, &no_vlan_get_range_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &vlan_get_range_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &show_vlan_cmd, CMD_LOCAL);
    install_element (VLAN_NODE, &vlan_name_cmd, CMD_SYNC);
    install_element (VLAN_NODE, &vlan_mac_learn_cmd, CMD_SYNC);
    install_element (VLAN_NODE, &vlan_mac_limit_cmd, CMD_SYNC);
    install_element (VLAN_NODE, &no_vlan_mac_limit_cmd, CMD_SYNC);
    install_element (VLAN_NODE, &no_vlan_name_cmd, CMD_SYNC);
    install_element (VLAN_NODE, &vlan_storm_supress_cmd, CMD_SYNC);
    install_element (VLAN_NODE, &no_vlan_storm_supress_cmd, CMD_SYNC);

	install_element(CONFIG_NODE, &vlan_range_set_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &no_vlan_range_set_cmd, CMD_SYNC);
}

