

#include <string.h>
#include <stdlib.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/command.h>
#include <lib/prefix.h>
#include <lib/log.h>
#include <lib/vty.h>
#include <lib/zassert.h>
#include <lib/ifm_common.h>
#include <lib/errcode.h>
#include <lib/msg_ipc_n.h>
#include "qos_msg.h"
#include "qos_main.h"
#include "qos_if.h"
#include "qos_car.h"
#include "qos_policy.h"
#include "acl.h"
#include "qos_mirror.h"


struct list qos_policy_global;


void qos_policy_global_init()
{
	memset(&qos_policy_global ,0,sizeof(struct list));
}

int qos_policy_add_list(uint32_t acl_num, enum POLICY_TYPE type, enum QOS_DIR direct,uint32_t ifindex, struct acl_action *paction)
{
	struct qos_policy *qos_policy = NULL;
	int ret = 0;

	qos_policy = (struct qos_policy *)XMALLOC(MTYPE_QOS_ENTRY, sizeof(struct qos_policy));
	if(NULL == qos_policy)
	{
		QOS_LOG_ERR("The memory is insufficient.\n");
		return ERRNO_FAIL;
	}

	memset(qos_policy,0,sizeof(struct qos_policy));
	memcpy(&qos_policy->action ,paction,sizeof(struct acl_action));
	qos_policy->type = type;
	qos_policy->direct = direct;
	qos_policy->acl_num = acl_num;
	qos_policy->ifindex = ifindex;
#if 0
	ret = ipc_send_hal_wait_ack(qos_policy , sizeof(struct qos_policy),1, MODULE_ID_HAL,
						MODULE_ID_QOS, IPC_TYPE_ACL, QOS_INFO_POLICY, IPC_OPCODE_ADD, 0);
#else
	ret = qos_ipc_send_and_wait_ack(qos_policy , sizeof(struct qos_policy),1, MODULE_ID_HAL,
						MODULE_ID_QOS, IPC_TYPE_ACL, QOS_INFO_POLICY, IPC_OPCODE_ADD, 0);
#endif
	if(ret)
    {
    	XFREE(MTYPE_QOS_ENTRY, qos_policy);
		QOS_LOG_ERR("ipc_send_hal_wait_ack failed.\n");
		return ERRNO_HAL;
    }

	listnode_add (&qos_policy_global,(void *)qos_policy);

	return ERRNO_SUCCESS;
}


int qos_policy_add(uint32_t acl_num, enum POLICY_TYPE type,enum QOS_DIR direct, uint32_t ifindex, struct acl_action *paction)
{
	struct qos_policy *qos_policy = NULL;
	struct listnode *node = NULL;
	int ret;

	if (list_isempty (&qos_policy_global))
	{
		ret = qos_policy_add_list(acl_num, type, direct, ifindex, paction);
		return ret;
	}

	for (node = qos_policy_global.head ; node; node = node->next)
	{
		qos_policy = listgetdata(node);
		if((qos_policy->acl_num == acl_num)&&(qos_policy->type == type)&&(qos_policy->direct == direct))
		{
			QOS_LOG_ERR("this policy existed.\n");
			return ERRNO_EXISTED;
		}
	}

	if(NULL == node)
	{
		ret = qos_policy_add_list(acl_num, type, direct, ifindex, paction);
		return ret;
	}

    return ERRNO_SUCCESS;
}


int qos_policy_delete(uint32_t acl_num, enum POLICY_TYPE type,enum QOS_DIR direct, uint32_t ifindex)
{
	struct qos_policy *qos_policy = NULL;
	struct listnode *node = NULL;
	struct car_t *acl_car = NULL;
	int ret = 0;

	if (list_isempty (&qos_policy_global))
	{
		QOS_LOG_ERR("can not find this policy.\n");
		return ERRNO_NOT_FOUND;
	}

	for (node = qos_policy_global.head ; node; node = node->next)
	{
		qos_policy = listgetdata(node);
		if((qos_policy->acl_num == acl_num)&&(qos_policy->type == type)&&(qos_policy->direct == direct))
		{
			if(type == POLICY_TYPE_CAR)
			{
				/*car 模板引用计数减一*/
				acl_car = qos_car_profile_lookup(qos_policy->action.car_id);
				if(NULL == acl_car)
				{
					QOS_LOG_ERR("qos_car_profile_lookup error.\n");
					return ERRNO_FAIL;
				}

				acl_car->ref_cnt--;
			}
#if 0
			ret =ipc_send_hal(qos_policy , sizeof(struct qos_policy),1, MODULE_ID_HAL, MODULE_ID_QOS,
							IPC_TYPE_ACL, QOS_INFO_POLICY, IPC_OPCODE_DELETE, 0);
#else
			ret = ipc_send_msg_n2(qos_policy , sizeof(struct qos_policy),1, MODULE_ID_HAL, MODULE_ID_QOS,
							IPC_TYPE_ACL, QOS_INFO_POLICY, IPC_OPCODE_DELETE, 0);
#endif
		    if(ret)
		    {
		    	QOS_LOG_ERR("Failed to send global qos policy to hal.\n");
				return ERRNO_IPC;
		    }

			listnode_delete (&qos_policy_global, qos_policy);
			XFREE(MTYPE_QOS_ENTRY, qos_policy);
			qos_policy = NULL;

			return ERRNO_SUCCESS;
		}
	}

	return ERRNO_NOT_FOUND;
}


struct qos_policy *qos_policy_lookup(uint32_t acl_num,enum POLICY_TYPE type,enum QOS_DIR direct, uint32_t ifindex)
{
	struct qos_policy *qos_policy = NULL;
	struct listnode *node = NULL;

	if (list_isempty (&qos_policy_global))
	{
		QOS_LOG_ERR("can not find this policy.\n");
		return NULL;
	}

	for (node = qos_policy_global.head ; node; node = node->next)
	{
		qos_policy = listgetdata(node);
		if((qos_policy->acl_num == acl_num)&&(qos_policy->type == type)&&(qos_policy->direct== direct))
		{
			return qos_policy;
		}
	}

	return NULL;
}



/* 接口配置 policy 的操作 */
int qos_policy_add_if(uint32_t acl_num,enum POLICY_TYPE type,enum QOS_DIR direct, uint32_t ifindex, struct acl_action *paction)
{
	struct qos_if *qos_if = NULL;
	struct qos_policy *qos_policy = NULL;
	struct listnode *node = NULL;
	int ret = 0;

	qos_if = qos_if_lookup(ifindex);
	if(NULL == qos_if)
	{
		qos_if = qos_if_create(ifindex);
		if(NULL == qos_if)
		{
			QOS_LOG_ERR("qos_if_create error.\n");
			return ERRNO_FAIL;
		}

		qos_policy = (struct qos_policy *)XMALLOC(MTYPE_QOS_ENTRY, sizeof(struct qos_policy));
		if(NULL == qos_policy)
		{
			QOS_LOG_ERR("The memory is insufficient.\n");
			return ERRNO_FAIL;
		}

		memset(qos_policy,0,sizeof(struct qos_policy));
		memcpy(&qos_policy->action ,paction,sizeof(struct acl_action));
		qos_policy->type = type;
		qos_policy->direct = direct;
		qos_policy->acl_num = acl_num;
		qos_policy->ifindex = ifindex;
#if 0
		ret = ipc_send_hal_wait_ack(qos_policy , sizeof(struct qos_policy),1, MODULE_ID_HAL,
						MODULE_ID_QOS, IPC_TYPE_ACL, QOS_INFO_POLICY, IPC_OPCODE_ADD,ifindex);
#else
		ret = qos_ipc_send_and_wait_ack(qos_policy , sizeof(struct qos_policy),1, MODULE_ID_HAL,
						MODULE_ID_QOS, IPC_TYPE_ACL, QOS_INFO_POLICY, IPC_OPCODE_ADD,ifindex);
#endif
		if(ret)
	    {
	    	XFREE(MTYPE_QOS_ENTRY, qos_policy);
			qos_policy = NULL;
			QOS_LOG_ERR("ipc_send_hal_wait_ack error.\n");
			return ERRNO_HAL;
	    }

		qos_if->pqos_policy = list_new();

		listnode_add (qos_if->pqos_policy,(void *)qos_policy);
		return ERRNO_SUCCESS;
	}

	if((paction->car == 1)&&(qos_if->car_id[direct-1] != 0))
	{
		QOS_LOG_DBG("Interface already configured with qos car.\n");
		return QOS_ERR_CONFLICT;
	}

	if(NULL == qos_if->pqos_policy)
	{
		qos_policy = (struct qos_policy *)XMALLOC(MTYPE_QOS_ENTRY, sizeof(struct qos_policy));
		if(NULL == qos_policy)
		{
			QOS_LOG_ERR("qos_if_create error.\n");
			return ERRNO_FAIL;
		}

		memset(qos_policy,0,sizeof(struct qos_policy));
		memcpy(&qos_policy->action ,paction,sizeof(struct acl_action));
		qos_policy->type = type;
		qos_policy->direct = direct;
		qos_policy->acl_num = acl_num;
		qos_policy->ifindex = ifindex;
#if 0
		ret = ipc_send_hal_wait_ack(qos_policy , sizeof(struct qos_policy), 1, MODULE_ID_HAL,
						MODULE_ID_QOS, IPC_TYPE_ACL, QOS_INFO_POLICY, IPC_OPCODE_ADD,ifindex);
#else
		ret = qos_ipc_send_and_wait_ack(qos_policy , sizeof(struct qos_policy), 1, MODULE_ID_HAL,
						MODULE_ID_QOS, IPC_TYPE_ACL, QOS_INFO_POLICY, IPC_OPCODE_ADD,ifindex);
#endif
		if(ret)
	    {
	    	XFREE(MTYPE_QOS_ENTRY, qos_policy);
			qos_policy = NULL;
			QOS_LOG_ERR("ipc_send_hal_wait_ack error.\n");
			return ERRNO_HAL;
	    }

		qos_if->ifindex = ifindex;
		qos_if->pqos_policy = list_new();
		qos_if->pqos_policy->head = NULL;
		qos_if->pqos_policy->tail = NULL;
		qos_if->pqos_policy->count = 0;

		listnode_add (qos_if->pqos_policy,(void *)qos_policy);
		return ERRNO_SUCCESS;
	}
	else
	{
		for (node = qos_if->pqos_policy->head ; node; node = node->next)
		{
			qos_policy = listgetdata(node);
			if((qos_policy->acl_num == acl_num)&&(qos_policy->type == type)&&(qos_policy->direct== direct))
			{
				QOS_LOG_ERR("this policy existed.\n");
				return ERRNO_EXISTED;
			}
		}
		qos_policy = (struct qos_policy *)XMALLOC(MTYPE_QOS_ENTRY, sizeof(struct qos_policy));
		if(NULL == qos_policy)
		{
			QOS_LOG_ERR("qos_if_create error.\n");
			return ERRNO_FAIL;
		}

		memset(qos_policy,0,sizeof(struct qos_policy));
		memcpy(&qos_policy->action ,paction,sizeof(struct acl_action));
		qos_policy->type = type;
		qos_policy->direct = direct;
		qos_policy->acl_num = acl_num;
		qos_policy->ifindex = ifindex;
#if 0
		ret = ipc_send_hal_wait_ack(qos_policy , sizeof(struct qos_policy),1, MODULE_ID_HAL, MODULE_ID_QOS,
						IPC_TYPE_ACL, QOS_INFO_POLICY, IPC_OPCODE_ADD,ifindex);
#else
		ret = qos_ipc_send_and_wait_ack(qos_policy , sizeof(struct qos_policy),1, MODULE_ID_HAL, MODULE_ID_QOS,
						IPC_TYPE_ACL, QOS_INFO_POLICY, IPC_OPCODE_ADD,ifindex);
#endif
		if(ret)
	    {
	    	XFREE(MTYPE_QOS_ENTRY, qos_policy);
			qos_policy = NULL;
			QOS_LOG_ERR("ipc_send_hal_wait_ack error.\n");
			return ERRNO_HAL;
	    }

		listnode_add (qos_if->pqos_policy,(void *)qos_policy);
		return ERRNO_SUCCESS;
	}
}



int qos_policy_delete_if(uint32_t acl_num, enum POLICY_TYPE type,enum QOS_DIR direct, uint32_t ifindex)
{
	struct qos_if *qos_if = NULL;
	struct qos_policy *qos_policy = NULL;
	struct listnode *node = NULL;
	struct car_t *acl_car = NULL;
	int ret = 0;

	qos_if = qos_if_lookup(ifindex);
	if(NULL == qos_if)
	{
		QOS_LOG_ERR("can not find this policy.\n");
		return ERRNO_NOT_FOUND;
	}
	if(NULL == qos_if->pqos_policy)
	{
		QOS_LOG_ERR("NULL == qos_if->pqos_policy.\n");
		return ERRNO_NOT_FOUND;
	}

	for (node = qos_if->pqos_policy->head ; node; node = node->next)
	{
		qos_policy = listgetdata(node);
		if((qos_policy->acl_num == acl_num)&&(qos_policy->type == type)&&(qos_policy->direct == direct))
		{
			if(type == POLICY_TYPE_CAR)
			{
				/*car 模板引用计数减一*/
				acl_car = qos_car_profile_lookup(qos_policy->action.car_id);
				if(NULL == acl_car)
				{
					QOS_LOG_ERR("qos_car_profile_lookup error.\n");
					return ERRNO_FAIL;
				}

				acl_car->ref_cnt--;
			}
#if 0
			ret =ipc_send_hal(qos_policy , sizeof(struct qos_policy),1, MODULE_ID_HAL, MODULE_ID_QOS,
											IPC_TYPE_ACL, QOS_INFO_POLICY, IPC_OPCODE_DELETE, ifindex);
#else
			ret = ipc_send_msg_n2(qos_policy , sizeof(struct qos_policy),1, MODULE_ID_HAL, MODULE_ID_QOS,
											IPC_TYPE_ACL, QOS_INFO_POLICY, IPC_OPCODE_DELETE, ifindex);
#endif
		    if(ret)
		    {
		    	QOS_LOG_ERR("Failed to send interface qos policy to hal.\n");
				return ERRNO_IPC;
		    }

		    XFREE ( MTYPE_QOS_ENTRY, node->data );
			list_delete_node (qos_if->pqos_policy, node);
			if (list_isempty (qos_if->pqos_policy))
			{
				list_free(qos_if->pqos_policy);
				qos_if->pqos_policy =NULL;
				QOS_LOG_DBG("policy list_free\n\n");
			}

			return ERRNO_SUCCESS;
		}
	}

	return ERRNO_NOT_FOUND;
}


struct qos_policy *qos_policy_lookup_if(uint32_t acl_num, enum POLICY_TYPE type,enum QOS_DIR direct, uint32_t ifindex)
{
	struct qos_if *qos_if = NULL;
	struct qos_policy *qos_policy = NULL;
	struct listnode *node = NULL;

	qos_if = qos_if_lookup(ifindex);
	if((NULL == qos_if)||((NULL != qos_if)&&(NULL == qos_if->pqos_policy)))
	{
		QOS_LOG_ERR("can not find this policy.\n");
		return NULL;
	}

	for (node = qos_if->pqos_policy->head ; node; node = node->next)
	{
		qos_policy = listgetdata(node);
		if((qos_policy->acl_num == acl_num)&&(qos_policy->type == type)&&(qos_policy->direct== direct))
		{
			return qos_policy;
		}
	}

	return NULL;
}

int qos_policy_get_bulk (uint32_t ifindex,enum POLICY_TYPE type,enum QOS_DIR direct, uint32_t acl_num,struct qos_policy *ppolicy )
{
	int msg_len = IPC_MSG_LEN/sizeof(struct qos_policy);
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
    struct qos_policy *pqos_policy = NULL;
	struct qos_policy *pif_policy = NULL;
	struct listnode  *if_node = NULL;
	struct listnode  *pnode = NULL;
	struct qos_if *qos_if = NULL;
	int cursor = 0;
	int flag = 0;
    int data_num = 0;
	int val = 0;

	struct list *plist = &qos_policy_global;

	/*首次获取传参*/
	if((ifindex ==0)&&(type == 0)&&(direct == 0)&&(acl_num == 0))
	{
		for (ALL_LIST_ELEMENTS_RO(plist, pnode, pqos_policy))
        {
			memcpy(&ppolicy[data_num++], pqos_policy, sizeof(struct qos_policy));
            if (data_num == msg_len)
            {
                return data_num;
            }
        }

		HASH_BUCKET_LOOP(pbucket, cursor, qos_if_table)
        {
            qos_if = (struct qos_if *)pbucket->data;
            if ((NULL != qos_if)&&(NULL != qos_if->pqos_policy))
            { 
				for (ALL_LIST_ELEMENTS_RO(qos_if->pqos_policy, if_node, pif_policy))
				{
					memcpy(&ppolicy[data_num++], pif_policy, sizeof(struct qos_policy));

					if (data_num == msg_len)
		            {
		                return data_num;
		            }
				}
            }
        }
	}
	else if((ifindex == 0)&&(acl_num != 0))
	{
		for (ALL_LIST_ELEMENTS_RO(plist, pnode, pqos_policy))
        {
        	if((flag == 0)&&(pqos_policy->ifindex == ifindex)&&(pqos_policy->direct == direct)
				&&(pqos_policy->type == type)&&(pqos_policy->acl_num == acl_num))
        	{
        		flag = 1;
				//break;
				continue;
			}

			if(flag == 1)
			{
				memcpy(&ppolicy[data_num++], pqos_policy, sizeof(struct qos_policy));
	            if (data_num == msg_len)
	            {
	                return data_num;
	            }
			}
        }

		HASH_BUCKET_LOOP(pbucket, cursor, qos_if_table)
        {
            qos_if = (struct qos_if *)pbucket->data;
            if((NULL != qos_if)&&(NULL != qos_if->pqos_policy))
            {
                for (ALL_LIST_ELEMENTS_RO(qos_if->pqos_policy, if_node, pif_policy))
				{
					memcpy(&ppolicy[data_num++], pif_policy, sizeof(struct qos_policy));

					if (data_num == msg_len)
		            {
		                return data_num;
		            }
				}
            }
        }
	}
	else if((ifindex !=0)&&(acl_num != 0))
	{
		pbucket = hios_hash_find(&qos_if_table, (void *)ifindex);

        if (NULL == pbucket)
        {
        	val = qos_if_table.compute_hash((void *)index)% HASHTAB_SIZE;

	        if (qos_if_table.buckets[val] != NULL)
	        {
	        	pbucket = qos_if_table.buckets[val];
	        }
	        else
	        {
	        	for (++val; val<HASHTAB_SIZE; ++val)
	            {
	                if (NULL != qos_if_table.buckets[val])
	                {
	                    pbucket = qos_if_table.buckets[val];

						flag = 1;
	                }
	            }
	        }
        }
        while (pbucket)
        {
            qos_if = (struct qos_if *)(pbucket->data);

			if ((NULL != qos_if)&&(NULL != qos_if->pqos_policy))
            {
				for (ALL_LIST_ELEMENTS_RO(qos_if->pqos_policy, if_node, pif_policy))
				{
					if((flag == 0)&&(pif_policy->ifindex == ifindex)&&(pif_policy->direct == direct)
					&&(pif_policy->type == type)&&(pif_policy->acl_num == acl_num))
		        	{
		        		flag = 1;
						//break;
						continue;
					}

					if(flag == 1)
					{
						memcpy(&ppolicy[data_num++], pif_policy, sizeof(struct qos_policy));

						if (data_num == msg_len)
			            {
			                return data_num;
			            }
					}
				}
			}

			pnext = hios_hash_next_cursor(&qos_if_table, pbucket);
            if ((NULL == pnext) || (NULL == pnext->data))
            {
                break;
            }

			pbucket = pnext;
        }
    }

    return data_num;
}


int qos_policy_return_str (int ret,struct vty *vty)
{
	if(ret == ERRNO_SUCCESS )
	{
		vty_out(vty, "Success to operate with the qos policy.%s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}
	if(ret == ERRNO_FAIL)
	{
		vty_error_out(vty, "Fail operate with  the qos policy.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(ret == ERRNO_EXISTED)
	{
		vty_warning_out(vty, "Qos policy has already existed.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(ret == ERRNO_NOT_FOUND)
	{
		vty_warning_out(vty, "Can not find this qos policy.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(ret == ERRNO_HAL)
	{
		vty_error_out(vty, "Failed to support the resource,please check out %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(ret == QOS_ERR_CONFLICT)
	{
		vty_error_out(vty, "Interface already configured with qos car%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	return CMD_WARNING;
}


static void qos_policy_show(struct vty *vty, struct qos_policy *qos_policy,uint32_t ifindex)
{
	char ifname[IFNET_NAMESIZE];
	char ifname1[IFNET_NAMESIZE]= "global";

	if(ifindex != 0)
	{
		ifm_get_name_by_ifindex ( qos_policy->action.ifindex,ifname1);
	}

	switch(qos_policy->type)
	{
		case POLICY_TYPE_FILTER:
			if(qos_policy->direct == QOS_DIR_EGRESS)
				vty_out(vty, "Apply interface: %-15s  Policy: qos policy egress acl %d filter %s%s",ifname1,qos_policy->acl_num,qos_policy->action.filter_act ? "deny" : "permit",VTY_NEWLINE);
			else
				vty_out(vty, "Apply interface: %-15s  Policy: qos policy ingress acl %d filter %s%s",ifname1,qos_policy->acl_num,qos_policy->action.filter_act ? "deny" : "permit",VTY_NEWLINE);
			break;
		case POLICY_TYPE_CAR:
			if(qos_policy->direct == QOS_DIR_EGRESS)
				vty_out(vty, "Apply interface: %-15s  Policy: qos policy egress acl %d car %d%s",ifname1,qos_policy->acl_num,qos_policy->action.car_id,VTY_NEWLINE);
			else
				vty_out(vty, "Apply interface: %-15s  Policy: qos policy ingress acl %d car %d%s",ifname1,qos_policy->acl_num,qos_policy->action.car_id,VTY_NEWLINE);
			break;
		case POLICY_TYPE_MIRROR:
			ifm_get_name_by_ifindex ( qos_policy->action.mirror_if,ifname);
			vty_out(vty, "Apply interface: %-15s  Policy: qos policy ingress acl %d mirror-to interface %s%s",ifname1,qos_policy->acl_num,ifname,VTY_NEWLINE);
			break;
		case POLICY_TYPE_REDIRECT:
			ifm_get_name_by_ifindex ( qos_policy->action.redirect_if,ifname);
			vty_out(vty, "Apply interface: %-15s  Policy: qos policy ingress acl %d redirect-to interface %s%s",ifname1,qos_policy->acl_num,ifname,VTY_NEWLINE);
			break;
		case POLICY_TYPE_QOSMAPPING:
			vty_out(vty, "Apply interface: %-15s  Policy: qos policy ingress acl %d qos-mapping queue %d%s",ifname1,qos_policy->acl_num,qos_policy->action.queue_id,VTY_NEWLINE);
			break;
		default:
			break;
	}

}


static void qos_policy_show_operation(struct vty *vty, struct qos_policy *qos_policy)
{
	char ifname[IFNET_NAMESIZE];

	switch(qos_policy->type)
	{
		case POLICY_TYPE_FILTER:
			if(qos_policy->direct == QOS_DIR_EGRESS)
				vty_out(vty, "qos policy egress acl %d filter %s%s",qos_policy->acl_num,qos_policy->action.filter_act ? "deny" : "permit",VTY_NEWLINE);
			else
				vty_out(vty, "qos policy ingress acl %d filter %s%s",qos_policy->acl_num,qos_policy->action.filter_act ? "deny" : "permit",VTY_NEWLINE);
			break;
		case POLICY_TYPE_CAR:
			if(qos_policy->direct == QOS_DIR_EGRESS)
				vty_out(vty, "qos policy egress acl %d car %d%s",qos_policy->acl_num,qos_policy->action.car_id,VTY_NEWLINE);
			else
				vty_out(vty, "qos policy ingress acl %d car %d%s",qos_policy->acl_num,qos_policy->action.car_id,VTY_NEWLINE);
			break;
		case POLICY_TYPE_MIRROR:
			ifm_get_name_by_ifindex ( qos_policy->action.mirror_if,ifname);
			vty_out(vty, "qos policy ingress acl %d mirror-to interface %s%s",qos_policy->acl_num,ifname,VTY_NEWLINE);
			break;
		case POLICY_TYPE_REDIRECT:
			ifm_get_name_by_ifindex ( qos_policy->action.redirect_if,ifname);
			vty_out(vty, "qos policy ingress acl %d redirect-to interface %s%s",qos_policy->acl_num,ifname,VTY_NEWLINE);
			break;
		case POLICY_TYPE_QOSMAPPING:
			vty_out(vty, "qos policy ingress acl %d qos-mapping queue %d%s",qos_policy->acl_num,qos_policy->action.queue_id,VTY_NEWLINE);
			break;
		default:
        	break;
	}

}



struct cmd_node qos_policy_node =
{
  QOS_POLICY_NODE,
  "%s(config-qos-policy)# ",
  1,
};

DEFUN (qos_policy_mode,
	qos_policy_mode_cmd,
	"qos policy",
	 "Qos\n"
     "Qos policy\n")
{
	vty->node = QOS_POLICY_NODE;
	return CMD_SUCCESS;
}


DEFUN(qos_subif_no_encap,
    qos_subif_no_encap_cmd,
    "no encapsulate",
	NO_STR
	"Subif encapsulate\n")
{
	struct hash_bucket *p_hash_bucket = NULL;
	struct qos_if *pif = NULL;
	struct qos_policy *qos_policy = NULL;
	struct listnode *node = NULL;
	struct listnode *next_node = NULL;
	struct car_t *pcar = NULL;
	struct ifm_info pifm = {0};

    uint32_t ifindex = (uint32_t)vty->index;

    if (IFM_TYPE_IS_TRUNK(ifindex))
    {
        return CMD_SUCCESS;
    }

	if(IFM_IS_SUBPORT(ifindex) && IFM_TYPE_IS_METHERNET(IFM_PARENT_IFINDEX_GET(ifindex)))
	{
		p_hash_bucket = hios_hash_find(&qos_if_table, (void *)ifindex);
		if(NULL == p_hash_bucket)
		{
			return CMD_SUCCESS;
		}

		pif = (struct qos_if *)p_hash_bucket->data;
		if(pif == NULL)
		{
			XFREE(MTYPE_HASH_BACKET, p_hash_bucket);
			return CMD_SUCCESS;
		}

		/* 删除qos policy */
		if(pif->pqos_policy != NULL)
		{
			return CMD_SUCCESS;
		}

		ifm_get_all_info( ifindex, MODULE_ID_QOS , &pifm);
		if ((pifm.mode != IFNET_MODE_L3))
	    {
	        return CMD_SUCCESS;
	    }

		for ( ALL_LIST_ELEMENTS ( pif->pqos_policy, node, next_node, qos_policy ) )
		{
			if(qos_policy->type == POLICY_TYPE_CAR)
			{
				/*car 模板引用计数减一*/
				pcar = qos_car_profile_lookup(qos_policy->action.car_id);
				if(NULL == pcar)
				{
					return CMD_WARNING;
				}
				qos_car_profile_attached(pcar, 0);
			}

			XFREE ( MTYPE_QOS_ENTRY, node->data );
			list_delete_node (pif->pqos_policy, node);
			if (list_isempty (pif->pqos_policy))
			{
				list_free(pif->pqos_policy);
				pif->pqos_policy =NULL;
			}
		}
	}

    return CMD_SUCCESS;
}


DEFUN  (qos_policy_acl_filter,
		qos_policy_acl_filter_cmd,
       "qos policy (ingress|egress) acl <1-65535> filter (permit|deny)",
       "Qos\n"
       "Qos policy\n"
       "Ingress\n"
       "Egress\n"
	   "Acl rule\n"
	   "Acl number <1-65535>\n"
	   "Acl action: filter\n"
	   "Permit acl rule filter\n"
	   "Deny acl rule filter\n")
{
	int ret =0;
	int rv =0;
	uint32_t ifindex = 0;
	struct acl_group * acl_node = NULL;
	struct qos_policy qos_policy;

	memset(&qos_policy,0,sizeof(struct qos_policy));

	if(!strncmp(argv[0], "ingress", 1))
	{
		qos_policy.direct = QOS_DIR_INGRESS;
	}
	else if(!strncmp(argv[0], "egress", 1))
	{
		qos_policy.direct = QOS_DIR_EGRESS;
	}

	qos_policy.acl_num = atoi(argv[1]);

	acl_node = acl_group_lookup( qos_policy.acl_num);
	if(NULL == acl_node)
	{
		vty_error_out(vty, "can not find this acl_group,please config acl_group first!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	memset(&qos_policy.action,0,sizeof(struct acl_action));
	qos_policy.action.filter = 1;
	qos_policy.action.act_num = 1;

	if(!strncmp(argv[2], "deny", 1))
	{
		qos_policy.action.filter_act = 1;  // 0: permit, 1: deny
	}
	else if(!strncmp(argv[2], "permit", 1))
	{
		qos_policy.action.filter_act = 0;
	}

	if(vty->node == CONFIG_NODE )
	{
		ifindex = 0;
		qos_policy.ifindex = 0;
		qos_policy.action.ifindex = 0;

		ret = qos_policy_add(qos_policy.acl_num, POLICY_TYPE_FILTER,qos_policy.direct, ifindex, &qos_policy.action);
	}
	else
	{
		ifindex = (uint32_t)vty->index;
		qos_policy.ifindex = ifindex;
		qos_policy.action.ifindex = ifindex;

		ret = qos_if_check(ifindex);

		// 2018-05-04, by caojt, add IFNET_MODE_PHYSICAL for L1 ethernet port
		if(QOS_ERR_NOT_SUPPORT == ret)
		{
			vty_error_out ( vty, "Failed to execute command, due to the interface not support!! %s", VTY_NEWLINE );
			return CMD_WARNING;
		}

		if(QOS_ERR_BINDED_TRUNK == ret)
		{
			vty_error_out ( vty, "Failed to execute command, due to the interface had binded trunk!! %s", VTY_NEWLINE );
			return CMD_WARNING;
		}
		if(QOS_ERR_CONFIG_IF == ret)
		{
			vty_error_out ( vty, "Failed to execute command, not support config ethernet 1/0/1!! %s", VTY_NEWLINE );
			return CMD_WARNING;
		}
		if(QOS_ERR_CONFIG_ETH_SUBIF == ret)
		{
			vty_error_out ( vty, "Failed to execute command, sub ethernet interface need encapsulate vlan!! %s", VTY_NEWLINE );
			return CMD_WARNING;
		}

		ret = qos_policy_add_if(qos_policy.acl_num, POLICY_TYPE_FILTER,qos_policy.direct, ifindex, &qos_policy.action);
	}

	rv = qos_policy_return_str(ret,vty);
	return rv;
}


DEFUN  (qos_policy_acl_car,
		qos_policy_acl_car_cmd,
       "qos policy (ingress|egress) acl <1-65535> car <1-1000>",
       "Qos\n"
       "Qos policy\n"
       "Ingress\n"
       "Egress\n"
	   "Acl rule\n"
	   "Acl number <1-65535>\n"
	   "Acl action: car \n"
	   "Qos car-profile id <1-1000>\n")
{
	int ret =0;
	uint32_t ifindex = 0;
	struct acl_group * acl_node = NULL;
	struct car_t *acl_car = NULL;
	struct qos_policy qos_policy;

	memset(&qos_policy,0,sizeof(struct qos_policy));

	if(!strncmp(argv[0], "ingress", 1))
	{
		qos_policy.direct = QOS_DIR_INGRESS;
	}
	else if(!strncmp(argv[0], "egress", 1))
	{
		qos_policy.direct = QOS_DIR_EGRESS;
	}

	qos_policy.acl_num = atoi(argv[1]);

	acl_node = acl_group_lookup( qos_policy.acl_num);
	if(NULL == acl_node)
	{
		vty_error_out(vty, "can not find this acl_group,please config acl_group first!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	memset(&qos_policy.action,0,sizeof(struct acl_action));
	qos_policy.action.act_num = 1;
	qos_policy.action.car = 1;
	qos_policy.action.car_id = atoi(argv[2]);

	acl_car = qos_car_profile_lookup(qos_policy.action.car_id);
	if(NULL == acl_car)
	{
		vty_error_out(vty, "can not find this car_id,please config car profile first!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(vty->node ==CONFIG_NODE )
	{
		ifindex = 0;
		qos_policy.ifindex = 0;
		qos_policy.action.ifindex = 0;

		ret = qos_policy_add(qos_policy.acl_num, POLICY_TYPE_CAR,qos_policy.direct, ifindex, &qos_policy.action);
	}
	else
	{
		ifindex = (uint32_t)vty->index;
		qos_policy.ifindex = ifindex;
		qos_policy.action.ifindex = ifindex;

		ret = qos_if_check(ifindex);

		// 2018-05-04, by caojt, add IFNET_MODE_PHYSICAL for L1 ethernet port
		if(QOS_ERR_NOT_SUPPORT == ret)
		{
			vty_error_out ( vty, "Failed to execute command, due to the interface not support!! %s", VTY_NEWLINE );
			return CMD_WARNING;
		}

		if(QOS_ERR_BINDED_TRUNK == ret)
		{
			vty_error_out ( vty, "Failed to execute command, due to the interface had binded trunk!! %s", VTY_NEWLINE );
			return CMD_WARNING;
		}
		if(QOS_ERR_CONFIG_IF == ret)
		{
			vty_error_out ( vty, "Failed to execute command, not support config ethernet 1/0/1!! %s", VTY_NEWLINE );
			return CMD_WARNING;
		}
		if(QOS_ERR_CONFIG_ETH_SUBIF == ret)
		{
			vty_error_out ( vty, "Failed to execute command, sub ethernet interface need encapsulate vlan!! %s", VTY_NEWLINE );
			return CMD_WARNING;
		}

		ret = qos_policy_add_if(qos_policy.acl_num, POLICY_TYPE_CAR,qos_policy.direct, ifindex, &qos_policy.action);
	}
	if(ret == CMD_SUCCESS)
	{
		/*car 模板引用计数加一*/
		acl_car->ref_cnt++;
		qos_policy_return_str(ret,vty);
		return CMD_SUCCESS;
	}
	else
	{
		qos_policy_return_str(ret,vty);
		return CMD_WARNING;
	}
}


DEFUN  (qos_policy_acl_mirror,
		qos_policy_acl_mirror_cmd,
		"qos policy ingress acl <1-65535> mirror-to interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP|trunk TRUNK}",
		"Qos\n"
		"Qos policy\n"
		"Ingress\n"
		"Acl rule\n"
		"Acl number <1-65535>\n"
		"Acl action: mirror-to\n"
		CLI_INTERFACE_STR
		CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
		CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR)
{
    int ret = 0;
	int rv = 0;
	uint32_t ifindex = 0;
	struct acl_group * acl_node = NULL;
	struct ifm_info pifm = {0};
	struct qos_policy qos_policy;


	memset(&qos_policy,0,sizeof(struct qos_policy));
	qos_policy.direct = QOS_DIR_INGRESS;
	qos_policy.acl_num = atoi(argv[0]);

	acl_node = acl_group_lookup( qos_policy.acl_num);
	if(NULL == acl_node)
	{
		vty_error_out(vty, "can not find this acl_group,please config acl_group first!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	memset(&qos_policy.action,0,sizeof(struct acl_action));
	qos_policy.action.act_num = 1;
	qos_policy.action.mirror = 1;

	if(( NULL != argv[1])&&( NULL == argv[2])&&( NULL == argv[3])&&( NULL == argv[4]))
	{
		qos_policy.action.mirror_if = ifm_get_ifindex_by_name ( (char *)"ethernet", (char *)argv[1]);
	}
	else if(( NULL == argv[1])&&( NULL != argv[2])&&( NULL == argv[3])&&( NULL == argv[4]))
	{
		qos_policy.action.mirror_if = ifm_get_ifindex_by_name ( (char *)"gigabitethernet", (char *)argv[2]);
	}
	else if(( NULL == argv[1])&&( NULL == argv[2])&&( NULL != argv[3])&&( NULL == argv[4]))
	{
		qos_policy.action.mirror_if = ifm_get_ifindex_by_name ( (char *)"xgigabitethernet", (char *)argv[3]);
	}
	else if(( NULL == argv[1])&&( NULL == argv[2])&&( NULL == argv[3])&&( NULL != argv[4]))
	{
		qos_policy.action.mirror_if = ifm_get_ifindex_by_name ( (char *)"trunk", (char *)argv[4]);
	}
	else
	{
		vty_error_out ( vty, "please config ethernet or trunk interface%s", VTY_NEWLINE );
	    return CMD_WARNING;
	}
	if (qos_policy.action.mirror_if == 0 )
	{
	    vty_error_out ( vty, "can not find this interface mirror-to ,please check out%s", VTY_NEWLINE );
	    return CMD_WARNING;
	}
	if(IFM_IS_SUBPORT(qos_policy.action.mirror_if))
	{
		vty_error_out ( vty, "can not support mirror-to subport ,please check out%s", VTY_NEWLINE );
	    return CMD_WARNING;
	}

	ifm_get_all_info(qos_policy.action.mirror_if, MODULE_ID_QOS, &pifm);

    // 2018-05-04, by caojt, add IFNET_MODE_PHYSICAL for L1 ethernet port
    if (IFNET_MODE_PHYSICAL == pifm.mode)
    {
        vty_error_out ( vty, "this interface not support mirror, please check out%s", VTY_NEWLINE );
        return CMD_WARNING;
    }

	if(((( NULL != argv[1])||(NULL != argv[2])||(NULL != argv[3]))&&( NULL == argv[4]))&&(pifm.mode == IFNET_MODE_INVALID))
	{
		vty_error_out ( vty, "this interface is member of trunk ,please check out%s", VTY_NEWLINE );
		return CMD_WARNING;
	}
	/*不为管理口*/
	if(IFM_TYPE_IS_METHERNET(qos_policy.action.mirror_if)&&(IFM_SLOT_ID_GET ( qos_policy.action.mirror_if ) == 0))
	{
		vty_error_out ( vty, "not support mirror-to ethernet 1/0/1,please check out! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(vty->node ==CONFIG_NODE )
	{
		ifindex = 0;
		qos_policy.ifindex = 0;
		qos_policy.action.ifindex = 0;

		ret = qos_policy_add(qos_policy.acl_num, POLICY_TYPE_MIRROR,qos_policy.direct, ifindex, &qos_policy.action);
	}
	else
	{
		ifindex = (uint32_t)vty->index;
		qos_policy.ifindex = ifindex;
		qos_policy.action.ifindex = ifindex;

		ret = qos_if_check(ifindex);

		// 2018-05-04, by caojt, add IFNET_MODE_PHYSICAL for L1 ethernet port
		if(QOS_ERR_NOT_SUPPORT == ret)
		{
			vty_error_out ( vty, "Failed to execute command, due to the interface not support!! %s", VTY_NEWLINE );
			return CMD_WARNING;
		}

		if(QOS_ERR_BINDED_TRUNK == ret)
		{
			vty_error_out ( vty, "Failed to execute command, due to the interface had binded trunk!! %s", VTY_NEWLINE );
			return CMD_WARNING;
		}
		if(QOS_ERR_CONFIG_IF == ret)
		{
			vty_error_out ( vty, "Failed to execute command, not support config ethernet 1/0/1!! %s", VTY_NEWLINE );
			return CMD_WARNING;
		}
		if(QOS_ERR_CONFIG_ETH_SUBIF == ret)
		{
			vty_error_out ( vty, "Failed to execute command, sub ethernet interface need encapsulate vlan!! %s", VTY_NEWLINE );
			return CMD_WARNING;
		}

		ret = qos_policy_add_if(qos_policy.acl_num, POLICY_TYPE_MIRROR,qos_policy.direct, ifindex, &qos_policy.action);
	}

	rv = qos_policy_return_str(ret,vty);
	return rv;
}


DEFUN  (qos_policy_acl_redirect,
		qos_policy_acl_redirect_cmd,
		"qos policy ingress acl <1-65535> redirect-to interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP|trunk TRUNK}",
		"Qos\n"
		"Qos policy\n"
		"Ingress\n"
		"Acl rule\n"
		"Acl number <1-65535>\n"
		"Acl action: redirect-to\n"
		CLI_INTERFACE_STR
		CLI_INTERFACE_ETHERNET_STR
		CLI_INTERFACE_ETHERNET_VHELP_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
		CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR)
{
    int ret =0;
	int rv =0;
	uint32_t ifindex = 0;
	struct acl_group * acl_node = NULL;
	struct ifm_info pifm = {0};
	struct qos_policy qos_policy;

	memset(&qos_policy,0,sizeof(struct qos_policy));
	qos_policy.direct = QOS_DIR_INGRESS;
	qos_policy.acl_num = atoi(argv[0]);

	acl_node = acl_group_lookup( qos_policy.acl_num);
	if(NULL == acl_node)
	{
		vty_error_out(vty, "can not find this acl_group,please config acl_group first!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	memset(&qos_policy.action,0,sizeof(struct acl_action));
	qos_policy.action.act_num = 1;
	qos_policy.action.redirect = 1;

	if(( NULL != argv[1])&&( NULL == argv[2])&&( NULL == argv[3])&&( NULL == argv[4]))
	{
		qos_policy.action.redirect_if = ifm_get_ifindex_by_name ( (char *)"ethernet", (char *)argv[1]);
	}
	else if(( NULL == argv[1])&&( NULL != argv[2])&&( NULL == argv[3])&&( NULL == argv[4]))
	{
		qos_policy.action.redirect_if = ifm_get_ifindex_by_name ( (char *)"gigabitethernet", (char *)argv[2]);
	}
	else if(( NULL == argv[1])&&( NULL == argv[2])&&( NULL != argv[3])&&( NULL == argv[4]))
	{
		qos_policy.action.redirect_if = ifm_get_ifindex_by_name ( (char *)"xgigabitethernet", (char *)argv[3]);
	}
	else if(( NULL == argv[1])&&( NULL == argv[2])&&( NULL == argv[3])&&( NULL != argv[4]))
	{
		qos_policy.action.redirect_if = ifm_get_ifindex_by_name ( (char *)"trunk", (char *)argv[4]);
	}
	else
	{
		vty_error_out ( vty, "Please config ethernet or trunk interface%s", VTY_NEWLINE );
	    return CMD_WARNING;
	}
	if (qos_policy.action.redirect_if== 0 )
	{
	    vty_error_out ( vty, "Can not find this interface redirect-to ,please check out%s", VTY_NEWLINE );
	    return CMD_WARNING;
	}
	if(IFM_IS_SUBPORT(qos_policy.action.redirect_if))
	{
		vty_error_out ( vty, "Can not support redirect-to subport ,please check out%s", VTY_NEWLINE );
	    return CMD_WARNING;
	}

	ifm_get_all_info(qos_policy.action.redirect_if, MODULE_ID_QOS, &pifm);
	
    // 2018-05-04, by caojt, add IFNET_MODE_PHYSICAL for L1 ethernet port
    if (IFNET_MODE_PHYSICAL == pifm.mode)
    {
        vty_error_out ( vty, "this interface not support acl redirect, please check out%s", VTY_NEWLINE );
        return CMD_WARNING;
    }

	if(((( NULL != argv[1])||(NULL != argv[2])||(NULL != argv[3]))&&( NULL == argv[4]))&&(pifm.mode == IFNET_MODE_INVALID))
	{
		vty_error_out ( vty, "this interface is member of trunk ,please check out%s", VTY_NEWLINE );
		return CMD_WARNING;
	}
	/*不为管理口*/
	if(IFM_TYPE_IS_METHERNET(qos_policy.action.redirect_if)&&(IFM_SLOT_ID_GET ( qos_policy.action.redirect_if ) == 0))
	{
		vty_error_out ( vty, "not support redirect-to ethernet 1/0/1,please check out! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(vty->node ==CONFIG_NODE )
	{
		ifindex = 0;
		qos_policy.ifindex = 0;
		qos_policy.action.ifindex = 0;

		ret = qos_policy_add(qos_policy.acl_num, POLICY_TYPE_REDIRECT,qos_policy.direct, ifindex, &qos_policy.action);
	}
	else
	{
		ifindex = (uint32_t)vty->index;
		qos_policy.ifindex = ifindex;
		qos_policy.action.ifindex = ifindex;

		ret = qos_if_check(ifindex);

		// 2018-05-04, by caojt, add IFNET_MODE_PHYSICAL for L1 ethernet port
		if(QOS_ERR_NOT_SUPPORT == ret)
		{
			vty_error_out ( vty, "Failed to execute command, due to the interface not support!! %s", VTY_NEWLINE );
			return CMD_WARNING;
		}

		if(QOS_ERR_BINDED_TRUNK == ret)
		{
			vty_error_out ( vty, "Failed to execute command, due to the interface had binded trunk!! %s", VTY_NEWLINE );
			return CMD_WARNING;
		}
		if(QOS_ERR_CONFIG_IF == ret)
		{
			vty_error_out ( vty, "Failed to execute command, not support config ethernet 1/0/1!! %s", VTY_NEWLINE );
			return CMD_WARNING;
		}
		if(QOS_ERR_CONFIG_ETH_SUBIF == ret)
		{
			vty_error_out ( vty, "Failed to execute command, sub ethernet interface need encapsulate vlan!! %s", VTY_NEWLINE );
			return CMD_WARNING;
		}

		ret = qos_policy_add_if(qos_policy.acl_num, POLICY_TYPE_REDIRECT,qos_policy.direct, ifindex, &qos_policy.action);
	}


	rv = qos_policy_return_str(ret,vty);
	return rv;
}

DEFUN  (qos_policy_acl_queue,
		qos_policy_acl_queue_cmd,
       "qos policy ingress acl <1-65535> qos-mapping queue <0-7>",
       "Qos\n"
       "Qos policy\n"
       "Ingress\n"
	   "Acl rule\n"
	   "Acl number <1-65535>\n"
	   "Acl action: qos-mapping\n"
	   "Qos mapping to queue\n"
	   "Queue  number <0-7>\n")
{
	int ret =0;
	int rv =0;
	uint32_t ifindex = 0;
	struct acl_group * acl_node = NULL;
	struct qos_policy qos_policy;

	memset(&qos_policy,0,sizeof(struct qos_policy));
	qos_policy.direct = QOS_DIR_INGRESS;
	qos_policy.acl_num = atoi(argv[0]);

	acl_node = acl_group_lookup( qos_policy.acl_num);
	if(acl_node == NULL)
	{
		vty_out(vty, "Error:can not find this acl_group,please config acl_group first!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	memset(&qos_policy.action,0,sizeof(struct acl_action));
	qos_policy.action.act_num = 1;
	qos_policy.action.queue = 1;
	qos_policy.action.queue_id = atoi(argv[1]);

	if(vty->node == CONFIG_NODE )
	{
		ifindex = 0;
		qos_policy.ifindex = 0;
		qos_policy.action.ifindex = 0;

		ret = qos_policy_add(qos_policy.acl_num, POLICY_TYPE_QOSMAPPING,qos_policy.direct, ifindex, &qos_policy.action);
	}
	else
	{
		ifindex = (uint32_t)vty->index;
		qos_policy.ifindex = ifindex;
		qos_policy.action.ifindex = ifindex;

		ret = qos_if_check(ifindex);

		// 2018-05-04, by caojt, add IFNET_MODE_PHYSICAL for L1 ethernet port
		if(QOS_ERR_NOT_SUPPORT == ret)
		{
			vty_error_out ( vty, "Failed to execute command, due to the interface not support!! %s", VTY_NEWLINE );
			return CMD_WARNING;
		}

		if(QOS_ERR_BINDED_TRUNK == ret)
		{
			vty_error_out ( vty, "Failed to execute command, due to the interface had binded trunk!! %s", VTY_NEWLINE );
			return CMD_WARNING;
		}
		if(QOS_ERR_CONFIG_IF == ret)
		{
			vty_error_out ( vty, "Failed to execute command, not support config ethernet 1/0/1!! %s", VTY_NEWLINE );
			return CMD_WARNING;
		}
		if(QOS_ERR_CONFIG_ETH_SUBIF == ret)
		{
			vty_error_out ( vty, "Failed to execute command, sub ethernet interface need encapsulate vlan!! %s", VTY_NEWLINE );
			return CMD_WARNING;
		}

		ret = qos_policy_add_if(qos_policy.acl_num, POLICY_TYPE_QOSMAPPING,qos_policy.direct, ifindex, &qos_policy.action);
	}

	rv = qos_policy_return_str(ret,vty);
	return rv;
}

DEFUN  (no_qos_policy_acl,
		no_qos_policy_acl_cmd,
       "no qos policy (ingress|egress) acl <1-65535> ( filter|mirror-to|redirect-to|qos-mapping|car)",
       	NO_STR
		"Qos\n"
		"Qos policy\n"
		"Ingress\n"
	   "Egress\n"
	   "Acl rule\n"
	   "Acl number <1-65535>\n"
	   "Acl action: filter\n"
	   "Acl action: mirror\n"
	   "Acl action: redirect\n"
	   "Acl action: qos-mapping\n"
	   "Acl action: car\n")
{
	int ret =0;
	int rv =0;
	uint32_t ifindex = 0;
	enum POLICY_TYPE type;
	struct acl_group * acl_node = NULL;
	struct qos_policy qos_policy;

	memset(&qos_policy,0,sizeof(struct qos_policy));

	if( strncmp ( argv[0], "ingress", 1 ) == 0)
	{
		qos_policy.direct = QOS_DIR_INGRESS;
	}
	else if(strncmp ( argv[0], "egress", 1 ) == 0)
	{
		qos_policy.direct = QOS_DIR_EGRESS;
	}

	qos_policy.acl_num = atoi(argv[1]);

	acl_node = acl_group_lookup( qos_policy.acl_num);
	if(NULL == acl_node)
	{
		vty_error_out(vty, "can not find this acl_group!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if ( strncmp ( argv[2], "fi", 1 ) == 0 )
	{
		type = POLICY_TYPE_FILTER;
	}
	else if ( strncmp ( argv[2], "mi", 1 ) == 0 )
	{
		type = POLICY_TYPE_MIRROR;
	}
	else  if ( strncmp ( argv[2], "re", 1 ) == 0 )
	{
		type = POLICY_TYPE_REDIRECT;
	}
	else   if ( strncmp ( argv[2], "qo", 1 ) == 0 )
	{
		type = POLICY_TYPE_QOSMAPPING;
	}
	else   if ( strncmp ( argv[2], "car", 1 ) == 0 )
	{
		type = POLICY_TYPE_CAR;

	}
	else
	 	return CMD_WARNING;


	if(vty->node ==CONFIG_NODE )
	{
		ret = qos_policy_delete(qos_policy.acl_num, type,qos_policy.direct, 0);
	}
	else
	{
		ifindex = (uint32_t)vty->index;
		ret = qos_policy_delete_if(qos_policy.acl_num, type,qos_policy.direct, ifindex);
	}

	rv = qos_policy_return_str(ret,vty);
	return rv;
}


DEFUN  (show_qos_policy_acl,
		show_qos_policy_acl_cmd,
       "show qos policy {acl <1-65535>} ",
       "show qos policy\n"
		"Qos\n"
		"Qos policy\n"
	   "Acl rule\n"
	   "Acl number <1-65535>\n")
{
	uint32_t acl_num = 0;
	uint32_t cursor = 0;
	struct qos_if *qos_if = NULL;
	struct qos_policy *qos_policy = NULL;
	struct listnode *node = NULL;
	struct hash_bucket *pbucket = NULL;

	if(NULL == argv[0])
	{
		if(NULL != qos_policy_global.head)
		{
			for (node = qos_policy_global.head ; node; node = node->next)
			{
				qos_policy = listgetdata(node);
				qos_policy_show(vty, qos_policy, 0);
			}
		}

		HASH_BUCKET_LOOP ( pbucket, cursor, qos_if_table)
		{
			if(NULL != pbucket->data)
			{
				qos_if = (struct qos_if *)pbucket->data;

				for ( ALL_LIST_ELEMENTS_RO ( qos_if->pqos_policy, node, qos_policy ) )
				{
					qos_policy_show(vty, qos_policy,qos_policy->action.ifindex);
				}
			}
		}
	}
	else
	{
		acl_num = atoi(argv[0]);
		if(NULL != qos_policy_global.head)
		{
			for (node = qos_policy_global.head ; node; node = node->next)
			{
				qos_policy = listgetdata(node);
				if(qos_policy->acl_num == acl_num)
				{
					qos_policy_show(vty, qos_policy, 0);
				}
			}
		}

		HASH_BUCKET_LOOP ( pbucket, cursor, qos_if_table)
		{
			if(NULL != pbucket->data)
			{
				qos_if = (struct qos_if *)pbucket->data;

				for ( ALL_LIST_ELEMENTS_RO ( qos_if->pqos_policy, node, qos_policy ) )
				{
					if(qos_policy->acl_num == acl_num)
					{
						qos_policy_show(vty, qos_policy,qos_policy->action.ifindex);
					}
				}
			}
		}
	}
	return CMD_SUCCESS;
}


int qos_policy_config_write (struct vty *vty)
{
	struct qos_policy *qos_policy = NULL;
	struct listnode *node = NULL;

	if ( NULL != qos_policy_global.head)
	{
		for (node = qos_policy_global.head ; node; node = node->next)
		{
			qos_policy = listgetdata(node);

			qos_policy_show_operation(vty, qos_policy);
		}
	}

    return ERRNO_SUCCESS;
}

void qos_policy_cmd_init(void)
{
	install_node (&qos_policy_node, qos_policy_config_write);
	install_default (QOS_POLICY_NODE);

	install_element (CONFIG_NODE, &qos_policy_acl_filter_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &qos_policy_acl_car_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &qos_policy_acl_mirror_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &qos_policy_acl_redirect_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &qos_policy_acl_queue_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &no_qos_policy_acl_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &show_qos_policy_acl_cmd, CMD_LOCAL);

	install_element (PHYSICAL_IF_NODE, &qos_policy_acl_filter_cmd, CMD_SYNC);
	install_element (PHYSICAL_SUBIF_NODE, &qos_policy_acl_filter_cmd, CMD_SYNC);
	install_element (TRUNK_SUBIF_NODE, &qos_policy_acl_filter_cmd, CMD_SYNC);
	install_element (TRUNK_IF_NODE, &qos_policy_acl_filter_cmd, CMD_SYNC);
	install_element (VLANIF_NODE, &qos_policy_acl_filter_cmd, CMD_SYNC);

	install_element (PHYSICAL_IF_NODE, &qos_policy_acl_car_cmd, CMD_SYNC);
	install_element (PHYSICAL_SUBIF_NODE, &qos_policy_acl_car_cmd, CMD_SYNC);
	install_element (TRUNK_SUBIF_NODE, &qos_policy_acl_car_cmd, CMD_SYNC);
	install_element (TRUNK_IF_NODE, &qos_policy_acl_car_cmd, CMD_SYNC);
	install_element (VLANIF_NODE, &qos_policy_acl_car_cmd, CMD_SYNC);

	install_element (PHYSICAL_IF_NODE, &qos_policy_acl_mirror_cmd, CMD_SYNC);
	install_element (PHYSICAL_SUBIF_NODE, &qos_policy_acl_mirror_cmd, CMD_SYNC);
	install_element (TRUNK_SUBIF_NODE, &qos_policy_acl_mirror_cmd, CMD_SYNC);
	install_element (TRUNK_IF_NODE, &qos_policy_acl_mirror_cmd, CMD_SYNC);
	install_element (VLANIF_NODE, &qos_policy_acl_mirror_cmd, CMD_SYNC);

	install_element (PHYSICAL_IF_NODE, &qos_policy_acl_redirect_cmd, CMD_SYNC);
	install_element (PHYSICAL_SUBIF_NODE, &qos_policy_acl_redirect_cmd, CMD_SYNC);
	install_element (TRUNK_SUBIF_NODE, &qos_policy_acl_redirect_cmd, CMD_SYNC);
	install_element (TRUNK_IF_NODE, &qos_policy_acl_redirect_cmd, CMD_SYNC);
	install_element (VLANIF_NODE, &qos_policy_acl_redirect_cmd, CMD_SYNC);

	install_element (PHYSICAL_IF_NODE, &qos_policy_acl_queue_cmd, CMD_SYNC);
	install_element (PHYSICAL_SUBIF_NODE, &qos_policy_acl_queue_cmd, CMD_SYNC);
	install_element (TRUNK_SUBIF_NODE, &qos_policy_acl_queue_cmd, CMD_SYNC);
	install_element (TRUNK_IF_NODE, &qos_policy_acl_queue_cmd, CMD_SYNC);
	install_element (VLANIF_NODE, &qos_policy_acl_queue_cmd, CMD_SYNC);

	install_element (PHYSICAL_IF_NODE, &no_qos_policy_acl_cmd, CMD_SYNC);
	install_element (PHYSICAL_SUBIF_NODE, &no_qos_policy_acl_cmd, CMD_SYNC);
	install_element (TRUNK_SUBIF_NODE, &no_qos_policy_acl_cmd, CMD_SYNC);
	install_element (TRUNK_IF_NODE, &no_qos_policy_acl_cmd, CMD_SYNC);
	install_element (VLANIF_NODE, &no_qos_policy_acl_cmd, CMD_SYNC);

	install_element (PHYSICAL_SUBIF_NODE, &qos_subif_no_encap_cmd, CMD_SYNC);
	install_element (TRUNK_SUBIF_NODE, &qos_subif_no_encap_cmd, CMD_SYNC);

}


