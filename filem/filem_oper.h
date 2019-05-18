/*file management head file
  name filem.h
  edit suxq
  date 2016/05/23
*/
#ifndef _FILEM_OPER_H_

#define _FILEM_OPER_H_

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
int filem_oper_namelist(char *pPath, char  *pfilter, 
                        int iBufLen, char *pNamedat);

int filem_oper_nameparse(char *pName, char **pfield, int iCou, char bsp);

int filem_oper_mkdir(char *pPath);

int filem_oper_isexist(char *pFile);

int filem_oper_isdir(char *pFile);

int filem_oper_size_get(char *pFile);

int filem_oper_size_getf(int iFd);

int filem_oper_seek(int iFd, int iOff, int iWhen);

void filem_oper_flush(void);

int filem_oper_syncf(int iFd);

int filem_oper_read(int iFd, char *pBuff, int iLen);

int filem_oper_write(int iFd, char *pBuff, int iLen);

int filem_oper_close(int iFd);

int filem_oper_open(char *pFile, int iFlag, int iMode);

int filem_oper_erase(int iFd, int istart, int len);

int filem_oper_sectorsize_get(int iFd);

int filem_oper_namechk(char *pFile);

int filem_oper_cutpath(char *pFile, char *pPath);

int filem_oper_cutname(char *pFile, char *pSname);

int filem_oper_pathchk(char *pPath);

int filem_oper_withpath(char *pFile);

int filem_oper_remove(char *pFile);

int filem_oper_rename(char *pSFile, char *pDFile);

int filem_oper_copy(char *pSFile, char *pDFile, FilemCall pProCall, void *pPara);

unsigned int filem_oper_availdisk(char *pDPath);

int filem_oper_extract(char *pSFile, char *pDPath, 
                       FilemCall pProCall, void *pPara, int iSpchk);

int filem_oper_clrdir(char *pDPath);

int filem_oper_str2low(char *src, int dessz, char *dest);

int filem_oper_percent(int iVa, int iVb, int iPrec);

#endif

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

