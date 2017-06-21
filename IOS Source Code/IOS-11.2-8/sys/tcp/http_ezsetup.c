/* $Id: http_ezsetup.c,v 3.2.4.14 1996/07/06 05:56:59 thille Exp $
 * $Source: /release/112/cvs/Xsys/tcp/http_ezsetup.c,v $
 *------------------------------------------------------------------
 * HTTP EZ Setup
 *
 * September 1995, Robert Widmer
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: http_ezsetup.c,v $
 * Revision 3.2.4.14  1996/07/06  05:56:59  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.2.4.13  1996/07/01  21:03:05  snyder
 * CSCdi61803:  html code uses char * in places it should use char []
 * bRanch: California_branch
 *              Only 88 bytes for this one.
 *
 * Revision 3.2.4.12  1996/06/19  18:59:06  widmer
 * CSCdi56931:  Length of the text fields are high
 * Branch: California_branch
 * Missed file
 *
 * Revision 3.2.4.11  1996/06/18  00:29:05  nkwok
 * CSCdi57945:  EZSetup name need changing
 * Branch: California_branch
 * Add sub-system setup registry
 *
 * Revision 3.2.4.10  1996/05/30  18:38:49  widmer
 * CSCdi58192:  Frame-relay IP netmask error not reported properly
 * Branch: California_branch
 * Call good_address() to check on address/mask
 *
 * Revision 3.2.4.9  1996/05/23  19:53:47  nkwok
 * CSCdi57859:  HTTP data verification
 * Branch: California_branch
 *
 * Revision 3.2.4.8  1996/05/21  10:06:17  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2.4.7  1996/05/07  16:39:24  nkwok
 * CSCdi56514:  http - Password verification
 * Branch: California_branch
 *
 * Revision 3.2.4.6  1996/05/06  20:06:09  widmer
 * CSCdi55520:  Add ip classless and ip subnet-zero as defaults to
 * Clickstart
 * Branch: California_branch
 *
 * Revision 3.2.4.5  1996/05/06  18:49:21  widmer
 * CSCdi56800:  occurred misspelled in http ezsetup code
 * Branch: California_branch
 *
 * Revision 3.2.4.4  1996/04/30  21:55:24  nkwok
 * CSCdi55776:  http setup leak memory
 * Branch: California_branch
 *
 * Revision 3.2.4.3  1996/04/18  01:58:45  fred
 * CSCdi55001:  correct tree
 * Branch: California_branch
 *
 * Revision 3.2.4.2  1996/04/15  21:23:25  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.2.4.1  1996/03/18  22:13:26  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.6.1  1996/03/16  07:40:42  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2  1996/03/09  22:57:41  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.1  1996/02/11  04:25:58  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add placeholder files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "http.h"
#include "../ui/debug.h"
#include "http_debug.h"
#include "tcp.h"
#include "../ui/setup.h"
#include "config.h"
#include "../parser/parser_actions.h"
#include "ttysrv.h"
#include "name.h"
#include "sys_registry.h"
#include "../ip/ip_registry.h"
#include "../ip/ipautoaddr.h"
#include "subsys.h"
#include "../os/nv.h"

static const char ezsetup_hostname[] = "hostname";
static const char ezsetup_verify_password[] = "verify_password";
static const char ezsetup_password[] = "password";
static const char ezsetup_eth0addr[] = "eth0addr";
static const char ezsetup_eth0mask[] = "eth0mask";
static const char ezsetup_dnsaddr[] = "dnsaddr";
static const char ezsetup_stdebug[] = "stdebug";

static void http_ezsetup_send_page(httpstate *);
static void http_ezsetup_prologue(httpstate *);


void http_ezsetup_error (httpstate *state, boolean *error,
			 const char *name, const char *msg)
{
    int i;

    if (!*error) {
	*error = TRUE;
	http_ezsetup_prologue(state);
	printf("\n<H1><I>The following configuration errors occurred:</I></H1>");
    }
    printf("\n<A HREF=#%s_tag>", name);
    http_form_print_error(state, msg);
    printf("</A>");

    if (state->form) {
	for (i=0; i<HTTP_MAX_FORM_VARIABLES; i++) {
	    if (!state->form[i]) {
		return;
	    }
	    if (strcasecmp(state->form[i]->name, name) == 0) {
		state->form[i]->error = msg;
		return;
	    }
	}
    }
}

static setup_struct *http_setup_init (httpstate *state, int request)
{
    setup_struct *sp;
    uint index;

    sp = malloc(sizeof(setup_struct));
    if (sp) {
	if (request == HTTP_REQUEST_POST) {
	    sp->cmds = malloc(config_bytes);
	    if (!sp->cmds) {
		free(sp);
		return(NULL);
	    }
	} else {
	    sp->cmds = NULL;
	}
	sp->idb = NULL;
	sp->defnetwork = 1;
	sp->run_ip = TRUE;
	sp->run_ip_snmp = FALSE;
	sp->ip_history_head = NULL;
	sp->http_state = state;
	sp->http_request = request;
	for (index = 0; index < MAXINTERFACES; index++) {
	    sp->addrlist[index] = (ipaddrtype) 0L;
	    sp->masklist[index] = (ipaddrtype) 0L;
	}
	sp->args[LINK_MOP_CONSOLE] = FALSE;
    }
    return(sp);
}

/*
 * http_ezsetup_prologue
 */
static void http_ezsetup_prologue (httpstate *state)
{
    http_send_prologue(state, HTTP_STATUS_OK, TRUE, TRUE);
    printf("<HTML><HEAD><TITLE>%s Setup</TITLE></HEAD><BODY>", hostname);
}

/*
 * http_ezsetup_pick_ipaddress
 */
static void http_ezsetup_pick_ipaddress (httpstate *state)
{
    idbtype *idb;

    FOR_ALL_SWIDBS(idb) {
	if (idb->hwptr->status & IDB_ETHER) {
	    break;
	}
    }

    if (idb) {
	http_set_form_variable(state, ezsetup_eth0addr, "%i", idb->ip_address);
	http_set_form_variable(state, ezsetup_eth0mask, "%i", idb->ip_nets_mask);
    }
}

/*
 * http_ezsetup_get
 * Send the proper HTML incantations to display ez setup page.
 */

static boolean http_ezsetup_get (httpstate *state)
{
    http_form **old_form;
    uint old_form_length;

    old_form = state->form;
    old_form_length = state->form_length;

    state->form = malloc(sizeof(http_form)*HTTP_MAX_FORM_VARIABLES);
    if (!state->form) {
	state->form = old_form;
	state->form_length = old_form_length;
	return(TRUE);
    }
    state->form_length = 0;

    http_form_add(state, ezsetup_hostname, "%s", hostname);
    http_form_add(state, ezsetup_eth0addr, "");
    http_form_add(state, ezsetup_eth0mask, "");
    http_ezsetup_pick_ipaddress(state);

    http_form_add(state, ezsetup_stdebug, "y");

    if (domain_server[0]) {
	http_form_add(state, ezsetup_dnsaddr, "%i", domain_server[0]);
    }

    reg_invoke_http_ezsetup_init(state);

    http_ezsetup_prologue(state);
    http_ezsetup_send_page(state);

    http_free_form(state->form);

    state->form = old_form;
    state->form_length = old_form_length;
    return(TRUE);
}

static void http_ezsetup_send_page (httpstate *state)
{
    idbtype *idb;

    printf("\n%s<P>\n<H2>\"%s\" Setup</H2>", logo_url, hostname);

    /*
     * Point form action back to us so we get a "post" with the
     * config info.
     */
    printf("\n<FORM METHOD=POST ACTION=/ezsetup/set>"


	   "\nTo configure your router fill in the fields "
	   "below.  Most of the information required should have been "
	   "supplied to you by your network manager or service provider."

	   "\n<H3>Router name and Password</H3>"

	   "\nName your router and give it a password. "
	   "The router name must be the name that you were told to "
	   "use.  The password is your choice."

	   "\n<DL>");

    http_form_print_variable(state, HTTP_INPUT_STRING, 244,
			     "Router name", ezsetup_hostname);
    http_form_print_variable(state, HTTP_INPUT_PASSWORD, PASSWORDLEN,
			     "Router password", ezsetup_password);
    http_form_print_variable(state, HTTP_INPUT_PASSWORD, PASSWORDLEN,
			     "Verify password", ezsetup_verify_password);
    printf("\n</DL>");

    reg_invoke_http_ezsetup_send(state);
	
   /*
    * Find first Ethernet interface
    */
    FOR_ALL_SWIDBS(idb) {
	if (idb->hwptr->status & IDB_ETHER) {
	    break;
	}
    }

    if (idb) {
	printf("\n<H3>IP Addresses</H3>");

	if (idb->resolvemethod == RES_HEURISTIC) {
	    printf("\n<P>The router used heuristics to pick <B>%i</B> "
		   "for an address. You may want to change it.",
		   idb->ip_address);
	}
	printf("\n<DL>");
	http_form_print_variable(state, HTTP_INPUT_STRING, 15,
				 "Router IP address", ezsetup_eth0addr);
	http_form_print_variable(state, HTTP_INPUT_STRING, 15,
				 "Router netmask", ezsetup_eth0mask);
	http_form_print_variable(state, HTTP_INPUT_STRING, 15,
				 "DNS server IP address", ezsetup_dnsaddr);
	printf("\n</DL>");
    }

    printf("\n<H3>Diagnostic Information</H3><P>"
	   "\nYou may want to enable some standard debugging "
	   "information. This will have little or no impact on "
	   "performance but will help diagnose ISDN and Dial-On-Demand "
	   "problems.  Debugging information is sent to the console "
	   "and is stored in an internal buffer."
	   "\n<P>Please choose one of the options below."
	   "\n<P><DL><DL>"
	   "\n<DT><INPUT TYPE=radio NAME=%s VALUE=y %s> Enable standard debug"
	   "\n<DT><INPUT TYPE=radio NAME=%s VALUE=n %s> Disable debug"
	   "\n</DL></DL><P>"

	   "\n<INPUT TYPE=submit VALUE=Submit>"
	   "\n<P>NOTE: Submission of this form will reconfigure and "
	   "then reload the device."
	   "</FORM>"
	   "\n</BODY></HTML>",
	   ezsetup_stdebug,
	   ((strcmp(http_get_form_variable(state, ezsetup_stdebug), "y") == 0)
	    ? "CHECKED" : ""),
	   ezsetup_stdebug,
	   ((strcmp(http_get_form_variable(state, ezsetup_stdebug), "n") == 0)
	    ? "CHECKED": ""));

    http_page_complete(state);
}

/*
 * http_send_congratulations_page
 * Page after a successful configuration.
 */

static void http_send_congratulations_page (httpstate *state,
					    ipaddrtype newipaddr)
{
    http_send_prologue(state, HTTP_STATUS_OK, TRUE, TRUE);

    printf("<HTML><HEAD><TITLE>%s Setup Complete</TITLE>", hostname);

    if (state->flags == HTTP_REFRESH_HOME_PAGE)
        printf("\n<META HTTP-EQUIV=\"Refresh\" CONTENT=\"60;"
	       " URL=http://%i\">", newipaddr);

    printf("</HEAD>"
	   "<BODY><H1>Congratulations!</H1>"
	   "You have successfully configured this router! "
	   "At this time your router is reloading.  The new "
	   "home page will reload automatically.");
    printf("<P>You probably should visit your router's home page "
	       "and make a bookmark to it. "
	       "When you do so you will be prompted for a username and "
	       "password.  You can leave the username blank.  Use the "
	       "router password you entered on the previous page. "
	       "<P><A HREF=http://%i/>Visit the router's home page.</A>"
	       "<P>If you would like to further enhance its "
	       "configuration, you can do that via "
	       "<A HREF=telnet://%i/>telnet.</A>",
	       newipaddr, newipaddr);
    printf("<P>If you are on the Internet, and "
	   "would like to register in Customer "
	   "Information On-line, <A HREF=http://www.cisco.com/>"
	   "click here</A>.</BODY></HTML>\r\n\r\n");
    http_page_complete(state);
    flush();
}

/*
 *
 */

static boolean http_ezsetup_post (httpstate *state)
{
    char *cp, *cpVerifyPassword;
    char *cmdstring;
    ipaddrtype dns;
    setup_struct *sp;
    ipaddrtype newipaddr;
    ipaddrtype newipmask;
    boolean error = FALSE;

    dns = 0;
    sp = http_setup_init(state, HTTP_REQUEST_POST);
    if (!sp) {
	return(TRUE);
    }
    cmdstring = sp->cmds;

    setup_printf(sp,
		 "\nservice timestamps debug"
		 "\nservice timestamps log"
		 "\nservice password-encryption"
		 "\n!"
		 );

    cp = http_get_form_variable(state, ezsetup_hostname);
    switch(http_token_count(cp)) {
      case 0:
	break;
      case 1:
	setup_printf(sp, "\nhostname %s", cp);
	break;
      default:
	http_ezsetup_error(state, &error,
			   ezsetup_hostname, "Host Name is invalid");
	break;
    }

    /*
     * Make sure Both Password and Verify Password are the same
     */

    cp = http_get_form_variable(state, ezsetup_password);
    cpVerifyPassword = http_get_form_variable(state, ezsetup_verify_password);

    if ((cp && *cp) || (cpVerifyPassword && *cpVerifyPassword)) {
      if ((NULL == cp) ||
	  ('\0' == *cp) ||
	  (NULL == cpVerifyPassword) ||
	  ('\0' == *cpVerifyPassword) ||
	  (0 != strcmp(cp, cpVerifyPassword))) {
	http_ezsetup_error(state, &error,
			   ezsetup_password, "Mismatch password");
      } else {
	setup_printf(sp,
		     "\n!"
		     "\nenable secret %s"
		     "\nline vty 0 4"
		     "\npassword %s"
		     "\n!",
		     cp, cp);
      }
    }

    setup_printf(sp, "\ninterface ethernet 0");
    cp = http_get_form_variable(state, ezsetup_eth0addr);
    if (cp && cp[0] &&
	parse_ip_address(cp, &newipaddr) &&
	!martian(newipaddr, TRUE)) {
	cp = http_get_form_variable(state, ezsetup_eth0mask);
	if (cp && cp[0] && parse_ip_address(cp, &newipmask)) {
	    if (good_address(newipaddr, newipmask, TRUE)) {
		setup_printf(sp, "\n ip address %i %i",
			     newipaddr, newipmask);
	    } else {
		http_ezsetup_error(state, &error, ezsetup_eth0mask,
				   "Bad mask for ip address");
	    }
	} else {
	    http_ezsetup_error(state, &error, ezsetup_eth0mask,
			       "Router netmask is invalid");
	}
	setup_printf(sp, "\n no shutdown");
    } else {
	http_ezsetup_error(state, &error, ezsetup_eth0addr,
			   "IP Address is invalid");
	http_ezsetup_pick_ipaddress(state);
    }

    cp = http_get_form_variable(state, ezsetup_dnsaddr);
    if (cp && cp[0]) {
	if (parse_ip_address(cp, &dns)) {
	    setup_printf(sp, "\n ip helper-address %i", dns);
	} else {
	    dns = 0;
	    http_ezsetup_error(state, &error, ezsetup_dnsaddr,
			       "DNS server IP address is invalid");
	}
    }

    if (dns) {
	setup_printf(sp, "\nip name-server %i", dns);
    }

    reg_invoke_http_ezsetup_post(state, sp, &error);

    setup_printf(sp,
		 "\nlogging buffered"
		 "\nsnmp-server community public RO"
		 "\nip http server"
		 "\nip classless"
		 "\nip subnet-zero"
		 "\n!"
		 "\nend");

    if (error) {
	printf("\n<H1><I>"
	       "Please correct any errors and resubmit the form"
	       "</I></H1><HR>");
	http_ezsetup_send_page(state);
    } else {
	/*
	 * Have to send congratulations before changing ip addresses out
	 * from under the tcp connection when the config is parsed.
	 */
        state->flags = HTTP_REFRESH_HOME_PAGE;
	http_send_congratulations_page(state, newipaddr);
	if (http_ezsetup_debug) {
	    ttyprintf(CONTTY, "%s", cmdstring);
	} else {
	    (void) nv_buffer_to_nv(cmdstring, strlen(cmdstring), FALSE);
	    cp = http_get_form_variable(state, ezsetup_stdebug);
	    if (cp && cp[0] && (strcasecmp(cp, "y") == 0)) {
		/*
		 * Turn on standard debugging information
		 */
		reg_invoke_debug_standard(TRUE);
	    } else {
		reg_invoke_debug_standard(FALSE);
	    }
            reload_system(RELOAD_NORMAL);
	}
    }
    free(cmdstring);
    free(sp);
    return(TRUE);
}

static void http_ezsetup_init (subsystype *subsys)
{
    reg_add_http_ezsetup_page_send(http_ezsetup_get, "http_ezsetup_get");
    reg_add_http_ezsetup_page_post(http_ezsetup_post, "http_ezsetup_post");

    /*
     * Prepare for address discovery
     */
    reg_add_autoaddr_stop(ip_dns_killproc, "ip_dns_killproc");
    reg_add_ip_autoaddr_load(ip_webinstall, "ip_webinstall");
}

#define	HTTP_EZSETUP_MAJVERSION		1
#define	HTTP_EZSETUP_MINVERSION		0
#define	HTTP_EZSETUP_EDITVERSION	1

SUBSYS_HEADER(http_ezsetup, HTTP_EZSETUP_MAJVERSION, HTTP_EZSETUP_MINVERSION,
	      HTTP_EZSETUP_EDITVERSION, http_ezsetup_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost, "req: iphost, http");
