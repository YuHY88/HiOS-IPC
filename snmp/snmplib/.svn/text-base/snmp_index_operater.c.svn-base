
#include "net-snmp-config.h"
#include <lib/types.h>
//#include "types.h"
#include <lib/log.h>

#include "net-snmp-includes.h"
#include "net-snmp-agent-includes.h"
//#include "lib/snmp_common.h"
#include "snmp_config_table.h"

#include "snmp_index_operater.h"

void *
oid_copy(void *dest, const void *src, size_t size)
{
    return memcpy(dest, src, size * sizeof(oid));
}

int
oid_compare(const oid *o1, int o1_len, const oid *o2, int o2_len)
{
    int i;

    for (i = 0; i < min(o1_len, o2_len); i++)
    {
        if (o1[i] < o2[i])
        {
            return -1;
        }
        else if (o1[i] > o2[i])
        {
            return 1;
        }
    }

    if (o1_len < o2_len)
    {
        return -1;
    }

    if (o1_len > o2_len)
    {
        return 1;
    }

    return 0;
}

/* an array of bit masks in SNMP bit order (*MSBit first) */
static char bitstring_mask[] = { 1 << 7, 1 << 6, 1 << 5, 1 << 4, 1 << 3, 1 << 2, 1 << 1, 1 << 0 };

/* copies a byte array into an oid, converting to unsigned int values. */

void
oid_copy_bytes2oid(oid oid[], void *addr, s_int32_t len)
{
    s_int32_t i;
    u_char *pnt = (u_char *) addr;

    for (i = 0; i < len; i++)
    {
        oid[i] = *pnt++;
    }
}

/* copy oid values to an unsigned byte array.
 */

int
oid2bytes(oid oid[], s_int32_t len, void *addr)
{
    s_int32_t i;
    u_char *pnt = (u_char *) addr;

    for (i = 0; i < len; i++)
    {
        *pnt++ = (oid[i] > UINT8_MAX ? UINT8_MAX : oid[i]);
    }

    return 0;
}

void print_oid(oid *name, size_t  length)
{
    int i = 0 ;

    for (i = 0 ; i < length ; i++)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%d." , name[i]);
    }

    zlog_debug(SNMP_DBG_MIB_GET, "\n");
}

/* Utility function to get INTEGER index.  */
int ipran_snmp_int_index_get(struct variable *v, oid *name, size_t *length,
                             u_int32_t *index, int exact)
{
    int result, len;

    *index = 0;
    len = v->namelen >= *length ? *length : v->namelen;
    result = oid_compare(name, len, v->name, len);

    if (exact)
    {
        /* Check the length. */
        if (result != 0 || *length - v->namelen != 1)
        {
            return -1;
        }

        *index = name[v->namelen];
        return 0;
    }
    else                          /* GETNEXT request */
    {
        /* return -1 if greater than our our table entry */
        if (result > 0)
        {
            return -1;
        }
        else if (result == 0)
        {
            /* use index value if present, otherwise 0. */
            len = *length - v->namelen;

            if (len >= 1)
            {
                *index = name[v->namelen];
                return 0;
            }
            else if (len < 0)
            {
                memcpy(name, v->name, ((int) v->namelen) * sizeof(oid));
            }
        }
        else
        {
            /* set the user's oid to be ours */
            memcpy(name, v->name, ((int) v->namelen) * sizeof(oid));
        }
    }

    return 1;
}

/* Utility function to set the object name and INTEGER index. */
void ipran_snmp_int_index_set(struct variable *v, oid *name, size_t *length,
                              u_int32_t index)
{
    name[v->namelen] = index;
    *length = v->namelen + 1;
}

// octstring index no IMPLICIT
int
ipran_snmp_octstring_index_get(struct variable *v, oid *name, size_t *length,
                               u_char *oct_string, int oct_maxlen, int exact)
{
    int         str_offset = 1;     // str_len
    int         min_idx_len = 2;    //str_len,str
    int         result;
    int         str_index_cur_len;
    int         str_index_val_len;
    int         get_idx;

    if (0 == oct_maxlen)
    {
        return -1;
    }

    memset(oct_string, 0x00, oct_maxlen);

    get_idx     = 0;
    str_index_val_len = 0;

    if (exact)
    {
        if (*length < v->namelen + min_idx_len)
        {
            return -1;
        }

        str_index_cur_len = *length - v->namelen + 1 - min_idx_len;
        str_index_val_len = name[v->namelen + str_offset - 1];

        if (str_index_cur_len != str_index_val_len)
        {
            return -1;
        }

        result = oid_compare(name, v->namelen, v->name, v->namelen);

        if (result != 0)
        {
            return -1;
        }

        get_idx   = 1;
    }
    else
    {
        if (*length >= v->namelen + min_idx_len)
        {
            str_index_cur_len = *length - v->namelen + 1 - min_idx_len;
            str_index_val_len = name[v->namelen + str_offset - 1];

            if (str_index_cur_len > str_index_val_len)
            {
                return -1;
            }

            result = oid_compare(name, v->namelen, v->name, v->namelen);

            if (result == 0 && str_index_cur_len == str_index_val_len)
            {
                get_idx = 1;
            }
        }
        else if (*length >= v->namelen)
        {
            result = oid_compare(name, v->namelen, v->name, v->namelen);
        }
        else
        {
            result = oid_compare(name, *length, v->name, v->namelen);
        }
    }

    if (result > 0)
    {
        return -1;
    }
    else if (result < 0)
    {
        memcpy(name, v->name, ((int)v->namelen) * sizeof(oid));
    }

    if (1 == get_idx)
    {
        if (0 != str_index_val_len &&  str_index_val_len < oct_maxlen)
        {
            oid2bytes(name + v->namelen + str_offset, str_index_val_len, oct_string);
        }
    }

    return 0;
}

void
ipran_snmp_octstring_index_set(struct variable *v, oid *name, size_t *length,
                               u_char *oct_string, int oct_len)
{
    name[v->namelen] = oct_len;
    oid_copy_bytes2oid(name + v->namelen + 1, oct_string, oct_len);
    *length = v->namelen + oct_len + 1;
}

// octstring index IMPLICIT
//   oct_maxlen is oct_string max byte length
int
ipran_snmp_octstring_index_with_implicit_get(struct variable *v, oid *name, size_t *length,
                                             u_char *oct_string, int oct_maxlen, int exact)
{
    int result, len;

    len    = (*length > v->namelen) ? v->namelen : *length;
    result = oid_compare(name, len, v->name, len);

    // init oct_string
    memset(oct_string, 0x00, oct_maxlen);

    if (exact)
    {
        if (result != 0 || *length - v->namelen < 1)
        {
            return -1;
        }

        len = *length - v->namelen;
        oid2bytes(name + v->namelen, len, oct_string);

        return 0;
    }
    else
    {
        if (result > 0)
        {
            return -1;
        }
        else if (0 == result)
        {
            if (*length - v->namelen > 0)
            {
                len = *length - v->namelen;
                oid2bytes(name + v->namelen, len, oct_string);
            }

            return 0;
        }
        else
        {
            // copy v->name to name
            oid_copy(name, v->name, v->namelen);

            return 0;
        }
    }

    return -1;
}

void
ipran_snmp_octstring_index_with_implicit_set(struct variable *v, oid *name, size_t *length,
                                             u_char *oct_string, int oct_len)
{
    oid_copy_bytes2oid(name + v->namelen, oct_string, oct_len);
    *length = v->namelen + oct_len;
}

/* Utility function to get TWO integer indices.  */
int ipran_snmp_intx2_index_get(struct variable *v, oid *name, size_t *length,
                               u_int32_t *index1, u_int32_t *index2, int exact)
{
    int         OID_IDX_LEN = 2;
    int         result;
    int         get_idx;

    get_idx     = 0;

    *index1     = 0;
    *index2     = 0;

    if (exact)
    {
        // rx oid length != reg oid length + index len
        if (*length != v->namelen + OID_IDX_LEN)
        {
            return -1;
        }

        // compare reg oid length
        result = oid_compare(name, v->namelen, v->name, v->namelen);

        if (result != 0)
        {
            return -1;
        }

        get_idx  = 1;
    }
    else
    {
        if (*length >= v->namelen)
        {
            result = oid_compare(name, v->namelen, v->name, v->namelen);

            if (result == 0 && *length > v->namelen)
            {
                get_idx  = 1;
            }
        }
        else
        {
            result = oid_compare(name, *length, v->name, v->namelen);
        }
    }

    if (result > 0)
    {
        return -1;
    }
    else if (result < 0)
    {
        memcpy(name, v->name, ((int)v->namelen) * sizeof(oid));
    }

    if (1 == get_idx)
    {
        *index1 = (*length > v->namelen) ? name[v->namelen] : 0;
        *index2 = (*length > v->namelen + 1) ? name[v->namelen + 1] : 0;

        return 0;
    }

    return 1;
}

/* Utility function to set the two INTEGER indices. */
void ipran_snmp_intx2_index_set(struct variable *v, oid *name, size_t *length,
                                u_int32_t index1, u_int32_t index2)
{
    name[v->namelen] = index1;
    name[v->namelen + 1] = index2;
    *length = v->namelen + 2;
}

/* Utility function to get THREE integer indices.  */
int ipran_snmp_intx3_index_get(struct variable *v, oid *name, size_t *length,
                               u_int32_t *index1, u_int32_t *index2,
                               u_int32_t *index3, int exact)
{
    int         OID_IDX_LEN = 3;
    int         result;
    int         get_idx;

    get_idx     = 0;

    *index1     = 0;
    *index2     = 0;
    *index3     = 0;

    if (exact)
    {
        // rx oid length != reg oid length + index len
        if (*length != v->namelen + OID_IDX_LEN)
        {
            return -1;
        }

        // compare reg oid length
        result = oid_compare(name, v->namelen, v->name, v->namelen);

        if (result != 0)
        {
            return -1;
        }

        get_idx  = 1;
    }
    else
    {
        if (*length >= v->namelen)
        {
            result = oid_compare(name, v->namelen, v->name, v->namelen);

            if (result == 0 && *length > v->namelen)
            {
                get_idx  = 1;
            }
        }
        else
        {
            result = oid_compare(name, *length, v->name, v->namelen);
        }
    }

    if (result > 0)
    {
        return -1;
    }
    else if (result < 0)
    {
        memcpy(name, v->name, ((int)v->namelen) * sizeof(oid));
    }

    if (1 == get_idx)
    {
        *index1 = (*length > v->namelen) ? name[v->namelen] : 0;
        *index2 = (*length > v->namelen + 1) ? name[v->namelen + 1] : 0;
        *index3 = (*length > v->namelen + 2) ? name[v->namelen + 2] : 0;

        return 0;
    }

    return 1;
}

/* API for setting three INTEGER indices. */
void ipran_snmp_intx3_index_set(struct variable *v, oid *name, size_t *length,
                                u_int32_t index1, u_int32_t index2, u_int32_t index3)
{
    name[v->namelen] = index1;
    name[v->namelen + 1] = index2;
    name[v->namelen + 2] = index3;
    *length = v->namelen + 3;
}

/* Utility function to get FOUR integer indices.  */
int
ipran_snmp_intx4_index_get(struct variable *v, oid *name, size_t *length,
                           u_int32_t *index1, u_int32_t *index2, u_int32_t *index3,
                           u_int32_t *index4, int exact)
{
    int         OID_IDX_LEN = 4;
    int         result;
    int         get_idx;

    get_idx     = 0;

    *index1     = 0;
    *index2     = 0;
    *index3     = 0;
    *index4     = 0;

    if (exact)
    {
        // rx oid length != reg oid length + index len
        if (*length != v->namelen + OID_IDX_LEN)
        {
            return -1;
        }

        // compare reg oid length
        result = oid_compare(name, v->namelen, v->name, v->namelen);

        if (result != 0)
        {
            return -1;
        }

        get_idx  = 1;
    }
    else
    {
        if (*length >= v->namelen)
        {
            result = oid_compare(name, v->namelen, v->name, v->namelen);

            if (result == 0 && *length == v->namelen + OID_IDX_LEN)
            {
                get_idx  = 1;
            }
        }
        else
        {
            result = oid_compare(name, *length, v->name, v->namelen);
        }
    }

    if (result > 0)
    {
        return -1;
    }
    else if (result < 0)
    {
        memcpy(name, v->name, ((int)v->namelen) * sizeof(oid));
    }

    if (1 == get_idx)
    {
        *index1     = name[v->namelen];
        *index2     = name[v->namelen + 1];
        *index3     = name[v->namelen + 2];
        *index4     = name[v->namelen + 3];
    }

    return 0;
}

/* API for setting four INTEGER indices. */
void
ipran_snmp_intx4_index_set(struct variable *v, oid *name, size_t *length,
                           u_int32_t index1, u_int32_t index2,
                           u_int32_t index3, u_int32_t index4)
{
    name[v->namelen] = index1;
    name[v->namelen + 1] = index2;
    name[v->namelen + 2] = index3;
    name[v->namelen + 3] = index4;
    *length = v->namelen + 4;
}

/* Utility function to get FIVE integer indices.  */
int ipran_snmp_intx5_index_get(struct variable *v, oid *name, size_t *length,
                               u_int32_t *index1, u_int32_t *index2,
                               u_int32_t *index3, u_int32_t *index4,
                               u_int32_t *index5, int exact)
{
    int         OID_IDX_LEN = 5;
    int         result;
    int         get_idx;

    get_idx     = 0;

    *index1     = 0;
    *index2     = 0;
    *index3     = 0;
    *index4     = 0;
    *index5     = 0;

    if (exact)
    {
        // rx oid length != reg oid length + index len
        if (*length != v->namelen + OID_IDX_LEN)
        {
            return -1;
        }

        // compare reg oid length
        result = oid_compare(name, v->namelen, v->name, v->namelen);

        if (result != 0)
        {
            return -1;
        }

        get_idx  = 1;
    }
    else
    {
        if (*length >= v->namelen)
        {
            result = oid_compare(name, v->namelen, v->name, v->namelen);

            if (result == 0 && *length > v->namelen)
            {
                get_idx  = 1;
            }
        }
        else
        {
            result = oid_compare(name, *length, v->name, v->namelen);
        }
    }

    if (result > 0)
    {
        return -1;
    }
    else if (result < 0)
    {
        memcpy(name, v->name, ((int)v->namelen) * sizeof(oid));
    }

    if (1 == get_idx)
    {
        *index1 = (*length > v->namelen) ? name[v->namelen] : 0;
        *index2 = (*length > v->namelen + 1) ? name[v->namelen + 1] : 0;
        *index3 = (*length > v->namelen + 2) ? name[v->namelen + 2] : 0;
        *index4 = (*length > v->namelen + 3) ? name[v->namelen + 3] : 0;
        *index5 = (*length > v->namelen + 4) ? name[v->namelen + 4] : 0;

        return 0;
    }

    return 1;
}

/* API for setting five INTEGER indices. */
void ipran_snmp_intx5_index_set(struct variable *v, oid *name, size_t *length,
                                u_int32_t index1, u_int32_t index2,
                                u_int32_t index3, u_int32_t index4,
                                u_int32_t index5)

{
    name[v->namelen] = index1;
    name[v->namelen + 1] = index2;
    name[v->namelen + 2] = index3;
    name[v->namelen + 3] = index4;
    name[v->namelen + 4] = index5;
    *length = v->namelen + 5;
}

/* Utility function to get  one ip indices.  */
int ipran_snmp_intx6_index_get(struct variable *v, oid *name, size_t *length,
                               u_int32_t *index1, u_int32_t *index2,
                               u_int32_t *index3, u_int32_t *index4,
                               u_int32_t *index5, u_int32_t *index6, int exact)
{
    int         OID_IDX_LEN = 6;
    int         result;
    int         get_idx;

    get_idx     = 0;

    *index1     = 0;
    *index2     = 0;
    *index3     = 0;
    *index4     = 0;
    *index5     = 0;
    *index6     = 0;

    if (exact)
    {
        // rx oid length != reg oid length + index len
        if (*length != v->namelen + OID_IDX_LEN)
        {
            return -1;
        }

        // compare reg oid length
        result = oid_compare(name, v->namelen, v->name, v->namelen);

        if (result != 0)
        {
            return -1;
        }

        get_idx  = 1;
    }
    else
    {
        if (*length >= v->namelen)
        {
            result = oid_compare(name, v->namelen, v->name, v->namelen);

            if (result == 0 && *length > v->namelen)
            {
                get_idx  = 1;
            }
        }
        else
        {
            result = oid_compare(name, *length, v->name, v->namelen);
        }
    }

    if (result > 0)
    {
        return -1;
    }
    else if (result < 0)
    {
        memcpy(name, v->name, ((int)v->namelen) * sizeof(oid));
    }

    if (1 == get_idx)
    {
        *index1 = (*length > v->namelen) ? name[v->namelen] : 0;
        *index2 = (*length > v->namelen + 1) ? name[v->namelen + 1] : 0;
        *index3 = (*length > v->namelen + 2) ? name[v->namelen + 2] : 0;
        *index4 = (*length > v->namelen + 3) ? name[v->namelen + 3] : 0;
        *index5 = (*length > v->namelen + 4) ? name[v->namelen + 4] : 0;
        *index6 = (*length > v->namelen + 5) ? name[v->namelen + 5] : 0;

        return 0;
    }

    return 1;
}

/* API for setting five INTEGER indices. */
void
ipran_snmp_intx6_index_set(struct variable *v, oid *name, size_t *length,
                           u_int32_t index1, u_int32_t index2,
                           u_int32_t index3, u_int32_t index4,
                           u_int32_t index5, u_int32_t index6)

{
    name[v->namelen] = index1;
    name[v->namelen + 1] = index2;
    name[v->namelen + 2] = index3;
    name[v->namelen + 3] = index4;
    name[v->namelen + 4] = index5;
    name[v->namelen + 5] = index6;
    *length = v->namelen + 6;
}


/* Utility function to get FIVE integer indices.    */
int
ipran_snmp_intx7_index_get(struct variable *v, oid *name, size_t *length,
                           u_int32_t *index1, u_int32_t *index2,
                           u_int32_t *index3, u_int32_t *index4,
                           u_int32_t *index5, u_int32_t *index6,
                           u_int32_t *index7, int exact)
{
    int         OID_IDX_LEN = 7;
    int         result;
    int         get_idx;

    get_idx     = 0;

    *index1     = 0;
    *index2     = 0;
    *index3     = 0;
    *index4     = 0;
    *index5     = 0;
    *index6     = 0;
    *index7     = 0;

    if (exact)
    {
        // rx oid length != reg oid length + index len
        if (*length != v->namelen + OID_IDX_LEN)
        {
            return -1;
        }

        // compare reg oid length
        result = oid_compare(name, v->namelen, v->name, v->namelen);

        if (result != 0)
        {
            return -1;
        }

        get_idx  = 1;
    }
    else
    {
        if (*length >= v->namelen)
        {
            result = oid_compare(name, v->namelen, v->name, v->namelen);

            if (result == 0 && *length == v->namelen + OID_IDX_LEN)
            {
                get_idx  = 1;
            }
        }
        else
        {
            result = oid_compare(name, *length, v->name, v->namelen);
        }
    }

    if (result > 0)
    {
        return -1;
    }
    else if (result < 0)
    {
        memcpy(name, v->name, ((int)v->namelen) * sizeof(oid));
    }

    if (1 == get_idx)
    {
        *index1     = name[v->namelen];
        *index2     = name[v->namelen + 1];
        *index3     = name[v->namelen + 2];
        *index4     = name[v->namelen + 3];
        *index5     = name[v->namelen + 4];
        *index6     = name[v->namelen + 5];
        *index7     = name[v->namelen + 6];
    }

    return 0;
}

/* API for setting five INTEGER indices. */
void
ipran_snmp_intx7_index_set(struct variable *v, oid *name, size_t *length,
                           u_int32_t index1, u_int32_t index2,
                           u_int32_t index3, u_int32_t index4,
                           u_int32_t index5, u_int32_t index6,
                           u_int32_t index7)

{
    name[v->namelen] = index1;
    name[v->namelen + 1] = index2;
    name[v->namelen + 2] = index3;
    name[v->namelen + 3] = index4;
    name[v->namelen + 4] = index5;
    name[v->namelen + 5] = index6;
    name[v->namelen + 6] = index7;
    *length = v->namelen + 7;
}


/* Utility function to get  one ip indices.  */
int
ipran_snmp_ip_index_get(struct variable *v, oid *name, size_t *length,
                        u_int32_t *ipvalue,  int exact)
{
    int         OID_IDX_LEN = 4;
    int         result;
    int         get_idx;

    int ip1_value1 = 0;
    int ip1_value2 = 0;
    int ip1_value3 = 0;
    int ip1_value4 = 0;
    u_int32_t ip_host = 0 ;

    get_idx     = 0;
    *ipvalue = 0;

    if (exact)
    {
        // rx oid length != reg oid length + index len
        if (*length != v->namelen + OID_IDX_LEN)
        {
            return -1;
        }

        // compare reg oid length
        result = oid_compare(name, v->namelen, v->name, v->namelen);

        if (result != 0)
        {
            return -1;
        }

        get_idx  = 1;
    }
    else
    {
        if (*length >= v->namelen)
        {
            result = oid_compare(name, v->namelen, v->name, v->namelen);

            if (result == 0 && *length == v->namelen + OID_IDX_LEN)
            {
                get_idx  = 1;
            }
        }
        else
        {
            result = oid_compare(name, *length, v->name, v->namelen);
        }
    }

    if (result > 0)
    {
        return -1;
    }
    else if (result < 0)
    {
        memcpy(name, v->name, ((int)v->namelen) * sizeof(oid));
    }

    if (1 == get_idx)
    {
        ip1_value1 = name[v->namelen];
        ip1_value2 = name[v->namelen + 1];
        ip1_value3 = name[v->namelen + 2];
        ip1_value4 = name[v->namelen + 3];
        ip_host = ((ip1_value1 << 24) & 0xff000000) |
                  ((ip1_value2 << 16) & 0x00ff0000) |
                  ((ip1_value3 << 8) & 0x0000ff00) |
                  ((ip1_value4)  & 0x000000ff) ;

        *ipvalue  = htonl(ip_host);

        return 0;
    }

    return 1;
}

/* Utility function to set  one ip indices.  */
void
ipran_snmp_ip_index_set(struct variable *v, oid *name, size_t *length,
                        u_int32_t ip_value)

{
    ip_value = ntohl(ip_value);

    name[v->namelen] = (ip_value >> 24) & 0xff;
    name[v->namelen + 1] = (ip_value >> 16) & 0xff;
    name[v->namelen + 2] = (ip_value >> 8) & 0xff;
    name[v->namelen + 3] = ip_value & 0xff;

    *length = v->namelen + 4;
}


/* Utility function to get  two ip indices.  */
int
ipran_snmp_ip2_index_get(struct variable *v, oid *name, size_t *length,
                         u_int32_t *ipvalue1, uint32_t *ipvalue2, int exact)
{
    int         OID_IDX_LEN = 8;
    int         result;
    int         get_idx;

    int ip1_value1 = 0;
    int ip1_value2 = 0;
    int ip1_value3 = 0;
    int ip1_value4 = 0;
    u_int32_t ip_host = 0 ;

    get_idx     = 0;
    *ipvalue1 = 0;

    if (exact)
    {
        // rx oid length != reg oid length + index len
        if (*length != v->namelen + OID_IDX_LEN)
        {
            return -1;
        }

        // compare reg oid length
        result = oid_compare(name, v->namelen, v->name, v->namelen);

        if (result != 0)
        {
            return -1;
        }

        get_idx  = 1;
    }
    else
    {
        if (*length >= v->namelen)
        {
            result = oid_compare(name, v->namelen, v->name, v->namelen);

            if (result == 0 && *length == v->namelen + OID_IDX_LEN)
            {
                get_idx  = 1;
            }
        }
        else
        {
            result = oid_compare(name, *length, v->name, v->namelen);
        }
    }

    if (result > 0)
    {
        return -1;
    }
    else if (result < 0)
    {
        memcpy(name, v->name, ((int)v->namelen) * sizeof(oid));
    }

    if (1 == get_idx)
    {
        ip1_value1 = name[v->namelen];
        ip1_value2 = name[v->namelen + 1];
        ip1_value3 = name[v->namelen + 2];
        ip1_value4 = name[v->namelen + 3];
        ip_host = ((ip1_value1 << 24) & 0xff000000) |
                  ((ip1_value2 << 16) & 0x00ff0000) |
                  ((ip1_value3 << 8) & 0x0000ff00) |
                  ((ip1_value4)  & 0x000000ff) ;

        *ipvalue1  = htonl(ip_host);

        ip1_value1 = name[v->namelen + 4];
        ip1_value2 = name[v->namelen + 5];
        ip1_value3 = name[v->namelen + 6];
        ip1_value4 = name[v->namelen + 7];
        ip_host = ((ip1_value1 << 24) & 0xff000000) |
                  ((ip1_value2 << 16) & 0x00ff0000) |
                  ((ip1_value3 << 8) & 0x0000ff00) |
                  ((ip1_value4)  & 0x000000ff) ;
        *ipvalue2  = htonl(ip_host);

    }

    return 0;
}



/* Utility function to set  two ip indices.  */
void
ipran_snmp_ip2_index_set(struct variable *v, oid *name, size_t *length,
                         u_int32_t ip_value1, uint32_t ip_value2)

{
    ip_value1 = ntohl(ip_value1);

    name[v->namelen] = (ip_value1 >> 24) & 0xff;
    name[v->namelen + 1] = (ip_value1 >> 16) & 0xff;
    name[v->namelen + 2] = (ip_value1 >> 8) & 0xff;
    name[v->namelen + 3] = ip_value1 & 0xff;

    ip_value2 = ntohl(ip_value2);

    name[v->namelen + 4] = (ip_value2 >> 24) & 0xff;
    name[v->namelen + 5] = (ip_value2 >> 16) & 0xff;
    name[v->namelen + 6] = (ip_value2 >> 8) & 0xff;
    name[v->namelen + 7] = ip_value2 & 0xff;

    *length = v->namelen + 8;
}


/* Utility function to get one integer one ip indices.  */
int
ipran_snmp_ip_int_index_get(struct variable *v, oid *name, size_t *length,
                            u_int32_t *ipvalue,  u_int32_t *index , int exact)
{
    int         OID_IDX_LEN = 5;
    int         result;
    int         get_idx;

    int ip1_value1 = 0;
    int ip1_value2 = 0;
    int ip1_value3 = 0;
    int ip1_value4 = 0;

    u_int32_t ip_host = 0 ;

    get_idx     = 0;
    *index = 0;

    if (exact)
    {
        // rx oid length != reg oid length + index len
        if (*length != v->namelen + OID_IDX_LEN)
        {
            return -1;
        }

        // compare reg oid length
        result = oid_compare(name, v->namelen, v->name, v->namelen);

        if (result != 0)
        {
            return -1;
        }

        get_idx  = 1;
    }
    else
    {
        if (*length >= v->namelen)
        {
            result = oid_compare(name, v->namelen, v->name, v->namelen);

            if (result == 0 && *length == v->namelen + OID_IDX_LEN)
            {
                get_idx  = 1;
            }
        }
        else
        {
            result = oid_compare(name, *length, v->name, v->namelen);
        }
    }

    if (result > 0)
    {
        return -1;
    }
    else if (result < 0)
    {
        memcpy(name, v->name, ((int)v->namelen) * sizeof(oid));
    }

    if (1 == get_idx)
    {
        ip1_value1 = name[v->namelen + 0];
        ip1_value2 = name[v->namelen + 1];
        ip1_value3 = name[v->namelen + 2];
        ip1_value4 = name[v->namelen + 3];
        ip_host = ((ip1_value1 << 24) & 0xff000000) |
                  ((ip1_value2 << 16) & 0x00ff0000) |
                  ((ip1_value3 << 8) & 0x0000ff00) |
                  ((ip1_value4) & 0x000000ff) ;

        *ipvalue = htonl(ip_host);
        *index   = name[v->namelen + 4];

        return 0;
    }

    return 1;
}

/* Utility function to set one integer one ip indices.  */
void
ipran_snmp_ip_int_index_set(struct variable *v, oid *name, size_t *length,
                            u_int32_t ip_value1 , u_int32_t index)

{
    ip_value1 = ntohl(ip_value1);

    name[v->namelen] = (ip_value1 >> 24) & 0xff;
    name[v->namelen + 1] = (ip_value1 >> 16) & 0xff;
    name[v->namelen + 2] = (ip_value1 >> 8) & 0xff;
    name[v->namelen + 3] = ip_value1 & 0xff;

    name[v->namelen + 4] = index;

    *length = v->namelen + 5;
}

/* Utility function to get one integer one ip indices.  */
int
ipran_snmp_int_ip_index_get(struct variable *v, oid *name, size_t *length,
                            u_int32_t *index, u_int32_t *ipvalue,  int exact)
{
    int         OID_IDX_LEN = 5;
    int         result;
    int         get_idx;

    int ip1_value1 = 0;
    int ip1_value2 = 0;
    int ip1_value3 = 0;
    int ip1_value4 = 0;

    u_int32_t ip_host = 0 ;

    get_idx     = 0;
    *index = 0;

    if (exact)
    {
        // rx oid length != reg oid length + index len
        if (*length != v->namelen + OID_IDX_LEN)
        {
            return -1;
        }

        // compare reg oid length
        result = oid_compare(name, v->namelen, v->name, v->namelen);

        if (result != 0)
        {
            return -1;
        }

        get_idx  = 1;
    }
    else
    {
        if (*length >= v->namelen)
        {
            result = oid_compare(name, v->namelen, v->name, v->namelen);

            if (result == 0 && *length == v->namelen + OID_IDX_LEN)
            {
                get_idx  = 1;
            }
        }
        else
        {
            result = oid_compare(name, *length, v->name, v->namelen);
        }
    }

    if (result > 0)
    {
        return -1;
    }
    else if (result < 0)
    {
        memcpy(name, v->name, ((int)v->namelen) * sizeof(oid));
    }

    if (1 == get_idx)
    {
        *index = name[v->namelen];
        ip1_value1 = name[v->namelen + 1];
        ip1_value2 = name[v->namelen + 2];
        ip1_value3 = name[v->namelen + 3];
        ip1_value4 = name[v->namelen + 4];

        ip_host = ((ip1_value1 << 24) & 0xff000000) |
                  ((ip1_value2 << 16) & 0x00ff0000) |
                  ((ip1_value3 << 8) & 0x0000ff00) |
                  ((ip1_value4)  & 0x000000ff) ;

        *ipvalue  = htonl(ip_host);
    }

    return 0;
}

/* Utility function to set one integer one ip indices.  */
void
ipran_snmp_int_ip_index_set(struct variable *v, oid *name, size_t *length,
                            u_int32_t index, u_int32_t ip_value1)

{
    name[v->namelen] = index;
    ip_value1 = ntohl(ip_value1);

    name[v->namelen + 1] = (ip_value1 >> 24) & 0xff;
    name[v->namelen + 2] = (ip_value1 >> 16) & 0xff;
    name[v->namelen + 3] = (ip_value1 >> 8) & 0xff;
    name[v->namelen + 4] = ip_value1 & 0xff;

    *length = v->namelen + 5;
}

/* Utility function to get one integer two ip indices.  */
int
ipran_snmp_int_ipx2_index_get(struct variable *v, oid *name, size_t *length,
                              u_int32_t *index, u_int32_t *ipvalue1, u_int32_t *ipvalue2, int exact)
{
    int         OID_IDX_LEN = 9;
    int         result;
    int         get_idx;

    int ip1_value1 = 0;
    int ip1_value2 = 0;
    int ip1_value3 = 0;
    int ip1_value4 = 0;

    int ip2_value1 = 0;
    int ip2_value2 = 0;
    int ip2_value3 = 0;
    int ip2_value4 = 0;

    u_int32_t ip_host = 0;

    get_idx     = 0;
    *index = 0;

    if (exact)
    {
        // rx oid length != reg oid length + index len
        if (*length != v->namelen + OID_IDX_LEN)
        {
            return -1;
        }

        // compare reg oid length
        result = oid_compare(name, v->namelen, v->name, v->namelen);

        if (result != 0)
        {
            return -1;
        }

        get_idx  = 1;
    }
    else
    {
        if (*length >= v->namelen)
        {
            result = oid_compare(name, v->namelen, v->name, v->namelen);

            if (result == 0 && *length == v->namelen + OID_IDX_LEN)
            {
                get_idx  = 1;
            }
        }
        else
        {
            result = oid_compare(name, *length, v->name, v->namelen);
        }
    }

    if (result > 0)
    {
        return -1;
    }
    else if (result < 0)
    {
        memcpy(name, v->name, ((int)v->namelen) * sizeof(oid));
    }

    if (1 == get_idx)
    {
        *index = name[v->namelen];
        ip1_value1 = name[v->namelen + 1];
        ip1_value2 = name[v->namelen + 2];
        ip1_value3 = name[v->namelen + 3];
        ip1_value4 = name[v->namelen + 4];

        ip_host = ((ip1_value1 << 24) & 0xff000000) |
                  ((ip1_value2 << 16) & 0x00ff0000) |
                  ((ip1_value3 << 8) & 0x0000ff00) |
                  ((ip1_value4)  & 0x000000ff) ;

        *ipvalue1  = htonl(ip_host);

        ip2_value1 = name[v->namelen + 5];
        ip2_value2 = name[v->namelen + 6];
        ip2_value3 = name[v->namelen + 7];
        ip2_value4 = name[v->namelen + 8];

        ip_host = ((ip2_value1 << 24) & 0xff000000) |
                  ((ip2_value2 << 16) & 0x00ff0000) |
                  ((ip2_value3 << 8) & 0x0000ff00) |
                  ((ip2_value4)  & 0x000000ff) ;

        *ipvalue2  = htonl(ip_host);

        return 0;
    }

    return 1;
}

/* Utility function to set one integer two ip indices.  */
void
ipran_snmp_int_ipx2_index_set(struct variable *v, oid *name, size_t *length,
                              u_int32_t index, u_int32_t ip_value1 , u_int32_t ip_value2)

{
    name[v->namelen] = index;
    ip_value1 = ntohl(ip_value1);
    ip_value2 = ntohl(ip_value2);

    name[v->namelen + 1] = (ip_value1 >> 24) & 0xff;
    name[v->namelen + 2] = (ip_value1 >> 16) & 0xff;
    name[v->namelen + 3] = (ip_value1 >> 8) & 0xff;
    name[v->namelen + 4] = ip_value1 & 0xff;

    name[v->namelen + 5] = (ip_value2 >> 24) & 0xff;
    name[v->namelen + 6] = (ip_value2 >> 16) & 0xff;
    name[v->namelen + 7] = (ip_value2 >> 8) & 0xff;
    name[v->namelen + 8] = ip_value2 & 0xff;

    *length = v->namelen + 9;
}


/* Utility function to get one integer two ip indices.  */
int
ipran_snmp_intx2_ipx2_index_get(struct variable *v, oid *name, size_t *length,
                                u_int32_t *index, u_int32_t *index2, u_int32_t *ipvalue1, u_int32_t *ipvalue2, int exact)
{
    int         OID_IDX_LEN = 10;
    int         result;
    int         get_idx;

    int ip1_value1 = 0;
    int ip1_value2 = 0;
    int ip1_value3 = 0;
    int ip1_value4 = 0;

    int ip2_value1 = 0;
    int ip2_value2 = 0;
    int ip2_value3 = 0;
    int ip2_value4 = 0;

    u_int32_t ip_host = 0;

    get_idx     = 0;
    *index = 0;

    if (exact)
    {
        // rx oid length != reg oid length + index len
        if (*length != v->namelen + OID_IDX_LEN)
        {
            return -1;
        }

        // compare reg oid length
        result = oid_compare(name, v->namelen, v->name, v->namelen);

        if (result != 0)
        {
            return -1;
        }

        get_idx  = 1;
    }
    else
    {
        if (*length >= v->namelen)
        {
            result = oid_compare(name, v->namelen, v->name, v->namelen);

            if (result == 0 && *length == v->namelen + OID_IDX_LEN)
            {
                get_idx  = 1;
            }
        }
        else
        {
            result = oid_compare(name, *length, v->name, v->namelen);
        }
    }

    if (result > 0)
    {
        return -1;
    }
    else if (result < 0)
    {
        memcpy(name, v->name, ((int)v->namelen) * sizeof(oid));
    }

    if (1 == get_idx)
    {
        *index = name[v->namelen];
        *index2 = name[v->namelen + 1];
        ip1_value1 = name[v->namelen + 2];
        ip1_value2 = name[v->namelen + 3];
        ip1_value3 = name[v->namelen + 4];
        ip1_value4 = name[v->namelen + 5];

        ip_host = ((ip1_value1 << 24) & 0xff000000) |
                  ((ip1_value2 << 16) & 0x00ff0000) |
                  ((ip1_value3 << 8) & 0x0000ff00) |
                  ((ip1_value4)  & 0x000000ff) ;

        *ipvalue1  = htonl(ip_host);

        ip2_value1 = name[v->namelen + 6];
        ip2_value2 = name[v->namelen + 7];
        ip2_value3 = name[v->namelen + 8];
        ip2_value4 = name[v->namelen + 9];

        ip_host = ((ip2_value1 << 24) & 0xff000000) |
                  ((ip2_value2 << 16) & 0x00ff0000) |
                  ((ip2_value3 << 8) & 0x0000ff00) |
                  ((ip2_value4)  & 0x000000ff) ;

        *ipvalue2  = htonl(ip_host);

        return 0;
    }

    return 1;
}

/* Utility function to set two integer two ip indices.  */
void
ipran_snmp_intx2_ipx2_index_set(struct variable *v, oid *name, size_t *length,
                                u_int32_t index, u_int32_t index2, u_int32_t ip_value1 , u_int32_t ip_value2)

{
    name[v->namelen] = index;
    name[v->namelen + 1] = index2;
    ip_value1 = ntohl(ip_value1);
    ip_value2 = ntohl(ip_value2);

    name[v->namelen + 2] = (ip_value1 >> 24) & 0xff;
    name[v->namelen + 3] = (ip_value1 >> 16) & 0xff;
    name[v->namelen + 4] = (ip_value1 >> 8) & 0xff;
    name[v->namelen + 5] = ip_value1 & 0xff;

    name[v->namelen + 6] = (ip_value2 >> 24) & 0xff;
    name[v->namelen + 7] = (ip_value2 >> 16) & 0xff;
    name[v->namelen + 8] = (ip_value2 >> 8) & 0xff;
    name[v->namelen + 9] = ip_value2 & 0xff;

    *length = v->namelen + 10;
}


/* Utility function to get  two ip one integer one ip indices.  */
int
ipran_snmp_ipx2_int_ip_index_get(struct variable *v, oid *name, size_t *length,
                                 u_int32_t *ipvalue1, u_int32_t *ipvalue2, u_int32_t *index,  u_int32_t *ipvalue3 , int exact)
{
    int         OID_IDX_LEN = 13;
    int         result;
    int         get_idx;

    int ip1_value1 = 0;
    int ip1_value2 = 0;
    int ip1_value3 = 0;
    int ip1_value4 = 0;

    int ip2_value1 = 0;
    int ip2_value2 = 0;
    int ip2_value3 = 0;
    int ip2_value4 = 0;

    int ip3_value1 = 0;
    int ip3_value2 = 0;
    int ip3_value3 = 0;
    int ip3_value4 = 0;

    u_int32_t ip_host = 0 ;

    get_idx     = 0;
    *index = 0;

    if (exact)
    {
        // rx oid length != reg oid length + index len
        if (*length != v->namelen + OID_IDX_LEN)
        {
            return -1;
        }

        // compare reg oid length
        result = oid_compare(name, v->namelen, v->name, v->namelen);

        if (result != 0)
        {
            return -1;
        }

        get_idx  = 1;
    }
    else
    {
        if (*length >= v->namelen)
        {
            result = oid_compare(name, v->namelen, v->name, v->namelen);

            if (result == 0 && *length == v->namelen + OID_IDX_LEN)
            {
                get_idx  = 1;
            }
        }
        else
        {
            result = oid_compare(name, *length, v->name, v->namelen);
        }
    }

    if (result > 0)
    {
        return -1;
    }
    else if (result < 0)
    {
        memcpy(name, v->name, ((int)v->namelen) * sizeof(oid));
    }

    if (1 == get_idx)
    {
        ip1_value1 = name[v->namelen];
        ip1_value2 = name[v->namelen + 1];
        ip1_value3 = name[v->namelen + 2];
        ip1_value4 = name[v->namelen + 3];

        ip_host = ((ip1_value1 << 24) & 0xff000000) |
                  ((ip1_value2 << 16) & 0x00ff0000) |
                  ((ip1_value3 << 8) & 0x0000ff00) |
                  ((ip1_value4)  & 0x000000ff) ;

        *ipvalue1  = htonl(ip_host);

        ip2_value1 = name[v->namelen + 4];
        ip2_value2 = name[v->namelen + 5];
        ip2_value3 = name[v->namelen + 6];
        ip2_value4 = name[v->namelen + 7];

        ip_host = ((ip2_value1 << 24) & 0xff000000) |
                  ((ip2_value2 << 16) & 0x00ff0000) |
                  ((ip2_value3 << 8) & 0x0000ff00) |
                  ((ip2_value4)  & 0x000000ff) ;

        *ipvalue2  = htonl(ip_host);

        *index = name[v->namelen + 8 ];

        ip3_value1 = name[v->namelen];
        ip3_value2 = name[v->namelen + 10];
        ip3_value3 = name[v->namelen + 11];
        ip3_value4 = name[v->namelen + 12];

        ip_host = ((ip3_value1 << 24) & 0xff000000) |
                  ((ip3_value2 << 16) & 0x00ff0000) |
                  ((ip3_value3 << 8) & 0x0000ff00) |
                  ((ip3_value4)  & 0x000000ff) ;

        *ipvalue3  = htonl(ip_host);

        return 0;
    }

    return 1;
}

/* Utility function to set  two ip one integer one ip indices.  */
void
ipran_snmp_ipx2_int_ip_index_set(struct variable *v, oid *name, size_t *length,
                                 u_int32_t ip_value1 , u_int32_t ip_value2 , u_int32_t index, u_int32_t ip_value3)

{
    ip_value1 = ntohl(ip_value1);
    ip_value2 = ntohl(ip_value2);
    ip_value3 = ntohl(ip_value3);

    name[v->namelen] = (ip_value1 >> 24) & 0xff;
    name[v->namelen + 1] = (ip_value1 >> 16) & 0xff;
    name[v->namelen + 2] = (ip_value1 >> 8) & 0xff;
    name[v->namelen + 3] = ip_value1 & 0xff;

    name[v->namelen + 4] = (ip_value2 >> 24) & 0xff;
    name[v->namelen + 5] = (ip_value2 >> 16) & 0xff;
    name[v->namelen + 6] = (ip_value2 >> 8) & 0xff;
    name[v->namelen + 7] = ip_value2 & 0xff;

    name[v->namelen +  8] = index;

    name[v->namelen + 9] = (ip_value3 >> 24) & 0xff;
    name[v->namelen + 10] = (ip_value3 >> 16) & 0xff;
    name[v->namelen + 11] = (ip_value3 >> 8) & 0xff;
    name[v->namelen + 12] = ip_value3 & 0xff;


    *length = v->namelen + 13;
}

/* Utility function to get  two ip one integer one ip one int indices.  */
int
ipran_snmp_ipx2_int_ip_int_index_get(struct variable *v, oid *name, size_t *length,
                                     u_int32_t *ipvalue1, u_int32_t *ipvalue2, u_int32_t *index,  u_int32_t *ipvalue3 , u_int32_t *index2, int exact)
{
    int         OID_IDX_LEN = 14;
    int         result;
    int         get_idx;

    int ip1_value1 = 0;
    int ip1_value2 = 0;
    int ip1_value3 = 0;
    int ip1_value4 = 0;

    int ip2_value1 = 0;
    int ip2_value2 = 0;
    int ip2_value3 = 0;
    int ip2_value4 = 0;

    int ip3_value1 = 0;
    int ip3_value2 = 0;
    int ip3_value3 = 0;
    int ip3_value4 = 0;

    u_int32_t ip_host = 0 ;

    get_idx     = 0;
    *index = 0;

    if (exact)
    {
        // rx oid length != reg oid length + index len
        if (*length != v->namelen + OID_IDX_LEN)
        {
            return -1;
        }

        // compare reg oid length
        result = oid_compare(name, v->namelen, v->name, v->namelen);

        if (result != 0)
        {
            return -1;
        }

        get_idx  = 1;
    }
    else
    {
        if (*length >= v->namelen)
        {
            result = oid_compare(name, v->namelen, v->name, v->namelen);

            if (result == 0 && *length == v->namelen + OID_IDX_LEN)
            {
                get_idx  = 1;
            }
        }
        else
        {
            result = oid_compare(name, *length, v->name, v->namelen);
        }
    }

    if (result > 0)
    {
        return -1;
    }
    else if (result < 0)
    {
        memcpy(name, v->name, ((int)v->namelen) * sizeof(oid));
    }

    if (1 == get_idx)
    {
        ip1_value1 = name[v->namelen];
        ip1_value2 = name[v->namelen + 1];
        ip1_value3 = name[v->namelen + 2];
        ip1_value4 = name[v->namelen + 3];

        ip_host = ((ip1_value1 << 24) & 0xff000000) |
                  ((ip1_value2 << 16) & 0x00ff0000) |
                  ((ip1_value3 << 8) & 0x0000ff00) |
                  ((ip1_value4)  & 0x000000ff) ;

        *ipvalue1  = htonl(ip_host);

        ip2_value1 = name[v->namelen + 4];
        ip2_value2 = name[v->namelen + 5];
        ip2_value3 = name[v->namelen + 6];
        ip2_value4 = name[v->namelen + 7];

        ip_host = ((ip2_value1 << 24) & 0xff000000) |
                  ((ip2_value2 << 16) & 0x00ff0000) |
                  ((ip2_value3 << 8) & 0x0000ff00) |
                  ((ip2_value4)  & 0x000000ff) ;

        *ipvalue2  = htonl(ip_host);

        *index = name[v->namelen + 8 ];

        ip3_value1 = name[v->namelen + 9];
        ip3_value2 = name[v->namelen + 10];
        ip3_value3 = name[v->namelen + 11];
        ip3_value4 = name[v->namelen + 12];

        ip_host = ((ip3_value1 << 24) & 0xff000000) |
                  ((ip3_value2 << 16) & 0x00ff0000) |
                  ((ip3_value3 << 8) & 0x0000ff00) |
                  ((ip3_value4)  & 0x000000ff) ;

        *ipvalue3  = htonl(ip_host);

        *index2 = name[v->namelen + 13 ];

        return 0;
    }

    return 1;
}

/* Utility function to set  two ip one integer one ip indices.  */
void
ipran_snmp_ipx2_int_ip_int_index_set(struct variable *v, oid *name, size_t *length,
                                     u_int32_t ip_value1 , u_int32_t ip_value2 , u_int32_t index, u_int32_t ip_value3 , u_int32_t index2)

{
    ip_value1 = ntohl(ip_value1);
    ip_value2 = ntohl(ip_value2);
    ip_value3 = ntohl(ip_value3);

    name[v->namelen] = (ip_value1 >> 24) & 0xff;
    name[v->namelen + 1] = (ip_value1 >> 16) & 0xff;
    name[v->namelen + 2] = (ip_value1 >> 8) & 0xff;
    name[v->namelen + 3] = ip_value1 & 0xff;

    name[v->namelen + 4] = (ip_value2 >> 24) & 0xff;
    name[v->namelen + 5] = (ip_value2 >> 16) & 0xff;
    name[v->namelen + 6] = (ip_value2 >> 8) & 0xff;
    name[v->namelen + 7] = ip_value2 & 0xff;

    name[v->namelen + 8] = index;

    name[v->namelen + 9] = (ip_value3 >> 24) & 0xff;
    name[v->namelen + 10] = (ip_value3 >> 16) & 0xff;
    name[v->namelen + 11] = (ip_value3 >> 8) & 0xff;
    name[v->namelen + 12] = ip_value3 & 0xff;

    name[v->namelen + 13] = index2;

    *length = v->namelen + 14;
}


int
ipran_snmp_intx2_ip_int_index_get(struct variable *v, oid *name, size_t *length,
                                  u_int32_t *index1, u_int32_t *index2,
                                  u_int32_t *ipvalue1, u_int32_t *index3, int exact)
{
    int         OID_IDX_LEN = 7;
    int         result;
    int         get_idx;
    uint8_t     ip[4];
    u_int32_t   *ip_host;

    get_idx     = 0;

    *index1     = 0;
    *index2     = 0;
    *index3     = 0;
    *ipvalue1   = 0;

    if (exact)
    {
        // rx oid length != reg oid length + index len
        if (*length != v->namelen + OID_IDX_LEN)
        {
            return -1;
        }

        // compare reg oid length
        result = oid_compare(name, v->namelen, v->name, v->namelen);

        if (result != 0)
        {
            return -1;
        }

        get_idx  = 1;
    }
    else
    {
        if (*length >= v->namelen)
        {
            result = oid_compare(name, v->namelen, v->name, v->namelen);

            if (result == 0 && *length == v->namelen + OID_IDX_LEN)
            {
                get_idx  = 1;
            }
        }
        else
        {
            result = oid_compare(name, *length, v->name, v->namelen);
        }
    }

    if (result > 0)
    {
        return -1;
    }
    else if (result < 0)
    {
        memcpy(name, v->name, ((int)v->namelen) * sizeof(oid));
    }

    if (1 == get_idx)
    {
        *index1     = name[v->namelen];
        *index2     = name[v->namelen + 1];
        ip[0]       = name[v->namelen + 2];
        ip[1]       = name[v->namelen + 3];
        ip[2]       = name[v->namelen + 4];
        ip[3]       = name[v->namelen + 5];
        *index3     = name[v->namelen + 6];

        ip_host     = (u_int32_t *)ip;
        *ipvalue1  = *ip_host;
    }

    return 0;
}


/* API for setting five INTEGER indices. */
void
ipran_snmp_intx2_ip_int_index_set(struct variable *v, oid *name, size_t *length,
                                  u_int32_t index1, u_int32_t index2, u_int32_t ip_value1 , u_int32_t index3)

{
    name[v->namelen] = index1;
    name[v->namelen + 1] = index2;

    ip_value1 = ntohl(ip_value1);

    name[v->namelen + 2] = (ip_value1 >> 24) & 0xff;
    name[v->namelen + 3] = (ip_value1 >> 16) & 0xff;
    name[v->namelen + 4] = (ip_value1 >> 8) & 0xff;
    name[v->namelen + 5] = ip_value1 & 0xff;

    name[v->namelen + 6] = index3;

    *length = v->namelen + 7;
}


int
ipran_snmp_ip_intx2_ip_index_get(struct variable *v, oid *name, size_t *length,
                                 u_int32_t *ipvalue1, u_int32_t *index1,
                                 u_int32_t *index2, u_int32_t *ipvalue2, int exact)
{
    int         OID_IDX_LEN = 10;
    int         result;
    int         get_idx;
    uint8_t     ip[4];
    u_int32_t   *ip_host;
    uint8_t     ip2[4];
    u_int32_t   *ip_host2;

    get_idx     = 0;

    *ipvalue1   = 0;
    *index1     = 0;
    *index2     = 0;
    *ipvalue2   = 0;

    if (exact)
    {
        // rx oid length != reg oid length + index len
        if (*length != v->namelen + OID_IDX_LEN)
        {
            return -1;
        }

        // compare reg oid length
        result = oid_compare(name, v->namelen, v->name, v->namelen);

        if (result != 0)
        {
            return -1;
        }

        get_idx  = 1;
    }
    else
    {
        if (*length >= v->namelen)
        {
            result = oid_compare(name, v->namelen, v->name, v->namelen);

            if (result == 0 && *length == v->namelen + OID_IDX_LEN)
            {
                get_idx  = 1;
            }
        }
        else
        {
            result = oid_compare(name, *length, v->name, v->namelen);
        }
    }

    if (result > 0)
    {
        return -1;
    }
    else if (result < 0)
    {
        memcpy(name, v->name, ((int)v->namelen) * sizeof(oid));
    }

    if (1 == get_idx)
    {
        ip[0]       = name[v->namelen];
        ip[1]       = name[v->namelen + 1];
        ip[2]       = name[v->namelen + 2];
        ip[3]       = name[v->namelen + 3];
        ip_host     = (u_int32_t *)ip;
        *ipvalue1   = *ip_host;

        *index1     = name[v->namelen + 4];
        *index2     = name[v->namelen + 5];

        ip2[0]      = name[v->namelen + 6];
        ip2[1]      = name[v->namelen + 7];
        ip2[2]      = name[v->namelen + 8];
        ip2[3]      = name[v->namelen + 9];
        ip_host2    = (u_int32_t *)ip2;
        *ipvalue2   = *ip_host2;

        return 0;
    }

    return 1;
}


/* API for setting five INTEGER indices. */
void
ipran_snmp_ip_intx2_ip_index_set(struct variable *v, oid *name, size_t *length,
                                 u_int32_t ip_value1, u_int32_t index1, u_int32_t index2 , u_int32_t ip_value2)

{
    ip_value1 = ntohl(ip_value1);

    name[v->namelen] = (ip_value1 >> 24) & 0xff;
    name[v->namelen + 1] = (ip_value1 >> 16) & 0xff;
    name[v->namelen + 2] = (ip_value1 >> 8) & 0xff;
    name[v->namelen + 3] = ip_value1 & 0xff;

    name[v->namelen + 4] = index1;
    name[v->namelen + 5] = index2;

    ip_value2 = ntohl(ip_value2);

    name[v->namelen + 6] = (ip_value2 >> 24) & 0xff;
    name[v->namelen + 7] = (ip_value2 >> 16) & 0xff;
    name[v->namelen + 8] = (ip_value2 >> 8) & 0xff;
    name[v->namelen + 9] = ip_value2 & 0xff;

    *length = v->namelen + 10;
}

/* Utility function to get a Integer index + MAC address */
int
ipran_snmp_int_mac_index_get(struct variable *v, oid *name, size_t *length,
                             int *idx, unsigned char *mac_addr, int exact)
{
    int         OID_IDX_LEN = 7;
    int         result;
    int         get_idx;

    get_idx     = 0;

    *idx = 0;
    memset(mac_addr, 0, 6);

    if (exact)
    {
        // rx oid length != reg oid length + index len
        if (*length != v->namelen + OID_IDX_LEN)
        {
            return -1;
        }

        // compare reg oid length
        result = oid_compare(name, v->namelen, v->name, v->namelen);

        if (result != 0)
        {
            return -1;
        }

        get_idx  = 1;
    }
    else
    {
        if (*length >= v->namelen)
        {
            result = oid_compare(name, v->namelen, v->name, v->namelen);

            if (result == 0 && *length == v->namelen + OID_IDX_LEN)
            {
                get_idx  = 1;
            }
        }
        else
        {
            result = oid_compare(name, *length, v->name, v->namelen);
        }
    }

    if (result > 0)
    {
        return -1;
    }
    else if (result < 0)
    {
        memcpy(name, v->name, ((int)v->namelen) * sizeof(oid));
    }

    if (1 == get_idx)
    {
        *idx = name[v->namelen];
        oid2bytes(name + v->namelen + 1, 6, mac_addr);
    }

    return 0;
}

/* Utility function to set a Integer Index + MAC address.  */
void
ipran_snmp_int_mac_index_set(struct variable *v, oid *name, size_t *length,
                             int idx, unsigned char *mac_addr)
{
    name[v->namelen] = idx;
    oid_copy_bytes2oid(name + v->namelen + 1, mac_addr, 6);

    *length = v->namelen + 7;
}

/* Utility function to get two Integer indexes + MAC address */
int
ipran_snmp_intx2_mac_index_get(struct variable *v, oid *name, size_t *length,
                               int *idx, int *idx2,  unsigned char *mac_addr, int exact)
{
    int         OID_IDX_LEN = 8;
    int         result;
    int         get_idx;

    get_idx     = 0;

    *idx = 0;
    memset(mac_addr, 0, 6);

    if (exact)
    {
        // rx oid length != reg oid length + index len
        if (*length != v->namelen + OID_IDX_LEN)
        {
            return -1;
        }

        // compare reg oid length
        result = oid_compare(name, v->namelen, v->name, v->namelen);

        if (result != 0)
        {
            return -1;
        }

        get_idx  = 1;
    }
    else
    {
        if (*length >= v->namelen)
        {
            result = oid_compare(name, v->namelen, v->name, v->namelen);

            if (result == 0 && *length == v->namelen + OID_IDX_LEN)
            {
                get_idx  = 1;
            }
        }
        else
        {
            result = oid_compare(name, *length, v->name, v->namelen);
        }
    }

    if (result > 0)
    {
        return -1;
    }
    else if (result < 0)
    {
        memcpy(name, v->name, ((int)v->namelen) * sizeof(oid));
    }

    if (1 == get_idx)
    {
        *idx = name[v->namelen];
        *idx2 = name[v->namelen + 1];
        oid2bytes(name + v->namelen + 2, 6, mac_addr);
    }

    return 0;
}

/* Utility function to set two Integer Indexes + MAC address.  */
void
ipran_snmp_intx2_mac_index_set(struct variable *v, oid *name, size_t *length,
                               int idx, int idx2,  unsigned char *mac_addr)
{
    name[v->namelen] = idx;
    name[v->namelen + 1] = idx2;
    oid_copy_bytes2oid(name + v->namelen + 2, mac_addr, 6);

    *length = v->namelen + 8;
}


int
ipran_snmp_intx4_mac_index_get(struct variable *v, oid *name,
                               size_t *length, u_int32_t *index1,
                               u_int32_t *index2, u_int32_t *index3,
                               u_int32_t *index4, struct mac_addr *addr,
                               int exact)
{
    int         OID_IDX_LEN = 10;
    int         result;
    int         get_idx;

    get_idx     = 0;

    *index1 = 0;
    *index2 = 0;
    *index3 = 0;
    *index4 = 0;
    memset(addr, 0, sizeof(struct mac_addr));

    if (exact)
    {
        // rx oid length != reg oid length + index len
        if (*length != v->namelen + OID_IDX_LEN)
        {
            return -1;
        }

        // compare reg oid length
        result = oid_compare(name, v->namelen, v->name, v->namelen);

        if (result != 0)
        {
            return -1;
        }

        get_idx  = 1;
    }
    else
    {
        if (*length >= v->namelen)
        {
            result = oid_compare(name, v->namelen, v->name, v->namelen);

            if (result == 0 && *length == v->namelen + OID_IDX_LEN)
            {
                get_idx  = 1;
            }
        }
        else
        {
            result = oid_compare(name, *length, v->name, v->namelen);
        }
    }

    if (result > 0)
    {
        return -1;
    }
    else if (result < 0)
    {
        memcpy(name, v->name, ((int)v->namelen) * sizeof(oid));
    }

    if (1 == get_idx)
    {
        *index1 = name[v->namelen];
        *index2 = name[v->namelen + 1];
        *index3 = name[v->namelen + 2];
        *index4 = name[v->namelen + 3];

        oid2bytes(name + v->namelen + 4, ETHER_ADDR_LEN, addr->addr) ;
    }

    return 0;
}

/* Utility function to set a 4 Integer Indices + MAC address.  */
void
ipran_snmp_intx4_mac_index_set(struct variable *v, oid *name,
                               size_t *length, u_int32_t index1,
                               u_int32_t index2, u_int32_t index3,
                               u_int32_t index4, struct mac_addr *addr)
{
    name[v->namelen]     = index1;
    name[v->namelen + 1] = index2;
    name[v->namelen + 2] = index3;
    name[v->namelen + 3] = index4;
    oid_copy_bytes2oid(name + v->namelen + 4, addr->addr, sizeof(struct mac_addr));
    *length = v->namelen + sizeof(struct mac_addr) + 4;
}

/* Utility function to get a mac_addr index + Integer index */
int
ipran_snmp_mac_int_index_get(struct variable *v, oid *name, size_t *length,
                             u_int16_t *idx, u_char *oct_string, int oct_len, int exact)
{
    int         OID_IDX_LEN = 7;
    int         result;
    int         get_idx;

    get_idx     = 0;

    *idx = 0;
    memset(oct_string, 0, oct_len);

    if (exact)
    {
        // rx oid length != reg oid length + index len
        if (*length != v->namelen + OID_IDX_LEN)
        {
            return -1;
        }

        // compare reg oid length
        result = oid_compare(name, v->namelen, v->name, v->namelen);

        if (result != 0)
        {
            return -1;
        }

        get_idx  = 1;
    }
    else
    {
        if (*length >= v->namelen)
        {
            result = oid_compare(name, v->namelen, v->name, v->namelen);

            if (result == 0 && *length == v->namelen + OID_IDX_LEN)
            {
                get_idx  = 1;
            }
        }
        else
        {
            result = oid_compare(name, *length, v->name, v->namelen);
        }
    }

    if (result > 0)
    {
        return -1;
    }
    else if (result < 0)
    {
        memcpy(name, v->name, ((int)v->namelen) * sizeof(oid));
    }

    if (1 == get_idx)
    {
        oid2bytes(name + v->namelen, oct_len, oct_string);
        *idx = name[v->namelen + oct_len];
    }

    return 0;
}

/* Utility function to set a mac_addr index + Integer inde  */
void
ipran_snmp_mac_int_index_set(struct variable *v, oid *name, size_t *length,
                             u_int16_t idx, u_char *oct_string, int oct_len)
{
    oid_copy_bytes2oid(name + v->namelen, oct_string, oct_len);
    name[v->namelen + oct_len] = idx;

    *length = v->namelen + oct_len + 1;
}

/* Utility function to get a Integer Index + oct_string Index+oct_stringlen*/
int
ipran_snmp_int_str_index_get(struct variable *v, oid *name, size_t *length,
                             u_int32_t *index, u_char *oct_string, int max_str_len, int *oct_stringlen, int exact)
{
    int         str_offset = 2;     // .index1(offset 0).str_len(offset 1).str.
    int         min_idx_len = 3;    // index1,str_len,str
    int         result;
    int         str_index_cur_len;
    int         str_index_val_len;
    int         get_idx;

    *index     = 0;

    if (0 == max_str_len)
    {
        return -1;
    }

    memset(oct_string, 0x00, max_str_len);

    get_idx     = 0;
    str_index_val_len = 0;

    if (exact)
    {
        if (*length < v->namelen + min_idx_len)
        {
            return -1;
        }

        str_index_cur_len = *length - v->namelen + 1 - min_idx_len;
        str_index_val_len = name[v->namelen + str_offset - 1];

        if (str_index_cur_len != str_index_val_len)
        {
            return -1;
        }

        result = oid_compare(name, v->namelen, v->name, v->namelen);

        if (result != 0)
        {
            return -1;
        }

        get_idx   = 1;
    }
    else
    {
        if (*length >= v->namelen + min_idx_len)
        {
            str_index_cur_len = *length - v->namelen + 1 - min_idx_len;
            str_index_val_len = name[v->namelen + str_offset - 1];

            if (str_index_cur_len > str_index_val_len)
            {
                return -1;
            }

            result = oid_compare(name, v->namelen, v->name, v->namelen);

            if (result == 0 && str_index_cur_len == str_index_val_len)
            {
                get_idx = 1;
            }
        }
        else if (*length >= v->namelen)
        {
            result = oid_compare(name, v->namelen, v->name, v->namelen);
        }
        else
        {
            result = oid_compare(name, *length, v->name, v->namelen);
        }
    }

    if (result > 0)
    {
        return -1;
    }
    else if (result < 0)
    {
        memcpy(name, v->name, ((int)v->namelen) * sizeof(oid));
    }

    if (1 == get_idx)
    {
        if (0 != str_index_val_len &&  str_index_val_len < max_str_len)
        {
            *index = name[v->namelen];
            *oct_stringlen = name[v->namelen + 1];
            oid2bytes(name + v->namelen + str_offset, str_index_val_len, oct_string);
        }
    }

    return 0;
}

/* Utility function to set a Integer Index + oct_string Index+oct_stringlen*/
int
ipran_snmp_int_str_index_set(struct variable *v, oid *name, size_t *length,
                             u_int32_t index, u_char *oct_string, int oct_len)
{
    name[v->namelen] = index;
    name[v->namelen + 1] = oct_len;
    oid_copy_bytes2oid(name + v->namelen + 2, oct_string, oct_len);
    *length = v->namelen + oct_len + 2;
}


/* Utility function to get a Integer Index + oct_string Index+oct_stringlen+Integer Index*/
int
ipran_snmp_int_str_int_index_get(struct variable *v, oid *name, size_t *length,
                                 u_int32_t *index, u_char *oct_string, int max_str_len, int *oct_stringlen, u_int32_t *index2, int exact)
{
    int        str_offset = 2;     // .index1(offset 0).str_len(offset 1).str.
    int        min_idx_len = 4;    // index1,str_len,str,index2
    int        result;
    int        str_index_cur_len;
    int        str_index_val_len;
    int        get_idx;

    *index    = 0;

    if (0 == max_str_len)
    {
        return -1;
    }

    memset(oct_string, 0x00, max_str_len);

    get_idx    = 0;
    str_index_val_len = 0;

    if (exact)
    {
        if (*length < v->namelen + min_idx_len)
        {
            return -1;
        }

        str_index_cur_len = *length - v->namelen + 1 - min_idx_len;
        str_index_val_len = name[v->namelen + str_offset - 1];

        if (str_index_cur_len != str_index_val_len)
        {
            return -1;
        }

        result = oid_compare(name, v->namelen, v->name, v->namelen);

        if (result != 0)
        {
            return -1;
        }

        get_idx  = 1;
    }
    else
    {
        if (*length >= v->namelen + min_idx_len)
        {
            str_index_cur_len = *length - v->namelen + 1 - min_idx_len;
            str_index_val_len = name[v->namelen + str_offset - 1];

            if (str_index_cur_len > str_index_val_len)
            {
                return -1;
            }

            result = oid_compare(name, v->namelen, v->name, v->namelen);

            if (result == 0 && str_index_cur_len == str_index_val_len)
            {
                get_idx = 1;
            }
        }
        else if (*length >= v->namelen)
        {
            result = oid_compare(name, v->namelen, v->name, v->namelen);
        }
        else
        {
            result = oid_compare(name, *length, v->name, v->namelen);
        }
    }

    if (result > 0)
    {
        return -1;
    }
    else if (result < 0)
    {
        memcpy(name, v->name, ((int)v->namelen) * sizeof(oid));
    }

    if (1 == get_idx)
    {
        if (0 != str_index_val_len &&  str_index_val_len < max_str_len)
        {
            *index = name[v->namelen];
            *oct_stringlen = name[v->namelen + 1];
            oid2bytes(name + v->namelen + str_offset, str_index_val_len, oct_string);
            *index2 = name[v->namelen + 2 + *oct_stringlen];
        }
    }

    return 0;
}

/* Utility function to set a Integer Index + oct_string Index+oct_stringlen+Integer Index*/
int
ipran_snmp_int_str_int_index_set(struct variable *v, oid *name, size_t *length,
                                 u_int32_t index, u_char *oct_string, int oct_len, u_int32_t index2)
{
    name[v->namelen] = index;
    name[v->namelen + 1] = oct_len;
    oid_copy_bytes2oid(name + v->namelen + 2, oct_string, oct_len);
    name[v->namelen + oct_len + 2] = index2;
    *length = v->namelen + oct_len + 3;
}

/* Utility function to set a oct_stringlen + oct_string + Integer Index */
int
ipran_snmp_str_int_index_get(struct variable *v, oid *name, size_t *length,
                             u_char *oct_string, int max_str_len,  int *oct_stringlen, u_int32_t *index,  int exact)
{
    int         str_offset = 1;     // .index1(offset 0).str_len(offset 1).str.
    int         min_idx_len = 3;    // index1,str_len,str
    int         result;
    int         str_index_cur_len;
    int         str_index_val_len;
    int         get_idx;

    *index     = 0;

    if (0 == max_str_len)
    {
        return -1;
    }

    memset(oct_string, 0x00, max_str_len);

    get_idx     = 0;
    str_index_val_len = 0;

    if (exact)
    {
        if (*length < v->namelen + min_idx_len)
        {
            return -1;
        }

        str_index_cur_len = *length - v->namelen;
        str_index_val_len = name[v->namelen];

        if (str_index_cur_len != str_index_val_len)
        {
            return -1;
        }

        result = oid_compare(name, v->namelen, v->name, v->namelen);

        if (result != 0)
        {
            return -1;
        }

        get_idx   = 1;
    }
    else
    {
        if (*length >= v->namelen + min_idx_len)
        {
            str_index_cur_len = v->namelen;
            str_index_val_len = name[v->namelen];
            result = oid_compare(name, v->namelen, v->name, v->namelen);

            if (result == 0)
            {
                get_idx = 1;
            }
            else
            {
                memcpy(name, v->name, ((int)v->namelen) * sizeof(oid));
            }
        }
        else if (*length >= v->namelen)
        {
            result = oid_compare(name, v->namelen, v->name, v->namelen);
        }
        else
        {
            result = oid_compare(name, *length, v->name, v->namelen);
        }
    }

    if (result > 0)
    {
        return -1;
    }
    else if (result < 0)
    {
        memcpy(name, v->name, ((int)v->namelen) * sizeof(oid));
    }

    if (1 == get_idx)
    {
        if (0 != str_index_val_len &&  str_index_val_len < max_str_len)
        {
            *oct_stringlen = name[v->namelen];
            oid2bytes(name + v->namelen + str_offset, str_index_val_len, oct_string);
            *index = name[v->namelen + *oct_stringlen + 1];
        }
    }

    return 0;
}

int
ipran_snmp_str_int_index_set(struct variable *v, oid *name, size_t *length,
                             u_char *oct_string, int oct_len , u_int32_t index)
{
    name[v->namelen] = oct_len;
    oid_copy_bytes2oid(name + v->namelen + 1, oct_string, oct_len);
    name[v->namelen + oct_len + 1] = index;
    *length = v->namelen + oct_len + 2;
}



int
//l2_snmp_intx2_str_index_get (struct variable *v, oid *name, size_t *length,
ipran_snmp_intx2_str_index_get(struct variable *v, oid *name, size_t *length,
                               u_int32_t *index1,
                               u_int32_t *index2,
                               u_char *index_str, int max_str_len, int *out_str_len , int exact)
{
    int         str_offset = 3;     // .index1(offset 0).index2(offset 1).str_len(offset 2).str.
    int         min_idx_len = 4;    // index1,index2,str_len,str
    int         result;
    int         str_index_cur_len;
    int         str_index_val_len;
    int         get_idx;

    *index1     = 0;
    *index2     = 0;

    if (0 == max_str_len)
    {
        return -1;
    }

    memset(index_str, 0x00, max_str_len);

    get_idx     = 0;
    str_index_val_len = 0;

    if (exact)
    {
        if (*length < v->namelen + min_idx_len)
        {
            return -1;
        }

        str_index_cur_len = *length - v->namelen + 1 - min_idx_len;
        str_index_val_len = name[v->namelen + str_offset - 1];

        if (str_index_cur_len != str_index_val_len)
        {
            return -1;
        }

        result = oid_compare(name, v->namelen, v->name, v->namelen);

        if (result != 0)
        {
            return -1;
        }

        get_idx   = 1;
    }
    else
    {
        if (*length >= v->namelen + min_idx_len)
        {
            str_index_cur_len = *length - v->namelen + 1 - min_idx_len;
            str_index_val_len = name[v->namelen + str_offset - 1];

            if (str_index_cur_len > str_index_val_len)
            {
                return -1;
            }

            result = oid_compare(name, v->namelen, v->name, v->namelen);

            if (result == 0 && str_index_cur_len == str_index_val_len)
            {
                get_idx = 1;
            }
        }
        else if (*length >= v->namelen)
        {
            result = oid_compare(name, v->namelen, v->name, v->namelen);
        }
        else
        {
            result = oid_compare(name, *length, v->name, v->namelen);
        }
    }

    if (result > 0)
    {
        return -1;
    }
    else if (result < 0)
    {
        memcpy(name, v->name, ((int)v->namelen) * sizeof(oid));
    }

    if (1 == get_idx)
    {
        if (0 != str_index_val_len &&  str_index_val_len < max_str_len)
        {
            *index1 = name[v->namelen];
            *index2 = name[v->namelen + 1];
            oid2bytes(name + v->namelen + str_offset, str_index_val_len, index_str);
            *out_str_len = strlen(index_str);
        }
    }

    return 0;
}

/* Utility function to set two Integer Index + oct_string Index+oct_stringlen*/
void
ipran_snmp_intx2_str_index_set(struct variable *v, oid *name, size_t *length,
                               u_int32_t index1, u_int32_t index2, u_char *oct_string, int oct_len)
{
    name[v->namelen] = index1;
    name[v->namelen + 1] = index2;
    name[v->namelen + 2] = oct_len;
    oid_copy_bytes2oid(name + v->namelen + 3, oct_string, oct_len);
    *length = v->namelen + oct_len + 3;
}

/* Utility function to get one integer + oct string(length fixed) index.  */
int
ipran_snmp_int_implicitstr_index_get(struct variable *v, oid *name, size_t *length,
                                     u_int32_t *index, u_int8_t *buff, int buff_len, int exact)
{
    int         OID_IDX_LEN;
    int         result;
    int         i;
    int         get_idx;

    get_idx     = 0;

    OID_IDX_LEN = buff_len + 2;

    // init index
    *index      = 0;
    memset(buff, 0x00, buff_len);

    if (exact)
    {
        // rx oid length != reg oid length + index len
        if (*length != v->namelen + OID_IDX_LEN)
        {
            return -1;
        }

        // compare reg oid length
        result = oid_compare(name, v->namelen, v->name, v->namelen);

        if (result != 0)
        {
            return -1;
        }

        get_idx  = 1;
    }
    else
    {
        if (*length >= v->namelen)
        {
            result = oid_compare(name, v->namelen, v->name, v->namelen);

            if (result == 0 && *length == v->namelen + OID_IDX_LEN)
            {
                get_idx  = 1;
            }
        }
        else
        {
            result = oid_compare(name, *length, v->name, v->namelen);
        }
    }

    if (result > 0)
    {
        return -1;
    }
    else if (result < 0)
    {
        memcpy(name, v->name, ((int)v->namelen) * sizeof(oid));
    }

    if (1 == get_idx)
    {
        *index   = name[v->namelen];

        if (*length - v->namelen != 0)
        {
            for (i = 0; i < buff_len; ++i)
            {
                buff[i] = name[v->namelen + 2 + i] & 0xFF;
            }
        }
    }

    return 0;
}

/* Utility function to set one integer + oct string(20byte) index.  */
void
ipran_snmp_int_implicitstr_index_set(struct variable *v, oid *name, size_t *length,
                                     u_int32_t index, u_int8_t *buff, int buff_len)

{
    int     i;

    name[v->namelen] = index;
    name[v->namelen + 1] = buff_len;

    for (i = 0; i < buff_len; ++i)
    {
        name[v->namelen + 2 + i] = buff[i];
    }

    *length = v->namelen + buff_len + 2;
}

char *
bitstring_init(char *bstring, char fillvalue, int bitstring_length)
{
    memset(bstring, fillvalue, bitstring_length);

    return (bstring);
}

int
bitstring_testbit(char *bstring, int number)
{
    bstring += number / 8;
    return ((*bstring & bitstring_mask[(number % 8)]));
}

void
bitstring_setbit(char *bstring, int number)
{
    bstring += number / 8;
    *bstring |= bitstring_mask[(number % 8)];
}

void
bitstring_clearbit(char *bstring, int number)
{
    bstring += number / 8;
    *bstring &= ~bitstring_mask[(number % 8)];
}

int
bitstring_count(char *bstring, int bstring_length)
{
    int i;
    char c;
    int n = 0;

    for (i = 0; i < bstring_length; i++)
    {
        c = *bstring++;

        if (c)
            do
            {
                n++;
            }

            while ((c = c & (c - 1)));
    }

    return (n);
}


