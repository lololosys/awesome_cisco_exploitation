/* $Id: frame_relay_http.c,v 3.2.4.7 1996/07/06 05:58:05 thille Exp $
 * $Source: /release/112/cvs/Xsys/wan/frame_relay_http.c,v $
 *------------------------------------------------------------------
 * Frame-Relay HTTP configuration
 *
 * January 1996, Robert Widmer
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: frame_relay_http.c,v $
 * Revision 3.2.4.7  1996/07/06  05:58:05  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.2.4.6  1996/07/03  18:03:01  snyder
 * CSCdi62021:  char *s -> char []s in http
 * Branch: California_branch
 *              100 bytes
 *
 * Revision 3.2.4.5  1996/06/19  18:30:50  widmer
 * CSCdi56931:  Length of the text fields are high
 * Branch: California_branch
 * Add maxlength to ezsetup form variables
 *
 * Revision 3.2.4.4  1996/06/19  13:35:52  widmer
 * CSCdi59768:  DLCI is not validated in EZSetup
 * Branch: California_branch
 * Enforce subsystem ordering
 *
 * Revision 3.2.4.3  1996/06/18  23:21:56  widmer
 * CSCdi59768:  DLCI is not validated in EZSetup
 * Branch: California_branch
 *
 * Revision 3.2.4.2  1996/06/18  00:29:39  nkwok
 * CSCdi57945:  EZSetup name need changing
 * Branch: California_branch
 * Add sub-system setup registry
 *
 * Revision 3.2.4.1  1996/05/30  18:38:56  widmer
 * CSCdi58192:  Frame-relay IP netmask error not reported properly
 * Branch: California_branch
 * Call good_address() to check on address/mask
 *
 * Revision 3.2  1996/03/09  22:58:26  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.1  1996/02/11  04:26:02  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add placeholder files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "subsys.h"
#include "../tcp/http.h"
#include "../tcp/tcp.h"
#include "../ui/setup.h"
#include "../ip/ip_registry.h"
#include "frame_relay.h"
#include <stdlib.h>

static const char ezsetup_fr_addr[] = "fr_addr";
static const char ezsetup_fr_mask[] = "fr_mask";
static const char ezsetup_fr_dlci[] = "fr_dlci";
static idbtype *fr_ezsetup_idb;

static void  fr_http_ezsetup_form_init (httpstate *state)
{
    int i;
    frmaptype *p;
    fr_idb_struct_type *fr_idb = fr_ezsetup_idb->hwptr->frame_relay_stuff;

    if (fr_ezsetup_idb && fr_ezsetup_idb->ip_address) {
	http_form_add(state, ezsetup_fr_addr, "%i",
		      fr_ezsetup_idb->ip_address);
	http_form_add(state, ezsetup_fr_mask, "%i",
		      fr_ezsetup_idb->ip_nets_mask);
	for (i=0; i<RELAYHASH; i++) {
	    for (p=relaytable[i]; p; p = p->next) {
		if ((p->map_type == FR_MAP_STATIC) &&
		    (fr_ezsetup_idb == p->idb) &&
		    (p->link == LINK_IP) &&
		    (fr_ezsetup_idb->ip_address == p->proto_addr.ip_addr)) {
		    http_form_add(state, ezsetup_fr_dlci, "%d",
				  DLCI_TO_NUM(p->dlci));
		}
	    }
	}
    }
}

static void fr_http_ezsetup_send (httpstate *state)
{
    printf("\n<H3>Frame-Relay Setup</H3><DL>");
    http_form_print_variable(state, HTTP_INPUT_STRING, 15,
			     "Frame-Relay IP address",
			     ezsetup_fr_addr);
    http_form_print_variable(state, HTTP_INPUT_STRING, 15,
			     "Frame-Relay netmask",
			     ezsetup_fr_mask);
    http_form_print_variable(state, HTTP_INPUT_STRING, 4,
			     "Frame-Relay DLCI",
			     ezsetup_fr_dlci);
    printf("\n</DL>");
}

static void fr_http_ezsetup_post (httpstate *state,
				  setup_struct *sp,
				  boolean *error)
{
    ipaddrtype newipaddr;
    ipaddrtype newipmask;
    char *cp;
    int dlci;

    cp = http_get_form_variable(state, ezsetup_fr_addr);
    if (!cp || !cp[0] ||
	!parse_ip_address(cp, &newipaddr) ||
	martian(newipaddr, TRUE)) {
	http_ezsetup_error(state, error, ezsetup_fr_addr,
			   "Frame-Relay IP Address is invalid");
    }

    cp = http_get_form_variable(state, ezsetup_fr_mask);
    if (!cp || !cp[0] || !parse_ip_address(cp, &newipmask)) {
	http_ezsetup_error(state, error, ezsetup_fr_mask,
			   "Frame-Relay netmask is invalid");
    }

    if (!good_address(newipaddr, newipmask, TRUE)) {
	http_ezsetup_error(state, error, ezsetup_fr_mask,
			   "Bad mask for ip address");
    }

    if (!(*error)) {
	setup_printf(sp,
		     "\ninterface serial 0"
		     "\n encapsulation frame-relay"
		     "\n ip address %i %i"
		     "\n no shutdown"
		     , newipaddr, newipmask);
	cp = http_get_form_variable(state, ezsetup_fr_dlci);
	if (cp && cp[0]) {
	    dlci = atoi(cp);
	    if ((dlci >= 16) && (dlci <= 1007)) {
		setup_printf(sp, "\n frame-relay map ip %i %d broadcast",
			     newipaddr, dlci);
	    } else {
		http_ezsetup_error(state, error, ezsetup_fr_dlci,
				   "Invalid DLCI");
	    }
	}
	setup_printf(sp, "\nip route 0.0.0.0 0.0.0.0 serial 0");
    }
}

static void fr_http_setup_startup (httpstate *state)
{
  printf("\n<DT><A HREF=http:/ezsetup>Frame Relay Setup</A>"
	 " - basic router configuration.");
}

static void fr_http_root (httpstate *state)
{
    printf("\n<DT><A HREF=http:/exec/show/frame-relay/map/CR>"
	   "Show Frame-Relay map</A> - display Frame-Relay map entries.");
    printf("\n<DT><A HREF=http:/exec/show/frame-relay/pvc/CR>"
	   "Show Frame-Relay pvc</A> - display Frame-Relay pvc statistics.");
    printf("\n<DT><A HREF=http:/exec/show/interface/serial/0/CR>"
	   "Show Frame-Relay interface</A> - "
	   "display Frame-Relay interface staticstics.");
}

static void fr_http_init (subsystype *subsys)
{
    hwidbtype *hwidb;

    FOR_ALL_HWIDBS(hwidb) {
	if (hwidb->status & IDB_SERIAL) {
	    fr_ezsetup_idb = hwidb->firstsw;
	    break;
	}
    }

    if (fr_ezsetup_idb) {
	reg_add_http_setup_startup(fr_http_setup_startup,
				   "fr_http_setup_startup");
	reg_add_http_root(fr_http_root, "fr_http_root");
	reg_add_http_ezsetup_init(fr_http_ezsetup_form_init,
				  "fr_http_ezsetup_form_init");
	reg_add_http_ezsetup_send(fr_http_ezsetup_send,
				  "fr_http_ezsetup_send");
	reg_add_http_ezsetup_post(fr_http_ezsetup_post,
				  "fr_http_ezsetup_post");
    }
}

#define FR_HTTP_MAJVERSION 	1
#define FR_HTTP_MINVERSION 	0
#define FR_HTTP_EDITVERSION  	1

SUBSYS_HEADER(fr_http, FR_HTTP_MAJVERSION,
	      FR_HTTP_MINVERSION, FR_HTTP_EDITVERSION,
	      fr_http_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost, "req: iphost, http, http_ezsetup");
