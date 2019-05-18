#include <string.h>

#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>
#include <lib/thread.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/log.h>
#include <lib/zassert.h>
#include <lib/ifm_common.h>
#include <lib/errcode.h>
#include <lib/module_id.h>
#include <ftm/pkt_tcp.h>
#include <ftm/pkt_udp.h>
#include <ftm/pkt_ip.h>
#include "pim_pkt.h"
#include "pim_neighbor.h"
#include "pim_join_prune.h"
#include "pim_register.h"

/*从ftm接收到PIM协议报文*/
sint32 pim_pkt_recv(struct pkt_buffer *ppkt)
{
	struct ip_control *ipcb = NULL;
	struct pim_hdr *pimhdr = NULL;
	struct ipmc_if *pif = NULL;
	uint32_t sip = 0, dip = 0;
	uint8_t pim_type = 0;
	uint32_t ifindex = 0;
	uint8_t *pim_para = NULL;
	int ret = 0, pim_para_len = 0; 
	
	/*处理不同类型的PIM报文*/
	ipcb = (struct ip_control *)&(ppkt->cb);
	pimhdr = (struct pim_hdr *)ppkt->data;
	
	if(ipcb->protocol != IP_P_PIM || pimhdr->pim_vers != PIM_VERSION || pimhdr->pim_reserved != 0)
	{
		return ERRNO_FAIL;
	}
	pim_type = pimhdr->pim_types;
	sip = ipcb->sip;
	dip = ipcb->dip;
	if(ppkt->inif_type == PKT_INIF_TYPE_IF)
	{
		ifindex = ppkt->in_port;
	}
	pif = ipmc_if_lookup(ifindex);
	/*注册报文，注册停止报文，CRP宣告报文为单播报文，接口不需要进行PIM检查*/
	if(pimhdr->pim_types != PIM_REGISTER &&
		pimhdr->pim_types != PIM_REGISTER_STOP &&
		pimhdr->pim_types != PIM_CAND_RP_ADV)
	{
		/*根据ifindex检查接口是否使能PIM*/
		if(pif == NULL)
		{
			zlog_notice("%s[%d]: pif 0x%x not enable pim\n", __FILE__, __LINE__, ifindex);
			return ERRNO_FAIL;
		}
	}
	/* 去掉 pim header 长度, ppkt->data 指向pim_parameters*/
	ret = pkt_pull(ppkt, PIM_HDR_LEN); 
	if(ret) 
	{
		zlog_debug(0, "pkt_pull failed\n");
		return ERRNO_FAIL;
	}
	//pim_para = (uint8_t *)ppkt->data + sizeof(struct pim_hdr);
	//pim_para_len = ppkt->data_len - sizeof(struct pim_hdr);
	pim_para = (uint8_t *)ppkt->data;
	pim_para_len = ppkt->data_len;
	switch(pimhdr->pim_types)
	{
		case PIM_HELLO:
			if(pif)
			{
				pim_hello_recv(pif, sip, pim_para, pim_para_len);
			}
			break;
		case PIM_REGISTER:
			pim_register_recv(dip, sip, ipcb->tos, pim_para, pim_para_len);
			break;
		case PIM_REGISTER_STOP:
			pim_register_stop_recv(dip, sip, pim_para, pim_para_len);
			break;
		case PIM_JOIN_PRUNE:
			if(pif)
			{
				pim_jp_recv(pif, sip, pim_para, pim_para_len);
			}
			break;
		default:
			break;
	}
	
	return ERRNO_SUCCESS;
}

/*全局组播功能开启时注册*/
void pim_pkt_register(void)
{
    union proto_reg proto;

	memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_PIM;
	proto.ipreg.dip = 0xe000000d;
    pkt_register(MODULE_ID_IPMC, PROTO_TYPE_IPV4, &proto);
	
	/*所有的PIM报文，包括单播报文和组播报文*/
    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_PIM;
    pkt_register(MODULE_ID_IPMC, PROTO_TYPE_IPV4, &proto);
}

void pim_pkt_unregister(void)
{
    union proto_reg proto;
	
    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_PIM;
    pkt_unregister(MODULE_ID_IPMC, PROTO_TYPE_IPV4, &proto);
}
