/* RIP debug routines
 * Copyright (C) 1999 Kunihiro Ishiguro <kunihiro@zebra.org>
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <zebra.h>
#include "lib/log.h"
#include "lib/memory.h"
#include "command.h"
#include "ripd/rip_debug.h"

/* For debug statement. */
unsigned long rip_debug_event = 0;
unsigned long rip_debug_packet = 0;

/* For debug statement. */
unsigned long ripng_debug_event = 0;
unsigned long ripng_debug_packet = 0;

extern void zlog_debug_set(struct vty *vty, unsigned int type, int enable);


DEFUN(show_debugging_ripng,
      show_debugging_ripng_cmd,
      "show ripng debug",
      SHOW_STR
      DEBUG_STR      
      RIPNG_STR)
{
    vty_out(vty, "RIPng debugging status:%s", VTY_NEWLINE);

    if (IS_RIPNG_DEBUG_EVENT)
        vty_out(vty, "  RIPng event debugging is on%s", VTY_NEWLINE);

    if (IS_RIPNG_DEBUG_PACKET)
    {
        vty_out(vty, "  RIPng packet debugging is on%s",
                VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}

DEFUN(debug_ripng_events,
      debug_ripng_events_cmd,
      "debug ripng fsm",
      DEBUG_STR
	  RIPNG_STR
      "Debug option set for ripng fsm\n")
{
    ripng_debug_event = RIPNG_DEBUG_EVENT;
    return CMD_WARNING;
}

DEFUN(debug_ripng_packet,
      debug_ripng_packet_cmd,
      "debug ripng packet",
      DEBUG_STR
      RIPNG_STR
      "Debug option set for ripng packet\n")
{
    ripng_debug_packet = RIPNG_DEBUG_PACKET;
    return CMD_SUCCESS;
}

DEFUN(no_debug_ripng_events,
      no_debug_ripng_events_cmd,
      "no debug ripng fsm",
      NO_STR
      DEBUG_STR
      RIPNG_STR
      "Debug option set for ripng fsm\n")
{
    ripng_debug_event = 0;
    return CMD_SUCCESS;
}

DEFUN(no_debug_ripng_packet,
      no_debug_ripng_packet_cmd,
      "no debug ripng packet",
      NO_STR
      DEBUG_STR
      RIPNG_STR
      "Debug option set for ripng packet\n")
{
    ripng_debug_packet = 0;
    return CMD_SUCCESS;
}

DEFUN(show_debugging_rip,
      show_debugging_rip_cmd,
      "show rip debug",
      SHOW_STR
      RIP_STR
      DEBUG_STR)
{
    vty_out(vty, "RIP debugging status:%s", VTY_NEWLINE);

    
    vty_out(vty, "  RIP fsm debugging is on%s", VTY_NEWLINE);

    vty_out(vty, "  RIP packet debugging is on%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}

DEFUN(debug_rip_events,
      debug_rip_events_cmd,
      "debug rip fsm",
      DEBUG_STR
      RIP_STR
      "RIP function status machine\n")
{
    rip_debug_event = RIP_DEBUG_EVENT;
    return CMD_WARNING;
}

DEFUN(debug_rip_packet,
      debug_rip_packet_cmd,
      "debug rip packet",
      DEBUG_STR
      RIP_STR
      "RIP packet\n")
{
    rip_debug_packet = RIP_DEBUG_PACKET;
    return CMD_SUCCESS;
}

DEFUN(no_debug_rip_events,
      no_debug_rip_events_cmd,
      "no debug rip fsm",
      NO_STR
      DEBUG_STR
      RIP_STR
      "RIP function status machine\n")
{
    rip_debug_event = 0;
    return CMD_SUCCESS;
}

DEFUN(no_debug_rip_packet,
      no_debug_rip_packet_cmd,
      "no debug rip packet",
      NO_STR
      DEBUG_STR
      RIP_STR
      "RIP packet\n")
{
    rip_debug_packet = 0;
    return CMD_SUCCESS;
}

void
rip_debug_reset(void)
{
    rip_debug_event = 0;
    rip_debug_packet = 0;
    ripng_debug_event = 0;
    ripng_debug_packet = 0;
}


/********************************************** debug ***********************************************/

const struct message rip_debug_name[] =
{
	{.key = RIP_DBG_EVENT,	  		.str = "event"},
	{.key = RIP_DBG_PACKET,	  		.str = "packet"},
	{.key = RIPNG_DBG_EVENT,	  	.str = "v6-event"},
	{.key = RIPNG_DBG_PACKET,	  	.str = "v6-packet"},
	{.key = RIPNG_DBG_ZEBRA,	  	.str = "v6-zebra"},
	{.key = RIP_DBG_ALL,	  		.str = "all"},
	{.key = RIP_DBG_V6_ALL,	  		.str = "v6-all"}
};


DEFUN (rip_debug_monitor,
  rip_debug_monitor_cmd,
  "debug rip (enable|disable) (event|packet|v6-event|v6-packet|v6-zebra|all|v6-all)",
  "Debug information to moniter\n"
  "Programe name\n"
  "Enable statue\n"
  "Disatble statue\n"
  "Event debug messege\n"
  "Packet debug messege\n"
  "ripng event debug messege\n"
  "ripng packet debug messege\n"
  "ripng zebra debug messege\n"
  "rip all debug messege\n"
  "ripng all debug messege\n")
{
  u_int32_t zlog_num = 0;

  for(zlog_num = 0; zlog_num < array_size(rip_debug_name); zlog_num++)
  {
	  if(!strncmp(argv[1], rip_debug_name[zlog_num].str, 3))
	  {
		  zlog_debug_set( vty, rip_debug_name[zlog_num].key, !strncmp(argv[0], "enable", 3));

		  return CMD_SUCCESS;
	  }
  }

  vty_out (vty, "No debug typd find %s", VTY_NEWLINE);

  return CMD_SUCCESS;
}

/* 显示个debug信息的状态	*/
DEFUN (show_rip_debug_monitor,
  show_rip_debug_monitor_cmd,
  "show rip debug",
  SHOW_STR
  "Programe name\n"
  "Debug status\n")
{
  u_int32_t type_num;

  vty_out(vty, "debug type		   status %s", VTY_NEWLINE);

  for(type_num = 0; type_num < array_size(rip_debug_name); ++type_num)
  {
	  vty_out(vty, "%-15s	 %-10s %s", rip_debug_name[type_num].str, 
		  !!(vty->monitor & (1 << type_num)) ? "enable" : "disable", VTY_NEWLINE);
  }

  return CMD_SUCCESS;
}

/*******************************************************************************************************/


void
rip_debug_init(void)
{

    rip_debug_event = 0;
    rip_debug_packet = 0;
    ripng_debug_event = 0;
    ripng_debug_packet = 0;

#if 0
    install_element(CONFIG_NODE, &show_debugging_rip_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &debug_rip_events_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &debug_rip_packet_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &no_debug_rip_events_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &no_debug_rip_packet_cmd, CMD_SYNC);

    install_element(CONFIG_NODE, &show_debugging_ripng_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &debug_ripng_events_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &debug_ripng_packet_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &no_debug_ripng_events_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &no_debug_ripng_packet_cmd, CMD_SYNC);
#endif

	install_element(CONFIG_NODE, &show_rip_debug_monitor_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &rip_debug_monitor_cmd, CMD_SYNC);
}
