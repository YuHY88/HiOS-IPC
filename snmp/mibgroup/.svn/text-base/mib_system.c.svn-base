/*
 *  cli command MIB group implementation - ifm_snmp.c
 *
 */
/* Portions of this file are subject to the following copyright(s).  See
 * the Net-SNMP's COPYING file for more details and other copyrights
 * that may apply:
 */
/*
 * Portions of this file are copyrighted by:
 * Copyright ?2003 Sun Microsystems, Inc. All rights reserved.
 * Use is subject to license terms specified in the COPYING file
 * distributed with the Net-SNMP package.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <lib/types.h>
#include <sys/sysinfo.h>

#include "net-snmp-config.h"
#include "types.h"
#include "net-snmp-includes.h"
#include "net-snmp-agent-includes.h"

#include "snmp_index_operater.h"
#include "ipran_snmp_data_cache.h"

#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/pkt_buffer.h>
#include <lib/msg_ipc_n.h>
#include <lib/linklist.h>
#include <lib/devm_com.h>
#include <devm/devm.h>

#include "mib_system.h"
#include "../../system/system.h"
#include "../../system/system_usage.h"
#include <lib/log.h>
//#include "lib/snmp_common.h"
#include "snmp_config_table.h"

/*local temp variable*/
static uchar str_value[STRING_LEN] = {'\0'};
static int   int_value_self = 0;

static int data_loaded = 0 ;


static struct ipran_snmp_data_cache *hhrEEpromInfoTable_cache = NULL ;


/*
 * Object ID definitions
 */
/*hhrSystemConf*/
static oid hhrSystemConf_oid[] = {HHRSYSTEMOID, 14};
FindVarMethod hhrSystemConf_get;
struct variable1 hhrSystemConf_variables[] =
{
    {hhrDeviceID, ASN_INTEGER, RONLY, hhrSystemConf_get, 1, {5}}
};

static oid devicefanInfo_oid[] = {HHRSYSTEMOID, 14, 1};
FindVarMethod devicefanInfo_get;
struct variable1 devicefanInfo_variables[] =
{
    {hhrFanSpeedMin, ASN_INTEGER, RONLY, devicefanInfo_get, 1, {1}},
    {hhrFanSpeed, ASN_INTEGER, RONLY, devicefanInfo_get, 1, {2}},
    {hhrFanSpeedMax, ASN_INTEGER, RONLY, devicefanInfo_get, 1, {3}}
};

static oid hhrDiviceBoardTable_oid[] = {HHRSYSTEMOID, 14, 2};
FindVarMethod hhrDiviceBoardTable_get;
struct variable2 hhrDiviceBoardTable_variables[] =
{
    {hhrDeviceBoardType,        ASN_INTEGER,   RONLY, hhrDiviceBoardTable_get, 2, {1, 2}},
    {hhrDeviceBoardStatus,      ASN_INTEGER,   RONLY, hhrDiviceBoardTable_get, 2, {1, 3}},
    {hhrDeviceBoardRunningTime, ASN_TIMETICKS, RONLY, hhrDiviceBoardTable_get, 2, {1, 4}},
    {hhrDeviceBoardName,        ASN_OCTET_STR, RONLY, hhrDiviceBoardTable_get, 2, {1, 5}},
    {hhrDeviceBoardID,          ASN_UNSIGNED,  RONLY, hhrDiviceBoardTable_get, 2, {1, 6}}
};

static oid hhrDevicePowerTable_oid[] = {HHRSYSTEMOID, 14, 3};
FindVarMethod hhrDevicePowerTable_get;
struct variable2 hhrDevicePowerTable_variables[] =
{
    {hhrDevicePowerType, 			ASN_INTEGER, RONLY, hhrDevicePowerTable_get, 2, {1, 2}},
    {hhrDevicePowerInVoltageLow, 	ASN_INTEGER, RONLY, hhrDevicePowerTable_get, 2, {1, 3}},
    {hhrDevicePowerInVoltage, 		ASN_INTEGER, RONLY, hhrDevicePowerTable_get, 2, {1, 4}},
    {hhrDevicePowerInVoltageHigh, 	ASN_INTEGER, RONLY, hhrDevicePowerTable_get, 2, {1, 5}},
	{hhrDevicePowerOutVoltage, 		ASN_INTEGER, RONLY, hhrDevicePowerTable_get, 2, {1, 6}}
};


static oid devicetempInfo_oid[] = {HHRSYSTEMOID, 14, 4};
FindVarMethod devicetempInfo_get;
struct variable1 devicetempInfo_variables[] =
{
    {hhrTemperatureMin      , ASN_INTEGER, RONLY, devicetempInfo_get, 1, {1}},
	{hhrTemperature      	, ASN_INTEGER, RONLY, devicetempInfo_get, 1, {2}},
	{hhrTemperatureMax      , ASN_INTEGER, RONLY, devicetempInfo_get, 1, {3}}
};



static oid systemThreshold_oid[] = {HHRSYSTEMOID, 17};
FindVarMethod systemThreshold_get;
struct variable1 systemThreshold_variables[] =
{
    {hhrCpuThreshold        , ASN_INTEGER, RONLY, systemThreshold_get, 1, {3}},
    {hhrMemoryThreshold     , ASN_INTEGER, RONLY, systemThreshold_get, 1, {4}},
};


static oid memoryInfo_oid[] = {HHRSYSTEMOID, 17, 1};
FindVarMethod memoryInfo_get;
struct variable1 memoryInfo_variables[] =
{
    {hhrMemoryTotal         , ASN_INTEGER, RONLY, memoryInfo_get, 1, {1}},
    {hhrMemoryUsed          , ASN_INTEGER, RONLY, memoryInfo_get, 1, {2}},
    {hhrMemoryFree          , ASN_INTEGER, RONLY, memoryInfo_get, 1, {3}},
    {hhrMemoryShared        , ASN_INTEGER, RONLY, memoryInfo_get, 1, {4}},
    {hhrMemoryBuffersUsed   , ASN_INTEGER, RONLY, memoryInfo_get, 1, {5}},
    {hhrMemoryBuffersFree   , ASN_INTEGER, RONLY, memoryInfo_get, 1, {6}},
    {hhrMemorySwapTotal     , ASN_INTEGER, RONLY, memoryInfo_get, 1, {7}},
    {hhrMemorySwapUsed      , ASN_INTEGER, RONLY, memoryInfo_get, 1, {8}},
    {hhrMemorySwapFree      , ASN_INTEGER, RONLY, memoryInfo_get, 1, {9}}
};

static oid hhrCpuInfoTable_oid[] = {HHRSYSTEMOID, 17, 2};
FindVarMethod hhrCpuInfoTable_get;
struct variable2 hhrCpuInfoTable_variables[] =
{
    {hhrCPUUsr      , ASN_INTEGER,  RONLY, hhrCpuInfoTable_get, 2, {1, 2}},
    {hhrCPUNice     , ASN_INTEGER,  RONLY, hhrCpuInfoTable_get, 2, {1, 3}},
    {hhrCPUSys      , ASN_INTEGER,  RONLY, hhrCpuInfoTable_get, 2, {1, 4}} ,
    {hhrCPUIdle     , ASN_INTEGER,  RONLY, hhrCpuInfoTable_get, 2, {1, 5}},
    {hhrCPUIO       , ASN_INTEGER,  RONLY, hhrCpuInfoTable_get, 2, {1, 6}},
    {hhrCPUIrq      , ASN_INTEGER,  RONLY, hhrCpuInfoTable_get, 2, {1, 7}} ,
    {hhrCPUSirq     , ASN_INTEGER,  RONLY, hhrCpuInfoTable_get, 2, {1, 8}}
};


static oid hhrEEpromInfoTable_oid[] = {HHRSYSTEMOID, 18, 10};
FindVarMethod hhrEEpromInfoTable_get;
struct variable2 hhrEEpromInfoTable_variables[] =
{
    {hhrEEpromHwcfg	, ASN_OCTET_STR,  RONLY, hhrEEpromInfoTable_get, 2, {1, 2}},
    {hhrEEpromDev   , ASN_OCTET_STR,  RONLY, hhrEEpromInfoTable_get, 2, {1, 3}},
    {hhrEEpromOem   , ASN_OCTET_STR,  RONLY, hhrEEpromInfoTable_get, 2, {1, 4}} ,
    {hhrEEpromMac   , ASN_OCTET_STR,  RONLY, hhrEEpromInfoTable_get, 2, {1, 5}},
    {hhrEEpromDver  , ASN_OCTET_STR,  RONLY, hhrEEpromInfoTable_get, 2, {1, 6}},
    {hhrEEpromHver  , ASN_OCTET_STR,  RONLY, hhrEEpromInfoTable_get, 2, {1, 7}} ,
    {hhrEEpromVnd   , ASN_OCTET_STR,  RONLY, hhrEEpromInfoTable_get, 2, {1, 8}},
	{hhrEEpromSn    , ASN_OCTET_STR,  RONLY, hhrEEpromInfoTable_get, 2, {1, 9}},
	{hhrEEpromDate  , ASN_UNSIGNED,   RONLY, hhrEEpromInfoTable_get, 2, {1, 10}},
	{hhrEEpromBom   , ASN_OCTET_STR,  RONLY, hhrEEpromInfoTable_get, 2, {1, 11}},
	{hhrEEpromLic   , ASN_OCTET_STR,  RONLY, hhrEEpromInfoTable_get, 2, {1, 12}}
};


void init_mib_system(void)
{
    REGISTER_MIB("hhrSystemConf", hhrSystemConf_variables,
                 variable1, hhrSystemConf_oid);

    REGISTER_MIB("devicefanInfo", devicefanInfo_variables,
                 variable1, devicefanInfo_oid);
    REGISTER_MIB("hhrDiviceBoardTable", hhrDiviceBoardTable_variables,
                 variable2, hhrDiviceBoardTable_oid);
    REGISTER_MIB("hhrDevicePowerTable", hhrDevicePowerTable_variables,
                 variable2, hhrDevicePowerTable_oid);
	REGISTER_MIB("devicetempInfo", devicetempInfo_variables,
                 variable1, devicetempInfo_oid);

    REGISTER_MIB("systemThreshold", systemThreshold_variables,
                 variable1, systemThreshold_oid);
    REGISTER_MIB("memoryInfo", memoryInfo_variables,
                 variable1, memoryInfo_oid);
    REGISTER_MIB("hhrCpuInfoTable", hhrCpuInfoTable_variables,
                 variable2, hhrCpuInfoTable_oid);

	REGISTER_MIB("hhrEEpromInfoTable", hhrEEpromInfoTable_variables,
                 variable2, hhrEEpromInfoTable_oid);
}

u_char *hhrSystemConf_get(struct variable *vp,
                          oid *name,
                          size_t *length,
                          int exact, size_t *var_len, WriteMethod **write_method)
{
    int ret = 0;
    uint32_t dev_id;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrDeviceID:
            ret = devm_comm_get_id(1, 0, MODULE_ID_SNMPD, &dev_id);

            if (0 != ret)
            {
                int_value_self = 0;
            }
            else
            {
                int_value_self = dev_id;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value_self;

        default :
            return NULL;
    }
}


u_char *devicefanInfo_get(struct variable *vp,
                          oid *name,
                          size_t *length,
                          int exact, size_t *var_len, WriteMethod **write_method)
{
    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    int ret = 0 ;

    struct devm_unit t_devm_unit;
    struct devm_box t_devm_box;
    enum DEV_TYPE para_dev_type;
    uint16_t *pfan_speed;

    memset(&t_devm_unit, 0, sizeof(struct devm_unit));
    memset(&t_devm_box, 0, sizeof(struct devm_box));

    ret =  devm_comm_get_device(MODULE_ID_SNMPD, &t_devm_unit, &t_devm_box, &para_dev_type);

    if (DEV_TYPE_BOX == para_dev_type)
    {
        return (NULL);
    }

    if (ret != 0)
    {
        return (NULL);
    }

	struct ipc_mesg_n *pmsg = NULL; 

    switch (vp->magic)
    {
        case hhrFanSpeedMin:
            *var_len = sizeof(int);
            int_value_self = t_devm_unit.fan.speed_min;
            return (u_char *)&int_value_self ;

        case hhrFanSpeed:
            *var_len = sizeof(int) ;
            //pfan_speed = ipc_send_hal_wait_reply(NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_SNMPD, IPC_TYPE_DEVM, DEVM_INFO_FAN, IPC_OPCODE_GET, 0);

			pmsg = (struct ipc_mesg_n*)ipc_sync_send_n2(NULL, 0, 0, MODULE_ID_HAL, MODULE_ID_SNMPD,
				IPC_TYPE_DEVM, DEVM_INFO_FAN, IPC_OPCODE_GET, 0, 1000);

            if (NULL != pmsg)
            {
                int_value_self = *((uint16_t *)pmsg->msg_data);

				mem_share_free(pmsg, MODULE_ID_SNMPD);
            }
            else
            {
                int_value_self = 0;
            }

            return (u_char *)&int_value_self ;

        case hhrFanSpeedMax:
            *var_len = sizeof(int) ;
            int_value_self = t_devm_unit.fan.speed_max;
            return (u_char *)&int_value_self ;

        default :
            return NULL;
    }
}

u_char *hhrDiviceBoardTable_get(struct variable *vp,
                                oid *name,
                                size_t *length,
                                int exact, size_t *var_len, WriteMethod **write_method)
{
    int ret = 0;
    int index1 = 0;
    int index_next = 0;
    time_t curr_time;

    static struct devm_slot slot_info;
    static struct devm_unit t_devm_unit;
    static struct devm_box t_devm_box;
    static enum DEV_TYPE para_dev_type ;

    *write_method = NULL;

    ret = ipran_snmp_int_index_get(vp, name, length, &index1, exact);

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index1 [%d]\n", __FILE__, __LINE__, __func__, index1);

    if (ret < 0 || index1 >= SLOT_NUM_MAX - 1)
    {
        return NULL;
    }

    if (0 == index1)
    {
        if (exact)
        {
            return NULL;
        }
    }

    if (0 == exact)
    {
        index_next = index1 + 1;
    }
    else
    {
        index_next = index1;
    }

    if (data_loaded == 0)
    {
        memset(&t_devm_unit, 0, sizeof(struct devm_unit));
        memset(&t_devm_box, 0, sizeof(struct devm_box));

        ret =  devm_comm_get_device(MODULE_ID_SNMPD, &t_devm_unit, &t_devm_box, &para_dev_type);

        if (0 != ret)
        {
            return (NULL);
        }
    }

    if (DEV_TYPE_SHELF == para_dev_type)
    {
        if (t_devm_unit.slot_num <= index1)
        {
            return (NULL);
        }

        memset(&slot_info, 0, sizeof(struct devm_slot));
        while(devm_comm_get_slot(1, index_next, MODULE_ID_SNMPD, &slot_info))
        {
		index_next += 1;
		if(index_next > t_devm_unit.slot_num)
		{
			return NULL;
		}
	}
	data_loaded = 1 ;

        if (hhrDeviceBoardRunningTime == vp->magic)
        {
            if (DEV_STATUS_PLUGIN == slot_info.status || DEV_STATUS_WORK == slot_info.status)
            {
                devm_comm_get_running_time(1, index_next, MODULE_ID_SNMPD, &curr_time);
            }
        }
    }
    else   //for box
    {
        if (index1 >= 1)
        {
            return (NULL);
        }

        if (hhrDeviceBoardRunningTime == vp->magic)
        {
            devm_comm_get_running_time(1, 1, MODULE_ID_SNMPD, &curr_time);
        }
    }

    if (0 == exact)
    {
        ipran_snmp_int_index_set(vp, name, length, index_next) ;
    }

    switch (vp->magic)
    {
        case hhrDeviceBoardType:

            if (DEV_TYPE_SHELF == para_dev_type)
            {
                *var_len = sizeof(int);
                int_value_self = slot_info.type;

                if (SLOT_TYPE_BOARD_MAIN == int_value_self)
                {
                    int_value_self = 1;
                }
                else if (SLOT_TYPE_BOARD_SLAVE == int_value_self)
                {
                    int_value_self = 2;
                }
                else if (SLOT_TYPE_CARD_ETH == int_value_self)
                {
                    int_value_self = 3;
                }
                else if (SLOT_TYPE_CARD_TDM == int_value_self)
                {
                    int_value_self = 4;
                }
                else if (SLOT_TYPE_CARD_STM == int_value_self)
                {
                    int_value_self = 5;
                }
                else if (SLOT_TYPE_CARD_CLOCK == int_value_self)
                {
                    int_value_self = 6;
                }
                else if (SLOT_TYPE_POWER == int_value_self)
                {
                    int_value_self = 7;
                }
                else if (SLOT_TYPE_FAN == int_value_self)
                {
                    int_value_self = 8;
                }
                else if (SLOT_TYPE_SDH == int_value_self)
                {
                    int_value_self = 9;
                }
                else
                {
                    int_value_self = 0;
                }
            }
            else
            {
                *var_len = sizeof(int);
                int_value_self = SLOT_TYPE_BOARD_MAIN;
            }

            return (u_char *)&int_value_self ;

        case hhrDeviceBoardStatus:
            *var_len = sizeof(int);

            if (DEV_TYPE_SHELF == para_dev_type)
            {
                int_value_self = slot_info.status;
            }
            else
            {
                int_value_self = 6;
            }

            return (u_char *)&int_value_self;

        case hhrDeviceBoardRunningTime:
            *var_len = sizeof(int);

            if (DEV_TYPE_SHELF == para_dev_type)
            {
                if (DEV_STATUS_PLUGIN == slot_info.status || DEV_STATUS_WORK == slot_info.status)
                {
                    int_value_self = slot_info.run_time;
                }
                else
                {
                    int_value_self = curr_time;
                }
            }
            else
            {
                int_value_self = t_devm_box.run_time;
            }

            if (curr_time >= int_value_self)
            {
                int_value_self = curr_time - int_value_self;
                int_value_self *= 100;
            }
            else
            {
                int_value_self = 0;
            }

            return (u_char *)&int_value_self;

        case hhrDeviceBoardName:
            memset(str_value, 0, NAME_STRING_LEN);

            if (DEV_TYPE_SHELF == para_dev_type)
            {
                memcpy(str_value, slot_info.name,  NAME_STRING_LEN);
            }
            else
            {
                memcpy(str_value, t_devm_box.dev_name,  NAME_STRING_LEN);
            }

            *var_len = strlen(str_value);
            return (str_value);

        case hhrDeviceBoardID:
            *var_len = sizeof(int);

            if (DEV_TYPE_SHELF == para_dev_type)
            {
                int_value_self = slot_info.id;
            }
            else
            {
                int_value_self = t_devm_box.id;
            }

            return (u_char *)&int_value_self;

        default :
            return NULL;
    }
}

u_char *hhrDevicePowerTable_get(struct variable *vp,
                                oid *name,
                                size_t *length,
                                int exact, size_t *var_len, WriteMethod **write_method)
{
    int ret = 0 ;
    int index1 = 0 , index_next = 0 ;
    struct devm_unit t_devm_unit;
    struct devm_box t_devm_box;
    enum DEV_TYPE para_dev_type;

    *write_method = NULL;

    ret = ipran_snmp_int_index_get(vp, name, length, &index1, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (0 == index1)
    {
        if (exact)
        {
            return NULL;
        }
    }

    if (0 == exact)
    {
        index_next = index1 + 1;
    }
    else
    {
        index_next = index1;
    }

    memset(&t_devm_unit, 0, sizeof(struct devm_unit));
    memset(&t_devm_box, 0, sizeof(struct devm_box));

    ret =  devm_comm_get_device(MODULE_ID_SNMPD, &t_devm_unit, &t_devm_box, &para_dev_type);

    if (ret != 0)
    {
        return (NULL);
    }

    if (DEV_TYPE_BOX == para_dev_type)
        if (index_next > 1)
        {
            return (NULL);
        }

    if (DEV_TYPE_SHELF == para_dev_type)
        if (index_next > 2)
        {
            return (NULL);
        }

    if (0 == exact)
    {
        ipran_snmp_int_index_set(vp, name, length, index_next) ;
    }

    switch (vp->magic)
    {
        case hhrDevicePowerType:
            *var_len = sizeof(int) ;

            if (DEV_TYPE_SHELF == para_dev_type)
            {
                int_value_self = t_devm_unit.power[index_next - 1].mode;
            }
            else
            {
                int_value_self = t_devm_box.power[index_next - 1].mode;

            }

            if (int_value_self == POWER_TYPE_48v_12V_75W)
            {
                int_value_self = 0;
            }
            else if (int_value_self == POWER_TYPE_48v_12V_150W)
            {
                int_value_self = 1;
            }
            else if (int_value_self == POWER_TYPE_220v_12V_75W)
            {
                int_value_self = 2;
            }
            else if (int_value_self == POWER_TYPE_220v_12V_150W)
            {
                int_value_self = 3;
            }
            else if (int_value_self == POWER_TYPE_PWR01A)
            {
                int_value_self = 5;
            }

            else if (int_value_self == POWER_TYPE_PWR02A)
            {
                int_value_self = 6;
            }
            else
            {
                int_value_self = 4;
            }

            return (u_char *)&int_value_self;

        case hhrDevicePowerInVoltageLow:
            *var_len = sizeof(int);

            if (DEV_TYPE_SHELF == para_dev_type)
            {
                int_value_self = t_devm_unit.power[index_next - 1].voltage_low;
            }
            else
            {
                int_value_self = t_devm_box.power[index_next - 1].voltage_low;
            }

            return (u_char *)&int_value_self ;

        case hhrDevicePowerInVoltage:
            *var_len = sizeof(int);

            if (DEV_TYPE_SHELF == para_dev_type)
            {

                int_value_self = t_devm_unit.power[index_next - 1].voltage_input;
            }
            else
            {
                int_value_self = t_devm_box.power[index_next - 1].voltage_input;
            }

            return (u_char *)&int_value_self ;

        case hhrDevicePowerInVoltageHigh:
            *var_len = sizeof(int) ;

            if (DEV_TYPE_SHELF == para_dev_type)
            {
                int_value_self = t_devm_unit.power[index_next - 1].voltage_high;
            }
            else
            {
                int_value_self = t_devm_box.power[index_next - 1].voltage_high;
            }

            return (u_char *)&int_value_self ;

		case hhrDevicePowerOutVoltage:
            *var_len = sizeof(int);

            if (DEV_TYPE_SHELF == para_dev_type)
            {

                int_value_self = t_devm_unit.power[index_next - 1].voltage_output;
            }
            else
            {
                int_value_self = t_devm_box.power[index_next - 1].voltage_output;
            }

            return (u_char *)&int_value_self ;

        default :
            return NULL;
    }
}

u_char *devicetempInfo_get(struct variable *vp,
							oid *name,
							size_t *length,
							int exact, size_t *var_len, WriteMethod **write_method)
{
	if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
	{
		return NULL;
	}

	devmtemper *pdata = (devmtemper *)devm_comm_get_dev_scalar(MODULE_ID_SNMPD, DEVM_INFO_TEMP);

	switch (vp->magic)
	{
		case hhrTemperatureMin:
			int_value_self = 0;

			if (pdata)
			{
				int_value_self = pdata->temp_low;
			}

			*var_len = sizeof(int);
			return (u_char *)&int_value_self;

		case hhrTemperature:
			int_value_self = 0;

			if (pdata)
			{
				int_value_self = pdata->temprature;
			}

			*var_len = sizeof(int);
			return (u_char *)&int_value_self;

		case hhrTemperatureMax:
			int_value_self = 0;

			if (pdata)
			{
				int_value_self = pdata->temp_high;
			}

			*var_len = sizeof(int);
			return (u_char *)&int_value_self;

		default:
			return NULL;
	}
}

struct system_threshold sys_threshold_common;

								

struct system_threshold *system_threshold_info_get(uint32_t index)
{
    struct ipc_mesg_n * psnd_msg = NULL;
    psnd_msg = ipc_sync_send_n2(NULL,1,1,MODULE_ID_SYSTEM,MODULE_ID_SNMPD,IPC_TYPE_SYSTEM, \ 
                    IPC_TYPE_SNMP_SYSTEM_THRESHOLD_INFO,IPC_OPCODE_GET,index,2000);
        
    if (psnd_msg == NULL)
    {
        zlog_debug(SNMP_DBG_MIB_GET,"%s[%d] send IPC get info failed! \n", __FUNCTION__, __LINE__);
        return NULL;
    }
    memcpy(&sys_threshold_common, psnd_msg->msg_data, sizeof(struct system_threshold));

    if(psnd_msg)
    {
        mem_share_free(psnd_msg, MODULE_ID_SNMPD);
    }
    return &sys_threshold_common;
}

u_char *systemThreshold_get(struct variable *vp,
                            oid *name,
                            size_t *length,
                            int exact, size_t *var_len, WriteMethod **write_method)
{
    int ret = 0 ;
    struct system_threshold *pthreshold_data = NULL;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    pthreshold_data = system_threshold_info_get(0);

    switch (vp->magic)
    {
        case hhrCpuThreshold:
            int_value_self = 0;

            if (pthreshold_data)
            {
                int_value_self = pthreshold_data->cpu_alarm_threshold;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value_self;

        case hhrMemoryThreshold:
            int_value_self = 0;

            if (pthreshold_data)
            {
                int_value_self = pthreshold_data->memory_alarm_threshold;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value_self;

        default:
            return NULL;
    }
}

u_char *memoryInfo_get(struct variable *vp,
                       oid *name,
                       size_t *length,
                       int exact, size_t *var_len, WriteMethod **write_method)
{
    int ret = 0;
    struct sysinfo info;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    sysinfo(&info);

    switch (vp->magic)
    {
        case hhrMemoryTotal:
            *var_len = sizeof(int);
            int_value_self = (info.totalram / 1024);
            return (u_char *)&int_value_self ;

        case hhrMemoryUsed:
            *var_len = sizeof(int) ;
            int_value_self = ((info.totalram - info.freeram) / 1024);
            return (u_char *)&int_value_self ;

        case hhrMemoryFree:
            *var_len = sizeof(int);
            int_value_self = (info.freeram / 1024);
            return (u_char *)&int_value_self ;

        case hhrMemoryShared:
            *var_len = sizeof(int) ;
            int_value_self = (info.sharedram / 1024);
            return (u_char *)&int_value_self ;

        case hhrMemoryBuffersUsed:
            *var_len = sizeof(int);
            int_value_self = ((info.totalram - info.freeram - info.bufferram) / 1024);
            return (u_char *)&int_value_self ;

        case hhrMemoryBuffersFree:
            *var_len = sizeof(int) ;
            int_value_self = ((info.freeram + info.bufferram) / 1024);
            return (u_char *)&int_value_self ;

        case hhrMemorySwapTotal:
            *var_len = sizeof(int);
            int_value_self = (info.totalswap / 1024);
            return (u_char *)&int_value_self ;

        case hhrMemorySwapUsed:
            *var_len = sizeof(int) ;
            int_value_self = ((info.totalswap - info.freeswap) / 1024);
            return (u_char *)&int_value_self ;

        case hhrMemorySwapFree:
            *var_len = sizeof(int);
            int_value_self = (info.freeswap / 1024);
            return (u_char *)&int_value_self ;

        default:
            return NULL;
    }
}
uint32_t system_next_cpu_num_common;
uint32_t *system_get_next_cpu_num(uint32_t ifindex, int module_id)
{
    struct ipc_mesg_n * psnd_msg = NULL;
    psnd_msg = ipc_sync_send_n2(NULL,1,1,MODULE_ID_SYSTEM,module_id,IPC_TYPE_SYSTEM, \ 
                    IPC_TYPE_SNMP_SYSTEM_CPU_NUM_NEXT,IPC_OPCODE_GET,ifindex,2000);
        
    if (psnd_msg == NULL)
    {
        zlog_debug(SNMP_DBG_MIB_GET,"%s[%d] send IPC get info failed! \n", __FUNCTION__, __LINE__);
        return NULL;
    }
    memcpy(&system_next_cpu_num_common, psnd_msg->msg_data, sizeof(uint32_t));
    if(psnd_msg)
    {
        mem_share_free(psnd_msg, MODULE_ID_SNMPD);
    }
    return &system_next_cpu_num_common;
}

int hhrCpuInfoTable_lookup(int exact, uint32_t index, uint32_t *index_next)
{
    uint32_t *pifindex = NULL;

    if (index_next == NULL)
    {
        return FALSE;
    }

    if (exact)
    {
        *index_next = index;
    }
    else
    {
        pifindex = system_get_next_cpu_num(index, MODULE_ID_SNMPD);

        if (pifindex)
        {
            if (*pifindex == 0)
            {
                return FALSE;
            }

            *index_next = *pifindex;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}

cpu_info_t system_cpu_info_common;

cpu_info_t *system_cpu_info_table_get(uint32_t cpu_num)
{
    struct ipc_mesg_n * psnd_msg = NULL;
    psnd_msg = ipc_sync_send_n2(NULL,1,1,MODULE_ID_SYSTEM,MODULE_ID_SNMPD,IPC_TYPE_SYSTEM, \ 
                    IPC_TYPE_SNMP_SYSTEM_CPU_INFO,IPC_OPCODE_GET,cpu_num,2000);
        
    if (psnd_msg == NULL)
    {
        zlog_debug(SNMP_DBG_MIB_GET,"%s[%d] send IPC get info failed! \n", __FUNCTION__, __LINE__);
        return NULL;
    }
    memcpy(&system_cpu_info_common, psnd_msg->msg_data, sizeof(cpu_info_t));
    if(psnd_msg)
    {
        mem_share_free(psnd_msg, MODULE_ID_SNMPD);
    }

    return &system_cpu_info_common;
}

u_char *hhrCpuInfoTable_get(struct variable *vp,
                            oid *name,
                            size_t *length,
                            int exact, size_t *var_len, WriteMethod **write_method)
{
    u_int32_t index1 = 0;
    u_int32_t index_next = 0;
    int ret = 0, int_value;
    cpu_info_t *pcpu_info = NULL;

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index1, exact);

    if (ret < 0)
    {
        return NULL;
    }

    /* lookup */
    ret = hhrCpuInfoTable_lookup(exact, index1, &index_next);

    if (ret == FALSE)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, index_next);
    }

    pcpu_info = system_cpu_info_table_get(index_next);

    if (pcpu_info == NULL)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "hhrCpuInfoTable_get failed index: 0x%0x\n", index_next);
        return NULL;
    }

    /*
    * this is where we do the value assignments for the mib results.
    */
    switch (vp->magic)
    {
        case hhrCPUUsr :
            *var_len = sizeof(int);
            int_value_self = pcpu_info->usr_usage;
            return (u_char *)&int_value_self;

        case hhrCPUNice :
            *var_len = sizeof(int);
            int_value_self = pcpu_info->nice_usage;
            return (u_char *)&int_value_self;

        case hhrCPUSys :
            *var_len = sizeof(int);
            int_value_self = pcpu_info->sys_usage;
            return (u_char *)&int_value_self;

        case hhrCPUIdle :
            *var_len = sizeof(int);
            int_value_self = pcpu_info->idle_usage;
            return (u_char *)&int_value_self;

        case hhrCPUIO :
            *var_len = sizeof(int);
            int_value_self = pcpu_info->io_usage;
            return (u_char *)&int_value_self;

        case hhrCPUIrq :
            *var_len = sizeof(int);
            int_value_self = pcpu_info->irq_usage;
            return (u_char *)&int_value_self;

        case hhrCPUSirq :
            *var_len = sizeof(int);
            int_value_self = pcpu_info->sirq_usage;
            return (u_char *)&int_value_self;

        default :
            return (NULL);
    }
}



int hhrEEpromInfoTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
											devdigtlablinfo *pinfo_input)
{
	devdigtlablinfo *pinfo = NULL;
	int data_num = 0;
	int ret = 0;
	pinfo = (devdigtlablinfo *)devm_comm_reply_snmp_bulk(&pinfo_input->index, &data_num);

	zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index [%d] data_num [%d]\n", __FILE__, __LINE__, __func__,
			   pinfo_input->index.slot, data_num);

	if (0 == data_num || NULL == pinfo)
	{
		return FALSE;
	}
	else
	{
		for (ret = 0; ret < data_num; ret++)
		{
			snmp_cache_add(cache, pinfo , sizeof(devdigtlablinfo));
			zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index [%d] [%d]\n", __FILE__, __LINE__, __func__,
					   pinfo->index.slot, ret);
			pinfo++;
		}

		return TRUE;
	}
}

devdigtlablinfo *hhrEEpromInfoTable_node_lookup(struct ipran_snmp_data_cache *cache,
						int exact, const devdigtlablindex *index)
{
	struct listnode  *node, *nnode;
	devdigtlablinfo *pinfo_find = NULL;

	for(ALL_LIST_ELEMENTS(cache->data_list, node, nnode, pinfo_find))
    {
        if ((NULL == index) || (0 == index->slot))
        {
            return (devdigtlablinfo *)cache->data_list->head->data ;
        }

        if (pinfo_find->index.slot == index->slot)
        {
            if (1 == exact) //get
            {
                return pinfo_find;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }
                else
                {
                    return (devdigtlablinfo *)node->next->data;
                }
            }
        }
    }

	return NULL;
}



u_char *hhrEEpromInfoTable_get(struct variable *vp,
								  oid *name,
								  size_t *length,
								  int exact, size_t *var_len, WriteMethod **write_method)
{
	devdigtlablinfo *pinfo = NULL;
	devdigtlablindex index;
	int ret = 0;
	int index_int = 0 ;

	/* validate the index */
	ret = ipran_snmp_int_index_get(vp, name, length, &index_int , exact);

	index.slot = index_int ;
	zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] : index = %d\n", __func__, __LINE__, index.slot);

	if (ret < 0)
	{
		return NULL;
	}

	if (NULL == hhrEEpromInfoTable_cache)
	{
		hhrEEpromInfoTable_cache = snmp_cache_init(sizeof(devdigtlablinfo),
													  hhrEEpromInfoTable_get_data_from_ipc,
													  hhrEEpromInfoTable_node_lookup);

		if (NULL == hhrEEpromInfoTable_cache)
		{
			zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
			return NULL;
		}
	}

	pinfo = (devdigtlablinfo *)snmp_cache_get_data_by_index(hhrEEpromInfoTable_cache, exact, &index);

	if (NULL == pinfo)
	{
		return NULL;
	}

	/* get ready the next index */
	if (!exact)
	{
		ipran_snmp_int_index_set(vp, name, length, pinfo->index.slot);
	}

	switch (vp->magic)
	{
		case hhrEEpromHwcfg:
			memset(str_value, 0, STRING_LEN);
			memcpy(str_value, pinfo->data.digtlabldata.hwcfg, strlen(pinfo->data.digtlabldata.hwcfg));
			*var_len = strlen(str_value);
			return str_value;

		case hhrEEpromDev:
			memset(str_value, 0, STRING_LEN);
			memcpy(str_value, pinfo->data.digtlabldata.dev, strlen(pinfo->data.digtlabldata.dev));
			*var_len = strlen(str_value);
			return str_value;

		case hhrEEpromOem:
			memset(str_value, 0, STRING_LEN);
			memcpy(str_value, pinfo->data.digtlabldata.oem, strlen(pinfo->data.digtlabldata.oem));
			*var_len = strlen(str_value);
			return str_value;

		case hhrEEpromMac:
			memset(str_value, 0, STRING_LEN);
			memcpy(str_value, pinfo->data.digtlabldata.mac, strlen(pinfo->data.digtlabldata.mac));
			*var_len = strlen(str_value);
			return str_value;

		case hhrEEpromDver:
			memset(str_value, 0, STRING_LEN);
			memcpy(str_value, pinfo->data.digtlabldata.dver, strlen(pinfo->data.digtlabldata.dver));
			*var_len = strlen(str_value);
			return str_value;

		case hhrEEpromHver:
			memset(str_value, 0, STRING_LEN);
			memcpy(str_value, pinfo->data.digtlabldata.hver, strlen(pinfo->data.digtlabldata.hver));
			*var_len = strlen(str_value);
			return str_value;

		case hhrEEpromVnd:
			memset(str_value, 0, STRING_LEN);
			memcpy(str_value, pinfo->data.digtlabldata.vnd, strlen(pinfo->data.digtlabldata.vnd));
			*var_len = strlen(str_value);
			return str_value;

		case hhrEEpromSn:
			memset(str_value, 0, STRING_LEN);
			memcpy(str_value, pinfo->data.digtlabldata.sn, strlen(pinfo->data.digtlabldata.sn));
			*var_len = strlen(str_value);
			return str_value;

		case hhrEEpromDate:
			*var_len = sizeof(int);
            int_value_self = pinfo->data.digtlabldata.date;
            return (u_char *)&int_value_self;

		case hhrEEpromBom:
			memset(str_value, 0, STRING_LEN);
			memcpy(str_value, pinfo->data.digtlabldata.bom, strlen(pinfo->data.digtlabldata.bom));
			*var_len = strlen(str_value);
			return str_value;

		case hhrEEpromLic:
			memset(str_value, 0, STRING_LEN);
			memcpy(str_value, pinfo->data.digtlabldata.lic, strlen(pinfo->data.digtlabldata.lic));
			*var_len = strlen(str_value);
			return str_value;

		default:
			return NULL;
	}
}



