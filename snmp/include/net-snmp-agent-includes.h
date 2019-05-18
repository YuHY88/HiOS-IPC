/*
 * A simple helpful wrapper to include lots of agent specific include
 * * files for people wanting to embed and snmp agent into an external
 * * application
 */
#ifndef NET_SNMP_AGENT_INCLUDES_H
#define NET_SNMP_AGENT_INCLUDES_H

#ifdef HAVE_LIBM
#undef HAVE_LIBM
#endif
#ifdef PACKAGE_BUGREPORT
#undef PACKAGE_BUGREPORT
#endif
#ifdef PACKAGE_NAME
#undef PACKAGE_NAME
#endif
#ifdef PACKAGE_STRING
#undef PACKAGE_STRING
#endif
#ifdef PACKAGE_TARNAME
#undef PACKAGE_TARNAME
#endif
#ifdef PACKAGE_VERSION
#undef PACKAGE_VERSION
#endif
#include "config.h"

#include "mib_module_config.h"
#include "agent_module_config.h"

#include "snmp_agent.h"
#include "snmp_vars.h"
#include "ds_agent.h"
#include "agent_handler.h"
#include "agent_read_config.h"
#include "agent_trap.h"
#include "agent_handler.h"
#include "all_helpers.h"
#include "var_struct.h"

#endif                          /* NET_SNMP_AGENT_INCLUDES_H */
