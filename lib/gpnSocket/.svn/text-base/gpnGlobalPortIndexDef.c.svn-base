/**********************************************************
* file name: gpnGlobalPortIndexDef.c
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-09-09
* function: 
*    define global port index define details
* modify:
*
***********************************************************/
#ifndef _GPN_GLOBAL_PORT_INDEX_DEF_C_
#define _GPN_GLOBAL_PORT_INDEX_DEF_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <string.h>

#include "socketComm/gpnGlobalPortIndexDef.h"
#include "devCoreInfo/gpnGlobalDevTypeDef.h"
#include "devCoreInfo/gpnDevCoreInfo.h"


#if 0
HH_DEVICE_INDEX DeviceIndex_Create( UINT32 level, UINT32 slotNo )
{
	DEV_INDEX index;

	index.devIndex = 0;
	
	index.field.level     = level;
	index.field.slotNo    = slotNo;
	index.field.index_2nd = 0;
	index.field.index_3rd = 0;

	return index.devIndex;
}

UINT32 DeviceIndex_GetLevel( HH_DEVICE_INDEX deviceIndex )
{
	DEV_INDEX index;

	index.devIndex = deviceIndex;

	return index.field.level;
}	
UINT32 DeviceIndex_GetSlot( HH_DEVICE_INDEX deviceIndex )
{
	DEV_INDEX index;

	index.devIndex = deviceIndex;

	return index.field.slotNo;
}
#endif


/* modify by lipf for ipran, 2018/4/20 */
UINT32 DeviceIndex_GetLevel( HH_DEVICE_INDEX deviceIndex )
{
	return 1;
}	
UINT32 DeviceIndex_GetSlot( HH_DEVICE_INDEX deviceIndex )
{
	return 1;
}

HH_DEVICE_INDEX DeviceIndex_Create( UINT32 unit )
{
	DEV_INDEX index;

	index.devIndex = 0;
	
	index.field.unit = unit;

	return index.devIndex;
}

UINT32 DeviceIndex_GetUnit( HH_DEVICE_INDEX deviceIndex )
{
	DEV_INDEX index;

	index.devIndex = deviceIndex;

	return index.field.unit;
}



HH_PORT_INDEX SDH_PortIndex_Create( UINT32 ifType, UINT32 slotNo, UINT32 portNo, UINT32 hp, UINT32 lp )
{
	SDH_PORT_INDEX port;

	port.portIndex = 0;
	port.field.ifType  = ifType;
	port.field.slotNo  = slotNo;
	port.field.portNo  = portNo;
	port.field.hp      = hp;
	port.field.lp      = lp;

	return port.portIndex;
}
HH_PORT_INDEX LAB_PortIndex_Create( UINT32 ifType, UINT32 slotNo, UINT32 lable )
{
	LAB_PORT_INDEX port;

	port.portIndex = 0;
	port.field.ifType  = ifType;
	port.field.slotNo  = slotNo;
	port.field.lable   = lable;

	return port.portIndex;
}
HH_PORT_INDEX VLAN_PortIndex_Create( UINT32 ifType, UINT32 slotNo, UINT32 portNo, UINT32 vlan )
{
	VLAN_PORT_INDEX port;

	port.portIndex = 0;
	port.field.ifType  = ifType;
	port.field.slotNo  = slotNo;
	//port.field.portNo  = portNo-5000;
	port.field.portNo  = portNo;
	port.field.vlan    = vlan;
	
	return port.portIndex;
}
HH_PORT_INDEX CFM_MEP_PortIndex_Create( UINT32 ifType, UINT32 slotNo, UINT32 MEPId )
{
	CFM_MEP_PORT_INDEX port;

	port.portIndex = 0;
	port.field.ifType  = ifType;
	port.field.slotNo  = slotNo;
	port.field.MEPId  = MEPId;
	
	return port.portIndex;
}
HH_PORT_INDEX VCG_PortIndex_Create( UINT32 ifType, UINT32 slotNo, UINT32 portNo, UINT32 lp )
{
	return SDH_PortIndex_Create( ifType, slotNo, portNo, 0, lp );
}
HH_PORT_INDEX PDH_PortIndex_Create( UINT32 ifType, UINT32 slotNo, UINT32 portNo, UINT32 lp )
{
	return SDH_PortIndex_Create( ifType, slotNo, portNo, 0, lp );
}
HH_PORT_INDEX SMP_PortIndex_Create( UINT32 ifType, UINT32 slotNo, UINT32 portNo )
{
	return SDH_PortIndex_Create( ifType, slotNo, portNo, 0, 0 );
}

UINT32 PortIndex_GetType( HH_PORT_INDEX index )
{
	PORT_INDEX port;

	port.portIndex = index;

	return port.field.ifType;
}

UINT32 PortIndex_GetSlot( HH_PORT_INDEX index )
{
	PORT_INDEX port;

	port.portIndex = index;

	return port.field.slotNo;
}

UINT32 PortIndex_GetPortNo( HH_PORT_INDEX index )
{
	PORT_INDEX port;

	port.portIndex = index;

	return port.field.portNo;
}
UINT32 SDH_PortIndex_GetHP( HH_PORT_INDEX index )
{
	SDH_PORT_INDEX port;

	port.portIndex = index;

	return port.field.hp;
}

UINT32 SDH_PortIndex_GetLP( HH_PORT_INDEX index )
{
	SDH_PORT_INDEX port;

	port.portIndex = index;

	return port.field.lp;
}

UINT32 PDH_PortIndex_GetLP( HH_PORT_INDEX index )
{
	PDH_PORT_INDEX port;

	port.portIndex = index;

	return port.field.lp;
}

UINT32 VCG_PortIndex_GetLP( HH_PORT_INDEX index )
{
	VCG_PORT_INDEX port;

	port.portIndex = index;

	return port.field.lp;
}
UINT32 LAB_PortIndex_GetLable( HH_PORT_INDEX index )
{
	LAB_PORT_INDEX port;

	port.portIndex = index;

	return port.field.lable;
}
UINT32 VLAN_PortIndex_GetVlan( HH_PORT_INDEX index )
{
	VLAN_PORT_INDEX port;

	port.portIndex = index;

	return port.field.vlan;
}


HH_PORT_INDEX FUN_ETH_PortIndex_Create( UINT32 ifType, UINT32 slotNo, UINT32 portNo)
{
	FUN_ETH_PORT_INDEX port;

	port.portIndex = 0;
	port.field.ifType  = ifType;
	port.field.slotNo  = slotNo;
	port.field.portNo  = portNo;
	
	return port.portIndex;
}
UINT32 FUN_ETH_PortIndex_GetPid( HH_PORT_INDEX index )
{
	FUN_ETH_PORT_INDEX port;

	port.portIndex = index;

	return port.field.portNo;
}

UINT32 IS_PHY_PORT( HH_PORT_INDEX index )
{
	UINT32 port_type;
	
	port_type = PortIndex_GetType(index);
	switch(port_type)
	{
		/* just keep ETH/ETH-SFP/E1 type for ipran, modify by lipf, 2018/4/13 */
	
		case IFM_FUN_ETH_TYPE:
		//case IFM_FUN_ETH_SUB_TYPE:
		//case IFM_SOFT_TYPE:
		//case IFM_STMN_TYPE:
		case IFM_E1_TYPE:
		//case IFM_E1_SUB_TYPE:
		//case IFM_E2_TYPE:
		//case IFM_E3_TYPE:
		//case IFM_V35_TYPE:
		//case IFM_DSL_TYPE:
		//case IFM_POWN48_TYPE:
		//case IFM_EQU_TYPE:
		//case IFM_ENV_TYPE:
		//case IFM_FAN_TYPE:
		//case IFM_PWE3E1_TYPE:
		//case IFM_POW220_TYPE:
		//case IFM_PWE3STMN_TYPE:
		//case IFM_ETH_CLC_IN_TYPE:
		//case IFM_ETH_CLC_OUT_TYPE:
		//case IFM_ETH_CLC_SUB_TYPE:
		//case IFM_ETH_CLC_LINE_TYPE:
		case IFM_ETH_SFP_TYPE:
			return GPN_PINDEX_GEN_YES;
	}
	return GPN_PINDEX_GEN_NO;
}

UINT32 INDEX_2_STR(optObjOrient *id_struct, char *id_string, UINT32 len)
{
	UINT32 re_val;
	UINT32 ust_ct;
	
	/* assert */
	if( (id_struct == NULL) ||\
		(id_string == NULL) ||\
		(len < GPN_INDEX_2_STR_STD_BUFF) )
	{
		return GPN_PINDEX_GEN_ERR;
	}

	re_val = DEV_INDEX_2_STR(id_struct, id_string, len);
	if(re_val != GPN_PINDEX_GEN_OK)
	{
		return GPN_PINDEX_GEN_ERR;
	}

	ust_ct = strlen(id_string);
	re_val = PORT_INDEX_2_STR(id_struct, (id_string + ust_ct), (len - ust_ct));
	if(re_val != GPN_PINDEX_GEN_OK)
	{
		return GPN_PINDEX_GEN_ERR;
	}

	return GPN_PINDEX_GEN_OK;
}

UINT32 DEV_INDEX_2_STR(optObjOrient *id_struct, char *dev_string, UINT32 len)
{
	/* assert */
	if( (id_struct == NULL) ||\
		(id_struct->devIndex == GPN_DEVICE_INDEX_NULL) ||\
		(id_struct->devIndex == GPN_ILLEGAL_DEVICE_INDEX) ||\
		(dev_string == NULL) ||\
		(len < GPN_DEV_INDEX_2_STR_STD_BUFF) )
	{
		return GPN_PINDEX_GEN_ERR;
	}
	
	/*snprintf(dev_string, len, "#level(%d) slot(%d) %s ",\
		IFM_DEV_LEVEL_DECOM(id_struct->devIndex),\
		IFM_DEV_SLOT_DECOM(id_struct->devIndex),\
		gpnGlobalDevStrGet(IFM_DEV_TYPE_DECOM(id_struct->devIndex)));*/

	sprintf(dev_string, "unit(%d)",\
		IFM_DEV_UNIT_GET(id_struct->devIndex));

	return GPN_PINDEX_GEN_OK;
}

UINT32 PORT_INDEX_2_STR(optObjOrient *id_struct, char *pid_string, UINT32 len)
{
	char *pt_str;
	UINT32 port_type;
	
	/* assert */
	if( (id_struct == NULL) ||\
		(id_struct->portIndex == GPN_PORT_INDEX_NULL) ||\
		(id_struct->portIndex == GPN_ILLEGAL_PORT_INDEX) ||\
		(pid_string == NULL) ||\
		(len < GPN_PORT_INDEX_2_STR_STD_BUFF) )
	{
		return GPN_PINDEX_GEN_ERR;
	}

	/* get main port type */
	port_type = IFM_PORT_TYPE_DECOM(id_struct->portIndex);
	pt_str = gpnGlobalPortTypeSmpStrGet(port_type);
	switch(port_type)
	{
		case IFM_FUN_ETH_TYPE:
		case IFM_ETH_TYPE:
			snprintf(pid_string, len, "#%s mac(%d)\n\r",\
				pt_str,\
				IFM_FUN_ETH_PORTID_DECOM(id_struct->portIndex));
			break;
			
		case IFM_SOFT_TYPE:
		case IFM_POWN48_TYPE:
		case IFM_ENV_TYPE:
		case IFM_FAN_TYPE:
		case IFM_POW220_TYPE:
		case IFM_ETH_CLC_IN_TYPE:
		case IFM_ETH_CLC_OUT_TYPE:
		case IFM_ETH_CLC_SUB_TYPE:
		case IFM_ETH_CLC_LINE_TYPE:	
			snprintf(pid_string, len, "#%s slot(%d) port_id(%d)\n\r",\
				pt_str,\
				IFM_PORT_SLOT_DECOM(id_struct->portIndex),\
				IFM_PORT_ID_DECOM(id_struct->portIndex));
			break;
			
		case IFM_EQU_TYPE:
			snprintf(pid_string, len, "#%s(%d) sub_slot(%d)\n\r",\
				pt_str,\
				IFM_PORT_SLOT_DECOM(id_struct->portIndex),\
				IFM_PORT_ID_DECOM(id_struct->portIndex));
			break;
			
		case IFM_PWE3E1_TYPE:
		case IFM_PWE3STMN_TYPE:
			snprintf(pid_string, len, "#%s slot(%d) pw(%d)\n\r",\
				pt_str,\
				IFM_PORT_SLOT_DECOM(id_struct->portIndex),\
				IFM_PORT_ID_DECOM(id_struct->portIndex));
			break;
				
		case IFM_STMN_TYPE:
		case IFM_VC4_TYPE:
		case IFM_VC3_TYPE:
		case IFM_VC12_TYPE:
		case IFM_VCG_TYPE:
		case IFM_VCG_LP_TYPE:
		case IFM_E1_TYPE:
		case IFM_E2_TYPE:
		case IFM_E3_TYPE:
		case IFM_V35_TYPE:
		case IFM_DSL_TYPE:
			snprintf(pid_string, len, "#%s slot(%d) port_id(%d)\n\r",\
				pt_str,\
				IFM_PORT_SLOT_DECOM(id_struct->portIndex),\
				IFM_PORT_ID_DECOM(id_struct->portIndex));	
			break;
			
		case IFM_VS_TYPE:
		case IFM_LSP_TYPE:
		case IFM_PW_TYPE:
		case IFM_MPLS_TYPE:
		case IFM_TUNNEL_TYPE:	//add for ipran
		case IFM_E1_SUB_TYPE:	//add for ipran		
		case IFM_BFD_TYPE:	//add for ipran
			snprintf(pid_string, len, "#%s slot(%d) id(%d)\n\r",\
				pt_str,\
				IFM_PORT_SLOT_DECOM(id_struct->portIndex),\
				IFM_LAB_LAB_DECOM(id_struct->portIndex));
			break;
			
		case IFM_ETH_SFP_TYPE:
			snprintf(pid_string, len, "#%s slot(%d) map_mac(%d)\n\r",\
				pt_str,\
				IFM_PORT_SLOT_DECOM(id_struct->portIndex),\
				IFM_LAB_LAB_DECOM(id_struct->portIndex));
			break;
			
		case IFM_VPLSPW_TYPE:
			snprintf(pid_string, len, "#%s slot(%d) id(%d)\n\r",\
				pt_str,\
				IFM_PORT_SLOT_DECOM(id_struct->portIndex),\
				IFM_LAB_LAB_DECOM(id_struct->portIndex));
			break;

#if 0
		case IFM_VUNI_TYPE:
			snprintf(pid_string, len, "#%s vlan(%d) port(%d) level(%d) mep_id(%d)\n\r",\
				pt_str,\
				IFM_VLAN_VLAN_DECOM(id_struct->portIndex),\
				id_struct->portIndex3,\
				id_struct->portIndex4,\
				IFM_CFM_MEP_MEPID_DECOM(id_struct->portIndex5));
			break;
#endif
			
		case IFM_VLAN_OAM_TYPE:
			snprintf(pid_string, len, "#%s vlan(%d pid(%d))\n\r",\
				pt_str,\
				IFM_VLAN_VLAN_DECOM(id_struct->portIndex),\
				IFM_VLAN_PORTID_DECOM(id_struct->portIndex));
			break;

		case IFM_CFM_MEP_TYPE:
			/* stupid compare with 0x0000FFFF because stupid Problem-Solving Ideas, Ha~Ha~Ha~ */
			if(IFM_CFM_MEP_MEPID_DECOM(id_struct->portIndex6) == 0x0000FFFF)
			{
				/* local mep */
				snprintf(pid_string, len, "#%s mep_id(%d) #ma(%d) #md(%d) #vlan(%d pid(%d))\n\r",\
					pt_str,\
					IFM_CFM_MEP_MEPID_DECOM(id_struct->portIndex),\
					id_struct->portIndex3,\
					id_struct->portIndex4,\
					IFM_VLAN_VLAN_DECOM(id_struct->portIndex5),\
					IFM_VLAN_PORTID_DECOM(id_struct->portIndex5));
			}
			else
			{
				/* remote mep */
				snprintf(pid_string, len, "#%s mep_id(%d) #ma(%d) #md(%d) #vlan(%d pid(%d)) #rem_mep_id(%d)\n\r",\
					pt_str,\
					IFM_CFM_MEP_MEPID_DECOM(id_struct->portIndex),\
					id_struct->portIndex3,\
					id_struct->portIndex4,\
					IFM_VLAN_VLAN_DECOM(id_struct->portIndex5),\
					IFM_VLAN_PORTID_DECOM(id_struct->portIndex5),\
					IFM_CFM_MEP_MEPID_DECOM(id_struct->portIndex6));
			}
			break;
			
		default:
			snprintf(pid_string, len, "#NULL slot(%d) index(%08x)\n\r",\
				IFM_PORT_SLOT_DECOM(id_struct->portIndex),\
				IFM_PORT_ID_DECOM(id_struct->portIndex));
			break;
	}

	return GPN_PINDEX_GEN_OK;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_GLOBAL_PORT_INDEX_DEF_C_*/


