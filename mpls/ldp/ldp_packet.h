/*
*   ldp 收发包和状态机处理
*/
#ifndef HIOS_LDP_PACKET_H
#define HIOS_LDP_PACKET_H

#include <lib/types.h>
#include <ftm/pkt_udp.h>
#include "ldp_session.h"
#define LDP_PKT_TYPE_UDP    1
#define LDP_PKT_TYPE_TCP    2


#define LDP_HELLO_ADDR  0xe0000002U         /* Link LDP hello packet destination ip address 224.0.0.2 */
#define LDP_VERSION     0x0001              /* ldp version */


/* ldp header len */
#define LDP_PDU_HEADER_LEN      10
#define LDP_HEADER_LEN          4   /* length of ldp version and pdu len in the ldp pdu header */
#define LDP_IDENTIFY_LEN        6   /* length of ldp identifier in the ldp pdu header */
#define LDP_BASEMSG_LEN         4   /* length of ldp msg type and msg length in the base msg */
#define LDP_MSG_HEAD_LEN        8  /* length of the msg header */
#define LDP_MSGID_LEN           4   /* length of ldp msg id */
#define LDP_BASETLV_LEN         4   /* length of tlv type and tlv len in the tlv base */
#define LDP_MAX_PDU_LEN         4096


/* ldp tlv vlaue len */
#define LDP_TLV_HELLO_VAL_LEN   4   /* Length of Common Hello Param TLV value field */
#define LDP_TLV_TRADDR_VAL_LEN  4   /* transport address for the tcp link in the hello paceket */
#define LDP_TLV_INIT_VAL_LEN    14
#define LDP_TLV_NOTY_VAL_LEN    10
#define LDP_TLV_FEC_VAL_LEN     8
#define LDP_TLV_PWFEC_VAL_LEN   16
#define LDP_TLV_LABEL_VAL_LEN   4
#define LDP_TLV_ADDR_MAP_LEN    6

/* ldp TLV type */
#define LDP_TLV_FEC           0x0100 /* fec tlv                         */
#define LDP_TLV_PW_FEC        0x0080 /* pw fec                                */
#define LDP_TLV_VC            0x0080 /* VC  type                               */
#define LDP_TLV_ADDR          0x0101 /* addr list tlv                   */
#define LDP_TLV_LABEL         0x0200 /* gernel label tlv            */
#define LDP_TLV_HELLO         0x0400 /* Common Hello Param Tlv      */
#define LDP_TLV_TRADDR        0x0401 /* Hello transport address */
#define LDP_TLV_INIT          0x0500 /* common params for init msg  */
#define LDP_TLV_STATUS        0x0300 /* status TLV */
#define LDP_TLV_REQMSGID      0x0600 /* label request message ID tlv*/

/* EXTEND LDP TLV FOR PW from RFC4447 */
#define PW_STATUS_TLV        0x096A
#define PW_INTF_P_TLV        0x096B
#define PW_GROUPID_TLV       0x096C
/* PW VC_TYPE */
#define PW_VC_TYPE_TDM      0x0011
#define PW_VC_TYPE_VLAN     0x0004
#define PW_VC_TYPE_ETH      0x0005


/* PW ERROR CODE FROM RFC 4447 */
#define ILLEGAL_C_BIT      0x00000024
#define WRONG_C_BIT        0x00000025
#define INCOMPATIBLE_C_BIT 0x00000026
#define CEP_TDM_MIS_CONF   0x00000027
#define PW_STATUS          0x00000028
#define UNKNOWN_TAI        0x00000029
#define MIS_CONF_ERROR     0x0000002A
#define PW_WITHDRAW_STAT   0x0000002B


/* ldp message type */
#define LDP_MSG_INIT                0x0200 /* initialization msg          */
#define LDP_MSG_ADDR                0x0300 /* address msg                 */
#define LDP_MSG_ADDRWITH            0x0301 /* address withdraw msg        */
#define LDP_MSG_KEEPALIVE           0x0201 /* keep alive msg              */
#define LDP_MSG_HELLO               0x0100 /* hello msg                   */
#define LDP_MSG_NOTIFY              0x0001 /* notification msg            */
#define LDP_MSG_LABEL_MAPPING       0x0400 /* label mapping msg           */
#define LDP_MSG_LABEL_REQUSET       0x0401 /* label request msg           */
#define LDP_MSG_LABEL_WITHRAW       0x0402  /* label withdraw msg          */
#define LDP_MSG_LABEL_RELEASE       0x0403 /* label release msg           */
#define LDP_MSG_LABEL_ABORT         0x0404 /* label abort request msg     */


/* ldp label space, global or interface */
enum LDP_LABEL_SPACE
{
    LDP_LABEL_GLOBAL = 0,  
    LDP_LABEL_INTERFACE,   
};


/* ldp pdu header int all ldp packets */
struct ldpmsg_pdu
{
    uint16_t version;              /* LDP VERSION  1 */
    uint16_t pdu_len;              /* length excluding the version and length */
    uint32_t lsrid;                /* mpls lsr_id */
    uint16_t label_space;          /* label space */
} __attribute__((packed));


/* ldp msg base header */
struct ldpmsg_base
{
#if BIG_ENDIAN_ON               /* big edian */
    uint16_t ubit: 1,           /* ubit :unknown message bit */
             msg_type: 15;      /* type of message */
#else
    uint16_t msg_type: 15,
             ubit: 1;
#endif
    uint16_t msg_len;            /* msgId + mandatory param + optional param */
    uint32_t msgid;              /* message id :increased by every message */
};


/*ldp tlv base header */
struct ldptlv_base
{
#if BIG_ENDIAN_ON                /* cpu big endian */
    uint16_t flag: 2,            /* u bit for unknown tlv and f bit for forward unknown tlv */
             tlv_type: 14;       /* tlv type */
#else
    uint16_t tlv_type: 14,
             flag: 2;
#endif

    uint16_t tlv_len;            /* length of value field in octets */
};


/* Common ldp hello params */
struct hellotlv_chp
{
    struct ldptlv_base basetlv;
    uint16_t neigh_hold;
    uint16_t flag;               /* 15bit: T, 14bit: R, 0-13bit: reserved */
};


/* LDP transport address tlv */
struct hellotlv_traddr
{
    struct ldptlv_base basetlv;
    uint32_t addr;
};


/* ldp hello message */
struct ldpmsg_hello
{
    struct ldpmsg_base      basemsg;
    struct hellotlv_chp     chp;          /* common hello parameters */
    struct hellotlv_traddr  traddr;
                                          /* optional ldp config sequence num tlv */
                                          /* optional ldp ipv6 transpot addr tlv */
};

/* common session init params TLV */
struct initlv_chp
{
    struct ldptlv_base basetlv;
    uint16_t           version;
    uint16_t           session_hold;       /* proposed keep alive interval */
    uint16_t           flag;               /* A bit :label distribute discipline D bit: loop detecte function 
                                               3-7: reserved 8-15: path vector limit */
    uint16_t           pdu_len;
    uint32_t           peer_lsrid;
    uint16_t           peer_labelspace;    /* Peer ldp label space */
} __attribute__((packed));

/* LDP init message */
struct ldpmsg_init
{
    struct ldpmsg_base  basemsg;
    struct initlv_chp   chp;
                                            /* atm session param tlv */
                                            /* fram delay session param tlv */
                                            /* graceful restart param tlv */
} __attribute__((packed));

/* LDP keepalive message */
struct ldpmsg_keepalive
{
    struct ldpmsg_base  basemsg;
};

/* */
struct notifytlv_status
{
    struct ldptlv_base basetlv;
    uint32_t           flag;
    uint32_t           msgid;
    uint16_t           msgtype;
} __attribute__((packed));

/* */
struct notifytlv_pwstatus
{
    uint16_t           tlv_type;
    uint16_t           tlv_len;
    uint32_t           pwstatus;
};


#define LDP_PW_PARA_ID          0x01
#define LDP_PW_MTU_LEN          4

struct pw_ifmtu
{
    uint8_t parmater_id;/* mtu 0x01*/
    uint8_t length;     /* 4 */
    uint16_t mtumax;    /* max mtu */
};

#define LDP_MPLS_VCCV_ID            0x0c
#define LDP_MPLS_VCCV_LEN           0x04

#define LDP_MPLS_VCCV_CONTROL_WORD_ENABLE   0x01
#define LDP_MPLS_VCCV_ROUTE_ALERT_ENABLE    0x02
#define LDP_MPLS_VCCV_BFD_ENABLE            0x04
#define LDP_MPLS_VCCV_LSP_PING_ENABLE       0x02
#define LDP_MPLS_VCCV_ICMP_PING_ENABLE      0x01

struct ldptlv_vccv
{
    uint8_t id;                    /* 0x0c     */
    uint8_t length;                /* 0x100    */
    uint8_t cctype;                /* reserved:inner label ttl:router alert: pwe3 control word */
    uint8_t cvtype;                /* reserved:bfd flag:lsp ping flag:icmp ping flag*/
}__attribute__((packed));

struct fec_pwelement
{
    uint8_t type;
    union
    {
        struct pw_bit
        {
#if BIG_ENDIAN_ON
            uint16_t cbit: 1,
                     vctype: 15;
#else
            uint16_t vctype: 15,
                     cbit: 1;
#endif
        } pwbit;
        uint16_t u;
    } bitmap;

    uint8_t length;
    uint32_t groupid;
    uint32_t vcid;
    struct pw_ifmtu mtu;
    struct ldptlv_vccv  vccv;           /* VCCV  TLV*/
} __attribute__((packed));

struct fec_element
{
    uint32_t type: 8,
             family: 16,
             prelen: 8;
    union
    {
        //uint16_t ipv6[8];
        uint32_t ipv4;
    } addr;
};

struct ldptlv_fec
{
    struct ldptlv_base basetlv;
    union
    {
        uint8_t              type;
        struct fec_element   fec;
        struct fec_pwelement pwfec;
    } element;
};

/* LDP notification message */
struct ldpmsg_notify
{
    struct ldpmsg_base          basemsg;
    struct notifytlv_status     status;
    struct notifytlv_pwstatus   pwstatus;
    struct ldptlv_fec           fec;
} __attribute__((packed));


/* LDP address maping message */
struct ldpmsg_addrmap
{
    struct ldpmsg_base basemsg;
    struct ldptlv_base basetlv;
    uint16_t           familiy;      /* ipv4 or ipv6 */
    uint32_t ipv4;
} __attribute__((packed));

/* LDP address withdraw message */
struct ldpmsg_addrwithdraw
{
    struct ldpmsg_base basemsg;
    struct ldptlv_base basetlv;
    uint16_t           familiy;      /* ipv4 or ipv6 */
    uint32_t ipv4;
} __attribute__((packed));


/* Gerneral LDP label tlv */
struct ldptlv_label
{
    struct ldptlv_base basetlv;

    uint8_t  res;
#if BIG_ENDIAN_ON
    uint8_t  res2: 4,
             label2: 4;
#else
    uint8_t  label2: 4,
             res2: 4;
#endif
    uint16_t label;               /*label 20bit*/
};

/* atm label tlv */
struct ldptlv_atmlabel
{

};


/* frame relay label tlv */
struct ldptlv_frlabel
{

};

/* hop count tlv */
struct ldptlv_hopcount
{
    uint16_t hop_count;
};

/* path vector tlv */
struct ldptlv_pathvector
{
    uint32_t lsrid;
};


/* LDP label maping message */
struct ldpmsg_map
{
    struct ldpmsg_base  basemsg;
    struct ldptlv_fec   fec;
    struct ldptlv_label label;
                                        /* interface param tlv */
                                        /* pw status tlv */
                                        /* label request id tlv */
                                        /* hop count tlv */
                                        /* path verctor tlv */
};


/* LDP label request message*/
struct ldpmsg_req
{
    struct ldpmsg_base basemsg;
    struct ldptlv_fec  fec;
    //struct ldptlv_hopcount hop_count; 
    //struct ldptlv_pathvector path_vector;
};


/* LDP label request message ID tlv */
struct ldptlv_labelreqid
{
    struct ldptlv_base basetlv;
    uint32_t           msgid;
};


/* LDP label requeser abort message */
struct ldpmsg_reqabort
{
    struct ldpmsg_base       basemsg;
    struct ldptlv_fec        fec;
    struct ldptlv_labelreqid labelreqid;
};


/* LDP label withdraw message */
struct ldpmsg_withdraw
{
    struct ldpmsg_base  basemsg;
    struct ldptlv_fec   fec;
    struct ldptlv_label label;   //optional
};


/* LDP label release message */
struct ldpmsg_release
{
    struct ldpmsg_base  basemsg;
    struct ldptlv_fec   fec;
    struct ldptlv_label label; //optional
};


/* ldp message union */
struct ldp_pkt
{
    struct ldpmsg_pdu               header;
    union
    {
        struct ldpmsg_hello         hello;
        struct ldpmsg_init          init;
        struct ldpmsg_keepalive     keepalive;
        struct ldpmsg_notify        notify;
        struct ldpmsg_addrmap       addr;
        struct ldpmsg_addrwithdraw  addrelease;
        struct ldpmsg_map           map;
        struct ldpmsg_req           request;
        struct ldpmsg_withdraw      withdraw;
        struct ldpmsg_release       release;
        struct ldpmsg_reqabort      reqabort;
    } msg;
} __attribute__((packed));

int  ldp_send_udp_pkt(uint32_t local_ip, uint32_t peer_ip, uint32_t ifindex, void *data, int data_len);
int  ldp_send_tcp_pkt(struct ldp_sess *psess, void *data, int data_len);
int ldp_recv_udp_pkt(struct pkt_buffer *ppkt);
int ldp_recv_tcp_pkt(struct pkt_buffer *ppkt);
int ldp_recv_pkt(struct pkt_buffer *ppkt, uint32_t pkt_type);

void ldp_set_msgbase(struct ldpmsg_base *pbasemsg, uint32_t pktype, uint32_t len);
void ldp_set_header(struct ldpmsg_pdu *pheader, uint32_t local_lsrid, uint32_t len);
void ldp_set_tlvbase(struct ldptlv_base *pbasetlv, uint32_t tlvtype, uint32_t len);

int ldp_send_hello(uint32_t local_ip, uint32_t peer_ip, uint32_t ifindex, uint32_t ldptype);
int ldp_send_init(struct ldp_sess *psess);
int ldp_send_keepalive(struct ldp_sess *psess);

int ldp_recv_hello(struct ldpmsg_base  *pbasemsg, uint32_t peer_lsrid, struct ip_control  *pipcb);
int ldp_recv_init(struct ldpmsg_base  *pbasemsg, struct ldp_sess *psess, struct ip_control  *pipcb);
int ldp_recv_keepalive(struct ldpmsg_base  *pbasemsg, struct ldp_sess *psess, struct ip_control  *pipcb);

int ldp_recv_request(struct ldpmsg_base  *pbasemsg, struct ldp_sess *psess, struct ip_control  *pipcb);
int ldp_recv_req_abort(struct ldpmsg_base  *pbasemsg, struct ldp_sess *psess, struct ip_control  *pipcb);
int ldp_recv_mapping(struct ldpmsg_base  *pbasemsg, struct ldp_sess *psess, struct ip_control  *pipcb);
int ldp_recv_withdraw(struct ldpmsg_base  *pbasemsg, struct ldp_sess *psess, struct ip_control  *pipcb);
int ldp_recv_release(struct ldpmsg_base  *pbasemsg, struct ldp_sess *psess, struct ip_control  *pipcb);


#endif

