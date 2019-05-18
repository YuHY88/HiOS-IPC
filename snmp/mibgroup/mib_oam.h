/*
 *
 */
#ifndef _HHR_OAM_H
#define _HHR_OAM_H

#define HHR_OAM_ROID                        1,3,6,1,4,1,9966,5,35,11
#define HHR_BFD_ROID                        1,3,6,1,4,1,9966,5,35,12

#define hhrMplsTpOamConfigTable                 0
#define hhrbfdDetectConfigTable                 1

/* table data refresh time(seconds) */
#define hhrMplsTpOamConfigTable_REFRESH_TIME                    5
#define hhrbfdDetectConfigTable_REFRESH_TIME                    5

#define hhrMplsTpOamConfigTable_GET_NEXT_INTERVAL               3
#define hhrbfdDetectConfigTable_GET_NEXT_INTERVAL               3

/*hhrMplsTpOamConfigTable*/
#define hhrTpOamPriority                    2
#define hhrTpOamPeerMep                     3
#define hhrTpOamCCEnable                    4
#define hhrTpOamCCInterval                  5
#define hhrTpOamAISEnable                   6
#define hhrTpOamPathType                    7
#define hhrTpOamPathName                    8
#define hhrTpOamSectionDestMAC              9
#define hhrTpOamSectionVlan                 10
#define hhrTpOamSectionChannelType          11
#define hhrTpOamSectionAlarmEnable          12
#define hhrTpOamNodeType                    13
#define hhrTpOamMpid                        14
#define hhrTpOamPeerMepForward              15
#define hhrTpOamPeerMepReverse              16
#define hhrTpOamAISLevel                    17
#define hhrTpOamAISInterval                 18
#define hhrTpOamBindMeg                     19
#define hhrTpOamPathNameRevLsp              20
#define hhrTpOamState                       21
#define hhrTpOamLCKEnable                   22
#define hhrTpOamLCKLevel                    23
#define hhrTpOamLCKInterval                 24
#define hhrTpOamCSFEnable                   25
#define hhrTpOamCSFLevel                    26
#define hhrTpOamCSFInterval                 27

/*hhrMplsTpOamMegTable*/
#define hhrTpOamMegName                     1
#define hhrTpOamMegLevel                    2

//bfd gobal information
#define hhrbfdEnable                        1
#define hhrbfdGlobalPriority                2
#define hhrbfdGlobalMinReInterval           3
#define hhrbfdGlobalDetectMultipier         4
#define hhrbfdGlobalWTR                     5

//bfd session information
#define hhrbfdSessionId                     1
#define hhrbfdRemoteDiscriminator           2
#define hhrbfdccInterval                    3
#define hhrbfdDetectMultiplier              4
#define hhrbfdDetectType                    5
#define hhrbfdDetectObjectDescr             6
#define hhrbfdSourceIp                      7
#define hhrbfdSessionState                  8
#define hhrbfdLocalDiscriminator            9
#define hhrbfdWithIPOrUDPHeaderEnable       10

/*
 * init_xxx(void)
 */
void init_hhr_oam(void);

#endif                          /* _HHR_OAM_H */
