/*
*    define mac address and ethernet
*/

#include <string.h>
#include "ether.h"
#include "inet_ip.h"

static uchar bcmac[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
static uchar zeromac[6] = {0x00,0x00,0x00,0x00,0x00,0x00};


static char  hexChar[] = "0123456789ABCDEF";
#define HEX_TO_DIGIT(x)  (((x) >= 'A') ? ((x) - 'A' + 10) : ((x) - '0'))


/* 将 mac 地址数组转换成字符串格式 */
uchar *ether_mac_to_string(char *mac, uchar *str)
{
  int i, j = 0;

  for (i = 0; i < 6; i++)
  {
    str[j++] = hexChar[(mac[i]) >> 4];
    str[j++] = hexChar[(mac[i]) & 0x0F];
    str[j++] = ':';
  }

  str[j-1] = '\0';

  return str;
}


/* 将字符串转换成 mac 地址数组 */
uchar *ether_string_to_mac (const char *str, uchar *mac)
{
  int i, j = 0;
  char c;
  unsigned char b1, b2;

  for (i = 0; i < 6; i++)
  {
      c = str[j++];
      if (c >= 'a' && c <= 'f')
        c &= 0x0DF;           /* Force to Uppercase. */
      b1 = HEX_TO_DIGIT(c);
      c = str[j++];
      if (c >= 'a' && c <= 'f')
        c &= 0x0DF;           /* Force to Uppercase. */
      b2 = HEX_TO_DIGIT(c);
      mac[i] = b1 * 16 + b2;
      j++;                    /* Skip a colon. */
  }

  return mac;
}


/* 将字符串HHHH-HHHH-HHHH转换成 mac 地址数组 */
uchar *ether_string1_to_mac (char *str, uchar *mac)
{
  int i, j = 0;
  char c;
  unsigned char b1, b2, b3, b4;

  for (i = 0; i < 6; i++)
  {
      c = str[j++];
      if (c >= 'a' && c <= 'f')
        c &= 0x0DF;           /* Force to Uppercase. */
      b1 = HEX_TO_DIGIT(c);
      c = str[j++];
      if (c >= 'a' && c <= 'f')
        c &= 0x0DF;           /* Force to Uppercase. */
      b2 = HEX_TO_DIGIT(c);
	  mac[i] = b1 * 16 + b2;
	  i++;
	  
	  c = str[j++];
      if (c >= 'a' && c <= 'f')
        c &= 0x0DF;           /* Force to Uppercase. */
      b3 = HEX_TO_DIGIT(c);
	  c = str[j++];
      if (c >= 'a' && c <= 'f')
        c &= 0x0DF;           /* Force to Uppercase. */
      b4 = HEX_TO_DIGIT(c);
      mac[i] = b3 * 16 + b4;
      j++;                    /* Skip a colon. */
  }

  return mac;
}


/* 获取全 0 mac */
uchar *ether_get_zero_mac(void)
{
	return zeromac;
}


/* 获取广播 mac */
uchar *ether_get_broadcast_mac(void)
{
	return bcmac;
}


/* 根据组播 IP 获取组播 mac，rfc1122 */
int ether_get_muticast_mac(uint32_t netip, uchar *mac)
{
	mac[0]  = 0x01;
	mac[1]  = 0x00;
	mac[2]  = 0x5E;
	mac[3]  = (netip >> 16) & 0x7F;
	mac[4]  = (netip >>  8) & 0xFF;
	mac[5]  =  netip	    & 0xFF;

	return 0;
}


/*IPv6 多播地址映射位以太网地址 33:33:xx:xx:xx:xx */
int ether_get_ipv6_muticast_mac(struct ipv6_addr *paddr, uchar *mac)
{
	mac[0]  = 0x33;
	mac[1]  = 0x33;
	mac[2]  = paddr->ipv6[12] & 0xFF;
	mac[3]  = paddr->ipv6[13] & 0xFF;
	mac[4]  = paddr->ipv6[14] & 0xFF;
	mac[5]  = paddr->ipv6[15] & 0xFF;

	return 0;
}


/* 判断是否广播 MAC, 返回 0 为 true */
int ether_is_broadcast_mac(uchar *mac)
{
	return memcmp(mac, bcmac, 6);
}


/* 判断是否组播MAC, 返回 0 为 true */
int ether_is_muticast_mac(uchar *mac)
{
	return ((mac[0] & 0x1) ? 0 : 1);
}


/* 判断是否全 0 MAC，返回 0 为 true */
int ether_is_zero_mac(uchar *mac)
{
	return memcmp(mac, zeromac, 6);
}


/* 判断单播 mac，返回 0 为  */
int ether_is_unicast_mac(uchar *mac)
{
	if(memcmp(mac, zeromac, 6))    /* 0 mac */
		return 1;
	else if(memcmp(mac, bcmac, 6)) /* 广播 mac */
		return 1;
	else if(mac[0] & 0x1)          /* 组播 mac */
		return 1;
	else
		return 0;
}


/* 检查 MAC 地址字符串的合法性 */
int ether_valid_mac(const char *str)
{
	int colon = 0;
	int legal_len = 0;
	int actual_len = 0;
	const char *prev = NULL;
	const char *smac = NULL;
	const char *str_tmp = str;

	if(str == NULL)
		return 1;

	legal_len = strlen("FF:FF:FF:FF:FF:FF");
	actual_len = strlen(str_tmp);

	if (actual_len != legal_len)
	{
		return 1;
	}

	if(!strcmp(str,"00:00:00:00:00:00"))
	{
		return 1;
	}

	while (1)
	{
		if (CHAR_IS_MAC_DIGITAL(*str_tmp))
		{
			if (smac == NULL)
			{
				smac = str_tmp;
			}
		}
		else if (*str_tmp == ':')
		{
			NOT_MAC_DIGITAL_CHK(prev);
			GET_ONE_COLONOFMAC(smac, str_tmp-1);

            smac = NULL;

			colon++;
			if (colon > 5)
			{
				return 1;
			}
		}
		else if (*str_tmp == '\0')
		{
			if (colon != 5)
			{
				return 1;
			}

			NOT_MAC_DIGITAL_CHK(prev);
			GET_ONE_COLONOFMAC(smac, str_tmp-1);

			break;
		}
		else
		{
			return 1;
		}

		prev = str_tmp;
		str_tmp = str_tmp + 1;
	}

    return 0;
}

