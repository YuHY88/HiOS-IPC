/*
*   define of SDHVCG interface
*/

#ifndef HIOS_SDHVCG_IF_H
#define HIOS_SDHVCG_IF_H

#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/linklist.h>
#include "lib/sdh_vcg_common.h"

#define  VCG_DISABLE                        0
#define  VCG_ENABLE                         1

#define  SDH_VCG_BINDING_UP_DIRECTION       1
#define  SDH_VCG_BINDING_DOWN_DIRECTION     2
#define  SDH_VCG_BINDING_BOTH_DIRECTION     0

#define  SDH_VCG_TRANSMIT                   0
#define  SDH_VCG_EXPECT                     1

/*sdhvcg debug module id*/
#define  SDH_VCG_DEBUG_TYPE_ALL             0X01

struct list         *g_vcg_if_list;

extern void vcg_if_table_init(int size);

extern t_sdh_vcg_interface *vcg_if_create(void *pdata, uint32_t ifindex);
extern t_sdh_vcg_interface *vcg_if_add(void *pdata, uint32_t ifindex);
extern int vcg_if_delete(uint32_t ifindex);
extern int vcg_if_status_update(void *pdata, int data_len, int data_num);

extern t_sdh_vcg_interface *vcg_if_lookup(uint32_t ifindex);

extern int vcg_set_binding(t_sdh_vcg_interface *pvcg);
extern int vcg_set_jx_transmit(t_sdh_vcg_interface *pvcg);
extern int vcg_set_jx_expect(t_sdh_vcg_interface *pvcg);

extern int vcg_set_scrambling(t_sdh_vcg_interface *pvcg);
extern int vcg_set_fcs(t_sdh_vcg_interface *pvcg);
extern int vcg_set_lcas_ena(t_sdh_vcg_interface *pvcg);
extern int vcg_set_lcas_reset(t_sdh_vcg_interface *pvcg);
extern int vcg_set_vlan(t_sdh_vcg_interface *pvcgif);

extern int vcg_if_info_set(t_sdh_vcg_interface *pvcgif, E_SDH_VCG_IPC_MSG_SUBTYPE type);
extern void vcg_get_config_bulk(void *pdata, struct ipc_msghdr_n *pmsghdr);
extern int vcg_set_image(t_sdh_vcg_interface *pvcgif);

#endif


