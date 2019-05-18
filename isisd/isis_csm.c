/*
 * IS-IS Rout(e)ing protocol - isis_csm.c
 *                             IS-IS circuit state machine
 * Copyright (C) 2001,2002    Sampo Saaristo
 *                            Tampere University of Technology      
 *                            Institute of Communications Engineering
 *
 * This program is free software; you can redistribute it and/or modify it 
 * under the terms of the GNU General Public Licenseas published by the Free 
 * Software Foundation; either version 2 of the License, or (at your option) 
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for 
 * more details.
 *
 * You should have received a copy of the GNU General Public License along 
 * with this program; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <zebra.h>

#include "log.h"
#include "memory.h"
#include "if.h"
#include "linklist.h"
#include "command.h"
#include "thread.h"
#include "hash.h"
#include "prefix.h"
#include "stream.h"

#include "isisd/dict.h"
#include "isisd/include-netbsd/iso.h"
#include "isisd/isis_constants.h"
#include "isisd/isis_common.h"
#include "isisd/isis_flags.h"
#include "isisd/isis_circuit.h"
#include "isisd/isis_tlv.h"
#include "isisd/isis_lsp.h"
#include "isisd/isis_pdu.h"
#include "isisd/isis_network.h"
#include "isisd/isis_misc.h"
#include "isisd/isis_constants.h"
#include "isisd/isis_adjacency.h"
#include "isisd/isis_dr.h"
#include "isisd/isisd.h"
#include "isisd/isis_csm.h"
#include "isisd/isis_events.h"

extern struct isis *isis;

static const char *csm_statestr[] = {
  "C_STATE_NA",
  "C_STATE_INIT",
  "C_STATE_CONF",
  "C_STATE_UP"
};

#define STATE2STR(S) csm_statestr[S]

static const char *csm_eventstr[] = {
  "NO_STATE",
  "ISIS_ENABLE",
  "IF_UP_FROM_Z",
  "ISIS_DISABLE",
  "IF_DOWN_FROM_Z",
};

#define EVENT2STR(E) csm_eventstr[E]

struct isis_circuit *
isis_csm_state_change (int event, struct isis_circuit *circuit, void *arg)
{
    int old_state;
    int retv;

    old_state = circuit ? circuit->state : C_STATE_NA;
//  	if (isis->debugs & DEBUG_EVENTS|| isis->debugs & ISIS_DEBUG_TYPE_ALL)
//    {
    zlog_debug (ISIS_DEBUG_TYPE_EVENTS,"CSM_EVENT: %s", EVENT2STR (event));
//  	}

  	switch (old_state)
  	{
    case C_STATE_NA:
        if (circuit)
            zlog_debug (ISIS_DEBUG_TYPE_EVENTS,"Non-null circuit while state C_STATE_NA");
        //assert (circuit == NULL);
			if(circuit != NULL)
			{
				zlog_err("%-20s[%d] error: ",__FUNCTION__ ,  __LINE__);
				return NULL;
			}
      		switch (event)
			{
				case ISIS_ENABLE:
	  				circuit = isis_circuit_new ();
	  				isis_circuit_configure (circuit, (struct isis_area *) arg);
	  				circuit->state = C_STATE_CONF;
	  				break;
				case IF_UP_FROM_Z:
	  				circuit = isis_circuit_new ();
	  				isis_circuit_if_add (circuit, (struct interface *) arg);
	  				listnode_add (isis->init_circ_list, circuit);
	  				circuit->state = C_STATE_INIT;
	  				circuit->rcv_flag = 1;
	  				break;
        case ISIS_DISABLE:
            zlog_debug (ISIS_DEBUG_TYPE_EVENTS,"circuit already disabled");
            break;
        case IF_DOWN_FROM_Z:
            zlog_debug (ISIS_DEBUG_TYPE_EVENTS,"circuit already disconnected");
	 				break;
			}
        	break;
    	case C_STATE_INIT:
      		//assert (circuit);
			if(!circuit)
			{
				zlog_err("%-20s[%d] error: ",__FUNCTION__ ,  __LINE__);
				return NULL;
			}

      		switch (event)
			{
				case ISIS_ENABLE:
	  				isis_circuit_configure (circuit, (struct isis_area *) arg);
	  				if (isis_circuit_up (circuit) != ISIS_OK)
	  				{
	      				isis_circuit_deconfigure (circuit, (struct isis_area *) arg);
	      				break;
	  				}
	  				circuit->state = C_STATE_UP;
	  				isis_event_circuit_state_change (circuit, circuit->area, 1);
	  				listnode_delete (isis->init_circ_list, circuit);
	  			break;
				case IF_UP_FROM_Z:
          			//assert (circuit);
					if(!circuit)
					{
                zlog_debug(ISIS_DEBUG_TYPE_EVENTS,"%-20s[%d] error: ",__FUNCTION__,  __LINE__);
                return NULL;
            }

            zlog_debug (ISIS_DEBUG_TYPE_EVENTS,"circuit already connected");
            break;
        case ISIS_DISABLE:
            zlog_debug (ISIS_DEBUG_TYPE_EVENTS,"circuit already disabled");
	  			break;
				case IF_DOWN_FROM_Z:
	  				isis_circuit_if_del (circuit, (struct interface *) arg);
	  				listnode_delete (isis->init_circ_list, circuit);
	  				isis_circuit_del (circuit);
	  				circuit = NULL;
	  			break;
			}
       		break;
    	case C_STATE_CONF:
      		//assert (circuit);
			if(!circuit)
			{
				zlog_err("%-20s[%d] error: ",__FUNCTION__ ,  __LINE__);
				return NULL;
			}

     		switch (event)
			{
				case ISIS_ENABLE:
            zlog_debug (ISIS_DEBUG_TYPE_EVENTS,"circuit already enabled");
            break;
				case IF_UP_FROM_Z:
	  				isis_circuit_if_add (circuit, (struct interface *) arg);
					retv = isis_circuit_up (circuit);
	  				if (retv != ISIS_OK)
      				{
      				    if(retv == ISIS_DISMATCH)
      				    {
      				       zlog_err("area lsp length longer than interface mtu");			   
						   break;
      				    }
						else
						{
          				  isis_circuit_if_del (circuit, (struct interface *) arg);
						}
	      				break;
      				}
	  				circuit->state = C_STATE_UP;
	  				isis_event_circuit_state_change (circuit, circuit->area, 1);
	  				circuit->rcv_flag = 1 ;
	  				break;
				case ISIS_DISABLE:
	  				isis_circuit_deconfigure (circuit, (struct isis_area *) arg);
					if(circuit->interface != NULL)
					{
					  isis_circuit_if_del (circuit, circuit->interface);
					}
	  				isis_circuit_del (circuit);
	  				circuit = NULL;
	  				break;
	   			case IF_DOWN_FROM_Z:
            zlog_debug (ISIS_DEBUG_TYPE_EVENTS,"circuit already disconnected");
            break;
			}
      		break;
      case C_STATE_UP:
      		//assert (circuit);
			if(!circuit)
			{
				zlog_err("%-20s[%d] error: ",__FUNCTION__ ,  __LINE__);
				return NULL;
			}

      		switch (event)
			{
        case ISIS_ENABLE:
            zlog_debug (ISIS_DEBUG_TYPE_EVENTS,"circuit already configured");
            break;
        case IF_UP_FROM_Z:
            zlog_debug (ISIS_DEBUG_TYPE_EVENTS,"circuit already connected");
	  				break;
				case ISIS_DISABLE:
	  				isis_circuit_down (circuit);
	  				isis_circuit_deconfigure (circuit, (struct isis_area *) arg);
	  				circuit->state = C_STATE_INIT;
	  				isis_event_circuit_state_change (circuit,
                                       (struct isis_area *)arg, 0);                    
	  				listnode_add (isis->init_circ_list, circuit);
	 				break;
				case IF_DOWN_FROM_Z:
	  				isis_circuit_down (circuit);
          			isis_circuit_if_del (circuit, (struct interface *) arg);
	  				circuit->state = C_STATE_CONF;
	  				isis_event_circuit_state_change (circuit, circuit->area, 0);
	  				break;
			}
    		break;

    default:
        zlog_debug (ISIS_DEBUG_TYPE_EVENTS,"Invalid circuit state %d", old_state);
	}

    //if (isis->debugs & DEBUG_EVENTS|| isis->debugs & ISIS_DEBUG_TYPE_ALL)
    zlog_debug (ISIS_DEBUG_TYPE_EVENTS,"CSM_STATE_CHANGE: %s -> %s ", STATE2STR (old_state),
                circuit ? STATE2STR (circuit->state) : STATE2STR (C_STATE_NA));

  	return circuit;
}
