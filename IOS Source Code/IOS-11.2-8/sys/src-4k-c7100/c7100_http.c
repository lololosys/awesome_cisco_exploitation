/* $Id: c7100_http.c,v 3.1.2.6 1996/07/25 18:00:07 rpratt Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c7100/c7100_http.c,v $
 *------------------------------------------------------------------
 * HTTP Platform Specific Utilities.
 *
 * May 1996, Rick Pratt
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c7100_http.c,v $
 * Revision 3.1.2.6  1996/07/25  18:00:07  rpratt
 * CSCdi64036:  Remove hard coded numbers from 7200 HTML Table Construction
 * Branch: California_branch
 *
 * Adds support for the 7204
 *
 * Revision 3.1.2.5  1996/07/18  20:04:52  mbeesley
 * CSCdi63360:  Not all IO cards have fastether, make it optional
 * Branch: California_branch
 *
 * Revision 3.1.2.4  1996/07/06  05:56:35  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.1.2.3  1996/06/20  04:24:55  rpratt
 * CSCdi60873:  hostname containing [number] triggers prologue missing bug
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/06/09  22:15:03  rpratt
 * CSCdi59969:  Adding /- token to user URL, if missing, gives weird
 * context
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/19  00:24:52  rpratt
 * CSCdi57306:  HTML config has problems in new platform
 * Branch: California_branch
 *
 * Revision 3.1  1996/05/18  17:45:03  rpratt
 * HTTP Platform Specific Utilities.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "../tcp/http.h"
#include "../tcp/http_debug.h"
#include "config.h"
#include "../ip/ip_registry.h"
#include "subsys.h"

#include "../parser/parser_defs_parser.h"
#include "../hes/if_fci.h"
#include "../pas/pas_id.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/ucode.h"
#include "../hes/slot.h"
#include "../src-4k-c7100/c7100.h"
#include "../src-4k-c7100/c7100_macaddr.h"


/*
 * save some space on multiple-use strings
 */
static const char *fastethernet = IFNAME_FASTETHER;
static const char *ethernet = IFNAME_ETHER;
static const char *serial = IFNAME_SERIAL;
static const char *tokenring = IFNAME_TOKENRING;
static const char *fddi = IFNAME_FDDI;
static const char *unknown = "Unknown";
static const char *empty = "Slot";

static const char *upper_frame = "c7200-control";
static const char *left_subframe = "c7200-view";
static const char *right_subframe = "c7200-hotlist";
static const char *lower_frame = "c7200-display";

static const char *status_frame = "c7200-status-frame";
static const char *status_view = "c7200-status-view";
static const char *status_hotlist = "c7200-status-hotlist";

static const char *config_frame = "c7200-config-frame";
static const char *config_view = "c7200-config-view";
static const char *config_hotlist = "c7200-config-hotlist";

static const char *noframes = "This URL utilizes frames. A frame capable browser is required.";

/*
 * c7100_http_top_frame
 * An HTML frame for containing the status/configuration & display frames
 * as part of the /platform home page.
 */

static void c7100_http_top_frame (void)
{
    printf("<html><head><title>%s Platform</title></head>\n"
	   "<frameset rows=%d%%,*>\n", hostname, 47);
    printf("<frame src=/platform/%s name=%s>\n", status_frame, upper_frame);
    printf("<frame src=/exec/- name=%s>\n", lower_frame);
    printf("</frameset>\n<noframes>%s</noframes>\n</html>", noframes);
}


/*
 * c7100_http_status_frame
 * An HTML frame for containing the view & command frames
 * as part of the /platform home page.
 */

static void c7100_http_status_frame (void)
{
    printf("<html>\n<frameset cols=%d%%,*>\n", 60);
    printf("<frame src=/platform/%s name=%s>\n", status_view, left_subframe);
    printf("<frame src=/platform/%s name=%s>\n", status_hotlist, 
			right_subframe);
    printf("</frameset>\n<noframes>%s</noframes>\n</html>", noframes);
}


/*
 * c7100_http_config_frame
 * An HTML frame for containing the view & command frames
 * as part of the /platform home page.
 */

static void c7100_http_config_frame (void)
{
    printf("<html>\n<frameset cols=%d%%,*>\n", 60);
    printf("<frame src=/platform/%s name=%s>\n", config_view, left_subframe);
    printf("<frame src=/platform/%s name=%s>\n", config_hotlist, 
			right_subframe);
    printf("</frameset>\n<noframes>%s</noframes>\n</html>", noframes);
}


/*
 * c7100_http_view
 * An HTML table view of the c7200 with <parameter> based hotlinks where output
 * from the links goes to the lower frame of the /platform home page.
 */

static void c7100_http_view (const char *url_prefix)
{
    int row, col, port, i;
    slottype *slot;
    const char *pa_name[MAX_PA_BAYS];
    int pa_count[MAX_PA_BAYS];

    printf("\n<table border=10 width=100%%>");

    /* What are the interface names & port counts? */
    for (i = 0, slot = &slots[i] ; i < MAX_PA_BAYS; ++i, slot = &slots[i]) {
	if (slot->flags & (BOARD_NONEXIST | BOARD_INVALID)) {
             pa_count[i] = 0;
             pa_name[i] = empty;
	} else {
            switch (slot->pa_type) {
              case PA_HARDWARE_1FE:
              case PA_HARDWARE_1FE_TX_ISL:
              case PA_HARDWARE_1FE_FX_ISL:
              case PA_HARDWARE_1FE_TX_NISL:
                pa_count[i] = 1;
                pa_name[i] = fastethernet;
                break;
         
              case PA_HARDWARE_8E:
                pa_count[i] = 8;
                pa_name[i] = ethernet;
                break;
         
              case PA_HARDWARE_4E:
                pa_count[i] = 4;
                pa_name[i] = ethernet;
                break;
         
              case PA_HARDWARE_5E:
                pa_count[i] = 5;
                pa_name[i] = ethernet;
                break;
         
              case PA_HARDWARE_4T:
                pa_count[i] = 4;
                pa_name[i] = serial;
                break;
         
              case PA_HARDWARE_4R:
                pa_count[i] = 4;
                pa_name[i] = tokenring;
                break;
         
              case PA_HARDWARE_C7100_IO1FE:
                pa_count[i] = 1;
                pa_name[i] = fastethernet;
                break;

              case PA_HARDWARE_C7100_IO:
                pa_count[i] = 0;
                pa_name[i] = empty;
                break;
         
              case PA_HARDWARE_FDDI_DAS_MM:
              case PA_HARDWARE_FDDI_DAS_SS:
                pa_count[i] = 1;
                pa_name[i] = fddi;
                break;
         
              default:
                pa_count[i] = -1;
                pa_name[i] = unknown;
                break;
            }
        }
    }


    /* Spit out the "view" of the c7200 as a table of tables */
    for (row = c7100_chassis_model - 1 ; row > -2 ; row = row - 2) {
	printf("\n<TR>");
	for (col = 0 ; col < 2 ; col++) {
	    if (row == -1) {
		if (col == 0) {
		    continue;
		}
		if (pa_name[row+col] != empty) {
	            printf("\n<TD COLSPAN=2><CENTER>I/O %s %d<BR>"
			   "\n<TABLE BORDER>\n<TR>", pa_name[row+col], row+col);
		} else {
	            printf("\n<TD COLSPAN=2><CENTER>I/O Card<BR>"
			   "\n<TABLE BORDER>\n<TR>"); 
		}
	    } else { 
	        printf("\n<TD><CENTER>%s %d<BR>\n<TABLE BORDER>\n<TR>", 
			pa_name[row+col], row+col);
	    }
	    for (port = 0 ; port < pa_count[row+col] ; port++) {
		printf("\n<TD><A HREF=%s/%s/%d\\/%d/CR target=%s>%d</A>%s"
			"\n</TD>", url_prefix, pa_name[row+col], row+col, port, 
			lower_frame, port, 
			(port == pa_count[row+col]-1)?"<BR>":""); 
	    }
	    if ((pa_count[row+col] == 0) && (row+col != 0)) {
		printf("\n<TD>empty<BR>\n</TD>");
	    }
	    if ((pa_count[row+col] == -1) && (row+col != 0)) {
		printf("\n<TD>?<BR>\n</TD>");
	    }
	    printf("\n</TR>\n</TABLE>\n</TD>");
	}
	printf("\n</TR>");
    }

    printf("\n</table>");
}


/*
 * c7100_http_status_view
 * An HTML table view of the c7200 with interface status hotlinks where output
 * from the links goes to the lower frame of the /platform home page.
 */

static void c7100_http_status_view (void) {

    printf("<html><head><title>%s Status View</title></head>\n<body>",
			hostname);
    printf("<center><H3><FONT COLOR=#FFFF00>Status View</FONT></H3>");

    c7100_http_view("/exec/show/interfaces");

    printf("\n</body></html>");
}


/*
 * c7100_http_status_hotlist
 * An HTML hotlist of the popular Status Commands where output
 * from the links goes to the lower frame of the /platform home page.
 */

static void c7100_http_status_hotlist (void)
{
    printf("<html><body><H3><A NAME=Status>Status Commands</A></H3>");

    printf("\n<a href=/exec/show/processes/cpu/cr target=%s>",lower_frame);
    printf("Show processor cpu</a><br>");

    printf("\n<a href=/exec/show/memory/free/cr target=%s>",lower_frame);
    printf("Show memory free</a><br>");

    printf("\n<a href=/exec/show/buffers/cr target=%s>",lower_frame);
    printf("Show buffers</a><br>");

    printf("\n<a href=/exec/show/environment/all/cr target=%s>",lower_frame);
    printf("Show environment all</a><br>");

    printf("\n<p>");

    printf("\n<a href=/exec/show/interfaces/cr target=%s>",lower_frame);
    printf("Show interfaces</a><br>");

    printf("\n<a href=/exec/show/ip/interface/brief/cr target=%s>",lower_frame);
    printf("Show ip interface brief</a><br>");

    printf("\n<p>");

    printf("\n<a href=/exec/- target=%s>",lower_frame);
    printf("Command Form (exec mode)</a><br>");

    printf("\n<p>");

    printf("\n<a href=#Diagnostics target=%s>", right_subframe);
    printf("<em>Diagnostic Commands<em></a><br>");

    printf("\n<a href=/platform/%s target=%s>", config_frame, upper_frame);
    printf("<em>Configuration Setup<em></a><FONT COLOR=#00FF90>*</FONT><br>");

    printf("\n<a href=/ target=_top>");
    printf("<em>Home<em></a><br>");

    printf("\n<p>");

    printf("<hr><H3><A NAME=Diagnostics>Diagnostic Commands</A></H3>");

    printf("\n<a href=/exec/show/version/cr target=%s>",lower_frame);
    printf("Show version</a><br>");

    printf("\n<a href=/exec/show/c7200/cr target=%s>",lower_frame);
    printf("Show c7200</a><br>");

    printf("\n<a href=/exec/show/diag/cr target=%s>",lower_frame);
    printf("Show diag</a><br>");

    printf("\n<a href=/exec/show/controllers/cr target=%s>",lower_frame);
    printf("Show controllers</a><br>");

    printf("\n<p>");

    printf("\n<a href=/exec/- target=%s>",lower_frame);
    printf("Command Form (exec mode)</a><br>");

    printf("\n<p>");

    printf("\n<a href=#Status target=%s>", right_subframe);
    printf("<em>Status Commands<em></a><br>");

    printf("\n<a href=/platform/%s target=%s>", config_frame, upper_frame);
    printf("<em>Configuration Setup<em></a><FONT COLOR=#00FF90>*</FONT><br>");

    printf("\n<a href=/ target=_top>");
    printf("<em>Home<em></a><br>");

    /* Push the Diagnostic Commands towards the top of the window */
    printf("\n<p><br><p><br><p><br><p>");

    printf("\n</body></html>");
}


/*
 * c7100_http_config_view
 * An HTML table view of the c7200 with interface config hot-links where output
 * from the links goes to the lower frame of the /platform home page.
 */

static void c7100_http_config_view (void)
{
    printf("<html><head><title>%s Configuration Setup</title></head>\n<body>",
			hostname);
    printf("<CENTER><H3><FONT COLOR=#00FF90>Configuration Setup</FONT></H3>");

    c7100_http_view("/configure/-/interface");

    printf("\n</body></html>");
}


/*
 * c7100_http_config_hotlist
 * An HTML hotlist of the popular Configuration Commands where output
 * from the links goes to the lower frame of the /platform home page.
 */

static void c7100_http_config_hotlist (void)
{
    printf("<html><body><H3>Configuration Commands</H3>");

    printf("\n<a href=/exec/show/running-config/cr target=%s>",lower_frame);
    printf("Show running-config</a><br>");

    printf("\n<a href=/exec/show/startup-config/cr target=%s>",lower_frame);
    printf("Show startup-config</a><br>");

    printf("\n<a href=/exec/show/flash/cr target=%s>",lower_frame);
    printf("Show flash</a><br>");

    printf("\n<a href=/exec/show/boot/cr target=%s>",lower_frame);
    printf("Show boot</a><br>");

    printf("\n<a href=/exec/show/ip/access-lists/cr target=%s>",lower_frame);
    printf("Show ip access-lists</a><br>");

    printf("\n<p>");

    printf("\n<a href=/exec/write/memory/cr target=%s>",lower_frame);
    printf("Write memory</a><br>");

    printf("\n<p>");

    printf("\n<a href=/exec/-/config/http/cr target=%s>",lower_frame);
    printf("Command Form (configure mode)</a><br>");

    printf("\n<p>");

    printf("\n<a href=/platform/%s target=%s>", status_frame, upper_frame);
    printf("<em>Status View<em></a><FONT COLOR=#FFFF00>*</FONT><br>");

    printf("\n<a href=/ target=_top>");
    printf("<em>Home<em></a><br>");

    printf("\n</body></html>");
}


/*
 * c7100_http_get
 * Send the proper HTML incantations to display the desired page
 */

static boolean c7100_http_get (httpstate *state)
{
    char *token;

    token = http_get_uri_token(state);
    if (http_transaction_debug) {
        buginf("\nHTTP: received platform GET '%s'", token);
    }

    http_send_prologue(state, HTTP_STATUS_OK, TRUE, TRUE);

    if (!(*token)) {
	c7100_http_top_frame();
    } else if (!(strcasecmp(token, status_frame))) {
	c7100_http_status_frame();
    } else if (!(strcasecmp(token, status_view))) {
	c7100_http_status_view();
    } else if (!(strcasecmp(token, status_hotlist))) {
	c7100_http_status_hotlist();
    } else if (!(strcasecmp(token, config_frame))) {
	c7100_http_config_frame();
    } else if (!(strcasecmp(token, config_view))) {
	c7100_http_config_view();
    } else if (!(strcasecmp(token, config_hotlist))) {
	c7100_http_config_hotlist();
    } else {
	http_send_not_found(state, NULL);
    }

    http_page_complete(state);
    return(TRUE);
}

static void c7100_http_init (subsystype *subsys)
{
    reg_add_platform_http_page_send(c7100_http_get, "c7100_http_get");
}

#define	C7100_HTTP_MAJVERSION		1
#define	C7100_HTTP_MINVERSION		0
#define	C7100_HTTP_EDITVERSION		1

SUBSYS_HEADER(platform_http, C7100_HTTP_MAJVERSION, C7100_HTTP_MINVERSION,
	      C7100_HTTP_EDITVERSION, c7100_http_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost, "req: iphost, http");
