/*file management head file
  name filem_slot.h
  edit suxq
  date 2016/05/23
  desc file data slot manage 
*/
#ifndef _FILEM_TRAN_H_

#define _FILEM_TRAN_H_
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
#if 0
struct fha_msghdr
{
	sint32     module_id;        /* module id of receiver */
	uint16_t   sender_id;        /* module id of sender */
	uint16_t   data_len;         /* IPC data length */
	enum IPC_TYPE  msg_type;	 /* IPC msg type */
	uint8_t    msg_subtype;      /* subtype of msg, app can customize if needed */
	uint8_t    opcode;           /* operation code of data */
	uint8_t    data_num;         /* number of data */
	uint8_t    priority;         /* ???¡é¦Ì?¨®??¨¨?? */
	uint8_t    unit;             /* unit o?¡ê?¨®?¨®¨²¨¦¨¨¡À???¦Ì????¡é, 0 ¡À¨ª¨º??¨°?¨² */
	uint8_t    slot;             /* slot o?¡ê?¨®?¨®¨²¡ã???¦Ì????¡é, 0 ¡À¨ª¨º?¡ã??¨² */
	uint16_t   sequence;         /* ???¡é¦Ì?D¨°o? */
    
	uint32_t   msg_index;        /* maybe ifindex?¡élsp_index?¡épw_index and so on */
                                 /* ¨¦???2?¡¤?o¨ªIPC ???¡é¨ª¡¤¨°???*/
	uint8_t    srcunit;          /* unit o?¡ê?¨®?¨®¨²¨¦¨¨¡À???¦Ì????¡é, 0 ¡À¨ª¨º??¨°?¨² */
	uint8_t    srcslot;          /* slot o?¡ê?¨®?¨®¨²¡ã???¦Ì????¡é, 0 ¡À¨ª¨º?¡ã??¨² */

    uint16_t   spare;
    sint32     result;       
};


struct fha_pkt
{
    struct fha_msghdr msghdr;
	char   msg_data[IPC_MSG_LEN1 - 8];
};
#endif
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
void filem_tran_timeout(void);

int  filem_tran_tranisbusy(void);

void filem_tran_forcestop(int iIsCb);

void filem_tran_btbreceive(struct ipc_mesg_n *pRcvPkt, int iDlen);

int  filem_tran_init(void);

int  filem_tran_putfile(int iUnit, int iSlot, char *pLocFile, 
                        char *pRemFile, FilemCall pCbfun, void *pCbPar);

int  filem_tran_getfile(int iUnit, int iSlot, char *pLocFile, 
                        char *pRemFile, FilemCall pCbfun, void *pCbPar);

int  filem_tran_delfile(int iUnit, int iSlot, char *pLocFile, 
                        char *pRemFile, FilemCall pCbfun, void *pCbPar);

#endif

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

