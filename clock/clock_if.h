/******************************************************************************
 * Filename: clock_if.h
 * Copyright (c) 2016 - 2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description: command functions for clock interface header file.
 *
 * History:
 * 2016.10.13  cuiyudong created
 *
******************************************************************************/


#ifndef HIOS_CLOCK_IF_H
#define HIOS_CLOCK_IF_H


#define CLOCKIF_NUM_MAX   2    /* 外时钟接口数量 */


/* clock 接口的模式 */
enum CLOCKIF_MODE
{
    CLOCKIF_MODE_INVALID = 0,
    CLOCKIF_MODE_2MBIT,        /* 2m bits 接口 */
    CLOCKIF_MODE_2MHZ,         /* 2m hz 接口 */
};


/* clock 接口的输入输出信号 */
enum CLOCKIF_SIGNAL
{
    CLOCKIF_SIGNAL_INVALID = 0,
    CLOCKIF_SIGNAL_IN,         /* 时钟信号输入 */
    CLOCKIF_SIGNAL_OUT,        /* 时钟信号输出 */
    CLOCKIF_SIGNAL_INOUT       /* 时钟信号输入和输出 */
};


/* 时钟类型 */
enum CLOCK_TYPE
{
    CLOCK_TYPE_INVALID = 0,
    CLOCK_TYPE_INTERNAL,       /* 系统内部时钟 */
    CLOCK_TYPE_GPS,            /* GPS 时钟 */
};


/* 时钟锁定状态 */
enum CLOCK_STAT
{
    CLOCK_STAT_INVALID = 0,
    CLOCK_STAT_LOSS,       /* 时钟丢失 */
    CLOCK_STAT_LOCK        /* 时钟锁定 */
};
/* 时钟有效状态 */
typedef enum VALID_STAT
{
    VALID_STAT_NULL = 0,
    VALID_STAT_INVALID, /* 时钟无效 */
    VALID_STAT_VALID,       /* 时钟有效 */
} E_VALID_STAT;


/* clock 接口下的配置信息类型 */
enum CLOCKIF_INFO
{
    CLOCKIF_INFO_INVALID = 16,
    CLOCKIF_INFO_MODE,         /* 接口模式 */
    CLOCKIF_INFO_QLEVEL,       /* 时钟 quality-level */
    CLOCKIF_INFO_PRIORITY,     /* 时钟的优先级 */
    CLOCKIF_INFO_SIGNAL,       /* 接口信号入出方向 */
    CLOCKIF_INFO_SA_SEL,       /* sa通道设置 */
    CLOCKIF_INFO_SDHMGT_BK_UPDATE, /* SyncE->SDHMGT, when SyncE bk clock data change  */
    CLOCKIF_INFO_SDHMGT_BK_GET,    /* SDHMGT->SyncE */

    CLOCKIF_INFO_MAX = 64
};


/* 时钟接口数据结构 */
struct clockif_t
{
    uint32_t            ifindex;        /* 接口索引 */
    uint8_t             id;             /* 时钟接口 id */
    uint8_t             ssm_level;      /* 时钟 quality-level */
    uint8_t             status;         /* 0: 接口 down，1: 接口 up */
    uint8_t             priority;       /* 时钟的优先级 */
    enum CLOCKIF_MODE   mode;           /* 接口模式 */
    enum CLOCKIF_SIGNAL signal;         /* 接口信号入出方向 */
    uint8_t             sa;
    uint8_t             ql_set_flag;    /*2MBits模式有效.
                                          手动配置时钟质量等级标志  1 : 手动设置，该情况下外时钟质量等级用手动配置
                                                                    0 : 未配置 ，该情况下从FPGA读出收到的质量等级*/
};

/* SyncE trib clock, for H9MOLMXE_VX, message from MX to TX */
struct syncE_sdhmgt
{
    uint8_t             trib_index; // [1..4], max 4
    uint8_t             trib_use;   // 0:no use，1: be used
    uint32_t            ifindex;    // interface index
};


/* 时钟接口表 */
extern struct clockif_t *pclkif[CLOCKIF_NUM_MAX]; /* 用接口 id 标志*/


/* l2if table 操作函数 */
void clockif_init(void);
struct clockif_t *clockif_create(uint32_t ifindex, uint8_t id); /* 新建一个时钟接口 */
int clockif_add(struct clockif_t *pif);         /* 添加到接口表 */
int clockif_delete(uint8_t id);                /* 从接口表删除 */
struct clockif_t *clockif_lookup(uint8_t id);  /* 查找一个时钟接口 */

int clockif_set_info(struct clockif_t *pclk, void *data, enum CLOCKIF_INFO info_type);
unsigned int synce_snmp_clk_if_next_ifindex_get(struct ipc_msghdr_n *phdr);
unsigned int synce_snmp_clk_if_info_get(struct ipc_msghdr_n *phdr);



#endif
