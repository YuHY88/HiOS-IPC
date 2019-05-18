/*
 *  lldp interface - lldp_snmp.h
 *
 */
#ifndef _LLDP_SNMP_H
#define _LLDP_SNMP_H

#define LLDP_MIB_ROOT                          1,0,8802,1,1,2,1

#define LLDPv2_MIB_ROOT                          1,3,111,2,802,1,1,13,1

#define LLDP_MIB_PRIV_ROOT                     1,3,6,1,4,1,9966,5,35,10


/*lldpConfiguration group*/
#define lldpMessageTxInterval                  1
#define lldpMessageTxHoldMultiplier            2
#define lldpNotificationInterval               5

/*lldpPortConfigTable*/
#define lldpPortConfigAdminStatus              2
#define lldpPortConfigTLVsTxEnable             4

/*lldpStatsTxPort*/
#define lldpStatsTxPortFramesTotal             2

/*lldpStatsRxPort*/
#define lldpStatsRxPortTLVsDiscardedTotal      2
#define lldpStatsRxPortFramesErrors            3
#define lldpStatsRxPortFramesTotal             4
#define lldpStatsRxPortFramesDiscardedTotal    5
#define lldpStatsRxPortTLVsUnrecognizedTotal   6
#define lldpStatsRxPortAgeoutsTotal            7

/*lldpLocalSystemData group*/
#define lldpLocChassisIdSubtype                1
#define lldpLocChassisId                       2
#define lldpLocSysName                         3
#define lldpLocSysDesc                         4
#define lldpLocSysCapSupported                 5
#define lldpLocSysCapEnabled                   6

/*lldpLocPortTable*/
#define lldpLocPortNum                         1
#define lldpLocPortIdSubtype                   2
#define lldpLocPortId                          3
#define lldpLocPortDesc                        4

/*append private mib node*/
#define lldpNeighborNum                        1
#define lldplldpLocPortDesc                    2
#define lldplldpLocPhysAddress                 3
#define hhrlldpLocPortArpLearning              4
#define hhrlldpLocPortArpLearningVlan          5

/*lldpLocManAddrTable*/
#define lldpLocManAddrLen                      3
#define lldpLocManAddrIfSubtype                4
#define lldpLocManAddrIfId                     5
#define lldpLocManAddrOID                      6

/*lldpRemTable*/
#define lldpRemChassisIdSubtype                4
#define lldpRemChassisId                       5
#define lldpRemPortIdSubtype                   6
#define lldpRemPortId                          7
#define lldpRemPortDesc                        8
#define lldpRemSysName                         9
#define lldpRemSysDesc                        10
#define lldpRemSysCapSupported                11
#define lldpRemSysCapEnabled                  12

/*append private mib nodes*/
#define lldpSvlanType                          6
#define lldpSvlanId                            7
#define lldpCvlanType                          8
#define lldpCvlanId                            9
#define lldpExpiredTime                       10
#define lldpMgmtAddrSubType                   11
#define lldpMgmtAddr		                  12

#define hhrlldpManagementAddressSubType        4
#define hhrlldpManagementAddress		       5

/*lldpRemManAddrTable*/
#define lldpRemManAddrSubtype			1
#define lldpRemManAddr						2
#define lldpRemManAddrIfSubtype                3
#define lldpRemManAddrIfId                     4
#define lldpRemManAddrOID                      5

/*lldpV2LocManAddrTable*/
#define lldpV2LocManAddrSubtype                1
#define lldpV2LocManAddr                       2
#define lldpV2LocManAddrLen                    3
#define lldpV2LocManAddrIfSubtype              4
#define lldpV2LocManAddrIfId                   5
#define lldpV2LocManAddrOID                    6

struct lldp_msap_man_addr_index
{
	uint32_t ifindex;
	uint32_t remote_index;
	time_t time_mark;
};

void init_lldp_snmp(void);

#endif                          /* _LLDP_SNMP_H */
