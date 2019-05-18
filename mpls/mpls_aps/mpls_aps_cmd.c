#include <lib/command.h>
#include <lib/errcode.h>
#include <lib/memtypes.h>
#include <lib/memory.h>
#include <lib/types.h>
#include <lib/ifm_common.h>
#include <lib/mpls_common.h>
#include <lib/vty.h>
#include <lib/log.h>

#include "mpls_aps.h"
#include "mpls_aps_fsm.h"
#include "mpls_aps_cmd.h"
#include "mpls.h"
#include "mpls_if.h"
#include "pw.h"
#include "tunnel.h"

struct mplsaps_global  gmplsaps;              /* aps 全局数据结构 */
struct hash_table  mplsaps_session_table; /* aps 会话 hash 表，用 sess_id 作为 key */

static struct cmd_node mplsaps_session_node =
{
	APS_NODE,
	"%s(config-mplsaps-session)# ",
	1
}; 

static void mplsaps_print_error (struct vty *vty, int error)
{
	switch(error)
	{
		case MPLSAPS_LOWER_PRIO_CMD:
		        		vty_error_out(vty, "Lower priority command not accepted.%s", VTY_NEWLINE);
		        		break;
		case MPLSAPS_IS_ALREADY_FS:
				vty_error_out(vty, "current state is already Forced Switch.%s", VTY_NEWLINE);
				break;
		case MPLSAPS_IS_NOT_FS:
				vty_error_out(vty, "current state is not Forced Switch.%s", VTY_NEWLINE);
				break; 	
		case MPLSAPS_IS_ALREADY_MS:
				vty_error_out(vty, "current state is already Manual Switch.%s", VTY_NEWLINE);
				break;
		case MPLSAPS_IS_NOT_MS:
				vty_error_out(vty, "current state is not Manual Switch.%s", VTY_NEWLINE);
				break; 
		case MPLSAPS_IS_ALREADY_LOP:
				vty_error_out(vty, "current state is already Lockout of protection.%s", VTY_NEWLINE);
				break;
		case MPLSAPS_IS_NOT_LOP:
				vty_error_out(vty, "current state is not Lockout of protection.%s", VTY_NEWLINE);
				break; 
		default:
                break;	
	}
}



DEFUN(mplsaps_session,
	mplsaps_session_cmd,
	"mpls-aps session <1-255>",
	"Mpls-aps management\n"
	"Session\n"
	"Range of sess_id:<1-255>\n")
{
	struct aps_sess *psess = NULL;
	uint16_t sess_index = 0;
    int ret = 0;

	sess_index = atoi(argv[0]);

	psess = mplsaps_sess_lookup(sess_index);
	if(NULL == psess)
	{
		psess = mplsaps_sess_create(sess_index);
		if(NULL == psess)
		{
			vty_error_out(vty, "Fail to malloc for session!%s",VTY_NEWLINE);
			return CMD_WARNING;
		}
		
        ret = mplsaps_sess_add(psess);
    	if(ret != ERRNO_SUCCESS)
    	{
		    vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);
    		XFREE(MTYPE_MPLSAPS_SESS, psess);
    		return CMD_WARNING;
    	}
        gmplsaps.sess_total++;
	}
	
	vty->index = psess;
	vty->node = APS_NODE;

	return CMD_SUCCESS;
}

DEFUN(no_mplsaps_session,
	no_mplsaps_session_cmd,
	"no mpls-aps session <1-255>",
	"Delete\n"
	"Mpls-aps management\n"
	"Session\n"
	"Range of sess_id:<1-255>\n")
{
	struct aps_sess *psess = NULL;
	uint16_t sess_index = 0;
	int ret = 0;
	
	sess_index = atoi(argv[0]);

	psess = mplsaps_sess_lookup(sess_index);
	if(NULL == psess)
	{
		vty_error_out(vty, "Session:%d is not exist!%s", sess_index, VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	MPLSAPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);
	if(psess->info.type == APS_TYPE_LSP )
	{
		if(psess->tunnel_pif == NULL)
		{
			vty_error_out(vty, "Tunnel is not exsit.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		psess->tunnel_pif->tunnel.aps_id = 0;
	    if(psess->tunnel_pif->p_mplstp != NULL && psess->tunnel_pif->p_mplstp->ingress_lsp != NULL)
	    {
	 	   psess->tunnel_pif->p_mplstp->ingress_lsp->aps_id = 0;
	    }
	    if(psess->tunnel_pif->p_mplstp != NULL && psess->tunnel_pif->p_mplstp->backup_ingress_lsp != NULL)
	    {
	 	   psess->tunnel_pif->p_mplstp->backup_ingress_lsp->aps_id = 0;
	    }
	    if(psess->tunnel_pif->p_mplstp != NULL && psess->tunnel_pif->p_mplstp->egress_lsp!= NULL)
	    {
	 	   psess->tunnel_pif->p_mplstp->egress_lsp->aps_id = 0;
	    }
	    if(psess->tunnel_pif->p_mplstp != NULL && psess->tunnel_pif->p_mplstp->backup_egress_lsp!= NULL)
	    {
	 	   psess->tunnel_pif->p_mplstp->backup_egress_lsp->aps_id = 0;
	    }
	}
	else if(psess->info.type == APS_TYPE_PW )
	{
		if(psess->pif == NULL)
		{
			vty_error_out(vty, "Pw is not exsit.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}

		psess->pif->aps_sessid = 0;
		if(psess->pif->ppw_master != NULL)
		{
			psess->pif->ppw_master->aps_id = 0;
		}
		if(psess->pif->ppw_slave != NULL)
		{
			psess->pif->ppw_slave->aps_id = 0;
		}
	}
	
	ret = mplsaps_sess_delete(sess_index);
	if(ret == ERRNO_NOT_FOUND)
	{
		vty_error_out(vty, "Not found session.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
    gmplsaps.sess_total--;
    
	return CMD_SUCCESS;
}

DEFUN(mplsaps_keep_alive,
	mplsaps_keep_alive_cmd,
	"mpls-aps keep-alive <1-600>",
	"Mpls-aps management\n"
	"Keepalive\n"
	"Range of keepalive:<1s-600s>\n")
{
	struct aps_sess *psess = (struct aps_sess *)vty->index;
	uint16_t keep_alive = 0;
	
    	MPLSAPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

	keep_alive = atoi(argv[0]);
	if(keep_alive < 1|| keep_alive > 600)
	{
		vty_error_out(vty, "Invalid keep alive value.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
    
	psess->info.keepalive = keep_alive;
	return CMD_SUCCESS;
}

DEFUN(no_mplsaps_keep_alive,
	no_mplsaps_keep_alive_cmd,
	"no mpls-aps keep-alive",
	"Delete\n"
	"Mpls-aps management\n"
	"Keepalive\n")
{
	struct aps_sess *psess = (struct aps_sess *)vty->index;	
	
    	MPLSAPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);
	psess->info.keepalive = 5;
	return CMD_SUCCESS;
}

DEFUN(mplsaps_hold_off,
	mplsaps_hold_off_cmd,
	"mpls-aps hold-off <0-600>",
	"Mpls-aps management\n"
	"Hold off\n"
	"Range of hold off:<0s-600s>\n")
{
	struct aps_sess *psess = (struct aps_sess *)vty->index;
	uint16_t hold_off = 0;
	
    	MPLSAPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

	hold_off = atoi(argv[0]);	
	if(hold_off > 600)
	{
		vty_error_out(vty, "Invalid hold off value.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	psess->info.holdoff = hold_off;

	return CMD_SUCCESS;
}

DEFUN(no_mplsaps_hold_off,
	no_mplsaps_hold_off_cmd,
	"no mpls-aps hold-off",
	"Delete\n"
	"Mpls-aps management\n"
	"Hold off\n")
{
	struct aps_sess *psess = (struct aps_sess *)vty->index;
	
   	MPLSAPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);
	psess->info.holdoff = 0;
	
	return CMD_SUCCESS;
}



DEFUN(mplsaps_backup_create,
	mplsaps_backup_create_cmd,
	"mpls-aps backup non-failback ",
	"Mpls-aps management\n"
	"Protection restoration mode\n"
	"Non-revertive mode\n")
{
	struct aps_sess *psess = (struct aps_sess *)vty->index;
	
    MPLSAPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);
		
  	psess->info.failback = FAILBACK_DISABLE;
	psess->info.wtr = 0; 
	return CMD_SUCCESS;
}

DEFUN(mplsaps_backup_failback_create,
	mplsaps_backup_failback_cmd,
	"mpls-aps backup failback [ wtr <1-12> ]",
	"Mpls-aps management\n"
	"Protection restoration mode\n"
	"Revertive mode\n"
	"Wait to restore\n"
	"Range of wtr:<1min-12min>\n")
{
	struct aps_sess *psess = (struct aps_sess *)vty->index;
	uint16_t wtr = 0;
	
	MPLSAPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);
		
  	psess->info.failback = FAILBACK_ENABLE;
	if(argv[0] != NULL)
	{
		wtr = atoi(argv[0])*60;		
		psess->info.wtr = wtr;
		
	}
	else
	{
		psess->info.wtr = 300;
	}
	
	return CMD_SUCCESS;
}

DEFUN(no_mplsaps_backup_create,
	no_mplsaps_backup_create_cmd,
	"no mpls-aps backup",
	"Delete\n"
	"Mpls-aps management\n"
	"Protection restoration mode\n")
{
	struct aps_sess *psess = (struct aps_sess *)vty->index;
	
    	MPLSAPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);
	psess->info.failback = FAILBACK_ENABLE;
	psess->info.wtr = 300;
			
	return CMD_SUCCESS;
}

DEFUN(mplsaps_priority,
	mplsaps_priority_cmd,
	"mpls-aps priority <0-7>",
	"Mpls-aps management\n"
	"Mpls-aps packet priority\n"
	"Range of priority:<0-7>\n")
{
	struct aps_sess *psess = (struct aps_sess *)vty->index;
	MPLSAPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);
	psess->info.priority = atoi(argv[0]);

	return CMD_SUCCESS;
}

 DEFUN(no_mplsaps_priority,
	 no_mplsaps_priority_cmd,
	 "no mpls-aps priority",
	 "Delete\n"
	 "Mpls-aps management\n"
	 "Mpls-aps packet priority\n")
 {	 
	 struct aps_sess *psess = (struct aps_sess *)vty->index;
	 
	 MPLSAPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);	 
	 psess->info.priority = 6;
 
	 return CMD_SUCCESS;
 }

DEFUN(mplsaps_bind_pw,
	mplsaps_bind_pw_cmd,
	"mpls-aps bind pw NAME",
	"Mpls-aps management\n"
	"Bind\n"
	"Pw\n"
	"The name for pw -- max.31\n"
	)
{
	struct aps_sess *psess = (struct aps_sess *)vty->index;
	struct l2vc_entry * l2vc_entry = NULL;
	struct mpls_if *pif;
	
	MPLSAPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);
	
	l2vc_entry = l2vc_lookup((uchar *)argv[0]);
	if(l2vc_entry == NULL)
	{
		vty_error_out(vty, "Pw  is not exsit.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(l2vc_entry->pwinfo.ifindex == 0)
	{
		vty_error_out(vty, "Pw is not bind port.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(l2vc_entry->aps_id)
	{
		vty_error_out(vty, "Pw is already bind mpls aps session %d .%s",l2vc_entry->aps_id,VTY_NEWLINE);
		return CMD_WARNING;
	}

	pif = mpls_if_lookup(l2vc_entry->pwinfo.ifindex);
	if(pif == NULL)
	{
		vty_error_out(vty, "mpls_if is not exsit.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(pif->ppw_slave== NULL)
	{
		vty_error_out(vty, "Backup pw is not exsit.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(pif->ppw_master== NULL)
	{
		vty_error_out(vty, "Master pw is not exsit.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
    if(psess->info.master_index != 0 || psess->info.backup_index != 0)
    {
		vty_out(vty, "The sess has bind pw or tunnel.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
		
	l2vc_entry->aps_id = psess->info.sess_id;
	psess->info.bind_index = l2vc_entry->pwinfo.pwindex;

	psess->pif = pif;
	psess->info.master_index = pif->ppw_master->pwinfo.pwindex;
	psess->info.backup_index = pif->ppw_slave->pwinfo.pwindex;
	psess->info.type = APS_TYPE_PW;

	pif->aps_sessid = psess->info.sess_id;
	pif->ppw_master->aps_id = psess->info.sess_id;
	pif->ppw_slave->aps_id = psess->info.sess_id;
	
	return CMD_SUCCESS;
}

 DEFUN(no_mplsaps_bind,
	 no_mplsaps_bind_cmd,
	 "no mpls-aps bind pw",
	 "Delete\n"
	 "Mpls-aps management\n"
	 "Bind\n"
	 "Pw\n")
 {	 
	 struct aps_sess *psess = (struct aps_sess *)vty->index;
	 
	 MPLSAPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

	 if(psess->pif == NULL)
	 {
		vty_error_out(vty, "Pw is not exsit.%s", VTY_NEWLINE);
		return CMD_WARNING;
	 }

	psess->pif->aps_sessid = 0;
	if(psess->pif->ppw_master != NULL)
	{
		psess->pif->ppw_master->aps_id = 0;
	}
	if(psess->pif->ppw_slave != NULL)
	{
		psess->pif->ppw_slave->aps_id = 0;
	}
	psess->pif = NULL;
	psess->info.master_index = 0;
	psess->info.backup_index =0;
	psess->info.type = APS_TYPE_MAX;
		 
	 return CMD_SUCCESS;
 }


DEFUN(mplsaps_bind_tunnel,
	mplsaps_bind_tunnel_cmd,
	"mpls-aps bind tunnel USP",
	"Mpls-aps management\n"
	"Bind\n"
	CLI_INTERFACE_TUNNEL_STR
	CLI_INTERFACE_TUNNEL_VHELP_STR
	)
{
	struct aps_sess *psess = (struct aps_sess *)vty->index;
	uint32_t ifindex = 0;
	struct tunnel_if *pif = NULL;
	
	MPLSAPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);
	

    ifindex = ifm_get_ifindex_by_name ( "tunnel", ( char * ) argv[0] );	
	pif = tunnel_if_lookup(ifindex);
	if(pif == NULL)
	{
        vty_error_out ( vty, "Tunnel %s is not exit or no protocol!%s", ( char * ) argv[0],  VTY_NEWLINE );
		return CMD_WARNING;
	}
	
	if(TUNNEL_PRO_MPLSTP != pif->tunnel.protocol)
	{
		vty_error_out(vty, "The specified tunnel protocol is not support.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	
	if(pif->tunnel.aps_id)
	{
		vty_error_out(vty, "Tunnel is already bind mpls aps session %d .%s",pif->tunnel.aps_id,VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(NULL == pif->p_mplstp || NULL == pif->p_mplstp->ingress_lsp)
	{
		vty_error_out(vty, "Master lsp is not exsit.%s", VTY_NEWLINE);
		return CMD_WARNING;		
	}
	
	if(NULL == pif->p_mplstp->backup_ingress_lsp)
	{
		vty_error_out(vty, "Backup lsp is not exsit.%s", VTY_NEWLINE);
		return CMD_WARNING;		
	}

    	if(psess->info.master_index != 0 || psess->info.backup_index != 0)
    	{
		vty_error_out(vty, "The sess has bind pw or tunnel.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
		
	pif->tunnel.aps_id = psess->info.sess_id;
	psess->info.bind_index = ifindex;

	psess->tunnel_pif = pif;
	psess->info.master_index = pif->tunnel.master_index;
	psess->info.backup_index = pif->tunnel.backup_index;
	psess->info.type = APS_TYPE_LSP;

	pif->p_mplstp->ingress_lsp->aps_id = psess->info.sess_id;
	pif->p_mplstp->egress_lsp->aps_id = psess->info.sess_id;
	pif->p_mplstp->backup_ingress_lsp->aps_id = psess->info.sess_id;
	pif->p_mplstp->backup_egress_lsp->aps_id = psess->info.sess_id;
	
	return CMD_SUCCESS;
}


DEFUN(no_mplsaps_bind_tunnel,
	no_mplsaps_bind_tunnel_cmd,
	"no mpls-aps bind tunnel",
	"Delete\n"
	"Mpls-aps management\n"
	"Bind\n"
	CLI_INTERFACE_TUNNEL_STR)
{	
	struct aps_sess *psess = (struct aps_sess *)vty->index;
	
	MPLSAPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);

	if(psess->tunnel_pif == NULL)
	{
	   vty_error_out(vty, "Tunnel is not exsit.%s", VTY_NEWLINE);
	   return CMD_WARNING;
    } 
 
    psess->tunnel_pif->tunnel.aps_id = 0;
    if(psess->tunnel_pif->p_mplstp != NULL && psess->tunnel_pif->p_mplstp->ingress_lsp != NULL)
    {
	    psess->tunnel_pif->p_mplstp->ingress_lsp->aps_id = 0;
    }
    if(psess->tunnel_pif->p_mplstp != NULL && psess->tunnel_pif->p_mplstp->backup_ingress_lsp != NULL)
    {
	    psess->tunnel_pif->p_mplstp->backup_ingress_lsp->aps_id = 0;
    }
    if(psess->tunnel_pif->p_mplstp != NULL && psess->tunnel_pif->p_mplstp->egress_lsp!= NULL)
    {
	    psess->tunnel_pif->p_mplstp->egress_lsp->aps_id = 0;
    }
    if(psess->tunnel_pif->p_mplstp != NULL && psess->tunnel_pif->p_mplstp->backup_egress_lsp!= NULL)
    {
	    psess->tunnel_pif->p_mplstp->backup_egress_lsp->aps_id = 0;
    }
    psess->tunnel_pif = NULL;
    psess->info.master_index = 0;
    psess->info.backup_index =0;
    psess->info.type = APS_TYPE_MAX;
		
	return CMD_SUCCESS;
}

DEFUN(mplsaps_session_enable,
	mplsaps_session_enable_cmd,
	"mpls-aps (enable | disable)",
	"Mpls-aps management\n"
	"Enable\n"
	"Disable\n")
{
	struct aps_sess *psess = (struct aps_sess *)vty->index;
	int ret = 0;
	
	if(argv[0][0] == 'e')
	{
		MPLSAPS_SESSION_ALREADY_ENABLE(vty, psess->info.status);
		if(psess->info.bind_index== 0)
		{
			vty_error_out(vty, "Path is not configure.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		
		ret = mplsaps_sess_enable (psess);
		if (ERRNO_SUCCESS != ret)
		{
			vty_warning_out(vty, "mplsaps is sent failed.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		mplsaps_init_session(psess);
		gmplsaps.sess_enable++;
		
	}
	else if(argv[0][0] == 'd')
	{
		MPLSAPS_SESSION_IS_DISABLE(vty, psess->info.status); 	
		ret = mplsaps_sess_disable (psess);
		if (ERRNO_SUCCESS != ret)
		{
			vty_warning_out(vty, "mplsaps is sent failed.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	
	return CMD_SUCCESS;	
}


DEFUN(mplsaps_debug_packet,
	mplsaps_debug_packet_cmd,
	"debug mpls-aps packet",
	"DEBUG_STR"
	"Mpls-aps management\n"
	"Mpls-aps packet\n" )
{	
	if(!gmplsaps.sess_enable)
	{
	        vty_warning_out ( vty, "Please enable mplsaps function at first%s", VTY_NEWLINE );
	        return CMD_WARNING;
	}

	gmplsaps.debug_packet = 1;
	return CMD_SUCCESS;	
}

DEFUN(no_mplsaps_debug_packet,
	no_mplsaps_debug_packet_cmd,
	"no debug mpls-aps packet",
	"Delete\n"
	"DEBUG_STR"
	"Mpls-aps management\n"
	"Mpls-aps packet\n" )
{	
	if(!gmplsaps.sess_enable)
	{
	        vty_warning_out ( vty, "Please enable mplsaps function at first%s", VTY_NEWLINE );
	        return CMD_WARNING;
	}
	gmplsaps.debug_packet = 0;
	return CMD_SUCCESS;	
	
}


/************************admin commands********************/

//Force Switch

DEFUN(mplsaps_admin_force,
	mplsaps_admin_force_cmd,
	"mpls-aps force-switch",
	"Mpls-aps management\n"
	"Force switching\n" )
{	
	struct aps_sess *psess = (struct aps_sess *)vty->index; 
	uint32_t ret;
	
	MPLSAPS_SESSION_IS_DISABLE(vty, psess->info.status); 	
	
	ret = mplsaps_sess_forceswitch(psess, 1);
	switch(ret)
	{
		case ERRNO_SUCCESS:
			return CMD_SUCCESS;
		default:
			mplsaps_print_error(vty,ret);
			return CMD_WARNING;
	}
			 
}

DEFUN(no_mplsaps_admin_force,
	no_mplsaps_admin_force_cmd,
	"no mpls-aps force-switch",
	"Delete\n"
	"Mpls-aps management\n"
	"Force switching\n" )
{	
	struct aps_sess *psess = (struct aps_sess *)vty->index; 
	uint32_t ret;
	
	MPLSAPS_SESSION_IS_DISABLE(vty, psess->info.status); 	
	
	ret = mplsaps_sess_forceswitch(psess, 0);
	switch(ret)
	{
		case ERRNO_SUCCESS:
			return CMD_SUCCESS;
		default:
			mplsaps_print_error(vty,ret);
			return CMD_WARNING;
	} 
}


//	Manual Switch

DEFUN(mplsaps_admin_manual,
	mplsaps_admin_manual_cmd,
	"mpls-aps manual-switch",
	"Mpls-aps management\n"
	"Manual switching\n" )
{	
	struct aps_sess *psess = (struct aps_sess *)vty->index; 
	uint32_t ret;
	
	MPLSAPS_SESSION_IS_DISABLE(vty, psess->info.status); 	 
	
	ret = mplsaps_sess_manualswitch(psess,1);
	switch(ret)
	{
		case ERRNO_SUCCESS:
			return CMD_SUCCESS;
		default:
			mplsaps_print_error(vty,ret);
			return CMD_WARNING;
	}	
}

DEFUN(no_mplsaps_admin_manual,
	no_mplsaps_admin_manual_cmd,
	"no mpls-aps manual-switch",
	"Delete\n"
	"Mpls-aps management\n"
	"Manual switching\n" )
{	
	struct aps_sess *psess = (struct aps_sess *)vty->index; 
	uint32_t ret;
	
	MPLSAPS_SESSION_IS_DISABLE(vty, psess->info.status); 	 
	
	ret = mplsaps_sess_manualswitch(psess,0);
	switch(ret)
	{
		case ERRNO_SUCCESS:
			return CMD_SUCCESS;
		default:
			mplsaps_print_error(vty,ret);
			return CMD_WARNING;;
	}	
}

//Lockout Of Protection

DEFUN(mplsaps_admin_lockout,
	mplsaps_admin_lockout_cmd,
	"mpls-aps lockout",
	"Mpls-aps management\n"
	"Lockout of protection\n" )
{	
	struct aps_sess *psess = (struct aps_sess *)vty->index; 
	uint32_t ret;
	
	MPLSAPS_SESSION_IS_DISABLE(vty, psess->info.status); 	 
	
	ret = mplsaps_sess_lock(psess,1);
	switch(ret)
	{
		case ERRNO_SUCCESS:
			return CMD_SUCCESS;
		default:
			mplsaps_print_error(vty,ret);
			return CMD_WARNING;
	}	
}

DEFUN(no_mplsaps_admin_lockout,
	no_mplsaps_admin_lockout_cmd,
	"no mpls-aps lockout",
	"Delete\n"
	"Mpls-aps management\n"
	"Lockout of protection\n" )
{	
	struct aps_sess *psess = (struct aps_sess *)vty->index; 
	uint32_t ret;
	
	MPLSAPS_SESSION_IS_DISABLE(vty, psess->info.status); 	 
	
	ret = mplsaps_sess_lock(psess,0);
	switch(ret)
	{
		case ERRNO_SUCCESS:
			return CMD_SUCCESS;
		default:
			mplsaps_print_error(vty,ret);
			return CMD_WARNING;
	}	
}



/************************show  commands********************/

DEFUN(show_mplsaps_config,
	show_mplsaps_config_cmd,
	"show mpls-aps config",
	"Display.\n"
	"Mpls-aps management\n"
	"Config\n")
{
	struct hash_bucket *bucket = NULL;
	struct aps_sess *psess = NULL;
	int cursor = 0;
    	int num = 0;
		
	HASH_BUCKET_LOOP(bucket, cursor, mplsaps_session_table)
	{
		psess = (struct aps_sess *)bucket->data;
        		vty_out(vty, "%4s:%d%s", "Num", ++num, VTY_NEWLINE);
		mplsaps_show_config(vty, psess);
		psess = NULL;
	}
    	return CMD_SUCCESS;
}


DEFUN(show_mplsaps_session,
	show_mplsaps_session_cmd,
	"show mpls-aps session [<1-255>]",
	"Display.\n"
	"Mpls-aps management\n"
	"Session\n"
	"Range of local_mep:<1-255>\n")
{
	struct hash_bucket *bucket = NULL;
	struct aps_sess *psess = NULL;
	uint16_t sess_id = 0;
	int cursor = 0;
    	int num = 0;

	if(NULL == argv[0])//显示所有session信息
	{
		vty_out(vty, "%4s%-18s: %d%s", "", "Total", gmplsaps.sess_total, VTY_NEWLINE);
		vty_out(vty, "%4s%-18s: %d%s", "", "enable_num", gmplsaps.sess_enable, VTY_NEWLINE);
		
		HASH_BUCKET_LOOP(bucket, cursor, mplsaps_session_table)
		{
			psess = (struct aps_sess *)bucket->data;
            		vty_out(vty, "%4s:%d%s", "Num", ++num, VTY_NEWLINE);
			mplsaps_show_session(vty, psess);
			psess = NULL;
		}
	}
	else
	{
		sess_id = atoi(argv[0]);
		psess = mplsaps_sess_lookup(sess_id);
		if(psess == NULL)
		{
			vty_out(vty, "Error:Session:%d is not exist!%s", sess_id, VTY_NEWLINE);
			return CMD_WARNING;
		}
		mplsaps_show_session(vty, psess);
	}
    
    return CMD_SUCCESS;
}

DEFUN(show_mplsaps_debug,
	show_mplsaps_debug_cmd,
	"show mpls-aps debug",
	"Display.\n"
	"Mpls-aps management\n"
	"Debug\n")
{
	vty_out(vty, "aps recv total %d%s", gmplsaps.pkt_recv, VTY_NEWLINE);
	vty_out(vty, "aps send total %d%s", gmplsaps.pkt_send, VTY_NEWLINE);
	vty_out(vty, "aps error total %d%s", gmplsaps.pkt_err, VTY_NEWLINE);
	if(gmplsaps.debug_packet == 0)
	{
		vty_out(vty, "aps debug packet is %s%s", "disable", VTY_NEWLINE);	
	}
	else
	{
		vty_out(vty, "aps debug packet is %s%s", "enable", VTY_NEWLINE);	
	}
	
	return CMD_SUCCESS;
}

int mplsaps_session_config_write(struct vty *vty)
{
	struct hash_bucket *bucket = NULL;
	struct aps_sess *psess = NULL;
	uchar *name = NULL;
   	char ifname[IFNET_NAMESIZE];
	int cursor = 0;
	int wtr;
	
 	vty_out(vty, "mpls-aps%s", VTY_NEWLINE);
	vty_out(vty, " mpls%s", VTY_NEWLINE);   
	HASH_BUCKET_LOOP(bucket, cursor, mplsaps_session_table)
	{   
		psess = (struct aps_sess *)bucket->data;
		vty_out(vty, "  mpls-aps session %d%s", psess->info.sess_id, VTY_NEWLINE);
		if(psess->info.keepalive != 5)
		{
			vty_out(vty, "    mpls-aps keep-alive %d%s", psess->info.keepalive, VTY_NEWLINE);
		}
		if(psess->info.holdoff != 0)
		{
			vty_out(vty, "    mpls-aps hold-off %d%s", psess->info.holdoff, VTY_NEWLINE);
		}
		
      		if(psess->info.failback == FAILBACK_ENABLE)
		{
			if(psess->info.wtr != 300)
			{
				wtr = psess->info.wtr/60;
				vty_out(vty, "    mpls-aps backup %s wtr %d%s", "failback", wtr,VTY_NEWLINE);
			}
		}
		else
		{
			vty_out(vty, "    mpls-aps backup %s%s", "non-failback", VTY_NEWLINE);
		}
		if(psess->info.priority != 6)
		{
			vty_out(vty, "    mpls-aps priority %d%s", psess->info.priority, VTY_NEWLINE);
		}
		
		if(psess->info.type == APS_TYPE_PW)
		{
			name = pw_get_name(psess->info.bind_index);
			if(name)
			{
				vty_out(vty, "    mpls-aps bind pw %s%s", name, VTY_NEWLINE);
			}
		}
		else if(psess->info.type == APS_TYPE_LSP)
		{
			ifm_get_name_by_ifindex(psess->info.bind_index, ifname);	
			if(ifname[0] != '\0')
			{
				vty_out(vty, "    mpls-aps bind %s%s", ifname, VTY_NEWLINE);
			}
		}

		if(psess->info.status == MPLS_APS_STATUS_ENABLE)
		{
			vty_out(vty, "    mpls-aps %s%s", "enable", VTY_NEWLINE);
		}
	}
    return ERRNO_SUCCESS;
}


void mplsaps_cmd_init(void)
{
	install_node (&mplsaps_session_node, mplsaps_session_config_write); 
	install_default (APS_NODE); 

	install_element (MPLS_NODE, &mplsaps_session_cmd,CMD_SYNC);
	install_element (MPLS_NODE, &no_mplsaps_session_cmd,CMD_SYNC); 
	install_element (CONFIG_NODE, &mplsaps_debug_packet_cmd,CMD_LOCAL);
	install_element (CONFIG_NODE, &no_mplsaps_debug_packet_cmd,CMD_LOCAL); 
	
	//show commands
	install_element (CONFIG_NODE, &show_mplsaps_config_cmd,CMD_LOCAL);
	install_element (CONFIG_NODE, &show_mplsaps_session_cmd,CMD_LOCAL); 
	install_element (CONFIG_NODE, &show_mplsaps_debug_cmd,CMD_LOCAL);
	install_element (MPLS_NODE, &show_mplsaps_config_cmd,CMD_LOCAL);
	install_element (MPLS_NODE, &show_mplsaps_session_cmd,CMD_LOCAL); 

	install_element (APS_NODE, &mplsaps_keep_alive_cmd,CMD_SYNC);
	install_element (APS_NODE, &no_mplsaps_keep_alive_cmd,CMD_SYNC); 
	install_element (APS_NODE, &mplsaps_hold_off_cmd,CMD_SYNC);
	install_element (APS_NODE, &no_mplsaps_hold_off_cmd,CMD_SYNC); 
	install_element (APS_NODE, &mplsaps_backup_create_cmd,CMD_SYNC);
	install_element (APS_NODE, &mplsaps_backup_failback_cmd,CMD_SYNC);
	install_element (APS_NODE, &no_mplsaps_backup_create_cmd,CMD_SYNC); 
	install_element (APS_NODE, &mplsaps_priority_cmd,CMD_SYNC);
	install_element (APS_NODE, &no_mplsaps_priority_cmd,CMD_SYNC); 
	install_element (APS_NODE, &mplsaps_bind_pw_cmd,CMD_SYNC);
	install_element (APS_NODE, &mplsaps_bind_tunnel_cmd,CMD_SYNC);
	install_element (APS_NODE, &no_mplsaps_bind_cmd,CMD_SYNC); 
	install_element (APS_NODE, &no_mplsaps_bind_tunnel_cmd,CMD_SYNC); 
	install_element (APS_NODE, &mplsaps_session_enable_cmd,CMD_SYNC); 

	//admin commands
	install_element (APS_NODE, &mplsaps_admin_force_cmd,CMD_LOCAL); 
	install_element (APS_NODE, &no_mplsaps_admin_force_cmd,CMD_LOCAL); 
	install_element (APS_NODE, &mplsaps_admin_manual_cmd,CMD_LOCAL); 
	install_element (APS_NODE, &no_mplsaps_admin_manual_cmd,CMD_LOCAL); 
	install_element (APS_NODE, &mplsaps_admin_lockout_cmd,CMD_LOCAL); 
	install_element (APS_NODE, &no_mplsaps_admin_lockout_cmd,CMD_LOCAL); 

}

void mplsaps_show_config(struct vty *vty, struct aps_sess *psess)
{
	int wtr;
	if(psess == NULL)
	{
		return ;
	}

	vty_out(vty, "%4s%-18s: %d%s", "", "session-id", psess->info.sess_id, VTY_NEWLINE);
	
	if(psess->info.type == APS_TYPE_PW)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "session-type", "PW", VTY_NEWLINE);
		if((psess->pif)&&(psess->pif->ppw_master) &&( psess->pif->ppw_master->name[0] != '\0'))
		{
			vty_out(vty, "%4s%-18s: %s%s", "", "master-path", psess->pif->ppw_master->name, VTY_NEWLINE);
		}
		else
		{
			vty_out(vty, "%4s%-18s: %s%s", "", "master-path", "--", VTY_NEWLINE);
		}
		
		if((psess->pif)&&(psess->pif->ppw_slave) &&( psess->pif->ppw_slave->name[0] != '\0'))
		{
			vty_out(vty, "%4s%-18s: %s%s", "", "backup-path", psess->pif->ppw_slave->name, VTY_NEWLINE);
		}
		else
		{
			vty_out(vty, "%4s%-18s: %s%s", "", "backup-path", "--", VTY_NEWLINE);
		}
	}
	else if(psess->info.type == APS_TYPE_LSP)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "session-type", "LSP", VTY_NEWLINE);
		
		if(psess->tunnel_pif && psess->tunnel_pif->inlsp_name[0]!= '\0')
		{
			vty_out(vty, "%4s%-18s: %s%s", "", "master-path", psess->tunnel_pif->inlsp_name, VTY_NEWLINE);
		}
		else
		{
			vty_out(vty, "%4s%-18s: %s%s", "", "master-path", "--", VTY_NEWLINE);
		}
		
		if(psess->tunnel_pif && psess->tunnel_pif->backup_inlsp_name[0]!= '\0')
		{
			vty_out(vty, "%4s%-18s: %s%s", "", "backup-path", psess->tunnel_pif->backup_inlsp_name, VTY_NEWLINE);
		}
		else
		{
			vty_out(vty, "%4s%-18s: %s%s", "", "backup-path", "--", VTY_NEWLINE);
		}
	}
	else
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "session-type", "--", VTY_NEWLINE);
		vty_out(vty, "%4s%-18s: %s%s", "", "master-path", "--", VTY_NEWLINE);
		vty_out(vty, "%4s%-18s: %s%s", "", "backup-path", "--", VTY_NEWLINE);
	}
	
	vty_out(vty, "%4s%-18s: %d s%s", "", "keep-alive", psess->info.keepalive, VTY_NEWLINE);
	vty_out(vty, "%4s%-18s: %d s%s", "", "hold-off", psess->info.holdoff, VTY_NEWLINE);

	if(psess->info.failback == FAILBACK_ENABLE)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "backup-mode", "Failback", VTY_NEWLINE);
		if((psess->info.failback) == FAILBACK_ENABLE)
		{
			wtr = psess->info.wtr/60;
			vty_out(vty, "%4s%-18s: %d min%s", "", "wtr", wtr, VTY_NEWLINE);
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
	
	if(psess->info.status == MPLS_APS_STATUS_DISABLE)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "session-status", "disable", VTY_NEWLINE);
	}
	else if(psess->info.status == MPLS_APS_STATUS_ENABLE)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "session-status", "enable", VTY_NEWLINE);
	}
    	vty_out(vty, "%s", VTY_NEWLINE);
	return ;
}


void mplsaps_show_session(struct vty *vty, struct aps_sess *psess)
{
	int wtr;
	if(psess == NULL)
	{
		return ;
	}
	
	vty_out(vty, "%-14s%-18s%-18s%-20s%s", "session-id", "master-path", "backup-path", "current-state", VTY_NEWLINE);
	vty_out(vty, "------------------------------------------------------------------------\r\n");
   	vty_out(vty, "%-14d", psess->info.sess_id);

	if(psess->info.type == APS_TYPE_PW)
	{
		if((psess->pif)&&(psess->pif->ppw_master)&& (psess->pif->ppw_master->name[0]!= '\0'))
		{
			vty_out(vty, "%-18s", psess->pif->ppw_master->name);
		}
		else
		{
			vty_out(vty, "%-18s", "--");
		}
		if((psess->pif)&&(psess->pif->ppw_slave)&& (psess->pif->ppw_slave->name[0]!= '\0'))
		{
			vty_out(vty, "%-18s", psess->pif->ppw_slave->name);
		}
		else
		{
			vty_out(vty, "%-18s", "--");
		}
	}
	else if(psess->info.type == APS_TYPE_LSP)
	{
		if(psess->tunnel_pif && psess->tunnel_pif->inlsp_name[0]!= '\0')
		{
			vty_out(vty, "%-18s",psess->tunnel_pif->inlsp_name);
		}
		else
		{
			vty_out(vty, "%-18s", "--");
		}
		
		if(psess->tunnel_pif && psess->tunnel_pif->backup_inlsp_name[0]!= '\0')
		{
			vty_out(vty, "%-18s",psess->tunnel_pif->backup_inlsp_name);
		}
		else
		{
			vty_out(vty, "%-18s", "--");
		}
	
	}
	else
	{
		vty_out(vty, "%-18s", "--");
		vty_out(vty, "%-18s", "--");
	}

	if(psess->info.current_status == APS_STATE_NR)
	{
		vty_out(vty, "%-15s%s", "No request", VTY_NEWLINE);
	}
	else if(psess->info.current_status == APS_STATE_DNR)
	{
		vty_out(vty, "%-15s%s", "Do not revert", VTY_NEWLINE);
	}
	else if(psess->info.current_status == APS_STATE_WTR)
	{
		vty_out(vty, "%s%s", "Wait-To-Restore", VTY_NEWLINE);
	}
	else if(psess->info.current_status == APS_STATE_MS)
	{
		vty_out(vty, "%s%s", "Manual Switch", VTY_NEWLINE);
	}
	else if(psess->info.current_status == APS_STATE_SF_W)
	{
		vty_out(vty, "%s%s", "Signal Fail for Working", VTY_NEWLINE);
	}
	else if(psess->info.current_status == APS_STATE_FS)
	{
		vty_out(vty, "%s%s", "Forced Switch", VTY_NEWLINE);
	}
	else if(psess->info.current_status == APS_STATE_SF_P)
	{
		vty_out(vty, "%s%s", "Signal Fail for Protection", VTY_NEWLINE);
	}
	else if(psess->info.current_status == APS_STATE_LO)
	{
		vty_out(vty, "%s%s", "Lockout Of Protection", VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "%s%s", "Invalid", VTY_NEWLINE);
	}
	
	vty_out(vty, "%s",VTY_NEWLINE);

	
	vty_out(vty, "%4s%-18s: %s%s", "", "active-path", (psess->info.backup == BACKUP_STATUS_MASTER )? "Master-path":"Backup-path", VTY_NEWLINE);
	if(psess->info.type == APS_TYPE_PW)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "type","PW", VTY_NEWLINE);
	}
	else if(psess->info.type == APS_TYPE_LSP)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "type", "LSP", VTY_NEWLINE);
	}
	else
	{
		
		vty_out(vty, "%4s%-18s: %s%s", "", "type", "--", VTY_NEWLINE);
	}
	vty_out(vty, "%4s%-18s: %d s%s", "", "keep-alive", psess->info.keepalive, VTY_NEWLINE);
	vty_out(vty, "%4s%-18s: %d s%s", "", "hold-off", psess->info.holdoff, VTY_NEWLINE);
	
	if(psess->info.failback == FAILBACK_ENABLE)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "backup-mode", "Failback", VTY_NEWLINE);
		wtr = psess->info.wtr/60;
		vty_out(vty, "%4s%-18s: %d min%s", "", "wtr", wtr, VTY_NEWLINE);
	}
	else if(psess->info.failback == FAILBACK_DISABLE)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "backup-mode", "Non-Failback", VTY_NEWLINE);
	}
	
    	vty_out(vty, "%4s%-18s: %d%s", "", "priority", psess->info.priority, VTY_NEWLINE);
	
	if(psess->info.status == MPLS_APS_STATUS_DISABLE)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "session-status", "disable", VTY_NEWLINE);
	}
	else if(psess->info.status == MPLS_APS_STATUS_ENABLE)
	{
		vty_out(vty, "%4s%-18s: %s%s", "", "session-status", "enable", VTY_NEWLINE);
	}
   	 vty_out(vty, "%s", VTY_NEWLINE);
}
