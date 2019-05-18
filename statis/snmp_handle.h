#if 0
#ifndef _SNMP_HANDLE_H
#define _SNMP_HANDLE_H

#if 0
#ifdef GPN_STAT_MIN
#error
#else
#define GPN_STAT_MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif
#endif

#if 1
#define GEN_STAT_DEBUG_PRINT(level, info...)
#else
#define GEN_STAT_DEBUG_PRINT(level, info...) 	printf(info)
#endif


#define GPN_STATMIB_PRINT(level, info...) 	GEN_STAT_DEBUG_PRINT((level), info)
#define GPN_STATMIB_AGP 					1
#define GPN_STATMIB_SVP 					2
#define GPN_STATMIB_CMP 					3
#define GPN_STATMIB_CUP 					4


#define GPN_STAT_MIN(a,b) (((a) < (b)) ? (a) : (b))


/* SNMP value return */
#define SNMP_RETURN_INTEGER(V) \
   do { \
      *var_len = sizeof (int); \
      gpnstat_int_val = V; \
      return (u_char *) &gpnstat_int_val; \
   } while (0)

#define NSM_SNMP_RETURN_TIMETICKS(V) \
	 do { \
	   *var_len = sizeof (u_long); \
	   gpnstat_ulong_val = V; \
	   return (u_char *) &gpnstat_ulong_val; \
	 } while (0)


int snmp_index1_get (struct variable *v, oid * name, size_t * length,
                    u_int32_t * index, int exact);

void snmp_index1_set (struct variable *v, oid * name, size_t * length,
                    u_int32_t index);

int
snmp_index2_get (struct variable *v, oid *name, size_t *length,
                             u_int32_t *index1, u_int32_t *index2, int exact);

void
snmp_index2_set (struct variable *v, oid * name, size_t * length,
                    u_int32_t index1, u_int32_t index2);

int
snmp_index3_get (struct variable *v, oid *name, size_t *length,
                            u_int32_t *index1, u_int32_t *index2,
                            u_int32_t *index3, int exact);
void
snmp_index3_set (struct variable *v, oid * name, size_t * length,
                            u_int32_t index1, u_int32_t index2,
                            u_int32_t index3);
int
snmp_index4_get (struct variable *v, oid *name, size_t *length,
                            u_int32_t *index1, u_int32_t *index2,
                            u_int32_t *index3, u_int32_t *index4, int exact);
void
snmp_index4_set (struct variable *v, oid * name, size_t * length,
                            u_int32_t index1, u_int32_t index2,
                            u_int32_t index3, u_int32_t index4);
/* API for setting five INTEGER indices. */
void
snmp_index5_set (struct variable *v, oid * name, size_t * length,
                            u_int32_t index1, u_int32_t index2,
                            u_int32_t index3, u_int32_t index4,
                            u_int32_t index5);


/* Utility function to get FIVE integer indices.  */
int
snmp_index5_get (struct variable *v, oid *name, size_t *length,
                            u_int32_t *index1, u_int32_t *index2,
                            u_int32_t *index3, u_int32_t *index4,
                            u_int32_t *index5, int exact);

void
snmp_index6_set (struct variable *v, oid * name, size_t * length,
                            u_int32_t index1, u_int32_t index2,
                            u_int32_t index3, u_int32_t index4,
                            u_int32_t index5, u_int32_t index6);
int
snmp_index6_get (struct variable *v, oid *name, size_t *length,
                            u_int32_t *index1, u_int32_t *index2,
                            u_int32_t *index3, u_int32_t *index4,
                            u_int32_t *index5, u_int32_t *index6, int exact);

int
snmp_index7_get (struct variable *v, oid *name, size_t *length,
                            u_int32_t *index1, u_int32_t *index2,
                            u_int32_t *index3, u_int32_t *index4,
                            u_int32_t *index5, u_int32_t *index6, 
                            u_int32_t *index7, int exact);

void
snmp_index7_set (struct variable *v, oid * name, size_t * length,
                            u_int32_t index1, u_int32_t index2,
                            u_int32_t index3, u_int32_t index4,
                            u_int32_t index5, u_int32_t index6, u_int32_t index7);

#endif

#endif
