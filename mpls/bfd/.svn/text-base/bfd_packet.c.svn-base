#include <stdlib.h>
#include <string.h>
#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>
#include <lib/checksum.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/zassert.h>
#include <lib/hash1.h>
#include <lib/log.h>
#include <lib/thread.h>
#include <lib/module_id.h>
#include <lib/ether.h>
#include <lib/ifm_common.h>
#include <lib/errcode.h>
#include <lib/mpls_common.h>
#include <lib/alarm.h>
#include <lib/gpnSocket/socketComm/gpnGlobalPortIndexDef.h>
#include <lib/devm_com.h>
#include  <lib/oam_common.h>

#include <ftm/pkt_mpls.h>
#include "mpls.h"
#include "mpls_if.h"
#include "mpls_main.h"
#include "tunnel.h"
#include "pw.h"
#include "lspm.h"
#include "lsp_static.h"
#include "bfd.h"
#include "bfd_session.h"
#include "bfd_packet.h"

void bfd_init(void)
{
	gbfd.enable = 1;
	gbfd.multiplier = BFD_DETECT_MULTIPLIER;
	gbfd.priority = BFD_PRIORITY;
	gbfd.cc_interval = BFD_CC_INTERVAL;
	gbfd.wtr = 30;
	gbfd.debug_fsm = 0;
	gbfd.debug_packet = 0;
	gbfd.up_count = 0;
	gbfd.down_count = 0;
	gbfd.ttl = BFD_TTL;

	gbfd.ip_router_alert = BFD_ENABLE;
	gbfd.mpls_bfd_enable = BFD_MPLS_DISABLE; 	//全局使能mpls bfd
	gbfd.bfd_init_mode = BFD_ACTIVE;   			//会话工作模式（主动/被动）；
	gbfd.snmp_trap = BFD_SNMP_ENABLE;           //使能bfd trap
	gbfd.mult_detect = BFD_DETECT_MULTIPLIER;	//全局多跳bfd配置，默认参数存在。mpls bfd 使能后，若未配置模板 则使用此模板；
	gbfd.mult_hop_cc_tx_interval = BFD_CC_INTERVAL;
	gbfd.mult_hop_cc_rx_interval = BFD_CC_INTERVAL;
	gbfd.mult_hop_port = BFD_MULTIHOP_DSTPORT;
	
	memset(bfd_up_timeout, 0, BFD_SESS_DISC_MAX);
	memset(bfd_up_wait_counter, 30, BFD_SESS_DISC_MAX);
	bfd_pkt_register();
}

void bfd_pkt_register(void)
{
	union proto_reg proto;

	memset(&proto, 0, sizeof(union proto_reg));	//bfd for ip proto
	proto.ipreg.protocol = IP_P_UDP;
	proto.ipreg.dport = 3784;
	pkt_register(MODULE_ID_BFD, PROTO_TYPE_IPV4, &proto);

	memset(&proto, 0, sizeof(union proto_reg));	//bfd for pw proto with ip/udp header
	proto.mplsreg.chtype = MPLS_CTLWORD_CHTYPE_IPV4;
	proto.mplsreg.if_type = PKT_INIF_TYPE_PW;
	proto.mplsreg.protocol = IP_P_UDP;
	proto.mplsreg.dport = 3784;
	pkt_register(MODULE_ID_BFD, PROTO_TYPE_MPLS, &proto);

	memset(&proto, 0, sizeof(union proto_reg));	//bfd for pw proto without ip/udp header
	proto.mplsreg.chtype = MPLS_CTLWORD_WITHOUT_IP;
	proto.mplsreg.if_type = PKT_INIF_TYPE_PW;
	pkt_register(MODULE_ID_BFD, PROTO_TYPE_MPLS, &proto);
}

/*该函数暂时不调用，因为pkt_unregister不好用*/
void bfd_pkt_unregister(void)
{
	union proto_reg proto;

	memset(&proto, 0, sizeof(union proto_reg));
	proto.ipreg.protocol = IP_P_UDP;
	proto.ipreg.dport = 3784;
	//proto.ipreg.dip = BFD_DFT_MULTI_INADDR;

	pkt_unregister(MODULE_ID_BFD, PROTO_TYPE_IPV4, &proto);
}

void bfd_pkt_debug(void *data, int len, uint32_t ifindex, int flag)
{
	char ifname[INTERFACE_NAMSIZE+1] = "";

    if(data == NULL || !gbfd.debug_packet)
    {
        return ;
    }
	ifm_get_name_by_ifindex(ifindex, ifname);
	if(flag == BFD_SEND)
	{
		BFD_DEBUG("----------%s Send BFD Packet----------\n", ifname);
	}
	else if(flag == BFD_RECV)
    {
		BFD_DEBUG("----------%s Recv BFD Packet----------\n", ifname);
    }

	printf("%s\n", pkt_dump(data, len));
}
void bfd_fsm_start_debug(enum BFD_EVENT event, struct bfd_sess *old_psess)
{
	if(!gbfd.debug_fsm)
	{
		return ;
	}
	BFD_DEBUG("-----------bfd fsm start-----------\n");
	BFD_DEBUG("local_id:%d, type:%d, status=%d, diag=%d, poll=%d, event=%d\n",
		old_psess->local_id_cfg ? old_psess->local_id_cfg : old_psess->local_id, old_psess->type, old_psess->status, old_psess->diag, old_psess->poll, old_psess->bfd_event);
	BFD_DEBUG("BFD_EVENT:%d\n", event);
}

void bfd_fsm_end_debug(struct bfd_sess *new_psess)
{
	if(!gbfd.debug_fsm)
	{
		return ;
	}
	BFD_DEBUG("local_id:%d, type:%d, status=%d, diag=%d, poll=%d, event=%d\n",
		new_psess->local_id_cfg ? new_psess->local_id_cfg : new_psess->local_id, new_psess->type, new_psess->status, new_psess->diag, new_psess->poll, new_psess->bfd_event);
	BFD_DEBUG("-----------bfd fsm end-----------\n\n");
}

/*仅bfd for ip 时调用*/
int bfd_ip_pkt_encap(struct bfd_sess *psess, uint8_t *pkt)
{
	struct bfd_message bfdmsg;
	struct udp_bfd_packet packet;
	uint32_t recv_interval = 0;
	uint32_t send_interval = 0;

	BFD_DEBUG();

	/*封装bfd payload*/
	memset(&bfdmsg, 0, sizeof(struct bfd_message));
	memset(&packet, 0, sizeof(packet));
	bfdmsg.version = BFD_VERSION;
	bfdmsg.diag = psess->diag;
	bfdmsg.sta = psess->status;
	/*在参数需要改变时置位*/
	bfdmsg.poll = psess->poll;
	/*在响应poll序列时置位*/
	bfdmsg.final = psess->final;
	bfdmsg.CPI = 0;
	bfdmsg.auth = 0;
	bfdmsg.demand = 0;
	bfdmsg.resv = 0;
	bfdmsg.detect_mult = psess->cc_multiplier_cfg ? psess->cc_multiplier_cfg : gbfd.multiplier;
	bfdmsg.length = BFD_LENGTH;
	bfdmsg.local_disc = htonl(psess->local_id_cfg ? psess->local_id_cfg : psess->local_id);
	bfdmsg.remote_disc = htonl(psess->remote_id_cfg ? psess->remote_id_cfg : psess->remote_id);

	if(psess->status != BFD_STATUS_UP)
	{
		/*协商阶段使用init_interval*/
		send_interval = recv_interval = psess->init_interval;
		if(psess->status == BFD_STATUS_INIT)
		{
			recv_interval = psess->cc_interval_cfg ? psess->cc_interval_cfg : gbfd.cc_interval;
		}
	}
	else
	{
		/*up阶段使用配置值*/
		send_interval = recv_interval = psess->cc_interval_cfg ? psess->cc_interval_cfg : gbfd.cc_interval;
	}

	bfdmsg.min_tx_interval = htonl(send_interval);
	bfdmsg.min_rx_interval = htonl(recv_interval);
	/*暂时不支持回声功能*/
	bfdmsg.min_echo_rx_interval = 0;
	/*封装udp*/
	packet.udp.sport = htons(BFD_SRCPORT);
	packet.udp.dport = htons(BFD_DSTPORT);
	packet.udp.len = htons((sizeof(packet.udp) + sizeof(struct bfd_message))); /* cheat on the psuedo-header */
	memcpy(&(packet.data), &bfdmsg, sizeof(struct bfd_message));
	packet.udp.checksum = 0;
	/*封装ip*/
	packet.ip.saddr = htonl(psess->srcip);
	packet.ip.daddr = htonl(psess->dstip);
	packet.ip.ihl = sizeof(packet.ip) >> 2;
	packet.ip.protocol = IP_P_UDP;
	packet.ip.tos = gbfd.priority << 5;
	packet.ip.tot_len = htons(52);
	packet.ip.ttl = 255;
	packet.ip.version = 4;
	packet.ip.check = (in_checksum((uint16_t *)&(packet.ip), sizeof(packet.ip)));

	memcpy(pkt, &packet, sizeof(packet));

	return sizeof(packet);
}
int bfd_pkt_encap(struct bfd_sess *psess, uint8_t *pkt)
{
	struct bfd_message bfdmsg;
	struct udp_bfd_packet packet;
	struct udp_bfd_ip_packet ip_packet;
	uint32_t recv_interval = 0;
	uint32_t send_interval = 0;
	char ploopback[16] = "127.0.0.1";

    BFD_DEBUG();

	/*封装bfd payload*/
	memset(&bfdmsg, 0, sizeof(struct bfd_message));
	memset(&packet, 0, sizeof(packet));
	bfdmsg.version = BFD_VERSION;
	bfdmsg.diag = psess->diag;
	bfdmsg.sta = psess->status;
	/*在参数需要改变时置位*/
	bfdmsg.poll = psess->poll;
	/*在响应poll序列时置位*/
	bfdmsg.final = psess->final;
	bfdmsg.CPI = 0;
	bfdmsg.auth = 0;
	bfdmsg.demand = 0;
	bfdmsg.resv = 0;
	bfdmsg.detect_mult = psess->cc_multiplier_cfg ? psess->cc_multiplier_cfg : gbfd.multiplier;
	bfdmsg.length = BFD_LENGTH;
	bfdmsg.local_disc = htonl(psess->local_id_cfg ? psess->local_id_cfg : psess->local_id);
	bfdmsg.remote_disc = htonl(psess->remote_id_cfg ? psess->remote_id_cfg : psess->remote_id);

	if(psess->status != BFD_STATUS_UP)
	{
		/*协商阶段使用init_interval*/
		send_interval = recv_interval = psess->init_interval;
		if(psess->status == BFD_STATUS_INIT)
		{
			recv_interval = psess->cc_interval_cfg ? psess->cc_interval_cfg : gbfd.cc_interval;
		}
	}
	else
	{
		/*up阶段使用配置值*/
		send_interval = recv_interval = psess->cc_interval_cfg ? psess->cc_interval_cfg : gbfd.cc_interval;
	}
	bfdmsg.min_tx_interval = htonl(send_interval);
	bfdmsg.min_rx_interval = htonl(recv_interval);
	/*暂时不支持回声功能*/
	bfdmsg.min_echo_rx_interval = 0;

	/*封装udp*/
	packet.udp.sport = htons(BFD_SRCPORT);
	packet.udp.dport = htons(BFD_DSTPORT);
	packet.udp.len = htons((sizeof(packet.udp) + sizeof(struct bfd_message))); /* cheat on the psuedo-header */
	memcpy(&(packet.data), &bfdmsg, sizeof(struct bfd_message));
	//packet.udp.checksum = in_checksum(&(packet), sizeof(struct bfd_message) + sizeof(struct udphdr) );
	packet.udp.checksum = 0;

	/*封装ip*/
	if(psess->type == BFD_TYPE_INTERFACE)
	{
		packet.ip.daddr = htonl(BFD_DFT_MULTI_INADDR);
		packet.ip.ihl = sizeof(packet.ip) >> 2;
		packet.ip.protocol = IP_P_UDP;
		packet.ip.saddr = htonl(psess->srcip);
		packet.ip.tos = gbfd.priority << 5;
		packet.ip.tot_len = htons(52);
		packet.ip.ttl = gbfd.ttl;
		packet.ip.version = 4;
		packet.ip.check = (in_checksum((uint16_t *)&(packet.ip), sizeof(packet.ip)));
	}
	else if(psess->type == BFD_TYPE_PW || psess->type == BFD_TYPE_LSP)
	{
		if(0 == psess->without_ip)
		{
			packet.ip.tot_len	= htons(52);
			packet.ip.ihl		= 5;
			packet.ip.version	= 4;
			packet.ip.tos		= gbfd.priority << 5;
			packet.ip.ttl		= gbfd.ttl;
			packet.ip.id		= 0;
			packet.ip.frag_off	= 0;
			packet.ip.protocol	= IP_P_UDP;
			packet.ip.saddr 	= htonl(gmpls.lsr_id);
			packet.ip.daddr 	= htonl(inet_strtoipv4(ploopback));
			packet.ip.check 	= (in_checksum((uint16_t *)&(packet.ip), sizeof(packet.ip)));
		}
		else
		{
			memcpy(pkt, &bfdmsg, sizeof(struct bfd_message));
			return sizeof(bfdmsg);
		}
	}
	else if(psess->type == BFD_TYPE_IP)
	{
		/*封装udp*/
		ip_packet.udp.sport = htons(BFD_SRCPORT);
		ip_packet.udp.dport = htons(BFD_DSTPORT);
		ip_packet.udp.len = htons((sizeof(ip_packet.udp) + sizeof(struct bfd_message))); /* cheat on the psuedo-header */
		memcpy(&(ip_packet.data), &bfdmsg, sizeof(struct bfd_message));
		ip_packet.udp.checksum = 0;

		memcpy(pkt, &ip_packet, sizeof(ip_packet));
		return sizeof(ip_packet);
	}

	memcpy(pkt, &packet, sizeof(packet));
	return sizeof(packet);
}

int bfd_send_hal(struct bfd_sess *psess, enum IPC_OPCODE optcode, uint8_t subtype)
{
	struct bfd_info hal_bfd;
    uint8_t ctrl_word[4] = {0x10, 0x0, 0x0, 0x21};
	uint8_t ctrl_word2[4] = {0x10, 0x0, 0x0, 0x7};
	int ret = 0;

    BFD_DEBUG();

	memset(&hal_bfd, 0, sizeof(hal_bfd));
	hal_bfd.index = psess->index;
	hal_bfd.without_ip = psess->without_ip;
	hal_bfd.tdm_pw_label = psess->tdm_pw_label;
	hal_bfd.sess_id = psess->session_id ? psess->session_id : psess->session_id_cfg;
	hal_bfd.local_id = psess->local_id ? psess->local_id : psess->local_id_cfg;
	hal_bfd.remote_id = psess->remote_id ? psess->remote_id : psess->remote_id_cfg;
	hal_bfd.offset = sizeof(struct iphdr) + sizeof(struct udphdr);
	hal_bfd.type = psess->type;
	hal_bfd.state = psess->status;
	hal_bfd.diag = psess->diag;
	hal_bfd.src_ip = psess->srcip;
	hal_bfd.dst_ip = psess->dstip;
	hal_bfd.priority = gbfd.priority;
	hal_bfd.remote_multiplier = psess->remote_multiplier;
	hal_bfd.recv_interval = (psess->cc_interval_recv)/1000;
	hal_bfd.send_interval = (psess->cc_interval_send)/1000;
	if(hal_bfd.type == BFD_TYPE_PW)
	{
		if(0 == psess->without_ip)
		{
			/*报文模板的最前面需要加控制字0x10000021*/
			memcpy(hal_bfd.pkt_template, ctrl_word, 4);
			bfd_pkt_encap(psess, hal_bfd.pkt_template + 4);
			hal_bfd.pkt_len = sizeof(struct udp_bfd_packet) + 4;
			hal_bfd.offset = hal_bfd.offset + 4;
		}
		else
		{
			/*报文模板的最前面需要加控制字0x10000007*/
			memcpy(hal_bfd.pkt_template, ctrl_word2, 4);
			bfd_pkt_encap(psess, hal_bfd.pkt_template + 4);
			hal_bfd.pkt_len = sizeof(struct bfd_message) + 4;
			hal_bfd.offset = hal_bfd.offset + 4;
		}
	}
	else if(BFD_TYPE_IP == hal_bfd.type)
	{
		bfd_ip_pkt_encap(psess, hal_bfd.pkt_template);
		hal_bfd.pkt_len = sizeof(struct udp_bfd_packet);
	}
	else
	{
		bfd_pkt_encap(psess, hal_bfd.pkt_template);
		hal_bfd.pkt_len = sizeof(struct udp_bfd_packet);
	}

	if(IPC_OPCODE_ADD == optcode)
	{
		ret = msg_sender_to_other_wait_ack(&hal_bfd , sizeof(struct bfd_info),1, MODULE_ID_HAL, MODULE_ID_BFD,
						IPC_TYPE_BFD, subtype, optcode, hal_bfd.sess_id);
		if(0 != ret)
		{
			if(psess->session_id)
			{
				bfd_session_delete(psess->session_id);
			}
			else if(psess->session_id_cfg)
			{
				bfd_session_disable(psess);
			}
			BFD_DEBUG("--->bfd session num enough.\n");
			return ret;
		}
	}
	else
	{
		ret = ipc_send_msg_n2(&hal_bfd , sizeof(struct bfd_info),1, MODULE_ID_HAL, MODULE_ID_BFD,
						IPC_TYPE_BFD, subtype, optcode, hal_bfd.sess_id);
	}
	if(ret < 0)
	{
		BFD_DEBUG("--->Failed to send bfd fsm to hal.\n");
		zlog_err("%s[%d]: Entering function '%s',Failed to send bfd fsm to hal.\n", __FILE__, __LINE__, __func__);
		return ret;
	}

	return ret;
}

int bfd_send(struct bfd_sess *psess)
{
	union pkt_control pktcontrol;
	struct mpls_control  mplscb;
	uint8_t bfd_pkt[BFD_PKT_TMP_MAX_LEN];
    uchar dmac[6] = {0x01, 0x00, 0x5e, 0x00, 0x00, 0xb8};
	int ret = 0;

	BFD_DEBUG();

	/*构造报文模板*/
	memset(bfd_pkt, 0, sizeof(bfd_pkt));
	bfd_pkt_encap(psess, bfd_pkt);

	/*bfd for pw走mpls转发*/
	if(psess->type == BFD_TYPE_PW)
	{
		if(0 == psess->without_ip)
		{
			memset(&mplscb, 0, sizeof(struct mpls_control));
			mplscb.ifindex = psess->index;
			mplscb.if_type = PKT_INIF_TYPE_PW;
			mplscb.chtype = MPLS_CTLWORD_CHTYPE_IPV4;
			mplscb.ttl = gbfd.ttl;
			mplscb.exp = gbfd.priority;
			mplscb.is_changed = 1;
			ret = pkt_send(PKT_TYPE_MPLS, (union pkt_control *)&mplscb, &bfd_pkt, sizeof(struct udp_bfd_packet));
		}
		else
		{
			memset(&mplscb, 0, sizeof(struct mpls_control));
			mplscb.ifindex = psess->index;
			mplscb.if_type = PKT_INIF_TYPE_PW;
			mplscb.chtype = MPLS_CTLWORD_WITHOUT_IP;
			mplscb.ttl = gbfd.ttl;
			mplscb.exp = gbfd.priority;
			mplscb.is_changed = 1;
			ret = pkt_send(PKT_TYPE_MPLS, (union pkt_control *)&mplscb, &bfd_pkt, sizeof(struct bfd_message)+16);
		}
	}
	if(psess->type == BFD_TYPE_LSP)
	{
		memset(&mplscb, 0, sizeof(struct mpls_control));
		mplscb.ifindex = psess->index;
		mplscb.if_type = PKT_INIF_TYPE_LSP;
		mplscb.chtype = 0;
		mplscb.ttl = gbfd.ttl;
		mplscb.exp = gbfd.priority;
		mplscb.is_changed = 1;
		ret = pkt_send(PKT_TYPE_MPLS, (union pkt_control *)&mplscb, &bfd_pkt, sizeof(struct udp_bfd_packet));
	}
	if(psess->type == BFD_TYPE_INTERFACE)
	{
	memset(&pktcontrol, 0, sizeof(pktcontrol));
	pktcontrol.ethcb.ethtype = ETH_P_IP;
	pktcontrol.ethcb.smac_valid = 0;
	pktcontrol.ethcb.is_changed = 1;
	pktcontrol.ethcb.ifindex = psess->index;
	/*将mac地址填为01:00:5e:00:00:b8*/
	memcpy ( pktcontrol.ethcb.dmac, dmac, 6 );

	ret = pkt_send(PKT_TYPE_ETH, &pktcontrol, &bfd_pkt, sizeof(struct udp_bfd_packet));
	}
	if(psess->type == BFD_TYPE_IP)
	{
		memset(&pktcontrol, 0, sizeof(pktcontrol));
		/*eth头控制部分*/
		pktcontrol.ethcb.ethtype = ETH_P_IP;
		pktcontrol.ethcb.smac_valid = 0;
		pktcontrol.ethcb.is_changed = 1;
		pktcontrol.ethcb.ifindex = psess->index;
		/*ip头控制部分*/
		pktcontrol.ipcb.ifindex = psess->index;
		pktcontrol.ipcb.sip = psess->srcip;
		pktcontrol.ipcb.dip = psess->dstip;
		pktcontrol.ipcb.if_type = PKT_INIF_TYPE_IF;
		pktcontrol.ipcb.tos = gbfd.priority;
		pktcontrol.ipcb.ttl = gbfd.ttl;
		pktcontrol.ipcb.is_changed = 1;
		pktcontrol.ipcb.protocol = IP_P_UDP;
		pktcontrol.ipcb.chsum_enable = 0;

		ret = pkt_send(PKT_TYPE_IP, &pktcontrol, &bfd_pkt, sizeof(struct udp_bfd_ip_packet));
	}
	if(ret == NET_SUCCESS)
	{
		bfd_pkt_debug(bfd_pkt, sizeof(struct udp_bfd_packet), psess->index, BFD_SEND);
		psess->send_count++;
	}
	else
	{
		/*发送失败*/
		BFD_DEBUG("--->BFD send pkt fail!!!\n");
		zlog_err("%s[%d]: Entering function '%s',Failed to send bfd packet.\n", __FILE__, __LINE__, __func__);
	}
	return ret;
}

//防止平台状态永久停滞
int bfd_cc_timeout(void *arg)
{
	struct bfd_sess *psess = NULL;
	uint16_t sess_id = 0;
	uint32_t local_id = 0;
	uint32_t remote_id = 0;
	int ret = 0;

	BFD_DEBUG();

	psess = (struct bfd_sess *)arg;

	sess_id = psess->session_id_cfg ? psess->session_id_cfg : psess->session_id;
	local_id = psess->local_id_cfg ? psess->local_id_cfg : psess->local_id;
	remote_id = psess->remote_id_cfg ? psess->remote_id_cfg : psess->remote_id;

	BFD_DEBUG("--->sess_id=%d, local_id=%d, remote_id=%d, state=%d.\n",
		sess_id, local_id, remote_id, psess->status);

	if(bfd_up_timeout[sess_id- 1] == 1)
	{
		if(bfd_up_wait_counter[sess_id - 1] == 0)
		{
			psess->retry_num = 0;
			BFD_THREAD_TIMER_OFF(psess->retry_timer);
			bfd_up_timeout[sess_id - 1] = 0;
			bfd_up_wait_counter[sess_id - 1] = 30;
			bfd_fsm(BFD_EVENT_INIT_TIMEOUT, psess);
			bfd_send_hal(psess, IPC_OPCODE_UPDATE, 0);
			return 0;
		}
		bfd_up_wait_counter[sess_id - 1]--;
	}

	BFD_DEBUG("--->sess_id=%d, local_id=%d, remote_id=%d, state=%d.\n",
		sess_id, local_id, remote_id, psess->status);

	return ret;
}
/* 协商阶段发送 bfd 控制报文 */
int bfd_send_retry(void *arg)
{
	struct bfd_sess *psess = NULL;
	uint16_t sess_id = 0;
	uint32_t local_id = 0;
	uint32_t remote_id = 0;
	int ret = 0;

	BFD_DEBUG();

	psess = (struct bfd_sess *)arg;

	sess_id = psess->session_id_cfg ? psess->session_id_cfg : psess->session_id;
	local_id = psess->local_id_cfg ? psess->local_id_cfg : psess->local_id;
	remote_id = psess->remote_id_cfg ? psess->remote_id_cfg : psess->remote_id;

	BFD_DEBUG("--->sess_id=%d, local_id=%d, remote_id=%d, state=%d.\n",
		sess_id, local_id, remote_id, psess->status);

	BFD_DEBUG("--->retry_num =%d.\n", psess->retry_num);

	if(psess->retry_num >= 20)
	{
		/*如果init重传3次，回到down状态*/
		if(psess->status == BFD_STATUS_INIT)
		{
			/*与华为设备对接，由于协商报文发送间隔大于10s，使得对接超时无法对接失败*/
			/*重传20次再超时，防止停在init状态*/
			psess->retry_num = 0;
			BFD_THREAD_TIMER_OFF(psess->retry_timer);
			bfd_fsm(BFD_EVENT_INIT_TIMEOUT, psess);
			return 0;
		}

		if(psess->status == BFD_STATUS_DOWN)
		{
			psess->retry_num = 0;
			psess->remote_id = 0;
		}
	}

	if( psess->remote_id_cfg == 0 && psess->remote_id == 0 && psess->status != BFD_STATUS_DOWN)
	{
		BFD_THREAD_TIMER_OFF(psess->retry_timer);
		psess->retry_num = 0;
		bfd_fsm(BFD_EVENT_INIT_TIMEOUT, psess);
		return 0;
	}

	BFD_DEBUG("--->sess_id=%d, local_id=%d, remote_id=%d, state=%d.\n",
		sess_id, local_id, remote_id, psess->status);

	ret = bfd_send(psess);
	if(ret == NET_FAILED)
	{
		BFD_DEBUG("--->BFD pkt send fail!!\n");
		zlog_err("%s, %d pkt_send fail!\n",__FUNCTION__, __LINE__);
	}
	else if(ret == NET_SUCCESS)
	{
		psess->retry_num++;
		BFD_DEBUG("--->add retry_num =%d.\n", psess->retry_num);
	}

	return ret;
}

/*发送poll序列*/
int bfd_send_poll(struct bfd_sess *psess, enum BFD_EVENT poll_event)
{
	int ret = 0, final = 0;
	uint32_t cc_interval = 0;
	uint16_t sess_id = 0;

	BFD_DEBUG();

	if(NULL == psess)
	{
		zlog_err("%s, %d pointer NULL\n",__FUNCTION__, __LINE__);
		return ERRNO_FAIL;
	}

	sess_id = psess->session_id_cfg ? psess->session_id_cfg : psess->session_id;

	BFD_DEBUG("--->sess_id=%d, state=%d, P=%d, F=%d.\n", sess_id, psess->status, psess->poll, psess->final);

	//if(psess->index != 0)
	{
		final = psess->final;
		psess->final = 0;
		/*up之后，若间隔增大，先把检测间隔下发到fsm*/
		/*收到Final报文之后再把发送间隔下发到fsm*/
		cc_interval = psess->cc_interval_cfg ? psess->cc_interval_cfg : gbfd.cc_interval;
		if(psess->remote_state == BFD_STATUS_UP && psess->status == BFD_STATUS_UP &&
			poll_event == BFD_EVENT_INTERVAL_INC)
		{
			if(cc_interval >= psess->cc_interval_recv)
			{
				psess->cc_interval_recv = cc_interval;
				/*检测间隔要变大，先下发到fpga*/
				BFD_DEBUG("\n>>>>>>>>>>>>>>>>>>>>>>>>>    Increase  >>>>>>>>>>>>>>>>>>>>\n");
				bfd_send_hal(psess, IPC_OPCODE_UPDATE, 0);
			}
			else
			{
				/*仅更新报文内容*/
				bfd_send_hal(psess, IPC_OPCODE_UPDATE, 0);
			}
		}
		if(psess->remote_state == BFD_STATUS_UP && psess->status == BFD_STATUS_UP &&
			poll_event == BFD_EVENT_INTERVAL_DEC)
		{
			if(cc_interval <= psess->cc_interval_send)
			{
				psess->cc_interval_send = cc_interval;
				/*发送间隔要变小，先下发到fpga*/
				BFD_DEBUG("\n<<<<<<<<<<<<<<<<<<<<<<<<<    Decrease  <<<<<<<<<<<<<<<<<<<\n");
				bfd_send_hal(psess, IPC_OPCODE_UPDATE, 0);
			}
			else
			{
				/*仅更新报文内容*/
				bfd_send_hal(psess, IPC_OPCODE_UPDATE, 0);
			}
		}

		if(poll_event == BFD_EVENT_CHANGE_PARA)
		{
			bfd_send_hal(psess, IPC_OPCODE_UPDATE, 0);
		}

		psess->poll = 1;
		//psess->bfd_event = BFD_EVENT_CHANGE_PARA;
		psess->poll_event = poll_event;
		ret = bfd_send(psess);
		if(ret == NET_FAILED)
		{
			zlog_err("%s, %d pkt_send fail!\n",__FUNCTION__, __LINE__);
		}
		BFD_DEBUG("--->\nbfd_send_poll\n");
		psess->final = final;
	}
	return ret;
}

/* 发送 final报文 */
int bfd_send_final(struct bfd_sess *psess)
{
	int ret = 0, poll = 0;
	uint16_t sess_id = 0;

	BFD_DEBUG();

	if(NULL == psess)
	{
		zlog_err("%s, %d pointer NULL\n",__FUNCTION__, __LINE__);
		return ERRNO_FAIL;
	}

	sess_id = psess->session_id_cfg ? psess->session_id_cfg : psess->session_id;

	BFD_DEBUG("--->sess_id=%d, state=%d, P=%d, F=%d.\n", sess_id, psess->status, psess->poll, psess->final);

	poll = psess->poll;
	psess->poll = 0;
	psess->final = 1;
	ret = bfd_send(psess);
	if(ret == NET_FAILED)
	{
			BFD_DEBUG("--->BFD pkt send fail!!\n");
		zlog_err("%s, %d pkt_send fail!\n",__FUNCTION__, __LINE__);
	}
		BFD_DEBUG("--->bfd_send_final\n");
	psess->final = 0;
	psess->poll = poll;

	return ret;
}

void bfd_recv_poll(struct bfd_sess *psess, struct bfd_message *pmsg)
{
	uint32_t cc_interval_recv = 0;
	uint32_t cc_interval_send = 0;
	uint16_t sess_id = 0;
	uint32_t flag = 0;

	BFD_DEBUG();

	if(NULL == pmsg)
	{
		zlog_err("%s, %d pointer NULL\n",__FUNCTION__, __LINE__);
		return;
	}

	if(NULL == psess)
	{
		zlog_err("%s, %d pointer NULL\n",__FUNCTION__, __LINE__);
		return;
	}

	sess_id = psess->session_id_cfg ? psess->session_id_cfg : psess->session_id;

	BFD_DEBUG("--->sess_id=%d, state=%d, P=%d, F=%d.\n", sess_id, psess->status, psess->poll, psess->final);

	if(psess->remote_state != BFD_STATUS_UP && pmsg->sta == BFD_STATUS_UP)
	{
		BFD_DEBUG("--->event = BFD_EVENT_RECV_FIRST_UP\n");
		/*首包up*/
		psess->bfd_event = BFD_EVENT_RECV_FIRST_UP;
		psess->remote_recv_interval = pmsg->min_rx_interval;
		psess->remote_send_interval = pmsg->min_tx_interval;
		psess->remote_multiplier = pmsg->detect_mult;
		psess->remote_state = pmsg->sta;
		bfd_fsm(psess->bfd_event, psess);
		return ;
	}
	if(pmsg->sta == BFD_STATUS_INIT)
	{
		BFD_DEBUG("--->event = BFD_EVENT_RECV_INIT_POLL\n");
	
		psess->bfd_event = BFD_EVENT_RECV_INIT_POLL;
		psess->remote_recv_interval = pmsg->min_rx_interval;
		psess->remote_send_interval = pmsg->min_tx_interval;
		psess->remote_multiplier = pmsg->detect_mult;
		psess->remote_state = pmsg->sta;
		bfd_fsm(psess->bfd_event, psess);
		return;
	}
	if(pmsg->sta == BFD_STATUS_DOWN)
	{
		BFD_DEBUG("--->event = BFD_EVENT_RECV_DOWN_POLL\n");
	
		psess->bfd_event = BFD_EVENT_RECV_DOWN_POLL;
		psess->remote_recv_interval = pmsg->min_rx_interval;
		psess->remote_send_interval = pmsg->min_tx_interval;
		psess->remote_multiplier = pmsg->detect_mult;
		psess->remote_state = pmsg->sta;
		bfd_fsm(psess->bfd_event, psess);
		return;
	}
	/*重新计算cc_interval*/
	psess->remote_recv_interval = pmsg->min_rx_interval;
	psess->remote_send_interval = pmsg->min_tx_interval;
	psess->remote_state = pmsg->sta;
	cc_interval_send = MAX(psess->remote_recv_interval, psess->cc_interval_cfg ? psess->cc_interval_cfg : gbfd.cc_interval);
	cc_interval_recv = MAX(psess->remote_send_interval, psess->cc_interval_cfg ? psess->cc_interval_cfg : gbfd.cc_interval);
	if(cc_interval_recv != psess->cc_interval_recv)
	{
		psess->cc_interval_recv = cc_interval_recv;
		flag = 1;
	}
	if(cc_interval_send != psess->cc_interval_send)
	{
		psess->cc_interval_send = cc_interval_send;
		flag = 1;
	}
	psess->retry_num = 0;
	if(psess->remote_multiplier != pmsg->detect_mult)
	{
		psess->remote_multiplier = pmsg->detect_mult;
		flag = 1;
	}
	bfd_send_final(psess);
	if(flag && psess->status == BFD_STATUS_UP && psess->poll == 0 && psess->final == 0)
	{
		BFD_THREAD_TIMER_OFF(psess->retry_timer);
		bfd_send_hal(psess, IPC_OPCODE_UPDATE, 0);
		bfd_state_notify(BFD_STATUS_UP, psess);

		//关闭up之后的检测定时器
		BFD_THREAD_TIMER_OFF(psess->cc_timer);
		bfd_up_timeout[sess_id - 1] = 0;
		bfd_up_wait_counter[sess_id - 1] = 30;
	}
	return ;
}

void bfd_recv_final(struct bfd_sess *psess, struct bfd_message *pmsg)
{
	uint32_t cc_interval_recv = 0;
	uint32_t cc_interval_send = 0;
	uint8_t flag = 0;
	uint16_t sess_id = 0;

	BFD_DEBUG();

	if(NULL == pmsg)
	{
		zlog_err("%s, %d pointer NULL\n",__FUNCTION__, __LINE__);
		return;
	}

	if(NULL == psess)
	{
		zlog_err("%s, %d pointer NULL\n",__FUNCTION__, __LINE__);
		return;
	}

	sess_id = psess->session_id_cfg ? psess->session_id_cfg : psess->session_id;

	BFD_DEBUG("--->sess_id=%d, state=%d, P=%d, F=%d.\n", sess_id, psess->status, psess->poll, psess->final);

	/*如果本地poll==1进行处理*/
	psess->remote_recv_interval = pmsg->min_rx_interval;
	psess->remote_send_interval = pmsg->min_tx_interval;
	psess->remote_multiplier = pmsg->detect_mult;
	psess->remote_state = pmsg->sta;
	if(psess->poll == 1)
	{
		psess->poll = 0;
		if(psess->poll_event == BFD_EVENT_INTERVAL_INC  ||
			psess->poll_event == BFD_EVENT_INTERVAL_DEC)
		{
			/*参数改变*/
			cc_interval_recv = MAX(psess->remote_send_interval, psess->cc_interval_cfg ? psess->cc_interval_cfg : gbfd.cc_interval);
			cc_interval_send = MAX(psess->remote_recv_interval, psess->cc_interval_cfg ? psess->cc_interval_cfg : gbfd.cc_interval);
			if(cc_interval_recv != psess->cc_interval_recv)
			{
				psess->cc_interval_recv = cc_interval_recv;
				flag = 1;
			}
			if(cc_interval_send != psess->cc_interval_send)
			{
				psess->cc_interval_send = cc_interval_send;
				flag = 1;
			}
			if(flag && psess->status == BFD_STATUS_UP && psess->remote_state == BFD_STATUS_UP)
			{
				BFD_DEBUG("\n^^^^^^^^^^^^^^^FFFFFFFFFFFFFFFF^^interval change ^^^^^^^FFFFFFFFFFF^^^^^^^^\n");
				BFD_THREAD_TIMER_OFF(psess->retry_timer);
				bfd_send_hal(psess, IPC_OPCODE_UPDATE, 0);
			}
			psess->retry_num = 0;
		}
		else if(psess->poll_event == BFD_EVENT_FIRST_UP )
		{
			if(psess->remote_state == BFD_STATUS_UP && psess->status == BFD_STATUS_UP)
			{
				BFD_THREAD_TIMER_OFF(psess->retry_timer);
				bfd_send_hal(psess, IPC_OPCODE_UPDATE, 0);
				bfd_state_notify(BFD_STATUS_UP, psess);

				//关闭up之后的检测定时器
				BFD_THREAD_TIMER_OFF(psess->cc_timer);
				bfd_up_timeout[sess_id - 1] = 0;
				bfd_up_wait_counter[sess_id - 1] = 30;
			}
			else
			{
				BFD_DEBUG("--->sess_id=%d\n", sess_id);
				if(psess->remote_state == BFD_STATUS_INIT && psess->status == BFD_STATUS_UP)
				{
					BFD_THREAD_TIMER_OFF(psess->retry_timer);
					bfd_send_hal(psess, IPC_OPCODE_UPDATE, 0);
					bfd_state_notify(BFD_STATUS_UP, psess);
				
					BFD_THREAD_TIMER_OFF(psess->cc_timer);
					bfd_up_timeout[sess_id - 1] = 0;
					bfd_up_wait_counter[sess_id - 1] = 30;
				}
			}
		}
		psess->poll_event = BFD_EVENT_MAX;
	}
}

/* 接收 bfd 报文 */
void bfd_recv(struct pkt_buffer *pkt)
{
	struct bfd_message *bfdmsg = NULL;
	struct mpls_if *pif = NULL;
	struct bfd_sess *psess = NULL;
    struct l2vc_entry *pl2vc = NULL;
	struct pw_info *pwinfo = NULL;
	struct tunnel_if *ptnlif = NULL;
	struct hash_bucket *pbucket = NULL;
	struct static_lsp *plsp = NULL;
	struct tunnel_if *tpif = NULL;
	uchar *lspname  = NULL;
	uint32_t local_id = 0;
	uint32_t src_ip = 0;
	uint16_t sess_id = 0;
	uint16_t cursor = 0;

	BFD_DEBUG();

	/*将报文的网络序转换为主机序*/
	bfdmsg = (struct bfd_message*)pkt->data;
	if(NULL == bfdmsg)
	{
		BFD_DEBUG("%s, %d, bfdmsg is NULL! \n");
		goto out;
	}

	//src_ip = ntohl(pkt->cb.ipcb.sip);
	BFD_DEBUG("--->src_ip = %d! \n", src_ip);

	bfdmsg->local_disc = ntohl(bfdmsg->local_disc);
	bfdmsg->remote_disc = ntohl(bfdmsg->remote_disc);
	bfdmsg->min_tx_interval = ntohl(bfdmsg->min_tx_interval);
	bfdmsg->min_rx_interval = ntohl(bfdmsg->min_rx_interval);
	bfdmsg->min_echo_rx_interval = ntohl(bfdmsg->min_echo_rx_interval);
	/*打印报文内容应该也是主机序*/

	bfd_pkt_debug(bfdmsg, pkt->data_len, pkt->in_ifindex, BFD_RECV);
	bfd_pkt_debug(pkt->data, pkt->data_len, pkt->in_ifindex, BFD_RECV);
	if(pkt->inif_type == PKT_INIF_TYPE_PW)
	{
		BFD_DEBUG("*****bfd for pw************\n");
		/*检查bfd是否绑定到pw,根据index查找绑定信息*/
		pwinfo = pw_lookup(pkt->in_ifindex);
		if(pwinfo == NULL)
		{
		    BFD_DEBUG("--->pwinfo is NULL! pw index:%d\n", pkt->in_ifindex);
			goto out;
		}

        pl2vc = pw_get_l2vc(pwinfo);
        if (NULL == pl2vc)
        {
            BFD_DEBUG("--->l2vc is NULL! pw index:%d\n", pkt->in_ifindex);
			goto out;
        }

        psess = bfd_session_lookup(pl2vc->bfd_id);
		if(NULL != psess)
		{
			if(MPLS_CTLWORD_WITHOUT_IP == pkt->cb.mplscb.chtype)
			{
				if(0 == psess->without_ip)
				{
					BFD_DEBUG("--->CHTYPE is diff!\n");
					//goto out;
				}
			}
			else
			{
				if(1 == psess->without_ip)
				{
					BFD_DEBUG("--->CHTYPE is diff!\n");
					//goto out;
				}
			}
		}
	}
	else if(pkt->inif_type == PKT_INIF_TYPE_TUNNEL)
	{
		BFD_DEBUG("*****bfd for tunnel**********\n");
	    ptnlif = tunnel_if_lookup(pkt->in_ifindex);
        if (NULL == pif)
        {
            BFD_DEBUG("--->ptnlif is NULL! tunnel if index:%d\n", pkt->in_ifindex);
            goto out;
        }

        psess = bfd_session_lookup(ptnlif->tunnel.bfd_id);
	}
	else if(pkt->inif_type == PKT_INIF_TYPE_LSP)
	{
		BFD_DEBUG("*****bfd pkt_inif_tpe_lsp*********\n");
		HASH_BUCKET_LOOP(pbucket, cursor, bfd_session_table) //bfd for ip 时使用
		{
			if(pbucket && pbucket->data)
			{
				psess = (struct bfd_sess *)pbucket->data;
				if(0 != psess->dstip)
				{
					if(pkt->cb.ipcb.sip == psess->dstip)
					{
						BFD_DEBUG("*****bfd for ip**********\n");
						goto get_psess;
					}
					psess = NULL;
				}
				else
					psess = NULL;
			}
		}

get_psess:
		if(NULL == psess)			//bfd for lsp 时使用
		{
			BFD_DEBUG("*****bfd for lsp*********\n");
			lspname = static_lsp_get_name(pkt->in_ifindex);//由lspindex获取静态lspname
			if(NULL == lspname)
			{
				BFD_DEBUG("--->lspname is NULL! lspindex:%d\n", pkt->in_ifindex);
				pif = mpls_if_lookup(pkt->in_port);		//bfd for 接口使用
				if(NULL  == pif)
				{
					BFD_DEBUG("--->pif is NULL! in port ifindex:%d\n", pkt->in_port);
					goto out;
				}
				if(0 == pif->bfd_sessid)
				{
					BFD_DEBUG("--->pif->bfd_sessid is NULL! in port ifindex:%d\n", pkt->in_port);
					goto out;
				}

				psess = bfd_session_lookup(pif->bfd_sessid);
				if(NULL == psess)
					goto out;
			}
			else
			{
				plsp = static_lsp_lookup(lspname);//由lspname获取静态lsp
				if(NULL == plsp)
				{
					BFD_DEBUG("--->plsp is NULL! lspindex:%d\n", pkt->in_ifindex);
					goto out;
				}

				tpif = tunnel_if_lookup(plsp->group_index);
				if(NULL == tpif)
				{
					BFD_DEBUG("--->tpif is NULL! group_index:%d\n", plsp->group_index);
					goto out;
				}
				if(NULL == tpif->p_mplstp)
				{
					BFD_DEBUG("--->p_mplstp == NULL\n");
					goto out;
				}
				if(NULL != tpif->p_mplstp->ingress_lsp)
				{
					if(NULL != tpif->p_mplstp->egress_lsp)
					{
						BFD_DEBUG("tpif->p_mplstp->egress_lsp->lsp_index = %d\n", tpif->p_mplstp->egress_lsp->lsp_index);
						if(pkt->in_ifindex == tpif->p_mplstp->egress_lsp->lsp_index)
						{
							sess_id = tpif->p_mplstp->ingress_lsp->bfd_id;
							psess = bfd_session_lookup(sess_id);
						}
					}
				}
				BFD_DEBUG("*****lspindex exist******\n");
				if(NULL != tpif->p_mplstp->backup_ingress_lsp)
				{
					if(NULL != tpif->p_mplstp->backup_egress_lsp)
					{
						BFD_DEBUG("tpif->p_mplstp->backup_egress_lsp->lsp_index = %d\n", tpif->p_mplstp->backup_egress_lsp->lsp_index);
						if(pkt->in_ifindex == tpif->p_mplstp->backup_egress_lsp->lsp_index)
						{
							sess_id = tpif->p_mplstp->backup_ingress_lsp->bfd_id;
							psess = bfd_session_lookup(sess_id);
						}
					}
				}
			}
		}
	}
	else
	{
		pif = mpls_if_lookup(pkt->in_port);
		if(NULL  == pif)
		{
			BFD_DEBUG("--->pif is NULL! in port ifindex:%d\n", pkt->in_port);
			goto out;
		}
		if(0 == pif->bfd_sessid)
		{
			BFD_DEBUG("--->pif->bfd_sessid is NULL! in port ifindex:%d\n", pkt->in_port);
			goto out;
		}
		psess = bfd_session_lookup(pif->bfd_sessid);
	}

	if(psess == NULL)
	{
		BFD_DEBUG("--->psess = NULL*******\n");
		goto out;
	}

	psess->recv_count++;

	if(bfdmsg->version != 1 || bfdmsg->length < 24 || bfdmsg->detect_mult == 0 || bfdmsg->local_disc == 0
		|| (bfdmsg->remote_disc == 0 && bfdmsg->sta != BFD_STATUS_ADMINDOWN && bfdmsg->sta != BFD_STATUS_DOWN))
	{
		BFD_DEBUG("--->bfdmsg check fail!!\n");
		goto out;
	}
	/*匹配会话*/
	if(psess->remote_id_cfg != 0)
	{
		if(psess->remote_id_cfg != bfdmsg->local_disc)
		{
			/*静态配置远端标识不一致*/
			BFD_DEBUG("--->bfd_disc check fail!!\n");
			goto out;
		}
	}
	else if(psess->remote_id != 0)
	{
		if(psess->remote_id != bfdmsg->local_disc)
		{
			/*动态的远端标识不一致*/
			BFD_DEBUG("--->bfd_disc check fail!!\n");
			goto out;
		}
	}
	else
	{
		/*第一次收到对端的控制报文*/
		psess->remote_id = bfdmsg->local_disc;
	}

	if(psess->poll == 1)
	{
		if(bfdmsg->final)
		{
			bfd_recv_final(psess, bfdmsg);
			goto out;
		}
	}

	if(bfdmsg->poll)
	{
		bfd_recv_poll(psess, bfdmsg);
		goto out;
	}
	if(bfdmsg->sta == BFD_STATUS_ADMINDOWN)
	{
		psess->bfd_event = BFD_EVENT_RECV_ADMINDOWN;
	}
	else if(bfdmsg->sta == BFD_STATUS_DOWN)
	{
		psess->bfd_event = BFD_EVENT_RECV_DOWN;
	}
	else if(bfdmsg->sta == BFD_STATUS_INIT)
	{
		local_id = psess->local_id ? psess->local_id : psess->local_id_cfg;
		if(local_id != bfdmsg->remote_disc)
		{
			goto out;
		}
		psess->bfd_event = BFD_EVENT_RECV_INIT;
	}
	else
	{
		psess->bfd_event = BFD_EVENT_RECV_UP;
	}
	psess->remote_recv_interval = bfdmsg->min_rx_interval;
	psess->remote_send_interval = bfdmsg->min_tx_interval;
	psess->remote_multiplier = bfdmsg->detect_mult;
	psess->remote_state = bfdmsg->sta;
	bfd_fsm(psess->bfd_event, psess);
out:
	return ;
}

void bfd_alarm_process(enum IPC_OPCODE opcode, uint32_t sess_id)
{
	struct gpnPortInfo gPortInfo;

    BFD_DEBUG();
	
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_BFD_TYPE;
	gPortInfo.iMsgPara1 = sess_id;

    if (IPC_OPCODE_CLEAR == opcode)
    {
        ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_BFD_SES_DOWN, GPN_SOCK_MSG_OPT_CLEAN);
    }
    else if (IPC_OPCODE_ADD == opcode)
    {
    	ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_BFD_SES_DOWN, GPN_SOCK_MSG_OPT_RISE);
    }

}

/* bfd 状态机变化 */
void bfd_fsm(enum BFD_EVENT event, struct bfd_sess *psess)
{
	uint16_t sess_id = 0;
	char retry_timer_name[NAME_STRING_LEN];
	char cc_timer_name[NAME_STRING_LEN];

    BFD_DEBUG();

	if(NULL == psess)
	{
		BFD_DEBUG("--->psess is NULL\n");
		return;
	}

	bfd_fsm_start_debug(event, psess);

	sess_id = psess->session_id_cfg ? psess->session_id_cfg : psess->session_id;

	memset(retry_timer_name, 0, NAME_STRING_LEN);
	memset(cc_timer_name, 0, NAME_STRING_LEN);
	snprintf(retry_timer_name, NAME_STRING_LEN, "BFD_SEND_TIMER_ID_%d", sess_id);
	snprintf(cc_timer_name, NAME_STRING_LEN, "BFD_CHECK_TIMER_ID_%d", sess_id);
	
	/*会话类型*/
	if(psess->type == BFD_TYPE_INVALID)
	{
		switch(event)
		{
			case BFD_EVENT_BIND_IF:
				psess->type = BFD_TYPE_INTERFACE;
				psess->status = BFD_STATUS_DOWN;
				gbfd.down_count++;
				/*进入协商阶段*/
				psess->retry_num = 0;
				BFD_THREAD_TIMER_OFF(psess->retry_timer);
				psess->retry_timer = BFD_THREAD_TIMER_ADD(retry_timer_name, LIB_TIMER_TYPE_LOOP, bfd_send_retry, (void *)psess, (psess->init_interval)/1000);
				break;
			case BFD_EVENT_BIND_PW:
				psess->type = BFD_TYPE_PW;
				psess->status = BFD_STATUS_DOWN;
				gbfd.down_count++;
				/*进入协商阶段*/
				psess->retry_num = 0;
				BFD_THREAD_TIMER_OFF(psess->retry_timer);
				psess->retry_timer = BFD_THREAD_TIMER_ADD(retry_timer_name, LIB_TIMER_TYPE_LOOP, bfd_send_retry, (void *)psess, (psess->init_interval)/1000);
				break;
			case BFD_EVENT_BIND_LSP:
				psess->type = BFD_TYPE_LSP;
				psess->status = BFD_STATUS_DOWN;
				gbfd.down_count++;
				/*进入协商阶段*/
				psess->retry_num = 0;
				BFD_THREAD_TIMER_OFF(psess->retry_timer);
				psess->retry_timer = BFD_THREAD_TIMER_ADD(retry_timer_name, LIB_TIMER_TYPE_LOOP, bfd_send_retry, (void *)psess, (psess->init_interval)/1000);
				break;
			case BFD_EVENT_BIND_TUNNEL:
				psess->type = BFD_TYPE_TUNNEL;
				psess->status = BFD_STATUS_DOWN;
				gbfd.down_count++;
				/*进入协商阶段*/
				psess->retry_num = 0;
				BFD_THREAD_TIMER_OFF(psess->retry_timer);
				psess->retry_timer = BFD_THREAD_TIMER_ADD(retry_timer_name, LIB_TIMER_TYPE_LOOP, bfd_send_retry, (void *)psess, (psess->init_interval)/1000);
				break;
			case BFD_EVENT_BIND_IF_IP:
				psess->type = BFD_TYPE_IP;
				psess->status = BFD_STATUS_DOWN;
				gbfd.down_count++;
				psess->retry_num = 0;
				BFD_THREAD_TIMER_OFF(psess->retry_timer);
				psess->retry_timer = BFD_THREAD_TIMER_ADD(retry_timer_name, LIB_TIMER_TYPE_LOOP, bfd_send_retry, (void *)psess, (psess->init_interval)/1000);
				break;
			case BFD_EVENT_SESSION_DELETE:
				bfd_session_delete(sess_id);
				break;
			default:
				break;
		}
	}
	else if(psess->type == BFD_TYPE_INTERFACE || psess->type == BFD_TYPE_PW || psess->type == BFD_TYPE_LSP || psess->type == BFD_TYPE_IP)
	{
		/*会话与端口已经绑定*/
		switch(event)
		{
			case BFD_EVENT_DISABLE:
				/*会话进入down并发送down报文，记录诊断原因*/
				if(psess->status == BFD_STATUS_UP)
				{
					psess->diag = BFD_DIAG_ADMIN_DOWN;
					psess->status = BFD_STATUS_ADMINDOWN;
					psess->down_count++;
					gbfd.up_count--;

					/*状态更新*/
					bfd_send_hal(psess, IPC_OPCODE_UPDATE, 0);
				}
				else if(psess->status == BFD_STATUS_DOWN || psess->status == BFD_STATUS_INIT)
				{
					gbfd.down_count--;
					/*若删除会话的流程中再调用了disable,不再清除计数*/

					bfd_alarm_process(IPC_OPCODE_CLEAR, sess_id);
				}
				psess->status = BFD_STATUS_ADMINDOWN;
				
				bfd_session_disable(psess);
				break;
			case BFD_EVENT_RECV_DOWN:
			case BFD_EVENT_RECV_DOWN_POLL:
				if(psess->status == BFD_STATUS_UP)
				{
					/*会话进入down，发送down报文,记录诊断原因*/
					psess->diag = BFD_DIAG_NEIGHBOR_DOWN;
					psess->status = BFD_STATUS_DOWN;

					bfd_alarm_process(IPC_OPCODE_ADD, sess_id);

					psess->remote_id = 0;
					gbfd.up_count--;
					gbfd.down_count++;
					psess->down_count++;

					/*删除检测定时器*/
					psess->retry_num = 0;
					bfd_send_hal(psess, IPC_OPCODE_UPDATE, 0);

					if(0 != psess->ldp_flag)
					{
						bfd_state_notify(BFD_STATUS_DOWN, psess);
						break;
					}
                    bfd_state_notify(BFD_STATUS_DOWN, psess);

					//删除防状态停滞定时器
					BFD_THREAD_TIMER_OFF(psess->cc_timer);

					/*立即发送一个down报文，进入到协商阶段*/
					BFD_THREAD_TIMER_OFF(psess->retry_timer);
					psess->retry_timer = BFD_THREAD_TIMER_ADD(retry_timer_name, LIB_TIMER_TYPE_LOOP, bfd_send_retry, (void *)psess, (psess->init_interval)/1000);

				}
				else if(psess->status == BFD_STATUS_DOWN)
				{
					/*会话进入init，发送init*/
					psess->status = BFD_STATUS_INIT;
					psess->retry_num = 0;
					if(0 == psess->retry_timer)
					{
						psess->retry_timer = BFD_THREAD_TIMER_ADD(retry_timer_name, LIB_TIMER_TYPE_LOOP, bfd_send_retry, (void *)psess, (psess->init_interval)/1000);
					}
					if(event == BFD_EVENT_RECV_DOWN_POLL)
					{
						bfd_send_final(psess);
					}
				}
				else if(psess->status == BFD_STATUS_INIT)
				{
					/*保持init*/
					if(event == BFD_EVENT_RECV_DOWN_POLL)
					{
						bfd_send_final(psess);
					}

					if(0 == psess->retry_timer)
					{
						psess->retry_timer = BFD_THREAD_TIMER_ADD(retry_timer_name, LIB_TIMER_TYPE_LOOP, bfd_send_retry, (void *)psess, (psess->init_interval)/1000);
					}
				}
				/*当前配置下会话没有admindown状态出现*/
				break;
			case BFD_EVENT_RECV_INIT:
			case BFD_EVENT_RECV_INIT_POLL:
				/*进入up状态，发送up报文*/
				if(psess->status == BFD_STATUS_INIT || psess->status == BFD_STATUS_DOWN)
				{
					psess->status = BFD_STATUS_UP;
					psess->poll_event = BFD_EVENT_FIRST_UP;

					bfd_alarm_process(IPC_OPCODE_CLEAR, sess_id);

					bfd_send_poll(psess, psess->poll_event);

					psess->cc_interval_send = MAX(psess->remote_recv_interval,
						psess->cc_interval_cfg ? psess->cc_interval_cfg : gbfd.cc_interval);
					psess->cc_interval_recv = MAX(psess->remote_send_interval,
						psess->cc_interval_cfg ? psess->cc_interval_cfg : gbfd.cc_interval);
					gbfd.up_count++;
					gbfd.down_count--;
					psess->up_count++;
					psess->retry_num = 0;
					//开启检测定时器，防止bfd状态收不到f置位报文，状态永久性停留；
					bfd_up_timeout[sess_id - 1] = 1;
					bfd_up_wait_counter[sess_id - 1] = 30;
					BFD_THREAD_TIMER_OFF(psess->cc_timer);
					psess->cc_timer = BFD_THREAD_TIMER_ADD(cc_timer_name, LIB_TIMER_TYPE_LOOP, bfd_cc_timeout, (void *)psess, 1000);
				}
				if(event == BFD_EVENT_RECV_INIT_POLL)
				{
					bfd_send_final(psess);
				}
				break;
			case BFD_EVENT_RECV_FIRST_UP:
			case BFD_EVENT_RECV_UP:
				if(psess->status == BFD_STATUS_DOWN || psess->status == BFD_STATUS_ADMINDOWN)
				{
					psess->remote_id = 0;
					break;
				}
				if(event == BFD_EVENT_RECV_FIRST_UP)
				{
					psess->cc_interval_send = MAX(psess->remote_recv_interval,
						psess->cc_interval_cfg ? psess->cc_interval_cfg : gbfd.cc_interval);
					psess->cc_interval_recv = MAX(psess->remote_send_interval,
						psess->cc_interval_cfg ? psess->cc_interval_cfg : gbfd.cc_interval);
				}
				
				if(psess->status == BFD_STATUS_INIT)
				{
					gbfd.up_count++;
					gbfd.down_count--;
					psess->up_count++;
					psess->status = BFD_STATUS_UP;

					bfd_alarm_process(IPC_OPCODE_CLEAR, sess_id);

					psess->poll_event = BFD_EVENT_FIRST_UP;
					bfd_send_poll(psess, psess->poll_event);
					psess->retry_num = 0;
					//开启检测定时器，防止bfd状态收不到f置位报文，状态永久性停留；
					bfd_up_timeout[sess_id - 1] = 1;
					bfd_up_wait_counter[sess_id - 1] = 30;
					BFD_THREAD_TIMER_OFF(psess->cc_timer);
					psess->cc_timer = BFD_THREAD_TIMER_ADD(cc_timer_name, LIB_TIMER_TYPE_LOOP, bfd_cc_timeout, (void *)psess, 1000);
				}
				else if(psess->status == BFD_STATUS_UP)
				{
					psess->retry_cc = 0;
				}
				
				if(event == BFD_EVENT_RECV_FIRST_UP)
				{
					bfd_send_final(psess);

					if(psess->poll == 0 && psess->status == BFD_STATUS_UP)
					{
						/*收到对端首包，并且本地首包得到确认，下发硬件状态机*/
						BFD_THREAD_TIMER_OFF(psess->retry_timer);
						bfd_send_hal(psess, IPC_OPCODE_UPDATE, 0);
						bfd_state_notify(psess->status, psess);

						BFD_THREAD_TIMER_OFF(psess->cc_timer);
						bfd_up_timeout[sess_id - 1] = 0;
						bfd_up_wait_counter[sess_id - 1] = 30;
					}
				}
				break;
			case BFD_EVENT_RECV_ADMINDOWN:
				if(psess->status == BFD_STATUS_UP)
				{
					psess->diag = BFD_DIAG_NEIGHBOR_DOWN;
					psess->status = BFD_STATUS_DOWN;

					bfd_alarm_process(IPC_OPCODE_CLEAR, sess_id);

					gbfd.up_count--;
					gbfd.down_count++;
					psess->down_count++;
					/*删除状态机*/
					bfd_send_hal(psess, IPC_OPCODE_UPDATE, 0);
				}
				psess->status = BFD_STATUS_DOWN;
				psess->remote_id = 0;
				psess->poll = 0;
				/*立即发送down报文*/
				psess->retry_num = 0;
				
				BFD_THREAD_TIMER_OFF(psess->cc_timer);
				/*init报文不需要更新检测定时器*/				
				BFD_THREAD_TIMER_OFF(psess->retry_timer);				
				psess->retry_timer = BFD_THREAD_TIMER_ADD(retry_timer_name, LIB_TIMER_TYPE_LOOP, bfd_send_retry, (void *)psess, (psess->init_interval)/1000);
				
				break;
			case BFD_EVENT_SESSION_DELETE:
				/*会话删除，发送admindown报文*/
				if(psess->status == BFD_STATUS_UP)
				{
					gbfd.up_count--;
				}
				else
				{
					gbfd.down_count--;
					/*pw lsp在解绑的时候将计数清除*/
				}
				psess->status = BFD_STATUS_ADMINDOWN;
				psess->diag = BFD_DIAG_ADMIN_DOWN;

				bfd_alarm_process(IPC_OPCODE_CLEAR, sess_id);
				/*删除状态机*/
				bfd_send_hal(psess, IPC_OPCODE_UPDATE, 0);
				//bfd_send(psess);
				bfd_session_delete(sess_id);
				break;
			case BFD_EVENT_INIT_TIMEOUT:
				if(BFD_STATUS_UP == psess->status)
				{
					gbfd.up_count--;
					gbfd.down_count++;
					psess->down_count++;
				}
				psess->status = BFD_STATUS_DOWN;

				bfd_alarm_process(IPC_OPCODE_ADD, sess_id);

				psess->poll = 0;
				psess->final = 0;
				psess->remote_state = 0;
				psess->remote_id = 0;
				BFD_THREAD_TIMER_OFF(psess->cc_timer);
				BFD_THREAD_TIMER_OFF(psess->retry_timer);				
				psess->retry_timer = BFD_THREAD_TIMER_ADD(retry_timer_name, LIB_TIMER_TYPE_LOOP, bfd_send_retry, (void *)psess, (psess->init_interval)/1000);
				break;
			case BFD_EVENT_TIMEOUT:
				if(psess->status == BFD_STATUS_UP)
				{
					gbfd.up_count--;
					gbfd.down_count++;
					psess->down_count++;
					psess->status = BFD_STATUS_DOWN;
					psess->diag = BFD_DIAG_DETECT_EXPIRE;
					psess->remote_id = 0;
					bfd_send_hal(psess, IPC_OPCODE_UPDATE, 0);
					//bfd_state_notify(BFD_STATUS_DOWN, psess);
				}

				bfd_alarm_process(IPC_OPCODE_ADD, sess_id);

				if(0 != psess->ldp_flag)
				{
					bfd_state_notify(BFD_STATUS_DOWN, psess);
					break;
				}
				bfd_state_notify(BFD_STATUS_DOWN, psess);

				psess->status = BFD_STATUS_DOWN;
				psess->diag = BFD_DIAG_DETECT_EXPIRE;
				psess->remote_id = 0;
				BFD_THREAD_TIMER_OFF(psess->cc_timer);
				BFD_THREAD_TIMER_OFF(psess->retry_timer);				
				/*立即发送一个down，重新进入协商阶段*/
				psess->retry_timer = BFD_THREAD_TIMER_ADD(retry_timer_name, LIB_TIMER_TYPE_LOOP, bfd_send_retry, (void *)psess, (psess->init_interval)/1000);
				break;
			case BFD_EVENT_ENABLE:
				/*会话不能重复被绑定到接口*/
				break;
			default:
				break;
		}
	}
	bfd_fsm_end_debug(psess);
	return ;
}

