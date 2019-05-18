
#ifndef _HHR_L2CP_H
#define _HHR_L2CP_H

#define HHRL2CPOID                          1,3,6,1,4,1,9966,5,35,16,2


#define hhrL2cpProtocolMac                    3
#define hhrL2cpProtocolEtherType              4
#define hhrL2cpAction                         5
#define hhrL2cpMode                           6
#define hhrL2cpProtoGroupMac                  7
#define hhrL2cpProtoGroupVlan                 8
#define hhrL2cpProtoGroupVlanCos              9
#define hhrL2cpMplsSessionId                  10
#define hhrL2cpMplsStatus                     11

#define DMAC_LEN 6
/* get-next operate interval time(seconds) */
#define HHR_L2CP_TABLE_GET_NEXT_INTERVAL 3


void init_mib_l2cp(void);

#endif                         

