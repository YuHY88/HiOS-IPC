/*file management head file
  name filem_md5.h
  edit suxq
  date 2016/05/23
  desc file data md5 check 
*/
#ifndef _FILEM_MD5_H_

#define _FILEM_MD5_H_

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
#define FILEM_PAD_MD5_MARK           16
#define FILEM_PAD_MD5_BUSZ          (FILEM_PAD_MD5_MARK* 2)

#define FILEM_PAD_INF_SIZE          128

#define FILEM_PAD_INF_VER_LEN        16
#define FILEM_PAD_INF_DATE_LEN       16
#define FILEM_PAD_INF_MD5_LEN        32
#define FILEM_PAD_INF_DEVID_LEN      12
#define FILEM_PAD_INF_DESC_LEN       40
#define FILEM_PAD_INF_FLAG_LEN        8

#define FILEM_PAD_INF_FLAG_DATA     "hhpfmpkt"

/* file append info struct */
typedef struct _filepadinf_
{
    char          ver[FILEM_PAD_INF_VER_LEN];        /*文件版本, ascii*/
    char          date[FILEM_PAD_INF_DATE_LEN];      /*文件创建的时间, ascii*/
    char          md5[FILEM_PAD_INF_MD5_LEN];        /*md5 标签, ascii*/
    unsigned char len[4];                            /*文件原始大小, hex bytes*/
    char          devid[FILEM_PAD_INF_DEVID_LEN];    /*设备id, ascii*/
    char          desc[FILEM_PAD_INF_DESC_LEN];      /*文件描述, ascii*/
    char          flags[FILEM_PAD_INF_FLAG_LEN];     /*是否存在MD5标记, ascii*/
}filepadinf;

int filem_md5_pad_getbyname(char *pFile, filepadinf *pPad, int iSize);

int filem_md5_calculate(int iFd, int iBlkLn, char *bMd5, int iAllLn);

int filem_md5_matchcheck(char *pFile, int iflen);

int filem_md5_filepadinf(char *pFile, char **pInfo);

int filem_md5_integritychk(char *pFile);

int filem_md5_wholefile(char *pFile, char *bOut, int iBsize);

#endif

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

