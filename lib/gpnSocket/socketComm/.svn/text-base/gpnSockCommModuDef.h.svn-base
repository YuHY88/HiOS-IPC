/**********************************************************
* file name: gpnSockCommModuDef.h
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-09-13
* function: 
*    define global communication Module Identify details
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_COMM_MODU_DEF_H_
#define _GPN_SOCK_COMM_MODU_DEF_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "lib/gpnSocket/socketComm/gpnSockTypeDef.h"


#define GPN_SOCK_COMM_GEN_OK 					GPN_SOCK_SYS_OK
#define GPN_SOCK_COMM_GEN_ERR 					GPN_SOCK_SYS_ERR

#define GPN_SOCK_COMM_GEN_YES 					GPN_SOCK_SYS_YES
#define GPN_SOCK_COMM_GEN_NO 					GPN_SOCK_SYS_NO

#define GPN_SOCK_COMM_GEN_ENABLE 				GPN_SOCK_SYS_ENABLE
#define GPN_SOCK_COMM_GEN_DISABLE 				GPN_SOCK_SYS_DISABLE

#define GPN_SOCK_COMM_PRINT(level, info...) 	GEN_SOCK_SYS_PRINT((level), info)
#define GPN_SOCK_COMM_AGP 						GEN_SOCK_SYS_AGP
#define GPN_SOCK_COMM_SVP 						GEN_SOCK_SYS_SVP
#define GPN_SOCK_COMM_CMP 						GEN_SOCK_SYS_CMP
#define GPN_SOCK_COMM_CUP 						GEN_SOCK_SYS_CUP

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                                                 GPN_SOCKET Communication Topology Diagram                                                                      */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Communication Topology Diagram 1 : communication in sub slot                                                                                               */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*   gpn_ft_sub_x    gpn_ft_sub_xx   ...      gpn_trs_sub_x     gpn_trs_sub_xx   ...                                                                            */
/*           \_____________|_______/                        \_________|_________/                                                                             */
/*                                    |                                                        |         ____________________________________________      */
/*                                    |                                                        |        /                                                       \                      \     */
/*   gpn_timer           gpn_ins_role_ft                              gpn_ins_role_transfer       gpn_ins_role_xx      gpn_ins_role_xxx          ...... */
/*        \_______________\________________________________\____________________/_____________________________/   */
/*                                                                 |                                                                                                                            */
/*                                                   gpn_ins_role_x(sub_slot_x)                                                                                                     */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Communication Topology Diagram 1 : communication in NM slot                                                                                               */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*   gpn_ft_sub_x    gpn_ft_sub_xx   ...      gpn_trs_sub_x     gpn_trs_sub_xx      ...                                                                            */
/*           \_____________|_______/                        \_________|_________/                                                                             */
/*                                    |                                                        |         ____________________________________________      */
/*                                    |                                                        |        /                                                       \                      \     */
/*   gpn_timer           gpn_ins_role_ft                              gpn_ins_role_transfer       gpn_ins_role_xx      gpn_ins_role_xxx          ...... */
/*        \_______________\________________________________\____________________/_____________________________/  */
/*                                                                 |                                                                                                                           */
/*                                                gpn_ins_role_x(NM_slot_a/b)                                                                                                   */
/*                                                                 |                                                                                                                            */
/*                                                     gpn_ins_role_nm_syn                                                                                                          */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                                                       PART(1) : Instance Role Define                              */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*messages which need to transfer to the other modules(self_slot or other_slot)	             */
/*instance role: server(when process-self is transfer modulel)                                             */
/*                    client(when process-self is not transfer modulel)	                                     */
#define GPN_INS_COMMM_TRANSFER         0

/*messages which used for communication between NMa and NMb	                                    */
/*instance role: server(when process-self runing in NMa)                                                   */
/*                    client(when process-self runing in NMb)                                                    */
#define GPN_INS_COMMM_NM_SYN           1

/*messages which used for communication between process and terminal shell                    */
/*instance role: server(when process-self runing as terminal-shell-module)                         */
/*		       client(when process-self is not alm-module process)                                   */
#define GPN_INS_COMMM_TERM_SHELL       2

/*messages which used for communication between ALM-module and others modules          */
/*instance role: server(when process-self is alm-module process)                                       */
/*                    client(when process-self is not alm-module process)                                   */
#define GPN_INS_COMMM_ALM              3

/*messages which used for communication between IFM-module and others modules            */
/*instance role: server(when process-self is interface-manage-module process)                     */
/*                    client(when process-self is not interface-manage-module process)                */
#define GPN_INS_COMMM_IFM              4

/*messages which used for communication between ZebOs-SDM-module and others modules	*/
/*instance role: server(when process-self is ZebOs-SDM-module process)                              */
/*                    client(when process-self is not ZebOs-SDM-module process)                          */
#define GPN_INS_COMMM_SDM              5

/*messages which used for communication between ZebOs-NSM-module and others modules */
/*instance role: server(when process-self is ZebOs-NSM-module process)                             */
/*                    client(when process-self is not ZebOs-NSM-module process)                         */
#define GPN_INS_COMMM_NSM              6

/*messages which used for communication between ZebOs-ONMD-module and others modules*/
/*instance role: server(when process-self is ZebOs-ONMD-module process)                            */
/*                    client(when process-self is not ZebOs-ONMD-module process)                        */
#define GPN_INS_COMMM_ONMD             7

/*messages which used for communication between ZebOs-LACP-module and others modules*/
/*instance role: server(when process-self is ZebOs-LACP-module process)                            */
/*                    client(when process-self is not ZebOs-LACP-module process)	                        */
#define GPN_INS_COMMM_LAG              8

/*messages which used for communication between EcMax-PWE3E1-module and others modules*/
/*instance role: server(when process-self is EcMax-PWE3E1-module process)                            */
/*                    client(when process-self is EcMax-PWE3E1-module process)                             */
#define GPN_INS_COMMM_PWE              9

/*messages which used for communication between INIT(gpn process initd)-module and others modules 	*/
/*instance role: server(when process-self is  INIT(gpn process initd)-module process)                             */
/*                    client(when process-self is not  INIT(gpn process initd)-module process)	                        */
#define GPN_INS_COMMM_INIT             10

/*messages which used for communication between STAT-module and others modules              */
/*instance role: server(when process-self is STAT-module process)                                          */
/*                    client(when process-self is STAT-module process)                                           */
#define GPN_INS_COMMM_STAT             11

/*messages which used for communication between CFGMGT-module and others modules         */
/*instance role: server(when process-self is CFGMGT-module process)                                     */
/*                    client(when process-self is CFGMGT-module process)                                       */
#define GPN_INS_COMMM_CFGMGT           12

/*messages which used for communication between SYSMGT-module(V8 NM system config mgt) and others modules	*/
/*instance role: server(when process-self is SYSMGT-module process)                                                                   */
/*                    client(when process-self is SYSMGT-module process)                                                                    */
#define GPN_INS_COMMM_SYSMGT           13

/*messages which used for communication between FT-module and others modules                  */
/*instance role: server(when process-self is FT-module process)                                              */
/*                    client(when process-self is FT-module process)                                               */
#define GPN_INS_COMMM_FT               14

/*messages which used for communication between NAMD-module and others modules            */
/*instance role: server(when process-self is NAMD-module process)                                        */
/*                    client(when process-self is NAMD-module process)                                         */
#define GPN_INS_COMMM_OAMD             15

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_SYNCED           16


/*messages which used for communication between COPROC-module and others modules        */
/*instance role: server(when process-self is COPROC-module process)                                    */
/*                    client(when process-self is COPROC-module process)                                     */
#define GPN_INS_COMMM_COPROC           17

/*messages which used for communication between XLOADD-module and others modules        */
/*instance role: server(when process-self is XLOADD-module process)                                    */
/*                    client(when process-self is XLOADD-module process)                                     */
#define GPN_INS_COMMM_XLOAD            18

/*messages which used for communication between IMI-module and others modules               */
/*instance role: server(when process-self is IMI-module process)                                           */
/*                    client(when process-self is IMI-module process)                                            */
#define GPN_INS_COMMM_IMI              19


/***************** lipf add for ipran ****************/
#define GPN_INS_COMMM_MPLS             20
#define GPN_INS_COMMM_CLOCK            21
#define GPN_INS_COMMM_SYSTEM           22
#define GPN_INS_COMMM_DEVM             23
#define GPN_INS_COMMM_HAL              24
#define GPN_INS_COMMM_L2               25
#define GPN_INS_COMMM_CES              26
#define GPN_INS_COMMM_VTYSH            27
#define GPN_INS_COMMM_FTM              28

/********************** end **************************/


/*---------------------------------------------------------------------------*/
/*gpn_socket communication valid instance role ID max                                                           */
#define GPN_VALID_INS_COMM_ROLE_ID_MAX (GPN_INS_COMMM_FTM)
/*---------------------------------------------------------------------------*/


/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED02       18

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED03       19

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED04       20

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED05       21

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED06       22

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED07       23

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED08       24

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED09       25

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED10       26

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED11       27

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED12       28

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED13       29

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED14       30

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED15       31

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED16       32

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED17       33

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED18       34

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED19       35

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED20       36

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED21       37

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED22       38

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED23       39

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED24       40

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED25       41

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED26       42

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED27       43

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED28       44

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED29       45

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED30       46

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED31       47

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED32       48

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED33       49

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED34       50

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED35       51

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED36       52

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED37       53

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED38       54

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED39       55

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED40       56

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED41       57

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED42       58

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED43       59

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED44       60

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED45       61

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED46       62

/*messages which used for communication between SYNCED-module and others modules        */
/*instance role: server(when process-self is SYNCED-module process)                                    */
/*                    client(when process-self is SYNCED-module process)                                     */
#define GPN_INS_COMMM_RESERVED47       63


/*---------------------------------------------------------------------------*/
/*gpn_socket communication instance role ID max                                                                  */
#define GPN_INS_COMM_ROLE_ID_MAX       (GPN_INS_COMMM_RESERVED47)
/*---------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                                                PART(2) : Timer Role Define                                             */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*messages which used for timer infomations                                                                          */
/*TIMER role: server(when process-self needs timer function)                                                  */
/*                  client(never to be client)                                                                                   */
#define GPN_TIM_COMMM_TIMER_SEV 	   (GPN_INS_COMM_ROLE_ID_MAX+1)


/*---------------------------------------------------------------------------*/
/*gpn_socket communication real role ID max                                                                        */
/*real fd:instance fd, see about fake fd                                                                                   */
#define GPN_REA_COMM_ROLE_ID_MAX       (GPN_TIM_COMMM_TIMER_SEV)
/*---------------------------------------------------------------------------*/


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                                                PART(3) : Slot-between Assit Role  Define                          */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*---------------------------------------------------------------------------*/
/*gpn_socket communication Slot-Bettewen-Transfer role define					           */
/*slot-bettewen comm role base define									           */
#define GPN_TRS_COMMM_SBT_BASE         (GPN_REA_COMM_ROLE_ID_MAX+1)

/*messages which used for gpn_transfer's communication between NMx and subslot(n)	*/
/*slot-between-transfer role: server(do not create instance, just save socket address)       */	
/*		                           client(do not use in this mode)                                          */
#define GPN_TRS_COMMM_NM_TO_SUB1T      (GPN_TRS_COMMM_SBT_BASE)
#define GPN_TRS_COMMM_NM_TO_SUB2T      (GPN_TRS_COMMM_NM_TO_SUB1T+1)
#define GPN_TRS_COMMM_NM_TO_SUB3T      (GPN_TRS_COMMM_NM_TO_SUB2T+1)
#define GPN_TRS_COMMM_NM_TO_SUB4T      (GPN_TRS_COMMM_NM_TO_SUB3T+1)
#define GPN_TRS_COMMM_NM_TO_SUB5T      (GPN_TRS_COMMM_NM_TO_SUB4T+1)
#define GPN_TRS_COMMM_NM_TO_SUB6T      (GPN_TRS_COMMM_NM_TO_SUB5T+1)
#define GPN_TRS_COMMM_NM_TO_SUB7T      (GPN_TRS_COMMM_NM_TO_SUB6T+1)
#define GPN_TRS_COMMM_NM_TO_SUB8T      (GPN_TRS_COMMM_NM_TO_SUB7T+1)
#define GPN_TRS_COMMM_NM_TO_SUB9T      (GPN_TRS_COMMM_NM_TO_SUB8T+1)
#define GPN_TRS_COMMM_NM_TO_SUB10T     (GPN_TRS_COMMM_NM_TO_SUB9T+1)
#define GPN_TRS_COMMM_NM_TO_SUB11T     (GPN_TRS_COMMM_NM_TO_SUB10T+1)
#define GPN_TRS_COMMM_NM_TO_SUB12T     (GPN_TRS_COMMM_NM_TO_SUB11T+1)
#define GPN_TRS_COMMM_NM_TO_SUB13T     (GPN_TRS_COMMM_NM_TO_SUB12T+1)
#define GPN_TRS_COMMM_NM_TO_SUB14T     (GPN_TRS_COMMM_NM_TO_SUB13T+1)
#define GPN_TRS_COMMM_NM_TO_SUB15T     (GPN_TRS_COMMM_NM_TO_SUB14T+1)
#define GPN_TRS_COMMM_NM_TO_SUB16T     (GPN_TRS_COMMM_NM_TO_SUB15T+1)
#define GPN_TRS_COMMM_NM_TO_SUB17T     (GPN_TRS_COMMM_NM_TO_SUB16T+1)
#define GPN_TRS_COMMM_NM_TO_SUB18T     (GPN_TRS_COMMM_NM_TO_SUB17T+1)
#define GPN_TRS_COMMM_NM_TO_SUB19T     (GPN_TRS_COMMM_NM_TO_SUB18T+1)
#define GPN_TRS_COMMM_NM_TO_SUB20T     (GPN_TRS_COMMM_NM_TO_SUB19T+1)

/*---------------------------------------------------------------------------*/
/*gpn_socket communication Slot-Bettewen-Transfer role role ID max                                      */
/*Slot-Bettewen-Transfer fd : fake fd, see about fake fd                                                                                   */
#define GPN_TRS_COMM_ROLE_ID_MAX       (GPN_TRS_COMMM_NM_TO_SUB20T)
/*---------------------------------------------------------------------------*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                                                PART(4) : Slot-between Assit Role  Define                          */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*---------------------------------------------------------------------------*/
/*gpn_socket communication slot-bettewen-file-transfer role define                                         */
/*slot-bettewen comm role base define                                                                                  */
#define GPN_FT_COMMM_SBF_BASE          (GPN_TRS_COMM_ROLE_ID_MAX+1)

/*messages which used for gpn_ft's communication between NMx and subslot(n)                       */
/*slot-bettewen-file-transfer role: server(do not create instance, just save socket address)          */	
/*		                                   client(do not use in this mode)                                            */
#define GPN_FT_COMMM_NM_TO_SUB1F       (GPN_FT_COMMM_SBF_BASE)
#define GPN_FT_COMMM_NM_TO_SUB2F       (GPN_FT_COMMM_NM_TO_SUB1F+1)
#define GPN_FT_COMMM_NM_TO_SUB3F       (GPN_FT_COMMM_NM_TO_SUB2F+1)
#define GPN_FT_COMMM_NM_TO_SUB4F       (GPN_FT_COMMM_NM_TO_SUB3F+1)
#define GPN_FT_COMMM_NM_TO_SUB5F       (GPN_FT_COMMM_NM_TO_SUB4F+1)
#define GPN_FT_COMMM_NM_TO_SUB6F       (GPN_FT_COMMM_NM_TO_SUB5F+1)
#define GPN_FT_COMMM_NM_TO_SUB7F       (GPN_FT_COMMM_NM_TO_SUB6F+1)
#define GPN_FT_COMMM_NM_TO_SUB8F       (GPN_FT_COMMM_NM_TO_SUB7F+1)
#define GPN_FT_COMMM_NM_TO_SUB9F       (GPN_FT_COMMM_NM_TO_SUB8F+1)
#define GPN_FT_COMMM_NM_TO_SUB10F      (GPN_FT_COMMM_NM_TO_SUB9F+1)
#define GPN_FT_COMMM_NM_TO_SUB11F      (GPN_FT_COMMM_NM_TO_SUB10F+1)
#define GPN_FT_COMMM_NM_TO_SUB12F      (GPN_FT_COMMM_NM_TO_SUB11F+1)
#define GPN_FT_COMMM_NM_TO_SUB13F      (GPN_FT_COMMM_NM_TO_SUB12F+1)
#define GPN_FT_COMMM_NM_TO_SUB14F      (GPN_FT_COMMM_NM_TO_SUB13F+1)
#define GPN_FT_COMMM_NM_TO_SUB15F      (GPN_FT_COMMM_NM_TO_SUB14F+1)
#define GPN_FT_COMMM_NM_TO_SUB16F      (GPN_FT_COMMM_NM_TO_SUB15F+1)
#define GPN_FT_COMMM_NM_TO_SUB17F      (GPN_FT_COMMM_NM_TO_SUB16F+1)
#define GPN_FT_COMMM_NM_TO_SUB18F      (GPN_FT_COMMM_NM_TO_SUB17F+1)
#define GPN_FT_COMMM_NM_TO_SUB19F      (GPN_FT_COMMM_NM_TO_SUB18F+1)
#define GPN_FT_COMMM_NM_TO_SUB20F      (GPN_FT_COMMM_NM_TO_SUB19F+1)

/*---------------------------------------------------------------------------*/
/*gpn_socket communication slot-bettewen-file-transfer role ID max                                       */
/*slot-bettewen-file-transfer fd : fake fd, see about fake fd                                                      */
#define GPN_FT_COMM_ROLE_ID_MAX        (GPN_FT_COMMM_NM_TO_SUB20F)
/*---------------------------------------------------------------------------*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                                                PART(5) : SYN-between-NM Role  Define                             */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*gpn_socket communication syn-bettewen-nm role define                                                       */
/*slot-bettewen comm role base define                                                                                    */
#define GPN_SYN_COMMM_SBN_BASE          (GPN_FT_COMM_ROLE_ID_MAX+1)

/*messages which used for gpn_syn's communication between NMa and NMb                              */
/*syn-bettewen-nm role: server(do not create instance, just save socket address)                        */	
/*		                      client(do not use in this mode)                                                          */
#define GPN_SYN_COMMM_SYN_TO_NMA        (GPN_SYN_COMMM_SBN_BASE)
#define GPN_SYN_COMMM_SYN_TO_NMB        (GPN_SYN_COMMM_SYN_TO_NMA+1)

/*---------------------------------------------------------------------------*/
/*gpn_socket communication syn-bettewen-nm role ID max                                                    */
/*syn-bettewen-nm fd : fake fd, see about fake fd                                                                  */
#define GPN_SYN_COMM_ROLE_ID_MAX        (GPN_SYN_COMMM_SYN_TO_NMB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                                                PART(6) : SYN-between-NM Role  Define                             */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/**/


#define GPN_COMM_ROLE_ID_MAX            (GPN_SYN_COMM_ROLE_ID_MAX)
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                                                                      END                                                            */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* redefine to standard format, used for other file                                                                      */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#define GPN_COMMM_TRANSFER             GPN_INS_COMMM_TRANSFER
#define GPN_COMMM_NM_SYN               GPN_INS_COMMM_NM_SYN
#define GPN_COMMM_TERM_SHELL           GPN_INS_COMMM_TERM_SHELL
#define GPN_COMMM_ALM                  GPN_INS_COMMM_ALM
#define GPN_COMMM_IFM                  GPN_INS_COMMM_IFM
#define GPN_COMMM_SDM                  GPN_INS_COMMM_SDM
#define GPN_COMMM_NSM                  GPN_INS_COMMM_NSM
#define GPN_COMMM_ONMD                 GPN_INS_COMMM_ONMD
#define GPN_COMMM_LAG                  GPN_INS_COMMM_LAG
#define GPN_COMMM_PWE                  GPN_INS_COMMM_PWE
#define GPN_COMMM_INIT                 GPN_INS_COMMM_INIT
#define GPN_COMMM_STAT                 GPN_INS_COMMM_STAT
#define GPN_COMMM_CFGMGT               GPN_INS_COMMM_CFGMGT
#define GPN_COMMM_SYSMGT               GPN_INS_COMMM_SYSMGT
#define GPN_COMMM_FT                   GPN_INS_COMMM_FT
#define GPN_COMMM_OAMD                 GPN_INS_COMMM_OAMD
#define GPN_COMMM_SYNCED               GPN_INS_COMMM_SYNCED
#define GPN_COMMM_COPROC               GPN_INS_COMMM_COPROC
#define GPN_COMMM_XLOAD                GPN_INS_COMMM_XLOAD
#define GPN_COMMM_IMI                  GPN_INS_COMMM_IMI

/***************** lipf add for ipran ******************************/
#define GPN_COMMM_MPLS                 GPN_INS_COMMM_MPLS
#define GPN_COMMM_CLOCK                GPN_INS_COMMM_CLOCK
#define GPN_COMMM_SYSTEM               GPN_INS_COMMM_SYSTEM
#define GPN_COMMM_DEVM                 GPN_INS_COMMM_DEVM
#define GPN_COMMM_HAL                  GPN_INS_COMMM_HAL
#define GPN_COMMM_L2                   GPN_INS_COMMM_L2
#define GPN_COMMM_CES                  GPN_INS_COMMM_CES
#define GPN_COMMM_VTYSH                GPN_INS_COMMM_VTYSH
#define GPN_COMMM_FTM                  GPN_INS_COMMM_FTM

/********************** end ***************************************/


//#define GPN_COMMM_RESERVED04           GPN_INS_COMMM_RESERVED04
//#define GPN_COMMM_RESERVED05           GPN_INS_COMMM_RESERVED05
//#define GPN_COMMM_RESERVED06           GPN_INS_COMMM_RESERVED06
//#define GPN_COMMM_RESERVED07           GPN_INS_COMMM_RESERVED07
//#define GPN_COMMM_RESERVED08           GPN_INS_COMMM_RESERVED08
//#define GPN_COMMM_RESERVED09           GPN_INS_COMMM_RESERVED09
//#define GPN_COMMM_RESERVED10           GPN_INS_COMMM_RESERVED10
//#define GPN_COMMM_RESERVED11           GPN_INS_COMMM_RESERVED11
#define GPN_COMMM_RESERVED12           GPN_INS_COMMM_RESERVED12
#define GPN_COMMM_RESERVED13           GPN_INS_COMMM_RESERVED13
#define GPN_COMMM_RESERVED14           GPN_INS_COMMM_RESERVED14
#define GPN_COMMM_RESERVED15           GPN_INS_COMMM_RESERVED15
#define GPN_COMMM_RESERVED16           GPN_INS_COMMM_RESERVED16
#define GPN_COMMM_RESERVED17           GPN_INS_COMMM_RESERVED17
#define GPN_COMMM_RESERVED18           GPN_INS_COMMM_RESERVED18
#define GPN_COMMM_RESERVED19           GPN_INS_COMMM_RESERVED19
#define GPN_COMMM_RESERVED20           GPN_INS_COMMM_RESERVED20
#define GPN_COMMM_RESERVED21           GPN_INS_COMMM_RESERVED21
#define GPN_COMMM_RESERVED22           GPN_INS_COMMM_RESERVED22
#define GPN_COMMM_RESERVED23           GPN_INS_COMMM_RESERVED23
#define GPN_COMMM_RESERVED24           GPN_INS_COMMM_RESERVED24
#define GPN_COMMM_RESERVED25           GPN_INS_COMMM_RESERVED25
#define GPN_COMMM_RESERVED26           GPN_INS_COMMM_RESERVED26
#define GPN_COMMM_RESERVED27           GPN_INS_COMMM_RESERVED27
#define GPN_COMMM_RESERVED28           GPN_INS_COMMM_RESERVED28
#define GPN_COMMM_RESERVED29           GPN_INS_COMMM_RESERVED29
#define GPN_COMMM_RESERVED30           GPN_INS_COMMM_RESERVED30
#define GPN_COMMM_RESERVED31           GPN_INS_COMMM_RESERVED31
#define GPN_COMMM_RESERVED32           GPN_INS_COMMM_RESERVED32
#define GPN_COMMM_RESERVED33           GPN_INS_COMMM_RESERVED33
#define GPN_COMMM_RESERVED34           GPN_INS_COMMM_RESERVED34
#define GPN_COMMM_RESERVED35           GPN_INS_COMMM_RESERVED35
#define GPN_COMMM_RESERVED36           GPN_INS_COMMM_RESERVED36
#define GPN_COMMM_RESERVED37           GPN_INS_COMMM_RESERVED37
#define GPN_COMMM_RESERVED38           GPN_INS_COMMM_RESERVED38
#define GPN_COMMM_RESERVED39           GPN_INS_COMMM_RESERVED39
#define GPN_COMMM_RESERVED40           GPN_INS_COMMM_RESERVED40
#define GPN_COMMM_RESERVED41           GPN_INS_COMMM_RESERVED41
#define GPN_COMMM_RESERVED42           GPN_INS_COMMM_RESERVED42
#define GPN_COMMM_RESERVED43           GPN_INS_COMMM_RESERVED43
#define GPN_COMMM_RESERVED44           GPN_INS_COMMM_RESERVED44
#define GPN_COMMM_RESERVED45           GPN_INS_COMMM_RESERVED45
#define GPN_COMMM_RESERVED46           GPN_INS_COMMM_RESERVED46
#define GPN_COMMM_RESERVED47           GPN_INS_COMMM_RESERVED47

#define GPN_COMMM_TIMER_SEV            GPN_TIM_COMMM_TIMER_SEV

#define GPN_COMMM_NM_TO_SUB1T          GPN_TRS_COMMM_NM_TO_SUB1T
#define GPN_COMMM_NM_TO_SUB2T          GPN_TRS_COMMM_NM_TO_SUB2T
#define GPN_COMMM_NM_TO_SUB3T          GPN_TRS_COMMM_NM_TO_SUB3T
#define GPN_COMMM_NM_TO_SUB4T          GPN_TRS_COMMM_NM_TO_SUB4T
#define GPN_COMMM_NM_TO_SUB5T          GPN_TRS_COMMM_NM_TO_SUB5T
#define GPN_COMMM_NM_TO_SUB6T          GPN_TRS_COMMM_NM_TO_SUB6T
#define GPN_COMMM_NM_TO_SUB7T          GPN_TRS_COMMM_NM_TO_SUB7T
#define GPN_COMMM_NM_TO_SUB8T          GPN_TRS_COMMM_NM_TO_SUB8T
#define GPN_COMMM_NM_TO_SUB9T          GPN_TRS_COMMM_NM_TO_SUB9T
#define GPN_COMMM_NM_TO_SUB10T         GPN_TRS_COMMM_NM_TO_SUB10T
#define GPN_COMMM_NM_TO_SUB11T         GPN_TRS_COMMM_NM_TO_SUB11T
#define GPN_COMMM_NM_TO_SUB12T         GPN_TRS_COMMM_NM_TO_SUB12T
#define GPN_COMMM_NM_TO_SUB13T         GPN_TRS_COMMM_NM_TO_SUB13T
#define GPN_COMMM_NM_TO_SUB14T         GPN_TRS_COMMM_NM_TO_SUB14T
#define GPN_COMMM_NM_TO_SUB15T         GPN_TRS_COMMM_NM_TO_SUB15T
#define GPN_COMMM_NM_TO_SUB16T         GPN_TRS_COMMM_NM_TO_SUB16T
#define GPN_COMMM_NM_TO_SUB17T         GPN_TRS_COMMM_NM_TO_SUB17T
#define GPN_COMMM_NM_TO_SUB18T         GPN_TRS_COMMM_NM_TO_SUB18T
#define GPN_COMMM_NM_TO_SUB19T         GPN_TRS_COMMM_NM_TO_SUB19T
#define GPN_COMMM_NM_TO_SUB20T         GPN_TRS_COMMM_NM_TO_SUB20T

#define GPN_COMMM_NM_TO_SUB1F          GPN_FT_COMMM_NM_TO_SUB1F
#define GPN_COMMM_NM_TO_SUB2F          GPN_FT_COMMM_NM_TO_SUB2F
#define GPN_COMMM_NM_TO_SUB3F          GPN_FT_COMMM_NM_TO_SUB3F
#define GPN_COMMM_NM_TO_SUB4F          GPN_FT_COMMM_NM_TO_SUB4F
#define GPN_COMMM_NM_TO_SUB5F          GPN_FT_COMMM_NM_TO_SUB5F
#define GPN_COMMM_NM_TO_SUB6F          GPN_FT_COMMM_NM_TO_SUB6F
#define GPN_COMMM_NM_TO_SUB7F          GPN_FT_COMMM_NM_TO_SUB7F
#define GPN_COMMM_NM_TO_SUB8F          GPN_FT_COMMM_NM_TO_SUB8F
#define GPN_COMMM_NM_TO_SUB9F          GPN_FT_COMMM_NM_TO_SUB9F
#define GPN_COMMM_NM_TO_SUB10F         GPN_FT_COMMM_NM_TO_SUB10F
#define GPN_COMMM_NM_TO_SUB11F         GPN_FT_COMMM_NM_TO_SUB11F
#define GPN_COMMM_NM_TO_SUB12F         GPN_FT_COMMM_NM_TO_SUB12F
#define GPN_COMMM_NM_TO_SUB13F         GPN_FT_COMMM_NM_TO_SUB13F
#define GPN_COMMM_NM_TO_SUB14F         GPN_FT_COMMM_NM_TO_SUB14F
#define GPN_COMMM_NM_TO_SUB15F         GPN_FT_COMMM_NM_TO_SUB15F
#define GPN_COMMM_NM_TO_SUB16F         GPN_FT_COMMM_NM_TO_SUB16F
#define GPN_COMMM_NM_TO_SUB17F         GPN_FT_COMMM_NM_TO_SUB17F
#define GPN_COMMM_NM_TO_SUB18F         GPN_FT_COMMM_NM_TO_SUB18F
#define GPN_COMMM_NM_TO_SUB19F         GPN_FT_COMMM_NM_TO_SUB19F
#define GPN_COMMM_NM_TO_SUB20F         GPN_FT_COMMM_NM_TO_SUB20F

#define GPN_COMMM_SYN_TO_NMA           GPN_SYN_COMMM_SYN_TO_NMA
#define GPN_COMMM_SYN_TO_NMB           GPN_SYN_COMMM_SYN_TO_NMB

#define GPN_COMMM_NULL                 0xFFFFFFFF

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                                     GPN_SOCKET Module used (1)                                                        */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* base 'gpn_comm_module' define 'gpn_comm_role'                                                                */
/* GPN_XXX_COMMM_XXX --> GPN_COMMM_XXX --> GPN_SOCK_ROLE_XXX                          */

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                                     GPN_SOCKET Module used (2)                                                        */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* base 'gpn_comm_module' define 'gpn_comm_msg' for communication                                    */
/* GPN_XXX_COMMM_XXX --> GPN_COMMM_XXX -->GPN_SOCK_MSG_M_XXX -->                  */
/* --> GPN_SOCK_OWNER_XXX                                                                                             */

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                                     GPN_SOCKET Module used (3)                                                        */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* base 'gpn_comm_module' define 'gpn_comm_msg_role' as msg src/dst addr                           */
/* GPN_XXX_COMMM_XXX --> GPN_COMMM_XXX -->GPN_SOCK_ROLE_XXX -->                    */
/* --> GPN_COMM_ROLE_FILED_XXX --> GPN_COMM_ROLE_XXX_ON_XX                               */


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                                     GPN_SOCKET Module used (4)                                                        */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* base 'gpn_comm_module' define 'gpn_init_proc_role' as msg src/dst addr                               */
/* GPN_XXX_COMMM_XXX --> GPN_COMMM_XXX -->GPN_INITD_PROC_XXX_MD                    */


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_SOCK_COMM_MODU_DEF_H_*/

