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
#include <ftm/pkt_mpls.h>
#include <stdlib.h>
#include <lib/alarm.h>

#include "mpls_main.h"
#include "mpls_if.h"
#include "lsp_static.h"
#include "lspm.h"
#include "pw.h"
#include "mpls_oam.h"
#include "mpls_oam_ais.h"
#include "../tunnel.h"


int mplsoam_ais_pdu_construct(struct oam_session *psess)
{
    struct oam_ais_pw pkt_common;
    uint32_t          gallabel = 13;

	OAM_DEBUG();

    if(psess == NULL)
    {
        return 1;
    }

    /*pw ais 和lsp ais报文公共部分*/
	memset(&pkt_common, 0, sizeof(struct oam_ais_pw));
    pkt_common.oam_header.level = psess->pais->level;
    pkt_common.oam_header.version = 0;
    pkt_common.oam_header.opcode = CFM_OPCODE_AIS;
    if(psess->pais->ais_interval == 1)
    {
        pkt_common.oam_header.flag = 0x4;//00000100
    }
    else if(psess->pais->ais_interval == 60)
    {
        pkt_common.oam_header.flag = 0x6;//00000110
    }
    pkt_common.oam_header.first_tlv_offset = 0;
    pkt_common.end_tlv = 0;

    /* lsp */	
    psess->pais->ais_lsp.oam_ach = htonl(0x10000000 + psess->info.channel_type);
    psess->pais->ais_lsp.gal_label.label1 = htons(gallabel >> 4);
    psess->pais->ais_lsp.gal_label.label0 = gallabel & 0xf;
    psess->pais->ais_lsp.gal_label.exp = 5;
    psess->pais->ais_lsp.gal_label.bos = 1;
    psess->pais->ais_lsp.gal_label.ttl = 1;

    memcpy(&(psess->pais->ais_lsp.ais_pdu), &pkt_common, sizeof(struct oam_ais_pw));

	/* pw */
	memcpy(&(psess->pais->ais_pw), &pkt_common, sizeof(struct oam_ais_pw));

    return 0;
}

int mplsoam_ais_start(struct oam_session *psess)
{ 
	OAM_DEBUG();

    if(NULL == psess)
    {
        return 1;
    }

	COM_THREAD_TIMER_OFF(psess->ais_send);
	mplsoam_ais_send((void *)psess);

    return 0;
}

int mplsoam_ais_send(void *arg)
{
    struct oam_session *psess = NULL;
    struct static_lsp  *plsp  = NULL;
    struct l2vc_entry  *pl2vc = NULL;
    struct mpls_control mplscb;
    struct listnode    *node  = NULL;
    int                 ret   = 0;

	OAM_DEBUG();

    psess = (struct oam_session *)arg;
    memset(&mplscb, 0, sizeof(struct mpls_control));

    if(0 == psess->ais_enable)
    {
		COM_THREAD_TIMER_OFF(psess->ais_send);
        return ERRNO_FAIL;    
    }
	
    if(psess->info.state != OAM_STATUS_DOWN)
    {
    	OAM_DEBUG("Session:%d is not down.\n", psess->info.session_id);
		COM_THREAD_TIMER_OFF(psess->ais_send);
		return ERRNO_SUCCESS;
    }

    /* 将client_list轮一次发送后,会再次获取client,重建client_list*/
    if(psess->info.type == OAM_TYPE_INTERFACE)
    {
        ret = mplsoam_ais_if_get_clients_info(psess, psess->info.index);
        if(ret)
        {
        	OAM_DEBUG("Fail to get client layer.\n");
			COM_THREAD_TIMER_OFF(psess->ais_send);
			return ERRNO_SUCCESS;
        }

        for(ALL_LIST_ELEMENTS_RO(psess->pais->ais_transport, node, plsp))
        {
        	OAM_DEBUG("ais rcv:send_lsp-name:%s\n", plsp->name);
            memset(&mplscb, 0, sizeof(struct mpls_control));
            mplscb.label_num = 1;
            mplscb.is_changed = 1;
            mplscb.if_type = PKT_INIF_TYPE_LSP;
            mplscb.ifindex = plsp->lsp_index;
            mplscb.ttl = 255;
            mplscb.exp = 5;
		
            ret = pkt_send(PKT_TYPE_MPLS, (union pkt_control *)(&mplscb), &(psess->pais->ais_lsp), sizeof(struct oam_ais_lsp));
			if(ret != 0)
			{
				OAM_DEBUG("send ais pkt fail \n");
			}
		}
    }
    else if(psess->info.type == OAM_TYPE_LSP)
    {
        ret = mplsoam_ais_lsp_get_clients_info(psess, psess->info.index);
        if(ret)
        {
        	OAM_ERROR("Fail to get client layer.\n");
			COM_THREAD_TIMER_OFF(psess->ais_send);
			return ERRNO_SUCCESS;
        }

        for(ALL_LIST_ELEMENTS_RO(psess->pais->ais_transport, node, pl2vc))
        {
            memset(&mplscb, 0, sizeof(struct mpls_control));
            mplscb.chtype = psess->info.channel_type;
            //mplscb.label_num = 2;
            mplscb.is_changed = 1;
            mplscb.if_type = PKT_INIF_TYPE_PW;
            //mplscb.ifindex = pl2vc->pwinfo.pwindex;
            
            /* 因ftm在switch-pw上的转发机制,传入的pwindex与实际发送的方向相反 */
            if(pl2vc != NULL && pl2vc->pswitch_pw != NULL)
            {
            	mplscb.ifindex = pl2vc->pswitch_pw->pwinfo.pwindex;
				OAM_DEBUG("mplscb.ifindex = %u.\n", mplscb.ifindex);
            }
			else
			{
				continue;
			}
            mplscb.ttl = 255;
            mplscb.exp = 5;

			OAM_DEBUG("ais send:fw_pwname:%s, pwoutlabel:%d, pwindex=%d.\n", pl2vc->name, pl2vc->outlabel, pl2vc->pwinfo.pwindex);

            pkt_send(PKT_TYPE_MPLS, (union pkt_control *)(&mplscb), &(psess->pais->ais_pw), sizeof(struct oam_ais_pw));
        }
    }

	COM_THREAD_TIMER_OFF(psess->ais_send);
	psess->ais_send = COM_THREAD_TIMER_ADD("mplsoam_ais_timer", LIB_TIMER_TYPE_LOOP, mplsoam_ais_send, (void *)psess, psess->pais->ais_interval*1000);

    return ERRNO_SUCCESS;
}

int mplsoam_ais_rcv_pkt(struct pkt_buffer *pkt)
{
    struct oam_session *psess    = NULL;
    struct l2vc_entry  *pl2vc    = NULL;
    struct lsp_entry   *egr_lsp  = NULL;
	struct static_lsp  *in_lsp   = NULL;
	struct pw_info     *ppw      = NULL;
    //struct oam_ais_lsp *ais_lsp  = NULL;
    struct oam_ais_pw  *ais_pkt  = NULL;
	struct tunnel_if   *tun_if   = NULL;
	//struct thread      *thread   = NULL;
    uint32_t            index    = 0;
    int                 interval = 0;//ms

	OAM_DEBUG();
	
    if(NULL == pkt)
    {
        return ERRNO_FAIL;
    }

    index = pkt->in_ifindex;

	ais_pkt = (struct oam_ais_pw *)pkt->data;
	if((ais_pkt->oam_header.flag & 0x7) == 0x4)
    {
        interval = 1 * 1000;
    }
    else if((ais_pkt->oam_header.flag & 0x7) == 0x6)
    {
        interval = 60 * 1000;
    }

	interval = interval / 10 * 35;
	
    if(pkt->inif_type == PKT_INIF_TYPE_LSP)
    {
        egr_lsp = mpls_lsp_lookup(index);
        if(NULL == egr_lsp)
        {
            return ERRNO_FAIL;
        }

		tun_if = tunnel_if_lookup(egr_lsp->group_index);
		if(NULL == tun_if)
		{
			OAM_ERROR("Can't find tunnel\n");
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
            return ERRNO_FAIL;
        }

		OAM_DEBUG("ais rev.ais_level:%d,local_level:%d.\n", ais_pkt->oam_header.level, psess->info.level);

        if(ais_pkt->oam_header.level != psess->info.level)
        {
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

        if(ais_pkt->oam_header.level != psess->info.level)
        {
            return ERRNO_FAIL;
        }
    }
	else
	{	
		OAM_ERROR("rcv err, not lsp or pw.\n");
		return ERRNO_FAIL;
	}
	
    if(NULL == psess)
    {
        return ERRNO_FAIL;
    }
    if(0 == psess->alarm.alarm_ais)
    {
        /* 客户层上报ais告警*/
        mplsoam_session_alarm_process(psess->info.session_id, IPC_OPCODE_ADD, OAM_ALARM_AIS);
    }

	COM_THREAD_TIMER_OFF(psess->ais_rcv);
	psess->ais_rcv = COM_THREAD_TIMER_ADD("mplsoam_ais_clear_timer", LIB_TIMER_TYPE_LOOP, mplsoam_ais_clear, (void *)psess, interval);	
	
    return ERRNO_SUCCESS;
}

int mplsoam_ais_clear(void *arg)
{
    struct oam_session *psess = NULL;

	OAM_DEBUG();

    psess = (struct oam_session *)arg;
    if(NULL == psess)
    {
        return ERRNO_FAIL;
    }
    
    if(1 == psess->alarm.alarm_ais)
    {
        mplsoam_session_alarm_process(psess->info.session_id, IPC_OPCODE_CLEAR, OAM_ALARM_AIS);
		COM_THREAD_TIMER_OFF(psess->ais_rcv);
    }
	
    return ERRNO_SUCCESS;
}

void mplsoam_ais_register(void)
{
    union proto_reg proto;

    memset(&proto, 0, sizeof(union proto_reg));
    proto.mplsreg.if_type       = PKT_INIF_TYPE_PW;
    proto.mplsreg.oam_opcode    = CFM_OPCODE_AIS;
	proto.mplsreg.inlabel_value = 13;
    proto.mplsreg.chtype        = MPLS_CTLWORD_CHTYPE_8902;
    pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);

    memset(&proto, 0, sizeof(union proto_reg));
    proto.mplsreg.if_type       = PKT_INIF_TYPE_PW;
    proto.mplsreg.oam_opcode    = CFM_OPCODE_AIS;
	proto.mplsreg.inlabel_value = 13;
    proto.mplsreg.chtype        = 0x7ffa;
    pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);

    memset(&proto, 0, sizeof(union proto_reg));
    proto.mplsreg.if_type    = PKT_INIF_TYPE_PW;
    proto.mplsreg.oam_opcode = CFM_OPCODE_AIS;
    proto.mplsreg.chtype     = MPLS_CTLWORD_CHTYPE_8902;
    pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);

    memset(&proto, 0, sizeof(union proto_reg));
    proto.mplsreg.if_type    = PKT_INIF_TYPE_PW;
    proto.mplsreg.oam_opcode = CFM_OPCODE_AIS;
    proto.mplsreg.chtype     = 0x7ffa;
    pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);

    return ;
}


int mplsoam_ais_if_get_clients_info(struct oam_session *psess, uint32_t ifindex)
{
    struct hash_bucket *pbucket  = NULL;
	struct oam_session *ppsess    = NULL;
    int                 cursor   = 0;

	OAM_DEBUG();

    if(NULL == psess)
    {
        return ERRNO_FAIL;
    }
    
    if(NULL == psess->client_list)
    {
        psess->client_list = list_new();
    }
	else if(!list_isempty(psess->client_list))
	{
		list_delete_all_node(psess->client_list);
	}
	
    if(NULL == psess->pais->ais_transport)
    {
        psess->pais->ais_transport = list_new();
    }
	else if(!list_isempty(psess->pais->ais_transport))
	{
		list_delete_all_node(psess->pais->ais_transport);
	}

    HASH_BUCKET_LOOP(pbucket, cursor, mplsoam_session_table)
    {
        ppsess = (struct oam_session *)pbucket->data;
        if(NULL == ppsess || ppsess->info.mp_type != MP_TYPE_MIP
			|| ppsess->info.type != OAM_TYPE_LSP
				|| ppsess->info.level != psess->pais->level)
        {
			OAM_DEBUG("------ppsess is wrong\n");
            continue;
        }

        if(ppsess->fwd_lsp != NULL && (ppsess->fwd_lsp->oam_nhp_index == ifindex || _SHR_PORT_PARENT_IFINDEX_GET(ppsess->fwd_lsp->oam_nhp_index) == ifindex) )
        {
            listnode_add(psess->pais->ais_transport, ppsess->rev_lsp);
            listnode_add(psess->client_list, ppsess);        
        }
        else if(ppsess->rev_lsp != NULL && (ppsess->rev_lsp->oam_nhp_index == ifindex || _SHR_PORT_PARENT_IFINDEX_GET(ppsess->rev_lsp->oam_nhp_index) == ifindex) )
        {
            listnode_add(psess->pais->ais_transport, ppsess->fwd_lsp);
            listnode_add(psess->client_list, ppsess);
        }
    }

    return 0;
}

int mplsoam_ais_lsp_get_clients_info(struct oam_session *psess, uint32_t lspindex)
{
    struct hash_bucket *pbucket   = NULL;
	struct oam_session *ppsess    = NULL;
    int                 cursor    = 0;
    struct tunnel_if   *ptunnel1  = NULL;
	struct tunnel_if   *ptunnel2  = NULL;

	OAM_DEBUG();

    if(NULL == psess)
    {
        return 1;
    }

    if(NULL == psess->client_list)
    {
        psess->client_list = list_new();
    }
    else if(!list_isempty(psess->client_list))
	{
		list_delete_all_node(psess->client_list);
	}
	
    if(NULL == psess->pais->ais_transport)
    {
        psess->pais->ais_transport = list_new();
    }
	else if(!list_isempty(psess->pais->ais_transport))
	{
		list_delete_all_node(psess->pais->ais_transport);
	}
	
    HASH_BUCKET_LOOP(pbucket, cursor, mplsoam_session_table)
    {
        ppsess = (struct oam_session *)pbucket->data;
        if(NULL == ppsess || ppsess->info.mp_type != MP_TYPE_MIP
			|| ppsess->info.type != OAM_TYPE_PW || NULL == ppsess->sw_pw
				|| ppsess->info.level != psess->pais->level)
        {
        	continue;
        }

		/* session 中存的sw_pw可能是前段也可能是后段 */
		if(ppsess->sw_pw != NULL)
		{
	        ptunnel1 = tunnel_if_lookup(ppsess->sw_pw->tunl_index);
	        if(ptunnel1 != NULL)
	        {
	        	if((ptunnel1->tunnel.master_index == lspindex
		            || ptunnel1->tunnel.backup_index == lspindex)
		            	&& (ppsess->sw_pw->pswitch_pw != NULL))
		        {
					OAM_DEBUG("pw-name:%s, outlabel:%d.\n", ppsess->sw_pw->pswitch_pw->name, ppsess->sw_pw->pswitch_pw->outlabel);
		            listnode_add(psess->pais->ais_transport, ppsess->sw_pw->pswitch_pw);
		            listnode_add(psess->client_list, ppsess);
		        }
	        }
			if(ppsess->sw_pw->pswitch_pw != NULL)
			{
				ptunnel2 = tunnel_if_lookup(ppsess->sw_pw->pswitch_pw->tunl_index);
		        if(ptunnel2 != NULL)
		        {
		        	if(ptunnel2->tunnel.master_index == lspindex
			            || ptunnel2->tunnel.backup_index == lspindex)
			        {
			        	OAM_DEBUG("pw-name:%s, outlabel:%d.\n", ppsess->sw_pw->name, ppsess->sw_pw->outlabel);
			            listnode_add(psess->pais->ais_transport, ppsess->sw_pw);
			            listnode_add(psess->client_list, ppsess);
			        }
		        }
			}
		}        
    }

    return 0;
}


