/*
*    loop-detect define
*/


#ifndef HIOS_LOOP_DETECT_H
#define HIOS_LOOP_DETECT_H

#include <lib/ether.h>
#include <ftm/pkt_eth.h>
#include <l2/l2_if.h>
#include "lib/hptimer.h"

#define LOOPD_DEF_VLAN_ID  4095
#define LOOPD_DEF_RES_TIME 30
#define LOOPD_DEF_INTERVAL 1
#define LOOPD_DEF_TLV_MAX 8

#define LOOPD_SUBTYPE (0x0003)
#define LOOPD_VERSION (0x0000)

enum LOOPD_FLAG
{
	LOOPD_FORWARD = 0,
	LOOPD_LOOPBACK
};

/*loopd ~{D?5D5XV7~}*/
#define LOOPD_MC_MAC  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}

/* TLV Types */
#define LOOPD_EOP_TLV           0
#define LOOPD_DEV_ID_TLV        1
#define LOOPD_PORT_IFINDEX_TLV  2
#define LOOPD_PORT_NAME_TLV	    3
#define LOOPD_SYS_NAME_TLV	    4
#define LOOPD_CHASSIS_ID_TLV    5
#define LOOPD_SLOT_ID_TLV       6
#define LOOPD_SUB_SLOT_TLV      7

#define LOOPDETECT_TIMER_ON_LOOP(T,F,S,V) \
			do { \
				if (! (T)) \
				(T) = high_pre_timer_add ("loopdetect_timer",LIB_TIMER_TYPE_LOOP, (F), (S), (V)*1000); \
			} while (0)

#define LOOPDETECT_TIMER_ON_UNLOOP(T,F,S,V) \
							do { \
								if (! (T)) \
								(T) = high_pre_timer_add ("loopdetect_timer",LIB_TIMER_TYPE_NOLOOP, (F), (S), (V)*1000); \
							} while (0)

#define LOOPDETECT_TIMER_OFF(T) \
			do { \
				if (T) \
				{ \
					high_pre_timer_delete (T); \
					(T) = 0; \
				} \
			} while (0)

/* ~{6K?Z;7B7<l2b5DJ}>]=a99~} */
struct loop_detect
{
	uint16_t vlan;          /* ~{7"KM:M=SJU1(ND5D~} vlan~{#,D,HO~} 4096 ~{1mJ>~} untag */
	uint16_t restore_time;  /* ~{;7B7;V845D<l2bJ1<d~}, ~{D,HO~} 10 ~{7VVS~} */
	uint8_t  interval;      /* ~{1(ND7"KM<d8t#,D,HO~} 1s */
	uint8_t  loop_flag;     /* 1: ~{SP;7B7#,~} 0: ~{C;;7B7~} */
	uint16_t loop_times;	/*~{;7B74NJ}~}*/
	uint32_t pkt_recv;      /* ~{=SJU5D1(NDJ}~} */
	uint32_t pkt_send;      /* ~{7"KM5D1(NDJ}~} */
    enum PORT_ACTION act;   /* ~{<l2b5=;7B7:sV4PP5D6/Ww~} */
	enum PORT_STATUS status;/* ~{6K?Z5DW*7"W4L,~} */
	//struct thread *ptimer1;	/* ~{7"KM1(ND5D~} timer */
	//struct thread *ptimer2; /* ~{<l2b;7B7;V845D~} timer */

	TIMERID ptimer1;
	TIMERID ptimer2;
};

struct loopd_hdr
{
	uint16_t h_code;  /* ~{P-RiWS@`PM~}*/
	uint16_t h_ver;  /* ~{0f1>~}*/
	uint16_t h_length;  /* ~{1(ND3$6H~}*/
	uint16_t h_resv;  /* ~{1#AtWV6N~}*/
}__attribute__((packed));

struct loopd_tlv
{
    uint8_t devid_type;/*Device Id*/
    uint8_t devid_len;
    uint8_t	devid_val[ETH_ALEN];
    uint8_t if_type;/*Ifindex*/
    uint8_t if_len;
    uint32_t if_val;
    uint8_t pname_type;/*Port Name*/
    uint8_t pname_len;
    sint8 pname_val[IFNET_NAMESIZE];
}__attribute__((packed));

struct loopd_frame
{
	struct loopd_hdr ldhdr;
	struct loopd_tlv tlv;
}__attribute__((packed));


struct loopdetect_global
{
	uint32_t loopdetect_debug;
	uint8_t source[ETH_ALEN];/*~{Ih18GE~}MAC*/
};

/*~{SCSZMx9\EzA?2iQ/~}*/
struct loop_detect_snmp
{
	uint32_t ifindex;
	struct loop_detect loopdetect;
};

extern struct loopdetect_global g_loopdetect_data;

/* ~{6K?ZJU5=1(NDTrK5Cw3vOV;7B7~} */
int loopdetect_pkt_recv(struct pkt_buffer *pkt);


/* ~{6(FZTZ6K?Z7"KM<l2b1(ND~} */
int loopdetect_pkt_send(void * arg);


/* ~{6K?Z;7B7W4L,IhVC~} */
int loopdetect_status_set(struct l2if *pl2_if, enum LOOPD_FLAG loop_flag);


/* ~{=S?ZOB5D~} loopdetect ~{EdVC1#4f~} */
void l2if_loopdetect_config_write(struct vty *vty, struct loop_detect *plpd);

void loopdetect_init(void);

void loopdetect_cli_init(void);

int loopdetect_snmp_port_info_get(uint32_t ifindex, struct  loop_detect_snmp loopdetect_snmp[]);

void loopdetect_clean_port(struct l2if *pif);

int loopdetect_restore_proc(void *arg);

void loopdetect_data_init(void);
void loopdetect_pkt_register(void);
void loopdetect_pkt_unregister(void);
void loopdetect_info_show(struct vty *vty, struct l2if *pl2_if);
#endif
