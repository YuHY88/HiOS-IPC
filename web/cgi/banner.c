
#include <stdio.h>
#include "cgic.h"
#include <string.h>
#include <stdlib.h>

//#define	LOGO_PATH				"/home/eric/www/Admin/logo"
#define	LOGO_PATH				"/home/boa/Admin/logo"
#define MAX_LOGO_LEN			0x40
//#define MAX_CNAME_LEN			0x10

int  GetSysVars();
void ShowPage();
char g_LogoStr[MAX_LOGO_LEN];

int cgiMain(int argc, char *argv[]) {

	cgiHeaderContentType("text/html");
	/* Top of the page */
	
	fprintf(cgiOut, "<HTML><HEAD>\n");
	fprintf(cgiOut, "<TITLE>banner</TITLE>\n");
	fprintf(cgiOut, "<META http-equiv=PRAGMA content=NO-CACHE>\n");
	fprintf(cgiOut, "<META http-equiv=Expires content=-1>\n");
	fprintf(cgiOut, "<META http-equiv=Cache-Control content=NO-CACHE>\n");
	fprintf(cgiOut, "<META http-equiv=Content-Type content=\"text/html; charset=gb2312\"> \n");
	fprintf(cgiOut, "<style type=\"text/css\">\n");
	fprintf(cgiOut, "a.tp:visited{COLOR: #ffffff; TEXT-DECORATION: none; underline: none; }\
					 a.tp:hover {COLOR: #ff4000;  TEXT-DECORATION: none; cursor: hand;}\
					 a.tp:link {COLOR: #ffffff; TEXT-DECORATION: none; underline: none;}\
					.font {	font-family: \"Courier New\", \"Courier\", \"mono\";font-size: 12px;color: #FFFFFF;}\
					 td {font-family: \"Times New Roman\";font-size: 20px; color: #FFFAFA;}\
					 form {font-family: \"Times New Roman\";font-size: 12px;}\
				     body {font-family: \"Arial Black\", \"黑体\";font-size: 16px;}\
					.unnamed1 {  letter-spacing: 50px; word-spacing: 100px;}\
					.unnamed2 {  word-spacing: 100px;}\
					</style>\n");
	fprintf(cgiOut, "<SCRIPT language=\"JavaScript\">\
			function Click(){ window.event.returnValue=false;}\
			document.oncontextmenu=Click;\
			</SCRIPT>\n");
			
	fprintf(cgiOut, "</HEAD><BODY bgcolor=\"#0052A4\" leftmargin=\"0\" topmargin=\"0\">\n");
	
	GetSysVars();

	ShowPage();
	fprintf(cgiOut, "</BODY></HTML>\n");
	return 0;
}

int GetSysVars()
{	
	FILE 	*fp;
	char 	LogoTemps[MAX_LOGO_LEN];
	//char	CompanyName[MAX_CNAME_LEN];
	char 	*Logofile = LOGO_PATH; 

	strcpy(g_LogoStr, "---");

	if((fp = fopen(Logofile, "r")) != NULL)
	{
		fseek(fp, 0, SEEK_SET);
		if( fgets(LogoTemps, MAX_LOGO_LEN, fp) != NULL )
		{
			if ( strcmp(LogoTemps, "Huahuan\n") == 0)
			{
				strcpy(g_LogoStr, "Beijing Huahuan Electronics Co., Ltd H20RN-181");
			}
			else
			{
			
				strcpy(g_LogoStr, LogoTemps);
		
			}
		}
		fclose(fp);
	}

	return 0;
}

void ShowPage()
{	
	fprintf(cgiOut, "<table width=100%% height=\"70\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
	fprintf(cgiOut, "<tr> <td width=100%% height=\"50\"> <table width=100%% height=100%% border=\"0\" cellspacing=\"0\" cellspacing=\"0\"> ");
//	fprintf(cgiOut,	"<tr align=\"middle\"><td align=middle width=\"1000\" height=\"50\"> Welcome to %s Web Manager !</td> </tr>\n", g_LogoStr);
	fprintf(cgiOut,	"<tr align=\"middle\"><td align=middle width=\"1080\" height=\"50\"> <img  align=\"middle\" src=\"/web/banner.png\"  height=\"48\" width=\"100%%\"></td> </tr>\n");


	fprintf(cgiOut, "</table></td></tr>");

	fprintf(cgiOut, "<tr bgcolor=\"#FFFFFF\"> <td height=5 colspan=\"2\"> </td></tr><tr bgcolor=\"#0052A4\"> \
    			<td height=\"10\" colspan=\"2\" >&nbsp;</td></tr></table>");

}


