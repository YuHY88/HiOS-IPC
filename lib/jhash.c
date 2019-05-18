/* jhash.h: Jenkins hash support.
 *
 * Copyright (C) 1996 Bob Jenkins (bob_jenkins@burtleburtle.net)
 *
 * http://burtleburtle.net/bob/hash/
 *
 * These are the credits from Bob's sources:
 *
 * lookup2.c, by Bob Jenkins, December 1996, Public Domain.
 * hash(), hash2(), hash3, and mix() are externally useful functions.
 * Routines to test the hash are included if SELF_TEST is defined.
 * You can use this free for any purpose.  It has no warranty.
 *
 * Copyright (C) 2003 David S. Miller (davem@redhat.com)
 *
 * I've modified Bob's hash to be useful in the Linux kernel, and
 * any bugs present are surely my fault.  -DaveM
 */

#include "zebra.h"
#include "jhash.h"

/* The golden ration: an arbitrary value */
#define JHASH_GOLDEN_RATIO  0x9e3779b9

/* NOTE: Arguments are modified. */
#define __jhash_mix(a, b, c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8); \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12);  \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}

/*liufy add*/
#define rot(x,k) (((x)<<(k)) ^ ((x)>>(32-(k))))

#define mix(a,b,c) \
{ \
  a -= c;  a ^= rot(c, 4);  c += b; \
  b -= a;  b ^= rot(a, 6);  a += c; \
  c -= b;  c ^= rot(b, 8);  b += a; \
  a -= c;  a ^= rot(c,16);  c += b; \
  b -= a;  b ^= rot(a,19);  a += c; \
  c -= b;  c ^= rot(b, 4);  b += a; \
}

#define final(a,b,c) \
{ \
  c ^= b; c -= rot(b,14); \
  a ^= c; a -= rot(c,11); \
  b ^= a; b -= rot(a,25); \
  c ^= b; c -= rot(b,16); \
  a ^= c; a -= rot(c,4);  \
  b ^= a; b -= rot(a,14); \
  c ^= b; c -= rot(b,24); \
}

/*--------------------------------------------------------------------
 * Function : hashword
 * Input    : k = The key, an array of u_in32_t values.
 *          : length = The length of key in u_int32_t values.
 *          : initval = The previous hash, or an arbitrary value.
 * Output   : Returns the 32bit hash value.
 * Synopsis : The function hashword() is identical to hashlittle() on 
 *            little-endian machines, and identical to hashbig() on big-endian 
 *            machines, except that the length has to be measured in uint32_ts 
 *            rather than in bytes.  hashlittle() is more complicated than 
 *            hashword() only because  hashlittle() has to dance around fitting
 *            the key bytes into registers.
 *            This works on all machines.  To be useful, it requires
 *            -- that the key be an array of uint32_t's, and
 *            -- that all your machines have the same endianness, and
 *            -- that the length be the number of uint32_t's in the key
 *-----------------------------------------------------------------*/
u_int32_t 
hashword (u_int32_t *k,
          size_t  length,                      
          u_int32_t  initval)              
{
  u_int32_t a,b,c;

  /* Set up the internal state */
  a = b = c = 0xdeadbeef + (((u_int32_t)length)<<2) + initval;

  /* handle most of the key */
  while (length > 3)
  {
    a += k[0];
    b += k[1];
    c += k[2];
    mix(a,b,c);
    length -= 3;
    k += 3;
  }

  /* handle the last 3 uint32_t's */
  switch(length)    
  { 
     case 3 : c+=k[2];
     case 2 : b+=k[1];
     case 1 : a+=k[0];
     final(a,b,c);
     case 0:     /* case 0: nothing left to add */
     break;
  }
  /*-- report the result */
  return c;
}




/* The most generic version, hashes an arbitrary sequence
 * of bytes.  No alignment or length assumptions are made about
 * the input key.
 */
u_int32_t
jhash (const void *key, u_int32_t length, u_int32_t initval)
{
  u_int32_t a, b, c, len;
  const u_int8_t *k = key;

  len = length;
  a = b = JHASH_GOLDEN_RATIO;
  c = initval;

  while (len >= 12)
    {
      a +=
        (k[0] + ((u_int32_t) k[1] << 8) + ((u_int32_t) k[2] << 16) +
         ((u_int32_t) k[3] << 24));
      b +=
        (k[4] + ((u_int32_t) k[5] << 8) + ((u_int32_t) k[6] << 16) +
         ((u_int32_t) k[7] << 24));
      c +=
        (k[8] + ((u_int32_t) k[9] << 8) + ((u_int32_t) k[10] << 16) +
         ((u_int32_t) k[11] << 24));

      __jhash_mix (a, b, c);

      k += 12;
      len -= 12;
    }

  c += length;
  switch (len)
    {
    case 11:
      c += ((u_int32_t) k[10] << 24);
    case 10:
      c += ((u_int32_t) k[9] << 16);
    case 9:
      c += ((u_int32_t) k[8] << 8);
    case 8:
      b += ((u_int32_t) k[7] << 24);
    case 7:
      b += ((u_int32_t) k[6] << 16);
    case 6:
      b += ((u_int32_t) k[5] << 8);
    case 5:
      b += k[4];
    case 4:
      a += ((u_int32_t) k[3] << 24);
    case 3:
      a += ((u_int32_t) k[2] << 16);
    case 2:
      a += ((u_int32_t) k[1] << 8);
    case 1:
      a += k[0];
    };

  __jhash_mix (a, b, c);

  return c;
}

/* A special optimized version that handles 1 or more of u_int32_ts.
 * The length parameter here is the number of u_int32_ts in the key.
 */
u_int32_t
jhash2 (const u_int32_t *k, u_int32_t length, u_int32_t initval)
{
  u_int32_t a, b, c, len;

  a = b = JHASH_GOLDEN_RATIO;
  c = initval;
  len = length;

  while (len >= 3)
    {
      a += k[0];
      b += k[1];
      c += k[2];
      __jhash_mix (a, b, c);
      k += 3;
      len -= 3;
    }

  c += length * 4;

  switch (len)
    {
    case 2:
      b += k[1];
    case 1:
      a += k[0];
    };

  __jhash_mix (a, b, c);

  return c;
}


/* A special ultra-optimized versions that knows they are hashing exactly
 * 3, 2 or 1 word(s).
 *
 * NOTE: In partilar the "c += length; __jhash_mix(a,b,c);" normally
 *       done at the end is not done here.
 */
u_int32_t
jhash_3words (u_int32_t a, u_int32_t b, u_int32_t c, u_int32_t initval)
{
  a += JHASH_GOLDEN_RATIO;
  b += JHASH_GOLDEN_RATIO;
  c += initval;

  __jhash_mix (a, b, c);

  return c;
}

u_int32_t
jhash_2words (u_int32_t a, u_int32_t b, u_int32_t initval)
{
  return jhash_3words (a, b, 0, initval);
}

u_int32_t
jhash_1word (u_int32_t a, u_int32_t initval)
{
  return jhash_3words (a, 0, 0, initval);
}
