/* OSPF VTY interface.
 * Copyright (C) 2000 Toshiaki Takada
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef _QUAGGA_OSPF_VTY_H
#define _QUAGGA_OSPF_VTY_H

/* Macros. */
#define VTY_GET_OSPF_AREA_ID(V,F,STR)                                         \
{                                                                             \
  int retv;                                                                   \
  retv = ospf_str2area_id ((STR), &(V), &(F));                                \
  if (retv < 0)                                                               \
    {                                                                         \
      vty_error_out(vty, "Invalid OSPF area ID%s",VTY_NEWLINE);					\
      return CMD_WARNING;                                                     \
    }                                                                         \
}

#define VTY_GET_OSPF_AREA_ID_NO_BB(NAME,V,F,STR)                              \
{                                                                             \
  int retv;                                                                   \
  retv = ospf_str2area_id ((STR), &(V), &(F));                                \
  if (retv < 0)                                                               \
    {                                                                         \
      vty_error_out (vty, "Invalid OSPF area ID.%s", VTY_NEWLINE);                \
      return CMD_WARNING;                                                     \
    }                                                                         \
  if (OSPF_IS_AREA_ID_BACKBONE ((V)))                                        \
    {                                                                         \
      vty_error_out (vty, "You can't configure %s to backbone%s",                \
               NAME, VTY_NEWLINE);                                            \
      return CMD_WARNING;                                                                                                     \
    }                                                                         \
}

#define VTY_CHECK_OSPF_ADDRESS(NAME,STR)                                      \
{                                                                             \
    int pnt ;                                                                                                                                       \
    int a,b,c,d;                                                                                                                                \
    pnt = sscanf(STR,"%d.%d.%d.%d",&a,&b,&c,&d);                                                                \
    if(pnt != 4)                                                                                                                                \
    {                                                                                                                                                       \
        vty_error_out(vty, "Please specify %s by A.B.C.D%s",NAME,VTY_NEWLINE);     \
        return CMD_WARNING;                                                                                                             \
    }                                                                                                                                                       \
}                                                                                                                                                           \
 
#define VTY_INTERFACE_MODE_ERR                                      \
{                                                                       \
    vty_error_out(vty,"Please select mode l3.%s",VTY_NEWLINE);        \
    return CMD_WARNING;                                                 \
}

#define VTY_INTERFACE_PARAMETER_ERR                                      \
{                                                                       \
    vty_error_out(vty,"Interface parameters is NULL.%s",VTY_NEWLINE);        \
    return CMD_WARNING;                                                 \
}


#define VTY_OSPF_INSTANCE_ERR                                      \
{                                                                       \
    vty_error_out(vty,"The specified instance does not exist.%s",VTY_NEWLINE);        \
    return CMD_WARNING;                                                 \
}

#define VTY_CHECK_NM_INTERFACE(V)                                     \
{                                                                       \
    if(IFM_TYPE_IS_OUTBAND (V))               \
    {                                               \
        vty_error_out(vty,"Outband is not support.%s",VTY_NEWLINE);        \
        return CMD_WARNING;                                                 \
    }                                                                       \
}                                                                           \
 
#define VTY_CHECK_OSPF_AREA                                    \
{                                                                       \
    vty_error_out (vty, "Get area node index error.%s", VTY_NEWLINE);        \
    return CMD_WARNING;                                                         \
}                                                                       \
 

#define OSPF_GET_INTEGER_RANGE_HEX(NAME,V,STR,MIN,MAX)                 		\
	do {                                                           			\
	  int retv;                                          			\
	  u_int32_t tmp;                                                        \
	  tmp = ospf_hexstr2int ((STR), &retv);                         			\
	  if (retv == -1 || (tmp) < (MIN) || (tmp) > (MAX))              		\
	    {                                                           		\
	      vty_error_out (vty, "Invalid %s value%s", NAME, VTY_NEWLINE); 		\
	      return CMD_WARNING;                                       		\
	    }                                                             		\
	    (V) = tmp;                                                     		\
	} while (0)

extern int inet_aton(const char *, struct in_addr *);
/* Prototypes. */
extern void ospf_vty_init(void);
extern void ospf_vty_show_init(void);
extern int ospf_str2area_id(const char *, struct in_addr *, int *);
extern u_int32_t ospf_hexstr2int ( char *str, int *ret );


#endif /* _QUAGGA_OSPF_VTY_H */


