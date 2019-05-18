
#include <zebra.h>
#include <lib/memory.h>
#include <lib/inet_ip.h>
#include <arpa/telnet.h>
#include "vtysh.h"
#include "vtysh_telnet_client.h"
#include "vtysh_telnet_client_cmd.h"


void telnelc_debug_show(struct vty *vty)
{
    int i = 0;
    struct telnet_client *tc;

    vty_out(vty, "    sip     sport   dip     dport    userinput  fromserver%s", VTY_NEWLINE);
    for(i = 0; i < TELNET_CLIENT_MAX; i++)
    {
        tc = telnet_data_get(i);
        if(tc->state == CLIENT_OCCUPY)
        {
            vty_out(vty, "%d   %2d.%2d   %5d   %2d.%2d   %d    %5d      %5d%s", i, 
            (tc->sip >> 8) & 0xff, tc->sip & 0xff, tc->sport,  (tc->dip >> 8) & 0xff, 
            tc->dip & 0xff, tc->dport, tc->user_input, tc->from_server, VTY_NEWLINE);
        }
    }
}


DEFUN(vtysh_telnet_host,
      vtysh_telnet_host_cmd,
      "telnet A.B.C.D {vpn-instance <1-128> | source ip A.B.C.D}", 
      "Establish a telnet connection\n"
      "IP address of remote system\n"
      "Specify a VPN instance\n"
      "L3VPN instance value\n"
      "Specify a source \n"
      "IP address of source\n")
{
    uint32_t hostip = 0; 
    uint32_t srcip  = 0;  
    int16_t dport   = TELNET_PORT;  
    uint16_t vpn    = 0;
    int32_t ret     = 0;

    hostip = inet_strtoipv4((char *)argv[0]);

	if(0 == hostip)
	{
		vty_error_out(vty, "The specified IP address is invalid!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

    ret = inet_valid_network(hostip);
    if (!ret)
    {
        vty_error_out(vty, "The specified IP address is invalid!%s", VTY_NEWLINE);

        return CMD_WARNING;
    }     
    
    if(argv[1] != NULL)
    {      
        vpn = atoi(argv[1]);
    }
    
    if(argv[2] != NULL)
    {
        srcip = inet_strtoipv4((char *)argv[2]);

		if(0 == srcip)
		{
			vty_error_out(vty, "The specified IP address is invalid!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}

        ret = inet_valid_network(srcip);
        if (!ret)
        {
            vty_error_out(vty, "The specified IP address is invalid!%s", VTY_NEWLINE);

            return CMD_WARNING;
        } 
    }

    VTY_DEBUG(VD_CLIENT, "hostip:%x, vpn %d, srcip:%x", hostip, vpn, srcip);

    vty_out(vty, "Telnet %s ... %s", argv[0], VTY_NEWLINE);   

    ret = telnetc_telnet_host(vty, srcip, hostip, dport, vpn);
    if(ret != 0)
    {
        vty_out(vty, "Telnet failed !%s", VTY_NEWLINE);
        
        VTY_DEBUG(VD_CLIENT, "telnet host ret %d", ret);
        return CMD_WARNING;
    }

	return CMD_SUCCESS;	
}


DEFUN(vtysh_telnet_client,
      vtysh_telnet_client_debug_cmd,
      "show client",
      "show\n"
      "client\n")
{
    telnelc_debug_show(vty);
    
    return CMD_SUCCESS;
}


DEFUN(vtysh_telnet_host_exit,
      vtysh_telnet_host_exit_cmd,
      "telnet_client exit",
      "Telnet client\n"
      "exit\n")
{
    if(telnelc_debug_console_exit() == 0)
    {
        vty_out(vty, "console client exit %s", VTY_NEWLINE);  
    }
    else
    {
        vty_out(vty, "no console client exit %s", VTY_NEWLINE);
    }
        
    return CMD_SUCCESS;	
}


void telnetc_cmd_reg(void)
{
    install_element_level(CONFIG_NODE, &vtysh_telnet_host_cmd, MONITOR_LEVE_2, CMD_LOCAL);
   
    install_element_level(CONFIG_NODE, &vtysh_telnet_client_debug_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    
    install_element_level(CONFIG_NODE, &vtysh_telnet_host_exit_cmd, MONITOR_LEVE_2, CMD_LOCAL);
}


