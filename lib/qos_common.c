
#include <lib/qos_common.h>
#include <lib/module_id.h>
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>
#include <lib/log.h>
#include <string.h>



/* 判断QoS Domain模板是否存在 */
int *qos_domain_exist_get(uint8_t index, enum QOS_TYPE type, int module_id)
{
#if 0
    return ipc_send_common_wait_reply(&type, sizeof(enum QOS_TYPE), 1 , MODULE_ID_QOS, module_id,
                                        IPC_TYPE_QOS, QOS_INFO_DOMAIN, IPC_OPCODE_GET, (uint32_t)index);
#else
	struct ipc_mesg_n* pMsgRep = NULL;
	static int exist = 0;
	pMsgRep = ipc_sync_send_n2(&type, sizeof(enum QOS_TYPE), 1 , MODULE_ID_QOS, module_id,
                                        IPC_TYPE_QOS, QOS_INFO_DOMAIN, IPC_OPCODE_GET, (uint32_t)index, 0);
    if (NULL == pMsgRep)
    {
    	zlog_err ( "send sync message fail.\n" );
        return NULL;
    }

    exist = *((int *)pMsgRep->msg_data);

    mem_share_free(pMsgRep, MODULE_ID_QOS);

    return &exist;
#endif
}


/* 操作QoS Domain引用计数 */
int qos_domain_ref_operate(uint8_t index, enum QOS_TYPE type, int module_id, uint8_t operate)
{
	enum IPC_OPCODE opcode = IPC_OPCODE_INVALID;

	if(REF_OPER_INCREASE == operate)
	{
		opcode = IPC_OPCODE_ADD;
	}
	else
	{
		opcode = IPC_OPCODE_DELETE;
	}
#if 0
	return ipc_send_common1(&type, sizeof(enum QOS_TYPE), 1, MODULE_ID_QOS, module_id,
									IPC_TYPE_QOS, QOS_INFO_DOMAIN, opcode, (uint32_t)index);
#else
	return ipc_send_msg_n2(&type, sizeof(enum QOS_TYPE), 1, MODULE_ID_QOS, module_id,
									IPC_TYPE_QOS, QOS_INFO_DOMAIN, opcode, (uint32_t)index);
#endif
}


/* 判断QoS Phb模板是否存在 */
int *qos_phb_exist_get(uint8_t index, enum QOS_TYPE type, int module_id)
{
#if 0
    return ipc_send_common_wait_reply(&type, sizeof(enum QOS_TYPE), 1 , MODULE_ID_QOS, module_id,
                                        IPC_TYPE_QOS, QOS_INFO_PHB, IPC_OPCODE_GET, (uint32_t)index);
#else
	struct ipc_mesg_n* pMsgRep = NULL;
	static int exist = 0;
	pMsgRep = ipc_sync_send_n2(&type, sizeof(enum QOS_TYPE), 1 , MODULE_ID_QOS, module_id,
                                        IPC_TYPE_QOS, QOS_INFO_PHB, IPC_OPCODE_GET, (uint32_t)index, 0);
    if (NULL == pMsgRep)
    {
    	zlog_err ( "send sync message fail.\n" );
        return NULL;
    }

    exist = *((int *)pMsgRep->msg_data);

    mem_share_free(pMsgRep, MODULE_ID_QOS);

    return &exist;
#endif
}


/* 操作QoS Phb引用计数 */
int qos_phb_ref_operate(uint8_t index, enum QOS_TYPE type, int module_id, uint8_t operate)
{
	enum IPC_OPCODE opcode = IPC_OPCODE_INVALID;

	if(REF_OPER_INCREASE == operate)
	{
		opcode = IPC_OPCODE_ADD;
	}
	else if(REF_OPER_DECREASE == operate)
	{
		opcode = IPC_OPCODE_DELETE;
	}
#if 0
	return ipc_send_common1(&type, sizeof(enum QOS_TYPE), 1, MODULE_ID_QOS, module_id,
									IPC_TYPE_QOS, QOS_INFO_PHB, opcode, (uint32_t)index);
#else
	return ipc_send_msg_n2(&type, sizeof(enum QOS_TYPE), 1, MODULE_ID_QOS, module_id,
									IPC_TYPE_QOS, QOS_INFO_PHB, opcode, (uint32_t)index);
#endif
}


/* 判断HQoS queue模板是否存在 */
int *qos_queue_profile_exist_get(uint32_t index, int module_id)
{
#if 0
    return ipc_send_common_wait_reply(NULL, 0, 1 , MODULE_ID_QOS, module_id, IPC_TYPE_HQOS,
    									HQOS_SUBTYPE_QUEUE_PROFILE, IPC_OPCODE_GET, index);
#else
	struct ipc_mesg_n* pMsgRep = NULL;
	static int exist = 0;
	pMsgRep = ipc_sync_send_n2(NULL, 0, 1 , MODULE_ID_QOS, module_id, IPC_TYPE_HQOS,
    									HQOS_SUBTYPE_QUEUE_PROFILE, IPC_OPCODE_GET, index, 0);
	if (NULL == pMsgRep)
	{
		zlog_err ( "send sync message fail.\n" );
		return NULL;
	}

	exist = *((int *)pMsgRep->msg_data);

	mem_share_free(pMsgRep, MODULE_ID_QOS);

	return &exist;
#endif
}


/* 从QoS进程分配hqos index */
uint16_t *qos_hqosid_get(struct hqos_t *phqos, int module_id)
{
#if 0
    return ipc_send_common_wait_reply(phqos, (sizeof(struct hqos_t)), 1 , MODULE_ID_QOS, module_id,
                                        IPC_TYPE_HQOS, HQOS_SUBTYPE_ID, IPC_OPCODE_ADD, 0);
#else
	struct ipc_mesg_n* pMsgRep = NULL;
	static uint16_t id = 0;
	pMsgRep = ipc_sync_send_n2(phqos, (sizeof(struct hqos_t)), 1 , MODULE_ID_QOS, module_id,
                                        IPC_TYPE_HQOS, HQOS_SUBTYPE_ID, IPC_OPCODE_ADD, 0, 0);
	if (NULL == pMsgRep)
	{
		zlog_err ( "send sync message fail.\n" );
		return NULL;
	}

	id = *((uint16_t *)pMsgRep->msg_data);

	mem_share_free(pMsgRep, MODULE_ID_QOS);

	return &id;
#endif
}


/* 从QoS进程获取hqos信息 */
struct hqos_t *qos_hqos_get(uint16_t hqos_id, int module_id)
{
#if 0
    return ipc_send_common_wait_reply(NULL, 0, 1 , MODULE_ID_QOS, module_id,
                                        IPC_TYPE_HQOS, HQOS_SUBTYPE_ID, IPC_OPCODE_GET, hqos_id);
#else
	struct ipc_mesg_n* pMsgRep = NULL;
	static struct hqos_t hqos;
	pMsgRep = ipc_sync_send_n2(NULL, 0, 1 , MODULE_ID_QOS, module_id,
                                        IPC_TYPE_HQOS, HQOS_SUBTYPE_ID, IPC_OPCODE_GET, hqos_id, 0);
	if (NULL == pMsgRep)
	{
		zlog_err ( "send sync message fail.\n" );
		return NULL;
	}

	memcpy(&hqos, pMsgRep->msg_data, sizeof(struct hqos_t));

	mem_share_free(pMsgRep, MODULE_ID_QOS);

	return &hqos;
#endif
}


/* 删除QoS进程hqos信息 */
void qos_hqos_delete(uint16_t hqos_id, int module_id)
{
#if 0
    ipc_send_common1(NULL, 0, 1 , MODULE_ID_QOS, module_id,
                              IPC_TYPE_HQOS, HQOS_SUBTYPE_ID, IPC_OPCODE_DELETE, hqos_id);
#else
	ipc_send_msg_n2(NULL, 0, 1 , MODULE_ID_QOS, module_id,
                              IPC_TYPE_HQOS, HQOS_SUBTYPE_ID, IPC_OPCODE_DELETE, hqos_id);
#endif
}



/* 批量返回 car_id 后面的car模板信息，返回值是数组的指针，*pdata_num 返回实际的car模板数量 */
struct ipc_mesg_n *qos_com_get_car_profile_bulk ( uint16_t car_id, int module_id, int *pdata_num )
{
#if 0
	struct ipc_mesg *pmesg = ipc_send_common_wait_reply1 ( NULL, 0, 1 , MODULE_ID_QOS, module_id,
                             IPC_TYPE_CAR, CAR_INFO_CARID, IPC_OPCODE_GET_BULK, car_id );
    if ( pmesg )
    {
        *pdata_num = pmesg->msghdr.data_num;
        return ( struct car_t * ) pmesg->msg_data;
    }

    return NULL;
#else
	struct ipc_mesg_n* pMsgRep = NULL;
	pMsgRep = ipc_sync_send_n2( NULL, 0, 1 , MODULE_ID_QOS, module_id,
                             IPC_TYPE_CAR, CAR_INFO_CARID, IPC_OPCODE_GET_BULK, car_id, 0 );
	if (NULL == pMsgRep)
	{
		zlog_err ( "send sync message fail.\n" );
		return NULL;
	}

	*pdata_num = pMsgRep->msghdr.data_num;

	return pMsgRep;
#endif
}

/* 批量返回 wred_id 后面的wred模板信息，返回值是数组的指针，*pdata_num 返回实际的wred模板数量 */
struct ipc_mesg_n *qos_com_get_wred_profile_bulk ( uint8_t wred_id, int module_id, int *pdata_num )
{
#if 0
	struct ipc_mesg *pmesg = ipc_send_common_wait_reply1 ( NULL, 0, 1 , MODULE_ID_QOS, module_id,
                             IPC_TYPE_HQOS, HQOS_SUBTYPE_WRED_PROFILE, IPC_OPCODE_GET_BULK, wred_id );
    if ( pmesg )
    {
        *pdata_num = pmesg->msghdr.data_num;
        return ( struct wred_profile * ) pmesg->msg_data;
    }

    return NULL;
#else
	struct ipc_mesg_n* pMsgRep = NULL;
	pMsgRep = ipc_sync_send_n2( NULL, 0, 1 , MODULE_ID_QOS, module_id,
                             IPC_TYPE_HQOS, HQOS_SUBTYPE_WRED_PROFILE, IPC_OPCODE_GET_BULK, wred_id, 0 );
	if (NULL == pMsgRep)
	{
		zlog_err ( "send sync message fail.\n" );
		return NULL;
	}

	*pdata_num = pMsgRep->msghdr.data_num;
	
	return pMsgRep;
#endif
}


/* 批量返回 queue_id 后面的队列模板信息，返回值是数组的指针，*pdata_num 返回实际的队列模板数量 */
struct ipc_mesg_n *qos_com_get_queue_profile_bulk ( uint8_t queue_id, int module_id, int *pdata_num )
{
#if 0
	struct ipc_mesg *pmesg = ipc_send_common_wait_reply1 ( NULL, 0, 1 , MODULE_ID_QOS, module_id,
                             IPC_TYPE_HQOS, HQOS_SUBTYPE_QUEUE_PROFILE, IPC_OPCODE_GET_BULK, queue_id );
    if ( pmesg )
    {
        *pdata_num = pmesg->msghdr.data_num;
        return ( struct queue_profile * ) pmesg->msg_data;
    }

    return NULL;
#else
	struct ipc_mesg_n* pMsgRep = NULL;
	pMsgRep = ipc_sync_send_n2( NULL, 0, 1 , MODULE_ID_QOS, module_id,
                             IPC_TYPE_HQOS, HQOS_SUBTYPE_QUEUE_PROFILE, IPC_OPCODE_GET_BULK, queue_id, 0 );
	if (NULL == pMsgRep)
	{
		zlog_err ( "send sync message fail.\n" );
		return NULL;
	}

	*pdata_num = pMsgRep->msghdr.data_num;

	return pMsgRep;
#endif
}

/* 批量返回 domain_id 后面的domain模板信息，返回值是数组的指针，*pdata_num 返回实际的domain模板数量 */
struct ipc_mesg_n *qos_com_get_domain_profile_bulk ( uint8_t domain_id, enum QOS_TYPE type, int module_id, int *pdata_num )
{
#if 0
	struct ipc_mesg *pmesg = ipc_send_common_wait_reply1 ( &type, sizeof(enum QOS_TYPE), 1 , MODULE_ID_QOS, 
									module_id, IPC_TYPE_QOS, QOS_INFO_DOMAIN, IPC_OPCODE_GET_BULK, domain_id );
    if ( pmesg )
    {
        *pdata_num = pmesg->msghdr.data_num;
        return ( struct qos_domain * ) pmesg->msg_data;
    }

    return NULL;
#else
	struct ipc_mesg_n* pMsgRep = NULL;
	pMsgRep = ipc_sync_send_n2( &type, sizeof(enum QOS_TYPE), 1 , MODULE_ID_QOS, 
									module_id, IPC_TYPE_QOS, QOS_INFO_DOMAIN, IPC_OPCODE_GET_BULK, domain_id, 0 );
	if (NULL == pMsgRep)
	{
		zlog_err ( "send sync message fail.\n" );
		return NULL;
	}

	*pdata_num = pMsgRep->msghdr.data_num;

	return pMsgRep;
#endif
}


/* 批量返回 phb_id 后面的phb模板信息，返回值是数组的指针，*pdata_num 返回实际的phb模板数量 */
struct ipc_mesg_n *qos_com_get_phb_profile_bulk ( uint8_t phb_id, enum QOS_TYPE type, int module_id, int *pdata_num )
{
#if 0
	struct ipc_mesg *pmesg = ipc_send_common_wait_reply1 ( &type, sizeof(enum QOS_TYPE), 1 , MODULE_ID_QOS, 
									module_id, IPC_TYPE_QOS, QOS_INFO_PHB, IPC_OPCODE_GET_BULK, phb_id );
    if ( pmesg )
    {
        *pdata_num = pmesg->msghdr.data_num;
        return ( struct qos_phb * ) pmesg->msg_data;
    }

    return NULL;
#else
	struct ipc_mesg_n* pMsgRep = NULL;
	pMsgRep = ipc_sync_send_n2( &type, sizeof(enum QOS_TYPE), 1 , MODULE_ID_QOS, 
									module_id, IPC_TYPE_QOS, QOS_INFO_PHB, IPC_OPCODE_GET_BULK, phb_id, 0 );
	if (NULL == pMsgRep)
	{
		zlog_err ( "send sync message fail.\n" );
		return NULL;
	}

	*pdata_num = pMsgRep->msghdr.data_num;

	return pMsgRep;
#endif
}


/* 批量返回 struct  rule_node信息，acl_num为0ruleid为0从头返，acl_num 有值，rule有值从后面返*/
struct ipc_mesg_n *qos_com_get_acl_rule_bulk ( uint32_t acl_num,uint16_t ruleid,int module_id, int *pdata_num )
{
#if 0
	struct ipc_mesg *pmesg = ipc_send_common_wait_reply1 ( &ruleid, sizeof(uint16_t), 1 , MODULE_ID_QOS, module_id,
                             IPC_TYPE_ACL, QOS_INFO_ACL_RULE, IPC_OPCODE_GET_BULK, acl_num );
    if ( pmesg )
    {
        *pdata_num = pmesg->msghdr.data_num;
        return ( struct rule_node *) pmesg->msg_data;
    }

    return NULL;
#else
	struct ipc_mesg_n* pMsgRep = NULL;
	pMsgRep = ipc_sync_send_n2( &ruleid, sizeof(uint16_t), 1 , MODULE_ID_QOS, module_id,
                             IPC_TYPE_ACL, QOS_INFO_ACL_RULE, IPC_OPCODE_GET_BULK, acl_num, 0 );
	if (NULL == pMsgRep)
	{
		zlog_err ( "send sync message fail.\n" );
		return NULL;
	}

	*pdata_num = pMsgRep->msghdr.data_num;

	return pMsgRep;
#endif
}

/* 批量返回 struct qos_policy信息,由qos_policy 里type,direct,ifindex,acl_num作为索引 */
struct ipc_mesg_n *qos_com_get_policy_bulk (struct qos_policy *policy,int module_id, int *pdata_num )
{
#if 0
	struct ipc_mesg *pmesg = ipc_send_common_wait_reply1 ( policy, sizeof(struct qos_policy), 1 , MODULE_ID_QOS, module_id,
                             IPC_TYPE_ACL, QOS_INFO_POLICY, IPC_OPCODE_GET_BULK, 0 );
    if ( pmesg )
    {
        *pdata_num = pmesg->msghdr.data_num;
        return ( struct qos_policy *) pmesg->msg_data;
    }

    return NULL;
#else
	struct ipc_mesg_n* pMsgRep = NULL;
	pMsgRep = ipc_sync_send_n2( policy, sizeof(struct qos_policy), 1 , MODULE_ID_QOS, module_id,
                             IPC_TYPE_ACL, QOS_INFO_POLICY, IPC_OPCODE_GET_BULK, 0, 0 );
	if (NULL == pMsgRep)
	{
		zlog_err ( "send sync message fail.\n" );
		return NULL;
	}

	*pdata_num = pMsgRep->msghdr.data_num;

	return pMsgRep;
#endif
}

/* 批量返回 mirror group信息，返回值是数组的指针，*pdata_num 返回实际的group id数量 */
struct ipc_mesg_n* qos_com_get_mirror_group_bulk ( uint32_t ifindex, int group_id, int module_id, int *pdata_num )
{
#if 0
	struct ipc_mesg *pmesg = ipc_send_common_wait_reply1 ( &group_id, sizeof(int), 1 , MODULE_ID_QOS, 
									module_id, IPC_TYPE_ACL, QOS_INFO_MIRROR_GROUP, IPC_OPCODE_GET_BULK, ifindex );
    if ( pmesg )
    {
        *pdata_num = pmesg->msghdr.data_num;
        return ( struct qos_mirror_snmp * ) pmesg->msg_data;
    }

    return NULL;
#else
	struct ipc_mesg_n* pMsgRep = NULL;
	pMsgRep = ipc_sync_send_n2( &group_id, sizeof(int), 1 , MODULE_ID_QOS, 
									module_id, IPC_TYPE_ACL, QOS_INFO_MIRROR_GROUP, IPC_OPCODE_GET_BULK, ifindex , 0);
	if (NULL == pMsgRep)
	{
		zlog_err ( "send sync message fail.\n" );
		return NULL;
	}

	*pdata_num = pMsgRep->msghdr.data_num;

	return pMsgRep;
#endif
}

/* 批量返回接口镜像信息，返回值是数组的指针，*pdata_num 返回实际的car_t数量 */
struct ipc_mesg_n* qos_com_get_mirror_port_bulk ( uint32_t ifindex, int module_id, int *pdata_num )
{
#if 0
	struct ipc_mesg *pmesg = ipc_send_common_wait_reply1 ( NULL, sizeof(enum QOS_DIR), 1 , MODULE_ID_QOS, 
									module_id, IPC_TYPE_ACL, QOS_INFO_MIRROR_GROUP_PORT, IPC_OPCODE_GET_BULK, ifindex );
    if ( pmesg )
    {
        *pdata_num = pmesg->msghdr.data_num;
        return ( struct qos_if_mirror* ) pmesg->msg_data;
    }

    return NULL;
#else
	struct ipc_mesg_n* pMsgRep = NULL;
	pMsgRep = ipc_sync_send_n2( NULL, sizeof(enum QOS_DIR), 1 , MODULE_ID_QOS, 
						module_id, IPC_TYPE_ACL, QOS_INFO_MIRROR_GROUP_PORT, IPC_OPCODE_GET_BULK, ifindex, 0 );
	if (NULL == pMsgRep)
	{
		zlog_err ( "send sync message fail.\n" );
		return NULL;
	}

	*pdata_num = pMsgRep->msghdr.data_num;

	return pMsgRep;
#endif
}


/* 批量返回 接口上应用个的car模板信息，返回值是数组的指针，*pdata_num 返回实际的car id数量 */
struct ipc_mesg_n *qos_com_get_car_apply_bulk ( uint32_t ifindex, enum QOS_DIR dir, int module_id, int *pdata_num )
{
#if 0
	struct ipc_mesg *pmesg = ipc_send_common_wait_reply1 ( &dir, sizeof(enum QOS_DIR), 1 , MODULE_ID_QOS, 
									module_id, IPC_TYPE_QOSIF, CAR_INFO_CARID, IPC_OPCODE_GET_BULK, ifindex );
    if ( pmesg )
    {
        *pdata_num = pmesg->msghdr.data_num;
        return ( uint32_t * ) pmesg->msg_data;
    }

    return NULL;
#else
	struct ipc_mesg_n* pMsgRep = NULL;
	pMsgRep = ipc_sync_send_n2( NULL, sizeof(enum QOS_DIR), 1 , MODULE_ID_QOS, 
						module_id, IPC_TYPE_ACL, QOS_INFO_MIRROR_GROUP_PORT, IPC_OPCODE_GET_BULK, ifindex, 0 );
	if (NULL == pMsgRep)
	{
		zlog_err ( "send sync message fail.\n" );
		return NULL;
	}

	*pdata_num = pMsgRep->msghdr.data_num;
	
	return pMsgRep;
#endif
}


/* 批量返回接口限速信息，返回值是数组的指针，*pdata_num 返回实际的car_t数量 */
struct ipc_mesg_n *qos_com_get_car_bulk ( uint32_t ifindex, enum QOS_DIR dir, int module_id, int *pdata_num )
{
#if 0
	struct ipc_mesg *pmesg = ipc_send_common_wait_reply1 ( &dir, sizeof(enum QOS_DIR), 1 , MODULE_ID_QOS, 
									module_id, IPC_TYPE_QOSIF, CAR_INFO_CIR, IPC_OPCODE_GET_BULK, ifindex );
    if ( pmesg )
    {
        *pdata_num = pmesg->msghdr.data_num;
        return ( struct car_t * ) pmesg->msg_data;
    }

    return NULL;
#else
	struct ipc_mesg_n* pMsgRep = NULL;
	pMsgRep = ipc_sync_send_n2( &dir, sizeof(enum QOS_DIR), 1 , MODULE_ID_QOS, 
						module_id, IPC_TYPE_QOSIF, CAR_INFO_CIR, IPC_OPCODE_GET_BULK, ifindex, 0 );
	if (NULL == pMsgRep)
	{
		zlog_err ( "send sync message fail.\n" );
		return NULL;
	}

	*pdata_num = pMsgRep->msghdr.data_num;

	return pMsgRep;
#endif
}


struct ipc_mesg_n *qos_com_get_qosif_bulk(uint32_t ifindex, int module_id, int*pdata_num)
{
#if 0
	struct ipc_mesg *pmesg = ipc_send_common_wait_reply1 ( NULL, 0, 1 , MODULE_ID_QOS, module_id, 
										IPC_TYPE_QOSIF, QOS_INFO_MAX, IPC_OPCODE_GET_BULK, ifindex );
    if ( pmesg )
    {
        *pdata_num = pmesg->msghdr.data_num;
        return ( struct qos_if_snmp * ) pmesg->msg_data;
    }

    return NULL;
#else
	struct ipc_mesg_n* pMsgRep = NULL;
	pMsgRep = ipc_sync_send_n2( NULL, 0, 1 , MODULE_ID_QOS, module_id, 
						IPC_TYPE_QOSIF, QOS_INFO_MAX, IPC_OPCODE_GET_BULK, ifindex, 0 );
	if (NULL == pMsgRep)
	{
		zlog_err ( "send sync message fail.\n" );
		return NULL;
	}

	*pdata_num = pMsgRep->msghdr.data_num;

	return pMsgRep;
#endif
}


