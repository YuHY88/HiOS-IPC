/******************************************************************************
 * Filename: alarm_cmd.c
 *	Copyright (c) 2016-2016 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2017.11.17  lipf created
 *
******************************************************************************/
#include <zebra.h>

#include "lib/memory.h"
#include "lib/vty.h"
#include "lib/command.h"
#include "lib/linklist.h"
#include "lib/log.h"

#include "gpnAlmDataSheet.h"
#include "gpnAlmCmd.h"
#include "gpnAlmCmdShow.h"
#include "gpnAlmDebug.h"

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

struct cmd_node alarmCliNode = {ALARM_NODE, "%s(config-alarm)# ", 1};

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

/**************************** alarm node ****************************/

DEFUN (alarm_mode_enable,
		alarm_mode_enable_cmd,
		"alarm",
		"alarm command node\n")
{
	vty->node = ALARM_NODE;

	return CMD_SUCCESS;
}



/**************************** alarm node ****************************/

DEFUN (alarm_show_code_fun,
       alarm_show_code_cmd,
       "show alarm code",
       SHOW_STR
       "Display alarm information\n"
       "Display alarm code define\n")
{
    alarm_show_alarm_code (vty);
    
    return(CMD_SUCCESS);
}

DEFUN (alarm_show_stype_fun,
       alarm_show_stype_cmd,
       "show alarm subtype",
       SHOW_STR
       "Display alarm information\n"
       "Display alarm subtype define\n")
{
    int iDLen = 0;

    //iDLen = alarm_code_stype_show (vty_out, vty);

    if(iDLen <= 0) vty_out(vty, "Show alarm subtype list error\r\n");

    return(CMD_SUCCESS);
}

DEFUN (alarm_show_scount_fun,
       alarm_show_scount_cmd,
       "show alarm statistic",
       SHOW_STR
       "Display alarm information\n"
       "Display alarm statistics(for subtype)\n")
{
    int iDLen = 0;

    //iDLen = alarm_code_count_show (vty_out, vty);

    if(iDLen <= 0) vty_out(vty, "Show alarm subtype statics error\r\n");

    return(CMD_SUCCESS);
}

DEFUN (alarm_show_config_fun,
       alarm_show_config_cmd,
       "show alarm config",
       SHOW_STR
       "Display alarm information\n"
       "Display alarm config\n")
{
    int iDLen = 0;

    //iDLen = alarm_conf_show (vty_out, vty);

    if(iDLen <= 0) vty_out (vty, "Show alarm config error\r\n");
         
    return(CMD_SUCCESS);
}

DEFUN (alarm_show_record_fun,
       alarm_show_record_cmd,
       "show alarm record (history|current|event)",
       SHOW_STR
       "Display alarm information\n"
       "Display alarm record\n"
       "Display alarm history record\n"
       "Display alarm current record\n"
       "Display alarm event record\n")
{    	
    if(argv[0][0] == 'h')
    {
        alarm_show_hist_alarm (vty);
    }
    else if(argv[0][0] == 'c') 
    {
        alarm_show_curr_alarm (vty);
    }
    else //if(argv[0][0] == 'e') 
    {
        alarm_show_event (vty);
    }

    return(CMD_SUCCESS);
}


DEFUN (alarm_show_register_info_fun,
	  alarm_show_register_info_cmd,
	  "show alarm register",
	  SHOW_STR
	  "Display alarm information\n"
	  "Display alarm register information\n")
{	   
   alarm_show_register_info(vty);
   return(CMD_SUCCESS);
}


/********************************************** debug ***********************************************/

const struct message alarm_debug_name[] =
{
	{.key = ALARM_DBG_REGISTER,	.str = "register"},
	{.key = ALARM_DBG_REPORT,	.str = "report"},
	{.key = ALARM_DBG_GET,	  	.str = "get"},
	{.key = ALARM_DBG_GET_NEXT,	.str = "get-next"},
	{.key = ALARM_DBG_SET, 	  	.str = "set"},
	{.key = ALARM_DBG_SYNC, 	.str = "sync"},
	{.key = ALARM_DBG_ALL,	  	.str = "all"}
};


DEFUN (alarm_debug_monitor,
	alarm_debug_monitor_cmd,
	"debug alarm (enable|disable) (register|report|get|get-next|set|sync|all)",
	"Debug information to moniter\n"
	"Programe name\n"
	"Enable statue\n"
	"Disatble statue\n"
	"Type name of register messege\n"
	"Type name of report messege\n"
	"Type name of get log queue \n"
	"Type name of get-next log queue \n"
	"Type name of set status\n"
	"Type name of sync status\n"
	"Type name of all debug\n")
{
	unsigned int typeid = 0;
	int zlog_num;

	for(zlog_num = 0; zlog_num < array_size(alarm_debug_name); zlog_num++)
	{
		if(!strncmp(argv[1], alarm_debug_name[zlog_num].str, 4))
		{
			zlog_debug_set( vty, alarm_debug_name[zlog_num].key, !strncmp(argv[0], "enable", 3));

			return CMD_SUCCESS;
		}
	}

	vty_out (vty, "No debug typd find %s", VTY_NEWLINE);

	return CMD_SUCCESS;
}

/* æ˜¾ç¤ºä¸ªdebugä¿¡æ¯çš„çŠ¶æ€	*/
DEFUN (show_alarm_debug_monitor,
	show_alarm_debug_monitor_cmd,
	"show alarm debug",
	SHOW_STR
	"Syslog"
	"Debug status\n")
{
	int type_num;

	vty_out(vty, "debug type		   status %s", VTY_NEWLINE);

	for(type_num = 0; type_num < array_size(alarm_debug_name); ++type_num)
	{
		vty_out(vty, "%-15s	 %-10s %s", alarm_debug_name[type_num].str, 
			!!(vty->monitor & (1 << type_num)) ? "enable" : "disable", VTY_NEWLINE);
	}

	return CMD_SUCCESS;
}


/*******************************************************************************************************/



	   

#if 0
DEFUN (alarm_show_hash_fun,
       alarm_show_hash_cmd,
       "show alarm hash record",
       SHOW_STR
       "Display alarm information\n"
       "Display alarm hash table\n"
       "Display alarm record hash table\n")
{
    int iDLen = 0;

    //iDLen = alarm_record_hash_show(vty_out, vty);

    if(iDLen <= 0) vty_out(vty, "Show alarm record hash table error\r\n");
         
    return(CMD_SUCCESS);
}

DEFUN (alarm_show_ifhash_fun,
       alarm_show_ifhash_cmd,
       "show alarm ifcount (subtype|hash)",
       SHOW_STR
       "Display alarm information\n"
       "Display alarm if record count\n"
       "Display alarm if record count by hash\n"
       "Display alarm if record count by subtype\n")
{
    int iDLen = 0;

    if(argv[0][0] == 'h')
    {
        //iDLen = alarm_ifman_hash_show(vty_out, vty); 
    }
    else 
    {
        //iDLen = alarm_ifman_stype_show(vty_out, vty); 
    }

    if(iDLen <= 0) vty_out(vty, "Show alarm if record table error\r\n");
         
    return(CMD_SUCCESS);
}

DEFUN (alarm_show_almstat_fun,
       alarm_show_almstat_cmd,
       "show alarm state subtype WORD",
       SHOW_STR
       "Display alarm information\n"
       "Display alarm current state\n"
       "Display special alarm type's alarm state\n"
       "Input special alarm type name\n")
{
    int iDLen = 0;
    unsigned short iAlmT = 0;
    
    //iAlmT = alarm_code_stype_id(argv[0]);

    if(iAlmT == 0)
    {
        vty_out(vty, "Input alarm type name error!\r\n");
    }
    else
    {
       //iDLen = alarm_ifman_almst_show(vty_out, vty, iAlmT); 
       
       if(iDLen <= 0) vty_out(vty, "Show alarm state error\r\n");
    }

    return(CMD_SUCCESS);
}

DEFUN (alarm_show_inhblogic_fun,
       alarm_show_inhblogic_cmd,
       "show alarm suppress logic",
       SHOW_STR
       "Display alarm information\n"
       "Display alarm suppress logic\n"
       "Display alarm suppress logic\n")
{
    
    int iDLen = 0;
    
    //iDLen = alarm_code_inblgc_show(vty_out,vty);

    if(iDLen <= 0) vty_out(vty, "Show alarm suppress logic error\r\n");
}

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
DEFUN (alarm_conf_debug_fun,
       alarm_conf_debug_cmd,
       "debug alarm (proc|ifm|all)",
       "Debuv config\n"
       "Alarm config\n"
       "Alarm process debug\n"
       "Alarm interface debug\n"
       "Alarm all debug\n")
{
    int iflag = 0;

#if 0   
    if(argv[0][0] == 'p')
         iflag = ALM_DEBUG_ALMMSG;
    else if(argv[0][0] == 'i')
         iflag = ALM_DEBUG_ALMDEV;
    else iflag = ALM_DEBUG_ALMMSG | ALM_DEBUG_ALMDEV;

    alarm_conf_debugset(iflag);
#endif

    return(CMD_SUCCESS);
}

DEFUN (alarm_conf_nodebug_fun,
       alarm_conf_nodebug_cmd,
       "no debug alarm (proc|ifm|all)",
       "No operate\n"
       "Debuv config\n"
       "Alarm config\n"
       "Alarm process debug\n"
       "Alarm interface debug\n"
       "Alarm all debug\n")
{
    int iflag = 0;

#if 0    
    if(argv[0][0] == 'p')
         iflag = ALM_DEBUG_ALMMSG;
    else if(argv[0][0] == 'i')
         iflag = ALM_DEBUG_ALMDEV;
    else iflag = ALM_DEBUG_ALMMSG | ALM_DEBUG_ALMDEV;

    alarm_conf_debugclr(iflag);
#endif

    return(CMD_SUCCESS);
}

DEFUN (alarm_conf_suppress_fun,
       alarm_conf_suppress_cmd,
       "alarm suppress (enable|disable)",
       "Alarm config\n"
       "Alarm suppress config\n"
       "Alarm suppress enable\n" 
       "Alarm suppress disable\n")
{
#if 0
    if(argv[0][0] == 'e')
         alarm_conf_suppresset(1);
    else alarm_conf_suppresset(0);
#endif
    
    return(CMD_SUCCESS);
}
/*
DEFUN (alarm_conf_nosuppress_fun,
       alarm_conf_nosuppress_cmd,
       "no alarm suppress",
       "No operate\n"
       "Alarm config\n"
       "Alarm suppress enable\n")
{
    alarm_conf_suppresset(0);

    return(CMD_SUCCESS);
}
*/

DEFUN (alarm_conf_repdly_fun,
       alarm_conf_repdly_cmd,
       "alarm delay <1-10>",
       "Alarm config\n"
       "Alarm report(trap) delay time\n"
       "Input delay time 1-10 seconds\n")
{
    //alarm_conf_repdelayset(atoi(argv[0]));

    return(CMD_SUCCESS);
}

DEFUN (alarm_conf_filesize_fun,
       alarm_conf_filesize_cmd,
       "alarm file size <10-100>",
       "Alarm config\n"
       "Alarm file config\n"
       "Alarm file size config\n"
       "Input file size 10-100M\n")
{
    //alarm_conf_filesizeset(atoi(argv[0]));

    return(CMD_SUCCESS);
}

DEFUN (alarm_conf_filewrite_fun,
       alarm_conf_filewrite_cmd,
       "alarm file write",
       "Alarm config\n"
       "Alarm file config\n"
       "Write alarm file buffer to file\n")
{
    //alarm_file_alarm_wrcmd();

    return(CMD_SUCCESS);
}

DEFUN (alarm_conf_typemask_fun,
       alarm_conf_typemask_cmd,
       "alarm shield subtype WORD",
       "Alarm config\n"
       "Alarm shield config\n"
       "Alarm subtype shied config\n"
       "Input subtype name\n")
{
    unsigned short iAlmT = 0;

#if 0
    if(argv[0] != NULL)
    {
        iAlmT = alarm_code_stype_id(argv[0]);

        if(iAlmT == 0)
             vty_out(vty, "Input alarm type name error!\r\n");
        else alarm_conf_typemaskset(iAlmT, 1);
          
    }
#endif

    return(CMD_SUCCESS);
}

DEFUN (alarm_conf_typenomask_fun,
       alarm_conf_typenomask_cmd,
       "no alarm shield subtype WORD",
       "No operate\n"
       "Alarm config\n"
       "Alarm shield config\n"
       "Alarm subtype shied config\n"
       "Input subtype name\n")
{
    unsigned short iAlmT = 0;

#if 0   
    if(argv[0] != NULL)
    {
        iAlmT = alarm_code_stype_id(argv[0]);

        if(iAlmT == 0)
             vty_out(vty, "Input alarm type name error!\r\n");
        else alarm_conf_typemaskset(iAlmT, 0);
    }
#endif

    return(CMD_SUCCESS);
}

DEFUN (alarm_conf_levelfilt_fun,
       alarm_conf_levelfilt_cmd,
       "alarm to server level (all|event|notice|minor|major|critical)",
       "Alarm config\n"
       "Alarm report config\n"
       "Alarm report to MGT config\n"
       "Report to MGT alarm level\n"
       "All alarm Report to MGT\n"
       "Report to MGT lowest level event\n"
       "Report to MGT lowest level notice\n"
       "Report to MGT lowest level minor\n"
       "Report to MGT lowest level major\n"
       "Report to MGT lowest level critical\n")
{
    int  iRetV = 0;
    char iLvel = 0;

#if 0    
    if(argv[0] != NULL)
    {
        if(argv[0][0] == 'a')
             iLvel = ALM_LEVEL_UNKNOWN;
        else if(argv[0][0] == 'e')
             iLvel = ALM_LEVEL_EVENT;
        else if(argv[0][0] == 'n')
             iLvel = ALM_LEVEL_NOTICE;
        else if(argv[0][0] == 'c')
             iLvel = ALM_LEVEL_CRITICAL;
        else if((argv[0][0] == 'm') && (argv[0][1] == 'i'))
             iLvel = ALM_LEVEL_MINOR;
        else if((argv[0][0] == 'm') && (argv[0][1] == 'a'))
             iLvel = ALM_LEVEL_MAJOR;
        else iLvel = ALM_LEVEL_UNKNOWN;

        iRetV = alarm_conf_serverlvlset(iLvel);
    }
#endif

    return(CMD_SUCCESS);
}

DEFUN (alarm_conf_typefilt_fun,
       alarm_conf_typefilt_cmd,
       "alarm filt subtype WORD name WORD",
       "Alarm config\n"
       "Alarm filt config\n"
       "Alarm subtype filt config\n"
       "Input subtype name\n"
       "Interface or service name\n"
       "Input name\n")
{
    int            iRetV = 0;
    unsigned short iAlmT = 0;

#if 0    
    if(argv[0] != NULL)
    {
        iAlmT = alarm_code_stype_id(argv[0]);

        if(iAlmT == 0)
        {
            vty_out(vty, "Input alarm type name error!\r\n");
        }
        else
        {
           iRetV = alarm_conf_filtadd(ALM_FILT_TYPE_IFANDTYPE, iAlmT, argv[1]);

           if(iRetV != 0) vty_out(vty, "Input alarm if name error!\r\n");
        }
    }
#endif

    return(CMD_SUCCESS);
}

DEFUN (alarm_conf_typenofilt_fun,
       alarm_conf_typenofilt_cmd,
       "no alarm filt subtype WORD name WORD",
       "No operate\n"
       "Alarm config\n"
       "Alarm filt config\n"
       "Alarm subtype filt config\n"
       "Input subtype name\n"
       "Interface or service name\n"
       "Input name\n")
{
    int            iRetV = 0;
    unsigned short iAlmT = 0;

#if 0    
    if(argv[0] != NULL)
    {
        iAlmT = alarm_code_stype_id(argv[0]);

        if(iAlmT == 0)
        {
            vty_out(vty, "Input alarm type name error!\r\n");
        }
        else
        {
           iRetV = alarm_conf_filtdel(ALM_FILT_TYPE_IFANDTYPE, iAlmT, argv[1]);

           if(iRetV != 0) vty_out(vty, "Input alarm if name error!\r\n");
        }
    }
#endif

    return(CMD_SUCCESS);
}
#endif

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
static int alarm_config_write(struct vty *vty)
{
#if 0
    int            iGetVa = 0;
    unsigned short iIndex = 0;

    AlarmSubType *pTypeItm = NULL;

    if(!alarm_conf_suppresget())
    {
        vty_out(vty, "alarm suppress disable%s", VTY_NEWLINE);
    }

    iGetVa = alarm_conf_repdelayget();
    if(iGetVa != ALM_RECO_REPORT_DELAY)
    {
        vty_out(vty, "alarm delay %d%s", iGetVa, VTY_NEWLINE);
    }

    iGetVa = alarm_conf_filesizeget();
    if(iGetVa != ALM_MEDIA_FILE_MAXLEN)
    {
        vty_out(vty, "alarm file size %d%s", iGetVa, VTY_NEWLINE);
    }

    iGetVa = (int)alarm_conf_mgtlevelget();
    if(iGetVa != ALM_LEVEL_EVENT)
    {
        vty_out(vty, "alarm to server level %s%s", alarm_code_level_name((unsigned char)iGetVa), VTY_NEWLINE);
    }

   
    for(iIndex = 0; iIndex < ALM_STYPE_NUMMAX; iIndex++)
    {
        pTypeItm = alarm_code_stype_table(iIndex);

        if(pTypeItm && pTypeItm->iMasked)
        {
           vty_out(vty, "alarm shield subtype %s%s", pTypeItm->bName, VTY_NEWLINE);
        }
    }

#endif
    vty_out ( vty, "!%s", VTY_NEWLINE );

	return 0;
}
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*------------------------------------------------------------*/
/*name: alarm_cmd_init                                        */
/*desc: ¸æ¾¯Ä£¿éÃüÁîÐÐ³õÊ¼»¯                                  */
/*------------------------------------------------------------*/
void alarm_cmd_init (void)
{
    install_node(&alarmCliNode, alarm_config_write );
	install_default(ALARM_NODE);

	/* alarm enter cmd */
	install_element(CONFIG_NODE, &alarm_mode_enable_cmd, CMD_LOCAL);
	
    install_element(ALARM_NODE, &alarm_show_code_cmd, CMD_LOCAL);
	install_element(ALARM_NODE, &alarm_show_record_cmd, CMD_LOCAL);
	install_element(ALARM_NODE, &alarm_show_register_info_cmd, CMD_LOCAL);
	
#if 0
    install_element(ALARM_NODE, &alarm_show_stype_cmd, CMD_LOCAL);
    install_element(ALARM_NODE, &alarm_show_scount_cmd, CMD_LOCAL);
    install_element(ALARM_NODE, &alarm_show_config_cmd, CMD_LOCAL);    
 // install_element(ALARM_NODE, &alarm_show_hash_cmd, CMD_LOCAL);
 // install_element(ALARM_NODE, &alarm_show_ifhash_cmd, CMD_LOCAL);
    install_element(ALARM_NODE, &alarm_show_almstat_cmd, CMD_LOCAL);
    install_element(ALARM_NODE, &alarm_show_inhblogic_cmd, CMD_LOCAL);
    
    install_element(ALARM_NODE, &alarm_conf_nodebug_cmd, CMD_LOCAL);
    install_element(ALARM_NODE, &alarm_conf_debug_cmd, CMD_LOCAL);
    
    install_element(ALARM_NODE, &alarm_conf_repdly_cmd, CMD_LOCAL);
    install_element(ALARM_NODE, &alarm_conf_filewrite_cmd, CMD_LOCAL);
    install_element(ALARM_NODE, &alarm_conf_filesize_cmd, CMD_LOCAL);
    install_element(ALARM_NODE, &alarm_conf_levelfilt_cmd, CMD_LOCAL);
    install_element(ALARM_NODE, &alarm_conf_typemask_cmd, CMD_LOCAL);
    install_element(ALARM_NODE, &alarm_conf_typenomask_cmd, CMD_LOCAL);
  //install_element(ALARM_NODE, &alarm_conf_typefilt_cmd, CMD_LOCAL);
  //install_element(ALARM_NODE, &alarm_conf_typenofilt_cmd, CMD_LOCAL);
    install_element(ALARM_NODE, &alarm_conf_suppress_cmd, CMD_LOCAL);
  //install_element(ALARM_NODE, &alarm_conf_nosuppress_cmd, CMD_LOCAL);
#endif


	/* new debug */
	install_element (CONFIG_NODE, &alarm_debug_monitor_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &show_alarm_debug_monitor_cmd, CMD_SYNC);
}
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/



