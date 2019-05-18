#include <lib/vty.h>
#include <lib/errcode.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lib/types.h>
#include <lib/command.h>
#include <lib/ifm_common.h>
#include <lib/inet_ip.h>
#include <lib/ether.h>
#include <lib/vty.h>
#include <lib/module_id.h>
#include <lib/msg_ipc_n.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/prefix.h>
#include <lib/index.h>
#include "mpls_if.h"
#include "mpls_oam/mpls_oam_cmd.h"
#include "mpls_oam/mpls_oam.h"
#include "mpls_oam/mpls_oam_pkt.h"
#include "mpls_oam/mpls_oam_ais.h"
#include "mpls_oam/mpls_oam_lck.h"
#include "mpls_oam/mpls_oam_csf.h"
#include "lsp_static.h"

uint32_t tpoam_line = 0;   //存储tp-oam session回显行数

static struct cmd_node mplstp_oam_sess_node =
{
	MPLSTP_OAM_SESS_NODE,
	"%s(mplstp-oam-session)# ",
	1
};

static struct cmd_node mplstp_oam_meg_node =
{
    MPLSTP_OAM_MEG_NODE,
    "%s(mplstp-oam-meg)# ",
    1
};

static int show_mplsoam_session_global(struct vty *vty)
{
    struct oam_session *psess = NULL;
    struct hash_bucket *bucket = NULL;
    int cursor = 0;
    int sess_up = 0;
    int sess_down = 0;
    int sess_enable = 0;
    
    HASH_BUCKET_LOOP(bucket, cursor, mplsoam_session_table)
    {
        psess = (struct oam_session *)bucket->data;
        if(psess->info.state == OAM_STATUS_UP)
        {
            sess_up++;
        }
        else if(psess->info.state == OAM_STATUS_DOWN)
        {
            sess_down++;
        }
        if(psess->info.type != OAM_TYPE_INVALID)
        {
            sess_enable++;
        }
    }
    gmplsoam.sess_up = sess_up;
    gmplsoam.sess_down = sess_down;
    gmplsoam.sess_enable = sess_enable;

    //FLUSH_LINE_CHECK( tpoam_line, vty->flush_cp)
    vty_out(vty, "--------------------------------------------------%s", VTY_NEWLINE);
    //FLUSH_LINE_CHECK( tpoam_line, vty->flush_cp)
    vty_out(vty, "%-8s%-13s%-9s%-11s%s%s", "Total", "enable_num", "up_num", "down_num", "channel-type", VTY_NEWLINE);
    //FLUSH_LINE_CHECK( tpoam_line, vty->flush_cp)
    {
        vty_out(vty, "%3d", gmplsoam.sess_total);
        vty_out(vty, "%10d", gmplsoam.sess_enable);
        if(0 == gmplsoam.sess_enable)
        {
            vty_out(vty, "%11s", "--");
            vty_out(vty, "%10s", "--");
        }
        else
        {
            vty_out(vty, "%11d", gmplsoam.sess_up);
            vty_out(vty, "%10d", gmplsoam.sess_down);
        }
		vty_out(vty, "%12s%x", "0x", gmplsoam.global_chtype);
        vty_out(vty, "%s", VTY_NEWLINE);
    }
    //FLUSH_LINE_CHECK( tpoam_line, vty->flush_cp)
    vty_out(vty, "--------------------------------------------------%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}

void show_mplsoam_session_current_alarm(struct vty *vty, struct oam_session *psess)
{
    if(NULL == psess)
    {
        return ;
    }

    vty_out(vty, "%2s%-32s: %s%s", "", "Alarm-report", psess->alarm_rpt == 1 ? "Enable":"Disable", VTY_NEWLINE);
    
    if(psess->alarm.alarm_level == 1)
    {
        vty_out(vty, "%2s%-32s: %s%s", "", "alarm-UNL", "Yes", VTY_NEWLINE);
    }
    if(psess->alarm.alarm_meg == 1)
    {
        vty_out(vty, "%2s%-32s: %s%s", "", "alarm-MMG", "Yes", VTY_NEWLINE);
    }
    if(psess->alarm.alarm_mep == 1)
    {
        vty_out(vty, "%2s%-32s: %s%s", "", "alarm-UNM", "Yes", VTY_NEWLINE);
    }
    if(psess->alarm.alarm_cc == 1)
    {
        vty_out(vty, "%2s%-32s: %s%s", "", "alarm-UNP", "Yes", VTY_NEWLINE);
    }
	
    if(psess->alarm.alarm_loc == 1)
    {
    	if(psess->alarm.alarm_ais == 1)
		{	
			vty_out(vty, "%2s%-32s: %s%s", "", "alarm-LOC", "Suppressed by AIS", VTY_NEWLINE);
		}
		else if(psess->alarm.alarm_lck == 1)
		{
			vty_out(vty, "%2s%-32s: %s%s", "", "alarm-LOC", "Suppressed by LCK", VTY_NEWLINE);
		}
		else
		{
			vty_out(vty, "%2s%-32s: %s%s", "", "alarm-LOC", "Yes", VTY_NEWLINE);
		}
    }
    if(psess->alarm.alarm_rdi == 1)
    {   
        vty_out(vty, "%2s%-32s: %s%s", "", "alarm-RDI", "Yes", VTY_NEWLINE);
    }
	if(psess->alarm.alarm_ais == 1)
    {   
        vty_out(vty, "%2s%-32s: %s%s", "", "alarm-AIS", "Yes", VTY_NEWLINE);
    }
	if(psess->alarm.alarm_lck == 1)
    {   
        vty_out(vty, "%2s%-32s: %s%s", "", "alarm-LCK", "Yes", VTY_NEWLINE);
    }
	if(psess->alarm.alarm_csf == 1)
	{
        vty_out(vty, "%2s%-32s: %s%s", "", "alarm-CSF", "Yes", VTY_NEWLINE);
	}

    return ;
}

void show_mplsoam_session_lm_result(struct vty *vty, struct oam_session *psess)
{
    char f_max[10] = {0};
    char f_min[10] = {0};
    char f_mean[10] = {0};
    char f_max_peer[10] = {0};
    char f_min_peer[10] = {0};
    char f_mean_peer[10] = {0};
    
    if(NULL == psess)
    {
        return ;
    }
    
    sprintf(f_max, "%d.%02d", psess->sla.lm_max/100, psess->sla.lm_max%100);
    sprintf(f_min, "%d.%02d", psess->sla.lm_min/100, psess->sla.lm_min%100);
    sprintf(f_mean, "%d.%02d", psess->sla.lm_mean/100, psess->sla.lm_mean%100);
    sprintf(f_max_peer, "%d.%02d", psess->sla.lm_max_peer/100, psess->sla.lm_max_peer%100);
    sprintf(f_min_peer, "%d.%02d", psess->sla.lm_min_peer/100, psess->sla.lm_min_peer%100);
    sprintf(f_mean_peer, "%d.%02d", psess->sla.lm_mean_peer/100, psess->sla.lm_mean_peer%100);

    if(psess->info.lm_enable == OAM_STATUS_ENABLE)
    {
        vty_out(vty, "%2s%-32s: %s%s", "", "LM-state", "Running", VTY_NEWLINE);
    }
    else if(psess->info.lm_enable == OAM_STATUS_FINISH)
    {
        vty_out(vty, "%2s%-32s: %s%s", "", "LM-state", "Finish", VTY_NEWLINE);
    }
    else if(psess->info.lm_enable == OAM_STATUS_DISABLE)
    {
        vty_out(vty, "%2s%-32s: %s%s", "", "LM-state", "Disable", VTY_NEWLINE);
        vty_out(vty, "%2s%-32s: %s%s", "", "LM-Interval", "--", VTY_NEWLINE);
        vty_out(vty, "%2s%-32s: %s%s", "", "LM-Frequency", "--", VTY_NEWLINE);
        vty_out(vty, "%2s%-32s: %s%s", "", "Max frame loss ratio(near-end)", "--", VTY_NEWLINE);
        vty_out(vty, "%2s%-32s: %s%s", "", "Min frame loss ratio(near-end)", "--", VTY_NEWLINE);
        vty_out(vty, "%2s%-32s: %s%s", "", "Mean frame loss ratio(near-end)", "--", VTY_NEWLINE);
        vty_out(vty, "%2s%-32s: %s%s", "", "Max frame loss ratio(far-end)", "--", VTY_NEWLINE);
        vty_out(vty, "%2s%-32s: %s%s", "", "Min frame loss ratio(far-end)", "--", VTY_NEWLINE);
        vty_out(vty, "%2s%-32s: %s%s", "", "Mean frame loss ratio(far-end)", "--", VTY_NEWLINE);
        return ;
    }

    vty_out(vty, "%2s%-32s: %d s%s", "", "LM-Interval", psess->info.lm_interval, VTY_NEWLINE);
    vty_out(vty, "%2s%-32s: %d times%s", "", "LM-Frequency", psess->info.lm_frequency, VTY_NEWLINE);
    vty_out(vty, "%2s%-32s: %s%%%s", "", "Max frame loss ratio(near-end)", f_max, VTY_NEWLINE);
    vty_out(vty, "%2s%-32s: %s%%%s", "", "Min frame loss ratio(near-end)", f_min, VTY_NEWLINE);
    vty_out(vty, "%2s%-32s: %s%%%s", "", "Mean frame loss ratio(near-end)", f_mean, VTY_NEWLINE);
    vty_out(vty, "%2s%-32s: %s%%%s", "", "Max frame loss ratio(far-end)", f_max_peer, VTY_NEWLINE);
    vty_out(vty, "%2s%-32s: %s%%%s", "", "Min frame loss ratio(far-end)", f_min_peer, VTY_NEWLINE);
    vty_out(vty, "%2s%-32s: %s%%%s", "", "Mean frame loss ratio(far-end)", f_mean_peer, VTY_NEWLINE);
    
    return ;
}

void show_mplsoam_session_dm_result(struct vty *vty, struct oam_session *psess)
{
    if(NULL == psess)
    {
        return ;
    }

    if(psess->info.dm_enable == OAM_STATUS_ENABLE)
    {
        vty_out(vty, "%2s%-32s: %s%s", "", "DM-state", "Running", VTY_NEWLINE);
    }
    else if(psess->info.dm_enable == OAM_STATUS_FINISH)
    {
        vty_out(vty, "%2s%-32s: %s%s", "", "DM-state", "Finish", VTY_NEWLINE);
    }
    else if(psess->info.dm_enable == OAM_STATUS_DISABLE)
    {
        vty_out(vty, "%2s%-32s: %s%s", "", "DM-state", "Disable", VTY_NEWLINE); 
        vty_out(vty, "%2s%-32s: %s%s", "", "DM-Interval", "--", VTY_NEWLINE);
        vty_out(vty, "%2s%-32s: %s%s", "", "DM-Frequency", "--", VTY_NEWLINE);
		vty_out(vty, "%2s%-32s: %s%s", "", "DM-exp", "--", VTY_NEWLINE);
		vty_out(vty, "%2s%-32s: %s%s", "", "DM-tlv-size", "--", VTY_NEWLINE);
        vty_out(vty, "%2s%-32s: %s%s", "", "The Max Delay(ns)", "--", VTY_NEWLINE);
        vty_out(vty, "%2s%-32s: %s%s", "", "The Max Delay Jitter(ns)", "--", VTY_NEWLINE);
        vty_out(vty, "%2s%-32s: %s%s", "", "The Min Delay(ns)", "--", VTY_NEWLINE);
        vty_out(vty, "%2s%-32s: %s%s", "", "The Min Delay Jitter(ns)", "--", VTY_NEWLINE);
        vty_out(vty, "%2s%-32s: %s%s", "", "The Mean Delay(ns)", "--", VTY_NEWLINE);
        vty_out(vty, "%2s%-32s: %s%s", "", "The Mean Delay Jitter(ns)", "--", VTY_NEWLINE);
        
        return ;
    }
    
    vty_out(vty, "%2s%-32s: %d s%s", "", "DM-Interval", psess->info.dm_interval, VTY_NEWLINE);
    vty_out(vty, "%2s%-32s: %d times%s", "", "DM-Frequency", psess->info.dm_frequency, VTY_NEWLINE);
	vty_out(vty, "%2s%-32s: %d%s", "", "DM-exp", psess->info.dm_exp, VTY_NEWLINE);
	vty_out(vty, "%2s%-32s: %d%s", "", "DM-tlv-size", psess->info.dm_tlv_len, VTY_NEWLINE);
	
    vty_out(vty, "%2s%-32s: %u%s", "", "The Max Delay(ns)", psess->sla.dm_max, VTY_NEWLINE);
    vty_out(vty, "%2s%-32s: %u%s", "", "The Max Delay Jitter(ns)", psess->sla.jitter_max, VTY_NEWLINE);
    vty_out(vty, "%2s%-32s: %u%s", "", "The Min Delay(ns)", psess->sla.dm_min, VTY_NEWLINE);
    vty_out(vty, "%2s%-32s: %u%s", "", "The Min Delay Jitter(ns)", psess->sla.jitter_min, VTY_NEWLINE);
    vty_out(vty, "%2s%-32s: %u%s", "", "The Mean Delay(ns)", psess->sla.dm_mean, VTY_NEWLINE);
    vty_out(vty, "%2s%-32s: %u%s", "", "The Mean Delay Jitter(ns)", psess->sla.jitter_mean, VTY_NEWLINE);
    
    return ;
}

void show_mplsoam_session(struct vty *vty, struct oam_session *psess)
{
	if(psess == NULL)
	{
		return ;
	}
	
	vty_out(vty, "%2s%-32s: %d%s", "", "session-id", psess->info.session_id, VTY_NEWLINE);
	if(psess->meg != NULL)
	{
    	vty_out(vty, "%2s%-32s: %s%s", "", "meg-name", psess->info.megname, VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "%2s%-32s: %s%s", "", "meg-name", "--", VTY_NEWLINE);
	}
	vty_out(vty, "%2s%-32s: %d%s", "", "meg-level", psess->info.level, VTY_NEWLINE);
	
	if(psess->info.mp_type == MP_TYPE_MEP)
	{
	//	vty_out(vty, "%2s%-32s: %d%s", "", "mp-type", "MEP", VTY_NEWLINE);
		vty_out(vty, "%2s%-32s: %d%s", "", "mep-id", psess->info.mp_id, VTY_NEWLINE);
	}
	else if(psess->info.mp_type == MP_TYPE_MIP)
	{
	//	vty_out(vty, "%2s%-32s: %d%s", "", "mp-type", "MIP", VTY_NEWLINE);
		vty_out(vty, "%2s%-32s: %d%s", "", "mip-id", psess->info.mp_id, VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "%2s%-32s: %s%s", "", "mep-id/mip-id", "--", VTY_NEWLINE);
	}
	if(psess->info.mp_type != MP_TYPE_MIP )
	{
		if(psess->info.remote_mep < 1)
	    {
	        vty_out(vty, "%2s%-32s: %s%s", "", "remote mep-id", "--", VTY_NEWLINE);
	    }
	    else
	    {
	        vty_out(vty, "%2s%-32s: %d%s", "", "remote mep-id", psess->info.remote_mep, VTY_NEWLINE);
	    }
	}
	else
	{
		if(psess->fwd_rmep == 0)
	    {
	        vty_out(vty, "%2s%-32s: %s%s", "", "fwd remote mep-id", "--", VTY_NEWLINE);
	    }
	    else
	    {
	        vty_out(vty, "%2s%-32s: %d%s", "", "fwd remote mep-id", psess->fwd_rmep, VTY_NEWLINE);
	    }
		if(psess->rev_rmep == 0)
	    {
	        vty_out(vty, "%2s%-32s: %s%s", "", "rev remote mep-id", "--", VTY_NEWLINE);
	    }
	    else
	    {
	        vty_out(vty, "%2s%-32s: %d%s", "", "rev remote mep-id", psess->rev_rmep, VTY_NEWLINE);
	    }
	}
	if(psess->info.mp_type != MP_TYPE_MIP)
	{
    	vty_out(vty, "%2s%-32s: %s%s", "", "oam-state", psess->info.state == OAM_STATUS_ENABLE ? "Enable":
	        (psess->info.state == OAM_STATUS_DISABLE ? "Disable":(psess->info.state == OAM_STATUS_UP ? "Up":"Down")), VTY_NEWLINE);
	}
	vty_out(vty, "%2s%-32s: %s%s", "", "oam-type", psess->info.type == OAM_TYPE_TUNNEL ? "Tunnel":
        (psess->info.type == OAM_TYPE_LSP ? "Lsp":(psess->info.type == OAM_TYPE_PW ? "Pw":
            (psess->info.type == OAM_TYPE_INTERFACE ? "Interface":"Invalid"))), VTY_NEWLINE);
	if(psess->info.mp_type == MP_TYPE_MIP)
	{
		if(psess->info.type == OAM_TYPE_LSP)
		{	
			if(psess->fwd_lsp == NULL)
			{
				vty_out(vty, "%2s%-32s: %s%s", "", "fwd lsp", "--", VTY_NEWLINE);
			}
			else
			{	
				vty_out(vty, "%2s%-32s: %s%s", "", "fwd lsp", psess->fwd_lsp->name, VTY_NEWLINE);
			}
			if(psess->rev_lsp == NULL)
			{
				vty_out(vty, "%2s%-32s: %s%s", "", "rev lsp", "--", VTY_NEWLINE);
			}
			else
			{	
				vty_out(vty, "%2s%-32s: %s%s", "", "rev lsp", psess->rev_lsp->name, VTY_NEWLINE);
			}
		}
		else if(psess->info.type == OAM_TYPE_PW)
		{
			if(psess->sw_pw == NULL)
			{
				vty_out(vty, "%2s%-32s: %s%s", "", "switch-pw", "--", VTY_NEWLINE);
			}
			else if(psess->sw_pw->pswitch_pw != NULL)
			{
				vty_out(vty, "%2s%-32s: %s/%s%s", "", "switch-pw", psess->sw_pw->name, psess->sw_pw->pswitch_pw->name, VTY_NEWLINE);
			}
		}
	}
	if(psess->info.mp_type != MP_TYPE_MIP)
	{
		vty_out(vty, "%2s%-32s: %s%s", "", "CC-state", psess->info.cc_enable == 1 ? "Enable":"Disable", VTY_NEWLINE);
	    if(psess->info.cc_interval == 0)
	    {
	        vty_out(vty, "%2s%-32s: %s%s", "", "CC-interval", "--", VTY_NEWLINE);    
    }
    else if(psess->info.cc_interval == 1000 || psess->info.cc_interval == 10000)
    {
	    vty_out(vty, "%2s%-32s: %ds%s", "", "CC-interval", psess->info.cc_interval == 1000 ? 1:10, VTY_NEWLINE);
    }
	else if(psess->info.cc_interval == 60000 || psess->info.cc_interval == 600000)
	{
		vty_out(vty, "%2s%-32s: %dmin%s", "", "CC-interval", psess->info.cc_interval == 60000 ? 1:10, VTY_NEWLINE);
	}
    else if(psess->info.cc_interval == 3)
    {
		    vty_out(vty, "%2s%-32s: 3.3ms%s", "", "CC-interval", VTY_NEWLINE);        
	    }
	    else
	    {
		    vty_out(vty, "%2s%-32s: %dms%s", "", "CC-interval", psess->info.cc_interval, VTY_NEWLINE);
	    }
	}

	vty_out(vty, "%2s%-32s: 0x%X%s", "", "Channel-type", psess->info.channel_type, VTY_NEWLINE);
	
	if(psess->info.mp_type != MP_TYPE_MIP)
	{
		vty_out(vty, "%2s%-32s: %d%s", "", "priority", psess->info.priority, VTY_NEWLINE);
    	//vty_out(vty, "%2s%-32s: 0x%X%s", "", "Channel-type", psess->info.channel_type, VTY_NEWLINE);
	    if(((psess->info.type == OAM_TYPE_INTERFACE) || (psess->info.type == OAM_TYPE_INVALID)) &&(ether_is_zero_mac(psess->info.mac)))
	    {
	        vty_out(vty, "%2s%-32s: %02X:%02X:%02X:%02X:%02X:%02X%s", "", "dmac", psess->info.mac[0], psess->info.mac[1],
	                psess->info.mac[2], psess->info.mac[3], psess->info.mac[4], psess->info.mac[5], VTY_NEWLINE);
	        if((psess->info.vlan == 0) || (psess->info.vlan > 4094))
	        {
	            vty_out(vty, "%2s%-32s: %s%s", "", "vlan", "--", VTY_NEWLINE);    
	        }
        	else 
        	{
     	       vty_out(vty, "%2s%-32s: %d%s", "", "vlan", psess->info.vlan, VTY_NEWLINE);
	        }
    	}
		if(psess->ais_enable == 1)
		{
			vty_out(vty, "%2s%-32s: %s%s", "", "AIS-state", "Enable", VTY_NEWLINE);
			vty_out(vty, "%2s%-32s: %d%s", "", "AIS-interval", psess->pais->ais_interval, VTY_NEWLINE);
			vty_out(vty, "%2s%-32s: %d%s", "", "AIS-level", psess->pais->level, VTY_NEWLINE);
		}
		else
		{
			vty_out(vty, "%2s%-32s: %s%s", "", "AIS-state", "Disable", VTY_NEWLINE);
			vty_out(vty, "%2s%-32s: %s%s", "", "AIS-interval", "--", VTY_NEWLINE);
			vty_out(vty, "%2s%-32s: %s%s", "", "AIS-level", "--", VTY_NEWLINE);
		}

		if(psess->lck_enable == 1)
		{
			vty_out(vty, "%2s%-32s: %s%s", "", "Lock-state", "Enable", VTY_NEWLINE);
			vty_out(vty, "%2s%-32s: %d%s", "", "Lock-interval", psess->plck->lck_interval, VTY_NEWLINE);
			vty_out(vty, "%2s%-32s: %d%s", "", "Lock-level", psess->plck->level, VTY_NEWLINE);
		}
		else
		{
			vty_out(vty, "%2s%-32s: %s%s", "", "Lock-state", "Disable", VTY_NEWLINE);
			vty_out(vty, "%2s%-32s: %s%s", "", "Lock-interval", "--", VTY_NEWLINE);
			vty_out(vty, "%2s%-32s: %s%s", "", "Lock-level", "--", VTY_NEWLINE);
		}

		if(psess->csf_enable == 1)
		{
			if(NULL != psess->pcsf)
			{
				vty_out(vty, "%2s%-32s: %s%s", "", "Csf-state", "Enable", VTY_NEWLINE);
				vty_out(vty, "%2s%-32s: %d%s", "", "Csf-interval", psess->pcsf->csf_interval, VTY_NEWLINE);
				vty_out(vty, "%2s%-32s: %d%s", "", "Csf-level", psess->pcsf->level, VTY_NEWLINE);
			}
		}
		else
		{
			vty_out(vty, "%2s%-32s: %s%s", "", "Csf-state", "Disable", VTY_NEWLINE);
			vty_out(vty, "%2s%-32s: %s%s", "", "Csf-interval", "--", VTY_NEWLINE);
			vty_out(vty, "%2s%-32s: %s%s", "", "Csf-level", "--", VTY_NEWLINE);
		}
		
	    vty_out(vty, "%s", VTY_NEWLINE);
    	show_mplsoam_session_current_alarm(vty, psess);
	    vty_out(vty, "%s", VTY_NEWLINE);
	    show_mplsoam_session_lm_result(vty, psess);
	    vty_out(vty, "%s", VTY_NEWLINE);
	    show_mplsoam_session_dm_result(vty, psess);
	    vty_out(vty, "%s", VTY_NEWLINE);
	}
	return ;
}

static int show_mplsoam_session_brief(struct vty *vty, struct oam_session *psess)
{
	//char ifname[IFNET_NAMESIZE];

	//FLUSH_LINE_CHECK( tpoam_line, vty->flush_cp)
	vty_out(vty, "%2s%-32s: %d%s", "", "session id", psess->info.session_id, VTY_NEWLINE);
		
	//FLUSH_LINE_CHECK( tpoam_line, vty->flush_cp)
	{
		if(psess->meg != NULL)
		{
			vty_out(vty, "%2s%-32s: %s%s", "", "meg-name", psess->info.megname, VTY_NEWLINE);
		}
		else
		{
			vty_out(vty, "%2s%-32s: %s%s", "", "meg-name", "--", VTY_NEWLINE);
		}
	}
	//FLUSH_LINE_CHECK( tpoam_line, vty->flush_cp)
	vty_out(vty, "%2s%-32s: %d%s", "", "meg-level", psess->info.level, VTY_NEWLINE);
    //FLUSH_LINE_CHECK( tpoam_line, vty->flush_cp)
	{
		if(psess->info.mp_type == MP_TYPE_MEP)
		{
			vty_out(vty, "%2s%-32s: %d%s", "", "mep-id", psess->info.mp_id, VTY_NEWLINE);
		}
		else if(psess->info.mp_type == MP_TYPE_MIP)
		{
			vty_out(vty, "%2s%-32s: %d%s", "", "mip-id", psess->info.mp_id, VTY_NEWLINE);
		}
	}
	//FLUSH_LINE_CHECK( tpoam_line, vty->flush_cp)
    {	if(psess->info.mp_type != MP_TYPE_MIP)
		{
	        if(psess->info.remote_mep < 1)
	        {
	            vty_out(vty, "%2s%-32s: %s%s", "", "remote mep-id", "--", VTY_NEWLINE);    
	        }
	        else
	        {
	            vty_out(vty, "%2s%-32s: %d%s", "", "remote mep-id", psess->info.remote_mep, VTY_NEWLINE);
	        }
		}
    }
	//FLUSH_LINE_CHECK( tpoam_line, vty->flush_cp)
    {	if(psess->info.mp_type == MP_TYPE_MIP)
		{
	        if(psess->fwd_rmep == 0)
	        {
	            vty_out(vty, "%2s%-32s: %s%s", "", "fwd remote mep-id", "--", VTY_NEWLINE);    
	        }
	        else
	        {
	            vty_out(vty, "%2s%-32s: %d%s", "", "fwd remote mep-id", psess->fwd_rmep, VTY_NEWLINE);
	        }
		}
    }
	//FLUSH_LINE_CHECK( tpoam_line, vty->flush_cp)
    {	if(psess->info.mp_type == MP_TYPE_MIP)
		{
	        if(psess->rev_rmep == 0)
	        {
	            vty_out(vty, "%2s%-32s: %s%s", "", "rev remote mep-id", "--", VTY_NEWLINE);    
	        }
	        else
	        {
	            vty_out(vty, "%2s%-32s: %d%s", "", "rev remote mep-id", psess->rev_rmep, VTY_NEWLINE);
	        }
		}
    }
    //FLUSH_LINE_CHECK( tpoam_line, vty->flush_cp)
    {
    	if(psess->info.mp_type != MP_TYPE_MIP)
    	{
		    vty_out(vty, "%2s%-32s: %s%s", "", "oam-state", psess->info.state == OAM_STATUS_ENABLE ? "Enable":
		        (psess->info.state == OAM_STATUS_DISABLE ? "Disable":(psess->info.state == OAM_STATUS_UP ? "Up":"Down")), VTY_NEWLINE);
    	}
	}
    //FLUSH_LINE_CHECK( tpoam_line, vty->flush_cp)
	vty_out(vty, "%2s%-32s: %s%s", "", "oam-type", psess->info.type == OAM_TYPE_TUNNEL ? "Tunnel":
            (psess->info.type == OAM_TYPE_LSP ? "Lsp":(psess->info.type == OAM_TYPE_PW ? "Pw":
                (psess->info.type == OAM_TYPE_INTERFACE ? "Interface":"Invalid"))), VTY_NEWLINE);
    //FLUSH_LINE_CHECK( tpoam_line, vty->flush_cp)
    {
    	if(psess->info.mp_type != MP_TYPE_MIP)
    	{
    		vty_out(vty, "%2s%-32s: %s%s", "", "CC-state", psess->info.cc_enable == 1 ? "Enable":"Disable", VTY_NEWLINE);
    	}
    }
    //FLUSH_LINE_CHECK( tpoam_line, vty->flush_cp)
    {
    	if(psess->info.mp_type != MP_TYPE_MIP)
    	{
	        if(psess->info.lm_enable == OAM_STATUS_ENABLE)
	        {
	            vty_out(vty, "%2s%-32s: %s%s", "", "LM-state", "Running", VTY_NEWLINE);
	        }
	        else if(psess->info.lm_enable == OAM_STATUS_FINISH)
	        {
	            vty_out(vty, "%2s%-32s: %s%s", "", "LM-state", "Finish", VTY_NEWLINE);
	        }
	        else if(psess->info.lm_enable == OAM_STATUS_DISABLE)
	        {
	            vty_out(vty, "%2s%-32s: %s%s", "", "LM-state", "Disable", VTY_NEWLINE);
	        }
    	}
    }
    //FLUSH_LINE_CHECK( tpoam_line, vty->flush_cp)
    {
    	if(psess->info.mp_type != MP_TYPE_MIP)
    	{
	        if(psess->info.dm_enable == OAM_STATUS_ENABLE)
	        {
	            vty_out(vty, "%2s%-32s: %s%s", "", "DM-state", "Running", VTY_NEWLINE);
	        }
	        else if(psess->info.dm_enable == OAM_STATUS_FINISH)
	        {
	            vty_out(vty, "%2s%-32s: %s%s", "", "DM-state", "Finish", VTY_NEWLINE);
	        }
	        else if(psess->info.dm_enable == OAM_STATUS_DISABLE)
	        {
	            vty_out(vty, "%2s%-32s: %s%s", "", "DM-state", "Disable", VTY_NEWLINE); 
	        }
    	}
    }
    //FLUSH_LINE_CHECK( tpoam_line, vty->flush_cp)
    vty_out(vty, "%2s%-32s: %s%s", "", "Alarm-report", psess->alarm_rpt == 1 ? "Enable":"Disable", VTY_NEWLINE);
    //FLUSH_LINE_CHECK( tpoam_line, vty->flush_cp)
    vty_out(vty, "%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}

static int show_mplsoam_meg_info(struct vty *vty, struct oam_meg *meg)
{
	struct listnode    *node    = NULL;
	struct oam_session *session = NULL;
	
	if(NULL == meg)
	{
		return CMD_WARNING;
	}
	
	vty_out(vty, "--------------------------------------------------%s", VTY_NEWLINE);
	vty_out(vty, "%2s%-5s %s%s", "", "MEG", meg->name, VTY_NEWLINE);
	vty_out(vty, "--------------------------------------------------%s", VTY_NEWLINE);

	vty_out(vty, "%2s%-32s: %s%s", "", "meg-name", meg->name, VTY_NEWLINE);
	vty_out(vty, "%2s%-32s: %d%s", "", "meg-level", meg->level, VTY_NEWLINE);
	vty_out(vty, "%2s%-32s: %d%s", "", "session count", meg->session_list->count, VTY_NEWLINE);
	vty_out(vty, "%s", VTY_NEWLINE);
	for(ALL_LIST_ELEMENTS_RO(meg->session_list, node, session))
	{
		vty_out(vty, "%2s[%-8s %d]%s", "", "session", session->info.session_id, VTY_NEWLINE);
		if(session->info.mp_type == MP_TYPE_MEP)
		{
			vty_out(vty, "%2s%-32s: %s%s", "", "mp-type", "MEP", VTY_NEWLINE);
			if(session->info.mp_id != 0)
			{
				vty_out(vty, "%2s%-32s: %d%s", "", "mep-id", session->info.mp_id, VTY_NEWLINE);	
			}
		}
		else if(session->info.mp_type == MP_TYPE_MIP)
		{
			vty_out(vty, "%2s%-32s: %s%s", "", "mp-type", "MIP", VTY_NEWLINE);
			if(session->info.mp_id != 0)
			{
				vty_out(vty, "%2s%-32s: %d%s", "", "mip-id", session->info.mp_id, VTY_NEWLINE);	
			}
		}
		else
		{
			vty_out(vty, "%2s%-32s: %s%s", "", "mp-type", "--", VTY_NEWLINE);
			if(session->info.mp_id == 0)
			{
				vty_out(vty, "%2s%-32s: %d%s", "", "mip-id", session->info.mp_id, VTY_NEWLINE);	
			}
		}
		if(session->info.remote_mep < 1)
        {
            vty_out(vty, "%2s%-32s: %s%s", "", "remote mep-id", "--", VTY_NEWLINE);    
        }
        else
        {
            vty_out(vty, "%2s%-32s: %d%s", "", "remote mep-id", session->info.remote_mep, VTY_NEWLINE);
        }
		
		vty_out(vty, "%2s%-32s: %s%s", "", "oam-state", session->info.state == OAM_STATUS_ENABLE ? "Enable":
        	(session->info.state == OAM_STATUS_DISABLE ? "Disable":(session->info.state == OAM_STATUS_UP ? "Up":"Down")), VTY_NEWLINE);
		vty_out(vty, "%2s%-32s: %s%s", "", "oam-type", session->info.type == OAM_TYPE_TUNNEL ? "Tunnel":
            (session->info.type == OAM_TYPE_LSP ? "Lsp":(session->info.type == OAM_TYPE_PW ? "Pw":
                (session->info.type == OAM_TYPE_INTERFACE ? "Interface":"Invalid"))), VTY_NEWLINE);
		vty_out(vty, "%2s%-32s: %s%s", "", "CC-state", session->info.cc_enable == 1 ? "Enable":"Disable", VTY_NEWLINE);

	}
	vty_out(vty, "--------------------------------------------------%s", VTY_NEWLINE);

	return CMD_SUCCESS;
}



/* oam mep*/
DEFUN(mplstp_oam_session,
	mplstp_oam_session_cmd,
	"mplstp-oam session <1-65535>",
	"Mpls transport profile OAM\n"
	"Session\n"
	"Range of session:<1-65535>\n")
{
	struct oam_session *psess = NULL;
	uint16_t session_id = 0;
    char *pprompt = NULL;
    int ret = 0;

	session_id = atoi(argv[0]);

	psess = mplsoam_session_lookup(session_id);
	if(NULL == psess)
	{
		psess = mplsoam_session_create(session_id);
		if(NULL == psess)
		{
			vty_error_out(vty, "Fail to malloc for session!%s",VTY_NEWLINE);
			return CMD_WARNING;
		}
        ret = mplsoam_session_add(psess);
    	if(ret != ERRNO_SUCCESS)
    	{
    	    vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);
    		XFREE(MTYPE_MPLSOAM_SESS, psess);
    		return CMD_WARNING;
    	}
        gmplsoam.sess_total++;
	}
	
	vty->index = psess;
	vty->node = MPLSTP_OAM_SESS_NODE;
    pprompt = vty->change_prompt;
    if ( pprompt )
    {
        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-oam-session-%d)# ", session_id);
	}

	return CMD_SUCCESS;
}

DEFUN(no_mplstp_oam_session,
	no_mplstp_oam_session_cmd,
	"no mplstp-oam session <1-65535>",
	"Delete\n"
	"Mpls transport profile OAM\n"
	"Session\n"
	"Range of session:<1-65535>\n")
{
	struct oam_session *psess = NULL;
	uint16_t session_id = 0;
	int ret = 0;
	
	session_id = atoi(argv[0]);

	psess = mplsoam_session_lookup(session_id);
	if(NULL == psess)
	{
		vty_error_out(vty, "Session:%d is not exist!%s", session_id, VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	MPLSOAM_SESSION_ALREADY_ENABLE(vty, psess->info.type);

	ret = mplsoam_session_delete(session_id);
    if(ret == ERRNO_NOT_FOUND)
    {
        vty_error_out(vty, "Not found.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    gmplsoam.sess_total--;
    
	return CMD_SUCCESS;
}

DEFUN(mplstp_oam_meg,
	mplstp_oam_meg_cmd,
	"mplstp-oam meg MEGNAME",
	"Mpls transport profile OAM\n"
	"Maintenance entity group\n"
	"String of meg name:<1-12>\n")
{
	struct oam_meg *meg = NULL;
	uchar name[NAME_STRING_LEN] = {0};
    char *pprompt = NULL;
    int ret = 0;

	if(strlen(argv[0]) > 12)
	{
		vty_error_out(vty, "Length of meg name is less than 12 bytes.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	memcpy(name, argv[0], strlen(argv[0]));
	
	meg = mplsoam_meg_lookup(name);
	if(NULL == meg)
	{
		meg = mplsoam_meg_create(name);
		if(NULL == meg)
		{
			vty_error_out(vty, "Fail to malloc for meg entry!%s",VTY_NEWLINE);
			return CMD_WARNING;
		}
        ret = mplsoam_meg_add(meg);
    	if(ret != ERRNO_SUCCESS)
    	{
    	    vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);
    		XFREE(MTYPE_MPLSOAM_SESS, meg);
    		return CMD_WARNING;
    	}
	}
	
	vty->index = meg;
	vty->node = MPLSTP_OAM_MEG_NODE;
    pprompt = vty->change_prompt;
    if ( pprompt )
    {
        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-oam-meg-%s)# ", name);
	}

	return CMD_SUCCESS;
}

DEFUN(no_mplstp_oam_meg,
	no_mplstp_oam_meg_cmd,
	"no mplstp-oam meg MEGNAME",
	"Delete\n"
	"Mpls transport profile OAM\n"
	"Maintenance entity group\n"
	"String of meg name:<1-12>\n")
{
	struct oam_meg *meg = NULL;
	uchar name[NAME_STRING_LEN] = {0};
	int ret = 0;

	if(strlen(argv[0]) > 12)
	{
		vty_error_out(vty, "Input parameter error.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	memcpy(name, argv[0], strlen(argv[0]));

	meg = mplsoam_meg_lookup(name);
	if(NULL == meg)
	{
		vty_error_out(vty, "Meg:%s is not exist!%s", name, VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(!list_isempty(meg->session_list))
	{
		vty_warning_out(vty, "Meg:%s is serviced by sessions, please unservice first.%s", meg->name, VTY_NEWLINE);
		return CMD_WARNING;
	}

	ret = mplsoam_meg_delete(name);
    if(ret == ERRNO_NOT_FOUND)
    {
        vty_error_out(vty, "Not found.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
	return CMD_SUCCESS;
}


DEFUN(mplstp_oam_meg_level,
	mplstp_oam_meg_level_cmd,
	"level <0-7>",
	"Meg level\n"
	"Range of meg level:<0-7>\n")
{
	struct oam_meg *meg = vty->index;

	if(!list_isempty(meg->session_list))
	{
		vty_warning_out(vty, "Meg:%s is already serviced by sessions.%s", meg->name, VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	meg->level = atoi(argv[0]);

	return CMD_SUCCESS;
}

DEFUN(no_mplstp_oam_meg_level,
	no_mplstp_oam_meg_level_cmd,
	"no level",
	"Delete\n"
	"Level\n")
{
	struct oam_meg *meg = vty->index;
	
	if(!list_isempty(meg->session_list))
	{
		vty_warning_out(vty, "Meg:%s is already serviced by sessions.%s", meg->name, VTY_NEWLINE);
		return CMD_WARNING;
	}

	meg->level = 7;

	return CMD_SUCCESS;
}


DEFUN(mplstp_oam_service_meg,
	mplstp_oam_service_meg_cmd,
	"service meg MEGNAME",
	"Service\n"
	"Maintenance entity group\n"
	"String of meg name:<1-12>\n")
{
	struct oam_session *psess = (struct oam_session *)vty->index;
	//char name[NAME_STRING_LEN] = {0};
	struct oam_meg *meg = NULL;

	if(strlen(argv[0]) > 12)
	{
		vty_error_out(vty, "Input parameter error.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	MPLSOAM_SESSION_ALREADY_ENABLE(vty, psess->info.type);
	
	meg = mplsoam_meg_lookup((uchar *)argv[0]);
	if(NULL == meg)
	{
		vty_error_out(vty, "Meg:%s is not exist.%s", argv[0], VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(psess->meg == meg)
	{
		return CMD_SUCCESS;
	}
	if(meg->session_list->count >= 1)
	{
		vty_warning_out(vty, "Meg:%s has already one ME.%s", meg->name, VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(psess->meg != NULL)
	{
		listnode_delete(psess->meg->session_list, psess);
		psess->meg = NULL;
		psess->info.level = 7;
		memset(psess->info.megname, 0, NAME_STRING_LEN);
	}
	
	psess->meg = meg;
	psess->info.level = meg->level;
	memcpy(psess->info.megname, argv[0], strlen(argv[0]));
	listnode_add(meg->session_list, psess);
	
	return CMD_SUCCESS;
}

DEFUN(no_mplstp_oam_service_meg,
	no_mplstp_oam_service_meg_cmd,
	"no service meg",
	"Delete\n"
	"Service\n"
	"Maintenance entity group\n")
{
	struct oam_session *psess = (struct oam_session *)vty->index;

	MPLSOAM_SESSION_ALREADY_ENABLE(vty, psess->info.type);

	if(NULL == psess->meg)
	{
		vty_warning_out(vty, "Session %d is not service meg.%s", psess->info.session_id, VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(psess->meg->session_list != NULL)
	{
		listnode_delete(psess->meg->session_list, psess);
	}
	else
	{
		return CMD_SUCCESS;
	}
	psess->meg = NULL;
	psess->info.level = 7;
	memset(psess->info.megname, 0, NAME_STRING_LEN);

	return CMD_SUCCESS;
}



DEFUN(mplstp_oam_mp,
	mplstp_oam_mp_cmd,
	"(mep-id | mip-id) <1-8191>",
	"MEP id\n"
	"MIP id\n"
	"Range of mp-id:<1-8191>\n")
{
	struct oam_session *psess    = (struct oam_session *)vty->index;
	struct oam_session *sess_old = NULL;
	uint16_t            mp_id    = 0;
	struct listnode    *node     = NULL;
	
	MPLSOAM_SESSION_ALREADY_ENABLE(vty, psess->info.type);
	
	mp_id = atoi(argv[1]);
	
	if(0 == strncmp(argv[0], "mep", 3))
	{
		if(psess->meg != NULL)
		{
			for(ALL_LIST_ELEMENTS_RO(psess->meg->session_list, node, sess_old))
			{
				if(sess_old != NULL && sess_old->info.mp_type == MP_TYPE_MEP && sess_old->info.mp_id == mp_id)
				{
					vty_error_out(vty, "Mep-id:%d is alredy exist in the meg:%s.%s",
							mp_id, psess->meg->name, VTY_NEWLINE);
					return CMD_WARNING;
				}
			}
		}
		psess->info.mp_type = MP_TYPE_MEP;
	}
	else if(0 == strncmp(argv[0], "mip", 3))
	{
		if(psess->meg != NULL)
		{
			for(ALL_LIST_ELEMENTS_RO(psess->meg->session_list, node, sess_old))
			{
				if(sess_old != NULL && sess_old->info.mp_type == MP_TYPE_MIP && sess_old->info.mp_id == mp_id)
				{
					vty_error_out(vty, "Mip-id:%d is alredy exist in the meg:%s.%s",
							mp_id, psess->meg->name, VTY_NEWLINE);
					return CMD_WARNING;
				}
			}
		}
		psess->info.mp_type = MP_TYPE_MIP;
		psess->info.cc_enable = 0;
		psess->info.cc_interval = 0;
		psess->info.remote_mep = 0;
	}
	psess->info.mp_id	= mp_id;

	return CMD_SUCCESS;
}

DEFUN(no_mplstp_oam_mp,
	no_mplstp_oam_mp_cmd,
	"no (mep-id | mip-id) <1-8191>",
	"Delete\n"
	"MEP id\n"
	"MIP id\n"
	"Range of mp-id:<1-8191>\n")
{
	struct oam_session *psess = (struct oam_session *)vty->index;
	uint16_t            mp_id = 0;
	struct oam_meg     *meg_backup = NULL;
	uint16_t            sess_backup = 0;
	
	MPLSOAM_SESSION_ALREADY_ENABLE(vty, psess->info.type);

	mp_id = atoi(argv[1]);
	if(psess->info.mp_id != mp_id)
	{
		vty_warning_out(vty, "The input parameter is error.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	meg_backup = psess->meg;
	sess_backup = psess->info.session_id;

	if(0 == strncmp(argv[0], "mep", 3))
	{
		if(psess->info.mp_type == MP_TYPE_MEP)
		{
			if(psess->ais_enable == 1)
			{
				mplsoam_session_ais_disable(psess);
			}
			if(psess->client_list)
			{
				list_delete(psess->client_list);
			}

			if(psess->lck_enable == 1)
			{
				mplsoam_session_lck_disable(psess);
			}
			memset(psess, 0, sizeof(struct oam_session));
			
			if(meg_backup != NULL)
			{
				psess->meg = meg_backup;
				memcpy(psess->info.megname, meg_backup->name, strlen((char *)meg_backup->name));
				psess->info.level        = meg_backup->level;
			}
			else
			{
				psess->info.level        = 7;
			}
			psess->client_list = list_new();
			psess->info.session_id   = sess_backup;
			psess->info.priority     = 5;
			psess->info.channel_type = gmplsoam.global_chtype;
		}
		else
		{
			vty_error_out(vty, "Type of MP is not MEP.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	else if(0 == strncmp(argv[0], "mip", 3))
	{
		if(psess->info.mp_type == MP_TYPE_MIP)
		{
			psess->fwd_rmep     = 0;
			psess->rev_rmep     = 0;
			psess->info.mp_type = MP_TYPE_INVALID;
			psess->info.mp_id   = 0;
		}
		else
		{	
			vty_error_out(vty, "Type of MP is not MIP.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}

	return CMD_SUCCESS;
}


DEFUN(mplstp_oam_mip_rmep_config,
	mplstp_oam_mip_rmep_config_cmd,
	"mip fwd-rmep <1-8191> rev-rmep <1-8191>",
	"Maintenance entity group intermediate Point\n"
	"Forward remote id\n"
	"Range of forward remote-id:<1-8191>\n"
	"Reverse remote id\n"
	"Range of reverse remote-id:<1-8191>\n")
{
	struct oam_session *psess = (struct oam_session *)vty->index;

	MPLSOAM_MPTYPE_NOT_MIP(vty, psess->info.mp_type);
	MPLSOAM_SESSION_ALREADY_ENABLE(vty, psess->info.type);

	psess->fwd_rmep = atoi(argv[0]);
	psess->rev_rmep = atoi(argv[1]);

    return CMD_SUCCESS;
}

DEFUN(no_mplstp_oam_mip_rmep_config,
	no_mplstp_oam_mip_rmep_config_cmd,
	"no mip remote-mep",
	"Delete\n"
	"Maintenance entity group intermediate Point\n"
	"Remote mep\n")
{
	struct oam_session *psess = (struct oam_session *)vty->index;

	MPLSOAM_MPTYPE_NOT_MIP(vty, psess->info.mp_type);
	MPLSOAM_SESSION_ALREADY_ENABLE(vty, psess->info.type);

    psess->fwd_rmep = 0;
    psess->rev_rmep = 0;

    return CMD_SUCCESS;
}


DEFUN(mplstp_oam_mip_bind_lsp,
	mplstp_oam_mip_bind_lsp_cmd,
	"mip bind lsp LSPNAME (fwd | rev)",
	"Maintenance entity group intermediate Point\n"
	"Bind\n"
	"Lsp\n"
	"String of lspname:<1-31>\n"
	"Forward transit lsp\n"
	"Reverse transit lsp\n")
{
	struct oam_session *psess = (struct oam_session *)vty->index;
    struct static_lsp *plsp = NULL;

	if(strlen(argv[0]) > 31)
	{
		vty_error_out(vty, "Input parameter error.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	MPLSOAM_MPTYPE_NOT_MIP(vty, psess->info.mp_type);
	
    if(psess->info.type == OAM_TYPE_PW)
    {
        vty_error_out(vty, "Already bind service pw.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if(psess->fwd_rmep == 0 || psess->rev_rmep == 0)
    {
        vty_error_out(vty, "Please config forward remote mep and reverse remote mep.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	if(psess->meg == NULL)
	{
		vty_warning_out(vty, "Please service meg.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
    plsp = static_lsp_lookup((uchar *)argv[0]);
    if(NULL == plsp)
    {
        vty_error_out(vty, "Lsp:%s is not exist.%s", argv[0], VTY_NEWLINE);
        return CMD_WARNING;
    }
	else if(plsp->direction != LSP_DIRECTION_TRANSIT)
	{
		vty_error_out(vty, "Lsp:%s is not transit lsp.%s", argv[0], VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	if(argv[1][0] == 'f')
	{
		if(psess->fwd_lsp != NULL)
		{
			vty_error_out(vty, "Already bind forward lsp.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		else if(psess->rev_lsp == plsp)
		{
			vty_error_out(vty, "Already bind reversed lsp.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		else
		{
			psess->fwd_lsp = plsp;
		}
		
	}
	else if(argv[1][0] == 'r')
	{
		if(psess->rev_lsp != NULL)
		{
			vty_error_out(vty, "Already bind reversed lsp.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		else if(psess->fwd_lsp == plsp)
		{
			vty_error_out(vty, "Already bind forward lsp.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		else
		{
			psess->rev_lsp = plsp;
		}
	}
	
	plsp->mplsoam_id = psess->info.session_id;
    psess->info.type = OAM_TYPE_LSP;

    return CMD_SUCCESS;
}

DEFUN(no_mplstp_oam_mip_bind_lsp,
	no_mplstp_oam_mip_bind_lsp_cmd,
	"no mip bind lsp (fwd | rev)",
	"Delete\n"
	"Maintenance entity group intermediate Point\n"
	"Bind\n"
	"Lsp\n"
	"Forward direction\n"
	"Reverse direction\n")
{
	struct oam_session *psess = (struct oam_session *)vty->index;

	MPLSOAM_MPTYPE_NOT_MIP(vty, psess->info.mp_type);
	
	if(psess->info.type != OAM_TYPE_LSP)
	{
		vty_error_out(vty, "Session:%d is not bind lsp.%s", psess->info.session_id, VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	if(argv[0][0] == 'f')
	{
		if(NULL == psess->fwd_lsp)
		{
			return CMD_WARNING;
		}
		else
		{	
			mplsoam_session_unbind_service(psess->info.session_id, psess->fwd_lsp);
		}
	}
	else if(argv[0][0] == 'r')
	{
		if(NULL == psess->rev_lsp)
		{
			return CMD_WARNING;
		}
		else
		{
			mplsoam_session_unbind_service(psess->info.session_id, psess->rev_lsp);
		}
	}
	
    return CMD_SUCCESS;
}

#if 0
DEFUN(mplstp_oam_mip_bind_pw,
	mplstp_oam_mip_bind_pw_cmd,
	"mip bind pw PWNAME",
	"Maintenance entity group intermediate Point\n"
	"Bind\n"
	"Pw\n"
	"String of pwname:<1-31>\n")
{
	struct oam_session *psess = (struct oam_session *)vty->index;
    struct l2vc_entry *pl2vc = NULL;

	if(strlen(argv[0]) > 31)
	{
		vty_error_out(vty, "Input parameter error.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	MPLSOAM_SESSION_ALREADY_ENABLE(vty, psess->info.type);
    if(psess->fwd_rmep == 0 || psess->rev_rmep == 0)
    {
        vty_error_out(vty, "Please config forward remote mep and reverse remote mep.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
	
	if(psess->meg == NULL)
	{
		vty_warning_out(vty, "Please service meg.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
    pl2vc = l2vc_lookup(argv[0]);
    if(NULL == pl2vc)
    {
        vty_error_out(vty, "Pw:%s is not exist.%s", argv[0], VTY_NEWLINE);
        return CMD_WARNING;
    }

    if(pl2vc->pswitch_pw == NULL)
    {
        vty_error_out(vty, "Pw:%s is not switch-pw.%s", argv[0], VTY_NEWLINE);
        return CMD_WARNING;
    }

	if((pl2vc->mplsoam_id == psess->info.mp_id 
		&& pl2vc->pswitch_pw->mplsoam_id == psess->info.mp_id)
			&& (psess->sw_pw == pl2vc || psess->sw_pw == pl2vc->pswitch_pw))
	{
		return CMD_SUCCESS;
	}
    if(pl2vc->mplsoam_id != 0 || pl2vc->pswitch_pw->mplsoam_id != 0)
    {
        vty_error_out(vty, "Pw:%s is already bind with other mip.%s", argv[0], VTY_NEWLINE);
        return CMD_WARNING;
    }

    psess->sw_pw = pl2vc;
	psess->info.type = OAM_TYPE_PW;
    pl2vc->mplsoam_id = psess->info.session_id;
	pl2vc->pswitch_pw->mplsoam_id = psess->info.session_id;
	
    return CMD_SUCCESS;
}

DEFUN(no_mplstp_oam_mip_bind_pw,
	no_mplstp_oam_mip_bind_pw_cmd,
	"no mip bind pw",
    "Delete\n"
	"Maintenance entity group intermediate Point\n"
	"Bind\n"
	"Pw\n")
{
    struct oam_session *psess = (struct oam_session *)vty->index;
	
    if(psess->sw_pw == NULL)
    {
        vty_error_out(vty, "Session:%d is not bind switch-pw.%s", psess->info.session_id, VTY_NEWLINE);
        return CMD_WARNING;
    }
	mplsoam_session_unbind_service(psess->info.session_id, psess->sw_pw);

    return CMD_SUCCESS;
}
#endif

DEFUN(mplstp_oam_session_priority,
	mplstp_oam_session_priority_cmd,
	"priority <0-7>",
	"Oam packet priority\n"
	"Range of priority:<0-7>\n")
{
	struct oam_session *psess = (struct oam_session *)vty->index;
	uint16_t pri = 0;
	
    MPLSOAM_SESSION_ALREADY_ENABLE(vty, psess->info.type);
    
    pri = atoi(argv[0]);

	psess->info.priority = pri;

	return CMD_SUCCESS;
}

DEFUN(no_mplstp_oam_session_priority,
	no_mplstp_oam_session_priority_cmd,
	"no priority",
	"Delete\n"
	"Oam packet priority\n")
{	
	struct oam_session *psess = (struct oam_session *)vty->index;
    
    MPLSOAM_SESSION_ALREADY_ENABLE(vty, psess->info.type);
    
	psess->info.priority = 5;

	return CMD_SUCCESS;
}


DEFUN(mplstp_oam_session_rmep,
	mplstp_oam_session_rmep_cmd,
	"remote-mep <1-8191>",
	"Remote-mep\n"
	"Range of remote-mep:<1-8191>\n")
{
	struct oam_session *psess = (struct oam_session *)vty->index;
	uint16_t rmep = 0;

	MPLSOAM_MPTYPE_MIP(vty, psess->info.mp_type);
    MPLSOAM_SESSION_ALREADY_ENABLE(vty, psess->info.type);

    rmep = atoi(argv[0]);

	psess->info.remote_mep = rmep;

	return CMD_SUCCESS;
}

DEFUN(no_mplstp_oam_session_rmep,
	no_mplstp_oam_session_rmep_cmd,
	"no remote-mep <1-8191>",
	"Delete\n"
	"Remote-mep\n"
	"Range of remote-mep:<1-8191>\n")
{
	struct oam_session *psess = (struct oam_session *)vty->index;
	uint16_t rmep = 0;
	
	MPLSOAM_MPTYPE_MIP(vty, psess->info.mp_type);
    MPLSOAM_SESSION_ALREADY_ENABLE(vty, psess->info.type);
    
	rmep = atoi(argv[0]);

	if(psess->info.remote_mep != rmep)
	{
		vty_error_out(vty, "The remote_mep_id of session:%d is not %d!%s",
				psess->info.session_id, rmep, VTY_NEWLINE);
		return CMD_WARNING;
	}

	psess->info.remote_mep = 0;

	return CMD_SUCCESS;
}

DEFUN(mplstp_oam_session_cc_enable,
	mplstp_oam_session_cc_enable_cmd,
	"cc enable (3 | 10 | 100 | 1000 | 10000 | 60000 | 600000)",
	"Continuity check\n"
	"Enable.\n"
	"3.3ms.\n"
	"10ms.\n"
	"100ms.\n"
	"1000ms.\n"
	"10s.\n"
	"1min.\n"
	"10min.\n")
{
	struct oam_session *psess = (struct oam_session *)vty->index;
	unsigned int interval = 0;
	int ret = 0;

	MPLSOAM_MPTYPE_MIP(vty, psess->info.mp_type);
    if((psess->info.type != OAM_TYPE_INVALID) && (psess->info.cc_enable == 1))
    {
       vty_warning_out(vty, "CC is already enable.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
	interval = atoi(argv[0]);
	if(interval != 3 && interval != 10 && interval != 100 && interval != 1000 
            && interval != 10000 && interval != 60000 && interval != 600000)
	{
		vty_error_out(vty, "Input illegal!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

    psess->info.cc_enable = 1;
	psess->info.cc_interval = interval;

    if(psess->info.type != OAM_TYPE_INVALID)
    {
        ret = ipc_send_msg_n2(&(psess->info), sizeof(struct oam_info), 1, MODULE_ID_HAL, MODULE_ID_MPLS,
                        IPC_TYPE_MPLSOAM, OAM_SUBTYPE_CC, IPC_OPCODE_ENABLE, psess->info.session_id);
        if(ret != ERRNO_SUCCESS)
        {
            psess->info.cc_enable = 0;
	        psess->info.cc_interval = 0;        
            vty_error_out(vty, "Ipc fail.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

	return CMD_SUCCESS;
}


DEFUN(no_mplstp_oam_session_cc_enable,
	no_mplstp_oam_session_cc_enable_cmd,
	"no cc enable",
	"Delete\n"
	"Continuity check\n"
	"Enable\n")
{
	struct oam_session *psess = (struct oam_session *)vty->index;
	unsigned int interval = 0;
	int ret = 0;

    MPLSOAM_MPTYPE_MIP(vty, psess->info.mp_type);
	if(psess->info.cc_enable == 0)
	{
		vty_error_out(vty, "CC is already disable!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

    if(psess->info.type != OAM_TYPE_INVALID)
    {   
        ret = ipc_send_msg_n2(&(psess->info), sizeof(struct oam_info), 1, MODULE_ID_HAL, MODULE_ID_MPLS,
                        IPC_TYPE_MPLSOAM, OAM_SUBTYPE_CC, IPC_OPCODE_DISABLE, psess->info.session_id);
        if(ret != ERRNO_SUCCESS)
        {
            vty_error_out(vty, "Ipc fail.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
        interval = psess->info.cc_interval;
    }
    psess->info.cc_enable = 0;
	psess->info.cc_interval = interval;

	return CMD_SUCCESS;
}


DEFUN(mplstp_oam_session_ais_enable,
	mplstp_oam_session_ais_enable_cmd,
	"ais enable level <0-7> interval (1s | 60s)",
	"Alarm indication signal\n"
	"Enable.\n"
	"Ais level\n"
	"Range of ais level:<0-7>\n"
	"Interval.\n"
	"Value of interval:1s\n"
	"Value of interval:60s\n")
{
	struct oam_session *psess = (struct oam_session *)vty->index;
    uint16_t interval = 0;
	uint16_t level = 0;

	MPLSOAM_MPTYPE_MIP(vty, psess->info.mp_type);
	if(psess->ais_enable == 1)
	{
		vty_error_out(vty, "Ais is already enable!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	level = atoi(argv[0]);
    if(!strncmp(argv[1], "1s", strlen(argv[1])))
    {
        interval = 1;
    }
    else if(!strncmp(argv[1], "60s", strlen(argv[1])))
    {
        interval = 60;
    }

    if(psess->info.type == OAM_TYPE_PW)
    {
        vty_error_out(vty, "Oam for pw is not support ais function.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    psess->ais_enable = 1;
    mplsoam_session_ais_enable(psess, level, interval);

	return CMD_SUCCESS;
}

DEFUN(no_mplstp_oam_session_ais_enable,
	no_mplstp_oam_session_ais_enable_cmd,
	"no ais enable",
	"Delete.\n"
	"Alarm indication signal.\n"
	"Enable.\n")
{
	struct oam_session *psess = (struct oam_session *)vty->index;
	
    MPLSOAM_MPTYPE_MIP(vty, psess->info.mp_type);
	if(psess->ais_enable == 0)
	{
		vty_error_out(vty, "Ais is already disable!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

    if(psess->info.type == OAM_TYPE_INVALID)
    {
        psess->ais_enable = 0;
    }
    else
    {
        mplsoam_session_ais_disable(psess);
    }
    
	return CMD_SUCCESS;
}

DEFUN(mplstp_oam_session_csf_enable,
	mplstp_oam_session_csf_enable_cmd,
	"csf enable level <0-7> interval (1s | 60s)",
	"Alarm indication signal\n"
	"Enable.\n"
	"Csf level\n"
	"Range of csf level:<0-7>\n"
	"Interval.\n"
	"Value of interval:1s\n"
	"Value of interval:60s\n")
{
	struct oam_session *psess = (struct oam_session *)vty->index;
	uint16_t interval = 0;
	uint16_t level = 0;
	int ret = 0;

	MPLSOAM_MPTYPE_MIP(vty, psess->info.mp_type);

	if(psess->info.type == OAM_TYPE_INTERFACE || psess->info.type == OAM_TYPE_LSP)
	{
		vty_error_out(vty, "just oam for pw is support CSF function.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	
	if(psess->csf_enable == 1)
	{
		vty_error_out(vty, "Csf is already enable!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	level = atoi(argv[0]);
	if(!strncmp(argv[1], "1s", strlen(argv[1])))
	{
		interval = 1;
	}
	else if(!strncmp(argv[1], "60s", strlen(argv[1])))
	{
		interval = 60;
	}

    psess->csf_enable = 1;
    ret = mplsoam_session_csf_enable(psess, level, interval);
	if(ERRNO_SUCCESS != ret)
	{
		psess->csf_enable = 0;
		vty_error_out(vty, "Csf configure wrong!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}

DEFUN(no_mplstp_oam_session_csf_enable,
	no_mplstp_oam_session_csf_enable_cmd,
	"no csf enable",
	"Delete.\n"
	"Alarm indication signal.\n"
	"Enable.\n")
{
	struct oam_session *psess = (struct oam_session *)vty->index;
	
	MPLSOAM_MPTYPE_MIP(vty, psess->info.mp_type);
	if(psess->csf_enable == 0)
	{
		vty_error_out(vty, "Ais is already disable!%s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	psess->csf_enable = 0;
	COM_THREAD_TIMER_OFF(psess->csf_send);
	if(NULL != psess->pcsf)
	{
		free(psess->pcsf);
		psess->pcsf = NULL;
	}
	
	return CMD_SUCCESS;
}

DEFUN(mplstp_oam_session_lock_enable,
     mplstp_oam_session_lock_enable_cmd,
     "lock enable level <0-7> interval (1s | 60s) ",
     "Lock\n"
     "Enable\n"
     "Lock level\n"
	 "Range of lock level:<0-7>\n"
	 "Interval.\n"
	 "Value of interval:1s\n"
	 "Value of interval:60s\n")
{
	struct oam_session *psess = (struct oam_session *)vty->index;
	uint16_t interval = 0;
	uint16_t level = 0;

	MPLSOAM_MPTYPE_MIP(vty, psess->info.mp_type);
	if(psess->lck_enable == 1)
	{
		vty_error_out(vty, "Lck is already enable!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	level = atoi(argv[0]);
    if(!strncmp(argv[1], "1s", strlen(argv[1])))
    {
        interval = 1;
    }
    else if(!strncmp(argv[1], "60s", strlen(argv[1])))
    {
        interval = 60;
    }

    if(psess->info.type == OAM_TYPE_PW)
    {
        vty_error_out(vty, "Oam for pw is not support lock enable.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    psess->lck_enable = 1;
    mplsoam_session_lck_enable(psess, level, interval);

	return CMD_SUCCESS;
}

DEFUN(no_mplstp_oam_session_lock_enable,
     no_mplstp_oam_session_lock_enable_cmd,
     "no lock enable",
     "Delete\n"
     "Lock\n"
     "Enable\n")
{
	struct oam_session *psess = (struct oam_session *)vty->index;
	
	MPLSOAM_MPTYPE_MIP(vty, psess->info.mp_type);
	if(psess->lck_enable == 0)
	{
		vty_error_out(vty, "Ais is already disable!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

    if(psess->info.type == OAM_TYPE_INVALID)
    {
        psess->lck_enable = 0;
    }
    else
    {
        mplsoam_session_lck_disable(psess);
    }
    
	return CMD_SUCCESS;
}


DEFUN(mplstp_oam_section_session_mac,
    mplstp_oam_section_session_mac_cmd,
    "section mac XX:XX:XX:XX:XX:XX [vlan <1-4094>]",
    "Section layer\n"
    "Destination mac address\n"
    "Mac format:XX:XX:XX:XX:XX:XX\n"
    "Vlan\n"
    "Range of vlan-id:<1-4094>\n")
{
    struct oam_session *psess = (struct oam_session *)vty->index;
    uchar mac[MAC_LEN] = {0};
    int ret = 0;
	
    MPLSOAM_MPTYPE_MIP(vty, psess->info.mp_type);
    MPLSOAM_SESSION_ALREADY_ENABLE(vty, psess->info.type);

    ret = ether_valid_mac((char *)argv[0]);
    if(ret != 0)
    {
        vty_error_out(vty, "Mac address is invalid.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ether_string_to_mac((char *)argv[0], mac);
    if((ether_is_broadcast_mac(mac)) && (mac[0] & 0x01))
    {
        vty_error_out(vty, "Can't set multicast mac.%s", VTY_NEWLINE );
        return CMD_WARNING;
    }
    
    memcpy((uchar *)(psess->info.mac), (uchar *)mac, MAC_LEN);

    if(argv[1] != NULL)
    {
        psess->info.vlan = atoi(argv[1]);
    }
    
    return CMD_SUCCESS;
}


DEFUN(no_mplstp_oam_section_session_mac,
    no_mplstp_oam_section_session_mac_cmd,
    "no section mac",
    "Delete\n"
    "Section layer\n"
    "Destination mac address\n")
{
    struct oam_session *psess = (struct oam_session *)vty->index;
	
    MPLSOAM_MPTYPE_MIP(vty, psess->info.mp_type);
    MPLSOAM_SESSION_ALREADY_ENABLE(vty, psess->info.type);

    memset(psess->info.mac, 0, MAC_LEN);
    psess->info.vlan = 0;

    return CMD_SUCCESS;
}

DEFUN(mplstp_oam_session_alarm_report_enable,
	mplstp_oam_session_alarm_report_enable_cmd,
	"alarm-report enable",
	"Alarm report\n"
	"Enable\n")
{
    struct oam_session *psess = (struct oam_session *)vty->index;

	MPLSOAM_MPTYPE_MIP(vty, psess->info.mp_type);
    if(psess->alarm_rpt == 1)
    {
        return CMD_WARNING;
    }
    
    psess->alarm_rpt = 1;
    mplsoam_session_alarm_rpt_enable(psess);
        
    return CMD_SUCCESS;
}


DEFUN(no_mplstp_oam_session_alarm_report_enable,
	no_mplstp_oam_session_alarm_report_enable_cmd,
	"no alarm-report enable",
	"Delete\n"
	"Alarm report\n")
{
    struct oam_session *psess = (struct oam_session *)vty->index;
	
    MPLSOAM_MPTYPE_MIP(vty, psess->info.mp_type);
    if(psess->alarm_rpt == 0)
    {
        return CMD_WARNING;
    }
    
    psess->alarm_rpt = 0;
    mplsoam_session_alarm_rpt_disable(psess);

    return CMD_SUCCESS;
}

DEFUN(mplstp_oam_session_global_channel_type,
	mplstp_oam_session_global_channel_type_cmd,
	"oam global channel-type (0x8902 | 0x7FFA)",
	"Oam.\n"
	"Global.\n"
	"Oam channel-type.\n"
	"0x8902\n"
	"0x7FFA\n")
{
	uint16_t global_chtype = 0;
	
	if(argv[0][2] == '8')
    {
        global_chtype = MPLS_CTLWORD_CHTYPE_8902;
    }
    else if(argv[0][2] == '7')
    {
        global_chtype = MPLS_CTLWORD_CHTYPE_7FFA;
    }
	
	if(gmplsoam.g_chtype_enable == 1)
	{	
		if(gmplsoam.global_chtype != global_chtype)
		{
			vty_error_out(vty, "Channel type:0x%x is Already enable.%s", gmplsoam.global_chtype, VTY_NEWLINE);
			return CMD_WARNING;
		}
		
		return CMD_SUCCESS;
	}
	else if(gmplsoam.sess_total != 0)
	{
		if(gmplsoam.global_chtype == MPLS_CTLWORD_CHTYPE_7FFA)
		{
			vty_warning_out(vty, "0x7ffa is already applied by oam session, can't enable.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		else if(gmplsoam.global_chtype == MPLS_CTLWORD_CHTYPE_8902)
		{
			vty_warning_out(vty, "0x8902 is already applied by oam session, can't enable.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}

	#if 0
	if(argv[0][2] == '8')
    {
        gmplsoam.global_chtype = MPLS_CTLWORD_CHTYPE_8902;
    }
    else if(argv[0][2] == '7')
    {
        gmplsoam.global_chtype = MPLS_CTLWORD_CHTYPE_7FFA;
    }
	#endif
	
	gmplsoam.global_chtype = global_chtype;
	gmplsoam.g_chtype_enable = 1;
	
    return CMD_SUCCESS;
}

DEFUN(no_mplstp_oam_session_global_channel_type,
	no_mplstp_oam_session_global_channel_type_cmd,
	"no oam global channel-type",
	"Delete.\n"
	"Oam.\n"
	"Global.\n"
	"Oam channel-type\n")
{	
	if(gmplsoam.g_chtype_enable == 1)
	{
		if(gmplsoam.sess_total != 0)
		{
			if(gmplsoam.global_chtype == MPLS_CTLWORD_CHTYPE_7FFA)
			{
				vty_warning_out(vty, "0x7ffa is already applied by oam session,can't delete global channel-type.%s", VTY_NEWLINE);
				return CMD_WARNING;
			}
			else if(gmplsoam.global_chtype == MPLS_CTLWORD_CHTYPE_8902)
			{
				vty_warning_out(vty, "0x8902 is already applied by oam session,can't delete global channel-type.%s", VTY_NEWLINE);
				return CMD_WARNING;
			}
		}
		
		gmplsoam.g_chtype_enable = 0;
	}

	gmplsoam.global_chtype = MPLS_CTLWORD_CHTYPE_8902;

	return CMD_SUCCESS;
}



DEFUN(mplstp_oam_session_channel_type,
	mplstp_oam_session_channel_type_cmd,
	"channel-type (0x8902 | 0x7FFA)",
	"Oam channel-type\n"
	"0x8902\n"
	"0x7FFA\n")
{
    struct oam_session *psess = (struct oam_session *)vty->index;
	uint16_t channel_type = 0;

	//MPLSOAM_MPTYPE_MIP(vty, psess->info.mp_type);
    MPLSOAM_SESSION_ALREADY_ENABLE(vty, psess->info.type);

	if(gmplsoam.g_chtype_enable == 1)
    {
    	vty_warning_out(vty, "Global channel-type is already config, can't config private channel-type.%s", VTY_NEWLINE);
		return CMD_WARNING;
    }

	#if 0
	if(gmplsoam.sess_total == 1)
	{
	    if(argv[0][2] == '8')
	    {
	        psess->info.channel_type = MPLS_CTLWORD_CHTYPE_8902;
	    }
	    else if(argv[0][2] == '7')
	    {
	        psess->info.channel_type = MPLS_CTLWORD_CHTYPE_7FFA;
	    }
		gmplsoam.global_chtype = psess->info.channel_type;
	}
	else
	{
		if(psess->info.channel_type == MPLS_CTLWORD_CHTYPE_7FFA)
		{
			vty_warning_out(vty, "0x7ffa is already applied by other oam session, can't modify.%s", VTY_NEWLINE);	
		}
		else if(psess->info.channel_type == MPLS_CTLWORD_CHTYPE_8902)
		{
			vty_warning_out(vty, "0x8902 is already applied by other oam session, can't modify.%s", VTY_NEWLINE);
		}
		return CMD_WARNING;
	}
	#endif
	
	if(argv[0][2] == '8')
    {
        channel_type = MPLS_CTLWORD_CHTYPE_8902;
    }
    else if(argv[0][2] == '7')
    {
        channel_type = MPLS_CTLWORD_CHTYPE_7FFA;
    }
	
	if(gmplsoam.sess_total == 1)
	{
		psess->info.channel_type = channel_type;
		gmplsoam.global_chtype = channel_type;
	}
	else
	{
		if(gmplsoam.global_chtype != channel_type)
		{
			if(gmplsoam.global_chtype == MPLS_CTLWORD_CHTYPE_8902)
			{
				vty_warning_out(vty, "0x7ffa is already applied by other oam session, can't modify.%s", VTY_NEWLINE);	
			}
			else if(gmplsoam.global_chtype == MPLS_CTLWORD_CHTYPE_7FFA)
			{
				vty_warning_out(vty, "0x8902 is already applied by other oam session, can't modify.%s", VTY_NEWLINE);
			}
		}
		return CMD_WARNING;
	}
	
    return CMD_SUCCESS;
}

DEFUN(no_mplstp_oam_session_channel_type,
	no_mplstp_oam_session_channel_type_cmd,
	"no channel-type",
	"Delete\n"
	"Oam channel-type\n")
{
    struct oam_session *psess = (struct oam_session *)vty->index;
	
    //MPLSOAM_MPTYPE_MIP(vty, psess->info.mp_type);
    MPLSOAM_SESSION_ALREADY_ENABLE(vty, psess->info.type);

	if(gmplsoam.g_chtype_enable == 1
		|| gmplsoam.sess_total > 1)
	{
		vty_warning_out(vty, "Global channel-type is already config or applied by other oam session, can't delete.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
		
    psess->info.channel_type = MPLS_CTLWORD_CHTYPE_8902;
	gmplsoam.global_chtype = psess->info.channel_type;

    return CMD_SUCCESS;
}


/* 接口下使能 mplstp oam */
DEFUN(mplstp_oam_section_session_enable,
	mplstp_oam_section_session_enable_cmd,
	"mplstp-oam enable session <1-65535>",
	"Mpls transport profile OAM\n"
	"Enable\n"
	"Session\n"
	"Range of session:<1-65535>\n")
{
	uint32_t ifindex = (uint32_t)vty->index;
	struct mpls_if *pif = NULL;
    struct oam_session *psess = NULL;
	uint16_t session_id = 0;
    int ret = 0;


    pif = mpls_if_get(ifindex);
    if (NULL == pif)
    {
        vty_error_out(vty, "Failed get interface !%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (pif->mode != IFNET_MODE_L3)
    {
        vty_error_out(vty, "Please select mode l3 !%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	/* 判断接口类型是否为以太物理接口*/
	if (!IFM_TYPE_IS_METHERNET(ifindex))
    {
        vty_error_out(vty, "This interface is not ethernet!%s", VTY_NEWLINE);
		return CMD_WARNING;
    }
	else
	{
		if (IFM_IS_SUBPORT (ifindex))
        { 
        	vty_error_out(vty, "This interface is not physical ethernet!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	
    session_id = atoi(argv[0]);
    psess = mplsoam_session_lookup(session_id);
    if(NULL == psess)
    {
        vty_error_out(vty, "Session:%d is not exist.%s", session_id, VTY_NEWLINE);
        return CMD_WARNING;
    }
	
	if(psess->info.mp_type == MP_TYPE_MIP)
	{
		vty_error_out(vty, "Mip node is not support this function.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
    if(!ether_is_zero_mac(psess->info.mac))
    {
        ether_string_to_mac((char *)"FF:FF:FF:FF:FF:FF", psess->info.mac);
    }
    
    /* 接口绑定指定session，并使能*/
    ret = mpls_if_bind_mplsoam_sess(pif, session_id);
    if(ret != ERRNO_SUCCESS)
    {   
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);
        return CMD_WARNING;
    }

	return CMD_SUCCESS;
}


DEFUN(no_mplstp_oam_section_session_enable,
	no_mplstp_oam_section_session_enable_cmd,
	"no mplstp-oam enable session",
	"Delete\n"
	"Mpls transport profile OAM\n"
	"Enable\n"
	"Session\n")
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct mpls_if *pif = NULL;
    int ret = 0;
	
    pif = mpls_if_lookup(ifindex);
    if(NULL == pif)
    {
        vty_error_out(vty, "Mpls interface has not create.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    /* 解绑并去使能*/
    ret = mpls_if_unbind_mplsoam_sess(pif);
    if(ret != ERRNO_SUCCESS)
    {
        vty_error_out(vty, "This interface isn't enable oam session.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    return CMD_SUCCESS;
}


DEFUN(mplstp_oam_lm_enable,
    mplstp_oam_lm_enable_cmd,
    "lm enable {interval <1-3600> | frequency <0-65535>}",
    "Loss Measurement\n"
    "Enable\n"
    "Interval of Loss Measurement Message\n"
    "Range of interval(s):<1-3600>\n"
    "Frequency\n"
    "Range of frequency(times)\n")
{
    struct oam_session *psess = (struct oam_session *)vty->index;
    uint16_t interval = OAM_LM_DEF_INTERVAL;
    uint16_t frequency = OAM_LM_DEF_FREQUENCY;

	if(psess->info.mp_type == MP_TYPE_MIP)
	{
		vty_error_out(vty, "Mip node is not support this function.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
    MPLSOAM_SESSION_IS_DISABLE(vty, psess->info.type);

    if(psess->info.lm_enable == OAM_STATUS_ENABLE)
    {
        vty_error_out(vty, "LM is running.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    if(argv[0] != NULL)
    {
        interval = atoi(argv[0]);        
    }
    if(argv[1] != NULL)
    {
        frequency = atoi(argv[1]);    
    }
    psess->info.lm_interval = interval;
    psess->info.lm_frequency = frequency;

    mplsoam_session_lm_enable(psess);
    
    return CMD_SUCCESS;
        
}

DEFUN(no_mplstp_oam_lm_enable,
    no_mplstp_oam_lm_enable_cmd,
    "no lm enable",
    "Delete\n"
    "Loss Measurement\n"
    "Enable\n")
{
    struct oam_session *psess = (struct oam_session *)vty->index;

	if(psess->info.mp_type == MP_TYPE_MIP)
	{
		vty_error_out(vty, "Mip node is not support this function.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
    if(psess->info.lm_enable != OAM_STATUS_ENABLE)
    {   
        vty_error_out(vty, "LM is not running.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    mplsoam_session_lm_disable(psess->info.session_id);
    
    return CMD_SUCCESS;
}


DEFUN(mplstp_oam_dm_enable,
    mplstp_oam_dm_enable_cmd,
    "dm enable {interval <1-3600> | frequency <0-65535> | exp <0-7> | tlv-size <1-400>}",
    "Delay Measurement\n"
    "Enable\n"
    "Interval of Delay Measurement Message\n"
    "Range of interval(s):<1-3600>\n"
    "Frequency\n"
    "Range of frequency(times)\n"
    "Priority of sending delay measurement packets, the default is 7\n"
    "Range of exp-value:<0-7>\n"
    "Tlv size of dmm\n"
    "Range of data-tlv size:<1-400>\n")
{
    struct oam_session *psess = (struct oam_session *)vty->index;
    uint16_t interval = OAM_DM_DEF_INTERVAL;
    uint16_t frequency = OAM_DM_DEF_FREQUENCY;
	uint8_t  dm_exp = 7;
	uint16_t dm_tlv_len = 0;

	if(psess->info.mp_type == MP_TYPE_MIP)
	{
		vty_error_out(vty, "Mip node is not support this function.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
    MPLSOAM_SESSION_IS_DISABLE(vty, psess->info.type);

    if(psess->info.dm_enable == OAM_STATUS_ENABLE)
    {
        vty_error_out(vty, "DM is running.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    if(argv[0] != NULL)
    {
        interval = atoi(argv[0]);        
    }
    if(argv[1] != NULL)
    {
        frequency = atoi(argv[1]);    
    }
	if(argv[2] != NULL)
	{
		dm_exp = atoi(argv[2]);
	}
	if(argv[3] != NULL)
	{
		dm_tlv_len = atoi(argv[3]);
	}
    psess->info.dm_interval = interval;
    psess->info.dm_frequency = frequency;
	psess->info.dm_exp = dm_exp;
	psess->info.dm_tlv_len = dm_tlv_len;

    mplsoam_session_dm_enable(psess);
    
    return CMD_SUCCESS;
}


DEFUN(no_mplstp_oam_dm_enable,
    no_mplstp_oam_dm_enable_cmd,
    "no dm enable",
    "Delete\n"
    "Delay Measurement\n"
    "Enable\n")
{
    struct oam_session *psess = (struct oam_session *)vty->index;
	
	if(psess->info.mp_type == MP_TYPE_MIP)
	{
		vty_error_out(vty, "Mip node is not support this function.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

    if(psess->info.dm_enable != OAM_STATUS_ENABLE)
    {   
        vty_error_out(vty, "DM is not running.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    mplsoam_session_dm_disable(psess->info.session_id);
    
    return CMD_SUCCESS;
}

DEFUN(mplstp_oam_lb_enable,
    mplstp_oam_lb_enable_cmd,
    "lb enable {mip-ttl <1-255> | node-id A.B.C.D | packet-size <64-1500> | request-tlv | exp <0-7>}",
    "Oam Loopback.\n"
    "Enable\n"
    "TTL values from local to mip.\n"
    "Range of ttl:<1-255>.\n"
    "The ID of node.\n"
    "Specify the ID of node.\n"
    "Size of an LBM packet.\n"
    "Range of packet-size:<64-1500>\n"
    "Optional requesting MEP ID TLV\n"
    "Exp\n"
    "Range of exp:<0-7>\n")
{
	struct oam_session *psess = (struct oam_session *)vty->index;
	uint16_t packet_size = 0;
	uint32_t node_id = 0;
	uint16_t ttl = 0;
	int ret = 0;

	if(psess->info.mp_type == MP_TYPE_MIP)
	{
		vty_error_out(vty, "Mip node is not support this function.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	MPLSOAM_SESSION_IS_DISABLE(vty, psess->info.type);
	
	if(psess->lt_enable == 1)
	{
		vty_error_out(vty, "LT is already enable,can't enable LB.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	psess->packet_size = 0;
	psess->data_tlv_len = 0;
	psess->data_tlv_flag = 0;
	psess->node_id = 0;
	psess->req_tlv_flag = 0;
	if(argv[0] != NULL)
	{
		ttl = atoi(argv[0]);
	}
	else if(argv[1] != NULL)
	{
		vty_error_out(vty, "Can't config Node-id .%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	if(argv[1] != NULL)
	{
		if(psess->info.type == OAM_TYPE_INTERFACE)
		{
			vty_error_out(vty, "Section layer can't config node-id.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		node_id = inet_strtoipv4((char *)argv[1]);
		
		ret = inet_valid_ipv4(node_id);
		if (ret != 1)
		{
			vty_error_out(vty, "Please input a valid address.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	else if(argv[0] != NULL && psess->info.type != OAM_TYPE_INTERFACE)
	{
		vty_error_out(vty, "Node id is not config.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	if(argv[2] != NULL)
	{
		packet_size = atoi(argv[2]);
	}
	else
	{
		packet_size = 41;
	}

	if(argv[3] != NULL)
	{
		if(argv[2] != NULL)
		{
			if(packet_size < 101)
			{
				vty_error_out(vty, "The specified packet size is too small.It must be greater than or equal to 101 bytes.%s", VTY_NEWLINE);
				return CMD_WARNING;
			}
			else if(packet_size > 101)
			{
				psess->data_tlv_flag = 1;
				psess->data_tlv_len = packet_size - 100;//data_len
			}
		}
		else
		{
			packet_size = 97;
		}
		psess->req_tlv_flag = 1;
	}
	else if(argv[2] != NULL)
	{
		psess->data_tlv_flag = 1;
		psess->data_tlv_len = packet_size - 44;
	}

	if(argv[4] != NULL)
	{
		psess->lb_exp = atoi(argv[4]);
	}
	else
	{
		psess->lb_exp = 7;
	}
	
	psess->pvty = vty;
	psess->lb_enable = 1;
	psess->type = OAM_PKT_LB;
	psess->lb_timer = 0;
	psess->packet_size = packet_size;

	if(ttl != 0)
	{
		psess->ttl = ttl;
		psess->node_id = node_id;
	}
	else
	{
		psess->ttl = 0;
		psess->ttl_curr = 0;
		psess->node_id = 0;
	}

	ret = mplsoam_send_lb(psess);
	if(ERRNO_SUCCESS == ret)
	{
		return CMD_WAIT;
	}
	else
	{
		vty_error_out(vty, "LB is send failed.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
}

#if 0
DEFUN(mplstp_oam_lb_enable,
    mplstp_oam_lb_enable_cmd,
    "lb (enable | disable)",
    "Oam Loopback.\n"
    "Enable.\n"
    "Disable.\n")
{
    struct oam_session *psess = (struct oam_session *)vty->index;
	int ret = 0;

	if(psess->info.mp_type == MP_TYPE_MIP)
	{
		vty_out(vty, "Error:Mip node is not support this function.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
    MPLSOAM_SESSION_IS_DISABLE(vty, psess->info.type);

    if(psess->lt_enable == 1)
    {
        vty_out(vty, "Warning:LT is already enable,can't enable LB.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
   
  	psess->pvty = vty;
   
    if(argv[0][0] == 'e')
    {
        if(psess->lb_enable == 1)
        {
            vty_out(vty, "Warning:LB is already enable.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
        psess->lb_enable = 1;
		psess->type = OAM_PKT_LB;
		psess->ttl = 0;
		psess->ttl_curr = 0;
		psess->lb_timer = NULL;
        ret = mplsoam_send_lb(psess);
		if(ERRNO_SUCCESS == ret)
		{
			return CMD_WAIT;
		}
		else
		{
			vty_out(vty, "Error:LB is send failed.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
    }
    else if(argv[0][0] == 'd')
    {
        psess->lb_enable = 0;
		
		return CMD_SUCCESS;
    }
    
}
#endif

DEFUN(mplstp_oam_lt_enable,
    mplstp_oam_lt_enable_cmd,
    "lt enable ttl <1-255> {packet-size <64-1500> | request-tlv | exp <0-7>}",
    "Oam link tracing.\n"
	"Enable\n"
	"Ttl\n"
	"Range of ttl:<1-255>\n"
	"Packet size\n"
	"Range of packet size:<64-1500>\n"
	"Request tlv\n"
	"Exp\n"
	"Range of exp:<0-7>\n")
{
	struct oam_session *psess = (struct oam_session *)vty->index;
	uint16_t packet_size = 0;
	uint16_t ttl = 0;
	int ret = 0;

	if(psess->info.mp_type == MP_TYPE_MIP)
	{
		vty_error_out(vty, "Mip node is not support this function.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	MPLSOAM_SESSION_IS_DISABLE(vty, psess->info.type);
	
	if(psess->lb_enable == 1)
	{
		vty_error_out(vty, "LB is already enable,can't enable LT.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	ttl = atoi(argv[0]);

	psess->packet_size = 0;
	psess->data_tlv_len = 0;
	psess->data_tlv_flag = 0;
	psess->node_id = 0;
	psess->req_tlv_flag = 0;
	psess->ttl = 0;
	psess->lb_exp = 0;
	psess->ttl_curr = 0;
	
	if(argv[1] != NULL)
	{
		packet_size = atoi(argv[1]);
	}
	else
	{
		packet_size = 41;
	}

	if(argv[2] != NULL)
	{
		if(argv[1] != NULL)
		{
			if(packet_size < 101)
			{
				vty_error_out(vty, "The specified packet size is too small.It must be greater than or equal to 101 bytes.%s", VTY_NEWLINE);
				return CMD_WARNING;
			}
			else if(packet_size > 101)
			{
				psess->data_tlv_flag = 1;
				psess->data_tlv_len = packet_size - 100;//data_len
			}
		}
		else
		{
			packet_size = 97;
		}
		psess->req_tlv_flag = 1;
	}
	else if(argv[1] != NULL)
	{
		psess->data_tlv_flag = 1;
		psess->data_tlv_len = packet_size - 44;
	}

	if(argv[3] != NULL)
	{
		psess->lb_exp = atoi(argv[3]);
	}
	else
	{
		psess->lb_exp = 7;
	}
	
	psess->pvty = vty;
	psess->lt_enable = 1;
	psess->type = OAM_PKT_LT;
	psess->lt_timer = 0;
	psess->packet_size = packet_size;

	if(ttl != 0)
	{
		psess->ttl = ttl;
		psess->ttl_curr = 1;
	}
	else
	{
		psess->ttl = 0;
		psess->ttl_curr = 0;

		vty_error_out(vty, "Please config ttl.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	ret = mplsoam_send_lt(psess);
	if(ERRNO_SUCCESS == ret)
	{
		return CMD_WAIT;
	}
	else
	{
		vty_error_out(vty, "LB is send failed.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
}


DEFUN(show_mplstp_oam_session,
	show_mplstp_oam_session_cmd,
	"show mplstp-oam session [<1-65535>]",
	"Display.\n"
	"Mpls transport profile OAM\n"
	"Session\n"
	"Range of session:<1-65535>\n")
{
	struct hash_bucket *bucket = NULL;
	struct oam_session *psess = NULL;
	uint16_t session_id = 0;
	int cursor = 0;
    int ret = 0;

	if(NULL == argv[0])//显示所有session信息
	{
	    if(vty->flush_cp == 0xffffffff)
        {
            return CMD_SUCCESS;
        }
        
	    ret = show_mplsoam_session_global(vty);
        if(ret == CMD_CONTINUE)
        {
            return ret;
        }
		HASH_BUCKET_LOOP(bucket, cursor, mplsoam_session_table)
		{
			psess = (struct oam_session *)bucket->data;
            if(NULL == psess)
            {
				continue;
            }
            ret = show_mplsoam_session_brief(vty, psess);
            if(ret == CMD_CONTINUE)
            {
                return ret;
            }    
			psess = NULL;
		}
        tpoam_line = 0;
	}
	else
	{
		session_id = atoi(argv[0]);
		psess = mplsoam_session_lookup(session_id);
		if(psess == NULL)
		{
			vty_error_out(vty, "Session:%d is not exist!%s", session_id, VTY_NEWLINE);
			return CMD_WARNING;
		}
		vty_out(vty, "===================================================%s", VTY_NEWLINE);
		show_mplsoam_session(vty, psess);
        vty_out(vty, "===================================================%s", VTY_NEWLINE);
	}
    
    return CMD_SUCCESS;
}


DEFUN(show_mplstp_oam_meg,
	show_mplstp_oam_meg_cmd,
	"show mplstp-oam meg [MEGNAME]",
	"Display.\n"
	"Mpls transport profile OAM\n"
	"Maintenance entity group\n"
	"String of meg name:<1-12>\n")
{
	struct hash_bucket *bucket = NULL;
	struct oam_meg     *meg    = NULL;
	char megname[NAME_STRING_LEN] = {0};
	int cursor = 0;

	if(NULL == argv[0])//显示所有session信息
	{
	#if 0	
	    if(vty->flush_cp == 0xffffffff)
        {
            return CMD_SUCCESS;
        }
	#endif
	
		HASH_BUCKET_LOOP(bucket, cursor, mplsoam_meg_table)
		{
			meg = (struct oam_meg *)bucket->data;
			show_mplsoam_meg_info(vty, meg);
			meg = NULL;
		}
    //    tpoam_line = 0;
	}
	else
	{
        
    	if(strlen(argv[0]) > 12)
    	{
    		vty_error_out(vty, "Length of meg name is less than 12 bytes.%s", VTY_NEWLINE);
    		return CMD_WARNING;
    	}

		memcpy(megname, argv[0], strlen(argv[0]));
		meg = mplsoam_meg_lookup((uchar *)argv[0]);
		if(meg == NULL)
		{
			vty_error_out(vty, "Meg:%s is not exist!%s", argv[0], VTY_NEWLINE);
			return CMD_WARNING;
		}
		show_mplsoam_meg_info(vty, meg);
	}
    
    return CMD_SUCCESS;

	
}




static int mplsoam_session_config_write(struct vty *vty)
{
	struct hash_bucket *bucket = NULL;
	struct oam_session *psess = NULL;
	int cursor = 0;
    
	HASH_BUCKET_LOOP(bucket, cursor, mplsoam_session_table)
	{   
		psess = (struct oam_session *)bucket->data;
		if(NULL == psess)
		{
			continue;
		}

		vty_out(vty, " mplstp-oam session %d%s", psess->info.session_id, VTY_NEWLINE);
		if(psess->meg != NULL)
		{
			vty_out(vty, "  service meg %s%s", psess->info.megname, VTY_NEWLINE);
		}
		if(psess->info.mp_id <= 8191)
		{
			if(psess->info.mp_type == MP_TYPE_MEP)
			{
				vty_out(vty, "  mep-id %d%s", psess->info.mp_id, VTY_NEWLINE);
			}
			else if(psess->info.mp_type == MP_TYPE_MIP)
			{
				vty_out(vty, "  mip-id %d%s", psess->info.mp_id, VTY_NEWLINE);
			}
		}
		
		if((psess->info.priority <= 7) && (psess->info.priority != 5))
		{
			vty_out(vty, "  priority %d%s", psess->info.priority, VTY_NEWLINE);
		}
		if(psess->info.remote_mep > 0)
		{
			vty_out(vty, "  remote-mep %d%s", psess->info.remote_mep, VTY_NEWLINE);
		}
		if(psess->rev_rmep != 0 && psess->fwd_rmep != 0)
		{
			vty_out(vty, "  mip fwd-rmep %d rev-rmep %d%s", psess->fwd_rmep, psess->rev_rmep, VTY_NEWLINE);
		}
		if(psess->info.channel_type == 0x7FFA)
    	{
    		vty_out(vty, "  channel-type 0x%X%s", psess->info.channel_type, VTY_NEWLINE);
    	}
		if(psess->fwd_lsp != NULL && psess->info.type == OAM_TYPE_LSP)
		{
			vty_out(vty, "  mip bind lsp %s fwd%s", psess->fwd_lsp->name, VTY_NEWLINE);
		}
		if(psess->rev_lsp != NULL && psess->info.type == OAM_TYPE_LSP)
		{
			vty_out(vty, "  mip bind lsp %s rev%s", psess->rev_lsp->name, VTY_NEWLINE);
		}
		if(psess->info.cc_enable == 1)
		{
			vty_out(vty, "  cc enable %d%s", psess->info.cc_interval, VTY_NEWLINE);	
		}
        
		if(psess->ais_enable == 1 && psess->pais != NULL)
		{
			vty_out(vty, "  ais enable level %d interval %ds%s", psess->pais->level, psess->pais->ais_interval, VTY_NEWLINE);
		}
		if(psess->lck_enable == 1 && psess->plck != NULL)
		{
			vty_out(vty, "  lock enable level %d interval %ds%s", psess->plck->level, psess->plck->lck_interval, VTY_NEWLINE);
		}
		if(psess->csf_enable == 1 && psess->pcsf != NULL)
		{
			vty_out(vty, "  csf enable level %d interval %ds%s", psess->pcsf->level, psess->pcsf->csf_interval, VTY_NEWLINE);
		}
        if(psess->alarm_rpt == 1)
        {
            vty_out(vty, "  alarm-report enable%s", VTY_NEWLINE);     
        }
        if(0 != ether_is_zero_mac(psess->info.mac))
        {
            if((0 == psess->info.vlan) && (0 != ether_is_broadcast_mac(psess->info.mac)))
            {
                vty_out(vty, "  section mac %02X:%02X:%02X:%02X:%02X:%02X%s", psess->info.mac[0],
                        psess->info.mac[1], psess->info.mac[2], psess->info.mac[3], psess->info.mac[4],
                            psess->info.mac[5], VTY_NEWLINE);    
            }
            else if(psess->info.vlan != 0)
            {
                vty_out(vty, "  section mac %02X:%02X:%02X:%02X:%02X:%02X vlan %d%s", psess->info.mac[0],
                        psess->info.mac[1],psess->info.mac[2], psess->info.mac[3], psess->info.mac[4],
                            psess->info.mac[5], psess->info.vlan, VTY_NEWLINE);
            }
        }
        /*  如果lm、dm测试连续发送，无时间限制，则保存lm、dm配置*/
        if((psess->info.lm_enable == 1) && (0 == psess->info.lm_frequency))
        {
            vty_out(vty, "  lm enable");
            if(psess->info.lm_interval != OAM_LM_DEF_INTERVAL)
            {
                vty_out(vty, " interval %d", psess->info.lm_interval);
            }
            if(psess->info.lm_frequency != OAM_LM_DEF_FREQUENCY)
            {
                vty_out(vty, " frequency %d", psess->info.lm_frequency);
            }
            vty_out(vty, "%s", VTY_NEWLINE);
        }
        if((psess->info.dm_enable == 1) && (0 == psess->info.dm_frequency))
        {
            vty_out(vty, "  dm enable");
            if(psess->info.dm_interval != OAM_LM_DEF_INTERVAL)
            {
                vty_out(vty, " interval %d", psess->info.dm_interval);
            }
            if(psess->info.dm_frequency != OAM_LM_DEF_FREQUENCY)
            {
                vty_out(vty, " frequency %d", psess->info.dm_frequency);
            }
            vty_out(vty, "%s", VTY_NEWLINE);
        }
	}
    return 0;
}

static int mplsoam_meg_config_write(struct vty *vty)
{
	struct hash_bucket *bucket = NULL;
	struct oam_meg     *meg    = NULL;
	int cursor = 0;

	HASH_BUCKET_LOOP(bucket, cursor, mplsoam_meg_table)
	{
		meg = (struct oam_meg *)bucket->data;
		if(NULL == meg)
			continue;

		vty_out(vty, " mplstp-oam meg %s%s", meg->name, VTY_NEWLINE);
		if(meg->level < 7)
		{
			vty_out(vty, "  level %d%s", meg->level, VTY_NEWLINE);
		}
	}

	return 0;
}

void mplstp_oam_cmd_init(void)
{
	install_node(&mplstp_oam_sess_node, mplsoam_session_config_write);
	install_default(MPLSTP_OAM_SESS_NODE);

	install_element(MPLS_NODE, &mplstp_oam_session_cmd, CMD_SYNC);
	install_element(MPLS_NODE, &no_mplstp_oam_session_cmd, CMD_SYNC);

	install_element(MPLS_NODE, &mplstp_oam_session_global_channel_type_cmd, CMD_SYNC);
	install_element(MPLS_NODE, &no_mplstp_oam_session_global_channel_type_cmd, CMD_SYNC);

	install_element(MPLS_NODE, &show_mplstp_oam_session_cmd, CMD_LOCAL);
    install_element(MPLSTP_OAM_SESS_NODE, &show_mplstp_oam_session_cmd, CMD_LOCAL);
	install_element(MPLSTP_OAM_SESS_NODE, &show_mplstp_oam_meg_cmd, CMD_LOCAL);

	install_element(MPLSTP_OAM_SESS_NODE, &mplstp_oam_mp_cmd, CMD_SYNC);
	install_element(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_mp_cmd, CMD_SYNC);

	install_element(MPLSTP_OAM_SESS_NODE, &mplstp_oam_service_meg_cmd, CMD_SYNC);
	install_element(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_service_meg_cmd, CMD_SYNC);

	install_element(MPLSTP_OAM_SESS_NODE, &mplstp_oam_session_priority_cmd, CMD_SYNC);
	install_element(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_session_priority_cmd, CMD_SYNC);
	install_element(MPLSTP_OAM_SESS_NODE, &mplstp_oam_session_rmep_cmd, CMD_SYNC);
	install_element(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_session_rmep_cmd, CMD_SYNC);
	install_element(MPLSTP_OAM_SESS_NODE, &mplstp_oam_session_cc_enable_cmd, CMD_SYNC);
	install_element(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_session_cc_enable_cmd, CMD_SYNC);
	install_element(MPLSTP_OAM_SESS_NODE, &mplstp_oam_session_ais_enable_cmd, CMD_SYNC);
	install_element(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_session_ais_enable_cmd, CMD_SYNC);
	install_element(MPLSTP_OAM_SESS_NODE, &mplstp_oam_session_csf_enable_cmd, CMD_SYNC);
	install_element(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_session_csf_enable_cmd, CMD_SYNC);

	install_element(MPLSTP_OAM_SESS_NODE, &mplstp_oam_session_lock_enable_cmd, CMD_SYNC);
	install_element(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_session_lock_enable_cmd, CMD_SYNC);

	install_element(MPLSTP_OAM_SESS_NODE, &mplstp_oam_section_session_mac_cmd, CMD_SYNC);
	install_element(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_section_session_mac_cmd, CMD_SYNC);
	install_element(MPLSTP_OAM_SESS_NODE, &mplstp_oam_session_alarm_report_enable_cmd, CMD_SYNC);    
	install_element(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_session_alarm_report_enable_cmd, CMD_SYNC);     
    install_element(MPLSTP_OAM_SESS_NODE, &mplstp_oam_session_channel_type_cmd, CMD_SYNC); 
    install_element(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_session_channel_type_cmd, CMD_SYNC);
	
	install_element(MPLSTP_OAM_SESS_NODE, &mplstp_oam_lm_enable_cmd, CMD_SYNC);
	install_element(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_lm_enable_cmd, CMD_SYNC);
    install_element(MPLSTP_OAM_SESS_NODE, &mplstp_oam_dm_enable_cmd, CMD_SYNC);
    install_element(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_dm_enable_cmd, CMD_SYNC);

    install_element(MPLSTP_OAM_SESS_NODE, &mplstp_oam_lb_enable_cmd, CMD_LOCAL);
    install_element(MPLSTP_OAM_SESS_NODE, &mplstp_oam_lt_enable_cmd, CMD_LOCAL);

	install_element(MPLSTP_OAM_SESS_NODE, &mplstp_oam_mip_rmep_config_cmd, CMD_SYNC);
	install_element(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_mip_rmep_config_cmd, CMD_SYNC);
	install_element(MPLSTP_OAM_SESS_NODE, &mplstp_oam_mip_bind_lsp_cmd, CMD_SYNC);
	install_element(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_mip_bind_lsp_cmd, CMD_SYNC);
	
	#if 0
	install_element(MPLSTP_OAM_SESS_NODE, &mplstp_oam_mip_bind_pw_cmd);
	install_element(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_mip_bind_pw_cmd);
	#endif

	install_element(PHYSICAL_IF_NODE, &mplstp_oam_section_session_enable_cmd, CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &no_mplstp_oam_section_session_enable_cmd, CMD_SYNC);
		
	/***************meg***************/
	install_node(&mplstp_oam_meg_node, mplsoam_meg_config_write);
	install_default(MPLSTP_OAM_MEG_NODE);
	install_element(MPLS_NODE, &mplstp_oam_meg_cmd, CMD_SYNC);
	install_element(MPLS_NODE, &no_mplstp_oam_meg_cmd, CMD_SYNC);
	install_element(MPLS_NODE, &show_mplstp_oam_meg_cmd, CMD_LOCAL);

	install_element(MPLSTP_OAM_MEG_NODE, &show_mplstp_oam_session_cmd, CMD_LOCAL);

	install_element(MPLSTP_OAM_MEG_NODE, &mplstp_oam_meg_level_cmd, CMD_SYNC);
	install_element(MPLSTP_OAM_MEG_NODE, &no_mplstp_oam_meg_level_cmd, CMD_SYNC);
	
}




