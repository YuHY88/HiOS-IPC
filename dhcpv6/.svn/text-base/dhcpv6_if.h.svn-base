#ifndef HIOS_DHCPV6_IF_H
#define HIOS_DHCPV6_IF_H

#define IFID_LEN_DEFAULT 64
#define SLA_LEN_DEFAULT 16

struct dhcp6_poolspec {
	char* name;
	u_int32_t pltime;
	u_int32_t vltime;
};

struct dhcp6_range {
	struct in6_addr min;
	struct in6_addr max;
};
/*
struct pool_conf {
	struct pool_conf *next;

	char* name;

	struct in6_addr min;
	struct in6_addr max;
};
*/
/* run-time authentication parameters */
struct authparam {
	int authproto;
	int authalgorithm;
	int authrdm;
	struct keyinfo *key;
	int flags;
#define AUTHPARAM_FLAGS_NOPREVRD	0x1

	u_int64_t prevrd;	/* previous RD value provided by the peer */
};

typedef enum 
{ 
	DHCP6_EVDATA_IAPD, 
	DHCP6_EVDATA_IANA 
} dhcp6_eventdata_t;

struct dhcp6_eventdata {
	TAILQ_ENTRY(dhcp6_eventdata) link;

	struct dhcp6_event *event;
	dhcp6_eventdata_t type;
	void *data;

	void (*destructor)(struct dhcp6_eventdata *);
	void *privdata;
};

/* client status code */
enum {
	DHCP6S_INIT, 
	DHCP6S_SOLICIT,
	DHCP6S_INFOREQ,
	DHCP6S_REQUEST,
	DHCP6S_RENEW,
	DHCP6S_REBIND,
	DHCP6S_RELEASE,
	DHCP6S_IDLE
};

struct prefix_ifconf {
	TAILQ_ENTRY(prefix_ifconf) link;

	char *ifname;		/* interface name such as ne0 */
	int sla_len;		/* SLA ID length in bits */
	u_int32_t sla_id;	/* need more than 32bits? */
	int ifid_len;		/* interface ID length in bits */
	int ifid_type;		/* EUI-64 and manual (unused?) */
	char ifid[16];		/* Interface ID, up to 128bits */
};

typedef enum { 
	IATYPE_PD,
	IATYPE_NA,	
	IATYPE_TA
} iatype_t;

enum { 
	DECL_SEND,
	DECL_ALLOW,
	DECL_INFO_ONLY,
	DECL_REQUEST,
	DECL_DUID,
	DECL_PREFIX,
	DECL_PREFERENCE,
	DECL_SCRIPT,
	DECL_DELAYEDKEY,
	DECL_ADDRESS,
	DECL_RANGE,
	DECL_ADDRESSPOOL,
	IFPARAM_SLA_ID,
	IFPARAM_SLA_LEN,
	DHCPOPT_RAPID_COMMIT,
	DHCPOPT_AUTHINFO,
	DHCPOPT_DNS,
	DHCPOPT_DNSNAME,
	DHCPOPT_IA_PD,
	DHCPOPT_IA_NA,
	DHCPOPT_NTP,
	DHCPOPT_REFRESHTIME,
	DHCPOPT_NIS,
	DHCPOPT_NISNAME, 
	DHCPOPT_NISP,
	DHCPOPT_NISPNAME, 
	DHCPOPT_BCMCS,
	DHCPOPT_BCMCSNAME, 
	CFLISTENT_GENERIC,
	IACONF_PIF,
	IACONF_PREFIX,
	IACONF_ADDR,
	DHCPOPT_SIP,
	DHCPOPT_SIPNAME,
	AUTHPARAM_PROTO,
	AUTHPARAM_ALG,
	AUTHPARAM_RDM,
	AUTHPARAM_KEY,
	KEYPARAM_REALM,
	KEYPARAM_KEYID,
	KEYPARAM_SECRET,
	KEYPARAM_EXPIRE
};

typedef enum {
	DHCP6_MODE_SERVER,
	DHCP6_MODE_CLIENT,
	DHCP6_MODE_RELAY
}dhcp6_mode_t;

extern const dhcp6_mode_t dhcp6_mode;

extern struct dhv6_interface *dhcp6_if;
extern struct dhcp6_ifconf *dhcp6_iflist;
extern struct prefix_ifconf *prefix_ifconflist;
extern long long optrefreshtime;

extern struct list dhcpv6_if_list;
struct keyinfo *find_key(char *realm, size_t realmlen, u_int32_t id);
//struct ia_conf *find_iaconf(struct ia_conflist *head, iatype_t type, u_int32_t iaid);
int in6_addr_cmp(struct in6_addr *addr1, struct in6_addr *addr2);

//int dhcpv6_if_add(struct dhcpv6_if *pif);
int dhcpv6_if_delete(uint32_t ifindex);
struct dhcpv6_if *dhcpv6_if_lookup(uint32_t ifindex);
int dhcpv6_client_enable(int ifindex);
int dhcpv6_client_disable(int ifindex);
int dhcpv6_client_pd_enable(int ifindex);
int dhcpv6_client_pd_disable(int ifindex);
struct dhcpv6_event{
	uint32_t state;
	uint32_t timeouts;
	struct timeval tv_start; /* timestamp when the 1st msg is sent */
};

struct ia_status_code{
	uint16_t code;/* ia 子option中 选项代码 */
	uint8_t *msg;/* ia 子option中 选项消息为UTF-8文本串 */
};

/* 前缀信息 */
struct iapd_conf{
	uint8_t  prefix_addr[16];
	uint32_t mask;
	uint8_t  prefix_len;
	uint32_t pre_time;/* 优先生存期 */
	uint32_t valid_time;/* 合法生存期 */
	struct ia_status_code status_code;/* 状态码 */
};


/* ipv6地址信息 */
struct iana_conf{
	uint8_t  addr[16];
	uint32_t mask;
	uint32_t pre_time;/* 优先生存期 */
	uint32_t valid_time;/* 合法生存期 */
	struct ia_status_code status_code;/* 状态码 */
};

struct ia_conf{
	iatype_t ia_type;/* ia type IATYPE_NA or IATYPE_PD */
	uint32_t iaid;
	uint32_t ifindex;
	uint32_t t1;
	uint32_t t2;
	union {
		struct iana_conf iana;
		struct iapd_conf iapd;
	}ia;
};

struct dhcpv6_if{
	uint32_t ifindex;
	u_long send_flags;
	uint32_t xid;
	struct dhcpv6_event event;
	struct ia_conf ia;
	struct duid server_id;
	struct duid client_id;
	uint32_t rapidcommit;
	int64_t refreshtime;
};

int dhcpv6_if_add(struct dhcpv6_if *pif);

#endif
