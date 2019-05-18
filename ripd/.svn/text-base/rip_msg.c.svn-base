#include "stream.h"
#include "log.h"
#include "thread.h"
#include "memory.h"
#include "errcode.h"
#include "ifm_common.h"
#include "lib/snmp_common.h"

#include "route_com.h"
#include "ripd/ripd.h"
#include "ripd/rip_interface.h"
#include "ripd/rip_debug.h"
#include "ripd/rip_msg.h"
#include "msg_ipc.h"

extern struct thread_master *master_rip;
extern struct list *rip_list;


int
rip_interface_get_bulk(uint32_t ifindex, int data_len, struct ripinter_brief ripinter_buff[])
{
    struct interface *ifp;
    struct listnode *node;
	struct rip_instance *rip = NULL;
    struct rip_interface *ri = NULL;
    struct ripinter_brief interbrief;
    
	
    int msg_len = IPC_MSG_LEN / sizeof(struct ripinter_brief);
	int data_num = 0;
    int flag = 0;
	
    memset(ripinter_buff, 0, msg_len*sizeof(struct ripinter_brief));

    for (ALL_LIST_ELEMENTS_RO(iflist, node, ifp))
    {
		if(0 == ifp->ifindex)
		{
			continue;
		}
        if (ifindex && ifp->ifindex == ifindex)
        {
            flag = 1;
            continue;
        }

        if ((1 == flag) || (0 == ifindex))
        {
            ri = (struct rip_interface *)ifp->info;

            if (ri->enable_inter_node)
            {
            	memset(&interbrief, 0, sizeof(struct ripinter_brief));
				
                interbrief.ifindex = ifp->ifindex;
                memcpy(&interbrief.name, &ifp->name, INTERFACE_NAMSIZ);
                interbrief.id = ri->enable_inter_node;
				memcpy(interbrief.auth_password, ri->auth_str, sizeof(interbrief.auth_password));
				interbrief.auth_md5_id =  ri->md5_auth_id;
				if(ri->split_horizon)
				{
					interbrief.split_horizon_status = ri->split_horizon;
				}
				else
				{
					interbrief.split_horizon_status = 2;
				}

				if(ri->poison_reverse == RIP_SPLIT_HORIZON_POISONED_REVERSE)
				{
					interbrief.poison_reverse_status = 1;
				}
				else
				{
					interbrief.poison_reverse_status = 2;
				}

				if(ri->passive)
				{
					interbrief.passive_status = ri->passive;
				}
				else
				{
					interbrief.passive_status = 2;
				}
				interbrief.metric = ifp->metric;
				if (ri->ri_send == RI_RIP_UNSPEC)
				{
					rip = rip_instance_lookup(ri->enable_inter_node);
					if(rip)
					{
						interbrief.rip_version = rip->version_send;
						if(rip->version_send == RI_RIP_VERSION_2)
						{
							interbrief.pkt_send_type =  2;
						}
						else
						{
							interbrief.pkt_send_type =  3;
						}
					}
				}
				else
				{
					interbrief.rip_version = ri->ri_send;
					if(ri->ri_send == RI_RIP_VERSION_2_BROAD)
					{
						interbrief.pkt_send_type = 1;
					}
					else if(ri->ri_send == RI_RIP_VERSION_1)
					{
						interbrief.pkt_send_type = 3;
					}
					else
					{
						interbrief.pkt_send_type = 2;
					}
				}

                memcpy(&ripinter_buff[data_num++], &interbrief, sizeof(struct ripinter_brief));
            }

            if (data_num == msg_len)
            {
                return data_num;
            }
        }
    }

    return data_num;


}

int
rip_instance_get_bulk(uint32_t pid, int data_len, struct rip_brief ripins_buff[])
{
    struct rip_instance *rip;
    struct rip_brief ripbrief;
    struct listnode *node;
    int data_num = 0;
    int msg_num  = IPC_MSG_LEN / sizeof(struct rip_brief);
    int flag = 0;

    if (pid >= listcount(rip_list))
    {
        return 0;
    }

    for (ALL_LIST_ELEMENTS_RO(rip_list, node, rip))
    {
        if (pid && rip->id == pid)
        {
            flag = 1;
            continue;
        }

        if ((1 == flag) || (0 == pid))
        {
        	memset(&ripbrief, 0, sizeof(struct rip_brief));
			
            ripbrief.id = rip->id;
            ripbrief.version = rip->version_recv;
            ripbrief.distance = rip->distance;
            ripbrief.update_time = (uint32_t)rip->update_time;
            ripbrief.timeout_time = (uint32_t)rip->timeout_time;
            ripbrief.garbage_time = (uint32_t)rip->garbage_time;
			if(rip->summaryflag)
			{
				ripbrief.auto_summary = rip->summaryflag;
			}
			else
			{
				ripbrief.auto_summary = 2;
			}
            memcpy(&ripins_buff[data_num++], &ripbrief, sizeof(struct rip_brief));

            if (data_num == msg_num)
            {
                return data_num;
            }
        }

    }

    return data_num;
}

static int
rip_redis_route_get_bulk(void *pdata, int data_len, struct rip_redistribute_route_info rip_redis_route_buff[])
{
	struct rip_redistribute_route_key rip_key = *((struct rip_redistribute_route_key*)pdata);
	struct rip_redistribute_route_info s_rip_redis_route;
	struct rip_instance *rip;
	struct listnode *node = NULL;
	struct listnode *node1 = NULL;
	struct redist_msg *rm = NULL;
	int flag = 0;
	
	
	int msg_num  = IPC_MSG_LEN/sizeof(struct rip_redistribute_route_info);
	//int msg_num  = 2;//pressure test
    int data_num = 0;

	if (listcount(rip_list) == 0)
    {
        return 0;
    }

	zlog_debug((RIP_DBG_EVENT|RIPNG_DBG_EVENT), "fun:%s line:%d snmp get rip_id:%d, route_type:%d ,instances:%d\n", __func__, __LINE__,\
								rip_key.rip_id_key,\
								rip_key.redis_route_type_key, rip_key.redis_target_instance_id_key);

	if(rip_key.rip_id_key == 0)   /*  1st get */
	{
		for (ALL_LIST_ELEMENTS_RO (rip_list, node, rip))
		{
			for (ALL_LIST_ELEMENTS_RO(rip->redist_infos, node1, rm))
			{
				if (!rm)
				{
		            continue;
				}
				
				memset(&s_rip_redis_route, 0, sizeof(struct rip_redistribute_route_info));
				s_rip_redis_route.rip_id = rip->id;
				s_rip_redis_route.redis_route_type = rm->type;
				s_rip_redis_route.redis_target_instance_id = rm->id;
				if(rm->metric < 16)
				{
					s_rip_redis_route.redis_route_metric = rm->metric;
				}

				zlog_debug((RIP_DBG_EVENT|RIPNG_DBG_EVENT), "fun:%s line:%d ack rip_id:%d, route_type:%d ,instances:%d\n", __func__, __LINE__,\
							s_rip_redis_route.rip_id,\
							s_rip_redis_route.redis_route_type, s_rip_redis_route.redis_target_instance_id);
							
				memcpy(&rip_redis_route_buff[data_num++], &s_rip_redis_route, sizeof(struct rip_redistribute_route_info));
                if (data_num == msg_num)
                {
                    return data_num;
                }
			}
			
		}
	}
	else
	{
		for (ALL_LIST_ELEMENTS_RO (rip_list, node, rip))
		{
			if(rip->id >= rip_key.rip_id_key)
			{
				for (ALL_LIST_ELEMENTS_RO(rip->redist_infos, node1, rm))
				{
					if (!rm)
					{
			            continue;
					}
					if(rm->type == rip_key.redis_route_type_key && rm->id == rip_key.redis_target_instance_id_key)
					{
						flag = 1;
						continue;
					}

					if (flag)
					{
						memset(&s_rip_redis_route, 0, sizeof(struct rip_redistribute_route_info));
						s_rip_redis_route.rip_id = rip->id;
						s_rip_redis_route.redis_route_type = rm->type;
						s_rip_redis_route.redis_target_instance_id = rm->id;
						if(rm->metric < 16)
						{
							s_rip_redis_route.redis_route_metric = rm->metric;
						}

						zlog_debug((RIP_DBG_EVENT|RIPNG_DBG_EVENT), "fun:%s line:%d ack rip_id:%d, route_type:%d ,instances:%d\n", __func__, __LINE__,\
									s_rip_redis_route.rip_id,\
									s_rip_redis_route.redis_route_type, s_rip_redis_route.redis_target_instance_id);
									
						memcpy(&rip_redis_route_buff[data_num++], &s_rip_redis_route, sizeof(struct rip_redistribute_route_info));
		                if (data_num == msg_num)
		                {
		                    return data_num;
		                }
					}
				}
			}
		}
	}
	return data_num;
}


static int
rip_route_sum_get_bulk(void *pdata, int data_len, struct rip_route_summary_info rip_route_sum_buff[])
{
	struct rip_route_summary_key rip_key = *((struct rip_route_summary_key*)pdata);
	struct rip_route_summary_info s_rip_route_sum;
	struct listnode *node = NULL;
	struct listnode *snode = NULL;
	struct interface *ifp = NULL;
	struct rip_interface *ri = NULL;
	struct rip_summary *rs = NULL;
	int flag = 0;
	
	
	int msg_num  = IPC_MSG_LEN/sizeof(struct rip_route_summary_info);
	//int msg_num  = 2;//pressure test
    int data_num = 0;

	if (listcount(rip_list) == 0)
    {
        return 0;
    }

	zlog_debug((RIP_DBG_EVENT|RIPNG_DBG_EVENT), "fun:%s line:%d snmp get ifindex:%d, summary_net_ip:%s\%d\n", __func__, __LINE__,\
								rip_key.ifindex_key,\
								inet_ntoa(rip_key.sum_net_ip_key), ip_masklen(rip_key.sum_net_mask_key));

	if(rip_key.ifindex_key == 0)   /*  1st get */
	{
		for (ALL_LIST_ELEMENTS_RO(iflist, node, ifp))
		{
			ri = (struct rip_interface *)ifp->info;
			if (ri->summary && listcount(ri->summary))
			{
				for (ALL_LIST_ELEMENTS_RO(ri->summary, snode, rs))
	            {
	                if (!rs->dflag)
	                {
						memset(&s_rip_route_sum, 0, sizeof(struct rip_route_summary_info));
						s_rip_route_sum.ifindex = ifp->ifindex;
						s_rip_route_sum.sum_net_ip = rs->p.u.prefix4;
						masklen_to_netip (rs->p.prefixlen, &(s_rip_route_sum.sum_net_mask));
						memcpy(s_rip_route_sum.sum_if_desc, ifp->name, sizeof(ifp->name));

						zlog_debug((RIP_DBG_EVENT|RIPNG_DBG_EVENT), "fun:%s line:%d ack ifindex:%d, summary_net_ip:%s\%d\n", __func__, __LINE__,\
									s_rip_route_sum.ifindex,\
									inet_ntoa(s_rip_route_sum.sum_net_ip), ip_masklen(s_rip_route_sum.sum_net_mask));
									
						memcpy(&rip_route_sum_buff[data_num++], &s_rip_route_sum, sizeof(struct rip_route_summary_info));
		                if (data_num == msg_num)
		                {
		                    return data_num;
		                }
	                }
	            }
			}
		}
	}
	else
	{
		for (ALL_LIST_ELEMENTS_RO(iflist, node, ifp))
		{
			if(ifp->ifindex >= rip_key.ifindex_key)
			{
				ri = (struct rip_interface *)ifp->info;
				if (ri->summary && listcount(ri->summary))
				{
					for (ALL_LIST_ELEMENTS_RO(ri->summary, snode, rs))
		            {
		                if (!rs->dflag)
		                {
		                	if((rs->p.u.prefix4.s_addr == rip_key.sum_net_ip_key.s_addr)\
								&& (rs->p.prefixlen == ip_masklen(rip_key.sum_net_mask_key)))
	                		{
								flag = 1;
								continue;
							}

							if(flag)
							{
								memset(&s_rip_route_sum, 0, sizeof(struct rip_route_summary_info));
								s_rip_route_sum.ifindex = ifp->ifindex;
								s_rip_route_sum.sum_net_ip = rs->p.u.prefix4;
								masklen_to_netip (rs->p.prefixlen, &(s_rip_route_sum.sum_net_mask));
								memcpy(s_rip_route_sum.sum_if_desc, ifp->name, sizeof(ifp->name));

								zlog_debug((RIP_DBG_EVENT|RIPNG_DBG_EVENT), "fun:%s line:%d ack ifindex:%d, summary_net_ip:%s\%d\n", __func__, __LINE__,\
									s_rip_route_sum.ifindex,\
									inet_ntoa(s_rip_route_sum.sum_net_ip), ip_masklen(s_rip_route_sum.sum_net_mask));
									
								memcpy(&rip_route_sum_buff[data_num++], &s_rip_route_sum, sizeof(struct rip_route_summary_info));
				                if (data_num == msg_num)
				                {
				                    return data_num;
				                }
							}
		                }
		            }
				}
			}
		}
	}

	return data_num;
}


static int
rip_if_nbr_get_bulk(void *pdata, int data_len, struct rip_if_nbr_info rip_if_nbr_buff[])
{
	struct rip_if_nbr_key rip_key = *((struct rip_if_nbr_key*)pdata);
	struct rip_if_nbr_info s_rip_if_nbr;
	struct listnode *node = NULL;
	struct listnode *snode = NULL;
	struct interface *ifp = NULL;
	struct rip_interface *ri = NULL;
	struct prefix *nbr_p = NULL;
	int flag = 0;
	
	
	int msg_num  = IPC_MSG_LEN/sizeof(struct rip_if_nbr_info);
	//int msg_num  = 2;//pressure test
    int data_num = 0;

	if (listcount(rip_list) == 0)
    {
        return 0;
    }

	zlog_debug((RIP_DBG_EVENT|RIPNG_DBG_EVENT), "fun:%s line:%d snmp get ifindex:%d, remote_nbr:%s\n", __func__, __LINE__,\
								rip_key.ifindex_key,\
								inet_ntoa(rip_key.remote_nbr_ip_key));

	if(rip_key.ifindex_key == 0)   /*  1st get */
	{
		for (ALL_LIST_ELEMENTS_RO(iflist, node, ifp))
		{
			ri = (struct rip_interface *)ifp->info;
			if (ri->neighbor && listcount(ri->neighbor))
			{
				for (ALL_LIST_ELEMENTS_RO(ri->neighbor, snode, nbr_p))
	            {
					memset(&s_rip_if_nbr, 0, sizeof(struct rip_if_nbr_info));
					s_rip_if_nbr.ifindex = ifp->ifindex;
					s_rip_if_nbr.remote_nbe_ip = nbr_p->u.prefix4;
					memcpy(s_rip_if_nbr.nbr_if_desc, ifp->name, sizeof(ifp->name));

					zlog_debug((RIP_DBG_EVENT|RIPNG_DBG_EVENT), "fun:%s line:%d ack ifindex:%d, remote_nbr:%s\n", __func__, __LINE__,\
								s_rip_if_nbr.ifindex,\
								inet_ntoa(s_rip_if_nbr.remote_nbe_ip));
								
					memcpy(&rip_if_nbr_buff[data_num++], &s_rip_if_nbr, sizeof(struct rip_if_nbr_info));
	                if (data_num == msg_num)
	                {
	                    return data_num;
	                }
	            }
			}
		}
	}
	else
	{
		for (ALL_LIST_ELEMENTS_RO(iflist, node, ifp))
		{
			if(ifp->ifindex >= rip_key.ifindex_key)
			{
				ri = (struct rip_interface *)ifp->info;
				if (ri->neighbor && listcount(ri->neighbor))
				{
					for (ALL_LIST_ELEMENTS_RO(ri->neighbor, snode, nbr_p))
		            {
	                	if(nbr_p->u.prefix4.s_addr == rip_key.remote_nbr_ip_key.s_addr)
                		{
							flag = 1;
							continue;
						}

						if(flag)
						{
							memset(&s_rip_if_nbr, 0, sizeof(struct rip_if_nbr_info));
							s_rip_if_nbr.ifindex = ifp->ifindex;
							s_rip_if_nbr.remote_nbe_ip = nbr_p->u.prefix4;
							memcpy(s_rip_if_nbr.nbr_if_desc, ifp->name, sizeof(ifp->name));

							zlog_debug((RIP_DBG_EVENT|RIPNG_DBG_EVENT), "fun:%s line:%d ack ifindex:%d, remote_nbr:%s\n", __func__, __LINE__,\
										s_rip_if_nbr.ifindex,\
										inet_ntoa(s_rip_if_nbr.remote_nbe_ip));
										
							memcpy(&rip_if_nbr_buff[data_num++], &s_rip_if_nbr, sizeof(struct rip_if_nbr_info));
			                if (data_num == msg_num)
			                {
			                    return data_num;
			                }
						}
		            }
				}
			}
		}
	}

	return data_num;
}

void
rip_ifm_manage_new(struct ipc_mesg_n *mesg)
{
    struct ipc_msghdr_n *pmsghdr = NULL;
    void *pdata = NULL;
    struct ifm_event *pevent = NULL;
    uint32_t ifindex;
    uint32_t pid = 0;
    vrf_id_t vrf_id = VRF_DEFAULT;
    int ret = 0;

    /* process the ipc message */
    pmsghdr = &mesg->msghdr;
    pdata = mesg->msg_data;

    if (pmsghdr->msg_type == IPC_TYPE_IFM)
    {
        if (pmsghdr->opcode == IPC_OPCODE_EVENT)
        {
            pevent = (struct ifm_event *)pdata;
            ifindex = pevent->ifindex;

            //if (IS_RIP_DEBUG_EVENT || IS_RIPNG_DEBUG_EVENT)
                zlog_debug((RIP_DBG_EVENT|RIPNG_DBG_EVENT), "rip interface event receive, event type %d, interface ifindex %02x, upflag %d, mode %d\n.",
                           pmsghdr->msg_subtype, ifindex, pevent->up_flag, pevent->mode);


            switch (pmsghdr->msg_subtype)
            {
                case IFNET_EVENT_IP_ADD:
                    rip_interface_address_add(pevent, vrf_id);
                    break;
                case IFNET_EVENT_IP_DELETE:
                    rip_interface_address_delete(pevent, vrf_id);
                    break;
                case IFNET_EVENT_UP:
                    rip_interface_up(pevent, vrf_id);
                    break;
                case IFNET_EVENT_DOWN:
                    rip_interface_down(pevent, vrf_id);
                    break;
                case IFNET_EVENT_IF_DELETE:
                    rip_interface_delete(pevent, vrf_id);
                    break;
                case IFNET_EVENT_MODE_CHANGE:
                    rip_interface_change_mode(pevent, vrf_id);
                    break;
                default:
                    break;
            }

        }
    }
    else if (pmsghdr->msg_type == IPC_TYPE_RIP)
    {
        if (pmsghdr->opcode == IPC_OPCODE_GET_BULK)
        {
        	switch(pmsghdr->msg_subtype)
        	{
            	case RIP_SNMP_INSTANCE_GET:  /* get rip instance brief */
		            {
		                int msg_num  = IPC_MSG_LEN / sizeof(struct rip_brief);
		                struct rip_brief ripins_buff[IPC_MSG_LEN/sizeof(struct rip_brief)];
		                pid = *((uint32_t*)pdata);
		                memset(ripins_buff, 0, msg_num*sizeof(struct rip_brief));
		                ret = rip_instance_get_bulk(pid, pmsghdr->data_len, ripins_buff);

		                if (ret > 0)
		                {
		                    /*ipc_send_reply_bulk(ripins_buff, ret * sizeof(struct rip_brief), ret, pmsghdr->sender_id,
		                                              MODULE_ID_RIP, IPC_TYPE_RIP, pmsghdr->msg_subtype, pmsghdr->msg_index);*/
							ipc_ack_to_snmp(pmsghdr, ripins_buff, ret * sizeof(struct rip_brief), ret);
		                }
		                else
		                {
		                    /*ipc_send_noack(ERRNO_NOT_FOUND, pmsghdr->sender_id, MODULE_ID_RIP, IPC_TYPE_RIP,
		                                         pmsghdr->msg_subtype, pmsghdr->msg_index);*/
							ipc_noack_to_snmp(pmsghdr);
		                }
		            }
					break;
            	case RIP_SNMP_INTERFACE_GET:
		            {
		                int msg_len = IPC_MSG_LEN / sizeof(struct ripinter_brief);
		                struct ripinter_brief ripinter_buff[IPC_MSG_LEN/sizeof(struct ripinter_brief)];
		                pid = *((uint32_t*)pdata);
		                memset(ripinter_buff, 0, msg_len*sizeof(struct ripinter_brief));
		                ret = rip_interface_get_bulk(pid, pmsghdr->data_len, ripinter_buff);

		                if (ret > 0)
		                {
		                     /*ipc_send_reply_bulk(ripinter_buff, ret * sizeof(struct ripinter_brief), ret, pmsghdr->sender_id,
		                                              MODULE_ID_RIP, IPC_TYPE_RIP, pmsghdr->msg_subtype, pmsghdr->msg_index);*/
		                     ipc_ack_to_snmp(pmsghdr, ripinter_buff, ret * sizeof(struct ripinter_brief), ret);
		                }
		                else
		                {
		                     /*ipc_send_noack(ERRNO_NOT_FOUND, pmsghdr->sender_id, MODULE_ID_RIP, IPC_TYPE_RIP,
		                                         pmsghdr->msg_subtype, pmsghdr->msg_index);*/
							 ipc_noack_to_snmp(pmsghdr);
		                }

		            }
					break;
				case RIP_SNMP_REDIS_ROUTE_GET:
					{
						int msg_len = IPC_MSG_LEN / sizeof(struct rip_redistribute_route_info);
		                struct rip_redistribute_route_info rip_redis_route_buff[msg_len];
		                memset(rip_redis_route_buff, 0, msg_len*sizeof(struct rip_redistribute_route_info));
		                ret = rip_redis_route_get_bulk(pdata, pmsghdr->data_len, rip_redis_route_buff);

		                if (ret > 0)
		                {
		                     /*ipc_send_reply_bulk(rip_redis_route_buff, ret * sizeof(struct rip_redistribute_route_info), ret, pmsghdr->sender_id,
		                                              MODULE_ID_RIP, IPC_TYPE_RIP, pmsghdr->msg_subtype, pmsghdr->msg_index);*/
							 ipc_ack_to_snmp(pmsghdr, rip_redis_route_buff, ret * sizeof(struct rip_redistribute_route_info), ret);
		                }
		                else
		                {
		                     /*ipc_send_noack(ERRNO_NOT_FOUND, pmsghdr->sender_id, MODULE_ID_RIP, IPC_TYPE_RIP,
		                                         pmsghdr->msg_subtype, pmsghdr->msg_index);*/
							 ipc_noack_to_snmp(pmsghdr);
		                }
					}
					break;
				case RIP_SNMP_ROUTE_SUMMARY_GET:
					{
						int msg_len = IPC_MSG_LEN / sizeof(struct rip_route_summary_info);
		                struct rip_route_summary_info rip_route_sum_buff[msg_len];
		                memset(rip_route_sum_buff, 0, msg_len*sizeof(struct rip_route_summary_info));
		                ret = rip_route_sum_get_bulk(pdata, pmsghdr->data_len, rip_route_sum_buff);

		                if (ret > 0)
		                {
		                     /*ipc_send_reply_bulk(rip_route_sum_buff, ret * sizeof(struct rip_route_summary_info), ret, pmsghdr->sender_id,
		                                              MODULE_ID_RIP, IPC_TYPE_RIP, pmsghdr->msg_subtype, pmsghdr->msg_index);*/
							 ipc_ack_to_snmp(pmsghdr, rip_route_sum_buff, ret * sizeof(struct rip_route_summary_info), ret);
		                }
		                else
		                {
		                     /*ipc_send_noack(ERRNO_NOT_FOUND, pmsghdr->sender_id, MODULE_ID_RIP, IPC_TYPE_RIP,
		                                         pmsghdr->msg_subtype, pmsghdr->msg_index);*/
							 ipc_noack_to_snmp(pmsghdr);
		                }
					}
					break;
				case RIP_SNMP_IF_NBR_GET:
					{
						int msg_len = IPC_MSG_LEN / sizeof(struct rip_if_nbr_info);
		                struct rip_if_nbr_info rip_if_nbr_buff[msg_len];
		                memset(rip_if_nbr_buff, 0, msg_len*sizeof(struct rip_if_nbr_info));
		                ret = rip_if_nbr_get_bulk(pdata, pmsghdr->data_len, rip_if_nbr_buff);

		                if (ret > 0)
		                {
		                     /*ipc_send_reply_bulk(rip_if_nbr_buff, ret * sizeof(struct rip_if_nbr_info), ret, pmsghdr->sender_id,
		                                              MODULE_ID_RIP, IPC_TYPE_RIP, pmsghdr->msg_subtype, pmsghdr->msg_index);*/
							 ipc_ack_to_snmp(pmsghdr, rip_if_nbr_buff, ret * sizeof(struct rip_if_nbr_info), ret);
		                }
		                else
		                {
		                     /*ipc_send_noack(ERRNO_NOT_FOUND, pmsghdr->sender_id, MODULE_ID_RIP, IPC_TYPE_RIP,
		                                         pmsghdr->msg_subtype, pmsghdr->msg_index);*/
							 ipc_noack_to_snmp(pmsghdr);
		                }
					}
					break;
				default:
					break;
        	}
        }
    }

}

#if 0
/* redistribute information handle */
void rip_route_manage(struct ipc_mesg *mesg)
{
    struct ipc_msghdr *pmsghdr = NULL;

    /* process the ipc message */
    pmsghdr = &(mesg->msghdr);

    //if (IS_RIP_DEBUG_EVENT || IS_RIPNG_DEBUG_EVENT)
        zlog_debug((RIP_DBG_EVENT|RIPNG_DBG_EVENT), "rip route event receive, event type %d, opcode %d.",
                   pmsghdr->msg_type, pmsghdr->opcode);

    if (pmsghdr->msg_type == IPC_TYPE_ROUTE)
    {
        rip_route_read(pmsghdr, mesg->msg_data);
    }
}
#endif
/* redistribute information handle */
void rip_route_manage_new(struct ipc_mesg_n *mesg)
{
    struct ipc_msghdr_n *pmsghdr = NULL;

    /* process the ipc message */
    pmsghdr = &(mesg->msghdr);

    //if (IS_RIP_DEBUG_EVENT || IS_RIPNG_DEBUG_EVENT)
        zlog_debug((RIP_DBG_EVENT|RIPNG_DBG_EVENT), "rip route event receive, event type %d, opcode %d.",
                   pmsghdr->msg_type, pmsghdr->opcode);

    if (pmsghdr->msg_type == IPC_TYPE_ROUTE)
    {
        rip_route_read(pmsghdr, mesg->msg_data);
    }
}

#if 0
int
add_rip_fifo_ifm(struct ipc_mesg *mesg)
{
    struct ifmeventfifo *newmesg;

	do
	{
    	newmesg = XCALLOC(MTYPE_IFMEVENT_FIFO, sizeof(struct ifmeventfifo));
		if (newmesg == NULL)
		{
			zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
		}
	}while(newmesg == NULL);

	memset(newmesg, 0, sizeof(struct ifmeventfifo));

    if (newmesg == NULL)
    {
        zlog_err("%-15s[%d]: memory xcalloc error!", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(newmesg, 0, sizeof(struct ifmeventfifo));
//  newmesg->fifo.next = NULL;
    memcpy(&newmesg->mesg, mesg, sizeof(struct ipc_mesg));
    FIFO_ADD(&(rip_m.ifmfifo), &(newmesg->fifo));
    return 0;
}

void
read_mesg_form_ifmfifo(void)
{
    struct ifmeventfifo *mesg;
    int item = 0;
    int maxcount = 10;

    while (!FIFO_EMPTY(&rip_m.ifmfifo) && (item++ < maxcount))
    {
        mesg = (struct ifmeventfifo *)FIFO_TOP(&rip_m.ifmfifo);
        rip_ifm_manage(&(mesg->mesg));
        FIFO_DEL(mesg);
        XFREE(MTYPE_IFMEVENT_FIFO, mesg);
		mesg = NULL;
    }

}

/* recieve interface msg from ifm and route */
int
rip_ifm_msg_rcv(struct thread *thread)
{
    struct ipc_mesg mesg;
    int ret = 0;

    while (ret != -1)
    {
        memset(&mesg, 0, sizeof(struct ipc_mesg));
        ret = ipc_recv_common(&mesg, MODULE_ID_RIP);

        if (ret != -1)
        {
            add_rip_fifo_ifm(&mesg);
        }
    }

    if (!FIFO_EMPTY(&rip_m.ifmfifo))
    {
        read_mesg_form_ifmfifo();
    }

	do{
    	rip_m.ifm_recv = thread_add_event(master_rip, rip_ifm_msg_rcv, NULL, 0);  /* thread to receive packet */
		if(!rip_m.ifm_recv)
			zlog_err("%-15s[%d]: can`t add reak ifm event", __func__, __LINE__);
	}
	while(!rip_m.ifm_recv);
    return 0;
}

/* recieve route msg from route */
int
rip_route_msg_rcv(struct thread *thread)
{
    struct ipc_mesg mesg;
    int ret;

    for (int i = 200; i > 0; i--)
    {
        memset(&mesg, 0, sizeof(struct ipc_mesg));
        ret = ipc_recv_route(&mesg, MODULE_ID_RIP);

        if (ret < 0)
        {
            break;
        }

        rip_route_manage(&mesg);
    }

    usleep(1000);
	do{
    	rip_m.route_recv = thread_add_event(master_rip, rip_route_msg_rcv, NULL, 0);  /* thread to receive packet */
		if(!rip_m.route_recv)
			zlog_err("%-15s[%d]: can`t add reak route event", __func__, __LINE__);
	}
	while(!rip_m.route_recv);
    return 0;
}

#endif

int rip_pkt_msg_rcv(struct ipc_mesg_n *pmsg, int imlen)
{
	int msg_block_len = 0;
	
	if ( NULL == pmsg || 0 == imlen )
	{
		zlog_debug(RIP_DBG_PACKET, "fun:%s line:%d-->ipmc recv msg: pmsg is NULL.\r\n",\
								__FUNCTION__, __LINE__);
		return ERRNO_FAIL;
	}
	
	zlog_debug(RIP_DBG_PACKET, "fun:%s line:%d-->Rip recv msg: pmsg=%p, imlen=%d\r\n", \
								__FUNCTION__, __LINE__, pmsg, imlen);

	msg_block_len = (int)pmsg->msghdr.data_len + IPC_HEADER_LEN_N;
	if(msg_block_len <= imlen)
	{
		switch ( pmsg->msghdr.msg_type )
		{
			case IPC_TYPE_PACKET:
				rip_pkt_rcv_new(pmsg);
				break;

			case IPC_TYPE_IFM:
			case IPC_TYPE_RIP:
				rip_ifm_manage_new(pmsg);
				break;

			case IPC_TYPE_ROUTE:
				rip_route_manage_new(pmsg);
				break;
			
			default:
				zlog_debug(RIP_DBG_PACKET, "fun:%s line:%d-->rip_pkt_msg_rcv, receive unk message\r\n", \
										 		__FUNCTION__, __LINE__);
				break;
		}
	}
	else
	{
		zlog_debug(RIP_DBG_PACKET, "fun:%s line:%d-->RIP recv msg: datalen error, data_len=%d, msgrcv len = %d\n", \
												 __FUNCTION__, __LINE__, msg_block_len, imlen);
	}

	if(pmsg)
	{
		mem_share_free(pmsg, MODULE_ID_RIP);
	}
	
	return ERRNO_SUCCESS;
}

void
ripc_init(void)
{

    ifm_event_register(IFNET_EVENT_IF_DELETE, MODULE_ID_RIP, IFNET_IFTYPE_L3IF);
    ifm_event_register(IFNET_EVENT_DOWN, MODULE_ID_RIP, IFNET_IFTYPE_L3IF); /* 注册接口 down 事件*/
    ifm_event_register(IFNET_EVENT_UP, MODULE_ID_RIP, IFNET_IFTYPE_L3IF); /* 注册接口 up 事件*/
    ifm_event_register(IFNET_EVENT_IP_ADD, MODULE_ID_RIP, IFNET_IFTYPE_L3IF);
    ifm_event_register(IFNET_EVENT_IP_DELETE, MODULE_ID_RIP, IFNET_IFTYPE_L3IF);
    ifm_event_register(IFNET_EVENT_MODE_CHANGE, MODULE_ID_RIP, IFNET_IFTYPE_L3IF);

}

