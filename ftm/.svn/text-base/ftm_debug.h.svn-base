#ifndef FTM_DEBUG_H
#define FTM_DEBUG_H

#include "ftm_ifm.h"

struct ftm_info{
	u_int32_t recvdebugs;		/* bitmap for receive debug */
	u_int32_t senddebugs;		/* bitmap for send debug */
};
	  


void ftm_debug_init (void);
int cmd_str2int (char *str, int *ret);
void ftm_ifm_config_show ( struct vty *vty,  struct ftm_ifm *p_ftm_ifm );

#endif

