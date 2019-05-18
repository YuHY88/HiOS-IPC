#include <string.h>
#include <unistd.h>
#include <lib/zassert.h>
#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/command.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/ifm_common.h>
#include <lib/linklist.h>
#include <lib/msg_ipc.h>
#include <lib/errcode.h>
#include <ifm/ifm.h>
#include "vlan.h"
#include "l2_if.h"
#include "qinq.h"
#include "l2_msg.h"


/*create linklist node , set PRM from cli*/
int qinq_add(struct l2if *p_l2if, struct vlan_mapping *pinfo,uint8_t send_range,uint8_t qinqtype)
{
	uint16_t i = 0;
	int ret = 0;
	uint16_t id_num = 0;
	uint16_t v_start = 0;
	uint16_t v_end = 0;
	uint16_t send_num = 1;
	uint8_t flag = 0;
	struct vlan_mapping t_vlan_mapping;
	struct vlan_mapping* p_vlan_mapping = NULL;
    
	QINQ_LOG_DBG("%s[%d]:debug %s:l2 if add:cvlan<%d-%d>,svlan<%d-%d>.\n", __FILE__, __LINE__, __func__,pinfo->cvlan.vlan_start, pinfo->cvlan.vlan_end, pinfo->svlan.vlan_start, pinfo->svlan.vlan_end );
	QINQ_LOG_DBG("%s[%d]:debug %s:l2 if add:newcvlan<%d-%d>,cvlan.cos=%d,newsvlan<%d-%d>,svlan.cos=%d.\n", __FILE__, __LINE__, __func__,pinfo->cvlan_new.vlan_start, pinfo->cvlan_new.vlan_end, pinfo->cvlan_cos, pinfo->svlan_new.vlan_start, pinfo->svlan_new.vlan_end, pinfo->svlan_cos);
	QINQ_LOG_DBG("%s[%d]:debug %s:l2 if add:cvlan act=%d,svlan act=%d.\n", __FILE__, __LINE__, __func__,pinfo->cvlan_act, pinfo->svlan_act);
    
	if(qinqtype == 0)
	{
		v_start = pinfo->cvlan.vlan_start;
		v_end = pinfo->cvlan.vlan_end;
	}
	else
	{
		v_start = pinfo->svlan.vlan_start;
		v_end = pinfo->svlan.vlan_end;
	}
    
	/*每次发送send_range个数目cvlan到hal*/
	id_num = v_end - v_start + 1;
    
	if(0 == id_num % send_range)
		send_num = id_num / send_range;
	else
		send_num = (id_num / send_range) + 1;
    
	memset(&t_vlan_mapping, 0, sizeof(struct vlan_mapping));
	memcpy(&t_vlan_mapping, pinfo, sizeof(struct vlan_mapping));
    
	for(i=0; i<send_num; ++i)
	{
		/*将vlanid分拆，每次最多发送send_range个vlan到hal进行创建*/
		if(qinqtype == 0)
		{
			t_vlan_mapping.cvlan.vlan_start = v_start+ i*send_range;
			if(i == send_num - 1)
                t_vlan_mapping.cvlan.vlan_end = v_end;
			else
                t_vlan_mapping.cvlan.vlan_end = v_start+ (i + 1)*send_range - 1;
		}
		else
		{
			t_vlan_mapping.svlan.vlan_start = v_start+ i*send_range;
			if(i == send_num - 1)
				t_vlan_mapping.svlan.vlan_end = v_end;
			else
				t_vlan_mapping.svlan.vlan_end = v_start+ (i + 1)*send_range - 1;
		}	
		QINQ_LOG_DBG("%s[%d]:debug %s:send to hal add qinq:cvlan<%d-%d>,svlan<%d-%d>.\n", __FILE__, __LINE__, __func__,
                   t_vlan_mapping.cvlan.vlan_start, t_vlan_mapping.cvlan.vlan_end, t_vlan_mapping.svlan.vlan_start,t_vlan_mapping.svlan.vlan_end);
        
		ret = l2_msg_send_hal_wait_ack(&t_vlan_mapping, sizeof(struct vlan_mapping),1,MODULE_ID_HAL,MODULE_ID_L2,IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_QINQ,IPC_OPCODE_ADD,p_l2if->ifindex);
		if(0 != ret)
		{
			if(-6==ret)
			{
				zlog_err("%s[%d]: %s:error:Chip  resource is full !\n",__FILE__,__LINE__,__func__);
				flag = 6;
			}
			else
			{
				QINQ_LOG_DBG("%s[%d]: %s:error:fail to send qinq_add msg to hal\n",__FILE__,__LINE__,__func__);
				flag = 1;
			}
			break;
		}
		usleep(10000);
	}
	if(1 == flag)
	{
		qinq_delete(p_l2if, pinfo, send_range, qinqtype);
		return ERRNO_FAIL;
	}
	else if((6 == flag)&&(0!=i))
	{
		qinq_delete(p_l2if, &t_vlan_mapping, send_range, qinqtype);
		if(qinqtype == 0)
		{
			pinfo->cvlan.vlan_start=v_start;
			pinfo->cvlan.vlan_end=v_start+ i*send_range - 1;
		}
		else
		{
			pinfo->svlan.vlan_start=v_start;
			pinfo->svlan.vlan_end=v_start+ i*send_range - 1;
		}
		if((pinfo->svlan_act == VLAN_ACTION_DELETE)&&(pinfo->cvlan_act== VLAN_ACTION_NO))
		{
			pinfo->svlan_new.vlan_end=pinfo->cvlan.vlan_end;
		}
		else if((pinfo->svlan_act == VLAN_ACTION_TRANSLATE)&&(pinfo->cvlan_act ==VLAN_ACTION_NO))
		{
			pinfo->cvlan_new.vlan_end=pinfo->cvlan.vlan_end;
		}
		else if((pinfo->svlan_act == VLAN_ACTION_ADD)&&(pinfo->cvlan_act == VLAN_ACTION_NO))
		{
			pinfo->cvlan_new.vlan_end=pinfo->svlan.vlan_end;
		}
	}
     
	p_vlan_mapping = (struct vlan_mapping*)XCALLOC(MTYPE_L2_VLAN, sizeof(struct vlan_mapping));
	if(NULL == p_vlan_mapping)
	{
		zlog_err("%s[%d]:leave %s:error:fail to XCALLOC struct vlan_mapping\n",__FILE__,__LINE__,__func__);
		return ERRNO_FAIL;
	}

	memcpy(p_vlan_mapping, pinfo, sizeof(struct vlan_mapping));
	if(0!=i)
	{
		listnode_add (p_l2if->switch_info.qinq_list, (void *)p_vlan_mapping);
		QINQ_LOG_DBG("%s[%d]:debug %s:send to hal add qinq:cvlan<%d-%d>,svlan<%d-%d>.\n", __FILE__, __LINE__, __func__,
               pinfo->cvlan.vlan_start, pinfo->cvlan.vlan_end, pinfo->svlan.vlan_start,pinfo->svlan.vlan_end);
	}

	QINQ_LOG_DBG("%s[%d]: leave %s: sucessfully to add qinq\n", __FILE__, __LINE__, __func__);
	if(6 == flag)
	{
		return ERRNO_OVERSIZE;
	}
	else
	{
		return ERRNO_SUCCESS;
	}
    
}


int qinq_delete(struct l2if *p_l2if, struct vlan_mapping *pinfo,uint8_t send_range,uint8_t qinqtype)
{
	uint16_t i = 0;
	int ret = 0;
	uint16_t id_num = 0;
	uint16_t v_start = 0;
	uint16_t v_end = 0;
	uint16_t send_num = 1;
	struct vlan_mapping t_vlan_mapping;
	
	QINQ_LOG_DBG("%s[%d]:debug %s:delete qinq:cvlan<%d-%d>svlan<%d-%d>.\n", __FILE__, __LINE__, __func__,pinfo->cvlan.vlan_start, pinfo->cvlan.vlan_end, pinfo->svlan.vlan_start ,pinfo->svlan.vlan_end);
	QINQ_LOG_DBG("%s[%d]:debug %s:delete qinq:ncvlan<%d-%d>,c.cos=%d,nsvlan<%d-%d>,s.cos=%d.\n", __FILE__, __LINE__, __func__,pinfo->cvlan_new.vlan_start, pinfo->cvlan_new.vlan_end,pinfo->cvlan_cos, pinfo->svlan_new.vlan_start, pinfo->svlan_new.vlan_end ,pinfo->svlan_cos);
	QINQ_LOG_DBG("%s[%d]:debug %s:delete qinq:cvlan act=%d,svlan act=%d.\n", __FILE__, __LINE__, __func__,pinfo->cvlan_act,pinfo->svlan_act);

	if(qinqtype == 0)
	{
		v_start = pinfo->cvlan.vlan_start;
		v_end = pinfo->cvlan.vlan_end;
	}
	else
	{
		v_start = pinfo->svlan.vlan_start;
		v_end = pinfo->svlan.vlan_end;
	}
		
	/*每次发送send_range个vlan到hal*/
	id_num = v_end - v_start + 1;
	
	if(0 == id_num % send_range)
		send_num = id_num / send_range;
	else
		send_num = (id_num / send_range) + 1;
		
	memset(&t_vlan_mapping, 0, sizeof(struct vlan_mapping));
	memcpy(&t_vlan_mapping, pinfo, sizeof(struct vlan_mapping));

	for(i=0; i<send_num; ++i)
	{
		/*将vlanid分拆，每次最多发送send_range个数量vlan到hal进行创建*/
		if(qinqtype == 0)
		{
			t_vlan_mapping.cvlan.vlan_start = v_start+ i*send_range;
			if(i == send_num - 1)
				t_vlan_mapping.cvlan.vlan_end = v_end;
			else
				t_vlan_mapping.cvlan.vlan_end = v_start+ (i + 1)*send_range - 1;
		}
		else
		{
			t_vlan_mapping.svlan.vlan_start = v_start+ i*send_range;
			if(i == send_num - 1)
				t_vlan_mapping.svlan.vlan_end = v_end;
			else
				t_vlan_mapping.svlan.vlan_end = v_start+ (i + 1)*send_range - 1;
		}	
		QINQ_LOG_DBG("%s[%d]:debug %s:send to hal delete qinq:cvlan<%d-%d>,svlan<%d-%d>.\n", __FILE__, __LINE__, __func__,
			t_vlan_mapping.cvlan.vlan_start, t_vlan_mapping.cvlan.vlan_end, t_vlan_mapping.svlan.vlan_start,t_vlan_mapping.svlan.vlan_end);
		
		ret = l2_msg_send_hal_wait_ack(&t_vlan_mapping, sizeof(struct vlan_mapping), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF, L2IF_INFO_QINQ, IPC_OPCODE_DELETE, p_l2if->ifindex);
		if(0 != ret)
		{
			QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to send qinq_delete msg to hal\n",__FILE__,__LINE__,__func__);
			return ERRNO_FAIL;
		}
		else
		{
			QINQ_LOG_DBG("%s[%d]: leave %s: sucessfully to send no qinq to hal\n", __FILE__, __LINE__, __func__);
			
		}
		usleep(10000);
	}
	return ERRNO_SUCCESS;
}

/*create linklist node , set PRM from cli*/
int qinq_range_add(struct l2if *p_l2if, struct vlan_mapping *pinfo)
{
	int ret = 0;
	struct vlan_mapping t_vlan_mapping;
	struct vlan_mapping* p_vlan_mapping = NULL;
   
	QINQ_LOG_DBG("%s[%d]:debug %s:l2 if add:cvlan<%d-%d>,svlan<%d-%d>.\n", __FILE__, __LINE__, __func__,pinfo->cvlan.vlan_start, pinfo->cvlan.vlan_end, pinfo->svlan.vlan_start, pinfo->svlan.vlan_end );
	QINQ_LOG_DBG("%s[%d]:debug %s:l2 if add:newcvlan<%d-%d>,cvlan.cos=%d,newsvlan<%d-%d>,svlan.cos=%d.\n", __FILE__, __LINE__, __func__,pinfo->cvlan_new.vlan_start, pinfo->cvlan_new.vlan_end, pinfo->cvlan_cos, pinfo->svlan_new.vlan_start, pinfo->svlan_new.vlan_end, pinfo->svlan_cos);
	QINQ_LOG_DBG("%s[%d]:debug %s:l2 if add:cvlan act=%d,svlan act=%d.\n", __FILE__, __LINE__, __func__,pinfo->cvlan_act, pinfo->svlan_act);

	memset(&t_vlan_mapping, 0, sizeof(struct vlan_mapping));
	memcpy(&t_vlan_mapping, pinfo, sizeof(struct vlan_mapping));

	ret = l2_msg_send_hal_wait_ack(&t_vlan_mapping, sizeof(struct vlan_mapping),1,MODULE_ID_HAL,MODULE_ID_L2,IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_QINQ_RANGE,IPC_OPCODE_ADD,p_l2if->ifindex);
	if(-21==ret)
	{
		zlog_err("%s[%d]: %s:error:Chip  resource is full !\n",__FILE__,__LINE__,__func__);
		return ERRNO_OVERSIZE;
	}
	else if(ret)
	{
		QINQ_LOG_DBG("%s[%d]: %s:error:fail to send qinq_add msg to hal\n",__FILE__,__LINE__,__func__);
		return ERRNO_FAIL;
	}
		
	p_vlan_mapping = (struct vlan_mapping*)XCALLOC(MTYPE_L2_VLAN, sizeof(struct vlan_mapping));
	if(NULL == p_vlan_mapping)
	{
		zlog_err("%s[%d]:leave %s:error:fail to XCALLOC struct vlan_mapping\n",__FILE__,__LINE__,__func__);
		return ERRNO_FAIL;
	}

	memcpy(p_vlan_mapping, pinfo, sizeof(struct vlan_mapping));

	listnode_add (p_l2if->switch_info.qinq_list, (void *)p_vlan_mapping);
	QINQ_LOG_DBG("%s[%d]:debug %s:send to hal add qinq:cvlan<%d-%d>,svlan<%d-%d>.\n", __FILE__, __LINE__, __func__,
           pinfo->cvlan.vlan_start, pinfo->cvlan.vlan_end, pinfo->svlan.vlan_start,pinfo->svlan.vlan_end);


	QINQ_LOG_DBG("%s[%d]: leave %s: sucessfully to add qinq\n", __FILE__, __LINE__, __func__);
	return ERRNO_SUCCESS;
    
}


int qinq_range_delete(struct l2if *p_l2if, struct vlan_mapping *pinfo)
{
	int ret = 0;
	struct vlan_mapping t_vlan_mapping;
	
	QINQ_LOG_DBG("%s[%d]:debug %s:delete qinq:cvlan<%d-%d>svlan<%d-%d>.\n", __FILE__, __LINE__, __func__,pinfo->cvlan.vlan_start, pinfo->cvlan.vlan_end, pinfo->svlan.vlan_start ,pinfo->svlan.vlan_end);
	QINQ_LOG_DBG("%s[%d]:debug %s:delete qinq:ncvlan<%d-%d>,c.cos=%d,nsvlan<%d-%d>,s.cos=%d.\n", __FILE__, __LINE__, __func__,pinfo->cvlan_new.vlan_start, pinfo->cvlan_new.vlan_end,pinfo->cvlan_cos, pinfo->svlan_new.vlan_start, pinfo->svlan_new.vlan_end ,pinfo->svlan_cos);
	QINQ_LOG_DBG("%s[%d]:debug %s:delete qinq:cvlan act=%d,svlan act=%d.\n", __FILE__, __LINE__, __func__,pinfo->cvlan_act,pinfo->svlan_act);

	memset(&t_vlan_mapping, 0, sizeof(struct vlan_mapping));
	memcpy(&t_vlan_mapping, pinfo, sizeof(struct vlan_mapping));

	ret = l2_msg_send_hal_wait_ack(&t_vlan_mapping, sizeof(struct vlan_mapping), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF, L2IF_INFO_QINQ_RANGE, IPC_OPCODE_DELETE, p_l2if->ifindex);
	if(ret)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to send qinq_range_delete msg to hal\n",__FILE__,__LINE__,__func__);
		return ERRNO_FAIL;
	}
	else
	{
		QINQ_LOG_DBG("%s[%d]: leave %s: sucessfully to send no qinq to hal\n", __FILE__, __LINE__, __func__);
		
	}
	return ERRNO_SUCCESS;
}

int dot1q_tunnel_add(struct l2if *p_l2if, uint16_t vlan, uint8_t cos)
{
	struct vlan_mapping t_vlan_mapping;
	int ret;

	QINQ_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

	if(DEFAULT_VLAN_ID != p_l2if->switch_info.dot1q_tunnel)  
	{
		memset(&t_vlan_mapping, 0, sizeof(struct vlan_mapping));
		t_vlan_mapping.svlan_new.vlan_start = p_l2if->switch_info.dot1q_tunnel;
		t_vlan_mapping.svlan_new.vlan_end= p_l2if->switch_info.dot1q_tunnel;
		t_vlan_mapping.svlan_cos = p_l2if->switch_info.dot1q_cos;
		
		ret = l2_msg_send_hal_wait_ack(&t_vlan_mapping, sizeof(struct vlan_mapping),1,MODULE_ID_HAL,MODULE_ID_L2,IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_DOT1Q_TUNNEL,IPC_OPCODE_DELETE,p_l2if->ifindex);
		if(ret)
		{
			QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to send dot1q_tunnel_add msg to hal\n",__FILE__,__LINE__,__func__);
			return ERRNO_FAIL;
		}		
	}
	memset(&t_vlan_mapping, 0, sizeof(struct vlan_mapping));
	t_vlan_mapping.svlan_new.vlan_start = vlan;
	t_vlan_mapping.svlan_new.vlan_end= vlan;
	t_vlan_mapping.svlan_cos = cos;
	
	ret = l2_msg_send_hal_wait_ack(&t_vlan_mapping, sizeof(struct vlan_mapping),1,MODULE_ID_HAL,MODULE_ID_L2,IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_DOT1Q_TUNNEL,IPC_OPCODE_ADD,p_l2if->ifindex);
	if(ret)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to send dot1q_tunnel_add msg to hal\n",__FILE__,__LINE__,__func__);
		return ERRNO_FAIL;
	}
	else
	{
		p_l2if->switch_info.dot1q_tunnel = vlan;
		p_l2if->switch_info.dot1q_cos = cos;
		
		vlan_add_interface(vlan,vlan,p_l2if->ifindex,DOT1Q_TUNNEL);	
		QINQ_LOG_DBG("%s[%d]: leave %s: sucessfully to set dot1q trunel\n", __FILE__, __LINE__, __func__);
		return ERRNO_SUCCESS;
	}
	return ERRNO_SUCCESS;
}


int dot1q_tunnel_delete(struct l2if *p_l2if)
{
	struct vlan_mapping t_vlan_mapping;
	int ret = 0;

	QINQ_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);
	
	memset(&t_vlan_mapping, 0, sizeof(struct vlan_mapping));
	t_vlan_mapping.svlan_new.vlan_start = p_l2if->switch_info.dot1q_tunnel;	
	t_vlan_mapping.svlan_new.vlan_end= p_l2if->switch_info.dot1q_tunnel;	
	t_vlan_mapping.svlan_cos = p_l2if->switch_info.dot1q_cos;
		
	ret = l2_msg_send_hal_wait_ack(&t_vlan_mapping, sizeof(struct vlan_mapping),1,MODULE_ID_HAL,MODULE_ID_L2,IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_DOT1Q_TUNNEL,IPC_OPCODE_DELETE,p_l2if->ifindex);
	if(ret)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to send dot1q_tunnel_delete msg to hal\n",__FILE__,__LINE__,__func__);
		return ERRNO_FAIL;
	}
	else
	{
		vlan_delete_interface(p_l2if->switch_info.dot1q_tunnel,p_l2if->switch_info.dot1q_tunnel,p_l2if->ifindex);
		p_l2if->switch_info.dot1q_tunnel = DEFAULT_VLAN_ID;
		p_l2if->switch_info.dot1q_cos = 8;

		QINQ_LOG_DBG("%s[%d]: leave %s: sucessfully  to delete dot1q trunel\n", __FILE__, __LINE__, __func__);
		return ERRNO_SUCCESS;
	}
}

/*check if svlan | clan is used*/
int qinq_check_vlan_range(struct vlan_range *pvlan_old, struct vlan_range *pvlan_new)
{
	if((pvlan_new->vlan_start <= pvlan_old->vlan_end) && (pvlan_new->vlan_start >= pvlan_old->vlan_start))
		return ERRNO_FAIL;
	
	if((pvlan_new->vlan_end <= pvlan_old->vlan_end) && (pvlan_new->vlan_end >= pvlan_old->vlan_start))
		return ERRNO_FAIL;
	
	if((pvlan_new->vlan_start<= pvlan_old->vlan_end) && (pvlan_new->vlan_end >= pvlan_old->vlan_end))
		return ERRNO_FAIL;
	
	return ERRNO_SUCCESS;
}

/*check if svlan | clan is used*/
int qinq_check_vlan_range_same(struct vlan_range *pvlan_old, struct vlan_range *pvlan_new)
{
	if((pvlan_new->vlan_start == pvlan_new->vlan_start) && (pvlan_old->vlan_end >= pvlan_old->vlan_start))
		return ERRNO_SUCCESS;
	
	return ERRNO_FAIL;
}

int qinq_check_action_same(struct vlan_mapping *p_vlan_mapping1,struct vlan_mapping *p_vlan_mapping2)
{
	if((p_vlan_mapping1->svlan_act==p_vlan_mapping2->svlan_act)&&
		(p_vlan_mapping1->cvlan_act==p_vlan_mapping2->cvlan_act))
		return ERRNO_SUCCESS;

	return ERRNO_FAIL;
}


/* 删除接口所有 vlan */
int l2if_clear_switch_vlan(struct l2if *p_l2if)
{
	int i = 0;
	int ret = 0;
	uint16_t line_num = 0;
	uint32_t switch_vlan_count[4094][2];
	struct vlan_range send_hal;
	void *pdata = NULL;
	struct listnode *p_listnode = NULL;
	uint8_t flag=0;

	memset(switch_vlan_count,0,sizeof(switch_vlan_count));
	if(listhead(p_l2if->switch_info.vlan_list) != NULL)
	{
		switch_vlan_count[line_num][0] = (uint32_t)listgetdata(p_l2if->switch_info.vlan_list->head);
		switch_vlan_count[line_num][1] = (uint32_t)listgetdata(p_l2if->switch_info.vlan_list->head);

		for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.vlan_list, p_listnode, pdata))
		{
			if((uint32_t)pdata == (switch_vlan_count[line_num][1] - 1))
			{
				switch_vlan_count[line_num][1] = (uint32_t)pdata;
			}
			else if((uint32_t)pdata != switch_vlan_count[line_num][0])
			{
				line_num++;
				switch_vlan_count[line_num][0] = (uint32_t)pdata;
				switch_vlan_count[line_num][1] = (uint32_t)pdata;
			}	
		}
		
		/*输出switch vlan*/
		for(i=line_num; i>=0; i--)
		{
			send_hal.vlan_start = switch_vlan_count[i][1];
			send_hal.vlan_end = switch_vlan_count[i][0];
			ret = l2if_delete_vlan(p_l2if, &send_hal,flag);
			if(ret)
			{	
				QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to send clear switch vlan msg to hal\n",__FILE__,__LINE__,__func__);
				return ERRNO_FAIL;
			}
			else
			{
				QINQ_LOG_DBG("%s[%d]: %s:send to hal to delete switch vlan<%d-%d>.\n", __FILE__, __LINE__, __func__,send_hal.vlan_start,send_hal.vlan_end);
			}
		}
	}

	list_delete_all_node (p_l2if->switch_info.vlan_list);
	list_delete_all_node (p_l2if->switch_info.vlan_tag_list);/*edit by jhz,20180313,empty vlan_tag_list*/
	return ERRNO_SUCCESS;
}

int l2if_clear_qinq(struct l2if *p_l2if)
{
	int ret = 0;
	struct listnode *list_node = NULL;
	struct vlan_mapping *p_vlan_mapping = NULL;
	
	for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.qinq_list, list_node, p_vlan_mapping))
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:debug:delete qinq:cvlan<%d-%d>, cvlan_new<%d-%d>, svlan<%d-%d>, svlan_new<%d-%d>\n"
			,__FILE__,__LINE__,__func__,p_vlan_mapping->cvlan.vlan_start,p_vlan_mapping->cvlan.vlan_end
			,p_vlan_mapping->cvlan_new.vlan_start,p_vlan_mapping->cvlan_new.vlan_end
			,p_vlan_mapping->svlan.vlan_start,p_vlan_mapping->svlan.vlan_end
			,p_vlan_mapping->svlan_new.vlan_start,p_vlan_mapping->svlan_new.vlan_end);	
		if(p_vlan_mapping->range_qinq==L2IF_INFO_QINQ_RANGE)
		{
			ret= l2_msg_send_hal_wait_ack(p_vlan_mapping, sizeof(struct vlan_mapping), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF, (uint8_t)L2IF_INFO_QINQ_RANGE, IPC_OPCODE_DELETE, p_l2if->ifindex);
		}
		else
		{
			ret= l2_msg_send_hal_wait_ack(p_vlan_mapping, sizeof(struct vlan_mapping), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF, (uint8_t)L2IF_INFO_QINQ, IPC_OPCODE_DELETE, p_l2if->ifindex);
		}
		if(ret)
		{	
			QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to send clear switch vlan msg to hal\n",__FILE__,__LINE__,__func__);
			return ERRNO_FAIL;
		}					
	}
	list_delete_all_node (p_l2if->switch_info.qinq_list);
	return ERRNO_SUCCESS;
}


int l2if_set_switch_mode(struct l2if *p_l2if, enum SWITCH_MODE mode)
{
	int ret = 0;

	QINQ_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

	/*Edit by jhz,20180313,The three switch modes do not need to delete the access-vlan, 
		but they need to clear the switch vlan in the trunk mode and the hybrid mode, because tagged is not the same.*/
	/*检查模式是否切换，如果发生模式切换,先发送删除相应无法配置的信息*/
	/*hybrid->trunk || trunk->hybrid时，删除switch vlan*/
	if((SWITCH_MODE_HYBRID == p_l2if->switch_info.mode && SWITCH_MODE_TRUNK == mode)
		||(SWITCH_MODE_TRUNK == p_l2if->switch_info.mode && SWITCH_MODE_HYBRID == mode))
	{
		/*删除switch vlan*/
		if(NULL != p_l2if->switch_info.vlan_list->head)
			if(l2if_clear_switch_vlan(p_l2if) == ERRNO_FAIL)
				return ERRNO_FAIL;
	}
	/*trunk->access||hybrid->access,删除switch vlan，qinq，dot1q tunnel*/
	else if((SWITCH_MODE_TRUNK == p_l2if->switch_info.mode && SWITCH_MODE_ACCESS == mode)
		||(SWITCH_MODE_HYBRID == p_l2if->switch_info.mode && SWITCH_MODE_ACCESS == mode))
	{
		/*删除switch vlan*/
		if(NULL != p_l2if->switch_info.vlan_list->head)
			if(l2if_clear_switch_vlan(p_l2if) == ERRNO_FAIL)
				return ERRNO_FAIL;
		/*删除qinq*/
		if(NULL != p_l2if->switch_info.qinq_list->head)
			if(l2if_clear_qinq(p_l2if) == ERRNO_FAIL)
				return ERRNO_FAIL;
		/*删除dot1q tunnel*/
		if(DEFAULT_VLAN_ID != p_l2if->switch_info.dot1q_tunnel)
			if(dot1q_tunnel_delete(p_l2if) == ERRNO_FAIL)
				return ERRNO_FAIL;
	}

	ret = l2_msg_send_hal_wait_ack(&mode, sizeof(enum SWITCH_MODE), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF, (uint8_t)L2IF_INFO_MODE, IPC_OPCODE_ADD, p_l2if->ifindex);
	if(ret)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to send switch mode  msg to hal\n",__FILE__,__LINE__,__func__);
		return ERRNO_FAIL;
	}
	else
	{
		p_l2if->switch_info.mode = mode;
		QINQ_LOG_DBG("%s[%d]: leave %s: sucessfully to send ipc hal,mode = %d.\n", __FILE__, __LINE__, __func__,p_l2if->switch_info.mode);
		return ERRNO_SUCCESS;
	}	
}


int l2if_add_access_vlan(struct l2if *p_l2if, uint16_t vlanid)
{
	int ret = 0;
	int i,port = 0;
	
	QINQ_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);
	
	/*发送access vlanid给hal*/
	ret = l2_msg_send_hal_wait_ack(&vlanid, sizeof(uint16_t), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF, (uint8_t)L2IF_INFO_ACCESS_VLAN, IPC_OPCODE_ADD, p_l2if->ifindex);
	if(ret)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to send l2if_set_access_vlan msg to hal\n",__FILE__,__LINE__,__func__);
		return ERRNO_FAIL;
	}

	if( devtype == ID_HT157 || devtype == ID_HT158 )
	{
		if(IFM_TYPE_IS_TRUNK(p_l2if->ifindex) && !(IFM_TYPE_IS_SUBPORT(p_l2if->ifindex)))
		{
			for(i = 0; i < 3; i++)
			{
				if(trunk_reserve_vlan_table[i].trunkid == IFM_TRUNK_ID_GET(p_l2if->ifindex))
				{
					if(trunk_reserve_vlan_table[i].reserve_vlan_bak != p_l2if->switch_info.access_vlan)
					{
						vlan_delete_interface(p_l2if->switch_info.access_vlan,p_l2if->switch_info.access_vlan,p_l2if->ifindex);	
					}
				}
			}
		}
		else
		{
		    port = IFM_PORT_ID_GET ( p_l2if->ifindex );
		    if( port < 1 || port > 6 )
		    {
		        QINQ_LOG_DBG ("Invalid port number!\n");
		        return -1;
		    }
			if((DEF_RESV_VLAN1 + port - 1) != p_l2if->switch_info.access_vlan)
			{
				vlan_delete_interface(p_l2if->switch_info.access_vlan,p_l2if->switch_info.access_vlan,p_l2if->ifindex);	
			}
		}
	}
	else
	{
		if(DEFAULT_VLAN_ID != p_l2if->switch_info.access_vlan)  
		{
			vlan_delete_interface(p_l2if->switch_info.access_vlan,p_l2if->switch_info.access_vlan,p_l2if->ifindex);		
		}
	}
	
	vlan_add_interface(vlanid,vlanid,p_l2if->ifindex,UNTAGGED);/*edit by jhz,20180313,add vlan untagged*/

	p_l2if->switch_info.access_vlan = vlanid;
	QINQ_LOG_DBG("%s[%d]: leave %s: sucessfully set single data.\n", __FILE__, __LINE__, __func__);
	return ERRNO_SUCCESS;
}

int l2if_delete_access_vlan(struct l2if *p_l2if)
{
	int ret = 0;
	uint16_t vlanid = DEFAULT_VLAN_ID;
	
	if( devtype == ID_HT157 || devtype == ID_HT158 )
	{
		uint8_t port = 0;
		uint8_t i = 0;

		if(IFM_TYPE_IS_TRUNK(p_l2if->ifindex) && !(IFM_TYPE_IS_SUBPORT(p_l2if->ifindex)))
		{
			for(i = 0; i < 3; i++)
			{
				if(trunk_reserve_vlan_table[i].trunkid == IFM_TRUNK_ID_GET(p_l2if->ifindex))
				{
					vlanid = trunk_reserve_vlan_table[i].reserve_vlan_bak;
				}
			}
		}
		else
		{
			port = IFM_PORT_ID_GET ( p_l2if->ifindex );
			if( port < 1 || port > 6 )
			{
				zlog_err ("Invalid port number! \n");
				return -1;
			}

			switch(port)
			{
				case 1: vlanid = DEF_RESV_VLAN1; break;
				case 2: vlanid = DEF_RESV_VLAN2; break;
				case 3: vlanid = DEF_RESV_VLAN3; break;
				case 4: vlanid = DEF_RESV_VLAN4; break;
				case 5: vlanid = DEF_RESV_VLAN5; break;
				case 6: vlanid = DEF_RESV_VLAN6; break;
				default:break;	
			}
		}
	}
	
	QINQ_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);
	
	/*发送access vlanid给hal*/
	ret = l2_msg_send_hal_wait_ack(&vlanid, sizeof(uint16_t), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF, (uint8_t)L2IF_INFO_ACCESS_VLAN, IPC_OPCODE_ADD, p_l2if->ifindex);
	if(ret)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to send l2if_set_access_vlan msg to hal\n",__FILE__,__LINE__,__func__);
		return ERRNO_FAIL;
	}
	else
	{
		vlan_delete_interface(p_l2if->switch_info.access_vlan,p_l2if->switch_info.access_vlan,p_l2if->ifindex);
		p_l2if->switch_info.access_vlan = vlanid;
		QINQ_LOG_DBG("%s[%d]: leave %s: sucessfully set single data.\n", __FILE__, __LINE__, __func__);
		return ERRNO_SUCCESS;
	}
}

int l2if_delete_vlan(struct l2if *p_l2if, struct vlan_range *switch_vlan,uint8_t flag)
{
	int i = 0;
	int ret = 0;
	uint16_t id_num = 0;
	uint8_t send_num = 1;
	uint16_t del_vnum = 0;
	uint16_t vlanid = 0;
	struct vlan_range vlan_send;
	struct listnode *vlan_node = NULL;
	struct vlan_tagged *pvlan_tagged = NULL;
	void *data = NULL;
	uint16_t vlan_num = 0;

	QINQ_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

	del_vnum = switch_vlan->vlan_end - switch_vlan->vlan_start + 1;

	/*check if vlanid is already added*/
	for(vlanid=switch_vlan->vlan_start; vlanid<=switch_vlan->vlan_end; vlanid++)
	{
		for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.vlan_list, vlan_node, data))
		{
			if((uint32_t)data == vlanid)
				vlan_num ++;
		}
	}

	if(vlan_num != del_vnum)
		return ERRNO_NOT_FOUND;
	else
	{
		/*send 100 vlan each time to hal to delete*/
		id_num = switch_vlan->vlan_end- switch_vlan->vlan_start + 1;

		if(0 == id_num % 100)
			send_num = id_num / 100;
		else
			send_num = (id_num / 100) + 1;
		
		for(i=0; i<send_num; ++i)
		{
			memset(&vlan_send, 0, sizeof(struct vlan_range));	
			
			vlan_send.vlan_start = switch_vlan->vlan_start + i*100;
			if(i == send_num - 1)
				vlan_send.vlan_end = switch_vlan->vlan_end;
			else
				vlan_send.vlan_end = switch_vlan->vlan_start+ (i + 1)*100 - 1;
			
			ret = l2_msg_send_hal_wait_ack(&vlan_send, sizeof(struct vlan_range), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF, (uint8_t)L2IF_INFO_SWITCH_VLAN, IPC_OPCODE_DELETE, p_l2if->ifindex);
			if(ret)
			{
				QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to send l2if_delete_vlan msg to hal\n",__FILE__,__LINE__,__func__);
				return ERRNO_FAIL;
			}
            QINQ_LOG_DBG("%s[%d]: %s: send msg to hal delete switch vlan<%d-%d>\n", __FILE__, __LINE__, __func__,vlan_send.vlan_start,vlan_send.vlan_end);
		}
		if(0==flag)
		{
			vlan_delete_interface(switch_vlan->vlan_start,switch_vlan->vlan_end,p_l2if->ifindex);
		}
		else if(1==flag)
		{
			ipc_send_msg_n2(switch_vlan, sizeof(struct vlan_range), 1,MODULE_ID_FTM, MODULE_ID_L2, IPC_TYPE_VLAN, VLAN_INFO_PORT,IPC_OPCODE_DELETE,p_l2if->ifindex);
		}
		
		for(vlanid=switch_vlan->vlan_start; vlanid<=switch_vlan->vlan_end; vlanid++)
		{
			for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.vlan_list, vlan_node, data))
			{
				if((uint32_t)data == vlanid)			
				{
					list_delete_node (p_l2if->switch_info.vlan_list, vlan_node);
					break;
				}
			}
			/*Edit by jhz,20180313,empty vlan_tag_list*/
			for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.vlan_tag_list, vlan_node, pvlan_tagged))
			{
				if(pvlan_tagged->vlanid == vlanid)			
				{
					XFREE(MTYPE_L2_VLAN, pvlan_tagged);				
					list_delete_node (p_l2if->switch_info.vlan_tag_list, vlan_node);
					break;
				}
			}	
		}
	}
	
	QINQ_LOG_DBG("%s[%d]: leave %s: sucessfully delete vlan<%d - %d>.\n", __FILE__, __LINE__, __func__,switch_vlan->vlan_start,switch_vlan->vlan_end);
	return ERRNO_SUCCESS;
}

/*Edit by jhz,20180313,When you notify Hal to add a vlan, add untagged or untagged flags.*/
int l2if_add_vlan(struct l2if *p_l2if, struct vlan_range *switch_vlan,uint8_t tag)
{
	int i = 0;
	int ret = 0;
	uint32_t vlanid = 0;
	uint16_t id_num = 0;
	uint8_t send_num = 1;
	uint8_t flag = 0;
	struct vlan_range vlan_send;
	struct vlan_tagged *pvlan_tagged=NULL;
	uint8_t subtype=0;

	if(tag==TAGGED)
	{
		subtype=L2IF_INFO_SWITCH_VLAN;
	}
	else if(tag==UNTAGGED)
	{
	//	subtype=L2IF_INFO_SWITCH_VLAN;
		subtype=L2IF_INFO_SWITCH_UNTAG_VLAN;
	}

	/*每次发送100个vlan到hal创建*/
	id_num = switch_vlan->vlan_end - switch_vlan->vlan_start + 1;
	
	if(0 == id_num % 100)
		send_num = id_num / 100;
	else
		send_num = (id_num / 100) + 1;
	
	QINQ_LOG_DBG("%s[%d]: Entering function '%s'.v_start=%d,v_end=%d,idnum = %d,send_num=%d\n", __FILE__, __LINE__, __func__
		,switch_vlan->vlan_start,switch_vlan->vlan_end,id_num,send_num);
	
	for(i=0; i<send_num; ++i)
	{
		/*将vlanid分拆，每次最多发送100个vlan到hal进行创建*/
		memset(&vlan_send, 0, sizeof(struct vlan_range));	
		
		vlan_send.vlan_start = switch_vlan->vlan_start+ i*100;
		if(i == send_num - 1)
			vlan_send.vlan_end = switch_vlan->vlan_end;
		else
			vlan_send.vlan_end = switch_vlan->vlan_start+ (i + 1)*100 - 1;

		QINQ_LOG_DBG("%s[%d]: leave %s: send msg to hal add switch vlan<%d-%d>\n", __FILE__, __LINE__, __func__,vlan_send.vlan_start,vlan_send.vlan_end);
		/*send vlan msg to hal*/	
		ret= l2_msg_send_hal_wait_ack(&vlan_send, sizeof(struct vlan_range), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF, subtype, IPC_OPCODE_ADD, p_l2if->ifindex);
		if(ret)
		{
			QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to send l2if_add_vlan msg to hal\n",__FILE__,__LINE__,__func__);
			flag = 1;
			break;
		}	
	}

	/*hal添加switch vlan失败，则删除所有已添加的vlan*/
	if(1 == flag)
	{
		/*send 100 vlan each time to hal to delete*/
		id_num = switch_vlan->vlan_end- switch_vlan->vlan_start + 1;

		if(0 == id_num % 100)
			send_num = id_num / 100;
		else
			send_num = (id_num / 100) + 1;
		
		for(i=0; i<send_num; ++i)
		{
			memset(&vlan_send, 0, sizeof(struct vlan_range));	
			
			vlan_send.vlan_start = switch_vlan->vlan_start+ i*100;
			if(i == send_num - 1)
				vlan_send.vlan_end = switch_vlan->vlan_end;
			else
				vlan_send.vlan_end = switch_vlan->vlan_start+ (i + 1)*100 - 1;
			
			ret = l2_msg_send_hal_wait_ack(&vlan_send, sizeof(struct vlan_range), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF, (uint8_t)L2IF_INFO_SWITCH_VLAN, IPC_OPCODE_DELETE, p_l2if->ifindex);
			if(ret)
			{
				QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to add all switch vlan and fail to delete it\n",__FILE__,__LINE__,__func__);
				return ERRNO_FAIL;
			}
			QINQ_LOG_DBG("%s[%d]: %s: fail to add all switch vlan ,sucessfully to msg to hal to  delete<%d-%d>\n", __FILE__, __LINE__, __func__,vlan_send.vlan_start,vlan_send.vlan_end);
		}
		QINQ_LOG_DBG("%s[%d]: %s: fail to add all switch vlan ,sucessfully to msg to hal to  delete it all <%d-%d>\n", __FILE__, __LINE__, __func__,switch_vlan->vlan_start,switch_vlan->vlan_end);
		return ERRNO_FAIL;
	}
	else
	{
		/*add vlan list node,and set PRM get from cli*/
		for(vlanid=switch_vlan->vlan_start; vlanid<=switch_vlan->vlan_end; vlanid++)
		{
			listnode_add_sort (p_l2if->switch_info.vlan_list, (void*)vlanid);
			/*Edit by jhz,20180313,Vlan_tag_list adds member vlan.*/
			pvlan_tagged=(struct vlan_tagged*)XCALLOC(MTYPE_L2_VLAN, sizeof(struct vlan_tagged));
			pvlan_tagged->vlanid=vlanid;
			pvlan_tagged->tag=tag;
			listnode_add_sort(p_l2if->switch_info.vlan_tag_list, (void*)pvlan_tagged);	
		}
		
		vlan_add_interface(switch_vlan->vlan_start,switch_vlan->vlan_end,p_l2if->ifindex,tag);

		QINQ_LOG_DBG("%s[%d]: leave %s: sucessfully to set vlan<%d-%d>\n", __FILE__, __LINE__, __func__,switch_vlan->vlan_start,switch_vlan->vlan_end);
		return ERRNO_SUCCESS;
	}
}

/*Add by jhz,20180313,Used to notify Hal to add a vlan.*/
int l2if_vlan_send_hal(uint32_t switch_vlan_count[][2],uint16_t line_num,uint32_t ifindex,uint8_t tag, enum SWITCH_MODE mode)
{
	struct vlan_range send_hal;
	struct vlan_range vlan_send;
	uint8_t send_num = 0;
	uint16_t id_num = 0;
	int i = 0;
	int j=0;
	int ret=0;
	int flag=0;
	uint8_t subtype=0;

	if(tag==TAGGED)
	{
		subtype=L2IF_INFO_SWITCH_VLAN;
	}
	else if(tag==UNTAGGED)
	{
	//	subtype=L2IF_INFO_SWITCH_VLAN;
		subtype=L2IF_INFO_SWITCH_UNTAG_VLAN;
	}
	
	memset(&send_hal, 0, sizeof(struct vlan_range));
	/*输出switch vlan*/
	for(i=0; i<=line_num; i++)
	{	
        if(mode == SWITCH_MODE_TRUNK)
        {
            send_hal.vlan_start = switch_vlan_count[i][1];
            send_hal.vlan_end = switch_vlan_count[i][0];
        }
        else
        {
            send_hal.vlan_start = switch_vlan_count[i][0];
    		send_hal.vlan_end = switch_vlan_count[i][1];
        }
		
		id_num = send_hal.vlan_end- send_hal.vlan_start + 1;
		if(0 == id_num % 100)
			send_num = id_num / 100;
		else
			send_num = (id_num / 100) + 1;
		
		for(j=0; j<send_num; ++j)
		{
			memset(&vlan_send, 0, sizeof(struct vlan_range));				
			vlan_send.vlan_start = send_hal.vlan_start + j*100;
			if(j== send_num - 1)
				vlan_send.vlan_end = send_hal.vlan_end;
			else
				vlan_send.vlan_end = send_hal.vlan_start+ (j + 1)*100 - 1;
			
			QINQ_LOG_DBG("%s[%d]:%s:l2if_add_vlan vlan:%d-%d \n",__FILE__,__LINE__,__func__,
				vlan_send.vlan_start,vlan_send.vlan_end);
			ret = l2_msg_send_hal_wait_ack(&vlan_send, sizeof(struct vlan_range), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF, subtype, IPC_OPCODE_ADD, ifindex);
			if(ret)
			{
				QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to send l2if_add_vlan msg to hal\n",__FILE__,__LINE__,__func__);
				flag = 1;
				break;
			}
		}
	}	

	/*hal添加switch vlan失败，则删除所有已添加的vlan*/
	if(1 == flag)
	{
		for(i=0; i<j; ++i)
		{
			memset(&vlan_send, 0, sizeof(struct vlan_range));	
			
			vlan_send.vlan_start = send_hal.vlan_start+ i*100;
			if(i == send_num - 1)
				vlan_send.vlan_end = send_hal.vlan_end;
			else
				vlan_send.vlan_end = send_hal.vlan_start+ (i + 1)*100 - 1;
			
			ret = l2_msg_send_hal_wait_ack(&vlan_send, sizeof(struct vlan_range), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF, (uint8_t)L2IF_INFO_SWITCH_VLAN, IPC_OPCODE_DELETE, ifindex);
			if(ret)
			{
				QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to add all switch vlan and fail to delete it\n",__FILE__,__LINE__,__func__);
				return -1;
			}
		}
		return -1;
	}
	return 0;
}

/*Add by jhz,20180317,Used to send tags to Hal to modify the vlan tag.*/
int l2if_vlan_send_hal_up_tagged(uint32_t switch_vlan_count[][2],uint16_t line_num,struct l2if *p_l2if,uint8_t tag)
{
	struct vlan_range send_hal;
	int i = 0;
	int ret=0;
	int flag=0;

	/*输出switch vlan*/
	for(i=0; i<=line_num; i++)
	{	
		send_hal.vlan_start = switch_vlan_count[i][0];
		send_hal.vlan_end = switch_vlan_count[i][1];

		ret = l2if_delete_vlan(p_l2if,&send_hal,flag);
		if(ERRNO_NOT_FOUND == ret)
		{
			QINQ_LOG_DBG("%s[%d]:%s:no switch vlan<%d-%d> failed,vlan input error\n",__FILE__,__LINE__,__FUNCTION__, 
					send_hal.vlan_start, send_hal.vlan_end);
			return ret;
		}
		else if(ERRNO_FAIL == ret)
		{
			QINQ_LOG_DBG("%s[%d]:%s:fail to find switch vlan<%d-%d>,Hal return failed !\n",__FILE__,__LINE__,__FUNCTION__, 
					send_hal.vlan_start, send_hal.vlan_end);
			return ret;
		}
					
		ret = l2if_add_vlan(p_l2if, &send_hal,tag);
		if(ret)
		{
			return ret;
		}		
	}	

	return ret;
}

int l2if_clear_storm_suppress(struct l2if *pif)
{	
	struct ifm_switch ifm_switch_t;
	int ret=0;

	memset(&ifm_switch_t,0,sizeof(struct ifm_switch));
	ifm_switch_t.storm_control.storm_unicast=DISABLE;
	ifm_switch_t.storm_control.storm_broadcast=DISABLE;
	ifm_switch_t.storm_control.storm_multicast=DISABLE;
	ifm_switch_t.storm_control.unicast_rate=0;
	ifm_switch_t.storm_control.broadcast_rate=0;
	ifm_switch_t.storm_control.multicast_rate=0;	

	ret=l2_msg_send_hal_wait_ack(&ifm_switch_t,sizeof(struct ifm_switch), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_STORM_SUPRESS, IPC_OPCODE_UPDATE, pif->ifindex);
	if(ret)
	{
		QINQ_LOG_DBG("%s[%d]:leave %s:error:fail to send storm suppress msg to hal\n",__FILE__,__LINE__,__FUNCTION__);
		return CMD_WARNING;
	}

	pif->switch_info.storm_control.storm_unicast=DISABLE;
	pif->switch_info.storm_control.storm_broadcast=DISABLE;
	pif->switch_info.storm_control.storm_multicast=DISABLE;		
	pif->switch_info.storm_control.unicast_rate=0;
	pif->switch_info.storm_control.broadcast_rate=0;
	pif->switch_info.storm_control.multicast_rate=0;
	return ret;
}

int l2if_get_vlan_mapping(struct l2if_vlan_mapping *key_entry,struct l2if_vlan_mapping entry_buff[],int data_num)
{
	struct l2if *p_l2if = NULL;
	struct listnode *p_listnode = NULL;
	struct vlan_mapping *p_vlan_mapping = NULL;
	struct l2if_vlan_mapping t_l2if_vlan_mapping;
	int msg_num  = IPC_MSG_LEN/sizeof(struct l2if_vlan_mapping);
	uint32_t ifindex;
	int flag=0;

	ifindex=key_entry->ifindex;
	p_l2if = l2if_get(ifindex);
	if(NULL == p_l2if)
	{
		return 0;
	}
	if(NULL==p_l2if->switch_info.qinq_list)
	{
		return 0;
	}
	else
	{
		/*The first query*/
		if((0==key_entry->t_vlan_mapping.svlan.vlan_start)&&(0==key_entry->t_vlan_mapping.svlan.vlan_end))
		{
			for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.qinq_list, p_listnode, p_vlan_mapping))
			{
				t_l2if_vlan_mapping.ifindex=ifindex;
				memcpy(&t_l2if_vlan_mapping.t_vlan_mapping,p_vlan_mapping,sizeof(struct vlan_mapping));
				memcpy(&entry_buff[data_num++],&t_l2if_vlan_mapping,sizeof(struct l2if_vlan_mapping));
				
				if (data_num == msg_num)
				{
				 	return data_num;
				}
			}
			return data_num;
		}
		else
		{
			for(ALL_LIST_ELEMENTS_RO(p_l2if->switch_info.qinq_list, p_listnode, p_vlan_mapping))
			{
				if(0==flag)
				{
					if((key_entry->t_vlan_mapping.svlan.vlan_start==p_vlan_mapping->svlan.vlan_start)&&\
						(key_entry->t_vlan_mapping.svlan.vlan_end==p_vlan_mapping->svlan.vlan_end)&&\
						(key_entry->t_vlan_mapping.cvlan.vlan_start==p_vlan_mapping->cvlan.vlan_start)&&\
						(key_entry->t_vlan_mapping.cvlan.vlan_end==p_vlan_mapping->cvlan.vlan_end))
					{
						flag=1;
					}
					continue;
				}
				else
				{
					t_l2if_vlan_mapping.ifindex=ifindex;
					memcpy(&t_l2if_vlan_mapping.t_vlan_mapping,p_vlan_mapping,sizeof(struct vlan_mapping));
					memcpy(&entry_buff[data_num++],&t_l2if_vlan_mapping,sizeof(struct l2if_vlan_mapping));
					if (data_num == msg_num)
					{
						flag=0;
					 	return data_num;
					}
				}
			}
			flag=0;
			return data_num;
		}
		
	}	
	return 0;
}

int l2if_get_vlan_mapping_bulk( struct l2if_vlan_mapping *key_entry,struct l2if_vlan_mapping entry_buff[])
{
	int msg_num  = IPC_MSG_LEN/sizeof(struct l2if_vlan_mapping);
	struct l2if_vlan_mapping t_l2if_vlan_mapping;
	uint32_t *ifindex_count=NULL;
	uint16_t ifindex_total=0;
	int data_num=0;
	int i=0;
	int j=0;

	ifindex_count=l2if_get_ifindex_list(&ifindex_total);
	if(0==ifindex_total)
	{
		return 0;
	}
	
	memset(&t_l2if_vlan_mapping, 0, sizeof(struct l2if_vlan_mapping));
	
	/*The first query*/
	if ( 0 == key_entry->ifindex)
	{
		while(data_num<msg_num)
		{
			if(i==ifindex_total)/*接口数组越界*/
				break;
			t_l2if_vlan_mapping.ifindex=ifindex_count[i++];
			data_num=l2if_get_vlan_mapping(&t_l2if_vlan_mapping,entry_buff,data_num);
		}
		
		return data_num;
	}
	else
	{
		for(i=0;i<ifindex_total;i++)
		{
			if(key_entry->ifindex==ifindex_count[i])/*找到上次查询的接口*/
			{
				data_num=l2if_get_vlan_mapping(key_entry,entry_buff,data_num);/*上次是否有未查询完成部分*/
				j=i+1;
				break;
			}
		}
		
		while(data_num<msg_num)
		{
			if(j==ifindex_total)/*接口数组越界*/
				break;
			t_l2if_vlan_mapping.ifindex=ifindex_count[j++];
			data_num=l2if_get_vlan_mapping(&t_l2if_vlan_mapping,entry_buff,data_num);
		}
		return data_num;	
		
	}
	return 0;
}

/*批量回应qinq_list查询*/
int l2if_reply_vlan_mapping_bulk(void *pdata,struct ipc_msghdr_n *phdr)
{
	struct l2if_vlan_mapping *key_entry=(struct l2if_vlan_mapping *)pdata;
	int msg_num  = IPC_MSG_LEN/sizeof(struct l2if_vlan_mapping);
	struct l2if_vlan_mapping entry_buff[IPC_MSG_LEN/sizeof(struct l2if_vlan_mapping)];
	int ret;

	memset(entry_buff, 0, msg_num*sizeof(struct l2if_vlan_mapping));
	
	ret=l2if_get_vlan_mapping_bulk(key_entry,entry_buff);
	if(ret>0)
	{
	
		ret = ipc_send_reply_n2(entry_buff, ret*sizeof(struct l2if_vlan_mapping), ret, phdr->sender_id,
	                        phdr->module_id, phdr->msg_type, phdr->msg_subtype, 0,phdr->msg_index, IPC_OPCODE_REPLY);
	}
	else
	{
		//ret = l2_msg_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, phdr->module_id, phdr->msg_type,
	          //              phdr->msg_subtype, phdr->msg_index);
		ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id,
	                        phdr->module_id, phdr->msg_type, phdr->msg_subtype, 0,0, IPC_OPCODE_NACK);
	}
	return 0;
}

