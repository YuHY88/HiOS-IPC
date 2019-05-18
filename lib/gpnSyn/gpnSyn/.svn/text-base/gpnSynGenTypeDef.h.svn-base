/**********************************************************
* file name: gpnSynGenTypeDef.h
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-04-19
* function: 
*    define general syn type about
* modify:
*
***********************************************************/
#ifndef _GPN_SYN_GEN_TYPE_DEF_H_
#define _GPN_SYN_GEN_TYPE_DEF_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "socketComm/gpnSockTypeDef.h"

/*
 * ==========================================================
 * GPN SYN Base Class
 * ==========================================================
 */
#define GPN_SYN_BASE_CLASS_NULL                        0
#define GPN_SYN_BASE_CLASS_NT                          1
#define GPN_SYN_BASE_CLASS_HA                          2

/*
 * ==========================================================
 * GPN SYN NM Master-Slave-Status Define
 * ==========================================================
 */
#define GPN_SYN_MSS_NULL                               0
#define GPN_SYN_MSS_MASTER                             1
#define GPN_SYN_MSS_SLAVE                              2

/*
 * ==========================================================
 * GPN SYN ATTRIBUTE : Syn-Data-Who-Base Type Define
 * ==========================================================
 */
#define GPN_SYN_SDATA_WBASE_NULL                       0
#define GPN_SYN_SDATA_WBASE_MASTER                     1
#define GPN_SYN_SDATA_WBASE_SLAVE                      2
#define GPN_SYN_SDATA_WBASE_MYSELF                     3
#define GPN_SYN_SDATA_WBASE_PARTNER                    4

#define GPN_SYN_SDATA_WBASE_TOT_NUM                    4

/*
 * ==========================================================
 * GPN SYN DATA : SYN DATA INDEX DEFINE
 * ==========================================================
 */
typedef struct _gpnSynSDIndex_
{
	UINT32 dev_index;
	UINT32 portIndex;
	UINT32 portIndex2;
	UINT32 portIndex3;
	UINT32 portIndex4;
	UINT32 portIndex5;
}gpnSynSDIndex;

/*
 * ==========================================================
 * GPN SYN TYPE : Syn-Data-Class/Obj Type Define
 * ==========================================================
 */
#define GPN_SYN_SDATA_CLASS_NULL                       0
#define GPN_SYN_SDATA_OBJ_NULL                         0

/*
 * ==========================================================
 * GPN SYN TYPE : Syn-Data-Class/Obj Type Method
 * ==========================================================
 */
#define GPN_SYN_SDATA_CLASS_TYPE_MASK                  0xFFFF0000
//#define GPN_SYN_SDATA_OBJ_TYPE_MASK                    0x0000FFFF

/*
 * ==========================================================
 * GPN SYN TYPE : Syn-Data-Class/Obj Type Define Base Alarm Moudle
 * ==========================================================
 */
#define GPN_SYN_SDATA_CLASS_ALM_MODU                   0x00010000
#define GPN_SYN_SDATA_OBJ_ALM_MODU_XXXCFG              (GPN_SYN_SDATA_CLASS_ALM_MODU + 1)
#define GPN_SYN_SDATA_OBJ_ALM_STAT_VIEW                (GPN_SYN_SDATA_CLASS_ALM_MODU + 2)
#define GPN_SYN_SDATA_OBJ_ALM_STARTUP_CFG_FILE         (GPN_SYN_SDATA_CLASS_ALM_MODU + 3)
#define GPN_SYN_SDATA_OBJ_ALM_PRODUCT_CFG_FILE         (GPN_SYN_SDATA_CLASS_ALM_MODU + 4)

typedef struct _gpnSynAlmXxxCfgData_
{
	UINT32 cfg;

}gpnSynAlmXxxCfgData;
typedef struct _gpnSynAlmStatData_
{
	UINT32 alm_stat;
	UINT32 evt_stat;
}gpnSynAlmStatData;

#define GPN_SYN_SDATA_CLASS_EMD_MODU                   0x00020000
#define GPN_SYN_SDATA_OBJ_EMD_SLOT_ABLITY_DATA         (GPN_SYN_SDATA_CLASS_EMD_MODU + 1)
#define GPN_SYN_SDATA_OBJ_EMD_SYS_INFO_DATA            (GPN_SYN_SDATA_CLASS_EMD_MODU + 2)
#define GPN_SYN_SDATA_OBJ_EMD_VER_INFO_DATA            (GPN_SYN_SDATA_CLASS_EMD_MODU + 3)
#define GPN_SYN_SDATA_OBJ_EMD_SYS_RES0_STA             (GPN_SYN_SDATA_CLASS_EMD_MODU + 4)
#define GPN_SYN_SDATA_OBJ_EMD_SYS_RES0_CFG             (GPN_SYN_SDATA_CLASS_EMD_MODU + 5)
#define GPN_SYN_SDATA_OBJ_EMD_DEV_ENV_STA              (GPN_SYN_SDATA_CLASS_EMD_MODU + 6)
#define GPN_SYN_SDATA_OBJ_EMD_DEV_ENV_CFG              (GPN_SYN_SDATA_CLASS_EMD_MODU + 7)
#define GPN_SYN_SDATA_OBJ_EMD_FAN_MIX                  (GPN_SYN_SDATA_CLASS_EMD_MODU + 8)
#define GPN_SYN_SDATA_OBJ_EMD_POW_MIX                  (GPN_SYN_SDATA_CLASS_EMD_MODU + 9)
#define GPN_SYN_SDATA_OBJ_EMD_STARTUP_CFG_FILE         (GPN_SYN_SDATA_CLASS_EMD_MODU + 10)
#define GPN_SYN_SDATA_OBJ_EMD_PRODUCT_CFG_FILE         (GPN_SYN_SDATA_CLASS_EMD_MODU + 11)
#define GPN_SYN_SDATA_OBJ_EMD_ADDRESS_CFG_FILE         (GPN_SYN_SDATA_CLASS_EMD_MODU + 12)

typedef struct _gpnSynEmdSlotAblityData_
{
	UINT32 slot_index;
	UINT32 slot_stat;
	UINT32 dev_type;
}gpnSynEmdSlotAblityData;

typedef struct _gpnSynEmdSysInfoData_
{
	UINT32 run_time;

}gpnSynEmdSysInfoData;

#define GPN_SYN_EMD_LEN_VERSION    64
typedef struct _gpnSynEmdVerInfoData_
{
	INT8 hard_ver[GPN_SYN_EMD_LEN_VERSION];
	INT8 soft_ver[GPN_SYN_EMD_LEN_VERSION];
	INT8 kernel_ver[GPN_SYN_EMD_LEN_VERSION];
	INT8 boot_ver[GPN_SYN_EMD_LEN_VERSION];
	INT8 fs_ver[GPN_SYN_EMD_LEN_VERSION];
	INT8 sdk_ver[GPN_SYN_EMD_LEN_VERSION];
	INT8 fpga_ver[GPN_SYN_EMD_LEN_VERSION];
	INT8 seri_num[GPN_SYN_EMD_LEN_VERSION];
}gpnSynEmdVerInfoData;

typedef struct _gpnSynEmdSysResoSta_
{
	UINT32 total_mem;
	UINT32 used_mem;
	UINT32 free_mem;
	UINT32 per_mem;
	UINT32 per_cpu;
}gpnSynEmdSysResoSta;
typedef struct _gpnSynEmdSysResoCfg_
{
	UINT32 thr_mem;
	UINT32 thr_cpu;
}gpnSynEmdSysResoCfg;

typedef struct _gpnSynEmdDevEnvSta_
{
	UINT32 curr_tem;
	UINT32 curr_hum;
}gpnSynEmdDevEnvSta;
typedef struct _gpnSynEmdDevEnvCfg_
{
	UINT32 tem_h_thr;
	UINT32 tem_l_thr;
	UINT32 hum_h_thr;
	UINT32 hum_l_thr;
}gpnSynEmdDevEnvCfg;

typedef struct _gpnSynEmdFanCfgData_
{
	UINT32 cfg;

}gpnSynEmdFanCfgData;

typedef struct _gpnSynEmdPowCfgData_
{
	UINT32 cfg;

}gpnSynEmdPowCfgData;

#define GPN_SYN_SDATA_CLASS_IMI_MODU                   0x00030000
#define GPN_SYN_SDATA_OBJ_IMI_STARTUP_CFG_FILE         (GPN_SYN_SDATA_CLASS_IMI_MODU + 1)
#define GPN_SYN_SDATA_OBJ_IMI_PRODUCT_CFG_FILE         (GPN_SYN_SDATA_CLASS_IMI_MODU + 2)



#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_SYN_GEN_TYPE_DEF_H_*/

