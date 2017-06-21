/* $Id: isdn_http.c,v 3.2.4.11 1996/07/10 07:11:38 anke Exp $
 * $Source: /release/112/cvs/Xsys/wan/isdn_http.c,v $
 *------------------------------------------------------------------
 * ISDN HTTP functions
 *
 * December 1995, Robert Widmer
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: isdn_http.c,v $
 * Revision 3.2.4.11  1996/07/10  07:11:38  anke
 * CSCdi59766:  Remote router name is not updated, if no phone number
 * Update remote router name, phone number, and interface speed fields
 * Branch: California_branch
 *
 * Revision 3.2.4.10  1996/07/10  02:49:28  anke
 * CSCdi56934:  Alphanumeric characters allowed in phone number field
 * Allow only #, - and numbers
 * Branch: California_branch
 *
 * Revision 3.2.4.9  1996/07/06  05:58:07  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.2.4.8  1996/07/03  18:03:02  snyder
 * CSCdi62021:  char *s -> char []s in http
 * Branch: California_branch
 *              100 bytes
 *
 * Revision 3.2.4.7  1996/06/19  18:30:52  widmer
 * CSCdi56931:  Length of the text fields are high
 * Branch: California_branch
 * Add maxlength to ezsetup form variables
 *
 * Revision 3.2.4.6  1996/06/18  14:48:17  nkwok
 * CSCdi57950:  Show dialer should be removed from non-ISDN router
 * Branch: California_branch
 * Move 'Show dialer status' from http_send_root_page() to isdn_http_root()
 *
 * Revision 3.2.4.5  1996/06/18  00:29:37  nkwok
 * CSCdi57945:  EZSetup name need changing
 * Branch: California_branch
 * Add sub-system setup registry
 *
 * Revision 3.2.4.4  1996/06/17  23:30:10  widmer
 * CSCdi60630:  Add callin to ezsetup generated ppp auth command
 * Branch: California_branch
 *
 * Revision 3.2.4.3  1996/06/17  23:22:32  widmer
 * CSCdi60631:  Change ezsetup generated dialer-list command
 * Branch: California_branch
 *
 * Revision 3.2.4.2  1996/05/23  19:54:00  nkwok
 * CSCdi57859:  HTTP data verification
 * Branch: California_branch
 *
 * Revision 3.2.4.1  1996/03/18  22:48:01  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.6.1  1996/03/16  07:55:50  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2  1996/03/09  22:58:29  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.1  1996/02/11  04:26:04  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add placeholder files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "ctype.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "subsys.h"
#include "packet.h"
#include "dialer.h"
#include "address.h"
#include "../tcp/http.h"
#include "../isdn/switch_def.h"
#include "../les/isdn_bri.h"
#include "../isdn/tune.h"
#include "../isdn/gendef.h"
#include "../isdn/gp.h"
#include "../ui/setup.h"
#include "../wan/dialer_private.h"
#include "../ip/ip_registry.h"
#include "login.h"

/*
 * We need to maintain our own list as the PRI and BRI switch-types are
 * intermingled in SWITCH_TYPE.  The corresponding help strings for
 * each type are held in switch_strings below.
 */
typedef struct switch_name_ {
    const char *keyword;
    const char *help;
} switch_name;

static switch_name switch_names[] = {
    { "basic-5ess", "AT&amp;T 5ESS (U.S)" },
    { "basic-1tr6", "1TR6 (Germany)" },
    { "basic-dms100", "Northern DMS-100" },
    { "basic-net3", "NET3 (UK & Europe)" },
    { "basic-nwnet3", "NET3 (Norway)" },
    { "basic-nznet3", "NET3 (New Zealand)" },
    { "basic-ni1", "National ISDN-1" },
    { "basic-ts013", "TS013 (Australia)" },
    { "ntt", "NTT (Japan)" },
    { "vn2", "VN2 (France)" },
    { "vn3", "VN3 (France)" },
    { NULL, NULL }
};

static const char ezsetup_switchtype[] = "isdn_stype";
static const char isdn_remotename[] = "isdn_remotename";
static const char isdn_remotenumber[] = "isdn_remotenumber";
static const char isdn_chappass[] = "isdn_chappass";
static const char isdn_spid1[] = "isdn_spid1";
static const char isdn_spid2[] = "isdn_spid2";
static const char isdn_speed[] = "isdn_speed";
static hwidbtype *isdn_ezsetup_idb;

static void isdn_http_ezsetup_form_init (httpstate *state)
{
    dialermaptype *dialer_map;
    dialerentrytype   *dialer_entry;
    addrtype      proto_addr;
    usernametype  *user = NULL;
    char          *dialer_str = NULL;
    char          *remotename = NULL;

    bzero(&proto_addr, sizeof(addrtype));
    proto_addr.ip_addr = (192<<24) + (168<<16) + (254<<8) + 254;
    proto_addr.length = ADDRLEN_IP;
    proto_addr.type = ADDR_IP;

    http_form_add(state, ezsetup_switchtype, "%s",
		  print_isdn_switch(isdn_getswitchtype()));

    dialer_map = 
	dialer_addr_lookup(isdn_ezsetup_idb->dialerdb, &proto_addr, LINK_IP);
    if (dialer_map) {
        http_form_add(state, isdn_speed, "%d", dialer_map->entry->speed);
	dialer_entry = dialer_map->entry;
        dialer_str = dialer_entry->dialer_string;
        remotename = dialer_entry->hostname;
        if (remotename) {
            for (user = (usernametype *)userQ.qhead; user; user = user->next) {
                if (strcmp(user->name, remotename) == 0) {
                    break;
		}
	    }
	}
    } else {
        http_form_add(state, isdn_speed, "64");
        if (isdn_ezsetup_idb && 
            isdn_ezsetup_idb->dialerdb &&
            isdn_ezsetup_idb->dialerdb->dialer_string &&
            isdn_ezsetup_idb->dialerdb->dialer_string->entry) {
            dialer_entry = isdn_ezsetup_idb->dialerdb->dialer_string->entry;
            dialer_str = dialer_entry->dialer_string;
        } else {
            dialer_str = NULL;
        }
        user = userQ.qtail;
        if (user)
            remotename = user->name;
    }

    if (dialer_str)
        http_form_add(state, isdn_remotenumber, "%s", dialer_str);

    if (user && user->password)
        http_form_add(state, isdn_chappass, HTTP_CHAP_PASSWD_SET);

    http_form_add(state, isdn_remotename, "%s", remotename);
}

static void isdn_http_ezsetup_send (httpstate *state)
{
    char *switch_type;
    int i;

    printf("\n<H3>ISDN Interface</H3>");

    printf("\nYou need to know the name of the router you will be "
	   "dialing up, and the password used for CHAP "
	   "authentication on the link.  This CHAP password should "
	   "have been supplied to you, and is different from the "
	   "router password above.  You also have to know the type "
	   "of ISDN Switch that the phone company is using, and "
	   "possibly the SPIDs that they have assigned to you.  If they "
	   "are not using SPIDs you may leave those fields blank."
	   "\n<DL>"
	   );

    http_form_print_variable(state, HTTP_INPUT_STRING, UNAMELEN,
			     "Remote router name", isdn_remotename);
    http_form_print_variable(state, HTTP_INPUT_STRING, 144,
			     "Remote phone number", isdn_remotenumber);
    http_form_print_variable(state, HTTP_INPUT_STRING, PASSWORDLEN,
			     "Remote CHAP password", isdn_chappass);

    /*
     * Get current switch-type.  If not set, default it to 5ESS for
     * now.
     */

    switch_type = http_get_form_variable(state, ezsetup_switchtype);
    printf("\n<DT>ISDN switch type <SELECT NAME=%s>",
	   ezsetup_switchtype);

    for (i = 0; switch_names[i].keyword; i++) {
	printf("\n<OPTION VALUE=%s %s> %s ",
	       switch_names[i].keyword,
	       (strcasecmp(switch_names[i].keyword, switch_type) == 0)
	       ? "SELECTED" : "",
	       switch_names[i].help);
    }

    printf("\n</SELECT>");
	
    printf("\n<DT>ISDN Interface Speed"
	   "\n<DL>"
	   "\n<DT><INPUT TYPE=radio NAME=%s VALUE=56 %s> 56k"
	   "\n<DT><INPUT TYPE=radio NAME=%s VALUE=64 %s> 64k"
	   "\n</DL>",
	   isdn_speed,
	   ((strcmp(http_get_form_variable(state, isdn_speed), "56") == 0)
	    ? "CHECKED" : ""),
	   isdn_speed,
	   ((strcmp(http_get_form_variable(state, isdn_speed), "64") == 0)
	    ? "CHECKED" : ""));

    http_form_print_variable(state, HTTP_INPUT_STRING,
			     (LEN_SPID + LEN_LDN) - 2,
			     "ISDN Service Profile Identifier (SPID) 1",
			     isdn_spid1);
    http_form_print_variable(state, HTTP_INPUT_STRING,
			     (LEN_SPID + LEN_LDN) - 2,
			     "ISDN Service Profile Identifier (SPID) 2",
			     isdn_spid2);
    printf("\n</DL>");
}

static void isdn_http_ezsetup_post (httpstate *state,
				    setup_struct *sp,
				    boolean *error)
{
    char *cp;
    char *arg1;
    usernametype *user;

    cp = http_get_form_variable(state, ezsetup_switchtype);
    if (cp && cp[0]) {
	setup_printf(sp, "\nisdn switch-type %s", cp);
    }
    cp = http_get_form_variable(state, isdn_remotename);
    switch(http_token_count(cp)) {
      case 0:
	http_ezsetup_error(state, error, isdn_remotename,
			   "Remote router name is required.");
	break;
      case 1:
	arg1 = cp;
	cp = http_get_form_variable(state, isdn_chappass);
	for (user = (usernametype *)userQ.qhead; user; user = user->next) {
            if (strcmp(user->name, arg1) == 0)
                break;
	}
        if ((strcmp(cp, HTTP_CHAP_PASSWD_SET) == 0) && !user) {
	    http_ezsetup_error(state, error, isdn_chappass,
			       "New remote router name requires new password.");
	}
	else if (!cp || !cp[0]) {
	    http_ezsetup_error(state, error, isdn_chappass,
			       "A remote CHAP password is required.");
	}
	else {
	    if (user) {
		setup_printf(sp, "\nusername %s password %d %s ",
			     user->name, user->flags & USER_ENCRYPTION, 
			     user->password);
	    }
	    else {
		setup_printf(sp, "\nusername %s password %s", arg1, cp);
	    }
	}
	break;
      default:
	http_ezsetup_error(state, error, isdn_remotename,
			   "Remote router name is invalid - too many tokens");
	break;
    }

    setup_printf(sp, "\ninterface bri 0");
    setup_printf(sp,
		 "\n ip unnumbered ethernet 0"
		 "\n encapsulation ppp"
		 "\n no shutdown"
		 );
    cp = http_get_form_variable(state, isdn_remotenumber);
    switch(http_token_count(cp)) {
      case 0:
	http_ezsetup_error(state, error, isdn_remotenumber,
			   "Remote phone number is required");
	break;
      case 1:
	arg1 = cp;
	if (!legal_phone_number(arg1)) {
	    http_ezsetup_error(state, error, isdn_remotenumber,
	        "Remote phone number is invalid - numbers only");
	    break;
	}
	cp = http_get_form_variable(state, isdn_speed);
	if (cp && cp[0] && !strcmp(cp, "64")) {
	    setup_printf(sp, "\n dialer string %s", arg1);
	} else {
	    setup_printf(sp, "\n dialer map ip 192.168.254.254 speed 56");
	    cp = http_get_form_variable(state, isdn_remotename);
	    if (cp && cp[0]) {
		setup_printf(sp, " name %s", cp);
	    }
	    setup_printf(sp, " %s", arg1);
	}
	break;
      default:
	http_ezsetup_error(state, error, isdn_remotenumber,
			   "Remote phone number is invalid - too many tokens");
	break;
    }

    cp = http_get_form_variable(state, isdn_spid1);
    switch(http_token_count(cp)) {
      case 0:
	break;
      case 1:
      case 2:
	setup_printf(sp, "\n isdn spid1 %s", cp);
	break;
      default:
	http_ezsetup_error(state, error, isdn_spid1,
			   "ISDN SPID1 is invalid - too many tokens");
	break;
    }

    cp = http_get_form_variable(state, isdn_spid2);
    switch(http_token_count(cp)) {
      case 0:
	break;
      case 1:
      case 2:
	setup_printf(sp, "\n isdn spid2 %s", cp);
	break;
      default:
	http_ezsetup_error(state, error, isdn_spid2,
			   "ISDN SPID2 is invalid - too many tokens");
	break;
    }

    setup_printf(sp,
		 "\n ppp authentication chap callin"
		 "\n dialer-group 1"
		 "\n!"
		 "\nip classless"
		 "\naccess-list 101 deny udp any any eq snmp"
		 "\naccess-list 101 deny udp any any eq ntp"
		 "\naccess-list 101 permit ip any any"
		 "\ndialer-list 1 protocol ip list 101"
		 );

    cp = http_get_form_variable(state, isdn_speed);
    if (cp && cp[0] && !strcmp(cp, "64")) {
	setup_printf(sp, "\nip route 0.0.0.0 0.0.0.0 bri 0");
    } else {
	setup_printf(sp,
		     "\nip route 0.0.0.0 0.0.0.0 192.168.254.254"
		     "\nip route 192.168.254.254 255.255.255.255 bri 0");
    }
}

static void isdn_http_setup_startup (httpstate *state)
{
    printf("\n<DT><A HREF=http:/ezsetup>ISDN Setup</A>"
	   " - basic router configuration.");
}

static void isdn_http_root (httpstate *state)
{
    printf("\n<DT><A HREF=http:/exec/show/dialer/CR>Show dialer state</A>"
	   " - display the dialing status.");
    printf("\n<DT><A HREF=http:/exec/show/isdn/status/CR>Show ISDN status</A>"
	   " - display the status of the ISDN link.");
}

static void isdn_http_init (subsystype *subsys)
{
    hwidbtype *idb;

    /*
     * Find first BRI interface
     */
    FOR_ALL_HWIDBS(idb) {
	if (idb->status & IDB_ISDN) {
	    isdn_ezsetup_idb = idb;
	    break;
	}
    }

    if (idb) {
	reg_add_http_setup_startup(isdn_http_setup_startup,
				   "isdn_http_setup_startup");
	reg_add_http_root(isdn_http_root, "isdn_http_root");
	reg_add_http_ezsetup_init(isdn_http_ezsetup_form_init,
				  "isdn_http_ezsetup_form_init");
	reg_add_http_ezsetup_send(isdn_http_ezsetup_send,
				  "isdn_http_ezsetup_send");
	reg_add_http_ezsetup_post(isdn_http_ezsetup_post,
				  "isdn_http_ezsetup_post");
    }
}

int
legal_phone_number (const char *number_str)
{
    int i=0;
    char c;

    while ((c = number_str[i]) != '\0') {
	if (!isdigit(c) && c != '#' && c != '-')
	    return(0);
	i++;
    }
    return(1);
}

#define ISDN_HTTP_MAJVERSION 	1
#define ISDN_HTTP_MINVERSION 	0
#define ISDN_HTTP_EDITVERSION  	1

SUBSYS_HEADER(isdn_http, ISDN_HTTP_MAJVERSION,
	      ISDN_HTTP_MINVERSION, ISDN_HTTP_EDITVERSION,
	      isdn_http_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost, "req: iphost, http, http_ezsetup");
