/* xalloc.c - Failsafe memory allocation functions.
 *            Taken from excellent glibc.info ;)
 * 
 * Copyright (C) 2010, Pawel Krawczyk <pawel.krawczyk@hush.com> and
 * Jeroen Nijhof <jeroen@jeroennijhof.nl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program - see the file COPYING.
 *
 * See `CHANGES' file for revision history.
 */

#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "xalloc.h"

void *xcalloc(size_t nmemb, size_t size)
{
    void *val = calloc(nmemb, size);
    if(val == NULL)
	{
        zlog(zlog_default, LOG_ERR, "%s: calloc(%u,%u) failed",\
			__FUNCTION__, (unsigned)nmemb, (unsigned)size);
        exit(1);
    }
    return val;
}

void *xrealloc(void *ptr, size_t size)
{
#if 0
    void *val = realloc(ptr, size);
    if(val == NULL)
	{
        zlog(zlog_default, LOG_ERR, "%s: realloc(%u) failed",\
			__FUNCTION__, (unsigned) size);
        exit(1);
    }
	return val;
#endif
	zlog_err ("%s[%d] : function has been shielded\n", __func__, __LINE__);
    return NULL;
}

char *xstrdup(const char *str)
{
    char *p;
    if (str == NULL) return NULL;

    if ( (p = strdup(str)) == NULL )
	{
        zlog(zlog_default, LOG_ERR, "%s: strdup(%s) failed: %m", __FUNCTION__, str);
        exit(1);
    }
    return p;
}


/*
    safe string copy that aborts when destination buffer is too small
*/
char *xstrcpy(char *dst, const char *src, size_t dst_size) {
    if (dst == NULL) {
        zlog(zlog_default, LOG_ERR, "xstrcpy(): dst == NULL");
        abort();
    }
    if (src == NULL) {
        zlog(zlog_default, LOG_ERR, "xstrcpy(): src == NULL");
        abort();
    }
    if (!dst_size)
        return NULL;

    if (strlen(src) >= dst_size) {
        zlog(zlog_default, LOG_ERR, "xstrcpy(): argument too long, aborting");
        abort();
    }

    return strcpy(dst, src);
}

