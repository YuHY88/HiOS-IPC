#include <stdlib.h>
#include <string.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/command.h>
#include <lib/vty.h>
#include <lib/log.h>
#include <lib/zassert.h>
#include <lib/errcode.h>
#include <lib/msg_ipc_n.h>
#include <lib/ifm_common.h>

#include <qos/qos_if.h>
#include <qos/qos_mirror.h>
#include <qos/qos_main.h>
#include <qos/qos_msg.h>
#include <lib/msg_ipc_n.h>




struct mirror_group mgroup[MIRROR_GROUP_NUM];

void qos_mirror_init ( void )
{
    uint8_t group_id;
    for ( group_id = 0; group_id < MIRROR_GROUP_NUM; group_id++ )
    {
    	memset(&mgroup[group_id] ,0 ,sizeof(struct mirror_group));
    }

    return ;
}

int qos_mirror_delete_group(uint8_t group_id)
{
	struct hash_bucket *pbucket = NULL;
	struct listnode *node = NULL;
	struct listnode *nextnode = NULL;
	struct qos_if *pif = NULL;
	uint32_t *pifindex = NULL;
	int cursor = 0;
	int ret = 0;

	if(mgroup[group_id].id != group_id)
	{
		QOS_LOG_ERR("can not find this group.\n");
		return ERRNO_FAIL;
	}
#if 0
	ret = ipc_send_hal_wait_ack( &group_id , sizeof(uint8_t),1, MODULE_ID_HAL, MODULE_ID_QOS,
								IPC_TYPE_QOS, QOS_INFO_MIRROR_GROUP, IPC_OPCODE_DELETE,0);
#else
	ret = qos_ipc_send_and_wait_ack( &group_id , sizeof(uint8_t),1, MODULE_ID_HAL, MODULE_ID_QOS,
								IPC_TYPE_QOS, QOS_INFO_MIRROR_GROUP, IPC_OPCODE_DELETE,0);
#endif

	if(ret)
    {
		QOS_LOG_ERR("delete group failed.\n");
		return ERRNO_HAL;
    }

	/*删除接口mirror_to group 的配置*/
	HASH_BUCKET_LOOP ( pbucket, cursor, qos_if_table)
	{
		if(NULL != pbucket->data)
		{
			pif = (struct qos_if *)pbucket->data;
			if((NULL != pif->if_mirror)&&(pif->if_mirror->mirror_group == group_id))
			{
				XFREE(MTYPE_QOS_ENTRY, pif->if_mirror);
				pif->if_mirror = NULL;
			}
		}
	}
	/*删除mirror group 配置*/
	if(NULL != mgroup[group_id].pport_list)
	{
		for ( ALL_LIST_ELEMENTS ( mgroup[group_id].pport_list, node, nextnode, pifindex ) )
		{
			listnode_delete (mgroup[group_id].pport_list, pifindex);
			XFREE(MTYPE_QOS_ENTRY, pifindex);
			pifindex = NULL;
		}

		list_free(mgroup[group_id].pport_list);
		mgroup[group_id].pport_list = NULL;
	}

	mgroup[group_id].id = 0;

	return ERRNO_SUCCESS;
}


int qos_mirror_group_add_port(uint8_t group_id, uint32_t mirror_if)
{
	struct listnode *node = NULL;
	uint32_t *pifindex = NULL;
	int ret;

	if(NULL == mgroup[group_id].pport_list)
	{
		mgroup[group_id].pport_list = (struct list *)list_new();
		if(NULL == mgroup[group_id].pport_list)
		{
			QOS_LOG_ERR("The memory is insufficient.\n");
			return ERRNO_FAIL;
		}
	}

	/*检查监视口是否添加过*/
	for ( ALL_LIST_ELEMENTS_RO ( mgroup[group_id].pport_list, node, pifindex ) )
	{
		if(*pifindex == mirror_if)
		{
			QOS_LOG_ERR("The mirror interface has existed.\n");
			return ERRNO_EXISTED;
		}
	}

	/*软件支持最多添加4个监视口*/
	if(mgroup[group_id].pport_list->count >= MIRROR_GROUP_PORT_MAX_NUM)
	{
		QOS_LOG_ERR("Add mirror interface out of range.\n");
		return ERRNO_OVERSIZE;
	}
	#if 0
	ret = ipc_send_hal_wait_ack( &mirror_if , sizeof(uint32_t),1, MODULE_ID_HAL, MODULE_ID_QOS,
								IPC_TYPE_QOS, QOS_INFO_MIRROR_GROUP_PORT, IPC_OPCODE_ADD,group_id);
	#else
	ret = qos_ipc_send_and_wait_ack( &mirror_if , sizeof(uint32_t),1, MODULE_ID_HAL, MODULE_ID_QOS,
								IPC_TYPE_QOS, QOS_INFO_MIRROR_GROUP_PORT, IPC_OPCODE_ADD,group_id);
	#endif
	if(ret)
    {
		QOS_LOG_ERR("mirror group add port failed.\n");
		return ERRNO_HAL;
    }

	pifindex = (uint32_t *)XMALLOC(MTYPE_QOS_ENTRY, sizeof(uint32_t));
	if(NULL == pifindex)
	{
		QOS_LOG_ERR("The memory is insufficient.\n");
		return ERRNO_FAIL;
	}

	*pifindex = mirror_if;
	listnode_add (mgroup[group_id].pport_list,(void *)pifindex);

	return ERRNO_SUCCESS;
}

int qos_mirror_group_delete_port(uint8_t  group_id, uint32_t mirror_if)
{
	struct listnode *node = NULL;
	uint32_t *pifindex = NULL;
	int ret;

	if(NULL == mgroup[group_id].pport_list)
	{
		QOS_LOG_ERR("Not find this mirror interface.\n");
		return ERRNO_NOT_FOUND;
	}

	for ( ALL_LIST_ELEMENTS_RO ( mgroup[group_id].pport_list, node, pifindex ) )
	{
		if(*pifindex == mirror_if)
		{
			#if 0
			ret = ipc_send_hal_wait_ack( &mirror_if , sizeof(uint32_t),1, MODULE_ID_HAL, MODULE_ID_QOS,
									IPC_TYPE_QOS, QOS_INFO_MIRROR_GROUP_PORT, IPC_OPCODE_DELETE,group_id);
			#else
			ret = qos_ipc_send_and_wait_ack( &mirror_if , sizeof(uint32_t),1, MODULE_ID_HAL, MODULE_ID_QOS,
									IPC_TYPE_QOS, QOS_INFO_MIRROR_GROUP_PORT, IPC_OPCODE_DELETE,group_id);
			#endif
			if(ret)
			{
				QOS_LOG_ERR("group delete port failed.\n");
				return ERRNO_HAL;
			}

			listnode_delete (mgroup[group_id].pport_list, pifindex);
			XFREE(MTYPE_QOS_ENTRY, pifindex);
			pifindex = NULL;

			if (list_isempty (mgroup[group_id].pport_list))
			{
				list_free(mgroup[group_id].pport_list);
				mgroup[group_id].pport_list = NULL;
			}

			return ERRNO_SUCCESS;
		}
	}

	return ERRNO_NOT_FOUND;
}


int qos_if_add_mirror_group (uint32_t ifindex, uint8_t group_id, enum QOS_DIR direct)
{
	struct qos_if *qos_if = NULL;
	struct qos_if_mirror mirror;
	int ret = 0;

	if( mgroup[group_id].id != group_id)
	{
		QOS_LOG_ERR("Can not find group.\n");
		return ERRNO_FAIL;
	}

	qos_if = qos_if_lookup(ifindex);
	if(NULL == qos_if)
	{
		qos_if = qos_if_create(ifindex);
		if(NULL == qos_if)
		{
			QOS_LOG_ERR("qos_if_create failed.\n");
			return ERRNO_FAIL;
		}
	}

	if((NULL != qos_if->if_mirror)&&((qos_if->if_mirror->mirror_group != 0)||(qos_if->if_mirror->mirror_if != 0)))
	{
		return ERRNO_EXISTED;
	}

	if(NULL == qos_if->if_mirror)
	{
		memset(&mirror, 0, sizeof(struct qos_if_mirror));
		mirror.ifindex = ifindex;
		mirror.direct = direct;
		mirror.mirror_group = group_id;
		mirror.type = QOS_INFO_MIRROR_TO_GROUP;
		#if 0
		ret = ipc_send_hal_wait_ack(&mirror , sizeof(struct qos_if_mirror),1, MODULE_ID_HAL, MODULE_ID_QOS,
								IPC_TYPE_QOSIF, QOS_INFO_MIRROR_TO_GROUP, IPC_OPCODE_ADD,0);
		#else
		ret = qos_ipc_send_and_wait_ack(&mirror , sizeof(struct qos_if_mirror),1, MODULE_ID_HAL, MODULE_ID_QOS,
								IPC_TYPE_QOSIF, QOS_INFO_MIRROR_TO_GROUP, IPC_OPCODE_ADD,0);
		#endif
		if(ret)
	    {
			QOS_LOG_ERR("mirror group add failed.\n");
			return ERRNO_HAL;
	    }

		qos_if->if_mirror = (struct qos_if_mirror *)XCALLOC(MTYPE_QOS_ENTRY, sizeof(struct qos_if_mirror));
		if(qos_if->if_mirror == NULL)
		{
			QOS_LOG_ERR("Fail to malloc for new qos_if.\n");
			return ERRNO_SUCCESS;
		}

		memcpy(qos_if->if_mirror, &mirror, sizeof(struct qos_if_mirror));

		return ERRNO_SUCCESS;
	}

	return ERRNO_FAIL;
}


int qos_if_delete_mirror_group (uint32_t ifindex)
{
	struct qos_if *qos_if = NULL;
	int ret;

	qos_if = qos_if_lookup(ifindex);
	if(NULL == qos_if)
	{
		QOS_LOG_ERR("qos_if_lookup failed,can not find qos_if.\n");
		return ERRNO_NOT_FOUND;
	}

	if((NULL != qos_if->if_mirror) &&(qos_if->if_mirror->mirror_group != 0)&&(qos_if->if_mirror->mirror_if == 0))
	{
		#if 0
		ret = ipc_send_hal_wait_ack(&ifindex , sizeof(uint32_t),1, MODULE_ID_HAL, MODULE_ID_QOS,
					IPC_TYPE_QOSIF, QOS_INFO_MIRROR_TO_GROUP, IPC_OPCODE_DELETE,qos_if->if_mirror->mirror_group);
		#else
		ret = qos_ipc_send_and_wait_ack(&ifindex , sizeof(uint32_t),1, MODULE_ID_HAL, MODULE_ID_QOS,
					IPC_TYPE_QOSIF, QOS_INFO_MIRROR_TO_GROUP, IPC_OPCODE_DELETE,qos_if->if_mirror->mirror_group);
		#endif
		if(ret)
	    {
			QOS_LOG_ERR("mirror group delete failed.\n");
			return ERRNO_HAL;
	    }

		XFREE(MTYPE_QOS_ENTRY, qos_if->if_mirror);
		qos_if->if_mirror = NULL;

		return ERRNO_SUCCESS;
	}

	return ERRNO_NOT_FOUND;
}


/* 接口mirror_to mirror_if操作 */
int qos_if_add_mirror_port(uint32_t ifindex, uint32_t mirror_if, enum QOS_DIR direct)
{
	struct qos_if *qos_if = NULL;
	struct qos_if_mirror mirror;
	int ret = 0;

	qos_if = qos_if_lookup(ifindex);
	if(NULL == qos_if)
	{
		qos_if = qos_if_create(ifindex);
		if(NULL == qos_if)
		{
			QOS_LOG_ERR("qos_if_create failed.\n");
			return ERRNO_FAIL;
		}
	}

	if((NULL != qos_if->if_mirror)&&((qos_if->if_mirror->mirror_group != 0)||(qos_if->if_mirror->mirror_if != 0)))
	{
		return ERRNO_EXISTED;
	}

	if(NULL == qos_if->if_mirror)
	{
		memset(&mirror, 0, sizeof(struct qos_if_mirror));
		mirror.ifindex = ifindex;
		mirror.direct = direct;
		mirror.mirror_if = mirror_if;
		mirror.type = QOS_INFO_MIRROR_TO_PORT;

		#if 0
		ret = ipc_send_hal_wait_ack(&mirror , sizeof(struct qos_if_mirror),1, MODULE_ID_HAL, MODULE_ID_QOS,
								IPC_TYPE_QOSIF, QOS_INFO_MIRROR_TO_PORT, IPC_OPCODE_ADD,0);
		#else
		ret = qos_ipc_send_and_wait_ack(&mirror , sizeof(struct qos_if_mirror),1, MODULE_ID_HAL, MODULE_ID_QOS,
								IPC_TYPE_QOSIF, QOS_INFO_MIRROR_TO_PORT, IPC_OPCODE_ADD,0);
		#endif
		if(ret)
	    {
			QOS_LOG_ERR("add mirror port failed.\n");
			return ERRNO_HAL;
	    }

		qos_if->if_mirror = (struct qos_if_mirror *)XCALLOC(MTYPE_QOS_ENTRY, sizeof(struct qos_if_mirror));
		if(qos_if->if_mirror == NULL)
		{
			QOS_LOG_ERR("Fail to malloc for new qos_if.\n");
			return ERRNO_SUCCESS;
		}

		memcpy(qos_if->if_mirror, &mirror, sizeof(struct qos_if_mirror));

		return ERRNO_SUCCESS;
	}

	return ERRNO_FAIL;
}


int qos_if_delete_mirror_port(uint32_t ifindex, uint32_t mirror_if)
{
	struct qos_if *qos_if = NULL;
	int ret;

	qos_if = qos_if_lookup(ifindex);
	if(NULL == qos_if)
	{
		QOS_LOG_ERR("qos_if_lookup failed,can not find qos_if.\n");
		return ERRNO_NOT_FOUND;
	}

	if((NULL != qos_if->if_mirror) &&(qos_if->if_mirror->mirror_group == 0)&&(qos_if->if_mirror->mirror_if == mirror_if))
	{
		#if 0
		ret = ipc_send_hal_wait_ack(&ifindex , sizeof(uint32_t),1, MODULE_ID_HAL, MODULE_ID_QOS,
								IPC_TYPE_QOSIF, QOS_INFO_MIRROR_TO_PORT, IPC_OPCODE_DELETE,mirror_if);
		#else
		ret = qos_ipc_send_and_wait_ack(&ifindex , sizeof(uint32_t),1, MODULE_ID_HAL, MODULE_ID_QOS,
								IPC_TYPE_QOSIF, QOS_INFO_MIRROR_TO_PORT, IPC_OPCODE_DELETE,mirror_if);
		#endif
		if(ret)
	    {
			QOS_LOG_ERR("mirror port delete failed.\n");
			return ERRNO_HAL;
	    }

		XFREE(MTYPE_QOS_ENTRY, qos_if->if_mirror);
		qos_if->if_mirror = NULL;

		return ERRNO_SUCCESS;
	}

	return ERRNO_NOT_FOUND;
}


 static int qos_mirror_return_str (int ret,struct vty *vty)
{
	if(ret == ERRNO_SUCCESS )
	{
		vty_out(vty, " Success to operate with the mirror config.%s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}
	if(ret == ERRNO_FAIL)
	{
		vty_error_out(vty, "Fail operate with  the mirror config.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(ret == ERRNO_EXISTED)
	{
		vty_warning_out(vty, "Configuration already exists.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(ret == ERRNO_NOT_FOUND)
	{
		vty_warning_out(vty, "Can not find this port.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(ret == ERRNO_HAL)
	{
		vty_error_out(vty, "Failed to support the resource,please check out %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(ret == ERRNO_OVERSIZE)
	{
		vty_error_out(vty, "Increase the number of monitor ports exceeded.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_WARNING;
}


char* qos_mirror_direct_parse_str(enum QOS_DIR direct)
{
  switch(direct)
  {
  	case QOS_DIR_INGRESS:
		return (char *)"ingress";
	case QOS_DIR_EGRESS:
		return (char *)"egress";
	case QOS_DIR_BIDIR:
		return (char *)"all";
	default:
		return (char *)"unknow";
  }
  return (char *)"unknow";
}


static int qos_mirror_config_write (struct vty *vty)
{
	uint8_t group_id;
	uint32_t *pifindex=  NULL;
	struct listnode *node = NULL;
	char ifname[IFNET_NAMESIZE];

    for ( group_id = 1; group_id < MIRROR_GROUP_NUM; group_id++ )
    {
    	if(mgroup[group_id].id != 0)
    	{
			vty_out(vty, "mirror-group %d%s", group_id, VTY_NEWLINE);
			for ( ALL_LIST_ELEMENTS_RO ( mgroup[group_id].pport_list, node, pifindex ) )
			{
				ifm_get_name_by_ifindex ( *pifindex,ifname);
				vty_out(vty, " mirror-to interface %-15s%s",ifname,VTY_NEWLINE);
			}
    	}
    }

	return ERRNO_SUCCESS;
}

/************************************************
 * Function: qos_mirror_group_get_bulk
 * Input:
 *      index: index group_id
 *		type:  mirror profile type
 * Output:
 *      mirror_array: mirror_group data
 * Return:
 *      data num
 * Description:
 *      Get qos mirror profile data.
 ************************************************/
int qos_mirror_group_get_bulk ( uint32_t index, uint8_t group_id, struct qos_mirror_snmp *pmirror)
{
	int msg_num = IPC_MSG_LEN/sizeof(struct qos_mirror_snmp);
	struct qos_mirror_snmp mirror;
	struct listnode  *pnode = NULL;
	uint32_t *iindex = NULL;
	uint8_t group = group_id;
	int data_num = 0;
	int flag = 0; //用来判断是否找到相应节点

	
	if (index == 0) 	  //首次传参索引值为0，从链表的初始值返回数值
	{
		for(group=1; group<MIRROR_GROUP_NUM; group++)
		{
			for( ALL_LIST_ELEMENTS_RO( mgroup[group].pport_list, pnode, iindex ) )	//查询链表，获取mirror_group信息
			{
				memset(&mirror,0,sizeof(struct qos_mirror_snmp));
				//将节点信息存入结构体并返回
				mirror.id = group;
				mirror.ifindex= *iindex;
				memcpy(&pmirror[data_num++], &mirror, sizeof(struct qos_mirror_snmp));
				if (data_num == msg_num)
				{
					return data_num;
				}
			}			
		}
	}
	else	   //传参索引值不为0，从索引对应值的下一个节点返回数值
	{	
		for( group = group_id; group<MIRROR_GROUP_NUM; group++)  //依次添加下一个group_id对应的节点
		{
			for( ALL_LIST_ELEMENTS_RO( mgroup[group].pport_list, pnode, iindex ))
			{
				if ((index == *iindex)&&(flag == 0))   //第一次找到索引值
				{		
					flag = 1;
					continue;
				}

				if((flag == 1) || ((group > group_id)&&(flag == 0))) 	   //找到对应节点后依次添加
				{
					//将节点信息存入结构体并返回
					memset(&mirror,0,sizeof(struct qos_mirror_snmp));
					mirror.id = group;
					mirror.ifindex = *iindex;
					memcpy(&pmirror[data_num++], &mirror, sizeof(struct qos_mirror_snmp));
					if (data_num == msg_num)
					{
						return data_num;
					}
				}
			}
		}
	}
	return data_num;
}



/************************************************
 * Function: qos_mirror_port_get_bulk
 * Input:
 *      index: qos_if_mirror mirror-group
 *		type:  mirror profile type
 * Output:
 *      mirror_array: qos_if_mirror data
 * Return:
 *      data num
 * Description:
 *      Get qos mirror profile data.
 ************************************************/
int qos_mirror_port_get_bulk ( uint32_t index, struct qos_if_mirror *pif_mirror)
{
	int msg_num = IPC_MSG_LEN/sizeof(struct qos_if_mirror);
	struct qos_if *qos_if = NULL;
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
    int data_num = 0;
	int cursor;
	int val = 0;

    if ( index == 0 )
	{
		HASH_BUCKET_LOOP(pbucket, cursor, qos_if_table)
		{
			qos_if = (struct qos_if *)pbucket->data;
			if((NULL == qos_if)||((NULL != qos_if)&&(NULL == qos_if->if_mirror)))
			{
				continue;
			}

			memcpy(&pif_mirror[data_num++], qos_if->if_mirror, sizeof(struct qos_if_mirror));
			if (data_num == msg_num)
			{
				return data_num;
			}
		}
	}
    else
	{	
		pbucket = hios_hash_find(&qos_if_table, (void *)index);
        
        if (NULL == pbucket)
        {
        	val = (qos_if_table.compute_hash((void *)index)) % HASHTAB_SIZE;
	        
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
	                    break;
	                }
	            }
	        }
        }

        while (pbucket)
        {
            pnext = hios_hash_next_cursor(&qos_if_table, pbucket);
            if ((NULL == pnext) || (NULL == pnext->data))
            {
                break;
            }
            pbucket = pnext;
            qos_if = (struct qos_if *)pbucket->data;
			if(qos_if->if_mirror != NULL)
			{
	            
	            memcpy(&pif_mirror[data_num++], qos_if->if_mirror, sizeof(struct qos_if_mirror));
				if (data_num == msg_num)
				{
					return data_num;
				}
			}
        }
	}

    return data_num;
}

int qos_if_mirror_already_occupy(  uint32_t mirror_if, uint8_t group_id, uint32_t index)             
{
	struct hash_bucket *pbucket = NULL;
	struct qos_if *qos_if = NULL;
	struct listnode *node = NULL;
	uint32_t *pifindex = NULL;
	int cursor;

	if((0 == mirror_if) && (0 == group_id) && (0 == index))
	{
		return ERRNO_FAIL;
	}
	
	if(0 != mirror_if)          //监视口是其他端口的镜像口
	{
		qos_if = qos_if_lookup(mirror_if);
			
		if(NULL == qos_if)
		{	
			return ERRNO_NOT_FOUND;
		}
		
		if((NULL != qos_if->if_mirror)&&((0 != qos_if->if_mirror->mirror_group)||(0 != qos_if->if_mirror->mirror_if)))
		{
			return ERRNO_EXISTED;
		}
	}
	else if(0 != index)                       //是某个组的镜像端口或者监视口是其他端口的镜像口
	{
		HASH_BUCKET_LOOP(pbucket, cursor, qos_if_table)    //哈希表中查找添加的组和接口
		{
			qos_if = (struct qos_if *)pbucket->data;
			if((NULL == qos_if)||((NULL != qos_if)&&(NULL == qos_if->if_mirror)))
			{
				continue;
			}
			
			if(qos_if->if_mirror->mirror_if == index)   //镜像口是其他端口的监视口
			{
				return ERRNO_EXISTED;
			}
			
			if(group_id == 0)                     //镜像口是其他监视组的端口
			{
				if((NULL != qos_if->if_mirror)&&((0 != qos_if->if_mirror->mirror_group)))
				{
					group_id = qos_if->if_mirror->mirror_group;
				}
				if(mgroup[group_id].id != 0)
				{
		    		for ( ALL_LIST_ELEMENTS_RO ( mgroup[group_id].pport_list, node, pifindex ) )
					{
						if(*pifindex == index)
						{
							return ERRNO_EXISTED;
						}
					} 	
				}
				
			}
		}
	}
	else    //监视组中的接口是其他接口的镜像口
	{ 
		if(mgroup[group_id].id != 0)
		{
			for ( ALL_LIST_ELEMENTS_RO ( mgroup[group_id].pport_list, node, pifindex ) )
			{
				if(NULL != pifindex)
				{
					qos_if = qos_if_lookup(*pifindex);
					if((NULL != qos_if) && (NULL != qos_if->if_mirror) && (0 != qos_if->if_mirror->mirror_if ))
					{
						return ERRNO_EXISTED;
					}
				}
			}
		}
	}
	
	return ERRNO_SUCCESS;
}


struct cmd_node qos_mirror_node =
{
  QOS_MIRROR_NODE,
  "%s(config-mirror)# ",
  1,
};

DEFUN(qos_mirror_group,
		qos_mirror_group_cmd,
		"mirror-group <1-3>",
		"Qos mirror group\n"
		"Qos mirror group id\n")
{
	uint8_t group_id = 0;
	int ret = 0;

	group_id = atoi(argv[0]);
	#if 0
	ret = ipc_send_hal_wait_ack( &group_id , sizeof(uint8_t),1, MODULE_ID_HAL, MODULE_ID_QOS,
								IPC_TYPE_QOS, QOS_INFO_MIRROR_GROUP, IPC_OPCODE_ADD,group_id);
	#else
	ret = qos_ipc_send_and_wait_ack( &group_id , sizeof(uint8_t),1, MODULE_ID_HAL, MODULE_ID_QOS,
								IPC_TYPE_QOS, QOS_INFO_MIRROR_GROUP, IPC_OPCODE_ADD,group_id);
	#endif
	if(ret)
    {
		QOS_LOG_ERR("Error:ipc send hal wait ack failed.\n");
		return ERRNO_HAL;
    }

	mgroup[group_id].id = group_id;

	vty->index = (void *)(uint32_t)group_id;
	vty->node = QOS_MIRROR_NODE;

	return CMD_SUCCESS;
}

DEFUN(no_qos_mirror_group,
		no_qos_mirror_group_cmd,
		"no mirror-group <1-3>",
		NO_STR
		"Qos mirror group\n"
		"Qos mirror group id\n")
{
	uint8_t group_id = 0;
	int ret = 0;
	int rv = 0;

	group_id = atoi(argv[0]);
	ret = qos_mirror_delete_group(group_id);
	rv = qos_mirror_return_str(ret,vty);
	return rv;
}


DEFUN(qos_mirror_to_group,
		qos_mirror_to_group_cmd,
		"mirror-to group <1-3> direction (ingress|egress|all)",
		"Qos mirror-to\n"
		"Qos mirror group\n"
		"Qos mirror group id\n"
		"Direct\n"
        "Ingress direction\n"
        "Egress direction\n"
        "All direction\n")
{
	uint8_t ret = 0;
	uint8_t rv = 0;
	uint8_t group_id;
	uint32_t ifindex;
	enum QOS_DIR direct = 0;

	ifindex = (uint32_t)vty->index;

	ret = qos_if_check(ifindex);

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
		vty_error_out ( vty, "Failed to execute command, not support config gigabitethernet 1/0/1!! %s", VTY_NEWLINE );
		return CMD_WARNING;
	}

	group_id = (uint8_t)atoi(argv[0]);

	if(!strncmp(argv[1], "ingress", 1))
	{
		direct = QOS_DIR_INGRESS;
	}
	else if(!strncmp(argv[1], "egress", 1))
	{
		direct = QOS_DIR_EGRESS;
	}
	else if(!strncmp(argv[1], "all", 1))
	{
		direct = QOS_DIR_BIDIR;
	}

	ret = qos_if_mirror_already_occupy(0, group_id, 0);   //监视口为mirror-group
	if(ERRNO_EXISTED == ret)
	{
		vty_error_out (vty, "Failed to execute command, monitor group already has mirroring configure!! %s", VTY_NEWLINE );
		return rv;
	}
	
	ret = qos_if_add_mirror_group (ifindex, group_id, direct);
	
	rv = qos_mirror_return_str(ret,vty);

	return rv;
}


DEFUN(no_qos_mirror_to_group,
		no_qos_mirror_to_group_cmd,
		"no mirror-to group",
		NO_STR
		"Qos mirror-to\n"
		"Qos mirror group\n")
{
	uint8_t ret = 0;
	uint8_t rv = 0;
	uint32_t ifindex;

	ifindex = (uint32_t)vty->index;

	ret = qos_if_delete_mirror_group (ifindex);

	rv = qos_mirror_return_str(ret,vty);

	return rv;
}


DEFUN(qos_mirror_to_group_add,
		qos_mirror_to_group_add_cmd,
		"mirror-to interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP|trunk TRUNK|loopback 0}",
		"Qos mirror-to\n"
		CLI_INTERFACE_STR
		CLI_INTERFACE_ETHERNET_STR
        "The port of ethernet, format: <0-7>/<0-31>/<1-255>\n"
		 CLI_INTERFACE_GIGABIT_ETHERNET_STR
		"The port of gigabitethernet, format: <0-7>/<0-31>/<1-255>\n"
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		"The port of xgigabitethernet, format: <0-7>/<0-31>/<1-255>\n"
		CLI_INTERFACE_TRUNK_STR
        "The port of trunk, format: <1-128>\n"
        CLI_INTERFACE_LOOPBACK_STR
        "Interface loopback 0\n")
{
	uint8_t ret = 0;
	uint8_t rv = 0;
	uint8_t group_id;
	uint32_t mirror_if;
	struct ifm_info pifm = {0};

	if(( NULL != argv[0])&&( NULL == argv[1])&&( NULL == argv[2])&&( NULL == argv[3]))
	{
		mirror_if = ifm_get_ifindex_by_name ( (char *)"ethernet", (char *)argv[0]);
	}
	else if(( NULL == argv[0])&&( NULL != argv[1])&&( NULL == argv[2])&&( NULL == argv[3]))
	{
		mirror_if = ifm_get_ifindex_by_name ( (char *)"gigabitethernet", (char *)argv[1]);
	}
	else if(( NULL == argv[0])&&( NULL == argv[1])&&( NULL != argv[2])&&( NULL == argv[3]))
	{
		mirror_if = ifm_get_ifindex_by_name ( (char *)"xgigabitethernet", (char *)argv[2]);
	}
	else if(( NULL == argv[0])&&( NULL == argv[1])&&( NULL == argv[2])&&( NULL != argv[3]))
	{
		mirror_if = ifm_get_ifindex_by_name ( (char *)"trunk", (char *)argv[3]);
	}
	else if(( NULL == argv[0])&&( NULL == argv[1])&&( NULL == argv[2])&&( NULL == argv[3]))
	{
		mirror_if = ifm_get_ifindex_by_name ( (char *)"loopback", (char *)"0");
	}
	else
	{
		vty_warning_out ( vty, "Please config interface gigabitethernet or trunk or loopback 0 %s", VTY_NEWLINE );
	    return CMD_WARNING;
	}
	if (mirror_if == 0 )
	{
	    vty_error_out ( vty, "Can not find this interface mirror-to ,please check out%s", VTY_NEWLINE );
	    return CMD_WARNING;
	}
	if(IFM_IS_SUBPORT(mirror_if))
	{
		vty_error_out ( vty, "Can not support mirror-to subport ,please check out%s", VTY_NEWLINE );
	    return CMD_WARNING;
	}

	if(ifm_get_all_info(mirror_if, MODULE_ID_QOS, &pifm) != 0)
	{
	    vty_error_out ( vty, "Can not find this interface mirror-to ,please check out%s", VTY_NEWLINE );
	    return CMD_WARNING;
	}

    // 2018-05-04, by caojt, add IFNET_MODE_PHYSICAL for L1 ethernet port
    if (IFNET_MODE_PHYSICAL == pifm.mode)
    {
        vty_error_out ( vty, "this interface not support mirror. %s", VTY_NEWLINE );
        return CMD_WARNING;
    }

	if(((( NULL != argv[0])||(NULL != argv[2])||(NULL != argv[1]))&&( NULL == argv[3]))&&(pifm.mode == IFNET_MODE_INVALID))
	{
		vty_error_out ( vty, "this interface is member of trunk ,please check out%s", VTY_NEWLINE );
	    return CMD_WARNING;
	}

	/*不为管理口*/
	if(IFM_TYPE_IS_METHERNET(mirror_if)&&(IFM_SLOT_ID_GET ( mirror_if ) == 0))
	{
		vty_error_out ( vty, "not support mirror-to gigabitethernet 1/0/1,please check out! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	group_id = (uint8_t)(uint32_t)vty->index;
	ret = qos_mirror_group_add_port(group_id,mirror_if);
	rv = qos_mirror_return_str(ret,vty);

	return rv;
}


DEFUN(qos_mirror_to_if,
		qos_mirror_to_if_cmd,
		"mirror-to interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP|trunk TRUNK|loopback 0} direction (ingress|egress|all)",
		"Qos mirror-to\n"
		CLI_INTERFACE_STR
		CLI_INTERFACE_ETHERNET_STR
        "The port of ethernet, format: <0-7>/<0-31>/<1-255>\n"
         CLI_INTERFACE_GIGABIT_ETHERNET_STR
		"The port of gigabitethernet, format: <0-7>/<0-31>/<1-255>\n"
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		"The port of xgigabitethernet, format: <0-7>/<0-31>/<1-255>\n"
		CLI_INTERFACE_TRUNK_STR
        "The port of trunk, format: <1-128>\n"
        CLI_INTERFACE_LOOPBACK_STR
        "Interface loopback 0\n"
        "Direct\n"
        "Ingress direction\n"
        "Egress direction\n"
        "All direction\n")
{
	uint8_t ret = 0;
	uint8_t rv = 0;
	uint32_t ifindex=0;
	uint32_t mirror_if=0;
	char name[32] = "";
	enum QOS_DIR direct = 0;
	struct ifm_info pifm = {0};

	if(( NULL != argv[0])&&( NULL == argv[1])&&( NULL == argv[2])&&( NULL == argv[3]))
	{
		mirror_if = ifm_get_ifindex_by_name ( (char *)"ethernet", (char *)argv[0]);
	}
	else if(( NULL == argv[0])&&( NULL != argv[1])&&( NULL == argv[2])&&( NULL == argv[3]))
	{
		mirror_if = ifm_get_ifindex_by_name ( (char *)"gigabitethernet", (char *)argv[1]);
	}
	else if(( NULL == argv[0])&&( NULL == argv[1])&&( NULL != argv[2])&&( NULL == argv[3]))
	{
		mirror_if = ifm_get_ifindex_by_name ( (char *)"xgigabitethernet", (char *)argv[2]);
	}
	else if(( NULL == argv[0])&&( NULL == argv[1])&&( NULL == argv[2])&&( NULL != argv[3]))
	{
		mirror_if = ifm_get_ifindex_by_name ( (char *)"trunk", (char *)argv[3]);
	}
	else if(( NULL == argv[0])&&( NULL == argv[1])&&( NULL == argv[2])&&( NULL == argv[3]))
	{
		mirror_if = ifm_get_ifindex_by_name ( (char *)"loopback", (char *)"0");
	}
	else
	{
		vty_warning_out ( vty, "Please config interface gigabitethernet or trunk or loopback 0 %s", VTY_NEWLINE );
	    return CMD_WARNING;
	}

	if (mirror_if == 0 )
	{
	    vty_error_out ( vty, "Can not find this interface mirror-to ,please check out%s", VTY_NEWLINE );
	    return CMD_WARNING;
	}
	if(IFM_IS_SUBPORT(mirror_if))
	{
		vty_error_out ( vty, "Can not support mirror-to subport ,please check out%s", VTY_NEWLINE );
	    return CMD_WARNING;
	}

	if(ifm_get_all_info(mirror_if, MODULE_ID_QOS, &pifm) != 0)
	{
	    vty_error_out ( vty, "Can not find this interface mirror-to ,please check out%s", VTY_NEWLINE );
	    return CMD_WARNING;
	}

    // 2018-05-04, by caojt, add IFNET_MODE_PHYSICAL for L1 ethernet port
    if (IFNET_MODE_PHYSICAL == pifm.mode)
    {
        vty_error_out ( vty, "this interface not support mirror. %s", VTY_NEWLINE );
        return CMD_WARNING;
    }

	if(((( NULL != argv[0])||(NULL != argv[1])||(NULL != argv[2]))&&( NULL == argv[3]))&&(pifm.mode == IFNET_MODE_INVALID))
	{
		vty_error_out ( vty, "this interface is member of trunk ,please check out%s", VTY_NEWLINE );
	    return CMD_WARNING;
	}

	/*不为管理口*/
	if(IFM_TYPE_IS_METHERNET(mirror_if)&&(IFM_SLOT_ID_GET ( mirror_if ) == 0))
	{
		vty_error_out ( vty, "not support mirror-to gigabitethernet 1/0/1,please check out! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	ifindex = (uint32_t)vty->index;

	ret = qos_if_check(ifindex);

    // 2018-05-04, by caojt, add IFNET_MODE_PHYSICAL for L1 ethernet port
    if(QOS_ERR_NOT_SUPPORT == ret)
    {
        vty_error_out ( vty, "Failed to execute command, due to the interface not support!! %s", VTY_NEWLINE );
        return CMD_WARNING;
    }

	if(QOS_ERR_BINDED_TRUNK == ret)
	{
		vty_info_out ( vty, "Failed to execute command, due to the interface had binded trunk!! %s", VTY_NEWLINE );
		return CMD_WARNING;
	}
	if(QOS_ERR_CONFIG_IF == ret)
	{
		vty_info_out ( vty, "Failed to execute command, not support config gigabitethernet 1/0/1!! %s", VTY_NEWLINE );
		return CMD_WARNING;
	}
	if(IFM_TYPE_IS_LOOPBCK(ifindex)&&(IFM_SUBPORT_ID_GET ( ifindex ) != 0))
	{
		vty_error_out ( vty, "support loopback 0 mirror-to  gigabitethernet port,please check out! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(!strncmp(argv[4], "ingress", 1))
	{
		direct = QOS_DIR_INGRESS;
	}
	else if(!strncmp(argv[4], "egress", 1))
	{
		direct = QOS_DIR_EGRESS;
	}
	else if(!strncmp(argv[4], "all", 1))
	{
		direct = QOS_DIR_BIDIR;
	}
	
	ret= qos_if_mirror_already_occupy(mirror_if, 0, 0); //判断监视口是否是其他端口的镜像口
	if(ERRNO_EXISTED == ret)
	{
		ifm_get_name_by_ifindex(mirror_if, name);  //获取接口名
		vty_error_out (vty, "Failed to execute command, %s already has mirroring configure!! %s",name, VTY_NEWLINE );
		return rv;
	}
	
	ret= qos_if_mirror_already_occupy(0, 0, ifindex); //判断镜像口是否是一个监视口或者监视组的端口
	if(ERRNO_EXISTED == ret)
	{
		ifm_get_name_by_ifindex(ifindex, name);  //获取接口名
		vty_error_out (vty, "Failed to execute command, %s already has mirroring configure!! %s",name, VTY_NEWLINE );
		return rv;
	}

	ret = qos_if_add_mirror_port (ifindex, mirror_if, direct);

	rv = qos_mirror_return_str(ret,vty);

	return rv;
}


DEFUN(no_qos_mirror_to_if,
		no_qos_mirror_to_if_cmd,
		"no mirror-to interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP|trunk TRUNK|loopback 0}",
		NO_STR
		"Qos mirror-to\n"
		CLI_INTERFACE_STR
		CLI_INTERFACE_ETHERNET_STR
        "The port of ethernet, format: <0-7>/<0-31>/<1-255>\n"
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
        "The port of gigabitethernet, format: <0-7>/<0-31>/<1-255>\n"
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
        "The port of xgigabitethernet, format: <0-7>/<0-31>/<1-255>\n"
		CLI_INTERFACE_TRUNK_STR
        "The port of trunk, format: <1-128>\n"
        CLI_INTERFACE_LOOPBACK_STR
        "Interface loopback 0\n")
{
	uint8_t ret = 0;
	uint8_t rv = 0;
	uint8_t group_id;
	uint32_t ifindex;
	uint32_t mirror_if;
	struct ifm_info pifm = {0};

	if(( NULL != argv[0])&&( NULL == argv[1])&&( NULL == argv[2])&&( NULL == argv[3]))
	{
		mirror_if = ifm_get_ifindex_by_name ( (char *)"ethernet", (char *)argv[0]);
	}
	else if(( NULL == argv[0])&&( NULL != argv[1])&&( NULL == argv[2])&&( NULL == argv[3]))
	{
		mirror_if = ifm_get_ifindex_by_name ( (char *)"gigabitethernet", (char *)argv[1]);
	}
	else if(( NULL == argv[0])&&( NULL == argv[1])&&( NULL != argv[2])&&( NULL == argv[3]))
	{
		mirror_if = ifm_get_ifindex_by_name ( (char *)"xgigabitethernet", (char *)argv[2]);
	}
	else if(( NULL == argv[0])&&( NULL == argv[1])&&( NULL == argv[2])&&( NULL != argv[3]))
	{
		mirror_if = ifm_get_ifindex_by_name ( (char *)"trunk", (char *)argv[3]);
	}
	else if(( NULL == argv[0])&&( NULL == argv[1])&&( NULL == argv[2])&&( NULL == argv[3]))
	{
		mirror_if = ifm_get_ifindex_by_name ( (char *)"loopback", (char *)"0");
	}
	else
	{
		vty_warning_out ( vty, "Please config interface gigabitethernet or trunk or loopback 0 %s", VTY_NEWLINE );
	    return CMD_WARNING;
	}
	if (mirror_if == 0 )
	{
	    vty_error_out ( vty, "Can not find this interface mirror-to ,please check out%s", VTY_NEWLINE );
	    return CMD_WARNING;
	}
	if(IFM_IS_SUBPORT(mirror_if))
	{
		vty_error_out ( vty, "Can not support mirror-to subport ,please check out%s", VTY_NEWLINE );
	    return CMD_WARNING;
	}

	if(ifm_get_all_info(mirror_if, MODULE_ID_QOS, &pifm) != 0)
	{
	    vty_error_out ( vty, "Can not find this interface mirror-to ,please check out%s", VTY_NEWLINE );
	    return CMD_WARNING;
	}

    // 2018-05-04, by caojt, add IFNET_MODE_PHYSICAL for L1 ethernet port
    if (IFNET_MODE_PHYSICAL == pifm.mode)
    {
        vty_error_out ( vty, "this interface not support mirror, please check out%s", VTY_NEWLINE );
        return CMD_WARNING;
    }

	if(((( NULL != argv[0])||(NULL != argv[1])||(NULL != argv[2]))&&( NULL == argv[3]))&&(pifm.mode == IFNET_MODE_INVALID))
	{
		vty_error_out ( vty, "this interface is member of trunk ,please check out%s", VTY_NEWLINE );
	    return CMD_WARNING;
	}

	if(vty->node ==QOS_MIRROR_NODE )
	{
		group_id = (uint8_t)(uint32_t)vty->index;
		ret = qos_mirror_group_delete_port(group_id,mirror_if);
		rv = qos_mirror_return_str(ret,vty);
	}
	else
	{
		ifindex = (uint32_t)vty->index;
		ret = qos_if_delete_mirror_port (ifindex, mirror_if);
		rv = qos_mirror_return_str(ret,vty);
	}
	return rv;
}


DEFUN  (show_qos_mirror_group,
		show_qos_mirror_group_cmd,
       "show mirror-group ",
       SHOW_STR
       "Qos mirror-group\n")

{
	uint8_t group_id;
	struct listnode *node = NULL;
	uint32_t *pifindex;
	char ifname[IFNET_NAMESIZE];

    for ( group_id = 1; group_id < MIRROR_GROUP_NUM; group_id++ )
    {
    	if(mgroup[group_id].id != 0)
    	{
			vty_out(vty, "-----------------------------------------------------%s",VTY_NEWLINE);
	        group_id = mgroup[group_id].id;
			vty_out(vty, "mirror-group %d%s", group_id, VTY_NEWLINE);
			for ( ALL_LIST_ELEMENTS_RO ( mgroup[group_id].pport_list, node, pifindex ) )
			{
				ifm_get_name_by_ifindex ( *pifindex,ifname);
				vty_out(vty, " mirror-to interface %-15s%s",ifname,VTY_NEWLINE);
			}
			vty_out(vty, "-----------------------------------------------------%s",VTY_NEWLINE);
		}
    }
	return CMD_SUCCESS;
}

DEFUN  (show_qos_mirror_if,
		show_qos_mirror_if_cmd,
       "show  mirror interface ",
       SHOW_STR
       "Qos mirror\n"
       CLI_INTERFACE_STR)
{
	int cursor = 0;
	struct qos_if *qos_if = NULL;
	struct hash_bucket *pbucket = NULL;
	char ifname[IFNET_NAMESIZE];
	char ifname1[IFNET_NAMESIZE];

	vty_out(vty, " %-17s %-17s %-17s %-8s%s","Interface","Mirror_to_group","Mirror_to_if", "Direction",VTY_NEWLINE);
	vty_out(vty, "------------------------------------------------------------------%s",VTY_NEWLINE);
	HASH_BUCKET_LOOP ( pbucket, cursor, qos_if_table)
	{
		if(NULL != pbucket->data)
		{
			qos_if = (struct qos_if *)pbucket->data;
			if(NULL != qos_if->if_mirror)
			{
				if((qos_if->if_mirror->mirror_group !=0)&&(qos_if->if_mirror->mirror_if ==0))
				{
					ifm_get_name_by_ifindex(qos_if->ifindex, ifname);
					vty_out(vty, " %-17s %-17d %-17s %-8s%s", ifname, qos_if->if_mirror->mirror_group,"--",qos_mirror_direct_parse_str(qos_if->if_mirror->direct),VTY_NEWLINE);
				}
				else if((qos_if->if_mirror->mirror_group ==0)&&(qos_if->if_mirror->mirror_if !=0))
				{
					ifm_get_name_by_ifindex(qos_if->ifindex, ifname);
					ifm_get_name_by_ifindex(qos_if->if_mirror->mirror_if, ifname1);
					vty_out(vty, " %-17s %-17s %-17s %-8s%s", ifname, "--",ifname1,qos_mirror_direct_parse_str(qos_if->if_mirror->direct),VTY_NEWLINE);
				}
			}
		}
	}
	return CMD_SUCCESS;
}


void qos_mirror_cmd_init(void)
{
	install_node(&qos_mirror_node, qos_mirror_config_write);
	install_default(QOS_MIRROR_NODE);

	install_element(CONFIG_NODE, &qos_mirror_group_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &no_qos_mirror_group_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &show_qos_mirror_group_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &show_qos_mirror_if_cmd, CMD_LOCAL);

	install_element(QOS_MIRROR_NODE, &qos_mirror_to_group_add_cmd, CMD_SYNC);
	install_element(QOS_MIRROR_NODE, &no_qos_mirror_to_if_cmd, CMD_SYNC);

	install_element(LOOPBACK_IF_NODE, &qos_mirror_to_if_cmd, CMD_SYNC);
	install_element(LOOPBACK_IF_NODE, &no_qos_mirror_to_if_cmd, CMD_SYNC);

	install_element(PHYSICAL_IF_NODE, &qos_mirror_to_group_cmd, CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &no_qos_mirror_to_group_cmd, CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &qos_mirror_to_if_cmd, CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &no_qos_mirror_to_if_cmd, CMD_SYNC);

}

