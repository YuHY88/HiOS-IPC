#include <string.h>
#include <stdlib.h>
#include <lib/command.h>
#include <lib/linklist.h>
#include <lib/msg_ipc_n.h>
#include <lib/msg_ipc.h>
#include <lib/memshare.h>
#include <lib/thread.h>
#include <lib/vty.h>
#include <lib/types.h>
#include <lib/log.h>

#include "ifm_common.h"

/* Command vector which includes some level of command lists. Normally
   each daemon maintains each own cmdvec. */
extern vector cmdvec;

struct cmd_node physical_if_node =
{
    PHYSICAL_IF_NODE,
    "%s(config-ethernet-if)# ",
    1
};

struct cmd_node physical_subif_node =
{
    PHYSICAL_SUBIF_NODE,
    "%s(config-ethernet-subif)# ",
    1
};

struct cmd_node tdm_if_node =
{
    TDM_IF_NODE,
    "%s(config-tdm-if)# ",
    1
};

struct cmd_node tdm_subif_node =
{
    TDM_SUBIF_NODE,
    "%s(config-tdm-subif)# ",
    1
};

struct cmd_node stm_if_node =
{
    STM_IF_NODE,
    "%s(config-stm-if)# ",
    1
};

struct cmd_node stm_subif_node =
{
    STM_SUBIF_NODE,
    "%s(config-stm-subif)# ",
    1
};


struct cmd_node loopback_if_node =
{
    LOOPBACK_IF_NODE,
    "%s(config-loopback-if)# ",
    1
};

struct cmd_node tunnel_if_node =
{
    TUNNEL_IF_NODE,
    "%s(config-tunnel-if)# ",
    1
};

struct cmd_node trunk_if_node =
{
    TRUNK_IF_NODE,
    "%s(config-trunk-if)# ",
    1
};

struct cmd_node trunk_subif_node =
{
    TRUNK_SUBIF_NODE,
    "%s(config-trunk-subif)# ",
    1
};


struct cmd_node vlanif_node =
{
    VLANIF_NODE,
    "%s(config-vlanif)# ",
    1
};

struct cmd_node clockif_node =
{
    CLOCK_NODE,
    "%s(config-clock)# ",
    1
};

struct cmd_node vcgif_node =
{
    VCG_NODE,
    "%s(config-sdhvcg-if)# ",
    1
};

struct cmd_node e1if_node =
{
    E1_IF_NODE,
    "%s(config-e1-if)# ",
    1
};

static int ifm_get_usp ( char *iusp, char **ousp )
{
    char *buf = iusp;
    int i = 0;
    char *tmp = NULL;
    int dot_count = 0;
    int slash_count = 0;
    char *tousp[IFM_USP_STRTOK_LEN] = {NULL};

    for ( tmp = buf; *tmp != '\0'; tmp++ )
    {
        if ( *tmp == '/' )
        {
            slash_count++;
            if ( * ( tmp + 1 ) == '.' )
            {
                return -1;
            }
        }
        else if ( *tmp == '.' )
        {
            if ( * ( tmp + 1 ) == '\0' )
            {
                return -1;
            }
            dot_count++;
        }
    }
    if ( slash_count != 2 )
    {
        return -1;
    }
    if ( dot_count != 1 && dot_count != 0 )
    {
        return -1;
    }

    while ( i < IFM_USP_STRTOK_LEN && ( tousp[i] = strtok ( buf, "/" ) ) != NULL )
    {

        if ( ( ( char * ) ousp + i * IFNET_NAMESIZE ) != NULL )
        {
            snprintf ( ( ( char * ) ousp + i * IFNET_NAMESIZE ), strlen ( tousp[i] ) + 1, "%s", tousp[i] );
        }
        else
        {
            return -1;
        }
		
        i++;

        buf = NULL;
    }
    if ( i != 3 )
    {
        return -1;
    }

    i   = 2;
    buf = tousp[2];

    while (i < IFM_USP_STRTOK_LEN && ( tousp[i] = strtok ( buf, "." ) ) != NULL )
    {
        if ( ( ( char * ) ousp + i * IFNET_NAMESIZE ) != NULL )
        {
            snprintf ( ( ( char * ) ousp + i * IFNET_NAMESIZE ), strlen ( tousp[i] ) + 1, "%s", tousp[i] );
        }
        else
        {
            return -1;
        }
		
        i++;
		
        buf = NULL;

    }
    if ( tousp[3] == NULL )
    {
        return IFNET_IS_PORT;
    }
    else
    {
        return IFNET_IS_SUBPORT;
    }

    return -1;
}


static int ifm_get_trunk ( char *iusp, char **ousp )
{
    char *buf = iusp;
    int i = 0;
    char *tousp[IFM_USP_STRTOK_LEN] = {NULL};
    char *tmp = NULL;
    int dot_count = 0;

    for ( tmp = buf; *tmp != '\0'; tmp++ )
    {
        if ( *tmp == '.' )
        {
            if ( * ( tmp + 1 ) == '\0' )
            {
                return -1;
            }
            dot_count++;
        }
    }
    if ( dot_count != 1 && dot_count != 0 )
    {
        return -1;
    }

    while (i < IFM_USP_STRTOK_LEN && ( tousp[i] = strtok ( buf, "." ) ) != NULL )
    {

        if ( ( ( char * ) ousp + i * IFNET_NAMESIZE ) != NULL )
        {
            snprintf ( ( ( char * ) ousp + i * IFNET_NAMESIZE ), strlen ( tousp[i] ) + 1, "%s", tousp[i] );

        }
        else
        {
            return -1;
        }

        i++;
        buf = NULL;
    }

    if ( i == 1 )
    {
        return IFNET_IS_PORT;
    }
    else if ( i == 2 )
    {
        return IFNET_IS_SUBPORT;
    }

    return -1;
}


const char *ifm_get_typestr ( enum IFNET_TYPE type )
{
    switch ( type )
    {
        case IFNET_TYPE_VLANIF:
            return "vlanif";
        case IFNET_TYPE_LOOPBACK:
            return "loopback";
        case IFNET_TYPE_TUNNEL:
            return "tunnel";
        case IFNET_TYPE_TRUNK:
            return "trunk";
        case IFNET_TYPE_TDM:
            return "tdm";
        case IFNET_TYPE_STM:
            return "stm";
        case IFNET_TYPE_ETHERNET:
            return "ethernet";
		case IFNET_TYPE_GIGABIT_ETHERNET:
        	return "gigabitethernet";
		case IFNET_TYPE_XGIGABIT_ETHERNET:
        	return "xgigabitethernet";
        case IFNET_TYPE_CLOCK:
            return "clock";
		case IFNET_TYPE_VCG:
			return "sdhvcg";
		case IFNET_TYPE_E1:
			return "e1";			
        default:
            return "unknown";
    }
    return "unknown";
}


int ifm_get_typenum ( char *type_str )
{
    if ( type_str == NULL )
    { return IFNET_TYPE_INVALID; }

    if ( !strcmp ( type_str, "vlanif" ) )
    {
        return  IFNET_TYPE_VLANIF;
    }
    else if ( !strcmp ( type_str, "loopback" ) )
    {
        return  IFNET_TYPE_LOOPBACK;
    }
    else if ( !strcmp ( type_str, "tunnel" ) )
    {
        return  IFNET_TYPE_TUNNEL;
    }
    else if ( !strcmp ( type_str, "trunk" ) )
    {
        return  IFNET_TYPE_TRUNK;
    }
    else if ( !strcmp ( type_str, "tdm" ) )
    {
        return  IFNET_TYPE_TDM;
    }
    else if ( !strcmp ( type_str, "stm" ) )
    {
        return  IFNET_TYPE_STM;
    }
    else if ( !strcmp ( type_str, "ethernet" ) )
    {
        return  IFNET_TYPE_ETHERNET;
    }
    else if ( !strcmp ( type_str, "clock" ) )
    {
        return  IFNET_TYPE_CLOCK;
    }
	else if ( !strcmp ( type_str, "gigabitethernet" ) )
    {
        return  IFNET_TYPE_GIGABIT_ETHERNET;
    }
	else if ( !strcmp ( type_str, "xgigabitethernet" ) )
    {
        return  IFNET_TYPE_XGIGABIT_ETHERNET;
    }
    else if ( !strcmp ( type_str, "sdhvcg" ) )
    {
        return  IFNET_TYPE_VCG;
    }	
    else if ( !strcmp ( type_str, "e1" ) )
    {
        return  IFNET_TYPE_E1;
    }	
    else
    {
        return IFNET_TYPE_INVALID;
    }

    return IFNET_TYPE_INVALID;
}



/* return ifindex, 0 means failure */
/* type:4bit, unit:3bit, slot:5bit, port:8bit, sub_port:12bit*/
uint32_t ifm_get_ifindex_by_usp ( struct ifm_usp *usp )
{
    uint32_t ifindex = 0;

	if(usp == NULL)
	{
        zlog_err ( "%s[%d] usp is null\n", __FUNCTION__, __LINE__ );
        return 0;

	}


    if ( usp->type == IFNET_TYPE_INVALID )
    {
        return 0;
    }

    if ( IFNET_TYPE_MAX < usp->type )
    {
        return 0;
    }
    if ( IFM_UNIT_MAX < usp->unit )
    {
        return 0;
    }
    if ( IFM_SLOT_MAX < usp->slot )
    {
        return 0;
    }
    //if ( IFM_PORT_MAX < usp->port )
    //{
    //    return 0;
    //}
    if ( IFM_SUBPORT_MAX < usp->sub_port )
    {
        return 0;
    }

    ifindex = ( ( usp->type & _SHR_PORT_TYPE_MASK ) << _SHR_PORT_TYPE_SHIFT )
              | ( ( usp->unit & _SHR_UNIT_MASK ) << _SHR_UNIT_SHIFT )
              | ( ( usp->slot & _SHR_SLOT_MASK ) << _SHR_SLOT_SHIFT )
              | ( ( usp->port & _SHR_PORT_MASK ) << _SHR_PORT_SHIFT )
              | ( ( usp->sub_port & _SHR_SUBPORT_MASK ) << _SHR_SUBPORT_SHIFT );

    return ifindex;
}

/* return ifindex, 0 means failure */
uint32_t ifm_get_ifindex_by_name ( const char *type, const char *ifname )
{
    uint32_t ifindex = 0;
    struct ifm_usp usp;
    int erro = 0;
    int ret = 0;
    char tmp[IFM_USP_STRTOK_LEN][IFNET_NAMESIZE];
    char name_tmp[IFNET_NAMESIZE] = {'\0'};

	memset(&tmp[0][0], '\0', IFM_USP_STRTOK_LEN*IFNET_NAMESIZE);
	if(type == NULL)
	{
        zlog_err ( "%s[%d] type is null\n", __FUNCTION__, __LINE__ );
        return 0;

	}

	if(ifname == NULL)
	{
        zlog_err ( "%s[%d] ifname is null\n", __FUNCTION__, __LINE__ );
        return 0;

	}

    memset ( &usp, 0, sizeof ( struct ifm_usp ) );

    if ( strlen ( ifname ) > ( NAME_STRING_LEN - 1 ) )
    {
        zlog_err ( "%s[%d] String out of range\n", __FUNCTION__, __LINE__ );
        return 0;
    }
    
    snprintf ( name_tmp, strlen ( ifname ) + 1, "%s", ifname );

    usp.type = ifm_get_typenum ( ( char * ) type );

    if ( usp.type == IFNET_TYPE_INVALID )
    {
        zlog_err ( "%s[%d] ifm_get_usp erro\n", __FUNCTION__, __LINE__ );
        return 0;
    }

    switch ( usp.type )
    {
        case IFNET_TYPE_TDM:
        case IFNET_TYPE_STM:
        case IFNET_TYPE_ETHERNET:
		case IFNET_TYPE_GIGABIT_ETHERNET:
		case IFNET_TYPE_XGIGABIT_ETHERNET:
            ret = ifm_get_usp ( ( char * ) name_tmp, ( char ** ) tmp );
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] ifm_get_usp erro\n", __FUNCTION__, __LINE__ );
                return 0;
            }
            usp.type = ifm_get_typenum ( ( char * ) type );
            if ( ret == IFNET_IS_SUBPORT )
            {
                if ( usp.type == IFNET_TYPE_TDM )
                {
                    GET_DIGIT_RANGE ( usp.sub_port, tmp[3], IFM_SUBTDM_MIN, IFM_SUBTDM_MAX, erro );
                }
                else if ( usp.type == IFNET_TYPE_STM )
                {
                    GET_DIGIT_RANGE ( usp.sub_port, tmp[3], IFM_SUBSTM_MIN, IFM_SUBSTM_MAX, erro );
                }
                else if (( usp.type == IFNET_TYPE_ETHERNET )
						|| ( usp.type == IFNET_TYPE_GIGABIT_ETHERNET )
						|| ( usp.type == IFNET_TYPE_XGIGABIT_ETHERNET ))
                {
                    GET_DIGIT_RANGE ( usp.sub_port, tmp[3], IFM_SUBPORT_MIN, IFM_SUBPORT_MAX, erro );
                }
                if ( erro < 0 )
                {
                    zlog_err ( "%s[%d] GET_DIGIT_RANGE sub_port erro\n", __FUNCTION__, __LINE__ );
                    return 0;
                }
            }

            GET_DIGIT_RANGE ( usp.unit, tmp[0], IFM_UNIT_MIN, IFM_UNIT_MAX, erro );
            if ( erro < 0 )
            {
                zlog_err ( "%s[%d] GET_DIGIT_RANGE unit erro\n", __FUNCTION__, __LINE__ );
                return 0;
            }
            GET_DIGIT_RANGE ( usp.slot, tmp[1], IFM_SLOT_MIN, IFM_SLOT_MAX, erro );
            if ( erro < 0 )
            {
                zlog_err ( "%s[%d] GET_DIGIT_RANGE slot erro\n", __FUNCTION__, __LINE__ );
                return 0;
            }

            GET_DIGIT_RANGE ( usp.port, tmp[2], IFM_PORT_MIN, IFM_PORT_MAX, erro );
            if ( erro < 0 )
            {
                zlog_err ( "%s[%d] GET_DIGIT_RANGE port erro\n", __FUNCTION__, __LINE__ );
                return 0;
            }
            break;
        case IFNET_TYPE_TRUNK:
            ret = ifm_get_trunk ( name_tmp, ( char ** ) tmp );
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] ifm_get_usp erro\n", __FUNCTION__, __LINE__ );
                return 0;
            }

            if ( ret == IFNET_IS_PORT )
            {
                GET_DIGIT_RANGE ( usp.port, tmp[0], IFM_TRUNK_MIN, IFM_TRUNK_MAX, erro );
                if ( erro < 0 )
                {
                    zlog_err ( "%s[%d] GET_DIGIT_RANGE port erro\n", __FUNCTION__, __LINE__ );
                    return 0;
                }
            }
            else if ( ret == IFNET_IS_SUBPORT )
            {
                GET_DIGIT_RANGE ( usp.port, tmp[0], IFM_TRUNK_MIN, IFM_TRUNK_MAX, erro );
                if ( erro < 0 )
                {
                    zlog_err ( "%s[%d] GET_DIGIT_RANGE port erro\n", __FUNCTION__, __LINE__ );
                    return 0;
                }
                GET_DIGIT_RANGE ( usp.sub_port, tmp[1], IFM_SUBTRUNK_MIN, IFM_SUBTRUNK_MAX, erro );
                if ( erro < 0 )
                {
                    zlog_err ( "%s[%d] GET_DIGIT_RANGE sub_port erro\n", __FUNCTION__, __LINE__ );
                    return 0;
                }
            }
            else
            {
                zlog_err ( "%s[%d] strtok erro\n", __FUNCTION__, __LINE__ );
                return 0;
            }

            break;
        case IFNET_TYPE_VLANIF:
            GET_DIGIT_RANGE ( usp.sub_port, name_tmp, IFM_SUBPORT_MIN, IFM_SUBPORT_MAX, erro );
            if ( erro < 0 )
            { return 0; }
            break;

        case IFNET_TYPE_LOOPBACK:
            GET_DIGIT_RANGE ( usp.sub_port, name_tmp, 0, IFM_SUBPORT_MAX, erro );
            if ( erro < 0 )
            { return 0; }
            break;
        case IFNET_TYPE_TUNNEL:
            ret = ifm_get_usp ( name_tmp, ( char ** ) tmp );
            if ( ret < 0 )
            {
                zlog_err ( "%s[%d] ifm_get_usp erro\n", __FUNCTION__, __LINE__ );
                return 0;
            }

            if ( ret != IFNET_IS_PORT )
            {
            	zlog_err ( "%s[%d] strtok erro ret %d\n", __FUNCTION__, __LINE__, ret );
                return 0;
            }
            else
            {
                GET_DIGIT_RANGE ( usp.unit, tmp[0], IFM_UNIT_MIN, IFM_UNIT_MAX, erro );
                if ( erro < 0 )
                {
                    zlog_err ( "%s[%d]  GET_DIGIT_RANGE unit erro\n", __FUNCTION__, __LINE__ );
                    return 0;
                }
                GET_DIGIT_RANGE ( usp.slot, tmp[1], IFM_SLOT_MIN, IFM_SLOT_MAX, erro );
                if ( erro < 0 )
                {
                    zlog_err ( "%s[%d]  GET_DIGIT_RANGE slot erro\n", __FUNCTION__, __LINE__ );
                    return 0;
                }

                GET_DIGIT_RANGE ( usp.sub_port, tmp[2], IFM_TUNNL_MIN, IFM_TUNNL_MAX, erro );
                if ( erro < 0 )
                {
                    zlog_err ( "%s[%d]  GET_DIGIT_RANGE port erro\n", __FUNCTION__, __LINE__ );
                    return 0;
                }
            }
            break;
		case IFNET_TYPE_VCG:
			ret = ifm_get_usp ( name_tmp, ( char ** ) tmp );
			if ( ret < 0 )
			{
				zlog_err ( "%s[%d] ifm_get_usp erro\n", __FUNCTION__, __LINE__ );
				return 0;
			}
		
			if ( ret != IFNET_IS_PORT )
			{
				zlog_err ( "%s[%d] strtok erro ret %d\n", __FUNCTION__, __LINE__, ret );
				return 0;
			}
			else
			{
				GET_DIGIT_RANGE ( usp.unit, tmp[0], IFM_UNIT_MIN, IFM_UNIT_MAX, erro );
				if ( erro < 0 )
				{
					zlog_err ( "%s[%d]	GET_DIGIT_RANGE unit erro\n", __FUNCTION__, __LINE__ );
					return 0;
				}
				GET_DIGIT_RANGE ( usp.slot, tmp[1], IFM_SLOT_MIN, IFM_SLOT_MAX, erro );
				if ( erro < 0 )
				{
					zlog_err ( "%s[%d]	GET_DIGIT_RANGE slot erro\n", __FUNCTION__, __LINE__ );
					return 0;
				}
		
				GET_DIGIT_RANGE ( usp.port, tmp[2], IFM_PORT_MIN, IFM_PORT_MAX, erro );
				if ( erro < 0 )
				{
					zlog_err ( "%s[%d]	GET_DIGIT_RANGE port erro\n", __FUNCTION__, __LINE__ );
					return 0;
				}
			}
			break;
		case IFNET_TYPE_E1:
			ret = ifm_get_usp ( name_tmp, ( char ** ) tmp );
			if ( ret < 0 )
			{
				zlog_err ( "%s[%d] ifm_get_usp erro\n", __FUNCTION__, __LINE__ );
				return 0;
			}
		
			if ( ret != IFNET_IS_PORT )
			{
				zlog_err ( "%s[%d] strtok erro ret %d\n", __FUNCTION__, __LINE__, ret );
				return 0;
			}
			else
			{
				GET_DIGIT_RANGE ( usp.unit, tmp[0], IFM_UNIT_MIN, IFM_UNIT_MAX, erro );
				if ( erro < 0 )
				{
					zlog_err ( "%s[%d]	GET_DIGIT_RANGE unit erro\n", __FUNCTION__, __LINE__ );
					return 0;
				}
				GET_DIGIT_RANGE ( usp.slot, tmp[1], IFM_SLOT_MIN, IFM_SLOT_MAX, erro );
				if ( erro < 0 )
				{
					zlog_err ( "%s[%d]	GET_DIGIT_RANGE slot erro\n", __FUNCTION__, __LINE__ );
					return 0;
				}
		
				GET_DIGIT_RANGE ( usp.port, tmp[2], IFM_PORT_MIN, IFM_PORT_MAX, erro );
				if ( erro < 0 )
				{
					zlog_err ( "%s[%d]	GET_DIGIT_RANGE port erro\n", __FUNCTION__, __LINE__ );
					return 0;
				}
			}
			break;
        case IFNET_TYPE_CLOCK:
            usp.unit = 1;
            GET_DIGIT_RANGE ( usp.port, name_tmp, IFM_CLOCK_MIN, IFM_CLOCK_MAX, erro );
            if ( erro < 0 )
            { return 0; }
            break;
        default:
            zlog_err ( "%s[%d] miss type %d\n", __FUNCTION__, __LINE__, usp.type );
            return 0;
    }

    ifindex = ifm_get_ifindex_by_usp ( &usp );

    return ifindex;
}

uint32_t ifm_get_ifindex_by_name2 ( char *type_ifname )
{
    char buf1[IFNET_NAMESIZE] = {'\0'};
    char tmp_type_ifname[2][IFNET_NAMESIZE];
    int i = 0;
    uint32_t ifindex = 0;

	memset(&tmp_type_ifname[0][0], '\0', 2*IFNET_NAMESIZE);

	if(type_ifname == NULL)
	{
        zlog_err ( "%s[%d] type_ifname is null\n", __FUNCTION__, __LINE__ );
        return 0;

	}

    if ( strlen ( type_ifname ) > ( NAME_STRING_LEN - 1 ) )
    {
        zlog_err ( "%s[%d] String out of range\n", __FUNCTION__, __LINE__ );
        return 0;
    }
    snprintf ( buf1, strlen ( type_ifname ) + 1, "%s", type_ifname );
    zlog_debug ( 1,"%s[%d]  buf1 %s type_ifname %s \n", __FUNCTION__, __LINE__, buf1, type_ifname );

    /* Establish string and get the first token: */
    char *token = strtok ( buf1, " " );
    while ( ( token != NULL ) && ( i < 2 ) )
    {
        /* While there are tokens in "string" */
        zlog_debug ( 1,"%s[%d]  token %s \n", __FUNCTION__, __LINE__, token );
        snprintf ( tmp_type_ifname[i], strlen ( token ) + 1, "%s", token );
        /* Get next token: */
        token = strtok ( NULL, " " );
        i ++;
    }
    if ( i != 2 )
    {
        return 0;
    }
    ifindex = ifm_get_ifindex_by_name ( tmp_type_ifname[0], tmp_type_ifname[1] );

    zlog_debug (1, "%s[%d]  ifindex 0x%0x \n", __FUNCTION__, __LINE__, ifindex );

    return ifindex;
}


/* type:4bit, unit:3bit, slot:5bit, port:8bit, sub_port:12bit*/
int ifm_get_usp_by_ifindex ( uint32_t ifindex, struct ifm_usp *if_usp )
{
    if ( NULL == if_usp )
    {
        return -1;
    }

    if_usp->type = IFM_TYPE_ID_GET ( ifindex );
    if_usp->unit = IFM_UNIT_ID_GET ( ifindex );
    if_usp->slot = IFM_SLOT_ID_GET ( ifindex );
    if_usp->port = IFM_PORT_ID_GET ( ifindex );
    if_usp->sub_port = IFM_SUBPORT_ID_GET ( ifindex );

    if ( IFNET_TYPE_MAX < if_usp->type
            || IFM_UNIT_MAX < if_usp->unit
            || IFM_SLOT_MAX < if_usp->slot)
            //|| IFM_PORT_MAX < if_usp->port)
            //|| IFM_SUBPORT_MAX < if_usp->sub_port )
    {
        return -1;
    }

    return 0;
}



int ifm_get_name_by_ifindex ( uint32_t ifindex, char *name )
{
    const char *type_name = NULL;
    int type;

    if ( name == NULL )
    { return -1; }

    type = IFM_TYPE_ID_GET ( ifindex );
    type_name = ifm_get_typestr ( type );

    switch ( type )
    {
        case IFNET_TYPE_VLANIF:
        case IFNET_TYPE_LOOPBACK:
            snprintf ( name, IFNET_NAMESIZE, "%s %d", type_name,
                       IFM_SUBPORT_ID_GET ( ifindex ) );
            break;
        case IFNET_TYPE_CLOCK:
            snprintf ( name, IFNET_NAMESIZE, "%s %d", type_name,
                       IFM_PORT_ID_GET ( ifindex ) );
            break;
        case IFNET_TYPE_TRUNK:
            if ( IFM_IS_SUBPORT ( ifindex ) )
            {
                snprintf ( name, IFNET_NAMESIZE, "%s %d.%d", type_name,
                           IFM_PORT_ID_GET ( ifindex ),
                           IFM_SUBPORT_ID_GET ( ifindex ) );
            }
            else
            {
                snprintf ( name, IFNET_NAMESIZE, "%s %d", type_name,
                           IFM_PORT_ID_GET ( ifindex ) );
            }

            break;
        case IFNET_TYPE_TDM:
        case IFNET_TYPE_STM:
        case IFNET_TYPE_ETHERNET:
		case IFNET_TYPE_GIGABIT_ETHERNET:
		case IFNET_TYPE_XGIGABIT_ETHERNET:
            if ( IFM_IS_SUBPORT ( ifindex ) )
            {
                snprintf ( name, IFNET_NAMESIZE, "%s %d/%d/%d.%d", type_name,
                           IFM_UNIT_ID_GET ( ifindex ),
                           IFM_SLOT_ID_GET ( ifindex ),
                           IFM_PORT_ID_GET ( ifindex ),
                           IFM_SUBPORT_ID_GET ( ifindex ) );
            }
            else
            {
                snprintf ( name,  IFNET_NAMESIZE, "%s %d/%d/%d", type_name,
                           IFM_UNIT_ID_GET ( ifindex ),
                           IFM_SLOT_ID_GET ( ifindex ),
                           IFM_PORT_ID_GET ( ifindex ) );
            }
            break;

		case IFNET_TYPE_TUNNEL:
			snprintf ( name, IFNET_NAMESIZE, "%s %d/%d/%d", type_name,
					   IFM_UNIT_ID_GET ( ifindex ),
					   IFM_SLOT_ID_GET ( ifindex ),
					   IFM_SUBPORT_ID_GET ( ifindex ) );
			break;
		case IFNET_TYPE_VCG:
			snprintf ( name, IFNET_NAMESIZE, "%s %d/%d/%d", type_name,
					   IFM_UNIT_ID_GET ( ifindex ),
					   IFM_SLOT_ID_GET ( ifindex ),
					   IFM_PORT_ID_GET ( ifindex ) );
			break;
		case IFNET_TYPE_E1:
			snprintf ( name, IFNET_NAMESIZE, "%s %d/%d/%d", type_name,
					   IFM_UNIT_ID_GET ( ifindex ),
					   IFM_SLOT_ID_GET ( ifindex ),
					   IFM_PORT_ID_GET ( ifindex ) );
			break;			
        default:
            snprintf ( name,  IFNET_NAMESIZE, "%s", "unknown" );
    }

    return 0;
}


/* 获取网管端口 */
uint32_t ifm_get_nm_port ( void )
{
    struct ifm_usp usp;
    //uint32_t ifindex = 0;

    usp.unit = 1;
    usp.slot = 0;
    usp.port = 1;
    usp.type = IFNET_TYPE_GIGABIT_ETHERNET;
    usp.subtype = IFNET_SUBTYPE_GE;
    usp.sub_port = 0;
    return ifm_get_ifindex_by_usp ( &usp );
}


DEFUN ( physical_ethernet_if_common,
        physical_ethernet_if_common_cmd,
        "interface ethernet USP",
        CLI_INTERFACE_STR
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR )
{
    uint32_t ifindex = 0;
    char *pprompt = NULL;
    struct cmd_node *cnode = NULL;

    ifindex = ifm_get_ifindex_by_name ( "ethernet", ( char * ) argv[0] );
    if ( ifindex == 0 )
    {
        vty_out ( vty, "%%Wrong format,please check out%s", VTY_NEWLINE );
        return CMD_WARNING;
    }


    if ( IFM_IS_SUBPORT ( ifindex ) )
    {
        cnode = vector_slot ( cmdvec, PHYSICAL_SUBIF_NODE );
        if ( cnode == NULL )
        {
            zlog_info ( "%s[%d] Command node PHYSICAL_SUBIF_NODE no install, may erro ???please check it\n", __FUNCTION__, __LINE__ );
            return CMD_SUCCESS;
        }
        vty->node  = PHYSICAL_SUBIF_NODE;
        pprompt = vty->change_prompt;
        if ( pprompt )
        {
            /* format the prompt */
            snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-ethernet%d/%d/%d.%d)#",
                       IFM_UNIT_ID_GET ( ifindex ),
                       IFM_SLOT_ID_GET ( ifindex ),
                       IFM_PORT_ID_GET ( ifindex ),
                       IFM_SUBPORT_ID_GET ( ifindex ) );

        }

    }
    else
    {
        vty->node  = PHYSICAL_IF_NODE;
        pprompt = vty->change_prompt;
        if ( pprompt )
        {
            /* format the prompt */
            snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-ethernet%d/%d/%d)#",
                       IFM_UNIT_ID_GET ( ifindex ),
                       IFM_SLOT_ID_GET ( ifindex ),
                       IFM_PORT_ID_GET ( ifindex ) );

        }
    }
    vty->index = ( void * ) ifindex;

    return CMD_SUCCESS;
}

DEFUN ( physical_gigabit_ethernet_if_common,
		physical_gigabit_ethernet_if_common_cmd,
		"interface gigabitethernet USP",
		CLI_INTERFACE_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR )
{
	int ifindex = 0;
	char *pprompt = NULL;
	struct cmd_node *cnode = NULL;

	if ( argc > 0 )
	{
		ifindex = ifm_get_ifindex_by_name ( "gigabitethernet", argv[0] );
		if ( ifindex == 0 )
		{
			vty_out ( vty, "%%Wrong format,please check out%s", VTY_NEWLINE );
			return CMD_WARNING;
		}

		/*change node*/
		if ( IFM_IS_SUBPORT ( ifindex ) )
		{
			cnode = vector_slot ( cmdvec, PHYSICAL_SUBIF_NODE );
	        if ( cnode == NULL )
	        {
	            return CMD_SUCCESS;
	        }
	        vty->node  = PHYSICAL_SUBIF_NODE;
	        pprompt = vty->change_prompt;
	        if ( pprompt )
	        {
	            /* format the prompt */
	            snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-gigabitethernet%d/%d/%d.%d)#",
	                       IFM_UNIT_ID_GET ( ifindex ),
	                       IFM_SLOT_ID_GET ( ifindex ),
	                       IFM_PORT_ID_GET ( ifindex ),
	                       IFM_SUBPORT_ID_GET ( ifindex ) );

	        }
		}
		else
		{
			vty->node  = PHYSICAL_IF_NODE;
			pprompt = vty->change_prompt;
			if ( pprompt )
			{
				/* format the prompt */
				snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-gigabitethernet%d/%d/%d)#",
						   IFM_UNIT_ID_GET ( ifindex ),
						   IFM_SLOT_ID_GET ( ifindex ),
						   IFM_PORT_ID_GET ( ifindex ) );
			}
		}
	}
	else
	{
		vty_out(vty,"Wrong eth Format!%s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	vty->index = ( void * ) ifindex;

    return CMD_SUCCESS;
}

DEFUN ( physical_xgigabit_ethernet_if_common,
		physical_xgigabit_ethernet_if_common_cmd,
		"interface xgigabitethernet USP",
		CLI_INTERFACE_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR )
{
	int ifindex = 0;
	char *pprompt = NULL;
	struct cmd_node *cnode = NULL;

	if ( argc > 0 )
	{
		ifindex = ifm_get_ifindex_by_name ( "xgigabitethernet", argv[0] );
		if ( ifindex == 0 )
		{
			vty_out ( vty, "%%Wrong format,please check out%s", VTY_NEWLINE );
			return CMD_WARNING;
		}

		/*change node*/
		if ( IFM_IS_SUBPORT ( ifindex ) )
		{
			cnode = vector_slot ( cmdvec, PHYSICAL_SUBIF_NODE );
			if ( cnode == NULL )
			{
				return CMD_SUCCESS;
			}
			vty->node  = PHYSICAL_SUBIF_NODE;
			pprompt = vty->change_prompt;
			if ( pprompt )
			{
				/* format the prompt */
				snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-xgigabitethernet%d/%d/%d.%d)#",
						   IFM_UNIT_ID_GET ( ifindex ),
						   IFM_SLOT_ID_GET ( ifindex ),
						   IFM_PORT_ID_GET ( ifindex ),
						   IFM_SUBPORT_ID_GET ( ifindex ) );

			}
		}
		else
		{
			vty->node  = PHYSICAL_IF_NODE;
			pprompt = vty->change_prompt;
			if ( pprompt )
			{
				/* format the prompt */
				snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-xgigabitethernet%d/%d/%d)#",
						   IFM_UNIT_ID_GET ( ifindex ),
						   IFM_SLOT_ID_GET ( ifindex ),
						   IFM_PORT_ID_GET ( ifindex ) );
			}
		}
	}
	else
	{
		vty_out(vty,"Wrong eth Format!%s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	vty->index = ( void * ) ifindex;

	return CMD_SUCCESS;
}

DEFUN ( clock_if_common,
        clock_if_common_cmd,
        "interface clock <1-2>",
        CLI_INTERFACE_STR
        CLI_INTERFACE_CLOCK_STR
        CLI_INTERFACE_CLOCK_VHELP_STR )

{
    uint32_t ifindex = 0;
    char *pprompt = NULL;

    ifindex = ifm_get_ifindex_by_name ( "clock", ( char * ) argv[0] );
    if ( ifindex == 0 )
    {
        vty_out ( vty, "%%Wrong format,please check out%s", VTY_NEWLINE );
        return CMD_WARNING;
    }

    /*change node*/
    vty->node = CLOCK_NODE;
    pprompt = vty->change_prompt;
    if ( pprompt )
    {
        /* format the prompt */
        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-clock%d)#",
                   IFM_PORT_ID_GET ( ifindex ) );
    }

    vty->index = ( void * ) ifindex;

    return CMD_SUCCESS;
}


DEFUN ( physical_tdm_if_common,
        physical_tdm_if_common_cmd,
        "interface tdm USP",
        CLI_INTERFACE_STR
        CLI_INTERFACE_TDM_STR
        CLI_INTERFACE_TDM_VHELP_STR )

{
    uint32_t ifindex = 0;
    char *pprompt = NULL;
    struct cmd_node *cnode = NULL;

    ifindex = ifm_get_ifindex_by_name ( "tdm", ( char * ) argv[0] );
    if ( ifindex == 0 )
    {
        vty_out ( vty, "%%Wrong format,please check out%s", VTY_NEWLINE );
        return CMD_WARNING;
    }

    /*change node*/
    if ( IFM_IS_SUBPORT ( ifindex ) )
    {
        cnode = vector_slot ( cmdvec, TDM_SUBIF_NODE );
        if ( cnode == NULL )
        {
            zlog_info ( "%s[%d] Command node TDM_SUBIF_NODE no install, may erro ???please check it\n", __FUNCTION__, __LINE__ );
            return CMD_SUCCESS;
        }

        vty->node  = TDM_SUBIF_NODE;
        pprompt = vty->change_prompt;
        if ( pprompt )
        {
            /* format the prompt */
            snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-tdm%d/%d/%d.%d)#",
                       IFM_UNIT_ID_GET ( ifindex ),
                       IFM_SLOT_ID_GET ( ifindex ),
                       IFM_PORT_ID_GET ( ifindex ),
                       IFM_SUBPORT_ID_GET ( ifindex ) );

        }

    }
    else
    {
        vty->node  = TDM_IF_NODE;
        pprompt = vty->change_prompt;
        if ( pprompt )
        {
            /* format the prompt */
            snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-tdm%d/%d/%d)#",
                       IFM_UNIT_ID_GET ( ifindex ),
                       IFM_SLOT_ID_GET ( ifindex ),
                       IFM_PORT_ID_GET ( ifindex ) );

        }
    }
    vty->index = ( void * ) ifindex;

    return CMD_SUCCESS;
}

DEFUN ( physical_if_common,
        physical_stm_if_common_cmd,
        "interface stm USP",
        CLI_INTERFACE_STR
        CLI_INTERFACE_STM_STR
        CLI_INTERFACE_STM_VHELP_STR )

{
    uint32_t ifindex = 0;
    char *pprompt = NULL;
    struct cmd_node *cnode = NULL;

    ifindex = ifm_get_ifindex_by_name ( "stm", ( char * ) argv[0] );
    if ( ifindex == 0 )
    {
        vty_out ( vty, "%%Wrong format,please check out%s", VTY_NEWLINE );
        return CMD_WARNING;
    }

    /*change node*/
    if ( IFM_IS_SUBPORT ( ifindex ) )
    {
        cnode = vector_slot ( cmdvec, TDM_SUBIF_NODE );
        if ( cnode == NULL )
        {
            zlog_info ( "%s[%d] Command node PHYSICAL_SUBIF_NODE no install, may erro ???please check it\n", __FUNCTION__, __LINE__ );
            return CMD_SUCCESS;
        }

        vty->node  = STM_SUBIF_NODE;
        pprompt = vty->change_prompt;
        if ( pprompt )
        {
            /* format the prompt */
            snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-stm%d/%d/%d.%d)#",
                       IFM_UNIT_ID_GET ( ifindex ),
                       IFM_SLOT_ID_GET ( ifindex ),
                       IFM_PORT_ID_GET ( ifindex ),
                       IFM_SUBPORT_ID_GET ( ifindex ) );

        }

    }
    else
    {
        vty->node  = STM_IF_NODE;
        pprompt = vty->change_prompt;
        if ( pprompt )
        {
            /* format the prompt */
            snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-stm%d/%d/%d)#",
                       IFM_UNIT_ID_GET ( ifindex ),
                       IFM_SLOT_ID_GET ( ifindex ),
                       IFM_PORT_ID_GET ( ifindex ) );

        }
    }
    vty->index = ( void * ) ifindex;

    return CMD_SUCCESS;
}


DEFUN ( tunnel_if_common,
        tunnel_if_common_cmd,
        "interface tunnel USP",
        CLI_INTERFACE_STR
        CLI_INTERFACE_TUNNEL_STR
        CLI_INTERFACE_TUNNEL_VHELP_STR )

{
    uint32_t ifindex = 0;
    char *pprompt = NULL;

    ifindex = ifm_get_ifindex_by_name ( "tunnel", ( char * ) argv[0] );
    if ( ifindex == 0 )
    {
        vty_out ( vty, "%%Wrong format,please check out%s", VTY_NEWLINE );
        return CMD_WARNING;
    }

    /*change node*/
    vty->node = TUNNEL_IF_NODE;
    pprompt = vty->change_prompt;
    if ( pprompt )
    {
        /* format the prompt */
        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-tunnel%d/%d/%d)#",
                   IFM_UNIT_ID_GET ( ifindex ),
                   IFM_SLOT_ID_GET ( ifindex ),
                   IFM_SUBPORT_ID_GET ( ifindex ) );
    }
    vty->index = ( void * ) ifindex;

    return CMD_SUCCESS;
}


DEFUN ( loopback_if_common,
        loopback_if_common_cmd,
        "interface loopback <0-128>",
        CLI_INTERFACE_STR
        CLI_INTERFACE_LOOPBACK_STR
        CLI_INTERFACE_LOOPBACK_VHELP_STR )
{
    uint32_t ifindex = 0;
    char *pprompt = NULL;

    ifindex = ifm_get_ifindex_by_name ( "loopback", ( char * ) argv[0] );
    if ( ifindex == 0 )
    {
        return CMD_WARNING;
    }

    /*change node*/
    vty->node = LOOPBACK_IF_NODE;
    pprompt = vty->change_prompt;
    if ( pprompt )
    {
        /* format the prompt */
        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-loopback%d)#",
                   IFM_SUBPORT_ID_GET ( ifindex ) );
    }
    vty->index = ( void * ) ifindex;

    return CMD_SUCCESS;
}


DEFUN ( trunk_if_common,
        trunk_if_common_cmd,
        "interface trunk TRUNK",
        CLI_INTERFACE_STR
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR )

{
    uint32_t ifindex = 0;
    char *pprompt = NULL;
    struct cmd_node *cnode = NULL;

    ifindex = ifm_get_ifindex_by_name ( "trunk", ( char * ) argv[0] );
    if ( ifindex == 0 )
    {
        vty_out ( vty, "%%Wrong format. eg: <1-128>[.<1-4095>]%s", VTY_NEWLINE );
        return CMD_WARNING;
    }

    /*change node*/
    if ( IFM_IS_SUBPORT ( ifindex ) )
    {
        cnode = vector_slot ( cmdvec, TRUNK_SUBIF_NODE );
        if ( cnode == NULL )
        {
            zlog_info ( "%s[%d] Command node TRUNK_SUBIF_NODE no install, may erro ???please check it\n", __FUNCTION__, __LINE__ );
            return CMD_SUCCESS;
        }

        vty->node = TRUNK_SUBIF_NODE;
        pprompt = vty->change_prompt;
        if ( pprompt )
        {
            /* format the prompt */
            snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-trunk%d.%d)#",
                       IFM_PORT_ID_GET ( ifindex ),
                       IFM_SUBPORT_ID_GET ( ifindex ) );
        }
    }
    else
    {
        vty->node = TRUNK_IF_NODE;
        pprompt = vty->change_prompt;
        if ( pprompt )
        {
            /* format the prompt */
            snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-trunk%d)#",
                       IFM_PORT_ID_GET ( ifindex ) );

        }
    }
    vty->index = ( void * ) ifindex;

    return CMD_SUCCESS;

}


DEFUN ( vlanif_if_common,
        vlanif_if_common_cmd,
        "interface vlanif <1-4094>",
        CLI_INTERFACE_STR
        CLI_INTERFACE_VLANIF_STR
        CLI_INTERFACE_VLANIF_VHELP_STR )

{
    uint32_t ifindex = 0;
    char *pprompt = NULL;

    ifindex = ifm_get_ifindex_by_name ( "vlanif", ( char * ) argv[0] );
    if ( ifindex == 0 )
    {
        return CMD_WARNING;
    }

    /*change node*/
    vty->node = VLANIF_NODE;
    pprompt = vty->change_prompt;
    if ( pprompt )
    {
        /* format the prompt */
        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-vlanif%d)#",
                   IFM_SUBPORT_ID_GET ( ifindex ) );
    }

    vty->index = ( void * ) ifindex;

    return CMD_SUCCESS;

}


DEFUN ( e1_if_common,
        e1_if_common_cmd,
        "interface e1 USP",
        CLI_INTERFACE_STR
        CLI_INTERFACE_E1_STR
        CLI_INTERFACE_E1_VHELP_STR )

{
    uint32_t ifindex = 0;
    char *pprompt = NULL;

    ifindex = ifm_get_ifindex_by_name ( "e1", ( char * ) argv[0] );
    if ( ifindex == 0 )
    {
        vty_out ( vty, "%%Wrong format,please check out%s", VTY_NEWLINE );
        return CMD_WARNING;
    }

    /*change node*/
    vty->node = E1_IF_NODE;
    pprompt = vty->change_prompt;
    if ( pprompt )
    {
        /* format the prompt */
        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-e1%d/%d/%d)#",
                   IFM_UNIT_ID_GET ( ifindex ),
                   IFM_SLOT_ID_GET ( ifindex ),
                   IFM_PORT_ID_GET ( ifindex ) );
    }
    vty->index = ( void * ) ifindex;

    return CMD_SUCCESS;
}

DEFUN ( vcg_if_common,
		vcg_if_common_cmd,
		"interface sdhvcg USP",
		CLI_INTERFACE_STR
		CLI_INTERFACE_VCG_STR
		CLI_INTERFACE_VCG_VHELP_STR )
{
	uint32_t ifindex = 0;
	char *pprompt = NULL;

	ifindex = ifm_get_ifindex_by_name ( "sdhvcg", ( char * ) argv[0] );
	if ( ifindex == 0 )
	{
		vty_out ( vty, "%%Wrong format,please check out%s", VTY_NEWLINE );
		return CMD_WARNING;
	}

	/*change node*/
	vty->node = VCG_NODE;
	pprompt = vty->change_prompt;
	if ( pprompt )
	{
		/* format the prompt */
		snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-sdhvcg%d/%d/%d)#",
				   IFM_UNIT_ID_GET ( ifindex ),
				   IFM_SLOT_ID_GET ( ifindex ),
				   IFM_PORT_ID_GET ( ifindex ) );
	}
	vty->index = ( void * ) ifindex;

	return CMD_SUCCESS;
}

/* 注册所有的接口*/
void ifm_common_init ( int ( *config_write ) ( struct vty *vty ) )
{
    install_node ( &physical_if_node, config_write );
    install_node ( &physical_subif_node, NULL );
    install_node ( &tdm_if_node, NULL );
    install_node ( &tdm_subif_node, NULL );
    install_node ( &stm_if_node, NULL );
    install_node ( &stm_subif_node, NULL );
    install_node ( &loopback_if_node, NULL );
    install_node ( &tunnel_if_node, NULL );
    install_node ( &trunk_if_node, NULL );
    install_node ( &trunk_subif_node, NULL );
    install_node ( &vlanif_node, NULL );
	install_node ( &e1if_node, NULL );
	install_node ( &vcgif_node, NULL );

    install_default ( PHYSICAL_IF_NODE );
    install_default ( PHYSICAL_SUBIF_NODE );
    install_default ( TDM_IF_NODE );
    install_default ( TDM_SUBIF_NODE );
    install_default ( STM_IF_NODE );
    install_default ( STM_SUBIF_NODE );
    install_default ( LOOPBACK_IF_NODE );
    install_default ( TUNNEL_IF_NODE );
    install_default ( TRUNK_IF_NODE );
    install_default ( TRUNK_SUBIF_NODE );
    install_default ( VLANIF_NODE );
	install_default ( E1_IF_NODE );
    install_default ( VCG_NODE );

    install_element ( CONFIG_NODE, &physical_ethernet_if_common_cmd, CMD_SYNC );
	install_element ( CONFIG_NODE, &physical_gigabit_ethernet_if_common_cmd, CMD_SYNC);
	install_element ( CONFIG_NODE, &physical_xgigabit_ethernet_if_common_cmd, CMD_SYNC);
    install_element ( CONFIG_NODE, &physical_tdm_if_common_cmd, CMD_SYNC );
    install_element ( CONFIG_NODE, &physical_stm_if_common_cmd, CMD_SYNC );
    install_element ( CONFIG_NODE, &loopback_if_common_cmd, CMD_SYNC );
    install_element ( CONFIG_NODE, &tunnel_if_common_cmd, CMD_SYNC );
    install_element ( CONFIG_NODE, &trunk_if_common_cmd, CMD_SYNC );
    install_element ( CONFIG_NODE, &vlanif_if_common_cmd, CMD_SYNC );
	install_element ( CONFIG_NODE, &e1_if_common_cmd, CMD_SYNC );
	install_element ( CONFIG_NODE, &vcg_if_common_cmd, CMD_SYNC );
}


/* 注册支持 L2 模式的接口*/
void ifm_l2if_init ( int ( *config_write ) ( struct vty *vty ) )
{
    install_node ( &physical_if_node, config_write );
    install_node ( &physical_subif_node, NULL );
    install_node ( &tdm_if_node, NULL );
    install_node ( &tdm_subif_node, NULL );
    install_node ( &stm_if_node, NULL );
    install_node ( &stm_subif_node, NULL );
    install_node ( &trunk_if_node, NULL );
    install_node ( &trunk_subif_node, NULL );

    install_default ( PHYSICAL_IF_NODE );
    install_default ( PHYSICAL_SUBIF_NODE );
    install_default ( TRUNK_IF_NODE );
    install_default ( TRUNK_SUBIF_NODE );
    install_default ( TDM_IF_NODE );
    install_default ( TDM_SUBIF_NODE );
    install_default ( STM_IF_NODE );
    install_default ( STM_SUBIF_NODE );

    install_element ( CONFIG_NODE, &physical_ethernet_if_common_cmd, CMD_SYNC );
	install_element ( CONFIG_NODE, &physical_gigabit_ethernet_if_common_cmd, CMD_SYNC);
	install_element ( CONFIG_NODE, &physical_xgigabit_ethernet_if_common_cmd, CMD_SYNC);
    install_element ( CONFIG_NODE, &physical_tdm_if_common_cmd, CMD_SYNC );
    install_element ( CONFIG_NODE, &physical_stm_if_common_cmd, CMD_SYNC);
    install_element ( CONFIG_NODE, &trunk_if_common_cmd, CMD_SYNC );
}


/* 注册 CES 接口*/
void ifm_ces_init ( int ( *config_write ) ( struct vty *vty ) )
{
    install_node ( &tdm_if_node, config_write );
    install_node ( &tdm_subif_node, NULL );

    install_default ( TDM_IF_NODE );
    install_default ( TDM_SUBIF_NODE );

    install_element ( CONFIG_NODE, &physical_tdm_if_common_cmd, CMD_SYNC );
}

/* 注册 STM 接口*/
void ifm_stm_init( int ( *config_write) ( struct vty *vty))
{
    install_node ( &stm_if_node, config_write );
    install_default ( STM_IF_NODE );
    install_element ( CONFIG_NODE, &physical_stm_if_common_cmd, CMD_SYNC);
}

/* 注册以太物理接口*/
void ifm_port_init ( int ( *config_write ) ( struct vty *vty ) )
{
    install_node ( &physical_if_node, config_write );
    install_node ( &trunk_if_node, NULL );

    install_default ( PHYSICAL_IF_NODE );
    install_default ( TRUNK_IF_NODE );

    install_element ( CONFIG_NODE, &physical_ethernet_if_common_cmd, CMD_SYNC );
	install_element ( CONFIG_NODE, &physical_gigabit_ethernet_if_common_cmd, CMD_SYNC);
	install_element ( CONFIG_NODE, &physical_xgigabit_ethernet_if_common_cmd, CMD_SYNC);
    install_element ( CONFIG_NODE, &trunk_if_common_cmd, CMD_SYNC );
}


/* 注册支持 L3 模式的接口*/
void ifm_l3if_init ( int ( *config_write ) ( struct vty *vty ) )
{
    install_node ( &physical_if_node, config_write );
    install_node ( &physical_subif_node, NULL );
    install_node ( &tdm_if_node, NULL );
    install_node ( &tdm_subif_node, NULL );
    install_node ( &stm_if_node, NULL );
    install_node ( &stm_subif_node, NULL );
    install_node ( &loopback_if_node, NULL );
    install_node ( &tunnel_if_node, NULL );
    install_node ( &trunk_if_node, NULL );
    install_node ( &trunk_subif_node, NULL );
    install_node ( &vlanif_node, NULL );

    install_default ( PHYSICAL_IF_NODE );
    install_default ( PHYSICAL_SUBIF_NODE );
    install_default ( LOOPBACK_IF_NODE );
    install_default ( TUNNEL_IF_NODE );
    install_default ( TRUNK_IF_NODE );
    install_default ( TRUNK_SUBIF_NODE );
    install_default ( VLANIF_NODE );
    install_default ( TDM_IF_NODE );
    install_default ( TDM_SUBIF_NODE );
    install_default ( STM_IF_NODE );
    install_default ( STM_SUBIF_NODE );

    install_element ( CONFIG_NODE, &physical_ethernet_if_common_cmd, CMD_SYNC );
	install_element ( CONFIG_NODE, &physical_gigabit_ethernet_if_common_cmd, CMD_SYNC);
	install_element ( CONFIG_NODE, &physical_xgigabit_ethernet_if_common_cmd, CMD_SYNC);
    install_element ( CONFIG_NODE, &physical_tdm_if_common_cmd, CMD_SYNC );
    install_element ( CONFIG_NODE, &physical_stm_if_common_cmd, CMD_SYNC );
    install_element ( CONFIG_NODE, &loopback_if_common_cmd, CMD_SYNC );
    install_element ( CONFIG_NODE, &tunnel_if_common_cmd, CMD_SYNC );
    install_element ( CONFIG_NODE, &trunk_if_common_cmd, CMD_SYNC );
    install_element ( CONFIG_NODE, &vlanif_if_common_cmd, CMD_SYNC );
}


void ifm_cotroller_e1_init ( int ( *config_write ) ( struct vty *vty ) )
{
    install_node ( &e1if_node, config_write );

    install_default ( E1_IF_NODE );
    install_element ( CONFIG_NODE, &e1_if_common_cmd, CMD_SYNC );
}


/* app call this function to register interface event */
int ifm_event_register ( enum IFNET_EVENT type, int module_id, enum IFNET_EVENT_IFTYPE iftype )
{
    int module_rcv;

   // if ( ipc_common_id < 0 )
   // { ipc_connect_common(); }

    switch ( type )
    {
        /* 接口 IP 地址事件的注册消息发给 route */
        case IFNET_EVENT_IP_ADD:
        case IFNET_EVENT_IP_DELETE:
        case IFNET_EVENT_L3VPN:
            module_rcv = MODULE_ID_ROUTE;
            break;
        default:
            module_rcv = MODULE_ID_IFM;
            break;
    }
	/*ipc_send_common1 ( NULL, 0, 1, module_rcv, module_id, IPC_TYPE_IFM, type, IPC_OPCODE_REGISTER, iftype );*/
	  //ifm_ipc_send(NULL, 0, 1, module_rcv, module_id, IPC_TYPE_IFM, type, IPC_OPCODE_REGISTER, iftype);
	

    return(ipc_send_msg_n2(NULL, 0, 0, module_rcv, module_id, IPC_TYPE_IFM, type, IPC_OPCODE_REGISTER, iftype));
	
}


/* app call this function to no register interface event */
int no_ifm_event_register ( enum IFNET_EVENT type, int module_id, enum IFNET_EVENT_IFTYPE iftype )
{
    int module_rcv;

   // if ( ipc_common_id < 0 )
   // { ipc_connect_common(); }

    switch ( type )
    {
        /* 接口 IP 地址事件的注册消息发给 route */
        case IFNET_EVENT_IP_ADD:
        case IFNET_EVENT_IP_DELETE:
        case IFNET_EVENT_L3VPN:
            module_rcv = MODULE_ID_ROUTE;
            break;
        default:
            module_rcv = MODULE_ID_IFM;
            break;
    }

   // ipc_send_common1 ( NULL, 0, 1, module_rcv, module_id, IPC_TYPE_IFM, type, IPC_OPCODE_UNREGISTER, iftype );
     return(ipc_send_msg_n2(NULL, 0, 0, module_rcv, module_id, IPC_TYPE_IFM, type, IPC_OPCODE_UNREGISTER, iftype));
}

/* app call this function to register specified interface event */
int ifm_event_register_with_ifindex ( enum IFNET_EVENT type, int module_id, u_int32 ifindex )
{
    int module_rcv;

    if(ifindex <= 7)
    {
        return -1;
    }

   // if ( ipc_common_id < 0 )
   // { ipc_connect_common(); }

    switch ( type )
    {
        /* 接口 IP 地址事件的注册消息发给 route */
        case IFNET_EVENT_IP_ADD:
        case IFNET_EVENT_IP_DELETE:
        case IFNET_EVENT_L3VPN:
            module_rcv = MODULE_ID_ROUTE;
            break;
        default:
            module_rcv = MODULE_ID_IFM;
            break;
    }

    //ipc_send_common1 ( NULL, 0, 1, module_rcv, module_id, IPC_TYPE_IFM, type, IPC_OPCODE_REGISTER, ifindex );
     return(ipc_send_msg_n2(NULL, 0, 0, module_rcv, module_id, IPC_TYPE_IFM, type, IPC_OPCODE_REGISTER, ifindex));
    
}


/* 设置接口 IP 地址 */
int ifm_set_l3if ( uint32_t ifindex, struct ifm_l3 *pl3if, enum IFNET_EVENT subtype, int module_id )
{
    int data_len = 0;

    if ( NULL != pl3if )
    {
        data_len = sizeof ( struct ifm_l3 );
    }
    
    return ipc_send_msg_n2(pl3if , data_len, 1, MODULE_ID_ROUTE, module_id, IPC_TYPE_L3IF, subtype, IPC_OPCODE_EVENT, ifindex);

#if 0
	if (subtype == IFNET_EVENT_IP_DELETE)
	{
		/*return ipc_send_common1 ( pl3if, data_len, 1, MODULE_ID_ROUTE, module_id,
                                      IPC_TYPE_L3IF, subtype, IPC_OPCODE_EVENT, ifindex );*/
//return ifm_ipc_send( pl3if , data_len, 1,MODULE_ID_ROUTE, module_id, 
//						IPC_TYPE_L3IF, subtype, IPC_OPCODE_EVENT, ifindex);
        return ipc_send_msg_n2(pl3if , data_len, 1, MODULE_ID_ROUTE, module_id, IPC_TYPE_L3IF, subtype, IPC_OPCODE_EVENT, ifindex);

	}
	else
	{
    	/*return ipc_send_common_wait_ack ( pl3if, data_len, 1, MODULE_ID_ROUTE, module_id,
                                      IPC_TYPE_L3IF, subtype, IPC_OPCODE_EVENT, ifindex );*/
		return ifm_ipc_send_common_wait_ack(pl3if, data_len, 1, MODULE_ID_ROUTE, module_id, 
							IPC_TYPE_L3IF, subtype, IPC_OPCODE_EVENT, ifindex);
	}
#endif    
}


/* èŽ·å–æŽ¥å£çš? IP */
int ifm_get_l3if ( uint32_t ifindex, int module_id, struct ifm_l3 *pdata)
{
	int ret = -1;

    struct ipc_mesg_n *pRevmsg = NULL;
    
    pRevmsg = ipc_sync_send_n2(NULL, 0, 0, MODULE_ID_ROUTE, module_id,
                               IPC_TYPE_L3IF, 0,  IPC_OPCODE_GET, ifindex, 0); 

    if(pRevmsg)
 	{
 	    if((pRevmsg->msghdr.opcode == IPC_OPCODE_REPLY)&&(pRevmsg->msghdr.data_num != 0 ))
        {        	
			ret = 0;
	     	memcpy(pdata, pRevmsg->msg_data, sizeof(struct ifm_l3));
              
        }

        mem_share_free(pRevmsg, module_id);
	}

    return(ret);
}


/* æ‰¹é‡è¿”å›ž ifindex åŽé¢çš? 100 ä¸? l3 æŽ¥å£ä¿¡æ¯ï¼Œè¿”å›žå?¼æ˜¯æ•°ç»„çš„æŒ‡é’ˆï¼Œ*pdata_num è¿”å›žå®žé™…çš„æŽ¥å£æ•°é‡? */
struct ifm_l3 *ifm_get_l3if_bulk ( uint32_t ifindex, int module_id, int *pdata_num )
{
	struct ipc_mesg_n * pMsgRcv = NULL;
   /* struct ipc_mesg *pmesg = ipc_send_common_wait_reply1 ( NULL, 0, 1 , MODULE_ID_ROUTE, module_id,
                             IPC_TYPE_L3IF, 0, IPC_OPCODE_GET_BULK, ifindex );
    if ( NULL != pmesg )
    {
        *pdata_num = pmesg->msghdr.data_num;
        return ( struct ifm_l3 * ) pmesg->msg_data;
    }*/
	pMsgRcv = ipc_sync_send_n2(NULL, 0, 0, MODULE_ID_ROUTE, module_id, IPC_TYPE_L3IF, 0, IPC_OPCODE_GET_BULK, ifindex, 0);

   	if(NULL != pMsgRcv)
    {
        if((pMsgRcv->msghdr.opcode == IPC_OPCODE_REPLY) && (pMsgRcv->msghdr.data_num != 0))
        {
            *pdata_num = pMsgRcv->msghdr.data_num;
             return ( struct ifm_l3 *)pMsgRcv->msg_data;
        }
        else
        {
        	*pdata_num = pMsgRcv->msghdr.data_num;
            mem_share_free(pMsgRcv, module_id);
 
            return NULL;
        }
	}
	else return NULL;
}

/* èŽ·å–æŽ¥å£çš? MAC */
int ifm_get_mac ( uint32_t ifindex, int module_id, uchar *pdata)
{
	int ret = -1;
    struct ipc_mesg_n *pRevmsg = NULL;

    pRevmsg = ipc_sync_send_n2(NULL, 0, 0, MODULE_ID_IFM, module_id,
                               IPC_TYPE_IFM, IFNET_INFO_MAC,  IPC_OPCODE_GET, ifindex, 0); 
    if(pRevmsg)
 	{
 	    if((pRevmsg->msghdr.opcode == IPC_OPCODE_REPLY)&&(pRevmsg->msghdr.data_num != 0 ))
        {        	
			ret = 0;
	     	memcpy(pdata, pRevmsg->msg_data, 6); 	             
        }

        mem_share_free(pRevmsg, module_id);
	}
    
    return(ret);
}


/* èŽ·å–æŽ¥å£çš? link çŠ¶æ?? */
int ifm_get_link ( uint32_t ifindex, int module_id,  uint8_t *pdata)
{
	int ret = -1;
    struct ipc_mesg_n *pRevmsg = NULL;
    /*return ipc_send_common_wait_reply ( NULL, 0, 1 , MODULE_ID_IFM, module_id,
                                        IPC_TYPE_IFM, IFNET_INFO_STATUS, IPC_OPCODE_GET, ifindex );
    ret = ifm_ipc_send_common_wait_reply(pdata, 0, 1, MODULE_ID_IFM, module_id, IPC_TYPE_IFM, 
         				IFNET_INFO_STATUS, IPC_OPCODE_GET, ifindex);*/

    pRevmsg = ipc_sync_send_n2(NULL, 0, 0, MODULE_ID_IFM, module_id,
                               IPC_TYPE_IFM, IFNET_INFO_STATUS,  IPC_OPCODE_GET, ifindex, 0); 
    if(pRevmsg)
 	{
 	    if((pRevmsg->msghdr.opcode == IPC_OPCODE_REPLY)&&(pRevmsg->msghdr.data_num != 0))
        {         	
			ret = 0;
        	*pdata = pRevmsg->msg_data[0];	             
        }

        mem_share_free(pRevmsg, module_id);
	}
    
    return(ret);
}


/* èŽ·å–æŽ¥å£çš? mode  */
int ifm_get_mode ( uint32_t ifindex, int module_id, uint8_t *pdata)
{
	int ret = -1;
    struct ipc_mesg_n *pRevmsg = NULL;
 
    pRevmsg = ipc_sync_send_n2(NULL, 0, 0, MODULE_ID_IFM, module_id,
                               IPC_TYPE_IFM, IFNET_INFO_MODE, IPC_OPCODE_GET, ifindex, 0); 
    if(pRevmsg)
 	{
 	    if((pRevmsg->msghdr.opcode == IPC_OPCODE_REPLY)&&(pRevmsg->msghdr.data_num != 0))
        {     	
			 ret = 0;
        	*pdata = pRevmsg->msg_data[0];		            
        }

        mem_share_free(pRevmsg, module_id);
	}
    
    return(ret);
}


/* èŽ·å–æŽ¥å£çš? mtu  */

int ifm_get_mtu ( uint32_t ifindex, int module_id, uint16_t *pmtu)
{
	int ret = -1;
    struct ipc_mesg_n *pRevmsg = NULL;
 
    pRevmsg = ipc_sync_send_n2(NULL, 0, 0, MODULE_ID_IFM, module_id,
                               IPC_TYPE_IFM, IFNET_INFO_MTU, IPC_OPCODE_GET, ifindex, 0); 
    if(pRevmsg)
 	{
 	    if((pRevmsg->msghdr.opcode == IPC_OPCODE_REPLY)&&(pRevmsg->msghdr.data_num != 0))
        {         	
			 ret = 0;
        	*pmtu = *((uint16_t*)pRevmsg->msg_data);
            
        }

        mem_share_free(pRevmsg, module_id);
	}
    
	return ret;
}

/*èŽ·å–æŽ¥å£å…¨éƒ¨ä¿¡æ¯ struct ifm_info*/
int ifm_get_all_info ( uint32_t ifindex, int module_id, struct ifm_info *pdata)
{
	int ret = -1;

    struct ipc_mesg_n *pRevmsg = NULL;
    
    pRevmsg = ipc_sync_send_n2(NULL, 0, 0, MODULE_ID_IFM, module_id,
                               IPC_TYPE_IFM, IFNET_INFO_MAX, IPC_OPCODE_GET, ifindex, 0); 

    if(pRevmsg)
 	{
 	    if((pRevmsg->msghdr.opcode == IPC_OPCODE_REPLY)&&(pRevmsg->msghdr.data_num != 0))
        {
			ret = 0;
	    	memcpy(pdata, pRevmsg->msg_data, sizeof(struct ifm_info));              
        }

        mem_share_free(pRevmsg, module_id);
	}

    return(ret);
}

/* 批量返回 ifindex 后面的 *pdata_num 个接口信息，返回值是数组的指针，*pdata_num 返回实际的接口数量 */

void *ifm_get_bulk ( uint32_t ifindex, int module_id, int *pdata_num )
{
    struct ipc_mesg_n *pRevmsg = NULL;
    
    pRevmsg = ipc_sync_send_n2(NULL, 0, 0, MODULE_ID_IFM, module_id,
                               IPC_TYPE_IFM, IFNET_INFO_MAX, IPC_OPCODE_GET_BULK, ifindex, 0); 

    if(pRevmsg)
 	{
 	    if((pRevmsg->msghdr.opcode == IPC_OPCODE_REPLY) && (pRevmsg->msghdr.data_num != 0))
        {  
             *pdata_num = pRevmsg->msghdr.data_num;
		     return((void *)pRevmsg->msg_data);
        }
        else 
        {
        	*pdata_num = pRevmsg->msghdr.data_num;
             mem_share_free(pRevmsg, module_id);
             return(NULL);
        }
	}
    else return(NULL);
}
void * ifm_get_speed_bulk(uint32_t * ifindex,int module_id, int *pdata_num)
{
	/*struct ipc_mesg *pmesg = ipc_send_hal_wait_reply1(NULL, 0, 1, MODULE_ID_HAL, module_id,
			IPC_TYPE_IFM, IFNET_INFO_SPEED_BULK, IPC_OPCODE_GET_BULK, 0);
	
	if( pmesg )
	{
		 *pdata_num = pmesg->msghdr.data_num;
		 *ifindex = pmesg->msghdr.msg_index;
         return ( void * ) pmesg->msg_data;
	}*/
	struct ipc_mesg_n *pRevmsg = NULL;

	pRevmsg = ipc_sync_send_n2(NULL, 0, 0, MODULE_ID_HAL, module_id, IPC_TYPE_IFM, 
		IFNET_INFO_SPEED_BULK, IPC_OPCODE_GET_BULK, 0, 0);
	
	 if(pRevmsg)
 	{
 	    if((pRevmsg->msghdr.opcode == IPC_OPCODE_REPLY) && (pRevmsg->msghdr.data_num != 0))
        {  
             *pdata_num = pRevmsg->msghdr.data_num;
		     return((void *)pRevmsg->msg_data);
        }
        else 
        {
        	*pdata_num = pRevmsg->msghdr.data_num;
             mem_share_free(pRevmsg, module_id);
             return(NULL);
        }
	}
    else return(NULL);
	
	
}

void* ifm_get_sfp_if_bulk( uint32_t ifindex, int module_id, int * pdata_num )
{
    struct ipc_mesg_n *pRevmsg = NULL;
      
    pRevmsg = ipc_sync_send_n2(NULL, 0, 0, MODULE_ID_IFM, module_id,
                               IPC_TYPE_IFM, IFNET_INFO_SFP, IPC_OPCODE_GET_BULK, ifindex, 0); 
    
    if(pRevmsg)
    {
        if((pRevmsg->msghdr.opcode == IPC_OPCODE_REPLY) && (pRevmsg->msghdr.data_num != 0))
        {  
             *pdata_num = pRevmsg->msghdr.data_num;
             return((void *)pRevmsg->msg_data);
        }
        else 
        {
        	*pdata_num = pRevmsg->msghdr.data_num;
             mem_share_free(pRevmsg, module_id);
             return(NULL);
        }
    }
    else return(NULL);
}
void * ifm_get_subencap_bulk( uint32_t ifindex, int module_id, int * pdata_num )
{
	struct ipc_mesg_n * pRevmsg = NULL;
	
	pRevmsg = ipc_sync_send_n2(NULL, 0, 0, MODULE_ID_IFM, module_id, 
  			IPC_TYPE_IFM, IFNET_INFO_ENCAP, IPC_OPCODE_GET_BULK, ifindex, 0);
	
	if(pRevmsg)
    {
        if((pRevmsg->msghdr.opcode == IPC_OPCODE_REPLY) && (pRevmsg->msghdr.data_num != 0))
        {  
             *pdata_num = pRevmsg->msghdr.data_num;
             return((void *)pRevmsg->msg_data);
        }
        else 
        {
			*pdata_num = pRevmsg->msghdr.data_num;
			mem_share_free(pRevmsg, module_id);
			return(NULL);
        }
    }
    else return(NULL);
}

/*批量返回ifindex后面的*pdata_num个接口统计信息，返回值是数组的指针，*pdata_num 返回实际的接口数量*/
struct ifm_counter_bulk *ifm_get_count_bulk(uint32_t ifindex, int module_id, int *pdata_num)
{
	struct ipc_mesg_n * pRevmsg = NULL;
	
	pRevmsg = ipc_sync_send_n2(NULL, 0, 0, MODULE_ID_HAL, module_id, 
  			IPC_TYPE_IFM, IFNET_INFO_COUNTER, IPC_OPCODE_GET_BULK, ifindex, 0);
	
	if(pRevmsg)
    {
        if((pRevmsg->msghdr.opcode == IPC_OPCODE_REPLY) && (pRevmsg->msghdr.data_num != 0))
        {  
             *pdata_num = pRevmsg->msghdr.data_num;
             return((struct ifm_counter_bulk *)pRevmsg->msg_data);
        }
        else 
        {
        	*pdata_num = pRevmsg->msghdr.data_num;
             mem_share_free(pRevmsg, module_id);
             return(NULL);
        }
    }
    else return(NULL);
}
/* 批量返回 ifindex 后面的 100 个接口信息，返回值是数组的指针，*pdata_num 返回实际的接口数量 */
struct ifm_sfp_info *ifm_get_sfp_bulk ( uint32_t ifindex, int module_id, int *pdata_num )
{
    struct ipc_mesg_n * pRevmsg = NULL;
        
    pRevmsg = ipc_sync_send_n2(NULL, 0, 0, MODULE_ID_HAL, module_id, 
            IPC_TYPE_SFP, IFNET_INFO_SFP_ALL, IPC_OPCODE_GET_BULK, ifindex, 0);
    
    if(pRevmsg)
    {
        if((pRevmsg->msghdr.opcode == IPC_OPCODE_REPLY) && (pRevmsg->msghdr.data_num != 0))
        {  
             *pdata_num = pRevmsg->msghdr.data_num;
             return((struct ifm_sfp_info  *)pRevmsg->msg_data);
        }
        else 
        {
        	*pdata_num = pRevmsg->msghdr.data_num;
             mem_share_free(pRevmsg, module_id);
             return(NULL);
        }
    }
    else return(NULL);
}

int ifm_get_port_info ( uint32_t ifindex, int module_id, struct ifm_port *pdata)
{
    int ret = -1;
    
    struct ipc_mesg_n *pRevmsg = NULL;
    
    pRevmsg = ipc_sync_send_n2(NULL, 0, 0, MODULE_ID_IFM, module_id,
                               IPC_TYPE_IFM, IFNET_INFO_PORT_INFO, IPC_OPCODE_GET, ifindex, 0); 
    
    if(pRevmsg)
    {
        if((pRevmsg->msghdr.opcode == IPC_OPCODE_REPLY)&&(pRevmsg->msghdr.data_num != 0))
        {  
             ret = 0;
             memcpy(pdata, pRevmsg->msg_data, sizeof(struct ifm_port)); 
        }
    
        mem_share_free(pRevmsg, module_id);
    }
    
    return(ret);
}

int ifm_set_enable ( uint32_t ifindex, int module_id, uint8_t enable )
{
    /*return ipc_send_common1 ( &enable, 1, 1 , MODULE_ID_IFM, module_id,
                              IPC_TYPE_IFM, IFNET_INFO_SHUTDOWN, IPC_OPCODE_UPDATE, ifindex );*/
                              
    //return ifm_ipc_send(&enable, 1, 1, MODULE_ID_IFM, module_id, IPC_TYPE_IFM, 
    //				IFNET_INFO_SHUTDOWN, IPC_OPCODE_UPDATE, ifindex);
    return ipc_send_msg_n2(&enable, 1, 1, MODULE_ID_IFM, module_id, IPC_TYPE_IFM, IFNET_INFO_SHUTDOWN, IPC_OPCODE_UPDATE, ifindex);
}

int ifm_set_block ( uint32_t ifindex, int module_id, uint8_t status )
{
    /*return ipc_send_hal ( &status, 1, 1 , MODULE_ID_HAL, module_id,
                          IPC_TYPE_IFM, IFNET_INFO_BLOCK, IPC_OPCODE_UPDATE, ifindex );*/
    //return ifm_ipc_send(&status, 1,1, MODULE_ID_HAL, module_id, 
    //		         IPC_TYPE_IFM, IFNET_INFO_BLOCK, IPC_OPCODE_UPDATE, ifindex);
    return ipc_send_msg_n2(&status, 1, 1, MODULE_ID_HAL, module_id, IPC_TYPE_IFM, IFNET_INFO_BLOCK, IPC_OPCODE_UPDATE, ifindex);
}
/* 传入物理接口的 ifindex 和 vlan，创建 l3 子接口 */
int ifm_set_subif ( uint32_t ifindex, int svlan, int cvlan, int module_id )
{
    struct ifm_encap encap;

    memset ( &encap, 0, sizeof ( struct ifm_encap ) );

    if ( svlan != 0 && cvlan == 0 )
    {
        encap.type = IFNET_ENCAP_DOT1Q;
    }
    else if ( svlan != 0 && cvlan != 0 )
    {
        encap.type = IFNET_ENCAP_QINQ;
    }
    encap.svlan.vlan_start = svlan;
    encap.svlan.vlan_end   = svlan;
    encap.cvlan.vlan_start = cvlan;
    encap.cvlan.vlan_end   = cvlan;


    /*return ipc_send_common_wait_ack ( &encap, sizeof ( struct ifm_encap ), 1 , MODULE_ID_IFM, module_id,
                                      IPC_TYPE_IFM, IFNET_INFO_SUBIF, IPC_OPCODE_ADD, ifindex );*/
    //return ifm_ipc_send_common_wait_ack( &encap, sizeof ( struct ifm_encap ), 1, MODULE_ID_IFM, 
    //module_id, IPC_TYPE_IFM, IFNET_INFO_SUBIF, IPC_OPCODE_ADD, ifindex);

    return ipc_send_msg_n2(&encap, sizeof ( struct ifm_encap ), 1, MODULE_ID_IFM, module_id, IPC_TYPE_IFM, IFNET_INFO_SUBIF, IPC_OPCODE_ADD, ifindex);

}

/* 传入物理接口的 ifindex 和 vlan，创建 l2 子接口 */

int ifm_set_subif_l2 ( uint32_t ifindex, int svlan, int cvlan, int module_id )
{
    struct ifm_encap encap;

    memset ( &encap, 0, sizeof ( struct ifm_encap ) );

    if ( svlan != 0 && cvlan == 0 )
    {
        encap.type = IFNET_ENCAP_DOT1Q;
    }
    else if ( svlan != 0 && cvlan != 0 )
    {
        encap.type = IFNET_ENCAP_QINQ;
    }
    encap.svlan.vlan_start = svlan;
    encap.svlan.vlan_end   = svlan;
    encap.cvlan.vlan_start = cvlan;
    encap.cvlan.vlan_end   = cvlan;


    /*return ipc_send_common_wait_ack ( &encap, sizeof ( struct ifm_encap ), 1 , MODULE_ID_IFM, module_id,
                                      IPC_TYPE_IFM, IFNET_INFO_SUBIF2, IPC_OPCODE_ADD, ifindex );*/
  //return ifm_ipc_send_common_wait_ack( &encap, sizeof ( struct ifm_encap ), 1, MODULE_ID_IFM, 
  //	module_id, IPC_TYPE_IFM, IFNET_INFO_SUBIF2, IPC_OPCODE_ADD, ifindex);
    return ipc_send_msg_n2(&encap, sizeof ( struct ifm_encap ), 1, MODULE_ID_IFM, module_id, IPC_TYPE_IFM, IFNET_INFO_SUBIF2, IPC_OPCODE_ADD, ifindex);
}


int ifm_unset_subif_l2 ( uint32_t ifindex , int module_id )
{
    /*return ipc_send_common_wait_ack ( NULL, 0, 1 , MODULE_ID_IFM, module_id,
                                      IPC_TYPE_IFM, IFNET_INFO_SUBIF2, IPC_OPCODE_DELETE, ifindex );*/
  //return ifm_ipc_send_common_wait_ack( NULL, 0, 1, MODULE_ID_IFM, module_id,
  //		 IPC_TYPE_IFM, IFNET_INFO_SUBIF2, IPC_OPCODE_DELETE, ifindex);
    return ipc_send_msg_n2(NULL, 0, 0, MODULE_ID_IFM, module_id, IPC_TYPE_IFM, IFNET_INFO_SUBIF2, IPC_OPCODE_DELETE, ifindex);
}


/* 批量返回接口上的qos mapping信息，返回值是数组的指针，*pdata_num 返回实际的数据数量 */

struct qos_entry_snmp *ifm_get_qos_bulk(uint32_t ifindex, int module_id, int*pdata_num)
{
    struct ipc_mesg_n * pRevmsg = NULL;
          
    pRevmsg = ipc_sync_send_n2(NULL, 0, 0, MODULE_ID_IFM, module_id, 
            IPC_TYPE_QOSIF, QOS_INFO_MAX, IPC_OPCODE_GET_BULK, ifindex, 0);    
   
	if(pRevmsg)
	{
		if((pRevmsg->msghdr.opcode == IPC_OPCODE_REPLY)&&(pRevmsg->msghdr.data_num != 0))
		{
			*pdata_num = pRevmsg->msghdr.data_num;
			return((struct qos_entry_snmp *)pRevmsg->msg_data);			
		}
		else
		{
			*pdata_num = pRevmsg->msghdr.data_num;
             mem_share_free(pRevmsg, module_id);
             return(NULL);
		}
	}

	else return(NULL);
}

int  ifm_get_port_count (int module_id, uint32_t *pdata)
{
	int ret = -1;
	struct ipc_mesg_n * pRevmsg = NULL;
	
    ret = ipc_sync_send_n2(0, 0, 0, MODULE_ID_IFM, module_id, 
    			IPC_TYPE_IFM,  IFNET_INFO_PORT_COUNT, IPC_OPCODE_GET, 0, 0);
    
	if(pRevmsg)
 	{
 	    if((pRevmsg->msghdr.opcode == IPC_OPCODE_REPLY) && (pRevmsg->msghdr.data_num !=0 ))
        {  
             ret  = 0;
            *pdata = *((uint32_t*)pRevmsg->msg_data);
        }

        mem_share_free(pRevmsg, module_id);
	}
    
	return ret;             
}
void * ifm_get_statistics_bulk(uint32_t ifindex,int module_id,int* pdata_num)
{
	#if 0
	struct ipc_mesg * pmesg = ipc_send_common_wait_reply1(NULL, 0, 1, MODULE_ID_IFM, module_id,
		IPC_TYPE_IFM, IFNET_INFO_COUNTER, IPC_OPCODE_GET_BULK, ifindex);
	if( pmesg && ( pmesg->msghdr.data_num != 0 ))
	{
		*pdata_num = pmesg->msghdr.data_num;
		 return ( void * )pmesg->msg_data;
	}
	#endif
	struct ipc_mesg_n *pRevmsg = NULL;
	pRevmsg = ipc_sync_send_n2(NULL, 0, 0, MODULE_ID_IFM, module_id,
                               IPC_TYPE_IFM, IFNET_INFO_COUNTER, IPC_OPCODE_GET_BULK, ifindex, 0);
	if(pRevmsg)
 	{
 	    if((pRevmsg->msghdr.opcode == IPC_OPCODE_REPLY) && (pRevmsg->msghdr.data_num != 0))
        {  
             *pdata_num = pRevmsg->msghdr.data_num;
		     return((void *)pRevmsg->msg_data);
        }
        else 
        {
        	*pdata_num = pRevmsg->msghdr.data_num;
             mem_share_free(pRevmsg, module_id);
             return(NULL);
        }
	}
    else return(NULL);
}

/************************************************************
* Function : ifm_ipc_send
* return	: 0 ok , not 0 error   
* description: send errcode as reply because of no local data
************************************************************/
#if 0
int ifm_ipc_send(void * pdata,int data_len,int data_num, int module_id,int sender_id,
				 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode,uint32_t msg_index)
{
	int ret = 0;
	struct ipc_mesg_n *pMsgSnd = NULL;
	if( data_len && pdata == NULL )
	{
		return -1;
	}
	pMsgSnd = mem_share_malloc( sizeof(struct ipc_msghdr_n ) + data_len, MODULE_ID_IFM );
	if( NULL == pMsgSnd )
	{
		return -1;
	}
	pMsgSnd->msghdr.data_len = data_len;
	pMsgSnd->msghdr.data_num = data_num;
	pMsgSnd->msghdr.module_id = module_id;
	pMsgSnd->msghdr.sender_id = sender_id;
	pMsgSnd->msghdr.msg_type = msg_type;
	pMsgSnd->msghdr.msg_subtype = subtype;
	pMsgSnd->msghdr.opcode = opcode;
	pMsgSnd->msghdr.msg_index = msg_index;

	if( pdata )
	{
		memcpy(pMsgSnd->msg_data, pdata, data_len);
	}
	ret = ipc_send_msg_n1(pMsgSnd, sizeof( struct ipc_msghdr_n ) + data_len );
	if( ret )
	{
		mem_share_free( pMsgSnd, MODULE_ID_IFM );
	}
	return 0;

}					 
/************************************************************
* Function : ifm_ipc_send_common_wait_ack
* return	: 0 ok , not 0 error   
* description: send errcode as reply because of no local data
************************************************************/
int ifm_ipc_send_common_wait_ack(void* pdata,uint32_t data_len,uint16_t data_num, int module_id, int sender_id,
	 enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
	int ret = 0;
	int rcvlen = 0;
	struct ipc_mesg_n * pSndMsg = NULL;
	struct ipc_mesg_n * pRcvMsg = NULL;

	pSndMsg = mem_share_malloc( sizeof( struct ipc_msghdr_n ) + data_len, MODULE_ID_IFM );
	if( pSndMsg == NULL )
	{
		return -1;
	}
	pSndMsg->msghdr.data_len	 = data_len;
	pSndMsg->msghdr.data_num	 = data_num;
	pSndMsg->msghdr.module_id	 = module_id ;
	pSndMsg->msghdr.sender_id	 = sender_id;
	pSndMsg->msghdr.msg_type	 = msg_type;
	pSndMsg->msghdr.msg_subtype = msg_subtype;
	pSndMsg->msghdr.opcode 	 = opcode;
	pSndMsg->msghdr.msg_index	 = msg_index;

	if(pdata)
	{
		memcpy(pSndMsg->msg_data,pdata,data_len);
	}

	/*send info*/
	ret = ipc_sync_send_n1(pSndMsg, sizeof(struct ipc_msghdr_n) + data_len, &pRcvMsg, &rcvlen, 5000);

	switch(ret)
	{
		case -1:
		{
			/*send fail*/
			mem_share_free( pSndMsg, MODULE_ID_IFM );
			return -1;
		}
		case -2:
		{
			/*receive fail*/
			return -1; 		 
		}
		case 0:
		{
			/*receive success*/
			if( pRcvMsg != NULL )
			{
				/*if noack return errcode*/
				if( IPC_OPCODE_NACK == pRcvMsg->msghdr.opcode )
				{
					memcpy(&ret,pRcvMsg->msg_data,sizeof(ret));				 
				}
				else if( IPC_OPCODE_ACK == pRcvMsg->msghdr.opcode )
				{
					ret = 0;
				}
				mem_share_free( pRcvMsg, MODULE_ID_IFM);				 
			}
			else
			{
				return -1;
			}
			return ret;
		}
		default :
		return -1;
	}
	return ret;
}
/************************************************************
* Function : ifm_ipc_send_common_wait_reply
* return	: 0 ok , not 0 error   
* description: send errcode as reply because of no local data
************************************************************/
int ifm_ipc_send_common_wait_reply(void* pdata,uint32_t data_len,uint16_t data_num, int module_id, int sender_id,
	 enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
	int iRetv = 0;
	int iRepL = 0;

		struct ipc_mesg_n * pMsgSnd = NULL;
	struct ipc_mesg_n * pMsgRcv = NULL;

	pMsgSnd = mem_share_malloc(sizeof(struct ipc_mesg_n), MODULE_ID_IFM);
	if( pMsgSnd == NULL )
	{
		return -1;
	}
	memset(pMsgSnd,0,sizeof(struct ipc_mesg_n));

	/*fill msg header*/

	pMsgSnd->msghdr.data_len		 = data_len;
	pMsgSnd->msghdr.data_num		 = data_num;
	pMsgSnd->msghdr.module_id		 = module_id;
	pMsgSnd->msghdr.sender_id		 = sender_id;
	pMsgSnd->msghdr.msg_type		 = msg_type;
	pMsgSnd->msghdr.msg_subtype	 = msg_subtype;
	pMsgSnd->msghdr.opcode 		 = opcode;
	pMsgSnd->msghdr.msg_index		 = msg_index;

	/*send info send fail:-1 rece fail -2 ok :0*/
	iRetv = ipc_sync_send_n1(pMsgSnd, sizeof(struct ipc_mesg_n), &pMsgRcv, &iRepL, 5000);

	switch( iRetv )
	{
		case -1:
		{
			/*send fail*/
			mem_share_free( pMsgSnd, MODULE_ID_IFM );
			return -1;
		}
		case -2:
		{
			/*receive fail*/
			return -1;
		}
		case 0:
		{
			/*receive success*/
			if( pMsgRcv == NULL )
			{
				return -1;
			}
			else
			{
				memcpy(pdata, pMsgRcv->msg_data, \
				pMsgRcv->msghdr.data_len >= data_len ? data_len : pMsgRcv->msghdr.data_len);				 
				mem_share_free(pMsgRcv, MODULE_ID_IFM);
				return 0;
			}
		}
		default:
		return -1;
	}

}

     
/************************************************************
* Function : ifm_ipc_send_common_wait_reply1
* return	: 0 ok , not 0 error   
* description: send errcode as reply because of no local data
************************************************************/
struct ipc_mesg_n * ifm_ipc_send_common_wait_reply1(void* pdata,uint32_t data_len,uint16_t data_num, int module_id, int sender_id,
	 enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
	int iRetv = 0;
	int iRepL = 0;

	struct ipc_mesg_n * pMsgSnd = NULL;
	struct ipc_mesg_n * pMsgRcv = NULL;
	//struct ipc_mesg_n	 pMsgAck ;

	pMsgSnd = mem_share_malloc(sizeof(struct ipc_mesg_n), MODULE_ID_IFM);
	if( pMsgSnd == NULL )
	{
		return NULL;
	}
	memset(pMsgSnd,0,sizeof(struct ipc_mesg_n));

	/*fill msg header*/

	pMsgSnd->msghdr.data_len		 = data_len;
	pMsgSnd->msghdr.data_num		 = data_num;
	pMsgSnd->msghdr.module_id		 = module_id;
	pMsgSnd->msghdr.sender_id		 = sender_id;
	pMsgSnd->msghdr.msg_type		 = msg_type;
	pMsgSnd->msghdr.msg_subtype	 = msg_subtype;
	pMsgSnd->msghdr.opcode 		 = opcode;
	pMsgSnd->msghdr.msg_index		 = msg_index;

	/*send info send fail:-1 rece fail -2 ok :0*/
	iRetv = ipc_sync_send_n1(pMsgSnd, sizeof(struct ipc_mesg_n), &pMsgRcv, &iRepL, 5000);

	switch( iRetv )
	{
		case -1:
		{
			/*send fail*/
			mem_share_free( pMsgSnd, MODULE_ID_IFM );
			return NULL;
		}
		case -2:
		{
			/*receive fail*/
			return NULL;
		}
		case 0:
		{
			/*receive success*/
			if( pMsgRcv == NULL )
			{
				return NULL;
			}
			else
			{
				//memcpy(&pMsgAck,pMsgRcv,sizeof(struct ipc_mesg_n)+pMsgRcv->msghdr.data_len);				 
				//mem_share_free(pMsgRcv, MODULE_ID_IFM);
				return pMsgRcv;
			}
		}
		default:
			return NULL;
	}

}

     
/************************************************************
* Function : ifm_send_hal_wait_reply1
* return	: 0 ok , not 0 error   
* description: send set msg to hal and wait reply, used to replace "ipc_send_hal_wait_reply1"
************************************************************/
struct ipc_mesg_n* ifm_send_hal_wait_reply1(void * pdata,uint32_t data_len,uint16_t data_num,int module_id,
				int sender_id,enum IPC_TYPE msg_type,uint16_t msg_subtype,enum IPC_OPCODE opcode,uint32_t msg_index)
{
	int iRetv = 0;
	int iRepL = 0;

	struct ipc_mesg_n * pMsgSnd = NULL;
	struct ipc_mesg_n * pMsgRcv = NULL;
	struct ipc_mesg_n	 pMsgAck;

	pMsgSnd = mem_share_malloc( sizeof( struct ipc_mesg_n), MODULE_ID_IFM );
	if( pMsgSnd == NULL )
	{
	return NULL;
	}
	memset(pMsgSnd, 0,sizeof(struct ipc_mesg_n));

	/*fill msg header*/
	pMsgSnd->msghdr.data_len		= data_len;
	pMsgSnd->msghdr.data_num		= data_num;
	pMsgSnd->msghdr.module_id		= module_id;
	pMsgSnd->msghdr.sender_id		= sender_id;
	pMsgSnd->msghdr.msg_type		= msg_type;
	pMsgSnd->msghdr.msg_subtype	= msg_subtype;
	pMsgSnd->msghdr.opcode 		= opcode;
	pMsgSnd->msghdr.msg_index		= msg_index;

	/*send info send fail -1 recv fail -2 ok 0*/
	iRetv = ipc_sync_send_n1(pMsgSnd, sizeof(struct ipc_mesg_n), &pMsgRcv, &iRepL, 5000);
	switch(iRetv)
	{
		/*send fail*/
		case -1:
		{
			mem_share_free(pMsgSnd, MODULE_ID_IFM);

			return NULL;
		}
		case -2:
		{
			/*receive fail*/

			return NULL;
		}
		case 0:
		{
			/*receive success*/
			if(pMsgRcv != NULL)
			{
				memcpy(&pMsgAck,pMsgRcv,sizeof(struct ipc_mesg_n)+pMsgRcv->msghdr.data_len);

				/*receive success free memory*/
				mem_share_free(pMsgRcv, MODULE_ID_IFM);
				return &pMsgAck;
			}
		}
		default:
		return NULL;				 
	}

}

#endif





#if 0
void * l2if_get_storm_suppress_bulk( uint32_t ifindex, int module_id, int *pdata_num )
{
	struct ipc_mesg *pmesg = ipc_send_common_wait_reply1 ( NULL, 0, 1 , MODULE_ID_L2, module_id,
                             IPC_TYPE_STORM_CONTROL, IFNET_INFO_MAX, IPC_OPCODE_GET_BULK, ifindex );
    if ( pmesg )
    {
        *pdata_num = pmesg->msghdr.data_num;
        return ( void * ) pmesg->msg_data;
    }

    return NULL;
}

void * l2if_get_dot1q_tunnel_bulk( uint32_t ifindex, int module_id, int *pdata_num )
{
	struct ipc_mesg *pmesg = ipc_send_common_wait_reply1 ( NULL, 0, 1 , MODULE_ID_L2, module_id,
								 IPC_TYPE_DOT1Q_TUNNEL, IFNET_INFO_MAX, IPC_OPCODE_GET_BULK, ifindex );
	if ( pmesg )
	{
		*pdata_num = pmesg->msghdr.data_num;
		return ( void * ) pmesg->msg_data;
	}

	return NULL;

}
#endif

void * l2if_get_storm_suppress_bulk( uint32_t ifindex, int module_id, int *pdata_num )
{	
	struct ipc_mesg_n *pRevmsg = NULL;

	pRevmsg = ipc_sync_send_n2(NULL, 0, 0,MODULE_ID_L2, module_id, IPC_TYPE_STORM_CONTROL, 
			IFNET_INFO_MAX, IPC_OPCODE_GET_BULK, ifindex,0);
	if( pRevmsg )
	{
		if((pRevmsg->msghdr.opcode == IPC_OPCODE_REPLY) && (pRevmsg->msghdr.data_num != 0))
        {  
             *pdata_num = pRevmsg->msghdr.data_num;
		     return((void *)pRevmsg->msg_data);
        }
        else 
        {
        	*pdata_num = pRevmsg->msghdr.data_num;
             mem_share_free(pRevmsg, module_id);
             return(NULL);
        }
	}
	else return(NULL);
}

void * l2if_get_dot1q_tunnel_bulk( uint32_t ifindex, int module_id, int *pdata_num )
{
	struct ipc_mesg_n *pRevmsg = NULL;

	pRevmsg = ipc_sync_send_n2(NULL, 0, 0,MODULE_ID_L2, module_id, IPC_TYPE_DOT1Q_TUNNEL, 
			IFNET_INFO_MAX, IPC_OPCODE_GET_BULK, ifindex, 0);
	if( pRevmsg )
	{
		if((pRevmsg->msghdr.opcode == IPC_OPCODE_REPLY) && (pRevmsg->msghdr.data_num != 0))
        {  
             *pdata_num = pRevmsg->msghdr.data_num;
		     return((void *)pRevmsg->msg_data);
        }
        else 
        {
        	*pdata_num = pRevmsg->msghdr.data_num;
             mem_share_free(pRevmsg, module_id);
             return(NULL);
        }
	}
	else return(NULL);

}


