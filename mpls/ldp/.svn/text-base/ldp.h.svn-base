/*
*   define of ldp 全局数据
*/

#ifndef HIOS_LDP_H
#define HIOS_LDP_H
#include <stdio.h>
#include <lib/types.h>
#include <time.h>
#include "../mpls_if.h"


void mpls_ldp_route_event_register(void);
void mpls_ldp_route_event_unregister(void);



#define MPLS_LDP_ERROR_MOD          -1

#define MPLS_LDP_DEBUG_TEST_ENABALE     0

#define MPLS_LDP_FEC_BROADCAST_MANAGEMENT           0

#if 1
#define MPLS_LDP_DEBUG(peerlsrid, level, format, ...)   do{\
                                                            if((0 == peerlsrid) || (0 == gldp.debug_peerlsrid) || (peerlsrid == gldp.debug_peerlsrid))\
                                                            {\
                                                                if(gldp.debug_ldpflag & level)\
                                                                {\
                                                                    FILE *debug_fp = fopen("/data/dat/ldp_log.txt", "a");\
                                                                    fprintf(debug_fp, ">%d %s %s "format"\n", __LINE__, __FILE__, __func__, ##__VA_ARGS__);\
                                                                    fclose(debug_fp);\
                                                                }\
                                                            }\
                                                         }while(0);

#else
#define MPLS_LDP_DEBUG(peerlsrid, level, format, ...)   do{\
                                                            if((0 == peerlsrid) || (0 == gldp.debug_peerlsrid) || (peerlsrid == gldp.debug_peerlsrid))\
                                                            {\
                                                                if(gldp.debug_ldpflag & level)\
                                                                {\
                                                                    zlog_debug(MPLS_DBG_LDP, "[LDP] %s[%d]: In function '%s'"format, __FILE__, __LINE__, __func__, ##__VA_ARGS__);\
                                                                }\
                                                            }\
                                                         }while(0);

#endif


#define MPLS_LDP_ERROR(format, ...)                   zlog_err("[LDP] %s[%d]: In function '%s'"format, __FILE__, __LINE__, __func__, ##__VA_ARGS__);

#define LDP_MPLS_LSRID          gmpls.lsr_id

#define LDP_SESSION_NUM   1024      /* ldp session hash size */

#define LDP_MARTINI_PW_RECORRECT_TIME       30

#define LDP_KEEP_ALIVE_TIME_DEFAULT         15
#define LDP_HELLO_INTERNAL_DEFAULT          5
#define LDP_NEIGH_EXPIRE_TIME       (gldp.hello_internal *3)
#define LDP_SESSION_EXPIRE_TIME     (gldp.keepalive_internal *3)

#define LDP_SESSION_BACKOFF_TIME_MAX    32
#define LDP_SESSION_BACKOFF_TIME_MIN    2


#define  LDP_PW_CONF_LOCAL      1
#define  LDP_PW_CONF_PEER       2


#define MPLS_LDP_SESS_CNT_INCREASE(v)   if(0 == (v).sess_cnt)\
                                        mpls_ldp_route_event_register();\
                                        (v).sess_cnt++;

#define MPLS_LDP_SESS_CNT_DECREASE(v)   (v).sess_cnt--;\
                                        if(0 == (v).sess_cnt)\
                                        mpls_ldp_route_event_unregister();\
                                        if(0 == gmpls.ldp_enable)\
                                        ldp_pkt_unregister();


#define MPLS_LDP_LABEL_CNT_SEND(v)      (v).label_send++;
#define MPLS_LDP_LABEL_CNT_RECV(v)      (v).label_recv++;
#define MPLS_LDP_PKT_SEND(v)            (v).pkt_send++;
#define MPLS_LDP_PKT_RECV(v)            (v).pkt_recv++;
#define MPLS_LDP_PKT_PW_SEND(v)         (v).pkt_pwsend++;
#define MPLS_LDP_PKT_PW_RECV(v)         (v).pkt_pwrecv++;
#define MPLS_LDP_PKT_KA_SEND(v)         (v)->kasnd_cnt++;
#define MPLS_LDP_PKT_KA_RECV(v)         (v)->karcv_cnt++;


#define LDP_SESS_CREAT_TIME_LEN     32

/* ldp debug procedures */
#define MPLS_LDP_DEBUG_DISCOVER     1U << 0
#define MPLS_LDP_DEBUG_FSM          1U << 1
#define MPLS_LDP_DEBUG_NOTIFY       1U << 2
#define MPLS_LDP_DEBUG_LABEL        1U << 3
#define MPLS_LDP_DEBUG_PW           1U << 4
#define MPLS_LDP_DEBUG_MAIN         1U << 5

/* ldp session role, active or passive */
enum LDP_ROLE
{
    LDP_ROLE_ACTIVE = 1,
    LDP_ROLE_PASSIVE
};

/* ldp session type, local or remote */
#define LDP_SESSION_LOCAL       1U << 0
#define LDP_SESSION_REMOT       1U << 1

/* ldp notify status code */
enum LDP_NOTIFYERRCODE
{
    LDP_NOTIF_SUCCESS = 0,
    LDP_NOTIF_BAD_LDP_ID,                               //parameter  error
    LDP_NOTIF_BAD_PROTO_VERSION,                        //parameter  error
    LDP_NOTIF_BAD_PDU_LEN,                          //parameter  error
    LDP_NOTIF_UNKNOWN_MESG,                         //parameter  error
    LDP_NOTIF_BAD_MESG_LEN,                         //parameter  error
    LDP_NOTIF_UNKNOWN_TVL,                          //parameter  error
    LDP_NOTIF_BAD_TLV_LEN,                          //parameter  error
    LDP_NOTIF_MALFORMED_TLV,                            //parameter  error
    LDP_NOTIF_HOLD_TIMER_EXPIRED,               //fatal event
    LDP_NOTIF_SHUTDOWN,                     //fatal event                                           10
    LDP_NOTIF_LOOP_DETECTED,
    LDP_NOTIF_UNKNOWN_FEC,
    LDP_NOTIF_NO_ROUTE,
    LDP_NOTIF_NO_LABEL_RESOURCES_AVAILABLE,
    LDP_NOTIF_LABEL_RESOURCES_AVAILABLE,
    LDP_NOTIF_SESSION_REJECTED_NO_HELLO,                        //session reject   16
    LDP_NOTIF_SESSION_REJECTED_PARAMETERS_ADVERTISEMENT_MODE, //session reject
    LDP_NOTIF_SESSION_REJECTED_PARAMETERS_MAX_PDU_LEN,      //session reject
    LDP_NOTIF_SESSION_REJECTED_PARAMETERS_LABEL_RANGE,      //session reject
    LDP_NOTIF_KEEPALIVE_TIMER_EXPIRED,      //fatal event                                               20
    LDP_NOTIF_LABEL_ABORT,
    LDP_NOTIF_MISSING_MSG_PARAMS,                       //parameter  error
    LDP_NOTIF_UNSUPORTED_ADDR_FAMILY,
    LDP_NOTIF_SESSION_REJECTED_BAD_KEEPALIVE_TIME,          //session reject
    LDP_NOTIF_INTERNAL_ERROR
};


/* mpls label policy */
enum LABEL_POLICY
{
    LABEL_POLICY_ALL = 1,
    LABEL_POLICY_HOST,
    LABEL_POLICY_NONE,
    LABEL_POLICY_PREFIX
};


/* ldp advertise mode */
enum LABEL_ADVERTISE
{
    LABEL_ADVERTISE_DOD = 1,
    LABEL_ADVERTISE_DU
};


/* ldp label control mode */
enum LABEL_CONTOL
{
    LABEL_CONTOL_ORDERED = 0,
    LABEL_CONTOL_INDEPENDENT
};

/* ldp label hold mode, liberal or conserve */
enum LABEL_HOLD
{
    LABEL_HOLD_LIBERAL = 0,
    LABEL_HOLD_CONSERVE
};


#define MPLS_LDP_BFD_ENABLE         1
#define MPLS_LDP_BFD_DISBALE        0

/* ldp global config */
struct ldp_global
{
    enum LABEL_POLICY     policy;   /**/
    enum LABEL_ADVERTISE  advertise;/**/
    enum LABEL_CONTOL     control;  /**/
    enum LABEL_HOLD       hold;     /**/
    uint16_t keepalive_internal;    /**/
    uint16_t hello_internal;        /**/

    uint8_t  proxy_egress;          /**/
    uint8_t  bfd_enable;            /* bfd for ldp MPLS_LDP_BFD_ENABLE */
    uint32_t debug_peerlsrid;       /* the peer lsrid to be debuged */
    uint8_t  debug_ldpflag;         /**/

    uint32_t label_send;            /**/
    uint32_t label_recv;            /**/
    uint32_t pkt_send;
    uint32_t pkt_recv;
    uint32_t pkt_pwsend;
    uint32_t pkt_pwrecv;
    uint32_t pkt_err;
    uint32_t sess_cnt;              /**/

    struct thread   *ppw_correct;
};


extern struct ldp_global gldp;

void ldp_init(void);
void ldp_cmd_init(void);
void mpls_ldp_fec_tree_show(void);

void ldp_pkt_register(void);
void ldp_pkt_unregister(void);
char *ldp_time_to_str(time_t sys_time);

int mpls_ldp_remot_enable(uint32_t peerlsrid);
int mpls_ldp_remot_disable(uint32_t peerlsrid);

char *ldp_ipv4_to_str(uint32_t addr);
char *ldp_ipv4_to_str2(uint32_t addr);

/* add for ldp mib */
int mpls_ldp_get_prefix(uint32_t prefix, uchar *prelen, int data_len, char *buf);
int mpls_ldp_get_global_conf(uint32_t index, int data_len, char *buf);
int mpls_ldp_get_localif(uint32_t index, int data_len, char *buf);
int mpls_get_remot_conf(uint32_t index, int data_len, char *buf);
int mpls_ldp_get_sess(uint32_t index, int data_len, char *buf);
int mpls_ldp_get_lsp(uint32_t lspindex, int data_len, char *buf);



#endif
