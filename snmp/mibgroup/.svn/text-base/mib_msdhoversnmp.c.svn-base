/*
 *  cli command MIB group implementation - ipran_cli_set.c
 *
 */
/* Portions of this file are subject to the following copyright(s).  See
 * the Net-SNMP's COPYING file for more details and other copyrights
 * that may apply:
 */
/*
 * Portions of this file are copyrighted by:
 * Copyright ?2003 Sun Microsystems, Inc. All rights reserved.
 * Use is subject to license terms specified in the COPYING file
 * distributed with the Net-SNMP package.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "net-snmp-config.h"
#include "net-snmp-includes.h"
#include "net-snmp-agent-includes.h"
#include "snmpd.h"

#include <lib/msg_ipc.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/devm_com.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/memory.h>
#include <lib/memtypes.h>

#include "ipran_snmp_data_cache.h"
#include "mib_msdhoversnmp.h"

FindVarMethod       _msdhoversnmp_get;
WriteMethod         _msdhoversnmp_set;

/*
 * define the structure we're going to ask the agent to register our
 * information at
 */
#define HHMSDHOVERSNMPCOMMON_OID    1,3,6,1,4,1,9966,5,37
#define HHMSDHFRAMETODEVICE         1
#define HHMSDHFRAMETONM             2

struct variable1    msdhoversnmp_variables[] =
{
    {HHMSDHFRAMETODEVICE, ASN_OCTET_STR, RWRITE, _msdhoversnmp_get, 1, {2}},
    {HHMSDHFRAMETONM,     ASN_OCTET_STR, RONLY,  _msdhoversnmp_get, 1, {3}},
};

/*
 * Define the OID pointer to the top of the mib tree that we're
 * registering underneath
 */
oid msdhoversnmp_oid[] = { HHMSDHOVERSNMPCOMMON_OID };

void init_mib_msdhoversnmp(void)
{
    if (gDevTypeID == ID_H9MOLMXE_VX)
    {
        REGISTER_MIB("vx_cli_set_mode", msdhoversnmp_variables, variable1, msdhoversnmp_oid);
    }
}

u_char *_msdhoversnmp_get(struct variable *vp,
                          oid *name,
                          size_t *length,
                          int exact, size_t *var_len, WriteMethod **write_method)
{
    static int      intval;
    char            *error_info = {0};

    *write_method = NULL;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case HHMSDHFRAMETODEVICE:
            *write_method = _msdhoversnmp_set;
            *var_len = msdh_frm_get_length(g_msdhoversnmp.msdhtodevice);
            return (u_char *)(g_msdhoversnmp.msdhtodevice);

        case HHMSDHFRAMETONM:
            *var_len = msdh_frm_get_length(g_msdhoversnmp.msdhtoPC);
            return (u_char *)(g_msdhoversnmp.msdhtoPC);

        default:
            return NULL;
    }

    return NULL;
}

int _msdhoversnmp_set(int action,
                      u_char *var_val,
                      u_char var_val_type,
                      size_t var_val_len,
                      u_char *statP, oid *name, size_t name_len)
{
    int     ret = -1;

    if (NULL == var_val)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (ASN_OCTET_STR != var_val_type)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > MSDH_OVER_SNMP_BUFFER)
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    switch (action)
    {
        case RESERVE1:
        case RESERVE2:
        case ACTION:
        case FREE:
        case UNDO:
            return SNMP_ERR_NOERROR;

        case COMMIT:
            memcpy(g_msdhoversnmp.msdhtodevice, var_val, var_val_len);

            if (0 == msdh_frm_isok(g_msdhoversnmp.msdhtodevice, var_val_len))
            {
                return SNMP_ERR_COMMITFAILED;
            }

            //ret = ipc_send_msdh(g_msdhoversnmp.msdhtodevice, var_val_len, 1, MODULE_ID_SDHMGT, MODULE_ID_SNMPD, IPC_TYPE_SNMP, 0, IPC_OPCODE_GET);

			ret = ipc_send_msg_n2((void *)g_msdhoversnmp.msdhtodevice, var_val_len, 1,
				MODULE_ID_SDHMGT, MODULE_ID_SNMPD, IPC_TYPE_SNMP, 0, IPC_OPCODE_GET, 0);

            if (ret < 0)
            {
                return SNMP_ERR_COMMITFAILED;
            }

            return SNMP_ERR_NOERROR;

        default:
            return SNMP_ERR_NOERROR;
    }

    return SNMP_ERR_NOERROR;
}
