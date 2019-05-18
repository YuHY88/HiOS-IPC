/*
 * =====================================================================================
 *
 *       Filename:  ces_msg.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/28/2017 09:09:18 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */

#include <string.h>
#include <unistd.h>
#include <lib/msg_ipc.h>
#include <lib/zassert.h>
#include <lib/thread.h>
#include <lib/ifm_common.h>
#include <lib/command.h>
#include <lib/errcode.h>
#include <lib/log.h>
#include <ifm/ifm.h>
#include <ces/ces_if.h>
#include <ces/ces_main.h>
#include <ces/ces_msg.h>
#include <ces/stm_if.h>

#if 0
/* 接收 IPC 消息 */
static int ces_msg_rcv_msg(struct ipc_mesg *pmesg)
{
    struct ipc_msghdr *phdr = NULL;
    struct ifm_event *pevent = NULL;
    //struct ces_if *pif = NULL;
    void *pdata = NULL;
    uint32_t ifindex = 0;
    int msg_num = IPC_MSG_LEN / sizeof(struct ces_if);
    struct ces_if cesif[msg_num];
    int ret = 0;
    int token = 100;
    uint32_t lp_status = 0;
    
    int base_msg_num = IPC_MSG_LEN/sizeof(struct stm_base_info);
    struct stm_base_info stm_base[base_msg_num];
    
    int rs_msg_num = IPC_MSG_LEN/sizeof(struct stm_rs_info);
    struct stm_rs_info  stm_rs[rs_msg_num];
    
    int ms_msg_num = IPC_MSG_LEN/sizeof(struct stm_ms_info);
    struct stm_ms_info  stm_ms[ms_msg_num];
    
    int hp_msg_num = IPC_MSG_LEN/sizeof(struct stm_hp_info);
    struct stm_hp_info  stm_hp[hp_msg_num];
    
    int msp_msg_num = IPC_MSG_LEN/sizeof(struct stm_msp_config);
    struct stm_msp_config  stm_msp[msp_msg_num];

    int dxc_msg_num = IPC_MSG_LEN/sizeof(struct stm_dxc_config);
    struct stm_dxc_config  stm_dxc[dxc_msg_num];

    while (token)
    {
        ret = ipc_recv_common(pmesg, MODULE_ID_CES);

        if (ret == -1)
        {
            return ERRNO_FAIL;
        }

        token--;

        /* process the ipc message */
        phdr = &(pmesg->msghdr);
        pdata = pmesg->msg_data;

        if (IPC_TYPE_IFM == phdr->msg_type)
        {
            if (IPC_OPCODE_EVENT == phdr->opcode)
            {
                pevent = (struct ifm_event *)pdata;
                ifindex = pevent->ifindex;

                if (IFNET_EVENT_DOWN == phdr->msg_subtype)
                {
                    zlog_debug("%s[%d] recv event notify ifindex 0x%0x down\n", __FUNCTION__, __LINE__, ifindex);
                    ces_if_down(ifindex);
                    stm_if_down(ifindex);
                }
                else if (IFNET_EVENT_UP == phdr->msg_subtype)
                {
                    zlog_debug("%s[%d] recv event notify ifindex 0x%0x up\n", __FUNCTION__, __LINE__, ifindex);
                    ces_if_up(ifindex);
                    stm_if_up(ifindex);
                }
                else if (IFNET_EVENT_IF_DELETE == phdr->msg_subtype)
                {
                    zlog_debug("%s[%d] recv event notify ifindex 0x%0x delete\n", __FUNCTION__, __LINE__, ifindex);
                    //delete函数内部会找ifindex是否存在，这里不需要重复
                    ces_if_delete(ifindex);
                    stm_if_delete(ifindex);
                }
                else if (IFNET_EVENT_IF_HIDE_RECOVER == phdr->msg_subtype)
                {
                    zlog_debug("%s[%d] recv event notify ifindex 0x%0x hide-recover\n", __FUNCTION__, __LINE__, ifindex);
                    ces_if_config_restore(ifindex);
                }
                else if(IFNET_EVENT_IF_ADD == phdr->msg_subtype)
                {
                    zlog_debug ( "%s[%d] recv add event notify ifindex 0x%0x add\n", __FUNCTION__, __LINE__, ifindex );                    
                    if (IFM_TYPE_IS_STM(ifindex))
                    {
                        stm_if_init();  //初始化创建stm接口
                    }
                }
            }
        }
        else if (IPC_TYPE_CES == phdr->msg_type)
        {
            if (IPC_OPCODE_UPDATE == phdr->opcode)
            {
                if(phdr->msg_subtype == 11)
                {
                    ces_if_sc01q_alarm_set((struct ces_sc01q_alarm_info *)pdata, phdr->msg_index);
                }
                else
                {
                    ifindex = phdr->msg_index;
                    ces_if_alarm_set(* (uint8_t *) pdata, ifindex, phdr->msg_subtype);
                }
            }
            else if (IPC_OPCODE_GET_BULK == phdr->opcode)
            {
                //ifindex = phdr->msg_index;
                ifindex = *(uint32_t *)pdata;
                memset(cesif, 0, msg_num * sizeof(struct ces_if));
                ret = ces_if_bulk_get(ifindex, cesif);
                if (ret > 0)
                {
                    ret = ipc_send_reply_bulk(cesif, ret * sizeof(struct ces_if), ret, phdr->sender_id,
                                              MODULE_ID_CES, IPC_TYPE_CES, phdr->msg_subtype, phdr->msg_index);
                }
                else
                {
                    ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_CES, IPC_TYPE_CES,
                                         phdr->msg_subtype, phdr->msg_index);
                }
            }
        }
        else if(IPC_TYPE_STM == phdr->msg_type)
        {
            if(IPC_OPCODE_UPDATE == phdr->opcode)
            {
                ifindex = phdr->msg_index;
                /*modified by liub 2018-10-18, release msg_subtype*/
                if(phdr->msg_subtype == STM_INFO_ALARM)
                {
                    stm_if_alarm_set((struct stm_alarm_info *)pdata, ifindex);
                }
                else if(phdr->msg_subtype == STM_INFO_STATISTICS)
                {
                    stm_if_statis_update((struct stm_statis_info *)pdata, ifindex);
                }
                else if(phdr->msg_subtype == STM_INFO_DXC_STATUS_GET) 
                {
                      stm_dxc_status_update(ifindex, * (uint8_t *) pdata);
                }
                else if(phdr->msg_subtype == STM_INFO_IMAGE_SWITCH) 
                {
                    stm_fpga_image_type_set(ifindex, * (uint8_t *)pdata);
                }
            }
            else if (IPC_OPCODE_GET_BULK == phdr->opcode)
            {
                //ifindex = phdr->msg_index;
                if(phdr->msg_subtype == STM_SNMP_TYPE_BASE_GET)
                {
                    ifindex = *(uint32_t *)pdata;
                    memset(stm_base, 0, base_msg_num*sizeof(struct stm_base_info));
                    ret  = stm_if_snmp_base_bulk_get(ifindex, stm_base);
                    if (ret > 0)
                    {
                        ret = ipc_send_reply_bulk(stm_base, ret*sizeof(struct stm_base_info), ret, phdr->sender_id,
                                                MODULE_ID_CES, IPC_TYPE_STM, phdr->msg_subtype, phdr->msg_index);
                    }
                    else
                    {
                        ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_CES, IPC_TYPE_STM,
                                                phdr->msg_subtype, phdr->msg_index);
                    }
                }
                else if(phdr->msg_subtype == STM_SNMP_TYPE_RS_GET)
                {
                    ifindex = *(uint32_t *)pdata;
                    memset(stm_rs, 0, rs_msg_num*sizeof(struct stm_rs_info));
                    ret  = stm_if_snmp_rs_bulk_get(ifindex, stm_rs);
                    if (ret > 0)
                    {
                        ret = ipc_send_reply_bulk(stm_rs, ret*sizeof(struct stm_rs_info), ret, phdr->sender_id,
                                                MODULE_ID_CES, IPC_TYPE_STM, phdr->msg_subtype, phdr->msg_index);
                    }
                    else
                    {
                        ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_CES, IPC_TYPE_STM,
                                                phdr->msg_subtype, phdr->msg_index);
                    }
                }
                else if(phdr->msg_subtype == STM_SNMP_TYPE_MS_GET)
                {
                    ifindex = *(uint32_t *)pdata;
                    memset(stm_ms, 0, ms_msg_num*sizeof(struct stm_ms_info));
                    ret  = stm_if_snmp_ms_bulk_get(ifindex, stm_ms);
                    if (ret > 0)
                    {
                        ret = ipc_send_reply_bulk(stm_ms, ret*sizeof(struct stm_ms_info), ret, phdr->sender_id,
                                                MODULE_ID_CES, IPC_TYPE_STM, phdr->msg_subtype, phdr->msg_index);
                    }
                    else
                    {
                        ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_CES, IPC_TYPE_STM,
                                                phdr->msg_subtype, phdr->msg_index);
                    }
                }
                else if(phdr->msg_subtype == STM_SNMP_TYPE_HP_GET)
                {
                    memset(stm_hp, 0, hp_msg_num*sizeof(struct stm_hp_info));
                    ret  = stm_if_snmp_hp_bulk_get(pdata, stm_hp);
                    if (ret > 0)
                    {
                        ret = ipc_send_reply_bulk(stm_hp, ret*sizeof(struct stm_hp_info), ret, phdr->sender_id,
                                                MODULE_ID_CES, IPC_TYPE_STM, phdr->msg_subtype, phdr->msg_index);
                    }
                    else
                    {
                        ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_CES, IPC_TYPE_STM,
                                                phdr->msg_subtype, phdr->msg_index);
                    }
                }
                else if(phdr->msg_subtype == STM_SNMP_TYPE_MSP_GET)
                {
                    ifindex = *(uint32_t *)pdata;
                    memset(stm_msp, 0, msp_msg_num*sizeof(struct stm_msp_config));
                    ret  = stm_if_snmp_msp_bulk_get(ifindex, stm_msp);
                    if (ret > 0)
                    {
                        ret = ipc_send_reply_bulk(stm_msp, ret*sizeof(struct stm_msp_config), ret, phdr->sender_id,
                                                MODULE_ID_CES, IPC_TYPE_STM, phdr->msg_subtype, phdr->msg_index);
                    }
                    else
                    {
                        ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_CES, IPC_TYPE_STM,
                                                phdr->msg_subtype, phdr->msg_index);
                    }
                }
                else if(phdr->msg_subtype == STM_SNMP_TYPE_SNCP_GET)
                {
                    memset(stm_dxc, 0, dxc_msg_num*sizeof(struct stm_dxc_config));
                    ret  = stm_if_snmp_dxc_bulk_get(pdata, phdr->data_len, stm_dxc);
                    if (ret > 0)
                    {
                        ret = ipc_send_reply_bulk(stm_dxc, ret*sizeof(struct stm_dxc_config), ret, phdr->sender_id,
                                                MODULE_ID_CES, IPC_TYPE_STM, phdr->msg_subtype, 0);
                    }
                    else
                    {
                        ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_CES, IPC_TYPE_STM,
                                                phdr->msg_subtype, 0);
                    }
                }

                
            }
            else if (IPC_OPCODE_GET == phdr->opcode)
            {
                //added by liub 2018-9-10 for mib_stm loopback status
                stm_if_loopback_status_get(phdr->msg_index, &lp_status);
                ret = ipc_send_reply(&lp_status, sizeof(lp_status), phdr->sender_id, MODULE_ID_CES, 
                                                   IPC_TYPE_STM, phdr->msg_subtype, phdr->msg_index);
            }
        }
    }

    return ret;
}

/* 处理 ces的 IPC 消息 */
int ces_msg_rcv(struct thread *thread)
{
    static struct ipc_mesg pmesg;

    ces_msg_rcv_msg(&pmesg);

    usleep(10000);
    thread_add_event(ces_master, ces_msg_rcv, NULL, 0);

    return 0;
}
#endif

/* ***********************************************************
 *  Function : ces_msg_send_noack
 *  return   : void
 *  description: send errcode, used to replace "ipc_send_noack"
 * ***********************************************************/
void ces_msg_send_noack(struct ipc_msghdr_n *pReqhdr, uint32_t errcode, unsigned int msg_index)
{
    struct ipc_mesg_n *pMsgRep = NULL;

    pMsgRep = mem_share_malloc(sizeof(struct ipc_msghdr_n) + sizeof(uint32_t), MODULE_ID_CES);
    if(pMsgRep == NULL)
    {
        return;     //if malloc fail, remote can only wait for a timeout
    }

    memset(pMsgRep, 0, sizeof(struct ipc_msghdr_n));

    pMsgRep->msghdr.data_len    = sizeof(uint32_t);
    pMsgRep->msghdr.opcode      = IPC_OPCODE_NACK;
    pMsgRep->msghdr.msg_index   = msg_index;
    pMsgRep->msghdr.data_num    = 1;
    memcpy(pMsgRep->msg_data, &errcode, sizeof(uint32_t));

    if(ipc_send_reply_n1(pReqhdr, pMsgRep, sizeof(struct ipc_msghdr_n) + sizeof(uint32_t)) == -1)
    {
        mem_share_free(pMsgRep, MODULE_ID_CES);
    }

}

/* ***********************************************************
 *  Function : ces_msg_send_hal_wait_ack
 *  return   : 0 ok, -1 error
 *  description: send set msg to hal and wait ack, used to replace "ipc_send_hal_wait_ack"
 * ***********************************************************/
int ces_msg_send_hal_wait_ack(void *pdata, uint32_t data_len, uint16_t data_num, int module_id, int sender_id,
                              enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
    int ret = 0;
    struct ipc_mesg_n* pMsgRep = NULL;

    pMsgRep = ipc_sync_send_n2( pdata , data_len, data_num, module_id, sender_id,
                    msg_type, msg_subtype, opcode, msg_index, 5000);
    if (NULL == pMsgRep)
    {
        printf ( "send sync message fail.\n" );
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

    mem_share_free(pMsgRep, MODULE_ID_CES);

    return ret;

}


/* 接收 IPC 消息 */
int ces_msg_rcv_msg(struct ipc_mesg_n *pmesg, int imlen)
{
    int msg_num = IPC_MSG_LEN / sizeof(struct ces_if);
    //int msg_num = 20;
    struct ipc_msghdr_n *phdr = NULL;
    struct ifm_event *pevent = NULL;
    //struct ces_if *pif = NULL;
    void *pdata = NULL;
    uint32_t ifindex = 0;
    struct ces_if cesif[msg_num];
    int ret = 0;
    uint32_t lp_status = 0;
    int revln;
        
    int base_msg_num = IPC_MSG_LEN/sizeof(struct stm_base_info);
    struct stm_base_info stm_base[base_msg_num];
    
    int rs_msg_num = IPC_MSG_LEN/sizeof(struct stm_rs_info);
    struct stm_rs_info  stm_rs[rs_msg_num];
    
    int ms_msg_num = IPC_MSG_LEN/sizeof(struct stm_ms_info);
    struct stm_ms_info  stm_ms[ms_msg_num];
    
    int hp_msg_num = IPC_MSG_LEN/sizeof(struct stm_hp_info);
    struct stm_hp_info  stm_hp[hp_msg_num];
    
    int msp_msg_num = IPC_MSG_LEN/sizeof(struct stm_msp_config);
    struct stm_msp_config  stm_msp[msp_msg_num];

    int dxc_msg_num = IPC_MSG_LEN/sizeof(struct stm_dxc_config);
    struct stm_dxc_config  stm_dxc[dxc_msg_num];

    revln = (int)pmesg->msghdr.data_len + IPC_HEADER_LEN_N;

    if(revln <= imlen)
    {
        /* 消息处理 */
        phdr = &(pmesg->msghdr);
        pdata = pmesg->msg_data;

        if (IPC_TYPE_IFM == phdr->msg_type)
        {
            if (IPC_OPCODE_EVENT ==phdr->opcode)
            {
                pevent = (struct ifm_event *)pdata;
                ifindex = pevent->ifindex;

                if (IFNET_EVENT_DOWN == phdr->msg_subtype)
                {
                    zlog_debug(0, "%s[%d] recv event notify ifindex 0x%0x down\n", __FUNCTION__, __LINE__, ifindex);
                    ces_if_down(ifindex);
                    stm_if_down(ifindex);
                }
                else if (IFNET_EVENT_UP == phdr->msg_subtype)
                {
                    zlog_debug(0, "%s[%d] recv event notify ifindex 0x%0x up\n", __FUNCTION__, __LINE__, ifindex);
                    ces_if_up(ifindex);
                    stm_if_up(ifindex);
                }
                else if (IFNET_EVENT_IF_DELETE == phdr->msg_subtype)
                {
                    zlog_debug(0, "%s[%d] recv event notify ifindex 0x%0x delete\n", __FUNCTION__, __LINE__, ifindex);
                    //delete函数内部会找ifindex是否存在，这里不需要重复
                    ces_if_delete(ifindex);
                    stm_if_delete(ifindex);
                }
                else if (IFNET_EVENT_IF_HIDE_RECOVER == phdr->msg_subtype)
                {
                    zlog_debug(0, "%s[%d] recv event notify ifindex 0x%0x hide-recover\n", __FUNCTION__, __LINE__, ifindex);
                    ces_if_config_restore(ifindex);
                }
                else if(IFNET_EVENT_IF_ADD == phdr->msg_subtype)
                {
                    zlog_debug (0,"%s[%d] recv add event notify ifindex 0x%0x add\n", __FUNCTION__, __LINE__, ifindex );                    
                    if (IFM_TYPE_IS_STM(ifindex))
                    {
                        stm_if_init();  //初始化创建stm接口
                    }
                }
            }
        }
        else if (IPC_TYPE_CES == phdr->msg_type)
        {
            if (IPC_OPCODE_UPDATE == phdr->opcode)
            {
                if(phdr->msg_subtype == 11)
                {
                    ces_if_sc01q_alarm_set((struct ces_sc01q_alarm_info *)pdata, phdr->msg_index);
                }
                else
                {
                    ifindex = phdr->msg_index;
                    ces_if_alarm_set(* (uint8_t *) pdata, ifindex, phdr->msg_subtype);
                }
            }
            else if (IPC_OPCODE_GET_BULK == phdr->opcode)
            {
                //ifindex = phdr->msg_index;
                ifindex = *(uint32_t *)pdata;
                memset(cesif, 0, msg_num * sizeof(struct ces_if));
                ret = ces_if_bulk_get(ifindex, cesif);
                if (ret > 0)
                {
                    ret = ipc_send_reply_n2(cesif, ret * sizeof(struct ces_if), ret, phdr->sender_id,
                                            MODULE_ID_CES, IPC_TYPE_CES, phdr->msg_subtype,  phdr->sequence, ifindex, IPC_OPCODE_REPLY);
                }
                else
                {                
                    ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id,
                                            MODULE_ID_CES, IPC_TYPE_CES, phdr->msg_subtype,  phdr->sequence, ifindex, IPC_OPCODE_NACK);
                }
            }
        }
        else if(IPC_TYPE_STM == phdr->msg_type)
        {
            if(IPC_OPCODE_UPDATE == phdr->opcode)
            {
                ifindex = phdr->msg_index;
                /*modified by liub 2018-10-18, release msg_subtype*/
                if(phdr->msg_subtype == STM_INFO_ALARM)
                {
                    stm_if_alarm_set((struct stm_alarm_info *)pdata, ifindex);
                }
                else if(phdr->msg_subtype == STM_INFO_STATISTICS)
                {
                    stm_if_statis_update((struct stm_statis_info *)pdata, ifindex);
                }
                else if(phdr->msg_subtype == STM_INFO_DXC_STATUS_GET) 
                {
                    stm_dxc_status_update(ifindex, * (uint8_t *) pdata);
                }
                else if(phdr->msg_subtype == STM_INFO_IMAGE_SWITCH) 
                {
                    stm_fpga_image_type_set(ifindex, * (uint8_t *)pdata);
                }
            }
            else if (IPC_OPCODE_GET_BULK == phdr->opcode)
            {
                //ifindex = phdr->msg_index;
                if(phdr->msg_subtype == STM_SNMP_TYPE_BASE_GET)
                {
                    ifindex = *(uint32_t *)pdata;
                    memset(stm_base, 0, base_msg_num*sizeof(struct stm_base_info));
                    ret  = stm_if_snmp_base_bulk_get(ifindex, stm_base);
                    if (ret > 0)
                    {
                        ret = ipc_send_reply_n2(stm_base, ret * sizeof(struct stm_base_info), ret, phdr->sender_id,
                                            MODULE_ID_CES, IPC_TYPE_STM, phdr->msg_subtype,  phdr->sequence, ifindex, IPC_OPCODE_REPLY);
                    }
                    else
                    {
                        ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id,
                                            MODULE_ID_CES, IPC_TYPE_STM, phdr->msg_subtype,  phdr->sequence, ifindex, IPC_OPCODE_NACK);
                    }
                }
                else if(phdr->msg_subtype == STM_SNMP_TYPE_RS_GET)
                {
                    ifindex = *(uint32_t *)pdata;
                    memset(stm_rs, 0, rs_msg_num*sizeof(struct stm_rs_info));
                    ret  = stm_if_snmp_rs_bulk_get(ifindex, stm_rs);
                    if (ret > 0)
                    {
                        ret = ipc_send_reply_n2(stm_rs, ret * sizeof(struct stm_rs_info), ret, phdr->sender_id,
                                            MODULE_ID_CES, IPC_TYPE_STM, phdr->msg_subtype,  phdr->sequence, ifindex, IPC_OPCODE_REPLY);
                    }
                    else
                    {
                        ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id,
                                            MODULE_ID_CES, IPC_TYPE_STM, phdr->msg_subtype,  phdr->sequence, ifindex, IPC_OPCODE_NACK);
                    }
                }
                else if(phdr->msg_subtype == STM_SNMP_TYPE_MS_GET)
                {
                    ifindex = *(uint32_t *)pdata;
                    memset(stm_ms, 0, ms_msg_num*sizeof(struct stm_ms_info));
                    ret  = stm_if_snmp_ms_bulk_get(ifindex, stm_ms);
                    if (ret > 0)
                    {
                        ret = ipc_send_reply_n2(stm_ms, ret * sizeof(struct stm_ms_info), ret, phdr->sender_id,
                                            MODULE_ID_CES, IPC_TYPE_STM, phdr->msg_subtype,  phdr->sequence, ifindex, IPC_OPCODE_REPLY);
                    }
                    else
                    {
                        ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id,
                                            MODULE_ID_CES, IPC_TYPE_STM, phdr->msg_subtype,  phdr->sequence, ifindex, IPC_OPCODE_NACK);
                    }
                }
                else if(phdr->msg_subtype == STM_SNMP_TYPE_HP_GET)
                {
                    memset(stm_hp, 0, hp_msg_num*sizeof(struct stm_hp_info));
                    ret  = stm_if_snmp_hp_bulk_get(pdata, stm_hp);
                    if (ret > 0)
                    {
                        ret = ipc_send_reply_n2(stm_hp, ret * sizeof(struct stm_hp_info), ret, phdr->sender_id,
                                            MODULE_ID_CES, IPC_TYPE_STM, phdr->msg_subtype,  phdr->sequence, ifindex, IPC_OPCODE_REPLY);
                    }
                    else
                    {
                        ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id,
                                            MODULE_ID_CES, IPC_TYPE_STM, phdr->msg_subtype,  phdr->sequence, ifindex, IPC_OPCODE_NACK);
                    }
                }
                else if(phdr->msg_subtype == STM_SNMP_TYPE_MSP_GET)
                {
                    ifindex = *(uint32_t *)pdata;
                    memset(stm_msp, 0, msp_msg_num*sizeof(struct stm_msp_config));
                    ret  = stm_if_snmp_msp_bulk_get(ifindex, stm_msp);
                    if (ret > 0)
                    {
                        ret = ipc_send_reply_n2(stm_msp, ret * sizeof(struct stm_msp_config), ret, phdr->sender_id,
                                            MODULE_ID_CES, IPC_TYPE_STM, phdr->msg_subtype,  phdr->sequence, ifindex, IPC_OPCODE_REPLY);
                    }
                    else
                    {
                        ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id,
                                            MODULE_ID_CES, IPC_TYPE_STM, phdr->msg_subtype,  phdr->sequence, ifindex, IPC_OPCODE_NACK);

                    }
                }
                else if(phdr->msg_subtype == STM_SNMP_TYPE_SNCP_GET)
                {
                    memset(stm_dxc, 0, dxc_msg_num*sizeof(struct stm_dxc_config));
                    ret  = stm_if_snmp_dxc_bulk_get(pdata, phdr->data_len, stm_dxc);
                    if (ret > 0)
                    {
                        ret = ipc_send_reply_n2(stm_dxc, ret * sizeof(struct stm_dxc_config), ret, phdr->sender_id,
                                            MODULE_ID_CES, IPC_TYPE_STM, phdr->msg_subtype,  phdr->sequence, ifindex, IPC_OPCODE_REPLY);
                    }
                    else
                    {
                        ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id,
                                            MODULE_ID_CES, IPC_TYPE_STM, phdr->msg_subtype,  phdr->sequence, ifindex, IPC_OPCODE_NACK);
                    }
                }

                
            }
            else if (IPC_OPCODE_GET == phdr->opcode)
            {
                //added by liub 2018-9-10 for mib_stm loopback status
                stm_if_loopback_status_get(phdr->msg_index, &lp_status);
                ret = ipc_send_reply_n2(&lp_status, sizeof(lp_status), 1, phdr->sender_id,
                                    MODULE_ID_CES, IPC_TYPE_STM, phdr->msg_subtype,  phdr->sequence, ifindex, IPC_OPCODE_REPLY);
            }
        }
    }
    
    mem_share_free(pmesg, MODULE_ID_CES);

    return ret;
}
