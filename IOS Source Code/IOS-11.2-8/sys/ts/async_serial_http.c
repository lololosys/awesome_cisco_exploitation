/* $Id: async_serial_http.c,v 3.1.2.5 1996/08/01 20:08:09 hma Exp $
 * $Source: /release/112/cvs/Xsys/ts/async_serial_http.c,v $
 *------------------------------------------------------------------
 * Async Serial HTTP EZSetup support 
 *
 * June 1996, Parag Bopardikar
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: async_serial_http.c,v $
 * Revision 3.1.2.5  1996/08/01  20:08:09  hma
 * CSCdi59767:  Phone number unable to remove thru http
 * CSCdi62453: Router name is not updated, if no phone number and CHAP
 * pass.
 * CSCdi62450: Alphanumeric characters allowed in phone number field.
 *
 * Code reviewer: ParagBopardikar
 *
 * Branch: California_branch
 *
 * Revision 3.1.2.4  1996/07/06  05:57:30  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.1.2.3  1996/06/19  18:30:47  widmer
 * CSCdi56931:  Length of the text fields are high
 * Branch: California_branch
 * Add maxlength to ezsetup form variables
 *
 * Revision 3.1.2.2  1996/06/18  00:29:27  nkwok
 * CSCdi57945:  EZSetup name need changing
 * Branch: California_branch
 * Add sub-system setup registry
 *
 * Revision 3.1.2.1  1996/06/12  23:04:00  pbopardi
 * CSCdi60241:  Async HTTP 11.2 commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/06/05  22:24:20  pbopardi
 * place holder for async HTTP ezsetup
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "login.h"
#include "interface_private.h"
#include "subsys.h"
#include "../tcp/http.h"
#include "../ui/setup.h"
#include "../ip/ip_registry.h"
#include "../wan/dialer_registry.h"
#include "ttysrv.h"
#include "chat.h"

static const char async_serial_remotename[] = "async_remotename";
static const char async_serial_remotenumber[] = "async_remotenumber";
static const char async_serial_chappass[] = "async_chappass";
static const char async_serial_linepass[] = "async_linepass";
static const char async_serial_modemspeed[] = "async_modemspeed";
static const char async_modem_baud_288k[] = "28.8";
static const char async_modem_baud_144k[] = "14.4";
static const char async_serial_modeminit[] = "async_modem_initstring";
static const char async_modem_resetscript[] = "modem-reset";


int IsPhoneString(char * lpStr)
{
int i;
 
   if (!(lpStr && lpStr[0]))
     return (0);
 
   for (i=0; (*lpStr != '\0') && (*lpStr != ' '); i++, lpStr++) {
     if ( ((*lpStr >= '0') && (*lpStr <= '9')) || (*lpStr == '-') )
       continue;
     else
       break;
   }
 
   if (*lpStr == '\0' && i > 1)
      return (1);
   else
      return (0);
}

static void async_get_first_username_passwd (char *username, char *passwd)
{
   usernametype *user;
   for (user = (usernametype *)userQ.qhead; user; user = user->next) {
       if (user->name) {
	   strcpy(username, user->name);
	   find_passwd(user->name, passwd);
           break;
       }
   }
}

static void async_serial_http_ezsetup_form_init (httpstate *state)
{
    hwidbtype *hwidb;
    char      userName[UNAMELEN+1];
    char      password[PASSWORDLEN+1];

    userName[0] = '\0';
    password[0] = '\0';

    FOR_ALL_HWIDBS(hwidb) {
	if (is_async(hwidb)) {
	    break;
	}
    }

    if (hwidb) {
	char *dialer_string = reg_invoke_get_dialer_string(hwidb);

        /* populate the phone number, username & chap password */
	if (dialer_string != NULL) {
	    http_form_add(state, async_serial_remotenumber, "%s",
			  dialer_string);
	}

	/* populate the username/password, get the first in the userQ
	 * There is a possibility that more than one username/passwd
	 * might have been configured via CLI, we present the first in
	 * the userQ no matter what !
         */
        async_get_first_username_passwd(userName, password);
	if (userName[0]) {
	    http_form_add(state, async_serial_remotename, "%s", userName);
        }
	/* populate the modem speed, we assume that the txspeed and the
	 * rxspeed are same & populate the reset chat script
	 */
	if (hwidb->idb_tty) {
	    if (hwidb->idb_tty->tty_txspeed == ASYNC_MAXSPEED) {
		http_form_add(state, async_serial_modemspeed,
			      async_modem_baud_288k);
	    } else {
		http_form_add(state, async_serial_modemspeed,
			      async_modem_baud_144k);
	    }
	}
    }
}

static void async_serial_http_ezsetup_send (httpstate *state)
{
    printf("\n<H3>Asynchronous Serial Interface</H3>");

    printf("\nYou need to know the name of the router you will be "
	   "dialing up, and the password used for CHAP "
	   "authentication on the link.  This CHAP password should "
	   "have been supplied to you, and is different from the "
	   "router password above."
	   "\n<DL>"
	   );

    http_form_print_variable(state, HTTP_INPUT_STRING, UNAMELEN,
	                     "Remote router name", async_serial_remotename);
    http_form_print_variable(state, HTTP_INPUT_STRING, 144,
			     "Remote phone number", async_serial_remotenumber);
    http_form_print_variable(state, HTTP_INPUT_STRING, PASSWORDLEN,
			     "Remote CHAP password", async_serial_chappass);

    printf("\n</DL>"
	   "\nEnter a password to be prompted when a user dials "
	   "into this router."
	   "\n<DL>"
	   );
    http_form_print_variable(state, HTTP_INPUT_STRING, PASSWORDLEN,
			     "Local line password", async_serial_linepass);

    printf("\n</DL>"
	   "\nSelect the speed of the modem connected to the router and "
	   "enter the Modem initialization string."
	   "\n<DL>"
	   );

    printf("\n<DT>Modem Speed"
	   "\n<DL>"
	   "\n<DT><INPUT TYPE=radio NAME=%s VALUE=14.4 %s> 14.4k"
	   "\n<DT><INPUT TYPE=radio NAME=%s VALUE=28.8 %s> 28.8k"
	   "\n</DL>",
	   async_serial_modemspeed,
	   ((strcmp(http_get_form_variable(state, async_serial_modemspeed),
		    async_modem_baud_144k) == 0)
	    ? "CHECKED" : ""),
	   async_serial_modemspeed,
	   ((strcmp(http_get_form_variable(state, async_serial_modemspeed),
		    async_modem_baud_288k) == 0)
	    ? "CHECKED" : ""));
    http_form_print_variable(state, HTTP_INPUT_STRING, 216,
	                     "Modem Initialization String",
			     async_serial_modeminit);
}

static void async_serial_http_ezsetup_post (httpstate *state,
					    setup_struct *sp,
					    boolean *error)
{
    char *cp;
    char *arg1;
    boolean reset_script_present;
    hwidbtype *hwidb;
    char * DialerString;

    FOR_ALL_HWIDBS(hwidb) {
        if (is_async(hwidb)) {
	    break;
	}
    }

    reset_script_present = FALSE;

    /* Global commands generated */
    cp = http_get_form_variable(state, async_serial_remotename);
    if (http_token_count(cp) == 1) {
        arg1 = cp;
        cp = http_get_form_variable(state, async_serial_chappass);
        if (http_token_count(cp) == 1) {
            setup_printf(sp, "\nusername %s password %s", arg1, cp);
        }
        else {
           http_ezsetup_error(state, error, async_serial_chappass,
                         "Please enter a correct CHAP password.");
        }
    }    
    else {
           http_ezsetup_error(state, error, async_serial_remotename,
                         "Remote router name is invalid.");
    }


    /* if a previous modem reset script exists then do a no on it */
    if (hwidb && hwidb->idb_tty && hwidb->idb_tty->scr_reset &&
	hwidb->idb_tty->scr_reset_comp) {
	chat_script *temp_script;
	temp_script = find_chat_script(hwidb->idb_tty->scr_reset_comp);
	if (temp_script && temp_script->script) {
	    setup_printf(sp, "\nno chat-script %s", async_modem_resetscript);
	}
    }

    cp = http_get_form_variable(state, async_serial_modeminit);
    if (cp && cp[0]) {
	setup_printf(sp, "\nchat-script %s \"\" \"%s\" \"OK\" \"\"",
		     async_modem_resetscript, cp);
	reset_script_present = TRUE;
    }

    setup_printf(sp,
		 "\nip classless"
		 "\nip subnet-zero"
		 "\naccess-list 101 deny udp any any eq snmp"
		 "\naccess-list 101 deny udp any any eq ntp"
		 "\naccess-list 101 permit ip any any"
		 "\ndialer-list 1 list 101"
		 "\n!"
		 );
    setup_printf(sp, "\nchat-script default ABORT ERROR");
    setup_printf(sp, " ABORT BUSY \"\" AT");
    setup_printf(sp, " OK ATDT\\T TIMEOUT 40 CONNECT \\p\\c");
    setup_printf(sp,"\nip route 0.0.0.0 0.0.0.0 async1"); 

    /* Interface specific commands */
    setup_printf(sp,
		 "\ninterface Async 1"
		 "\n ip unnumbered ethernet 0"
		 "\n encapsulation ppp"
		 "\n ppp authentication chap callin"
		 "\n no shutdown"
		 "\n async mode interactive"
		 "\n keepalive 10"
		 );

    /* dial on demand stuff */
    cp = http_get_form_variable(state, async_serial_remotenumber);
    if (IsPhoneString(cp)) {

       DialerString = reg_invoke_get_dialer_string(hwidb);
       setup_printf(sp, "\ndialer in-band");

       /* RemoteNumber changed ? */
       if ((DialerString) && (strlen(DialerString)))
          setup_printf(sp, "\nno dialer string %s", DialerString );
       setup_printf(sp, "\ndialer string %s", cp );
       setup_printf(sp,
                 "\ndialer wait-for-carrier-time 120"
                 "\ndialer-group 1");
    }
    else {
            http_ezsetup_error(state, error, async_serial_remotenumber,
                         "Remote phone number is invalid. Please enter a correct Remote phone number.");
    }

    /* line specific commands */
    setup_printf(sp,
		 "\nline 1"
		 "\n exec"
		 "\n autoselect ppp"
		 "\n login"
		 "\n script dialer default"
		 "\n modem inout"
		 "\n transport input all"
		 "\n stopbits 1"
		 "\n flowcontrol hardware"
		 );

    cp = http_get_form_variable(state, async_serial_modemspeed);
    if (cp && cp[0] && !strcmp(cp, async_modem_baud_288k)) {
	setup_printf(sp, "\nspeed 115200");
    } else {
	setup_printf(sp, "\nspeed 57600");
    }

    cp = http_get_form_variable(state, async_serial_linepass);
    if (cp && cp[0]) {
	setup_printf(sp, "\npassword %s", cp);
    }

    if (reset_script_present) {
	setup_printf(sp, "\nscript reset %s", async_modem_resetscript);
    }
}

static void async_serial_http_setup_startup (httpstate *state)
{
    printf("\n<DT><A HREF=http:/ezsetup>Async Setup</A>"
	   " - basic router configuration.");
}

static void async_serial_http_root (httpstate *state)
{
    printf("\n<DT><A HREF=http:/exec/show/async/status/CR>Show Async status</A>"
	   " - display the status of the Async link.");
}

static void async_serial_http_init (subsystype *subsys)
{
    reg_add_http_setup_startup(async_serial_http_setup_startup,
			       "async_serial_http1_setup_startup");
    reg_add_http_root(async_serial_http_root, "async_serial_http_root");
    reg_add_http_ezsetup_init(async_serial_http_ezsetup_form_init,
			      "async_serial_http_ezsetup_form_init");
    reg_add_http_ezsetup_send(async_serial_http_ezsetup_send,
			      "async_serial_http_ezsetup_send");
    reg_add_http_ezsetup_post(async_serial_http_ezsetup_post,
			      "async_serial_http_ezsetup_post");
}

#define ASYNC_SERIAL_HTTP_MAJVERSION 	1
#define ASYNC_SERIAL_HTTP_MINVERSION 	0
#define ASYNC_SERIAL_HTTP_EDITVERSION  	1

SUBSYS_HEADER(async_serial_http, ASYNC_SERIAL_HTTP_MAJVERSION,
	      ASYNC_SERIAL_HTTP_MINVERSION, ASYNC_SERIAL_HTTP_EDITVERSION,
	      async_serial_http_init, SUBSYS_CLASS_MANAGEMENT,
	      subsys_seq_iphost, "req: iphost, http, http_ezsetup");
