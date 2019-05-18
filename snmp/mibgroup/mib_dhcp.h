/*
 *
 */
#ifndef _HHR_DHCP_H
#define _HHR_DHCP_H

#define HHRDHCPOID                          1,3,6,1,4,1,9966,5,35,4


#define hhrDHCPIPPoolTable                  0
#define hhrDHCPIPBindTable                  1
#define hhrDHCPIfConfigTable                2
#define hhrDHCPDynamicIPBindTable           3


#define hhrDHCPIPPoolTable_REFRESH_TIME                 5
#define hhrDHCPIPBindTable_REFRESH_TIME                 5
#define hhrDHCPIfConfigTable_REFRESH_TIME               5
#define hhrDHCPDynamicIPBindTable_REFRESH_TIME          5

#define hhrDHCPIPPoolTable_GET_NEXT_INTERVAL            3
#define hhrDHCPIPBindTable_GET_NEXT_INTERVAL            3
#define hhrDHCPIfConfigTable_GET_NEXT_INTERVAL          3
#define hhrDHCPDynamicIPBindTable_GET_NEXT_INTERVAL     3

/*hhrDHCPIPPoolTable*/
#define hhrIPPollName                       1
#define hhrGatewayIP                        2
#define hhrGatewayMask                      3
#define hhrStartIP                          4
#define hhrIPPoolIPNum                      5
#define hhrDNSIP                            6
#define hhrIPPoolLease                      7

/*hhrDHCPIPBindTable*/
#define hhrIPPoolIP                         1
#define hhrBindMac                          2

/*hhrDHCPIfConfigTable*/
#define hhrDHCPIfDesc                       1
#define hhrDHCPServerEnable                 2
#define hhrDHCPClientEnable                 3
#define hhrDHCPRelayEnable                  4
#define hhrDHCPRelayNextHot                 5
#define hhrDHCPClientSaveEnable             6
#define hhrDHCPClientZeroEnable             7

/*hhrDHCPDynamicIPBindTable*/
#define hhrDHCPDynamicMac                   2

/*
 * init_xxx(void)
 */
void init_mib_dhcp(void);

#endif                          /* _HHR_DHCP_H */
