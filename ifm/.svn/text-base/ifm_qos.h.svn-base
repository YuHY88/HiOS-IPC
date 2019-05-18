/*
*            define of qos interface
*/

#ifndef HIOS_IFM_QOS_H
#define HIOS_IFM_QOS_H


#include <lib/types.h>
#include <qos/qos_mapping.h>
#include "ifm.h"


#define DEFAULT_UNTAG_PRI 5
#define QOS_PHB_ENABLE 1
#define QOS_PHB_DISABLE 0


#define QOS_DOMAIN_TYPE_GET(_mode,_type)  \
do{                                    \
switch(_mode){                        \
    case IFNET_MODE_L2:             \
    case IFNET_MODE_SWITCH:         \
        _type = QOS_TYPE_COS;        \
        break;                        \
    case IFNET_MODE_L3:             \
        _type = QOS_TYPE_DSCP;        \
        break;                        \
}                                    \
}while(0)

#define IFM_QOS_DOMAIN_STR_GET(_type,_str)\
do{                                       \
switch(_type){                        \
    case QOS_TYPE_COS:                \
        _str = "Cos-Domain";        \
            break;                    \
    case QOS_TYPE_TOS:                \
        _str = "Tos-Domain";        \
            break;                    \
    case QOS_TYPE_EXP:                \
        _str = "Exp-Domain";        \
            break;                    \
    case QOS_TYPE_DSCP:                \
        _str = "Dscp-Domain";        \
            break;                    \
    default:                        \
        break;                        \
}                                      \
}while(0)

#define IFM_QOS_PHB_STR_GET(_type,_str)\
do{                                 \
switch(_type){                        \
    case QOS_TYPE_COS:                \
        _str = "Cos-Phb";             \
            break;                    \
    case QOS_TYPE_TOS:                \
        _str = "Tos-Phb";             \
            break;                    \
    case QOS_TYPE_EXP:                \
        _str = "Exp-Phb";             \
            break;                    \
    case QOS_TYPE_DSCP:             \
        _str = "Dscp-Phb";            \
            break;                    \
    default:                        \
        break;                        \
}                                    \
}while(0)


int ifm_qos_set_domain(struct ifm_entry *pifm, struct qos_entry *pqos);    
int ifm_qos_set_phb(struct ifm_entry *pifm, struct qos_entry *pqos);    
int ifm_qos_set_untag_pri(struct ifm_entry *pifm, uint8_t untag_pri);
int ifm_qos_set_info(uint32_t ifindex, void *pdata, enum QOS_INFO type);
int ifm_qos_unset_domain(struct ifm_entry *pifm);
int ifm_qos_unset_phb(struct ifm_entry *pifm);
void ifm_qos_show_operation(struct vty *vty, struct ifm_entry *pifm);
void ifm_qos_config_write (struct ifm_entry *pifm, struct vty *vty);
void ifm_qos_init(void);
int ifm_qos_get_bulk ( uint32_t index, struct qos_entry_snmp *qos_array );




#endif
