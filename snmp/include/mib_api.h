#ifndef NET_SNMP_MIB_API_H
#define NET_SNMP_MIB_API_H

    /**
     *  Library API routines concerned with MIB files and objects, and OIDs
     */

#include "types.h"

    /*
     *  For the initial release, this will just refer to the
     *  relevant UCD header files.
     *    In due course, the routines relevant to this area of the
     *  API will be identified, and listed here directly.
     *
     *  But for the time being, this header file is a placeholder,
     *  to allow application writers to adopt the new header file names.
     */

#include "snmp_api.h"

#include "mib.h"
#ifndef DISABLE_MIB_LOADING
#include "parse.h"
#endif
#include "callback.h"
#include "oid_stash.h"
#include "ucd_compat.h"

#endif                          /* NET_SNMP_MIB_API_H */
