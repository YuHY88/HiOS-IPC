#include <string.h>
#include <lib/zassert.h>
#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/command.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/ifm_common.h>
#include <lib/linklist.h>
#include <lib/errcode.h>
#include <ifm/ifm.h>
#include "vlan.h"
#include "qinq.h"
#include "l2_if.h"



struct vlan_entry *vlan_table[VLAN_MAX];

/************************************************
 * Add by jhz,20180313
 * Function: tagged_portlist_sort
 * Description:
 *      Compare functions for list sorting.
 ************************************************/
int tagged_portlist_sort(void* newif, void* oldif)
{
	if(((struct port_tagged*)newif)->ifindex < ((struct port_tagged*)oldif)->ifindex)
		return -1;
	else
		return 0;
}

/************************************************
 * Add by jhz,20180313
 * Function: portlist_sort
 * Description:
 *      Compare functions for list sorting.
 ************************************************/
int portlist_sort(void* newif, void* oldif)
{
	if((uint32_t)newif < (uint32_t)oldif)
		return -1;
	else
		return 0;
}

int vlan_table_init()
{
	int vlanid = 0;	
	int v_start=0,v_end=0;
	int id_num,send_num = 0;
	int i=0,j=0,k=0;
	int size=0,range_size;
	int vlan_info_size=sizeof(struct vlan_info);
	uchar vlan_info_buff[100*vlan_info_size];
	
	VLAN_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

	/* 初始化创建 4096 个 vlan，并下发 hal */
	for(vlanid = 0; vlanid < VLAN_MAX; vlanid++)
	{
		vlan_table[vlanid] = (struct vlan_entry *)XCALLOC(MTYPE_L2_VLAN,sizeof(struct vlan_entry));
		vlan_table[vlanid]->vlan.enable = DISABLE;
		vlan_table[vlanid]->vlan.vlanid = vlanid;
		vlan_table[vlanid]->vlan.mac_learn= ENABLE;
		vlan_table[vlanid]->vlan.limit_num= 0;
		vlan_table[vlanid]->vlan.limit_action= DISABLE;
		vlan_table[vlanid]->vlan.storm_unicast= DISABLE;	
		vlan_table[vlanid]->vlan.storm_broadcast= DISABLE;	
		vlan_table[vlanid]->vlan.storm_multicast= DISABLE;	
		vlan_table[vlanid]->portlist.cmp = portlist_sort;/*Add by jhz,20180313,init portlist and port_tag_list */
		vlan_table[vlanid]->port_tag_list.cmp = tagged_portlist_sort;
	}
	v_start=1;
	v_end=VLAN_MAX-1;
	id_num = v_end - v_start + 1;
	send_num = (id_num / 100) + 1;
	for(i = 0; i < send_num; i++)
	{
		k=0;
		memset(vlan_info_buff, 0, 100*vlan_info_size);	
		vlanid= v_start + i*100;
		if(i==(send_num-1))
		{
			size=(v_end-i*100)*vlan_info_size;
			range_size=v_end-i*100-v_start+1;
		}
		else
		{
			size=100*vlan_info_size;
			range_size=100;
		}
		
		for(j=vlanid;j<vlanid+range_size;j++)
		{
			memcpy(&vlan_info_buff[k], (&(vlan_table[j]->vlan)), vlan_info_size);
			k+=vlan_info_size;
		}
		
		ipc_send_msg_n2(vlan_info_buff, size, range_size, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_VLAN, 0, IPC_OPCODE_ADD, 0);
		
	}
	VLAN_LOG_DBG("%s[%d]:leave %s:sucessfully to init vlan_table[]\n",__FILE__,__LINE__,__func__);
    return 0;
}


int vlan_create(uint16_t v_start,uint16_t v_end)
{
	uint16_t vlanid = 0;
	struct vlan_range vlan;

	/*上层进行vlan 使能*/
	for(vlanid=v_start; vlanid<=v_end; ++vlanid)
	{
		vlan_table[vlanid]->vlan.enable = ENABLE;
	}

	vlan_set_info(v_start,v_end, NULL,NULL,NULL, VLAN_INFO_ENABLE);	
	vlan.vlan_start=v_start;
	vlan.vlan_end=v_end;
	ipc_send_msg_n2(&vlan, sizeof(struct vlan_range),1,MODULE_ID_FTM, MODULE_ID_L2, IPC_TYPE_VLAN,VLAN_INFO_ENABLE,IPC_OPCODE_ADD,ENABLE);	
	VLAN_LOG_DBG("%s[%d]:leave %s:sucessfully to create vlan_table[%d]\n",__FILE__,__LINE__,__func__,vlanid);
	return ERRNO_SUCCESS;
}


int vlan_delete(uint16_t v_start,uint16_t v_end)
{
	struct vlan_range vlan;
	struct listnode *p_listnode = NULL;
	void *data = NULL;
	uint32_t *ifindex_count=NULL;
	uint32_t limit_num = 0;
	uint32_t ifindex = 0;
	uint16_t ifindex_total=0;
	uint16_t vlanid = 0;
	uint8_t ifindex_flag[150];
	uint8_t learn_num = ENABLE;
	uint8_t supress_num =DISABLE;
	uchar test_name = '\0';
	int i=0;
	struct port_tagged *pvlan_tagged=NULL;
	
	memset(ifindex_flag,0,sizeof(ifindex_flag));
	ifindex_count=l2if_get_ifindex_list(&ifindex_total);

	for(vlanid=v_start; vlanid<=v_end; ++vlanid)
	{
		vlan_table[vlanid]->vlan.enable = DISABLE;
		
		/*删除vlan时，vlan属性恢复至默认值*/
		if(ENABLE != vlan_table[vlanid]->vlan.mac_learn)
		{
			if(vlan_set_info(vlanid,vlanid, &learn_num,NULL,NULL, VLAN_INFO_MAC_LEARN))
			{
				VLAN_LOG_DBG("%s[%d]:%s:vlanid=%d  set  mac learn  error\n",__FILE__,__LINE__,__func__,vlanid);
			}
		}
		if(0 != vlan_table[vlanid]->vlan.limit_num)
		{
			if(vlan_set_info(vlanid,vlanid, &limit_num, NULL,NULL,VLAN_INFO_MAC_LIMIT))
			{
				VLAN_LOG_DBG("%s[%d]:%s:vlanid=%d  set  mac limit  error\n",__FILE__,__LINE__,__func__,vlanid);
			}
		}
		if((DISABLE != vlan_table[vlanid]->vlan.storm_unicast)||(DISABLE != vlan_table[vlanid]->vlan.storm_broadcast)||
			(DISABLE != vlan_table[vlanid]->vlan.storm_multicast))
		{
			if(vlan_set_info(vlanid,vlanid, &supress_num,&supress_num,&supress_num, VLAN_INFO_STORM_SUPRESS))
			{
				VLAN_LOG_DBG("%s[%d]:%s:vlanid=%d  unicast storm suppress error\n",__FILE__,__LINE__,__func__,vlanid);
			}
		}
		if(strcmp((const char *)&test_name,(const char *)vlan_table[vlanid]->name))
		{
			vlan_table[vlanid]->name[0] = '\0';
		}
		if(NULL!=(&(vlan_table[vlanid]->portlist)))
		{
			for(ALL_LIST_ELEMENTS_RO(&(vlan_table[vlanid]->portlist),p_listnode,data))
			{
				if(NULL==ifindex_count)
				{
					break;
				}
				ifindex=(uint32_t) data;
				/*通过标记接口，每个接口只操作一次*/
				for(i=0;i<ifindex_total;i++)
				{
					if(ifindex==ifindex_count[i])
					{
						if((uint8_t)DISABLE==ifindex_flag[i])
						{
							l2if_delete_range_switch_vlan(ifindex,v_start,v_end);
							ifindex_flag[i]=(uint8_t)ENABLE;
						}
						break;
					}
				}
			}
			for(ALL_LIST_ELEMENTS_RO(&(vlan_table[vlanid]->port_tag_list),p_listnode,pvlan_tagged))
			{
				if(pvlan_tagged->tag==DOT1Q_TUNNEL)
				{
					l2if_delete_dot1q_tunnel_by_vlan(pvlan_tagged->ifindex ,vlanid); /*清除端口下配置的dot1q tunnel*/
				}
			}
			list_delete_all_node (&(vlan_table[vlanid]->portlist));	/*清空vlan的port_list*/
			list_delete_all_node (&(vlan_table[vlanid]->port_tag_list));	/*清空vlan的port_tag_list*/
		}
	}

	vlan_set_info(v_start,v_end, NULL,NULL,NULL, VLAN_INFO_ENABLE);	

	vlan.vlan_start=v_start;
	vlan.vlan_end=v_end;
	ipc_send_msg_n2(&vlan, sizeof(struct vlan_range),1,MODULE_ID_FTM, MODULE_ID_L2, IPC_TYPE_VLAN,VLAN_INFO_ENABLE,IPC_OPCODE_DELETE,DISABLE);	
	VLAN_LOG_DBG("%s[%d]:%s:vlan_table[%d] delete successfully\n",__FILE__,__LINE__,__func__,vlanid);
	return ERRNO_SUCCESS;
}


int vlan_set_info(uint16_t v_start,uint16_t v_end, void *pinfo1, void *pinfo2,void *pinfo3,enum VLAN_INFO type)	
{
	struct vlan_info vlan_info_send;
	int vlanid =v_start;	
	int id_num=0,send_num = 0,remain_num=0;
	int i=0,j=0,k=0;
	int data_len=0,data_num;
	int vlan_info_size=sizeof(struct vlan_info);
	uchar vlan_info_buff[100*vlan_info_size];
	
	VLAN_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

	memset(&vlan_info_send, 0, sizeof(struct vlan_info));
	switch(type)
	{
		case VLAN_INFO_MAC_LEARN:
			vlan_info_send.vlanid = vlanid;
			vlan_info_send.mac_learn = *(uint8_t*)pinfo1;
			ipc_send_msg_n2(&vlan_info_send,vlan_info_size, 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_VLAN, (uint8_t)type, IPC_OPCODE_UPDATE, 0);
			break;
		case VLAN_INFO_MAC_LIMIT:
			vlan_info_send.vlanid = vlanid;	
			vlan_info_send.limit_num= *(uint32_t*)pinfo1;
			if(pinfo2!=NULL)
			{
				vlan_info_send.limit_action= *(uint8_t*)pinfo2;
			}
			ipc_send_msg_n2(&vlan_info_send,vlan_info_size, 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_VLAN, (uint8_t)type, IPC_OPCODE_UPDATE, 0);
			break;
		case VLAN_INFO_STORM_SUPRESS:
			vlan_info_send.vlanid = vlanid;	
			vlan_info_send.storm_unicast= *(uint8_t*)pinfo1;
			vlan_info_send.storm_broadcast= *(uint8_t*)pinfo2;
			vlan_info_send.storm_multicast= *(uint8_t*)pinfo3;
			VLAN_LOG_DBG("[%d]:%s:set unicast=%d,broadcast=%d,multicast=%d\n",__LINE__,__FUNCTION__,
					vlan_info_send.storm_unicast,vlan_info_send.storm_broadcast,vlan_info_send.storm_multicast);
			ipc_send_msg_n2(&vlan_info_send,vlan_info_size, 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_VLAN, (uint8_t)type, IPC_OPCODE_UPDATE, 0);
			break; 
		case VLAN_INFO_ENABLE:
			id_num = v_end - v_start + 1;
			send_num = id_num / 100;
			remain_num=id_num-send_num*100;
			
			for(i = 0; i < send_num; i++)
			{
				k=0;
				memset(vlan_info_buff, 0, 100*vlan_info_size);	
				vlanid= v_start + i*100;

				data_len=100*vlan_info_size;
				data_num=100;
				
				for(j=vlanid;j<vlanid+data_num;j++)
				{
					memcpy(&vlan_info_buff[k], (&(vlan_table[j]->vlan)), vlan_info_size);
					k+=vlan_info_size;
				}
				ipc_send_msg_n2(vlan_info_buff, data_len, data_num, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_VLAN,  (uint8_t)type, IPC_OPCODE_UPDATE, 0);				
			}
			if( 0 != remain_num )
			{
				k=0;
				memset(vlan_info_buff, 0, 100*vlan_info_size);	
				vlanid= v_start + send_num*100;

				data_len=remain_num*vlan_info_size;
				data_num=remain_num;
				
				for(j=vlanid;j<vlanid+data_num;j++)
				{
					memcpy(&vlan_info_buff[k], (&(vlan_table[j]->vlan)), vlan_info_size);
					k+=vlan_info_size;
				}
				ipc_send_msg_n2(vlan_info_buff, data_len, data_num, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_VLAN,  (uint8_t)type, IPC_OPCODE_UPDATE, 0);				
			}
			
			break;
		default:
			VLAN_LOG_DBG("%s[%d]:leave %s:error:invalid type\n",__FILE__,__LINE__,__func__);
        	return ERRNO_FAIL;
	}
	
	switch(type)
	{
		case VLAN_INFO_MAC_LEARN:
			vlan_table[vlanid]->vlan.mac_learn = *(uint8_t*)pinfo1;
			break;
		case VLAN_INFO_MAC_LIMIT:
			vlan_table[vlanid]->vlan.limit_num = *(uint32_t*)pinfo1;
			if(pinfo2!=NULL)
			{
				vlan_table[vlanid]->vlan.limit_action = *(uint8_t*)pinfo2;
			}
			break;
		case VLAN_INFO_STORM_SUPRESS:
			vlan_table[vlanid]->vlan.storm_unicast= *(uint8_t*)pinfo1;
			vlan_table[vlanid]->vlan.storm_broadcast= *(uint8_t*)pinfo2;
			vlan_table[vlanid]->vlan.storm_multicast= *(uint8_t*)pinfo3;
			break;
		case VLAN_INFO_ENABLE:
			break;
		default:
			VLAN_LOG_DBG("%s[%d]:leave %s:error:invalid type\n",__FILE__,__LINE__,__func__);
        	return ERRNO_FAIL;
	}
					
		VLAN_LOG_DBG("%s[%d]:leave %s:successfully to set vlan_table[%d]\n",__FILE__,__LINE__,__func__,vlanid);		
		return ERRNO_SUCCESS;
	
}


int vlan_clear(void)
{
	uint16_t v_start = 0;
	uint16_t v_end = 0;	
	VLAN_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

	v_start = 1;
	v_end = 4094;
	
	vlan_delete(v_start,v_end);
	
	VLAN_LOG_DBG("%s[%d]:leave %s:successfully clear all vlan\n",__FILE__,__LINE__,__func__);
	return ERRNO_SUCCESS;
}

/*Edit by jhz,20180313,Vlan adds a member port to add tagged or untagged flags.*/
void vlan_add_interface(uint16_t v_start,uint16_t v_end,uint32_t ifindex,uint8_t tag)
{
	uint16_t vlanid = 0;
	struct vlan_range vlan;
	struct port_tagged *pport_tag=NULL;

	for(vlanid=v_start; vlanid<=v_end; ++vlanid)
	{
		pport_tag=(struct port_tagged*)XCALLOC(MTYPE_L2_VLAN, sizeof(struct port_tagged));
		pport_tag->ifindex=ifindex;
		pport_tag->tag=tag;
		listnode_add_sort(&(vlan_table[vlanid]->port_tag_list), (void*)pport_tag);	

		if(tag==DOT1Q_TUNNEL)	
		{
			return;
		}
		listnode_add_sort(&(vlan_table[vlanid]->portlist), (void*)ifindex);			
	}
	
	vlan.vlan_start=v_start;
	vlan.vlan_end=v_end;
	ipc_send_msg_n2(&vlan, sizeof(struct vlan_range), 1,MODULE_ID_FTM, MODULE_ID_L2, IPC_TYPE_VLAN, VLAN_INFO_PORT,IPC_OPCODE_ADD,ifindex);
	VLAN_LOG_DBG("%s[%d]:%s:v_start[%d] v_end[%d] ifindex[%x] successfully\n",__FILE__,__LINE__,__func__,v_start,v_end,ifindex);
	return;
}

/*Edit by jhz,20180313,Vlan deletes a member port to add tagged or untagged flags.*/
void vlan_delete_interface(uint16_t v_start,uint16_t v_end,uint32_t ifindex)
{
	uint16_t vlanid = 0;
	struct vlan_range vlan;
	struct listnode *p_listnode = NULL;
	struct port_tagged *pport_tag = NULL;
	uint8_t tag=0;
	
	for(vlanid=v_start; vlanid<=v_end; ++vlanid)
	{
		for(ALL_LIST_ELEMENTS_RO(&(vlan_table[vlanid]->port_tag_list), p_listnode, pport_tag))
		{
			if(ifindex == pport_tag->ifindex)
			{
				tag=pport_tag->tag;
				XFREE(MTYPE_L2_VLAN, pport_tag);
				list_delete_node (&(vlan_table[vlanid]->port_tag_list), p_listnode);
				break;
			}
		}
		if(tag==DOT1Q_TUNNEL)
		{
			return;
		}
	
		listnode_delete (&(vlan_table[vlanid]->portlist), (void*)ifindex);
	}

	vlan.vlan_start=v_start;
	vlan.vlan_end=v_end;
	ipc_send_msg_n2(&vlan, sizeof(struct vlan_range), 1,MODULE_ID_FTM, MODULE_ID_L2, IPC_TYPE_VLAN, VLAN_INFO_PORT,IPC_OPCODE_DELETE,ifindex);
	VLAN_LOG_DBG("%s[%d]:%s:v_start[%d] v_end[%d] ifindex[%x] successfully\n",__FILE__,__LINE__,__func__,v_start,v_end,ifindex);
	return;
}

int l2if_get_vlan_info_bulk(uint32_t v_start,struct vlan_info entry_buff[])
{
	uint16_t vlanid = 0;
	int data_num = 0;
	int msg_num  = IPC_MSG_LEN/sizeof(struct vlan_info);
	int flag=0;
	int i=0;

	/*The first query*/
	if(0==v_start)
	{
		for(i=vlanid;i<VLAN_MAX;i++)
		{
			if(ENABLE == vlan_table[i]->vlan.enable)
			{
				memcpy(&entry_buff[data_num++],&(vlan_table[i]->vlan),sizeof(struct vlan_info));
				if (data_num == msg_num)
				{
				 	return data_num;
				}
			}
		}
		return data_num;
	}
	else
	{
		for(i=vlanid;i<VLAN_MAX;i++)
		{
			if(0==flag)
			{
				if(v_start==(uint32_t)i)
				{
					flag=1;
				}
			}
			else
			{
				if(DISABLE != vlan_table[i]->vlan.enable)
				{
					memcpy(&entry_buff[data_num++],&(vlan_table[i]->vlan),sizeof(struct vlan_info));
					if (data_num == msg_num)
					{
						flag=0;
					 	return data_num;
					}
				}
			}
		}
		flag=0;
		return data_num;
	}
}

int vlan_reply_vlan_info_bulk(struct ipc_msghdr_n *phdr, uint16_t vlanid)
{
	int msg_num  = IPC_MSG_LEN/sizeof(struct vlan_info);
	struct vlan_info entry_buff[IPC_MSG_LEN/sizeof(struct vlan_info)];
	//uint32_t vlanid;
	int ret;

	//vlanid = phdr->msg_index;
	memset(entry_buff, 0, msg_num*sizeof(struct vlan_info));
	
	ret=l2if_get_vlan_info_bulk(vlanid, entry_buff);
	if(ret>0)
	{
		//ret = ipc_send_reply_bulk(entry_buff, ret*sizeof(struct vlan_info), ret, phdr->sender_id,
	    //                    phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index);
        ret = ipc_send_reply_n2(entry_buff, ret*sizeof(struct vlan_info), ret, phdr->sender_id,
                            phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->sequence, phdr->msg_index, IPC_OPCODE_REPLY);
	}
	else
	{
		//ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, phdr->module_id, phdr->msg_type,
	    //                    phdr->msg_subtype, phdr->msg_index);
       // ret = l2_msg_send_noack(ERRNO_NOT_FOUND, phdr, phdr->msg_index);
		ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id,
	                        phdr->module_id, phdr->msg_type, phdr->msg_subtype, 0,0, IPC_OPCODE_NACK);
	}
	return 0;
}

void vlan_single_config_write(uint16_t vlanid,struct vty *vty)
{
	uchar learn_info[2][10] = {"disable","enable"};
	uchar mac_limit_action[2][10] = {"discard","forward"};
	struct vlan_entry *P_vlan_entry = NULL;
	
	P_vlan_entry = vlan_table[vlanid];					
	vty_out ( vty, "vlan %d%s", P_vlan_entry->vlan.vlanid, VTY_NEWLINE );

	if('\0' != P_vlan_entry->name[0])
	{
		 vty_out ( vty, " name %s%s", P_vlan_entry->name,VTY_NEWLINE );
	}
	if(DISABLE==P_vlan_entry->vlan.mac_learn)
	{
		vty_out ( vty, " mac learn %s%s", learn_info[P_vlan_entry->vlan.mac_learn],VTY_NEWLINE );
	}

	if(P_vlan_entry->vlan.limit_num)
	{
		vty_out ( vty, " mac limit %d action %s%s", P_vlan_entry->vlan.limit_num,mac_limit_action[P_vlan_entry->vlan.limit_action],VTY_NEWLINE );
	}
	
	if((ENABLE==P_vlan_entry->vlan.storm_unicast)||(ENABLE==P_vlan_entry->vlan.storm_broadcast)||
			(ENABLE==P_vlan_entry->vlan.storm_multicast))
	{
		 vty_out ( vty, " storm suppress " );	
		 if(ENABLE==P_vlan_entry->vlan.storm_unicast)
		 {
			vty_out ( vty, "unicast ");
		 }
		 if(ENABLE==P_vlan_entry->vlan.storm_broadcast)
		 {
			vty_out ( vty, "broadcast ");
		 }
		 if(ENABLE==P_vlan_entry->vlan.storm_multicast)
		 {
			vty_out ( vty, "multicast ");
		 }
		 vty_out ( vty, "%s",VTY_NEWLINE);
	}

}

int vlan_config_write (struct vty *vty)
{
	uint16_t vlanid = 0;
	uint16_t v_start = 0;
	uint16_t v_end = 0;
	struct vlan_entry *P_vlan_entry = NULL;
	
	VLAN_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

	for(vlanid=1; vlanid<4095 ; vlanid++)
	{
		if(ENABLE == vlan_table[vlanid]->vlan.enable)
		{
			P_vlan_entry = vlan_table[vlanid];
			
			if(('\0' == P_vlan_entry->name[0])&&(ENABLE==P_vlan_entry->vlan.mac_learn)&&
				(!P_vlan_entry->vlan.limit_num)&&(DISABLE==P_vlan_entry->vlan.storm_unicast)&&
				(DISABLE==P_vlan_entry->vlan.storm_broadcast)&&(DISABLE==P_vlan_entry->vlan.storm_multicast))
			{
				if(!v_start)
				{
					v_start=vlanid;
					v_end=vlanid;
				}
				else
				{
					if(vlanid==v_end+1)
					{
						v_end=vlanid;
					}
					else 
					{
						if(v_start==v_end)
						{
							vty_out ( vty, "vlan %d%s", v_start, VTY_NEWLINE );
						}
						else
						{
							vty_out ( vty, "vlan %d to %d%s", v_start,v_end, VTY_NEWLINE );
						}
						v_start=vlanid;
						v_end=vlanid;
					}
				}
			}
			else
			{
				if(!v_start)
				{
					vlan_single_config_write(vlanid,vty);
				}
				else
				{
					if(v_start==v_end)
					{
						vty_out ( vty, "vlan %d%s", v_start, VTY_NEWLINE );
					}
					else
					{
						vty_out ( vty, "vlan %d to %d%s", v_start,v_end, VTY_NEWLINE );
					}
					v_start=0;
					v_end=0;
					vlan_single_config_write(vlanid,vty);
				}
			}				
		}
	}
	
	if(v_start)
	{
		if(v_start==v_end)
		{
			vty_out ( vty, "vlan %d%s", v_start, VTY_NEWLINE );
		}
		else
		{
			vty_out ( vty, "vlan %d to %d%s", v_start,v_end, VTY_NEWLINE );
		}
	}

	VLAN_LOG_DBG("%s[%d]:leave %s:successfully write vlan config \n",__FILE__,__LINE__,__func__);
	return ERRNO_SUCCESS;
}

