#ifndef INCLUDE_H
#define INCLUDE_H

#include <openssl/evp.h>

/* Define to 1 if you have the `SHA256_Update' function. */
#define HAVE_SHA256_UPDATE 1

/* Define to 1 if you have the `EVP_sha256' function. */
#define HAVE_EVP_SHA256 1

/* OpenSSL 0.9.8e returns cipher key len not context key len */
#if (OPENSSL_VERSION_NUMBER == 0x0090805fL)
# define EVP_CIPHER_CTX_key_length(c) ((c)->key_len)
#endif

#endif