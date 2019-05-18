/* $OpenBSD: compress.h,v 1.12 2006/03/25 22:22:43 djm Exp $ */

/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * Interface to packet compression for ssh.
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */

#ifndef COMPRESS_H
#define COMPRESS_H

#include "vtysh_sshd.h"


void	 buffer_compress_init_send(int, struct ssh_packet *);
void	 buffer_compress_init_recv(struct ssh_packet *ssh_packet);
void     buffer_compress_uninit(struct ssh_packet *ssh_packet);
void     buffer_compress(Buffer *, Buffer *);
void     buffer_uncompress(Buffer *, Buffer *, struct ssh_packet *);

#endif				/* COMPRESS_H */
