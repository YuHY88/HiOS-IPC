
#ifndef _MSDH_COMMON_H_
#define _MSDH_COMMON_H_

// ======================================================
// macro about msdh frame
// ======================================================
#define MSDH_DEF_VER                0x02            // current msdh protocol version

#define MSDH_MAX_FRAME_LENGTH       1096
#define MSDH_MAX_FRAME_DATA_LENGTH  1074

#define MSDH_BROADCAST              0xFFFFFFFF      // broad addr
#define MSDH_AGENTADDR              0xC0C0C0C0      // agent addr
#define S_EQP_LO_ADDR               0x7F000001      // 127.0.0.1

#define MSDH_FRM_HEAD               0x96            // The frame head of the MSDH frame 
#define MSDH_FRM_HEAD_LEN           18
#define MSDH_FRM_CHKSUM_LEN         4
#define MSDH_FRM_COST_LEN           (MSDH_FRM_HEAD_LEN + MSDH_FRM_CHKSUM_LEN)

#define MSDH_FRM_FIX_CMD_LEN        2
#define MSDH_FRM_LEN_SHORTEST       (MSDH_FRM_COST_LEN + MSDH_FRM_FIX_CMD_LEN)

#define MSDH_FRM_IND_HEAD            0
#define MSDH_FRM_IND_CTRL            1
#define MSDH_FRM_IND_DSTADDR         2
#define MSDH_FRM_IND_SRCADDR         6
#define MSDH_FRM_IND_VERCTRL        10
#define MSDH_FRM_IND_HOP            11
#define MSDH_FRM_IND_XCHGH          12
#define MSDH_FRM_IND_XCHGL          13
#define MSDH_FRM_IND_FRMNO          14
#define MSDH_FRM_IND_DATALEN        16
#define MSDH_FRM_IND_DATA           MSDH_FRM_HEAD_LEN

// ======================================================
// msdh frame ctrl
// ======================================================
#define MSDH_CTRL_CR                0x01
#define MSDH_CTRL_EF                0x02
#define MSDH_CTRL_OPT               0x04
#define MSDH_CTRL_CONT              0x08
#define MSDH_CTRL_TABS              0x10
#define MSDH_CTRL_MD                0x20
#define MSDH_CTRL_ERR               0x28

// ======================================================
// msdh frame cmd type
// ======================================================
typedef struct
{
    unsigned char       head;
    unsigned char       ctrl;
    unsigned char       dst1;
    unsigned char       dst2;
    unsigned char       dst3;
    unsigned char       dst4;
    unsigned char       src1;
    unsigned char       src2;
    unsigned char       src3;
    unsigned char       src4;
    unsigned char       ver_pt;
    unsigned char       hop;
    unsigned char       xchg_h;
    unsigned char       xchg_l;
    unsigned char       no_h;
    unsigned char       no_l;
    unsigned char       len_h;
    unsigned char       len_l;
    unsigned char      *pdata;
}   t_msdhinf;

typedef struct
{
    unsigned char       ver_pt;
    unsigned char       hop;
    unsigned char       xchg_h;
    unsigned char       xchg_l;
}   t_msdhctl;

/*****************************************************
* get frame check sum
*   param 1 : frame data
*   param 2 : frame length
*   return  :
*****************************************************/
unsigned long   msdh_checksum(unsigned char *, unsigned short);

/*****************************************************
* packet to msdh frame
*   param 1 : ctrl byte
*   param 2 : dst addr, host ip
*   param 3 : src addr, host ip
*   param 4 : frame data length
*   param 5 : frame ctrl pointer
*   param 6 : frame pointer
*   return  : frame length
*****************************************************/
unsigned short  msdh_packet(unsigned char, unsigned long, unsigned long, unsigned short, t_msdhctl *, t_msdhinf *);

/*****************************************************
* packet to msdh frame
*   param 1 : dst addr is 255.255.255.255 or 127.0.0.1
*   return  :
*       1: frame need local deal
*****************************************************/
int             msdh_frm_tome(unsigned long);

/*****************************************************
* check frame
*   param 1 : frame pointer
*   param 2 : frame length
*   return  :
*       1: frame is ok
*****************************************************/
int             msdh_frm_isok(t_msdhinf *, unsigned short);

unsigned short  msdh_frm_get_length(unsigned char *pframe);

#endif
