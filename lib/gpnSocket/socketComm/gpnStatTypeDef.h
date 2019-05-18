/**********************************************************
* file name: gpnStatTypeDef.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-03-12
* function: 
*    define all stat types we supported
* modify:
*
***********************************************************/
#ifndef _GPN_STAT_TYPE_DEF_H_
#define _GPN_STAT_TYPE_DEF_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "lib/gpnSocket/socketComm/gpnSockTypeDef.h"


/*stat type mask*/
#define GPN_STAT_TYPE_MASK			0xFFFF0000
/*stat class mask*/
#define GPN_STAT_CLASS_MASK			0xF0000000
/*stat sub class mask*/
#define GPN_STAT_SUBCLASS_MASK		0x0FFF0000

/*sub stat type mask*/
#define GPN_SUB_STAT_TYPE_MASK		0x0000FFFF
/* sub stat class mask*/
#define GPN_SUB_STAT_CLASS_MASK		0x0000F000
/* sub stat class mask*/
#define GPN_SUB_STAT_SUBCLASS_MASK 	0x00000FFF


/* all stat type define*/
#define GPN_STAT_TYPE_ALL 			0xFFFF0000
/* null stat type define*/
#define GPN_STAT_TYPE_NULL 			0x00000000
/* all stat class define*/
#define GPN_STAT_CLASS_ALL 			0xF0000000
/* null stat class define*/
#define GPN_STAT_CLASS_NULL 		0x00000000
/* all stat sub class define */
#define GPN_STAT_SUBCLASS_ALL 		0x0FFF0000
/* null stat sub class define*/
#define GPN_STAT_SUBCLASS_NULL 		0x00000000

/* all sub stat type define */
#define GPN_SUB_STAT_TYPE_ALL 		0x0000FFFF
/*null sub stat type define*/
#define GPN_SUB_STAT_TYPE_NULL 		0x00000000
/* all sub stat class define*/
#define GPN_SUB_STAT_CLASS_ALL 		0x0000F000
/* null sub stat class define*/
#define GPN_SUB_STAT_CLASS_NULL 	0x00000000
/* all sub stat sub class define */
#define GPN_SUB_STAT_SUBCLASS_ALL 	0x00000FFF
/* null sub stat sub class define*/
#define GPN_SUB_STAT_SUBCLASS_NULL 	0x00000000

#define GPN_STAT_TYPE_INVALID 		0xFFFFFFFF
#define GPN_STAT_SUB_TYPE_INVALID 	0xFFFFFFFF


typedef UINT32 GPN_STAT_TYPE;
typedef UINT32 GPN_SUB_STAT_TYPE;

typedef UINT32 GPN_STAT_D_H32;
typedef UINT32 GPN_STAT_D_L32;
typedef UINT32 GPN_TDM_STAT_D;
typedef UINT32 GPN_POS_STAT_D_H32;
typedef UINT32 GPN_POS_STAT_D_L32;
typedef UINT32 GPN_MON_STAT_D;

#define IS_GPN_SUB_STAT_TYPE(subStatType)		(((subStatType) & GPN_SUB_STAT_TYPE_MASK) != 0)	
/************************************************************************************************************/
/* stat type define */
/************************************************************************************************************/
#define GPN_STAT_TDM_TYPE 				0x10000000
#define GPN_STAT_PKT_TYPE 				0x20000000
#define GPN_STAT_MON_TYPE 				0x30000000

/************************************************************************************************************/
/*sub stat type define*/
/************************************************************************************************************/
/*TDM*/
#define GPN_STAT_T_SDH_RS_TYPE          (GPN_STAT_TDM_TYPE + 0x00010000)
#define GPN_STAT_T_SDH_MS_TYPE          (GPN_STAT_TDM_TYPE + 0x00020000)
#define GPN_STAT_T_SDH_HP_TYPE          (GPN_STAT_TDM_TYPE + 0x00030000)
#define GPN_STAT_T_SDH_LP_TYPE          (GPN_STAT_TDM_TYPE + 0x00040000)
#define GPN_STAT_T_PDH_PPI_TYPE         (GPN_STAT_TDM_TYPE + 0x00050000)

/*POS*/
#define GPN_STAT_T_ETH_MAC_P_TYPE       (GPN_STAT_PKT_TYPE + 0x00010000)
#if 0 /* delete VCG stat in PTN stat */
#define GPN_STAT_T_VCG_MAC_TYPE         (GPN_STAT_PKT_TYPE + 0x00020000)
#define GPN_STAT_T_SDH_GFP_TYPE         (GPN_STAT_PKT_TYPE + 0x00030000)
#define GPN_STAT_T_SDH_LAPS_TYPE        (GPN_STAT_PKT_TYPE + 0x00040000)
#endif
#define GPN_STAT_T_PTN_VS_P_TYPE        (GPN_STAT_PKT_TYPE + 0x00050000)
#define GPN_STAT_T_PTN_LSP_P_TYPE       (GPN_STAT_PKT_TYPE + 0x00060000)
#define GPN_STAT_T_PTN_PW_P_TYPE        (GPN_STAT_PKT_TYPE + 0x00070000)
#define GPN_STAT_T_FOLLOW_P_TYPE        (GPN_STAT_PKT_TYPE + 0x00080000)
#define GPN_STAT_T_PWE3_TYPE            (GPN_STAT_PKT_TYPE + 0x00090000)
/* miss A B C */
#define GPN_STAT_T_PTN_VPLSPW_P_TYPE	(GPN_STAT_PKT_TYPE + 0x000D0000)
#define GPN_STAT_T_PTN_V_UNI_P_TYPE		(GPN_STAT_PKT_TYPE + 0x000E0000)

/*MON*/
#define GPN_STAT_T_ETH_SFP_TYPE         (GPN_STAT_MON_TYPE + 0x00010000)
#define GPN_STAT_T_POW_TYPE             (GPN_STAT_MON_TYPE + 0x00020000)
#define GPN_STAT_T_EQU_TYPE             (GPN_STAT_MON_TYPE + 0x00030000)
#define GPN_STAT_T_PTN_VS_M_TYPE        (GPN_STAT_MON_TYPE + 0x00040000)
#define GPN_STAT_T_PTN_LSP_M_TYPE       (GPN_STAT_MON_TYPE + 0x00050000)
#define GPN_STAT_T_PTN_PW_M_TYPE        (GPN_STAT_MON_TYPE + 0x00060000)
#define GPN_STAT_T_ETH_MAC_M_TYPE       (GPN_STAT_MON_TYPE + 0x00070000)
#define GPN_STAT_T_SDH_SFP_TYPE         (GPN_STAT_MON_TYPE + 0x00080000)
#define GPN_STAT_T_PDH_SFP_TYPE         (GPN_STAT_MON_TYPE + 0x00090000)
#define GPN_STAT_T_ENV_TYPE             (GPN_STAT_MON_TYPE + 0x000A0000)
#define GPN_STAT_T_SOFT_TYPE            (GPN_STAT_MON_TYPE + 0x000B0000)
#define GPN_STAT_T_FOLLOW_M_TYPE        (GPN_STAT_MON_TYPE + 0x000C0000)
#define GPN_STAT_T_PTN_VPLSPW_M_TYPE	(GPN_STAT_MON_TYPE + 0x000D0000)
#define GPN_STAT_T_PTN_MEP_M_TYPE	    (GPN_STAT_MON_TYPE + 0x000E0000)


/************************************************************************************************************/
/*sub stat class define*/
/************************************************************************************************************/
#define GPN_COUNTER_CLASS               1
#define GPN_MEASURE_CLASS               2
/*#define GPN_STAT_COUNTER_CLASS                     1U << 12*/
/*#define GPN_STAT_MEASURE_CLASS                     0U << 12*/

#define GPN_STAT_COUNTER_CLASS          0x00001000
#define GPN_STAT_MEASURE_CLASS          0x00000000
/************************************************************************************************************/
/*sub stat class define*/
/************************************************************************************************************/
#define GPN_INTEGER_NUM_CLASS           1 /*... -2,-1,0,1,2...*/
#define GPN_NATURAL_NUM_CLASS           2 /*             0,1,2...*/

/*#define GPN_STAT_INTEGER_NUM_CLASS             1U << 13*/
/*#define GPN_STAT_NATURAL_NUM_CLASS            0U << 13*/

#define GPN_STAT_INTEGER_NUM_CLASS      0x00002000
#define GPN_STAT_NATURAL_NUM_CLASS      0x00000000


/************************************************************************************************************/
/*sub stat sub class define*/
/************************************************************************************************************/
/*SDH RS stat type define */
#define GPN_STAT_T_SDH_RS_COUNT_TYPE    ((GPN_STAT_T_SDH_RS_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_SDH_RS_MEASU_TYPE    ((GPN_STAT_T_SDH_RS_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_SDH_RS_BBE_TYPE      (GPN_STAT_T_SDH_RS_COUNT_TYPE + 0x00000001)
#define GPN_STAT_D_SDH_RS_ES_TYPE       (GPN_STAT_T_SDH_RS_COUNT_TYPE + 0x00000002)	
#define GPN_STAT_D_SDH_RS_SES_TYPE      (GPN_STAT_T_SDH_RS_COUNT_TYPE + 0x00000003)
#define GPN_STAT_D_SDH_RS_UAS_TYPE      (GPN_STAT_T_SDH_RS_COUNT_TYPE + 0x00000004)
#define GPN_STAT_D_SDH_RS_OFS_TYPE      (GPN_STAT_T_SDH_RS_COUNT_TYPE + 0x00000005)

/*SDH RS stat data struct define */
typedef struct _gpnStatSdhRsData_
{
	GPN_TDM_STAT_D statSdhRsBBE;
	GPN_TDM_STAT_D statSdhRsES;
	GPN_TDM_STAT_D statSdhRsSES;
	GPN_TDM_STAT_D statSdhRsUAS;
	GPN_TDM_STAT_D statSdhRsOFS;
	/*5*/
}gpnStatSdhRsData;

/*SDH MS stat type define */
#define GPN_T_STAT_SDH_MS_COUNT_TYPE    ((GPN_STAT_T_SDH_MS_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_T_STAT_SDH_MS_MEASU_TYPE    ((GPN_STAT_T_SDH_MS_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_SDH_MS_BBE_TYPE      (GPN_T_STAT_SDH_MS_COUNT_TYPE + 0x00000001)
#define GPN_STAT_D_SDH_MS_ES_TYPE       (GPN_T_STAT_SDH_MS_COUNT_TYPE + 0x00000002)
#define GPN_STAT_D_SDH_MS_SES_TYPE      (GPN_T_STAT_SDH_MS_COUNT_TYPE + 0x00000003)
#define GPN_STAT_D_SDH_MS_UAS_TYPE      (GPN_T_STAT_SDH_MS_COUNT_TYPE + 0x00000004)
#define GPN_STAT_D_SDH_MS_REI_BBE_TYPE  (GPN_T_STAT_SDH_MS_COUNT_TYPE + 0x00000005)
#define GPN_STAT_D_SDH_MS_REI_ES_TYPE   (GPN_T_STAT_SDH_MS_COUNT_TYPE + 0x00000006)
#define GPN_STAT_D_SDH_MS_REI_SES_TYPE  (GPN_T_STAT_SDH_MS_COUNT_TYPE + 0x00000007)

/*SDH MS stat data struct define */
typedef struct _gpnStatSdhMsData_
{
	GPN_TDM_STAT_D statSdhMsBBE;
	GPN_TDM_STAT_D statSdhMsES;
	GPN_TDM_STAT_D statSdhMsSES;
	GPN_TDM_STAT_D statSdhMsUAS;
	GPN_TDM_STAT_D statSdhMsReiBBE;
	GPN_TDM_STAT_D statSdhMsReiES;
	GPN_TDM_STAT_D statSdhMsReiSES;
	/* 7 */
}gpnStatSdhMsData;

/*SDH HP stat type define */
#define GPN_STAT_T_SDH_HP_COUNT_TYPE    ((GPN_STAT_T_SDH_HP_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_SDH_HP_MEASU_TYPE    ((GPN_STAT_T_SDH_HP_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_SDH_HP_BBE_TYPE      (GPN_STAT_T_SDH_HP_COUNT_TYPE + 0x00000001)
#define GPN_STAT_D_SDH_HP_ES_TYPE       (GPN_STAT_T_SDH_HP_COUNT_TYPE + 0x00000002)
#define GPN_STAT_D_SDH_HP_SES_TYPE      (GPN_STAT_T_SDH_HP_COUNT_TYPE + 0x00000003)
#define GPN_STAT_D_SDH_HP_UAS_TYPE      (GPN_STAT_T_SDH_HP_COUNT_TYPE + 0x00000004)
#define GPN_STAT_D_SDH_HP_REI_BBE_TYPE  (GPN_STAT_T_SDH_HP_COUNT_TYPE + 0x00000005)
#define GPN_STAT_D_SDH_HP_REI_ES_TYPE   (GPN_STAT_T_SDH_HP_COUNT_TYPE + 0x00000006)
#define GPN_STAT_D_SDH_HP_REI_SES_TYPE  (GPN_STAT_T_SDH_HP_COUNT_TYPE + 0x00000007)
#define GPN_STAT_D_SDH_HP_AU_PJCP_TYPE  (GPN_STAT_T_SDH_HP_COUNT_TYPE + 0x00000008)
#define GPN_STAT_D_SDH_HP_AU_PJCN_TYPE  (GPN_STAT_T_SDH_HP_COUNT_TYPE + 0x00000009)

/*SDH HP stat data struct define */
typedef struct _gpnStatSdhHpData_
{
	GPN_TDM_STAT_D statSdhHpBBE;
	GPN_TDM_STAT_D statSdhHpES;
	GPN_TDM_STAT_D statSdhHpSES;
	GPN_TDM_STAT_D statSdhHpUAS;
	GPN_TDM_STAT_D statSdhHpReiBBE;
	GPN_TDM_STAT_D statSdhHpReiES;
	GPN_TDM_STAT_D statSdhHpReiSES;
	GPN_TDM_STAT_D statSdhHpAuPJCP;
	GPN_TDM_STAT_D statSdhHpAuPJCN;
	/* 9 */
}gpnStatSdhHpData;

/*SDH LP stat type define */
#define GPN_STAT_T_SDH_LP_COUNT_TYPE    ((GPN_STAT_T_SDH_LP_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_SDH_LP_MEASU_TYPE    ((GPN_STAT_T_SDH_LP_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_SDH_LP_BBE_TYPE      (GPN_STAT_T_SDH_LP_COUNT_TYPE + 0x00000001)
#define GPN_STAT_D_SDH_LP_ES_TYPE       (GPN_STAT_T_SDH_LP_COUNT_TYPE + 0x00000002)
#define GPN_STAT_D_SDH_LP_SES_TYPE      (GPN_STAT_T_SDH_LP_COUNT_TYPE + 0x00000003)
#define GPN_STAT_D_SDH_LP_UAS_TYPE      (GPN_STAT_T_SDH_LP_COUNT_TYPE + 0x00000004)
#define GPN_STAT_D_SDH_LP_REI_BBE_TYPE  (GPN_STAT_T_SDH_LP_COUNT_TYPE + 0x00000005)
#define GPN_STAT_D_SDH_LP_REI_ES_TYPE   (GPN_STAT_T_SDH_LP_COUNT_TYPE + 0x00000006)
#define GPN_STAT_D_SDH_LP_REI_SES_TYPE  (GPN_STAT_T_SDH_LP_COUNT_TYPE + 0x00000007)
#define GPN_STAT_D_SDH_LP_TU_PJCP_TYPE  (GPN_STAT_T_SDH_LP_COUNT_TYPE + 0x00000008)
#define GPN_STAT_D_SDH_LP_TU_PJCN_TYPE  (GPN_STAT_T_SDH_LP_COUNT_TYPE + 0x00000009)

/*SDH HP stat data struct define */
typedef struct _gpnStatSdhLpData_
{
	GPN_TDM_STAT_D statSdhLpBBE;
	GPN_TDM_STAT_D statSdhLpES;
	GPN_TDM_STAT_D statSdhLpSES;
	GPN_TDM_STAT_D statSdhLpUAS;
	GPN_TDM_STAT_D statSdhLpReiBBE;
	GPN_TDM_STAT_D statSdhLpReiES;
	GPN_TDM_STAT_D statSdhLpReiSES;
	GPN_TDM_STAT_D statSdhLpTuPJCP;
	GPN_TDM_STAT_D statSdhLpTuPJCN;
	/* 9 */
}gpnStatSdhLpData;

/*PDH PPI stat type define */
#define GPN_STAT_T_PDH_PPI_COUNT_TYPE   ((GPN_STAT_T_PDH_PPI_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_PDH_PPI_MEASU_TYPE   ((GPN_STAT_T_PDH_PPI_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_PDH_BBE_TYPE         (GPN_STAT_T_PDH_PPI_COUNT_TYPE + 0x00000001)
#define GPN_STAT_D_PDH_ES_TYPE          (GPN_STAT_T_PDH_PPI_COUNT_TYPE + 0x00000002)
#define GPN_STAT_D_PDH_SES_TYPE         (GPN_STAT_T_PDH_PPI_COUNT_TYPE + 0x00000003)
#define GPN_STAT_D_PDH_UAS_TYPE         (GPN_STAT_T_PDH_PPI_COUNT_TYPE + 0x00000004)
#define GPN_STAT_D_PDH_CV_TYPE          (GPN_STAT_T_PDH_PPI_COUNT_TYPE + 0x00000005)

/*PDH PPI stat data struct define */
typedef struct _gpnStatPdhPpiData_
{
    GPN_TDM_STAT_D statPdhBBE;
	GPN_TDM_STAT_D statPdhES;
	GPN_TDM_STAT_D statPdhSES;	
	GPN_TDM_STAT_D statPdhUAS;
	GPN_TDM_STAT_D statPdhCV;
	/* 5*1 */
}gpnStatPdhPpiData;

/*----------------------------------------------------------------------------------------------------------*/

/*ETH MAC stat type define */
#define GPN_STAT_T_ETH_MAC_P_COUNT_TYPE     ((GPN_STAT_T_ETH_MAC_P_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_ETH_MAC_P_MEASU_TYPE     ((GPN_STAT_T_ETH_MAC_P_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_ETH_MAC_RXGB_TYPE        (GPN_STAT_T_ETH_MAC_P_COUNT_TYPE + 0x00000001)
#define GPN_STAT_D_ETH_MAC_RXGP_TYPE        (GPN_STAT_T_ETH_MAC_P_COUNT_TYPE + 0x00000002)
#define GPN_STAT_D_ETH_MAC_TXGB_TYPE        (GPN_STAT_T_ETH_MAC_P_COUNT_TYPE + 0x00000003)
#define GPN_STAT_D_ETH_MAC_TXGP_TYPE        (GPN_STAT_T_ETH_MAC_P_COUNT_TYPE + 0x00000004)
#define GPN_STAT_D_ETH_MAC_FCS_TYPE         (GPN_STAT_T_ETH_MAC_P_COUNT_TYPE + 0x00000005)
#define GPN_STAT_D_ETH_MAC_DROP_TYPE		(GPN_STAT_T_ETH_MAC_P_COUNT_TYPE + 0x00000006)
#define GPN_STAT_D_ETH_MAC_RXB_TYPE			(GPN_STAT_T_ETH_MAC_P_COUNT_TYPE + 0x00000007)
#define GPN_STAT_D_ETH_MAC_RXP_TYPE			(GPN_STAT_T_ETH_MAC_P_COUNT_TYPE + 0x00000008)
#define GPN_STAT_D_ETH_MAC_TXB_TYPE			(GPN_STAT_T_ETH_MAC_P_COUNT_TYPE + 0x00000009)
#define GPN_STAT_D_ETH_MAC_TXP_TYPE			(GPN_STAT_T_ETH_MAC_P_COUNT_TYPE + 0x0000000A)
#define GPN_STAT_D_ETH_MAC_RERR_TYPE		(GPN_STAT_T_ETH_MAC_P_COUNT_TYPE + 0x0000000B)

/* 2018/3/15, lipf, add for ipran */
#define GPN_STAT_D_ETH_MAC_ROSPC_TYPE		(GPN_STAT_T_ETH_MAC_P_COUNT_TYPE + 0x0000000C)
#define GPN_STAT_D_ETH_MAC_RUSPC_TYPE		(GPN_STAT_T_ETH_MAC_P_COUNT_TYPE + 0x0000000D)
#define GPN_STAT_D_ETH_MAC_RUCPC_TYPE		(GPN_STAT_T_ETH_MAC_P_COUNT_TYPE + 0x0000000E)
#define GPN_STAT_D_ETH_MAC_RMCPC_TYPE		(GPN_STAT_T_ETH_MAC_P_COUNT_TYPE + 0x0000000F)
#define GPN_STAT_D_ETH_MAC_RBCPC_TYPE		(GPN_STAT_T_ETH_MAC_P_COUNT_TYPE + 0x00000010)
#define GPN_STAT_D_ETH_MAC_TUCPC_TYPE		(GPN_STAT_T_ETH_MAC_P_COUNT_TYPE + 0x00000011)
#define GPN_STAT_D_ETH_MAC_TMCPC_TYPE		(GPN_STAT_T_ETH_MAC_P_COUNT_TYPE + 0x00000012)
#define GPN_STAT_D_ETH_MAC_TBCPC_TYPE		(GPN_STAT_T_ETH_MAC_P_COUNT_TYPE + 0x00000013)
/* end */

/*ETH MAC stat data struct define */
typedef struct _gpnStatEthMacPData_
{
	GPN_POS_STAT_D_H32 statEthMacRXGBh32;
	GPN_POS_STAT_D_L32 statEthMacRXGBl32;
	GPN_POS_STAT_D_H32 statEthMacRXGPh32;
	GPN_POS_STAT_D_L32 statEthMacRXGPl32;
	GPN_POS_STAT_D_H32 statEthMacTXGBh32;
	GPN_POS_STAT_D_L32 statEthMacTXGBl32;
	GPN_POS_STAT_D_H32 statEthMacTXGPh32;
	GPN_POS_STAT_D_L32 statEthMacTXGPl32;
	GPN_POS_STAT_D_H32 statEthMacFCSh32;
	GPN_POS_STAT_D_L32 statEthMacFCSl32;
	GPN_POS_STAT_D_H32 statEthMacDroph32;
	GPN_POS_STAT_D_L32 statEthMacDropl32;
	GPN_POS_STAT_D_H32 statEthMacRXBh32;
	GPN_POS_STAT_D_L32 statEthMacRXBl32;
	GPN_POS_STAT_D_H32 statEthMacRXPh32;
	GPN_POS_STAT_D_L32 statEthMacRXPl32;
	GPN_POS_STAT_D_H32 statEthMacTXBh32;
	GPN_POS_STAT_D_L32 statEthMacTXBl32;
	GPN_POS_STAT_D_H32 statEthMacTXPh32;
	GPN_POS_STAT_D_L32 statEthMacTXPl32;
	GPN_POS_STAT_D_H32 statEthMacRERRSh32;
	GPN_POS_STAT_D_L32 statEthMacRERRl32;

	/* 2018/3/15, lipf, add for ipran */
	GPN_POS_STAT_D_H32 statEthMacROSPCh32;
	GPN_POS_STAT_D_L32 statEthMacROSPCl32;
	GPN_POS_STAT_D_H32 statEthMacRUSPCh32;
	GPN_POS_STAT_D_L32 statEthMacRUSPCl32;
	GPN_POS_STAT_D_H32 statEthMacRUCPCh32;
	GPN_POS_STAT_D_L32 statEthMacRUCPCl32;
	GPN_POS_STAT_D_H32 statEthMacRMCPCh32;
	GPN_POS_STAT_D_L32 statEthMacRMCPCl32;
	GPN_POS_STAT_D_H32 statEthMacRBCPCh32;
	GPN_POS_STAT_D_L32 statEthMacRBCPCl32;
	GPN_POS_STAT_D_H32 statEthMacTUCPCh32;
	GPN_POS_STAT_D_L32 statEthMacTUCPCl32;
	GPN_POS_STAT_D_H32 statEthMacTMCPCh32;
	GPN_POS_STAT_D_L32 statEthMacTMCPCl32;
	GPN_POS_STAT_D_H32 statEthMacTBCPCh32;
	GPN_POS_STAT_D_L32 statEthMacTBCPCl32;
	/* end */

	/* 19*2 */
}gpnStatEthMacPData;

#if 0 /* delete VCG stat in PTN stat */
/*VCG MAC stat data struct define */
#define GPN_STAT_T_VCG_MAC_COUNT_TYPE       ((GPN_STAT_T_VCG_MAC_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_VCG_MAC_MEASU_TYPE       ((GPN_STAT_T_VCG_MAC_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_VCG_MAC_TXB_TYPE         (GPN_STAT_T_VCG_MAC_COUNT_TYPE + 0x00000001)
#define GPN_STAT_D_VCG_MAC_RXB_TYPE         (GPN_STAT_T_VCG_MAC_COUNT_TYPE + 0x00000002)
#define GPN_STAT_D_VCG_MAC_TXP_TYPE         (GPN_STAT_T_VCG_MAC_COUNT_TYPE + 0x00000003)
#define GPN_STAT_D_VCG_MAC_RXP_TYPE         (GPN_STAT_T_VCG_MAC_COUNT_TYPE + 0x00000004)
#define GPN_STAT_D_VCG_MAC_TXSF_TYPE        (GPN_STAT_T_VCG_MAC_COUNT_TYPE + 0x00000005)
#define GPN_STAT_D_VCG_MAC_RXSF_TYPE        (GPN_STAT_T_VCG_MAC_COUNT_TYPE + 0x00000006)
#define GPN_STAT_D_VCG_MAC_TXEF_TYPE        (GPN_STAT_T_VCG_MAC_COUNT_TYPE + 0x00000007)
#define GPN_STAT_D_VCG_MAC_RXEF_TYPE        (GPN_STAT_T_VCG_MAC_COUNT_TYPE + 0x00000008)
#define GPN_STAT_D_VCG_MAC_TXFCS_TYPE       (GPN_STAT_T_VCG_MAC_COUNT_TYPE + 0x00000009)
#define GPN_STAT_D_VCG_MAC_RXFCS_TYPE       (GPN_STAT_T_VCG_MAC_COUNT_TYPE + 0x0000000A)
#define GPN_STAT_D_VCG_MAC_TXUN_TYPE        (GPN_STAT_T_VCG_MAC_COUNT_TYPE + 0x0000000B)
#define GPN_STAT_D_VCG_MAC_RXUN_TYPE        (GPN_STAT_T_VCG_MAC_COUNT_TYPE + 0x0000000C)
#define GPN_STAT_D_VCG_MAC_TXMU_TYPE        (GPN_STAT_T_VCG_MAC_COUNT_TYPE + 0x0000000D)
#define GPN_STAT_D_VCG_MAC_RXMU_TYPE        (GPN_STAT_T_VCG_MAC_COUNT_TYPE + 0x0000000E)
#define GPN_STAT_D_VCG_MAC_TXBR_TYPE        (GPN_STAT_T_VCG_MAC_COUNT_TYPE + 0x0000000F)
#define GPN_STAT_D_VCG_MAC_RXBR_TYPE        (GPN_STAT_T_VCG_MAC_COUNT_TYPE + 0x00000010)

/*VCG MAC stat data struct define */
typedef struct _gpnStatVcgMacData_
{
	GPN_POS_STAT_D_H32 statVcgMacTXBh32;
	GPN_POS_STAT_D_L32 statVcgMacTXBl32;
	GPN_POS_STAT_D_H32 statVcgMacRXBh32;
	GPN_POS_STAT_D_L32 statVcgMacRXBl32;
	GPN_POS_STAT_D_H32 statVcgMacTXPh32;
	GPN_POS_STAT_D_L32 statVcgMacTXPl32;
	GPN_POS_STAT_D_H32 statVcgMacRXPh32;
	GPN_POS_STAT_D_L32 statVcgMacRXPl32;
	GPN_POS_STAT_D_H32 statVcgMacTXSFh32;
	GPN_POS_STAT_D_L32 statVcgMacTXSFl32;
	GPN_POS_STAT_D_H32 statVcgMacRXSFh32;
	GPN_POS_STAT_D_L32 statVcgMacRXSFl32;
	GPN_POS_STAT_D_H32 statVcgMacTXEFh32;
	GPN_POS_STAT_D_L32 statVcgMacTXEFl32;
	GPN_POS_STAT_D_H32 statVcgMacRXEFh32;
	GPN_POS_STAT_D_L32 statVcgMacRXEFl32;
	GPN_POS_STAT_D_H32 statVcgMacTXFCSh32;
	GPN_POS_STAT_D_L32 statVcgMacTXFCSl32;
	GPN_POS_STAT_D_H32 statVcgMacRXFCSh32;
	GPN_POS_STAT_D_L32 statVcgMacRXFCSl32;
	GPN_POS_STAT_D_H32 statVcgMacTXUNh32;
	GPN_POS_STAT_D_L32 statVcgMacTXUNl32;
	GPN_POS_STAT_D_H32 statVcgMacRXUNh32;
	GPN_POS_STAT_D_L32 statVcgMacRXUNl32;
	GPN_POS_STAT_D_H32 statVcgMacTXMUh32;
	GPN_POS_STAT_D_L32 statVcgMacTXMUl32;
	GPN_POS_STAT_D_H32 statVcgMacRXMUh32;
	GPN_POS_STAT_D_L32 statVcgMacRXMUl32;
	GPN_POS_STAT_D_H32 statVcgMacTXBRh32;
	GPN_POS_STAT_D_L32 statVcgMacTXBRl32;
	GPN_POS_STAT_D_H32 statVcgMacRXBRh32;
	GPN_POS_STAT_D_L32 statVcgMacRXBRl32;
	/* 16*2 */
}gpnStatVcgMacData;

/*VCG GFP stat type define */
#define GPN_STAT_T_SDH_GFP_COUNT_TYPE   ((GPN_STAT_T_SDH_GFP_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_SDH_GFP_MEASU_TYPE   ((GPN_STAT_T_SDH_GFP_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_VCG_GFP_TXF_TYPE     (GPN_STAT_T_SDH_GFP_COUNT_TYPE + 0x00000001)
#define GPN_STAT_D_VCG_GFP_TXB_TYPE     (GPN_STAT_T_SDH_GFP_COUNT_TYPE + 0x00000002)
#define GPN_STAT_D_VCG_GFP_RXF_TYPE     (GPN_STAT_T_SDH_GFP_COUNT_TYPE + 0x00000003)
#define GPN_STAT_D_VCG_GFP_RXB_TYPE     (GPN_STAT_T_SDH_GFP_COUNT_TYPE + 0x00000004)
#define GPN_STAT_D_VCG_GFP_RXFCS_TYPE   (GPN_STAT_T_SDH_GFP_COUNT_TYPE + 0x00000005)
#define GPN_STAT_D_VCG_GFP_CHECAE_TYPE  (GPN_STAT_T_SDH_GFP_COUNT_TYPE + 0x00000006)
#define GPN_STAT_D_VCG_GFP_CHEC_TYPE    (GPN_STAT_T_SDH_GFP_COUNT_TYPE + 0x00000007)
#define GPN_STAT_D_VCG_GFP_THECAE_TYPE  (GPN_STAT_T_SDH_GFP_COUNT_TYPE + 0x00000008)
#define GPN_STAT_D_VCG_GFP_THEC_TYPE    (GPN_STAT_T_SDH_GFP_COUNT_TYPE + 0x00000009)
#define GPN_STAT_D_VCG_GFP_EHECAE_TYPE  (GPN_STAT_T_SDH_GFP_COUNT_TYPE + 0x0000000A)
#define GPN_STAT_D_VCG_GFP_EHEC_TYPE    (GPN_STAT_T_SDH_GFP_COUNT_TYPE + 0x0000000B)
#define GPN_STAT_D_VCG_GFP_RXLOF_TYPE   (GPN_STAT_T_SDH_GFP_COUNT_TYPE + 0x0000000C)
#define GPN_STAT_D_VCG_GFP_PTI_MM_TYPE  (GPN_STAT_T_SDH_GFP_COUNT_TYPE + 0x0000000D)
#define GPN_STAT_D_VCG_GFP_EXI_MM_TYPE  (GPN_STAT_T_SDH_GFP_COUNT_TYPE + 0x0000000E)
#define GPN_STAT_D_VCG_GFP_UPI_MM_TYPE  (GPN_STAT_T_SDH_GFP_COUNT_TYPE + 0x0000000F)
#define GPN_STAT_D_VCG_GFP_CID_MM_TYPE  (GPN_STAT_T_SDH_GFP_COUNT_TYPE + 0x00000010)

/*VCG GFP stat data struct define */
typedef struct _gpnStatSdhGfpData_
{
	GPN_POS_STAT_D_H32 statVcgGfpTXFh32;
	GPN_POS_STAT_D_L32 statVcgGfpTXFl32;
	GPN_POS_STAT_D_H32 statVcgGfpTXBh32;
	GPN_POS_STAT_D_L32 statVcgGfpTXBl32;
	GPN_POS_STAT_D_H32 statVcgGfpRXFh32;
	GPN_POS_STAT_D_L32 statVcgGfpRXFl32;
	GPN_POS_STAT_D_H32 statVcgGfpRXBh32;
	GPN_POS_STAT_D_L32 statVcgGfpRXBl32;
	GPN_POS_STAT_D_H32 statVcgGfpRXFCSh32;
	GPN_POS_STAT_D_L32 statVcgGfpRXFCSl32;
	GPN_POS_STAT_D_H32 statVcgGfpCHECAEh32;
	GPN_POS_STAT_D_L32 statVcgGfpCHECAEl32;
	GPN_POS_STAT_D_H32 statVcgGfpCHECh32;
	GPN_POS_STAT_D_L32 statVcgGfpCHECl32;
	GPN_POS_STAT_D_H32 statVcgGfpTHECAEh32;
	GPN_POS_STAT_D_L32 statVcgGfpTHECAEl32;
	GPN_POS_STAT_D_H32 statVcgGfpTHECh32;
	GPN_POS_STAT_D_L32 statVcgGfpTHECl32;
	GPN_POS_STAT_D_H32 statVcgGfpEHECAEh32;
	GPN_POS_STAT_D_L32 statVcgGfpEHECAEl32;
	GPN_POS_STAT_D_H32 statVcgGfpEHECh32;
	GPN_POS_STAT_D_L32 statVcgGfpEHECl32;
	GPN_POS_STAT_D_H32 statVcgGfpRXLOFh32;
	GPN_POS_STAT_D_L32 statVcgGfpRXLOFl32;
	GPN_POS_STAT_D_H32 statVcgGfpPTI_MMh32;
	GPN_POS_STAT_D_L32 statVcgGfpPTI_MMl32;
	GPN_POS_STAT_D_H32 statVcgGfpEXI_MMh32;
	GPN_POS_STAT_D_L32 statVcgGfpEXI_MMl32;
	GPN_POS_STAT_D_H32 statVcgGfpUPI_MMh32;
	GPN_POS_STAT_D_L32 statVcgGfpUPI_MMl32;
	GPN_POS_STAT_D_H32 statVcgGfpCID_MMh32;
	GPN_POS_STAT_D_L32 statVcgGfpCID_MMl32;
	/* 16*2 */
}gpnStatSdhGfpData;
#endif

/*PTN VS stat type define */
#define GPN_STAT_T_PTN_VS_P_COUNT_TYPE      ((GPN_STAT_T_PTN_VS_P_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_PTN_VS_P_MEASU_TYPE      ((GPN_STAT_T_PTN_VS_P_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_PTN_VS_TXF_TYPE          (GPN_STAT_T_PTN_VS_P_COUNT_TYPE + 0x00000001)
#define GPN_STAT_D_PTN_VS_TXB_TYPE          (GPN_STAT_T_PTN_VS_P_COUNT_TYPE + 0x00000002)
#define GPN_STAT_D_PTN_VS_RXF_TYPE          (GPN_STAT_T_PTN_VS_P_COUNT_TYPE + 0x00000003)
#define GPN_STAT_D_PTN_VS_RXB_TYPE          (GPN_STAT_T_PTN_VS_P_COUNT_TYPE + 0x00000004)

/*PTN VS stat data struct define */
typedef struct _gpnStatPtnVsPData_
{
	GPN_POS_STAT_D_H32 statPtnVsTXFh32;
	GPN_POS_STAT_D_L32 statPtnVsTXFl32;
	GPN_POS_STAT_D_H32 statPtnVsTXBh32;
	GPN_POS_STAT_D_L32 statPtnVsTXBl32;
	GPN_POS_STAT_D_H32 statPtnVsRXFh32;
	GPN_POS_STAT_D_L32 statPtnVsRXFl32;
	GPN_POS_STAT_D_H32 statPtnVsRXBh32;
	GPN_POS_STAT_D_L32 statPtnVsRXBl32;
	/* 4*2 */
}gpnStatPtnVsPData;

/*PTN LSP POS stat type define */
#define GPN_STAT_T_PTN_LSP_P_COUNT_TYPE      ((GPN_STAT_T_PTN_LSP_P_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_PTN_LSP_P_MEASU_TYPE      ((GPN_STAT_T_PTN_LSP_P_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_PTN_LSP_TXF_TYPE          (GPN_STAT_T_PTN_LSP_P_COUNT_TYPE + 0x00000001)
#define GPN_STAT_D_PTN_LSP_TXB_TYPE          (GPN_STAT_T_PTN_LSP_P_COUNT_TYPE + 0x00000002)
#define GPN_STAT_D_PTN_LSP_RXF_TYPE          (GPN_STAT_T_PTN_LSP_P_COUNT_TYPE + 0x00000003)
#define GPN_STAT_D_PTN_LSP_RXB_TYPE          (GPN_STAT_T_PTN_LSP_P_COUNT_TYPE + 0x00000004)
#define GPN_STAT_D_PTN_REV_LSP_TXF_TYPE      (GPN_STAT_T_PTN_LSP_P_COUNT_TYPE + 0x00000005)
#define GPN_STAT_D_PTN_REV_LSP_TXB_TYPE      (GPN_STAT_T_PTN_LSP_P_COUNT_TYPE + 0x00000006)
#define GPN_STAT_D_PTN_REV_LSP_RXF_TYPE      (GPN_STAT_T_PTN_LSP_P_COUNT_TYPE + 0x00000007)
#define GPN_STAT_D_PTN_REV_LSP_RXB_TYPE      (GPN_STAT_T_PTN_LSP_P_COUNT_TYPE + 0x00000008)

/*PTN LSP stat data struct define */
typedef struct _gpnStatPtnLspPData_
{
	GPN_POS_STAT_D_H32 statPtnLspTXFh32;
	GPN_POS_STAT_D_L32 statPtnLspTXFl32;
	GPN_POS_STAT_D_H32 statPtnLspTXBh32;
	GPN_POS_STAT_D_L32 statPtnLspTXBl32;
	GPN_POS_STAT_D_H32 statPtnLspRXFh32;
	GPN_POS_STAT_D_L32 statPtnLspRXFl32;
	GPN_POS_STAT_D_H32 statPtnLspRXBh32;
	GPN_POS_STAT_D_L32 statPtnLspRXBl32;
	GPN_POS_STAT_D_H32 statPtnRevLspTXFh32;
	GPN_POS_STAT_D_L32 statPtnRevLspTXFl32;
	GPN_POS_STAT_D_H32 statPtnRevLspTXBh32;
	GPN_POS_STAT_D_L32 statPtnRevLspTXBl32;
	GPN_POS_STAT_D_H32 statPtnRevLspRXFh32;
	GPN_POS_STAT_D_L32 statPtnRevLspRXFl32;
	GPN_POS_STAT_D_H32 statPtnRevLspRXBh32;
	GPN_POS_STAT_D_L32 statPtnRevLspRXBl32;
	/* 8*2 */
}gpnStatPtnLspPData;

/*PTN PW POS stat type define */
#define GPN_STAT_T_PTN_PW_P_COUNT_TYPE      ((GPN_STAT_T_PTN_PW_P_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_PTN_PW_P_MEASU_TYPE      ((GPN_STAT_T_PTN_PW_P_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_PTN_PW_TXF_TYPE          (GPN_STAT_T_PTN_PW_P_COUNT_TYPE + 0x00000001)
#define GPN_STAT_D_PTN_PW_TXB_TYPE          (GPN_STAT_T_PTN_PW_P_COUNT_TYPE + 0x00000002)
#define GPN_STAT_D_PTN_PW_RXF_TYPE          (GPN_STAT_T_PTN_PW_P_COUNT_TYPE + 0x00000003)
#define GPN_STAT_D_PTN_PW_RXB_TYPE          (GPN_STAT_T_PTN_PW_P_COUNT_TYPE + 0x00000004)

/*PTNPW stat data struct define */
typedef struct _gpnStatPtnPwPData_
{
	GPN_POS_STAT_D_H32 statPtnPwTXFh32;
	GPN_POS_STAT_D_L32 statPtnPwTXFl32;
	GPN_POS_STAT_D_H32 statPtnPwTXBh32;
	GPN_POS_STAT_D_L32 statPtnPwTXBl32;
	GPN_POS_STAT_D_H32 statPtnPwRXFh32;
	GPN_POS_STAT_D_L32 statPtnPwRXFl32;
	GPN_POS_STAT_D_H32 statPtnPwRXBh32;
	GPN_POS_STAT_D_L32 statPtnPwRXBl32;
	/* 4*2 */
}gpnStatPtnPwPData;

/*PTN VPLS PW POS stat type define */
#define GPN_STAT_T_PTN_VPLSPW_P_COUNT_TYPE      ((GPN_STAT_T_PTN_VPLSPW_P_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_PTN_VPLSPW_P_MEASU_TYPE      ((GPN_STAT_T_PTN_VPLSPW_P_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_PTN_VPLSPW_TXF_TYPE          (GPN_STAT_T_PTN_VPLSPW_P_COUNT_TYPE + 0x00000001)
#define GPN_STAT_D_PTN_VPLSPW_TXB_TYPE          (GPN_STAT_T_PTN_VPLSPW_P_COUNT_TYPE + 0x00000002)
#define GPN_STAT_D_PTN_VPLSPW_RXF_TYPE          (GPN_STAT_T_PTN_VPLSPW_P_COUNT_TYPE + 0x00000003)
#define GPN_STAT_D_PTN_VPLSPW_RXB_TYPE          (GPN_STAT_T_PTN_VPLSPW_P_COUNT_TYPE + 0x00000004)

/*PTNPW stat data struct define */
typedef struct _gpnStatPtnVplsPwPData_
{
	GPN_POS_STAT_D_H32 statPtnVplsPwTXFh32;
	GPN_POS_STAT_D_L32 statPtnVplsPwTXFl32;
	GPN_POS_STAT_D_H32 statPtnVplsPwTXBh32;
	GPN_POS_STAT_D_L32 statPtnVplsPwTXBl32;
	GPN_POS_STAT_D_H32 statPtnVplsPwRXFh32;
	GPN_POS_STAT_D_L32 statPtnVplsPwRXFl32;
	GPN_POS_STAT_D_H32 statPtnVplsPwRXBh32;
	GPN_POS_STAT_D_L32 statPtnVplsPwRXBl32;
	/* 4*2 */
}gpnStatPtnVplsPwPData;


/*follow stat type define */
#define GPN_STAT_T_FOLLOW_P_COUNT_TYPE      ((GPN_STAT_T_FOLLOW_P_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_FOLLOW_P_MEASU_TYPE      ((GPN_STAT_T_FOLLOW_P_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_FOLLOW_TXF_TYPE          (GPN_STAT_T_FOLLOW_P_COUNT_TYPE + 0x00000001)
#define GPN_STAT_D_FOLLOW_TXB_TYPE          (GPN_STAT_T_FOLLOW_P_COUNT_TYPE + 0x00000002)
#define GPN_STAT_D_FOLLOW_RXF_TYPE          (GPN_STAT_T_FOLLOW_P_COUNT_TYPE + 0x00000003)
#define GPN_STAT_D_FOLLOW_RXB_TYPE          (GPN_STAT_T_FOLLOW_P_COUNT_TYPE + 0x00000004)

/*follow stat data struct define */
typedef struct _gpnStatFollowPData_
{
	GPN_POS_STAT_D_H32 statFollowTXFh32;
	GPN_POS_STAT_D_L32 statFollowTXFl32;
	GPN_POS_STAT_D_H32 statFollowTXBh32;
	GPN_POS_STAT_D_L32 statFollowTXBl32;
	GPN_POS_STAT_D_H32 statFollowRXFh32;
	GPN_POS_STAT_D_L32 statFollowRXFl32;
	GPN_POS_STAT_D_H32 statFollowRXBh32;
	GPN_POS_STAT_D_L32 statFollowRXBl32;
	/* 4*2 */
}gpnStatFollowPData;

/*PWE3 stat type define */
#define GPN_STAT_T_PWE3_COUNT_TYPE          ((GPN_STAT_T_PWE3_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_PWE3_MEASU_TYPE          ((GPN_STAT_T_PWE3_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_PWE3_1_TYPE              (GPN_STAT_T_PWE3_COUNT_TYPE + 0x00000001)
#define GPN_STAT_D_PWE3_2_TYPE              (GPN_STAT_T_PWE3_COUNT_TYPE + 0x00000002)

/*follow stat data struct define */
typedef struct _gpnStatPWE3Data_
{
	GPN_POS_STAT_D_H32 statPWE31h32;
	GPN_POS_STAT_D_L32 statPWE31l32;
	GPN_POS_STAT_D_H32 statPWE32h32;
	GPN_POS_STAT_D_L32 statPWE32l32;
	/* 2*2 */
}gpnStatPWE3Data;

/*********************************************************************************************/
/*								MON													*/
/*********************************************************************************************/
/*ETH SFP stat type define */
#define GPN_STAT_T_ETHSFP_COUNT_TYPE        ((GPN_STAT_T_ETH_SFP_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_ETHSFP_MEASU_TYPE        ((GPN_STAT_T_ETH_SFP_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_ETHSFP_TEM_TYPE          ((GPN_STAT_T_ETHSFP_MEASU_TYPE + 0x00000001) | GPN_STAT_INTEGER_NUM_CLASS)
#define GPN_STAT_D_ETHSFP_TXPOW_TYPE        ((GPN_STAT_T_ETHSFP_MEASU_TYPE + 0x00000002) | GPN_STAT_INTEGER_NUM_CLASS)
#define GPN_STAT_D_ETHSFP_RXPOW_TYPE        ((GPN_STAT_T_ETHSFP_MEASU_TYPE + 0x00000003) | GPN_STAT_INTEGER_NUM_CLASS)
/* 2018/3/15, lipf, add for ipran */
#define GPN_STAT_D_ETHSFP_BIAS_TYPE         ((GPN_STAT_T_ETHSFP_MEASU_TYPE + 0x00000004) | GPN_STAT_INTEGER_NUM_CLASS)


/*SFP stat data struct define */
typedef struct _gpnStatEthSFPData_
{
	GPN_MON_STAT_D statSfpTem;
	GPN_MON_STAT_D statSfpTxPow;
	GPN_MON_STAT_D statSfpRxPow;

	/* 2018/3/15, lipf, add for ipran */
	GPN_MON_STAT_D statSfpBias;
	
	/* 3 */
}gpnStatEthSFPData;

/*POW stat type define */
#define GPN_STAT_T_POW_COUNT_TYPE           ((GPN_STAT_T_POW_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_POW_MEASU_TYPE           ((GPN_STAT_T_POW_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_POW_VOL_TYPE             (GPN_STAT_T_POW_MEASU_TYPE + 0x00000001)
#define GPN_STAT_D_POW_POW_TYPE             (GPN_STAT_T_POW_MEASU_TYPE + 0x00000002)

/*POW stat data struct define */
typedef struct _gpnStatPOWData_
{
	GPN_MON_STAT_D statPowVol;
	GPN_MON_STAT_D statPowPow;
	/* 2 */
}gpnStatPOWData;

/*DEV MON stat type define */
#define GPN_STAT_T_EQU_COUNT_TYPE           ((GPN_STAT_T_EQU_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_EQU_MEASU_TYPE           ((GPN_STAT_T_EQU_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_EQU_CPU_UTILIZA_TYPE		(GPN_STAT_T_EQU_MEASU_TYPE + 0x00000001)
#define GPN_STAT_D_EQU_TEM_TYPE     		((GPN_STAT_T_EQU_MEASU_TYPE + 0x00000002) | GPN_STAT_INTEGER_NUM_CLASS)


/*ETH MAC stat data struct define */
typedef struct _gpnStatEquData_
{
	GPN_MON_STAT_D statEquCpuUtileza;
	GPN_MON_STAT_D statEquTemp;
	/* 2*1 */
}gpnStatEquData;

/*PTN VS MON stat data struct define */
#define GPN_STAT_T_PTN_VS_M_COUNT_TYPE      ((GPN_STAT_T_PTN_VS_M_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_PTN_VS_M_MEASU_TYPE      ((GPN_STAT_T_PTN_VS_M_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_PTN_VS_DROP_RATA_TYPE    (GPN_STAT_T_PTN_VS_M_MEASU_TYPE + 0x00000001)
#define GPN_STAT_D_PTN_VS_DELAY_TYPE        (GPN_STAT_T_PTN_VS_M_MEASU_TYPE + 0x00000002)
#define GPN_STAT_D_PTN_VS_DELAY_CHG_TYPE    (GPN_STAT_T_PTN_VS_M_MEASU_TYPE + 0x00000003)
#define GPN_STAT_D_PTN_VS_R_DROP_RATA_TYPE  (GPN_STAT_T_PTN_VS_M_MEASU_TYPE + 0x00000004)

/*PTN VS stat data struct define */
typedef struct _gpnStatPtnVsMData_
{
	GPN_MON_STAT_D statPtnVsMDROPRATA;
	GPN_MON_STAT_D statPtnVsMDELAY;
	GPN_MON_STAT_D statPtnVsMDELAYCHG;
	GPN_MON_STAT_D statPtnVsMDRROPRATA;
	/* 4*1 */
}gpnStatPtnVsMData;

/*PTN LSP MON stat data struct define */
#define GPN_STAT_T_PTN_LSP_M_COUNT_TYPE     	((GPN_STAT_T_PTN_LSP_M_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_PTN_LSP_M_MEASU_TYPE     	((GPN_STAT_T_PTN_LSP_M_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_PTN_LSP_DROP_RATA_TYPE   	(GPN_STAT_T_PTN_LSP_M_MEASU_TYPE + 0x00000001)
#define GPN_STAT_D_PTN_LSP_DELAY_TYPE       	(GPN_STAT_T_PTN_LSP_M_MEASU_TYPE + 0x00000002)
#define GPN_STAT_D_PTN_LSP_DELAY_CHG_TYPE   	(GPN_STAT_T_PTN_LSP_M_MEASU_TYPE + 0x00000003)
#define GPN_STAT_D_PTN_REV_LSP_DROP_RATA_TYPE   (GPN_STAT_T_PTN_LSP_M_MEASU_TYPE + 0x00000004)
#define GPN_STAT_D_PTN_REV_LSP_DELAY_TYPE       (GPN_STAT_T_PTN_LSP_M_MEASU_TYPE + 0x00000005)
#define GPN_STAT_D_PTN_REV_LSP_DELAY_CHG_TYPE   (GPN_STAT_T_PTN_LSP_M_MEASU_TYPE + 0x00000006)
#define GPN_STAT_D_PTN_LSP_R_DROP_RATA_TYPE 	(GPN_STAT_T_PTN_LSP_M_MEASU_TYPE + 0x00000007)
#define GPN_STAT_D_PTN_REV_LSP_R_DROP_RATA_TYPE (GPN_STAT_T_PTN_LSP_M_MEASU_TYPE + 0x00000008)

/*PTN LSP stat data struct define */
typedef struct _gpnStatPtnLspMData_
{
	GPN_MON_STAT_D statPtnLspdLDROPRATA;
	GPN_MON_STAT_D statPtnLspdDELAY;
	GPN_MON_STAT_D statPtnLspdDELAYCHG;
	GPN_MON_STAT_D statPtnRevLspdLDROPRATA;
	GPN_MON_STAT_D statPtnRevLspdDELAY;
	GPN_MON_STAT_D statPtnRevLspdDELAYCHG;
	GPN_MON_STAT_D statPtnLspdRDROPRATA;
	GPN_MON_STAT_D statPtnRevLspdRDROPRATA;
	/* 8*1 */
}gpnStatPtnLspMData;

/*PTN PW MON stat type define */
#define GPN_STAT_T_PTN_PW_M_COUNT_TYPE      ((GPN_STAT_T_PTN_PW_M_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_PTN_PW_M_MEASU_TYPE      ((GPN_STAT_T_PTN_PW_M_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_PTN_PW_DROP_RATA_TYPE    (GPN_STAT_T_PTN_PW_M_MEASU_TYPE + 0x00000001)
#define GPN_STAT_D_PTN_PW_DELAY_TYPE        (GPN_STAT_T_PTN_PW_M_MEASU_TYPE + 0x00000002)
#define GPN_STAT_D_PTN_PW_DELAY_CHG_TYPE    (GPN_STAT_T_PTN_PW_M_MEASU_TYPE + 0x00000003)
#define GPN_STAT_D_PTN_PW_R_DROP_RATA_TYPE  (GPN_STAT_T_PTN_PW_M_MEASU_TYPE + 0x00000004)
#define GPN_STAT_D_PTN_PW_BAND_W_RATA_TYPE  (GPN_STAT_T_PTN_PW_M_MEASU_TYPE + 0x00000005)   //add for CMCC, 2019/3/7

/*PTNPW stat data struct define */
typedef struct _gpnStatPtnPwMData_
{
	GPN_MON_STAT_D statPtnPwLDROPRATA;
	GPN_MON_STAT_D statPtnPwDELAY;
	GPN_MON_STAT_D statPtnPwDELAYCHG;
	GPN_MON_STAT_D statPtnPwRDROPRATA;

    GPN_MON_STAT_D statPtnPwBWRATA;
	/* 5*1 */
}gpnStatPtnPwMData;

/*PTN VPLS PW MON stat type define */
#define GPN_STAT_T_PTN_VPLSPW_M_COUNT_TYPE      ((GPN_STAT_T_PTN_VPLSPW_M_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_PTN_VPLSPW_M_MEASU_TYPE      ((GPN_STAT_T_PTN_VPLSPW_M_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_PTN_VPLSPW_DROP_RATA_TYPE    (GPN_STAT_T_PTN_VPLSPW_M_MEASU_TYPE + 0x00000001)
#define GPN_STAT_D_PTN_VPLSPW_DELAY_TYPE        (GPN_STAT_T_PTN_VPLSPW_M_MEASU_TYPE + 0x00000002)
#define GPN_STAT_D_PTN_VPLSPW_DELAY_CHG_TYPE    (GPN_STAT_T_PTN_VPLSPW_M_MEASU_TYPE + 0x00000003)
#define GPN_STAT_D_PTN_VPLSPW_R_DROP_RATA_TYPE    (GPN_STAT_T_PTN_VPLSPW_M_MEASU_TYPE + 0x00000004)

/*PTN VPLS PW stat data struct define */
typedef struct _gpnStatPtnVplsPwMData_
{
	GPN_MON_STAT_D statPtnVplsPwLDROPRATA;
	GPN_MON_STAT_D statPtnVplsPwDELAY;
	GPN_MON_STAT_D statPtnVplsPwDELAYCHG;
	GPN_MON_STAT_D statPtnVplsPwRDROPRATA;
	/* 4*1 */
}gpnStatPtnVplsPwMData;


/*ETH MAC MON stat type define */
#define GPN_STAT_T_ETH_MAC_M_COUNT_TYPE     ((GPN_STAT_T_ETH_MAC_M_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_ETH_MAC_M_MEASU_TYPE     ((GPN_STAT_T_ETH_MAC_M_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_ETH_MAC_DROP_RATA_TYPE   	(GPN_STAT_T_ETH_MAC_M_MEASU_TYPE + 0x00000001)
#define GPN_STAT_D_ETH_MAC_DELAY_TYPE       	(GPN_STAT_T_ETH_MAC_M_MEASU_TYPE + 0x00000002)
#define GPN_STAT_D_ETH_MAC_DELAY_CHG_TYPE   	(GPN_STAT_T_ETH_MAC_M_MEASU_TYPE + 0x00000003)
#define GPN_STAT_D_ETH_MAC_R_DROP_RATA_TYPE 	(GPN_STAT_T_ETH_MAC_M_MEASU_TYPE + 0x00000004)
#define GPN_STAT_D_ETH_MAC_RX_BAND_W_RATA_TYPE 	(GPN_STAT_T_ETH_MAC_M_MEASU_TYPE + 0x00000005)
#define GPN_STAT_D_ETH_MAC_TX_BAND_W_RATA_TYPE 	(GPN_STAT_T_ETH_MAC_M_MEASU_TYPE + 0x00000006)


/*ETH MAC stat data struct define */
typedef struct _gpnStatEthMacMData_
{
	GPN_MON_STAT_D statEthMacLDROPRATA;
	GPN_MON_STAT_D statEthMacDELAY;
	GPN_MON_STAT_D statEthMacDELAYCHG;
	GPN_MON_STAT_D statEthMacRDROPRATA;

	GPN_MON_STAT_D statEthMacRxBWRATA;
	GPN_MON_STAT_D statEthMacTxBWRATA;
	/* 6*1 */
}gpnStatEthMacMData;

/*SDH SFP stat type define */
#define GPN_STAT_T_SDHSFP_COUNT_TYPE        ((GPN_STAT_T_SDH_SFP_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_SDHSFP_MEASU_TYPE        ((GPN_STAT_T_SDH_SFP_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_SDHSFP_TEM_TYPE        	((GPN_STAT_T_SDHSFP_MEASU_TYPE + 0x00000001) | GPN_STAT_INTEGER_NUM_CLASS)
#define GPN_STAT_D_SDHSFP_TXPOW_TYPE       	((GPN_STAT_T_SDHSFP_MEASU_TYPE + 0x00000002) | GPN_STAT_INTEGER_NUM_CLASS)
#define GPN_STAT_D_SDHSFP_RXPOW_TYPE        ((GPN_STAT_T_SDHSFP_MEASU_TYPE + 0x00000003) | GPN_STAT_INTEGER_NUM_CLASS)
/* 2018/3/15, lipf, add for ipran */
#define GPN_STAT_D_SDHSFP_BIAS_TYPE         ((GPN_STAT_T_SDHSFP_MEASU_TYPE + 0x00000004) | GPN_STAT_INTEGER_NUM_CLASS)


/*SDH SFP stat data struct define */
typedef struct _gpnStatSdhSfpData_
{
	GPN_MON_STAT_D statSfpTem;
	GPN_MON_STAT_D statSfpTxPow;
	GPN_MON_STAT_D statSfpRxPow;

	/* 2018/3/15, lipf, add for ipran */
	GPN_MON_STAT_D statSfpBias;
	
	/* 3 */
}gpnStatSdhSfpData;

/*PDH SFP stat type define */
#define GPN_STAT_T_PDHSFP_COUNT_TYPE        ((GPN_STAT_T_PDH_SFP_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_PDHSFP_MEASU_TYPE        ((GPN_STAT_T_PDH_SFP_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_PDHSFP_TEM_TYPE        	((GPN_STAT_T_PDHSFP_MEASU_TYPE + 0x00000001) | GPN_STAT_INTEGER_NUM_CLASS)
#define GPN_STAT_D_PDHSFP_TXPOW_TYPE        ((GPN_STAT_T_PDHSFP_MEASU_TYPE + 0x00000002) | GPN_STAT_INTEGER_NUM_CLASS)
#define GPN_STAT_D_PDHSFP_RXPOW_TYPE        ((GPN_STAT_T_PDHSFP_MEASU_TYPE + 0x00000003) | GPN_STAT_INTEGER_NUM_CLASS)
/* 2018/3/15, lipf, add for ipran */
#define GPN_STAT_D_PDHSFP_BIAS_TYPE         ((GPN_STAT_T_PDHSFP_MEASU_TYPE + 0x00000004) | GPN_STAT_INTEGER_NUM_CLASS)

/*PDH SFP stat data struct define */
typedef struct _gpnStatPdhSfpData_
{
	GPN_MON_STAT_D statSfpTem;
	GPN_MON_STAT_D statSfpTxPow;
	GPN_MON_STAT_D statSfpRxPow;

	/* 2018/3/15, lipf, add for ipran */
	GPN_MON_STAT_D statSfpBias;
	
	/* 3*1 */
}gpnStatPdhSfpData;


/*ENV MON stat type define */
#define GPN_STAT_T_ENV_COUNT_TYPE           ((GPN_STAT_T_ENV_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_ENV_MEASU_TYPE           ((GPN_STAT_T_ENV_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_ENV_HUM_TYPE             (GPN_STAT_T_ENV_MEASU_TYPE + 0x00000001)
#define GPN_STAT_D_ENV_TEM_TYPE             ((GPN_STAT_T_ENV_MEASU_TYPE + 0x00000002) | GPN_STAT_INTEGER_NUM_CLASS)


/*ENV stat data struct define */
typedef struct _gpnStatEnvData_
{
	GPN_MON_STAT_D statEnvHumidity;
	GPN_MON_STAT_D statEnvTemp;
	/* 2*1 */
}gpnStatEnvData;

/*SOFT MON type define */
#define GPN_STAT_T_SOFT_COUNT_TYPE          ((GPN_STAT_T_SOFT_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_SOFT_MEASU_TYPE          ((GPN_STAT_T_SOFT_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_CPU_UTILIZA_TYPE         (GPN_STAT_T_SOFT_MEASU_TYPE + 0x00000001)
#define GPN_STAT_D_MEM_UTILIZA_TYPE         (GPN_STAT_T_SOFT_MEASU_TYPE + 0x00000002)


/*SOFT stat data struct define */
typedef struct _gpnStatSoftData_
{
	GPN_MON_STAT_D statSoftCpuUtileza;
	GPN_MON_STAT_D statSoftMemUtileza;
	/* 2*1 */
}gpnStatSoftData;

/*FOLLOW MON type define */
#define GPN_STAT_T_FOLLOW_M_COUNT_TYPE      ((GPN_STAT_T_FOLLOW_M_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_FOLLOW_M_MEASU_TYPE      ((GPN_STAT_T_FOLLOW_M_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_FOLLOW_DROP_RATA_TYPE    (GPN_STAT_T_FOLLOW_M_MEASU_TYPE + 0x00000001)
#define GPN_STAT_D_FOLLOW_DELAY_TYPE        (GPN_STAT_T_FOLLOW_M_MEASU_TYPE + 0x00000002)
#define GPN_STAT_D_FOLLOW_DELAY_CHG_TYPE    (GPN_STAT_T_FOLLOW_M_MEASU_TYPE + 0x00000003)

/*follow stat data struct define */
typedef struct _gpnStatFollowMData_
{
	GPN_POS_STAT_D_H32 statFollowDROPRATA;
	GPN_POS_STAT_D_H32 statFollowDELAY;
	GPN_POS_STAT_D_H32 statFollowDELAYCHG;
	/* 3*1 */
}gpnStatFollowMData;

/*PTN VLAN Packet stat type define */
#define GPN_STAT_T_PTN_V_UNI_P_COUNT_TYPE    ((GPN_STAT_T_PTN_V_UNI_P_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_PTN_V_UNI_P_MEASU_TYPE    ((GPN_STAT_T_PTN_V_UNI_P_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_PTN_V_UNI_TXF_TYPE        (GPN_STAT_T_PTN_V_UNI_P_COUNT_TYPE + 0x00000001)
#define GPN_STAT_D_PTN_V_UNI_TXB_TYPE        (GPN_STAT_T_PTN_V_UNI_P_COUNT_TYPE + 0x00000002)
#define GPN_STAT_D_PTN_V_UNI_RXF_TYPE        (GPN_STAT_T_PTN_V_UNI_P_COUNT_TYPE + 0x00000003)
#define GPN_STAT_D_PTN_V_UNI_RXB_TYPE        (GPN_STAT_T_PTN_V_UNI_P_COUNT_TYPE + 0x00000004)

/*PTN VLAN Packet stat data struct define */
typedef struct _gpnStatPtnVUniPData_
{
	GPN_POS_STAT_D_H32 statPtnVUniTXFh32;
	GPN_POS_STAT_D_L32 statPtnVUniTXFl32;
	GPN_POS_STAT_D_H32 statPtnVUniTXBh32;
	GPN_POS_STAT_D_L32 statPtnVUniTXBl32;
	GPN_POS_STAT_D_H32 statPtnVUniRXFh32;
	GPN_POS_STAT_D_L32 statPtnVUniRXFl32;
	GPN_POS_STAT_D_H32 statPtnVUniRXBh32;
	GPN_POS_STAT_D_L32 statPtnVUniRXBl32;
	/* 4*2 */
}gpnStatPtnVUniPData;

/*MEP OAM MON stat type define */
#define GPN_STAT_T_PTN_MEP_M_COUNT_TYPE     ((GPN_STAT_T_PTN_MEP_M_TYPE) | GPN_STAT_COUNTER_CLASS)
#define GPN_STAT_T_PTN_MEP_M_MEASU_TYPE     ((GPN_STAT_T_PTN_MEP_M_TYPE) | GPN_STAT_MEASURE_CLASS)

#define GPN_STAT_D_PTN_MEP_DROP_RATA_TYPE 	(GPN_STAT_T_PTN_MEP_M_MEASU_TYPE + 0x00000001)
#define GPN_STAT_D_PTN_MEP_DELAY_TYPE       (GPN_STAT_T_PTN_MEP_M_MEASU_TYPE + 0x00000002)
#define GPN_STAT_D_PTN_MEP_DELAY_CHG_TYPE   (GPN_STAT_T_PTN_MEP_M_MEASU_TYPE + 0x00000003)
#define GPN_STAT_D_PTN_MEP_R_DROP_RATA_TYPE (GPN_STAT_T_PTN_MEP_M_MEASU_TYPE + 0x00000004)

/*MEP OAM stat data struct define */
typedef struct _gpnStatPtnMepMData_
{
	GPN_MON_STAT_D statPtnMepLDROPRATA;	
	GPN_MON_STAT_D statPtnMepDELAY;
	GPN_MON_STAT_D statPtnMepDELAYCHG;
    GPN_MON_STAT_D statPtnMepRDROPRATA;
	/* 4*1 */
}gpnStatPtnMepMData;

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_STAT_TYPE_DEF_H_*/

