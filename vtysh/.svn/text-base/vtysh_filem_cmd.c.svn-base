
#include <zebra.h>
#include "command.h"
#include "vtysh.h"

DEFSH (VTYSH_FILEM, 
       filem_show_type_cmd_vtysh,
       "show file (bin|config|data|sys) {slot <1-31>}",
       SHOW_STR
       "Display file list\n"
       "Display exec file list,app and driver\n"
       "Display config file list\n"
       "Display data file list, log alarm and statistics\n"
       "Display sys version file list\n"
       "Select slot\n"
       "Input slot no\n")


DEFSH (VTYSH_FILEM, 
       filem_show_unkn_cmd_vtysh,
       "show file other WORD",
       SHOW_STR
       "Display file list\n"
       "Display file list in a special path\n"
       "Input path name(whole path)\n")

DEFSH (VTYSH_FILEM,
       filem_show_padi_cmd_vtysh,
       "show file version WORD",
       SHOW_STR
       "Display file info\n"
       "Display version info of sys file\n"
       "Input name\n")

DEFSH (VTYSH_FILEM,
       filem_show_slot_cmd_vtysh,
       "show file slot info",
       SHOW_STR
       "Display file info\n"
       "Display slot hardware info\n"
       "Display slot hardware info\n")
       
DEFSH (VTYSH_FILEM,
       filem_show_bootarea_cmd_vtysh,
       "show file boot",
       SHOW_STR
       "Display file info\n"
       "Display boot info\n")

DEFSH (VTYSH_FILEM,
       filem_show_knlarea_cmd_vtysh,
       "show file kernel {slot <1-31>}",
       SHOW_STR
       "Display file information\n"
       "Display kernel image information\n" 
       "Display a special slot\n" 
       "Input a special slot number\n")



DEFSH (VTYSH_FILEM,
       filem_show_slotver_cmd_vtysh,
       "show file state infor {slot <1-31>}",
       SHOW_STR
       "Display file information\n"
       "Display file state information\n"
       "Display ver or status information\n"
       "Display a special slot\n" 
       "Input a special slot number\n")


DEFSH (VTYSH_FILEM,
       filem_delete_type_cmd_vtysh,
       "file delete (config|data|sys) WORD",
       "File operation\n"
       "File delete\n"
       "Delete config file\n"
       "Delete data file,log or alarm\n"
       "Delete sys version file\n"
       "Input file name\n")


DEFSH (VTYSH_FILEM,
       filem_rename_type_cmd_vtysh,
       "file rename (config|data|sys) WORD WORD",
       "File operation\n"
       "File rename\n"
       "Rename config file\n"
       "Rename data file,log or alarm file\n"
       "Rename sys version file\n"
       "Input src file name\n" 
       "Input des file name\n")

DEFSH (VTYSH_FILEM,
       filem_copy_type_cmd_vtysh,
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


DEFSH (VTYSH_FILEM,
       filem_dload_ver4_cmd_vtysh,
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

DEFSH (VTYSH_FILEM,
       filem_dload_unk4_cmd_vtysh,
       "file download other ipv4 A.B.C.D WORD WORD",
       "File operation\n"
       "File download\n"
       "Download a file to speical path\n"
       "Download file use ipv4\n"
       "Input server ipv4 address\n"
       "Input remote file name \n" 
       "Input local file name, must with path\n")

DEFSH (VTYSH_FILEM,
       filem_dload_ver6_cmd_vtysh,
       "file download ipv6 X:X::X:X (sys|bin|config) WORD [WORD]",
       "File operation\n"
       "File download\n"
       "Download file use ipv6\n"
       "Input server ipv6 address\n"
       "Download a sys version file\n"
       "Download a app bin file\n"
       "Download a config file\n"
       "Input remote file name \n" 
       "Input local file name, if no input, use the same remote\n")

DEFSH (VTYSH_FILEM,
       filem_dload_unk6_cmd_vtysh,
       "file download ipv6 X:X::X:X other WORD WORD",
       "File operation\n"
       "File download\n"
       "Download file use ipv6\n"
       "Input server ipv6 address\n"
       "Download a file to speical path\n"
       "Input remote file name \n" 
       "Input local file name, must with path\n")


DEFSH (VTYSH_FILEM,
       filem_uload_ver4_cmd_vtysh,
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
      
DEFSH (VTYSH_FILEM,
       filem_uload_unk4_cmd_vtysh,
       "file upload other WORD ipv4 A.B.C.D [WORD]",
       "File operation\n"
       "File download\n"
       "Upload other type file\n"
       "Input local file path and name\n" 
       "Upload file use ipv4\n"
       "Input server ipv4 address\n"
       "Input remote file name, if no input, use the same local name\n") 

DEFSH (VTYSH_FILEM,
       filem_uload_ver6_cmd_vtysh,
       "file upload ipv6 X:X::X:X (bin|config|data|sys) WORD [WORD]",
       "File operation\n"
       "File download\n"
       "Upload file use ipv6\n"
       "Input server ipv6 address\n"
       "Upload exec file\n"
       "Upload config file\n"
       "Upload data file\n"
       "Upload sys version file\n"
       "Input local file name \n" 
       "Input remote file name, if no input, use the same local\n")

DEFSH (VTYSH_FILEM,
       filem_uload_unk6_cmd_vtysh,
       "file upload ipv6 X:X::X:X other WORD [WORD]",
       "File operation\n"
       "File download\n"
       "Download file use ipv6\n"
       "Input server ipv6 address\n"
       "Download version type file\n"
       "Input local file path and name\n"
       "Input remote file name, if no input, use the same local\n")

DEFSH (VTYSH_FILEM,
       filem_ftp_dload_ver4_cmd_vtysh,
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


DEFSH (VTYSH_FILEM,
       filem_ftp_dload_unk4_cmd_vtysh,
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
       "Input remote file name\n" 
       "Input local file name, must with path\n")


DEFSH (VTYSH_FILEM,
       filem_ftp_dload_ver6_cmd_vtysh,
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
       "Input remote file name\n" 
       "Input local file name, if no input, use the same remote\n")


DEFSH (VTYSH_FILEM,
       filem_ftp_dload_unk6_cmd_vtysh,
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
       "Input remote file name\n" 
       "Input local file name, must with path\n")


DEFSH (VTYSH_FILEM,
       filem_ftp_uload_ver4_cmd_vtysh,
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


DEFSH (VTYSH_FILEM,
       filem_ftp_uload_unk4_cmd_vtysh,
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


DEFSH (VTYSH_FILEM,
       filem_ftp_uload_ver6_cmd_vtysh,
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


DEFSH (VTYSH_FILEM,
       filem_ftp_uload_unk6_cmd_vtysh,
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


DEFSH (VTYSH_FILEM,
       filem_update_sys_cmd_vtysh,
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

DEFSH (VTYSH_FILEM,
       filem_update_patch_cmd_vtysh,
       "file upgrade patch WORD {slot <1-31>}",
       "File operation\n"
       "File Upgrade\n"
       "Patch Upgrade\n"
       "Input patch file name\n" 
       "Upgrade slot select\n"
       "Upgrade a special slot\n")


DEFSH (VTYSH_FILEM,
       filem_update_fpga_cmd_vtysh,
       "file upgrade fpga (bysys|byname) WORD {slot <1-31>}",
       "File operation\n"
       "File upgrade\n"
       "Fpga upgrade\n"
       "Fpga upgrade by sys file\n"
       "Fpga upgrade by a special file name\n"
       "Input sys file name(bysys) or fpga file name(byname)\n" 
       "Upgrade slot select\n"
       "Input a special slot\n")


DEFSH (VTYSH_FILEM,
       filem_update_kernel_cmd_vtysh,
       "file upgrade kernel (bysys|byname) WORD {slot <1-31>}",
       "File operation\n"
       "File upgrade\n"
       "Kernel upgrade\n"
       "Kernel upgrade by sys file\n"
       "Kernel upgrade by a special file name\n"
       "Input sys file name(bysys) or kernel file name(byname)\n" 
       "Upgrade slot select\n"
       "Input a special slot\n")


DEFSH (VTYSH_FILEM,
       filem_update_appbin_cmd_vtysh,
       "file upgrade app (bysys|byname) WORD {slot <1-31>}",
       "File operation\n"
       "File upgrade\n"
       "App upgrade\n"
       "App upgrade by sys file\n"
       "App upgrade by a special file name\n"
       "Input sys file name(bysys) or app file name(byname)\n" 
       "Upgrade slot select\n"
       "Input a special slot\n")


DEFSH (VTYSH_FILEM,
       filem_update_uboot_cmd_vtysh,
       "file upgrade uboot WORD",// {slot <1-32>}",
       "File operation\n"
       "File upgrade\n"
       "UBOOT upgrade\n"
       "Input uboot file version name\n" 
       "Upgrade slot select\n"
       "Upgrade a special slot\n")


DEFSH (VTYSH_FILEM,
       filem_conf_debug_cmd_vtysh,
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
       
DEFSH (VTYSH_FILEM,
       filem_show_debug_cmd_vty,
       "show debug filem",
        SHOW_STR
       "Debuv config\n"
       "Clear filem debug set\n")


DEFSH (VTYSH_FILEM,
       filem_btb_transfer_cmd_vtysh,
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


DEFSH (VTYSH_FILEM,
       filem_btb_delete_cmd_vtysh,
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


DEFSH (VTYSH_FILEM,
       filem_active_kernel_cmd_vtysh,
       "file active kernel switch {slot <1-31>}",
       "File operation\n"
       "File active a version\n"
       "Kernel version active\n"
       "Kernel version active switch\n"
       "Upgrade slot select\n"
       "Upgrade a special slot\n")


DEFSH (VTYSH_FILEM,
       filem_share_memcache_cmd_vtysh,
       "show sys sharememory cache",
       SHOW_STR
       "Sys information\n"
       "Share memory infor\n"
       "Share cache information\n")


DEFSH (VTYSH_FILEM,
       filem_share_memmodule_cmd_vtysh,
       "show sys sharememory block",
       SHOW_STR
       "Sys information\n"
       "Share memory infor\n"
       "Share blocks statistics information\n")


DEFSH (VTYSH_FILEM,
       filem_share_memmsginf_cmd_vtysh,
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



DEFSH (VTYSH_FILEM,
       filem_show_timerinfo_cmd_vtysh,
       "show filem timer",
       SHOW_STR
       "File module information\n"
       "File module timer\n")

DEFSH (VTYSH_FILEM,
       filem_l3vpn_set_cmd_vtysh,
       "file l3vpn <0-127>",
       "File operation\n"
       "File tftp l3vpn config\n"
       "Vpn number 0-127\n")


void vtysh_init_filem_cmd (void)
{
    install_element(CONFIG_NODE, &filem_show_type_cmd_vtysh,CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_show_unkn_cmd_vtysh,CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_show_padi_cmd_vtysh,CMD_LOCAL);
    //install_element(CONFIG_NODE, &filem_show_slot_cmd_vtysh);//deletedby lihz
    install_element(CONFIG_NODE, &filem_show_bootarea_cmd_vtysh,CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_show_knlarea_cmd_vtysh,CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_show_slotver_cmd_vtysh,CMD_LOCAL);

    install_element(CONFIG_NODE, &filem_delete_type_cmd_vtysh,CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_rename_type_cmd_vtysh,CMD_LOCAL);   
    install_element(CONFIG_NODE, &filem_copy_type_cmd_vtysh,CMD_LOCAL);
    
    install_element(CONFIG_NODE, &filem_dload_ver4_cmd_vtysh,CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_dload_unk4_cmd_vtysh,CMD_LOCAL);
  //install_element(CONFIG_NODE, &filem_dload_ver6_cmd_vtysh);//deleted by lihz
  //install_element(CONFIG_NODE, &filem_dload_unk6_cmd_vtysh);//deleted by lihz
    install_element(CONFIG_NODE, &filem_ftp_dload_ver4_cmd_vtysh,CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_ftp_dload_unk4_cmd_vtysh,CMD_LOCAL);
  //install_element(CONFIG_NODE, &filem_ftp_dload_ver6_cmd_vtysh);//deleted by lihz
  //install_element(CONFIG_NODE, &filem_ftp_dload_unk6_cmd_vtysh);//deleted by lihz
    
    install_element(CONFIG_NODE, &filem_uload_ver4_cmd_vtysh,CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_uload_unk4_cmd_vtysh,CMD_LOCAL);
  //install_element(CONFIG_NODE, &filem_uload_ver6_cmd_vtysh);//deleted by lihz
  //install_element(CONFIG_NODE, &filem_uload_unk6_cmd_vtysh);//deleted by lihz
    install_element(CONFIG_NODE, &filem_ftp_uload_ver4_cmd_vtysh,CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_ftp_uload_unk4_cmd_vtysh,CMD_LOCAL);
  //install_element(CONFIG_NODE, &filem_ftp_uload_ver6_cmd_vtysh);//deleted by lihz
  //install_element(CONFIG_NODE, &filem_ftp_uload_unk6_cmd_vtysh);//deleted by lihz

    install_element(CONFIG_NODE, &filem_update_sys_cmd_vtysh,CMD_LOCAL);
  //install_element(CONFIG_NODE, &filem_update_patch_cmd_vtysh,CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_update_fpga_cmd_vtysh,CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_update_kernel_cmd_vtysh,CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_update_appbin_cmd_vtysh,CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_active_kernel_cmd_vtysh,CMD_LOCAL);

    install_element(CONFIG_NODE, &filem_l3vpn_set_cmd_vtysh,CMD_SYNC);
    
  //install_element(CONFIG_NODE, &filem_update_uboot_cmd_vtysh);

    install_element(CONFIG_NODE, &filem_btb_transfer_cmd_vtysh,CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_btb_delete_cmd_vtysh,CMD_LOCAL);
    
    install_element(CONFIG_NODE, &filem_conf_debug_cmd_vtysh,CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_show_debug_cmd_vty, CMD_LOCAL);

    install_element(CONFIG_NODE, &filem_share_memcache_cmd_vtysh,CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_share_memmodule_cmd_vtysh,CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_share_memmsginf_cmd_vtysh,CMD_LOCAL);
    install_element(CONFIG_NODE, &filem_show_timerinfo_cmd_vtysh,CMD_LOCAL);
    
    return ;
}


