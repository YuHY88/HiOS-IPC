#include <memory.h>
#include <lib/linklist.h>
#include <lib/msg_ipc.h>
#include <lib/module_id.h>
#include <lib/zassert.h>
#include <lib/log.h>
#include "ifm_register.h"
#include "ifm_message.h"
#include "ifm.h"


struct list *ifm_event_list[IFNET_EVENT_MAX][IFNET_TYPE_MAX];

static int ifm_event_add ( int module_id, uint8_t type, uint8_t subtype )
{
    struct list *plist;

    if ( type >= IFNET_EVENT_MAX )
    {
        zlog_err ( "%s[%d] : pif is type(%d) > max(%d)", __FUNCTION__, __LINE__,
                   type, IFNET_EVENT_MAX );
        assert ( 0 );
        return -1;
    }

    if ( subtype >= IFNET_TYPE_MAX )
    {
        zlog_err ( "%s[%d] : pif is subtype(%d) > max(%d)", __FUNCTION__, __LINE__,
                   subtype, IFNET_TYPE_MAX );
        assert ( 0 );
        return -1;
    }

    plist = ifm_event_list[type][subtype];
    if ( plist == NULL )
    {
        zlog_err ( "%s[%d] : ifm_event_list[%d][%d] is null", __FUNCTION__, __LINE__,
                   type, subtype );
        assert ( 0 );
        return -1;
    }

    if ( listnode_lookup ( plist, ( void * ) module_id ) == NULL )
    {
        listnode_add ( plist, ( void * ) module_id );
    }

    return 0;
}


int ifm_register_msg ( int module_id, uint8_t type, uint8_t subtype )
{
    if ( type >= IFNET_EVENT_MAX )
    {
        zlog_err ( "%s[%d] : type(%d) > max(%d)", __FUNCTION__, __LINE__,
                   type, IFNET_EVENT_MAX );

        assert ( 0 );
        return -1;
    }

    if ( subtype > IFNET_IFTYPE_ALL )
    {
        zlog_err ( "%s[%d] : subtype(%d) > max(%d)", __FUNCTION__, __LINE__,
                   subtype, IFNET_IFTYPE_ALL );
        assert ( 0 );
        return -1;
    }

    switch ( subtype )
    {
        case IFNET_IFTYPE_L2IF:
            ifm_event_add ( module_id, type, IFNET_TYPE_ETHERNET );
			ifm_event_add ( module_id, type, IFNET_TYPE_GIGABIT_ETHERNET );
			ifm_event_add ( module_id, type, IFNET_TYPE_XGIGABIT_ETHERNET );
            ifm_event_add ( module_id, type, IFNET_TYPE_ETH_SUBPORT );
            ifm_event_add ( module_id, type, IFNET_TYPE_TRUNK );
            ifm_event_add ( module_id, type, IFNET_TYPE_TDM );
            ifm_event_add ( module_id, type, IFNET_TYPE_STM );

            break;
        case IFNET_IFTYPE_L3IF:
            ifm_event_add ( module_id, type, IFNET_TYPE_ETHERNET );
			ifm_event_add ( module_id, type, IFNET_TYPE_GIGABIT_ETHERNET );
			ifm_event_add ( module_id, type, IFNET_TYPE_XGIGABIT_ETHERNET );
            ifm_event_add ( module_id, type, IFNET_TYPE_ETH_SUBPORT );
            ifm_event_add ( module_id, type, IFNET_TYPE_LOOPBACK );
            ifm_event_add ( module_id, type, IFNET_TYPE_TUNNEL );
            ifm_event_add ( module_id, type, IFNET_TYPE_TRUNK );
            ifm_event_add ( module_id, type, IFNET_TYPE_VLANIF );
            ifm_event_add ( module_id, type, IFNET_TYPE_TDM );
            ifm_event_add ( module_id, type, IFNET_TYPE_STM );
            break;
        case IFNET_IFTYPE_CES:
            ifm_event_add ( module_id, type, IFNET_TYPE_TDM );
            ifm_event_add ( module_id, type, IFNET_TYPE_STM );
            break;
        case IFNET_IFTYPE_PORT:
            ifm_event_add ( module_id, type, IFNET_TYPE_ETHERNET );
			ifm_event_add ( module_id, type, IFNET_TYPE_GIGABIT_ETHERNET );
			ifm_event_add ( module_id, type, IFNET_TYPE_XGIGABIT_ETHERNET );
            ifm_event_add ( module_id, type, IFNET_TYPE_TRUNK );
            break;
        case IFNET_IFTYPE_ALL:
            ifm_event_add ( module_id, type, IFNET_TYPE_ETHERNET );
			ifm_event_add ( module_id, type, IFNET_TYPE_GIGABIT_ETHERNET );
			ifm_event_add ( module_id, type, IFNET_TYPE_XGIGABIT_ETHERNET );
            ifm_event_add ( module_id, type, IFNET_TYPE_ETH_SUBPORT );
            ifm_event_add ( module_id, type, IFNET_TYPE_LOOPBACK );
            ifm_event_add ( module_id, type, IFNET_TYPE_TUNNEL );
            ifm_event_add ( module_id, type, IFNET_TYPE_TRUNK );
            ifm_event_add ( module_id, type, IFNET_TYPE_VLANIF );
            ifm_event_add ( module_id, type, IFNET_TYPE_TDM );
            ifm_event_add ( module_id, type, IFNET_TYPE_STM );
            break;
        case IFNET_IFTYPE_VCG:
            ifm_event_add ( module_id, type, IFNET_TYPE_VCG );
            break;	
		case IFNET_IFTYPE_E1:
			ifm_event_add ( module_id, type, IFNET_TYPE_E1 );
			break;	
        default:
            zlog_err ( "[%s %d] subtype %d", __FUNCTION__, __LINE__,
                       subtype );

            break;

    }

    return 0;
}


void ifm_event_list_init ( void )
{
    for ( int i = 0; i < IFNET_EVENT_MAX; ++i )
    {
        for ( int j = 0; j < IFNET_TYPE_MAX; ++j )
        {
            ifm_event_list[i][j] = list_new();
        }
    }
}


int ifm_event_notify ( enum IFNET_EVENT type, struct ifm_event *pevent )
{
    enum IFNET_TYPE subtype = 0;
    struct list *plist;
    struct listnode *node;
    int *pdata = NULL;
    int module_id;
   // struct ipc_msghdr_n msghdr;

    if ( pevent == NULL )
    {        
        return -1;
    }
    zlog_debug ( IFM_DBG_COMMON,"[%s %d] ifm_event_notify pevent  ifindex=0x%0x", __FUNCTION__, __LINE__,
                   pevent->ifindex );


    if ( IFM_TYPE_IS_ETHERNET ( pevent->ifindex ) )
    {
        if ( IFM_IS_SUBPORT ( pevent->ifindex ) )
        { subtype = IFNET_TYPE_ETH_SUBPORT; }
        else
        { subtype = IFNET_TYPE_ETHERNET; }
    }
	else if ( IFM_TYPE_IS_GIGABIT_ETHERNET ( pevent->ifindex ) )
    {
        if ( IFM_IS_SUBPORT ( pevent->ifindex ) )
        { subtype = IFNET_TYPE_ETH_SUBPORT; }
        else
        { subtype = IFNET_TYPE_GIGABIT_ETHERNET; }
    }
	else if ( IFM_TYPE_IS_XGIGABIT_ETHERNET ( pevent->ifindex ) )
    {
        if ( IFM_IS_SUBPORT ( pevent->ifindex ) )
        { subtype = IFNET_TYPE_ETH_SUBPORT; }
        else
        { subtype = IFNET_TYPE_XGIGABIT_ETHERNET; }
    }
    else if ( IFM_TYPE_IS_VLANIF ( pevent->ifindex ) )
    {
        subtype = IFNET_TYPE_VLANIF;
    }
    else if ( IFM_TYPE_IS_LOOPBCK ( pevent->ifindex ) )
    {
        subtype = IFNET_TYPE_LOOPBACK;
    }
    else if ( IFM_TYPE_IS_TUNNEL ( pevent->ifindex ) )
    {
        subtype = IFNET_TYPE_TUNNEL;
    }
    else if ( IFM_TYPE_IS_TRUNK ( pevent->ifindex ) )
    {
        if ( IFM_IS_SUBPORT ( pevent->ifindex ) )
        { subtype = IFNET_TYPE_ETH_SUBPORT; }
        else
        { subtype = IFNET_TYPE_TRUNK; }

    }
    else if ( IFM_TYPE_IS_TDM ( pevent->ifindex ) )
    {
        subtype = IFNET_TYPE_TDM;
    }
    else if ( IFM_TYPE_IS_STM ( pevent->ifindex ) )
    {
        subtype = IFNET_TYPE_STM;

    }
    else if ( IFM_TYPE_IS_VCG( pevent->ifindex ) )
    {
        subtype = IFNET_TYPE_VCG;
    }	
    else if ( IFM_TYPE_IS_E1( pevent->ifindex ) )
    {
        subtype = IFNET_TYPE_E1;
    }	
    else
    {
        zlog_err ( "[%s %d] ifm_event_notify pevent  ifindex=0x%0x is erro", __FUNCTION__, __LINE__,
                   pevent->ifindex );
        return -1;

    }

    zlog_debug ( IFM_DBG_COMMON,"[%s %d] type=%d subtype=%d ifindex= 0x%0x\n", __FUNCTION__, __LINE__, type, subtype, pevent->ifindex );

    plist = ifm_event_list[type][subtype];
    for ( ALL_LIST_ELEMENTS_RO ( plist, node, pdata ) )
    {
        module_id = ( int ) pdata;

        zlog_debug ( IFM_DBG_COMMON,"[%s %d]module_id=%d type=%d ifindex=0x%0x\n", __FUNCTION__, __LINE__, module_id, type, pevent->ifindex );

        //ipc_send_common ( pevent, sizeof ( struct ifm_event ), 1, module_id, MODULE_ID_IFM, IPC_TYPE_IFM, type, IPC_OPCODE_EVENT );
        //ifm_msg_send_com ( &msghdr, pevent );
        
        ipc_send_msg_n2(pevent, sizeof(struct ifm_event), 1, module_id, MODULE_ID_IFM, IPC_TYPE_IFM, type, IPC_OPCODE_EVENT, 0);
    }

    return 0;
}



