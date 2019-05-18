/******************************************************************************
 * Filename: sdhmgt_cmd.c
 *  Copyright (c) 2016-2016 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 * Functional description:
 *
 * History:
 *
******************************************************************************/

#include <string.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/inet_ip.h>
#include <lib/memory.h>
#include <lib/memtypes.h>

#include "sdhmgt.h"
#include "sdhmgt_cmd.h"

extern uint32_t   g_sdhmgt_debug;
extern int        syslog_debug;

/************************* debug cmd *************************/
DEFUN(debug_sdhmgt_packet_enable_fun,
      debug_sdhmgt_packet_enable_cmd,
      "debug sdhmgt packet enable",
      "Debug control\n"
      "sdhmgt module\n"
      "packet debug\n"
      "enable\n")
{
    if (0 == syslog_debug)
    {
        vty_error_out(vty, "sdhmgt debug switch is off, please turn on first!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    g_sdhmgt_debug = 1;
    vty_out(vty, "Enable debug sdhmgt msdh packet!%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}

DEFUN(debug_sdhmgt_packet_disable_fun,
      debug_sdhmgt_packet_disable_cmd,
      "debug sdhmgt packet disable",
      "Debug control\n"
      "sdhmgt module\n"
      "packet debug\n"
      "disable\n")
{
    g_sdhmgt_debug = 0;
    vty_out(vty, "Disable debug sdhmgt msdh packet!%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}

DEFUN(show_debug_sdhmgt_packet_fun,
      show_debug_sdhmgt_packet_cmd,
      "show debug sdhmgt packet",
      SHOW_STR
      "Debug control\n"
      "sdhmgt module\n"
      "packet debug\n")
{
    vty_out(vty, "SDHMGT: %s", VTY_NEWLINE);

    if (1 == g_sdhmgt_debug)
    {
        vty_out(vty, "  debug msdh packet: enable %s", VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "  debug msdh packet: disable %s", VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}

void sdhmgt_cmd_init(void)
{
    /* debug cmd */
    install_element(CONFIG_NODE, &debug_sdhmgt_packet_enable_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &debug_sdhmgt_packet_disable_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_debug_sdhmgt_packet_cmd, CMD_LOCAL);
}


