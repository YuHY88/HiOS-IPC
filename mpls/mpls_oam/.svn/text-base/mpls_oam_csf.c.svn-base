#include <lib/memory.h>
#include <lib/hash1.h>
#include <lib/index.h>
#include <lib/zassert.h>
#include <lib/log.h>
#include <lib/types.h>
#include <lib/module_id.h>
#include <lib/msg_ipc.h>
#include <lib/errcode.h>
#include <lib/command.h>
#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>
#include <ftm/pkt_mpls.h>
#include <stdlib.h>
#include <lib/alarm.h>

#include "mpls_main.h"
#include "mpls_if.h"
#include "lsp_static.h"
#include "lspm.h"
#include "pw.h"
#include "mpls_oam.h"
#include "mpls_oam_csf.h"
#include "mpls_oam_ais.h"
#include "../tunnel.h"


int mplsoam_csf_pdu_construct(struct oam_session *psess)
{
    struct oam_ais_pw pkt_common;
    
    if(psess == NULL)
    {
        return ERRNO_FAIL;
    }

	if(psess->pcsf == NULL)
	{
		return ERRNO_FAIL;
	}

	memset(&pkt_common, 0, sizeof(struct oam_ais_pw));
    pkt_common.oam_header.level = psess->pcsf->level;
    pkt_common.oam_header.version = 0;
    pkt_common.oam_header.opcode = CFM_OPCODE_CSF;
    if(psess->pcsf->csf_interval == 1)
    {
        pkt_common.oam_header.flag = 0x4;//00000100
    }
    else if(psess->pcsf->csf_interval == 60)
    {
        pkt_common.oam_header.flag = 0x6;//00000110
    }
    pkt_common.oam_header.first_tlv_offset = 0;
    pkt_common.end_tlv = 0;

	/* pw */
    memcpy(&psess->pcsf->csf_pw, &pkt_common, sizeof(struct oam_ais_pw));

    return ERRNO_SUCCESS;
}


int mplsoam_csf_start(struct oam_session *psess)
{   
    if(NULL == psess)
    {
        return 1;
    }

#if 0
	THREAD_OFF(psess->csf_send);
    psess->csf_send = thread_add_timer(mpls_master, mplsoam_csf_send, psess, 0);
#endif

	if(psess->csf_send == 0)
	{
		psess->csf_send = COM_THREAD_TIMER_ADD("mplsoam_csf_snd_timer", LIB_TIMER_TYPE_LOOP, mplsoam_csf_send, (void *)psess, psess->pcsf->csf_interval);
	}

    return 0;
}

#if 0
int mplsoam_csf_send(struct thread *thread)
{
    struct oam_session *psess = NULL;
	struct pw_info *ppwinfo = NULL;
	struct l2vc_entry *pl2vc = NULL;
    struct mpls_control mplscb;

    psess = THREAD_ARG(thread);
    memset(&mplscb, 0, sizeof(struct mpls_control));

    if(0 == psess->csf_enable)
    {
    	THREAD_OFF(psess->csf_send);
        return ERRNO_SUCCESS;    
    }

	if(NULL == psess->pcsf)
	{
    	THREAD_OFF(psess->csf_send);
		return ERRNO_FAIL;
	}

	if(OAM_TYPE_PW != psess->info.type)
	{
    	THREAD_OFF(psess->csf_send);
		return ERRNO_FAIL;
	}
	
	ppwinfo = pw_lookup(psess->info.index);
	if(NULL == ppwinfo)
	{
    	THREAD_OFF(psess->csf_send);
		return ERRNO_FAIL;	
	}
	
	pl2vc = pw_get_l2vc(ppwinfo);
	if(NULL == pl2vc)
	{
    	THREAD_OFF(psess->csf_send);
		return ERRNO_FAIL;
	}

	if(LINK_DOWN == pl2vc->ac_flag)
	{
		memset(&mplscb, 0, sizeof(struct mpls_control));
		mplscb.chtype = psess->info.channel_type;
		mplscb.is_changed = 1;
		mplscb.if_type = PKT_INIF_TYPE_PW;
		mplscb.ttl = 255;
		mplscb.exp = 5;
		mplscb.ifindex = psess->info.index;
		
		pkt_send(PKT_TYPE_MPLS, (union pkt_control *)(&mplscb), &(psess->pcsf->csf_pw), sizeof(struct oam_ais_pw));
	}
	else
	{
		THREAD_OFF(psess->csf_send);
		return ERRNO_SUCCESS;
	}

    psess->csf_send = thread_add_timer(mpls_master, mplsoam_csf_send, psess, psess->pcsf->csf_interval);

    return ERRNO_SUCCESS;
}
#endif

int mplsoam_csf_send(void *arg)
{
    struct oam_session *psess = NULL;
	struct pw_info *ppwinfo = NULL;
	struct l2vc_entry *pl2vc = NULL;
    struct mpls_control mplscb;

    psess = (struct oam_session *)arg;
    memset(&mplscb, 0, sizeof(struct mpls_control));

    if(0 == psess->csf_enable)
    {
    	COM_THREAD_TIMER_OFF(psess->csf_send);
        return ERRNO_SUCCESS;    
    }

	if(NULL == psess->pcsf)
	{
    	COM_THREAD_TIMER_OFF(psess->csf_send);
		return ERRNO_FAIL;
	}

	if(OAM_TYPE_PW != psess->info.type)
	{
    	COM_THREAD_TIMER_OFF(psess->csf_send);
		return ERRNO_FAIL;
	}
	
	ppwinfo = pw_lookup(psess->info.index);
	if(NULL == ppwinfo)
	{
    	COM_THREAD_TIMER_OFF(psess->csf_send);
		return ERRNO_FAIL;	
	}
	
	pl2vc = pw_get_l2vc(ppwinfo);
	if(NULL == pl2vc)
	{
    	COM_THREAD_TIMER_OFF(psess->csf_send);
		return ERRNO_FAIL;
	}

	if(LINK_DOWN == pl2vc->ac_flag)
	{
		memset(&mplscb, 0, sizeof(struct mpls_control));
		mplscb.chtype = psess->info.channel_type;
		mplscb.is_changed = 1;
		mplscb.if_type = PKT_INIF_TYPE_PW;
		mplscb.ttl = 255;
		mplscb.exp = 5;
		mplscb.ifindex = psess->info.index;
		
		pkt_send(PKT_TYPE_MPLS, (union pkt_control *)(&mplscb), &(psess->pcsf->csf_pw), sizeof(struct oam_ais_pw));
	}
	else
	{
		COM_THREAD_TIMER_OFF(psess->csf_send);
		return ERRNO_SUCCESS;
	}

    return ERRNO_SUCCESS;
}


int mplsoam_csf_rcv_pkt(struct pkt_buffer *pkt)
{
    struct oam_session *psess    = NULL;
    struct l2vc_entry  *pl2vc    = NULL;
	struct pw_info     *ppw      = NULL;
    struct oam_ais_pw  *csf_pkt  = NULL;
    uint32_t            index    = 0;
    int                 interval = 0;//ms
    
    if(NULL == pkt)
    {
        return ERRNO_FAIL;
    }

    index = pkt->in_ifindex;

	csf_pkt = (struct oam_ais_pw *)pkt->data;
	if((csf_pkt->oam_header.flag & 0x7) == 0x4)
    {
        interval = 1 * 1000;
    }
    else if((csf_pkt->oam_header.flag & 0x7) == 0x6)
    {
        interval = 60 * 1000;
    }

	interval = interval / 10 * 35;
	
    if(pkt->inif_type == PKT_INIF_TYPE_PW)
    {
        ppw = pw_lookup(index);
        if(NULL == ppw)
        {
            return ERRNO_FAIL;
        }
        pl2vc = pw_get_l2vc(ppw);
        if(NULL == pl2vc)
        {
            return ERRNO_FAIL;
        }
    }
	else
	{	
		zlog_err("%s[%s]:rcv err, not pw.--line:%d", __FILE__, __FUNCTION__, __LINE__);
		return ERRNO_FAIL;
	}

	psess = mplsoam_session_lookup(pl2vc->mplsoam_id);
	if(NULL == psess)
	{
		return ERRNO_FAIL;
	}
	
	if(csf_pkt->oam_header.level != psess->info.level)
	{
		return ERRNO_FAIL;
	}
	
    if(psess->alarm.alarm_csf == 0)
    {
        mplsoam_session_alarm_process(psess->info.session_id, IPC_OPCODE_ADD, OAM_ALARM_CSF);
    }

#if 0
	THREAD_OFF(psess->csf_rcv);
	psess->csf_rcv = thread_add_timer_msec(mpls_master, mplsoam_csf_clear, psess, interval);	
#endif

	if(psess->csf_rcv == 0)
	{
		psess->csf_rcv = COM_THREAD_TIMER_ADD("mplsoam_csf_rcv_timer", LIB_TIMER_TYPE_LOOP, mplsoam_csf_clear, (void *)psess, interval);
	}

    return ERRNO_SUCCESS;
}

int mplsoam_csf_clear(void *arg)
{
    struct oam_session *psess = NULL;

    psess = (struct oam_session *)arg;
    if(NULL == psess)
    {
		return ERRNO_FAIL;
	}
	
    if(psess->alarm.alarm_csf == 1)
    {
        mplsoam_session_alarm_process(psess->info.session_id, IPC_OPCODE_CLEAR, OAM_ALARM_CSF);
		COM_THREAD_TIMER_OFF(psess->csf_rcv);
    }

    return 0;
}

void mplsoam_csf_register(void)
{
    union proto_reg proto;

    memset(&proto, 0, sizeof(union proto_reg));
    proto.mplsreg.if_type       = PKT_INIF_TYPE_PW;
    proto.mplsreg.oam_opcode    = CFM_OPCODE_CSF;
	proto.mplsreg.inlabel_value = 13;
    proto.mplsreg.chtype        = MPLS_CTLWORD_CHTYPE_8902;
    pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);

    memset(&proto, 0, sizeof(union proto_reg));
    proto.mplsreg.if_type       = PKT_INIF_TYPE_PW;
    proto.mplsreg.oam_opcode    = CFM_OPCODE_CSF;
	proto.mplsreg.inlabel_value = 13;
    proto.mplsreg.chtype        = 0x7ffa;
    pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);

    memset(&proto, 0, sizeof(union proto_reg));
    proto.mplsreg.if_type    = PKT_INIF_TYPE_PW;
    proto.mplsreg.oam_opcode = CFM_OPCODE_CSF;
    proto.mplsreg.chtype     = MPLS_CTLWORD_CHTYPE_8902;
    pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);

    memset(&proto, 0, sizeof(union proto_reg));
    proto.mplsreg.if_type    = PKT_INIF_TYPE_PW;
    proto.mplsreg.oam_opcode = CFM_OPCODE_CSF;
    proto.mplsreg.chtype     = 0x7ffa;
    pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);

    return ;
}

