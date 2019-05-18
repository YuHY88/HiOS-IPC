/* Copyright (C) 2001-2011 IP Infusion, Inc. All Rights Reserved. */

#ifndef _SNMP_INDEX_OPERATOR_H
#define _SNMP_INDEX_OPERATOR_H

struct vlan_table
{
    int vlanid;
};

#define L2_SNMP_PROTO_VAL_LEN               2
#define L2_SNMP_FRAME_TYPE_MIN              0
#define L2_SNMP_FRAME_TYPE_MAX              5
#define IPI_CLASS_FRAME_TYPE_ETHER          1
#define IPI_CLASS_FRAME_TYPE_RFC1042        2
#define IPI_CLASS_FRAME_TYPE_SNAP8021H      3
#define IPI_CLASS_FRAME_TYPE_SNAPOTHER      4
#define IPI_CLASS_FRAME_TYPE_LLCOTHER       5
#define L2_SNMP_FALSE                      -1

#define min(A,B) ((A) < (B) ? (A) : (B))

#define ETHER_ADDR_LEN                         6

struct mac_addr
{
    unsigned char addr[ETHER_ADDR_LEN];
};

void oid_copy_bytes2oid(oid oid[], void *addr, int len);
int oid2bytes(oid oid[], int len, void *addr);

extern int ipran_snmp_int_index_get(struct variable *v, oid *name, size_t *length, u_int32_t *index, int exact);
extern void ipran_snmp_int_index_set(struct variable *v, oid *name, size_t *length, u_int32_t index);

extern int ipran_snmp_intx2_index_get(struct variable *v, oid *name, size_t *length, u_int32_t *index1, u_int32_t *index2, int exact);
extern void ipran_snmp_intx2_index_set(struct variable *v, oid *name, size_t *length, u_int32_t index1, u_int32_t index2);

extern int ipran_snmp_intx3_index_get(struct variable *v, oid *name, size_t *length, u_int32_t *index1, u_int32_t *index2, u_int32_t *index3, int exact);
extern void ipran_snmp_intx3_index_set(struct variable *v, oid *name, size_t *length, u_int32_t index1, u_int32_t index2, u_int32_t index3);

int
ipran_snmp_intx4_index_get(struct variable *v, oid *name, size_t *length,
                           u_int32_t *index1, u_int32_t *index2,
                           u_int32_t *index3, u_int32_t *index4, int exact);
void
ipran_snmp_intx4_index_set(struct variable *v, oid *name, size_t *length,
                           u_int32_t index1, u_int32_t index2,
                           u_int32_t index3, u_int32_t index4);
/* API for setting five INTEGER indices. */
void
ipran_snmp_intx5_index_set(struct variable *v, oid *name, size_t *length,
                           u_int32_t index1, u_int32_t index2,
                           u_int32_t index3, u_int32_t index4,
                           u_int32_t index5);

/* Utility function to get FIVE integer indices.  */
int
ipran_snmp_intx5_index_get(struct variable *v, oid *name, size_t *length,
                           u_int32_t *index1, u_int32_t *index2,
                           u_int32_t *index3, u_int32_t *index4,
                           u_int32_t *index5, int exact);


void
ipran_snmp_intx6_index_set(struct variable *v, oid *name, size_t *length,
                           u_int32_t index1, u_int32_t index2,
                           u_int32_t index3, u_int32_t index4,
                           u_int32_t index5, u_int32_t index6);

int
ipran_snmp_intx6_index_get(struct variable *v, oid *name, size_t *length,
                           u_int32_t *index1, u_int32_t *index2,
                           u_int32_t *index3, u_int32_t *index4,
                           u_int32_t *index5, u_int32_t *index6, int exact);

void
ipran_snmp_intx7_index_set(struct variable *v, oid *name, size_t *length,
                           u_int32_t index1, u_int32_t index2,
                           u_int32_t index3, u_int32_t index4,
                           u_int32_t index5, u_int32_t index6,
                           u_int32_t index7);

int
ipran_snmp_intx7_index_get(struct variable *v, oid *name, size_t *length,
                           u_int32_t *index1, u_int32_t *index2,
                           u_int32_t *index3, u_int32_t *index4,
                           u_int32_t *index5, u_int32_t *index6,
                           u_int32_t *index7, int exact);


/* Utility function to get  one ip indices.  */
int
ipran_snmp_ip_index_get(struct variable *v, oid *name, size_t *length,
                        u_int32_t *ipvalue,  int exact);

/* Utility function to set  one ip indices.  */
void
ipran_snmp_ip_index_set(struct variable *v, oid *name, size_t *length,
                        u_int32_t ip_value);

/* Utility function to get  two ip indices.  */
int
ipran_snmp_ip2_index_get(struct variable *v, oid *name, size_t *length,
                         u_int32_t *ipvalue1, uint32_t *ipvalue2, int exact);

/* Utility function to set  two ip indices.  */
void
ipran_snmp_ip2_index_set(struct variable *v, oid *name, size_t *length,
                         u_int32_t ip_value1, uint32_t ip_value2);

/* Utility function : index[ip ip integer ip] */
int
ipran_snmp_ipx2_int_ip_index_get(struct variable *v, oid *name, size_t *length,
                                 u_int32_t *ipvalue1, u_int32_t *ipvalue2, u_int32_t *index,  u_int32_t *ipvalue3 , int exact) ;

void
ipran_snmp_ipx2_int_ip_index_set(struct variable *v, oid *name, size_t *length,
                                 u_int32_t ip_value1 , u_int32_t ip_value2 , u_int32_t index, u_int32_t ip_value3) ;

/* Utility function : index[ip ip integer ip integer]*/
int
ipran_snmp_ipx2_int_ip_int_index_get(struct variable *v, oid *name, size_t *length,
                                     u_int32_t *ipvalue1, u_int32_t *ipvalue2, u_int32_t *index,  u_int32_t *ipvalue3 , u_int32_t *index2, int exact);

void
ipran_snmp_ipx2_int_ip_int_index_set(struct variable *v, oid *name, size_t *length,
                                     u_int32_t ip_value1 , u_int32_t ip_value2 , u_int32_t index, u_int32_t ip_value3 , u_int32_t index2);

/* Utility function to get one integer one ip indices.  */
int
ipran_snmp_int_ip_index_get(struct variable *v, oid *name, size_t *length,
                            u_int32_t *index, u_int32_t *ipvalue,  int exact);
/* Utility function to set one integer one ip indices.  */
void
ipran_snmp_int_ip_index_set(struct variable *v, oid *name, size_t *length,
                            u_int32_t index, u_int32_t ip_value1);

/* Utility function to get one ip  one integer  indices.  */
int
ipran_snmp_ip_int_index_get(struct variable *v, oid *name, size_t *length,
                            u_int32_t *ipvalue, u_int32_t *index,  int exact);
/* Utility function to set one integer one ip indices.  */
void
ipran_snmp_ip_int_index_set(struct variable *v, oid *name, size_t *length,
                            u_int32_t ip_value1, u_int32_t index);

int
ipran_snmp_intx2_ip_int_index_get(struct variable *v, oid *name, size_t *length,
                                  u_int32_t *index1, u_int32_t *index2,
                                  u_int32_t *ipvalue1, u_int32_t *index3, int exact) ;
void
ipran_snmp_intx2_ip_int_index_set(struct variable *v, oid *name, size_t *length,
                                  u_int32_t index1, u_int32_t index2, u_int32_t ip_value1 , u_int32_t index3) ;

int
ipran_snmp_ip_intx2_ip_index_get(struct variable *v, oid *name, size_t *length,
                                 u_int32_t *ipvalue1, u_int32_t *index1,
                                 u_int32_t *index2, u_int32_t *ipvalue2, int exact) ;

void
ipran_snmp_ip_intx2_ip_index_set(struct variable *v, oid *name, size_t *length,
                                 u_int32_t ip_value1, u_int32_t index1, u_int32_t index2 , u_int32_t ip_value2) ;


/* Utility function to get one integer + oct string(fixed length) index.  */
int
ipran_snmp_int_implicitstr_index_get(struct variable *v, oid *name, size_t *length,
                                     u_int32_t *index, u_int8_t *buff, int buff_len, int exact);
void
ipran_snmp_int_implicitstr_index_set(struct variable *v, oid *name, size_t *length,
                                     u_int32_t index, u_int8_t *buff, int buff_len);

//float64_t
//time_ticks (pal_time_t time1, pal_time_t time0);

/* bit string utilities. SNMP bit strings start at the MSBit and move to the
 * right for increasing bit numbers. There must enough bytes in the string to
 * contain each multiple of 8 bits.
 */

#define BITSTRINGSIZE(n) (((n)+7)/8)

char *bitstring_init(char *bstring, char value, int bstring_length);
int bitstring_testbit(char *bstring, int number);
void bitstring_setbit(char *bstring, int number);
void bitstring_clearbit(char *bstring, int number);
int bitstring_count(char *bstring, int bstring_length);
void *
l2_bsearch_next(const void *key, const void *base0, size_t nelements,
                size_t element_size, int (*compar)(const void *, const void *));
int
ipran_snmp_int_mac_index_get(struct variable *v, oid *name, size_t *length,
                             int *idx, unsigned char *mac_addr, int exact);
void
ipran_snmp_int_mac_index_set(struct variable *v, oid *name, size_t *length,
                             int idx, unsigned char *mac_addr);

/* Utility function to set a mac_addr index + Integer inde  */
int
ipran_snmp_mac_int_index_get(struct variable *v, oid *name, size_t *length,
                             u_int16_t *idx, u_char *oct_string, int oct_len, int exact);
void
ipran_snmp_mac_int_index_set(struct variable *v, oid *name, size_t *length,
                             u_int16_t idx, u_char *oct_string, int oct_len);

/* Utility function to set a Integer Index + oct_stringlen + oct_string*/
int
ipran_snmp_int_str_index_get(struct variable *v, oid *name, size_t *length,
                             u_int32_t *index, u_char *oct_string, int max_str_len,  int *oct_stringlen, int exact);
int
ipran_snmp_int_str_index_set(struct variable *v, oid *name, size_t *length,
                             u_int32_t index, u_char *oct_string, int oct_len);

/* Utility function to set a oct_stringlen + oct_string + Integer Index */
int
ipran_snmp_str_int_index_get(struct variable *v, oid *name, size_t *length,
                             u_char *oct_string, int max_str_len, int *oct_stringlen, u_int32_t *index, int exact);
int
ipran_snmp_str_int_index_set(struct variable *v, oid *name, size_t *length,
                             u_char *oct_string, int oct_len , u_int32_t index);


/* Utility function to set two Integer Index + oct_stringlen + oct_string*/
int
ipran_snmp_intx2_str_index_get(struct variable *v, oid *name, size_t *length,
                               u_int32_t *index1, u_int32_t *index2, u_char *oct_string, int max_str_len, int *out_str_len , int exact);
void
ipran_snmp_intx2_str_index_set(struct variable *v, oid *name, size_t *length,
                               u_int32_t index1, u_int32_t index2, u_char *oct_string, int oct_len);

#endif /* _ZEBOS_L2_SNMP_H */



