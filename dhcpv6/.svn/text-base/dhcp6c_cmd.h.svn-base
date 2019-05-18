#ifndef HIOS_DHCP6C_CMD_H
#define HIOS_DHCP6C_CMD_H

int dhcpv6_key_set(char *key_string);
void dhcp6c_cmd_init(void);
int dhcpv6_duid_set(char *duid_string);
char * show_dhcpv6_duid(void);
int dhcpv6_if_get_intf_mode(struct vty *vty, uint32_t module_id);
int show_dhcpv6_client_status(struct vty *vty, struct dhcpv6_if *ifp);
int set_interface_ia(uint32_t ifindex, char *iana, iatype_t type);
int dhcp6c_cmd_config_write (struct vty *vty);
char *show_dhcpv6_key(void);
#endif
