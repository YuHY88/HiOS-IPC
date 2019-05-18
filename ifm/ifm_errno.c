#include "command.h"
#include "types.h"
#include "ifm_errno.h"
int
ifm_cli_parse_erro ( struct vty *vty, int ret )
{
    const char *str = NULL;

    switch ( ret )
    {
        case IFM_SUCCESS:
            break;
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
	     str = "Loopback is not configured";
            break;
	case IFM_ERR_SLA_IS_SETTED:
	     str = "Loopback can't set with sla meanwhile!";
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
	case IFM_ERR_PARAM_TUNNEL_RANGE_128:
		str = "Out of tunnel range, device HT201 tunnel range 1-128";
		break;
	case IFM_ERR_PARAM_TUNNEL_RANGE_2000:
		str = "Out of tunnel range, device HT2200 tunnel range 1-128";
		break;
	case IFM_ERR_TUNNEL_UNIT:
		str = "The unit is not 1 !";
		break;
	case IFM_ERR_VLANIF:
		str = "The vlan value is resered vlan !";
		break;
	case IFM_ERR_REFLECTOR :
		str = "The port not support reflector !";
		break;
	case IFM_ERR_PARAM_SPEED_GE :
		str = "Copper port not support force 1000M !";
		break;
    default :
        str = "Failed to execute command";
        break;
    }

    vty_error_out ( vty, "%s %s", str, VTY_NEWLINE );
    return 0;
}

