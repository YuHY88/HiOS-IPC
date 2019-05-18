

#ifndef HIOS_MPLSTP_OAM_CMD_H
#define HIOS_MPLSTP_OAM_CMD_H

#include "mpls_oam/mpls_oam.h"


#define OAM_LM_DEF_INTERVAL      (10)
#define OAM_LM_DEF_FREQUENCY     (10)
#define OAM_DM_DEF_INTERVAL      (10)
#define OAM_DM_DEF_FREQUENCY     (10)


#define MPLSOAM_SESSION_ALREADY_ENABLE(vty, type)\
    if((type) != OAM_TYPE_INVALID)\
    {\
        vty_error_out(vty, "Session is already bind service.%s", VTY_NEWLINE);\
        return CMD_WARNING;\
    }

#define MPLSOAM_SESSION_IS_DISABLE(vty, type)\
if((type) == OAM_TYPE_INVALID)\
    {\
        vty_error_out(vty, "Oam-session is not enable.%s", VTY_NEWLINE);\
        return CMD_WARNING;\
    }

#define MPLSOAM_SESSION_NOT_SERVICE_MEG(vty, meg, session)\
	if((meg) == NULL)\
	{\
		vty_error_out(vty, "Session:%d is not service meg.%s", session, VTY_NEWLINE);\
		return CMD_WARNING;\
	}


#define MPLSOAM_MPTYPE_NOT_MIP(vty, mp_type)\
	if((mp_type) != MP_TYPE_MIP)\
	{\
		vty_error_out(vty, "Not mip.%s", VTY_NEWLINE);\
		return CMD_WARNING;\
	}
	
#define MPLSOAM_MPTYPE_MIP(vty, mp_type)\
	if((mp_type) == MP_TYPE_MIP)\
	{\
		vty_error_out(vty, "Mip node.%s", VTY_NEWLINE);\
		return CMD_WARNING;\
	}



void show_mplsoam_session_current_alarm(struct vty *vty, struct oam_session *psess);
void show_mplsoam_session_lm_result(struct vty *vty, struct oam_session *psess);
void show_mplsoam_session_dm_result(struct vty *vty, struct oam_session *psess);

void show_mplsoam_session(struct vty *vty, struct oam_session *psess);

void mplstp_oam_cmd_init(void);


#endif

