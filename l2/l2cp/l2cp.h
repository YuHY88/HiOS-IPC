#ifndef _L2CP_H_
#define _L2CP_H_

#include "../l2_if.h"
#include <lib/types.h>

#define L2CP_DEBUG_ENABLE	1
#define	L2CP_DEBUG_DISABLE	0

#define ERROR_ACL_UPDATE -5
#define ERROR_EGRESS_DELETE_VLAN -4
#define ERROR_PORT_MESS -3
#define ERROR_LINK_DOWN -2
#define L2CP_ERROR		-1
#define L2CP_SUCCESS	0

#define	COS_DEFAULT  	6
#define	VLAN_INVALID 	4095
#define ETHTYPE_INVALID 0x0
#define VLAN_MAX_PRIORITY 7
#define VSI_MAX_PW_CONUT  16


/* Layer 2 protocol list */
typedef enum hal_l2_proto
  {
    HAL_PROTO_STP,
    HAL_PROTO_RSTP,
    HAL_PROTO_MSTP,
    HAL_PROTO_LACP,
    HAL_PROTO_DOT1X,
    HAL_PROTO_LLDP,
    HAL_PROTO_EOAM,
    
    HAL_PROTO_ALL,
    HAL_PROTO_MAX,
  } hal_l2_proto_t;

struct l2cp_dmac
{
	u_int8_t mac[6];
	u_int16_t type;
};

enum  l2cp_action
{
	L2CP_TUNNEL 	   = 0,
	L2CP_DISCARD	   = 1,
	L2CP_NOACTION      = 2,
};
	
enum  l2cp_mode
{
	L2CP_BASE_MAC   = 0,
	L2CP_BASE_VLAN  = 1,
	L2CP_BASE_VPWS  = 2,
	L2CP_BASE_VPLS  = 3,
};

enum l2cp_vlan_action
{
  UNDO_EGRESS_DELETE_VLAN  = 0,
  EGRESS_DELETE_VLAN  = 1,
};


enum l2cp_mpls_status
{
	L2CP_MPLS_ACTIVE = 0,
	L2CP_MPLS_INACTIVE,

	L2CP_MPLS_INVALID,
};

struct l2cp_base_mac
{
	u_int8_t mac[6];
	u_int16_t vlan;
	u_int8_t  cos;
};

struct l2cp_base_mpls
{
	int       mpls_port_id;
	u_int32_t vc_id;
	u_int32_t entry;
	u_int32_t ac_ifindex;
	enum l2cp_mpls_status status;
	u_int32_t vlan_id;
	u_int32_t nni_port_number;
	int       nni_bcm_port[VSI_MAX_PW_CONUT];
};

struct l2cp_info
{
	struct l2cp_process *master;

	struct l2cp_info *next;
	struct l2cp_info *prev;
    
	enum l2cp_action l2cpAction;
	enum  l2cp_mode  l2cpMode;
	enum l2cp_vlan_action vlan_action;
	
	u_int8_t dmac[6];
	u_int16_t etherType;
	u_int32_t proto_type;

	struct l2cp_base_mac mac_info;
	struct l2cp_base_mpls mpls_info;
};

struct l2cp_snmp_info
{
	u_int32_t ifindex;
	u_int32_t protoTypeIndex;
	
	enum l2cp_action l2cpAction;
	enum  l2cp_mode  l2cpMode;
	u_int8_t dmac[6];
	u_int16_t etherType;
	
	u_int8_t gmac[6];
	u_int16_t gvlan;
	u_int8_t  gcos;
	u_int32_t vc_id;
	enum l2cp_mpls_status status;
	
};

struct l2cp_process
{
	struct l2cp_info *head;
	struct l2cp_info *tail;
};



int l2cp_info_master_init(struct l2if * p_l2if);
int l2cp_info_master_deinit(struct l2if * p_l2if);
int l2cp_tunnel_process(struct l2if * p_l2if, enum hal_l2_proto proto,enum l2cp_mode mode,
			u_int8_t *dmac, u_int16_t ethertype, u_int8_t *gmac, u_int16_t vlan, u_int8_t cos);
int l2cp_discard_process(struct l2if * p_l2if, enum hal_l2_proto proto, 
			u_int8_t *dmac, u_int16_t ethertype, enum l2cp_action action);
int hal_l2cp_tunnel_process(uint32_t ifindex,enum hal_l2_proto proto, enum l2cp_mode mode,
			u_int8_t *dmac, u_int16_t ethertype, u_int8_t *gmac, u_int16_t vlan, u_int8_t cos);
enum hal_l2_proto l2cp_str_to_protocol (u_int8_t *proto_str);
void l2cp_cli_init(void);
int l2if_l2cp_config_write(struct vty *vty, struct l2cp_process *l2cp_master);
enum hal_l2_proto l2cp_str_to_protocol (u_int8_t *proto_str);
int l2cp_no_action(struct l2if * p_l2if, enum hal_l2_proto proto, 
			u_int8_t *dmac, u_int16_t ethertype);
const char *l2cp_proto_to_str (hal_l2_proto_t proto);
int l2cp_show_config(struct vty *vty, struct l2if * p_l2if,enum l2cp_action action,enum l2cp_mode l2cpMode);
void l2cp_init(void);
int l2cp_mpls_process(struct l2if * p_l2if, enum hal_l2_proto proto, u_int8_t * dmac,
					   u_int16_t ethertype,enum l2cp_mode mode, uint32_t vc_id);

void
l2cp_info_free(struct l2cp_info *l2cp_ptr);
void
l2cp_info_delete(struct l2cp_info *l2cp_ptr);
void
l2cp_info_clean (struct l2cp_process *master);
struct l2cp_info *
l2cp_info_insert(struct l2cp_process *master, u_int8_t *mac,
			u_int16_t ethertype, u_int32_t proto_type);
struct l2cp_info *
l2cp_info_lookup_by_proto(struct l2cp_process *master, 
 	u_int8_t *mac, u_int16_t ethertype, u_int32_t proto_type);
struct l2cp_info *
l2cp_info_get(struct l2cp_process *master, u_int8_t *mac, 
		u_int16_t ethertype, u_int32_t proto_type);

int 
hal_l2cp_mpls_process(uint32_t ifindex,enum hal_l2_proto proto, u_int8_t *dmac, 
						u_int16_t ethertype, enum l2cp_mode mode, uint32_t vc_id);
int 
hal_l2cp_discard_process(uint32_t ifindex,enum hal_l2_proto proto, 
			u_int8_t *dmac, u_int16_t ethertype);
int 
hal_l2cp_noaction_process(uint32_t ifindex,enum hal_l2_proto proto, 
			u_int8_t *dmac, u_int16_t ethertype);

int 
l2cp_config_write_process(struct vty *vty, struct l2cp_info *pl2cp_info,u_int32_t proto);
void 
delete_l2cp_config(uint32_t ifindex, uint32_t mode);

#endif
