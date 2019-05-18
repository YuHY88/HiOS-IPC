/*定义pim 邻居/DR  相关的结构体，宏*/
#ifndef PIM_NEIGHBOR_H
#define PIM_NEIGHBOR_H

#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/linklist.h>
#include <lib/thread.h>
#include <lib/hptimer.h>

#include "../ipmc_if.h" 

/* Interface is DR */
#define PIM_DR_LOCAL 1

/* PIM neighbor definition */
#define PIM_HELLO_PERIOD_DEFAULT 30                                            /* Default 30 secs, send hello message interval */
#define PIM_HELLO_HOLDTIME_DEFAULT (3.5 * (PIM_HELLO_PERIOD_DEFAULT))     /* Default holdtime to keep neighbor state alive */
#define PIM_DR_PRIORITY_DEFAULT 1                     /* DR default priority is 1. */
#define PIM_LAN_DELAY_DEFAULT 500				  /* hello message lan delay 500(ms)*/
#define PIM_OVERRIDE_DEFAULT 2500				  		  /* hello message override 2500(ms)*/

/*option type 2B, option len 2B*/
#define PIM_OPTION_TYPE_LEN 		2
#define PIM_OPTION_LENGTH_LEN 	2
#define PIM_OPTION_HOLDTIME_LEN 	2
#define PIM_OPTION_LAN_DELAY_LEN 4
#define PIM_OPTION_DR_PRIORITY_LEN 4
#define PIM_OPTION_GEN_ID_LEN 4

/*PIM Hello option type*/
#define PIM_HELLO_OPTION_HOLDTIME      1
#define PIM_HELLO_OPTION_LAN_DELAY     2
#define PIM_HELLO_OPTION_DR_PRIORITY   19
#define PIM_HELLO_OPTION_GEN_ID        20
#define PIM_HELLO_OPTION_ADDR_LIST     24

/* PIM hello option : gen_id must exist, other may exist*/
#define PIM_HELLO_HOLDTIME_PRESENT 0x01
#define PIM_HELLO_LAN_DELAY_PRESENT 0x02
#define PIM_HELLO_DR_PRI_PRESENT 0x04
#define PIM_HELLO_ADDRLIST_PRESENT 0x08

/*PIM Hello message opt info*/
struct pim_hello_opt
{
    uint16_t opt_flag;					/* present option flag : holdtime, lan_delay,  dr_pri, sec_addr, state_refresh*/
    uint16_t holdtime;					/* del nbr if 0*/	
    uint16_t override;					
    uint16_t lan_delay;						
    uint32_t gen_id;					/* must exist */
    uint32_t dr_priority;
	uint32_t nbr_addr;					/* neighbor address*/
    uint32_t second_addr_num;
    uint32_t second_addr;				/*store one second address*/
};

/*decap pim option type len*/
struct pim_hello_opt_type_len
{
	uint16_t opt_type;
	uint16_t opt_len;
};

/*PIM Hello option */
struct pim_hello_opt_holdtime
{
	uint16_t opt_type;
	uint16_t opt_len;
	uint16_t holdtime;
};

struct pim_hello_opt_gen_id
{
	uint16_t opt_type;
	uint16_t opt_len;
	uint32_t gen_id;
};

struct pim_hello_opt_dr_pri
{
	uint16_t opt_type;
	uint16_t opt_len;
	uint32_t dr_priority;
};

struct pim_hello_opt_lan_delay
{
	uint16_t opt_type;
	uint16_t opt_len;
	uint16_t lan_delay;
	uint16_t override;
};

struct pim_dr_state
{
    uint32_t dr_state;                  /* Current DR state */
    uint32_t dr_priority;               /* Current DR Priority */
    uint32_t dr_addr;        			/* Current DR address */
};

struct pim_neighbor_node
{
    uint16_t opt_flag;                  /* PIM neighbor option_flag*/
    uint16_t t_bit;                     /* T bit */
    uint16_t override;                  /* Override Interval */
    uint16_t lan_delay;                 /* LAN Delay */
    uint32_t gen_id;                    /* Generation ID */
    uint32_t dr_priority;               /* DR Priority */
    uint32_t nbr_addr;					/* Neighbor address */
    uint32_t second_addr;      			/* secondary addresses */
    void    *to_if;                     /* Point to mc_if */    
    #if 0
    struct thread *nlt_timer;         	/* Neighbor Liveness Timer, delete nbr when expire */
	#endif
	TIMERID nlt_timer;
	struct hash_table up_wc_jp;		    /* Send upstream (*,G) SWR to this neighbor (struct pim_up_jp_grp_node) */
	struct hash_table up_sg_jp;			/* Send upstream (S,G)/(S,G,RPG)  S/SR */
};

struct pim_neighbor
{
	uint32_t gen_id;					/* if start ,set gen_id*/
    uint32_t nbr_num;                   /* PIM neighbor count */
    struct pim_dr_state cur_dr;         /* Current DR */
	#if 0
    struct thread *hello_timer;  	    /* PIM Hello Timer, send hello when expire, default 30s*/
	#endif
	TIMERID hello_timer;
	struct list neighbor_list;
};

struct pim_neighbor_node* pim_nbr_node_create(struct pim_neighbor *pnbr, struct pim_hello_opt *options);
sint32 pim_nbr_node_add(struct pim_neighbor *pnbr, struct pim_neighbor_node *pnbr_node);
sint32 pim_nbr_node_delete(struct pim_neighbor *pnbr, struct pim_neighbor_node *pnbr_node);
struct pim_neighbor_node* pim_nbr_node_lookup(struct pim_neighbor *pnbr, uint32_t nbr_addr);
sint32 pim_nbr_ntl_timer_expire(void *para);
sint32 pim_nbr_option_update(struct ipmc_if *pif, struct pim_neighbor_node *pnbr_node, struct pim_hello_opt *hello_options);
sint32 pim_dr_is_better(struct pim_dr_state *dr, struct pim_neighbor_node *pnbr_node, uint16_t dr_pri_elec);
sint32 pim_dr_election(struct ipmc_if *pif);
sint32 pim_hello_send_timer_expire(void *para);
sint32 pim_hello_recv(struct ipmc_if *pif, uint32_t sip, uint8_t *pim_para, uint32_t pim_para_len);
int pim_hello_send(struct ipmc_if *pif, uint32_t reset_timer);
void pim_nbr_stop(struct ipmc_if *pif);
void pim_nbr_start(struct ipmc_if *pif);


#endif
