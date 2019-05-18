#ifndef HIOS_ERPS_CMD_H
#define HIOS_ERPS_CMD_H

#include <lib/types.h>
#include "raps/erps.h"
#include <lib/vty.h>




#define ERPS_SESSION_ALREADY_ENABLE(vty, status)\
    if((status) == SESSION_STATUS_ENABLE)\
    {\
        vty_out(vty, "Error:Erps-session is already enable.%s", VTY_NEWLINE);\
        return CMD_WARNING;\
    }

#define ERPS_SESSION_IS_DISABLE(vty, status)\
    if((status) == SESSION_STATUS_DISABLE)\
    {\
        vty_out(vty, "Error:Erps-session is not enable.%s", VTY_NEWLINE);\
        return CMD_WARNING;\
    }



void erps_cmd_init(void);
void eprs_show_config(struct vty *vty, struct erps_sess *psess);
void erps_show_session(struct vty *vty, struct erps_sess *psess);
void erps_vlan_show(struct vty *vty, struct erps_sess *psess, uint8_t is_write);

#endif
