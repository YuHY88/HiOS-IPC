#ifndef _ZEBRA_ISIS_MSG_H
#define _ZEBRA_ISIS_MSG_H

struct isis_instance_entry
{
  u_int32_t instance;	
  u_int32_t level;
  u_int32_t metric_type;
  u_int32_t lsp_gen_interval;
  u_int32_t lsp_refresh;
  u_int32_t max_lsp_lifetime;
  u_int32_t lsp_mtu;
  u_int32_t spf_interval;
  u_int32_t overloadbit;
  u_int32_t attachedbit;
  u_int32_t hellopadding;
  u_int32_t distance;
  u_int32_t level1_to_level2;
  u_int32_t level2_to_level1;
};

struct isis_netentity_entry
{
  u_int32_t instance;
  u_int32_t netentity_len;
  u_char netentity[20];
};

struct isis_circuit_entry
{
   u_int32_t ifindex;
   u_int32_t instance;
   char  name[INTERFACE_NAMSIZ + 1];
   u_int32_t passive;
   u_int32_t circuit_type;
   u_int32_t circuit_level;
   int password_type;
   u_char passwd[255];
   u_int32_t priority;
   u_int32_t metric;
   u_int32_t hello_interval;
   u_int32_t hello_multiplier;
   u_int32_t csnp_interval;
   u_int32_t psnp_interval;
};


void isis_pkt_init(void);
void isis_pkt_register(void);

#endif

