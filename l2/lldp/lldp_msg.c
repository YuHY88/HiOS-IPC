
/*
*  Copyright (C) 2016  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: lldp_msg.c	
*
*  date: 2016.8.11
*
*  modify:	2018.3.12 modified by liufuying to make LLDP module code beautiful
*
*/

#include <stdio.h>
#include <string.h>

#include "lib/module_id.h"
#include "pkt_type.h"
#include "lib/pkt_buffer.h"
#include "lib/thread.h"
#include "command.h"
#include "l2/l2_if.h"
#include "lib/hash1.h"
#include "lib/ifm_common.h"
#include "lib/if.h"


#include "lldp.h"
#include "lldp_msg.h"
#include "lldp_rx_sm.h"
#include "l2/efm/efm_agent.h"

extern struct thread_master		*l2_master;
extern lldp_global				lldp_global_msg;


void lldp_pkt_register()	
{
	union proto_reg proto;

	memset(&proto, 0, sizeof(union proto_reg));

	proto.ethreg.dmac[0] = 0x01;
	proto.ethreg.dmac[1] = 0x80;
	proto.ethreg.dmac[2] = 0xC2;
	proto.ethreg.dmac[3] = 0x00;
	proto.ethreg.dmac[4] = 0x00;
	proto.ethreg.dmac[5] = 0x0E;

	proto.ethreg.dmac_valid = 1;	
	proto.ethreg.ethtype = 0x88CC;

	pkt_register(MODULE_ID_L2, PROTO_TYPE_ETH, &proto);	

	return;
}


void lldp_pkt_rcv(struct l2if	*pif, struct pkt_buffer *pkt)
{	
	struct lldp_port	*lldp_port = NULL;
    uint32_t sub_ifindex = 0 ; 
    struct ifm_info subif_info;
	int ret = 0;

	if((NULL == pif) || (NULL == pkt))
	{
		return;
	}

	lldp_port = pif->lldp_port_info;
	if(lldp_port != NULL)
	{
		memcpy(&lldp_port->rx.frame[0], pkt->cb.ethcb.dmac, 6);
		memcpy(&lldp_port->rx.frame[6], pkt->cb.ethcb.smac, 6);

		/*add svlan msg*/
		/*if svlan_tpid or svlan which one is 0 , we think the value is invalid*/
		if((pkt->cb.ethcb.svlan_tpid != 0)
			&& (pkt->cb.ethcb.svlan != 0))
		{
			lldp_port->rx.frame[12] = (unsigned char)(pkt->cb.ethcb.svlan_tpid >> 8);
			lldp_port->rx.frame[13] = (unsigned char)pkt->cb.ethcb.svlan_tpid;
			lldp_port->rx.frame[14] = (unsigned char)(pkt->cb.ethcb.svlan >> 8);
			lldp_port->rx.frame[15] = (unsigned char)pkt->cb.ethcb.svlan;
		}
		else
		{
			lldp_port->rx.frame[12] = 0x00;
			lldp_port->rx.frame[13] = 0x00;
			lldp_port->rx.frame[14] = 0x00;
			lldp_port->rx.frame[15] = 0x00;
		}
		
		/*add cvlan msg*/
		/*if cvlan_tpid or cvlan which one is 0 , we think the value is invalid*/
		if((pkt->cb.ethcb.cvlan_tpid != 0)
			&& (pkt->cb.ethcb.cvlan != 0))
		{
			lldp_port->rx.frame[16] = (unsigned char)(pkt->cb.ethcb.cvlan_tpid >> 8);
			lldp_port->rx.frame[17] = (unsigned char)pkt->cb.ethcb.cvlan_tpid;
			lldp_port->rx.frame[18] = (unsigned char)(pkt->cb.ethcb.cvlan >> 8);
			lldp_port->rx.frame[19] = (unsigned char)pkt->cb.ethcb.cvlan;
		}
		else
		{
			lldp_port->rx.frame[16] = 0;
			lldp_port->rx.frame[17] = 0;
			lldp_port->rx.frame[18] = 0;
			lldp_port->rx.frame[19] = 0;
		}

		lldp_port->rx.frame[20] = (unsigned char)(pkt->protocol >> 8);
		lldp_port->rx.frame[21] = (unsigned char)pkt->protocol; 				
			
		memcpy(&lldp_port->rx.frame[22], pkt->data, pkt->data_len);

#if 0
	int i;
	for(i = 0; i < 22+pkt->data_len; i++)
	{
		printf(" %02x", lldp_port->rx.frame[i]);
		if(i % 9 == 0)
			printf("\n");
	}
	printf("\n");
#endif

		lldp_port->rx.recvsize = pkt->data_len + 22;
		lldp_port->rx.rcvFrame = LLDP_TRUE;
		lldp_port->l3_vpnid = pkt->vpn;
		lldp_port->pkt_ifindex = pkt->in_ifindex;

        if (lldp_port->arp_nd_learning_vlan !=0)
        {            
            sub_ifindex = IFM_SUBIFINDEX_GET(pkt->in_ifindex, lldp_port->arp_nd_learning_vlan);

            ret =  ifm_get_all_info(sub_ifindex , MODULE_ID_L2, &subif_info) ;

            if (!ret && IFNET_LINKUP == subif_info.status )
            {          
                 lldp_port->pkt_ifindex = sub_ifindex ;
            }            
        }                
        lldp_rx_state_machine_action(lldp_port);
    }
    return;
}


void lldp_pkt_rcv_efm_agent(uint32_t ifindex, efm_agent_info *aefm)
{
	struct l2if *pif = NULL;
	struct lldp_port *lldp_port = NULL;
	//efm_agent_remote_SetGet_info * aefm_RsetGetInfo;
	char * agent_data = NULL;
	const char *port_name = "gigabitethernet 1/1/1";
	uint8_t L = 0,M =0,N = 0,P = 0;
	char device_name[20] = {0};
	char sys_desc[50] = {0};

	if (NULL == aefm && NULL == aefm->aefm_RsetGetInfo)
	{
		
		return;
	}
	
	pif = l2if_lookup(ifindex);
	if(NULL == pif)
	{
		return;
	}

	/*return local port value*/
	lldp_port = pif->lldp_port_info;
	if (NULL == lldp_port)
	{
		return;
	}

	agent_data = (char *)aefm->aefm_RsetGetInfo->efm_agent_remote_SetGet_data;
	

	//lldp dmac: 01 80 c2 00 00 0e
	lldp_port->rx.frame[ETH_HEAD_ADDR] = LLDP_DEST_MAC_0;
	lldp_port->rx.frame[ETH_HEAD_ADDR+1] = LLDP_DEST_MAC_1;
	lldp_port->rx.frame[ETH_HEAD_ADDR+2] = LLDP_DEST_MAC_2;
	lldp_port->rx.frame[ETH_HEAD_ADDR+3] = LLDP_DEST_MAC_3;
	lldp_port->rx.frame[ETH_HEAD_ADDR+4] = LLDP_DEST_MAC_4;
	lldp_port->rx.frame[ETH_HEAD_ADDR+5] = LLDP_DEST_MAC_5;

	
	//lldp smac: agent device mac
	memcpy(&lldp_port->rx.frame[ETH_HEAD_ADDR+6], &agent_data[efm_remote_set_6hpmcrtMacAddr], MAC_LEN);

	memset(&lldp_port->rx.frame[ETH_HEAD_ADDR+12],0,8);
	#if 0
	lldp_port->rx.frame[12] = 0x00;
	lldp_port->rx.frame[13] = 0x00;
	lldp_port->rx.frame[14] = 0x00;
	lldp_port->rx.frame[15] = 0x00;
	lldp_port->rx.frame[16] = 0;
	lldp_port->rx.frame[17] = 0;
	lldp_port->rx.frame[18] = 0;
	lldp_port->rx.frame[19] = 0;
	#endif
	lldp_port->rx.frame[ETH_HEAD_ADDR+20] = 0x88;
	lldp_port->rx.frame[ETH_HEAD_ADDR+21] = 0xcc; 	

	//lldp data	
	//memcpy(&lldp_port->rx.frame[22], pkt->data, pkt->data_len);
	//lldp_rx_process_frm();
	//lldp_encap_frame();
	
	/*CHASSIS_ID_TLV, subtype is MAC 22-30*/
	  lldp_port->rx.frame[LLDP_CH_TLV_ADDR] = 0x02;//tlv type:chassis id 1
	  lldp_port->rx.frame[LLDP_CH_TLV_ADDR+1] = 0x07;//tlv len: 0x7
	  lldp_port->rx.frame[LLDP_CH_TLV_ADDR+2] = CHASSIS_ID_MAC_ADDRESS;//chassis id subtype: mac

	  //25-30 remote device mac, 6 bytes
	  memcpy(&lldp_port->rx.frame[LLDP_CH_TLV_ADDR+3], &agent_data[efm_remote_set_6hpmcrtMacAddr], MAC_LEN);

	
	  /*PORT_ID_TLV 31*/
	  lldp_port->rx.frame[LLDP_PORT_TLV_ADDR] = 0x04;//tlv type: port id 2
	  lldp_port->rx.frame[LLDP_PORT_TLV_ADDR+1] = strlen(port_name) + 1;//tlv len: 0x16 len=(sizeof(port_name) + 1)
	  lldp_port->rx.frame[LLDP_PORT_TLV_ADDR+2] = PORT_ID_INTERFACE_NAME;//tlv len: 0x16
	  //33-? port id: remote port name, string "gigabitethernet 1/1/1", 21 bytes ge1
	  memcpy(&lldp_port->rx.frame[LLDP_PORT_TLV_ADDR+3],port_name,strlen(port_name));

	  L = strlen(port_name);
	
	  /*TIME_TO_LIVE_TLV*/
	  lldp_port->rx.frame[LLDP_TTL_TLV_ADDR(L)] = 0x06;//tlv type: ttl 3
	  lldp_port->rx.frame[LLDP_TTL_TLV_ADDR(L)+1] = 0x02;//tlv len: 0x2
	  lldp_port->rx.frame[LLDP_TTL_TLV_ADDR(L)+2] = 0x00;//tlv data: 0x0078
	  lldp_port->rx.frame[LLDP_TTL_TLV_ADDR(L)+3] = 0x78;//tlv data: 0x0078
	
	  /*port description tlv*/
	  lldp_port->rx.frame[LLDP_TTL_TLV_ADDR(L)+4] = 0x08;//tlv type: port description 4
	  lldp_port->rx.frame[LLDP_TTL_TLV_ADDR(L)+5] = strlen(port_name);//tlv len: 0x15 len=sizeof(port_name)
	  //len+
	  //60-80 port description: remote port name, string "gigabitethernet 1/1/1" , 21 bytes
	  memcpy(&lldp_port->rx.frame[LLDP_TTL_TLV_ADDR(L)+6],port_name,strlen(port_name));
	  M = strlen(port_name)+L;
	  
	  /*system name tlv*/
	  lldp_port->rx.frame[LLDP_SYS_TLV_ADDR(M)] = 0x0a;//tlv type: system name 5
	  #if 0
	   switch (aefm->efm_agent_remote_device_tid)
	{
		case EFM_REMOTE_11000A_TYPE:
			 memcpy(device_name,"11000A",strlen("11000A"));
			break;		
		case EFM_REMOTE_1101n_TYPE:
			memcpy(device_name,"1101n",strlen("1101n"));
			break;
		case EFM_REMOTE_11000An_TYPE:
			memcpy(device_name,"11000An",strlen("11000An"));
			break;
		default:
			break;
	}	  
	   #endif
	    memcpy(device_name,"H20RN-161-S",strlen("H20RN-161-S"));
	  lldp_port->rx.frame[LLDP_SYS_TLV_ADDR(M)+1] = strlen(device_name);//tlv len: 0x9(if system name is "undefined") len=sizeof(device name)
	  //83 + sizeof (device name)
	 
	  memcpy( &lldp_port->rx.frame[LLDP_SYS_TLV_ADDR(M)+2],device_name,strlen(device_name));
	  N = strlen(device_name) + M;
	  
	
	  //system description tlv
	  lldp_port->rx.frame[LLDP_SYS_TLV_ADDR(N)+2] = 0x0c; //tlv type:system description 6
	  sprintf(sys_desc,"[%s]Fiber optic transceiver",device_name);
	  lldp_port->rx.frame[LLDP_SYS_TLV_ADDR(N)+3] = /*0x09*/ strlen(sys_desc); //tlv len: sizeof("undefined") = 0x9;
	  //83 + sizeof(device_name) + 2 to 83 + sizeof(device_name) + 2 + 9 is data:undefined
	  memcpy( &lldp_port->rx.frame[LLDP_SYS_TLV_ADDR(N)+4],sys_desc,strlen(sys_desc));
	  P = strlen(sys_desc) + N;
	  
	  //capabilities tlv
	  lldp_port->rx.frame[LLDP_SYS_TLV_ADDR(P)+4] = 0x0e; //tlv type:system capabilities tlv 7
	  lldp_port->rx.frame[LLDP_SYS_TLV_ADDR(P)+5] = 0x04;//tlv len 4
	  lldp_port->rx.frame[LLDP_SYS_TLV_ADDR(P)+6] = 0x00;//tlv data:0x0014
	  lldp_port->rx.frame[LLDP_SYS_TLV_ADDR(P)+7] = 0x14;//tlv data:0x0014
	  lldp_port->rx.frame[LLDP_SYS_TLV_ADDR(P)+8] = 0x00;//tlv data:0x0014
	  lldp_port->rx.frame[LLDP_SYS_TLV_ADDR(P)+9] = 0x14;//tlv data:0x0014
	
	  //management address tlv
	  lldp_port->rx.frame[LLDP_SYS_TLV_ADDR(P)+10] = 0x10;//tlv type:management tlv 8
	  lldp_port->rx.frame[LLDP_SYS_TLV_ADDR(P)+11] = 0x0c;//tlv len: 12
	  lldp_port->rx.frame[LLDP_SYS_TLV_ADDR(P)+12] = 0x05;//address string len: 5
	  lldp_port->rx.frame[LLDP_SYS_TLV_ADDR(P)+13] = 0x01;//address subtype:1 ipv4, maybe subtype is mac,2
	  /*update ip*/
	  lldp_port->rx.frame[LLDP_SYS_TLV_ADDR(P)+14] = aefm->efm_agent_update_ip[0];
	  
	  lldp_port->rx.frame[LLDP_SYS_TLV_ADDR(P)+15] = aefm->efm_agent_update_ip[1];
	  lldp_port->rx.frame[LLDP_SYS_TLV_ADDR(P)+16] = aefm->efm_agent_update_ip[2];
	  
	  lldp_port->rx.frame[LLDP_SYS_TLV_ADDR(P)+17] = aefm->efm_agent_update_ip[3];
	
	  //end tlv
	  lldp_port->rx.frame[LLDP_SYS_TLV_ADDR(P)+18] = 0x00;//tlv type: end of lldpdu 0 
	  lldp_port->rx.frame[LLDP_SYS_TLV_ADDR(P)+19] = 0x00;//tlv len = 0



	lldp_port->rx.recvsize = LLDP_SYS_TLV_ADDR(P)+20;
	lldp_port->rx.rcvFrame = LLDP_TRUE;
	lldp_port->l3_vpnid = 0;
	lldp_port->pkt_ifindex = ifindex;

	lldp_rx_state_machine_action(lldp_port);

}

