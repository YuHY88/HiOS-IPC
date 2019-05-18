

#ifndef HIOS_MPLSTP_OAM_PKT_H
#define HIOS_MPLSTP_OAM_PKT_H

#include "mpls_oam/mpls_oam.h"
#include <ftm/pkt_mpls.h>

/* oam 报文类型 */
enum OAM_LB_PKT_E
{
	OAM_LB_REPLY= 2,
	OAM_LB_REQUEST = 3
};

/* lb mep type  */
enum OAM_LB_MEP_TYPE_E
{
	OAM_LB_MEP_TARGET = 33,
	OAM_LB_MEP_REPLY = 34
};

/*  报文类型 */
enum OAM_PKT_TYPE
{
	OAM_PKT_INVALID = 0,
	OAM_PKT_LB,	
	OAM_PKT_LT	
};

enum LB_TLV_TYPE
{
	LB_TLV_END = 0,
	LB_TLV_REQUEST,
	LB_TLV_DATA,
	LB_TLV_REQ_DATA
};

struct oam_mep_tlv
{
	uint8_t  type;
	uint16_t len;    //25bytes
	uint8_t  sub_type;
	uint16_t mep_id;
	uchar    resvd[22];
}__attribute__((packed));

struct oam_mip_tlv
{
	uint8_t  type;
	uint16_t len;    //25bytes
	uint8_t  sub_type;
	uchar    icc[6];
	uint32_t node_id;
	uint32_t if_num;
	uchar    resvd[10];
}__attribute__((packed));


/* 56 bytes */
struct oam_lb_requesting_mep_tlv
{
	uint8_t type;//35
	uint16_t len;//53
	uint8_t lk_indication;
	uint16_t mep_id;
	uint8_t resvd;// 1
	uint8_t meg_type;//32
	uint8_t meg_len;//13
	uchar megid[13];
	uchar unuse[32];
	uchar req_resvd[2];
}__attribute__((packed));

struct oam_lb_data_tlv
{
	uint8_t type;
	uint16_t len;
	uint8_t data[1];
}__attribute__((packed));


/* lbm 报文格式 */
struct oam_lbm_pkt
{
	#if BIG_ENDIAN_ON            /* CPU 大端序 */
		uint8_t  mel:3,
				 version:5;
	#else                           /* CPU 小端序 */
		uint8_t version:5,
				mel:3;
	#endif
    
     uint8_t  opcode;
     uint8_t  flags;
     uint8_t  tlv_offset;
     uint32_t seqNum;                        /* 序列号 */
	 union 
		 {
			 struct oam_mep_tlv mep;
			 struct oam_mip_tlv mip;
		 }tlv;
     u_int8_t end_tlv;
}__attribute__((packed));


/* lbm 报文格式 */
struct oam_lbm_data_pkt
{
	#if BIG_ENDIAN_ON            /* CPU 大端序 */
		uint8_t  mel:3,
				 version:5;
	#else                           /* CPU 小端序 */
		uint8_t version:5,
				mel:3;
	#endif
    
     uint8_t  opcode;
     uint8_t  flags;
     uint8_t  tlv_offset;
     uint32_t seqNum;                        /* 序列号 */
	 union 
		 {
			 struct oam_mep_tlv mep;
			 struct oam_mip_tlv mip;
		 }tlv;
	 struct oam_lb_data_tlv data_tlv;
     u_int8_t end_tlv;
}__attribute__((packed));


/* lbm 报文格式 */
struct oam_lbm_request_pkt
{
	#if BIG_ENDIAN_ON            /* CPU 大端序 */
		uint8_t  mel:3,
				 version:5;
	#else                           /* CPU 小端序 */
		uint8_t version:5,
				mel:3;
	#endif
    
     uint8_t  opcode;
     uint8_t  flags;
     uint8_t  tlv_offset;
     uint32_t seqNum;                        /* 序列号 */
	 union 
		 {
			 struct oam_mep_tlv mep;
			 struct oam_mip_tlv mip;
		 }tlv;
	 struct oam_lb_requesting_mep_tlv request_tlv;
     u_int8_t end_tlv;
}__attribute__((packed));


/* lbm 报文格式 */
struct oam_lbm_request_data_pkt
{
	#if BIG_ENDIAN_ON        /* CPU 大端序 */
	uint8_t mel:3,
			version:5;
	#else                    /* CPU 小端序 */
	uint8_t version:5,
			mel:3;
	#endif
    
    uint8_t opcode;
    uint8_t flags;
    uint8_t tlv_offset;
    uint32_t seqNum;         /* 序列号 */
	union 
	{
		struct oam_mep_tlv mep;
		struct oam_mip_tlv mip;
	}tlv;
	struct oam_lb_requesting_mep_tlv request_tlv;
	struct oam_lb_data_tlv data_tlv;
    u_int8_t end_tlv;
}__attribute__((packed));


struct oam_lb_pkt
{
	struct mpls_label label;
	uint32_t ctrl_word;
	uint8_t data[1];
}__attribute__((packed));


#if 0
/* lbm payload for lsp*/
struct oam_ltm_pkt_lsp
{
	struct mpls_label label;
	uint32_t ctrl_word;	
    struct oam_lbm_pkt lb_pkt;
}__attribute__((packed));


/* lbm payload for lsp*/
struct oam_ltm_data_pkt_lsp
{
	struct mpls_label label;
	uint32_t ctrl_word;	
    struct oam_lbm_data_pkt lb_pkt;
}__attribute__((packed));


/* lbm payload for lsp*/
struct oam_ltm_request_pkt_lsp
{
	struct mpls_label label;
	uint32_t ctrl_word;	
    struct oam_lbm_request_pkt lb_pkt;
}__attribute__((packed));


/* lbm payload for lsp*/
struct oam_ltm_request_data_pkt_lsp
{
	struct mpls_label label;
	uint32_t ctrl_word;	
    struct oam_lbm_request_data_pkt lb_pkt;
}__attribute__((packed));


/* lbm 报文格式 :含data tlv*/
struct oam_lbm_data_pkt
{
	#if BIG_ENDIAN_ON            /* CPU 大端序 */
		uint8_t  mel:3,
				 version:5;
	#else                           /* CPU 小端序 */
		uint8_t version:5,
				mel:3;
	#endif
    
     uint8_t  opcode;
     uint8_t  flags;
     uint8_t  tlv_offset;
     uint32_t seqNum;                        /* 序列号 */
	 union 
		 {
			 struct oam_mep_tlv mep;
			 struct oam_mip_tlv mip;
		 }tlv;
	 uint8_t  data[1];
}__attribute__((packed));


/* lbm payload for lsp*/
struct oam_ltm_pkt_lsp
{
	struct mpls_label label;
	uint32_t ctrl_word;	
    uint8_t data[1];
}__attribute__((packed));
#endif


void mplsoam_packet_register(void);
//void mplsoam_lb_set_pkt(struct oam_lbm_data_pkt *ppkt);
void mplsoam_lb_set_pkt(struct oam_session *psess, void *ppkt, enum LB_TLV_TYPE tlv_type);

int mplsoam_lb_timer_thread(void *arg);
int mplsoam_lt_timer_thread(void *arg);
//int mplsoam_lb_send_pkt(struct oam_session *psess, struct oam_lbm_data_pkt *preq);
int mplsoam_lb_send_pkt(struct oam_session *psess, void *preq, enum LB_TLV_TYPE tlv_type);
int mplsoam_pw_rcv_pkt(struct pkt_buffer *pkt);
int mplsoam_lsp_rcv_pkt(uint32_t lsp_index, struct pkt_buffer *pkt);
int mplsoam_lb_lt_pkt_set(struct oam_session * psess);

int mplsoam_send_lb(struct oam_session *psess);
int mplsoam_send_lt(struct oam_session *psess);

int mplsoam_recv_lb(struct pkt_buffer *pkt);



#endif

