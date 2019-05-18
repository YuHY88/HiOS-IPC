#include <string.h>
#include <stdlib.h>
#include <lib/log.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/zassert.h>
#include <lib/msg_ipc_n.h>
#include <lib/errcode.h>
#include <lib/qos_common.h>

#include "qos_if.h"
#include "qos_car.h"
#include "qos_policy.h"
#include "qos_main.h"
#include "qos_msg.h"

#include "acl.h"




/**
 * @param   void *hash_key
 * @return  hash_value, ERRNO_SUCCESS means failure 
 */
static unsigned int compute(void *hash_key)
{
	if ( NULL == hash_key )
	{
		return ERRNO_FAIL;
	}
	
	return (unsigned int)hash_key;
}

/**
 * @param  		void *item
 * @param		void *hash_key
 * @return		ERRNO_SUCCESS, 1 means failure 
 */
static int compare(void *item, void *hash_key)
{
    struct hash_bucket *pbucket = (struct hash_bucket *)item;

    if ( NULL == item || NULL == hash_key )
    {
        return ERRNO_FAIL;
    }

    if ( pbucket->hash_key == hash_key )
    {
        return ERRNO_SUCCESS;
    }

	return ERRNO_FAIL; 
}

/**
 * @param  unsigned int size   ACL_TAB_MSIZE
 */
void acl_group_table_init(unsigned int size)
{
	hios_hash_init(&acl_group_table, size, compute, compare);
}


/**
 * @param		uchar *acl_name
 * @return		acl_item->hash_key,NULL means failure 
 */
int acl_group_add(uint32_t acl_num)
{
	struct hash_bucket *acl_item = NULL;
	struct acl_group *acl_node = NULL;
	int ret;
	
	if(0 == acl_num)	
	{
		QOS_LOG_ERR("Parameter illegal.\n");
		return ERRNO_PARAM_ILLEGAL;
	}

	acl_item = hios_hash_find(&acl_group_table, (void *)acl_num);
	if(NULL != acl_item)
	{
		return ERRNO_SUCCESS;
	}

	acl_item = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if(NULL == acl_item)
	{
		QOS_LOG_ERR("The memory is insufficient.\n");
		return ERRNO_FAIL;
	}

	acl_node = (struct acl_group *)XMALLOC(MTYPE_ACL_ENTRY, sizeof(struct acl_group));
	if(NULL == acl_node)
	{
		XFREE(MTYPE_HASH_BACKET, acl_item);
		acl_item = NULL;		
		return ERRNO_FAIL;
	}

	memset(acl_item, 0, sizeof(struct hash_bucket));
	memset(acl_node, 0, sizeof(struct acl_group));

	acl_node->acl_num = acl_num;
	acl_item->data = (void *)acl_node;
	acl_item->hash_key = (void *)acl_node->acl_num;
	
	ret = hios_hash_add(&acl_group_table, acl_item);
	if(ret != 0)
	{
		XFREE(MTYPE_ACL_ENTRY, acl_node);
		acl_node = NULL;
		XFREE(MTYPE_HASH_BACKET, acl_item);
		acl_item = NULL;
		return ERRNO_FAIL;
	}
	//ret =ipc_send_hal(acl_node , sizeof(struct acl_group),1, MODULE_ID_HAL, MODULE_ID_QOS, 
	//						IPC_TYPE_ACL, QOS_INFO_ACL_GROUP, IPC_OPCODE_ADD,acl_node->acl_num);
	ret = ipc_send_msg_n2(&acl_num, sizeof(struct acl_group), 1, MODULE_ID_HAL, MODULE_ID_QOS,
				IPC_TYPE_ACL, QOS_INFO_ACL_GROUP, IPC_OPCODE_ADD, acl_node->acl_num);

	if(ret != 0)
	{
		QOS_LOG_ERR("Failed to send add acl group to hal.\n");
		return ERRNO_IPC;
	}	
	
	return ERRNO_SUCCESS;
}



/**
 * @param 		uchar *acl_name
 * @return		ERRNO_SUCCESS, 1 means failure 
 */
int acl_group_delete(uint32_t acl_num)
{
	struct hash_bucket *acl_item = NULL;
	struct acl_group *acl_node = NULL;
	struct qos_if *qos_if = NULL;
	struct qos_policy *qos_policy = NULL;
	struct qos_policy *glo_policy = NULL;
	struct listnode *glo_node = NULL;
	struct listnode *node = NULL;	
	struct listnode *glo_next_node = NULL;
	struct listnode *next_node = NULL;
	struct hash_bucket *pbucket = NULL;
	struct car_t *acl_car = NULL;
	int cursor = 0;
	int ret = 0;

	struct list *plist = &qos_policy_global;
	
	acl_item = hios_hash_find(&acl_group_table, (void *)acl_num);
	if(acl_item == NULL)
	{
		QOS_LOG_ERR("Can not find this acl group.\n");
		return ERRNO_NOT_FOUND;
	}
	
	acl_node =(struct acl_group *)acl_item->data;
	
	//ret = ipc_send_hal(acl_node , sizeof(struct acl_group),1, MODULE_ID_HAL, MODULE_ID_QOS, 
	//						IPC_TYPE_ACL, QOS_INFO_ACL_GROUP, IPC_OPCODE_DELETE,acl_num);
	ret = ipc_send_msg_n2(&acl_num, sizeof(struct acl_group), 1, MODULE_ID_HAL, MODULE_ID_QOS,
				IPC_TYPE_ACL, QOS_INFO_ACL_GROUP, IPC_OPCODE_DELETE, acl_node->acl_num);

	if(ret == -1)
    {
		QOS_LOG_ERR("Failed to send delete acl group to hal.\n");
		return ERRNO_IPC;
    }

	/*删除group 对应全局策略*/	
	for ( ALL_LIST_ELEMENTS ( plist, glo_node, glo_next_node, glo_policy ) )
	{
		if(glo_policy->acl_num == acl_num)
		{
			if(glo_policy->type == POLICY_TYPE_CAR)
			{	
				/*car 模板引用计数减一*/
				acl_car = qos_car_profile_lookup(glo_policy->action.car_id);
				if(NULL == acl_car)
				{
					QOS_LOG_ERR("qos_car_profile_lookup error.\n");
					return ERRNO_FAIL;
				}
				acl_car->ref_cnt--;
			}
			XFREE ( MTYPE_QOS_ENTRY, glo_node->data );
			list_delete_node ( &qos_policy_global, glo_node );
		}
	}	

	/*删除group 对应接口策略*/	
	HASH_BUCKET_LOOP ( pbucket, cursor, qos_if_table)
	{
		if(NULL != pbucket->data)
		{
			qos_if = (struct qos_if *)pbucket->data;

			for ( ALL_LIST_ELEMENTS ( qos_if->pqos_policy, node, next_node, qos_policy ) )
			{
				if(qos_policy->acl_num == acl_num)
				{
					if(qos_policy->type == POLICY_TYPE_CAR)
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
					XFREE ( MTYPE_QOS_ENTRY, node->data );
					list_delete_node ( qos_if->pqos_policy, node );
					if (list_isempty (qos_if->pqos_policy))
					{	
						list_free(qos_if->pqos_policy);
						qos_if->pqos_policy =NULL;
						QOS_LOG_DBG("policy list_free\n\n");
					}
				}
			}		
		}	
	}
	
	hios_hash_delete(&acl_group_table, acl_item);
	XFREE(MTYPE_ACL_ENTRY,acl_item->data );	
	acl_item->data = NULL; 
	XFREE(MTYPE_HASH_BACKET, acl_item);
	acl_item = NULL;

	return ERRNO_SUCCESS;
}


/**
 * @param		uchar *acl_name
 * @return		struct acl_group * ,NULL means failure
 */
struct acl_group *acl_group_lookup(uint32_t acl_num)
{
	struct hash_bucket *acl_item = NULL;

	acl_item = hios_hash_find(&acl_group_table, (void *)acl_num);
	if(acl_item == NULL)
	{
		QOS_LOG_ERR("Can not find this acl group.\n");
		return NULL;
	}

	return ((struct acl_group *)(acl_item->data));
}

int acl_rule_size_get(uint8_t rule_type)
{
	int size = 0;
	 switch (rule_type)
    {
    case ACL_TYPE_MAC:
	case ACL_TYPE_MAC_IP_MIX:
        size = sizeof ( struct mac_rule );
        break;
    case ACL_TYPE_IP:
        size = sizeof ( struct ip_rule );
        break;
    case ACL_TYPE_IP_EXTEND:
        size = sizeof ( struct ip_extend_rule );
        break;
    default:
        return ERRNO_FAIL;
    }

	return size;
}

/**
 * @param   	void *prule 
 * @param		uint8_t rule_type
 * @param		uchar *acl_name
 * @return		ERRNO_SUCCESS ,other means failure
 */
int acl_rule_add(void *prule, uint8_t rule_type, uint32_t acl_num)
{
	struct hash_bucket *item = NULL;
	struct acl_group *acl_node = NULL;	
	struct listnode *node = NULL;
	struct rule_node *node_tmp = NULL;
	struct rule_node *p_node = NULL;
	struct rule_node acl_rule;
	struct mac_rule mac_temp;
	int ret = 0;
	int size = 0;
	
	item = hios_hash_find(&acl_group_table, (void *)acl_num);
	if(NULL == item)
	{
		QOS_LOG_ERR("Can not find this acl group.\n");
		return ERRNO_FAIL;
	}	
	
	acl_node = (struct acl_group *)item->data;
	size = acl_rule_size_get( rule_type);

	/*rulelist 为空的情况*/
	if (list_isempty (&(acl_node->rulelist)))
	{
		p_node = (struct rule_node *)XMALLOC(MTYPE_ACL_ENTRY, sizeof(struct rule_node));
		if(NULL == p_node)
		{
			QOS_LOG_ERR("The memory is insufficient.\n");
			return ERRNO_FAIL;
		}
		
		memset(p_node, 0, sizeof(struct rule_node));
		memcpy((void *)&(p_node->rule), prule, size);
		p_node->type = rule_type;
		p_node->acl_num = acl_num;

//		ret = ipc_send_hal_wait_ack(p_node , sizeof(struct rule_node),1, MODULE_ID_HAL, MODULE_ID_QOS, 
//							IPC_TYPE_ACL, QOS_INFO_ACL_RULE, IPC_OPCODE_ADD,p_node->acl_num);
		ret = qos_ipc_send_and_wait_ack(p_node , sizeof(struct rule_node),1, MODULE_ID_HAL, MODULE_ID_QOS, 
									IPC_TYPE_ACL, QOS_INFO_ACL_RULE, IPC_OPCODE_ADD,p_node->acl_num);
		if(ret)
	    {
	    	XFREE(MTYPE_ACL_ENTRY, p_node);
			p_node = NULL;
			QOS_LOG_ERR("ipc_send_hal_wait_ack error.\n");
			return ERRNO_HAL;
	    }
		
		listnode_add (&(acl_node->rulelist),(void *)p_node);
		return ERRNO_SUCCESS;
	}

	/*rulelist 存在节点的情况*/
	for (node = acl_node->rulelist.head ; node; node = node->next)
	{
		node_tmp = listgetdata(node);
		memcpy(&mac_temp, prule, sizeof(struct mac_rule));

		/*rule 从小到大排序*/
		if(node_tmp->rule.mac_acl.ruleid > mac_temp.ruleid)
		{
			p_node = (struct rule_node *)XMALLOC(MTYPE_ACL_ENTRY, sizeof(struct rule_node));
			if(NULL == p_node)
			{
				QOS_LOG_ERR("The memory is insufficient.\n");
				return ERRNO_FAIL;
			}
			
			memset(p_node, 0, sizeof(struct rule_node));
			memcpy((void *)&(p_node->rule), prule, size);
			p_node->type = rule_type;
			p_node->acl_num = acl_num;
			
			//ret = ipc_send_hal_wait_ack(p_node , sizeof(struct rule_node),1, MODULE_ID_HAL, MODULE_ID_QOS, 
			//				IPC_TYPE_ACL, QOS_INFO_ACL_RULE, IPC_OPCODE_ADD,p_node->acl_num);
			ret = qos_ipc_send_and_wait_ack(p_node , sizeof(struct rule_node),1, MODULE_ID_HAL, MODULE_ID_QOS, 
							IPC_TYPE_ACL, QOS_INFO_ACL_RULE, IPC_OPCODE_ADD,p_node->acl_num);
			if(ret)
		    {
		    	XFREE(MTYPE_ACL_ENTRY, p_node);
				p_node = NULL;
				QOS_LOG_ERR("ipc_send_hal_wait_ack error.\n");
				return ERRNO_HAL;
		    }
			
			listnode_add_prev (&(acl_node->rulelist), node, p_node);
			return ERRNO_SUCCESS;
		}

		/*rule  更新*/
		if(node_tmp->rule.mac_acl.ruleid  == mac_temp.ruleid)
		{	
			memset(&acl_rule, 0, size);
			memcpy(&acl_rule.rule, prule, size);
			acl_rule.type = rule_type;
			acl_rule.acl_num = acl_num;
			
			//ret = ipc_send_hal_wait_ack(&acl_rule , sizeof(struct rule_node),1, MODULE_ID_HAL, MODULE_ID_QOS, 
			//				IPC_TYPE_ACL, QOS_INFO_ACL_RULE, IPC_OPCODE_UPDATE ,acl_rule.acl_num);
			ret = qos_ipc_send_and_wait_ack(&acl_rule , sizeof(struct rule_node),1, MODULE_ID_HAL, MODULE_ID_QOS, 
							IPC_TYPE_ACL, QOS_INFO_ACL_RULE, IPC_OPCODE_UPDATE ,acl_rule.acl_num);
			if(ret)
		    {
				QOS_LOG_ERR("ipc_send_hal_wait_ack error.\n");
				return ERRNO_HAL;
		    }
			
			memcpy(node_tmp, &acl_rule, sizeof(struct rule_node));
			return ERRNO_SUCCESS;
		}
	}

	/*添加到节点末尾*/
	if (node == NULL)
	{
		p_node = (struct rule_node *)XMALLOC(MTYPE_ACL_ENTRY, sizeof(struct rule_node));
		if(NULL == p_node)
		{
			QOS_LOG_ERR("The memory is insufficient.\n");
			return ERRNO_FAIL;
		}
		
		memset(p_node, 0, sizeof(struct rule_node));
		memcpy((void *)&(p_node->rule), prule, size);
		p_node->type = rule_type;
		p_node->acl_num = acl_num;
		
		//ret = ipc_send_hal_wait_ack(p_node , sizeof(struct rule_node),1, MODULE_ID_HAL, MODULE_ID_QOS, 
		//					IPC_TYPE_ACL, QOS_INFO_ACL_RULE, IPC_OPCODE_ADD,p_node->acl_num);
		ret = qos_ipc_send_and_wait_ack(p_node , sizeof(struct rule_node),1, MODULE_ID_HAL, MODULE_ID_QOS, 
							IPC_TYPE_ACL, QOS_INFO_ACL_RULE, IPC_OPCODE_ADD,p_node->acl_num);
		if(ret)
	    {
	    	XFREE(MTYPE_ACL_ENTRY, p_node);
			p_node = NULL;
			QOS_LOG_ERR("ipc_send_hal_wait_ack error.\n");
			return ERRNO_HAL;
	    }
		
		listnode_add (&(acl_node->rulelist),(void *)p_node);
	}
	
	return ERRNO_SUCCESS;
}


/**
 * @param		uint16_t ruleid
 * @param		uchar *acl_name
 * @return		ERRNO_SUCCESS ,1 means failure
 */
int acl_rule_delete(uint16_t ruleid,  uint32_t acl_num)
{
	struct hash_bucket *item = NULL;
	struct acl_group *acl_node = NULL;
	struct listnode *node = NULL;
	struct rule_node *node_tmp = NULL;
	int ret = 0;

	item = hios_hash_find(&acl_group_table, (void *)acl_num);
	if(NULL == item) 
	{
		QOS_LOG_ERR("Can not find this acl group.\n");
		return ERRNO_FAIL;
	}
	
	acl_node = (struct acl_group *)item->data;

	if(acl_node->rulelist.head == NULL)
	{
		QOS_LOG_ERR("Can not find this acl rule.\n");
		return ERRNO_NOT_FOUND;
	}

	for (node = acl_node->rulelist.head ; node; node = node->next)
	{	
		node_tmp = listgetdata(node);
		if (ruleid == node_tmp->rule.mac_acl.ruleid)
		{
			ret =ipc_send_msg_n2(&ruleid , sizeof(struct rule_node),1, MODULE_ID_HAL, MODULE_ID_QOS, 
										IPC_TYPE_ACL, QOS_INFO_ACL_RULE, IPC_OPCODE_DELETE,ruleid);
		    if(ret)
		    {
				QOS_LOG_ERR("ipc_send_hal error.\n");
				return ERRNO_IPC;
		    }
			
			XFREE ( MTYPE_ACL_ENTRY, node->data );
			list_delete_node (&acl_node->rulelist, node);
			
		  	return ERRNO_SUCCESS;
		}
	}

	return ERRNO_NOT_FOUND;
}


struct rule_node *acl_rule_lookup(uint16_t ruleid, uint32_t acl_num)
{
	struct acl_group *acl_node = NULL;
	struct listnode *node = NULL;
	struct rule_node *rule_node = NULL;

	acl_node = acl_group_lookup(acl_num);
	if(NULL == acl_node)
	{
		QOS_LOG_ERR("Can not find this acl group.\n");
		return NULL;
	}

	if(acl_node->rulelist.head != NULL)
	{
		for (node = acl_node->rulelist.head ; node; node = node->next)
		{
			rule_node = listgetdata(node);
			if(ruleid == rule_node->rule.mac_acl.ruleid)
			{
				return rule_node;
			}
		}
	}
	
	return NULL;
}


int acl_rule_get_bulk ( uint32_t acl_num, uint16_t ruleid, struct rule_node *prule)
{
	int msg_num = IPC_MSG_LEN/sizeof(struct rule_node);
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
	struct listnode  *pnode = NULL;
	struct acl_group *pacl = NULL;
	struct rule_node *prule_node = NULL;
	int data_num = 0;
	int cursor = 0;
	int flag = 0;
	int val = 0;

    if (( acl_num == 0 )&&( ruleid == 0))
    {
    	HASH_BUCKET_LOOP(pbucket, cursor, acl_group_table)
        {
            pacl = (struct acl_group *)pbucket->data;
            if (NULL == pacl)
            {
                continue;
            }

			for (ALL_LIST_ELEMENTS_RO(&pacl->rulelist, pnode, prule_node))
			{
				memcpy(&prule[data_num++], prule_node, sizeof(struct rule_node));
				
				if (data_num == msg_num)
	            {
	                return data_num;
	            }
			}          
        }
    }
    else if( acl_num != 0 )
    {
        pbucket = hios_hash_find(&acl_group_table, (void *)acl_num);
        
        if (NULL == pbucket)
        {
        	val = acl_group_table.compute_hash((void *)acl_num)% HASHTAB_SIZE;
	        
	        if (acl_group_table.buckets[val] != NULL)
	        {
	        	pbucket = acl_group_table.buckets[val];
	        }
	        else
	        {
	        	for (++val; val<HASHTAB_SIZE; ++val)
	            {
	                if (NULL != acl_group_table.buckets[val])
	                {
	                    pbucket = acl_group_table.buckets[val];
						ruleid = 0;
	                }
	            }
	        }            
        } 
        while (pbucket)
        {      
            pacl = (struct acl_group *)(pbucket->data);

			if(ruleid != 0)
			{
				for (ALL_LIST_ELEMENTS_RO(&pacl->rulelist, pnode, prule_node))
				{
				   	if(prule_node->rule.mac_acl.ruleid == ruleid)
				   	{
						flag = 1;
					}
					if((prule_node->rule.mac_acl.ruleid > ruleid)&&(flag == 1))
					{
						memcpy(&prule[data_num++], prule_node, sizeof(struct rule_node));
										
						if (data_num == msg_num)
						{
							return data_num;
						}
					}
				}
			}
			
			if((ruleid == 0)||(flag == 0))
			{
				for (ALL_LIST_ELEMENTS_RO(&pacl->rulelist, pnode, prule_node))
				{
					
					memcpy(&prule[data_num++], prule_node, sizeof(struct rule_node));
					
					if (data_num == msg_num)
		            {
		                return data_num;
		            }
				}
			}
			
			ruleid = 0;
			flag = 0;
			
			pnext = hios_hash_next_cursor(&acl_group_table, pbucket);
            if ((NULL == pnext) || ((NULL != pnext)&&(NULL == pnext->data)))
            {
                break;
            }

			pbucket = pnext;
        }
    }  

    return data_num;
}


