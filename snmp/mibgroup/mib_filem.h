/*
 *  alarm snmp browser interface - filem_snmp.h
 *
 */
#ifndef _MIB_FIELM_H
#define _MIB_FIELM_H

#define filemSystemSlotNum              1
#define filemSystemSoftVer              2
#define filemSystemFpgaVer              3
#define filemSystemKernelVer            4
#define filemSystemUpgradeTime          5
#define filemSystemFreeSpace            6
#define filemSystemHardwareVer          7

#define filemFlieSysSatusUpgrade        1
#define filemFileSysSatusDownload       2
#define filemFileSysSatusUpload         3
#define filemFileSysL3VPNNumber         4
#define filemFileSysStaFpgaUpgrade      5
#define filemFileSysStaKrnlUpgrade      10




#define filemFileSysDevFileIndex        1
#define filemFileSysDevFileName         2
#define filemFileSysDevFileTye          3
#define filemFileSysDevFileSize         4
#define filemFileSysDevFileCreatetime   5


void  init_filem_snmp(void);


#endif                          /* _ALARM_SNMP_H */
