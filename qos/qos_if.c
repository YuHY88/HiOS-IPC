#include <lib/types.h>
#include <lib/zassert.h>
#include <lib/errcode.h>
#include <lib/msg_ipc.h>
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>
#include <lib/module_id.h>
#include <lib/index.h>
#include <qos/qos_mapping.h>
#include <qos/qos_car.h>
#include <qos/hqos.h>
#include <qos/qos_main.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "command.h"
#include "memory.h"
#include "memtypes.h"
#include "vty.h"
#include "hash1.h"
#include "ifm_common.h"
#include "qos_mirror.h"
#include "qos_policy.h"
#include "qos_if.h"
#include "qos_msg.h"

struct hash_table qos_if_table;
static int qos_if_config_write (struct vty *vty);
static int qos_if_add_hqos(uint32_t ifindex, struct hqos_t *phqos);
static int qos_if_delete_hqos(uint32_t ifindex);
static int qos_if_snmp_get(struct qos_if *pqos_if, struct qos_if_snmp *pqos_snmp);


static unsigned int qos_if_compute_hash ( void *hash_key )
{
    if ( NULL == hash_key )
    {
        return 0;
    }
    return (unsigned int)hash_key;
}


static int qos_if_compare ( void *item, void *hash_key )
{
    struct hash_bucket *pbucket = item;

    if ( NULL == item || NULL == hash_key )
    {
        return 1;
    }

    if ( pbucket->hash_key == hash_key )
    {
        return 0;
    }
    else
    {
    	return 1;
	}
}


void qos_if_table_init ( int size )
{
    hios_hash_init ( &qos_if_table, size, qos_if_compute_hash, qos_if_compare );
}


/* 创建一个接口 */
struct qos_if *qos_if_create(uint32_t ifindex)
{
	struct qos_if *pif = NULL;
	struct ifm_info  pifm = {0};

	/* 获取接口信息 */
	if(ifm_get_all_info(ifindex, MODULE_ID_QOS, &pifm) != 0) return NULL;

	/* 分配内存 */
	pif = (struct qos_if *)XCALLOC(MTYPE_QOS_ENTRY, sizeof(struct qos_if));
	if(pif == NULL)
	{
		QOS_LOG_ERR("Fail to malloc for new qos_if.\n");
		return NULL;
	}
	pif->ifindex = ifindex;
	pif->mode = pifm.mode;
	pif->ifg  = QOS_PACKET_IFG_DEFAULT;

	/*初始化完成，加入hash*/
	if(ERRNO_FAIL == qos_if_add(pif))
	{
		XFREE(MTYPE_QOS_ENTRY, pif);
		return NULL;
	}

	QOS_LOG_DBG("Exit function %s\n", __func__);
	return pif;
}


/* 将接口添加到 hash 表 */
int qos_if_add(struct qos_if *pif)
{
	int ret = 0;
	struct hash_bucket *p_hash_bucket = NULL;

	if (NULL == pif)					//预防操作空指针
	{
		return ERRNO_FAIL;
	}

	p_hash_bucket = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if (NULL == p_hash_bucket)
	{
		QOS_LOG_ERR("Fail to malloc for the hash_node of the new qos_if!\n");
		return ERRNO_FAIL;
	}

	memset(p_hash_bucket, 0, sizeof(struct hash_bucket));
	p_hash_bucket->hash_key = (void *)pif->ifindex;
	p_hash_bucket->data = (void *)pif;

	ret = hios_hash_add(&qos_if_table, p_hash_bucket);
	if(ret)
	{
		QOS_LOG_ERR("Fail to add hash_node of qos_if table!\n");
		XFREE(MTYPE_HASH_BACKET, p_hash_bucket);
		return ERRNO_FAIL;
	}

	return ERRNO_SUCCESS;
}


/* 删除接口 */
int qos_if_delete(uint32_t ifindex)
{
	struct hash_bucket *p_hash_bucket = NULL;
	struct qos_if *pif = NULL;
	struct qos_policy *qos_policy = NULL;
	struct listnode *node = NULL;
	struct listnode *next_node = NULL;
	struct car_t *pcar = NULL;
//	struct hqos_t *phqos = NULL;
//	struct queue_profile *pque = NULL;

	if (0 == ifindex)
	{
		QOS_LOG_ERR("Invalid ifindex!\n");
		return ERRNO_FAIL;
	}

	p_hash_bucket = hios_hash_find(&qos_if_table, (void *)ifindex);
	if(NULL == p_hash_bucket)
	{
		QOS_LOG_DBG("The qos_if item is not exist.\n");
		return ERRNO_SUCCESS;
	}

	pif = (struct qos_if *)p_hash_bucket->data;
	if(pif == NULL)
	{
		XFREE(MTYPE_HASH_BACKET, p_hash_bucket);
		return ERRNO_SUCCESS;
	}

	/* 删除qos policy */
	if(pif->pqos_policy != NULL)
	{
		for (ALL_LIST_ELEMENTS (pif->pqos_policy, node, next_node, qos_policy))
		{
			if(qos_policy->type == POLICY_TYPE_CAR)
			{
				qos_policy_delete_if(qos_policy->acl_num, qos_policy->type, qos_policy->direct, pif->ifindex);
			}
#if 0
			{
				/*car 模板引用计数减一*/
				pcar = qos_car_profile_lookup(qos_policy->action.car_id);
				if(NULL == pcar)
				{
					QOS_LOG_ERR("Entering function '%s',qos_car_profile_lookup error.\n", __func__);
					return ERRNO_FAIL;
				}
				qos_car_profile_attached(pcar, 0);
			}

		    XFREE ( MTYPE_QOS_ENTRY, node->data );
			list_delete_node (pif->pqos_policy, node);
			if (list_isempty (pif->pqos_policy))
			{
				list_free(pif->pqos_policy);
				pif->pqos_policy =NULL;
				QOS_LOG_DBG("Exit function '%s'. list_free\n", __func__);
			}
#endif
		}
	}

	/* 删除QoS car */
	if(pif->car_id[0])
	{
		if (IFM_IS_SUBPORT(ifindex) && IFM_TYPE_IS_METHERNET(ifindex))
		{
			pcar = qos_car_profile_lookup(pif->car_id[0]);
			if(NULL == pcar)
			{
				QOS_LOG_ERR("qos_car_profile_lookup error.\n");
				return ERRNO_FAIL;
			}

			if(pif->car_id[0] > QOS_CAR_INDEX_OFFSET)
			{
				qos_car_profile_delete(pif->car_id[0]);
			}
			else
			{
				qos_car_profile_attached(pcar, 0);
			}
		}
		else
		{
			qos_if_delete_car(pif, QOS_DIR_INGRESS);
		}

		pif->car_flag[0] = 0;
	}

	if(pif->car_id[1])
	{
		if (IFM_IS_SUBPORT(ifindex) && IFM_TYPE_IS_METHERNET(ifindex))
		{
			pcar = qos_car_profile_lookup(pif->car_id[1]);
			if(NULL == pcar)
			{
				QOS_LOG_ERR("qos_car_profile_lookup error.\n");
				return ERRNO_FAIL;
			}

			if(pif->car_id[1] > QOS_CAR_INDEX_OFFSET)
			{
				qos_car_profile_delete(pif->car_id[1]);
			}
			else
			{
				qos_car_profile_attached(pcar, 0);
			}
		}
		else
		{
			qos_if_delete_car(pif, QOS_DIR_EGRESS);
		}

		pif->car_flag[1] = 0;
	}

	if (pif->hqos_id)
	{
		qos_if_delete_hqos(pif->ifindex);
#if 0
		phqos = hqos_table_lookup(pif->hqos_id);
		if (NULL == phqos)
		{
			QOS_LOG_ERR("qos hqos table info null.\n");
			return ERRNO_FAIL;
		}

		if (phqos->hqos_id)
		{
			pque = hqos_queue_profile_lookup(phqos->hqos_id);
			if (NULL == pque)
			{
				QOS_LOG_ERR("qos queue profile info null, profile_id=%d.\n", phqos->hqos_id);
				return ERRNO_FAIL;
			}

			hqos_queue_profile_attached(pque, 0);
		}
#endif
	}

	hios_hash_delete(&qos_if_table, p_hash_bucket);

	XFREE(MTYPE_HASH_BACKET, p_hash_bucket);
	XFREE(MTYPE_QOS_ENTRY, pif);

	QOS_LOG_DBG("Exit function '%s'.\n", __func__);
	return ERRNO_SUCCESS;
}

struct qos_if *qos_if_lookup(uint32_t ifindex)
{
	struct hash_bucket *p_hash_bucket = NULL;

	p_hash_bucket = hios_hash_find(&qos_if_table, (void *)ifindex);
	if (NULL == p_hash_bucket)
	{
		return NULL;
	}
	else
	{
		return (struct qos_if *)p_hash_bucket->data;
	}
}


static int qos_if_snmp_get(struct qos_if *pqos_if, struct qos_if_snmp *pqos_snmp)
{
	struct car_t *pcar = NULL;
	struct hqos_t *phqos = NULL;

	if(NULL == pqos_if || NULL == pqos_snmp)
	{
		QOS_LOG_ERR("Input param check error in func\n");
		return ERRNO_PARAM_ILLEGAL;
	}

	if((0 == pqos_if->car_id[0]) &&
	   (0 == pqos_if->car_id[1]) &&
	   (0 == pqos_if->hqos_id))
	{
		return ERRNO_PARAM_ILLEGAL;
	}

	memset(pqos_snmp, 0, sizeof(struct qos_if_snmp));
	pqos_snmp->ifindex = pqos_if->ifindex;

	/* qos car ingress */
	if (pqos_if->car_id[0] > QOS_CAR_INDEX_OFFSET)
	{
		pcar = qos_car_profile_lookup(pqos_if->car_id[0]);
		assert (pcar != NULL);

		memcpy (&(pqos_snmp->car[0]), pcar, sizeof(struct car_t));
	}
	else
	{
		pqos_snmp->car_id[0] = pqos_if->car_id[0];
	}

    pqos_snmp->car_flag[0] = pqos_if->car_flag[0];

	/* qos car egress */
	if (pqos_if->car_id[1] > QOS_CAR_INDEX_OFFSET)
	{
		pcar = qos_car_profile_lookup(pqos_if->car_id[1]);
		assert (pcar != NULL);

		memcpy (&(pqos_snmp->car[1]), pcar, sizeof(struct car_t));
	}
	else
	{
		pqos_snmp->car_id[1] = pqos_if->car_id[1];
	}

    pqos_snmp->car_flag[1] = pqos_if->car_flag[1];

	if (pqos_if->hqos_id)
	{
		phqos = hqos_table_lookup(pqos_if->hqos_id);
		assert (phqos != NULL);

		memcpy (&(pqos_snmp->hqos), phqos, sizeof(struct hqos_t));
	}

	return ERRNO_SUCCESS;
}

int qos_if_get_bulk ( uint32_t index, struct qos_if_snmp *qosif_array )
{
    int msg_num = IPC_MSG_LEN/sizeof(struct qos_if_snmp);
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct qos_if *pqos_if 		= NULL;
    struct qos_if_snmp qos_snmp;
    int cursor;
    int val = 0;
    int data_num = 0;
    int ret = 0;

    if ( index == 0 )
    {
    	HASH_BUCKET_LOOP(pbucket, cursor, qos_if_table)
        {
            pqos_if = (struct qos_if *)pbucket->data;
            if (NULL == pqos_if)
            {
                continue;
            }

			ret = qos_if_snmp_get( pqos_if, &qos_snmp );
			if (ret)
			{
				continue;
			}

            memcpy(&qosif_array[data_num++], &qos_snmp, sizeof(struct qos_if_snmp));
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

            pqos_if = (struct qos_if *)(pnext->data);

			ret = qos_if_snmp_get(pqos_if, &qos_snmp);
			if (ret)
			{
				continue;
			}

            memcpy(&qosif_array[data_num++], &qos_snmp, sizeof(struct qos_if_snmp));
            if (data_num == msg_num)
            {
                return data_num;
            }
        }
    }

    return data_num;
}


int qos_if_car_apply_get_bulk ( uint32_t index, enum QOS_DIR dir, uint32_t *carid_array )
{
    int msg_num = IPC_MSG_LEN/sizeof(uint32_t);
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct qos_if *pqos_if 		= NULL;
    int cursor;
    int val = 0;
    int data_num = 0;

    if ( index == 0 )
    {
    	HASH_BUCKET_LOOP(pbucket, cursor, qos_if_table)
        {
            pqos_if = (struct qos_if *)pbucket->data;
            if (NULL == pqos_if)
            {
                continue;
            }

			if(pqos_if->car_id[dir-1] <= QOS_CAR_INDEX_OFFSET)
			{
				carid_array[data_num++] = pqos_if->car_id[dir-1];
			}

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

            pqos_if = (struct qos_if *)(pnext->data);

            if(pqos_if->car_id[dir-1] <= QOS_CAR_INDEX_OFFSET)
			{
				carid_array[data_num++] = pqos_if->car_id[dir-1];
			}

            if (data_num == msg_num)
            {
                return data_num;
            }
        }
    }

    return data_num;
}


int qos_if_car_param_get_bulk ( uint32_t index, enum QOS_DIR dir, struct car_t *car_array )
{
    int msg_num = IPC_MSG_LEN/sizeof(struct car_t);
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct qos_if *pqos_if 		= NULL;
    struct car_t *pcar          = NULL;
    struct car_t car;
    int cursor;
    int val = 0;
    int data_num = 0;

    if ( index == 0 )
    {
    	HASH_BUCKET_LOOP(pbucket, cursor, qos_if_table)
        {
            pqos_if = (struct qos_if *)pbucket->data;
            if (NULL == pqos_if)
            {
                continue;
            }

			if(pqos_if->car_id[dir-1] > QOS_CAR_INDEX_OFFSET)
			{
				pcar = qos_car_profile_lookup(pqos_if->car_id[dir-1]);
				assert(pcar != NULL);

				memcpy(&car_array[data_num++], pcar, sizeof(struct car_t));
			}
			else
			{
				memset(&car, 0, sizeof(struct car_t));
				memcpy(&car_array[data_num++], pcar, sizeof(struct car_t));
			}

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

            pqos_if = (struct qos_if *)(pnext->data);

            if(pqos_if->car_id[dir-1] > QOS_CAR_INDEX_OFFSET)
			{
				pcar = qos_car_profile_lookup(pqos_if->car_id[dir-1]);
				assert(pcar != NULL);

				memcpy(&car_array[data_num++], pcar, sizeof(struct car_t));
			}
			else
			{
				memset(&car, 0, sizeof(struct car_t));
				memcpy(&car_array[data_num++], pcar, sizeof(struct car_t));
			}

            if (data_num == msg_num)
            {
                return data_num;
            }
        }
    }

    return data_num;
}


/* 接口配置 car 参数的操作
 *
 */
int qos_if_add_car(uint32_t ifindex, enum QOS_DIR dir, struct car_t *pcar)
{
    struct qos_if *pif = NULL;
    struct qos_if qif;
    struct car_t *pcar_new = NULL;
    struct car_t *pcar_old = NULL;
    struct qos_policy *ppolicy = NULL;
	struct listnode *node = NULL;
	int *pdata = NULL;
    uint32_t carid_old;
    int ret;

    if(pcar->pir && pcar->pir < pcar->cir)
        return QOS_ERR_ILL_PIR;
    if(pcar->pbs && pcar->pbs < pcar->cbs)
        return QOS_ERR_ILL_PBS;

    pif = qos_if_lookup(ifindex);

    if (pif && (pif->car_id[dir-1] > QOS_CAR_INDEX_OFFSET))
    {
        if ((pcar->color_blind && pif->car_flag[dir-1] == QOS_CAR_FLAG_LR)
        || (0 == pcar->color_blind && pif->car_flag[dir-1] == QOS_CAR_FLAG_CAR))
        {
            return QOS_ERR_CONFIGURED;
        }
    }

    if((NULL == pif) || ((NULL != pif) && (0 == pif->car_id[dir-1])) ||
        ((NULL != pif) && (pif->car_id[dir-1] > QOS_CAR_INDEX_OFFSET)))
    {
        /* 接口指定方向上没有应用共享模板 */
        pcar_new = qos_car_create(pcar);
        if(NULL == pcar_new)
        {
            QOS_LOG_ERR("qos_car_create failed!!\n");
            return ERRNO_FAIL;
        }
    }

    if(NULL == pif)
    {
        pif = qos_if_create(ifindex);
        if(NULL == pif)
        {
            QOS_LOG_ERR("qos_if_create failed!!\n");
            return ERRNO_FAIL;
        }

        memset(&qif, 0, sizeof(struct qos_if));
        qif.ifindex = ifindex;

        /* 发送接口上应用car消息 */
        qif.car_id[dir - 1] = pcar->car_id;
        #if 0
        ret = ipc_send_hal_wait_ack((void *)&qif, sizeof(struct qos_if), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_QOSIF, QOS_INFO_CAR, IPC_OPCODE_ADD, pcar->car_id);
        #else
        ret = qos_ipc_send_and_wait_ack((void *)&qif, sizeof(struct qos_if), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_QOSIF, QOS_INFO_CAR, IPC_OPCODE_ADD, pcar->car_id);
        #endif
        if(ret)
        {
            qos_car_profile_delete(pcar->car_id);
            return ret;
        }

    }
    else
    {
    	/* 检查接口下是否已经配置qos policy car */
    	if(pif->pqos_policy)
    	{
			for(ALL_LIST_ELEMENTS_RO( pif->pqos_policy, node, pdata ))
			{
				ppolicy = (struct qos_policy *)pdata;
				if(POLICY_TYPE_CAR == ppolicy->type)
				{
					if(dir == ppolicy->direct)
					{
						QOS_LOG_DBG("Already configured with qos policy car.\n");
						return QOS_ERR_CONFLICT;
					}
				}
			}
    	}

    	if((QOS_DIR_EGRESS == dir) &&
    	   (pif->hqos_id != 0))
    	{
    		return QOS_ERR_CAR_HQOS;
    	}

        carid_old = pif->car_id[dir-1];
        if(QOS_CAR_INDEX_OFFSET < carid_old)
        {
            /* 接口上已配置了其他car参数，更新 */
            pcar_old = qos_car_profile_lookup(carid_old);
            if(NULL == pcar_old)
            {
                QOS_LOG_ERR("qos_car_profile_lookup failed!!\n");
                return ERRNO_NOT_FOUND;
            }

            if (0 == memcmp(pcar_old, pcar, sizeof(struct car_t)))
            {
                /* 重复配置 */
                ret = qos_car_profile_delete(pcar->car_id);
                if(ret)
                {
                    QOS_LOG_ERR("qos_car_profile_delete car_id=%d failed!!\n", pcar->car_id);
                }
                return ERRNO_SUCCESS;
            }

            pif->car_id[dir-1] = pcar->car_id;
            #if 0
            ret = ipc_send_hal_wait_ack((void *)pif, sizeof(struct qos_if), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_QOSIF, QOS_INFO_CAR, IPC_OPCODE_UPDATE, pcar->car_id);
            #else
            ret = qos_ipc_send_and_wait_ack((void *)pif, sizeof(struct qos_if), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_QOSIF, QOS_INFO_CAR, IPC_OPCODE_UPDATE, pcar->car_id);
            #endif
            if(ret)
            {
                pif->car_id[dir-1] = carid_old;
                qos_car_profile_delete(pcar->car_id);
                return ret;
            }

            /* 删除接口上原有car模板 */
            ret = qos_car_profile_delete(carid_old);
            if(ret)
            {
                QOS_LOG_ERR("qos_car_profile_delete car_id=%d failed!!\n", pcar->car_id);
                return ret;
            }
        }
        else if(0 < carid_old)
        {
            /* 接口上引用了car模板 */
            QOS_LOG_ERR("This interface was configured to use qos car-profile %d!!\n", pif->car_id[dir-1]);
            return QOS_ERR_CONFIGURED;
        }
        else
        {
            /* 接口上没有配置其他car参数，添加 */
            pif->car_id[dir-1] = pcar->car_id;
            #if 0
            ret = ipc_send_hal_wait_ack((void *)pif, sizeof(struct qos_if), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_QOSIF, QOS_INFO_CAR, IPC_OPCODE_ADD, pcar->car_id);
             
			#else
			ret = qos_ipc_send_and_wait_ack((void *)pif, sizeof(struct qos_if), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_QOSIF, QOS_INFO_CAR, IPC_OPCODE_ADD, pcar->car_id);
			#endif
            if(ret)
            {
                pif->car_id[dir-1] = 0;
                qos_car_profile_delete(pcar->car_id);
                return ret;
            }
        }
    }

    if (pcar->color_blind)
    {
        pif->car_flag[dir-1] = QOS_CAR_FLAG_CAR;
    }
    else
    {
        pif->car_flag[dir-1] = QOS_CAR_FLAG_LR;
    }

    pif->car_id[dir-1] = pcar->car_id;
    return ERRNO_SUCCESS;
}

int qos_if_add_car_profile(uint32_t ifindex, enum QOS_DIR dir, uint32_t car_id)
{
    struct qos_if *pif = NULL;
    struct car_t *pcar_old = NULL;
    struct car_t *pcar = NULL;
    struct qos_policy *ppolicy = NULL;
	struct listnode *node = NULL;
	int *pdata = NULL;
    struct qos_if qif;
    uint32_t carid_old;
    int ret;

    pcar = qos_car_profile_lookup(car_id);
    if(NULL == pcar)
    {
        QOS_LOG_ERR("The specified car profile %d is not found!!\n", car_id);
        return ERRNO_NOT_FOUND;
    }

#if 0
    /* Qos car profile has been created but not configured!!*/
    if(0 == pcar->cir)
    {
        QOS_LOG_ERR("The specified car profile %d was not configured!!\n", car_id);
        return QOS_ERR_CAR_PROFILE_NOT_CONFIG;
    }
#endif

    pif = qos_if_lookup(ifindex);
    if(NULL == pif)
    {
        pif = qos_if_create(ifindex);
        if(NULL == pif)
        {
            QOS_LOG_ERR("qos_if_create failed!!\n");
            return ERRNO_FAIL;
        }

        memset(&qif, 0, sizeof(struct qos_if));
        qif.ifindex = ifindex;
        qif.car_id[dir-1] = car_id;
		#if 0
        ret = ipc_send_hal_wait_ack((void *)&qif, sizeof(struct qos_if), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_QOSIF, QOS_INFO_CAR, IPC_OPCODE_ADD, car_id);
        #else
        ret = qos_ipc_send_and_wait_ack((void *)&qif, sizeof(struct qos_if), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_QOSIF, QOS_INFO_CAR, IPC_OPCODE_ADD, car_id);
        #endif
        if(ret)
        {
            return ret;
        }

        pif->car_id[dir-1] = car_id;
    }
    else
    {
    	/* 检查接口下是否已经配置qos policy car */
    	if(pif->pqos_policy)
    	{
			for(ALL_LIST_ELEMENTS_RO( pif->pqos_policy, node, pdata ))
			{
				ppolicy = (struct qos_policy *)pdata;
				if(POLICY_TYPE_CAR == ppolicy->type)
				{
					if(dir == ppolicy->direct)
					{
						QOS_LOG_DBG("Already configured with qos policy car.\n");
						return QOS_ERR_CONFLICT;
					}
				}
			}
    	}

        /* 检查接口上是否已配置了具体的car参数 */
        if(QOS_CAR_INDEX_OFFSET < pif->car_id[dir-1])
        {
            QOS_LOG_DBG("This interface was configured with special qos car paremeters!!\n");
            return QOS_ERR_CONFIGURED;
        }

        if(car_id == pif->car_id[2-dir])
        {
            QOS_LOG_DBG("This car profile has been applied to the other direction of the interface!!\n");
            return QOS_ERR_CAR_PROFILE_NOT_SHARED;
        }

        /* 检查接口上是否应用了其他模板 */
        if(0 == pif->car_id[dir-1])
        {
            /* 接口上指定方向没有应用任何模板 */
            pif->car_id[dir-1] = car_id;
            #if 0
            ret = ipc_send_hal_wait_ack(pif, sizeof(struct qos_if), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_QOSIF, QOS_INFO_CAR, IPC_OPCODE_ADD, car_id);
            #else
            ret = qos_ipc_send_and_wait_ack(pif, sizeof(struct qos_if), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_QOSIF, QOS_INFO_CAR, IPC_OPCODE_ADD, car_id);
            #endif
            if(ret)
            {
                pif->car_id[dir-1] = 0;
                return ret;
            }
        }
        else
        {
            /* 接口上指定方向已经配置了其他模板
             * 修改接口上引用的模板
             */
            carid_old = pif->car_id[dir-1];
            if(car_id == carid_old)
            {
                return ERRNO_SUCCESS;
            }

            pif->car_id[dir-1] = car_id;
            #if 0
            ret = ipc_send_hal_wait_ack((void *)pif, sizeof(struct qos_if), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_QOSIF, QOS_INFO_CAR, IPC_OPCODE_UPDATE, car_id);
            #else
            ret = qos_ipc_send_and_wait_ack((void *)pif, sizeof(struct qos_if), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_QOSIF, QOS_INFO_CAR, IPC_OPCODE_UPDATE, car_id);
            #endif
            if(ret)
            {
                pif->car_id[dir-1] = carid_old;
                return ret;
            }

            pcar_old= qos_car_profile_lookup(carid_old);
		    if(NULL == pcar_old)
		    {
		        QOS_LOG_ERR("The specified car profile %d is not found!!\n", carid_old);
		        return ERRNO_NOT_FOUND;
		    }

		    /* 原模板的引用计数减1 */
		    qos_car_profile_attached(pcar_old, 0);

        }

        /* 将模板ID保存到qos_if */
        pif->car_id[dir-1] = car_id;
    }

    /* 模板引用计数加1 */
    qos_car_profile_attached(pcar, 1);

    return ERRNO_SUCCESS;
}

int qos_if_delete_car(struct qos_if *pif, enum QOS_DIR dir)
{
    uint32_t car_id;
    struct car_t *pcar = NULL;
    int ret;

    if (NULL == pif)
    {
        return ERRNO_SUCCESS;
    }

    car_id = pif->car_id[dir-1];

    if(0 == car_id)
    {
        return ERRNO_SUCCESS;
    }
    else
    {
        pcar = qos_car_profile_lookup(car_id);
        if(NULL == pcar)
        {
            QOS_LOG_ERR("The qos car profile %d is not exist!!\n", car_id);
            return QOS_ERR_CAR_PROFILE_NOT_FOUND;
        }
		#if 0
        ret = ipc_send_hal_wait_ack((void *)&dir, sizeof(enum QOS_DIR), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                            IPC_TYPE_QOSIF, QOS_INFO_CAR, IPC_OPCODE_DELETE, pif->ifindex);
		#else
		ret = qos_ipc_send_and_wait_ack((void *)&dir, sizeof(enum QOS_DIR), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                            IPC_TYPE_QOSIF, QOS_INFO_CAR, IPC_OPCODE_DELETE, pif->ifindex);
		#endif
        if(ret)
        {
            return ret;
        }

        if(car_id <= QOS_CAR_INDEX_OFFSET)
        {
            qos_car_profile_attached(pcar, 0);
        }
        else
        {
            ret = qos_car_profile_delete(car_id);
            if(ret)
            {
                return ret;
            }
        }

        pif->car_id[dir-1] = 0;
    }

    return ERRNO_SUCCESS;
}


/************************************************
 * Function: qos_if_add_hqos
 * Input:
 *      ifindex: index of interface
 *      phqos  : pointer to structure of hqos
 * Output:
 *      NONE
 * Return:
 *      ERRCODE
 * Description:
 *      Set the hqos parameters of interface.
 ************************************************/
int qos_if_add_hqos(uint32_t ifindex, struct hqos_t *phqos)
{
	struct qos_if        *pif = NULL;
	struct queue_profile *pqueue = NULL;
	int ret;

	ret = hqos_table_create(phqos);
	if(ret)
	{
		QOS_LOG_ERR("hqos_table_add failed. ret=%d\n", ret);
		return ret;
	}

	QOS_LOG_DBG("hqos_id=%d\n", phqos->id);

	pif = qos_if_lookup(ifindex);
    if(NULL == pif)
    {
        pif = qos_if_create(ifindex);
        if(NULL == pif)
        {
            QOS_LOG_ERR("qos_if_create failed!!\n");
            return ERRNO_FAIL;
        }

        QOS_LOG_DBG("hqos_id = %d\n", phqos->id);
        #if 0
        ret = ipc_send_hal_wait_ack(&(phqos->id), sizeof(uint16_t), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_QOSIF, QOS_INFO_HQOS, IPC_OPCODE_ADD, ifindex);
        #else
        ret = qos_ipc_send_and_wait_ack(&(phqos->id), sizeof(uint16_t), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_QOSIF, QOS_INFO_HQOS, IPC_OPCODE_ADD, ifindex);
        #endif
        if(ret)
        {
            return ret;
        }

		pif->hqos_id = phqos->id;
    }
    else
    {
    	if(pif->car_id[1] != 0)
    	{
    		return QOS_ERR_CAR_HQOS;
    	}

    	if(0 == pif->hqos_id)
    	{
    		#if 0
    		ret = ipc_send_hal_wait_ack(&(phqos->id), sizeof(uint16_t), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_QOSIF, QOS_INFO_HQOS, IPC_OPCODE_ADD, ifindex);
			#else
			ret = qos_ipc_send_and_wait_ack(&(phqos->id), sizeof(uint16_t), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_QOSIF, QOS_INFO_HQOS, IPC_OPCODE_ADD, ifindex);
			#endif
	        if(ret)
	        {
	            return ret;
	        }

			pif->hqos_id = phqos->id;
    	}
    	else
    	{
    		QOS_LOG_ERR("HQoS not supported update for now!!\n");
    		return QOS_ERR_CONFIGURED;
    	}
    }

    if(phqos->hqos_id)
    {
    	pqueue = hqos_queue_profile_lookup(phqos->hqos_id);
		assert(NULL != pqueue);
	    hqos_queue_profile_attached(pqueue, 1);
    }

	return ERRNO_SUCCESS;
}


/************************************************
 * Function: qos_if_delete_hqos
 * Input:
 *      ifindex: index of interface
 * Output:
 *      NONE
 * Return:
 *      ERRCODE
 * Description:
 *      Delete the hqos parameters of interface.
 ************************************************/
int qos_if_delete_hqos(uint32_t ifindex)
{
	struct qos_if *pif = NULL;
    int ret;

    pif = qos_if_lookup(ifindex);
    if(NULL == pif)
    {
        QOS_LOG_DBG("The qos_if item is not exist!!\n");
        return ERRNO_SUCCESS;
    }

	if(0 == pif->hqos_id)
	{
		return ERRNO_SUCCESS;
	}
	#if 0
    ret = ipc_send_hal_wait_ack(NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_QOS,
                        IPC_TYPE_QOSIF, QOS_INFO_HQOS, IPC_OPCODE_DELETE, ifindex);
    #else
    ret = qos_ipc_send_and_wait_ack(NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_QOS,
                        IPC_TYPE_QOSIF, QOS_INFO_HQOS, IPC_OPCODE_DELETE, ifindex);
    #endif
    if(ret)
    {
        return ret;
    }

	ret = hqos_table_delete(pif->hqos_id);
	if(ret)
	{
		QOS_LOG_ERR("hqos_table_delete failed!!\n");
		return ret;
	}

	pif->hqos_id = 0;

	return ERRNO_SUCCESS;
}

/*删除mirror 监视口或policy  镜像口、重定向口的情况*/
int qos_if_delete_config(uint32_t ifindex)
{
	struct hash_bucket *pbucket = NULL;
	struct qos_if *qos_if = NULL;
	struct listnode *action_node = NULL;
	struct listnode *next_node = NULL;
	struct listnode *node = NULL;
	struct listnode *global_node = NULL;
	struct listnode *global_next_node = NULL;
	struct qos_policy *qos_policy = NULL;
	struct qos_policy *global_qos_policy = NULL;
	uint32_t *pifindex = NULL;
	uint8_t group_id;
	int cursor = 0;

	struct list *plist = &qos_policy_global;

	HASH_BUCKET_LOOP ( pbucket, cursor, qos_if_table)
	{
		if(NULL != pbucket->data)
		{
			qos_if = (struct qos_if *)pbucket->data;

			if((NULL != qos_if->if_mirror)&&(qos_if->if_mirror->mirror_if == ifindex))
			{
				XFREE(MTYPE_QOS_ENTRY, qos_if->if_mirror);
				qos_if->if_mirror = NULL;
			}

			if(qos_if->pqos_policy != NULL)
			{
				for ( ALL_LIST_ELEMENTS ( qos_if->pqos_policy, action_node, next_node, qos_policy ) )
				{
					if(((qos_policy->type == POLICY_TYPE_MIRROR)&&(qos_policy->action.mirror_if == ifindex))
						||((qos_policy->type == POLICY_TYPE_REDIRECT)&&(qos_policy->action.redirect_if == ifindex)))
					{
						XFREE ( MTYPE_QOS_ENTRY, action_node->data );
						list_delete_node (qos_if->pqos_policy, action_node);
					}

					if (list_isempty (qos_if->pqos_policy))
					{
						list_free(qos_if->pqos_policy);
						qos_if->pqos_policy =NULL;
						QOS_LOG_DBG("In function '%s' list_free\n", __func__);
					}
				}
			}
		}
	}

	for ( ALL_LIST_ELEMENTS (plist, global_node, global_next_node, global_qos_policy ) )
	{
		if(((global_qos_policy->type == POLICY_TYPE_MIRROR)&&(global_qos_policy->action.mirror_if == ifindex))
			||((global_qos_policy->type == POLICY_TYPE_REDIRECT)&&(global_qos_policy->action.redirect == ifindex)))
		{
			XFREE ( MTYPE_QOS_ENTRY, global_node->data );
			list_delete_node (&qos_policy_global, global_node);
		}
	}

	/* 删除mirror group虚接口监视口*/
    for ( group_id = 1; group_id < MIRROR_GROUP_NUM; group_id++ )
    {
    	if(mgroup[group_id].id != 0)
    	{
			for ( ALL_LIST_ELEMENTS_RO ( mgroup[group_id].pport_list, node, pifindex ) )
			{
				if(*pifindex == ifindex)
				{
					listnode_delete (mgroup[group_id].pport_list, pifindex);
					XFREE(MTYPE_QOS_ENTRY, pifindex);
					pifindex = NULL;

					if (list_isempty (mgroup[group_id].pport_list))
					{
						list_free(mgroup[group_id].pport_list);
						mgroup[group_id].pport_list = NULL;
					}
					break;
				}
			}
		}
    }

	return ERRNO_SUCCESS;
}


int qos_if_process_mode_change(uint32_t ifindex)
{
	struct hash_bucket *p_hash_bucket = NULL;
	struct qos_if *pif = NULL;
//	struct car_t *pcar = NULL;
	struct qos_policy *qos_policy = NULL;
	struct listnode *node = NULL;
	struct listnode *next_node = NULL;

	if (0 == ifindex)
	{
		QOS_LOG_ERR("Invalid ifindex!\n");
		return ERRNO_FAIL;
	}

	p_hash_bucket = hios_hash_find(&qos_if_table, (void *)ifindex);
	if(NULL == p_hash_bucket)
	{
		QOS_LOG_DBG("The qos_if item is not exist.\n");
		return ERRNO_SUCCESS;
	}

	pif = (struct qos_if *)p_hash_bucket->data;
	if(pif == NULL)
	{
		XFREE(MTYPE_HASH_BACKET, p_hash_bucket);
		return ERRNO_SUCCESS;
	}

	/*qos policy delete*/
	if (NULL != pif->pqos_policy)
	{
		for (ALL_LIST_ELEMENTS (pif->pqos_policy, node, next_node, qos_policy))
		{
			if (qos_policy->type == POLICY_TYPE_CAR)
			{
				qos_policy_delete_if(qos_policy->acl_num, qos_policy->type, qos_policy->direct, pif->ifindex);
			}
		}
	}

#if 0
	if(IFM_IS_SUBPORT(ifindex) && IFM_TYPE_IS_METHERNET(IFM_PARENT_IFINDEX_GET(ifindex)))
	{
		/* 删除qos policy */
		if(pif->pqos_policy != NULL)
		{
			for ( ALL_LIST_ELEMENTS ( pif->pqos_policy, node, next_node, qos_policy ) )
			{
				if(qos_policy->type == POLICY_TYPE_CAR)
				{
					/*car 模板引用计数减一*/
					pcar = qos_car_profile_lookup(qos_policy->action.car_id);
					if(NULL == pcar)
					{
						QOS_LOG_ERR("qos_car_profile_lookup error.\n");
						return ERRNO_FAIL;
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
	}	
#endif

	/*ingress delete car*/
	if(pif->car_id[0])
	{
#if 0
		if( IFM_IS_SUBPORT(ifindex) && IFM_TYPE_IS_METHERNET(ifindex))
		{
			/* 子接口入方向car删除 */
			pcar = qos_car_profile_lookup(pif->car_id[0]);
			if(NULL == pcar)
			{
				QOS_LOG_ERR("qos_car_profile_lookup error.\n");
				return ERRNO_FAIL;
			}

			if(pif->car_id[0] > QOS_CAR_INDEX_OFFSET)
			{
				qos_car_profile_delete(pif->car_id[0]);
			}
			else
			{
				qos_car_profile_attached(pcar, 0);
			}

			pif->car_id[0] = 0;
		}
#endif
		qos_if_delete_car(pif, QOS_DIR_INGRESS);
		pif->car_flag[0] = 0;
	}

	/*egress delete car*/
	if(pif->car_id[1])
	{
#if 0
		if( IFM_TYPE_IS_METHERNET(ifindex) )
		{
			/* 子接口出方向car删除 */
			pcar = qos_car_profile_lookup(pif->car_id[1]);
			if(NULL == pcar)
			{
				QOS_LOG_ERR("qos_car_profile_lookup error.\n");
				return ERRNO_FAIL;
			}

			if(pif->car_id[1] > QOS_CAR_INDEX_OFFSET)
			{
				qos_car_profile_delete(pif->car_id[1]);
			}
			else
			{
				qos_car_profile_attached(pcar, 0);
			}

			pif->car_id[1] = 0;
		}
#endif
		qos_if_delete_car(pif, QOS_DIR_EGRESS);
		pif->car_flag[1] = 0;
	}

	/* 删除物理接口上的hqos */
	if(pif->hqos_id)
	{
#if 0
		hqos_table_delete(pif->hqos_id);
		pif->hqos_id = 0;
#endif
		qos_if_delete_hqos(pif->ifindex);
	}

	QOS_LOG_DBG("Exit function '%s'.\n", __func__);
	return ERRNO_SUCCESS;
}



int qos_if_check(uint32_t ifindex)
{
	struct ifm_info pinfo = {0};

	if (ifm_get_all_info ( ifindex, MODULE_ID_QOS, &pinfo) != 0)
    {
        QOS_LOG_ERR("Get interface info timeout.\n");
        return ERRNO_FAIL;
    }

    // 2018-05-04, by caojt, add IFNET_MODE_PHYSICAL for L1 ethernet port
    if (IFNET_MODE_PHYSICAL == pinfo.mode)
    {
        QOS_LOG_ERR("Error: this interface not support qos.\n");
        return QOS_ERR_NOT_SUPPORT;
    }

    if(IFM_TYPE_IS_METHERNET(ifindex)&&(pinfo.mode == IFNET_MODE_INVALID))
	{
		QOS_LOG_ERR("Error: this interface is member of trunk ,please check out!\n");
		return QOS_ERR_BINDED_TRUNK;
	}

	if(IFM_IS_SUBPORT(ifindex))
	{
		if(IFM_TYPE_IS_METHERNET(IFM_PARENT_IFINDEX_GET(ifindex)))
		{
			if(pinfo.encap.type == IFNET_ENCAP_INVALID)
			{
				QOS_LOG_ERR("Error: this interface is member of trunk ,please check out!\n");
				return QOS_ERR_CONFIG_ETH_SUBIF;
			}
		}
	}

	if((IFM_TYPE_IS_METHERNET(ifindex))&&(IFM_SLOT_ID_GET ( ifindex ) == 0))
	{
		QOS_LOG_ERR("Error:not support config ethernet 1/0/1,please check out!\n");
		return QOS_ERR_CONFIG_IF;
	}

    return ERRNO_SUCCESS;
}

int qos_if_config_write (struct vty *vty)
{
	uint32_t ifindex = 0;
	unsigned int cursor = 0;
	struct qos_if *qos_if = NULL;
	char ifname[IFNET_NAMESIZE] = "";
	char ifname1[IFNET_NAMESIZE] = "";
	struct qos_policy *qos_policy = NULL;
	struct listnode *node = NULL;
	struct hash_bucket *pbucket = NULL;
	struct car_t *pcar_ing = NULL;
	struct car_t *pcar_egr = NULL;
	struct hqos_t *phqos = NULL;
	uint32_t carid_ing = 0;
	uint32_t carid_egr = 0;
	uint16_t hqos_id = 0;

	HASH_BUCKET_LOOP ( pbucket, cursor, qos_if_table)
	{
		if(NULL == pbucket->data)
		{
			continue;
		}

		qos_if = (struct qos_if *)pbucket->data;
		ifindex = qos_if->ifindex;

		if((qos_if->if_mirror == NULL) &&
		   (qos_if->car_id[0] == 0) &&
		   (qos_if->car_id[1] == 0) &&
		   (qos_if->hqos_id == 0) &&
		   (qos_if->pqos_policy == NULL) &&
		   (qos_if->ifg == QOS_PACKET_IFG_DEFAULT))
		{
			continue;
		}

		ifm_get_name_by_ifindex(ifindex, ifname);
		vty_out ( vty, "interface %s%s", ifname, VTY_NEWLINE );

		if(NULL != qos_if->pqos_policy)
		{
			for (node = qos_if->pqos_policy->head ; node; node = node->next)
			{
				qos_policy = listgetdata(node);
				switch(qos_policy->type)
				{
					case POLICY_TYPE_FILTER:
						if(qos_policy->direct == QOS_DIR_EGRESS)
							vty_out(vty, " qos policy egress acl %d filter %s%s",qos_policy->acl_num,qos_policy->action.filter_act ? "deny" : "permit",VTY_NEWLINE);
						else
							vty_out(vty, " qos policy ingress acl %d filter %s%s",qos_policy->acl_num,qos_policy->action.filter_act ? "deny" : "permit",VTY_NEWLINE);
						break;
					case POLICY_TYPE_CAR:
						if(qos_policy->direct == QOS_DIR_EGRESS)
							vty_out(vty, " qos policy egress acl %d car %d%s",qos_policy->acl_num,qos_policy->action.car_id,VTY_NEWLINE);
						else
							vty_out(vty, " qos policy ingress acl %d car %d%s",qos_policy->acl_num,qos_policy->action.car_id,VTY_NEWLINE);
						break;
					case POLICY_TYPE_MIRROR:
						ifm_get_name_by_ifindex ( qos_policy->action.mirror_if,ifname1);
						vty_out(vty, " qos policy ingress acl %d mirror-to interface %s%s",qos_policy->acl_num,ifname1,VTY_NEWLINE);
						break;
					case POLICY_TYPE_REDIRECT:
						ifm_get_name_by_ifindex ( qos_policy->action.redirect_if,ifname1);
						vty_out(vty, " qos policy ingress acl %d redirect-to interface %s%s",qos_policy->acl_num,ifname1,VTY_NEWLINE);
						break;
					case POLICY_TYPE_QOSMAPPING:
						vty_out(vty, " qos policy ingress acl %d qos-mapping queue %d%s",qos_policy->acl_num,qos_policy->action.queue_id,VTY_NEWLINE);
						break;
					default:
			         	break;
				}
			}
		}

		/* save qos if mirror */
		if(NULL != qos_if->if_mirror)
		{
			if((qos_if->if_mirror->mirror_group !=0)&&(qos_if->if_mirror->mirror_if ==0))
			{
				vty_out(vty, " mirror-to group %d direction %s%s",qos_if->if_mirror->mirror_group,qos_mirror_direct_parse_str(qos_if->if_mirror->direct),VTY_NEWLINE);
			}
			else if((qos_if->if_mirror->mirror_group ==0)&&(qos_if->if_mirror->mirror_if !=0))
			{
				ifm_get_name_by_ifindex(qos_if->if_mirror->mirror_if, ifname1);
				vty_out(vty, " mirror-to interface %s direction %s%s", ifname1,qos_mirror_direct_parse_str(qos_if->if_mirror->direct),VTY_NEWLINE);
			}
		}

		/* save qos if ingress car */
		carid_ing = qos_if->car_id[0];
		if(0 != carid_ing)
		{
			pcar_ing = qos_car_profile_lookup(carid_ing);
			if(NULL == pcar_ing)
				return ERRNO_FAIL;

			if(carid_ing > QOS_CAR_INDEX_OFFSET)
			{
                if (QOS_CAR_FLAG_LR == qos_if->car_flag[0])
                {
                    vty_out(vty, " qos lr ingress cir %u cbs %u %s", pcar_ing->cir, 
						pcar_ing->cbs, VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, " qos car ingress cir %u cbs %u pir %u pbs %u", pcar_ing->cir, 
						pcar_ing->cbs, pcar_ing->pir, pcar_ing->pbs);
						
    				if (QOS_CAR_COLOR_BLIND == pcar_ing->color_blind)
    				{
    					vty_out(vty, " %s", "color-blind");
    				}
    				else
    				{
    					vty_out(vty, " %s", "color-aware");
    				}
    				
    				if (pcar_ing->green_act.new_cos & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " green-remark-cos %d ", pcar_ing->green_act.new_cos & CAR_ACTION_COS_MASK);
                    }
                    
                    if (pcar_ing->green_act.new_dscp & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " green-remark-dscp %d ", pcar_ing->green_act.new_dscp & CAR_ACTION_DSCP_MASK);
                    }
                    
                    if (pcar_ing->green_act.new_queue & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " green-queue %d", pcar_ing->green_act.new_queue & CAR_ACTION_QUEUE_MASK);
                    }
                    
                    if (QOS_CAR_PACKETS_DROP == pcar_ing->green_act.drop)
                    {
                    	vty_out(vty, " green-drop");
                    }
                    
                    if (pcar_ing->yellow_act.new_cos & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " yellow-remark-cos %d ", pcar_ing->yellow_act.new_cos & CAR_ACTION_COS_MASK);
                    }
                    
                    if (pcar_ing->yellow_act.new_dscp & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " yellow-remark-dscp %d ", pcar_ing->yellow_act.new_dscp & CAR_ACTION_DSCP_MASK);
                    }
                    
                    if (pcar_ing->yellow_act.new_queue & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " yellow-queue %d", pcar_ing->yellow_act.new_queue & CAR_ACTION_QUEUE_MASK);
                    }
                    
                    if (QOS_CAR_PACKETS_DROP == pcar_ing->yellow_act.drop)
                    {
                    	vty_out(vty, " yellow-drop");
                    }
                    
                    if (pcar_ing->red_act.new_cos & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " red-remark-cos %d ", pcar_ing->red_act.new_cos & CAR_ACTION_COS_MASK);
                    }
                    
                    if (pcar_ing->red_act.new_dscp & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " red-remark-dscp %d ", pcar_ing->red_act.new_dscp & CAR_ACTION_DSCP_MASK);
                    }
                    
                    if (pcar_ing->red_act.new_queue & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " red-queue %d", pcar_ing->red_act.new_queue & CAR_ACTION_QUEUE_MASK);
                    }
                    
                    if (QOS_CAR_PACKETS_PASS == pcar_ing->red_act.drop)
                    {
                    	vty_out(vty, " red-pass");
                    }
                    
                    vty_out(vty, "%s", VTY_NEWLINE);
                }
			}
			else
			{
				vty_out(vty, " qos car ingress profile %d%s", carid_ing, VTY_NEWLINE);
			}
		}

		/* save qos if egress car */
		carid_egr = qos_if->car_id[1];
		if(0 != carid_egr)
		{
			pcar_egr = qos_car_profile_lookup(carid_egr);
			if(NULL == pcar_egr)
				return ERRNO_FAIL;

			if(carid_egr > QOS_CAR_INDEX_OFFSET)
			{
                if (QOS_CAR_FLAG_LR == qos_if->car_flag[1])
                {
                    vty_out(vty, " qos lr egress cir %u cbs %u %s", pcar_egr->cir, 
						pcar_egr->cbs, VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, " qos car egress cir %d cbs %d pir %d pbs %d", pcar_egr->cir, 
						pcar_egr->cbs, pcar_egr->pir, pcar_egr->pbs);
						
    				if (QOS_CAR_COLOR_BLIND == pcar_egr->color_blind)
    				{
    					vty_out(vty, " %s", "color-blind");
    				}
    				else
    				{
    					vty_out(vty, " %s", "color-aware");
    				}
    				
    				if (pcar_egr->green_act.new_cos & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " green-remark-cos %d ", pcar_egr->green_act.new_cos & CAR_ACTION_COS_MASK);
                    }
                    
                    if (pcar_egr->green_act.new_dscp & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " green-remark-dscp %d ", pcar_egr->green_act.new_dscp & CAR_ACTION_DSCP_MASK);
                    }
                    
                    if (pcar_egr->green_act.new_queue & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " green-queue %d", pcar_egr->green_act.new_queue & CAR_ACTION_QUEUE_MASK);
                    }
                    
                    if (QOS_CAR_PACKETS_DROP == pcar_egr->green_act.drop)
                    {
                    	vty_out(vty, " green-drop");
                    }
                    
                    if (pcar_egr->yellow_act.new_cos & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " yellow-remark-cos %d ", pcar_egr->yellow_act.new_cos & CAR_ACTION_COS_MASK);
                    }
                    
                    if (pcar_egr->yellow_act.new_dscp & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " yellow-remark-dscp %d ", pcar_egr->yellow_act.new_dscp & CAR_ACTION_DSCP_MASK);
                    }
                    
                    if (pcar_egr->yellow_act.new_queue & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " yellow-queue %d", pcar_egr->yellow_act.new_queue & CAR_ACTION_QUEUE_MASK);
                    }
                    
                    if (QOS_CAR_PACKETS_DROP == pcar_egr->yellow_act.drop)
                    {
                    	vty_out(vty, " yellow-drop");
                    }
                    
                    if (pcar_egr->red_act.new_cos & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " red-remark-cos %d ", pcar_egr->red_act.new_cos & CAR_ACTION_COS_MASK);
                    }
                    
                    if (pcar_egr->red_act.new_dscp & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " red-remark-dscp %d ", pcar_egr->red_act.new_dscp & CAR_ACTION_DSCP_MASK);
                    }
                    
                    if (pcar_egr->red_act.new_queue & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " red-queue %d", pcar_egr->red_act.new_queue & CAR_ACTION_QUEUE_MASK);
                    }
                    
                    if (QOS_CAR_PACKETS_PASS == pcar_egr->red_act.drop)
                    {
                    	vty_out(vty, " red-pass");
                    }
                    
                    vty_out(vty, "%s", VTY_NEWLINE);
                }
			}
			else
			{
				vty_out(vty, " qos car egress profile %d%s", carid_egr, VTY_NEWLINE);
			}
		}

		/* save hqos config */
		hqos_id = qos_if->hqos_id;
		if(0 != hqos_id )
		{
			phqos = hqos_table_lookup(hqos_id);
			if (NULL != phqos)
            {
                vty_out(vty, " hqos pir %d", phqos->cir);

                if (phqos->pir != 0)
                {
                    vty_out(vty, " pbs %d", phqos->pir);
                }

                if (phqos->hqos_id != 0)
                {
                    vty_out(vty, " queue-profile %d", phqos->hqos_id);
                }

                vty_out(vty, "%s", VTY_NEWLINE);
            }
		}

		if (qos_if->ifg != QOS_PACKET_IFG_DEFAULT)
		{
			vty_out(vty, " packet ifg-length %u%s", qos_if->ifg, VTY_NEWLINE);
		}

	}
	return ERRNO_SUCCESS;
}

static int qos_if_show_operation(struct vty *vty, struct qos_if *pif)
{
	struct car_t *pcar_ing = NULL;
	struct car_t *pcar_egr = NULL;
	struct hqos_t *phqos = NULL;
	char ifname1[IFNET_NAMESIZE] = "";
	struct qos_policy *qos_policy = NULL;
	struct listnode *node = NULL;
	uint32_t carid_ing = 0;
	uint32_t carid_egr = 0;
	uint16_t hqos_id = 0;

	/* show qos if ingress car */
	carid_ing = pif->car_id[0];
	if(0 != carid_ing)
	{
		pcar_ing = qos_car_profile_lookup(carid_ing);
		if(NULL == pcar_ing)
		{
			vty_out(vty, "CAR INFO ERROR!%s", VTY_NEWLINE);
			return ERRNO_NOT_FOUND;
		}
	
		if(carid_ing > QOS_CAR_INDEX_OFFSET)
		{
            if (QOS_CAR_FLAG_LR == pif->car_flag[0])
            {
                vty_out(vty, "%18s ", "QoS LR ingress:");
                
                vty_out(vty, "CIR %u CBS %u %s", pcar_ing->cir, 
					pcar_ing->cbs, VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, "%18s ", "QoS CAR ingress:");
                
                vty_out(vty, "CIR %u CBS %u PIR %u PBS %u%s", pcar_ing->cir, 
					pcar_ing->cbs, pcar_ing->pir, pcar_ing->pbs, VTY_NEWLINE);
					
    			if (QOS_CAR_PACKETS_PASS != pcar_ing->green_act.drop
                || (pcar_ing->green_act.new_cos & CAR_ACTION_CONFIGED)
                || (pcar_ing->green_act.new_dscp & CAR_ACTION_CONFIGED)
                || (pcar_ing->green_act.new_queue & CAR_ACTION_CONFIGED))
                {
                    vty_out(vty, "%18s color green %s", "", (QOS_CAR_PACKETS_DROP == pcar_ing->green_act.drop) ? "discard" : "pass");
                    			
                    if (pcar_ing->green_act.new_cos & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " remark-cos %d ", pcar_ing->green_act.new_cos & CAR_ACTION_COS_MASK);
                    }
                    
                    if (pcar_ing->green_act.new_dscp & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " remark-dscp %d ", pcar_ing->green_act.new_dscp & CAR_ACTION_DSCP_MASK);
                    }
                    
                    if (pcar_ing->green_act.new_queue & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " queue %d", pcar_ing->green_act.new_queue & CAR_ACTION_QUEUE_MASK);
                    }
                    
                    vty_out(vty, "%s", VTY_NEWLINE);
                }
                
                if (QOS_CAR_PACKETS_PASS != pcar_ing->yellow_act.drop
                || (pcar_ing->yellow_act.new_cos & CAR_ACTION_CONFIGED)
                || (pcar_ing->yellow_act.new_dscp & CAR_ACTION_CONFIGED)
                || (pcar_ing->yellow_act.new_queue & CAR_ACTION_CONFIGED))
                {
                    vty_out(vty, "%18s color yellow %s", "", (QOS_CAR_PACKETS_DROP == pcar_ing->yellow_act.drop) ? "discard" : "pass");
                    			
                    if (pcar_ing->yellow_act.new_cos & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " remark-cos %d ", pcar_ing->yellow_act.new_cos & CAR_ACTION_COS_MASK);
                    }
                    
                    if (pcar_ing->yellow_act.new_dscp & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " remark-dscp %d ", pcar_ing->yellow_act.new_dscp & CAR_ACTION_DSCP_MASK);
                    }
                    
                    if (pcar_ing->yellow_act.new_queue & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " queue %d", pcar_ing->yellow_act.new_queue & CAR_ACTION_QUEUE_MASK);
                    }
                    
                    vty_out(vty, "%s", VTY_NEWLINE);
                }
                
                if (QOS_CAR_PACKETS_DROP != pcar_ing->red_act.drop
                || (pcar_ing->red_act.new_cos & CAR_ACTION_CONFIGED)
                || (pcar_ing->red_act.new_dscp & CAR_ACTION_CONFIGED)
                || (pcar_ing->red_act.new_queue & CAR_ACTION_CONFIGED))
                {
                    vty_out(vty, "%18s color red %s", "", (QOS_CAR_PACKETS_DROP == pcar_ing->red_act.drop) ? "discard" : "pass");
                    			
                    if (pcar_ing->red_act.new_cos & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " remark-cos %d ", pcar_ing->red_act.new_cos & CAR_ACTION_COS_MASK);
                    }
                    
                    if (pcar_ing->red_act.new_dscp & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " remark-dscp %d ", pcar_ing->red_act.new_dscp & CAR_ACTION_DSCP_MASK);
                    }
                    
                    if (pcar_ing->red_act.new_queue & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " queue %d", pcar_ing->red_act.new_queue & CAR_ACTION_QUEUE_MASK);
                    }
                    
                    vty_out(vty, "%s", VTY_NEWLINE);
                }
            }
			
		}
		else
		{
            vty_out(vty, "%18s ", "QoS CAR ingress:");
			vty_out(vty, "CAR-Profile %d%s", carid_ing, VTY_NEWLINE);
		}
	}

	/* show qos if egress car */
	carid_egr = pif->car_id[1];
	if(0 != carid_egr)
	{
		pcar_egr = qos_car_profile_lookup(carid_egr);
		if(NULL == pcar_egr)
		{
			vty_out(vty, "CAR INFO ERROR!%s", VTY_NEWLINE);
			return ERRNO_NOT_FOUND;
		}
		
		if(carid_egr > QOS_CAR_INDEX_OFFSET)
		{
            if (QOS_CAR_FLAG_LR == pif->car_flag[1])
            {
                vty_out(vty, "%18s ", "QoS LR egress:");
                vty_out(vty, "CIR %u CBS %u %s", pcar_egr->cir, 
					pcar_egr->cbs, VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, "%18s ", "QoS CAR egress:");
                vty_out(vty, "CIR %u CBS %u PIR %u PBS %u%s", pcar_egr->cir, 
					pcar_egr->cbs, pcar_egr->pir, pcar_egr->pbs, VTY_NEWLINE);
					
    			if (QOS_CAR_PACKETS_PASS != pcar_egr->green_act.drop
                || (pcar_egr->green_act.new_cos & CAR_ACTION_CONFIGED)
                || (pcar_egr->green_act.new_dscp & CAR_ACTION_CONFIGED)
                || (pcar_egr->green_act.new_queue & CAR_ACTION_CONFIGED))
                {
                    vty_out(vty, "%18s color green %s", "", (QOS_CAR_PACKETS_DROP == pcar_egr->green_act.drop) ? "discard" : "pass");
                    			
                    if (pcar_egr->green_act.new_cos & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " remark-cos %d ", pcar_egr->green_act.new_cos & CAR_ACTION_COS_MASK);
                    }
                    
                    if (pcar_egr->green_act.new_dscp & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " remark-dscp %d ", pcar_egr->green_act.new_dscp & CAR_ACTION_DSCP_MASK);
                    }
                    
                    if (pcar_egr->green_act.new_queue & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " queue %d", pcar_egr->green_act.new_queue & CAR_ACTION_QUEUE_MASK);
                    }
                    
                    vty_out(vty, "%s", VTY_NEWLINE);
                }
                
                if (QOS_CAR_PACKETS_PASS != pcar_egr->yellow_act.drop
                || (pcar_egr->yellow_act.new_cos & CAR_ACTION_CONFIGED)
                || (pcar_egr->yellow_act.new_dscp & CAR_ACTION_CONFIGED)
                || (pcar_egr->yellow_act.new_queue & CAR_ACTION_CONFIGED))
                {
                    vty_out(vty, "%18s color yellow %s", "", (QOS_CAR_PACKETS_DROP == pcar_egr->yellow_act.drop) ? "discard" : "pass");
                    			
                    if (pcar_egr->yellow_act.new_cos & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " remark-cos %d ", pcar_egr->yellow_act.new_cos & CAR_ACTION_COS_MASK);
                    }
                    
                    if (pcar_egr->yellow_act.new_dscp & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " remark-dscp %d ", pcar_egr->yellow_act.new_dscp & CAR_ACTION_DSCP_MASK);
                    }
                    
                    if (pcar_egr->yellow_act.new_queue & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " queue %d", pcar_egr->yellow_act.new_queue & CAR_ACTION_QUEUE_MASK);
                    }
                    
                    vty_out(vty, "%s", VTY_NEWLINE);
                }
                
                if (QOS_CAR_PACKETS_PASS != pcar_egr->red_act.drop
                || (pcar_egr->red_act.new_cos & CAR_ACTION_CONFIGED)
                || (pcar_egr->red_act.new_dscp & CAR_ACTION_CONFIGED)
                || (pcar_egr->red_act.new_queue & CAR_ACTION_CONFIGED))
                {
                    vty_out(vty, "%18s color red %s", "", (QOS_CAR_PACKETS_DROP == pcar_egr->red_act.drop) ? "discard" : "pass");
                    			
                    if (pcar_egr->red_act.new_cos & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " remark-cos %d ", pcar_egr->red_act.new_cos & CAR_ACTION_COS_MASK);
                    }
                    
                    if (pcar_egr->red_act.new_dscp & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " remark-dscp %d ", pcar_egr->red_act.new_dscp & CAR_ACTION_DSCP_MASK);
                    }
                    
                    if (pcar_egr->red_act.new_queue & CAR_ACTION_CONFIGED)
                    {
                    	vty_out(vty, " queue %d", pcar_egr->red_act.new_queue & CAR_ACTION_QUEUE_MASK);
                    }
                    
                    vty_out(vty, "%s", VTY_NEWLINE);
                }            
            }
		}
		else
		{
            vty_out(vty, "%18s ", "QoS CAR egress:");
			vty_out(vty, "CAR-Profile %d%s", carid_egr, VTY_NEWLINE);
		}
	}

	/* show hqos config */
	hqos_id = pif->hqos_id;
	if(0 != hqos_id )
	{
		phqos = hqos_table_lookup(hqos_id);
		if (NULL != phqos)
        {
        	vty_out(vty, "%18s ", "HQoS:");
            vty_out(vty, " HQoS PIR %d", phqos->cir);

            if (phqos->pir != 0)
            {
                vty_out(vty, " PBS %d", phqos->pir);
            }

            if (phqos->hqos_id != 0)
            {
                vty_out(vty, " Queue-Profile %d", phqos->hqos_id);
            }

            vty_out(vty, "%s", VTY_NEWLINE);
        }
	}

	/* show qos if policy */
	if(NULL != pif->pqos_policy)
	{
		for (node = pif->pqos_policy->head ; node; node = node->next)
		{
			qos_policy = listgetdata(node);
			switch(qos_policy->type)
			{
				case POLICY_TYPE_FILTER:
					if(qos_policy->direct == QOS_DIR_EGRESS)
						vty_out(vty, " qos policy egress acl %d filter %s%s",qos_policy->acl_num,qos_policy->action.filter_act ? "deny" : "permit",VTY_NEWLINE);
					else
						vty_out(vty, " qos policy ingress acl %d filter %s%s",qos_policy->acl_num,qos_policy->action.filter_act ? "deny" : "permit",VTY_NEWLINE);
					break;
				case POLICY_TYPE_CAR:
					if(qos_policy->direct == QOS_DIR_EGRESS)
						vty_out(vty, " qos policy egress acl %d car %d%s",qos_policy->acl_num,qos_policy->action.car_id,VTY_NEWLINE);
					else
						vty_out(vty, " qos policy ingress acl %d car %d%s",qos_policy->acl_num,qos_policy->action.car_id,VTY_NEWLINE);
					break;
				case POLICY_TYPE_MIRROR:
					ifm_get_name_by_ifindex ( qos_policy->action.mirror_if,ifname1);
					vty_out(vty, " qos policy ingress acl %d mirror-to interface %s%s",qos_policy->acl_num,ifname1,VTY_NEWLINE);
					break;
				case POLICY_TYPE_REDIRECT:
					ifm_get_name_by_ifindex ( qos_policy->action.redirect_if,ifname1);
					vty_out(vty, " qos policy ingress acl %d redirect-to interface %s%s",qos_policy->acl_num,ifname1,VTY_NEWLINE);
					break;
				case POLICY_TYPE_QOSMAPPING:
					vty_out(vty, " qos policy ingress acl %d qos-mapping queue %d%s",qos_policy->acl_num,qos_policy->action.queue_id,VTY_NEWLINE);
					break;
				default:
		         	break;
			}
		}
	}

	if (pif->ifg != QOS_PACKET_IFG_DEFAULT)
	{
		vty_out(vty, "%18s%d%s","packet ifg-length: ", pif->ifg, VTY_NEWLINE);
	}

	return ERRNO_SUCCESS;
}


DEFUN(qos_car_apply_to_if,
		qos_car_apply_to_if_cmd,
		"qos car (ingress | egress) profile <1-1000>",
		"Quality of Service\n"
		"Committed Access Rate\n"
		"Ingress\n"
		"Egress\n"
		"QoS car-profile\n"
		"QoS car-profile ID\n")
{
	int ret;
	uint32_t ifindex;
	uint32_t car_id;
	enum QOS_DIR dir;

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
		vty_error_out ( vty, "Failed to execute command, not support config outbound interface!! %s", VTY_NEWLINE );
		return CMD_WARNING;
	}

	/* get argv[0] */
	if ( strncmp ( argv[0], "ing", 3 ) == 0 )
	{
		dir = QOS_DIR_INGRESS;
	}
	else
	{
		dir = QOS_DIR_EGRESS;
	}

	/* get argv[1] */
	car_id = atoi(argv[1]);

	ret = qos_if_add_car_profile(ifindex, dir, car_id);
	if(ret)
	{
		if(ERRNO_NOT_FOUND == ret)
		{
			vty_error_out ( vty, "The specified Qos car profile %d is not created!! %s", car_id, VTY_NEWLINE);
		}
		else if(QOS_ERR_CONFIGURED == ret)
		{
			vty_error_out ( vty, "This interface was configured with qos car or qos lr!! %s", VTY_NEWLINE );
		}
		else if(QOS_ERR_CAR_PROFILE_NOT_CONFIG == ret)
		{
			vty_error_out ( vty, "The specified Qos car profile %d was not configured!! %s", car_id, VTY_NEWLINE );
		}
		else if(QOS_ERR_CAR_PROFILE_NOT_SHARED == ret)
		{
			vty_error_out ( vty, "This car profile has been applied to the other direction of the interface.%s", VTY_NEWLINE );
		}
		else if(QOS_ERR_CONFLICT == ret)
		{
			vty_error_out ( vty, "Already configured with qos policy car in the %s direction!! %s", argv[0], VTY_NEWLINE );
		}
		else if(-14 == ret)
		{
			vty_error_out ( vty, "Insufficient hardware resources!! %s", VTY_NEWLINE );
		}
		else
		{
			vty_error_out ( vty, "Failed to execute command!! %s", VTY_NEWLINE);
		}
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}

DEFUN(qos_car_config_if,
		qos_car_config_if_cmd,
		"qos car (ingress | egress) cir <0-10000000> cbs <0-10000000> pir <0-10000000> pbs <0-10000000> (color-blind | color-aware) "
	    "{green-remark-cos <0-7> | green-remark-dscp <0-63> | green-queue <0-7> | green-drop |"
		"yellow-remark-cos <0-7> | yellow-remark-dscp <0-63> | yellow-queue <0-7> | yellow-drop |"
		"red-remark-cos <0-7> | red-remark-dscp <0-63> | red-queue <0-7> | red-pass}",
		"Quality of Service\n"
		"Committed Access Rate\n"
		"Ingress\n"
		"Egress\n"
		"Committed information rate\n"
		"Committed information rate value <0-10000000> (Unit: Kbps)\n"
		"Committed burst size\n"
		"Committed burst size <0-10000000> (Unit: Kb)\n"
		"Peak information rate(Invalide for physical interface)\n"
		"Peak information rate value <0-10000000> (Unit: Kbps)\n"
		"Peak burst size(Invalide for physical interface)\n"
		"Peak burst size <0-10000000> (Unit: Kb)\n"
		"Color-blind mode\n"
		"Color-aware mode\n"
		"Remark green-packets cos\n"
		"Remark green-packets cos value <0-7>\n"
		"Remark green-packets dscp\n"
		"Remark green-packets dscp value <0-63>\n"
		"Remark green-packets queue\n"
		"Remark green-packets queue value <0-7>\n"
		"Drop green-packets\n"
		"Remark yellow-packets cos\n"
		"Remark yellow-packets cos value <0-7>\n"
		"Remark yellow-packets dscp\n"
		"Remark yellow-packets dscp value <0-63>\n"
		"Remark yellow-packets queue\n"
		"Remark yellow-packets queue value <0-7>\n"
		"Drop yellow-packets\n"
		"Remark red-packets cos\n"
		"Remark red-packets cos value <0-7>\n"
		"Remark red-packets dscp\n"
		"Remark red-packets dscp value <0-63>\n"
		"Remark red-packets queue\n"
		"Remark red-packets queue value <0-7>\n"
		"Forward red-packets\n")
{
	int ret;
	uint32_t ifindex;
	enum QOS_DIR dir;
	struct car_t car;

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
		vty_error_out ( vty, "Failed to execute command, not support config outbound interface!! %s", VTY_NEWLINE );
		return CMD_WARNING;
	}
	
	memset(&car, 0, sizeof(struct car_t));

	/* get argv[0] */
	if ( strncmp ( argv[0], "ing", 3 ) == 0 )
	{
		dir = QOS_DIR_INGRESS;
	}
	else
	{
		dir = QOS_DIR_EGRESS;
	}

	VTY_GET_INTEGER_RANGE("Committed information rate", car.cir, argv[1], QOS_CAR_CIR_MIN, QOS_CAR_CIR_MAX);
	VTY_GET_INTEGER_RANGE("Committed burst size", car.cbs, argv[2], QOS_CAR_CBS_MIN, QOS_CAR_CBS_MAX);	
	VTY_GET_INTEGER_RANGE("Peak information rate", car.pir, argv[3], QOS_CAR_PIR_MIN, QOS_CAR_PIR_MAX);
	VTY_GET_INTEGER_RANGE("Peak burst size", car.pbs, argv[4], QOS_CAR_PBS_MIN, QOS_CAR_PBS_MAX);
	
	if ( strncmp ( argv[5], "color-blind", 11 ) == 0 )
	{
		car.color_blind = QOS_CAR_COLOR_BLIND;
	}
	else
	{
		car.color_blind = QOS_CAR_COLOR_AWARE;
	}
	
	if (NULL != argv[6])
	{
		VTY_GET_INTEGER_RANGE("Cos value", car.green_act.new_cos, argv[6], 0, 7);
		car.green_act.new_cos |= CAR_ACTION_CONFIGED;
	}
	
	if (NULL != argv[7])
	{
		VTY_GET_INTEGER_RANGE("Dscp value", car.green_act.new_dscp, argv[7], 0, 63);
		car.green_act.new_dscp |= CAR_ACTION_CONFIGED;
	}
	
	if (NULL != argv[8])
	{
		if (QOS_DIR_EGRESS == dir)
		{
			vty_error_out(vty, "Param queue not support in egress direction.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		
		VTY_GET_INTEGER_RANGE("Queue value", car.green_act.new_queue, argv[8], 0, 7);
		car.green_act.new_queue |= CAR_ACTION_CONFIGED;
	}
	
	if (NULL != argv[9])
	{
		car.green_act.drop = QOS_CAR_PACKETS_DROP;
	}
	else
	{
		car.green_act.drop = QOS_CAR_PACKETS_PASS;
	}
	
	if (NULL != argv[10])
	{
		VTY_GET_INTEGER_RANGE("Cos value", car.yellow_act.new_cos, argv[10], 0, 7);
		car.yellow_act.new_cos |= CAR_ACTION_CONFIGED;
	}
	
	if (NULL != argv[11])
	{
		VTY_GET_INTEGER_RANGE("Dscp value", car.yellow_act.new_dscp, argv[11], 0, 63);
		car.yellow_act.new_dscp |= CAR_ACTION_CONFIGED;
	}
	
	if (NULL != argv[12])
	{
		if (QOS_DIR_EGRESS == dir)
		{
			vty_error_out(vty, "Param queue not support in egress direction.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		
		VTY_GET_INTEGER_RANGE("Queue value", car.yellow_act.new_queue, argv[12], 0, 7);
		car.yellow_act.new_queue |= CAR_ACTION_CONFIGED;
	}
	
	if (NULL != argv[13])
	{
		car.yellow_act.drop = QOS_CAR_PACKETS_DROP;
	}
	else
	{
		car.yellow_act.drop = QOS_CAR_PACKETS_PASS;
	}
	
	if (NULL != argv[14])
	{
		VTY_GET_INTEGER_RANGE("Cos value", car.red_act.new_cos, argv[14], 0, 7);
		car.red_act.new_cos |= CAR_ACTION_CONFIGED;
	}
	
	if (NULL != argv[15])
	{
		VTY_GET_INTEGER_RANGE("Dscp value", car.red_act.new_dscp, argv[15], 0, 63);
		car.red_act.new_dscp |= CAR_ACTION_CONFIGED;
	}
	
	if (NULL != argv[16])
	{
		if (QOS_DIR_EGRESS == dir)
		{
			vty_error_out(vty, "Param queue not support in egress direction.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		
		VTY_GET_INTEGER_RANGE("Queue value", car.red_act.new_queue, argv[16], 0, 7);
		car.red_act.new_queue |= CAR_ACTION_CONFIGED;
	}
	
	if (NULL != argv[17])
	{
		car.red_act.drop = QOS_CAR_PACKETS_PASS;
	}
	else
	{
		car.red_act.drop = QOS_CAR_PACKETS_DROP;
	}

	car.car_id = index_alloc(INDEX_TYPE_CAR) + QOS_CAR_INDEX_OFFSET;
	car.green_act.color  = QOS_COLOR_GREEN;
	car.yellow_act.color = QOS_COLOR_YELLOW;
	car.red_act.color    = QOS_COLOR_RED;
	car.shared		     = QOS_CAR_PROFILE_MONOPOLIZE;

	ret = qos_if_add_car(ifindex, dir, &car);
	if(ret)
	{
		index_free(INDEX_TYPE_CAR, car.car_id - QOS_CAR_INDEX_OFFSET);
		if(QOS_ERR_CONFIGURED == ret)
		{
			vty_error_out ( vty, "This interface was configured with qos car profile or qos lr!! %s", VTY_NEWLINE );
		}
		else if(QOS_ERR_ILL_PIR == ret)
		{
			vty_error_out(vty, "PIR must larger than CIR or equal to CIR.%s", VTY_NEWLINE);
		}
		else if(QOS_ERR_ILL_PBS == ret)
		{
			vty_error_out(vty, "PBS must larger than CBS or equal to CBS.%s", VTY_NEWLINE);
		}
		else if(QOS_ERR_CONFLICT == ret)
		{
			vty_error_out ( vty, "Already configured with qos policy car in the %s direction!! %s", argv[0], VTY_NEWLINE );
		}
		else if(QOS_ERR_CAR_HQOS == ret)
		{
			vty_error_out ( vty, "Hqos was configured on the interface, delete it first!! %s", VTY_NEWLINE);
		}
		else if(-14 == ret)
		{
			vty_error_out ( vty, "Insufficient hardware resources!! %s", VTY_NEWLINE );
		}
		else
		{
			vty_error_out (vty, "Failed to execute command!!%s", VTY_NEWLINE );
		}
        
		return CMD_WARNING;
	}
	
	return CMD_SUCCESS;
}

DEFUN(no_qos_car_apply_to_if,
        no_qos_car_apply_to_if_cmd,
        "no qos car (ingress | egress)",
        NO_STR
        "Quality of Service\n"
        "Committed Access Rate\n"
        "Ingress\n"
        "Egress\n")
{
    int ret;
    enum QOS_DIR dir;
    uint32_t ifindex;
    struct qos_if *pif = NULL;

    ifindex = (uint32_t)vty->index;

    pif = qos_if_lookup(ifindex);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }
    
    /* get argv[0] */
    if ( strncmp ( argv[0], "ing", 3 ) == 0 )
    {
        dir = QOS_DIR_INGRESS;
    }
    else
    {
        dir = QOS_DIR_EGRESS;
    }

    if (0 == pif->car_id[dir-1])
    {
        return CMD_SUCCESS;
    }
    else if (pif->car_id[dir-1] > QOS_CAR_INDEX_OFFSET
        && pif->car_flag[dir-1] != QOS_CAR_FLAG_CAR)
    {
        return CMD_SUCCESS;
    }

    ret = qos_if_delete_car(pif, dir);
    if(ret)
    {
        vty_error_out (vty, "Failed to execute command!!%s", VTY_NEWLINE );
        return CMD_WARNING;
    }

    pif->car_flag[dir-1] = 0;

    return CMD_SUCCESS;
}


DEFUN(qos_lr_config_if,
        qos_lr_config_if_cmd,
        "qos lr (ingress | egress) cir <0-10000000> cbs <0-16000>",
        "Quality of Service\n"
        "Limit Rate\n"
        "Ingress\n"
        "Egress\n"
        "Committed information rate\n"
        "Committed information rate value <0-10000000> (Unit: Kbps)\n"
        "Committed burst size\n"
        "Committed burst size <0-16000> (Unit: Kb)\n")
{
    int ret;
    uint32_t ifindex;
    enum QOS_DIR dir;
    struct car_t car;

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
        vty_error_out ( vty, "Failed to execute command, not support config outbound interface!! %s", VTY_NEWLINE );
        return CMD_WARNING;
    }
    
    memset(&car, 0, sizeof(struct car_t));

    /* get argv[0] */
    if ( strncmp ( argv[0], "ing", 3 ) == 0 )
    {
        dir = QOS_DIR_INGRESS;
    }
    else
    {
        dir = QOS_DIR_EGRESS;
    }

    VTY_GET_INTEGER_RANGE("Committed information rate", car.cir, argv[1], QOS_CAR_CIR_MIN, QOS_CAR_CIR_MAX);
    VTY_GET_INTEGER_RANGE("Committed burst size", car.cbs, argv[2], QOS_CAR_CBS_MIN, QOS_CAR_PHYSICAL_BURST_MAX);

    car.car_id = index_alloc(INDEX_TYPE_CAR) + QOS_CAR_INDEX_OFFSET;
    car.shared = QOS_CAR_PROFILE_MONOPOLIZE;
    car.green_act.drop  = QOS_CAR_PACKETS_PASS;
    car.yellow_act.drop = QOS_CAR_PACKETS_PASS;
    car.red_act.drop    = QOS_CAR_PACKETS_DROP;

    ret = qos_if_add_car(ifindex, dir, &car);
    if(ret)
    {
        index_free(INDEX_TYPE_CAR, car.car_id - QOS_CAR_INDEX_OFFSET);
        if(QOS_ERR_CONFIGURED == ret)
        {
            vty_error_out ( vty, "This interface was configured with qos car profile or qos car!! %s", VTY_NEWLINE );
        }
        else if(QOS_ERR_ILL_PIR == ret)
        {
            vty_error_out(vty, "PIR must larger than CIR or equal to CIR.%s", VTY_NEWLINE);
        }
        else if(QOS_ERR_ILL_PBS == ret)
        {
            vty_error_out(vty, "PBS must larger than CBS or equal to CBS.%s", VTY_NEWLINE);
        }
        else if(QOS_ERR_CONFLICT == ret)
        {
            vty_error_out ( vty, "Already configured with qos policy car in the %s direction!! %s", argv[0], VTY_NEWLINE );
        }
        else if(-14 == ret)
        {
            vty_error_out ( vty, "Insufficient hardware resources!! %s", VTY_NEWLINE );
        }
        else
        {
            vty_error_out (vty, "Failed to execute command!!%s", VTY_NEWLINE );
        }
        
        return CMD_WARNING;
    }
    
    return ERRNO_SUCCESS;
}       

DEFUN(no_qos_lr_config_if,
        no_qos_lr_config_if_cmd,
        "no qos lr (ingress | egress)",
        NO_STR
        "Quality of Service\n"
        "Limit Rate\n"
        "Ingress\n"
        "Egress\n")
{
    int ret;
    enum QOS_DIR dir;
    uint32_t ifindex;
    struct qos_if *pif = NULL;

    ifindex = (uint32_t)vty->index;

    pif = qos_if_lookup(ifindex);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }
    
    /* get argv[0] */
    if ( strncmp ( argv[0], "ing", 3 ) == 0 )
    {
        dir = QOS_DIR_INGRESS;
    }
    else
    {
        dir = QOS_DIR_EGRESS;
    }

    if (pif->car_id[dir-1] <= QOS_CAR_INDEX_OFFSET)
    {
        return CMD_SUCCESS;
    }
    else if (pif->car_flag[dir-1] != QOS_CAR_FLAG_LR)
    {
        return CMD_SUCCESS;
    }

    ret = qos_if_delete_car(pif, dir);
    if(ret)
    {
        vty_error_out (vty, "Failed to execute command!!%s", VTY_NEWLINE );
        return CMD_WARNING;
    }

    pif->car_flag[dir-1] = 0;

    return CMD_SUCCESS;
}

DEFUN (show_qos_interface_ethernet,                                                         
        show_qos_interface_ethernet_cmd,                                                     
        "show qos interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP}",
        SHOW_STR
        "Qos\n"
        "Qos interface information\n"
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR)
{
	uint32_t ifindex = 0;
    struct qos_if *pif = NULL;
	char ifname[IFNET_NAMESIZE] = "";
	struct hash_bucket *pbucket = NULL;
	unsigned int cursor = 0;
	int ret = 0;

	if(argv[0] == NULL && argv[1] == NULL && argv[2] == NULL)
	{
		HASH_BUCKET_LOOP ( pbucket, cursor, qos_if_table)
		{
			if(NULL == pbucket->data)
			{
				continue;
			}

			pif = (struct qos_if *)pbucket->data;
			ifindex = pif->ifindex;

			if((pif->car_id[0] != 0) ||
			   (pif->car_id[1] != 0) ||
			   (pif->hqos_id != 0) ||
			   (pif->pqos_policy != NULL) ||
			   (pif->ifg != QOS_PACKET_IFG_DEFAULT))
			{
				ifm_get_name_by_ifindex(ifindex, ifname);
				vty_out ( vty, "interface %s%s", ifname, VTY_NEWLINE );

				ret = qos_if_show_operation(vty, pif);
			    if(ret)
			    {
			    	return CMD_WARNING;
			    }
			}
		}

		return CMD_SUCCESS;
	}

    if(argv[0] != NULL)
    {
    	ifindex = ifm_get_ifindex_by_name ( "ethernet", ( char * ) argv[0] );
	}
	else if(argv[1] != NULL)
    {
    	ifindex = ifm_get_ifindex_by_name ( "gigabitethernet", ( char * ) argv[1] );
	}
	else if(argv[2] != NULL)
    {
    	ifindex = ifm_get_ifindex_by_name ( "xgigabitethernet", ( char * ) argv[2] );
	}

    pif = qos_if_lookup( ifindex );
    if ( !pif )
    {
        return CMD_SUCCESS;
    }

    ret = qos_if_show_operation(vty, pif);
    if(ret)
    {
    	return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN (show_qos_interface_tdm,
        show_qos_interface_tdm_cmd,
        "show qos interface tdm USP",
        SHOW_STR
        "Qos\n"
        "Qos interface information\n"
        CLI_INTERFACE_TDM_STR
        CLI_INTERFACE_TDM_VHELP_STR)
{
	uint32_t ifindex = 0;
    struct qos_if *pif = NULL;
    int ret = 0;

    ifindex = ifm_get_ifindex_by_name((char *)"tdm", (char *)argv[0]);

    pif = qos_if_lookup( ifindex );
    if ( !pif )
    {
        return CMD_SUCCESS;
    }

    ret = qos_if_show_operation(vty, pif);
    if(ret)
    {
    	return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN (show_qos_interface_vlanif,
        show_qos_interface_vlanif_cmd,
        "show qos interface vlanif <1-4094>",
        SHOW_STR
        "Qos\n"
        "Qos interface information\n"
        CLI_INTERFACE_VLANIF_STR
        CLI_INTERFACE_VLANIF_VHELP_STR)
{
	uint32_t ifindex = 0;
    struct qos_if *pif = NULL;
    int ret = 0;

    ifindex = ifm_get_ifindex_by_name((char *)"vlanif", (char *)argv[0]);

    pif = qos_if_lookup( ifindex );
    if ( !pif )
    {
        return CMD_SUCCESS;
    }

    ret = qos_if_show_operation(vty, pif);
    if(ret)
    {
    	return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN (show_qos_interface_trunk,
        show_qos_interface_trunk_cmd,
        "show qos interface trunk TRUNK",
        SHOW_STR
        "Qos\n"
        "Qos interface information\n"
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR)
{
	uint32_t ifindex = 0;
    struct qos_if *pif = NULL;
    int ret = 0;

    ifindex = ifm_get_ifindex_by_name((char *)"trunk", (char *)argv[0]);

    pif = qos_if_lookup( ifindex );
    if ( !pif )
    {
        return CMD_SUCCESS;
    }

    ret = qos_if_show_operation(vty, pif);
    if(ret)
    {
    	return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN(hqos_queue_apply,
        hqos_queue_apply_cmd,
        "hqos pir <0-10000000> {pbs <16-16000> | queue-profile <1-100>}",
        "Hierarchical QoS\n"
        "Peak information rate\n"
        "Peak information rate value (Unit: Kbps)\n"
        "Peak burst size\n"
        "Peak burst size value (Unit: Kbits)\n"
        "Queue profile\n"
        "Queue profile ID\n")
{
	int ret = 0;
	uint32_t ifindex = 0;
	struct hqos_t hqos;
	struct hqos_t *phqos = NULL;
	struct queue_profile *pqueue = NULL;

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
		vty_error_out ( vty, "Failed to execute command, not support config outbound interface!! %s", VTY_NEWLINE );
		return CMD_WARNING;
	}

	memset(&hqos, 0, sizeof(struct hqos_t));

	hqos.id = index_alloc(INDEX_TYPE_HQOS);

	VTY_GET_INTEGER_RANGE("Committed information rate", hqos.cir, argv[0], HQOS_RATE_MIN, HQOS_RATE_MAX);

	if(NULL != argv[1])
	{
		VTY_GET_INTEGER_RANGE("Peak information rate", hqos.pir, argv[1], HQOS_RATE_MIN, HQOS_RATE_MAX);
	}

	if(NULL != argv[2])
	{
		pqueue = hqos_queue_profile_lookup(atoi(argv[2]));
		if(NULL == pqueue)
		{
			vty_error_out(vty, "The specified queue profile %d is not exist!! %s", atoi(argv[2]), VTY_NEWLINE);
			return CMD_WARNING;
		}

		VTY_GET_INTEGER_RANGE("Queue profile index", hqos.hqos_id, argv[2], 1, HQOS_QUEUE_PROFILE_NUM);
	}

	if((0 == hqos.cir) &&
		(0 == hqos.pir) &&
		(0 == hqos.hqos_id))
	{
		vty_out ( vty, "Meaningless parameters, please select other configuration!! %s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	ret = qos_if_add_hqos(ifindex, &hqos);
	if(ret)
	{
		if(QOS_ERR_CONFIGURED == ret)
		{
			vty_error_out ( vty, "The specified interface has been configured with hqos param, delete it first!! %s", VTY_NEWLINE);
		}
		else if(QOS_ERR_CAR_HQOS == ret)
		{
			vty_error_out ( vty, "Egress car was configured on the interface, delete it first!! %s", VTY_NEWLINE);
		}
		else
		{
			vty_error_out ( vty, "Failed to execute command!! %s", VTY_NEWLINE);
		}

		phqos = hqos_table_lookup(hqos.id);
		if(phqos)
		{
			phqos->hqos_id = 0;
			hqos_table_delete(hqos.id);
		}

		return CMD_WARNING;
	}


	return CMD_SUCCESS;
}


DEFUN(no_hqos_queue_apply,
        no_hqos_queue_apply_cmd,
        "no hqos",
        NO_STR
        "Hierarchical QoS\n")
{
	int ret = 0;
	uint32_t ifindex = 0;

	ifindex = (uint32_t)vty->index;

	ret = qos_if_delete_hqos(ifindex);
	if(ret)
	{
		vty_error_out ( vty, "Failed to execute command!! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}


DEFUN(qos_ifg,
        qos_ifg_cmd,
        "packet ifg-length <0-50>",
        "Packet\n"
        "Interframe Gap Length\n"
        "Interframe Gap Length value (Unit: bytes)\n")
{
	int ret = 0;
	int value = 0;
	uint32_t ifindex = 0;
	struct qos_if *pif = NULL;

	ifindex = (uint32_t)vty->index;
	value = atoi(argv[0]);

	pif = qos_if_lookup(ifindex);
    if(NULL == pif)
    {
        pif = qos_if_create(ifindex);
        if(NULL == pif)
        {
            QOS_LOG_ERR("qos_if_create failed!!\n");
            vty_error_out ( vty, "Failed to execute command!! %s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    else
    {
    	if(value == pif->ifg)
    	{
    		return CMD_SUCCESS;
    	}
    }
	#if 0
	ret = ipc_send_hal_wait_ack(&value, sizeof(int), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_QOSIF, QOS_INFO_PACKET_IFG, IPC_OPCODE_UPDATE, ifindex);
	#else
	ret = qos_ipc_send_and_wait_ack(&value, sizeof(int), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_QOSIF, QOS_INFO_PACKET_IFG, IPC_OPCODE_UPDATE, ifindex);
	#endif
    if(ret)
    {
    	vty_error_out ( vty, "Failed to execute command!! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    pif->ifg = (uint8_t)value;

	return CMD_SUCCESS;
}

DEFUN(no_qos_ifg,
        no_qos_ifg_cmd,
        "no packet ifg-length",
        NO_STR
        "Packet\n"
        "Interframe Gap Length\n")
{
	int ret = 0;
	int value = 0;
	uint32_t ifindex = 0;
	struct qos_if *pif = NULL;

	ifindex = (uint32_t)vty->index;
	value   = QOS_PACKET_IFG_DEFAULT;

	pif = qos_if_lookup(ifindex);
    if(NULL == pif)
    {
        return CMD_SUCCESS;
    }
    else
    {
    	if(value == pif->ifg)
    	{
    		return CMD_SUCCESS;
    	}
    }
	#if 0
	ret = ipc_send_hal_wait_ack(&value, sizeof(int), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_QOSIF, QOS_INFO_PACKET_IFG, IPC_OPCODE_UPDATE, ifindex);
    #else
    ret = qos_ipc_send_and_wait_ack(&value, sizeof(int), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_QOSIF, QOS_INFO_PACKET_IFG, IPC_OPCODE_UPDATE, ifindex);
    #endif
    if(ret)
    {
    	vty_error_out ( vty, "Failed to execute command!! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    pif->ifg = (uint8_t)value;

	return CMD_SUCCESS;
}


void qos_if_cmd_init(void)
{
	ifm_common_init(qos_if_config_write);

	install_element(CONFIG_NODE, &show_qos_interface_ethernet_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &show_qos_interface_tdm_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &show_qos_interface_vlanif_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &show_qos_interface_trunk_cmd, CMD_LOCAL);

	install_element(PHYSICAL_IF_NODE, &qos_car_apply_to_if_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &qos_car_config_if_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_qos_car_apply_to_if_cmd, CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &qos_lr_config_if_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_qos_lr_config_if_cmd, CMD_SYNC);
	install_element(PHYSICAL_SUBIF_NODE, &qos_car_apply_to_if_cmd, CMD_SYNC);
	install_element(PHYSICAL_SUBIF_NODE, &no_qos_car_apply_to_if_cmd, CMD_SYNC);
	install_element(PHYSICAL_SUBIF_NODE, &qos_car_config_if_cmd, CMD_SYNC);
    
    install_element(PHYSICAL_IF_NODE, &hqos_queue_apply_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_hqos_queue_apply_cmd, CMD_SYNC);

    install_element(PHYSICAL_IF_NODE, &qos_ifg_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_qos_ifg_cmd, CMD_SYNC);
}

