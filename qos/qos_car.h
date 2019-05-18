
#ifndef HIOS_QOS_CAR_H
#define HIOS_QOS_CAR_H

#include <lib/types.h>
#include <lib/qos_common.h>
#include <qos/qos_if.h>
#include <qos/qos_main.h>

#define CAR_ACTION_CONFIGED    (1<<7)
#define CAR_ACTION_COS_MASK    (0x7)
#define CAR_ACTION_DSCP_MASK   (0x3f)
#define CAR_ACTION_QUEUE_MASK  (0x7)

#define QOS_CAR_NUM   6144


extern struct hash_table qos_car_profile;/* 全局模式下的 car profile hash 表, 使用 car_id 做 key */


/* 大数量回显时判断条件，一次最多回显 60 行 */
#define CAR_PROFILE_LINE_CHECK(line, line_num)\
    if ((line) == ((line_num)+60)) {(line)=0; return CMD_CONTINUE;}\
    if (((line)<((line_num)+60)) && ((++(line))>line_num))


void qos_car_init(int size);             /* 初始化 car profile hash 表 */
int qos_car_profile_add(struct car_t *pcar); /* 添加到 car profile */
int qos_car_profile_delete(uint32_t car_id);/* 从 car profile 删除 */
struct car_t *qos_car_profile_lookup(uint32_t car_id);/* 从 car profile 查找 */
int qos_car_profile_get_bulk ( uint16_t index, struct car_t *car_array );
int qos_car_profile_param_set(struct car_t *pold, struct car_t *pcar);
int qos_car_profile_color_action_set(struct car_t *pold, struct car_action *paction, enum CAR_INFO type);
int qos_car_profile_color_mode_set(struct car_t *pold, uint8_t mode);
int qos_car_profile_set(struct car_t *pcar, void *pdata, enum CAR_INFO type);/* 设置 car profile 的参数 */
struct car_t *qos_car_create(struct car_t *pcar); /* 创建 car 数据结构 */


/* 接口配置 car 的操作 */
int qos_if_add_car(uint32_t ifindex, enum QOS_DIR dir, struct car_t *pcar); /* 接口配置 car 参数 */
int qos_if_add_car_profile(uint32_t ifindex, enum QOS_DIR dir, uint32_t car_id); /* 接口应用 car profile */
int qos_if_delete_car(struct qos_if *pif, enum QOS_DIR dir);



void qos_car_profile_attached (struct car_t *pcar, int flag);
void qos_car_cmd_init(void);


#endif



