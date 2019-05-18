#include <lib/types.h>
#include <lib/ifm_common.h>
#include <lib/hash1.h>
#include <lib/msg_ipc.h>
#include <lib/errcode.h>
#include <lib/log.h>


#include "l2_if.h"
#include "raps/erps_fsm_action.h"
#include "raps/erps_fsm.h"
#include "raps/erps_timer.h"


struct cfm_sess *erps_find_cfm(uint32_t ifindex, uint16_t sess_id)
{
    struct cfm_sess *sess = NULL;
    sess = cfm_session_lookup_new(sess_id);

    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);

    if(sess != NULL)
    {
        if(sess->ifindex == ifindex)
        {

            ERPS_LOG_DBG("%s: '%s'--the line of %d sess id %d", __FILE__, __func__, __LINE__, sess->sess_id);
            return sess;
        }
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
    return NULL;
}

int erps_init_session(struct erps_sess *psess)
{
    struct l2if *pif_east = NULL;
    struct l2if *pif_west = NULL;

    int is_change = 0;
    int ret = ERRNO_FAIL;

    if(NULL == psess)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d sess id %d", __FILE__, __func__, __LINE__, psess->info.sess_id);

    psess->info.current_event = ERPS_EVENT_RAPS_NR;
    psess->info.east_interface_flag = 0;
    psess->info.west_interface_flag = 0;
    
    if(psess->info.east_interface)
    {
        pif_east = l2if_lookup(psess->info.east_interface);

        if(pif_east == NULL)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
            return ERRNO_FAIL;
        }


        ERPS_LOG_DBG("%s: '%s'--the line of %d pif_east->down_flag %d", __FILE__, __func__, __LINE__, pif_east->down_flag);

        if(IFNET_LINKDOWN == pif_east->down_flag)
        {
            psess->info.current_event = ERPS_EVENT_LOCAL_SF;
            psess->info.east_interface_flag = 1;
            is_change = 1;
        }
        else if(psess->east_cfm_session)
        {

            ERPS_LOG_DBG("%s: '%s'--the line of %d psess->east_cfm_session->state %d", __FILE__, __func__, __LINE__, psess->east_cfm_session->state);

            if(OAM_STATUS_DOWN == psess->east_cfm_session->state)
            {
                psess->info.current_event = ERPS_EVENT_LOCAL_SF;
                psess->info.east_interface_flag = 1;
                is_change = 1;
            }
        }

        ERPS_LOG_DBG("%s: '%s'--the line of %d is_change %d", __FILE__, __func__, __LINE__, is_change);

        if(is_change)
        {
            psess->info.event_block = psess->info.east_interface;
            ret = erps_fsm(psess, NULL);

            if(ret)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                return ERRNO_FAIL;
            }
			is_change = 0;
        }
    }

    if(psess->info.west_interface)
    {
        pif_west = l2if_lookup(psess->info.west_interface);

        if(NULL == pif_west)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
            return ERRNO_FAIL;
        }
        else if(IFNET_LINKDOWN == pif_west->down_flag)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d pif_west->down_flag %d", __FILE__, __func__, __LINE__, pif_west->down_flag);
            psess->info.current_event = ERPS_EVENT_LOCAL_SF;
            psess->info.west_interface_flag = 1;
            is_change = 1;
        }
        else if(psess->west_cfm_session)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d psess->west_cfm_session->state %d", __FILE__, __func__, __LINE__, psess->west_cfm_session->state);

            if(OAM_STATUS_DOWN == psess->west_cfm_session->state)
            {
                psess->info.current_event = ERPS_EVENT_LOCAL_SF;
                psess->info.west_interface_flag = 1;
                is_change = 1;
            }
        }

        ERPS_LOG_DBG("%s: '%s'--the line of %d is_change %d", __FILE__, __func__, __LINE__, is_change);

        if(is_change)
        {
            psess->info.event_block = psess->info.west_interface;
            ret = erps_fsm(psess, NULL);

            if(ret)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                return ERRNO_FAIL;
            }
			is_change = 0;
        }
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d ", __FILE__, __func__, __LINE__, ret);
    return ret;
}


int erps_state_update(uint32_t ifindex, uint16_t sess_id, enum ERPS_PORT_EVENT event)
{
    struct hash_bucket *bucket = NULL;
    struct erps_sess *psess = NULL;
    int cursor = 0;
    int ret ;

    if(ifindex == 0)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);

    HASH_BUCKET_LOOP(bucket, cursor, erps_session_table)
    {
        psess = (struct erps_sess *)bucket->data;

        ERPS_LOG_DBG("%s: '%s'--the line of %d ifindex %d psess->info.east_interface %d psess->info.west_interface %d",
                   __FILE__, __func__, __LINE__, ifindex, psess->info.east_interface, psess->info.west_interface);

        ERPS_LOG_DBG("%s: '%s'--the line of %d elps sess id %d cfm sess_id %d  event %d", __FILE__, __func__, __LINE__, psess->info.sess_id, sess_id, event);

        if(ifindex == psess->info.east_interface)
        {

            if(ERPS_PORT_DOWN == event)
            {
                psess->info.event_block = psess->info.east_interface;
                psess->info.current_event = ERPS_EVENT_LOCAL_SF;
                psess->info.east_interface_flag = 1;
            }
            else if(ERPS_PORT_UP == event)
            {
                if(psess->east_cfm_session)
                {
                    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->east_cfm_session->state %d", __FILE__, __func__, __LINE__, psess->east_cfm_session->state);

                    if(psess->east_cfm_session->state == OAM_STATUS_DOWN)
                    {
                        psess->info.event_block = psess->info.east_interface;
                        psess->info.current_event = ERPS_EVENT_LOCAL_SF;
                        psess->info.east_interface_flag = 1;
                    }
                    else
                    {
                        psess->info.east_interface_flag = 0;
                        ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.current_status %d psess->info.event_block %d",
                                   __FILE__, __func__, __LINE__, psess->info.current_status, psess->info.event_block);

                        if((psess->info.current_status == ERPS_STATE_PROTECTION) &&
                                (psess->info.event_block == psess->info.east_interface))
                        {
                            if(psess->info.west_interface_flag)
                            {
                                psess->info.event_block = psess->info.west_interface;
                                psess->info.current_event = ERPS_EVENT_LOCAL_SF;
                            }
                            else
                            {
                                psess->info.event_block = 0;
                                psess->info.current_event = ERPS_EVENT_LOCAL_CLEAR_SF;
                            }
                        }
                        else
                        {
                            ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                            continue;
                        }
                    }
                }
                else
                {
                    psess->info.east_interface_flag = 0;
                    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.current_status %d psess->info.event_block %d",
                               __FILE__, __func__, __LINE__, psess->info.current_status, psess->info.event_block);

                    if((ERPS_STATE_PROTECTION == psess->info.current_status) &&
                            (psess->info.event_block == psess->info.east_interface))
                    {
                        if(psess->info.west_interface_flag)
                        {
                            psess->info.event_block = psess->info.west_interface;
                            psess->info.current_event = ERPS_EVENT_LOCAL_SF;
                        }
                        else
                        {
                            psess->info.event_block = 0;
                            psess->info.current_event = ERPS_EVENT_LOCAL_CLEAR_SF;
                        }
                    }
                    else
                    {
                        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                        continue;
                    }
                }
            }
            else if(ERPS_CC_DOWN == event)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.east_cfm_session_id %d",
                           __FILE__, __func__, __LINE__, psess->info.east_cfm_session_id);

                if((psess->east_cfm_session) || (psess->info.east_cfm_session_id == sess_id))
                {
                    psess->info.event_block = psess->info.east_interface;
                    psess->info.current_event = ERPS_EVENT_LOCAL_SF;
                    psess->info.east_interface_flag = 1;
                }
                else
                {
                    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                    continue;
                }
            }
            else if((ERPS_CC_UP == event) || (ERPS_CC_DELETE == event))
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.east_cfm_session_id %d",
                           __FILE__, __func__, __LINE__, psess->info.east_cfm_session_id);

                if((psess->east_cfm_session) || (psess->info.east_cfm_session_id == sess_id))
                {
                    psess->info.east_interface_flag = 0;
                    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.current_status %d psess->info.event_block %d",
                               __FILE__, __func__, __LINE__, psess->info.current_status, psess->info.event_block);

                    if(ERPS_CC_DELETE == event)
                    {
                        //psess->info.east_cfm_session_id = 0;
                        psess->east_cfm_session = NULL;
                    }

                    if((ERPS_STATE_PROTECTION == psess->info.current_status) &&
                            (psess->info.event_block == psess->info.east_interface))
                    {
                        if(psess->info.west_interface_flag)
                        {
                            psess->info.event_block = psess->info.west_interface;
                            psess->info.current_event = ERPS_EVENT_LOCAL_SF;
                        }
                        else
                        {
                            psess->info.event_block = 0;
                            psess->info.current_event = ERPS_EVENT_LOCAL_CLEAR_SF;
                        }
                    }
                    else
                    {
                        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                        continue;
                    }
                }
                else
                {
                    ERPS_LOG_DBG("%s:'%s'--the line of %d", __FILE__, __func__, __LINE__);
                    continue;
                }
            }
            else
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                return ERRNO_FAIL;
            }

            if((psess->info.holdoff) && ((ERPS_PORT_DOWN == event) || (ERPS_CC_DOWN == event)))
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);

                if(psess->holdoff_timer)
                {
                    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                    continue;
                }
                else
                {
                    erps_start_holdoff_timer(psess);
                }
            }
            else
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d sess_id %d ; current_event %d ; current_status %d",
                           __FILE__, __func__,__LINE__, psess->info.sess_id, psess->info.current_event, psess->info.current_status);
                ret = erps_fsm(psess, NULL);

                if(ret)
                {
                    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                    continue;
                }
            }
        }
        else if(ifindex == psess->info.west_interface)
        {
            if(ERPS_PORT_DOWN == event)
            {
                psess->info.event_block = psess->info.west_interface;
                psess->info.current_event = ERPS_EVENT_LOCAL_SF;
                psess->info.west_interface_flag = 1;
            }
            else if(ERPS_PORT_UP == event)
            {
                if(psess->west_cfm_session)
                {

                    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->west_cfm_session->state %d", __FILE__, __func__, __LINE__, psess->west_cfm_session->state);

                    if(OAM_STATUS_DOWN == psess->west_cfm_session->state)
                    {
                        psess->info.event_block = psess->info.west_interface;
                        psess->info.current_event = ERPS_EVENT_LOCAL_SF;
                        psess->info.west_interface_flag = 1;
                    }
                    else
                    {
                        psess->info.west_interface_flag = 0;
                        ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.current_status %d psess->info.event_block %d",
                                   __FILE__, __func__, __LINE__, psess->info.current_status, psess->info.event_block);

                        if((ERPS_STATE_PROTECTION == psess->info.current_status) &&
                                (psess->info.event_block == psess->info.west_interface))
                        {
                            if(psess->info.east_interface_flag)
                            {
                                psess->info.event_block = psess->info.east_interface;
                                psess->info.current_event = ERPS_EVENT_LOCAL_SF;
                            }
                            else
                            {
                                psess->info.event_block = 0;
                                psess->info.current_event = ERPS_EVENT_LOCAL_CLEAR_SF;
                            }

                        }
                        else
                        {
                            ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                            continue;
                        }
                    }
                }
                else
                {
                    psess->info.west_interface_flag = 0;
                    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.current_status %d psess->info.event_block %d",
                               __FILE__, __func__, __LINE__, psess->info.current_status, psess->info.event_block);

                    if((ERPS_STATE_PROTECTION == psess->info.current_status) &&
                            (psess->info.event_block == psess->info.west_interface))
                    {
                        if(psess->info.east_interface_flag)
                        {
                            psess->info.event_block = psess->info.east_interface;
                            psess->info.current_event = ERPS_EVENT_LOCAL_SF;
                        }
                        else
                        {
                            psess->info.event_block = 0;
                            psess->info.current_event = ERPS_EVENT_LOCAL_CLEAR_SF;
                        }
                    }
                    else
                    {
                        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                        continue;
                    }
                }
            }
            else if(ERPS_CC_DOWN == event)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.west_cfm_session_id %d",
                           __FILE__, __func__, __LINE__, psess->info.west_cfm_session_id);

                if((psess->west_cfm_session) || (psess->info.west_cfm_session_id == sess_id))
                {
                    psess->info.event_block = psess->info.west_interface;
                    psess->info.current_event = ERPS_EVENT_LOCAL_SF;
                    psess->info.west_interface_flag = 1;
                }
                else
                {
                    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                    continue;
                }
            }
            else if((ERPS_CC_UP == event) || (ERPS_CC_DELETE == event))
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.west_cfm_session_id %d",
                           __FILE__, __func__, __LINE__, psess->info.west_cfm_session_id);

                if((psess->west_cfm_session) || (psess->info.west_cfm_session_id == sess_id))
                {
                    psess->info.west_interface_flag = 0;
                    ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.current_status %d psess->info.event_block %d",
                               __FILE__, __func__, __LINE__, psess->info.current_status, psess->info.event_block);

                    if(ERPS_CC_DELETE == event)
                    {
                        //psess->info.west_cfm_session_id = 0;
                        psess->west_cfm_session = NULL;
                    }

                    if((ERPS_STATE_PROTECTION == psess->info.current_status) &&
                            (psess->info.event_block == psess->info.west_interface))
                    {
                        if(psess->info.east_interface_flag)
                        {
                            psess->info.event_block = psess->info.east_interface;
                            psess->info.current_event = ERPS_EVENT_LOCAL_SF;
                        }
                        else
                        {
                            psess->info.event_block = 0;
                            psess->info.current_event = ERPS_EVENT_LOCAL_CLEAR_SF;
                        }
                    }
                    else
                    {
                        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                        continue;
                    }
                }
                else
                {
                    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                    continue ;
                }
            }
            else
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                return ERRNO_FAIL;
            }

            if((psess->info.holdoff) && ((ERPS_PORT_DOWN == event) || (ERPS_CC_DOWN == event)))
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);

                if(psess->holdoff_timer)
                {
                    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                    continue;
                }
                else
                {
                    erps_start_holdoff_timer(psess);
                }
            }
            else
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d sess_id %d ; current_event %d ; current_status %d",
                           __FILE__, __func__,__LINE__, psess->info.sess_id, psess->info.current_event, psess->info.current_status);
                ret = erps_fsm(psess, NULL);

                if(ret)
                {
                    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                    continue;
                }
            }
        }
    }
    return ERRNO_SUCCESS;

}

int erps_fsm(struct erps_sess *psess, struct raps_pkt *raps_pdu)
{
    int ret = ERRNO_FAIL;

    if(NULL == psess)
    {
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--sess_id %d ; current_event %d ; current_status %d",
               __FILE__, __func__, psess->info.sess_id, psess->info.current_event, psess->info.current_status);

    switch(psess->info.current_event)
    {
        case ERPS_EVENT_ADMIN_CLEAR:
            if(ERPS_STATE_PENDING == psess->info.current_status)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                ret = erps_pending_handle_admin_clear(psess);
            }
            else if((ERPS_STATE_MS == psess->info.current_status) || (ERPS_STATE_FS == psess->info.current_status))
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                ret = erps_fsms_handle_admin_clear(psess);
            }

            break;

        case ERPS_EVENT_ADMIN_FS:
            if(ERPS_STATE_PENDING == psess->info.current_status)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                ret = erps_pending_handle_admin_fs(psess);
            }
            else if(ERPS_STATE_FS == psess->info.current_status)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                ret = erps_fs_handle_admin_fs(psess);
            }
            else
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                ret = erps_x_handle_admin_fs(psess);
            }

            break;

        case ERPS_EVENT_RAPS_FS:
            if(ERPS_STATE_PENDING == psess->info.current_status)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                ret = erps_pending_handle_raps_fs(psess);
            }
            else if(psess->info.current_status != ERPS_STATE_FS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                ret = erps_x_handle_raps_fs(psess);
            }

            break;

        case ERPS_EVENT_LOCAL_SF:
            if(ERPS_STATE_PENDING == psess->info.current_status)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                ret = erps_pending_handle_local_sf(psess);
            }
            else if(psess->info.current_status != ERPS_STATE_FS)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                ret = erps_x_handle_local_sf(psess);
            }

            break;

        case ERPS_EVENT_LOCAL_CLEAR_SF:
            if(ERPS_STATE_PROTECTION == psess->info.current_status)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                ret = erps_protection_handle_clear_sf(psess);
            }

            break;

        case ERPS_EVENT_RAPS_SF:
            if(ERPS_STATE_PENDING == psess->info.current_status)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                ret = erps_pending_handle_raps_sf(psess);
            }
            else if((ERPS_STATE_IDLE == psess->info.current_status) || (ERPS_STATE_MS == psess->info.current_status))
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                ret = erps_idlems_handle_raps_sf(psess);
            }

            break;

        case ERPS_EVENT_RAPS_MS:
            if(ERPS_STATE_IDLE == psess->info.current_status)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                ret = erps_idle_handle_raps_ms(psess);
            }
            else if(ERPS_STATE_MS == psess->info.current_status)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                ret = erps_ms_handle_raps_ms(psess);
            }
            else if(ERPS_STATE_PENDING == psess->info.current_status)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                ret = erps_pending_handle_raps_ms(psess);
            }

            break;

        case ERPS_EVENT_ADMIN_MS:
            if(ERPS_STATE_IDLE == psess->info.current_status)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                ret = erps_idle_handle_admin_ms(psess);
            }
            else if(ERPS_STATE_PENDING == psess->info.current_status)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                ret = erps_pending_handle_admin_ms(psess);
            }

            break;

        case ERPS_EVENT_WTR_EXP:
            if(ERPS_STATE_PENDING == psess->info.current_status)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                ret = erps_pending_handle_wtre(psess);
            }

            break;

        case ERPS_EVENT_WTB_EXP:
            if(ERPS_STATE_PENDING == psess->info.current_status)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                ret = erps_pending_handle_wtbe(psess);
            }

            break;

        case ERPS_EVENT_RAPS_NR_RB:
            if(ERPS_STATE_IDLE == psess->info.current_status)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                ret = erps_idle_handle_raps_nrrb(psess);
            }
            else if(ERPS_STATE_PENDING == psess->info.current_status)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                ret = erps_pending_handle_nrrb(psess);
            }

            break;

        case ERPS_EVENT_RAPS_NR:
            if(ERPS_STATE_IDLE == psess->info.current_status)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                ret = erps_idle_handle_raps_nr(psess, raps_pdu);
            }
            else if(ERPS_STATE_PENDING == psess->info.current_status)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                ret = erps_pending_handle_nr(psess, raps_pdu);
            }
            else if((ERPS_STATE_PROTECTION_REMOTE == psess->info.current_status) ||
                    (ERPS_STATE_MS == psess->info.current_status) ||
                    (ERPS_STATE_FS == psess->info.current_status))
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
                ret = erps_x_handle_raps_nr(psess);
            }

            break;

        default:
            return ERRNO_SUCCESS;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d ret %d", __FILE__, __func__, __LINE__, ret);

    if(ERRNO_SUCCESS == ret)
    {
        ret = erps_sess_update(psess);

        if(ret)
        {
            ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        }
    }

    return ret;
}
