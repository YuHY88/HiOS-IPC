#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <lib/ifm_common.h>
#include <lib/module_id.h>
#include <lib/types.h>
#include <lib/inet_ip.h>
#include <lib/ether.h>
#include <lib/syslog.h>
#include <lib/log.h>
#include <lib/inet_ip.h>
#include "cgic.h"


#define	MAX_INTERFACE_ROW_NUM					100
#define	MAX_L3_INTERFACE_COLUMN_NUM				4

typedef struct if_l3_table_info
{
	char			        name[100];
	int                     ifindex;
	enum IP_TYPE            type;
	unsigned int 			ip;
	unsigned int			prefixlen;
	
}if_l3_table_info_t;

static int g_if_l3_entry_num; 
static if_l3_table_info_t	g_if_l3_table[MAX_INTERFACE_ROW_NUM];
uint32_t get_prefixlen(uint32_t mask)
{
	int i;
	for(i = 0; i < 32; i++)
	{
		if(0 == (mask << i))
		{
			return i;
		}
	}
	return 0;

}

uint32_t get_mask(uint32_t prefixlen)
{
	int mask;
	int i;
	mask = 0;
	for(i = 0; i < prefixlen; i++)
	{
		mask |= (1 << (31 - i));
	}
	return mask;

}


void ShowForm()
{	
	unsigned int			i;
	struct in_addr          ip_tmp;

	fprintf(cgiOut,"Content-type: text/html\n\n");
	fprintf(cgiOut, "<HTML><HEAD>\n");
	fprintf(cgiOut, "<TITLE>Interface List</TITLE>\n");
	fprintf(cgiOut, "<META http-equiv=PRAGMA content=NO-CACHE>\n");
	fprintf(cgiOut, "<META http-equiv=Expires content=-1>\n");
	fprintf(cgiOut, "<META http-equiv=Cache-Control content=NO-CACHE>\n");
	fprintf(cgiOut, "<META http-equiv=Content-Type content='text/html; charset=gb2312'> </HEAD>\n");

	fprintf(cgiOut, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/web/r271.css\" />\n");

	fprintf(cgiOut, "<script language=\"JavaScript\">\n\
		function Click(){ window.event.returnValue=false;}	document.oncontextmenu=Click; \n");
	fprintf(cgiOut, "</script></head>\n");
	
	fprintf(cgiOut, "<script language=\"JavaScript\">var L3_SRTPara = new Array( %d, %d, %d );\n",MAX_INTERFACE_ROW_NUM, MAX_L3_INTERFACE_COLUMN_NUM, g_if_l3_entry_num);
	fprintf(cgiOut, "var TypeStr = new Array(\"INVALID\",\"STATIC\",\"UNNUMBERED\",\"SLAVE\");\n");
	

	fprintf(cgiOut, "var InterfaceL3Info = new Array(\n");
	for( i=0; i<MAX_INTERFACE_ROW_NUM; i++ )
	{
		if( i < g_if_l3_entry_num )
		{
			fprintf(cgiOut,"'%s'",g_if_l3_table[i].name);
			fprintf(cgiOut,",%d",g_if_l3_table[i].type);
			ip_tmp.s_addr = g_if_l3_table[i].ip;
			fprintf(cgiOut,",'%s'",inet_ntoa(ip_tmp));
			ip_tmp.s_addr = get_mask(g_if_l3_table[i].prefixlen);
			fprintf(cgiOut,",'%s',",inet_ntoa(ip_tmp));
			
		}
	}
	fprintf(cgiOut, "0);\n");
	
	fprintf(cgiOut, "</script>\n");
	fprintf(cgiOut,"<body> <center> \n");
	fprintf(cgiOut, "<table width=957 border=0 cellspacing=0 cellpadding=0>\n"); 
	fprintf(cgiOut,	"<tr>\n");
		fprintf(cgiOut, "<td width=15 class=\"title_bowl\"></td>\n");
		fprintf(cgiOut, "<td width=950 align=\"left\" valign=\"middle\" class=\"title\">Interface -- > Ipv4 Address</td>\n");
	fprintf(cgiOut,"</tr>\n");

	fprintf(cgiOut,"<tr>\n");
		fprintf(cgiOut,"<td colspan=2>\n");
			fprintf(cgiOut,"<table width=957 border=0 cellspacing=0 cellpadding=0>\n");
				fprintf(cgiOut,"<form action=\"if_l3.cgi\" method=\"post\" name=\"if_l3\">\n");
					fprintf(cgiOut,"<tr>\n");
							fprintf(cgiOut,"<td class=\"vline\" rowspan=15><br> </td>\n");
							fprintf(cgiOut,"<td width=930>  \n");
								fprintf(cgiOut, " <table width=880 border=0 align=\"center\" cellpadding=1 cellspacing=1 class=\"space\">\n");	
									fprintf(cgiOut, "<tr>\n");
										fprintf(cgiOut,"<td colspan=%d height=\"1\">&nbsp</td>\n",MAX_L3_INTERFACE_COLUMN_NUM);
									fprintf(cgiOut, "</tr>\n");

									fprintf(cgiOut, "<tr >\n");
									fprintf(cgiOut,"<td align=\"center\" width=6.7%%  class=\"row\">Port</td>\n");
									fprintf(cgiOut,"<td align=\"center\" width=6.7%%  class=\"row\">Address Type</td>\n");
									fprintf(cgiOut,"<td align=\"center\" width=6.7%%  class=\"row\">IP Address</td>\n");
									fprintf(cgiOut,"<td align=\"center\" width=6.7%%  class=\"row\">IP Mask</td>\n");
									fprintf(cgiOut, "</tr>\n");
		
									fprintf(cgiOut, "<script language=\"JavaScript\"> \n");
									fprintf(cgiOut, "if (L3_SRTPara[2]>0){	var row=0; var i=0;\n\
														for (i=0;(i< L3_SRTPara[0]) && (i< L3_SRTPara[2]);i++)\n\
														{\n\
														document.write('<tr>');\n\
														row = i*L3_SRTPara[1];\n\
														document.write('<td align=\"left\">&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp'+InterfaceL3Info[row]+' <input  maxLength=30 size=30 name=\"input_if_name_'+i+'\"   value=\"'+InterfaceL3Info[row]+'\" type=\"hidden\"  ></td>');\n\
														document.write('<td align=\"center\">'+TypeStr[InterfaceL3Info[row+1]]+'</td>');\n\
														document.write('<td align=\"center\"> <input  maxLength=15 size=18 name=\"input_if_ip_'+i+'\"   value='+InterfaceL3Info[row+2]+'></td>');\n\
														document.write('<td align=\"center\"> <input  maxLength=15 size=18 name=\"input_if_mask_'+i+'\" value='+InterfaceL3Info[row+3]+'></td>');\n\
														document.write('</tr>');}}\n");
									fprintf(cgiOut, "</script>\n");
								fprintf(cgiOut, "</table>\n");
							fprintf(cgiOut,"</td>\n");
							fprintf(cgiOut, "<td class=\"vline\" rowspan=15><br> </td>\n");
					fprintf(cgiOut,"</tr>\n");	
					fprintf(cgiOut,"<tr><td class=\"hline\"><img src=\"/web/empty.gif\" width=1 height=1></td></tr>\n");
					fprintf(cgiOut,"<tr>\n");
						fprintf(cgiOut,"<td class=\"tail\" align=\"center\">\n");
							fprintf(cgiOut,"<input type=\"submit\" value=\"Set\" name=\"submit_ip_set\" class=\"button\" ;\" >\n");
							fprintf(cgiOut,"&nbsp&nbsp&nbsp&nbsp\n");
							fprintf(cgiOut,"<input type=\"submit\" value=\"Get\" name=\"submit_ip_get\" class=\"button\" ;\" >\n");
						fprintf(cgiOut,"</td>\n");
					fprintf(cgiOut,"</tr>\n");
					fprintf(cgiOut,"<tr><td class=\"hline\"><img src=\"/web/empty.gif\" width=1 height=\"1\"></td></tr> \n");
				fprintf(cgiOut,"</form>\n");
			fprintf(cgiOut, "</table>\n");
		fprintf(cgiOut,"</td>\n");
	fprintf(cgiOut, "</table>\n");
	fprintf(cgiOut, "</center>\n");
	fprintf(cgiOut, "</body>\n");
	fprintf(cgiOut, "</html>\n");
}

int if_l3_comp_inc(const void *a, const void *b)  
{  
    return (*(if_l3_table_info_t *)a).ifindex - (*(if_l3_table_info_t *)b).ifindex;  
}  

int get_subif_ip_all(void)
{
		int i;
		int pos;
		int ifindex;
		int if_num;
		int if_type;
		char is_subport;
		char if_name[100];
		struct ifm_l3 *p_ifl3 = NULL;
		struct ifm_info *p_ifinfo = NULL;
		struct ifm_info *tp_ifinfo = NULL;
		pos = 0;
		ifindex = 0;
	
		do{
			p_ifinfo = ifm_get_bulk(ifindex,MODULE_ID_WEB, &if_num );
			tp_ifinfo = p_ifinfo;
			zlog_debug("[%s %d] if_num : %d\n",__FUNCTION__, __LINE__, if_num);
			for(i = 0 ; i < if_num ; i++)
			{	
				if_type = IFM_TYPE_ID_GET(p_ifinfo[i].ifindex);
				is_subport = IFM_IS_SUBPORT(p_ifinfo[i].ifindex);
				ifindex    = p_ifinfo[i].ifindex;
				ifm_get_name_by_ifindex(ifindex, if_name);
				zlog_debug("[%s %d] %s ,ifindex : %d\n",__FUNCTION__, __LINE__, if_name,ifindex);
				if((IFNET_TYPE_ETHERNET == if_type) && (1==is_subport) )
				{	
					if(pos < MAX_INTERFACE_ROW_NUM)
					{	
						g_if_l3_table[pos].ifindex 	 = ifindex;
						ifm_get_name_by_ifindex(ifindex, g_if_l3_table[pos].name);
						p_ifl3 = ifm_get_l3if(ifindex,MODULE_ID_WEB);
						if(NULL == p_ifl3)
						{
							g_if_l3_table[pos].type 	 = 0;
							g_if_l3_table[pos].ip		 = 0;
							g_if_l3_table[pos].prefixlen = 0;
							zlog_debug("[%s %d] %s ,ifindex : %d can't get p_ifl3 information!\n",__FUNCTION__, __LINE__, if_name,ifindex);
						}
						else
						{
							g_if_l3_table[pos].type 	 = p_ifl3->ipv4_flag;
							g_if_l3_table[pos].ip		 = p_ifl3->ipv4[0].addr;
							g_if_l3_table[pos].prefixlen = p_ifl3->ipv4[0].prefixlen;
	
						}
						zlog_debug("[%s %d] subif %s , type :%d, ip: %d,prefixlen : %d\n",\
									__FUNCTION__, __LINE__, g_if_l3_table[pos].name,g_if_l3_table[pos].type,g_if_l3_table[pos].ip,g_if_l3_table[pos].prefixlen);
					}
					pos++;
				}
			}
			mem_share_free_bydata(tp_ifinfo, MODULE_ID_WEB);
		}while(if_num > 0);
		g_if_l3_entry_num = pos;

		qsort(g_if_l3_table, g_if_l3_entry_num, sizeof(g_if_l3_table[0]), if_l3_comp_inc);
		return 0;
}

int cgiMain(int argc, char *argv[]) 
{
	int i;
	int ret;
	int pos;
	int ifindex;
	unsigned int	prefixlen;
	char input_if_name[100];
	char input_if_ip[100];
	char input_if_mask[100];
	char str_if_name[100];
	char str_if_ip[100];
	char str_if_mask[100];	
	char str_pt_type[20];
	char str_pt_name[20];
	char *p_str;
	struct in_addr 			ip_addr;
	struct in_addr 			mask_addr;
	struct ifm_l3 l3if;
	char *p;
	char *progname;;
	progname = ((p = strrchr (argv[0], '/')) ? ++p : argv[0]);
	zlog_default = openzlog(progname, ZLOG_WEB,LOG_CONS | LOG_NDELAY | LOG_PID, LOG_DAEMON);

	get_subif_ip_all();

	if (cgiFormSubmitClicked("submit_ip_set") == cgiFormSuccess) 
	{
		for(i = 0 ; i < MAX_INTERFACE_ROW_NUM; i++)
		{
			sprintf(input_if_name,"input_if_name_%d",i);
			if(cgiFormStringNoNewlines(input_if_name,str_if_name,100) != cgiFormSuccess)
			{
				continue ;
			}
			
			zlog_debug("[%s %d] if_name : %s\n",__FUNCTION__, __LINE__, str_if_name);
			sprintf(input_if_ip,"input_if_ip_%d",i);
			if(cgiFormStringNoNewlines(input_if_ip,str_if_ip,100) == cgiFormSuccess)
			{
				zlog_debug("[%s %d] if_ip : %s\n",__FUNCTION__, __LINE__, str_if_ip);
				ret = inet_aton (str_if_ip, &ip_addr);
				if (! ret)
				{
					continue ;
				}
				ret = inet_valid_ipv4(ip_addr.s_addr);
				if (!ret)
				{
					continue ;
				}
				
			}
			else
			{
				continue ;
			}
			
			sprintf(input_if_mask,"input_if_mask_%d",i);
			if(cgiFormStringNoNewlines(input_if_mask,str_if_mask,100) == cgiFormSuccess)
			{
				zlog_debug("[%s %d] if_mask : %s\n",__FUNCTION__, __LINE__, str_if_mask);
				ret = inet_aton (str_if_mask, &mask_addr);
				if (! ret)
				{
					continue ;
				}
				ret = inet_valid_mask(mask_addr.s_addr);
				if (! ret)
				{
					continue ;
				}
				prefixlen = get_prefixlen(mask_addr.s_addr);
			}
			else
			{
				continue ;
			}

			for(pos = 0 ; pos < MAX_INTERFACE_ROW_NUM; pos++)
			{
				if(0== strcmp(str_if_name , g_if_l3_table[pos].name))
				{
					if((ip_addr.s_addr != g_if_l3_table[pos].ip )|| (prefixlen !=  g_if_l3_table[pos].prefixlen))
					{
						p_str = strtok(str_if_name," ");
						if(NULL != p_str)
						{
							strcpy(str_pt_type, p_str);
						}
						p_str = strtok(NULL,str_if_name );
						if(NULL != p_str)
						{
							strcpy(str_pt_name, p_str);
						}
						 memset(&l3if, 0 ,sizeof(l3if));
						 l3if.vpn = 0;
						 l3if.ipv4_flag = IP_TYPE_STATIC;
						 l3if.ipv4[0].addr = ip_addr.s_addr;
						 l3if.ipv4[0].prefixlen = prefixlen;
						 ifindex = ifm_get_ifindex_by_name(str_pt_type,str_pt_name);
						 if(0== ifindex)
						 {
						 	 
							 zlog_err("[%s %d]ERROR:  type : %s OR pt_name : %s error!!  \n", __FUNCTION__, __LINE__, str_pt_type,str_pt_name);
							 continue ;
						 }
						 zlog_debug("[%s %d] ethernet %s , l3if.vpn :%d, l3if.ipv4_flag: %d,l3if.ipv4[0].addr : %d, l3if.ipv4[0].prefixlen : %d, subtype: %d, module_id : %d\n",\
									 __FUNCTION__, __LINE__, str_pt_name,l3if.vpn,l3if.ipv4_flag,l3if.ipv4[0].addr,l3if.ipv4[0].prefixlen, IFNET_EVENT_IP_ADD, MODULE_ID_WEB);
						 ifm_set_l3if(ifindex, &l3if, IFNET_EVENT_IP_ADD, MODULE_ID_WEB);
					}
				}
			}
		}

		get_subif_ip_all();
	}

	ShowForm();
	return 0;
}


