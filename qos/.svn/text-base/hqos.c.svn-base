/*
 * Copyright (c) 2017,Beijing Huahuan Electronics Co.,Ltd.
 * All rights reserved.
 *
 * File name    : hqos.c
 * First Author : Liu Gan
 * Created Date : 2017.01.18
 * Modify Date  : 2017.02.25
 */

#include <lib/errcode.h>
#include <lib/memtypes.h>
#include <lib/memory.h>
#include <lib/command.h>
#include <lib/zassert.h>
#include <lib/module_id.h>
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>
#include <lib/index.h>

#include <qos/hqos.h>
#include <qos/qos_main.h>
#include <qos/qos_car.h>
#include <qos/qos_msg.h>

#include <errno.h>
#include <string.h>
#include <stdlib.h>



struct wred_profile   *gwred_profile[HQOS_WRED_PROFILE_NUM]; /* 全局 wred 模板，使用数组存储 */
struct queue_profile  *gque_profile[HQOS_QUEUE_PROFILE_NUM]; /* 全局队列模板，使用数组存储 */
struct hash_table hqos_table; /*用于保存和恢复hqos配置*/
uint32_t profile_line = 0;        /* 存储回显行数 */


static unsigned int compute_hash ( void *hash_key )
{
    if ( NULL == hash_key )
    {
        return 0;
    }

    return ( ( unsigned int ) hash_key );
}

static int compare ( void *item, void *hash_key )
{
    struct hash_bucket *pbucket = item;

    if ( NULL == item || NULL == hash_key )
    {
        return ERRNO_FAIL;
    }

    if ( pbucket->hash_key == hash_key )
    {
        return ERRNO_SUCCESS;
    }
    else
    {
        return ERRNO_FAIL;
    }
}


/************************************************
 * Function: hqos_wred_profile_init
 * Input:
 *      NONE
 * Output:
 *      NONE
 * Return:
 *      NONE
 * Description:
 *      Initialize wred profile for HQoS.
 ************************************************/
void hqos_wred_profile_init(void)
{
	int i = 0;

	for(i=0; i<HQOS_WRED_PROFILE_NUM; i++)
	{
		gwred_profile[i] = NULL;
	}

	return ;
}


/************************************************
 * Function: hqos_queue_profile_init
 * Input:
 *      NONE
 * Output:
 *      NONE
 * Return:
 *      NONE
 * Description:
 *      Initialize queue profile for HQoS.
 ************************************************/
void hqos_queue_profile_init(void)
{
	int i = 0;

	for(i=0; i<HQOS_QUEUE_PROFILE_NUM; i++)
	{
		gque_profile[i] = NULL;
	}

	return ;
}


void hqos_table_init()
{
    hios_hash_init ( &hqos_table, HASHTAB_SIZE, compute_hash, compare );
    hqos_wred_profile_init();
    hqos_queue_profile_init();
    index_register(INDEX_TYPE_HQOS, 4096);
}


struct hqos_t *hqos_table_lookup ( uint32_t index )
{
    struct hash_bucket *pbucket = NULL;

    pbucket = hios_hash_find ( &hqos_table, ( void * ) index );
    if ( NULL == pbucket )
    {
        QOS_LOG_DBG ( "HQoS table %d lookup failed!!\n", index );
        return NULL;
    }

    return ( struct hqos_t * ) pbucket->data;
}

int hqos_table_create ( struct hqos_t *phqos )
{
    struct hqos_t *phqos_new = NULL;
    int ret;
#if 0
	ret = ipc_send_hal_wait_ack((void *)phqos, sizeof(struct hqos_t), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_HQOS, HQOS_SUBTYPE_ID, IPC_OPCODE_ADD, phqos->id);
#else
	ret = qos_ipc_send_and_wait_ack((void *)phqos, sizeof(struct hqos_t), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_HQOS, HQOS_SUBTYPE_ID, IPC_OPCODE_ADD, phqos->id);
#endif
    if(ret)
    {
        QOS_LOG_ERR("ipc_send_hal_wait_ack failed.\n");
        return ret;
    }

    phqos_new = ( struct hqos_t * ) XCALLOC ( MTYPE_QOS_ENTRY, sizeof ( struct hqos_t ) );
    if ( NULL == phqos_new )
    {
        zlog_err ( "%s[%d]: The memory is insufficient.\n", __func__, __LINE__ );
        return ERRNO_MALLOC;
    }

    memcpy(phqos_new, phqos, sizeof(struct hqos_t));

    ret = hqos_table_add ( phqos_new );
    if ( 0 != ret )
    {
        zlog_err ( "%s[%d]: hqos add to hash failed, index = 0x%0x!!\n",
                   __func__, __LINE__, phqos->id );
        XFREE ( MTYPE_QOS_ENTRY, phqos_new );
        return ret;
    }

    return ret;
}


int hqos_table_add ( struct hqos_t *phqos )
{
    struct hash_bucket *pbucket = NULL;
    int ret;

    pbucket = ( struct hash_bucket * ) XCALLOC ( MTYPE_HASH_BACKET, sizeof ( struct hash_bucket ) );
    if ( NULL == pbucket )
    {
        zlog_err ( "%s[%d]: The memory is insufficient.\n", __func__, __LINE__ );
        return ERRNO_MALLOC;
    }

    pbucket->hash_key = ( void * )(int) phqos->id;
    pbucket->data = ( void * ) phqos;

    ret = hios_hash_add ( &hqos_table, pbucket );
    if ( 0 != ret )
    {
        zlog_err ( "%s[%d]: hios hash add failed!!\n", __func__, __LINE__ );
        XFREE ( MTYPE_HASH_BACKET, pbucket );
        return ERRNO_FAIL;
    }

    return ERRNO_SUCCESS;
}


int hqos_table_delete ( uint16_t index )
{
    struct hqos_t *phqos = NULL;
    struct hash_bucket *pbucket = NULL;
    struct queue_profile *pqueue = NULL;
    int ret;

    pbucket = hios_hash_find ( &hqos_table, ( void * )(int)index );
    if ( NULL == pbucket )
    {
        QOS_LOG_DBG ( "The hqos config 0x%0x is not exist!!\n", index );
        return ERRNO_SUCCESS;
    }

    phqos = ( struct hqos_t * ) pbucket->data;
    if ( NULL == phqos )
    {
        XFREE ( MTYPE_HASH_BACKET, pbucket );
        return ERRNO_SUCCESS;
    }
#if 0
    ret = ipc_send_hal_wait_ack(NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_QOS,
                            IPC_TYPE_HQOS, HQOS_SUBTYPE_ID, IPC_OPCODE_DELETE, index);
#else
	ret = qos_ipc_send_and_wait_ack(NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_QOS,
								IPC_TYPE_HQOS, HQOS_SUBTYPE_ID, IPC_OPCODE_DELETE, index);
#endif

    if(ret)
    {
    	QOS_LOG_ERR ( "ipc_send_hal_wait_ack hqos delete failed.\n" );
        return ret;
    }

    if(0 != phqos->hqos_id)
	{
		pqueue = hqos_queue_profile_lookup(phqos->hqos_id);
		assert(NULL != pqueue);
		hqos_queue_profile_attached(pqueue, 0);
	}

    hios_hash_delete ( &hqos_table, pbucket );

    XFREE ( MTYPE_HASH_BACKET, pbucket );
    XFREE ( MTYPE_QOS_ENTRY, phqos );
    index_free(INDEX_TYPE_HQOS, index);

    return ERRNO_SUCCESS;
}


/************************************************
 * Function: hqos_wred_profile_lookup
 * Input:
 *      wred_id: wred profile index
 * Output:
 *      NONE
 * Return:
 *      wred_profile*: point to wred profile
 * Description:
 *      Get specified wred profile.
 ************************************************/
struct wred_profile *hqos_wred_profile_lookup(uint8_t wred_id)
{
	return gwred_profile[wred_id-1];
}


/************************************************
 * Function: hqos_wred_profile_add
 * Input:
 *      pwred: pointer to wred profile to be added
 * Output:
 *      NONE
 * Return:
 *      ERRCODE
 * Description:
 *      Add a new wred profile.
 ************************************************/
int hqos_wred_profile_add(struct wred_profile *pwred)
{
	struct wred_profile *pwred_lookup = NULL;
	struct wred_profile *pwred_new = NULL;
	int ret = 0;

	assert(NULL != pwred);

	pwred_lookup = hqos_wred_profile_lookup(pwred->id);
	if(NULL != pwred_lookup)
	{
		QOS_LOG_DBG("Qos wred-profile %d exist.\n", pwred->id);
		return ERRNO_SUCCESS;
	}
#if 0
	ret = ipc_send_hal_wait_ack(pwred, sizeof(struct wred_profile), 1, MODULE_ID_HAL, MODULE_ID_QOS,
						IPC_TYPE_HQOS, HQOS_SUBTYPE_WRED_PROFILE, IPC_OPCODE_ADD, pwred->id);
#else
	ret = qos_ipc_send_and_wait_ack(pwred, sizeof(struct wred_profile), 1, MODULE_ID_HAL, MODULE_ID_QOS,
						IPC_TYPE_HQOS, HQOS_SUBTYPE_WRED_PROFILE, IPC_OPCODE_ADD, pwred->id);
#endif

	if(ret)
    {
        QOS_LOG_ERR("ipc_send_hal_wait_ack (wred profile) failed.\n");
        return ret;
    }

	pwred_new = (struct wred_profile *)XCALLOC(MTYPE_QOS_ENTRY, sizeof(struct wred_profile));
	if(NULL == pwred_new)
	{
		QOS_LOG_ERR("In function '%s' the memory is insufficient.\n", __func__);
        return ERRNO_MALLOC;
	}

	memcpy(pwred_new, pwred, sizeof(struct wred_profile));

	gwred_profile[pwred->id-1] = pwred_new;

	return ERRNO_SUCCESS;
}


/************************************************
 * Function: hqos_wred_profile_delete
 * Input:
 *      wred_id: wred profile to be deleted
 * Output:
 *      NONE
 * Return:
 *      ERRCODE
 * Description:
 *      Delete an wred profile.
 ************************************************/
int hqos_wred_profile_delete(uint8_t wred_id)
{
	struct wred_profile *pwred_lookup = NULL;
	int ret = 0;

	pwred_lookup = hqos_wred_profile_lookup(wred_id);
	if(NULL == pwred_lookup)
	{
		return ERRNO_SUCCESS;
	}

	if(0 != pwred_lookup->ref_cnt)
	{
		return QOS_ERR_ATTACHED;
	}
#if 0
	ret = ipc_send_hal_wait_ack(NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_QOS,
						IPC_TYPE_HQOS, HQOS_SUBTYPE_WRED_PROFILE, IPC_OPCODE_DELETE, wred_id);
#else
	ret = qos_ipc_send_and_wait_ack(NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_QOS,
						IPC_TYPE_HQOS, HQOS_SUBTYPE_WRED_PROFILE, IPC_OPCODE_DELETE, wred_id);
#endif
	if(ret)
    {
        QOS_LOG_ERR("ipc_send_hal_wait_ack (wred profile) failed.\n");
        return ret;
    }

	XFREE(MTYPE_QOS_ENTRY, pwred_lookup);
	gwred_profile[wred_id-1] = NULL;

	return ERRNO_SUCCESS;
}


/************************************************
 * Function: hqos_wred_profile_set_rule
 * Input:
 *      pwred: pointer to wred profile that wred rule
 *             to be set
 *      prule: pointer to wred rule to be set
 * Output:
 *      NONE
 * Return:
 *      ERRCODE
 * Description:
 *      Set an wred rule.
 ************************************************/
int hqos_wred_profile_set_rule(struct wred_profile *pwred, struct wred_rule *prule)
{
    int ret = 0;
    struct wred_rule *pold         = NULL;
    struct wred_rule *pold_tcp     = NULL;
    struct wred_rule *pold_non_tcp = NULL;

    assert(NULL != pwred);
    assert(NULL != prule);

    if (pwred->ref_cnt)
    {
        return QOS_ERR_ATTACHED;
    }

    /* parameter legitimacy check*/
    if (prule->drop_high < prule->drop_low)
        return QOS_ERR_ILL_DROPHIGH;

    /* repeat configuration check*/
    if (WRED_PROTOCOL_ALL == prule->protocol)
    {
        pold_tcp     = &(pwred->wred[prule->color][WRED_PROTOCOL_TCP]);
        pold_non_tcp = &(pwred->wred[prule->color][WRED_PROTOCOL_NON_TCP]);

        if (WRED_RULE_REPEAT_CHECK(pold_tcp, prule)
        && WRED_RULE_REPEAT_CHECK(pold_non_tcp, prule))
        {
            QOS_LOG_DBG("Wred rule repeat configure, return success\n");
            return ERRNO_SUCCESS;
        }
    }
    else
    {
        pold = &(pwred->wred[prule->color][prule->protocol]);

        if (WRED_RULE_REPEAT_CHECK(pold, prule))
        {
            QOS_LOG_DBG("Wred rule repeat configure, return success\n");
            return ERRNO_SUCCESS;
        }
    }
#if 0
    ret = ipc_send_hal_wait_ack(prule, sizeof(struct wred_rule), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_HQOS, HQOS_SUBTYPE_WRED_RULE, IPC_OPCODE_ADD, pwred->id);
#else
	ret = qos_ipc_send_and_wait_ack(prule, sizeof(struct wred_rule), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_HQOS, HQOS_SUBTYPE_WRED_RULE, IPC_OPCODE_ADD, pwred->id);
#endif

    if (ret)
    {
        QOS_LOG_ERR("ipc_send_hal_wait_ack failed. pwred_id = %d\n", pwred->id);
        return ret;
    }

    if (prule->protocol == WRED_PROTOCOL_ALL)
    {
        memcpy(&(pwred->wred[prule->color][WRED_PROTOCOL_TCP]), prule, sizeof(struct wred_rule));
        pwred->wred[prule->color][WRED_PROTOCOL_TCP].protocol = WRED_PROTOCOL_TCP;

        memcpy(&(pwred->wred[prule->color][WRED_PROTOCOL_NON_TCP]), prule, sizeof(struct wred_rule));
        pwred->wred[prule->color][WRED_PROTOCOL_NON_TCP].protocol = WRED_PROTOCOL_NON_TCP;
    }
    else
    {
        memcpy(&(pwred->wred[prule->color][prule->protocol]), prule, sizeof(struct wred_rule));
    }

    return ERRNO_SUCCESS;
}


/************************************************
 * Function: hqos_wred_profile_get_bulk
 * Input:
 *      index: wred profile index
 * Output:
 *      wred_array: wred data
 * Return:
 *      data num
 * Description:
 *      Get wred profile data.
 ************************************************/
int hqos_wred_profile_get_bulk ( uint8_t index, struct wred_profile *wred_array )
{
    int data_num = 0;
    int i = 0;

    if ( index == 0 )
    {
    	for (i=0; i<HQOS_WRED_PROFILE_NUM; i++)
	    {
	        if (gwred_profile[i] != NULL)
	        {
	        	memcpy(&wred_array[data_num++], gwred_profile[i], sizeof(struct wred_profile));
	        }
	    }
    }
    else
    {
    	for (i=++index; i<HQOS_WRED_PROFILE_NUM+1; i++)
	    {
	        if (NULL == gwred_profile[i-1])
	        {
	        	continue;
	        }

	        memcpy(&wred_array[data_num++], gwred_profile[i-1], sizeof(struct wred_profile));
	    }
    }

    return data_num;
}


/************************************************
 * Function: hqos_queue_profile_lookup
 * Input:
 *      profile_id: queue profile id
 * Output:
 *      NONE
 * Return:
 *      queue_profile*: point to queue profile
 * Description:
 *      Get specified queue profile.
 ************************************************/
struct queue_profile *hqos_queue_profile_lookup(uint8_t profile_id)
{
	return gque_profile[profile_id-1];
}


/************************************************
 * Function: hqos_queue_profile_add
 * Input:
 *      pwred: pointer to queue profile to be added
 * Output:
 *      NONE
 * Return:
 *      ERRCODE
 * Description:
 *      Add a new queue profile.
 ************************************************/
int hqos_queue_profile_add(struct queue_profile *pque_profile)
{
	struct queue_profile *pqueue_lookup = NULL;
	struct queue_profile *pqueue_new = NULL;
	int ret = 0;

	assert(NULL != pque_profile);

	pqueue_lookup = hqos_queue_profile_lookup(pque_profile->id);
	if(NULL != pqueue_lookup)
	{
		return ERRNO_SUCCESS;
	}
#if 0
	ret = ipc_send_hal_wait_ack(pque_profile, sizeof(struct queue_profile), 1, MODULE_ID_HAL, MODULE_ID_QOS,
						IPC_TYPE_HQOS, HQOS_SUBTYPE_QUEUE_PROFILE, IPC_OPCODE_ADD, pque_profile->id);
#else
	ret = qos_ipc_send_and_wait_ack(pque_profile, sizeof(struct queue_profile), 1, MODULE_ID_HAL, MODULE_ID_QOS,
						IPC_TYPE_HQOS, HQOS_SUBTYPE_QUEUE_PROFILE, IPC_OPCODE_ADD, pque_profile->id);
#endif

	if(ret)
    {
        QOS_LOG_ERR("ipc_send_hal_wait_ack (queue profile) failed.\n");
        return ret;
    }

	pqueue_new = (struct queue_profile *)XCALLOC(MTYPE_QOS_ENTRY, sizeof(struct queue_profile));
	if(NULL == pqueue_new)
	{
		QOS_LOG_ERR("In function '%s' the memory is insufficient.\n", __func__);
        return ERRNO_MALLOC;
	}

	memcpy(pqueue_new, pque_profile, sizeof(struct queue_profile));

	gque_profile[pque_profile->id-1] = pqueue_new;

	return ERRNO_SUCCESS;
}


/************************************************
 * Function: hqos_queue_profile_delete
 * Input:
 *      profile_id: queue profile to be deleted
 * Output:
 *      NONE
 * Return:
 *      ERRCODE
 * Description:
 *      Delete a queue profile.
 ************************************************/
int hqos_queue_profile_delete(uint8_t profile_id)
{
	struct queue_profile *pqueue_lookup = NULL;
	struct wred_profile *pwred = NULL;
	int ret = 0;
	int i = 0;
	uint8_t wred_id = 0;

	pqueue_lookup = hqos_queue_profile_lookup(profile_id);
	if(NULL == pqueue_lookup)
	{
		return ERRNO_SUCCESS;
	}

	if(0 != pqueue_lookup->ref_cnt)
	{
		return QOS_ERR_ATTACHED;
	}
#if 0
	ret = ipc_send_hal_wait_ack(NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_QOS,
						IPC_TYPE_HQOS, HQOS_SUBTYPE_QUEUE_PROFILE, IPC_OPCODE_DELETE, profile_id);
#else
	ret = qos_ipc_send_and_wait_ack(NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_QOS,
						IPC_TYPE_HQOS, HQOS_SUBTYPE_QUEUE_PROFILE, IPC_OPCODE_DELETE, profile_id);
#endif

	if(ret)
    {
        QOS_LOG_ERR("ipc_send_hal_wait_ack (wred profile) failed.\n");
        return ret;
    }

    for(i = 0; i < 8; i++)
	{
		wred_id = pqueue_lookup->queue[i].wred_id;
		if(0 != wred_id)
		{
			pwred = hqos_wred_profile_lookup(wred_id);
			assert(NULL != pwred);

			hqos_wred_profile_attached(pwred, 0);
		}
	}

	XFREE(MTYPE_QOS_ENTRY, pqueue_lookup);
	gque_profile[profile_id-1] = NULL;

	return ERRNO_SUCCESS;
}


/************************************************
 * Function: hqos_queue_profile_set_queue
 * Input:
 *      pque_profile: pointer to queue profile that queue rule
 *			   		  to be set
 *      pqueue: pointer to queue rule to be set
 * Output:
 *      NONE
 * Return:
 *      ERRCODE
 * Description:
 *      Set a queue rule.
 ************************************************/
int hqos_queue_profile_set_queue(struct queue_profile *pque_profile, struct hqos_queue *pqueue)
{
	int ret = 0;
	struct hqos_queue *pold = NULL;
    struct wred_profile *pwred = NULL;

	assert(NULL != pque_profile);
	assert(NULL != pqueue);

	if(pque_profile->ref_cnt)
	{
		return QOS_ERR_ATTACHED;
	}

	pold = &(pque_profile->queue[pqueue->id]);
	if((pold->cir == pqueue->cir) &&
	   (pold->pir == pqueue->pir) &&
	   (pold->weight == pqueue->weight) &&
	   (pold->wred_id == pqueue->wred_id))
	{
		return ERRNO_SUCCESS;
	}
#if 0
	ret = ipc_send_hal_wait_ack(pqueue, sizeof(struct hqos_queue), 1, MODULE_ID_HAL, MODULE_ID_QOS,
						IPC_TYPE_HQOS, HQOS_SUBTYPE_QUEUE, IPC_OPCODE_ADD, pque_profile->id);
#else
	ret = qos_ipc_send_and_wait_ack(pqueue, sizeof(struct hqos_queue), 1, MODULE_ID_HAL, MODULE_ID_QOS,
						IPC_TYPE_HQOS, HQOS_SUBTYPE_QUEUE, IPC_OPCODE_ADD, pque_profile->id);
#endif

	if(ret)
    {
        QOS_LOG_ERR("ipc_send_hal_wait_ack (queue rule) failed.\n");
        return ret;
    }

    if (pold->wred_id)
    {
        pwred = hqos_wred_profile_lookup(pold->wred_id);
        if (NULL != pwred)
        {
            hqos_wred_profile_attached(pwred, 0);
        }
    }

	memcpy(&(pque_profile->queue[pqueue->id]), pqueue, sizeof(struct hqos_queue));

	return ERRNO_SUCCESS;
}


/************************************************
 * Function: hqos_queue_profile_set_scheduler
 * Input:
 *      pque_profile: pointer to queue profile that scheduler
 *			   		  to be set
 *      sheduler: scheduler mode
 * Output:
 *      NONE
 * Return:
 *      ERRCODE
 * Description:
 *      Set the queue scheduler mode.
 ************************************************/
int hqos_queue_profile_set_scheduler(struct queue_profile *pque_profile, enum HQOS_SCHEDULER scheduler)
{
	int ret = 0;

	assert(NULL != pque_profile);

	if(pque_profile->ref_cnt)
	{
		return QOS_ERR_ATTACHED;
	}

	if(pque_profile->scheduler == scheduler)
	{
		return ERRNO_SUCCESS;
	}
#if 0
	ret = ipc_send_hal_wait_ack(&scheduler, sizeof(enum HQOS_SCHEDULER), 1, MODULE_ID_HAL, MODULE_ID_QOS,
						IPC_TYPE_HQOS, HQOS_SUBTYPE_QUEUE_SCHEDULER, IPC_OPCODE_ADD, pque_profile->id);
#else
	ret = qos_ipc_send_and_wait_ack(&scheduler, sizeof(enum HQOS_SCHEDULER), 1, MODULE_ID_HAL, MODULE_ID_QOS,
						IPC_TYPE_HQOS, HQOS_SUBTYPE_QUEUE_SCHEDULER, IPC_OPCODE_ADD, pque_profile->id);
#endif

	if(ret)
    {
        QOS_LOG_ERR("ipc_send_hal_wait_ack (queue rule) failed.\n");
        return ret;
    }

	pque_profile->scheduler = scheduler;

	return ERRNO_SUCCESS;
}


/************************************************
 * Function: hqos_queue_profile_get_bulk
 * Input:
 *      index: queue profile index
 * Output:
 *      queue_array: queue data
 * Return:
 *      data num
 * Description:
 *      Get queue profile data.
 ************************************************/
int hqos_queue_profile_get_bulk ( uint8_t index, struct queue_profile *queue_array )
{
	int msg_num = IPC_MSG_LEN/sizeof(struct queue_profile);
    int data_num = 0;
    int i = 0;

    if ( index == 0 )
    {
    	for (i=0; i<HQOS_QUEUE_PROFILE_NUM; i++)
	    {
	        if (gque_profile[i] != NULL)
	        {
	        	memcpy(&queue_array[data_num++], gque_profile[i], sizeof(struct queue_profile));
	        	if(data_num == msg_num)
	        	{
	        		return data_num;
	        	}
	        }
	    }
    }
    else
    {
    	for (i=++index; i<HQOS_QUEUE_PROFILE_NUM+1; i++)
	    {
	        if (NULL == gque_profile[i-1])
	        {
	        	continue;
	        }

	        memcpy(&queue_array[data_num++], gque_profile[i-1], sizeof(struct queue_profile));
	        if(data_num == msg_num)
	        {
	        	return data_num;
	        }
	    }
    }

    return data_num;
}


/************************************************
 * Function: hqos_wred_profile_attached
 * Input:
 *      pwred : pointer to structure of wred profile
 *      flag  : 0 for decreace 1, others for increace 1
 * Output:
 *      NONE
 * Return:
 *      ERRCODE
 * Description:
 *      Operate wred profile reference count.
 ************************************************/
void hqos_wred_profile_attached (struct wred_profile *pwred, int flag)
{
    if(flag)
    {
        pwred->ref_cnt++;
    }
    else
    {
        if(pwred->ref_cnt > 0)
            pwred->ref_cnt--;
    }

    return;
}


/************************************************
 * Function: hqos_queue_profile_attached
 * Input:
 *      pqueue : pointer to structure of queue profile
 *      flag  : 0 for decreace 1, others for increace 1
 * Output:
 *      NONE
 * Return:
 *      ERRCODE
 * Description:
 *      Operate queue profile reference count.
 ************************************************/
void hqos_queue_profile_attached (struct queue_profile *pqueue, int flag)
{
    if(flag)
    {
        pqueue->ref_cnt++;
    }
    else
    {
        if(pqueue->ref_cnt > 0)
            pqueue->ref_cnt--;
    }

    return;
}


/************************************************
 * Function: hqos_wred_profile_show_operation
 * Input:
 *      pwred : pointer to structure of wred profile
 *      vty   : struct vty
 * Output:
 *      NONE
 * Return:
 *      ERRNO
 * Description:
 *      Show qos wred profile.
 ************************************************/
static int hqos_wred_profile_show_operation(struct wred_profile *pwred, struct vty *vty)
{

    PROFILE_LINE_CHECK(profile_line, vty->flush_cp)
    {
        vty_out(vty, "Profile ID:%d, ref_cnt:%d%s", pwred->id, pwred->ref_cnt, VTY_NEWLINE);
    }

    PROFILE_LINE_CHECK(profile_line, vty->flush_cp)
    {
        vty_out(vty, "==========================================================================%s", VTY_NEWLINE);
    }

    PROFILE_LINE_CHECK(profile_line, vty->flush_cp)
    {
        vty_out(vty, " %-10s%-10s%-19s%-20s%-15s%s", "color", "protocol", "drop low limit", "drop high limit", "max drop ratio", VTY_NEWLINE);
    }

    PROFILE_LINE_CHECK(profile_line, vty->flush_cp)
    {
        vty_out(vty, "--------------------------------------------------------------------------%s", VTY_NEWLINE);
    }

    PROFILE_LINE_CHECK(profile_line, vty->flush_cp)
    {
        vty_out(vty, " %-10s%-15s%-19d%-20d%d%s", "green", "Tcp",
                pwred->wred[0][0].drop_low, pwred->wred[0][0].drop_high, pwred->wred[0][0].drop_ratio, VTY_NEWLINE);
    }

    PROFILE_LINE_CHECK(profile_line, vty->flush_cp)
    {
        vty_out(vty, " %-10s%-15s%-19d%-20d%d%s", "green", "non-Tcp",
                pwred->wred[0][1].drop_low, pwred->wred[0][1].drop_high, pwred->wred[0][1].drop_ratio, VTY_NEWLINE);
    }

    PROFILE_LINE_CHECK(profile_line, vty->flush_cp)
    {
        vty_out(vty, " %-10s%-15s%-19d%-20d%d%s", "yellow", "Tcp",
                pwred->wred[1][0].drop_low, pwred->wred[1][0].drop_high, pwred->wred[1][0].drop_ratio, VTY_NEWLINE);
    }

    PROFILE_LINE_CHECK(profile_line, vty->flush_cp)
    {
        vty_out(vty, " %-10s%-15s%-19d%-20d%d%s", "yellow", "non-Tcp",
                pwred->wred[1][1].drop_low, pwred->wred[1][1].drop_high, pwred->wred[1][1].drop_ratio, VTY_NEWLINE);
    }

    PROFILE_LINE_CHECK(profile_line, vty->flush_cp)
    {
        vty_out(vty, " %-10s%-15s%-19d%-20d%d%s", "red", "Tcp",
                pwred->wred[2][0].drop_low, pwred->wred[2][0].drop_high, pwred->wred[2][0].drop_ratio, VTY_NEWLINE);
    }

    PROFILE_LINE_CHECK(profile_line, vty->flush_cp)
    {
        vty_out(vty, " %-10s%-15s%-19d%-20d%d%s", "red", "non-Tcp",
                pwred->wred[2][1].drop_low, pwred->wred[2][1].drop_high, pwred->wred[2][1].drop_ratio, VTY_NEWLINE);
    }

    PROFILE_LINE_CHECK(profile_line, vty->flush_cp)
    {
        vty_out(vty, "==========================================================================%s%s", VTY_NEWLINE, VTY_NEWLINE);
    }

    PROFILE_LINE_CHECK(profile_line, vty->flush_cp)
    {
        vty_out(vty, "%s", VTY_NEWLINE);
    }

    return ERRNO_SUCCESS;
}


/************************************************
 * Function: hqos_queue_profile_show_operation
 * Input:
 *      pwred : pointer to structure of wred profile
 *      vty   : struct vty
 * Output:
 *      NONE
 * Return:
 *      NONE
 * Description:
 *      Operate queue profile reference count.
 ************************************************/
static int hqos_queue_profile_show_operation(struct queue_profile *pqueue, struct vty *vty)
{
    int i = 0;

	PROFILE_LINE_CHECK(profile_line, vty->flush_cp)
	vty_out(vty, "Profile ID:%d, ref_cnt:%d%s", pqueue->id, pqueue->ref_cnt, VTY_NEWLINE);
	PROFILE_LINE_CHECK(profile_line, vty->flush_cp)
    vty_out(vty, "Scheduler:%s%s", (HQOS_SCHEDULER_PQ == pqueue->scheduler) ? "Priority Queue" :
    								(HQOS_SCHEDULER_WDRR == pqueue->scheduler) ? "Weight Deficit Round Robin" :
    								"Weight Round Robin", VTY_NEWLINE);
	PROFILE_LINE_CHECK(profile_line, vty->flush_cp)
   	vty_out(vty, "=====================================================%s", VTY_NEWLINE);
	PROFILE_LINE_CHECK(profile_line, vty->flush_cp)
    vty_out(vty, " %-9s%-11s%-10s%-12s%-10s%s","queue","wred_id","weight","cir(kbps)","pir(kbps)", VTY_NEWLINE);
	PROFILE_LINE_CHECK(profile_line, vty->flush_cp)
    vty_out(vty, "-----------------------------------------------------%s", VTY_NEWLINE);
    for(i = 0; i < 8; i++)
    {
    	PROFILE_LINE_CHECK(profile_line, vty->flush_cp)
    	vty_out(vty, "   %-9d%-11d%-8d%-12d%d%s",
    			pqueue->queue[i].id, pqueue->queue[i].wred_id, pqueue->queue[i].weight,
    			   pqueue->queue[i].cir, pqueue->queue[i].pir, VTY_NEWLINE);
    }
    PROFILE_LINE_CHECK(profile_line, vty->flush_cp)
    vty_out(vty, "=====================================================%s", VTY_NEWLINE);
    PROFILE_LINE_CHECK(profile_line, vty->flush_cp)
	vty_out(vty, "%s", VTY_NEWLINE);

    return ERRNO_SUCCESS;
}


DEFUN(show_hqos_wred_profile,
        show_hqos_wred_profile_cmd,
        "show qos wred-profile [<1-10>]",
        SHOW_STR
        "Quality of Service\n"
        "QoS wred-profile\n"
        "QoS wred-profile ID\n")
{
	uint8_t wred_id;
    struct wred_profile *pwred = NULL;
    int i = 0;
    int ret;

    if (vty->flush_cp == 0xFFFFFFFF)
    {
        return CMD_SUCCESS;
    }

    if(NULL != argv[0])
    {
        wred_id = atoi( argv[0] );

        pwred = hqos_wred_profile_lookup(wred_id);
        if (NULL != pwred)
        {
            ret = hqos_wred_profile_show_operation(pwred, vty);
            if(ret == CMD_CONTINUE)
            {
            	return CMD_CONTINUE;
            }
        }
        else
        {
            return CMD_SUCCESS;
        }
    }
    else
    {
        for(i = 0; i < HQOS_WRED_PROFILE_NUM; i++)
        {
        	if (NULL != gwred_profile[i])
	        {
	            ret = hqos_wred_profile_show_operation(gwred_profile[i], vty);
	            if(ret == CMD_CONTINUE)
	            {
	            	return CMD_CONTINUE;
	            }
	        }
        }
    }

	profile_line = 0;
	return CMD_SUCCESS;
}


DEFUN(show_hqos_queue_profile,
        show_hqos_queue_profile_cmd,
        "show qos queue-profile [<1-100>]",
        SHOW_STR
        "Quality of Service\n"
        "QoS queue-profile\n"
        "QoS queue-profile ID\n")
{
	uint8_t queue_profile;
    struct queue_profile *pqueue = NULL;
    int i = 0;
    int ret;

    if (vty->flush_cp == 0xFFFFFFFF)
    {
        return CMD_SUCCESS;
    }

    if(NULL != argv[0])
    {
        queue_profile = atoi( argv[0] );

        pqueue = hqos_queue_profile_lookup(queue_profile);
        if (NULL != pqueue)
        {
            ret = hqos_queue_profile_show_operation(pqueue, vty);
            if(ret == CMD_CONTINUE)
            {
            	return CMD_CONTINUE;
            }
        }
        else
        {
            return CMD_SUCCESS;
        }
    }
    else
    {
        for(i = 0; i < HQOS_QUEUE_PROFILE_NUM; i++)
        {
        	if (NULL != gque_profile[i])
	        {
	            ret = hqos_queue_profile_show_operation(gque_profile[i], vty);
	            if(ret == CMD_CONTINUE)
	            {
	            	return CMD_CONTINUE;
	            }
	        }
        }
    }

	profile_line = 0;
	return CMD_SUCCESS;
}


DEFUN(hqos_wred_profile,
        hqos_wred_profile_cmd,
        "qos wred-profile <1-10>",
        "Quality of Service\n"
        "QoS wred-profile\n"
        "QoS wred-profile ID\n")
{
	struct wred_profile wred;
	int ret = 0;
    int i = 0, j = 0;
   
	memset(&wred, 0, sizeof(struct wred_profile));
	VTY_GET_INTEGER_RANGE("Wred profile index", wred.id, argv[0], 1, HQOS_WRED_PROFILE_NUM);

    /* init info */
    for ( i = QOS_COLOR_GREEN; i <= QOS_COLOR_RED; i++ )
    {
    	for (j = WRED_PROTOCOL_TCP; j < WRED_PROTOCOL_ALL; j++)
    	{
            wred.wred[i][j].color = i;
            wred.wred[i][j].protocol = j;
    	}
    }
    
	ret = hqos_wred_profile_add(&wred);
	if(ret)
    {
        vty_error_out(vty, "Wred profile created failed!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	vty->index = (void *)(int)wred.id;
    vty->node  = HQOS_WRED_NODE;

	return CMD_SUCCESS;
}


DEFUN(no_hqos_wred_profile,
        no_hqos_wred_profile_cmd,
        "no qos wred-profile <1-10>",
        NO_STR
        "Quality of Service\n"
        "QoS wred-profile\n"
        "QoS wred-profile ID\n")
{
	int ret = 0;
	uint8_t wred_id = 0;

	VTY_GET_INTEGER_RANGE("Wred profile index", wred_id, argv[0], 1, HQOS_WRED_PROFILE_NUM);

	ret = hqos_wred_profile_delete(wred_id);
	if(ret)
	{
		if(QOS_ERR_ATTACHED == ret)
		{
			vty_error_out ( vty, "Wred profile %d was applied, please detach first!!%s", wred_id, VTY_NEWLINE);
		}
		else
		{
			vty_error_out(vty, "Wred profile delete failed!!%s", VTY_NEWLINE);
		}

		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}


DEFUN(hqos_wred_rule,
      hqos_wred_rule_cmd,
      "color (green|yellow|red) protocol (tcp|non-tcp|all) "
      "drop-threshold low <1-100> high <1-100> ratio <1-100>",
      "Specify color\n"
      "Green\n"
      "Yellow\n"
      "Red\n"
      "Specify protocol\n"
      "Tcp\n"
      "None-Tcp\n"
      "Do not care protocol\n"
      "Packet drop threshold\n"
      "Low limit\n"
      "Low limit Percentage\n"
      "High limit\n"
      "High limit Percentage\n"
      "Max drop ratio\n"
      "Max drop ratio value\n")
{
    struct wred_profile *pwred = NULL;
    struct wred_rule     rule;
    uint8_t index = 0;
    int ret = 0;

    index = (uint8_t)(int)vty->index;

    pwred = hqos_wred_profile_lookup(index);
    if (NULL == pwred)
    {
        vty_error_out(vty, "Wred profile info error!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memset(&rule, 0, sizeof(struct wred_rule));
    switch (argv[0][0])
    {
    case 'g':
        rule.color = QOS_COLOR_GREEN;
        break;
    case 'y':
        rule.color = QOS_COLOR_YELLOW;
        break;
    case 'r':
        rule.color = QOS_COLOR_RED;
        break;
    default:
        return CMD_WARNING;
    }

    switch (argv[1][0])
    {
    case 't':
        rule.protocol = WRED_PROTOCOL_TCP;
        break;
    case 'n':
        rule.protocol = WRED_PROTOCOL_NON_TCP;
        break;
    case 'a':
        rule.protocol = WRED_PROTOCOL_ALL;
        break;
    default:
        return CMD_WARNING;
    }

    VTY_GET_INTEGER_RANGE("Wred low limit", rule.drop_low, argv[2], HQOS_WRED_DROP_LOW, HQOS_WRED_DROP_HIGH);
    VTY_GET_INTEGER_RANGE("Wred high limit", rule.drop_high, argv[3], HQOS_WRED_DROP_LOW, HQOS_WRED_DROP_HIGH);
    VTY_GET_INTEGER_RANGE("Wred max drop ratio", rule.drop_ratio, argv[4], HQOS_WRED_DROP_LOW, HQOS_WRED_DROP_HIGH);

    ret = hqos_wred_profile_set_rule(pwred, &rule);
    if (ret)
    {
        if (QOS_ERR_ILL_DROPHIGH == ret)
        {
            vty_error_out(vty, "Drop high limit must larger than low limit or equal to low limit.%s", VTY_NEWLINE);
        }
        else if (QOS_ERR_ATTACHED == ret)
        {
            vty_error_out(vty, "Wred profile was referenced, not permit to modify.%s", VTY_NEWLINE);
        }
        else
        {
            vty_error_out(vty, "Failed to set wred rule!!%s", VTY_NEWLINE);
        }
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN(no_hqos_wred_rule,
      no_hqos_wred_rule_cmd,
      "no color (green|yellow|red) protocol (tcp|non-tcp|all)",
      NO_STR
      "Specify color\n"
      "Specify protocol\n"
      "Tcp\n"
      "None-Tcp\n"
      "Do not care protocol\n"
      "Green\n"
      "Yellow\n"
      "Red\n")
{
    struct wred_profile *pwred = NULL;
    struct wred_rule     rule;
    uint8_t index = 0;
    int ret = 0;

    index = (uint8_t)(int)vty->index;

    pwred = hqos_wred_profile_lookup(index);
    if (NULL == pwred)
    {
        vty_error_out(vty, "Wred profile info error!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memset(&rule, 0, sizeof(struct wred_rule));
    switch (argv[0][0])
    {
    case 'g':
        rule.color = QOS_COLOR_GREEN;
        break;
    case 'y':
        rule.color = QOS_COLOR_YELLOW;
        break;
    case 'r':
        rule.color = QOS_COLOR_RED;
        break;
    default:
        return CMD_WARNING;
    }

    switch (argv[1][0])
    {
    case 't':
        rule.protocol = WRED_PROTOCOL_TCP;
        break;
    case 'n':
        rule.protocol = WRED_PROTOCOL_NON_TCP;
        break;
    case 'a':
        rule.protocol = WRED_PROTOCOL_ALL;
        break;
    default:
        return CMD_WARNING;
    }

    rule.drop_low = 0;
    rule.drop_high = 0;
    rule.drop_ratio = 0;

    ret = hqos_wred_profile_set_rule(pwred, &rule);
    if (ret)
    {
        if (QOS_ERR_ATTACHED == ret)
        {
            vty_error_out(vty, "Wred profile was referenced, not permit to modify.%s", VTY_NEWLINE);
        }
        else
        {
            vty_error_out(vty, "Failed to set wred rule!!%s", VTY_NEWLINE);
        }
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN(hqos_queue_profile,
        hqos_queue_profile_cmd,
        "qos queue-profile <1-100>",
        "Quality of Service\n"
        "QoS queue-profile\n"
        "QoS queue-profile ID\n")
{
	struct queue_profile queue;
	int i = 0;
	int ret = 0;

	memset(&queue, 0, sizeof(struct queue_profile));
	VTY_GET_INTEGER_RANGE("Queue profile index", queue.id, argv[0], 1, HQOS_QUEUE_PROFILE_NUM);
	queue.scheduler = HQOS_SCHEDULER_PQ;

	for(i=0; i<8; i++)
	{
		queue.queue[i].id = i;
	}

	ret = hqos_queue_profile_add(&queue);
	if(ret)
    {
        vty_error_out(vty, "Queue profile created failed!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	vty->index = (void *)(int)queue.id;
    vty->node  = HQOS_QUEUE_NODE;

	return CMD_SUCCESS;
}


DEFUN(no_hqos_queue_profile,
        no_hqos_queue_profile_cmd,
        "no qos queue-profile <1-100>",
        NO_STR
        "Quality of Service\n"
        "QoS queue-profile\n"
        "QoS queue-profile ID\n")
{
	int ret = 0;
	uint8_t profile_id = 0;

	VTY_GET_INTEGER_RANGE("Queue profile index", profile_id, argv[0], 1, HQOS_QUEUE_PROFILE_NUM);

	ret = hqos_queue_profile_delete(profile_id);
	if(ret)
	{
		if(QOS_ERR_ATTACHED == ret)
		{
			vty_error_out ( vty, "Queue profile %d was applied, please detach first!!%s", profile_id, VTY_NEWLINE);
		}
		else
		{
			vty_error_out(vty, "Queue profile delete failed!!%s", VTY_NEWLINE);
		}

		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}


DEFUN(hqos_queue_rule,
        hqos_queue_rule_cmd,
        "queue <0-7> weight <0-127> cir <0-10000000> {pir <0-10000000> | wred-profile <1-10>}",
        "Queue\n"
        "Queue value\n"
        "Weight\n"
        "Weight value\n"
        "Committed information rate\n"
        "Committed information rate value (Unit: Kbps)\n"
        "Peak information rate\n"
        "Peak information rate value (Unit: Kbps)\n"
        "Wred profile\n"
        "Wred profile ID\n")
{
	struct queue_profile *pqueue = NULL;
	struct wred_profile  *pwred  = NULL;
	struct hqos_queue     rule;
	uint8_t index = 0;
	int ret = 0;

	index = (uint8_t)(int)vty->index;

	pqueue = hqos_queue_profile_lookup(index);
	if(NULL == pqueue)
	{
		vty_error_out(vty, "Hqos queue profile info error!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	memset(&rule, 0, sizeof(struct hqos_queue));
	VTY_GET_INTEGER_RANGE("Queue id", rule.id, argv[0], HQOS_QUEUE_MIN, HQOS_QUEUE_MAX);
    VTY_GET_INTEGER_RANGE("Queue weight", rule.weight, argv[1], HQOS_QUEUE_WEIGHT_MIN, HQOS_QUEUE_WEIGHT_MAX);
    VTY_GET_INTEGER_RANGE("Committed information rate", rule.cir, argv[2], 0, QOS_CAR_CIR_MAX);

    if(NULL != argv[3])
    {
        VTY_GET_INTEGER_RANGE("Peak information rate", rule.pir, argv[3], 0, QOS_CAR_PIR_MAX);
        if(rule.pir < rule.cir)
		{
			vty_error_out(vty, "PIR must larger than CIR or equal to CIR.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
    }

    if(NULL != argv[4])
    {
        VTY_GET_INTEGER_RANGE("Wred profile id", rule.wred_id, argv[4], 1, HQOS_WRED_PROFILE_NUM);

        pwred = hqos_wred_profile_lookup(rule.wred_id);
        if(NULL == pwred)
        {
        	vty_error_out(vty, "The specified WRED profile %d is not exist!!%s", rule.wred_id, VTY_NEWLINE);
        	return CMD_WARNING;
        }
    }
    else
    {
        rule.wred_id = DISABLE;
    }

	ret = hqos_queue_profile_set_queue(pqueue, &rule);
	if(ret)
	{
		if(QOS_ERR_ATTACHED == ret)
		{
			vty_error_out(vty, "Queue profile was referenced, not permit to modify.%s", VTY_NEWLINE);
		}
		else
		{
			vty_error_out(vty, "Failed to set hqos queue rule!!%s", VTY_NEWLINE);
		}
		return CMD_WARNING;
	}

	if(rule.wred_id)
	{
		hqos_wred_profile_attached(pwred, 1);
	}

	return CMD_SUCCESS;
}


DEFUN(no_hqos_queue_rule,
        no_hqos_queue_rule_cmd,
        "no queue <0-7>",
        NO_STR
        "Queue\n"
        "Queue value\n")
{
	struct queue_profile *pqueue = NULL;
	struct wred_profile  *pwred  = NULL;
	struct hqos_queue     rule;
	uint8_t queue_id;
	uint8_t wred_id;
	uint8_t index = 0;
	int ret = 0;

	index = (uint8_t)(int)vty->index;

	pqueue = hqos_queue_profile_lookup(index);
	if(NULL == pqueue)
	{
		vty_error_out(vty, "Hqos queue profile %d info error!!%s", index, VTY_NEWLINE);
		return CMD_WARNING;
	}

	VTY_GET_INTEGER_RANGE("Queue id", queue_id, argv[0], HQOS_QUEUE_MIN, HQOS_QUEUE_MAX);

    memset(&rule, 0, sizeof(struct hqos_queue));
    rule.id = queue_id;

    wred_id = pqueue->queue[queue_id].wred_id;

	ret = hqos_queue_profile_set_queue(pqueue, &rule);
	if(ret)
	{
		if(QOS_ERR_ATTACHED == ret)
		{
			vty_error_out(vty, "Queue profile was referenced, not permit to modify.%s", VTY_NEWLINE);
		}
		else
		{
			vty_error_out(vty, "Failed to set hqos queue rule!!%s", VTY_NEWLINE);
		}
		return CMD_WARNING;
	}

	/* modify success, then operate ref_cnt */
	if(0 != wred_id)
	{
		pwred = hqos_wred_profile_lookup(wred_id);
		if(NULL == pwred)
		{
			vty_error_out(vty, "Hqos wred profile info error!!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}

		hqos_wred_profile_attached(pwred, 0);
	}

	return CMD_SUCCESS;
}


DEFUN(hqos_queue_scheduler,
        hqos_queue_scheduler_cmd,
        "scheduler (pq|wrr|wdrr)",
        "Queue scheduler\n"
        "Priority queue\n"
        "Weight Round Robin\n"
        "Weight Deficit Round Robin\n")
{
	uint8_t index;
	struct queue_profile *pqueue = NULL;
	enum HQOS_SCHEDULER scheduler = HQOS_SCHEDULER_PQ;
	int ret = 0;

	index = (uint8_t)(int)vty->index;
	pqueue = hqos_queue_profile_lookup(index);
	if(NULL == pqueue)
	{
		vty_error_out(vty, "Hqos queue profile info error!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if (strncmp ( argv[0], "pq", 2 ) == 0 )
    {
		scheduler = HQOS_SCHEDULER_PQ;
    }
    else if ( strncmp ( argv[0], "wrr", 3 ) == 0 )
    {
		scheduler = HQOS_SCHEDULER_WRR;
    }
    else if ( strncmp ( argv[0], "wdrr", 4 ) == 0 )
    {
		scheduler = HQOS_SCHEDULER_WDRR;
    }

	ret = hqos_queue_profile_set_scheduler(pqueue, scheduler);
	if(ret)
	{
		if(QOS_ERR_ATTACHED == ret)
		{
			vty_error_out(vty, "Queue profile was referenced, not permit to modify.%s", VTY_NEWLINE);
		}
		else
		{
			vty_error_out(vty, "Failed to set hqos queue scheduler mode!!%s", VTY_NEWLINE);
		}
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}


DEFUN(no_hqos_queue_scheduler,
        no_hqos_queue_scheduler_cmd,
        "no scheduler",
        NO_STR
        "Queue scheduler\n")
{
	uint8_t index;
	struct queue_profile *pqueue = NULL;
	enum HQOS_SCHEDULER scheduler = HQOS_SCHEDULER_PQ;
	int ret = 0;

	index = (uint8_t)(int)vty->index;
	pqueue = hqos_queue_profile_lookup(index);
	if(NULL == pqueue)
	{
		vty_error_out(vty, "Hqos queue profile info error!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	ret = hqos_queue_profile_set_scheduler(pqueue, scheduler);
	if(ret)
	{
		if(QOS_ERR_ATTACHED == ret)
		{
			vty_error_out(vty, "Queue profile was referenced, not permit to modify.%s", VTY_NEWLINE);
		}
		else
		{
			vty_error_out(vty, "Failed to reset hqos queue scheduler mode!!%s", VTY_NEWLINE);
		}

		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}


static struct cmd_node hqos_wred_node =
{
    HQOS_WRED_NODE,
    "%s(config-qos-wred)# ",
    1
};


static struct cmd_node hqos_queue_node =
{
    HQOS_QUEUE_NODE,
    "%s(config-qos-queue)# ",
    1
};


/************************************************
 * Function: hqos_config_write
 * Input:
 *      vty: struct vty
 * Output:
 *      NONE
 * Return:
 *      ERRCODE
 * Description:
 *      Save hqos configuration.
 ************************************************/
static int hqos_config_write (struct vty *vty)
{
    int i = 0, j = 0, k = 0;
    enum QOS_COLOR color;
    enum HQOS_SCHEDULER sche;
	enum WRED_PROTOCOL protocol;
    uint8_t  weight;
	uint8_t  wred_id;
    uint32_t cir;
    uint32_t pir;

	for (i=0; i<HQOS_WRED_PROFILE_NUM; i++)
    {
        if (NULL == gwred_profile[i])
        {
            continue;
        }

        vty_out(vty, "qos wred-profile %d%s", gwred_profile[i]->id, VTY_NEWLINE);

        for (j = 0; j < 3; j++)
        {
            for (k = 0; k < 2; k++)
            {
                if ((gwred_profile[i]->wred[j][k].drop_low != 0)
                        || (gwred_profile[i]->wred[j][k].drop_high != 0)
                        || (gwred_profile[i]->wred[j][k].drop_ratio != 0))
                {
                    color    = gwred_profile[i]->wred[j][k].color;
                    protocol = gwred_profile[i]->wred[j][k].protocol;

                    vty_out(vty, " color %s protocol %s drop-threshold low %d high %d ratio %d%s",
                            color == QOS_COLOR_GREEN ? "green" : color == QOS_COLOR_YELLOW ? "yellow" : "red",
                            protocol == WRED_PROTOCOL_TCP ? "tcp" : "non-tcp", gwred_profile[i]->wred[j][k].drop_low,
                            gwred_profile[i]->wred[j][k].drop_high, gwred_profile[i]->wred[j][k].drop_ratio, VTY_NEWLINE);
                }
            }
        }
    }

    for (i=0; i<HQOS_QUEUE_PROFILE_NUM; i++)
    {
        if (NULL != gque_profile[i])
        {
            vty_out(vty, "qos queue-profile %d%s", gque_profile[i]->id, VTY_NEWLINE);

			sche = gque_profile[i]->scheduler;
			if(HQOS_SCHEDULER_PQ != sche)
			{
				vty_out(vty, " scheduler %s%s", sche == HQOS_SCHEDULER_WRR ? "wrr" : "wdrr", VTY_NEWLINE);
			}

            for (j=0; j<8; j++)
            {
            	cir = gque_profile[i]->queue[j].cir;
            	pir = gque_profile[i]->queue[j].pir;
            	wred_id = gque_profile[i]->queue[j].wred_id;
            	weight = gque_profile[i]->queue[j].weight;

                if((weight != 0) ||(wred_id != 0) ||
                	(cir != 0) || (pir != 0))
                {
					vty_out(vty, " queue %d weight %d cir %d", j, weight, cir);

					if(pir != 0)
					{
						vty_out(vty, " pir %d", pir);
					}

					if(wred_id != 0)
					{
						vty_out(vty, " wred-profile %d", wred_id);
					}

					vty_out(vty, "%s", VTY_NEWLINE);
                }
            }
        }
    }

    return ERRNO_SUCCESS;
}


/************************************************
 * Function: hqos_cmd_init
 * Input:
 *      NONE
 * Output:
 *      NONE
 * Return:
 *      NONE
 * Description:
 *      Initialize hqos command.
 ************************************************/
void hqos_cmd_init(void)
{
    install_node(&hqos_wred_node, hqos_config_write);
    install_default(HQOS_WRED_NODE);

    install_node(&hqos_queue_node, NULL);
    install_default(HQOS_QUEUE_NODE);

    install_element(CONFIG_NODE, &hqos_wred_profile_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &no_hqos_wred_profile_cmd, CMD_SYNC);
    install_element(HQOS_WRED_NODE, &hqos_wred_rule_cmd, CMD_SYNC);
    install_element(HQOS_WRED_NODE, &no_hqos_wred_rule_cmd, CMD_SYNC);

 	install_element(CONFIG_NODE, &hqos_queue_profile_cmd, CMD_SYNC);
 	install_element(CONFIG_NODE, &no_hqos_queue_profile_cmd, CMD_SYNC);
    install_element(HQOS_QUEUE_NODE, &hqos_queue_rule_cmd, CMD_SYNC);
    install_element(HQOS_QUEUE_NODE, &no_hqos_queue_rule_cmd, CMD_SYNC);
 	install_element(HQOS_QUEUE_NODE, &hqos_queue_scheduler_cmd, CMD_SYNC);
 	install_element(HQOS_QUEUE_NODE, &no_hqos_queue_scheduler_cmd, CMD_SYNC);

 	install_element(CONFIG_NODE, &show_hqos_wred_profile_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_hqos_queue_profile_cmd, CMD_LOCAL);
}





