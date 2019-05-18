/*
*  Copyright (C) 2016~2017  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: mstp.h
*
*  date: 2016.12.23
*
*  modify:	2018.3.12 modified by liufuying to make mstp module code beautiful
*
*/


#ifndef _MSTP_H_
#define _MSTP_H_
#include <lib/ifm_common.h>

#define FALSE						0
#define TRUE						1

#define	PORT_LINK_DOWN				0
#define	PORT_LINK_UP				1

#define	PORT_DUPLEX_HALF			0
#define	PORT_DUPLEX_FULL			1

/*used for mstp return value*/
#define MSTP_RESULT_ERROR			-1
#define MSTP_RESULT_OK				0

#define NAME_LEN					32
#define INTERFACE_NAMSIZE   		32
#define DIGEST_LEN					16
#define MAC_LEN						6
#define MAX_REVISION_LEVEL			65535
#define MSTP_LOG_DBG(fmt, ...) \
    do { \
        zlog_debug ( L2_DBG_MSTP, fmt, ##__VA_ARGS__ ); \
    } while(0)



#define MSTP_LONG_PC_FACTOR						20000000
#define MSTP_PORT_COST_BASIS					200000000

#define MSTP_TIMER_EXPIRE_COUNT_PER_SECOND		5               //10
#define TOPOLOGY_CHANGE_CALC_TIME				8


#ifndef USE_INSTANCE_VARIABLES
#define USE_INSTANCE_VARIABLES
#endif


#define PROTO_DISBALE				-1
#define MSTP_PROTO_ID				0x00
#define PROTO_VERID_STP				0x00
#define PROTO_VERID_RSTP			0x02
#define PROTO_VERID_MSTP			0x03
#define PROTO_VERID_DEF				PROTO_VERID_MSTP


typedef enum
{
	MSTP_PORT_ROLE_DISABLED = 0x01,
	MSTP_PORT_ROLE_ALTERNATE,
	MSTP_PORT_ROLE_BACKUP,
	MSTP_PORT_ROLE_ROOT,
	MSTP_PORT_ROLE_DESIGNATED,
	MSTP_PORT_ROLE_MASTER,

}mstp_port_role_type;


#define MSTP_DEFAULT_SELECTOR		0
#define MSTP_DEFAULT_RV_LEVEL		0
#define MSTP_DEFAULT_BRIDGE_PRI		0x8000

/*instance id*/
#define MSTP_MSTI_MIN				1
#define MSTP_MSTI_MAX				8
#define MSTP_INSTANCE_MIN			1
#define MSTP_INSTANCE_MAX			8

/*vlan range*/
#define MSTP_VLAN_MIN_ID			1
#define MSTP_VLAN_MAX_ID			4094

#define MSTP_VLAN_MAX				4094				//4096
#define MSTP_VLAN_MAP_SIZE			((MSTP_VLAN_MAX_ID+7)/8)		/*value is 4096=512*8*/


/*bridge priority*/
#define MIN_BRIDGE_PRIORITY			0
#define DEF_BRIDGE_PRIORITY			32768
#define MAX_BRIDGE_PRIORITY			61440

/*hello timer*/
#define MIN_HELLO_TIMER				1
#define MSTP_DEFAULT_HELLO_TIME		2
#define MAX_HELLO_TIMER				10

/*forward delay*/
#define MIN_FORWARD_DELAY			4
#define MSTP_DEFAULT_FWD_DELAY		15
#define MAX_FORWARD_DELAY			30

/*max age*/
#define MIN_MAXAGE					6
#define MSTP_DEFAULT_MAX_AGE		20
#define MAX_MAXAGE					40

/*port priority*/
#define MIN_PORT_PRIORITY			0
#define DEF_PORT_PRIORITY			128
#define MAX_PORT_PRIORITY			240

/*max hop*/
#define MIN_MAXHOP					1
#define MSTP_DEFAULT_MAX_HOP		20
#define MAX_MAXHOP					40

/*time out*/
#define MIN_TIMEOUT					1
#define DEF_TIMEOUT					MSTP_DEFAULT_HELLO_TIME*3
#define MAX_TIMEOUT					600

/*port path cost*/
#define	MIN_PORT_PATH_COST			1
#define	DEF_PORT_PATH_COST			20000			//default port is 1000MF
#define	MAX_PORT_PATH_COST			200000000		



#define MSTP_DEC(x) if((x) != 0) (x) = (x) - 1



#define MSTP_ENCODE_U16_LENGTH(T,L)                               	 				\
	do {																			\
		ushort_value = htons(((unsigned short)(T) << 8) + ((unsigned short)(L))); 	\
		memcpy(buf, &short_value, 2);												\
		buf += 2;																	\
		} while (0)

#define MSTP_ENCODE_UINT32_LENGTH(T, LT, L, LL)										\
	do {																			\
		uint_value = htonl(((unsigned int)(T) << 24) + ((unsigned int)(LT) << 16) 	\
						+((unsigned int)(L) << 8) + ((unsigned int)(LL)));			\
		memcpy(buf, &uint_value, 4);												\
		buf += 4;																	\
	} while (0)


#ifndef NULL
#define			NULL		0
#endif



struct mstp_port_status
{
	unsigned char	port_status;
	unsigned char	port_status_str[64];

};

struct mstp_bridge_status
{
	unsigned char	port_status;
	unsigned char	port_status_str[64];

};


struct mstp_port_role
{	
	unsigned char	port_role;
	unsigned char	port_role_str[64];
};

struct list_head
{
	struct list_head *next;
	struct list_head *prev;
};

#define MSTP_HASH_BIT 				8
#define MSTP_HASH_SIZE 				(1 << MSTP_HASH_BIT)	/*value is 256*/

#define INIT_LIST_HEAD(ptr) 					\
do{ 											\
	(ptr)->next = (ptr); (ptr)->prev = (ptr); 	\
} while (0)

/**
 * list_entry-----get the struct for this entry
 * @ptr:			the &struct list_head pointer.
 * @type:		the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 */
#define list_entry(ptr, type, member)			\
((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/**
 * list_for_each_entry_safe----iterate over list of given type safe against removal of list entry
 * @pos:		the type * to use as a loop counter.
 * @n:			another type * to use as temporary storage
 * @head:		the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define list_for_each_entry_safe(pos, n, head, member)			\
for(pos = list_entry((head)->next, typeof(*pos), member),		\
	n = list_entry(pos->member.next, typeof(*pos), member);		\
	&pos->member != (head);										\
	pos = n, n = list_entry(n->member.next, typeof(*n), member))

/*mstp disable & enable status*/
typedef enum mstp_status
{
	MSTP_DISABLE,
	MSTP_ENABLE,
}mstp_enable_status;

typedef enum 
{
	MSTP_FALSE = 0x00,
	MSTP_TRUE

}mstp_bool;




enum PORT_ROLE
{
	ROEL_UNKNOWN 			= 0,	/*stp mode*/	
	ROEL_MASTER 			= 0,	/*mstp mode*/
	ROLE_ALTERNATE_BACKUP	= 1,	/*rstp & mstp mode*/
	ROLE_ROOT				= 2,	/*stp & rstp & mstp mode*/
	ROLE_DESIGNED			= 3		/*stp & rstp & mstp mode*/
};

enum PORT_STATE					/*need to confirm*/
{
	STATE_DISCARDING		= 0,
	STATE_LISTENING,
	STATE_LEARNING,
	STATE_FORWARDING,
	STATE_BLOCKING
};

enum MSTP_ROOT_ROLE
{
	MSTP_ROOT_DEF			= 0,
	MSTP_ROOT_MASTER,			/*master*/
	MSTP_ROOT_SLAVE				/*slave*/
};

/*if one vlan id mapping instance, vlan_start == vlan_end*/
struct mstp_vlan
{
	/*need to modify (vlan_start & vlan_end)---vlan_map use one*/

	/*if instance vlan_map can config vlan_list more than one, vlan_start & vlan_end no useful*/
	unsigned short			vlan_start;
	unsigned short			vlan_end;
	/*one bit which is vlan id map*/
	unsigned char			vlan_map[MSTP_VLAN_MAP_SIZE];	/*value is 4096=512*8*/

};


/*******************************************************/


/*compare info result*/
#define	INFO_BETTER				1
#define	INFO_SAME				0
#define	INFO_WORSE				-1


/*Bridge Detection state machine*/
typedef enum
{
	MSTP_EDGE = 0x01,
	MSTP_NOT_EDGE,
	MSTP_ISOLATED,
}bridge_detection_state_machine;

/*used for mstp port info*/
typedef enum
{
	MSTP_INFO_DISABLED = 0x01,			/*MSTP_INFO_DISABLED is default 0 */
	MSTP_INFO_AGED,
	MSTP_INFO_MINE,
	MSTP_INFO_RECEIVED
}mstp_port_info;

/*used for recording the result of compare (bpdu vector) with (port vector)*/
typedef enum
{
	MSTP_INFO_SUPERIOR_DESIGNATED,
	MSTP_INFO_REPEATED_DESIGNATED,
	MSTP_INFO_INFERIOR_DESIGNATED,
	MSTP_INFO_NOT_DESIGNATED,
	MSTP_INFO_OTHER
}mstp_info;



/************************************************/

/*Port Timers state machine*/
typedef enum
{
	MSTP_ONE_SECOND = 0x01,
	MSTP_TICK
}port_timer_state_machine;


/*Port Receive state machine*/
typedef enum
{
	MSTP_RECV_SM_DISCARD = 0x01,
	MSTP_RECV_SM_RECEIVE
}port_receive_state_machine;

/*Port Transmit state machine*/
typedef enum
{
	MSTP_TRANSMIT_INIT = 0x01,
	MSTP_IDLE,
	MSTP_TRANSMIT_PERIODIC,
	MSTP_TRANSMIT_CONFIG,
	MSTP_TRANSMIT_TCN,
	MSTP_TRANSMIT_RSTP
}port_transmit_state_machine;

/*Port Information state machine*/
typedef enum
{
	MSTP_DISABLED = 0x01,
	MSTP_AGED,
	MSTP_UPDATE,
	MSTP_CURRENT,
	MSTP_RECEIVE,
	MSTP_SUPERIOR_DESIGNATED,
	MSTP_REPEATED_DESIGNATED,
	MSTP_INFERIOR_DESIGNATED,
	MSTP_NOT_DESIGNATED,
	MSTP_OTHER,
}port_information_state_machine;

/*Port Role Selection state machine*/
typedef enum
{
	MSTP_INIT_BRIDGE = 1,
	MSTP_ROLE_SELECTION,

}port_role_selection_state_machine;

/*Figure 13-16 Port Role Transitions state machine-
Part 1: Disabled Roles*/
typedef enum
{
	MSTP_INIT_PORT = 0x01,
	MSTP_DISABLE_PORT,
	MSTP_DISABLED_PORT,

}mstp_port_role_trans_disabled_state_machine;

/*Figure 13-16 Port Role Transitions state machine-
Part 1: Alternate, and Backup Roles */
typedef enum
{
	MSTP_ALTERNATE_PORT = 0x01,
	MSTP_ALTERNATE_AGREED,
	MSTP_ALTERNATE_PROPOSED,
	MSTP_BLOCK_PORT,
	MSTP_BACKUP_PORT,

}mstp_port_role_trans_alternate_and_backup_state_machine;



/*Figure 13-17 Port Role Transitions state machine
Part 2: Root Roles */
typedef enum
{
	MSTP_ROOT_PORT = 0x01,
	MSTP_ROOT_PROPOSED,
	MSTP_ROOT_AGREED,
	MSTP_ROOT_SYNCED,
	MSTP_REROOT,
	MSTP_ROOT_FORWARD,
	MSTP_ROOT_LEARN,
	MSTP_REROOTED


}mstp_port_role_trans_root_state_machine;


/*Figure 13-17 Port Role Transitions state machine
Part 2: Designated Roles*/
typedef enum
{
	MSTP_DESIGNATED_PORT = 1,
	MSTP_DESIGNATED_PROPOSE,		
	MSTP_DESIGNATED_AGREED,
	MSTP_DESIGNATED_SYNCED,
	MSTP_DESIGNATED_RETIRED,
	MSTP_DESIGNATED_FORWARD,
	MSTP_DESIGNATED_LEARN,
	MSTP_DESIGNATED_DISCARD

}mstp_port_role_trans_designated_state_machine;

/*Figure 13-17 Port Role Transitions state machine
Part 2: Master Roles*/
typedef enum
{

/*The Bridge Port that is the CIST Root Port for the CIST Regional Root 
is the Master Port for all MSTIs*/
	MSTP_MASTER_PORT = 0x01,
	MSTP_MASTER_FORWARD,
	MSTP_MASTER_LEARN,
	MSTP_MASTER_DISCARD,
	MSTP_MASTER_PROPOSED,
	MSTP_MASTER_AGREED,
	MSTP_MASTER_SYNCED,
	MSTP_MASTER_RETIRED,

}mstp_port_role_trans_master_state_machine;



/*Port forward state in instance*/
typedef enum
{
	MSTP_STATE_DISCARDING,
	MSTP_STATE_BLOCKING,
	MSTP_STATE_LEARNING,
	MSTP_STATE_FORWARDING,
	MSTP_STATE_REMOVE,
	MSTP_STATE_INVALID = -1,

}port_forword_state_in_instance;

typedef enum port_state
{
    PORT_STATE_DISABLE,
    PORT_STATE_BLOCKING,
    PORT_STATE_LISTEN,
    PORT_STATE_LEARNING,
    PORT_STATE_FORWARDING,
	PORT_STATE_REMOVE,    
    PORT_STATE_INVALID = -1,

}mstp_port_state_e;


/*need to modify*/
struct mstp_ipc_msg
{
	unsigned int		instance_index;		/*value is bigger than 1 is valid*/
	unsigned int 		port_index;
	struct mstp_vlan	vlan_map;			/*this is right*/
	mstp_port_state_e	port_state;
};


/*MSTP STG subtype*/
typedef enum
{
	MSTP_STG_NULL,
	MSTP_STG_ADD = 0x01,
	MSTP_STG_SET,
}mstp_stg_type;


/*Port State Transition state machine*/
typedef enum
{
	MSTP_NULL,
	MSTP_ST_DISCARDING = 0x01,
	MSTP_ST_LEARNING,
	MSTP_ST_FORWARDING

}port_state_transition_state_machine;


/*Topology Change state machine*/
typedef enum
{
	MSTP_INACTIVE = 1,
	MSTP_LEARNING,
	MSTP_DETECTED,
	MSTP_ACTIVE,
	MSTP_NOTIFIED_TCN,
	MSTP_NOTIFIED_TC,
	MSTP_PROPAGATING,
	MSTP_ACKNOWLEDGED,

}port_topology_change_state_machine;


/*mstp protocol migration state machine change: no use just now */
typedef enum
{
	MSTP_CHECKING_RSTP = 0x01,
	MSTP_SELECTING_STP,
	MSTP_SENSING,
}port_protocol_migration_states;






#endif

