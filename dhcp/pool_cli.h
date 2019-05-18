#ifndef HIOS_POOL_CLI_H
#define HIOS_POOL_CLI_H

#include <lib/command.h>
#include "pool.h"
#include <lib/types.h>

extern struct cmd_node pool_node;

void dhcp_pool_cmd_init(void);
void show_ip_pool_node_offered( struct vty *vty ,struct ip_pool *pool_node);
void show_ip_pool_node ( struct vty *vty ,struct ip_pool *pool_node);
int dhcp_pool_config_write ( struct vty *vty );

#endif

