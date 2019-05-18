

#ifndef _HHR_PORT_PROPERTIES_H
#define _HHR_PORT_PROPERTIES_H

#define HHR_IF_EXT_OID                          1,3,6,1,4,1,9966,5,35,1

//hhrPortStormSuppressConfigTable
#define hhrPortStormSuppressIfDescr                         1
#define hhrPortStormSuppressUnicast                2
#define hhrPortStormSuppressBroadcast                3
#define hhrPortStormSuppressMulticast                 4
#define hhrPortStormSuppressUnicastRate                 5
#define hhrPortStormSuppressBroadcastRate                 6
#define hhrPortStormSuppressMulticastRate                       7

//hhrPortDot1qTunnelConfigTable
#define hhrPortDot1qTunnelIfDescr                         1
#define hhrPortDot1qTunnelSVlan                        2
#define hhrPortDot1qTunnelCos                   3

//hhrSubIfManageIPTable
#define hhrSubIfManageIPIfDescr                1
#define hhrSubIfManageIPEnable             2

void init_mib_port_properties(void);

#endif                         

