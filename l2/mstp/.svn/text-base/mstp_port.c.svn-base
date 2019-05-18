
/*
*  Copyright (C) 2016~2017  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: mstp_port.c
*
*  date: 2017.4
*
*  modify:2017.10~2017.12
*
*  modify:	2018.3.12 modified by liufuying to make mstp module code beautiful
*/
#include <string.h>
#include <stdlib.h>

#include "lib/log.h"
#include "lib/vty.h"
#include "lib/hash1.h"
#include "lib/memory.h"
#include "lib/memtypes.h"

#include "command.h"

#include "mstp.h"
#include "mstp_port.h"
#include "mstp_init.h"
#include "mstp_base_procedure.h"
#include "mstp_sm_procedure.h"

extern struct mstp_vlan		bridge_cist_vlan_map;
extern struct mstp_bridge	*mstp_global_bridge;
extern struct mstp_vlan		global_ins_vlan_map[MSTP_INSTANCE_MAX];

int mstp_clean_instance(struct vty *vty, unsigned int instance_id)
{

	struct mstp_instance		*instance = NULL;
    struct mstp_msti_br    		*msti_br = NULL;
	struct mstp_msti_port 		*msti_port = NULL;
	struct mstp_msti_port 		*next_msti_port = NULL;


	if(instance_id > MSTP_INSTANCE_MAX)
	{
		MSTP_LOG_DBG("%s[%d]:%s:error:instance_id[%u]\n", __FILE__, __LINE__, __func__, instance_id);
		return MSTP_RESULT_ERROR;
	}


	/*judge*/
	if(NULL == mstp_global_bridge)
	{
		MSTP_LOG_DBG("%s[%d]:%s:error:(mstp_global_bridge == NULL)\n", __FILE__, __LINE__, __func__);	
		return MSTP_RESULT_ERROR;
	}


	msti_br = mstp_global_bridge->msti_br[instance_id-1];
	if(msti_br == NULL)
	{
		MSTP_LOG_DBG("%s[%d]:%s:error:(msti_br == NULL)\n", __FILE__, __LINE__, __func__);
		goto ERROR;
	}


	instance = &mstp_global_bridge->mstp_instance[instance_id-1];
	if(instance == NULL)
	{
		MSTP_LOG_DBG("%s[%d]:%s:error:(instance[%d] == NULL)\n", __FILE__, __LINE__, __func__, instance_id);
		vty_error_out(vty," mstp clean instance (instance == NULL) %s", VTY_NEWLINE);

		goto ERROR;
	}


	/*delete all port struct of the instance , set the status of port in the instance is forward*/
	list_for_each_entry_safe (msti_port, next_msti_port, &msti_br->port_head, port_list)
	{
		/* delete stg */
		/* call driver interface, set corresponding ports state to forward*/
		mstp_set_port_state(msti_port->mport, instance_id, MSTP_STATE_FORWARDING);
		mstp_del_msti_port(msti_port->mport, instance_id);

MSTP_LOG_DBG("%s: mstp_add_stg port(%s), instance_id = %u, state(%s)!\n",
						__func__, msti_port->mport->if_name, instance_id, "MSTP_STATE_REMOVE");

		//STG_MODIFY
		/*add instance_id(instance_id), vlan_bitmap, port, state(FORWARDING) */ 	
		mstp_add_stg_vlan(instance_id, msti_port->mport, MSTP_STATE_REMOVE);
		
	}


	XFREE(MTYPE_MSTI_BR, msti_br);
	mstp_global_bridge->msti_br[instance_id-1] = NULL;

	/*modify the instance and vlan map, deal the instance in vlan map*/
	/*back instance vlan_map to  cist instance[0] vlan map*/	
	mstp_msti_vlanmap_backto_cist_vlanmap(bridge_cist_vlan_map.vlan_map, global_ins_vlan_map[instance_id-1].vlan_map);

	
	/*clean instance vlan map*/
	global_ins_vlan_map[instance_id-1].vlan_start = 0;
	global_ins_vlan_map[instance_id-1].vlan_end = 0;
	memset(global_ins_vlan_map[instance_id-1].vlan_map, 0x00, MSTP_VLAN_MAP_SIZE);

	/*change instance config to default*/
	mstp_instance_init(instance_id);
	return MSTP_RESULT_OK;

ERROR:
	if((NULL == msti_br) || (NULL == instance))
	{
		vty_out(vty,"%s line[%d] %s error.\n", __FILE__, __LINE__, __func__);
		return CMD_WARNING;
	}

	return MSTP_RESULT_OK;
}


void mstp_msti_vlanmap_backto_cist_vlanmap(unsigned char *dest, unsigned char *src)
{
	int				i = 0;

	for(i = 0; i < MSTP_VLAN_MAP_SIZE; i++)
	{
		dest[i] |= src[i];
	}

	return;
}




#if 0
int mstp_delete_instance_mstp_port(unsigned int instance_id, struct mstp_port *port)
{
	MSTP_LOG_DBG("%s: mstp_delete_instance_mstp_port() !\n", __func__);

	struct mstp_instance		*instance	= NULL;

	struct mstp_port 			*current = NULL;
	struct mstp_port 			*tail    = NULL;
	struct mstp_port 			*tmp     = NULL;

	instance = &mstp_global_bridge->mstp_instance[instance_id-1];
	current = instance->mstp_port_list;

	while(current != NULL)
	{	
    	if(current->ifindex == port->ifindex)
		{
			if(current == instance->mstp_port_list)
			{
				instance->mstp_port_list = current->next;
			}
			else
			{
				tail->next = current->next;
			}

			tmp = current;
//			current = current->next;

//			free(tmp);

			break;
		}
		else
		{
			tail = current;
			current = current->next;
		} 
  	}

	return 0;

	return MSTP_RESULT_OK;
}
#endif


int mstp_delete_instance_msti_port(unsigned int instance_id, struct mstp_port *port)
{
	struct mstp_instance		*instance	= NULL;

	struct mstp_msti_port 			*current = NULL;
	struct mstp_msti_port 			*tail    = NULL;

	if ((NULL == mstp_global_bridge) || (NULL == port))
	{
		return MSTP_RESULT_ERROR;
	}

	instance = &mstp_global_bridge->mstp_instance[instance_id-1];
	current = instance->msti_port_list;

	while(current != NULL)
	{	
    	if(current->mport->ifindex == port->ifindex)
		{
			if(current == instance->msti_port_list)
			{
				instance->msti_port_list = current->next;
			}
			else
			{
				tail->next = current->next;
			}

//			tmp = current;
//			current = current->next;

//			free(tmp);

			break;
		}
		else
		{
			tail = current;
			current = current->next;
		} 
  	}

	return MSTP_RESULT_OK;
}


/*delete instance msti port*/
void mstp_del_msti_port(struct mstp_port* mstp_port, unsigned int instance_id)
{
	struct mstp_msti_port* msti_port = NULL;

	if (NULL == mstp_port)
	{
		return;
	}

	if(instance_id > MSTP_INSTANCE_MAX)
	{
		MSTP_LOG_DBG("%s[%d]:%s:error:instance_id[%u]\n", __FILE__, __LINE__, __func__, instance_id);
		return;
	}

	msti_port = mstp_port->msti_port[instance_id-1];
	if(NULL == msti_port)
	{
		return;
	}

	/* delete msti port from msti_bridge_port_list */
	list_del(&msti_port->port_list);

	/**/
	/*add msti port to this instance msti_port_list head*/
	mstp_delete_instance_msti_port(instance_id, mstp_port);

	XFREE(MTYPE_MSTI_PORT, msti_port);
	mstp_port->msti_port[instance_id-1] = NULL;
}


void mstp_add_msti_port(struct mstp_port* mstp_port, unsigned int instance_id)
{
	struct mstp_msti_br			*msti_br = NULL;
	struct mstp_msti_port		*msti_port = NULL;

	if ((NULL == mstp_global_bridge) || (NULL == mstp_port))
	{
		return;
	}

	msti_br = mstp_global_bridge->msti_br[instance_id-1];
	msti_port = mstp_port->msti_port[instance_id-1];
	if(msti_port != NULL)
	{
		return;
	}

	msti_port = (struct mstp_msti_port *)XMALLOC(MTYPE_MSTI_PORT, sizeof(struct mstp_msti_port));
	if(NULL == msti_port)
	{
		return;
	}
	memset(msti_port, 0, sizeof(struct mstp_msti_port));
	/*add msti port to mstp_port*/
	mstp_port->msti_port[instance_id-1] = msti_port;

	/*init msti port*/
	memcpy(msti_port->if_name, mstp_port->if_name, INTERFACE_NAMSIZE);
	msti_port->mport						= mstp_port;
	msti_port->msti_bridge					= msti_br;
	msti_port->port_index					= mstp_port->ifindex;
	msti_port->common_info.port_cost		= mstp_port->msti_port_path_cost[instance_id-1];
	msti_port->common_info.AdminPathCost	= mstp_global_bridge->mstp_instance[instance_id-1].AdminPathCost;
	msti_port->common_info.rootPtState		= MSTP_FALSE;
//	msti_port->common_info.loopPtState		= MSTP_FALSE;

	/*msti_port sm begin flag*/
	msti_port->common_info.sm_info_state_begin = MSTP_TRUE;
	msti_port->common_info.sm_disable_role_state_begin = MSTP_TRUE;
	msti_port->common_info.sm_state_trans_state_begin = MSTP_TRUE;
	msti_port->common_info.sm_tplg_change_state_begin = MSTP_TRUE;
	
	//msti_port->common_info.loopPtState		= MSTP_LOOP_PTST_NO;
	INIT_LIST_HEAD(&msti_port->port_list);

	/*add port to msti bridge list*/
	list_add_tail(&msti_port->port_list, &msti_br->port_head);

	/*add msti port to this instance msti_port_list head*/
	msti_port->next = mstp_global_bridge->mstp_instance[instance_id-1].msti_port_list;	
	mstp_global_bridge->mstp_instance[instance_id-1].msti_port_list = msti_port;

	/*add port to msti bridge port list*/
	return;
}


void mstp_clean_mstp_port(struct mstp_port *clean_mstp_port)
{
	unsigned int				instance_id = 0;

	struct mstp_msti_port		*msti_port	= NULL;
	struct l2if					*pif		= NULL;

	if (NULL == clean_mstp_port)
	{
		return;
	}

	/* delete cist port */
	list_del(&clean_mstp_port->port_list);
	mstp_del_cist_port(clean_mstp_port);

	/*add instance_id(0), vlan_bitmap, port, state(FORWARDING)*/
	//STG_MODIFY
MSTP_LOG_DBG("%s:mstp_add_stg  port(%s), instance_id = 0, state(%s)!\n",
					__func__, clean_mstp_port->if_name, "MSTP_STATE_REMOVE");	
	mstp_add_stg_vlan(0, clean_mstp_port, MSTP_STATE_REMOVE);


	/*delete msti port from msti_br, mstp_instance_port_list*/
	for(instance_id = 1; instance_id <= MSTP_INSTANCE_MAX; instance_id++)
	{

		msti_port = clean_mstp_port->msti_port[instance_id-1];
		if(NULL == msti_port)
		{
			continue;
		}
		
		/* delete msti port from msti_bridge_port_list */
		list_del(&msti_port->port_list);
		
		/**/
		/*add msti port to this instance msti_port_list head*/
		mstp_delete_instance_msti_port(instance_id, clean_mstp_port);
		
		XFREE(MTYPE_MSTI_PORT, msti_port);
		clean_mstp_port->msti_port[instance_id-1] = NULL;

		/*delete instance_id(instance_id), vlan_bitmap, port, state(FORWARDING) */
		//STG_MODIFY	
MSTP_LOG_DBG("%s:mstp_add_stg  port(%s), instance_id = %u, state(%s)!\n",
				__func__, clean_mstp_port->if_name, instance_id, "MSTP_STATE_REMOVE");			
		mstp_add_stg_vlan(instance_id, clean_mstp_port, MSTP_STATE_REMOVE);
	}


	pif = l2if_lookup(clean_mstp_port->ifindex);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return;
	}

	if(pif->mstp_port_info != NULL)
	{
		XFREE(MTYPE_MSTP_PORT, pif->mstp_port_info);
		pif->mstp_port_info = NULL;
	}

	/*delete port need to reinit*/
	mstp_reinit();

	return;
}

void mstp_port_vlan_change(struct l2if *pif)
{
	/*vlan get*/
	unsigned int			instance_id = 0;
	unsigned int			change_flag = MSTP_FALSE;
	
	struct mstp_vlan		vlan_info;
	struct mstp_port		*mstp_port = NULL;
	

	/* get port vlan info*/
    memset(&vlan_info, 0, sizeof(struct mstp_vlan));

	/**/
	if((NULL == mstp_global_bridge) || 
		(NULL == pif) || 
		(NULL == pif->mstp_port_info))
	{
		return;
	}

	/*if mode is not mstp mode , port no add to msti
	***/
	if(mstp_global_bridge->stp_mode != PROTO_VERID_MSTP)
	{
		return;
	}

	
	mstp_port = pif->mstp_port_info;
    mstp_port_vlan_map_get(mstp_port->ifindex, &vlan_info);

	/*check all instance, if the port belong to this instance, add msti port */
	/*all mstp ports belong to instance[0]*/
	for(instance_id = 1; instance_id <= MSTP_INSTANCE_MAX; instance_id++)
	{
		/*(port vlan_list & instance vlan_list) is true*/
		if(MSTP_TRUE == mstp_cmp_vlan_map_same(global_ins_vlan_map[instance_id-1].vlan_map, vlan_info.vlan_map))
		{
			/*msti_port is NULL*/
			if((NULL == mstp_port->msti_port[instance_id-1]) && (MSTP_TRUE == mstp_port->adminStatus))
			{
				/*malloc msti_port space, add instance-port-list*/
				mstp_add_msti_port(mstp_port, instance_id);
				change_flag = MSTP_TRUE;

				//STG_MODIFY
				/*add instance_id(instance_id), vlan_bitmap, port, state(DISCARDING) */
MSTP_LOG_DBG("%s:mstp_add_stg  port(%s), instance_id = %u, state(%s)!\n",
				__func__, mstp_port->if_name, instance_id, "MSTP_STATE_BLOCKING");					
				mstp_add_stg_vlan(instance_id, mstp_port, MSTP_STATE_BLOCKING);
			
			}
			else if((mstp_port->msti_port[instance_id-1] != NULL) && (MSTP_FALSE == mstp_port->adminStatus))
			{			
				mstp_del_msti_port(mstp_port, instance_id);
				change_flag = MSTP_TRUE;
				/*this mstp have msti_port and have add this instance msti-port-list already*/


				//STG_MODIFY
				/*delete instance_id(instance_id), vlan_bitmap, port, state(FORWARDING) */
MSTP_LOG_DBG("%s:mstp_add_stg  port(%s), instance_id = %u, state(%s)!\n",
				__func__, mstp_port->if_name, instance_id, "MSTP_STATE_REMOVE");					
				mstp_add_stg_vlan(instance_id, mstp_port, MSTP_STATE_REMOVE);			
			}
		}
		else	/**/
		{
			/*(port vlan_list & instance vlan_list) is false, 
			**and msti-port is need delete from this instance*/
			if(mstp_port->msti_port[instance_id-1] != NULL)
			{
				mstp_del_msti_port(mstp_port, instance_id);
				change_flag = MSTP_TRUE;

				//STG_MODIFY
				/*delete instance_id(instance_id), vlan_bitmap, port, state(FORWARDING) */
MSTP_LOG_DBG("%s: mstp_add_stg port(%s), instance_id = %u, state(%s)!\n",
				__func__, mstp_port->if_name, instance_id, "MSTP_STATE_REMOVE");					
				mstp_add_stg_vlan(instance_id, mstp_port, MSTP_STATE_REMOVE);				
			}
			else
			{
				/*no need deal*/
			}
		}
	}

	if(MSTP_TRUE == change_flag)
	{
		mstp_reinit();
	}

	return;
}


void mstp_mode_modify_add_msti_port(void)
{
	/*all mstp port mode is mstp mode*/

	/*all port decide to add which instance*/
	int 					cursor = 0;
	unsigned int			instance_id = 0;

	struct mstp_vlan		vlan_info;

	struct l2if 			*pif = NULL;
	struct mstp_port		*mstp_port = NULL;
	struct hash_bucket		*pbucket = NULL;


	/* get port vlan info*/
    memset(&vlan_info, 0, sizeof(struct mstp_vlan));

	/*check all instance, if the port belong to this instance, add msti port */
	/*all mstp ports belong to instance[0]*/
	for(instance_id = 1; instance_id <= MSTP_INSTANCE_MAX; instance_id++)
	{
		/*check all port about this instance*/
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = pbucket->data;
			if(NULL == pif)
			{
				continue;
			}
			
			mstp_port = pif->mstp_port_info;
	
			/*judge interface mode*/
			if((IFNET_MODE_INVALID != pif->mode)
				&& (IFNET_MODE_L3 != pif->mode))
			{
				/*mstp port have config [mstp enable]*/
				if((mstp_port != NULL) && (MSTP_TRUE == mstp_port->adminStatus))
				{
					/*modify port stp mode*/
					mstp_port->stp_mode = PROTO_VERID_MSTP;
					
					/*port vlan map get*/
					memset(&vlan_info, 0, sizeof(struct mstp_vlan));
					mstp_port_vlan_map_get(mstp_port->ifindex, &vlan_info);
				
					/*port vlan belong to this instance*/
					if(MSTP_TRUE == mstp_cmp_vlan_map_same(global_ins_vlan_map[instance_id-1].vlan_map, vlan_info.vlan_map))
					{
						if(NULL == mstp_port->msti_port[instance_id-1])
						{
							mstp_add_msti_port(mstp_port, instance_id);
	
							//STG_MODIFY
							/*add instance_id(instance_id), vlan_bitmap, port, state(DISCARDING) */
MSTP_LOG_DBG("%s: mstp_add_stg port(%s), instance_id = %u, state(%s)!\n",
				__func__, mstp_port->if_name, instance_id, "MSTP_STATE_BLOCKING");								
							mstp_add_stg_vlan(instance_id, mstp_port, MSTP_STATE_BLOCKING);							
						}
						else
						{
							/*this mstp have msti_port and have add this instance msti-port-list already*/
MSTP_LOG_DBG("%s:mstp_add_stg  port(%s), instance_id = %u, state(%s)!\n",
				__func__, mstp_port->if_name, instance_id, "MSTP_STATE_BLOCKING");							
							mstp_add_stg_vlan(instance_id, mstp_port, MSTP_STATE_BLOCKING);								
						}
					}
					else
					{
						/*(port vlan_list & instance vlan_list) is false, 
						**and msti-port is need delete from this instance*/
						if(mstp_port->msti_port[instance_id-1] != NULL)
						{
							mstp_del_msti_port(mstp_port, instance_id);
							//STG_MODIFY
	
							/*delete instance_id(instance_id), vlan_bitmap, port, state(FORWARDING) */
MSTP_LOG_DBG("%s:mstp_add_stg  port(%s), instance_id = %u, state(%s)!\n",
				__func__, mstp_port->if_name, instance_id, "MSTP_STATE_REMOVE");								
							mstp_add_stg_vlan(instance_id, mstp_port, MSTP_STATE_REMOVE);							
						}
						else
						{
							/*no need deal*/
						}
					}
				}
			}
		}
	}
	
	/*need to think about trunk port*/

	return;
}

void mstp_mode_modify_delete_msti_port(void)
{
	/*all instance, all interface msti delete*/
	int 					cursor = 0;
	unsigned int			instance_id	= 0;

	struct l2if 			*pif		= NULL;
	struct mstp_port		*mstp_port	= NULL;
	struct hash_bucket		*pbucket	= NULL;

	for(instance_id = 1; instance_id <= MSTP_INSTANCE_MAX; instance_id++)
	{
		/*check all port about this instance*/
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = pbucket->data;
			if(NULL == pif)
			{
				continue;
			}

			mstp_port = pif->mstp_port_info;

			/*judge interface mode*/
			if((IFNET_MODE_INVALID != pif->mode)
				&& (IFNET_MODE_L3 != pif->mode))
			{
				/*mstp port have config [mstp enable]*/
				if((mstp_port != NULL) && (MSTP_TRUE == mstp_port->adminStatus))
				{
					/*(port vlan_list & instance vlan_list) is false, 
					**and msti-port is need delete from this instance*/
					if(mstp_port->msti_port[instance_id-1] != NULL)
					{
						mstp_del_msti_port(mstp_port, instance_id);
						//STG_MODIFY
		
						/*delete instance_id(instance_id), vlan_bitmap, port, state(FORWARDING) */
MSTP_LOG_DBG("%s:mstp_add_stg  port(%s), instance_id = %u, state(%s)!\n",
				__func__, mstp_port->if_name, instance_id, "MSTP_STATE_REMOVE");								
						mstp_add_stg_vlan(instance_id, mstp_port, MSTP_STATE_REMOVE);				
					}
				}
			}
		}
	}

	return;
}

