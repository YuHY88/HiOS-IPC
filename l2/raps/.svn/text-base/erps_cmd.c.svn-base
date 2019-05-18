#include <string.h>
#include <stdlib.h>
#include <lib/command.h>
#include <lib/errcode.h>
#include <lib/ifm_common.h>
#include <lib/memtypes.h>
#include <lib/memory.h>
#include <lib/types.h>
#include <lib/oam_common.h>
#include <lib/log.h>


#include "raps/erps_cmd.h"
#include "raps/erps_timer.h"
#include "raps/erps_fsm.h"
#include "raps/erps_fsm_action.h"

#include "l2_if.h"

static struct cmd_node erps_session_node =
{
    ERPS_SESSION_NODE,
    "%s(config-erps-session)# ",
    1
};


DEFUN(erps_session,
      erps_session_cmd,
      "erps session <1-255>",
      "Erps management\n"
      "Session\n"
      "Range of sess_id:<1-255>\n")
{
    struct erps_sess *psess = NULL;
    uint16_t sess_index = 0;
    int ret = 0;

    sess_index = atoi(argv[0]);

    psess = erps_sess_lookup(sess_index);

    if(NULL == psess)
    {
        psess = erps_sess_create(sess_index);

        if(NULL == psess)
        {
            vty_error_out(vty, "Fail to malloc for session!%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

		gerps.sess_total += 1;
        ret = erps_sess_add(psess);

        if(ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);
            XFREE(MTYPE_ERPS_SESS, psess);
            return CMD_WARNING;
        }
    }

    vty->index = psess;
    vty->node = ERPS_SESSION_NODE;

    return CMD_SUCCESS;
}

DEFUN(no_erps_session,
      no_erps_session_cmd,
      "no erps session <1-255>",
      "Delete\n"
      "Erps management\n"
      "Session\n"
      "Range of sess_id:<1-255>\n")
{
    struct erps_sess *psess = NULL;
    uint16_t sess_index = 0;
    int ret = 0;

    sess_index = atoi(argv[0]);

    psess = erps_sess_lookup(sess_index);

    if(NULL == psess)
    {
        vty_error_out(vty, "Session:%d is not exist!%s", sess_index, VTY_NEWLINE);
        return CMD_WARNING;
    }

    ERPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

    ret = erps_sess_delete(sess_index);

    if(ERRNO_NOT_FOUND == ret)
    {
        vty_error_out(vty, "Not found erps session %d.%s", sess_index, VTY_NEWLINE);
        return CMD_WARNING;
    }
	
	if(gerps.sess_total != 0)
	{
		gerps.sess_total -= 1;
	}
	
    return CMD_SUCCESS;
}

DEFUN(erps_physical_ring,
      erps_physical_ring_cmd,
      "physical-ring east-interface (ethernet|gigabitethernet|xgigabitethernet) USP west-interface (ethernet|gigabitethernet|xgigabitethernet) USP",
      "physical-ring\n"
      "east-interface\n"
      CLI_INTERFACE_ETHERNET_STR
      CLI_INTERFACE_GIGABIT_ETHERNET_STR
      CLI_INTERFACE_XGIGABIT_ETHERNET_STR
      CLI_INTERFACE_ETHERNET_VHELP_STR
      "west-interface \n"
      CLI_INTERFACE_ETHERNET_STR
      CLI_INTERFACE_GIGABIT_ETHERNET_STR
      CLI_INTERFACE_XGIGABIT_ETHERNET_STR
      CLI_INTERFACE_ETHERNET_VHELP_STR)
{
    struct erps_sess *psess = (struct erps_sess *)vty->index;
    uint32_t ifindex_e = 0;
    uint32_t ifindex_w = 0;
    struct l2if *pif = NULL;

    ERPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

    if((psess->info.west_interface != 0) || (psess->info.east_interface != 0))
    {
        vty_error_out(vty, "Session is already configure protection port. %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ifindex_e = ifm_get_ifindex_by_name((char *) argv[0], (char *) argv[1]);

    if(0 == ifindex_e)
    {
        vty_error_out(vty, "East interface is wrong format,please check out. %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    pif = l2if_get(ifindex_e);

    if((NULL == pif) || (IFNET_MODE_SWITCH != pif->mode))
    {
        vty_error_out(vty, "East interface is no l2 port or port does not exist. %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
	memcpy(psess->info.eastInterfaceMac, pif->mac, 6);
	
    ifindex_w = ifm_get_ifindex_by_name((char *) argv[2], (char *) argv[3]);

    if(0 == ifindex_w)
    {
        vty_error_out(vty, "West interface is wrong format,please check out. %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    pif = l2if_get(ifindex_w);

    if((NULL == pif) || (IFNET_MODE_SWITCH != pif->mode))
    {
        vty_error_out(vty, "West interface is no l2 port or port does not exist. %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    else if(ifindex_e == ifindex_w)
    {
        vty_error_out(vty, "East interface is same as West interface,please check out. %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
	memcpy(psess->info.westInterfaceMac, pif->mac, 6);

    psess->info.east_interface = ifindex_e;
    psess->info.west_interface = ifindex_w;
	psess->info.sub_ring = 0;

    memcpy(psess->info.node_id, pif->mac, MAC_LEN);
    memcpy(psess->r_aps.node_id, pif->mac, MAC_LEN);
    return CMD_SUCCESS;
}

DEFUN(erps_physical_subring,
	erps_physical_subring_cmd,
	"physical-subring east-interface (ethernet|gigabitethernet|xgigabitethernet) USP",
	"physical-subring\n"
	"east-interface\n"
	CLI_INTERFACE_ETHERNET_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_STR
	CLI_INTERFACE_XGIGABIT_ETHERNET_STR
	CLI_INTERFACE_ETHERNET_VHELP_STR
	)
{
	struct erps_sess *psess = (struct erps_sess *)vty->index;
	uint32_t ifindex_e = 0;
	struct l2if *pif = NULL;

	ERPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

	if((psess->info.west_interface != 0) || (psess->info.east_interface != 0))
	{
	  vty_error_out(vty, "Session is already configure protection port. %s", VTY_NEWLINE);
	  return CMD_WARNING;
	}

	ifindex_e = ifm_get_ifindex_by_name((char *) argv[0], (char *) argv[1]);

	if(0 == ifindex_e)
	{
	  vty_error_out(vty, "East interface is wrong format,please check out. %s", VTY_NEWLINE);
	  return CMD_WARNING;
	}

	pif = l2if_get(ifindex_e);

	if((NULL == pif) || (IFNET_MODE_SWITCH != pif->mode))
	{
	  vty_error_out(vty, "East interface is no l2 port or port does not exist. %s", VTY_NEWLINE);
	  return CMD_WARNING;
	}
	memcpy(psess->info.eastInterfaceMac, pif->mac, 6);
	memcpy(psess->info.westInterfaceMac, pif->mac, 6);

	psess->info.east_interface = ifindex_e;
	psess->info.west_interface = 0;
	psess->info.sub_ring = 1;
	
	memcpy(psess->info.node_id, pif->mac, MAC_LEN);
	memcpy(psess->r_aps.node_id, pif->mac, MAC_LEN);
	return CMD_SUCCESS;
}

DEFUN(no_erps_physical_ring,
      no_erps_physical_ring_cmd,
      "no physical-ring",
      "Delete\n"
      "physical-ring\n")
{
    struct erps_sess *psess = (struct erps_sess *)vty->index;
    struct cfm_sess *unbind_cfm_sess = NULL;

    ERPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

	if(psess->info.sub_ring == 1)
	{
		vty_error_out(vty, "This command only takes effect on the master ring.%s", VTY_NEWLINE);
	 	 return CMD_WARNING;
	}
    erps_vlan_map_unset(psess->info.data_vlan_map, 1, 4094);
    psess->info.role = ERPS_ROLE_INVALID;
    psess->info.east_interface = 0;
    psess->info.west_interface = 0;
    psess->info.block_interface = 0;
    psess->info.rpl_interface = 0;
    psess->info.pvlan = 0;
    psess->info.attach_sess_id = 0;
    psess->attach_to_sess = NULL;
	psess->info.sub_ring = 0;

    if(psess->info.east_cfm_session_id)
    {
        unbind_cfm_sess = cfm_session_unbind_elps(psess->info.east_cfm_session_id);

        if(NULL == unbind_cfm_sess)
        {
            vty_error_out(vty, "unbind cfm session failed%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        psess->info.east_cfm_session_id = 0;
        psess->east_cfm_session = NULL;
    }
    else if(psess->info.west_cfm_session_id)
    {
        unbind_cfm_sess = cfm_session_unbind_elps(psess->info.west_cfm_session_id);

        if(NULL == unbind_cfm_sess)
        {
            vty_error_out(vty, "unbind cfm session failed%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        psess->info.west_cfm_session_id = 0;
        psess->west_cfm_session = NULL;
    }


    return CMD_SUCCESS;
}

DEFUN(no_erps_physical_subring,
	no_erps_physical_subring_cmd,
	"no physical-subring",
	"Delete\n"
	"physical-subring\n")
{
	struct erps_sess *psess = (struct erps_sess *)vty->index;
	struct cfm_sess *unbind_cfm_sess = NULL;

	ERPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

	if(psess->info.sub_ring == 0)
	{
		vty_error_out(vty, "This command only takes effect on the subring.%s", VTY_NEWLINE);
	 	 return CMD_WARNING;
	}
	erps_vlan_map_unset(psess->info.data_vlan_map, 1, 4094);
	psess->info.role = ERPS_ROLE_INVALID;
	psess->info.east_interface = 0;
	psess->info.west_interface = 0;
	psess->info.block_interface = 0;
	psess->info.rpl_interface = 0;
	psess->info.pvlan = 0;
	psess->info.attach_sess_id = 0;
	psess->attach_to_sess = NULL;
	psess->info.sub_ring = 0;

	if(psess->info.east_cfm_session_id)
	{
	  unbind_cfm_sess = cfm_session_unbind_elps(psess->info.east_cfm_session_id);

	  if(NULL == unbind_cfm_sess)
	  {
		  vty_error_out(vty, "unbind cfm session failed%s", VTY_NEWLINE);
		  return CMD_WARNING;
	  }

	  psess->info.east_cfm_session_id = 0;
	  psess->east_cfm_session = NULL;
	}
	else if(psess->info.west_cfm_session_id)
    {
        unbind_cfm_sess = cfm_session_unbind_elps(psess->info.west_cfm_session_id);

        if(NULL == unbind_cfm_sess)
        {
            vty_error_out(vty, "unbind cfm session failed%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        psess->info.west_cfm_session_id = 0;
        psess->west_cfm_session = NULL;
    }

	return CMD_SUCCESS;
}

DEFUN(erps_rpl_role,
      erps_rpl_role_cmd,
      "rpl role (owner | neighbor) (east-interface | west-interface)",
      "ring protection link\n"
      "role\n"
      "owner \n"
      "neighbor \n"
      "rpl interface:east-interface \n"
      "rpl interface:west-interface \n")
{
    struct erps_sess *psess = (struct erps_sess *)vty->index;
    ERPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

    if(psess->info.role != ERPS_ROLE_INVALID)
    {
        vty_error_out(vty, "Have already configure ring protection link role. %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if('o' == argv[0][0])
    {
        psess->info.role = ERPS_ROLE_OWNER;
    }
    else if('n' == argv[0][0])
    {
        psess->info.role = ERPS_ROLE_NEIGHBOUR;
    }

    if('e' == argv[1][0])
    {
    	if(0 == psess->info.east_interface)
	    {
	        vty_error_out(vty, "Configure protection east port first. %s", VTY_NEWLINE);
	        return CMD_WARNING;
	    }
        psess->info.rpl_interface = psess->info.east_interface;
        psess->info.block_interface = psess->info.east_interface;
    }
    else if('w' == argv[1][0])
    {
    	if(0 == psess->info.west_interface)
	    {
	        vty_error_out(vty, "Configure protection west port first. %s", VTY_NEWLINE);
	        return CMD_WARNING;
	    }
        psess->info.rpl_interface = psess->info.west_interface;
        psess->info.block_interface = psess->info.west_interface;
    }

    return CMD_SUCCESS;
}

DEFUN(erps_rpl_role_nonowner,
      erps_rpl_role_nonowner_cmd,
      "rpl role non-owner",
      "ring protection link\n"
      "role\n"
      "non-owner \n")
{
    struct erps_sess *psess = (struct erps_sess *)vty->index;
    ERPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

    if((0 == psess->info.east_interface) && (0 == psess->info.west_interface))
    {
        vty_error_out(vty, "Configure protection port first. %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if(psess->info.role != ERPS_ROLE_INVALID)
    {
        vty_error_out(vty, "Have already configure ring protection link role. %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    psess->info.role = ERPS_ROLE_NONOWNER;
    psess->info.rpl_interface = 0;

    return CMD_SUCCESS;
}

DEFUN(no_erps_rpl_role,
      no_erps_rpl_role_cmd,
      "no rpl role",
      "Delete\n"
      "ring protection link\n"
      "role\n")
{
    struct erps_sess *psess = (struct erps_sess *)vty->index;
    ERPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

    if(ERPS_ROLE_INVALID == psess->info.role)
    {
        vty_error_out(vty, "Have not configure ring protection link role. %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    psess->info.role = ERPS_ROLE_INVALID;
    psess->info.rpl_interface = 0;

    return CMD_SUCCESS;
}

DEFUN(erps_data_traffic,
      erps_data_traffic_cmd,
      "data-traffic <1-4094> {to <1-4094> }",
      "data-traffic\n"
      "Range of data vlan:<1-4094>\n"
      "data-traffic end\n"
      "Range of data vlan:<1-4094>\n")
{
    struct erps_sess *psess = (struct erps_sess *)vty->index;
    uint16_t dvlan_start = 0;
    uint16_t dvlan_end = 0;
    struct erps_sess *sess = NULL;
    struct hash_bucket *pbucket = NULL;
    int cursor;
    int ret;
    ERPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

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
            dvlan_end = atoi(argv[1]);
        }
    }
    else
    {
        dvlan_end = atoi(argv[0]);
    }

    if((dvlan_start < 1 || dvlan_start > 4094) || (dvlan_end < 1 || dvlan_end > 4094))
    {
        vty_error_out(vty, "Invalid VLAN id value.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
	
	HASH_BUCKET_LOOP(pbucket, cursor, erps_session_table)
	{
		sess = (struct erps_sess *)pbucket->data;

		if(psess->info.sess_id != sess->info.sess_id)
		{
			if(((psess->info.east_interface == sess->info.east_interface) ||
					(psess->info.west_interface == sess->info.west_interface)) ||
					((psess->info.west_interface == sess->info.east_interface) ||
					 (psess->info.east_interface == sess->info.west_interface)))
			{
				ret = erps_vlan_map_isset(sess->info.data_vlan_map, dvlan_start, dvlan_end);

				if(ret)
				{
					vty_error_out(vty, "Vlan %d already configured as session %d data vlan.%s", ret, sess->info.sess_id, VTY_NEWLINE);
					return CMD_WARNING;
				}

			}

			if(sess->info.pvlan >= dvlan_start && sess->info.pvlan <= dvlan_end)
			{
				vty_error_out(vty, " Vlan %d already configured as session %d protocol vlan.%s", sess->info.pvlan, sess->info.sess_id, VTY_NEWLINE);
				return CMD_WARNING;
			}
            
		}
	}

	psess->info.dataVlanMapIsSet = 1;
    erps_vlan_map_set(psess->info.data_vlan_map, dvlan_start, dvlan_end);
    return CMD_SUCCESS;
}

DEFUN(no_erps_data_traffic,
      no_erps_data_traffic_cmd,
      "no data-traffic <1-4094> {to <1-4094> }",
      "Delete\n"
      "data-traffic\n"
      "Range of data vlan:<1-4094>\n"
      "data-traffic end\n"
      "Range of data vlan:<1-4094>\n")
{
    struct erps_sess *psess = (struct erps_sess *)vty->index;
    uint16_t dvlan_start = 0;
    uint16_t dvlan_end = 0;
    uint16_t ret;

    ERPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

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
            dvlan_end = atoi(argv[1]);
        }
    }
    else
    {
        dvlan_end = atoi(argv[0]);
    }

    ret = erps_vlan_map_isset(psess->info.data_vlan_map, dvlan_start, dvlan_end);

    if(0 == ret)
    {
        vty_error_out(vty, "Vlan not configured as session %d data vlan.%s", psess->info.sess_id, VTY_NEWLINE);
        return CMD_WARNING;
    }
    else
    {
        erps_vlan_map_unset(psess->info.data_vlan_map, dvlan_start, dvlan_end);
    }

    return CMD_SUCCESS;
}

DEFUN(no_erps_data_traffic_all,
      no_erps_data_traffic_all_cmd,
      "no data-traffic all",
      "Delete\n"
      "data-traffic\n"
      "all\n")
{
    struct erps_sess *psess = (struct erps_sess *)vty->index;
    int ret;
    ERPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

    ret = erps_vlan_map_isset(psess->info.data_vlan_map, 1, 4094);

    if(ret)
    {
        erps_vlan_map_unset(psess->info.data_vlan_map, 1, 4094);
    }
    else
    {
        vty_error_out(vty, "Data traffic is not configured%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}



DEFUN(erps_raps_channel,
      erps_raps_channel_cmd,
      "raps-channel <1-4094>",
      "raps-channel\n"
      "Range of primary vlan:<1-4094>\n")
{
    struct erps_sess *psess = (struct erps_sess *)vty->index;
    uint16_t raps_vlan = 0;
    struct erps_sess *sess = NULL;
    struct hash_bucket *pbucket = NULL;
    int cursor;
    int ret;

    ERPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

    raps_vlan = atoi(argv[0]);

    if(raps_vlan < 1 || raps_vlan > 4094)
    {
        vty_error_out(vty, "Invalid VLAN id value.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if(psess->info.pvlan != 0)
    {
        vty_error_out(vty, "Raps channel is already configured%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    HASH_BUCKET_LOOP(pbucket, cursor, erps_session_table)
    {
        sess = (struct erps_sess *)pbucket->data;

        if(psess->info.sess_id != sess->info.sess_id)
        {
        	if(sess->info.pvlan == raps_vlan)
            {
                vty_error_out(vty, "Vlan %d already configured as session %d protocol vlan.%s", raps_vlan, sess->info.sess_id, VTY_NEWLINE);
                return CMD_WARNING;
            }

			ret = erps_vlan_map_isset(sess->info.data_vlan_map, raps_vlan, raps_vlan);
			
			if(ret)
			{
				vty_error_out(vty, "Vlan %d already configured as session %d data vlan.%s", raps_vlan, sess->info.sess_id, VTY_NEWLINE);
				return CMD_WARNING;
			}
        }
    }

    psess->info.pvlan = raps_vlan;
    return CMD_SUCCESS;
}

DEFUN(no_erps_raps_channel,
      no_erps_raps_channel_cmd,
      "no raps-channel",
      "Delete\n"
      "raps-channel\n")
{
    struct erps_sess *psess = (struct erps_sess *)vty->index;

    ERPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

    if(psess->info.pvlan != 0)
    {
        psess->info.pvlan = 0;
    }
    else
    {
        vty_error_out(vty, "Raps channel is not configured%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}



DEFUN(erps_backup_create,
      erps_backup_create_cmd,
      "backup non-failback ",
      "Protection restoration mode\n"
      "Non-revertive mode\n")
{
    struct erps_sess *psess = (struct erps_sess *)vty->index;

    ERPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

    psess->info.failback = FAILBACK_DISABLE;
    psess->info.wtr = 0;
    return CMD_SUCCESS;
}

DEFUN(erps_backup_failback_create,
      erps_backup_failback_cmd,
      "backup failback [ wtr <1-12> ]",
      "Protection restoration mode\n"
      "Revertive mode\n"
      "Wait to restore\n"
      "Range of wtr:<1-12>,step:60s\n")
{
    struct erps_sess *psess = (struct erps_sess *)vty->index;
    uint16_t wtr = 0;

    ERPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

    psess->info.failback = FAILBACK_ENABLE;

    if(argv[0] != NULL)
    {
        wtr = atoi(argv[0]);

        if(wtr < 1 || wtr > 12)
        {
            vty_error_out(vty, "Invalid wtr value.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        psess->info.wtr = wtr;
    }
    else
    {
        psess->info.wtr = 5;
    }

    return CMD_SUCCESS;
}

DEFUN(no_erps_backup_failback_create,
      no_erps_backup_failback_cmd,
      "no backup",
      "Delete\n"
      "Protection restoration mode\n")
{
    struct erps_sess *psess = (struct erps_sess *)vty->index;

    ERPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

    psess->info.failback = FAILBACK_ENABLE;
    psess->info.wtr = 5;

    return CMD_SUCCESS;
}


DEFUN(erps_hold_off ,
      erps_hold_off_cmd,
      "hold-off <0-100>",
      "hold-off \n"
      "Range of hold off:<0-100>,step:100ms\n")
{
    struct erps_sess *psess = (struct erps_sess *)vty->index;
    uint16_t hold_off = 0;

    hold_off = atoi(argv[0]);

    psess->info.holdoff = hold_off * 100;

    if(SESSION_STATUS_ENABLE == psess->info.status)
    {
        erps_sess_update(psess);
    }

    return CMD_SUCCESS;
}

DEFUN(no_erps_hold_off ,
      no_erps_hold_off_cmd,
      "no hold-off",
      "Delete\n"
      "hold-off \n")
{
    struct erps_sess *psess = (struct erps_sess *)vty->index;

    psess->info.holdoff = 0;

    if(SESSION_STATUS_ENABLE == psess->info.status)
    {
        erps_sess_update(psess);
    }

    return CMD_SUCCESS;
}

DEFUN(erps_keep_alive,
      erps_keep_alive_cmd,
      "keep-alive <1-600>",
      "Erps keep alive\n"
      "Range of keep alive:<1s-600s>\n")
{
    struct erps_sess *psess = (struct erps_sess *)vty->index;
    uint16_t keep_alive = 0;

    erps_stop_msg_timer(psess);

    keep_alive = atoi(argv[0]);
    psess->info.keepalive = keep_alive;

    if(SESSION_STATUS_ENABLE == psess->info.status)
    {
        erps_start_msg_timer(psess);
        erps_sess_update(psess);
    }

    return CMD_SUCCESS;
}

DEFUN(no_erps_keep_alive,
      no_erps_keep_alive_cmd,
      "no keep-alive",
      "Delete\n"
      "Erps keep alive \n")
{
    struct erps_sess *psess = (struct erps_sess *)vty->index;

    erps_stop_msg_timer(psess);
    psess->info.keepalive = 5;

    if(SESSION_STATUS_ENABLE == psess->info.status)
    {
        erps_start_msg_timer(psess);
        erps_sess_update(psess);
    }

    return CMD_SUCCESS;
}


DEFUN(erps_guard_timer ,
      erps_guard_timer_cmd,
      "guard-timer <1-200>",
      "guard-timer \n"
      "Range of guard timer:<1-200>,step:10ms\n")
{
    struct erps_sess *psess = (struct erps_sess *)vty->index;
    uint16_t guard_timer = 0;

    if(psess->guard_timer)
    {
        vty_error_out(vty, "guard timer is running.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    guard_timer = atoi(argv[0]);

    if(guard_timer < 1 || guard_timer > 200)
    {
        vty_error_out(vty, "Invalid hold off value.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    psess->info.guardtimer = guard_timer * 10;
    psess->info.wtb = psess->info.guardtimer + 5000;

    if(SESSION_STATUS_ENABLE == psess->info.status)
    {
        erps_sess_update(psess);
    }

    return CMD_SUCCESS;
}

DEFUN(no_erps_guard_timer ,
      no_erps_guard_timer_cmd,
      "no guard-timer",
      "Delete\n"
      "guard-timer \n")
{
    struct erps_sess *psess = (struct erps_sess *)vty->index;

    if(psess->guard_timer)
    {
        vty_error_out(vty, "guard timer is running.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    psess->info.guardtimer = 500;
    psess->info.wtb = psess->info.guardtimer + 5000;

    if(SESSION_STATUS_ENABLE == psess->info.status)
    {
        erps_sess_update(psess);
    }

    return CMD_SUCCESS;
}

DEFUN(erps_ring_id ,
      erps_ring_id_cmd,
      "ring-id <1-255>",
      "ring id\n"
      "Range of ring id :<1-255>\n")
{
    struct erps_sess *psess = (struct erps_sess *)vty->index;
    uint8_t ringid = 0;

    ringid = atoi(argv[0]);
    psess->info.ring_id = ringid;

    if(SESSION_STATUS_ENABLE == psess->info.status)
    {
        erps_sess_update(psess);
    }

    return CMD_SUCCESS;
}

DEFUN(erps_level ,
      erps_level_cmd,
      "level <0-7>",
      "level\n"
      "Range of level:<0-7>.The default value is 7\n")
{
    struct erps_sess *psess = (struct erps_sess *)vty->index;
    uint8_t level = 0;

    level = atoi(argv[0]);
    psess->info.level = level;

    if(SESSION_STATUS_ENABLE == psess->info.status)
    {
        erps_sess_update(psess);
    }

    return CMD_SUCCESS;
}

DEFUN(erps_bind_cfm_session,
      erps_bind_cfm_session_cmd,
      "bind cfm session <1-1024> (east-interface | west-interface)",
      "Bind\n"
      "Connectivity fault management\n"
      "Session\n"
      "Session index ,<1-1024>\n"
      "East-interface\n"
      "West-interface\n")
{
    struct erps_sess *psess = (struct erps_sess *)vty->index;
    struct cfm_sess *cfm_sess = NULL;
    struct cfm_sess *bind_cfm_sess = NULL;
    uint16_t sess_id = 0;

    ERPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

    sess_id = atoi(argv[0]);

    if(argv[1][0] == 'e')
    {
        if(0 == psess->info.east_cfm_session_id)
        {
            if(psess->info.east_interface)
            {
                cfm_sess = erps_find_cfm(psess->info.east_interface, sess_id);

                if(cfm_sess)
                {
                    psess->east_cfm_session = cfm_sess;
                    psess->info.east_cfm_session_id = sess_id;
                    bind_cfm_sess = cfm_session_bind_elps(sess_id);

                    if(NULL == bind_cfm_sess)
                    {
                        vty_error_out(vty, "bind cfm session failed%s", VTY_NEWLINE);
                        psess->info.east_cfm_session_id = 0;
                        psess->east_cfm_session = NULL;
                        return CMD_WARNING;
                    }
                }
                else
                {
                    vty_error_out(vty, "East interface is not configured cfm session %d %s", sess_id, VTY_NEWLINE);
                    return CMD_WARNING;
                }
            }
            else
            {
                vty_error_out(vty, "East interface is subring block %s", VTY_NEWLINE);
                return CMD_WARNING;
            }
        }
        else
        {
            vty_error_out(vty, "East interface is already configured cfm session %s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    if(argv[1][0] == 'w')
    {
        if(0 == psess->info.west_cfm_session_id)
        {
            if(psess->info.west_interface)
            {
                cfm_sess = erps_find_cfm(psess->info.west_interface, sess_id);

                if(cfm_sess)
                {
                    psess->west_cfm_session = cfm_sess;
                    psess->info.west_cfm_session_id = sess_id;
                    bind_cfm_sess = cfm_session_bind_elps(sess_id);

                    if(NULL == bind_cfm_sess)
                    {
                        vty_error_out(vty, "bind cfm session failed%s", VTY_NEWLINE);
                        psess->info.west_cfm_session_id = 0;
                        psess->west_cfm_session = NULL;
                        return CMD_WARNING;
                    }
                }
                else
                {
                    vty_error_out(vty, "West interface is not configured cfm session %d %s", sess_id, VTY_NEWLINE);
                    return CMD_WARNING;
                }
            }
            else
            {
                vty_error_out(vty, "West interface is subring block %s", VTY_NEWLINE);
                return CMD_WARNING;
            }
        }
        else
        {
            vty_error_out(vty, "West interface is already configured cfm session%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    return CMD_SUCCESS;
}

DEFUN(no_erps_bind_cfm_session,
      no_erps_bind_cfm_session_cmd,
      "no bind cfm session (east-interface | west-interface)",
      "Delete\n"
      "Bind\n"
      "Connectivity fault management\n"
      "Session\n"
      "East-interface\n"
      "West-interface\n")

{
    struct erps_sess *psess = (struct erps_sess *)vty->index;
    struct cfm_sess *unbind_cfm_sess = NULL;

    ERPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

    if('e' == argv[0][0])
    {
        unbind_cfm_sess = cfm_session_unbind_elps(psess->info.east_cfm_session_id);

        if(NULL == unbind_cfm_sess)
        {
            vty_error_out(vty, "unbind cfm session failed%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        psess->info.east_cfm_session_id = 0;
        psess->east_cfm_session = NULL;
    }

    if('w' == argv[0][0])
    {
        unbind_cfm_sess = cfm_session_unbind_elps(psess->info.west_cfm_session_id);

        if(NULL == unbind_cfm_sess)
        {
            vty_error_out(vty, "unbind cfm session failed%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        psess->info.west_cfm_session_id = 0;
        psess->west_cfm_session = NULL;
    }

    return CMD_SUCCESS;
}

DEFUN(erps_virtual_channel,
      erps_virtual_channel_cmd,
      "sub-ring attached-to-session <1-255>",
      "subring\n"
      "attached to master ring session id\n"
      "rang of session :<1-255>\n")
{
    struct erps_sess *psess = (struct erps_sess *)vty->index;
    struct erps_sess *attach_to_sess = NULL;
    uint16_t attached_session_id = 0;

    ERPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

	if(ERPS_ROLE_INVALID == psess->info.role)
    {
        vty_error_out(vty, " Configured rpl role first.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	if(psess->info.attach_sess_id != 0)
    {
        vty_error_out(vty, "ALready configured as interconnection node.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    attached_session_id = atoi(argv[0]);

    if(psess->info.attach_sess_id != 0)
    {
        vty_error_out(vty, "Already configured attach to session %d .%s", psess->info.attach_sess_id, VTY_NEWLINE);
        return CMD_WARNING;
    }
    else
    {
        attach_to_sess = erps_sess_lookup(attached_session_id);

        if(NULL == attach_to_sess)
        {
            vty_error_out(vty, "Session %d not configured.%s", attached_session_id, VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
  
    psess->info.attach_sess_id = attached_session_id;
    psess->info.sub_ring = 1;
    psess->attach_to_sess = attach_to_sess;

    return CMD_SUCCESS;
}


DEFUN(np_erps_sub_ring_block,
      no_erps_sub_ring_block_cmd,
      "no sub-ring",
      "Delete\n"
      "Sub-ring\n")
{
    struct erps_sess *psess = (struct erps_sess *)vty->index;

    ERPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

    if((psess->info.east_interface != 0) && (psess->info.west_interface != 0))
    {
        vty_error_out(vty, "Not configured as sub-ring. %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    psess->info.sub_ring = 0 ;
    psess->info.attach_sess_id = 0;
    if(psess->attach_to_sess != NULL)
    {
        psess->attach_to_sess = NULL;
    }

    return CMD_SUCCESS;
}


DEFUN(erps_session_enable,
      erps_session_enable_cmd,
      "erps (enable | disable)",
      "Erps management\n"
      "Enable\n"
      "Disable\n")
{
    struct erps_sess *psess = (struct erps_sess *)vty->index;
    int ret = 0;

    if('e' == argv[0][0])
    {
        ERPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

        if((0 == psess->info.west_interface) && (0 == psess->info.east_interface))
        {
            vty_error_out(vty, "ERPS port not configure.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        if(0 == psess->info.pvlan)
        {
            vty_error_out(vty, "ERPS raps channel not configure.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        if(0 == psess->info.ring_id)
        {
            vty_error_out(vty, "ERPS ring ID not configure.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        if(ERPS_ROLE_INVALID == psess->info.role)
        {
            vty_error_out(vty, "ERPS rpl role not configure.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        if(psess->info.level > 7)
        {
            vty_error_out(vty, "ERPS level not configure.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        psess->r_aps.level = psess->info.level;
        psess->r_aps.version = 1;
        psess->r_aps.opcode = CFM_RAPS_OPCODE;
        psess->r_aps.tlv_offset = 0x20;
        gerps.sess_enable++;

        ret = erps_sess_enable(psess);

        if(ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "ERPS sent failed.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        erps_init_handle_initialization(psess);

        erps_init_session(psess);


    }
    else if('d' == argv[0][0])
    {
        ERPS_SESSION_IS_DISABLE(vty, psess->info.status);
        ret = erps_sess_disable(psess);

        if(ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "ERPS sent failed.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

		erps_clear_timer(psess);
        gerps.sess_enable--;
    }

    return CMD_SUCCESS;
}



DEFUN(erps_debug_packet,
      erps_debug_packet_cmd,
      "debug erps packet",
      "DEBUG_STR"
      "Erps management\n"
      "Erps packet\n")
{
    if(!gerps.sess_enable)
    {
        vty_warning_out(vty, "Please enable ERPS function at first%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    gerps.debug_packet = 1;
    return CMD_SUCCESS;
}

DEFUN(no_erps_debug_packet,
      no_erps_debug_packet_cmd,
      "no debug erps packet",
      "Delete\n"
      "DEBUG_STR"
      "Erps management\n"
      "Erps packet\n")
{
    if(!gerps.sess_enable)
    {
        vty_warning_out(vty, "Please enable ERPS function at first%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    gerps.debug_packet = 0;
    return CMD_SUCCESS;

}

//admin command

DEFUN(erps_admin_force,
      erps_admin_force_cmd,
      "(force-switch|manual-switch) (east-interface | west-interface)",
      "Force switch\n"
      "Manual switch\n"
      "East interface will be block\n"
      "West interface will be block\n")
{
    struct erps_sess *psess = (struct erps_sess *)vty->index;

    ERPS_SESSION_IS_DISABLE(vty, psess->info.status);

    if( 'e' == argv[1][0])
    {
        if(psess->info.east_interface)
        {
            psess->info.event_block = psess->info.east_interface;
        }
        else
        {
            vty_error_out(vty, "East interface is not exist.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    else if('w' == argv[1][0])
    {
        if(psess->info.west_interface)
        {
            psess->info.event_block = psess->info.west_interface;
        }
        else
        {
            vty_error_out(vty, "West interface is not exist.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    if( 'f' == argv[0][0])
    {
        psess->info.current_event = ERPS_EVENT_ADMIN_FS;
    }
    else if('m' == argv[0][0])
    {
        psess->info.current_event = ERPS_EVENT_ADMIN_MS;
    }


    erps_fsm(psess, NULL);
    return CMD_SUCCESS;

}

DEFUN(erps_admin_clear,
      erps_admin_clear_cmd,
      "clear",
      "Clear\n")
{
    struct erps_sess *psess = (struct erps_sess *)vty->index;

    ERPS_SESSION_IS_DISABLE(vty, psess->info.status);

    psess->info.current_event = ERPS_EVENT_ADMIN_CLEAR;
    psess->info.event_block = 0;

    erps_fsm(psess, NULL);

    if(psess->info.east_interface_flag)
    {
        erps_state_update(psess->info.east_interface, 0, ERPS_PORT_DOWN);
    }

    if(psess->info.west_interface_flag)
    {
        erps_state_update(psess->info.west_interface, 0, ERPS_PORT_DOWN);
    }

    return CMD_SUCCESS;

}


//show command

DEFUN(show_erps_config,
      show_erps_config_cmd,
      "show erps config",
      "Display.\n"
      "Erps management\n"
      "Config\n")
{
    struct hash_bucket *bucket = NULL;
    struct erps_sess *psess = NULL;
    int cursor = 0;
    int num = 0;

    HASH_BUCKET_LOOP(bucket, cursor, erps_session_table)
    {
        psess = (struct erps_sess *)bucket->data;
        vty_out(vty, "%4s:%d%s", "Num", ++num, VTY_NEWLINE);
        eprs_show_config(vty, psess);
        psess = NULL;
    }
    return CMD_SUCCESS;
}


DEFUN(show_erps_session,
      show_erps_session_cmd,
      "show erps session [<1-255>]",
      "Display.\n"
      "Erlps management\n"
      "Session\n"
      "Range of session id:<1-255>\n")
{
    struct hash_bucket *bucket = NULL;
    struct erps_sess *psess = NULL;
    uint16_t sess_id = 0;
    int cursor = 0;
    int num = 0;

    if(NULL == argv[0])//显示所有session信息
    {
        vty_out(vty, "%4s%-18s: %d%s", "", "Total", gerps.sess_total, VTY_NEWLINE);
        vty_out(vty, "%4s%-18s: %d%s", "", "enable_num", gerps.sess_enable, VTY_NEWLINE);

        HASH_BUCKET_LOOP(bucket, cursor, erps_session_table)
        {
            psess = (struct erps_sess *)bucket->data;
            vty_out(vty, "%-4s:%d%s", "Num", ++num, VTY_NEWLINE);
            erps_show_session(vty, psess);
            psess = NULL;
        }
    }
    else
    {
        sess_id = atoi(argv[0]);
        psess = erps_sess_lookup(sess_id);

        if(NULL == psess)
        {
            vty_out(vty, "Error:Session:%d is not exist!%s", sess_id, VTY_NEWLINE);
            return CMD_WARNING;
        }

        erps_show_session(vty, psess);
    }

    return CMD_SUCCESS;
}

static int erps_session_config_write(struct vty *vty)
{
    struct hash_bucket *bucket = NULL;
    struct erps_sess *psess = NULL;
    char ifname_w[NAME_STRING_LEN], ifname_e[NAME_STRING_LEN];
    int cursor = 0;
    int guard, holdoff;
    int ret;
    HASH_BUCKET_LOOP(bucket, cursor, erps_session_table)
    {
        psess = (struct erps_sess *)bucket->data;

        vty_out(vty, "erps session %d%s", psess->info.sess_id, VTY_NEWLINE);

        if(psess->info.east_interface != 0 && psess->info.west_interface != 0)
        {	
            memset(ifname_e, 0, sizeof(char)*NAME_STRING_LEN);
		    memset(ifname_w, 0, sizeof(char)*NAME_STRING_LEN);
            ifm_get_name_by_ifindex(psess->info.east_interface, ifname_e);
			ifm_get_name_by_ifindex(psess->info.west_interface, ifname_w);
			if((ifname_e[0] != 'u') && (ifname_w[0] != 'u') && (ifname_e[0] != 0) && (ifname_w[0] != 0))
	        {
	            vty_out(vty, " physical-ring east-interface %s west-interface %s%s", ifname_e, ifname_w, VTY_NEWLINE);
	        }
        }
        else if(psess->info.east_interface != 0 && psess->info.west_interface == 0)
        {
        	memset(ifname_e, 0, sizeof(char)*NAME_STRING_LEN);
            ifm_get_name_by_ifindex(psess->info.east_interface, ifname_e);
			if((ifname_e[0] != 'u') && (ifname_e[0] != 0) )
	        {
	            vty_out(vty, " physical-subring east-interface %s%s", ifname_e, VTY_NEWLINE);
	        }
        }

        if(psess->info.role)
        {
            if(ERPS_ROLE_OWNER == psess->info.role)
            {
                if(psess->info.rpl_interface == psess->info.east_interface)
                {
                    vty_out(vty, " rpl role owner %s%s", "east-interface", VTY_NEWLINE);
                }
                else if(psess->info.rpl_interface == psess->info.west_interface)
                {
                    vty_out(vty, " rpl role owner %s%s", "west-interface", VTY_NEWLINE);
                }
            }
            else if(ERPS_ROLE_NEIGHBOUR == psess->info.role)
            {
                if(psess->info.rpl_interface == psess->info.east_interface)
                {
                    vty_out(vty, " rpl role neighbor %s%s", "east-interface", VTY_NEWLINE);
                }
                else if(psess->info.rpl_interface == psess->info.west_interface)
                {
                    vty_out(vty, " rpl role neighbor %s%s", "west-interface", VTY_NEWLINE);
                }
            }
            else if(ERPS_ROLE_NONOWNER == psess->info.role)
            {
                vty_out(vty, " rpl role non-owner %s", VTY_NEWLINE);
            }

        }

        ret = erps_vlan_map_isset(psess->info.data_vlan_map, 1, 4094);

        if(ret)
        {
            erps_vlan_show(vty, psess, 1);

        }

        if(psess->info.pvlan)
        {
            vty_out(vty, " raps-channel %d%s", psess->info.pvlan, VTY_NEWLINE);
        }

        if(psess->info.holdoff)
        {
            holdoff = psess->info.holdoff / 100;
            vty_out(vty, " hold-off %d%s", holdoff, VTY_NEWLINE);
        }

        if((psess->info.guardtimer) && (psess->info.guardtimer != 500))
        {
            guard = psess->info.guardtimer / 10;
            vty_out(vty, " guard-timer %d%s", guard, VTY_NEWLINE);
        }

        if((psess->info.keepalive) && (psess->info.keepalive != 5))
        {
            vty_out(vty, " keep-alive %d%s", psess->info.keepalive, VTY_NEWLINE);
        }

        if(psess->info.failback == FAILBACK_ENABLE && psess->info.wtr != 5)
        {
            vty_out(vty, " backup failback wtr %d %s", psess->info.wtr, VTY_NEWLINE);
        }
		else if(psess->info.failback == FAILBACK_DISABLE)
		{
			vty_out(vty, " backup non-failback %s", VTY_NEWLINE);
		}

        if(psess->info.ring_id)
        {
            vty_out(vty, " ring-id %d%s", psess->info.ring_id, VTY_NEWLINE);
        }

        if((psess->info.level) && (psess->info.level != 7))
        {
            vty_out(vty, " level %d%s", psess->info.level, VTY_NEWLINE);
        }

        if(psess->info.east_cfm_session_id)
        {
            vty_out(vty, " bind cfm session %d east-interface %s", psess->info.east_cfm_session_id, VTY_NEWLINE);
        }

        if(psess->info.west_cfm_session_id)
        {
            vty_out(vty, " bind cfm session %d west-interface %s", psess->info.west_cfm_session_id, VTY_NEWLINE);
        }

        if(psess->info.attach_sess_id != 0)
        {
        	if(psess->info.attach_sess_id < psess->info.sess_id)
        	{
				vty_out(vty, " sub-ring attached-to-session %d%s", psess->info.attach_sess_id, VTY_NEWLINE);
				if(psess->info.status == SESSION_STATUS_ENABLE)
				{
					vty_out(vty, " erps %s%s", "enable", VTY_NEWLINE);
				}	
        	}
        }
		else
		{
			if(psess->info.status == SESSION_STATUS_ENABLE)
	        {
	            vty_out(vty, " erps %s%s", "enable", VTY_NEWLINE);
	        }
		}
        
    }

	cursor = 0;
	HASH_BUCKET_LOOP(bucket, cursor, erps_session_table)
	{
		psess = (struct erps_sess *)bucket->data;
		if((psess->info.attach_sess_id > psess->info.sess_id) && (psess->info.attach_sess_id != 0))
		{ 
			vty_out(vty, " erps session %d%s", psess->info.sess_id, VTY_NEWLINE);
			vty_out(vty, " sub-ring attached-to-session %d%s", psess->info.attach_sess_id, VTY_NEWLINE);
			if(psess->info.status == SESSION_STATUS_ENABLE)
			{
				vty_out(vty, " erps %s%s", "enable", VTY_NEWLINE);
			}
		}
	}
    return ERRNO_SUCCESS;
}

void erps_cmd_init(void)
{
    install_node(&erps_session_node, erps_session_config_write);
	
    install_default(ERPS_SESSION_NODE);

    install_element(CONFIG_NODE, &erps_session_cmd,CMD_SYNC);
    install_element(CONFIG_NODE, &no_erps_session_cmd,CMD_SYNC);
    install_element(CONFIG_NODE, &erps_debug_packet_cmd,CMD_LOCAL);
    install_element(CONFIG_NODE, &no_erps_debug_packet_cmd,CMD_LOCAL);


    install_element(ERPS_SESSION_NODE, &erps_physical_ring_cmd,CMD_SYNC);
	install_element(ERPS_SESSION_NODE, &erps_physical_subring_cmd,CMD_SYNC);
    install_element(ERPS_SESSION_NODE, &no_erps_physical_ring_cmd,CMD_SYNC);
	install_element(ERPS_SESSION_NODE, &no_erps_physical_subring_cmd,CMD_SYNC);
    install_element(ERPS_SESSION_NODE, &erps_rpl_role_cmd,CMD_SYNC);
    install_element(ERPS_SESSION_NODE, &erps_rpl_role_nonowner_cmd,CMD_SYNC);
    install_element(ERPS_SESSION_NODE, &no_erps_rpl_role_cmd,CMD_SYNC);
    install_element(ERPS_SESSION_NODE, &erps_data_traffic_cmd,CMD_SYNC);
    install_element(ERPS_SESSION_NODE, &no_erps_data_traffic_cmd,CMD_SYNC);
    install_element(ERPS_SESSION_NODE, &no_erps_data_traffic_all_cmd,CMD_SYNC);
    install_element(ERPS_SESSION_NODE, &erps_raps_channel_cmd,CMD_SYNC);
    install_element(ERPS_SESSION_NODE, &no_erps_raps_channel_cmd,CMD_SYNC);
    install_element(ERPS_SESSION_NODE, &erps_backup_failback_cmd,CMD_SYNC);
    install_element(ERPS_SESSION_NODE, &erps_backup_create_cmd,CMD_SYNC);
    install_element(ERPS_SESSION_NODE, &no_erps_backup_failback_cmd,CMD_SYNC);
    install_element(ERPS_SESSION_NODE, &erps_hold_off_cmd,CMD_SYNC);
    install_element(ERPS_SESSION_NODE, &no_erps_hold_off_cmd,CMD_SYNC);
    install_element(ERPS_SESSION_NODE, &erps_keep_alive_cmd,CMD_SYNC);
    install_element(ERPS_SESSION_NODE, &no_erps_keep_alive_cmd,CMD_SYNC);
    install_element(ERPS_SESSION_NODE, &erps_guard_timer_cmd,CMD_SYNC);
    install_element(ERPS_SESSION_NODE, &no_erps_guard_timer_cmd,CMD_SYNC);
    install_element(ERPS_SESSION_NODE, &erps_ring_id_cmd,CMD_SYNC);
    install_element(ERPS_SESSION_NODE, &erps_level_cmd,CMD_SYNC);
    install_element(ERPS_SESSION_NODE, &erps_bind_cfm_session_cmd,CMD_SYNC);
    install_element(ERPS_SESSION_NODE, &no_erps_bind_cfm_session_cmd,CMD_SYNC);
    install_element(ERPS_SESSION_NODE, &erps_virtual_channel_cmd,CMD_SYNC);
    install_element(ERPS_SESSION_NODE, &no_erps_sub_ring_block_cmd,CMD_SYNC);
    install_element(ERPS_SESSION_NODE, &erps_session_enable_cmd,CMD_SYNC);

    install_element(ERPS_SESSION_NODE, &erps_admin_force_cmd,CMD_LOCAL);
    install_element(ERPS_SESSION_NODE, &erps_admin_clear_cmd,CMD_LOCAL);

    install_element(CONFIG_NODE, &show_erps_config_cmd,CMD_LOCAL);
    install_element(CONFIG_NODE, &show_erps_session_cmd,CMD_LOCAL);

}


void erps_vlan_show(struct vty *vty, struct erps_sess *psess, uint8_t is_write)
{
    uint16_t vlan_list[4095] = {0};
    uint16_t i, j = 0;
    uint16_t vlan_start = 0;
    int ret;

    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);

    for(i = 1; i < 4095; i++)
    {
        ret = erps_vlan_map_isset(psess->info.data_vlan_map, i, i);

        if(ret)
        {
            vlan_list[j] = ret;
            j++;
        }
    }

    for(i = 0; i < j - 1; i++)
    {
        if((vlan_list[i + 1] - vlan_list[i]) == 1)
        {
            if(0 == vlan_start)
            {
                vlan_start = vlan_list[i];
            }
        }
        else
        {
            if(0 == vlan_start)
            {

                if(is_write)
                {
                    vty_out(vty, " data-traffic %d%s", vlan_list[i], VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, "%d, ", vlan_list[i]);
                }
            }
            else
            {
                if(is_write)
                {
                    vty_out(vty, " data-traffic %d to %d%s", vlan_start, vlan_list[i], VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, "%d - %d, ", vlan_start, vlan_list[i]);
                }

                vlan_start = 0;

            }
        }
    }

    if(vlan_start)
    {

        if(is_write)
        {
            vty_out(vty, " data-traffic %d to %d%s", vlan_start, vlan_list[i], VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, "%d - %d ", vlan_start, vlan_list[i]);
        }

        vlan_start = 0;
    }
    else
    {
        if(is_write)
        {
            vty_out(vty, " data-traffic %d%s", vlan_list[i], VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, "%d ", vlan_list[i]);
        }
    }
}

void eprs_show_config(struct vty *vty, struct erps_sess *psess)
{
    char ifname[NAME_STRING_LEN];
	char data_vlan_map[VLAN_MEAP_MAX];
	
    if(NULL == psess)
    {
        return ;
    }

	memset(data_vlan_map, 0, sizeof(char)*VLAN_MEAP_MAX);
    vty_out(vty, "%4s%-18s: %d%s", "", "session-id", psess->info.sess_id, VTY_NEWLINE);

    if(SESSION_STATUS_DISABLE == psess->info.status)
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "session-status", "disable", VTY_NEWLINE);
    }
    else if(SESSION_STATUS_ENABLE == psess->info.status)
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "session-status", "enable", VTY_NEWLINE);
    }

    if(ERPS_ROLE_OWNER == psess->info.role)
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "session-role", "owner", VTY_NEWLINE);
    }
    else if(ERPS_ROLE_NEIGHBOUR == psess->info.role)
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "session-role", "neighbour", VTY_NEWLINE);
    }
    else if(ERPS_ROLE_NONOWNER == psess->info.role)
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "session-role", "non-owner", VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "session-role", "--", VTY_NEWLINE);
    }

    ifm_get_name_by_ifindex(psess->info.east_interface, ifname);

    if(ifname[0] != 'u')
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "east_interface", ifname, VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "east_interface", "--", VTY_NEWLINE);
    }

    ifm_get_name_by_ifindex(psess->info.west_interface, ifname);

    if(ifname[0] != 'u')
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "west_interface", ifname, VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "west_interface", "--", VTY_NEWLINE);
    }

    if((psess->info.east_interface) && (psess->info.rpl_interface == psess->info.east_interface))
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "rpl_interface", "east interface", VTY_NEWLINE);
    }
    else if((psess->info.west_interface) && (psess->info.rpl_interface == psess->info.west_interface))
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "rpl_interface", "west interface", VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "rpl_interface", "--", VTY_NEWLINE);
    }

	if(memcmp(data_vlan_map, psess->info.data_vlan_map, sizeof(char)*VLAN_MEAP_MAX) == 0)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "data-traffic", "--", VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "%4s%-18s: ", "", "data-traffic");
	    erps_vlan_show(vty, psess, 0);
	    vty_out(vty, "%s", VTY_NEWLINE);
	}

	if(psess->info.pvlan != 0)
	{
		vty_out(vty, "%4s%-18s: %d%s", "", "raps-channel", psess->info.pvlan, VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "raps-channel", "--", VTY_NEWLINE);
	}

    if(FAILBACK_ENABLE == psess->info.failback )
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "backup-mode", "Failback", VTY_NEWLINE);
        vty_out(vty, "%4s%-18s: %d min%s", "", "WTR", psess->info.wtr, VTY_NEWLINE);
    }
    else if(FAILBACK_DISABLE == psess->info.failback)
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "backup-mode", "Non-Failback", VTY_NEWLINE);
    }

    vty_out(vty, "%4s%-18s: %dms%s", "", "hold-off", psess->info.holdoff, VTY_NEWLINE);
    vty_out(vty, "%4s%-18s: %dms%s", "", "guard-timer", psess->info.guardtimer, VTY_NEWLINE);
    vty_out(vty, "%4s%-18s: %dms%s", "", "WTB", psess->info.wtb, VTY_NEWLINE);
    vty_out(vty, "%4s%-18s: %ds%s", "", "keep-alive", psess->info.keepalive, VTY_NEWLINE);

    vty_out(vty, "%4s%-18s: %d%s", "", "priority", psess->info.priority, VTY_NEWLINE);
    
	if(psess->info.ring_id != 0)
	{
		vty_out(vty, "%4s%-18s: %d%s", "", "ring-id", psess->info.ring_id, VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "ring-id", "--", VTY_NEWLINE);
	}
	
    vty_out(vty, "%4s%-18s: %d%s", "", "level", psess->info.level, VTY_NEWLINE);

    if(psess->info.east_cfm_session_id)
    {
        vty_out(vty, "%4s%-18s: %d%s", "", "east cfm session", psess->info.east_cfm_session_id, VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "east cfm session", "--", VTY_NEWLINE);
    }

    if(psess->info.west_cfm_session_id)
    {
        vty_out(vty, "%4s%-18s: %d%s", "", "west cfm session", psess->info.west_cfm_session_id, VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "west cfm session", "--", VTY_NEWLINE);
    }

	if(psess->info.sub_ring)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "sub-ring", "YES", VTY_NEWLINE);
	}

	if(psess->info.attach_sess_id)
    {
        vty_out(vty, "%4s%-18s: %d%s", "", "attach to session", psess->info.attach_sess_id, VTY_NEWLINE);   
    }
    vty_out(vty, "%s", VTY_NEWLINE);
    return ;
}


void erps_show_session(struct vty *vty, struct erps_sess *psess)
{
    char ifname[NAME_STRING_LEN];
	char data_vlan_map[VLAN_MEAP_MAX];
	
    if(psess == NULL)
    {
        return ;
    }

	memset(data_vlan_map, 0, sizeof(char)*VLAN_MEAP_MAX);
    vty_out(vty, "%-5s%-11s%-23s%-23s%-18s%s", "id", "role", "east_interface", "west_interface", "current-state", VTY_NEWLINE);
    vty_out(vty, "----------------------------------------------------------------------------\r\n");

    vty_out(vty, "%-5d" , psess->info.sess_id);

    if(ERPS_ROLE_OWNER == psess->info.role)
    {
        vty_out(vty, "%-11s", "owner");
    }
    else if(ERPS_ROLE_NEIGHBOUR == psess->info.role)
    {
        vty_out(vty, "%-11s", "neighbour");
    }
    else if(ERPS_ROLE_NONOWNER == psess->info.role)
    {
        vty_out(vty, "%-11s", "non-owner");
    }
    else
    {
        vty_out(vty, "%-11s", "--");
    }

    ifm_get_name_by_ifindex(psess->info.east_interface, ifname);

    if(ifname[0] != 'u')
    {
        vty_out(vty, "%-23s", ifname);
    }
    else
    {
        vty_out(vty, "%-23s", "--");
    }

    ifm_get_name_by_ifindex(psess->info.west_interface, ifname);

    if(ifname[0] != 'u')
    {
        vty_out(vty, "%-23s", ifname);
    }
    else
    {
        vty_out(vty, "%-23s", "--");
    }

    if(ERPS_STATE_INIT == psess->info.current_status)
    {
        vty_out(vty, "%s%s", "Initialize", VTY_NEWLINE);
    }
    else if(ERPS_STATE_PENDING == psess->info.current_status)
    {
        vty_out(vty, "%s%s", "Pending", VTY_NEWLINE);
    }
    else if((ERPS_STATE_PROTECTION == psess->info.current_status) ||
            (ERPS_STATE_PROTECTION_REMOTE == psess->info.current_status))
    {
        vty_out(vty, "%s%s", "Protection", VTY_NEWLINE);
    }
    else if(ERPS_STATE_IDLE == psess->info.current_status)
    {
        vty_out(vty, "%s%s", "Idle", VTY_NEWLINE);
    }
    else if(ERPS_STATE_FS == psess->info.current_status)
    {
        vty_out(vty, "%s%s", "Force Switch", VTY_NEWLINE);
    }
    else if(ERPS_STATE_MS == psess->info.current_status)
    {
        vty_out(vty, "%s%s", "Manual Switch", VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "%s%s", "Invalid", VTY_NEWLINE);
    }

    vty_out(vty, "%s", VTY_NEWLINE);

    if(SESSION_STATUS_DISABLE == psess->info.status)
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "session-status", "disable", VTY_NEWLINE);
    }
    else if(SESSION_STATUS_ENABLE == psess->info.status)
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "session-status", "enable", VTY_NEWLINE);
    }

    if((psess->info.east_interface) && (psess->info.rpl_interface == psess->info.east_interface))
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "rpl_interface", "east interface", VTY_NEWLINE);
    }
    else if((psess->info.west_interface) && (psess->info.rpl_interface == psess->info.west_interface))
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "rpl_interface", "west interface", VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "rpl_interface", "--", VTY_NEWLINE);
    }

    if((psess->info.east_interface) && (psess->info.block_interface == psess->info.east_interface))
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "block_interface", "east interface", VTY_NEWLINE);
    }
    else if((psess->info.west_interface) && (psess->info.block_interface == psess->info.west_interface))
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "block_interface", "west interface", VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "block_interface", "--", VTY_NEWLINE);
    }

	if(memcmp(data_vlan_map, psess->info.data_vlan_map, sizeof(char)*VLAN_MEAP_MAX) == 0)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "data-traffic", "--", VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "%4s%-18s: ", "", "data-traffic");
	    erps_vlan_show(vty, psess, 0);
	    vty_out(vty, "%s", VTY_NEWLINE);
	}

	if(psess->info.pvlan != 0)
	{
		vty_out(vty, "%4s%-18s: %d%s", "", "raps-channel", psess->info.pvlan, VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "raps-channel", "--", VTY_NEWLINE);
	}
	
    if(FAILBACK_ENABLE == psess->info.failback)
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "backup-mode", "Failback", VTY_NEWLINE);
        vty_out(vty, "%4s%-18s: %d min%s", "", "WTR", psess->info.wtr, VTY_NEWLINE);
    }
    else if(FAILBACK_DISABLE == psess->info.failback)
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "backup-mode", "Non-Failback", VTY_NEWLINE);
    }

    vty_out(vty, "%4s%-18s: %dms%s", "", "hold-off", psess->info.holdoff, VTY_NEWLINE);
    vty_out(vty, "%4s%-18s: %dms%s", "", "guard-timer", psess->info.guardtimer, VTY_NEWLINE);
    vty_out(vty, "%4s%-18s: %dms%s", "", "WTB", psess->info.wtb, VTY_NEWLINE);
    vty_out(vty, "%4s%-18s: %ds%s", "", "keep-alive", psess->info.keepalive, VTY_NEWLINE);


    vty_out(vty, "%4s%-18s: %d%s", "", "priority", psess->info.priority, VTY_NEWLINE);

	if(psess->info.ring_id != 0)
	{
		vty_out(vty, "%4s%-18s: %d%s", "", "ring-id", psess->info.ring_id, VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "ring-id", "--", VTY_NEWLINE);
	}
	
    vty_out(vty, "%4s%-18s: %d%s", "", "level", psess->info.level, VTY_NEWLINE);

    if(psess->info.east_cfm_session_id)
    {
        vty_out(vty, "%4s%-18s: %d%s", "", "east cfm session", psess->info.east_cfm_session_id, VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "east cfm session", "--", VTY_NEWLINE);
    }

    if(psess->info.west_cfm_session_id)
    {
        vty_out(vty, "%4s%-18s: %d%s", "", "west cfm session", psess->info.west_cfm_session_id, VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "%4s%-18s: %s%s", "", "west cfm session", "--", VTY_NEWLINE);
    }

	if(psess->info.west_cfm_session_id != 0 || psess->info.east_cfm_session_id != 0)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "detection mode", "port + cfm", VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "detection mode", "port", VTY_NEWLINE);
	}

	if(psess->info.sub_ring)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "sub-ring", "YES", VTY_NEWLINE);
	}
	
	if(psess->info.attach_sess_id)
	{
		vty_out(vty, "%4s%-18s: %d%s", "", "attach to session", psess->info.attach_sess_id, VTY_NEWLINE); 
	}
    vty_out(vty, "%s", VTY_NEWLINE);
}

