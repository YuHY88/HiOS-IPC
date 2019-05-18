
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
//#include <arpa/inet.h>
#include "cgic.h"
#include <lib/ifm_common.h>
#include <lib/module_id.h>
#include <lib/syslog.h>
#include <lib/log.h>

#define	MAX_INTERFACE_ROW_NUM						100
#define	MAX_INTERFACE_COLUMN_NUM					6
#define MAX_PIC_INTERFACE_MAX                       100

typedef struct if_basic_table_info
{
	char			        name[100];
	int                     ifindex;
	unsigned int			en;	
	unsigned int 			link;
	unsigned char			encap_type;
	unsigned char			svlan;
	unsigned int			cvlan;
	char			        mac[20];
	
}if_basic_table_info_t;

static int g_if_basic_entry_num; 
static if_basic_table_info_t	g_if_basic_table[MAX_INTERFACE_ROW_NUM];


void ShowForm()
{	
	int i;
	fprintf(cgiOut,"Content-type: text/html\n\n");
	fprintf(cgiOut, "<HTML><HEAD>\n");
	fprintf(cgiOut, "<TITLE>Interface List</TITLE>\n");
	fprintf(cgiOut, "<META http-equiv=PRAGMA content=NO-CACHE>\n");
	fprintf(cgiOut, "<META http-equiv=Expires content=-1>\n");
	fprintf(cgiOut, "<META http-equiv=Cache-Control content=NO-CACHE>\n");
	fprintf(cgiOut, "<META http-equiv=Content-Type content='text/html; charset=gb2312'> </HEAD>\n");

	fprintf(cgiOut, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/web/r271.css\" />\n");

	fprintf(cgiOut, "<script language=\"JavaScript\">\n");
		fprintf(cgiOut,"function Click(){ window.event.returnValue=false;}	document.oncontextmenu=Click; \n");
		fprintf(cgiOut,"function goUrl(url)	{	location.href =url;	}\n");
	fprintf(cgiOut, "</script></head>\n");
	

	fprintf(cgiOut, "<script language=\"JavaScript\">var SRTPara = new Array( %d, %d, %d );\n",MAX_INTERFACE_ROW_NUM, MAX_INTERFACE_COLUMN_NUM , g_if_basic_entry_num);
	fprintf(cgiOut, "var EncapStr = new Array(\"INVALID\",\"UNTAG\",\"DOT1Q\",\"QINQ\");\n");
	fprintf(cgiOut, "var StatusStr = new Array(\"UP\",\"Down\" );\n");
	fprintf(cgiOut, "var EnableStr = new Array(\"Enable\",\"Disable\" );\n");
	fprintf(cgiOut, "var InterfaceList = new Array(\n");

	for( i=0; i<g_if_basic_entry_num; i++ )
	{
			fprintf( cgiOut, " '%s',  %d,  %d,  %d, %d, %d,  \n", 
							g_if_basic_table[i].name, g_if_basic_table[i].en , 
							g_if_basic_table[i].link,g_if_basic_table[i].encap_type,
							g_if_basic_table[i].svlan, g_if_basic_table[i].cvlan);
	}
	

	fprintf(cgiOut, "0);\n");
	fprintf(cgiOut, "</script>\n");

	fprintf(cgiOut,"<body> <center> \n");
	fprintf(cgiOut, "<table width=957 border=0 cellspacing=0 cellpadding=0>\n"); 
	fprintf(cgiOut,	"<tr>\n");
		fprintf(cgiOut, "<td width=15 class=\"title_bowl\"></td>\n");
		fprintf(cgiOut, "<td width=950 align=\"left\" valign=\"middle\" class=\"title\">Interface -- > Port</td>\n");
	fprintf(cgiOut,"</tr>\n");

	fprintf(cgiOut,"<tr>\n");
		fprintf(cgiOut,"<td colspan=2>\n");
			fprintf(cgiOut,"<table width=957 border=0 cellspacing=0 cellpadding=0>\n");
				fprintf(cgiOut,"<tr>\n");
						fprintf(cgiOut,"<td class=\"vline\" rowspan=15><br> </td>\n");
						fprintf(cgiOut,"<td width=930>  \n");
							fprintf(cgiOut, " <table width=880 border=0 align=\"center\" cellpadding=1 cellspacing=1 class=\"space\">\n");	
								fprintf(cgiOut, "<tr>\n");
									fprintf(cgiOut,"<td colspan=%d height=\"1\">&nbsp</td>\n",MAX_INTERFACE_COLUMN_NUM);
								fprintf(cgiOut, "</tr>\n");
								fprintf(cgiOut, "<tr >\n");
									fprintf(cgiOut,"<td align=\"center\" width=%d%%  class=\"row\">Port</td>\n",100/MAX_INTERFACE_COLUMN_NUM);
									fprintf(cgiOut,"<td align=\"center\" width=%d%%  class=\"row\">AdminStatus</td>\n",100/MAX_INTERFACE_COLUMN_NUM);
									fprintf(cgiOut,"<td align=\"center\" width=%d%%  class=\"row\">OperStatus</td>\n",100/MAX_INTERFACE_COLUMN_NUM);
									fprintf(cgiOut,"<td align=\"center\" width=%d%%  class=\"row\">Encap Type</td>\n",100/MAX_INTERFACE_COLUMN_NUM);
									fprintf(cgiOut,"<td align=\"center\" width=%d%%  class=\"row\">Outer Vlan</td>\n",100/MAX_INTERFACE_COLUMN_NUM);
									fprintf(cgiOut,"<td align=\"center\" width=%d%%  class=\"row\">Inner Vlan</td>\n",100/MAX_INTERFACE_COLUMN_NUM);
								fprintf(cgiOut, "</tr>\n");
								fprintf(cgiOut, "<script language=\"JavaScript\">  \n");
								fprintf(cgiOut, "if (SRTPara[2]>0){	 var row=0; var i=0;\n\
														for (i=0;(i< SRTPara[0]) && (i< SRTPara[2]);i++)\n\
													{\n\
													document.write('<tr>');\n\
													row = i*SRTPara[1];\n\
													document.write('<td align=\"left\">'+InterfaceList[row]+'</td>');\n\
													document.write('<td align=\"center\">'+EnableStr[InterfaceList[row+1]]+'</td>');\n\
													document.write('<td align=\"center\">'+StatusStr[InterfaceList[row+2]]+'</td>');\n\
													document.write('<td align=\"center\">'+EncapStr[InterfaceList[row+3]]+'</td>');\n\
													document.write('<td align=\"center\">'+InterfaceList[row+4]+'</td>');\n\
													document.write('<td align=\"center\">'+InterfaceList[row+5]+'</td>');\n\
													document.write('</tr>');}}\n");
								fprintf(cgiOut, "</script>	\n");
							fprintf(cgiOut, "</table>\n");
						fprintf(cgiOut,"</td>\n");
						fprintf(cgiOut, "<td class=\"vline\" rowspan=15><br> </td>\n");
				fprintf(cgiOut,"</tr>\n");	
				fprintf(cgiOut,"<tr><td class=\"hline\"><img src=\"/web/empty.gif\" width=1 height=1></td></tr>\n");
				fprintf(cgiOut,"<tr>\n");
					fprintf(cgiOut,"<form action=\"if_basic.cgi\" method=\"post\" name=\"if_basic_get\">\n");
						fprintf(cgiOut,"<td class=\"tail\" align=\"center\">\n");
							fprintf(cgiOut,"<input type=\"submit\" value=\"Add Port\" name=\"btn_add_if\" class=\"button\"	 >\n");
							fprintf(cgiOut,"&nbsp&nbsp&nbsp&nbsp\n");
							fprintf(cgiOut,"<input type=\"submit\" value=\"Get\" name=\"submit_get_if\" class=\"button\"  ;\" >\n");
						fprintf(cgiOut,"</td>\n");
					fprintf(cgiOut,"</form>\n");
				fprintf(cgiOut,"</tr>\n");
				fprintf(cgiOut,"<tr><td class=\"hline\"><img src=\"/web/empty.gif\" width=1 height=\"1\"></td></tr>  \n");

			fprintf(cgiOut, "</table>\n");
		fprintf(cgiOut,"</td>\n");
	fprintf(cgiOut, "</table>\n");
	fprintf(cgiOut, "</center>\n");
	fprintf(cgiOut, "</body>\n");
	fprintf(cgiOut, "</html>\n");



}
int if_basic_comp_inc(const void *a, const void *b)  
{  
    return (*(if_basic_table_info_t *)a).ifindex - (*(if_basic_table_info_t *)b).ifindex;  
}  


int get_subif_all(void)
{
	int i,pos;
	int ifindex;
	int if_num;
	int port_type;
	char is_subport;
	char if_name[100];
	struct ifm_info *p_ifinfo;
	struct ifm_info *tp_ifinfo;
	pos = 0;
	ifindex = 0;
	do{
		p_ifinfo = ifm_get_bulk(ifindex,MODULE_ID_WEB, &if_num );
		tp_ifinfo = p_ifinfo;
		zlog_debug("[%s %d] if_num : %d\n",__FUNCTION__, __LINE__, if_num);
		for(i = 0 ; i < if_num ; i++)
		{	
			port_type  = IFM_TYPE_ID_GET(p_ifinfo[i].ifindex);
			is_subport = IFM_IS_SUBPORT(p_ifinfo[i].ifindex);
			ifindex    = p_ifinfo[i].ifindex;
			ifm_get_name_by_ifindex(ifindex, if_name);
			zlog_debug("[%s %d] %s ,ifindex : %d\n",__FUNCTION__, __LINE__, if_name,ifindex);
			if((IFNET_TYPE_ETHERNET == port_type) && (1 ==is_subport) )
			{	
				if(pos < MAX_INTERFACE_ROW_NUM)
				{
					g_if_basic_table[pos].ifindex   = p_ifinfo[i].ifindex;
					ifm_get_name_by_ifindex(p_ifinfo[i].ifindex, g_if_basic_table[pos].name);
					g_if_basic_table[pos].en   = p_ifinfo[i].shutdown;
					g_if_basic_table[pos].link = p_ifinfo[i].status;
					g_if_basic_table[pos].encap_type = p_ifinfo[i].encap.type;
					if(0 == p_ifinfo[i].encap.cvlan.vlan_start)
					{
						g_if_basic_table[pos].svlan= 0;
						g_if_basic_table[pos].cvlan= p_ifinfo[i].encap.svlan.vlan_start;
					}
					else
					{
						g_if_basic_table[pos].svlan= p_ifinfo[i].encap.svlan.vlan_start;
						g_if_basic_table[pos].cvlan= p_ifinfo[i].encap.cvlan.vlan_start;
					}
					zlog_debug("[%s %d] subif %s , en :%d, link: %d,encap : %d, svlan: %d, cvlan : %d,\n",\
								__FUNCTION__, __LINE__, g_if_basic_table[pos].name,g_if_basic_table[pos].en,g_if_basic_table[pos].link,\
								g_if_basic_table[pos].encap_type,g_if_basic_table[pos].svlan,g_if_basic_table[pos].cvlan);
					pos++;
				}
			}
		}
		mem_share_free_bydata(tp_ifinfo, MODULE_ID_WEB);
	}while(if_num > 0);
	g_if_basic_entry_num = pos;
	qsort(g_if_basic_table, g_if_basic_entry_num, sizeof(g_if_basic_table[0]), if_basic_comp_inc);

	return 0;
}

int cgiMain(int argc, char *argv[]) 
{
	char *p;
	char *progname;;
	progname = ((p = strrchr (argv[0], '/')) ? ++p : argv[0]);
	zlog_default = openzlog(progname, ZLOG_WEB,LOG_CONS | LOG_NDELAY | LOG_PID, LOG_DAEMON);
#if 1
	
	if(cgiFormSubmitClicked("btn_add_if") == cgiFormSuccess) 
	{
		zlog_debug("[%s %d] btn_add_if\n",__FUNCTION__, __LINE__);
		fprintf(cgiOut,"Content-type: text/html\n\n");
		fprintf(cgiOut, "<HTML>\n");
		fprintf(cgiOut,"<body> <center> \n");
		fprintf(cgiOut, "<script language=\"JavaScript\">\n");
		fprintf(cgiOut,"function goUrl(url)	{	location.href =url;	}\n");
		fprintf(cgiOut,"goUrl('if_attr.cgi') \n");		
		fprintf(cgiOut, "</script>\n");
		fprintf(cgiOut, "</center>\n");
		fprintf(cgiOut, "</body>\n");
		fprintf(cgiOut, "</html>\n");
	}
	else
	{
		get_subif_all();
		ShowForm();
	}
#endif
	return 0;
}














