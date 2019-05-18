/*
 *  cli_set group interface - mib_snmp.h
 *
 */
#ifndef _MIB_SNMP_H
#define _MIB_SNMP_H

#define HHR_SNMP_ROID                   1,3,6,1,4,1,9966,5,35,8,16

#define hhrSNMPVersion                  1
#define hhrSNMPVpnInstance             	2

#define hhrSNMPServerIP                 1
#define hhrSNMPServerUdpPort            2
#define hhrSNMPServerVPN                3
#define hhrSNMPServerVersion            4
#define hhrSNMPServerV1V2cCommunity     5
#define hhrSNMPServerV3User             6


#define hhrSNMPV1V2cCommunity           1
#define hhrSNMPV1V2cCommunityLevel      2

#define hhrSNMPv3User                   1
#define hhrSNMPv3UserAuthType           2
#define hhrSNMPv3UserAuthPwd            3
#define hhrSNMPv3UserPrivilege          4
#define hhrSNMPv3UserPriType            5
#define hhrSNMPv3UserPriPwd          	6

/*-------------------------------for H3C oid------------------------*/
#define H3C_SNMP_ROID                   1,3,6,1,4,1,9966,2,201,2,104

#define hhSnmpExtVersion                              5

#define hhSnmpExtCommunitySecurityLevel_mib           1
#define hhSnmpExtCommunitySecurityName_mib            2
#define hhSnmpExtCommunityName_mib                    3
#define hhSnmpExtCommunityAclNum_mib                  4
#define hhSnmpExtCommunityIPv6AclNum_mib              5

/*-------------------------------H3C mib oid-----------------------*/
void init_mib_snmp(void);

#endif                          /* _IFM_SNMP_H */
