#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <lib/thread.h>
#include <lib/msg_ipc.h>
#include <lib/zassert.h>
#include <lib/devm_com.h>
#include <lib/memtypes.h>
#include <lib/memory.h>
#include <lib/linklist.h>
#include <lib/errcode.h>
#include <lib/devm_com.h>
#include <lib/log.h>
#include <syslog/syslogd.h>
#include "devm.h"
#include "devm_ha.h"
#include <lib/memtypes.h>
#include <lib/memshare.h>
#include <lib/msg_ipc_n.h>
#include <lib/hptimer.h>
#include <lib/alarm.h>
#include <lib/gpnSocket/socketComm/gpnGlobalPortIndexDef.h>


int gPwrSlot1 = 0, gPwrSlot2 = 0, gFanSlot = 0;
uint8_t powerAlmFlg = 0;
extern int syslog_debug;
extern unsigned char sys_version[SLOT_NUM_MAX][NAME_STRING_LEN];

extern enum HA_BOOTSTATE myboard_dev_start_status;
extern enum HA_BOOTSTATE otherboard_dev_start_status;
extern enum HA_ROLE ha_dev_ha_role;
extern unsigned char sys_objectid[64];
extern unsigned char a_ha_bootstate[][25];
extern unsigned char a_ha_dev_ha_role[][25];
extern unsigned int devm_ha_app_init_count[HA_MODULE_ID_MAX];

extern unsigned char myslot;
extern unsigned char partnerslot;


static struct devm_backplane backplaneeinfo = {0,0,0};

/* 发消息给 hal，多个线程调用，需要加锁 */
int devm_msg_send_hal(void *pdata, struct ipc_msghdr_n *phdr)
{
//  uint32_t index = 0;

//  index |= device.myunit <<8;
//  index |= device.myslot;

    return ipc_send_msg_n2(pdata, phdr->data_len, 1, MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM,
                           phdr->msg_subtype, phdr->opcode, phdr->msg_index);
}


/* 处理 dem get 消息 */
static void devm_msg_get(struct ipc_mesg_n *pmesg)
{
    int ret = 0;
    uint8_t unit = 0;
    uint8_t slot = 0;
    struct devm_slot *pslot = NULL;
    struct devm_unit *punit = NULL;
    struct devm_unit unit_send;
    struct devm_version version;
    struct devm_box *pbox = NULL;
    struct devm t_devm;
    struct ipc_msghdr_n *phdr = &pmesg->msghdr;
    uint32_t index = phdr->msg_index;

    unit = (index >> 8) & 0xff;
    slot = (index & 0xff);

    switch (phdr->msg_subtype)
    {
        case DEVM_INFO_BOX:/*收到获取 BOX 消息*/
            if (DEV_TYPE_BOX != device.type)
            {
                pbox = NULL;
                zlog_err("%s[%d]:leave %s,error, box equ,failed to get box info\n", __FILE__, __LINE__, __func__);
            }
            else
            {
                pbox = device.pbox;
            }

            if (NULL != pbox)
            {
//                ipc_send_reply(pbox, sizeof(struct devm_box), phdr->sender_id, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_BOX, index);
                ipc_send_reply_n2(pbox, sizeof(struct devm_box), 1, phdr->sender_id, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_BOX, 0, index, IPC_OPCODE_REPLY);

                if (ret < 0)
                {
                    zlog_err("%s[%d]:leave %s,devm failed to send box msg to module(%d),index = 0x%x\n", __FILE__, __LINE__, __func__, phdr->sender_id, index);
                }
                else
                {
                    zlog_debug(DEVM_DBG_MSG, "%s[%d]:%s: sucessfully send box info msg to module(%d),index = 0x%x .\n", __FILE__, __LINE__, __func__, phdr->sender_id, index);
                }
            }
            else
            {
                zlog_err("%s[%d]:leave %s,devm_get_box error,box == NULL,send noack to module(%d),index = 0x%x\n", __FILE__, __LINE__, __func__, phdr->sender_id, index);
//                ipc_send_noack(1, phdr->sender_id, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_BOX, index);
                ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_BOX, 0, index, IPC_OPCODE_NACK);                                   
            }

            break;

        case DEVM_INFO_UNIT:/*收到获取 UNIT 消息*/
            if (DEV_TYPE_BOX == device.type)
            {
                punit = NULL;
                zlog_err("%s[%d]:leave %s,box equ, devm get unit(%d) error\n", __FILE__, __LINE__, __func__, unit);
            }
            else if (DEV_TYPE_SHELF == device.type)
            {
                punit = devm_get_unit(unit);

                if (NULL == punit)
                {
                    zlog_err("%s[%d]:leave %s,error ,shelf equ ,but devm_get_unit return NULL, failed to get unit(%d)\n", __FILE__, __LINE__, __func__, unit);
                }
            }
            else
            {
                punit = NULL;
                zlog_err("%s[%d]:leave %s,not box or shelf  equ, devm get unit(%d) error\n", __FILE__, __LINE__, __func__, unit);
            }

            if (NULL != punit)
            {
                memset(&unit_send, 0, sizeof(struct devm_unit));
                unit_send.slot_num = punit->slot_num;
                unit_send.slot_main_board = punit->slot_main_board;
                unit_send.slot_slave_board = punit->slot_slave_board;
                unit_send.myslot = punit->myslot;
                unit_send.slot_ha = punit->slot_ha;
                unit_send.heart_beat = punit->heart_beat;
                unit_send.watchdog = punit->watchdog;
                memcpy(&unit_send.bplane, &punit->bplane, sizeof(struct devm_backplane));
                memcpy(&unit_send.fan, &punit->fan, sizeof(struct devm_fan));
                memcpy(&unit_send.power[0], &punit->power[0], sizeof(struct devm_power));
                memcpy(&unit_send.power[1], &punit->power[1], sizeof(struct devm_power));

//                ipc_send_reply(&unit_send, sizeof(struct devm_unit), phdr->sender_id, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_UNIT, index);
                ipc_send_reply_n2(&unit_send, sizeof(struct devm_unit), 1, phdr->sender_id, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_UNIT, 0, index, IPC_OPCODE_REPLY);

                if (ret < 0)
                {
                    zlog_err("%s[%d]:leave %s,devm failed to send msg unit(%d)\n", __FILE__, __LINE__, __func__, unit);
                }
                else
                {
                    zlog_debug(DEVM_DBG_MSG, "%s[%d]:%s: sucessfully send unit(%d) info msg\n", __FILE__, __LINE__, __func__, unit);
                }
            }
            else
            {
                zlog_err("%s[%d]:leave %s, error , NULL == punit, can not send unit(%d), send noack info to module(%d),index = 0x%x \n", __FILE__, __LINE__, __func__, unit, phdr->sender_id, index);
//                ipc_send_noack(1, phdr->sender_id, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_UNIT, index);
                ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_UNIT, 0, index, IPC_OPCODE_NACK);                                   
                
            }

            break;

        case DEVM_INFO_SLOT:/*收到获取 SLOT 消息*/
            if (DEV_TYPE_BOX == device.type)
            {
                pslot = NULL;
                zlog_err("%s[%d]:leave %s,error ,box equ can not get slot(%d,%d) \n", __FILE__, __LINE__, __func__, unit, slot);
            }
            else if (DEV_TYPE_SHELF == device.type)
            {
                pslot = (struct devm_slot *)devm_get_slot(unit, slot);

                if (NULL == pslot)
                {
                    zlog_err("%s[%d]:leave %s,error ,shelf equ,but fun devm_get_slot return NULL, fail to get slot(%d,%d)\n", __FILE__, __LINE__, __func__, unit, slot);
                }
            }
            else
            {
                pslot = NULL;
                zlog_err("%s[%d]:leave %s,error ,not box or shelf equ, can not get slot(%d,%d) \n", __FILE__, __LINE__, __func__, unit, slot);
            }

            if (NULL != pslot)
            {
//                ipc_send_reply(pslot, sizeof(struct devm_slot), phdr->sender_id, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_SLOT, index);
                ipc_send_reply_n2(pslot, sizeof(struct devm_slot), 1, phdr->sender_id, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_SLOT, 0, index, IPC_OPCODE_REPLY);

                if (ret < 0)
                {
                    zlog_err("%s[%d]:leave %s,devm failed to send slot msg to module(%d),slot(%d,%d),index = %d\n", __FILE__, __LINE__, __func__, phdr->sender_id, unit, slot, index);
                }
                else
                {
                    zlog_debug(DEVM_DBG_MSG, "%s[%d]:%s: sucessfully send slot info msg to module (%d) ,unit=%d,slotid=%d.index = 0x%x\n", __FILE__, __LINE__, __func__, phdr->sender_id, unit, slot, index);
                }
            }
            else
            {                
                zlog_err("%s[%d]:leave %s, error,pslot == NULL, can not send slot( %d, %d), send noack to module (%d),index = 0x%x\n", __FILE__, __LINE__, __func__, unit, slot, phdr->sender_id, index);

                ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_SLOT, 0, index, IPC_OPCODE_NACK);                                                   
            }

            break;

        case DEVM_INFO_STRUCT_DEVM:/*get  struct devm device*/
            memcpy(&t_devm, &device, sizeof(struct devm));
            t_devm.pbox = NULL;
            t_devm.pmyunit = NULL;
            memset(t_devm.punit , 0, sizeof(struct devm_unit *)*UNIT_NUM_MAX);

//            ret = ipc_send_reply(&t_devm, sizeof(struct devm), phdr->sender_id, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_STRUCT_DEVM, index);
            ret = ipc_send_reply_n2(&t_devm, sizeof(struct devm), 1, phdr->sender_id, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_STRUCT_DEVM, 0, index, IPC_OPCODE_REPLY);

            if (ret < 0)
            {
                zlog_err("%s[%d]:leave %s,devm send struct devm msg error to module(%d),index = 0x%x\n", __FILE__, __LINE__, __func__, phdr->sender_id, index);
            }
            else
            {
                zlog_debug(DEVM_DBG_MSG, "%s[%d]:%s: sucessfully send struct devm device msg to module (%d),index = 0x%x\n", __FILE__, __LINE__, __func__, phdr->sender_id, index);
            }

            break;

        case DEVM_INFO_SYS_OBJECTID:/*收到获取 BOX 消息*/
//            ipc_send_reply(sys_objectid, sizeof(sys_objectid), phdr->sender_id, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_SYS_OBJECTID, index);
            ipc_send_reply_n2(sys_objectid, sizeof(sys_objectid), 1, phdr->sender_id, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_SYS_OBJECTID, 0, index, IPC_OPCODE_REPLY);

            if (ret < 0)
            {
                zlog_err("%s[%d]:leave %s,devm failed to send sys_objectid msg to module(%d),index = 0x%x\n", __FILE__, __LINE__, __func__, phdr->sender_id, index);
            }
            else
            {
                zlog_debug(DEVM_DBG_MSG, "%s[%d]:%s: sucessfully send sys_objectid msg to module(%d),index = 0x%x\n", __FILE__, __LINE__, __func__, phdr->sender_id, index);
            }

            break;

        case DEVM_INFO_SYS_VERSION:
             printf("%s device.myslot = %d, device.myunit = %d, pmesg->msghdr.srcslot = %d\r\n", __func__ , device.myslot, device.myunit, pmesg->msghdr.srcslot);
         
            if(pmesg->msghdr.srcslot != device.myslot)
            {
                index = (device.myunit << 8) | (device.myslot << 0);
#if 0
                ret = ipc_send_ha(sys_version[device.myslot - 1], sizeof(sys_version[slot - 1]), 1, MODULE_ID_DEVM,
                                  IPC_TYPE_DEVM, DEVM_INFO_SYS_VERSION, IPC_OPCODE_UPDATE , index);
#else
                ret = ipc_send_msg_ha_n2(sys_version[device.myslot - 1], NAME_STRING_LEN, 1, MODULE_ID_DEVM,  
                                    MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_SYS_VERSION, IPC_OPCODE_UPDATE, index, 0);
#endif
                if (ret)
                {
                    zlog_err("%s[%d]:leave %s:error:fail to send msg to send slot %d sys_ver to  slot %d!\n", __FILE__, __LINE__, __func__ , device.myslot, slot);
                }
                else
                {
                    zlog_debug(DEVM_DBG_MSG, "%s[%d]:leave %s:sucessfully to send msg to send slot %d,sys_ver(%s) ,data len = %d!\n", __FILE__, __LINE__, __func__ , slot, sys_version[slot - 1], sizeof(sys_version[device.myslot - 1]));
                }
            }

            break;

        case DEVM_INFO_PARTNERSLOT:
//            ret = ipc_send_reply(&partnerslot, sizeof(partnerslot), phdr->sender_id, MODULE_ID_DEVM, IPC_TYPE_DEVM, phdr->msg_subtype, index);
              ret = ipc_send_reply_n2(&partnerslot, sizeof(partnerslot), 1, phdr->sender_id, MODULE_ID_DEVM, IPC_TYPE_DEVM, phdr->msg_subtype, 0, index, IPC_OPCODE_REPLY);
  
              if (ret < 0)
              {
                  zlog_err("%s[%d]:leave %s,devm failed to send partnerslot(%d) msg to module(%d),index = 0x%x\n", __FILE__, __LINE__, __func__, partnerslot, phdr->sender_id, index);
              }
              else
              {
                  zlog_debug(DEVM_DBG_MSG, "%s[%d]:%s: sucessfully send partnerslot(%d) msg to module(%d),index = 0x%x\n", __FILE__, __LINE__, __func__, partnerslot, phdr->sender_id, index);
              }
              break;

         case DEVM_INFO_E2DATA_HAGET:
            
              if(MODULE_ID_DEVM == pmesg->msghdr.sender_id)
              {
                   punit = NULL;
                    
                   if(DEV_TYPE_SHELF == device.type) punit = devm_get_unit(device.myunit);
               
                   if(NULL != punit)
                   {
                        if(punit->bplane.id != 0)
                        {
                            ipc_send_msg_ha_n2(&punit->bplane, sizeof(struct devm_backplane), 1, MODULE_ID_DEVM, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_E2DATA_HAGET, IPC_OPCODE_ADD, 0, 0);

                            printf("%s[%d]:%s: reply other cpu devm get e2 data sucessfully %d\n", __FILE__, __LINE__, __func__, punit->bplane.id);
                        }
                  }
              }

              break;

         case DEVM_INFO_EEPROM_INFO:
            
              if(MODULE_ID_HAL == pmesg->msghdr.sender_id)
              {
                  if(backplaneeinfo.id != 0)
                  {
                      ipc_send_reply_n2(&backplaneeinfo, sizeof(backplaneeinfo), 1, MODULE_ID_HAL, MODULE_ID_DEVM, 
                                        IPC_TYPE_DEVM, DEVM_INFO_EEPROM_INFO, 0, 0, IPC_OPCODE_REPLY);
                      printf("%s[%d]:%s: reply hal get e2 data sucessfully %d\n", __FILE__, __LINE__, __func__, backplaneeinfo.id);
                  }
                  else
                  {
                      ipc_send_reply_n2(NULL, 0, 0, MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_EEPROM_INFO, 0, 0, IPC_OPCODE_NACK);
                                            /*request form main board*/
                      ipc_send_msg_ha_n2(NULL, 0, 0, MODULE_ID_DEVM, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_E2DATA_HAGET, IPC_OPCODE_GET, 0, 0);
                  }
              }
   
              break;

          case DEVM_INFO_REBOOT_COUNT:
               {
                    struct ha_com *t_ha_com = NULL;
               
                    t_ha_com = (struct ha_com *)pmesg->msg_data;
                 
                    if (0 == t_ha_com->module_id || t_ha_com->module_id > HA_MODULE_ID_MAX)
                    {
                         ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, MODULE_ID_DEVM, phdr->msg_type, phdr->msg_subtype, 0, 0, IPC_OPCODE_NACK);                                   
               
                         if (ret < 0)
                         {
                             zlog_err("%s[%d]:leave %s:error: ha failed to send  noack msg to module %d to reply app init get msg, since module id of data is 0, is out of range(%d, %d), failed to write into the code\n"
                                      , __FILE__, __LINE__, __func__, phdr->sender_id, 1, HA_MODULE_ID_MAX);
                         }
                         else
                         {
                             zlog_debug(DEVM_DBG_MSG, "%s[%d]:leave %s,ha sucessfully to send  noack msg to module %d to reply app init get msg, since module id of data is 0,  is out of range(%d, %d), failed to write into the code\n"
                                        , __FILE__, __LINE__, __func__, phdr->sender_id, 1, HA_MODULE_ID_MAX);
                         }
                    }
                    else
                    {
                         devm_ha_app_init_count[t_ha_com->module_id]++;
                         
                         t_ha_com->module_init_count = devm_ha_app_init_count[t_ha_com->module_id];
               
                         ret = ipc_send_reply_n2(t_ha_com, sizeof(struct ha_com), 1, phdr->sender_id, MODULE_ID_DEVM, phdr->msg_type, phdr->msg_subtype, 0, 0, IPC_OPCODE_REPLY);

               
                         if (ret < 0)
                         {
                             zlog_err("%s[%d]:leave %s,ha failed to send  module(%d) init count(%d) msg reply to module %d\n", __FILE__, __LINE__, __func__
                                      , t_ha_com->module_id, t_ha_com->module_init_count, phdr->sender_id);
                         }
                         else
                         {
                             zlog_debug(DEVM_DBG_MSG, "%s[%d]:leave %s,ha sucessfully to send  module(%d) init count(%d) msg reply to module %d\n", __FILE__, __LINE__, __func__
                                        , t_ha_com->module_id, t_ha_com->module_init_count, phdr->sender_id);
                         }
                    }
                    break;
                }        
              
        default:
            break;
    }
}


static void devm_send_init_ha_to_otherboard(void)
{
    int ret = 0;
    struct devm_com t_devm_com;

    if (NULL != device.pmyunit)
    {
        memset(&t_devm_com, 0, sizeof(struct devm_com));
        t_devm_com.unit = device.main_unit;
        t_devm_com.slot = device.myslot;
        t_devm_com.main_slot = device.pmyunit->slot_main_board;
        t_devm_com.slave_slot = device.pmyunit->slot_slave_board;
        t_devm_com.type = device.slot_type;

        if (NULL != device.pmyunit->pslot[device.myslot - 1])
        {
            t_devm_com.status = device.pmyunit->pslot[device.myslot - 1]->type;
            memcpy(t_devm_com.mac,  device.pmyunit->pslot[device.myslot - 1]->mac, sizeof(t_devm_com.mac));
        }
        else
        {
            t_devm_com.status = DEV_STATUS_UNKOWN;
        }

        t_devm_com.ha_role = ha_dev_ha_role;
        t_devm_com.ha_status = myboard_dev_start_status;

        ret = ipc_send_msg_ha_n2(&t_devm_com, sizeof(struct devm_com), 1, MODULE_ID_DEVM,  
                                       MODULE_ID_DEVM, IPC_TYPE_HA, HA_SMSG_BOOT_STATE, IPC_OPCODE_EVENT, 0, 0);

        if (ret)
        {
            zlog_err("%s[%d]:leave %s:error:fail to send ha status chang(->%s) to otherboard!\n", __FILE__, __LINE__, __func__ , a_ha_bootstate[t_devm_com.ha_status]);
        }
        else
        {
            zlog_debug(DEVM_DBG_MSG, "%s[%d]:leave %s:sucessfully to send ha status chang(->%s) to otherboard!\n", __FILE__, __LINE__, __func__ , a_ha_bootstate[t_devm_com.ha_status]);
        }
#if 0        

        if (HA_BOOTSTA_START == myboard_dev_start_status)
        {
#if 0
            ret = ipc_send_ha(&t_devm_com, sizeof(struct devm_com), 1, MODULE_ID_HA, IPC_TYPE_HA, HA_SMSG_BOOT_STATE, IPC_OPCODE_EVENT, 0);
#else
            ret = ipc_send_msg_ha_n2(&t_devm_com, sizeof(struct devm_com), 1, MODULE_ID_HA,  
                                           MODULE_ID_HA, IPC_TYPE_HA, HA_SMSG_BOOT_STATE, IPC_OPCODE_EVENT, 0, 0);
#endif

#if 0
            ret = ipc_send_slot_byha(&t_devm_com, sizeof(struct devm_com), 1, MODULE_ID_HA, IPC_TYPE_HA, HA_SMSG_BOOT_STATE
                                     , IPC_OPCODE_EVENT, 0, partnerslot, 0);
#endif
            if (ret)
            {
                zlog_err("%s[%d]:leave %s:error:fail to send ha status chang(->%s) to otherboard!\n", __FILE__, __LINE__, __func__ , a_ha_bootstate[t_devm_com.ha_status]);
            }
            else
            {
                zlog_debug(DEVM_DBG_MSG, "%s[%d]:leave %s:sucessfully to send ha status chang(->%s) to otherboard!\n", __FILE__, __LINE__, __func__ , a_ha_bootstate[t_devm_com.ha_status]);
            }
        }
        else if (HA_BOOTSTA_END == myboard_dev_start_status)
        {
#if 0
            ret = ipc_send_ha(&t_devm_com, sizeof(struct devm_com), 1, MODULE_ID_HA, IPC_TYPE_HA, HA_SMSG_BOOT_STATE, IPC_OPCODE_EVENT, 0);
#else
            ret = ipc_send_msg_ha_n2(&t_devm_com, sizeof(struct devm_com), 1, MODULE_ID_HA,  
                                  MODULE_ID_HA, IPC_TYPE_HA, HA_SMSG_BOOT_STATE, IPC_OPCODE_EVENT, 0, 0);
#endif

#if 0
            ret = ipc_send_slot_byha(&t_devm_com, sizeof(struct devm_com), 1, MODULE_ID_HA, IPC_TYPE_HA, HA_SMSG_BOOT_STATE
                                     , IPC_OPCODE_EVENT, 0, partnerslot, 0);
#endif
            if (ret)
            {
                zlog_err("%s[%d]:leave %s:error:fail to send ha status chang(->%s) to otherboard!\n", __FILE__, __LINE__, __func__ , a_ha_bootstate[t_devm_com.ha_status]);
            }
            else
            {
                zlog_debug(DEVM_DBG_MSG, "%s[%d]:leave %s:sucessfully to send ha status chang(->%s) to otherboard!\n", __FILE__, __LINE__, __func__ , a_ha_bootstate[t_devm_com.ha_status]);
            }

            usleep(300000);

            t_devm_com.ha_status = HA_BOOTSTA_END;
#if 0
            ret = ipc_send_ha(&t_devm_com, sizeof(struct devm_com), 1, MODULE_ID_HA, IPC_TYPE_HA, HA_SMSG_BOOT_STATE, IPC_OPCODE_EVENT, 0);
#else
            ret = ipc_send_msg_ha_n2(&t_devm_com, sizeof(struct devm_com), 1, MODULE_ID_HA,  
                                              MODULE_ID_HA, IPC_TYPE_HA, HA_SMSG_BOOT_STATE, IPC_OPCODE_EVENT, 0, 0);
#endif

#if 0
            ret = ipc_send_slot_byha(&t_devm_com, sizeof(struct devm_com), 1, MODULE_ID_HA, IPC_TYPE_HA, HA_SMSG_BOOT_STATE
                                     , IPC_OPCODE_EVENT, 0, partnerslot, 0);
#endif
            if (ret)
            {
                zlog_err("%s[%d]:leave %s:error:fail to send ha status chang(->%s) to otherboard!\n", __FILE__, __LINE__, __func__ , a_ha_bootstate[t_devm_com.ha_status]);
            }
            else
            {
                zlog_debug(DEVM_DBG_MSG, "%s[%d]:leave %s:sucessfully to send ha status chang(->%s) to otherboard!\n", __FILE__, __LINE__, __func__ , a_ha_bootstate[t_devm_com.ha_status]);
            }
        }
        else if (HA_BOOTSTA_DATAOK == myboard_dev_start_status)
        {
#if 0
            ret = ipc_send_ha(&t_devm_com, sizeof(struct devm_com), 1, MODULE_ID_HA, IPC_TYPE_HA, HA_SMSG_BOOT_STATE, IPC_OPCODE_EVENT, 0);
#else
            ret = ipc_send_msg_ha_n2(&t_devm_com, sizeof(struct devm_com), 1, MODULE_ID_HA,  
                                                          MODULE_ID_HA, IPC_TYPE_HA, HA_SMSG_BOOT_STATE, IPC_OPCODE_EVENT, 0, 0);
#endif

#if 0
            ret = ipc_send_slot_byha(&t_devm_com, sizeof(struct devm_com), 1, MODULE_ID_HA, IPC_TYPE_HA, HA_SMSG_BOOT_STATE
                                     , IPC_OPCODE_EVENT, 0, partnerslot, 0);
#endif
            if (ret)
            {
                zlog_err("%s[%d]:leave %s:error:fail to send ha status chang(->%s) to otherboard!\n", __FILE__, __LINE__, __func__ , a_ha_bootstate[t_devm_com.ha_status]);
            }
            else
            {
                zlog_debug(DEVM_DBG_MSG, "%s[%d]:leave %s:sucessfully to send ha status chang(->%s) to otherboard!\n", __FILE__, __LINE__, __func__ , a_ha_bootstate[t_devm_com.ha_status]);
            }

            usleep(300000);

            t_devm_com.ha_status = HA_BOOTSTA_END;
#if 0
            ret = ipc_send_ha(&t_devm_com, sizeof(struct devm_com), 1, MODULE_ID_HA, IPC_TYPE_HA, HA_SMSG_BOOT_STATE, IPC_OPCODE_EVENT, 0);
#else
            ret = ipc_send_msg_ha_n2(&t_devm_com, sizeof(struct devm_com), 1, MODULE_ID_HA,  
                                                          MODULE_ID_HA, IPC_TYPE_HA, HA_SMSG_BOOT_STATE, IPC_OPCODE_EVENT, 0, 0);
#endif
#if 0
            ret = ipc_send_slot_byha(&t_devm_com, sizeof(struct devm_com), 1, MODULE_ID_HA, IPC_TYPE_HA, HA_SMSG_BOOT_STATE
                                     , IPC_OPCODE_EVENT, 0, partnerslot, 0);
#endif
            if (ret)
            {
                zlog_err("%s[%d]:leave %s:error:fail to send ha status chang(->%s) to otherboard!\n", __FILE__, __LINE__, __func__ , a_ha_bootstate[t_devm_com.ha_status]);
            }
            else
            {
                zlog_debug(DEVM_DBG_MSG, "%s[%d]:leave %s:sucessfully to send ha status chang(->%s) to otherboard!\n", __FILE__, __LINE__, __func__ , a_ha_bootstate[t_devm_com.ha_status]);
            }

            usleep(300000);

            t_devm_com.ha_status = HA_BOOTSTA_DATAOK;
#if 0
            ret = ipc_send_ha(&t_devm_com, sizeof(struct devm_com), 1, MODULE_ID_HA, IPC_TYPE_HA, HA_SMSG_BOOT_STATE, IPC_OPCODE_EVENT, 0);
#else
            ret = ipc_send_msg_ha_n2(&t_devm_com, sizeof(struct devm_com), 1, MODULE_ID_HA,  
                                                                      MODULE_ID_HA, IPC_TYPE_HA, HA_SMSG_BOOT_STATE, IPC_OPCODE_EVENT, 0, 0);
#endif
#if 0
            ret = ipc_send_slot_byha(&t_devm_com, sizeof(struct devm_com), 1, MODULE_ID_HA, IPC_TYPE_HA, HA_SMSG_BOOT_STATE
                                     , IPC_OPCODE_EVENT, 0, partnerslot, 0);
#endif
            if (ret)
            {
                zlog_err("%s[%d]:leave %s:error:fail to send ha status chang(->%s) to otherboard!\n", __FILE__, __LINE__, __func__ , a_ha_bootstate[t_devm_com.ha_status]);
            }
            else
            {
                zlog_debug(DEVM_DBG_MSG, "%s[%d]:leave %s:sucessfully to send ha status chang(->%s) to otherboard!\n", __FILE__, __LINE__, __func__ , a_ha_bootstate[t_devm_com.ha_status]);
            }
        }
#endif        
        
    }
}


static void devm_msg_add(struct ipc_mesg_n *mesg)
{
    int ret = 0;
    uint8_t unit = 0;
    uint8_t slot = 0;
    struct ipc_msghdr_n *phdr = &mesg->msghdr;
    struct devm_port *p_devm_port = NULL;

    unit = (uint8_t)((phdr->msg_index & 0xff00) >> 8);
    slot = (uint8_t)(phdr->msg_index & 0xff);

    if (phdr->msg_subtype == DEVM_INFO_BOX)  /* 创建盒式设备 */
    {
        struct devm_com t_devm_com;

        devm_box_create((struct devm_box *)mesg->msg_data);

        device.slot_type = SLOT_TYPE_BOARD_MAIN;
        device.myslot = 1;
        ha_dev_ha_role = HA_ROLE_MASTER;

        devm_get_slot_app_version(sys_version[device.myslot - 1],  device.myslot);

        /*send master myboard_dev_start_status msg */
        memset(&t_devm_com, 0, sizeof(struct devm_com));
        t_devm_com.unit = 1;
        t_devm_com.slot = 1;
        t_devm_com.main_slot = 1;
        t_devm_com.slave_slot = 0;
        t_devm_com.type = SLOT_TYPE_BOARD_MAIN;
        t_devm_com.status = DEV_STATUS_WORK;
        t_devm_com.ha_role = ha_dev_ha_role;
        t_devm_com.ha_status = myboard_dev_start_status;
        memcpy(t_devm_com.mac,  device.pbox->mac, sizeof(device.pbox->mac));

        devm_event_notify(DEV_EVENT_HA_BOOTSTATE_CHANGE, &t_devm_com);

        /*通知文件管理系统*/
        ipc_send_msg_n2( &t_devm_com, sizeof(struct devm_com), 1, MODULE_ID_FILE,  MODULE_ID_DEVM,  IPC_TYPE_DEVM, DEV_EVENT_HA_BOOTSTATE_CHANGE, IPC_OPCODE_EVENT, 0);

    }
    else if (phdr->msg_subtype == DEVM_INFO_UNIT)  /* Unit 创建 */
    {
        if (unit > 0 && device.punit[unit - 1] == NULL)
        {
            devm_unit_create(unit, (struct devm_unit *)mesg->msg_data);
            myboard_dev_start_status = HA_BOOTSTA_START;

//            devm_update_slot_partnerslot();
            zlog_debug(DEVM_DBG_MSG, "%s[%d]devm recv add unit(%d) msg, myslot = %d\n", __func__, __LINE__, unit, device.pmyunit->myslot);

//            devm_sig_func(NULL);                      /*IPC optimization and replaced by high_pre_timer_add*/
            high_pre_timer_add("DevmTimer", LIB_TIMER_TYPE_LOOP, devm_sig_func, NULL, 1000);
            devm_get_sysObjectid();
        }
    }
    else if (phdr->msg_subtype == DEVM_INFO_SLOT)  /* 板卡创建 */
    {
        struct devm_unit *punit = device.pmyunit;
        struct devm_slot *pslot = (struct devm_slot *)mesg->msg_data;
        struct devm_com t_devm_com;
 
        if (NULL != punit)
        {
            zlog_debug(DEVM_DBG_MSG, "%s[%d]devm recv add slot(%d,%d) msg,myslot = %d\n", __func__, __LINE__, unit, slot, device.pmyunit->myslot);
            ret = devm_add_slot(unit, slot, pslot);
            if (ERRNO_SUCCESS == ret && NULL != device.pmyunit->pslot[slot - 1])
            {
//                ipc_send_ack(phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index);

                if (pslot->type == SLOT_TYPE_BOARD_MAIN)      /* 创建主用主控板 */
                {
                	//device.pmyunit->slot_main_board = pslot->slot;
					device.punit[unit - 1]->slot_main_board= pslot->slot;
                    if (0 == device.myslot)
                    {
                        punit->myslot = pslot->slot;
                        punit->slot_ha = DEVM_HA_MASTER;
                        device.slot_type = SLOT_TYPE_BOARD_MAIN;
                        device.myslot = pslot->slot;
                    }

                    if (punit->myslot == punit->slot_main_board)
                    {
                        ha_dev_ha_role = HA_ROLE_MASTER;
                    }
                    else if (punit->myslot == punit->slot_slave_board)
                    {
                        ha_dev_ha_role = HA_ROLE_SLAVE;
                    }
                    else
                    {
                        ha_dev_ha_role = HA_ROLE_INIT;
                    }

                    //punit->slot_main_board = pslot->slot;
//                    ha_start_heartbeat_timer(NULL);
                    high_pre_timer_add("HaStartHeartbeatTimer", LIB_TIMER_TYPE_LOOP, ha_start_heartbeat_timer, NULL, 15000);
                }
                else if (pslot->type == SLOT_TYPE_BOARD_SLAVE)
                {
                	//device.pmyunit->slot_slave_board = pslot->slot;
                    device.punit[unit - 1]->slot_slave_board= pslot->slot;
              
                    if (0 == device.myslot)
                    {
                        punit->myslot = pslot->slot;
                        punit->slot_ha = DEVM_HA_SLAVE;
                        device.slot_type = SLOT_TYPE_BOARD_SLAVE;
                        device.myslot = pslot->slot;
                    }

                    if (punit->myslot == punit->slot_main_board)
                    {
                        ha_dev_ha_role = HA_ROLE_MASTER;
                    }
                    else if (punit->myslot == punit->slot_slave_board)
                    {
                        ha_dev_ha_role = HA_ROLE_SLAVE;
                    }
                    else
                    {
                        ha_dev_ha_role = HA_ROLE_INIT;
                    }

//                    punit->slot_slave_board = pslot->slot;
//                    ha_start_heartbeat_timer(NULL);
                    high_pre_timer_add("HaStartHeartbeatTimer", LIB_TIMER_TYPE_LOOP, ha_start_heartbeat_timer, NULL, 15000);                    
                }

                devm_get_slot_app_version(sys_version[slot - 1], slot);
                //devm_update_slot_partnerslot();
                /*acquire other slot sys version info*/
                if (0 != pslot->hw.pad[0])
                {
                    sprintf(sys_version[slot - 1], "%x.%-2x", pslot->hw.pad[0]/16, pslot->hw.pad[0]%16);    
                }
                if (pslot->slot == device.myslot)     /* create myslot */
                {
                    /*send master myboard_dev_start_status msg */
                    memset(&t_devm_com, 0, sizeof(struct devm_com));
                    t_devm_com.unit = device.main_unit;
                    t_devm_com.slot = slot;
                    t_devm_com.main_slot = device.pmyunit->slot_main_board;
                    t_devm_com.slave_slot = device.pmyunit->slot_slave_board;
                    t_devm_com.type = device.pmyunit->pslot[slot - 1]->type;
                    t_devm_com.status = DEV_STATUS_WORK;
                    t_devm_com.ha_role = ha_dev_ha_role;
                    t_devm_com.ha_status = myboard_dev_start_status;
                    memcpy(t_devm_com.mac,  device.pmyunit->pslot[slot - 1]->mac, sizeof(t_devm_com.mac));

                    devm_event_notify(DEV_EVENT_HA_BOOTSTATE_CHANGE, &t_devm_com);

                    /*通知文件管理系统*/
                    ipc_send_msg_n2( &t_devm_com, sizeof(struct devm_com), 1, MODULE_ID_FILE,  MODULE_ID_DEVM,  IPC_TYPE_DEVM, DEV_EVENT_HA_BOOTSTATE_CHANGE, IPC_OPCODE_EVENT, 0);              
                }
                else if (SLOT_TYPE_BOARD_SLAVE == pslot->type
                         || SLOT_TYPE_BOARD_MAIN == pslot->type)
                {
                    devm_send_init_ha_to_otherboard();
                }
            }

        }
    }
    else if (phdr->msg_subtype == DEVM_INFO_PORT)  /* 创建端口 */
    {
        if (DEV_TYPE_BOX == device.type)
        {
            if (NULL == device.pbox)
            {
                zlog_err("%s[%d]:leave %s:error:fail to add port, box not exist,NULL == device.pbox\n", __FILE__, __LINE__, __func__);
            }
            else
            {
                p_devm_port = (struct devm_port *)mesg->msg_data;

                if (ERRNO_SUCCESS == devm_add_port(unit, p_devm_port->slot, &device.pbox->port_list, p_devm_port))
                {
                    device.pbox->port_num++;
                }
            }
        }
        else if (DEV_TYPE_SHELF == device.type)
        {
            if (unit > 0 && NULL == device.punit[unit - 1])
            {
                zlog_err("%s[%d]:leave %s:error:fail to add port, unit(%d) not exist\n", __FILE__, __LINE__, __func__ , unit);
            }
            else
            {
                if (NULL == device.punit[unit - 1]->pslot[slot - 1]  || 1 == device.punit[unit - 1]->pslot[slot - 1]->plugout)
                {
                    zlog_err("%s[%d]:leave %s:error:fail to add port, slot(%d,%d) not exist,or slot plugout\n", __FILE__, __LINE__, __func__ , unit, slot);
                }
                else
                {
                    /* 1000&2000 设备主卡 */
                    p_devm_port = (struct devm_port *)mesg->msg_data;

                    if (ERRNO_SUCCESS == devm_add_port(unit, p_devm_port->slot, &device.punit[unit - 1]->pslot[slot - 1]->port_list, p_devm_port))
                    {
                        device.punit[unit - 1]->pslot[slot - 1]->port_num++;
                    }
                }
            }
        }
        else
        {
            zlog_err("%s[%d]:leave %s:error:fail to add port, device type(%d) error, not box or shelf\n", __FILE__, __LINE__, __func__, device.type);
        }
    }
    else if (phdr->msg_subtype == DEVM_INFO_PLUGIN) /* 配置子卡 slot plugin */
    {
        enum SLOT_TYPE  *slot_type = (enum SLOT_TYPE *)mesg->msg_data;

        if (*slot_type == SLOT_TYPE_BOARD_SLAVE) /* 主卡收到备卡插入事件 */
        {
            zlog_notice("%s[%d]devm recv slave board plugin,unit = %d, slot = %d\n", __func__, __LINE__, unit, slot);

            if (unit > 0 && NULL != device.pmyunit)
            {
                ;//device.pmyunit->slot_slave_board = slot;
            }
        }
        else
        {
            zlog_notice("%s[%d]devm recv slot (%d,%d) plugin,send msg to other board\n", __func__, __LINE__, unit, slot);
            ret = devm_slot_plugin(unit, slot);
        }
    }
    else if (phdr->msg_subtype == DEVM_INFO_POWER)  /* 板卡插入 */
    {
        gPwrSlot1 = (uint8_t)((phdr->msg_index & 0xff000000) >> 24);
        gPwrSlot2 = (uint8_t)((phdr->msg_index & 0xff0000) >> 16);
        devm_add_power(unit, slot, (struct devm_power *)mesg->msg_data);
    }
    else if (phdr->msg_subtype == DEVM_INFO_FAN)    /* 板卡插入 */
    {
        gFanSlot = slot;
        devm_add_fan(unit, slot, (struct devm_fan *)mesg->msg_data);
    }
    else if (phdr->msg_subtype == DEVM_INFO_E2DATA_HAGET)
    {
         if((phdr->module_id == MODULE_ID_DEVM) && (phdr->data_len >= sizeof(struct devm_backplane)))
         {
              printf("receive form master cpu, set backplan e2 data for slave cpu\r\n");
              
              memcpy(&backplaneeinfo, mesg->msg_data, sizeof(struct devm_backplane));
             
              if((backplaneeinfo.id == 0) || (backplaneeinfo.id == 0xffff)) memset(&backplaneeinfo, 0, sizeof(struct devm_backplane));
         }
    }

    return;
}

static void devm_msg_del(struct ipc_mesg_n *mesg)
{
    int ret = 0;
    uint8_t unit = 0;
    uint8_t slot = 0;
    struct ipc_msghdr_n *phdr = &mesg->msghdr;
    struct devm_port *p_devm_port = NULL;

    unit = (uint8_t)((phdr->msg_index & 0xff00) >> 8);
    slot = (uint8_t)(phdr->msg_index & 0xff);

    if (phdr->msg_subtype == DEVM_INFO_SLOT)
    {
        int ha_op = ((phdr->msg_index >> 16) & 0x1);
        zlog_debug(DEVM_DBG_MSG, "%s[%d]:leave %s:devm msg recv delete slot %d msg,ha_op= %d\n", __FILE__, __LINE__, __func__, slot, ha_op);
        ret = devm_delete_slot(unit, slot);
    }
    else if (phdr->msg_subtype == DEVM_INFO_PORT)
    {
        p_devm_port = (struct devm_port *)mesg->msg_data;
        devm_delete_port(unit, slot, &device.punit[unit - 1]->pslot[slot - 1]->port_list, p_devm_port);
    }
    else if (phdr->msg_subtype == DEVM_INFO_PLUGOUT) /* 配置slot plugout */
    {
        enum SLOT_TYPE  *slot_type = (enum SLOT_TYPE *)mesg->msg_data;

        if (*slot_type == SLOT_TYPE_BOARD_SLAVE) /* 判断是否备份主控板拔出 */
        {
            if (unit > 0 && device.punit[unit - 1] != NULL)
            {
                device.punit[unit - 1]->slot_slave_board = 0;
            }
        }
        else
        {
            ret = devm_slot_plugout(unit, slot);

            if (ERRNO_SUCCESS == ret && 1 == device.punit[unit - 1]->pslot[slot - 1]->plugout)
            {
                zlog_notice("%s[%d]devm recv slot (%d,%d) plugout,send msg to other board\n", __func__, __LINE__, unit, slot);
            }
        }
    }
    else if (phdr->msg_subtype == DEVM_INFO_POWER)
    {
        devm_del_power(unit, slot);
    }
    else if (phdr->msg_subtype == DEVM_INFO_FAN)
    {
        devm_del_fan(unit, slot);
    }

    return;
}


static void devm_power_alarm_init(uint32_t powerslot1, uint32_t powerslot2)
{
    /* modify for ipran by lipf, 2018/4/25 */
    struct ifm_usp usp;
    struct gpnPortInfo gPortInfo;

    memset(&usp, 0, sizeof(struct ifm_usp));
    usp.type = IFNET_TYPE_MAX;
    usp.unit = 1;
    usp.slot = powerslot1;
    usp.port = 0;

    memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
    gPortInfo.iAlarmPort = IFM_POW220_TYPE;
    gPortInfo.iIfindex = ifm_get_ifindex_by_usp(&usp);
    ipran_alarm_port_register(&gPortInfo);

    gPortInfo.iAlarmPort = IFM_POWN48_TYPE;
    ipran_alarm_port_register(&gPortInfo);


    usp.slot = powerslot2;
    gPortInfo.iAlarmPort = IFM_POW220_TYPE;
    gPortInfo.iIfindex = ifm_get_ifindex_by_usp(&usp);
    ipran_alarm_port_register(&gPortInfo);

    gPortInfo.iAlarmPort = IFM_POWN48_TYPE;
    ipran_alarm_port_register(&gPortInfo);
    return;
}


static void devm_msg_update(struct ipc_mesg_n *mesg)
{
    int ret = 0;
    uint8_t unit = 0;
    uint8_t slot = 0;
    unsigned short *fpga_ver = NULL;
    struct ipc_msghdr_n *phdr = &mesg->msghdr;

    unit = (uint8_t)((phdr->msg_index & 0xff00) >> 8);
    slot = (uint8_t)(phdr->msg_index & 0xff);

    if (phdr->msg_subtype == DEVM_INFO_UP_FPGA_VER)
    {
        if (unit > 0 && NULL != device.punit[unit - 1])
        {
            if (NULL != device.punit[unit - 1]->pslot[slot - 1])
            {
                fpga_ver = (unsigned short *)mesg->msg_data;
                zlog_notice("%s[%d]:leave %s:slot %d fpga ver update to %x.%x\n", __FILE__, __LINE__, __func__ , slot, (*fpga_ver) / 16, (*fpga_ver) % 16);
                device.punit[unit - 1]->pslot[slot - 1]->hw.fpga_ver = *fpga_ver;
            }
        }
        else if (NULL != device.pbox)
        {
            fpga_ver = (unsigned short *)mesg->msg_data;
            zlog_notice("%s[%d]:leave %s:fpga ver update to %x.%x\n", __FILE__, __LINE__, __func__ , (*fpga_ver) / 16, (*fpga_ver) % 16);
            device.pbox->hw.fpga_ver = *fpga_ver;
        }
    }
    else if (phdr->msg_subtype == DEVM_INFO_POWER)
    {
        gPwrSlot1 = (uint8_t)((phdr->msg_index & 0xff000000) >> 24);
        gPwrSlot2 = (uint8_t)((phdr->msg_index & 0xff0000) >> 16);
        if( !powerAlmFlg )
        {
            devm_power_alarm_init(gPwrSlot1, gPwrSlot2);
            powerAlmFlg = 0x1;
        }
        devm_update_power(unit, slot, (struct devm_power *)mesg->msg_data);
    }
    else if (phdr->msg_subtype == DEVM_INFO_FAN)
    {
        devm_update_fan(unit, slot, (struct devm_fan *)mesg->msg_data);
    }
    else if (phdr->msg_subtype == DEVM_INFO_SLAVE_TEMP)
    {
        devm_update_slave_temp(unit, slot, (char *)mesg->msg_data);
    }	
    else if (phdr->msg_subtype == DEVM_INFO_TEMP)
    {
        devm_update_temp(unit, slot, (char *)mesg->msg_data);
    }
    else if (phdr->msg_subtype == DEVM_INFO_MASTER_BACKUP)
    {
        struct devm_com         t_devm_com;
        struct slot_ha_status   ha_status;
        enum DEV_EVENT          t_dev_event;
        int                     my_slot;
        int                     neighbour_slot;

        memcpy(&ha_status, mesg->msg_data, sizeof(struct slot_ha_status));

        my_slot = device.pmyunit->myslot;
        neighbour_slot = ha_status.neighbour_slot;

        if (0 == my_slot || 0 == neighbour_slot || NULL == device.pmyunit || NULL == device.pmyunit->pslot[my_slot - 1])
        {
            zlog_err("%s[%d]:leave %s: error, 0 == device.pmyunit->myslot, failed to exchange main/slave\n", __FILE__, __LINE__, __func__);
        }
        else 
        {
            if (my_slot == ha_status.my_slot)
            {
                if (HA_ROLE_MASTER == ha_status.my_status)
                {
                    device.pmyunit->slot_main_board  = my_slot;
                    device.pmyunit->slot_slave_board = ha_status.neighbour_exsit ? neighbour_slot : 0;

                    device.pmyunit->pslot[my_slot - 1]->type = SLOT_TYPE_BOARD_MAIN;

                    if (NULL != device.pmyunit->pslot[neighbour_slot - 1])
                    {
                        device.pmyunit->pslot[neighbour_slot - 1]->type = SLOT_TYPE_BOARD_SLAVE;
                    }
                    else
                    {
                        ; // error, neighbour_slot slot no add
                    }
                    ha_dev_ha_role = HA_ROLE_MASTER;
                    device.slot_type = SLOT_TYPE_BOARD_MAIN;
                }
                else 
                {
                    device.pmyunit->slot_main_board  = neighbour_slot;
                    device.pmyunit->slot_slave_board = my_slot;

                    device.pmyunit->pslot[my_slot - 1]->type = SLOT_TYPE_BOARD_SLAVE;
                    if (NULL != device.pmyunit->pslot[neighbour_slot - 1])
                    {
                        device.pmyunit->pslot[neighbour_slot - 1]->type = SLOT_TYPE_BOARD_MAIN;
                    }
                    else
                    {
                        ; // error, neighbour_slot slot no add
                    }
                    ha_dev_ha_role = HA_ROLE_SLAVE;
                    device.slot_type = SLOT_TYPE_BOARD_SLAVE;
                }
            }
        }

        zlog_notice("%s[%d] %s: master backup, myslot = %d, neighbour_slot = %d, mainslot = %d, slaveslot = %d, ha role = %d\n"
                    , __FILE__, __LINE__, __func__
                    , device.pmyunit->myslot, neighbour_slot, device.pmyunit->slot_main_board, device.pmyunit->slot_slave_board
                    , ha_dev_ha_role);

        /*send master backup msg */
        memset(&t_devm_com, 0, sizeof(struct devm_com));
        t_devm_com.unit = device.main_unit;
        t_devm_com.slot = device.pmyunit->myslot;
        t_devm_com.main_slot = device.pmyunit->slot_main_board;
        t_devm_com.slave_slot = device.pmyunit->slot_slave_board;
        t_devm_com.type = device.pmyunit->pslot[t_devm_com.slot - 1]->type;
        t_devm_com.status = DEV_STATUS_WORK;
        t_devm_com.ha_role = ha_dev_ha_role;
        t_devm_com.ha_status = myboard_dev_start_status;
        memcpy(t_devm_com.mac,  device.pmyunit->pslot[t_devm_com.slot - 1]->mac, sizeof(t_devm_com.mac));

        zlog_debug(DEVM_DBG_MSG, "%s[%d],func %s:broadcast DEV_EVENT_HA_BOOTSTATE_CHANGE msg, slot = %d, main slot = %d, slave slot = %d\n", __FILE__, __LINE__, __func__, t_devm_com.slot, t_devm_com.main_slot, t_devm_com.slave_slot);

        devm_event_notify(DEV_EVENT_HA_BOOTSTATE_CHANGE, &t_devm_com);

        ret = ipc_send_msg_ha_n2(&t_devm_com, sizeof(struct devm_com), 1, MODULE_ID_DEVM,  MODULE_ID_DEVM, IPC_TYPE_DEVM, HA_SMSG_BOOT_STATE, IPC_OPCODE_EVENT, 0, 0);

        if (ret != 0)
        {
            zlog_err("%s[%d]:leave %s: error, failed to send this board(%d) ha_dev_ha_role chang to %s to ha\n", __FILE__, __LINE__, __func__, device.myslot, a_ha_dev_ha_role[ha_dev_ha_role]);
        }
        else
        {
            zlog_debug(DEVM_DBG_MSG, "send  DEV_EVENT_HA_BOOTSTATE_CHANGE to otherboard, slot = %d, main slot = %d, slave slot = %d\n", t_devm_com.slot, t_devm_com.main_slot, t_devm_com.slave_slot);
        }
    }
    else if (phdr->msg_subtype == DEVM_INFO_RUNNINGTIME)
    {
        time_t *p_running_time = (time_t *)mesg->msg_data;

        if (NULL != device.pmyunit)
        {
            if (NULL !=  device.pmyunit->pslot[slot - 1])
            {
                device.pmyunit->pslot[slot - 1]->run_time = *p_running_time;
//                ipc_send_ack(phdr->module_id, phdr->sender_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index);
                zlog_debug(DEVM_DBG_MSG, "%s[%d]:leave %s:recv set slot %d running time=0x%lx\n", __FILE__, __LINE__, __func__ , slot, (long int)*p_running_time);
            }
            else
            {
//                ipc_send_noack(1, phdr->module_id, phdr->sender_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index);
                zlog_err("%s[%d]:leave %s:recv set slot %d running time,but failed, NULL ==  device.pmyunit->pslot[%d]\n", __FILE__, __LINE__, __func__ , slot, slot - 1);
            }
        }
        else if (NULL != device.pbox)
        {
            device.pbox->run_time = *p_running_time;
//            ipc_send_ack(phdr->module_id, phdr->sender_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index);
            zlog_debug(DEVM_DBG_MSG, "%s[%d]:leave %s:recv set box running time=0x%lx\n", __FILE__, __LINE__, __func__ , (long int)*p_running_time);
        }
        else
        {
//            ipc_send_noack(1, phdr->module_id, phdr->sender_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index);
            zlog_err("%s[%d]:leave %s:recv set slot %d running time,but failed, NULL ==  device.pmyunit\n", __FILE__, __LINE__, __func__ , slot);
        }
    }
    else if (phdr->msg_subtype == DEVM_INFO_SYS_VERSION)
    {
        memcpy(sys_version[slot - 1], mesg->msg_data, mesg->msghdr.data_len);
        zlog_debug(DEVM_DBG_MSG, "%s[%d]:leave %s:sucessfully to recv msg to get slot %d,sys_ver(%s) ,data len = %d!\n", __FILE__, __LINE__, __func__ , slot, sys_version[slot - 1], sizeof(sys_version[slot - 1]));
    }

    return;
}


static void devm_msg_event(struct ipc_mesg_n *mesg)
{
    int ret = 0;
    uint8_t unit = 0;
    uint8_t slot = 0;
    struct ipc_msghdr_n *phdr = &mesg->msghdr;


    unit = (uint8_t)((phdr->msg_index & 0xff00) >> 8);
    slot = (uint8_t)(phdr->msg_index & 0xff);

    if (DEVM_INFO_FINISH_CREATE_SLOTS == phdr->msg_subtype)
    {
        ret = ipc_send_msg_n2(NULL, 0, 0, MODULE_ID_IFM, MODULE_ID_DEVM,
                              IPC_TYPE_DEVM, DEV_EVENT_SLOT_ADD_FINISH, IPC_OPCODE_EVENT, 0);

        if (-1 == ret)
        {
            zlog_err("%s[%d]:leave %s:error:fail to send slot add finish msg to ifm\n", __FILE__, __LINE__, __func__);
        }
        else
        {
            printf("%s[%d]:leave %s:sucessfully to to send slot add finish msg to ifm\n", __FILE__, __LINE__, __func__);
            zlog_debug(DEVM_DBG_MSG, "%s[%d]:leave %s:sucessfully to to send slot add finish msg to ifm\n", __FILE__, __LINE__, __func__);
        }
    }

    return;
}


static void devm_msg_enable(struct ipc_mesg_n *mesg)
{
    if (mesg->msghdr.msg_subtype == DEVM_INFO_HA)
    {
        printf("devm_msg_enable mesg->msghdr.msg_index = %d\r\n", mesg->msghdr.msg_index);
        
        ha_init(mesg->msghdr.msg_index);    /* 初始化 HA 通道 */
    }

    return;
}

#if 0
static void ha_msg_get(struct ipc_mesg_n *pmesg)
{
    int ret = 0;
    uint8_t unit = 0;
    uint8_t slot = 0;
    struct ipc_msghdr_n *phdr = &pmesg->msghdr;


    unit = (uint8_t)((phdr->msg_index & 0xff00) >> 8);
    slot = (uint8_t)(phdr->msg_index & 0xff);

    if (HA_INFO_APP_INIT == phdr->msg_subtype)
    {
        struct ha_com t_ha_com;

        memset(&t_ha_com, 0, sizeof(t_ha_com));
        memcpy(&t_ha_com, pmesg->msg_data, pmesg->msghdr.data_len);

        if (0 == t_ha_com.module_id || t_ha_com.module_id > HA_MODULE_ID_MAX)
        {
//            ret = ipc_send_noack(1, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index);
            ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, 0, phdr->msg_index, IPC_OPCODE_NACK);                                   

            if (ret < 0)
            {
                zlog_err("%s[%d]:leave %s:error: ha failed to send  noack msg to module %d to reply app init event msg, since module id of data is 0, is out of range(%d, %d), failed to write into the code\n"
                         , __FILE__, __LINE__, __func__, phdr->sender_id, 1, HA_MODULE_ID_MAX);
            }
            else
            {
                zlog_debug(DEVM_DBG_MSG, "%s[%d]:leave %s,ha sucessfully to send  noack msg to module %d to reply app init event msg, since module id of data is 0,  is out of range(%d, %d), failed to write into the code\n"
                           , __FILE__, __LINE__, __func__, phdr->sender_id, 1, HA_MODULE_ID_MAX);
            }
        }
        else
        {
            t_ha_com.module_init_count = devm_ha_app_init_count[t_ha_com.module_id];

//            ret = ipc_send_reply(&t_ha_com, sizeof(t_ha_com), phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index);
            ret = ipc_send_reply_n2(&t_ha_com, sizeof(t_ha_com), 1, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, 0, phdr->msg_index, IPC_OPCODE_REPLY);

            if (ret < 0)
            {
                zlog_err("%s[%d]:leave %s,ha failed to send  module(%d) init count(%d) msg reply to module %d\n", __FILE__, __LINE__, __func__
                         , t_ha_com.module_id, t_ha_com.module_init_count, phdr->sender_id);
            }
            else
            {
                zlog_debug(DEVM_DBG_MSG, "%s[%d]:leave %s,ha sucessfully to send  module(%d) init count(%d) msg reply to module %d\n", __FILE__, __LINE__, __func__
                           , t_ha_com.module_id, t_ha_com.module_init_count, phdr->sender_id);
            }
        }
    }
}
#endif

static void ha_msg_event(struct ipc_mesg_n *mesg)
{
    if (HA_SMSG_BOOT_STATE == mesg->msghdr.msg_subtype)
    {
        if (MODULE_ID_VTY == mesg->msghdr.sender_id)
        {
            int             iStaChg = 0;
            struct devm_com t_devm_com;

            memcpy(&t_devm_com, mesg->msg_data, sizeof(struct devm_com));

            if(((HA_BOOTSTA_END == t_devm_com.ha_status) && (HA_BOOTSTA_START == myboard_dev_start_status)) ||
               ((HA_BOOTSTA_DATAOK == t_devm_com.ha_status) && (HA_BOOTSTA_END == myboard_dev_start_status)))
            {
                  zlog_debug(DEVM_DBG_MSG, "%s[%d]:%s: recv HA_SMSG_BOOT_STATE msg(%s) from vtysh, chang myboard_dev_start_status(%s) to HA_BOOTSTA_END\n", __FILE__, __LINE__, __func__, a_ha_bootstate[t_devm_com.ha_status], a_ha_bootstate[myboard_dev_start_status]);

                  myboard_dev_start_status = t_devm_com.ha_status;

                  /*send master myboard_dev_start_status msg */
                  if (device.pbox)
                  {
                      iStaChg = 1;
                      
                      memset(&t_devm_com, 0, sizeof(struct devm_com));
                      t_devm_com.unit = device.main_unit;
                      t_devm_com.slot = 1;
                      t_devm_com.main_slot = 1;
                      t_devm_com.slave_slot = 0;
                      t_devm_com.type = device.pbox->type;
                      t_devm_com.status = DEV_STATUS_WORK;
                      t_devm_com.ha_status = myboard_dev_start_status;
                      t_devm_com.ha_role = ha_dev_ha_role;
                      memcpy(t_devm_com.mac,  device.pbox->mac, sizeof(t_devm_com.mac));
                  }
                  else if (NULL != device.pmyunit)
                  {
                      if((device.pmyunit->myslot > 0) && (device.pmyunit->myslot < SLOT_NUM_MAX) && 
                         (device.pmyunit->pslot[device.pmyunit->myslot - 1] != NULL))
                      {       
                           iStaChg = 1;
                           
                           memset(&t_devm_com, 0, sizeof(struct devm_com));
                           t_devm_com.unit = device.main_unit;
                           t_devm_com.slot = device.pmyunit->myslot;
                           t_devm_com.main_slot = device.pmyunit->slot_main_board;
                           t_devm_com.slave_slot = device.pmyunit->slot_slave_board;
                           t_devm_com.type = device.pmyunit->pslot[device.pmyunit->myslot - 1]->type;
                           t_devm_com.status = DEV_STATUS_WORK;
                           t_devm_com.ha_status = myboard_dev_start_status;
                           t_devm_com.ha_role = ha_dev_ha_role;
                           memcpy(t_devm_com.mac,  device.pmyunit->pslot[device.pmyunit->myslot - 1]->mac, sizeof(t_devm_com.mac));
                      }
                      else
                      {
                           zlog_err("%s[%d]:leave %s:error:recv ha status update msg(ha_status = %s) from vtysh, but NULL == myslot device.pmyunit->myslot =%d!\n", __FILE__, __LINE__, __func__ , a_ha_bootstate[t_devm_com.ha_status] , device.pmyunit->myslot);
                      }
                      
                  }
                  else
                  {
                      zlog_err("%s[%d]:leave %s:error:recv ha status update msg(ha_status = %s) from vtysh, but NULL == pmyunit or NULL ==  device.pmyunit->pslot[%d]!\n", __FILE__, __LINE__, __func__ , a_ha_bootstate[t_devm_com.ha_status] , t_devm_com.slot - 1);
                  }

                  if(iStaChg)
                  {
                      printf("ha_msg_event send ha status to other slot t_devm_com.slot = %d t_devm_com.main_slot =%d t_devm_com.ha_status=%d\r\n", t_devm_com.slot, t_devm_com.main_slot, t_devm_com.ha_status);

                      devm_event_notify(DEV_EVENT_HA_BOOTSTATE_CHANGE, &t_devm_com);

                      ipc_send_msg_ha_n2(&t_devm_com, sizeof(struct devm_com), 1, MODULE_ID_DEVM,  MODULE_ID_DEVM, IPC_TYPE_HA, HA_SMSG_BOOT_STATE, IPC_OPCODE_EVENT, 0, 0);
/*
                      if (ret)
                      {
                          zlog_err("%s[%d]:leave %s:error:fail to send ha status chang(->%s) to otherboard!\n", __FILE__, __LINE__, __func__ , a_ha_bootstate[t_devm_com.ha_status]);
                      }
                      else
                      {
                          zlog_debug(DEVM_DBG_MSG, "%s[%d]:leave %s:sucessfully to send ha status chang(->%s) to otherboard!\n", __FILE__, __LINE__, __func__ , a_ha_bootstate[t_devm_com.ha_status]);
                      }
*/
                 }
            }
        }
        else if (MODULE_ID_DEVM == mesg->msghdr.sender_id)  //from other board
        {
            struct devm_com *recv_devm_com = NULL;

            recv_devm_com = (struct devm_com *)mesg->msg_data;

            otherboard_dev_start_status = recv_devm_com->ha_status;

            zlog_debug(DEVM_DBG_HA, "%s[%d],func %s:recv DEV_EVENT_HA_BOOTSTATE_CHANGE msg from otherboard, slot = %d, main slot = %d, slave slot = %d\n", __FILE__, __LINE__, __func__, recv_devm_com->slot, recv_devm_com->main_slot, recv_devm_com->slave_slot);

            devm_event_notify(DEV_EVENT_HA_BOOTSTATE_CHANGE, recv_devm_com);
            
            printf("ha_msg_event send ha status to all APP slot recv_devm_com.slot = %d recv_devm_com.main_slot =%d\r\n", recv_devm_com->slot, recv_devm_com->main_slot );
        }
    }
#if 0    
    else if (HA_INFO_APP_INIT == phdr->msg_subtype)
    {
        struct ha_com t_ha_com;
        memset(&t_ha_com, 0, sizeof(t_ha_com));
        memcpy(&t_ha_com, mesg->msg_data, mesg->msghdr.data_len);

        if (0 == t_ha_com.module_id || t_ha_com.module_id > HA_MODULE_ID_MAX)
        {
//            ret = ipc_send_noack(1, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index);
            ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, 0, phdr->msg_index, IPC_OPCODE_NACK);                                   

            if (ret < 0)
            {
                zlog_err("%s[%d]:leave %s:error: ha failed to send  noack msg to module %d to reply app init event msg, since module id of data is 0, is out of range(%d, %d), failed to write into the code\n"
                         , __FILE__, __LINE__, __func__, phdr->sender_id,  1, HA_MODULE_ID_MAX);
            }
            else
            {
                zlog_debug(DEVM_DBG_MSG, "%s[%d]:leave %s,ha sucessfully to send  noack msg to module %d to reply app init event msg, since module id of data is 0, is out of range(%d, %d), failed to write into the code\n"
                           , __FILE__, __LINE__, __func__, phdr->sender_id, 1, HA_MODULE_ID_MAX);
            }
        }
        else
        {
            devm_ha_app_init_count[t_ha_com.module_id]++;

            t_ha_com.module_init_count = devm_ha_app_init_count[t_ha_com.module_id];
//            ret = ipc_send_reply(&t_ha_com, sizeof(t_ha_com), phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index);
            ret = ipc_send_reply_n2(&t_ha_com, sizeof(t_ha_com), 1, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, 0, phdr->msg_index, IPC_OPCODE_REPLY);

            if (ret < 0)
            {
                zlog_err("%s[%d]:leave %s,ha failed to send  module(%d) init counter(%d) msg reply to module %d\n", __FILE__, __LINE__, __func__
                         , t_ha_com.module_id, t_ha_com.module_init_count, phdr->sender_id);
            }
            else
            {
                zlog_debug(DEVM_DBG_MSG, "%s[%d]:leave %s,ha sucessfully to send  module(%d) init counter(%d) msg reply to module %d\n", __FILE__, __LINE__, __func__
                           , t_ha_com.module_id, t_ha_com.module_init_count, phdr->sender_id);
            }
        }
    }
#endif
}

#if 0
/*接收公共 IPC 消息*/
int devm_msg_rcv(struct thread *p_thread)
{
//  int ret = 0;
    uint8_t unit = 0;
    uint8_t slot = 0;
    static struct ipc_mesg mesg;
    struct ipc_msghdr *phdr = NULL;
//  struct devm_port *p_devm_port = NULL;
//  unsigned short *fpga_ver = NULL;

    if (-1 == ipc_recv_common(&mesg, MODULE_ID_DEVM) &&  -1 == ipc_recv_common(&mesg, MODULE_ID_HA))
    {
        goto out;
    }

    phdr = &(mesg.msghdr);

    zlog_debug(DEVM_DBG_MSG, "%s[%d]:%s: devm msg get , opcode = %d,subtype = %d,senderid=%d,msg_type=%d, index = 0x%x\n", __FILE__, __LINE__, __func__, phdr->opcode, phdr->msg_subtype, phdr->sender_id, phdr->msg_type , phdr->msg_index);

    if (MODULE_ID_DEVM == phdr->module_id)
    {
        if (phdr->msg_type == IPC_TYPE_DEVM) /* 处理设备管理消息*/
        {
            unit = (uint8_t)((phdr->msg_index & 0xff00) >> 8);
            slot = (uint8_t)(phdr->msg_index & 0xff);

            /* handle ifm event register msg */
            if (phdr->opcode == IPC_OPCODE_REGISTER)   /* 事件注册消息 */
            {
                unsigned char t_queue_id = 0;
                memcpy(&t_queue_id, (unsigned char *)mesg.msg_data, phdr->data_len);
                devm_register_msg(phdr->sender_id, phdr->msg_subtype,  t_queue_id);
            }
            else if (phdr->opcode == IPC_OPCODE_GET)   /* 获取设备信息 */
            {
                devm_msg_get(&mesg);
            }
            else if (phdr->opcode == IPC_OPCODE_ADD)
            {
                devm_msg_add(&mesg);
            }
            else if (phdr->opcode == IPC_OPCODE_DELETE)
            {
                devm_msg_del(&mesg);
            }
            else if (phdr->opcode == IPC_OPCODE_UPDATE)
            {
                devm_msg_update(&mesg);
            }
            else if (phdr->opcode == IPC_OPCODE_EVENT)
            {
                devm_msg_event(&mesg);
            }
            else if (phdr->opcode == IPC_OPCODE_ENABLE)
            {
                devm_msg_enable(&mesg);
            }
        }
    }
    else if (MODULE_ID_HA == phdr->module_id)
    {
        if (IPC_TYPE_HA == phdr->msg_type)
        {
            if (IPC_OPCODE_EVENT == phdr->opcode)
            {
                ha_msg_event(&mesg);
            }
            else if (IPC_OPCODE_GET == phdr->opcode)
            {
                ha_msg_get(&mesg);
            }
        }
    }

out:
    usleep(10000);//让出 CPU 10ms
    thread_add_event(devm_master, devm_msg_rcv, NULL, 0);

    return 0;
}
#endif

void handle_devm_msg(struct ipc_mesg_n *pmsg)
{
/*
    uint8_t unit = 0;
    uint8_t slot = 0;

    unit = (uint8_t)((pmsg->msghdr.msg_index & 0xff00) >> 8);
    slot = (uint8_t)(pmsg->msghdr.msg_index & 0xff);
*/
    unsigned char t_queue_id = 0;

    switch(pmsg->msghdr.opcode)
    {
        /* handle ifm event register msg */
        case IPC_OPCODE_REGISTER:           /* 事件注册消息 */
            
            memcpy(&t_queue_id, (unsigned char *)pmsg->msg_data, pmsg->msghdr.data_len);
            devm_register_msg(pmsg->msghdr.sender_id, pmsg->msghdr.msg_subtype,  t_queue_id);
            break;
            
        case IPC_OPCODE_GET:                    /* 获取设备信息 */
            devm_msg_get(pmsg);
            break;
            
        case IPC_OPCODE_ADD:
            devm_msg_add(pmsg);
            break;
            
        case IPC_OPCODE_DELETE:
            devm_msg_del(pmsg);
            break;
            
        case IPC_OPCODE_UPDATE:
            devm_msg_update(pmsg);
            break;
            
        case IPC_OPCODE_EVENT:
            devm_msg_event(pmsg);
            break;
            
        case IPC_OPCODE_ENABLE:
            devm_msg_enable(pmsg);
            break;
            
        default:
            printf("%s %d:msg opcode is error!\n", __FILE__, __LINE__);
            break;
    }
}

void devm_handle_ha_msg(struct ipc_mesg_n *pmsg)
{
    switch(pmsg->msghdr.opcode)
    {
        case IPC_OPCODE_EVENT:
             ha_msg_event(pmsg);
             break;
            
        case IPC_OPCODE_GET:
          // ha_msg_get(pmsg);
             break;
            
        default:
                printf("%s %d:msg opcode is error!\n", __FILE__, __LINE__);
                break;
    }
}

/* receive common IPC messages */
int devm_msg_rcv(struct ipc_mesg_n *pmsg, int imlen)
{
    int retva = 0;
    int revln = 0;

    if(NULL == pmsg)
    {
        return 0;
    }

    revln = (int)pmsg->msghdr.data_len + IPC_HEADER_LEN_N; 

    if(revln <= imlen)
    {
        /*handle messages*/
        //if (MODULE_ID_DEVM == pmsg->msghdr.module_id || MODULE_ID_HA == pmsg->msghdr.module_id)
        {        
            switch(pmsg->msghdr.msg_type)
            {
                case IPC_TYPE_DEVM:
                    handle_devm_msg (pmsg);
                    break;

                case IPC_TYPE_HA:
                	devm_handle_ha_msg (pmsg);
                	break;

                default:
                    printf("devm_msg_rcv, receive unknown message\r\n");
                    break;
            }
        }
    }
    else
    {
         printf("devm recv msg: datalen error, data_len=%d, msgrcv len = %d\n", revln, imlen);
    }

    mem_share_free(pmsg, MODULE_ID_DEVM);    
    return retva;
}

int devm_sendto_hal_and_wait_ack(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
        int ret = 0;
        struct ipc_mesg_n* pMsgRep = NULL;

        pMsgRep = ipc_sync_send_n2( pdata , data_len, data_num, module_id, sender_id,
        				msg_type, subtype, opcode, msg_index, 0);
        if (NULL == pMsgRep)
        {
            zlog_err ( "send sync message fail.\n" );
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

        mem_share_free(pMsgRep, MODULE_ID_DEVM);

        return ret;
}



