#include <string.h>
#include <stdlib.h>
#include <lib/command.h>
#include <lib/errcode.h>
#include <lib/ifm_common.h>
#include <lib/memtypes.h>
#include <lib/memory.h>
#include <lib/types.h>
#include <lib/log.h>
#include <lib/vty.h>

#include "l2/l2_if.h"
#include "aps/elps_cmd.h"
#include "aps/elps.h"
#include "aps/elps_pkt.h"
#include "aps/elps_fsm.h"
#include "cfm/cfm_session.h"
#include "aps/elps_timer.h"

struct elps_global gelps;              /* elps 全局数据结构 */
struct hash_table  elps_session_table; /* elps 会话 hash 表，用 sess_id 作为 key */

static struct cmd_node elps_session_node =
{
	ELPS_SESSION_NODE,
	"%s(config-elps-session)# ",
	1
};

static void elps_print_error (struct vty *vty, int error)
{
	switch(error)
	{
		case ELPS_LOWER_PRIO_CMD:
    		vty_error_out(vty, "Lower priority command not accepted.%s", VTY_NEWLINE);
    		break;
		case ELPS_IS_ALREADY_FS:
			vty_error_out(vty, "current state is already Forced Switch.%s", VTY_NEWLINE);
			break;
		case ELPS_IS_NOT_FS:
			vty_error_out(vty, "current state is not Forced Switch.%s", VTY_NEWLINE);
			break; 	
		case ELPS_IS_ALREADY_MS:
			vty_error_out(vty, "current state is already Manual Switch.%s", VTY_NEWLINE);
			break;
		case ELPS_IS_NOT_MS:
			vty_error_out(vty, "current state is not Manual Switch.%s", VTY_NEWLINE);
			break; 
		case ELPS_IS_ALREADY_LOP:
			vty_error_out(vty, "current state is already Lockout of protection.%s", VTY_NEWLINE);
			break;
		case ELPS_IS_NOT_LOP:
			vty_error_out(vty, "current state is not Lockout of protection.%s", VTY_NEWLINE);
			break; 
		default:
                		break;	
	}
}



DEFUN(elps_session,
	elps_session_cmd,
	"elps session <1-128>",
	"Elps management\n"
	"Session\n"
	"Range of sess_id:<1-128>\n")
{
	struct elps_sess *psess = NULL;
	uint16_t sess_index = 0;
    	int ret = 0;

	sess_index = atoi(argv[0]);

	psess = elps_sess_lookup(sess_index);
	if(NULL == psess)
	{
		psess = elps_sess_create(sess_index);
		if(NULL == psess)
		{
			vty_error_out(vty, "Fail to malloc for session!%s",VTY_NEWLINE);
			return CMD_WARNING;
		}
		
	        ret = elps_sess_add(psess);
	    	if(ret != ERRNO_SUCCESS)
	    	{
			vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);
	    		XFREE(MTYPE_ELPS_SESS, psess);
	    		return CMD_WARNING;
	    	}
	        gelps.sess_total++;
	}
	
	vty->index = psess;
	vty->node = ELPS_SESSION_NODE;

	return CMD_SUCCESS;
}

DEFUN(no_elps_session,
	no_elps_session_cmd,
	"no elps session <1-128>",
	"Delete\n"
	"Elps management\n"
	"Session\n"
	"Range of sess_id:<1-128>\n")
{
	struct elps_sess *psess = NULL;
	uint16_t sess_index = 0;
	int ret = 0;
	
	sess_index = atoi(argv[0]);

	psess = elps_sess_lookup(sess_index);
	if(NULL == psess)
	{
		vty_error_out(vty, "Session:%d is not exist!%s", sess_index, VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	ELPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);
	
	ret = elps_sess_delete(sess_index);
	if(ret == ERRNO_NOT_FOUND)
	{
		vty_error_out(vty, "Not found.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
    if(ret == ERRNO_FAIL)
    {
        vty_error_out(vty, "Unbind cfm session failed.%s", VTY_NEWLINE);
		return CMD_WARNING;
    }
    	gelps.sess_total--;
    
	return CMD_SUCCESS;
}




DEFUN(elps_protect_port,
	elps_protect_port_cmd,
	"elps protect-port master (ethernet|gigabitethernet|xgigabitethernet) USP backup (ethernet|gigabitethernet|xgigabitethernet) USP",
	"Elps management\n"
	"Protect port\n"
	"Master\n"
	CLI_INTERFACE_ETHERNET_STR
	CLI_INTERFACE_ETHERNET_VHELP_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_STR
	CLI_INTERFACE_XGIGABIT_ETHERNET_STR
	"Backup\n"
	CLI_INTERFACE_ETHERNET_STR
	CLI_INTERFACE_ETHERNET_VHELP_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_STR
	CLI_INTERFACE_XGIGABIT_ETHERNET_STR)
{
	struct elps_sess *psess =  (struct elps_sess *)vty->index;
	uint32_t ifindex_m = 0;
	uint32_t ifindex_b = 0;
	struct l2if *pif = NULL;
	
	ELPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);
	if(((psess->info.master_port) != 0)&&((psess->info.backup_port) != 0))
	{
		vty_error_out ( vty, "Session is already configure protection port%s", VTY_NEWLINE );
		return CMD_WARNING;
	}
	
	ifindex_m = ifm_get_ifindex_by_name ( ( char * ) argv[0], ( char * ) argv[1] );
	if ( ifindex_m == 0 )
	{
		vty_error_out ( vty, "Master is wrong format,please check out%s", VTY_NEWLINE );
		return CMD_WARNING;
	} 
	
	pif = l2if_get(ifindex_m);
	if((pif == NULL)||(pif->mode != IFNET_MODE_SWITCH ))
	{
		vty_error_out ( vty, "Master is no l2 port or port  does not exist. %s", VTY_NEWLINE );
		return CMD_WARNING;
	}
	memcpy(psess->info.master_port_mac, pif->mac, 6);
	
	ifindex_b = ifm_get_ifindex_by_name ( ( char * ) argv[2], ( char * ) argv[3] );
	if ( ifindex_b == 0 )
	{
		vty_error_out ( vty, "Backup is wrong format,please check out%s", VTY_NEWLINE );
		return CMD_WARNING;
	} 

	pif = l2if_get(ifindex_b);
	if((pif == NULL)||(pif->mode != IFNET_MODE_SWITCH ))
	{
		vty_error_out ( vty, "Backup is no l2 port or port  does not exist. %s", VTY_NEWLINE );
		return CMD_WARNING;
	}
	else if(ifindex_m == ifindex_b)
	{
		vty_error_out ( vty, "Master port is same as backup port,please check out%s", VTY_NEWLINE );
		return CMD_WARNING;
	}
	memcpy(psess->info.backup_port_mac, pif->mac, 6);
	
	psess->info.master_port = ifindex_m;
	psess->info.active_port = ifindex_m;	
	psess->info.backup_port = ifindex_b;
	return CMD_SUCCESS;
}

DEFUN(no_elps_protect_port,
	no_elps_protect_port_cmd,
	"no elps protect-port",
	"Delete\n"
	"Elps management\n"
	"Protect port\n")
{
	struct elps_sess *psess =  (struct elps_sess *)vty->index;
    struct cfm_sess  *unbind_cfm_sess = NULL;
	ELPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);
	if(((psess->info.master_port) == 0 )||((psess->info.backup_port) == 0 ))
	{
		vty_error_out ( vty, "Session is not configure protection port%s", VTY_NEWLINE );
		return CMD_WARNING;
	}
    if(psess->info.master_cfm_session)
	{	
        unbind_cfm_sess = cfm_session_unbind_elps(psess->info.master_cfm_session);
        if(unbind_cfm_sess == NULL)
        {
            vty_error_out(vty, "unbind cfm session failed%s",VTY_NEWLINE);
		    return CMD_WARNING;
        }
        psess->info.master_cfm_session= 0;
	}
    if(psess->info.backup_cfm_session)
    {
        unbind_cfm_sess = cfm_session_unbind_elps(psess->info.backup_cfm_session);
        if(unbind_cfm_sess == NULL)
        {
            vty_error_out(vty, "unbind cfm session failed%s",VTY_NEWLINE);
		    return CMD_WARNING;
        }
        psess->info.backup_cfm_session= 0;
    }
    
    psess->info.master_port = 0;
	psess->info.backup_port = 0;
	elps_vlan_map_unset(psess->info.data_vlan_map, 1, 4094);
	return CMD_SUCCESS;
}


DEFUN(elps_data_vlan,
	elps_data_vlan_cmd,
	"elps vlan <1-4094> {to <1-4094> }",
	"Elps management\n"
	"Elps data vlan start\n"
	"Range of data vlan:<1-4094>\n"
	"Elps data vlan end\n"
	"Range of data vlan:<1-4094>\n")
{
	struct elps_sess *psess = (struct elps_sess *)vty->index;
	uint16_t dvlan_start = 0;
	uint16_t dvlan_end = 0;
	struct elps_sess *sess = NULL;
	struct hash_bucket *pbucket = NULL;
   	int cursor;
	uint16_t ret;
	 
    ELPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);
    
    dvlan_start = atoi(argv[0]);
	if(argv[1])
	{
		if(atoi(argv[1]) < dvlan_start)
		{
			dvlan_end = dvlan_start;
			dvlan_start = atoi(argv[1]);
		}
		else
		{
			dvlan_end = atoi(argv[1])	;
		}
	}
	else
	{
		dvlan_end = atoi(argv[0])	;
	}
	if((dvlan_start < 1|| dvlan_start > 4094)||(dvlan_end < 1|| dvlan_end > 4094))
	{
		vty_error_out(vty, "Invalid VLAN id value.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(((psess->info.master_port) == 0)||((psess->info.backup_port) == 0))
	{
		vty_error_out ( vty, "Session is not configure protection port,please configure protection port first.%s", VTY_NEWLINE );
		return CMD_WARNING;
	}
	HASH_BUCKET_LOOP(pbucket, cursor, elps_session_table)
	{
		sess = (struct elps_sess *)pbucket->data; 
		if(psess->info.sess_id != sess->info.sess_id)
		{
			if((psess->info.master_port == sess->info.master_port)||
				(psess->info.master_port == sess->info.backup_port)||
				(psess->info.backup_port == sess->info.master_port)||
				(psess->info.backup_port == sess->info.backup_port))
			{
				ret = elps_vlan_map_isset(sess->info.data_vlan_map,dvlan_start,dvlan_end);
				if(ret)
				{
					vty_error_out(vty, "Vlan %d already configured as session %d data vlan.%s", ret,sess->info.sess_id,VTY_NEWLINE);
					return CMD_WARNING;
				}
				else
				{			
					ret = elps_vlan_map_isset(sess->info.data_vlan_map,1,4094);
					if(ret == 0)
					{				
						vty_error_out(vty, "Session %d protect this port all vlan.%s", ret,sess->info.sess_id,VTY_NEWLINE);
						return CMD_WARNING;
					}
				}
			}

		}
		elps_vlan_map_set(psess->info.data_vlan_map,dvlan_start,dvlan_end);
		psess->info.data_vlan_set_flag = 1;
	
	}
	
	return CMD_SUCCESS;

}
DEFUN(no_elps_data_vlan,
	no_elps_data_vlan_cmd,
	"no elps vlan <1-4094> {to <1-4094> }",

	"Delete\n"
	"Elps management\n"
	"Elps data  vlan \n"
	"Elps data  vlan start\n"
	"Range of data vlan:<1-4094>\n"
	"Elps data  vlan end\n"
	"Range of data vlan:<1-4094>\n")
{
	struct elps_sess *psess = (struct elps_sess *)vty->index;
	uint16_t dvlan_start = 0;
	uint16_t dvlan_end = 0;
	uint16_t ret;

    	ELPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

	dvlan_start = atoi(argv[0]);
	if(argv[1])
	{
		if(atoi(argv[1]) < dvlan_start)
		{
			dvlan_end = dvlan_start;
			dvlan_start = atoi(argv[1]);
		}
		else
		{
			dvlan_end = atoi(argv[1])	;
		}
	}
	else
	{
		dvlan_end = atoi(argv[0])	;
	}
	ret = elps_vlan_map_isset(psess->info.data_vlan_map,dvlan_start,dvlan_end);
	if(ret == 0)
	{
		vty_error_out(vty, "Vlan %d not configured as session %d data vlan.%s", ret,psess->info.sess_id,VTY_NEWLINE);
		return CMD_WARNING;
	}
	else
	{
		elps_vlan_map_unset(psess->info.data_vlan_map,dvlan_start,dvlan_end);
	}
	return CMD_SUCCESS;
}


DEFUN(no_elps_data_vlan_all,
	no_elps_data_vlan_all_cmd,
	"no elps vlan all",
	"Delete\n"
	"Elps management\n"
	"Elps data  vlan \n"
	"All \n")
{
	struct elps_sess *psess = (struct elps_sess *)vty->index;
	
    	ELPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

	elps_vlan_map_unset(psess->info.data_vlan_map,1,4094);
	
	return CMD_SUCCESS;
}


DEFUN(elps_primary_vlan,
	elps_primary_vlan_cmd,
	"elps primary-vlan <1-4094>",
	"Elps management\n"
	"Elps primary vlan\n"
	"Range of primary vlan:<1-4094>\n")
{
	struct elps_sess *psess = (struct elps_sess *)vty->index;
	uint16_t pri_vlan = 0;
	
    	ELPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);
    
    	pri_vlan = atoi(argv[0]);
	if(pri_vlan < 1|| pri_vlan > 4094)
	{
		vty_error_out(vty, "Invalid VLAN id value.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(psess->info.pvlan!= 0)
	{
		vty_error_out(vty, "Primary vlan is already configured%s",VTY_NEWLINE);
		return CMD_WARNING;
	}
	else
	{
		psess->info.pvlan = pri_vlan;
	}
	return CMD_SUCCESS;
}

DEFUN(no_elps_primary_vlan,
	no_elps_primary_vlan_cmd,
	"no elps primary-vlan",
	"Delete\n"
	"Elps management\n"
	"Elps primary vlan\n")
{
	struct elps_sess *psess = (struct elps_sess *)vty->index;
	
    	ELPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);
    
	if((psess->info.pvlan) != 0)
	{
		psess->info.pvlan = 0;
	}
	else
	{
		vty_error_out(vty, "Primary vlan is not already configured%s",VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	return CMD_SUCCESS;
}





DEFUN(elps_keep_alive,
	elps_keep_alive_cmd,
	"elps keep-alive <1-600>",
	"Elps management\n"
	"Elps keep alive\n"
	"Range of keep alive:<1s-600s>\n")
{
	struct elps_sess *psess = (struct elps_sess *)vty->index;
	uint16_t keep_alive = 0;
	
    	ELPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

	keep_alive = atoi(argv[0]);
	if(keep_alive < 1|| keep_alive > 600)
	{
		vty_error_out(vty, "Invalid keep alive value.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
    
	psess->info.keepalive = keep_alive;
	return CMD_SUCCESS;
}

DEFUN(no_elps_keep_alive,
	no_elps_keep_alive_cmd,
	"no elps keep-alive",
	"Delete\n"
	"Elps management\n"
	"Elps keep alive\n")
{
	struct elps_sess *psess = (struct elps_sess *)vty->index;
	
    	ELPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);
	 
	psess->info.keepalive = 5;
	return CMD_SUCCESS;
}




DEFUN(elps_hold_off,
	elps_hold_off_cmd,
	"elps hold-off <0-600>",
	"Elps management\n"
	"Elps hold off\n"
	"Range of hold off:<0s-600s>\n")
{
	struct elps_sess *psess = (struct elps_sess *)vty->index;
	uint16_t hold_off = 0;

    	ELPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

	hold_off = atoi(argv[0]);	
	psess->info.holdoff = hold_off;

	return CMD_SUCCESS;
}

DEFUN(no_elps_hold_off,
	no_elps_hold_off_cmd,
	"no elps hold-off",
	"Delete\n"
	"Elps management\n"
	"Elps hold off\n")
{
	struct elps_sess *psess = (struct elps_sess *)vty->index;
	
    	ELPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);
	 
	psess->info.holdoff = 0;
	
	return CMD_SUCCESS;
}




DEFUN(elps_backup_create,
	elps_backup_create_cmd,
	"elps backup non-failback ",
	"Elps management\n"
	"Protection restoration mode\n"
	"Non-revertive mode\n")
{
	struct elps_sess *psess = (struct elps_sess *)vty->index;
	
    	ELPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);
		
  	psess->info.failback = FAILBACK_DISABLE;
	psess->info.wtr = 0; 
	return CMD_SUCCESS;
}

DEFUN(elps_backup_failback_create,
	elps_backup_failback_cmd,
	"elps backup failback [ wtr <1-12> ]",
	"Elps management\n"
	"Protection restoration mode\n"
	"Revertive mode\n"
	"Wait to restore\n"
	"Range of wtr:<1min-12min>\n")
{
	struct elps_sess *psess = (struct elps_sess *)vty->index;
	uint16_t wtr = 0;
	
    	ELPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);
		
  	psess->info.failback = FAILBACK_ENABLE;
	if(argv[0] != NULL)
	{
		wtr = atoi(argv[0]);
		psess->info.wtr = wtr;
		
	}
	else
	{
		psess->info.wtr = 5;
	}
	
	return CMD_SUCCESS;
}

DEFUN(no_elps_backup_create,
	no_elps_backup_create_cmd,
	"no elps backup",
	"Delete\n"
	"Elps management\n"
	"Protection restoration mode\n")
{
	struct elps_sess *psess = (struct elps_sess *)vty->index;
	
    	ELPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);


	psess->info.failback = FAILBACK_ENABLE;
	psess->info.wtr = 5;
			
	return CMD_SUCCESS;
}





DEFUN(elps_priority,
	elps_priority_cmd,
	"elps priority <0-7>",
	"Elps management\n"
	"Elps packet priority\n"
	"Range of priority:<0-7>\n")
{
	struct elps_sess *psess = (struct elps_sess *)vty->index;
	uint8_t pri = 0;
	
    	ELPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);
    
    	pri = atoi(argv[0]);
	psess->info.priority = pri;

	return CMD_SUCCESS;
}

 DEFUN(no_elps_priority,
	 no_elps_priority_cmd,
	 "no elps priority ",
	 "Delete\n"
	 "Elps management\n"
	 "Elps packet priority\n")
 {	 
	 struct elps_sess *psess = (struct elps_sess *)vty->index;
	 
	 ELPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);
 
	 psess->info.priority = 6;
 
	 return CMD_SUCCESS;
 }

DEFUN(elps_bind_cfm_session,
	elps_bind_cfm_session_cmd,
	"elps bind cfm session <1-1024>(master |backup)",
	"Elps management\n"
	"bind\n"
	"Connectivity fault management\n"
	"Session\n"
	"Session index ,<1-1024>\n"
	"Master\n"
	"Backup\n")
{
	struct elps_sess *psess = (struct elps_sess *)vty->index;
	struct cfm_sess  *cfm_sess = NULL;
    struct cfm_sess  *bind_cfm_sess = NULL;
	uint16_t sess_id = 0;
	
    ELPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);
    
    sess_id = atoi(argv[0]);
		
	if(argv[1][0] == 'm')
	{
		if(psess->info.master_cfm_session== 0)
		{	
			cfm_sess = elps_find_cfm(psess->info.master_port,sess_id);
			if(cfm_sess)
			{
				psess->info.master_cfm_session = sess_id;
                bind_cfm_sess = cfm_session_bind_elps(sess_id);
                if(bind_cfm_sess == NULL)
                {
                    vty_error_out(vty, "bind cfm session failed%s",VTY_NEWLINE);
                    psess->info.master_cfm_session = 0;
				    return CMD_WARNING;
                }
			}
			else
			{
				vty_error_out(vty, "master port is not configured cfm session%s",VTY_NEWLINE);
				return CMD_WARNING;
			}

		}
		else
		{
			vty_error_out(vty, "master port is already configured cfm session%s",VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	if(argv[1][0] == 'b')
	{
		if(psess->info.backup_cfm_session== 0)
		{	
			cfm_sess = elps_find_cfm(psess->info.backup_port,sess_id);
			if(cfm_sess)
			{
				psess->info.backup_cfm_session = sess_id;
                bind_cfm_sess = cfm_session_bind_elps(sess_id);
                if(bind_cfm_sess == NULL)
                {
                    vty_error_out(vty, "bind cfm session failed%s",VTY_NEWLINE);
                    psess->info.backup_cfm_session = 0;
				    return CMD_WARNING;
                }
			}
			else
			{
				vty_error_out(vty, "backup port is not configured cfm session%s",VTY_NEWLINE);
				return CMD_WARNING;
			}

		}
		else
		{
			vty_error_out(vty, "backup port is already configured cfm session%s",VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	return CMD_SUCCESS;
}

DEFUN(no_elps_bind_cfm_session,
	no_elps_bind_cfm_session_cmd,
	"no elps bind cfm session <1-1024>(master |backup)",
	"Delete\n"
	"Elps management\n"
	"bind\n"
	"Connectivity fault management\n"
	"Session\n"
	"Session index ,<1-1024>\n"
	"Master\n"
	"Backup\n")

{
	struct elps_sess *psess = (struct elps_sess *)vty->index;
    struct cfm_sess  *unbind_cfm_sess = NULL;
	uint16_t sess_id = 0;
	
    ELPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);
    
    sess_id = atoi(argv[0]);
		
	if(argv[1][0] == 'm')
	{
		if(psess->info.master_cfm_session== sess_id)
		{	
			psess->info.master_cfm_session= 0;
            unbind_cfm_sess = cfm_session_unbind_elps(sess_id);
            if(unbind_cfm_sess == NULL)
            {
                vty_error_out(vty, "unbind cfm session failed%s",VTY_NEWLINE);
                psess->info.master_cfm_session = sess_id;
			    return CMD_WARNING;
            }
		}
		else
		{
			vty_error_out(vty, "master port configured cfm session is %d not %d %s",psess->info.master_cfm_session,sess_id,VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	if(argv[1][0] == 'b')
	{
		if(psess->info.backup_cfm_session== sess_id)
		{	
			psess->info.backup_cfm_session = 0;
            unbind_cfm_sess = cfm_session_unbind_elps(sess_id);
            if(unbind_cfm_sess == NULL)
            {
                vty_error_out(vty, "unbind cfm session failed%s",VTY_NEWLINE);
                psess->info.backup_cfm_session = sess_id;
			    return CMD_WARNING;
            }

		}
		else
		{
			vty_error_out(vty, "backup port configured cfm session is %d not %d %s",psess->info.backup_cfm_session,sess_id,VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	return CMD_SUCCESS;
}

DEFUN(elps_session_enable,
	elps_session_enable_cmd,
	"elps (enable | disable)",
	"Elps management\n"
	"Enable\n"
	"Disable\n")
{
    	struct elps_sess *psess = (struct elps_sess *)vty->index;
	struct elps_sess *sess = NULL;
	struct hash_bucket *pbucket = NULL;
   	int cursor;
    	int ret = 0;
		
	if(argv[0][0] == 'e')
	{
		ELPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);
		if(((psess->info.master_port) ==0) ||((psess->info.backup_port) == 0 ))
		{
			vty_error_out(vty, "ELPS port is not configure.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		if((psess->info.pvlan) == 0)
		{
			vty_error_out(vty, "ELPS primariy vlan is not configure.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		if((elps_vlan_map_isset(psess->info.data_vlan_map, 1, 4094)) == 0)
		{
			HASH_BUCKET_LOOP(pbucket, cursor, elps_session_table)
			{
				sess = (struct elps_sess *)pbucket->data; 
				if(psess->info.sess_id != sess->info.sess_id)
				{
					if((psess->info.master_port == sess->info.master_port)||
						(psess->info.master_port == sess->info.backup_port)||
						(psess->info.backup_port == sess->info.master_port)||
						(psess->info.backup_port == sess->info.backup_port))
					{
						vty_error_out(vty, "port is configure in session %d.%s",sess->info.sess_id,VTY_NEWLINE);
						return CMD_WARNING;
					}
				}
			}
		}
		ret = elps_sess_enable (psess);
		if (ERRNO_SUCCESS != ret)
		{
			vty_error_out(vty, "ELPS is sent failed.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		elps_init_session(psess);
		gelps.sess_enable++;
		
	}
	else if(argv[0][0] == 'd')
	{
		ELPS_SESSION_IS_DISABLE(vty, psess->info.status); 	
		ret = elps_sess_disable (psess);
		if (ERRNO_SUCCESS != ret)
		{
			vty_error_out(vty, "ELPS is sent failed.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}

		elps_clear_timer(psess);
		gelps.sess_enable--;	
	}
	
	return CMD_SUCCESS;	
}


DEFUN(elps_debug_packet,
	elps_debug_packet_cmd,
	"debug elps packet",
	"DEBUG_STR"
	"Elps management\n"
	"Elps packet\n" )
{	
	if(!gelps.sess_enable)
	{
	        vty_warning_out ( vty, "Please enable ELPS function at first%s", VTY_NEWLINE );
	        return CMD_WARNING;
	}

	gelps.debug_packet = 1;
	return CMD_SUCCESS;	
}

DEFUN(no_elps_debug_packet,
	no_elps_debug_packet_cmd,
	"no debug elps packet",
	"Delete\n"
	"DEBUG_STR"
	"Elps management\n"
	"Elps packet\n" )
{	
	if(!gelps.sess_enable)
	{
	        vty_warning_out ( vty, "Please enable ELPS function at first%s", VTY_NEWLINE );
	        return CMD_WARNING;
	}
	gelps.debug_packet = 0;
	return CMD_SUCCESS;	
	
}


/************************admin commands********************/

//Force Switch

DEFUN(elps_admin_force,
	elps_admin_force_cmd,
	"elps force-switch",
	"Elps management\n"
	"Force switching\n" )
{	
	struct elps_sess *psess = (struct elps_sess *)vty->index; 
	uint32_t ret;
	
	ELPS_SESSION_IS_DISABLE(vty, psess->info.status); 	
	
	ret = elps_sess_forceswitch(psess, 1);
	switch(ret)
	{
		case ERRNO_SUCCESS:
			return CMD_SUCCESS;
		default:
			elps_print_error(vty,ret);
			return CMD_WARNING;
	}
			 
}

DEFUN(no_elps_admin_force,
	no_elps_admin_force_cmd,
	"no elps force-switch",
	"Delete\n"
	"Elps management\n"
	"Force switching\n" )
{	
	struct elps_sess *psess = (struct elps_sess *)vty->index; 
	uint32_t ret;
	
	ELPS_SESSION_IS_DISABLE(vty, psess->info.status); 	
	
	ret = elps_sess_forceswitch(psess, 0);
	switch(ret)
	{
		case ERRNO_SUCCESS:
			return CMD_SUCCESS;
		default:
			elps_print_error(vty,ret);
			return CMD_WARNING;
	} 
}


//	Manual Switch

DEFUN(elps_admin_manual,
	elps_admin_manual_cmd,
	"elps manual-switch",
	"Elps management\n"
	"Manual switching\n" )
{	
	struct elps_sess *psess = (struct elps_sess *)vty->index; 
	uint32_t ret;
	
	ELPS_SESSION_IS_DISABLE(vty, psess->info.status); 	 
	
	ret = elps_sess_manualswitch(psess,1);
	switch(ret)
	{
		case ERRNO_SUCCESS:
			return CMD_SUCCESS;
		default:
			elps_print_error(vty,ret);
			return CMD_WARNING;
	}	
}

DEFUN(no_elps_admin_manual,
	no_elps_admin_manual_cmd,
	"no elps manual-switch",
	"Delete\n"
	"Elps management\n"
	"Manual switching\n" )
{	
	struct elps_sess *psess = (struct elps_sess *)vty->index; 
	uint32_t ret;
	
	ELPS_SESSION_IS_DISABLE(vty, psess->info.status); 	 
	
	ret = elps_sess_manualswitch(psess,0);
	switch(ret)
	{
		case ERRNO_SUCCESS:
			return CMD_SUCCESS;
		default:
			elps_print_error(vty,ret);
			return CMD_WARNING;;
	}	
}

//Lockout Of Protection

DEFUN(elps_admin_lockout,
	elps_admin_lockout_cmd,
	"elps lockout",
	"Elps management\n"
	"Lockout of protection\n" )
{	
	struct elps_sess *psess = (struct elps_sess *)vty->index; 
	uint32_t ret;
	
	ELPS_SESSION_IS_DISABLE(vty, psess->info.status); 	 
	
	ret = elps_sess_lock(psess,1);
	switch(ret)
	{
		case ERRNO_SUCCESS:
			return CMD_SUCCESS;
		default:
			elps_print_error(vty,ret);
			return CMD_WARNING;
	}	
}

DEFUN(no_elps_admin_lockout,
	no_elps_admin_lockout_cmd,
	"no elps lockout",
	"Delete\n"
	"Elps management\n"
	"Lockout of protection\n" )
{	
	struct elps_sess *psess = (struct elps_sess *)vty->index; 
	uint32_t ret;
	
	ELPS_SESSION_IS_DISABLE(vty, psess->info.status); 	 
	
	ret = elps_sess_lock(psess,0);
	switch(ret)
	{
		case ERRNO_SUCCESS:
			return CMD_SUCCESS;
		default:
			elps_print_error(vty,ret);
			return CMD_WARNING;
	}	
}



/************************show  commands********************/

DEFUN(show_elps_config,
	show_elps_config_cmd,
	"show elps config",
	"Display.\n"
	"Elps management\n"
	"Config\n")
{
	struct hash_bucket *bucket = NULL;
	struct elps_sess *psess = NULL;
	int cursor = 0;
    	int num = 0;
		
	HASH_BUCKET_LOOP(bucket, cursor, elps_session_table)
	{
		psess = (struct elps_sess *)bucket->data;
        		vty_out(vty, "%4s:%d%s", "Num", ++num, VTY_NEWLINE);
		epls_show_config(vty, psess);
		psess = NULL;
	}
    	return CMD_SUCCESS;
}


DEFUN(show_elps_session,
	show_elps_session_cmd,
	"show elps session [<1-128>]",
	"Display.\n"
	"Elps management\n"
	"Session\n"
	"Range of local_mep:<1-128>\n")
{
	struct hash_bucket *bucket = NULL;
	struct elps_sess *psess = NULL;
	uint16_t sess_id = 0;
	int cursor = 0;
    	int num = 0;

	if(NULL == argv[0])//显示所有session信息
	{
		vty_out(vty, "%4s%-18s: %d%s", "", "Total", gelps.sess_total, VTY_NEWLINE);
		vty_out(vty, "%4s%-18s: %d%s", "", "enable_num", gelps.sess_enable, VTY_NEWLINE);
		
		HASH_BUCKET_LOOP(bucket, cursor, elps_session_table)
		{
			psess = (struct elps_sess *)bucket->data;
            		vty_out(vty, "%4s:%d%s", "Num", ++num, VTY_NEWLINE);
			elps_show_session(vty, psess);
			psess = NULL;
		}
	}
	else
	{
		sess_id = atoi(argv[0]);
		psess = elps_sess_lookup(sess_id);
		if(psess == NULL)
		{
			vty_error_out(vty, "Session:%d is not exist!%s", sess_id, VTY_NEWLINE);
			return CMD_WARNING;
		}
		elps_show_session(vty, psess);
	}
    
    return CMD_SUCCESS;
}

DEFUN(show_elps_debug,
	show_elps_debug_cmd,
	"show elps debug",
	"Display.\n"
	"Elps management\n"
	"Debug\n")
{
	vty_out(vty, "aps recv total %d%s", gelps.pkt_recv, VTY_NEWLINE);
	vty_out(vty, "aps send total %d%s", gelps.pkt_send, VTY_NEWLINE);
	vty_out(vty, "aps error total %d%s", gelps.pkt_err, VTY_NEWLINE);
	if(gelps.debug_packet == 0)
	{
		vty_out(vty, "aps debug packet is %s%s", "disable", VTY_NEWLINE);	
	}
	else
	{
		vty_out(vty, "aps debug packet is %s%s", "enable", VTY_NEWLINE);	
	}
	
	return CMD_SUCCESS;
}

static int elps_session_config_write(struct vty *vty)
{
	struct hash_bucket *bucket = NULL;
	struct elps_sess *psess = NULL;
	char ifname_m[NAME_STRING_LEN],ifname_b[NAME_STRING_LEN];
	int cursor = 0;
	int ret = 0;
    
	HASH_BUCKET_LOOP(bucket, cursor, elps_session_table)
	{   
		psess = (struct elps_sess *)bucket->data;

		vty_out(vty, "elps session %d%s", psess->info.sess_id, VTY_NEWLINE);

		ifm_get_name_by_ifindex(psess->info.master_port, ifname_m);
		ifm_get_name_by_ifindex(psess->info.backup_port, ifname_b);
		if((ifname_m[0] != 'u') &&(ifname_b[0] != 'u'))
		{
			vty_out(vty, " elps protect-port master %s backup %s%s", ifname_m,ifname_b, VTY_NEWLINE);
		}	
		
		ret = elps_vlan_map_isset(psess->info.data_vlan_map,1,4094);
		if(ret)
		{
			elps_vlan_show(vty,psess,2);

		}
		if(psess->info.pvlan)
		{
			vty_out(vty, " elps primary-vlan %d%s", psess->info.pvlan, VTY_NEWLINE);
		}
		if((psess->info.keepalive)&&(psess->info.keepalive != 5))
		{
			vty_out(vty, " elps keep-alive %d%s", psess->info.keepalive, VTY_NEWLINE);
		}
		if(psess->info.holdoff)
		{
			vty_out(vty, " elps hold-off %d%s", psess->info.holdoff, VTY_NEWLINE);
		}	
		
       		if(psess->info.failback == FAILBACK_DISABLE)
		{
			vty_out(vty, " elps backup %s%s", "non-failback", VTY_NEWLINE);
		}
		else if((psess->info.failback == FAILBACK_ENABLE)&&(psess->info.wtr != 5))
		{
			vty_out(vty, " elps backup %s wtr %d%s", "failback", psess->info.wtr,VTY_NEWLINE);
		}
		if(psess->info.priority != 6)
		{
			vty_out(vty, " elps priority %d%s", psess->info.priority, VTY_NEWLINE);
		}
		if(psess->info.master_cfm_session)
		{
			vty_out(vty, " elps bind cfm session %d master %s", psess->info.master_cfm_session, VTY_NEWLINE);
		}
		if(psess->info.backup_cfm_session)
		{
			vty_out(vty, " elps bind cfm session %d backup %s", psess->info.backup_cfm_session, VTY_NEWLINE);
		}
		
		if(psess->info.status ==APS_STATUS_ENABLE)
		{
			vty_out(vty, " elps %s%s", "enable", VTY_NEWLINE);
		}
	}
    return ERRNO_SUCCESS;
}


void elps_cmd_init(void)
{
	install_node (&elps_session_node, elps_session_config_write); 
	install_default (ELPS_SESSION_NODE); 

	install_element (CONFIG_NODE, &elps_session_cmd,CMD_SYNC);
	install_element (CONFIG_NODE, &no_elps_session_cmd,CMD_SYNC); 
	install_element (CONFIG_NODE, &elps_debug_packet_cmd,CMD_LOCAL);
	install_element (CONFIG_NODE, &no_elps_debug_packet_cmd,CMD_LOCAL); 
	
	//show commands
	install_element (CONFIG_NODE, &show_elps_config_cmd,CMD_LOCAL);
	install_element (CONFIG_NODE, &show_elps_session_cmd,CMD_LOCAL); 
	install_element (CONFIG_NODE, &show_elps_debug_cmd,CMD_LOCAL);
	
	install_element (ELPS_SESSION_NODE, &elps_protect_port_cmd,CMD_SYNC); 
	install_element (ELPS_SESSION_NODE, &no_elps_protect_port_cmd,CMD_SYNC); 
	install_element (ELPS_SESSION_NODE, &elps_data_vlan_cmd,CMD_SYNC);
	install_element (ELPS_SESSION_NODE, &no_elps_data_vlan_cmd,CMD_SYNC); 
	install_element (ELPS_SESSION_NODE, &no_elps_data_vlan_all_cmd,CMD_SYNC); 
	install_element (ELPS_SESSION_NODE, &elps_primary_vlan_cmd,CMD_SYNC);
	install_element (ELPS_SESSION_NODE, &no_elps_primary_vlan_cmd,CMD_SYNC); 
	install_element (ELPS_SESSION_NODE, &elps_keep_alive_cmd,CMD_SYNC);
	install_element (ELPS_SESSION_NODE, &no_elps_keep_alive_cmd,CMD_SYNC); 
	install_element (ELPS_SESSION_NODE, &elps_hold_off_cmd,CMD_SYNC);
	install_element (ELPS_SESSION_NODE, &no_elps_hold_off_cmd,CMD_SYNC); 
	install_element (ELPS_SESSION_NODE, &elps_backup_create_cmd,CMD_SYNC);
	install_element (ELPS_SESSION_NODE, &elps_backup_failback_cmd,CMD_SYNC);
	install_element (ELPS_SESSION_NODE, &no_elps_backup_create_cmd,CMD_SYNC); 
	install_element (ELPS_SESSION_NODE, &elps_priority_cmd,CMD_SYNC);
	install_element (ELPS_SESSION_NODE, &no_elps_priority_cmd,CMD_SYNC); 
	install_element (ELPS_SESSION_NODE, &elps_bind_cfm_session_cmd,CMD_SYNC); 
	install_element (ELPS_SESSION_NODE, &no_elps_bind_cfm_session_cmd,CMD_SYNC); 
	install_element (ELPS_SESSION_NODE, &elps_session_enable_cmd,CMD_SYNC); 

	//admin commands
	install_element (ELPS_SESSION_NODE, &elps_admin_force_cmd,CMD_LOCAL); 
	install_element (ELPS_SESSION_NODE, &no_elps_admin_force_cmd,CMD_LOCAL); 
	install_element (ELPS_SESSION_NODE, &elps_admin_manual_cmd,CMD_LOCAL); 
	install_element (ELPS_SESSION_NODE, &no_elps_admin_manual_cmd,CMD_LOCAL); 
	install_element (ELPS_SESSION_NODE, &elps_admin_lockout_cmd,CMD_LOCAL); 
	install_element (ELPS_SESSION_NODE, &no_elps_admin_lockout_cmd,CMD_LOCAL); 

}

void elps_vlan_show(struct vty *vty,struct elps_sess *psess,uint8_t is_write)
{
	uint16_t vlan_list[4095] = {0};
	uint16_t i,j =0;
	uint16_t vlan_start = 0;
	int ret;
	
	ELPS_LOG_DBG("%s: '%s'--the line of %d",__FILE__,__func__,__LINE__);
	for(i = 1; i<4095; i++)
	{
		ret = elps_vlan_map_isset(psess->info.data_vlan_map,i,i);		
		if(ret)
		{			
			ELPS_LOG_DBG("%s: '%s'--the line of %d vlan %d ",__FILE__,__func__,__LINE__,ret);
			vlan_list[j] = ret;
			j++;
		}
	}
	
	ELPS_LOG_DBG("%s: '%s'--the line of %d j %d ",__FILE__,__func__,__LINE__,j);

	for(i =0; i<j-1; i++)
	{
		if((vlan_list[i+1]-vlan_list[i]) == 1)
		{
			if(vlan_start == 0)
			{
				vlan_start = vlan_list[i];
				ELPS_LOG_DBG("%s: '%s'--the line of %d vlan_start %d ",__FILE__,__func__,__LINE__,vlan_start);
			}
		}
		else
		{	
			if(vlan_start == 0)
			{
				if(is_write)
				{	
					vty_out(vty, " elps vlan %d%s", vlan_list[i],VTY_NEWLINE);
				}
				else
				{
					vty_out(vty, "%d, ", vlan_list[i]);
				}
			}
			else
			{	if(is_write)
				{
					vty_out(vty, " elps vlan %d to %d%s", vlan_start,vlan_list[i],VTY_NEWLINE);
				}
				else
				{
					vty_out(vty, "%d - %d, ", vlan_start,vlan_list[i]);		
					ELPS_LOG_DBG("%s: '%s'--the line of %d vlan_start %d vlan_list[i] %d",__FILE__,__func__,__LINE__,vlan_start,vlan_list[i]);
				}
				vlan_start = 0;
				
			}
		}
	}

	if(vlan_start)
	{	
		if(is_write)
		{
			vty_out(vty, " elps vlan %d to %d%s", vlan_start,vlan_list[i],VTY_NEWLINE);
		}
		else
		{
			vty_out(vty, "%d - %d ", vlan_start,vlan_list[i]);		
			ELPS_LOG_DBG("%s: '%s'--the line of %d vlan_start %d vlan_list[i] %d",__FILE__,__func__,__LINE__,vlan_start,vlan_list[i]);
		}
		vlan_start = 0;
	}
	else
	{
		if(is_write)
		{	
			vty_out(vty, " elps vlan %d%s", vlan_list[i],VTY_NEWLINE);
		}
		else
		{
			vty_out(vty, "%d ", vlan_list[i]);
		}
	}
}
void epls_show_config(struct vty *vty, struct elps_sess *psess)
{
	char ifname[NAME_STRING_LEN];
	
	if(psess == NULL)
	{
		return ;
	}

	vty_out(vty, "%4s%-18s: %d%s", "", "session-id", psess->info.sess_id, VTY_NEWLINE);

	ifm_get_name_by_ifindex(psess->info.master_port, ifname);
	if(ifname[0] != 'u')
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "master-port", ifname, VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "master-port", "--" ,VTY_NEWLINE);
	}
	
	ifm_get_name_by_ifindex(psess->info.backup_port, ifname);
	if(ifname[0] != 'u')
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "backup-port", ifname, VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "backup_port", "--" ,VTY_NEWLINE);
	}
	
	vty_out(vty, "%4s%-18s: %d%s", "", "primary-vlan", psess->info.pvlan, VTY_NEWLINE);
	vty_out(vty, "%4s%-18s: ", "", "protection-vlan");
	elps_vlan_show(vty,psess,0);
	vty_out(vty, "%s",VTY_NEWLINE);
	vty_out(vty, "%4s%-18s: %d s%s", "", "keep-alive", psess->info.keepalive, VTY_NEWLINE);
	vty_out(vty, "%4s%-18s: %d s%s", "", "hold-off", psess->info.holdoff, VTY_NEWLINE);

	if(psess->info.failback == FAILBACK_ENABLE)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "backup-mode", "Failback", VTY_NEWLINE);
		if((psess->info.failback) ==FAILBACK_ENABLE)
		{
			vty_out(vty, "%4s%-18s: %d min%s", "", "wtr", psess->info.wtr, VTY_NEWLINE);
		}
	}
	else if(psess->info.failback == FAILBACK_DISABLE)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "backup-mode", "Non-Failback", VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "backup-mode", "Invalid-mode", VTY_NEWLINE);
	}
	
	vty_out(vty, "%4s%-18s: %d%s", "", "priority", psess->info.priority, VTY_NEWLINE); 
	if(psess->info.master_cfm_session)
	{
		vty_out(vty, "%4s%-18s: %d%s", "", "master cfm session", psess->info.master_cfm_session, VTY_NEWLINE);	}
	else
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "master cfm session", "--", VTY_NEWLINE);
	}

	if(psess->info.backup_cfm_session)
	{
		vty_out(vty, "%4s%-18s: %d%s", "", "backup cfm session", psess->info.backup_cfm_session, VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "backup cfm session", "--", VTY_NEWLINE);
	}	

	if(psess->info.status ==APS_STATUS_DISABLE)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "session-status", "disable", VTY_NEWLINE);
	}
	else if(psess->info.status ==APS_STATUS_ENABLE)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "session-status", "enable", VTY_NEWLINE);
	}
    	vty_out(vty, "%s", VTY_NEWLINE);
	return ;
}


void elps_show_session(struct vty *vty, struct elps_sess *psess)
{
	char ifname[NAME_STRING_LEN];
	
	if(psess == NULL)
	{
		return ;
	}
	
	vty_out(vty, "%-11s%-23s%-23s%-20s%s", "session-id", "master-port", "backup-port", "current-state", VTY_NEWLINE);
	vty_out(vty, "----------------------------------------------------------------------------\r\n");
   	vty_out(vty, "%-11d", psess->info.sess_id);
	ifm_get_name_by_ifindex(psess->info.master_port, ifname);
	if(ifname[0] != 'u')
	{
		vty_out(vty, "%-23s", ifname);
	}
	else
	{	
		vty_out(vty, "%-23s", "--");
	}
	ifm_get_name_by_ifindex(psess->info.backup_port, ifname);
	if(ifname[0] == 'u')
	{	
		vty_out(vty, "%-23s", "--");
	}
	else
	{
		
		vty_out(vty, "%-23s", ifname);
	}

	if(psess->info.current_status ==ELPS_NO_REQUEST)
	{
		vty_out(vty, "%s%s", "No request", VTY_NEWLINE);
	}
	else if(psess->info.current_status ==ELPS_DO_NOT_REVERT)
	{
		vty_out(vty, "%s%s", "Do not revert", VTY_NEWLINE);
	}
	else if(psess->info.current_status ==ELPS_WAIT_TO_RESTORE)
	{
		vty_out(vty, "%s%s", "Wait-To-Restore", VTY_NEWLINE);
	}
	else if(psess->info.current_status ==ELPS_MANUAL_SWITCH)
	{
		vty_out(vty, "%s%s", "Manual Switch", VTY_NEWLINE);
	}
	else if(psess->info.current_status ==ELPS_SIGNAL_FAIL_FOR_WORKING)
	{
		vty_out(vty, "%s%s", "Signal Fail for Working", VTY_NEWLINE);
	}
	else if(psess->info.current_status ==ELPS_FORCED_SWITCH)
	{
		vty_out(vty, "%s%s", "Forced Switch", VTY_NEWLINE);
	}
	else if(psess->info.current_status ==ELPS_SIGNAL_FAIL_FOR_PROTECTION)
	{
		vty_out(vty, "%s%s", "Signal Fail for Protection", VTY_NEWLINE);
	}
	else if(psess->info.current_status ==ELPS_LOCKOUT_OF_PROTECTION)
	{
		vty_out(vty, "%s%s", "Lockout Of Protection", VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "%s%s", "Invalid", VTY_NEWLINE);
	}
	
	vty_out(vty, "%s",VTY_NEWLINE);
	
	if(psess->info.dfop_alarm == 0)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "DFOP state  ", "Normal", VTY_NEWLINE);
	}
	else if(psess->info.dfop_alarm == ELPS_RECV_APS_ON_MASTER) 
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "DFOP state  ", "APS Received on Master port", VTY_NEWLINE);
	}
	else if(psess->info.dfop_alarm == ELPS_RECV_APS_B_INVALID)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "DFOP state  ", "B bit Mismatch Defect", VTY_NEWLINE);
	}
	else if(psess->info.dfop_alarm == ELPS_RECV_APS_ABDR_INVALID)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "DFOP state  ", "ABDR invalid Defect", VTY_NEWLINE);
	}
	else if (psess->info.dfop_alarm == ELPS_NO_APS_RECV)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "DFOP state  ", "NO APS Receive on Backup port", VTY_NEWLINE);
	}
	
	vty_out(vty, "%s",VTY_NEWLINE);

	vty_out(vty, "%4s%-18s: %d%s", "", "primary-vlan", psess->info.pvlan, VTY_NEWLINE);
	vty_out(vty, "%4s%-18s: %s%s", "", "active-port", ((psess->info.active_port)== (psess->info.master_port))? "Master-port":"Backup-port", VTY_NEWLINE);

	vty_out(vty, "%4s%-18s: ", "", "protection-vlan");
	elps_vlan_show(vty,psess,0);
	vty_out(vty, "%s",VTY_NEWLINE);
	vty_out(vty, "%4s%-18s: %d s%s", "", "keep-alive", psess->info.keepalive, VTY_NEWLINE);
	vty_out(vty, "%4s%-18s: %d s%s", "", "hold-off", psess->info.holdoff, VTY_NEWLINE);
	
	if(psess->info.failback == FAILBACK_ENABLE)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "backup-mode", "Failback", VTY_NEWLINE);
		vty_out(vty, "%4s%-18s: %d min%s", "", "wtr", psess->info.wtr, VTY_NEWLINE);
	}
	else if(psess->info.failback == FAILBACK_DISABLE)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "backup-mode", "Non-Failback", VTY_NEWLINE);
	}
	
    	vty_out(vty, "%4s%-18s: %d%s", "", "priority", psess->info.priority, VTY_NEWLINE);
	if(psess->info.master_cfm_session)
	{
		vty_out(vty, "%4s%-18s: %d%s", "", "master cfm session", psess->info.master_cfm_session, VTY_NEWLINE);	}
	else
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "master cfm session", "--", VTY_NEWLINE);
	}

	if(psess->info.backup_cfm_session)
	{
		vty_out(vty, "%4s%-18s: %d%s", "", "backup cfm session", psess->info.backup_cfm_session, VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "backup cfm session", "--", VTY_NEWLINE);
	}	
	if(psess->info.status ==APS_STATUS_DISABLE)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "session-status", "disable", VTY_NEWLINE);
	}
	else if(psess->info.status ==APS_STATUS_ENABLE)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "session-status", "enable", VTY_NEWLINE);
	}
    	vty_out(vty, "%s", VTY_NEWLINE);
}
