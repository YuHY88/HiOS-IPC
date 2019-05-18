/* Copyright (C) 2013-2013 huahuan, Inc.  All Rights Reserved.*/
#ifndef _ZEBOS_DHCP_SNMP_H
#define _ZEBOS_DHCP_SNMP_H

#include "../libnet/libnet-asn1.h"

//neDiscoverTrap
//static oid neDiscoverTrap_oid[] = {1,3,6,1,4,1,9966,1,10,2};
//neDiscoverAck
//static oid neDiscoverAck_oid[] = {1,3,6,1,4,1,9966,1,12,0};
//neBasicInfoAlmInfo
//static oid neBasicInfoAlmInfo_oid[] = {1,3,6,1,4,1,9966,1,11,0};
//neDiscoverTrap_param
//static oid neDiscoverTrap_param_oid[] = {1,3,6,1,4,1,9966,1,11};

#define DHCP_SYSMGM_OID 1,3,6,1,4,1,9966
#ifndef HAVE_EMD
//static oid DHCP_SYSMGM_OID_VLAUE [] = {DHCP_SYSMGM_OID} ;
#else
static oid DHCP_SYSMGM_OID_VLAUE [] = {1,3,6,1,4,1,9966,6,3,2} ;
#endif
//static oid DHCP_NE_MIBObjects_oid [] = {DHCP_SYSMGM_OID,1};

#define DHCP_INFO_OID 1,3,6,1,4,1,9966,5,25,19,1
#ifndef HAVE_EMD
//static oid DHCP_INFO_MIBObjects_oid [] = { DHCP_INFO_OID , 8};
#else
static oid DHCP_INFO_MIBObjects_oid [] = {1,3,6,1,4,1,9966,6,3,2,13};
#endif
//////////////////////////////////////////////

#define estDhcp_Enable 				1

//////////////////////////////////////////////

#define DHCP_NE_DISCOVER_ACK           12 

//////////////////////////////////////////////

//void dhcp_snmp_init (struct lib_globals *);

//void dhcp_snmp_deinit (struct lib_globals *);

//struct lib_globals * dhcp_get_globalzg();

#endif  /* _ZEBOS_SDM_SNMP_H */
