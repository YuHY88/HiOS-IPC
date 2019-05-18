
#include <stdio.h>
#include "cgic.h"
#include <string.h>
#include <stdlib.h>

int cgiMain(int argc, char *argv[]) {
	//	fprintf(cgiOut, "Connect message queue successful, process(1628) id(196614) \n");

		fprintf(cgiOut, "<HTML><HEAD>\n");
		fprintf(cgiOut, "<TITLE>Web Manager</TITLE>\n");
		fprintf(cgiOut, "<META http-equiv=PRAGMA content=NO-CACHE>\n");
		fprintf(cgiOut, "<META http-equiv=Expires content=-1>\n");
		fprintf(cgiOut, "<META http-equiv=Cache-Control content=NO-CACHE>\n");
		fprintf(cgiOut, "<META http-equiv=Content-Type content=\"text/html; charset=gb2312\"> \n");
		fprintf(cgiOut, "<script language=\"JavaScript\"> \n");
		fprintf(cgiOut, "function Click(){ window.event.returnValue=false;} document.oncontextmenu=Click;	\n");
		fprintf(cgiOut, " if(window != window.parent)  { window.parent.location.href = \"/web/index.cgi\"; }\n");
		fprintf(cgiOut, " </script></HEAD>\n");


		fprintf(cgiOut, "<FRAMESET border=\"0\" rows=\"70,*\" cols=\"*\" frameBorder=\"NO\" frameSpacing=\"0\" > \n");
			
		fprintf(cgiOut, " <FRAMESET border=\"0\" rows=\"70\" cols=\"200,*\" frameBorder=\"NO\" frameSpacing=\"0\" > \n");
		fprintf(cgiOut, "  <FRAME name=\"topLeftFrame\" src=\"/web/logo.html\" scrolling=\"no\" noResize></FRAME>\n");
		fprintf(cgiOut, "  <FRAME name=\"topRightFrame\"  src=\"/cgi-bin/banner.cgi\" scrolling=\"no\"  noResize></FRAME>\n");
		fprintf(cgiOut, " </FRAMESET>\n");
		  
		fprintf(cgiOut, " <FRAMESET border=\"0\" rows=\"*\" cols=\"200,*\" frameSpacing=\"0\"> \n");
		fprintf(cgiOut, "  <FRAME name=\"bottomLeftFrame\"  src=\"/cgi-bin/leftframe.cgi\"  noResize></FRAME>\n");
		fprintf(cgiOut, " <FRAME name=\"mainFrame\" src=\"/cgi-bin/if_basic.cgi\" frameborder=\"NO\"></FRAME>\n");
		fprintf(cgiOut, "</FRAMESET>\n");


		fprintf(cgiOut, "</FRAMESET>\n");
		fprintf(cgiOut, "<noframes><body>Sorry, Your browser does not support frames ! </body></noframes>\n");
		fprintf(cgiOut, "</html>\n");
	return 0;
	}




