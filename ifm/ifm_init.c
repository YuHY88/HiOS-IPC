#include <lib/prefix.h>
#include "ifm_init.h"

int ifm_interface_init( void )
{
    int ret = 0;
    uint32_t ifindex = 0;

    /*loopback*/
    ifindex = ifm_get_ifindex_by_name ( "loopback", "0" );
    ret = ifm_create_loopback ( ifindex );
    if ( ret < 0 )
    {
        zlog_err ( "%s[%d] create loopback port 0 erro\n", __FUNCTION__, __LINE__ );
        return IFM_ERR_GENERAL;
    }
    return ( 0 );
}

int ifm_netif_mac_inc ( char *ipMac, char *opMac )
{
    int i = 0;
    int temp = 0;

    if ( ( ipMac == NULL ) || ( opMac == NULL ) ) { return ( -1 ); }

    do
    {
        opMac[i] = ipMac[i] + 1;
        temp = ipMac[i] + 1;
        i++;
    }
    while ( i < 6 && temp > 255 );

    for ( ; i < 6; i++ )
    {
        opMac[i] = ipMac[i];
    }
    return 0;
}

