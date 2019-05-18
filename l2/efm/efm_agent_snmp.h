#ifndef _EFM_AGENT_SNMP_H_
#define _EFM_AGENT_SNMP_H_
#include "efm.h"
#include <lib/vty.h>
#define IP_NUM 4
#define MAC_NUM 6
#define BITS_NUM 8
#define LM(V,BIT) (((V)>>(BIT))&0x01)

#define PORT_NUM 1             //maybe to change

enum efm_agent_snmp_info_type
{
	EFM_COMM_INFO_TYPE,
	EFM_OLTPT_INFO_TYPE,
	EFM_ETHPT_INFO_TYPE,	
	EFM_OLTPTALM_INFO_TYPE,
	EFM_ETHPTALM_INFO_TYPE,
	EFM_VLAN_INFO_TYPE,
	EFM_OLTVLAN_INFO_TYPE,
	EFM_ETHVLAN_INFO_TYPE,
	EFM_CPUVLAN_INFO_TYPE,
	EFM_VLANGROUP_INFO_TYPE,
	EFM_INVALID_INFO_TYPE,
	EFM_QINQ_INFO_TYPE,
	EFM_QOS_INFO_TYPE,
	EFM_PERF_INFO_TYPE,
	EFM_OLTPERF_INFO_TYPE,
	EFM_ETHPERF_INFO_TYPE,
	EFM_CPUPERF_INFO_TYPE,
	EFM_OAM_INFO_TYPE,
	EFM_ADDR_INFO_TYPE,
	EFM_CONVER_INFO_TYPE,

};


struct efm_comm_info_data
{
	  uint8_t efm_SubType;		   
	  uint8_t efm_SoftVer;  
	  uint8_t efm_EthChNum; 
	  uint8_t efm_EthPtNum;
	  uint8_t efm_OltPtNum; 
	  uint8_t efm_HwSw;
	  uint8_t efm_SFI;
	
	  uint8_t efm_DataResum;
	  uint8_t efm_LFPCtrl;
	  uint8_t efm_ALSCtrl;
	  uint8_t efm_Jumbo;
	  uint8_t efm_Reboot;
	  uint8_t efm_IpCtrl;
	  uint8_t efm_EthStromfilter;
	  uint8_t efm_DHCPclient;
	
	  uint8_t efm_DipSwitchSta;
	uint8_t efm_HostName[VTYSH_HOST_NAME_LEN + 1];



};
struct efm_comm_info_snmp
{

	uint32_t ifindex;
	struct efm_comm_info_data efm_comm_info;

};


struct efm_oltpt_info_data
{
	uint8_t efm_Oltifindex;
	uint8_t efm_OltPtNegCtrl;
	uint8_t efm_OltPtSpdCtrl;
	uint8_t efm_OltPtDplCtrl;
	uint8_t efm_OltPtDisCtrl  ;
	uint32_t efm_OltPtIngressRateN;
	uint32_t efm_OltPtEgressRateN;
	uint8_t efm_OltPtFlowCtrl;

};

struct efm_oltpt_info_snmp
{
	uint32_t ifindex;
	struct efm_oltpt_info_data efm_oltpt_info;
};


struct efm_ethpt_info_data
{
	uint8_t efm_Ethifindex;
	uint8_t efm_EthPtNegCtrl;
	uint8_t efm_EthPtSpdCtrl ;
	uint8_t efm_EthPtDplCtrl ;
	uint8_t efm_EthPtDisCtrl;
	uint32_t efm_EthPtIngressRateN;
	uint32_t efm_EthPtEgressRateN;
	uint8_t efm_EthPtFlowCtrl;


};
struct efm_ethpt_info_snmp
{
	uint32_t ifindex;
	struct efm_ethpt_info_data efm_ethpt_info;

};


struct efm_oltptalm_info_data
{
	uint8_t efm_OltPtalmNO;
         uint8_t efm_OltPtalmStatInfo;
         uint8_t efm_OltPtalmAlmInfo;


};
struct efm_oltptalm_info_snmp
{
	uint32_t ifindex;
	struct efm_oltptalm_info_data efm_oltptalm_info;

};

struct efm_ethptalm_info_data
{
	uint8_t efm_EthPtNO;
         uint8_t efm_EthPtStatInfo;
         uint8_t efm_EthPtAlmInfo;


};
struct efm_ethptalm_info_snmp
{
	uint32_t ifindex;
	struct efm_ethptalm_info_data efm_ethptalm_info;

};
/*only one value */
struct efm_vlan_info_snmp
{
	uint32_t ifindex;
	uint8_t efm_VlanMode;

};
struct efm_oltvlan_info_data
{

	uint8_t efm_OltPtInMode;
	uint16_t efm_OltPtPVID;
         uint8_t efm_OltPtPrior;
         uint8_t efm_OltPtTagMode;


};
struct efm_oltvlan_info_snmp
{
	uint32_t ifindex;
	struct efm_oltvlan_info_data efm_oltvlan_info;

};

struct efm_ethvlan_info_data
{
	uint8_t efm_EthPtNo;
	uint8_t efm_EthPtInMode;
	uint32_t efm_EthPtPVID;
         uint8_t efm_EthPtPrior;
         uint8_t efm_EthPtTagMode;


};
struct efm_ethvlan_info_snmp
{
	uint32_t ifindex;
	struct efm_ethvlan_info_data efm_ethvlan_info;

};



struct efm_cpuvlan_info_data
{

	uint8_t efm_CpuPtInMode;
         uint32_t efm_CpuPtPVID ;
         uint8_t efm_CpuPtPrior ;
         uint8_t efm_CpuPtTagMode;


};
struct efm_cpuvlan_info_snmp
{
	uint32_t ifindex;
	struct efm_cpuvlan_info_data efm_cpuvlan_info;

};

struct efm_vlangroup_info_data
{

	uint8_t efm_VlanGroupNO;
	uint16_t efm_VlanId;
	uint8_t efm_VlanMember;



};
struct efm_vlangroup_info_snmp
{
	uint32_t ifindex;
	struct efm_vlangroup_info_data efm_vlangroup_info;

};
struct efm_invalid_info_snmp
{
	uint32_t ifindex;
	uint8_t invalid_data;

};

struct efm_qinq_info_data
{

	uint8_t efm_QinQDirection;
	uint32_t efm_QinQTPID;
	uint32_t efm_QinQVid;
         uint8_t efm_QinQPrior;

};
struct efm_qinq_info_snmp
{
	uint32_t ifindex;
	struct efm_qinq_info_data efm_qinq_info;

};

struct efm_qos_info_snmp
{
	uint32_t ifindex;
	uint8_t efm_1pQos;

};

struct efm_perf_info_snmp
{
	uint32_t ifindex;
	uint8_t efm_CleanCounter;

};
struct efm_oltperf_info_data
{

	uint8_t efm_OltCntType;
         uint32_t efm_OltRxPCnt ;
         uint32_t efm_OltTxPCnt;
         uint8_t efm_OltRxBytes[BITS_NUM];

};
struct efm_oltperf_info_snmp
{
	uint32_t ifindex;
	struct efm_oltperf_info_data efm_oltperf_info;

};
struct efm_ethperf_info_data
{
	uint8_t efm_EthPerfNo;
	uint8_t efm_EthCntType;
         uint32_t efm_EthRxPCnt ;
         uint32_t efm_EthTxPCnt;
         uint8_t efm_EthRxBytes[BITS_NUM];

};
struct efm_ethperf_info_snmp
{
	uint32_t ifindex;
	struct efm_ethperf_info_data efm_ethperf_info;

};


struct efm_cpuperf_info_data
{

	uint8_t efm_CpuCntType;
         uint32_t efm_CpuRxPCnt ;
         uint32_t efm_CpuTxPCnt;
         uint8_t efm_CpuRxBytes[BITS_NUM];

};
struct efm_cpuperf_info_snmp
{
	uint32_t ifindex;
	struct efm_cpuperf_info_data efm_cpuperf_info;

};

struct efm_oam_info_data
{
    uint8_t efm_OamMode;
    uint8_t efm_OamLpbkSta;	

};
struct efm_oam_info_snmp
{
	uint32_t ifindex;
	struct efm_oam_info_data efm_oam_info;
};


struct efm_addr_info_data
{
	uint8_t efm_IpAddr[IP_NUM];
	uint8_t efm_IpMask[IP_NUM];
	uint8_t efm_GatewayAddr[IP_NUM];
         uint8_t efm_MacAddr[MAC_NUM];


};
struct efm_addr_info_snmp
{
	uint32_t ifindex;
	struct efm_addr_info_data efm_addr_info;
};

struct efm_conver_info_snmp
{
	uint32_t ifindex;
	uint32_t efm_conver;
};

int 
efm_agent_get_comm_info(struct efm_comm_info_data  * efm_comm_info,struct efm_if * pefm);

int 
efm_agent_snmp_comm_info_get(uint32_t ifindex, struct efm_comm_info_snmp  efm_snmp_mux[]);

int 
efm_agent_snmp_vlan_info_get(uint32_t ifindex,struct efm_vlan_info_snmp  efm_snmp_mux[]);

int 
efm_agent_get_oltvlan_info(struct efm_oltvlan_info_data  * efm_oltvlan_info,efm_agent_remote_SetGet_info * efm_RsetGetInfo);

int 
efm_agent_snmp_oltvlan_info_get(uint32_t ifindex,struct efm_oltvlan_info_snmp  efm_snmp_mux[]);

int 
efm_agent_get_cpuvlan_info(struct efm_cpuvlan_info_data  * efm_cpuvlan_info,efm_agent_remote_SetGet_info * efm_RsetGetInfo);

int 
efm_agent_snmp_cpuvlan_info_get(uint32_t ifindex,struct efm_cpuvlan_info_snmp  efm_snmp_mux[]);

int 
efm_agent_get_qinq_info(struct efm_qinq_info_data  * efm_qinq_info,efm_agent_remote_SetGet_info * efm_RsetGetInfo);

int 
efm_agent_snmp_qinq_info_get(uint32_t ifindex,struct efm_qinq_info_snmp  efm_snmp_mux[]);

int 
efm_agent_snmp_qos_info_get(uint32_t ifindex,struct efm_qos_info_snmp  efm_snmp_mux[]);

int 
efm_agent_snmp_perf_info_get(uint32_t ifindex,struct efm_perf_info_snmp  efm_snmp_mux[]);

int 
efm_agent_snmp_oltperf_info_get(uint32_t ifindex,struct efm_oltperf_info_snmp  efm_snmp_mux[]);

int 
efm_agent_get_cpuperf_info(struct efm_cpuperf_info_data  * efm_cpuperf_info,efm_agent_info * aefm);

int 
efm_agent_snmp_cpuperf_info_get(uint32_t ifindex,struct efm_cpuperf_info_snmp  efm_snmp_mux[]);

int 
efm_agent_get_oam_info(struct efm_oam_info_data  * efm_oam_info,efm_agent_remote_SetGet_info * efm_RsetGetInfo,efm_agent_remote_state_info * efm_RstInfo);

int 
efm_agent_snmp_oam_info_get(uint32_t ifindex,struct efm_oam_info_snmp  efm_snmp_mux[]);

int 
efm_agent_get_addr_info(struct efm_addr_info_data  * efm_addr_info,efm_agent_remote_SetGet_info * efm_RsetGetInfo);

int 
efm_agent_snmp_addr_info_get(uint32_t ifindex,struct efm_addr_info_snmp  efm_snmp_mux[]);

int 
efm_agent_snmp_conver_info_get(uint32_t ifindex,struct efm_conver_info_snmp  efm_snmp_mux[]);

void 
snmp_msg_rcv_efm_agent(struct ipc_msghdr_n * phdr);

void 
efm_agent_get_remote_refresh_oltptinfo(efm_agent_remote_SetGet_info * efm_RsetGetinfo);

int
efm_agent_get_oltpt_info(struct efm_oltpt_info_data  * efm_oltpt_info,efm_oltpt_info_log efm_oltpt_log_data);


int 
efm_agent_snmp_oltpt_info_get(uint32_t ifindex, uint32_t SecIfindex,struct efm_oltpt_info_snmp  efm_snmp_mux[]);

void 
efm_agent_get_remote_refresh_ethptinfo(efm_agent_remote_SetGet_info * efm_RsetGetinfo);

int 
efm_agent_get_ethpt_info(struct efm_ethpt_info_data  * efm_ethpt_info,efm_ethpt_info_log efm_ethpt_log_data);


int 
efm_agent_snmp_ethpt_info_get(uint32_t ifindex, uint32_t SecIfindex,struct efm_ethpt_info_snmp  efm_snmp_mux[]);

void 
efm_agent_get_remote_refresh_vlangroupinfo(efm_agent_remote_SetGet_info * efm_RsetGetinfo);

int 
efm_agent_get_vlangroup_info(struct efm_vlangroup_info_data  *efm_vlangroup_info,efm_vlangroup_info_log efm_vlangroup_log_data);

int 
efm_agent_snmp_vlangroup_info_get(uint32_t ifindex, uint32_t SecIfindex,struct efm_vlangroup_info_snmp  efm_snmp_mux[]);

int 
efm_agent_get_oltptalm_info(struct efm_oltptalm_info_data  *efm_oltptalm_info,efm_oltptalm_info_log efm_oltptalm_log_data);


int 
efm_agent_snmp_oltptalm_info_get(uint32_t ifindex, uint32_t SecIfindex,struct efm_oltptalm_info_snmp  efm_snmp_mux[]);

void 
efm_agent_get_remote_refresh_ethptalminfo(efm_agent_remote_state_info * efm_RstInfo);

int 
efm_agent_get_ethptalm_info(struct efm_ethptalm_info_data  *efm_ethptalm_info,efm_ethptalm_info_log efm_ethptalm_log_data);

int 
efm_agent_snmp_ethptalm_info_get(uint32_t ifindex, uint32_t SecIfindex,struct efm_ethptalm_info_snmp  efm_snmp_mux[]);

void 
efm_agent_get_remote_refresh_ethvlaninfo(efm_agent_remote_SetGet_info * efm_RsetGetInfo);

int 
efm_agent_get_ethvlan_info(struct efm_ethvlan_info_data  * efm_ethvlan_info,efm_ethvlan_info_log efm_ethvlan_log_data);

int 
efm_agent_snmp_ethvlan_info_get(uint32_t ifindex, uint32_t SecIfindex,struct efm_ethvlan_info_snmp  efm_snmp_mux[]);

int 
efm_agent_get_ethperf_info(struct efm_ethperf_info_data  * efm_ethperf_info,efm_agent_info * aefm);

int 
efm_agent_snmp_ethperf_info_get(uint32_t ifindex,struct efm_ethperf_info_snmp  efm_snmp_mux[]);

void 
snmp_msg_rcv_efm_agent_SecIfindex_info(struct ipc_msghdr_n  *phdr,uint32_t SecIfindex);

int 
efm_agent_send_data_to_snmp(struct ipc_msghdr_n * phdr,void * pdata,int msg_num,uint32_t ifindex,uint8_t type);

void 
snmp_msg_rcv_efm_agent_set(struct ipc_msghdr_n *phdr,uint32_t * pdata);

int 
efm_agent_set_remote_Table(struct efm_if * pefm,uint32_t set_value,uint32_t set_item,uint32_t set_table);

int 
efm_agent_set_remote_OamMode_Item(struct efm_if * pefm,uint32_t set_value,uint32_t set_item);

int 
efm_agent_set_remote_EthPerf_Item(struct efm_if * pefm,uint32_t set_value,uint32_t set_item);

int 
efm_agent_set_remote_OltPerf_Item(struct efm_if * pefm,uint32_t set_value,uint32_t set_item);

int 
efm_agent_set_remote_Perf_Item(struct efm_if * pefm,uint32_t set_value,uint32_t set_item);

int 
efm_agent_set_remote_QinQ_Item(struct efm_if * pefm,uint32_t set_value,uint32_t set_item);

int 
efm_agent_set_remote_CpuptVlanConf_Item(struct efm_if * pefm,uint32_t set_value,uint32_t set_item);

int 
efm_agent_set_remote_OltptVlanConf_Item(struct efm_if * pefm,uint32_t set_value,uint32_t set_item);

int 
efm_agent_set_remote_VlanConf_Item(struct efm_if * pefm,uint32_t set_value,uint32_t set_item);

int 
efm_agent_set_remote_CommConf_Item(struct efm_if * pefm,uint32_t set_value,uint32_t set_item);

int 
efm_agent_set_remote_TwoIfindex_Table(uint32_t two_ifindex,struct efm_if * pefm,uint32_t set_value,uint32_t set_item,uint32_t set_table);

int 
efm_agent_set_remote_VlanGroupConf_Item(uint32_t two_ifindex,struct efm_if * pefm,uint32_t set_value,uint32_t set_item);

int 
efm_agent_set_remote_EthptVlanConf_Item(uint32_t two_ifindex,struct efm_if * pefm,uint32_t set_value,uint32_t set_item);

int 
efm_agent_set_remote_EthptConf_Item(uint32_t two_ifindex,struct efm_if *pefm,uint32_t set_value,uint32_t set_item);

int 
efm_agent_set_remote_OltptConf_Item(uint32_t two_ifindex,struct efm_if *pefm,uint32_t set_value,uint32_t set_item);

int efm_agent_send_data_to_snmp(struct ipc_msghdr_n * phdr,void * pdata,int msg_num,uint32_t ifindex,uint8_t type);

int 
efm_agent_set_remote_Qos_Item(struct efm_if * pefm,uint32_t set_value,uint32_t set_item);

int 
efm_agent_set_remote_CpuPerf_Item(struct efm_if * pefm,uint32_t set_value,uint32_t set_item);

int 
efm_agent_set_remote_AddrInfo_Item(struct efm_if * pefm,uint32_t set_value,uint32_t set_item);

int
efm_agent_get_oltperf_info(struct efm_oltperf_info_data  * efm_oltperf_info,efm_agent_info * aefm);

void 
efm_agent_get_remote_refresh_oltptalminfo(efm_agent_remote_state_info * efm_RstInfo);

#endif

