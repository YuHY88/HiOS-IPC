#include <lib/vty.h>
#include <lib/errcode.h>
#include <lib/memory.h>
#include <lib/module_id.h>
#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>
#include <lib/oam_common.h>
#include <lib/command.h>
#include <lib/log.h>
#include <stdlib.h>
#include "mpls_main.h"
#include "mpls_if.h"
#include "lsp_static.h"
#include "lspm.h"
#include "pw.h"
#include "mpls.h"
#include "mpls_oam.h"
#include "mpls_oam_pkt.h"

/* 注册 lb/lt 报文包括普通mpls以及gre的acl */
void mplsoam_packet_register(void)
{
	union proto_reg proto;

	/* transit匹配lsp的request报文.chtype = 0x10007ffa*/
    memset(&proto, 0, sizeof(union proto_reg));
    proto.mplsreg.chtype        = MPLS_CTLWORD_CHTYPE_7FFA;
    proto.mplsreg.if_type       = PKT_INIF_TYPE_PW;
    proto.mplsreg.oam_opcode    = CFM_OPCODE_LBM;
	proto.mplsreg.inlabel_value = 13;
    proto.mplsreg.out_ttl       = 1;//match outer label
    pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);  

	/* transit匹配lsp的request报文.chtype = 0x10008902*/
	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.chtype        = MPLS_CTLWORD_CHTYPE_8902;
	proto.mplsreg.if_type       = PKT_INIF_TYPE_PW;
	proto.mplsreg.oam_opcode    = CFM_OPCODE_LBM;
	proto.mplsreg.inlabel_value = 13;
	proto.mplsreg.out_ttl       = 1;//match outer label
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);	

	/***************/
	/*SPE 收request.chtype = 0x10007ffa*/
	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.chtype     = MPLS_CTLWORD_CHTYPE_7FFA;
	proto.mplsreg.if_type    = PKT_INIF_TYPE_PW;
	proto.mplsreg.oam_opcode = CFM_OPCODE_LBM;
	proto.mplsreg.ttl        = 1;//match inner label
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);
	
	/*SPE 收request.chtype = 0x10008902*/
	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.chtype	 = MPLS_CTLWORD_CHTYPE_8902;
	proto.mplsreg.if_type	 = PKT_INIF_TYPE_PW;
	proto.mplsreg.oam_opcode = CFM_OPCODE_LBM;
	proto.mplsreg.ttl		 = 1;//match inner label
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);	


	/*LER 节点收request. chtype = 0x10007ffa*/
	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.chtype	    = MPLS_CTLWORD_CHTYPE_7FFA;
	proto.mplsreg.if_type	    = PKT_INIF_TYPE_PW;
	proto.mplsreg.oam_opcode    = CFM_OPCODE_LBM;
	proto.mplsreg.inlabel_value = 13;
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);
	
	/*LER 节点收request. chtype = 0x10008902*/
	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.chtype	    = MPLS_CTLWORD_CHTYPE_8902;
	proto.mplsreg.if_type	    = PKT_INIF_TYPE_PW;
	proto.mplsreg.oam_opcode    = CFM_OPCODE_LBM;
	proto.mplsreg.inlabel_value = 13;
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);

	
	/*LER 节点收reply. chtype = 0x10007ffa*/
	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.chtype	    = MPLS_CTLWORD_CHTYPE_7FFA;
	proto.mplsreg.if_type	    = PKT_INIF_TYPE_PW;
	proto.mplsreg.oam_opcode    = CFM_OPCODE_LBR;
	proto.mplsreg.inlabel_value	= 13;
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);
	
	/*LER 节点收reply. chtype = 0x10008902*/
	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.chtype	    = MPLS_CTLWORD_CHTYPE_8902;
	proto.mplsreg.if_type	    = PKT_INIF_TYPE_PW;
	proto.mplsreg.oam_opcode    = CFM_OPCODE_LBR;
	proto.mplsreg.inlabel_value	= 13;
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);
	/*end*/

	/*PE节点收reply. chtype = 0x10007ffa*/
    memset(&proto, 0, sizeof(union proto_reg));
    proto.mplsreg.chtype     = MPLS_CTLWORD_CHTYPE_7FFA;
    proto.mplsreg.if_type    = PKT_INIF_TYPE_PW;
    proto.mplsreg.oam_opcode = CFM_OPCODE_LBR;
    pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);
	
	/*PE节点收reply. chtype = 0x10008902*/
	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.chtype     = MPLS_CTLWORD_CHTYPE_8902;
	proto.mplsreg.if_type    = PKT_INIF_TYPE_PW;
	proto.mplsreg.oam_opcode = CFM_OPCODE_LBR;
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);	

	/*PE节点收request. chtype = 0x10008902*/
	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.chtype     = MPLS_CTLWORD_CHTYPE_8902;
	proto.mplsreg.if_type    = PKT_INIF_TYPE_PW;
	proto.mplsreg.oam_opcode = CFM_OPCODE_LBM;
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);
	
	/*PE节点收request. chtype = 0x10007ffa*/
	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.chtype     = MPLS_CTLWORD_CHTYPE_7FFA;
	proto.mplsreg.if_type    = PKT_INIF_TYPE_PW;
	proto.mplsreg.oam_opcode = CFM_OPCODE_LBM;
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);

#if 0
	/* PE,huawei three labels,chtype = 0x10007ffa */
	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.chtype     = MPLS_CTLWORD_CHTYPE_7FFA;
	proto.mplsreg.if_type    = PKT_INIF_TYPE_PW;
	proto.mplsreg.oam_opcode = CFM_OPCODE_LBM;
	proto.mplsreg.inlabel_value = 13;
	proto.mplsreg.ttl = 1;
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);

	/* PE,huawei three labels,chtype = 0x10007ffa */
	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.chtype     = MPLS_CTLWORD_CHTYPE_8902;
	proto.mplsreg.if_type    = PKT_INIF_TYPE_PW;
	proto.mplsreg.oam_opcode = CFM_OPCODE_LBM;
	proto.mplsreg.inlabel_value = 13;
	proto.mplsreg.ttl = 1;
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);
#endif

	/* section lb request :0x8902 */
	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.chtype	 = MPLS_CTLWORD_CHTYPE_8902;
	proto.mplsreg.if_type	 = PKT_INIF_TYPE_LSP;
	proto.mplsreg.oam_opcode = CFM_OPCODE_LBM;
	proto.mplsreg.inlabel_value = 13;
	//proto.mplsreg.ttl = 1;
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);

	/* section lb request :0x7ffa */
	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.chtype	 = MPLS_CTLWORD_CHTYPE_7FFA;
	proto.mplsreg.if_type	 = PKT_INIF_TYPE_LSP;
	proto.mplsreg.oam_opcode = CFM_OPCODE_LBM;
	proto.mplsreg.inlabel_value = 13;
	//proto.mplsreg.ttl = 1;
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);


	/* section lb reply :0x8902 */
	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.chtype	 = MPLS_CTLWORD_CHTYPE_8902;
	proto.mplsreg.if_type	 = PKT_INIF_TYPE_LSP;
	proto.mplsreg.oam_opcode = CFM_OPCODE_LBR;
	proto.mplsreg.inlabel_value = 13;
	//proto.mplsreg.ttl = 1;
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);

	/* section lb reply :0x7ffa */
	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.chtype	 = MPLS_CTLWORD_CHTYPE_7FFA;
	proto.mplsreg.if_type	 = PKT_INIF_TYPE_LSP;
	proto.mplsreg.oam_opcode = CFM_OPCODE_LBR;
	proto.mplsreg.inlabel_value = 13;
	//proto.mplsreg.ttl = 1;
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);
	
	return;
}



/* 设置lb 报文的pkt 结构.use new struct */
void mplsoam_lb_set_pkt(struct oam_session *psess, void *ppkt, enum LB_TLV_TYPE tlv_type)
{
	struct timeval time;
	static uint32_t seqnum = 0;
	struct oam_lbm_pkt pkt;
	//struct oam_lbm_pkt *lbm_pkt = NULL;
	struct oam_lbm_request_pkt *lbm_req_pkt = NULL;
	struct oam_lbm_data_pkt *lbm_data_pkt = NULL;
	struct oam_lbm_request_data_pkt *lbm_req_data_pkt = NULL;

	OAM_DEBUG();

	if(NULL == psess || NULL == ppkt)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ;
	}
	
	memset(&pkt, 0, sizeof(struct oam_lbm_pkt));
	
	time_get_time (TIME_CLK_REALTIME, &time); 
	seqnum++;
	
    /* 填写 lb pkt 报文内容 */
	pkt.mel = 7;
	pkt.version = 0;
	pkt.opcode = OAM_LB_REQUEST;
	pkt.flags = 0;
	pkt.tlv_offset = 4;
	pkt.seqNum = htonl(seqnum);

	if(psess->node_id == 0)
	{
		pkt.tlv.mep.type = OAM_LB_MEP_TARGET;  /* traget mep */
		pkt.tlv.mep.len = htons(25);
		pkt.tlv.mep.sub_type = 0x02;    /* icc based MEP ID */
		if(psess->type == OAM_PKT_LB)
		{
			pkt.tlv.mep.mep_id = htons(psess->info.remote_mep);/* fill remote mep-id */
		}
		else if(psess->type == OAM_PKT_LT)
		{
			/* 据中兴设备，发送的lt报文中target_mep_tlv中的mep-id都为0 */
			pkt.tlv.mep.mep_id = 0;
		}
		memset(pkt.tlv.mep.resvd, 0, 22);
	}
	else if(psess->ttl >= 1 && psess->node_id != 0)
	{
		pkt.tlv.mip.type = OAM_LB_MEP_TARGET;  /*traget mep*/
		pkt.tlv.mip.len = htons(25);
		pkt.tlv.mip.sub_type = 0x03;    /* icc based MIP ID */
		memcpy(pkt.tlv.mip.icc, psess->info.megname, 6);
		pkt.tlv.mip.node_id = htonl(psess->node_id);
		pkt.tlv.mip.if_num = 0;
	}
	
	memcpy(ppkt, &pkt, sizeof(struct oam_lbm_pkt));

	if(tlv_type == LB_TLV_END)
	{
		//lbm_pkt = (struct oam_lbm_pkt *)ppkt;
	}
	else if(tlv_type == LB_TLV_REQUEST)
	{
		lbm_req_pkt = (struct oam_lbm_request_pkt *)ppkt;
		lbm_req_pkt->request_tlv.type = 35;
		lbm_req_pkt->request_tlv.len  = htons(53);
		lbm_req_pkt->request_tlv.lk_indication = 0;/* lbm:0;lbr:1 */
		lbm_req_pkt->request_tlv.mep_id = htons(psess->info.mp_id);/* local_mep */
		lbm_req_pkt->request_tlv.resvd = 1;
		lbm_req_pkt->request_tlv.meg_type = 32;
		lbm_req_pkt->request_tlv.meg_len = 13;
		memcpy(lbm_req_pkt->request_tlv.megid, psess->info.megname, strlen((char *)psess->info.megname));
		memset(lbm_req_pkt->request_tlv.unuse, 0, 32);
		memset(lbm_req_pkt->request_tlv.req_resvd, 0, 2);
	}
	else if(tlv_type == LB_TLV_DATA)
	{
		lbm_data_pkt = (struct oam_lbm_data_pkt *)ppkt;
		lbm_data_pkt->data_tlv.type = 3;
		lbm_data_pkt->data_tlv.len  = htons(psess->data_tlv_len);
		memset(lbm_data_pkt->data_tlv.data, 0, psess->data_tlv_len + 1);
	}
	else if(tlv_type == LB_TLV_REQ_DATA)
	{
		lbm_req_data_pkt = (struct oam_lbm_request_data_pkt *)ppkt;
		lbm_req_data_pkt->request_tlv.type = 35;
		lbm_req_data_pkt->request_tlv.len  = htons(53);
		lbm_req_data_pkt->request_tlv.lk_indication = 0;/* lbm:0;lbr:1 */
		lbm_req_data_pkt->request_tlv.mep_id = htons(psess->info.mp_id);/* local_mep */
		memcpy(lbm_req_data_pkt->request_tlv.megid, psess->info.megname, strlen((char *)psess->info.megname));
		memset(lbm_req_data_pkt->request_tlv.unuse, 0, 32);
		memset(lbm_req_data_pkt->request_tlv.req_resvd, 0, 2);
		
		lbm_req_data_pkt->data_tlv.type = 3;
		lbm_req_data_pkt->data_tlv.len  = htons(psess->data_tlv_len);
		memset(lbm_req_data_pkt->data_tlv.data, 0, psess->data_tlv_len + 1);
	}

	 return;
}




/* lb timeout */
int mplsoam_lb_timer_thread(void *arg)
{
	struct oam_session *psess;
	struct vty *vty = NULL;

	OAM_DEBUG();
	
	psess = (struct oam_session *)arg;
	vty = (struct vty *)psess->pvty;
	
	if(psess->lb_enable == 1)
	{
		psess->lb_enable = 0;
		psess->lb_timer = 0;
		vty_out(vty, "MPLS lb timeout !\r\n");
		vtysh_return(vty, CMD_SUCCESS);
	}
	
	if(psess->lb_timer != 0)
	{	
		psess->lb_timer = 0;
	}

	return 0;
}

/* lt timeout */
int mplsoam_lt_timer_thread(void *arg)
{
	struct oam_session *psess = NULL;
	struct vty *vty = NULL;

	OAM_DEBUG();
	
	psess = (struct oam_session *)arg;
	vty = (struct vty *)psess->pvty;
	
	if(psess->lt_enable == 1 )
	{
		if(vty->sig_int_recv == 1)
		{
			psess->lt_enable = 0;
			COM_THREAD_TIMER_OFF(psess->lt_timer);
			vtysh_return(vty, CMD_SUCCESS);
		}
		if(psess->ttl_curr > psess->ttl)
		{
			psess->lt_enable = 0;
			vty_out(vty, "MPLS lt timeout !\r\n");
			vtysh_return(vty, CMD_SUCCESS);
		}
		else
		{
			if(psess->lt_timer != 0)
			{
				COM_THREAD_TIMER_OFF(psess->lt_timer);
			}
			
			//psess->ttl_curr = psess->ttl;
			mplsoam_send_lt(psess);
		}
	}
	
	if(psess->lt_timer != 0)
	{
		COM_THREAD_TIMER_OFF(psess->lt_timer);
	}
	
	return ERRNO_SUCCESS;
}

/* 设置控制块内容并发送报文:use new struct */
int mplsoam_lb_send_pkt(struct oam_session *psess, void *preq, enum LB_TLV_TYPE tlv_type)
{
	struct mpls_control  mplscb ;
	struct eth_control   ethcb  ;
	struct oam_lb_pkt *lb_pkt = NULL;
	uint16_t data_length = 0;
	uint8_t opcode = 0;
	uint32_t label = 13;
	int ret = 0;

	OAM_DEBUG();

	if(NULL == psess || NULL == preq)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ERRNO_FAIL;
	}

	memset(&mplscb, 0, sizeof(struct mpls_control));
	memset(&ethcb, 0, sizeof(struct eth_control));

	opcode = *((uint8_t *)preq + 1);
	if(tlv_type == LB_TLV_END)
	{
		data_length = sizeof(struct oam_lbm_pkt);
	}
	else if(tlv_type == LB_TLV_DATA)
	{
		data_length = sizeof(struct oam_lbm_data_pkt) + psess->data_tlv_len - 1;
	}
	else if(tlv_type == LB_TLV_REQUEST)
	{
		data_length = sizeof(struct oam_lbm_request_pkt);
	}
	else if(tlv_type == LB_TLV_REQ_DATA)
	{
		data_length = sizeof(struct oam_lbm_request_data_pkt) + psess->data_tlv_len - 1;
	}

	
	lb_pkt = malloc(sizeof(struct oam_lb_pkt) + data_length - 1);
	if(NULL == lb_pkt)
	{
		OAM_ERROR("malloc fial\n");
		return ERRNO_FAIL;
	}
	
	memset(lb_pkt, 0, sizeof(struct oam_lb_pkt) + data_length - 1);
	memcpy(lb_pkt->data, preq, data_length);

	if(psess->info.type == OAM_TYPE_PW)
	{
	    /* 使用mpls control */
		memset(&mplscb, 0, sizeof(struct mpls_control));
		mplscb.ifindex = psess->info.index;
		mplscb.if_type = PKT_INIF_TYPE_PW;
		mplscb.chtype = psess->info.channel_type;
		mplscb.exp = psess->lb_exp;/* 待定 */

		if(opcode == OAM_LB_REQUEST)
		{
			if(psess->type == OAM_PKT_LB)
			{
				if(0 == psess->ttl)
				{
					mplscb.ttl = 255;
				}
				else
				{
					mplscb.ttl = psess->ttl;
				}
			}
			else
			{
				mplscb.ttl = psess->ttl_curr;
			}
		}
		else
		{
			mplscb.ttl = 255;
		}
		mplscb.is_changed = 1;

		#if 0
   	   	    int i = 0;
   	   	    u_char *pktdata = ( unsigned char * ) (preq);
   	   	    for (i = 1; i<= data_length; i++)
   	   	    {
   	   			printf ( "%02x ", pktdata[i-1] );
   	   		  	if (0 == i % 16)
   	   			printf("\n");
   	   	    }
		#endif

		ret = pkt_send(PKT_TYPE_MPLS, (union pkt_control *)(&mplscb), preq, data_length);
	}
	else if(psess->info.type == OAM_TYPE_LSP)
	{
		/* 使用mpls control */
		memset(&mplscb, 0, sizeof(struct mpls_control));
		mplscb.ifindex =psess->info.index;
		mplscb.if_type = PKT_INIF_TYPE_LSP;
		mplscb.chtype = 0;
		//mplscb.exp = 5;
		mplscb.exp = psess->lb_exp;/* 待定 */
		
		if(opcode == OAM_LB_REQUEST)
		{
			if(psess->type == OAM_PKT_LB)
			{
				if(0 == psess->ttl)
				{
					mplscb.ttl = 255;
				}
				else
				{
					mplscb.ttl = psess->ttl;
				}
			}
			else
			{
				mplscb.ttl = psess->ttl_curr;
			}
		}
		else
		{
			mplscb.ttl = 255;
		}
		mplscb.is_changed = 1;		
		
		lb_pkt->label.label1 = htons(label >> 4);  /* get high 16bit */
		lb_pkt->label.label0 = label & 0xf;        /* get low 4bit */
		lb_pkt->label.exp = psess->lb_exp;//13标签和外层lsp标签都为命令行配置的exp。
		lb_pkt->label.bos = 1;
		lb_pkt->label.ttl = 1;
		lb_pkt->ctrl_word = htonl(0x10000000 + psess->info.channel_type);

		mplscb.label_num = 1;

		#if 0
   	   	    int i = 0;
   	   	    u_char *pktdata = ( unsigned char * ) (lb_pkt);
   	   	    for (i = 1; i <= (sizeof(struct oam_lb_pkt) + data_length - 1); i++)
   	   	    {
   	   			printf ( "%02x ", pktdata[i-1] );
   	   		  	if (0 == i % 16)
   	   			printf("\n");
   	   	    }
		#endif

		ret = pkt_send(PKT_TYPE_MPLS, (union pkt_control *)(&mplscb), lb_pkt, sizeof(struct oam_lb_pkt) + data_length - 1);
	}
	else if(psess->info.type == OAM_TYPE_INTERFACE)
	{
		memset(&ethcb, 0, sizeof(struct eth_control));
		ethcb.ifindex    = psess->info.index;
		ethcb.is_changed = 1;
		ethcb.ethtype    = 0x8847;
		if(psess->info.vlan >= 1 && psess->info.vlan <= 4094)
		{
			ethcb.svlan = psess->info.vlan;
		}
		ethcb.cos = psess->info.priority;
		memcpy(ethcb.dmac, psess->info.mac, MAC_LEN);
		
		lb_pkt->label.label1 = htons(label >> 4);  /* get high 16bit */
		lb_pkt->label.label0 = label & 0xf;        /* get low 4bit */
		lb_pkt->label.exp = psess->lb_exp;//可配置exp值
		lb_pkt->label.bos = 1;
		if(opcode == OAM_LB_REQUEST)
		{
			if(psess->type == OAM_PKT_LB)
			{
				if(psess->ttl == 0)
				{
					lb_pkt->label.ttl = 255;/* 待定 */
				}
				else
				{
					lb_pkt->label.ttl = psess->ttl;
				}
			}
			else
			{
				lb_pkt->label.ttl = psess->ttl_curr;
			}
		}
		else
		{
			lb_pkt->label.ttl = 255;/* 待定 */
		}
		lb_pkt->ctrl_word = htonl(0x10000000 + psess->info.channel_type);
		
		#if 0
   	   	    int i = 0;
   	   	    u_char *pktdata = ( unsigned char * )(lb_pkt);
   	   	    for (i = 1; i <= (sizeof(struct oam_lb_pkt) + data_length - 1); i++)
   	   	    {
   	   			printf ( "%02x ", pktdata[i-1] );
   	   		  	if (0 == i % 16)
   	   			printf("\n");
   	   	    }
		#endif

		ret = pkt_send(PKT_TYPE_ETH, (union pkt_control *)(&ethcb), lb_pkt, sizeof(struct oam_lb_pkt) + data_length - 1);
	}
	else
		ret = ERRNO_FAIL;
	
	if(ret != ERRNO_FAIL)
	{
		gmplsoam.pkt_send++;     /*mpls进程总共发送lb/lt包的次数*/
	}

	if(opcode == OAM_LB_REQUEST)
	{
		if(tlv_type == LB_TLV_DATA || tlv_type == LB_TLV_REQ_DATA)
		{
			free(preq);
			preq = NULL;	
		}
		
		free(lb_pkt);
		lb_pkt = NULL;
	}
	
	return ret;
}


/* 多段pw回包，设置控制块内容并发送报文,use new struct */
int mplsoam_pw_rcv_pkt(struct pkt_buffer *pkt)
{
	struct mpls_control mplscb;	
	uint32_t pw_index = 0;
	uint16_t chtype = 0;
	int ret = 0;

	OAM_DEBUG();

	if(NULL == pkt)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ERRNO_FAIL;
	}

	pw_index = pkt->in_ifindex;
	chtype = pkt->cb.mplscb.chtype;
    /* 使用mpls control */
	memset(&mplscb, 0, sizeof(struct mpls_control));
	mplscb.ifindex = pw_index;
	mplscb.if_type = PKT_INIF_TYPE_PW;
	mplscb.chtype = chtype;
	mplscb.ttl = 255;
	mplscb.exp = pkt->priority;
	mplscb.is_changed = 1;
	ret = pkt_send(PKT_TYPE_MPLS, (union pkt_control *)(&mplscb), pkt->data, pkt->data_len);
	if(ret != ERRNO_FAIL)
	{
		gmplsoam.pkt_send++;     /*mpls进程总共发送lb/lt包的次数*/
	}
	
	return ret;
}


/* trasit回包，设置控制块内容并发送报文*/
int mplsoam_lsp_rcv_pkt(uint32_t lsp_index, struct pkt_buffer *pkt)
{
	struct mpls_control mplscb;
	struct oam_lb_pkt *lb_pkt = NULL;
	uint16_t chtype = 0;
	uint32_t label = 13;
	int ret = 0;

	OAM_DEBUG();

	if(NULL == pkt)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ERRNO_FAIL;
	}

	chtype = pkt->cb.mplscb.chtype;

	lb_pkt = malloc(sizeof(struct oam_lb_pkt) + pkt->data_len - 1);
	if(NULL == lb_pkt)
	{
		return ERRNO_FAIL;
	}
	memset(lb_pkt, 0, sizeof(struct oam_lb_pkt) + pkt->data_len - 1);

	/* 使用mpls control */
	memset(&mplscb, 0, sizeof(struct mpls_control));
	mplscb.ifindex = lsp_index;
	mplscb.if_type = PKT_INIF_TYPE_LSP;
	mplscb.chtype = 0;
	mplscb.exp = pkt->priority;
	mplscb.ttl = 255;
	mplscb.is_changed = 1;
	
	lb_pkt->label.label1 = htons(label >> 4);  /* get high 16bit */
	lb_pkt->label.label0 = label & 0xf; 	   /* get low 4bit */
	lb_pkt->label.exp = pkt->priority;
	lb_pkt->label.bos = 1;
	lb_pkt->label.ttl = 1;//13标签的ttl置为1
	lb_pkt->ctrl_word = htonl(0x10000000 + chtype);

	mplscb.label_num = 1;
	
	memcpy(lb_pkt->data, pkt->data, pkt->data_len);
	ret = pkt_send(PKT_TYPE_MPLS, (union pkt_control *)(&mplscb), lb_pkt, sizeof(struct oam_lb_pkt) + pkt->data_len - 1);
	if(ret != ERRNO_FAIL)
	{
		gmplsoam.pkt_send++;     /*mpls进程总共发送lb/lt包的次数*/
	}

	free(lb_pkt);
	lb_pkt = NULL;
	
	return ret;
}

int mplsoam_lb_lt_pkt_set(struct oam_session *psess)
{
	struct oam_lbm_pkt              lbm_pkt; 
	struct oam_lbm_request_pkt      lbm_req_pkt;
	struct oam_lbm_data_pkt         *lbm_data_pkt = NULL;
	struct oam_lbm_request_data_pkt *lbm_req_data_pkt = NULL;
	struct timeval time;
	int ret = 0;

	OAM_DEBUG();

	if(NULL == psess)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ERRNO_FAIL;
	}
	time_get_time (TIME_CLK_REALTIME, &time); 
	psess->sendSec.tv_sec = time.tv_sec;
	psess->sendSec.tv_usec = time.tv_usec;

	memset(&lbm_pkt, 0, sizeof(struct oam_lbm_pkt));
	memset(&lbm_req_pkt, 0, sizeof(struct oam_lbm_request_pkt));

	/* ���ñ����ֶ�*/
	if(psess->data_tlv_flag == 0 && psess->req_tlv_flag == 0)
	{
		mplsoam_lb_set_pkt(psess, &lbm_pkt, LB_TLV_END);
		psess->seqNum = ntohl(lbm_pkt.seqNum);
		ret = mplsoam_lb_send_pkt(psess, &lbm_pkt, LB_TLV_END);
		if(ret == ERRNO_FAIL)
		{
			psess->lb_enable = 0;
			
			OAM_ERROR("lb pkt_send fail!\n");
			return ERRNO_FAIL;		
		}
	}
	else if(psess->data_tlv_flag == 1 && psess->req_tlv_flag == 0)
	{
		lbm_data_pkt = (struct oam_lbm_data_pkt *)malloc(sizeof(struct oam_lbm_data_pkt) + psess->data_tlv_len - 1);
		if(NULL == lbm_data_pkt)
		{
			OAM_ERROR("malloc fail!\n");
			return ERRNO_FAIL;
		}
		memset(lbm_data_pkt, 0, sizeof(struct oam_lbm_data_pkt) + psess->data_tlv_len - 1);
		mplsoam_lb_set_pkt(psess, lbm_data_pkt, LB_TLV_DATA);
		
		psess->seqNum = ntohl(lbm_data_pkt->seqNum);
		ret = mplsoam_lb_send_pkt(psess, lbm_data_pkt, LB_TLV_DATA);
		if(ret == ERRNO_FAIL)
		{
			psess->lb_enable = 0;
			
			OAM_ERROR("lb pkt_send fail!\n");
			return ERRNO_FAIL;		
		}
	}
	else if(psess->data_tlv_flag == 0 && psess->req_tlv_flag == 1)
	{
		mplsoam_lb_set_pkt(psess, &lbm_req_pkt, LB_TLV_REQUEST);
		
		psess->seqNum = ntohl(lbm_req_pkt.seqNum);
		ret = mplsoam_lb_send_pkt(psess, &lbm_req_pkt, LB_TLV_REQUEST);
		if(ret == ERRNO_FAIL)
		{
			psess->lb_enable = 0;
			
			OAM_ERROR("lb pkt_send fail!\n");
			return ERRNO_FAIL;		
		}
	}
	else if(psess->data_tlv_flag == 1 && psess->req_tlv_flag == 1)
	{
		lbm_req_data_pkt = (struct oam_lbm_request_data_pkt *)malloc(sizeof(struct oam_lbm_request_data_pkt) + psess->data_tlv_len - 1);
		if(NULL == lbm_req_data_pkt)
		{
			OAM_ERROR("malloc fail!\n");
			return ERRNO_FAIL;
		}
		memset(lbm_req_data_pkt, 0, sizeof(struct oam_lbm_request_data_pkt) + psess->data_tlv_len - 1);
		mplsoam_lb_set_pkt(psess, lbm_req_data_pkt, LB_TLV_REQ_DATA);

		psess->seqNum = ntohl(lbm_req_data_pkt->seqNum);
		ret = mplsoam_lb_send_pkt(psess, lbm_req_data_pkt, LB_TLV_REQ_DATA);
		if(ret == ERRNO_FAIL)
		{
			psess->lb_enable = 0;
			
			OAM_ERROR("lb pkt_send fail!\n");
			return ERRNO_FAIL;		
		}
	}
	
	return ERRNO_SUCCESS;
}


/* 发送 lb 报文, 起一个 timer 等待应答 .use new struct.*/
int mplsoam_send_lb(struct oam_session *psess)
{
	OAM_DEBUG();

	if(NULL == psess)
	{
		return ERRNO_FAIL;
	}
	
	mplsoam_lb_lt_pkt_set(psess);
	
	if(psess->lb_timer == 0)
	{
		psess->lb_timer = COM_THREAD_TIMER_ADD("mplsoam_lb_timer", LIB_TIMER_TYPE_NOLOOP, mplsoam_lb_timer_thread, (void *)psess, 5*1000);
	}
	
	return ERRNO_SUCCESS;
}


/* 收到 lb 报文, 回应答:use new struct */
int mplsoam_recv_lb(struct pkt_buffer *pkt)
{
	struct oam_lb_requesting_mep_tlv *req_tlv = NULL;
	struct cfm_pdu_header *oam_header = NULL;
	struct oam_mep_tlv *mep_tlv = NULL;
	struct oam_mip_tlv *mip_tlv = NULL;
	struct oam_session *psess = NULL;
	struct oam_session *mip_sess  = NULL;
	char   node_id[INET_ADDRSTRLEN] = "";
	struct l2vc_entry *l2vc_entry = NULL;
	struct static_lsp *plsp = NULL;
	struct mpls_if *pif = NULL;
	uchar *lspname = NULL;
	struct vty *vty = NULL;
	uint16_t data_tlv_len = 0;
	uint16_t target_mep_len = 0;
	uint8_t tgt_sub_type = 0;
	uint8_t tlv_type = 0;
	uchar *pname = NULL;
	struct timeval time;

	OAM_DEBUG();
	
	gmplsoam.pkt_recv++;

	oam_header = (struct cfm_pdu_header *)pkt->data;
	target_mep_len = *((uint16_t *)((uint8_t *)pkt->data + 9));//协议25，中兴15
	target_mep_len = ntohs(target_mep_len);
	if(target_mep_len != 25)
	{
		OAM_ERROR("target mep len err:mep_len = %d.\n", target_mep_len);
		return ERRNO_FAIL;
	}
	
	if(*((uint8_t *)pkt->data + 36) == 35)	
	{
		if(pkt->data_len > 92 && *((uint8_t *)pkt->data + 92) == 3)
		{
			data_tlv_len = ntohs(*((uint16_t *)((uint8_t *)pkt->data + 93)));
			
			OAM_DEBUG("data tlv len:%d.\n", data_tlv_len);
			tlv_type = LB_TLV_REQ_DATA;
		}
		else
		{
			tlv_type = LB_TLV_REQUEST;
		}
		req_tlv = (struct oam_lb_requesting_mep_tlv *)((uint8_t *)pkt->data + 36);
	}
	else if(*((uint8_t *)pkt->data + 36) == 3)
	{
		data_tlv_len = ntohs(*((uint16_t *)((uint8_t *)pkt->data + 37)));
		OAM_DEBUG("data tlv len:%d.\n", data_tlv_len);
		tlv_type = LB_TLV_DATA;
	}

	tgt_sub_type = *((uint8_t *)pkt->data + 11);
	if(tgt_sub_type == 0x2)
	{
		mep_tlv = (struct oam_mep_tlv *)((uint8_t *)pkt->data + 8);
	}
	else if(tgt_sub_type == 0x3)
	{
		mip_tlv = (struct oam_mip_tlv *)((uint8_t *)pkt->data + 8);
	}
	/* 中兴设备发送的lt报文sub_type为0x00 */
	else
	{
		OAM_DEBUG("tgt_sub_type = 0x00.\n");
		mep_tlv = (struct oam_mep_tlv *)((uint8_t *)pkt->data + 8);
	}

	/* section lb */
	if(pkt->cb.mplscb.label_num == 1 && pkt->cb.mplscb.label == 13 && pkt->inif_type == PKT_INIF_TYPE_LSP)//section
	{
		pif = mpls_if_lookup(pkt->in_port);
		if(NULL == pif)
		{
			OAM_ERROR("Can't find mpls_if.\n");
			goto out;
		}
		
		psess = mplsoam_session_lookup(pif->mplsoam_id);
		if((NULL == psess) || (psess->info.state == OAM_STATUS_DOWN))
		{
			OAM_ERROR("Oam session is not exit or is down!\n");
			goto out;
		}
		psess->lb_exp = pkt->priority;

		/* request:与local mep一致；reply:与remote mep一致*/
		if(tgt_sub_type == 0x2)
		{
			if(ntohs(mep_tlv->mep_id) != 0)
			{
				if(((oam_header->opcode == OAM_LB_REQUEST) && (psess->info.mp_id != ntohs(mep_tlv->mep_id)))
					|| ((oam_header->opcode == OAM_LB_REPLY) && (psess->info.remote_mep != ntohs(mep_tlv->mep_id))))
				{
					OAM_ERROR("Oam lb pkt does not match oam session!\n");
					goto out;
				}
			}
		}
		psess->data_tlv_len = data_tlv_len;
		
		/* 判断vlan信息是否与配置的一致 */
		

	}
	else if(pkt->cb.mplscb.label_num == 2 && pkt->cb.mplscb.label == 13 && pkt->inif_type == PKT_INIF_TYPE_LSP)
	{
		lspname = static_lsp_get_name(pkt->in_ifindex);
		if(lspname == NULL)
		{
			OAM_ERROR("Lsp is not exit!\n");
			goto out;
		}
		
	    plsp = static_lsp_lookup(lspname);
		if(plsp == NULL)
		{
			OAM_ERROR("Plsp is not exit!\n");
			goto out;
		}

		if(plsp->direction == LSP_DIRECTION_TRANSIT)
		{
			if(oam_header->opcode != OAM_LB_REQUEST)
			{
				goto out;
			}
			mip_sess = mplsoam_session_lookup(plsp->mplsoam_id);
			if(NULL == mip_sess || mip_sess->info.mp_type != MP_TYPE_MIP)
			{	
				goto out;
			}
			
			if(tgt_sub_type != 0x3)
			{
				/* 中兴lt报文的mep-id为0 */
				if((ntohs(mep_tlv->mep_id) != 0) && (oam_header->opcode == OAM_LB_REQUEST)
					&& (mip_sess->info.mp_id != ntohs(mep_tlv->mep_id)))
				{
					OAM_ERROR("Oam lb pkt does not match oam session!\n");
					goto out;
				}
					
				OAM_ERROR("Sub-type not 0x3, not mip id tlv.\n");
				//goto out;
			}
			
			#if 0
			mip_sess = mplsoam_session_lookup(plsp->mplsoam_id);
			if(NULL == mip_sess || mip_sess->info.mp_type != MP_TYPE_MIP)
			{	
				goto out;
			}
			#endif
			
			if(tlv_type == LB_TLV_REQUEST || tlv_type == LB_TLV_REQ_DATA)
			{
				req_tlv->lk_indication = 1;
			}

			oam_header->opcode = OAM_LB_REPLY;

			if(mip_tlv != NULL)
			{
				if(ntohl(mip_tlv->node_id) != gmpls.lsr_id)
				{
					goto out;
				}
				mip_tlv->type = OAM_LB_MEP_REPLY;
			}
			
			if(plsp == mip_sess->fwd_lsp)
			{	
				mplsoam_lsp_rcv_pkt(mip_sess->rev_lsp->lsp_index, pkt);
				return ERRNO_SUCCESS;
			}
			else if(plsp == mip_sess->rev_lsp)
			{	
				mplsoam_lsp_rcv_pkt(mip_sess->fwd_lsp->lsp_index, pkt);
				return ERRNO_SUCCESS;
			}
			else
			{
				goto out;
			}
		}
		else
		{
			psess = mplsoam_session_lookup(plsp->mplsoam_id);
			if((NULL == psess) || (psess->info.state == OAM_STATUS_DOWN))
			{
				OAM_ERROR("ession is not exit or is down!\n");
				goto out;
			}
			psess->lb_exp = pkt->priority;
			
			if(tgt_sub_type == 0x2)//mep packet
			{	
				if(ntohs(mep_tlv->mep_id) != 0)
				{
					if(((oam_header->opcode == OAM_LB_REQUEST) && (psess->info.mp_id != ntohs(mep_tlv->mep_id)))
						|| ((oam_header->opcode == OAM_LB_REPLY) && (psess->info.remote_mep != ntohs(mep_tlv->mep_id))))
					{
						OAM_ERROR(" lb pkt does not match oam session!\n");
						goto out;
					}
				}
			}
			else if(tgt_sub_type == 0x3)
			{
				if(oam_header->opcode == OAM_LB_REQUEST)
				{
					if(ntohl(mip_tlv->node_id) != gmpls.lsr_id)
					{
						OAM_ERROR("Oam lb pkt:node-id is not match.\n");
						goto out;
					}
					
					if(tlv_type == LB_TLV_REQUEST || tlv_type == LB_TLV_REQ_DATA)
					{
						req_tlv->lk_indication = 1;
					}
					oam_header->opcode = OAM_LB_REPLY;
					mip_tlv->type = OAM_LB_MEP_REPLY;

					mplsoam_lsp_rcv_pkt(psess->info.index, pkt);
					return ERRNO_SUCCESS;
				}
			}
			psess->data_tlv_len = data_tlv_len;
		}
	}
	else if(pkt->inif_type == PKT_INIF_TYPE_PW)
	{
		pname = pw_get_name(pkt->in_ifindex);//pw的ilm的pwindex，为switch-pw的pwindex。
		if(pname == NULL)
		{
			OAM_ERROR("Pw is not exit!\n");
			goto out;
		}
		
	    l2vc_entry = l2vc_lookup(pname);
		if(l2vc_entry == NULL)
		{
			OAM_ERROR("L2vc_entry is not exit!\n");
			goto out;
		}

		if((l2vc_entry->pswitch_pw != NULL) && (l2vc_entry->pwinfo.mspw_index != 0))
		{
			if(oam_header->opcode == OAM_LB_REPLY)
			{
				goto out;
			}

			mip_sess = mplsoam_session_lookup(l2vc_entry->mplsoam_id);
			if(NULL == mip_sess || mip_sess->info.mp_type != MP_TYPE_MIP)
			{
				goto out;
			}

			if(tgt_sub_type != 0x3)
			{
				/* 中兴设备发送到中间设备的subtype并非是0x3,且指定的是mip id */
				if((ntohs(mep_tlv->mep_id) != 0) && (oam_header->opcode == OAM_LB_REQUEST)
					&& (mip_sess->info.mp_id != ntohs(mep_tlv->mep_id)))
				{
					OAM_ERROR("Oam lb pkt does not match oam session!\n");
					goto out;
				}

			
				OAM_ERROR("ID Sub-type not 0x3.\n");
				//goto out;
			}

			//switch-pw的一个pw的ilm存储的是另一个pw的pwindex
			pkt->in_ifindex = l2vc_entry->pswitch_pw->pwinfo.pwindex;
 
			/* 据ftm机制SPE中下发的pwindex与实际发送的pw是相反的。*/
			pkt->in_ifindex = l2vc_entry->pwinfo.pwindex;
			
			oam_header->opcode = OAM_LB_REPLY;

			if(mip_tlv != NULL)
			{
				if(gmpls.lsr_id != ntohl(mip_tlv->node_id))
				{
					OAM_ERROR("Oam pw lb pkt:node-id is not match.\n");
					goto out;	
				}
				mip_tlv->type = OAM_LB_MEP_REPLY;
			}
			
			if(tlv_type == LB_TLV_REQUEST || tlv_type == LB_TLV_REQ_DATA)
			{
				req_tlv->lk_indication = 1;
			}
			mplsoam_pw_rcv_pkt(pkt);
			
			return ERRNO_SUCCESS;
		}	
		else
		{
			psess = mplsoam_session_lookup(l2vc_entry->mplsoam_id);
			if((NULL == psess) || (psess->info.state == OAM_STATUS_DOWN))
			{
				OAM_ERROR("oam session is not exist or is down!\n");
				goto out;
			}
			
			psess->lb_exp = pkt->priority;
			
			if(tgt_sub_type == 0x2)
			{	
				if(ntohs(mep_tlv->mep_id) != 0)
				{
					if(((oam_header->opcode == OAM_LB_REQUEST) && (psess->info.mp_id != ntohs(mep_tlv->mep_id)))
						|| ((oam_header->opcode == OAM_LB_REPLY) && (psess->info.remote_mep != ntohs(mep_tlv->mep_id))))
					{
						OAM_ERROR("Oam lb pkt does not match oam session!\n");
						goto out;
					}
				}
			}
			else if(tgt_sub_type == 0x3)
			{
				if(oam_header->opcode == OAM_LB_REQUEST)
				{
					if(gmpls.lsr_id != ntohl(mip_tlv->node_id))
					{
						OAM_ERROR("Oam lb pkt:node-id is not match.\n");
						goto out;
					}
					oam_header->opcode = OAM_LB_REPLY;
					mip_tlv->type = OAM_LB_MEP_REPLY;
					if(tlv_type == LB_TLV_REQUEST || tlv_type == LB_TLV_REQ_DATA)
					{
						req_tlv->lk_indication = 1;
					}
					mplsoam_pw_rcv_pkt(pkt);
					
					return ERRNO_SUCCESS;
				}
			}
			psess->data_tlv_len = data_tlv_len;
		}
	}
	if(oam_header->opcode == OAM_LB_REQUEST)
	{
		oam_header->opcode = OAM_LB_REPLY;
		mep_tlv->type = OAM_LB_MEP_REPLY;
		
		if(tlv_type == LB_TLV_REQUEST || tlv_type == LB_TLV_REQ_DATA)
		{
			req_tlv->lk_indication = 1;
		}
		psess->lb_exp = pkt->priority;
		mplsoam_lb_send_pkt(psess, pkt->data, tlv_type);
	}
	else    /*�յ�reply*/
	{
		vty = (struct vty *)psess->pvty;
		time_get_time (TIME_CLK_REALTIME, &time);
		if(tgt_sub_type == 0x2)
		{
			if((ntohs(mep_tlv->mep_id) != 0)
				&& (psess->info.remote_mep != ntohs(mep_tlv->mep_id)))
			{
				OAM_ERROR("tlv mep-id err.\n");
				goto out;
			}
		}
		else if(tgt_sub_type == 0x3)
		{
			if(psess->node_id != ntohl(mip_tlv->node_id))
			{
				goto out;
			}
		}
		
		if(ntohl(*((uint32_t *)((uint8_t *)pkt->data + 4))) != psess->seqNum)	
		{
			goto out;
		}
		if(psess->type == OAM_PKT_LB)
		{
			if(psess->lb_enable == 1) /*δ��ʱ*/
			{
				psess->lb_enable = 0;
				if(tgt_sub_type == 0x2)
				{
					vty_out(vty, "\rMPLS lb Reply from %d: bytes=%d, Sequence=%d time=%ld ms  !\r\n", 
						psess->info.remote_mep, psess->packet_size, psess->seqNum, timeval_elapsed (time, psess->sendSec)/1000);
				}
				else if(tgt_sub_type == 0x3)
				{
					inet_ipv4tostr(ntohl(mip_tlv->node_id), node_id);
					vty_out(vty, "\rMPLS lb Reply from node %s: bytes=%d, Sequence=%d time=%ld ms  !\r\n", 
						node_id, psess->packet_size, psess->seqNum, timeval_elapsed (time, psess->sendSec)/1000);
					psess->node_id   = 0;
					psess->ttl       = 0;
				}
				else
				{
					vty_out(vty, "\rMPLS lb Reply: bytes=%d, Sequence=%d time=%ld ms  !\r\n", 
						psess->packet_size, psess->seqNum, timeval_elapsed (time, psess->sendSec)/1000);
				}
				vtysh_return(vty, CMD_SUCCESS);
			}
		}
		else
		{
			if(psess->lt_enable == 1) /*δ��ʱ*/
			{
				if(psess->ttl_curr > psess->ttl)
				{
					psess->lt_enable = 0;
					
					vty_out(vty, "MPLS lt Reply: ttl= %d, bytes=%d, Sequence=%u time=%lu ms  !\r\n", 
						(psess->ttl_curr - 1), psess->packet_size, psess->seqNum, (timeval_elapsed (time, psess->sendSec)/1000));
					
					vtysh_return(vty, CMD_SUCCESS);
				}
				else
				{
					vty_out(vty, "MPLS lt Reply: ttl= %d, bytes=%d, Sequence=%u time=%lu ms  !\r\n", 
						(psess->ttl_curr - 1), psess->packet_size, psess->seqNum, (timeval_elapsed (time, psess->sendSec)/1000));
					
				}
				//vtysh_return(vty, CMD_SUCCESS);
			}
		}
	}
	return ERRNO_SUCCESS;
out:	
	gmplsoam.pkt_err++;
	return ERRNO_FAIL;
}


int mplsoam_send_lt(struct oam_session *psess)
{
	OAM_DEBUG();

	if(NULL == psess)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ERRNO_FAIL;
	}

	if(psess->lt_enable == 0)
	{
		OAM_ERROR("lt isn`t enable\n");
		return ERRNO_FAIL;
	}
	
	mplsoam_lb_lt_pkt_set(psess);

	psess->ttl_curr ++;

	if(psess->lt_timer == 0)
	{
		psess->lt_timer = COM_THREAD_TIMER_ADD("mplsoam_lt_timer", LIB_TIMER_TYPE_LOOP, mplsoam_lt_timer_thread, (void *)psess, 1000);
	}
	
	return ERRNO_SUCCESS;
}



