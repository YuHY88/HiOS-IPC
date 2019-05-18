#include <lib/types.h>
#include <lib/ifm_common.h>
#include <lib/hash1.h>
#include <lib/msg_ipc.h>
#include <lib/msg_ipc_n.h>
#include <lib/errcode.h>
#include <lib/log.h>


#include "l2/l2_if.h"
#include "raps/erps_pkt.h"
#include "raps/erps.h"
#include "raps/erps_fsm_action.h"
#include "raps/erps_fsm.h"
#include "raps/erps_timer.h"
#include "l2_msg.h"
//node id higher
int erps_node_id_higher(u_int8_t aps_node[], u_int8_t session_node[])
{
    u_int8_t i = 0;

    for(i = 0; i < MAC_LEN; i++)
    {
        if(aps_node[i] > session_node[i])
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
            return 1;
        }
        else if(aps_node[i] < session_node[i])
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
            return 2;
        }
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
    return 0;
}

//init action
int erps_init_handle_initialization(struct erps_sess *psess)
{
    int ret = ERRNO_FAIL;
    uint8_t subtype = 0;

    if(NULL == psess)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d sess id %d", __FILE__, __func__, __LINE__, psess->info.sess_id);
    erps_stop_guard_timer(psess);
    erps_stop_wtr_timer(psess);
    erps_stop_wtb_timer(psess);

    ERPS_LOG_DBG("%s:'%s'--the line of %d psess->info.role %d ", __FILE__, __func__, __LINE__, psess->info.role);

    if(ERPS_ROLE_OWNER == psess->info.role)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.rpl_interface %d psess->info.east_interface %d psess->info.west_interface %d",
                   __FILE__, __func__, __LINE__, psess->info.rpl_interface, psess->info.east_interface, psess->info.west_interface);

        //block RPL port
        if(psess->info.rpl_interface == psess->info.east_interface)
        {
            subtype = OAM_SUBTYPE_ERPS_BLOCK_EAST;
        }
        else if(psess->info.rpl_interface == psess->info.west_interface)
        {
            subtype = OAM_SUBTYPE_ERPS_BLOCK_WEST;
        }

        ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                           IPC_TYPE_ERPS, subtype, IPC_OPCODE_UPDATE, psess->info.sess_id);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return ERRNO_IPC;
        }

        psess->info.block_interface = psess->info.rpl_interface;
        //Unblock non-RPL port
        //Tx R-APS(NR)
        psess->r_aps.rb = 0;
        psess->r_aps.dnf = 0;
        psess->r_aps.request_state = ERPS_NO_REQUEST;
        ret = raps_send_burst(psess);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return ERRNO_FAIL;
        }

        if(FAILBACK_ENABLE == psess->info.failback)
        {
            erps_start_wtr_timer(psess);
        }
    }
    else if(ERPS_ROLE_NEIGHBOUR == psess->info.role)
    {

        ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.rpl_interface %d psess->info.east_interface %d psess->info.west_interface %d",
                   __FILE__, __func__, __LINE__, psess->info.rpl_interface, psess->info.east_interface, psess->info.west_interface);

        //block RPL port
        if(psess->info.rpl_interface == psess->info.east_interface)
        {
            subtype = OAM_SUBTYPE_ERPS_BLOCK_EAST;
        }
        else if(psess->info.rpl_interface == psess->info.west_interface)
        {
            subtype = OAM_SUBTYPE_ERPS_BLOCK_WEST;
        }

        psess->info.block_interface = psess->info.rpl_interface;
        ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                           IPC_TYPE_ERPS, subtype, IPC_OPCODE_UPDATE, psess->info.sess_id);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return ERRNO_IPC;
        }

        //Unblock non-RPL port
        //Tx R-APS(NR)
        psess->r_aps.rb = 0;
        psess->r_aps.dnf = 0;
        psess->r_aps.request_state = ERPS_NO_REQUEST;
        ret = raps_send_burst(psess);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return ERRNO_FAIL;
        }
    }
    else
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.rpl_interface %d psess->info.east_interface %d psess->info.west_interface %d",
                   __FILE__, __func__, __LINE__, psess->info.rpl_interface, psess->info.east_interface, psess->info.west_interface);

        //block RPL port
        if(psess->info.west_interface)
        {
            ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                               IPC_TYPE_ERPS, OAM_SUBTYPE_ERPS_BLOCK_WEST, IPC_OPCODE_UPDATE, psess->info.sess_id);

            psess->info.block_interface = psess->info.west_interface;
        }
        else
        {
            ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                               IPC_TYPE_ERPS, OAM_SUBTYPE_ERPS_BLOCK_EAST, IPC_OPCODE_UPDATE, psess->info.sess_id);

            psess->info.block_interface = psess->info.east_interface;
        }

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return ERRNO_IPC;
        }

        //Unblock non-RPL port
        //Tx R-APS(NR)
        psess->r_aps.rb = 0;
        psess->r_aps.dnf = 0;
        psess->r_aps.request_state = ERPS_NO_REQUEST;
        ret = raps_send_burst(psess);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return ERRNO_FAIL;
        }
    }

    psess->info.current_status = ERPS_STATE_PENDING;
    return ret;
}



//idle action
int erps_idlems_handle_raps_sf(struct erps_sess *psess)
{
    int ret = ERRNO_FAIL;
    uint8_t subtype = 0;

    if(NULL == psess)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d sess id %d", __FILE__, __func__, __LINE__, psess->info.sess_id);
    //unblock non-failed ring port

    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.block_interface %d psess->info.east_interface %d psess->info.west_interface %d",
               __FILE__, __func__, __LINE__, psess->info.block_interface, psess->info.east_interface, psess->info.west_interface);

    if((psess->info.east_interface) && (psess->info.block_interface == psess->info.east_interface))
    {
        subtype = OAM_SUBTYPE_ERPS_FOWARD_EAST;
    }
    else if((psess->info.west_interface) && (psess->info.block_interface == psess->info.west_interface))
    {
        subtype = OAM_SUBTYPE_ERPS_FOWARD_WEST;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d subtype %d", __FILE__, __func__, __LINE__, subtype);

    if(subtype)
    {
        psess->info.block_interface = 0;
        ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                           IPC_TYPE_ERPS, subtype, IPC_OPCODE_UPDATE, psess->info.sess_id);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return ERRNO_IPC;
        }
    }

    //Stop Tx R-APS
    raps_stop(psess);

    psess->info.current_status = ERPS_STATE_PROTECTION_REMOTE;
    return ERRNO_SUCCESS;
}

int erps_idle_handle_raps_ms(struct erps_sess *psess)
{
    int ret = ERRNO_FAIL;
    uint8_t subtype = 0;

    if(NULL == psess)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d sess id %d", __FILE__, __func__, __LINE__, psess->info.sess_id);
    //unblock non-failed ring port

    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.block_interface %d psess->info.east_interface %d psess->info.west_interface %d",
               __FILE__, __func__, __LINE__, psess->info.block_interface, psess->info.east_interface, psess->info.west_interface);

    if((psess->info.east_interface) && (psess->info.block_interface == psess->info.east_interface))
    {
        subtype = OAM_SUBTYPE_ERPS_FOWARD_EAST;
    }
    else if((psess->info.west_interface) && (psess->info.block_interface == psess->info.west_interface))
    {
        subtype = OAM_SUBTYPE_ERPS_FOWARD_WEST;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d subtype %d", __FILE__, __func__, __LINE__, subtype);

    if(subtype)
    {
        psess->info.block_interface = 0;
        ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                           IPC_TYPE_ERPS, subtype, IPC_OPCODE_UPDATE, psess->info.sess_id);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return ERRNO_IPC;
        }
    }

    //Stop Tx R-APS
    raps_stop(psess);

    psess->info.current_status = ERPS_STATE_MS;
    return ERRNO_SUCCESS;
}

int erps_idle_handle_admin_ms(struct erps_sess *psess)
{
    int ret = ERRNO_FAIL;
    uint8_t subtype_block = 0 , subtype_foward = 0;

    if(NULL == psess)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d sess id %d", __FILE__, __func__, __LINE__, psess->info.sess_id);


    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.event_block %d psess->info.block_interface %d psess->info.east_interface %d psess->info.west_interface %d",
               __FILE__, __func__, __LINE__, psess->info.event_block, psess->info.block_interface, psess->info.east_interface, psess->info.west_interface);

    if((psess->info.event_block) && (psess->info.event_block == psess->info.block_interface))
    {
        //Tx R-APS(MS,DNF)
        psess->r_aps.request_state = ERPS_MANUAL_SWITCH;
        psess->r_aps.rb = 0;
        psess->r_aps.dnf = 1;
        ret = raps_send_burst(psess);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return ERRNO_FAIL;
        }
    }
    else
    {
        if((psess->info.east_interface) && (psess->info.event_block == psess->info.east_interface))
        {
            subtype_block = OAM_SUBTYPE_ERPS_BLOCK_EAST;

            if(psess->info.west_interface)
            {
                subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_WEST;
            }
        }
        else if((psess->info.west_interface) && (psess->info.event_block == psess->info.west_interface))
        {
            subtype_block = OAM_SUBTYPE_ERPS_BLOCK_WEST;

            if(psess->info.east_interface)
            {
                subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_EAST;
            }
        }


        ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_block %d", __FILE__, __func__, __LINE__, subtype_block);

        if(subtype_block)
        {
            ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                               IPC_TYPE_ERPS, subtype_block, IPC_OPCODE_UPDATE, psess->info.sess_id);

            if(ret != ERRNO_SUCCESS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                return ERRNO_IPC;
            }
        }

        psess->info.block_interface = psess->info.event_block;
        //Tx R-APS(MS,DNF)
        psess->r_aps.request_state = ERPS_MANUAL_SWITCH;
        psess->r_aps.rb = 0;
        psess->r_aps.dnf = 0;
        ret = raps_send_burst(psess);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
            return ERRNO_FAIL;
        }

        ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_foward %d", __FILE__, __func__, __LINE__, subtype_foward);

        if(subtype_foward)
        {
            ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                               IPC_TYPE_ERPS, subtype_foward, IPC_OPCODE_UPDATE, psess->info.sess_id);

            if(ret != ERRNO_SUCCESS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                return ERRNO_IPC;
            }
        }
    }


    psess->info.current_status = ERPS_STATE_MS;
    return ERRNO_SUCCESS;
}

int erps_idle_handle_raps_nrrb(struct erps_sess *psess)
{
    int ret = ERRNO_FAIL;
    uint8_t subtype = 0;

    if(NULL == psess)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d sess id %d", __FILE__, __func__, __LINE__, psess->info.sess_id);
    //unblock non-RPL port

    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.rpl_interface %d psess->info.east_interface %d psess->info.west_interface %d",
               __FILE__, __func__, __LINE__, psess->info.rpl_interface, psess->info.east_interface, psess->info.west_interface);

    if(psess->info.block_interface == psess->info.rpl_interface)
    {
        return ERRNO_EXISTED;
    }

    if((psess->info.west_interface) && (psess->info.rpl_interface == psess->info.east_interface))
    {
        subtype = OAM_SUBTYPE_ERPS_FOWARD_WEST;
    }
    else if((psess->info.east_interface) && (psess->info.rpl_interface == psess->info.west_interface))
    {
        subtype = OAM_SUBTYPE_ERPS_FOWARD_EAST;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d subtype %d", __FILE__, __func__, __LINE__, subtype);

    if(subtype)
    {
        psess->info.block_interface = psess->info.rpl_interface;
        ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                           IPC_TYPE_ERPS, subtype, IPC_OPCODE_UPDATE, psess->info.sess_id);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return ERRNO_IPC;
        }
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.role %d", __FILE__, __func__, __LINE__, psess->info.role);

    if(psess->info.role != ERPS_ROLE_OWNER)
    {
        //Stop Tx R-APS
        raps_stop(psess);
    }

    psess->info.current_status = ERPS_STATE_IDLE;
    return ERRNO_SUCCESS;
}

int erps_idle_handle_raps_nr(struct erps_sess *psess, struct raps_pkt *raps_pdu)
{
    int ret = ERRNO_FAIL;
    uint8_t subtype = 0;

    if((NULL == psess) || (NULL == raps_pdu))
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.role %d", __FILE__, __func__, __LINE__, psess->info.role);

    if(ERPS_ROLE_NONOWNER == psess->info.role)
    {
        ret = erps_node_id_higher(raps_pdu->node_id, psess->info.node_id);

        if(ret == 1)
        {
            //unblock non-failed ring port
            if((psess->info.east_interface_flag == 0) && (psess->info.west_interface_flag == 0))
            {
                if((psess->info.east_interface) && (psess->info.block_interface == psess->info.east_interface))
                {
                    subtype = OAM_SUBTYPE_ERPS_FOWARD_EAST;
                }
                else if((psess->info.west_interface) && (psess->info.block_interface == psess->info.west_interface))
                {
                    subtype = OAM_SUBTYPE_ERPS_FOWARD_WEST;
                }

                ERPS_LOG_DBG("%s: '%s'--the line of %d subtype %d", __FILE__, __func__, __LINE__, subtype);

                if(subtype)
                {
                    psess->info.block_interface = 0;
                    ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                       IPC_TYPE_ERPS, subtype, IPC_OPCODE_UPDATE, psess->info.sess_id);

                    if(ret != ERRNO_SUCCESS)
                    {
                        ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                        return ERRNO_IPC;
                    }
                }
            }

            //Stop Tx R-APS
            raps_stop(psess);
        }
    }

    if(0 == subtype)
    {
        return ERRNO_EXISTED;
    }
    else
    {
        return ERRNO_SUCCESS;
    }
}



//protection action
int erps_protection_handle_clear_sf(struct erps_sess *psess)
{
    int ret = ERRNO_FAIL;

    if(NULL == psess)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    erps_start_guard_timer(psess);
    ERPS_LOG_DBG("%s: '%s'--the line of %d sess id %d", __FILE__, __func__, __LINE__, psess->info.sess_id);
    //Tx R-APS(NR)
    psess->r_aps.dnf = 0;
    psess->r_aps.rb = 0;
    psess->r_aps.request_state = ERPS_NO_REQUEST;
    ret = raps_send_burst(psess);

    if(ret != ERRNO_SUCCESS)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.role %d psess->info.failback %d", __FILE__, __func__, __LINE__, psess->info.role, psess->info.failback);

    if((ERPS_ROLE_OWNER == psess->info.role) && (FAILBACK_ENABLE == psess->info.failback))
    {
        erps_start_wtr_timer(psess);
    }

    psess->info.current_status = ERPS_STATE_PENDING;
    return ERRNO_SUCCESS;
}



//Manual switch action
int erps_ms_handle_raps_ms(struct erps_sess *psess)
{
    int ret = ERRNO_FAIL;

    if(NULL == psess)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.block_interface %d", __FILE__, __func__, __LINE__, psess->info.block_interface);

    if(psess->info.block_interface)
    {
        erps_start_guard_timer(psess);
        //Tx R-APS(NR)
        psess->r_aps.dnf = 0;
        psess->r_aps.rb = 0;
        psess->r_aps.request_state = ERPS_NO_REQUEST;
        ret = raps_send_burst(psess);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return ERRNO_FAIL;
        }

        ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.role %d psess->info.failback %d", __FILE__, __func__, __LINE__, psess->info.role, psess->info.failback);

        if((psess->info.role == ERPS_ROLE_OWNER) && (psess->info.failback == FAILBACK_ENABLE))
        {
            erps_start_wtb_timer(psess);
        }

        psess->info.current_status = ERPS_STATE_PENDING;
    }
    else
    {
        psess->info.current_status = ERPS_STATE_MS;
        return ERRNO_EXISTED;
    }

    return ERRNO_SUCCESS;
}



//Force switch action
int erps_fsms_handle_admin_clear(struct erps_sess *psess)
{
    int ret = ERRNO_FAIL;

    if(NULL == psess)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.block_interface %d", __FILE__, __func__, __LINE__, psess->info.block_interface);

    if(psess->info.block_interface)
    {
        erps_start_guard_timer(psess);
        //Tx R-APS(NR)
        psess->r_aps.dnf = 0;
        psess->r_aps.rb = 0;
        psess->r_aps.request_state = ERPS_NO_REQUEST;
        ret = raps_send_burst(psess);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return ERRNO_FAIL;
        }

        ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.role %d psess->info.failback %d", __FILE__, __func__, __LINE__, psess->info.role, psess->info.failback);

        if((ERPS_ROLE_OWNER == psess->info.role) && (FAILBACK_ENABLE == psess->info.failback))
        {
            erps_start_wtb_timer(psess);
        }
    }

    psess->info.current_status = ERPS_STATE_PENDING;
    return ERRNO_SUCCESS;
}

int erps_fs_handle_admin_fs(struct erps_sess *psess)
{
    int ret = ERRNO_FAIL;
    uint8_t subtype_block = 0 , subtype_foward = 0;

    if(NULL == psess)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.event_block %d psess->info.east_interface %d psess->info.west_interface %d",
               __FILE__, __func__, __LINE__, psess->info.event_block, psess->info.east_interface, psess->info.west_interface);

    //block requested ring port
    if((psess->info.east_interface) && (psess->info.event_block == psess->info.east_interface))
    {
        subtype_block = OAM_SUBTYPE_ERPS_BLOCK_EAST;

        if(psess->info.west_interface)
        {
            subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_WEST;
        }
    }
    else if((psess->info.west_interface) && (psess->info.event_block == psess->info.west_interface))
    {
        subtype_block = OAM_SUBTYPE_ERPS_BLOCK_WEST;

        if(psess->info.east_interface)
        {
            subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_EAST;
        }
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_block %d", __FILE__, __func__, __LINE__, subtype_block);

    if(subtype_block)
    {
        ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                           IPC_TYPE_ERPS, subtype_block, IPC_OPCODE_UPDATE, psess->info.sess_id);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return ERRNO_IPC;
        }
    }

    psess->info.block_interface = psess->info.event_block;

    //tx r-aps(fs)
    psess->r_aps.rb = 0;
    psess->r_aps.dnf = 0;
    psess->r_aps.request_state = ERPS_FORCED_SWITCH;
    ret = raps_send_burst(psess);

    if(ret != ERRNO_SUCCESS)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_foward %d", __FILE__, __func__, __LINE__, subtype_foward);

    if(subtype_foward)
    {
        ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                           IPC_TYPE_ERPS, subtype_foward, IPC_OPCODE_UPDATE, psess->info.sess_id);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return ERRNO_IPC;
        }
    }

    psess->info.current_status = ERPS_STATE_FS;
    return ERRNO_SUCCESS;
}


//pending action
int erps_pending_handle_admin_clear(struct erps_sess *psess)
{
    int ret = ERRNO_FAIL ;
    uint8_t subtype_block = 0, subtype_foward = 0;

    if(NULL == psess)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.role %d", __FILE__, __func__, __LINE__, psess->info.role);

    if(ERPS_ROLE_OWNER == psess->info.role)
    {
        erps_stop_wtr_timer(psess);
        erps_stop_wtb_timer(psess);

        ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.rpl_interface %d psess->info.block_interface %d psess->info.east_interface %d psess->info.west_interface %d",
                   __FILE__, __func__, __LINE__, psess->info.rpl_interface, psess->info.block_interface, psess->info.east_interface, psess->info.west_interface);

        if((psess->info.block_interface) && (psess->info.block_interface == psess->info.rpl_interface))
        {
            psess->r_aps.rb = 1;
            psess->r_aps.dnf = 1;
            psess->r_aps.request_state = ERPS_NO_REQUEST;

            if((psess->info.west_interface) && (psess->info.rpl_interface == psess->info.east_interface))
            {
                subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_WEST;
            }
            else if((psess->info.east_interface) && (psess->info.rpl_interface == psess->info.west_interface))
            {
                subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_EAST;
            }

            ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_foward %d", __FILE__, __func__, __LINE__, subtype_foward);

            if(subtype_foward)
            {
            	psess->info.sendMsgRapsNRRB = 1;
                ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ERPS, subtype_foward, IPC_OPCODE_UPDATE, psess->info.sess_id);

                if(ret != ERRNO_SUCCESS)
                {
                    ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                }
				psess->info.sendMsgRapsNRRB = 0;
            }

			//Tx R-APS(NR,RB,DNF)
			ret = raps_send_burst(psess);
            if(ret != ERRNO_SUCCESS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                return ERRNO_FAIL;
            }

        }
        else
        {
            if((psess->info.east_interface) && (psess->info.rpl_interface == psess->info.east_interface))
            {
                subtype_block = OAM_SUBTYPE_ERPS_BLOCK_EAST;

                if(psess->info.west_interface)
                {
                    subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_WEST;
                }
            }
            else if((psess->info.west_interface) && (psess->info.rpl_interface == psess->info.west_interface))
            {
                subtype_block = OAM_SUBTYPE_ERPS_BLOCK_WEST;

                if(psess->info.east_interface)
                {
                    subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_EAST;
                }
            }

            ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_block %d", __FILE__, __func__, __LINE__, subtype_block);

            if(subtype_block)
            {
            	psess->info.sendMsgRapsNRRB = 1;
                ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ERPS, subtype_block, IPC_OPCODE_UPDATE, psess->info.sess_id);

                if(ret != ERRNO_SUCCESS)
                {
                    ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                }
				psess->info.sendMsgRapsNRRB = 0;
            }

            psess->info.block_interface = psess->info.rpl_interface;
            //tx R-APS(NR,RB)
            psess->r_aps.rb = 1;
            psess->r_aps.dnf = 0;
            psess->r_aps.request_state = ERPS_NO_REQUEST;

            ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_foward %d", __FILE__, __func__, __LINE__, subtype_foward);

            if(subtype_foward)
            {
            	psess->info.sendMsgRapsNRRB = 1;
				ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                               IPC_TYPE_ERPS, subtype_foward, IPC_OPCODE_UPDATE, psess->info.sess_id);

                if(ret != ERRNO_SUCCESS)
                {
                    ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                }
				psess->info.sendMsgRapsNRRB = 0;
            }

			ret = raps_send_burst(psess);
            if(ret != ERRNO_SUCCESS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                return ERRNO_FAIL;
            }

        }

    }

    psess->info.current_status = ERPS_STATE_IDLE;
    return ERRNO_SUCCESS;
}

int erps_pending_handle_admin_fs(struct erps_sess *psess)
{
    int ret = ERRNO_FAIL;
    uint8_t subtype_block = 0, subtype_foward = 0;

    if(NULL == psess)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.event_block %d psess->info.block_interface %d psess->info.east_interface %d psess->info.west_interface %d",
               __FILE__, __func__, __LINE__, psess->info.event_block, psess->info.block_interface, psess->info.east_interface, psess->info.west_interface);

    if((psess->info.event_block) && (psess->info.event_block == psess->info.block_interface))
    {
        //tx R-APS(FS,DNF)
        psess->r_aps.rb = 0;
        psess->r_aps.dnf = 1;
        psess->r_aps.request_state = ERPS_FORCED_SWITCH;
        ret = raps_send_burst(psess);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return ERRNO_FAIL;
        }

        if((psess->info.east_interface) && (psess->info.event_block == psess->info.west_interface))
        {
            subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_EAST;
        }
        else if((psess->info.west_interface) && (psess->info.event_block == psess->info.east_interface))
        {
            subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_WEST;
        }

        ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_foward %d", __FILE__, __func__, __LINE__, subtype_foward);

        if(subtype_foward)
        {
            ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                               IPC_TYPE_ERPS, subtype_foward, IPC_OPCODE_UPDATE, psess->info.sess_id);

            if(ret != ERRNO_SUCCESS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                return ERRNO_IPC;
            }
        }
    }
    else
    {
        if((psess->info.east_interface) && (psess->info.event_block == psess->info.east_interface))
        {
            subtype_block = OAM_SUBTYPE_ERPS_BLOCK_EAST;
            psess->info.block_interface = psess->info.east_interface;

            if(psess->info.west_interface)
            {
                subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_WEST;
            }
        }
        else if((psess->info.west_interface) && (psess->info.event_block == psess->info.west_interface))
        {
            subtype_block = OAM_SUBTYPE_ERPS_BLOCK_WEST;
            psess->info.block_interface = psess->info.west_interface;

            if(psess->info.east_interface)
            {
                subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_EAST;
            }
        }

        ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_block %d", __FILE__, __func__, __LINE__, subtype_block);

        if(subtype_block)
        {
            ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                               IPC_TYPE_ERPS, subtype_block, IPC_OPCODE_UPDATE, psess->info.sess_id);

            if(ret != ERRNO_SUCCESS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                return ERRNO_IPC;
            }
        }

        //Tx R-APS(FS)
        psess->r_aps.rb = 0;
        psess->r_aps.dnf = 0;
        psess->r_aps.request_state = ERPS_FORCED_SWITCH;
        ret = raps_send_burst(psess);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return ERRNO_FAIL;
        }

        ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_foward %d", __FILE__, __func__, __LINE__, subtype_foward);

        if(subtype_foward)
        {
            ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                               IPC_TYPE_ERPS, subtype_foward, IPC_OPCODE_UPDATE, psess->info.sess_id);

            if(ret != ERRNO_SUCCESS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                return ERRNO_IPC;
            }
        }
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.role %d", __FILE__, __func__, __LINE__, psess->info.role);

    if(ERPS_ROLE_OWNER == psess->info.role)
    {
        erps_stop_wtr_timer(psess);
        erps_stop_wtb_timer(psess);
    }

    psess->info.current_status = ERPS_STATE_FS;
    return ERRNO_SUCCESS;

}

int erps_pending_handle_raps_fs(struct erps_sess *psess)
{
    int ret = ERRNO_FAIL;
    uint8_t subtype = 0;

    if(NULL == psess)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.block_interface %d psess->info.east_interface %d psess->info.west_interface %d",
               __FILE__, __func__, __LINE__ , psess->info.block_interface, psess->info.east_interface, psess->info.west_interface);

    //unblock ring port
    if((psess->info.east_interface) && (psess->info.block_interface == psess->info.east_interface))
    {
        subtype = OAM_SUBTYPE_ERPS_FOWARD_EAST;
    }
    else if((psess->info.west_interface) && (psess->info.block_interface == psess->info.west_interface))
    {
        subtype = OAM_SUBTYPE_ERPS_FOWARD_WEST;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d subtype %d", __FILE__, __func__, __LINE__, subtype);

    if(subtype)
    {
        psess->info.block_interface = 0;
        ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                           IPC_TYPE_ERPS, subtype, IPC_OPCODE_UPDATE, psess->info.sess_id);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return ERRNO_IPC;
        }
    }

    //stop tx r-aps
    raps_stop(psess);
    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.role %d", __FILE__, __func__, __LINE__, psess->info.role);

    if(ERPS_ROLE_OWNER == psess->info.role)
    {
        erps_stop_wtr_timer(psess);
        erps_stop_wtb_timer(psess);
    }

    psess->info.current_status = ERPS_STATE_FS;
    return ERRNO_SUCCESS;
}

int erps_pending_handle_local_sf(struct erps_sess *psess)
{
    int ret = ERRNO_FAIL;
    uint8_t subtype_block = 0, subtype_foward = 0;

    if(NULL == psess)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.event_block %d psess->info.block_interface %d psess->info.east_interface %d psess->info.west_interface %d",
               __FILE__, __func__, __LINE__, psess->info.event_block, psess->info.block_interface, psess->info.east_interface, psess->info.west_interface);

    if((psess->info.event_block) && (psess->info.event_block == psess->info.block_interface))
    {
        //tx r-aps(sf,dnf)
        psess->r_aps.rb = 0;
        psess->r_aps.dnf = 1;
        psess->r_aps.request_state = ERPS_SIGNAL_FAIL;
        ret = raps_send_burst(psess);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return ERRNO_FAIL;
        }

        //unblock non-failed ring port
        if((psess->info.west_interface) && (psess->info.block_interface == psess->info.east_interface))
        {
            subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_WEST;
        }
        else if((psess->info.east_interface) && (psess->info.block_interface == psess->info.west_interface))
        {
            subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_EAST;
        }

        ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_foward %d", __FILE__, __func__, __LINE__, subtype_foward);

        if(subtype_foward)
        {
            psess->info.block_interface = psess->info.event_block;
            ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                               IPC_TYPE_ERPS, subtype_foward, IPC_OPCODE_UPDATE, psess->info.sess_id);

            if(ret != ERRNO_SUCCESS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                return ERRNO_IPC;
            }
        }
    }
    else
    {
        //block falied ring port
        if((psess->info.east_interface) && (psess->info.event_block == psess->info.east_interface))
        {
            subtype_block = OAM_SUBTYPE_ERPS_BLOCK_EAST;

            if(psess->info.west_interface)
            {
                subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_WEST;
            }
        }
        else if((psess->info.west_interface) && (psess->info.event_block == psess->info.west_interface))
        {
            subtype_block = OAM_SUBTYPE_ERPS_BLOCK_WEST;

            if(psess->info.east_interface)
            {
                subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_EAST;
            }
        }

        ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_block %d", __FILE__, __func__, __LINE__, subtype_block);

        if(subtype_block)
        {
            ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                               IPC_TYPE_ERPS, subtype_block, IPC_OPCODE_UPDATE, psess->info.sess_id);

            if(ret != ERRNO_SUCCESS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                return ERRNO_IPC;
            }
        }

        psess->info.block_interface = psess->info.event_block;

        //tx r-aps(sf)
        psess->r_aps.rb = 0;
        psess->r_aps.dnf = 0;
        psess->r_aps.request_state = ERPS_SIGNAL_FAIL;
        ret = raps_send_burst(psess);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
            return ERRNO_FAIL;
        }

        //unblock non-failed ring port
        ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_foward %d", __FILE__, __func__, __LINE__, subtype_foward);

        if(subtype_foward)
        {
            ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                               IPC_TYPE_ERPS, subtype_foward, IPC_OPCODE_UPDATE, psess->info.sess_id);

            if(ret != ERRNO_SUCCESS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                return ERRNO_IPC;
            }
        }
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.role %d", __FILE__, __func__, __LINE__, psess->info.role);

    if(ERPS_ROLE_OWNER == psess->info.role)
    {
        erps_stop_wtr_timer(psess);
        erps_stop_wtb_timer(psess);
    }

    psess->info.current_status = ERPS_STATE_PROTECTION;
    return ERRNO_SUCCESS;
}

int erps_pending_handle_raps_sf(struct erps_sess *psess)
{
    int ret = ERRNO_FAIL;
    uint8_t subtype = 0;

    if(NULL == psess)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d  psess->info.block_interface %d psess->info.east_interface %d psess->info.west_interface %d",
               __FILE__, __func__, __LINE__ , psess->info.block_interface, psess->info.east_interface, psess->info.west_interface);

    //unblock non-failed ring port
    if((psess->info.east_interface) && (psess->info.block_interface == psess->info.east_interface))
    {
        subtype = OAM_SUBTYPE_ERPS_FOWARD_EAST;
    }
    else if((psess->info.west_interface) && (psess->info.block_interface == psess->info.west_interface))
    {
        subtype = OAM_SUBTYPE_ERPS_FOWARD_WEST;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d subtype %d", __FILE__, __func__, __LINE__, subtype);

    if(subtype)
    {
        psess->info.block_interface = 0;
        ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                           IPC_TYPE_ERPS, subtype, IPC_OPCODE_UPDATE, psess->info.sess_id);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return ERRNO_IPC;
        }
    }

    //stop tx r-aps
    raps_stop(psess);
    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.role %d", __FILE__, __func__, __LINE__, psess->info.role);

    if(ERPS_ROLE_OWNER == psess->info.role)
    {
        erps_stop_wtr_timer(psess);
        erps_stop_wtb_timer(psess);
    }

    psess->info.current_status = ERPS_STATE_PROTECTION_REMOTE;
    return ERRNO_SUCCESS;
}

int erps_pending_handle_raps_ms(struct erps_sess *psess)
{
    int ret = ERRNO_FAIL;
    uint8_t subtype = 0;

    if(NULL == psess)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d  psess->info.block_interface %d psess->info.east_interface %d psess->info.west_interface %d",
               __FILE__, __func__, __LINE__ , psess->info.block_interface, psess->info.east_interface, psess->info.west_interface);


    //unblock non-failed ring port
    if((psess->info.east_interface) && (psess->info.block_interface == psess->info.east_interface))
    {
        subtype = OAM_SUBTYPE_ERPS_FOWARD_EAST;
    }
    else if((psess->info.west_interface) && (psess->info.block_interface == psess->info.west_interface))
    {
        subtype = OAM_SUBTYPE_ERPS_FOWARD_WEST;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d subtype %d", __FILE__, __func__, __LINE__, subtype);

    if(subtype)
    {
        psess->info.block_interface = 0;
        ret = ipc_send_reply_n2(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                           IPC_TYPE_ERPS, subtype, IPC_OPCODE_UPDATE, psess->info.sess_id, IPC_OPCODE_REPLY);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return ERRNO_IPC;
        }
    }

    //stop tx r-aps
    raps_stop(psess);
    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.role %d", __FILE__, __func__, __LINE__, psess->info.role);

    if(ERPS_ROLE_OWNER == psess->info.role)
    {
        erps_stop_wtr_timer(psess);
        erps_stop_wtb_timer(psess);
    }

    psess->info.current_status = ERPS_STATE_MS;
    return ERRNO_SUCCESS;
}

int erps_pending_handle_admin_ms(struct erps_sess *psess)
{
    int ret = ERRNO_FAIL;
    uint8_t subtype_block = 0, subtype_foward = 0;

    if(NULL == psess)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.event_block %d psess->info.block_interface %d psess->info.east_interface %d psess->info.west_interface %d",
               __FILE__, __func__, __LINE__, psess->info.event_block, psess->info.block_interface, psess->info.east_interface, psess->info.west_interface);

    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.role %d", __FILE__, __func__, __LINE__, psess->info.role);

    if(ERPS_ROLE_OWNER == psess->info.role)
    {
        erps_stop_wtr_timer(psess);
        erps_stop_wtb_timer(psess);
    }

    if((psess->info.event_block) && (psess->info.event_block == psess->info.block_interface))
    {
        //tx r-aps(ms,dnf)
        psess->r_aps.rb = 0;
        psess->r_aps.dnf = 1;
        psess->r_aps.request_state = ERPS_MANUAL_SWITCH;
        ret = raps_send_burst(psess);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return ERRNO_FAIL;
        }

        //ublock non-request ring port
        if((psess->info.east_interface) && (psess->info.event_block == psess->info.west_interface))
        {
            subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_EAST;
        }
        else if((psess->info.west_interface) && (psess->info.event_block == psess->info.east_interface))
        {
            subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_WEST;
        }

        ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_foward %d", __FILE__, __func__, __LINE__, subtype_foward);

        if(subtype_foward)
        {
            psess->info.block_interface = psess->info.event_block;
            ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                               IPC_TYPE_ERPS, subtype_foward, IPC_OPCODE_UPDATE, psess->info.sess_id);

            if(ret != ERRNO_SUCCESS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                return ERRNO_IPC;
            }
        }
    }
    else
    {
        //block request ring port
        if((psess->info.east_interface) && (psess->info.event_block == psess->info.east_interface))
        {
            subtype_block = OAM_SUBTYPE_ERPS_BLOCK_EAST;

            if(psess->info.west_interface)
            {
                subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_WEST;
            }
        }
        else if((psess->info.west_interface) && (psess->info.east_interface == psess->info.west_interface))
        {
            subtype_block = OAM_SUBTYPE_ERPS_BLOCK_WEST;

            if(psess->info.east_interface)
            {
                subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_EAST;
            }
        }

        ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_block %d", __FILE__, __func__, __LINE__, subtype_block);

        if(subtype_block)
        {
            ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                               IPC_TYPE_ERPS, subtype_block, IPC_OPCODE_UPDATE, psess->info.sess_id);

            if(ret != ERRNO_SUCCESS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                return ERRNO_IPC;
            }
        }

        psess->info.block_interface = psess->info.event_block;
        //tx r-aps(ms)
        psess->r_aps.rb = 0;
        psess->r_aps.dnf = 0;
        psess->r_aps.request_state = ERPS_MANUAL_SWITCH;
        ret = raps_send_burst(psess);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return ERRNO_FAIL;
        }

        //unblock non-request ring port
        ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_foward %d", __FILE__, __func__, __LINE__, subtype_foward);

        if(subtype_foward)
        {
            ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                               IPC_TYPE_ERPS, subtype_foward, IPC_OPCODE_UPDATE, psess->info.sess_id);

            if(ret != ERRNO_SUCCESS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                return ERRNO_IPC;
            }
        }
    }

    psess->info.current_status = ERPS_STATE_MS;
    return ERRNO_SUCCESS;
}

int erps_pending_handle_wtre(struct erps_sess *psess)
{
    int ret = ERRNO_FAIL;
    uint8_t subtype_block = 0, subtype_foward = 0;

    if(NULL == psess)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.rpl_interface %d psess->info.block_interface %d psess->info.east_interface %d psess->info.west_interface %d",
               __FILE__, __func__, __LINE__, psess->info.rpl_interface, psess->info.block_interface, psess->info.east_interface, psess->info.west_interface);

    if(ERPS_ROLE_OWNER == psess->info.role)
    {
        erps_stop_wtb_timer(psess);

        if(psess->info.rpl_interface == psess->info.block_interface)
        {
            //tx r-aps(nr,rb,dnf)
            psess->r_aps.rb = 1;
            psess->r_aps.dnf = 1;
            psess->r_aps.request_state = ERPS_NO_REQUEST;

            //unblock non-rpl port
            if((psess->info.east_interface) && (psess->info.rpl_interface == psess->info.west_interface))
            {
                subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_EAST;
            }
            else if((psess->info.west_interface) && (psess->info.rpl_interface == psess->info.east_interface))
            {
                subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_WEST;
            }

            ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_foward %d", __FILE__, __func__, __LINE__, subtype_foward);

            if(subtype_foward)
            {
                psess->info.block_interface = psess->info.rpl_interface;
				psess->info.sendMsgRapsNRRB = 1;
                ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ERPS, subtype_foward, IPC_OPCODE_UPDATE, psess->info.sess_id);

                if(ret != ERRNO_SUCCESS)
                {
                    ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                }
				psess->info.sendMsgRapsNRRB = 0;
            }
			
			ret = raps_send_burst(psess);
            if(ret != ERRNO_SUCCESS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                return ERRNO_FAIL;
            }

        }
        else
        {
            //block rpl port
            if((psess->info.east_interface) && (psess->info.rpl_interface == psess->info.east_interface))
            {
                subtype_block = OAM_SUBTYPE_ERPS_BLOCK_EAST;

                if(psess->info.west_interface)
                {
                    subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_WEST;
                }
            }
            else if((psess->info.west_interface) && (psess->info.rpl_interface == psess->info.west_interface))
            {
                subtype_block = OAM_SUBTYPE_ERPS_BLOCK_WEST;

                if(psess->info.east_interface)
                {
                    subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_EAST;
                }
            }

            ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_block %d", __FILE__, __func__, __LINE__, subtype_block);

            if(subtype_block)
            {
            	psess->info.sendMsgRapsNRRB = 1;
                ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ERPS, subtype_block, IPC_OPCODE_UPDATE, psess->info.sess_id);

                if(ret != ERRNO_SUCCESS)
                {
                    ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                }
				psess->info.sendMsgRapsNRRB = 0;
            }

            psess->info.block_interface = psess->info.rpl_interface;
            //tx r-aps(nr,rb)
            psess->r_aps.rb = 1;
            psess->r_aps.dnf = 0;
            psess->r_aps.request_state = ERPS_NO_REQUEST;

            //unblock non-request ring port
            ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_foward %d", __FILE__, __func__, __LINE__, subtype_foward);

            if(subtype_foward)
            {
            	psess->info.sendMsgRapsNRRB = 1;
                ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ERPS, subtype_foward, IPC_OPCODE_UPDATE, psess->info.sess_id);

                if(ret != ERRNO_SUCCESS)
                {
                    ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                }
				psess->info.sendMsgRapsNRRB = 0;
            }
			
			ret = raps_send_burst(psess);
            if(ret != ERRNO_SUCCESS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                return ERRNO_FAIL;
            }
			
        }
    }

    psess->info.current_status = ERPS_STATE_IDLE;
    return ERRNO_SUCCESS;
}
int erps_pending_handle_wtbe(struct erps_sess *psess)
{
    int ret = ERRNO_FAIL;
    uint8_t subtype_block = 0, subtype_foward = 0;

    if(NULL == psess)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.rpl_interface %d psess->info.block_interface %d psess->info.east_interface %d psess->info.west_interface %d",
               __FILE__, __func__, __LINE__, psess->info.rpl_interface, psess->info.block_interface, psess->info.east_interface, psess->info.west_interface);

    if(ERPS_ROLE_OWNER == psess->info.role)
    {
        erps_stop_wtr_timer(psess);

        if(psess->info.rpl_interface == psess->info.block_interface)
        {
            //tx r-aps(nr,rb,dnf)
            psess->r_aps.rb = 1;
            psess->r_aps.dnf = 1;
            psess->r_aps.request_state = ERPS_NO_REQUEST;

            //unblock non-rpl port
            if((psess->info.east_interface) && (psess->info.rpl_interface == psess->info.west_interface))
            {
                subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_EAST;
            }
            else if((psess->info.west_interface) && (psess->info.rpl_interface == psess->info.east_interface))
            {
                subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_WEST;
            }

            ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_foward %d", __FILE__, __func__, __LINE__, subtype_foward);

            if(subtype_foward)
            {
                psess->info.block_interface = psess->info.rpl_interface;
				psess->info.sendMsgRapsNRRB = 1;
                ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ERPS, subtype_foward, IPC_OPCODE_UPDATE, psess->info.sess_id);

                if(ret != ERRNO_SUCCESS)
                {
                    ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                }
				psess->info.sendMsgRapsNRRB = 0;
            }
			
            ret = raps_send_burst(psess);
            if(ret != ERRNO_SUCCESS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                return ERRNO_FAIL;
            }
			
        }
        else
        {
            //block rpl port
            if((psess->info.east_interface) && (psess->info.rpl_interface == psess->info.east_interface))
            {
                subtype_block = OAM_SUBTYPE_ERPS_BLOCK_EAST;

                if(psess->info.west_interface)
                {
                    subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_WEST;
                }
            }
            else if((psess->info.west_interface) && (psess->info.rpl_interface == psess->info.west_interface))
            {
                subtype_block = OAM_SUBTYPE_ERPS_BLOCK_WEST;

                if(psess->info.east_interface)
                {
                    subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_EAST;
                }
            }

            ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_block %d", __FILE__, __func__, __LINE__, subtype_block);

            if(subtype_block)
            {
            	psess->info.sendMsgRapsNRRB = 1;
                ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ERPS, subtype_block, IPC_OPCODE_UPDATE, psess->info.sess_id);

                if(ret != ERRNO_SUCCESS)
                {
                    ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                }
				psess->info.sendMsgRapsNRRB = 0;
            }

            psess->info.block_interface = psess->info.rpl_interface;
            //tx r-aps(nr,rb)
            psess->r_aps.rb = 1;
            psess->r_aps.dnf = 0;
            psess->r_aps.request_state = ERPS_NO_REQUEST;

            //unblock non-request ring port
            ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_foward %d", __FILE__, __func__, __LINE__, subtype_foward);

            if(subtype_foward)
            {
            	psess->info.sendMsgRapsNRRB = 1;
                ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ERPS, subtype_foward, IPC_OPCODE_UPDATE, psess->info.sess_id);

                if(ret != ERRNO_SUCCESS)
                {
                    ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                }
				psess->info.sendMsgRapsNRRB = 0;
            }
			
			ret = raps_send_burst(psess);
            if(ret != ERRNO_SUCCESS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                return ERRNO_FAIL;
            }
			
        }
    }

    psess->info.current_status = ERPS_STATE_IDLE;
    return ERRNO_SUCCESS;
}

int erps_pending_handle_nrrb(struct erps_sess *psess)
{
    int ret = ERRNO_FAIL;
    uint8_t subtype_block = 0, subtype_foward = 0;

    if(NULL == psess)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d  psess->info.block_interface %d psess->info.east_interface %d psess->info.west_interface %d",
               __FILE__, __func__, __LINE__, psess->info.block_interface, psess->info.east_interface, psess->info.west_interface);

    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.role %d", __FILE__, __func__, __LINE__, psess->info.role);

    if(ERPS_ROLE_OWNER == psess->info.role)
    {
        erps_stop_wtr_timer(psess);
        erps_stop_wtb_timer(psess);
    }
    else if(ERPS_ROLE_NONOWNER == psess->info.role)
    {
        //unblock ring ports
        if((psess->info.east_interface) && (psess->info.block_interface == psess->info.east_interface))
        {
            subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_EAST;
        }
        else if((psess->info.west_interface) && (psess->info.block_interface == psess->info.west_interface))
        {
            subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_WEST;
        }

        ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_foward %d", __FILE__, __func__, __LINE__, subtype_foward);

        if(subtype_foward)
        {
            ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                               IPC_TYPE_ERPS, subtype_foward, IPC_OPCODE_UPDATE, psess->info.sess_id);

            if(ret != ERRNO_SUCCESS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                return ERRNO_IPC;
            }

            psess->info.block_interface = psess->info.rpl_interface;
        }

        //stop tx r-aps
        raps_stop(psess);
    }
    else if(ERPS_ROLE_NEIGHBOUR == psess->info.role)
    {
        //block rpl port
        if((psess->info.east_interface) && (psess->info.rpl_interface == psess->info.east_interface))
        {
            subtype_block = OAM_SUBTYPE_ERPS_BLOCK_EAST;

            if(psess->info.west_interface)
            {
                subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_WEST;
            }
        }
        else if((psess->info.west_interface) && (psess->info.rpl_interface == psess->info.west_interface))
        {
            subtype_block = OAM_SUBTYPE_ERPS_BLOCK_WEST;

            if(psess->info.east_interface)
            {
                subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_EAST;
            }
        }

        ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_block %d", __FILE__, __func__, __LINE__, subtype_block);

        if(subtype_block)
        {
            ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                               IPC_TYPE_ERPS, subtype_block, IPC_OPCODE_UPDATE, psess->info.sess_id);

            if(ret != ERRNO_SUCCESS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                return ERRNO_IPC;
            }
        }

        psess->info.block_interface = psess->info.rpl_interface;
        //unblock non-request ring port
        ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_foward %d", __FILE__, __func__, __LINE__, subtype_foward);

        if(subtype_foward)
        {
            ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                               IPC_TYPE_ERPS, subtype_foward, IPC_OPCODE_UPDATE, psess->info.sess_id);

            if(ret != ERRNO_SUCCESS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                return ERRNO_IPC;
            }
        }

        //stop tx r-aps
        raps_stop(psess);
    }

    psess->info.current_status = ERPS_STATE_IDLE;
    return ERRNO_SUCCESS;
}

int erps_pending_handle_nr(struct erps_sess *psess, struct raps_pkt *raps_pdu)
{
    int ret = ERRNO_FAIL;
    uint8_t subtype = 0;

    if((NULL == psess) || (NULL == raps_pdu))
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d sess id %d", __FILE__, __func__, __LINE__, psess->info.sess_id);

    ret = erps_node_id_higher(raps_pdu->node_id, psess->info.node_id);

    if(ret == 1)
    {
        //unblock non-failed ring port
        if((0 == psess->info.east_interface_flag) && (0 == psess->info.west_interface_flag))
        {
            if((psess->info.east_interface) && (psess->info.block_interface == psess->info.east_interface))
            {
                subtype = OAM_SUBTYPE_ERPS_FOWARD_EAST;
            }
            else if((psess->info.west_interface) && (psess->info.block_interface == psess->info.west_interface))
            {
                subtype = OAM_SUBTYPE_ERPS_FOWARD_WEST;
            }

            ERPS_LOG_DBG("%s: '%s'--the line of %d subtype %d", __FILE__, __func__, __LINE__, subtype);

            if(subtype)
            {
                psess->info.block_interface = 0;
                ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ERPS, subtype, IPC_OPCODE_UPDATE, psess->info.sess_id);

                if(ret != ERRNO_SUCCESS)
                {
                    ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                    return ERRNO_IPC;
                }
            }
        }

        //stop tx r-aps
        raps_stop(psess);
    }

    psess->info.current_status = ERPS_STATE_PENDING;

    if(0 == subtype)
    {
        return ERRNO_EXISTED;
    }
    else
    {
        return ERRNO_SUCCESS;
    }
}


//same action
int erps_x_handle_admin_fs(struct erps_sess *psess)
{
    int ret = ERRNO_FAIL;
    uint8_t subtype_block = 0, subtype_foward = 0;

    if(NULL == psess)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.event_block %d psess->info.block_interface %d psess->info.east_interface %d psess->info.west_interface %d",
               __FILE__, __func__, __LINE__, psess->info.event_block, psess->info.block_interface, psess->info.east_interface, psess->info.west_interface);

    if((psess->info.event_block) && (psess->info.event_block == psess->info.block_interface))
    {
        //tx r-aps(fs,dnf)
        psess->r_aps.rb = 0;
        psess->r_aps.dnf = 1;
        psess->r_aps.request_state = ERPS_FORCED_SWITCH;
        ret = raps_send_burst(psess);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return ERRNO_FAIL;
        }

        //unblock non-requested ring port
        if((psess->info.east_interface) && (psess->info.event_block == psess->info.west_interface))
        {
            subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_EAST;
        }
        else if((psess->info.west_interface) && (psess->info.event_block == psess->info.east_interface))
        {
            subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_WEST;
        }

        ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_foward %d", __FILE__, __func__, __LINE__, subtype_foward);

        if(subtype_foward)
        {
            psess->info.block_interface = psess->info.event_block;
            ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                               IPC_TYPE_ERPS, subtype_foward, IPC_OPCODE_UPDATE, psess->info.sess_id);

            if(ret != ERRNO_SUCCESS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                return ERRNO_IPC;
            }
        }

    }
    else
    {
        //block request ring port
        if((psess->info.east_interface) && (psess->info.event_block == psess->info.east_interface))
        {
            subtype_block = OAM_SUBTYPE_ERPS_BLOCK_EAST;

            if(psess->info.west_interface)
            {
                subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_WEST;
            }
        }
        else if((psess->info.west_interface) && (psess->info.event_block == psess->info.west_interface))
        {
            subtype_block = OAM_SUBTYPE_ERPS_BLOCK_WEST;

            if(psess->info.east_interface)
            {
                subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_EAST;
            }
        }

        ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_block %d", __FILE__, __func__, __LINE__, subtype_block);

        if(subtype_block)
        {
            ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                               IPC_TYPE_ERPS, subtype_block, IPC_OPCODE_UPDATE, psess->info.sess_id);

            if(ret != ERRNO_SUCCESS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                return ERRNO_IPC;
            }
        }

        psess->info.block_interface = psess->info.event_block;
        //tx r-aps(fs)
        psess->r_aps.rb = 0;
        psess->r_aps.dnf = 0;
        psess->r_aps.request_state = ERPS_FORCED_SWITCH;
        ret = raps_send_burst(psess);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return ERRNO_FAIL;
        }

        //unblock non-request ring port
        ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_foward %d", __FILE__, __func__, __LINE__, subtype_foward);

        if(subtype_foward)
        {
            ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                               IPC_TYPE_ERPS, subtype_foward, IPC_OPCODE_UPDATE, psess->info.sess_id);

            if(ret != ERRNO_SUCCESS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                return ERRNO_IPC;
            }
        }
    }

    psess->info.current_status = ERPS_STATE_FS;
    return ERRNO_SUCCESS;
}

int erps_x_handle_raps_fs(struct erps_sess *psess)
{
    int ret = ERRNO_FAIL;
    uint8_t subtype = 0;

    if(NULL == psess)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d  psess->info.block_interface %d psess->info.east_interface %d psess->info.west_interface %d",
               __FILE__, __func__, __LINE__, psess->info.block_interface, psess->info.east_interface, psess->info.west_interface);

    //unblock ring port
    if((psess->info.east_interface) && (psess->info.block_interface == psess->info.east_interface))
    {
        subtype = OAM_SUBTYPE_ERPS_FOWARD_EAST;
    }
    else if((psess->info.west_interface) && (psess->info.block_interface == psess->info.west_interface))
    {
        subtype = OAM_SUBTYPE_ERPS_FOWARD_WEST;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d subtype %d", __FILE__, __func__, __LINE__, subtype);

    if(subtype)
    {
        // psess->info.block_interface = psess->info.event_block;
        psess->info.block_interface = 0;
        ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                           IPC_TYPE_ERPS, subtype, IPC_OPCODE_UPDATE, psess->info.sess_id);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return ERRNO_IPC;
        }
    }

    //stop tx r-aps
    raps_stop(psess);

    psess->info.current_status = ERPS_STATE_FS;
    return ERRNO_SUCCESS;
}

int erps_x_handle_local_sf(struct erps_sess *psess)
{
    int ret = ERRNO_FAIL;
    uint8_t subtype_block = 0, subtype_foward = 0;

    if(NULL == psess)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.event_block %d psess->info.block_interface %d psess->info.east_interface %d psess->info.west_interface %d",
               __FILE__, __func__, __LINE__, psess->info.event_block, psess->info.block_interface, psess->info.east_interface, psess->info.west_interface);

    if((psess->info.event_block) && (psess->info.event_block == psess->info.block_interface))
    {
        //tx r-aps(sf,dnf)
        psess->r_aps.rb = 0;
        psess->r_aps.dnf = 1;
        psess->r_aps.request_state = ERPS_SIGNAL_FAIL;
        ret = raps_send_burst(psess);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return ERRNO_FAIL;
        }

        //unblock non-failed ring port
        if((psess->info.east_interface) && (psess->info.event_block == psess->info.west_interface))
        {
            subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_EAST;
        }
        else if((psess->info.west_interface) && (psess->info.event_block == psess->info.east_interface))
        {
            subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_WEST;
        }

        ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_foward %d", __FILE__, __func__, __LINE__, subtype_foward);

        if(subtype_foward)
        {
            psess->info.block_interface = psess->info.event_block;
            ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                               IPC_TYPE_ERPS, subtype_foward, IPC_OPCODE_UPDATE, psess->info.sess_id);

            if(ret != ERRNO_SUCCESS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                return ERRNO_IPC;
            }
        }

    }
    else
    {
        //block failed ring port
        if((psess->info.east_interface) && (psess->info.event_block == psess->info.east_interface))
        {
            subtype_block = OAM_SUBTYPE_ERPS_BLOCK_EAST;

            if(psess->info.west_interface)
            {
                subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_WEST;
            }
        }
        else if((psess->info.west_interface) && (psess->info.event_block == psess->info.west_interface))
        {
            subtype_block = OAM_SUBTYPE_ERPS_BLOCK_WEST;

            if(psess->info.east_interface)
            {
                subtype_foward = OAM_SUBTYPE_ERPS_FOWARD_EAST;
            }
        }

        ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_block %d", __FILE__, __func__, __LINE__, subtype_block);

        if(subtype_block)
        {
            ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                               IPC_TYPE_ERPS, subtype_block, IPC_OPCODE_UPDATE, psess->info.sess_id);

            if(ret != ERRNO_SUCCESS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                return ERRNO_IPC;
            }
        }

        psess->info.block_interface = psess->info.event_block;
        //tx r-aps(sf)
        psess->r_aps.rb = 0;
        psess->r_aps.dnf = 0;
        psess->r_aps.request_state = ERPS_SIGNAL_FAIL;
        ret = raps_send_burst(psess);

        if(ret != ERRNO_SUCCESS)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return ERRNO_FAIL;
        }

        //unblock non-request ring port
        ERPS_LOG_DBG("%s: '%s'--the line of %d subtype_foward %d", __FILE__, __func__, __LINE__, subtype_foward);

        if(subtype_foward)
        {
            ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct erps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                               IPC_TYPE_ERPS, subtype_foward, IPC_OPCODE_UPDATE, psess->info.sess_id);

            if(ret != ERRNO_SUCCESS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
                return ERRNO_IPC;
            }
        }
    }

    psess->info.current_status = ERPS_STATE_PROTECTION;
    return ERRNO_SUCCESS;
}

int erps_x_handle_raps_nr(struct erps_sess *psess)
{

    if(NULL == psess)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.role %d", __FILE__, __func__, __LINE__, psess->info.role);

    if((ERPS_ROLE_OWNER == psess->info.role) && (FAILBACK_ENABLE == psess->info.failback))
    {

        ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.current_status %d", __FILE__, __func__, __LINE__, psess->info.current_status);

        if(psess->info.current_status == ERPS_STATE_PROTECTION_REMOTE)
        {
            erps_start_wtr_timer(psess);
        }
        else if((ERPS_STATE_MS == psess->info.current_status) || (ERPS_STATE_FS == psess->info.current_status))
        {
            erps_start_wtb_timer(psess);
        }
    }

    psess->info.current_status = ERPS_STATE_PENDING;

    return ERRNO_SUCCESS;
}

void erps_link_down_handler(struct erps_sess *psess)
{
    struct l2if *pif = NULL;
    int ret = 0;
    int is_down = 0;

    if(NULL == psess)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d sess_id %d", __FILE__, __func__, __LINE__, psess->info.sess_id);

    if(psess->info.east_interface)
    {
        pif = l2if_lookup(psess->info.east_interface);

        if(NULL == pif)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
            return;
        }

        ERPS_LOG_DBG("%s: '%s'--the line of %d pif->down_flag %d", __FILE__, __func__, __LINE__, pif->down_flag);

        if(IFNET_LINKDOWN == pif->down_flag)
        {
            psess->info.current_event = ERPS_EVENT_LOCAL_SF;
            psess->info.event_block = psess->info.east_interface;
            is_down = 1;
        }
        else if(psess->east_cfm_session)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d psess->east_cfm_session->state %d", __FILE__, __func__, __LINE__, psess->east_cfm_session->state);

            if(OAM_STATUS_DOWN == psess->east_cfm_session->state)
            {
                psess->info.current_event = ERPS_EVENT_LOCAL_SF;
                psess->info.event_block = psess->info.east_interface;
                is_down = 1;
            }
        }
    }

    if(psess->info.west_interface)
    {
        pif = l2if_lookup(psess->info.west_interface);

        if(NULL == pif)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
            return;
        }

        ERPS_LOG_DBG("%s: '%s'--the line of %d pif->down_flag %d", __FILE__, __func__, __LINE__, pif->down_flag);

        if(IFNET_LINKDOWN == pif->down_flag)
        {
            psess->info.current_event = ERPS_EVENT_LOCAL_SF;
            psess->info.event_block = psess->info.west_interface;
            is_down = 1;
        }
        else if(psess->west_cfm_session)
        {

            ERPS_LOG_DBG("%s: '%s'--the line of %d psess->west_cfm_session->state %d", __FILE__, __func__, __LINE__, psess->west_cfm_session->state);

            if(OAM_STATUS_DOWN == psess->west_cfm_session->state)
            {
                psess->info.current_event = ERPS_EVENT_LOCAL_SF;
                psess->info.event_block = psess->info.east_interface;
                is_down = 1;
            }
        }
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d is_down %d", __FILE__, __func__, __LINE__, is_down);

    if(is_down)
    {
        ret = erps_fsm(psess, NULL);

        if(ret)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);
            return;
        }
    }

    return;
}

