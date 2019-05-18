#ifndef HIOS_QOS_MAPPING_CMD_H
#define HIOS_QOS_MAPPING_CMD_H

#include <lib/vty.h>
#include <qos/qos_mapping.h>


#define QOS_MAPPING_PROMPT_GET(_type,_str)\
do{                                       \
switch(_type){                        \
    case QOS_TYPE_COS:                \
        _str = "cos";                 \
        break;                        \
    case QOS_TYPE_TOS:                \
        _str = "tos";                 \
        break;                        \
    case QOS_TYPE_EXP:                \
        _str = "exp";                 \
        break;                        \
    case QOS_TYPE_DSCP:               \
        _str = "dscp";                \
        break;                        \
    default:                          \
        break;                        \
}                                     \
}while(0) 



void qos_mapping_cmd_init(void);/*命令行初始化*/

#endif
