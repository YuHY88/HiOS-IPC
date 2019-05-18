/******************************************************************************
 * Filename: clock_cmd.c
 *  Copyright (c) 2016 - 2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description: command functions for clock interface.
 *
 * History:
 * 2016.10.13  cuiyudong created
 *
******************************************************************************/


#include <string.h>
#include <stdlib.h>

#include "lib/command.h"
#include "lib/hash1.h"
#include "lib/linklist.h"
#include "lib/ifm_common.h"
#include "lib/memtypes.h"
#include "lib/memory.h"
#include "lib/log.h"
#include "lib/pkt_buffer.h"
#include "lib/msg_ipc.h"

#include "clock_if.h"
#include "clock_cmd.h"
#include "clock_src.h"
#include "synce/synce.h"
#include "synce/synce_ssm.h"
#include "synce/synce_cmd.h"
const  char *clk_ql_string[16] = {"unk", "unk", "prc", "unk", "ssua", "unk", "unk", "unk",
                                  "ssub", "unk", "unk", "sec", "unk", "unk", "unk", "dnu",
                                 };

const  char *clk_sa_string[16] = {"sa8", "sa7", "sa6", "sa5", "sa4", "sa4", "sa4", "sa4",
                                  "sa4", "sa4", "sa4", "sa4", "sa4", "sa4", "sa4", "sa4",
                                 };
/********************************************** debug ***********************************************/

const struct message clock_debug_name[] =
{
	{.key = CLOCK_DBG_COMMON,		.str = "common"},
	{.key = CLOCK_DBG_EVENT,		.str = "event"},
	{.key = CLOCK_DBG_INFO,		    .str = "info"},
	{.key = CLOCK_DBG_PACKAGE,		.str = "package"},
	{.key = CLOCK_DBG_ERROR,		.str = "error"},
	{.key = CLOCK_DBG_ALL,		    .str = "all"}
};

extern void zlog_debug_set(struct vty *vty, unsigned int type, int enable);

DEFUN (clock_debug_monitor,
	clock_debug_monitor_cmd,
	"debug clock (enable|disable) (common|event|info|package|error|all)",
	"Debug information to moniter\n"
	"Programe name\n"
	"Enable statue\n"
	"Disatble statue\n"
	"Type name of common messege\n"
	"Type name of event messege\n"
	"Type name of info messege\n"
	"Type name of package messege \n"
	"Type name of error messege \n"
	"Type name of all debug\n")
{
	unsigned int zlog_num;

	for(zlog_num = 0; zlog_num < array_size(clock_debug_name); zlog_num++)
	{
		if(!strncmp(argv[1], clock_debug_name[zlog_num].str, 3))
		{
			zlog_debug_set( vty, clock_debug_name[zlog_num].key, !strncmp(argv[0], "enable", 3));

			return CMD_SUCCESS;
		}
	}

	vty_out (vty, "No debug typd find %s", VTY_NEWLINE);

	return CMD_SUCCESS;
}

/* 显示个debug信息的状态  */
DEFUN (show_clock_debug_monitor,
	show_clock_debug_monitor_cmd,
	"show debug clock",
	SHOW_STR
	"Syslog"
	"Debug status\n")
{
	unsigned int type_num;

	vty_out(vty, "debug type         status %s", VTY_NEWLINE);

	for(type_num = 0; type_num < array_size(clock_debug_name); ++type_num)
	{
		vty_out(vty, "%-15s    %-10s %s", clock_debug_name[type_num].str, 
			!!(vty->monitor & (1 << type_num)) ? "enable" : "disable", VTY_NEWLINE);
	}

	return CMD_SUCCESS;
}


/*******************************************************************************************************/

DEFUN(clock_mode_func,
      clock_mode_cmd,
      "mode (2mb|2mh)",
      "Clock mode\n"
      "Set clock 2Mbis mode \n"
      "Set clock 2MH mode \n"
     )
{
    int ifindex;
    uint8_t id = 0;
    enum CLOCKIF_MODE mode;
    struct clockif_t *pclk;

    ifindex = (uint32_t) vty->index;
    id = IFM_PORT_ID_GET(ifindex);
    pclk = clockif_lookup(id);

    if (NULL == pclk || pclk->priority == 0)
    {
        vty_warning_out(vty, "%% set failed! need enable this port through set priority  %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (!strncmp(argv[0], "2mb", 3))
    {
        mode = CLOCKIF_MODE_2MBIT;
    }
    else if (!strncmp(argv[0], "2mh", 3))
    {
        mode = CLOCKIF_MODE_2MHZ;
    }
    else
    {
        return CMD_WARNING;
    }

    clockif_set_info(pclk, &mode, CLOCKIF_INFO_MODE);
    return CMD_SUCCESS;
}
DEFUN(no_clock_mode_func,
      no_clock_mode_cmd,
      "no mode",
      NO_STR
      "Set clock mode default\n"
     )
{
    int ifindex;
    uint8_t id = 0;
    enum CLOCKIF_MODE mode;
    struct clockif_t *pclk;

    ifindex = (uint32_t) vty->index;
    id = IFM_PORT_ID_GET(ifindex);
    pclk = clockif_lookup(id);

    if (NULL == pclk || pclk->priority == 0)
    {
        vty_warning_out(vty, "%% set failed! need enable this port through set priority  %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    mode = CLOCKIF_MODE_2MHZ;
    clockif_set_info(pclk, &mode, CLOCKIF_INFO_MODE);
    return CMD_SUCCESS;
}

DEFUN(clock_signal_func,
      clock_signal_cmd,
      "signal (in|out|in-out)",
      "Clock signal set\n"
      "Set clock signal in mode\n"
      "Set clock signal out mode\n"
      "Set clock signal in-out mode\n"
     )
{

    int ifindex;
    uint8_t id = 0;
    enum CLOCKIF_SIGNAL singal_type;
    struct clockif_t *pclk = NULL;

    ifindex = (uint32_t) vty->index;
    id = IFM_PORT_ID_GET(ifindex);
    pclk = clockif_lookup(id);

    if (NULL == pclk || pclk->priority == 0)
    {
        vty_warning_out(vty, "%% set failed! need enable this port through set priority  %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (!strcmp(argv[0], "in"))
    {
        singal_type = CLOCKIF_SIGNAL_IN;
    }
    else if (!strcmp(argv[0], "out"))
    {
        singal_type = CLOCKIF_SIGNAL_OUT;
    }
    else if (!strcmp(argv[0], "in-out"))
    {
        singal_type = CLOCKIF_SIGNAL_INOUT;
    }
    else
    {
        return CMD_WARNING;
    }

    clockif_set_info(pclk, &singal_type, CLOCKIF_INFO_SIGNAL);
    return CMD_SUCCESS;
}

DEFUN(no_clock_signal_func,
      no_clock_signal_cmd,
      "no signal",
      NO_STR
      "Set clock signal default mode\n"
     )
{

    int ifindex;
    uint8_t id = 0;
    enum CLOCKIF_SIGNAL singal_type;
    struct clockif_t *pclk = NULL;

    ifindex = (uint32_t) vty->index;
    id = IFM_PORT_ID_GET(ifindex);
    pclk = clockif_lookup(id);

    if (NULL == pclk || pclk->priority == 0)
    {
        vty_warning_out(vty, "%% set failed! need enable this port through set priority  %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    singal_type = CLOCKIF_SIGNAL_INVALID;
    clockif_set_info(pclk, &singal_type, CLOCKIF_INFO_SIGNAL);
    return CMD_SUCCESS;
}


DEFUN(clock_sa_sel_func,
      clock_sa_sel_cmd,
      "sa (sa4|sa5|sa6|sa7|sa8)",
      "Clock sa channel set\n"
      "Set sa channel is sa4\n"
      "Set sa channel is sa5\n"
      "Set sa channel is sa6\n"
      "Set sa channel is sa7\n"
      "Set sa channel is sa8\n"
     )
{

    int ifindex;
    uint8_t id = 0;
    unsigned char sa = 0;
    struct clockif_t *pclk = NULL;

    ifindex = (uint32_t) vty->index;
    id = IFM_PORT_ID_GET(ifindex);
    pclk = clockif_lookup(id);

    if (NULL == pclk || pclk->priority == 0)
    {
        vty_warning_out(vty, "%% set failed! need enable this port through set priority  %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (!strcmp(argv[0], "sa4"))
    {
        sa = 4;
    }
    else if (!strcmp(argv[0], "sa5"))
    {
        sa = 3;
    }
    else if (!strcmp(argv[0], "sa6"))
    {
        sa = 2;
    }
    else if (!strcmp(argv[0], "sa7"))
    {
        sa = 1;
    }
    else if (!strcmp(argv[0], "sa8"))
    {
        sa = 0;
    }
    else
    {
        vty_warning_out(vty, "%% set failed!  sa num : %s  %s", argv[0], VTY_NEWLINE);
        return CMD_WARNING;
    }

    clockif_set_info(pclk, &sa, CLOCKIF_INFO_SA_SEL);
    return CMD_SUCCESS;
}
DEFUN(no_clock_sa_sel_func,
      no_clock_sa_sel_cmd,
      "no sa",
      NO_STR
      "Set clock sa channel default mode\n"
     )
{

    int ifindex;
    uint8_t id = 0;
    uint8_t sa;
    struct clockif_t *pclk = NULL;

    ifindex = (uint32_t) vty->index;
    id = IFM_PORT_ID_GET(ifindex);
    pclk = clockif_lookup(id);

    if (NULL == pclk || pclk->priority == 0)
    {
        vty_warning_out(vty, "%% set failed! need enable this port through set priority  %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    sa = 4;
    clockif_set_info(pclk, &sa, CLOCKIF_INFO_SA_SEL);
    return CMD_SUCCESS;
}


DEFUN(clock_quality_level_func,
      clock_quality_level_cmd,
      "synce quality-level (dnu|prc|sec|ssua|ssub|unk)",
      "Set synce function\n"
      "Clock quality-level\n"
      "QL value is DNU\n"
      "QL value is PRC\n"
      "QL value is SEC\n"
      "QL value is SSU-A\n"
      "QL value is SSU-B\n"
      "QL value is UNK\n"
     )
{
    int ifindex;
    uint8_t id = 0, ql = 0;
    struct clockif_t *pclk = NULL;

    ifindex = (uint32_t) vty->index;
    id = IFM_PORT_ID_GET(ifindex);
    pclk = clockif_lookup(id);

    if (NULL == pclk || pclk->priority == 0)
    {
        vty_warning_out(vty, "%% set failed! need enable this port through set priority  %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (pclk->mode != CLOCKIF_MODE_2MBIT)
    {
        vty_warning_out(vty, "%% set failed! the ql setting is needed by mode of 2Mbits %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (!strcmp(argv[0], "dnu"))
    {
        ql = SSM_DNU;
    }
    else if (!strcmp(argv[0], "prc"))
    {
        ql = SSM_PRC;
    }
    else if (!strcmp(argv[0], "sec"))
    {
        ql = SSM_SEC;
    }
    else if (!strcmp(argv[0], "ssua"))
    {
        ql = SSM_SSUA;
    }
    else if (!strcmp(argv[0], "ssub"))
    {
        ql = SSM_SSUB;
    }
    else if (!strcmp(argv[0], "unk"))
    {
        ql = SSM_UNK;
    }
    else
    {
        return CMD_WARNING;
    }

    pclk->ql_set_flag = 1;

    clockif_set_info(pclk, &ql, CLOCKIF_INFO_QLEVEL);
    //synce_select_clock(SYNCE_EVENT_SEL_IF_DOWN);

    return CMD_SUCCESS;

}

DEFUN(no_clock_quality_level_func,
      no_clock_quality_level_cmd,
      "no synce quality-level",
      NO_STR
      "Set clock quality-level default\n"
     )
{
    int ifindex;
    uint8_t id = 0, ql = 0;
    struct clockif_t *pclk = NULL;

    ifindex = (uint32_t) vty->index;
    id = IFM_PORT_ID_GET(ifindex);
    pclk = clockif_lookup(id);

    if (NULL == pclk || pclk->priority == 0)
    {
        vty_warning_out(vty, "%% set failed! need enable this port through set priority  %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    pclk->ql_set_flag = 0;
    clockif_set_info(pclk, &ql, CLOCKIF_INFO_QLEVEL);

    return CMD_SUCCESS;
}

DEFUN(synce_priority_func,
      clock_priority_cmd,
      "synce priority <1-8>",
      "Set synce function\n"
      "Clock priority set\n"
      "Priority value\n"
     )
{
    int ifindex;
    uint8_t pri;
    struct synce_clk_t clk_src_data;
    ifindex = (uint32_t) vty->index;
    pri = atoi(argv[0]);
    struct clock_src_t  *pclk_src = NULL;
    struct synce_if     *psynce_if = NULL;
    struct clockif_t *pclk = NULL;
    uint8_t id = 1;
    pclk_src = clock_src_lookup(ifindex);

    if (pclk_src == NULL)
    {
        pclk_src = clock_src_create(ifindex);

        if (NULL == pclk_src)
        {
            zlog_err("%s[%d]:leave %s:error:fail to create clock src table bucket\n", __FILE__, __LINE__, __func__);
            return CMD_WARNING;
        }

        if (clock_src_add(pclk_src))
        {
            zlog_err("%s[%d]:leave %s:error:fail to add clock src table bucket\n", __FILE__, __LINE__, __func__);
            return CMD_WARNING;
        }
    }

    id = (uint8_t)IFM_PORT_ID_GET(ifindex);
    pclk = clockif_lookup(id);

    if (NULL == pclk)
    {
        pclk = clockif_create(ifindex, id);

        if (NULL == pclk)
        {
            vty_warning_out(vty, "%% creat clock interface failed! %s", VTY_NEWLINE);
            return CMD_WARNING;
        }
        else
        {
            clockif_add(pclk);
            pclk->mode = CLOCK_DEF_MODE;
            pclk->signal = CLOCK_DEF_SIGNAL;
            clockif_set_info(pclk, &pclk->mode, CLOCKIF_INFO_MODE);

            clockif_set_info(pclk, &pclk->signal, CLOCKIF_INFO_SIGNAL);
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
        pclk->priority = pri;

        if (synce_add_clock(&clk_src_data))
        {
            clockif_delete(id);
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
            pclk->priority = pri;

            if (synce_add_clock(&clk_src_data))
            {
                clockif_delete(id);
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
            zlog_err("create interface synce date failed!...%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    return CMD_SUCCESS;
}
DEFUN(no_synce_priority_func,
      no_clock_priority_cmd,
      "no synce priority",
      NO_STR
      "Synce\n"
      "Set port synce function disable\n"
     )
{
    uint32_t ifindex;
    uint8_t  id;
    uint8_t pri = 0;
    struct clockif_t *pclk = NULL;

    ifindex = (uint32_t) vty->index;
    id = (uint8_t)IFM_PORT_ID_GET(ifindex);
    pclk = clockif_lookup(id);

    if (NULL == pclk)
    {
        zlog_err("%s[%d]:leave %s:error:can't find the clockif id=%d\n", __FILE__, __LINE__, __func__, id);
        return CMD_WARNING;
    }

    pclk->priority = pri;
    struct synce_clk_t clk_src_data;
    clk_src_data.ifindex = ifindex;
    clk_src_data.type = SYNCE_CLK_TYPE_CLOCKIF;
    clk_src_data.status = CLOCK_STAT_LOSS;
    synce_delete_clock(&clk_src_data);
    clockif_delete(id);
    clock_src_delete(ifindex);

    return CMD_SUCCESS;
}
static int synce_print_clock_interface_config_node(struct vty *vty)
{
    int i;
    char ql_str[64];
    char sa_str[64];
    struct clockif_t *pclk = NULL;

    for (i = 0; i < CLOCKIF_NUM_MAX; i++)
    {
        pclk = pclkif[i];

        if (pclk == NULL)
        {
            continue;
        }

        memset(ql_str, 0, 64);
        synce_ql_val_to_str(ql_str, pclk->ssm_level);
        synce_sa_val_to_str(sa_str, pclk->sa);
        vty_out(vty, "the interface id is %d%s", pclk->id, VTY_NEWLINE);
        vty_out(vty, "------------------------------------------------------------%s", VTY_NEWLINE);
        vty_out(vty, "clock interface mode :  \t\t%s%s", CLOCK_MODE_DISPLAY(pclk->mode), VTY_NEWLINE);
        vty_out(vty, "clock interface synce priority :\t%d%s", pclk->priority, VTY_NEWLINE);
        vty_out(vty, "clock interface signal drection :  \t%s%s", CLOCK_SIGNAL_DISPLAY(pclk->signal), VTY_NEWLINE);
        vty_out(vty, "clock interface ssm level :  \t\t%s%s", ql_str, VTY_NEWLINE);
        vty_out(vty, "clock interface sa channel select :  \t%s%s", sa_str, VTY_NEWLINE);
        vty_out(vty, "clock interface status :  \t\t%s%s", CLOCK_IFM_STATE_DISPLAY(pclk->status), VTY_NEWLINE);
        vty_out(vty, "------------------------------------------------------------%s", VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}
static int synce_print_clock_interface_clock_node(struct vty *vty, int id)
{
    struct clockif_t *pclk = NULL;
    uint8_t num = 0;
    char ql_str[64];
    char sa_str[64];

    if (id < 1 || id > CLOCKIF_NUM_MAX)
    {
        vty_warning_out(vty, "%% the clock interface id %d is not exist!%s", id, VTY_NEWLINE);
        return CMD_WARNING;
    }

    num = (uint8_t)id;
    pclk = clockif_lookup(num);

    if (NULL == pclk)
    {
        vty_warning_out(vty, "%% the clock interface need enable through setting priority!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memset(ql_str, 0, 64);
    synce_ql_val_to_str(ql_str, pclk->ssm_level);
    synce_sa_val_to_str(sa_str, pclk->sa);
    vty_out(vty, "the interface id is %d%s", pclk->id, VTY_NEWLINE);
    vty_out(vty, "------------------------------------------------------------%s", VTY_NEWLINE);
    vty_out(vty, "clock interface mode :  \t\t%s%s", CLOCK_MODE_DISPLAY(pclk->mode), VTY_NEWLINE);
    vty_out(vty, "clock interface synce priority :\t%d%s", pclk->priority, VTY_NEWLINE);
    vty_out(vty, "clock interface signal drection : \t%s%s", CLOCK_SIGNAL_DISPLAY(pclk->signal), VTY_NEWLINE);
    vty_out(vty, "clock interface ssm level :  \t\t%s%s", ql_str, VTY_NEWLINE);
    vty_out(vty, "clock interface sa channel select :  \t%s%s", sa_str, VTY_NEWLINE);
    vty_out(vty, "clock interface status :  \t\t%s%s", CLOCK_IFM_STATE_DISPLAY(pclk->status), VTY_NEWLINE);
    vty_out(vty, "------------------------------------------------------------%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}

DEFUN(show_clock_interface_func,
      show_clock_interface_cmd,
      "show interface clock [<1-2>]",
      SHOW_STR
      "Interface\n"
      "Show clock config information\n"
      "Clock interface value\n"
     )
{
    int id;
    uint32_t ifindex;

    if (vty->node == CONFIG_NODE)
    {
        if (argv[0] == NULL)
        {
            synce_print_clock_interface_config_node(vty);
        }
        else
        {
            id = atoi(argv[0]);
            synce_print_clock_interface_clock_node(vty, id);
        }
    }
    else if (vty->node == CLOCK_NODE)
    {
        if (NULL == argv[0])
        {
            ifindex = (uint32_t) vty->index;
            id = (uint8_t)IFM_PORT_ID_GET(ifindex);
            synce_print_clock_interface_clock_node(vty, id);
        }
        else
        {
            vty_warning_out(vty, "%% the command format is 'show interface clock'!%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    return CMD_SUCCESS;
}
static int clock_config_write(struct vty *vty)
{
    int id, ret;
    struct clockif_t *pclk;
    char ifname[IFNET_NAMESIZE];
    const char *mode_str[] = {"2mh", "2mb", "2mh"};
    const char *signal_str[] = {"in-out", "in", "out", "in-out"};

    for (id = 1; id <= CLOCKIF_NUM_MAX; id++)
    {
        pclk = clockif_lookup(id);

        if (pclk == NULL)
        {
            continue;
        }

        ret = ifm_get_name_by_ifindex(pclk->ifindex, ifname);

        if (ret < 0)
        {
            continue;
        }

        vty_out(vty, "interface %s%s", ifname, VTY_NEWLINE);

        vty_out(vty, " synce priority %d%s", pclk->priority, VTY_NEWLINE);

        if (pclk->mode != CLOCK_DEF_MODE)
        {
            vty_out(vty, " mode %s%s", mode_str[pclk->mode], VTY_NEWLINE);
        }

        vty_out(vty, " sa %s%s", clk_sa_string[pclk->sa], VTY_NEWLINE);

        if (pclk->signal != CLOCK_DEF_SIGNAL)
        {
            vty_out(vty, " signal %s%s", signal_str[pclk->signal], VTY_NEWLINE);
        }

        if (pclk->ssm_level != CLOCK_DEF_CLK_QL)
        {
            vty_out(vty, " synce quality-level %s%s", clk_ql_string[pclk->ssm_level], VTY_NEWLINE);
        }
    }

    return 0;
}

void clock_cmd_init(void)
{
    install_node(&clockif_node, clock_config_write);
    install_default(CLOCK_NODE);

    install_element(CONFIG_NODE, &clock_if_common_cmd, CMD_SYNC);
    install_element(CLOCK_NODE, &clock_mode_cmd, CMD_SYNC);
    install_element(CLOCK_NODE, &clock_signal_cmd, CMD_SYNC);
    install_element(CLOCK_NODE, &clock_sa_sel_cmd, CMD_SYNC);
    install_element(CLOCK_NODE, &clock_quality_level_cmd, CMD_SYNC);
    install_element(CLOCK_NODE, &clock_priority_cmd, CMD_SYNC);

    install_element(CLOCK_NODE, &no_clock_mode_cmd, CMD_SYNC);
    install_element(CLOCK_NODE, &no_clock_signal_cmd, CMD_SYNC);
    install_element(CLOCK_NODE, &no_clock_sa_sel_cmd, CMD_SYNC);
    install_element(CLOCK_NODE, &no_clock_quality_level_cmd, CMD_SYNC);
    install_element(CLOCK_NODE, &no_clock_priority_cmd, CMD_SYNC);
    install_element(CLOCK_NODE, &show_clock_interface_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_clock_interface_cmd, CMD_LOCAL);

	/* new debug */
	install_element (CONFIG_NODE, &clock_debug_monitor_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &show_clock_debug_monitor_cmd, CMD_SYNC);

}


