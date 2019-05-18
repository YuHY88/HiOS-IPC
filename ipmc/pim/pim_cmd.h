#ifndef _PIM_CMD_H_
#define _PIM_CMD_H_


//struct cmd_node pim_instance_node;

void pim_cmd_init();

int pim_instance_config_write (struct vty *vty);

#endif /* _PIM_CMD_H_ */

