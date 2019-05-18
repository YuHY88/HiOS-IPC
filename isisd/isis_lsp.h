/*
 * IS-IS Rout(e)ing protocol - isis_lsp.h   
 *                             LSP processing
 *
 * Copyright (C) 2001,2002   Sampo Saaristo
 *                           Tampere University of Technology      
 *                           Institute of Communications Engineering
 *
 * This program is free software; you can redistribute it and/or modify it 
 * under the terms of the GNU General Public Licenseas published by the Free 
 * Software Foundation; either version 2 of the License, or (at your option) 
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for 
 * more details.

 * You should have received a copy of the GNU General Public License along 
 * with this program; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef _ZEBRA_ISIS_LSP_H
#define _ZEBRA_ISIS_LSP_H

/* Structure for isis_lsp, this structure will only support the fixed
 * System ID (Currently 6) (atleast for now). In order to support more
 * We will have to split the header into two parts, and for readability
 * sake it should better be avoided */
struct isis_lsp
{
  struct isis_fixed_hdr *isis_header;		/* normally equals pdu */
  struct isis_link_state_hdr *lsp_header;	/* pdu + isis_header_len */
  struct stream *pdu;				/* full pdu lsp */
  union
  {
    struct list *frags;
    struct isis_lsp *zero_lsp;
  } lspu;
  u_int32_t auth_tlv_offset;    /* authentication TLV position in the pdu */
  u_int32_t SRMflags[ISIS_MAX_CIRCUITS];
  u_int32_t SSNflags[ISIS_MAX_CIRCUITS];
  int level;			/* L1 or L2? */
  int scheduled;		/* scheduled for sending */
  time_t installed;
  time_t last_generated;
  int own_lsp;
#ifdef TOPOLOGY_GENERATE
    int from_topology;
    TIMERID t_lsp_top_ref;
#endif
  /* used for 60 second counting when rem_lifetime is zero */
  int age_out;
  struct isis_area *area;
  struct tlvs tlv_data;		/* Simplifies TLV access */
};

void lsp_clear_data (struct isis_lsp *lsp);

void lsp_destroy (struct isis_lsp *lsp);

void lsp_tlv_fit_directly(struct isis_lsp *lsp, struct list **from, struct list **to,
	     int tlvsize, int frag_thold, int tlv_build_func (struct list *, struct stream *, struct isis_area *area));

void lsp_build_pdu (struct isis_lsp *lsp, struct isis_area *area);


void lsp_build_ext_reach_ipv4(struct isis_lsp *lsp, struct isis_area *area,struct tlvs *tlv_data, struct list *summary, int level);

void lsp_purge_some_frags(u_char frag_num, struct isis_lsp *lsp0);

void lsp_summary_metric_deafult(struct isis_area *area);

void lsp_summary_ipv6_metric_deafult(struct isis_area *area);

u_int8_t lsp_bits_generate (int level, int overload_bit, int attached_bit,struct isis_area *area);

void get_extend_summary_list(struct list *summary_l1, struct list *summary_l2, struct list * summary);

int lsp_route_check(struct isis_area *area, struct prefix *route_p);

int lsp_summary_check(struct list *summary, struct prefix *ipv4 ,u_int32_t metric,uchar ext_flag);

int lsp_summary_ipv6_check(struct list *summary, struct prefix *ipv6 ,u_int32_t metric , u_char ext);

void lsp_tlv_add_summary(struct isis_area * area, struct tlvs * tlv_data,struct list * summary);

void lsp_tlv_add_summary_ipv6(struct isis_area *area, struct tlvs *tlv_data, struct list *summary);

dict_t *lsp_db_init (void);

void lsp_db_destroy (dict_t * lspdb);

int lsp_tick (void * argv);

int lsp_generate (struct isis_area *area, int level);

int lsp_regenerate_schedule (struct isis_area *area, int level,int all_pseudo);

int lsp_generate_pseudo (struct isis_circuit *circuit, int level);

int lsp_regenerate_schedule_pseudo (struct isis_circuit *circuit, int level);

struct isis_lsp *lsp_new (struct isis_area *area, u_char * lsp_id,
			  u_int16_t rem_lifetime,
			  u_int32_t seq_num, u_int8_t lsp_bits,
			  u_int16_t checksum, int level);
struct isis_lsp *lsp_new_from_stream_ptr (struct stream *stream,
					  u_int16_t pdu_len,
					  struct isis_lsp *lsp0,
					  struct isis_area *area,
                                          int level);
void lsp_insert (struct isis_lsp *lsp, dict_t * lspdb);
struct isis_lsp *lsp_search (u_char * id, dict_t * lspdb);

void lsp_build_list (u_char * start_id, u_char * stop_id, u_char num_lsps,
		     struct list *list, dict_t * lspdb);
void lsp_build_list_nonzero_ht (u_char * start_id, u_char * stop_id,
				struct list *list, dict_t * lspdb);
void lsp_build_list_ssn (struct isis_circuit *circuit, u_char num_lsps,
                         struct list *list, dict_t * lspdb);

void lsp_search_and_destroy (u_char * id, dict_t * lspdb);
void lsp_purge_pseudo (u_char * id, struct isis_circuit *circuit, int level);
void lsp_purge_non_exist (int level, struct isis_link_state_hdr *lsp_hdr, struct isis_area *area);
void lsp_add_ipv6_interface_addr(struct isis_lsp *lsp, struct tlvs *tlv_data ,struct isis_circuit * circuit);
void lsp_add_from_ipv6_route_table(struct isis_area * area , int lsp_level , struct tlvs * tlv_data , int level);
void lsp_add_router_id(struct isis_lsp * lsp);
void lsp_add_area_addr(struct isis_lsp * lsp);
void lsp_auth_add(struct isis_lsp *lsp);
void lsp_auth_update(struct isis_lsp *lsp);
void lsp_add_ipv4_interface_addr(struct isis_lsp *lsp, struct tlvs *tlv_data ,struct isis_circuit * circuit );
void lsp_add_from_ipv4_route_table(struct isis_area * area , int lsp_level ,struct tlvs * tlv_data , int level);
void lsp_add_te_is_neighbors(struct isis_lsp * lsp, struct isis_circuit * circuit,struct tlvs * tlv_data);
void lsp_add_is_neighbors(struct isis_lsp * lsp, struct isis_circuit * circuit,struct tlvs * tlv_data);

#define LSP_EQUAL 1
#define LSP_NEWER 2
#define LSP_OLDER 3

#define LSP_PSEUDO_ID(I) ((I)[ISIS_SYS_ID_LEN])
#define LSP_FRAGMENT(I) ((I)[ISIS_SYS_ID_LEN + 1])

int lsp_id_cmp (u_char * id1, u_char * id2);
int lsp_compare (char *areatag, struct isis_lsp *lsp, u_int32_t seq_num,
		 u_int16_t checksum, u_int16_t rem_lifetime);
void lsp_update (struct isis_lsp *lsp, struct stream *stream,
                 struct isis_area *area, int level);
void lsp_inc_seqnum (struct isis_lsp *lsp, u_int32_t seq_num);
void lsp_print (struct isis_lsp *lsp, struct vty *vty, char dynhost);
void lsp_print_detail (struct isis_lsp *lsp, struct vty *vty, char dynhost);
int lsp_print_all (struct vty *vty, dict_t * lspdb, char detail,
		   char dynhost);
const char *lsp_bits2string (u_char *);
void lsp_add_nlpid(struct isis_lsp * lsp);


/* sets SRMflags for all active circuits of an lsp */
void lsp_set_all_srmflags (struct isis_lsp *lsp);

#ifdef TOPOLOGY_GENERATE
void generate_topology_lsps (struct isis_area *area);
void remove_topology_lsps (struct isis_area *area);
void build_topology_lsp_data (struct isis_lsp *lsp,
			      struct isis_area *area, int lsp_top_num);
#endif /* TOPOLOGY_GENERATE */

#endif /* ISIS_LSP */
