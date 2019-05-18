/**
 * @file      : pw_cmd.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年2月28日 9:40:58
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_PW_CMD_H
#define HIOS_PW_CMD_H

#include "pw.h"

#define PW_ALREADY_CONFIG_STATIC_TUNNEL(pl2vc)  (((pl2vc)->tunl_index!=0)&&((pl2vc)->pwinfo.nhp_type==NHP_TYPE_TUNNEL))
#define PW_ALREADY_CONFIG_LSP_TUNNEL(pl2vc)     (((pl2vc)->tunl_index!=0)&&((pl2vc)->pwinfo.nhp_type==NHP_TYPE_LSP))
#define PW_ALREADY_CONFIG_SLSP_TUNNEL(pl2vc)    ((pl2vc)->lsp_tunnel==ENABLE)
#define PW_ALREADY_CONFIG_LDP_TUNNEL(pl2vc)     ((pl2vc)->ldp_tunnel==ENABLE)
#define PW_ALREADY_CONFIG_GRE_TUNNEL(pl2vc)     ((pl2vc)->gre_tunnel==ENABLE)
#define PW_ALREADY_CONFIG_TP_TUNNEL(pl2vc)      ((pl2vc)->mplstp_tunnel==ENABLE)
#define PW_ALREADY_CONFIG_TE_TUNNEL(pl2vc)      ((pl2vc)->mplste_tunnel==ENABLE)
#define PW_ALREADY_BIND_AC(pwinfo)              ((pwinfo)->ifindex!=0)
#define PW_ALREADY_BIND_VSI(pwinfo)             ((pwinfo)->vpls.vsi_id!=0)
#define PW_ALREADY_BIND_MSPW(pwinfo)            ((pwinfo)->mspw_index!=0)
#define PW_ALREADY_BIND_MPLSTP_OAM(pl2vc)       ((pl2vc)->mplsoam_id!=0)
#define PW_ALREADY_BIND_BFD(pl2vc)              ((pl2vc)->bfd_id!=0)
#define PW_PROTO_IS_SVC(pwinfo)                 ((pwinfo)->protocol==PW_PROTO_SVC)
#define PW_PROTO_IS_MARTINI(pwinfo)             ((pwinfo)->protocol==PW_PROTO_MARTINI)

#define PW_ALREADY_BIND(vty, pwinfo)\
    if (PW_ALREADY_BIND_AC(pwinfo) || PW_ALREADY_BIND_VSI(pwinfo) || PW_ALREADY_BIND_MSPW(pwinfo))\
    {\
        vty_out((vty), "Error: PW has been binding and cannot be modified !%s", VTY_NEWLINE);\
        return CMD_WARNING;\
    }

#define PW_IS_INCOMPLETE(pl2vc)\
    (((pl2vc)->pwinfo.protocol == PW_PROTO_INVALID) \
    || ((pl2vc)->peerip.addr.ipv4 == 0) \
    || ((pl2vc)->pwinfo.vcid == 0) \
    || (((pl2vc)->tunl_index == 0) \
      && ((pl2vc)->ldp_tunnel == 0) \
      && ((pl2vc)->mplste_tunnel == 0) \
      && ((pl2vc)->mplstp_tunnel == 0) \
      && ((pl2vc)->gre_tunnel == 0)))

#define PW_PROTOCOL_NOT_CONFIG(vty, proto)\
    if ((proto) == PW_PROTO_INVALID)\
    {\
        vty_out(vty, "Error: Please configure the protocol type !%s", VTY_NEWLINE);\
        return CMD_WARNING;\
    }

#define PW_VC_TYPE_NOT_CONFIG(vty, type1)\
    if ((type1) == AC_TYPE_INVALID)\
    {\
        vty_out(vty, "Error: Please configure the vc type !%s", VTY_NEWLINE);\
        return CMD_WARNING;\
    }

#define PW_VC_TYPE_NOT_ETH(vty, type1)\
    if ((type1) != AC_TYPE_ETH)\
    {\
        vty_out(vty, "Error: Only the ethernet type allows configuration.%s", VTY_NEWLINE);\
        return CMD_WARNING;\
    }

#define PW_VC_TYPE_NOT_TDM(vty, type1)\
    if ((type1) != AC_TYPE_TDM)\
    {\
        vty_out(vty, "Error: Only the tdm type allows configuration.%s", VTY_NEWLINE);\
        return CMD_WARNING;\
    }

#define PW_LABEL_NOT_CONFIG(vty, l2vc)\
    if (((l2vc)->pwinfo.protocol == PW_PROTO_SVC) &&\
        (((l2vc)->inlabel == 0) || ((l2vc)->outlabel == 0)))\
    {\
        vty_out(vty, "Error: Please configure the PW label.%s", VTY_NEWLINE);\
        return CMD_WARNING;\
    }


extern int mpls_pw_pwinfo_download(struct pw_info *ppwinfo, enum OPCODE_E opcode);
extern int mpls_pw_status_download(struct pw_info *ppwinfo, enum PW_SUBTYPE subtype);
extern int mpls_pw_mplsif_download(uint32_t ifindex, void *pinfo, enum MPLSIF_INFO info,
                                         enum OPCODE_E opcode);

extern void mpls_pw_cmd_init(void);
extern int mpls_pw_config_show_this(struct vty *vty);

extern void  mpls_config_finish_func(void * a);


#endif

