/* function of IP address */


#include <netinet/in.h>
#include <arpa/nameser.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <ctype.h>
#include "inet_ip.h"
#include "prefix.h"


static const u_int8_t maskbit[] = {0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff};
static uint8_t ipv6_loopback[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01}; /* ipv6 loopback 地址 */
static uint8_t ipv6_zero[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};     /* ipv6 不确定地址 */
static uint8_t ipv6_ipv4map[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x00,0x00,0x00,0x00};  /* ipv4 映射地址 */
int inet_aton(const char *cp, struct in_addr *addr);
char *inet_ntoa(struct in_addr in);

/*
 *验证主机序 IP 地址是合法的单播 IP，返回 1 为 true
*/
int inet_valid_ipv4(uint32_t ip)
{
    uint32_t addr = 0;
    unsigned int a1=0,a2=0,a3=0,a4=0;

    addr = htonl(ip);

    a1 = ((unsigned char *)&addr)[0];
    a2 = ((unsigned char *)&addr)[1];
    a3 = ((unsigned char *)&addr)[2];
    a4 = ((unsigned char *)&addr)[3];

    if(0==a1)
    {
        if(0!=addr)
        {
            return FALSE;
        }

        if (0==a2 && 0==a3 && 0==a4)
        {
            return FALSE;
        }
    }
    else if(255==a1 && 255==a2 && 255==a3 && 255==a4)
    {
        return FALSE;
    }
    else if(a1>=224 && a1<=239)
    {
        return FALSE;
    }
    else if(a1>=240 && a1<=255)
    {
        return FALSE;/*experimental address*/
    }
    else if(127 == a1)
    {
        return FALSE;
    }

    return TRUE;
}


/*
 *验证主机序 IP 地址是合法的网段地址，返回 1 为 true
*/
int inet_valid_network(uint32_t ip)
{
    uint32_t addr = 0;
    unsigned int a1=0,a2=0,a3=0,a4=0;

    addr = htonl(ip);

    a1 = ((unsigned char *)&addr)[0];
    a2 = ((unsigned char *)&addr)[1];
    a3 = ((unsigned char *)&addr)[2];
    a4 = ((unsigned char *)&addr)[3];

    if(0==a1)
    {
        if(0!=addr)
        {
            return FALSE;
        }
    }
    else if(255==a1 && 255==a2 && 255==a3 && 255==a4)
    {
        return FALSE;
    }
    else if(a1>=224 && a1<=239)
    {
        return FALSE;
    }
    else if(a1>=240 && a1<=255)
    {
        return FALSE;/*experimental address*/
    }
    else if(127 == a1)
    {
        return FALSE;
    }

    return TRUE;
}


/*
 *验证主机序 IP 地址是合法的单播 IPV4 地址，返回 1 为 true
 */
int inet_valid_host(u_int32 addr)
{
    return   (!(((addr & 0xff000000) == 0)||                           /* 第 1 个字节 00 */
              ((addr & IN_CLASSC_BROADCAST) == 0)||                     /* 最后 1 个字节 00 */
              ((addr & 0xff000000) == 0xff000000) ||                    /* 第 1 个字节 ff*/
              ((addr & IN_CLASSC_BROADCAST) == IN_CLASSC_BROADCAST)||    /* 后 1 个字节 ff*/
              ((addr & IN_CLASSB_BROADCAST) == IN_CLASSB_BROADCAST)||    /* 后 2 个字节 ff*/
              ((addr & IN_CLASSA_BROADCAST) == IN_CLASSA_BROADCAST)));    /* 后 3 个字节 ff*/
}


/*
 *验证 IP 地址是合法的掩码地址，返回 1 为 true
 */
int inet_valid_mask(u_int32 mask)
{
    if (~(((mask & -mask) - 1) | mask) != 0)
    {
        /* Mask is not contiguous */
        return (FALSE);
    }

    return (TRUE);
}


/*
 *验证网段地址和掩码相匹配，返回 1 为 true
 */
int inet_valid_subnet(u_int32 subnet, u_int32 mask)
{
    if ((subnet & mask) != subnet)
    return (FALSE);

    if (subnet == 0)
    return (mask == 0);

    if (IN_CLASSA(subnet))
    {
      if (mask < 0xff000000 ||
        (subnet & 0xff000000) == 0x7f000000 ||
        (subnet & 0xff000000) == 0x00000000) return (FALSE);
    }
    else if (IN_CLASSD(subnet) || IN_BADCLASS(subnet))
    {
        /* Above Class C address space */
        return (FALSE);
    }

    if (subnet & ~mask)
    {
        /* Host bits are set in the subnet */
        return (FALSE);
    }

    if (!inet_valid_mask(mask))
    {
        /* Netmask is not contiguous */
        return (FALSE);
    }

    return (TRUE);
}


/*
 * 将主机序的 IPv4 地址转换成字符串.
 */
char *inet_ipv4tostr(u_int32 addr, char *s)
{
    register uchar *a;
    uint32_t naddr = 0;

    naddr = (htonl(addr));
    a = (uchar *)&naddr;

    sprintf(s, "%u.%u.%u.%u", a[0], a[1], a[2], a[3]);
    return (s);
}


/*
 * 将字符串转换成主机序的 IP 地址，返回 0 表示错误
 */
u_int32 inet_strtoipv4(char *s)
{
    u_int32 a = 0;
    u_int a0 = 0, a1 = 0, a2 = 0, a3 = 0;
    int i;
    char c;

    i = sscanf(s, "%u.%u.%u.%u%c", &a0, &a1, &a2, &a3, &c);
    if (i > 4 || a0 > 255 || a1 > 255 || a2 > 255 || a3 > 255)
    return 0;

    ((uchar *)&a)[0] = a0;
    ((uchar *)&a)[1] = a1;
    ((uchar *)&a)[2] = a2;
    ((uchar *)&a)[3] = a3;

    return ntohl(a);
}


/*
 * 将主机序的 IPv6 地址转换成字符串.
 */
char *inet_ipv6tostr(struct ipv6_addr *addr, char *s, int size)
{
    if(NULL == addr || NULL == s)
    {
        return NULL;
    }

    return inet_ntop(AF_INET6, (const void *)addr->ipv6, s, size);
}


/*
 * 将字符串转换成主机序的 IPv6 地址，返回 0 表示错误
 */
int inet_strtoipv6(char *s, struct ipv6_addr *dest)
{
    if(NULL == s || NULL == dest)
    {
        return 0;
    }

    return inet_pton(AF_INET6, s, (void *)dest->ipv6);
}

char *inet_prefixtostr(struct inet_prefix *prefix, char *dest, int size)
{
    if(NULL == prefix || NULL == dest)
    {
        return NULL;
    }

    if(INET_FAMILY_IPV4 == prefix->type)
    {
        return inet_ntop(AF_INET, (const void *)&prefix->addr.ipv4, dest, size);
    }
    else if(INET_FAMILY_IPV6 == prefix->type)
    {
        return inet_ntop(AF_INET6, (const void *)prefix->addr.ipv6, dest, size);
    }

    return NULL;
}

char *inet_addrtostr(struct inet_addr *addr, char *dest, int size)
{
    if(NULL == addr || NULL == dest)
    {
        return NULL;
    }

    if(INET_FAMILY_IPV4 == addr->type)
    {
        return inet_ntop(AF_INET, (const void *)&addr->addr.ipv4, dest, size);
    }
    else if(INET_FAMILY_IPV6 == addr->type)
    {
        return inet_ntop(AF_INET6, (const void *)addr->addr.ipv6, dest, size);
    }

    return NULL;
}

/*
 * 计算 checksum
 */
int inet_checksum(u_int16 *addr, u_int len)
{
    register int nleft = (int)len;
    register u_int16 *w = addr;
    u_int16 answer = 0;
    register int sum = 0;

    /*
     *  Our algorithm is simple, using a 32 bit accumulator (sum),
     *  we add sequential 16 bit words to it, and at the end, fold
     *  back all the carry bits from the top 16 bits into the lower
     *  16 bits.
     */
    while (nleft > 1)  {
        sum += *w++;
        nleft -= 2;
    }

    /* mop up an odd byte, if necessary */
    if (nleft == 1) {
        *(u_char *) (&answer) = *(u_char *)w ;
        sum += answer;
    }

    /*
     * add back carry outs from top 16 bits to low 16 bits
     */
    sum = (sum >> 16) + (sum & 0xffff);    /* add hi 16 to low 16 */
    sum += (sum >> 16);            /* add carry */
    answer = ~sum;                /* truncate to 16 bits */
    return (answer);
}


/*
 * Called by following inet_masktostr() to create a mask specified network address.
 */
//void trimdomain(char *cp)
//{
//    static char domain[MAXHOSTNAMELEN + 1];
//    static int first = 1;
//    char *s;

//    if (first)
//    {
//        first = 0;
//        if (gethostname(domain, MAXHOSTNAMELEN) == 0 && (s = strchr(domain, '.')))
//            (void) strcpy(domain, s + 1);
//        else
//            domain[0] = 0;
//    }

//    if (domain[0])
//    {
//        while ((cp = strchr(cp, '.')))
//        {
//            if (!strcasecmp(cp + 1, domain))
//            {
//                *cp = 0;        /* hit it */
//                break;
//            }
//            else
//            {
//                cp++;
//            }
//        }
//    }
//}


static u_long forgemask(u_long a)
{
    u_long m;

    if (IN_CLASSA(a))
    m = IN_CLASSA_NET;
    else if (IN_CLASSB(a))
    m = IN_CLASSB_NET;
    else
    m = IN_CLASSC_NET;
    return (m);
}


static void domask(char *dst, u_long addr, u_long mask)
{
    int b, i;

    if (!mask || (forgemask(addr) == mask))
    {
        *dst = '\0';
        return;
    }

    i = 0;
    for (b = 0; b < 32; b++)
    if (mask & (1 << b))
    {
        int bb;

        i = b;
        for (bb = b+1; bb < 32; bb++)
        if (!(mask & (1 << bb)))
        {
            i = -1; /* noncontig */
            break;
        }
        break;
    }

    if (i == -1)
        sprintf(dst, "&0x%lx", mask);
    else
        sprintf(dst, "/%d", 32 - i);
}


/*
 * Return the string of the network whose ip and mask are given.
 */
char *inet_masktostr(u_int32 addr, u_int32 mask)
{
    static char line[MAXHOSTNAMELEN + 4];

    if ((addr & 0xffffff) == 0)
    sprintf(line, "%u", (addr >> 24));
    else if ((addr & 0xffff) == 0)
    sprintf(line, "%u.%u", (addr >> 24) , (addr >> 16));
    else if ((addr & 0xff) == 0)
    sprintf(line, "%u.%u.%u", (addr >> 24), (addr >> 16), (addr >> 8));
    else
    sprintf(line, "%u.%u.%u.%u", (addr >> 24),
        (addr >> 16), (addr >> 8), (addr));
    domask(line+strlen(line), addr, mask);
    return (line);
}


char *inet_ntoa(struct in_addr in)
{
    static char b[18];
    register char *p;

    p = (char *)&in;
#define    UC(b)    (((int)b)&0xff)
    (void)snprintf(b, sizeof(b),
        "%u.%u.%u.%u", UC(p[0]), UC(p[1]), UC(p[2]), UC(p[3]));
    return (b);
}


/*
 * Check whether "cp" is a valid ascii representation
 * of an Internet address and convert to a binary address.
 * Returns 1 if the address is valid, 0 if not.
 * This replaces inet_addr, the return value from which
 * cannot distinguish between failure and a local broadcast address.
 */
int inet_aton(const char *cp, struct in_addr *addr)
{
    register u_int32_t val;
    register int base, n;
    register char c;
    unsigned int parts[4];
    register unsigned int *pp = parts;

    c = *cp;
    for (;;) {
        /*
         * Collect number up to ``.''.
         * Values are specified as for C:
         * 0x=hex, 0=octal, isdigit=decimal.
         */
        if (!isdigit(c))
            return (0);
        val = 0; base = 10;

        /* 暂时屏蔽，默认按十进制转换 */
        /*
        if (c == '0') {
            c = *++cp;
            if (c == 'x' || c == 'X')
                base = 16, c = *++cp;
            else
                base = 8;
        }
        */

        for (;;) {
            if (isascii(c) && isdigit(c)) {
                val = (val * base) + (c - '0');
                c = *++cp;
            } else if (base == 16 && isascii(c) && isxdigit(c)) {
                val = (val << 4) |
                    (c + 10 - (islower(c) ? 'a' : 'A'));
                c = *++cp;
            } else
                break;
        }
        if (c == '.') {
            /*
             * Internet format:
             *    a.b.c.d
             *    a.b.c    (with c treated as 16 bits)
             *    a.b    (with b treated as 24 bits)
             */
            if (pp >= parts + 3)
                return (0);
            *pp++ = val;
            c = *++cp;
        } else
            break;
    }
    /*
     * Check for trailing characters.
     */
    if (c != '\0' && (!isascii(c) || !isspace(c)))
        return (0);
    /*
     * Concoct the address according to
     * the number of parts specified.
     */
    n = pp - parts + 1;
    switch (n) {

    case 0:
        return (0);        /* initial nondigit */

    case 1:                /* a -- 32 bits */
        break;

    case 2:                /* a.b -- 8.24 bits */
        if ((val > 0xffffff) || (parts[0] > 0xff))
            return (0);
        val |= parts[0] << 24;
        break;

    case 3:                /* a.b.c -- 8.8.16 bits */
        if ((val > 0xffff) || (parts[0] > 0xff) || (parts[1] > 0xff))
            return (0);
        val |= (parts[0] << 24) | (parts[1] << 16);
        break;

    case 4:                /* a.b.c.d -- 8.8.8.8 bits */
        if ((val > 0xff) || (parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff))
            return (0);
        val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
        break;
    }
    if (addr)
        addr->s_addr = htonl(val);
    return (1);
}

/* convert a network format ipv4 address to string format.
 * return:
 *    `dst' (as a const)
 */
char *inet_ntop4(const u_char *src, char *dst, unsigned int size)
{
    static const char fmt[] = "%u.%u.%u.%u";
    char tmp[sizeof "255.255.255.255"];

    if (SPRINTF((tmp, fmt, src[0], src[1], src[2], src[3])) > size)
    {
        return (NULL);
    }
    strcpy(dst, tmp);
    return (dst);
}


/* convert a network format ipv6 address to string format.
 */
char *inet_ntop6(const u_char *src, char *dst, unsigned int size)
{
    char tmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"], *tp;
    struct { int base, len; } best, cur;
    u_int words[sizeof(struct in6_addr) / INT16SZ];
    int i;

    /*
     * Preprocess:
     *    Copy the input (bytewise) array into a wordwise array.
     *    Find the longest run of 0x00's in src[] for :: shorthanding.
     */
	memset(&best, 0, sizeof best);
	memset(&cur, 0, sizeof cur);
    memset(words, '\0', sizeof words);
    for (i = 0; i < IN6ADDRSZ; i++)
        words[i / 2] |= (src[i] << ((1 - (i % 2)) << 3));
    best.base = -1;
    cur.base = -1;
    for (i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
        if (words[i] == 0) {
            if (cur.base == -1)
                cur.base = i, cur.len = 1;
            else
                cur.len++;
        } else {
            if (cur.base != -1) {
                if (best.base == -1 || cur.len > best.len)
                    best = cur;
                cur.base = -1;
            }
        }
    }
    if (cur.base != -1) {
        if (best.base == -1 || cur.len > best.len)
            best = cur;
    }
    if (best.base != -1 && best.len < 2)
        best.base = -1;

    /*
     * Format the result.
     */
    tp = tmp;
    for (i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
        /* Are we inside the best run of 0x00's? */
        if (best.base != -1 && i >= best.base &&
            i < (best.base + best.len)) {
            if (i == best.base)
                *tp++ = ':';
            continue;
        }
        /* Are we following an initial run of 0x00s or any real hex? */
        if (i != 0)
            *tp++ = ':';
        /* Is this address an encapsulated IPv4? */
        if (i == 6 && best.base == 0 &&
            (best.len == 6 || (best.len == 5 && words[5] == 0xffff))) {
            if (!inet_ntop4(src+12, tp, sizeof tmp - (tp - tmp)))
                return (NULL);
            tp += strlen(tp);
            break;
        }
        tp += SPRINTF((tp, "%x", words[i]));
    }
    /* Was it a trailing run of 0x00's? */
    if (best.base != -1 && (best.base + best.len) == (IN6ADDRSZ / INT16SZ))
        *tp++ = ':';
    *tp++ = '\0';

    /*
     * Check for overflow, copy, and we're done.
     */
    if ((size_t)(tp - tmp) > size) {
        errno = (ENOSPC);
        return (NULL);
    }
    strcpy(dst, tmp);
    return (dst);
}


/* convert a network format ip address to string format.
 * return:
 *    pointer to presentation format address (`dst'), or NULL (see errno).
 */
char *inet_ntop(int af, const void *src, char *dst, int size)
{
    switch (af) {
    case AF_INET:
        return (inet_ntop4(src, dst, size));
    case AF_INET6:
        return (inet_ntop6(src, dst, size));
    default:
        errno = (EAFNOSUPPORT);
        return (NULL);
    }
}


/* convert a string format ipv4 address to network format.
 * return:
 *    1 if `src' is a valid dotted quad, else 0.
 * notice:
 *    does not touch `dst' unless it's returning 1.
 */
int inet_pton4(const char *src, u_char *dst)
{
    static const char digits[] = "0123456789";
    int saw_digit, octets, ch;
    u_char tmp[INADDRSZ], *tp;

    saw_digit = 0;
    octets = 0;
    *(tp = tmp) = 0;
    while ((ch = *src++) != '\0') {
        const char *pch;

        if ((pch = strchr(digits, ch)) != NULL) {
            u_int new = *tp * 10 + (pch - digits);

            if (new > 255)
                return (0);
            *tp = new;
            if (! saw_digit) {
                if (++octets > 4)
                    return (0);
                saw_digit = 1;
            }
        } else if (ch == '.' && saw_digit) {
            if (octets == 4)
                return (0);
            *++tp = 0;
            saw_digit = 0;
        } else
            return (0);
    }
    if (octets < 4)
        return (0);

    memcpy(dst, tmp, INADDRSZ);
    return (1);
}


/* convert a string format ipv6 address to network format.
 * return:
 *    1 if `src' is a valid [RFC1884 2.2] address, else 0.
 * notice:
 *    (1) does not touch `dst' unless it's returning 1.
 *    (2) :: in a full address is silently ignored.
 */
int inet_pton6(const char *src, u_char *dst)
{
    static const char xdigits_l[] = "0123456789abcdef",
                      xdigits_u[] = "0123456789ABCDEF";
    u_char tmp[IN6ADDRSZ], *tp, *endp, *colonp;
    const char *xdigits, *curtok;
    int ch, saw_xdigit;
    u_int val;

    memset((tp = tmp), '\0', IN6ADDRSZ);
    endp = tp + IN6ADDRSZ;
    colonp = NULL;
    /* Leading :: requires some special handling. */
    if (*src == ':')
        if (*++src != ':')
            return (0);
    curtok = src;
    saw_xdigit = 0;
    val = 0;
    while ((ch = *src++) != '\0') {
        const char *pch;

        if ((pch = strchr((xdigits = xdigits_l), ch)) == NULL)
            pch = strchr((xdigits = xdigits_u), ch);
        if (pch != NULL) {
            val <<= 4;
            val |= (pch - xdigits);
            if (val > 0xffff)
                return (0);
            saw_xdigit = 1;
            continue;
        }
        if (ch == ':') {
            curtok = src;
            if (!saw_xdigit) {
                if (colonp)
                    return (0);
                colonp = tp;
                continue;
            }
            if (tp + INT16SZ > endp)
                return (0);
            *tp++ = (u_char) (val >> 8) & 0xff;
            *tp++ = (u_char) val & 0xff;
            saw_xdigit = 0;
            val = 0;
            continue;
        }
        if (ch == '.' && ((tp + INADDRSZ) <= endp) &&
            inet_pton4(curtok, tp) > 0) {
            tp += INADDRSZ;
            saw_xdigit = 0;
            break;    /* '\0' was seen by inet_pton4(). */
        }
        return (0);
    }
    if (saw_xdigit) {
        if (tp + INT16SZ > endp)
            return (0);
        *tp++ = (u_char) (val >> 8) & 0xff;
        *tp++ = (u_char) val & 0xff;
    }
    if (colonp != NULL) {
        /*
         * Since some memmove()'s erroneously fail to handle
         * overlapping regions, we'll do the shift by hand.
         */
        const int n = tp - colonp;
        int i;

		if(tp == endp)
		{
			return (0);
		}

        for (i = 1; i <= n; i++) {
            endp[- i] = colonp[n - i];
            colonp[n - i] = 0;
        }
        tp = endp;
    }
    if (tp != endp)
        return (0);
    memcpy(dst, tmp, IN6ADDRSZ);
    return (1);
}


/* convert a string format ip address to network format.
 * return:
 *    1 if the address was valid for the specified address family
 *    0 if the address wasn't valid (`dst' is untouched in this case)
 *    -1 if some other error occurred (`dst' is untouched in this case, too)
 */
int inet_pton(int af, const char *src, void *dst)
{
    switch (af) {
    case AF_INET:
        return (inet_pton4(src, dst));
    case AF_INET6:
        return (inet_pton6(src, dst));
    default:
        errno = EAFNOSUPPORT;
        return (-1);
    }
}

/* 获取 127.0.0.1 的主机序 IPV4 地址 */
uint32_t ipv4_get_loopback()
{
    return 0x7f000001;
}


/* 判断 IP 是 127 网段，返回 1 为 true */
uint32_t ipv4_is_loopback(uint32_t addr)
{
    return (addr & 0xff000000) == 0x7f000000;
}

/* 判断 IP 是组播地址，返回 1 为 true */
uint32_t ipv4_is_multicast(uint32_t addr)
{
    return (addr & 0xf0000000) == 0xe0000000;
}

/* 判断 IP 是保留组播地址，返回 1 为 true */
uint32_t ipv4_is_local_multicast(uint32_t addr)
{
    return (addr & 0xffffff00) == 0xe0000000;
}


/* 判断 IP 是全 F 地址，返回 1 为 true */
uint32_t ipv4_is_lbcast(uint32_t addr)
{
    /* limited broadcast */
    return addr == INADDR_BROADCAST;
}

/* 判断 IP 是 0 网段，返回 1 为 true */
uint32_t ipv4_is_zeronet(uint32_t addr)
{
    return (addr & 0xff000000) == 0x00000000;
}

/* Special-Use IPv4 Addresses (RFC3330) */

uint32_t ipv4_is_private_10(uint32_t addr)
{
    return (addr & 0xff000000) == 0x0a000000;
}

uint32_t ipv4_is_private_172(uint32_t addr)
{
    return (addr & 0xfff00000) == 0xac100000;
}

uint32_t ipv4_is_private_192(uint32_t addr)
{
    return (addr & 0xffff0000) == 0xc0a80000;
}

uint32_t ipv4_is_linklocal_169(uint32_t addr)
{
    return (addr & 0xffff0000) == 0xa9fe0000;
}

uint32_t ipv4_is_anycast_6to4(uint32_t addr)
{
    return (addr & 0xffffff00) == 0xc0586300;
}

uint32_t ipv4_is_test_192(uint32_t addr)
{
    return (addr & 0xffffff00) == 0xc0000200;
}

uint32_t ipv4_is_test_198(uint32_t addr)
{
    return (addr & 0xfffe0000) == 0xc6120000;
}


/* 判断 IP 是广播地址，返回 1 为 true */
uint32_t ipv4_is_broadcast(uint32_t addr)
{
    /* 受限广播 */
    if(((addr & 0xff000000) == 0xff000000))
    {
        return 1;
    }
    else
        return 0;
}


/*根据掩码长度判断ip 是广播地址，返回1 为true*/
uint32_t ipv4_is_broadcast1(uint32_t addr, uint8_t masklen)
{
    struct in_addr mask;

    if(masklen == 32)
        return 0;

    masklen2ip(masklen,&mask);
    mask.s_addr = ntohl(mask.s_addr);

    return ((addr & (~mask.s_addr)) == (~mask.s_addr));
}


/* if addr1 and addr2 belong to the same subnet, return 1, otherwise return 0 */
int ipv4_is_same_subnet(uint32_t addr1, uint32_t addr2, uint8_t masklen)
{
    struct in_addr mask;

    masklen2ip(masklen, &mask);
    mask.s_addr = ntohl(mask.s_addr);

    addr1 &= mask.s_addr;
    addr2 &= mask.s_addr;

    if(addr1 == addr2)
        return 1;
    else
        return 0;
}


/* 判断 IP 是合法地址，返回 1 为 true */
uint32_t ipv4_is_valid(uint32_t addr)
{
    /* 第 1 个字节 00 */
    if((addr & 0xff000000) == 0)
    {
        return 0;
    }

    return 1;
}


/* if addr1 and addr2 belong to the same subnet, return 1, otherwise return 0 */
int ipv6_is_same_subnet(struct ipv6_addr *paddr1, struct ipv6_addr *paddr2, uint8_t masklen)
{
    int masklen_1,masklen_2;
    uint8_t ipv6_mask[16];

    memset(ipv6_mask, 0, 16);

    masklen_1 = masklen/ 8;
    masklen_2 = masklen % 8;

    if (masklen_1 < 16)
    {
        ipv6_mask[masklen_1] = ~(0xFF >> masklen_2);

        if ((memcmp(paddr1->ipv6, paddr2->ipv6, masklen_1) == 0)
        && ((paddr1->ipv6[masklen_1] & ipv6_mask[masklen_1]) == (paddr2->ipv6[masklen_1] & ipv6_mask[masklen_1])))
        {
            return 1;
        }
    }
    else if (masklen_1 == 16)
    {
        if (memcmp(paddr1->ipv6, paddr2->ipv6, masklen_1) == 0)
        {
            return 1;
        }
    }

    return 0;
}


/* 判断 IPv6 是合法地址，返回 1 为 true */
int ipv6_is_valid(struct ipv6_addr *paddr)
{
    if(IPV6_ADDR_SAME(paddr, ipv6_zero))
        return 0;

    return 1;
}


/* 判断 IPv6 是全 0 地址，返回 1 为 true */
int ipv6_is_zero(struct ipv6_addr *paddr)
{
    if(IPV6_ADDR_SAME(paddr, ipv6_zero))
        return 1;

    return 0;
}


/* 判断 IP 是 0 网段，返回 1 为 true */
int ipv6_is_zeronet(struct ipv6_addr *paddr)
{
    if(paddr->ipv6[0] == 0)
        return 1;
    else
        return 0;
}


/* 获取 0::1/128 的 loopback 地址 */
//static struct ipv6_addr * ipv6_get_loopback()
//{
//    return (struct ipv6_addr * )ipv6_loopback;
//}


/* 根据 mac 获取链路本地地址 */
#if 0
struct ipv6_addr * ipv6_get_linklocal(uint8_t *pmac)
{
    uint8_t ipv6[16];

    memset(ipv6, 0, 16);

    ipv6[0] = 0xfe;  /* 第 1 个字节 fe */
    ipv6[1] = 0x80;  /* 第 2 个字节 80 */

    memcpy(ipv6[8], pmac, 3);       /* 8 - 10 字节拷贝 mac 的前 3 个字节 */
    ipv6[11] = 0xff;
    ipv6[12] = 0xfe;
    memcpy(ipv6[13], &pmac[4], 3);  /* 13 - 15 字节拷贝 mac 的后 3 个字节 */

    return (struct ipv6_addr * )ipv6;
}
#endif

/* 判断 IPv6 是 loopback 地址( ::1/128)，返回 1 为 true */
int ipv6_is_loopback(struct ipv6_addr *paddr)
{
    if(IPV6_ADDR_SAME(paddr, ipv6_loopback))
        return 1;
    else
        return 0;
}


/* 判断 IP 是组播地址(ff00::/8)，返回 1 为 true */
int ipv6_is_multicast(struct ipv6_addr *paddr)
{
    if(paddr->ipv6[0] == 0xff)
        return 1;
    else
        return 0;
}


/* 判断 IP 是链路本地地址(fe80::/10)，返回 1 为 true */
int ipv6_is_linklocal(struct ipv6_addr *paddr)
{
    if((paddr->ipv6[0] == 0xfe) && ((paddr->ipv6[1] & 0x80) == 0x80))
        return 1;
    else
        return 0;
}


/* 判断 IP 是站点本地地址(fec0::/10)，返回 1 为 true */
int ipv6_is_sitelocal(struct ipv6_addr *paddr)
{
    if((paddr->ipv6[0] == 0xfe) && ((paddr->ipv6[1] & 0xc0) == 0xc0))
        return 1;
    else
        return 0;
}


/* 判断 IP 是 IPV4 映射地址，返回 1 为 true */
int ipv6_is_ipv4map(struct ipv6_addr *paddr)
{
    if(memcmp(paddr, ipv6_ipv4map, 12) == 0)
        return 1;
    else
        return 0;
}


/* 检查 inet_addr 是否有效，返回 1 表示有效，0 无效 */
//int ip_is_valid(struct inet_addr *paddr)
//{
//    if(paddr == NULL)
//        return 0;

//    if(paddr->type == INET_FAMILY_IPV4)
//    {
//        if(paddr->addr.ipv4)
//            return 1;
//    }
//    else if(paddr->type == INET_FAMILY_IPV6)
//    {
//        if(ipv6_is_valid(&paddr->addr))
//            return 1;
//    }

//    return 0;
//}


/* 判断 IP 是全球单播地址，返回 1 为 true */
uint32_t ipv6_is_global_unicast(struct ipv6_addr *paddr)
{
    return (paddr->ipv6[0]& 0xFE) == 0xFC;
}


