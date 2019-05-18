/*
    ospf register to ftm , prepare to recive message from ftm
*/
#include <lib/thread.h>
#include <lib/module_id.h>
#include <lib/pkt_type.h>
#include <lib/memory.h>
#include "lib/memtypes.h"
#include <lib/ospf_common.h>
#include <lib/ifm_common.h>
#include <lib/command.h>
#include <lib/linklist.h>
#include <lib/errcode.h>
#include <lib/pkt_buffer.h>
#include <ftm/pkt_ip.h>
#include "ospfd.h"
#include "ospf_dump.h"
#include "ospf_lsa.h"
#include "ospf_flood.h"
#include "ospf_lsdb.h"


/* register recevie packet */
void ospf_pkt_register(void)
{
    union proto_reg proto;
    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.dip = OSPF_ALLSPFROUTERS;
    //proto.ipreg.type = INET_FAMILY_IPV4;
    proto.ipreg.protocol = IP_P_OSPF;
    pkt_register(MODULE_ID_OSPF, PROTO_TYPE_IPV4, &proto);
    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.dip = OSPF_ALLDROUTERS;
    //proto.ipreg.type = INET_FAMILY_IPV4;
    proto.ipreg.protocol = IP_P_OSPF;
    pkt_register(MODULE_ID_OSPF, PROTO_TYPE_IPV4, &proto);
    memset(&proto, 0, sizeof(union proto_reg));
    //proto.ipreg.type = INET_FAMILY_IPV4;
    proto.ipreg.protocol = IP_P_OSPF;
    pkt_register(MODULE_ID_OSPF, PROTO_TYPE_IPV4, &proto);
}
void ospf_pkt_unregister(void)
{
    union proto_reg proto;
    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.dip = htonl(OSPF_ALLSPFROUTERS);
    proto.ipreg.protocol = IP_P_OSPF;
    pkt_unregister(MODULE_ID_OSPF, PROTO_TYPE_IPV4, &proto);
    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.dip = htonl(OSPF_ALLDROUTERS);
    proto.ipreg.protocol = IP_P_OSPF;
    pkt_unregister(MODULE_ID_OSPF, PROTO_TYPE_IPV4, &proto);
    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_OSPF;
    pkt_unregister(MODULE_ID_OSPF, PROTO_TYPE_IPV4, &proto);
}

static uint32_t creat_map_loopback_for_u0(struct in_addr ip_addr)
{
	int i = 0, j = 0;
	int ret = 0;
	uint32_t ifindex = 0;
	struct interface *p_loopback = NULL;
	char if_name[NAME_STRING_LEN] = "";

	p_loopback = if_lookup_address (ip_addr);
	if(p_loopback)
	{
		ifindex = p_loopback->ifindex;
		if(IFNET_TYPE_LOOPBACK == IFM_TYPE_ID_GET(ifindex))
		{
			/*printf("%s[%d]:There is already an IP on the device(ifindex == 0x%x) "
				"that is the same as new U0 \n", __FUNCTION__, __LINE__, ifindex);*/
			i = IFM_SUBPORT_ID_GET(ifindex);
			i = i - U0_LOOPBACK_INDEX_BASE;
			om->loopback_index_pool[i] = LOOPBACK_HAS_USED;
			return ifindex;
		}
		else
		{	
			OSPF_LOG_ERROR("There is already an IP on the device(ifindex == 0x%x) that is the same as new U0 and not loopback if.", ifindex);
			return 0;
		}
	}
	
	for(i = 0; i < U0_DEVICE_NUM; i++)
	{
		j = i + U0_LOOPBACK_INDEX_BASE;
		memset(if_name, 0, sizeof(if_name));
		sprintf(if_name, "%d", j);
		ifindex = ifm_get_ifindex_by_name ( "loopback", if_name);
		if ( ifindex == 0 )
		{
			OSPF_LOG_ERROR("Get %s %s ifindex fail!!  \n","loopback", if_name);
			return 0;
		}

		p_loopback = NULL;
		p_loopback = if_lookup_by_index(ifindex);
		if(p_loopback)
		{
			om->loopback_index_pool[i] = LOOPBACK_HAS_USED;
			continue;
		}
		
		if(!(om->loopback_index_pool[i]))
		{
			om->loopback_index_pool[i] = LOOPBACK_HAS_USED;
			break;
		}
	}

	i = i + U0_LOOPBACK_INDEX_BASE;
	memset(if_name, 0, sizeof(if_name));
	sprintf(if_name, "%d", i);
	ifindex = ifm_get_ifindex_by_name ( "loopback", if_name);
	if ( ifindex == 0 )
	{
		OSPF_LOG_DEBUG("Get %s %s ifindex fail!!  \n","loopback", if_name);
		return 0;
	}

	//Notify IFM to create target loopback.
	ret = ipc_send_common_wait_ack_n ( NULL, 0, 1 , MODULE_ID_IFM, MODULE_ID_OSPF,
                                      IPC_TYPE_IFM, IFNET_INFO_LOOPBACK, IPC_OPCODE_ADD, ifindex );
	if ( ret < 0 )
	{
		return 0;
	}
	return ifindex;
}

static uint32_t delete_map_loopback_for_u0(uint32_t ifindex)
{
	int i = 0;
	int ret = 0;

	if(!ifindex)
	{
		OSPF_LOG_DEBUG("The target loopback 0x%x is not exist.", ifindex);
		return -1;
	}
	
	i = IFM_SUBPORT_ID_GET(ifindex);
	i = i - U0_LOOPBACK_INDEX_BASE;
	
	if(om->loopback_index_pool[i])
	{
		om->loopback_index_pool[i] = LOOPBACK_NO_USED;
	}

	//Notify IFM to delete target loopback.
	ret = ipc_send_common_wait_ack_n ( NULL, 0, 1 , MODULE_ID_IFM, MODULE_ID_OSPF,
                                      IPC_TYPE_IFM, IFNET_INFO_LOOPBACK, IPC_OPCODE_DELETE, ifindex );
	if ( ret < 0 )
	{
		return -1;
	}
	return 0;
}

int interface_set_ip(uint32_t ifindex, struct in_addr ip_addr)
{
	int ret = 0;
	struct ifm_l3 l3if;
	
	memset(&l3if, 0, sizeof(struct ifm_l3));
	l3if.vpn = OSPF_DCN_VRF_ID;
	l3if.ipv4_flag = IP_TYPE_STATIC;
	l3if.ipv4[0].addr = ntohl(ip_addr.s_addr);
	l3if.ipv4[0].prefixlen = IPV4_MAX_BITLEN;
	
	ret = ifm_set_l3if(ifindex, &l3if, IFNET_EVENT_IP_ADD, MODULE_ID_OSPF);
    if(ret == ERRNO_EXISTED_IP)
   	{
		OSPF_LOG_DEBUG("The IP address for ospf DCN have already existed.");
        return 0;//已经存在，说明是配置恢复加载过程，当做ip设置成功来看待
    }
	else if(ret == ERRNO_CONFLICT_IP)
	{
		OSPF_LOG_DEBUG("The IP address for ospf DCN conflicts with another address !");
        return CMD_WARNING;
	}
	else if(ret == ERRNO_SUBNET_SIMILAR)
	{
		OSPF_LOG_DEBUG("The IP address for ospf DCN A similar IP subnet already exists !");
        return CMD_WARNING;
	}
	else if(ret == -1)
	{
		OSPF_LOG_DEBUG("The IP address for ospf DCN set failed!");
        return CMD_WARNING;
	}

	return 0;
}

int interface_update_ip(uint32_t ifindex, struct in_addr ip_addr, struct in_addr new_ip_addr)
{
	int ret = 0;
	struct ifm_l3 l3if;
	
	memset(&l3if, 0, sizeof(struct ifm_l3));
	l3if.vpn = OSPF_DCN_VRF_ID;
	l3if.ipv4_flag = IP_TYPE_STATIC;
	l3if.ipv4[0].addr = ntohl(ip_addr.s_addr);
	l3if.ipv4[0].prefixlen = IPV4_MAX_BITLEN;
	
	ret = ifm_set_l3if(ifindex, &l3if, IFNET_EVENT_IP_DELETE, MODULE_ID_OSPF);
	if(ret == -1)
	{
		OSPF_LOG_DEBUG("The u0 ne-ip update failed!");
		return -1;
	}

	if((ret = interface_set_ip(ifindex, new_ip_addr)) != CMD_SUCCESS)
	{
		OSPF_LOG_DEBUG("The u0 ne-ip update failed! ret = %d", ret);
		return -1;
	}
	
	return 0;
}


int u0_map_info_send_to_target_process(struct u0_device_info *new_u0, int target_module_id, enum IPC_OPCODE opcode)
{
	int ret = 0;
	//struct u0_device_info temp_u0;

	//memset(&temp_u0, 0, sizeof(struct u0_device_info));

	ret = ipc_send_msg_n2(new_u0, sizeof(struct u0_device_info), 1,
											   target_module_id, MODULE_ID_OSPF,
											   IPC_TYPE_EFM, 0, opcode, 0);
	return ret;
}
struct u0_device_info_local *u0_repeat_detect(struct u0_device_info *new_u0)
{
	struct listnode *node = NULL;
    struct u0_device_info_local *u0_local = NULL;
	
	if(NULL == new_u0)
	{
		OSPF_LOG_DEBUG("new_u0 == NULL, return!!!\n");
		return NULL;
	}
	for (ALL_LIST_ELEMENTS_RO (om->u0_list, node, u0_local))
	{
		if((u0_local->u0_info.ne_ip.s_addr == new_u0->ne_ip.s_addr)
			|| (u0_local->u0_info.ifindex == (new_u0->ifindex + 0xffe)))
		{
			return u0_local;
		}
	}
	return NULL;
}

struct u0_device_info_local *u0_repeat_detect_by_ne_ip(uint32_t ne_ip_decimal)
{
	struct listnode *node = NULL;
    struct u0_device_info_local *u0_local = NULL;
	
	if(0 == ne_ip_decimal)
	{
		OSPF_LOG_DEBUG("Invalid ne_ip, return!!!\n");
		return NULL;
	}
	//printf("%s[%d]: #####################session ip:0x%x\n", __FUNCTION__, __LINE__, ne_ip_decimal);
	for (ALL_LIST_ELEMENTS_RO (om->u0_list, node, u0_local))
	{
		//printf("%s[%d]: #####################u0 ip:0x%x\n", __FUNCTION__, __LINE__, u0_local->u0_info.ne_ip.s_addr);
		if(u0_local->u0_info.ne_ip.s_addr == ne_ip_decimal)
		{
			return u0_local;
		}
	}
	return NULL;
}

int u0_device_add_handle(struct u0_device_info *new_u0)
{

	int ret = -1;
	uint32_t ifindex = 0;
	uint16_t ospf_id = 0;
	struct in_addr area_id;
	struct ospf * ospf = NULL;
	struct ospf_area *area = NULL;
	struct u0_device_info_local *u0_local = NULL;

	ospf_id = OSPF_MAX_PROCESS_ID;

	if(NULL == new_u0)
	{
		return -1;
	}
	OSPF_LOG_DEBUG("ne-ip %s ifindex 0x%x\n", inet_ntoa(new_u0->ne_ip), new_u0->ifindex);

	if((u0_local = u0_repeat_detect(new_u0)) != NULL)
	{
		u0_map_info_send_to_target_process(new_u0, MODULE_ID_SNMPD_TRAP, IPC_OPCODE_ADD);
		/* Send to vty so that vty is used to distinguish hostname*/
		u0_map_info_send_to_target_process(new_u0, MODULE_ID_VTY, IPC_OPCODE_ADD);
		return 0;
	}
	
	u0_local = (struct u0_device_info_local*)XCALLOC(MTYPE_OSPF_TMP, sizeof(struct u0_device_info_local));
	if (NULL == u0_local)
	{
		zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
		return -1;
	}
		
	memset(u0_local, 0, sizeof(struct u0_device_info_local));
	u0_local->u0_info.ne_ip.s_addr = new_u0->ne_ip.s_addr;
	//printf("%s[%d]: u0 ne-id 0x%x ; ne-ip: 0x%x\n", __FUNCTION__, __LINE__, new_u0->ne_id.s_addr, new_u0->ne_ip.s_addr);
	u0_local->u0_info.ne_id.s_addr = (new_u0->ne_id.s_addr & htonl(0xffffff));
	memcpy(u0_local->u0_info.dcn_ne_device_type, new_u0->dcn_ne_device_type, sizeof(new_u0->dcn_ne_device_type));
	memcpy(u0_local->u0_info.dcn_ne_vendor, new_u0->dcn_ne_vendor, sizeof(new_u0->dcn_ne_vendor));
	memcpy(u0_local->u0_info.mac, new_u0->mac, sizeof(new_u0->mac));
	memcpy(u0_local->u0_info.hostname, new_u0->hostname, sizeof(new_u0->hostname));
	u0_local->u0_info.ifindex = new_u0->ifindex + 0xffe;

	if(!(ifindex = creat_map_loopback_for_u0(new_u0->ne_ip)))
	{
		OSPF_LOG_DEBUG("loopback creat fail!!!\n");
		XFREE(MTYPE_OSPF_TMP, u0_local);
		u0_local = NULL;
		return -1;
	}
	u0_local->loopback_ifindex = ifindex;

	if((ret = interface_set_ip(ifindex, new_u0->ne_ip)) != CMD_SUCCESS)
	{
		OSPF_LOG_DEBUG("loopback 0x%x ip set fail!!!\n", ifindex);
		delete_map_loopback_for_u0(ifindex);
		XFREE(MTYPE_OSPF_TMP, u0_local);
		u0_local = NULL;
		return -1;
	}
	
	listnode_add(om->u0_list, u0_local);
	om->u0_flag = U0_DEVICE_EXIST;

	u0_map_info_send_to_target_process(new_u0, MODULE_ID_SNMPD_TRAP, IPC_OPCODE_ADD);
	/* Send to vty so that vty is used to distinguish hostname*/
	u0_map_info_send_to_target_process(new_u0, MODULE_ID_VTY, IPC_OPCODE_ADD);
	
	ospf = ospf_lookup_id (ospf_id);
	if(ospf)
	{
		area_id.s_addr = 0;
		area = ospf_area_lookup_by_area_id (ospf, area_id);
		if(area)
		{
			ospf_router_lsa_update_area (area);
			ospf_router_lsa_originate_for_u0 (area, &(u0_local->u0_info));
			ospf_opaque_type10_lsa_generate_for_u0(area, &(u0_local->u0_info));
		}
		else
		{
			OSPF_LOG_DEBUG("The area %s does not exist.", inet_ntoa(area_id));
		}
	}
	else
	{
		OSPF_LOG_DEBUG("The ospf instance %d does not exist.", ospf_id);
	}
	
	return 0;
}

int u0_device_del_handle(struct u0_device_info *new_u0)
{
	
	int ret = -1;
	//uint32_t ifindex = 0;
	uint16_t ospf_id = 0;
	struct in_addr dcn_opaque_id;
	struct in_addr area_id;
	struct ospf * ospf = NULL;
	struct ospf_area *area = NULL;
	struct ospf_lsa *route_lsa = NULL;
	struct ospf_lsa *opaque_type10_lsa = NULL;
	struct u0_device_info_local *u0_local = NULL;

	ospf_id = OSPF_MAX_PROCESS_ID;
	inet_aton ("202.255.238.0", &dcn_opaque_id);

	if(NULL == new_u0)
	{
		return -1;
	}
	OSPF_LOG_DEBUG("ne-ip %s ifindex 0x%x\n", inet_ntoa(new_u0->ne_ip), new_u0->ifindex);
	
	if((u0_local = u0_repeat_detect(new_u0)) == NULL)
	{
		OSPF_LOG_DEBUG("The target u0(ne-ip:%s) is not exist.", inet_ntoa(new_u0->ne_ip));
		return -1;
	}

	if((ret = delete_map_loopback_for_u0(u0_local->loopback_ifindex)) < 0)
	{
		OSPF_LOG_DEBUG("The target loopback 0x%x delete fail.", u0_local->loopback_ifindex);
		return -1;
	}

	if(listcount (om->u0_list) > 0)
	{
		listnode_delete (om->u0_list, u0_local);
        XFREE (MTYPE_OSPF_TMP, u0_local);
		u0_local = NULL;
	}
	else
	{
		om->u0_flag = !U0_DEVICE_EXIST;
	}
	
	u0_map_info_send_to_target_process(new_u0, MODULE_ID_SNMPD_TRAP, IPC_OPCODE_DELETE);
	/* Send to vty so that vty is used to distinguish hostname*/
	u0_map_info_send_to_target_process(new_u0, MODULE_ID_VTY, IPC_OPCODE_DELETE);
	
	ospf = ospf_lookup_id (ospf_id);
	if(ospf)
	{
		area_id.s_addr = 0;
		area = ospf_area_lookup_by_area_id (ospf, area_id);
		if(area)
		{
			//clean route-lsa
			route_lsa = ospf_lsa_lookup_by_id (area, OSPF_ROUTER_LSA, new_u0->ne_ip);
			if(!route_lsa)
			{
				OSPF_LOG_DEBUG("The route-lsa id = %s is not exist. ", inet_ntoa(new_u0->ne_ip));
				return -1;
			}
			SET_FLAG (route_lsa->flags, OSPF_LSA_SELF);
			ospf_lsa_flush (ospf, route_lsa);

			//clean opaque-type10-lsa
			opaque_type10_lsa = ospf_lsdb_lookup_by_id(area->lsdb, OSPF_OPAQUE_AREA_LSA, dcn_opaque_id, new_u0->ne_ip);
			if(!opaque_type10_lsa)
			{
				OSPF_LOG_DEBUG("The type10-lsa adv = %s is not exist. ", inet_ntoa(new_u0->ne_ip));
				return -1;
			}
			SET_FLAG (opaque_type10_lsa->flags, OSPF_LSA_SELF);
			ospf_lsa_flush (ospf, opaque_type10_lsa);
			
			ospf_router_lsa_update_area (area);
		}
		else
		{
			OSPF_LOG_DEBUG("The area %s does not exist.", inet_ntoa(area_id));
		}
	}
	else
	{
		OSPF_LOG_DEBUG("The ospf instance %d does not exist.", ospf_id);
	}

	return 0;
}


int u0_device_update_handle(struct u0_device_info *new_u0)
{
	int ret = -1;
	uint16_t ospf_id = 0;
	uchar flag = 0;/*Used to distinguish whether to modify ne-ip or ne-id.*/
	struct in_addr dcn_opaque_id;
	struct in_addr area_id;
	struct in_addr old_ne_ip;
	struct ospf * ospf = NULL;
	struct ospf_area *area = NULL;
	struct ospf_lsa *route_lsa = NULL;
	struct ospf_lsa *opaque_type10_lsa = NULL;
	struct u0_device_info_local *u0_local = NULL;
	struct u0_device_info_local u0_info_temp;

	if(NULL == new_u0)
	{
		return -1;	
	}
	
	ospf_id = OSPF_MAX_PROCESS_ID;
	inet_aton ("202.255.238.0", &dcn_opaque_id);
	
	OSPF_LOG_DEBUG("Update ne-ip %s ifindex 0x%x\n", inet_ntoa(new_u0->ne_ip), new_u0->ifindex);
	
	u0_local = u0_repeat_detect(new_u0);
	if(!u0_local)
	{
		OSPF_LOG_DEBUG("The u0(ifindex = 0x%x) you update does not exist.", new_u0->ifindex);
		return -1;
	}

	ret = interface_update_ip(u0_local->loopback_ifindex, u0_local->u0_info.ne_ip, new_u0->ne_ip);
	if( ret != CMD_SUCCESS)
	{
		OSPF_LOG_DEBUG("The u0(ifindex = 0x%x) ip(loopback) update fail. ", u0_local->loopback_ifindex);
		return -1;
	}

	u0_map_info_send_to_target_process(&(u0_local->u0_info), MODULE_ID_SNMPD_TRAP, IPC_OPCODE_DELETE);
	old_ne_ip = u0_local->u0_info.ne_ip;
	if((old_ne_ip.s_addr == new_u0->ne_ip.s_addr) && (u0_local->u0_info.ne_id.s_addr != new_u0->ne_id.s_addr))
	{
		flag = 1;/* flag == 1 is modify ne-id */
	}
	u0_local->u0_info.ne_ip = new_u0->ne_ip;
	u0_local->u0_info.ne_id = new_u0->ne_id;

	u0_map_info_send_to_target_process(new_u0, MODULE_ID_SNMPD_TRAP, IPC_OPCODE_ADD);
	/* Send to vty so that vty is used to distinguish hostname*/
	memset(&u0_info_temp, 0, sizeof(struct u0_device_info_local));
	memcpy(&u0_info_temp, u0_local, sizeof(struct u0_device_info_local));
	u0_info_temp.u0_info.ifindex = u0_info_temp.u0_info.ifindex - 0xffe;
	u0_map_info_send_to_target_process(&(u0_info_temp.u0_info), MODULE_ID_VTY, IPC_OPCODE_UPDATE);
	
	ospf = ospf_lookup_id (ospf_id);
	if(ospf)
	{
		area_id.s_addr = 0;
		area = ospf_area_lookup_by_area_id (ospf, area_id);
		if(area)
		{	
			/* Find the old route-lsa */
			route_lsa = ospf_lsa_lookup_by_id (area, OSPF_ROUTER_LSA, old_ne_ip);
			if(!route_lsa)
			{
				OSPF_LOG_DEBUG("The route-lsa id = %s is not exist. ", inet_ntoa(old_ne_ip));
				return -1;
			}
			SET_FLAG (route_lsa->flags, OSPF_LSA_SELF);

			/* Find the old type10-lsa */
			opaque_type10_lsa = ospf_lsdb_lookup_by_id(area->lsdb, OSPF_OPAQUE_AREA_LSA, dcn_opaque_id, old_ne_ip);
			if(!opaque_type10_lsa)
			{
				OSPF_LOG_DEBUG("The type10-lsa adv = %s is not exist. ", inet_ntoa(old_ne_ip));
				return -1;
			}
			SET_FLAG (opaque_type10_lsa->flags, OSPF_LSA_SELF);
			
			if(!flag)
			{
				//clean target u0 route-lsa
				ospf_lsa_flush (ospf, route_lsa);

				//clean opaque-type10-lsa
				ospf_lsa_flush (ospf, opaque_type10_lsa);
			
				//reoriginate u0 route_lsa
				ospf_router_lsa_originate_for_u0 (area, &(u0_local->u0_info));
				//ospf_router_lsa_refresh_for_u0 (route_lsa)
				ospf_opaque_type10_lsa_generate_for_u0(area, &(u0_local->u0_info));
			}
			else
			{
				//ospf_router_lsa_refresh_for_u0 (route_lsa)
				ospf_opaque_type10_lsa_refresh_for_u0(opaque_type10_lsa);
			}
			ospf_router_lsa_update_area (area);//update self-originate route_lsa

			//u0_map_info_send_to_target_process(new_u0, MODULE_ID_SNMPD, IPC_OPCODE_ADD);
		}
		else
		{
			OSPF_LOG_DEBUG("The area %s does not exist.", inet_ntoa(area_id));
		}
	}
	return 0;
}


