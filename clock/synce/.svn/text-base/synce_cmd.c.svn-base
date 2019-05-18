/*
*    synce cmd function
*/
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/command.h>
#include <lib/module_id.h>
#include <lib/ifm_common.h>
#include <lib/memtypes.h>
#include <lib/memory.h>
#include <lib/vty.h>
#include <lib/log.h>
#include <lib/devm_com.h>
#include "lib/pkt_buffer.h"
#include "lib/msg_ipc.h"

#include "lib/linklist.h"

#include "clock/clock_src.h"
#include "clock/clock_if.h"
#include "clock/clock_cmd.h"

#include "synce.h"
#include "synce_ssm.h"
#include "synce_cmd.h"

extern struct synce_t g_synce;         /* synce 全局数据结构 */
extern unsigned char e1_if_priority[32];

static struct cmd_node synce_node =
{
    SYNCE_NODE,
    "%s(config-synce)# ",
    1,
};

/************************************************
 * Function: synce_clock_state_val_to_str
 * Input:    state 运行状态
 * Output:   str 字符串指针
 * Return:   NONE
 * Description:  运行状态转换成字符串，命令行中显示
 ************************************************/
static void synce_clock_state_val_to_str(char *str, enum SYNCE_STAT state)
{
    switch (state)
    {
        case SYNCE_RUN_STATE_AUTO :
            strcpy(str, SYNCE_RUN_STATE_AUTO_STR);
            break;

        case SYNCE_RUN_STATE_FREERUN :
            strcpy(str, SYNCE_RUN_STATE_FREERUN_STR);
            break;

        case SYNCE_RUN_STATE_HOLDOVER :
            strcpy(str, SYNCE_RUN_STATE_HOLDOVER_STR);
            break;

        case SYNCE_RUN_STATE_LOCKED :
            strcpy(str, SYNCE_RUN_STATE_LOCKED_STR);
            break;

        case SYNCE_RUN_STATE_LOSELOCK :
            strcpy(str, SYNCE_RUN_STATE_LOSELOCK_STR);
            break;

        default:
            strcpy(str, "INVALID");
            break;
    }

    return;
}

/************************************************
 * Function: synce_clean_ssm_pkt_data
 * Input:    none
 * Output:   none
 * Return:   0 : successed
 * Description:  synce清理报文统计数据，防止关闭功能，又开启时，报文统计不归零。
 ************************************************/
static int synce_clean_ssm_pkt_data(void)
{
    struct synce_if         *synce_port;
    int                     i;

    for (i = 0; i < CLOCK_SRC_NUM_MAX; i++)
    {
        if (pclock_src[i] == NULL)
        {
            continue;
        }

        synce_port = pclock_src[i]->psynce;

        if (synce_port != NULL)
        {
            synce_port->pkt_err = 0;
            synce_port->pkt_recv_cnt = 0;
            synce_port->pkt_send_cnt = 0;
            synce_port->timeout_cnt = 0;
            synce_port->send_interval_cnt = 0;
        }
    }

    return 0;
}

/************************************************
 * Function: synce_print_clock_mode
 * Input:    mode 时钟运行模式
 * Output:   p ；字符串指针
 * Return:   0 : successed
 * Description:  synce时钟运行模式转换字符串
 ************************************************/
static int synce_print_clock_mode(enum SYNCE_MODE mode, char *p)
{
    switch (mode)
    {
        case SYNCE_MODE_AUTO:
            sprintf(p, CLOCK_MODE_AUTO);
            break;

        case SYNCE_MODE_FORCE:
            sprintf(p, CLOCK_MODE_FORCE);
            break;

        case SYNCE_MODE_MANUAL:
            sprintf(p, CLOCK_MODE_MANUAL);
            break;

        default:
            sprintf(p, "none");
            break;
    }

    return 0;
}


DEFUN(synce_enable_func,
      synce_enable_cmd,
      "synce",
      "enter synce command node\n"
     )
{
    vty->node = SYNCE_NODE;
    return CMD_SUCCESS;

}


DEFUN(synce_mode_select_func,
      synce_mode_select_cmd,
      "synce mode (auto|force|manual)",
      "synce function set\n"
      "set current run mode\n"
      "auto mode\n"
      "force mode\n"
      "manual mode\n"
     )
{
    enum SYNCE_MODE mode = 0;

    if (!strncmp(argv[0], "auto", 3))
    {
        mode = SYNCE_MODE_AUTO;
    }
    else if (!strncmp(argv[0], "force", 3))
    {
        g_synce.preempt = 0;
        mode = SYNCE_MODE_FORCE;
    }
    else if (!strncmp(argv[0], "manual", 3))
    {
        g_synce.preempt = 0;
        mode = SYNCE_MODE_MANUAL;
    }
    else
    {
        return CMD_WARNING;
    }

    synce_set_info(&mode, SYNCE_INFO_MODE);
    return CMD_SUCCESS;
}


DEFUN(no_synce_mode_select_func,
      no_synce_mode_select_cmd,
      "no synce mode",
      NO_STR
      "synce\n"
      "set synce default mode \n"
     )
{
    g_synce.mode = SYNCE_CONFIG_MODE_DEFAULT;
    synce_set_info(&g_synce.mode, SYNCE_INFO_MODE);

    return CMD_SUCCESS;
}


DEFUN(synce_ssm_enable_func,
      synce_ssm_enable_cmd,
      "synce ssm (disable|enable)",
      "synce function set\n"
      "ssm function \n"
      "set ssm disable \n"
      "set ssm enable \n"
     )
{
    uint8_t ssm_enable = 0;

    if (!strncmp(argv[0], "disable", 3))
    {
        ssm_enable = SYNCE_SSM_DISABLE;
    }
    else if (!strncmp(argv[0], "enable", 3))
    {
        ssm_enable = SYNCE_SSM_ENABLE;
        synce_clean_ssm_pkt_data();
    }
    else
    {
        return CMD_WARNING;
    }

    synce_set_info(&ssm_enable, SYNCE_INFO_SSM_ENABLE);
    return CMD_SUCCESS;
}


DEFUN(no_synce_ssm_enable_func,
      no_synce_ssm_enable_cmd,
      "no synce ssm",
      NO_STR
      "synce\n"
      "set synce ssm function default\n"
     )
{
    g_synce.ssm_enable = SYNCE_CONFIG_SSM_ENALBE_DEFAULT;
    synce_set_info(&g_synce.ssm_enable, SYNCE_INFO_SSM_ENABLE);
    return CMD_SUCCESS;
}


DEFUN(synce_ssm_interval_func,
      synce_ssm_interval_cmd,
      "synce ssm interval <1-10>",
      "synce function set\n"
      "ssm function\n"
      "send interval\n"
      "send interval value and unit is second\n"
     )
{
    int interval;
    interval = atoi(argv[0]);

    if ((interval < 1) || (interval > 10))
    {
        vty_error_out(vty, "Wrong parameter range, interval is <1-10>. %s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (g_synce.ssm_enable == SYNCE_DISABLE)
    {
        vty_warning_out(vty, "%% set failed! the synce ssm is disable! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    g_synce.ssm_interval = interval;
    g_synce.ssm_timeout  = 5 * interval;

    return CMD_SUCCESS;
}


DEFUN(no_synce_ssm_interval_func,
      no_synce_ssm_interval_cmd,
      "no synce ssm interval",
      NO_STR
      "synce\n"
      "set ssm function\n"
      "set synce ssm interval default\n"
     )
{
    g_synce.ssm_interval = SYNCE_CONFIG_SSM_INTERVAL_DEFAULT;
    return CMD_SUCCESS;
}


DEFUN(synce_preempt_enable_func,
      synce_preempt_enable_cmd,
      "synce preempt (disable|enable)",
      "set synce function\n"
      "set preempt function\n"
      "set preempt function disable\n"
      "set preempt function enable\n"
     )
{
    uint8_t value;

    if (!strncmp(argv[0], "disable", 3))
    {
        value = SYNCE_DISABLE;
    }
    else if (!strncmp(argv[0], "enable", 3))
    {
        value = SYNCE_ENABLE;
    }
    else
    {
        return CMD_WARNING;
    }

    if (g_synce.mode != SYNCE_MODE_AUTO)
    {
        vty_warning_out(vty, "%% set failed! the preempt function is used with the auto mode %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    synce_set_info(&value, SYNCE_INFO_PREEMPT);
    return CMD_SUCCESS;
}


DEFUN(no_synce_preempt_enable_func,
      no_synce_preempt_enable_cmd,
      "no synce preempt",
      NO_STR
      "synce\n"
      "set preempt function\n"
      "set synce preempt default\n"
     )
{
    g_synce.preempt = SYNCE_CONFIG_PREEMPT_DEFAULT;
    synce_set_info(&g_synce.preempt, SYNCE_INFO_PREEMPT);
    return CMD_SUCCESS;
}


DEFUN(synce_failback_wtr_func,
      synce_failback_wtr_cmd,
      "synce failback wtr <0-3600>",
      "set synce function\n"
      "failback\n"
      "set wtr\n"
      "failback time value and unit is second\n"
     )
{
    int wtr;
    wtr = atoi(argv[0]);

    if ((wtr < 0) || (wtr > 3600))
    {
        vty_error_out(vty, "Wrong parameter range, interval is <0-3600>. %s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    g_synce.wtr = wtr;

    return CMD_SUCCESS;
}


DEFUN(no_synce_failback_wtr_func,
      no_synce_failback_wtr_cmd,
      "no synce failback wtr",
      NO_STR
      "synce\n"
      "set synce failback\n"
      "set synce failback wtr default\n"
     )
{
    g_synce.wtr = SYNCE_CONFIG_WTR_DEFAULT;
    return CMD_SUCCESS;
}


DEFUN(synce_holdtime_func,
      synce_holdtime_cmd,
      "synce hold-time <300-604800>",
      "set synce function\n"
      "set clock hold-time \n"
      "clock hold-time value and unit is second\n"
     )
{
    int time;
    time = atoi(argv[0]);

    if ((time < 300) || (time > 604800))
    {
        vty_error_out(vty, "Wrong parameter range, interval is <300-604800>. %s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    g_synce.hold_time = time;

    if (g_synce.iHoldOverTimer > 0)
    {
        g_synce.iHoldOverTimer = g_synce.hold_time;
    }

    return CMD_SUCCESS;
}


DEFUN(no_synce_holdtime_func,
      no_synce_holdtime_cmd,
      "no synce hold-time",
      NO_STR
      "synce\n"
      "set synce hold-time default\n"
     )
{
    g_synce.hold_time = SYNCE_CONFIG_HOLDTIME_DEFAULT;
    return CMD_SUCCESS;
}
     
 DEFUN(synce_freqoffset_func,
       synce_freqoffset_cmd,
       "clock frequency deviation <-75-75>",
       "set clock function\n"
       "set clock frequency \n"
       "set clock deviation \n"
       "clock frequency deviation value\n"
      )
 {
     int freq_offset;
     freq_offset = atoi(argv[0]);
 
     if ((freq_offset < -75) || (freq_offset > 75))
     {
         vty_error_out(vty, "Wrong parameter range, interval is <-75-75>. %s", VTY_NEWLINE);
 
         return CMD_WARNING;
     }
 
     g_synce.iFreqOffset = freq_offset;
     synce_set_info(&g_synce.iFreqOffset, SYNCE_INFO_SET_FREQ_OFFSET);
     return CMD_SUCCESS;
 }
 
 
 DEFUN(no_synce_freqoffset_func,
       no_synce_freqoffset_cmd,
       "no clock frequency deviation",
       NO_STR
       "clock\n"
       "set clock frequency \n"
       "set clock frequency deviation default\n"
      )
 {
     g_synce.iFreqOffset = 0;
     synce_set_info(&g_synce.iFreqOffset, SYNCE_INFO_SET_FREQ_OFFSET);
     return CMD_SUCCESS;
 }


DEFUN(synce_priority_func,
      synce_clock_priority_cmd,
      "synce priority <1-8>",
      "set synce function\n"
      "clock priority\n"
      "clock priority value\n"
     )
{
    uint32_t ifindex;
    uint8_t pri;
    struct synce_clk_t clk_src_data;
    struct clock_src_t  *pclk_src = NULL;
    struct synce_if     *psynce_if = NULL;
    ifindex = (uint32_t) vty->index;
    pri = atoi(argv[0]);
    pclk_src = clock_src_lookup(ifindex);

    if (pclk_src == NULL)
    {
        pclk_src = clock_src_create(ifindex);

        if (NULL == pclk_src)
        {
            zlog_debug(CLOCK_DBG_ERROR,"%s[%d]:leave %s:error:fail to create clock src table bucket\n", __FILE__, __LINE__, __func__);
            return CMD_WARNING;
        }

        if (clock_src_add(pclk_src))
        {
            zlog_debug(CLOCK_DBG_ERROR,"%s[%d]:leave %s:error:fail to add clock src table bucket\n", __FILE__, __LINE__, __func__);
            return CMD_WARNING;
        }
    }

    if (pri <= 0 || pri > 8)
    {
        vty_warning_out(vty, "%% priority range is <1-8 >...%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    psynce_if = pclk_src->psynce;

    if (psynce_if != NULL)
    {
        psynce_if->priority = pri;
        clk_src_data.ifindex = ifindex;
        clk_src_data.type = synce_get_clk_type(ifindex);
        clk_src_data.status = CLOCK_STAT_LOSS;

        if (synce_add_clock(&clk_src_data))
        {
            synce_delete_clock(&clk_src_data);
            XFREE(MTYPE_CLOCK_ENTRY, pclk_src->psynce);
            pclk_src->psynce = NULL;
            clock_src_delete(ifindex);
            vty_warning_out(vty, "%% add clock source failed! can't find valid resource %s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    else
    {
        pclk_src->psynce = XCALLOC(MTYPE_CLOCK_ENTRY, sizeof(struct synce_if));
        psynce_if = pclk_src->psynce;

        if (psynce_if != NULL)
        {
            psynce_if->priority = pri;

            clk_src_data.ifindex = ifindex;
            clk_src_data.type = synce_get_clk_type(ifindex);
            clk_src_data.status = CLOCK_STAT_LOSS;

            if (synce_add_clock(&clk_src_data))
            {
                synce_delete_clock(&clk_src_data);
                XFREE(MTYPE_CLOCK_ENTRY, pclk_src->psynce);
                pclk_src->psynce = NULL;
                clock_src_delete(ifindex);
                vty_warning_out(vty, "%% add clock source failed! can't find valid resource %s", VTY_NEWLINE);
                return CMD_WARNING;
            }
        }
        else
        {
            zlog_debug(CLOCK_DBG_ERROR,"create interface synce date failed!...%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    return CMD_SUCCESS;
}


DEFUN(no_synce_priority_func,
      no_synce_clock_priority_cmd,
      "no synce priority",
      NO_STR
      "synce\n"
      "set current port synce fucntion disable\n"
     )
{
    uint32_t ifindex;
    struct synce_clk_t clk_src_data;
    struct clock_src_t  *pclk_src = NULL;
    struct synce_if     *psynce_if = NULL;

    ifindex = (uint32_t) vty->index;
    pclk_src = clock_src_lookup(ifindex);

    if (NULL == pclk_src)
    {
        zlog_debug(CLOCK_DBG_ERROR,"%s[%d]:leave %s:error:fail to lookup clock if table bucket\n", __FILE__, __LINE__, __func__);
        return CMD_WARNING;
    }

    psynce_if = pclk_src->psynce;

    if (psynce_if != NULL)
    {
        clk_src_data.ifindex = ifindex;
        clk_src_data.type = synce_get_clk_type(ifindex);
        clk_src_data.status = CLOCK_STAT_LOSS;
        synce_delete_clock(&clk_src_data);
        XFREE(MTYPE_CLOCK_ENTRY, pclk_src->psynce);
        pclk_src->psynce = NULL;
    }

    clock_src_delete(ifindex);
    return CMD_SUCCESS;
}


DEFUN(synce_clk_source_set_func,
      synce_clk_source_set_cmd,
      "synce source (force | manual) interface clock <1-2>",
      "set synce function\n"
      "set clock soure\n"
      "under force mode set clock source\n"
      "under manual mode set clock source\n"
      "interface select\n"
      "clock interface\n"
      "clock interface value\n"
     )
{
    uint32_t ifindex = 0;
    ifindex = ifm_get_ifindex_by_name("clock", (char *) argv[1]);

    if (g_synce.mode == SYNCE_MODE_AUTO)
    {
        vty_warning_out(vty, "%% set failed! cur synce mode is auto,can't set clock src! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (synce_lookup_clk(ifindex) == NULL)
    {
        vty_warning_out(vty, "%% the port is not clock source interface!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (!strcmp(argv[0], "force"))
    {
        if (g_synce.mode == SYNCE_MODE_MANUAL)
        {
            vty_warning_out(vty, "%% set failed! cur synce mode is manual,please select manual mode! %s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        if (ifindex == 0)
        {
            vty_warning_out(vty, "%% the port is not exist!%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        g_synce.clk_force = ifindex;
        //synce_select_clock(SYNCE_EVENT_SEL_CMD_FORCE_SELECT);
    }
    else if (!strcmp(argv[0], "manual"))
    {
        if (g_synce.mode == SYNCE_MODE_FORCE)
        {
            vty_warning_out(vty, "%% set failed! cur synce mode is force,please select force mode! %s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        if (ifindex == 0)
        {
            vty_warning_out(vty, "%% set failed! the port is not exist%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        g_synce.clk_manul = ifindex;
        //synce_select_clock(SYNCE_EVENT_SEL_CMD_MANUAL_SELECT);
    }
    else
    {
        printf("mode error!\n");
    }

    return CMD_SUCCESS;
}


DEFUN(synce_eth_source_set_func,
      synce_eth_source_set_cmd,
      "synce source (force | manual) interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP|tdm USP}",
      "set synce function\n"
      "set clock soure\n"
      "under force mode set clock source\n"
      "under manual mode set clock source\n"
      "interface select\n"
      "ethernet interface\n"
      "The port/subport of the interface, format: <0-7>/<0-31>/<1-255>\n"
      CLI_INTERFACE_GIGABIT_ETHERNET_STR
      CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
      CLI_INTERFACE_XGIGABIT_ETHERNET_STR
      CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
      CLI_INTERFACE_TDM_STR
      CLI_INTERFACE_TDM_VHELP_STR
     )
{
    uint32_t ifindex = 0;

    if (argv[1] != NULL && argv[2] == NULL && argv[3] == NULL && argv[4] == NULL)
    {
        ifindex = ifm_get_ifindex_by_name("ethernet", (char *) argv[1]);
    }
    else if (argv[1] == NULL && argv[2] != NULL && argv[3] == NULL && argv[4] == NULL)
    {
        ifindex = ifm_get_ifindex_by_name("gigabitethernet", (char *) argv[2]);
    }
    else if (argv[1] == NULL && argv[2] == NULL && argv[3] != NULL && argv[4] == NULL)
    {
        ifindex = ifm_get_ifindex_by_name("xgigabitethernet", (char *) argv[3]);
    }
    else if (argv[1] == NULL && argv[2] == NULL && argv[3] == NULL && argv[4] != NULL)
    {
        ifindex = ifm_get_ifindex_by_name("tdm", (char *) argv[4]);
    }

    if (g_synce.mode == SYNCE_MODE_AUTO)
    {
        vty_warning_out(vty, "%% set failed! cur synce mode is auto,can't set clock src! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (synce_lookup_clk(ifindex) == NULL)
    {
        vty_warning_out(vty, "%% the port is not clock source interface!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (!strcmp(argv[0], "force"))
    {
        if (g_synce.mode == SYNCE_MODE_MANUAL)
        {
            vty_warning_out(vty, "%% set failed! cur synce mode is manual,please select manual mode! %s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        if (ifindex == 0)
        {
            vty_warning_out(vty, "%% the port is not exist!%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        g_synce.clk_force = ifindex;
        g_synce.clk_select = 0;
        //synce_select_clock(SYNCE_EVENT_SEL_CMD_FORCE_SELECT);
    }
    else if (!strcmp(argv[0], "manual"))
    {
        if (g_synce.mode == SYNCE_MODE_FORCE)
        {
            vty_warning_out(vty, "%% set failed! cur synce mode is force,please select force mode! %s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        if (ifindex == 0)
        {
            vty_warning_out(vty, "%% set failed! the port is not exist%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        g_synce.clk_manul = ifindex;
        g_synce.clk_select = 0;
        //synce_select_clock(SYNCE_EVENT_SEL_CMD_MANUAL_SELECT);
    }

    return CMD_SUCCESS;
}


DEFUN(synce_show_config_func,
      synce_show_config_cmd,
      "show synce config",
      SHOW_STR
      "synce function\n"
      "show current synce configure\n"
     )
{
    char if_name[NAME_STRING_LEN];
    char quality_level[NAME_STRING_LEN] = {'U', 'N', 'K', '\0'};

    synce_print_clock_mode(g_synce.mode, if_name);
    vty_out(vty, "------------------------------------------------------------------------------%s", VTY_NEWLINE);
    vty_out(vty, "current clock mode :\t%s%s", if_name, VTY_NEWLINE);

    vty_out(vty, "clock preempt state:\t%s%s", CLOCK_ENALBE_DISPLAY(g_synce.preempt), VTY_NEWLINE);

    vty_out(vty, "ssm send interval  : \t%d s %s", g_synce.ssm_interval, VTY_NEWLINE);

    vty_out(vty, "failback time      : \t%d s %s", g_synce.wtr, VTY_NEWLINE);
    vty_out(vty, "hold time          : \t%d s %s", g_synce.hold_time, VTY_NEWLINE);
    vty_out(vty, "ssm enalbe state   : \t%s %s", SSM_ENALBE_DISPLAY(g_synce.ssm_enable), VTY_NEWLINE);
    vty_out(vty, "ssm timeout time   : \t%d s %s", g_synce.ssm_timeout, VTY_NEWLINE);
    ifm_get_name_by_ifindex(g_synce.clk_select, if_name);
    vty_out(vty, "current device select clock source   : \t%s%s", if_name, VTY_NEWLINE);

    if (g_synce.mode == SYNCE_MODE_FORCE)
    {
        ifm_get_name_by_ifindex(g_synce.clk_force, if_name);
        vty_out(vty, "current force clock source           : \t%s%s", if_name, VTY_NEWLINE);
    }
    else if (g_synce.mode == SYNCE_MODE_MANUAL)
    {
        ifm_get_name_by_ifindex(g_synce.clk_manul, if_name);
        vty_out(vty, "current manual clock source      : \t%s%s", if_name, VTY_NEWLINE);
    }

    if (g_synce.ssm_enable)
    {
        synce_ql_val_to_str(quality_level, g_synce.cur_clk_ssm_ql);
        vty_out(vty, "current device ssm quality level : \t%s%s", quality_level, VTY_NEWLINE);
    }

    vty_out(vty, "hold timer count        : \t%d%s", g_synce.iHoldOverTimer, VTY_NEWLINE);
    vty_out(vty, "Wait timer count        : \t%d%s", g_synce.iWaitTimer, VTY_NEWLINE);
    vty_out(vty, "freeclean timer count   : \t%d%s", g_synce.iFreeClrTimer, VTY_NEWLINE);
    vty_out(vty, "Clock Freq Deviation    : \t%d%s", g_synce.iFreqOffset, VTY_NEWLINE);

    vty_out(vty, "------------------------------------------------------------------------------%s", VTY_NEWLINE);
    return CMD_SUCCESS;
}


DEFUN(synce_show_source_func,
      synce_show_source_cmd,
      "show synce source",
      SHOW_STR
      "synce function\n"
      "show current synce clock source\n"
     )
{
    struct synce_clk_t *pclk = NULL;
    struct listnode *pnode = NULL;
    char if_name[NAME_STRING_LEN];
    char quality_level[NAME_STRING_LEN] = {'U', 'N', 'K', '\0'};
    char clock_state_str[NAME_STRING_LEN];
    uint8_t pri, ql;
    int ret = -1;
    struct ifm_info ifm_info;

    memset(clock_state_str, 0, IF_NAME_LEN);
    synce_clock_state_val_to_str(clock_state_str, g_synce.cur_clk_stat);
    vty_out(vty, "current clock source state : %s%s", clock_state_str, VTY_NEWLINE);

    if (g_synce.ssm_enable)
    {
        vty_out(vty, "source\t\t\tpriority        valid_state    lock_state    ssm_value%s", VTY_NEWLINE);
        vty_out(vty, "------------------------------------------------------------------------------%s", VTY_NEWLINE);

        for (ALL_LIST_ELEMENTS_RO(&g_synce.clk_list, pnode, pclk))
        {
            memset(if_name, 0, IF_NAME_LEN);
            pri = synce_get_priority(pclk->ifindex);
            ret = ifm_get_all_info(pclk->ifindex, MODULE_ID_CLOCK,&ifm_info);

            if ((pri == 0) || (ret < 0))
            {
                continue;
            }

            ql  = synce_get_quality_level(pclk->ifindex);
            memset(quality_level, 0, IF_NAME_LEN);
            synce_ql_val_to_str(quality_level, ql);

            ifm_get_name_by_ifindex(pclk->ifindex, if_name);

            if (IFM_TYPE_IS_CLOCK(pclk->ifindex) || IFM_TYPE_IS_TDM(pclk->ifindex) || IFM_TYPE_IS_E1(pclk->ifindex) || IFM_TYPE_IS_STM(pclk->ifindex))
            {
                vty_out(vty, "%s \t\t %d\t\t%s\t\t%8s\t %s%s" \
                        , if_name, pri, CLOCK_VALID_STATE_DISPLAY(pclk->valid_stat) \
                        , CLOCK_STATE_DISPLAY(pclk->status), quality_level, VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, "%s \t %d\t\t%s\t\t%8s\t %s%s" \
                        , if_name, pri, CLOCK_VALID_STATE_DISPLAY(pclk->valid_stat) \
                        , CLOCK_STATE_DISPLAY(pclk->status), quality_level, VTY_NEWLINE);
            }
        }
    }
    else
    {
        vty_out(vty, "source \t\t\tpriority \tvalid state \tlock state%s", VTY_NEWLINE);
        vty_out(vty, "------------------------------------------------------------------%s", VTY_NEWLINE);

        for (ALL_LIST_ELEMENTS_RO(&g_synce.clk_list, pnode, pclk))
        {
            memset(if_name, 0, IF_NAME_LEN);
            pri = synce_get_priority(pclk->ifindex);
            ret = ifm_get_all_info(pclk->ifindex, MODULE_ID_CLOCK,&ifm_info);

            if ((pri == 0) || (ret < 0))
            {
                continue;
            }

            ifm_get_name_by_ifindex(pclk->ifindex, if_name);

            if (IFM_TYPE_IS_CLOCK(pclk->ifindex) || IFM_TYPE_IS_TDM(pclk->ifindex) || IFM_TYPE_IS_E1(pclk->ifindex) || IFM_TYPE_IS_STM(pclk->ifindex))
            {
                vty_out(vty, "%s \t\t %d\t\t%s\t\t  %s\t%s" \
                        , if_name, pri, CLOCK_VALID_STATE_DISPLAY(pclk->valid_stat) \
                        , CLOCK_STATE_DISPLAY(pclk->status), VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, "%s \t %d\t\t%s\t\t  %s\t%s" \
                        , if_name, pri, CLOCK_VALID_STATE_DISPLAY(pclk->valid_stat) \
                        , CLOCK_STATE_DISPLAY(pclk->status), VTY_NEWLINE);
            }
        }
    }

    return CMD_SUCCESS;
}


DEFUN(synce_show_ssm_pkt_func,
      synce_show_ssm_pkt_cmd,
      "show synce ssm",
      SHOW_STR
      "synce function\n"
      "show current port synce ssm state\n"
     )
{
    int ifindex;
    char ql_str[64];
    struct clock_src_t  *pclk_src = NULL;
    struct synce_if     *psynce_if = NULL;


    ifindex = (uint32_t) vty->index;
    pclk_src = clock_src_lookup(ifindex);

    if (NULL == pclk_src)
    {
        zlog_debug(CLOCK_DBG_ERROR,"%s[%d]:leave %s:error:fail to get l2if :0x%x\n", __FILE__, __LINE__, __func__, ifindex);
        return CMD_WARNING;
    }

    if (g_synce.ssm_enable == SYNCE_DISABLE)
    {
        vty_warning_out(vty, "%% the synce ssm is disable! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memset(ql_str, 0, 64);

    psynce_if = pclk_src->psynce;

    if (psynce_if != NULL)
    {
        synce_ql_val_to_str(ql_str, psynce_if->recv_ql);
        vty_out(vty, "pkt_recv :  %d %s", psynce_if->pkt_recv_cnt, VTY_NEWLINE);
        vty_out(vty, "pkt_send :  %d %s", psynce_if->pkt_send_cnt, VTY_NEWLINE);
        vty_out(vty, "priority :  %d %s", psynce_if->priority, VTY_NEWLINE);
        vty_out(vty, "recv_ql  :  %s %s", ql_str, VTY_NEWLINE);
    }
    else
    {
        vty_warning_out(vty, "%% the port synce is disable! %s", VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}

/************************************************
 * Function: synce_node_config_write
 * Input:    vty : vty指针
 * Output:   none
 * Return:   none
 * Description:  synce节点下的配置保存处理
 ************************************************/
int synce_node_config_write(struct vty *vty)
{
    char str[128];
    const char *mode_str[] = {"auto", "force", "manual"};

    vty_out(vty, "synce%s", VTY_NEWLINE);

    if (g_synce.mode != SYNCE_CONFIG_MODE_DEFAULT)
    {
        vty_out(vty, " synce mode %s%s", mode_str[g_synce.mode], VTY_NEWLINE);

        if ((g_synce.mode == SYNCE_MODE_FORCE) && (g_synce.clk_force != 0))
        {
            ifm_get_name_by_ifindex(g_synce.clk_force, str);
            vty_out(vty, " synce source force interface %s%s", str, VTY_NEWLINE);
        }
        else if ((g_synce.mode == SYNCE_MODE_MANUAL) && (g_synce.clk_manul != 0))
        {
            ifm_get_name_by_ifindex(g_synce.clk_manul, str);
            vty_out(vty, " synce source manual interface %s%s", str, VTY_NEWLINE);
        }
    }

    if (g_synce.ssm_enable != SYNCE_CONFIG_SSM_ENALBE_DEFAULT)
    {
        vty_out(vty, " synce ssm %s%s", (g_synce.ssm_enable ? "enable" : "disable"), VTY_NEWLINE);

        if (g_synce.ssm_interval != SYNCE_CONFIG_SSM_INTERVAL_DEFAULT)
        {
            vty_out(vty, " synce ssm interval %d%s", g_synce.ssm_interval, VTY_NEWLINE);
        }
    }

    if (g_synce.hold_time != SYNCE_CONFIG_HOLDTIME_DEFAULT)
    {
        vty_out(vty, " synce hold-time %d%s", g_synce.hold_time, VTY_NEWLINE);
    }

    if (g_synce.wtr != SYNCE_CONFIG_WTR_DEFAULT)
    {
        vty_out(vty, " synce failback wtr %d%s", g_synce.wtr, VTY_NEWLINE);
    }

    if (g_synce.preempt != SYNCE_CONFIG_PREEMPT_DEFAULT)
    {
        vty_out(vty, " synce preempt %s%s", (g_synce.preempt ? "enable" : "disable"), VTY_NEWLINE);
    }

    return 0;
}


/************************************************
 * Function: synce_node_config_write
 * Input:    vty : vty指针
 * Output:   none
 * Return:   none
 * Description:  synce节点下的配置保存处理
 ************************************************/
static int synce_physical_if_config_write(struct vty *vty)
{
    char ifname[IFNET_NAMESIZE];
    int i, ret;
    uint32_t ifindex;
    sint8 cmd[128];
    struct clock_src_t *pclk_src = NULL;

    for (i = 0; i < CLOCK_SRC_NUM_MAX; i++)
    {
        pclk_src = pclock_src[i];

        if (pclk_src == NULL)
        {
            continue;
        }

        ifindex = pclk_src->ifindex;
        ret = ifm_get_name_by_ifindex(ifindex, ifname);

        if (ret < 0)
        {
            continue;
        }

        if (pclk_src->psynce != NULL && IFM_TYPE_IS_METHERNET(ifindex))
        {
            memset(cmd, 0, 128);
            vty_out(vty, "interface %s%s", ifname, VTY_NEWLINE);
            sprintf(cmd, " synce priority %d%s", pclk_src->psynce->priority, VTY_NEWLINE);
            vty_out(vty, "%s", cmd);
            vty_out(vty, "!%s", VTY_NEWLINE);
        }

    }

    return 0;
}
/************************************************
 * Function: synce_node_config_write
 * Input:    vty : vty指针
 * Output:   none
 * Return:   none
 * Description:  synce节点下的配置保存处理
 ************************************************/
static int synce_tdm_if_config_write(struct vty *vty)
{
    char ifname[IFNET_NAMESIZE];
    int i, ret;
    uint32_t ifindex;
    sint8 cmd[128];
    struct clock_src_t *pclk_src = NULL;

    for (i = 0; i < CLOCK_SRC_NUM_MAX; i++)
    {
        pclk_src = pclock_src[i];

        if (pclk_src == NULL)
        {
            continue;
        }

        ifindex = pclk_src->ifindex;

        ret = ifm_get_name_by_ifindex(ifindex, ifname);

        if (ret < 0)
        {
            continue;
        }

        if (pclk_src->psynce != NULL && IFM_TYPE_IS_TDM(ifindex))
        {
            memset(cmd, 0, 128);
            vty_out(vty, "interface %s%s", ifname, VTY_NEWLINE);
            sprintf(cmd, " synce priority %d%s", pclk_src->psynce->priority, VTY_NEWLINE);
            vty_out(vty, "%s", cmd);
            vty_out(vty, "!%s", VTY_NEWLINE);
        }

    }

    return 0;

}
/************************************************
 * Function: synce_node_config_write
 * Input:    vty : vty指针
 * Output:   none
 * Return:   none
 * Description:  synce节点下的配置保存处理
 ************************************************/
static int synce_e1_if_config_write(struct vty *vty)
{
    char ifname[IFNET_NAMESIZE];
    int i, ret;
    uint32_t ifindex;
    sint8 cmd[128];
    struct clock_src_t *pclk_src = NULL;

    for (i = 0; i < CLOCK_SRC_NUM_MAX; i++)
    {
        pclk_src = pclock_src[i];

        if (pclk_src == NULL)
        {
            continue;
        }

        ifindex = pclk_src->ifindex;

        ret = ifm_get_name_by_ifindex(ifindex, ifname);

        if (ret < 0)
        {
            continue;
        }

        if (pclk_src->psynce != NULL && IFM_TYPE_IS_E1(ifindex))
        {
            memset(cmd, 0, 128);
            vty_out(vty, "interface %s%s", ifname, VTY_NEWLINE);
            sprintf(cmd, " synce priority %d%s", pclk_src->psynce->priority, VTY_NEWLINE);
            vty_out(vty, "%s", cmd);
            vty_out(vty, "!%s", VTY_NEWLINE);
        }

    }

    return 0;

}
/************************************************
 * Function: synce_node_config_write
 * Input:    vty : vty指针
 * Output:   none
 * Return:   none
 * Description:  synce节点下的配置保存处理
 ************************************************/
static int synce_stm_if_config_write(struct vty *vty)
{
    char ifname[IFNET_NAMESIZE];
    int i, ret;
    uint32_t ifindex;
    sint8 cmd[128];
    struct clock_src_t *pclk_src = NULL;

    for (i = 0; i < CLOCK_SRC_NUM_MAX; i++)
    {
        pclk_src = pclock_src[i];

        if (pclk_src == NULL)
        {
            continue;
        }

        ifindex = pclk_src->ifindex;

        ret = ifm_get_name_by_ifindex(ifindex, ifname);

        if (ret < 0)
        {
            continue;
        }

        if (pclk_src->psynce != NULL && IFM_TYPE_IS_STM(ifindex))
        {
            memset(cmd, 0, 128);
            vty_out(vty, "interface %s%s", ifname, VTY_NEWLINE);
            sprintf(cmd, " synce priority %d%s", pclk_src->psynce->priority, VTY_NEWLINE);
            vty_out(vty, "%s", cmd);
            vty_out(vty, "!%s", VTY_NEWLINE);
        }

    }

    return 0;

}

/************************************************
 * Function: synce_cmd_init
 * Input:    none
 * Output:   none
 * Return:   none
 * Description:  synce相关命令行注册
 ************************************************/
void synce_cmd_init(void)
{
    install_node(&synce_node, synce_node_config_write);
    install_default(SYNCE_NODE);

    install_node(&physical_if_node, synce_physical_if_config_write);
    install_default(PHYSICAL_IF_NODE);

    install_node(&tdm_if_node, synce_tdm_if_config_write);
    install_default(TDM_IF_NODE);

    install_node(&e1if_node, synce_e1_if_config_write);
    install_default(E1_IF_NODE);
    
    install_node(&stm_if_node, synce_stm_if_config_write);
    install_default(STM_IF_NODE);

    install_element(CONFIG_NODE, &physical_ethernet_if_common_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &physical_gigabit_ethernet_if_common_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &physical_xgigabit_ethernet_if_common_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &physical_tdm_if_common_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &physical_stm_if_common_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &e1_if_common_cmd, CMD_SYNC);

    install_element(CONFIG_NODE, &synce_enable_cmd, CMD_SYNC);
    install_element(SYNCE_NODE, &synce_mode_select_cmd, CMD_SYNC);
    install_element(SYNCE_NODE, &synce_ssm_enable_cmd, CMD_SYNC);
    install_element(SYNCE_NODE, &synce_ssm_interval_cmd, CMD_SYNC);
    install_element(SYNCE_NODE, &synce_preempt_enable_cmd, CMD_SYNC);
    install_element(SYNCE_NODE, &synce_failback_wtr_cmd, CMD_SYNC);
    install_element(SYNCE_NODE, &synce_holdtime_cmd, CMD_SYNC);
    install_element(SYNCE_NODE, &synce_freqoffset_cmd, CMD_SYNC);
    install_element(SYNCE_NODE, &no_synce_mode_select_cmd, CMD_SYNC);
    install_element(SYNCE_NODE, &no_synce_ssm_enable_cmd, CMD_SYNC);
    install_element(SYNCE_NODE, &no_synce_ssm_interval_cmd, CMD_SYNC);
    install_element(SYNCE_NODE, &no_synce_preempt_enable_cmd, CMD_SYNC);
    install_element(SYNCE_NODE, &no_synce_failback_wtr_cmd, CMD_SYNC);
    install_element(SYNCE_NODE, &no_synce_holdtime_cmd, CMD_SYNC);
    install_element(SYNCE_NODE, &no_synce_freqoffset_cmd, CMD_SYNC);
    install_element(SYNCE_NODE, &synce_clk_source_set_cmd, CMD_SYNC);
    install_element(SYNCE_NODE, &synce_eth_source_set_cmd, CMD_SYNC);

    install_element(PHYSICAL_IF_NODE, &synce_clock_priority_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_synce_clock_priority_cmd, CMD_SYNC);

    install_element(TDM_IF_NODE, &synce_clock_priority_cmd, CMD_SYNC);
    install_element(TDM_IF_NODE, &no_synce_clock_priority_cmd, CMD_SYNC);
    install_element(E1_IF_NODE, &synce_clock_priority_cmd, CMD_SYNC);
    install_element(E1_IF_NODE, &no_synce_clock_priority_cmd, CMD_SYNC);
    install_element(STM_IF_NODE, &synce_clock_priority_cmd, CMD_SYNC);
    install_element(STM_IF_NODE, &no_synce_clock_priority_cmd, CMD_SYNC);

    install_element(SYNCE_NODE, &synce_show_config_cmd, CMD_LOCAL);
    install_element(SYNCE_NODE, &synce_show_source_cmd, CMD_LOCAL);
    install_element(PHYSICAL_IF_NODE, &synce_show_ssm_pkt_cmd, CMD_LOCAL);


    install_element(CONFIG_NODE, &synce_show_config_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &synce_show_source_cmd, CMD_LOCAL);
    
    return;
}



