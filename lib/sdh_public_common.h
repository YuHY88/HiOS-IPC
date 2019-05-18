
#ifndef _SDH_PUBLIC_COMMON_H_
#define _SDH_PUBLIC_COMMON_H_

typedef enum
{
    soh_j0 = 3,
    soh_e1,
    soh_f1,
    soh_z0,
    soh_e2 = 8,
    soh_k1,
    soh_k2,
    soh_s1,
    soh_m1,
    soh_j1 = 14,
    soh_c2,
    soh_g1,
    soh_f2,
    soh_f3,
    soh_k3,
    soh_n1,
    soh_v5,
    soh_j2,
    soh_n2,
    soh_k4
}   sdh_oh_type;

#define SDH_OH_JX_LEN       16

/*
 * check pinput jx and regularization to poutput
 *   para1: input jx, 16 bytes
 *   para2: output jx, 16 bytes
 * return :
 *       0: pinput valid, regularization to poutput
 *      -1: pinput jx error, poutput EQUAL 16bytes 0x00
 *      -2: pinput or poutput is NULL
 */
extern int get_valid_jx(unsigned char *pinput, unsigned char *poutput);

#endif

