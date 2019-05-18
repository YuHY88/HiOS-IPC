#ifndef VTYSH_SNMP_H
#define VTYSH_SNMP_H


#include <lib/vty.h>
#include <lib/msg_ipc_n.h>


/* vty subtype */
enum VTY_MIB_SUBTYPE
{
	/********** scalar subtype ***********/
	VTY_MIB_COMM_CFG_SCALAR,
	
	/********** table subtype ***********/
	VTY_MIB_U0_INFO_TABLE,
	
};


typedef struct _vty_mib_comm_cfg
{
	char hostname[VTYSH_HOST_NAME_LEN+1];
	
} VTY_MIB_COMM_CFG;


/**************************** U0信息数据结构定义 **********************************/

typedef struct vty_u0_info_index
{
	uint32_t ipv4;
	
} VTY_U0_INFO_INDEX;

typedef struct vty_u0_info_data
{
	char hostname[VTYSH_HOST_NAME_LEN+1];
	
} VTY_U0_INFO_DATA;


typedef struct vty_u0_info
{
	VTY_U0_INFO_INDEX 	index;	
	VTY_U0_INFO_DATA 	data;
	
} VTY_U0_INFO;



void vty_handle_snmp_msg(struct ipc_mesg_n *pmsg);




#if 0
enum vty_scalar_subtype {
	VTYSH_USER_TIME_OUT = 1,
	VTYSH_USER_MODE
};


enum vty_table_subtype {
	VTYSH_USER = 1,
	VTYSH_ONLINE_USER
};

struct vtysh_snmp_user_index{
       char username[32];
};
struct vtysh_snmp_user_data{
       int          privilege;
};

struct vtysh_snmp_user_info{
      struct vtysh_snmp_user_index index;
      struct vtysh_snmp_user_data  data;
};

struct vtysh_snmp_online_user_index{
       int usernum;
};
struct vtysh_snmp_online_user_data{
       char username[32];
       char userfrom[32];
       int  userprivilege;
};
struct vtysh_snmp_online_user_info{
       struct vtysh_snmp_online_user_index index;
       struct vtysh_snmp_online_user_data     data;
};

#endif


#endif

