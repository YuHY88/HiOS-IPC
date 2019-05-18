/**********************************************************
* file name: gpnGlobalPortIndexDef.h
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-09-09
* function: 
*    define global port index define details
* modify:
*
***********************************************************/
#ifndef _GPN_GLOBAL_PORT_INDEX_DEF_H_
#define _GPN_GLOBAL_PORT_INDEX_DEF_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

//#include "lib/ifm_common.h"


#include "lib/gpnSocket/socketComm/gpnSockTypeDef.h"

#define GPN_PINDEX_GEN_OK          GPN_SOCK_SYS_OK
#define GPN_PINDEX_GEN_ERR         GPN_SOCK_SYS_ERR

#define GPN_PINDEX_GEN_YES         GPN_SOCK_SYS_OK
#define GPN_PINDEX_GEN_NO          GPN_SOCK_SYS_ERR

#define IFM_INDEX_SLOT_ID_MASK     0x0000001F
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++             SLOT MACRO DEFINE               ++++++++++++++++++*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#define IFM_SLOT_MAX_ID            IFM_INDEX_SLOT_ID_MASK


/*should not be here, should in Makefile !!!*/
//#define BIG_ENG_STYLE

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++             define device index                   ++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
typedef UINT32 HH_DEVICE_INDEX;
#define GPN_DEVICE_INDEX_NULL       0x00000000
#define GPN_ILLEGAL_DEVICE_INDEX    0xFFFFFFFF


/* modify by lipf for ipran, 2018/4/20 */
#if 0
typedef union
{
	HH_DEVICE_INDEX devIndex;		
	struct
	{
		#ifdef BIG_ENG_STYLE
		UINT32 slotNo:5;      	/* 设备所在槽位号(32) */
		UINT32 level:3;       	/* 远端设备级别，局端为1级，局端板卡下挂设备为2级，... */
		UINT32 index_2nd:8;  	/* 二级远端索引号(256) */
		UINT32 index_3rd:4;  	/* 三级远端索引号(16) */
		UINT32 devType:12;  		/*device type, 0~4095 */
		#else
		UINT32 devType:12;  		/* device type, 0~4095 */
		UINT32 index_3rd:4;  	/* 三级远端索引号(16) */
		UINT32 index_2nd:8;  	/* 二级远端索引号(256) */
		UINT32 level:3;       	/* 远端设备级别，局端为1级，局端板卡下挂设备为2级，... */
		UINT32 slotNo:5;      	/* 设备所在槽位号(32) */
		#endif
	}field;
}DEV_INDEX;

#if defined(BIG_ENG_STYLE)
#define IFM_DEV_INDEX_SLOT_MASK     IFM_INDEX_SLOT_ID_MASK
#define IFM_DEV_INDEX_SLOT_OFFSET   27

#define IFM_DEV_INDEX_LEVEL_MASK    0x00000007
#define IFM_DEV_INDEX_LEVEL_OFFSET  24

#define IFM_DEV_INDEX_TYPE_MASK     0x00000FFF
#define IFM_DEV_INDEX_TYPE_OFFSET   0

#else
//error;
#define IFM_DEV_INDEX_SLOT_MASK     IFM_INDEX_SLOT_ID_MASK
#define IFM_DEV_INDEX_SLOT_OFFSET   0

#define IFM_DEV_INDEX_LEVEL_MASK    0x00000007
#define IFM_DEV_INDEX_LEVEL_OFFSET  5

#define IFM_DEV_INDEX_TYPE_MASK     0x00000FFF
#define IFM_DEV_INDEX_TYPE_OFFSET   20

#endif

/* define device position level */
#define IFM_DEV_LEVEL_CENTER        1
#define IFM_DEV_LEVEL_NERREM        2
#define IFM_DEV_LEVEL_FAREDN        3

/* device index option */
#define IFM_DEV_SLOT_DECOM(dev_index) (((dev_index)>>IFM_DEV_INDEX_SLOT_OFFSET)&IFM_DEV_INDEX_SLOT_MASK)
#define IFM_DEV_LEVEL_DECOM(dev_index) (((dev_index)>>IFM_DEV_INDEX_LEVEL_OFFSET)&IFM_DEV_INDEX_LEVEL_MASK)
#define IFM_DEV_TYPE_DECOM(dev_index) (((dev_index)>>IFM_DEV_INDEX_TYPE_OFFSET)&IFM_DEV_INDEX_TYPE_MASK)

#define IFM_DEV_INDEX_COMP(level,slot, dev_type) ((((slot)&IFM_DEV_INDEX_SLOT_MASK)<<IFM_DEV_INDEX_SLOT_OFFSET) |\
                                                  (((level)&IFM_DEV_INDEX_LEVEL_MASK)<<IFM_DEV_INDEX_LEVEL_OFFSET) |\
                                                  (((dev_type)&IFM_DEV_INDEX_TYPE_MASK)<<IFM_DEV_INDEX_TYPE_OFFSET))

#define IFM_DEV_CHANG_SLOT(dev_index, new_slot) \
	((dev_index) =\
	((dev_index) & (~(IFM_DEV_INDEX_SLOT_MASK << IFM_DEV_INDEX_SLOT_OFFSET))) |\
	(((new_slot) & IFM_DEV_INDEX_SLOT_MASK) << IFM_DEV_INDEX_SLOT_OFFSET))
#define IFM_LEVEL_CHANG_ID(dev_index, new_level) \
	((dev_index) =\
	((dev_index) & (~(IFM_DEV_INDEX_LEVEL_MASK << IFM_DEV_INDEX_LEVEL_OFFSET))) |\
	(((new_level) & IFM_DEV_INDEX_LEVEL_MASK) << IFM_DEV_INDEX_LEVEL_OFFSET))
#define IFM_DEV_CHANG_TYPE(dev_index, new_type) \
	((dev_index) =\
	((dev_index) & (~(IFM_DEV_INDEX_TYPE_MASK << IFM_DEV_INDEX_TYPE_OFFSET))) |\
	(((new_type) & IFM_DEV_INDEX_TYPE_MASK) << IFM_DEV_INDEX_TYPE_OFFSET))
#endif



typedef union
{
	HH_DEVICE_INDEX devIndex;		
	struct
	{
		#ifdef BIG_ENG_STYLE
		UINT32 rsv:29;
		UINT32 unit:3;  	/* unit number */
		#else
		UINT32 unit:3;  	/* unit number */
		UINT32 rsv:29;
		#endif
	}field;
}DEV_INDEX;


#define IFM_INDEX_UNIT_ID_MASK			0x07

#define IFM_DEV_INDEX_UNIT_MASK     	IFM_INDEX_UNIT_ID_MASK
#define IFM_DEV_INDEX_UNIT_OFFSET   	0


#define IFM_DEV_UNIT_GET(dev_index) 	(((dev_index)>>IFM_DEV_INDEX_UNIT_OFFSET)&IFM_DEV_INDEX_UNIT_MASK)


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++    PORT TYPE DEFINE     ++++++++++++++++++*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*temp define for FUN_ETH*/
#define IFM_ALL_TYPE				0
#define IFM_FUN_ETH_TYPE			1
#define IFM_SOFT_TYPE				2
#define IFM_STMN_TYPE				3
#define IFM_VC4_TYPE				4
#define IFM_VC3_TYPE				5
#define IFM_VC12_TYPE				6
#define IFM_VCG_TYPE				7
#define IFM_VCG_LP_TYPE				8
#define IFM_E1_TYPE					9
#define IFM_E2_TYPE					10
#define IFM_E3_TYPE					11
#define IFM_V35_TYPE				12
#define IFM_DSL_TYPE				13
#define IFM_ETH_TYPE				14
#define IFM_VS_TYPE					15
#define IFM_LSP_TYPE				16
#define IFM_PW_TYPE					17
#define IFM_MPLS_TYPE				18
#define IFM_VLAN_OAM_TYPE			19
#define IFM_POWN48_TYPE				20
#define IFM_EQU_TYPE				21
#define IFM_ENV_TYPE				22
#define IFM_FAN_TYPE				23
#define IFM_PWE3E1_TYPE				24
#define IFM_CFM_MEP_TYPE			25
#define IFM_POW220_TYPE				26
#define IFM_PWE3STMN_TYPE			27
#define IFM_ETH_CLC_IN_TYPE			28
#define IFM_ETH_CLC_OUT_TYPE		29
#define IFM_ETH_CLC_SUB_TYPE		30
#define IFM_ETH_CLC_LINE_TYPE		31
#define IFM_ETH_SFP_TYPE			32
#define IFM_VPLSPW_TYPE				33
#define IFM_VUNI_TYPE				34

/********************** lipf add for ipran ***********************/
#define IFM_FUN_ETH_SUB_TYPE		35
#define IFM_TUNNEL_TYPE				36
#define IFM_E1_SUB_TYPE				37
#define IFM_BFD_TYPE				38
#define IFM_TRUNK_TYPE				39
#define IFM_TRUNK_SUBIF_TYPE		40
#define IFM_VS_2_TYPE				41
#define IFM_LSP_2_TYPE				42
#define IFM_PW_2_TYPE				43


/****************************** end ******************************/

#define IFM_NULL_TYPE	 			IFM_PW_2_TYPE + 1

/*#define IFM_NULL_TYPE				0x7F*//*127*/

#define IFM_PORT_TYPE_MAX			(0x7E+1)/*just 0x7F*/

#define IFM_PORT_TYPE_INVALID		0xFFFFFFFF/*(-1)*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++            PORT TYPE DEFINE  END           ++++++++++++++++++*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
typedef UINT32 HH_PORT_INDEX;
#define GPN_PORT_INDEX_NULL			0x00000000
#define GPN_ILLEGAL_PORT_INDEX		0xFFFFFFFF
//#define GPN_ILLEGAL_PORT_INDEX		0


typedef union
{
	HH_PORT_INDEX portIndex;		
	struct
	{
		#ifdef BIG_ENG_STYLE
		UINT32 slotNo:5;
		UINT32 ifType:7;
		UINT32 info:12;
		UINT32 portNo:8;
		#else
		UINT32 portNo:8;
		UINT32 info:12;	
		UINT32 ifType:7;
		UINT32 slotNo:5;
		#endif
		
	}field;
}PORT_INDEX;

//#if defined(BIG_ENG_STYLE)
#define IFM_PORT_INDEX_SLOT_MASK    IFM_INDEX_SLOT_ID_MASK
#define IFM_PORT_INDEX_SLOT_OFFSET  27

#define IFM_PORT_INDEX_TYPE_MASK    0x0000007F
#define IFM_PORT_INDEX_TYPE_OFFSET  20

#define IFM_PORT_INDEX_PID_MASK     0x000000FF
#define IFM_PORT_INDEX_PID_OFFSET   0

#define IFM_PORT_INDEX_LAB_MASK     0x000FFFFF
#define IFM_PORT_INDEX_LAB_OFFSET   0

#if 0
//error;
#define IFM_PORT_INDEX_SLOT_MASK    IFM_INDEX_SLOT_ID_MASK
#define IFM_PORT_INDEX_SLOT_OFFSET  0

#define IFM_PORT_INDEX_TYPE_MASK    0x0000007F
#define IFM_PORT_INDEX_TYPE_OFFSET  5

#define IFM_PORT_INDEX_PID_MASK     0x000000FF
#define IFM_PORT_INDEX_PID_OFFSET   24

#define IFM_PORT_INDEX_LAB_MASK     0x000FFFFF
#define IFM_PORT_INDEX_LAB_OFFSET   12


#endif

/* port index option */
#define IFM_PORT_SLOT_DECOM(port_index) (((port_index) >> IFM_PORT_INDEX_SLOT_OFFSET) & IFM_PORT_INDEX_SLOT_MASK)
#define IFM_PORT_TYPE_DECOM(port_index) (((port_index) >> IFM_PORT_INDEX_TYPE_OFFSET) & IFM_PORT_INDEX_TYPE_MASK)
#define IFM_PORT_ID_DECOM(port_index) (((port_index) >> IFM_PORT_INDEX_PID_OFFSET) & IFM_PORT_INDEX_PID_MASK)

#define IFM_PORT_ALT_SLOT(port_index, new_slot) \
	(((port_index) & (~(IFM_PORT_INDEX_SLOT_MASK << IFM_PORT_INDEX_SLOT_OFFSET))) |\
	(((new_slot) & IFM_PORT_INDEX_SLOT_MASK) << IFM_PORT_INDEX_SLOT_OFFSET))
#define IFM_PORT_ALT_TYPE(port_index, new_type) \
	(((port_index) & (~(IFM_PORT_INDEX_TYPE_MASK << IFM_PORT_INDEX_TYPE_OFFSET))) |\
	(((new_type) & IFM_PORT_INDEX_TYPE_MASK) << IFM_PORT_INDEX_TYPE_OFFSET))
#define IFM_PORT_ALT_ID(port_index, new_id) \
	(((port_index) & (~(IFM_PORT_INDEX_PID_MASK << IFM_PORT_INDEX_PID_OFFSET))) |\
	(((new_id) & IFM_PORT_INDEX_PID_MASK) << IFM_PORT_INDEX_PID_OFFSET))

#define IFM_PORT_CHANG_SLOT(port_index, new_slot) \
	((port_index) =\
	((port_index) & (~(IFM_PORT_INDEX_SLOT_MASK << IFM_PORT_INDEX_SLOT_OFFSET))) |\
	(((new_slot) & IFM_PORT_INDEX_SLOT_MASK) << IFM_PORT_INDEX_SLOT_OFFSET))
#define IFM_PORT_CHANG_TYPE(port_index, new_type) \
	((port_index) =\
	((port_index) & (~(IFM_PORT_INDEX_TYPE_MASK << IFM_PORT_INDEX_TYPE_OFFSET))) |\
	(((new_type) & IFM_PORT_INDEX_TYPE_MASK) << IFM_PORT_INDEX_TYPE_OFFSET))
#define IFM_PORT_CHANG_ID(port_index, new_id) \
	((port_index) =\
	((port_index) & (~(IFM_PORT_INDEX_PID_MASK << IFM_PORT_INDEX_PID_OFFSET))) |\
	(((new_id) & IFM_PORT_INDEX_PID_MASK) << IFM_PORT_INDEX_PID_OFFSET))

/*SDH_PORT_INDEX操作宏*/
#define IFM_SDH_SLOT_DECOM(prot_index)			IFM_PORT_SLOT_DECOM(prot_index)
#define IFM_SDH_TYPE_DECOM(prot_index)			IFM_PORT_TYPE_DECOM(prot_index)
#define IFM_SDH_PORTID_DECOM(prot_index)		IFM_PORT_ID_DECOM(prot_index)
#define IFM_SDH_HP_DECOM(prot_index)			(((prot_index)>>12)&0x0000003F)
#define IFM_SDH_LP_DECOM(prot_index)			(((prot_index)>>8)&0x0000003F)

#define IFM_SDH_PID_COMP(pType, slot, portNo, hp, lp) \
	((((pType)&IFM_PORT_INDEX_TYPE_MASK)<<IFM_PORT_INDEX_TYPE_OFFSET) |\
	(((slot)&IFM_PORT_INDEX_SLOT_MASK)<<IFM_PORT_INDEX_SLOT_OFFSET) |\
	(((hp)&0x0000003F)<<12) |\
	(((lp)&0x0000003F)<<8) |\
	(((portNo)&IFM_PORT_INDEX_PID_MASK)<<IFM_PORT_INDEX_PID_OFFSET))

typedef union
{
	HH_PORT_INDEX portIndex;		
	struct
	{
		#ifdef BIG_ENG_STYLE
		UINT32 slotNo:5;
		UINT32 ifType:7;
		UINT32 hp:6;
		UINT32 lp:6;
		UINT32 portNo:8;
		#else
		UINT32 portNo:8;
		UINT32 lp:6;
		UINT32 hp:6;
		UINT32 ifType:7;
		UINT32 slotNo:5;
		#endif
	}field;
}SDH_PORT_INDEX;

/*VCG_PORT_INDEX操作宏*/
#define IFM_VCG_SLOT_DECOM(prot_index)			IFM_PORT_SLOT_DECOM(prot_index)
#define IFM_VCG_TYPE_DECOM(prot_index)			IFM_PORT_TYPE_DECOM(prot_index)
#define IFM_VCG_PORTID_DECOM(prot_index)		IFM_PORT_ID_DECOM(prot_index)
#define IFM_VCG_LP_DECOM(prot_index)			(((prot_index)>>8)&0x0000003F)

#define IFM_VCG_PID_COMP(pType, slot, portNo, lp) \
	((((pType)&IFM_PORT_INDEX_TYPE_MASK)<<IFM_PORT_INDEX_TYPE_OFFSET) |\
	(((slot)&IFM_PORT_INDEX_SLOT_MASK)<<IFM_PORT_INDEX_SLOT_OFFSET)|\
	(((lp)&0x0000003F)<<8) |\
	(((portNo)&IFM_PORT_INDEX_PID_MASK)<<IFM_PORT_INDEX_PID_OFFSET))

typedef union
{
	HH_PORT_INDEX portIndex;		
	struct
	{
		#ifdef BIG_ENG_STYLE
		UINT32 slotNo:5;
		UINT32 ifType:7;
		UINT32 rsv:6;
		UINT32 lp:6;
		UINT32 portNo:8;
		#else
		UINT32 portNo:8;
		UINT32 lp:6;
		UINT32 rsv:6;
		UINT32 ifType:7;
		UINT32 slotNo:5;
		#endif
	}field;
}VCG_PORT_INDEX;

/*LAB_PORT_INDEX操作宏*/
#define IFM_LAB_SLOT_DECOM(prot_index)			IFM_PORT_SLOT_DECOM(prot_index)
#define IFM_LAB_TYPE_DECOM(prot_index)			IFM_PORT_TYPE_DECOM(prot_index)
#define IFM_LAB_LAB_DECOM(prot_index)			((prot_index)&0x000FFFFF)

#define IFM_LAB_PID_COMP(pType, slot, lable) \
	((((pType)&IFM_PORT_INDEX_TYPE_MASK)<<IFM_PORT_INDEX_TYPE_OFFSET) |\
	(((slot)&IFM_PORT_INDEX_SLOT_MASK)<<IFM_PORT_INDEX_SLOT_OFFSET) |\
	((lable)&0x000FFFFF))
typedef union
{
	HH_PORT_INDEX portIndex;		
	struct
	{
		#ifdef BIG_ENG_STYLE
		UINT32 slotNo:5;
		UINT32 ifType:7;
		UINT32 lable:20;
		#else
		UINT32 lable:20;
		UINT32 ifType:7;
		UINT32 slotNo:5;
		#endif
	}field;
}LAB_PORT_INDEX;

/*VLAN_PORT_INDEX操作宏*/
#define IFM_VLAN_SLOT_DECOM(prot_index)			IFM_PORT_SLOT_DECOM(prot_index)
#define IFM_VLAN_TYPE_DECOM(prot_index)			IFM_PORT_TYPE_DECOM(prot_index)
#define IFM_VLAN_PORTID_DECOM(prot_index)		IFM_PORT_ID_DECOM(prot_index)
#define IFM_VLAN_VLAN_DECOM(prot_index)			(((prot_index)>>8)&0x00000FFF)

#define IFM_VLAN_PID_COMP(pType, slot, portNo, vlanId) \
	((((pType)&IFM_PORT_INDEX_TYPE_MASK)<<IFM_PORT_INDEX_TYPE_OFFSET) |\
	(((slot)&IFM_PORT_INDEX_SLOT_MASK)<<IFM_PORT_INDEX_SLOT_OFFSET) |\
	(((vlanId)&0x00000FFF)<<8) |\
	(((portNo)&IFM_PORT_INDEX_PID_MASK)<<IFM_PORT_INDEX_PID_OFFSET))
	
typedef union
{
	HH_PORT_INDEX portIndex;		
	struct
	{
		#ifdef BIG_ENG_STYLE
		UINT32 slotNo:5;
		UINT32 ifType:7;
		UINT32 vlan:12;
		UINT32 portNo:8;
		#else
		UINT32 portNo:8;
		UINT32 vlan:12;
		UINT32 ifType:7;
		UINT32 slotNo:5;
		#endif
	}field;
}VLAN_PORT_INDEX;

/*CFM_MEP_PORT_INDEX操作宏*/
#define IFM_CFM_MEP_SLOT_DECOM(prot_index)		IFM_PORT_SLOT_DECOM(prot_index)
#define IFM_CFM_MEP_TYPE_DECOM(prot_index)		IFM_PORT_TYPE_DECOM(prot_index)
#define IFM_CFM_MEP_MEPID_DECOM(prot_index)		(((prot_index))&0x0000FFFF)

#define IFM_CFM_MEP_PID_COMP(pType, slot, portNo, MEPId) \
	((((pType)&IFM_PORT_INDEX_TYPE_MASK)<<IFM_PORT_INDEX_TYPE_OFFSET) |\
	(((slot)&IFM_PORT_INDEX_SLOT_MASK)<<IFM_PORT_INDEX_SLOT_OFFSET) |\
	((MEPId)&0x0000FFFF))

typedef union
{
	HH_PORT_INDEX portIndex;		
	struct
	{
		#ifdef BIG_ENG_STYLE
		UINT32 slotNo:5;
		UINT32 ifType:7;
		UINT32 rev:4;
		UINT32 MEPId:16;
		#else
		UINT32 MEPId:16;
		UINT32 rev:4;
		UINT32 ifType:7;
		UINT32 slotNo:5;
		#endif
	}field;
}CFM_MEP_PORT_INDEX;

/*PDH_PORT_INDEX操作宏*/
#define IFM_PDH_SLOT_DECOM(prot_index)			IFM_PORT_SLOT_DECOM(prot_index)
#define IFM_PDH_TYPE_DECOM(prot_index)			IFM_PORT_TYPE_DECOM(prot_index)
#define IFM_PDH_PORTID_DECOM(prot_index)		IFM_PORT_ID_DECOM(prot_index)
#define IFM_PDH_LP_DECOM(prot_index)			(((prot_index)>>8)&0x0000003F)

#define IFM_PDH_PID_COMP(pType, slot, portNo, lp) \
	((((pType)&IFM_PORT_INDEX_TYPE_MASK)<<IFM_PORT_INDEX_TYPE_OFFSET) |\
	(((slot)&IFM_PORT_INDEX_SLOT_MASK)<<IFM_PORT_INDEX_SLOT_OFFSET) |\
	(((lp)&0x0000003F)<<8) |\
	(((portNo)&IFM_PORT_INDEX_PID_MASK)<<IFM_PORT_INDEX_PID_OFFSET))

typedef union
{
	HH_PORT_INDEX portIndex;		
	struct
	{
		#ifdef BIG_ENG_STYLE
		UINT32 slotNo:5;
		UINT32 ifType:7;
		UINT32 rsv:6;
		UINT32 lp:6;
		UINT32 portNo:8;
		#else
		UINT32 portNo:8;
		UINT32 lp:6;
		UINT32 rsv:6;
		UINT32 ifType:7;
		UINT32 slotNo:5;
		#endif
	}field;
}PDH_PORT_INDEX;

/*Funny_EHT_PORT_INDEX操作宏*/
#define IFM_FUN_ETH_SLOT_DECOM(prot_index)			IFM_PORT_SLOT_DECOM(prot_index)
#define IFM_FUN_ETH_TYPE_DECOM(prot_index)			IFM_PORT_TYPE_DECOM(prot_index)
#define IFM_FUN_ETH_PORTID_DECOM(prot_index)		(((prot_index))&0x0000FFFF)

#define IFM_FUN_ETH_PID_COMP(pType, slot, portNo) \
	((((pType)&IFM_PORT_INDEX_TYPE_MASK)<<IFM_PORT_INDEX_TYPE_OFFSET) |\
	(((slot)&IFM_PORT_INDEX_SLOT_MASK)<<IFM_PORT_INDEX_SLOT_OFFSET) |\
	(((portNo)&IFM_PORT_INDEX_LAB_MASK)<<IFM_PORT_INDEX_LAB_OFFSET))

typedef union
{
	HH_PORT_INDEX portIndex;		
	struct
	{
		#ifdef BIG_ENG_STYLE
		UINT32 slotNo:5;
		UINT32 ifType:7;
		UINT32 rsv:4;
		UINT32 portNo:16;
		#else
		UINT32 portNo:16;
		UINT32 rsv:4;
		UINT32 ifType:7;
		UINT32 slotNo:5;
		#endif
	}field;
}FUN_ETH_PORT_INDEX;


/*SMP_PORT_INDEX操作宏*/
#define IFM_SMP_SLOT_DECOM(prot_index)			IFM_PORT_SLOT_DECOM(prot_index)
#define IFM_SMP_TYPE_DECOM(prot_index)			IFM_PORT_TYPE_DECOM(prot_index)
#define IFM_SMP_PORTID_DECOM(prot_index)		IFM_PORT_ID_DECOM(prot_index)

#define IFM_SMP_PID_COMP(pType, slot, portNo) \
	((((pType)&IFM_PORT_INDEX_TYPE_MASK)<<IFM_PORT_INDEX_TYPE_OFFSET) |\
	(((slot)&IFM_PORT_INDEX_SLOT_MASK)<<IFM_PORT_INDEX_SLOT_OFFSET) |\
	(((portNo)&IFM_PORT_INDEX_PID_MASK)<<IFM_PORT_INDEX_PID_OFFSET))

typedef union
{
	HH_PORT_INDEX portIndex;		
	struct
	{
		#ifdef BIG_ENG_STYLE
		UINT32 slotNo:5;
		UINT32 ifType:7;
		UINT32 rsv:12;
		UINT32 portNo:8;
		#else
		UINT32 portNo:8;
		UINT32 rsv:12;
		UINT32 ifType:7;
		UINT32 slotNo:5;
		#endif
	}field;
}SMP_PORT_INDEX;

/*define how sys port index location a port*/
typedef UINT32 PORT_INFO_32;
typedef UINT32 DEV_INFO_32;

typedef struct _objLogicDesc_
{
	DEV_INFO_32 devIndex;
	PORT_INFO_32 portIndex;
	PORT_INFO_32 portIndex3;
	PORT_INFO_32 portIndex4;
	PORT_INFO_32 portIndex5;
	PORT_INFO_32 portIndex6;
}objLogicDesc;

typedef struct _objOrientation_
{
	DEV_INFO_32 devIndex;
	PORT_INFO_32 portIndex;
	PORT_INFO_32 portIndex3;
	PORT_INFO_32 portIndex4;
	PORT_INFO_32 portIndex5;
	PORT_INFO_32 portIndex6;
}optObjOrient;

//HH_DEVICE_INDEX DeviceIndex_Create( UINT32 level, UINT32 slotNo );
HH_DEVICE_INDEX DeviceIndex_Create( UINT32 unit );
UINT32 DeviceIndex_GetUnit( HH_DEVICE_INDEX deviceIndex );
UINT32 DeviceIndex_GetLevel( HH_DEVICE_INDEX deviceIndex );
UINT32 DeviceIndex_GetSlot( HH_DEVICE_INDEX deviceIndex );
HH_PORT_INDEX SDH_PortIndex_Create( UINT32 ifType, UINT32 slotNo, UINT32 portNo, UINT32 hp, UINT32 lp );
HH_PORT_INDEX LAB_PortIndex_Create( UINT32 ifType, UINT32 slotNo, UINT32 lable );
HH_PORT_INDEX VLAN_PortIndex_Create( UINT32 ifType, UINT32 slotNo, UINT32 portNo, UINT32 vlan );
HH_PORT_INDEX _VLAN_PortIndex_Create( UINT32 ifType, UINT32 slotNo, UINT32 vlan );
HH_PORT_INDEX CFM_MEP_PortIndex_Create( UINT32 ifType, UINT32 slotNo, UINT32 MEPId );
HH_PORT_INDEX VCG_PortIndex_Create( UINT32 ifType, UINT32 slotNo, UINT32 portNo, UINT32 lp );
HH_PORT_INDEX PDH_PortIndex_Create( UINT32 ifType, UINT32 slotNo, UINT32 portNo, UINT32 lp );
HH_PORT_INDEX SMP_PortIndex_Create( UINT32 ifType, UINT32 slotNo, UINT32 portNo );
UINT32 PortIndex_GetType( HH_PORT_INDEX index );
UINT32 PortIndex_GetSlot( HH_PORT_INDEX index );
UINT32 PortIndex_GetPortNo( HH_PORT_INDEX index );
UINT32 SDH_PortIndex_GetHP( HH_PORT_INDEX index );
UINT32 SDH_PortIndex_GetLP( HH_PORT_INDEX index );
UINT32 PDH_PortIndex_GetLP( HH_PORT_INDEX index );
UINT32 VCG_PortIndex_GetLP( HH_PORT_INDEX index );
UINT32 LAB_PortIndex_GetLable( HH_PORT_INDEX index );
UINT32 VLAN_PortIndex_GetVlan( HH_PORT_INDEX index );
UINT32 CFM_MEP_PortIndex_GetMEPID( HH_PORT_INDEX index );

HH_PORT_INDEX FUN_ETH_PortIndex_Create( UINT32 ifType, UINT32 slotNo, UINT32 portNo);
UINT32 FUN_ETH_PortIndex_GetPid( HH_PORT_INDEX index );

UINT32 IS_PHY_PORT( HH_PORT_INDEX index );

#define GPN_INDEX_2_STR_STD_BUFF               256
#define GPN_DEV_INDEX_2_STR_STD_BUFF           64
#define GPN_PORT_INDEX_2_STR_STD_BUFF          192
UINT32 INDEX_2_STR(optObjOrient *id_struct, char *id_string, UINT32 len);
UINT32 DEV_INDEX_2_STR(optObjOrient *id_struct, char *dev_string, UINT32 len);
UINT32 PORT_INDEX_2_STR(optObjOrient *id_struct, char *pid_string, UINT32 len);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_GLOBAL_PORT_INDEX_DEF_H_*/

