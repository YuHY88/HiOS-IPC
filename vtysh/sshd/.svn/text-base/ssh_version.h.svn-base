#ifndef SSH_VERSION_H
#define SSH_VERSION_H

#include "vtysh.h"

#define SSH_PROTO_UNKNOWN 0x00
#define	SSH_PROTO_1		0x01
#define	SSH_PROTO_1_PREFERRED	0x02
#define	SSH_PROTO_2		0x04

#define SSH_VERSION "Huahuan_3.2"

/* Minimum modulus size (n) for RSA keys. */
#define SSH_RSA_MINIMUM_MODULUS_SIZE	768

#define SSH_MAX_PUBKEY_BYTES		8192

#ifndef roundup
# define roundup(x, y)   ((((x)+((y)-1))/(y))*(y))
#endif

/*
 * Length of the session key in bytes.  (Specified as 256 bits in the
 * protocol.)
 */
#define SSH_SESSION_KEY_LENGTH		32

extern void fatal(const char *fmt,...);
extern void arc4random_stir(void);
extern void cleanup_exit(int i);
extern unsigned int arc4random(void);
extern long long strtonum(const char *numstr, long long minval, long long maxval,
    const char **errstrp);

extern int compat20;


#endif
