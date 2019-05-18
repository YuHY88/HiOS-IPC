/*file management head file
  name filem_image.h
  edit suxq
  date 2016/05/23
  desc image file manage kernel boot  
*/
#ifndef _FILEM_IMAGE_H_
#define _FILEM_IMAGE_H_

#include "filem_md5.h"
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
#define FILEM_BOOT_DEVICE        "/dev/mtd0" 

#define FILEM_BOOT_ASIZE         0x100000;

#define FILEM_KRL_AREA_TBL       "/dev/mtd5" //"/boottbl"

#define FILEM_KRL_DEV_AREA0      "ubi0_0"

#define FILEM_KRL_DEV_AREA1      "ubi1_0"

#define FILEM_KRL_PATH_AREA0     "/os1"

#define FILEM_KRL_PATH_AREA1     "/os2"

#define FILEM_KRL_KNL_IMAGE      "uImage"

#define FILEM_KRL_WRT_FUNC       "ubiupdatevol"

#define FILEM_KRL_DEV_MTDNUM      2 
                                  /*不使用UBI文件系统时, KNL的MTD设备号 此类设备必须保持该设备号不变*/
/*-----------------------------------------------------------------------------------------*/

#define FILEM_KRL_AREA_MAX        2

/* kernel boot area info struct */
typedef struct _kernelarea_
{
    unsigned char  isused[4];     /*启动区存在标志*/
    
    unsigned char  valid[4];      /*启动区KNL数据存在标志*/ 

    unsigned char  active[4];     /*启动区激活标志*/ 

    int            areasize;      /*启动区大小*/ 

    int            datalen;       /*数据长度*/ 
   
    char           filevers[FILEM_VERS_MAXLN];/*版本*/

    char           md5data[FILEM_PAD_MD5_MARK];/*MD5标签*/
}kernelarea;

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
int filem_img_init(void);

int filem_img_kernel_info_get(char *bBufDat, int iSlot);

int filem_fpga_upgrade(int slot, int index, char *filename);

int filem_img_kernel_upgrade(char *bBName);

int filem_img_knlareaset(int index, int used, int size);

int filem_img_kernel_activesw(void);

int filem_img_boot_upgrade(char *bFName);

int filem_img_bootinfo_get(char *bBufDat);

int filem_img_bootver_get(char *bBufDat);

int filem_img_kernelver_get(char *bBufDat);

#endif

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

