#ifndef HIOS_IFM_CLI_H
#define HIOS_IFM_CLI_H

#include "command.h"
#include "types.h"
#include "ifm.h"


#define IFM_GET_INTEGER_RANGE_HEX(NAME,V,STR,MIN,MAX)                         \
            do {                                                                      \
              unsigned int retv;                                                      \
              u_int32_t tmp;                                                        \
              tmp = cmd_hexstr2int ((STR), &retv);                                    \
              if (retv == -1 || (tmp) < (MIN) || (tmp) > (MAX))                           \
                {                                                                     \
                  vty_out (vty, "%% Invalid %s value%s", NAME, VTY_NEWLINE);                      \
                  return CMD_WARNING;                                                 \
                }                                                                     \
                (V) = tmp;                                                           \
            } while (0)

/*added by liub 2018-4-25*/
#define FLOW_CONTROL_SHOW_HEAD(vty)   \
do{\
    vty_out ( vty, "Flow Control list:%s", VTY_NEWLINE );\
    vty_out ( vty, "-------------------------------------------------------------------------------%s", VTY_NEWLINE );\
    vty_out ( vty, "%-25s %9s %10s %12s %12s %s","INTERFACE","Send","Receive", "RxPause", "TxPause", VTY_NEWLINE );\
    vty_out ( vty, "-------------------------------------------------------------------------------%s", VTY_NEWLINE );\
}while(0);

#define FLOW_CONTROL_SHOW_END(vty)   \
do{\
    vty_out ( vty, "-------------------------------------------------------------------------------%s", VTY_NEWLINE );\
} while ( 0 );

int flow_control_print (struct vty *vty, uint32_t ifindex);
void ifm_flow_control_show ( struct vty *vty );

const char *ifm_get_sub_typestr ( enum IFNET_SUBTYPE subtype );
u_int32_t cmd_hexstr2int ( char *str, int *ret );
int cmd_check_macform ( char *mac_string );
void ifm_cli_init ( void );
void ifm_brief_show ( struct vty *vty,  struct ifm_entry *pifm );
void ifm_type_brief_show ( struct vty *vty,  enum IFNET_TYPE type );
void ifm_all_brief_show ( struct vty *vty );
void ifm_interface_config_show ( struct vty *vty,  struct ifm_entry *pifm );
void ifm_all_config_show ( struct vty *vty );
void ifm_type_config_show ( struct vty *vty,  enum IFNET_TYPE type );
void ifm_encap_config_write ( struct vty *vty,  struct ifm_info ifmifo );
void ifm_interface_config_write ( struct vty *vty, struct ifm_entry *pifm );
int ifm_config_write ( struct vty *vty );
void ifm_encap_config_show ( struct vty *vty,  struct ifm_entry *pifm );

#endif
