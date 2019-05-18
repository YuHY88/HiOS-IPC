/*定义pim mroute相关的结构体*/
#ifndef PIM_MROUTE_H
#define PIM_MROUTE_H

#include <lib/types.h>
#include <lib/hash1.h>
#include "../ipmc.h"
#include "../ipmc_main.h"
#include "../ipmc_if.h"
#include "pim.h"
#include "pim_join_prune.h"

#define  Keepalive_Period_default 210


#define PIM_BIT(x) (0x0001 << (x))
#define PIM_MRT_SM PIM_BIT(0)
#define PIM_MRT_SSM PIM_BIT(1)
#define PIM_MRT_SPT PIM_BIT(2)
#define PIM_MRT_LOCAL PIM_BIT(3)
#define PIM_MRT_UNUSED PIM_BIT(4)
#define PIM_MRT_CONNECTED PIM_BIT(5)
#define PIM_MRT_CACHE PIM_BIT(6)
#define PIM_MRT_SG PIM_BIT(7)
#define PIM_MRT_WC PIM_BIT(8)
#define PIM_MRT_RPT PIM_BIT(9)
#define PIM_MRT_PRUNED PIM_BIT(10)
#define PIM_MRT_SWT PIM_BIT(11)
#define PIM_MRT_SST PIM_BIT(12)
#define PIM_MRT_REG PIM_BIT(13)

#define PIM_MRT_UPSTREAM_TO_RP 0x1
#define PIM_MRT_UPSTREAM_TO_SRC 0x2

/* Switchover to the SPT  state machine event */
#define PIM_MRT_SWT_BASE 0x10
#define PIM_MRT_SWT_RCV_DATA (PIM_MRT_SWT_BASE + 1)
#define PIM_MRT_SWT_RCV_IGMP (PIM_MRT_SWT_BASE + 2)
#define PIM_MRT_SWT_RCV_LEAVE (PIM_MRT_SWT_BASE + 3)
#define PIM_MRT_SWT_NOPKT (PIM_MRT_SWT_BASE + 4)
#define PIM_MRT_SWT_UNUSED (PIM_MRT_SWT_BASE + 5)
#define PIM_MRT_SWT_USED (PIM_MRT_SWT_BASE + 6)
#if 0
/*使用位图来存储出接口*/
#define IPMC_IF_MAX 16
#define IFBIT_MASK (sizeof(uint8_t)*8)

#define IFBIT_SIZE ((PIM_IF_MAX) / (IFBIT_MASK))
struct if_set
{
	uint8_t if_bits[IFBIT_SIZE];
};

#define IF_ISSET(X, Y) (((Y)->if_bits[(X)/(IFBIT_MASK)]) & (1 << (((IFBIT_MASK) -1)-((X)%(IFBIT_MASK)))))
#define IF_SET(X, Y) (((Y)->if_bits[(X)/(IFBIT_MASK)]) |= (1 << (((IFBIT_MASK) -1)-((X)%(IFBIT_MASK)))))
#define IF_CLR(X, Y) (((Y)->if_bits[(X)/(IFBIT_MASK)]) &= ~(1 << (((IFBIT_MASK) -1)-((X)%(IFBIT_MASK)))))
#define IF_SETZERO(p)  (memset((p),(0), sizeof(*(p))))
#define IF_COPY(d, s, n)  (memcpy((d),(s), (n)))
#define IF_PLUS(d, s, i) ((d)->if_bits[(i)] |= (s)->if_bits[(i)])
#define IF_MINUS(d, s, i) ((d)->if_bits[(i)] &= ~((s)->if_bits[(i)]))
static inline uint32_t IF_ISZERO(const struct if_set *oifset)
{
	const struct if_set zero = {};

	return !memcmp(&zero, oifset, sizeof(struct if_set));
}
#endif
struct pim_mrt_oif_list
{
	struct if_set join_oif;						/*  joins(*,G) /  joins(S,G)*/
	struct if_set prune_oif;					/*  prunes(S,G,rpt) */
	struct if_set pim_in_oif;					/*  pim_include(*,G) /  pim_include(S,G) */
	struct if_set pim_ex_oif;					/*  pim_exclude(S,G)*/
	struct if_set lost_assert_oif;				/*  lost_assert(*,G) / lost_assert(S,G) */
	struct if_set res_oif;						/*  immediate_olist(*,G) / inherited_olist(S,G)*/
	uint32_t oif_num;
	//uint32_t oif_array[IPMC_IF_MAX];			/* immediate_olist(*,G) / inherited_olist(S,G)*/
};

struct pim_mrt_upstream
{
	uint32_t flag;
	uint32_t toward;
    uint32_t metric;
	uint32_t index;
	uint32_t ifindex;
	uint32_t rpf_nbr;
};

/*mrt_wc/mrt_sg 都存储在pim_instance下*/
struct pim_mrt_wc
{
	uint32_t flag;
	uint32_t grp_addr;
	uint32_t rp_addr;
	uint32_t rpf_nbr;
	uint32_t upstream_index;
	uint32_t upstream_ifindex;
	uint32_t oif_num;
	struct pim_mrt_oif_list oif;
	uint8_t jp_up_state;	/* Not Joined /Joined*/
};

struct pim_mrt_sg
{
	uint32_t flag;
	uint32_t src_addr;
	uint32_t grp_addr;
	uint32_t rp_addr;
	uint32_t rpf_nbr;
	uint32_t upstream_ifindex;
	uint32_t upstream_index;
	struct pim_mrt_oif_list oif;
	struct pim_mrt_wc *wc_mrt;
	uint8_t jp_up_state;				/* (*,G)/(S,G) Not Joined /Joined*/
	uint32_t rpf_intf_rp;				/*rpf interface rp*/
	uint8_t reg_state;					/*register state*/
	#if 0
	struct thread *keepalive_timer;		/*entry timer*/
	struct thread *rs_timer;			/*register-stop timer*/
	#endif
	TIMERID keepalive_timer;
	TIMERID rs_timer;
	uint8_t rs_tos;						/*register message tos*/
	uint8_t jp_up_rptstate;				/* Not Joined /Joined*/
};

struct pim_mrt_sg_grp
{
	uint32_t sg_num;
	uint32_t grp_addr;
	struct hash_table sg_table;
};

struct pim_mrt_sg_grp *pim_mrt_sg_grp_lookup(struct hash_table *table, uint32_t grp_addr);
struct pim_mrt_sg_grp *pim_mrt_sg_grp_add(struct hash_table *table, uint32_t src_addr, uint32_t grp_addr);
struct pim_mrt_sg *pim_mrt_sg_add(struct hash_table *table, uint32_t src_addr, uint32_t grp_addr);
struct pim_mrt_sg * pim_mrt_sg_get( uint32_t src_addr, uint32_t grp_addr, uint32_t create_flags);
uint32_t ip4_addr_local_main(uint32_t addr);
struct pim_mrt_upstream *pim_mrt_upstream_lookup(struct hash_table *table, uint32_t src_addr);
struct pim_mrt_upstream *pim_mrt_upstream_add(struct hash_table *table, uint32_t src_addr);

struct pim_mrt_upstream * pim_mrt_upstream_get( uint32_t src_addr, uint32_t flag);
uint32_t pim_mrt_sg_oif_cal(struct pim_mrt_sg *sg);
uint32_t pim_mrt_sg_oif_del(struct ipmc_if *pif, uint32_t src_addr, uint32_t grp_addr);
uint32_t immediate_olist_wc(struct pim_mrt_wc *wc);
uint32_t immediate_olist_sg(struct pim_mrt_sg *sg);
uint32_t inherited_olist_sgrpt(struct pim_mrt_sg *sg);
uint32_t inherited_olist_sg(struct pim_mrt_sg *sg);
uint32_t pim_prune_desired_sgrpt(struct pim_mrt_sg *sg);


uint32_t pim_mrt_sg_cache_add(struct pim_mrt_sg *sg);
uint32_t pim_rp_check( uint32_t grp_addr);
uint32_t pim_mrt_wc_oif_add(struct ipmc_if *pif, struct pim_down_jp_grp_node *grp_node);
struct pim_mrt_wc* pim_mrt_wc_lookup(struct pim_instance *ppim, uint32_t grp_addr);
struct pim_mrt_wc* pim_mrt_wc_add(struct pim_instance *ppim, uint32_t grp_addr);
struct pim_mrt_wc* pim_mrt_wc_get( uint32_t grp_addr);
uint32_t pim_mrt_wc_oif_cal(struct pim_mrt_wc *wc);
uint32_t pim_mrt_wc_oif_add(struct ipmc_if *pif, struct pim_down_jp_grp_node *grp_node);
uint32_t pim_mrt_wc_oif_del(struct ipmc_if *pif, uint32_t grp_addr);
void pim_jp_up_wc_state_machine(struct pim_mrt_wc *wc_mrt, struct pim_mrt_wc *old_wc_mrt, uint32_t event);
void pim_jp_up_sg_state_machine(struct pim_mrt_sg *sg_mrt, struct pim_mrt_sg *old_sg_mrt, uint32_t event);
void pim_jp_up_sg_rpt_state_machine(struct pim_mrt_sg *sg_mrt, uint32_t event);

sint32 pim_mrt_sg_keepalive_timer_expire(void *);
struct ifm_l3 *pim_getbulk_if_ip(int module_id, int *pdata_num);
//struct ifm_l3 *pim_get_if_ip(int module_id, uint32_t ifindex);
void pim_mrt_sg_keepalive_timer_set(struct pim_mrt_sg * mrt_sg, uint8_t set_flag);
void pim_mrt_wc_to_used(struct pim_mrt_wc *wc, struct pim_mrt_upstream *new_upstream);
void pim_mrt_wc_to_unused(struct pim_mrt_wc *wc, struct pim_mrt_upstream *new_upstream);
void pim_mrt_wc_iif_change(struct pim_mrt_wc *wc, struct pim_mrt_upstream *new_upstream);
void pim_mrt_wc_rpf_nbr_change(struct pim_mrt_wc *wc, struct pim_mrt_upstream *new_upstream);
void pim_mrt_wc_change(struct pim_mrt_wc *wc, struct pim_mrt_upstream *new_upstream);
void pim_mrt_wc_rp_change(struct pim_mrt_wc *wc, struct pim_mrt_upstream *new_upstream);
void pim_mrt_rp_change(struct pim_instance *ppim);
void pim_mrt_swt_state_machine(struct pim_mrt_sg *mrt_sg, uint32_t event);
uint32_t I_Am_Assert_Loser(struct pim_mrt_sg * mrt_sg, uint32_t iif);
void Update_SPTbit(struct pim_mrt_sg * mrt_sg, uint32_t iif);
uint32_t pim_local_reciever(struct pim_mrt_sg * sg);
uint32_t SwitchToSptDesired(struct pim_mrt_sg * sg);
uint32_t CheckSwitchToSpt(struct pim_mrt_sg * mrt_sg);
uint32_t RPTJoinDesired(struct pim_mrt_sg * mrt_sg);
uint32_t PruneDesiredSGRPT(struct pim_mrt_sg * mrt_sg);
uint32_t JoinDesiredWC(struct pim_mrt_wc * wc);
uint32_t JoinDesiredSG(struct pim_mrt_sg * mrt_sg);

#endif
