/*
 * Memory type definitions. This file is parsed by memtypes.awk to extract
 * MTYPE_ and memory_list_.. information in order to autogenerate 
 * memtypes.h.
 *
 * The script is sensitive to the format (though not whitespace), see
 * the top of memtypes.awk for more details.
 */

//#include "zebra.h"
#include <lib/memory.h>

#ifndef	NULL
#define NULL 0
#endif


struct memory_list memory_list_lib[] =
{
  { MTYPE_TMP,			"Temporary memory"		},
  { MTYPE_STRVEC,		"String vector"			},
  { MTYPE_VECTOR,		"Vector"			},
  { MTYPE_VECTOR_INDEX,		"Vector index"			},
  { MTYPE_LINK_LIST,		"Link List"			},
  { MTYPE_LINK_NODE,		"Link Node"			},
  { MTYPE_THREAD,		"Thread"			},
  { MTYPE_VTY,			"VTY"				},
  { MTYPE_VTY_HIST,		"VTY history"			},
  { MTYPE_L2_VLAN,		"L2 vlan"			},
  { MTYPE_IF,			"Interface"			},
  { MTYPE_CONNECTED,		"Connected" 			},
  { MTYPE_CONNECTED_LABEL,	"Connected interface label"	},
  { MTYPE_BUFFER,		"Buffer"			},
  { MTYPE_STREAM,		"Stream"			},
  { MTYPE_PREFIX,		"Prefix"			},
  { MTYPE_PREFIX_IPV4,		"Prefix IPv4"			},
  { MTYPE_PREFIX_IPV6,		"Prefix IPv6"			},
  { MTYPE_HASH,			"Hash"				},
  { MTYPE_HASH_BACKET,		"Hash Bucket"			},
  { MTYPE_HASH_INDEX,		"Hash Index"			},
  { MTYPE_ROUTE_TABLE,		"Route table"			},
  { MTYPE_ROUTE_NODE,		"Route node"			},
  { MTYPE_DISTRIBUTE,		"Distribute list"		},
  { MTYPE_DISTRIBUTE_IFNAME,	"Dist-list ifname"		},
  { MTYPE_ACCESS_LIST,		"Access List"			},
  { MTYPE_ACCESS_LIST_STR,	"Access List Str"		},
  { MTYPE_ACCESS_FILTER,	"Access Filter"			},
  { MTYPE_PREFIX_LIST,		"Prefix List"			},
  { MTYPE_PREFIX_LIST_ENTRY,	"Prefix List Entry"		},
  { MTYPE_PREFIX_LIST_STR,	"Prefix List Str"		},
  { MTYPE_CMD_TOKENS,		"Command desc"			},
  { MTYPE_KEY,			"Key"				},
  { MTYPE_KEYCHAIN,		"Key chain"			},
  { MTYPE_IF_RMAP,		"Interface route map"		},
  { MTYPE_IF_RMAP_NAME,		"I.f. route map name",		},
  { MTYPE_SOCKUNION,		"Socket union"			},
  { MTYPE_PRIVS,		"Privilege information"		},
  { MTYPE_ZLOG,			"Logging"			},
  { MTYPE_ZCLIENT,		"Zclient"			},
  { MTYPE_WORK_QUEUE,		"Work queue"			},
  { MTYPE_WORK_QUEUE_ITEM,	"Work queue item"		},
  { MTYPE_WORK_QUEUE_NAME,	"Work queue name string"	},
  { MTYPE_PQUEUE,		"Priority queue"		},
  { MTYPE_PQUEUE_DATA,		"Priority queue data"		},
  { MTYPE_HOST,			"Host config"			},
  { MTYPE_VRF,			"VRF"				},
  { MTYPE_VRF_NAME,		"VRF name"			},
  { MTYPE_VRF_BITMAP,		"VRF bit-map"			},  
  { MTYPE_PKT_BUFFER,          "PACKET  buff"             },
  { MTYPE_ACL_ENTRY,         "ACL entry"      },
  { MTYPE_IFM_ENTRY,          "IFM entry"       },  
  { MTYPE_LSP_ENTRY,       "LSP entry"      },
  { MTYPE_PW_ENTRY,         "pw entry"	          },
  { MTYPE_VSI_ENTRY,        "vsi entry"},
  { MTYPE_MPLSOAM_SESS,        "mplsoam session entry"},
  { MTYPE_FIB_ENTRY,         "FIB entry" },
  { MTYPE_NHP_ENTRY,       "NHP entry"      },
  { MTYPE_ECMP_ENTRY,        "ECMP entry"       },
  { MTYPE_ARP_ENTRY, "Arp entry"      },
  { MTYPE_ARP_STATIC_ENTRY,          "arp static entry"         },
  { MTYPE_FTM_NHLFE_ENTRY,      "FTM NHLFE entry"               },  
  { MTYPE_FTM_ILM_ENTRY,       "FTM ILM entry"      },
  { MTYPE_FTM_PROTO_REG,         "FTM proto reg"	          },
  { MTYPE_LOG_ITEM,         "LOG item" },  
  { MTYPE_TCP,         "TCP item" },
  { MTYPE_AAA,      "AAA item" },
  { MTYPE_ALARM_ENTRY,      "ALARM item" },
  { MTYPE_STAT,      "STATIS item" },
  { MTYPE_ROUTE_FIFO,     	  "PROTOCAL ROUTE SEND FIFO queue"   },
  { MTYPE_BGP_VRF_ROUTE_INFO, "PROTOCAL VRF ROUTE SEND FIFO queue"     },
  { MTYPE_ROUTEEVENT_FIFO,    "PROTOCAL ROUTE RCV FIFO queue"     },
  { MTYPE_IFMEVENT_FIFO,      "PROTOCAL IFM RCV FIFO queue"     },
  { -1, NULL },
};

struct memory_list memory_list_zebra[] = 
{
  { MTYPE_RTADV_PREFIX,		"Router Advertisement Prefix"	},
  { MTYPE_ZEBRA_VRF,		"ZEBRA VRF"				},
  { MTYPE_NEXTHOP,		"Nexthop"			},
  { MTYPE_RIB,			"RIB"				},
  { MTYPE_RIB_QUEUE,		"RIB process work queue"	},
  { MTYPE_STATIC_ROUTE,		"Static route"			},
  { MTYPE_NETLINK_NAME,	"Netlink name"			},
  { -1, NULL },
};

struct memory_list memory_list_bgp[] =
{
  { MTYPE_BGP,			            "BGP instance"			      },
  { MTYPE_BGP_PEER,			        "BGP peer"			          },
  { MTYPE_BGP_PEER_STR,			    "BGP peer str"			      },
  { MTYPE_BGP_PEER_HOST,			"BGP peer host"			      },
  { MTYPE_PEER_GROUP,			    "BGP peer host"			      },
  { MTYPE_PEER_DESC,			    "BGP peer desc"			      },
  { MTYPE_PEER_PASSWORD,			"BGP peer password"			  },
  { MTYPE_ATTR,			            "BGP attr"			          },
  { MTYPE_ATTR_EXTRA,			    "BGP attr extra"			  },
  { MTYPE_AS_PATH,			        "BGP as path"			      },
  { MTYPE_AS_SEG,			        "BGP as seg"			      },
  { MTYPE_AS_SEG_DATA,			    "BGP as seg data"			  },
  { MTYPE_AS_STR,			        "BGP as str"			      },
  { MTYPE_BGP_TABLE,			    "BGP bgp table"			      },
  { MTYPE_BGP_NODE,			        "BGP node"			          },
  { MTYPE_BGP_INFO,			        "BGP info"			          },
  { MTYPE_BGP_ROUTE_EXTRA,			"BGP route extra"			  },
  { MTYPE_BGP_CONN,			        "BGP conn"			          },
  { MTYPE_BGP_ROUTE,			    "BGP route"			      },
  { MTYPE_BGP_NETWORK,			    "BGP network"			      },
  { MTYPE_BGP_ADVERTISE_ATTR,		"BGP advertise attr"		  },
  { MTYPE_BGP_ADVERTISE,			"BGP advertise"			      },
  { MTYPE_BGP_SYNCHRONISE,			"BGP synchronise"			  },
  { MTYPE_BGP_ADJ_IN,			    "BGP adj in"			      },
  { MTYPE_BGP_ADJ_OUT,			    "BGP adj out"			      },
  { MTYPE_BGP_MPATH_INFO,			"BGP mpath info"			  },
  { MTYPE_BGP_REDIST,			    "BGP redist"			      },
  { MTYPE_BGP_VRF,			        "BGP vrf"			          },
  { MTYPE_BGP_VRF_ROUTE_INFO,		"BGP vrf route info"	      },
  { MTYPE_BGP_ROUTE_RCV_INFO,		"BGP route rcv info"	      },
  { MTYPE_BGP_COMMON_RCV_INFO,      "BGP common rcv info"	      },
  { MTYPE_AS_LIST,			        "BGP as list"			      },
  { MTYPE_AS_FILTER,			    "BGP as filter"			      },
  { MTYPE_AS_FILTER_STR,			"BGP as filter str"			  },
  { MTYPE_COMMUNITY,			    "BGP community"			      },
  { MTYPE_COMMUNITY_VAL,			"BGP community val"			  },
  { MTYPE_COMMUNITY_STR,			"BGP community str"			  },
  { MTYPE_ECOMMUNITY,			    "BGP ecommunity"			  },
  { MTYPE_ECOMMUNITY_VAL,			"BGP ecommunity val"		  },
  { MTYPE_ECOMMUNITY_STR,			"BGP ecommunity str"		  },
  { MTYPE_COMMUNITY_LIST,			"BGP community list"		  },
  { MTYPE_COMMUNITY_LIST_NAME,		"BGP community list name"	  },
  { MTYPE_COMMUNITY_LIST_ENTRY,		"BGP community list entry "	  },
  { MTYPE_COMMUNITY_LIST_CONFIG,	"BGP community list config"	  },
  { MTYPE_COMMUNITY_LIST_HANDLER,	"BGP community list handler"  },
  { MTYPE_CLUSTER,			        "BGP cluster"			      },
  { MTYPE_CLUSTER_VAL,			    "BGP cluster val"			  },
  { MTYPE_BGP_PROCESS_QUEUE,		"BGP process queue"			  },
  { MTYPE_BGP_CLEAR_NODE_QUEUE,		"BGP clear node queue"		  },
  { MTYPE_TRANSIT,			        "BGP transit"			      },	
  { MTYPE_TRANSIT_VAL,			    "BGP transit val"			  },
  { MTYPE_BGP_DISTANCE,			    "BGP distance"			      },
  { MTYPE_BGP_CONFED_LIST,			"BGP confed list"			  },
  { MTYPE_PEER_UPDATE_SOURCE,		"BGP peer update source"	  },
  { MTYPE_BGP_DAMP_INFO,			"BGP damp info"			      },
  { MTYPE_BGP_DAMP_ARRAY,			"BGP damp array"			  },
  { MTYPE_BGP_REGEXP,			    "BGP regexp"			      },
  { MTYPE_BGP_AGGREGATE,			"BGP aggregate"			      },
  { MTYPE_BGP_ADDR,			        "BGP addr"			          },
  { MTYPE_ENCAP_TLV,			    "BGP encap tlv"			      },
  { MTYPE_ROUTE_MAP,			    "BGP route map"			      },
  { MTYPE_ROUTE_MAP_NAME,			"BGP route map name"		  },
  { MTYPE_ROUTE_MAP_INDEX,			"BGP route map index"		  },
  { MTYPE_ROUTE_MAP_RULE,			"BGP route map rule"		  },
  { MTYPE_ROUTE_MAP_RULE_STR,		"BGP route map rule str"	  },
  { MTYPE_ROUTE_MAP_COMPILED,		"BGP route map compiled"	  },
  { -1, NULL }
};

struct memory_list memory_list_rip[] =
{
  { MTYPE_RIP,                "RIP structure"			},
  { MTYPE_RIP_INFO,           "RIP route info"			},
  { MTYPE_RIP_INTERFACE,      "RIP interface"			},
  { MTYPE_RIP_PEER,           "RIP peer"			    },
  { MTYPE_RIP_OFFSET_LIST,    "RIP offset list"			},
  { MTYPE_RIP_DISTANCE,       "RIP distance"			},
  { MTYPE_RIP_REDIST,		  "RIP redistribute"    	},
  { MTYPE_RIP_SUMMARY, 		  "RIP summary info"		},
  { MTYPE_RIPNG,			  "RIPng structure" 		},
  { MTYPE_RIPNG_ROUTE,	      "RIPng route info"		},
  { MTYPE_RIPNG_AGGREGATE, 	  "RIPng aggregate"			},
  { MTYPE_RIPNG_OFFSET_LIST,  "RIPng offset list"		},
  { MTYPE_RIPNG_PEER, 		  "RIPng peer"				},
  { MTYPE_RIPNG_RTE_DATA,	  "RIPng rte data"			},
  { MTYPE_RIPNG_HOP_NODE, 	  "RIPng nexthop"			},
  { -1, NULL }
};

struct memory_list memory_list_ripng[] =
{
  { MTYPE_RIPNG,              "RIPng structure"			},
  { -1, NULL }
};

struct memory_list memory_list_babel[] =
{
  { -1, NULL }
};

struct memory_list memory_list_ospf[] =
{
  { MTYPE_OSPF_TOP,           "OSPF top"			},
  { MTYPE_OSPF_AREA,          "OSPF area"			},
  { MTYPE_OSPF_AREA_RANGE,    "OSPF area range"			},
  { MTYPE_OSPF_NETWORK,       "OSPF network"			},
  { MTYPE_OSPF_NEIGHBOR_STATIC,"OSPF static nbr"		},
  { MTYPE_OSPF_IF,            "OSPF interface"			},
  { MTYPE_OSPF_NEIGHBOR,      "OSPF neighbor"			},
  { MTYPE_OSPF_ROUTE,         "OSPF route"			},
  { MTYPE_OSPF_TMP,           "OSPF tmp mem"			},
  { MTYPE_OSPF_LSA,           "OSPF LSA"			},
  { MTYPE_OSPF_LSA_DATA,      "OSPF LSA data"			},
  { MTYPE_OSPF_LSDB,          "OSPF LSDB"			},
  { MTYPE_OSPF_PACKET,        "OSPF packet"			},
  { MTYPE_OSPF_FIFO,          "OSPF FIFO queue"			},
  { MTYPE_OSPF_VERTEX,        "OSPF vertex"			},
  { MTYPE_OSPF_VERTEX_PARENT, "OSPF vertex parent",		},
  { MTYPE_OSPF_NEXTHOP,       "OSPF nexthop"			},
  { MTYPE_OSPF_PATH,	      "OSPF path"			},
  { MTYPE_OSPF_VL_DATA,       "OSPF VL data"			},
  { MTYPE_OSPF_CRYPT_KEY,     "OSPF crypt key"			},
  { MTYPE_OSPF_EXTERNAL_INFO, "OSPF ext. info"			},
  { MTYPE_OSPF_DISTANCE,      "OSPF distance"			},
  { MTYPE_OSPF_IF_INFO,       "OSPF if info"			},
  { MTYPE_OSPF_IF_PARAMS,     "OSPF if params"			},
  { MTYPE_OSPF_MESSAGE,		"OSPF message"			},
  { -1, NULL },
};

struct memory_list memory_list_ospf6[] =
{
  { MTYPE_OSPF6_TOP,          "OSPF6 top"               },
  { MTYPE_OSPF6_AREA,         "OSPF6 area"              },
  { MTYPE_OSPF6_IF,           "OSPF6 interface"         },
  { MTYPE_OSPF6_NEIGHBOR,     "OSPF6 neighbor"          },
  { MTYPE_OSPF6_ROUTE,        "OSPF6 route"             },
  { MTYPE_OSPF6_LSA,          "OSPF6 LSA"               },
  { MTYPE_OSPF6_LSDB,         "OSPF6 LSDB"              },
  { MTYPE_OSPF6_OTHER,        "OSPF6 nexthop"           },
  { MTYPE_OSPF6_VERTEX,       "OSPF6 vertex"            },
  { MTYPE_OSPF6_EXTERNAL_INFO,"OSPF6 external info"     },
  { MTYPE_OSPF6_REDIST,       "OSPF6 redistribute"      },
  { MTYPE_OSPF6_MESSAGE,      "OSPF6 message"           },
  { -1, NULL },
};

struct memory_list memory_list_isis[] =
{
  { MTYPE_ISIS,               "ISIS"				},
  { MTYPE_ISIS_TMP,           "ISIS TMP"			},
  { MTYPE_ISIS_CIRCUIT,       "ISIS circuit"			},
  { MTYPE_ISIS_LSP,           "ISIS LSP"			},
  { MTYPE_ISIS_ADJACENCY,     "ISIS adjacency"			},
  { MTYPE_ISIS_AREA,          "ISIS instance"			},
  { MTYPE_ISIS_AREA_ADDR,     "ISIS area address"		},
  { MTYPE_ISIS_TLV,           "ISIS TLV"			},
  { MTYPE_ISIS_DYNHN,         "ISIS dyn hostname"		},
  { MTYPE_ISIS_SPFTREE,       "ISIS SPFtree"			},
  { MTYPE_ISIS_VERTEX,        "ISIS vertex"			},
  { MTYPE_ISIS_ROUTE_INFO,    "ISIS route info"			},
  { MTYPE_ISIS_NEXTHOP,       "ISIS nexthop"			},
  { MTYPE_ISIS_NEXTHOP6,      "ISIS nexthop6"			},
  { MTYPE_ISIS_DICT,          "ISIS dictionary"			},
  { MTYPE_ISIS_DICT_NODE,     "ISIS dictionary node"		},
  { -1, NULL },
};

struct memory_list memory_list_pim[] =
{
  { -1, NULL },
};

/*liufy add*/
struct memory_list memory_list_ldpd[] =
{
  { -1, NULL },
};



struct memory_list memory_list_vtysh[] =
{
  { MTYPE_VTYSH_CONFIG,		"Vtysh configuration",		},
  { MTYPE_VTYSH_CONFIG_LINE,	"Vtysh configuration line"	},
  { -1, NULL },
};

/*liufy add*/
struct memory_list memory_list_ptree[] =
{
  { MTYPE_PTREE,       "ptree"      },
  { MTYPE_PTREE_NODE,  "ptree node"},
  { -1, NULL },
};

/*liufy add*/
struct memory_list memory_list_lsp[] =
{
  { -1, NULL },
};


struct mlist mlists[] __attribute__ ((unused)) = {
  { memory_list_lib,	"LIB"	},
  { memory_list_zebra,	"ZEBRA"	},
  { memory_list_rip,	"RIP"	},
  { memory_list_ripng,	"RIPNG"	},
  { memory_list_ospf,	"OSPF"	},
  { memory_list_ospf6,	"OSPF6"	},
  { memory_list_isis,	"ISIS"	},
  { memory_list_bgp,	"BGP"	},
  { memory_list_pim,	"PIM"	},
  { NULL, NULL},
};
