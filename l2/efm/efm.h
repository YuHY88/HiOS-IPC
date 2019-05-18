#ifndef HIOS_EFM_H
#define HIOS_EFM_H

#include <sys/time.h>
#include <lib/vty.h>
#include "lib/hptimer.h"
#include "l2/l2_if.h"
#include "efm_link_monitor.h"
#include "efm_def.h"
#include <ftm/pkt_eth.h>
extern uint8_t efm_mac[6];
#define POWEROFF_TEST 24
/* 802.3ah states.  */
#define EFM_DISCOVERY_FAULT_STATE           				0
#define EFM_DISCOVERY_ACTIVE_SEND_LOCAL     			1
#define EFM_DISCOVERY_PASSIVE_WAIT         				2
#define EFM_DISCOVERY_SEND_LOCAL_REMOTE     		3
#define EFM_DISCOVERY_SEND_LOCAL_REMOTE_OK  	4
#define EFM_DISCOVERY_SEND_ANY              					5				
#define EFM_REMOTE_LOOPBACK_INITIALIZE      			6
#define EFM_REMOTE_LOOPBACK_RECEIVE         			7 // in receive function
#define EFM_REMOTE_LOOPBACK_INFO_RECEIVE    		8
#define EFM_REMOTE_LOOPBACK_EXIT            				9
#define EFM_TRANSMIT_RESET                  						10
#define EFM_TRANSMIT_WAIT_FOR_TX            					11
#define EFM_TRANSMIT_TX_OAMPDU              					12
#define EFM_TRANSMIT_DEC_PDU_CNT            				13

#define EFM_SUCCESS          	0
#define EFM_FAILURE          		1

#define EFM_AGENT_BASE_PDU_TIME 3
#define EFM_LINK_TIMER       	5
#define EFM_PDU_TIMER        	1

#define EFM_REM_LB_LINK_TIMER       		5

#define EFM_MAX_EVLOG_ENTRIES_DEF	100

#define EFM_PAR_MASK       		0x03
#define EFM_MUX_BIT_SHIFT  	2
#define EFM_MUX_BIT        			(1 << EFM_MUX_BIT_SHIFT)

#define EFM_STATE_FIELD_MASK      		0x07
#define EFM_CONFIG_FIELD_MASK     		0x1f
#define EFM_PDU_CONFIG_FIELD_MASK 	0x7ff

#define  EFM_HT2200_TEMPRATURE_MIN  0
#define  EFM_HT2200_TEMPRATURE_MAX  45

#define  EFM_HT201_TEMPRATURE_MIN   10
#define  EFM_HT201_TEMPRATURE_MAX  55

#define EFM_AGENT_STATE_DATA_MAX 0x77
#define EFM_AGENT_SET_DATA_MAX 0x87
#define EFM_AGENT_RESUME_SET_MAX 0x50
#define EFM_AGENT_CONFIG_VLAN_GROUP 15
#define EFM_LOG_DBG(fmt, ...) \
    do { \
        zlog_debug ( L2_DBG_EFM, fmt, ##__VA_ARGS__ ); \
    } while(0)

enum efm_pdu_req
{
	EFM_NONE,
	EFM_CRITICAL,
	EFM_NORMAL,
};  

enum efm_local_pdu
{
	EFM_LF_INFO,
	EFM_RX_INFO,
	EFM_INFO,
	EFM_ANY,
};

enum efm_loopback
{
	EFM_NONE_LB,
	EFM_LOCAL_LB,		//local side make remote side loopback
	EFM_REM_LB,		//remote side make local side loopback
	EFM_INVALID_LB,
};
enum efm_remote_loopback
{
         EFM_DISABLE_LB,
         EFM_LB,
};
enum efm_mac_swap
{
        EFM_NO_MAC_SWAP,
        EFM_MAC_SWAP,
};
enum efm_set_port_state_flag
{
               LINK_FAULT,
	      REMOTE_LOOPBACK,
	      MAC_SWAP,
                
};
enum efm_loopback_port_state
{
        DISCARD,
         FORWARD,
         LOOPBACK,
};
enum efm_local_par_action
{
	EFM_PAR_FWD,
	EFM_PAR_LB,
	EFM_PAR_DISCARD,
	EFM_PAR_INVALID,
};

enum efm_local_mux_action
{
	EFM_MUX_FWD,
	EFM_MUX_DISCARD,
	EFM_MUX_INVALID,
};

enum efm_lb_status
{
	EFM_NO_LB = 1,
	EFM_INIT_LB,
	EFM_REMOTE_LB,
	EFM_TERMINATING_LB,
	EFM_LOCAL_LBK,
	EFM_UNKNOWN,
};

enum efm_agent_pdu_type
{
	EFM_AGENT_DEVICE_INFO_PDU,
	EFM_AGENT_GET_INFO_PDU,
	EFM_AGENT_SET_INFO_PDU,
	EFM_AGENT_SET_GET_INFO_PDU,
	EFM_AGENT_CONFIG_ALL_DATA_PDU,
};
struct efm_info
{
	enum efm_loopback loopback;
	uint8_t		oam_version;
	uint16_t	revision;
	uint8_t		mac_addr [6];

	/* Multiplexer and Parser Action */
	uint8_t		state;
	uint8_t		oam_config;
	uint16_t	oam_pdu_config;
	uint8_t		oui [EFM_OUI_LENGTH];
	uint8_t		vend_spec_info [EFM_VSI_LENGTH];
};

 typedef struct efm_agent_remote_base
 {
	 /*base device info*/
	 uint8_t efm_remote_reply_words;
	 uint8_t efm_remote_fpga_ver;    	/*device FPGA version*/
	 uint8_t efm_remote_soft_ver;	   	 /*device soft version*/
	 uint8_t efm_remote_hard_ver;	 /*device hard version*/
	 uint8_t efm_remote_base_info_len; /*device base info table len*/
	 uint8_t efm_remote_state_info_len; /*device status info table len*/
	 uint8_t efm_remote_set_info_len;	/*device config info table len*/
	 uint8_t efm_remote_specific_info_len;/*device specific info table len*/
	 uint8_t efm_remote_E1_num;
	 uint8_t efm_remote_V35_num;
	 uint8_t efm_remote_eth_num;
	 uint8_t efm_remote_olt_num;
	 uint8_t efm_remote_eth_internal_num;
	 uint8_t efm_remote_tabs_data_len;
	 uint8_t efm_remote_device_channel_num;
	 uint8_t efm_remote_device_channel_type;
	 /*set device info*/
 }efm_agent_remote_base_info;
 
 enum efm_agent_remote_state_info_data
 {
	
	 efm_remote_state_HsSfi,		  //7bit --h/s	  1bit---SFI	0bit----SET_I
	 efm_remote_state_PowOlos,		//7bit---poweroff	  6bit----oam loopback state	1bit--olt2 los	   0bit--olt1 los
	 efm_remote_state_olt1, 	       //3--no link  2(1) bits---work speed(10M 100M (1000M))	  0bits--work mode(haif   double)
	 efm_remote_state_olt2, 		 //3--no link	 2(1) bits---work speed(10M 100M (1000M))	  0bits--work mode(half   double)
	 efm_remote_state_efh,		     //3--no link	  2(1) bits---work speed(10M 100M (1000M))	   0bits--work mode(half   double)
	 efm_remote_unknow,
	 efm_remote_pletcrum_state,
	 efm_remote_Oam_discovery_state,
	 efm_remote_Oam_Fiber_port_state,
	 efm_remote_Oam_flags_local,
	 efm_remote_Oam_flags_remote,
	 efm_remote_Oam_discovery_info,
	 efm_remote_Oam_state_field_local,
	 efm_remote_Oam_state_field_remote,
	 efm_remote_Oam_configuration_field_local,
	 efm_remote_Oam_configuration_field_remote,
	 efm_remote_Dying_Gasp_Counter4,
	 efm_remote_Dying_Gasp_Counter3,
	 efm_remote_Dying_Gasp_Counter2,
	 efm_remote_Dying_Gasp_Counter1,
	 efm_remote_Olt_Counter4,
	 efm_remote_Olt_Counter3,
	 efm_remote_Olt_Counter2,
	 efm_remote_Olt_Counter1,
	 efm_remote_Eth_Counter4,
	 efm_remote_Eth_Counter3,
	 efm_remote_Eth_Counter2,
	 efm_remote_Eth_Counter1,
	 efm_remote_Cpu_Counter4,
	 efm_remote_Cpu_Counter3,
	 efm_remote_Cpu_Counter2,
	 efm_remote_Cpu_Counter1,	 
	 efm_remote_Olt_Bytes_Counter8,
	 efm_remote_Olt_Bytes_Counter7,
	 efm_remote_Olt_Bytes_Counter6,
	 efm_remote_Olt_Bytes_Counter5,
	 efm_remote_Olt_Bytes_Counter4,
	 efm_remote_Olt_Bytes_Counter3,
	 efm_remote_Olt_Bytes_Counter2,
	 efm_remote_Olt_Bytes_Counter1,	 
	 efm_remote_Eth_Bytes_Counter8,
	 efm_remote_Eth_Bytes_Counter7,
	 efm_remote_Eth_Bytes_Counter6,
	 efm_remote_Eth_Bytes_Counter5,
	 efm_remote_Eth_Bytes_Counter4,
	 efm_remote_Eth_Bytes_Counter3,
	 efm_remote_Eth_Bytes_Counter2,
	 efm_remote_Eth_Bytes_Counter1,
	 efm_remote_Cpu_Bytes_Counter8,
	 efm_remote_Cpu_Bytes_Counter7,
	 efm_remote_Cpu_Bytes_Counter6,
	 efm_remote_Cpu_Bytes_Counter5,
	 efm_remote_Cpu_Bytes_Counter4,
	 efm_remote_Cpu_Bytes_Counter3,
	 efm_remote_Cpu_Bytes_Counter2,
	 efm_remote_Cpu_Bytes_Counter1,
	
 };
 typedef struct efm_oltptalm_info_data_log
 {
	uint8_t efm_Log_OltPtNO;
         uint8_t efm_Log_OltPtStatInfo;
         uint8_t efm_Log_OltPtAlmInfo;
 
 }efm_oltptalm_info_log;
 typedef struct efm_ethptalm_info_data_log
 {
	uint8_t efm_Log_EthPtNO;
         uint8_t efm_Log_EthPtStatInfo;
         uint8_t efm_Log_EthPtAlmInfo;
 
 }efm_ethptalm_info_log;
typedef struct efm_agent_remote_state
{
	uint8_t efm_agent_remote_state_data[EFM_AGENT_STATE_DATA_MAX];    /*act on get remote device status info*/
	 efm_oltptalm_info_log efm_oltptalm_info_two[2];
	  efm_ethptalm_info_log efm_ethptalm_info_two[2];
	 /*get device info*/
	 uint8_t efm_remote_reply_words;
	 uint8_t efm_remote_start_addr_num;
	 uint8_t efm_remote_start_addr;
	 uint8_t efm_remote_addr_len;
	uint8_t DyingGasp_flag;
	uint8_t LinkFault_flag;
}efm_agent_remote_state_info;
/*efm agent set remote info*/
typedef struct efm_agent_remote_set
{
	uint8_t efm_remote_set_reply_words;
	 uint8_t efm_remote_set_start_addr_num;
	 uint8_t efm_remote_set_start_addr;
	 uint8_t efm_remote_set_addr_len;
	 uint8_t efm_remote_set_value[EFM_AGENT_SET_DATA_MAX ];  /*act on get remote device config info*/
}efm_agent_remote_set_info;
/*efm agent set info list*/
enum efm_agent_remote_set_data
{
	
     efm_remote_set_Data_Resume,						 //0bits and 1bits is valid
     efm_remote_set_Oam_Mode,								  //0bits and 1 bits is vaild
     efm_remote_set_OamLoopb_Machange,			 //0bit-----loopback 1bit-----mac-change
     efm_remote_set_EthLLp, 									//0bit----ethernet inner port loopback
     efm_remote_set_ALS,										   //0bit is valid
     efm_remote_set_JumFrame,							   //0bit is valid
     efm_remote_set_Olt1Port,									   //0bit--ffdx 1 and 2bits---fspd	  2bit---fult	6bit----olt1   7bit---dis
     efm_remote_set_Olt1Flow,									 //0bit is valid
     efm_remote_set_OltPtIngressRateH,			
     efm_remote_set_OltPtIngressRateL,
     efm_remote_set_OltPtEgressRateH,		   
     efm_remote_set_OltPtEgressRateL,
     efm_remote_set_Olt2Port,											  //7bits---dis
 
     efm_remote_set_EthPort,									  //0bit--ffdx 1 and 2bits---fspd	 2bit---fult   6bit----olt1   7bit---dis
     efm_remote_set_EthFlow,									//0bit is valid 	   
     efm_remote_set_EthPtIngressRateH,			
     efm_remote_set_EthPtIngressRateL,
     efm_remote_set_EthPtEgressRateH,		   
     efm_remote_set_EthPtEgressRateL,
 
     efm_remote_set_1pQos,								//0bit is valid,
 
     efm_remote_set_QinQDirection,				   //0bit is valid
     efm_remote_set_QinQTpidH,
     efm_remote_set_QinQTpidL,
     efm_remote_set_QinQVidH,
     efm_remote_set_QinQVidL,
     efm_remote_set_QinQPiror,								//012bits is valid
 
     efm_remote_set_VlanMode,							   //0 1bits is valid
 
     efm_remote_set_OltPtInMode,						  //0 bit is valid
     efm_remote_set_OltPtTagMode,					   //0 bit is valid
     efm_remote_set_OltPtPrior,					//012bits  is valid
     efm_remote_set_OltPtPvidH, 						 //0123bits is valid
     efm_remote_set_OltPtPvidL, 				  
 
     efm_remote_set_EthPtInMode,						 //0 bit is valid
     efm_remote_set_EthPtTagMode,					  //0 bit is valid
     efm_remote_set_EthPtPrior,				   //012bits  is valid
     efm_remote_set_EthPtPvidH, 						//0123bits is valid
     efm_remote_set_EthPtPvidL, 				 
 
     efm_remote_set_CpuPtInMode,						 //0 bit is valid
     efm_remote_set_CpuPtTagMode,					  //0 bit is valid
     efm_remote_set_CpuPtPrior,				   //012bits  is valid
     efm_remote_set_CpuPtPvidH, 						//0123bits is valid
     efm_remote_set_CpuPtPvidL,  
 
 /*double ifindex(15 group)*/
 //    efm_remote_set_VlanGroupNO[EFM_AGENT_CONFIG_VLAN_GROUP],
     efm_remote_set_ThVlanmber_FoVlanId1,    //0123bits--vlanid_low4	  4--olt 5--eth 6--cpu
     efm_remote_set_VlanId1,
     efm_remote_set_ThVlanmber_FoVlanId2,    //0123bits--vlanid_low4	  4--olt 5--eth 6--cpu
     efm_remote_set_VlanId2,
     
       efm_remote_set_ThVlanmber_FoVlanId3,    //0123bits--vlanid_low4	  4--olt 5--eth 6--cpu
     efm_remote_set_VlanId3,
     
       efm_remote_set_ThVlanmber_FoVlanId4,    //0123bits--vlanid_low4	  4--olt 5--eth 6--cpu
     efm_remote_set_VlanId4,
     
       efm_remote_set_ThVlanmber_FoVlanId5,    //0123bits--vlanid_low4	  4--olt 5--eth 6--cpu
     efm_remote_set_VlanId5,
     
       efm_remote_set_ThVlanmber_FoVlanId6,    //0123bits--vlanid_low4	  4--olt 5--eth 6--cpu
     efm_remote_set_VlanId6,
     
       efm_remote_set_ThVlanmber_FoVlanId7,    //0123bits--vlanid_low4	  4--olt 5--eth 6--cpu
     efm_remote_set_VlanId7,
     
       efm_remote_set_ThVlanmber_FoVlanId8,    //0123bits--vlanid_low4	  4--olt 5--eth 6--cpu
     efm_remote_set_VlanId8,
     
       efm_remote_set_ThVlanmber_FoVlanId9,    //0123bits--vlanid_low4	  4--olt 5--eth 6--cpu
     efm_remote_set_VlanId9,
     
       efm_remote_set_ThVlanmber_FoVlanId10,    //0123bits--vlanid_low4	  4--olt 5--eth 6--cpu
     efm_remote_set_VlanId10,
     
       efm_remote_set_ThVlanmber_FoVlanId11,    //0123bits--vlanid_low4	  4--olt 5--eth 6--cpu
     efm_remote_set_VlanId11,
     efm_remote_set_ThVlanmber_FoVlanId12,	  //0123bits--vlanid_low4	 4--olt 5--eth 6--cpu
     efm_remote_set_VlanId12,
     efm_remote_set_ThVlanmber_FoVlanId13,		//0123bits--vlanid_low4    4--olt 5--eth 6--cpu
     efm_remote_set_VlanId13,
     efm_remote_set_ThVlanmber_FoVlanId14,		//0123bits--vlanid_low4    4--olt 5--eth 6--cpu
     efm_remote_set_VlanId14,
	
     efm_remote_set_ThVlanmber_FoVlanId15,		//0123bits--vlanid_low4    4--olt 5--eth 6--cpu
     efm_remote_set_VlanId15,
	
     efm_remote_set_CleanCounter,	 //0bit is valid
 
     efm_remote_set_OltCounterType,
     efm_remote_set_EthCounterType,
     efm_remote_set_CpuCounterType,
 
 
     efm_remote_set_IpCtrl, 										 //0bit is valid
     efm_remote_set_DHCPclient, 							 //0bit is valid
     efm_remote_set_EthStromfilter,						 //012bit is valid	0---broadcast	   1---multicast  2---uincasr
     efm_remote_set_LFPCtrl,  
     efm_remote_set_Reboot,
 
     efm_remote_set_4IpAddr,
     efm_remote_set_3IpAddr,
     efm_remote_set_2IpAddr,
     efm_remote_set_1IpAddr,
     efm_remote_set_4IpMask,
     efm_remote_set_3IpMask,
     efm_remote_set_2IpMask,
     efm_remote_set_1IpMask,
 
      efm_remote_set_4GatewayAddr,
      efm_remote_set_3GatewayAddr,
      efm_remote_set_2GatewayAddr,
      efm_remote_set_1GatewayAddr,
     efm_remote_set_6hpmcrtMacAddr,
     efm_remote_set_5hpmcrtMacAddr,
     efm_remote_set_4hpmcrtMacAddr,
     efm_remote_set_3hpmcrtMacAddr,
     efm_remote_set_2hpmcrtMacAddr,
     efm_remote_set_1hpmcrtMacAddr,
};
typedef struct efm_oltpt_info_data_log
{
	uint8_t efm_Log_Oltifindex;
	uint8_t efm_Log_OltPtNegCtrl;
	uint8_t efm_Log_OltPtSpdCtrl;
	uint8_t efm_Log_OltPtDplCtrl;
	uint8_t efm_Log_OltPtDisCtrl  ;
	uint16_t efm_Log_OltPtIngressRateN;
	uint16_t efm_Log_OltPtEgressRateN;
	uint8_t efm_Log_OltPtFlowCtrl;
}efm_oltpt_info_log;
typedef struct efm_ethpt_info_data_log
{
	uint8_t efm_Log_Ethifindex;
	uint8_t efm_Log_EthPtNegCtrl;
	uint8_t efm_Log_EthPtSpdCtrl;
	uint8_t efm_Log_EthPtDplCtrl;
	uint8_t efm_Log_EthPtDisCtrl  ;
	uint16_t efm_Log_EthPtIngressRateN;
	uint16_t efm_Log_EthPtEgressRateN;
	uint8_t efm_Log_EthPtFlowCtrl;
}efm_ethpt_info_log;
typedef struct efm_vlangroup_info_data_log
{
	uint8_t efm_Log_VlanGroupNO;
	uint16_t efm_Log_VlanId;
	uint8_t efm_Log_VlanMember;

}efm_vlangroup_info_log;

typedef struct efm_ethvlan_info_data_log
{
	uint8_t efm_Log_EthPtNo;
	uint8_t efm_Log_EthPtInMode;
	uint32_t efm_Log_EthPtPVID;
         uint8_t efm_Log_EthPtPrior;
         uint8_t efm_Log_EthPtTagMode;

}efm_ethvlan_info_log;

typedef struct efm_agent_remote_SetGet
{
   uint8_t efm_agent_remote_SetGet_data[EFM_AGENT_SET_DATA_MAX]; 
   efm_oltpt_info_log efm_oltpt_info_two[2];
   efm_ethpt_info_log efm_ethpt_info_two[2];
   efm_vlangroup_info_log efm_vlangroup_info_fif[EFM_AGENT_CONFIG_VLAN_GROUP];
    efm_ethvlan_info_log efm_ethvlan_info_two[2];
	 /*get device info*/
   uint8_t efm_remote_reply_words;
   uint8_t efm_remote_start_addr_num;
   uint8_t efm_remote_start_addr;
   uint8_t efm_remote_addr_len;
		
		 
}efm_agent_remote_SetGet_info;
 typedef struct efm_agent_info
{
	//struct thread *efm_agent_pdu_timer;
	TIMERID efm_agent_pdu_timer;
	uint8_t efm_agent_pdu_time;             			      /*efm agent send pdu every time*/
	uint8_t efm_agent_last_link_flag;				      /*efm agent bulid connect flag*/
	uint8_t efm_agent_link_flag;					      /*efm agent bulid connect flag*/
	uint16_t efm_agent_Msdhfrmnum;                                   /*send num*/
	uint8_t efm_agent_pdu_type;                                            /*send pdu type(base get set)*/
	uint8_t efm_rx_DifSpecific_pdu[4];			              /*efm agent receive pdu statistics*/							
	uint8_t efm_tx_DifSpecific_pdu[4];				    /*efm agent send pdu statistics*/	
	uint8_t efm_agent_remote_device_sid;                         /*get remote device id */
	uint8_t efm_agent_remote_device_tid;                         /*get remote device type*/
	
	
	efm_agent_remote_base_info * aefm_RbaseInfo;		/*efm agent base info struct*/
	efm_agent_remote_state_info * aefm_RstInfo;		/*efm agent status info struct*/
	efm_agent_remote_set_info * aefm_RsetInfo;		/*efm agent config info struct*/
	efm_agent_remote_SetGet_info * aefm_RsetGetInfo;	/*efm agent get config info struct*/

	uint8_t efm_agent_default_ip[4];
	uint8_t efm_agent_update_ip[4];  /*record new config ip*/
	uint32_t efm_agent_update_id;
	uint8_t efm_agent_hostname[VTYSH_HOST_NAME_LEN + 1];
	
}efm_agent_info;

struct efm_if
{
	 uint32_t if_index;                                               /*端口索引*/
	 char if_name[INTERFACE_NAMSIZ];                                  /*端口名字*/
	 uint8_t mac[6];                                                  /*mac字节*/
	 /*efm agent base info*/                                
	 efm_agent_info * aefm;                                           /*代管的信息结构*/
	 uint8_t efm_agent_remote_DeviceSid; 	                          /*device sequence ID*/
	 uint8_t efm_agent_remote_DeviceTid; 	                          /*device type ID*/
	uint8_t efm_agent_remote_config_data[EFM_AGENT_SET_DATA_MAX]; /*记录远端小盒的配置数据*/ 
	
	/* Configured Timer Values */
	uint8_t    efm_pdu_time;                                       /*发包时间*/
	uint8_t    efm_link_time;                                      /*链路发现时间*/
	uint8_t    efm_remote_loopback_timeout;                      /*远端环回超时时间*/
 
	/* Discovery State machine Variables */
	uint16_t efm_flags;                                            /*efm flag字段*/
	uint16_t efm_flags_last;                                      /*记录上一次的flag*/
	uint8_t  efm_discovery_state;                                  /*发现状态*/
	uint8_t  efm_local_link_status;                               /*本地端口链路状态*/
	uint8_t  efm_local_oam_mode;                                 /*oam模式（主动、被动）*/
	uint8_t  efm_local_unidirectional;                            /*无用*/
	uint8_t  efm_local_lost_link_timer_done:1;                   /*链路发现超时标志*/
	uint8_t  efm_localside_make_remoteside_loopback_start:1;     /*无用*/
	uint8_t  efm_rem_loopback:1;                                /*使能远端环回标志*/
	uint8_t  efm_local_critical_event:1;                        /*紧急事件发生标志*/
	uint8_t  efm_local_oam_enable;                              /*efm使能标志*/
	uint8_t  efm_local_dying_gasp:1;                             /*设备掉电标志*/
	uint8_t  efm_local_satisfied:1;				               /*收包efm匹配标志（二次握手）*/
	uint8_t  efm_remote_stable:1;                                /*连接稳定标志（三次握住标志）*/
	uint8_t  efm_remote_state_valid:1;			                /*第一次握手标志（收到efm报文）*/
	uint8_t  efm_local_stable:1;                                   /*第二次握手成功下发的动作*/
	
	uint8_t	efm_remoteside_make_localside_loopback_start:1;   /*无用*/

	/* Transmit State Machine Variables */
	uint8_t  valid_pdu_req:1;                 /*发包条件*/
	enum efm_pdu_req pdu_req;           /*发包条件状态*/
	uint8_t  efm_pdu_cnt;                /*发包计数*/
	uint8_t  efm_pdu_max;               /*发包最大数（默认是10）*/
	enum efm_local_pdu local_pdu;         /*包类型*/
	uint8_t  efm_transmit_state;           /*发包状态*/
	uint8_t  efm_pdu_max_per_second;     /*无用*/

	/* Local and Remote Information */
	struct efm_info rem_info;                                   /*本段TLV*/
	struct efm_info local_info;                                 /*远端TLV*/

	/* Pending Link Events to be Sent */
#define EFM_SYM_PERIOD_EVENT_PENDING   (1 << 0)                   /*错误符号周期事件标志位*/
#define EFM_FRAME_EVENT_PENDING            	 (1 << 1)             /*错误帧事件标志位*/
#define EFM_FRAME_PERIOD_EVENT_PENDING     (1 << 2)               /*错误帧周期事件标志位*/
#define EFM_FRAME_SEC_SUM_EVENT_PENDING	 (1 << 3)                   /*错误帧秒事件标志位*/
uint8_t	efm_pending_link_events;

#define EFM_OAM_REMOTE_EXCESS_ERRORS	 (1 << 1)                    /*错误事件发生标志*/
#define EFM_OAM_LOCAL_EXCESS_ERRORS	 (1 << 0)                    /*错误事件发生标志*/
	uint8_t	efm_excess_errrors;

	/* Local Link Event Configuration */
#define EFM_LINK_EVENT_ON		(1 << 0)                           /*一般链路事件检测开启标志*/
	uint8_t 	 local_link_event_flags;                          /*一般链路事件检测开启标志*/

	/* Local and Remote Link Event Information */
	struct efm_link_event_info rem_link_info;                     /*一般链路事件报文远端*/
	struct efm_link_event_info local_link_info;                    /*一般链路事件报文局端*/

	/* Local Multiplexer and Parser Action */
	enum efm_local_mux_action efm_local_mux_action_t;       /*远端环回状态标志*/
	enum efm_local_par_action efm_local_par_action_t;        /*远端环回状态标志*/

	/* Timers */
	#if 0
	struct thread *efm_tx_timer;                /*发包计时*/
	struct thread *efm_link_timer;              /*发现计时*/
	struct thread *efm_rem_lb_timer;           /*远端环回超时计时*/
	#endif

	TIMERID efm_tx_timer;                                          /*发包计时*/
	TIMERID efm_link_timer;                                       /*发现计时*/
	TIMERID efm_rem_lb_timer;                                     /*远端环回超时计时*/

	/*Used to count no of OAMPDUs sent or recieved*/
	uint32_t rx_count [EFM_PDU_MAX];                              /*收包计数*/
	uint32_t tx_count [EFM_PDU_MAX];                               /*发包计数*/

	/*Link Event Log Entries*/
	struct list *efm_if_eventlog_list;                             /*日志链表*/

	/*this variable keeps track of total number of event entries, per port.*/
	uint32_t num_event_log_entries;                             /*日志个数计数*/

	uint32_t max_event_log_entries;                             /*最大日志个数（可配置）,默认是100*/

	uint32_t event_log_index;                                   /*日志排列序号*/

	enum efm_local_mux_action efm_local_mux_action_tlv_lst;     /*上次远端环回状态标志*/    
	enum efm_local_par_action efm_local_par_action_tlv_lst;     /*上次远端环回状态标志*/

	uint8_t   efm_config_tlv_lst;                                /*上次报文字段-oam-config*/
	uint16_t  efm_pdu_config_tlv_lst;                            /*上次报文字段-oam-pdu-config*/

	uint8_t efm_mac_swap;                                         /*mac交换动作*/
	uint8_t efm_mac_swap_flag;                                    /*mac交换标志位*/
	uint8_t efm_loopback_flag;                                   /*环回标志位*/

	struct efm_link_monitor_run efm_link_monitor_t;             /*一般链路事件参数配置保存*/
};


#define EFM_CENTISECS_PER_SECS         			(10)
#define EFM_MICROSECS_PER_CENTI_SECS   	(100000)
#define EFM_TIME_STAMP_MASK            			(0xffff)

/* List iteration macro. */
#define EFM_LIST_LOOP(L,V,N) \
  if (L) \
    for ((N) = (L)->head; (N); (N) = (N)->next) \
      if (((V) = (N)->data) != NULL)

#define EFM_LIST_LOOP_DEL(L,V,N,NN) \
	if (L) \
		for ((N) = (L)->head, NN = ((N)!=NULL) ? (N)->next : NULL;	\
				 (N); 																									\
				 (N) = (NN), NN = ((N)!=NULL) ? (N)->next : NULL) 			\
			if (((V) = (N)->data) != NULL)
void
efm_pthread_dyinggasp_event_check(void);

void 
efm_check_dying_gasp( void *arg);

s_int32_t
efm_process_return (struct vty *vty, s_int32_t retval);

uint8_t *
efm_discovery_state_to_str (const uint8_t discovery_state);

uint8_t *
efm_par_state_to_str (enum efm_local_par_action par_action);
uint8_t *
efm_mux_state_to_str (enum efm_local_mux_action mux_action);


s_int32_t
efm_protocol_enable (struct l2if *pif);
s_int32_t
efm_protocol_disable (struct l2if *pif);
s_int32_t
efm_remote_unidir_link_set(struct efm_if *pefm, uint8_t enable);
s_int32_t
efm_mode_active_set (struct efm_if *pefm);
s_int32_t
efm_mode_passive_set (struct efm_if *pefm);
s_int32_t
efm_remote_loopback_set (struct efm_if *pefm, u_int8_t enable);
s_int32_t
efm_remote_loopback_timeout_set (struct efm_if *pefm, u_int8_t timeout);
s_int32_t
efm_remote_loopback_start_api (struct efm_if *pefm);
int
efm_remote_loopback_initialize(struct efm_if *pefm);
int
efm_rem_lb_timer_expiry (void *arg);
int
efm_remote_loopback_exit (struct efm_if *pefm);

s_int32_t
efm_symbol_period_threshold_set (struct efm_if *pefm,
                                   						uint64_t threshold, efm_bool set);

s_int32_t
efm_symbol_period_window_set (struct efm_if *pefm,
                               							uint64_t window, efm_bool set);

s_int32_t
efm_err_frame_threshold_set (struct efm_if *pefm,
                                 						uint32_t threshold, efm_bool set);

s_int32_t
efm_err_frame_window_set (struct efm_if *pefm,
							  							uint16_t window, efm_bool set);

s_int32_t
efm_err_frame_period_threshold_set (struct efm_if *pefm,
                                        				uint32_t threshold, efm_bool set);

s_int32_t
efm_err_frame_period_window_set (struct efm_if *pefm,
                                        				uint32_t window, efm_bool set);

s_int32_t
efm_err_frame_second_threshold_set (struct efm_if *pefm,
                                        				uint16_t threshold, efm_bool set);

s_int32_t
efm_err_frame_second_window_set (struct efm_if *pefm,
                                        				uint16_t window, efm_bool set);

s_int32_t
efm_event_notify_set(struct efm_if *pefm, const char *str1, efm_bool is_set);

s_int32_t
efm_set_link_timer (struct efm_if *pefm, uint32_t secs);

s_int32_t
efm_pdu_timer_set (struct efm_if *pefm, uint8_t secs);


s_int32_t
efm_clear_statistics (struct efm_if *pefm);

void
efm_show_statistics (struct vty *vty);

void
efm_show_if_statistics (struct efm_if *pefm, struct vty *vty);

void 
efm_show_if_interface (struct efm_if *pefm, struct vty *vty);

void
efm_show_interface (struct vty *vty);

void
efm_show_if_discovery (struct efm_if *pefm, struct vty *vty);

void
efm_show_discovery (struct vty *vty);

void
efm_show_if_status (struct efm_if *pefm, struct vty *vty);

void
efm_show_status (struct vty *vty);

void
efm_show_if_event_log (struct efm_if *pefm, struct vty *vty);

void
efm_show_event_log (struct vty *vty);

void l2if_efm_config_write(struct vty *vty, struct efm_if *pefm);
void efm_init(void);
void efm_deinit(void);
void efm_clean_port(struct efm_if *pefm);


#endif

