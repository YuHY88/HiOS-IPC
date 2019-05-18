#include "stream.h"
#include "linklist.h"
#include "log.h"
#include "thread.h"
#include "if.h"
#include "memory.h"
#include "ifm_common.h"
#include "command.h"
#include "vrf.h"
#include "errcode.h"
#include <lib/pkt_buffer.h>

#include "isisd/dict.h"
#include "isisd/include-netbsd/iso.h"
#include "isisd/isis_constants.h"
#include "isisd/isis_common.h"
#include "isisd/isis_flags.h"
#include "isisd/isis_circuit.h"
#include "isisd/isis_csm.h"
#include "isisd/isisd.h"
#include "isisd/isis_dynhn.h"
#include "isisd/isis_adjacency.h"
#include "isisd/isis_pdu.h"
#include "isisd/isis_misc.h"
#include "isisd/isis_constants.h"
#include "isisd/isis_tlv.h"
#include "isisd/isis_lsp.h"
#include "isisd/isis_spf.h"
#include "isisd/isis_route.h"
#include "isisd/isis_zebra.h"
#include "isisd/isis_events.h"
#include "isisd/isis_pdu.h"
#include "isisd/isis_ipc.h"
#include "isisd/isis_msg.h"
#include "isisd/isis_ipc.h"
#include "route_com.h"
#include "mpls_common.h"
#include "lib/snmp_common.h"

extern struct isis_statistics isis_sta;
extern struct thread_master *master;
//struct isis *isis = NULL;
/*
int isis_slot_sendmsg(struct ipc_mesg_n *pMsg, int iLen)
{
    int iresult =0;

    iresult = ipc_send_msg_n1(pMsg,  iLen);

    if(iresult != 0) mem_share_free(pMsg, MODULE_ID_ISIS);

    return(iresult);
}

void isis_sync_fillhead(struct ipc_msghdr_n  *pMsgHdr , enum PKT_TYPE type)
{
     pMsgHdr->module_id   = MODULE_ID_FTM;
     pMsgHdr->sender_id   = MODULE_ID_ISIS;
     pMsgHdr->msg_type    = IPC_TYPE_PACKET;
     pMsgHdr->msg_subtype = type;
     pMsgHdr->opcode      = 0;
     pMsgHdr->data_num    = 0;
     pMsgHdr->priority    = 0;
     pMsgHdr->unit        = 0;
     pMsgHdr->slot        = 0;
     pMsgHdr->srcunit     = 0;
     pMsgHdr->srcslot     = 0;
     pMsgHdr->sequence    = 0;
     pMsgHdr->result      = 0;
}

int isis_sync_statussend(enum PKT_TYPE type,struct stream * data,int stream_length)
{
     struct ipc_mesg_n *pSndMsg =  NULL;
	 int len = 0;
	 int ret;

     pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + stream_length, MODULE_ID_ISIS);

     if(pSndMsg != NULL)
     {

         isis_sync_fillhead(&pSndMsg->msghdr,type);

		 len = sizeof(struct ipc_msghdr_n);

		 memcpy(pSndMsg->msg_data + len, STREAM_DATA(data) , stream_length);

		 len += stream_length;

         pSndMsg->msghdr.data_len = len;

         ret = isis_slot_sendmsg(pSndMsg, len);

		 if(ret != 0)
		 {
			zlog_err("isis shared memory pkt send error !\n");
			return -1;
		 }
     }
     return 0;
}

int
isis_send_message_route(struct route_entry route,enum IPC_OPCODE opcode)
{
	int ret = ipc_send_route(&route, sizeof(struct route_entry), 1, MODULE_ID_ROUTE, MODULE_ID_ISIS,
							IPC_TYPE_ROUTE, 0, opcode);

	if(opcode == IPC_OPCODE_ADD)
		isis_sta.isis_route++;
	else if(opcode == IPC_OPCODE_DELETE)
		isis_sta.isis_route--;

	return ret;
}
*/
int
isis_send_multiplemessage_route(struct route_entry *route,enum IPC_OPCODE opcode, unsigned int item)
{
    //int ret = ipc_send_route(route, sizeof(struct route_entry)*item, item, MODULE_ID_ROUTE, MODULE_ID_ISIS,
    //					IPC_TYPE_ROUTE, 0, opcode);

    int ret = ipc_send_msg_n2(route, sizeof(struct route_entry)*item, item,MODULE_ID_ROUTE,MODULE_ID_ISIS,
                              IPC_TYPE_ROUTE, 0, opcode, 0);

    if(opcode == IPC_OPCODE_ADD)
        isis_sta.isis_route++;
    else if(opcode == IPC_OPCODE_DELETE)
        isis_sta.isis_route--;

    return ret;
}
/*
int
isis_send_multiplemessage_route_n(struct route_entry *route,enum IPC_OPCODE opcode, unsigned int item)
{
     struct ipc_mesg_n *pSndMsg =  NULL;
	 int len = 0;
	 int ret = 0;
	 int data_len = sizeof(struct route_entry) * item;


     pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + data_len, MODULE_ID_ISIS);

     if(pSndMsg != NULL)
     {

	     pSndMsg->msghdr.module_id   = MODULE_ID_ROUTE;
	     pSndMsg->msghdr.sender_id   = MODULE_ID_ISIS;
	     pSndMsg->msghdr.msg_type    = IPC_TYPE_ROUTE;
	     pSndMsg->msghdr.msg_subtype = 0;
	     pSndMsg->msghdr.opcode      = opcode;
	     pSndMsg->msghdr.data_num    = item;
	     pSndMsg->msghdr.priority    = 0;
	     pSndMsg->msghdr.unit        = 0;
	     pSndMsg->msghdr.slot        = 0;
	     pSndMsg->msghdr.srcunit     = 0;
	     pSndMsg->msghdr.srcslot     = 0;
	     pSndMsg->msghdr.sequence    = 0;
	     pSndMsg->msghdr.result      = 0;

		 len = sizeof(struct ipc_msghdr_n);

		 memcpy(pSndMsg->msg_data + len, route , data_len);

		 len += data_len;

         pSndMsg->msghdr.data_len = len;

         ret = isis_slot_sendmsg(pSndMsg, len);

		 if(ret != 0)
		 {
			zlog_err("isis shared memory pkt send to route error !\n");
			return -1;
		 }
     }
     return 0;
}

int
isis_reply_to_snmp(void *data, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, uint32_t msg_index,enum IPC_OPCODE opcode)
{
	struct ipc_mesg_n *pSndMsg =  NULL;
	int len = 0;
	int ret = 0;
	u_int32_t data_len = data_num * sizeof(struct isis_instance_entry);

	pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + data_len, MODULE_ID_ISIS);

	if(pSndMsg != NULL)
	{

		pSndMsg->msghdr.module_id	 = module_id;
		pSndMsg->msghdr.sender_id	 = MODULE_ID_ISIS;
		pSndMsg->msghdr.msg_type	 = msg_type;
		pSndMsg->msghdr.msg_subtype = subtype;
		pSndMsg->msghdr.opcode 	 = opcode;
		pSndMsg->msghdr.data_num	 = data_num;
		pSndMsg->msghdr.priority	 = 0;
		pSndMsg->msghdr.unit		 = 0;
		pSndMsg->msghdr.slot		 = 0;
		pSndMsg->msghdr.srcunit	 = 0;
		pSndMsg->msghdr.srcslot	 = 0;
		pSndMsg->msghdr.sequence	 = 0;
		pSndMsg->msghdr.result 	 = 0;
		pSndMsg->msghdr.msg_index   = msg_index;


		len = sizeof(struct ipc_msghdr_n);

		memcpy(pSndMsg->msg_data + len, data , data_len);

		len += data_len;

		pSndMsg->msghdr.data_len = len;

		ret = isis_slot_sendmsg(pSndMsg, len);

		if(ret != 0)
		{
		   zlog_err("isis shared memory pkt send error !\n");
		   return -1;
		}
	}
	return 0;
}

int
ipc_noack_to_snmp(uint32_t errcode, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, uint32_t msg_index,enum IPC_OPCODE opcode)
{
	struct ipc_mesg_n *pSndMsg =  NULL;
	int len = 0;
	int ret = 0;
	u_int32_t data_len = data_num * sizeof(uint32_t);

	pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + data_len, MODULE_ID_ISIS);

	if(pSndMsg != NULL)
	{

		pSndMsg->msghdr.module_id	 = module_id;
		pSndMsg->msghdr.sender_id	 = MODULE_ID_ISIS;
		pSndMsg->msghdr.msg_type	 = msg_type;
		pSndMsg->msghdr.msg_subtype = subtype;
		pSndMsg->msghdr.opcode 	 = opcode;
		pSndMsg->msghdr.data_num	 = data_num;
		pSndMsg->msghdr.priority	 = 0;
		pSndMsg->msghdr.unit		 = 0;
		pSndMsg->msghdr.slot		 = 0;
		pSndMsg->msghdr.srcunit	 = 0;
		pSndMsg->msghdr.srcslot	 = 0;
		pSndMsg->msghdr.sequence	 = 0;
		pSndMsg->msghdr.result 	 = 0;
		pSndMsg->msghdr.msg_index   = msg_index;


		len = sizeof(struct ipc_msghdr_n);

		memcpy(pSndMsg->msg_data + len, &errcode, data_len);

		len += data_len;

		pSndMsg->msghdr.data_len = len;

		ret = isis_slot_sendmsg(pSndMsg, len);

		if(ret != 0)
		{
		   zlog_err("isis shared memory pkt send error !\n");
		   return -1;
		}
	}
	return 0;
}

*/

int isis_asyc_send_route_cmd (void * argv)
{
    int msg_num = IPC_ISIS_LEN/sizeof(struct route_entry);
    struct routefifo *routefifo_entry[msg_num];
    struct fifo *pfifo = NULL;
    unsigned int opcodeflag = 0;
    struct route_entry route[msg_num];
    int item = 0;	/*  �?条IPC msg 承载路由条目个数*/
    int send_or_count = 0;
    int ret = 0;
    int i;

    for (item = 0; item < msg_num; item++)
        routefifo_entry[item] = NULL;

    while (!FIFO_EMPTY(&isis_m.routefifo))
    {
        if (send_or_count >= 2000)
        {
            break;
        }

        pfifo = &isis_m.routefifo;

        routefifo_entry[0] = (struct routefifo *)FIFO_TOP(pfifo);
        opcodeflag = routefifo_entry[0]->opcode;
        pfifo = &(routefifo_entry[0]->fifo);
        memcpy(&route[0], &routefifo_entry[0]->route, sizeof(struct route_entry));
        item = 1;

        while ((pfifo->next != (struct fifo *)&isis_m.routefifo) && item < msg_num)
        {
            routefifo_entry[item] = (struct routefifo *)FIFO_TOP(pfifo);
            if (opcodeflag != routefifo_entry[item]->opcode)
                break;
            memcpy(&route[item], &routefifo_entry[item]->route, sizeof(struct route_entry));
            pfifo = &(routefifo_entry[item]->fifo);
            item++;
        }

        ret = isis_send_multiplemessage_route(route, opcodeflag, item);
        if (ret < 0)
        {
            break;
        }
        else
        {
            send_or_count = send_or_count + item;
        }

        for (i = 0; i < item; i++)
        {
            FIFO_DEL(routefifo_entry[i]);
            XFREE (MTYPE_ROUTE_FIFO, routefifo_entry[i]);
        }
    }
	
	isis_m.t_thread_routefifo = 0 ;
	THREAD_TIMER_ON_NEW((char *)"ISISSendRouteTimer",isis_m.t_thread_routefifo,isis_asyc_send_route_cmd,NULL,1);
	
    return 0;
}


static int
isis_interface_get_bulk(uint32_t ifindex, int data_len, struct isis_circuit_entry isis_cir_buff[])
{
    struct listnode *node, *nnode;
    struct isis_circuit *circuit;
    struct isis_area *area;
    struct isis_circuit_entry isiscir;
    int data_num = 0;
    int msg_len = IPC_MSG_LEN / sizeof(struct isis_circuit_entry);
    int flag = 0;

    memset(isis_cir_buff, 0, msg_len*sizeof(struct isis_circuit_entry));

    for (ALL_LIST_ELEMENTS_RO (isis->area_list, node, area))
    {
        for(ALL_LIST_ELEMENTS_RO (area->circuit_list, nnode, circuit))
        {
            if(ifindex && circuit->interface->ifindex == ifindex)
            {
                flag = 1;
                continue;
            }

            if((1 == flag) || (0 == ifindex))
            {
                isiscir.ifindex = circuit->interface->ifindex;
                isiscir.instance = atoi(area->area_tag);
                memcpy(isiscir.name, circuit->interface->name, INTERFACE_NAMSIZ + 1);
                isiscir.passive = circuit->is_passive;
                isiscir.circuit_level = circuit->is_type;
                isiscir.circuit_type = circuit->circ_type;
                isiscir.password_type = circuit->passwd.type;
                memset(isiscir.passwd,0,255*sizeof(u_char));
                memcpy(isiscir.passwd, circuit->passwd.passwd, circuit->passwd.len);
                isiscir.priority = circuit->priority[0];
                if(area->oldmetric)
                    isiscir.metric = circuit->metrics[0].metric_default;
                else
                    isiscir.metric = circuit->te_metric[0];
                /* 
                    no support for ipv6 cost now, Will add soon!
                */    
                isiscir.hello_interval = circuit->hello_interval[0];
                isiscir.hello_multiplier = circuit->hello_multiplier[0];
                isiscir.csnp_interval = circuit->csnp_interval[0];
                isiscir.psnp_interval = circuit->psnp_interval[0];

                memcpy(&isis_cir_buff[data_num++], &isiscir, sizeof(struct isis_circuit_entry));

                if(data_num == msg_len)
                {
                    return data_num;
                }
            }
        }
    }
    return data_num;
}

static int
isis_instance_get_bulk(uint32_t instance, int data_len, struct isis_instance_entry isisins_buff[])
{
    uint32_t instance_tmp;
    struct isis_area *area;
    struct isis_instance_entry isis_instance;
    struct listnode *node;
    int data_num = 0;
    int msg_num  = IPC_MSG_LEN / sizeof(struct isis_instance_entry);
    int flag = 0;

    if(instance >= 255)
    {
        return 0;
    }

    for(ALL_LIST_ELEMENTS_RO(isis->area_list, node, area))
    {
        instance_tmp = (uint32_t)atoi(area->area_tag);

        if((int)instance && instance_tmp == instance)
        {
            flag = 1;
            continue;
        }
        if((1 == flag) || (0 == instance))
        {
            isis_instance.instance = atoi(area->area_tag);
            isis_instance.level = area->is_type;
            isis_instance.lsp_gen_interval = area->lsp_gen_interval[0];
            isis_instance.lsp_mtu = area->lsp_mtu;
            isis_instance.lsp_refresh = area->lsp_refresh[0];
            isis_instance.max_lsp_lifetime = area->max_lsp_lifetime[0];
            isis_instance.spf_interval = area->min_spf_interval[0];
            isis_instance.overloadbit = area->overload_bit;
            isis_instance.attachedbit = 1;
            isis_instance.hellopadding = 1;
			isis_instance.distance = area->distance4;
			isis_instance.level1_to_level2 = area->area_import_to_l2;
			isis_instance.level2_to_level1 = area->area_import_to_l1;
			
            if(area->oldmetric == 1 && area->newmetric == 0)
            {
                isis_instance.metric_type = 1;
            }
            else if (area->oldmetric == 1 && area->newmetric == 1)
            {
                isis_instance.metric_type = 2;
            }
            else
            {
                isis_instance.metric_type=3;
            }
            memcpy(&isisins_buff[data_num++], &isis_instance, sizeof(struct isis_instance_entry));
            if(data_num == msg_num)
            {
                return data_num;
            }
        }
    }
    return data_num;
}

static int
isis_netentity_get_bulk(struct isis_netentity_entry isis_net, int data_len, struct isis_netentity_entry isisnet_buff[])
{
    struct isis_area *area;
    struct isis_netentity_entry isis_net_tmp;
    struct listnode *node, *nnode;
    struct area_addr *area_addr;
    int data_num = 0;
    int msg_num  = IPC_MSG_LEN / sizeof(struct isis_netentity_entry);
    int flag = 0;

    if(isis_net.instance >= 255)
    {
        return 0;
    }

    for(ALL_LIST_ELEMENTS_RO(isis->area_list, node, area))
    {
        for (ALL_LIST_ELEMENTS_RO (area->area_addrs, nnode, area_addr))
        {
            if(isis_net.instance &&
                    (atoi(area->area_tag) == (int)isis_net.instance &&
                     !memcmp(isis_net.netentity, area_addr->area_addr, area_addr->addr_len)))
            {
                flag = 1;
                continue;
            }

            if((1 == flag) || (0 == isis_net.instance))
            {
                isis_net_tmp.instance = atoi(area->area_tag);
                memset(isis_net_tmp.netentity,0,sizeof(isis_net_tmp.netentity));
                memcpy(isis_net_tmp.netentity, area_addr->area_addr,area_addr->addr_len+7);
                isis_net_tmp.netentity_len = area_addr->addr_len+7;

                memcpy(&isisnet_buff[data_num++], &isis_net_tmp, sizeof(struct isis_netentity_entry));
                if(data_num == msg_num)
                {
                    return data_num;
                }
            }
        }
    }
    return data_num;
}

/*regist to l3vpn for vpn events*/
int isis_l3vpn_event_register(void)
{
    return mpls_com_l3vpn_event_register(MODULE_ID_ISIS);
}

/*	In this founction,dispose the events from l3vpn,
  	which kind message is no important ,
  	beacuse whatever I'm recevied,
  	I should always disable the instance directly.
*/
int isis_l3vpn_event_dispose(struct ifm_event *pevent, vrf_id_t vrf_id)
{
    struct interface *ifp;
    char ifname_tmp[INTERFACE_NAMSIZ];
    struct isis_circuit *circuit;

    /* Get interface name. */
    ifm_get_name_by_ifindex(pevent->ifindex, ifname_tmp);

    /* Lookup this by interface index. */
    ifp = if_lookup_by_name_len_vrf (ifname_tmp,strnlen (ifname_tmp, INTERFACE_NAMSIZ),vrf_id);

    if (ifp == NULL)
        return CMD_WARNING;

    circuit = circuit_scan_by_ifp(ifp);

    if(circuit == NULL)
    {
        return CMD_WARNING;
    }

    if(pevent->mode == IFNET_MODE_L3)
    {
        /*get the vpn value from l3vpn,save the value to ifp*/
        ifp->vpn = pevent->vpn;
        isis_csm_state_change (ISIS_DISABLE, circuit, circuit->area);
    }
    return 0;

}

void isis_vpn_manage(void *pdata, struct ipc_msghdr_n *pmsghdr)
{
    struct l3vpn_entry *l3vpn_info  = NULL;
    struct listnode * node = NULL;
    struct listnode * nnode = NULL;
    struct isis_area * area = NULL;
    l3vpn_info = (struct l3vpn_entry *)pdata;

    if(l3vpn_info == NULL || l3vpn_info->l3vpn.vrf_id > 128)
        return;

    switch (pmsghdr->opcode)
    {
    case IPC_OPCODE_DELETE:
    {
        for(ALL_LIST_ELEMENTS(isis->area_list,node,nnode,area))
        {
            if(area->vpn == l3vpn_info->l3vpn.vrf_id)
            {
                isis_area_destroy_without_vty(area->area_tag);
            }
        }
        break;
    }

    default:
        break;
    }
    return ;
}


/* 接收 IPC 消息 */
int isis_ifm_manage(struct ipc_mesg_n *mesg)
{
    struct ipc_msghdr_n *phdr = NULL;
    void *pdata = NULL;
    struct ifm_event *pevent = NULL;
    uint32_t data;
    vrf_id_t vrf_id = VRF_DEFAULT;
    int ret=0;

    phdr = &(mesg->msghdr);
    pdata = mesg->msg_data;

    if(phdr->msg_type == IPC_TYPE_IFM)
    {
        if(phdr->opcode == IPC_OPCODE_EVENT)
        {
            pevent = (struct ifm_event *)pdata;

            switch(phdr->msg_subtype)
            {
            case IFNET_EVENT_DOWN:
                isis_zebra_if_state_down(pevent,vrf_id);
                break;
            case IFNET_EVENT_UP:
                isis_zebra_if_state_up(pevent,vrf_id);
                break;
            case IFNET_EVENT_IP_ADD:
                isis_zebra_if_address_add(pevent,vrf_id);
                break;
            case IFNET_EVENT_IP_DELETE:
                isis_zebra_if_address_del(pevent,vrf_id);
                break;
            case IFNET_EVENT_IF_DELETE:
                isis_zebra_if_del(pevent,vrf_id);
                break;
            case IFNET_EVENT_MODE_CHANGE:
                isis_interface_change_mode(pevent, vrf_id);
                break;

            case IFNET_EVENT_MTU_CHANGE:
                isis_interface_mtu_change (pevent, vrf_id);
                break;

            case IFNET_EVENT_L3VPN:
                isis_l3vpn_event_dispose(pevent, vrf_id);
                break;

            default:
                break;
            }
        }
    }
    else if(phdr->msg_type == IPC_TYPE_ISIS)
    {
        if (phdr->opcode == IPC_OPCODE_GET_BULK)
        {
            if (0 == phdr->msg_subtype)
            {
                int msg_num  = IPC_MSG_LEN / sizeof(struct isis_instance_entry);
                struct isis_instance_entry isisins_buff[msg_num];
                data =*((uint32_t*)pdata);
                memset(isisins_buff, 0, msg_num * sizeof(struct isis_instance_entry));
                ret = isis_instance_get_bulk(data, phdr->data_len, isisins_buff);
				
                if (ret > 0)
                {
                    ipc_send_reply_n2(isisins_buff, ret * sizeof(struct isis_instance_entry), ret, 
                    							MODULE_ID_SNMPD, MODULE_ID_ISIS,
                                      IPC_TYPE_ISIS, phdr->msg_subtype, phdr->sequence,	phdr->msg_index, IPC_OPCODE_REPLY);
                }
                else
                {
                    ipc_noack_to_snmp(phdr);
                }
            }
            else if(1 == phdr->msg_subtype)
            {
                int msg_len = IPC_MSG_LEN / sizeof(struct isis_circuit_entry);
                struct isis_circuit_entry isiscir_buff[msg_len];
                data =*((uint32_t*)pdata);
                memset(isiscir_buff, 0, msg_len * sizeof(struct isis_circuit_entry));
                ret = isis_interface_get_bulk(data, phdr->data_len, isiscir_buff);
				
                if (ret > 0)
                {
                    ipc_send_reply_n2(isiscir_buff, ret * sizeof(struct isis_circuit_entry), ret,
                    						MODULE_ID_SNMPD, MODULE_ID_ISIS,
                                      IPC_TYPE_ISIS, phdr->msg_subtype, phdr->sequence,	phdr->msg_index, IPC_OPCODE_REPLY);
                }
                else
                {
                    ipc_noack_to_snmp(phdr);
                }
            }
            else if(2 == phdr->msg_subtype)
            {
                int msg_len = IPC_MSG_LEN / sizeof(struct isis_netentity_entry);
				
                struct isis_netentity_entry isisnet_buff[msg_len];
                struct isis_netentity_entry isis_net = *((struct isis_netentity_entry *)pdata);
                memset(isisnet_buff, 0, msg_len * sizeof(struct isis_netentity_entry));
                ret = isis_netentity_get_bulk(isis_net, phdr->data_len, isisnet_buff);
                if (ret > 0)
                {
                    ipc_send_reply_n2(isisnet_buff, ret * sizeof(struct isis_netentity_entry), ret, 
                    						MODULE_ID_SNMPD , MODULE_ID_ISIS,
                                      IPC_TYPE_ISIS, phdr->msg_subtype, phdr->sequence, phdr->msg_index, IPC_OPCODE_REPLY);
                }
                else
                {
                    ipc_noack_to_snmp(phdr);
                }
            }
        }
    }

    else if(phdr->msg_type == IPC_TYPE_L3VPN)
    {
        isis_vpn_manage(pdata,phdr);
    }
    return ISIS_OK;
}

void
isis_route_read_ipv4 (struct route_entry *proute,u_int8_t code)
{
    struct isis_ipv4 api;
    struct prefix p_generic;
    struct in_addr nexthop;
    int type;

    memset(&nexthop, 0, sizeof(nexthop));

    memset(&api, 0, sizeof(api));

#if 1
    if(proute->nhp[0].protocol == ROUTE_PROTO_CONNECT && proute->prefix.prefixlen == IPV4_MAX_BITLEN)
    {
        type = IFM_TYPE_ID_GET((uint32_t)(proute->nhp[0].ifindex));

        if(type != IFNET_TYPE_LOOPBACK)
            return;
    }
#endif

    if (IPV4_NET127(proute->prefix.addr.ipv4))
        return;
    api.type = proute->nhp[0].protocol;
    api.distance = proute->nhp[0].distance;
    api.metric = proute->nhp[0].cost;
    api.instance = proute->nhp[0].instance;
    api.vpn = proute->vpn;

    p_generic.family = AF_INET;
    p_generic.prefixlen = proute->prefix.prefixlen;
    p_generic.u.prefix4.s_addr = htonl(proute->prefix.addr.ipv4);

    if(proute->nhp[0].action != NHP_ACTION_DROP)
    {
        if(proute->nhp[0].nexthop.addr.ipv4 != 0)
        {
            api.nexthop_num = proute->nhp_num;
            nexthop.s_addr = htonl(proute->nhp[0].nexthop.addr.ipv4);
        }
    }

    if(code)
    {
        isis_redist_add(api.type, &p_generic, api.distance,
                        api.metric, api.instance, api.vpn, &nexthop);
    }
    else
    {
        isis_redist_delete(api.type, &p_generic, api.instance, api.metric,api.instance, api.vpn, &nexthop);
    }
    return;
}

void
isis_route_read_ipv6 (struct route_entry *proute,u_int8_t code)
{
    struct isis_ipv4 api;
    struct prefix p_generic;
    struct in6_addr nexthop6;
    int type;

    memset(&nexthop6, 0, sizeof(nexthop6));

    memset(&api, 0, sizeof(api));

#if 1
    if(proute->nhp[0].protocol == ROUTE_PROTO_CONNECT)
    {
        type = IFM_TYPE_ID_GET((uint32_t)(proute->nhp[0].ifindex));

        if(type != IFNET_TYPE_LOOPBACK)
            return;
    }
#endif

    api.type = proute->nhp[0].protocol;
    api.distance = proute->nhp[0].distance;
    api.metric = proute->nhp[0].cost;
    api.instance = proute->nhp[0].instance;
    api.instance = proute->vpn;

    p_generic.family = AF_INET6;
    p_generic.prefixlen = proute->prefix.prefixlen;
    IPV6_ADDR_COPY(&p_generic.u.prefix6, &proute->prefix.addr.ipv6);

    if(proute->nhp[0].action != NHP_ACTION_DROP)
    {
        if(proute->nhp[0].nexthop.addr.ipv6 != 0)
        {
            api.nexthop_num = proute->nhp_num;
            IPV6_ADDR_COPY(&nexthop6, &proute->nhp[0].nexthop.addr.ipv6);
        }
    }

    if(code)
    {
        isis_redist_add(api.type, &p_generic, api.distance, api.metric, api.instance, api.instance,&nexthop6);
    }
    else
    {
        isis_redist_delete(api.type, &p_generic, api.instance, api.metric, api.instance, api.instance,&nexthop6);
    }

    return;
}

void isis_route_manage(struct ipc_mesg_n *mesg)
{
    struct route_entry *proute = NULL;
    struct ipc_msghdr_n *phdr = NULL;
    int i;

    /* process the ipc message */
    phdr = &(mesg->msghdr);
    proute =(struct route_entry *)mesg->msg_data;

    if (phdr->msg_type == IPC_TYPE_ROUTE)
    {
        for(i = 0 ; i < phdr->data_num ; i ++)
        {
            proute = (struct route_entry *)mesg->msg_data + i;
            if(proute->prefix.type == INET_FAMILY_IPV4)
            {
                if (phdr->opcode == IPC_OPCODE_ADD)
                {
                    isis_route_read_ipv4(proute,1);
                }
                if(phdr->opcode == IPC_OPCODE_DELETE)
                {
                    isis_route_read_ipv4(proute,0);
                }
            }
            else if(proute->prefix.type == INET_FAMILY_IPV6)
            {
                if (phdr->opcode == IPC_OPCODE_ADD)
                {
                    isis_route_read_ipv6(proute,1);
                }
                if(phdr->opcode == IPC_OPCODE_DELETE)
                {
                    isis_route_read_ipv6(proute,0);
                }
            }
        }
    }
}

#if 0
int isis_ifm_msg_rcv(void)
{
    static struct ipc_mesg mesg;
    struct ifmeventfifo *ifmevent;
    struct ifmeventfifo *ifmeventfifo_entry;
    struct fifo *pfifo = NULL;
    int processifmeventcount = 0;
    int recv_event_num_per_thread = 2000;
    int ret;
    //int recv_event_num = 0;

    /*取出事件并存入FIFO*/
    do
    {
        ret = ipc_recv_common(&mesg, MODULE_ID_ISIS);
        if (ret < 0)
        {
            break;
        }

        /*可采用cache进一步优�?*/
        ifmevent = XCALLOC (MTYPE_IFMEVENT_FIFO, sizeof (struct ifmeventfifo));
        if (ifmevent == NULL)
        {
            zlog_debug(ISIS_DEBUG_TYPE_MSG,"%-15s[%d]: memory xcalloc error!",__FUNCTION__, __LINE__);
            break;
        }
        memset(ifmevent, 0, sizeof (struct ifmeventfifo));
        memcpy(&ifmevent->mesg, &mesg, sizeof(struct ipc_mesg));
        FIFO_ADD(&isis_m.ifm_event_fifo, &ifmevent->fifo);
        recv_event_num_per_thread--;
        //recv_event_num++;
    } while(ret >= 0 && recv_event_num_per_thread > 0);

    /*每thread处理10个事�?*/
    while (!FIFO_EMPTY(&isis_m.ifm_event_fifo) && processifmeventcount < 10)
    {
        pfifo = &isis_m.ifm_event_fifo;
        ifmeventfifo_entry = (struct ifmeventfifo *)FIFO_TOP(pfifo);
        isis_ifm_manage(&ifmeventfifo_entry->mesg_n);

        FIFO_DEL(ifmeventfifo_entry);
        XFREE (MTYPE_IFMEVENT_FIFO, ifmeventfifo_entry);
        processifmeventcount++;
    }

    return 0;
}


void isis_read_mesg_from_routefifo(void)
{
    struct ifmeventfifo *mesg;
    int item = 0;
    int maxcount = 100;

    while(!FIFO_EMPTY(&isis_m.routefifo_rcv) && (item++ < maxcount))
    {
        mesg = (struct ifmeventfifo *)FIFO_TOP(&isis_m.routefifo_rcv);
        isis_route_manage(&mesg->mesg_n);
        FIFO_DEL(mesg);
        XFREE (MTYPE_ROUTEEVENT_FIFO, mesg);
    }
}
#endif

#if 0 
int isis_add_fifo_route(struct ipc_mesg *mesg)
{
    struct ifmeventfifo *newmesg = NULL;

    do
    {
        newmesg = XCALLOC (MTYPE_ROUTEEVENT_FIFO, sizeof(struct ifmeventfifo));

        if (newmesg == NULL)
        {
            zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
            sleep(1);
        }
    } while(newmesg == NULL);
    if (newmesg == NULL)
    {
        zlog_err("%-15s[%d]: memory xcalloc error!",__FUNCTION__, __LINE__);
        return -1;
    }
    memset(newmesg, 0, sizeof(struct ifmeventfifo));
    memcpy(&newmesg->mesg, mesg, sizeof(struct ipc_mesg));
    FIFO_ADD(&isis_m.routefifo_rcv, &(newmesg->fifo));
    return 0;
}
#endif 

#if 0
/* recieve route msg from route */
int isis_route_msg_rcv(void)
{
    static struct ipc_mesg mesg;
    int ret = 0;

    while(ret != -1)
    {
        memset(&mesg,0,sizeof(struct ipc_mesg));
        ret = ipc_recv_route(&mesg, MODULE_ID_ISIS);
        if (ret != -1)
        {
            /*add message into fifo*/
            //		isis_add_fifo_route(&mesg);
        }
    }

    /*dispose the message*/
    if(!FIFO_EMPTY(&isis_m.routefifo_rcv))
    {
        isis_read_mesg_from_routefifo();
    }
    return ret ;

}
#endif 

#if 0
int isis_msg_rcv(struct thread *thread)
{
    isis_ifm_msg_rcv();
    isis_route_msg_rcv();

    usleep(50000);//free CPU 50ms
    thread_add_event (master, isis_msg_rcv, NULL, 0);
    return ERRNO_SUCCESS;
}
#endif

int isis_msg_rcv_n(struct ipc_mesg_n *pmesg, int imlen)
{
    int revln = 0;

    revln = (int)pmesg->msghdr.data_len + IPC_HEADER_LEN_N;

    if(revln <= imlen)
    {
        switch(pmesg->msghdr.msg_type)
        {
        case IPC_TYPE_IFM:
        case IPC_TYPE_ISIS:
        case IPC_TYPE_L3VPN:
            isis_ifm_manage(pmesg);
			break;
        case IPC_TYPE_ROUTE:
            isis_route_manage(pmesg);
			break;
        case IPC_TYPE_PACKET:
            isis_receive(pmesg,imlen);
			break;
        default:
            zlog_debug(ISIS_DEBUG_TYPE_MSG,"isis_msg_recv_n, receive unk message\r\n");
            break;
        }
    }
    else
    {
        zlog_err("isis recv msg: datalen error, data_len=%d, msgrcv len = %d\n", revln, imlen);
    }

    mem_share_free(pmesg, MODULE_ID_ISIS);
    return ERRNO_SUCCESS;
}

void isis_msg_init(void)
{
    ifm_event_register(IFNET_EVENT_IF_DELETE, MODULE_ID_ISIS, IFNET_IFTYPE_L3IF);
    ifm_event_register(IFNET_EVENT_DOWN, MODULE_ID_ISIS, IFNET_IFTYPE_L3IF);
    ifm_event_register(IFNET_EVENT_UP, MODULE_ID_ISIS, IFNET_IFTYPE_L3IF); 
    ifm_event_register(IFNET_EVENT_IP_ADD, MODULE_ID_ISIS, IFNET_IFTYPE_L3IF);
    ifm_event_register(IFNET_EVENT_IP_DELETE, MODULE_ID_ISIS, IFNET_IFTYPE_L3IF);
    ifm_event_register(IFNET_EVENT_MODE_CHANGE, MODULE_ID_ISIS, IFNET_IFTYPE_L3IF);
    ifm_event_register(IFNET_EVENT_MTU_CHANGE, MODULE_ID_ISIS, IFNET_IFTYPE_L3IF);
    ifm_event_register(IFNET_EVENT_L3VPN,MODULE_ID_ISIS,IFNET_IFTYPE_L3IF);

    //thread_add_event (master, isis_msg_rcv, NULL, 0);

    return;
}

