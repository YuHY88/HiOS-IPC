/**
 * @file      : labelm.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月8日 14:48:13
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_LABELM_H
#define HIOS_LABELM_H

#include <lib/types.h>
#include <lib/index.h>

#define LABEL_POOL_MAX   32   /* 第 32 个标签池留给保留标签 */

extern struct index_bitmap label_pool[LABEL_POOL_MAX];

enum label_state
{
    LABEL_ALLOC = 1,
    LABEL_PENDING,
    LABEL_INUSED
};
extern void label_pool_init(void);
extern int label_alloc(uint32_t module_id);
extern int label_get(uint32_t label, uint32_t module_id);
extern int label_free(uint32_t label);
extern int label_min_alloc(uint32_t label_min);

#endif


