#ifndef HIOS_MPLS_APS_CMD_H
#define HIOS_MPLS_APS_CMD_H

#include <lib/vty.h>
#include "mpls_aps.h"




#define MPLSAPS_SESSION_ALREADY_ENABLE(vty, status)\
    if((status) != MPLS_APS_STATUS_DISABLE)\
    {\
        vty_out(vty, "Error:Mplsaps-session is already enable.%s", VTY_NEWLINE);\
        return CMD_WARNING;\
    }

#define MPLSAPS_SESSION_IS_DISABLE(vty, status)\
    if((status) == MPLS_APS_STATUS_DISABLE)\
    {\
        vty_out(vty, "Error:Mplsaps-session is not enable.%s", VTY_NEWLINE);\
        return CMD_WARNING;\
    }  

void mplsaps_cmd_init(void);
int mplsaps_session_config_write(struct vty *vty);
void mplsaps_show_config(struct vty *vty, struct aps_sess *psess);

void mplsaps_show_session(struct vty *vty, struct aps_sess *psess);


#endif





