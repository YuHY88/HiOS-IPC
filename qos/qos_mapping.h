/*
*             
*                  define of qos mapping
*/

#ifndef HIOS_QOS_MAPPING_H
#define HIOS_QOS_MAPPING_H

#include <lib/types.h>
#include <lib/ifm_common.h>
#include <qos/qos_main.h>



#define QOS_DOMAIN_NUM   6    /* domain 模板的数量 */
#define QOS_PHB_NUM      6    /* phb 模板的数量 */

#define QOS_MAPPING_DETACHED 0

#define QOS_DOMAIN_MAPPING_NUM_GET(_type,_num)  \
do                                              \
{                                               \
    switch(_type)                               \
    {                                           \
        case QOS_TYPE_COS:                      \
        case QOS_TYPE_TOS:                      \
        case QOS_TYPE_EXP:                      \
        _num = 8;                               \
        break;                                  \
        case QOS_TYPE_DSCP:                     \
        _num = 64;                              \
        break;                                  \
        default:                                \
        break;                                  \
    }                                           \
}while(0) 


extern struct qos_domain *pdomain_cos[QOS_DOMAIN_NUM];
extern struct qos_domain *pdomain_tos[QOS_DOMAIN_NUM];
extern struct qos_domain *pdomain_exp[QOS_DOMAIN_NUM];
extern struct qos_domain *pdomain_dscp[QOS_DOMAIN_NUM];


extern struct qos_phb *pphb_cos[QOS_PHB_NUM];
extern struct qos_phb *pphb_tos[QOS_PHB_NUM];
extern struct qos_phb *pphb_exp[QOS_PHB_NUM];
extern struct qos_phb *pphb_dscp[QOS_PHB_NUM];


/* Prototypes. */
void qos_domain_init(void);
struct qos_domain *qos_domain_create(uint8_t id, enum QOS_TYPE type); /* 创建一个 domain */
int qos_domain_delete(uint8_t id, enum QOS_TYPE type);             /* 删除一个 domain */
struct qos_domain *qos_domain_get(uint8_t id, enum QOS_TYPE type); /* 查找一个 domain */
int qos_domain_add_mapping(uint8_t id, enum QOS_TYPE type, struct qos_map *pmap);
int qos_domain_delete_mapping(uint8_t id, enum QOS_TYPE type, uint8_t priority);
int qos_domain_get_bulk ( uint8_t index, enum QOS_TYPE type, struct qos_domain *domain_array );


void qos_phb_init(void);
struct qos_phb *qos_phb_create(uint8_t id, enum QOS_TYPE type); /* 创建一个 phb */
int qos_phb_delete(uint8_t id, enum QOS_TYPE type);          /* 删除一个 phb */
struct qos_phb *qos_phb_get(uint8_t id, enum QOS_TYPE type); /* 查找一个 phb */
int qos_phb_add_mapping(uint8_t id, enum QOS_TYPE type, struct qos_map *pmap);
int qos_phb_add_all_color_mapping(uint8_t index, enum QOS_TYPE type, uint8_t queue, uint8_t priority);
int qos_phb_delete_mapping(uint8_t id, enum QOS_TYPE type, uint8_t queue, enum QOS_COLOR color);
int qos_phb_delete_all_color_mapping(uint8_t index, enum QOS_TYPE type, uint8_t queue);
int qos_phb_get_bulk ( uint8_t index, enum QOS_TYPE type, struct qos_phb *phb_array );


int qos_domain_set_default_mapping(uint8_t id, enum QOS_TYPE type, struct qos_domain *pdomain);/*配置qos_domain默认映射*/
int qos_phb_set_default_mapping(uint8_t id, enum QOS_TYPE type, struct qos_phb *pphb);


#endif
