
#include <zebra.h>
#include "command.h"
#include "vtysh.h"

DEFSH(VTYSH_SDHMGT,
      sdhmgt_log_level_ctl_cmd_vtysh,
      "debug sdhmgt (enable | disable)",
      "Output log of debug level\n"
      "Program name\n"
      "Enable\n""Disable\n");

DEFSH(VTYSH_SDHMGT,
      sdhmgt_show_log_level_ctl_cmd_vtysh,
      "show debug sdhmgt",
      SHOW_STR
      "Output log of debug level\n"
      "Program name\n");

DEFSH(VTYSH_SDHMGT,
      debug_sdhmgt_packet_enable_cmd,
      "debug sdhmgt packet enable",
      "Debug control\n"
      "sdhmgt module\n"
      "packet debug\n"
      "enable\n");

DEFSH(VTYSH_SDHMGT,
      debug_sdhmgt_packet_disable_cmd,
      "debug sdhmgt packet disable",
      "Debug control\n"
      "sdhmgt module\n"
      "packet debug\n"
      "disable\n");

DEFSH(VTYSH_SDHMGT,
      show_debug_sdhmgt_packet_display_cmd,
      "show debug sdhmgt packet",
      SHOW_STR
      "Debug control\n"
      "sdhmgt module\n"
      "packet debug\n");

void vtysh_init_sdhmgt_cmd()
{
    install_element_level(CONFIG_NODE, &sdhmgt_log_level_ctl_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &sdhmgt_show_log_level_ctl_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);

    install_element_level(CONFIG_NODE, &debug_sdhmgt_packet_enable_cmd, MANAGE_LEVE, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &debug_sdhmgt_packet_disable_cmd, MANAGE_LEVE, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &show_debug_sdhmgt_packet_display_cmd, MANAGE_LEVE, CMD_LOCAL);
}

