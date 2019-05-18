/*
*            define of qos interface
*/

#ifndef HIOS_QOS_IF_H
#define HIOS_QOS_IF_H


#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/qos_common.h>
#include <qos/qos_mirror.h>


#define QOS_IF_TAB_SIZE 1024 * 6
#define QOS_PACKET_IFG_DEFAULT 20


/* 接口上的 QOS 配置 */
struct qos_if
{
	uint32_t ifindex;
	uint8_t  mode;             /* 接口模式 */
	uint8_t  ifg;
	uint8_t  car_flag[2];	        /* 0:qos lr; 1:qos car */
	uint32_t car_id[2];        		/* 接口上应用的 car 模板 id */
	uint32_t hqos_id;
	struct qos_if_mirror *if_mirror; 	/* 接口配置的 mirror */
	struct list  *pqos_policy; 		/* 接口配置的 qos policy */
};


extern struct hash_table qos_if_table;

void qos_if_table_init ( int size );
struct qos_if *qos_if_create(uint32_t ifindex);
int qos_if_add(struct qos_if *pif);
int qos_if_delete(uint32_t ifindex);
struct qos_if *qos_if_lookup(uint32_t ifindex);
void qos_if_cmd_init(void);


int qos_if_delete_config(uint32_t ifindex);
int qos_if_process_mode_change(uint32_t ifindex);
int qos_if_check(uint32_t ifindex);


int qos_if_car_apply_get_bulk ( uint32_t index, enum QOS_DIR dir, uint32_t *carid_array );
int qos_if_car_param_get_bulk ( uint32_t index, enum QOS_DIR dir, struct car_t *car_array );
int qos_if_get_bulk ( uint32_t index, struct qos_if_snmp *qosif_array );



#endif


