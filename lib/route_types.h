

#include "route_com.h"

/* Zebra route's types. */
#define ZEBRA_ROUTE_SYSTEM               0
#define ZEBRA_ROUTE_KERNEL               1
#define ZEBRA_ROUTE_CONNECT              2
#define ZEBRA_ROUTE_STATIC               3
#define ZEBRA_ROUTE_RIP                  4
#define ZEBRA_ROUTE_RIPNG                5
#define ZEBRA_ROUTE_OSPF                 6
#define ZEBRA_ROUTE_OSPF6                7
#define ZEBRA_ROUTE_ISIS                 8
#define ZEBRA_ROUTE_BGP                  9
#define ZEBRA_ROUTE_PIM                  10
#define ZEBRA_ROUTE_HSLS                 11
#define ZEBRA_ROUTE_OLSR                 12
#define ZEBRA_ROUTE_BABEL                13
#define ZEBRA_ROUTE_MAX                  14

#define SHOW_ROUTE_V4_HEADER \
  "Codes: K - kernel route, C - connected, S - static, R - RIP,%s" \
  "       O - OSPF, I - IS-IS, B - BGP, P - PIM, A - Babel,%s" \
  "       > - selected route, * - FIB route%s%s", \
  VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE
#define SHOW_ROUTE_V6_HEADER \
  "Codes: K - kernel route, C - connected, S - static, R - RIPng,%s" \
  "       O - OSPFv6, I - IS-IS, B - BGP, A - Babel,%s" \
  "       > - selected route, * - FIB route%s%s", \
  VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE

/* babeld */
#define QUAGGA_REDIST_STR_BABELD \
  "(kernel|connected|static|rip|ripng|ospf|ospf6|isis|bgp|pim)"
#define QUAGGA_REDIST_HELP_STR_BABELD \
  "Kernel routes (not installed via the zebra RIB)\n" \
  "Connected routes (directly attached subnet or host)\n" \
  "Statically configured routes\n" \
  "Routing Information Protocol (RIP)\n" \
  "Routing Information Protocol next-generation (IPv6) (RIPng)\n" \
  "Open Shortest Path First (OSPFv2)\n" \
  "Open Shortest Path First (IPv6) (OSPFv3)\n" \
  "Intermediate System to Intermediate System (IS-IS)\n" \
  "Border Gateway Protocol (BGP)\n" \
  "Protocol Independent Multicast (PIM)\n"
#define QUAGGA_IP_REDIST_STR_BABELD \
  "(kernel|connected|static|rip|ospf|isis|bgp|pim)"
#define QUAGGA_IP_REDIST_HELP_STR_BABELD \
  "Kernel routes (not installed via the zebra RIB)\n" \
  "Connected routes (directly attached subnet or host)\n" \
  "Statically configured routes\n" \
  "Routing Information Protocol (RIP)\n" \
  "Open Shortest Path First (OSPFv2)\n" \
  "Intermediate System to Intermediate System (IS-IS)\n" \
  "Border Gateway Protocol (BGP)\n" \
  "Protocol Independent Multicast (PIM)\n"
#define QUAGGA_IP6_REDIST_STR_BABELD \
  "(kernel|connected|static|ripng|ospf6|isis|bgp)"
#define QUAGGA_IP6_REDIST_HELP_STR_BABELD \
  "Kernel routes (not installed via the zebra RIB)\n" \
  "Connected routes (directly attached subnet or host)\n" \
  "Statically configured routes\n" \
  "Routing Information Protocol next-generation (IPv6) (RIPng)\n" \
  "Open Shortest Path First (IPv6) (OSPFv3)\n" \
  "Intermediate System to Intermediate System (IS-IS)\n" \
  "Border Gateway Protocol (BGP)\n"

/* bgpd */
#define QUAGGA_REDIST_STR_BGPD \
  "(kernel|connected|static|rip|ripng|ospf|ospf6|isis|pim|babel)"
#define QUAGGA_REDIST_HELP_STR_BGPD \
  "Kernel routes (not installed via the zebra RIB)\n" \
  "Connected routes (directly attached subnet or host)\n" \
  "Statically configured routes\n" \
  "Routing Information Protocol (RIP)\n" \
  "Routing Information Protocol next-generation (IPv6) (RIPng)\n" \
  "Open Shortest Path First (OSPFv2)\n" \
  "Open Shortest Path First (IPv6) (OSPFv3)\n" \
  "Intermediate System to Intermediate System (IS-IS)\n" \
  "Protocol Independent Multicast (PIM)\n" \
  "Babel routing protocol (Babel)\n"
#define QUAGGA_IP_REDIST_STR_BGPD \
  "(connected|static)"
#define QUAGGA_IP_REDIST_HELP_STR_BGPD \
  "Connected routes (directly attached subnet or host)\n" \
  "Statically configured routes\n"
  #define QUAGGA_IP_REDIST_STR_ROI_BGPD \
  "(rip|ospf|isis) <1-255>"
  #define QUAGGA_IP_REDIST_HELP_STR_ROI_BGPD \
  "Routing Information Protocol (RIP)\n" \
  "Open Shortest Path First (OSPFv2)\n" \
  "Intermediate System to Intermediate System (IS-IS)\n"\
  "Instance number\n"
  #define QUAGGA_IP6_REDIST_STR_ROI_BGPD \
  "(ripng|ospfv3|isis) <1-255>"
#define QUAGGA_IP6_REDIST_HELP_STR_ROI_BGPD \
  "Routing Information Protocol for IPv6 (RIPng) routes\n" \
  "Open Shortest Path First (OSPFv3) routes\n" \
  "Intermediate System to Intermediate System (IS-IS)\n"\
  "Instance number\n"
#define QUAGGA_IP6_REDIST_STR_BGPD \
  "(kernel|connected|static|ripng|ospf6|isis|babel)"
#define QUAGGA_IP6_REDIST_HELP_STR_BGPD \
  "Kernel routes (not installed via the zebra RIB)\n" \
  "Connected routes (directly attached subnet or host)\n" \
  "Statically configured routes\n" \
  "Routing Information Protocol next-generation (IPv6) (RIPng)\n" \
  "Open Shortest Path First (IPv6) (OSPFv3)\n" \
  "Intermediate System to Intermediate System (IS-IS)\n" \
  "Babel routing protocol (Babel)\n"

/* isisd */
#define QUAGGA_REDIST_STR_ISISD \
  "(connected|static|rip|ospf|bgp)"
#define QUAGGA_REDIST_HELP_STR_ISISD \
  "Kernel routes (not installed via the zebra RIB)\n" \
  "Connected routes (directly attached subnet or host)\n" \
  "Statically configured routes\n" \
  "Routing Information Protocol (RIP)\n" \
  "Routing Information Protocol next-generation (IPv6) (RIPng)\n" \
  "Open Shortest Path First (OSPFv2)\n" \
  "Open Shortest Path First (IPv6) (OSPFv3)\n" \
  "Border Gateway Protocol (BGP)\n" \
  "Protocol Independent Multicast (PIM)\n" \
  "Babel routing protocol (Babel)\n"
#define QUAGGA_IP_REDIST_STR_ISISD \
  "(kernel|connected|static|rip|ospf|bgp|pim|babel)"
#define QUAGGA_IP_REDIST_HELP_STR_ISISD \
  "Kernel routes (not installed via the zebra RIB)\n" \
  "Connected routes (directly attached subnet or host)\n" \
  "Statically configured routes\n" \
  "Routing Information Protocol (RIP)\n" \
  "Open Shortest Path First (OSPFv2)\n" \
  "Border Gateway Protocol (BGP)\n" \
  "Protocol Independent Multicast (PIM)\n" \
  "Babel routing protocol (Babel)\n"
#define QUAGGA_IP6_REDIST_STR_ISISD \
  "(kernel|connected|static|ripng|ospf6|bgp|babel)"
#define QUAGGA_IP6_REDIST_HELP_STR_ISISD \
  "Kernel routes (not installed via the zebra RIB)\n" \
  "Connected routes (directly attached subnet or host)\n" \
  "Statically configured routes\n" \
  "Routing Information Protocol next-generation (IPv6) (RIPng)\n" \
  "Open Shortest Path First (IPv6) (OSPFv3)\n" \
  "Border Gateway Protocol (eBGP)\n" \
  "Border Gateway Protocol (iBGP)\n" 


/* ospf6d */
#define QUAGGA_REDIST_STR_OSPF6D \
  "(connected|static|ebgp|ibgp)"
#define QUAGGA_REDIST_HELP_STR_OSPF6D \
  "Connected routes (directly attached subnet or host)\n" \
  "Statically configured routes\n" \
  "Intermediate System to Intermediate System (IS-IS)\n" \
  "Border Gateway Protocol (eBGP)\n" \
  "Border Gateway Protocol (iBGP)\n" 

#define QUAGGA_REDIST_STR_OSPF6D_RI \
  "(ripng |isis |ospf6) "
#define QUAGGA_REDIST_HELP_STR_OSPF6D_RI \
  "Routing Information Protocol next-generation (IPv6) (RIPng)\n" \
  "Intermediate System to Intermediate System (IS-IS)\n" \
  "Open Shortest Path First (IPv6) (OSPFv3)\n" 


/* ospfd */
#define QUAGGA_REDIST_STR_OSPFD \
  "(connected|static|ebgp|ibgp)"
#define QUAGGA_REDIST_HELP_STR_OSPFD \
  "Connected routes (directly attached subnet or host)\n" \
  "Statically configured routes\n" \
  "Border Gateway Protocol (EBGP)\n" \
  "Border Gateway Protocol (IBGP)\n" \

#define QUAGGA_REDIST_STR_OSPFD_RI \
  "(rip |isis |ospf) "
#define QUAGGA_REDIST_HELP_STR_OSPFD_RI \
  "Routing Information Protocol (RIP)\n" \
  "Intermediate System to Intermediate System (IS-IS)\n" \
  "Open Shortest Path First (OSPFv2)\n" \



/* pimd */
#define QUAGGA_REDIST_STR_PIMD \
  "(kernel|connected|static|rip|ospf|isis|bgp|babel)"
#define QUAGGA_REDIST_HELP_STR_PIMD \
  "Kernel routes (not installed via the zebra RIB)\n" \
  "Connected routes (directly attached subnet or host)\n" \
  "Statically configured routes\n" \
  "Routing Information Protocol (RIP)\n" \
  "Open Shortest Path First (OSPFv2)\n" \
  "Intermediate System to Intermediate System (IS-IS)\n" \
  "Border Gateway Protocol (BGP)\n" \
  "Babel routing protocol (Babel)\n"

/* ripd */
#define QUAGGA_REDIST_STR_RIPD \
  "(kernel|connected|static|ospf|isis|bgp|pim|babel)"
#define QUAGGA_REDIST_HELP_STR_RIPD \
  "Kernel routes (not installed via the zebra RIB)\n" \
  "Connected routes (directly attached subnet or host)\n" \
  "Statically configured routes\n" \
  "Open Shortest Path First (OSPFv2)\n" \
  "Intermediate System to Intermediate System (IS-IS)\n" \
  "Border Gateway Protocol (BGP)\n" \
  "Protocol Independent Multicast (PIM)\n" \
  "Babel routing protocol (Babel)\n"

/* ripngd */
#define QUAGGA_REDIST_STR_RIPNGD \
  "(kernel|connected|static|ospf6|isis|bgp|babel)"
#define QUAGGA_REDIST_HELP_STR_RIPNGD \
  "Kernel routes (not installed via the zebra RIB)\n" \
  "Connected routes (directly attached subnet or host)\n" \
  "Statically configured routes\n" \
  "Open Shortest Path First (IPv6) (OSPFv3)\n" \
  "Intermediate System to Intermediate System (IS-IS)\n" \
  "Border Gateway Protocol (BGP)\n" \
  "Babel routing protocol (Babel)\n"

/* zebra */
#define QUAGGA_REDIST_STR_ZEBRA \
  "(kernel|connected|static|rip|ripng|ospf|ospf6|isis|bgp|pim|babel)"
#define QUAGGA_REDIST_HELP_STR_ZEBRA \
  "Kernel routes (not installed via the zebra RIB)\n" \
  "Connected routes (directly attached subnet or host)\n" \
  "Statically configured routes\n" \
  "Routing Information Protocol (RIP)\n" \
  "Routing Information Protocol next-generation (IPv6) (RIPng)\n" \
  "Open Shortest Path First (OSPFv2)\n" \
  "Open Shortest Path First (IPv6) (OSPFv3)\n" \
  "Intermediate System to Intermediate System (IS-IS)\n" \
  "Border Gateway Protocol (BGP)\n" \
  "Protocol Independent Multicast (PIM)\n" \
  "Babel routing protocol (Babel)\n"
#define QUAGGA_IP_REDIST_STR_ZEBRA \
  "(kernel|connected|static|rip|ospf|isis|bgp|pim|babel)"
#define QUAGGA_IP_REDIST_HELP_STR_ZEBRA \
  "Kernel routes (not installed via the zebra RIB)\n" \
  "Connected routes (directly attached subnet or host)\n" \
  "Statically configured routes\n" \
  "Routing Information Protocol (RIP)\n" \
  "Open Shortest Path First (OSPFv2)\n" \
  "Intermediate System to Intermediate System (IS-IS)\n" \
  "Border Gateway Protocol (BGP)\n" \
  "Protocol Independent Multicast (PIM)\n" \
  "Babel routing protocol (Babel)\n"
#define QUAGGA_IP6_REDIST_STR_ZEBRA \
  "(kernel|connected|static|ripng|ospf6|isis|bgp|babel)"
#define QUAGGA_IP6_REDIST_HELP_STR_ZEBRA \
  "Kernel routes (not installed via the zebra RIB)\n" \
  "Connected routes (directly attached subnet or host)\n" \
  "Statically configured routes\n" \
  "Routing Information Protocol next-generation (IPv6) (RIPng)\n" \
  "Open Shortest Path First (IPv6) (OSPFv3)\n" \
  "Intermediate System to Intermediate System (IS-IS)\n" \
  "Border Gateway Protocol (BGP)\n" \
  "Babel routing protocol (Babel)\n"



