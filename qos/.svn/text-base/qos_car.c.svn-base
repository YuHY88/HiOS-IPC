#include <qos/qos_car.h>
#include <lib/errcode.h>
#include <lib/memtypes.h>
#include <lib/memory.h>
#include <lib/command.h>
#include <lib/zassert.h>
#include <lib/module_id.h>
#include <lib/msg_ipc.h>
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>
#include <lib/index.h>
#include <lib/hash1.h>
#include <lib/ifm_common.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "qos_msg.h"

struct hash_table qos_car_profile;
uint32_t car_profile_line = 0;        /* 存储回显行数 */


static unsigned int qos_car_profile_compute(void *hash_key)
{
    return (uint32_t)hash_key;
}

static int qos_car_profile_compare(void *item, void *key)
{
    struct hash_bucket *pbucket = item;

    if(NULL == item || NULL == key)
        return ERRNO_FAIL;

    if (pbucket->hash_key == key)
        return ERRNO_SUCCESS;

    return ERRNO_FAIL;
}

void qos_car_init(int size)
{
    hios_hash_init(&qos_car_profile, size, qos_car_profile_compute, qos_car_profile_compare);
    index_register(INDEX_TYPE_CAR, 6144); /* 修改为整机接口规格 */
}

struct car_t *qos_car_profile_lookup(uint32_t car_id)
{
    struct hash_bucket *pbucket = NULL;

    pbucket = hios_hash_find(&qos_car_profile, (void *)car_id);
    if(NULL == pbucket)
    {
        QOS_LOG_DBG("Qos car profile %d lookup failed!!\n", car_id);
        return NULL;
    }

    return (struct car_t *)pbucket->data;
}

struct car_t *qos_car_create(struct car_t *pcar)
{
    struct car_t *pcar_new = NULL;
    struct car_t *pcar_lookup = NULL;
    int ret;

    pcar_lookup = qos_car_profile_lookup(pcar->car_id);
    if(NULL != pcar_lookup)
    {
        QOS_LOG_DBG("The qos car profile item is already exist.\n");
        return pcar_lookup;
    }
#if 0
    ret = ipc_send_hal_wait_ack((void *)pcar, sizeof(struct car_t), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_CAR, CAR_INFO_CARID, IPC_OPCODE_ADD, pcar->car_id);
#else
	ret = qos_ipc_send_and_wait_ack((void *)pcar, sizeof(struct car_t), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_CAR, CAR_INFO_CARID, IPC_OPCODE_ADD, pcar->car_id);
#endif
    if(ret)
    {
        QOS_LOG_ERR("ipc_send_hal_wait_ack failed.\n");
        return NULL;
    }

    pcar_new = (struct car_t *)XCALLOC(MTYPE_QOS_ENTRY, sizeof(struct car_t));
    if(NULL == pcar_new)
    {
        QOS_LOG_ERR("The memory is insufficient.\n");
        return NULL;
    }

    memcpy(pcar_new, pcar, sizeof(struct car_t));

    ret = qos_car_profile_add(pcar_new);
    if(0 != ret)
    {
        QOS_LOG_ERR("Qos car profile add to hash failed, car_id = %d!!\n", pcar->car_id);
        XFREE(MTYPE_QOS_ENTRY, pcar_new);
        return NULL;
    }
    return pcar_new;
}


int qos_car_profile_add(struct car_t *pcar)
{
    struct hash_bucket *pbucket = NULL;
    int ret;

    pbucket = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
    if(NULL == pbucket)
    {
        QOS_LOG_ERR("The memory is insufficient.\n");
        return ERRNO_MALLOC;
    }

    pbucket->hash_key = (void *)(int)pcar->car_id;
    pbucket->data = (void *)pcar;

    ret = hios_hash_add(&qos_car_profile, pbucket);
    if(0 != ret)
    {
        QOS_LOG_ERR("hios hash add failed!!\n");
        XFREE(MTYPE_HASH_BACKET, pbucket);
        return ERRNO_FAIL;
    }

    return ERRNO_SUCCESS;
}

int qos_car_profile_delete(uint32_t car_id)
{
    struct car_t *pcar = NULL;
    struct hash_bucket *pbucket = NULL;
    int ret;

    pbucket = hios_hash_find(&qos_car_profile, (void *)car_id);
    if(NULL == pbucket)
    {
        QOS_LOG_DBG("The qos car profile %d is not exist!!\n", car_id);
        return ERRNO_SUCCESS;
    }

    pcar = (struct car_t *)pbucket->data;
    if(NULL == pcar)
    {
        XFREE(MTYPE_HASH_BACKET, pbucket);
        return ERRNO_SUCCESS;
    }

    if(0 != pcar->ref_cnt)
    {
        QOS_LOG_ERR("The qos car profile %d was applied to interface!!\n", car_id);
        return QOS_ERR_ATTACHED;
    }

    if(QOS_CAR_PROFILE_SHARED == pcar->shared)
    {
    	#if 0
    	ret = ipc_send_hal_wait_ack(NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_QOS,
                            IPC_TYPE_CAR, CAR_INFO_CARID, IPC_OPCODE_DELETE, car_id);
        #else
        ret = qos_ipc_send_and_wait_ack(NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_QOS,
                            IPC_TYPE_CAR, CAR_INFO_CARID, IPC_OPCODE_DELETE, car_id);
        #endif
	    if(ret)
	    {
	        return ret;
	    }
    }

    hios_hash_delete(&qos_car_profile, pbucket);

    XFREE(MTYPE_HASH_BACKET, pbucket);
    XFREE(MTYPE_QOS_ENTRY, pcar);

    return ERRNO_SUCCESS;
}


int qos_car_profile_get_bulk ( uint16_t index, struct car_t *car_array )
{
    int msg_num = IPC_MSG_LEN/sizeof(struct car_t);
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct car_t *pcar = NULL;
    int cursor;
    int val = 0;
    int data_num = 0;

    if ( index == 0 )
    {
    	HASH_BUCKET_LOOP(pbucket, cursor, qos_car_profile)
        {
            pcar = (struct car_t *)pbucket->data;
            if (NULL == pcar)
            {
                continue;
            }

            if(pcar->car_id > QOS_CAR_INDEX_OFFSET)
            {
            	continue;
            }

            memcpy(&car_array[data_num++], pcar, sizeof(struct car_t));
            if (data_num == msg_num)
            {
                return data_num;
            }
        }
    }
    else
    {
        pbucket = hios_hash_find(&qos_car_profile, (void *)(int)index);

        if (NULL == pbucket)
        {
        	val = (qos_car_profile.compute_hash((void *)(int)index)) % HASHTAB_SIZE;

	        if (qos_car_profile.buckets[val] != NULL)
	        {
	        	pbucket = qos_car_profile.buckets[val];
	        }
	        else
	        {
	        	for (++val; val<HASHTAB_SIZE; ++val)
	            {
	                if (NULL != qos_car_profile.buckets[val])
	                {
	                    pbucket = qos_car_profile.buckets[val];
	                    break;
	                }
	            }
	        }
        }

        while (pbucket)
        {
            pnext = hios_hash_next_cursor(&qos_car_profile, pbucket);
            if ((NULL == pnext) || (NULL == pnext->data))
            {
                break;
            }
            pbucket = pnext;

            pcar = (struct car_t *)(pnext->data);

            if(pcar->car_id > QOS_CAR_INDEX_OFFSET)
            {
            	continue;
            }

            memcpy(&car_array[data_num++], pcar, sizeof(struct car_t));
            if (data_num == msg_num)
            {
                return data_num;
            }
        }
    }

    return data_num;
}


int qos_car_profile_param_set(struct car_t *pold, struct car_t *pcar)
{
    int ret;

    if(pold->cir == pcar->cir &&
        pold->pir == pcar->pir &&
        pold->cbs == pcar->cbs &&
        pold->pbs == pcar->pbs)
    {
        return ERRNO_SUCCESS;
    }

    if(pcar->pir < pcar->cir)
        return QOS_ERR_ILL_PIR;
    if(pcar->pbs < pcar->cbs)
        return QOS_ERR_ILL_PBS;
#if 0
    ret = ipc_send_hal_wait_ack((void *)pcar, sizeof(struct car_t), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_CAR, CAR_INFO_CIR, IPC_OPCODE_UPDATE, pold->car_id);
#else
	ret = qos_ipc_send_and_wait_ack((void *)pcar, sizeof(struct car_t), 1, MODULE_ID_HAL, MODULE_ID_QOS,
                                IPC_TYPE_CAR, CAR_INFO_CIR, IPC_OPCODE_UPDATE, pold->car_id);
#endif
    if(ret)
    {
        return ret;
    }

    pold->cir = pcar->cir;
    pold->cbs = pcar->cbs;
    pold->pir = pcar->pir;
    pold->pbs = pcar->pbs;

    return ERRNO_SUCCESS;
}

int qos_car_profile_color_action_set(struct car_t *pold, struct car_action *paction, enum CAR_INFO type)
{
    int ret;
    struct car_action act;

    switch(type)
    {
        case CAR_INFO_GREEN_ACT:
        	memcpy(&act, &pold->green_act, sizeof(struct car_action));
            break;
        case CAR_INFO_YELLOW_ACT:
        	memcpy(&act, &pold->yellow_act, sizeof(struct car_action));
            break;
        case CAR_INFO_RED_ACT:
            memcpy(&act, &pold->red_act, sizeof(struct car_action));
            break;
        default:
            return ERRNO_FAIL;
    }

    if(act.color == paction->color
    && act.drop   == paction->drop
    && act.new_cos == paction->new_cos
    && act.new_dscp == paction->new_dscp
    && act.new_queue == paction->new_queue)
    {
        return ERRNO_SUCCESS;
    }
#if 0    
    ret = ipc_send_hal_wait_ack((void *)paction, sizeof(struct car_action), 1, MODULE_ID_HAL, MODULE_ID_QOS, 
            IPC_TYPE_CAR, type, IPC_OPCODE_UPDATE, pold->car_id);
#else
	ret = qos_ipc_send_and_wait_ack((void *)paction, sizeof(struct car_action), 1, MODULE_ID_HAL, MODULE_ID_QOS, 
            IPC_TYPE_CAR, type, IPC_OPCODE_UPDATE, pold->car_id);
#endif
    if(ret)
    {
        return ret;
    }

    switch(type)
    {
        case CAR_INFO_GREEN_ACT:
            memcpy(&pold->green_act, paction, sizeof(struct car_action));
            break;
        case CAR_INFO_YELLOW_ACT:
            memcpy(&pold->yellow_act, paction, sizeof(struct car_action));
            break;
        case CAR_INFO_RED_ACT:
            memcpy(&pold->red_act, paction, sizeof(struct car_action));
            break;
        default:
            return ERRNO_FAIL;
    }

    return ERRNO_SUCCESS;
}

int qos_car_profile_color_mode_set(struct car_t *pold, uint8_t mode)
{
    int ret;

    if(mode == pold->color_blind)
    {
        return ERRNO_SUCCESS;
    }
#if 0
    ret = ipc_send_hal_wait_ack((void *)&mode, sizeof(uint32_t), 1, MODULE_ID_HAL, MODULE_ID_QOS,
            IPC_TYPE_CAR, CAR_INFO_COLOR_BLIND, IPC_OPCODE_UPDATE, pold->car_id);
#else
	ret = qos_ipc_send_and_wait_ack((void *)&mode, sizeof(uint32_t), 1, MODULE_ID_HAL, MODULE_ID_QOS,
            IPC_TYPE_CAR, CAR_INFO_COLOR_BLIND, IPC_OPCODE_UPDATE, pold->car_id);
#endif
    if(ret)
    {
        return ret;
    }

    pold->color_blind = mode;
    return ERRNO_SUCCESS;
}

int qos_car_profile_set(struct car_t *pcar, void *pdata, enum CAR_INFO type)
{
    int ret;
    
    switch(type)
    {
        case CAR_INFO_INVALID:
            QOS_LOG_ERR("Invalid car info type!!\n");
            return ERRNO_FAIL;
        case CAR_INFO_CARID:
            break;
        case CAR_INFO_CIR:
        case CAR_INFO_CBS:
        case CAR_INFO_PIR:
        case CAR_INFO_PBS:
            ret = qos_car_profile_param_set(pcar, (struct car_t*)pdata);
            if(ret)
            {
                QOS_LOG_ERR("qos_car_profile_param_set failed!!\n");
                return ret;
            }
            break;
        case CAR_INFO_RED_ACT:
        case CAR_INFO_GREEN_ACT:
        case CAR_INFO_YELLOW_ACT:
            ret = qos_car_profile_color_action_set(pcar, (struct car_action *)pdata, type);
            if(ret)
            {
                QOS_LOG_ERR("qos_car_profile_color_action_set failed!!\n");
                return ret;
            }
            break;
        case CAR_INFO_COLOR_BLIND:
            ret = qos_car_profile_color_mode_set(pcar, *(uint8_t *)pdata);
            if(ret)
            {
                QOS_LOG_ERR("qos_car_profile_color_mode_set failed!!\n");
                return ret;
            }
            break;
        default:
            break;
    }

    return ERRNO_SUCCESS;
}


void qos_car_profile_attached (struct car_t *pcar, int flag)
{
    if(flag)
    {
        pcar->ref_cnt++;
    }
    else
    {
        if(pcar->ref_cnt > 0)
            pcar->ref_cnt--;
    }

    return;
}


static int qos_car_profile_show_operation(struct car_t *pcar, struct vty *vty)
{
    
    CAR_PROFILE_LINE_CHECK(car_profile_line, vty->flush_cp)
    {
    	vty_out(vty, "Car_id:%d, ref_cnt:%d %s", pcar->car_id, pcar->ref_cnt, VTY_NEWLINE);
    }
    
    CAR_PROFILE_LINE_CHECK(car_profile_line, vty->flush_cp)
    {
    	vty_out(vty, " cir %d cbs %d pir %d pbs %d%s", pcar->cir, 
    			pcar->cbs, pcar->pir, pcar->pbs, VTY_NEWLINE);
    }
    
    CAR_PROFILE_LINE_CHECK(car_profile_line, vty->flush_cp)
    {
    	vty_out(vty, " color-mode   : %s%s", (QOS_CAR_COLOR_BLIND == pcar->color_blind)
    	           ? "color-blind" : "color-aware", VTY_NEWLINE);
    }
    
    CAR_PROFILE_LINE_CHECK(car_profile_line, vty->flush_cp)
    {
    	vty_out(vty, " green action : %s ", (QOS_CAR_PACKETS_DROP == pcar->green_act.drop) ? "Discard" : "Pass");
    	
    	if (pcar->green_act.new_cos & CAR_ACTION_CONFIGED)
    	{
    		vty_out(vty, "new-cos %d ", pcar->green_act.new_cos & CAR_ACTION_COS_MASK);
    	}

    	if (pcar->green_act.new_dscp & CAR_ACTION_CONFIGED)
    	{
    		vty_out(vty, "new-dscp %d ", pcar->green_act.new_dscp & CAR_ACTION_DSCP_MASK);
    	}
    	
    	if (pcar->green_act.new_queue & CAR_ACTION_CONFIGED)
    	{
    		vty_out(vty, "queue %d ", pcar->green_act.new_queue & CAR_ACTION_QUEUE_MASK);
    	}
    	
    	vty_out(vty, "%s", VTY_NEWLINE);
    }
    
    CAR_PROFILE_LINE_CHECK(car_profile_line, vty->flush_cp)
    {
    	vty_out(vty, " yellow action: %s ", (QOS_CAR_PACKETS_DROP == pcar->yellow_act.drop) ? "Discard" : "Pass");
    	
    	if (pcar->yellow_act.new_cos & CAR_ACTION_CONFIGED)
    	{
    		vty_out(vty, "new-cos %d ", pcar->yellow_act.new_cos & CAR_ACTION_COS_MASK);
    	}
    	
    	if (pcar->yellow_act.new_dscp & CAR_ACTION_CONFIGED)
    	{
    		vty_out(vty, "new-dscp %d ", pcar->yellow_act.new_dscp & CAR_ACTION_DSCP_MASK);
    	}
    	
    	if (pcar->yellow_act.new_queue & CAR_ACTION_CONFIGED)
    	{
    		vty_out(vty, "queue %d ", pcar->yellow_act.new_queue & CAR_ACTION_QUEUE_MASK);
    	}
    	
    	vty_out(vty, "%s", VTY_NEWLINE);
    }
    
    CAR_PROFILE_LINE_CHECK(car_profile_line, vty->flush_cp)
    {
    	vty_out(vty, " red action   : %s ", (QOS_CAR_PACKETS_DROP == pcar->red_act.drop) ? "Discard" : "Pass");
    	
    	if (pcar->red_act.new_cos & CAR_ACTION_CONFIGED)
    	{
    		vty_out(vty, "new-cos %d ", pcar->red_act.new_cos & CAR_ACTION_COS_MASK);
    	}
    	
    	if (pcar->red_act.new_dscp & CAR_ACTION_CONFIGED)
    	{
    		vty_out(vty, "new-dscp %d ", pcar->red_act.new_dscp & CAR_ACTION_DSCP_MASK);
    	}
    	
    	if (pcar->red_act.new_queue & CAR_ACTION_CONFIGED)
    	{
    		vty_out(vty, "queue %d ", pcar->red_act.new_queue & CAR_ACTION_QUEUE_MASK);
    	}
    	
    	vty_out(vty, "%s", VTY_NEWLINE);
    }
    
    CAR_PROFILE_LINE_CHECK(car_profile_line, vty->flush_cp)
	{
		vty_out(vty, "%s", VTY_NEWLINE);
	}	
    
    return ERRNO_SUCCESS;
}

DEFUN(show_qos_car,
        show_qos_car_cmd,
        "show qos car-profile [<1-1000>]",
        SHOW_STR
        "Quality of Service\n"
        "QoS car-profile\n"
        "QoS car-profile ID\n")
{
    uint32_t car_id;
    struct car_t *pcar = NULL;
    struct hash_bucket *pbucket = NULL;
    int cursor;
    int ret = 0;

    if (vty->flush_cp == 0xFFFFFFFF)
    {
        return CMD_SUCCESS;
    }

    if(NULL != argv[0])
    {
        car_id = atoi( argv[0] );

        pcar = qos_car_profile_lookup(car_id);
        if (NULL != pcar)
        {
            ret = qos_car_profile_show_operation(pcar, vty);
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
        HASH_BUCKET_LOOP(pbucket, cursor, qos_car_profile)
        {
            pcar = pbucket->data;
            assert(NULL != pcar);

            if(pcar->car_id <= QOS_CAR_INDEX_OFFSET)
            {
                ret = qos_car_profile_show_operation(pcar, vty);
                if(ret == CMD_CONTINUE)
	            {
	            	return CMD_CONTINUE;
	            }
            }
        }
    }

    car_profile_line = 0;
    return CMD_SUCCESS;
}

DEFUN(qos_car,
        qos_car_cmd,
        "qos car-profile <1-1000>",
        "Quality of Service\n"
        "QoS car-profile\n"
        "QoS car-profile ID\n")
{
    struct car_t car;
    struct car_t *pcar = NULL;

    memset(&car, 0, sizeof(struct car_t));
    car.car_id = atoi(argv[0]);
	
	car.green_act.color  = QOS_COLOR_GREEN;
    car.green_act.drop   = QOS_CAR_PACKETS_PASS;
    car.yellow_act.color = QOS_COLOR_YELLOW;
    car.yellow_act.drop  = QOS_CAR_PACKETS_PASS;
    car.red_act.color    = QOS_COLOR_RED;
    car.red_act.drop     = QOS_CAR_PACKETS_DROP;
    car.color_blind = QOS_CAR_COLOR_AWARE;
    car.shared      = QOS_CAR_PROFILE_SHARED;

    pcar = qos_car_create(&car);
    if(NULL == pcar)
    {
        vty_error_out(vty, "QoS car profile created failed!!%s", VTY_NEWLINE);
		return CMD_WARNING;
    }

    vty->index = (void *)pcar;
    vty->node  = QOS_CAR_NODE;
    
    return CMD_SUCCESS;
}

DEFUN(no_qos_car,
        no_qos_car_cmd,
        "no qos car-profile <1-1000>",
        NO_STR
        "Quality of Service\n"
        "QoS car-profile\n"
        "QoS car-profile ID\n")
{
    int ret;
    uint32_t car_id;

    car_id = atoi(argv[0]);

    ret = qos_car_profile_delete(car_id);
    if(ret)
    {
        if(QOS_ERR_ATTACHED == ret)
        {
            vty_error_out ( vty, "QoS car profile %d was applied to interface, please detach first!!%s", car_id, VTY_NEWLINE);
        }
        else
        {
            vty_error_out ( vty, "Failed to execute command!! %s", VTY_NEWLINE);
        }
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}        

DEFUN(qos_car_rate_limit,
        qos_car_rate_limit_cmd,
        "cir <0-10000000> cbs <0-10000000> {pir <0-10000000> pbs <0-10000000>}",
        "Committed information rate\n"
        "Committed information rate value <0-10000000> (Unit: Kbps)\n"
        "Committed burst size\n"
        "Committed burst size <0-10000000> (Unit: Kb)\n"
        "Peak information rate\n"
        "Peak information rate value <0-10000000> (Unit: Kbps)\n"
        "Peak burst size\n"
        "Peak burst size <0-10000000> (Unit: Kb)\n")
{
    struct car_t car;
    struct car_t *pcar = NULL;
    int ret;

    pcar = (struct car_t *)vty->index;
    if (NULL == pcar)
    {
    	vty_error_out(vty, "Failed to set car parameters!!%s", VTY_NEWLINE);
    	return CMD_WARNING;
    }
    memset(&car, 0, sizeof(struct car_t));

    VTY_GET_INTEGER_RANGE("Committed information rate", car.cir, argv[0], QOS_CAR_CIR_MIN, QOS_CAR_CIR_MAX);
    VTY_GET_INTEGER_RANGE("Committed burst size", car.cbs, argv[1], QOS_CAR_CBS_MIN, QOS_CAR_CBS_MAX);

    if(NULL != argv[2])
    {
        VTY_GET_INTEGER_RANGE("Peak information rate", car.pir, argv[2], QOS_CAR_PIR_MIN, QOS_CAR_PIR_MAX);
        VTY_GET_INTEGER_RANGE("Peak burst size", car.pbs, argv[3], QOS_CAR_PBS_MIN, QOS_CAR_PBS_MAX);
    }
    else
    {
        car.pir = car.cir;

        if (car.pir > 100000)
        {
            car.pbs = 100000;
        }
        else
        {
            car.pbs = car.pir;
        }
    }
    
    ret = qos_car_profile_set(pcar, (void *)&car, CAR_INFO_CIR);
    if(ret)
    {
        if(QOS_ERR_ILL_PIR == ret)
        {
            vty_error_out(vty, "PIR must larger than CIR or equal to CIR.%s", VTY_NEWLINE);
        }
        else if(QOS_ERR_ILL_PBS == ret)
        {
            vty_error_out(vty, "PBS must larger than CBS or equal to CBS.%s", VTY_NEWLINE);
        }
        else
        {
            vty_error_out(vty, "Failed to set car parameters!!%s", VTY_NEWLINE);
        }
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(qos_car_color_action,
        qos_car_color_action_cmd,
        "color (green | yellow | red) (discard | pass) {remark-cos <0-7>| remark-dscp <0-63> | queue <0-7>}",
        "Packet color\n"
        "Green packets\n"
        "Red packets\n"
        "Yellow packets\n"
        "Discard packets\n"
        "Forward packets\n"
        "Remark pass packet cos\n"
        "cos value"
        "Remark pass packet dscp\n"
        "dscp value"
        "Remark packet queue\n"
        "queue value")
{
    struct car_action act;
    struct car_t *pcar = NULL;
    enum CAR_INFO type = CAR_INFO_INVALID;
    int ret;
	
	pcar = (struct car_t *)vty->index;
    if (NULL == pcar)
    {
    	vty_error_out(vty, "Failed to set car parameters!!%s", VTY_NEWLINE);
    	return CMD_WARNING;
    }
	
    /* set action to color */
    if('g' == argv[0][0])
    {
        type = CAR_INFO_GREEN_ACT;
        memcpy(&act, &pcar->green_act, sizeof(struct car_action));
        act.color = QOS_COLOR_GREEN;
    }
    else if('r' == argv[0][0])
    {
        type = CAR_INFO_RED_ACT;
        memcpy(&act, &pcar->red_act, sizeof(struct car_action));
        act.color = QOS_COLOR_RED;
    }
    else if('y' == argv[0][0])
    {
        type = CAR_INFO_YELLOW_ACT;
        memcpy(&act, &pcar->yellow_act, sizeof(struct car_action));
        act.color = QOS_COLOR_YELLOW;
    }    

    /* get packets action */
    if('d' == argv[1][0])
    {
        act.drop = QOS_CAR_PACKETS_DROP;
    }
    else if('p' == argv[1][0])
    {
        act.drop = QOS_CAR_PACKETS_PASS;
    }

	if (NULL != argv[2])
	{
		VTY_GET_INTEGER_RANGE("Vlan cos", act.new_cos, argv[2], 0, 7);
		act.new_cos |= CAR_ACTION_CONFIGED;
	}
	
	if (NULL != argv[3])
	{
		VTY_GET_INTEGER_RANGE("Ip dscp", act.new_dscp, argv[3], 0, 63);
		act.new_dscp |= CAR_ACTION_CONFIGED;
	}
	
	if (NULL != argv[4])
	{
		VTY_GET_INTEGER_RANGE("Queue id", act.new_queue, argv[4], 0, 7);
		act.new_queue |= CAR_ACTION_CONFIGED;
	}
	
    ret = qos_car_profile_set(pcar, (void *)&act, type);
    if(ret)
    {
        vty_error_out(vty, "Failed to set car profile color action!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(qos_car_color_mode,
        qos_car_color_mode_cmd,
        "color blind",
        "Configure qos car color mode\n"
        "color-blind mode\n")
{
	struct car_t *pcar = NULL;
    uint8_t color_mode;
    int ret;

    pcar = (struct car_t *)vty->index;
    if (NULL == pcar)
    {
    	vty_error_out(vty, "Failed to set car mode!!%s", VTY_NEWLINE);
    	return CMD_WARNING;
    }

    color_mode = QOS_CAR_COLOR_BLIND;
    ret = qos_car_profile_set(pcar, (void *)&color_mode, CAR_INFO_COLOR_BLIND);
    if(ret)
    {
        vty_error_out(vty, "Failed to set car profile color blind mode!!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(no_qos_car_color_action,
        no_qos_car_color_action_cmd,
        "no color (green | red | yellow| blind)",
        NO_STR
        "Packet color\n"
        "Green packets\n"
        "Red packets\n"
        "Yellow packets\n"
        "color-blind mode\n")
{
    struct car_t *pcar = NULL;
    int ret;
    uint8_t data;
    struct car_action action;
    enum CAR_INFO type = CAR_INFO_INVALID;

    pcar = (struct car_t *)vty->index;
    if (NULL == pcar)
    {
    	vty_error_out(vty, "Failed to set car parameters!!%s", VTY_NEWLINE);
    	return CMD_WARNING;
    }
    
    memset(&action, 0, sizeof(struct car_action));
	
    if('g' == argv[0][0])
    {
    	action.color = QOS_COLOR_GREEN;
    	action.drop  = QOS_CAR_PACKETS_PASS;
        type = CAR_INFO_GREEN_ACT;
    }
    else if('r' == argv[0][0])
    {
        action.color = QOS_COLOR_RED;
    	action.drop  = QOS_CAR_PACKETS_DROP;
        type = CAR_INFO_RED_ACT;
    }
    else if('y' == argv[0][0])
    {
        action.color = QOS_COLOR_YELLOW;
    	action.drop  = QOS_CAR_PACKETS_PASS;
        type = CAR_INFO_YELLOW_ACT;
    }
    else if('b' == argv[0][0])
    {
        data = QOS_CAR_COLOR_AWARE;
        type = CAR_INFO_COLOR_BLIND;
    }
	
	if ('b' == argv[0][0])
	{
		ret = qos_car_profile_set(pcar, (void *)&data, type);
	}
	else
	{
		ret = qos_car_profile_set(pcar, &action, type);
	}
    if(ret)
    {
        vty_error_out(vty, "Failed to restore default color %s!!%s", 
                    (CAR_INFO_COLOR_BLIND == type) ? "mode" : "action", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

static struct cmd_node qos_car_node =
{
    QOS_CAR_NODE,
    "%s(config-qos-car)# ",
    1
};

static int qos_car_config_write (struct vty *vty)
{
    struct car_t *pcar = NULL;
    struct hash_bucket *pbucket = NULL;
    unsigned int cursor = 0;
    int ret = 0;

    /* 保存CAR模板 */
    HASH_BUCKET_LOOP(pbucket, cursor, qos_car_profile)
    {
        pcar = pbucket->data;
        if(NULL != pcar && (pcar->car_id) <= QOS_CAR_INDEX_OFFSET)
        {
            vty_out(vty, "qos car-profile %d%s", pcar->car_id, VTY_NEWLINE);
            if (0 != pcar->cir)
            {
                vty_out(vty, " cir %d cbs %d pir %d pbs %d%s", pcar->cir, 
                           pcar->cbs, pcar->pir, pcar->pbs, VTY_NEWLINE);
            }    
            
            if (QOS_CAR_PACKETS_PASS != pcar->green_act.drop
            || (pcar->green_act.new_cos & CAR_ACTION_CONFIGED)
            || (pcar->green_act.new_dscp & CAR_ACTION_CONFIGED)
            || (pcar->green_act.new_queue & CAR_ACTION_CONFIGED))
            {
                vty_out(vty, " color green %s", (QOS_CAR_PACKETS_DROP == pcar->green_act.drop) ? "discard" : "pass");
                			
                if (pcar->green_act.new_cos & CAR_ACTION_CONFIGED)
                {
                	vty_out(vty, " remark-cos %d ", pcar->green_act.new_cos & CAR_ACTION_COS_MASK);
                }
                
                if (pcar->green_act.new_dscp & CAR_ACTION_CONFIGED)
                {
                	vty_out(vty, " remark-dscp %d ", pcar->green_act.new_dscp & CAR_ACTION_DSCP_MASK);
                }
                
                if (pcar->green_act.new_queue & CAR_ACTION_CONFIGED)
                {
                	vty_out(vty, " queue %d", pcar->green_act.new_queue & CAR_ACTION_QUEUE_MASK);
                }
                
                vty_out(vty, "%s", VTY_NEWLINE);
            }
            
            if (QOS_CAR_PACKETS_PASS != pcar->yellow_act.drop
            || (pcar->yellow_act.new_cos & CAR_ACTION_CONFIGED)
            || (pcar->yellow_act.new_dscp & CAR_ACTION_CONFIGED)
            || (pcar->yellow_act.new_queue & CAR_ACTION_CONFIGED))
            {
                vty_out(vty, " color yellow %s", (QOS_CAR_PACKETS_DROP == pcar->yellow_act.drop) ? "discard" : "pass");
                			
                if (pcar->yellow_act.new_cos & CAR_ACTION_CONFIGED)
                {
                	vty_out(vty, " remark-cos %d ", pcar->yellow_act.new_cos & CAR_ACTION_COS_MASK);
                }
                
                if (pcar->yellow_act.new_dscp & CAR_ACTION_CONFIGED)
                {
                	vty_out(vty, " remark-dscp %d ", pcar->yellow_act.new_dscp & CAR_ACTION_DSCP_MASK);
                }
                
                if (pcar->yellow_act.new_queue & CAR_ACTION_CONFIGED)
                {
                	vty_out(vty, " queue %d", pcar->yellow_act.new_queue & CAR_ACTION_QUEUE_MASK);
                }
                
                vty_out(vty, "%s", VTY_NEWLINE);
            }
            
            if(QOS_CAR_PACKETS_DROP != pcar->red_act.drop
            || (pcar->red_act.new_cos & CAR_ACTION_CONFIGED)
            || (pcar->red_act.new_dscp & CAR_ACTION_CONFIGED)
            || (pcar->red_act.new_queue & CAR_ACTION_CONFIGED))
            {
                vty_out(vty, " color red %s", (QOS_CAR_PACKETS_DROP == pcar->red_act.drop) ? "discard" : "pass");
                			
                if (pcar->red_act.new_cos & CAR_ACTION_CONFIGED)
                {
                	vty_out(vty, " remark-cos %d ", pcar->red_act.new_cos & CAR_ACTION_COS_MASK);
                }
                
                if (pcar->red_act.new_dscp & CAR_ACTION_CONFIGED)
                {
                	vty_out(vty, " remark-dscp %d ", pcar->red_act.new_dscp & CAR_ACTION_DSCP_MASK);
                }
                
                if (pcar->red_act.new_queue & CAR_ACTION_CONFIGED)
                {
                	vty_out(vty, " queue %d", pcar->red_act.new_queue & CAR_ACTION_QUEUE_MASK);
                }
                
                vty_out(vty, "%s", VTY_NEWLINE);
            }
            
            if(QOS_CAR_COLOR_AWARE != pcar->color_blind)
            {
                vty_out(vty, " color blind%s", VTY_NEWLINE);
            }
        }
    }

    return ret;
}

void qos_car_cmd_init(void)
{
    install_node(&qos_car_node, qos_car_config_write);
    install_default(QOS_CAR_NODE);

    install_element(CONFIG_NODE, &show_qos_car_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &qos_car_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &no_qos_car_cmd, CMD_SYNC);
    install_element(QOS_CAR_NODE, &qos_car_rate_limit_cmd, CMD_SYNC);
    install_element(QOS_CAR_NODE, &qos_car_color_action_cmd, CMD_SYNC);
    install_element(QOS_CAR_NODE, &qos_car_color_mode_cmd, CMD_SYNC);
    install_element(QOS_CAR_NODE, &no_qos_car_color_action_cmd, CMD_SYNC);
}

