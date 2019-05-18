/*
*  Copyright (C) 2017  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: mstp_base_procedure.c	
*
*  date: 2017.3
*
*  modify:	2018.3.12 modified by liufuying to make mstp module code beautiful
*
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lib/log.h"
#include "lib/command.h"
#include "lib/ifm_common.h"
#include "lib/linklist.h"
#include "lib/zassert.h"
#include <lib/module_id.h>
#include <lib/msg_ipc_n.h>
#include "l2_if.h"
#include "trunk.h"
#include "lib/hash1.h"

#include "mstp.h"
#include "mstp_port.h"
#include "mstp_msg.h"
#include "mstp_base_procedure.h"
#include "mstp_sm_procedure.h"


extern struct mstp_bridge	*mstp_global_bridge;
extern struct mstp_vlan		global_ins_vlan_map[MSTP_INSTANCE_MAX];
extern struct hash_table 	l2if_table;

void __list_add(struct list_head * new,struct list_head * prev,struct list_head * next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

void list_add_tail(struct list_head *new, struct list_head *head)	
{
	__list_add(new, head->prev, head);
}

void __list_del(struct list_head * prev, struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = entry->prev = 0;
}

/*These parameters are treated as constants by the CIST and MSTI state machines; their values
can be modified only by management action.
a) ForceVersion
b) FwdDelay
c) TxHoldCount
d) MigrateTime
e) HelloTime
f) MaxHops
g) ExternalPortPathCost
h) InternalPortPathCost*/

/*The following parameters are as specified in 17.16 of IEEE Std 802.1D, 1998 Edition for RSTP. A single
value of each parameter applies to the MST Bridge as a whole, including all Ports and all CIST and MSTI
state machines:
ForceVersion
FwdDelay
TxHoldCount
MigrateTime.*/


/*13.26.8 FwdDelay*/
int mstp_fwd_delay(struct mstp_port *port)
{
	if (NULL == port)
	{
		return MSTP_FALSE;
	}

	return port->cist_port.designated_times.forward_delay;	
}

/*13.26.6 EdgeDelay*/
int mstp_edge_delay(struct mstp_port *port)
{
	struct mstp_port_variables	*port_variables = NULL;
	struct mstp_bridge			*m_br = NULL;

	if (NULL == port)
	{
		return MSTP_FALSE;
	}

	port_variables = &(port->port_variables);

	m_br = port->m_br;
	if (NULL == m_br)
	{
		return MSTP_FALSE;
	}

	if(MSTP_TRUE == port_variables->operPointToPointMAC)
	{
		/*time is always 0*/
		return m_br->common_br.migrate_time;
	}
	else
	{
		return m_br->common_br.bridge_max_age;
	}
}
int mstp_pdu_older(struct mstp_port *port)
{

	uint8_t pdu_older_time;
	if (NULL == port)
	{
		return MSTP_FALSE;
	}
	#if 0
	if(6 == port->cist_port.designated_times.max_age)
	{
		return 2 * port->cist_port.designated_times.max_age;
	}
	return 20;
	#endif
	/*pdu older time must more than 2 forwarddelay + hello time*/
	/*bug#51830 Aging time inaccuracy*/
	/*<stp>stp模式，配置bpdu老化时间Max Age不准确*/
	pdu_older_time = port->cist_port.designated_times.hello_time + 2 * \
						port->cist_port.designated_times.forward_delay;

	return pdu_older_time;
	
	                       
}

/*forwardDelay*/
int mstp_forward_delay(struct mstp_port *port)
{
	struct mstp_port_variables	*port_variables = NULL;

	if (NULL == port)
	{
		return MSTP_FALSE;
	}

	port_variables = &(port->port_variables);

	if(MSTP_TRUE == port_variables->sendRSTP)
	{
		return mstp_hello_time(port);
	}
	else
	{
		return mstp_fwd_delay(port);
	}
}

/*13.26.18 reRooted*/
int mstp_rerooted(struct mstp_port *port, unsigned int instance_id)
{
    struct mstp_msti_port 	*msti_port = NULL, *cur_mist_port = NULL, *next_mist_port = NULL;
	struct mstp_cist_port 	*cist_port = NULL, *cur_cist_port = NULL, *next_cist_port = NULL;
	struct mstp_bridge		*m_br = NULL;

	if (NULL == port)
	{
		return MSTP_FALSE;
	}

	m_br = port->m_br;
	if (NULL == m_br)
	{
		return MSTP_FALSE;
	}
	
	if(0 == instance_id)	/*cist*/
    {
		cur_cist_port = &port->cist_port;
	    list_for_each_entry_safe(cist_port, next_cist_port, &m_br->cist_br.port_head, port_list)
	    {
	        if(cur_cist_port == cist_port)
	        {
				continue;
			}
	        else
	        {
	        	if(cist_port->common_info.rrWhile != 0)
                {
                    MSTP_LOG_DBG("instance(%u) cist_port(%s) reRooted is error. rrWhile is %u\n", instance_id, port->if_name, cist_port->common_info.rrWhile);
                    return MSTP_FALSE;
                }     
	        }
	    }
	}
	else 	/*msti*/
	{
		 cur_mist_port = port->msti_port[instance_id-1];
		 list_for_each_entry_safe(msti_port, next_mist_port, &m_br->msti_br[instance_id-1]->port_head, port_list)
		 {
		 	if(cur_mist_port == msti_port)
	        {
				continue;
			}
	        else
	        {
	        	if(msti_port->common_info.rrWhile != 0)
                {      
                    MSTP_LOG_DBG("%s:instance(%u) msti_port(%s) reRooted is error. rrWhile is %u\n", __func__, instance_id, port->if_name, msti_port->common_info.rrWhile);
	            	return MSTP_FALSE;
                }
	        }
		 }
	}
	
    return MSTP_TRUE;
}

/*13.26.19 rstpVersion*/
int mstp_rstp_version(struct mstp_port *port)
{
	struct mstp_bridge		*m_br = NULL;

	if ((NULL == port) || (NULL == port->m_br))
	{
		return MSTP_FALSE;
	}

	m_br = port->m_br;

	/*force protocol version*/
	if(m_br->common_br.force_pt_version >= PROTO_VERID_RSTP)
	{
		return MSTP_TRUE;
	}

	return MSTP_FALSE;
}

/*13.26.20 stpVersion*/
int mstp_stp_version(struct mstp_port *port)
{
	struct mstp_bridge		*m_br = NULL;

	if ((NULL == port) || (NULL == port->m_br))
	{
		return MSTP_FALSE;
	}

	m_br = port->m_br;

	/*force protocol version*/
	if (m_br->common_br.force_pt_version < PROTO_VERID_RSTP)
	{
		return MSTP_TRUE;
	}

	return MSTP_FALSE;
}

/*13.26.1 allSynced*/
int mstp_all_synced(struct mstp_port *port, unsigned int instance_id)
{
	struct mstp_msti_port	*msti_port = NULL;
	struct mstp_msti_port	*cur_msti_port = NULL;

	struct mstp_msti_port	*next_msti_port = NULL;
	struct mstp_cist_port	*cist_port = NULL;
	struct mstp_cist_port	*cur_cist_port = NULL;
	struct mstp_cist_port	*next_cist_port = NULL;
	struct mstp_bridge		*m_br = NULL;

	if ((NULL == port) || (NULL == port->m_br))
	{
		return MSTP_FALSE;
	}

	m_br = port->m_br;

	if(instance_id == 0)	/*cist*/
	{
		cur_cist_port = &port->cist_port;
		list_for_each_entry_safe(cist_port, next_cist_port, &m_br->cist_br.port_head, port_list)
		{
			if((cist_port->common_info.selected != MSTP_TRUE)
				|| (cist_port->common_info.role != cist_port->common_info.selectedRole)
				|| (cist_port->common_info.updtInfo != MSTP_FALSE))
			{

				MSTP_LOG_DBG("%s:cist_port(%s) synced error! instance %u, selected: %u, updtInfo: %u, selectedRole: %u\n",
					__func__, cist_port->mport->if_name, instance_id, cist_port->common_info.selected, cist_port->common_info.updtInfo, cist_port->common_info.selectedRole);

				return MSTP_FALSE;
			}

			switch(cur_cist_port->common_info.role)
			{
				case MSTP_PORT_ROLE_ROOT:
				case MSTP_PORT_ROLE_ALTERNATE:
				case MSTP_PORT_ROLE_BACKUP:
				{	if((cist_port->common_info.role != MSTP_PORT_ROLE_ROOT)
					&& (cist_port->common_info.synced == MSTP_FALSE))
					{
						MSTP_LOG_DBG("%s:cist_port(%s) synced error! instance: %u, port role: %u, synced: %u\n",
							__func__, cist_port->mport->if_name, instance_id, cur_cist_port->common_info.role, cist_port->common_info.synced);

						return MSTP_FALSE;
					}

					break;
				}
				case MSTP_PORT_ROLE_DESIGNATED:
				case MSTP_PORT_ROLE_MASTER:
				{
					if((cur_cist_port != cist_port)
						&& (cist_port->common_info.synced == MSTP_FALSE))
						{
							MSTP_LOG_DBG("%s:cist_port(%s) synced error! instance: %u, synced: %u\n",
								__func__, cist_port->mport->if_name, instance_id, cist_port->common_info.synced);
							return MSTP_FALSE;
						}
					
						break;
				}
				default:
					break;
			}
		}
	}
	else	/*msti*/
	{
		cur_msti_port = port->msti_port[instance_id-1];
		list_for_each_entry_safe(msti_port, next_msti_port, &m_br->msti_br[instance_id-1]->port_head, port_list)
		{
		   if((msti_port->common_info.selected != MSTP_TRUE)
		   	|| (msti_port->common_info.role != msti_port->common_info.selectedRole)
		   	|| (msti_port->common_info.updtInfo != MSTP_FALSE))
		   {
			   MSTP_LOG_DBG("%s:msti_port(%s) synced error! instance %u, selected: %u, updtInfo: %u, selectedRole: %u\n",
				   __func__, msti_port->if_name, instance_id, msti_port->common_info.selected, msti_port->common_info.updtInfo, msti_port->common_info.selectedRole);
			   return MSTP_FALSE;
		   }
		
		   switch(cur_msti_port->common_info.role)
		   {
				case MSTP_PORT_ROLE_ROOT:
				case MSTP_PORT_ROLE_ALTERNATE:
				case MSTP_PORT_ROLE_BACKUP:
				{
					if((msti_port->common_info.role != MSTP_PORT_ROLE_ROOT)
				   		&& (msti_port->common_info.synced == MSTP_FALSE))
					{
					   MSTP_LOG_DBG("%s: msti_port(%s) synced error! instance: %u, port role: %u, synced: %u\n",
						__func__, msti_port->if_name, instance_id, msti_port->common_info.role, msti_port->common_info.synced);
					   return MSTP_FALSE;
			   		}
				}	
					break;
				case MSTP_PORT_ROLE_DESIGNATED:
				case MSTP_PORT_ROLE_MASTER:
				{
					if((cur_msti_port != msti_port)
						&& (msti_port->common_info.synced == MSTP_FALSE))
					{
						MSTP_LOG_DBG("%s: msti_port(%s) synced error! instance: %u, synced: %02x\n",
						__func__, msti_port->if_name, instance_id, msti_port->common_info.synced);

						return MSTP_FALSE;
					}
				}
					break;
				default:
					break;
		   }
		}
	}

	return MSTP_TRUE;
}

/*13.26.9 HelloTime*/
int mstp_hello_time(struct mstp_port *port)
{
	if (NULL == port)
	{
		return MSTP_FALSE;
	}
	
	return port->cist_port.port_times.hello_time;
}

/*13.26.11 MaxAge*/
int mstp_max_age(struct mstp_port *port)
{
	if (NULL == port)
	{
		return MSTP_FALSE;
	}

	return port->cist_port.designated_times.max_age;
}

/*13.26.2 allTransmitReady*/
int mstp_all_transmit_ready(struct mstp_port *port)
{
	unsigned int			i = 0;
	struct mstp_msti_port	*msti_port = NULL;
	struct mstp_cist_port	*cist_port = NULL;

	if (NULL == port)
	{
		return MSTP_FALSE;
	}

	cist_port = &port->cist_port;

	if((cist_port->common_info.selected != MSTP_TRUE)
		|| (cist_port->common_info.updtInfo != MSTP_FALSE))
	{
	    MSTP_LOG_DBG("%s: cist_port(%s) transmitReady error! selected: %u, updtInfo: %u\n",
           __func__, port->if_name, cist_port->common_info.selected, cist_port->common_info.updtInfo);

		return MSTP_FALSE;
	}

	for(i = 0;i < MSTP_INSTANCE_MAX;i++)
	{
		msti_port = port->msti_port[i];
		if((msti_port != NULL)
			&& ((msti_port->common_info.selected != MSTP_TRUE)
				|| (msti_port->common_info.updtInfo != MSTP_FALSE)))
		{
		    MSTP_LOG_DBG("%s: msti_port(%s) transmitReady error! selected: %02x, updtInfo: %02x\n",
				__func__, port->if_name, msti_port->common_info.selected, msti_port->common_info.updtInfo);

			return MSTP_FALSE;
		}
	}

	return MSTP_TRUE;
}

/*13.26.3 cist*/
int mstp_cist(unsigned int instance_id)
{
	if(0 == instance_id)
	{
		return MSTP_TRUE;
	}
	else
	{
		return MSTP_FALSE;
	}
}

/*13.26.4 cistRootPort*/
int mstp_cist_root_port(struct mstp_port *port)
{
	if (NULL == port)
	{
		return MSTP_FALSE;
	}
	
	if(MSTP_PORT_ROLE_ROOT == port->cist_port.common_info.role)
	{
		return MSTP_TRUE;
	}

	return MSTP_FALSE;
}

/*13.26.5 cistDesignatedPort*/
int mstp_cist_designated_port(struct mstp_port *port)
{
	if (NULL == port)
	{
		return MSTP_FALSE;
	}

	if(MSTP_PORT_ROLE_DESIGNATED == port->cist_port.common_info.role)
	{
		return MSTP_TRUE;
	}

	return MSTP_FALSE;

}

/*13.26.12 mstiDesignatedOrTCpropagatingRootPort*/
int mstp_msti_designated_port(struct mstp_port *port)
{
	unsigned int					i = 0;	/*ins_id*/
	struct mstp_msti_port 			*msti_port = NULL;

	if (NULL == port)
	{
		return MSTP_FALSE;
	}

	for(i = 0;i < MSTP_MSTI_MAX;i++)
	{
		msti_port = port->msti_port[i];
		if(msti_port != NULL)
		{
/*		
TRUE if the role for any MSTI for the given port is either:
a) DesignatedPort; or
b) RootPort, and the instance for the given MSTI and port of the tcWhile timer is not zero.		
*/
			if((MSTP_PORT_ROLE_DESIGNATED == msti_port->common_info.role)
			|| ((MSTP_PORT_ROLE_ROOT == msti_port->common_info.role)
				&& (msti_port->common_info.tcWhile != 0)))
			{
				return MSTP_TRUE;
			}
		}
	}
	
	return MSTP_FALSE;
}

/*13.26.13 mstiMasterPort*/
int mstp_msti_master_port(struct mstp_port *port)
{
	unsigned int	i = 0;	//instance id
	struct mstp_msti_port *msti_port = NULL;

	if (NULL == port)
	{
		return MSTP_FALSE;
	}

	for(i = 0; i < MSTP_INSTANCE_MAX; i++)
	{
		msti_port = port->msti_port[i];
		if((msti_port != NULL)
			&& (MSTP_PORT_ROLE_MASTER == msti_port->common_info.role))
		{
			return MSTP_TRUE;
		}
	}

	return MSTP_FALSE;
}

/*13.26.15 rcvdAnyMsg*/
int mstp_rcvd_any_msg(struct mstp_port *port)
{
	unsigned int			i = 0;	//instance id
	struct mstp_msti_port	*msti_port = NULL;
	struct mstp_cist_port	*cist_port = NULL;

	if (NULL == port)
	{
		return MSTP_FALSE;
	}

	cist_port = &port->cist_port;

	if(MSTP_TRUE == cist_port->common_info.rcvdMsg)
	{
		return MSTP_TRUE;
	}

	for(i = 0;i < MSTP_INSTANCE_MAX;i++)
	{
		msti_port = port->msti_port[i];
		if((msti_port != NULL)
			&& (MSTP_TRUE == msti_port->common_info.rcvdMsg))
		{
			return MSTP_TRUE;
		}
	}

	return MSTP_FALSE;
}

/*13.26.16 rcvdCistMsg*/
int mstp_rcvd_cist_msg(struct mstp_port *port)
{
	if (NULL == port)
	{
		return MSTP_FALSE;
	}

	if(MSTP_TRUE == port->cist_port.common_info.rcvdMsg)
	{
		return MSTP_TRUE;
	}
	
	return MSTP_FALSE;
}

/*13.26.17 rcvdMstiMsg*/
int mstp_rcvd_msti_msg(struct mstp_port *port, unsigned int instance_id)
{
	if ((NULL == port) || (NULL == port->msti_port[instance_id-1]))
	{
		return MSTP_FALSE;
	}

	if((instance_id != 0)
		&& (MSTP_FALSE == port->cist_port.common_info.rcvdMsg)
		&& (MSTP_TRUE == port->msti_port[instance_id-1]->common_info.rcvdMsg))
	{
		return MSTP_TRUE;
	}

	return MSTP_FALSE;
}

/*13.25.49 restrictedRole*/
int mstp_restricted_role(struct mstp_port *port)
{
	if (NULL == port)
	{
		return MSTP_FALSE;
	}

	return port->port_variables.restrictedRole;
}

/*13.25.50 restrictedTcn*/
int mstp_restricted_tcn(struct mstp_port *port)
{
	if (NULL == port)
	{
		return MSTP_FALSE;
	}

	return port->port_variables.restrictedTcn;
}

/*13.26.21 updtCistInfo*/
int mstp_updt_cist_info(struct mstp_port *port)
{
	if (NULL == port)
	{
		return MSTP_FALSE;
	}

	if(MSTP_TRUE == port->cist_port.common_info.updtInfo)
	{
		return MSTP_TRUE;
	}

	return MSTP_FALSE;
}

/*13.26.22 updtMstiInfo*/
int mstp_updt_msti_info(struct mstp_port *port, unsigned int instance_id)
{
	if ((NULL == port) || (NULL == port->msti_port[instance_id-1]))
	{
		return MSTP_FALSE;
	}

	if((instance_id != 0)
		&& ((MSTP_TRUE == port->cist_port.common_info.updtInfo)
			|| (MSTP_TRUE == port->msti_port[instance_id-1]->common_info.updtInfo)))
	{
		return MSTP_TRUE;
	}

	return MSTP_FALSE;
}

struct bridge_id mstp_get_br_id(unsigned short br_pri, unsigned int instance_id)
{
	unsigned short		temp = 0;
	struct bridge_id	br_id;
	
	memset((void *)&br_id, 0, sizeof(struct bridge_id));

	temp = (br_pri & 0xF000) | ((unsigned short)instance_id);
	br_id.pri[0] = (unsigned char)(temp >> 8);
	br_id.pri[1] = (unsigned char)temp;

	memcpy(br_id.mac_addr, mstp_global_bridge->bridge_mac.mac_addr, MAC_LEN);

	return br_id;
}

/**/
void mstp_mst_digest_recalc(void)
{
	/**/



	//FIXME




	return;
}

void mstp_change_vlan_map(unsigned char *new_map, unsigned int instance_id)
{
	unsigned short			vlan_id = 0;
	unsigned int			old_vid_flag = 0, new_vid_flag = 0;
	unsigned char			*old_map = &global_ins_vlan_map[instance_id].vlan_map[0];

	if (NULL == new_map)
	{
		return;
	}

	for(vlan_id = 1; vlan_id <= MSTP_VLAN_MAX_ID; vlan_id++)
	{
		old_vid_flag = mstp_vlan_map_get(old_map, vlan_id);
		new_vid_flag = mstp_vlan_map_get(new_map, vlan_id);

		if((1 == old_vid_flag) && (0 == new_vid_flag))
		{
			/* Used to have a instance of the vlan, now add a vlan to default STG*/
			mstp_stp_vlan_set(vlan_id, 0);				//FIXME
		}
		else if((0 == old_vid_flag) && (1 == new_vid_flag))
		{
			/* Instance is not the original vlan, now add a vlan to the instance of STG*/
			mstp_stp_vlan_set(vlan_id, instance_id);	//FIXME
		}
	}

	return;
}

void mstp_add_cist_port(struct mstp_port* mstp_port)
{
	struct mstp_cist_port* cist_port	= NULL;

	if (NULL == mstp_port)
	{
		return;
	}

	cist_port = &mstp_port->cist_port;

	cist_port->port_index				= mstp_port->ifindex;

	/*back point*/
	cist_port->mport					= mstp_port;
	cist_port->cist_bridge				= &mstp_global_bridge->cist_br;

	cist_port->common_info.AdminPathCost	= MSTP_FALSE;
	cist_port->common_info.rootPtState		= MSTP_FALSE;
//	cist_port->common_info.loopPtState		= MSTP_FALSE;

	/*cist sm begin flag*/
	cist_port->common_info.sm_info_state_begin = MSTP_TRUE;
	cist_port->common_info.sm_disable_role_state_begin = MSTP_TRUE;
	cist_port->common_info.sm_state_trans_state_begin = MSTP_TRUE;
	cist_port->common_info.sm_tplg_change_state_begin = MSTP_TRUE;

/*init mstp_hello_time() hello_time variable*/
	cist_port->designated_times.forward_delay = mstp_global_bridge->fwd_delay;
	/*need to confirm*/
	cist_port->port_times.hello_time = mstp_global_bridge->hello_time;	//FIXME	
	cist_port->designated_times.hello_time = mstp_global_bridge->hello_time;
	cist_port->designated_times.max_age = mstp_global_bridge->msg_max_age;
	cist_port->designated_times.remaining_hops = mstp_global_bridge->msg_max_hop_count;


	INIT_LIST_HEAD(&cist_port->port_list);
	list_add_tail(&cist_port->port_list, &mstp_global_bridge->cist_br.port_head);
}

/*add port to mstp list*/
void mstp_add_mport(struct mstp_port* mstp_port)
{
	if (NULL == mstp_port)
	{
		return;
	}

	list_add_tail(&mstp_port->port_list, &mstp_global_bridge->port_head);
}

void mstp_del_mport(struct mstp_port* mstp_port)
{
	if (NULL == mstp_port)
	{
		return;
	}

	list_del(&mstp_port->port_list);
}

void mstp_vlan_map_set(unsigned char *map, unsigned short vlan)
{
	if (NULL == map)
	{
		return;
	}

    map[vlan / 8] |= (1 << (vlan % 8));
    return ;
}

void mstp_vlan_map_unset(unsigned char *map, unsigned short vlan)
{
	if (NULL == map)
	{
		return;
	}

    map[vlan / 8] &= ~(1 << (vlan % 8));
    return ;
}

/*check vlan id if belong to this vlan map or not*/
int mstp_vlan_map_get (unsigned char *map, unsigned short vlan)
{
	if (NULL == map)
	{
		return MSTP_FALSE;
	}

	return (map[vlan / 8] >> (vlan % 8)) & 1;
}

unsigned int mstp_vlanid_belongto_vlanmap(unsigned char *vlan_map, unsigned short vlan_id)
{
	if (NULL == vlan_map)
	{
		return MSTP_FALSE;
	}

	/*check vlan id belong to this vlan_map, Y:return MSTP_TRUE, N:return MSTP_FALSE*/
	if (vlan_map[vlan_id / 8] & (1 << (vlan_id % 8)))
	{
		return MSTP_TRUE;
	}
	
	return MSTP_FALSE;
}


unsigned int mstp_cmp_vlan_map(unsigned char *vlan_map, unsigned char *cvlan_map)
{
	unsigned int	i = 0;

	if ((NULL == vlan_map) || (NULL == cvlan_map))
	{
		return MSTP_FALSE;
	}
	
	/*compare if have same vlan id, Y:return MSTP_TRUE, N:return MSTP_FALSE*/
	for(i = 0; i < MSTP_VLAN_MAP_SIZE; i++)
	{
		if(vlan_map[i] & cvlan_map[i])
		{
			return MSTP_TRUE;
		}
	}
	
	return MSTP_FALSE;
}

/*need to modify*/
unsigned int mstp_cmp_vlan_map_same(unsigned char *vlan_map, unsigned char *cvlan_map)
{
	unsigned int		i = 0;
	unsigned int		ret = MSTP_FALSE;

	if ((NULL == vlan_map) || (NULL == cvlan_map))
	{
		return MSTP_FALSE;
	}

	for(i = 0; i < MSTP_VLAN_MAP_SIZE; i++)
	{
		if(vlan_map[i] & cvlan_map[i])
		{
			ret = MSTP_TRUE;
			return ret;
		}
	}
	
	return ret;
}


/*Spanning tree groups are groups of VLANs:A port has a forwarding state for each
spanning tree group and this is the state used for each VLAN in that group. */
void mstp_stp_vlan_set(unsigned short vid, unsigned int stg_id)
{
	/*add vlan to stg*/
	
	MSTP_LOG_DBG("%s:  mstp_stp_vlan_set() \n", __func__);

}

void mstp_del_cist_port(struct mstp_port* port)
{
	if (NULL == port)
	{
		return;
	}

	list_del(&port->cist_port.port_list);
}


void mstp_add_del_msti_port_by_instancedelete(struct mstp_port* mstp_port)
{


}

/*judge whether mstp port is msti port*/
void mstp_add_del_msti_port_by_mportenable(struct mstp_port* mstp_port)
{
	unsigned int			instance_id = 0;
	struct mstp_vlan		vlan_info;

	if (NULL == mstp_port)
	{
		return;
	}

	/* get port vlan info*/
    memset(&vlan_info, 0, sizeof(struct mstp_vlan));
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

MSTP_LOG_DBG("%s:mstp_add_stg  port(%s), instance_id = %u, state(%s)!\n",
			__func__, mstp_port->if_name, instance_id, "MSTP_STATE_BLOCKING");

				/*add instance_id(instance_id), vlan_bitmap, port, state(PORT_STATE_BLOCKING) */
				//STG_MODIFY
				mstp_add_stg_vlan(instance_id, mstp_port, MSTP_STATE_BLOCKING);
				
				
			}
			else if((mstp_port->msti_port[instance_id-1] != NULL) && (MSTP_FALSE == mstp_port->adminStatus))
			{			
				mstp_del_msti_port(mstp_port, instance_id);
				/*this mstp have msti_port and have add this instance msti-port-list already*/

MSTP_LOG_DBG("%s:mstp_add_stg  port(%s), instance_id = %u, state(%s)!\n",
		__func__, mstp_port->if_name, instance_id, "MSTP_STATE_REMOVE");

				/*delete instance_id(instance_id), vlan_bitmap, port, state(FORWARDING) */
				//STG_MODIFY				
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

MSTP_LOG_DBG("%s:mstp_add_stg  port(%s), instance_id = %u, state(%s)!\n",
		__func__, mstp_port->if_name, instance_id, "MSTP_STATE_REMOVE");

				/*delete instance_id(instance_id), vlan_bitmap, port, state(FORWARDING) */
				//STG_MODIFY
				mstp_add_stg_vlan(instance_id, mstp_port, MSTP_STATE_REMOVE);
			}
			else
			{
				/*no need deal*/
			}
		}
	}

	return;
}

void mstp_add_delete_msti_port_by_instancevlan(unsigned int instance_id)
{
	int						cursor = 0;
	
	struct mstp_vlan		vlan_info;
	struct l2if				*pif = NULL;
	struct mstp_port		*mstp_port = NULL;
	struct hash_bucket		*pbucket = NULL;

/*need to think about trunk port*/

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
			&& (IFNET_MODE_L3 != pif->mode ))
		{
			/*mstp port have config [mstp enable]*/
			if((mstp_port != NULL) && (MSTP_TRUE == mstp_port->adminStatus))
			{
				/*port vlan map get*/
				memset(&vlan_info, 0, sizeof(struct mstp_vlan));
				mstp_port_vlan_map_get(mstp_port->ifindex, &vlan_info);
			
				/*port vlan belong to this instance*/
				if(mstp_cmp_vlan_map_same(global_ins_vlan_map[instance_id-1].vlan_map, vlan_info.vlan_map) == MSTP_TRUE)
				{
					if(NULL == mstp_port->msti_port[instance_id-1])
					{
						mstp_add_msti_port(mstp_port, instance_id);

MSTP_LOG_DBG("%s: mstp_add_stg port(%s), instance_id = %u, state(%s)!\n",
				__func__, mstp_port->if_name, instance_id, "MSTP_STATE_BLOCKING");

						//STG_MODIFY
						/*add instance_id(instance_id), vlan_bitmap, port, state(DISCARDING) */
						mstp_add_stg_vlan(instance_id, mstp_port, MSTP_STATE_BLOCKING);						
					}
					else
					{
MSTP_LOG_DBG("%s:mstp_add_stg  port(%s), instance_id = %u, state(%s)!\n",
			__func__, mstp_port->if_name, instance_id, "MSTP_STATE_BLOCKING");						
						/*this mstp have msti_port and have add this instance msti-port-list already*/
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
	
	return;
}


/*base ifindex get mstp port vlan msg*/
void mstp_port_vlan_map_get(unsigned int ifindex, struct mstp_vlan *vlan_map)
{
	/*get vlan info in l2 vlan struct*/
	void						*pdata = NULL;
	struct listnode				*p_listnode = NULL;	
	struct l2if 				*pif		= NULL;
	unsigned int				vlan_id = 0;

	if (NULL == vlan_map)
	{
		return;
	}

	/*get l2if*/
	pif = l2if_get(ifindex);
	if(NULL == pif)
	{
		MSTP_LOG_DBG("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return;
	}

	if(listhead(pif->switch_info.vlan_list) != NULL)
	{
		for(ALL_LIST_ELEMENTS_RO(pif->switch_info.vlan_list, p_listnode, pdata))
		{
			vlan_id = (unsigned int)pdata;
			mstp_vlan_map_set(vlan_map->vlan_map, vlan_id);
		}
	}	


    return;
}

void mstp_send_bpdu(void)
{
	struct mstp_port		*mstp_port = NULL, *mstp_port_next = NULL;

	list_for_each_entry_safe(mstp_port, mstp_port_next, &mstp_global_bridge->port_head, port_list)
	{
		/**/
		if((MSTP_PORT_ROLE_DESIGNATED == mstp_port->cist_port.common_info.role)
			&& (PORT_LINK_UP == mstp_port->port_variables.portEnabled))
		{
			mstp_port->cist_port.new_info = MSTP_FALSE;
			mstp_port->port_variables.newInfoMsti = MSTP_FALSE;


			if(PROTO_VERID_STP == mstp_port->stp_mode)
			{
				mstp_tx_config(mstp_port);
			}
			else
			{
				mstp_tx_mstp(mstp_port);
			}

			mstp_port->port_variables.txCount++;
			mstp_port->port_variables.tcAck = MSTP_FALSE;
		}
	}
}

void mstp_update_link_state(struct mstp_port* mstp_port)
{
	unsigned int			i = 0;
	unsigned int			status 		= 0;
	unsigned int			if_index 	= 0;
	
	/*default speed is 1000M GE/FE port*/
	unsigned int			speed = 1000;
	int                     ret  = 0;

	struct l2if				*pif		= NULL;
	struct mstp_cist_port	*cist_port 	= NULL;
	struct mstp_msti_port	*msti_port 	= NULL;

	struct ifm_port		port_info ;

	if (NULL == mstp_port)
	{
		return;
	}

	/*get l2if*/
	if_index = (unsigned int)mstp_port->ifindex;
	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		MSTP_LOG_DBG("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return;
	}

	/*link status*/
	status = pif->down_flag;	/* 0: linkup, 1: linkdown */ 
	if(1 == status)	/*link down*/
	{
		mstp_port->port_variables.portEnabled = PORT_LINK_DOWN;
	
		mstp_port->link_status = PORT_LINK_DOWN;
	}
	else	/*link up*/
	{
		mstp_port->port_variables.portEnabled = PORT_LINK_UP;
		
		mstp_port->link_status = PORT_LINK_UP;
	}
	memset(&port_info,0,sizeof(port_info));
	/*duplex status*/
	ret = mstp_if_duplex_get(mstp_port->ifindex,&port_info);

	/*set P2P base on port duplex /speed(port cost)*/
	if(!ret)
	{
		if(mstp_port->port_variables.AdminLink)
		{
			switch(port_info.duplex)
			{
				case IFNET_DUPLEX_INVALID:
				{
					mstp_port->port_variables.operPointToPointMAC = MSTP_FALSE;
				}
					break;
				case IFNET_DUPLEX_FULL:
				{
					mstp_port->port_variables.operPointToPointMAC = MSTP_TRUE;
				}
					break;
				case IFNET_DUPLEX_HALF:
				{
					mstp_port->port_variables.operPointToPointMAC = MSTP_FALSE;
				}
					break ;
				default:
				{
					mstp_port->port_variables.operPointToPointMAC = MSTP_FALSE;
				}
					break ;
			}


			switch(port_info.speed)
			{//printf("%s[%d]###########speed = %d\n",__FUNCTION__,__LINE__,port_info->speed);
				case IFNET_SPEED_INVALID:
				{
					speed = 1000;
				}
					break;
				case IFNET_SPEED_GE:
				{
					speed = 1000;
				}
					break;
				case IFNET_SPEED_FE:
				{
					speed = 100;
				}
					break;
				case IFNET_SPEED_10GE:
				{
					speed = 10000;
				}
					break;
				default:
					/*need to add deal, get real speed*/
					break;								
			}
		}
		else 
		{
			mstp_port->port_variables.operPointToPointMAC = MSTP_FALSE;

		}
	}


	cist_port = &mstp_port->cist_port;
	if(cist_port != NULL)
	{
		/*check cli config port path cost*/
		if(!cist_port->common_info.AdminPathCost)
		{
			/*set port cost is default*/
			if(PORT_LINK_DOWN == mstp_port->port_variables.portEnabled)
			{
				cist_port->common_info.port_cost = DEF_PORT_PATH_COST;
			}
			else/*set port cost is base on speed*/
			{
				if(speed != 0)
				{
					cist_port->common_info.port_cost = MSTP_LONG_PC_FACTOR / speed;
					//printf("%s[%d]###########speed = %d\n",__FUNCTION__,__LINE__,speed);
				}
				else
				{
					cist_port->common_info.port_cost = DEF_PORT_PATH_COST;
				}
			}
		}

	}

	/*set msti port port cost*/
	for(i = 0; i < MSTP_MSTI_MAX; i++)
	{
		msti_port = mstp_port->msti_port[i];
		if(msti_port != NULL)
		{
			/*check cli config instance[id] port path cost*/
			if(!msti_port->common_info.AdminPathCost)
			{
				/*link down, set port cost is default*/
				if(PORT_LINK_DOWN == mstp_port->port_variables.portEnabled)
				{
					msti_port->common_info.port_cost = DEF_PORT_PATH_COST;
				}
				else
				{
					/*set port cost base on speed*/
					if(speed != 0)
					{
						msti_port->common_info.port_cost = MSTP_LONG_PC_FACTOR / speed;
					}
					else
					{
						msti_port->common_info.port_cost = DEF_PORT_PATH_COST;						
					}
				}
			}
		}
	}
		  
 
	return;
}



int mstp_if_duplex_get (uint32_t msg_index,struct ifm_port *port_info)
{
	struct ipc_mesg_n * pmesg = NULL;
	int ret = -1;

	pmesg =  ipc_sync_send_n2(NULL, 0, 0, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_IFM, IFNET_INFO_SPEED, IPC_OPCODE_GET, msg_index,5000);
	if(pmesg)
	{
		if(pmesg->msghdr.data_len == sizeof(port_info))
		{
			memcpy(port_info,pmesg->msg_data,sizeof(port_info));
			ret = 0;
		}
		else
		{
			ret = -1;
		}
		mem_share_free(pmesg, MODULE_ID_L2);
		 
	}
	
	return ret;
}

#if 0
struct ifm_port *mstp_if_speed_get(uint32_t ifindex)
{
	/*get value(speed and duplex) form SDK*/
	return mstp_send_hal_wait_reply(NULL, 0, IFNET_INFO_SPEED, IPC_OPCODE_GET, ifindex);
}

#endif

void mstp_get_link_state(struct mstp_port* mstp_port, unsigned int if_index)
{
	unsigned char			status	= 0;
	unsigned int			i = 0;

	/*default speed is 1000M GE/FE port*/
	unsigned int			speed = 1000;

	struct mstp_msti_port	*msti_port = NULL;
	struct mstp_cist_port	*cist_port = NULL;
	struct ifm_port		port_info;
	
	int 					ret = 0;

	if (NULL == mstp_port)
	{
		return;
	}

	/*get interface info*/
	if(ifm_get_link(if_index, MODULE_ID_L2, &status) == 0)
	{
		if(IFNET_LINKUP == status)
		{
			mstp_port->port_variables.portEnabled = PORT_LINK_UP;
			mstp_port->link_status = PORT_LINK_UP;
			
			MSTP_LOG_DBG("%s:%s: port ifindex: %08x link,--the line of %d", __FILE__, __func__, if_index, __LINE__);
		}
		else
		{
			mstp_port->port_variables.portEnabled = PORT_LINK_DOWN;		
			mstp_port->link_status = PORT_LINK_DOWN;
			
			MSTP_LOG_DBG("%s:%s: port ifindex: %08x unlink,--the line of %d", __FILE__, __func__, if_index, __LINE__);
		}
	}
	else
	{
		mstp_port->port_variables.portEnabled = PORT_LINK_DOWN;
		mstp_port->link_status = PORT_LINK_DOWN;
		
		MSTP_LOG_DBG("%s:%s: port ifindex: %08x get interface status timeout...,--the line of %d", __FILE__, __func__, if_index, __LINE__);
	}


	/*duplex status*/
	memset(&port_info,0,sizeof(port_info));
	ret = mstp_if_duplex_get(mstp_port->ifindex,&port_info);

	if(!ret)
	{
		/*check cli config port path cost*/
		/*p2p has enable*/
		if(mstp_port->port_variables.AdminLink)
		{
			/*set P2P base on port duplex*/
			switch(port_info.duplex)
			{
				case IFNET_DUPLEX_INVALID:
				{
					mstp_port->port_variables.operPointToPointMAC = MSTP_FALSE;
				}
					break;
				case IFNET_DUPLEX_FULL:
				{
					mstp_port->port_variables.operPointToPointMAC = MSTP_TRUE;
				}
					break;
				case IFNET_DUPLEX_HALF:
				{
					mstp_port->port_variables.operPointToPointMAC = MSTP_FALSE;
				}
					break ;
				default:
				{
					mstp_port->port_variables.operPointToPointMAC = MSTP_FALSE;
				}
					break ; 			
			}

			switch(port_info.speed)
			{
				case IFNET_SPEED_INVALID:
				{
					speed = 1000;
				}
					break;
				case IFNET_SPEED_GE:
				{
					speed = 1000;
				}
					break;
				case IFNET_SPEED_FE:
				{
					speed = 100;
				}
					break;
				case IFNET_SPEED_10GE:
				{
					speed = 10000;
				}
					break;
				default:
					/*need to add deal, get real speed*/
					break;								
			}
		}
		else
		{
			mstp_port->port_variables.operPointToPointMAC = MSTP_FALSE;
			
		}
	}
	
	cist_port = &mstp_port->cist_port;
	if(cist_port != NULL)
	{
		/*check cli config port path cost*/
		if(!cist_port->common_info.AdminPathCost)
		{
			/*set port cost is default*/
			if(PORT_LINK_DOWN == mstp_port->port_variables.portEnabled)
			{
				cist_port->common_info.port_cost = DEF_PORT_PATH_COST;
			}
			else/*set port cost is base on speed*/
			{
				if(speed != 0)
				{
					cist_port->common_info.port_cost = MSTP_LONG_PC_FACTOR / speed;
				}
				else
				{
					cist_port->common_info.port_cost = DEF_PORT_PATH_COST;
				}
			}
		}
	}

	/*set msti port port cost*/
	for(i = 0; i < MSTP_MSTI_MAX; i++)
	{
		msti_port = mstp_port->msti_port[i];
		if(msti_port != NULL)
		{
			/*check cli config instance[id] port path cost*/
			if(!msti_port->common_info.AdminPathCost)
			{
				/*link down, set port cost is default*/
				if(PORT_LINK_DOWN == mstp_port->port_variables.portEnabled)
				{
					msti_port->common_info.port_cost = DEF_PORT_PATH_COST;
				}
				else
				{
					/*set port cost base on speed*/
					if(speed != 0)
					{
						msti_port->common_info.port_cost = MSTP_LONG_PC_FACTOR / speed;
					}
					else
					{
						msti_port->common_info.port_cost = DEF_PORT_PATH_COST;
					}
				}
			}
		}
	}

	return;
}
/*bug#51828  add mstp about trunk pathcost calculate function*/
/*<stp>trunk口上应用stp功能，对应的链路开销值不正确*/
void mstp_trunk_if_speed_get(struct mstp_port * port)
{
	//int cursor = 0;
	uint8_t i = 0;
//	uint16_t trunkid = 0;
	uint32_t speed = 0;
	//struct hash_bucket	*pbucket = NULL;	
	//struct trunk * trunk_entry = NULL;
	//struct l2if * pif = NULL;
	struct mstp_cist_port * cist_port = NULL;
	struct mstp_msti_port * msti_port = NULL;
	

	if(NULL == port)
	{
		return ;
	}
	
	if(!IFM_TYPE_IS_TRUNK_PHYSICAL(port->ifindex))
	{
		return;
	}

	speed = trunk_speed_set(IFM_TRUNK_ID_GET(port->ifindex));
	//printf("%s[%d]#############speed = %d\n",__FUNCTION__,__LINE__,speed);
	if(speed > 0)
	{
		/*trunk port inlude only one member port*/
		
		cist_port = &port->cist_port;
		
		if(!cist_port->common_info.AdminPathCost && speed)
		{	
			cist_port->common_info.port_cost = MSTP_LONG_PC_FACTOR/speed;
			if(mstp_global_bridge)
			{
				mstp_global_bridge->cist_br.root_priority.external_root_path_cost = MSTP_LONG_PC_FACTOR/speed; 
			}
			//MSTP_LOG_DBG("port_cost = %d speed = %d\n",cist_port->common_info.port_cost,trunk_entry->speed);

		}
		
		for(i = 0;i<MSTP_MSTI_MAX;i++ )
		{
				msti_port = port->msti_port[i];
				if(msti_port && !msti_port->common_info.AdminPathCost && speed)
				{		
					msti_port->common_info.port_cost = MSTP_LONG_PC_FACTOR/speed;
				
				}

		}
        
	}
	
	
	return ;
}
/*get mstp port base on (interface port index and port priority)*/
unsigned short mstp_port_map(unsigned int ifindex, unsigned char port_pri)
{
	return  (unsigned short)((port_pri & 0xF0) << 8) | ((unsigned short)IFM_PORT_ID_GET(ifindex) & 0xFFF);
}


