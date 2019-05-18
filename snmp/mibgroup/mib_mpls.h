/*
 *
 */
#ifndef _HHR_MPLS_H
#define _HHR_MPLS_H

#define HHRMPLSCONFIG                       1,3,6,1,4,1,9966,5,35,5

/*hhrMplsconfigComm*/
#define hhrMplsEnable                       1
#define hhrMplsLsrID                        2

/*hhrLspConfigTable*/
#define hhrLspNodeType                      2
#define hhrLspDestIP                        3
#define hhrLspDestMask                      4
#define hhrLspOutIF                         5
#define hhrLspDestMac                       6
#define hhrLspNexthop                       7
#define hhrLspInLabel                       8
#define hhrLspOutLabel                      9
#define hhrLspTpOamSessionId               10
#define hhrLspIndex                        11
#define hhrLspLinkStatus                   12
#define hhrLspAdminStatus                  13

/*hhrPseudoWireTable*/
#define hhrPWName                           1
#define hhrVCid                             2
#define hhrPWProtocolType                   3
#define hhrPWVCType                         4
#define hhrPWDestination                    5
#define hhrPWInLabel                        6
#define hhrPWOutLabel                       7
#define hhrPWRelatedTunnelName              8
#define hhrPWControlWord                    9
#define hhrPWEncapsulationType             10
#define hhrPWCESFrameNum                   11
#define hhrPWTPID                          12
#define hhrPWVlan                          13
#define hhrPWmtu                           14
#define hhrRelatedVplsid                   15
#define hhrPWInVplsNetLocation             16
#define hhrPWIndex                         17
#define hhrPWSwitchPWIndex                 18
#define hhrPWEnableMip                     19
#define hhrPWTpOamSessionId                20
#define hhrPWStatisticsEnable              21
#define hhrPWTunnelPolicyLdp               22
#define hhrPWExpDomain                     23
#define hhrPWPhd                           24
#define hhrPWTunnelPolicymplstp            25
#define hhrPWTunnelPolicyGre               26
#define hhrPWLinkStatus                    27
#define hhrPWAdminStatus                   28

/*hhrUNIOfVCTable*/
#define hhrUNIOfCESIfDESC                   1
#define hhrUNIRelatedVCId                   2
#define hhrPWProtectionRole                 3
#define hhrPWProtectionFailback             4
#define hhrPWProtectionFailbackWtr          5
#define hhrPWProtectionWorkStatus           6

/*hhrVplsServiceTable*/
#define hhrVplsId                           1
#define hhrVplsName                         2
#define hhrVplsMacLearnEnable               3
#define hhrVplsMacLearnLimit                4
#define hhrVplsEncapsulateType              5
#define hhrVplsEncapsulateTag               6
#define hhrVplsEncapsulateTpId              7
#define hhrVplsEncapsulateVlan              8

/*hhrVplsStormSuppresTable*/
#define hhrVplsUnicastEnable                1
#define hhrVplsUnicastCir                   2
#define hhrVplsUnicastCbs                   3
#define hhrVplsBroadcastEnable              4
#define hhrVplsBroadcastCir                 5
#define hhrVplsBroadcastCbs                 6
#define hhrVplsMulticastEnable              7
#define hhrVplsMulticastCir                 8
#define hhrVplsMulticastCbs                 9

/*hhrPseudoWireServiceRelateTable*/
#define hhrRelatedVplsName                  1
#define hhrRelatedPwName                    2
#define hhrRelatedPwNetLocation             3

/*hhrUNIOfVPLSTable*/
#define hhrUNIOfVPLSIfDesc                  1

/*hhrTunnelConfigTable*/
#define hhrTunnelName                       1
#define hhrTunnelEncapsulateType            2
#define hhrTunnelSrcIP                      3
#define hhrTunnelDestIP                     4
#define hhrTunnelRelatedIngressLspName      5
#define hhrTunnelRelatedEgressLspName       6
#define hhrTunnelLinkStatus                 7
#define hhrTunneBackupIngressLspName        8
#define hhrTunnelBackupEgressLspName        9
#define hhrTunneOutIf                      10
#define hhrTunnelDestMac                   11
#define hhrTunnelNexthop                   12
#define hhrTunnelQosRateLimitIngressCIR    13
#define hhrTunnelQosRateLimitIngressPIR    14
#define hhrTunnelQosRateLimitEgressCIR     15
#define hhrTunnelQosRateLimitEgressPIR     16
#define hhrTunnelWorkingStatus             17
#define hhrTunnelFailback                  18
#define hhrTunnelFailbackWtr               19


/*hhrMplsTpOamConfigTable*/
#define hhrTpOamPriority                    1
#define hhrTpOamDirection                   2
#define hhrTpOamPeerMep                     3
#define hhrTpOamCCEnable                    4
#define hhrTpOamCCInterval                  5
#define hhrTpOamAISEnable                   6
#define hhrTpOamPathType                    7
#define hhrTpOamPathName                    8
#define hhrTpOamLspPathType                 9

/*hhrTunnelStatisticsTable*/
#define hhrTSIfDescr                        1
#define hhrTSInputTotalBytes                2
#define hhrTSInputTotalPackets              3
#define hhrTSInputDropPackets               4
#define hhrTSOutputTotalBytes               5
#define hhrTSOutputTotalPackets             6
#define hhrTSOutputDropPackets              7

/*hhrPWStatisticsTable*/
#define hhrPSPWName                         1
#define hhrPSInputTotalBytes                2
#define hhrPSInputTotalPackets              3
#define hhrPSInputDropPackets               4
#define hhrPSOutputTotalBytes               5
#define hhrPSOutputTotalPackets             6
#define hhrPSOutputDropPackets              7

/*hhrMplsApsTable*/
#define hhrMplsApsSessionid                 1
#define hhrMplsApsKeepAlive                 2
#define hhrMplsApsHoldOff                   3
#define hhrMplsApsBackup                    4
#define hhrMplsApsBackupFailbackWtr         5
#define hhrMplsApsPriority                  6
#define hhrMplsApsMasterIndex               7
#define hhrMplsApsMasterName                8
#define hhrMplsApsBackupIndex               9
#define hhrMplsApsBackupName               10
#define hhrMplsApsEnable                   11
#define hhrMplsApsActivePath               12
#define hhrMplsApsCurrentState             13
#define hhrMplsApsType                     14


/*hhrL3vpnInstanceTable*/
#define hhrL3vpnInstanceId                  1
#define hhrL3vpnInstanceName                2
#define hhrL3vpnApplyLabel                  3
#define hhrL3vpnRD                          4

/*hhrL3vpnTargetTable*/
#define hhrL3vpnInstanceId                  1
#define hhrL3vpnTarget                      2
#define hhrL3vpnTargetType                  3

/**/
#define inLabel                             1
#define owner                               2
#define labelState                          3


/*LDP common*/
#define hhrMplsLdpEnable                     1
#define hhrMplsLdpLabelPolicy                2
#define hhrMplsLdpAdvertise                  3
#define hhrMplsLdpKeepalive                  4
#define hhrMplsLdphello                      5

/*hhrMplsLdpLocalifTable*/
#define hhrMplsLdpLocalifIndex               1
#define hhrMplsLdpLocalIfDesrc               2
#define hhrMplsLdpLocalLdpEnable             3

/*hhrMplsLdpSessionTable*/
#define hhrMplsLdpSessionPeerId              1
#define hhrMplsLdpSessionRole                2
#define hhrMplsLdpSessionStatus              3
#define hhrMplsLdpSessionType                4
#define hhrMplsLdpSessionCreateTime          5

/*hhrMplsLdpLspTable*/
#define hhrMplsLdpLspDir                     1
#define hhrMplsLdpLspFec                     2
#define hhrMplsLdpLspIngressLabel            3
#define hhrMplsLdpLspEgressLabel             4
#define hhrMplsLdpLspNextHop                 5

/*hhrMplsLdpLspIPPrefixTable*/
#define hhrMplsLdpIPPrefix                   1
#define hhrMplsLdpIPPrefixMask               2
#define hhrMplsLdpIPPrefixReservedField      3

/*hhrMplsLdpRemotePeerTable*/
#define hhrMplsLdpRemotePeer                 1
#define hhrMplsLdpRemotePeerReservedField    2

#define hhrVplsMacBlackWhiteType                1
#define hhrVplsMacBlackWhiteMacAddress          2
#define hhrVplsMacBlackWhiteReservedField       3

#define hhrVplsEtreeServiceId                   1
#define hhrVplsServiceType                      2
#define hhrVplsEtreeServiceName                 3
#define hhrVplsServiceVpnID                     4
#define hhrVplsRootPortName                     5

#define hhrVplsStaticMacAddress                 1
#define hhrVplsStaticMacBindMode                2
#define hhrVplsStaticMacIfDescr                 3
#define hhrVplsStaticMacPWId                    4


#define hhrVplsDynamicMacAddress                1
#define hhrVplsdynamicMacType                   2
#define hhrVplsdynamicMacAcIf                   3
#define hhrVplsdynamicMacPW                     4




/**/
/*
 * init_xxx(void)
 */
void init_hhr_mpls(void);

#endif                          /* _HHR_MPLS_H */
