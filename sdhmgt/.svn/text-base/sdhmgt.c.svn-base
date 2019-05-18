/**********************************************************
* file name: sdhmgt.c
* Copyright:
     Copyright 2018 huahuan.
* author:
*    huahuan jianghongzhi 2018-04-16
* function:
*
* modify:
*
***********************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "types.h"
#include "md5.h"

#include "sdhmgt.h"

#define READ_DATA_SIZE  1024

struct sdhmgt_entry     g_sdhmgt_entry = {0};
struct thread_master   *g_psdhmgt_master = NULL;
uint32_t                g_sdhmgt_debug = 0;

typedef struct _tx_id_map_mx_id
{
    uint8_t     tx_id;
    uint32_t    mx_id;
}   tx_id_map_mx_id;

const tx_id_map_mx_id _id_map[] =
{
    { 0x04, ID_EP01     },
    { 0x05, ID_FE01     },
    { 0x08, ID_EP03     },
    { 0x0b, ID_LA01     },
    { 0x0f, ID_FE02     },
    { 0x5c, ID_SD02     },
    { 0x5d, ID_CHD01    },
    { 0x5f, ID_DIO20    },
    { 0x65, ID_C37D     },
    { 0x6a, ID_FE64     },
    { 0x6b, ID_SD03     },
    { 0x6e, ID_CHU02    },
    { 0x73, ID_DIO04    },
    { 0x74, ID_CH4W02   },
    { 0x75, ID_GX05C    },
    { 0x7f, ID_CHU03    },
    { 0x83, ID_TX02     },
    { 0x85, ID_OS16A    },
    { 0x86, ID_OS04A    },
    { 0x87, ID_OS01A    },
    { 0x8c, ID_CH4W03   },
    { 0xA4, ID_OS01S    },
};

int sdhmgt_compute_file_md5(const char *file_path, char *md5_str);

uint32_t sdhmgt_get_MXID_from_TXID(uint8_t id)
{
    int     i;
    int     num;

    num = sizeof(_id_map) / sizeof(tx_id_map_mx_id);

    for (i = 0; i < num; ++i)
    {
        if (id == _id_map[i].tx_id)
        {
            return _id_map[i].mx_id;
        }
    }

    return 0;
}

int sdhmgt_compute_file_md5(const char *file_path, char *md5_str)
{
    int i;
    int fd;
    int ret;
    unsigned char data[READ_DATA_SIZE];
    unsigned char md5_value[MD5_SIZE];
    MD5_CTX md5;

    if (NULL == file_path || NULL == md5_str)
    {
        return -1;
    }

    fd = open(file_path, O_RDONLY);

    if (-1 == fd)
    {
        return -1;
    }

    MD5Init(&md5);

    while (1)
    {
        ret = read(fd, data, READ_DATA_SIZE);

        if (ret <= 0)
        {
            break;
        }

        MD5Update(&md5, data, ret);

        if (ret < READ_DATA_SIZE)
        {
            break;
        }
    }

    close(fd);

    MD5Final(md5_value, &md5);

    for (i = 0; i < MD5_SIZE; i++)
    {
        snprintf(md5_str + i * 2, 2 + 1, "%02x", md5_value[i]);
    }

    md5_str[MD5_STR_LEN] = '\0'; // add end

    return 0;
}

void sdhmgt_config_file_check(void)
{
    struct stat file_stat;

    if (stat(SDH_CONFIG_FILE, &file_stat) != -1)
    {
        g_sdhmgt_entry.config_file = 1;

        sdhmgt_compute_file_md5(SDH_CONFIG_FILE, g_sdhmgt_entry.md5_str);
        g_sdhmgt_entry.config_file_mtime = file_stat.st_mtime;
    }
    else
    {
        g_sdhmgt_entry.config_file = 0;

        memset(g_sdhmgt_entry.md5_str, 0, sizeof(g_sdhmgt_entry.md5_str));
        g_sdhmgt_entry.config_file_mtime = 0;
    }
}

void sdhmgt_config_file_recheck(void)
{
    struct stat file_stat;

    if (stat(SDH_CONFIG_FILE, &file_stat) != -1)
    {
        g_sdhmgt_entry.config_file = 1;

        if (g_sdhmgt_entry.config_file_mtime != file_stat.st_mtime)
        {
            sdhmgt_compute_file_md5(SDH_CONFIG_FILE, g_sdhmgt_entry.md5_str);
            g_sdhmgt_entry.config_file_mtime = file_stat.st_mtime;
        }
    }
    else
    {
        g_sdhmgt_entry.config_file = 0;

        memset(g_sdhmgt_entry.md5_str, 0, sizeof(g_sdhmgt_entry.md5_str));
        g_sdhmgt_entry.config_file_mtime = 0;
    }
}


