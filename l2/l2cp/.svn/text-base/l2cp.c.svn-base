
#include <lib/log.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <lib/memory.h>
#include <lib/oam_common.h>
#include <lib/command.h>
#include <lib/ether.h>
#include <memory.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include "l2cp/l2cp.h"
#include "l2_msg.h"

#if 1
/*****l2cp list*****/
static struct l2cp_info *l2cp_info_new(void)
{
	struct l2cp_info *pNode = NULL;

	pNode = (struct l2cp_info *)XMALLOC (MTYPE_L2, sizeof (struct l2cp_info));
    if(NULL != pNode)
    {
        pNode->next = NULL;
        pNode->prev = NULL;
        pNode->master = NULL;
    }
    else
    {
        zlog_err ( "\n\r%s(%d): l2cp config malloc new l2cp_info node failed\n", __FUNCTION__, __LINE__ );
    }
    
    return pNode;
}

void
l2cp_info_free(struct l2cp_info *l2cp_ptr)
{
	if (NULL != l2cp_ptr)
	{
		XFREE(MTYPE_L2 ,l2cp_ptr);	
	}
}

void
l2cp_info_delete(struct l2cp_info *l2cp_ptr)
{
	struct l2cp_process *master = NULL;
	if (NULL != l2cp_ptr)
	{
		master = l2cp_ptr->master;
		if (NULL != master)
		{
			if (NULL != l2cp_ptr->next)
			{
        		l2cp_ptr->next->prev = l2cp_ptr->prev;
			}
    		else
    		{
        		master->tail = l2cp_ptr->prev;
    		}
			
    		if (NULL != l2cp_ptr->prev)
    		{
        		l2cp_ptr->prev->next = l2cp_ptr->next;
    		}
    		else
    		{
        		master->head = l2cp_ptr->next;
    		}

			l2cp_info_free(l2cp_ptr);
		}
	}
}

void
l2cp_info_clean (struct l2cp_process *master)
{
    struct l2cp_info *l2cp;

	if (master == NULL)
	{
		return;
	}
	
    for (l2cp = master->head; l2cp; l2cp = l2cp->next)
    {
        l2cp_info_delete (l2cp);
    }
    master->head = master->tail = NULL;
	
	return;
}

struct l2cp_info *
l2cp_info_insert(struct l2cp_process *master, u_int8_t *mac,
			u_int16_t ethertype, u_int32_t proto_type)
{
	struct l2cp_info *l2cp = NULL;
	
	if (master == NULL)
	{
		return NULL;
	}

	l2cp = l2cp_info_new();
	if (l2cp == NULL)
	{
		return NULL;
	}

	l2cp->master = master;
	if (NULL != master->tail)
    {
        master->tail->next = l2cp;
        l2cp->prev = master->tail;
    }
    else
        master->head = l2cp;

    master->tail = l2cp;
    l2cp->next = NULL;

	memcpy(l2cp->dmac, mac, 6);
	l2cp->etherType 	= ethertype;
	l2cp->proto_type	= proto_type;
	return l2cp;
}

struct l2cp_info *
l2cp_info_lookup_by_proto(struct l2cp_process *master, 
 	u_int8_t *mac, u_int16_t ethertype, u_int32_t proto_type)
{
	struct l2cp_info *l2cp = NULL;
	
    if (master == NULL || NULL == mac)
    {
        return NULL;
    }

	for (l2cp = master->head; l2cp; l2cp = l2cp->next)
	{
		if ((memcmp(l2cp->dmac, mac, 6) == 0) && 
			(l2cp->etherType == ethertype) && 
			(l2cp->proto_type == proto_type))
		{
			break;
		}
	}
  
    return l2cp;
}

struct l2cp_info *
l2cp_info_get(struct l2cp_process *master, u_int8_t *mac, 
		u_int16_t ethertype, u_int32_t proto_type)
{
	struct l2cp_info *l2cp = NULL;

	if (master == NULL)
	{
		return NULL;
	}

	l2cp = l2cp_info_lookup_by_proto(master, mac, ethertype, proto_type);
	if (l2cp == NULL)
	{
		l2cp = l2cp_info_insert(master, mac, ethertype, proto_type);
	}

	return l2cp;
}

int
l2cp_info_master_init(struct l2if * p_l2if)
{
	struct l2cp_process *master = NULL;

	if (p_l2if == NULL)
	{
		return L2CP_ERROR;
	}
	
    master = (struct l2cp_process *)XMALLOC (MTYPE_L2, sizeof (struct l2cp_process));
    if (NULL == master)
	{
		return L2CP_ERROR;
	}

    p_l2if->l2cp_master = master;
    master->head = master->tail = NULL;
    return L2CP_SUCCESS;
}

int
l2cp_info_master_deinit(struct l2if * p_l2if)
{
	if (p_l2if == NULL)
	{
		return L2CP_ERROR;
	}
	
	if (p_l2if->l2cp_master)
	{
		l2cp_info_clean(p_l2if->l2cp_master);
		XFREE(MTYPE_L2, p_l2if->l2cp_master);
		p_l2if->l2cp_master = NULL;
	}

	return L2CP_SUCCESS;
}
#endif


enum hal_l2_proto
l2cp_str_to_protocol (u_int8_t *proto_str)
{

  if (0 == strncmp ((char *)proto_str, "stp", 3))
    return HAL_PROTO_STP;
  else if (0 == strncmp ((char *)proto_str, "rstp", 4))
    return HAL_PROTO_RSTP;
  else if (0 == strncmp ((char *)proto_str, "mstp", 4))
    return HAL_PROTO_MSTP;
  else if (0 == strncmp ((char *)proto_str, "lacp", 4))
    return HAL_PROTO_LACP;
  else if (0 == strncmp ((char *)proto_str, "dot1x", 5))
    return HAL_PROTO_DOT1X;
  else if (0 == strncmp ((char *)proto_str, "lldp", 4))
  	return HAL_PROTO_LLDP;
  else if (0 == strncmp ((char *)proto_str, "eoam", 4))
  	return HAL_PROTO_EOAM;
  else if (0 == strncmp ((char *)proto_str, "all", 3))
  	return HAL_PROTO_ALL;
  
  return HAL_PROTO_MAX;
}

const char *
l2cp_proto_to_str (hal_l2_proto_t proto)
{
	switch(proto)
	{
		case HAL_PROTO_STP:
		case HAL_PROTO_RSTP:
		case HAL_PROTO_MSTP:
			return "stp";
		
	/*	case HAL_PROTO_GMRP:
			return "gmrp";

		case HAL_PROTO_GVRP:
			return "gvrp";

		case HAL_PROTO_MMRP:
			return "mmrp";

		case HAL_PROTO_MVRP:
			return "mvrp";*/
			
		case HAL_PROTO_LACP:
			return "lacp";
			
		case HAL_PROTO_DOT1X:
			return "dot1x";
			
		case HAL_PROTO_LLDP:
			return "lldp";
			
		case HAL_PROTO_EOAM:
			return "eoam";

		case HAL_PROTO_MAX:
			return "mac";

		default:
			break;
	}

	return "invalid";
}
/*****l2cp function*****/

int 
hal_l2cp_mpls_process(uint32_t ifindex,enum hal_l2_proto proto, u_int8_t *dmac, 
						u_int16_t ethertype, enum l2cp_mode mode, uint32_t vc_id)
{
    int ret = L2CP_ERROR;
    struct l2cp_info stl2cp_infor;

    memset(&stl2cp_infor,0,sizeof(struct l2cp_info));
    stl2cp_infor.l2cpAction		      = L2CP_TUNNEL;
	stl2cp_infor.l2cpMode             = mode;
    stl2cp_infor.etherType            = ethertype;
    stl2cp_infor.proto_type           = proto;
	stl2cp_infor.mpls_info.vc_id 	  = vc_id;
	memcpy(stl2cp_infor.dmac, dmac, 6);
    
    ret = l2_msg_send_hal_wait_ack( &stl2cp_infor, sizeof(struct l2cp_info), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2CP, L2CP_TUNNEL, IPC_OPCODE_ADD, ifindex);  
    if (L2CP_SUCCESS != ret)
    {
        zlog_err ( "\n\r%s(%d): l2cp config send message failed, error_code = %d\n", __FUNCTION__, __LINE__, ret );
    }
    return ret;
  
}

int 
hal_l2cp_tunnel_process(uint32_t ifindex,enum hal_l2_proto proto, enum l2cp_mode mode,
			u_int8_t *dmac, u_int16_t ethertype, u_int8_t *gmac, u_int16_t vlan, u_int8_t cos)
{
    int ret = L2CP_ERROR;
    struct l2cp_info stl2cp_infor;

    memset(&stl2cp_infor,0,sizeof(struct l2cp_info));
    stl2cp_infor.l2cpAction		 = L2CP_TUNNEL;
	stl2cp_infor.l2cpMode        = mode;
    stl2cp_infor.etherType         = ethertype;
    stl2cp_infor.proto_type         = proto;
    memcpy(stl2cp_infor.dmac, dmac, 6);
	stl2cp_infor.mac_info.vlan 		 = vlan;
	stl2cp_infor.mac_info.cos 		 = cos;
	memcpy(stl2cp_infor.mac_info.mac, gmac, 6);
    
    ret = l2_msg_send_hal_wait_ack( &stl2cp_infor, sizeof(struct l2cp_info), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2CP, L2CP_TUNNEL, IPC_OPCODE_ADD, ifindex); 
    if (L2CP_SUCCESS != ret)
    {
        zlog_err ( "\n\r%s(%d): l2cp config send message failed, error_code = %d\n", __FUNCTION__, __LINE__, ret );
    }
    return ret;
  
}

int 
hal_l2cp_discard_process(uint32_t ifindex,enum hal_l2_proto proto, 
			u_int8_t *dmac, u_int16_t ethertype)
{
    int ret = L2CP_ERROR;
    struct l2cp_info stl2cp_infor;

    memset(&stl2cp_infor,0,sizeof(struct l2cp_info));
    stl2cp_infor.l2cpAction		 = L2CP_DISCARD;
    stl2cp_infor.etherType         = ethertype;
    stl2cp_infor.proto_type         = proto;
    memcpy(stl2cp_infor.dmac, dmac, 6);
    
    ret = l2_msg_send_hal_wait_ack( &stl2cp_infor, sizeof(struct l2cp_info), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2CP, L2CP_DISCARD, IPC_OPCODE_ADD, ifindex);  
    if (L2CP_SUCCESS != ret)
    {
        zlog_err ( "\n\r%s(%d): l2cp config send message failed, error_code = %d\n", __FUNCTION__, __LINE__, ret );
    }
    return ret;
}

int 
hal_l2cp_noaction_process(uint32_t ifindex,enum hal_l2_proto proto, 
			u_int8_t *dmac, u_int16_t ethertype)
{
    int ret = L2CP_ERROR;
    struct l2cp_info stl2cp_infor;

    memset(&stl2cp_infor,0,sizeof(struct l2cp_info));
    stl2cp_infor.l2cpAction		 = L2CP_NOACTION;
    stl2cp_infor.etherType         = ethertype;
    stl2cp_infor.proto_type         = proto;
    memcpy(stl2cp_infor.dmac, dmac, 6);

    ret = l2_msg_send_hal_wait_ack( &stl2cp_infor, sizeof(struct l2cp_info), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2CP, L2CP_NOACTION, IPC_OPCODE_DELETE, ifindex);  
    if (L2CP_SUCCESS != ret)
    {
        zlog_err ( "\n\r%s(%d): l2cp config send message failed, error_code = %d\n", __FUNCTION__, __LINE__, ret );
    }
    return ret;
}
		

int
l2cp_mpls_process(struct l2if * p_l2if, enum hal_l2_proto proto, u_int8_t * dmac,
					   u_int16_t ethertype,enum l2cp_mode mode, uint32_t vc_id)
{
	struct l2cp_info *pstl2cp = NULL;
	int ret;

	/*check conflict*/
	
	if(NULL == p_l2if->l2cp_master)
	{
		ret = l2cp_info_master_init(p_l2if);
		if(L2CP_SUCCESS != ret)
		{
			return ret;
		}
	}

	ret = hal_l2cp_mpls_process(p_l2if->ifindex, proto, dmac, ethertype, mode, vc_id);
	
	/******save info******/
	pstl2cp = l2cp_info_get(p_l2if->l2cp_master, dmac, ethertype, proto);
	pstl2cp->l2cpAction 	 = L2CP_TUNNEL;
	pstl2cp->l2cpMode		 = mode;
	pstl2cp->mpls_info.vc_id = vc_id;
	if(L2CP_SUCCESS == ret)
	{
		pstl2cp->mpls_info.status = L2CP_MPLS_ACTIVE;
	}
	else
	{
		pstl2cp->mpls_info.status = L2CP_MPLS_INACTIVE;
		zlog_err ( "\n\r%s(%d): l2cp config failed, error_code = %d\n", __FUNCTION__, __LINE__, ret );
	}
	
	return ret;
}

int
l2cp_tunnel_process(struct l2if * p_l2if, enum hal_l2_proto proto, enum l2cp_mode mode,
			u_int8_t *dmac, u_int16_t ethertype, u_int8_t *gmac, u_int16_t vlan, u_int8_t cos)
{
	struct l2cp_info *pstl2cp = NULL;
    int ret;

	/*check conflict*/
    
    if(NULL == p_l2if->l2cp_master)
    {
        ret = l2cp_info_master_init(p_l2if);
        if(L2CP_SUCCESS != ret)
        {
            return ret;
        }
    }
	pstl2cp = l2cp_info_lookup_by_proto(p_l2if->l2cp_master, dmac, ethertype, proto);
	if (NULL != pstl2cp)
	{
		if ((pstl2cp->l2cpAction == L2CP_TUNNEL)&& (pstl2cp->mac_info.vlan == vlan)&&(pstl2cp->l2cpMode == mode)\
            && (pstl2cp->mac_info.cos == cos)&& (!memcmp(pstl2cp->mac_info.mac, gmac, 6)))
		{
			return L2CP_SUCCESS;
		}
	}
	/******hal_process******/
	ret = hal_l2cp_tunnel_process(p_l2if->ifindex, proto, mode, dmac, ethertype, gmac, vlan, cos);
	
	/******save info******/
	if(L2CP_SUCCESS == ret)
	{
        pstl2cp = l2cp_info_get(p_l2if->l2cp_master, dmac, ethertype, proto);
    	pstl2cp->l2cpAction		 = L2CP_TUNNEL;
		pstl2cp->l2cpMode        = mode;
    	pstl2cp->mac_info.vlan 		 = vlan;
    	pstl2cp->mac_info.cos 		 = cos;
    	memcpy(pstl2cp->mac_info.mac, gmac, 6);
    }
    else
    {
        zlog_err ( "\n\r%s(%d): l2cp config failed, error_code = %d\n", __FUNCTION__, __LINE__, ret );
    }
	
	return ret;
}

int
l2cp_discard_process(struct l2if * p_l2if, enum hal_l2_proto proto, 
			u_int8_t *dmac, u_int16_t ethertype, enum l2cp_action action)
{
    struct l2cp_info *pstl2cp = NULL;
    int ret = 0;
    
	/*check conflict */ 
    
    if(NULL == p_l2if->l2cp_master)
    {
        ret = l2cp_info_master_init(p_l2if);
        if(L2CP_SUCCESS != ret)
        {
            return ret;
        }
    }
	pstl2cp = l2cp_info_lookup_by_proto(p_l2if->l2cp_master, dmac, ethertype, proto);
	if (NULL != pstl2cp)
	{
		if (pstl2cp->l2cpAction == action)
		{
			return L2CP_SUCCESS;
		}
	}

	ret = hal_l2cp_discard_process(p_l2if->ifindex, proto, dmac, ethertype);
	
	/******save info******/
	if(L2CP_SUCCESS == ret)
	{
        pstl2cp = l2cp_info_get(p_l2if->l2cp_master, dmac, ethertype, proto);
	    pstl2cp->l2cpAction	= action;
    }
    else
    {
        zlog_err ( "\n\r%s(%d): l2cp config failed, error_code = %d\n", __FUNCTION__, __LINE__, ret );
    }

	return ret;
}

int
l2cp_no_action(struct l2if * p_l2if, enum hal_l2_proto proto, 
			u_int8_t *dmac, u_int16_t ethertype)
{
	struct l2cp_info     *pst_l2cp = NULL;
	struct l2cp_info     *pst_next_l2cp = NULL;
    struct l2cp_process  *l2cp_master = NULL; 
    int                  ret = L2CP_SUCCESS;
    uint32_t             ifindex = p_l2if->ifindex;
	/*check conflict  */

    if(HAL_PROTO_ALL == proto)
    {
        l2cp_master = p_l2if->l2cp_master;
        if(NULL == l2cp_master)
        {
            return L2CP_SUCCESS;
        }

        pst_l2cp = l2cp_master->head;
        while(NULL != pst_l2cp)
        {
           ret = hal_l2cp_noaction_process(ifindex, pst_l2cp->proto_type, pst_l2cp->dmac, pst_l2cp->etherType);
           if(L2CP_SUCCESS == ret)
    	    {
    	    	if(pst_l2cp->next != NULL)
    	    	{
    	    		pst_next_l2cp = pst_l2cp->next;
    	    	}
				else
				{
					pst_next_l2cp = NULL;
				}
                l2cp_info_delete(pst_l2cp);
				pst_l2cp = pst_next_l2cp;
            }
            else
            {
                zlog_err ( "\n\r%s(%d): l2cp config failed, error_code = %d\n", __FUNCTION__, __LINE__, ret );
                return L2CP_ERROR;
            }
            
        }
		
        l2cp_master = p_l2if->l2cp_master;
        if(NULL != l2cp_master)
        {
            if(NULL == l2cp_master->head && NULL == l2cp_master->tail)
            {
                l2cp_info_master_deinit(p_l2if);
            }
        }
        
        return L2CP_SUCCESS;
    }
    else
	{
        pst_l2cp = l2cp_info_lookup_by_proto(p_l2if->l2cp_master, dmac, ethertype, proto);
    	if (NULL != pst_l2cp)
    	{
            /******hal_process******/
    	    ret = hal_l2cp_noaction_process(p_l2if->ifindex, proto, dmac, ethertype);
    	    if(L2CP_SUCCESS == ret)
    	    {
                l2cp_info_delete(pst_l2cp);
                l2cp_master = p_l2if->l2cp_master;
                if(NULL != l2cp_master)
                {
                    if(NULL == l2cp_master->head && NULL == l2cp_master->tail)
                    {
                        l2cp_info_master_deinit(p_l2if);
                    }
                }
                return L2CP_SUCCESS;
            }
            else
            {
                zlog_err ( "\n\r%s(%d): l2cp config failed, error_code = %d\n", __FUNCTION__, __LINE__, ret );
            }
    	}
    }
    return L2CP_ERROR;
}

int 
l2cp_show_config(struct vty *vty, struct l2if * p_l2if,enum l2cp_action action,enum l2cp_mode l2cpMode)
{	
	struct l2cp_process *master = NULL;
	struct l2cp_info *l2cp_temp = NULL;

	if (vty == NULL || NULL == p_l2if)
	{
		return L2CP_ERROR;
	}
	master = p_l2if->l2cp_master;
	if (master == NULL)
	{
		return L2CP_ERROR;
	}  
	
    if (action == L2CP_TUNNEL && l2cpMode < L2CP_BASE_VPWS)
	{
	    for (l2cp_temp = master->head; l2cp_temp; l2cp_temp = l2cp_temp->next)
    	{
            if(l2cp_temp->l2cpAction != L2CP_TUNNEL || l2cp_temp->l2cpMode > L2CP_BASE_VLAN)
                continue;
			vty_out (vty, "\r%-10s", l2cp_proto_to_str(l2cp_temp->proto_type));
			if (l2cp_temp->etherType)
			{
				vty_out (vty, "0x%04x        ", l2cp_temp->etherType);
			}
			else
			{
				vty_out (vty, "%-14s", "------");
			}

			vty_out (vty, "%02x:%02x:%02x:%02x:%02x:%02x%2s%02x:%02x:%02x:%02x:%02x:%02x%2s", 
					l2cp_temp->dmac[0],l2cp_temp->dmac[1], l2cp_temp->dmac[2], l2cp_temp->dmac[3], l2cp_temp->dmac[4], 
					l2cp_temp->dmac[5]," ",l2cp_temp->mac_info.mac[0],l2cp_temp->mac_info.mac[1],l2cp_temp->mac_info.mac[2], 
					l2cp_temp->mac_info.mac[3],l2cp_temp->mac_info.mac[4],l2cp_temp->mac_info.mac[5], " ");

			if (l2cp_temp->mac_info.vlan != VLAN_INVALID)
			{
				vty_out (vty, "%-6d%-5d", l2cp_temp->mac_info.vlan, l2cp_temp->mac_info.cos);
			}
			else
			{
				vty_out (vty, "%-6s%-5s", "----", "----");
			}

			vty_out (vty, " \r\n");	
    	}
	}
	else if(action == L2CP_TUNNEL && l2cpMode > L2CP_BASE_VLAN)
	{
		for (l2cp_temp = master->head; l2cp_temp; l2cp_temp = l2cp_temp->next)
    	{
            if(l2cp_temp->l2cpAction != L2CP_TUNNEL || l2cp_temp->l2cpMode < L2CP_BASE_VPWS)
                continue;
			vty_out (vty, "\r%-10s", l2cp_proto_to_str(l2cp_temp->proto_type));
			if (l2cp_temp->etherType)
			{
				vty_out (vty, "0x%04x        ", l2cp_temp->etherType);
			}
			else
			{
				vty_out (vty, "%-14s", "------");
			}

			vty_out (vty, "%02x:%02x:%02x:%02x:%02x:%02x%3s", 
					l2cp_temp->dmac[0],l2cp_temp->dmac[1], l2cp_temp->dmac[2], l2cp_temp->dmac[3], l2cp_temp->dmac[4], 
					l2cp_temp->dmac[5]," ");

			if (l2cp_temp->l2cpMode == L2CP_BASE_VPWS)
			{
				vty_out (vty, "%-14d%-5s", l2cp_temp->mpls_info.vc_id, "----");
			}
			else if(l2cp_temp->l2cpMode == L2CP_BASE_VPLS)
			{
				vty_out (vty, "%-14s%-5d", "----", l2cp_temp->mpls_info.vc_id);
			}
			else 
			{
				vty_out (vty, "%-14s%-5s", "----", "----");
			}

			vty_out (vty, " \r\n");	
    	}
	}
    else if (action == L2CP_DISCARD)
	{
    	for (l2cp_temp = master->head; l2cp_temp; l2cp_temp = l2cp_temp->next)
    	{
            if(l2cp_temp->l2cpAction != L2CP_DISCARD)
                continue;
			vty_out (vty, "\r%-10s", l2cp_proto_to_str(l2cp_temp->proto_type));
			if (l2cp_temp->etherType)
			{
				vty_out (vty, "0x%-12x", l2cp_temp->etherType);
			}
			else
			{
				vty_out (vty, "%-14s", "------");
			}

			vty_out (vty, "%02x:%02x:%02x:%02x:%02x:%02x%2s", 
					l2cp_temp->dmac[0],l2cp_temp->dmac[1], l2cp_temp->dmac[2], l2cp_temp->dmac[3], l2cp_temp->dmac[4], 
					l2cp_temp->dmac[5], " ");
			vty_out (vty, " \r\n");
    	}
    }
    else
    {
        vty_error_out (vty, "\r unkonw action \n");
		return L2CP_ERROR;
    }
    
	return L2CP_SUCCESS;
}


void 
delete_l2cp_config(uint32_t ifindex, uint32_t mode)
{
	struct l2if *pif= NULL;
	
	pif = l2if_get(ifindex);
	if(NULL == pif)
	{
		//zlog_debug("%s[%d]:leave %s:error:fail to l2if_get ! \n",__FILE__,__LINE__,__func__);
		return;
	}

	if(pif->mode == mode)
		return;
	
	/* l2 模式改变时，删除 swtich 配置 */
	if(pif->mode == IFNET_MODE_SWITCH)
	{
		l2cp_no_action(pif, HAL_PROTO_ALL, NULL, 0);
	}
	
	return;
}

void 
l2cp_init(void)
{
	int ret = L2CP_ERROR;

    ret = ipc_send_msg_n2( NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2CP, 0, IPC_OPCODE_READY, 0); 
    if (L2CP_SUCCESS != ret)
    {
        zlog_err ( "\n\r%s(%d): l2cp config send message failed, error_code = %d\n", __FUNCTION__, __LINE__, ret );
    }
    return ;
}

int 
l2cp_config_write_process(struct vty *vty, struct l2cp_info *pl2cp_info,u_int32_t proto)
{
	#define PROTO_TYPE_MAX_LENGTH 10
	int ret = -1;
	u_int8_t gmac[6];
	u_int8_t proto_type[HAL_PROTO_MAX][PROTO_TYPE_MAX_LENGTH] = {"stp",
																 "stp",
																 "stp",
																 "lacp",
																 "dot1x",
																 "lldp",
																 "eoam",
																	};
	if(proto > HAL_PROTO_MAX)
	{
		return ret;
	}
	
	if(pl2cp_info->l2cpAction == L2CP_TUNNEL)
    {
    	if(pl2cp_info->l2cpMode < L2CP_BASE_VPWS)
    	{
    		memcpy(gmac, pl2cp_info->mac_info.mac, 6);
            if(pl2cp_info->mac_info.vlan != VLAN_INVALID)
            {
                vty_out(vty, " l2cp %s tunnel dmac %02x:%02x:%02x:%02x:%02x:%02x vlan %d cos %d %s",proto_type[proto],
                    gmac[0],gmac[1],gmac[2],gmac[3],gmac[4],gmac[5],pl2cp_info->mac_info.vlan,pl2cp_info->mac_info.cos,VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, " l2cp %s tunnel dmac %02x:%02x:%02x:%02x:%02x:%02x %s",proto_type[proto],
                    gmac[0],gmac[1],gmac[2],gmac[3],gmac[4],gmac[5],VTY_NEWLINE);
            }
    	}
		else if(pl2cp_info->l2cpMode == L2CP_BASE_VPWS)
		{
			vty_out(vty, " l2cp %s mpls vc-id %d %s",proto_type[proto],pl2cp_info->mpls_info.vc_id,VTY_NEWLINE);
		}
		else if(pl2cp_info->l2cpMode == L2CP_BASE_VPLS)
		{
			vty_out(vty, " l2cp %s mpls vsi %d %s",proto_type[proto],pl2cp_info->mpls_info.vc_id,VTY_NEWLINE);
		}
    }
    else if(pl2cp_info->l2cpAction == L2CP_DISCARD)
    {
        vty_out(vty, " l2cp %s discard %s",proto_type[proto],VTY_NEWLINE);
    }   

	return ret;
}


int 
l2if_l2cp_config_write(struct vty *vty, struct l2cp_process *l2cp_master)
{
	struct l2cp_info *pl2cp_info = NULL;

	if(l2cp_master->head == NULL)
	{
		return L2CP_ERROR;
	}
    else
    {
       pl2cp_info = l2cp_master->head;
       
       while(pl2cp_info != NULL)
       {
			l2cp_config_write_process(vty, pl2cp_info, pl2cp_info->proto_type);
            pl2cp_info = pl2cp_info->next;
       }
    }

   return L2CP_SUCCESS;
}


