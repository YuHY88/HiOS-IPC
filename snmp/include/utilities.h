#ifndef NET_SNMP_UTILITIES_H
#define NET_SNMP_UTILITIES_H

    /**
     *  Library API routines not specifically concerned with SNMP directly,
     *    but used more generally within the library, agent and other applications.
     *
     *  This also includes "standard" system routines, which are missing on
     *    particular O/S distributiones.
     */

#ifndef NET_SNMP_CONFIG_H
#error "Please include net-snmp-config.h before this file"
#endif

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
#include "snmp_client.h"
#if HAVE_GETOPT_H
#include <getopt.h>
#else
#include "getopt.h"
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>         /* for in_addr_t */
#endif
#include "system.h"
#include "tools.h"
#include "asn1.h"      /* for counter64 */
#include "int64.h"

#include "mt_support.h"
/*  #include "snmp_locking.h"  */
#include "snmp_alarm.h"
#include "callback.h"
#include "data_list.h"
#include "oid_stash.h"
#include "snmp.h"
#include "snmp_impl.h"
#include "snmp-tc.h"
#include "check_varbind.h"
#include "container.h"
#include "container_binary_array.h"
#include "container_list_ssll.h"
#include "container_iterator.h"

#include "snmp_assert.h"

#include "version.h"

#endif                          /* NET_SNMP_UTILITIES_H */
