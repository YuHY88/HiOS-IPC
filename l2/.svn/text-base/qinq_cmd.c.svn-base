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
#include <lib/devm_com.h>
#include <lib/log.h>
#include <syslog/syslogd.h>
#include "vlan.h"
#include "qinq.h"
#include "l2_if.h"



int check_input_in_vlanlist(struct l2if *p_l2if,uint16_t vlanid)
{
	struct listnode *p_listnode = NULL;
	void *pdata = NULL;

	QINQ_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __FUNCTION__);
	for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.vlan_list, p_listnode, pdata))
	{
		if((uint32_t)pdata == vlanid)
		{
			QINQ_LOG_DBG("%s[%d]:%s: find vlanid in vlanlist\n", __FILE__, __LINE__, __FUNCTION__);
			return ERRNO_SUCCESS;
		}
	}
	return ERRNO_FAIL;
}

void qinq_device_type_init(void)
{
	int ret=0;
	ret=devm_comm_get_id(1, 0, MODULE_ID_L2, &devtype );
	if(ret)
	{
		QINQ_LOG_DBG("%s[%d]:%s:devm_comm_get_id failed\n",__FILE__,__LINE__, __FUNCTION__);
	}
	else
	{
		printf("qinq get devtype %x\n", devtype);
	}	
}

DEFUN(no_dot1q_tunnel_svlan,
	no_dot1q_tunnel_svlan_cmd,
	"no dot1q-tunnel",
	"No command\n"
	"Dot1q-tunnel rule\n" 
)
{
	uint32_t ifindex = 0;
	int ret = 0;
	struct l2if *p_l2if = NULL;
	uint8_t mode = 0;
	
	/*get ifindex from interface we use*/
	ifindex = (uint32_t)vty->index;
	
	/*检查mode是否是switch*/
	if(ifm_get_mode(ifindex,MODULE_ID_L2, &mode) != 0)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to get mode from ifm\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Fail to get interface mode%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if((uint8_t)IFNET_MODE_SWITCH != mode)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s,mode != IFNET_MODE_SWITCH\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	/*在哈希表搜寻ifindex对应节点，如果没有则返回ok*/
	p_l2if = l2if_lookup(ifindex);
	if(NULL == p_l2if)
	{
		vty_error_out (vty, "%%No dot1q-tunnel error,lookup failed%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(DEFAULT_VLAN_ID != p_l2if->switch_info.dot1q_tunnel)
	{
		ret = dot1q_tunnel_delete(p_l2if);
		if(ret != ERRNO_SUCCESS)
		{
			QINQ_LOG_DBG("%s[%d]:%s:no dot1q tunnel failed\n",__FILE__,__LINE__, __FUNCTION__);
        	vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	else
	{
		vty_error_out (vty, "No dot1q tunnel failed,dot1q tunnel already empty%s",VTY_NEWLINE);
		QINQ_LOG_DBG("%s[%d]:%s:no dot1q tunnel sucessfully\n",__FILE__,__LINE__,__FUNCTION__);
	}
	return CMD_SUCCESS;
}

DEFUN(dot1q_tunnel_svlan,
	dot1q_tunnel_svlan_cmd,
	"dot1q-tunnel svlan <1-4094> cos <0-7>",
	"Dot1q-tunnel rule\n"
	"Svlan_new\n" 
	"Svlan_newid<1-4094>\n"
	"Set cos\n"
	"Cos <0-7>\n"
)
{
	uint16_t svlan_id = 0;
	uint16_t cos_id = 0;
	uint32_t ifindex = 0;
	int ret = 0;
	int i,port = 0;
	uint8_t flag = 0;
	uint8_t mode = 0;
	struct l2if *p_l2if = NULL;
	
	/*get ifindex from interface we use*/	
	ifindex = (uint32_t)vty->index;
	
	/*检查mode是否是switch*/
	if(ifm_get_mode(ifindex,MODULE_ID_L2, &mode) != 0)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to get mode from ifm\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Fail to get interface mode%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if((uint8_t)IFNET_MODE_SWITCH != mode)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s,mode != IFNET_MODE_SWITCH\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	/*在哈希表搜寻ifindex对应节点，如果没有则创建*/
	p_l2if = l2if_get(ifindex);
	if(NULL == p_l2if)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to CALLOC if l2 table bucket\n",__FILE__,__LINE__,__FUNCTION__);
		return CMD_WARNING;
	}
	
	/*switch mode 限制*/
	if(SWITCH_MODE_ACCESS == p_l2if->switch_info.mode)
	{
		vty_error_out (vty, "Switch mode error: must be hybrid or trunk%s", VTY_NEWLINE);
		QINQ_LOG_DBG("%s[%d]:leave %s:error:switch mode must be hybrid or trunk\n",__FILE__,__LINE__,__FUNCTION__);
		return CMD_WARNING;
	}
	
	/*检查access vlanid，vlan list，qinq list*/
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
						QINQ_LOG_DBG("%s[%d]:%s:error:fail to create dot1q tunnel:access vlanid != %d\n",__FILE__,__LINE__, __FUNCTION__,DEFAULT_VLAN_ID);
						vty_error_out (vty, "Fail to create dot1q tunnel:Please delete access vlan first.%s", VTY_NEWLINE);
						flag = 1;
					}
				}
			}
		}
		else
		{
		    port = IFM_PORT_ID_GET ( ifindex );
		    if( port < 1 || port > 6 )
		    {
		        QINQ_LOG_DBG ("Invalid port number!\n");
		        return -1;
		    }
			if((DEF_RESV_VLAN1 + port - 1) != p_l2if->switch_info.access_vlan)
			{
				QINQ_LOG_DBG("%s[%d]:%s:error:fail to create dot1q tunnel:access vlanid != %d\n",__FILE__,__LINE__, __FUNCTION__,DEFAULT_VLAN_ID);
				vty_error_out (vty, "Fail to create dot1q tunnel:Please delete access vlan first.%s", VTY_NEWLINE);
				flag = 1;
			}
		}
	}
	else
	{
		if(DEFAULT_VLAN_ID != p_l2if->switch_info.access_vlan)
		{
			QINQ_LOG_DBG("%s[%d]:%s:error:fail to create dot1q tunnel:access vlanid != %d\n",__FILE__,__LINE__, __FUNCTION__,DEFAULT_VLAN_ID);
			vty_error_out (vty, "Fail to create dot1q tunnel:Please delete access vlan first.%s", VTY_NEWLINE);
			flag = 1;
		}
	}
	if(NULL != p_l2if->switch_info.vlan_list->head)
	{
		QINQ_LOG_DBG("%s[%d]:%s:error:fail to create dot1q tunnel:vlan list != NULL\n",__FILE__,__LINE__, __FUNCTION__);
		vty_error_out (vty, "Fail to create dot1q tunnel:Please delete switch vlan first.%s", VTY_NEWLINE);
		flag = 1;
	}
	if(NULL != p_l2if->switch_info.qinq_list->head)
	{
		QINQ_LOG_DBG("%s[%d]:%s:error:fail to create dot1q tunnel:qinq_list != NULL\n",__FILE__,__LINE__, __FUNCTION__);
		vty_error_out (vty, "Fail to create dot1q tunnel:Please delete vlantranslate first.%s", VTY_NEWLINE);
		flag = 1;
	}
	if(1 == flag)
		return CMD_WARNING;
	
	svlan_id = atoi(argv[0]);
	cos_id = atoi(argv[1]);

	if(DISABLE==vlan_table[svlan_id]->vlan.enable)
	{
		vty_error_out (vty, "Vlanid = %d is not created%s",svlan_id, VTY_NEWLINE);	
		return CMD_WARNING;
	}
	
	ret = dot1q_tunnel_add(p_l2if, svlan_id, cos_id);
	if(ret != ERRNO_SUCCESS)
	{
		QINQ_LOG_DBG("%s[%d]:%s:fail to set dot1q tunnel\n",__FILE__,__LINE__,__FUNCTION__);
        vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	QINQ_LOG_DBG("%s[%d]:%s:vlan-mapping dot1q-tunnel svlan = %d,cos = %d\n",__FILE__,__LINE__,__FUNCTION__,svlan_id,cos_id);

	return CMD_SUCCESS;
}

DEFUN(no_vlan_mapping_qinq,
	no_vlan_mapping_qinq_cmd,
	"no vlan-mapping qinq svlan <1-4094> cvlan <1-4094> {to <1-4094>}",
	"No command \n"
	"Vlan-mapping\n"
	"Qinq rule\n"
	"Svlan\n"
	"Svlanid<1-4094>\n"
	"Cvlan\n"
	"Cvlanid start <1-4094>\n"
	"To\n"
	"Cvlanid end <1-4094>\n"
)
{
	uint32_t ifindex = 0;
	int ret = 0;
	struct vlan_mapping info;
	struct l2if *p_l2if = NULL;
	struct listnode *p_listnode = NULL;
	struct vlan_mapping *p_vlan_mapping = NULL;
	uint8_t mode = 0;

	/*get ifindex from interface we use*/	
	ifindex = (uint32_t)vty->index;
	
	/*检查mode是否是switch*/
	if(ifm_get_mode(ifindex,MODULE_ID_L2, &mode) != 0)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to get mode from ifm\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Fail to get interface mode%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if((uint8_t)IFNET_MODE_SWITCH != mode)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s,mode != IFNET_MODE_SWITCH\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	/*在哈希表搜寻ifindex对应节点，如果没有则返回ok*/
	p_l2if = l2if_lookup(ifindex);
	if(NULL == p_l2if)
	{
		QINQ_LOG_DBG("%s[%d]:%s:invalid ifindex,do not need to set no qinq\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "No vlan-mapping error,lookup failed%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	memset(&info, 0, sizeof(struct vlan_mapping));
	info.svlan.vlan_start = atoi(argv[0]);
	info.cvlan.vlan_start = atoi(argv[1]);

	if(NULL != argv[2])
		info.cvlan.vlan_end = atoi(argv[2]);
	else
		info.cvlan.vlan_end = info.cvlan.vlan_start;
	
	if(info.cvlan.vlan_end < info.cvlan.vlan_start)
	{
		QINQ_LOG_DBG("%s[%d]:%s:,vlanid_end = %d must not less than vlanid_start = %d\n",__FILE__,__LINE__,__FUNCTION__, info.cvlan.vlan_end, info.cvlan.vlan_start);
		vty_error_out (vty, "Vlan input error,vlanid_end(%d) must not less than vlanid_start(%d) !%s", info.cvlan.vlan_end, info.cvlan.vlan_start, VTY_NEWLINE);
		return CMD_WARNING;
	}

	info.svlan.vlan_end= info.svlan.vlan_start;
	info.cvlan_cos = 8;
	info.svlan_cos = 8;

	for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.qinq_list, p_listnode, p_vlan_mapping))
	{
		if(info.cvlan.vlan_start == p_vlan_mapping->cvlan.vlan_start
			&& info.cvlan.vlan_end== p_vlan_mapping->cvlan.vlan_end
			&& info.svlan.vlan_start == p_vlan_mapping->svlan.vlan_start
			&& info.svlan.vlan_end== p_vlan_mapping->svlan.vlan_end)
		{
			ret = qinq_delete(p_l2if, p_vlan_mapping, 100, 0);
			if(ERRNO_FAIL == ret)
			{
				QINQ_LOG_DBG("%s[%d]:%s:no vlan-mapping qinq svlan %d cvlan<%d-%d> failed\n",__FILE__,__LINE__,__FUNCTION__,info.svlan.vlan_start, info.cvlan.vlan_start, info.cvlan.vlan_end);
				if(NULL == argv[2])
					vty_error_out (vty, "No vlan-mapping qinq svlan %d cvlan %d error,hal return failed.%s", info.svlan.vlan_start, info.cvlan.vlan_start, VTY_NEWLINE);
				else
					vty_error_out (vty, "No vlan-mapping qinq svlan %d cvlan %d to %d error,hal return failed.%s", info.svlan.vlan_start, info.cvlan.vlan_start, info.cvlan.vlan_end,VTY_NEWLINE);
				return CMD_WARNING;
			}
			else
			{
				XFREE(MTYPE_L2_VLAN, p_vlan_mapping);
				list_delete_node (p_l2if->switch_info.qinq_list, p_listnode);
				QINQ_LOG_DBG("%s[%d]:%s:sucessfully delete svlan %d cvlan<%d-%d> dot1q \n",__FILE__,__LINE__,__FUNCTION__,info.svlan.vlan_start, info.cvlan.vlan_start, info.cvlan.vlan_end);
				return CMD_SUCCESS;
			}
		}
	}

	QINQ_LOG_DBG("%s[%d]:%s:no vlan-mapping qinq failed,can not find qinq svlan %d cvlan<%d-%d>\n",__FILE__,__LINE__,__FUNCTION__, info.svlan.vlan_start, info.cvlan.vlan_start, info.cvlan.vlan_end);
	if(NULL == argv[2])
		vty_error_out (vty, "No vlan-mapping qinq svlan %d cvlan %d,lookup failed%s", info.svlan.vlan_start, info.cvlan.vlan_start, VTY_NEWLINE);
	else
		vty_error_out (vty, "No vlan-mapping qinq svlan %d cvlan %d to %d,lookup failed%s", info.svlan.vlan_start, info.cvlan.vlan_start, info.cvlan.vlan_end,VTY_NEWLINE);

	return CMD_WARNING;

}


DEFUN(vlan_mapping_qinq_cvlans_delete_svlan,
	vlan_mapping_qinq_cvlans_delete_svlan_cmd,
	"vlan-mapping qinq svlan <1-4094> cvlan <1-4094> {to <1-4094>} delete svlan",
	"Vlan-mapping rule\n"
	"Qinq rule\n" 
	"Svlan\n"
	"Svlanid <1-4094>\n"
	"Cvlan\n"
	"Cvlanid start <1-4094>\n"
	"To\n"
	"Tvlanid end <1-4094>\n"
	"Delete command\n"
	"Svlan\n"
)
{
	uint32_t ifindex = 0;
	int ret = 0;
	struct vlan_mapping info;
	uint8_t mode = 0;	
	struct l2if *p_l2if = NULL;
	struct listnode *p_listnode = NULL;
	struct vlan_mapping *p_vlan_mapping = NULL;
	
	/*get ifindex from interface we use*/	
	ifindex = (uint32_t)vty->index;
	
	/*检查mode是否是switch*/
	if(ifm_get_mode(ifindex,MODULE_ID_L2, &mode) != 0)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to get mode from ifm\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Fail to get interface mode%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if((uint8_t)IFNET_MODE_SWITCH != mode)
	{
		vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	/*在哈希表搜寻ifindex对应节点，如果没有则创建*/
	p_l2if = l2if_get(ifindex);
	if(NULL == p_l2if)
	{
		vty_error_out (vty, "Fail to CALLOC if l2 table bucket %s", VTY_NEWLINE);
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to CALLOC if l2 table bucket\n",__FILE__,__LINE__,__FUNCTION__);
		return CMD_WARNING;
	}
	
	/*检查switch mode*/
	if(p_l2if->switch_info.mode == SWITCH_MODE_ACCESS)
	{
		vty_error_out (vty, "Switch mode error: must be hybrid or trunk%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	/*检查dot1q tunnel是否为空*/
	if(DEFAULT_VLAN_ID != p_l2if->switch_info.dot1q_tunnel)
	{
		vty_error_out (vty, "Set vlan-mapping error: please delete dot1q_tunnel first. %s",VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	memset(&info, 0, sizeof(struct vlan_mapping));
	info.svlan.vlan_start = atoi(argv[0]);
	info.cvlan.vlan_start = atoi(argv[1]);

	if(NULL != argv[2])
	{
		info.cvlan.vlan_end = atoi(argv[2]);
		if(info.cvlan.vlan_end <= info.cvlan.vlan_start)
		{
			QINQ_LOG_DBG("%s[%d]:,vlan input error,vlanid_end(%d) must more than vlanid_start(%d) !\n",__FILE__,__LINE__, info.cvlan.vlan_end, info.cvlan.vlan_start);
			vty_error_out (vty, "Vlan input error,vlanid_end(%d) must more than vlanid_start(%d) !%s", info.cvlan.vlan_end, info.cvlan.vlan_start, VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	else
	{
		info.cvlan.vlan_end = info.cvlan.vlan_start;
	}
	
	info.svlan.vlan_end = info.svlan.vlan_start;
	info.svlan_new.vlan_start = info.cvlan.vlan_start;
	info.svlan_new.vlan_end = info.cvlan.vlan_end;
	info.cvlan_act = VLAN_ACTION_NO;
	info.svlan_act = VLAN_ACTION_DELETE;
	info.cvlan_cos = 8;
	info.svlan_cos = 8;

	/*check if clan is used*/
	for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.qinq_list, p_listnode, p_vlan_mapping))
	{
		if(qinq_check_vlan_range(&(p_vlan_mapping->cvlan), &(info.cvlan)))
		{
			if(qinq_check_vlan_range(&(p_vlan_mapping->svlan), &(info.svlan)))
			{
				vty_error_out (vty, "Input error: svlan<%d-%d> cvlan<%d-%d> has member already be used%s",info.svlan.vlan_start,info.svlan.vlan_end,info.cvlan.vlan_start,info.cvlan.vlan_end,VTY_NEWLINE);
				QINQ_LOG_DBG("%s[%d]:leave %s:error:cvlan<%d-%d> svlan<%d-%d> has vlan used\n",__FILE__,__LINE__,__FUNCTION__,info.cvlan.vlan_start,info.cvlan.vlan_end,info.svlan.vlan_start,info.svlan.vlan_end);
				return CMD_WARNING;
			}
		}
		if(qinq_check_vlan_range(&(p_vlan_mapping->cvlan_new), &(info.cvlan_new)))
		{
			if(qinq_check_vlan_range(&(p_vlan_mapping->svlan_new),&(info.svlan_new)))
			{
				vty_error_out (vty, "Input error:svlan_new<%d-%d> has member already be used%s",info.svlan_new.vlan_start,info.svlan_new.vlan_end,VTY_NEWLINE);
				QINQ_LOG_DBG("%s[%d]:leave %s:error:svlan_new<%d-%d> has vlan used\n",__FILE__,__LINE__,__FUNCTION__,info.svlan_new.vlan_start,info.svlan_new.vlan_end);
				return CMD_WARNING;
			}
		}
	}

	if( devtype == ID_HT157 || devtype == ID_HT158 )
	{
		ret = qinq_add(p_l2if, &info, 5, 0);
	}
	else 
	{
		ret = qinq_add(p_l2if, &info, 100, 0);
	}
	
	if(ret != ERRNO_SUCCESS)
	{
		if(ret==ERRNO_OVERSIZE)
		{
			/*for print configured conversion*/
			for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.qinq_list, p_listnode, p_vlan_mapping))
			{  
				if(info.svlan_new.vlan_start==p_vlan_mapping->svlan_new.vlan_start)
				{    
					if(info.cvlan_new.vlan_start==p_vlan_mapping->cvlan_new.vlan_start)
					{ 
						if(NULL != argv[2])
						{
							vty_error_out (vty, "Vlan-mapping: chip resource is full,partial translate configured:vlan-mapping qinq svlan %d cvlan %d to %d delete svlan%s",
								p_vlan_mapping->svlan.vlan_start,p_vlan_mapping->cvlan.vlan_start,p_vlan_mapping->cvlan.vlan_end,VTY_NEWLINE);
						}
						else 
						{
							vty_error_out (vty, "Vlan-mapping: chip resource is full,partial translate configured:vlan-mapping qinq svlan %d cvlan %d delete svlan%s",
								p_vlan_mapping->svlan.vlan_start,p_vlan_mapping->cvlan.vlan_start,VTY_NEWLINE);
						}
						return CMD_SUCCESS;
					}
				}
			}
			vty_error_out (vty, "Fail to set vlan-mapping:chip resource is full.%s", VTY_NEWLINE);
		}
		else
		{
			QINQ_LOG_DBG("%s[%d]:%s:fail to set vlan-mapping\n",__FILE__,__LINE__, __FUNCTION__);
        	vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
		}
	    return CMD_WARNING;
	}


	return CMD_SUCCESS;
}


DEFUN(vlan_mapping_qinq_scvlans_to_svlan,
	vlan_mapping_qinq_scvlans_to_svlan_cmd,
	"vlan-mapping qinq svlan <1-4094> cvlan <1-4094> to <1-4094> translate svlan  <1-4094> {cos <0-7>} cvlan * ",
	"Vlan-mapping rule\n"
	"Qinq rule\n" 
	"Svlan\n"
	"Svlanid<1-4094>\n"
	"Cvlan\n"
	"Cvlanid start <1-4094>\n"
	"To\n"
	"Cvlanid end <1-4094>\n"
	"Translate command\n"
	"Svlan_new\n"
	"Svlan_newid<1-4094>\n"
	"Cos of svlan_new\n"
	"Cos<0-7>\n"
	"Clvan_new\n"
	"Cvlan_newid not change\n"
)
{
	uint32_t ifindex = 0;
	int ret = 0;
	struct vlan_mapping info;
	uint8_t mode = 0;
	struct l2if *p_l2if = NULL;
	struct listnode *p_listnode = NULL;
	struct vlan_mapping *p_vlan_mapping = NULL;
	
	/*get ifindex from interface we use*/	
	ifindex = (uint32_t)vty->index;
	
	/*检查mode是否是switch*/
	if(ifm_get_mode(ifindex,MODULE_ID_L2, &mode) != 0)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to get mode from ifm\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Fail to get interface mode%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if((uint8_t)IFNET_MODE_SWITCH != mode)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s,mode != IFNET_MODE_SWITCH\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*在哈希表搜寻ifindex对应节点，如果没有则创建*/
	p_l2if = l2if_get(ifindex);
	if(NULL == p_l2if)
	{
		vty_error_out (vty, "Fail to CALLOC if l2 table bucket %s", VTY_NEWLINE);
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to CALLOC if l2 table bucket\n",__FILE__,__LINE__,__FUNCTION__);
		return CMD_WARNING;
	}
	
	/*检查switch mode*/
	if(p_l2if->switch_info.mode == SWITCH_MODE_ACCESS)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:mode must be hybrid or trunk\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Switch mode error: must be hybrid or trunk%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	/*检查dot1q tunnel是否为空*/
	if(DEFAULT_VLAN_ID != p_l2if->switch_info.dot1q_tunnel)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error: dot1q tunnel != 0\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Set vlan-mapping error: Please delete dot1q_tunnel first. %s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*get input PRM from cli*/
	memset(&info, 0, sizeof(struct vlan_mapping));
	info.svlan.vlan_start = atoi(argv[0]);
	info.cvlan.vlan_start = atoi(argv[1]);
	info.cvlan.vlan_end = atoi(argv[2]);

	if(info.cvlan.vlan_end <= info.cvlan.vlan_start)
	{
		QINQ_LOG_DBG("%s[%d]:,vlan input error,vlanid_end(%d) must more than vlanid_start(%d) !\n",__FILE__,__LINE__, info.cvlan.vlan_end, info.cvlan.vlan_start);
		vty_error_out (vty, "Vlan input error,vlanid_end(%d) must more than vlanid_start(%d) !%s", info.cvlan.vlan_end, info.cvlan.vlan_start, VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	info.svlan_new.vlan_start = atoi(argv[3]);
	if(info.svlan.vlan_start == info.svlan_new.vlan_start)
	{
		QINQ_LOG_DBG("%s[%d]:,vlan input error,svlan_new(%d) must not equal to svlan(%d) !\n",__FILE__,__LINE__, info.svlan_new.vlan_start, info.svlan.vlan_start);
		vty_error_out (vty, "Vlan input error,svlan_new(%d) must not equal to svlan(%d) !%s", info.svlan_new.vlan_start, info.svlan.vlan_start, VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(NULL != argv[4])
		info.svlan_cos = atoi(argv[4]);
	else
		info.svlan_cos = 8;

	info.svlan.vlan_end= info.svlan.vlan_start;
	info.cvlan_new.vlan_start = info.cvlan.vlan_start;
	info.cvlan_new.vlan_end= info.cvlan.vlan_end;
	info.svlan_new.vlan_end= info.svlan_new.vlan_start;
	info.cvlan_cos = 8;
	info.svlan_act = VLAN_ACTION_TRANSLATE;
	info.cvlan_act= VLAN_ACTION_NO;
	
	/*check if cvlan/svlan is used*/
	for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.qinq_list, p_listnode, p_vlan_mapping))
	{
		if(qinq_check_vlan_range(&(p_vlan_mapping->cvlan), &(info.cvlan)))
		{
			if(qinq_check_vlan_range(&(p_vlan_mapping->svlan), &(info.svlan)))
			{
				vty_error_out (vty, "Input error:svlan<%d-%d> cvlan<%d-%d> has member already be used%s"
					,info.svlan.vlan_start,info.svlan.vlan_end,info.cvlan.vlan_start,info.cvlan.vlan_end,VTY_NEWLINE);
				QINQ_LOG_DBG("%s[%d]:leave %s:error:cvlan<%d-%d> svlan<%d-%d> has vlan used\n",__FILE__,__LINE__,__FUNCTION__
					,info.cvlan.vlan_start,info.cvlan.vlan_end,info.svlan.vlan_start,info.svlan.vlan_end);
				return CMD_WARNING;
			}
		}

		if(qinq_check_vlan_range(&(p_vlan_mapping->cvlan_new), &(info.cvlan_new)))
		{
			if(qinq_check_vlan_range(&(p_vlan_mapping->svlan_new), &(info.svlan_new)))
			{
				vty_error_out (vty, "Input error:svlan_new<%d-%d> cvlan_new<%d-%d> has member already be used%s"
					,info.svlan_new.vlan_start,info.svlan_new.vlan_end,info.cvlan_new.vlan_start,info.cvlan_new.vlan_end,VTY_NEWLINE);
				QINQ_LOG_DBG("%s[%d]:leave %s:error:cvlan_new<%d-%d> svlan_new<%d-%d> has vlan used\n",__FILE__,__LINE__,__FUNCTION__
					,info.cvlan_new.vlan_start,info.cvlan_new.vlan_end,info.svlan_new.vlan_start,info.svlan_new.vlan_end);
				return CMD_WARNING;
			}
		}
	}

	if( devtype == ID_HT157 || devtype == ID_HT158 )
	{
		ret = qinq_add(p_l2if, &info, 5, 0);
	}
	else 
	{
		ret = qinq_add(p_l2if, &info, 100, 0);
	}

	if(ret != ERRNO_SUCCESS)
	{
		if(ret==ERRNO_OVERSIZE)
		{
			/*for print configured conversion*/
			for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.qinq_list, p_listnode, p_vlan_mapping))
			{  
				if(info.svlan_new.vlan_start==p_vlan_mapping->svlan_new.vlan_start)
				{    
					if(info.cvlan_new.vlan_start==p_vlan_mapping->cvlan_new.vlan_start)
					{ 
						if(NULL != argv[4])
						{
							vty_error_out (vty, "Vlan-mapping: chip resource is full,partial translate configured:vlan-mapping qinq svlan %d cvlan %d to %d translate svlan %d cos %d cvlan *%s",
								p_vlan_mapping->svlan.vlan_start,p_vlan_mapping->cvlan.vlan_start,p_vlan_mapping->cvlan.vlan_end,
							p_vlan_mapping->svlan_new.vlan_start,p_vlan_mapping->svlan_cos,VTY_NEWLINE);
						}
						else 
						{
							vty_error_out (vty, "Vlan-mapping: chip resource is full,partial translate configured:vlan-mapping qinq svlan %d cvlan %d to %d translate svlan %d cvlan *%s",
								p_vlan_mapping->svlan.vlan_start,p_vlan_mapping->cvlan.vlan_start,p_vlan_mapping->cvlan.vlan_end,
							p_vlan_mapping->svlan_new.vlan_start,VTY_NEWLINE);
						}
						return CMD_SUCCESS;
					}
				}
			}
			vty_error_out (vty, "Fail to set vlan-mapping:chip resource is full.%s", VTY_NEWLINE);
		}
		else
		{
			QINQ_LOG_DBG("%s[%d]:%s:fail to set vlan-mapping\n",__FILE__,__LINE__, __FUNCTION__);
        	vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
		}
	    return CMD_WARNING;
	}


	QINQ_LOG_DBG("%s[%d]:%s:vlan-mapping qinq svlan = %d,cvlan<%d-%d> translate svlan = %d cos = %d cvlan *\n",__FILE__,__LINE__,__FUNCTION__, info.svlan.vlan_start, info.cvlan.vlan_start, info.cvlan.vlan_end, info.svlan_new.vlan_start, info.svlan_cos);

	return CMD_SUCCESS;
}


DEFUN(vlan_mapping_qinq_scvlan_to_scvlan_1,
	vlan_mapping_qinq_scvlan_to_scvlan_cmd_1,
	"vlan-mapping qinq svlan <1-4094> cvlan <1-4094> translate svlan <1-4094> cos <0-7> cvlan <1-4094> ",
	"Vlan-mapping rule\n"
	"Qinq rule\n" 
	"Svlan\n"
	"Svlanid<1-4094>\n"
	"Cvlan\n"
	"Cvlanid<1-4094>\n"
	"Translate command\n"
	"Svlan_new\n"
	"Svlan_newid<1-4094>\n"
	"Cos of svlan_new\n"
	"Cos<0-7>\n"
	"Cvlan_new\n"
	"Cvlan_newid<1-4094>\n"
)
{
	uint32_t ifindex = 0;
	int ret = 0;
	struct vlan_mapping info;
	uint8_t mode = 0;
	struct l2if *p_l2if = NULL;
	struct listnode *p_listnode = NULL;
	struct vlan_mapping *p_vlan_mapping = NULL;
	
	/*get ifindex from interface we use*/	
	ifindex = (uint32_t)vty->index;
	
	/*检查mode是否是switch*/
	if(ifm_get_mode(ifindex,MODULE_ID_L2, &mode) != 0)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to get mode from ifm\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Fail to get interface mode%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if((uint8_t)IFNET_MODE_SWITCH != mode)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s,mode != IFNET_MODE_SWITCH\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*在哈希表搜寻ifindex对应节点，如果没有则创建*/
	p_l2if = l2if_get(ifindex);
	if(NULL == p_l2if)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to CALLOC if l2 table bucket\n",__FILE__,__LINE__,__FUNCTION__);
		return CMD_WARNING;
	}
	
	/*检查switch mode*/
	if(p_l2if->switch_info.mode == SWITCH_MODE_ACCESS)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:mode must be hybrid or trunk\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Switch mode error: must be hybrid or trunk%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	/*检查dot1q tunnel是否为空*/
	if(DEFAULT_VLAN_ID != p_l2if->switch_info.dot1q_tunnel)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error: dot1q tunnel != 0\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Set vlan-mapping error: Please delete dot1q_tunnel first. %s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*get input PRM from cli*/
	memset(&info, 0, sizeof(struct vlan_mapping));
	info.svlan.vlan_start = atoi(argv[0]);	
	info.cvlan.vlan_start = atoi(argv[1]);
	info.svlan_new.vlan_start = atoi(argv[2]);
	info.svlan_cos = atoi(argv[3]);
	info.cvlan_new.vlan_start = atoi(argv[4]);

	info.cvlan.vlan_end= info.cvlan.vlan_start;
	info.svlan.vlan_end= info.svlan.vlan_start;
	info.cvlan_new.vlan_end= info.cvlan_new.vlan_start;
	info.svlan_new.vlan_end= info.svlan_new.vlan_start;
	info.cvlan_cos = 8;
	info.cvlan_act = VLAN_ACTION_TRANSLATE;
	info.svlan_act = VLAN_ACTION_TRANSLATE;
	
	if((info.svlan.vlan_start == info.svlan_new.vlan_start)&&(info.cvlan.vlan_start == info.cvlan_new.vlan_start))
	{
		QINQ_LOG_DBG("%s[%d]:vlan input error,No change after translate!\n",__FILE__,__LINE__);
		vty_error_out (vty, "Vlan input error,No change after translate!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	/*check if vlan is used*/
	for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.qinq_list, p_listnode, p_vlan_mapping))
	{
		if(qinq_check_vlan_range(&(p_vlan_mapping->cvlan), &(info.cvlan)))
		{
			if(qinq_check_vlan_range(&(p_vlan_mapping->svlan), &(info.svlan)))
			{
				vty_error_out (vty, "Input error:svlan<%d-%d> cvlan<%d-%d> has member already be used%s"
					,info.svlan.vlan_start,info.svlan.vlan_end,info.cvlan.vlan_start,info.cvlan.vlan_end,VTY_NEWLINE);
				QINQ_LOG_DBG("%s[%d]:leave %s:error:cvlan<%d-%d> svlan<%d-%d> has vlan used\n",__FILE__,__LINE__,__FUNCTION__
					,info.cvlan.vlan_start,info.cvlan.vlan_end,info.svlan.vlan_start,info.svlan.vlan_end);
				return CMD_WARNING;
			}
		}

		if(qinq_check_vlan_range(&(p_vlan_mapping->cvlan_new), &(info.cvlan_new)))
		{
			if(qinq_check_vlan_range(&(p_vlan_mapping->svlan_new), &(info.svlan_new)))
			{
				vty_error_out (vty, "Input error:svlan_new<%d-%d> cvlan_new<%d-%d> has member already be used%s"
					,info.svlan_new.vlan_start,info.svlan_new.vlan_end,info.cvlan_new.vlan_start,info.cvlan_new.vlan_end,VTY_NEWLINE);
				QINQ_LOG_DBG("%s[%d]:leave %s:error:cvlan_new<%d-%d> svlan_new<%d-%d> has vlan used\n",__FILE__,__LINE__,__FUNCTION__
					,info.cvlan_new.vlan_start,info.cvlan_new.vlan_end,info.svlan_new.vlan_start,info.svlan_new.vlan_end);
				return CMD_WARNING;
			}
		}
	}

	if( devtype == ID_HT157 || devtype == ID_HT158 )
	{
		ret = qinq_add(p_l2if, &info, 5, 0);
	}
	else 
	{
		ret = qinq_add(p_l2if, &info, 100, 0);
	}

	if(ret != ERRNO_SUCCESS)
	{
		if(ret==ERRNO_OVERSIZE)
		{
			/*for print configured conversion*/
			for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.qinq_list, p_listnode, p_vlan_mapping))
			{  
				if(info.svlan_new.vlan_start==p_vlan_mapping->svlan_new.vlan_start)
				{    
					if(info.cvlan_new.vlan_start==p_vlan_mapping->cvlan_new.vlan_start)
					{ 
						vty_error_out (vty, "Vlan-mapping: chip resource is full,partial translate configured:vlan-mapping qinq svlan %d cvlan %d translate svlan %d cos %d cvlan %d%s",
							p_vlan_mapping->svlan.vlan_start,p_vlan_mapping->cvlan.vlan_start,p_vlan_mapping->svlan_new.vlan_start,p_vlan_mapping->svlan_cos,
						p_vlan_mapping->cvlan_new.vlan_start,VTY_NEWLINE);
						return CMD_SUCCESS;
					}
				}
			}
			vty_error_out (vty, "Fail to set vlan-mapping:chip resource is full.%s", VTY_NEWLINE);
		}
		else
		{
			QINQ_LOG_DBG("%s[%d]:%s:fail to set vlan-mapping\n",__FILE__,__LINE__, __FUNCTION__);
        	vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
		}
	    return CMD_WARNING;
	}


	QINQ_LOG_DBG("%s[%d]:%s:vlan-mapping qinq svlan = %d,cvlan = %d translate svlan = %d cos = %d  cvlan = %d\n",__FILE__,__LINE__,__FUNCTION__, info.svlan.vlan_start, info.cvlan.vlan_start, info.svlan_new.vlan_start, info.svlan_cos, info.cvlan_new.vlan_start);

	return CMD_SUCCESS;
}


DEFUN(vlan_mapping_qinq_scvlan_to_scvlan_2,
	vlan_mapping_qinq_scvlan_to_scvlan_cmd_2,
	"vlan-mapping qinq svlan <1-4094> cvlan <1-4094> translate svlan <1-4094> {cos <0-7>} ",
	"Vlan-mapping rule\n"
	"Qinq rule\n" 
	"Svlan\n"
	"Svlanid<1-4094>\n"
	"Cvlan\n"
	"Cvlanid<1-4094>\n"
	"Translate command\n"
	"Svlan_new\n"
	"Svlan_newid<1-4094>\n"
	"Cos of svlan_new\n"
	"Cos<0-7>\n"
)
{
	uint32_t ifindex = 0;
	int ret = 0;
	struct vlan_mapping info;
	uint8_t mode = 0;
	struct l2if *p_l2if = NULL;
	struct listnode *p_listnode = NULL;
	struct vlan_mapping *p_vlan_mapping = NULL;
	
	/*get ifindex from interface we use*/	
	ifindex = (uint32_t)vty->index;
	
	/*检查mode是否是switch*/
	if(ifm_get_mode(ifindex,MODULE_ID_L2, &mode) != 0)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to get mode from ifm\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Fail to get interface mode%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if((uint8_t)IFNET_MODE_SWITCH != mode)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s,mode != IFNET_MODE_SWITCH\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*在哈希表搜寻ifindex对应节点，如果没有则创建*/
	p_l2if = l2if_get(ifindex);
	if(NULL == p_l2if)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to CALLOC if l2 table bucket\n",__FILE__,__LINE__,__FUNCTION__);
		return CMD_WARNING;
	}
	
	/*检查switch mode*/
	if(p_l2if->switch_info.mode == SWITCH_MODE_ACCESS)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:mode must be hybrid or trunk\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Switch mode error: must be hybrid or trunk%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	/*检查dot1q tunnel是否为空*/
	if(DEFAULT_VLAN_ID != p_l2if->switch_info.dot1q_tunnel)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error: dot1q tunnel != 0\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Set vlan-mapping error: Please delete dot1q_tunnel first. %s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*get input PRM from cli*/
	memset(&info, 0, sizeof(struct vlan_mapping));
	info.svlan.vlan_start = atoi(argv[0]);	
	info.cvlan.vlan_start = atoi(argv[1]);
	info.svlan_new.vlan_start = atoi(argv[2]);
	if(NULL != argv[3])
	{
		info.svlan_cos = atoi(argv[3]);
	}
	else
	{
		info.svlan_cos = 8;
	}
	
	info.cvlan_new.vlan_start = info.cvlan.vlan_start;
	info.cvlan.vlan_end= info.cvlan.vlan_start;
	info.svlan.vlan_end= info.svlan.vlan_start;
	info.cvlan_new.vlan_end= info.cvlan_new.vlan_start;
	info.svlan_new.vlan_end= info.svlan_new.vlan_start;
	info.cvlan_cos = 8;
	info.cvlan_act = VLAN_ACTION_TRANSLATE;
	info.svlan_act = VLAN_ACTION_TRANSLATE;
	
	if((info.svlan.vlan_start == info.svlan_new.vlan_start)&&(info.cvlan.vlan_start == info.cvlan_new.vlan_start))
	{
		QINQ_LOG_DBG("%s[%d]:vlan input error,No change after translate!\n",__FILE__,__LINE__);
		vty_error_out (vty, "Vlan input error,No change after translate!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	/*check if vlan is used*/
	for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.qinq_list, p_listnode, p_vlan_mapping))
	{
		if(qinq_check_vlan_range(&(p_vlan_mapping->cvlan), &(info.cvlan)))
		{
			if(qinq_check_vlan_range(&(p_vlan_mapping->svlan), &(info.svlan)))
			{
				vty_error_out (vty, "Input error:svlan<%d-%d> cvlan<%d-%d> has member already be used%s"
					,info.svlan.vlan_start,info.svlan.vlan_end,info.cvlan.vlan_start,info.cvlan.vlan_end,VTY_NEWLINE);
				QINQ_LOG_DBG("%s[%d]:leave %s:error:cvlan<%d-%d> svlan<%d-%d> has vlan used\n",__FILE__,__LINE__,__FUNCTION__
					,info.cvlan.vlan_start,info.cvlan.vlan_end,info.svlan.vlan_start,info.svlan.vlan_end);
				return CMD_WARNING;
			}
		}

		if(qinq_check_vlan_range(&(p_vlan_mapping->cvlan_new), &(info.cvlan_new)))
		{
			if(qinq_check_vlan_range(&(p_vlan_mapping->svlan_new), &(info.svlan_new)))
			{
				vty_error_out (vty, "Input error:svlan_new<%d-%d> cvlan_new<%d-%d> has member already be used%s"
					,info.svlan_new.vlan_start,info.svlan_new.vlan_end,info.cvlan_new.vlan_start,info.cvlan_new.vlan_end,VTY_NEWLINE);
				QINQ_LOG_DBG("%s[%d]:leave %s:error:cvlan_new<%d-%d> svlan_new<%d-%d> has vlan used\n",__FILE__,__LINE__,__FUNCTION__
					,info.cvlan_new.vlan_start,info.cvlan_new.vlan_end,info.svlan_new.vlan_start,info.svlan_new.vlan_end);
				return CMD_WARNING;
			}
		}
	}

	if( devtype == ID_HT157 || devtype == ID_HT158 )
	{
		ret = qinq_add(p_l2if, &info, 5, 0);
	}
	else 
	{
		ret = qinq_add(p_l2if, &info, 100, 0);
	}

	if(ret != ERRNO_SUCCESS)
	{
		if(ret==ERRNO_OVERSIZE)
		{
			/*for print configured conversion*/
			for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.qinq_list, p_listnode, p_vlan_mapping))
			{  
				if(info.svlan_new.vlan_start==p_vlan_mapping->svlan_new.vlan_start)
				{    
					if(info.cvlan_new.vlan_start==p_vlan_mapping->cvlan_new.vlan_start)
					{ 
						if(NULL != argv[3])
						{
							vty_error_out (vty, "Vlan-mapping: chip resource is full,partial translate configured:vlan-mapping qinq svlan %d cvlan %d translate svlan %d cos %d%s",
								p_vlan_mapping->svlan.vlan_start,p_vlan_mapping->cvlan.vlan_start,p_vlan_mapping->svlan_new.vlan_start,p_vlan_mapping->svlan_cos,VTY_NEWLINE);
						}
						else 
						{
							vty_error_out (vty, "Vlan-mapping: chip resource is full,partial translate configured:vlan-mapping qinq svlan %d cvlan %d translate svlan %d%s",
								p_vlan_mapping->svlan.vlan_start,p_vlan_mapping->cvlan.vlan_start,p_vlan_mapping->svlan_new.vlan_start,VTY_NEWLINE);
						}
						return CMD_SUCCESS;
					}
				}
			}
			vty_error_out (vty, "Fail to set vlan-mapping:chip resource is full.%s", VTY_NEWLINE);
		}
		else
		{
			QINQ_LOG_DBG("%s[%d]:%s:fail to set vlan-mapping\n",__FILE__,__LINE__, __FUNCTION__);
        	vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
		}
	    return CMD_WARNING;
	}


	QINQ_LOG_DBG("%s[%d]:%s:vlan-mapping qinq svlan = %d,cvlan = %d translate svlan = %d cos = %d  cvlan = %d\n",__FILE__,__LINE__,__FUNCTION__, info.svlan.vlan_start, info.cvlan.vlan_start, info.svlan_new.vlan_start, info.svlan_cos, info.cvlan_new.vlan_start);

	return CMD_SUCCESS;
}

DEFUN(vlan_mapping_qinq_scvlan_to_scvlan_3,
	vlan_mapping_qinq_scvlan_to_scvlan_cmd_3,
	"vlan-mapping qinq svlan <1-4094> cvlan <1-4094> translate svlan <1-4094> cvlan <1-4094>",
	"Vlan-mapping rule\n"
	"Qinq rule\n" 
	"Svlan\n"
	"Svlanid<1-4094>\n"
	"Cvlan\n"
	"Cvlanid<1-4094>\n"
	"Translate command\n"
	"Svlan_new\n"
	"Svlan_newid<1-4094>\n"
	"Cvlan_new\n"
	"Cvlan_newid<1-4094>\n"
)
{
	uint32_t ifindex = 0;
	int ret = 0;
	struct vlan_mapping info;
	uint8_t mode = 0;
	struct l2if *p_l2if = NULL;
	struct listnode *p_listnode = NULL;
	struct vlan_mapping *p_vlan_mapping = NULL;
	
	/*get ifindex from interface we use*/	
	ifindex = (uint32_t)vty->index;
	
	/*检查mode是否是switch*/
	if(ifm_get_mode(ifindex,MODULE_ID_L2, &mode) != 0)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to get mode from ifm\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Fail to get interface mode%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if((uint8_t)IFNET_MODE_SWITCH != mode)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s,mode != IFNET_MODE_SWITCH\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*在哈希表搜寻ifindex对应节点，如果没有则创建*/
	p_l2if = l2if_get(ifindex);
	if(NULL == p_l2if)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to CALLOC if l2 table bucket\n",__FILE__,__LINE__,__FUNCTION__);
		return CMD_WARNING;
	}
	
	/*检查switch mode*/
	if(p_l2if->switch_info.mode == SWITCH_MODE_ACCESS)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:mode must be hybrid or trunk\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Switch mode error: must be hybrid or trunk%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	/*检查dot1q tunnel是否为空*/
	if(DEFAULT_VLAN_ID != p_l2if->switch_info.dot1q_tunnel)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error: dot1q tunnel != 0\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Set vlan-mapping error: Please delete dot1q_tunnel first. %s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*get input PRM from cli*/
	memset(&info, 0, sizeof(struct vlan_mapping));
	info.svlan.vlan_start = atoi(argv[0]);	
	info.cvlan.vlan_start = atoi(argv[1]);
	info.svlan_new.vlan_start = atoi(argv[2]);
	info.svlan_cos = 8;	
	info.cvlan_new.vlan_start = atoi(argv[3]);

	info.cvlan.vlan_end= info.cvlan.vlan_start;
	info.svlan.vlan_end= info.svlan.vlan_start;
	info.cvlan_new.vlan_end= info.cvlan_new.vlan_start;
	info.svlan_new.vlan_end= info.svlan_new.vlan_start;
	info.cvlan_cos = 8;
	info.cvlan_act = VLAN_ACTION_TRANSLATE;
	info.svlan_act = VLAN_ACTION_TRANSLATE;
	
	if((info.svlan.vlan_start == info.svlan_new.vlan_start)&&(info.cvlan.vlan_start == info.cvlan_new.vlan_start))
	{
		QINQ_LOG_DBG("%s[%d]:vlan input error,No change after translate!\n",__FILE__,__LINE__);
		vty_error_out (vty, "Vlan input error,No change after translate!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	/*check if vlan is used*/
	for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.qinq_list, p_listnode, p_vlan_mapping))
	{
		if(qinq_check_vlan_range(&(p_vlan_mapping->cvlan), &(info.cvlan)))
		{
			if(qinq_check_vlan_range(&(p_vlan_mapping->svlan), &(info.svlan)))
			{
				vty_error_out (vty, "Input error:svlan<%d-%d> cvlan<%d-%d> has member already be used%s"
					,info.svlan.vlan_start,info.svlan.vlan_end,info.cvlan.vlan_start,info.cvlan.vlan_end,VTY_NEWLINE);
				QINQ_LOG_DBG("%s[%d]:leave %s:error:cvlan<%d-%d> svlan<%d-%d> has vlan used\n",__FILE__,__LINE__,__FUNCTION__
					,info.cvlan.vlan_start,info.cvlan.vlan_end,info.svlan.vlan_start,info.svlan.vlan_end);
				return CMD_WARNING;
			}
		}

		if(qinq_check_vlan_range(&(p_vlan_mapping->cvlan_new), &(info.cvlan_new)))
		{
			if(qinq_check_vlan_range(&(p_vlan_mapping->svlan_new), &(info.svlan_new)))
			{
				vty_error_out (vty, "Input error:svlan_new<%d-%d> cvlan_new<%d-%d> has member already be used%s"
					,info.svlan_new.vlan_start,info.svlan_new.vlan_end,info.cvlan_new.vlan_start,info.cvlan_new.vlan_end,VTY_NEWLINE);
				QINQ_LOG_DBG("%s[%d]:leave %s:error:cvlan_new<%d-%d> svlan_new<%d-%d> has vlan used\n",__FILE__,__LINE__,__FUNCTION__
					,info.cvlan_new.vlan_start,info.cvlan_new.vlan_end,info.svlan_new.vlan_start,info.svlan_new.vlan_end);
				return CMD_WARNING;
			}
		}
	}

	if( devtype == ID_HT157 || devtype == ID_HT158 )
	{
		ret = qinq_add(p_l2if, &info, 5, 0);
	}
	else 
	{
		ret = qinq_add(p_l2if, &info, 100, 0);
	}

	if(ret != ERRNO_SUCCESS)
	{
		if(ret==ERRNO_OVERSIZE)
		{
			/*for print configured conversion*/
			for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.qinq_list, p_listnode, p_vlan_mapping))
			{  
				if(info.svlan_new.vlan_start==p_vlan_mapping->svlan_new.vlan_start)
				{    
					if(info.cvlan_new.vlan_start==p_vlan_mapping->cvlan_new.vlan_start)
					{ 
						vty_error_out (vty, "Vlan-mapping: chip resource is full,partial translate configured:vlan-mapping qinq svlan %d cvlan %d translate svlan %d cvlan %d%s",
							p_vlan_mapping->svlan.vlan_start,p_vlan_mapping->cvlan.vlan_start,p_vlan_mapping->svlan_new.vlan_start,p_vlan_mapping->cvlan_new.vlan_start,VTY_NEWLINE);
						return CMD_SUCCESS;
					}
				}
			}
			vty_error_out (vty, "Fail to set vlan-mapping:chip resource is full.%s", VTY_NEWLINE);
		}
		else
		{
			QINQ_LOG_DBG("%s[%d]:%s:fail to set vlan-mapping\n",__FILE__,__LINE__, __FUNCTION__);
        	vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
		}
	    return CMD_WARNING;
	}


	QINQ_LOG_DBG("%s[%d]:%s:vlan-mapping qinq svlan = %d,cvlan = %d translate svlan = %d cos = %d  cvlan = %d\n",__FILE__,__LINE__,__FUNCTION__, info.svlan.vlan_start, info.cvlan.vlan_start, info.svlan_new.vlan_start, info.svlan_cos, info.cvlan_new.vlan_start);

	return CMD_SUCCESS;
}



DEFUN(no_vlan_mapping_svlan,
	no_vlan_mapping_svlan_cmd,
	"no vlan-mapping dot1q <1-4094> {to <1-4094>} ",
	"No command\n"
	"Vlan-mapping rule\n"
	"Dot1q rule\n"
	"Svlanid start <1-4094>\n"
	"To\n"
	"Svlanid end <1-4094>\n"
)
{
	uint32_t ifindex = 0;
	int ret = 0;
	struct vlan_mapping info;
	struct l2if *p_l2if = NULL;
	struct listnode *p_listnode = NULL;
	struct vlan_mapping *p_vlan_mapping = NULL;
	uint8_t mode = 0;
	
	/*get ifindex from interface we use*/	
	ifindex = (uint32_t)vty->index;
	
	/*检查mode是否是switch*/
	if(ifm_get_mode(ifindex,MODULE_ID_L2, &mode) != 0)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to get mode from ifm\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Fail to get interface mode%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if((uint8_t)IFNET_MODE_SWITCH != mode)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s,mode != IFNET_MODE_SWITCH\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	p_l2if = l2if_lookup(ifindex);
	if(NULL == p_l2if)
	{
		QINQ_LOG_DBG("%s[%d]:%s:invalid ifindex,do not need to set no dot1q\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "No vlan-mapping error,lookup failed%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*get input PRM from cli*/
	memset(&info, 0, sizeof(struct vlan_mapping));
	info.svlan.vlan_start = atoi(argv[0]);

	if(NULL != argv[1])
		info.svlan.vlan_end = atoi(argv[1]);
	else
		info.svlan.vlan_end = info.svlan.vlan_start;
	
	if(info.svlan.vlan_end < info.svlan.vlan_start)
	{
		QINQ_LOG_DBG("%s[%d]:vlanid_end = %d must great than or equal to vlanid_start = %d\n",__FILE__,__LINE__, info.svlan.vlan_end, info.svlan.vlan_start);
		vty_error_out (vty, "Vlan input error,vlanid_end(%d) must not less than vlanid_start(%d) !%s", info.svlan.vlan_end, info.svlan.vlan_start, VTY_NEWLINE);
		return CMD_WARNING;
	}

	for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.qinq_list, p_listnode, p_vlan_mapping))
	{
		if(info.cvlan.vlan_start == p_vlan_mapping->cvlan.vlan_start
			&& info.cvlan.vlan_end== p_vlan_mapping->cvlan.vlan_end
			&& info.svlan.vlan_start == p_vlan_mapping->svlan.vlan_start
			&& info.svlan.vlan_end== p_vlan_mapping->svlan.vlan_end)
		{
			if(p_vlan_mapping->range_qinq==L2IF_INFO_QINQ_RANGE)
			{
				ret = qinq_range_delete(p_l2if, p_vlan_mapping);
			}
			else
			{
				ret = qinq_delete(p_l2if, p_vlan_mapping, 10, 1);
			}
			if(ERRNO_FAIL == ret)
			{
				QINQ_LOG_DBG("%s[%d]:leave %s:fail to delete vlan-mapping dot1q <%d-%d>\n",__FILE__,__LINE__, __FUNCTION__,info.svlan.vlan_start, info.svlan.vlan_end);
				if(NULL != argv[1])
					vty_error_out (vty, "%%No vlan-mapping dot1q %d to %d error,hal return fail.%s",info.svlan.vlan_start, info.svlan.vlan_end,VTY_NEWLINE);
				else
					vty_error_out (vty, "%%No vlan-mapping dot1q %d error,hal return fail.%s",info.svlan.vlan_start,VTY_NEWLINE);
				return CMD_WARNING;
			}
			else 
			{
				XFREE(MTYPE_L2_VLAN, p_vlan_mapping);
				list_delete_node (p_l2if->switch_info.qinq_list, p_listnode);
				QINQ_LOG_DBG("%s[%d]:%s:sucessfully delete vlan-mapping dot1q <%d-%d> \n",__FILE__,__LINE__,__FUNCTION__, info.svlan.vlan_start, info.svlan.vlan_end);
				return CMD_SUCCESS;
			}
		}
	}

	QINQ_LOG_DBG("%s[%d]:leave %s:fail to find vlan-mapping dot1q <%d-%d>\n",__FILE__,__LINE__, __FUNCTION__,info.svlan.vlan_start, info.svlan.vlan_end);
	if(NULL != argv[1])
		vty_error_out (vty, "No vlan-mapping dot1q %d to %d,lookup failed%s",info.svlan.vlan_start, info.svlan.vlan_end,VTY_NEWLINE);
	else
		vty_error_out (vty, "No vlan-mapping dot1q %d,lookup failed%s",info.svlan.vlan_start,VTY_NEWLINE);
	return CMD_WARNING;
}


DEFUN(vlan_mapping_svlan_to_scvlan,
	vlan_mapping_svlan_to_scvlan_cmd,
	"vlan-mapping dot1q <1-4094> translate svlan <1-4094> {cos <0-7>} cvlan <1-4094> ",
	"Vlan-mapping rule\n"
	"Dot1q rule\n"
	"Svlanid start <1-4094>\n"
	"Translate command\n"
	"Svlan_new\n"
	"Svlan_newid<1-4094>\n"
	"Cos of svlan_new\n"
	"Cos<0-7>\n"
	"Cvlan_new\n"
	"Cvlan_newid <1-4094>\n"
)
{
	uint32_t ifindex = 0;
	int ret = 0;
	struct vlan_mapping info;
	uint8_t mode = 0;
	struct l2if *p_l2if = NULL;
	struct listnode *p_listnode = NULL;
	struct vlan_mapping *p_vlan_mapping = NULL;
	
	/*get ifindex from interface we use*/
	ifindex = (uint32_t)vty->index;
	
	/*检查mode是否是switch*/
	if(ifm_get_mode(ifindex,MODULE_ID_L2, &mode) != 0)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to get mode from ifm\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Fail to get interface mode%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if((uint8_t)IFNET_MODE_SWITCH != mode)
	{
		vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	/*在哈希表搜寻ifindex对应节点，如果没有则创建*/
	p_l2if = l2if_get(ifindex);
	if(NULL == p_l2if)
	{
		vty_error_out (vty, "Fail to CALLOC if l2 table bucket %s", VTY_NEWLINE);
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to CALLOC if l2 table bucket\n",__FILE__,__LINE__,__FUNCTION__);
		return CMD_WARNING;
	}	
	
	/*检查switch mode*/
	if(p_l2if->switch_info.mode == SWITCH_MODE_ACCESS)
	{
		vty_error_out (vty, "Switch mode error: must be hybrid or trunk%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	/*检查dot1q tunnel是否为空*/
	if(DEFAULT_VLAN_ID != p_l2if->switch_info.dot1q_tunnel )
	{
		vty_error_out (vty, "Set vlan-mapping error: Please delete dot1q_tunnel first. %s",VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	/*get PRM from cli*/
	memset(&info, 0, sizeof(struct vlan_mapping));
	info.svlan.vlan_start= atoi(argv[0]);	
	info.svlan_new.vlan_start = atoi(argv[1]);
	info.cvlan_new.vlan_start = atoi(argv[3]);
	
	if(NULL != argv[2])
		info.svlan_cos = atoi(argv[2]);
	else
		info.svlan_cos = 8;
#if 0 
	//edit for bug#53181
	if(info.svlan_new.vlan_start == p_l2if->switch_info.access_vlan)
	{
		QINQ_LOG_DBG("%s[%d]:error:svlanid must not be equal access vlanid\n",__FILE__,__LINE__);
		vty_error_out (vty, "Svlanid must not be equal access vlanid%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
#endif
	info.svlan.vlan_end = info.svlan.vlan_start;
	info.svlan_new.vlan_end = info.svlan_new.vlan_start;
	info.cvlan_new.vlan_end = info.cvlan_new.vlan_start;
	info.cvlan_cos = 8;
	info.cvlan_act = VLAN_ACTION_ADD;
	info.svlan_act = VLAN_ACTION_TRANSLATE;
	
	/*check if slan is used*/
	for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.qinq_list, p_listnode, p_vlan_mapping))
	{
		if(qinq_check_vlan_range(&(p_vlan_mapping->svlan), &(info.svlan)))
		{
			if(qinq_check_vlan_range(&(p_vlan_mapping->cvlan), &(info.cvlan)))
			{
				vty_error_out (vty, "Input error:dot1q vlanid<%d-%d> has member already be used%s",info.svlan.vlan_start,info.svlan.vlan_end,VTY_NEWLINE);
				QINQ_LOG_DBG("%s[%d]:leave %s:error:vlan<%d-%d> has vlan used\n",__FILE__,__LINE__,__FUNCTION__
				,info.svlan.vlan_start,info.svlan.vlan_end);
				return CMD_WARNING;
			}	
		}
		if(qinq_check_vlan_range(&(p_vlan_mapping->cvlan_new), &(info.cvlan_new)))
		{
			if(qinq_check_vlan_range(&(p_vlan_mapping->svlan_new),&(info.svlan_new)))
			{
				vty_error_out (vty, "Input error:svlan_new<%d-%d> cvlan_new<%d-%d> has member already be used%s"
					,info.svlan_new.vlan_start,info.svlan_new.vlan_end,info.cvlan_new.vlan_start,info.cvlan_new.vlan_end,VTY_NEWLINE);
				QINQ_LOG_DBG("%s[%d]:leave %s:error:cvlan_new<%d-%d> svlan_new<%d-%d> has vlan used\n",__FILE__,__LINE__,__FUNCTION__
					,info.cvlan_new.vlan_start,info.cvlan_new.vlan_end,info.svlan_new.vlan_start,info.svlan_new.vlan_end);
				return CMD_WARNING;
			}
		}
	}

	if( devtype == ID_HT157 || devtype == ID_HT158 )
	{
		ret = qinq_add(p_l2if, &info, 5, 1);
	}
	else 
	{
		ret = qinq_add(p_l2if, &info, 100, 1);
	}

	if(ret != ERRNO_SUCCESS)
	{
		if(ret==ERRNO_OVERSIZE)
		{
			/*for print configured conversion*/
			for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.qinq_list, p_listnode, p_vlan_mapping))
			{  
				if(info.svlan_new.vlan_start==p_vlan_mapping->svlan_new.vlan_start)
				{    
					if(info.cvlan_new.vlan_start==p_vlan_mapping->cvlan_new.vlan_start)
					{ 
						if(NULL != argv[2])
						{
							vty_error_out (vty, "Vlan-mapping: chip resource is full,partial translate configured:vlan-mapping dot1q %d translate svlan %d cos %d cvlan %d%s",
								p_vlan_mapping->svlan.vlan_start,p_vlan_mapping->svlan_new.vlan_start,p_vlan_mapping->svlan_cos,p_vlan_mapping->cvlan_new.vlan_start,VTY_NEWLINE);
						}
						else 
						{
							vty_error_out (vty, "Vlan-mapping: chip resource is full,partial translate configured:vlan-mapping dot1q %d translate svlan %d cvlan %d%s",
								p_vlan_mapping->svlan.vlan_start,p_vlan_mapping->svlan_new.vlan_start,p_vlan_mapping->cvlan_new.vlan_start,VTY_NEWLINE);
						}
						return CMD_SUCCESS;
					}
				}
			}
			vty_error_out (vty, "Fail to set vlan-mapping:chip resource is full.%s", VTY_NEWLINE);
		}
		else
		{
			QINQ_LOG_DBG("%s[%d]:%s:fail to set vlan-mapping\n",__FILE__,__LINE__, __FUNCTION__);
        	vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
		}
	    return CMD_WARNING;
	}

	QINQ_LOG_DBG("%s[%d]:%s:vlan-mapping dot1q %d translate svlan = %d,cos = %d, cvlan = %d\n",__FILE__,__LINE__,__FUNCTION__, info.svlan.vlan_start,info.svlan_new.vlan_start,info.svlan_cos,info.cvlan_new.vlan_start);

	return CMD_SUCCESS;
}

DEFUN(vlan_mapping_add_svlan,
	vlan_mapping_add_svlan_cmd,
	"vlan-mapping dot1q <1-4094> {to <1-4094>} add svlan <1-4094> { cos <0-7> }",
	"Vlan-mapping rule\n"
	"Dot1q rule\n"
	"Svlanid start <1-4094>\n"
	"To\n"
	"Svlanid end <1-4094>\n"
	"Add command\n"
	"Svlan_new\n"
	"Svlan_newid<1-4094>\n"
	"Cos of svlan_new\n"
	"Cos<0-7>\n"
)
{
	uint32_t ifindex = 0;
	int ret = 0;
	struct vlan_mapping info;
	uint8_t mode = 0;
	struct l2if *p_l2if = NULL;
	struct listnode *p_listnode = NULL;
	struct vlan_mapping *p_vlan_mapping = NULL;
	uint32_t range_num=0;
	
	/*get ifindex from interface we use*/
	ifindex = (uint32_t)vty->index;
	
	/*检查mode是否是switch*/
	if(ifm_get_mode(ifindex,MODULE_ID_L2, &mode) != 0)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to get mode from ifm\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Fail to get interface mode%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if((uint8_t)IFNET_MODE_SWITCH != mode)
	{
		vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	/*在哈希表搜寻ifindex对应节点，如果没有则创建*/
	p_l2if = l2if_get(ifindex);
	if(NULL == p_l2if)
	{
		vty_error_out (vty, "Fail to CALLOC if l2 table bucket %s", VTY_NEWLINE);
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to CALLOC if l2 table bucket\n",__FILE__,__LINE__,__FUNCTION__);
		return CMD_WARNING;
	}	
	
	/*检查mode*/
	if(p_l2if->switch_info.mode == SWITCH_MODE_ACCESS)
	{
		vty_error_out (vty, "Switch mode error: must be hybrid or trunk%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	/*检查dot1q tunnel是否为空*/
	if(DEFAULT_VLAN_ID != p_l2if->switch_info.dot1q_tunnel)
	{
		vty_error_out (vty, "Set vlan-mapping error: Please delete dot1q_tunnel first. %s",VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	/*get PRM from cli*/
	memset(&info, 0, sizeof(struct vlan_mapping));
	info.svlan.vlan_start = atoi(argv[0]);
	info.svlan_new.vlan_start = atoi(argv[2]);

	if(NULL != argv[1])
	{
		info.svlan.vlan_end = atoi(argv[1]);
		if(info.svlan.vlan_end <= info.svlan.vlan_start)
		{
			QINQ_LOG_DBG("%s[%d]:,vlan input error,vlanid_end(%d) must more than vlanid_start(%d) !\n",__FILE__,__LINE__, info.svlan.vlan_end, info.svlan.vlan_start);
			vty_error_out (vty, "Vlan input error,vlanid_end(%d) must more than vlanid_start(%d) !%s", info.svlan.vlan_end,info.svlan.vlan_start,VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	else
	{
		info.svlan.vlan_end = info.svlan.vlan_start;
	}

	if(NULL != argv[3])
		info.svlan_cos = atoi(argv[3]);
	else
		info.svlan_cos = 8;
	
#if 0 
	//edit for bug#53181
	if(info.svlan_new.vlan_start == p_l2if->switch_info.access_vlan)
	{
		QINQ_LOG_DBG("%s[%d]:error:svlanid must not be equal to access vlanid\n",__FILE__,__LINE__);
		vty_error_out (vty, "Error:svlanid must not be equal to access vlanid%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
#endif
	info.svlan_new.vlan_end = info.svlan_new.vlan_start;
	info.cvlan_new.vlan_start = info.svlan.vlan_start;
	info.cvlan_new.vlan_end = info.svlan.vlan_end;
	info.cvlan_cos = 8;
	info.cvlan_act = VLAN_ACTION_NO;
	info.svlan_act = VLAN_ACTION_ADD;

	/*check if cvlan/svlan is used*/
	for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.qinq_list, p_listnode, p_vlan_mapping))
	{
		if(qinq_check_vlan_range(&(p_vlan_mapping->svlan), &(info.svlan)))
		{
			if(qinq_check_vlan_range(&(p_vlan_mapping->cvlan), &(info.cvlan)))
			{
				vty_error_out (vty, "Input error:dot1q vlanid<%d-%d> has member already be used%s",info.svlan.vlan_start,info.svlan.vlan_end,VTY_NEWLINE);
				QINQ_LOG_DBG("%s[%d]:leave %s:error:vlan<%d-%d> has vlan used\n",__FILE__,__LINE__,__FUNCTION__,info.svlan.vlan_start,info.svlan.vlan_end);
				return CMD_WARNING;
			}
		}
		
		if(qinq_check_vlan_range(&(p_vlan_mapping->cvlan_new), &(info.cvlan_new)))
		{
			if(qinq_check_vlan_range(&(p_vlan_mapping->svlan_new), &(info.svlan_new)))
			{
				vty_error_out (vty, "Input error:svlan_new<%d-%d> cvlan_new<%d-%d> has member already be used%s"
					,info.svlan_new.vlan_start,info.svlan_new.vlan_end,info.cvlan_new.vlan_start,info.cvlan_new.vlan_end,VTY_NEWLINE);
				QINQ_LOG_DBG("%s[%d]:leave %s:error:cvlan_new<%d-%d> svlan_new<%d-%d> has vlan used\n",__FILE__,__LINE__,__FUNCTION__
					,info.cvlan_new.vlan_start,info.cvlan_new.vlan_end,info.svlan_new.vlan_start,info.svlan_new.vlan_end);
				return CMD_WARNING;
			}
		}
	}

	range_num=info.svlan.vlan_end-info.svlan.vlan_start; 
	/*use L2IF_INFO_QINQ_RANGE*/
	if((range_num>=99)&&((devtype != ID_HT157)||(devtype != ID_HT158) ))
	{	
		info.range_qinq=L2IF_INFO_QINQ_RANGE; 
		ret=qinq_range_add(p_l2if, &info);
		if(ret==ERRNO_FAIL)
		{
			QINQ_LOG_DBG("%s[%d]:%s:fail to set vlan-mapping\n",__FILE__,__LINE__, __FUNCTION__);
        	vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
	 	  	return CMD_WARNING;
		}
		else if(ret==ERRNO_OVERSIZE)
		{
			info.range_qinq=L2IF_INFO_QINQ;
			zlog_err("%s[%d]: %s:error:Chip  resource is full !\n",__FILE__,__LINE__,__func__);
		}
		else if(ret==ERRNO_SUCCESS)
		{
			return CMD_SUCCESS;
		}
	}

	if( devtype == ID_HT157 || devtype == ID_HT158 )
	{
		ret = qinq_add(p_l2if, &info, 5, 1);
	}
	else 
	{
		ret = qinq_add(p_l2if, &info, 100, 1);
	}

	if(ret != ERRNO_SUCCESS)
	{
		if(ret==ERRNO_OVERSIZE)
		{
			/*for print configured conversion*/
			for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.qinq_list, p_listnode, p_vlan_mapping))
			{  
				if(info.svlan_new.vlan_start==p_vlan_mapping->svlan_new.vlan_start)
				{    
					if(info.cvlan_new.vlan_start==p_vlan_mapping->cvlan_new.vlan_start)
					{   
						if(NULL != argv[3])
						{
							vty_error_out (vty, "Vlan-mapping: chip resource is full,partial translate configured:vlan-mapping dot1q %d to %d add %d cos %d%s",
								p_vlan_mapping->svlan.vlan_start,p_vlan_mapping->svlan.vlan_end, p_vlan_mapping->svlan_new.vlan_start,p_vlan_mapping->svlan_cos, VTY_NEWLINE);
						}
						else 
						{
							vty_error_out (vty, "Vlan-mapping: chip resource is full,partial translate configured:vlan-mapping dot1q %d to %d add %d%s",
								p_vlan_mapping->svlan.vlan_start,p_vlan_mapping->svlan.vlan_end, p_vlan_mapping->svlan_new.vlan_start,VTY_NEWLINE);
						}
						return CMD_SUCCESS;
					}
				}
			}
			vty_error_out (vty, "Fail to set vlan-mapping:chip resource is full.%s", VTY_NEWLINE);
		}
		else
		{
			QINQ_LOG_DBG("%s[%d]:%s:fail to set vlan-mapping\n",__FILE__,__LINE__, __FUNCTION__);
        	vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
		}
	    return CMD_WARNING;
	}

	QINQ_LOG_DBG("%s[%d]:%s:svlan<%d-%d> add svlan %d , cos %d\n",__FILE__,__LINE__,__FUNCTION__,info.svlan.vlan_start, info.svlan.vlan_end, info.svlan_new.vlan_start, info.svlan_cos);

	return CMD_SUCCESS;
}

DEFUN(vlan_mapping_svlans_to_svlan,
	vlan_mapping_svlans_to_svlan_cmd,
	"vlan-mapping dot1q <1-4094> {to <1-4094>} translate <1-4094>",
	"Vlan-mapping rule\n"
	"Dot1q rule\n"
	"Svlanid start <1-4094>\n"
	"To\n"
	"Svlanid end <1-4094>\n"
	"Translate command\n"
	"Svlan_newid <1-4094>\n"
)
{
	uint32_t ifindex = 0;
	int ret = 0;
	struct vlan_mapping info;
	uint8_t mode = 0;
	struct l2if *p_l2if = NULL;
	struct listnode *p_listnode = NULL;
	struct vlan_mapping *p_vlan_mapping = NULL;

	/*get ifindex from interface we use*/
	ifindex = (uint32_t)vty->index;
	
	/*检查mode是否是switch*/
	if(ifm_get_mode(ifindex,MODULE_ID_L2, &mode) != 0)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to get mode from ifm\n",__FILE__,__LINE__,__FUNCTION__);
		vty_error_out (vty, "Fail to get interface mode%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if((uint8_t)IFNET_MODE_SWITCH != mode)
	{
		vty_error_out (vty, "Mode error,not switch %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	/*在哈希表搜寻ifindex对应节点，如果没有则创建*/
	p_l2if = l2if_get(ifindex);
	if(NULL == p_l2if)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to CALLOC if l2 table bucket\n",__FILE__,__LINE__,__FUNCTION__);
		return CMD_WARNING;
	}	
	
	/*检查mode*/
	if(p_l2if->switch_info.mode == SWITCH_MODE_ACCESS)
	{
		vty_error_out (vty, "Switch mode error: must be hybrid or trunk%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	/*检查dot1q tunnel是否为空*/
	if(DEFAULT_VLAN_ID != p_l2if->switch_info.dot1q_tunnel)
	{
		vty_error_out (vty, "Set vlan-mapping error: Please delete dot1q_tunnel first. %s",VTY_NEWLINE);
		return CMD_WARNING;
	}

    /*get PRM from cli*/
	memset(&info, 0, sizeof(struct vlan_mapping));
	info.svlan.vlan_start = atoi(argv[0]);
	info.svlan_new.vlan_start = atoi(argv[2]);

	if(NULL != argv[1])
	{
		if(devtype == ID_HT157 || devtype == ID_HT158)
		{
			vty_warning_out (vty, "HT15X not support!%s",VTY_NEWLINE);
			return CMD_WARNING;
		}
		info.svlan.vlan_end  = atoi(argv[1]);
		if(info.svlan.vlan_end <= info.svlan.vlan_start)
		{
			QINQ_LOG_DBG("%s[%d]:,vlan input error,vlanid_end(%d) must more than vlanid_start(%d) !\n",__FILE__,__LINE__, info.svlan.vlan_end, info.svlan.vlan_start);
			vty_error_out (vty, "Vlan input error,vlanid_end(%d) must more than vlanid_start(%d) !%s", info.svlan.vlan_end,info.svlan.vlan_start,VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	else
	{
		info.svlan.vlan_end  = info.svlan.vlan_start;
	}
	
	info.svlan_new.vlan_end = info.svlan_new.vlan_start;
	info.cvlan_cos = 8;
	info.svlan_cos = 8;
	info.cvlan_act = VLAN_ACTION_NO;
	info.svlan_act = VLAN_ACTION_TRANSLATE;

	if(qinq_check_vlan_range(&(info.svlan_new),&(info.svlan)))
	{
		QINQ_LOG_DBG("%s[%d]:,vlan input error,svlan_new(%d) must not include in svlan(%d-%d) !\n",__FILE__,__LINE__, info.svlan_new.vlan_start, info.svlan.vlan_start,info.svlan.vlan_end);
		vty_error_out (vty, "Vlan input error,svlan_new(%d) must not include in svlan(%d-%d) !%s", info.svlan_new.vlan_start, info.svlan.vlan_start,info.svlan.vlan_end,VTY_NEWLINE);
		return CMD_WARNING;
	}
	/*check if slan is used*/
	for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.qinq_list, p_listnode, p_vlan_mapping))
	{
		if(qinq_check_vlan_range(&(p_vlan_mapping->svlan),&(info.svlan)))
		{
			if(qinq_check_vlan_range(&(p_vlan_mapping->cvlan), &(info.cvlan)))
			{
				vty_error_out (vty, "Input error:dot1q vlanid<%d-%d> has member already be used%s",info.svlan.vlan_start,info.svlan.vlan_end,VTY_NEWLINE);
				QINQ_LOG_DBG("%s[%d]:leave %s:error:vlan<%d-%d> has vlan used\n",__FILE__,__LINE__,__FUNCTION__,info.svlan.vlan_start,info.svlan.vlan_end);
				return CMD_WARNING;
			}
		}
		if(qinq_check_vlan_range(&(p_vlan_mapping->cvlan_new), &(info.cvlan_new)))
		{
			if(qinq_check_vlan_range(&(p_vlan_mapping->svlan_new),&(info.svlan_new)))
			{
				/*action相同，且cvlan相同可以配置，否则失败*/
				if((qinq_check_action_same(p_vlan_mapping,&info))||
					(qinq_check_vlan_range_same(&(p_vlan_mapping->cvlan_new), &(info.cvlan_new))))
				{
					vty_error_out (vty, "Input error:svlan_new<%d-%d> has member already be used%s",info.svlan_new.vlan_start,info.svlan_new.vlan_end,VTY_NEWLINE);
					QINQ_LOG_DBG("%s[%d]:leave %s:error:svlan_new<%d-%d> has vlan used\n",__FILE__,__LINE__,__FUNCTION__,info.svlan_new.vlan_start,info.svlan_new.vlan_end);
					return CMD_WARNING;
				}
			}
		}
	}

	if( devtype == ID_HT157 || devtype == ID_HT158 )
	{
		ret = qinq_add(p_l2if, &info, 5, 1);
	}
	else 
	{
		ret = qinq_add(p_l2if, &info, 100, 1);
	}

	if(ret != ERRNO_SUCCESS)
	{
		if(ret==ERRNO_OVERSIZE)
		{
			/*for print configured conversion*/
			for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.qinq_list, p_listnode, p_vlan_mapping))
			{  
				if(info.svlan_new.vlan_start==p_vlan_mapping->svlan_new.vlan_start)
				{   
					if(info.cvlan_new.vlan_start==p_vlan_mapping->cvlan_new.vlan_start)
					{   
						vty_error_out (vty, "Vlan-mapping: chip resource is full,partial translate configured:vlan-mapping dot1q %d to %d translate %d %s"
                    ,p_vlan_mapping->svlan.vlan_start,p_vlan_mapping->svlan.vlan_end, p_vlan_mapping->svlan_new.vlan_start, VTY_NEWLINE);
						return CMD_WARNING;
					}
				}
			}
			vty_error_out (vty, "Fail to set vlan-mapping:chip resource is full.%s", VTY_NEWLINE);
		}
		else
		{
			QINQ_LOG_DBG("%s[%d]:%s:fail to set vlan-mapping\n",__FILE__,__LINE__, __FUNCTION__);
        	vty_error_out(vty, "Hal return failed ! %s", VTY_NEWLINE);
		}
	    return CMD_WARNING;
	}

	return CMD_SUCCESS;
}

void l2if_qinq_cli_init (void)
{
	qinq_device_type_init();
	install_element (PHYSICAL_IF_NODE, &no_dot1q_tunnel_svlan_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &dot1q_tunnel_svlan_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_vlan_mapping_qinq_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &vlan_mapping_qinq_cvlans_delete_svlan_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &vlan_mapping_qinq_scvlans_to_svlan_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &vlan_mapping_qinq_scvlan_to_scvlan_cmd_1, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &vlan_mapping_qinq_scvlan_to_scvlan_cmd_2, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &vlan_mapping_qinq_scvlan_to_scvlan_cmd_3, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_vlan_mapping_svlan_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &vlan_mapping_svlan_to_scvlan_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &vlan_mapping_add_svlan_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &vlan_mapping_svlans_to_svlan_cmd, CMD_SYNC);
}
