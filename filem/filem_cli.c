/* file operate 
   name filem_cli.c
   edit suxq
   date 2016/05/26
*/
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
#include <zebra.h>

#include "lib/memory.h"
#include "lib/vty.h"
#include "lib/log.h"
#include "lib/command.h"
#include "lib/linklist.h"
#include "lib/module_id.h"
#include "lib/msg_ipc_n.h"
#include "lib/vty.h"
#include "lib/memshare.h"
#include "lib/hptimer.h"

#include "filem.h"
#include "filem_oper.h"
#include "filem_tran.h"
#include "filem_img.h"
#include "filem_slot.h"
#include "filem_cmd.h"
#include "filem_tftp.h"
#include "filem_sync.h"

/*------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------*/

static void filem_cli_proccall(int iResult, char *pResInfo, void *pPar, int iFlag)
{
    struct vty *pVty = (struct vty*)pPar;

    if(pVty)  
    {
        if(pResInfo) vty_out(pVty, "%s", pResInfo);

        vtysh_flush(pVty);

        if(iFlag)  vtysh_return(pVty, CMD_SUCCESS);
    }
}

static int filem_cli_typenametoid(char *pName)
{
    int iType = 0;
   
    if(strcmp(pName, "config") == 0)
         iType = FILEM_MTYPE_CFG;
    else if(strcmp(pName, "bin") == 0)
         iType = FILEM_MTYPE_BIN;
    else if(strcmp(pName, "data") == 0)
         iType = FILEM_MTYPE_DAT;
    else if(strcmp(pName, "sys") == 0)
         iType = FILEM_MTYPE_VER;
    else if(strcmp(pName, "temp") == 0)
         iType = FILEM_MTYPE_TMP;
    else iType = FILEM_MTYPE_NON;
 
    return(iType);
}

static int filem_cli_namelencheck(struct vty *vty, char *name)
{
    if(strlen(name) >= FILEM_PATH_MAXLN)  
    {
         vty_error_out(vty, "Input file name too long, the max length is %d bytes%s", 
                       FILEM_PATH_MAXLN-1, VTY_NEWLINE);
         return(0);
    }
    else return(1);
}

static int filem_cli_downloadcomfirm(struct vty *vty, char *bPName, int iType)
{
    int   iRetv = 0;
    char  bTName[FILEM_NAME_MAXLN];

    iRetv = filem_cmd_inputnamecheck(iType, bPName, bTName);

    if(iRetv != 0) 
    {
        vty_error_out(vty, "Download file fail, %s!%s", filem_sys_errget(iRetv), VTY_NEWLINE);

        return(CMD_WARNING);
    }
    
    if(filem_oper_isexist(bTName))
    {
         if(!filem_oper_isdir(bTName))
         {
             switch(vty->cmd_replay)
             {
                 case VTY_REPLAY_NONE:            /*first time exec the cmd, notice user */

                      vty_out(vty, "The local file already exists, whether it is replaced? (Y/N)\r\n");
                      return(CMD_REPLAY);
 
                 case VTY_REPLAY_YES:            /*user confirmed*/
                      return(-FILEM_ERROR_MAXNO);
 
                 case VTY_REPLAY_NO:             /*user canceled*/
                      return(CMD_SUCCESS);
                      
                 default:
                      return(-FILEM_ERROR_MAXNO);
             }
         }
         else
         {
             vty_error_out(vty, "Download file fail, %s!%s", filem_sys_errget(-FILEM_ERROR_DIRNAMECONF), VTY_NEWLINE);
             return(CMD_WARNING);
         }
    }

    return(-FILEM_ERROR_MAXNO);
}

/*------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------*/
//extern void zlog_debug_set(struct vty *, unsigned int, int);

const struct message filem_debug_name[] =
{
	{.key = FILEM_DEBUG_TFTP,       .str = "tftp"},
	{.key = FILEM_DEBUG_UPGRADE,    .str = "upgrade"},
	{.key = FILEM_DEBUG_SYNC,       .str = "sync"},
	{.key = FILEM_DEBUG_TRANS,      .str = "trans"},
	{.key = FILEM_DEBUG_FTP,        .str = "ftp"},
	{.key = FILEM_DEBUG_OTHER,      .str = "other"},
	{.key = FILEM_DEBUG_ALL,        .str = "all"}
};

DEFUN (filem_conf_debug_fun,
       filem_conf_debug_cmd,
       "debug filem (enable|disable|clear) (all|tftp|ftp|upgrade|sync|trans|other)",
       "Debuv config\n"
       "Filem config\n"
       "Filem debug enable\n"
       "Filem debug disable\n"
       "Filem debug set clear\n"
       "Filem debug type all\n"
       "Filem debug type tftp\n"
       "Filem debug type ftp\n"
       "Filem debug type upgrade\n"
       "Filem debug type sync\n"
       "Filem debug type trans\n"
       "Filem debug type other\n")
{
    unsigned int type = 0;

    for(type = 0; type < array_size(filem_debug_name); type++)
	{
		if(strcmp(argv[1], filem_debug_name[type].str) == 0) break;
	}
	
    if(type >= array_size(filem_debug_name))
    {
        vty_error_out(vty, "Filem debug %s fail.%s", argv[0], VTY_NEWLINE);
    }
    else
    {
        if(argv[0][0] == 'e')
        {
             zlog_debug_set(vty, (unsigned int)filem_debug_name[type].key,  1);
        }
        else if(argv[0][0] == 'd')
        {
             zlog_debug_set(vty, (unsigned int)filem_debug_name[type].key,  0);
        }
        else 
        {
             zlog_debug_clear(NULL, (unsigned int)filem_debug_name[type].key);
        }
    }

    return(CMD_SUCCESS);
}


DEFUN (filem_show_debug_fun,
       filem_show_debug_cmd,
       "show debug filem",
        SHOW_STR
       "Debuv config\n"
       "Clear filem debug set\n")
{
     unsigned int index = 0;
     unsigned int type  = 0;
     struct vty* tmpvty = NULL;

     while(1)
     {
         tmpvty = zlog_debug_nextvty(&index);

         if(tmpvty == NULL) break;

	     vty_out(vty, "VTY=%p debug set: %s", tmpvty, VTY_NEWLINE);
	     
	     vty_out(vty, "Debug type      Status %s", VTY_NEWLINE);

         vty_out(vty, "--------------- ------ %s", VTY_NEWLINE);
 
         for(type = 0; type < (array_size(filem_debug_name) - 1); type++)
         {
         	  vty_out(vty, "%-15s:%-10s %s", filem_debug_name[type].str, 
         	         ((tmpvty->monitor & filem_debug_name[type].key) ? "enable" : "disable"), VTY_NEWLINE);
         }  

	     vty_out(vty, "%s", VTY_NEWLINE);
     }

     return(CMD_SUCCESS);

}


DEFUN (filem_show_type_fun,
       filem_show_type_cmd,
       "show file (bin|config|data|sys) {slot <1-31>}",
       SHOW_STR
       "Display file list\n"
       "Display exec file list,app and driver\n"
       "Display config file list\n"
       "Display data file list, log alarm and statistics\n"
       "Display sys version file list\n"
       "Select slot\n"
       "Input slot no\n")
{
    int   iType = 0;
    int   iDLen = 0;
    int   iSlot = 0;
    char *bBuff = NULL;

    iType = filem_cli_typenametoid((char*)argv[0]);

    if((argc == 2) && (argv[1] != NULL))
    {
        iSlot = atoi(argv[1]);

        if((gfilelocslot != iSlot) && !filem_slot_ismaster())
        {
            vty_error_out(vty, "Not master cpu, can't show other slot files.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    
    iDLen = filem_show_filelist(iSlot, iType, NULL, &bBuff);

    if(iDLen < 0)
         vty_error_out(vty, "Show file list error, %s%s", filem_sys_errget(iDLen), VTY_NEWLINE);
    else vty_out(vty, "%s%s", bBuff, VTY_NEWLINE);

    if(bBuff) XFREE(MTYPE_FILEM_ENTRY, bBuff);

    return CMD_SUCCESS;
}

DEFUN (filem_show_unkn_fun,
       filem_show_unkn_cmd,
       "show file other WORD",
       SHOW_STR
       "Display file list\n"
       "Display file list in a special path\n"
       "Input path name(whole path)\n")
{
    if(filem_cli_namelencheck(vty, (char*)argv[0]))
    {
        int   iDLen = 0;
        char *bBuff = NULL;

        if(argv[0][0] != '/')
            vty_error_out(vty, "Input path name error %s", VTY_NEWLINE);
        else
        {
            iDLen = filem_show_filelist(0, FILEM_MTYPE_NON, (char *)argv[0], &bBuff);

            if(iDLen < 0)
                 vty_error_out(vty, "Show file list in path %s error, %s%s", argv[0], filem_sys_errget(iDLen), VTY_NEWLINE);
            else vty_out(vty, "%s%s", bBuff, VTY_NEWLINE);

            if(bBuff) XFREE(MTYPE_FILEM_ENTRY, bBuff);
        }
    }

    return CMD_SUCCESS;
}

DEFUN (filem_show_padi_fun,
       filem_show_padi_cmd,
       "show file version WORD",
       SHOW_STR
       "Display file information\n"
       "Display version information of sys file\n"
       "Input name\n")
{
    if(filem_cli_namelencheck(vty, (char*)argv[0]))
    {
        int   iDLen = 0;
        char *bBuff = NULL;

        iDLen = filem_md5_filepadinf((char *)argv[0], &bBuff);

        if(iDLen > 0)
             vty_out(vty, "%s%s", bBuff, VTY_NEWLINE);
        else vty_error_out(vty, "Show file md5 pad information error,%s %s", filem_sys_errget(iDLen), VTY_NEWLINE);

        if(bBuff) XFREE(MTYPE_FILEM_ENTRY, bBuff);
    }

    return CMD_SUCCESS;
}


DEFUN (filem_show_bootarea_fun,
       filem_show_bootarea_cmd,
       "show file boot",
       SHOW_STR
       "Display file information\n"
       "Display boot information\n")
{
    int   iDLen = 0;
    char  bBuff[1024] = {0};

    iDLen = filem_img_bootinfo_get(bBuff);

    if(iDLen > 0)
         vty_out(vty, "%s%s", bBuff, VTY_NEWLINE);
    else vty_error_out(vty, "Show boot information error%s", VTY_NEWLINE);


    return CMD_SUCCESS;
}

DEFUN (filem_show_knlarea_fun,
       filem_show_knlarea_cmd,
       "show file kernel {slot <1-31>}",
       SHOW_STR
       "Display file information\n"
       "Display kernel image information\n" 
       "Display a special slot\n" 
       "Input a special slot number\n")
{
    int   iDLen = 0;
    int   iSlot = 0;
    char  bBuff[1024] = {0};

    if((argc > 0) && (argv[0] != NULL))
    {
        iSlot = atoi(argv[0]);
        
        if((gfilelocslot != iSlot) && !filem_slot_ismaster())
        {
            vty_error_out(vty, "Not master cpu, can't show other slot.%s", VTY_NEWLINE);
            return CMD_SUCCESS;
        }
    }

    iDLen = filem_img_kernel_info_get(bBuff, iSlot);

    if(iDLen > 0)
         vty_out(vty, "%s%s", bBuff, VTY_NEWLINE);
    else vty_error_out(vty, "No slot match or information error%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}


DEFUN (filem_show_slotver_fun,
       filem_show_slotver_cmd,
       "show file state infor {slot <1-31>}",
       SHOW_STR
       "Display file information\n"
       "Display file state information\n"
       "Display ver or status information\n"
       "Display a special slot\n" 
       "Input a special slot number\n")
{
    int   iSlot = 0;
    int   iDLen = 0;
    char  bBuff[1024] = {0};

    if((argc > 0) && (argv[0] != NULL))
    {
        iSlot = atoi(argv[0]);
        
        if((gfilelocslot != iSlot) && !filem_slot_ismaster())
        {
            vty_error_out(vty, "Not master cpu, can't show other slot.%s", VTY_NEWLINE);
            return CMD_SUCCESS;
        }
    }

    iDLen = filem_slot_verinfo(bBuff, iSlot);

    if(iDLen > 0)
         vty_out(vty, "%s%s", bBuff, VTY_NEWLINE);
    else vty_error_out(vty, "No slot match or information error%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}


DEFUN (filem_delete_type_fun,
       filem_delete_type_cmd,
       "file delete (config|data|sys) WORD",
       "File operation\n"
       "File delete\n"
       "Delete config file\n"
       "Delete data file,log or alarm\n"
       "Delete sys version file\n"
       "Input file name\n")
{
    if(filem_cli_namelencheck(vty, (char*)argv[1]))
    {
         int   iType = 0;
         int   iRetv = 0;

         iType = filem_cli_typenametoid((char*)argv[0]);
 
         iRetv = filem_cmd_remove(iType, (char *)argv[1], NULL, NULL, 0);
 
         if(iRetv < 0)
         {
              vty_error_out(vty, "Delete file %s error, %s%s", argv[1], filem_sys_errget(iRetv), VTY_NEWLINE);
              return CMD_WARNING;
         }
         else return CMD_SUCCESS;
    }
    else return CMD_WARNING;
}

DEFUN (filem_delete_unkn_fun,
       filem_delete_unkn_cmd,
       "file delete unk WORD",
       "File operation\n"
       "File delete\n"
       "Delete unknown type file\n"
       "Input file name (inlucde whole path)\n")
{
     if(filem_cli_namelencheck(vty, (char*)argv[0]))
     {
         int   iRetv = 0;
         
         iRetv = filem_cmd_remove(FILEM_MTYPE_NON, (char *)argv[0], NULL, NULL, 0);

         if(iRetv < 0)
         {
              vty_error_out(vty, "Delete file %s error, %s%s", argv[0], filem_sys_errget(iRetv), VTY_NEWLINE);
              return CMD_WARNING;
         }
         else return CMD_SUCCESS;
    }
    else return CMD_WARNING;
}

DEFUN (filem_rename_type_fun,
       filem_rename_type_cmd,
       "file rename (config|data|sys) WORD WORD",
       "File operation\n"
       "File rename\n"
       "Rename config file\n"
       "Rename data file,log or alarm file\n"
       "Rename sys version file\n"
       "Input src file name\n" 
       "Input des file name\n")
{
    if(filem_cli_namelencheck(vty, (char*)argv[1]) &&
       filem_cli_namelencheck(vty, (char*)argv[2]))
    {
         int   iType = 0;
         int   iRetv = 0;
         
         iType = filem_cli_typenametoid((char*)argv[0]);
 
         iRetv = filem_cmd_rename(iType, (char *)argv[1], (char *)argv[2], NULL, NULL, 0);
     
         if(iRetv < 0)
         {
              vty_error_out(vty, "Rename file %s error, %s%s", (char *)argv[1], filem_sys_errget(iRetv), VTY_NEWLINE);
              return CMD_WARNING;
         }
         else return CMD_SUCCESS;
    }
    else return CMD_WARNING;
}

DEFUN (filem_rename_unkn_fun,
       filem_rename_unkn_cmd,
       "file rename unk WORD WORD",
       "File operation\n"
       "File rename\n"
       "Rename unknown type file\n"
       "Input src file name (inlucde whole path)\n" 
       "Input des file name (inlucde whole path)\n")
{
    if(filem_cli_namelencheck(vty, (char*)argv[0]) &&
       filem_cli_namelencheck(vty, (char*)argv[1]))
    {
         int   iRetv = 0;

         iRetv = filem_cmd_rename(FILEM_MTYPE_NON, (char*)argv[0], (char*)argv[1], NULL, NULL, 0);
 
         if(iRetv < 0)
         {
              vty_error_out(vty, "Rename file %s error, %s%s", argv[0], filem_sys_errget(iRetv), VTY_NEWLINE);
              return CMD_WARNING;
         }
         else return CMD_SUCCESS;
    }
    else return CMD_WARNING;
}

DEFUN (filem_copy_type_fun,
       filem_copy_type_cmd,
       "file copy (bin|config|data|sys) WORD (bin|config|data|sys) WORD",
       "File operation\n"
       "File copy\n"
       "Copy exec src file\n"
       "Copy config src file\n"
       "Copy data src file\n"
       "Copy sys version src file\n"
       "Input src file name\n"
       "Copy exec des file\n"
       "Copy config des file\n"
       "Copy data des file\n"
       "Copy version type des file\n"
       "Input des file name\n")
{
    int   iStyp = 0;
    int   iDtyp = 0;
    int   iRetv = 0;

    char  bSname[FILEM_NAME_MAXLN] = {0};
    char  bDname[FILEM_NAME_MAXLN] = {0};

    if(!filem_cli_namelencheck(vty, (char*)argv[1])) return CMD_SUCCESS;
    
    if(!filem_cli_namelencheck(vty, (char*)argv[3])) return CMD_SUCCESS;

    iStyp = filem_cli_typenametoid((char*)argv[0]);

    iDtyp = filem_cli_typenametoid((char*)argv[2]);

    if((iDtyp == FILEM_MTYPE_VER) && !filem_slot_ismaster())
    {
        vty_error_out(vty, "Only master can copy file to version path!%s", VTY_NEWLINE);
        return(CMD_WARNING);
    }
        
    iRetv = filem_cmd_inputnamecheck(iStyp , (char*)argv[1], bSname);
    
    if(iRetv != 0) 
    {
        vty_error_out(vty, "Copy file %s fail, src name error, %s%s", argv[1], filem_sys_errget(iRetv), VTY_NEWLINE);

        return CMD_WARNING;
    }

    iRetv = filem_cmd_inputnamecheck(iDtyp , (char*)argv[3], bDname);
    
    if(iRetv != 0) 
    {
        vty_error_out(vty, "Copy file %s fail, dst name error, %s%s", argv[3], filem_sys_errget(iRetv), VTY_NEWLINE);

        return CMD_WARNING;
    }

    if(strcmp(bSname, bDname) == 0)
    {
        vty_error_out(vty, "Copy file %s fail, %s%s", argv[1], filem_sys_errget(-FILEM_ERROR_SAMENAME), VTY_NEWLINE);

        return CMD_WARNING;
    }
    
    if(!filem_oper_isexist(bSname))  
    {
        vty_error_out(vty, "Copy file %s fail, %s%s", argv[1], filem_sys_errget(-FILEM_ERROR_NOEXIST), VTY_NEWLINE);

        return CMD_WARNING;
    }
    
    if(filem_oper_isexist(bDname))
    {
        if(filem_oper_isdir(bDname))
        {
            vty_error_out(vty, "Destination file is a dir! %s", VTY_NEWLINE);
    
            return CMD_WARNING;
        }
        else
        {
            switch(vty->cmd_replay)
            {
                case VTY_REPLAY_NONE:           /*first time exec the cmd, notice user */
                     vty_out(vty, "The file already exists, whether it is replaced? (Y/N)\r\n");
                     return CMD_REPLAY;
    
                case VTY_REPLAY_YES:            /*user confirmed*/
                     break;
                case VTY_REPLAY_NO:             /*user canceled*/
                     vty_out(vty, "Copy file %s cancel, %s%s", argv[1], filem_sys_errget(-FILEM_ERROR_CANCEL), VTY_NEWLINE);
                     return CMD_SUCCESS;
            }
        }
    }

    iRetv = filem_cmd_copy(iStyp, (char *)argv[1], iDtyp, (char *)argv[3], filem_cli_proccall, vty, 1);
                                                                      /*必须异步执行 拷贝时间太长,解决回调返回冲突*/
    if(iRetv < 0)
    {
         vty_error_out(vty, "Copy file %s error, %s%s", argv[1], filem_sys_errget(iRetv), VTY_NEWLINE);
 
         return CMD_WARNING;
    }
    else return CMD_WAIT;
}

DEFUN (filem_dload_ver4_fun,
       filem_dload_ver4_cmd,
       "file download (sys|bin|config) ipv4 A.B.C.D WORD [WORD]",
       "File operation\n"
       "File download\n"
       "Download a sys version file\n"
       "Download a app bin file\n"
       "Download a config file\n"
       "Download file use ipv4\n"
       "Input server ipv4 address\n"
       "Input remote file name \n" 
       "Input local file name, if no input, use the same remote\n")
{
    int   iRetv = 0;
    int   iType = 0;

    char  bLName[FILEM_NAME_MAXLN];

    memset(bLName, 0, FILEM_NAME_MAXLN);

    if(!filem_cli_namelencheck(vty, (char*)argv[2])) 
    {
        filem_download_stat_set(-FILEM_ERROR_TOOLONG);  
        return(CMD_WARNING);
    }

    if(!filem_cmd_inputipv4check((char*)argv[1]))
    {
        filem_download_stat_set(-FILEM_ERROR_IPADDRERR);  

        vty_error_out(vty, "Input IP address error! %s %s", argv[1], VTY_NEWLINE);
        return(CMD_WARNING);
    }


    if((argc == 4) && (argv[3] != NULL))
    {
        if(!filem_cli_namelencheck(vty, (char*)argv[3]))
        {
            filem_download_stat_set(-FILEM_ERROR_TOOLONG);  
            return(CMD_WARNING);
        }
    }

    iType = filem_cli_typenametoid((char*)argv[0]);
    
    if((iType == FILEM_MTYPE_VER) && !filem_slot_ismaster())
    {
        filem_download_stat_set(-FILEM_ERROR_NOMASTER);  
        vty_error_out(vty, "Only master can download sys version file!%s", VTY_NEWLINE);
        return(CMD_WARNING);
    }

    if((argc == 4) && (argv[3] != NULL))
         strncpy(bLName, argv[3], FILEM_NAME_MAXLN - 1);
    else filem_oper_cutname((char*)argv[2], bLName);


    iRetv = filem_cli_downloadcomfirm(vty, bLName, iType);

    if(iRetv >= 0) return(iRetv);
    else
    {
        iRetv = filem_cmd_tftpdnload(iType, (char*)argv[1], (char*)argv[2], bLName, filem_cli_proccall, vty, 0);
                                                                          /*同步调用,由主线程异步处理*/
        if(iRetv < 0)
        {
             filem_download_stat_set(iRetv);            /*此处设置错误状态, 可能导致其他操作通道的执行结果, 被修改*/
             vty_error_out(vty, "Download file %s error, %s%s", argv[2], filem_sys_errget(iRetv), VTY_NEWLINE);
             return(CMD_WARNING);
        }
        else return(CMD_WAIT);
    }
}


DEFUN (filem_dload_unk4_fun,
       filem_dload_unk4_cmd,
       "file download other ipv4 A.B.C.D WORD WORD",
       "File operation\n"
       "File download\n"
       "Download a file to speical path\n"
       "Download file use ipv4\n"
       "Input server ipv4 address\n"
       "Input remote file name \n" 
       "Input local file name, must with path\n")
{
    if(filem_cli_namelencheck(vty, (char*)argv[1]) &&
       filem_cli_namelencheck(vty, (char*)argv[2])) 
    {
         int   iRetv = 0;
         int   iType = FILEM_MTYPE_NON;

         if(!filem_cmd_inputipv4check((char*)argv[0]))
         {
             filem_download_stat_set(-FILEM_ERROR_IPADDRERR);  
             vty_error_out(vty, "Input IP address error! %s %s", argv[0], VTY_NEWLINE);
             return(CMD_WARNING);
         }

         iType = filem_name_totype((char*)argv[2]);

         if((iType == FILEM_MTYPE_VER) && !filem_slot_ismaster())
         {
             filem_download_stat_set(-FILEM_ERROR_NOMASTER);  
             vty_error_out(vty, "Only master can download sys version file!%s", VTY_NEWLINE);
             return(CMD_WARNING);
         }
         
         iRetv = filem_cli_downloadcomfirm(vty, (char*)argv[2], FILEM_MTYPE_NON);
 
         if(iRetv >= 0) return(iRetv);
         else
         {
             iRetv = filem_cmd_tftpdnload(FILEM_MTYPE_NON, (char *)argv[0], (char*)argv[1], (char*)argv[2], filem_cli_proccall, vty, 0);
                                                                           /*同步调用,由主线程异步处理*/
             if(iRetv < 0)
             {
                  filem_download_stat_set(iRetv);            /*此处设置错误状态, 可能导致其他操作通道的执行结果, 被修改*/

                  vty_error_out(vty, "Download file %s error, %s%s", argv[1], filem_sys_errget(iRetv), VTY_NEWLINE);
                  return(CMD_WARNING);
             }
             else return(CMD_WAIT);
         }
    }
    else 
    {
        filem_download_stat_set(-FILEM_ERROR_TOOLONG);  
        return CMD_WARNING;
    }
}


DEFUN (filem_dload_ver6_fun,
       filem_dload_ver6_cmd,
       "file download (sys|bin|config) ipv6 X:X::X:X WORD [WORD]",
       "File operation\n"
       "File download\n"
       "Download a sys version file\n"
       "Download a app bin file\n"
       "Download a config file\n"
       "Download file use ipv6\n"
       "Input server ipv6 address\n"
       "Input remote file name \n" 
       "Input local file name, if no input, use the same remote\n")
{
    int   iRetv = 0;
    int   iType = 0;

    char  bLName[FILEM_NAME_MAXLN];

    memset(bLName, 0, FILEM_NAME_MAXLN);

    if(!filem_cli_namelencheck(vty, (char*)argv[2]))
    {
        filem_download_stat_set(-FILEM_ERROR_TOOLONG);  
        return(CMD_WARNING);
    }
    
    if((argc == 4) && (argv[3] != NULL))
    {
        if(!filem_cli_namelencheck(vty, (char*)argv[3]))
        {
            filem_download_stat_set(-FILEM_ERROR_TOOLONG);  
            return(CMD_WARNING);
        }
    }

    iType = filem_cli_typenametoid((char*)argv[0]);
    
    if((iType == FILEM_MTYPE_VER) && !filem_slot_ismaster())
    {
        filem_download_stat_set(-FILEM_ERROR_NOMASTER);  
        vty_error_out(vty, "Only master can download sys version file!%s", VTY_NEWLINE);
        return(CMD_WARNING);
    }

    if((argc == 4) && (argv[3] != NULL))
         strncpy(bLName, argv[3], FILEM_NAME_MAXLN - 1);
    else filem_oper_cutname((char*)argv[2], bLName);

    iRetv = filem_cli_downloadcomfirm(vty, bLName, iType);

    if(iRetv >= 0) return(iRetv);
    else
    {
        iRetv = filem_cmd_tftpdnload(iType, (char *)argv[1], (char*)argv[2], bLName, filem_cli_proccall, vty, 0);
                                                                          /*同步调用,由主线程异步处理*/
        if(iRetv < 0)
        {
             filem_download_stat_set(iRetv);            /*此处设置错误状态, 可能导致其他操作通道的执行结果, 被修改*/
             vty_error_out(vty, "Download file %s error, %s%s", argv[2], filem_sys_errget(iRetv), VTY_NEWLINE);
             return(CMD_WARNING);
        }
        else return(CMD_WAIT);
    }
}

DEFUN (filem_dload_unk6_fun,
       filem_dload_unk6_cmd,
       "file download other ipv6 X:X::X:X WORD WORD",
       "File operation\n"
       "File download\n"
       "Download a file to speical path\n"
       "Download file use ipv6\n"
       "Input server ipv6 address\n"
       "Input remote file name \n" 
       "Input local file name, must with path\n")
{
    if(filem_cli_namelencheck(vty, (char*)argv[1]) &&
       filem_cli_namelencheck(vty, (char*)argv[2])) 
    {
         int   iRetv = 0;
         int   iType = FILEM_MTYPE_NON;
         
         iType = filem_name_totype((char*)argv[2]);

         if((iType == FILEM_MTYPE_VER) && !filem_slot_ismaster())
         {
             filem_download_stat_set(-FILEM_ERROR_NOMASTER);  
             vty_error_out(vty, "Only master can download sys version file!%s", VTY_NEWLINE);
             return(CMD_WARNING);
         }

         iRetv = filem_cli_downloadcomfirm(vty, (char*)argv[2], FILEM_MTYPE_NON);
 
         if(iRetv >= 0) return(iRetv);
         else
         {
             iRetv = filem_cmd_tftpdnload(FILEM_MTYPE_NON, (char *)argv[0], (char*)argv[1], (char*)argv[2], filem_cli_proccall, vty, 0);
                                                                           /*必须同步调用,由主线程异步处理*/
             if(iRetv < 0)
             {
                  filem_download_stat_set(iRetv);            /*此处设置错误状态, 可能导致其他操作通道的执行结果被修改*/
                  vty_error_out(vty, "Download file %s error, %s%s", argv[1], filem_sys_errget(iRetv), VTY_NEWLINE);
                  return(CMD_WARNING);
             }
             else return(CMD_WAIT);
         }
    }
    else  
    {
        filem_download_stat_set(-FILEM_ERROR_TOOLONG);  
        return CMD_WARNING;
    }
}

DEFUN (filem_uload_ver4_fun,
       filem_uload_ver4_cmd,
       "file upload (bin|config|data|sys) WORD ipv4 A.B.C.D [WORD]",
       "File operation\n"
       "File upload\n"
       "Upload exec file\n"
       "Upload config file\n"
       "Upload data type file\n"
       "Upload sys version file\n"
       "Input local file name \n" 
       "Upload file use ipv4\n"
       "Input server ipv4 address\n"
       "Input remote file name, if no input, use the same local\n")
{
    int   iStyp = 0;
    int   iRetv = 0;

    char  bRName[FILEM_NAME_MAXLN];
    
    memset(bRName, 0, FILEM_NAME_MAXLN);

    if(!filem_cli_namelencheck(vty, (char*)argv[1]))  
    {
        filem_upload_stat_set(-FILEM_ERROR_TOOLONG);  
        return CMD_WARNING;
    }
 
    if((argc == 4) && (argv[3] != NULL))
    {
        if(!filem_cli_namelencheck(vty, (char*)argv[3])) 
        {
            filem_upload_stat_set(-FILEM_ERROR_TOOLONG);  
            return CMD_WARNING;
        }
    }

    if(!filem_cmd_inputipv4check((char*)argv[2]))
    {
        filem_upload_stat_set(-FILEM_ERROR_IPADDRERR);  
        vty_error_out(vty, "Input IP address error! %s %s", argv[2], VTY_NEWLINE);
        return(CMD_WARNING);
    }

    if((argc == 4) && (argv[3] != NULL))
         strncpy(bRName, argv[3], FILEM_NAME_MAXLN -1);
    else strncpy(bRName, argv[1], FILEM_NAME_MAXLN -1);

    iStyp = filem_cli_typenametoid((char*)argv[0]);

    iRetv = filem_cmd_tftpupload(iStyp, (char *)argv[2], bRName, (char*)argv[1], filem_cli_proccall, vty, 0);
                                                                          /*同步调用,由主线程异步处理*/
    if(iRetv >= 0)
    {
        return(CMD_WAIT);
    }
    else
    {
        filem_upload_stat_set(iRetv);  /*此处设置错误状态, 可能导致其他操作通道的执行结果被修改*/
        
        vty_error_out(vty, "Upload file %s error, %s%s", argv[1], filem_sys_errget(iRetv), VTY_NEWLINE);

        return(CMD_WARNING);
    }
}

DEFUN (filem_uload_unk4_fun,
       filem_uload_unk4_cmd,
       "file upload other WORD ipv4 A.B.C.D [WORD]",
       "File operation\n"
       "File upload\n"
       "Upload other type file\n"
       "Input local file path and name\n" 
       "Upload file use ipv4\n"
       "Input server ipv4 address\n"
       "Input remote file name, if no input, use the same local name\n") 
{
    int   iRetv = 0;

    char  bRName[FILEM_NAME_MAXLN];

    if(!filem_cli_namelencheck(vty, (char*)argv[0]))  
    {
        filem_upload_stat_set(-FILEM_ERROR_TOOLONG);  
        return CMD_WARNING;
    }
    
    if((argc == 3) && (argv[2] != NULL))
    {
        if(!filem_cli_namelencheck(vty, (char*)argv[2]))  
        {
            filem_upload_stat_set(-FILEM_ERROR_TOOLONG);  
            return CMD_WARNING;
        }
    }

    if(!filem_cmd_inputipv4check((char*)argv[1]))
    {
        filem_upload_stat_set(-FILEM_ERROR_IPADDRERR);  
        vty_error_out(vty, "Input IP address error! %s %s", argv[1], VTY_NEWLINE);
        return(CMD_WARNING);
    }

    memset(bRName, 0, FILEM_NAME_MAXLN);

    if((argc == 3) && (argv[2] != NULL))
         strncpy(bRName, argv[2], FILEM_NAME_MAXLN -1);
    else filem_oper_cutname((char*)argv[0], bRName);
    
    iRetv = filem_cmd_tftpupload(FILEM_MTYPE_NON, (char *)argv[1], bRName, (char*)argv[0], filem_cli_proccall, vty, 0);
                                                                          /*同步调用,由主线程异步处理*/
    if(iRetv >= 0)
    {
        return(CMD_WAIT);
    }
    else
    {
        filem_upload_stat_set(iRetv);            /*此处设置错误状态, 可能导致其他操作通道的执行结果, 被修改*/

        vty_error_out(vty, "Upload file %s error, %s%s", argv[1], filem_sys_errget(iRetv), VTY_NEWLINE);
        return(CMD_WARNING);
    }
}

DEFUN (filem_uload_ver6_fun,
       filem_uload_ver6_cmd,
       "file upload (bin|config|data|sys) WORD ipv6 X:X::X:X [WORD]",
       "File operation\n"
       "File upload\n"
       "Upload exec file\n"
       "Upload config file\n"
       "Upload data file\n"
       "Upload sys version file\n"
       "Input local file name \n" 
       "Upload file use ipv6\n"
       "Input server ipv6 address\n"
       "Input remote file name, if no input, use the same local\n")
{
    int   iStyp = 0;
    int   iRetv = 0;

    char  bRName[FILEM_NAME_MAXLN];
    
    memset(bRName, 0, FILEM_NAME_MAXLN);
 
    if(!filem_cli_namelencheck(vty, (char*)argv[1])) 
    {
        filem_upload_stat_set(-FILEM_ERROR_TOOLONG);  
        return CMD_WARNING;
    }
    
    if((argc == 4) && (argv[3] != NULL))
    {
        if(!filem_cli_namelencheck(vty, (char*)argv[3]))  
        {
            filem_upload_stat_set(-FILEM_ERROR_TOOLONG);  
            return CMD_WARNING;
        }
    }

    memset(bRName, 0, FILEM_NAME_MAXLN);

    iStyp = filem_cli_typenametoid((char*)argv[0]);
    
    if((argc == 4) && (argv[3] != NULL))
         strncpy(bRName, argv[3], FILEM_NAME_MAXLN -1);
    else strncpy(bRName, argv[1], FILEM_NAME_MAXLN -1);

    iRetv = filem_cmd_tftpupload(iStyp, (char *)argv[2], bRName, (char*)argv[1], filem_cli_proccall, vty, 0);
                                                                           /*必须同步调用,由主线程异步处理*/
    if(iRetv >= 0)
    {
        return(CMD_WAIT);
    }
    else
    {
        filem_upload_stat_set(iRetv);

        vty_error_out(vty, "Upload file %s error, %s%s", argv[1], filem_sys_errget(iRetv), VTY_NEWLINE);
        return(CMD_WARNING);
    }
}


DEFUN (filem_uload_unk6_fun,
       filem_uload_unk6_cmd,
       "file upload other WORD ipv6 X:X::X:X [WORD]",
       "File operation\n"
       "File upload\n"
       "Upload other type file\n"
       "Input local file path and name\n"
       "Upload file use ipv6\n"
       "Input server ipv6 address\n"
       "Input remote file name, if no input, use the same local name\n") 
{

    int   iRetv = 0;

    char  bRName[FILEM_NAME_MAXLN];

    if(!filem_cli_namelencheck(vty, (char*)argv[0]))  
    {
        filem_upload_stat_set(-FILEM_ERROR_TOOLONG);  
        return CMD_WARNING;
    }
    
    if((argc == 3) && (argv[2] != NULL))
    {
        if(!filem_cli_namelencheck(vty, (char*)argv[2]))  
        {
            filem_upload_stat_set(-FILEM_ERROR_TOOLONG);  
            return CMD_WARNING;
        }
    }

    memset(bRName, 0, FILEM_NAME_MAXLN);

    if((argc == 3) && (argv[2] != NULL))
         strncpy(bRName, argv[2], FILEM_NAME_MAXLN -1);
    else filem_oper_cutname((char*)argv[0], bRName);
    
    iRetv = filem_cmd_tftpupload(FILEM_MTYPE_NON, (char *)argv[1], bRName, (char*)argv[0], filem_cli_proccall, vty, 0);
                                                                          /*必须同步调用,由主线程异步处理*/
    if(iRetv >= 0)
    {
        return(CMD_WAIT);
    }
    else
    {
        filem_upload_stat_set(iRetv);

        vty_error_out(vty, "Upload file %s error, %s%s", argv[1], filem_sys_errget(iRetv), VTY_NEWLINE);
        return(CMD_WARNING);
    }
}
/*------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------*/

DEFUN (filem_ftp_dload_ver4_fun,
       filem_ftp_dload_ver4_cmd,
       "ftp download (sys|bin|config) ipv4 A.B.C.D user WORD password WORD WORD [WORD]",
       "File ftp operation\n"
       "File ftp download\n"
       "Download a sys version file\n"
       "Download a app bin file\n"
       "Download a config file\n"
       "Download file use ipv4\n"
       "Input server ipv4 address\n"
       "Login user name\n"
       "Input user name\n"
       "Login user password\n"
       "Input user password\n"
       "Input remote file name \n" 
       "Input local file name, if no input, use the same remote\n")
{
    int   iRetv = 0;
    int   iType = 0;

    char  bLName[FILEM_NAME_MAXLN];

    memset(bLName, 0, FILEM_NAME_MAXLN);
    
    if(!filem_cmd_inputipv4check((char*)argv[1]))
    {
        filem_download_stat_set(-FILEM_ERROR_IPADDRERR);  

        vty_error_out(vty, "Input IP address error! %s %s", argv[1], VTY_NEWLINE);
        return(CMD_WARNING);
    }

    if(strlen((char*)argv[2]) >= FILEM_USER_STRLN)  
    {
        filem_download_stat_set(-FILEM_ERROR_AUTHERROR);  
        vty_error_out(vty, "User name too long %s", VTY_NEWLINE);
        return(CMD_WARNING);
    }

    if(strlen((char*)argv[3]) >= FILEM_USER_STRLN)
    {
        filem_download_stat_set(-FILEM_ERROR_AUTHERROR);  
        vty_error_out(vty, "User password too long %s", VTY_NEWLINE);
        return(CMD_WARNING);
    }
    

    if(!filem_cli_namelencheck(vty, (char*)argv[4])) 
    {
        filem_download_stat_set(-FILEM_ERROR_TOOLONG);  
        return(CMD_WARNING);
    }

    if((argc == 6) && (argv[5] != NULL))
    {
        if(!filem_cli_namelencheck(vty, (char*)argv[5]))
        {
            filem_download_stat_set(-FILEM_ERROR_TOOLONG);  
            return(CMD_WARNING);
        }
    }

    iType = filem_cli_typenametoid((char*)argv[0]);
    
    if((iType == FILEM_MTYPE_VER) && !filem_slot_ismaster())
    {
        filem_download_stat_set(-FILEM_ERROR_NOMASTER);  
        vty_error_out(vty, "Only master can download sys version file!%s", VTY_NEWLINE);
        return(CMD_WARNING);
    }

    if((argc == 6) && (argv[5] != NULL))
         strncpy(bLName, argv[5], FILEM_NAME_MAXLN - 1);
    else filem_oper_cutname((char*)argv[4], bLName);


    iRetv = filem_cli_downloadcomfirm(vty, bLName, iType);

    if(iRetv >= 0) return(iRetv);
    else
    {
        iRetv = filem_cmd_ftpdnload(iType, (char*)argv[1], (char*)argv[2], (char*)argv[3], (char*)argv[4], bLName, filem_cli_proccall, vty, 0);
                                                                          /*同步调用,由主线程异步处理*/
        if(iRetv < 0)
        {
             filem_download_stat_set(iRetv);            /*此处设置错误状态, 可能导致其他操作通道的执行结果, 被修改*/
             vty_error_out(vty, "Download file %s error, %s%s", argv[4], filem_sys_errget(iRetv), VTY_NEWLINE);
             return(CMD_WARNING);
        }
        else return(CMD_WAIT);
    }
}


DEFUN (filem_ftp_dload_unk4_fun,
       filem_ftp_dload_unk4_cmd,
       "ftp download other ipv4 A.B.C.D user WORD password WORD WORD WORD",
       "File ftp operation\n"
       "File download\n"
       "Download a file to speical path\n"
       "Download file use ipv4\n"
       "Input server ipv4 address\n"
       "Login user name\n"
       "Input user name\n"
       "Login user password\n"
       "Input user password\n"
       "Input remote file name \n" 
       "Input local file name, must with path\n")
{
    if(filem_cli_namelencheck(vty, (char*)argv[3]) &&
       filem_cli_namelencheck(vty, (char*)argv[4])) 
    {
         int   iRetv = 0;
         int   iType = FILEM_MTYPE_NON;

         if(!filem_cmd_inputipv4check((char*)argv[0]))
         {
             filem_download_stat_set(-FILEM_ERROR_IPADDRERR);  
             vty_error_out(vty, "Input IP address error! %s %s", argv[0], VTY_NEWLINE);
             return(CMD_WARNING);
         }

         if(strlen((char*)argv[1]) >= FILEM_USER_STRLN)  
         {
             filem_download_stat_set(-FILEM_ERROR_AUTHERROR);  
             vty_error_out(vty, "User name too long %s", VTY_NEWLINE);
             return(CMD_WARNING);
         }
 
         if(strlen((char*)argv[2]) >= FILEM_USER_STRLN)
         {
             filem_download_stat_set(-FILEM_ERROR_AUTHERROR);  
             vty_error_out(vty, "User password too long %s", VTY_NEWLINE);
             return(CMD_WARNING);
         }

         iType = filem_name_totype((char*)argv[4]);

         if((iType == FILEM_MTYPE_VER) && !filem_slot_ismaster())
         {
             filem_download_stat_set(-FILEM_ERROR_NOMASTER);  
             vty_error_out(vty, "Only master can download sys version file!%s", VTY_NEWLINE);
             return(CMD_WARNING);
         }
         
         iRetv = filem_cli_downloadcomfirm(vty, (char*)argv[4], FILEM_MTYPE_NON);
 
         if(iRetv >= 0) return(iRetv);
         else
         {
             iRetv = filem_cmd_ftpdnload(FILEM_MTYPE_NON, (char *)argv[0], (char*)argv[1], (char*)argv[2], (char*)argv[3], (char*)argv[4], filem_cli_proccall, vty, 0);
                                                                           /*同步调用,由主线程异步处理*/
             if(iRetv < 0)
             {
                  filem_download_stat_set(iRetv);            /*此处设置错误状态, 可能导致其他操作通道的执行结果, 被修改*/

                  vty_error_out(vty, "Download file %s error, %s%s", argv[3], filem_sys_errget(iRetv), VTY_NEWLINE);
                  return(CMD_WARNING);
             }
             else return(CMD_WAIT);
         }
    }
    else 
    {
        filem_download_stat_set(-FILEM_ERROR_TOOLONG);  
        return CMD_WARNING;
    }
}


DEFUN (filem_ftp_dload_ver6_fun,
       filem_ftp_dload_ver6_cmd,
       "ftp download (sys|bin|config) ipv6 X:X::X:X user WORD password WORD WORD [WORD]",
       "File ftp operation\n"
       "File download\n"
       "Download a sys version file\n"
       "Download a app bin file\n"
       "Download a config file\n"
       "Download file use ipv6\n"
       "Input server ipv6 address\n"
       "Login user name\n"
       "Input user name\n"
       "Login user password\n"
       "Input user password\n"
       "Input remote file name \n" 
       "Input local file name, if no input, use the same remote\n")
{
    int   iRetv = 0;
    int   iType = 0;

    char  bLName[FILEM_NAME_MAXLN];

    memset(bLName, 0, FILEM_NAME_MAXLN);

    if(strlen((char*)argv[2]) >= FILEM_USER_STRLN)  
    {
        filem_download_stat_set(-FILEM_ERROR_AUTHERROR);  
        vty_error_out(vty, "User name too long %s", VTY_NEWLINE);
        return(CMD_WARNING);
    }
    
    if(strlen((char*)argv[3]) >= FILEM_USER_STRLN)
    {
        filem_download_stat_set(-FILEM_ERROR_AUTHERROR);  
        vty_error_out(vty, "User password too long %s", VTY_NEWLINE);
        return(CMD_WARNING);
    }

    if(!filem_cli_namelencheck(vty, (char*)argv[4]))
    {
        filem_download_stat_set(-FILEM_ERROR_TOOLONG);  
        return(CMD_WARNING);
    }
    
    if((argc == 6) && (argv[5] != NULL))
    {
        if(!filem_cli_namelencheck(vty, (char*)argv[5]))
        {
            filem_download_stat_set(-FILEM_ERROR_TOOLONG);  
            return(CMD_WARNING);
        }
    }

    iType = filem_cli_typenametoid((char*)argv[0]);
    
    if((iType == FILEM_MTYPE_VER) && !filem_slot_ismaster())
    {
        filem_download_stat_set(-FILEM_ERROR_NOMASTER);  
        vty_error_out(vty, "Only master can download sys version file!%s", VTY_NEWLINE);
        return(CMD_WARNING);
    }

    if((argc == 6) && (argv[5] != NULL))
         strncpy(bLName, argv[5], FILEM_NAME_MAXLN - 1);
    else filem_oper_cutname((char*)argv[4], bLName);

    iRetv = filem_cli_downloadcomfirm(vty, bLName, iType);

    if(iRetv >= 0) return(iRetv);
    else
    {
        iRetv = filem_cmd_ftpdnload(iType, (char *)argv[1], (char*)argv[2],  (char*)argv[3], (char*)argv[4], bLName, filem_cli_proccall, vty, 0);
                                                                          /*同步调用,由主线程异步处理*/
        if(iRetv < 0)
        {
             filem_download_stat_set(iRetv);            /*此处设置错误状态, 可能导致其他操作通道的执行结果, 被修改*/
             vty_error_out(vty, "Download file %s error, %s%s", argv[4], filem_sys_errget(iRetv), VTY_NEWLINE);
             return(CMD_WARNING);
        }
        else return(CMD_WAIT);
    }
}

DEFUN (filem_ftp_dload_unk6_fun,
       filem_ftp_dload_unk6_cmd,
       "ftp download other ipv6 X:X::X:X user WORD password WORD WORD WORD",
       "File ftp operation\n"
       "File download\n"
       "Download a file to speical path\n"
       "Download file use ipv6\n"
       "Input server ipv6 address\n"
       "Login user name\n"
       "Input user name\n"
       "Login user password\n"
       "Input user password\n"
       "Input remote file name \n" 
       "Input local file name, must with path\n")
{
    if(filem_cli_namelencheck(vty, (char*)argv[3]) &&
       filem_cli_namelencheck(vty, (char*)argv[4])) 
    {
         int   iRetv = 0;
         int   iType = FILEM_MTYPE_NON;

         if(strlen((char*)argv[1]) >= FILEM_USER_STRLN)  
         {
             filem_download_stat_set(-FILEM_ERROR_AUTHERROR);  
             vty_error_out(vty, "User name too long %s", VTY_NEWLINE);
             return(CMD_WARNING);
         }
         
         if(strlen((char*)argv[2]) >= FILEM_USER_STRLN)
         {
             filem_download_stat_set(-FILEM_ERROR_AUTHERROR);  
             vty_error_out(vty, "User password too long %s", VTY_NEWLINE);
             return(CMD_WARNING);
         }
         
         iType = filem_name_totype((char*)argv[4]);

         if((iType == FILEM_MTYPE_VER) && !filem_slot_ismaster())
         {
             filem_download_stat_set(-FILEM_ERROR_NOMASTER);  
             vty_error_out(vty, "Only master can download sys version file!%s", VTY_NEWLINE);
             return(CMD_WARNING);
         }

         iRetv = filem_cli_downloadcomfirm(vty, (char*)argv[4], FILEM_MTYPE_NON);
 
         if(iRetv >= 0) return(iRetv);
         else
         {
             iRetv = filem_cmd_ftpdnload(FILEM_MTYPE_NON, (char *)argv[0], (char*)argv[1], (char*)argv[2], (char*)argv[3], (char*)argv[4], filem_cli_proccall, vty, 0);
                                                                           /*必须同步调用,由主线程异步处理*/
             if(iRetv < 0)
             {
                  filem_download_stat_set(iRetv);            /*此处设置错误状态, 可能导致其他操作通道的执行结果被修改*/
                  vty_error_out(vty, "Download file %s error, %s%s", argv[3], filem_sys_errget(iRetv), VTY_NEWLINE);
                  return(CMD_WARNING);
             }
             else return(CMD_WAIT);
         }
    }
    else  
    {
        filem_download_stat_set(-FILEM_ERROR_TOOLONG);  
        return CMD_WARNING;
    }
}

DEFUN (filem_ftp_uload_ver4_fun,
       filem_ftp_uload_ver4_cmd,
       "ftp upload (bin|config|data|sys) WORD ipv4 A.B.C.D user WORD password WORD [WORD]",
       "File ftp operation\n"
       "File upload\n"
       "Upload exec file\n"
       "Upload config file\n"
       "Upload data type file\n"
       "Upload sys version file\n"
       "Input local file name \n" 
       "Upload file use ipv4\n"
       "Input server ipv4 address\n"
       "Login user name\n"
       "Input user name\n"
       "Login user password\n"
       "Input user password\n"
       "Input remote file name, if no input, use the same local\n")
{
    int   iStyp = 0;
    int   iRetv = 0;

    char  bRName[FILEM_NAME_MAXLN];
    
    memset(bRName, 0, FILEM_NAME_MAXLN);

    if(!filem_cli_namelencheck(vty, (char*)argv[1]))  
    {
        filem_upload_stat_set(-FILEM_ERROR_TOOLONG);  
        return CMD_WARNING;
    }

    if(strlen((char*)argv[3]) >= FILEM_USER_STRLN)  
    {
        filem_download_stat_set(-FILEM_ERROR_AUTHERROR);  
        vty_error_out(vty, "User name too long %s", VTY_NEWLINE);
        return(CMD_WARNING);
    }
    
    if(strlen((char*)argv[4]) >= FILEM_USER_STRLN)
    {
        filem_download_stat_set(-FILEM_ERROR_AUTHERROR);  
        vty_error_out(vty, "User password too long %s", VTY_NEWLINE);
        return(CMD_WARNING);
    }
    
    if((argc == 6) && (argv[5] != NULL))
    {
        if(!filem_cli_namelencheck(vty, (char*)argv[5])) 
        {
            filem_upload_stat_set(-FILEM_ERROR_TOOLONG);  
            return CMD_WARNING;
        }
    }

    if(!filem_cmd_inputipv4check((char*)argv[2]))
    {
        filem_upload_stat_set(-FILEM_ERROR_IPADDRERR);  
        vty_error_out(vty, "Input IP address error! %s %s", argv[2], VTY_NEWLINE);
        return(CMD_WARNING);
    }

    if((argc == 6) && (argv[5] != NULL))
         strncpy(bRName, argv[5], FILEM_NAME_MAXLN -1);
    else strncpy(bRName, argv[1], FILEM_NAME_MAXLN -1);

    iStyp = filem_cli_typenametoid((char*)argv[0]);

    iRetv = filem_cmd_ftpupload(iStyp, (char*)argv[2], (char*)argv[3],(char*)argv[4], bRName, (char*)argv[1], filem_cli_proccall, vty, 0);
                                                                          /*同步调用,由主线程异步处理*/
    if(iRetv >= 0)
    {
        return(CMD_WAIT);
    }
    else
    {
        filem_upload_stat_set(iRetv);  /*此处设置错误状态, 可能导致其他操作通道的执行结果被修改*/
        
        vty_error_out(vty, "Upload file %s error, %s%s", argv[1], filem_sys_errget(iRetv), VTY_NEWLINE);

        return(CMD_WARNING);
    }
}

DEFUN (filem_ftp_uload_unk4_fun,
       filem_ftp_uload_unk4_cmd,
       "ftp upload other WORD ipv4 A.B.C.D user WORD password WORD [WORD]",
       "File ftp operation\n"
       "File upload\n"
       "Upload other type file\n"
       "Input local file path and name\n" 
       "Upload file use ipv4\n"
       "Input server ipv4 address\n"
       "Login user name\n"
       "Input user name\n"
       "Login user password\n"
       "Input user password\n"
       "Input remote file name, if no input, use the same local name\n") 
{
    int   iRetv = 0;

    char  bRName[FILEM_NAME_MAXLN];

    if(!filem_cli_namelencheck(vty, (char*)argv[0]))  
    {
        filem_upload_stat_set(-FILEM_ERROR_TOOLONG);  
        return CMD_WARNING;
    }
    
    if(strlen((char*)argv[2]) >= FILEM_USER_STRLN)  
    {
        filem_download_stat_set(-FILEM_ERROR_AUTHERROR);  
        vty_error_out(vty, "User name too long %s", VTY_NEWLINE);
        return(CMD_WARNING);
    }
    
    if(strlen((char*)argv[3]) >= FILEM_USER_STRLN)
    {
        filem_download_stat_set(-FILEM_ERROR_AUTHERROR);  
        vty_error_out(vty, "User password too long %s", VTY_NEWLINE);
        return(CMD_WARNING);
    }    
    
    if((argc == 5) && (argv[4] != NULL))
    {
        if(!filem_cli_namelencheck(vty, (char*)argv[4]))  
        {
            filem_upload_stat_set(-FILEM_ERROR_TOOLONG);  
            return CMD_WARNING;
        }
    }

    if(!filem_cmd_inputipv4check((char*)argv[1]))
    {
        filem_upload_stat_set(-FILEM_ERROR_IPADDRERR);  
        vty_error_out(vty, "Input IP address error! %s %s", argv[1], VTY_NEWLINE);
        return(CMD_WARNING);
    }

    memset(bRName, 0, FILEM_NAME_MAXLN);

    if((argc == 5) && (argv[4] != NULL))
         strncpy(bRName, argv[4], FILEM_NAME_MAXLN -1);
    else filem_oper_cutname((char*)argv[0], bRName);
    
    iRetv = filem_cmd_ftpupload(FILEM_MTYPE_NON, (char*)argv[1], (char*)argv[2], (char*)argv[3], bRName, (char*)argv[0], filem_cli_proccall, vty, 0);
                                                                          /*同步调用,由主线程异步处理*/
    if(iRetv >= 0)
    {
        return(CMD_WAIT);
    }
    else
    {
        filem_upload_stat_set(iRetv);            /*此处设置错误状态, 可能导致其他操作通道的执行结果, 被修改*/

        vty_error_out(vty, "Upload file %s error, %s%s", argv[0], filem_sys_errget(iRetv), VTY_NEWLINE);
        return(CMD_WARNING);
    }
}


DEFUN (filem_ftp_uload_ver6_fun,
       filem_ftp_uload_ver6_cmd,
       "ftp upload (bin|config|data|sys) WORD ipv6 X:X::X:X user WORD password WORD [WORD]",
       "File ftp operation\n"
       "File upload\n"
       "Upload exec file\n"
       "Upload config file\n"
       "Upload data file\n"
       "Upload sys version file\n"
       "Input local file name \n" 
       "Upload file use ipv6\n"
       "Input server ipv6 address\n"
       "Login user name\n"
       "Input user name\n"
       "Login user password\n"
       "Input user password\n"
       "Input remote file name, if no input, use the same local\n")
{
    int   iStyp = 0;
    int   iRetv = 0;

    char  bRName[FILEM_NAME_MAXLN];
    
    memset(bRName, 0, FILEM_NAME_MAXLN);
 
    if(!filem_cli_namelencheck(vty, (char*)argv[1])) 
    {
        filem_upload_stat_set(-FILEM_ERROR_TOOLONG);  
        return CMD_WARNING;
    }

    if(strlen((char*)argv[3]) >= FILEM_USER_STRLN)  
    {
        filem_download_stat_set(-FILEM_ERROR_AUTHERROR);  
        vty_error_out(vty, "User name too long %s", VTY_NEWLINE);
        return(CMD_WARNING);
    }
    
    if(strlen((char*)argv[4]) >= FILEM_USER_STRLN)
    {
        filem_download_stat_set(-FILEM_ERROR_AUTHERROR);  
        vty_error_out(vty, "User password too long %s", VTY_NEWLINE);
        return(CMD_WARNING);
    }    
    
    if((argc == 6) && (argv[5] != NULL))
    {
        if(!filem_cli_namelencheck(vty, (char*)argv[5]))  
        {
            filem_upload_stat_set(-FILEM_ERROR_TOOLONG);  
            return CMD_WARNING;
        }
    }

    memset(bRName, 0, FILEM_NAME_MAXLN);

    iStyp = filem_cli_typenametoid((char*)argv[0]);
    
    if((argc == 6) && (argv[5] != NULL))
         strncpy(bRName, argv[5], FILEM_NAME_MAXLN -1);
    else strncpy(bRName, argv[1], FILEM_NAME_MAXLN -1);

    iRetv = filem_cmd_ftpupload(iStyp, (char*)argv[2], (char*)argv[3], (char *)argv[4], bRName, (char*)argv[1], filem_cli_proccall, vty, 0);
                                                                           /*必须同步调用,由主线程异步处理*/
    if(iRetv >= 0)
    {
        return(CMD_WAIT);
    }
    else
    {
        filem_upload_stat_set(iRetv);

        vty_error_out(vty, "Upload file %s error, %s%s", argv[1], filem_sys_errget(iRetv), VTY_NEWLINE);
        return(CMD_WARNING);
    }
}

DEFUN (filem_ftp_uload_unk6_fun,
       filem_ftp_uload_unk6_cmd,
       "ftp upload other WORD ipv6 X:X::X:X user WORD password WORD [WORD]",
       "File ftp operation\n"
       "File upload\n"
       "Upload other type file\n"
       "Input local file path and name\n"
       "Upload file use ipv6\n"
       "Input server ipv6 address\n"
       "Login user name\n"
       "Input user name\n"
       "Login user password\n"
       "Input user password\n"
       "Input remote file name, if no input, use the same local name\n") 
{
    int   iRetv = 0;

    char  bRName[FILEM_NAME_MAXLN];

    if(!filem_cli_namelencheck(vty, (char*)argv[0]))  
    {
        filem_upload_stat_set(-FILEM_ERROR_TOOLONG);  
        return CMD_WARNING;
    }
    
    if(strlen((char*)argv[2]) >= FILEM_USER_STRLN)  
    {
        filem_download_stat_set(-FILEM_ERROR_AUTHERROR);  
        vty_error_out(vty, "User name too long %s", VTY_NEWLINE);
        return(CMD_WARNING);
    }
    
    if(strlen((char*)argv[3]) >= FILEM_USER_STRLN)
    {
        filem_download_stat_set(-FILEM_ERROR_AUTHERROR);  
        vty_error_out(vty, "User password too long %s", VTY_NEWLINE);
        return(CMD_WARNING);
    }        
    
    if((argc == 5) && (argv[4] != NULL))
    {
        if(!filem_cli_namelencheck(vty, (char*)argv[2]))  
        {
            filem_upload_stat_set(-FILEM_ERROR_TOOLONG);  
            return CMD_WARNING;
        }
    }

    memset(bRName, 0, FILEM_NAME_MAXLN);

    if((argc == 5) && (argv[4] != NULL))
         strncpy(bRName, argv[4], FILEM_NAME_MAXLN -1);
    else filem_oper_cutname((char*)argv[0], bRName);
    
    iRetv = filem_cmd_ftpupload(FILEM_MTYPE_NON, (char*)argv[1],(char*)argv[2], (char*)argv[3], bRName, (char*)argv[0], filem_cli_proccall, vty, 0);
                                                                          /*必须同步调用,由主线程异步处理*/
    if(iRetv >= 0)
    {
        return(CMD_WAIT);
    }
    else
    {
        filem_upload_stat_set(iRetv);

        vty_error_out(vty, "Upload file %s error, %s%s", argv[0], filem_sys_errget(iRetv), VTY_NEWLINE);
        return(CMD_WARNING);
    }
}

/*------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------*/

DEFUN (filem_update_sys_fun,
       filem_update_sys_cmd,
       "file upgrade sys WORD (with|no) kernel (with|no) fpga {slot <1-31>}",
       "File operation\n"
       "File upgrade\n"
       "Sys upgrade\n"
       "Input sys file name\n"
       "Upgrade kernel\n"
       "No upgrade kernel\n"
       "Upgrade kernel select\n"
       "Upgrade fpga\n"
       "No upgrade fpga\n"
       "Upgrade fpga select\n"
       "Upgrade slot select\n"
       "Upgrade a special slot\n")
{
    int   iRetv = 0;
    int   iFlag = FILEM_OPTFLG_UPDAPP;
    int   iSlot = 0;

    if(filem_cli_namelencheck(vty, (char*)argv[0])) 
    {
        if(strcmp(argv[1], "with") == 0)  iFlag |= FILEM_OPTFLG_UPDKNL;

        if(strcmp(argv[2], "with") == 0)  iFlag |= FILEM_OPTFLG_UPDFPGA;
        
        if((argc > 3) && (argv[3] != NULL))
        {
            //if(strcmp(argv[3], "all") == 0) 
            //   iSlot = FILEM_SLOT_NUM_ALL;
            //else iSlot = atoi(argv[3]);

            iSlot = atoi(argv[3]);

            if(gfilelocslot != iSlot)
            {
                if(!filem_slot_ismaster())
                {
                    filem_upgrade_stat_set(-FILEM_ERROR_NOMASTER);
                    vty_error_out(vty, "Not master cpu, can't upgrade other slot.%s", VTY_NEWLINE);
                
                    return CMD_WARNING;
                }
                else
                {
                    if((iSlot != FILEM_SLOT_NUM_ALL) &&
                       (!filem_slot_cansync(iSlot)))
                    {
                        filem_upgrade_stat_set(-FILEM_ERROR_CONFLICT);
                        vty_error_out(vty, "Slot error or channel busy.%s", VTY_NEWLINE);
                    
                        return CMD_WARNING;
                    }
                }
            }
        }
    
        iRetv = filem_cmd_upgradesys(iSlot, (char *)argv[0], filem_cli_proccall, vty, iFlag, 1);

        if(iRetv < 0)
        {  
            if(iRetv == -FILEM_ERROR_NOOP) return CMD_WAIT;
            else
            {
                filem_upgrade_stat_set(iRetv);
            
                vty_error_out(vty, "Sys update %s error, %s%s", argv[0], filem_sys_errget(iRetv), VTY_NEWLINE);
                
                return CMD_WARNING;
            }
        }
        else return CMD_SUCCESS;
    }
    else
    {
        filem_upgrade_stat_set(-FILEM_ERROR_TOOLONG);
        return CMD_WARNING;
    }
}

DEFUN (filem_update_patch_fun,
       filem_update_patch_cmd,
       "file upgrade patch WORD {slot <1-31>}",
       "File operation\n"
       "File Upgrade\n"
       "Patch Upgrade\n"
       "Input patch file name\n" 
       "Upgrade slot select\n"
       "Upgrade a special slot\n")
{
    int  iRetv = 0;
    int  iSlot = 0;
    
    if(filem_cli_namelencheck(vty, (char*)argv[0])) 
    {
        if((argc > 1) && (argv[1] != NULL))
        {
            /*
            if(strcmp(argv[1], "all") == 0) 
                 iSlot = FILEM_SLOT_NUM_ALL;
            else iSlot = atoi(argv[1]);
            */

            iSlot = atoi(argv[1]);
            
            if(gfilelocslot != iSlot)
            {
                if(!filem_slot_ismaster())
                {
                    filem_upgrade_stat_set(-FILEM_ERROR_NOMASTER);
                    vty_error_out(vty, "Not master cpu, can't upgrade other slot.%s", VTY_NEWLINE);
                
                    return CMD_WARNING;
                }
                else
                {
                    if((iSlot != FILEM_SLOT_NUM_ALL) &&
                       (!filem_slot_cansync(iSlot)))
                    {
                        filem_upgrade_stat_set(-FILEM_ERROR_CONFLICT);
                        vty_error_out(vty, "Slot error or channel busy.%s", VTY_NEWLINE);
                    
                        return CMD_WARNING;
                    }
                }
            }
        }

        iRetv = filem_cmd_upgardepatch(iSlot, (char *)argv[0], filem_cli_proccall, vty, 1);

        if(iRetv < 0)
        {  
            if(iRetv == -FILEM_ERROR_NOOP)
                return CMD_WAIT;
            else
            {
                filem_upgrade_stat_set(iRetv);
                
                vty_error_out(vty, "Patch update %s error, %s%s", argv[0], filem_sys_errget(iRetv), VTY_NEWLINE);

                return CMD_WARNING;
            }
        }
        else return CMD_SUCCESS;
    }
    else  
    {
        filem_upgrade_stat_set(-FILEM_ERROR_TOOLONG);
        return CMD_WARNING;
    }
}

DEFUN (filem_update_fpga_fun,
       filem_update_fpga_cmd,
       "file upgrade fpga (bysys|byname) WORD {slot <1-31>}",
       "File operation\n"
       "File upgrade\n"
       "Fpga upgrade\n"
       "Fpga upgrade by sys file\n"
       "Fpga upgrade by a special file name\n"
       "Input sys file name(bysys) or fpga file name (byname)\n" 
       "Upgrade slot select\n"
       "Input a special slot\n")
{
    int   iRetv = 0;
    int   iSlot = 0;
    
    if(filem_cli_namelencheck(vty, (char*)argv[1])) 
    {
        if((argc > 2) && (argv[2] != NULL))
        {
            iSlot = atoi(argv[2]);
            
            if(gfilelocslot != iSlot)
            {
                if(!filem_slot_ismaster())
                {
                    filem_upgfpga_stat_set(-FILEM_ERROR_NOMASTER);
                    vty_error_out(vty, "Not master cpu, can't upgrade other slot.%s", VTY_NEWLINE);
                
                    return CMD_WARNING;
                }
                /*
                else
                {
                    if(!filem_slot_cansync(iSlot)) 
                    {
                        vty_out(vty, "Slot error or channel busy.%s", VTY_NEWLINE);
                    
                        return CMD_WARNING;
                    }
                }
                */
            }
        }

        if(strcmp(argv[0], "bysys") == 0) 
        {
            iRetv = filem_cmd_upgradefpga(iSlot, (char *)argv[1], filem_cli_proccall, vty, 1, FILEM_OPTFLG_BYSYS);
        }
        else
        {
            iRetv = filem_cmd_upgradefpga(iSlot, (char *)argv[1], filem_cli_proccall, vty, 1, 0);
        }
        
        if(iRetv < 0)
        {  
            if(iRetv == -FILEM_ERROR_NOOP)
                return CMD_WAIT;
            else
            {
                filem_upgfpga_stat_set(iRetv);
                
                vty_error_out(vty, "FPGA upgrade %s %s error, %s%s", argv[0], argv[1], filem_sys_errget(iRetv), VTY_NEWLINE);

                return CMD_WARNING;
            }
        }
        else return CMD_SUCCESS;
    }
    else  
    {
        filem_upgfpga_stat_set(-FILEM_ERROR_TOOLONG);
        return CMD_WARNING;
    }
}


/*------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------*/

DEFUN (filem_update_kernel_fun,
       filem_update_kernel_cmd,
       "file upgrade kernel (bysys|byname) WORD {slot <1-31>}",
       "File operation\n"
       "File upgrade\n"
       "Kernel upgrade\n"
       "Kernel upgrade by sys file\n"
       "Kernel upgrade by a special file name\n"
       "Input sys file name(bysys) or kernel file name (byname)\n" 
       "Upgrade slot select\n"
       "Input a special slot\n")
{
    int   iRetv = 0;
    int   iSlot = 0;
    
    if(filem_cli_namelencheck(vty, (char*)argv[1])) 
    {
        if((argc > 2) && (argv[2] != NULL))
        {
            iSlot = atoi(argv[2]);
            
            if(gfilelocslot != iSlot)
            {
                if(!filem_slot_ismaster())
                {
                    filem_upgkernel_stat_set(-FILEM_ERROR_NOMASTER);
                    vty_error_out(vty, "Not master cpu, can't upgrade other slot.%s", VTY_NEWLINE);
                
                    return CMD_WARNING;
                }
                /*
                else
                {
                    if(!filem_slot_cansync(iSlot)) 
                    {
                        vty_out(vty, "Slot error or channel busy.%s", VTY_NEWLINE);
                    
                        return CMD_WARNING;
                    }
                }
                */
            }
        }
        
        if(strcmp(argv[0], "bysys") == 0) 
        {
            iRetv = filem_cmd_upgradekernel(iSlot, (char*)argv[1], filem_cli_proccall, vty, 1, FILEM_OPTFLG_BYSYS);
        }
        else
        {
            iRetv = filem_cmd_upgradekernel(iSlot, (char*)argv[1], filem_cli_proccall, vty, 1, 0);
        }

        if(iRetv < 0)
        {  
            if(iRetv == -FILEM_ERROR_NOOP)
                return CMD_WAIT;
            else
            {
                filem_upgkernel_stat_set(iRetv);
                
                vty_error_out(vty, "Kernel upgrade %s %s error, %s%s", argv[0], argv[1],filem_sys_errget(iRetv), VTY_NEWLINE);

                return CMD_WARNING;
            }
        }
        else return CMD_SUCCESS;
    }
    else 
    {
        filem_upgkernel_stat_set(-FILEM_ERROR_TOOLONG);
        return CMD_WARNING;
    }
}


DEFUN (filem_update_appbin_fun,
       filem_update_appbin_cmd,
       "file upgrade app (bysys|byname) WORD {slot <1-31>}",
       "File operation\n"
       "File upgrade\n"
       "App upgrade\n"
       "App upgrade by sys file\n"
       "App upgrade by a special file name\n"
       "Input sys file name(bysys) or app file name (byname)\n" 
       "Upgrade slot select\n"
       "Input a special slot\n")
{
    int   iRetv = 0;
    int   iSlot = 0;
    
    if(filem_cli_namelencheck(vty, (char*)argv[1])) 
    {
        if((argc > 2) && (argv[2] != NULL))
        {
            iSlot = atoi(argv[2]);
            
            if(gfilelocslot != iSlot)
            {
                if(!filem_slot_ismaster())
                {
                    filem_upgrade_stat_set(-FILEM_ERROR_NOMASTER);
                    vty_error_out(vty, "Not master cpu, can't upgrade other slot.%s", VTY_NEWLINE);
                
                    return CMD_WARNING;
                }
                /*
                else
                {
                    if(!filem_slot_cansync(iSlot)) 
                    {
                        vty_out(vty, "Slot error or channel busy.%s", VTY_NEWLINE);
                    
                        return CMD_WARNING;
                    }
                }
                */
            }
        }
        
        if(strcmp(argv[0], "bysys") == 0) 
        {
            iRetv = filem_cmd_upgradeappbin(iSlot, (char*)argv[1], filem_cli_proccall, vty, 1, FILEM_OPTFLG_BYSYS);
        }
        else
        {
            iRetv = filem_cmd_upgradeappbin(iSlot, (char*)argv[1], filem_cli_proccall, vty, 1, 0);
        }

        if(iRetv < 0)
        {  
            if(iRetv == -FILEM_ERROR_NOOP)
                return CMD_WAIT;
            else
            {
                filem_upgrade_stat_set(iRetv);
                
                vty_error_out(vty, "App upgrade %s %s error, %s%s", argv[0], argv[1], filem_sys_errget(iRetv), VTY_NEWLINE);
                
                return CMD_WARNING;
            }
        }
        else return CMD_SUCCESS;
    }
    else  
    {
        filem_upgrade_stat_set(-FILEM_ERROR_TOOLONG);
        return CMD_WARNING;
    }
}



DEFUN (filem_update_uboot_fun,
       filem_update_uboot_cmd,
       "file upgrade uboot WORD", 
       "File operation\n"
       "File upgrade\n"
       "UBOOT upgrade\n"
       "Input uboot file name\n" 
       "Upgrade slot select\n"
       "Upgrade a special slot\n")
{
    int   iRetv = 0;
    int   iSlot = 0;

    if(filem_cli_namelencheck(vty, (char*)argv[0])) 
    {
        iRetv = filem_cmd_upgradeuboot(iSlot, (char *)argv[0], NULL, vty, 0);

        if(iRetv < 0)
        {
           vty_error_out(vty, "UBOOT upgrade version %s error, %s%s", argv[0], filem_sys_errget(iRetv), VTY_NEWLINE);
           return CMD_WARNING;
        }
        else
        {
           vty_error_out(vty, "UBOOT upgrade version %s ok!%s", argv[0], VTY_NEWLINE);

           return CMD_SUCCESS;
        }
    }
    else return CMD_WARNING;
}

DEFUN (filem_btb_transfer_fun,
       filem_btb_transfer_cmd,
       "file transfer (config|data|sys) WORD (to|from) slot <1-31>",
       "File operation\n"
       "File transfer\n"
       "Select config file\n"
       "Select data file\n"
       "Select sys file\n"
       "Input file name\n"
       "Transfer to a destnation slot\n"
       "Transfer from a destnation slot \n"
       "destnation slot\n"
       "Input slot no\n")
{
    int   iSlot = 0;
    int   iRetv = 0;
    int   iStyp = 0;
    char  bFname[128] = {0};
    
    if(filem_cli_namelencheck(vty, (char*)argv[1]))
    {
        if(!filem_slot_ismaster())
        {
            vty_error_out(vty, "Only master can run this command!%s", VTY_NEWLINE);
            return(CMD_WARNING);
        }

        iStyp = filem_cli_typenametoid((char*)argv[0]);

        snprintf(bFname, 127, "%s%s", filem_type_pathget(iStyp), argv[1]);


        iSlot = atoi(argv[3]);
#if 1
        if((iSlot != gfilelocslot) && 
           !filem_sync_rttranrun() && filem_slot_cansync(iSlot))
        {
            if(strcmp(argv[2], "to") == 0)
            {
                iRetv = filem_tran_putfile(0, iSlot, bFname, NULL, filem_cli_proccall, vty);
            }
            else
            {
                iRetv = filem_tran_getfile(0, iSlot, NULL, bFname, filem_cli_proccall, vty);
            }
                                                                          /*同步调用,由主线程异步处理*/
            if(iRetv < 0)
            {
               vty_error_out(vty, "File %s transfer error, %s%s", argv[1], filem_sys_errget(iRetv),VTY_NEWLINE);
               return CMD_WARNING;
            }
            else
            {
               vty_out(vty, "File %s transfer request ok! please wait! %s", argv[1], VTY_NEWLINE);

               return CMD_WAIT;
            }
        }
        else
        {
            vty_error_out(vty, "Slot error or channel busy!%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
#else
    
        if(strcmp(argv[2], "to") == 0)
        {
            filem_sync_rtfilesync(0, iSlot, IPC_OPCODE_ADD, 0, bFname); 
        }
        else
        {
            filem_sync_rtfilesync(0, iSlot, IPC_OPCODE_GET, 1, bFname); 
        }

        return CMD_WARNING;
#endif       
    }
    else return CMD_WARNING;
}


DEFUN (filem_btb_delete_fun,
       filem_btb_delete_cmd,
       "file delete (config|data|sys) WORD from slot <1-31>",
       "File operation\n"
       "File delete\n"
       "Select config file\n"
       "Select data file\n"
       "Select sys file\n"
       "Input file name\n"
       "Delete file from special slot\n"
       "Transfer to destnation slot\n"
       "Input slot no\n")
{
     int   iSlot = 0;
     int   iRetv = 0;
     int   iStyp = 0;
     char  bFname[128] = {0};

     if(filem_cli_namelencheck(vty, (char*)argv[1]))
     {
         iStyp = filem_cli_typenametoid((char*)argv[0]);

         snprintf(bFname, 127, "%s%s", filem_type_pathget(iStyp), argv[1]);
         
         iSlot = atoi(argv[2]);

         if(gfilelocslot == iSlot)
         {
             iRetv = filem_cmd_remove(FILEM_MTYPE_NON, bFname, NULL, NULL, 0);
 
             if(iRetv < 0)
             {
                 vty_error_out(vty, "Delete file %s error, %s%s", argv[1], filem_sys_errget(iRetv), VTY_NEWLINE);
                 return CMD_WARNING;
             }
             else return CMD_SUCCESS;
         }
         else
         {
             if(!filem_slot_ismaster())
             {
                 vty_error_out(vty, "Not master cpu, can delete other slot's file!%s", VTY_NEWLINE);
                 return(CMD_WARNING);
             }

             if(filem_slot_cansync(iSlot))
             {
                 iRetv = filem_tran_delfile(0, iSlot, NULL, bFname, filem_cli_proccall, vty);
                                                                              /*同步调用,由主线程异步处理*/
                 if(iRetv < 0)
                 {
                     vty_error_out(vty, "Delete slot %s file %s error, %s%s", 
                                  argv[2], argv[1], filem_sys_errget(iRetv),VTY_NEWLINE);
                     return CMD_WARNING;
                 }
                 else
                 {
                     vty_out(vty, "Delete slot %s file %s request ok! please wait! %s", argv[2], argv[1], VTY_NEWLINE);

                     return CMD_WAIT;
                 }
             }
             else
             {
                 vty_error_out(vty, "Slot error or channel busy!%s", VTY_NEWLINE);
                 return(CMD_WARNING);
             }

         }
     }
     else return CMD_WARNING;
}


DEFUN (filem_active_kernel_fun,
       filem_active_kernel_cmd,
       "file active kernel switch {slot <1-31>}",
       "File operation\n"
       "File active a version\n"
       "Kernel version active\n"
       "Kernel version active switch\n"
       "Upgrade slot select\n"
       "Upgrade a special slot\n")
{
    int   iRetv = 0;
    int   iSlot = 0;
    
    if((argc > 0) && (argv[0] != NULL))
    {
        iSlot = atoi(argv[0]);
        
        if(gfilelocslot != iSlot)
        {
            if(!filem_slot_ismaster())
            {
                vty_error_out(vty, "Not master cpu, can't active other slot.%s", VTY_NEWLINE);
            
                return CMD_WARNING;
            }
            /*
            else
            {
                if(!filem_slot_cansync(iSlot)) 
                {
                    vty_out(vty, "Slot error or channel busy.%s", VTY_NEWLINE);
                
                    return CMD_WARNING;
                }
            }
            */
        }
    }
    
    iRetv = filem_cmd_activekernel(iSlot, 0, filem_cli_proccall, vty, 0);

    if(iRetv < 0)
    {  
         vty_error_out(vty, "Kernel active switch error, %s%s", filem_sys_errget(iRetv), VTY_NEWLINE);
         return CMD_WARNING;
    }
    else return CMD_SUCCESS;
}

DEFUN (filem_l3vpn_set_fun,
       filem_l3vpn_set_cmd,
       "file l3vpn <0-127>",
       "File operation\n"
       "File tftp l3vpn config\n"
       "Vpn number 0-127\n")
{
    int vpn = 0;
    
    if((argc > 0) && (argv[0] != NULL))
    {
        vpn = atoi(argv[0]);

        filem_tftp_l3vpn_set(vpn);
    }

    return CMD_SUCCESS;
}   
       
/*------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------*/

DEFUN (filem_share_memcache_fun,
       filem_share_memcache_cmd,
       "show sys sharememory cache",
       SHOW_STR
       "Sys information\n"
       "Share memory infor\n"
       "Share cache information\n")
{
    int  iInLn = 0;
    char bInfo[2048] = {0};

    iInLn = mem_share_cacheinfor(bInfo, 2048);

    if(iInLn > 0)
         vty_out(vty, "%s%s", bInfo, VTY_NEWLINE);
    else vty_error_out(vty, "Show share memory cache infor error. %s", VTY_NEWLINE);

    return CMD_SUCCESS;
}

DEFUN (filem_share_memmodule_fun,
       filem_share_memmodule_cmd,
       "show sys sharememory block",
       SHOW_STR
       "Sys information\n"
       "Share memory infor\n"
       "Share blocks statistics information\n")
{
    int   iInLn = 20000;
    char *bInfo = NULL;
    
    bInfo = XMALLOC(MTYPE_FILEM_ENTRY, iInLn);

    if(bInfo)
    {
        iInLn = mem_share_moduleinfor(bInfo, iInLn);

        if(iInLn > 0)
             vty_out(vty, "%s%s", bInfo, VTY_NEWLINE);
        else vty_error_out(vty, "Show share memory blocks infor error. %s", VTY_NEWLINE);
        XFREE(MTYPE_FILEM_ENTRY, bInfo);
    }
    else vty_error_out(vty, "Show share memory blocks infor error, no memory!%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}


DEFUN (filem_share_memmsginf_fun,
       filem_share_memmsginf_cmd,
       "show sys sharememory msgdata (64b|128b|256b|512b|1kb|2kb|4kb|8kb|16kb) <1-128>",
       SHOW_STR
       "Sys information\n"
       "Share memory infor\n"
       "Share share memory message data information\n"
       "Message data in 64 bytes blocks\n"
       "Message data in 128 bytes blocks\n"
       "Message data in 256 bytes blocks\n"
       "Message data in 512 bytes blocks\n"
       "Message data in 1024 bytes blocks\n"
       "Message data in 2048 bytes blocks\n"
       "Message data in 4096 bytes blocks\n"
       "Message data in 8192 bytes blocks\n"
       "Message data in 16384 bytes blocks\n"
       "Input show max blocks\n")
{
    int   iInLn = 20000;
    int   iCach = 0;
    int   iShCo = 0;
    char *bInfo = NULL;

    if(strcmp(argv[0], "64b") == 0)
         iCach = 0;
    else if(strcmp(argv[0], "128b") == 0)
         iCach = 1;
    else if(strcmp(argv[0], "256b") == 0)
         iCach = 2;
    else if(strcmp(argv[0], "512b") == 0)
         iCach = 3;
    else if(strcmp(argv[0], "1kb") == 0)
         iCach = 4;
    else if(strcmp(argv[0], "2kb") == 0)
         iCach = 5;
    else if(strcmp(argv[0], "4kb") == 0)
         iCach = 6;
    else if(strcmp(argv[0], "8kb") == 0)
         iCach = 7;
    else iCach = 8;

    iShCo = atoi(argv[1]);

    iInLn = 128 * iShCo + 1024;
    
    bInfo = XMALLOC(MTYPE_FILEM_ENTRY, iInLn);

    if(bInfo)
    {
        iInLn = mem_share_usedblkinfor(bInfo, iInLn, iCach, iShCo);

        if(iInLn > 0)
             vty_out(vty, "%s%s", bInfo, VTY_NEWLINE);
        else vty_error_out(vty, "Show share memory message infor error. %s", VTY_NEWLINE);
        XFREE(MTYPE_FILEM_ENTRY, bInfo);
    }
    else vty_error_out(vty, "Show share memory message infor error, no memory!%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}


DEFUN (filem_show_timerinfo_fun,
       filem_show_timerinfo_cmd,
       "show filem timer",
       SHOW_STR
       "File module information\n"
       "File module timer\n")
{
    int   iInLn = 1000;
    char  bInfo[1000] = {0};
    
   
    iInLn = high_pre_timer_info(bInfo, iInLn);

    if(iInLn > 0)
         vty_out(vty, "%s%s", bInfo, VTY_NEWLINE);
    else vty_error_out(vty, "Show file module timer infor error. %s", VTY_NEWLINE);

    return CMD_SUCCESS;
}

/*------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------*/
static int filem_config_write(struct vty *vty)
{
    if(filem_tftp_l3vpn_get() != 0)
    {
        vty_out(vty, "file l3vpn %d%s",  filem_tftp_l3vpn_get(), VTY_NEWLINE);
    }

    return(CMD_SUCCESS);
}

       
/*------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------*/

static struct cmd_node filem_node =
{
  FILEM_NODE,
  "%s(config-file)# ",
  1
};
/*----------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------*/

int filem_cli_init(void)
{
    install_node(&filem_node, filem_config_write);

    install_default(FILEM_NODE);

    install_element(CONFIG_NODE, &filem_conf_debug_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_show_debug_cmd, CMD_LOCAL);

    install_element(CONFIG_NODE, &filem_show_type_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_show_unkn_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_show_padi_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_show_bootarea_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_show_knlarea_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_show_slotver_cmd, CMD_LOCAL);
       
    install_element(CONFIG_NODE, &filem_delete_type_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_rename_type_cmd, CMD_LOCAL);   
    install_element(CONFIG_NODE, &filem_copy_type_cmd, CMD_LOCAL);
    
    install_element(CONFIG_NODE, &filem_dload_ver4_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_dload_unk4_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_dload_ver6_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_dload_unk6_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_uload_ver4_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_uload_unk4_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_uload_ver6_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_uload_unk6_cmd, CMD_LOCAL);

    install_element(CONFIG_NODE, &filem_ftp_dload_ver4_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_ftp_dload_unk4_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_ftp_dload_ver6_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_ftp_dload_unk6_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_ftp_uload_ver4_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_ftp_uload_unk4_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_ftp_uload_ver6_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_ftp_uload_unk6_cmd, CMD_LOCAL);

    install_element(CONFIG_NODE, &filem_update_sys_cmd,   CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_update_patch_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_update_fpga_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_update_kernel_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_update_appbin_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_active_kernel_cmd, CMD_LOCAL);

    install_element(CONFIG_NODE, &filem_share_memcache_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_share_memmodule_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_share_memmsginf_cmd, CMD_LOCAL);   

    install_element(CONFIG_NODE, &filem_show_timerinfo_cmd, CMD_LOCAL);
    
	install_element(CONFIG_NODE, &filem_l3vpn_set_cmd, CMD_SYNC);
	 
  //install_element(CONFIG_NODE, &filem_update_uboot_cmd);

    install_element(CONFIG_NODE, &filem_btb_transfer_cmd, CMD_LOCAL);
  
    install_element(CONFIG_NODE, &filem_btb_delete_cmd, CMD_LOCAL);  /* only for debug*/

    return(0);
}
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

