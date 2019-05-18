/*
 * spilter snmp browser interface - mib_spilter.c
 *
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "net-snmp-config.h"
#include <lib/types.h>
#include "types.h"

#include "net-snmp-includes.h"
#include "net-snmp-agent-includes.h"

#include "snmp_index_operater.h"

#include <lib/msg_ipc.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/pkt_buffer.h>
#include <lib/msg_ipc.h>
#include "mib_spliter.h"
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

#define hhrMirrorGroupSupport_profix                            1,3,6,1,4,1,9966,5,35,1
#define hhrRIPInstanceSupport_profix                            1,3,6,1,4,1,9966,5,35,3,3
#define hhrOSPFInstanceSupport_profix                           1,3,6,1,4,1,9966,5,35,3,4
#define hhrArpSupport_profix                                    1,3,6,1,4,1,9966,5,35,3,5
#define hhrISISInstanceSupport_profix                           1,3,6,1,4,1,9966,5,35,3,8
#define hhrBGPNeighborSupport_profix                            1,3,6,1,4,1,9966,5,35,3,7
#define hhrOSPFDCNSupport_profix                                1,3,6,1,4,1,9966,5,35,3,8
#define hhrDHCPDynamicIPBindSupport_profix                      1,3,6,1,4,1,9966,5,35,4
#define hhrVplsServiceSupport_profix                            1,3,6,1,4,1,9966,5,35,5

#define hhrMplsLdpSupport_profix                                1,3,6,1,4,1,9966,5,35,5,19
#define hhrL3vpnSupport_profix                                  1,3,6,1,4,1,9966,5,35,5,22
#define hhrQosMappingSupport_profix                             1,3,6,1,4,1,9966,5,35,6,2
#define hhrHQosSupport_profix                                   1,3,6,1,4,1,9966,5,35,6,12
#define hhrQosPortLRSupport_profix                              1,3,6,1,4,1,9966,5,35,6
#define hhrCE1SubIfSupport_profix                               1,3,6,1,4,1,9966,5,35,7
#define hhrCFMSLASupport_profix                                 1,3,6,1,4,1,9966,5,35,9,1
#define hhrEFMEventSupport_profix                               1,3,6,1,4,1,9966,5,35,9,2
#define hhrMplsTpOamMegSupport_profix                           1,3,6,1,4,1,9966,5,35,11
#define hhrbfdDetectSupport_profix                              1,3,6,1,4,1,9966,5,35,12
#define hhry1564MeasureSupport_profix                           1,3,6,1,4,1,9966,5,35,13
#define hhrMacIfLimitSupport_profix                             1,3,6,1,4,1,9966,5,35,16,1
#define hhrEndMib_profix                                        1,3,6,1,4,1,9966,5,35

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

static oid hhrMirrorGroupSupport_oid[] = {hhrMirrorGroupSupport_profix};
static oid hhrRIPInstanceSupport_oid[] = {hhrRIPInstanceSupport_profix};
static oid hhrOSPFInstanceSupport_oid[] = {hhrOSPFInstanceSupport_profix};
static oid hhrArpSupport_oid[] = {hhrArpSupport_profix};
static oid hhrISISInstanceSupport_oid[] = {hhrISISInstanceSupport_profix};
static oid hhrBGPNeighborSupport_oid[] = {hhrBGPNeighborSupport_profix};
static oid hhrOSPFDCNSupport_oid[] = {hhrOSPFDCNSupport_profix};
static oid hhrDHCPDynamicIPBindSupport_oid[] = {hhrDHCPDynamicIPBindSupport_profix};
static oid hhrVplsServiceSupport_oid[] = {hhrVplsServiceSupport_profix};

static oid hhrMplsLdpSupport_oid[] = {hhrMplsLdpSupport_profix};
static oid hhrL3vpnSupport_oid[] = {hhrL3vpnSupport_profix};
static oid hhrQosMappingSupport_oid[] = {hhrQosMappingSupport_profix};
static oid hhrHQosSupport_oid[] = {hhrHQosSupport_profix};
static oid hhrQosPortLRSupport_oid[] = {hhrQosPortLRSupport_profix};

static oid hhrCE1SubIfSupport_oid[] = {hhrCE1SubIfSupport_profix};
static oid hhrCFMSLASupport_oid[] = {hhrCFMSLASupport_profix};

static oid hhrEFMEventSupport_oid[] = {hhrEFMEventSupport_profix};
static oid hhrMplsTpOamMegSupport_oid[] = {hhrMplsTpOamMegSupport_profix};
static oid hhrbfdDetectSupport_oid[] = {hhrbfdDetectSupport_profix};
static oid hhry1564MeasureSupport_oid[] = {hhry1564MeasureSupport_profix};
static oid hhrMacIfLimitSupport_oid[] = {hhrMacIfLimitSupport_profix};
static oid hhrEndMib_oid[] = {hhrEndMib_profix};


FindVarMethod hhrMirrorGroupSupport_get;
FindVarMethod hhrRIPInstanceSupport_get;
FindVarMethod hhrOSPFInstanceSupport_get;
FindVarMethod hhrArpSupport_get;
FindVarMethod hhrISISInstanceSupport_get;
FindVarMethod hhrBGPNeighborSupport_get;
FindVarMethod hhrOSPFDCNSupport_get;
FindVarMethod hhrDHCPDynamicIPBindSupport_get;
FindVarMethod hhrVplsServiceSupport_get;

FindVarMethod hhrMplsLdpSupport_get;
FindVarMethod hhrL3vpnSupport_get;
FindVarMethod hhrQosMappingSupport_get;
FindVarMethod hhrHQosSupport_get;
FindVarMethod hhrQosPortLRSupport_get;
FindVarMethod hhrCE1SubIfSupport_get;
FindVarMethod hhrCFMSLASupport_get;

FindVarMethod hhrCFMETHTestResultSupport_get ;
FindVarMethod hhrCFMMepSupport_get;
FindVarMethod hhrCFMSLAResultSupport_get;

FindVarMethod hhrEFMEventSupport_get;
FindVarMethod hhrMplsTpOamMegSupport_get;
FindVarMethod hhrbfdDetectSupport_get;
FindVarMethod hhry1564MeasureSupport_get;
FindVarMethod hhrMacIfLimitSupport_get;
FindVarMethod hhrEndMib_get;



struct variable1 hhrMirrorGroupSupport_variables[] =
{
    {hhrMirrorGroupSupport, ASN_INTEGER,   RONLY, hhrMirrorGroupSupport_get, 1, {hhrMirrorGroupSupport}}
};
struct variable1 hhrRIPInstanceSupport_variables[] =
{
    {hhrRIPInstanceSupport, ASN_INTEGER,   RONLY, hhrRIPInstanceSupport_get, 1, {hhrRIPInstanceSupport}}
};
struct variable1 hhrOSPFInstanceSupport_variables[] =
{
    {hhrOSPFInstanceSupport, ASN_INTEGER,   RONLY, hhrOSPFInstanceSupport_get, 1, {hhrOSPFInstanceSupport}}
};
struct variable1 hhrArpSupport_variables[] =
{
    {hhrArpSupport, ASN_INTEGER,   RONLY, hhrArpSupport_get, 1, {hhrArpSupport}}
};
struct variable1 hhrISISInstanceSupport_variables[] =
{
    {hhrISISInstanceSupport, ASN_INTEGER,   RONLY, hhrISISInstanceSupport_get, 1, {hhrISISInstanceSupport}}
};
struct variable1 hhrBGPNeighborSupport_variables[] =
{
    {hhrBGPNeighborSupport, ASN_INTEGER,   RONLY, hhrBGPNeighborSupport_get, 1, {hhrBGPNeighborSupport}}
};
struct variable1 hhrOSPFDCNSupport_variables[] =
{
    {hhrOSPFDCNSupport, ASN_INTEGER,   RONLY, hhrOSPFDCNSupport_get, 1, {hhrOSPFDCNSupport}}
};
struct variable1 hhrDHCPDynamicIPBindSupport_variables[] =
{
    {hhrDHCPDynamicIPBindSupport, ASN_INTEGER,   RONLY, hhrDHCPDynamicIPBindSupport_get, 1, {hhrDHCPDynamicIPBindSupport}}
};
struct variable1 hhrVplsServiceSupport_variables[] =
{
    {hhrVplsServiceSupport, ASN_INTEGER,   RONLY, hhrVplsServiceSupport_get, 1, {hhrVplsServiceSupport}}
};


struct variable1 hhrMplsLdpSupport_variables[] =
{
    {hhrMplsLdpSupport, ASN_INTEGER,   RONLY, hhrMplsLdpSupport_get, 1, {hhrMplsLdpSupport}}
};
struct variable1 hhrL3vpnSupport_variables[] =
{
    {hhrL3vpnSupport, ASN_INTEGER,   RONLY, hhrL3vpnSupport_get, 1, {hhrL3vpnSupport}}
};
struct variable1 hhrQosMappingSupport_variables[] =
{
    {hhrQosMappingSupport, ASN_INTEGER,   RONLY, hhrQosMappingSupport_get, 1, {hhrQosMappingSupport}}
};
struct variable1 hhrHQosSupport_variables[] =
{
    {hhrHQosSupport, ASN_INTEGER,   RONLY, hhrHQosSupport_get, 1, {hhrHQosSupport}}
};
struct variable1 hhrQosPortLRSupport_variables[] =
{
    {hhrQosPortLRSupport, ASN_INTEGER,   RONLY, hhrQosPortLRSupport_get, 1, {hhrQosPortLRSupport}}
};
struct variable1 hhrCE1SubIfSupport_variables[] =
{
    {hhrCE1SubIfSupport, ASN_INTEGER,   RONLY, hhrCE1SubIfSupport_get, 1, {hhrCE1SubIfSupport}}
};
struct variable1 hhrCFMSLASupport_variables[] =
{
    {hhrCFMSLASupport, ASN_INTEGER,   RONLY, hhrCFMSLASupport_get, 1, {hhrCFMSLASupport}}
};

struct variable1 hhrCFMETHTestResultSupport_variables[] =
{
    {hhrCFMETHTestResultSupport, ASN_INTEGER,   RONLY, hhrCFMETHTestResultSupport_get, 1, {hhrCFMETHTestResultSupport}}
};


struct variable1 hhrCFMMepSupport_variables[] =
{
    {hhrCFMMepSupport, ASN_INTEGER,   RONLY, hhrCFMMepSupport_get, 1, {hhrCFMMepSupport}}
};

struct variable1 hhrCFMSLAResultSupport_variables[] =
{
    {hhrCFMSLAResultSupport, ASN_INTEGER,   RONLY, hhrCFMSLAResultSupport_get, 1, {hhrCFMSLAResultSupport}}
};

struct variable1 hhrEFMEventSupport_variables[] =
{
    {hhrEFMEventSupport, ASN_INTEGER,   RONLY, hhrEFMEventSupport_get, 1, {hhrEFMEventSupport}}
};
struct variable1 hhrMplsTpOamMegSupport_variables[] =
{
    {hhrMplsTpOamMegSupport, ASN_INTEGER,   RONLY, hhrMplsTpOamMegSupport_get, 1, {hhrMplsTpOamMegSupport}}
};
struct variable1 hhrbfdDetectSupport_variables[] =
{
    {hhrbfdDetectSupport, ASN_INTEGER,   RONLY, hhrbfdDetectSupport_get, 1, {hhrbfdDetectSupport}}
};
struct variable1 hhry1564MeasureSupport_variables[] =
{
    {hhry1564MeasureSupport, ASN_INTEGER,   RONLY, hhry1564MeasureSupport_get, 1, {hhry1564MeasureSupport}}
};
struct variable1 hhrMacIfLimitSupport_variables[] =
{
    {hhrMacIfLimitSupport, ASN_INTEGER,   RONLY, hhrMacIfLimitSupport_get, 1, {hhrMacIfLimitSupport}}
};
struct variable1 hhrEndMib_variables[] =
{
    {hhrEndMib, ASN_INTEGER,   RONLY, hhrEndMib_get, 1, {hhrEndMib}}
};


/*-----------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------*/

static int int_value;

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
void  init_mib_spliter(void)
{
    REGISTER_MIB("spliter/hhrMirrorGroupSupport", hhrMirrorGroupSupport_variables, variable1, hhrMirrorGroupSupport_oid);
    REGISTER_MIB("spliter/hhrRIPInstanceSupport", hhrRIPInstanceSupport_variables, variable1, hhrRIPInstanceSupport_oid);
    REGISTER_MIB("spliter/hhrOSPFInstanceSupport", hhrOSPFInstanceSupport_variables, variable1, hhrOSPFInstanceSupport_oid);
    REGISTER_MIB("spliter/hhrArpSupport", hhrArpSupport_variables, variable1, hhrArpSupport_oid);
    REGISTER_MIB("spliter/hhrISISInstanceSupport", hhrISISInstanceSupport_variables, variable1, hhrISISInstanceSupport_oid);
    REGISTER_MIB("spliter/hhrBGPNeighborSupport", hhrBGPNeighborSupport_variables, variable1, hhrBGPNeighborSupport_oid);
    REGISTER_MIB("spliter/hhrOSPFDCNSupport", hhrOSPFDCNSupport_variables, variable1, hhrOSPFDCNSupport_oid);
    REGISTER_MIB("spliter/hhrDHCPDynamicIPBindSupport", hhrDHCPDynamicIPBindSupport_variables, variable1, hhrDHCPDynamicIPBindSupport_oid);
    REGISTER_MIB("spliter/hhrVplsServiceSupport", hhrVplsServiceSupport_variables, variable1, hhrVplsServiceSupport_oid);

    REGISTER_MIB("spliter/hhrMplsLdpSupport", hhrMplsLdpSupport_variables, variable1, hhrMplsLdpSupport_oid);
    REGISTER_MIB("spliter/hhrL3vpnSupport", hhrL3vpnSupport_variables, variable1, hhrL3vpnSupport_oid);
    REGISTER_MIB("spliter/hhrQosMappingSupport", hhrQosMappingSupport_variables, variable1, hhrQosMappingSupport_oid);
    REGISTER_MIB("spliter/hhrHQosSupport", hhrHQosSupport_variables, variable1, hhrHQosSupport_oid);
    REGISTER_MIB("spliter/hhrQosPortLRSupport", hhrQosPortLRSupport_variables, variable1, hhrQosPortLRSupport_oid);
    REGISTER_MIB("spliter/hhrCE1SubIfSupport", hhrCE1SubIfSupport_variables, variable1, hhrCE1SubIfSupport_oid);
    REGISTER_MIB("spliter/hhrCFMSLASupport", hhrCFMSLASupport_variables, variable1, hhrCFMSLASupport_oid);

    REGISTER_MIB("spliter/hhrCFMETHTestResultSupport", hhrCFMETHTestResultSupport_variables, variable1, hhrCFMSLASupport_oid);
    REGISTER_MIB("spliter/hhrCFMMepSupport", hhrCFMMepSupport_variables, variable1, hhrCFMSLASupport_oid);
    REGISTER_MIB("spliter/hhrCFMSLAResultSupport", hhrCFMSLAResultSupport_variables, variable1, hhrCFMSLASupport_oid);

    REGISTER_MIB("spliter/hhrEFMEventSupport", hhrEFMEventSupport_variables, variable1, hhrEFMEventSupport_oid);
    REGISTER_MIB("spliter/hhrMplsTpOamMegSupport", hhrMplsTpOamMegSupport_variables, variable1, hhrMplsTpOamMegSupport_oid);
    REGISTER_MIB("spliter/hhrbfdDetectSupport", hhrbfdDetectSupport_variables, variable1, hhrbfdDetectSupport_oid);
    REGISTER_MIB("spliter/hhry1564MeasureSupport", hhry1564MeasureSupport_variables, variable1, hhry1564MeasureSupport_oid);
    REGISTER_MIB("spliter/hhrMacIfLimitSupport", hhrMacIfLimitSupport_variables, variable1, hhrMacIfLimitSupport_oid);
    REGISTER_MIB("spliter/hhrEndMib", hhrEndMib_variables, variable1, hhrEndMib_oid);
}

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
u_char *hhrMirrorGroupSupport_get(struct variable *vp,
                                  oid *name,
                                  size_t *length,
                                  int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t    *pdata;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrMirrorGroupSupport:
            *var_len = sizeof(int);
            int_value = IPRAN_SPLITER_SUPPORT;
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}

u_char *hhrRIPInstanceSupport_get(struct variable *vp,
                                  oid *name,
                                  size_t *length,
                                  int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t    *pdata;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrRIPInstanceSupport:
            *var_len = sizeof(int);
            int_value = IPRAN_SPLITER_SUPPORT;
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}

u_char *hhrOSPFInstanceSupport_get(struct variable *vp,
                                   oid *name,
                                   size_t *length,
                                   int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t    *pdata;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrOSPFInstanceSupport:
            *var_len = sizeof(int);
            int_value = IPRAN_SPLITER_SUPPORT;
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}

u_char *hhrArpSupport_get(struct variable *vp,
                          oid *name,
                          size_t *length,
                          int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t    *pdata;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrArpSupport:
            *var_len = sizeof(int);
            int_value = IPRAN_SPLITER_SUPPORT;
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}

u_char *hhrISISInstanceSupport_get(struct variable *vp,
                                   oid *name,
                                   size_t *length,
                                   int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t    *pdata;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrISISInstanceSupport:
            *var_len = sizeof(int);
            int_value = IPRAN_SPLITER_SUPPORT;
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}

u_char *hhrBGPNeighborSupport_get(struct variable *vp,
                                  oid *name,
                                  size_t *length,
                                  int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t    *pdata;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrBGPNeighborSupport:
            *var_len = sizeof(int);
            int_value = IPRAN_SPLITER_SUPPORT;
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}

u_char *hhrOSPFDCNSupport_get(struct variable *vp,
                              oid *name,
                              size_t *length,
                              int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t    *pdata;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrOSPFDCNSupport:
            *var_len = sizeof(int);
            int_value = IPRAN_SPLITER_SUPPORT;
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}

u_char *hhrDHCPDynamicIPBindSupport_get(struct variable *vp,
                                        oid *name,
                                        size_t *length,
                                        int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t    *pdata;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrDHCPDynamicIPBindSupport:
            *var_len = sizeof(int);
            int_value = IPRAN_SPLITER_SUPPORT;
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}

u_char *hhrVplsServiceSupport_get(struct variable *vp,
                                  oid *name,
                                  size_t *length,
                                  int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t    *pdata;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrVplsServiceSupport:
            *var_len = sizeof(int);
            int_value = IPRAN_SPLITER_SUPPORT;
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}

u_char *hhrMplsLdpSupport_get(struct variable *vp,
                              oid *name,
                              size_t *length,
                              int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t    *pdata;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrMplsLdpSupport:
            *var_len = sizeof(int);
            int_value = IPRAN_SPLITER_SUPPORT;
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}

u_char *hhrL3vpnSupport_get(struct variable *vp,
                            oid *name,
                            size_t *length,
                            int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t    *pdata;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrL3vpnSupport:
            *var_len = sizeof(int);
            int_value = IPRAN_SPLITER_SUPPORT;
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}

u_char *hhrQosMappingSupport_get(struct variable *vp,
                                 oid *name,
                                 size_t *length,
                                 int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t    *pdata;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrQosMappingSupport:
            *var_len = sizeof(int);
            int_value = IPRAN_SPLITER_SUPPORT;
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}

u_char *hhrHQosSupport_get(struct variable *vp,
                           oid *name,
                           size_t *length,
                           int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t    *pdata;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrHQosSupport:
            *var_len = sizeof(int);
            int_value = IPRAN_SPLITER_SUPPORT;
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}

u_char *hhrQosPortLRSupport_get(struct variable *vp,
                                oid *name,
                                size_t *length,
                                int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t    *pdata;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrQosPortLRSupport:
            *var_len = sizeof(int);
            int_value = IPRAN_SPLITER_SUPPORT;
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}

u_char *hhrCE1SubIfSupport_get(struct variable *vp,
                               oid *name,
                               size_t *length,
                               int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t    *pdata;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrCE1SubIfSupport:
            *var_len = sizeof(int);
            int_value = IPRAN_SPLITER_SUPPORT;
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}

u_char *hhrCFMSLASupport_get(struct variable *vp,
                             oid *name,
                             size_t *length,
                             int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t    *pdata;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrCFMSLASupport:
            *var_len = sizeof(int);
            int_value = IPRAN_SPLITER_SUPPORT;
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}

u_char *hhrCFMMepSupport_get(struct variable *vp,
                             oid *name,
                             size_t *length,
                             int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t    *pdata;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrCFMMepSupport:
            *var_len = sizeof(int);
            int_value = IPRAN_SPLITER_SUPPORT;
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}

u_char *hhrCFMETHTestResultSupport_get(struct variable *vp,
                                       oid *name,
                                       size_t *length,
                                       int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t    *pdata;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrCFMETHTestResultSupport:
            *var_len = sizeof(int);
            int_value = IPRAN_SPLITER_SUPPORT;
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}


u_char *hhrCFMSLAResultSupport_get(struct variable *vp,
                                   oid *name,
                                   size_t *length,
                                   int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t    *pdata;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrCFMSLAResultSupport:
            *var_len = sizeof(int);
            int_value = IPRAN_SPLITER_SUPPORT;
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}


u_char *hhrEFMEventSupport_get(struct variable *vp,
                               oid *name,
                               size_t *length,
                               int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t    *pdata;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrEFMEventSupport:
            *var_len = sizeof(int);
            int_value = IPRAN_SPLITER_SUPPORT;
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}

u_char *hhrMplsTpOamMegSupport_get(struct variable *vp,
                                   oid *name,
                                   size_t *length,
                                   int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t    *pdata;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrMplsTpOamMegSupport:
            *var_len = sizeof(int);
            int_value = IPRAN_SPLITER_SUPPORT;
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}

u_char *hhrbfdDetectSupport_get(struct variable *vp,
                                oid *name,
                                size_t *length,
                                int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t    *pdata;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrbfdDetectSupport:
            *var_len = sizeof(int);
            int_value = IPRAN_SPLITER_SUPPORT;
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}

u_char *hhry1564MeasureSupport_get(struct variable *vp,
                                   oid *name,
                                   size_t *length,
                                   int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t    *pdata;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhry1564MeasureSupport:
            *var_len = sizeof(int);
            int_value = IPRAN_SPLITER_SUPPORT;
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}

u_char *hhrMacIfLimitSupport_get(struct variable *vp,
                                 oid *name,
                                 size_t *length,
                                 int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t    *pdata;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrMacIfLimitSupport:
            *var_len = sizeof(int);
            int_value = IPRAN_SPLITER_SUPPORT;
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}

u_char *hhrEndMib_get(struct variable *vp,
                      oid *name,
                      size_t *length,
                      int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t    *pdata;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrEndMib:
            *var_len = sizeof(int);
            int_value = IPRAN_SPLITER_SUPPORT;
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}


