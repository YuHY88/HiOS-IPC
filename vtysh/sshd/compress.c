/* $OpenBSD: compress.c,v 1.25 2006/08/06 01:13:32 stevesk Exp $ */
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

//#include "includes.h"

#include <sys/types.h>

#include <stdarg.h>
#include <zlib.h>

#include "log.h"
#include "buffer_ssh.h"
#include "compress.h"
#include "vtysh_sshd.h"
#include "ssh_version.h"


z_stream incoming_stream;
z_stream outgoing_stream;
//static int compress_init_send_called = 0;
//static int compress_init_recv_called = 0;
//static int inflate_failed = 0;
static int deflate_failed = 0;

/*
 * Initializes compression; level is compression level from 1 to 9
 * (as in gzip).
 */

void
buffer_compress_init_send(int level, struct ssh_packet *ssh_packet)
{
	if (ssh_packet->compress_init_send_called == 1)
		deflateEnd(&ssh_packet->outgoing_stream);
	ssh_packet->compress_init_send_called = 1;
	zlog_debug(VD_SSH, "Enabling compression at level %d.", level);
	if (level < 1 || level > 9)
		fatal("Bad compression level %d.", level);
	deflateInit(&ssh_packet->outgoing_stream, level);
}
void
buffer_compress_init_recv(struct ssh_packet *ssh_packet)
{
	if (ssh_packet->compress_init_recv_called == 1)
		inflateEnd(&ssh_packet->incoming_stream);
	ssh_packet->compress_init_recv_called = 1;
	inflateInit(&ssh_packet->incoming_stream);
}

/* Frees any data structures allocated for compression. */

void
buffer_compress_uninit(struct ssh_packet *ssh_packet)
{
#if 0
	zlog_debug(VD_SSH, "compress outgoing: raw data %llu, compressed %llu, factor %.2f",
	    (unsigned long long)ssh_packet->outgoing_stream.total_in,
	    (unsigned long long)ssh_packet->outgoing_stream.total_out,
	    ssh_packet->outgoing_stream.total_in == 0 ? 0.0 :
	    (double) ssh_packet->outgoing_stream.total_out / ssh_packet->outgoing_stream.total_in);
	zlog_debug(VD_SSH, "compress incoming: raw data %llu, compressed %llu, factor %.2f",
	    (unsigned long long)ssh_packet->incoming_stream.total_out,
	    (unsigned long long)ssh_packet->incoming_stream.total_in,
	    ssh_packet->incoming_stream.total_out == 0 ? 0.0 :
	    (double) ssh_packet->incoming_stream.total_in / ssh_packet->incoming_stream.total_out);
#endif
	if (ssh_packet->compress_init_recv_called == 1 && ssh_packet->inflate_failed == 0)
		inflateEnd(&ssh_packet->incoming_stream);
	if (ssh_packet->compress_init_send_called == 1 && ssh_packet->deflate_failed == 0)
		deflateEnd(&ssh_packet->outgoing_stream);
}

/*
 * Compresses the contents of input_buffer into output_buffer.  All packets
 * compressed using this function will form a single compressed data stream;
 * however, data will be flushed at the end of every call so that each
 * output_buffer can be decompressed independently (but in the appropriate
 * order since they together form a single compression stream) by the
 * receiver.  This appends the compressed data to the output buffer.
 */

void
buffer_compress(Buffer * input_buffer, Buffer * output_buffer)
{
	u_char buf[4096];
	int status;

	/* This case is not handled below. */
	if (buffer_len(input_buffer) == 0)
		return;

	/* Input is the contents of the input buffer. */
	outgoing_stream.next_in = buffer_ptr(input_buffer);
	outgoing_stream.avail_in = buffer_len(input_buffer);

	/* Loop compressing until deflate() returns with avail_out != 0. */
	do {
		/* Set up fixed-size output buffer. */
		outgoing_stream.next_out = buf;
		outgoing_stream.avail_out = sizeof(buf);

		/* Compress as much data into the buffer as possible. */
		status = deflate(&outgoing_stream, Z_PARTIAL_FLUSH);
		switch (status) {
		case Z_OK:
			/* Append compressed data to output_buffer. */
			buffer_append(output_buffer, buf,
			    sizeof(buf) - outgoing_stream.avail_out);
			break;
		default:
			deflate_failed = 1;
			fatal("buffer_compress: deflate returned %d", status);
			/* NOTREACHED */
		}
	} while (outgoing_stream.avail_out == 0);
}

/*
 * Uncompresses the contents of input_buffer into output_buffer.  All packets
 * uncompressed using this function will form a single compressed data
 * stream; however, data will be flushed at the end of every call so that
 * each output_buffer.  This must be called for the same size units that the
 * buffer_compress was called, and in the same order that buffers compressed
 * with that.  This appends the uncompressed data to the output buffer.
 */

void
buffer_uncompress(Buffer * input_buffer, Buffer * output_buffer, struct ssh_packet *ssh_packet)
{
	u_char buf[4096];
	int status;

	ssh_packet->incoming_stream.next_in = buffer_ptr(input_buffer);
	ssh_packet->incoming_stream.avail_in = buffer_len(input_buffer);

	for (;;) {
		/* Set up fixed-size output buffer. */
		ssh_packet->incoming_stream.next_out = buf;
		ssh_packet->incoming_stream.avail_out = sizeof(buf);

		status = inflate(&ssh_packet->incoming_stream, Z_PARTIAL_FLUSH);
		switch (status) {
		case Z_OK:
			buffer_append(output_buffer, buf,
			    sizeof(buf) - ssh_packet->incoming_stream.avail_out);
			break;
		case Z_BUF_ERROR:
			/*
			 * Comments in zlib.h say that we should keep calling
			 * inflate() until we get an error.  This appears to
			 * be the error that we get.
			 */
			return;
		default:
			ssh_packet->inflate_failed = 1;
			fatal("buffer_uncompress: inflate returned %d", status);
			/* NOTREACHED */
		}
	}
}
