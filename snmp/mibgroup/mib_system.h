/*
 *
 */
#ifndef _HHR_SYSTEM_H
#define _HHR_SYSTEM_H

#define HHRSYSTEMOID                       1,3,6,1,4,1,9966,5,35,8

/*hhrSystemConf*/
#define hhrDeviceID                        1

/*hhrSynceConf*/
#define hhrSyncEMode                       1
#define hhrSyncESSMEnable                  2
#define hhrSyncESSMInterval                3
#define hhrSyncEPreemptEnable              4
#define hhrSyncEFailbackWtr                5
#define hhrSyncEHoldTime                   6
#define hhrSyncEClockSource                7

/*hhrSyncEClockIfConfigTable*/
#define hhrSCCIfDescr                      1
#define hhrSyncEClockIfMode                2
#define hhrSyncEClockIfSignal              3
#define hhrSyncEClockIfQualityLevel        4

/*hhrSyncEColckPriorityTable*/
#define hhrSCPIfDescr                      1
#define hhrSyncEColckPriority              2

/*hhrntpConf*/
#define hhrNtpMode                         1
#define hhrNtpPacket                       2
#define hhrNtpSynceInterval                3
#define hhrNtpVersion                      4
#define hhrNtpServerOrPeer                 5
#define hhrNtpClientEnable                 6

//devicefanInfo
#define hhrFanSpeedMin                     1
#define hhrFanSpeed                        2
#define hhrFanSpeedMax                     3

//hhrDiviceBoardTable
#define hhrDeviceBoardType                 2
#define hhrDeviceBoardStatus               3
#define hhrDeviceBoardRunningTime          4
#define hhrDeviceBoardName                 5
#define hhrDeviceBoardID                   6


//hhrDevicePowerTable
#define hhrDevicePowerType                 2
#define hhrDevicePowerInVoltageLow         3
#define hhrDevicePowerInVoltage            4
#define hhrDevicePowerInVoltageHigh        5
#define hhrDevicePowerOutVoltage       	   6


//hhtTemperature
#define hhrTemperatureMin       	   	   1
#define hhrTemperature       	   	   	   2
#define hhrTemperatureMax       	   	   3



#define hhrMemoryTotal                     1
#define hhrMemoryUsed                      2
#define hhrMemoryFree                      3
#define hhrMemoryShared                    4
#define hhrMemoryBuffersUsed               5
#define hhrMemoryBuffersFree               6
#define hhrMemorySwapTotal                 7
#define hhrMemorySwapUsed                  8
#define hhrMemorySwapFree                  9

//hhrCpuInfoTable
#define hhrCPUUsr               2
#define hhrCPUNice              3
#define hhrCPUSys               4
#define hhrCPUIdle              5
#define hhrCPUIO                6
#define hhrCPUIrq               7
#define hhrCPUSirq              8

#define hhrCpuThreshold         3
#define hhrMemoryThreshold      4


//eepromInfo
#define hhrEEpromHwcfg			2
#define hhrEEpromDev			3
#define hhrEEpromOem			4
#define hhrEEpromMac			5
#define hhrEEpromDver			6
#define hhrEEpromHver			7
#define hhrEEpromVnd			8
#define hhrEEpromSn				9
#define hhrEEpromDate			10
#define hhrEEpromBom			11
#define hhrEEpromLic			12



/*
 * init_xxx(void)
 */
void init_hhr_system(void);

#endif                          /* _HHR_SYSTEM_H */
