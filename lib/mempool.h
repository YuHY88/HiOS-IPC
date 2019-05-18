/*====================================================================================================================*/
/*-------------------------------------------------------------*/
/*  FileName : mempool.h                                       */
/*  Author   : suxiqing                                        */
/*  EditTime : 2011-10-25                                      */
/*  CopyRight:                                                 */
/*-------------------------------------------------------------*/
/*====================================================================================================================*/
/*====================================================================================================================*/
#ifndef _MEM_POOL_H_
#define _MEM_POOL_H_

#define MEM_POOL_BLOCK_SIZE_MIN   8       /*块的最小长度          */

#define MEM_POOL_BUCKET_COUN_MIN  1       /*最少桶数            */

#define MEM_POOL_BUCKET_COUN_MAX  20      /*最大桶数 不严格要求           */

#define MEM_POOL_BUCKET_SIZE_MIN  512     /*桶的最小长度          */

#define MEM_POOL_BUCKET_OBJS_MAX  1024    /*每桶最大块数限制*/

/*====================================================================================================================*/
/*====================================================================================================================*/
typedef unsigned long  mempool_id;

/*====================================================================================================================*/
/*====================================================================================================================*/
/*-----------------------------------------------------------*/
/*Name: alarm_memp_destroy                                   */
/*Retu: void                                                 */
/*Desc: 销毁专用内存池                                              */
/*-----------------------------------------------------------*/
void mem_pool_destroy(mempool_id  mempid);

/*-----------------------------------------------------------*/
/*Name: alarm_memp_malloc                                    */
/*Retu: void*                                                */
/*Desc: 从指定池中申请一个块                                           */
/*-----------------------------------------------------------*/
void* mem_pool_malloc(mempool_id  mempid);

/*-----------------------------------------------------------*/
/*Name: alarm_memp_free                                      */
/*Retu: void                                                 */
/*Desc: 释放一块                                                 */
/*-----------------------------------------------------------*/
void* mem_pool_free(mempool_id  mempid, void *pBlock);

/*-----------------------------------------------------------*/
/*Name: alarm_memp_create                                    */
/*Para：iBucketCo 最大桶数                                        */
/*      iBucketSz 每桶大小                                       */
/*      iBlockSz  每次申请的内存块大小                                 */
/*      iFreeEna  空闲桶释放开关,没有特殊需要建议不释放 =0                     */
/*Desc: 创建专用内存池                                              */
/*-----------------------------------------------------------*/

mempool_id mem_pool_create(int iBucketCo, int iBucketSz, int iBlockSz, int iFreeEna);

/*-----------------------------------------------------------*/
/*Name: mem_pool_infor                                       */
/*Para: iBufLen > 1024 根据桶的多少确定缓存大小                          */
/*      1024+桶数*100                                          */
/*Retu: -1 error >=0 datalen                                 */
/*Desc: 显示内存池的状态信息                                           */
/*-----------------------------------------------------------*/

int mem_pool_infor(mempool_id lMempid, char *pBuffer, int iBufLen);

/*====================================================================================================================*/
/*====================================================================================================================*/
#endif
