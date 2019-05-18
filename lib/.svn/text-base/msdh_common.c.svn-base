
#include <stdio.h>

#include "msdh_common.h"

/**** global variable **********************************/
static unsigned short           g_reps_frmno = 1;

unsigned long msdh_checksum(unsigned char *pframe, unsigned short length)
{
    unsigned short      i;
    unsigned long       sum = 0;

    if (NULL == pframe)
    {
        return 0;
    }

    for (i = 0; i < length; ++i)
    {
        sum += pframe[i];
    }

    return sum;
}

unsigned short msdh_frm_get_length(unsigned char *pframe)
{
    t_msdhinf   *pfrm = (t_msdhinf *)pframe;
    unsigned short  datalen;
    unsigned short  frm_len = 0;

    if (NULL == pframe)
    {
        return 0;
    }

    if (pfrm->head == MSDH_FRM_HEAD)
    {
        datalen = (((unsigned short)pframe[MSDH_FRM_IND_DATALEN]) << 8) | pframe[MSDH_FRM_IND_DATALEN + 1];
        frm_len = datalen + MSDH_FRM_COST_LEN;
    }

    return frm_len;
}

unsigned short msdh_packet(unsigned char ctrl, unsigned long dstaddr, unsigned long srcaddr, unsigned short datalen, t_msdhctl *pctrl, t_msdhinf *pframe)
{
    unsigned short       frmlen  = 0;
    unsigned long        chksum  = 0;
    unsigned char        *ptmp;

    if (NULL == pframe)
    {
        return 0;
    }

    pframe->head    = MSDH_FRM_HEAD;
    pframe->ctrl    = ctrl;
    pframe->dst1    = (unsigned char)(dstaddr >> 24);
    pframe->dst2    = (unsigned char)(dstaddr >> 16);
    pframe->dst3    = (unsigned char)(dstaddr >>  8);
    pframe->dst4    = (unsigned char)(dstaddr);
    pframe->src1    = (unsigned char)(srcaddr >> 24);
    pframe->src2    = (unsigned char)(srcaddr >> 16);
    pframe->src3    = (unsigned char)(srcaddr >>  8);
    pframe->src4    = (unsigned char)(srcaddr);

    if (pctrl == NULL)
    {
        pframe->ver_pt  = MSDH_DEF_VER;
        pframe->hop     = 0x00;
        pframe->xchg_h  = 0x00;
        pframe->xchg_l  = 0x00;
    }
    else
    {
        pframe->ver_pt  = ((pctrl->ver_pt) & 0xF0) | MSDH_DEF_VER;
        pframe->hop     = pctrl->hop;
        pframe->xchg_h  = pctrl->xchg_h;
        pframe->xchg_l  = pctrl->xchg_l;
    }

    if (g_reps_frmno == 0)
    {
        g_reps_frmno = 1;
    }

    pframe->no_h    = (unsigned char)(g_reps_frmno >> 8);
    pframe->no_l    = (unsigned char)(g_reps_frmno);
    g_reps_frmno++;

    pframe->len_h   = (unsigned char)(datalen >> 8);
    pframe->len_l   = (unsigned char)(datalen);

    ptmp            = (unsigned char *)pframe;
    frmlen          = datalen + MSDH_FRM_HEAD_LEN;
    chksum          = msdh_checksum(ptmp, frmlen);

    ptmp[frmlen++]  = (unsigned char)(chksum >> 24);
    ptmp[frmlen++]  = (unsigned char)(chksum >> 16);
    ptmp[frmlen++]  = (unsigned char)(chksum >>  8);
    ptmp[frmlen++]  = (unsigned char)(chksum);

    return frmlen;
}

int msdh_frm_tome(unsigned long dstaddr)
{
    if (dstaddr == S_EQP_LO_ADDR || dstaddr == MSDH_BROADCAST)
    {
        return 1;
    }

    return 0;
}

int msdh_frm_isok(t_msdhinf *pframe, unsigned short frmlen)
{
    unsigned long       chksum;
    unsigned long       frmsum;
    unsigned char       *ptmp;

    if (NULL == pframe)
    {
        return 0;
    }

    if (pframe->head == MSDH_FRM_HEAD)
    {
        ptmp    = (unsigned char *)pframe;
        chksum  = msdh_checksum(ptmp, frmlen - MSDH_FRM_CHKSUM_LEN);

        frmsum  = (((unsigned long)ptmp[frmlen - 4]) << 24)
                  | (((unsigned long)ptmp[frmlen - 3]) << 16)
                  | (((unsigned long)ptmp[frmlen - 2]) << 8)
                  | (((unsigned long)ptmp[frmlen - 1]));

        if (chksum == frmsum)
        {
            return 1;
        }
    }

    return 0;
}
