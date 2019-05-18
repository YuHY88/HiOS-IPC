/*
 * $Id: radiusclient.h,v 3.0 2003/08/18 15:34:16 jhb Exp $
 *
 * Copyright (C) 1995,1996,1997,1998 Lars Fenneberg
 *
 * Copyright 1992 Livingston Enterprises, Inc.
 *
 * Copyright 1992,1993, 1994,1995 The Regents of the University of Michigan 
 * and Merit Network, Inc. All Rights Reserved
 *
 * See the file COPYRIGHT for the respective terms and conditions. 
 * If the file is missing contact me at lf@elemental.net 
 * and I'll send you a copy.
 *
 */

#ifndef RADIUSCLIENT_H
#define RADIUSCLIENT_H

#include	<sys/types.h>
#include	<stdio.h>
#include	<time.h>



//#define AUTH_VECTOR_LEN		16
#define AUTH_PASS_LEN		    (3 * 16) /* multiple of 16 */
//#define AUTH_ID_LEN		    64
#define AUTH_STRING_LEN		    253	 /* maximum of 253 */

#define	BUFFER_LEN		        8192

#define NAME_LENGTH		        32
#define	GETSTR_LENGTH	        128	/* must be bigger than AUTH_PASS_LEN */


#define SERVER_MAX              8

#define AUTH_LOCAL_FST	        (1<<0)
#define AUTH_RADIUS_FST         (1<<1)
#define AUTH_LOCAL_SND          (1<<2)
#define AUTH_RADIUS_SND         (1<<3)

#define CHAP_VALUE_LENGTH		16

/* standard RADIUS codes */

#define	PW_ACCESS_REQUEST		1
#define	PW_ACCESS_ACCEPT		2
#define	PW_ACCESS_REJECT		3
#define	PW_ACCOUNTING_REQUEST	4
#define	PW_ACCOUNTING_RESPONSE	5
#define	PW_ACCOUNTING_STATUS	6
#define	PW_PASSWORD_REQUEST		7
#define	PW_PASSWORD_ACK			8
#define	PW_PASSWORD_REJECT		9
#define	PW_ACCOUNTING_MESSAGE	10
#define	PW_ACCESS_CHALLENGE		11
#define	PW_STATUS_SERVER		12
#define	PW_STATUS_CLIENT		13


/* standard RADIUS attribute-value pairs */

#define	PW_USER_NAME			    1	/* string */
#define	PW_USER_PASSWORD		    2	/* string */
#define	PW_CHAP_PASSWORD		    3	/* string */
#define	PW_NAS_IP_ADDRESS		    4	/* ipaddr */
#define	PW_NAS_PORT			        5	/* integer */
#define	PW_SERVICE_TYPE			    6	/* integer */
#define	PW_FRAMED_PROTOCOL		    7	/* integer */
#define	PW_FRAMED_IP_ADDRESS	    8	/* ipaddr */
#define	PW_FRAMED_IP_NETMASK	    9	/* ipaddr */
#define	PW_FRAMED_ROUTING		    10	/* integer */
#define	PW_FILTER_ID		        11	/* string */
#define	PW_FRAMED_MTU			    12	/* integer */
#define	PW_FRAMED_COMPRESSION	    13	/* integer */
#define	PW_LOGIN_IP_HOST		    14	/* ipaddr */
#define	PW_LOGIN_SERVICE		    15	/* integer */
#define	PW_LOGIN_PORT			    16	/* integer */
#define	PW_OLD_PASSWORD			    17	/* string */ /* deprecated */
#define	PW_REPLY_MESSAGE		    18	/* string */
#define	PW_LOGIN_CALLBACK_NUMBER	19	/* string */
#define	PW_FRAMED_CALLBACK_ID		20	/* string */
#define	PW_EXPIRATION			    21	/* date */ /* deprecated */
#define	PW_FRAMED_ROUTE			    22	/* string */
#define	PW_FRAMED_IPX_NETWORK	    23	/* integer */
#define	PW_STATE			        24	/* string */
#define	PW_CLASS			        25	/* string */
#define	PW_VENDOR_SPECIFIC		    26	/* string */
#define	PW_SESSION_TIMEOUT		    27	/* integer */
#define	PW_IDLE_TIMEOUT			    28	/* integer */
#define	PW_TERMINATION_ACTION	    29	/* integer */
#define	PW_CALLED_STATION_ID        30  /* string */
#define	PW_CALLING_STATION_ID       31  /* string */
#define	PW_NAS_IDENTIFIER		    32	/* string */
#define	PW_PROXY_STATE			    33	/* string */
#define	PW_LOGIN_LAT_SERVICE	    34	/* string */
#define	PW_LOGIN_LAT_NODE		    35	/* string */
#define	PW_LOGIN_LAT_GROUP		    36	/* string */
#define	PW_FRAMED_APPLETALK_LINK	37	/* integer */
#define	PW_FRAMED_APPLETALK_NETWORK	38	/* integer */
#define	PW_FRAMED_APPLETALK_ZONE	39	/* string */
#define	PW_CHAP_CHALLENGE           60  /* string */
#define	PW_NAS_PORT_TYPE            61  /* integer */
#define	PW_PORT_LIMIT               62  /* integer */
#define PW_LOGIN_LAT_PORT           63  /* string */
#define PW_EAP_MESSAGE				79	/* string */
#define PW_MESSAGE_AUTHENTICATOR	80	/* string */
#define PW_ACCT_INTERIM_INTERVAL    85  /*string  */  
#define PW_NAS_PORT_ID			    87	/* string */

#define PW_SESSION_EQOTA		    100 /* string */

#define PW_BANDWIDTH_LIMIT		    64	/* integer */
/*	Accounting */

#define	PW_ACCT_STATUS_TYPE		    40	/* integer */
#define	PW_ACCT_DELAY_TIME		    41	/* integer */
#define	PW_ACCT_INPUT_OCTETS	    42	/* integer */
#define	PW_ACCT_OUTPUT_OCTETS	    43	/* integer */
#define	PW_ACCT_SESSION_ID		    44	/* string */
#define	PW_ACCT_AUTHENTIC		    45	/* integer */
#define	PW_ACCT_SESSION_TIME		46	/* integer */
#define	PW_ACCT_INPUT_PACKETS		47	/* integer */
#define	PW_ACCT_OUTPUT_PACKETS		48	/* integer */
#define PW_ACCT_TERMINATE_CAUSE		49	/* integer */
#define PW_ACCT_MULTI_SESSION_ID	50	/* string */
#define PW_ACCT_LINK_COUNT		    51	/* integer */
#define PW_ACCT_INPUT_GIGWORDS		52  /* integer */
#define PW_ACCT_OUTPUT_GIGWORDS		53	/* integer */

/*	Merit Experimental Extensions */

#define PW_USER_ID                  222 /* string */
#define PW_USER_REALM               223 /* string */

/* Huahuan private attribute */
#define HH_EXEC_PRIVILEGE			29

/*	Integer Translations */

/*	SERVICE TYPES	*/

#define	PW_LOGIN			    1
#define	PW_FRAMED			    2
#define	PW_CALLBACK_LOGIN		3
#define	PW_CALLBACK_FRAMED		4
#define	PW_OUTBOUND			    5
#define	PW_ADMINISTRATIVE		6
#define PW_NAS_PROMPT           7
#define PW_AUTHENTICATE_ONLY	8
#define PW_CALLBACK_NAS_PROMPT  9

/*	FRAMED PROTOCOLS	*/

#define	PW_PPP				    1
#define	PW_SLIP				    2
#define PW_ARA                  3
#define PW_GANDALF              4
#define PW_XYLOGICS             5

/*	FRAMED ROUTING VALUES	*/

#define	PW_NONE				    0
#define	PW_BROADCAST			1
#define	PW_LISTEN			    2
#define	PW_BROADCAST_LISTEN		3

/*	FRAMED COMPRESSION TYPES	*/

#define	PW_VAN_JACOBSON_TCP_IP		1
#define	PW_IPX_HEADER_COMPRESSION	2

/*	LOGIN SERVICES	*/

#define PW_TELNET                   0
#define PW_RLOGIN                   1
#define PW_TCP_CLEAR                2
#define PW_PORTMASTER               3
#define PW_LAT                      4
#define PW_X25_PAD                  5
#define PW_X25_T3POS                6

/*	TERMINATION ACTIONS	*/

#define	PW_DEFAULT			    0
#define	PW_RADIUS_REQUEST		1

/*	PROHIBIT PROTOCOL  */

#define PW_DUMB		            0	/* 1 and 2 are defined in FRAMED PROTOCOLS */
#define PW_AUTH_ONLY	        3
#define PW_ALL		            255

/*	ACCOUNTING STATUS TYPES    */

#define PW_STATUS_START		    1
#define PW_STATUS_STOP		    2
#define PW_STATUS_ALIVE		    3
#define PW_STATUS_MODEM_START	4
#define PW_STATUS_MODEM_STOP	5
#define PW_STATUS_CANCEL	    6
#define PW_ACCOUNTING_ON	    7
#define PW_ACCOUNTING_OFF	    8

/*      ACCOUNTING TERMINATION CAUSES   */

#define PW_USER_REQUEST         1
#define PW_LOST_CARRIER         2
#define PW_LOST_SERVICE         3
#define PW_ACCT_IDLE_TIMEOUT    4
#define PW_ACCT_SESSION_TIMEOUT 5
#define PW_ADMIN_RESET          6
#define PW_ADMIN_REBOOT         7
#define PW_PORT_ERROR           8
#define PW_NAS_ERROR            9
#define PW_NAS_REQUEST          10
#define PW_NAS_REBOOT           11
#define PW_PORT_UNNEEDED        12
#define PW_PORT_PREEMPTED       13
#define PW_PORT_SUSPENDED       14
#define PW_SERVICE_UNAVAILABLE  15
#define PW_CALLBACK             16
#define PW_USER_ERROR           17
#define PW_HOST_REQUEST         18
 
/*     NAS PORT TYPES    */

#define PW_ASYNC		        0
#define PW_SYNC			        1
#define PW_ISDN_SYNC		    2
#define PW_ISDN_SYNC_V120	    3
#define PW_ISDN_SYNC_V110	    4
#define PW_VIRTUAL		        5

/*	   AUTHENTIC TYPES */
#define PW_RADIUS	1
#define PW_LOCAL	2
#define PW_REMOTE	3



#endif /* RADIUSCLIENT_H */
