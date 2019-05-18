#ifndef _MIB_MSTP_H_
#define _MIB_MSTP_H_

#define MSTP_OID 1,3,6,1,4,1,9966,5,35,19

enum mstp_global_config_table
{
    MstpRegionName,
    MstpRevisionLevel,
    MstpVlanMap ,
    MstpMode,
    MstpBridgeId,
	MstpRootBridgeId,
	MstpRegionRootBridgeId,
	MstpInternRootPathCost,
	MstpExternRootPathCost,
    MstpBridgeRole,
    MstpBridgePriority,
    MstpHelloTime,
    MstpFowardDelay,
    MstpMaxAge,
    MstpMaxHopCount,
};

enum mstp_Port_Info_table
{
	MstpPortProtocalState,
	MstpPortState,
	MstpPortMode,
	MstpPortDesignatedBridgeId,
	MstpPortRole,
	MstpPortPriority,
	MstpPortPathCost,
	MstpPortMessage,
	MstpPortEdgeState,
	MstpPortFilterState,
	MstpPortP2P,
};

enum mstp_instance_table
{
	MstpInstanceId,
	MstpInstanceVlanMap,
	MstpInstanceBridgePriority,
	MstpInstanceBridgeId,
	MstpInstanceRegRootBridgeId,
	MstpInstanceInternalRootPathCost,

};

enum mstp_instance_port_table
{
	MstpInstancePortIfindex,
	MstpInstancePortState,
	MstpInstancePortRole,
	MstpInstancePortPriority,
	MstpInstancePortPathCost,
	MstpInstancePortMessage,
	MstpInstancePortDesignalBridgeId,
	MstpInstancePortMaxHop,
	MstpInstancePortEdgeState,
 	MstpInstancePortFilterState,
	MstpInstancePortP2p,
};
enum mstp_port_statistics_table
{

	MstpPortTcSend,
	MstpPortTcRecv,
	MstpPortTcnSend,
	MstpPortTcnRecv,
	MstpPortBpduSend,
	MstpPortBpduRecv,
	MstpPortConfigSend,
	MstpPortConfigRecv,
	MstpPortRstpSend,
	MstpPortRstpRecv,
	MstpPortMstpSend,
	MstpPortMstpRecv,	
};
struct ipc_mesg_n  *snmp_get_mstp_info_bulk(uint32_t ifindex, int module_id , int msg_subtype, int data_num);
struct ipc_mesg_n  *snmp_get_mstp_instance_port_info_bulk(void * pdata,int data_len,uint32_t ifindex, int module_id , int msg_subtype, int data_num);

#endif

