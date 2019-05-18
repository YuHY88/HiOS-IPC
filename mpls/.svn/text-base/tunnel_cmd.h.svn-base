/**
 * @file      : tunnel_cmd.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月7日 10:11:29
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_TUNNEL_CMD_H
#define HIOS_TUNNEL_CMD_H


/* 暂时不支持 tunnel 下配置协议类型为 mplste，配置所有参数都不处理 */
#define TUNNEL_PROTOCOL_IS_MPLSTE(protocol) \
    if (TUNNEL_PRO_MPLSTE == (protocol))\
    {\
        return CMD_SUCCESS;\
    }

/* tunnel 不是双向的 */
#define TUNNEL_IS_NOT_BIDIRICTIONAL(pif) \
    ((NULL != pif->p_mplstp) && \
    ((NULL == pif->p_mplstp->ingress_lsp) || (NULL == pif->p_mplstp->egress_lsp)) && \
    ((NULL == pif->p_mplstp->backup_ingress_lsp) || (NULL == pif->p_mplstp->backup_egress_lsp)))


extern void tunnel_if_cmd_init(void);
extern int tunnel_if_tunnel_t_download(struct tunnel_t *ptunnel, enum TUNNEL_INFO subtype,
                                            enum OPCODE_E opcode);


#endif


