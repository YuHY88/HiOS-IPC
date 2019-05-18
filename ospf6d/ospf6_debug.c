/*
 * OSPFd dump routine.
 * Copyright (C) 1999, 2000 Toshiaki Takada
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
 * along with GNU Zebra; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <zebra.h>


#include "lib/linklist.h"
#include "lib/thread.h"
#include "lib/prefix.h"
#include "lib/command.h"
#include "lib/stream.h"
#include "lib/log.h"
#include "lib/vty.h"
#include "lib/timer.h"
#include "lib/hptimer.h"
#include "lib/vector.h"

#include "ospf6d/ospf6_debug.h"
#include "ospf6d/ospf6_interface.h"
#include "ospf6d/ospf6_message.h"
#include "ospf6d/ospf6_lsa.h"
#include "ospf6d/ospf6_intra.h"
#include "ospf6d/ospf6_abr.h"
#include "ospf6d/ospf6_asbr.h"
#include "ospf6d/ospf6_neighbor.h"
#include "ospf6d/ospf6_spf.h"
#include "ospf6d/ospf6_flood.h"

DEFUN(debug_ospf6_interface,
	  debug_ospf6_interface_cmd,
	  "debug ospf6 (enable|disable) interface",
	  DEBUG_STR
	  OSPF6_STR
	  "Ospf debug enable\n"
      "Ospf debug disable\n"
	  "Debug OSPF6 Interface\n"
	 )
{
	int enable = 0;
	unsigned int type_new = 0;

	if (argc <= 0)
	{
		return CMD_WARNING;
	}

	if(argv[0][0] == 'e') enable = 1;
	
	type_new = OSPF6_DBG_INTERFACE;
	if(enable)
	{
		OSPF6_DEBUG_INTERFACE_ON();
	}
	else
	{
		OSPF6_DEBUG_INTERFACE_OFF();
	}
	zlog_debug_set(vty, type_new,  enable);
	return CMD_SUCCESS;
}
	 
void
install_element_ospf6_debug_interface(void)
{
	install_element(CONFIG_NODE, &debug_ospf6_interface_cmd, CMD_SYNC);
}

/* Commands */
DEFUN(debug_ospf6_message,
      debug_ospf6_message_cmd,
      "debug ospf6 (enable|disable) packet (unknown|hello|dbdesc|lsreq|lsupdate|lsack|all)",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "Debug OSPF6 packet\n"
      "Debug Unknown message\n"
      "Debug Hello message\n"
      "Debug Database Description message\n"
      "Debug Link State Request message\n"
      "Debug Link State Update message\n"
      "Debug Link State Acknowledgement message\n"
      "Debug All message\n"
     )
{
    unsigned char level = 0;
    int type = 0;
	int enable = 0;
	int debug_flag = 0;
	unsigned int type_new = 0;
    int i;
    assert(argc > 0);

	if(argv[0][0] == 'e') enable = 1;
	
    /* check type */
    if (! strncmp(argv[1], "u", 1))
    {
        type = OSPF6_MESSAGE_TYPE_UNKNOWN;
    }
    else if (! strncmp(argv[1], "h", 1))
    {
        type = OSPF6_MESSAGE_TYPE_HELLO;
    }
    else if (! strncmp(argv[1], "d", 1))
    {
        type = OSPF6_MESSAGE_TYPE_DBDESC;
    }
    else if (! strncmp(argv[1], "lsr", 3))
    {
        type = OSPF6_MESSAGE_TYPE_LSREQ;
    }
    else if (! strncmp(argv[1], "lsu", 3))
    {
        type = OSPF6_MESSAGE_TYPE_LSUPDATE;
    }
    else if (! strncmp(argv[1], "lsa", 3))
    {
        type = OSPF6_MESSAGE_TYPE_LSACK;
    }
    else if (! strncmp(argv[1], "a", 1))
    {
        type = OSPF6_MESSAGE_TYPE_ALL;
    }

    if (argc == 2)
    {
        level = OSPF6_DEBUG_MESSAGE_SEND | OSPF6_DEBUG_MESSAGE_RECV;
		type_new = OSPF6_DBG_MESSAGE_RECV | OSPF6_DBG_MESSAGE_SEND;
    }
    else if (! strncmp(argv[2], "s", 1))
    {
        level = OSPF6_DEBUG_MESSAGE_SEND;
		type_new = OSPF6_DBG_MESSAGE_SEND;
    }
    else if (! strncmp(argv[2], "r", 1))
    {
        level = OSPF6_DEBUG_MESSAGE_RECV;
		type_new = OSPF6_DBG_MESSAGE_RECV;
    }

    if (type == OSPF6_MESSAGE_TYPE_ALL)
    {
        for (i = 0; i < 6; i++)
        {
        	if(enable)
        	{
            	OSPF6_DEBUG_MESSAGE_ON(i, level);
        	}
			else
			{
				OSPF6_DEBUG_MESSAGE_OFF(i, level);
			}
        }
    }
    else
    {
		if(enable)
    	{
        	OSPF6_DEBUG_MESSAGE_ON(type, level);
    	}
		else
		{
			OSPF6_DEBUG_MESSAGE_OFF(type, level);
		}
    }

	if(enable)
    {
		zlog_debug_set(vty, type_new,  enable);
	}
	else
	{
		for (i = 1; i < 6; i++)
	    {
	    	if(conf_debug_ospf6_message[i])
	    	{
				debug_flag = 1;
				break;
			}
	    }
		if(!debug_flag)
		{
			zlog_debug_set(vty, type_new,  enable);
		}
	}

    return CMD_SUCCESS;
}

ALIAS(debug_ospf6_message,
      debug_ospf6_message_sendrecv_cmd,
      "debug ospf6 (enable|disable) packet (unknown|hello|dbdesc|lsreq|lsupdate|lsack|all) (send|recv)",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "Debug OSPF6 packet\n"
      "Debug Unknown message\n"
      "Debug Hello message\n"
      "Debug Database Description message\n"
      "Debug Link State Request message\n"
      "Debug Link State Update message\n"
      "Debug Link State Acknowledgement message\n"
      "Debug All message\n"
      "Debug only sending message\n"
      "Debug only receiving message\n"
     )


void
install_element_ospf6_debug_message(void)
{
	install_element(CONFIG_NODE, &debug_ospf6_message_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &debug_ospf6_message_sendrecv_cmd, CMD_SYNC);
}


DEFUN(debug_ospf6_lsa_type,
      debug_ospf6_lsa_hex_cmd,
      "debug ospf6 (enable|disable) lsa (router|network|inter-prefix|inter-router|as-ext|grp-mbr|type7|link|intra-prefix|unknown)",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "Debug Link State Advertisements (LSAs)\n"
      "Specify LS type is router-lsa\n"
      "Specify LS type is network-lsa\n"
      "Specify LS type is inter-prefix-lsa\n"
      "Specify LS type is inter-router-lsa\n"
      "Specify LS type is as-ext-lsa\n"
      "Specify LS type is grp-mbr-lsa\n"
      "Specify LS type is type7-lsa\n"
      "Specify LS type is link-lsa\n"
      "Specify LS type is intra-prefix-lsa\n"
      "Specify LS type is unknown-lsa\n"
     )
{
    unsigned int i;
	int enable = 0;
	unsigned int type_new = 0;
    struct ospf6_lsa_handler *handler = NULL;
    assert(argc);

	if(argv[0][0] == 'e') enable = 1;
	
    for (i = 0; i < vector_active(ospf6_lsa_handler_vector); i++)
    {
        handler = vector_slot(ospf6_lsa_handler_vector, i);

        if (handler == NULL)
        {
            continue;
        }

        if (strncmp(argv[1], ospf6_lsa_handler_name(handler), strlen(argv[0])) == 0)
        {
            break;
        }

        if (! strcasecmp(argv[1], handler->name))
        {
            break;
        }

        handler = NULL;
    }

    if (handler == NULL)
    {
        handler = &unknown_handler;
    }

    if (argc >= 3)
    {
        if (! strcmp(argv[2], "originate"))
        {
        	if(enable)
        	{
            	SET_FLAG(handler->debug, OSPF6_LSA_DEBUG_ORIGINATE);
        	}
			else
			{
				UNSET_FLAG(handler->debug, OSPF6_LSA_DEBUG_ORIGINATE);
			}
        }

        if (! strcmp(argv[2], "examine"))
        {
			if(enable)
        	{
            	SET_FLAG(handler->debug, OSPF6_LSA_DEBUG_EXAMIN);
        	}
			else
			{
				UNSET_FLAG(handler->debug, OSPF6_LSA_DEBUG_EXAMIN);
			}
        }

        if (! strcmp(argv[2], "flooding"))
        {
			if(enable)
        	{
            	SET_FLAG(handler->debug, OSPF6_LSA_DEBUG_FLOOD);
        	}
			else
			{
				UNSET_FLAG(handler->debug, OSPF6_LSA_DEBUG_FLOOD);
			}
        }
    }
    else
    {
		if(enable)
    	{
        	SET_FLAG(handler->debug, OSPF6_LSA_DEBUG);
    	}
		else
		{
			UNSET_FLAG(handler->debug, OSPF6_LSA_DEBUG);
		}
    }

	type_new = OSPF6_DBG_LSA_DEBUG;
	if(enable)
	{
		zlog_debug_set(vty, type_new,  enable);
	}
	else
	{
		//all lsa debug flag is off
		if((!router_handler.debug) && (!network_handler.debug) && (!link_handler.debug)
			&& (!intra_prefix_handler.debug) && (!inter_prefix_handler.debug) && (!inter_router_handler.debug)
			&& (!as_external_handler.debug) && (!unknown_handler.debug))
		{
			zlog_debug_set(vty, type_new,  enable);
		}
	}

    return CMD_SUCCESS;
}

ALIAS(debug_ospf6_lsa_type,
      debug_ospf6_lsa_hex_detail_cmd,
      "debug ospf6 (enable|disable) lsa (router|network|inter-prefix|inter-router|as-ext|grp-mbr|type7|link|intra-prefix|unknown) (originate|examine|flooding)",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "Debug Link State Advertisements (LSAs)\n"
      "Specify LS type is router-lsa\n"
      "Specify LS type is network-lsa\n"
      "Specify LS type is inter-prefix-lsa\n"
      "Specify LS type is inter-router-lsa\n"
      "Specify LS type is as-ext-lsa\n"
      "Specify LS type is grp-mbr-lsa\n"
      "Specify LS type is type7-lsa\n"
      "Specify LS type is link-lsa\n"
      "Specify LS type is intra-prefix-lsa\n"
      "Specify LS type is unknown-lsa\n"
      "Debug option is originate\n"
      "Debug option is examine\n"
      "Debug option is flooding\n"
     )

void
install_element_ospf6_debug_lsa(void)
{
	install_element(CONFIG_NODE, &debug_ospf6_lsa_hex_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &debug_ospf6_lsa_hex_detail_cmd, CMD_SYNC);
}


DEFUN(debug_ospf6_neighbor,
      debug_ospf6_neighbor_cmd,
      "debug ospf6 (enable|disable) neighbor",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "Debug OSPF6 Neighbor\n"
     )
{
    unsigned char level = 0;
	int enable = 0;
	unsigned int type_new = 0;

	if(argv[0][0] == 'e') enable = 1;
	
    if (argc)
    {
        if (! strncmp(argv[1], "s", 1))
        {
            level = OSPF6_DEBUG_NEIGHBOR_STATE;
        }

        if (! strncmp(argv[1], "e", 1))
        {
            level = OSPF6_DEBUG_NEIGHBOR_EVENT;
        }
    }
    else
    {
        level = OSPF6_DEBUG_NEIGHBOR_STATE | OSPF6_DEBUG_NEIGHBOR_EVENT;
    }

	type_new = OSPF6_DBG_NEIGHBOR;
	
	if(enable)
	{
		OSPF6_DEBUG_NEIGHBOR_ON(level);
		zlog_debug_set(vty, type_new,  enable);
	}
	else
	{
		OSPF6_DEBUG_NEIGHBOR_OFF(level);
		if(!conf_debug_ospf6_neighbor)
		{
			zlog_debug_set(vty, type_new,  enable);
		}
	}
    return CMD_SUCCESS;
}

ALIAS(debug_ospf6_neighbor,
      debug_ospf6_neighbor_detail_cmd,
      "debug ospf6 (enable|disable) neighbor (state|event)",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "Debug OSPF6 Neighbor\n"
      "Debug OSPF6 Neighbor State Change\n"
      "Debug OSPF6 Neighbor Event\n"
     )
     
void
install_element_ospf6_debug_neighbor(void)
{
	install_element(CONFIG_NODE, &debug_ospf6_neighbor_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &debug_ospf6_neighbor_detail_cmd, CMD_SYNC);
}


DEFUN(debug_ospf6_route_sendrecv,
      debug_ospf6_route_sendrecv_cmd,
      "debug ospf6 (enable|disable) msg (send|recv|all)",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "message between route and ospf6"
      "Debug connection between route\n"
      "Debug Sending route\n"
      "Debug Receiving route\n"
      "Debug connection between route send and recv\n"
     )
{
    unsigned char level = 0;
	int enable = 0;
	unsigned int type_new = 0;

	if(argv[0][0] == 'e') enable = 1;

    if (! strncmp(argv[1], "s", 1))
    {
        level = OSPF6_DEBUG_ZEBRA_SEND;
    }
    else if (! strncmp(argv[1], "r", 1))
    {
        level = OSPF6_DEBUG_ZEBRA_RECV;
    }
    else if (! strncmp(argv[1], "a", 1))
    {

        level = OSPF6_DEBUG_ZEBRA_SEND | OSPF6_DEBUG_ZEBRA_RECV;
    }
    else
    {
        vty_error_out(vty,"command error,%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	type_new = OSPF6_DBG_ZEBRA_MSG;
	if(enable)
	{
		OSPF6_DEBUG_ZEBRA_ON(level);
		zlog_debug_set(vty, type_new,  enable);
	}
	else
	{
		OSPF6_DEBUG_ZEBRA_OFF(level);
		if(!conf_debug_ospf6_zebra)
		{
			zlog_debug_set(vty, type_new,  enable);
		}
	}

    return CMD_SUCCESS;
}


void
install_element_ospf6_debug_zebra(void)
{
    install_element(CONFIG_NODE, &debug_ospf6_route_sendrecv_cmd, CMD_SYNC);
}


DEFUN(debug_ospf6_spf_process,
      debug_ospf6_spf_process_cmd,
      "debug ospf6 (enable|disable) spf (process | time | lsdb)",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "Debug SPF Calculation\n"
      "Debug Detailed SPF Process\n"
      "Measure time taken by SPF Calculation\n"
      "Log number of LSAs at SPF Calculation time\n"
     )
{
    unsigned char level = 0;
	int enable = 0;
	unsigned int type_new = 0;

	if(argv[0][0] == 'e') enable = 1;

	if (! strncmp(argv[1], "p", 1))
    {
        level = OSPF6_DEBUG_SPF_PROCESS;
    }
    else if (! strncmp(argv[1], "t", 1))
    {
        level = OSPF6_DEBUG_SPF_TIME;
    }
    else if (! strncmp(argv[1], "l", 1))
    {

        level = OSPF6_DEBUG_SPF_DATABASE;
    }
    else
    {
        vty_error_out(vty,"command error,%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	type_new = OSPF6_DBG_SPF;
	if(enable)
	{
		OSPF6_DEBUG_SPF_ON(level);
		zlog_debug_set(vty, type_new,  enable);
	}
	else
	{
		OSPF6_DEBUG_SPF_OFF(level);
		if(!conf_debug_ospf6_spf)
		{
			zlog_debug_set(vty, type_new,  enable);
		}
	}
	
    return CMD_SUCCESS;
}

void
install_element_ospf6_debug_spf(void)
{
    install_element(CONFIG_NODE, &debug_ospf6_spf_process_cmd, CMD_SYNC);
}

DEFUN(debug_ospf6_route,
      debug_ospf6_route_cmd,
      "debug ospf6 (enable|disable) route (table|intra-area|inter-area|memory)",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "Debug route table calculation\n"
      "Debug detail\n"
      "Debug intra-area route calculation\n"
      "Debug inter-area route calculation\n"
      "Debug route memory use\n"
     )
{
    unsigned char level = 0;
	int enable = 0;
	unsigned int type_new = 0;

	if(argv[0][0] == 'e') enable = 1;

    if (! strncmp(argv[1], "table", 5))
    {
        level = OSPF6_DEBUG_ROUTE_TABLE;
    }
    else if (! strncmp(argv[1], "intra", 5))
    {
        level = OSPF6_DEBUG_ROUTE_INTRA;
    }
    else if (! strncmp(argv[1], "inter", 5))
    {
        level = OSPF6_DEBUG_ROUTE_INTER;
    }
    else if (! strncmp(argv[1], "memor", 5))
    {
        level = OSPF6_DEBUG_ROUTE_MEMORY;
    }
	else
    {
        vty_error_out(vty,"command error,%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	type_new = OSPF6_DBG_ROUTE;
	if(enable)
	{
		OSPF6_DEBUG_ROUTE_ON(level);
		zlog_debug_set(vty, type_new,  enable);
	}
	else
	{
		OSPF6_DEBUG_ROUTE_OFF(level);
		if(!conf_debug_ospf6_route)
		{
			zlog_debug_set(vty, type_new,  enable);
		}
	}
    return CMD_SUCCESS;
}

void
install_element_ospf6_debug_route(void)
{
    install_element(CONFIG_NODE, &debug_ospf6_route_cmd, CMD_SYNC);
}


DEFUN(debug_ospf6_brouter,
      debug_ospf6_brouter_cmd,
      "debug ospf6 (enable|disable) border-routers",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "Debug border router\n"
     )
{
	int enable = 0;
	unsigned int type_new = 0;

	if(argv[0][0] == 'e') enable = 1;
	
	type_new = OSPF6_DBG_BROUTER;
	if(enable)
	{
		OSPF6_DEBUG_BROUTER_ON();
		zlog_debug_set(vty, type_new,  enable);
	}
	else
	{
		OSPF6_DEBUG_BROUTER_OFF();
		if(!conf_debug_ospf6_brouter)
		{
			zlog_debug_set(vty, type_new,  enable);
		}
	}
    return CMD_SUCCESS;
}

#define DEBUG_ROUTER_ID		(1)
#define DEBUG_AREA_ID		(2)

DEFUN(debug_ospf6_brouter_router,
      debug_ospf6_brouter_router_cmd,
      "debug ospf6 (enable|disable) border-routers (router-id|area-id) {A.B.C.D}",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "Debug border router\n"
      "Debug specific border router\n"
      "Debug border routers in specific Area\n"
      "Specify border-router's router-id or Area-ID\n"
     )
{
    u_int32_t argv_id;
	int enable = 0;
	unsigned int type_new = 0;
	unsigned char level = 0;

	if(argv[0][0] == 'e') enable = 1;

	if (! strncmp(argv[1], "r", 1))
    {
        level = DEBUG_ROUTER_ID;
    }
    else if (! strncmp(argv[1], "a", 1))
    {
        level = DEBUG_AREA_ID;
    }
	else
    {
        vty_error_out(vty,"command error,%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	type_new = OSPF6_DBG_BROUTER;
	
	if(enable)
	{
    	inet_pton(AF_INET, argv[2], &argv_id);
		if(level == DEBUG_ROUTER_ID)
		{
			OSPF6_DEBUG_BROUTER_SPECIFIC_ROUTER_ON(argv_id);
		}
		else if(level == DEBUG_AREA_ID)
		{
			OSPF6_DEBUG_BROUTER_SPECIFIC_AREA_ON(argv_id);
		}
		
		zlog_debug_set(vty, type_new,  enable);
	}
	else
	{
		if(level == DEBUG_ROUTER_ID)
		{
			OSPF6_DEBUG_BROUTER_SPECIFIC_ROUTER_OFF();
		}
		else if(level == DEBUG_AREA_ID)
		{
			OSPF6_DEBUG_BROUTER_SPECIFIC_AREA_OFF();
		}
		
		if(!conf_debug_ospf6_brouter)
		{
			zlog_debug_set(vty, type_new,  enable);
		}
	}
	
    
    return CMD_SUCCESS;
}

void
install_element_ospf6_debug_brouter(void)
{
    install_element(CONFIG_NODE, &debug_ospf6_brouter_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &debug_ospf6_brouter_router_cmd, CMD_SYNC);
}

DEFUN(debug_ospf6_asbr,
      debug_ospf6_asbr_cmd,
      "debug ospf6 (enable|disable) asbr",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "Debug OSPF6 ASBR function\n"
     )
{
	int enable = 0;
	unsigned int type_new = 0;

	if(argv[0][0] == 'e') enable = 1;
	
	type_new = OSPF6_DBG_ASBR;
	if(enable)
	{
		OSPF6_DEBUG_ASBR_ON();		
	}
	else
	{
		OSPF6_DEBUG_ASBR_OFF();
	}
	zlog_debug_set(vty, type_new,  enable);
    
    return CMD_SUCCESS;
}

void
install_element_ospf6_debug_asbr()
{
    install_element(CONFIG_NODE, &debug_ospf6_asbr_cmd, CMD_SYNC);
}

DEFUN(debug_ospf6_abr,
      debug_ospf6_abr_cmd,
      "debug ospf6 (enable|disable) abr",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "Debug OSPF6 ABR function\n"
     )
{
	int enable = 0;
	unsigned int type_new = 0;

	if(argv[0][0] == 'e') enable = 1;
	
	type_new = OSPF6_DBG_ABR;
	if(enable)
	{
		OSPF6_DEBUG_ABR_ON();
	}
	else
	{
		OSPF6_DEBUG_ABR_OFF();
	}
	zlog_debug_set(vty, type_new,  enable);
    return CMD_SUCCESS;
}

void
install_element_ospf6_debug_abr(void)
{
    install_element(CONFIG_NODE, &debug_ospf6_abr_cmd, CMD_SYNC);
}

DEFUN(debug_ospf6_flooding,
      debug_ospf6_flooding_cmd,
      "debug ospf6 (enable|disable) flooding",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "Debug OSPF6 flooding function\n"
     )
{
	int enable = 0;
	unsigned int type_new = 0;

	if(argv[0][0] == 'e') enable = 1;
	
	type_new = OSPF6_DBG_FLOODING;
	if(enable)
	{
		OSPF6_DEBUG_FLOODING_ON();
	}
	else
	{
		OSPF6_DEBUG_FLOODING_OFF();
	}
	zlog_debug_set(vty, type_new,  enable);
    return CMD_SUCCESS;
}

void
install_element_ospf6_debug_flood(void)
{
    install_element(CONFIG_NODE, &debug_ospf6_flooding_cmd, CMD_SYNC);
}

DEFUN(debug_ospf6_other,
      debug_ospf6_other_cmd,
      "debug ospf6 (enable|disable) other",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "Debug OSPF6 no classified, scattered debugging information.\n"
     )
{
	int enable = 0;
	unsigned int type_new = 0;

	if(argv[0][0] == 'e') enable = 1;
	
	type_new = OSPF6_DBG_OTHER;
	zlog_debug_set(vty, type_new,  enable);
    return CMD_SUCCESS;
}
	 
void
install_element_ospf6_debug_other(void)
{
    install_element(CONFIG_NODE, &debug_ospf6_other_cmd, CMD_SYNC);
}
	 