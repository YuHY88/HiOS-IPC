#ifndef NET_SNMP_SNMPV3_H
#define NET_SNMP_SNMPV3_H

    /**
     *  Library API routines concerned with SNMPv3 handling.
     *
     *  Most of these would typically not be used directly,
     *     but be invoked via version-independent API routines.
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

#include "callback.h"
#include "snmpv3.h"
#include "transform_oids.h"
#include "keytools.h"
#include "scapi.h"
#include "lcd_time.h"
#ifdef USE_INTERNAL_MD5
#include "md5.h"
#endif

#include "snmp_secmod.h"
#include "snmpv3-security-includes.h"

#endif                          /* NET_SNMP_SNMPV3_H */
