#ifndef HIOS_IFM_INIT_H
#define HIOS_IFM_INIT_H

#include "ifm.h"

#define IFM_DEF_SUB_TYPE             IFNET_SUBTYPE_GE
#define IFM_DEF_MODE                 IFNET_MODE_L3
#define IFM_DEF_ENCP                 IFNET_ENCAP_INVALID
#define IFM_DEF_SHUTDOWN             IFNET_NO_SHUTDOWN
#define IFM_DEF_STAT                 IFNET_STAT_ENABLE
#define IFM_DEF_FLAP                 1
#define IFM_DEF_PIR                  0
#define IFM_DEF_TPID                 0x8100
#define IFM_DEF_SVLANID              0
#define IFM_DEF_CVLANID              0
#define IFM_DEF_MTU                  1500
#define IFM_DEF_JUMBO                12288
#define IFM_DEF_SFP_OFF              6
#define IFM_DEF_SFP_ON               2
#define IFM_DEF_SFP_ALS              IFM_SFP_ALS_DISABLE  
#define IFM_DEF_SFP_TX               IFM_SFP_TX_ENABLE
#define IFM_DEF_SYS_MAC              0,0,0,0,0,0
#define IFM_DEF_COMBO_TYPE           IFNET_FIBERTYPE_FIBER
#define IFM_DEF_INTERVAL             5

int ifm_interface_init(void);
int ifm_netif_mac_inc ( char *ipMac, char *opMac );

#endif

