/*
*       根据错误码获取错误提示信息
*/

#include <lib/types.h>

#include "errcode.h"


const char *errcode_get_string (int errcode)
{
    const char *str = NULL;

    switch ( errcode )
    {
        case ERRNO_SUCCESS:
            break;
        case ERRNO_FAIL:
            str = "Operation Failed !";
            break;
        case ERRNO_NOT_FOUND:
            str = "The entry not found !";
            break;
        case ERRNO_EXISTED:
            str = "The entry is already existed !";
            break;
        case ERRNO_UNMATCH:
            str = "The parameter is unmatched !";
            break;
        case ERRNO_TIMEOUT:
            str = "Timeout !";
            break;
        case ERRNO_OVERSIZE:
            str = "The entry number exceed limit !";
            break;
        case ERRNO_MALLOC:
            str = "Memory alloc failed !";
            break;
        case ERRNO_IPC:
            str = "IPC send failed !";
            break;
        case ERRNO_CLI:
            str = "The command is invalid !";
            break;
        case ERRNO_PARAM_ILLEGAL:
            str = "Error in the input parameters !";
            break;
        case ERRNO_HAL:
            str = "Driver failed !";
            break;
        case ERRNO_FTM:
            str = "Forwording failed !";
            break;
		case ERRNO_INDEX_ALLOC:
			str = "Index alloc failed !";
			break;
		case ERRNO_PORT_ALLOC:
			str = "Port alloc failed !";
			break;
		case ERRNO_PKT_SEND:
			str = "Packet send failed !";
			break;
		case ERRNO_PKT_RECV:
			str = "Packet receive failed !";
			break;
        case ERRNO_ROUTER_ID:
            str = "Please config router-id at first !";
            break;
		case ERRNO_EXISTED_NHP:
			str = "Routing the next-hop already exists !";
			break;
		case ERRNO_EXISTED_STATIC_ROUTE:
			str = "Static routing already exists !";
			break;
		case ERRNO_INVALID_IP:
			str = "The specified IP address is invalid !";
			break;
		case ERRNO_EXISTED_IP:
			str = "IP address already exists !";
			break;
		case ERRNO_CONFLICT_IP:
			str = "The specified address conflicts with another address !";
			break;
		case ERRNO_SUBNET_SIMILAR:
			str = "A similar IP subnet already exists !";
			break;
		case ERRNO_MODE_NOT_L2:
			str = "Please select mode l2 !";
			break;
		case ERRNO_EXISTED_PW_DESTIP:
			str = "The same destination of PW already exists !";
			break;
		case ERRNO_ALREADY_BIND_L2VPN:
			str = "Interfaces already binding L2VPN !";
			break;
		case ERRNO_ALREADY_BIND_PW:
			str = "Interfaces already binding PW !";
			break;
        case ERRNO_ALREADY_BIND_TPOAM:
			str = "Already bind mplstp-oam !";
			break;
        case ERRNO_ALREADY_ENABLE_OAM:
            str = "Oam-session is already enable !";
            break;
        case ERRNO_MASTER_NOT_EXISTED:
            str = "The master does not exist !";
            break;
        case ERRNO_CONFIG_INCOMPLETE:
            str = "Configuration is incomplete !";
            break;
        case ERRNO_PEER_IP_UNMATCH:
            str = "Peer ip and tunnel dip don't match !";
            break;
        case ERRNO_MODE_NOT_CONFIG:
            str = "Mode is not configured !";
            break;
        case ERRNO_GRE_NOT_SUPPORT:
            str = "Does not support the GRE configuration !";
            break;
        case ERRNO_PW_INGRESS_CIR:
            str = "ingress CAR CAC check failed !";
            break;
        case ERRNO_PW_EGRESS_CIR:
            str = "egress CAR CAC check failed !";
            break;
	    case ERRNO_INTERFACE_NOT_EXIST:
            str = "The interface not exist, Please Check it!";
            break;
		case ERRNO_OUTIF_NOT_SUPPORT:
            str = "The interface cannot be used for GRE outif, Please Check it!";
            break;
		case ERRNO_OUTIF_UNMATCH_TUNNEL:
            str = "The slot of the interface and the slot of the tunnel do not match, Please Check it!!";
            break;
		case ERRNO_OUTIF_NOT_L3:
			str = "The interface mode is not L3, Please check it!";
			break;
		#if 0
        case IFM_ERR_GENERAL:
            str = "Failed to execute command";
            break;
        case IFM_ERR_XMALLOC:
            str = "Out of memory";
            break;
        case IFM_ERR_PORT_NOT_FOUND:
            str = "Physical interface not exist ,please check out";
            break;
        case IFM_ERR_PARNT_NOT_FOUND:
            str = "Physical interface of this subport not exist ,please check out";
            break;
        case IFM_ERR_MEM:
            str = "Memy out of range";
            break;
        case IFM_ERR_ENCAP_UNTAG:
            str = "Only one untag subport can config under a physical interface";
            break;
        case IFM_ERR_ENCAP_DOT1Q:
            str = "Encap vlan of subport can not be repeated or have an intersection";
            break;
        case IFM_ERR_ENCAP_QINQ:
            str = "Encap vlan of subport can not be repeated or have an intersection";
            break;
        case IFM_ERR_ENCAP_RANGE:
            str = "Can't set end of vlan < start of vlan";
            break;
        case IFM_ERR_ENCAP_RANGE_L3:
            str = "Can't set vlan range in l3 mode";
            break;
        case IFM_ERR_PARAM_MAC_MC:
            str = "Can't set multicasts mac";
            break;
        case IFM_ERR_PARAM_MAC_BC:
            str = "Can't set broadcast mac";
            break;
        case IFM_ERR_PARAM_MAC_ZERO:
            str = "Can't set mac all zero";
            break;
        case IFM_ERR_PARNT_NO_L3:
            str = "This physical interface mode isn't l3";
            break;
        case IFM_ERR_ENCAP_UNTAG_L3:
            str = "Can't set vlan untag in l3 mode";
            break;
        case IFM_ERR_PARAM_SPEED_AUTO:
            str = "AutoNegotiation is Enabled";
            break;
        case IFM_ERR_PARAM_NOSUPPORT:
            str = "No support this config to the interface";
            break;
        case IFM_ERR_PARAM_E1_NO_FRAMED:
            str = "This physical interface mode isn't framded";
            break;
        case IFM_ERR_ENCAP_VLANT_MODE_ERRO:
            str = "Can't set vlan translate in l3 mode";
            break;
        case IFM_ERR_ENCAP_XLVLAN_SAME_ERRO:
            str = "Can't set translate vlan to the same vlan";
            break;
        case IFM_ERR_BINDED_TRUNK_ERRO:
            str = "Failed to execute command, due to the interface had binded trunk";
            break;
        case IFM_ERR_INTERFACE_OUTOFNUM_ERRO:
            str = "The number of interfaces exceeds the capacity";
            break;
        case IFM_ERR_ENCAP_EXIST:
            str = "The encapsulate vlan had been setted, please unset the old encapsulate vlan first";
            break;
        case IFM_ERR_NOT_FOUND:
            str = "This interface not exist";
            break;
        case IFM_ERR_PARAM_TRUNK_RANGE_32:
            str = "Out of trunk range, this device trunk range 1-32";
            break;
        case IFM_ERR_PARAM_SUPPORT_ONE:
            str = "Only support one interface at the same time";
            break;
		case IFM_ERR_PARAM_FORMAT:
            str = "Wrong format ,please check out";
            break;
		case IFM_ERR_PORT_NOSUPPORT_DELETE:
            str = "This physical interface, can't delete";
            break;
		case IFM_ERR_STRING_OUT_RANGE:
            str = "String out of range";
            break;
		case IFM_ERR_PARAM_MAC_INVALID:
            str = "Please check out mac foramt";
            break;
		case IFM_ERR_PARAM_IP_INVALID:
	     	str = "The ip is invalid";
            break;
		case IFM_ERR_LOOPBACK_NO_SET:
	     	str = "Reflector is not configured";
            break;
		case IFM_ERR_SLA_IS_SETTED:
	     	str = "Reflector can't set with sla meanwhile!";
            break;	
		case IFM_ERR_TRUNK_NO_EXIST:
	     	str = "The trunk not exist,please first create trunk";
	     	break;	
		case IFM_ERR_LOOPBACK_EX_SET:
	     	str = "Reflector external had been setted";
            break;	
		case IFM_ERR_LOOPBACK_IN_SET:
	     	str = "Reflector internal had been setted";
	    	break;
		case IFM_ERR_DCN_ENABLE:
			str = "DCN has been enabled, this port is occupied by DCN and cannot be accessed";
	    	break;
		#endif
		case CES_ERR_MODE_NO_SET:
			str = "Mode is not configured,please set mode";
			break;
		case CES_ERR_EXE_FAILED:
			str = "Failed to set";
			break;
		case CES_ERR_UNFRAM_NO_SET:
			str = "The mode is not framed or multiframed";
			break;
		case CES_ERR_NOSUPPORT_CESOPSN:
			str = "Do not support cesopsn";
			break;
		case CES_ERR_CREATE:
			str = "Failed to create";
			break;
		case CES_ERR_DELETE:
			str = "Failed to delete";
			break;
		case CES_ERR_TIMESLOT:
			str = "The start timeslot must less than the end timeslot";
			break;
		case CES_ERR_TIMESLOT_USED:
			str = "Part of the timeslot had been set by other interface";
			break;
		case CES_ERR_PRBS_USED:
			str = "The prbs had been set by other interface";
			break;
		case EFM_ERR_IF_NOT_FOUND:
	     	str = "Interface Not found!%s";	
			break;
		case EFM_ERR_NOT_ENABLED:
	      	str = "EFM not enabled!%s";
			break;
		case EFM_ERR_ENABLED:
	        str = "EFM already enabled!%s";
			break;
			
		case EFM_ERR_MEMORY:
			str =  "Out of memory while updating!%s";
			break;
		
		case EFM_ERR_REM_LB_NOT_SUPP:
			str = "Remote Loopback not supported!%s";
			break;
		
		case EFM_ERR_DISCOVERY:
			str =  "EFM discovery state is not SEND_ANY!%s";
			break;
			
		case EFM_ERR_RB_MODE:
			str = "EFM is in Remote Loopback mode!%s";
			break;
			
		case EFM_ERR_SET_UNIDIREC:
			str = "Set EFM Unidirectional Link Support failed!%s";
			break;
			
		case EFM_ERR_LOCALSIDE_MAKE_REMOTESIDE_STARTED:
			str = "EFM is already start remote loopback!%s";
			break;
			
		case EFM_ERR_REMOTESIDE_MAKE_LOCALSIDE_STARTED:
			str = "EFM is already start local loopback!%s";
			break;
			
		case EFM_ERR_MODE_PASSIVE:
			str = "EFM mode is passive!%s";
			break;	
		case L2_TRUNK_ERR_GET_L2IF:
			str = "L2if failed to get";
			break;
		case L2_TRUNK_ERR_NOT_FOUND:
			str = "The trunk failed to lookup";
			break;
		case L2_TRUNK_ERR_MEMBER_OUTOFNUM:
			str = "The aggregation group exceeds the maximum number of members";
			break;
		case L2_TRUNK_ERR_HAL:
			str = "The ipc message send hal error";
			break;
		case L2_TRUNK_ERR_NOFAILBACK_NO_SET:
			str = "The non-failback is not set, please set failback";
			break;
		case L2_TRUNK_ERR_NOSUPPORT_OUTBAND:
			str = "The outband is not support";
			break;
		case L2_TRUNK_ERR_MEMBER_USED:
			str = "The interface has been set to other aggregation group members";
			break;
		case L2_TRUNK_ERR_LACP_DISABLE:
			str = "The lacp disable,please enable lacp";
			break;
		case L2_TRUNK_ERR_MANUAL_ECMP_NO_SET:
			str = "The manual load-balance mode is not set";
			break;
		case L2_TRUNK_ERR_NOT_TRUNK_MEMBER:
			str = "The interface is not its aggregation group members";
			break;
		case L2_TRUNK_ERR_ECMP_NO_SET:
			str = "The load-balance mode is not set";
			break;
		case L2_TRUNK_ERR_BACKUP_NO_SET:
			str = "The backup mode is not set";
			break;
		case L2_TRUNK_ERR_IF_NO_EXISTED:
			str = "The trunk interface not exist";
			break;
		case L2_TRUNK_ERR_MEMBER_ADD:
			str = "The trunk member failed to add";
			break;
		case L2_TRUNK_ERR_MEMBER_DEL:
			str = "The trunk member failed to delete";
			break;
		case L2_TRUNK_ERR_MEMBER_INIT:
			str = "The trunk member failed to init";
			break;
		case L2_TRUNK_ERR_TRUNK_DEL:
			str = "The group members is not empty,please delete trunk member at first";
			break;
		case L2_TRUNK_ERR_TRUNK_UN_CONFIGURABLE:
			str = "The trunk can not config ecmp in this node";
			break;
		#if 0
		case IFM_ERR_PARAM_TUNNEL_RANGE_128:
			str = "Out of tunnel range, device HT201 tunnel range 1-128";
		break;
		case IFM_ERR_PARAM_TUNNEL_RANGE_2000:
			str = "Out of tunnel range, device HT2200 tunnel range 1-128";
		break;
		case IFM_ERR_TUNNEL_UNIT:
			str = "The unit is not 1 !";
			break;
		#endif
        default :
            str = "Undefined error !";
            break;
    }

	return str;
}

