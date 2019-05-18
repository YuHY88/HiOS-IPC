/* tacplus.h
 *
 * Copyright (C) 2010, Pawel Krawczyk <pawel.krawczyk@hush.com> and
 * Jeroen Nijhof <jeroen@jeroennijhof.nl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program - see the file COPYING.
 *
 * See `CHANGES' file for revision history.
 *
 * modified by luoyz @ 2016.9.27
 */

#ifndef _TACPLUS_H
#define _TACPLUS_H

/* https://tools.ietf.org/html/draft-grant-tacacs-02#page-5 */
#define TAC_PLUS_MAJOR_VER_MASK 0xf0
#define TAC_PLUS_MAJOR_VER      0xc0

#define TAC_PLUS_MINOR_VER_0 0x00
#define TAC_PLUS_VER_0  (TAC_PLUS_MAJOR_VER | TAC_PLUS_MINOR_VER_0)

#define TAC_PLUS_MINOR_VER_1 0x01
#define TAC_PLUS_VER_1  (TAC_PLUS_MAJOR_VER | TAC_PLUS_MINOR_VER_1)

#define TAC_PLUS_AUTHEN 0x01
#define TAC_PLUS_AUTHOR	0x02
#define TAC_PLUS_ACCT   0x03

#define TAC_PLUS_ENCRYPTED_FLAG      0x00    /* packet is encrypted */
#define TAC_PLUS_UNENCRYPTED_FLAG    0x01    /* packet is unencrypted */
#define TAC_PLUS_SINGLE_CONNECT_FLAG 0x04    /* multiplexing supported */

/*
 * All tacacs+ packets have the same header format
 *
 *  1 2 3 4 5 6 7 8  1 2 3 4 5 6 7 8  1 2 3 4 5 6 7 8  1 2 3 4 5 6 7 8
 * +----------------+----------------+----------------+----------------+
 * |major  | minor  |                |                |                |
 * |version| version|      type      |     seq_no     |   flags        |
 * +----------------+----------------+----------------+----------------+
 * |                                                                   |
 * |                            session_id                             |
 * +----------------+----------------+----------------+----------------+
 * |                                                                   |
 * |                              length                               |
 * +----------------+----------------+----------------+----------------+
 */
struct tac_plus_pak_hdr {
    char version;
    char type;
    char seq_no;        /* packet sequence number */
    char encryption;    /* packet is encrypted or cleartext */
    int session_id;       /* session identifier FIXME: Is this needed? */
    int datalength;       /* length of encrypted data following this
                            header datalength bytes of encrypted data.
                            Packets are never padded beyond this length. */
};

#define TAC_PLUS_HDR_SIZE 12

typedef struct tac_plus_pak_hdr TAC_HDR;

/***************************** authentication ********************************/

/* https://tools.ietf.org/html/draft-grant-tacacs-02#page-11 */
#define TAC_PLUS_AUTHEN_LOGIN    0x01
#define TAC_PLUS_AUTHEN_CHPASS   0x02
#define TAC_PLUS_AUTHEN_SENDPASS 0x03 /* deprecated */
#define TAC_PLUS_AUTHEN_SENDAUTH 0x04

#define TAC_PLUS_PRIV_LVL_MIN  0x00
#define TAC_PLUS_PRIV_LVL_MAX  0x0f
#define TAC_PLUS_PRIV_LVL_USER 0x01
#define TAC_PLUS_PRIV_LVL_ROOT 0x0f

#define TAC_PLUS_AUTHEN_TYPE_ASCII  0x01
#define TAC_PLUS_AUTHEN_TYPE_PAP    0x02 /* Password Authentication Protocol */
#define TAC_PLUS_AUTHEN_TYPE_CHAP   0x03 /* Challenge Handshake Authentication
											Protocol */
#define TAC_PLUS_AUTHEN_TYPE_ARAP   0x04 /* AppleTalk Remote Access Protocol ? */
#define TAC_PLUS_AUTHEN_TYPE_MSCHAP 0x05 /* Microsoft Challenge Handshake
											Authentication Protocol */

#define TAC_PLUS_AUTHEN_SVC_NONE    0x00
#define TAC_PLUS_AUTHEN_SVC_LOGIN   0x01
#define TAC_PLUS_AUTHEN_SVC_ENABLE  0x02
#define TAC_PLUS_AUTHEN_SVC_PPP     0x03
#define TAC_PLUS_AUTHEN_SVC_ARAP    0x04
#define TAC_PLUS_AUTHEN_SVC_PT      0x05
#define TAC_PLUS_AUTHEN_SVC_RCMD    0x06
#define TAC_PLUS_AUTHEN_SVC_X25     0x07
#define TAC_PLUS_AUTHEN_SVC_NASI    0x08
#define TAC_PLUS_AUTHEN_SVC_FWPROXY 0x09

/* Authentication packet we send to tacacs+ server */
struct authen_start {
    char action;
    char priv_lvl;
    char authen_type;
    char service;

    char user_len;
    char port_len;
    char rem_addr_len;
    char data_len;

};

#define TAC_AUTHEN_START_FIXED_FIELDS_SIZE 8

/* https://tools.ietf.org/html/draft-grant-tacacs-02#page-15 */
/* Authentication continue packet we send to tacacs+ server */
struct authen_cont {
    u_int16 user_msg_len;
    u_int16 user_data_len;
    char flags;
};
#define TAC_PLUS_CONTINUE_FLAG_ABORT 0x01

#define TAC_AUTHEN_CONT_FIXED_FIELDS_SIZE 5


/* https://tools.ietf.org/html/draft-grant-tacacs-02#page-14 */
#define TAC_PLUS_AUTHEN_STATUS_PASS    0x01
#define TAC_PLUS_AUTHEN_STATUS_FAIL    0x02
#define TAC_PLUS_AUTHEN_STATUS_GETDATA 0x03
#define TAC_PLUS_AUTHEN_STATUS_GETUSER 0x04
#define TAC_PLUS_AUTHEN_STATUS_GETPASS 0x05
#define TAC_PLUS_AUTHEN_STATUS_RESTART 0x06
#define TAC_PLUS_AUTHEN_STATUS_ERROR   0x07
#define TAC_PLUS_AUTHEN_STATUS_FOLLOW  0x21

#define TAC_PLUS_AUTHEN_FLAG_NOECHO    0x01

/* Authentication reply packet server send to us */
struct authen_reply {
    char status;
    char flags;
    u_int16 msg_len;
    u_int16 data_len;
};

#define TAC_AUTHEN_REPLY_FIXED_FIELDS_SIZE 6


/* https://tools.ietf.org/html/draft-grant-tacacs-02#page-25 */
#define TAC_PLUS_AUTHEN_METH_NOT_SET    0x00
#define TAC_PLUS_AUTHEN_METH_NONE       0x01
#define TAC_PLUS_AUTHEN_METH_KRB5       0x02
#define TAC_PLUS_AUTHEN_METH_LINE       0x03
#define TAC_PLUS_AUTHEN_METH_ENABLE     0x04
#define TAC_PLUS_AUTHEN_METH_LOCAL      0x05
#define TAC_PLUS_AUTHEN_METH_TACACSPLUS 0x06
#define TAC_PLUS_AUTHEN_METH_GUEST      0x08
#define TAC_PLUS_AUTHEN_METH_RADIUS     0x10
#define TAC_PLUS_AUTHEN_METH_KRB4       0x11
#define TAC_PLUS_AUTHEN_METH_RCMD       0x20

#define AUTHEN_METH_NONE       TAC_PLUS_AUTHEN_METH_NONE
#define AUTHEN_METH_KRB5       TAC_PLUS_AUTHEN_METH_KRB5
#define AUTHEN_METH_LINE       TAC_PLUS_AUTHEN_METH_LINE
#define AUTHEN_METH_ENABLE     TAC_PLUS_AUTHEN_METH_ENABLE
#define AUTHEN_METH_LOCAL      TAC_PLUS_AUTHEN_METH_LOCAL
#define AUTHEN_METH_TACACSPLUS TAC_PLUS_AUTHEN_METH_TACACSPLUS
#define AUTHEN_METH_RCMD       TAC_PLUS_AUTHEN_METH_RCMD

/***************************** authorization ********************************/

/* An authorization request packet */
struct author {
    char authen_method;
    char priv_lvl;
    char authen_type;
    char service;

    char user_len;
    char port_len;
    char r_addr_len;
    char arg_cnt;    /* the number of args */

};

#define TAC_AUTHOR_REQ_FIXED_FIELDS_SIZE 8

/* An authorization reply packet */

#define TAC_PLUS_AUTHOR_STATUS_PASS_ADD  0x01
#define TAC_PLUS_AUTHOR_STATUS_PASS_REPL 0x02
#define TAC_PLUS_AUTHOR_STATUS_FAIL      0x10
#define TAC_PLUS_AUTHOR_STATUS_ERROR     0x11
#define TAC_PLUS_AUTHOR_STATUS_FOLLOW    0x21

#define AUTHOR_STATUS_PASS_ADD       TAC_PLUS_AUTHOR_STATUS_PASS_ADD
#define AUTHOR_STATUS_PASS_REPL      TAC_PLUS_AUTHOR_STATUS_PASS_REPL
#define AUTHOR_STATUS_FAIL           TAC_PLUS_AUTHOR_STATUS_FAIL
#define AUTHOR_STATUS_ERROR          TAC_PLUS_AUTHOR_STATUS_ERROR
#define AUTHOR_STATUS_FOLLOW         TAC_PLUS_AUTHOR_STATUS_FOLLOW

struct author_reply {
    char status;
    char arg_cnt;
    u_int16 msg_len;
    u_int16 data_len;
};

#define TAC_AUTHOR_REPLY_FIXED_FIELDS_SIZE 6

/***************************** account ********************************/
#define TAC_PLUS_ACCT_FLAG_MORE     0x01
#define TAC_PLUS_ACCT_FLAG_START    0x02
#define TAC_PLUS_ACCT_FLAG_STOP     0x04
#define TAC_PLUS_ACCT_FLAG_WATCHDOG 0x08

struct acct {
    char flags;
    char authen_method;
    char priv_lvl;
    char authen_type;
    char authen_service;
    char user_len;
    char port_len;
    char r_addr_len;
    char arg_cnt;    /* the number of cmd args */

};

#define TAC_ACCT_REQ_FIXED_FIELDS_SIZE 9

#define TAC_PLUS_ACCT_STATUS_SUCCESS 0x1
#define TAC_PLUS_ACCT_STATUS_ERROR   0x2
#define TAC_PLUS_ACCT_STATUS_FOLLOW  0x21

struct acct_reply {
    u_int16 msg_len;
    u_int16 data_len;
    char status;
};

#define TAC_ACCT_REPLY_FIXED_FIELDS_SIZE 5

#endif
