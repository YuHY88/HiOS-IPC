#ifndef _EFM_SNMP_H_
#define _EFM_SNMP_H_
#include "efm_def.h"
#define EFM_LOCAL_INFO_LEN 1
#define EFM_PEER_INFO_LEN 2
#define EFM_LOOPBACK_INFO_LEN 3
#define EFM_STATS_INFO_LEN 4
#define EFM_EVENT_CONFIG_INFO_LEN 5
#define EFM_EVENTLOG_INFO_LEN 6
#define EFM_INFO_LEN 7
#define EFM_REMLOOPBACK_INFO_LEN 8
#define EFM_EVENT_INFO_LEN 9

enum efm_snmp_mode
{
	EFM_SNMP_PASSIVE = 1,
	EFM_SNMP_ACTIVE ,
	EFM_SNMP_UNKNOWN,
};
struct efm_local_info_data
{
       uint8_t efm_local_enable;
       uint8_t efm_local_discovery_status;	
       uint8_t efm_local_mode;
       uint16_t efm_local_pdu_size;
       uint16_t efm_local_config_revision;
       uint8_t efm_local_supports;
      
};
struct efm_peer_info_data
{
	   uint8_t efm_peer_mac[6];
	   uint8_t efm_peer_oui [EFM_OUI_LENGTH];
	   uint32_t efm_peer_vendor_info;
	   uint8_t efm_peer_mode;
	   uint16_t efm_peer_pdu_size;
	   uint16_t efm_peer_config_revision;
	   uint8_t efm_peer_supports;


};
enum efm_loopback_state
{
          SNMP_NOLOOPBACK =1,
          SNMP_INITATING_LOOPBACK,
          SNMP_REMOTE_LOOPBACK,
          SNMP_TERMINATING_LOOPBACK,
          SNMP_LOCAL_LOOPBACK,
          SNMP_KNOWM,

};
enum efm_loopback_act
{
	IGNORE_REMOTE_LOOPBACK =1,
	PROCESS_REMOTE_LOOPBACK,
};
struct efm_loopback_info_data
{
	uint8_t efm_loopback_state;
         uint8_t efm_loopback_ignore_rx;
 

};
struct efm_stats_info_data
{
         uint32_t   efm_InformationTx ;   
 	uint32_t   efm_InformationRx ;               
         uint32_t      efm_UniqueEventNotificationTx;
         uint32_t        efm_UniqueEventNotificationRx;    
         uint32_t        efm_DuplicateEventNotificationTx; 
         uint32_t       efm_DuplicateEventNotificationRx; 
         uint32_t   efm_LoopbackControlTx;            
         uint32_t     efm_LoopbackControlRx;    
	uint32_t	   efm_VariableRequestTx;		  
	uint32_t	  efm_VariableRequestRx;		   
	uint32_t	 efm_VariableResponseTx;			
	uint32_t	  efm_VariableResponseRx;		   
	uint32_t	  efm_OrgSpecificTx;			  
	uint32_t	  efm_OrgSpecificRx;			 
	uint32_t	  efm_UnsupportedCodesTx;		   
	uint32_t	 efm_UnsupportedCodesRx ;			
	uint32_t	 efm_FramesLostDueToOam;			



};
struct  efm_event_config_info_data
{

	uint32_t 			efm_ErrSymPeriodWindowHi;
	uint32_t 			efm_ErrSymPeriodWindowLo ;
	uint32_t 			 efm_ErrSymPeriodThresholdHi ;
	uint32_t 			 efm_ErrSymPeriodThresholdLo ;
	uint32_t 			 efm_ErrSymPeriodEvNotifEnable;
	uint32_t 			 efm_ErrFramePeriodWindow	;
	uint32_t 			 efm_ErrFramePeriodThreshold ;
	uint32_t 			 efm_ErrFramePeriodEvNotifEnable ;
	uint32_t 			 efm_ErrFrameWindow;
	uint32_t 			 efm_ErrFrameThreshold	;	 
	uint32_t 			 efm_ErrFrameEvNotifEnable;
	uint32_t 			 efm_ErrFrameSecsSummaryWindow;
	uint32_t 			 efm_ErrFrameSecsSummaryThreshold;
	uint32_t 			 efm_ErrFrameSecsEvNotifEnable;
	uint32_t 			 efm_DyingGaspEnable ;
	uint32_t 			 efm_CriticalEventEnable ;



};
struct efm_eventlog_info_data
{
	uint32_t 		 efm_EventLogIndex	;
	uint32_t		 efm_EventLogTimestamp;
	uint8_t		 efm_EventLogOui[3] ;
	uint32_t		 efm_EventLogType	;
	uint32_t		 efm_EventLogLocation;
	uint32_t		 efm_EventLogWindowHi	 ;
	uint32_t		 efm_EventLogWindowLo;
	uint32_t		 efm_EventLogThresholdHi ;
	uint32_t		 efm_EventLogThresholdLo ;
	uint64_t		 efm_EventLogValue;
	uint64_t		 efm_EventLogRunningTotal ;
	uint32_t		 efm_EventLogEventTotal;


};
struct efm_info_data
{
	uint32_t efm_timeout;
	uint8_t  efm_unidirectionalLinkSupport;
	uint32_t efm_rate;
};

struct efm_remloopback_info_data
{
	uint8_t efm_remloopback_action;
	uint32_t efm_remloopback_timeout;
	uint8_t efm_remloopback_support;
	uint8_t efm_remloopback_macswap;
};
struct efm_event_info_data
{
	uint8_t efm_event_linkmonitor_status;
	uint8_t efm_event_linkmonitor_support;
	uint32_t efm_eventlog_number;

};

struct efm_local_info_snmp
{
       uint32_t ifindex;
       struct efm_local_info_data efm_local_info;

};
struct efm_peer_info_snmp
{
	uint32_t ifindex;
         struct efm_peer_info_data efm_peer_info;

};
struct efm_loopback_info_snmp
{
	uint32_t ifindex;
	 struct efm_loopback_info_data efm_loopback_info;


};
struct efm_stats_info_snmp
{
	uint32_t ifindex;
	struct efm_stats_info_data efm_stats_info;
};
struct efm_event_config_info_snmp
{
	uint32_t ifindex;
	struct  efm_event_config_info_data efm_event_config_info;
};
struct efm_eventlog_info_snmp
{
	uint32_t ifindex;
	struct efm_eventlog_info_data efm_eventlog_info;
};
struct efm_extra_info_snmp
{
	uint32_t ifindex;
	uint8_t efm_extra_value;

};
struct efm_info_snmp
{	
	uint32_t ifindex;
	struct efm_info_data efm_info;

};
struct efm_remloopback_info_snmp
{
	uint32_t ifindex;
	struct efm_remloopback_info_data efm_remloopback_info;
};
struct efm_event_info_snmp
{
	uint32_t ifindex;
	struct efm_event_info_data efm_event_info;
};

int efm_get_local_info(struct efm_local_info_data * local_info,struct efm_if * pefm);
int efm_get_peer_info(struct efm_peer_info_data * peer_info,struct efm_if *pefm);
int efm_get_loopback_info(struct efm_loopback_info_data  * loopback_info,struct efm_if *pefm);
int efm_get_stats_info(struct efm_stats_info_data * stats_info,struct efm_if *pefm);
int efm_get_event_config_info(struct efm_event_config_info_data * event_config_info ,struct efm_if  *pefm);
int efm_get_eventlog_info(struct efm_eventlog_info_data * eventlog_info ,struct efm_event_log_entry *eventlog);
int efm_get_base_info(struct efm_info_data * efm_info ,struct efm_if  *pefm);
int efm_get_remloopback_info(struct efm_remloopback_info_data * efm_info,struct efm_if * pefm);
int efm_get_event_info(struct efm_event_info_data * efm_info,struct efm_if * pefm);

int efm_snmp_port_local_info_get(uint32_t ifindex, struct efm_local_info_snmp  efm_snmp_mux[]);
int efm_snmp_port_peer_info_get(uint32_t ifindex, struct efm_peer_info_snmp  efm_snmp_mux[]);
int efm_snmp_port_loopback_info_get(uint32_t ifindex, struct efm_loopback_info_snmp  efm_snmp_mux[]);
int efm_snmp_port_stats_info_get(uint32_t ifindex, struct efm_stats_info_snmp  efm_snmp_mux[]);
int efm_snmp_port_event_config_info_get(uint32_t ifindex, struct efm_event_config_info_snmp efm_snmp_mux[]);
int efm_snmp_port_eventlog_info_get(uint32_t ifindex, uint32_t log_index,struct efm_eventlog_info_snmp efm_snmp_mux[]);
int efm_snmp_port_base_info_get(uint32_t ifindex, struct efm_info_snmp efm_snmp_mux[]);
int efm_snmp_port_remloopback_info_get(uint32_t ifindex, struct efm_remloopback_info_snmp efm_snmp_mux[]);
int efm_snmp_port_event_info_get(uint32_t ifindex, struct efm_event_info_snmp efm_snmp_mux[]);

void snmp_msg_rcv_efm(struct ipc_msghdr_n *phdr);
void snmp_msg_rcv_efm_eventlog_info(struct ipc_msghdr_n *phdr,uint32_t log_index);
int efm_send_data_to_snmp(struct ipc_msghdr_n * phdr,void * pdata,int ret,uint32_t ifindex,uint8_t num);
int efm_send_eventlog_data_to_snmp(struct ipc_msghdr_n * phdr,void * pdata,int ret,uint32_t ifindex);



#endif

