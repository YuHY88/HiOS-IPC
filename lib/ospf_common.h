#ifndef HIOS_OSPF_COMMON_H
#define HIOS_OSPF_COMMON_H

#include <lib/prefix.h>
#include <lib/vty.h>


#define OSPF_DCN_USE_LOOPBACK_ID				(128)
#define OSPF_DCN_USE_VLAN						(4094)

enum IPC_SUB_TYPE_OSPF
{
	IPC_SUB_TYPE_INVALID = 0,
	IPC_SUB_TYPE_VPN,
	IPC_SUB_TYPE_NE_IP,
	IPC_SUB_TYPE_DCN_STATUS
};

/* U0 Device manager*/
struct u0_device_info
{
	struct in_addr ne_id;
	struct in_addr ne_ip;
	char dcn_ne_device_type[64];
	char dcn_ne_vendor[64];
	uint8_t  mac[6];
	uint32_t ifindex;						//U3上与U0连接的接口
	char hostname[VTYSH_HOST_NAME_LEN + 1];		//用来传递U0设备hostname，以便EFM和vty使用
};

struct u0_device_info_local
{
	struct u0_device_info u0_info;
	uint32_t loopback_ifindex;				//虚拟出的U0设备ip，在U3上终结的接口.
};

extern int get_ospf_dcn_status(int module_id);


#endif

