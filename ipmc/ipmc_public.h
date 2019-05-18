#ifndef IP_MC_PUBLIC_H
#define IP_MC_PUBLIC_H

/*ip4mc/ip6mc */
#define IPMC_TRUE  1
#define IPMC_FALSE 0

#define IP_MC_SEND_BUF_SIZE (4 * 1024)		/*4KB*/
#define IP_MC_RECV_BUF_SIZE (4 * 1024)		

static inline uint8_t ipmc_getc(uint8_t **buf)
{
    uint8_t c;

    c = *(*buf);
	(*buf)++;

    return c;
}

static inline uint16_t ipmc_getw(uint8_t **buf)
{
    uint16_t w;

	w = *(*buf) << 8;
	(*buf)++;
	w |= *(*buf);
	(*buf)++;
	
    return w;
}

static inline uint32_t ipmc_getl(uint8_t **buf)
{
    uint32_t l;

    l  = *(*buf) << 24;
	(*buf)++;
    l |= *(*buf) << 16;
	(*buf)++;
    l |= *(*buf) << 8;
	(*buf)++;
    l |= *(*buf);
	(*buf)++;

    return l;
}

static inline uint32_t ipmc_setc(uint8_t **buf, uint8_t c)
{
	*(*buf) = (uint8_t)c;
	(*buf)++;

	return 1;
}

static inline uint32_t ipmc_setw(uint8_t **buf, uint16_t w)
{
    *(*buf) = (uint8_t)(w >> 8);
	(*buf)++;
    *(*buf) = (uint8_t)w;
	(*buf)++;
	
    return 2;
}

static inline uint32_t ipmc_setl(uint8_t **buf, uint32_t l)
{
    *(*buf) = (uint8_t)(l >> 24);
	(*buf)++;
    *(*buf) = (uint8_t)(l >> 16);
	(*buf)++;
    *(*buf) = (uint8_t)(l >> 8);
	(*buf)++;
    *(*buf) = (uint8_t)l;
	(*buf)++;
	
    return 4;
}

#endif
