#ifndef HIOS_ELPS_CMD_H
#define HIOS_ELPS_CMD_H

#include <lib/vty.h>
#include "aps/elps.h"



#define ELPS_SESSION_ALREADY_ENABLE(vty, status)\
    if((status) != APS_STATUS_DISABLE)\
    {\
        vty_out(vty, "Error:Elps-session is already enable.%s", VTY_NEWLINE);\
        return CMD_WARNING;\
    }

#define ELPS_SESSION_IS_DISABLE(vty, status)\
    if((status) == APS_STATUS_DISABLE)\
    {\
        vty_out(vty, "Error:Elps-session is not enable.%s", VTY_NEWLINE);\
        return CMD_WARNING;\
    }  

void elps_cmd_init(void);
void elps_show_session(struct vty *vty, struct elps_sess *psess);
void epls_show_config(struct vty *vty, struct elps_sess *psess);
void elps_vlan_show(struct vty *vty,struct elps_sess *psess,uint8_t is_write);


#endif

