/*
*测试MIB使用，包括15个标量，8个表
*/

#include <lib/types.h>
#include "testDataStructure.h"

//#include "subAgent/alarmPortMonitorCfgTable/alarmPortMonitorCfgTable.h"
#include "gpnAlmApi.h"




//testAlmPortMoniCfg
unsigned long	testAlmPortMoniCfg[3][7] = {{1,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,11,12},
											{2,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,21,22},
											{3,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,21,32}};

//testAlmPortMoniCfg
unsigned long	testAlmAttribCfg[3][14] = {{1,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,11,12,13,14,15,16,17,18},
										   {2,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,21,22,23,24,25,26,27,28},
										   {3,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,21,32,33,34,35,36,37,38}};

//testEventDB
unsigned long	testEventDB[3][11] = {{1,11,12,13,14,15,16,17,18,19,10},
									  {2,21,22,23,24,25,26,27,28,29,20},
									  {3,21,32,33,34,35,36,37,38,39,30}};

//test

long testPortMonitorData[3][7] = {{1,12,13,14,15,16,17},
								  {2,22,23,24,25,26,27},
								  {3,32,33,34,35,36,37}};

UINT32 testCurrAlmDB[3][12] = {{1,12,13,14,15,16,17,16,15,14,13,10},
							   {2,22,23,24,25,26,27,26,25,24,23,15},
							   {3,32,33,34,35,36,37,36,35,34,33,20}};



unsigned char  testCurrAlmSuffix[256] = "123456789abcdefghijklmnopqrstuvwxyz123456789abcdefghijklmnopqrstuvwxyz123456789abcdefghijklmnopqrstuvwxyz123456789abcdefghijklmnopqrstuvwxyz"; 

unsigned char  test1[256] = "123456789abcdefghijklmnopqrstuvwxyz123456789abcdefghijklmnopqrstuvwxyz123456789abcdefghijklmnopqrstuvwxyz";
unsigned char  test2[256] = "defighlajlfkjadjflajdf;ljsldjf;la";



//---------------------------------- portMonitorCfg --------------------------------------//

UINT32 gpnAlmSnmpApiPortMoniDBGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32 temp;
	//char  *suffix;
	UINT32	 iIndex = 0;

	printf("%s : It has run to here\n",__FUNCTION__);
		
	if(pgpnAlmMsgSpIn->iMsgType != GPN_ALM_MSG_PORT_MONI_DB_GET)
	{
		return FALSE;
	}
	
	pgpnAlmMsgSpOut->iIndex		= iIndex;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT; 
	pgpnAlmMsgSpOut->iMsgPara1 	= pgpnAlmMsgSpIn->iMsgPara1;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_GET_RSP;
	pgpnAlmMsgSpOut->msgCellLen	= 0;

	
	temp = pgpnAlmMsgSpIn->iMsgPara1;
	//suffix = (char*)(&(pgpnAlmMsgSpOut->msgCellLen)+1);
	
	printf("%s : temp == %u\n",__FUNCTION__,temp);
	
	switch(temp)
	{
		case 1:
			{
				memcpy(&(pgpnAlmMsgSpOut->iMsgPara1), &testAlmPortMoniCfg[0][0], 7*sizeof(long));
			}break;
		case 2:
			{
				memcpy(&(pgpnAlmMsgSpOut->iMsgPara1), &testAlmPortMoniCfg[1][0], 7*sizeof(long));
			}break;
		case 3:
			{
				memcpy(&(pgpnAlmMsgSpOut->iMsgPara1), &testAlmPortMoniCfg[2][0], 7*sizeof(long));
			}break;
		default:break;
	}
	return GPN_ALM_API_OK;
}

UINT32 gpnAlmSnmpApiPortMoniDBGetNext(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32	iIndex = 0;
	UINT32	scanIndex;

	printf("%s : It has run here ---------++++++++********++++++++--------\n",__FUNCTION__);
		
	if(pgpnAlmMsgSpIn->iMsgType != GPN_ALM_MSG_PORT_MONI_DB_GET_NEXT)
	{
		return FALSE;
	}
	scanIndex = pgpnAlmMsgSpIn->iMsgPara1;
	
	pgpnAlmMsgSpOut->iIndex		= iIndex;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT; 
	//pgpnAlmMsgSpOut->iMsgPara1 	= scanIndex;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_GET_NEXT_RSP;
	pgpnAlmMsgSpOut->msgCellLen	= 0;
	
	switch(scanIndex)
	{
		case 1:
			memcpy(&(pgpnAlmMsgSpOut->iMsgPara1), &(pgpnAlmMsgSpIn->iMsgPara1), 5*sizeof(long));
			memcpy(&(pgpnAlmMsgSpOut->iMsgPara6), &testAlmPortMoniCfg[1][0], 5*sizeof(long));
			break;
		case 2:
			memcpy(&(pgpnAlmMsgSpOut->iMsgPara1), &(pgpnAlmMsgSpIn->iMsgPara1), 5*sizeof(long));
			memcpy(&(pgpnAlmMsgSpOut->iMsgPara6), &testAlmPortMoniCfg[2][0], 5*sizeof(long));
			break;
		case 3:
			return GPN_ALM_API_ERR;
			break;
		case 0xFFFFFFFF:
			memcpy(&(pgpnAlmMsgSpOut->iMsgPara1), &(pgpnAlmMsgSpIn->iMsgPara1), 5*sizeof(long));
			memcpy(&(pgpnAlmMsgSpOut->iMsgPara6), &testAlmPortMoniCfg[0][0], 5*sizeof(long));
			break;
		default:
			break;
	}

	printf("\n%s : pgpnAlmMsgSpOut->iMsgPara1 == %d\n",__FUNCTION__,pgpnAlmMsgSpOut->iMsgPara1);
	printf("%s : pgpnAlmMsgSpOut->iMsgPara2 == %d\n",__FUNCTION__,pgpnAlmMsgSpOut->iMsgPara2);
	printf("%s : pgpnAlmMsgSpOut->iMsgPara3 == %d\n",__FUNCTION__,pgpnAlmMsgSpOut->iMsgPara3);
	printf("%s : pgpnAlmMsgSpOut->iMsgPara4 == %d\n",__FUNCTION__,pgpnAlmMsgSpOut->iMsgPara4);
	printf("%s : pgpnAlmMsgSpOut->iMsgPara5 == %d\n",__FUNCTION__,pgpnAlmMsgSpOut->iMsgPara5);
	printf("%s : pgpnAlmMsgSpOut->iMsgPara6 == %d\n",__FUNCTION__,pgpnAlmMsgSpOut->iMsgPara6);
	printf("%s : pgpnAlmMsgSpOut->iMsgPara7 == %d\n",__FUNCTION__,pgpnAlmMsgSpOut->iMsgPara7);
	printf("%s : pgpnAlmMsgSpOut->iMsgPara8 == %d\n",__FUNCTION__,pgpnAlmMsgSpOut->iMsgPara8);
	printf("%s : pgpnAlmMsgSpOut->iMsgPara9 == %d\n",__FUNCTION__,pgpnAlmMsgSpOut->iMsgPara9);
	printf("%s : pgpnAlmMsgSpOut->iMsgParaA == %d\n\n",__FUNCTION__,pgpnAlmMsgSpOut->iMsgParaA);
	return GPN_ALM_API_OK;
}

//---------------------------------- AttributeCfg --------------------------------------//

UINT32 gpnAlmSnmpApiAttribDBGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32 temp;
	UINT32	 iIndex = 0;

	printf("%s : It has run to here\n",__FUNCTION__);
	printf("%s : iMsgPara1 == %u\n",__FUNCTION__,pgpnAlmMsgSpIn->iMsgPara1);
		
	if(pgpnAlmMsgSpIn->iMsgType != GPN_ALM_MSG_ATTRIB_DB_GET)
	{
		return FALSE;
	}
	
	pgpnAlmMsgSpOut->iIndex		= iIndex;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT; 
	pgpnAlmMsgSpOut->iMsgPara1 	= pgpnAlmMsgSpIn->iMsgPara1;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_ATTRIB_DB_GET_RSP;
	pgpnAlmMsgSpOut->msgCellLen	= 0;

	
	temp = pgpnAlmMsgSpIn->iMsgPara1;

	if(0xFFFFFFFF == temp)
	{
		printf("%s : Wrong index == %u\n",__FUNCTION__,temp);
		return GPN_ALM_API_ERR;
	}
	printf("%s : temp == %u\n",__FUNCTION__,temp);
	
	switch(temp)
	{
		case 1:
			{
				memcpy(&(pgpnAlmMsgSpOut->iMsgPara1), &testAlmAttribCfg[0][0], 14*sizeof(long));
			}break;
		case 2:
			{
				memcpy(&(pgpnAlmMsgSpOut->iMsgPara1), &testAlmAttribCfg[1][0], 14*sizeof(long));
			}break;
		case 3:
			{
				memcpy(&(pgpnAlmMsgSpOut->iMsgPara1), &testAlmAttribCfg[2][0], 14*sizeof(long));
			}break;
		default:break;
	}
	return GPN_ALM_API_OK;
}

UINT32 gpnAlmSnmpApiAttribDBGetNext(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32	iIndex = 0;
	UINT32	scanIndex;
	printf("%s : It has run here ---------++++++++********++++++++--------\n",__FUNCTION__);	
	if(pgpnAlmMsgSpIn->iMsgType != GPN_ALM_MSG_ATTRIB_DB_GET_NEXT)
	{
		return FALSE;
	}
	scanIndex = pgpnAlmMsgSpIn->iMsgPara1;
	
	pgpnAlmMsgSpOut->iIndex		= iIndex;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT; 
	//pgpnAlmMsgSpOut->iMsgPara1 	= scanIndex;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_ATTRIB_DB_GET_NEXT_RSP;
	pgpnAlmMsgSpOut->msgCellLen	= 0;
	
	switch(scanIndex)
	{
		case 1:
			memcpy(&(pgpnAlmMsgSpOut->iMsgPara1), &(pgpnAlmMsgSpIn->iMsgPara1), 6*sizeof(long));
			memcpy(&(pgpnAlmMsgSpOut->iMsgPara7), &testAlmAttribCfg[1][0], 6*sizeof(long));
			break;
		case 2:
			memcpy(&(pgpnAlmMsgSpOut->iMsgPara1), &(pgpnAlmMsgSpIn->iMsgPara1), 6*sizeof(long));
			memcpy(&(pgpnAlmMsgSpOut->iMsgPara7), &testAlmAttribCfg[2][0], 6*sizeof(long));
			break;
		case 3:
			return GPN_ALM_API_ERR;
			break;
		case 0xFFFFFFFF:
			memcpy(&(pgpnAlmMsgSpOut->iMsgPara1), &(pgpnAlmMsgSpIn->iMsgPara1), 6*sizeof(long));
			memcpy(&(pgpnAlmMsgSpOut->iMsgPara7), &testAlmAttribCfg[0][0], 6*sizeof(long));
			break;
		default:
			break;
	}
	return GPN_ALM_API_OK;
}


//----------------------------------currAlmDB--------------------------------------//

UINT32 gpnAlmSnmpApiCurrAlmDBGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32 temp;
	char  *suffix;
	UINT32	 iIndex = 0;

	printf("%s : It has run to here\n",__FUNCTION__);
	printf("%s : iMsgPara1 == %u\n",__FUNCTION__,pgpnAlmMsgSpIn->iMsgPara1);
		
	if(pgpnAlmMsgSpIn->iMsgType != GPN_ALM_MSG_CURR_ALM_DB_GET)
	{
		return FALSE;
	}
	
	pgpnAlmMsgSpOut->iIndex		= iIndex;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT; 
	pgpnAlmMsgSpOut->iMsgPara1 	= pgpnAlmMsgSpIn->iMsgPara1;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_CURR_ALM_DB_GET_RSP;
	pgpnAlmMsgSpOut->msgCellLen	= 0;

	
	temp = pgpnAlmMsgSpIn->iMsgPara1;
	suffix = (char*)(&(pgpnAlmMsgSpOut->msgCellLen)+1);

	if(0xFFFFFFFF == temp)
	{
		printf("%s : Wrong index == %u\n",__FUNCTION__,temp);
		return GPN_ALM_API_ERR;
	}
	printf("%s : temp == %u\n",__FUNCTION__,temp);
	
	switch(temp)
	{
		case 1:
			{
				memcpy(&(pgpnAlmMsgSpOut->iMsgPara1),&testCurrAlmDB[0][0],12*sizeof(long));
				memcpy(suffix, testCurrAlmSuffix, testCurrAlmDB[0][11]);
			}break;
		case 2:
			{
				memcpy(&(pgpnAlmMsgSpOut->iMsgPara1), &testCurrAlmDB[1][0],12*sizeof(long));
				memcpy(suffix, testCurrAlmSuffix, testCurrAlmDB[1][11]);
			}break;
		case 3:
			{
				memcpy(&(pgpnAlmMsgSpOut->iMsgPara1), &testCurrAlmDB[2][0],12*sizeof(long));
				memcpy(suffix, testCurrAlmSuffix, testCurrAlmDB[2][11]);
			}break;
		default:break;
	}
	return GPN_ALM_API_OK;
}

UINT32 gpnAlmSnmpApiCurrAlmDBGetNext(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32	iIndex = 0;
	UINT32	scanIndex;
	printf("%s : It has run here ---------++++++++********++++++++--------\n",__FUNCTION__);	
	if(pgpnAlmMsgSpIn->iMsgType != GPN_ALM_MSG_CURR_ALM_DB_GET_NEXT)
	{
		return FALSE;
	}
	scanIndex = pgpnAlmMsgSpIn->iMsgPara1;
	
	pgpnAlmMsgSpOut->iIndex		= iIndex;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT; 
	pgpnAlmMsgSpOut->iMsgPara1 	= scanIndex;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_CURR_ALM_DB_GET_NEXT_RSP;
	pgpnAlmMsgSpOut->msgCellLen	= 0;
	
	switch(scanIndex)
	{
		case 1:
			pgpnAlmMsgSpOut->iMsgPara2 = testCurrAlmDB[1][0];
			break;
		case 2:
			pgpnAlmMsgSpOut->iMsgPara2 = testCurrAlmDB[2][0];
			break;
		case 3:
			return GPN_ALM_API_ERR;
			break;
		case 0xFFFFFFFF:
			pgpnAlmMsgSpOut->iMsgPara2 = testCurrAlmDB[0][0];
			break;
		default:
			break;
	}
	return GPN_ALM_API_OK;
}



UINT32 gpnAlmSnmpApiCurrAlmDBModify(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32	 iIndex = 0;
	UINT32	index,column;
	
	pgpnAlmMsgSpOut->iIndex		= iIndex;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_CURR_ALM_DB_MODIFY_RSP;
	pgpnAlmMsgSpOut->msgCellLen	= 0;

	if(pgpnAlmMsgSpIn->iMsgType != GPN_ALM_MSG_CURR_ALM_DB_MODIFY)
	{
		return FALSE;
	}
	index = pgpnAlmMsgSpIn->iMsgPara1;
	column = pgpnAlmMsgSpIn->iMsgPara2;
	
	testCurrAlmDB[index-1][column-1] = pgpnAlmMsgSpIn->iMsgPara3;
	
	return GPN_ALM_API_OK;
}

//----------------------------------histAlmDB--------------------------------------//

UINT32 gpnAlmSnmpApiHistAlmDBGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32 temp;
	char  *suffix;
	UINT32	 iIndex = 0;

	printf("%s : It has run to here\n",__FUNCTION__);
	printf("%s : iMsgPara1 == %u\n",__FUNCTION__,pgpnAlmMsgSpIn->iMsgPara1);
		
	if(pgpnAlmMsgSpIn->iMsgType != GPN_ALM_MSG_HIST_ALM_DB_GET)
	{
		return FALSE;
	}
	
	pgpnAlmMsgSpOut->iIndex		= iIndex;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT; 
	pgpnAlmMsgSpOut->iMsgPara1 	= pgpnAlmMsgSpIn->iMsgPara1;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_HIST_ALM_DB_GET_RSP;
	pgpnAlmMsgSpOut->msgCellLen	= 0;

	
	temp = pgpnAlmMsgSpIn->iMsgPara1;
	suffix = (char*)(&(pgpnAlmMsgSpOut->msgCellLen)+1);

	if(0xFFFFFFFF == temp)
	{
		printf("%s : Wrong index == %u\n",__FUNCTION__,temp);
		return GPN_ALM_API_ERR;
	}
	printf("%s : temp == %u\n",__FUNCTION__,temp);
	
	switch(temp)
	{
		case 1:
			{
				memcpy(&(pgpnAlmMsgSpOut->iMsgPara1),&testCurrAlmDB[0][0],12*sizeof(long));
				memcpy(suffix, testCurrAlmSuffix, testCurrAlmDB[0][11]);
			}break;
		case 2:
			{
				memcpy(&(pgpnAlmMsgSpOut->iMsgPara1), &testCurrAlmDB[1][0],12*sizeof(long));
				memcpy(suffix, testCurrAlmSuffix, testCurrAlmDB[1][11]);
			}break;
		case 3:
			{
				memcpy(&(pgpnAlmMsgSpOut->iMsgPara1), &testCurrAlmDB[2][0],12*sizeof(long));
				memcpy(suffix, testCurrAlmSuffix, testCurrAlmDB[2][11]);
			}break;
		default:break;
	}
	return GPN_ALM_API_OK;
}

UINT32 gpnAlmSnmpApiHistAlmDBGetNext(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32	iIndex = 0;
	UINT32	scanIndex;
	printf("%s : It has run here ---------++++++++********++++++++--------\n",__FUNCTION__);	
	if(pgpnAlmMsgSpIn->iMsgType != GPN_ALM_MSG_HIST_ALM_DB_GET_NEXT)
	{
		return FALSE;
	}
	scanIndex = pgpnAlmMsgSpIn->iMsgPara1;
	
	pgpnAlmMsgSpOut->iIndex		= iIndex;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT; 
	pgpnAlmMsgSpOut->iMsgPara1 	= scanIndex;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_HIST_ALM_DB_GET_NEXT_RSP;
	pgpnAlmMsgSpOut->msgCellLen	= 0;
	
	switch(scanIndex)
	{
		case 1:
			pgpnAlmMsgSpOut->iMsgPara2 = testCurrAlmDB[1][0];
			break;
		case 2:
			pgpnAlmMsgSpOut->iMsgPara2 = testCurrAlmDB[2][0];
			break;
		case 3:
			return GPN_ALM_API_ERR;
			break;
		case 0xFFFFFFFF:
			pgpnAlmMsgSpOut->iMsgPara2 = testCurrAlmDB[0][0];
			break;
		default:
			break;
	}
	return GPN_ALM_API_OK;
}



UINT32 gpnAlmSnmpApiHistAlmDBModify(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32	iIndex = 0;
	UINT32	index,column;
	
	pgpnAlmMsgSpOut->iIndex		= iIndex;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_HIST_ALM_DB_MODIFY_RSP;
	pgpnAlmMsgSpOut->msgCellLen	= 0;

	if(pgpnAlmMsgSpIn->iMsgType != GPN_ALM_MSG_HIST_ALM_DB_MODIFY)
	{
		return FALSE;
	}
	index = pgpnAlmMsgSpIn->iMsgPara1;
	column = pgpnAlmMsgSpIn->iMsgPara2;
	
	testCurrAlmDB[index-1][column-1] = pgpnAlmMsgSpIn->iMsgPara3;
	
	return GPN_ALM_API_OK;
}

//----------------------------------eventDB--------------------------------------//

UINT32 gpnAlmSnmpApiEventDBGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32 temp;
	char  *suffix;
	UINT32	 iIndex = 0;

	printf("%s : It has run to here\n",__FUNCTION__);
	printf("%s : iMsgPara1 == %u\n",__FUNCTION__,pgpnAlmMsgSpIn->iMsgPara1);
		
	if(pgpnAlmMsgSpIn->iMsgType != GPN_ALM_MSG_EVT_DATA_DB_GET)
	{
		return FALSE;
	}
	
	pgpnAlmMsgSpOut->iIndex		= iIndex;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT; 
	pgpnAlmMsgSpOut->iMsgPara1 	= pgpnAlmMsgSpIn->iMsgPara1;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_EVT_DATA_DB_GET_RSP;
	pgpnAlmMsgSpOut->msgCellLen	= 0;

	
	temp = pgpnAlmMsgSpIn->iMsgPara1;
	suffix = (char*)(&(pgpnAlmMsgSpOut->msgCellLen)+1);

	if(0xFFFFFFFF == temp)
	{
		printf("%s : Wrong index == %u\n",__FUNCTION__,temp);
		return GPN_ALM_API_ERR;
	}
	printf("%s : temp == %d\n",__FUNCTION__,temp);
	
	switch(temp)
	{
		case 1:
			{
				memcpy(&(pgpnAlmMsgSpOut->iMsgPara1),&testEventDB[0][0],11*sizeof(long));
				memcpy(suffix, testCurrAlmSuffix, testEventDB[0][10]);
			}break;
		case 2:
			{
				memcpy(&(pgpnAlmMsgSpOut->iMsgPara1), &testEventDB[1][0],11*sizeof(long));
				memcpy(suffix, testCurrAlmSuffix, testEventDB[1][10]);
			}break;
		case 3:
			{
				memcpy(&(pgpnAlmMsgSpOut->iMsgPara1), &testEventDB[2][0],11*sizeof(long));
				memcpy(suffix, testCurrAlmSuffix, testEventDB[2][10]);
			}break;
		default:break;
	}
	return GPN_ALM_API_OK;
}

UINT32 gpnAlmSnmpApiEventDBGetNext(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32	iIndex = 0;
	UINT32	scanIndex;
	printf("%s : It has run here ---------++++++++********++++++++--------\n",__FUNCTION__);	
	if(pgpnAlmMsgSpIn->iMsgType != GPN_ALM_MSG_EVT_DATA_DB_GET_NEXT)
	{
		return FALSE;
	}
	scanIndex = pgpnAlmMsgSpIn->iMsgPara1;
	
	pgpnAlmMsgSpOut->iIndex		= iIndex;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT; 
	pgpnAlmMsgSpOut->iMsgPara1 	= scanIndex;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_EVT_DATA_DB_GET_NEXT_RSP;
	pgpnAlmMsgSpOut->msgCellLen	= 0;
	
	switch(scanIndex)
	{
		case 1:
			pgpnAlmMsgSpOut->iMsgPara2 = testEventDB[1][0];
			break;
		case 2:
			pgpnAlmMsgSpOut->iMsgPara2 = testEventDB[2][0];
			break;
		case 3:
			return GPN_ALM_API_ERR;
			break;
		case 0xFFFFFFFF:
			pgpnAlmMsgSpOut->iMsgPara2 = testEventDB[0][0];
			break;
		default:
			break;
	}
	return GPN_ALM_API_OK;
}


//----------------------------------  debugCurrAlmDB  --------------------------------------//

UINT32 gpnAlmSnmpApiDebugCurrAlmDBGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32 temp;
	char  *suffix;
	UINT32	 iIndex = 0;

	if(pgpnAlmMsgSpIn->iMsgType != GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET)
	{
		return FALSE;
	}
	
	pgpnAlmMsgSpOut->iIndex		= iIndex;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT; 
	pgpnAlmMsgSpOut->iMsgPara1 	= pgpnAlmMsgSpIn->iMsgPara1;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET_RSP;
	pgpnAlmMsgSpOut->msgCellLen	= 0;
	
	temp = pgpnAlmMsgSpIn->iMsgPara1;
	suffix = (char*)(&(pgpnAlmMsgSpOut->msgCellLen) + 1);

	if(0xFFFFFFFF == temp)
	{
		printf("%s : Wrong index == %d\n",__FUNCTION__,temp);
		return GPN_ALM_API_ERR;
	}
	
	switch(temp)
	{
		case 1:
			{
				pgpnAlmMsgSpOut->iMsgPara1 = 1;
				pgpnAlmMsgSpOut->iMsgPara2 = 1;
				pgpnAlmMsgSpOut->iMsgPara3 = 2;
				pgpnAlmMsgSpOut->iMsgPara4 = 3;
				pgpnAlmMsgSpOut->iMsgPara5 = 4;
				pgpnAlmMsgSpOut->iMsgPara6 = 5;
				pgpnAlmMsgSpOut->iMsgPara7 = 6;
				pgpnAlmMsgSpOut->iMsgPara8 = 5;
				pgpnAlmMsgSpOut->iMsgPara9 = 4;
				pgpnAlmMsgSpOut->iMsgParaA = 3;
				pgpnAlmMsgSpOut->iMsgParaB = 2;				
				pgpnAlmMsgSpOut->iMsgParaC = 1;
				strcpy(suffix, (char*)test1);
			}break;
		case 2:
			{
				pgpnAlmMsgSpOut->iMsgPara1 = 2;
				pgpnAlmMsgSpOut->iMsgPara2 = 6;
				pgpnAlmMsgSpOut->iMsgPara3 = 5;
				pgpnAlmMsgSpOut->iMsgPara4 = 4;
				pgpnAlmMsgSpOut->iMsgPara5 = 3;
				pgpnAlmMsgSpOut->iMsgPara6 = 2;
				pgpnAlmMsgSpOut->iMsgPara7 = 1;
				pgpnAlmMsgSpOut->iMsgPara8 = 2;
				pgpnAlmMsgSpOut->iMsgPara9 = 3;
				pgpnAlmMsgSpOut->iMsgParaA = 4;
				pgpnAlmMsgSpOut->iMsgParaB = 5;
				pgpnAlmMsgSpOut->iMsgParaC = 6;
				strcpy(suffix, (char*)test1);
			}break;
		case 3:
			{
				pgpnAlmMsgSpOut->iMsgPara1 = 3;
				pgpnAlmMsgSpOut->iMsgPara2 = 3;
				pgpnAlmMsgSpOut->iMsgPara3 = 2;
				pgpnAlmMsgSpOut->iMsgPara4 = 1;
				pgpnAlmMsgSpOut->iMsgPara5 = 2;
				pgpnAlmMsgSpOut->iMsgPara6 = 3;
				pgpnAlmMsgSpOut->iMsgPara7 = 4;
				pgpnAlmMsgSpOut->iMsgPara8 = 5;
				pgpnAlmMsgSpOut->iMsgPara9 = 6;
				pgpnAlmMsgSpOut->iMsgParaA = 7;
				pgpnAlmMsgSpOut->iMsgParaB = 8;
				pgpnAlmMsgSpOut->iMsgParaC = 9;
				strcpy(suffix, (char*)test1);
			}break;
		default:break;
	}
	return GPN_ALM_API_OK;
}

UINT32 gpnAlmSnmpApiDebugCurrAlmDBGetNext(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32	iIndex = 0;
	UINT32	scanIndex;
	if(pgpnAlmMsgSpIn->iMsgType != GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET_NEXT)
	{
		return GPN_ALM_API_ERR;
	}
	scanIndex = pgpnAlmMsgSpIn->iMsgPara1;
	
	pgpnAlmMsgSpOut->iIndex		= iIndex;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT; 
	//pgpnAlmMsgSpOut->iMsgPara1 	= scanIndex;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET_NEXT_RSP;
	pgpnAlmMsgSpOut->msgCellLen	= 0;

	printf("%s[%d] : index == %d\n", __FUNCTION__, __LINE__, scanIndex);
	switch(scanIndex)
	{
		case 1:
			pgpnAlmMsgSpOut->iMsgPara1 = testCurrAlmDB[1][0];
			break;
		case 2:
			pgpnAlmMsgSpOut->iMsgPara1 = testCurrAlmDB[2][0];
			break;
		case 3:
			return GPN_ALM_API_ERR;
			break;
		case 0xFFFFFFFF:
			pgpnAlmMsgSpOut->iMsgPara1 = testCurrAlmDB[0][0];
			break;
		default:
			break;
	}
	return GPN_ALM_API_OK;
}


UINT32 gpnAlmSnmpApiDebugCurrAlmDBModify(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32	iIndex = 0;
	//UINT32	index,column;
	char*	pPosition;
	UINT32	length;
	
	if(pgpnAlmMsgSpIn->iMsgType != GPN_ALM_MSG_DEBUG_CURR_ALM_DB_MODIFY)
	{
		return FALSE;
	}
	
	pgpnAlmMsgSpOut->iIndex		= iIndex;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_DEBUG_CURR_ALM_DB_MODIFY_RSP;
	pgpnAlmMsgSpOut->msgCellLen	= 0;
	
	//index = pgpnAlmMsgSpIn->iMsgPara1;
	//column = pgpnAlmMsgSpIn->iMsgPara2;
	length = pgpnAlmMsgSpIn->msgCellLen;

	pPosition = (char*)(&(pgpnAlmMsgSpIn->msgCellLen) + 1);
	
	memset(test2, 0, 256);
	memcpy(test2, pPosition, length);
	
	return GPN_ALM_API_OK;
}


//----------------------------------  debugHistAlmDB  --------------------------------------//

UINT32 gpnAlmSnmpApiDebugHistAlmDBGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32 temp;
	char  *suffix;
	UINT32	 iIndex = 0;

	if(pgpnAlmMsgSpIn->iMsgType != GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET)
	{
		return FALSE;
	}
	
	pgpnAlmMsgSpOut->iIndex		= iIndex;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT; 
	pgpnAlmMsgSpOut->iMsgPara1 	= pgpnAlmMsgSpIn->iMsgPara1;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET_RSP;
	pgpnAlmMsgSpOut->msgCellLen	= 0;
	
	temp = pgpnAlmMsgSpIn->iMsgPara1;
	suffix = (char*)(&(pgpnAlmMsgSpOut->msgCellLen) + 1);

	if(0xFFFFFFFF == temp)
	{
		printf("%s : Wrong index == %u\n",__FUNCTION__,temp);
		return GPN_ALM_API_ERR;
	}
	
	switch(temp)
	{
		case 1:
			{
				pgpnAlmMsgSpOut->iMsgPara1 = 1;
				pgpnAlmMsgSpOut->iMsgPara2 = 1;
				pgpnAlmMsgSpOut->iMsgPara3 = 2;
				pgpnAlmMsgSpOut->iMsgPara4 = 3;
				pgpnAlmMsgSpOut->iMsgPara5 = 4;
				pgpnAlmMsgSpOut->iMsgPara6 = 5;
				pgpnAlmMsgSpOut->iMsgPara7 = 6;
				pgpnAlmMsgSpOut->iMsgPara8 = 5;
				pgpnAlmMsgSpOut->iMsgPara9 = 4;
				pgpnAlmMsgSpOut->iMsgParaA = 3;
				pgpnAlmMsgSpOut->iMsgParaB = 2;				
				pgpnAlmMsgSpOut->iMsgParaC = 1;
				memcpy(suffix, test1, 36);
			}break;
		case 2:
			{
				pgpnAlmMsgSpOut->iMsgPara1 = 2;
				pgpnAlmMsgSpOut->iMsgPara2 = 6;
				pgpnAlmMsgSpOut->iMsgPara3 = 5;
				pgpnAlmMsgSpOut->iMsgPara4 = 4;
				pgpnAlmMsgSpOut->iMsgPara5 = 3;
				pgpnAlmMsgSpOut->iMsgPara6 = 2;
				pgpnAlmMsgSpOut->iMsgPara7 = 1;
				pgpnAlmMsgSpOut->iMsgPara8 = 2;
				pgpnAlmMsgSpOut->iMsgPara9 = 3;
				pgpnAlmMsgSpOut->iMsgParaA = 4;
				pgpnAlmMsgSpOut->iMsgParaB = 5;
				pgpnAlmMsgSpOut->iMsgParaC = 6;
				memcpy(suffix, test1, 41);
			}break;
		case 3:
			{
				pgpnAlmMsgSpOut->iMsgPara1 = 3;
				pgpnAlmMsgSpOut->iMsgPara2 = 3;
				pgpnAlmMsgSpOut->iMsgPara3 = 2;
				pgpnAlmMsgSpOut->iMsgPara4 = 1;
				pgpnAlmMsgSpOut->iMsgPara5 = 2;
				pgpnAlmMsgSpOut->iMsgPara6 = 3;
				pgpnAlmMsgSpOut->iMsgPara7 = 4;
				pgpnAlmMsgSpOut->iMsgPara8 = 5;
				pgpnAlmMsgSpOut->iMsgPara9 = 6;
				pgpnAlmMsgSpOut->iMsgParaA = 7;
				pgpnAlmMsgSpOut->iMsgParaB = 8;
				pgpnAlmMsgSpOut->iMsgParaC = 9;
				memcpy(suffix, test1, 50);
			}break;
		default:break;
	}
	return GPN_ALM_API_OK;
}

UINT32 gpnAlmSnmpApiDebugHistAlmDBGetNext(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32	iIndex = 0;
	UINT32	scanIndex;
	printf("%s : It has run here ---------++++++++********++++++++--------\n",__FUNCTION__);	
	if(pgpnAlmMsgSpIn->iMsgType != GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET_NEXT)
	{
		return GPN_ALM_API_ERR;
	}
	scanIndex = pgpnAlmMsgSpIn->iMsgPara1;
	
	pgpnAlmMsgSpOut->iIndex		= iIndex;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT; 
	pgpnAlmMsgSpOut->iMsgPara1 	= scanIndex;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET_NEXT_RSP;
	pgpnAlmMsgSpOut->msgCellLen	= 0;
	
	switch(scanIndex)
	{
		case 1:
			pgpnAlmMsgSpOut->iMsgPara2 = testCurrAlmDB[1][0];
			break;
		case 2:
			pgpnAlmMsgSpOut->iMsgPara2 = testCurrAlmDB[2][0];
			break;
		case 3:
			return GPN_ALM_API_ERR;
			break;
		case 0xFFFFFFFF:
			pgpnAlmMsgSpOut->iMsgPara2 = testCurrAlmDB[0][0];
			break;
		default:
			break;
	}
	return GPN_ALM_API_OK;
}


UINT32 gpnAlmSnmpApiDebugHistAlmDBModify(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32	iIndex = 0;
	//UINT32	index,column;
	char*	pPosition;
	UINT32	length;
	
	if(pgpnAlmMsgSpIn->iMsgType != GPN_ALM_MSG_DEBUG_HIST_ALM_DB_MODIFY)
	{
		return FALSE;
	}
	
	pgpnAlmMsgSpOut->iIndex		= iIndex;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_DEBUG_HIST_ALM_DB_MODIFY_RSP;
	pgpnAlmMsgSpOut->msgCellLen	= 0;
	
	//index = pgpnAlmMsgSpIn->iMsgPara1;
	//column = pgpnAlmMsgSpIn->iMsgPara2;
	length = pgpnAlmMsgSpIn->msgCellLen;

	pPosition = (char*)(&(pgpnAlmMsgSpIn->msgCellLen) + 1);
	
	memset(test2, 0, 256);
	memcpy(test2, pPosition, length);
	
	return GPN_ALM_API_OK;
}


//----------------------------------debugEventDB--------------------------------------//

UINT32 gpnAlmSnmpApiDebugEventDBGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32 temp,sum_length,ret;
	char  *suffix;
	UINT32	 iIndex = 0;

	printf("%s : It has run to here\n",__FUNCTION__);
	printf("%s : iMsgPara1 == %u\n",__FUNCTION__,pgpnAlmMsgSpIn->iMsgPara1);
		
	if(pgpnAlmMsgSpIn->iMsgType != GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET)
	{
		return FALSE;
	}
	
	pgpnAlmMsgSpOut->iIndex		= iIndex;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT; 
	pgpnAlmMsgSpOut->iMsgPara1 	= pgpnAlmMsgSpIn->iMsgPara1;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET_RSP;
	pgpnAlmMsgSpOut->msgCellLen	= 0;

	
	temp = pgpnAlmMsgSpIn->iMsgPara1;
	suffix = (char*)(&(pgpnAlmMsgSpOut->msgCellLen)+1);

	if(0xFFFFFFFF == temp)
	{
		printf("%s : Wrong index == %u\n",__FUNCTION__,temp);
		return GPN_ALM_API_ERR;
	}
	printf("%s : temp == %x\n",__FUNCTION__,temp);
	
	switch(temp)
	{
		case 1:
			{
				sum_length = 0;
				for(ret = 0;ret < 10;ret++)
				{
					sum_length += testEventDB[0][ret+1];
				}
				memcpy(&(pgpnAlmMsgSpOut->iMsgPara1), &testEventDB[0][0], 11*sizeof(long));
				memcpy(suffix, testCurrAlmSuffix, sum_length);
			}break;
		case 2:
			{
				sum_length = 0;
				for(ret = 0;ret < 10;ret++)
				{
					sum_length += testEventDB[1][ret+1];
				}
				memcpy(&(pgpnAlmMsgSpOut->iMsgPara1), &testEventDB[1][0], 11*sizeof(long));
				memcpy(suffix, testCurrAlmSuffix, sum_length);
			}break;
		case 3:
			{
				sum_length = 0;
				for(ret = 0;ret < 10;ret++)
				{
					sum_length += testEventDB[2][ret+1];
				}
				memcpy(&(pgpnAlmMsgSpOut->iMsgPara1), &testEventDB[2][0], 11*sizeof(long));
				memcpy(suffix, testCurrAlmSuffix, sum_length);
			}break;
		default:break;
	}
	return GPN_ALM_API_OK;
}

UINT32 gpnAlmSnmpApiDebugEventDBGetNext(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32	iIndex = 0;
	UINT32	scanIndex;
	printf("%s : It has run here ---------++++++++********++++++++--------\n",__FUNCTION__);	
	if(pgpnAlmMsgSpIn->iMsgType != GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET_NEXT)
	{
		return FALSE;
	}
	scanIndex = pgpnAlmMsgSpIn->iMsgPara1;
	
	pgpnAlmMsgSpOut->iIndex		= iIndex;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT; 
	pgpnAlmMsgSpOut->iMsgPara1 	= scanIndex;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET_NEXT_RSP;
	pgpnAlmMsgSpOut->msgCellLen	= 0;
	printf("%s : scanIndex == %d\n",__FUNCTION__,scanIndex);
	switch(scanIndex)
	{
		case 1:
			pgpnAlmMsgSpOut->iMsgPara1 = testEventDB[1][0];
			break;
		case 2:
			pgpnAlmMsgSpOut->iMsgPara1 = testEventDB[2][0];
			break;
		case 3:
			return GPN_ALM_API_ERR;
			break;
		case 0xFFFFFFFF:
			pgpnAlmMsgSpOut->iMsgPara1 = testEventDB[0][0];
			break;
		default:
			break;
	}
	return GPN_ALM_API_OK;
}


