#include <lib/memory.h>
#include <lib/hash1.h>
#include <lib/index.h>
#include <lib/zassert.h>
#include <lib/log.h>
#include <lib/types.h>
#include <lib/module_id.h>
#include <lib/msg_ipc_n.h>
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
#include "mpls_oam_lck.h"
#include "../tunnel.h"


int mplsoam_lck_pdu_construct(struct oam_session *psess)
{
    struct oam_ais_pw pkt_common;
    uint32_t          gallabel = 13;

	OAM_DEBUG();
	
    if(psess == NULL)
    {
        return 1;
    }

	memset(&pkt_common, 0, sizeof(struct oam_ais_pw));
    pkt_common.oam_header.level = psess->plck->level;
    pkt_common.oam_header.version = 0;
    pkt_common.oam_header.opcode = CFM_OPCODE_LCK;
    if(psess->plck->lck_interval == 1)
    {
        pkt_common.oam_header.flag = 0x4;//00000100
    }
    else if(psess->plck->lck_interval == 60)
    {
        pkt_common.oam_header.flag = 0x6;//00000110
    }
    pkt_common.oam_header.first_tlv_offset = 0;
    pkt_common.end_tlv = 0;

    /* lsp */
    psess->plck->lck_lsp.oam_ach = htonl(0x10000000 + psess->info.channel_type);
    psess->plck->lck_lsp.gal_label.label1 = htons(gallabel >> 4);
    psess->plck->lck_lsp.gal_label.label0 = gallabel & 0xf;
    psess->plck->lck_lsp.gal_label.exp = 5;
    psess->plck->lck_lsp.gal_label.bos = 1;
    psess->plck->lck_lsp.gal_label.ttl = 1;

    memcpy(&(psess->plck->lck_lsp.ais_pdu), &pkt_common, sizeof(struct oam_ais_pw));

	/* pw */
    memcpy(&psess->plck->lck_pw, &pkt_common, sizeof(struct oam_ais_pw));
    

    return 0;
}


int mplsoam_lck_start(struct oam_session *psess)
{  
	OAM_DEBUG();

    if(NULL == psess)
    {
        return 1;
    }

	COM_THREAD_TIMER_OFF(psess->lck_send);
	mplsoam_lck_send((void *)psess);

    return 0;
}

int mplsoam_lck_send(void *arg)
{
    struct oam_session *psess = NULL;
	struct oam_session *cli_sess = NULL;
    struct mpls_control mplscb;
    struct listnode    *node  = NULL;
    int                 ret   = 0;

	OAM_DEBUG();

    psess = (struct oam_session *)arg;
    memset(&mplscb, 0, sizeof(struct mpls_control));

    if(0 == psess->lck_enable)
    {
        return ERRNO_FAIL;    
    }
	
    if(psess->info.type == OAM_TYPE_INTERFACE)
    {
        ret = mplsoam_lck_if_get_clients_info(psess, psess->info.index);
        if(ret)
        {
        	
			OAM_DEBUG("Fail to get client layer.\n");
            goto next;
        }

        for(ALL_LIST_ELEMENTS_RO(psess->plck->lck_transport, node, cli_sess))
        {
        	if(NULL == cli_sess)
        	{
        		continue;
        	}
			
            memset(&mplscb, 0, sizeof(struct mpls_control));
            mplscb.label_num = 1;
            mplscb.is_changed = 1;
            mplscb.if_type = PKT_INIF_TYPE_LSP;
            mplscb.ttl = 255;
            mplscb.exp = 5;
			
			/* forward direction lsp */
			if(cli_sess->fwd_lsp != NULL)
			{
				mplscb.ifindex = cli_sess->fwd_lsp->lsp_index;
				pkt_send(PKT_TYPE_MPLS, (union pkt_control *)(&mplscb), &(psess->plck->lck_lsp), sizeof(struct oam_ais_lsp));
			}
			/* reverse direction lsp */
			if(cli_sess->rev_lsp != NULL)
			{
				mplscb.ifindex = cli_sess->rev_lsp->lsp_index;
				pkt_send(PKT_TYPE_MPLS, (union pkt_control *)(&mplscb), &(psess->plck->lck_lsp), sizeof(struct oam_ais_lsp));
			}
        }
    }
    else if(psess->info.type == OAM_TYPE_LSP)
    {
        ret = mplsoam_lck_lsp_get_clients_info(psess, psess->info.index);
        if(ret)
        {
        	
			OAM_DEBUG("Fail to get client layer.\n");
            goto next;
        }
        
        for(ALL_LIST_ELEMENTS_RO(psess->plck->lck_transport, node, cli_sess))
        {
        	if(NULL == cli_sess)
        	{
        		continue;
        	}
				
            memset(&mplscb, 0, sizeof(struct mpls_control));
            mplscb.chtype = psess->info.channel_type;
            //mplscb.label_num = 2;
            mplscb.is_changed = 1;
            mplscb.if_type = PKT_INIF_TYPE_PW;
            mplscb.ttl = 255;
            mplscb.exp = 5;

			if(cli_sess->sw_pw != NULL)
 			{
 				OAM_DEBUG("lck send:fw_pwname:%s, pwoutlabel:%d.\n", cli_sess->sw_pw->name, cli_sess->sw_pw->outlabel);
 				mplscb.ifindex = cli_sess->sw_pw->pwinfo.pwindex;
            	pkt_send(PKT_TYPE_MPLS, (union pkt_control *)(&mplscb), &(psess->plck->lck_pw), sizeof(struct oam_ais_pw));
 			}
			if(cli_sess->sw_pw != NULL && cli_sess->sw_pw->pswitch_pw != NULL)
			{
				OAM_DEBUG("lck send:fw_pwname:%s, pwoutlabel:%d.\n", cli_sess->sw_pw->pswitch_pw->name, cli_sess->sw_pw->pswitch_pw->outlabel);

				mplscb.ifindex = cli_sess->sw_pw->pswitch_pw->pwinfo.pwindex;
            	pkt_send(PKT_TYPE_MPLS, (union pkt_control *)(&mplscb), &(psess->plck->lck_pw), sizeof(struct oam_ais_pw));
			}
        }
    }

next:
	COM_THREAD_TIMER_OFF(psess->lck_send);
	psess->lck_send = COM_THREAD_TIMER_ADD("mplsoam_lck_timer", LIB_TIMER_TYPE_LOOP, mplsoam_lck_send, (void *)psess, psess->plck->lck_interval*1000);

    return ERRNO_SUCCESS;
}


int mplsoam_lck_rcv_pkt(struct pkt_buffer *pkt)
{
    struct oam_session *psess    = NULL;
    struct l2vc_entry  *pl2vc    = NULL;
    struct lsp_entry   *egr_lsp  = NULL;
	struct static_lsp  *in_lsp   = NULL;
	struct pw_info     *ppw      = NULL;
    struct oam_ais_pw  *lck_pkt  = NULL;
	struct tunnel_if   *tun_if   = NULL;
    uint32_t            index    = 0;
    int                 interval = 0;//ms

	OAM_DEBUG();
	
    if(NULL == pkt)
    {
        return ERRNO_FAIL;
    }

    index = pkt->in_ifindex;

	lck_pkt = (struct oam_ais_pw *)pkt->data;
	if((lck_pkt->oam_header.flag & 0x7) == 0x4)
    {
        interval = 1 * 1000;
    }
    else if((lck_pkt->oam_header.flag & 0x7) == 0x6)
    {
        interval = 60 * 1000;
    }

	interval = interval / 10 * 35;
	
    if(pkt->inif_type == PKT_INIF_TYPE_LSP)
    {
        egr_lsp = mpls_lsp_lookup(index);
        if(NULL == egr_lsp)
        {
        	OAM_ERROR("Egress lsp of rcv lck is not exist.\n");
            return ERRNO_FAIL;
        }

		tun_if = tunnel_if_lookup(egr_lsp->group_index);
		if(NULL == tun_if)
		{
			OAM_DEBUG("Can't find tunnel.\n");
			return ERRNO_FAIL;
		}

		if(tun_if->p_mplstp)
		{
			in_lsp = tun_if->p_mplstp->ingress_lsp;
			if(NULL == in_lsp)
			{
				return ERRNO_FAIL;
			}
		}
		else
		{
			return ERRNO_FAIL;
		}
		
        psess = mplsoam_session_lookup(in_lsp->mplsoam_id);
        if(NULL == psess)
        {
        	OAM_ERROR("oam_id:%d is not bind lsp:%s.\n", in_lsp->mplsoam_id, in_lsp->name);
            return ERRNO_FAIL;
        }

        if(lck_pkt->oam_header.level != psess->info.level)
        {
        	OAM_ERROR("level err.lck_level:%d,local_level:%d.\n", lck_pkt->oam_header.level, psess->info.level);
            return ERRNO_FAIL;
        }
    }
    else if(pkt->inif_type == PKT_INIF_TYPE_PW)
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

        psess = mplsoam_session_lookup(pl2vc->mplsoam_id);
        if(NULL == psess)
        {
            return ERRNO_FAIL;
        }

        if(lck_pkt->oam_header.level != psess->info.level)
        {
        	OAM_ERROR("level err.lck_level:%d,local_level:%d.\n", lck_pkt->oam_header.level, psess->info.level);
            return ERRNO_FAIL;
        }
    }
	else
	{	
		OAM_ERROR("rcv err, not lsp or pw.\n");
		return ERRNO_FAIL;
	}
	
    if(0 == psess->alarm.alarm_lck)
    {
        mplsoam_session_alarm_process(psess->info.session_id, IPC_OPCODE_ADD, OAM_ALARM_LCK);
    }

	COM_THREAD_TIMER_OFF(psess->lck_rcv);
	psess->lck_rcv = COM_THREAD_TIMER_ADD("mplsoam_lck_clear_timer", LIB_TIMER_TYPE_LOOP, mplsoam_lck_clear, (void *)psess, interval);	
	
    return ERRNO_SUCCESS;
}

int mplsoam_lck_clear(void *arg)
{
    struct oam_session *psess = NULL;

	OAM_DEBUG();

    psess = (struct oam_session *)arg;

	if(NULL == psess)
	{
		return ERRNO_FAIL;
	}
	
    if(psess->alarm.alarm_lck == 1)
    {
        mplsoam_session_alarm_process(psess->info.session_id, IPC_OPCODE_CLEAR, OAM_ALARM_LCK);
		COM_THREAD_TIMER_OFF(psess->lck_rcv);
    }

    return 0;
}

void mplsoam_lck_register(void)
{
    union proto_reg proto;

    memset(&proto, 0, sizeof(union proto_reg));
    proto.mplsreg.if_type       = PKT_INIF_TYPE_PW;
    proto.mplsreg.oam_opcode    = CFM_OPCODE_LCK;
	proto.mplsreg.inlabel_value = 13;
    proto.mplsreg.chtype        = MPLS_CTLWORD_CHTYPE_8902;
    pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);

    memset(&proto, 0, sizeof(union proto_reg));
    proto.mplsreg.if_type       = PKT_INIF_TYPE_PW;
    proto.mplsreg.oam_opcode    = CFM_OPCODE_LCK;
	proto.mplsreg.inlabel_value = 13;
    proto.mplsreg.chtype        = 0x7ffa;
    pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);

    memset(&proto, 0, sizeof(union proto_reg));
    proto.mplsreg.if_type    = PKT_INIF_TYPE_PW;
    proto.mplsreg.oam_opcode = CFM_OPCODE_LCK;
    proto.mplsreg.chtype     = MPLS_CTLWORD_CHTYPE_8902;
    pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);

    memset(&proto, 0, sizeof(union proto_reg));
    proto.mplsreg.if_type    = PKT_INIF_TYPE_PW;
    proto.mplsreg.oam_opcode = CFM_OPCODE_LCK;
    proto.mplsreg.chtype     = 0x7ffa;
    pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);

    return ;
}



int mplsoam_lck_if_get_clients_info(struct oam_session *psess, uint32_t ifindex)
{
    struct hash_bucket *pbucket  = NULL;
	struct oam_session *ppsess    = NULL;
    int                 cursor   = 0;
	OAM_DEBUG();

    if(NULL == psess || NULL == psess->plck)
    {
        return ERRNO_FAIL;
    }

    if(NULL == psess->plck->lck_transport)
    {
        psess->plck->lck_transport = list_new();
    }
	else if(!list_isempty(psess->plck->lck_transport))
	{
		list_delete_all_node(psess->plck->lck_transport);
	}

    HASH_BUCKET_LOOP(pbucket, cursor, mplsoam_session_table)
    {
        ppsess = (struct oam_session *)pbucket->data;
        if(NULL == ppsess || ppsess->info.mp_type != MP_TYPE_MIP
			|| ppsess->info.type != OAM_TYPE_LSP
				|| ppsess->info.level != psess->plck->level)
        {
				OAM_DEBUG("get clients.mp_type=%d,oam type:%d,oam level:%d,lck level:%d.\n",  ppsess->info.mp_type, ppsess->info.type, ppsess->info.level, psess->plck->level);
            continue;
        }

		if(ppsess->fwd_lsp != NULL && (ppsess->fwd_lsp->oam_nhp_index == ifindex || _SHR_PORT_PARENT_IFINDEX_GET(ppsess->fwd_lsp->oam_nhp_index) == ifindex) )
        {
            	
				OAM_DEBUG("lck rcv:rev_lsp-name:%s.\n", ppsess->rev_lsp->name);
            listnode_add(psess->plck->lck_transport, ppsess);
        }
		else if(ppsess->rev_lsp != NULL && (ppsess->rev_lsp->oam_nhp_index == ifindex || _SHR_PORT_PARENT_IFINDEX_GET(ppsess->rev_lsp->oam_nhp_index) == ifindex) )
        {
            	
				OAM_DEBUG("lck rcv:fwd_lsp-name:%s.\n", ppsess->fwd_lsp->name);
            listnode_add(psess->plck->lck_transport, ppsess);
        }
    }

    return 0;
}


int mplsoam_lck_lsp_get_clients_info(struct oam_session *psess, uint32_t lspindex)
{
    struct hash_bucket *pbucket   = NULL;
	struct oam_session *ppsess     = NULL;
    int                 cursor    = 0;
    struct tunnel_if   *ptunnel1   = NULL;
	struct tunnel_if   *ptunnel2   = NULL;

	OAM_DEBUG();

    if(NULL == psess || NULL == psess->plck)
    {
        return 1;
    }
	
    if(NULL == psess->plck->lck_transport)
    {
        psess->plck->lck_transport = list_new();
    }
	else if(!list_isempty(psess->plck->lck_transport))
	{
		list_delete_all_node(psess->plck->lck_transport);
	}
	
    HASH_BUCKET_LOOP(pbucket, cursor, mplsoam_session_table)
    {
        ppsess = (struct oam_session *)pbucket->data;
        if(NULL == ppsess || ppsess->info.mp_type != MP_TYPE_MIP
			|| ppsess->info.type != OAM_TYPE_PW || NULL == ppsess->sw_pw
				|| ppsess->info.level != psess->plck->level)
        {
        	continue;
        }

		if(ppsess->sw_pw != NULL)
		{
	        ptunnel1 = tunnel_if_lookup(ppsess->sw_pw->tunl_index);
			if(ppsess->sw_pw->pswitch_pw != NULL)
			{
				ptunnel2 = tunnel_if_lookup(ppsess->sw_pw->pswitch_pw->tunl_index);
			}
			
	        if(NULL == ptunnel1 && NULL == ptunnel2)
	            continue;

			if((ptunnel1 != NULL && (ptunnel1->tunnel.master_index == lspindex
	            || ptunnel1->tunnel.backup_index == lspindex))
	            	|| (ptunnel2 != NULL && (ptunnel2->tunnel.master_index == lspindex
	            		|| ptunnel2->tunnel.backup_index == lspindex)))
	        {
	            listnode_add(psess->plck->lck_transport, ppsess);
	        } 
		}
	}

    return 0;
}


