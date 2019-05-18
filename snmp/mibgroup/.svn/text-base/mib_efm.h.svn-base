#ifndef _MIB_EFM_H_
#define _MIB_EFM_H_

#define EFM_HHREFM 1,3,6,1,2,1,158,1
#define EFM_PRI_HHREFM 1,3,6,1,4,1,9966,5,35,9,2

enum efm_local_info_table
{
    dot3OamAdminState = 1,
    dot3OamOperStatus,
    dot3OamMode ,
    dot3OamMaxOamPduSize ,
    dot3OamConfigRevision ,
    dot3OamFunctionsSupported,
};
enum efm_peer_table
{
    dot3OamPeerMacAddress = 1,
    dot3OamPeerVendorOui ,
    dot3OamPeerVendorInfo,
    dot3OamPeerMode,
    dot3OamPeerMaxOamPduSize,
    dot3OamPeerConfigRevision,
    dot3OamPeerFunctionsSupported,
};
enum efm_loopback
{

    dot3OamLoopbackStatus = 1,
    dot3OamLoopbackIgnoreRx,

};
enum efm_stats
{
    dot3OamInformationTx   = 1,
    dot3OamInformationRx ,
    dot3OamUniqueEventNotificationTx,
    dot3OamUniqueEventNotificationRx,
    dot3OamDuplicateEventNotificationTx,
    dot3OamDuplicateEventNotificationRx,
    dot3OamLoopbackControlTx,
    dot3OamLoopbackControlRx,
    dot3OamVariableRequestTx,
    dot3OamVariableRequestRx,
    dot3OamVariableResponseTx,
    dot3OamVariableResponseRx,
    dot3OamOrgSpecificTx,
    dot3OamOrgSpecificRx,
    dot3OamUnsupportedCodesTx,
    dot3OamUnsupportedCodesRx ,
    dot3OamFramesLostDueToOam,
};
enum event_configs
{
    dot3OamErrSymPeriodWindowHi = 1,
    dot3OamErrSymPeriodWindowLo ,
    dot3OamErrSymPeriodThresholdHi     ,
    dot3OamErrSymPeriodThresholdLo     ,
    dot3OamErrSymPeriodEvNotifEnable   ,
    dot3OamErrFramePeriodWindow        ,
    dot3OamErrFramePeriodThreshold     ,
    dot3OamErrFramePeriodEvNotifEnable ,
    dot3OamErrFrameWindow              ,
    dot3OamErrFrameThreshold          ,
    dot3OamErrFrameEvNotifEnable       ,
    dot3OamErrFrameSecsSummaryWindow   ,
    dot3OamErrFrameSecsSummaryThreshold ,
    dot3OamErrFrameSecsEvNotifEnable   ,
    dot3OamDyingGaspEnable             ,
    dot3OamCriticalEventEnable         ,
};
enum efm_eventlog
{
    dot3OamEventLogIndex              = 1,
    dot3OamEventLogTimestamp      ,
    dot3OamEventLogOui            ,
    dot3OamEventLogType           ,
    dot3OamEventLogLocation       ,
    dot3OamEventLogWindowHi       ,
    dot3OamEventLogWindowLo       ,
    dot3OamEventLogThresholdHi    ,
    dot3OamEventLogThresholdLo    ,
    dot3OamEventLogValue   ,
    dot3OamEventLogRunningTotal   ,
    dot3OamEventLogEventTotal  ,
    dot3OamPacketStaticClearAll,


};
enum efm_base
{
    hhrEFMTimeout   = 1,
    hhrEFMUnidirectionalLinkSupport ,
    hhrEFMRate,
};

enum efm_remloopback
{
    hhrEFMRemLoopbackAction = 1,
    hhrEFMRemLoopbackTimeout ,
    hhrEFMRemLoopbackSupport,
    hhrEFMRemLoopbackMacSwap,

};

enum efm_event
{
    hhrEFMEventLinkMonitorStatus = 1,
    hhrEFMEventLinkMonitorSupport,
    hhrEFMEventLogNumber,
};
#endif

struct ipc_mesg_n * snmp_get_efm_info_bulk(uint32_t ifindex,int *pdata_num, int msg_subtype, int data_num);

struct ipc_mesg_n * snmp_get_efm_eventlog_info_bulk(void *pdata, int data_len, uint32_t ifindex, int *pdata_num, int msg_subtype, int data_num);


























