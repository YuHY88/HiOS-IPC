

#include <stdio.h>
#include "cgic.h"
#include <string.h>
#include <stdlib.h>
//#include "Global.h"

int cgiMain(int argc, char *argv[]) {

		
		cgiHeaderContentType("text/html");
		fprintf(cgiOut, "<HTML><HEAD>\n");
		fprintf(cgiOut, "<TITLE>leftFrame</TITLE>\n");
		fprintf(cgiOut, "<META http-equiv=PRAGMA content=NO-CACHE>\n");
		fprintf(cgiOut, "<META http-equiv=Expires content=-1>\n");
		fprintf(cgiOut, "<META http-equiv=Cache-Control content=NO-CACHE>\n");
		fprintf(cgiOut, "<META http-equiv=Content-Type content='text/html; charset=gb2312'> \n");
		fprintf(cgiOut, "<script language=\"JavaScript\"> \n");
		#ifdef SA_63
		fprintf(cgiOut, "var visibleMenuList = new Array(\
		\"IpInfo\",\
		\"VerInfo\",\
		\"SetTime\",\
		\"Reboot\",\
		0,0 ); \n");
		#else
		fprintf(cgiOut, "var visibleMenuList = new Array(\
		\"if_basic.cgi\",\
		\"if_l3.cgi\",\
		0,0 ); \n");
		#endif
		fprintf(cgiOut, "function Click(){ window.event.returnValue=false;}  document.oncontextmenu=Click;\n");
		fprintf(cgiOut, "</script>\n");

		fprintf(cgiOut, "<script language=\"JavaScript\" src=\"/web/str_menu.js\" type=\"text/JavaScript\"></script>\n");
		fprintf(cgiOut, "<script language=\"JavaScript\" src=\"/web/menu.js\" type=\"text/JavaScript\"></script>\n");
		fprintf(cgiOut, "<style type=\"text/css\">\n");
		fprintf(cgiOut, "BODY {margin-top: 25px; margin-left: 15px; FONT-FAMILY: \"Times New Roman\"; WHITE-SPACE: nowrap; BACKGROUND-COLOR: #0052A4;}\n");
		fprintf(cgiOut, "menu {margin-left:0px; PADDING-LEFT: 0px;   FONT-SIZE: 15px;  TEXT-DECORATION: none; color: white;}\n");
		fprintf(cgiOut, "ol {margin-top:8px; margin-left:2px; PADDING-LEFT: 18px;   FONT-SIZE: 16px;  TEXT-DECORATION: none; height:  18px; \
							line-height:  18px; list-style: inside none; }\n");
		fprintf(cgiOut, "ol.info {COLOR: white; PADDING-LEFT: 2px;}\n");
		fprintf(cgiOut, "a.L1  {COLOR: white;}\n");
		fprintf(cgiOut, "a.L2  {COLOR: #fff685;}\n");
		fprintf(cgiOut, "A:visited {TEXT-DECORATION: none; underline: none;}\n");
		fprintf(cgiOut, "A:hover {CURSOR: hand; font-size: 17px; COLOR: #fff685; TEXT-DECORATION: none; font-weight:bold;}\n");
		fprintf(cgiOut, "A:link {TEXT-DECORATION: none; underline: none;}\n");
		fprintf(cgiOut, "ol.dot1	{BACKGROUND-IMAGE: url(\"/web/pw.gif\");	background-repeat:no-repeat; }\n");
		fprintf(cgiOut, "ol.plus 	{BACKGROUND-IMAGE: url(\"/web/plus.gif\");   background-repeat:no-repeat;  }\n");
		fprintf(cgiOut, "ol.minus 	{BACKGROUND-IMAGE: url(\"/web/minus.gif\"); background-repeat:no-repeat; }\n");
		fprintf(cgiOut, "ol.dot2 	{BACKGROUND-IMAGE: url(\"/web/pw.gif\");	background-repeat:no-repeat; }\n");
		fprintf(cgiOut, "ol.otherInfo {PADDING-LEFT: 1px; PADDING-RIGHT: 20px;}\n");
		fprintf(cgiOut, "</style>\n");
		fprintf(cgiOut, "</HEAD>\n");
		fprintf(cgiOut, "<BODY bgColor=#330099 leftMargin=0 topMargin=0 marginheight=0 marginwidth=0>");
		fprintf(cgiOut, "<menu cellSpacing=2 cellPadding=1 border=0 width=180 >");
		fprintf(cgiOut, "<script language=\"JavaScript\">\n");
		fprintf(cgiOut, "menuInit(visibleMenuList);\n");
		fprintf(cgiOut, "menuDisplay();\n");
		fprintf(cgiOut, "</script>\n");
		fprintf(cgiOut, "</menu>\n");
		fprintf(cgiOut, "</BODY></HTML>\n");
		return 0;
}

