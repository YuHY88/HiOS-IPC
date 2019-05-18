
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
#define ENCAP_QINQ   0
#define ENCAP_DOT1Q  1
#define	MAX_INTERFACE_ROW_NUM						100

static int g_if_parent_num;

typedef struct if_parent_table_info
{
	char name[100];
	int  ifindex;

}if_parent_table_info_t;

if_parent_table_info_t g_if_parent_table[MAX_INTERFACE_ROW_NUM];


void ShowForm()
{	
	unsigned int			i;

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


	
#if 1	
		fprintf(cgiOut,"function SelectFreeze(sel_value)\n\
						{\n\
							var outer_vlan = document.getElementById(\"input_outer_vlan_id\")\n\
							if(sel_value == 0)\n\
							{\n\
								outer_vlan.disabled=false\n\
								outer_vlan.style.backgroundColor='#FFFFFF' \n\
							}\n\
							else if(sel_value == 1)\n\
							{\n\
								outer_vlan.disabled=true\n\
								outer_vlan.style.backgroundColor='#EEEEEE' \n\
								outer_vlan.value=0\n\
							}\n\
						}\n"); 
#endif					
	fprintf(cgiOut, "</script></head>\n");

	fprintf(cgiOut,"<body> <center> \n");
	fprintf(cgiOut, "<table width=602 border=0 cellspacing=0 cellpadding=0>\n"); 
	fprintf(cgiOut,	"<tr>\n");
		fprintf(cgiOut, "<td width=7 class=\"title_bowl\"></td>\n");
		fprintf(cgiOut, "<td width=592 align=\"left\" valign=\"middle\" class=\"title\">Interface  -- > Add Port </td>\n");
	fprintf(cgiOut,"</tr>\n");

	fprintf(cgiOut,"<tr>\n");
		fprintf(cgiOut,"<td colspan=2>\n");
			fprintf(cgiOut,"<table width=602 border=0 cellspacing=0 cellpadding=0>\n");
				fprintf(cgiOut,"<form action=\"if_attr.cgi\" method=\"post\" name=\"if_attr\">\n");
					fprintf(cgiOut,"<tr>\n");
							fprintf(cgiOut,"<td class=\"vline\" rowspan=15><br> </td>\n");
							fprintf(cgiOut,"<td width=600>  \n");
								fprintf(cgiOut, " <table width=520 border=0 align=\"center\" cellpadding=1 cellspacing=1 class=\"space\">\n");	
									fprintf(cgiOut, "<tr>\n");
										fprintf(cgiOut,"<td>Type: </td>\n");
										fprintf(cgiOut,"<td>Ethernet Subif</td>\n");
									fprintf(cgiOut, "<tr>\n");
										
									fprintf(cgiOut, "<tr>\n");
										fprintf(cgiOut,"<td>Father Port: </td>\n");
										fprintf(cgiOut,"<td><SELECT id=\"sel_if_parent_id\"	name=\"sel_if_parent\"  class=\"list\"  >");

										for(i = 0; i < g_if_parent_num; i++)
										{
											fprintf(cgiOut,"<OPTION id=\"sel_opt_if_parent_id_%d\">%s</OPTION>",i,g_if_parent_table[i].name);	
										}
										fprintf(cgiOut,"</SELECT></td>\n");
									fprintf(cgiOut, "</tr>\n");
										
									fprintf(cgiOut, "<tr>\n");
										fprintf(cgiOut, "<td align=\"left\" >Port Id</td>");
										fprintf(cgiOut, "<td align=\"left\" ><INPUT maxLength=15 size=8 name=\"input_if_name\" value=\"%d\" > </td>", 100);
									fprintf(cgiOut, "</tr>\n");
									
									fprintf(cgiOut, "<tr>\n");
										fprintf(cgiOut,"<td>Encap Type: </td>\n");
										fprintf(cgiOut,"<td><SELECT id=\"sel_encap_type_id\" name=\"sel_encap_type\"  class=\"list\" onChange=\"SelectFreeze(this.selectedIndex)\">\
													<OPTION id=\"sel_opt_encap_type_1\" selected = \"selected\" >QinQ</OPTION>	<OPTION id=\"sel_opt_encap_type_2\">DOT1.Q</OPTION> </SELECT></td>\n");
									fprintf(cgiOut, "</tr>\n");
										
									fprintf(cgiOut, "<tr>\n");
										fprintf(cgiOut, "<td align=\"left\" >Outer Vlan</td>");
										fprintf(cgiOut, "<td align=\"left\" ><INPUT maxLength=15 size=8 id=\"input_outer_vlan_id\" name=\"input_outer_vlan\" value=\"%d\" > </td>", 0);
									fprintf(cgiOut, "</tr>\n");
									
									fprintf(cgiOut, "<tr>\n");
										fprintf(cgiOut, "<td align=\"left\" >Inner Vlan</td>");
										fprintf(cgiOut, "<td align=\"left\" ><INPUT maxLength=15 size=8 id=\"input_inner_vlan_id\" name=\"input_inner_vlan\" value=\"%d\" > </td>", 0);
									fprintf(cgiOut, "</tr>\n");
#if 0					
									fprintf(cgiOut, "<tr>\n");
										fprintf(cgiOut,"<td>TPID: </td>\n");
										fprintf(cgiOut,"<td><SELECT name=\"if_attr_tpid\" class=\"list\">\
															<OPTION selecte_tpid_1>0x8100</OPTION>\
															<OPTION selecte_tpid_2>0x9100</OPTION>\
															<OPTION selecte_tpid_2>0x88a8</OPTION>\
															</SELECT></td>\n");
									fprintf(cgiOut, "<tr>\n");
										
									fprintf(cgiOut, "<tr>\n");
										fprintf(cgiOut, "<td align=\"left\" >Jumbo</td>");
										fprintf(cgiOut, "<td align=\"left\" ><INPUT maxLength=15 size=20 name=\"if_attr_jumbo\" value=\"%d\" > </td>", 100);
									fprintf(cgiOut, "</tr>\n");
									
									fprintf(cgiOut, "<tr>\n");
										fprintf(cgiOut,"<td>Negotiation: </td>\n");
										fprintf(cgiOut,"<td><SELECT name=\"if_attr_neg\" class=\"list\">\
															<OPTION selecte_neg_1>enable</OPTION>\
															<OPTION selecte_neg_2>disable</OPTION>\
															</SELECT></td>\n");
									fprintf(cgiOut, "<tr>\n");

									fprintf(cgiOut, "<tr>\n");
										fprintf(cgiOut,"<td>Speed: </td>\n");
										fprintf(cgiOut,"<td><SELECT name=\"if_attr_speed\" class=\"list\">\
															<OPTION selecte_speed_1>10M </OPTION>\
															<OPTION selecte_speed_2>100M</OPTION>\
															<OPTION selecte_speed_3>1000M</OPTION>\
															</SELECT></td>\n");
									fprintf(cgiOut, "<tr>\n");
									fprintf(cgiOut, "<tr>\n");
										fprintf(cgiOut,"<td>Admin: </td>\n");
										fprintf(cgiOut,"<td><SELECT id=\"sel_if_admin_id\" name=\"sel_if_admin\" class=\"list\">\
															<OPTION id=\"sel_opt_if_admin_1_id\">enable </OPTION>\
															<OPTION id=\"sel_opt_if_admin_1_id\">disable</OPTION>\
															</SELECT></td>\n");
									fprintf(cgiOut, "</tr>\n");
#endif
								fprintf(cgiOut, "</table>\n");
							fprintf(cgiOut,"</td>\n");
							fprintf(cgiOut, "<td class=\"vline\" rowspan=15><br> </td>\n");
					fprintf(cgiOut,"</tr>\n");	
					fprintf(cgiOut,"<tr><td class=\"hline\"><img src=\"/web/empty.gif\" width=1 height=1></td></tr>\n");
					fprintf(cgiOut,"<tr>\n");
						fprintf(cgiOut,"<td class=\"tail\" align=\"center\">\n");
							fprintf(cgiOut,"<input type=\"submit\" value=\"Add\"  name=\"if_attr_add\" class=\"button\" onClick=\"return goUrl('if_basic.cgi');\">\n");
							fprintf(cgiOut,"&nbsp;&nbsp;&nbsp;&nbsp;\n");
							fprintf(cgiOut,"<input type=\"button\" value=\"Back\" name=\"if_addr_back\" class=\"button\" onClick=\"return goUrl('if_basic.cgi');\" >\n");
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

int if_attr_comp_inc(const void *a, const void *b)  
{  
    return (*(if_parent_table_info_t *)a).ifindex - (*(if_parent_table_info_t *)b).ifindex;  
}  

int get_parif_all(void)
{
		int i;
		int pos;
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
				port_type = IFM_TYPE_ID_GET(p_ifinfo[i].ifindex);
				is_subport = IFM_IS_SUBPORT(p_ifinfo[i].ifindex);
				ifindex    = p_ifinfo[i].ifindex;
				ifm_get_name_by_ifindex(ifindex, if_name);
				zlog_debug("[%s %d] %s ,ifindex : %d\n",__FUNCTION__, __LINE__, if_name,ifindex);
				if(((IFNET_TYPE_ETHERNET == port_type)||(IFNET_TYPE_GIGABIT_ETHERNET == port_type)||(IFNET_TYPE_XGIGABIT_ETHERNET == port_type)) && (0 == is_subport)&&(0!= IFM_SLOT_ID_GET ( ifindex )))
				{	
					if(pos < MAX_INTERFACE_ROW_NUM)
					{	
						g_if_parent_table[pos].ifindex = ifindex;
						ifm_get_name_by_ifindex(ifindex, g_if_parent_table[pos].name);
						zlog_debug("[%s %d] subif %s \n",__FUNCTION__, __LINE__, g_if_parent_table[pos].name);
					}
					pos++;
				}
			}
			mem_share_free_bydata(tp_ifinfo, MODULE_ID_WEB);
		}while(if_num > 0);
		g_if_parent_num = pos;
		qsort(g_if_parent_table, g_if_parent_num, sizeof(g_if_parent_table[0]), if_attr_comp_inc);
		return 0;
}


int cgiMain(int argc, char *argv[]) 
{
	int i;
	int ifindex;
	int encap_type_sel;
	int parent_sel;
	int outer_vlan_id;
	int inner_vlan_id;
	char str_pt_type[20];
	char str_pt_name[20];
	char str_subpt_name[20];
	char *p_parent_name[MAX_INTERFACE_ROW_NUM];
	char *encap_type[] = {"QinQ","DOT1.Q"};
	char *p_str;
	char *p;
	char *progname;;
	
	progname = ((p = strrchr (argv[0], '/')) ? ++p : argv[0]);
	zlog_default = openzlog(progname, ZLOG_WEB,LOG_CONS | LOG_NDELAY | LOG_PID, LOG_DAEMON);
	
	memset(p_parent_name, 0 ,sizeof(p_parent_name));
	
	get_parif_all();

	if (cgiFormSubmitClicked("if_attr_add") == cgiFormSuccess)	
	{
		zlog_debug("[%s %d] if_attr_add\n",__FUNCTION__, __LINE__);
		cgiFormStringNoNewlines("input_if_name",str_subpt_name,100); 
		
		zlog_debug("[%s %d] input_if_name : %s\n",__FUNCTION__, __LINE__,str_subpt_name);
		cgiFormSelectSingle("sel_encap_type",encap_type,2,&encap_type_sel,-1);
		zlog_debug("[%s %d] encap_type_sel : %d\n",__FUNCTION__, __LINE__,encap_type_sel);
		if(ENCAP_QINQ == encap_type_sel )
		{
			cgiFormInteger("input_outer_vlan",&outer_vlan_id, -1);
			if(outer_vlan_id < 1 || outer_vlan_id > 4095)
			{
				zlog_err("[%s %d]ERROR: outvlan : %d \n", __FUNCTION__, __LINE__, outer_vlan_id);
				goto ERROR;
		//	continue;
			}
			zlog_debug("[%s %d] outer_vlan_id : %d\n",__FUNCTION__, __LINE__,outer_vlan_id);
		}
		cgiFormInteger("input_inner_vlan",&inner_vlan_id, -1);
		if(inner_vlan_id < 1 || inner_vlan_id > 4095)
		{
			zlog_err("[%s %d]ERROR: innvlan : %d \n", __FUNCTION__, __LINE__, inner_vlan_id);
			goto ERROR;
		}
		zlog_debug("[%s %d] inner_vlan_id : %d\n",__FUNCTION__, __LINE__,inner_vlan_id);

		for(i = 0; i < g_if_parent_num; i++)
		{
			p_parent_name[i] = g_if_parent_table[i].name;
		}
		
		cgiFormSelectSingle("sel_if_parent",p_parent_name,g_if_parent_num,&parent_sel,-1);
		if(-1 == parent_sel)
		{
			zlog_err("[%s %d]ERROR: Can't get parent ifname  \n", __FUNCTION__, __LINE__);
			goto ERROR;
		}
		zlog_debug("[%s %d] sel parent name  : %s\n",__FUNCTION__, __LINE__,p_parent_name[parent_sel]);
		p_str = strtok(p_parent_name[parent_sel]," ");
		if(NULL != p_str)
		{
			strcpy(str_pt_type, p_str);
		}
		p_str = strtok(NULL,p_parent_name[parent_sel] );
		if(NULL != p_str)
		{
			strcpy(str_pt_name, p_str);
		}
		strcat(str_pt_name,".");
		strcat(str_pt_name,str_subpt_name);
		fprintf(cgiOut,"str_pt_name %s \n",str_pt_name);
		ifindex = ifm_get_ifindex_by_name(str_pt_type,str_pt_name);
		if(0 == ifindex)
		{
			zlog_err("[%s %d]ERROR: Can't get parent ifindex  \n", __FUNCTION__, __LINE__);
			goto ERROR;
		}

		if(ENCAP_QINQ == encap_type_sel )
		{
			zlog_debug("[%s %d] add subif name : %s , ifindex :%d , outvlan : %d, innervlan : %d,   module_id : %d\n",\
				__FUNCTION__, __LINE__,str_pt_name, ifindex, outer_vlan_id, inner_vlan_id, MODULE_ID_WEB);
			ifm_set_subif(ifindex, outer_vlan_id, inner_vlan_id, MODULE_ID_WEB);
		}
		else
		{
			zlog_debug("[%s %d] add subif name : %s , ifindex :%d , outvlan : %d, innervlan : %d,   module_id : %d\n",\
				__FUNCTION__, __LINE__,str_pt_name, ifindex, inner_vlan_id, 0, MODULE_ID_WEB);
			ifm_set_subif(ifindex, inner_vlan_id, 0, MODULE_ID_WEB);
		}
ERROR:		
		fprintf(cgiOut,"Content-type: text/html\n\n");
		fprintf(cgiOut, "<HTML>\n");
		fprintf(cgiOut,"<body> <center> \n");
		fprintf(cgiOut, "<script language=\"JavaScript\">\n");
		fprintf(cgiOut,"function goUrl(url)	{	location.href =url;	}\n");
		fprintf(cgiOut,"goUrl('if_basic.cgi') \n");		
		fprintf(cgiOut, "</script>\n");
		fprintf(cgiOut, "</center>\n");
		fprintf(cgiOut, "</body>\n");
		fprintf(cgiOut, "</html>\n");
	}
	else
	{
		ShowForm();
	}
	
	return 0;
}














