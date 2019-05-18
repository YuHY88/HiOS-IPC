#ifndef _QUAGGA_BGP_MSG_H
#define _QUAGGA_BGP_MSG_H

#include <lib/sockunion.h>
#include <lib/route_com.h>
#include <lib/prefix.h>

/*snmp get information*/
#define BGP_SNMP_GET_AS             0
#define BGP_SNMP_GET_REDISTRIBUTE   1
#define BGP_SNMP_GET_PEER           2
#define BGP_SNMP_GET_PUBLISH        3
#define BGP_SNMP_GET_SUMMARY        4
#define BGP_SNMP_GET_ROUTES         5

struct bgp_as_key
{
    u_int32_t as;
    struct in_addr router_id;
    uint8_t as_path:1,
            damped :1,
            med_diff:1,
            med_deter:1,
            unused:4;
    u_char distance_ebgp;
    u_char distance_ibgp;
    u_char distance_local;
    u_int32_t keepalive;
    u_int32_t holdtime;
    u_int32_t retrytime;
    u_int32_t local_pref;
    u_int32_t med;
    time_t half_life;
    u_int32_t reuse_limit;
    u_int32_t suppress_value;
    time_t max_suppress_time;
};

struct bgp_redistribute_key
{
    u_int32_t as;
    enum ROUTE_PROTO redist_type;
    u_char redist_instance;
    u_char redist_metric_flag;
    u_int16_t vrf_id;
    u_int32_t redist_metric;
};

struct bgp_peer_key
{
    u_int32_t local_as;
    struct in_addr remote_ip;
    u_int32_t remote_as;
    char desc[81];
    u_char enable:1,
           ttl_enable:1,
           send_default:1,
           send_ibgp:1,
           nexthop_change_local:1,
           exclude_private_as:1,
           route_limit:1,
           allow_enable:1;
    u_char allow_in;
    u_char vpnv4_enable:1,
		   unused:7;
    struct in_addr source_ip;
    u_int32_t ttl;
    u_int32_t change_local_as;
    u_int32_t weight;
    u_int32_t routeadv;
	uint32_t ifindex_tnl;
    uint64_t route_max;
	uint16_t vrf_id;
	char     binding_tunnel_des[81];
	int32_t  negr_status;
	char     md5_password[81];
};

struct bgp_publish_key
{
    u_int32_t local_as;
    struct prefix p; 
	struct in_addr router_mask;
	u_int32_t reserved;
};

struct bgp_summary_key
{
    u_int32_t local_as;
    struct prefix p; 
	struct in_addr router_mask;
	u_char as_set;
    u_char summary_only;		   
};

struct bgp_route_key
{
    struct prefix p; 
	struct in_addr router_mask; 
    u_int32_t vpn_id;
    struct in_addr next_hop;
    u_int32_t metric;
	int32_t preference;
	u_int32_t med;
};

extern void bgp_pkt_register(void);
extern void bgp_pkt_unregister(void);

#endif /* _QUAGGA_BGP_MSG_H */

