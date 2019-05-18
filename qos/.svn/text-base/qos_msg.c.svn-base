#include <qos/qos_mapping.h>
#include <qos/qos_if.h>
#include <qos/qos_car.h>
#include <qos/hqos.h>
#include <qos/acl.h>
#include <qos/qos_policy.h>
#include <qos/qos_msg.h>
#include <qos/sla/sla_session.h>

#include <lib/qos_common.h>
#include <lib/module_id.h>
#include <lib/ifm_common.h>
#include <lib/types.h>
#include <lib/errcode.h>
#include <lib/index.h>
#include <lib/thread.h>
#include <lib/log.h>
#include <lib/memshare.h>

#include <string.h>
#include <unistd.h>

static int qos_msg_rcv_qos(struct ipc_mesg_n* mesg);
static int qos_msg_rcv_ifm(struct ipc_mesg_n* mesg);
static int qos_msg_rcv_sla(struct ipc_mesg_n* mesg);
static int qos_msg_rcv_hqos(struct ipc_mesg_n* mesg);
//extern void sla_if_delete(uint32_t ifindex);


static int sla_get_bulk_session(struct sla_session *sla_buf, uint32_t session_id, uint32_t sla_max)
{
    struct hash_bucket *pbucket = NULL;
    struct sla_session *psla    = NULL;
    uint32_t sla_cnt            = 0;
	int cursor                  = 0;
	int flag					= 0;

	if(0 == session_id)
	{
		HASH_BUCKET_LOOP(pbucket, cursor, sla_session_table)
		{
			psla = (struct sla_session *)pbucket->data;
	        if (NULL == psla)
	        {
				continue;
	        }

			if (psla->info.session_id > session_id)
			{
	        	memcpy(&sla_buf[sla_cnt++], psla, sizeof(struct sla_session));
	        	if (sla_cnt == sla_max)
	        	{
	            	return sla_cnt;
	        	}
			}
	    }
	}
	else
	{
		HASH_BUCKET_LOOP(pbucket, cursor, sla_session_table)
		{
			psla = (struct sla_session *)pbucket->data;
			if (NULL == psla)
	        {
				continue;
	        }
			
			if(psla->info.session_id == session_id)//找到上一轮返回的最后一个索引的位置
			{
				flag = 1;
				continue;
			}

			if(flag == 1)
			{
				memcpy(&sla_buf[sla_cnt++], psla, sizeof(struct sla_session));
	        	if (sla_cnt == sla_max)
	        	{
	            	return sla_cnt;
	        	}
			}
		}
	}
	

	return sla_cnt;
}

static int sla_msg_rcv_get_bulk_session(struct ipc_msghdr_n *pmsghdr, void *pdata)
{
    uint32_t sla_max            = IPC_MSG_LEN/sizeof(struct sla_session);
    struct sla_session sla_buf[sla_max];
	uint32_t sla_cnt            = 0;
	int ret                     = 0;

	memset (sla_buf, 0, sla_max * sizeof(struct sla_session));

    sla_cnt = sla_get_bulk_session(sla_buf, pmsghdr->msg_index, sla_max);
	if (sla_cnt > 0)
	{
		ret = ipc_send_reply_n2(sla_buf, sla_cnt * sizeof(struct sla_session), sla_cnt, pmsghdr->sender_id,
                             MODULE_ID_QOS, IPC_TYPE_SLA, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_REPLY);						 
	}
	else
	{
		ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_QOS, 
							IPC_TYPE_SLA, pmsghdr->msg_subtype, 1, pmsghdr->msg_index, IPC_OPCODE_NACK);	
	}

	return ret;
}


static int qos_msg_reply_car_profile_bulk ( struct ipc_msghdr_n *phdr )
{
	int data_num = IPC_MSG_LEN/sizeof(struct car_t);
    struct car_t car_array[data_num];
    uint32_t car_id = 0;
    int ret = 0;

    if ( NULL == phdr )
    {
    	QOS_LOG_DBG( "Input param check failed!\n" );
        return ERRNO_PARAM_ILLEGAL;
    }

    car_id = phdr->msg_index;
    memset ( car_array, 0, data_num * sizeof ( struct car_t ) );

	ret = qos_car_profile_get_bulk(car_id, car_array);
    if (ret > 0)
    {
    #if 0
        ret = ipc_send_reply_bulk(car_array, ret*sizeof(struct car_t), ret, phdr->sender_id,
                                MODULE_ID_QOS, IPC_TYPE_CAR, phdr->msg_subtype, phdr->msg_index);
    #else
    	ret = ipc_send_reply_n2(car_array, ret*sizeof(struct car_t), ret, phdr->sender_id,
                                MODULE_ID_QOS, IPC_TYPE_CAR, phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_REPLY);	
    #endif
    }
    else
    {
    #if 0
        ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_QOS, IPC_TYPE_CAR,
                                phdr->msg_subtype, phdr->msg_index);
    #else
		ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, MODULE_ID_QOS, IPC_TYPE_CAR, 
								phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_NACK);		
    #endif
    }

    return ret;
}


static int qos_msg_reply_wred_profile_bulk ( struct ipc_msghdr_n *phdr )
{
    struct wred_profile wred_array[HQOS_WRED_PROFILE_NUM];
    uint8_t wred_id = 0;
    int ret = 0;

    if ( NULL == phdr )
    {
    	QOS_LOG_DBG( "Input param check failed!\n" );
        return ERRNO_PARAM_ILLEGAL;
    }

    wred_id = phdr->msg_index;
    memset ( wred_array, 0, HQOS_WRED_PROFILE_NUM * sizeof ( struct wred_profile ) );

	ret = hqos_wred_profile_get_bulk(wred_id, wred_array);
    if (ret > 0)
    {
    #if 0
        ret = ipc_send_reply_bulk(wred_array, ret*sizeof(struct wred_profile), ret, phdr->sender_id,
                                MODULE_ID_QOS, IPC_TYPE_HQOS, phdr->msg_subtype, phdr->msg_index);
	#else
		ret = ipc_send_reply_n2(wred_array, ret*sizeof(struct wred_profile), ret, phdr->sender_id,
                                MODULE_ID_QOS, IPC_TYPE_HQOS, phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_REPLY);	
	#endif
    }
    else
    {
    #if 0
        ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_QOS, IPC_TYPE_HQOS,
                                phdr->msg_subtype, phdr->msg_index);
    #else
		ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, MODULE_ID_QOS, IPC_TYPE_HQOS, 
								phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_NACK);	
    #endif
    }

    return ret;
}


static int qos_msg_reply_queue_profile_bulk ( struct ipc_msghdr_n *phdr )
{
	int data_num = IPC_MSG_LEN/sizeof(struct queue_profile);
    struct queue_profile queue_array[data_num];
    uint8_t queue_id = 0;
    int ret = 0;

    if ( NULL == phdr )
    {
    	QOS_LOG_DBG( "Input param check failed!\n" );
        return ERRNO_PARAM_ILLEGAL;
    }

    queue_id = phdr->msg_index;
    memset ( queue_array, 0, data_num * sizeof ( struct queue_profile ) );

	ret = hqos_queue_profile_get_bulk(queue_id, queue_array);
    if (ret > 0)
    {
    #if 0
        ret = ipc_send_reply_bulk(queue_array, ret*sizeof(struct queue_profile), ret, phdr->sender_id,
                                MODULE_ID_QOS, IPC_TYPE_HQOS, phdr->msg_subtype, phdr->msg_index);
    #else
    	ret = ipc_send_reply_n2(queue_array, ret*sizeof(struct queue_profile), ret, phdr->sender_id,
                                MODULE_ID_QOS, IPC_TYPE_HQOS, phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_REPLY);	
    #endif
    }
    else
    {
    #if 0
        ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_QOS, IPC_TYPE_HQOS,
                                phdr->msg_subtype, phdr->msg_index);
    #else
		ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, MODULE_ID_QOS, IPC_TYPE_HQOS, 
								phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_NACK);	
    #endif
    }

    return ret;
}


static int qos_msg_reply_domain_bulk ( struct ipc_msghdr_n *phdr, enum QOS_TYPE type )
{
	int data_num = IPC_MSG_LEN/sizeof(struct qos_domain);
    struct qos_domain domain_array[data_num];
    uint8_t domain_id = 0;
    int ret = 0;

    if ( NULL == phdr )
    {
    	QOS_LOG_DBG( "Input param check failed!\n" );
        return ERRNO_PARAM_ILLEGAL;
    }

    domain_id = phdr->msg_index;
    memset ( domain_array, 0, data_num * sizeof ( struct qos_domain ) );

	ret = qos_domain_get_bulk(domain_id, type, domain_array);
    if (ret > 0)
    {
    #if 0
        ret = ipc_send_reply_bulk(domain_array, ret*sizeof(struct qos_domain), ret, phdr->sender_id,
                                MODULE_ID_QOS, IPC_TYPE_QOS, phdr->msg_subtype, phdr->msg_index);
    #else
    	ret = ipc_send_reply_n2(domain_array, ret*sizeof(struct qos_domain), ret, phdr->sender_id,
                                MODULE_ID_QOS, IPC_TYPE_QOS, phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_REPLY);
    #endif
    }
    else
    {
    #if 0
        ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_QOS, IPC_TYPE_QOS,
                                phdr->msg_subtype, phdr->msg_index);
    #else
		ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, MODULE_ID_QOS, IPC_TYPE_QOS, 
								phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_NACK);	
    #endif
    }

    return ret;
}


static int qos_msg_reply_phb_bulk ( struct ipc_msghdr_n *phdr, enum QOS_TYPE type )
{
    struct qos_phb phb_array[QOS_PHB_NUM];
    uint8_t phb_id = 0;
    int ret = 0;

    if ( NULL == phdr )
    {
    	QOS_LOG_DBG( "Input param check failed!\n" );
        return ERRNO_PARAM_ILLEGAL;
    }

    phb_id = phdr->msg_index;
    memset ( phb_array, 0, QOS_PHB_NUM * sizeof ( struct qos_phb ) );

	ret = qos_phb_get_bulk(phb_id, type, phb_array);
    if (ret > 0)
    {
    #if 0
        ret = ipc_send_reply_bulk(phb_array, ret*sizeof(struct qos_phb), ret, phdr->sender_id,
                                MODULE_ID_QOS, IPC_TYPE_QOS, phdr->msg_subtype, phdr->msg_index);
    #else
    	ret = ipc_send_reply_n2(phb_array, ret*sizeof(struct qos_phb), ret, phdr->sender_id,
                                MODULE_ID_QOS, IPC_TYPE_QOS, phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_REPLY);
    #endif
    }
    else
    {
    #if 0
        ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_QOS, IPC_TYPE_QOS,
                                phdr->msg_subtype, phdr->msg_index);
    #else
		ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, MODULE_ID_QOS, IPC_TYPE_QOS, 
								phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_NACK);	
    #endif
    }

    return ret;
}


static int qos_msg_reply_acl_rule_bulk ( struct ipc_msghdr_n *phdr, uint16_t ruleid)
{
	int data_num = IPC_MSG_LEN/sizeof(struct rule_node);
	struct rule_node acl_rule[data_num];
    uint32_t acl_num = 0;
    int ret = 0;

    if ( NULL == phdr )
    {
    	QOS_LOG_DBG( "Input param check failed!\n" );
        return ERRNO_PARAM_ILLEGAL;
    }

    acl_num = phdr->msg_index;
    memset ( acl_rule, 0, data_num * sizeof (struct rule_node));

	ret = acl_rule_get_bulk(acl_num, ruleid, acl_rule);
    if (ret > 0)
    {
    #if 0
        ret = ipc_send_reply_bulk(acl_rule, ret*sizeof(struct rule_node), ret, phdr->sender_id,
                                MODULE_ID_QOS, IPC_TYPE_ACL, phdr->msg_subtype, phdr->msg_index);
    #else
    	ret = ipc_send_reply_n2(acl_rule, ret*sizeof(struct rule_node), ret, phdr->sender_id,
                                MODULE_ID_QOS, IPC_TYPE_ACL, phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_REPLY);	
    #endif
    }
    else
    {
    #if 0
        ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_QOS, IPC_TYPE_ACL,
                                phdr->msg_subtype, phdr->msg_index);
    #else
		ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, MODULE_ID_QOS, IPC_TYPE_ACL, 
								phdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);	
    #endif
    }

    return ret;
}


static int qos_msg_reply_policy_bulk ( struct ipc_msghdr_n *phdr,struct qos_policy *ppolicy )
{
  	int data_num = IPC_MSG_LEN/sizeof(struct qos_policy);
	struct qos_policy qos_policy[data_num];
    int ret = 0;

    if ( NULL == phdr )
    {
    	QOS_LOG_DBG( "Input param check failed!\n" );
        return ERRNO_PARAM_ILLEGAL;
    }

    memset ( qos_policy, 0, data_num * sizeof (struct qos_policy ) );

	ret = qos_policy_get_bulk(ppolicy->ifindex,ppolicy->type,ppolicy->direct,ppolicy->acl_num,qos_policy);
    if (ret > 0)
    {
    #if 0
        ret = ipc_send_reply_bulk(qos_policy, ret*sizeof(struct qos_policy), ret, phdr->sender_id,
                                MODULE_ID_QOS, IPC_TYPE_ACL, phdr->msg_subtype, phdr->msg_index);
    #else
    	ret = ipc_send_reply_n2(qos_policy, ret*sizeof(struct qos_policy), ret, phdr->sender_id,
                                MODULE_ID_QOS, IPC_TYPE_ACL, phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_REPLY);	
    #endif
    }
    else
    {
    #if 0
        ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_QOS, IPC_TYPE_ACL,
                                phdr->msg_subtype, phdr->msg_index);
	#else
		ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, MODULE_ID_QOS, IPC_TYPE_ACL, 
								phdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);	
	#endif
    }

    return ret;
}

static int qos_msg_reply_mirror_group_bulk ( struct ipc_msghdr_n *phdr, struct qos_mirror_snmp *pmirror )
{
	int data_num = IPC_MSG_LEN/sizeof(struct qos_mirror_snmp);
	struct qos_mirror_snmp qos_mirror_snmp[data_num];
    int ret = 0;

    if ( NULL == phdr )
    {
    	QOS_LOG_DBG( "Input param check failed!\n" );
        return ERRNO_PARAM_ILLEGAL;
    }

    
    memset ( qos_mirror_snmp, 0, data_num * sizeof ( struct qos_mirror_snmp ) );
	ret = qos_mirror_group_get_bulk(pmirror->ifindex, pmirror->id, qos_mirror_snmp);
    if (ret > 0)
    {
    #if 0
        ret = ipc_send_reply_bulk(qos_mirror_snmp, ret*sizeof(struct qos_mirror_snmp), ret, phdr->sender_id,
                                MODULE_ID_QOS, IPC_TYPE_ACL, phdr->msg_subtype, phdr->msg_index);
    #else
    	ret = ipc_send_reply_n2(qos_mirror_snmp, ret*sizeof(struct qos_mirror_snmp), ret, phdr->sender_id,
                                MODULE_ID_QOS, IPC_TYPE_ACL, phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_REPLY);	
    #endif
    }
    else
    {
    #if 0
        ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_QOS, IPC_TYPE_ACL,
                                phdr->msg_subtype, phdr->msg_index);
    #else
		ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, MODULE_ID_QOS, IPC_TYPE_ACL, 
								phdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);	
    #endif
    }

    return ret;
}


static int qos_msg_reply_mirror_port_bulk ( struct ipc_msghdr_n *phdr, struct qos_if_mirror *pmirror )
{
	int data_num = IPC_MSG_LEN/sizeof(struct qos_if_mirror);
	struct qos_if_mirror pif_mirror[data_num];
    uint32_t ifindex = 0;
    int ret = 0;

    if ( NULL == phdr )
    {
    	QOS_LOG_DBG( "Input param check failed!\n" );
        return ERRNO_PARAM_ILLEGAL;
    }

    ifindex = phdr->msg_index;
    memset ( pif_mirror, 0, data_num * sizeof ( struct qos_if_mirror ) );

	ret = qos_mirror_port_get_bulk(ifindex, pif_mirror);
    if (ret > 0)
    {
    #if 0
        ret = ipc_send_reply_bulk(pif_mirror, ret*sizeof(struct qos_if_mirror), ret, phdr->sender_id,
                                MODULE_ID_QOS, IPC_TYPE_ACL, phdr->msg_subtype, phdr->msg_index);
    #else
    	ret = ipc_send_reply_n2(pif_mirror, ret*sizeof(struct qos_if_mirror), ret, phdr->sender_id,
                                MODULE_ID_QOS, IPC_TYPE_ACL, phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_REPLY);
    #endif
    }
    else
    {
    #if 0
        ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_QOS, IPC_TYPE_ACL,
                                phdr->msg_subtype, phdr->msg_index);
    #else
		ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, MODULE_ID_QOS, IPC_TYPE_ACL, 
								phdr->msg_subtype, 1, 0, IPC_OPCODE_NACK);	
    #endif
    }

    return ret;
}

static int qos_msg_reply_car_apply_bulk ( struct ipc_msghdr_n *phdr, enum QOS_DIR dir )
{
	int data_num = IPC_MSG_LEN/sizeof(uint32_t);
    uint32_t carid_array[data_num];
    uint32_t ifindex = 0;
    int ret = 0;

    if ( NULL == phdr )
    {
    	QOS_LOG_DBG( "Input param check failed!\n" );
        return ERRNO_PARAM_ILLEGAL;
    }

    ifindex = phdr->msg_index;
    memset ( carid_array, 0, data_num * sizeof ( uint32_t ) );

	ret = qos_if_car_apply_get_bulk(ifindex, dir, carid_array);
    if (ret > 0)
    {
    #if 0
        ret = ipc_send_reply_bulk(carid_array, ret*sizeof(uint32_t), ret, phdr->sender_id,
                                MODULE_ID_QOS, IPC_TYPE_QOSIF, phdr->msg_subtype, phdr->msg_index);
	#else
		ret = ipc_send_reply_n2(carid_array, ret*sizeof(uint32_t), ret, phdr->sender_id,
                                MODULE_ID_QOS, IPC_TYPE_QOSIF, phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_REPLY);
	#endif
    }
    else
    {
    #if 0
        ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_QOS, IPC_TYPE_CAR,
                                phdr->msg_subtype, phdr->msg_index);
    #else
		ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, MODULE_ID_QOS, IPC_TYPE_QOSIF, 
								phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_NACK);	
    #endif
    }

    return ret;
}


static int qos_msg_reply_car_param_bulk ( struct ipc_msghdr_n *phdr, enum QOS_DIR dir )
{
	int data_num = IPC_MSG_LEN/sizeof(struct car_t);
    struct car_t car_array[data_num];
    uint32_t ifindex = 0;
    int ret = 0;

    if ( NULL == phdr )
    {
    	QOS_LOG_DBG( "Input param check failed!\n" );
        return ERRNO_PARAM_ILLEGAL;
    }

    ifindex = phdr->msg_index;
    memset ( car_array, 0, data_num * sizeof ( struct car_t ) );

	ret = qos_if_car_param_get_bulk(ifindex, dir, car_array);
    if (ret > 0)
    {
    #if 0
        ret = ipc_send_reply_bulk(car_array, ret*sizeof(struct car_t), ret, phdr->sender_id,
                                MODULE_ID_QOS, IPC_TYPE_QOSIF, phdr->msg_subtype, phdr->msg_index);
	#else
		ret = ipc_send_reply_n2(car_array, ret*sizeof(struct car_t), ret, phdr->sender_id,
                                MODULE_ID_QOS, IPC_TYPE_QOSIF, phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_REPLY);
	#endif
    }
    else
    {
    #if 0
        ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_QOS, IPC_TYPE_QOSIF,
                                phdr->msg_subtype, phdr->msg_index);
	#else
		ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, MODULE_ID_QOS, IPC_TYPE_QOSIF, 
								phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_NACK);	
	#endif
    }

    return ret;
}


static int qos_msg_reply_qosif_bulk ( struct ipc_msghdr_n *phdr )
{
	int data_num = IPC_MSG_LEN/sizeof(struct qos_if_snmp);
    struct qos_if_snmp qosif_array[data_num];
    uint32_t ifindex = 0;
    int ret = 0;

    if ( NULL == phdr )
    {
    	QOS_LOG_DBG( "Input param check failed!\n" );
        return ERRNO_PARAM_ILLEGAL;
    }

    ifindex = phdr->msg_index;
    memset ( qosif_array, 0, data_num * sizeof ( struct qos_if_snmp ) );

	ret = qos_if_get_bulk(ifindex, qosif_array);
    if (ret > 0)
    {
    #if 0
        ret = ipc_send_reply_bulk(qosif_array, ret*sizeof(struct qos_if_snmp), ret, phdr->sender_id,
                                MODULE_ID_QOS, IPC_TYPE_QOSIF, phdr->msg_subtype, phdr->msg_index);
	#else
		ret = ipc_send_reply_n2(qosif_array, ret*sizeof(struct qos_if_snmp), ret, phdr->sender_id,
                                MODULE_ID_QOS, IPC_TYPE_QOSIF, phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_REPLY);
	#endif
    }
    else
    {
    #if 0
        ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_QOS, IPC_TYPE_QOSIF,
                                phdr->msg_subtype, phdr->msg_index);
    #else
		ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, MODULE_ID_QOS, IPC_TYPE_QOSIF, 
								phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_NACK);	
    #endif
    }

    return ret;
}


static int qos_msg_rcv_qos(struct ipc_mesg_n* mesg)
{
	struct ipc_msghdr_n *phdr = NULL;
    enum QOS_TYPE type;
    struct qos_domain *pdomain = NULL;
    struct qos_phb *pphb = NULL;
    struct car_t *pcar = NULL;
	void *pdata = NULL;
    uint32_t exist = 0;
	uint8_t id = 0;
    int ret = 0;

	phdr = & ( mesg->msghdr );
	pdata = mesg->msg_data;

    if(IPC_OPCODE_GET == phdr->opcode)
    {
    	struct ipc_mesg_n *pMsgRep = NULL;
    	int msgRepLen = sizeof(struct ipc_msghdr_n) + sizeof(uint32_t);

    	pMsgRep = mem_share_malloc(msgRepLen, MODULE_ID_QOS);
	    if(pMsgRep == NULL)
	    {
	    	QOS_LOG_ERR("malloc memory fail.\n");
	    	return ERRNO_MALLOC;
	    }

	    memset(pMsgRep, 0, msgRepLen);

        if(QOS_INFO_DOMAIN == phdr->msg_subtype)
        {
            id = (uint8_t)phdr->msg_index;
            type = *(enum QOS_TYPE *)pdata;
            pdomain = qos_domain_get(id, type);

            exist = (NULL == pdomain) ? 0 : 1;
            pMsgRep->msghdr.msg_subtype = QOS_INFO_DOMAIN;
        }
        else if(QOS_INFO_PHB == phdr->msg_subtype)
        {
            id = (uint8_t)phdr->msg_index;
            type = *(enum QOS_TYPE *)pdata;
            pphb = qos_phb_get(id, type);

            exist = (NULL == pphb) ? 0 : 1;
            pMsgRep->msghdr.msg_subtype = QOS_INFO_PHB;
        }
        else if(QOS_INFO_CAR == phdr->msg_subtype)
        {
        	pcar = qos_car_profile_lookup(phdr->msg_index);

        	exist = (NULL == pcar) ? 0 : 1;
        	pMsgRep->msghdr.msg_subtype = QOS_INFO_CAR;
        }
        
		pMsgRep->msghdr.msg_index   = (uint8_t)phdr->msg_index;
	    pMsgRep->msghdr.data_len    = sizeof(uint32_t);
	    pMsgRep->msghdr.opcode      = IPC_OPCODE_REPLY;
	    pMsgRep->msghdr.msg_index   = phdr->msg_index;
	    pMsgRep->msghdr.data_num    = 1;
		memcpy(pMsgRep->msg_data, &exist, sizeof(uint32_t));
		
	    if(ipc_send_reply_n1(phdr, pMsgRep, msgRepLen) == -1)
	    {
	        mem_share_free(pMsgRep, MODULE_ID_QOS);
	        return ERRNO_FAIL;
	    }
    }
    else if(IPC_OPCODE_ADD == phdr->opcode)
    {
    	if(QOS_INFO_DOMAIN == phdr->msg_subtype)
        {
            id = (uint8_t)phdr->msg_index;
            type = *(enum QOS_TYPE *)pdata;
            pdomain = qos_domain_get(id, type);

            if (pdomain)
            {
                pdomain->ref_cnt++;
            }
        }
        else if(QOS_INFO_PHB == phdr->msg_subtype)
        {
            id = (uint8_t)phdr->msg_index;
            type = *(enum QOS_TYPE *)pdata;
            pphb = qos_phb_get(id, type);

            if (pphb)
            {
                pphb->ref_cnt++;
            }
        }
        else if(QOS_INFO_CAR == phdr->msg_subtype)
        {
        	pcar = qos_car_profile_lookup(phdr->msg_index);

            if (pcar)
            {
                qos_car_profile_attached(pcar, 1);
            }
        }
    }
    else if(IPC_OPCODE_DELETE == phdr->opcode)
    {
    	if(QOS_INFO_DOMAIN == phdr->msg_subtype)
        {
            id = (uint8_t)phdr->msg_index;
            type = *(enum QOS_TYPE *)pdata;
            pdomain = qos_domain_get(id, type);

            if (pdomain)
            {
                pdomain->ref_cnt--;
            }
        }
        else if(QOS_INFO_PHB == phdr->msg_subtype)
        {
            id = (uint8_t)phdr->msg_index;
            type = *(enum QOS_TYPE *)pdata;
            pphb = qos_phb_get(id, type);

            if (pphb)
            {
                pphb->ref_cnt--;
            }
        }
        else if(QOS_INFO_CAR == phdr->msg_subtype)
        {
        	pcar = qos_car_profile_lookup(phdr->msg_index);

            if (pcar)
            {
                qos_car_profile_attached(pcar, 0);
            }
        }
    }
    else if(IPC_OPCODE_GET_BULK == phdr->opcode)
    {
    	if (QOS_INFO_DOMAIN == phdr->msg_subtype)
    	{
    		type = *(enum QOS_TYPE *)pdata;

    		ret = qos_msg_reply_domain_bulk( phdr, type );
		    if ( ret )
		    {
		        QOS_LOG_ERR ( "qos_msg_reply_domain_bulk failed, ret %d\n", ret );
		        return ret;
		    }
    	}
    	else if (QOS_INFO_PHB == phdr->msg_subtype)
    	{
    		type = *(enum QOS_TYPE *)pdata;

    		ret = qos_msg_reply_phb_bulk( phdr, type );
		    if ( ret )
		    {
		        QOS_LOG_ERR ( "qos_msg_reply_phb_bulk failed, ret %d\n", ret );
		        return ret;
		    }
    	}
    }

    return ERRNO_SUCCESS;
}


static int qos_msg_rcv_acl(struct ipc_mesg_n* mesg)
{
	struct qos_policy *policy = NULL;
	struct qos_if_mirror *pif_mirror = NULL;
	struct qos_mirror_snmp pmirror;
	struct ipc_msghdr_n *phdr = NULL;
	void *pdata = NULL;
	uint16_t ruleid = 0;
	int ret;

	phdr = & ( mesg->msghdr );
	pdata = (void *)(mesg->msg_data);

	if(IPC_OPCODE_GET_BULK == phdr->opcode)
	{
		if (QOS_INFO_ACL_RULE == phdr->msg_subtype)
		{
			ruleid = *(uint16_t *)pdata;
			ret = qos_msg_reply_acl_rule_bulk( phdr,  ruleid);
			if ( ret )
			{
				QOS_LOG_ERR ( "qos_msg_reply_acl_group_bulk failed, ret %d\n", ret );
				return ret;
			}
		}
		else if (QOS_INFO_POLICY == phdr->msg_subtype)
		{
			policy = (struct qos_policy *)pdata;	  
			ret = qos_msg_reply_policy_bulk( phdr, policy);
			if ( ret )
			{
				QOS_LOG_ERR ( "qos_msg_reply_policy_bulk failed, ret %d\n", ret );
				return ret;
			}
		}
		else if (QOS_INFO_MIRROR_GROUP == phdr->msg_subtype)
		{
			pmirror.id = *(uint32_t *)pdata;
			pmirror.ifindex = mesg->msghdr.msg_index;
			ret = qos_msg_reply_mirror_group_bulk( phdr, &pmirror);
			if ( ret )
			{
				QOS_LOG_ERR ( "qos_msg_reply_mirror_group_bulk failed, ret %d\n", ret );
				return ret;
			}
		}
		else if (QOS_INFO_MIRROR_GROUP_PORT == phdr->msg_subtype)
		{
			pif_mirror = (struct qos_if_mirror*)pdata;
			ret = qos_msg_reply_mirror_port_bulk( phdr, pif_mirror);
			if ( ret )
			{
				QOS_LOG_ERR ( "qos_msg_reply_mirror_port_bulk failed, ret %d\n", ret );
				return ret;
			}
		}
	}

	return ERRNO_SUCCESS;
}





static int qos_msg_rcv_car(struct ipc_mesg_n* mesg)
{
	struct ipc_msghdr_n *phdr = NULL;
    int ret;

	phdr = & ( mesg->msghdr );

    if(IPC_OPCODE_GET_BULK == phdr->opcode)
    {
        ret = qos_msg_reply_car_profile_bulk( phdr );
	    if ( ret )
	    {
	        QOS_LOG_ERR ( "qos_msg_reply_car_profile_bulk failed, ret %d\n", ret );
	        return ret;
	    }
    }

    return ERRNO_SUCCESS;
}


static int qos_msg_rcv_ifm(struct ipc_mesg_n* mesg)
{
	struct ipc_msghdr_n *phdr = NULL;
	struct ifm_event *pevent = NULL;
	uint32_t ifindex;

	phdr = & ( mesg->msghdr );

	if (phdr->opcode == IPC_OPCODE_EVENT)
    {
        pevent = (struct ifm_event *)mesg->msg_data;
        ifindex = pevent->ifindex;

        if( IFNET_EVENT_IF_DELETE == phdr->msg_subtype)
        {
            QOS_LOG_DBG ( "recv event notify ifindex 0x%0x delete\n", ifindex );
			qos_if_delete_config(ifindex);
            qos_if_delete(ifindex);
            sla_if_delete(ifindex);
        }
        else if( IFNET_EVENT_MODE_CHANGE == phdr->msg_subtype)
		{
			QOS_LOG_DBG ( "recv event mode change ifindex 0x%0x mode change %d\n", ifindex, pevent->mode);

			/*接口加入trunk成员，删除对应的qos 配置*/
			// 2018-05-04, by caojt, add IFNET_MODE_PHYSICAL for L1 ethernet port
			if (IFNET_MODE_INVALID == pevent->mode || IFNET_MODE_PHYSICAL == pevent->mode)
			{
				qos_if_delete_config(ifindex);
				qos_if_delete(ifindex);
				sla_if_delete(ifindex);
			}
			else
			{
				qos_if_process_mode_change(ifindex);
				sla_if_delete(ifindex);
			}
		}
    }

	return ERRNO_SUCCESS;
}



static int qos_msg_rcv_sla(struct ipc_mesg_n* mesg)
{
	struct ipc_msghdr_n *phdr = NULL;
	struct sla_session *data = NULL, *sess = NULL;
    void *pdata = NULL;

	phdr = & ( mesg->msghdr );

	if (phdr->opcode == IPC_OPCODE_EVENT)
    {
		data = (struct sla_session *)mesg->msg_data;
		if ((sess = sla_session_lookup(data->info.session_id)) == NULL)
		{
			return ERRNO_FAIL;
		}
		if(sess->info.state == SLA_STATUS_DISABLE || sla_entry.state == SLA_STATUS_DISABLE)
		{
			return ERRNO_FAIL;
		}

		sla_session_finish(sess, &data->result);

	}
    else if(phdr->opcode == IPC_OPCODE_GET_BULK)
    {
        pdata = mesg->msg_data;
        sla_msg_rcv_get_bulk_session(phdr, pdata);
    }
	else if(phdr->opcode == IPC_OPCODE_GET)
    {
        if(phdr->msg_subtype == SLA_SUBTYPE_SESS_IF_INFO)
        {
        #if 0
            ipc_send_reply(&sla_entry, sizeof(struct sla_if_entry), phdr->sender_id, MODULE_ID_QOS,
                                            IPC_TYPE_SLA, phdr->msg_subtype, phdr->msg_index);
		#else
			ipc_send_reply_n2(&sla_entry, sizeof(struct sla_if_entry), 1, phdr->sender_id, MODULE_ID_QOS,
                                            IPC_TYPE_SLA, phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_REPLY);
		#endif
        }
    }

	return ERRNO_SUCCESS;
}

static int qos_msg_rcv_hqos(struct ipc_mesg_n* mesg)
{
	struct ipc_msghdr_n *phdr = NULL;
	uint16_t hqos_id = 0;
    struct queue_profile *pqueue = NULL;
    struct hqos_t *phqos = NULL;
	uint32_t exist = 0;
	uint8_t id = 0;
	int size;
	int ret;

	phdr = & ( mesg->msghdr );

	if (phdr->opcode == IPC_OPCODE_GET)
    {
		if (HQOS_SUBTYPE_QUEUE_PROFILE == phdr->msg_subtype)
        {
            id = (uint8_t)phdr->msg_index;
            pqueue = hqos_queue_profile_lookup(id);

            exist = (NULL == pqueue) ? 0 : 1;
            #if 0
            ret = ipc_send_reply ( &exist, sizeof(uint32_t), phdr->sender_id, MODULE_ID_QOS,
                       IPC_TYPE_HQOS, HQOS_SUBTYPE_QUEUE_PROFILE, id );
            #else
			ret = ipc_send_reply_n2( &exist, sizeof(uint32_t), 1, phdr->sender_id, MODULE_ID_QOS,
                       IPC_TYPE_HQOS, HQOS_SUBTYPE_QUEUE_PROFILE, 0, id , IPC_OPCODE_REPLY);
			#endif
            if ( ret )
            {
                QOS_LOG_ERR ( "ipc_send_reply hqos queue profile exist error. ret=%d", ret );
                return ERRNO_FAIL;
            }
        }
        else if (HQOS_SUBTYPE_ID == phdr->msg_subtype)
        {
            hqos_id = (uint16_t)phdr->msg_index;

            phqos = hqos_table_lookup(hqos_id);

            if (NULL == phqos)
            {
            	size = 0;
            }
            else
            {
            	size = sizeof(struct hqos_t);
            }
			#if 0
        	ret = ipc_send_reply ( phqos, size, phdr->sender_id, MODULE_ID_QOS,
                       IPC_TYPE_HQOS, HQOS_SUBTYPE_ID, hqos_id );
            #else
            ret = ipc_send_reply_n2( phqos, size, 1, phdr->sender_id, MODULE_ID_QOS,
                       IPC_TYPE_HQOS, HQOS_SUBTYPE_ID, 0, hqos_id , IPC_OPCODE_REPLY);
            #endif
            if ( ret )
            {
                QOS_LOG_ERR ( "ipc_send_reply hqos id error. ret=%d", ret );
                return ERRNO_FAIL;
            }
        }
	}
	else if(IPC_OPCODE_ADD == phdr->opcode)
    {
    	if (HQOS_SUBTYPE_QUEUE_PROFILE == phdr->msg_subtype)
        {
            id = (uint8_t)phdr->msg_index;
            pqueue = hqos_queue_profile_lookup(id);

            hqos_queue_profile_attached(pqueue, 1);
        }
        else if (HQOS_SUBTYPE_ID == phdr->msg_subtype)
        {
            hqos_id = index_alloc(INDEX_TYPE_HQOS);

            phqos = (struct hqos_t *)mesg->msg_data;
            phqos->id = hqos_id;

            ret = hqos_table_create(phqos);
            if (ret)
            {
            	QOS_LOG_ERR ( "hqos_table_create error. ret=%d", ret );
            	index_free(INDEX_TYPE_HQOS, hqos_id);
            	hqos_id = 0;
            }
			#if 0
        	ret = ipc_send_reply ( &hqos_id, sizeof(uint16_t), phdr->sender_id, MODULE_ID_QOS,
                       IPC_TYPE_HQOS, HQOS_SUBTYPE_ID, 0 );
            #else
            ret = ipc_send_reply_n2( &hqos_id, sizeof(uint16_t), 1, phdr->sender_id, MODULE_ID_QOS,
                       IPC_TYPE_HQOS, HQOS_SUBTYPE_ID, 0, 0 , IPC_OPCODE_REPLY);
            #endif
            if ( ret )
            {
                QOS_LOG_ERR ( "ipc_send_reply hqos id error. ret=%d", ret );
                return ERRNO_FAIL;
            }

            if (phqos->hqos_id)
            {
            	pqueue = hqos_queue_profile_lookup(phqos->hqos_id);

            	hqos_queue_profile_attached(pqueue, 1);
            }
        }
    }
    else if (IPC_OPCODE_DELETE == phdr->opcode)
    {
    	if (HQOS_SUBTYPE_QUEUE_PROFILE == phdr->msg_subtype)
        {
            id = (uint8_t)phdr->msg_index;
            pqueue = hqos_queue_profile_lookup(id);

            hqos_queue_profile_attached(pqueue, 0);
        }
        else if (HQOS_SUBTYPE_ID == phdr->msg_subtype)
        {
            hqos_id = (uint16_t)phdr->msg_index;

            hqos_table_delete(hqos_id);
        }
    }
    else if(IPC_OPCODE_GET_BULK == phdr->opcode)
    {
    	if (HQOS_SUBTYPE_WRED_PROFILE == phdr->msg_subtype)
    	{
    		ret = qos_msg_reply_wred_profile_bulk( phdr );
		    if ( ret )
		    {
		        QOS_LOG_ERR ( "qos_msg_reply_wred_profile_bulk failed, ret %d\n", ret );
		        return ret;
		    }
    	}
    	else if (HQOS_SUBTYPE_QUEUE_PROFILE == phdr->msg_subtype)
    	{
    		ret = qos_msg_reply_queue_profile_bulk( phdr );
		    if ( ret )
		    {
		        QOS_LOG_ERR ( "qos_msg_reply_queue_profile_bulk failed, ret %d\n", ret );
		        return ret;
		    }
    	}
    }

	return ERRNO_SUCCESS;
}


/* qos if消息处理 */
static int qos_msg_rcv_qosif(struct ipc_mesg_n* mesg)
{
	struct ipc_msghdr_n *phdr = NULL;
	void *pdata = NULL;
	enum QOS_DIR dir;
	int ret = 0;

	phdr = & ( mesg->msghdr );
	pdata = mesg->msg_data;

	if (phdr->opcode == IPC_OPCODE_GET_BULK)
    {
		if (CAR_INFO_CARID == phdr->msg_subtype)
        {
        	dir = *(enum QOS_DIR *)pdata;

            ret = qos_msg_reply_car_apply_bulk( phdr, dir );
		    if ( ret )
		    {
		        QOS_LOG_ERR ( "qos_msg_reply_car_apply_bulk failed, ret %d\n", ret );
		        return ret;
		    }
        }
        else if (CAR_INFO_CIR == phdr->msg_subtype)
        {
        	dir = *(enum QOS_DIR *)pdata;

            ret = qos_msg_reply_car_param_bulk( phdr, dir );
		    if ( ret )
		    {
		        QOS_LOG_ERR ( "qos_msg_reply_car_param_bulk failed, ret %d\n", ret );
		        return ret;
		    }
        }
        else if (QOS_INFO_MAX == phdr->msg_subtype)
        {
            ret = qos_msg_reply_qosif_bulk( phdr );
		    if ( ret )
		    {
		        QOS_LOG_ERR ( "qos_msg_reply_qosif_bulk failed, ret %d\n", ret );
		        return ret;
		    }
        }
	}

	return ERRNO_SUCCESS;
}


/* qos消息处理 */
#if 0
int qos_msg_rcv(struct thread *pthread)
{
    static struct ipc_mesg mesg;
    struct ipc_msghdr *phdr = NULL;
    int ret = 0;
    int token = 100;

    while ( token )
    {
        if ( -1 == ipc_recv_common ( &mesg, MODULE_ID_QOS ) )
        {
            goto out;
        }

        token--;
        phdr = & ( mesg.msghdr );

        QOS_LOG_DBG ( "opcode=%d msg_type=%d sender_id=%d msg_subtype=%d phdr->msg_index=0x%0x\n",
                     phdr->opcode, phdr->msg_type, phdr->sender_id, phdr->msg_subtype, phdr->msg_index );

        if ( IPC_TYPE_QOS == phdr->msg_type )
        {
            ret = qos_msg_rcv_qos(&mesg);
            if ( ret )
            {
                QOS_LOG_ERR ( "qos_msg_rcv_qos error. ret=%d", ret );
                goto out;
            }
        }
        else if(IPC_TYPE_CAR == phdr->msg_type)
	    {
	        ret = qos_msg_rcv_car(&mesg);
            if ( ret )
            {
                QOS_LOG_ERR ( "qos_msg_rcv_car error. ret=%d", ret );
                goto out;
            }
	    }
        else if(IPC_TYPE_IFM == phdr->msg_type)
	    {
	        ret = qos_msg_rcv_ifm(&mesg);
            if ( ret )
            {
                QOS_LOG_ERR ( "qos_msg_rcv_ifm error. ret=%d", ret );
                goto out;
            }
	    }
        else if(IPC_TYPE_SLA == phdr->msg_type)
        {
            ret = qos_msg_rcv_sla(&mesg);
            if ( ret )
            {
                QOS_LOG_ERR ( "qos_msg_rcv_sla error. ret=%d", ret );
                goto out;
            }
		}
		else if(IPC_TYPE_HQOS == phdr->msg_type)
        {
            ret = qos_msg_rcv_hqos(&mesg);
            if ( ret )
            {
                QOS_LOG_ERR ( "qos_msg_rcv_hqos error. ret=%d", ret );
                goto out;
            }
		}
		else if(IPC_TYPE_ACL == phdr->msg_type)
        {
            ret = qos_msg_rcv_acl(&mesg);
            if ( ret )
            {
                QOS_LOG_ERR ( "qos_msg_rcv_hqos error. ret=%d", ret );
                goto out;
            }
		}
		else if(IPC_TYPE_QOSIF == phdr->msg_type)
		{
			ret = qos_msg_rcv_qosif(&mesg);
			if ( ret )
            {
                QOS_LOG_ERR ( "qos_msg_rcv_qosif error. ret=%d", ret );
                goto out;
            }
		}
    }

out:
    usleep ( 1000 ); //?? CPU
    thread_add_event ( qos_master, qos_msg_rcv, NULL, 0 );

	return ERRNO_SUCCESS;
}

#else
int qos_msg_rcv(struct ipc_mesg_n *pmsg, int imlen)
{
    int ret = 0;
    int revln = 0;

    if (NULL == pmsg)
    {
    	return -1;
    }

    QOS_LOG_DBG("qos recv msg: pmsg=%p, imlen=%d\r\n", pmsg, imlen);

    revln = (int)pmsg->msghdr.data_len + IPC_HEADER_LEN_N; 

    if(revln <= imlen)
    {
    	switch(pmsg->msghdr.msg_type)
    	{
	    	case IPC_TYPE_QOS:
	        {
	        	//opt end
	            ret = qos_msg_rcv_qos(pmsg);
	            if ( ret )
	            {
	                QOS_LOG_ERR ( "qos_msg_rcv_qos error. ret=%d", ret );
	            }
	            break;
	        }
	        case IPC_TYPE_CAR:
		    {
		    	//opt end
		        ret = qos_msg_rcv_car(pmsg);
	            if ( ret )
	            {
	                QOS_LOG_ERR ( "qos_msg_rcv_car error. ret=%d", ret );
	            }
	            break;
		    }
		    case IPC_TYPE_IFM:
		    {
		    	//opt end
		        ret = qos_msg_rcv_ifm(pmsg);
	            if ( ret )
	            {
	                QOS_LOG_ERR ( "qos_msg_rcv_ifm error. ret=%d", ret );
	            }
	            break;
		    }
		    case IPC_TYPE_SLA:
	        {
	        	//opt end
	            ret = qos_msg_rcv_sla(pmsg);
	            if ( ret )
	            {
	                QOS_LOG_ERR ( "qos_msg_rcv_sla error. ret=%d", ret );
	            }
	            break;
			}
			case IPC_TYPE_HQOS:
	        {
	        	//opt end
	            ret = qos_msg_rcv_hqos(pmsg);
	            if ( ret )
	            {
	                QOS_LOG_ERR ( "qos_msg_rcv_hqos error. ret=%d", ret );
	            }
	            break;
			}
			case IPC_TYPE_ACL:
	        {
	        	//opt end
	            ret = qos_msg_rcv_acl(pmsg);
	            if ( ret )
	            {
	                QOS_LOG_ERR ( "qos_msg_rcv_hqos error. ret=%d", ret );
	            }
	            break;
			}
			case IPC_TYPE_QOSIF:
			{
				//opt end
				ret = qos_msg_rcv_qosif(pmsg);
				if ( ret )
	            {
	                QOS_LOG_ERR ( "qos_msg_rcv_qosif error. ret=%d", ret );
	            }
	            break;
			}
			default:
			{
				QOS_LOG_ERR ( "qos_msg_rcv_qosif error. ret=%d", ret );
				break;
			}
    	}
    }

    mem_share_free(pmsg, MODULE_ID_QOS);
    
	return ERRNO_SUCCESS;
}

#endif

int qos_ipc_send_and_wait_ack(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
	int ret = 0;
	struct ipc_mesg_n* pMsgRep = NULL;
	
	pMsgRep = ipc_sync_send_n2( pdata , data_len, data_num, module_id, sender_id,
								msg_type, subtype, opcode, msg_index, 0);
	if (NULL == pMsgRep)
    {
    	QOS_LOG_ERR ( "send sync message fail.\n" );
        return ERRNO_MALLOC;
    }

	if (IPC_OPCODE_ACK == pMsgRep->msghdr.opcode) {
		ret = 0;
	}
    else if (IPC_OPCODE_NACK == pMsgRep->msghdr.opcode){
		memcpy(&ret, pMsgRep->msg_data, sizeof(ret));
	}
	else{
		ret = -1;
	}
	
    mem_share_free(pMsgRep, MODULE_ID_QOS);
    return ret;
}




