/*
*this file handle ospf bfd session
*/
#include "zebra.h"
#include "lib/prefix.h"
#include "lib/if.h"
#include "lib/table.h"
#include "lib/msg_ipc_n.h"

#include "ospfd/ospfd.h"
#include "ospfd/ospf_lsdb.h"
#include "ospfd/ospf_neighbor.h"
#include "ospfd/ospf_interface.h"
#include "ospfd/ospf_lsa.h"
#include "ospfd/ospf_dump.h"

#include "ospfd/ospf_nsm.h"
#include "ospfd/ospf_bfd.h"

const char *ospf_bfd_diag_string[8] = 
{
	"No diagnostic",
	"The detection timer times out",
	"Echo function failed",
	"Session neighbor down",
	"Forwarding Plane Reset",
	"Path down",
	"Link down",
	"Admin down"
};


static int ospf_bfd_add_session(struct ospf_neighbor *nbr)
{
	if(IS_DEBUG_OSPF_BFD)
	{
		zlog_debug(OSPF_DBG_BFD, "%s[%d]: In function '%s' ", __FILE__, __LINE__, __func__);
	}
	struct ospf_interface *oi = nbr->oi;
	struct bfd_sess session;
	
	/* Sanity check. */
	if (!(OSPF_IF_PARAM(oi, bfd_flag)) || nbr->state != NSM_Full)
	{
		zlog_debug(OSPF_DBG_BFD, "The oi %s bfd is %s or nbr state is not NSM_Full\n", IF_NAME(oi), (oi->params->bfd_flag == 1) ? "enable" : "disable");
		return -1;
	}

	//Fill the target session content and send it to BFD.
	memset(&session, 0, sizeof(struct bfd_sess));
	session.index = oi->ifp->ifindex;
	session.srcip = oi->address->u.prefix4.s_addr;
	session.dstip = nbr->address.u.prefix4.s_addr;
	if(OSPF_IF_PARAM_IS_SET(oi, if_bfd_recv_interval))
	{
		if(IS_DEBUG_OSPF_BFD)
		{
			zlog_debug(OSPF_DBG_BFD, " oi->params->if_bfd_recv_interval = %d default_params recv_interval = %d\n",\
				oi->params->if_bfd_recv_interval, IF_DEF_PARAMS(oi->ifp)->if_bfd_recv_interval);
		}
		session.cc_interval_cfg = OSPF_IF_PARAM(oi, if_bfd_recv_interval);
	}
	if(OSPF_IF_PARAM_IS_SET(oi, if_bfd_detect_multiplier))
	{
		if(IS_DEBUG_OSPF_BFD)
		{
			zlog_debug(OSPF_DBG_BFD, " oi->params->detect_multiplier = %d default_params detect_multiplier = %d\n",\
				oi->params->if_bfd_detect_multiplier, IF_DEF_PARAMS(oi->ifp)->if_bfd_detect_multiplier);
		}
		session.cc_multiplier_cfg =  OSPF_IF_PARAM(oi, if_bfd_detect_multiplier);
	}

	if(IS_DEBUG_OSPF_BFD)
	{
		zlog_debug(OSPF_DBG_BFD, "index = %d srcip = %d srcip_addr = %s dstip = %d\n", session.index, oi->address->u.prefix4.s_addr, inet_ntoa(oi->address->u.prefix4), session.dstip);
	}
	//Here you can use fifo to ensure that the message sent successfully.
	/*int ret = ipc_send_common(&session, sizeof(struct bfd_sess), 1, \
		MODULE_ID_MPLS, MODULE_ID_OSPF, IPC_TYPE_BFD, OSPF_SUBTYPE_BIND_BFD, IPC_OPCODE_ADD);*/
	int ret = ipc_send_msg_n2(&session, sizeof(struct bfd_sess), 1, \
		MODULE_ID_MPLS, MODULE_ID_OSPF, IPC_TYPE_BFD, OSPF_SUBTYPE_BIND_BFD, IPC_OPCODE_ADD, 0);
	if(ret < 0)
    {
        zlog_err("%-15s[%d]:ERROR:ospf send session creat fail: bfd_sess peer ip:%s",\
			__FUNCTION__, __LINE__, inet_ntoa(nbr->address.u.prefix4));
        return -1;
    }

	nbr->bfd_flag = OSPF_NEIGHBOR_BFD;
	
	return 0;
}


static int ospf_bfd_delete_session(struct ospf_neighbor *nbr)
{
	if(IS_DEBUG_OSPF_BFD)
	{
		zlog_debug(OSPF_DBG_BFD, "%s[%d]: In function '%s' ", __FILE__, __LINE__, __func__);
	}
	struct ospf_interface *oi = nbr->oi;
	struct bfd_sess session;
	/* Sanity check. */
	if (!(nbr->bfd_flag))
	{
		zlog_debug(OSPF_DBG_BFD, "Delete session fail, the nbr bfd is not exist! because nbr->bfd_flag == 0\n");
		return -1;
	}

	//Fill the target session content and send it to BFD.
	memset(&session, 0, sizeof(struct bfd_sess));
	session.index = oi->ifp->ifindex;
	session.srcip = oi->address->u.prefix4.s_addr;
	session.dstip = nbr->address.u.prefix4.s_addr;

	if(IS_DEBUG_OSPF_BFD)
	{
		zlog_debug(OSPF_DBG_BFD, "index = %d srcip = %d srcip_addr = %s dstip = %d\n", session.index, oi->address->u.prefix4.s_addr, inet_ntoa(oi->address->u.prefix4), session.dstip);
	}
	//Here you can use fifo to ensure that the message sent successfully.
	/*int ret = ipc_send_common(&session, sizeof(struct bfd_sess), 1, \
			MODULE_ID_MPLS, MODULE_ID_OSPF, IPC_TYPE_BFD, OSPF_SUBTYPE_UNBIND_BFD, IPC_OPCODE_DELETE);*/
	int ret = ipc_send_msg_n2(&session, sizeof(struct bfd_sess), 1, \
			MODULE_ID_MPLS, MODULE_ID_OSPF, IPC_TYPE_BFD, OSPF_SUBTYPE_UNBIND_BFD, IPC_OPCODE_DELETE, 0);
	
	if(ret < 0)
    {
        zlog_err("%-15s[%d]:ERROR:ospf send session delete fail: bfd_sess peer ip:%s",\
			__FUNCTION__, __LINE__, inet_ntoa(nbr->address.u.prefix4));
        return -1;
    }

	memset(&(nbr->bfd_session_info), 0, sizeof(struct ospf_bfd_sess_info));
	nbr->bfd_flag = OSPF_NEIGHBOR_NO_BFD;
	return 0;
}

static int ospf_bfd_modify_session(struct ospf_neighbor *nbr)
{
	if(IS_DEBUG_OSPF_BFD)
	{
		zlog_debug(OSPF_DBG_BFD, "%s[%d]: In function '%s' ", __FILE__, __LINE__, __func__);
	}
	struct ospf_interface *oi = nbr->oi;
	struct bfd_sess session;
	
	/* Sanity check. */
	if (!(OSPF_IF_PARAM(oi, bfd_flag)) || nbr->state != NSM_Full)
	{
		OSPF_LOG_ERROR("The oi %s bfd is %s or nbr state is not NSM_Full\n", IF_NAME(oi), (oi->params->bfd_flag == 1) ? "enable" : "disable");
		return -1;
	}

	//Fill the target session content and send it to BFD.
	memset(&session, 0, sizeof(struct bfd_sess));
	session.index = oi->ifp->ifindex;
	session.srcip = oi->address->u.prefix4.s_addr;
	session.dstip = nbr->address.u.prefix4.s_addr;
	if(OSPF_IF_PARAM_IS_SET(oi, if_bfd_recv_interval))
	{
		if(IS_DEBUG_OSPF_BFD)
		{
			zlog_debug(OSPF_DBG_BFD, " oi->params->if_bfd_recv_interval = %d default_params recv_interval = %d\n",\
				oi->params->if_bfd_recv_interval, IF_DEF_PARAMS(oi->ifp)->if_bfd_recv_interval);
		}
		session.cc_interval_cfg = OSPF_IF_PARAM(oi, if_bfd_recv_interval);
	}
	if(OSPF_IF_PARAM_IS_SET(oi, if_bfd_detect_multiplier))
	{
		if(IS_DEBUG_OSPF_BFD)
		{
			zlog_debug(OSPF_DBG_BFD, " oi->params->detect_multiplier = %d default_params detect_multiplier = %d\n",\
				oi->params->if_bfd_detect_multiplier, IF_DEF_PARAMS(oi->ifp)->if_bfd_detect_multiplier);
		}
		session.cc_multiplier_cfg =  OSPF_IF_PARAM(oi, if_bfd_detect_multiplier);
	}
	//Here you can use fifo to ensure that the message sent successfully.
	/*int ret = ipc_send_common(&session, sizeof(struct bfd_sess), 1, \
				MODULE_ID_MPLS, MODULE_ID_OSPF, IPC_TYPE_BFD, 0, IPC_OPCODE_UPDATE);*/
	int ret = ipc_send_msg_n2(&session, sizeof(struct bfd_sess), 1, \
				MODULE_ID_MPLS, MODULE_ID_OSPF, IPC_TYPE_BFD, 0, IPC_OPCODE_UPDATE, 0);
	
	if(ret < 0)
    {
        zlog_err("%-15s[%d]:ERROR:ospf send session creat fail: bfd_sess peer ip:%s",\
			__FUNCTION__, __LINE__, inet_ntoa(nbr->address.u.prefix4));
        return -1;
    }

	nbr->bfd_flag = OSPF_NEIGHBOR_BFD;
	
	return 0;
}


void
ospf_bfd_update_session (struct ospf_neighbor *nbr, int old_state)
{
	if(IS_DEBUG_OSPF_BFD)
	{
		zlog_debug(OSPF_DBG_BFD, "%s[%d]: In function '%s' ", __FILE__, __LINE__, __func__);
	}
	if(!(OSPF_IF_PARAM(nbr->oi, bfd_flag)))
	{
		return;
	}
	
	if (nbr->state == NSM_Full && old_state != NSM_Full)
		ospf_bfd_add_session (nbr);
	else if (old_state == NSM_Full && nbr->state != NSM_Full)
		ospf_bfd_delete_session (nbr);
}

int ospf_bfd_add_session_by_interface (struct ospf_interface *oi)
{
	if(IS_DEBUG_OSPF_BFD)
	{
		zlog_debug(OSPF_DBG_BFD, "%s[%d]: In function '%s' ", __FILE__, __LINE__, __func__);
	}
	struct route_node *rn = NULL;
	struct ospf_neighbor *nbr;

	if (oi->type == OSPF_IFTYPE_VIRTUALLINK
      && (nbr = ospf_nbr_lookup_ptop (oi)))
	{
		ospf_bfd_add_session (nbr);
	}
	else
	{
		for (rn = route_top (oi->nbrs); rn; rn = route_next (rn))
		{
			if ((nbr = rn->info) == NULL || nbr == oi->nbr_self)
	        {
	            continue;
	        }

			if (nbr->state == NSM_Full)
			{
				ospf_bfd_add_session (nbr);
			}
		}
	}
	return 0;
}

int ospf_bfd_modify_session_by_interface (struct ospf_interface *oi)
{
	if(IS_DEBUG_OSPF_BFD)
	{
		zlog_debug(OSPF_DBG_BFD, "%s[%d]: In function '%s' ", __FILE__, __LINE__, __func__);
	}
	struct route_node *rn = NULL;
	struct ospf_neighbor *nbr;

	if(oi == NULL)
	{
		OSPF_LOG_ERROR(" oi == NULL\n");
		return -1;
	}

	if(IS_DEBUG_OSPF_BFD)
	{
		zlog_debug(OSPF_DBG_BFD, "oi->name = %s oi->type = %d\n", IF_NAME(oi), oi->type);
	}
	if (oi->type == OSPF_IFTYPE_VIRTUALLINK
      && (nbr = ospf_nbr_lookup_ptop (oi)))
	{
		ospf_bfd_modify_session (nbr);
	}
	else
	{
		for (rn = route_top (oi->nbrs); rn; rn = route_next (rn))
		{
			if ((nbr = rn->info) == NULL || nbr == oi->nbr_self)
	        {
	            continue;
	        }

			if (nbr->state == NSM_Full)
			{
				ospf_bfd_modify_session (nbr);
			}
		}
	}
	return 0;
}


static int ospf_bfd_delete_session_by_interface (struct ospf_interface *oi)
{
	if(IS_DEBUG_OSPF_BFD)
	{
		zlog_debug(OSPF_DBG_BFD, "%s[%d]: In function '%s' ", __FILE__, __LINE__, __func__);
	}
	struct route_node *rn = NULL;
	struct ospf_neighbor *nbr;
	
	if (oi->type == OSPF_IFTYPE_VIRTUALLINK
		  && (nbr = ospf_nbr_lookup_ptop (oi)))
	{
		ospf_bfd_delete_session (nbr);
	}
	else
	{
		for (rn = route_top (oi->nbrs); rn; rn = route_next (rn))
		{
			if ((nbr = rn->info) == NULL || nbr == oi->nbr_self)
	        {
	            continue;
	        }

			if (nbr->state == NSM_Full)
			{
				ospf_bfd_delete_session (nbr);
			}
		}
	}
	return 0;
}

void
ospf_bfd_session_down (struct bfd_info *base_sess_info)
{
	if(IS_DEBUG_OSPF_BFD)
	{
		zlog_debug(OSPF_DBG_BFD, "%s[%d]: In function '%s' ", __FILE__, __LINE__, __func__);
	}
	
	struct ospf_neighbor *nbr  = NULL;
	struct listnode *node = NULL;
	struct interface *ifp = NULL;
	struct route_node *rn = NULL;
	struct ospf_interface *oi = NULL;
	struct in_addr peer_addr;
	int flag = 0;
	
	if((base_sess_info == NULL) || ((base_sess_info->type != BFD_TYPE_INTERFACE) && (base_sess_info->type != BFD_TYPE_IP)))
	{
		OSPF_LOG_ERROR("recv data from BFD is error!");
		return;
	}
	
	for (ALL_LIST_ELEMENTS_RO (om->iflist, node, ifp))
	{
		if(ifp->ifindex == base_sess_info->index)
		{
			for (rn = route_top (IF_OIFS (ifp)); rn; rn = route_next (rn))
			{
		        if ( (oi = rn->info) == NULL)
		        {
		            continue;
		        }
				if(base_sess_info->src_ip == ntohl(oi->address->u.prefix4.s_addr))
				{
					flag = 1;
					break;	
				}
			}
		}
		if(flag)
		{
			break;
		}
	}
	
	peer_addr.s_addr = htonl(base_sess_info->dst_ip);
	if(IS_DEBUG_OSPF_BFD)
	{
		zlog_debug(OSPF_DBG_BFD, " peer_addr = %s \n", inet_ntoa(peer_addr));
	}
	if(oi)
	{
		nbr = ospf_nbr_lookup_by_addr (oi->nbrs, &peer_addr);
	  	if (nbr)
			OSPF_NSM_EVENT_EXECUTE (nbr, NSM_KillNbr);
	}
	else
		zlog_debug(OSPF_DBG_BFD, " The oi is not exist that BFD reported.ifindex = %d\n", base_sess_info->index);
	
}

void ospf_bfd_session_info_load(struct bfd_sess *session_info)
{
	if(IS_DEBUG_OSPF_BFD)
	{
		zlog_debug(OSPF_DBG_BFD, "%s[%d]: In function '%s' ", __FILE__, __LINE__, __func__);
	}
	struct ospf_neighbor *nbr  = NULL;
	struct listnode *node = NULL;
	struct interface *ifp = NULL;
	struct route_node *rn = NULL;
	struct ospf_interface *oi = NULL;
	struct in_addr peer_addr;
	int flag = 0;

	if((session_info == NULL) || ((session_info->type != BFD_TYPE_INTERFACE) && (session_info->type != BFD_TYPE_IP)))
	{
		OSPF_LOG_ERROR("recv data from BFD is error!");
		return;
	}
	for (ALL_LIST_ELEMENTS_RO (om->iflist, node, ifp))
	{
		if(ifp->ifindex == session_info->index)
		{
			for (rn = route_top (IF_OIFS (ifp)); rn; rn = route_next (rn))
			{
		        if ( (oi = rn->info) == NULL)
		        {
		            continue;
		        }
				if(session_info->srcip == ntohl(oi->address->u.prefix4.s_addr))
				{
					flag = 1;
					break;	
				}
			}
		}
		if(flag)
		{
			break;
		}
	}
	
	peer_addr.s_addr = htonl(session_info->dstip);
	if(IS_DEBUG_OSPF_BFD)
	{
		zlog_debug(OSPF_DBG_BFD, "peer_addr = %s \n", inet_ntoa(peer_addr));
	}
	if(oi)
	{
		nbr = ospf_nbr_lookup_by_addr (oi->nbrs, &peer_addr);
	  	if (nbr)
	  	{
	  		memset(&(nbr->bfd_session_info), 0, sizeof(struct ospf_bfd_sess_info));
			nbr->bfd_session_info.session_id = session_info->session_id;
			nbr->bfd_session_info.local_id = session_info->local_id;
			nbr->bfd_session_info.remote_id = session_info->remote_id;
			nbr->bfd_session_info.srcip = session_info->srcip;
			nbr->bfd_session_info.dstip = session_info->dstip;
			nbr->bfd_session_info.cc_multiplier_cfg = session_info->cc_multiplier_cfg;
			nbr->bfd_session_info.cc_interval_cfg = session_info->cc_interval_cfg;
			nbr->bfd_session_info.cc_interval_recv  = session_info->cc_interval_recv;
			nbr->bfd_session_info.cc_interval_send = session_info->cc_interval_send;
			nbr->bfd_session_info.status = session_info->status;
		}
	}
	else
		zlog_debug(OSPF_DBG_BFD, " The oi is not exist that BFD reported.ifindex = %d\n", session_info->index);
}

void ospf_bfd_session_status_update(struct bfd_info *bfd_sess_info)
{
	if(IS_DEBUG_OSPF_BFD)
	{
		zlog_debug(OSPF_DBG_BFD, "%s[%d]: In function '%s' ", __FILE__, __LINE__, __func__);
	}
	
	struct ospf_neighbor *nbr  = NULL;
	struct listnode *node = NULL;
	struct interface *ifp = NULL;
	struct route_node *rn = NULL;
	struct ospf_interface *oi = NULL;
	struct in_addr peer_addr;
	int flag = 0;

	if((bfd_sess_info == NULL) || ((bfd_sess_info->type != BFD_TYPE_INTERFACE) && (bfd_sess_info->type != BFD_TYPE_IP)))
	{
		OSPF_LOG_ERROR("recv data from BFD is error!");
		return;
	}
	for (ALL_LIST_ELEMENTS_RO (om->iflist, node, ifp))
	{
		if(ifp->ifindex == bfd_sess_info->index)
		{
			for (rn = route_top (IF_OIFS (ifp)); rn; rn = route_next (rn))
			{
		        if ( (oi = rn->info) == NULL)
		        {
		            continue;
		        }
				if(bfd_sess_info->src_ip == ntohl(oi->address->u.prefix4.s_addr))
				{
					flag = 1;
					break;	
				}
			}
		}
		if(flag)
		{
			break;
		}
	}
	
	peer_addr.s_addr = htonl(bfd_sess_info->dst_ip);
	if(IS_DEBUG_OSPF_BFD)
	{
		zlog_debug(OSPF_DBG_BFD, " peer_addr = %s \n", inet_ntoa(peer_addr));
	}
	if(oi)
	{
		nbr = ospf_nbr_lookup_by_addr (oi->nbrs, &peer_addr);
	  	if (nbr)
	  	{
			nbr->bfd_session_info.cc_multiplier = bfd_sess_info->remote_multiplier;
			nbr->bfd_session_info.cc_interval_recv = bfd_sess_info->recv_interval;
			nbr->bfd_session_info.cc_interval_send = bfd_sess_info->send_interval;
			nbr->bfd_session_info.status = bfd_sess_info->state;
			nbr->bfd_session_info.session_id = bfd_sess_info->sess_id;
		}
	}
	else
		zlog_debug(OSPF_DBG_BFD, " The oi is not exist that BFD reported.ifindex = %d\n", bfd_sess_info->index);

	
}

void
ospf_bfd_if_update (struct ospf_interface *oi)
{
	if(IS_DEBUG_OSPF_BFD)
	{
		zlog_debug(OSPF_DBG_BFD, "%s[%d]: In function '%s' ", __FILE__, __LINE__, __func__);
	}
	if(OSPF_IF_PARAM(oi, bfd_flag))
	{

		ospf_bfd_add_session_by_interface (oi);
	}
	else
	{

		ospf_bfd_delete_session_by_interface (oi);
	}
}




