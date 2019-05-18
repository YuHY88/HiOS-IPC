/*
*  Copyright (C) 2016~2017  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: mstp_port.h
*
*  date: 2017.4
*
*  modify:
*
*/



#ifndef _MSTP_PORT_H_
#define _MSTP_PORT_H_


#include "mstp.h"
#include "mstp_bpdu.h"

#include "l2_if.h"


/*cist priority vector*/
struct cist_priority_vector
{
	struct bridge_id		root_id;
	unsigned int			external_root_path_cost;
	struct bridge_id		regional_root_id;
	unsigned int			internal_root_path_cost;
	struct bridge_id		designated_bridge_id;
	unsigned short			designated_port_id;	
	unsigned short			recv_port_id;
};

/*cist timer vector*/
struct cist_timer_vector
{
	unsigned short			forward_delay;
	unsigned short			hello_time;
	unsigned short			max_age;
	unsigned short			msg_age;
	unsigned char			remaining_hops;
};

/*msti priority vector*/
struct msti_priority_vector
{	
	struct bridge_id		rg_root_id;
	unsigned int			internal_root_path_cost;	
	struct bridge_id		designated_bridge_id;
	unsigned short			designated_port_id;		
	unsigned short			recv_port_id;	
};

/*msti timer vector*/
struct msti_timer_vector
{
	unsigned char			remaining_hops;
};

/*store recv cist bpdu cfg
**every cist port have one this struct*/
struct mstp_cist_bpdu_cfg
{
	unsigned char			message_type;		/*paterner proto mode*/
	unsigned char			topology_change_ack;
	unsigned char			topology_change;
	unsigned char			proposal;
	unsigned char			agreement;
	unsigned char			learning;
	unsigned char			forwarding;
	unsigned char			role;
};

/*store recv msti bpdu cfg
**every mst instance port have one this struct*/
struct mstp_msti_bpdu_cfg
{
	unsigned char			new_msg;	/*the msg deal flag*/
	unsigned char			topology_change_ack;
	unsigned char			topology_change;
	unsigned char			proposal;
	unsigned char			agreement;
	unsigned char			learning;
	unsigned char			forwarding;
	unsigned char			role;
};



/*port variables applies to the CIST and to all MSTIs*/
struct mstp_port_variables
{
	/*portEnabled is TRUE if and only if:
	a) MAC_Operational (6.6.2) is TRUE; and
	b) Administrative Bridge Port State (8.4, 13.11) for the port is Enabled.
	means is port link & port enable mstp func*/
	unsigned char				portEnabled;/*record port mstp function enabled, [Figure 13-12—Port Receive state machine]*/

	/*State Machine Timers	[chapter-13.21]*/
	/*mdelayWhile: port MIGRATION delay time , we think is 0*/
	unsigned char				mdelayWhile;
	/*The Hello timer. Used to ensure that at least one BPDU is transmitted by a 
	Designated Port in each HelloTime period. */
	unsigned char				helloWhen;


	/*The Edge Delay timer.
	**The time remaining, in the absence of a received BPDU, 
	before this port is identified as an operEdgePort.*/
	/*edgeDelayWhile is equal to bridge migrate_time(we think is 0)*/
	unsigned char				edgeDelayWhile;


	/*A single per-Port instance applies to the CIST and to all MSTIs*/
	/*13.25.2 ageingTime
	Filtering database entries for this port are aged out after ageingTime has elapsed since they were first created
	or refreshed by the Learning Process. The value of this parameter is normally Ageing Time (8.8.3, Table 8-
	6), and is changed to FwdDelay (13.26.8) for a period of FwdDelay after  fdbFlush (13.25.13) is set by the
	topology change state machine if stpVersion (13.26.20) is TRUE.*/
	unsigned char				ageingTime;/*used for flushing of dynamic filtering information that has been learned;*/
	unsigned char				tick;	/*record [Port Timers state machine] tick variable*/	
	unsigned char				txCount;/*used for [Figure 13-13 Port Transmit state machine]*/
	unsigned char				operEdge;/*Figure 13-19 Topology Change state machine*/

	unsigned char                isolate;
	/*applies to the CIST and or all MSTIs*/
	/*infoInternal value is come from rcvdInternal*/
	unsigned char				infoInternal;/* Figure G-5 [Port Information state machine]*/
	unsigned char				newInfoCist;/*used for [Figure 13-13 Port Transmit state machine]*/

/*A Boolean. Set TRUE if a BPDU conveying changed MSTI information is to be transmitted. 
It is set FALSE by the Port Transmit state machine*/
	unsigned char				newInfoMsti;/*used for [Figure 13-13 Port Transmit state machine]*/
											/* Figure G-5 [Port Information state machine]*/

	/*recvInternal record msg recv from external mst or internal mst
	A Boolean. Set TRUE by the Receive Machine if the BPDU received was transmitted 
	by a bridge in the same MST Region as the receiving bridge.*/		
	unsigned char				rcvdInternal;/* Figure G-5 [Port Information state machine]*/

	/*13.25.49 restrictedRole
	A Boolean. Set by management. If TRUE causes the port not to be selected as Root Port for the CIST or any
	MSTI, even it has the best spanning tree priority vector. Such a port will be selected as an Alternate Port
	after the Root Port has been selected. This parameter should be FALSE by default. If set, it can cause lack of
	spanning tree connectivity. It is set by a network administrator to prevent bridges external to a core region of
	the network influencing the spanning tree active topology, possibly because those bridges are not under the
	full control of the administrator.*/
	unsigned char				restrictedRole;

	/*13.25.50 restrictedTcn
	A Boolean. Set by management. If TRUE causes the port not to propagate received topology change
	notifications and topology changes to other ports. This parameter should be FALSE by default. If set it can
	cause temporary loss of connectivity after changes in a spanning  trees active topology as a result of
	persistent incorrectly learned station location information.  It is set by a network administrator to prevent
	bridges external to a core region of the network, causing address flushing in that region, possibly because
	those bridges are not under the full control of the administrator or MAC_Operational for the attached LANs
	transitions frequently.*/
	unsigned char				restrictedTcn;	//FIXME

	/*A single per-Port instance is used by all state machines*/
	/*13.25.25 mcheck
	A Boolean. May be set by management to force the Port Protocol Migration state machine to transmit RST
	(or MST) BPDUs for a MigrateTime (13.24.5, Table 13-5) period, to test whether all STP Bridges on the
	attached LAN have been removed and the port can continue to transmit RSTP BPDUs. Setting mcheck has
	no effect if stpVersion (13.26.20) is TRUE*/
	unsigned char				mcheck;	/*Huahuan`s IPRAN think always true*/

	/*13.25.38 rcvdBPDU
	A Boolean. Set by system dependent processes, this variable notifies the Port Receive state machine (13.29)
	when a valid (Clause 14) Configuration, TCN, RST, or MST BPDU (14.3) is received on the port. Reset by
	the Port Receive state machine.*/
	unsigned char				rcvdBpdu;/*[Figure 13-12 Port Receive state machine], chapter 13.27*/
	unsigned char				rcvdRSTP;/*[Figure 13-12 Port Receive state machine], chapter 13.27*/
	unsigned char				rcvdSTP;/* [Figure 13-12 Port Receive state machine], chapter 13.27*/
	unsigned char				rcvdTcAck;/*Figure 13-19 Topology Change state machine*/
	unsigned char				rcvdTcn;/*Figure 13-19 Topology Change state machine*/

/*set value(true or false) in [Port Protocol Migration sm]
** used in [Port Transmit sm]*/
	unsigned char				sendRSTP;

	/*13.25.57 tcAck
	A Boolean. Set to transmit a Configuration Message with a topology change acknowledge flag set.*/
	unsigned char				tcAck;/*[Figure 13-13 Port Transmit state machine]*/
										/*Figure 13-19 Topology Change state machine*/

	/*root protection switch(开关)
	**no use now*/
	unsigned char				rootPtSwitch;
	/*loop path  switch(开关)
	**no use now*/
	unsigned char				loopPtSwitch;

	/*manual config this port is edge port*/
	/*13.25.1 AdminEdge
	A Boolean. Set by management if the port is to be identified as operEdge immediately on initialization,
	without a delay to detect other bridges attached to the LAN. The recommended default value is FALSE.*/	
	unsigned char				AdminEdge;	/*edge port*/

	/*13.25.5 AutoEdge
	A Boolean. Set by management if the bridge detection state machine (BDM, 13.31) is to detect other bridges
	attached to the LAN, and set operEdge automatically. The recommended default is TRUE.*/
	unsigned char				AutoEdge;

	unsigned char				AdminLink;

	/*6.6.3 Point-to-point parameters
	The adminPointToPointMAC parameter can take one of three values. If it is
	a) ForceTrue, operPointToPointMAC shall be TRUE, regardless of any indications to the contrary
	generated by the service providing entity.
	b) ForceFalse, operPointToPointMAC shall be FALSE.
	c) Auto, operPointToPointMAC is determined by the service providing entity, as specified in 6.7.*/
	unsigned char				operPointToPointMAC;

	/*used for sm state begin flag*/
	unsigned char				sm_timer_state_begin;
	unsigned char				sm_rcv_state_begin;
	unsigned char				sm_migration_state_begin;
	unsigned char				sm_tx_state_begin;
	unsigned char				sm_br_detect_state_begin;

	unsigned char				sm_timer_state; 	/*record [Port Timers state machine]*/
	unsigned char				sm_rcv_state;		/*record [Port Receive state machine]*/
	unsigned char				sm_migration_state; /*Port Protocol Migration state machine*/
	unsigned char				sm_tx_state;		/*Port Transmit state machine*/
	unsigned char				sm_br_detect_state; /*Bridge Detection state machine*/ 
};

/*base on the rfc-802.1s(part 13)*/
/*mstp port common variables, every cist port and msti port have one this struct
**cist port & msti port have this struct to store info*/
struct mstp_common_port
{
/*One instance per-Port of the following shall be implemented
**for the CIST and one per-Port for each MSTI*/

	/*The Forward Delay timer.
	Used to delay Port State transitions until other Bridges have received spanning
	tree information.*/
 	unsigned int				fdWhile;/*The Forward Delay timer: Figure 13-16 Port Role Transitions state machine
										Part 1: Disabled Roles*/

	unsigned int				rrWhile;		/*The Recent Root timer. */
	unsigned int				rbWhile;		/*The Recent Backup timer*/
	unsigned int				tcWhile;/*The Topology Change timer. TCN Messages are sent while this timer is running*/

	/*13.23.6 rcvdInfoWhile
	The Received Info timer. The time remaining before information, i.e. portPriority (13.25.33) and portTimes
	(13.25.34), received in a Configuration Message is aged out if a further message is not received*/
	unsigned int				rcvdInfoWhile;	/*The Received Info timer.*/
	
	unsigned int				port_cost;
	mstp_bool					fdbFlush;		/*Forward database */

	/*13.25.9 disputed
	A Boolean. See 13.19, 13.18, Figure 13-20, 13.27.15, Figure 13-24, and Figure 13-25.*/
	/*13.19 Partial and disputed connectivity*/
	/*need to confirm*/
	unsigned char				disputed;		

	/*13.25.14 forward
	A Boolean. Set or cleared by the Port Role Transitions state machine (13.35) to instruct the Port State
	Transitions state machine (13.36) to enable or disable forwarding.*/
	/*There is one instance per-Port of each variable for the CIST, and one per-Port for each MSTI*/	
	unsigned char				forward;/*Figure 13-18 Port State Transition state machine*/

	/*13.25.15 forwarding
	A Boolean. Set or cleared by the Port Port State Transitions state machine (13.36) to indicate 
	that forwarding has been enabled or disabled. */
	unsigned char				forwarding;
										
	/*13.25.17 infoIs:
	A variable that takes the values Mine, Aged, Received, or Disabled, to indicate the origin/state
	of the port`s Spanning Tree information (portInfo) held for the port:*/										
	unsigned char				infoIs;

	/*13.25.21 learn
	A Boolean. Set or cleared by the Port Role Transitions state machine (13.35) to instruct the Port State
	Transitions state machine (13.36) to enable or disable learning.*/
	unsigned char				learn;	

	/*13.25.22 learning
	A Boolean. Set or cleared by the Port Port State Transitions state machine (13.36) to indicate that learning
	has been enabled or disabled. */	
	unsigned char				learning;

	/*used for PA*/
	unsigned char				proposed;
	unsigned char				proposing;	/*PA*/

	/*13.25.44 rcvdTc:  used for recv TCN bpdu
	A Boolean. See 13.27.13, 13.27.23 and 13.37.*/
	unsigned char				rcvdTc; /*recv topology change msg, Figure 13-19 Topology Change state machine*/

	/*13.25.47 reRoot
	A Boolean. Set by a newly selected Root Port to force prior Root Ports to Discarding, before it becomes
	forwarding. See Figure 13-24, 13.27.20, Figure 13-23, Figure 13-25, and Figure 13-26.*/
	unsigned char				reRoot;	/*Figure 13-16 Port Role Transitions state machine-Part 1: Disabled, alternate, backup*/
	unsigned char				reselect;/*A Boolean. Set to prompt recomputation of the CIST or an MSTI. See 13.33 and 13.34.*/

	/*13.27.21 setSelectedTree()
	Sets selected TRUE for this tree (the CIST or a given MSTI) for all ports of the bridge if reselect is FALSE
	for all ports in this tree. If reselect is TRUE for any port in this tree, this procedure takes no action.*/
	unsigned char				selected;/* Figure G-5 Port Information state machine*/
										/*Figure 13-16 Port Role Transitions state machine-Part 1: Disabled*/

	/*13.25.58 tcProp
	A Boolean. Set by the Topology Change state machine of any other port, to indicate that a topology change
	should be propagated through this port.*/
	unsigned char				tcProp;/*Figure 13-19 Topology Change state machine*/

	/*13.25.61 updtInfo
	A boolean. Set by the Port Role Selection state machine (13.34, 13.27.31) to tell the Port
	Information state machine that it should copy designatedPriority to portPriority and 
	designatedTimes to portTimes.*/
	unsigned char				updtInfo;/* Figure G-5 Port Information state machine*/
									/*Figure 13-16 Port Role Transitions state machine-Part 1: Disabled*/

	/*13.25.4 agreed
	A Boolean. Set, for the CIST or an MSTI, if an Agreement has been received indicating that the Port States
	and transmitted priority vectors for the other bridge attached to this LAN are (and, in the absence of further
	communication with this bridge and within the design probabilities of protocol failure due to repeated
	BPDU loss, will remain) compatible with a loop-free active topology determined by this port’s priority
	vectors (13.15, 13.22).*/
	unsigned char				agreed;/* Figure G-5 Port Information state machine*/

	/*13.25.32 portId
	The Port Identifier for this port. This variable forms a component of the port priority and designated priority
	vectors (13.9,13.10).
	The 4 most significant bits of the Port Identifier (the settable Priority component) for the CIST and for each
	MSTI can be modified independently of the setting of those bits for all other trees, as a part of allowing full
	and independent configuration control to be exerted over each Spanning Tree instance.*/
	unsigned short				portId;

	/*13.27.12 rcvInfo()
	Returns SuperiorDesignatedInfo if, for a given port and tree (CIST, or MSTI):*/
	unsigned char				rcvdInfo;/* Figure G-5 Port Information state machine*/

	/*13.25.51 role
	The current Port Role. DisabledPort, RootPort, DesignatedPort, AlternatePort, BackupPort, or MasterPort.*/
	unsigned char				role; /*Figure 13-19 Topology Change state machine*/

	/*13.25.53 selectedRole
	A newly computed role for the port.*/
	unsigned char				selectedRole;

	/*Set to force the Port State to be compatible with the loop-free active topology determined by the
priority vectors held by this bridge (13.15, 13.22) for this tree (CIST, or MSTI), transitioning the Port State
to Discarding, and soliciting an Agreement if possible, if the port is not already synchronized (13.25.56)*/
	unsigned char				sync;

/*A Boolean. TRUE only if the Port State is compatible with the loop-free active topology
determined by the priority vectors held by this bridge for this tree (13.15, 13.22)..*/
	unsigned char				synced;	

	/*A Boolean. See 13.15 and Figure 13-12*/									
	unsigned char				agree;

	/*update value by 13.27.13 rcvMsgs()
	value is true or false*/
	unsigned char				rcvdMsg;/* Figure G-5 Port Information state machine*/

	/*if AdminPathCost is true, update port status will not change port_path_cost*/
	/*when config port path cost is TRUE*/	
	unsigned char				AdminPathCost;

	/*each instance root protect state
	**we no use now*/
	unsigned char				rootPtState; 
#if 0
	unsigned char				loopPtState;
#endif

/*state machine begin flag*/
	unsigned char				sm_info_state_begin;
	unsigned char				sm_disable_role_state_begin;
	unsigned char				sm_state_trans_state_begin;
	unsigned char				sm_tplg_change_state_begin;

/*state machine*/
	unsigned char				sm_info_state;			/*Port Information state machine*/
	unsigned char				sm_disable_role_state;	/*Port Role Transitions state machine*/
	unsigned char				sm_master_role_state;	/*Port Master Role state machine*/
	unsigned char				sm_root_role_state;		/*Port Root Role state machine*/
	unsigned char				sm_deg_role_state;		/*Port Designated Role state machine*/
	unsigned char				sm_ab_role_state;		/*Port Alter Backup Role state machine*/
	unsigned char				sm_state_trans_state;	/*Port State Transition state machine*/
	unsigned char				sm_tplg_change_state;	/*Topology Change state machine*/

};

struct mstp_cist_port
{	
	/*this is physics port index*/
	unsigned int				port_index;

	/*point to port mstp_port*/
	struct mstp_port* 		 	mport;

	/*A Boolean. Set TRUE if a BPDU conveying changed CIST information is to be transmitted.
	It is set FALSE by the Port Transmit state machine.*/	
    unsigned int				new_info;/* Figure G-5 Port Information state machine*/

	/*because ipran`s every instance config para value
	**and port can enable each instance, so msti port which is use instance vlaue*/
//	struct mstp_port_variables	port_variables;	//FIXME


	/*CIST bridge pointer*/
	struct mstp_cist_br*     	cist_bridge;

	struct mstp_common_port		common_info;

	/*[designated_priority/designated_times] store send bpdu info */
	struct cist_priority_vector	designated_priority;
	struct cist_timer_vector	designated_times;

	/*[msg_priority /msg_times]  store recv msg info*/
	struct cist_priority_vector msg_priority;
	struct cist_timer_vector  	msg_times;

	/**/
	struct cist_priority_vector port_priority;/* Figure G-5 Port Information state machine*/
	struct cist_timer_vector  	port_times;/* Figure G-5 Port Information state machine*/

	/*bpdu_cfg  store recv msg flag info*/
	struct mstp_cist_bpdu_cfg	bpdu_cfg;


	/*Link to the mstp bridge*/
    struct list_head 			port_list;		//FIXME
};

struct mstp_msti_port
{	
	struct mstp_msti_port		*next;

	/*this is physics port index*/
	unsigned int				port_index;

	/*store mstp port name*/
	unsigned char				if_name[INTERFACE_NAMSIZE];
		
	unsigned int				master;
	unsigned int				mastered;

	/*because ipran`s every instance config para value
	**and port can enable each instance, so msti port which is use instance vlaue*/
//	struct mstp_port_variables	port_variables;	//FIXME

	/*MSTI bridge pointer*/
    struct mstp_msti_br*		msti_bridge;

	struct mstp_common_port		common_info;

	/*[designated_priority/designated_times] store send bpdu info */
	struct msti_priority_vector	designated_priority;/* Figure G-5 Port Information state machine*/
	struct msti_timer_vector	designated_times;/* Figure G-5 Port Information state machine*/

	/*[msg_priority/msg_times]store recv msti bpdu cfg*/
	struct msti_priority_vector msg_priority;
	struct msti_timer_vector  	msg_times;

	/**/
	struct msti_priority_vector port_priority;/* Figure G-5 Port Information state machine*/
	struct msti_timer_vector  	port_times;/* Figure G-5 Port Information state machine*/

	/*store recv msti bpdu cfg
	**every mst instance port have one this struct*/
	struct mstp_msti_bpdu_cfg	bpdu_cfg;

	/*point to port mstp_port*/
	struct mstp_port*			mport;

	struct list_head			port_list;/*link to the bridge*/
};

struct mstp_port
{
	struct mstp_port			*next;

	/*store interface index*/
	unsigned int				ifindex;

	/*store mstp port name*/
	unsigned char				if_name[INTERFACE_NAMSIZE];

	unsigned char				link_status;
	/*store bridge stp work mode*/
	unsigned char				stp_mode;


	unsigned char				adminStatus;	/*true:enable, false: disable*/

	/*always true*/
//	mstp_bool					mcheck;

	/*in steps of 16, default is 128*/
	unsigned char				port_pri;
	unsigned char				msti_port_pri[MSTP_INSTANCE_MAX];

	/*cist/msti port cost value*/
	unsigned int				port_path_cost;
	unsigned int				msti_port_path_cost[MSTP_INSTANCE_MAX];

	/*edge port role
	**the port don't participate in the calculation of spanning tree
	*/
	mstp_bool					mstp_edge_port;

	/*filter port role
	**the port do not deal and send bpdu, if value is true*/
	mstp_bool					mstp_filter_port;

	/*mstp port link type*/
	mstp_bool					mstp_p2p;

	/*mstp port force full duplex and p2p*/
	mstp_bool                  mstp_force;

	/*used for bpdu protect function:
	**when if interface cfg bpdu protect, and because this interface down*/
	unsigned int				bpdu_down;

	/*in fact: port vlaue is common use for [cist_port & msti_port & (stp & rstp) ports]*/
	struct mstp_port_variables	port_variables;
    struct mstp_cist_port  		cist_port;

	/*when the port exist mst instances more than one*/
    struct mstp_msti_port  		*msti_port[MSTP_INSTANCE_MAX];
	struct mstp_bridge			*m_br;

	/*store recv msg mst, check if is come from same region by (compare with bridge mst_cfg_id)*/
	struct mst_config_id		mst_cfg_id;
	

	struct list_head        	port_list;

	/**/ 
/*used for mstp bpdu*/

	/*frame buff*/
	unsigned char				tx_frame[MSTP_MAX_FRAME_SIZE];
	unsigned char				rx_frame[MSTP_MAX_FRAME_SIZE];

	/*frame buff len*/
	unsigned short				tx_len;
	unsigned short				rx_len;

	/*port send bpdu statics*/
	unsigned long long			bpdu_config_send;
	unsigned long long			bpdu_rst_send;
	unsigned long long			bpdu_mst_send;
	unsigned long long			bpdu_tc_send;
	unsigned long long			bpdu_tcn_send;

	/*port recv bpdu statics*/
	unsigned long long			bpdu_config_recv;
	unsigned long long			bpdu_rst_recv;
	unsigned long long			bpdu_mst_recv;
	unsigned long long			bpdu_tc_recv;
	unsigned long long			bpdu_tcn_recv;		
};



typedef unsigned int(* MSTP_MSTI_FUN)(struct mstp_port*, unsigned int);
typedef unsigned int(* MSTP_PORT_FUN)(struct mstp_port*);
typedef unsigned int(* MSTP_CIST)(unsigned int);



/*used for store instance config variables*/
struct mstp_instance
{
	struct mstp_instance		*next;

#if 0
	/*add mstp port to instance*/
	struct mstp_port			*mstp_port_list;
#endif

	/*this instance list of all msti ports*/
	struct mstp_msti_port		*msti_port_list;

	/*this instance id*/	
	unsigned char				instance_id;

	/*this instance of work region vlan*/
	struct mstp_vlan			msti_vlan;

	/*this instance of bridge priority*/
	/*range<1-614440>,
	**when msti_bridge_root_role is master config priority is 0
	**when msti_bridge_root_role is slave config priority is 4096*/
	unsigned short				msti_br_priority;

	/*when config cost by manual AdminPathCost is true or false*/
	unsigned char				AdminPathCost;

	/*this instance of bridge root role
	**you can not config master and salve role in one bridge*/
//	enum MSTI_PORT_ROOT			msti_bridge_root_role;
};


/*common bridge*/
struct mstp_common_br
{
	/*used for  [Port Timers state machine]	for all port(cist ports & msti ports)*/
	/* This is a Boolean variable controlled by the system initialization process. A value of TRUE
	causes all TPMR(Two-Port MAC Relay) state machines to continuously execute their initial state.
	A value of FALSE 	allows all state machines to perform transitions out of their initial state,
	in accordance with the relevant state machine definitions.*/
	unsigned int				begin;	/*because */

	/*store mst info*/
//	struct mst_config_id		mst_cfg_id;

	/*force protocol version*/
	unsigned char				force_pt_version;

	/*in the rfc TxHoldCount default is 6, in ipran do not care*/
	unsigned int				tx_hold_count;/*used for [Figure 13-13 Port Transmit state machine]*/

	/*migrate time is always 0. direct transition port state*/
	unsigned short				migrate_time;
	
	unsigned short				bridge_max_age;

	unsigned short				max_hops;

	/*time*/
	MSTP_PORT_FUN				edge_delay;
	MSTP_PORT_FUN				forward_delay;
    MSTP_PORT_FUN				fwd_delay;
    MSTP_PORT_FUN				hello_time;/* HelloTime :used for [Figure 13-13 Port Transmit state machine]*/
    MSTP_PORT_FUN				max_age;

	MSTP_PORT_FUN               pdu_older;
	/*version*/
	MSTP_PORT_FUN				rstp_version;
	MSTP_PORT_FUN				stp_version;

    MSTP_PORT_FUN				all_transmit_ready;
	
	MSTP_MSTI_FUN				rerooted;
    MSTP_MSTI_FUN 				all_synced;
	
    MSTP_CIST					cist;

	/*port role*/
    MSTP_PORT_FUN				cist_root_port; /*(port == MSTP_PORT_ROLE_ROOT)? 1:0*/
    MSTP_PORT_FUN				cist_designated_port; /*(port == MSTP_PORT_ROLE_DESIGNATED)? 1:0*/
	
//	MSTP_PORT_FUN				mstiDesignatedOrTCpropagatingRootPort;
								/*msti_designated_port == mstiDesignatedOrTCpropagatingRootPort*/
	MSTP_PORT_FUN				msti_designated_port;/*Figure 13-13[Port Transmit state machine]:mstiDesignatedPort*/
//	MSTP_PORT_FUN				tc_propagating_root_port;

	/*TRUE if the role for any MSTI for the given port is MasterPort.*/
	MSTP_PORT_FUN				msti_master_port;/*Figure 13-13[Port Transmit state machine]:mstiRootPort*/

	/*recv msg*/
    MSTP_PORT_FUN				rcvd_any_msg;/*rcvdAnyMsg Figure 13-12-[Port Receive state machine]*/
    MSTP_PORT_FUN				rcvd_cist_msg;/*rcvdXstMsg: Figure G-5 Port Information state machine*/
    MSTP_MSTI_FUN				rcvd_msti_msg;/*rcvdXstMsg: Figure G-5 Port Information state machine*/
	
	MSTP_PORT_FUN				restricted_role;
	MSTP_PORT_FUN				restricted_tcn;
	
    MSTP_PORT_FUN				updt_cist_info;/*updtXstInfo: Figure G-5 Port Information state machine*/
    MSTP_MSTI_FUN				updt_msti_info;/*updtXstInfo:Figure G-5 Port Information state machine*/
};

/*cist bridge*/
struct mstp_cist_br
{
	/*Port Role Selection state machine begin flag*/
	unsigned int					port_role_select_state_begin;

	/*Port Role Selection state machine*/
	unsigned int					port_role_select_state;
	struct bridge_id				bridge_mac;

	/**/
    struct cist_priority_vector 	bridge_priority; 
	struct cist_timer_vector		bridge_times;

	/*a)root_priority:	chose the best pri as the root_priority
	**b)root_times:	cist_br->root_times = cist_br->root_port->port_times;*/
    struct cist_priority_vector		root_priority;
	struct cist_timer_vector		root_times;

	/*store cist bridge root port*/
    struct mstp_cist_port*  		root_port;

	/*cist bridge cist port list head*/
	struct list_head        		port_head;		//FIXME
};

/*msti bridge*/
struct mstp_msti_br
{
	/*Port Role Selection state machine*/
	unsigned int					port_role_select_state_begin;

	/*Port Role Selection state machine*/
	unsigned int					port_role_select_state;
	
	struct bridge_id				bridge_mac;

    struct msti_priority_vector    	bridge_priority; 
    struct msti_timer_vector	 	bridge_times;

    struct msti_priority_vector    	root_priority;
	struct msti_timer_vector	 	root_times;
	
    struct mstp_msti_port*  		root_port;

//	struct hlist_head					hash[MSTP_HASH_SIZE];	//FIXME
	/*store mstp msti port*/
	struct list_head        		port_head;
};

/*mstp bridge struct*/
struct mstp_bridge
{
	/*store bridge mac addr*/
	struct bridge_id				bridge_mac;/*default bridge priority is 0*/

	/*store bridge stp work mode*/
	unsigned char					stp_mode;

	/*store mst msg*/
	struct mst_config_id			mst_cfg_id;

	/*store instance config msg*/
	struct mstp_instance			mstp_instance[MSTP_INSTANCE_MAX];

	/*mstp common bridge (CST)*/
	struct mstp_common_br			common_br;

	/*this bridge is located in cist area*/
	struct mstp_cist_br     		cist_br;

	/*store this bridge msg in every msti*/
    struct mstp_msti_br    			*msti_br[MSTP_INSTANCE_MAX];

	/*Port Timers state machine tick, default is 100ms*/
	unsigned int					mstp_tick_timer;
	TIMERID                         mstp_tick_time;
	/*this instance of bridge priority*/
	/*range<1-614440>,
	**when bridge_root_role is master config priority is 0
	**when msti_bridge_root_role is slave config priority is 4096*/
	unsigned short					br_priority;

	/*config master and salve role in this bridge*/
	enum MSTP_ROOT_ROLE				bridge_root_role;

	/*instance times variable 
	**the value of hello_time/fwd_delay/msg_max_age should be 
	**Satisfy the following formula, or network will frequently
	2*(Forward Delay-1.0 second) >= Max Age
	Max Age >= 2*(Hello Time + 1.0 second)
	*/
	unsigned short					hello_time;
	unsigned short					fwd_delay;
	unsigned short					msg_max_age;
	unsigned short					msg_max_hop_count;

	/*add mstp port to [mstp bridge mstp-port-list]*/
	struct list_head        		port_head;
};


int mstp_clean_instance(struct vty *vty, unsigned int instance_id);
int mstp_delete_instance_msti_port(unsigned int instance_id, struct mstp_port *port);
void mstp_del_msti_port(struct mstp_port* mstp_port, unsigned int instance_id);
void mstp_add_msti_port(struct mstp_port* mstp_port, unsigned int instance_id);
void mstp_clean_mstp_port(struct mstp_port *clean_mstp_port);
void mstp_msti_vlanmap_backto_cist_vlanmap(unsigned char *dest, unsigned char *src);
void mstp_port_vlan_change(struct l2if *pif);
void mstp_mode_modify_delete_msti_port(void);
void mstp_mode_modify_add_msti_port(void);



#endif
