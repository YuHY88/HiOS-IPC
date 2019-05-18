/**
 * @file      : mpls.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月7日 17:14:25
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_MPLS_H
#define HIOS_MPLS_H

#include <pthread.h>
#include <lib/types.h>
#include <lib/inet_ip.h>
#include <lib/linklist.h>
#include <lib/devm_com.h>

extern void mpls_set_lsr_id(void);

#define MPLS_LSRID_CHECK(vty) \
    if(0 == gmpls.lsr_id) \
    { \
        mpls_set_lsr_id(); \
        if(0 == gmpls.lsr_id) \
        { \
            vty_out(vty, "Warning:Please config lsr-id at first!%s", VTY_NEWLINE); \
            return CMD_WARNING; \
        } \
    }

#define MPLS_LDP_GLOBAL_ENABEL_CHECK(vty) \
	    if(ENABLE != gmpls.ldp_enable) \
    { \
        vty_out(vty, "Warning: Please enable ldp at first !%s", VTY_NEWLINE); \
        return CMD_WARNING; \
    }

#define MPLS_BOARD_MASTER(v) \
        if(SLOT_TYPE_BOARD_MAIN != (v).board_status) \
        { \
            return ERRNO_SUCCESS; \
        }
        
/* mpls 的全局数据结构 */
struct mpls_global
{
    uint8_t          board_status;          //显示当前主控板卡的主备状态
    uint8_t          enable;                // 0:disable, 1:mpls enable
	uint8_t          ldp_enable;            // 0:disable, 1:ldp enable
	uint8_t          rsvp_enable;           // 0:disable, 1:rsvp-te enable
    uint8_t          pad;
    uint32_t         lsr_id;
	struct ipv6_addr lsrv6_id;              // ipv6 的 lsrid

    uint32_t         lsp_up_num;            // lsp up 状态数量统计
    uint32_t         static_lsp_up_num;     // 静态 lsp up 状态数量统计
    uint32_t         pw_up_num;             // pw up 状态数量统计
    uint32_t         vsi_pw_num;            // vpls 配置静态 MAC 数量统计

	uint32_t         pw_limit;              // 芯片注册的 pw 数量限制
	uint32_t         lsp_limit;             // 芯片注册的 lsp 数量限制
	uint32_t         tunnel_limit;          // 芯片注册的 tunnel 数量限制
    uint32_t         vsi_pw_mac_limit;      // vpls 配置静态 MAC 上限

};


extern struct mpls_global gmpls;


extern void mpls_global_cmd_init(void);

extern void mpls_service_table_init(unsigned int size);

#endif


