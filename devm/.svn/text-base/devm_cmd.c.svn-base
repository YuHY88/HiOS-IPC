#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <lib/command.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/hash1.h>
#include <lib/ifm_common.h>
#include <lib/linklist.h>
#include <lib/msg_ipc.h>
#include <lib/zassert.h>
#include <lib/errcode.h>
#include <lib/log.h>
#include <lib/vty.h>
#include <syslog/syslogd.h>
#include "devm.h"
#include "devm_ha.h"

unsigned int reset_master_count = 0;
int reset_master_slot_status[2] = {0};

extern unsigned char sys_version[SLOT_NUM_MAX][NAME_STRING_LEN];
extern int gPwrSlot1, gPwrSlot2, gFanSlot;

uint8_t watchdog_max = 1;

uchar a_backplane_ver[][10] =
{
    "invalid",
    "H20RN2000",
    "H20RN1000",
    "H20RN2000F",
    "HT2200",
    "HT2100"
};


uchar a_slot_type[][10] =
{
    "invalid",
    "main",
    "slave",
    "eth",
    "tdm",
    "stm",
    "clock",
    "power",
    "fan",
    "sdh"
};


uchar a_slot_status[][25] =
{
    "unkown",
    "plugin",
    "plugout",
    "hide",
    "reboot",
    "reset",
    "work"
};


uchar a_port_fiber[][8] =
{
    "fiber",
    "copper",
    "combo",
    "other"
};


uchar a_port_type[][25] =
{
    "invalid",
    "subport",
    "vport",
    "fe",
    "ge",
    "10ge",
    "40ge",
    "100ge",
    "combo",
    "e1",
    "t1",
    "stm1",
    "stm4",
    "stm16",
    "stm64",
    "clock2m",
    "odu0",
    "odu1",
    "odu2",
    "odu3",
    "odu4",
    "physical",
    "ce1"
};


uchar a_cpu_type[][25] =
{
    "none",
    "BCM5626X",
    "BCM53003",
    "P1020NSN2HFB",
    "VITESS7430"
};

uchar a_mem_size[][7] =
{
    "NONE",
    "256MB",
    "512MB",
    "1GB",
    "2GB",
    "4GB",
};

uchar a_flash_size[][7] =
{
    "NONE",
    "8GB",
    "16GB",
    "32GB",
    "64GB",
    "128GB",
    "256GB",
};


uchar a_sd_size[][7] =
{
    "NONE",
    "16GB",
    "32GB",
    "64GB",
    "128GB",
    "256GB",
};


uchar a_gre_cmd[][10] = {"disable", "enable"};

uchar a_power_type[][25] =
{
    "PWR4875(-48V/12V)75W",
    "-",
    "PWR48150(-48V/12V)150W",
    "-",
    "PWR2275(~220V/12V)75W",
    "-",
    "PWR22150(~220V/12V)150W",
    "NO POWER",
    "PWR01A(-48V/12V)350W",
    "PWR02A(~220V/12V)350W",
};
const struct message devm_debug_name[] =
{
	{.key = DEVM_DBG_COMM,		.str = "comm"},
	{.key = DEVM_DBG_HA,		       .str = "ha"},
	{.key = DEVM_DBG_MSG,		.str = "msg"},
	{.key = DEVM_DBG_REG,		.str = "reg"},
	{.key = DEVM_DBG_TIMER,		.str = "timer"},
	{.key = DEVM_DBG_ALL,		.str = "all"}
};


DEFUN (devm_debug_monitor,
	devm_debug_monitor_cmd,
	"debug devm (enable|disable) (comm|ha|msg|reg|timer|all)",
	"Debug information to moniter\n"
	"Programe name\n"
	"Enable statue\n"
	"Disatble statue\n"
	"Type name of common message\n"
	"Type name of ha message\n"
	"Type name of msg message\n"
	"Type name of register message \n"
	"Type name of timer message \n"
	"Type name of all debug\n")
{
	unsigned int typeid = 0;
	int zlog_num;

	for(zlog_num = 0; zlog_num < array_size(devm_debug_name); zlog_num++)
	{
		if(!strncmp(argv[1], devm_debug_name[zlog_num].str, 2))
		{
			zlog_debug_set( vty, devm_debug_name[zlog_num].key, !strncmp(argv[0], "enable", 3));

			return CMD_SUCCESS;
		}
	}

	vty_out (vty, "No debug typd find %s", VTY_NEWLINE);

	return CMD_SUCCESS;
}


DEFUN (show_devm_debug_monitor,
	show_devm_debug_monitor_cmd,
	"show devm debug",
	SHOW_STR
	"Syslog"
	"Debug status\n")
{
	int type_num;

	vty_out(vty, "debug type         status %s", VTY_NEWLINE);

	for(type_num = 0; type_num < array_size(devm_debug_name); ++type_num)
	{
		vty_out(vty, "%-15s    %-10s %s", devm_debug_name[type_num].str, 
			!!(vty->monitor & (1 << type_num)) ? "enable" : "disable", VTY_NEWLINE);
	}

	return CMD_SUCCESS;
}


extern unsigned char a_ha_bootstate[][25];
extern char dynamic_voltage_switch[2];
extern enum HA_BOOTSTATE myboard_dev_start_status ;
extern enum HA_BOOTSTATE otherboard_dev_start_status;
extern unsigned int slotn_power_ctrl;
extern EEP_KEY *g_peep_info;
extern EEP_KEY key_words_main;
extern EEP_KEY key_words_back;
extern EEP_KEY key_words_slot;
extern EEP_KEY key_words_power;
extern EEP_KEY key_words_fan;
extern EEP_KEY key_words_digital;

#if 1
#endif


DEFUN(devm_node_inter,
      devm_node_inter_cmd,
      "device",
      "inter devm node\n"
     )
{
    vty->node = DEVM_NODE;
    return CMD_SUCCESS;
}


#if 1
#endif


DEFUN(set_slotn_power,
      set_slotn_power_cmd,
      "set slot <1-32> power (up|down)",
      "set devm para\n"
      "shelf equ slot num\n"
      "slot range from 1 to 32\n"
      "power cmd control slot power\n"
      "set slotn power up\n"
      "set slotn power down\n"
     )
{
    unsigned char slot = 0;
    unsigned char slot_cmd_str[16] = {0};
    unsigned char slot_cmd = 0;
    unsigned char ret_func = CMD_SUCCESS;

    if (NULL != argv[0])
    {
        slot = atoi(argv[0]);
    }

    memcpy(slot_cmd_str, argv[1], strlen(argv[1]));

    if (!strncmp((char *)slot_cmd_str, "up", strlen("up")))
    {
        slot_cmd = 0;
    }
    else if (!strncmp((char *)slot_cmd_str, "down", strlen("down")))
    {
        slot_cmd = 1;
    }

    switch (device.type)
    {
        case DEV_TYPE_BOX:
            vty_warning_out(vty, "box equ not support to set slot power up/down%s", VTY_NEWLINE);
            ret_func = CMD_WARNING;
            break;

        case DEV_TYPE_SHELF:
            if (NULL == device.pmyunit)
            {
                vty_warning_out(vty, "shelf equ but unit not exist%s", VTY_NEWLINE);
                ret_func = CMD_WARNING;
            }
            else if ((NULL != device.pmyunit->pslot[slot - 1])
                     && (SLOT_TYPE_BOARD_MAIN == device.pmyunit->pslot[slot - 1]->type
                         || SLOT_TYPE_BOARD_SLAVE == device.pmyunit->pslot[slot - 1]->type))
            {
                vty_warning_out(vty, "mainboard or slaveboard not support to execute power up/down cmd%s", VTY_NEWLINE);
                ret_func = CMD_WARNING;
            }
            else
            {
                if (!devm_send_set_slot_power_up_down(slot, slot_cmd))
                {
                    if (1 == slot_cmd)
                    {
                        slotn_power_ctrl |= (1 << (slot - 1));
                    }
                    else
                    {
                        slotn_power_ctrl &= (~(1 << (slot - 1)));
                    }

                    ret_func = CMD_SUCCESS;
                }
                else
                {
                    vty_warning_out(vty, "fail to set slot %d power %s%s", slot, (0 == slot_cmd) ? "up" : "down", VTY_NEWLINE);
                    ret_func = CMD_WARNING;
                }
            }

            break;

        default:
            vty_warning_out(vty, "equ type(%d) error not box and shelf%s", device.type, VTY_NEWLINE);
            ret_func = CMD_WARNING;
            break;
    }

    return ret_func;
}


DEFUN(set_temperature_limit,
      set_temperature_limit_cmd,
      "set temperature (upper|lower) limit <-50-100>",
      "set devm para\n"
      "the temperature limit of device\n"
      "the top of working temperature\n"
      "the bottom of wroking temperature\n"
      "limit\n"
      "the range in which we can set temperature,box equ work temperature <-10~55>, shelf equ work temperate <0~45>\n"
     )
{
    int ret = 0;
    char temp_cmd[32] = {0};
    unsigned char t_temp_cmd = 0;
    signed int t_temp_limit = 0;

    memcpy(temp_cmd, argv[0], strlen(argv[0]));

    if (!strncmp(temp_cmd, "upper", strlen("upper")))
    {
        t_temp_cmd = 0;
    }
    else if (!strncmp(temp_cmd, "lower", strlen("lower")))
    {
        t_temp_cmd = 1;
    }

    t_temp_limit = atoi(argv[1]);

    switch (device.type)
    {
        case DEV_TYPE_BOX:
            if (0 == t_temp_cmd)
            {
                if (t_temp_limit < device.pbox->temp.temp_low)
                {
                    vty_error_out(vty, "upper limit(%d) can not less than lower limit(%d)%s", t_temp_limit, device.pbox->temp.temp_low, VTY_NEWLINE);
                }
                else
                {
                    device.pbox->temp.temp_high = t_temp_limit;
                    ret = devm_update_temperature_limit(&device.pbox->temp);

                    if (0 == ret)
                    {
                        devm_check_temp_alarm(device.pbox->temp.temprature, &device.pbox->temp);
                    }
                }
            }
            else if (1 == t_temp_cmd)
            {
                if (t_temp_limit > device.pbox->temp.temp_high)
                {
                    vty_error_out(vty, "lower limit(%d) can not greater than upper limit(%d)%s", t_temp_limit, device.pbox->temp.temp_high, VTY_NEWLINE);
                }
                else
                {
                    device.pbox->temp.temp_low = t_temp_limit;
                    ret = devm_update_temperature_limit(&device.pbox->temp);

                    if (0 == ret)
                    {
                        devm_check_temp_alarm(device.pbox->temp.temprature, &device.pbox->temp);
                    }
                }
            }

            break;

        case DEV_TYPE_SHELF:
            if (0 == t_temp_cmd)
            {
                if (t_temp_limit < device.pmyunit->pslot[device.pmyunit->myslot - 1]->temp.temp_low)
                {
                    vty_error_out(vty, "upper limit(%d) can not less than lower limit(%d)%s", t_temp_limit, device.pmyunit->pslot[device.pmyunit->myslot - 1]->temp.temp_low, VTY_NEWLINE);
                }
                else
                {
                    device.pmyunit->pslot[device.pmyunit->myslot - 1]->temp.temp_high = t_temp_limit;
                    ret = devm_update_temperature_limit(&device.pmyunit->pslot[device.pmyunit->myslot - 1]->temp);

                    if (0 == ret)
                    {
                        devm_check_temp_alarm(device.pmyunit->pslot[device.pmyunit->myslot - 1]->temp.temprature, &device.pmyunit->pslot[device.pmyunit->myslot - 1]->temp);
                    }
                }
            }
            else if (1 == t_temp_cmd)
            {
                if (t_temp_limit > device.pmyunit->pslot[device.pmyunit->myslot - 1]->temp.temp_high)
                {
                    vty_error_out(vty, "lower limit(%d) can not greater than upper limit(%d)%s", t_temp_limit, device.pmyunit->pslot[device.pmyunit->myslot - 1]->temp.temp_high, VTY_NEWLINE);
                }
                else
                {
                    device.pmyunit->pslot[device.pmyunit->myslot - 1]->temp.temp_low = t_temp_limit;
                    ret = devm_update_temperature_limit(&device.pmyunit->pslot[device.pmyunit->myslot - 1]->temp);

                    if (0 == ret)
                    {
                        devm_check_temp_alarm(device.pmyunit->pslot[device.pmyunit->myslot - 1]->temp.temprature, &device.pmyunit->pslot[device.pmyunit->myslot - 1]->temp);
                    }
                }
            }

            break;

        default:
            break;
    }

    return CMD_SUCCESS;
}


DEFUN(set_dynamic_voltage,
      set_dynamic_voltage_cmd,
      "set power <1-2> dynamic voltage (enable|disable)",
      "set devm para\n"
      "which power you want to set\n"
      "power range from 1 to 2\n"
      "read dynamic voltage from power,need power hardware support\n"
      "voltage from power\n"
      "open read dynamic voltage and voltage alarm\n"
      "dclose read dynamic voltage and voltage alarm\n"
     )
{
    unsigned char t_voltage_cmd = 0;
    unsigned char temp_cmd[16] = {0};
    unsigned char powerslot = 0 ;
    int ret = 0;
    int ret_func = CMD_SUCCESS;

    powerslot = atoi(argv[0]);

    memcpy(temp_cmd, argv[1], strlen(argv[1]));

    if (!strncmp((char *)temp_cmd, "enable", strlen("enable")))
    {
        t_voltage_cmd = 0;
    }
    else if (!strncmp((char *)temp_cmd, "disable", strlen("disable")))
    {
        t_voltage_cmd = 1;
    }

    switch (device.type)
    {
        case DEV_TYPE_BOX:
            vty_warning_out(vty, "box equ not support set voltage limit%s", VTY_NEWLINE);
            ret_func = CMD_SUCCESS;
            break;

        case DEV_TYPE_SHELF:
            if (POWER_TYPE_48v_12V_75W == device.pmyunit->power[powerslot - 1].mode
                    || POWER_TYPE_48v_12V_150W == device.pmyunit->power[powerslot - 1].mode
                    || POWER_TYPE_PWR01A == device.pmyunit->power[powerslot - 1].mode)
            {
                ret = devm_send_hal_set_dynamic_voltage(powerslot, t_voltage_cmd);

                if (0 == ret)
                {
                    dynamic_voltage_switch[powerslot - 1] = t_voltage_cmd;
                    ret_func = CMD_SUCCESS;
                }
                else
                {
                    vty_warning_out(vty, "failed to set power %d dynamic%s", powerslot, VTY_NEWLINE);
                    ret_func = CMD_WARNING;
                }
            }
            else if (POWER_TYPE_220v_12V_75W == device.pmyunit->power[powerslot - 1].mode
                     || POWER_TYPE_220v_12V_150W == device.pmyunit->power[powerslot - 1].mode
                     || POWER_TYPE_PWR02A == device.pmyunit->power[powerslot - 1].mode)
            {
                vty_warning_out(vty, "alternating current not support read dynamic voltage%s", VTY_NEWLINE);
                ret_func = CMD_WARNING;
            }
            else
            {
                vty_warning_out(vty, "power not exist%s", VTY_NEWLINE);
                ret_func = CMD_WARNING;
            }

            break;

        default:
            vty_warning_out(vty, "equ type error(%d)%s", device.type, VTY_NEWLINE);
            ret_func = CMD_WARNING;
            break;
    }

    return ret_func;
}


DEFUN(set_voltage_limit,
      set_voltage_limit_cmd,
      "set power <1-2> voltage (upper|lower) limit <1-242>",
      "set devm para\n"
      "which power you want to set"
      "power range from 1 to 2"
      "the voltage limit of device\n"
      "the top of working voltage\n"
      "the bottom of wroking voltage\n"
      "limit\n"
      "the range in which we can set voltage from 1 to 242\n"
     )
{
    unsigned int powerslot = 0;
    char voltage_cmd[8] = {0};
    unsigned char t_voltage_cmd = 0;
    unsigned char t_voltage_limit = 0;
    int power_mode = 0;
    struct devm_power   *p_devm_power = NULL;
    struct devm_power temp_devm_power;

    powerslot = atoi(argv[0]);

    memcpy(voltage_cmd, argv[1], strlen(argv[1]));

    if (!strncmp(voltage_cmd, "upper", strlen("upper")))
    {
        t_voltage_cmd = 0;
    }
    else if (!strncmp(voltage_cmd, "lower", strlen("lower")))
    {
        t_voltage_cmd = 1;
    }

    t_voltage_limit = atoi(argv[2]);

    switch (device.type)
    {
        case DEV_TYPE_BOX:
            vty_warning_out(vty, "box equ not support set voltage limit%s", VTY_NEWLINE);
            break;

        case DEV_TYPE_SHELF:
            if (0 != device.pmyunit->power[powerslot - 1].voltage_output)
            {
                p_devm_power = &device.pmyunit->power[powerslot - 1];
            }
            else
            {
                vty_error_out(vty, "power %d not working, can not set limit%s", powerslot, VTY_NEWLINE);
                return CMD_WARNING;
            }

            if (0 == t_voltage_cmd)
            {
                if (t_voltage_limit < p_devm_power->voltage_low)
                {
                    vty_error_out(vty, "upper limit(%d) can not less than lower limit(%d)%s", t_voltage_limit, p_devm_power->voltage_low, VTY_NEWLINE);
                }
                else
                {
                    memcpy(&temp_devm_power, p_devm_power, sizeof(struct devm_power));
                    temp_devm_power.voltage_high = t_voltage_limit;
                    power_mode = devm_get_power_alarm_type_by_mode(temp_devm_power.mode);

                    if (0 != power_mode && -1 != power_mode)
                    {
                        devm_check_power_alarm(device.main_unit, powerslot, p_devm_power, &temp_devm_power, power_mode);
                    }

                    memcpy(p_devm_power, &temp_devm_power, sizeof(struct devm_power));

                    devm_update_voltage_limit(device.main_unit, powerslot, p_devm_power);
                }
            }
            else if (1 == t_voltage_cmd)
            {
                if (t_voltage_limit > p_devm_power->voltage_high)
                {
                    vty_error_out(vty, "lower limit(%d) can not greater than upper limit(%d)%s", t_voltage_limit, p_devm_power->voltage_high, VTY_NEWLINE);
                }
                else
                {
                    memcpy(&temp_devm_power, p_devm_power, sizeof(struct devm_power));
                    temp_devm_power.voltage_low = t_voltage_limit;
                    power_mode = devm_get_power_alarm_type_by_mode(temp_devm_power.mode);

                    if (0 != power_mode && -1 != power_mode)
                    {
                        devm_check_power_alarm(device.main_unit, powerslot, p_devm_power, &temp_devm_power, power_mode);
                    }

                    memcpy(p_devm_power, &temp_devm_power, sizeof(struct devm_power));


                    devm_update_voltage_limit(device.myunit, powerslot, &device.pmyunit->power[powerslot - 1]);
                }
            }

            break;

        default:
            break;
    }

    return CMD_SUCCESS;
}


#if 1
#endif


DEFUN(hal_cmd_show_slot_hash,
      hal_cmd_show_slot_hash_cmd,
      "hal-cmd show slot <1-32> hash",
      "show or set hal devm commond\n"
      "show devm para\n"
      "which slot\n"
      "slot range from 1 to 32\n"
      "slot plug status hash info\n"
     )
{
    int i = 0, j = 0;
    int ret = 0;
    unsigned int slot = 0;
    unsigned char slot_plug_status[SLOT_NUM_MAX] = {0};
    uchar a_slot_plug_status[][10] = {"plugout", "plugin"};

    switch (device.type)
    {
        case DEV_TYPE_BOX:
            vty_warning_out(vty, "box equ can not do this cmd%s", VTY_NEWLINE);
            break;

        case DEV_TYPE_SHELF:
            if (NULL != argv[0])
            {
                slot = atoi(argv[0]);

                memset(slot_plug_status, 0, sizeof(slot_plug_status));

                ret = devm_get_hal_slot_plug_status_hash(slot, slot_plug_status);

                if (ret < 0)
                {
                    vty_error_out(vty, "fail to get slot %d plug status%s", slot, VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, "slot %d  plug status count = %d%s", slot, ret, VTY_NEWLINE);

                    for (i = ret - 1; i >= 0; i--)
                    {
                        j++;
                        vty_out(vty, "%d   %s%s", j, a_slot_plug_status[slot_plug_status[i]], VTY_NEWLINE);
                    }
                }
            }
            else
            {
                vty_error_out(vty, "fail to get slot%s", VTY_NEWLINE);
            }

            break;

        default:
            break;
    }

    return CMD_SUCCESS;
}


DEFUN(hal_cmd_show_global_variables,
      hal_cmd_show_global_variables_cmd,
      "hal-cmd show global variables",
      "show or set hal devm commond\n"
      "show devm para\n"
      "global info\n"
      "variables info\n"
     )
{
    int ret = 0;
    unsigned int hal_device_status[2] = {0};

    switch (device.type)
    {
        case DEV_TYPE_BOX:
            vty_warning_out(vty, "box equ can not do this cmd%s", VTY_NEWLINE);
            break;

        case DEV_TYPE_SHELF:
            ret = devm_get_hal_device_status(hal_device_status, 2);

            if (0 == ret)
            {
                vty_out(vty, "   %-18s : 0x%x%s", "slot_status", hal_device_status[0], VTY_NEWLINE);
                vty_out(vty, "   %-18s : 0x%x%s", "slot_lock", hal_device_status[1], VTY_NEWLINE);
            }

            break;

        default:
            break;
    }

    return CMD_SUCCESS;
}


DEFUN(hal_cmd_show_slot,
      hal_cmd_show_slot_cmd,
      "hal-cmd show slot [<1-32>]",
      "show or set hal devm commond\n"
      "show devm para\n"
      "which slot\n"
      "slot range from 1 to 32\n"
     )
{
    int i = 0;
    int ret = 0;
    uint8_t port_num = 0;
    uint8_t slot = 1;
    uint8_t unit = device.main_unit;
    struct devm_slot t_devm_slot;
    struct devm_box t_devm_box;
    struct t_hsl_devm_port temp_hsl_devm_port[40];
    time_t current_secs;
    struct tm *p_tm = NULL;

    switch (device.type)
    {
        case DEV_TYPE_BOX:
            memset(&t_devm_box, 0, sizeof(struct devm_box));
            memset(temp_hsl_devm_port, 0, sizeof(struct t_hsl_devm_port) * 40);

            ret = devm_get_hal_box_info(&t_devm_box);

            if (0 == ret)
            {
                vty_out(vty, "box:%s", VTY_NEWLINE);

                devm_comm_get_running_time(1, 1, MODULE_ID_DEVM, &current_secs);

                if (current_secs < t_devm_box.run_time)
                {
                    vty_out(vty, "   %-18s : %s%s", "start_time", "error", VTY_NEWLINE);
                }
                else
                {
                    current_secs -= t_devm_box.run_time;
                    vty_out(vty, "   %-18s: %dD %02u:%02u:%02u%s", "running_time", current_secs/86400, (current_secs%86400)/3600, (current_secs%3600)/60, current_secs%60, VTY_NEWLINE);                    
                }

                vty_out(vty, "   %-18s: %s%s", "dev_name", t_devm_box.dev_name, VTY_NEWLINE);
                vty_out(vty, "   %-18s: %s%s", "manufacture", t_devm_box.manufacture, VTY_NEWLINE);
                vty_out(vty, "   %-18s: %#x%s", "id", t_devm_box.id, VTY_NEWLINE);

                if (0 == t_devm_box.mac[0] && 0 == t_devm_box.mac[1] && 0 == t_devm_box.mac[2] && 0 == t_devm_box.mac[3] && 0 == t_devm_box.mac[4] && 0 == t_devm_box.mac[5])
                {
                    vty_out(vty, "	 %-18s: %s:%s:%s:%s:%s:%s%s", "--", "--", "--", "--", "--", "--", "--", VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, "   %-18s: %02x:%02x:%02x:%02x:%02x:%02x%s", "mac", t_devm_box.mac[0], t_devm_box.mac[1], t_devm_box.mac[2], t_devm_box.mac[3], t_devm_box.mac[4], t_devm_box.mac[5], VTY_NEWLINE);
                }

                vty_out(vty, "   %-18s: %d%s", "mac_num", t_devm_box.mac_num, VTY_NEWLINE);
                vty_out(vty, "   %-18s: %d%s", "port_num", t_devm_box.port_num, VTY_NEWLINE);

                if (0 == t_devm_box.watchdog)
                {
                    vty_out(vty, "   %-18s: %s%s", "watchdog", "-", VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, "   %-18s: %d%s", "watchdog", t_devm_box.watchdog, VTY_NEWLINE);
                }

                /*硬件信息*/
                vty_out(vty, "   %-18s: %s%s", "cpu", a_cpu_type[t_devm_box.hw.cpu_type], VTY_NEWLINE);
                vty_out(vty, "   %-18s: %s%s", "mem", a_mem_size[t_devm_box.hw.mem_type], VTY_NEWLINE);
                vty_out(vty, "   %-18s: %s%s", "flash", a_flash_size[t_devm_box.hw.flash_type], VTY_NEWLINE);

                if (0 == t_devm_box.hw.fpga_ver)
                {
                    vty_out(vty, "   %-18s: %s%s", "fpga_ver", "-", VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, "   %-18s: %x.%x%s", "fpga_ver", (t_devm_box.hw.fpga_ver) / 16, (t_devm_box.hw.fpga_ver) % 16, VTY_NEWLINE);
                }

                vty_out(vty, "   %-18s: %s%s", "hw_ver", t_devm_box.hw.hw_ver, VTY_NEWLINE);

                if ('\0' == sys_version[slot - 1])
                {
                    vty_out(vty, "   %-18s: %s%s", "sys_ver", "-", VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, "   %-18s: %s%s", "sys_ver", sys_version[slot - 1], VTY_NEWLINE);
                }

                if (0 == t_devm_box.hw.sd_type)
                {
                    vty_out(vty, "   %-18s: %s%s", "sd", "-", VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, "   %-18s: %s%s", "sd", a_sd_size[t_devm_box.hw.sd_type], VTY_NEWLINE);
                }

                vty_out(vty, "   %-18s: %d %s", "temprature(deg C)", t_devm_box.temp.temprature, VTY_NEWLINE);
                vty_out(vty, "%s", VTY_NEWLINE);

                /*端口信息*/
                port_num = devm_get_hal_port_info(1, temp_hsl_devm_port);
                vty_out(vty, "%s", VTY_NEWLINE);
                vty_out(vty, "   port   port_type   medium   hide   phyid    gport       mac%s", VTY_NEWLINE);

                for (i = 0; i < port_num; i++)
                {
                    vty_out(vty, "   %-7d", temp_hsl_devm_port[i].port);
                    vty_out(vty, "%-12s", a_port_type[temp_hsl_devm_port[i].port_type]);
                    vty_out(vty, "%-9s", a_port_fiber[temp_hsl_devm_port[i].fiber]);
                    vty_out(vty, "%-7d", temp_hsl_devm_port[i].hide);
                    vty_out(vty, "0x%-7x", temp_hsl_devm_port[i].phyid);
                    vty_out(vty, "0x%-10x", temp_hsl_devm_port[i].gport);

                    if (0 == temp_hsl_devm_port[i].mac[0] && 0 == temp_hsl_devm_port[i].mac[1] && 0 == temp_hsl_devm_port[i].mac[2] && 0 == temp_hsl_devm_port[i].mac[3] && 0 == temp_hsl_devm_port[i].mac[4] && 0 == temp_hsl_devm_port[i].mac[5])
                    {
                        vty_out(vty, "%s:%s:%s:%s:%s:%s%s", "--", "--", "--", "--", "--", "--", VTY_NEWLINE);
                    }
                    else
                        vty_out(vty, "%02x:%02x:%02x:%02x:%02x:%02x%s", temp_hsl_devm_port[i].mac[0], temp_hsl_devm_port[i].mac[1], temp_hsl_devm_port[i].mac[2]
                                , temp_hsl_devm_port[i].mac[3], temp_hsl_devm_port[i].mac[4], temp_hsl_devm_port[i].mac[5], VTY_NEWLINE);
                }
            }
            else
            {
                vty_error_out(vty, "can not get box info from hal%s", VTY_NEWLINE);
            }

            break;

        case DEV_TYPE_SHELF:
            if (NULL != argv[0])
            {
                slot = atoi(argv[0]);
            }
            else
            {
                vty_out(vty, "you must choice slot for shelf equ%s", VTY_NEWLINE);
                return CMD_WARNING;
            }

            memset(&t_devm_slot, 0, sizeof(struct devm_slot));
            memset(temp_hsl_devm_port, 0, sizeof(struct t_hsl_devm_port) * 40);

            ret = devm_get_hal_slot_info(slot, &t_devm_slot);

            if (0 == ret)
            {
                devm_comm_get_running_time(1, slot, MODULE_ID_DEVM, &current_secs);

                vty_out(vty, "hal slot %d:%s", slot, VTY_NEWLINE);
                vty_out(vty, "   %-18s: %s %s", "type", a_slot_type[t_devm_slot.type], VTY_NEWLINE);
                vty_out(vty, "   %-18s: %s %s", "status", a_slot_status[t_devm_slot.status], VTY_NEWLINE);

                if (device.punit != NULL)
                {
                    vty_out(vty, "   %-18s: %d %s", "main_unit", device.main_unit, VTY_NEWLINE);
                    vty_out(vty, "   %-18s: %d %s", "slot_main_board", device.punit[device.main_unit - 1]->slot_main_board, VTY_NEWLINE);
                    vty_out(vty, "   %-18s: %d %s", "slot_slave_board", device.punit[device.main_unit - 1]->slot_slave_board, VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, "device.punit != NULL %s", VTY_NEWLINE);
                }

                if (t_devm_slot.slot == device.punit[device.main_unit - 1]->slot_main_board
                        || t_devm_slot.slot == device.punit[device.main_unit - 1]->slot_slave_board)
                {
                    vty_out(vty, "   %-18s: %s %s", "dev_name", t_devm_slot.dev_name, VTY_NEWLINE);
                    vty_out(vty, "   %-18s: %d %s", "watchdog", t_devm_slot.watchdog, VTY_NEWLINE);
                    vty_out(vty, "   %-18s: %d %s", "heartbeat", t_devm_slot.heartbeat, VTY_NEWLINE);
                    vty_out(vty, "   %-18s: %d %s", "temprature(deg C)", t_devm_slot.temp.temprature, VTY_NEWLINE);
                }

                vty_out(vty, "   %-18s: %#x %s", "id", t_devm_slot.id, VTY_NEWLINE);
                vty_out(vty, "   %-18s: %d %s", "unit", t_devm_slot.unit, VTY_NEWLINE);
                vty_out(vty, "   %-18s: %d %s", "slot", t_devm_slot.slot, VTY_NEWLINE);
                vty_out(vty, "   %-18s: 0x%x %s", "slot_hw", t_devm_slot.slot_hw, VTY_NEWLINE);
                vty_out(vty, "   %-18s: %d %s", "plugout", t_devm_slot.plugout, VTY_NEWLINE);
                vty_out(vty, "   %-18s: %d %s", "port_num", t_devm_slot.port_num, VTY_NEWLINE);

                if (current_secs < t_devm_slot.run_time)
                {
                    vty_out(vty, "   %-18s : %s%s", "running_time", "error", VTY_NEWLINE);
                }
                else
                {
                    current_secs -= t_devm_slot.run_time;
                    vty_out(vty, "   %-18s: %dD %02u:%02u:%02u%s", "running_time", current_secs/86400, (current_secs%86400)/3600, (current_secs%3600)/60, current_secs%60, VTY_NEWLINE);
                }

                /*硬件信息*/
                if (t_devm_slot.slot == device.punit[unit - 1]->slot_main_board
                        || t_devm_slot.slot == device.punit[unit - 1]->slot_slave_board)
                {
                    vty_out(vty, "   %-18s: %s%s", "cpu", a_cpu_type[t_devm_slot.hw.cpu_type], VTY_NEWLINE);
                }

                vty_out(vty, "   %-18s: %s%s", "mem", a_mem_size[t_devm_slot.hw.mem_type], VTY_NEWLINE);
                vty_out(vty, "   %-18s: %s%s", "flash", a_flash_size[t_devm_slot.hw.flash_type], VTY_NEWLINE);

                if (0 == t_devm_slot.hw.fpga_ver)
                {
                    vty_out(vty, "   %-18s: %s%s", "fpga_ver", "-", VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, "   %-18s: %x.%x%s", "fpga_ver", (t_devm_slot.hw.fpga_ver) / 16, (t_devm_slot.hw.fpga_ver) % 16, VTY_NEWLINE);
                }

                vty_out(vty, "   %-18s: %s%s", "hw_ver", t_devm_slot.hw.hw_ver, VTY_NEWLINE);

                if ('\0' == sys_version[slot - 1])
                {
                    vty_out(vty, "   %-18s: %s%s", "sys_ver", "-", VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, "   %-18s: %s%s", "sys_ver", sys_version[slot - 1], VTY_NEWLINE);
                }

                if (0 == t_devm_slot.hw.sd_type)
                {
                    vty_out(vty, "   %-18s: %s%s", "sd", "-", VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, "   %-18s: %s%s", "sd", a_sd_size[t_devm_slot.hw.sd_type], VTY_NEWLINE);
                }

                /*端口信息*/
                port_num = devm_get_hal_port_info(slot, temp_hsl_devm_port);
                vty_out(vty, "%s", VTY_NEWLINE);
                vty_out(vty, "   port   port_type   medium   hide   phyid    gport       mac%s", VTY_NEWLINE);

                for (i = 0; i < port_num; i++)
                {
                    vty_out(vty, "   %-7d", temp_hsl_devm_port[i].port);
                    vty_out(vty, "%-12s", a_port_type[temp_hsl_devm_port[i].port_type]);
                    vty_out(vty, "%-9s", a_port_fiber[temp_hsl_devm_port[i].fiber]);
                    vty_out(vty, "%-7d", temp_hsl_devm_port[i].hide);
                    vty_out(vty, "0x%-7x", temp_hsl_devm_port[i].phyid);
                    vty_out(vty, "0x%-10x", temp_hsl_devm_port[i].gport);

                    if (0 == temp_hsl_devm_port[i].mac[0] && 0 == temp_hsl_devm_port[i].mac[1] && 0 == temp_hsl_devm_port[i].mac[2] && 0 == temp_hsl_devm_port[i].mac[3] && 0 == temp_hsl_devm_port[i].mac[4] && 0 == temp_hsl_devm_port[i].mac[5])
                    {
                        vty_out(vty, "%s:%s:%s:%s:%s:%s%s", "--", "--", "--", "--", "--", "--", VTY_NEWLINE);
                    }
                    else
                        vty_out(vty, "%02x:%02x:%02x:%02x:%02x:%02x%s", temp_hsl_devm_port[i].mac[0], temp_hsl_devm_port[i].mac[1], temp_hsl_devm_port[i].mac[2]
                                , temp_hsl_devm_port[i].mac[3], temp_hsl_devm_port[i].mac[4], temp_hsl_devm_port[i].mac[5], VTY_NEWLINE);
                }

                vty_out(vty, "-------------------------------------------------------------------%s", VTY_NEWLINE);
            }
            else
            {
                vty_error_out(vty, "can not get slot %d info from hal%s", slot, VTY_NEWLINE);
            }


            break;

        default:
            break;
    }

    return CMD_SUCCESS;
}


DEFUN(hal_cmd_set_hash_add_slot,
      hal_cmd_set_hash_add_slot_cmd,
      "hal-cmd set hash add slot <1-32>",
      "show or set hal devm commond\n"
      "change hal hash\n"
      "slot plug status hash info hash\n"
      "insert add slot bucket to hash\n"
      "which slot\n"
      "slot range from1 to 32\n"
     )
{
    uint8_t slot = 1;

    if (NULL != argv[0])
    {
        slot = atoi(argv[0]);

        switch (device.type)
        {
            case DEV_TYPE_BOX:
                vty_warning_out(vty, "box equ do not support this cmd%s", VTY_NEWLINE);
                break;

            case DEV_TYPE_SHELF:
                if (NULL == device.pmyunit)
                {
                    vty_error_out(vty, "dev not ready%s", VTY_NEWLINE);
                }
                else
                {
                    devm_set_hash_add_slot(slot);
                }

                break;

            default:
                break;
        }
    }
    else
    {
        vty_warning_out(vty, "failed to get slot%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN(hal_cmd_set_hash_del_slot,
      hal_cmd_set_hash_del_slot_cmd,
      "hal-cmd set hash delete slot <1-32>",
      "show or set hal devm commond\n"
      "change hal hash\n"
      "slot plug status hash info hash\n"
      "insert del slot bucket to hash\n"
      "which slot\n"
      "slot range from1 to 32\n"
     )
{
    uint8_t slot = 1;

    if (NULL != argv[0])
    {
        slot = atoi(argv[0]);

        switch (device.type)
        {
            case DEV_TYPE_BOX:
                vty_warning_out(vty, "box equ do not support this cmd%s", VTY_NEWLINE);
                break;

            case DEV_TYPE_SHELF:
                if (NULL == device.pmyunit)
                {
                    vty_error_out(vty, "dev not ready%s", VTY_NEWLINE);
                }
                else
                {
                    devm_set_hash_del_slot(slot);
                }

                break;

            default:
                break;
        }
    }
    else
    {
        vty_warning_out(vty, "failed to get slot%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN(hal_cmd_set_slot_fpga_reconfig,
      hal_cmd_set_slot_fpga_reconfig_cmd,
      "hal-cmd set  slot <1-32> fpga-reconfig",
      "show or set hal devm commond\n"
      "set commond\n"
      "which slot to execute cmd\n"
      "slot range from 1 to 32\n"
      "fpga reconfig commond\n"
     )
{
    uint8_t slot = 1;

    if (NULL != argv[0])
    {
        slot = atoi(argv[0]);

        switch (device.type)
        {
            case DEV_TYPE_BOX:
                vty_warning_out(vty, "box equ do not support this cmd%s", VTY_NEWLINE);
                break;

            case DEV_TYPE_SHELF:
                if (NULL == device.pmyunit)
                {
                    vty_error_out(vty, "dev not ready%s", VTY_NEWLINE);
                }
                else if (slot == device.pmyunit->slot_main_board || slot == device.pmyunit->slot_slave_board)
                {
                    vty_error_out(vty, "this can not reconfig master or slave board%s", VTY_NEWLINE);
                }
                else if (device.pmyunit->myslot == device.pmyunit->slot_slave_board)
                {
                    vty_error_out(vty, "eonly master board can do this cmd%s", VTY_NEWLINE);
                }
                else if (device.pmyunit->pslot[slot - 1] == NULL)
                {
                    vty_error_out(vty, "slot %d not exist%s", slot, VTY_NEWLINE);
                }
                else if (SLOT_TYPE_BOARD_MAIN == device.pmyunit->pslot[slot - 1]->type
                         || SLOT_TYPE_BOARD_SLAVE == device.pmyunit->pslot[slot - 1]->type)
                {
                    vty_error_out(vty, "mainboard or slaveboard not support to execute fpga reconfig cmd%s", slot, VTY_NEWLINE);
                }
                else
                {
                    devm_set_slot_fpga_reconfig(slot);
                    sleep(10);
                }

                break;

            default:
                break;
        }

        return CMD_SUCCESS;
    }
    else
    {
        vty_warning_out(vty, "failed to get slot%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN(devm_op_eeprom_info,
      devm_op_eeprom_info_cmd,
      "devm (set|get|clear|erase) slot <0-32> eeprom (device|dev|hardware_version|oem|hver|vendor|slot_name|vnd|date|mac|1st_mac|hwcfg|macnum|ver|bom|sn|lic|cfginit|all) [WORD]",
      "show or set hal devm commond\n"
      "set commond\n"
      "set commond\n"
      "clear selected eeprom info str commond\n"
      "erase all the eeprom info commond\n"
      "box equ eeprom or shelf equ backplane eeprom\n"
      "slot 0 is box eeprom or shelf equ backplane eeprom, shelf card slot <1-32>\n"
      "eeprominfo\n"
      "dev_name info from eeprom\n"
      "oem_name info from eeprom\n"
      "hardware_version info from eeprom\n"
      "vendor info from eeprom\n"
      "slot_name info from eeprom\n"
      "mac info from eeprom\n"
      "ID info from eeprom\n"
      "mac_num info from eeprom\n"
      "all info from this eeprom\n"
      "the info user want to set"
     )
{
    int i = 0;
    int data_len = 0;
    int func_ret = 0;
    unsigned char digiFlag = 0;
    unsigned char slot = 0;
    unsigned char cmd_type = 0;
    unsigned char info_data[256] = {0};
    unsigned char find_buff[64] = {0};
    int info_type = 0;

    if (!memcmp(argv[0], "set", sizeof("set")))
    {
        cmd_type = EEP_INFO_CMD_SET;
    }
    else if (!memcmp(argv[0], "get", sizeof("get")))
    {
        cmd_type = EEP_INFO_CMD_GET;
    }
    else if (!memcmp(argv[0], "erase", sizeof("erase")))
    {
        cmd_type = EEP_INFO_CMD_ERASE;
    }
    else if (!memcmp(argv[0], "clear", sizeof("clear")))
    {
        cmd_type = EEP_INFO_CMD_CLEAR;
    }

    slot = atoi(argv[1]);

    if (EEP_INFO_CMD_SET == cmd_type)
    {
        if (NULL != argv[3])
        {
            if (strlen(argv[3]) > NAME_STRING_LEN)
            {
                vty_error_out(vty, "str len(%d) can not longer than 32%s", strlen(argv[3]), VTY_NEWLINE);
                return CMD_WARNING;
            }
            else
            {
                memset((char *)info_data, 0, sizeof(info_data));
                memcpy((char *)info_data, argv[3], strlen(argv[3]));
                info_data[strlen(argv[3]) + 1] = '\0';
                data_len = strlen((char *)info_data);
            }
        }
        else
        {
            vty_error_out(vty, "please inter eeprom info%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    else if (EEP_INFO_CMD_GET == cmd_type || EEP_INFO_CMD_ERASE == cmd_type)
    {
        data_len = sizeof(info_data);
    }
    else if (EEP_INFO_CMD_CLEAR == cmd_type)
    {
        info_data[0] = '-';
        info_data[1] = '\0';
        data_len = 1;
    }

    if (NULL != device.pbox)
    {
        info_type = devm_get_op_eeprom_type_digital((char *)argv[2]);
        g_peep_info = &key_words_digital;
        digiFlag = 1;		
    }
    else if (NULL != device.pmyunit)
    {
        info_type = devm_get_op_eeprom_type_digital((char *)argv[2]);
        g_peep_info = &key_words_digital;
        digiFlag = 1;
    }

    if (NULL == g_peep_info)
    {
        vty_error_out(vty, "box or unit not exist in devm%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    func_ret = devm_op_eeprom_check_type(cmd_type, (unsigned char *)argv[2], g_peep_info);

    if (1 == func_ret)
    {
        vty_error_out(vty, "this kind of eeprom type is not needed int the eeprom%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (EEP_INFO_CMD_SET == cmd_type)
    {
        if (EEP_INFO_MAC == info_type)
        {
            func_ret = devm_op_eeprom_check_data_form(slot, info_type, info_data, data_len);

            if (0 != func_ret)
            {
                if (1 == func_ret)
                {
                    vty_error_out(vty, "the form of mac is not correct%s", VTY_NEWLINE);
                    return CMD_WARNING;
                }
            }
        }
    }
    func_ret = devm_op_eeprom_info_func(cmd_type, slot, info_type, info_data, data_len);

    if (EEP_INFO_CMD_SET == cmd_type || EEP_INFO_CMD_ERASE == cmd_type || EEP_INFO_CMD_CLEAR == cmd_type)
    {
        if (0 == func_ret)
        {
            return CMD_SUCCESS;
        }
        else
        {
            vty_error_out(vty, "fail to op eeprom info%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    else if (EEP_INFO_CMD_GET == cmd_type)
    {
        if (0 != func_ret)
        {
            vty_error_out(vty, "fail to op eeprom info%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
        else
        {
            if(digiFlag == 1)
            {
                switch (info_type)
                {
                    case EEP_INFO_ALL:
                        for (i = 0; i < g_peep_info->num; i++)
                        {
                            memset(find_buff, 0, sizeof(find_buff));
                            devm_find_info_in_orig_buf(info_data, g_peep_info->unit[i].key_word, find_buff, sizeof(info_data));
                            vty_out(vty, "%-18s = %s%s", g_peep_info->unit[i].key_word, find_buff, VTY_NEWLINE);
                        }

                        break;

                    case EEP_INFO_DEVNAME:
                        memset(find_buff, 0, sizeof(find_buff));
                        devm_find_info_in_orig_buf(info_data, (unsigned char *)PRODUCT_KEY_DEV, find_buff, sizeof(info_data));
                        vty_out(vty, "%-18s = %s%s", PRODUCT_KEY_DEV_NAME, find_buff, VTY_NEWLINE);
                        break;

                    case EEP_INFO_OEM_NAME:
                        memset(find_buff, 0, sizeof(find_buff));
                        devm_find_info_in_orig_buf(info_data, (unsigned char *)PRODUCT_KEY_OEM_NAME, find_buff, sizeof(info_data));
                        vty_out(vty, "%-18s = %s%s", PRODUCT_KEY_OEM_NAME, find_buff, VTY_NEWLINE);
                        break;

                    case EEP_INFO_HW_VER:
                        memset(find_buff, 0, sizeof(find_buff));
                        devm_find_info_in_orig_buf(info_data, (unsigned char *)PRODUCT_KEY_HVER, find_buff, sizeof(info_data));
                        vty_out(vty, "%-18s = %s%s", PRODUCT_KEY_HW_VERSION, find_buff, VTY_NEWLINE);
                        break;

                    case EEP_INFO_VENDOR:
                        memset(find_buff, 0, sizeof(find_buff));
                        devm_find_info_in_orig_buf(info_data, (unsigned char *)PRODUCT_KEY_VND, find_buff, sizeof(info_data));
                        vty_out(vty, "%-18s = %s%s", PRODUCT_KEY_VENDOR, find_buff, VTY_NEWLINE);
                        break;

                    case EEP_INFO_BOM:
                        memset(find_buff, 0, sizeof(find_buff));
                        devm_find_info_in_orig_buf(info_data, (unsigned char *)PRODUCT_KEY_BOM, find_buff, sizeof(info_data));
                        vty_out(vty, "%-18s = %s%s", PRODUCT_KEY_BOM, find_buff, VTY_NEWLINE);
                        break;

                    case EEP_INFO_MAC:
                        memset(find_buff, 0, sizeof(find_buff));
                        devm_find_info_in_orig_buf(info_data, (unsigned char *)PRODUCT_KEY_MAC, find_buff, sizeof(info_data));
                        vty_out(vty, "%-18s = %s%s", PRODUCT_KEY_MAC, find_buff, VTY_NEWLINE);
                        break;

                    case EEP_INFO_ID:
                        memset(find_buff, 0, sizeof(find_buff));
                        devm_find_info_in_orig_buf(info_data, (unsigned char *)PRODUCT_KEY_DEV_TYPE, find_buff, sizeof(info_data));
                        vty_out(vty, "%-18s = %s%s", PRODUCT_KEY_DEV_TYPE, find_buff, VTY_NEWLINE);
                        break;

                    case EEP_INFO_DATE:
                        memset(find_buff, 0, sizeof(find_buff));
                        devm_find_info_in_orig_buf(info_data, (unsigned char *)PRODUCT_KEY_DATE, find_buff, sizeof(info_data));
                        vty_out(vty, "%-18s = %s%s", PRODUCT_KEY_DATE, find_buff, VTY_NEWLINE);
                        break;

                    case EEP_INFO_SN:
                        memset(find_buff, 0, sizeof(find_buff));
                        devm_find_info_in_orig_buf(info_data, (unsigned char *)PRODUCT_KEY_SN, find_buff, sizeof(info_data));
                        vty_out(vty, "%-18s = %s%s", PRODUCT_KEY_SN, find_buff, VTY_NEWLINE);
                        break;

                    case EEP_INFO_LIC:
                        memset(find_buff, 0, sizeof(find_buff));
                        devm_find_info_in_orig_buf(info_data, (unsigned char *)PRODUCT_KEY_LIC, find_buff, sizeof(info_data));
                        vty_out(vty, "%-18s = %s%s", PRODUCT_KEY_LIC, find_buff, VTY_NEWLINE);
                        break;

					case EEP_INFO_CFGINIT:
						memset(find_buff, 0, sizeof(find_buff));
						devm_find_info_in_orig_buf(info_data, (unsigned char *)PRODUCT_KEY_CFGINIT, find_buff, sizeof(info_data));
						vty_out(vty, "%-18s = %s%s", PRODUCT_KEY_CFGINIT, find_buff, VTY_NEWLINE);
						break;

                    case EEP_INFO_DVER:
                        memset(find_buff, 0, sizeof(find_buff));
                        devm_find_info_in_orig_buf(info_data, (unsigned char *)PRODUCT_KEY_DVER, find_buff, sizeof(info_data));
                        vty_out(vty, "%-18s = %s%s", PRODUCT_KEY_DVER, find_buff, VTY_NEWLINE);
                        break;

                    default:
                        vty_error_out(vty, "fail to op eeprom info%s", VTY_NEWLINE);
                        break;
                }
            }
            else
            {
                switch (info_type)
                {
                    case EEP_INFO_ALL:
                        for (i = 0; i < g_peep_info->num; i++)
                        {
                            memset(find_buff, 0, sizeof(find_buff));
                            devm_find_info_in_orig_buf(info_data, g_peep_info->unit[i].key_word, find_buff, sizeof(info_data));
                            vty_out(vty, "%-18s = %s%s", g_peep_info->unit[i].key_word, find_buff, VTY_NEWLINE);
                        }
                    
                        break;
                    
                    case EEP_INFO_DEVNAME:
                        memset(find_buff, 0, sizeof(find_buff));
                        devm_find_info_in_orig_buf(info_data, (unsigned char *)PRODUCT_KEY_DEV_NAME, find_buff, sizeof(info_data));
                        vty_out(vty, "%-18s = %s%s", PRODUCT_KEY_DEV_NAME, find_buff, VTY_NEWLINE);
                        break;
                    
                    case EEP_INFO_OEM_NAME:
                        memset(find_buff, 0, sizeof(find_buff));
                        devm_find_info_in_orig_buf(info_data, (unsigned char *)PRODUCT_KEY_OEM_NAME, find_buff, sizeof(info_data));
                        vty_out(vty, "%-18s = %s%s", PRODUCT_KEY_OEM_NAME, find_buff, VTY_NEWLINE);
                        break;
                    
                    case EEP_INFO_HW_VER:
                        memset(find_buff, 0, sizeof(find_buff));
                        devm_find_info_in_orig_buf(info_data, (unsigned char *)PRODUCT_KEY_HW_VERSION, find_buff, sizeof(info_data));
                        vty_out(vty, "%-18s = %s%s", PRODUCT_KEY_HW_VERSION, find_buff, VTY_NEWLINE);
                        break;
                    
                    case EEP_INFO_VENDOR:
                        memset(find_buff, 0, sizeof(find_buff));
                        devm_find_info_in_orig_buf(info_data, (unsigned char *)PRODUCT_KEY_VENDOR, find_buff, sizeof(info_data));
                        vty_out(vty, "%-18s = %s%s", PRODUCT_KEY_VENDOR, find_buff, VTY_NEWLINE);
                        break;
                    
                    case EEP_INFO_SLOT_NAME:
                        memset(find_buff, 0, sizeof(find_buff));
                        devm_find_info_in_orig_buf(info_data, (unsigned char *)PRODUCT_KEY_SLOT_NAME, find_buff, sizeof(info_data));
                        vty_out(vty, "%-18s = %s%s", PRODUCT_KEY_SLOT_NAME, find_buff, VTY_NEWLINE);
                        break;
                    
                    case EEP_INFO_MAC:
                        memset(find_buff, 0, sizeof(find_buff));
                        devm_find_info_in_orig_buf(info_data, (unsigned char *)PRODUCT_KEY_MAC, find_buff, sizeof(info_data));
                        vty_out(vty, "%-18s = %s%s", PRODUCT_KEY_MAC, find_buff, VTY_NEWLINE);
                        break;
                    
                    case EEP_INFO_ID:
                        memset(find_buff, 0, sizeof(find_buff));
                        devm_find_info_in_orig_buf(info_data, (unsigned char *)PRODUCT_KEY_DEV_TYPE, find_buff, sizeof(info_data));
                        vty_out(vty, "%-18s = %s%s", PRODUCT_KEY_DEV_TYPE, find_buff, VTY_NEWLINE);
                        break;
                    
                    case EEP_INFO_MAC_NUM:
                        memset(find_buff, 0, sizeof(find_buff));
                        devm_find_info_in_orig_buf(info_data, (unsigned char *)PRODUCT_KEY_MAC_NUM, find_buff, sizeof(info_data));
                        vty_out(vty, "%-18s = %s%s", PRODUCT_KEY_MAC_NUM, find_buff, VTY_NEWLINE);
                        break;
                    
                    case EEP_INFO_SN:
                        memset(find_buff, 0, sizeof(find_buff));
                        devm_find_info_in_orig_buf(info_data, (unsigned char *)PRODUCT_KEY_SN, find_buff, sizeof(info_data));
                        vty_out(vty, "%-18s = %s%s", PRODUCT_KEY_SN, find_buff, VTY_NEWLINE);
                        break;

                    default:
                        vty_error_out(vty, "fail to op eeprom info%s", VTY_NEWLINE);
                        break;
                }
            }     
            return CMD_SUCCESS;
        }
    }

    return CMD_SUCCESS;
}


#if 1
#endif


DEFUN(device_watchdog_interval,
      device_watchdog_interval_cmd,
      "device watchdog interval <0-30>",
      "device command\n"
      "set watchdog config\n"
      "watchdog feed interval\n"
      "0 means no watchdog\n"
     )
{
    uint8_t wdi = 0;

    wdi = atoi(argv[0]);

    if (wdi > watchdog_max)
    {
        vty_warning_out(vty, "invalid input, watchdog mus not greate than %d%s", watchdog_max, VTY_NEWLINE);
        return CMD_WARNING;
    }
    else
    {
        vty_warning_out(vty, "not suggest to close watchdog,it's dangerous%s", VTY_NEWLINE);

        switch (device.type)
        {
            case DEV_TYPE_BOX:
                device.pbox->watchdog = wdi;
                devm_set_watchdog(wdi, 1, 1);
                break;

            case DEV_TYPE_SHELF:
                vty_out(vty, "only box equ can set/close watch%s", VTY_NEWLINE);

//              device.punit[0]->pslot[device.punit[0]->slot_main_board - 1]->watchdog= wdi;
//              devm_set_watchdog(wdi,1,device.punit[0]->slot_main_board);
//              break;
            default:
                break;
        }
    }

    return CMD_SUCCESS;
}


DEFUN(device_heart_beat_interval,
      device_heart_beat_interval_cmd,
      "device heart-beat  interval <0-255>",
      "device command\n"
      "heart-beat\n"
      "interval\n"
      "para range from 0 to 255, 0 means no heart beat\n"
     )
{
    uint8_t beat_val = 0;
    uint8_t main_unit = 0;
    uint8_t main_board_slot = 0;

    beat_val = atoi(argv[0]);

    switch (device.type)
    {
        case DEV_TYPE_BOX:
            vty_warning_out(vty, "the box equipment can not set heart-beat%s", VTY_NEWLINE);
            zlog_err("%s[%d]:leave %s,the box equipment can not set heart-beat\n", __FILE__, __LINE__, __func__);
            break;

        case DEV_TYPE_SHELF:
            main_unit = device.main_unit;
            main_board_slot = device.punit[main_unit - 1]->slot_main_board;
            device.punit[main_unit - 1]->heart_beat = beat_val;
            device.punit[main_unit - 1]->pslot[main_board_slot - 1]->heartbeat = beat_val;
            devm_set_heart_beat(beat_val, main_unit, main_board_slot);
            break;

        default:
            break;
    }

    return CMD_SUCCESS;
}


DEFUN(show_slot_rtime,
      show_slot_rtime_cmd,
      "show slot run-time [<1-32>]",
      "show command\n"
      "which slot\n"
      "total run-time\n"
      "slot range from 1 to 32>\n"
     )
{
    uint8_t slot_start = 1;
    uint8_t slot_end = 1;
    uint8_t slot = 1;
    uint8_t unit = device.main_unit;
    time_t current_secs;
    struct devm_slot *p_devm_slot = NULL;
    struct tm *p_tm = NULL;

    switch (device.type)
    {
        case DEV_TYPE_BOX:
            vty_warning_out(vty, "box equipment use 'show device unit' to show run-time%s", VTY_NEWLINE);
            break;

        case DEV_TYPE_SHELF:
            vty_out(vty, "slot   running-time%s", VTY_NEWLINE);

            if (NULL != argv[0])
            {
                slot_start = atoi(argv[0]);
                slot_end = slot_start;
            }
            else
            {
                slot_start = 1;
                slot_end = 32;
            }

            for (slot = slot_start; slot < slot_end + 1; slot++)
            {
                p_devm_slot = devm_get_slot(unit, slot);

                if (NULL != p_devm_slot)
                {
                    vty_out(vty, "%-7d", slot);

                    devm_comm_get_running_time(1, slot, MODULE_ID_DEVM, &current_secs);

                    if (current_secs < p_devm_slot->run_time)
                    {
                        vty_out(vty, "%s%s", "error", VTY_NEWLINE);
                    }
                    else
                    {
                        current_secs -= p_devm_slot->run_time;
                        vty_out(vty, "%dD %02u:%02u:%02u%s", current_secs/86400, (current_secs%86400)/3600, (current_secs%3600)/60, current_secs%60, VTY_NEWLINE);
                    }
                }
                else if (slot_start == slot_end)
                {
                    zlog_err("%s[%d]:leave %s,slot %d not exist\n", __FILE__, __LINE__, __func__, slot);
                    vty_out(vty, "%-9dnot exist %s", slot, VTY_NEWLINE);
                    return CMD_WARNING;
                }
            }

            break;

        default:
            break;
    }

    return CMD_SUCCESS;
}


DEFUN(show_slot_port,
      show_slot_port_cmd,
      "show slot port [<1-32>]",
      "show command\n"
      "which slot\n"
      "which port\n"
      "slot range from 1 to 32>\n"
     )
{
    uint8_t slot_start = 1;
    uint8_t slot_end = 1;
    uint8_t slot = 1;
    uint8_t unit = device.main_unit;
    uint8_t portid = 0;
    uint8_t flag = 0;
    struct devm_port *p_devm_port = NULL;
    struct devm_slot *p_devm_slot = NULL;
    struct listnode *p_listnode = NULL;

    switch (device.type)
    {
        case DEV_TYPE_BOX:
            vty_warning_out(vty, "box equipment use 'show device unit' to show port info%s", VTY_NEWLINE);
            break;

        case DEV_TYPE_SHELF:
            if (NULL != argv[0])
            {
                slot_start = atoi(argv[0]);
                slot_end = slot_start;
            }
            else
            {
                slot_start = 1;
                slot_end = 32;
            }

            vty_out(vty, "slot   port   port_type   medium   mac%s", VTY_NEWLINE);

            for (slot = slot_start; slot < slot_end + 1; slot++)
            {
                flag = 0;

                p_devm_slot = (struct devm_slot *)devm_get_slot(unit, slot);

                if (NULL != p_devm_slot)
                {
                    vty_out(vty, "%-7d", slot);

                    portid = 1;

                    for (ALL_LIST_ELEMENTS_RO(&(p_devm_slot->port_list), p_listnode, p_devm_port))
                    {
                        if (p_devm_port->hide != 1)
                        {
                            if (0 != flag)
                            {
                                vty_out(vty, "%-7s", " ");
                            }

                            vty_out(vty, "%-7d%-12s%-9s", p_devm_port->port, a_port_type[p_devm_port->port_type], a_port_fiber[p_devm_port->fiber]);
                            vty_out(vty, "%02x:%02x:%02x:%02x:%02x:%02x%s", p_devm_port->mac[0], p_devm_port->mac[1], p_devm_port->mac[2]
                                    , p_devm_port->mac[3], p_devm_port->mac[4], p_devm_port->mac[5], VTY_NEWLINE);
                            flag++;
                        }
                    }

                    vty_out(vty, "%s%s", "-------------------------------------------------------------------", VTY_NEWLINE);
                }
                else if (slot_start == slot_end)
                {
                    zlog_err("%s[%d]:leave %s,slot %d not exist\n", __FILE__, __LINE__, __func__, slot);
                    vty_error_out(vty, "%-9dnot exist%s", slot, VTY_NEWLINE);
                    return CMD_WARNING;
                }
            }

            break;

        default:
            break;
    }

    return CMD_SUCCESS;
}


DEFUN(show_slot_mac,
      show_slot_mac_cmd,
      "show slot mac [<1-32>]",
      "show command\n"
      "which slot\n"
      "which mac\n"
      "slot range from 1 to 32>\n"
     )
{
    uint8_t slot_start = 1;
    uint8_t slot_end = 1;
    uint8_t slot = 1;
    uint8_t unit = device.main_unit;
    struct devm_slot *p_devm_slot = NULL;
    uchar *p_mac = NULL;

    switch (device.type)
    {
        case DEV_TYPE_BOX:
            vty_warning_out(vty, "box equipment use 'show device unit' to show mac info%s", VTY_NEWLINE);
            break;

        case DEV_TYPE_SHELF:
            if (NULL != argv[0])
            {
                slot_start = atoi(argv[0]);
                slot_end = slot_start;
            }
            else
            {
                slot_start = 1;
                slot_end = 32;
            }

            vty_out(vty, "slot   mac%s", VTY_NEWLINE);

            for (slot = slot_start; slot < slot_end + 1; slot++)
            {
                p_devm_slot = (struct devm_slot *)devm_get_slot(unit, slot);

                if (NULL != p_devm_slot)
                {
                    p_mac = p_devm_slot->mac;
                    vty_out(vty, "%-7d%02x:%02x:%02x:%02x:%02x:%02x%s", slot, p_mac[0]
                            , p_mac[1], p_mac[2], p_mac[3], p_mac[4], p_mac[5], VTY_NEWLINE);
                }
                else if (slot_start == slot_end)
                {
                    zlog_err("%s[%d]:leave %s,slot %d not exist\n", __FILE__, __LINE__, __func__, slot);
                    vty_error_out(vty, "%-9d%s%s", slot, "not exist", VTY_NEWLINE);
                    return CMD_WARNING;
                }
            }

            break;

        default:
            break;
    }

    return CMD_SUCCESS;
}


DEFUN(show_slot_version,
      show_slot_version_cmd,
      "show slot version [<1-32>]",
      "show command\n"
      "which slot\n"
      "version of slot\n"
      "slot range from 1 to 32>\n"
     )
{
    uint8_t slot_start = 1;
    uint8_t slot_end = 1;
    uint8_t slot = 1;
    uint8_t unit = device.main_unit;
    uint8_t fpga_ver = 0;
    unsigned char t_hw_ver[64] = {0};
    struct devm_slot *p_devm_slot = NULL;

    switch (device.type)
    {
        case DEV_TYPE_BOX:
            vty_warning_out(vty, "box equipment use 'show device unit' to show version info%s", VTY_NEWLINE);
            break;

        case DEV_TYPE_SHELF:
            if (NULL != argv[0])
            {
                slot_start = atoi(argv[0]);
                slot_end = slot_start;
            }
            else
            {
                slot_start = 1;
                slot_end = 32;
            }

            vty_out(vty, "slot   fpga_ver   sys_ver%s", VTY_NEWLINE);

            for (slot = slot_start; slot < slot_end + 1; slot++)
            {
                p_devm_slot = (struct devm_slot *)devm_get_slot(unit, slot);

                if (NULL != p_devm_slot)
                {
                    memcpy(t_hw_ver, p_devm_slot->hw.hw_ver, sizeof(p_devm_slot->hw.hw_ver));
                    fpga_ver = p_devm_slot->hw.fpga_ver;

                    vty_out(vty, "%-7d", slot);

                    if (0 == fpga_ver)
                    {
                        vty_out(vty, "%-11s", "-");
                    }
                    else
                    {
                        vty_out(vty, "%x.%-2x%-7s", fpga_ver / 16, fpga_ver % 16, " ");
                    }

                    if ('\0' == sys_version[slot - 1])
                    {
                        vty_out(vty, "%-8s%s", "-", VTY_NEWLINE);
                    }
                    else
                    {
                        vty_out(vty, "%-12s%s", sys_version[slot - 1], VTY_NEWLINE);
                    }

                }
                else if (slot_start == slot_end)
                {
                    zlog_err("%s[%d]:leave %s,slot %d not exist\n", __FILE__, __LINE__, __func__, slot);
                    vty_error_out(vty, "%-9d%s%s", slot, "not exist", VTY_NEWLINE);
                    return CMD_WARNING;
                }
            }

            break;

        default:
            break;
    }

    return CMD_SUCCESS;
}


DEFUN(show_slot_type,
      show_slot_type_cmd,
      "show slot type [<1-32>]",
      "show command\n"
      "which slot\n"
      "type of slot\n"
      "slot range from 1 to 32>\n"
     )
{
    uint8_t slot_start = 1;
    uint8_t slot_end = 1;
    uint8_t slot = 1;
    uint8_t unit = device.main_unit;
    struct devm_slot *p_devm_slot = NULL;

    switch (device.type)
    {
        case DEV_TYPE_BOX:
            vty_warning_out(vty, "box equipment can not do show slot type command%s", VTY_NEWLINE);
            break;

        case DEV_TYPE_SHELF:
            if (NULL != argv[0])
            {
                slot_start = atoi(argv[0]);
                slot_end = slot_start;
            }
            else
            {
                slot_start = 1;
                slot_end = 32;
            }

            vty_out(vty, "slot   type%s", VTY_NEWLINE);

            for (slot = slot_start; slot < slot_end + 1; slot++)
            {
                p_devm_slot = (struct devm_slot *)devm_get_slot(unit, slot);

                if (NULL != p_devm_slot)
                {
                    vty_out(vty, "%-7d%s%s", slot, a_slot_type[p_devm_slot->type], VTY_NEWLINE);
                }
                else if (slot_start == slot_end)
                {
                    zlog_err("%s[%d]:leave %s,slot %d  not exist\n", __FILE__, __LINE__, __func__, slot);
                    vty_error_out(vty, "%-9d%s%s", slot, "not exist", VTY_NEWLINE);
                    return CMD_WARNING;
                }
            }

            break;

        default:
            break;
    }

    return CMD_SUCCESS;
}


#if 1
#endif


DEFUN(reboot_slot_all,
      reboot_slot_all_cmd,
      "reboot slot all",
      "reboot command\n"
      "which slot\n"
      "all slot\n"
     )
{
    switch (device.type)
    {
        case DEV_TYPE_BOX:
            devm_reboot_device(device.type, device.main_unit, 0);
            break;

        case DEV_TYPE_SHELF:
            if (device.pmyunit->myslot != device.pmyunit->slot_main_board)
            {
                vty_warning_out(vty, "only mainboard can reboot slot all%s", VTY_NEWLINE);
            }
            else
            {
                devm_reboot_device(device.type, device.main_unit, 0);
            }

            break;

        default:
            break;
    }

    return CMD_SUCCESS;
}


DEFUN(reboot_slot,
      reboot_slot_cmd,
      "reboot slot <1-32>",
      "reboot command\n"
      "which slot\n"
      "slot range from 1 to 32\n"
     )
{
    uint8_t slot = 1;

    slot = atoi(argv[0]);

    switch (device.type)
    {
        case DEV_TYPE_BOX:
//          devm_reboot_device(device.type,device.main_unit,1);
            vty_warning_out(vty, "box equ not support to do this%s", VTY_NEWLINE);
            break;

        case DEV_TYPE_SHELF:
            if (SLOT_TYPE_BOARD_MAIN == device.slot_type)
            {
                if ((NULL != device.pmyunit->pslot[slot - 1])
                        && (device.pmyunit->pslot[slot - 1]->type == SLOT_TYPE_BOARD_MAIN
                            || device.pmyunit->pslot[slot - 1]->type == SLOT_TYPE_BOARD_SLAVE))
                {
                    devm_reboot_device(device.type, device.main_unit, slot);
                }
                else
                {
                    devm_send_set_slot_power_up_down(slot, 1);
                    usleep(30000);
                    devm_send_set_slot_power_up_down(slot, 0);
                }
            }
            else if (SLOT_TYPE_BOARD_SLAVE == device.slot_type)
            {
                if (NULL != device.pmyunit->pslot[slot - 1]
                        && device.pmyunit->pslot[slot - 1]->type == SLOT_TYPE_BOARD_MAIN)
                {
                    vty_warning_out(vty, "slave slot can not reboot main slot%s", VTY_NEWLINE);
                }
                else if (NULL != device.pmyunit->pslot[slot - 1]
                         && device.pmyunit->pslot[slot - 1]->type == SLOT_TYPE_BOARD_SLAVE)
                {
                    devm_reboot_device(device.type, device.main_unit, slot);
                }
                else
                {
                    vty_warning_out(vty, "slave board not support to reboot card%s", VTY_NEWLINE);
                }
            }
            else
            {
                vty_warning_out(vty, "slot type error, only main or slave support reboot cmd%s", VTY_NEWLINE);
            }

            break;

        default:
            break;
    }

    return CMD_SUCCESS;
}


//DEFUN(reset_master,
//  reset_master_cmd,
//  "reset master",
//  "reset command\n"
//  "master\n"
//  )
//{
//  unsigned int t_slot = 0;
//
//  switch(device.type)
//  {
//      case DEV_TYPE_BOX:
//          vty_out (vty, "error: box equ do not support this cmd%s", VTY_NEWLINE);
//          break;
//      case DEV_TYPE_SHELF:
//          if(device.pmyunit->myslot == device.pmyunit->slot_main_board)
//          {
//              if(!reset_master_count)
//              {
//                  reset_master_slot_status[0] = device.pmyunit->slot_main_board;
//                  reset_master_slot_status[1] = device.pmyunit->slot_slave_board;
//              }
//              reset_master_count++;
//              if(reset_master_count)
//              {
//                  t_slot = reset_master_slot_status[0];
//                  reset_master_slot_status[0] = reset_master_slot_status[1];
//                  reset_master_slot_status[1] = t_slot;
//              }
//              devm_reset_master(device.type,device.main_unit);
//          }
//          else
//          {
//              vty_out (vty, "error: only master board can reset master%s", VTY_NEWLINE);
//          }
//      default:
//          break;
//  }
//}



DEFUN(setmx_m2s,
      setmx_m2s_cmd,
      "setmx m2s",
      "shelf equ set its mainboard status\n"
      "switch slave to master\n"
     )
{
    switch (device.type)
    {
        case DEV_TYPE_BOX:
            vty_warning_out(vty, "box equipment not support to do this%s", VTY_NEWLINE);
            break;

        case DEV_TYPE_SHELF:
            if (device.pmyunit->myslot != device.pmyunit->slot_main_board)
            {
                vty_warning_out(vty, "only master board can setmx m2s%s", VTY_NEWLINE);
            }
            else
            {
                devm_setmx_m2s(device.type, device.main_unit);
            }

            break;

        default:
            break;
    }

    return CMD_SUCCESS;
}


DEFUN(reset_slot,
      reset_slot_cmd,
      "reset slot <1-32>",
      "reset card\n"
      "which slot to execute cmd\n"
      "slot range is  from 1 to 32\n"
     )
{
    uint8_t slot = 1;
    uint8_t unit = device.main_unit;

    slot = atoi(argv[0]);

    switch (device.type)
    {
        case DEV_TYPE_BOX:
            vty_warning_out(vty, "box equ support reset itself%s", VTY_NEWLINE);
            break;

        case DEV_TYPE_SHELF:
            if (NULL == device.punit[unit - 1])
            {
                vty_error_out(vty, "unit not exist, failed to reset%s", VTY_NEWLINE);
            }
            else if (slot == device.punit[unit - 1]->slot_main_board
                     || slot == device.punit[unit - 1]->slot_slave_board)
            {
                vty_error_out(vty, "invalid input slot(%d), mainboard(%d) or slaveboard(%d)  not support to execute reset cmd%s"
                              , slot, device.punit[unit - 1]->slot_main_board, device.punit[unit - 1]->slot_slave_board, VTY_NEWLINE);
            }
            else
            {
                if (SLOT_TYPE_BOARD_MAIN == device.slot_type)
                {
                    if (NULL != device.punit[unit - 1]->pslot[slot - 1]
                            && (SLOT_TYPE_BOARD_MAIN == device.punit[unit - 1]->pslot[slot - 1]->type
                                || SLOT_TYPE_BOARD_SLAVE == device.punit[unit - 1]->pslot[slot - 1]->type))
                    {
                        vty_error_out(vty, "mainboard or slaveboard  not support to execute reset cmd%s", VTY_NEWLINE);
                    }
                    else
                    {
                        devm_reset_device(device.type, unit, slot);
                    }
                }
                else if (SLOT_TYPE_BOARD_SLAVE == device.slot_type)
                {
                    vty_error_out(vty, "invalid input, only not main board can reset card%s", VTY_NEWLINE);
                }
                else
                {
                    vty_error_out(vty, "board type error(type = %d)%s", device.pmyunit->pslot[device.pmyunit->myslot - 1]->type, VTY_NEWLINE);
                }
            }

            break;

        default:
            break;
    }

    return CMD_SUCCESS;
}


#if 1
#endif


DEFUN(show_dev_slot_all_info,
      show_dev_slot_all_info_cmd,
      "show slot all",
      "show  command\n"
      "which slot\n"
      "all slot\n"
     )
{
    uint8_t slot = 0;
    uint8_t unit = device.main_unit;
    time_t current_secs;
    struct devm_slot *p_devm_slot = NULL;
    struct tm *p_tm = NULL;

    switch (device.type)
    {
        case DEV_TYPE_BOX:
            vty_warning_out(vty, "box equipment can not do show slot command%s", VTY_NEWLINE);
            break;

        case DEV_TYPE_SHELF:
            vty_out(vty, "slot   type   status   running_time     name%s", VTY_NEWLINE);

            for (slot = 1; slot < SLOT_NUM_MAX + 1; slot++)
            {
                p_devm_slot  = (struct devm_slot *)devm_get_slot(unit, slot);

                if (NULL != p_devm_slot)
                {
                    vty_out(vty, "%-7d", slot);
                    vty_out(vty, "%-7s", a_slot_type[p_devm_slot->type]);
                    vty_out(vty, "%-9s", a_slot_status[p_devm_slot->status]);

                    devm_comm_get_running_time(1, slot, MODULE_ID_DEVM, &current_secs);

                    if (current_secs < p_devm_slot->run_time)
                    {
                        vty_out(vty, "%s%s", "error", VTY_NEWLINE);
                    }
                    else
                    {
                        current_secs -= (time_t)p_devm_slot->run_time;
                        vty_out(vty, "%dD %02u:%02u:%02u", current_secs/86400, (current_secs%86400)/3600, (current_secs%3600)/60, current_secs%60);
                        vty_out(vty, "      %s%s", p_devm_slot->name, VTY_NEWLINE);                        
                    }
                }
            }

            break;

        default:
            break;
    }

    return CMD_SUCCESS;
}


DEFUN(show_dev_slot_info,
      show_dev_slot_info_cmd,
      "show slot <1-32>",
      "show  command\n"
      "which slot\n"
      "slot range is  from 1 to 32\n"
     )
{
    uint8_t slot = 1;
    uint8_t unit = device.main_unit;
    time_t current_secs;
    struct devm_slot *p_devm_slot = NULL;
    struct devm_port *p_devm_port = NULL;
    struct listnode *p_listnode = NULL;
    struct tm *p_tm = NULL;

    switch (device.type)
    {
        case DEV_TYPE_BOX:
            vty_warning_out(vty, "box equipment can not do show slot command%s", VTY_NEWLINE);
            break;

        case DEV_TYPE_SHELF:
            slot = atoi(argv[0]);
            p_devm_slot = devm_get_slot(unit, slot);

            if (NULL != p_devm_slot)
            {
                devm_comm_get_running_time(unit, slot, MODULE_ID_DEVM, &current_secs);

                vty_out(vty, "slot %d:%s", slot, VTY_NEWLINE);
                vty_out(vty, "   %-18s: %s %s", "type", a_slot_type[p_devm_slot->type], VTY_NEWLINE);
                vty_out(vty, "   %-18s: %s %s", "name", p_devm_slot->name, VTY_NEWLINE);

                if (p_devm_slot->slot == device.punit[device.main_unit - 1]->slot_main_board
                        || p_devm_slot->slot == device.punit[device.main_unit - 1]->slot_slave_board)
                {
//                  vty_out (vty, "   %-18s: %s %s", "dev_name",p_devm_slot->dev_name, VTY_NEWLINE);
                    vty_out(vty, "   %-18s: %d %s", "watchdog", p_devm_slot->watchdog, VTY_NEWLINE);
//                  vty_out (vty, "   %-18s: %d %s", "card_num",p_devm_slot->card_num, VTY_NEWLINE);
                    vty_out(vty, "   %-18s: %d %s", "heartbeat", p_devm_slot->heartbeat, VTY_NEWLINE);
                    vty_out(vty, "   %-18s: %d %s", "temprature(deg C)", p_devm_slot->temp.temprature, VTY_NEWLINE);
//                  vty_out (vty, "   %-18s: %02x:%02x:%02x:%02x:%02x:%02x %s", "mac",p_devm_slot->mac[0]
//                      ,p_devm_slot->mac[1],p_devm_slot->mac[2],p_devm_slot->mac[3],p_devm_slot->mac[4],p_devm_slot->mac[5], VTY_NEWLINE);
                }

                vty_out(vty, "   %-18s: %#x %s", "id", p_devm_slot->id, VTY_NEWLINE);
                vty_out(vty, "   %-18s: %d %s", "unit", p_devm_slot->unit, VTY_NEWLINE);
                vty_out(vty, "   %-18s: %d %s", "slot", p_devm_slot->slot, VTY_NEWLINE);
                vty_out(vty, "   %-18s: %d %s", "port_num", p_devm_slot->port_num, VTY_NEWLINE);

                if (current_secs < p_devm_slot->run_time)
                {
                    vty_out(vty, "   %-18s : %s%s", "running_time", "error", VTY_NEWLINE);
                }
                else
                {
                    current_secs -= (time_t)p_devm_slot->run_time;
                    vty_out(vty, "   %-18s: %dD %02u:%02u:%02u%s", "running_time", current_secs/86400, (current_secs%86400)/3600, (current_secs%3600)/60, current_secs%60, VTY_NEWLINE);                    
                }

                /*硬件信息*/
                if (p_devm_slot->slot == device.punit[unit - 1]->slot_main_board
                        || p_devm_slot->slot == device.punit[unit - 1]->slot_slave_board)
                {
                    vty_out(vty, "   %-18s: %s%s", "cpu", a_cpu_type[p_devm_slot->hw.cpu_type], VTY_NEWLINE);
                }

                vty_out(vty, "   %-18s: %s%s", "mem", a_mem_size[p_devm_slot->hw.mem_type], VTY_NEWLINE);
                vty_out(vty, "   %-18s: %s%s", "flash", a_flash_size[p_devm_slot->hw.flash_type], VTY_NEWLINE);

                if (0 == p_devm_slot->hw.fpga_ver)
                {
                    vty_out(vty, "   %-18s: %s%s", "fpga_ver", "-", VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, "   %-18s: %x.%x%s", "fpga_ver", (p_devm_slot->hw.fpga_ver) / 16, (p_devm_slot->hw.fpga_ver) % 16, VTY_NEWLINE);
                }

                vty_out(vty, "   %-18s: %s%s", "hw_ver", p_devm_slot->hw.hw_ver, VTY_NEWLINE);

                if ('\0' == sys_version[slot - 1])
                {
                    vty_out(vty, "   %-18s: %s%s", "sys_ver", "-", VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, "   %-18s: %s%s", "sys_ver", sys_version[slot - 1], VTY_NEWLINE);
                }

                if (0 == p_devm_slot->hw.sd_type)
                {
                    vty_out(vty, "   %-18s: %s%s", "sd", "-", VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, "   %-18s: %s%s", "sd", a_sd_size[p_devm_slot->hw.sd_type], VTY_NEWLINE);
                }

                /*端口信息*/
                vty_out(vty, "%s", VTY_NEWLINE);
                vty_out(vty, "   port   port_type   medium   mac%s", VTY_NEWLINE);

                if (listhead(&p_devm_slot->port_list))
                {
                    for (ALL_LIST_ELEMENTS_RO(&p_devm_slot->port_list, p_listnode, p_devm_port))
                    {
                        if (p_devm_port->hide != 1)
                        {
                            vty_out(vty, "   %-7d", p_devm_port->port);
                            vty_out(vty, "%-12s", a_port_type[p_devm_port->port_type]);
                            vty_out(vty, "%-9s", a_port_fiber[p_devm_port->fiber]);

                            if (0 == p_devm_port->mac[0] && 0 == p_devm_port->mac[1] && 0 == p_devm_port->mac[2] && 0 == p_devm_port->mac[3] && 0 == p_devm_port->mac[4] && 0 == p_devm_port->mac[5])
                            {
                                vty_out(vty, "%s:%s:%s:%s:%s:%s%s", "--", "--", "--", "--", "--", "--", VTY_NEWLINE);
                            }
                            else
                                vty_out(vty, "%02x:%02x:%02x:%02x:%02x:%02x%s", p_devm_port->mac[0], p_devm_port->mac[1], p_devm_port->mac[2]
                                        , p_devm_port->mac[3], p_devm_port->mac[4], p_devm_port->mac[5], VTY_NEWLINE);
                        }
                    }
                }

                vty_out(vty, "-------------------------------------------------------------------%s", VTY_NEWLINE);
            }
            else
            {
                zlog_err("%s[%d]:leave %s,slot %d not exist\n", __FILE__, __LINE__, __func__, slot);
                vty_error_out(vty, "slot %d not exist%s", slot, VTY_NEWLINE);
                return CMD_WARNING;
            }

            break;

        default:
            break;
    }

    return CMD_SUCCESS;
}

extern devdigtlabl digtLabl;

DEFUN(show_dev_backplane,
      show_dev_backplane_cmd,
      "show device backplane",
      "show  command\n"
      "device info\n"
      "backplane info\n"
     )
{
    uint16_t version = 0;
    struct devm_unit *p_devm_unit = NULL;

    switch (device.type)
    {
        case DEV_TYPE_BOX:
            vty_warning_out(vty, "box equipment can not do show device backplane command%s", VTY_NEWLINE);
            break;

        case DEV_TYPE_SHELF:
            p_devm_unit = devm_get_unit(device.main_unit);

            if (NULL != p_devm_unit)
            {
                version = p_devm_unit->bplane.version;

                vty_out(vty, "backplane:%s", VTY_NEWLINE);

                if (('a' <= p_devm_unit->bplane.oem_name[0] && p_devm_unit->bplane.oem_name[0] <= 'z')
                        || ('A' <= p_devm_unit->bplane.oem_name[0] && p_devm_unit->bplane.oem_name[0] <= 'Z'))
                {
                    vty_out(vty, "   %-15s: %s%s", "dev_name", p_devm_unit->bplane.oem_name, VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, "   %-15s: %s%s", "dev_name", p_devm_unit->bplane.dev_name, VTY_NEWLINE);
                }

                vty_out(vty, "   %-15s: %s %s", "manufacture", p_devm_unit->bplane.manufacture, VTY_NEWLINE);
                vty_out(vty, "   %-15s: %#x %s", "id", p_devm_unit->bplane.id, VTY_NEWLINE);
                vty_out(vty, "   %-15s: %d %s", "mac_num", p_devm_unit->bplane.mac_num, VTY_NEWLINE);
                vty_out(vty, "   %-15s: %02x:%02x:%02x:%02x:%02x:%02x%s", "mac", p_devm_unit->bplane.mac[0]
                        , p_devm_unit->bplane.mac[1], p_devm_unit->bplane.mac[2], p_devm_unit->bplane.mac[3], p_devm_unit->bplane.mac[4], p_devm_unit->bplane.mac[5],  VTY_NEWLINE);

                vty_out(vty, "   %-15s: %s%s", "bplane_ver", a_backplane_ver[p_devm_unit->bplane.version], VTY_NEWLINE);
                return CMD_SUCCESS;
            }
            else
            {
                zlog_err("%s[%d]:leave %s,unit %d no exist\n", __FILE__, __LINE__, __func__, device.main_unit);
                vty_error_out(vty, "unit %d no exist%s", device.main_unit, VTY_NEWLINE);
                return CMD_WARNING;
            }

            break;

        default:
            break;
    }

    return CMD_SUCCESS;
}


DEFUN(show_dev_unit,
      show_dev_unit_cmd,
      "show device unit",
      "show  command\n"
      "device info\n"
      "unit of the equipment\n"
     )
{
    int i = 0;
    uint8_t unit = device.main_unit;
    uint16_t version = 0;
    uint16_t t_fan_speed = 0;
    time_t current_secs;
    struct devm_unit *p_devm_unit = NULL;
    struct devm_slot *p_devm_slot = NULL;
    struct devm_box *p_devm_box = NULL;
    struct devm_port *p_devm_port = NULL;
    struct listnode *p_listnode = NULL;
    struct tm *p_tm = NULL;

    switch (device.type)
    {
        case DEV_TYPE_BOX:
            p_devm_box = device.pbox;

            if (NULL != p_devm_box)
            {
                vty_out(vty, "box:%s", VTY_NEWLINE);

                devm_comm_get_running_time(1, 1, MODULE_ID_DEVM, &current_secs);

                if (current_secs < p_devm_box->run_time)
                {
                    vty_out(vty, "   %-18s : %s%s", "start_time", "error", VTY_NEWLINE);
                }
                else
                {
                    current_secs -= p_devm_box->run_time;
                    
                    vty_out(vty, "   %-18s: %dD %02u:%02u:%02u%s", "running_time", current_secs/86400, (current_secs%86400)/3600, (current_secs%3600)/60, current_secs%60, VTY_NEWLINE);
                }

                if (('a' <= p_devm_box->oem_name[0] && p_devm_box->oem_name[0] <= 'z')
                        || ('A' <= p_devm_box->oem_name[0] && p_devm_box->oem_name[0] <= 'Z'))
                {
                    vty_out(vty, "   %-18s: %s%s", "dev_name", p_devm_box->oem_name, VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, "   %-18s: %s%s", "dev_name", p_devm_box->dev_name, VTY_NEWLINE);
                }

                vty_out(vty, "   %-18s: %s %s", "bootstate", a_ha_bootstate[myboard_dev_start_status], VTY_NEWLINE);

                vty_out(vty, "   %-18s: %s%s", "manufacture", p_devm_box->manufacture, VTY_NEWLINE);
                vty_out(vty, "   %-18s: %#x%s", "id", p_devm_box->id, VTY_NEWLINE);

                if (0 == p_devm_box->mac[0] && 0 == p_devm_box->mac[1] && 0 == p_devm_box->mac[2] && 0 == p_devm_box->mac[3] && 0 == p_devm_box->mac[4] && 0 == p_devm_box->mac[5])
                {
                    vty_out(vty, "   %-18s: %s:%s:%s:%s:%s:%s%s", "mac", "--", "--", "--", "--", "--", "--", VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, "   %-18s: %02x:%02x:%02x:%02x:%02x:%02x%s", "mac", p_devm_box->mac[0], p_devm_box->mac[1], p_devm_box->mac[2], p_devm_box->mac[3], p_devm_box->mac[4], p_devm_box->mac[5], VTY_NEWLINE);
                }

                vty_out(vty, "   %-18s: %d%s", "mac_num", p_devm_box->mac_num, VTY_NEWLINE);
                vty_out(vty, "   %-18s: %d%s", "port_num", p_devm_box->port_num, VTY_NEWLINE);

                if (0 == p_devm_box->watchdog)
                {
                    vty_out(vty, "   %-18s: %s%s", "watchdog", "-", VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, "   %-18s: %d%s", "watchdog", p_devm_box->watchdog, VTY_NEWLINE);
                }

                /*硬件信息*/
                vty_out(vty, "   %-18s: %s%s", "cpu", a_cpu_type[p_devm_box->hw.cpu_type], VTY_NEWLINE);
                vty_out(vty, "   %-18s: %s%s", "mem", a_mem_size[p_devm_box->hw.mem_type], VTY_NEWLINE);
                vty_out(vty, "   %-18s: %s%s", "flash", a_flash_size[p_devm_box->hw.flash_type], VTY_NEWLINE);

                if (0 == p_devm_box->hw.fpga_ver)
                {
                    vty_out(vty, "   %-18s: %s%s", "fpga_ver", "-", VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, "   %-18s: %x.%x%s", "fpga_ver", (p_devm_box->hw.fpga_ver) / 16, (p_devm_box->hw.fpga_ver) % 16, VTY_NEWLINE);
                }

                vty_out(vty, "   %-18s: %s%s", "hw_ver", p_devm_box->hw.hw_ver, VTY_NEWLINE);

                if ('\0' == sys_version[device.myslot - 1])
                {
                    vty_out(vty, "   %-18s: %s%s", "sys_ver", "-", VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, "   %-18s: %s%s", "sys_ver", sys_version[device.myslot - 1], VTY_NEWLINE);
                }

                if (0 == p_devm_box->hw.sd_type)
                {
                    vty_out(vty, "   %-18s: %s%s", "sd", "-", VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, "   %-18s: %s%s", "sd", a_sd_size[p_devm_box->hw.sd_type], VTY_NEWLINE);
                }

                vty_out(vty, "   %-18s: %d %s", "temprature(deg C)", p_devm_box->temp.temprature, VTY_NEWLINE);
                vty_out(vty, "%s", VTY_NEWLINE);
                vty_out(vty, "   port   port_type   medium   mac%s", VTY_NEWLINE);

                for (ALL_LIST_ELEMENTS_RO(&p_devm_box->port_list, p_listnode, p_devm_port))
                {
                
                    if (p_devm_port->hide != 1)
                    {
                        vty_out(vty, "   %-7d", p_devm_port->port);
                        vty_out(vty, "%-12s", a_port_type[p_devm_port->port_type]);
                        vty_out(vty, "%-9s", a_port_fiber[p_devm_port->fiber]);

                        if (0 == p_devm_port->mac[0] && 0 == p_devm_port->mac[1] && 0 == p_devm_port->mac[2] && 0 == p_devm_port->mac[3] && 0 == p_devm_port->mac[4] && 0 == p_devm_port->mac[5])
                        {
                            vty_out(vty, "%s:%s:%s:%s:%s:%s%s", "--", "--", "--", "--", "--", "--", VTY_NEWLINE);
                        }
                        else
                            vty_out(vty, "%02x:%02x:%02x:%02x:%02x:%02x%s", p_devm_port->mac[0], p_devm_port->mac[1], p_devm_port->mac[2]
                                    , p_devm_port->mac[3], p_devm_port->mac[4], p_devm_port->mac[5], VTY_NEWLINE);
                    }
                }
            }
            else
            {
                zlog_err("%s[%d]:leave %s,box no exist\n", __FILE__, __LINE__, __func__);
                vty_error_out(vty, "box no exist%s", VTY_NEWLINE);
                return CMD_WARNING;
            }

            break;

        case DEV_TYPE_SHELF:
            p_devm_unit = devm_get_unit(device.main_unit);

            if (NULL != p_devm_unit)
            {
                char dynamic_vol[32] = {0};

                /*unit 自有信息*/
                vty_out(vty, "unit:%s", VTY_NEWLINE);
                vty_out(vty, "   %-17s: %d %s", "slot_max", p_devm_unit->slot_num, VTY_NEWLINE);

//              vty_out (vty, "   %-17s: %d %s", "myslot",p_devm_unit->myslot, VTY_NEWLINE);

                if (!reset_master_count)
                {
                    vty_out(vty, "   %-17s: %d %s", "slot_main_board", p_devm_unit->slot_main_board, VTY_NEWLINE);

                    if (0 == p_devm_unit->slot_slave_board)
                    {
                        vty_out(vty, "   %-17s: %s %s", "slot_slave_board", "-", VTY_NEWLINE);
                    }
                    else
                    {
                        vty_out(vty, "   %-17s: %d %s", "slot_slave_board", p_devm_unit->slot_slave_board, VTY_NEWLINE);
                    }
                }
                else
                {
                    vty_out(vty, "   %-17s: %d %s", "cur", p_devm_unit->myslot, VTY_NEWLINE);
                    vty_out(vty, "   %-17s: %d %s", "slot_main_board", reset_master_slot_status[0], VTY_NEWLINE);

                    if (0 == p_devm_unit->slot_slave_board)
                    {
                        vty_out(vty, "   %-17s: %s %s", "slot_slave_board", "-", VTY_NEWLINE);
                    }
                    else
                    {
                        vty_out(vty, "   %-17s: %d %s", "slot_slave_board", reset_master_slot_status[1], VTY_NEWLINE);
                    }
                }

//              if(0 == p_devm_unit->heart_beat)
//                  vty_out (vty, "   %-17s: %s%s","heart_beat","-", VTY_NEWLINE);
//              else
//                  vty_out (vty, "   %-17s: %d%s","heart_beat",p_devm_unit->heart_beat, VTY_NEWLINE);

//              if(0 == p_devm_unit->watchdog)
//                  vty_out (vty, "   %-17s: %s%s","watchdog","-", VTY_NEWLINE);
//              else
//                  vty_out (vty, "   %-17s: %d%s","watchdog",p_devm_unit->watchdog, VTY_NEWLINE);
                vty_out(vty, "-------------------------------------------------------------------%s", VTY_NEWLINE);


                /*打印背板信息*/
                version = p_devm_unit->bplane.version;

                vty_out(vty, "backplane:%s", VTY_NEWLINE);

                if (('a' <= p_devm_unit->bplane.oem_name[0] && p_devm_unit->bplane.oem_name[0] <= 'z')
                        || ('A' <= p_devm_unit->bplane.oem_name[0] && p_devm_unit->bplane.oem_name[0] <= 'Z'))
                {
                    vty_out(vty, "   %-18s: %s%s", "dev_name", p_devm_unit->bplane.oem_name, VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, "   %-18s: %s%s", "dev_name", p_devm_unit->bplane.dev_name, VTY_NEWLINE);
                }

                vty_out(vty, "   %-18s: %s %s", "manufacture", p_devm_unit->bplane.manufacture, VTY_NEWLINE);
                vty_out(vty, "   %-18s: %#x %s", "id", p_devm_unit->bplane.id, VTY_NEWLINE);

                vty_out(vty, "   %-18s: %d %s", "mac_num", p_devm_unit->bplane.mac_num, VTY_NEWLINE);
                vty_out(vty, "   %-18s: %02x:%02x:%02x:%02x:%02x:%02x%s", "mac", p_devm_unit->bplane.mac[0], p_devm_unit->bplane.mac[1], p_devm_unit->bplane.mac[2]
                        , p_devm_unit->bplane.mac[3], p_devm_unit->bplane.mac[4], p_devm_unit->bplane.mac[5],  VTY_NEWLINE);

                vty_out(vty, "   %-18s: %s%s", "bplane_ver", a_backplane_ver[p_devm_unit->bplane.version], VTY_NEWLINE);

                vty_out(vty, "-------------------------------------------------------------------%s", VTY_NEWLINE);

                /*打印电源信息*/
                vty_out(vty, "power   mode                        input_vol_min(V)   vol_input   input_vol_max(V)   vol_output(V)    %s", VTY_NEWLINE);

                for (i = 1; i < 3; i++)
                {
                    vty_out(vty, "%-8d", i);

                    vty_out(vty, "%-28s", a_power_type[p_devm_unit->power[i - 1].mode]);

                    if ((POWER_TYPE_48v_12V_75W == p_devm_unit->power[i - 1].mode)
                            || (POWER_TYPE_48v_12V_150W == p_devm_unit->power[i - 1].mode)
                            || (POWER_TYPE_PWR01A == p_devm_unit->power[i - 1].mode))
                    {
                        if (0 != p_devm_unit->power[i - 1].voltage_low)
                        {
                            vty_out(vty, "%s", "-");
                            vty_out(vty, "%-18d", p_devm_unit->power[i - 1].voltage_low);
                        }
                        else
                        {
                            vty_out(vty, "%-19s", "-");
                        }

                        if (0 != p_devm_unit->power[i - 1].voltage_input)
                        {
                            vty_out(vty, "%s", "-");
                            sprintf(dynamic_vol, "%d.%03d", p_devm_unit->power[i - 1].voltage_input / 1000, p_devm_unit->power[i - 1].voltage_input % 1000);
                            vty_out(vty, "%-11s", dynamic_vol);
                        }
                        else
                        {
                            vty_out(vty, "%-12s", "-");
                        }

                        if (0 != p_devm_unit->power[i - 1].voltage_high)
                        {
                            vty_out(vty, "%s", "-");
                            vty_out(vty, "%-18d", p_devm_unit->power[i - 1].voltage_high);
                        }
                        else
                        {
                            vty_out(vty, "%-19s", "-");
                        }

                        if (0 != p_devm_unit->power[i - 1].voltage_output)
                        {
                            vty_out(vty, "%-14d%s", p_devm_unit->power[i - 1].voltage_output, VTY_NEWLINE);
                        }
                        else
                        {
                            vty_out(vty, "%-14s%s", "-", VTY_NEWLINE);
                        }
                    }
                    else if ((POWER_TYPE_220v_12V_75W == p_devm_unit->power[i - 1].mode)
                             || (POWER_TYPE_220v_12V_150W == p_devm_unit->power[i - 1].mode)
                             || (POWER_TYPE_PWR02A == p_devm_unit->power[i - 1].mode))
                    {
                        if (0 != p_devm_unit->power[i - 1].voltage_low)
                        {
                            vty_out(vty, "%s", "~");
                            vty_out(vty, "%-18d", p_devm_unit->power[i - 1].voltage_low);
                        }
                        else
                        {
                            vty_out(vty, "%-19s", "-");
                        }

                        if (0 == p_devm_unit->power[i - 1].voltage_input)
                        {
                            vty_out(vty, "%-12s", "-");
                        }
                        else
                        {
                            sprintf(dynamic_vol, "%d.%03d", p_devm_unit->power[i - 1].voltage_input / 1000, p_devm_unit->power[i - 1].voltage_input % 1000);
                            vty_out(vty, "%s", "~");
                            vty_out(vty, "%-12s", dynamic_vol);
                        }

                        if (0 != p_devm_unit->power[i - 1].voltage_high)
                        {
                            vty_out(vty, "%s", "~");
                            vty_out(vty, "%-18d", p_devm_unit->power[i - 1].voltage_high);
                        }
                        else
                        {
                            vty_out(vty, "%-19s", "-");
                        }

                        if (0 == p_devm_unit->power[i - 1].voltage_output)
                        {
                            vty_out(vty, "%-14s%s", "-", VTY_NEWLINE);
                        }
                        else
                        {
                            vty_out(vty, "%-14d%s", p_devm_unit->power[i - 1].voltage_output, VTY_NEWLINE);
                        }
                    }
                    else
                    {
                        vty_out(vty, "%-19s", "-");
                        vty_out(vty, "%-12s", "-");
                        vty_out(vty, "%-19s", "-");
                        vty_out(vty, "%-14s%s", "-", VTY_NEWLINE);
                    }
                }

                vty_out(vty, "-------------------------------------------------------------------%s", VTY_NEWLINE);

                /*打印风扇信息*/
                vty_out(vty, "mode   speed_min(r/min)   speed(r/min)   speed_max(r/min)%s", VTY_NEWLINE);
//              if(0 == p_devm_unit->fan.mode)
                vty_out(vty, "%-7s", "FAN02");
//              else
//                  vty_out (vty, "%-7d",p_devm_unit->fan.mode);

                vty_out(vty, "%-19d", p_devm_unit->fan.speed_min);

                devm_get_fan_speed(&t_fan_speed);

                if (0 == t_fan_speed)
                {
                    vty_out(vty, "%-15s", "-");
                }
                else
                {
                    vty_out(vty, "%-15d", t_fan_speed);
                }

                vty_out(vty, "%-16d%s", p_devm_unit->fan.speed_max, VTY_NEWLINE);
                vty_out(vty, "-------------------------------------------------------------------%s", VTY_NEWLINE);

                /*打印slot信息*/
                for (i = 1; i < SLOT_NUM_MAX + 1; i++)
                {
                    p_devm_slot = devm_get_slot(unit, i);

                    if (NULL != p_devm_slot)
                    {
                        devm_comm_get_running_time(1, i, MODULE_ID_DEVM, &current_secs);

                        vty_out(vty, "slot %d:%s", i, VTY_NEWLINE);
                        vty_out(vty, "   %-18s: %s %s", "type", a_slot_type[p_devm_slot->type], VTY_NEWLINE);
                        vty_out(vty, "   %-18s: %s %s", "name", p_devm_slot->name, VTY_NEWLINE);

                        if (p_devm_slot->slot == device.punit[device.main_unit - 1]->slot_main_board
                                || p_devm_slot->slot == device.punit[device.main_unit - 1]->slot_slave_board)
                        {
//                          vty_out (vty, "   %-18s: %s %s", "dev_name",p_devm_slot->dev_name, VTY_NEWLINE);
                            vty_out(vty, "   %-18s: %d %s", "watchdog", p_devm_slot->watchdog, VTY_NEWLINE);
//                          vty_out (vty, "   %-18s: %d %s", "card_num",p_devm_slot->card_num, VTY_NEWLINE);
                            vty_out(vty, "   %-18s: %d %s", "heartbeat", p_devm_slot->heartbeat, VTY_NEWLINE);
                            vty_out(vty, "   %-18s: %d %s", "temprature(deg C)", p_devm_slot->temp.temprature, VTY_NEWLINE);

//                          vty_out (vty, "   %-18s: %02x:%02x:%02x:%02x:%02x:%02x %s", "mac",p_devm_slot->mac[0]
//                              ,p_devm_slot->mac[1],p_devm_slot->mac[2],p_devm_slot->mac[3],p_devm_slot->mac[4],p_devm_slot->mac[5], VTY_NEWLINE);

                            if (i + 1 == device.myslot)
                            {
                                vty_out(vty, "   %-18s: %s %s", "bootstate", a_ha_bootstate[myboard_dev_start_status], VTY_NEWLINE);
                            }
                            else
                            {
                                vty_out(vty, "   %-18s: %s %s", "bootstate", a_ha_bootstate[otherboard_dev_start_status], VTY_NEWLINE);
                            }
                        }

                        vty_out(vty, "   %-18s: %#x %s", "id", p_devm_slot->id, VTY_NEWLINE);
                        vty_out(vty, "   %-18s: %d %s", "unit", p_devm_slot->unit, VTY_NEWLINE);
                        vty_out(vty, "   %-18s: %d %s", "slot", p_devm_slot->slot, VTY_NEWLINE);
                        vty_out(vty, "   %-18s: %d %s", "port_num", p_devm_slot->port_num, VTY_NEWLINE);

                        if (current_secs < p_devm_slot->run_time)
                        {
                            vty_out(vty, "   %-18s : %s%s", "start_time", "error", VTY_NEWLINE);
                        }
                        else
                        {
                            current_secs -= p_devm_slot->run_time;
                            vty_out(vty, "   %-18s: %dD %02u:%02u:%02u%s", "running_time", current_secs/86400, (current_secs%86400)/3600, (current_secs%3600)/60, current_secs%60, VTY_NEWLINE);                           
                        }


                        /*硬件信息*/
                        if (p_devm_slot->slot == device.punit[device.main_unit - 1]->slot_main_board
                                || p_devm_slot->slot == device.punit[device.main_unit - 1]->slot_slave_board)
                        {
                            vty_out(vty, "   %-18s: %s%s", "cpu", a_cpu_type[p_devm_slot->hw.cpu_type], VTY_NEWLINE);
                        }

                        vty_out(vty, "   %-18s: %s%s", "mem", a_mem_size[p_devm_slot->hw.mem_type ], VTY_NEWLINE);
                        vty_out(vty, "   %-18s: %s%s", "flash", a_flash_size[p_devm_slot->hw.flash_type], VTY_NEWLINE);

                        if (0 == p_devm_slot->hw.fpga_ver)
                        {
                            vty_out(vty, "   %-18s: %s%s", "fpga_ver", "-", VTY_NEWLINE);
                        }
                        else
                        {
                            vty_out(vty, "   %-18s: %x.%x%s", "fpga_ver", (p_devm_slot->hw.fpga_ver) / 16, (p_devm_slot->hw.fpga_ver) % 16, VTY_NEWLINE);
                        }

                        vty_out(vty, "   %-18s: %s%s", "hw_ver", p_devm_slot->hw.hw_ver, VTY_NEWLINE);

                        if ('\0' == sys_version[i - 1])
                        {
                            vty_out(vty, "   %-18s: %s%s", "sys_ver", "-", VTY_NEWLINE);
                        }
                        else
                        {
                            vty_out(vty, "   %-18s: %s%s", "sys_ver", sys_version[i - 1], VTY_NEWLINE);
                        }

                        if (0 == p_devm_slot->hw.sd_type)
                        {
                            vty_out(vty, "   %-18s: %s%s", "sd", "-", VTY_NEWLINE);
                        }
                        else
                        {
                            vty_out(vty, "   %-18s: %s%s", "sd", a_sd_size[p_devm_slot->hw.sd_type], VTY_NEWLINE);
                        }

                        /*端口信息*/
                        vty_out(vty, "%s", VTY_NEWLINE);
                        vty_out(vty, "   port   port_type   medium   mac%s", VTY_NEWLINE);

                        if (listhead(&p_devm_slot->port_list))
                        {
                            for (ALL_LIST_ELEMENTS_RO(&p_devm_slot->port_list, p_listnode, p_devm_port))
                            {
                                if (p_devm_port->hide != 1)
                                {
                                    vty_out(vty, "   %-7d", p_devm_port->port);
                                    vty_out(vty, "%-12s", a_port_type[p_devm_port->port_type]);
                                    vty_out(vty, "%-9s", a_port_fiber[p_devm_port->fiber]);

                                    if (0 == p_devm_port->mac[0] && 0 == p_devm_port->mac[1]
                                            && 0 == p_devm_port->mac[2] && 0 == p_devm_port->mac[3] && 0 == p_devm_port->mac[4] && 0 == p_devm_port->mac[5])
                                    {
                                        vty_out(vty, "%s:%s:%s:%s:%s:%s%s", "--", "--", "--", "--", "--", "--", VTY_NEWLINE);
                                    }
                                    else
                                        vty_out(vty, "%02x:%02x:%02x:%02x:%02x:%02x%s", p_devm_port->mac[0], p_devm_port->mac[1], p_devm_port->mac[2]
                                                , p_devm_port->mac[3], p_devm_port->mac[4], p_devm_port->mac[5], VTY_NEWLINE);
                                }
                            }
                        }

                        vty_out(vty, "-------------------------------------------------------------------%s", VTY_NEWLINE);
                    }
                }

                return CMD_SUCCESS;
            }
            else
            {
                zlog_err("%s[%d]:leave %s,unit %d no exist\n", __FILE__, __LINE__, __func__, device.main_unit);
                vty_error_out(vty, "unit %d no exist%s", device.main_unit, VTY_NEWLINE);
                return CMD_WARNING;
            }

            break;

        default:
            break;
    }

    return CMD_SUCCESS;
}


DEFUN(show_dev_power,
      show_dev_power_cmd,
      "show device power",
      "show  command\n"
      "device info\n"
      "power of the unit\n"
     )
{
    uint8_t i = 0;
    struct devm_unit *p_devm_unit = NULL;

    switch (device.type)
    {
        case DEV_TYPE_BOX:
            vty_warning_out(vty, "box equipment can not do show device power command%s", VTY_NEWLINE);
            break;

        case DEV_TYPE_SHELF:
            p_devm_unit = devm_get_unit(device.main_unit);

            if (NULL != p_devm_unit)
            {
                char dynamic_vol[32] = {0};

                vty_out(vty, "power   mode                        input_vol_min(V)   vol_input   input_vol_max(V)   vol_output(V)    %s", VTY_NEWLINE);

                for (i = 1; i < 3; i++)
                {
                    vty_out(vty, "%-8d", i);

                    vty_out(vty, "%-28s", a_power_type[p_devm_unit->power[i - 1].mode]);

                    if ((POWER_TYPE_48v_12V_75W == p_devm_unit->power[i - 1].mode)
                            || (POWER_TYPE_48v_12V_150W == p_devm_unit->power[i - 1].mode)
                            || (POWER_TYPE_PWR01A == p_devm_unit->power[i - 1].mode))
                    {
                        if (0 != p_devm_unit->power[i - 1].voltage_low)
                        {
                            vty_out(vty, "%s", "-");
                            vty_out(vty, "%-18d", p_devm_unit->power[i - 1].voltage_low);
                        }
                        else
                        {
                            vty_out(vty, "%-19s", "-");
                        }

                        if (0 != p_devm_unit->power[i - 1].voltage_input)
                        {
                            vty_out(vty, "%s", "-");
                            sprintf(dynamic_vol, "%d.%03d", p_devm_unit->power[i - 1].voltage_input / 1000, p_devm_unit->power[i - 1].voltage_input % 1000);
                            vty_out(vty, "%-11s", dynamic_vol);
                        }
                        else
                        {
                            vty_out(vty, "%-12s", "-");
                        }

                        if (0 != p_devm_unit->power[i - 1].voltage_high)
                        {
                            vty_out(vty, "%s", "-");
                            vty_out(vty, "%-18d", p_devm_unit->power[i - 1].voltage_high);
                        }
                        else
                        {
                            vty_out(vty, "%-19s", "-");
                        }

                        if (0 != p_devm_unit->power[i - 1].voltage_output)
                        {
                            vty_out(vty, "%-14d%s", p_devm_unit->power[i - 1].voltage_output, VTY_NEWLINE);
                        }
                        else
                        {
                            vty_out(vty, "%-14s%s", "-", VTY_NEWLINE);
                        }
                    }
                    else if ((POWER_TYPE_220v_12V_75W == p_devm_unit->power[i - 1].mode)
                             || (POWER_TYPE_220v_12V_150W == p_devm_unit->power[i - 1].mode)
                             || (POWER_TYPE_PWR02A == p_devm_unit->power[i - 1].mode))
                    {
                        if (0 != p_devm_unit->power[i - 1].voltage_low)
                        {
                            vty_out(vty, "%s", "~");
                            vty_out(vty, "%-18d", p_devm_unit->power[i - 1].voltage_low);
                        }
                        else
                        {
                            vty_out(vty, "%-19s", "-");
                        }

                        if (0 == p_devm_unit->power[i - 1].voltage_input)
                        {
                            vty_out(vty, "%-12s", "-");
                        }
                        else
                        {
                            sprintf(dynamic_vol, "%d.%03d", p_devm_unit->power[i - 1].voltage_input / 1000, p_devm_unit->power[i - 1].voltage_input % 1000);
                            vty_out(vty, "%s", "~");
                            vty_out(vty, "%-12s", dynamic_vol);
                        }

                        if (0 != p_devm_unit->power[i - 1].voltage_high)
                        {
                            vty_out(vty, "%s", "~");
                            vty_out(vty, "%-18d", p_devm_unit->power[i - 1].voltage_high);
                        }
                        else
                        {
                            vty_out(vty, "%-19s", "-");
                        }

                        if (0 == p_devm_unit->power[i - 1].voltage_output)
                        {
                            vty_out(vty, "%-14s%s", "-", VTY_NEWLINE);
                        }
                        else
                        {
                            vty_out(vty, "%-14d%s", p_devm_unit->power[i - 1].voltage_output, VTY_NEWLINE);
                        }
                    }
                    else
                    {
                        vty_out(vty, "%-19s", "-");
                        vty_out(vty, "%-12s", "-");
                        vty_out(vty, "%-19s", "-");
                        vty_out(vty, "%-14s%s", "-", VTY_NEWLINE);
                    }
                }

                return CMD_SUCCESS;
            }
            else
            {
                zlog_err("%s[%d]:leave %s,unit %d no exist\n", __FILE__, __LINE__, __func__, device.main_unit);
                vty_error_out(vty, "unit %d no exist%s", device.main_unit, VTY_NEWLINE);
                return CMD_WARNING;
            }

            break;

        default:
            break;
    }

    return CMD_SUCCESS;
}


DEFUN(show_dev_fan,
      show_dev_fan_cmd,
      "show device fan",
      "show  command\n"
      "device info\n"
      "fan info of the unit\n"
     )
{
    struct devm_unit *p_devm_unit = NULL;
    unsigned short t_fan_speed = 0;

    switch (device.type)
    {
        case DEV_TYPE_BOX:
            vty_warning_out(vty, "box equipment can not do show device fan command%s", VTY_NEWLINE);
            break;

        case DEV_TYPE_SHELF:
            p_devm_unit = devm_get_unit(device.main_unit);

            if (NULL != p_devm_unit)
            {
                vty_out(vty, "mode   speed_min(r/min)   speed(r/min)   speed_max(r/min)%s", VTY_NEWLINE);
//              if(0 == p_devm_unit->fan.mode)
                vty_out(vty, "%-7s", "FAN02");
//              else
//                  vty_out (vty, "%-7d",p_devm_unit->fan.mode);


                vty_out(vty, "%-19d", p_devm_unit->fan.speed_min);
                devm_get_fan_speed(&t_fan_speed);

                if (0 == t_fan_speed)
                {
                    vty_out(vty, "%-15s", "-");
                }
                else
                {
                    vty_out(vty, "%-15d", t_fan_speed);
                }

                vty_out(vty, "%-16d%s", p_devm_unit->fan.speed_max, VTY_NEWLINE);

                return CMD_SUCCESS;
            }
            else
            {
                zlog_err("%s[%d]:leave %s,unit %d no exist\n", __FILE__, __LINE__, __func__, device.main_unit);
                vty_error_out(vty, "unit %d no exist%s", device.main_unit, VTY_NEWLINE);
                return CMD_WARNING;
            }

            break;

        default:
            break;
    }

    return CMD_SUCCESS;
}


DEFUN(show_dev_temp,
      show_dev_temp_cmd,
      "show device temperature {slot <1-32>}",
      "show  command\n"
      "device info\n"
      "temperature info\n"
      "which slot\n"
      "slot range from 1 to 32\n"
     )
{
    uint8_t i = 0;
    uint8_t slot_start = 1;
    uint8_t slot_end = 0;
    struct devm_slot *p_devm_slot = NULL;

    if (NULL != argv[0])
    {
        slot_start = atoi(argv[0]);
        slot_end = slot_start;
    }
    else
    {
        slot_start = 1;
        slot_end = 32;
    }

    switch (device.type)
    {
        case DEV_TYPE_BOX:
            if (device.pbox == NULL)
            {
                vty_warning_out(vty, "box info not exist%s", VTY_NEWLINE);
            }
            else if (slot_start > 1)
            {
                vty_out(vty, "box equipment only have one slot%s", VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, "slot   temp_low(deg C)   temprature(deg C)   temp_high(deg C)%s",  VTY_NEWLINE);

                vty_out(vty, "%-7s", "box");
                vty_out(vty, "%-18d", device.pbox->temp.temp_low);
                vty_out(vty, "%-20d", device.pbox->temp.temprature);
                vty_out(vty, "%-16d%s", device.pbox->temp.temp_high, VTY_NEWLINE);
            }

            break;

        case DEV_TYPE_SHELF:
            vty_out(vty, "slot   temp_low(deg C)   temprature(deg C)   temp_high(deg C)%s",  VTY_NEWLINE);

            if (slot_start == slot_end)
            {
                if (slot_start == device.punit[device.main_unit - 1]->slot_main_board || slot_start == device.punit[device.main_unit - 1]->slot_slave_board)
                {
                    p_devm_slot = (struct devm_slot *)devm_get_slot(device.main_unit, slot_start);

                    if (NULL == p_devm_slot)
                    {
                        vty_out(vty, "slot %d not exist%s", slot_start + 1, VTY_NEWLINE);
                        return CMD_WARNING;
                    }

                    vty_out(vty, "%-7d", slot_start);
                    vty_out(vty, "%-18d", p_devm_slot->temp.temp_low);
                    vty_out(vty, "%-20d", p_devm_slot->temp.temprature);
                    vty_out(vty, "%-16d%s", p_devm_slot->temp.temp_high, VTY_NEWLINE);
                }
            }
            else
            {
                for (i = slot_start; i < slot_end + 1; i++)
                {
                    if (i == device.punit[device.main_unit - 1]->slot_main_board || i == device.punit[device.main_unit - 1]->slot_slave_board)
                    {
                        p_devm_slot = (struct devm_slot *)devm_get_slot(device.main_unit, i);

                        if (NULL != p_devm_slot)
                        {
                            vty_out(vty, "%-11d", i);
                            vty_out(vty, "%-18d", p_devm_slot->temp.temp_low);
                            vty_out(vty, "%-20d", p_devm_slot->temp.temprature);
                            vty_out(vty, "%-16d%s", p_devm_slot->temp.temp_high, VTY_NEWLINE);
                        }
                    }
                }
            }

            break;

        default:
            break;
    }

    return CMD_SUCCESS;
}


/* write the configuration of each interface */
int devm_config_write(struct vty *vty)
{
    int i = 0;
//  int slot = 0;
//  struct devm_slot *pdevm_slot = NULL;

    if (DEV_TYPE_SHELF == device.type || DEV_TYPE_BOX == device.type)
    {
        vty_out(vty, "device%s", VTY_NEWLINE);
        vty_out(vty, " set power %d dynamic voltage %s%s", 1, (0 == dynamic_voltage_switch[0]) ? "enable" : "disable", VTY_NEWLINE);
        vty_out(vty, " set power %d dynamic voltage %s%s", 2, (0 == dynamic_voltage_switch[1]) ? "enable" : "disable", VTY_NEWLINE);

        if (0 != slotn_power_ctrl)
        {
            for (i = 0; i < SLOT_NUM_MAX; i++)
            {
                if (slotn_power_ctrl & (1 << i))
                {
                    vty_out(vty, " set slot %d power %s%s", i + 1, "down", VTY_NEWLINE);
                }
            }
        }

        /*write temperature limit to startup.config*/
        struct devm_temp  *p_temp = NULL;

        if (device.pbox)
        {
            p_temp = &device.pbox->temp;
        }
        else if (device.pmyunit)
        {
            p_temp = &device.pmyunit->pslot[device.pmyunit->myslot - 1]->temp;
        }

        if (NULL != p_temp)
        {
            vty_out(vty, " set temperature lower limit %d%s", p_temp->temp_low, VTY_NEWLINE);
            vty_out(vty, " set temperature upper limit %d%s", p_temp->temp_high, VTY_NEWLINE);
        }

        /*write power limit to startup.config*/
        struct devm_power  *p_devm_power = NULL;

        if (device.pbox)
        {
            p_devm_power = NULL;
        }
        else if (device.pmyunit)
        {
            p_devm_power = &device.pmyunit->power[0];
        }

        if (NULL != p_devm_power)
        {
            if(0 != p_devm_power[0].voltage_output)
            {
                vty_out(vty, " set power %d voltage lower limit %d%s", 1, p_devm_power[0].voltage_low, VTY_NEWLINE);
                vty_out(vty, " set power %d voltage upper limit %d%s", 1, p_devm_power[0].voltage_high, VTY_NEWLINE);
            }

            if(0 != p_devm_power[1].voltage_output)
            {
                vty_out(vty, " set power %d voltage lower limit %d%s", 2, p_devm_power[1].voltage_low, VTY_NEWLINE);
                vty_out(vty, " set power %d voltage upper limit %d%s", 2, p_devm_power[1].voltage_high, VTY_NEWLINE);
            }
        }

#if 0
        /*write fan limit to startup.config*/
        struct devm_fan  *p_devm_fan = NULL;

        if (device.pbox)
        {
            p_devm_fan = NULL;
        }
        else if (device.pmyunit)
        {
            p_devm_fan = &device.pmyunit->fan;
        }

        if (NULL != p_devm_fan)
        {
            vty_out(vty, " set fan lower limit %d%s", p_devm_fan->speed_min, VTY_NEWLINE);
            vty_out(vty, " set fan upper limit %d%s", p_devm_fan->speed_max, VTY_NEWLINE);
        }

#endif
#if 0 /*主副卡未调试，暂时不用*/

        if (DEVM_HEART_BEAT != device.punit[device.main_unit - 1]->heart_beat)
        {
            vty_out(vty, "device heart-beat  interval %d%s", device.punit[device.main_unit - 1]->heart_beat, VTY_NEWLINE);
        }

        for (slot = 1; slot < SLOT_NUM_MAX + 1; slot++)
        {

        }

#endif

    }

    return ERRNO_SUCCESS;
}


static struct cmd_node devm_node =
{
    DEVM_NODE,
    "%s(config-devm)# ",
    1 /* VTYSH */
};


void devm_cli_init(void)
{
    install_node(&devm_node, devm_config_write);
    install_element(CONFIG_NODE, &devm_node_inter_cmd, CMD_SYNC);

    install_default(DEVM_NODE);

    install_element(DEVM_NODE, &set_slotn_power_cmd, CMD_LOCAL);
    install_element(DEVM_NODE, &set_temperature_limit_cmd, CMD_SYNC);
    install_element(DEVM_NODE, &set_dynamic_voltage_cmd, CMD_LOCAL);
    install_element(DEVM_NODE, &set_voltage_limit_cmd, CMD_SYNC);

    install_element(DEVM_NODE, &hal_cmd_show_slot_hash_cmd, CMD_LOCAL);
    install_element(DEVM_NODE, &hal_cmd_show_global_variables_cmd, CMD_LOCAL);
    install_element(DEVM_NODE, &hal_cmd_show_slot_cmd, CMD_LOCAL);
    install_element(DEVM_NODE, &hal_cmd_set_hash_add_slot_cmd, CMD_LOCAL);
    install_element(DEVM_NODE, &hal_cmd_set_hash_del_slot_cmd, CMD_LOCAL);
    install_element(DEVM_NODE, &hal_cmd_set_slot_fpga_reconfig_cmd, CMD_LOCAL);
    install_element(SYSRUN_NODE, &devm_op_eeprom_info_cmd, CMD_LOCAL);

    install_element(DEVM_NODE, &device_watchdog_interval_cmd, CMD_LOCAL);
    install_element(DEVM_NODE, &device_heart_beat_interval_cmd, CMD_LOCAL);
    install_element(DEVM_NODE, &show_slot_rtime_cmd, CMD_LOCAL);
    install_element(DEVM_NODE, &show_slot_port_cmd, CMD_LOCAL);

    install_element(DEVM_NODE, &show_slot_mac_cmd, CMD_LOCAL);
    install_element(DEVM_NODE, &show_slot_version_cmd, CMD_LOCAL);
    install_element(DEVM_NODE, &show_slot_type_cmd, CMD_LOCAL);

    install_element(DEVM_NODE, &reboot_slot_all_cmd, CMD_LOCAL);
//    install_element(DEVM_NODE, &reboot_slot_cmd, CMD_LOCAL);
//  install_element (DEVM_NODE, &reset_master_cmd, CMD_LOCAL);
    install_element(DEVM_NODE, &setmx_m2s_cmd, CMD_LOCAL);
    install_element(DEVM_NODE, &reset_slot_cmd, CMD_LOCAL);
    install_element(DEVM_NODE, &show_dev_slot_all_info_cmd, CMD_LOCAL);
    install_element(DEVM_NODE, &show_dev_slot_info_cmd, CMD_LOCAL);
    install_element(DEVM_NODE, &show_dev_backplane_cmd, CMD_LOCAL);
    install_element(DEVM_NODE, &show_dev_unit_cmd, CMD_LOCAL);
    install_element(DEVM_NODE, &show_dev_power_cmd, CMD_LOCAL);
    install_element(DEVM_NODE, &show_dev_fan_cmd, CMD_LOCAL);
    install_element(DEVM_NODE, &show_dev_temp_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &devm_debug_monitor_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_devm_debug_monitor_cmd, CMD_LOCAL);    
}
