#ifndef _MIB_EFM_AGENT_H_
#define _MIB_EFM_AGENT_H_

#define EFM_AGENT_HPMCREMOTE 1,3,6,1,4,1,9966,2,100,1,1,11,2
#define EFM_AGENT_CONVERTER 1,3,6,1,4,1,9966,6,3,2,19

enum hpmcrtCommConf_info
{

    hpmcrtSubType,
    hpmcrtSoftVer,
    hpmcrtEthChNum,
    hpmcrtEthPtNum,
    hpmcrtOltPtNum,
    hpmcrtHwSw,
    hpmcrtSFI,

    hpmcrtDataResum  = 9,
    hpmcrtLFPCtrl ,
    hpmcrtALSCtrl,
    hpmcrtJumbo,
    hpmcrtReboot,
    hpmcrtIpCtrl,
    hpmcrtEthStromfilter,
    hpmcrtDHCPclient,

    hpmcrtDipSwitchSta,
    hpmcrtHostName,
};
enum hpmcrtOltPtConf_info
{
    hpmcrtOltPtNegCtrl = 1 ,
    hpmcrtOltPtSpdCtrl  ,
    hpmcrtOltPtDplCtrl  ,
    hpmcrtOltPtDisCtrl  ,
    hpmcrtOltPtIngressRateN = 6 ,
    hpmcrtOltPtEgressRateN  = 8 ,
    hpmcrtOltPtFlowCtrl,

};

enum hpmcrtEthPtConf_info
{

    hpmcrtEthPtNegCtrl = 1,
    hpmcrtEthPtSpdCtrl ,
    hpmcrtEthPtDplCtrl ,
    hpmcrtEthPtDisCtrl  ,
    hpmcrtEthPtIngressRateN = 6,
    hpmcrtEthPtEgressRateN = 8,
    hpmcrtEthPtFlowCtrl ,
};

enum hpmcrtOltPtAlm_info
{
    hpmcrtOltPtNO,
    hpmcrtOltPtStatInfo,
    hpmcrtOltPtAlmInfo,

};
enum hpmcrtEthPtAlm_info
{
    hpmcrtEthPtNO,
    hpmcrtEthPtStatInfo,
    hpmcrtEthPtAlmInfo,


};

enum hpmcrtVlanConf_info
{
    hpmcrtVlanMode,

};

enum hpmcrtOltPtVlanConf_info
{
    hpmcrtOltPtInMode = 1,
    hpmcrtOltPtPVID,
    hpmcrtOltPtPrior,
    hpmcrtOltPtTagMode,

};

enum hpmcrtEthPtVlanConf_info
{

    hpmcrtEthPtInMode = 1,
    hpmcrtEthPtPVID,
    hpmcrtEthPtPrior,
    hpmcrtEthPtTagMode,
};

enum hpmcrtCpuPtVlanConf_info
{
    hpmcrtCpuPtInMode = 1,
    hpmcrtCpuPtPVID ,
    hpmcrtCpuPtPrior ,
    hpmcrtCpuPtTagMode,

};

enum hpmcrtVlanGroupConf_info
{
    hpmcrtVlanGroupNO = 1,
    hpmcrtVlanId,
    hpmcrtVlanMember,

};
enum hpmcrtInvalid_info
{
    hpmcrtInvildvalue,

};
enum hpmcrtQinQ_info
{
    hpmcrtQinQDirection = 1,
    hpmcrtQinQTPID,
    hpmcrtQinQVid,
    hpmcrtQinQPrior,
};
enum hpmcrtQos_info
{
    hpmcrt1pQos = 1,

};
enum hpmcrtPerf_info
{
    hpmcrtCleanCounter = 1,
};
enum hpmcrtOltPerf_info
{
    hpmcrtOltCntType = 1,
    hpmcrtOltRxPCnt ,
    hpmcrtOltTxPCnt,
    hpmcrtOltRxBytes,

};
enum hpmcrtEthPtPerf_info
{
    hpmcrtEthPtNo,
    hpmcrtEthCntType,
    hpmcrtEthRxPCnt,
    hpmcrtEthTxPCnt ,
    hpmcrtEthRxBytes,


};

enum hpmcrtCpuPerf_info
{

    hpmcrtCpuCntType = 1 ,
    hpmcrtCpuRxPCnt ,
    hpmcrtCpuTxPCnt ,
    hpmcrtCpuRxBytes,

};

enum hpmcrtOam_info
{
    hpmcrtOamMode = 1,
    hpmcrtOamLpbkSta,

};

enum hpmcrtAddr_Info
{
    hpmcrtIpAddr = 1,
    hpmcrtIpMask,
    hpmcrtGatewayAddr,
    hpmcrtMacAddr,

};

enum hpmcrtConver_Info
{
    hpmcrtIpConverterMode = 1,

};

enum efm_agent_set_stable
{
    hpmcrtCommConf_set = 100,
    hpmcrtOltPtConf_set,
    hpmcrtEthPtConf_set,
    hpmcrtVlanConf_set,
    hpmcrtOltPtVlanConf_set,
    hpmcrtEthPtVlanConf_set,
    hpmcrtCpuPtVlanConf_set,
    hpmcrtVlanGroupConf_set,
    hpmcrtQinQ_set,
    hpmcrtQoS_set,
    hpmcrtPerf_set,
    hpmcrtOltPerf_set,
    hpmcrtEthPtPerf_set,
    hpmcrtCpuPerf_set,
    hpmcrtOam_set,
    hpmcrtAddr_set,


};


int snmp_send_SetInfo_to_efmagent(void *pdata, int data_len, int data_num, uint32_t msg_ifindex);
struct ipc_mesg_n  *snmp_get_efm_agent_info_bulk(uint32_t ifindex,int *pdata_num, int msg_subtype, int data_num);
struct ipc_mesg_n  *snmp_get_efm_agent_SecIfindex_info_bulk(void *data, int data_len, uint32_t ifindex, int *pdata_num, int msg_subtype, int data_num);
int snmp_config_msg_send_l2_wait_ack(void *pdata, uint32_t data_len, uint16_t data_num,uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index);





#endif
