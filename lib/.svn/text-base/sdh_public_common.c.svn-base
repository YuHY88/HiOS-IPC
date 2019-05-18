
#include <stdio.h>
#include <string.h>
#include "sdh_public_common.h"

int get_valid_jx(unsigned char *pinput, unsigned char *poutput)
{
    unsigned char   i;
    unsigned char   temp[32];
    unsigned char   test = 0;

    if (NULL == pinput || NULL == poutput)
    {
        return -2;
    }

    memcpy(temp, pinput, 16);
    memcpy(temp + 16, pinput, 16);
    memset(poutput, 0x00, 16);

    for (i = 0; i < 16; ++i)
    {
        if (temp[i] & 0x80)
        {
            memcpy(poutput, temp + i, 16);
            break;
        }
    }

    if (i == 16)
    {
        memset(poutput, 0x00, 16);
        return -1;
    }

    for (i = 0; i < 16; ++i)
    {
        if (poutput[i] & 0x80)
        {
            test++;

            if (test > 1)
            {
                memset(poutput, 0x00, 16);
                return -1;
            }
        }
    }

    return 0;
}

