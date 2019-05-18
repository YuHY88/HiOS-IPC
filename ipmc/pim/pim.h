/*定义pim配置及相关的结构体*/
#ifndef PIM_H
#define PIM_H

#include <lib/types.h>

#include "../ipmc_if.h"
#include "pim.h"

#define PIM_INSTANCE_MAX 256
//#define PIM_IF_MAX 16
#define PIM_MINLEN		8
#define PIM_SM 1
#define PIM_DM 2
#define PIM_SSM_ENABLE 1
#define PIM_SSM_DISABLE 0 

struct pim_udphdr 
{
	uint16_t	sport;    /* destination port*/
	uint16_t	dport;    /* source port*/
	uint16_t	len;      /* udp 头长度 + payload 长度*/
	uint16_t	checksum;
};

struct pim_iphdr
{
#if BIG_ENDIAN_ON
	unsigned int ip_v:4; /* version */
    unsigned int ip_hl:4; /* header length */
#else
    unsigned int ip_hl:4; /* header length */
    unsigned int ip_v:4; /* version */
#endif
    uint8_t ip_tos; /* type of service */
    uint16_t ip_len; /* total length */
    uint16_t ip_id; /* identification */
    uint16_t ip_off; /* fragment offset field */
#define IP_RF 0x8000 /* reserved fragment flag */
#define IP_DF 0x4000 /* dont fragment flag */
#define IP_MF 0x2000 /* more fragments flag */
#define IP_OFFMASK 0x1fff /* mask for fragmenting bits */
    u_int8_t ip_ttl; /* time to live */
    u_int8_t ip_p; /* protocol */
    uint16_t ip_sum; /* checksum */
    uint32_t ip_src;
	uint32_t ip_dst; /* source and dest address */
};


/* Check if group is in PIM-SSM range, x must be in network byte order */
#define IN_PIM_SSM_RANGE(x) (((unsigned)(x) & 0xff000000) == 0xe8000000)

/*
** Boolean values
*/
typedef enum
{
  PIM_FALSE = 0,                        /* Everybody calls zero false... */
  PIM_TRUE = (!PIM_FALSE)       /* Some want TRUE=1 or TRUE=-1 or TRUE!=0 */
}
pim_bool_t;

struct pim_config
{
	uint16_t ssm_state;					/* PIM SSM state, default disable :0*/
    uint16_t hello_itv;                 /* PIM Hello interval */
    uint16_t override;				  	/* Override Interval ,default 2500ms*/
    uint16_t lan_delay;			     	/* LAN Prune Delay ,default 500ms*/
    uint32_t dr_priority;            	/* interface DR priority, default 1 */
};

struct pim_statistics
{
	/*hello stat*/
	uint32_t pimstat_hello_msg_rx;			
	uint32_t pimstat_hello_msg_tx;
	uint32_t pimstat_hello_msg_rx_err;
	/*register stat*/
	uint32_t pimstat_reg_msg_rx;
	uint32_t pimstat_reg_msg_tx;
	uint32_t pimstat_reg_msg_rx_err;
	/*register stop stat*/
	uint32_t pimstat_rs_msg_rx;
	uint32_t pimstat_rs_msg_tx;
	uint32_t pimstat_rs_msg_rx_err;
	/*join prune stat*/	
	uint32_t pimstat_jp_msg_rx;
	uint32_t pimstat_jp_msg_tx;
	uint32_t pimstat_jp_msg_rx_err;
	/*bootstrap stat*/
	uint32_t pimstat_bs_msg_rx;
	uint32_t pimstat_bs_msg_tx;
	uint32_t pimstat_bs_msg_rx_err;
	/*assert stat*/
	uint32_t pimstat_assert_msg_rx;
	uint32_t pimstat_assert_msg_tx;
	uint32_t pimstat_assert_msg_rx_err;
	/*candidate_rp*/
	uint32_t pimstat_crp_msg_rx;
	uint32_t pimstat_crp_msg_tx;
	uint32_t pimstat_crp_msg_rx_err;

	/*data msg dip is not a multicast*/
	uint32_t pimstat_not_mcast_dip_msg;
	/*data msg src ip is not same net */
	uint32_t pimstat_not_same_subnet_msg;
	
	/*other err*/
	uint32_t pimstat_unknown_type_msg;
	uint32_t pimstat_unknown_version_msg;
	uint32_t pimstat_nbr_unknown_msg;
	uint32_t pimstat_bad_length_msg;
	uint32_t pimstat_bad_checksum_msg;

	uint32_t pimstat_rx_intf_disabled_msg;								/*rx interface not enable pim*/
	uint32_t pimstat_rx_reg_not_rp;											/*rx register, but not rp*/
	uint32_t pimstat_unknown_rs;												/*unknown register stop*/

	uint32_t pimstat_rx_crp_not_bsr;
	uint32_t pimstat_rx_bsr_not_rpf_intf;
	uint32_t pimstat_rx_unknown_hello_option;	
	uint32_t pimstat_rx_aggregate;											/*agg intf*/
	uint32_t pimstat_rx_malformed_packet;							/*malformed packet*/
	uint32_t pimstat_no_rp; 		
	uint32_t pimstat_no_route_upstream; 	
	uint32_t pimstat_rp_mismatch; 		
	uint32_t pimstat_rpf_nbr_unknown;	
	
	uint32_t pimstat_rx_join_rp;
	uint32_t pimstat_rx_prune_rp;
	uint32_t pimstat_rx_join_wc;
	uint32_t pimstat_rx_prune_wc;
	uint32_t pimstat_rx_join_sg;
	uint32_t pimstat_rx_prune_sg;
	uint32_t pimstat_rx_join_sg_rpt;
	uint32_t pimstat_rx_prune_sg_rpt;
};

struct pim_instance
{
	uint8_t pim_id;				    	/* pim instance id */
	//uint8_t pim_if_num;					/* pim instance contain interface count*/
	//uint32_t pim_if_array[PIM_IF_MAX];	/* pim instance contain interface*/
	uint32_t rp_addr;					/* bsr rp ...config info*/
	struct pim_config pim_conf;		    /* pim instance config*/
	struct hash_table pim_wc_table;		/* pim  wc*/
	struct hash_table pim_sg_table;		/* pim  sg*/
	struct hash_table same_src_table;	/* same src upstream info*/
	struct pim_statistics pim_stat;
};

extern struct pim_instance *pim_instance_global;

//extern struct pim_instance *pim_instance_array[PIM_INSTANCE_MAX];
//struct pim_instance * pim_instance_create(uint8_t pim_id);
//sint32 pim_instance_delete(uint8_t pim_id);
void pim_instance_init();
extern void pim_mrt_sg_grp_table_init(struct hash_table *table, uint32_t size);
#endif
