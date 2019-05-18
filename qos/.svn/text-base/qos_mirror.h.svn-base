
#ifndef HIOS_QOS_MIRROR_H
#define HIOS_QOS_MIRROR_H

#include <lib/types.h>
#include <lib/linklist.h>
#include <lib/qos_common.h>


#define MIRROR_GROUP_NUM   			4  	/* 镜像组的个数 */
#define MIRROR_GROUP_PORT_MAX_NUM  	4	/* 加到镜像组port 最多个数 */


struct qos_if_mirror
{
	uint32_t ifindex;
	uint8_t  mirror_group;     /* 镜像组 id */
	uint32_t mirror_if;        /* mirror 到出接口,mirror_if 与mirror_group不同时存在 */
	enum QOS_DIR direct;	   /* 方向支持出，入，双向*/
	enum QOS_INFO type;        /* 映射类型*/
};	



/* 镜像组的数据结构 */
struct mirror_group
{
	uint8_t  id;	          /* 镜像组的 id */
	uint32_t index;           /* 接口索引*/
	struct list  *pport_list; /* 镜像组的监视接口链表 */
};


extern struct mirror_group mgroup[MIRROR_GROUP_NUM]; 


/* 接口配置 mirror_to  group的操作 */
void qos_mirror_init(void);                         		    			/* init mgroup */
int qos_mirror_delete_group(uint8_t group_id);
int qos_mirror_group_add_port(uint8_t group_id, uint32_t mirror_if);   	/*add mirror_if to mirror_group */
int qos_mirror_group_delete_port(uint8_t group_id, uint32_t mirror_if);	/*delete mirror_if from mirror_group*/

int qos_if_add_mirror_group (uint32_t ifindex, uint8_t group_id, enum QOS_DIR direct);	  	/*interface mirror_to  group*/
int qos_if_delete_mirror_group (uint32_t ifindex);	/*interface no mirror_to group*/


/* 接口配置 mirror_to mirror_if的操作 */
int qos_if_add_mirror_port(uint32_t ifindex, uint32_t mirror_if, enum QOS_DIR direct);     	/* interface mirror_to mirror_if*/
int qos_if_delete_mirror_port(uint32_t ifindex, uint32_t mirror_if);	  	/*interface no mirror_to mirror_if*/

void qos_mirror_cmd_init(void);
char* qos_mirror_direct_parse_str(enum QOS_DIR direct);

int qos_mirror_group_get_bulk ( uint32_t index, uint8_t group_id, struct qos_mirror_snmp *pmirror);
int qos_mirror_port_get_bulk ( uint32_t index, struct qos_if_mirror *pif_mirror);

int qos_if_mirror_already_occupy(uint32_t mirror_if, uint8_t group_id, uint32_t index);

#endif



