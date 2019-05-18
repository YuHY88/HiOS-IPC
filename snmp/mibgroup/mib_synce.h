/*
 *  alarm snmp browser interface - alarm_snmp.h
 *
 */
#ifndef _MIB_SYNCE_H
#define _MIB_SYNCE_H

#define synceCfgMode            1
#define synceCfgSSMEnable       2
#define synceCfgSSMInterval     3
#define synceCfgPreemtEnable    4
#define synceCfgFailback        5
#define synceCfgHoldTime        6
#define synceCfgClockSource     7
#define SyncECurrentClockState  8
#define SyncECurrentFreqOffset  9


#define synceClkIfIndex         1
#define synceClkIfDescr         2
#define synceClkIfMode          3
#define synceClkIfSignal        4
#define synceClkIfQL            5
#define synceClkIfSA            6


#define synceClkSrcIndex        1
#define synceClkSrcDescr        2
#define synceClkSrcPriority     3
#define synceClkSrcValidState   4
#define synceClkSrcClockState   5
#define synceClkSrcQL           6


struct synce_snmp_t
{
    int                 mode;
    int                 ssm_mode;
    int                 cur_clk_stat;
    uint32_t            clk_select;
    uint32_t            clk_manul;
    uint32_t            clk_force;
    uint32_t            hold_time;
    uint16_t            wtr;
    uint16_t            ssm_interval;
    uint16_t            ssm_timeout;
    uint8_t             cur_clk_ssm_ql;
    uint8_t             uc_send_ql;
    uint8_t             preempt;
    uint8_t             ssm_enable;
    int                 iSendFlag;
    int                 iStatScanTime;
    int                 iFlgClrTimer;
    int                 iHoldOverTimer;
    int                 iWaitTimer;
    int                 iFreqOffset;            /* DPLL frequence offset set */
    int                 iFreeClrTimer;
    void               *clk_list;
};

struct synce_snmp_clock_src_t
{
    int         status;
    int         valid_stat;
    uint32_t    ifindex;
    uint8_t     priority;
    uint8_t     quality_level;
};

enum CLOCKIF_MODE
{
    CLOCKIF_MODE_INVALID = 0,
    CLOCKIF_MODE_2MBIT,
    CLOCKIF_MODE_2MHZ,
};

enum CLOCKIF_SIGNAL
{
    CLOCKIF_SIGNAL_INVALID = 0,
    CLOCKIF_SIGNAL_IN,
    CLOCKIF_SIGNAL_OUT,
    CLOCKIF_SIGNAL_INOUT
};

struct synce_snmp_extern_clock_if_t
{
    uint32_t            ifindex;
    uint8_t             id;
    uint8_t             ssm_level;
    uint8_t             status;
    uint8_t             priority;
    enum CLOCKIF_MODE   mode;
    enum CLOCKIF_SIGNAL signal;
    uint8_t             sa;
    uint8_t             ql_set_flag;
};

void init_mib_synce(void);

#endif                          /* _ALARM_SNMP_H */
