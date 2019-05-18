#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <lib/command.h>
#include <lib/log.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/hash1.h>
#include <lib/ifm_common.h>
#include <lib/linklist.h>
#include <lib/msg_ipc.h>
#include <lib/zassert.h>
#include <lib/errcode.h>
#include <lib/ifm_common.h>
#include <lib/devm_com.h>
#include <lib/alarm.h>
#include <lib/gpnSocket/socketComm/gpnAlmTypeDef.h>
#include <lib/gpnSocket/socketComm/gpnSockCommRoleMan.h>
//#include <alarm/alarm_def.h>
#include <syslog/syslogd.h>
#include "devm.h"


/*device info struct*/
struct devm device;

/*mainboard status*/
extern enum HA_BOOTSTATE myboard_dev_start_status;
extern enum HA_BOOTSTATE otherboard_dev_start_status;
extern enum HA_ROLE ha_dev_ha_role;
extern int gPwrSlot1 , gPwrSlot2 , gFanSlot ;

unsigned char myslot = 0;
unsigned char partnerslot = 0;

static struct plugout_timer_info plugout_timer_t[MAX_TIMER_CNT];
unsigned char sys_version[SLOT_NUM_MAX][NAME_STRING_LEN] = {{0}};
unsigned char sys_objectid[64] = {0};
char dynamic_voltage_switch[2] = {1, 1};
unsigned int slotn_power_ctrl = 0;

unsigned char temperature_alarm_record = 0; /*temperature_alarm_record==0 temperature normal,temperature_alarm_record==1 low temperature alarm,  temperature_alarm_record==2  high temperature alarm*/
unsigned char vol_alarm_record_input[2] = {0}; /*vol_alarm_record_input[n]==0 vol normal,vol_alarm_record_input[n]==1 vol low alarm send,  vol_alarm_record_input[n]==2 vol high alarm send*/
unsigned char vol_alarm_record_output[2] = {0}; /*vol_alarm_record_input[n]==1 vol output down alarm send, vol_alarm_record_input[n]==0 vol output up alarm send*/

EEP_KEY *g_peep_info = NULL;

EEP_KEY  key_words_main =
{
    EEPROM_PRODUCT_INFO_MAX,
    6,
    {
        { PRODUCT_KEY_DEV_TYPE,   INPUT_INFO_LEN_MIN },
//      { PRODUCT_KEY_DEV_NAME,   INPUT_INFO_LEN_STR },
        { PRODUCT_KEY_MAC,   INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_MAC_NUM,   INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_HW_VERSION,   INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_SLOT_NAME, NAME_STRING_LEN },
        { PRODUCT_KEY_SN, NAME_STRING_LEN },
    }
};


EEP_KEY     key_words_back =
{
    EEPROM_PRODUCT_INFO_MAX,
    8,
    {
        { PRODUCT_KEY_DEV_TYPE,   INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_DEV_NAME, NAME_STRING_LEN },
        { PRODUCT_KEY_OEM_NAME, NAME_STRING_LEN },
        { PRODUCT_KEY_MAC,  INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_MAC_NUM, INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_HW_VERSION,    INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_VENDOR,   NAME_STRING_LEN },
        { PRODUCT_KEY_SN, NAME_STRING_LEN },
    }
};


EEP_KEY     key_words_slot =
{
    EEPROM_PRODUCT_INFO_MIN,
    4,
    {
        { PRODUCT_KEY_DEV_TYPE,   INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_HW_VERSION,    INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_SLOT_NAME, NAME_STRING_LEN },
        { PRODUCT_KEY_SN, NAME_STRING_LEN },
    }
};


EEP_KEY     key_words_power =
{
    EEPROM_PRODUCT_INFO_MIN,
    4,
    {
        { PRODUCT_KEY_DEV_TYPE,   INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_HW_VERSION,    INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_SLOT_NAME, NAME_STRING_LEN },
        { PRODUCT_KEY_SN, NAME_STRING_LEN },
    }
};


EEP_KEY     key_words_fan =
{
    EEPROM_PRODUCT_INFO_MIN,
    4,
    {
        { PRODUCT_KEY_DEV_TYPE,   INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_HW_VERSION,    INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_SLOT_NAME, NAME_STRING_LEN },
        { PRODUCT_KEY_SN, NAME_STRING_LEN },
    }
};

#if 0
#endif

/* digital define for digital label */
EEP_KEY     key_words_digital =
{
    EEPROM_PRODUCT_INFO_MAX,
    12,
    {
        { PRODUCT_KEY_DEV_TYPE,   INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_DEV, INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_OEM_NAME, INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_MAC,  INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_DVER, INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_HVER,    INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_VND,   INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_SN, INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_DATE,    INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_BOM,   INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_LIC, NAME_STRING_LEN },
        { PRODUCT_KEY_CFGINIT, NAME_STRING_LEN },
    }
};

#if 0
EEP_KEY  key_words_main_digital =
{
    EEPROM_PRODUCT_INFO_MAX,
    5,
    {
        { PRODUCT_KEY_DEV_TYPE,   INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_DEV,   INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_MAC,   INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_HVER,   INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_SN, INPUT_INFO_LEN_MIN },
    }
};

EEP_KEY     key_words_slot_digital =
{
    EEPROM_PRODUCT_INFO_MIN,
    4,
    {
        { PRODUCT_KEY_DEV_TYPE,   INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_HVER,    INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_DEV,INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_SN,INPUT_INFO_LEN_MIN },
    }
};

EEP_KEY     key_words_power_digital =
{
    EEPROM_PRODUCT_INFO_MIN,
    4,
    {
        { PRODUCT_KEY_DEV_TYPE,   INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_HVER,    INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_DEV, NAME_STRING_LEN },
        { PRODUCT_KEY_SN, NAME_STRING_LEN },
    }
};


EEP_KEY     key_words_fan_digital =
{
    EEPROM_PRODUCT_INFO_MIN,
    4,
    {
        { PRODUCT_KEY_DEV_TYPE,   INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_HVER,    INPUT_INFO_LEN_MIN },
        { PRODUCT_KEY_DEV, NAME_STRING_LEN },
        { PRODUCT_KEY_SN, NAME_STRING_LEN },
    }
};
#endif

unsigned char a_ha_bootstate[][25] =
{
    "HA_INVALID",         /* 设备启动开始 */
    "HA_BOOTSTA_START",       /* 设备启动开始 */
    "HA_BOOTSTA_END",         /* 设备启动完成 */
    "HA_BOOTSTA_DATAOK",         /* 数据加载完成 */
};

unsigned char a_ha_dev_ha_role[][25] =
{
    "INVALID",
    "HA_ROLE_INIT",          /*HA 角色*/
    "HA_ROLE_MASTER",
    "HA_ROLE_SLAVE"
};

unsigned char a_eep_op_cmd[][25] =
{
    "INVALID",
    "set",
    "get",
    "erase",
};

unsigned char a_eepinto_type[][25] =
{
    "INVALID",
    "dev_name",
    "oem_name",
    "hardware_version",
    "vendor",
    "slot_name",
    "1st_mac",
    "hwcfg",
    "mac_num"
    "all"
};

#if 1
#endif

static enum IFNET_TYPE  port_type_set(enum IFNET_SUBTYPE port_type)
{
    enum IFNET_TYPE ifnet_type = IFNET_TYPE_INVALID;

    switch (port_type)
    {
        case IFNET_SUBTYPE_FE:
            ifnet_type = IFNET_TYPE_ETHERNET;
            break;

        case IFNET_SUBTYPE_GE:
        case IFNET_SUBTYPE_PHYSICAL:            
            ifnet_type = IFNET_TYPE_GIGABIT_ETHERNET;
            break;

        case IFNET_SUBTYPE_10GE:
        case IFNET_SUBTYPE_40GE:
            ifnet_type = IFNET_TYPE_XGIGABIT_ETHERNET;
            break;

        case IFNET_SUBTYPE_E1:
            ifnet_type = IFNET_TYPE_TDM;
            break;

        case IFNET_SUBTYPE_STM1:
        case IFNET_SUBTYPE_STM4:            
        case IFNET_SUBTYPE_STM16:   
        case IFNET_SUBTYPE_STM64:            
            ifnet_type = IFNET_TYPE_STM;
            break;

        case IFNET_SUBTYPE_CLOCK2M:
            ifnet_type = IFNET_TYPE_CLOCK;
            break;
        case IFNET_SUBTYPE_CE1:
            ifnet_type = IFNET_TYPE_E1;
            break;
            
        default:
            break;
    }

    return ifnet_type;
}


int devm_get_power_alarm_type_by_mode(uint8_t  mode)
{
    int ret = 0;

    if ((POWER_TYPE_48v_12V_75W == mode)
            || (POWER_TYPE_48v_12V_150W == mode)
            || (POWER_TYPE_PWR01A == mode))
    {
        ret = IFM_POWN48_TYPE;
    }
    else if ((POWER_TYPE_220v_12V_75W == mode)
             || (POWER_TYPE_220v_12V_150W == mode)
            || (POWER_TYPE_PWR02A == mode))
    {
        ret = IFM_POW220_TYPE;
    }
    else if (POWER_TYPE_NONE == mode)
    {
        zlog_err("%s[%d]failed to get power type , mode(%d) should be 0/2/4/6\n", __func__, __LINE__, mode);
        ret = -1;
    }
    else
    {
        zlog_err("%s[%d]failed to get power type , mode(%d) should be 0/2/4/6\n", __func__, __LINE__, mode);
        ret = 0;
    }

    return ret;
}


int devm_find_info_in_orig_buf(unsigned char *orig_buf, unsigned char *key_word, unsigned char *find_buf , int len)
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


int devm_get_slot_app_version(unsigned char *sys_version, unsigned char slot)
{
    int ret = 0;
    unsigned int index = 0;

    if (device.myslot == slot)
    {
        ret = devm_comm_get_sys_version(sys_version);
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully get slot %d sys version(%s) from sys_ver.txt!\n", __FILE__, __LINE__, __func__ , slot , sys_version);
    }
    else if (NULL != device.pmyunit
             && (slot == device.pmyunit->slot_main_board || slot == device.pmyunit->slot_slave_board)
             && device.myslot != slot)
    {

        index = (device.myunit << 8) | slot;
        
//      ret = ipc_send_ha(NULL, 0, 1, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_SYS_VERSION, IPC_OPCODE_GET , index);        
        ret = ipc_send_msg_ha_n2(NULL, 0, 0, MODULE_ID_DEVM,  MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_SYS_VERSION, IPC_OPCODE_GET, index, 0);

        if (ret)
        {
            zlog_err("%s[%d]:leave %s:error:fail to send msg to get sys_ver from slot %d!\n", __FILE__, __LINE__, __func__ , slot);
        }
        else
        {
            zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to send msg to get sys_ver from slot %d!\n", __FILE__, __LINE__, __func__ , slot);
        }
    }

    return ret;
}


/* 创建盒式设备 */
int devm_box_create(const struct devm_box *pbox)
{
    int ret = 0;
    unsigned int index = 0;
    struct devm_com t_devm_com;
    unsigned char filem_device_name[32] = {0};

    if (NULL == pbox)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para pbox\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    if (NULL != device.pbox)
    {
        zlog_err("%s[%d]:leave %s:error:fail to create box slot, box already exist\n", __FILE__, __LINE__, __func__);
    }

    device.pbox = (struct devm_box *)XCALLOC(MTYPE_DEVM_ENTRY, sizeof(struct devm_box));

    if (NULL == device.pbox)
    {
        zlog_err("%s[%d]:leave %s:error:fail to XCALLOC struct devm_box\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    memcpy(device.pbox, pbox, sizeof(struct devm_box));
    device.type = DEV_TYPE_BOX;
    device.slot_type = SLOT_TYPE_BOARD_MAIN;
    device.unit_num = 1;
    device.main_unit = 1;
    device.myunit = 1;
    device.myslot = 1;
    device.pbox->port_num = 0;
    device.pbox->port_list.head = NULL;
    device.pbox->port_list.tail = NULL;
    device.pbox->card_list.head = NULL;
    device.pbox->card_list.tail = NULL;

    /*broadcast to other registered module , box create*/
    memset(&t_devm_com, 0, sizeof(struct devm_com));
    t_devm_com.unit = 1;
    t_devm_com.slot = 1;
    t_devm_com.main_slot = 1;
    t_devm_com.slave_slot = 0;
    t_devm_com.type = device.pbox->type;
    t_devm_com.status = DEV_STATUS_PLUGIN;
    t_devm_com.ha_role = ha_dev_ha_role;
    t_devm_com.ha_status = myboard_dev_start_status;
    memcpy(t_devm_com.mac, device.pbox->mac, sizeof(t_devm_com.mac));
    devm_event_notify(DEV_EVENT_SLOT_ADD, &t_devm_com);

    //ipran_alarm_report (IFM_EQU_TYPE, 0, 0, 0, 0, GPN_ALM_TYPE_DEV_POW_FAIL, GPN_SOCK_MSG_OPT_CLEAN); /* 上电告警 */

    /* modify for ipran by lipf, 2018/4/25 */
    struct ifm_usp usp;
    struct gpnPortInfo gPortInfo;

    memset(&usp, 0, sizeof(struct ifm_usp));
    usp.type = IFNET_TYPE_MAX;
    usp.unit = 1;
    usp.slot = 0;
    usp.port = 0;
    memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
    gPortInfo.iAlarmPort = IFM_EQU_TYPE;
    gPortInfo.iIfindex = ifm_get_ifindex_by_usp(&usp);

    ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_DEV_POW_FAIL, GPN_SOCK_MSG_OPT_CLEAN);  /* 上电告警 */
    zlog_notice("%s[%d]send powerup to alarm\n", __func__, __LINE__);


    if (('a' <= device.pbox->oem_name[0] && device.pbox->oem_name[0] <= 'z')
            || ('A' <= device.pbox->oem_name[0] && device.pbox->oem_name[0] <= 'Z'))
    {
        memcpy(filem_device_name, device.pbox->oem_name, sizeof(device.pbox->oem_name));
    }
    else
    {
        memcpy(filem_device_name, device.pbox->dev_name, sizeof(device.pbox->dev_name));
    }

    index = 0;
//    index |= ((device.myunit) << 25);
//    index |= ((device.myslot) << 20);
#if 0
    ret = ipc_send_msg_n2(filem_device_name, sizeof(filem_device_name), 1, MODULE_ID_SYSLOG, MODULE_ID_DEVM,
                           IPC_TYPE_DEVM, DEVM_INFO_DEV_NAME, IPC_OPCODE_EVENT , index);
#endif
    ret = ipc_send_msg_n2(filem_device_name, sizeof(filem_device_name), 1, MODULE_ID_SYSLOG, MODULE_ID_DEVM,
                           IPC_TYPE_DEVM, DEVM_INFO_DEV_NAME, IPC_OPCODE_EVENT , index);

    if (ret)
    {
        zlog_err("%s[%d]:leave %s:error:fail to send dev_name info filem, dev name = %s\n", __FILE__, __LINE__, __func__ , filem_device_name);
    }
    else
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to send dev_name info filem, dev name = %s\n", __FILE__, __LINE__, __func__ , filem_device_name);
    }

    zlog_debug(DEVM_DBG_COMM, "%s[%d]:%s: sucessfully create box\n", __FILE__, __LINE__, __func__);

    return ERRNO_SUCCESS;
}


/* 框式设备创建 unit */
int devm_unit_create(uint8_t unit, const struct devm_unit *punit)
{
    int ret = 0;
    unsigned int index = 0;
    struct devm_unit *p_devm_unit = NULL;
    unsigned char filem_device_name[32] = {0};

    if (NULL == punit)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    if (NULL != device.punit[unit - 1])
    {
        zlog_err("%s[%d]:leave %s:error:fail to create unit, unit already exist,unitid=%d\n", __FILE__, __LINE__, __func__ , unit);
        return ERRNO_FAIL;
    }

    p_devm_unit = (struct devm_unit *)XCALLOC(MTYPE_DEVM_ENTRY, sizeof(struct devm_unit));

    if (NULL == p_devm_unit)
    {
        zlog_err("%s[%d]:leave %s:error:fail to XCALLOC struct devm_unit\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    memcpy(p_devm_unit, punit, sizeof(struct devm_unit));
    memset(&p_devm_unit->pslot, 0, sizeof(p_devm_unit->pslot));

    device.punit[unit - 1] = p_devm_unit;
    device.unit_num ++;

    if (device.main_unit == unit)   /* 创建主框 */
    {
        device.type = DEV_TYPE_SHELF;
        device.pmyunit = p_devm_unit;
        device.myunit = unit;
        device.slot_type = SLOT_TYPE_INVALID; /* 还没有创建 slot */
        device.myslot = 0;
    }

    /* modify for ipran by lipf, 2018/4/25 */
    struct ifm_usp usp;
    struct gpnPortInfo gPortInfo;

    memset(&usp, 0, sizeof(struct ifm_usp));
    usp.type = IFNET_TYPE_MAX;
    usp.unit = unit;
    usp.slot = 0;
    usp.port = 0;
    memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
    gPortInfo.iAlarmPort = IFM_EQU_TYPE;
    gPortInfo.iIfindex = ifm_get_ifindex_by_usp(&usp);

    ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_DEV_POW_FAIL, GPN_SOCK_MSG_OPT_CLEAN);
    zlog_notice("%s[%d]send powerup to alarm\n", __func__, __LINE__);

    if (('a' <= p_devm_unit->bplane.oem_name[0] && p_devm_unit->bplane.oem_name[0] <= 'z')
            || ('A' <= p_devm_unit->bplane.oem_name[0] && p_devm_unit->bplane.oem_name[0] <= 'Z'))
    {
        memcpy(filem_device_name, p_devm_unit->bplane.oem_name, sizeof(p_devm_unit->bplane.oem_name));
    }
    else
    {
        memcpy(filem_device_name, p_devm_unit->bplane.dev_name, sizeof(p_devm_unit->bplane.dev_name));
    }

    index = 0;
//    index = ((device.myunit) << 25);

    ret = ipc_send_msg_n2(filem_device_name, sizeof(filem_device_name), 1, MODULE_ID_SYSLOG, MODULE_ID_DEVM,
                           IPC_TYPE_DEVM, DEVM_INFO_DEV_NAME, IPC_OPCODE_EVENT , index);

    if (ret)
    {
        zlog_err("%s[%d]:leave %s:error:fail to send dev_name info filem, dev name = %s\n", __FILE__, __LINE__, __func__ , filem_device_name);
    }
    else
    {
        zlog_notice("%s[%d]:leave %s:sucessfully to send dev_name info filem, dev name = %s\n", __FILE__, __LINE__, __func__ , filem_device_name);
    }

    return ERRNO_SUCCESS;
}


/* 板卡插入事件处理，对隐藏的板卡取消隐藏 */
int devm_slot_plugin(uint8_t unit, uint8_t slot)
{
    int ret = 0;
    int ret_func = 0;
    unsigned char port_count = 0;
//  struct ifm_common t_ifm_common;
    struct list *p_list = NULL;
    struct listnode *p_listnode = NULL;
    struct devm_port *p_devm_port = NULL;
    struct devm_com t_devm_com;
    struct ifm_common ifm_common_send[SLOT_PORT_NUM];
    struct ifm_usp usp;
    struct gpnPortInfo gPortInfo;

    if (NULL == device.punit[unit - 1] || NULL == device.punit[unit - 1]->pslot[slot - 1])
    {
        zlog_err("%s[%d]:leave %s:error:fail to plugin slot\n", __FILE__, __LINE__, __func__);
        ret_func = ERRNO_FAIL;
    }
    else if (0 == device.punit[unit - 1]->pslot[slot - 1]->plugout)
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:slot %d exist,and not hide,can not do plugin\n", __FILE__, __LINE__, __func__, slot);
        ret_func = ERRNO_SUCCESS;
    }
    else
    {
        if (1 == plugout_timer_t[slot - 1].state)
        {
            devm_del_a_timer(slot);
        }

        time(& device.punit[unit - 1]->pslot[slot - 1]->run_time);

        p_list = &device.punit[unit - 1]->pslot[slot - 1]->port_list;

        memset(ifm_common_send, 0, sizeof(struct ifm_common) * SLOT_PORT_NUM);

        for (ALL_LIST_ELEMENTS_RO(p_list, p_listnode, p_devm_port)) /*遍历所有端口去除隐藏*/
        {
            ifm_common_send[port_count].usp.unit = unit;
            ifm_common_send[port_count].usp.slot = slot;
            ifm_common_send[port_count].usp.type = port_type_set(p_devm_port->port_type);
            ifm_common_send[port_count].usp.port = p_devm_port->port;
            ifm_common_send[port_count].usp.subtype = p_devm_port->port_type;
            ifm_common_send[port_count].port_info.hide = 0;
            port_count++;

            /*通知其他进程端口创建*/
            memset(&t_devm_com, 0, sizeof(struct devm_com));
            t_devm_com.unit = unit;
            t_devm_com.slot = slot;
            t_devm_com.port = p_devm_port->port;
            t_devm_com.main_slot = device.pmyunit->slot_main_board;
            t_devm_com.slave_slot = device.pmyunit->slot_slave_board;
            t_devm_com.ha_role = ha_dev_ha_role;
            t_devm_com.ha_status = myboard_dev_start_status;

            if (DEV_TYPE_BOX == device.type)
            {
                t_devm_com.type = SLOT_TYPE_BOARD_MAIN;
                memcpy(t_devm_com.mac, device.pbox->mac, sizeof(t_devm_com.mac));
            }
            else
            {
                t_devm_com.type = device.punit[unit - 1]->pslot[slot - 1]->type;
                memcpy(t_devm_com.mac, device.punit[unit - 1]->pslot[slot - 1]->mac, sizeof(t_devm_com.mac));
            }

            devm_event_notify(DEV_EVENT_PORT_ADD, &t_devm_com);
        }

        /*tel ifm port unhide*/
        ret = ipc_send_msg_n2(ifm_common_send, sizeof(struct ifm_common) * port_count, port_count, MODULE_ID_IFM, MODULE_ID_DEVM,
                              IPC_TYPE_DEVM, DEVM_INFO_PORT, IPC_OPCODE_UPDATE, 0);

        if (-1 == ret)
        {
            zlog_err("%s[%d]:leave %s:error:fail to send port unhide msg to ifm,unit = %d, slot = %d\n", __FILE__, __LINE__, __func__, unit, slot);
        }
        else
        {
            zlog_notice("%s[%d]:leave %s:sucessfully to send port unhide msg to ifm,unit = %d, slot = %d\n", __FILE__, __LINE__, __func__, unit, slot);
        }

        device.punit[unit - 1]->pslot[slot - 1]->plugout = 0;

        /*通知其他进程板卡插入*/
        memset(&t_devm_com, 0, sizeof(struct devm_com));
        t_devm_com.unit = unit;
        t_devm_com.slot = slot;
        t_devm_com.main_slot = device.pmyunit->slot_main_board;
        t_devm_com.slave_slot = device.pmyunit->slot_slave_board;
        t_devm_com.type = device.punit[unit - 1]->pslot[slot - 1]->type;
        t_devm_com.status = DEV_STATUS_PLUGIN;
        t_devm_com.ha_role = ha_dev_ha_role;
        t_devm_com.ha_status = myboard_dev_start_status;
        memcpy(t_devm_com.mac, device.punit[unit - 1]->pslot[slot - 1]->mac, sizeof(t_devm_com.mac));
        devm_event_notify(DEV_EVENT_SLOT_ADD, &t_devm_com);

        /*send slot plugin alarm*/
        if (0 != device.pmyunit->myslot && device.pmyunit->slot_slave_board != device.pmyunit->myslot)
        {
            //ipran_alarm_report (IFM_EQU_TYPE, slot, 0, 0, 0, GPN_ALM_TYPE_UNIT_MIS, GPN_SOCK_MSG_OPT_CLEAN);    /*发送板卡插入告警*/

            /* modify for ipran by lipf, 2018/4/25 */
            memset(&usp, 0, sizeof(struct ifm_usp));
            usp.type = IFNET_TYPE_MAX;
            usp.unit = unit;
            usp.slot = slot;
            usp.port = 0;
            memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
            gPortInfo.iAlarmPort = IFM_EQU_TYPE;
            gPortInfo.iIfindex = ifm_get_ifindex_by_usp(&usp);

            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_UNIT_MIS, GPN_SOCK_MSG_OPT_CLEAN);

            zlog_notice("%s[%d]send slot %d plugin info to alarm modult\n", __func__, __LINE__, slot);
        }

        zlog_notice("%s[%d]:set slot (%d,%d) plugin\n", __func__, __LINE__, unit, slot);

        ret_func = ERRNO_SUCCESS;
    }

    return ret_func;
}


int devm_slot_plugout(uint8_t unit, uint8_t slot)
{
    int ret = 0;
    unsigned char port_count = 0;
    struct list *p_list = NULL;
    struct listnode *p_listnode = NULL;
    struct devm_port *p_devm_port = NULL;
    struct devm_com t_devm_com;
    unsigned int index = 0;
    struct ifm_common ifm_common_send[SLOT_PORT_NUM];
    struct ifm_usp usp;
    struct gpnPortInfo gPortInfo;

    if (NULL == device.punit[unit - 1] || NULL == device.punit[unit - 1]->pslot[slot - 1])
    {
        zlog_err("%s[%d]:leave %s:error:fail to plugout slot\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }
    else if (1 == device.punit[unit - 1]->pslot[slot - 1]->plugout)
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:slot %d exist,and hide,can not do plugout\n", __FILE__, __LINE__, __func__, slot);
        return ERRNO_SUCCESS;
    }

    device.punit[unit - 1]->pslot[slot - 1]->run_time = 0;
    p_list = &device.punit[unit - 1]->pslot[slot - 1]->port_list;

    memset(ifm_common_send, 0, sizeof(struct ifm_common) * SLOT_PORT_NUM);

    for (ALL_LIST_ELEMENTS_RO(p_list, p_listnode, p_devm_port)) /*遍历所有端口配置隐藏*/
    {
        ifm_common_send[port_count].usp.unit = unit;
        ifm_common_send[port_count].usp.slot = slot;
        ifm_common_send[port_count].usp.type = port_type_set(p_devm_port->port_type);
        ifm_common_send[port_count].usp.port = p_devm_port->port;
        ifm_common_send[port_count].usp.subtype = p_devm_port->port_type;
        ifm_common_send[port_count].port_info.hide = 1;
        port_count++;

        /*通知其他进程端口删除*/
        memset(&t_devm_com, 0, sizeof(struct devm_com));
        t_devm_com.unit = unit;
        t_devm_com.slot = slot;
        t_devm_com.main_slot = device.pmyunit->slot_main_board;
        t_devm_com.slave_slot = device.pmyunit->slot_slave_board;
        t_devm_com.port = p_devm_port->port;
        t_devm_com.ha_role = ha_dev_ha_role;
        t_devm_com.ha_status = myboard_dev_start_status;
        devm_event_notify(DEV_EVENT_PORT_DELETE, &t_devm_com);
    }

    /*tel ifm port hide*/
    ret = ipc_send_msg_n2(ifm_common_send, sizeof(struct ifm_common) * port_count, port_count, MODULE_ID_IFM, MODULE_ID_DEVM,
                          IPC_TYPE_DEVM, DEVM_INFO_PORT, IPC_OPCODE_UPDATE, 0);

    if (-1 == ret)
    {
        zlog_err("%s[%d]:leave %s:error:fail to send port hide msg to ifm,unit = %d, slot = %d\n", __FILE__, __LINE__, __func__, unit, slot);
    }
    else
    {
        zlog_notice("%s[%d]:leave %s:sucessfully to send port hide msg to ifm,unit = %d, slot = %d\n", __FILE__, __LINE__, __func__, unit, slot);
    }

    device.punit[unit - 1]->pslot[slot - 1]->plugout = 1;


    /*通知其他进程板卡拔出*/
    memset(&t_devm_com, 0, sizeof(struct devm_com));
    t_devm_com.unit = unit;
    t_devm_com.slot = slot;
    t_devm_com.main_slot = device.pmyunit->slot_main_board;
    t_devm_com.slave_slot = device.pmyunit->slot_slave_board;
    t_devm_com.type = device.punit[unit - 1]->pslot[slot - 1]->type;
    t_devm_com.status = DEV_STATUS_PLUGOUT;
    t_devm_com.ha_role = ha_dev_ha_role;
    t_devm_com.ha_status = myboard_dev_start_status;
    memcpy(t_devm_com.mac, device.punit[unit - 1]->pslot[slot - 1]->mac, sizeof(t_devm_com.mac));
    devm_event_notify(DEV_EVENT_SLOT_DELETE, &t_devm_com);

    /*设定1min定时器，到时间板卡未插入，删除数据结构*/
    index = unit << 8 | slot;
    devm_set_a_timer(PLUGOUT_TIME, devm_plug_timer_func, (void *)&index, sizeof(index), slot);

    /*send slot plugout alarm*/
    if (0 != device.pmyunit->myslot && device.pmyunit->slot_slave_board != device.pmyunit->myslot)
    {
        //ipran_alarm_report (IFM_EQU_TYPE, slot, 0, 0, 0, GPN_ALM_TYPE_UNIT_MIS, GPN_SOCK_MSG_OPT_RISE);      //告警产生

        memset(&usp, 0, sizeof(struct ifm_usp));
        usp.type = IFNET_TYPE_MAX;
        usp.unit = unit;
        usp.slot = slot;
        usp.port = 0;
        memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
        gPortInfo.iAlarmPort = IFM_EQU_TYPE;
        gPortInfo.iIfindex = ifm_get_ifindex_by_usp(&usp);
        ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_UNIT_MIS, GPN_SOCK_MSG_OPT_RISE);

        zlog_notice("%s[%d]send slot %d plugout info to alarm modul\n", __func__, __LINE__, slot);
    }

    zlog_notice("%s[%d]:set slot (%d,%d) plugout\n", __func__, __LINE__, unit, slot);

    return ERRNO_SUCCESS;
}


/* 创建板卡，要保证 unit 已经存在 */
int devm_add_slot(uint8_t unit, uint8_t slot, const struct devm_slot *pslot)
{
    struct devm_com t_devm_com;
    struct devm_slot *p_devm_slot = NULL;
    struct devm_unit *punit = NULL;
    struct ifm_usp usp;
    struct gpnPortInfo gPortInfo;

    if (NULL == pslot)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para pslot\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    punit = device.punit[unit - 1];

    if (NULL == punit)
    {
        zlog_err("%s[%d]:leave %s:error:can not add slot, unit %d not exist\n", __FILE__, __LINE__, __func__, unit - 1);
        return ERRNO_FAIL;
    }

    if (NULL != punit->pslot[slot - 1])
    {
        zlog_err("%s[%d]:leave %s:error:fail to create slot, slot already exist, unit=%d, slotu=%d\n", __FILE__, __LINE__, __func__ , unit, slot);
        return ERRNO_FAIL;
    }

    p_devm_slot = (struct devm_slot *)XCALLOC(MTYPE_DEVM_ENTRY, sizeof(struct devm_slot));

    if (NULL == p_devm_slot)
    {
        zlog_err("%s[%d]:leave %s:error:fail to XCALLOC struct devm_slot\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    zlog_notice("%s[%d]:%s: prepare create slot, unitid=%d,slot=%d\n", __FILE__, __LINE__, __func__, unit, slot);

    memcpy(p_devm_slot, pslot, sizeof(struct devm_slot));

    p_devm_slot->port_list.head = NULL;
    p_devm_slot->port_list.tail = NULL;
    p_devm_slot->card_list.head = NULL;
    p_devm_slot->card_list.tail = NULL;
    p_devm_slot->port_num = 0;

    punit->pslot[slot - 1] = p_devm_slot;

    /*通知其他进程板卡插入*/
    memset(&t_devm_com, 0, sizeof(struct devm_com));
    t_devm_com.unit = unit;
    t_devm_com.slot = slot;
    t_devm_com.main_slot = device.pmyunit->slot_main_board;
    t_devm_com.slave_slot = device.pmyunit->slot_slave_board;
    t_devm_com.type = device.punit[unit - 1]->pslot[slot - 1]->type;
    t_devm_com.status = DEV_STATUS_PLUGIN;
    t_devm_com.ha_role = ha_dev_ha_role;
    t_devm_com.ha_status = myboard_dev_start_status;
    memcpy(t_devm_com.mac, device.punit[unit - 1]->pslot[slot - 1]->mac, sizeof(t_devm_com.mac));
    devm_event_notify(DEV_EVENT_SLOT_ADD, &t_devm_com);

    if (0 != device.pmyunit->myslot && device.pmyunit->slot_slave_board != device.pmyunit->myslot)
    {
        //ipran_alarm_report (IFM_EQU_TYPE, slot, 0, 0, 0, GPN_ALM_TYPE_UNIT_MIS, GPN_SOCK_MSG_OPT_CLEAN);     //告警产生

        memset(&usp, 0, sizeof(struct ifm_usp));
        usp.type = IFNET_TYPE_MAX;
        usp.unit = unit;
        usp.slot = slot;
        usp.port = 0;
        memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
        gPortInfo.iAlarmPort = IFM_EQU_TYPE;
        gPortInfo.iIfindex = ifm_get_ifindex_by_usp(&usp);
        ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_UNIT_MIS, GPN_SOCK_MSG_OPT_CLEAN);

        zlog_notice("%s[%d]send slot %d create info to alarm modul\n", __func__, __LINE__, pslot->slot);
    }

    zlog_notice("%s[%d]:%s: sucessfully create slot, unitid=%d,slot=%d\n", __FILE__, __LINE__, __func__, unit, slot);

    return ERRNO_SUCCESS;
}


int devm_delete_slot(uint8_t unit, uint8_t slot)
{
    struct ifm_common ifcom;
    struct devm_com t_devm_com;
    struct devm_port *p_devm_port = NULL;
    struct listnode *p_listnode = NULL;

    if (NULL == device.punit[unit - 1])
    {
        zlog_err("%s[%d]:leave %s:error:can not delete slot, unit %d not exist\n", __FILE__, __LINE__, __func__, unit - 1);
        return ERRNO_FAIL;

    }
    else if (NULL == device.punit[unit - 1]->pslot[slot - 1])
    {
        zlog_err("%s[%d]:leave %s:error:can not delete slot, slot %d not exist\n", __FILE__, __LINE__, __func__, slot);
        return ERRNO_SUCCESS;
    }

    zlog_notice("%s[%d]:%s: prepare del slot, unitid=%d,slot=%d\n", __FILE__, __LINE__, __func__, unit, slot);

    /*通知其他进程板卡拔出*/
    memset(&t_devm_com, 0, sizeof(struct devm_com));
    t_devm_com.unit = unit;
    t_devm_com.slot = slot;
    t_devm_com.main_slot = device.pmyunit->slot_main_board;
    t_devm_com.slave_slot = device.pmyunit->slot_slave_board;
    t_devm_com.type = device.punit[unit - 1]->pslot[slot - 1]->type;
    t_devm_com.status = DEV_STATUS_PLUGOUT;
    t_devm_com.ha_role = ha_dev_ha_role;
    t_devm_com.ha_status = myboard_dev_start_status;
    memcpy(t_devm_com.mac, device.punit[unit - 1]->pslot[slot - 1]->mac, sizeof(t_devm_com.mac));
    devm_event_notify(DEV_EVENT_SLOT_DELETE, &t_devm_com);

    memset(&ifcom, 0, sizeof(struct ifm_common));
    ifcom.usp.unit = unit;
    ifcom.usp.slot = slot;

    /*check port list*/
    for (ALL_LIST_ELEMENTS_RO(&(device.punit[unit - 1]->pslot[slot - 1]->port_list), p_listnode, p_devm_port))
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:before delete slot, delete port %d\n", __FILE__, __LINE__, __func__, p_devm_port->port);
        devm_send_ifm_del_port(&ifcom, p_devm_port);
        XFREE(MTYPE_DEVM_ENTRY, p_devm_port);
        p_devm_port = NULL;
    }

    list_delete_all_node(&(device.punit[unit - 1]->pslot[slot - 1]->port_list));

    if (SLOT_TYPE_BOARD_SLAVE == device.pmyunit->pslot[slot - 1]->type)
    {
        device.punit[unit - 1]->slot_slave_board = 0;
        device.pmyunit->slot_slave_board = 0;
    }

    XFREE(MTYPE_DEVM_ENTRY, device.punit[unit - 1]->pslot[slot - 1]);
    device.punit[unit - 1]->pslot[slot - 1] = NULL;

    /*删除板卡对应的plugout定时器*/
    if (1 == plugout_timer_t[slot - 1].state)
    {
        devm_del_a_timer(slot);
    }

    /*send slot plugout alarm*/
    if (0 != device.pmyunit->myslot && device.pmyunit->slot_slave_board != device.pmyunit->myslot)
    {
        //ipran_alarm_report (IFM_EQU_TYPE, slot, 0, 0, 0, GPN_ALM_TYPE_UNIT_MIS, GPN_SOCK_MSG_OPT_RISE);      //告警产生

        struct ifm_usp usp;
        struct gpnPortInfo gPortInfo;
        memset(&usp, 0, sizeof(struct ifm_usp));
        usp.type = IFNET_TYPE_MAX;
        usp.unit = unit;
        usp.slot = slot;
        usp.port = 0;
        memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
        gPortInfo.iAlarmPort = IFM_EQU_TYPE;
        gPortInfo.iIfindex = ifm_get_ifindex_by_usp(&usp);
        ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_UNIT_MIS, GPN_SOCK_MSG_OPT_RISE);
        zlog_notice("%s[%d]send slot %d del info to alarm modult\n", __func__, __LINE__, slot);
    }

    zlog_notice("%s[%d]:leave %s:sucessfully to delete slot,slot=%d\n", __FILE__, __LINE__, __func__, slot);

    return ERRNO_SUCCESS;
}


/*由于数据结构变更，函数暂时搁置*/
int devm_add_card(uint8_t unit, uint8_t slot, uint8_t card, const struct devm_card *t_devm_card)
{
    struct devm_com t_devm_com;
    struct devm_card *p_devm_card = NULL;
    zlog_debug(DEVM_DBG_COMM, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    if (NULL == t_devm_card)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para t_devm_card\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    if (NULL == device.punit[unit - 1])
    {
        zlog_err("%s[%d]:leave %s:error:can not add_card, unit %d not exist\n", __FILE__, __LINE__, __func__, unit - 1);
        return ERRNO_FAIL;

    }
    else if (NULL == device.punit[unit - 1]->pslot[slot - 1])
    {
        zlog_err("%s[%d]:leave %s:error:fail to create card, slot not exist,unitid=%d,slotuid=%d\n", __FILE__, __LINE__, __func__ , unit, slot);
        return ERRNO_FAIL;
    }

    p_devm_card = (struct devm_card *)XCALLOC(MTYPE_DEVM_ENTRY, sizeof(struct devm_card));

    if (NULL == p_devm_card)
    {
        zlog_err("%s[%d]:leave %s:error:fail to XCALLOC struct p_devm_card\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    memcpy(p_devm_card, t_devm_card, sizeof(struct devm_card));
    listnode_add(&device.punit[unit - 1]->pslot[slot - 1]->card_list, p_devm_card);
    device.punit[unit - 1]->pslot[slot - 1]->card_num = listcount(&device.punit[unit - 1]->pslot[slot - 1]->card_list);

    /*通知其他进程板卡插入*/
    memset(&t_devm_com, 0, sizeof(struct devm_com));
    t_devm_com.unit = unit;
    t_devm_com.slot = card;
    t_devm_com.main_slot = device.pmyunit->slot_main_board;
    t_devm_com.slave_slot = device.pmyunit->slot_slave_board;
    t_devm_com.type = p_devm_card->type;
    t_devm_com.status = DEV_STATUS_PLUGIN;
    t_devm_com.ha_role = ha_dev_ha_role;
    t_devm_com.ha_status = myboard_dev_start_status;
    devm_event_notify(DEV_EVENT_SLOT_ADD, &t_devm_com);

    zlog_debug(DEVM_DBG_COMM, "%s[%d]:%s: sucessfully create card, unitid=%d,slotid=%d,cardid=%d\n", __FILE__, __LINE__, __func__, unit, slot, card);

    return ERRNO_SUCCESS;
}


/*由于数据结构变更，函数暂时搁置*/
int devm_delete_card(uint8_t unit, uint8_t slot, uint8_t card)
{
    struct ifm_common ifcom;
    struct devm_com t_devm_com;
    struct devm_port *p_devm_port = NULL;
    struct listnode *p_listnode = NULL;
    struct devm_card *p_devm_card = NULL;

    if (NULL == device.punit[unit - 1])
    {
        zlog_err("%s[%d]:leave %s:error:can not delete card, unit %d not exist\n", __FILE__, __LINE__, __func__, unit - 1);
        return ERRNO_FAIL;

    }
    else if (NULL == device.punit[unit - 1]->pslot[slot - 1])
    {
        zlog_err("%s[%d]:leave %s:error:can not delete card ,slot %d not exist\n", __FILE__, __LINE__, __func__, slot);
        return ERRNO_FAIL;
    }

    for (ALL_LIST_ELEMENTS_RO(&(device.punit[unit - 1]->pslot[slot - 1]->card_list), p_listnode, p_devm_card))
    {
        if (card == p_devm_card->slot)
        {
            /*通知其他进程板卡拔出*/
            memset(&t_devm_com, 0, sizeof(struct devm_com));
            t_devm_com.unit = unit;
            t_devm_com.slot = card;
            t_devm_com.main_slot = device.pmyunit->slot_main_board;
            t_devm_com.slave_slot = device.pmyunit->slot_slave_board;
            t_devm_com.type = p_devm_card->type;
            t_devm_com.status = DEV_STATUS_PLUGOUT;
            t_devm_com.ha_role = ha_dev_ha_role;
            t_devm_com.ha_status = myboard_dev_start_status;
            devm_event_notify(DEV_EVENT_SLOT_DELETE, &t_devm_com);

            /*通知ifm，删除端口*/
            memset(&ifcom, 0, sizeof(struct ifm_common));
            ifcom.usp.unit = unit;
            ifcom.usp.slot = slot;

            for (ALL_LIST_ELEMENTS_RO(&(p_devm_card->port_list), p_listnode, p_devm_port))
            {
                devm_send_ifm_del_port(&ifcom, p_devm_port);
                XFREE(MTYPE_DEVM_ENTRY, p_devm_port);
                p_devm_port = NULL;
            }

            list_delete_all_node(&(p_devm_card->port_list));

            /*删除card 节点*/
            XFREE(MTYPE_DEVM_ENTRY, p_devm_card);
            p_devm_card = NULL;
            list_delete_node(&(device.punit[unit - 1]->pslot[slot - 1]->card_list), p_listnode);

            zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to delete card,unitid=%d,slotid=%d,cardid=%d\n", __FILE__, __LINE__, __func__, unit, slot, card);
            return ERRNO_SUCCESS;
        }
    }

    zlog_err("%s[%d]:leave %s:error:can not find card ,unitid=%d,slotid=%d,cardid=%d \n", __FILE__, __LINE__, __func__, unit, slot, card);

    return ERRNO_SUCCESS;
}


static void devm_listnode_add_sort(struct list *list, struct devm_port *val)
{
    struct devm_port *n_data = NULL;
    struct listnode *p_node = NULL;
    struct listnode *new = NULL;

    assert(val != NULL);

    if (NULL == list || NULL == val)
    {
        zlog_err("%s[%d]:leave %s:invalid para list or val\n", __FILE__, __LINE__, __func__);
        return ;
    }

    new = listnode_new();
    new->data = val;

    for (p_node = list->head; p_node; p_node = p_node->next)
    {
        if (p_node->data)
        {
            n_data = (struct devm_port *)p_node->data;

            if (n_data->port > val->port && n_data->port_type == val->port_type)
            {
                new->next = p_node;
                new->prev = p_node->prev;

                if (p_node->prev)
                {
                    p_node->prev->next = new;
                }
                else
                {
                    list->head = new;
                }

                p_node->prev = new;
                list->count++;
                return;
            }
        }
        else
        {
            zlog_err("%s[%d]:leave %s:error:can not add port,p_node->data == NULL\n", __FILE__, __LINE__, __func__);
            return;
        }

    }

    new->prev = list->tail;

    if (list->tail)
    {
        list->tail->next = new;
    }
    else
    {
        list->head = new;
    }

    list->tail = new;
    list->count++;

    return;
}


int devm_add_port(uint8_t unit, uint8_t slot, struct list *p_port_list, struct devm_port *t_devm_port)
{
    struct devm_com t_devm_com;
    struct ifm_common ifcom;
    struct devm_port *p_devm_port = NULL;

    if (NULL == p_port_list || NULL == t_devm_port)
    {
        zlog_err("%s[%d]:leave %s:invalid para p_port_list or t_devm_port\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    p_devm_port = (struct devm_port *)XCALLOC(MTYPE_DEVM_ENTRY, sizeof(struct devm_port));

    if (NULL == p_devm_port)
    {
        zlog_err("%s[%d]:leave %s:error:fail to XCALLOC struct devm_port\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    memcpy(p_devm_port, t_devm_port, sizeof(struct devm_port));
    devm_listnode_add_sort(p_port_list, p_devm_port);

    memset(&ifcom, 0, sizeof(struct ifm_common));
    ifcom.usp.unit = unit;
    ifcom.usp.slot = slot;
    ifcom.usp.port = p_devm_port->port;
    devm_send_ifm_add_port(&ifcom, p_devm_port);

    /*通知其他进程端口创建*/
    memset(&t_devm_com, 0, sizeof(struct devm_com));
    t_devm_com.unit = unit;
    t_devm_com.slot = slot;

    if (NULL != device.pbox)
    {
        t_devm_com.main_slot = 1;
        t_devm_com.slave_slot = 1;
    }
    else if (NULL != device.pmyunit)
    {
        t_devm_com.main_slot = device.pmyunit->slot_main_board;
        t_devm_com.slave_slot = device.pmyunit->slot_slave_board;
    }

    t_devm_com.port = p_devm_port->port;
    t_devm_com.ha_role = ha_dev_ha_role;
    t_devm_com.ha_status = myboard_dev_start_status;

    if (DEV_TYPE_BOX == device.type)
    {
        t_devm_com.type = SLOT_TYPE_BOARD_MAIN;
    }
    else
    {
        if (0 == slot)
        {
            if (device.punit[device.main_unit - 1]->myslot == device.punit[device.main_unit - 1]->slot_main_board)
            {
                t_devm_com.type = device.punit[unit - 1]->pslot[device.punit[device.main_unit - 1]->slot_main_board - 1]->type;
            }
            else if (device.punit[device.main_unit - 1]->myslot == device.punit[device.main_unit - 1]->slot_slave_board)
            {
                t_devm_com.type = device.punit[unit - 1]->pslot[device.punit[device.main_unit - 1]->slot_slave_board - 1]->type;
            }
            else
            {
                return ERRNO_FAIL;
            }
        }
        else
        {
            t_devm_com.type = device.punit[unit - 1]->pslot[slot - 1]->type;
        }
    }

    devm_event_notify(DEV_EVENT_PORT_ADD, &t_devm_com);

    zlog_notice("%s[%d]:%s: sucessfully create port , unit=%d,slot=%d,port=%d,port type=%d,\n", __FILE__, __LINE__, __func__, unit, slot, p_devm_port->port, p_devm_port->port_type);

    return ERRNO_SUCCESS;
}


int devm_delete_port(uint8_t unit, uint8_t slot, struct list *p_port_list, struct devm_port *t_devm_port)
{
    struct ifm_common ifcom;
    struct devm_port *p_devm_port = NULL;
    struct listnode *p_listnode = NULL;
//  struct devm_com t_devm_com;

    if (NULL == p_port_list || NULL == t_devm_port)
    {
        zlog_err("%s[%d]:leave %s:invalid para p_port_list or t_devm_port\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    if (NULL == device.punit[unit - 1] || NULL == device.punit[unit - 1]->pslot[slot - 1])
    {
        zlog_err("%s[%d]:leave %s:unit[%d-1].slot[%d-1] not exist\n", __FILE__, __LINE__, __func__, unit, slot);
        return ERRNO_FAIL;
    }

    memset(&ifcom, 0, sizeof(struct ifm_common));
    ifcom.usp.unit = unit;
    ifcom.usp.slot = slot;

    for (ALL_LIST_ELEMENTS_RO(&(device.punit[unit - 1]->pslot[slot - 1]->port_list), p_listnode, p_devm_port))
    {
        if (!memcmp(p_devm_port, t_devm_port, sizeof(struct devm_port)))
        {
            zlog_notice("%s[%d]:%s:delete port unit=%d,slot=%d port %d,port type = %d，\n", __FILE__, __LINE__, __func__, unit, slot, t_devm_port->port, t_devm_port->port_type);
            devm_send_ifm_del_port(&ifcom, p_devm_port);
            XFREE(MTYPE_DEVM_ENTRY, p_devm_port);
            p_devm_port = NULL;
            list_delete_node(&(device.punit[unit - 1]->pslot[slot - 1]->port_list), p_listnode);

            break;
        }
    }

    return ERRNO_SUCCESS;
}


int devm_add_power(uint8_t unit, uint8_t powerslot, const struct devm_power *ppower)
{
    int func_ret = 0;
    int power_mode = 0;
    struct devm_power temp_devm_power;
    struct ifm_usp usp;
    struct gpnPortInfo gPortInfo;

    if (NULL != device.punit[unit - 1])
    {
        if (0 != device.punit[unit - 1]->myslot)
        {
            memset(&device.punit[unit - 1]->power[powerslot - 1], 0, sizeof(struct devm_power));

            if (device.pmyunit->slot_slave_board != device.pmyunit->myslot)
            {
                //ipran_alarm_report (IFM_EQU_TYPE, slot, 0, 0, 0, GPN_ALM_TYPE_POW_MIS, GPN_SOCK_MSG_OPT_CLEAN);   /*发送电源盘插入告警信息*/

                /* modify for ipran by lipf, 2018/4/25 */
                memset(&usp, 0, sizeof(struct ifm_usp));
                usp.type = IFNET_TYPE_MAX;
                usp.unit = unit;
                if(1 == powerslot)
                    usp.slot = gPwrSlot1;
                else if(2 == powerslot)
                    usp.slot = gPwrSlot2;
                usp.port = 0;
                memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
                gPortInfo.iAlarmPort = IFM_EQU_TYPE;
                gPortInfo.iIfindex = ifm_get_ifindex_by_usp(&usp);
                ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_POW_MIS, GPN_SOCK_MSG_OPT_CLEAN);
                zlog_notice("%s[%d]send power add info to alarm modult,power = %d\n", __func__, __LINE__, powerslot);

                memcpy(&temp_devm_power, ppower, sizeof(struct devm_power));
                power_mode = devm_get_power_alarm_type_by_mode(temp_devm_power.mode);
#if 0
                if (0 != power_mode && -1 != power_mode)
                {
                    devm_check_power_alarm(unit, powerslot, &device.punit[unit - 1]->power[powerslot - 1], &temp_devm_power, power_mode);
                }
#endif                
            }

            memcpy(&device.punit[unit - 1]->power[powerslot - 1], ppower, sizeof(struct devm_power));

            func_ret = 0;
        }
        else
        {
            zlog_err("%s[%d]:leave %s:invalid para , 0 == device.pmyunit->myslot(%d), powerslot = %d\n", __FILE__, __LINE__, __func__, device.pmyunit->myslot, powerslot);
            func_ret = 1;
        }
    }
    else
    {
        zlog_err("%s[%d]:leave %s:invalid para , NULL == device.punit[%d]\n", __FILE__, __LINE__, __func__, unit - 1);
        func_ret = 1;
    }

    return func_ret;
}


int devm_del_power(uint8_t unit, uint8_t powerslot)
{
    int func_ret = 0;
    struct devm_power temp_devm_power;

    struct ifm_usp usp;
    struct gpnPortInfo gPortInfo;

    if (NULL != device.punit[unit - 1])
    {
        if (0 != device.pmyunit->myslot)
        {
            int power_mode = 0;

            if (device.pmyunit->slot_slave_board != device.pmyunit->myslot)
            {
                memset(&temp_devm_power, 0, sizeof(struct devm_power));
                power_mode = devm_get_power_alarm_type_by_mode(device.punit[unit - 1]->power[powerslot - 1].mode);

                if (0 != power_mode &&  -1 != power_mode)
                {
                    devm_check_power_alarm(unit, powerslot, &device.punit[unit - 1]->power[powerslot - 1], &temp_devm_power, power_mode);
                }

                //ipran_alarm_report (IFM_EQU_TYPE, slot, 0, 0, 0, GPN_ALM_TYPE_POW_MIS, GPN_SOCK_MSG_OPT_RISE);/*���͵�Դ�̰γ��澯��Ϣ*/

                /* modify for ipran by lipf, 2018/4/25 */
                memset(&usp, 0, sizeof(struct ifm_usp));
                usp.type = IFNET_TYPE_MAX;
                usp.unit = unit;
                if(1 == powerslot)
                    usp.slot = gPwrSlot1;
                else if(2 == powerslot)
                    usp.slot = gPwrSlot2;
                usp.port = 0;
                memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
                gPortInfo.iAlarmPort = IFM_EQU_TYPE;
                gPortInfo.iIfindex = ifm_get_ifindex_by_usp(&usp);
                ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_POW_MIS, GPN_SOCK_MSG_OPT_RISE);
                memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
                gPortInfo.iAlarmPort = power_mode;
                gPortInfo.iIfindex = ifm_get_ifindex_by_usp(&usp);
                ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_POW_DOWN, GPN_SOCK_MSG_OPT_CLEAN);
                vol_alarm_record_output[powerslot - 1] = VOL_OUTPUT_POWER_UP_ALARM;

                zlog_notice("%s[%d]send power delete info to alarm modult,powerslot = %d\n", __func__, __LINE__, powerslot);
            }

            memset(&device.punit[unit - 1]->power[powerslot - 1], 0, sizeof(struct devm_power));
            device.punit[unit - 1]->power[powerslot - 1].mode = 0x7;

            func_ret = 0;
        }
        else
        {
            zlog_err("%s[%d]:leave %s:invalid para , 0 == device.pmyunit->myslot(%d), powerslot = %d\n", __FILE__, __LINE__, __func__, device.pmyunit->myslot, powerslot);
            func_ret = 1;
        }
    }
    else
    {
        zlog_err("%s[%d]:leave %s:invalid para , NULL == device.punit[%d]\n", __FILE__, __LINE__, __func__, unit - 1);
        func_ret = 1;
    }

    return func_ret;
}


int devm_update_power(uint8_t unit, uint8_t powerslot, const struct devm_power *ppower)
{
    int func_ret = 0;
    struct devm_power temp_devm_power;
    struct ifm_usp usp;
    struct gpnPortInfo gPortInfo;

    if (NULL != device.punit[unit - 1])
    {
        int power_mode = 0;

        power_mode = devm_get_power_alarm_type_by_mode(ppower->mode);

        if (0 != power_mode)
        {
            memcpy(&temp_devm_power, ppower, sizeof(struct devm_power));

            if (0 != power_mode &&  -1 != power_mode)
            {
                devm_check_power_alarm(unit, powerslot, &device.punit[unit - 1]->power[powerslot - 1], &temp_devm_power, power_mode);
            }
            else if(-1 == power_mode)
            {
                memset(&usp, 0, sizeof(struct ifm_usp));
                usp.type = IFNET_TYPE_MAX;
                usp.unit = unit;
                if(1 == powerslot)
                    usp.slot = gPwrSlot1;
                else if(2 == powerslot)
                    usp.slot = gPwrSlot2;
                usp.port = 0;
                memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
                gPortInfo.iAlarmPort = IFM_EQU_TYPE;
                gPortInfo.iIfindex = ifm_get_ifindex_by_usp(&usp);
                ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_POW_MIS, GPN_SOCK_MSG_OPT_RISE);
            }

            memcpy(&device.punit[unit - 1]->power[powerslot - 1], &temp_devm_power, sizeof(struct devm_power));

            func_ret = 0;
        }
        else
        {
            zlog_err("%s[%d]send power mode error(%d),should be 0/2/4/6\n", __func__, __LINE__, ppower->mode);
            func_ret = 1;
        }
    }
    else
    {
        func_ret = 1;
        zlog_err("%s[%d]NULL == device.punit[%d]\n", __func__, __LINE__, unit - 1);
    }

    return func_ret;
}


int devm_get_fan_speed(unsigned short *fan_speed)
{
    int ret = 0;
    int index = 0;
    struct ipc_mesg_n* pmsg = NULL;

    index |= (1 << 8);
    index |= 14;

//    p_fan_speed = ipc_send_hal_wait_reply(NULL, 0, 1 , MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_FAN, IPC_OPCODE_GET, index);
    pmsg = ipc_sync_send_n2(NULL, 0, 1, 
		 MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_FAN, IPC_OPCODE_GET, index, 1000);
    if(NULL == pmsg)
    {
        ret = 1;
    }
    else
    {
        memcpy(fan_speed, pmsg->msg_data, sizeof(unsigned short));
        ret = 0;
        mem_share_free(pmsg, MODULE_ID_DEVM);
    }
    return ret;
}


int devm_add_fan(uint8_t unit, uint8_t slot, const struct devm_fan *pfan)
{
    int ret = 0;

    struct ifm_usp usp;
    struct gpnPortInfo gPortInfo;

    if (NULL != device.punit[unit - 1])
    {
        device.punit[unit - 1]->fan.speed_min = FAN_SPEED_MIN;
        device.punit[unit - 1]->fan.speed_max = FAN_SPEED_MAX;

        if (NULL != device.pmyunit && 0 != device.pmyunit->myslot)
        {
            if (device.pmyunit->slot_slave_board != device.pmyunit->myslot)
            {
                /* modify for ipran by lipf, 2018/4/25 */
                memset(&usp, 0, sizeof(struct ifm_usp));
                usp.type = IFNET_TYPE_MAX;
                usp.unit = unit;
                usp.slot = slot;
                usp.port = 0;
                memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
                gPortInfo.iAlarmPort = IFM_EQU_TYPE;
                gPortInfo.iIfindex = ifm_get_ifindex_by_usp(&usp);
                ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FAN_MIS, GPN_SOCK_MSG_OPT_CLEAN);
                zlog_notice("%s[%d]send fan add info to alarm modult,slot = %d\n", __func__, __LINE__, slot);
            }
        }

        ret = 0;
    }
    else
    {
        zlog_err("%s[%d]:leave %s:invalid para , NULL != device.punit[%d-1]\n", __FILE__, __LINE__, __func__, unit);
        ret = 1;
    }

    return ret;
}


int devm_del_fan(uint8_t unit, uint8_t slot)
{
    struct ifm_usp usp;
    struct gpnPortInfo gPortInfo;
    int ret = 0;

    if (NULL != device.punit[unit - 1])
    {
        memset(&device.punit[unit - 1]->fan, 0, sizeof(struct devm_fan));

        if (NULL != device.pmyunit && 0 != device.pmyunit->myslot)
        {
            if (device.pmyunit->slot_slave_board != device.pmyunit->myslot)
            {
                //ipran_alarm_report (IFM_EQU_TYPE, slot, 0, 0, 0, GPN_ALM_TYPE_FAN_MIS, GPN_SOCK_MSG_OPT_RISE); //告警产生

                /* modify for ipran by lipf, 2018/4/25 */
                memset(&usp, 0, sizeof(struct ifm_usp));
                usp.type = IFNET_TYPE_MAX;
                usp.unit = unit;
                usp.slot = slot;
                usp.port = 0;
                memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
                gPortInfo.iAlarmPort = IFM_EQU_TYPE;
                gPortInfo.iIfindex = ifm_get_ifindex_by_usp(&usp);
                ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FAN_MIS, GPN_SOCK_MSG_OPT_RISE);
                zlog_notice("%s[%d]send fan delete info to alarm modult,slot = %d\n", __func__, __LINE__, slot);
            }
        }

        ret = 0;
    }
    else
    {
        zlog_err("%s[%d]:leave %s:invalid para , NULL != device.punit[%d-1]\n", __FILE__, __LINE__, __func__, unit);
        ret = 1;
    }

    return ret;
}


int devm_check_temp_alarm(signed char temperature, struct devm_temp *temp)
{
    int ret = 0;
    char temperature_flag = 0;

    /* modify for ipran by lipf, 2018/4/25 */
    struct ifm_usp usp;
    struct gpnPortInfo gPortInfo;
    memset(&usp, 0, sizeof(struct ifm_usp));
    usp.type = IFNET_TYPE_MAX;
    usp.unit = 1;
    usp.slot = 0;
    usp.port = 0;
    memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
    gPortInfo.iAlarmPort = IFM_ENV_TYPE;
    gPortInfo.iIfindex = ifm_get_ifindex_by_usp(&usp);


    if (temperature > temp->temp_high && temperature_alarm_record != TEMPERATURE_INPUT_HIGH_ALARM)
    {
        temperature_flag = 1;
        //ipran_alarm_report (IFM_ENV_TYPE, 0, 0, 0, 0, GPN_ALM_TYPE_TEM_H_ALM, GPN_SOCK_MSG_OPT_RISE);            //告警产生

        ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_TEM_H_ALM, GPN_SOCK_MSG_OPT_RISE);
        temperature_alarm_record = TEMPERATURE_INPUT_HIGH_ALARM;
        zlog_debug(DEVM_DBG_COMM, "%s[%d]send add temp high create info to alarm modult,temp = %d\n", __func__, __LINE__, temperature);

        /*send temp high msg to l2 module*/
        ret = ipc_send_msg_n2(&temperature_flag, sizeof(temperature_flag), 1, MODULE_ID_L2, MODULE_ID_DEVM,
                               IPC_TYPE_EFM, 0, IPC_OPCODE_EVENT , 0);

        if (ret)
        {
            zlog_err("%s[%d]:leave %s:error:fail to send temperature high msg to l2, ret = %d!\n", __FILE__, __LINE__, __func__ , ret);
        }
        else
        {
            zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to send temperature high msg to l2!\n", __FILE__, __LINE__, __func__);
        }
    }
    else if (temperature < temp->temp_low &&  temperature_alarm_record != TEMPERATURE_INPUT_LOW_ALARM)
    {
        temperature_flag = 1;
        //ipran_alarm_report (IFM_ENV_TYPE, 0, 0, 0, 0, GPN_ALM_TYPE_TEM_L_ALM, GPN_SOCK_MSG_OPT_RISE);        //告警产生

        ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_TEM_L_ALM, GPN_SOCK_MSG_OPT_RISE);
        temperature_alarm_record = TEMPERATURE_INPUT_LOW_ALARM;
        zlog_debug(DEVM_DBG_COMM, "%s[%d]send add temp low info to alarm modult,temp = %d\n", __func__, __LINE__, temperature);

        /*send temp low msg to l2 module*/
        ret = ipc_send_msg_n2(&temperature_flag, sizeof(temperature_flag), 1, MODULE_ID_L2, MODULE_ID_DEVM,
                               IPC_TYPE_EFM, 0, IPC_OPCODE_EVENT , 0);

        if (ret)
        {
            zlog_err("%s[%d]:leave %s:error:fail to send temperature low msg to l2, ret = %d!\n", __FILE__, __LINE__, __func__ , ret);
        }
        else
        {
            zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to send temperature low msg to l2!\n", __FILE__, __LINE__, __func__);
        }
    }
    else if ((temperature >= temp->temp_low) && (temperature <= temp->temp_high))
    {
        if (temperature_alarm_record == TEMPERATURE_INPUT_LOW_ALARM)
        {
            temperature_flag = 0;

//              alarm_report(IPC_OPCODE_CLEAR,MODULE_ID_DEVM,ALM_CODE_ENV_TEM_LOW  ,index,0,0,0);/*发送告警信息*/
            //ipran_alarm_report (IFM_ENV_TYPE, 0, 0, 0, 0, GPN_ALM_TYPE_TEM_L_ALM, GPN_SOCK_MSG_OPT_CLEAN);           //告警产生

            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_TEM_L_ALM, GPN_SOCK_MSG_OPT_CLEAN);

            temperature_alarm_record = TEMPERATURE_NORMAL;
            zlog_debug(DEVM_DBG_COMM, "%s[%d]send clear temp low info to alarm modult,temp = %d\n", __func__, __LINE__, temperature);

            /*send temp normal msg to l2 module*/
            ret = ipc_send_msg_n2(&temperature_flag, sizeof(temperature_flag), 1, MODULE_ID_L2, MODULE_ID_DEVM,
                                   IPC_TYPE_EFM, 0, IPC_OPCODE_EVENT , 0);

            if (ret)
            {
                zlog_err("%s[%d]:leave %s:error:fail to send temperature low msg to l2, ret = %d!\n", __FILE__, __LINE__, __func__ , ret);
            }
            else
            {
                zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to send temperature low msg to l2!\n", __FILE__, __LINE__, __func__);
            }
        }
        else if (temperature_alarm_record == TEMPERATURE_INPUT_HIGH_ALARM)
        {
            temperature_flag = 0;

//              alarm_report(IPC_OPCODE_CLEAR,MODULE_ID_DEVM,ALM_CODE_ENV_TEM_HIGH ,index,0,0,0);/*发送告警信息*/
            //ipran_alarm_report (IFM_ENV_TYPE, 0, 0, 0, 0, GPN_ALM_TYPE_TEM_H_ALM, GPN_SOCK_MSG_OPT_CLEAN);           //告警产生

            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_TEM_H_ALM, GPN_SOCK_MSG_OPT_CLEAN);

            temperature_alarm_record = TEMPERATURE_NORMAL;
            zlog_debug(DEVM_DBG_COMM, "%s[%d]send clear temp high create info to alarm modult,temp = %d\n", __func__, __LINE__, temperature);

            /*send temp normal msg to l2 module*/
            ret = ipc_send_msg_n2(&temperature_flag, sizeof(temperature_flag), 1, MODULE_ID_L2, MODULE_ID_DEVM,
                                   IPC_TYPE_EFM, 0, IPC_OPCODE_EVENT , 0);

            if (ret)
            {
                zlog_err("%s[%d]:leave %s:error:fail to send temperature low msg to l2, ret = %d!\n", __FILE__, __LINE__, __func__ , ret);
            }
            else
            {
                zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to send temperature low msg to l2!\n", __FILE__, __LINE__, __func__);
            }
        }
    }

    return 0;
}


int devm_update_temp(uint8_t unit, uint8_t slot, const char *ptemp)
{
    int ret = 0;

    switch (device.type)
    {
        case DEV_TYPE_BOX:
            device.pbox->temp.temprature = *ptemp;
            devm_check_temp_alarm(*ptemp, &device.pbox->temp);
            ret = 0;
            break;

        case DEV_TYPE_SHELF:
            device.punit[unit - 1]->pslot[slot - 1]->temp.temprature = *ptemp;
            devm_check_temp_alarm(*ptemp, &device.punit[unit - 1]->pslot[slot - 1]->temp);
            ret = 0;
            break;

        default:
            ret = 1;
            break;
    }

    return ret;
}


int devm_update_slave_temp(uint8_t unit, uint8_t slot, const char *ptemp)
{
    int ret = 0;
	int index = 0;
	if(NULL == ptemp || NULL == device.punit[unit - 1]->pslot[slot - 1])
		return 1;
    switch (device.type)
    {
        case DEV_TYPE_BOX:
            device.pbox->temp.temprature = *ptemp;
            devm_check_temp_alarm(*ptemp, &device.pbox->temp);
            ret = 0;
            break;

        case DEV_TYPE_SHELF:
//            printf("%s %d:---get slave temperatue---unit=%d---slot=%d----*ptemp=%d----\n",__func__,__LINE__,unit,slot,*ptemp);
            device.punit[unit - 1]->pslot[slot - 1]->temp.temprature = *ptemp;
	        index = 0;
	        index |= unit << 8;
	        index |= slot;

			ret = ipc_send_msg_n2(ptemp, 1, 1, MODULE_ID_HAL, MODULE_ID_DEVM,
								  IPC_TYPE_DEVM, DEVM_INFO_SLAVE_TEMP, IPC_OPCODE_UPDATE,index);
			
			if (-1 == ret)
			{
				zlog_err("%s[%d]:leave %s:error:fail to send slot add finish msg to ifm\n", __FILE__, __LINE__, __func__);
			}
			else
			{
//				printf("%s[%d]:leave %s:sucessfully to to send slot add finish msg to ifm\n", __FILE__, __LINE__, __func__);
				zlog_debug("%s[%d]:leave %s:sucessfully to to send slot add finish msg to ifm\n", __FILE__, __LINE__, __func__);
			}

            devm_check_temp_alarm(*ptemp, &device.punit[unit - 1]->pslot[slot - 1]->temp);
            ret = 0;
            break;

        default:
            ret = 1;
            break;
	}
    return ret;
}

int temper_send_to_master(struct thread *para_thread)
{
    int ret;
    int unit = 1;
    int index;
	if(HA_ROLE_SLAVE == ha_dev_ha_role)
	{    
	    if(device.myslot != 0)
	    {
	        index = 0;
	        index |= (device.myunit << 8);
	        index |= device.myslot;
//	        ret = ipc_send_ha(&device.punit[unit - 1]->pslot[device.myslot - 1]->temp.temprature, 1, 1, MODULE_ID_DEVM,
//	                          IPC_TYPE_DEVM, DEVM_INFO_SLAVE_TEMP, IPC_OPCODE_UPDATE , index);
            ret = ipc_send_msg_ha_n2(&device.punit[unit - 1]->pslot[device.myslot - 1]->temp.temprature, 1, 1, MODULE_ID_DEVM,  
                                MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_SLAVE_TEMP, IPC_OPCODE_UPDATE, index, 0);

//	        printf("%s %d:------device.punit[unit - 1]->pslot[device.pmyunit->myslot - 1]->temp.temprature=%d------\n",__func__,__LINE__,device.punit[unit - 1]->pslot[device.pmyunit->myslot - 1]->temp.temprature);
	        if (ret)
	        {
	            zlog_err("%s[%d]:leave %s:error:fail to send msg temperature to master\n", __FILE__, __LINE__, __func__ );
	        }
	        else
	        {
	            zlog_debug("%s[%d]:leave %s:sucessfully to send msg temperature master \n", __FILE__, __LINE__, __func__ );
	        }        
	    }     
	}

//    thread_add_timer(devm_master, temper_send_to_master, NULL, 3);

    return 0;
}

int devm_update_fan(uint8_t unit, uint8_t slot, const struct devm_fan *pfan)
{
    int ret = 0;

    if (NULL != device.punit[unit - 1])
    {
        memcpy(&device.punit[unit - 1]->fan, pfan, sizeof(struct devm_fan));
        ret = 0;
    }
    else
    {
        ret = 1;
    }

    return ret;
}


int devm_update_slot_partnerslot(void)
{
    if (NULL != device.pmyunit)
    {
        myslot = device.pmyunit->myslot;
        partnerslot = (device.pmyunit->myslot == device.pmyunit->slot_main_board) ? device.pmyunit->slot_slave_board : device.pmyunit->slot_main_board;
    }

    return 0;
}


int devm_pthread_create(pthread_t *pthread_id, void *(*pthread_func)(void *arg), void *arg)
{
    int ret = 0;
    pthread_attr_t pthread_att;
    pthread_t t_pthread_id;

    ret = pthread_attr_init(&pthread_att);

    if (ret != 0)
    {
        zlog_err("%s[%d]:leave %s:error:fail to init pthread_attr_init,ret = %d,error = %s\n", __FILE__, __LINE__, __func__, ret, strerror(errno));
    }
    else
    {
        ret = pthread_attr_setdetachstate(&pthread_att, PTHREAD_CREATE_DETACHED);

        if (0 == ret)
        {
            ret = pthread_create(&t_pthread_id, &pthread_att, pthread_func, arg);

            if (0 != ret)
            {
                zlog_err("%s[%d]:leave %s:error:fail to create pthread ,ret = %d,error = %s\n", __FILE__, __LINE__, __func__, ret, strerror(errno));
            }
            else
            {
                *pthread_id = t_pthread_id;
                zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to create pthread ,ret = %d\n", __FILE__, __LINE__, __func__, ret);
            }
        }

        pthread_attr_destroy(&pthread_att);

        if (0 != ret)
        {
            zlog_err("%s[%d]:leave %s:error:fail to destory pthread_att,ret = %d,error = %s\n", __FILE__, __LINE__, __func__, ret, strerror(errno));
        }
    }

    return ret;
}


#if 0
int devm_check_power_alarm_test(uint8_t unit, uint8_t slot , struct devm_power *prevpower, struct devm_power *postpower, int power_type)
{
    if (12 != slot && 13 != slot)
    {
        zlog_err("%s[%d]failed to check power alarm , slot %d error , not 12 or not 13\n", __func__, __LINE__, slot);
        return 1;
    }

    if (0 == power_type
            || -1 == power_type)
    {
        zlog_err("%s[%d]failed to check power alarm , power type error(%d) == (0 || -1)\n", __func__, __LINE__, power_type);
        return 1;
    }

    if (0 == prevpower->mode
            && 0 == prevpower->voltage_output
            && 0 == prevpower->voltage_input
            && 0 == prevpower->voltage_low
            && 0 == prevpower->voltage_high) /*power add*/
    {
        if (0 != postpower->voltage_input)
        {
            if (postpower->voltage_low > (postpower->voltage_input / 1000))
            {
                if (vol_alarm_record_input[slot - POWER_SLOT_START] != VOL_INPUT_LOW_ALARM)
                {
                    ipran_alarm_report(power_type, slot, 0, 0, 0, GPN_ALM_TYPE_VOL_L_ALM, GPN_SOCK_MSG_OPT_RISE);  //告警产生

                    vol_alarm_record_input[slot - POWER_SLOT_START] = VOL_INPUT_LOW_ALARM;
                }
            }
            else if (postpower->voltage_high < (postpower->voltage_input / 1000))
            {
                if (vol_alarm_record_input[slot - POWER_SLOT_START] != VOL_INPUT_HIGH_ALARM)
                {
                    ipran_alarm_report(power_type, slot, 0, 0, 0, GPN_ALM_TYPE_VOL_H_ALM, GPN_SOCK_MSG_OPT_RISE);  //告警产生

                    vol_alarm_record_input[slot - POWER_SLOT_START] = VOL_INPUT_HIGH_ALARM;
                }
            }
        }
    }
    else if ((0 == postpower->voltage_input && 0 != prevpower->voltage_input) /*dynamic vol close,clear alarm*/
             || (0 == prevpower->voltage_low && 0 == prevpower->voltage_high && 0 == postpower->voltage_low && 0 == postpower->voltage_high)) /*power del*/
    {
        ipran_alarm_report(power_type, slot, 0, 0, 0, GPN_ALM_TYPE_VOL_L_ALM, GPN_SOCK_MSG_OPT_CLEAN);  //clear vol low alarm
        ipran_alarm_report(power_type, slot, 0, 0, 0, GPN_ALM_TYPE_VOL_H_ALM, GPN_SOCK_MSG_OPT_CLEAN);  //clear vol high alarm
    }
    else /*power update*/
    {
        if (0 != postpower->voltage_input)
        {
            if (postpower->voltage_low > (postpower->voltage_input / 1000))
            {
                if (vol_alarm_record_input[slot - POWER_SLOT_START] != VOL_INPUT_LOW_ALARM)
                {
                    ipran_alarm_report(power_type, slot, 0, 0, 0, GPN_ALM_TYPE_VOL_L_ALM, GPN_SOCK_MSG_OPT_RISE);  //rise vol low alarm
                    vol_alarm_record_input[slot - POWER_SLOT_START] = VOL_INPUT_LOW_ALARM;
                }
            }
            else if (postpower->voltage_high < (postpower->voltage_input / 1000))
            {
                if (vol_alarm_record_input[slot - POWER_SLOT_START] != VOL_INPUT_HIGH_ALARM)
                {
                    ipran_alarm_report(power_type, slot, 0, 0, 0, GPN_ALM_TYPE_VOL_H_ALM, GPN_SOCK_MSG_OPT_RISE);  //rise vol high alarm
                    vol_alarm_record_input[slot - POWER_SLOT_START] = VOL_INPUT_HIGH_ALARM;
                }
            }
            else if (postpower->voltage_low <= (postpower->voltage_input / 1000) && (postpower->voltage_input / 1000) <= postpower->voltage_high)
            {
                if (vol_alarm_record_input[slot - POWER_SLOT_START] == VOL_INPUT_HIGH_ALARM)
                {
                    ipran_alarm_report(power_type, slot, 0, 0, 0, GPN_ALM_TYPE_VOL_H_ALM, GPN_SOCK_MSG_OPT_CLEAN);  //clear vol high alarm
                    vol_alarm_record_input[slot - POWER_SLOT_START] = VOL_NORMAL;
                }
                else if (vol_alarm_record_input[slot - POWER_SLOT_START] == VOL_INPUT_LOW_ALARM)
                {

                    ipran_alarm_report(power_type, slot, 0, 0, 0, GPN_ALM_TYPE_VOL_L_ALM, GPN_SOCK_MSG_OPT_CLEAN); //clear vol low alarm
                    vol_alarm_record_input[slot - POWER_SLOT_START] = VOL_NORMAL;
                }
            }
        }
        else
        {
            if (vol_alarm_record_input[slot - POWER_SLOT_START] == VOL_INPUT_HIGH_ALARM)
            {
                ipran_alarm_report(power_type, slot, 0, 0, 0, GPN_ALM_TYPE_VOL_H_ALM, GPN_SOCK_MSG_OPT_CLEAN);  //clear vol high alarm
                vol_alarm_record_input[slot - POWER_SLOT_START] = VOL_NORMAL;
            }
            else if (vol_alarm_record_input[slot - POWER_SLOT_START] == VOL_INPUT_LOW_ALARM)
            {
                ipran_alarm_report(power_type, slot, 0, 0, 0, GPN_ALM_TYPE_VOL_L_ALM, GPN_SOCK_MSG_OPT_CLEAN); //clear vol low alarm
                vol_alarm_record_input[slot - POWER_SLOT_START] = VOL_NORMAL;
            }
        }
    }

    return 0;
}
#endif


int devm_check_power_alarm(uint8_t unit, uint8_t powerslot , struct devm_power *prevpower, struct devm_power *postpower, int power_type)
{
    /* modify for ipran by lipf, 2018/4/25 */
    struct ifm_usp usp;
    struct gpnPortInfo gPortInfo;
    memset(&usp, 0, sizeof(struct ifm_usp));
    usp.type = IFNET_TYPE_MAX;
    usp.unit = unit; 
    if(1 == powerslot)
        usp.slot = gPwrSlot1;
    else if(2 == powerslot)
        usp.slot = gPwrSlot2;
    usp.port = 0;
    memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
    gPortInfo.iAlarmPort = power_type;
    gPortInfo.iIfindex = ifm_get_ifindex_by_usp(&usp);

    if (0 == power_type
            || -1 == power_type)
    {
        zlog_err("%s[%d]failed to check power alarm , power type error(%d) == (0 || -1)\n", __func__, __LINE__, power_type);
        return 1;
    }

    if (0 == prevpower->mode
            && 0 == prevpower->voltage_output
            && 0 == prevpower->voltage_input
            && 0 == prevpower->voltage_low
            && 0 == prevpower->voltage_high) /*power add*/
    {
        if(0 == postpower->voltage_output && 0x7 != postpower->mode)
        {
            memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
            gPortInfo.iAlarmPort = IFM_EQU_TYPE;
            gPortInfo.iIfindex = ifm_get_ifindex_by_usp(&usp);
            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_POW_MIS, GPN_SOCK_MSG_OPT_CLEAN);   
            
            memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
            gPortInfo.iAlarmPort = power_type;
            gPortInfo.iIfindex = ifm_get_ifindex_by_usp(&usp);            
            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_POW_DOWN, GPN_SOCK_MSG_OPT_RISE);
            vol_alarm_record_output[powerslot - 1] = VOL_OUTPUT_POWER_DOWN_ALARM;
        }

        if (0 != postpower->voltage_input)
        {
            if (postpower->voltage_low > (postpower->voltage_input / 1000))
            {
                if (vol_alarm_record_input[powerslot - 1] != VOL_INPUT_LOW_ALARM)
                {
                    //ipran_alarm_report (power_type, slot, 0, 0, 0, GPN_ALM_TYPE_VOL_L_ALM, GPN_SOCK_MSG_OPT_RISE); //�澯����

                    ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_VOL_L_ALM, GPN_SOCK_MSG_OPT_RISE);
                    vol_alarm_record_input[powerslot - 1] = VOL_INPUT_LOW_ALARM;
                }
            }
            else if (postpower->voltage_high < (postpower->voltage_input / 1000))
            {
                if (vol_alarm_record_input[powerslot - 1] != VOL_INPUT_HIGH_ALARM)
                {
                    //ipran_alarm_report (power_type, slot, 0, 0, 0, GPN_ALM_TYPE_VOL_H_ALM, GPN_SOCK_MSG_OPT_RISE); //告警产生

                    ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_VOL_H_ALM, GPN_SOCK_MSG_OPT_RISE);
                    vol_alarm_record_input[powerslot - 1] = VOL_INPUT_HIGH_ALARM;
                }
            }
        }
    }
#if 0    
    else if ((0 == postpower->voltage_input && 0 != prevpower->voltage_input) /*dynamic vol close,clear alarm*/
             || (0 == prevpower->voltage_low && 0 == prevpower->voltage_high && 0 == postpower->voltage_low && 0 == postpower->voltage_high)) /*power del*/
    {
//      ipran_alarm_report (power_type, slot, 0, 0, 0, GPN_ALM_TYPE_POW_DOWN, GPN_SOCK_MSG_OPT_CLEAN); //clear power down alarm

        //ipran_alarm_report (power_type, slot, 0, 0, 0, GPN_ALM_TYPE_VOL_L_ALM, GPN_SOCK_MSG_OPT_CLEAN); //clear vol low alarm
        //ipran_alarm_report (power_type, slot, 0, 0, 0, GPN_ALM_TYPE_VOL_H_ALM, GPN_SOCK_MSG_OPT_CLEAN); //clear vol high alarm

        ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_VOL_L_ALM, GPN_SOCK_MSG_OPT_CLEAN);
        ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_VOL_H_ALM, GPN_SOCK_MSG_OPT_CLEAN);
    }
#endif    
    else /*power update*/
    {
        if(0 != postpower->voltage_output )
        {
            if(vol_alarm_record_output[powerslot-1] == VOL_OUTPUT_POWER_DOWN_ALARM)
            {
                //ipran_alarm_report (power_type, slot, 0, 0, 0, GPN_ALM_TYPE_POW_DOWN, GPN_SOCK_MSG_OPT_CLEAN); //clear power down alarm
                ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_POW_DOWN, GPN_SOCK_MSG_OPT_CLEAN);
                vol_alarm_record_output[powerslot-1] = VOL_OUTPUT_POWER_UP_ALARM;
            }
        }
        else if(0x7 != postpower->mode)
        {
            if( vol_alarm_record_output[powerslot-1] != VOL_OUTPUT_POWER_DOWN_ALARM)
            {
                //ipran_alarm_report (power_type, slot, 0, 0, 0, GPN_ALM_TYPE_POW_DOWN, GPN_SOCK_MSG_OPT_RISE);  //rise power down alarm
                ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_POW_DOWN, GPN_SOCK_MSG_OPT_RISE);
                vol_alarm_record_output[powerslot-1] = VOL_OUTPUT_POWER_DOWN_ALARM;
            }
        }

        if (0 != postpower->voltage_input)
        {
            if (postpower->voltage_low > (postpower->voltage_input / 1000))
            {
                if (vol_alarm_record_input[powerslot - 1] != VOL_INPUT_LOW_ALARM)
                {
                    //ipran_alarm_report (power_type, slot, 0, 0, 0, GPN_ALM_TYPE_VOL_L_ALM, GPN_SOCK_MSG_OPT_RISE); //rise vol low alarm

                    ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_VOL_L_ALM, GPN_SOCK_MSG_OPT_RISE);
                    vol_alarm_record_input[powerslot - 1] = VOL_INPUT_LOW_ALARM;
                }
            }
            else if (postpower->voltage_high < (postpower->voltage_input / 1000))
            {
                if (vol_alarm_record_input[powerslot - 1] != VOL_INPUT_HIGH_ALARM)
                {
                    //ipran_alarm_report (power_type, slot, 0, 0, 0, GPN_ALM_TYPE_VOL_H_ALM, GPN_SOCK_MSG_OPT_RISE); //rise vol high alarm

                    ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_VOL_H_ALM, GPN_SOCK_MSG_OPT_RISE);
                    vol_alarm_record_input[powerslot - 1] = VOL_INPUT_HIGH_ALARM;
                }
            }
            else if (postpower->voltage_low <= (postpower->voltage_input / 1000) && (postpower->voltage_input / 1000) <= postpower->voltage_high)
            {
                if (vol_alarm_record_input[powerslot - 1] == VOL_INPUT_HIGH_ALARM)
                {
                    //ipran_alarm_report (power_type, slot, 0, 0, 0, GPN_ALM_TYPE_VOL_H_ALM, GPN_SOCK_MSG_OPT_CLEAN); //clear vol high alarm

                    ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_VOL_H_ALM, GPN_SOCK_MSG_OPT_CLEAN);
                    vol_alarm_record_input[powerslot - 1] = VOL_NORMAL;
                }
                else if (vol_alarm_record_input[powerslot - 1] == VOL_INPUT_LOW_ALARM)
                {

                    //ipran_alarm_report (power_type, slot, 0, 0, 0, GPN_ALM_TYPE_VOL_L_ALM, GPN_SOCK_MSG_OPT_CLEAN);//clear vol low alarm

                    ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_VOL_L_ALM, GPN_SOCK_MSG_OPT_CLEAN);
                    vol_alarm_record_input[powerslot - 1] = VOL_NORMAL;
                }
            }
        }
        else
        {
            if (vol_alarm_record_input[powerslot - 1] == VOL_INPUT_HIGH_ALARM)
            {
                //ipran_alarm_report (power_type, slot, 0, 0, 0, GPN_ALM_TYPE_VOL_H_ALM, GPN_SOCK_MSG_OPT_CLEAN); //clear vol high alarm

                ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_VOL_H_ALM, GPN_SOCK_MSG_OPT_CLEAN);
                vol_alarm_record_input[powerslot - 1] = VOL_NORMAL;
            }
            else if (vol_alarm_record_input[powerslot - 1] == VOL_INPUT_LOW_ALARM)
            {
                //ipran_alarm_report (power_type, slot, 0, 0, 0, GPN_ALM_TYPE_VOL_L_ALM, GPN_SOCK_MSG_OPT_CLEAN);//clear vol low alarm

                ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_VOL_L_ALM, GPN_SOCK_MSG_OPT_CLEAN);
                vol_alarm_record_input[powerslot - 1] = VOL_NORMAL;
            }
        }
    }

    return 0;
}


#if 1
#endif


int devm_get_hal_slot_plug_status_hash(unsigned int slot, unsigned char *slot_plug_status)
{
    int ret = 0;
    int index = 0;
//    struct ipc_mesg *p_ipc_mesg = NULL;
    struct ipc_mesg_n *pmsg = NULL;

    index |= (1 << 8);
    index |= slot;

//    p_ipc_mesg = ipc_send_hal_wait_reply1(NULL, 0, 1 , MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_HAL_SLOT_PLUGSTATUS_HASH, IPC_OPCODE_GET, index);
    pmsg = ipc_sync_send_n2(NULL, 1, 1, 
		 MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_HAL_SLOT_PLUGSTATUS_HASH, 0, index, 1000);

    if (NULL == pmsg)
    {
        ret = -1;
    }
    else
    {
        memcpy(slot_plug_status, pmsg->msg_data, sizeof(unsigned char) * pmsg->msghdr.data_len);
        ret = pmsg->msghdr.data_len;
        mem_share_free(pmsg, MODULE_ID_DEVM);
    }

    return ret;
}


int devm_get_hal_device_status(unsigned int *para_hal_status, unsigned int array_num)
{
    int ret = 0;
    int index = 0;
    unsigned int *t_hal_status = NULL;
    struct ipc_mesg_n *pmsg = NULL;
    
    index |= (1 << 8);

//    t_hal_status = ipc_send_hal_wait_reply(NULL, 0, 1 , MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_HAL_DEVICE_STATUS, IPC_OPCODE_GET, index);
    pmsg = ipc_sync_send_n2(NULL, 1, 1, 
		 MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_HAL_DEVICE_STATUS, IPC_OPCODE_GET, index, 1000);

    if (NULL == pmsg)
    {
        ret = 1;
    }
    else
    {
        memcpy(para_hal_status, pmsg->msg_data, sizeof(unsigned int) * array_num);
        ret = 0;
        mem_share_free(pmsg, MODULE_ID_DEVM);
    }

    return ret;
}

int devm_get_hal_box_info(struct devm_box *para_devm_box)
{
    int ret = 0;
    int index = 0;
//    struct devm_box *p_devm_box = NULL;
    struct ipc_mesg_n *pmsg = NULL;

    index |= (1 << 8);

//    p_devm_box = ipc_send_hal_wait_reply(NULL, 0, 1 , MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_BOX, IPC_OPCODE_GET, index);
    pmsg = ipc_sync_send_n2(NULL, 1, 1, 
		 MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_BOX, IPC_OPCODE_GET, index, 1000);

    if (NULL == pmsg)
    {
        ret = 1;
    }
    else
    {
        memcpy(para_devm_box, pmsg->msg_data, sizeof(struct devm_box));
        ret = 0;
        mem_share_free(pmsg, MODULE_ID_DEVM);
    }    

    return ret;
}


int devm_get_hal_slot_info(unsigned int slot, struct devm_slot *para_devm_slot)
{
    int ret = 0;
    int index = 0;
//    struct devm_slot *p_devm_slot = NULL;
    struct ipc_mesg_n *pmsg = NULL;

    index |= (1 << 8);
    index |= slot;

//    p_devm_slot = ipc_send_hal_wait_reply(NULL, 0, 1 , MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_SLOT, IPC_OPCODE_GET, index);
    pmsg = ipc_sync_send_n2(NULL, 1, 1, 
		 MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_SLOT, IPC_OPCODE_GET, index, 1000);

    if (NULL == pmsg)
    {
        ret = 1;
    }
    else
    {
        memcpy(para_devm_slot, pmsg->msg_data, sizeof(struct devm_slot));
        ret = 0;
        mem_share_free(pmsg, MODULE_ID_DEVM);
    }

    return ret;
}


int devm_get_hal_port_info(unsigned int slot, struct t_hsl_devm_port *para_hsl_devm_port)
{
    int ret = 0;
    int index = 0;
//  int i = 0;
//    struct ipc_mesg *t_msg = NULL;
    struct ipc_mesg_n *t_msg = NULL;

    index |= (1 << 8);
    index |= slot;

//    t_msg = ipc_send_hal_wait_reply1(NULL, 0, 1 , MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_PORT, IPC_OPCODE_GET, index);
    t_msg = ipc_sync_send_n2(NULL, 1, 1, 
		 MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_PORT, IPC_OPCODE_GET, index, 1000);

    if (NULL == t_msg)
    {
        ret = -1;
    }
    else
    {
        memcpy(para_hsl_devm_port, t_msg->msg_data, sizeof(struct t_hsl_devm_port)*t_msg->msghdr.data_num);
        ret = t_msg->msghdr.data_num;
        mem_share_free(t_msg, MODULE_ID_DEVM);
    }

    return ret;
}


#if 1
#endif


struct devm_slot *devm_get_slot(uint8_t unit, uint8_t slot)
{
    struct devm_slot *p_ret = NULL;

    if (NULL == device.punit[unit - 1])
    {
        zlog_err("%s[%d]:leave %s:unit %d not exist\n", __FILE__, __LINE__, __func__, unit);
        p_ret = NULL;
    }
    else if (0 == slot)
    {
        zlog_err("%s[%d]:leave %s:0 == slot, failed to get pslot\n", __FILE__, __LINE__, __func__);
        p_ret = NULL;
    }
    else if (NULL == device.punit[unit - 1]->pslot[slot - 1])
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:slot %d == NULL\n", __FILE__, __LINE__, __func__, slot);
        p_ret = NULL;
    }
    else if (1 == device.punit[unit - 1]->pslot[slot - 1]->plugout)
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:slot %d plugout , can not get\n", __FILE__, __LINE__, __func__, slot);
        p_ret = NULL;
    }
    else
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to get slot %d info\n ", __FILE__, __LINE__, __func__, slot);
        p_ret = device.punit[unit - 1]->pslot[slot - 1];
    }

    return p_ret;
}


int devm_get_device_id(void)
{
    int ret = 0;

    if (NULL == device.punit[device.main_unit - 1])
    {
        zlog_err("%s[%d]:leave %s:unit %d  not exist\n", __FILE__, __LINE__, __func__, device.main_unit);
        ret = -1;
    }
    else
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to get unit %d id info\n ", __FILE__, __LINE__, __func__, device.main_unit);
        ret = device.punit[device.main_unit - 1]->bplane.id;
    }

    return ret;
}


uchar *devm_get_slot_name(uint8_t unit, uint8_t slot)
{
    uchar *p_ret = NULL;

    if (NULL == device.punit[unit - 1])
    {
        zlog_err("%s[%d]:leave %s:unit[%d] not exist\n", __FILE__, __LINE__, __func__, unit - 1);
        p_ret = NULL;
    }
    else if (NULL == device.punit[unit - 1]->pslot[slot - 1])
    {
        zlog_err("%s[%d]:leave %s:unit[%d] slot[%d] == NULL\n", __FILE__, __LINE__, __func__, unit - 1, slot - 1);
        p_ret = NULL;
    }
    else if (1 == device.punit[unit - 1]->pslot[slot - 1]->plugout)
    {
        zlog_err("%s[%d]:leave %s:unit[%d] slot[%d] exist, but plugout, can not get slot info\n", __FILE__, __LINE__, __func__, unit - 1, slot - 1);
        p_ret = NULL;
    }
    else
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to get unit[%d] slot [%d] slot name info,slot_name = %s\n ", __FILE__, __LINE__, __func__, unit - 1, slot - 1, device.punit[unit - 1]->pslot[slot - 1]->name);
        p_ret = device.punit[unit - 1]->pslot[slot - 1]->name;
    }

    return p_ret;
}


uchar *devm_get_device_name(uint8_t unit, uint8_t slot)
{
    uchar *p_ret = NULL;

    if (NULL == device.punit[unit - 1])
    {
        zlog_err("%s[%d]:leave %s:unit[%d] == NULL\n", __FILE__, __LINE__, __func__, unit - 1);
        p_ret = NULL;
    }
    else if (0 == slot)
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to get unit[%d] dev name info\n ", __FILE__, __LINE__, __func__, unit - 1);
        p_ret = device.punit[unit - 1]->bplane.dev_name;
    }
    else if (NULL == device.punit[unit - 1]->pslot[slot - 1])
    {
        zlog_err("%s[%d]:leave %s:unit[%d] slot[%d] == NULL\n", __FILE__, __LINE__, __func__, unit - 1, slot - 1);
        p_ret = NULL;
    }
    else if (1 == device.punit[unit - 1]->pslot[slot - 1]->plugout)
    {
        zlog_err("%s[%d]:leave %s:unit[%d] slot[%d] exist,but plugout, can not get slot info\n", __FILE__, __LINE__, __func__, unit - 1, slot - 1);
        p_ret = NULL;
    }
    else
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to get unit[%d] slot [%d] dev name info,dev_name=%s\n ", __FILE__, __LINE__, __func__, unit - 1, slot - 1, device.punit[unit - 1]->pslot[slot - 1]->dev_name);
        p_ret = device.punit[unit - 1]->pslot[slot - 1]->dev_name;
    }

    return p_ret;
}


struct devm_fan *devm_get_fan(uint8_t unit)
{
    struct devm_fan *p_ret = NULL;

    if (NULL == device.punit[unit - 1])
    {
        zlog_err("%s[%d]:leave %s:unit[%d] == NULL\n", __FILE__, __LINE__, __func__, unit - 1);
        p_ret = NULL;
    }
    else
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to get fan info \n", __FILE__, __LINE__, __func__);
        p_ret = &device.punit[unit - 1]->fan;
    }

    return p_ret;
}


struct devm_power *devm_get_power(uint8_t unit)
{
    struct devm_power *p_ret = NULL;

    if (NULL == device.punit[unit - 1])
    {
        zlog_err("%s[%d]:leave %s:unit[%d] == NULL\n", __FILE__, __LINE__, __func__, unit - 1);
        p_ret = NULL;
    }
    else
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to get power info \n", __FILE__, __LINE__, __func__);
        p_ret = &device.punit[unit - 1]->power[0];
    }

    return p_ret;
}


struct devm_unit *devm_get_unit(uint8_t unit)
{
    struct devm_unit *p_ret = NULL;

    if (NULL == device.punit[unit - 1])
    {
        zlog_err("%s[%d]:leave %s:unit[%d] == NULL\n", __FILE__, __LINE__, __func__, unit - 1);
        p_ret = NULL;
    }
    else
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to get unit info \n", __FILE__, __LINE__, __func__);
        p_ret = device.punit[unit - 1];
    }

    return p_ret;
}


struct devm_backplane *devm_get_backplane(uint8_t unit)
{
    struct devm_backplane *p_ret = NULL;

    if (NULL == device.punit[unit - 1])
    {
        zlog_err("%s[%d]:leave %s:unit[%d] == NULL\n", __FILE__, __LINE__, __func__, unit - 1);
        p_ret = NULL;
    }
    else
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to get backplane info \n", __FILE__, __LINE__, __func__);
        p_ret = &device.punit[unit - 1]->bplane;
    }

    return p_ret;
}


uchar *devm_get_manufacture(uint8_t unit)
{
    uchar *p_ret = NULL;

    if (NULL == device.punit[unit - 1])
    {
        zlog_err("%s[%d]:leave %s:NULL == device.unit[%d]\n", __FILE__, __LINE__, __func__, unit - 1);
        p_ret = NULL;
    }
    else
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:%s:get manufacture=%s\n", __FILE__, __LINE__, __func__, device.punit[unit - 1]->bplane.manufacture);
        p_ret = device.punit[unit - 1]->bplane.manufacture;
    }

    return p_ret;
}


int devm_get_slot_status(uint8_t unit, uint8_t slot)
{
    int ret = 0;

    if (NULL == device.punit[unit - 1])
    {
        zlog_err("%s[%d]:leave %s:NULL == device.unit[%d]\n", __FILE__, __LINE__, __func__, unit - 1);
        ret = -1;
    }
    else if (NULL == device.punit[unit - 1]->pslot[slot - 1])
    {
        zlog_err("%s[%d]:leave %s:NULL == device.unit[%d]->slot[%d]\n", __FILE__, __LINE__, __func__, unit - 1, slot - 1);
        ret = -1;
    }
    else if (1 == device.punit[unit - 1]->pslot[slot - 1]->plugout)
    {
        zlog_err("%s[%d]:leave %s:device.unit[%d]->slot[%d] exist, but plugout,can not get info\n", __FILE__, __LINE__, __func__, unit - 1, slot - 1);
        ret = -1;
    }
    else
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:%s:get slot_status=%d\n", __FILE__, __LINE__, __func__, device.punit[unit - 1]->pslot[slot - 1]->status);
        ret = device.punit[unit - 1]->pslot[slot - 1]->status;
    }

    return ret;
}


uchar *devm_get_mac(uint8_t unit, uint8_t slot)
{
    uchar *p_ret = NULL;

    if (NULL != device.pbox)
    {
        p_ret = device.pbox->mac;
    }
    else if (NULL == device.punit[unit - 1])
    {
        zlog_err("%s[%d]:leave %s:unit[%d] not exist\n", __FILE__, __LINE__, __func__, unit - 1);
        p_ret = NULL;
    }
    else if (slot == 0)
    {
        p_ret = device.punit[unit - 1]->bplane.mac;
    }
    else if (NULL == device.punit[unit - 1]->pslot[slot - 1])
    {
        zlog_err("%s[%d]:leave %s:NULL == device.unit[%d]->slot[%d]\n", __FILE__, __LINE__, __func__, unit - 1, slot - 1);
        p_ret = NULL;
    }
    else if (1 == device.punit[unit - 1]->pslot[slot - 1]->plugout)
    {
        zlog_err("%s[%d]:leave %s:device.unit[%d]->slot[%d] exist, but plugout,can not get info\n", __FILE__, __LINE__, __func__, unit - 1, slot - 1);
        p_ret = NULL;
    }
    else
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:%s:get mac sucessfully from device.unit[%d]->slot[%d]\n", __FILE__, __LINE__, __func__, unit - 1, slot - 1);
        p_ret = device.punit[unit - 1]->pslot[slot - 1]->mac;
    }

    return p_ret;
}


int devm_get_timezone(int *timezone)
{
    int func_ret = 0;
    int t_timezone = 0;
    unsigned char a_timezone[256] = {0};
    ssize_t ret_read = 0;
    char *p_str = NULL;
    int fd = 0;

    if (NULL == timezone)
    {
        zlog_err("%s[%d]:leave %s:invlaid para , NULL == timezone\n", __FILE__, __LINE__, __func__);
        return 1;
    }

    fd = open(TIMEZONE_PATH, O_RDONLY);

    if (-1 == fd)
    {
        *timezone = 0;
        func_ret = 0;
    }
    else
    {
        ret_read = read(fd, (void *)a_timezone, (size_t)sizeof(a_timezone));

        if (0 != ret_read)
        {
            p_str = strstr((char *)&a_timezone[50], (char *)"GMT");

            if (NULL != p_str)
            {
                t_timezone = (p_str[4]) - '0';

                if ('+' == p_str[3])
                {
                    *timezone = t_timezone;
                    func_ret = 0;
                }
                else if ('-' == p_str[3])
                {
                    *timezone = -t_timezone;
                    func_ret = 0;
                }
                else
                {
                    *timezone = 0;
                    func_ret = 1;
                }
            }
            else
            {
                *timezone = 0;
                func_ret = 1;
            }
        }
        else
        {
            *timezone = 0;
            func_ret = 1;
        }
    }

    return func_ret;
}


#if 1
#endif


void devm_set_hash_del_slot(uint8_t slot)
{
    uint32_t index = 0;
    int ret = 0;

    index |= device.main_unit << 8;
    index |= slot ;

    ret = ipc_send_msg_n2(NULL, 1, 1, MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_SET_HAL_HASH, IPC_OPCODE_DELETE, index);

    if (ret != 0)
    {
        zlog_err("%s[%d]:leave %s:failed to send set hal hash del slot %d msg\n", __FILE__, __LINE__, __func__, slot);
    }
    else
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to send set hal hash del slot %d msg\n", __FILE__, __LINE__, __func__, slot);
    }

    return;
}


void devm_set_hash_add_slot(uint8_t slot)
{
    uint32_t index = 0;
    int ret = 0;

    index |= device.main_unit << 8;
    index |= slot ;

    ret = ipc_send_msg_n2(NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_SET_HAL_HASH, IPC_OPCODE_ADD, index);

    if (ret != 0)
    {
        zlog_err("%s[%d]:leave %s:failed to send set hal hash add slot %d msg\n", __FILE__, __LINE__, __func__, slot);
    }
    else
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to send set hal hash add slot %d msg\n", __FILE__, __LINE__, __func__, slot);
    }

    return;
}


void devm_set_slot_fpga_reconfig(unsigned int slot)
{
    uint32_t index = 0;
    int ret = 0;

    index |= device.main_unit << 8;
    index |= slot ;

    ret = ipc_send_msg_n2(NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_FPGA_RECONFIG, IPC_OPCODE_EVENT, index);

    if (ret != 0)
    {
        zlog_err("%s[%d]:leave %s:failed to send slot %d FPGA reconfig\n", __FILE__, __LINE__, __func__, slot);
    }
    else
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to send slot %d FPGA reconfig\n", __FILE__, __LINE__, __func__, slot);
    }

    return;
}


void devm_reset_master(enum DEV_TYPE type, uint8_t unit)
{
    uint32_t index = 0;
    int ret = 0;

    index |= type << 16;
    index |= unit << 8;

    ret = ipc_send_msg_n2(NULL, 1, 1, MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_RESET_MASTER, IPC_OPCODE_EVENT, index);

    if (ret != 0)
    {
        zlog_err("%s[%d]:leave %s:failed to send master reset cmd to hal\n", __FILE__, __LINE__, __func__);
    }
    else
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to master reset cmd to hal\n", __FILE__, __LINE__, __func__);
    }

    return ;
}


void devm_reset_device(enum DEV_TYPE type, uint8_t unit, uint8_t slot)
{
    int ret  = 0;
    uint32_t index = 0;

    index |= type << 16;
    index |= unit << 8;
    index |= slot;

    ret = ipc_send_msg_n2(NULL, 1, 1, MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_RESET, IPC_OPCODE_EVENT, index);

    if (ret != 0)
    {
        zlog_err("%s[%d]:leave %s:failed to send reset msg to hal, type = %d,unit = %d ,slot = %d\n", __FILE__, __LINE__, __func__, type, unit , slot);
    }
    else
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:send reset msg to hal, type = %d,unit = %d ,slot = %d \n", __FILE__, __LINE__, __func__, type, unit , slot);
    }

    return ;
}


void devm_setmx_m2s(enum DEV_TYPE type, uint8_t unit)
{
    int ret = 0;
    uint32_t index = 0;

    index |= type << 16;
    index |= unit << 8;

    ret = ipc_send_msg_n2(NULL, 1, 1, MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_SETMX, IPC_OPCODE_EVENT, index);

    if (ret != 0)
    {
        zlog_err("%s[%d]:leave %s:failed to send setmx m2s msg to hal, type = %d,unit = %d \n", __FILE__, __LINE__, __func__, type, unit);
    }
    else
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:send setmx m2s msg to hal, type = %d,unit = %d \n", __FILE__, __LINE__, __func__, type, unit);
    }

    return ;
}


void devm_reboot_device(enum DEV_TYPE type, uint8_t unit, uint8_t slot)
{
    int ret = 0;
    uint32_t index = 0;

    index |= type << 16;
    index |= unit << 8;
    index |= slot;

    ipc_send_msg_n2(NULL, 1, 1, MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_REBOOT, IPC_OPCODE_EVENT, index);

    if (ret != 0)
    {
        zlog_err("%s[%d]:leave %s:failed to send reboot msg to hal, type = %d,unit = %d ,slot = %d \n", __FILE__, __LINE__, __func__, type, unit , slot);
    }
    else
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:send reboot msg to hal, type = %d,unit = %d ,slot = %d\n", __FILE__, __LINE__, __func__, type, unit , slot);
    }

    return ;
}


/* 将端口消息发送给接口管理创建物理端口 */
void devm_send_ifm_add_port(struct ifm_common *ifcom, struct devm_port *p_devm_port)
{
    int ret = 0;

    if (NULL == ifcom || NULL == p_devm_port)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para ifcom or p_devm_port\n", __FILE__, __LINE__, __func__);
        return ;
    }

    ifcom->usp.type = port_type_set(p_devm_port->port_type);
    ifcom->usp.port = p_devm_port->port;
    ifcom->usp.subtype = p_devm_port->port_type;
    ifcom->port_info.fiber = p_devm_port->fiber;
    ifcom->port_info.hide = p_devm_port->hide;

    memcpy(ifcom->mac, p_devm_port->mac, sizeof(ifcom->mac));

    ret = ipc_send_msg_n2(ifcom, sizeof(struct ifm_common), 1, MODULE_ID_IFM, MODULE_ID_DEVM,
                          IPC_TYPE_DEVM, DEV_EVENT_PORT_ADD, IPC_OPCODE_EVENT, 0);

    if (-1 == ret)
    {
        zlog_err("%s[%d]:leave %s:error:fail to send portinfo to ifm!,portnum = %d,mac:%x %x %x %x %x %x\n", __FILE__, __LINE__, __func__, ifcom->usp.port, ifcom->mac[0], ifcom->mac[1], ifcom->mac[2], ifcom->mac[3], ifcom->mac[4], ifcom->mac[5]);
    }
    else
    {
        zlog_notice("%s[%d]:leave %s:sucessfully to send portinfo to ifm,unit=%d,slot=%d,port_num = %d,type=%d,subtype=%d,mac:%x %x %x %x %x %x\n", __FILE__, __LINE__, __func__, ifcom->usp.unit, ifcom->usp.slot, ifcom->usp.port, ifcom->usp.type, ifcom->usp.subtype, ifcom->mac[0], ifcom->mac[1], ifcom->mac[2], ifcom->mac[3], ifcom->mac[4], ifcom->mac[5]);
    }

    return ;
}


void devm_send_ifm_del_port(struct ifm_common *ifcom, struct devm_port *t_port)
{
    int ret = 0;

    if (NULL == ifcom || NULL == t_port)
    {
        zlog_err("%s[%d]:leave %s:error:invalid para ifcom or t_port\n", __FILE__, __LINE__, __func__);
        return ;
    }

    ifcom->usp.type = port_type_set(t_port->port_type);
    ifcom->usp.port = t_port->port;
    ifcom->usp.subtype = t_port->port_type;
    ifcom->port_info.fiber = t_port->fiber;
    ifcom->port_info.hide = t_port->hide;
    memcpy(ifcom->mac, t_port->mac, sizeof(ifcom->mac));

    ret = ipc_send_msg_n2(ifcom, sizeof(struct ifm_common), 1, MODULE_ID_IFM, MODULE_ID_DEVM,
                          IPC_TYPE_DEVM, DEV_EVENT_PORT_DELETE, IPC_OPCODE_EVENT, 0);

    if (-1 == ret)
    {
        zlog_err("%s[%d]:leave %s:error:fail to send portinfo to ifm!,unit = %d,slot = %d,portnum = %d,mac:%x %x %x %x %x %x\n", __FILE__, __LINE__, __func__, ifcom->usp.unit, ifcom->usp.slot, ifcom->usp.port, ifcom->mac[0], ifcom->mac[1], ifcom->mac[2], ifcom->mac[3], ifcom->mac[4], ifcom->mac[5]);
    }
    else
    {
        zlog_notice("%s[%d]:leave %s:sucessfully to send portinfo to ifm,unit = %d,slot = %d,port = %d,mac:%x %x %x %x %x %x\n", __FILE__, __LINE__, __func__, ifcom->usp.unit, ifcom->usp.slot, ifcom->usp.port, ifcom->mac[0], ifcom->mac[1], ifcom->mac[2], ifcom->mac[3], ifcom->mac[4], ifcom->mac[5]);
    }

    return ;
}


void devm_set_watchdog(uint8_t wdi, uint8_t unit, uint8_t slot)
{
    int ret = 0;
    uint32_t index = 0;

    index |= unit << 8;
    index |= slot;

    ipc_send_msg_n2(&wdi, sizeof(uint8_t), 1, MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_WATCHDOG, IPC_OPCODE_UPDATE, index);

    if (-1 == ret)
    {
        zlog_err("%s[%d]:leave %s:error:fail to send set heart beat msg to hal ,unit = %d, slot = %d,wdi = %d\n", __FILE__, __LINE__, __func__, unit, slot, wdi);
    }
    else
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to send set heart beat msg to hal ,unit = %d, slot = %d,wdi = %d\n", __FILE__, __LINE__, __func__, unit, slot, wdi);
    }

    return;
}


void devm_set_heart_beat(uint8_t heart, uint8_t unit, uint8_t slot)
{
    int ret = 0;
    uint32_t index = 0;

    index |= unit << 8;
    index |= slot;

    ret = ipc_send_msg_n2(&heart, sizeof(uint8_t), 1, MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_HEART_BEAT, IPC_OPCODE_UPDATE, index);

    if (-1 == ret)
    {
        zlog_err("%s[%d]:leave %s:error:fail to send set heart beat msg to hal,unit = %d, slot = %d,heart = %d\n", __FILE__, __LINE__, __func__, unit, slot, heart);
    }
    else
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to send set heart beat msg to hal,unit = %d, slot = %d,heart = %d\n", __FILE__, __LINE__, __func__, unit, slot, heart);
    }

    return;
}


int devm_send_hal_set_dynamic_voltage(unsigned char powerslot, unsigned char voltage_cmd)
{
    int ret = 0;
    int ret_func = 0;
    uint32_t index = 0;
    unsigned char t_voltage_cmd = voltage_cmd;
    struct devm_power temp_devm_power;

    index |= 1 << 8;
    index |= (1 == powerslot)? gPwrSlot1: gPwrSlot2;

    ret = devm_sendto_hal_and_wait_ack(&t_voltage_cmd , sizeof(t_voltage_cmd), 1, MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_SET_DYNAMIC_VOLTAGE, IPC_OPCODE_EVENT, index);

    if (-1 == ret)
    {
        zlog_err("%s[%d]:leave %s:failed to send set power %d %s dynamic voltage msg to hal\n", __FILE__, __LINE__, __func__, powerslot, (0 == t_voltage_cmd) ? "enable" : "disable");
        ret_func = 1;
    }
    else
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to send set power %d  %s dynamic voltage msg to hal\n", __FILE__, __LINE__, __func__, powerslot, (0 == t_voltage_cmd) ? "enable" : "disable");

        if (1 == voltage_cmd)
        {
            int power_mode = 0;

            memcpy(&temp_devm_power, &device.punit[device.main_unit - 1]->power[0], sizeof(struct devm_power));
//            temp_devm_power.voltage_input = 0;
            power_mode = devm_get_power_alarm_type_by_mode(temp_devm_power.mode);

            if (0 != power_mode && (-1) != power_mode)
            {
                devm_check_power_alarm(device.main_unit, gPwrSlot1, &device.punit[device.main_unit - 1]->power[0], &temp_devm_power, power_mode);
            }

            memcpy(&device.punit[device.main_unit - 1]->power[0], &temp_devm_power, sizeof(struct devm_power));

            memcpy(&temp_devm_power, &device.punit[device.main_unit - 1]->power[1], sizeof(struct devm_power));
//            temp_devm_power.voltage_input = 0;
            power_mode = devm_get_power_alarm_type_by_mode(temp_devm_power.mode);

            if (0 != power_mode && -1 != power_mode)
            {
                devm_check_power_alarm(device.main_unit, gPwrSlot2, &device.punit[device.main_unit - 1]->power[1], &temp_devm_power, power_mode);
            }

            memcpy(&device.punit[device.main_unit - 1]->power[1], &temp_devm_power, sizeof(struct devm_power));
        }

        ret_func = 0;
    }

    return ret_func;
}


int devm_update_voltage_limit(int unit, int powerslot, struct devm_power *p_power)
{
    int ret = 0;
    int ret_func = 0;
    uint32_t index = 0;
#if 0
    devm_check_power_alarm(device.main_unit, powerslot, &device.punit[device.main_unit - 1]->power[powerslot - 1]);
#endif
    index |= unit << 8;
    index |= powerslot;

    ret = devm_sendto_hal_and_wait_ack(p_power, sizeof(struct devm_power), 1, MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_POWER, IPC_OPCODE_UPDATE, index);

    if (-1 == ret)
    {
        zlog_err("%s[%d]:leave %s:error:fail to send set update voltage limit msg to hal,voltage limit(%d,%d)\n", __FILE__, __LINE__, __func__, p_power->voltage_low, p_power->voltage_high);
        ret_func = 1;
    }
    else
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to send set update voltage limit msg to hal,voltage limit(%d,%d)\n", __FILE__, __LINE__, __func__, p_power->voltage_low, p_power->voltage_high);
        ret_func = 0;
    }

    return ret_func;
}


int devm_update_temperature_limit(struct devm_temp *p_temp)
{
    int ret = 0;
    int ret_func = 0;
    uint32_t index = 0;

    index |= 1 << 8;
    index |= 1;

    ret = devm_sendto_hal_and_wait_ack(p_temp, sizeof(struct devm_temp), 1, MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_TEMP, IPC_OPCODE_UPDATE, index);

    if (-1 == ret)
    {
        zlog_err("%s[%d]:leave %s:error:fail to send set update temperature limit msg to hal,temperature limit(%d,%d)\n", __FILE__, __LINE__, __func__, p_temp->temp_low, p_temp->temp_high);
        ret_func = 1;
    }
    else
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to send set update temperature limit msg to hal,temperature limit(%d,%d)\n", __FILE__, __LINE__, __func__, p_temp->temp_low, p_temp->temp_high);
        ret_func = 0;
    }

    return ret_func;
}


int devm_send_set_slot_power_up_down(int slot, int slot_cmd)
{
    int ret = 0;
    int ret_func = 0;
    uint32_t index = 0;
    int temp_slot_cmd = slot_cmd;

    index |= 1 << 8;
    index |= slot;

    ret = devm_sendto_hal_and_wait_ack(&temp_slot_cmd, sizeof(temp_slot_cmd), 1, MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_CTRL_SLOT_POWER, IPC_OPCODE_UPDATE, index);

    if (-1 == ret)
    {
        zlog_err("%s[%d]:leave %s:error:fail to send set slot %d power %s\n", __FILE__, __LINE__, __func__, slot, (1 == slot_cmd) ? "down" : "up");
        ret_func = 1;
    }
    else
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to send set slot %d power %s\n", __FILE__, __LINE__, __func__, slot, (1 == slot_cmd) ? "down" : "up");
        ret_func = 0;
    }

    return ret_func;
}

int devm_get_op_eeprom_type(char *info_type)
{
    enum EEP_INFO_TYPE op_info_type = EEP_INFO_NONE;

    if (!memcmp((char *)info_type, "device", strlen("device")))
    {
        op_info_type = EEP_INFO_DEVNAME;
    }
    else if (!memcmp(info_type, "oem", strlen("oem")))
    {
        op_info_type = EEP_INFO_OEM_NAME;
    }
    else if (!memcmp(info_type, "hardware_version", strlen("hardware_version")))
    {
        op_info_type = EEP_INFO_HW_VER;
    }
    else if (!memcmp(info_type, "vendor", strlen("vendor")))
    {
        op_info_type = EEP_INFO_VENDOR;
    }
    else if (!memcmp(info_type, "slot_name", strlen("slot_name")))
    {
        op_info_type = EEP_INFO_SLOT_NAME;
    }
    else if (!memcmp(info_type, "1st_mac", strlen("1st_mac")))
    {
        op_info_type = EEP_INFO_MAC;
    }
    else if (!memcmp(info_type, "hwcfg", strlen("hwcfg")))
    {
        op_info_type = EEP_INFO_ID;
    }
    else if (!memcmp(info_type, "macnum", strlen("macnum")))
    {
        op_info_type = EEP_INFO_MAC_NUM;
    }
    else if (!memcmp(info_type, "sn", strlen("sn")))
    {
        op_info_type = EEP_INFO_SN;
    }
    else if (!memcmp(info_type, "cfginit", strlen("cfginit")))
    {
        op_info_type = EEP_INFO_CFGINIT;
    }    		
    else if (!memcmp(info_type, "all", strlen("all")))
    {
        op_info_type = EEP_INFO_ALL;
    }

    return op_info_type;
}


int devm_get_op_eeprom_type_digital(char *info_type)
{
    enum EEP_INFO_TYPE op_info_type = EEP_INFO_NONE;

    if (!memcmp((char *)info_type, "dev", strlen("dev")))
    {
        op_info_type = EEP_INFO_DEVNAME;
    }
    else if (!memcmp(info_type, "oem", strlen("oem")))
    {
        op_info_type = EEP_INFO_OEM_NAME;
    }
    else if (!memcmp(info_type, "hver", strlen("hver")))
    {
        op_info_type = EEP_INFO_HW_VER;
    }
    else if (!memcmp(info_type, "vnd", strlen("vnd")))
    {
        op_info_type = EEP_INFO_VENDOR;
    }
    else if (!memcmp(info_type, "bom", strlen("bom")))
    {
        op_info_type = EEP_INFO_BOM;
    }
    else if (!memcmp(info_type, "mac", strlen("mac")))
    {
        op_info_type = EEP_INFO_MAC;
    }
    else if (!memcmp(info_type, "hwcfg", strlen("hwcfg")))
    {
        op_info_type = EEP_INFO_ID;
    }
    else if (!memcmp(info_type, "dver", strlen("dver")))
    {
        op_info_type = EEP_INFO_DVER;
    }
    else if (!memcmp(info_type, "sn", strlen("sn")))
    {
        op_info_type = EEP_INFO_SN;
    }
    else if (!memcmp(info_type, "lic", strlen("lic")))
    {
        op_info_type = EEP_INFO_LIC;
    } 
    else if (!memcmp(info_type, "date", strlen("date")))
    {
        op_info_type = EEP_INFO_DATE;
    }
    else if (!memcmp(info_type, "cfginit", strlen("cfginit")))
    {
        op_info_type = EEP_INFO_CFGINIT;
    } 	
    else if (!memcmp(info_type, "all", strlen("all")))
    {
        op_info_type = EEP_INFO_ALL;
    }

    return op_info_type;
}


/* 1 this kind of eeprom type is not needed int the eeprom
  0 ok*/
int devm_op_eeprom_check_type(int cmd_type, unsigned char *str_info_type, EEP_KEY *g_peep_info)
{
    int i = 0;
    int ret = 1;

    if (!memcmp((char *)str_info_type, "all", strlen("all")))
    {
        if (EEP_INFO_CMD_GET == cmd_type || EEP_INFO_CMD_ERASE == cmd_type)
        {
            ret = 0;
        }
        else
        {
            ret = 1;
        }
    }
    else
    {
        for (i = 0; i < g_peep_info->num; i++)
        {
            if (!memcmp((char *)g_peep_info->unit[i].key_word, str_info_type, strlen((char *)str_info_type)))
            {
                ret = 0;
                break;
            }
        }
    }

    return ret;
}


int devm_op_eeprom_check_data_form(int slot, int info_type, unsigned char *info_data, int data_len)
{
    int func_ret = 0;
    unsigned int mac[6] = {0};

    switch (info_type)
    {
        case EEP_INFO_MAC:
            if (strlen((char *)info_data) > strlen("00:11:22:33:44:55"))
            {
                func_ret = 1;
            }
            else if (sscanf((char *)info_data, "%2x:%2x:%2x:%2x:%2x:%2x", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]) == 6)
            {
                func_ret = 0;
            }
            else
            {
                func_ret = 1;
            }

            break;

        default:
            func_ret = 0;
            break;
    }

    return func_ret;
}


int devm_op_eeprom_info_func(int cmd_type, int slot, int info_type, unsigned char *info_data, int data_len)
{
    uint32_t index = 0;
    unsigned char *p_eep_info = NULL;
    struct ipc_mesg_n *pmsg = NULL;

    index |= cmd_type << 24;
    index |= info_type << 16;
    index |= 1 << 8;
    index |= slot;

//    p_eep_info = ipc_send_hal_wait_reply(info_data, data_len, 1, MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_EEPINFO_OP, IPC_OPCODE_UPDATE, index);
    pmsg = ipc_sync_send_n2(info_data, data_len, 1, 
		 MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_EEPINFO_OP, IPC_OPCODE_UPDATE, index, 1000);

    if (NULL == pmsg)
    {
        zlog_err("%s[%d]:leave %s:error:fail to %s slot %d eeprom %s info %s \n", __FILE__, __LINE__, __func__, a_eep_op_cmd[cmd_type], slot, a_eepinto_type[info_type], info_data);
        mem_share_free(pmsg, MODULE_ID_DEVM);

        return 1;
    }
    else
    {
        zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to %s slot %d eeprom %s info %s\n", __FILE__, __LINE__, __func__, a_eep_op_cmd[cmd_type], slot, a_eepinto_type[info_type], info_data);
        memcpy(info_data, pmsg->msg_data, data_len);
        mem_share_free(pmsg, MODULE_ID_DEVM);
    }

    return 0;
}


/*定时器函数*/
int devm_sig_func(struct thread *para_thread)
{
    int i = 0;

    for (i = 0; i < MAX_TIMER_CNT; i++)
    {
        if (plugout_timer_t[i].state == 0)
        {
            continue;
        }

        plugout_timer_t[i].elapse++;

        if (plugout_timer_t[i].elapse == plugout_timer_t[i].interval)
        {
            plugout_timer_t[i].timer_proc(plugout_timer_t[i].func_arg, plugout_timer_t[i].arg_len);
        }
    }

//    thread_add_timer(devm_master, devm_sig_func, NULL, 1);

    return 0;
}


/* success, return 0; failed, return -1 */
int devm_init_mul_timer(void)
{
    memset(plugout_timer_t, 0, sizeof(struct plugout_timer_info) * MAX_TIMER_CNT);
    zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to init all mul timer\n", __FILE__, __LINE__, __func__);

    return ERRNO_SUCCESS;
}


/* success, return 0; failed, return -1 */
int devm_destroy_mul_timer(void)
{
    zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to destory all slot plug timer\n", __FILE__, __LINE__, __func__);
    memset(plugout_timer_t, 0, sizeof(struct plugout_timer_info) * MAX_TIMER_CNT);

    return ERRNO_SUCCESS;
}


/* success, return timer handle(>=0); failed, return -1 */
int devm_set_a_timer(uint32_t interval, int (* timer_proc)(void *arg, int arg_len), void *arg, uint32_t arg_len, uint8_t slot)

{
    if (NULL == timer_proc
            || slot <= 0 || slot > MAX_TIMER_CNT
            || plugout_timer_t[slot - 1].state == 1
            || 0 == interval)
    {
        return ERRNO_FAIL;
    }

    if (NULL == device.punit[device.main_unit - 1]->pslot[slot - 1])
    {
        return ERRNO_FAIL;
    }

    memset(&plugout_timer_t[slot - 1], 0, sizeof(plugout_timer_t[slot - 1]));
    plugout_timer_t[slot - 1].timer_proc = devm_plug_timer_func;

    if (arg != NULL)
    {
        if (arg_len > MAX_FUNC_ARG_LEN || 0 == arg_len)
        {
            return ERRNO_FAIL;
        }

        memcpy(&plugout_timer_t[slot - 1].func_arg, arg, arg_len);
        plugout_timer_t[slot - 1].arg_len = arg_len;
    }

    plugout_timer_t[slot - 1].interval = interval;
    plugout_timer_t[slot - 1].elapse = 0;
    plugout_timer_t[slot - 1].state = 1;
    zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to set  slot %d plug timer\n", __FILE__, __LINE__, __func__, slot);

    return ERRNO_SUCCESS;
}


/* success, return 0; failed, return -1 */
int devm_del_a_timer(uint8_t slot)
{
    if (slot == 0 || slot > MAX_TIMER_CNT)
    {
        return ERRNO_FAIL;
    }

    memset(&plugout_timer_t[slot - 1], 0, sizeof(plugout_timer_t[slot - 1]));
    zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to del  slot %d plug timer\n", __FILE__, __LINE__, __func__, slot);

    return ERRNO_SUCCESS;
}


int devm_plug_timer_func(void *arg, int arg_len)
{
    int ret = 0;
    int ret_func = 0;
    unsigned int index = 0;
    uint8_t unit = 0;
    uint8_t slot = 0;


    if (sizeof(unsigned int) == arg_len)
    {
        index = *(unsigned int *)arg;

        slot = index & 0xff;
        unit = (index & 0xff00) >> 8;

        devm_delete_slot(unit, slot);

        ret = devm_sendto_hal_and_wait_ack(NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_DEVM, IPC_TYPE_DEVM, DEVM_INFO_SLOT, IPC_OPCODE_UPDATE, index);

        if (-1 == ret)
        {
            zlog_err("%s[%d]:leave %s:error:fail to send msg to hal to plugout slot (%d,%d)\n", __FILE__, __LINE__, __func__, unit, slot);
            ret_func = ERRNO_FAIL;
        }
        else
        {
            zlog_debug(DEVM_DBG_COMM, "%s[%d]:%s:sucessfully to send msg to hal to plugout slot (%d,%d)\n", __FILE__, __LINE__, __func__, unit, slot);
            ret_func = ERRNO_SUCCESS;
        }
    }
    else
    {
        ret_func = ERRNO_FAIL;
    }

    return ret_func;
}


int devm_get_sysObjectid(void)
{
    unsigned int t_dev_id = 0;
    FILE *oem_fd;
    int  ret_func = 0;
//  unsigned int dev_id = 0;
    char orig_buf[256] = "\0";
    char a_devid[256] = "\0";
    char a_sysObjectid[256] = "\0";
    char *find = NULL;
    char *p_orig = NULL;

    oem_fd = fopen("/home/bin/oem.xml", "r");

    if (oem_fd != NULL)
    {
        while (1)
        {
            p_orig = fgets(orig_buf, sizeof(orig_buf), oem_fd);

            if (p_orig == NULL)
            {
                fclose(oem_fd);
                ret_func = 1;
                break;
            }

            find = strstr((char *)orig_buf, "dev_id");

            if (NULL != find)
            {
                sscanf(find,  "%*[^=]=\"%[^\"]", a_devid);

                /*提取设备id并对比当前设备id*/
                t_dev_id = atoi(a_devid);

                if (t_dev_id == device.pmyunit->bplane.id)
                {
                    find = strstr((char *)orig_buf, "sysObjectID");
                    sscanf(find,  "%*[^=]=\"%[^\"]", a_sysObjectid);
                    memcpy(sys_objectid, a_sysObjectid, strlen(a_sysObjectid));
                    zlog_debug(DEVM_DBG_COMM, "%s[%d]:leave %s:sucessfully to get sysObjectid : %s\n", __FILE__, __LINE__, __func__, sys_objectid);
                    ret_func = 0;
                    break;
                }
            }
        }
    }
    else
    {
        zlog_err("%s[%d]:leave %s:error:fail to open oem.xml, file not exist\n", __FILE__, __LINE__, __func__);
        ret_func = 1;
    }

    return ret_func;
}


/* 初始化 device 数据结构 */
void devm_init_dev(void)
{
    memset(&device, 0, sizeof(struct devm));
    device.main_unit = 1;   /* 目前只支持一个框，主框固定为 1 */
    device.myunit = 0;
    device.myslot = 0;
    device.unit_num = 0;
    device.pbox = NULL;
    device.pmyunit = NULL;

    return;
}


void devm_init_default_event_module(void)
{
    devm_event_register(DEV_EVENT_HA_BOOTSTATE_CHANGE, MODULE_ID_VTY, MODULE_ID_VTY);
    devm_event_register(DEV_EVENT_HA_BOOTSTATE_CHANGE, MODULE_ID_HAL, MODULE_ID_HAL);

    return;
}

