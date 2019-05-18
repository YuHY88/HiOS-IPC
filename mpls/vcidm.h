/*
*    标签管理模块
*
*/


#ifndef HIOS_VCIDM_H
#define HIOS_VCIDM_H

#include <lib/types.h>

void vcid_init(void);

/*  随机分配标签 */
int vcid_alloc(void);

/* 分配指定的 vcid */
int vcid_get(uint32_t vcid);

/* 释放指定的 vcid */
int vcid_free(uint32_t label);

/*  分配比 vcid_min 大的标签 */
int vcid_min_alloc(uint32_t vcid_min);




#endif

