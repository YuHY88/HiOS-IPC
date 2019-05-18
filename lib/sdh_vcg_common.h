
#ifndef _SDH_VCG_COMMON_H_
#define _SDH_VCG_COMMON_H_

#include "sdh_public_common.h"

#define SDH_VCG_SUPPORT_LCAS            (1L)
#define SDH_VCG_SUPPORT_SCRAMBLING      (1L << 1)
#define SDH_VCG_SUPPORT_FCS             (1L << 2)
#define SDH_VCG_SUPPORT_LFP             (1L << 3)
#define SDH_VCG_SUPPORT_LEVEL_VC4       (1L << 4)
#define SDH_VCG_SUPPORT_LEVEL_VC3       (1L << 5)
#define SDH_VCG_SUPPORT_LEVEL_VC12      (1L << 6)
#define SDH_VCG_SUPPORT_VLAN            (1L << 7)

typedef enum
{
    SDH_VCG_LEVEL_VC4 = 0,
    SDH_VCG_LEVEL_VC3,
    SDH_VCG_LEVEL_VC12
}   E_SDH_VCG_LEVEL;

typedef enum
{
    SDH_VCG_ENCAP_TYPE_EOS = 0,     /*default*/
    SDH_VCG_ENCAP_TYPE_EOE,
    SDH_VCG_ENCAP_TYPE_EOP
}   E_SDH_VCG_ENCAP_TYPE;

typedef enum
{
    SDH_VCG_ENCAP_PROTOCOL_GFPF = 0, /*default*/
    SDH_VCG_ENCAP_PROTOCOL_HDLC,
}   E_SDH_VCG_ENCAP_PROTOCOL;

typedef enum
{
    SDH_VCG_SCRAMBLING_NONE = 0,
    SDH_VCG_SCRAMBLING_UP,
    SDH_VCG_SCRAMBLING_DOWN,
    SDH_VCG_SCRAMBLING_BOTH
}   E_SDH_VCG_SCRAMBLING;

typedef enum
{
    SDH_LCAS_STAT_FIXED = 0,
    SDH_LCAS_STAT_ADD,
    SDH_LCAS_STAT_NORMAL,
    SDH_LCAS_STAT_EOS,
    SDH_LCAS_STAT_IDLE = 5,
    SDH_LCAS_STAT_DNU = 15
}   E_SDH_LCAS_STATUS;

typedef enum
{
    VCG_INFO_INVALID = 0,
    VCG_INFO_BINDING,
    VCG_INFO_BINDING_LEVEL,
    VCG_INFO_JX_TRANSMIT,
    VCG_INFO_JX_EXPECT,
    VCG_INFO_SCRAMBLING,
    VCG_INFO_FCS,
    VCG_INFO_LCAS_ENA,
    VCG_INFO_LCAS_RESET,
    VCG_INFO_IF_ADD,
    VCG_INFO_IF_DELETE,
    VCG_INFO_B3,
    VCG_INFO_BIP,
    VCG_INFO_INDEX,
    VCG_GET_CONFIG,
    VCG_GET_VCINFO,
    VCG_GET_STATISTICS,
    VCG_INFO_VLAN,
    VCG_STATUS_IF_UPDATE,
    VCG_VTYSH_RECONFIG,
    VCG_INFO_SWITCH_IMAGE,
    VCG_GET_FPGA_IMAGE
}   E_SDH_VCG_IPC_MSG_SUBTYPE;

typedef struct
{
    int                         vc4;            /*start vc4*/
    int                         vc3_vc12;       /*start vc3 or vc12*/
    int                         num;            /*binding num*/
}   t_sdh_vcg_binding;

/*vc12 lcas status*/
typedef struct
{
    E_SDH_LCAS_STATUS           status;
    int                         sq;
}   t_sdh_vcg_lcas_status;

/*vcg port total statistics*/
typedef struct
{
    unsigned long long          rx_packet;
    unsigned long long          tx_packet;
    unsigned long long          gfp_los;
}   t_sdh_vcg_statistics;

/*vc4/vc3 poh*/
typedef struct
{
    uint8_t                     c2;
    uint8_t                     g1;
    uint8_t                     f2;
    uint8_t                     k3;
    uint8_t                     f3;
    uint8_t                     n1;
    uint8_t                     resv[2];
}   t_sdh_vcg_oh_hp;

typedef struct
{
    unsigned int                support;        /*SDH_VCG_SUPPORT_###*/

    E_SDH_VCG_ENCAP_TYPE        type;           /*vc4/vc3/vc12*/
    E_SDH_VCG_ENCAP_PROTOCOL    protocol;

    unsigned char               tra_jx[SDH_OH_JX_LEN];
    unsigned char               exp_jx[SDH_OH_JX_LEN];

    E_SDH_VCG_LEVEL             level;          /*vc4/vc3/vc12*/
    t_sdh_vcg_binding           binding_up;
    t_sdh_vcg_binding           binding_down;

    E_SDH_VCG_SCRAMBLING        scrambling;
    int                         fcs_insert;
    int                         lcas_reset;
    int                         lcas_enable;

    uint16_t                    vid;
    uint16_t                    tpid;
    uint8_t 	                cos;
    uint8_t                     upvlan_ena;     /*1:enable up direction remove vlan */
    uint8_t                     downvlan_ena;   /*1:enable down direction add vlan*/
    uint8_t                     resv;
}   t_sdh_vcgif_config;

typedef struct
{
    uint32_t                    ifindex;

    int                         link_status;    /*0:up, 1:down*/
    t_sdh_vcgif_config          config;
	uint32_t                    image_level;
}   t_sdh_vcg_interface;

typedef struct  // VCG_GET_CONFIG
{
    // 0:from >= msg.key return, include msg.key
    // 1:ignore msg.key, from 1st return
    // 2:from == msg.key return, include msg.key
    uint32_t                    req_flag;   

    t_sdh_vcg_interface         msg;
}   t_sdh_vcg_interface_getbulk_req_msg;

typedef struct
{
    E_SDH_VCG_LEVEL             level;

    unsigned char               rcv_jx[SDH_OH_JX_LEN];

    unsigned long long          biporb3_increment;
    unsigned long long          biporb3_total;
    uint32_t                    biporb3_reg;

    /*vc12, lp_unq 0x00000001*/
    /*vc12, tu_ais 0x00000002*/
    /*vc12, tu_lop 0x00000004*/
    /*vc12, lp_ais 0x00000008*/
    /*vc12, lok4   0x00000010*/
    /*vc12, tim    0x00000020*/
    uint32_t                    alarm;

    t_sdh_vcg_lcas_status       lcas_status;

    t_sdh_vcg_oh_hp             oh_hp_rx;
}   t_sdh_vcg_unit_info;

typedef struct
{
    uint32_t                    ifindex;    // key

    t_sdh_vcg_statistics        cnt_increment;
    t_sdh_vcg_statistics        cnt_total;
}   t_sdh_vcg_count;

typedef struct  // VCG_GET_STATISTICS
{
    // 0:from >= msg.key return, include msg.key
    // 1:ignore msg.key, from 1st return
    // 2:from == msg.key return, include msg.key
    uint32_t                    req_flag;

    t_sdh_vcg_count             msg;
}   t_sdh_vcg_count_getbulk_req_msg;

typedef struct
{
    uint32_t                    ifindex;    // key
    int                         vc4;        // key
    int                         vc3_vc12;   // key

    t_sdh_vcg_unit_info         info;
}   t_sdh_vcg_info;

typedef struct  // VCG_GET_VCINFO
{
    // 0:from >= msg.key return, include msg.key
    // 1:ignore msg.key, from 1st return
    // 2:from == msg.key return, include msg.key
    uint32_t                    req_flag;

    t_sdh_vcg_info              msg;
}   t_sdh_vcg_info_getbulk_req_msg;

typedef struct
{
    uint32_t                    ifindex;    // key

    int                         link_status;
}   t_sdh_vcgif_status_msg;


#endif
