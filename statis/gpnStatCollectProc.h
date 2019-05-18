/**********************************************************
* file name: gpnStatCollectProc.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-04-25
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_STAT_CELLECT_PROC_H_
#define _GPN_STAT_CELLECT_PROC_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdio.h>

#include "type_def.h"
#include "stat_debug.h"

#include "socketComm/gpnGlobalPortIndexDef.h"

#define GPN_STAT_COLL_GEN_OK 					GEN_SYS_OK
#define GPN_STAT_COLL_GEN_ERR 					GEN_SYS_ERR

#define GPN_STAT_COLL_GEN_YES 					GEN_SYS_YES
#define GPN_STAT_COLL_GEN_NO					GEN_SYS_NO

#define GPN_STAT_COLL_GEN_ENABLE				GEN_SYS_ENABLE
#define GPN_STAT_COLL_GEN_DISABLE				GEN_SYS_DISABLE

#define GPN_STAT_COLL_PRINT(level, info...) 	GEN_SYS_DEBUG_PRINT((level), info)
#define GPN_STAT_COLL_EGP 						GEN_SYS_DEBUG_EGP
#define GPN_STAT_COLL_SVP 						GEN_SYS_DEBUG_SVP
#define GPN_STAT_COLL_CMP 						GEN_SYS_DEBUG_CMP
#define GPN_STAT_COLL_CUP 						GEN_SYS_DEBUG_CUP
#define GPN_STAT_COLL_UD5						GEN_SYS_DEBUG_UD5
#define GPN_STAT_COLL_UD4						GEN_SYS_DEBUG_UD4
#define GPN_STAT_COLL_UD3						GEN_SYS_DEBUG_UD3
#define GPN_STAT_COLL_UD2						GEN_SYS_DEBUG_UD2
#define GPN_STAT_COLL_UD1						GEN_SYS_DEBUG_UD1
#define GPN_STAT_COLL_UD0						GEN_SYS_DEBUG_UD0

typedef struct _stGpnStatGen64bit_
{
	UINT32 h;
	UINT32 l;
}stGpnStatGen64bit;

typedef struct _stGpnStatGen32bit_
{
	UINT32 l;
}stGpnStatGen32bit;

/*64bit C = ( A + B) */
#define GPN_STAT_ADD_64_UINT(A,B,C)                 \
	do{                                   		    \
		UINT32 miniv = (A->l < B->l)?A->l:B->l;     \
		C->l = A->l + B->l;                         \
		if( (C->l < miniv) &&                       \
			(C->l < miniv) )                        \
		{                                           \
			C->h = A->h + B->h + 1;                 \
		}                                           \
		else                                        \
		{										    \
			C->h = A->h + B->h;                     \
		}                                           \
  	}while(0);

/*32bit C = ( A + B) */    //modify by geqian        delete   (*(UINT32 *)B = *(UINT32 *)A);       for xCycCurrData is zero, then cannot riase alarm   
#define GPN_STAT_XADD_32_UNIT(A,B,C)                \
	do{                                             \
		*(UINT32 *)C += (*(UINT32 *)A - *(UINT32 *)B);\
	}while(0);

	
/*64bit C = A - B */
#define GPN_STAT_XSUN_64_UINT(A,B,C)                \
	do{ 											\
		INT32 jst;									\
		if(A->l < B->l) 							\
		{											\
			jst = 1;									\
			C->l = 0xFFFFFFFF + 1 + A->l - B->l;	  \
		}											\
		else										\
		{											\
			jst = 0;									\
			C->l = A->l - B->l; 					\
		}											\
		if((A->h - jst) < B->h) 					\
		{											\
			C->h = 0xFFFFFFFF + 1 + A->h - jst - B->h;\
		}											\
		else										\
		{											\
			C->h = A->h - B->h - jst;				\
		}											\
	}while (0);
	
/*64bit C +=( A - B) */
#define GPN_STAT_XADD_64_UINT(A,B,C)                \
	do{ 											\
		stGpnStatGen64bit tmp64;					\
		stGpnStatGen64bit *ptmp64;					\
		ptmp64 = &tmp64;							\
		GPN_STAT_XSUN_64_UINT(A,B,ptmp64);			\
		GPN_STAT_ADD_64_UINT(ptmp64, C, C); 		\
	} while (0);
		

#if 0
/*64bit C +=( A - B) */
#define GPN_STAT_XADD_64_UINT(A,B,C)                \
	do{                                             \
		stGpnStatGen64bit tmp64;                    \
		stGpnStatGen64bit *ptmp64;                  \
		ptmp64 = &tmp64;                            \
		if( (A->h < B->h) ||                   	    \
			((A->h == B->h) &&                 	    \
			 (A->l < B->l)) )                       \
		{                                           \
			/*low bit add 1,most high bit not add 1*/          \
			ptmp64->l = 0xFFFFFFFF - B->l + 1;        \
			ptmp64->h = 0xFFFFFFFF - B->h;            \
			GPN_STAT_ADD_64_UINT(A, ptmp64, ptmp64);\
			GPN_STAT_ADD_64_UINT(ptmp64, C, C);     \
		}                                           \
		else                                        \
		{										    \
			if(A->l < B->l)                  	    \
			{                                  		\
				C->l += (0xFFFFFFFF -                 \
					B->l + 1 + A->l);               \
				C->h += (A->h - 1 - B->h);          \
			}                                  		\
			else                                 	\
			{                                  		\
				C->l += (A->l - B->l);              \
				C->h += (A->h - B->h);              \
			}                                  		\
		}                                       	\
	} while (0);
#endif
	

UINT32 gpnStatDataCollectProc(optObjOrient *pPeerIndex, UINT32 statType, void *pdata, UINT32 len);
UINT32 gpnStatDataCollectBase2CurrProc(void *pbase,
	void *pcurr, void *pdata, UINT32 bitDeep, UINT32 vType, UINT32 subNum, UINT32 dataCnt);

UINT32 gpnStatDataCollectDebug(optObjOrient *pPeerIndex, UINT32 statType);
UINT32 gpnStatDataCollectBaseAndCurrPrintDebug(void *pbase,
	void *pcurr, UINT32 bitDeep, UINT32 subNum);

void gpnStatCalcCountAverage_32(UINT32 *pbase, UINT32 *pcurr, UINT32 *pdata, UINT32 dataCnt);
void gpnStatCalcMeasureAverage_32(UINT32 *pcurr, UINT32 *pdata, UINT32 dataCnt);
void gpnStatCalcCountAverage_64(stGpnStatGen64bit *pbase, stGpnStatGen64bit *pcurr, stGpnStatGen64bit *pdata, UINT32 dataCnt);
void gpnStatMeasureCountAverage_64(stGpnStatGen64bit *pbase, stGpnStatGen64bit *pcurr, stGpnStatGen64bit *pdata, UINT32 dataCnt);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_STAT_CELLECT_PROC_H_*/


