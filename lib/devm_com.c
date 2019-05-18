#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <lib/log.h>
#include "memory.h"
#include "memtypes.h"
#include "command.h"
#include "linklist.h"
#include "msg_ipc.h"
#include "errcode.h"
#include "devm_com.h"
#include <lib/module_id.h>
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>


/*hh device oid info*/
struct devm_oid sysoid[] = 
{
    {ID_HT201,       11, {1,3,6,1,4,1,9966,2,20,201,1028} },
    {ID_HT201E,      11, {1,3,6,1,4,1,9966,2,20,201,1029} },
    {ID_HT2200,      11, {1,3,6,1,4,1,9966,2,20,201,1520} },
    {ID_HT2100,      11, {1,3,6,1,4,1,9966,2,20,201,1521} },
    {ID_H9MOLMXE_VX, 11, {1,3,6,1,4,1,9966,2,20,201,1522} },
    {ID_HT2200V2,    11, {1,3,6,1,4,1,9966,2,20,201,1523} },
    {ID_HT2100V2,    11, {1,3,6,1,4,1,9966,2,20,201,1524} },    
    {ID_HT157,       11, {1,3,6,1,4,1,9966,2,201,1,1}     },
    {ID_HT158,       11, {1,3,6,1,4,1,9966,2,20,201,1028} }
};

char *pDigtLabl[] = {"hwcfg", "mac", "oem", "vnd", "bom", "dev", "date", "hver", "dver", "sn", "lic", "cfginit",};
/* app call this function to register device event *//*unitid &slotid start from 1*/
/* eg: common queue : queue_id = IPC_MSG_COMMON*/
int devm_event_register(enum DEV_EVENT type, int module_id, uint8_t queue_id)
{
    uint8_t t_queue_id = queue_id;
#if 0
    if (ipc_common_id < 0)
    {
        ipc_connect_common();
    }
#endif
//    ipc_send_common(&t_queue_id, sizeof(uint8_t), 1, MODULE_ID_DEVM, module_id, IPC_TYPE_DEVM, type, IPC_OPCODE_REGISTER);
    ipc_send_msg_n2(&t_queue_id, sizeof(uint8_t), 1, MODULE_ID_DEVM, module_id, IPC_TYPE_DEVM, type, IPC_OPCODE_REGISTER, 0);
    return ERRNO_SUCCESS;
}


#if 1
#endif


/* app call this function to get device or mainboard mac*/
/*unit start from 1, slot 0 means backplane mac*/
int devm_comm_get_mac(uint8_t unit, uint8_t slot, int module_id, uchar *mac)
{
    int ret = 0;
    enum DEV_TYPE dev_type = DEV_TYPE_INVALID;
    struct devm_slot t_devm_slot;
    struct devm_unit t_devm_unit;
    struct devm_box t_devm_box;

    if (NULL == mac)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para, NULL == mac\n", __FILE__, __LINE__, __func__);
        return 1;
    }

    if (slot > SLOT_NUM_MAX || unit == 0 || unit > UNIT_NUM_MAX  || module_id <= 0)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para,slot %d not (%d~%d) or unit %d not (%d~%d) or module_id %d not (%d~max)\n", __FILE__, __LINE__, __func__
                 , slot, 0, SLOT_NUM_MAX, unit, 1, UNIT_NUM_MAX, module_id, 1);
        return 1;
    }

    ret = devm_comm_get_dev_type(module_id, &dev_type);

    if (0 == ret)
    {
        if (DEV_TYPE_BOX == dev_type)
        {
            memset(&t_devm_box, 0, sizeof(struct devm_box));
            ret = devm_comm_get_box(module_id, &t_devm_box);

            if (0 == ret)
            {
                memcpy(mac, t_devm_box.mac, 6);
                zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get box,mac  from devm :%02x:%02x:%02x:%02x:%02x:%02x.\n", __FILE__, __LINE__, __func__, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
                return 0;
            }
            else
            {
                zlog_err("%s[%d]:leave %s:devm_comm_get_box error\n", __FILE__, __LINE__, __func__);
                return 1;
            }
        }
        else if (DEV_TYPE_SHELF == dev_type)
        {
            if (0 != slot)
            {
                memset(&t_devm_slot, 0, sizeof(struct devm_slot));
                ret = devm_comm_get_slot(unit , slot, module_id, &t_devm_slot);

                if (0 != ret)
                {
                    zlog_err("%s[%d]:leave %s:devm_comm_get_mac error,unit=%d,slot=%d,mac == NULL\n", __FILE__, __LINE__, __func__, unit, slot);
                    return 1;
                }
                else
                {
                    memcpy(mac, t_devm_slot.mac, 6);
                    zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get unit=%d,slot=%d,mac  from devm :%02x:%02x:%02x:%02x:%02x:%02x.\n", __FILE__, __LINE__, __func__, unit, slot, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
                    return 0;
                }
            }
            else
            {
                memset(&t_devm_unit, 0, sizeof(struct devm_unit));
                ret = devm_comm_get_unit(unit, module_id, &t_devm_unit);

                if (0 != ret)
                {
                    zlog_err("%s[%d]:leave %s:devm_comm_get_mac error,unit=%d,slot=%d,mac == NULL\n", __FILE__, __LINE__, __func__, unit, slot);
                    return 1;
                }
                else
                {
                    memcpy(mac, t_devm_unit.bplane.mac, 6);
                    zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get unit=%d,mac  from devm :%02x:%02x:%02x:%02x:%02x:%02x.\n", __FILE__, __LINE__, __func__, unit, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
                    return 0;
                }
            }
        }
    }
    else
    {
        zlog_err("%s[%d]:leave %s:devm_comm_get_dev_type error,dev_type = %d\n", __FILE__, __LINE__, __func__, dev_type);
        ret = 1;
    }

    return ret;
}


/* app call this function to get device oid or slot id */
/*unit start from 1,slot 0 means dev id*/
int devm_comm_get_id(uint8_t unit, uint8_t slot, int module_id, unsigned int *para_dev_id)
{
    int ret = 0;
    enum DEV_TYPE dev_type = DEV_TYPE_INVALID;
    struct devm_slot t_devm_slot;
    struct devm_unit t_myunit;
    struct devm_box t_devm_box;

    if (NULL == para_dev_id)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para\n", __FILE__, __LINE__, __func__);
        return 1;
    }

    if (slot > SLOT_NUM_MAX || unit == 0 || unit > UNIT_NUM_MAX || module_id <= 0)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para,slot %d not (%d~%d) or unit %d not (%d~%d) or module_id %d not (%d~max)\n", __FILE__, __LINE__, __func__
                 , slot, 0, SLOT_NUM_MAX, unit, 1, UNIT_NUM_MAX, module_id, 1);
        return 1;
    }

    ret = devm_comm_get_dev_type(module_id, &dev_type);

    if (0 == ret)
    {
        if (DEV_TYPE_BOX == dev_type)
        {
            memset(&t_devm_box, 0, sizeof(struct devm_box));
            ret = devm_comm_get_box(module_id, &t_devm_box);

            if (0 == ret)
            {
                *para_dev_id = t_devm_box.id;
                zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get id  from box devm id=%#x\n", __FILE__, __LINE__, __func__, *para_dev_id);
                return 0;
            }
            else
            {
                zlog_err("%s[%d]:leave %s:devm_comm_get_box error\n", __FILE__, __LINE__, __func__);
                return 1;
            }
        }
        else if (DEV_TYPE_SHELF == dev_type)
        {
            if (0 == slot)
            {
                memset(&t_myunit, 0, sizeof(struct devm_unit));
                ret = devm_comm_get_myunit_info(module_id, &t_myunit);

                if (0 != ret)
                {
                    zlog_err("%s[%d]:leave %s:devm_comm_get_id error,unit=%d,slot=%d\n", __FILE__, __LINE__, __func__, unit, slot);
                    return 1;
                }
                else
                {
                    *para_dev_id = t_myunit.bplane.id;
                    zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get id  from devm ,unit=%d,slot=%d:id=%#x\n", __FILE__, __LINE__, __func__, unit, slot, *para_dev_id);
                    return 0;
                }
            }
            else
            {
                memset(&t_devm_slot, 0, sizeof(struct devm_slot));
                ret = devm_comm_get_slot(unit , slot, module_id, &t_devm_slot);

                if (0 != ret)
                {
                    zlog_err("%s[%d]:leave %s:devm_comm_get_id error,unit=%d,slot=%d\n", __FILE__, __LINE__, __func__, unit, slot);
                    return 1;
                }
                else
                {
                    *para_dev_id = t_devm_slot.id;
                    zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get id  from devm ,unit=%d,slot=%d:id=%#x\n", __FILE__, __LINE__, __func__, unit, slot, *para_dev_id);
                    return 0;
                }
            }
        }
    }
    else
    {
        zlog_err("%s[%d]:leave %s:devm_comm_get_dev_type error,dev_type = %d\n", __FILE__, __LINE__, __func__, dev_type);
        ret = 1;
    }

    return ret;
}


int devm_comm_get_running_time(unsigned char unit, unsigned char slot, unsigned int module_id, time_t *current_secs)
{
    int               iResult = 0;
    int               iRevLen = 0;

    struct ipc_mesg_n *pSndMsg = NULL;
    struct ipc_mesg_n *pRcvMsg = NULL;
 
    if (unit == 0 || unit > UNIT_NUM_MAX)
    {
        zlog_err("%s[%d]:leave %s:invalid para, unit(%d) range is (%d~%d)\n", __FILE__, __LINE__, __func__, unit, 1, UNIT_NUM_MAX);
        return 1;
    }

    if (slot == 0 || slot > SLOT_NUM_MAX)
    {
        zlog_err("%s[%d]:leave %s:invalid para, slot(%d) range is (%d~%d)\n", __FILE__, __LINE__, __func__, slot, 1, UNIT_NUM_MAX);
        return 1;
    }

    if (0 == module_id)
    {
        zlog_err("%s[%d]:leave %s:invlaid para , 0 == module_id\n", __FILE__, __LINE__, __func__);
        return 1;
    }

    if (NULL == current_secs)
    {
        zlog_err("%s[%d]:leave %s:invlaid para , NULL == current_secs\n", __FILE__, __LINE__, __func__);
        return 1;
    }

    pSndMsg =  ipc_msg_malloc(sizeof(struct ipc_mesg_n), module_id);
    
    if (NULL == pSndMsg)
    {
       //zlog_debug("%s[%d]:malloc share memory\r\n", __FILE__, __LINE__);
        return -1;
    }     
    
    memset(pSndMsg, 0, sizeof(struct ipc_mesg_n));
    
    pSndMsg->msghdr.module_id = MODULE_ID_HAL;
    pSndMsg->msghdr.sender_id = module_id;
    pSndMsg->msghdr.msg_type  = IPC_TYPE_DEVM;
    pSndMsg->msghdr.msg_subtype  = DEVM_INFO_RUNNINGTIME;
    pSndMsg->msghdr.data_len    = 0;
    pSndMsg->msghdr.data_num  = 1;
    pSndMsg->msghdr.opcode  = IPC_OPCODE_GET;
    pSndMsg->msghdr.sequence  = 0;
    pSndMsg->msghdr.unit  = unit;
    pSndMsg->msghdr.slot  = slot;
    pSndMsg->msghdr.msg_index = unit << 8 |slot;

    iResult = ipc_sync_send_n1(pSndMsg, sizeof(struct ipc_mesg_n), &pRcvMsg, &iRevLen, 2000);

    
    if (iResult == -1)
    {
        ipc_msg_free(pSndMsg, module_id);
        return -1;
    }
    else if(iResult == -2)
    {
         return -1;
    }
    else  
    {
        if(pRcvMsg != NULL)
        {
             *current_secs = *((time_t *)pRcvMsg->msg_data);             
             ipc_msg_free(pRcvMsg,module_id);
             return(0);
        }
        else return(-1);
    }

}


/*para_time is 64 byte Array first address*/
/*unit start from 1,slot start from 0, slot 0 means mainslot running time*/
int devm_comm_get_runtime(uint8_t unit, uint8_t slot, int module_id, unsigned int *para_time)
{
    int ret = 0;
    unsigned char mainslot = 0;
    enum DEV_TYPE dev_type = DEV_TYPE_INVALID;
    struct devm_slot t_devm_slot;
    struct devm_unit t_myunit;
    struct devm_box t_devm_box;
    //uint32_t t_run_time;
    time_t current_secs;

    if (NULL == para_time)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para, NULL	== para_time\n", __FILE__, __LINE__, __func__);
        return 1;
    }

    if (slot > SLOT_NUM_MAX || unit == 0 || unit > UNIT_NUM_MAX || module_id <= 0)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para,slot %d not (%d~%d) or unit %d not (%d~%d) or module_id %d not (%d~max)\n", __FILE__, __LINE__, __func__
                 , slot, 0, SLOT_NUM_MAX, unit, 1, UNIT_NUM_MAX, module_id, 1);
        return 1;
    }

    ret = devm_comm_get_dev_type(module_id, &dev_type);

    if (0 == ret)
    {
        if (DEV_TYPE_BOX == dev_type)
        {
            memset(&t_devm_box, 0, sizeof(struct devm_box));
            ret = devm_comm_get_box(module_id, &t_devm_box);

            if (0 == ret)
            {
                devm_comm_get_running_time(1, 1, module_id, &current_secs);

                if (current_secs < t_devm_box.run_time)
                {
                    zlog_err("%s[%d]:leave %s: current_secs(0x%lx) is less than start_time(0x%lx), fail to get running time\n", __FILE__, __LINE__, __func__
                             , current_secs, t_devm_box.run_time);
                    ret = 1;
                }
                else
                {
                    current_secs -= (time_t)t_devm_box.run_time;
                    *para_time = current_secs;

                    zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully to get box running time = 0x%x, current_secs=0x%lx start_time=0x%lx\n", __FILE__, __LINE__, __func__
                               , *para_time, current_secs, t_devm_slot.run_time);
                    ret = 0;
                }
            }
            else
            {
                zlog_err("%s[%d]:leave %s:devm_comm_get_box error\n", __FILE__, __LINE__, __func__);
                ret = 1;
            }
        }
        else if (DEV_TYPE_SHELF == dev_type)
        {
            if (0 == slot)
            {
                memset(&t_myunit, 0, sizeof(struct devm_unit));
                ret = devm_comm_get_myunit_info(module_id, &t_myunit);

                if (0 != ret)
                {
                    zlog_err("%s[%d]:leave %s:devm_comm_get_myunit_info error\n", __FILE__, __LINE__, __func__);
                    ret = 1;
                }
                else
                {
                    mainslot = t_myunit.slot_main_board;
                    memset(&t_devm_slot, 0, sizeof(struct devm_slot));
                    ret = devm_comm_get_slot(unit , mainslot, module_id, &t_devm_slot);

                    if (0 != ret)
                    {
                        zlog_err("%s[%d]:leave %s:devm_comm_get_slot error,unit=%d,slot=%d\n", __FILE__, __LINE__, __func__, unit, slot);
                        ret = 1;
                    }
                    else
                    {
                        devm_comm_get_running_time(unit, mainslot, module_id, &current_secs);

                        if (current_secs < t_devm_slot.run_time)
                        {
                            zlog_err("%s[%d]:leave %s: current_secs(0x%lx) is less than start_time(0x%lx), fail to get running time\n", __FILE__, __LINE__, __func__
                                     , current_secs, t_devm_slot.run_time);
                            ret = 1;
                        }
                        else
                        {
                            current_secs -= t_devm_slot.run_time;
                            *para_time = current_secs;

                            zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully to get slot(%d,%d) running time = 0x%x, current_secs=0x%lx start_time=0x%lx\n", __FILE__, __LINE__, __func__
                                       , unit, slot, *para_time, current_secs, t_devm_slot.run_time);
                            ret = 0;
                        }
                    }
                }
            }
            else
            {
                memset(&t_devm_slot, 0, sizeof(struct devm_slot));
                ret = devm_comm_get_slot(unit , slot, module_id, &t_devm_slot);

                if (0 != ret)
                {
                    zlog_err("%s[%d]:leave %s:devm_comm_get_runtime error,unit=%d,slot=%d\n", __FILE__, __LINE__, __func__, unit, slot);
                    ret = 1;
                }
                else
                {
                    devm_comm_get_running_time(unit, slot, module_id, &current_secs);
                    current_secs -= t_devm_slot.run_time;
                    *para_time = current_secs;
                    zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get runningtime:%d,unit=%d,slot=%d\n", __FILE__, __LINE__, __func__, *para_time, unit, slot);
                    ret = 0;
                }
            }
        }
    }
    else
    {
        zlog_err("%s[%d]:leave %s:devm_comm_get_dev_type error,dev_type = %d\n", __FILE__, __LINE__, __func__, dev_type);
        ret = 1;
    }

    return ret;
}


/*get oem name from backplane or slot*/
/*unit start from 1,slot start from 0, slot 0 means backplane oem name*/
int devm_comm_get_oem_name(uint8_t unit, uint8_t slot, int module_id, uchar *para_oem_name)
{
    int ret = 0;
    enum DEV_TYPE dev_type = DEV_TYPE_INVALID;
    struct devm_slot t_devm_slot;
    struct devm_unit t_myunit;
    struct devm_box t_devm_box;

    if (NULL == para_oem_name)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para, NULL == para_oem_name\n", __FILE__, __LINE__, __func__);
        return 1;
    }

    if (slot > SLOT_NUM_MAX || unit == 0 || unit > UNIT_NUM_MAX || module_id <= 0)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para,slot %d not (%d~%d) or unit %d not (%d~%d) or module_id %d not (%d~max)\n", __FILE__, __LINE__, __func__
                 , slot, 0, SLOT_NUM_MAX, unit, 1, UNIT_NUM_MAX, module_id, 1);
        return 1;
    }

    ret = devm_comm_get_dev_type(module_id, &dev_type);

    if (0 == ret)
    {
        if (DEV_TYPE_BOX == dev_type)
        {
            memset(&t_devm_box, 0, sizeof(struct devm_box));
            ret = devm_comm_get_box(module_id, &t_devm_box);

            if (0 == ret)
            {
                memcpy(para_oem_name, &t_devm_box.oem_name, NAME_STRING_LEN);
                zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get oem_name  from box devm,dev_name = %s\n", __FILE__, __LINE__, __func__, para_oem_name);
                return 0;
            }
            else
            {
                zlog_err("%s[%d]:leave %s:devm_comm_get_box error\n", __FILE__, __LINE__, __func__);
                return 1;
            }
        }
        else if (DEV_TYPE_SHELF == dev_type)
        {
            if (0 == slot)
            {
                memset(&t_myunit, 0, sizeof(struct devm_unit));
                ret = devm_comm_get_myunit_info(module_id, &t_myunit);

                if (0 != ret)
                {
                    zlog_err("%s[%d]:leave %s:devm_comm_get_dev_name error,name == NULL,unit=%d,slot=%d\n", __FILE__, __LINE__, __func__, unit, slot);
                    return 1;
                }
                else
                {
                    memcpy(para_oem_name, &t_myunit.bplane.oem_name, NAME_STRING_LEN);
                    zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get dev_name  from devm,oem_name = %s,unit=%d,slot=%d\n", __FILE__, __LINE__, __func__, para_oem_name, unit, slot);
                    return 0;
                }
            }
            else
            {
                memset(&t_devm_slot, 0, sizeof(struct devm_slot));
                ret = devm_comm_get_slot(unit , slot, module_id, &t_devm_slot);

                if (0 != ret)
                {
                    zlog_err("%s[%d]:leave %s:devm_comm_get_dev_name error,name == NULL,unit=%d,slot=%d\n", __FILE__, __LINE__, __func__, unit, slot);
                    return 1;
                }
                else
                {
                    memcpy(para_oem_name, t_devm_slot.dev_name, NAME_STRING_LEN);
                    zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get dev_name  from devm,dev_name = %s,unit=%d,slot=%d\n", __FILE__, __LINE__, __func__, para_oem_name, unit, slot);
                    return 0;
                }
            }
        }
    }
    else
    {
        zlog_err("%s[%d]:leave %s:devm_comm_get_dev_type error,dev_type = %d\n", __FILE__, __LINE__, __func__, dev_type);
        ret = 1;
    }

    return ret;
}


/*get dev name from backplane or slot*/
/*unit start from 1,slot start from 0, slot 0 means backplane dev name*/
int devm_comm_get_dev_name(uint8_t unit, uint8_t slot, int module_id, uchar *para_dev_name)
{
    int ret = 0;
    enum DEV_TYPE dev_type = DEV_TYPE_INVALID;
    struct devm_slot t_devm_slot;
    struct devm_unit t_myunit;
    struct devm_box t_devm_box;

    if (NULL == para_dev_name)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para, NULL == para_dev_name\n", __FILE__, __LINE__, __func__);
        return 1;
    }

    if (slot > SLOT_NUM_MAX || unit == 0 || unit > UNIT_NUM_MAX || module_id <= 0)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para,slot %d not (%d~%d) or unit %d not (%d~%d) or module_id %d not (%d~max)\n", __FILE__, __LINE__, __func__
                 , slot, 0, SLOT_NUM_MAX, unit, 1, UNIT_NUM_MAX, module_id, 1);
        return 1;
    }

    ret = devm_comm_get_dev_type(module_id, &dev_type);

    if (0 == ret)
    {
        if (DEV_TYPE_BOX == dev_type)
        {
            memset(&t_devm_box, 0, sizeof(struct devm_box));
            ret = devm_comm_get_box(module_id, &t_devm_box);

            if (0 == ret)
            {
                if('\0' != t_devm_box.oem_name[0])
                {
                    memcpy(para_dev_name, t_devm_box.oem_name, NAME_STRING_LEN);
                } 
                else
                {
                    if('\0' != t_devm_box.dev_name[0])
                    {
                        memcpy(para_dev_name, t_devm_box.dev_name, NAME_STRING_LEN);
                    }
                    else
                    {
                        memcpy(para_dev_name, "undefined", NAME_STRING_LEN);
                    }
                }
                zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get dev_name  from box devm,dev_name = %s\n", __FILE__, __LINE__, __func__, para_dev_name);
                return 0;
            }
            else
            {
                zlog_err("%s[%d]:leave %s:devm_comm_get_box error\n", __FILE__, __LINE__, __func__);
                return 1;
            }
        }
        else if (DEV_TYPE_SHELF == dev_type)
        {
            if (0 == slot)
            {
                memset(&t_myunit, 0, sizeof(struct devm_unit));
                ret = devm_comm_get_myunit_info(module_id, &t_myunit);

                if (0 != ret)
                {
                    zlog_err("%s[%d]:leave %s:devm_comm_get_dev_name error,name == NULL,unit=%d,slot=%d\n", __FILE__, __LINE__, __func__, unit, slot);
                    return 1;
                }
                else
                {
                    if('\0' != t_myunit.bplane.oem_name[0])
                    {
                        memcpy(para_dev_name, t_myunit.bplane.oem_name, NAME_STRING_LEN);
                    } 
                    else
                    {
                        if('\0' != t_myunit.bplane.dev_name[0])
                        {
                            memcpy(para_dev_name, t_myunit.bplane.dev_name, NAME_STRING_LEN);
                        }
                        else
                        {
                            memcpy(para_dev_name, "undefined", NAME_STRING_LEN);
                        }
                    }                
                    zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get dev_name  from devm,dev_name = %s,unit=%d,slot=%d\n", __FILE__, __LINE__, __func__, para_dev_name, unit, slot);
                    return 0;
                }
            }
            else
            {
                memset(&t_devm_slot, 0, sizeof(struct devm_slot));
                ret = devm_comm_get_slot(unit , slot, module_id, &t_devm_slot);

                if (0 != ret)
                {
                    zlog_err("%s[%d]:leave %s:devm_comm_get_dev_name error,name == NULL,unit=%d,slot=%d\n", __FILE__, __LINE__, __func__, unit, slot);
                    return 1;
                }
                else
                {
                    memcpy(para_dev_name, t_devm_slot.dev_name, NAME_STRING_LEN);
                    zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get dev_name  from devm,dev_name = %s,unit=%d,slot=%d\n", __FILE__, __LINE__, __func__, para_dev_name, unit, slot);
                    return 0;
                }
            }
        }
    }
    else
    {
        zlog_err("%s[%d]:leave %s:devm_comm_get_dev_type error,dev_type = %d\n", __FILE__, __LINE__, __func__, dev_type);
        ret = 1;
    }

    return ret;
}



/*unit &slot start from 1*/
int devm_comm_get_slot_name(uint8_t unit, uint8_t slot, int module_id, uchar *para_slot_name)
{
    int ret = 0;
    struct devm_slot t_devm_slot;

    if (NULL == para_slot_name)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para, NULL == para_slot_name\n", __FILE__, __LINE__, __func__);
        return 1;
    }

    if (slot == 0 || slot > SLOT_NUM_MAX || unit == 0 || unit > UNIT_NUM_MAX || module_id <= 0)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para,slot %d not (%d~%d) or unit %d not (%d~%d) or module_id %d not (%d~max)\n", __FILE__, __LINE__, __func__
                 , slot, 1, SLOT_NUM_MAX, unit, 1, UNIT_NUM_MAX, module_id, 1);
        return 1;
    }

    memset(&t_devm_slot, 0, sizeof(struct devm_slot));
    ret = devm_comm_get_slot(unit , slot, module_id, &t_devm_slot);

    if (0 != ret)
    {
        zlog_err("%s[%d]:leave %s:devm_comm_get_slot_name error,name == NULL,unit=%d,slot=%d\n", __FILE__, __LINE__, __func__, unit, slot);
        return 1;
    }
    else
    {
        memcpy(para_slot_name, t_devm_slot.name, NAME_STRING_LEN);
        zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get devm_comm_get_slot_name  from devm,dev_name = %s,unit=%d,slot=%d\n", __FILE__, __LINE__, __func__, para_slot_name, unit, slot);
        return 0;
    }
}


/*unit &slot start from 1*/
int devm_comm_get_slot_status(uint8_t unit, uint8_t slot, int module_id, enum DEV_STATUS *slot_status)
{
    int ret = 0;
    struct devm t_devm;
    struct devm_slot t_devm_slot;

    if (slot == 0 || slot > SLOT_NUM_MAX || unit == 0 || unit > UNIT_NUM_MAX || module_id <= 0)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para,slot %d not (%d~%d) or unit %d not (%d~%d) or module_id %d not (%d~max)\n", __FILE__, __LINE__, __func__
                 , slot, 1, SLOT_NUM_MAX, unit, 1, UNIT_NUM_MAX, module_id, 1);
        return -1;
    }

    memset(&t_devm, 0, sizeof(struct devm));
    ret = devm_comm_get_mydevice_info(module_id, &t_devm);

    if (0 == ret)
    {
        if (DEV_TYPE_BOX == t_devm.type)
        {
            if (1 == slot)
            {
                *slot_status = DEV_STATUS_WORK;
                return 0;
            }
            else
            {
                return 1;
            }
        }
        else if (DEV_TYPE_SHELF == t_devm.type)
        {
            memset(&t_devm_slot, 0, sizeof(struct devm_slot));
            ret = devm_comm_get_slot(unit , slot, module_id, &t_devm_slot);

            if (0 != ret)
            {
                zlog_err("%s[%d]:leave %s:devm_comm_get_slot error,unit=%d,slot=%d\n", __FILE__, __LINE__, __func__, unit, slot);
                return 1;
            }
            else
            {
                *slot_status = t_devm_slot.status;
                zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get devm_comm_get_slot from devm,slot_status = %d,unit=%d,slot=%d\n", __FILE__, __LINE__, __func__, *slot_status, unit, slot);
                return 0;
            }
        }
        else
        {
            zlog_err("%s[%d]:leave %s:devm_comm_get_slot_name error,dev type (%d) error\n", __FILE__, __LINE__, __func__, t_devm.type);
            return 1;
        }
    }
    else
    {
        zlog_err("%s[%d]:leave %s:devm_comm_get_slot_name error,unit=%d,slot=%d\n", __FILE__, __LINE__, __func__, unit, slot);
        return 1;
    }
}


/*unit start from 1*/
int devm_comm_get_manufacture(uint8_t unit, int module_id, uchar *para_manu)
{
    int ret = 0;
    struct devm_unit t_myunit;
    struct devm_box t_devm_box;
    enum DEV_TYPE dev_type = DEV_TYPE_INVALID;

    if (NULL == para_manu)
    {
        zlog_err("%s[%d]:leave %s:error:invalid input para, NULL == para_manu\n", __FILE__, __LINE__, __func__);
        return 1;
    }

    if (unit == 0 || unit > UNIT_NUM_MAX || module_id <= 0)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para, unit %d not (%d~%d) or module_id %d not (%d~max)\n", __FILE__, __LINE__, __func__
                 , unit, 1, UNIT_NUM_MAX, module_id, 1);
        return 1;
    }

    ret = devm_comm_get_dev_type(module_id, &dev_type);

    if (0 == ret)
    {
        if (DEV_TYPE_BOX == dev_type)
        {
            memset(&t_devm_box, 0, sizeof(struct devm_box));
            ret = devm_comm_get_box(module_id, &t_devm_box);

            if (0 == ret)
            {
                memcpy(para_manu, &t_devm_box.manufacture, NAME_STRING_LEN);
                zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get box manufacture  from devm, manufacture = %s\n", __FILE__, __LINE__, __func__, para_manu);
                return 0;
            }
            else
            {
                zlog_err("%s[%d]:leave %s:devm_comm_get_box error\n", __FILE__, __LINE__, __func__);
                return 1;
            }
        }
        else if (DEV_TYPE_SHELF == dev_type)
        {
            memset(&t_myunit, 0, sizeof(struct devm_unit));
            ret = devm_comm_get_unit(unit, module_id, &t_myunit);

            if (0 != ret)
            {
                zlog_err("%s[%d]:leave %s:devm_comm_get_manufacture error,manufacture == NULL,unit=%d\n", __FILE__, __LINE__, __func__, unit);
                return 1;
            }
            else
            {
                memcpy(para_manu, &t_myunit.bplane.manufacture, NAME_STRING_LEN);
                zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get manufacture  from devm, manufacture = %s,unit=%d\n", __FILE__, __LINE__, __func__, t_myunit.bplane.manufacture, unit);
                return 0;
            }
        }
    }
    else
    {
        zlog_err("%s[%d]:leave %s:devm_comm_get_dev_type error,dev_type = %d\n", __FILE__, __LINE__, __func__, dev_type);
        ret = 1;
    }

    return ret;
}


int devm_comm_get_dev_type(int module_id, enum DEV_TYPE *para_dev_type)
{
    int ret = 0;
    struct devm t_devm;

    if (module_id <= 0)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para,  module_id %d not (%d~max)\n", __FILE__, __LINE__, __func__, module_id, 1);
        return 1;
    }

    if (NULL == para_dev_type)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para , NULL == para_dev_type\n", __FILE__, __LINE__, __func__);
        return 1;
    }

    memset(&t_devm, 0, sizeof(struct devm));
    ret = devm_comm_get_mydevice_info(module_id, &t_devm);

    if (0 == ret)
    {
        *para_dev_type = t_devm.type;
        zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get dev_type  from devm, dev_type = %d\n", __FILE__, __LINE__, __func__, *para_dev_type);
        return 0;
    }
    else
    {
        zlog_err("%s[%d]:leave %s:devm_comm_get_manufacture error,manufacture == NULL\n", __FILE__, __LINE__, __func__);
        return 1;
    }
}


unsigned char devm_comm_get_mainslot(int module_id)
{
    int ret = 0;
    struct devm_unit t_devm_unit;
    enum DEV_TYPE dev_type = DEV_TYPE_INVALID;

    if (module_id <= 0)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para,  module_id %d not (%d~max)\n", __FILE__, __LINE__, __func__, module_id, 1);
        return 0;
    }

    ret = devm_comm_get_dev_type(module_id, &dev_type);

    if (0 == ret)
    {
        if (DEV_TYPE_BOX == dev_type)
        {
            zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get box mainslot 1\n", __FILE__, __LINE__, __func__);
            return 1;
        }
        else if (DEV_TYPE_SHELF == dev_type)
        {
            memset(&t_devm_unit, 0, sizeof(struct devm_unit));
            ret = devm_comm_get_myunit_info(module_id, &t_devm_unit);

            if (0 == ret)
            {
                zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get mainslot  from devm, mainslot = %d\n", __FILE__, __LINE__, __func__, t_devm_unit.slot_main_board);
                return t_devm_unit.slot_main_board;
            }
            else
            {
                zlog_err("%s[%d]:leave %s:devm_comm_get_mainslot error\n", __FILE__, __LINE__, __func__);
                return 0;
            }
        }
    }
    else
    {
        zlog_err("%s[%d]:leave %s:devm_comm_get_dev_type error,dev_type = %d\n", __FILE__, __LINE__, __func__, dev_type);
        ret = 1;
    }

    return ret;
}


/*unit & slot  start from 1*/
int devm_comm_get_fpga_ver(uint8_t unit, uint8_t slot, uint32_t module_id, int16_t *fpga_ver)
{
    int ret = 0;
    struct devm_slot t_devm_slot;
    struct devm_box t_devm_box;
    enum DEV_TYPE dev_type = DEV_TYPE_INVALID;

    if (NULL == fpga_ver)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para  NULL == fpgaid\n", __FILE__, __LINE__, __func__);
        return 1;
    }

    if (0 == unit || slot < 1 || slot > SLOT_NUM_MAX)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para,slot %d not (%d~%d) or unit %d not (%d~%d) or module_id %d not (%d~max)\n", __FILE__, __LINE__, __func__
                 , slot, 1, SLOT_NUM_MAX, unit, 1, UNIT_NUM_MAX, module_id, 1);
        return 1;
    }

    ret = devm_comm_get_dev_type(module_id, &dev_type);

    if (0 == ret)
    {
        if (DEV_TYPE_BOX == dev_type)
        {
            memset(&t_devm_box, 0, sizeof(struct devm_box));
            ret = devm_comm_get_box(module_id, &t_devm_box);

            if (0 == ret)
            {
                *fpga_ver = t_devm_box.hw.fpga_ver;
                zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get box fpgaid  from devm, dev_type = %d\n", __FILE__, __LINE__, __func__, *fpga_ver);
                return 0;
            }
            else
            {
                zlog_err("%s[%d]:leave %s:devm_comm_get_box error\n", __FILE__, __LINE__, __func__);
                return 1;
            }
        }
        else if (DEV_TYPE_SHELF == dev_type)
        {
            memset(&t_devm_slot, 0, sizeof(struct devm_slot));
            ret = devm_comm_get_slot(unit, slot, module_id, &t_devm_slot);

            if (0 == ret)
            {
                *fpga_ver = t_devm_slot.hw.fpga_ver;
                zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get fpgaid  from devm, dev_type = %d\n", __FILE__, __LINE__, __func__, *fpga_ver);
                return 0;
            }
            else
            {
                zlog_err("%s[%d]:leave %s:devm_comm_get_fpgaid error\n", __FILE__, __LINE__, __func__);
                return 1;
            }
        }
    }
    else
    {
        zlog_err("%s[%d]:leave %s:devm_comm_get_dev_type error,dev_type = %d\n", __FILE__, __LINE__, __func__, dev_type);
        ret = 1;
    }

    return ret;
}


/*unit & slot  start from 1*/
int devm_comm_get_cpu(uint32_t module_id, uint8_t unit, uint8_t slot, int *cpu_type)
{
    int ret = 0;
    struct devm_slot t_devm_slot;
    struct devm_box t_devm_box;
    enum DEV_TYPE dev_type = DEV_TYPE_INVALID;

    if (0 == unit || slot < 1 || slot > SLOT_NUM_MAX)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para,slot %d not (%d~%d) or unit %d not (%d~%d) or module_id %d not (%d~max)\n", __FILE__, __LINE__, __func__
                 , slot, 1, SLOT_NUM_MAX, unit, 1, UNIT_NUM_MAX, module_id, 1);
        return 1;
    }

    if (NULL == cpu_type)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para,  NULL == cpu_type\n", __FILE__, __LINE__, __func__);
        return 1;
    }

    ret = devm_comm_get_dev_type(module_id, &dev_type);

    if (0 == ret)
    {
        if (DEV_TYPE_BOX == dev_type)
        {
            memset(&t_devm_box, 0, sizeof(struct devm_box));
            ret = devm_comm_get_box(module_id, &t_devm_box);

            if (0 == ret)
            {
                *cpu_type = t_devm_box.hw.cpu_type;
                zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get cpu_type  from devm, cpu_type = %d\n", __FILE__, __LINE__, __func__, *cpu_type);
                return 0;
            }
            else
            {
                zlog_err("%s[%d]:leave %s:devm_comm_get_box error\n", __FILE__, __LINE__, __func__);
                return 1;
            }
        }
        else if (DEV_TYPE_SHELF == dev_type)
        {
            memset(&t_devm_slot, 0, sizeof(struct devm_slot));
            ret = devm_comm_get_slot(unit, slot, module_id, &t_devm_slot);

            if (0 == ret)
            {
                *cpu_type = t_devm_slot.hw.cpu_type;
                zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get cpu_type  from devm, cpu_type = %d\n", __FILE__, __LINE__, __func__, *cpu_type);
                return 0;
            }
            else
            {
                zlog_err("%s[%d]:leave %s:devm_comm_get_cpu error\n", __FILE__, __LINE__, __func__);
                return 1;
            }
        }
    }
    else
    {
        zlog_err("%s[%d]:leave %s:devm_comm_get_dev_type error,dev_type = %d\n", __FILE__, __LINE__, __func__, dev_type);
        ret = 1;
    }

    return ret;
}


/*unit & slot  start from 1*/
int devm_comm_get_Hardware_Version(uint8_t unit, uint8_t slot, uint32_t module_id, unsigned char *Hw_Ver, unsigned int data_len)
{
    int ret = 0;
    struct devm_slot t_devm_slot;
    struct devm_box t_devm_box;
    enum DEV_TYPE dev_type = DEV_TYPE_INVALID;

    if (0 == unit || slot < 1 || slot > SLOT_NUM_MAX)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para,slot %d not (%d~%d) or unit %d not (%d~%d) or module_id %d not (%d~max)\n", __FILE__, __LINE__, __func__
                 , slot, 1, SLOT_NUM_MAX, unit, 1, UNIT_NUM_MAX, module_id, 1);
        return 1;
    }

    if (NULL == Hw_Ver)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para,  NULL == Hw_Ver\n", __FILE__, __LINE__, __func__);
        return 1;
    }

    if (0 == data_len)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para,  0 == data_len\n", __FILE__, __LINE__, __func__);
        return 1;
    }

    ret = devm_comm_get_dev_type(module_id, &dev_type);

    if (0 == ret)
    {
        if (DEV_TYPE_BOX == dev_type)
        {
            memset(&t_devm_box, 0, sizeof(struct devm_box));
            ret = devm_comm_get_box(module_id, &t_devm_box);

            if (0 == ret)
            {
                if (data_len >= strlen((char *)t_devm_box.hw.hw_ver))
                {
                    memcpy(Hw_Ver, t_devm_box.hw.hw_ver, strlen((char *)t_devm_box.hw.hw_ver));
                    zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get hw_ver  from devm, t_devm_box.hw.hw_ver = %s, hw_ver = %s, data_len = %d, strlen(t_devm_box.hw.hw_ver) = %d\n", __FILE__, __LINE__, __func__, t_devm_box.hw.hw_ver, Hw_Ver, data_len, strlen((char *)t_devm_box.hw.hw_ver));
                    return 0;
                }
                else
                {
                    memcpy(Hw_Ver, t_devm_box.hw.hw_ver, data_len);
                    zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get hw_ver  from devm, t_devm_box.hw.hw_ver = %s, hw_ver = %s, data_len = %d, strlen(t_devm_box.hw.hw_ver) = %d\n", __FILE__, __LINE__, __func__, t_devm_box.hw.hw_ver, Hw_Ver, data_len, strlen((char *)t_devm_box.hw.hw_ver));
                    return 0;
                }
            }
            else
            {
                zlog_err("%s[%d]:leave %s:devm_comm_get_box error\n", __FILE__, __LINE__, __func__);
                return 1;
            }
        }
        else if (DEV_TYPE_SHELF == dev_type)
        {
            memset(&t_devm_slot, 0, sizeof(struct devm_slot));
            ret = devm_comm_get_slot(unit, slot, module_id, &t_devm_slot);

            if (0 == ret)
            {
                if (data_len >= strlen((char *)t_devm_box.hw.hw_ver))
                {
                    memcpy(Hw_Ver, t_devm_slot.hw.hw_ver, strlen((char *)t_devm_slot.hw.hw_ver));
                    zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get hw_ver  from devm, t_devm_slot.hw.hw_ver = %s, hw_ver = %s,data_len = %d, strlen(t_devm_slot.hw.hw_ver) = %d\n", __FILE__, __LINE__, __func__, t_devm_slot.hw.hw_ver, Hw_Ver, data_len, strlen((char *)t_devm_slot.hw.hw_ver));
                    return 0;
                }
                else
                {
                    memcpy(Hw_Ver, t_devm_slot.hw.hw_ver, data_len);
                    zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get hw_ver  from devm, t_devm_slot.hw.hw_ver = %s, hw_ver = %s,data_len = %d, strlen(t_devm_slot.hw.hw_ver) = %d\n", __FILE__, __LINE__, __func__, t_devm_slot.hw.hw_ver, Hw_Ver, data_len, strlen((char *)t_devm_slot.hw.hw_ver));
                    return 0;
                }
            }
            else
            {
                zlog_err("%s[%d]:leave %s:devm_comm_get_Hardware_Version error\n", __FILE__, __LINE__, __func__);
                return 1;
            }
        }
    }
    else
    {
        zlog_err("%s[%d]:leave %s:devm_comm_get_Hardware_Version error,dev_type = %d\n", __FILE__, __LINE__, __func__, dev_type);
        ret = 1;
    }

    return ret;
}

devmtemper tempToSnmp;
void *devm_comm_get_dev_scalar(uint32_t module_id, uint32_t sub_type)
{
    int ret = 0;
    struct devm_slot t_devm_slot;
    struct devm_box t_devm_box;
    struct devm_unit t_devm_unit;
    enum DEV_TYPE dev_type = DEV_TYPE_INVALID;

    ret = devm_comm_get_dev_type(module_id, &dev_type);

    if (0 == ret)
    {
        if (DEV_TYPE_BOX == dev_type)
        {
            switch(sub_type)
            {
                case DEVM_INFO_TEMP:
                    memset(&t_devm_box, 0, sizeof(struct devm_box));
                    ret = devm_comm_get_box(module_id, &t_devm_box);

                    if (0 == ret)
                    {
                        //if (data_len >= sizeof(t_devm_box.temp))
                        {
//                            memcpy(temprature, &t_devm_box.temp, sizeof(t_devm_box.temp));
                            tempToSnmp.temp_low = (uint32_t )t_devm_box.temp.temp_low;
                            tempToSnmp.temprature = (uint32_t )t_devm_box.temp.temprature;
                            tempToSnmp.temp_high = (uint32_t )t_devm_box.temp.temp_high;
                            zlog_debug(ZLOG_LIB_DBG_DEVM,"%s[%d]:%s: sucessfully get temprature  from devm, temprature = %d\n", __FILE__, __LINE__, __func__, t_devm_box.temp.temprature);
                            return &tempToSnmp;
                        }
                    }
                    else
                    {                  
                        zlog_err("%s[%d]:leave %s:devm_comm_get_box error\n", __FILE__, __LINE__, __func__);
                    }
                    break;
                default:
                    zlog_err("%s[%d]:leave %s:DEV_TYPE_BOX msg sub_type is error!\n", __FILE__, __LINE__, __func__);
                    break;
            }
        }
        else if (DEV_TYPE_SHELF == dev_type)
        {
            switch(sub_type)
            {
                case DEVM_INFO_TEMP:        
                    memset(&t_devm_unit, 0, sizeof(struct devm_unit));
                    ret = devm_comm_get_unit(1, module_id, &t_devm_unit);

                    if (0 == ret)
                    {
                        memset(&t_devm_slot, 0, sizeof(struct devm_slot));
                        ret = devm_comm_get_slot(1, t_devm_unit.myslot, module_id, &t_devm_slot);

                        if (0 == ret)
                        {
                            //if (data_len >= sizeof(t_devm_slot.temp))
                            {
//                                memcpy(temprature, &t_devm_slot.temp, sizeof(t_devm_slot.temp));
                                tempToSnmp.temp_low = t_devm_slot.temp.temp_low;
                                tempToSnmp.temprature = t_devm_slot.temp.temprature;
                                tempToSnmp.temp_high = t_devm_slot.temp.temp_high;                                
                                zlog_debug(ZLOG_LIB_DBG_DEVM,"%s[%d]:%s: sucessfully get temprature  from devm, temprature = %d\n", __FILE__, __LINE__, __func__, t_devm_slot.temp.temprature);
                                return &tempToSnmp;
                            }
                        }
                        else
                        {                      
                            zlog_err("%s[%d]:leave %s:devm_comm_get_slot error\n", __FILE__, __LINE__, __func__);
                        }
                    }
                    else
                    {
                        zlog_err("%s[%d]:leave %s:devm_comm_get_unit error\n", __FILE__, __LINE__, __func__);
                    }
                    break;
                default:
                    zlog_err("%s[%d]:leave %s:DEV_TYPE_SHELF msg sub_type is error!\n", __FILE__, __LINE__, __func__);
                    break;
            }
        }
    }
    else
    {
        zlog_err("%s[%d]:leave %s:devm_comm_get_dev_temprature error,dev_type = %d\n", __FILE__, __LINE__, __func__, dev_type);
    }

    return NULL;
}


int devm_comm_get_dev_temprature(uint8_t unit, uint32_t module_id, signed char *temprature, unsigned int data_len)
{
    int ret = 0;
    struct devm_slot t_devm_slot;
    struct devm_box t_devm_box;
    struct devm_unit t_devm_unit;
    enum DEV_TYPE dev_type = DEV_TYPE_INVALID;

    if (0 == unit)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para,unit %d not (%d~%d) \n", __FILE__, __LINE__, __func__, unit, 1, UNIT_NUM_MAX);
        return 1;
    }

    if (NULL == temprature)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para,  NULL == temprature\n", __FILE__, __LINE__, __func__);
        return 1;
    }

    if (0 == data_len)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para,  0 == data_len\n", __FILE__, __LINE__, __func__);
        return 1;
    }

    ret = devm_comm_get_dev_type(module_id, &dev_type);

    if (0 == ret)
    {
        if (DEV_TYPE_BOX == dev_type)
        {
            memset(&t_devm_box, 0, sizeof(struct devm_box));
            ret = devm_comm_get_box(module_id, &t_devm_box);

            if (0 == ret)
            {
                if (data_len >= sizeof(t_devm_box.temp.temprature))
                {
                    memcpy(temprature, &t_devm_box.temp.temprature, sizeof(t_devm_box.temp.temprature));
                    zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get temprature  from devm, temprature = %d\n", __FILE__, __LINE__, __func__, t_devm_box.temp.temprature);
                    return 0;
                }
                else
                {
                    zlog_err("%s[%d]:%s: error, get temprature  from devm, data_len(%d) < sizeof(t_devm_box.temp.temprature)(%d)\n", __FILE__, __LINE__, __func__, data_len, sizeof(t_devm_box.temp.temprature));
                    return 1;
                }
            }
            else
            {
                zlog_err("%s[%d]:leave %s:devm_comm_get_box error\n", __FILE__, __LINE__, __func__);
                return 1;
            }
        }
        else if (DEV_TYPE_SHELF == dev_type)
        {
            memset(&t_devm_unit, 0, sizeof(struct devm_unit));
            ret = devm_comm_get_unit(unit, module_id, &t_devm_unit);

            if (0 == ret)
            {
                memset(&t_devm_slot, 0, sizeof(struct devm_slot));
                ret = devm_comm_get_slot(unit, t_devm_unit.myslot, module_id, &t_devm_slot);

                if (0 == ret)
                {
                    if (data_len >= sizeof(t_devm_slot.temp.temprature))
                    {
                        memcpy(temprature, &t_devm_slot.temp.temprature, sizeof(t_devm_slot.temp.temprature));
                        zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get temprature  from devm, temprature = %d\n", __FILE__, __LINE__, __func__, t_devm_slot.temp.temprature);
                        return 0;
                    }
                    else
                    {
                        zlog_err("%s[%d]:%s: error, get temprature  from devm, data_len(%d) < sizeof(t_devm_slot.temp.temprature)(%d)\n", __FILE__, __LINE__, __func__, data_len, sizeof(t_devm_slot.temp.temprature));
                        return 1;
                    }
                }
                else
                {
                    zlog_err("%s[%d]:leave %s:devm_comm_get_slot error\n", __FILE__, __LINE__, __func__);
                    return 1;
                }
            }
            else
            {
                zlog_err("%s[%d]:leave %s:devm_comm_get_unit error\n", __FILE__, __LINE__, __func__);
                return 1;
            }
        }
    }
    else
    {
        zlog_err("%s[%d]:leave %s:devm_comm_get_dev_temprature error,dev_type = %d\n", __FILE__, __LINE__, __func__, dev_type);
        ret = 1;
    }

    return ret;
}


#if 1
#endif


int devm_comm_set_rtc(int module_id, uint8_t unit, time_t rtc_time)
{
    int               iResult = 0;
    int               iRevLen = 0;

    struct ipc_mesg_n *pSndMsg = NULL;
    struct ipc_mesg_n *pRcvMsg = NULL;
 
    if (0 == unit || module_id <= 0 || rtc_time < 0)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para,unit %d not (%d~%d) or module_id %d not (%d~max) or rtc_time %ld not ( 0 ~ max)\n", __FILE__, __LINE__, __func__
              , unit, 1, UNIT_NUM_MAX, module_id, 1 , rtc_time);
        return 1;
    }

    pSndMsg =  ipc_msg_malloc(sizeof(struct ipc_mesg_n), module_id);
    
    if (NULL == pSndMsg)
    {
       //zlog_debug("%s[%d]:malloc share memory\r\n", __FILE__, __LINE__);
        return -1;
    }     
    
    memset(pSndMsg, 0, sizeof(struct ipc_mesg_n));
    
    pSndMsg->msghdr.module_id = MODULE_ID_HAL;
    pSndMsg->msghdr.sender_id = module_id;
    pSndMsg->msghdr.msg_type  = IPC_TYPE_DEVM;
    pSndMsg->msghdr.msg_subtype  = DEVM_INFO_RTC;
    pSndMsg->msghdr.data_num  = 1;
    pSndMsg->msghdr.opcode  = IPC_OPCODE_UPDATE;
    pSndMsg->msghdr.sequence  = 0;
    pSndMsg->msghdr.unit = unit;
    pSndMsg->msghdr.msg_index = unit << 8;
    memcpy(pSndMsg->msg_data, &rtc_time, sizeof(time_t));

    iResult = ipc_sync_send_n1(pSndMsg, sizeof(struct ipc_mesg_n), &pRcvMsg, &iRevLen, 2000);

    
    if (iResult == -1)
    {
        ipc_msg_free(pSndMsg, module_id);
        return -1;
    }
    else if(iResult == -2)
    {
         return -1;
    }
    else  
    {
        if(pRcvMsg != NULL)
        {
             ipc_msg_free(pRcvMsg, module_id);
             return(0);
        }
        else return(-1);
    }
    
}

#if 0
/*pos = 1, mainboard upgrade FPGA
  pos = 0, mainboard not upgrade FPGA*/
int devm_comm_set_fpga_upgrade_register(int module_id, unsigned char pos)
{
    int ret = 0;
    int index = 0;

    index |= 1 << 8 ;

    ret = ipc_send_hal_wait_ack(&pos, sizeof(unsigned char), 1 , MODULE_ID_HAL, module_id, IPC_TYPE_DEVM, DEVM_INFO_FPGA_UPGRADE_REGISTER, IPC_OPCODE_UPDATE, index);

    if (0 != ret)
    {
        zlog_err("%s[%d]:leave %s:devm_comm_set_fpga_upgrade_register failed\n", __FILE__, __LINE__, __func__);
    }
    else
    {
        zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully devm_comm_set_fpga_upgrade_registerm, pos = %d\n", __FILE__, __LINE__, __func__ , pos);
        printf("%s[%d]:%s: sucessfully devm_comm_set_fpga_upgrade_registerm, pos = %d\n", __FILE__, __LINE__, __func__ , pos);
    }

    return ret;
}
#endif

#if 1
#endif


int devm_comm_get_device(int module_id, struct devm_unit *para_devm_unit, struct devm_box *para_devm_box, enum DEV_TYPE *para_dev_type)
{
    int ret = 0;
    enum DEV_TYPE device_type = 0;
    struct devm_unit t_devm_unit;
    struct devm_box t_devm_box;

    if (module_id <= 0)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para,module_id %d not (%d~max)\n", __FILE__, __LINE__, __func__, module_id, 1);
        return 1;
    }

    if (NULL == para_devm_unit || NULL == para_devm_box || NULL == para_dev_type)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para, NULL == para_devm_unit || NULL == para_devm_box || NULL == para_dev_type\n", __FILE__, __LINE__, __func__);
        return 1;
    }

    ret = devm_comm_get_dev_type(module_id, &device_type);

    if (0 == ret)
    {
        switch (device_type)
        {
            case DEV_TYPE_BOX:
                memset(&t_devm_box, 0, sizeof(struct devm_box));
                ret = devm_comm_get_box(module_id, &t_devm_box);

                if (0 == ret)
                {
                    memcpy(para_devm_box, &t_devm_box, sizeof(struct devm_box));
                    *para_dev_type = device_type;
                }

                break;

            case DEV_TYPE_SHELF:
                memset(&t_devm_unit, 0, sizeof(struct devm_unit));
                ret = devm_comm_get_unit(1, module_id, &t_devm_unit);

                if (0 == ret)
                {
                    memcpy(para_devm_unit, &t_devm_unit, sizeof(struct devm_unit));
                    *para_dev_type = device_type;
                }

                break;

            default:
                *para_dev_type = device_type;
                ret = 1;
                break;
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}


int devm_comm_get_box(int module_id, struct devm_box *para_dev_box)
{
    int               iResult = 0;
    int               iRevLen = 0;

    struct ipc_mesg_n *pSndMsg = NULL;
    struct ipc_mesg_n *pRcvMsg = NULL;
 
    if (NULL == para_dev_box)
    {
       //zlog_err("%s[%d]:invalid para , NULL == para_dev_box\n", __FILE__, __LINE__);
        return -1;
    }

    pSndMsg =  ipc_msg_malloc(sizeof(struct ipc_mesg_n), module_id);
    
    if (NULL == pSndMsg)
    {
       //zlog_debug("%s[%d]:malloc share memory\r\n", __FILE__, __LINE__);
        return -1;
    }     
    
    memset(pSndMsg, 0, sizeof(struct ipc_mesg_n));
    
    pSndMsg->msghdr.module_id = MODULE_ID_DEVM;
    pSndMsg->msghdr.sender_id = module_id;
    pSndMsg->msghdr.msg_type  = IPC_TYPE_DEVM;
    pSndMsg->msghdr.msg_subtype  = DEVM_INFO_BOX;
    pSndMsg->msghdr.data_len   = 0;
    pSndMsg->msghdr.data_num  = 1;
    pSndMsg->msghdr.opcode  = IPC_OPCODE_GET;
    pSndMsg->msghdr.sequence  = 0;

    iResult = ipc_sync_send_n1(pSndMsg, sizeof(struct ipc_mesg_n), &pRcvMsg, &iRevLen, 2000);

    
    if (iResult == -1)
    {
        ipc_msg_free(pSndMsg, module_id);

       //zlog_debug("%s[%d]:send message error\n", __FILE__, __LINE__);
        return -1;
    }
    else if(iResult == -2)
    {
         //zlog_debug
         return -1;
    }
    else  
    {
        if(pRcvMsg != NULL)
        {
            switch(pRcvMsg->msghdr.opcode)
            {
                case IPC_OPCODE_ACK:
                case IPC_OPCODE_REPLY:
                    memcpy(para_dev_box, pRcvMsg->msg_data, sizeof(struct devm_box));
                    ipc_msg_free(pRcvMsg, module_id);
                    return 0;                
                    
                case IPC_OPCODE_NACK:
                    ipc_msg_free(pRcvMsg, module_id);                    
                    return -1;
                    
                default:
                    ipc_msg_free(pRcvMsg, module_id);
                    return -1;
            }

        }
        else return(-1);
    }

}



/*unit &slot start from 1*/
int devm_comm_get_unit(uint8_t unit, int module_id, struct devm_unit *para_devm_unit)
{
    int   iResult = -1;
 
    if (NULL != para_devm_unit)
    {
        struct ipc_mesg_n *pRcvMsg = NULL;

        pRcvMsg = ipc_sync_send_n2(NULL, 0, 0, MODULE_ID_DEVM, module_id, IPC_TYPE_DEVM, DEVM_INFO_UNIT,IPC_OPCODE_GET, (unit << 8), 4000);

        if(pRcvMsg != NULL)
        {
            if((pRcvMsg->msghdr.opcode == IPC_OPCODE_ACK) ||
               (pRcvMsg->msghdr.opcode == IPC_OPCODE_REPLY)) 
            {
                iResult = 0;
                memcpy(para_devm_unit, pRcvMsg->msg_data, sizeof(struct devm_unit));
            }

            ipc_msg_free(pRcvMsg, module_id);
        }
    }

    return iResult;
}



/*unit &slot start from 1*/
int devm_comm_get_slot(uint8_t unit, uint8_t slot, int module_id, struct devm_slot *para_devm_slot)
{
    int               iResult = 0;
    int               iRevLen = 0;

    struct ipc_mesg_n *pSndMsg = NULL;
    struct ipc_mesg_n *pRcvMsg = NULL;
 
    if (NULL == para_devm_slot)
    {
       //zlog_err("%s[%d]:invalid para , NULL == para_dev_box\n", __FILE__, __LINE__);
        return -1;
    }

    pSndMsg =  ipc_msg_malloc(sizeof(struct ipc_mesg_n), module_id);
    
    if (NULL == pSndMsg)
    {
       //zlog_debug("%s[%d]:malloc share memory\r\n", __FILE__, __LINE__);
        return -1;
    }     
    
    memset(pSndMsg, 0, sizeof(struct ipc_mesg_n));
    
    pSndMsg->msghdr.module_id = MODULE_ID_DEVM;
    pSndMsg->msghdr.sender_id = module_id;
    pSndMsg->msghdr.msg_type  = IPC_TYPE_DEVM;
    pSndMsg->msghdr.msg_subtype  = DEVM_INFO_SLOT;
    pSndMsg->msghdr.data_len    = 0;
    pSndMsg->msghdr.data_num  = 1;
    pSndMsg->msghdr.opcode  = IPC_OPCODE_GET;
    pSndMsg->msghdr.sequence  = 0;
    pSndMsg->msghdr.unit  = unit;
    pSndMsg->msghdr.slot  = slot;
    pSndMsg->msghdr.msg_index = unit << 8 |slot;

    iResult = ipc_sync_send_n1(pSndMsg, sizeof(struct ipc_mesg_n), &pRcvMsg, &iRevLen, 4000);

    
    if (iResult == -1)
    {
        ipc_msg_free(pSndMsg, module_id);

       //zlog_debug("%s[%d]:send message error\n", __FILE__, __LINE__);
        return -1;
    }
    else if(iResult == -2)
    {
         //zlog_debug
         return -1;
    }
    else  
    {
        if(pRcvMsg != NULL)
        {
            switch(pRcvMsg->msghdr.opcode)
            {
                case IPC_OPCODE_ACK:
                case IPC_OPCODE_REPLY:
                    memcpy(para_devm_slot, pRcvMsg->msg_data, sizeof(struct devm_slot));
                    ipc_msg_free(pRcvMsg, module_id);
                    return(0);
                    
                case IPC_OPCODE_NACK:
                    ipc_msg_free(pRcvMsg, module_id);
                    return -1;
                    
                default:
                    ipc_msg_free(pRcvMsg, module_id);
                    return -1;

            }
        
        }
        else return(-1);
    }

}


int devm_comm_get_mydevice_info(int module_id, struct devm *p_devm)
{
    int               iResult = 0;
    int               iRevLen = 0;

    struct ipc_mesg_n *pSndMsg = NULL;
    struct ipc_mesg_n *pRcvMsg = NULL;
 
    if (NULL == p_devm || module_id <= 0)
    {
       //zlog_err("%s[%d]:invalid para , NULL == para_dev_box\n", __FILE__, __LINE__);
        return -1;
    }

    pSndMsg =  ipc_msg_malloc(sizeof(struct ipc_mesg_n), module_id);
    
    if (NULL == pSndMsg)
    {
       //zlog_debug("%s[%d]:malloc share memory\r\n", __FILE__, __LINE__);
        return -1;
    }     
    
    memset(pSndMsg, 0, sizeof(struct ipc_mesg_n));
    
    pSndMsg->msghdr.module_id = MODULE_ID_DEVM;
    pSndMsg->msghdr.sender_id = module_id;
    pSndMsg->msghdr.msg_type  = IPC_TYPE_DEVM;
    pSndMsg->msghdr.msg_subtype  = DEVM_INFO_STRUCT_DEVM;
    pSndMsg->msghdr.data_len    = 0;
    pSndMsg->msghdr.data_num  = 0;
    pSndMsg->msghdr.opcode  = IPC_OPCODE_GET;
    pSndMsg->msghdr.sequence  = 0;

    iResult = ipc_sync_send_n1(pSndMsg, sizeof(struct ipc_mesg_n), &pRcvMsg, &iRevLen, 4000);

    
    if (iResult == -1)
    {
        ipc_msg_free(pSndMsg, module_id);

       //zlog_debug("%s[%d]:send message error\n", __FILE__, __LINE__);
        return -1;
    }
    else if(iResult == -2)
    {
         //zlog_debug
         return -1;
    }
    else  
    {
        if(pRcvMsg != NULL)
        {
            switch(pRcvMsg->msghdr.opcode)
            {
                case IPC_OPCODE_ACK:
                case IPC_OPCODE_REPLY:
                    memcpy(p_devm, pRcvMsg->msg_data, sizeof(struct devm));
                    ipc_msg_free(pRcvMsg, module_id);
                    return(0);
                    
                case IPC_OPCODE_NACK:
                    ipc_msg_free(pRcvMsg, module_id);
                    return -1;
                    
                default:
                    ipc_msg_free(pRcvMsg, module_id);
                    return -1;

            }

        }
        else return(-1);
    }

}

#if 0
int devm_comm_get_partner_slot(unsigned int unit, unsigned int module_id, unsigned char *para_partnerslot)
{
    unsigned int index = 0;
    unsigned char *p_partner_slot = NULL;

    if (NULL == para_partnerslot)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para NULL == para_partnerslot\n", __FILE__, __LINE__, __func__);
        return 1;
    }

    if (module_id == 0)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para,  module_id %d not (%d~max)\n", __FILE__, __LINE__, __func__, module_id, 1);
        return 1;
    }

    index = 0;
    index |= unit << 8;
    index |= 0xff;

    p_partner_slot = ipc_send_common_wait_reply(NULL, sizeof(NULL), 1 , MODULE_ID_DEVM, module_id, IPC_TYPE_DEVM, DEVM_INFO_PARTNERSLOT, IPC_OPCODE_GET, index);

    if (p_partner_slot == NULL)
    {
        zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:leave %s:devm_comm_get_partnerslot error\n", __FILE__, __LINE__, __func__);
        return 1;
    }
    else
    {
        *para_partnerslot = *p_partner_slot;
        zlog_debug(ZLOG_LIB_DBG_DEVM, "%s[%d]:%s: sucessfully get partner_slot(%d)  from devm\n", __FILE__, __LINE__, __func__, *para_partnerslot);
        return 0;
    }
}
#endif

int devm_comm_get_myunit_info(int module_id, struct devm_unit *para_myunit)
{
    struct devm t_devm;
//    struct devm_unit *p_unit = NULL;

    int               iResult = 0;
    int               iRevLen = 0;

    struct ipc_mesg_n *pSndMsg = NULL;
    struct ipc_mesg_n *pRcvMsg = NULL;
 
    if (NULL == para_myunit)
    {
       //zlog_err("%s[%d]:invalid para , NULL == para_dev_box\n", __FILE__, __LINE__);
        return -1;
    }

    if (module_id <= 0)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para,  module_id %d not (%d~max)\n", __FILE__, __LINE__, __func__, module_id, 1);
        return 1;
    }

    memset(&t_devm, 0, sizeof(struct devm));
    iResult = devm_comm_get_mydevice_info(module_id, &t_devm);
    if(0 == iResult)
    {
        pSndMsg =  ipc_msg_malloc(sizeof(struct ipc_mesg_n), module_id);
        
        if (NULL == pSndMsg)
        {
           //zlog_debug("%s[%d]:malloc share memory\r\n", __FILE__, __LINE__);
            return -1;
        }     
        
        memset(pSndMsg, 0, sizeof(struct ipc_mesg_n));
        
        pSndMsg->msghdr.module_id = MODULE_ID_DEVM;
        pSndMsg->msghdr.sender_id = module_id;
        pSndMsg->msghdr.msg_type  = IPC_TYPE_DEVM;
        pSndMsg->msghdr.msg_subtype  = DEVM_INFO_UNIT;
        pSndMsg->msghdr.data_len    = 0;
        pSndMsg->msghdr.data_num  = 1;
        pSndMsg->msghdr.opcode  = IPC_OPCODE_GET;
        pSndMsg->msghdr.sequence  = 0;
        pSndMsg->msghdr.unit = t_devm.myunit;
        pSndMsg->msghdr.msg_index = t_devm.myunit << 8;

        iResult = ipc_sync_send_n1(pSndMsg, sizeof(struct ipc_mesg_n), &pRcvMsg, &iRevLen, 4000);
        
        if (iResult == -1)
        {
            ipc_msg_free(pSndMsg, module_id);

            //zlog_debug("%s[%d]:send message error\n", __FILE__, __LINE__);
            return -1;
        }
        else if(iResult == -2)
        {
             //zlog_debug
             return -1;
        }
        else  
        {
            if(pRcvMsg != NULL)
            {
                switch(pRcvMsg->msghdr.opcode)
                {
                    case IPC_OPCODE_ACK:
                    case IPC_OPCODE_REPLY:
                          memcpy(para_myunit, pRcvMsg->msg_data, sizeof(struct devm_unit));
                          ipc_msg_free(pRcvMsg, module_id);
                          return(0);
                        
                    case IPC_OPCODE_NACK:
                          ipc_msg_free(pRcvMsg, module_id);
                          return -1;
                        
                    default:
                          ipc_msg_free(pRcvMsg, module_id);
                          return -1;
                
                }
            
            }
            else return(-1);
        }
    }

}


int devm_comm_get_sys_version(uchar *sys_ver)
{
    int fd, ret = 0;
    char orig_buf[256] = "\0";
    char a_sys_ver[256] = "\0";
    char *find = NULL;

    if (NULL == sys_ver)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para,  NULL == sys_ver\n", __FILE__, __LINE__, __func__);
        return 1;
    }

    fd = open("/home/bin/sys_version.txt", O_RDONLY);

    if (fd > 0)
    {
        ret = read(fd, orig_buf, 256);

        if (ret < 0)
        {
            zlog_err("%s[%d]:'%s' error: fail to get sys_ver from sys_version.txt.\n", __FILE__, __LINE__, __func__);
        }

        orig_buf[ret] = '\0';

        find = strstr((char *)orig_buf, "sys_version");

        sscanf(find,  "%*[^=]=%[^;]", a_sys_ver);
        memcpy(sys_ver, a_sys_ver, strlen(a_sys_ver));

        return 0;
    }
    else
    {
        zlog_err("%s[%d]:leave %s:error:can not find sys_version.txt\n", __FILE__, __LINE__, __func__);
        return 1;
    }
}


int devm_comm_get_sysObjectid(int module_id, unsigned int *oidLen, unsigned int *sysobjectid)
{
    int ret, i = 0, j = 0, k = 0;
    FILE *fd = NULL;
    unsigned int dev_id = 0;
    char orig_buf[256] = "\0";
    char oem_name[32] = "\0";
    char *find = NULL;
    int nlen = 0;
    char tmp[32] = "\0";
    int oidNum = 0;

    if (NULL == sysobjectid || NULL == oidLen)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para,  NULL == sys_ver\n", __FILE__, __LINE__, __func__);
        return 1;
    }

    /*check whether oem name is valid*/
    ret = devm_comm_get_oem_name(1, 0, module_id, (uchar *)oem_name);

    if (ret || '\0' == oem_name[0])
    {
        ret = devm_comm_get_id(1, 0, module_id, &dev_id);

        if(ret)
        {
            zlog_err("%s[%d]:'%s' err: not get device id from eeprom.\n", __FILE__, __LINE__, __func__);
            return 1;          
        }

        oidNum = sizeof(sysoid)/sizeof(struct devm_oid);

        while(i < oidNum)
        {
            if(sysoid[i].devId == dev_id)
            {
                for(j = 0; j < sysoid[i].oidLen && j < 32; j++)
                {
                    sysobjectid[j] = sysoid[i].oid[j];
                }
                break;
            }

            i++;
        }

        if(oidNum == i)
        {
            zlog_err("%s[%d]:leave %s:can not find device id, return sysObject failed\n", __FILE__, __LINE__, __func__);
            return 1;
        }

        *oidLen = sysoid[i].oidLen;

        return 0;
    }
    else
    {
        fd = fopen("/home/bin/sysoid.txt", "r");
		if( NULL == fd )
		{
			zlog_err("%s[%d]:leave %s error:not find sysoid.txt file!\n", __FILE__, __LINE__, __func__);
			return 1;
		}
		else
		{
    		while(fgets(orig_buf, sizeof(orig_buf), fd))
	        {
	            find = strstr((char *)orig_buf, oem_name);

	            if (NULL != find)
	            {
	                nlen = strlen(orig_buf);
	                orig_buf[nlen] = '\0';

	                if(':' != *(find  + strlen(oem_name)))
	                    continue;

	                for(i = strlen(oem_name) + 1; i < nlen; i++)
	                {
	                    tmp[j++] = *(find + i);
	                    if((*(find + i) == '.') || (*(find + i) == '\0'))
	                    {
	                        tmp[j] = '\0';
	                        sysobjectid[k++] = atoi(tmp);
	                        j = 0;
	                    }
	                }

	                *oidLen = k;
	                fclose(fd);

	                return 0;
	            }
	        }
		}

        *oidLen = 0;
        fclose(fd);
    }

    return 1;
}


int devm_comm_get_slot_version(uint8_t unit, uint8_t slot, int module_id, struct devm_version *pversion)
{
    int               iResult = 0;
    int               iRevLen = 0;

    struct ipc_mesg_n *pSndMsg = NULL;
    struct ipc_mesg_n *pRcvMsg = NULL;
 
    if (NULL == pversion)
    {
       //zlog_err("%s[%d]:invalid para , NULL == para_dev_box\n", __FILE__, __LINE__);
        return -1;
    }

    pSndMsg =  ipc_msg_malloc(sizeof(struct ipc_mesg_n), module_id);
    
    if (NULL == pSndMsg)
    {
       //zlog_debug("%s[%d]:malloc share memory\r\n", __FILE__, __LINE__);
        return -1;
    }     
    
    memset(pSndMsg, 0, sizeof(struct ipc_mesg_n));
    
    pSndMsg->msghdr.module_id = MODULE_ID_DEVM;
    pSndMsg->msghdr.sender_id = module_id;
    pSndMsg->msghdr.msg_type  = IPC_TYPE_DEVM;
    pSndMsg->msghdr.msg_subtype  = DEVM_INFO_SLOT_VERSION;
    pSndMsg->msghdr.data_len    = 4;
    pSndMsg->msghdr.data_num  = 1;
    pSndMsg->msghdr.opcode  = IPC_OPCODE_GET;
    pSndMsg->msghdr.sequence  = 0;
    pSndMsg->msghdr.unit  = unit;
    pSndMsg->msghdr.slot  = slot;
    pSndMsg->msghdr.msg_index = unit << 8 | slot;

    iResult = ipc_sync_send_n1(pSndMsg, sizeof(struct ipc_mesg_n), &pRcvMsg, &iRevLen, 2000);

    
    if (iResult == -1)
    {
        ipc_msg_free(pSndMsg, module_id);

       //zlog_debug("%s[%d]:send message error\n", __FILE__, __LINE__);
        return -1;
    }
    else if(iResult == -2)
    {
         //zlog_debug
         return -1;
    }
    else  
    {
        if(pRcvMsg != NULL)
        {
            switch(pRcvMsg->msghdr.opcode)
            {
                case IPC_OPCODE_ACK:
                case IPC_OPCODE_REPLY:
                    memcpy(pversion, pRcvMsg->msg_data, sizeof(struct devm_version));
                    ipc_msg_free(pRcvMsg, module_id);
                    return 0;
                    
                case IPC_OPCODE_NACK:
                    ipc_msg_free(pRcvMsg, module_id);
                    return -1;
                    
                default:
                    ipc_msg_free(pRcvMsg, module_id);
                    return -1;
            
            }
        
        }
        else return(-1);
    }
}

int devm_find_eep_info_from_buf(unsigned char *orig_buf, unsigned char *key_word, unsigned char *find_buf , int len)
{
    int i = 0;
    char *find = NULL;
    int date_len = 0;
    unsigned char *str = orig_buf;

    if (NULL == orig_buf || NULL == key_word || NULL == find_buf)
    {
        zlog_err("%s[%d]:leave %s error:invalid input\n", __FILE__, __LINE__, __func__);
        return -1;
    }

    for (i = 0; i < len; i++)
    {
        if (date_len > len)
        {
            break;
        }

        find = strstr((char *)str, (char *)key_word);

        if (NULL == find)
        {
            str = str + strlen((char *)str) + 1;
            date_len = date_len + strlen((char *)str) + 1;

            if (date_len > len)
            {
                break;
            }
        }
        else
        {
            sscanf(find, "%*[^=]=%[^; \t\n\r\f]", find_buf);
            return 0;
        }
    }

    return 1;
}

int devm_comm_get_digital_label( int slot, devdigtlabl *digtlabl)    
{
    uint32_t index = 0, i = 0;
    unsigned char eep_label[256] = {0};
     char find_buff[64] = {0};
//    struct devm_digtlabl digtlabl; 
    struct tm Tm1;

    int               iResult = 0;
    int               iRevLen = 0;

    struct ipc_mesg_n *pSndMsg = NULL;
    struct ipc_mesg_n *pRcvMsg = NULL;
 
    if (slot == 0 || slot > SLOT_NUM_MAX)
    {
        zlog_err("%s[%d]:leave %s:invalid para, slot(%d) range is (%d~%d)\n", __FILE__, __LINE__, __func__, slot, 1, UNIT_NUM_MAX);
        return 1;
    }

    pSndMsg =  ipc_msg_malloc(sizeof(struct ipc_mesg_n), MODULE_ID_DEVM);
    
    if (NULL == pSndMsg)
    {
       //zlog_debug("%s[%d]:malloc share memory\r\n", __FILE__, __LINE__);
        return -1;
    }     
    
    memset(pSndMsg, 0, sizeof(struct ipc_mesg_n));
    
    pSndMsg->msghdr.module_id = MODULE_ID_HAL;
    pSndMsg->msghdr.sender_id = MODULE_ID_DEVM;
    pSndMsg->msghdr.msg_type  = IPC_TYPE_DEVM;
    pSndMsg->msghdr.msg_subtype  = DEVM_INFO_EEPINFO_OP;
    pSndMsg->msghdr.data_len    = 0;
    pSndMsg->msghdr.data_num  = 1;
    pSndMsg->msghdr.opcode  = IPC_OPCODE_UPDATE;
    pSndMsg->msghdr.sequence  = 0;
    pSndMsg->msghdr.unit  = 1;
    pSndMsg->msghdr.slot  = slot;
    pSndMsg->msghdr.msg_index = 1 << 8 |slot;

    iResult = ipc_sync_send_n1(pSndMsg, sizeof(struct ipc_mesg_n), &pRcvMsg, &iRevLen, 2000);

    
    if (iResult == -1)
    {
        ipc_msg_free(pSndMsg, MODULE_ID_DEVM);
        return -1;
    }
    else if(iResult == -2)
    {
         return -1;
    }
    else  
    {
        if(pRcvMsg != NULL)
        {
            index |= 2 << 24;
            index |= 15 << 16;
            index |= 1 << 8;
            index |= slot;

            zlog_debug(ZLOG_LIB_DBG_DEVM,"%s[%d]:leave %s:sucessfully to get slot %d eeprom info\n", __FILE__, __LINE__, __func__,slot);
            memcpy(eep_label, pRcvMsg->msg_data, sizeof(eep_label));
            for (i = 0; pDigtLabl[i] != NULL; i++)
            {
                memset(find_buff, 0, sizeof(find_buff));
                devm_find_eep_info_from_buf(eep_label, pDigtLabl[i], find_buff, sizeof(eep_label));
                switch( i )
                {
                    case DEVM_DIGITAL_LABEL_HWCFG:
                        find_buff[strlen(find_buff)] = '\0';
                        memcpy(digtlabl->hwcfg, find_buff, sizeof(digtlabl->hwcfg));
                        break;              
                    case DEVM_DIGITAL_LABEL_MAC:
                        find_buff[strlen(find_buff)] = '\0';
                        memcpy(digtlabl->mac, find_buff, sizeof(digtlabl->mac));
                        break;
                    case DEVM_DIGITAL_LABEL_OEM:
                        find_buff[strlen(find_buff)] = '\0';
                        memcpy(digtlabl->oem, find_buff, sizeof(digtlabl->oem));
                        break;
                    case DEVM_DIGITAL_LABEL_VND:
                        find_buff[strlen(find_buff)] = '\0';
                        memcpy(digtlabl->vnd, find_buff, sizeof(digtlabl->vnd));
                        break;
                    case DEVM_DIGITAL_LABEL_BOM:
                        find_buff[strlen(find_buff)] = '\0';
                        memcpy(digtlabl->bom, find_buff, sizeof(digtlabl->bom));
                        break;
                    case DEVM_DIGITAL_LABEL_DEV:
                        find_buff[strlen(find_buff)] = '\0';
                        memcpy(digtlabl->dev, find_buff, sizeof(digtlabl->dev));
                        break;
                    case DEVM_DIGITAL_LABEL_DATE:
                        if(0 < strlen(find_buff))
                        {
                            strncat(find_buff, " 00:00:00", sizeof(" 00:00:00"));
                            strptime(find_buff, "%Y.%m.%d %H:%M:%S", &Tm1);
                            digtlabl->date = mktime(&Tm1);
                        }
                        else digtlabl->date = 0;
                        break;
                    case DEVM_DIGITAL_LABEL_HVER:
                        find_buff[strlen(find_buff)] = '\0';
                        memcpy(digtlabl->hver, find_buff, sizeof(digtlabl->hver));
                        break;
                    case DEVM_DIGITAL_LABEL_DVER:
                        find_buff[strlen(find_buff)] = '\0';
                        memcpy(digtlabl->dver, find_buff, sizeof(digtlabl->dver));
                        break;
                    case DEVM_DIGITAL_LABEL_SN:
                        find_buff[strlen(find_buff)] = '\0';
                        memcpy(digtlabl->sn, find_buff, sizeof(digtlabl->sn));
                        break;
                    case DEVM_DIGITAL_LABEL_LIC:
                        find_buff[strlen(find_buff)] = '\0';
                        memcpy(digtlabl->lic, find_buff, sizeof(digtlabl->lic));
                        return 0;                 
                    default:
                        break;
                        
                }
            }                         
             ipc_msg_free(pRcvMsg,MODULE_ID_DEVM);
             return(0);
        }
        else return(-1);
    }

}

devdigtlablinfo lablinfo;

devdigtlablinfo *devm_comm_reply_snmp_bulk (devdigtlablindex *index, int *lablnum)
{
    int ret = 0, i = 0;
    devdigtlabl digtlabl;    
    int slotcount = 0;
    enum DEV_TYPE dev_type = DEV_TYPE_INVALID;

    ret = devm_comm_get_dev_type(MODULE_ID_SNMPD, &dev_type);

    if (0 == ret)
    {
        if (DEV_TYPE_BOX == dev_type)
        {
            slotcount = index->slot;
            if(SLOT_NUM_MAX < slotcount)
            {
                zlog_err("%s[%d]:leave %s:error:slot %d exceed the MAX value \n", __FILE__, __LINE__, __func__,slotcount);
                return NULL;        
            }
            
            for(i = slotcount; i < 1; i++)
            {
                ret = devm_comm_get_digital_label( i, &digtlabl);
                if(0 != ret)
                {
                    continue;
                }
                else
                {
                    *lablnum = 1;    
                    lablinfo.index.slot = i + 1;
                    memcpy(&lablinfo.data.digtlabldata, &digtlabl, sizeof(devdigtlabl));
                    return &lablinfo;
                }
            }
            *lablnum = 0;
            return NULL;

        }
        else if (DEV_TYPE_SHELF == dev_type)
        {
            slotcount = index->slot;
            if(SLOT_NUM_MAX < slotcount)
            {
                zlog_err("%s[%d]:leave %s:error:slot %d exceed the MAX value \n", __FILE__, __LINE__, __func__,slotcount);
                return NULL;        
            }
            
            for(i = slotcount; i < (SLOT_NUM_MAX + 1); i++)
            {
                ret = devm_comm_get_digital_label( i, &digtlabl);
                if(0 != ret)
                {
                    continue;
                }
                else
                {
                    *lablnum = 1;    
                    lablinfo.index.slot = i + 1;
                    memcpy(&lablinfo.data.digtlabldata, &digtlabl, sizeof(devdigtlabl));
                    return &lablinfo;
                }
            }
            *lablnum = 0;
            return NULL;

        }
    }
    else
    {
        *lablnum = 0;
        return NULL;
    }

}


