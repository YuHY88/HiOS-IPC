
#ifndef HIOS_PKT_MPLS_H
#define HIOS_PKT_MPLS_H

#include <lib/types.h>
#include <lib/pkt_buffer.h>
#include <lib/route_com.h>


#define MPLS_LABEL_LEN   4        /* MPLS һ�� label �ĳ��� */
#define MPLS_ARCH_LEN    4        /* MPLS arch �ֶεĳ��� */
#define MPLS_LABEL_NUM   3        /* ���ĵ�����ǩ�� */


/* mpls �����ֵĶ��� */
#define MPLS_CTLWORD_FLAG           0x1     /* mpls control word flag */
#define MPLS_CTLWORD_WITHOUT_IP		0x0007
#define MPLS_CTLWORD_CHTYPE_IPV4    0x0021  /* ipv4 channel type */
#define MPLS_CTLWORD_CHTYPE_IPV6    0x0057  /* ipv6 channel type */
#define MPLS_CTLWORD_CHTYPE_APS     0x8908  /* mpls aps */
#define MPLS_CTLWORD_CHTYPE_8902    0x8902  /* mpls-tp oam */
#define MPLS_CTLWORD_CHTYPE_7FFA    0x7ffa  /* mpls-tp oam */


/* mpls label header.  */ 
struct mpls_label
{
	uint16_t label1;	 /* ��ǩ�ĸ� 16bit */
	
#if BIG_ENDIAN_ON        /* CPU ����� */
	uint8_t  label0:4,	 /* ��ǩ�ĵ� 4bit */
			 exp:3,
			 bos:1;  	 /* ջ�ױ�־ */
#else                    /* CPU С���� */
	uint8_t  bos:1, 	 /* ջ�ױ�־ */
			 exp:3,
			 label0:4;	 /* ��ǩ�ĵ� 4bit */
#endif

	uint8_t  ttl;
};


/*	mpls arch header. */ 
struct mpls_arch 
{
#if BIG_ENDIAN_ON           /* CPU ����� */
	uint8_t    flag:4,
			   version:4; 
#else                       /* CPU С���� */
	uint8_t    version:4,
			   flag:4; 
#endif
    uint8_t	   reserve;
    uint16_t   chtype;      /* channel type */
};

int mpls_rcv_lsp(struct pkt_buffer *pkt);
int mpls_rcv_pw(struct pkt_buffer *pkt);
int mpls_rcv_local(struct pkt_buffer *pkt);
int mpls_rcv(struct pkt_buffer *pkt);
int mpls_output(struct pkt_buffer *pkt);
int mpls_xmit(struct pkt_buffer *pkt);
int mpls_encap(struct pkt_buffer *pkt);
int mpls_decap(struct pkt_buffer *pkt);
int mpls_forward(struct pkt_buffer *pkt);
int mpls_forward_ecmp(struct pkt_buffer *pkt, uint32_t nhp_index, enum NHP_TYPE nhp_type);


#endif


