/*
 *
 */
#ifndef _HHR_STM_H
#define _HHR_STM_H


/*hhrSdhStmBaseTable*/
#define hhrSdhStmType       1
#define hhrSdhStmAlarm      2

/*hhrSdhStmRSTable*/
#define hhrSdhStmRSJ0TRANSMIT       1
#define hhrSdhStmRSJ0EXPECTED       2
#define hhrSdhStmRSJ0RECEIVE        3
#define hhrSdhStmRSEXCTHRESHOLD     4
#define hhrSdhStmRSDEGTHRESHOLD     5
#define hhrSdhStmRSB1COUNT          6
#define hhrSdhStmRSALARM            7

/*hhrSdhStmMSTable*/
#define hhrSdhStmMSK2RECEIVE        1
#define hhrSdhStmMSEXCTHRESHOLD     2
#define hhrSdhStmMSDEGTHRESHOLD     3
#define hhrSdhStmMSB2COUNT          4
#define hhrSdhStmMSREICOUNT         5
#define hhrSdhStmMSALARM            6

/*hhrSdhStmHPTable*/
#define hhrSdhStmHPVC4INDEX         1
#define hhrSdhStmHPJ1TRANSMIT       2
#define hhrSdhStmHPJ1EXPECTED       3
#define hhrSdhStmHPJ1RECEIVE        4
#define hhrSdhStmHPC2TRANSMIT       5
#define hhrSdhStmHPC2RECEIVE        6
#define hhrSdhStmHPG1RECEIVE        7
#define hhrSdhStmHPEXCTHRESHOLD     8
#define hhrSdhStmHPDEGTHRESHOLD     9
#define hhrSdhStmHPB3COUNT          10
#define hhrSdhStmHPREICOUNT         11
#define hhrSdhStmHPPLUCOUNT         12
#define hhrSdhStmHPNGACOUNT         13
#define hhrSdhStmHPALARM            14

/*hhrSdhStmMSPTable*/
#define hhrSdhStmMSPGROUPINDEX              1
#define hhrSdhStmMSPGROUPMODE               2
#define hhrSdhStmMSPGROUPWTR                3
#define hhrSdhStmMSPGROUPMASTERIFINDEX      4
#define hhrSdhStmMSPGROUPBACKUPIFINDEX      5
#define hhrSdhStmMSPGROUPSTATUS             6

/*hhrSdhStmSNCPTable*/
#define  hhrSdhStmSNCPGROUPNAME               1
#define  hhrSdhStmSNCPGROUPMODE               2
#define  hhrSdhStmSNCPGROUPLEVEL              3
#define  hhrSdhStmSNCPGROUPSRCIFINDEX         4
#define  hhrSdhStmSNCPGROUPSRCVC4             5
#define  hhrSdhStmSNCPGROUPSRCVC3             6
#define  hhrSdhStmSNCPGROUPSRCVC12            7
#define  hhrSdhStmSNCPGROUPSRCBACKUPIFINDEX   8
#define  hhrSdhStmSNCPGROUPSRCBACKUPVC4       9
#define  hhrSdhStmSNCPGROUPSRCBACKUPVC3       10
#define  hhrSdhStmSNCPGROUPSRCBACKUPVC12      11
#define  hhrSdhStmSNCPGROUPDSTIFINDEX         12
#define  hhrSdhStmSNCPGROUPDSTVC4             13
#define  hhrSdhStmSNCPGROUPDSTVC3             14
#define  hhrSdhStmSNCPGROUPDSTVC12            15
#define  hhrSdhStmSNCPGROUPDSTBACKUPIFINDEX   16
#define  hhrSdhStmSNCPGROUPDSTBACKUPVC4       17
#define  hhrSdhStmSNCPGROUPDSTBACKUPVC3       18
#define  hhrSdhStmSNCPGROUPDSTBACKUPVC12      19
#define  hhrSdhStmSNCPGROUPWTRMODE            20
#define  hhrSdhStmSNCPGROUPWTR                21
#define  hhrSdhStmSNCPGROUPSTATUS             22
#define  hhrSdhStmSNCPGROUPNUM                23

/*
 * init_xxx(void)
 */
void init_hhr_tdm(void);

#endif                          /* _HHR_STM_H */
