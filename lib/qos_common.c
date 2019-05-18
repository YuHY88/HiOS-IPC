
#include <lib/qos_common.h>
#include <lib/module_id.h>
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>
#include <lib/log.h>
#include <string.h>



/* �ж�QoS Domainģ���Ƿ���� */
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


/* ����QoS Domain���ü��� */
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


/* �ж�QoS Phbģ���Ƿ���� */
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


/* ����QoS Phb���ü��� */
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


/* �ж�HQoS queueģ���Ƿ���� */
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


/* ��QoS���̷���hqos index */
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


/* ��QoS���̻�ȡhqos��Ϣ */
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


/* ɾ��QoS����hqos��Ϣ */
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



/* �������� car_id �����carģ����Ϣ������ֵ�������ָ�룬*pdata_num ����ʵ�ʵ�carģ������ */
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

/* �������� wred_id �����wredģ����Ϣ������ֵ�������ָ�룬*pdata_num ����ʵ�ʵ�wredģ������ */
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


/* �������� queue_id ����Ķ���ģ����Ϣ������ֵ�������ָ�룬*pdata_num ����ʵ�ʵĶ���ģ������ */
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

/* �������� domain_id �����domainģ����Ϣ������ֵ�������ָ�룬*pdata_num ����ʵ�ʵ�domainģ������ */
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


/* �������� phb_id �����phbģ����Ϣ������ֵ�������ָ�룬*pdata_num ����ʵ�ʵ�phbģ������ */
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


/* �������� struct  rule_node��Ϣ��acl_numΪ0�rruleidΪ0��ͷ����acl_num ��ֵ��rule��ֵ�Ӻ��淵*/
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

/* �������� struct qos_policy��Ϣ,��qos_policy ��type,direct,ifindex,acl_num��Ϊ���� */
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

/* �������� mirror group��Ϣ������ֵ�������ָ�룬*pdata_num ����ʵ�ʵ�group id���� */
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

/* �������ؽӿھ�����Ϣ������ֵ�������ָ�룬*pdata_num ����ʵ�ʵ�car_t���� */
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


/* �������� �ӿ���Ӧ�ø���carģ����Ϣ������ֵ�������ָ�룬*pdata_num ����ʵ�ʵ�car id���� */
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


/* �������ؽӿ�������Ϣ������ֵ�������ָ�룬*pdata_num ����ʵ�ʵ�car_t���� */
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


