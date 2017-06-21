/* $Id: login.c,v 3.17.6.19 1996/08/09 21:18:17 billw Exp $
 * $Source: /release/112/cvs/Xsys/os/login.c,v $
 *------------------------------------------------------------------
 * login.c -- perform various sorts of login operations.
 *
 * December 1990, Bill Westfield
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: login.c,v $
 * Revision 3.17.6.19  1996/08/09  21:18:17  billw
 * CSCdi65014:  conflict in username parsing
 * Branch: California_branch
 *
 * Revision 3.17.6.18  1996/07/30  01:41:54  billw
 * CSCdi59105:  AAA authenthenication errors during continue login cause
 * failure
 * Branch: California_branch
 *
 * Revision 3.17.6.17  1996/07/17  19:11:20  sjackson
 * CSCdi49571:  AAA user name is not passed in login get address requests
 * User name determined from the authentication method in use
 * Branch: California_branch
 *
 * Revision 3.17.6.16  1996/07/15  19:37:05  widmer
 * CSCdi26155:  username foo bar pass ww saved to memory without the quotes
 * Branch: California_branch
 *
 * Revision 3.17.6.15  1996/06/28  23:24:53  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.17.6.14  1996/06/25  16:11:18  tkolar
 * CSCdi61155:  ARAP keepalive timeout will cause stack corruption
 * Branch: California_branch
 * Add more space for the tickle notification string, and
 * fix another small issue before it happens.
 *
 * Revision 3.17.6.13  1996/06/20  08:55:00  che
 * CSCdi60765:  Boot-up of igs-j-l.111-4.0.1 Kills passwords--Box needs
 * break-in
 * Branch: California_branch
 *
 * Revision 3.17.6.12  1996/06/18  21:01:39  sbelair
 * CSCdi54289:  NAS continuously displays timeout expired! messages
 * Branch: California_branch
 *
 * Revision 3.17.6.11  1996/06/18  10:33:26  che
 * CSCdi41718:  login timeout gives an extra message
 * Branch: California_branch
 * Removed printing of extra error message if a message has already been
 * printed.
 *
 * Revision 3.17.6.10  1996/06/18  00:33:25  ahh
 * CSCdi60459:  User Access Verification not printed anymore
 * Test the callback flag on the acflags field of the tty, not the actype
 * field.
 * Branch: California_branch
 *
 * Revision 3.17.6.9  1996/06/08  19:43:26  che
 * CSCdi57850:  password encrytion not working correctly for vty
 * Branch: California_branch
 * Implement proper handling of line password structures when
 * password encryption is turned on.
 *
 * Revision 3.17.6.8  1996/06/05  21:12:35  carrel
 * CSCdi44545:  enable secret ? help is confusing to customers
 * Branch: California_branch
 * cleanup password usage (lines).
 *
 * Revision 3.17.6.7  1996/06/05  18:19:10  carrel
 * CSCdi44545:  enable secret ? help is confusing to customers
 * Branch: California_branch
 * Cleaned up usage of passwords (enable pass, enable secret and username)
 * throughout.
 *
 * Revision 3.17.6.6  1996/05/29  00:17:56  swenson
 * CSCdi58793:  conflict in parsing of username command
 * Branch: California_branch
 * use different variables for privilege and callback options
 *
 * Revision 3.17.6.5  1996/05/21  09:58:17  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.17.6.4  1996/05/10  01:36:27  wmay
 * CPP and ppp half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.17.6.3  1996/04/19  15:24:32  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.2.6.9  1996/04/16  03:55:54  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 2.2.6.8  1996/04/09  21:28:15  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.2.6.7  1996/03/19  08:25:23  che
 * Branch: IosSec_branch
 *
 * Revision 2.2.6.6  1996/03/05  04:43:59  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 2.2.6.5  1996/02/28  06:14:45  lol
 * Branch: IosSec_branch
 * TACACS+ processing.
 *
 * Revision 2.2.6.4  1996/02/21  03:49:07  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.2.6.3  1995/12/19  04:09:21  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 2.2.6.2  1995/12/17  02:12:29  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.2.6.1  1995/11/17  17:22:41  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.17.6.2  1996/04/15  14:59:01  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.17.6.1  1996/03/18  21:30:17  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.10.2.3  1996/03/16  07:20:49  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.10.2.2  1996/03/07  10:18:59  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.10.2.1  1996/02/20  16:40:15  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.17  1996/03/05  01:19:17  billw
 * CSCdi49461:  autoselect during-login causes premature authorization
 * Additional commenting...
 *
 * Revision 3.16  1996/02/23  10:37:59  billw
 * CSCdi49461:  autoselect during-login causes premature authorization
 * Define a new flag AUTOSELECT_AUTHORIZE that survives across death().
 * Set it at the same time AUTOSELECTED is set, but only clear it when
 * a new autoselect starts.
 *
 * Revision 3.15  1996/02/09  10:04:03  billw
 * CSCdi48421:  autoselect during-login causes multiple execs on single tty
 *
 * Revision 3.14  1996/02/09  09:03:03  billw
 * CSCdi43680:  RADIUS doesnt support magic number ip address values
 * also add authstruct->auth_idb, and change auth->tty to auth->auth_tty
 *
 * Revision 3.13  1996/02/07  16:14:29  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.12  1996/01/29  11:29:58  billw
 * CSCdi13190:  Increase password characters encryted
 * Tell users how long the password could be if it gets truncated.
 * Fix a couple spots where the length was not checked or not checked
 * unless encryption was turned on first.
 *
 * Revision 3.11  1996/01/29  00:30:33  billw
 * CSCdi13190:  Increase password characters encryted
 * Make sure that the maximum length password that can be encrypted is
 * the maximum length that will be accepted.
 *
 * Revision 3.10  1996/01/22  06:56:12  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.9  1995/12/17  18:34:18  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.8  1995/12/12  06:01:02  billw
 * CSCdi41773:  Login (username) promtps different with local override
 * confiured
 *
 * Revision 3.7  1995/12/10  21:13:00  widmer
 * CSCdi45105:  Running-configs line exec-timeout and history change at
 * login
 * Add argument to readline() to specify whether command should be added
 * to history, change existing readline() calls to use TRUE, change
 * askstring() to call readline() with FALSE.
 *
 * Revision 3.6  1995/12/06  02:30:27  lol
 * CSCdi44055:  bug in the NAS code, after auth message not sent
 *
 * Revision 3.5  1995/11/29  00:16:12  lol
 * CSCdi42385:  TACACS+ crashes router in aaa_split_AV
 *
 * Revision 3.4  1995/11/22  23:18:23  che
 * CSCdi41291:  Second login on same session doesnt generate xlogoff packet
 * Make EXEC LOGIN call reg_invoke_login_logout() to generate xlogoff
 * packet.
 *
 * Revision 3.3  1995/11/17  18:48:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:18:09  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:46:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.22  1995/11/08  21:19:01  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.21  1995/10/26  13:36:17  thille
 * CSCdi36960:  setup messages overwrite previous lines
 * Fix errors made by folks who apparently weren't aware that for some
 * reason we prepend \n on the front of lines of output instead of
 * appending it to the end.
 *
 * Revision 2.20  1995/10/21  00:21:03  carrel
 * CSCdi42490:  AAA cant set privilege level to 0
 * No longer use 0 to indicate "unset".
 *
 * Revision 2.19  1995/10/02  20:42:54  lol
 * CSCdi36244:  Console line failing authorization should be more forgiving
 *              Authenticated console users now bypass authorization.
 *
 * Revision 2.18  1995/09/25  08:37:09  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.17  1995/09/16  21:42:56  carrel
 * CSCdi40334:  Bus error crash in AAA accounting code
 * Removed unnecessary NULL ptr checks.
 * Added some needed NULL ptr checks!!
 * Made a call to acct_free() be safe!!
 * Removed unneeded call to aaa_dup_user().
 * Generally made the code prettier.  ;-)
 * Removed unused arg from aaa_dup_ifneeded().
 *
 * Revision 2.16  1995/09/12  04:33:03  carrel
 * CSCdi40150:  AAA privilege levels are not properly installed
 * Install the new privilege level in the csb in AAA authorization.
 *
 * Revision 2.15  1995/09/06  18:40:50  syiu
 * CSCdi39844:  PPP callback is broken (regress)
 *
 * Revision 2.14  1995/08/30  19:56:36  vandys
 * CSCdi38886:  Longer usernames needed
 * Extend, while maintaining compat with named things
 *
 * Revision 2.13  1995/08/08  20:50:25  lol
 * CSCdi37361:  AAA should not require auth for originating PPP connections
 *
 * Revision 2.12  1995/08/04  23:14:04  irfan
 * CSCdi38157:  memory leaks in PPP PAP, CHAP, Callback
 * fix memory leaks in pap_rrequest and chap_rresponse path towards
 * callback. change as many malloc's to malloc_named, as possible.
 *
 * Revision 2.11  1995/07/29  01:51:19  billw
 * CSCdi35954:  tacacs+ creates tcbs stuck in CLOSEWAIT (causes buffer
 * leak)
 *
 * Revision 2.10  1995/07/14  07:02:48  hampton
 * Convert Terminal Server to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi37093]
 *
 * Revision 2.9  1995/07/13 06:45:49  billw
 * CSCdi36954:  AAA accounting done improperly when exec login command is
 * used
 *
 * aaa_dup_user no longer duplicates acctQ.  New registry point for
 * changing from one username to another.
 *
 * Revision 2.8  1995/07/13  00:38:40  billw
 * CSCdi36951:  duplicate AAA ACCounting EXEC start records generated
 * (regress)
 * Note that the exec's "login" command is still broken.
 *
 * Revision 2.7  1995/06/30  18:52:31  widmer
 * CSCdi36635:  nv_add string should not contain newline
 * Fix dlsw NV generation
 *
 * Revision 2.6  1995/06/24  20:13:40  billw
 * CSCdi36362:  login exec command causes AAA accounting crash
 *
 * Revision 2.5  1995/06/20  16:11:14  lol
 * CSCdi34401: Tacacs notif.type 6-ena and 10-slipoff not sent to
 *             nondefault server
 *
 * Revision 2.4  1995/06/15  22:57:48  syiu
 * CSCdi35686:  Async callback AAA authorization and accounting phase not
 * working
 *
 * Revision 2.3  1995/06/15  03:42:19  lol
 * CSCdi35713:  AAA subsystems needs more debugging output
 *              Add
 *                 aaa debug authentication
 *                 aaa debug authorization
 *                 aaa debug accounting
 *
 * Revision 2.2  1995/06/09  13:12:10  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:56:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <stdlib.h>		/* for atoi */
#include <ctype.h>
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "../h/cs_registry.regh"
#include "../h/auth_registry.regh"
#include "ttysrv.h"
#include "packet.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "../ui/command.h"
#include "connect.h"
#include "aaa.h"
#include "login.h"
#include "hostname.h"
#include "access.h"
#include "../util/md5_crypt.h"
#include "../parser/parser_defs_config.h"
#include "../parser/parser_defs_autoselect.h"
#include "../parser/parser_actions.h"
#include "../ts/callback.h"
#include "../os/async.h"
#include "../ui/debug.h"
#include "../os/os_debug_flags.h"
#include "../util/random.h"
#include "techsupport.h"

/*
 * local storage
 */
queuetype userQ;
boolean login_have_servers;
/* OLD_AUTH -- remove next 2 lines when old authentication goes away */
boolean enable_use_login_server;
int enable_lastresort;
boolean password_encryption_enable;
password_struct enable_password[PRIV_MAX+1];
password_struct enable_secret[PRIV_MAX+1];
int login_server_attempts;
int login_input_timeout;
static const char useraccver[] = "\n\nUser Access Verification\n";

/*
 * login_init
 * Initialize the various login variables
 */

void login_init (void)
{
    int i;

    queue_init(&userQ, 0);
    for (i=PRIV_MIN; i<=PRIV_MAX; i++) {
	enable_password[i].pw = NULL;
	enable_password[i].encrypt_type = ENCRYPT_NONE;
	enable_secret[i].pw = NULL;
	enable_secret[i].encrypt_type = ENCRYPT_NONE;
    }
/* OLD_AUTH -- remove next 2 lines when old authentication goes away */
    enable_use_login_server = FALSE;
    enable_lastresort = TACACS_LAST_FAIL;
    password_encryption_enable = FALSE;
    login_have_servers = FALSE;
    login_server_attempts = LOGIN_DEF_ATTEMPTS;
    login_input_timeout = LOGIN_DEF_TIMEOUT;
    login_parser_init();
}

usernametype *find_passwd (char *username, char *password)
{
    usernametype *info = NULL;

    info = find_namedthing(&userQ, username);

    if (info == NULL) {
	return(NULL);
    }

    if (info->flags&USER_ENCRYPTION) {
        decrypt(info->password, password, 50);
    } else {
        strcpy(password, info->password);
    }

    return(info);
}

enum password_check_type check_local_password (char *name, char *password,
					       usernametype **ret_info)
{
    usernametype *info;
    char secret[PASSWORDLEN + 1];

    info = find_namedthing(&userQ, name);
    *ret_info = info;
    if (info == NULL)
	return(PASSWORD_NO_USER);

    if (info->flags & USER_NOPASSWORD)
	return(PASSWORD_NO_PASSWORD);

    if (info->flags & USER_ENCRYPTION) {
	decrypt(info->password, secret, PASSWORDLEN);
    } else {
	bcopy(info->password, secret, strlen(info->password) + 1);
    }

    if (strcmp(secret, password) != 0)
	return(PASSWORD_NOT_CORRECT);
    return(PASSWORD_CORRECT);
}
    

/* 
 * Duplicate an usernametype and all its fields (except next)
 */
usernametype *duplicate_usernametype (usernametype *orig_info)
{
    usernametype *new_info = NULL;
 
    if (orig_info) {
	/*
	 * Misc. Authentication blocks should use less
	 * obvious/pointing names
	 */
	new_info = malloc_named(sizeof(usernametype), "Ion Block");
	if (new_info) {
	   new_info->next = 0;
	   sstrncpy(new_info->password, orig_info->password, 
		    PASSWORDLEN_ENCRYPTED);
	   new_info->flags = orig_info->flags;
	   new_info->accesslist = orig_info->accesslist;
	   strcpy(new_info->name, orig_info->name);
	   setstring(&new_info->autocommand, orig_info->autocommand);
	   setstring(&new_info->dial_string, orig_info->dial_string);
	   new_info->callback_rotary_or_line =
		orig_info->callback_rotary_or_line;
           new_info->callback_incoming_idb = orig_info->callback_incoming_idb;
	}
    } 
    return(new_info);
}

/* 
 * Free an usernametype and all its string fields
 */
void free_usernametype (usernametype *info)
{
    if (info) {
	setstring(&info->autocommand, NULL);
	setstring(&info->dial_string, NULL);
	free(info);
    }
}
/* 
 * Clear the resources in a usernametype struct
 */
void clear_usernametype (usernametype *info)
{
    if (info) {
	setstring(&info->autocommand, NULL);
	setstring(&info->dial_string, NULL);
    }
}

/*
 * get_user_name
 *
 * Return address of the user name string associated with this tty (or
 * NULL if no user string set or invalid).
 */
char *get_user_name (tt_soc *tty)
{
    char *user;

    if (tty == NULL) {
	return (NULL);
    }

    if (old_access_control) {
	user = tty->user;
    } else {
	user = NULL;
	aaa_get_user_p(tty->ustruct, &user);
    }
    return (user && *user ? user : NULL);
}

/* OLD_AUTH -- remove this section when old authentication goes away */

/*
 * login_askboth
 * Ask for both a username and password (if required).  Check validity
 * using the local username database, and (if using tacacs) the remote
 * tacacs server as well.
 */

static boolean login_askboth (tt_soc *tty, boolean enabling, int attempts)
{
    char user[100], pw[50], *reason, olduser[100];
    usernametype *info = NULL;
    usernametype *temp_info = NULL;
    boolean ok, userresp;
    int i;

    tty->acflags &= ~ACF_RELOGIN;
    tty->tty_user_server = 0;
    /*
     * If this is a Callback user, bypass Authentication if requested
     */
    if ((tty->statbits & CALLBACK_IN_PROGRESS) && 
        (tty->acflags & ACF_CALLBACK_NOVERIFY)){
      tty->acflags &= ~ACF_CALLBACK_NOVERIFY;
      return(TRUE); 
    }

    ok = FALSE;
    reason = NULL;
    for (i = 0; !ok && i < attempts; i++) {
	reason = NULL;
	userresp = FALSE;
	do {
	    userresp = askstring(tty, user, sizeof user, "Username: ",
				 30 * ONESEC, 1, FALSE);
	} while (userresp && null(user));

	if ((userresp == FALSE) && (tty->autoselectmask & AUTOSELECTED))
	    return(FALSE);

	if (tty->statbits & (IDLETIMEOUT|CARDROP|HANGUP))
	    break;

	if (userresp == 0)
	    continue;

	info = find_namedthing(&userQ, user);

	if (tty->user != NULL) {
	    bcopy(tty->user, olduser, (strlen(tty->user) + 1));
	} else {
	    olduser[0] = '\0';
	}

	if (info) {
	    if (info->flags & USER_NOPASSWORD)
		ok = TRUE;
	    else {
		userresp = (askstring(tty, pw, sizeof pw, "Password: ",
				      30 * ONESEC, 1, TRUE));
		if ((userresp == FALSE) && (tty->autoselectmask & AUTOSELECTED))
	    	    return(FALSE);
		if (userresp != OK)
		    continue;
		if (passwdcheck(pw, info->password, 
				info->flags&USER_ENCRYPTION)) {
		    ok = TRUE;
		    sstrncpy(tty->saved_passwd, pw, sizeof(tty->saved_passwd));
		}
	    }
	} else if (login_have_servers && (tty->actype != AC_LOCAL)) {
	    ok = reg_invoke_login_login(tty, user, NULL, &reason);
	    if (reason != NULL)
		printf("\n%% %s\n", reason);
	} 
	if ((!ok) && (reason == NULL))
	    printf("\n%% Login invalid\n");
    }
    if (ok) {
	/* The login succeeded, so if we're executing "login" from the
	 * EXEC prompt, set the tty username back to the old user
	 * and do a TACACS logout to keep the accounting straight.  After
	 * we're done, reset the username to the new user.
	 */
	if (strlen(olduser) > 0) {
	    setstring(&tty->user, olduser);
	    reg_invoke_login_logout(tty);
	}
	setstring(&tty->user, user);
        temp_info = duplicate_usernametype(info);

        /*
         * If there is Callback to the caller, initiate it here
         * If return is FALSE
         *    callback is disabled, user does not have callback or registry 
         *    is not registered.
         *    login proceeds as if no callback were in effect. 
         * else if return is TRUE (control actually is not returned)
         *    Callback is in effect or the selected line for callback is 
         *    unavailable. In either case the line is disconnected for
         *    code consistency.
         */
        if (reg_invoke_callback_initiate(temp_info, tty, 
                                                ACF_CALLBACK_EXEC, TRUE)){
           death(tty);
	}
	if (info) {
	    /*
	     * Process flags and data in local username
	     */
	    if (info->flags & USER_ACCESSLIST)
		tty->accessout = info->accesslist;
	    if (info->flags & USER_AUTOCOMMAND)
		tty->tty_autocommand = info->autocommand;
	    if (info->flags & USER_NOESCAPE) {
		tty_savedefaults(tty);
		tty->tty_escapechar = -1;
	    }
	    if (info->flags & USER_NOHANGUP)
		tty->acflags |= ACF_RELOGIN;
	    SET_PRIV(tty->privilege, ((info->flags & USER_PRIV) >> 28));
	}
    }
    return(ok);
}
/* end of OLD_AUTH section */

/*
 * new_ascii_login()
 * This is the new mechanism for doing all ascii authentications via the
 * new authentication model.
 */
static boolean
new_ascii_login (tt_soc *tty, char *acc_list_name, int num_retries)
{
    authenstruct authen;
    userstruct *ustruct = NULL;
    char *buf, tty_str[10], *rem;
    boolean ret = FALSE;
    int num_tries = 0;

    /*
     * If this is a Callback user, bypass Authentication if requested
     */
    if ((tty->statbits & CALLBACK_IN_PROGRESS) && 
        (tty->acflags & ACF_CALLBACK_NOVERIFY)){
      tty->acflags &= ~ACF_CALLBACK_NOVERIFY;
      return(TRUE); 
    }

    /*
     * Misc. Authentication blocks should use less
     * obvious/pointing names
     */
    buf = malloc_named(AAA_MAX_REPLY_LEN, "Ion Reply Buff");
    if (!buf)
	return(FALSE);
    sprintf(&tty_str[0], "tty%d", tty->ttynum);
    while (num_tries++ < num_retries) {
	memset(&authen, 0, sizeof(authen));
	authen.tty = tty;
	rem = tty_get_remote_addr(tty);

	/* Create a new ustruct only if there isn't already one in the 
	 * tty-struct.  If tty->ustruct is not NULL, the function calling
	 * us MUST guarentee that it is a valid pointer!!!
	 */
	if (!tty->ustruct) {
	    ret = aaa_create_user(&ustruct, NULL, NULL, tty_str, rem,
				  AAA_AUTHEN_TYPE_ASCII, AAA_SVC_LOGIN,
				  tty->csb ? tty->csb->priv :
				  CUR_PRIV(tty->privilege));
	} else {
	    ustruct = tty->ustruct;
	    ret = TRUE;
	}
	if (ret)
	    ret = aaa_start_login(ustruct, &authen, acc_list_name,
				  AAA_AUTHEN_ACT_LOGIN, AAA_SVC_LOGIN);
	/* Now if we can't find any authen mechanism, then fail right away. */
	if (!ret) {
	    authen.status = AAA_AUTHEN_STATUS_FAIL;
	    num_tries = num_retries; /* Don't bother trying any more times. */
	}

	/* print banner_login if not NONE */
	if ((num_tries == 1) && ustruct->acc_method != AAA_ACC_METH_NONE) {
	    if (banner_login)
		printf("%r", banner_login);
	    if (local_override ||
		ustruct->acc_method != AAA_ACC_METH_TACACSPLUS)
		printf(useraccver);
	}

	while ((authen.status == AAA_AUTHEN_STATUS_GETDATA) ||
	       (authen.status == AAA_AUTHEN_STATUS_GETUSER) ||
	       (authen.status == AAA_AUTHEN_STATUS_GETPASS)) {

	    if ( authen.status == AAA_AUTHEN_STATUS_GETPASS &&
		ustruct->acc_method != AAA_ACC_METH_ENABLE &&
		ustruct->password) {
		/*
		 * Check for fallover to a new method that needs a password
		 * that we've already read.  Note that continue_login takes
		 * care of freeing ustruct->password.
		 */
		strcpy(buf, ustruct->password);
		ret = TRUE;
	    } else {
		ret = askstring(tty, buf, AAA_MAX_REPLY_LEN, authen.server_msg,
				login_input_timeout * ONESEC, 1,
				(authen.flags & AAA_RESP_FLAG_NOECHO));
	    }
	    if (!ret) {
		if (authen.server_msg)
		    *authen.server_msg = '\0'; /* So msg isn't printed again.*/
		if (stdio->statbits & CARDROP) {
		    aaa_abort_login(ustruct, &authen, "Carrier dropped");
		    authen.status = AAA_AUTHEN_STATUS_FAIL;
		    aaa_cleanup_login(ustruct, &authen);
		    if (ustruct != tty->ustruct) {
			aaa_free_user(&ustruct);
		    }
		    num_tries = num_retries;
		    continue;
		} else if (tty->autoselectmask & AUTOSELECTED) {
		    aaa_abort_login(ustruct, &authen, "Autoselected");
		    authen.status = AAA_AUTHEN_STATUS_FAIL;
		    aaa_cleanup_login(ustruct, &authen);
		    if (ustruct != tty->ustruct) {
			aaa_free_user(&ustruct);
		    }
		    num_tries = num_retries;
		    continue;
		}
		break;
	    }
	    if (authen.flags & AAA_RESP_FLAG_NOECHO) {
		printf("\n"); 	/* Since echo was off above */
	    }
	    authen.user_reply = buf;

	    setstring(&authen.server_msg, NULL);

	    aaa_cont_login(ustruct, &authen);
	    memset(buf, 0, AAA_MAX_REPLY_LEN);
	    authen.user_reply = NULL;
	}
	switch (authen.status) {
	case AAA_AUTHEN_STATUS_PASS:
	    if (authen.server_msg && *authen.server_msg)
		printf("%s\n", authen.server_msg);
	    if (ustruct != tty->ustruct) {
		/* Do this only if we did not pass in a ustruct in the tty */
		aaa_free_user(&(tty->ustruct));	/* Checks for NULL */
		tty->ustruct = ustruct;		/* Success, set new user */
	    }
	    break;
	case AAA_AUTHEN_STATUS_RESTART:
	case AAA_AUTHEN_STATUS_FAIL:
	    ret = FALSE;
	    if ((tty->statbits & CARDROP) ||
		(tty->autoselectmask & AUTOSELECTED))
		break;
	    if (authen.server_msg && *authen.server_msg)
		printf("%s\n", authen.server_msg);
	    else
		printf("\n%% Authentication failed.\n");
	    process_sleep_for(2*ONESEC);
	    break;
	case AAA_AUTHEN_STATUS_ERROR:
	default:
	    if (authen.server_msg) {
		if (*authen.server_msg) {
		    printf("%s\n", authen.server_msg);
		}
	    } else {
		printf("\n%% Error in authentication.\n");
	    }
	    if (authen.server_data_len && authen.server_data &&
		isprint(*authen.server_data)) {
		char *str = malloc_named(authen.server_data_len + 1,
					 "Login TA+ Msg");
		if (str) {
		    sstrncpy(str, authen.server_data,
			     authen.server_data_len+1);
		    buginf("\n%% Login: TACACS+ Server msg: %s", str);
		    free(str);
		}
	    }
	    process_sleep_for(2*ONESEC);
	    ret = FALSE;
	    break;
	} /* switch  authen.status */

	if (ustruct) {
	    aaa_cleanup_login(ustruct, &authen);
	    if (ustruct != tty->ustruct) {
		aaa_free_user(&ustruct);
	    }
	}
	if (ret)
	    break;	/* login succeeded */
    }

    free(buf);
    return(ret);
}

/*
 * check_login
 * Check for a password on this line.
 * We already know that logins are required.
 */

boolean check_login (tt_soc *tty)
{

    boolean result, madedefaults = FALSE;
    /*
     * If an automatic command specified, don't check password 
     */
    if (tty->tty_autocommand) {
	/*
	 * don't contine to run if some other process is still fiddling
	 * with the tty data structures.  This prevents nasty occurances
	 * like this exec finishing before it does.  Hopefully.
	 */
	while (tty->autoselectmask & AUTOSELECTED)
	    process_sleep_for(ONESEC/2);
	return(TRUE);
    }

    /*
     * Initialize autoselect variables.  Clear any previous notion of
     * whether the process was started by autoselect or not.
     */
    tty->autoselectmask &= ~(AUTOSELECTED|AUTOSELECT_AUTHORIZE);
    tty->autoselect_sample = 0;

    /*
     * The code that asks for usernames and passwords implements a timeout
     * by temporarilly modifying the line exec-timeout.  This can cause
     * problems if an nv write is attemped, so we need to save our defaults.
     * Note that if autoselect occurs, we have to leave the saved defaults
     * in place, since they may be in use by the autoselected process.
     */
    if (!tty->defaults) {
	tty_savedefaults(tty);
	madedefaults = TRUE;
    }
/* OLD_AUTH -- remove this section when old authentication goes away */
    if (old_access_control) {
	result = FALSE;		/* Assume failure */
	if ((tty->actype != AC_NONE) && banner_login)
	    printf("%r", banner_login);

	switch (tty->actype) {
	case AC_NONE:
	    result = TRUE;
	    break;

	case AC_PASSWORD:
	    /*
	     * If no password is set, don't insist on checking unless a vty.
	     */
	    if (!tty->password || !tty->password->pw ||
		!tty->password->pw[0]) {
		if (tty->type != VTY_LINE) {
		    result = TRUE;
		    break;
		} else {
		    printf("\n\nPassword required, but none set\n");
		    break;
		}
	    }
	    printf(useraccver);

	    /* 
	     *  if we have something selected to autoselect, and we want
	     *	to do it during login, start autoselecting 
	     */
	    if ((tty->autoselectmask & PARSER_AUTOSELECT_ALL)  &&
		(tty->autoselectmask & PARSER_AUTOSELECT_DURING_LOGIN)) {
		tty->autoselectmask |= AUTOSELECTING;
	    }

	    result = askpassword(tty, tty->password->pw,
				 tty->password->encrypt_type);
	    tty->autoselectmask &= ~AUTOSELECTING;
	    if (result) {
		break;		/* get out early */
	    }
	    if (tty->autoselectmask & AUTOSELECTED) {
		/* clean up and kill process */
		tty->autoselectmask &= ~AUTOSELECTED;
		process_kill(THIS_PROCESS);
	    }
	    break;
	case AC_LOCAL:
	case AC_TACACS:
	    if (!(tty->acflags & ACF_CALLBACK_NOVERIFY)) {
		printf(useraccver);
	    }

	    /* 
	     *  if we have something selected to autoselect, and we want
	     *	to do it during login, start autoselecting 
	     */
	    if ((tty->autoselectmask & PARSER_AUTOSELECT_ALL)  &&
		(tty->autoselectmask & PARSER_AUTOSELECT_DURING_LOGIN))
		tty->autoselectmask |= AUTOSELECTING;

	    result = login_askboth(tty, FALSE, login_server_attempts);
	    tty->autoselectmask &= ~AUTOSELECTING;
	    if (result) {
		break;		/* get out early */
	    }
	    if (tty->autoselectmask & AUTOSELECTED) {
		/* clean up and kill process */
		tty->autoselectmask &= ~AUTOSELECTED;
		process_kill(THIS_PROCESS);
	    }
	    break;
	} /* End of switch */
	if (madedefaults && tty->defaults &&
	    (memcmp(tty->defaults, &tty->current,
		    sizeof(tty_userdata)) == 0)) {
	    /*
	     * if we had to save our defaults, get rid of the extra
	     * storage now to save memory. The login code may
	     * change some of the user tty variables, so make sure
	     * it hasn't changed before freeing it.
	     */
	    free(tty->defaults);
	    tty->defaults = NULL;
	}	      
	return(result);
    } else {
/* end of OLD_AUTH section */

	/* 
	 *  if we have something selected to autoselect, and we want
	 *	to do it during login, start autoselecting 
	 */
	if ((tty->autoselectmask & PARSER_AUTOSELECT_ALL)  &&
	    (tty->autoselectmask & PARSER_AUTOSELECT_DURING_LOGIN))
	    tty->autoselectmask |= AUTOSELECTING;

	result = new_ascii_login(tty, &tty->login_acc_list_name[0],
				 login_server_attempts);
	tty->autoselectmask &= ~AUTOSELECTING;
	if (tty->autoselectmask & AUTOSELECTED) {
	    /* clean up and kill process */
	    tty->autoselectmask &= ~AUTOSELECTED;
	    process_kill(THIS_PROCESS);
	}
	if (madedefaults && tty->defaults &&
	    (memcmp(tty->defaults, &tty->current,
		    sizeof(tty_userdata)) == 0)) {
	    /*
	     * if we had to save our defaults, get rid of the extra
	     * storage now to save memory. The login code may
	     * change some of the user tty variables, so make sure
	     * it hasn't changed before freeing it.
	     */
	    free(tty->defaults);
	    tty->defaults = NULL;
	}	      
	return(result);
    }
}

/*
 * username_command
 * Parser a username and possible arguments.  The format is:
 *    USERNAME user [ACCESSLIST n] [NOPASSWORD | PASSWORD [crypttype] text]
 *    USERNAME user  [NOESCAPE] AUTOCOMMAND command...
 * Note that "Autocommand" and "password" must be the last argument in the
 * line since they can have arbitrary lengths and characters.
 */
void username_command (parseinfo *csb)
{
    usernametype *user;
    char password[PASSWORDLEN_ENCRYPTED+1];
    uint crypttype, len;
    boolean new_user = FALSE, user_error = FALSE;

    if (csb->nvgen) {

	for (user = (usernametype *)userQ.qhead; user; user = user->next) {
	    nv_write(TRUE, "%s %S", csb->nv_command, user->name);
	    nv_add(user->flags & USER_ACCESSLIST, " access-class %d",
		   user->accesslist);
	    if (((user->flags & USER_PRIV) >> 28) != DEFAULT_PRIV) {
		nv_add(TRUE, " privilege %d",
		       ((user->flags & USER_PRIV) >> 28));
	    }
	    nv_add(user->flags & USER_NOPASSWORD, " nopassword");
	    nv_add(user->flags & USER_NOESCAPE, " noescape");
	    nv_add(user->flags & USER_NOHANGUP, " nohangup");
            nv_add(user->flags & USER_NOCALLBACK_VERIFY," nocallback-verify");

            if (strlen(user->dial_string) == 0)
               nv_add(user->flags & USER_CALLBACK_DIALSTRING, 
                      " callback-dialstring \"\"");
            else
               nv_add(user->flags & USER_CALLBACK_DIALSTRING,
                      " callback-dialstring %s", user->dial_string);

            nv_add(user->flags & USER_CALLBACK_ROTARY," callback-rotary %d",
                  user->callback_rotary_or_line);
            nv_add(user->flags & USER_CALLBACK_LINE," callback-line %d",
                  user->callback_rotary_or_line);

	    /*
	     * If the password is not encrypted,
	     * and we have service password-encryption, then encrypt it now
	     */
	    if (password_encryption_enable &&
		(!(user->flags & USER_ENCRYPTION)) ) {
		memset(password, '\0', PASSWORDLEN_ENCRYPTED+1);
		crypttype = encrypt(user->password, password, NULL, 
				    PASSWORDLEN_ENCRYPTED);
		
		user->flags |= crypttype;
		memcpy(user->password, password, PASSWORDLEN_ENCRYPTED+1);
	    }
	    nv_add(*user->password, " password %d %s",
		   user->flags & USER_ENCRYPTION, user->password);

	    nv_write(user->flags & USER_AUTOCOMMAND,
		     "username %s autocommand %s",
		     user->name, user->autocommand);
	}
	return;
    }

    user = find_namedthing(&userQ, GETOBJ(string,1));
    if (user == NULL && csb->sense == TRUE) {
	user = malloc_named(sizeof(usernametype), "Ion New Block");
	if (!user)
	    return;
	sstrncpy(user->name, GETOBJ(string,1), sizeof(user->name));
	new_user = TRUE;
    } else {
	if (csb->sense == FALSE) {
	    if (user != NULL) {
	        free(user->autocommand);
		free(user->dial_string);
	        unqueue(&userQ, user);
		free(user);
	    }
	    return;
	}
    }
    /*  access-class  */
    if (GETOBJ(int,1)) {
	user->accesslist = GETOBJ(int,2);
	user->flags |= USER_ACCESSLIST;
    }
    /*  autocommand  */
    if (GETOBJ(int,3)) {
	setstring(&user->autocommand, GETOBJ(string,2));
        user->flags |= USER_AUTOCOMMAND;
    }
    /*  nopassword  */
    if (GETOBJ(int,4)) {
	user->flags |= USER_NOPASSWORD;
        user->password[0] = 0;
    }
    /*  nohangup  */
    if (GETOBJ(int,5)) {
	user->flags |= USER_NOESCAPE;
    }
    /*  password  */
    if (GETOBJ(int,6)) {
	char *pass = GETOBJ(string, 3);
	crypttype = GETOBJ(int,7);
	switch (crypttype) {
	case ENCRYPT_NONE:
	    len = PASSWORDLEN;
	    if (strlen(pass) > len) {
		printf("\n%% Overly long Password truncated "
		       "after %d characters", PASSWORDLEN);
		pass[len] = 0;
	    }
	    break;
	case ENCRYPT_TEMP:
	    /* Do nothing. */
	    break;
	default:
	    printf("Invalid encryption type: %d.  Password not set.\n",
		   crypttype);
	    user_error = TRUE;
	    break;
	}
	if (!user_error) {
	    user->flags &= ~USER_NOPASSWORD;
	    user->flags &= ~USER_ENCRYPTION;
	    user->flags |= crypttype;
	    sstrncpy(user->password, pass, sizeof(user->password));
	}
    }
    /*  nohangup  */
    if (GETOBJ(int,8)) {
	user->flags |= USER_NOHANGUP;
    }
    /*  privilege */
    if (GETOBJ(int,14)) {
	user->flags |= ((GETOBJ(int,15) & 0xf) << 28);
    } else {
	user->flags |= ((DEFAULT_PRIV & 0xf) << 28);
    }

    /* nocallback-verify */
    if (GETOBJ(int,9)) {
	user->flags |= USER_NOCALLBACK_VERIFY;
    }

    /*  callback-dialstring */
    if (GETOBJ(int,10)) {
	setstring(&user->dial_string, GETOBJ(string,4));
	user->flags |= USER_CALLBACK_DIALSTRING;
    }    
  
    /*  callback-rotary */
    if (GETOBJ(int,11)) {
	/*
	 * If callback-line also defined, illegal
	 */
	if (GETOBJ(int,12) || (user->flags & USER_CALLBACK_LINE)){
	    printf("\n%% callback-line already defined, rotary disallowed\n");
	    user_error = TRUE;
	} else {
	    user->callback_rotary_or_line = GETOBJ(int,13); 
	    user->flags |= USER_CALLBACK_ROTARY;
	}
    }    

    /*  callback-line */
    if (GETOBJ(int,12)) {
	/*
	 * If callback-rotary also defined, illegal
	 */
	if (GETOBJ(int,11) || (user->flags & USER_CALLBACK_ROTARY)){
	    printf("\n%% callback-rotary already defined, line disallowed\n");
	    user_error = TRUE;
	}

	/*
	 * Check for lines that support callback
	 */
	if ((MODEMS[GETOBJ(int,13)]->type != TTY_LINE) && 
		(MODEMS[GETOBJ(int,13)]->type != AUX_LINE)){
	    printf("\n%% callback-line type not supported\n");
	    user_error = TRUE;
	} else {
	    user->callback_rotary_or_line = GETOBJ(int,13); 
	    user->flags |= USER_CALLBACK_LINE;
	}
    }    

    if (new_user) {
	if (user_error) {
	    free(user->autocommand);
	    free(user->dial_string);
	    free(user);
	} else {
	    enqueue(&userQ, user);
	}
    }
}

/*
 * nv_password
 * Do NV generation of password
 * WARNING!!!  Do not pass in a pointer to the TTY if you are not
 * calling nv_password() for line passwords!  Pass in NULL instead!
 */
void nv_password (parseinfo *csb, char *prefix, int level, tt_soc *tty,
		  password_struct *password, char *temppassword, uint len)
{
    const char *pw;
    tt_soc *prevtty;
    password_struct *tmppwdstruct;
    int encrypt_type;

    /* Check for a password */
    if (!password || !password->pw || !password->pw[0]) {
	return;
    }

    if (csb->flags & HIDE_PASSWORDS) {
	pw = techsupport_blank_string;
    } else {
	pw = password->pw;
    }
    encrypt_type = password->encrypt_type;

    nv_write(TRUE, "%s", prefix);
    nv_add(level != PRIV_MAX, " level %d", level);

    /* We're not doing encryption, nor has the password been
     * encrypted, so leave it as plain-text.
     */
    switch(encrypt_type) {
	case ENCRYPT_NONE:
	    if (!password_encryption_enable) {
		nv_add(TRUE, " %s", pw);
		return;
	    } else {
		/* Encryption has been enabled, but the password is
		 * still in plain-text.  We need to encrypt it before
		 * writing it out.
		 */
		bzero(temppassword, len);
		if (tty) {
		    /* We're dealing with line passwords.  Check if the
		     * password is the same as the previous line.  If so,
		     * point this line's pasword_struct pointer to the
		     * previous line's password struct and memlock it.
		     */
		    if (csb->line1 != 0) {
			prevtty = MODEMS[(int)csb->line1-1];
			if (prevtty->password && prevtty->password->pw) {
			    decrypt(prevtty->password->pw, temppassword, len);
			} else {
			    prevtty = NULL;
			}
		    } else {
			prevtty = NULL;
		    }
		    if (!strcmp(temppassword, password->pw) && prevtty) {
			/* Password is the same as previous password */
			/* bundle our pointers and memlock it */
			tty->password = prevtty->password;
			mem_lock(tty->password);
			mem_lock(tty->password->pw);
		    } else {
			/* Password is different from previous password */
			/* Allocate a new structure */
			tmppwdstruct = malloc(sizeof(password_struct));
			if (!tmppwdstruct) {
			    break;
			}
			tmppwdstruct->encrypt_type = encrypt(password->pw,
							     temppassword,
							     0, len);
			setstring(&tmppwdstruct->pw, temppassword);
			tty->password = tmppwdstruct;
		    }
		    bzero(temppassword, len);
		    if (!(csb->flags & HIDE_PASSWORDS)) {
			pw = tty->password->pw;
		    }
		    encrypt_type = tty->password->encrypt_type;
		    free(password->pw);
		    free(password);
		} else {
		    /* This case handles all the non-line passwords. */
		    password->encrypt_type = encrypt(password->pw, temppassword,
						     0, len);
		    encrypt_type = password->encrypt_type;
		    setstring(&password->pw, temppassword);
		    bzero(temppassword, len);
		    if (!(csb->flags & HIDE_PASSWORDS)) {
			pw = password->pw;
		    }
		}
	    }
	    break;
	default:
	    break;
    }
    nv_add(TRUE, " %d %s", encrypt_type, pw);
}

/*
 * configure_enable
 * Determine if this is setting a password or the use of tacacs.
 *
 *    [no] enable password [0-7] [password]
 *    [no] enable use-tacacs
 *    [no] enable last-resort [password | succeed]
 */

void configure_enable (parseinfo *csb)
{
    char *pass;
    char temppassword[PASSWORDLEN_ENCRYPTED+1];
    uint level;
    int type;
    static sys_timestamp last_warned_time;

    TIMER_STOP(last_warned_time);
    if (csb->nvgen) {
	switch (csb->which) {
	case ENABLE_SECRET:
	    for (level=PRIV_MIN+1; level <= PRIV_MAX; level++) {
		nv_password(csb, csb->nv_command, level, NULL, 
			    &enable_secret[level], temppassword,
			    PASSWORDLEN_ENCRYPTED);
	    }
	    break;
	case ENABLE_PASSWORD:
	    for (level=PRIV_MIN+1; level <= PRIV_MAX; level++) {
		nv_password(csb, csb->nv_command, level, NULL, 
			    &enable_password[level], temppassword,
			    PASSWORDLEN_ENCRYPTED);
	    }
	    nv_write(TRUE, "!");
	    break;
/* OLD_AUTH -- remove this section when old authentication goes away */
	case ENABLE_LASTRESORT:
	    nv_write(GETOBJ(int,1) == enable_lastresort, "%s",
		     csb->nv_command);
	    break;
	case ENABLE_USETACACS:
	    nv_write(enable_use_login_server, "%s", csb->nv_command);
	    break;
/* end of OLD_AUTH section */
	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }
		
    switch (csb->which) {
    case ENABLE_PASSWORD:
	level = GETOBJ(int,2);
	type = GETOBJ(int,1);
	pass = GETOBJ(string,1);
	if (!csb->sense) {
	    free(enable_password[level].pw);
	    enable_password[level].pw = NULL;
	    break;
	}
	if (level == PRIV_MAX) {
	    switch (type) {
	    case ENCRYPT_NONE:
		if (strlen(pass) > PASSWORDLEN) {
		    printf("\n%% Overly long Password truncated "
			   "after %d characters", PASSWORDLEN);
		    pass[PASSWORDLEN] = 0;
		}
		/*
		 * if we are encrypting
		 * and we got a non encrypted password
		 * encrypt it.
		 */
		if (password_encryption_enable) {
		    memset(temppassword, 0, sizeof(temppassword));
		    type = encrypt(pass, temppassword, 0,
				   PASSWORDLEN_ENCRYPTED);
		    pass = temppassword;
		}
		break;
	    case ENCRYPT_TEMP:
		/* Do nothing. */
		break;
	    default:
		printf("Invalid encryption type: %d\n", type);
		return;
		break;
	    }
	    setstring(&enable_password[level].pw, pass);
	    enable_password[level].encrypt_type = type;
	    if ((csb->resolvemethod == RES_MANUAL) &&
		enable_secret[level].pw && enable_password[level].pw &&
		(!TIMER_RUNNING(last_warned_time) ||
		 CLOCK_OUTSIDE_INTERVAL(last_warned_time, 60*ONESEC)) &&
		md5_crypt_compare(enable_secret[level].pw,
				  enable_password[level].pw,
				  enable_password[level].encrypt_type)) {
		printf("\nThe enable password you have chosen is the same as "
		       "your enable secret.");
		printf("\nThis is not recommended.  Re-enter the enable "
		       "password.\n");
		GET_TIMESTAMP(last_warned_time);
	    } else {
		TIMER_STOP(last_warned_time);
	    }
	    if (parser_priv_debug) {
		printf("\nSetting password for level %d to '%s'",
		       level, pass);
	    }
	    break;
	}
	/* If not PRIV_MAX, then convert it to a secret and clear password. */
	if (enable_password[level].pw) {
	    free(enable_password[level].pw);
	    enable_password[level].pw = NULL;
	}
	if (csb->resolvemethod == RES_MANUAL)
	    printf("%% Converting to a secret.  Please use \"enable secret\" "
		   "in the future.\n");
	/* FALLTHRU */
    case ENABLE_SECRET:
	level = GETOBJ(int,2);
	if (csb->sense) {
	    type = GETOBJ(int,1);
	    pass = GETOBJ(string,1);

	    switch (type) {
	    case ENCRYPT_MD5:
		if (!md5_crypt_valid_secret(pass)) {
		    printf("\nERROR: The secret you entered is not a valid"
			   " encrypted secret."
			   "\nTo enter an UNENCRYPTED secret, do not"
			   " specify type %d encryption."
			   "\nWhen you properly enter an UNENCRYPTED secret,"
			   " it will be encrypted.\n", ENCRYPT_MD5);
		    return;
		}
		enable_secret[level].encrypt_type = type;
		setstring(&enable_secret[level].pw, pass);
		break;
	    case ENCRYPT_TEMP:
		decrypt(pass, temppassword, PASSWORDLEN);
		pass = temppassword;
		/* FALLTHRU */
	    case ENCRYPT_NONE:
		if (strlen(pass) > PASSWORDLEN) {
		    printf("\n%% Overly long Password truncated "
			   "after %d characters", PASSWORDLEN);
		    pass[PASSWORDLEN] = 0;
		}
		free(enable_secret[level].pw);
		enable_secret[level].pw =
		    malloc_named(CRYPT_SECRET_LEN,"Ion hash Block");
		if (enable_secret[level].pw) {
		    md5_crypt(pass, NULL, enable_secret[level].pw);
		    enable_secret[level].encrypt_type = ENCRYPT_MD5;
		}
		break;
	    default:
		printf("Invalid encryption type: %d\n", type);
		return;
		break;
	    }
	    if ((csb->resolvemethod == RES_MANUAL) &&
		enable_secret[level].pw && enable_password[level].pw &&
		(!TIMER_RUNNING(last_warned_time) ||
		 CLOCK_OUTSIDE_INTERVAL(last_warned_time, 60*ONESEC)) &&
		md5_crypt_compare(enable_secret[level].pw,
				  enable_password[level].pw,
				  enable_password[level].encrypt_type)) {
		printf("\nThe enable secret you have chosen is the same as"
		       " your enable password."
		       "\nThis is not recommended.  Re-enter the enable"
		       " secret.\n");
		GET_TIMESTAMP(last_warned_time);
	    } else {
		TIMER_STOP(last_warned_time);
	    }
	} else {
	    free(enable_secret[level].pw);
	    enable_secret[level].pw = NULL;
	    TIMER_STOP(last_warned_time);
	}
	break;
/* OLD_AUTH -- remove this section when old authentication goes away */
    case ENABLE_LASTRESORT:
	if (csb->sense) {
	    enable_lastresort = GETOBJ(int,1);
	} else {
	    enable_lastresort = TACACS_LAST_FAIL;
	}
	break;
    case ENABLE_USETACACS:
        if (!login_have_servers && (csb->resolvemethod == RES_MANUAL)) {
	    printf("\nenable use-tacacs warning: tacacs "
		   "server is not running.");
	}
	enable_use_login_server = csb->sense;
        reg_invoke_login_set_enable(csb);
	break;
/* end of OLD_AUTH section */
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * login_exec_command
 * The EXEC "login" command
 */

void login_exec_command (parseinfo *csb)
{
    int save_access;

    if (old_access_control) {
	if (login_have_servers || !QUEUEEMPTY(&userQ)) {
	    save_access = stdio->accessout;
	    stdio->accessout = stdio->accessout_default;
	    if (!login_askboth(stdio, FALSE, 1) && stdio->user) {
		stdio->accessout = save_access;
		printf("Still logged in as \"%s\"", stdio->user);
	    } else {
		csb->priv = CUR_PRIV(stdio->privilege);
		if (parser_priv_debug) {
		    printf("\nPrivilege level set to %d", csb->priv);
		}
	    }
	} else {
	    printf("%% No login server running.");
	}
    } else {
	userstruct *orig_u;
	orig_u = stdio->ustruct;
	stdio->ustruct = NULL;
	if (new_ascii_login(stdio, "default" , 1)) {
	    if (!do_exec_author(stdio, NULL)) {
		/*
		 * Authentication worked, but authorization failed.
		 * Restore old user context.
		 */
		aaa_free_user(&stdio->ustruct);
		stdio->ustruct = orig_u;
	    } else {
		/*
		 * Successfull new login.
		 * Fix up Accounting, since we've changed our userid.
		 * This is complicated and obscure, and is relegated to AAA
		 */
		reg_invoke_aaa_acct_relogin_fixup(stdio->ustruct, orig_u);
		aaa_free_user(&orig_u);		/* Free old user */
	    }
	} else {
	    /*
	     * Authentication failure.  Restore previous user info.
	     */
	    aaa_free_user(&stdio->ustruct);
	    stdio->ustruct = orig_u;
	}
    }
}

/*
 * setencryptpasswd
 * Put encrypt value in all ttys
 */

static void setencryptpasswd (parseinfo *csb, uint encrypt, char *passwd)
{
    password_struct *spassword = NULL;
    int i;

    if (passwd) {
	spassword = malloc(sizeof(password_struct));
	if (!spassword) {
	    return;
	}

	setstring(&spassword->pw, passwd);
	if (!spassword->pw) {
	    return;
	}
	spassword->encrypt_type = encrypt;
    }

    for (i = csb->line1; i < csb->line2; i++) {
	if (MODEMS[i]->password) {
	    free(MODEMS[i]->password->pw);
	    free(MODEMS[i]->password);
	}
	if (spassword) {
	    mem_lock(spassword->pw);
	    mem_lock(spassword);
	}
	MODEMS[i]->password = spassword;
    }
    if (spassword) {
	free(spassword->pw);
	free(spassword);
    }
}

/*
 * password_line_command
 * Put a password on a terminal line
 */

void password_line_command (parseinfo *csb)
{
    char temppassword[PASSWORDLEN_ENCRYPTED+1];
    char *pass;
    uint encrypt_type;
    tt_soc *tty = MODEMS[(int)csb->line1];

    if (csb->nvgen) {
	nv_password(csb, csb->nv_command, PRIV_MAX, tty, tty->password,
		    temppassword, PASSWORDLEN_ENCRYPTED);
	return;
    }

    if (csb->sense) {
	encrypt_type = GETOBJ(int,1);
	pass = GETOBJ(string,1);

	switch (encrypt_type) {
	case ENCRYPT_NONE:
	    if (strlen(pass) > PASSWORDLEN) {
		printf("\n%% Overly long Password truncated "
		       "after %d characters", PASSWORDLEN);
		pass[PASSWORDLEN] = 0;
	    }
	    /*
	     * if we are encrypting
	     * and we got a non encrypted password
	     * encrypt it.
	     */
	    if (password_encryption_enable) {
		memset(temppassword, 0, sizeof(temppassword));
		encrypt_type = encrypt(pass, temppassword, 0,
				       PASSWORDLEN_ENCRYPTED);
		pass = temppassword;
	    }
	    break;
	case ENCRYPT_TEMP:
	    /* do nothing */
	    break;
	default:
	    printf("Invalid encryption type: %d\n", encrypt_type);
	    return;
	    break;
	}

	setencryptpasswd(csb, encrypt_type, pass);
    } else {
	setencryptpasswd(csb, 0, NULL);
    }
}

/*
 * encrypt
 * Encrypt an otherwise unencrypted password before writing to nv memory.
 * return a number that represents the type of encryption used.
 */

static char *rndstring = "dsfd;kfoA,.iyewrkldJKDHSUBsgvca69834ncxv9873254k;fg87";

uint encrypt (char *input, char *output, char *seed, ulong maxlen)
{
    int key;
    int len;

    process_sleep_for(ONESEC/20);
    if (seed)
	key = (seed[0] - '0')*10 + (seed[1] - '0');
    else
	key = (15 & random_gen());

    seed = rndstring + key;
    len = sprintf(output, "%02d", key);
    while (*input) {
	len += sprintf(&output[len], "%02x", (uchar)(*input++ ^ *seed++));
	if (len >= maxlen-2)
	    break;
    }
    return(ENCRYPT_TEMP);
}

/*
 * decrypt
 * Decrypt an encrypted password .  Used by the CHAP software since
 * it needs the unencrypted secrets for MD5 computations.  Only works
 * on ciscos type 7 encryption.
 */

uint decrypt (char *input, char *output, ulong maxlen)
{
    int key;
    int len;
    char *seed;
    char temp[5];	/* to convert ASCII to hex */
    boolean parsed;

    key = (*input++ - '0') * 10;
    key += *input++ - '0';

    seed = rndstring + key;
    len = 0;
    temp[0] = '0';
    temp[1] = 'x';
    temp[4] = '\0';
    while (*input) {
        temp[2] = *input++;
        temp[3] = *input++;
	key = parse_unsigned(temp, &parsed);
	len += sprintf(&output[len], "%c", key ^ *seed++);
	if (len >= maxlen-2)
	    break;
    }
    return(ENCRYPT_TEMP);
}

/*
 * passwdcheck
 * check to see whether a password is valid
 */

boolean passwdcheck (char *input, char *correct, uint crypttype)
{
    char pass[PASSWORDLEN_ENCRYPTED+1];


    if (strlen(input) > PASSWORDLEN)
	input[PASSWORDLEN] = 0; /* Truncate if too long */

    switch(crypttype) {
      case ENCRYPT_NONE:
	return(strcmp(input, correct) == 0);
      case ENCRYPT_TEMP:
	encrypt(input, pass, correct, PASSWORDLEN_ENCRYPTED);
	return(strcmp(pass, correct) == 0);
      case ENCRYPT_UNIX:
	return(FALSE);
      default:
	buginf("\n%% Password check with invalid encryption type");
	return(FALSE);
    }
}

#define EXEC_INVALID_ATTR -1
/*
 * do_exec_author()
 */
boolean
do_exec_author (tt_soc *tty, boolean *use_def_ac)
{
    authorstruct author;
    int i, avail = 0;
    boolean ret = TRUE;
    char *attr, *val;
    enum AV_TYPE type;
    char *tmp_autocmd = (char *)EXEC_INVALID_ATTR;
    int tmp_priv = EXEC_INVALID_ATTR, tmp_acl = EXEC_INVALID_ATTR;
    int tmp_timeout = EXEC_INVALID_ATTR, tmp_idletime = EXEC_INVALID_ATTR;
    boolean tmp_noescape = FALSE, tmp_nohangup = FALSE;
    usernametype callback_info;
    acl_headertype *aclp;

    if (!aaa_author_needed(AAA_AUTHOR_TYPE_SHELL, 0, NULL, tty))
	return(TRUE);

    memset(&author, 0, sizeof(authorstruct));
    memset(&callback_info, 0, sizeof(usernametype));
    author.auth_idb = tty->tty_idb;
    author.auth_tty = tty;

    /* First step.  Build outgoing A-V lists. */
    if (!aaa_add_AV(&author.out_args, &avail, AAA_ATTR_service,
		    AAA_VAL_SVC_shell, AAA_AV_MAND) ||
	!aaa_add_AV(&author.out_args, &avail, AAA_ATTR_cmd, AAA_VAL_null,
		    AAA_AV_OPT))
	ret = FALSE;
    if (ret) {
	if (tty->tty_autocommand) {
	    if (!aaa_add_AV(&author.out_args, &avail, AAA_ATTR_autocmd,
			    tty->tty_autocommand, AAA_AV_OPT))
		ret = FALSE;
	}
	else if (tty->tty_def_autocommand) {
	    if (!aaa_add_AV(&author.out_args, &avail, AAA_ATTR_autocmd,
			    tty->tty_def_autocommand, AAA_AV_OPT))
		ret = FALSE;
	}
    }

    if (ret) {
	/* Second step.  Ask. */
	ret = aaa_do_author(tty->ustruct, &author,
			    AAA_AUTHOR_TYPE_SHELL, 0);

	/* Third step.  Deal with response. */
	switch(author.status) {
	case AAA_AUTHOR_STATUS_ERROR:
	case AAA_AUTHOR_STATUS_FAIL:
	    ret = FALSE;
	    break;
	case AAA_AUTHOR_STATUS_PASS_ADD:
	    if (!author.in_args)
		break;
	    aaa_merge_attrs(author.out_args, author.in_args, &author.in_args);
	    /* FALLTHRU */
	case AAA_AUTHOR_STATUS_PASS_REPL:
	    if (!author.in_args)
		break;
	    for (i = 0; ret && author.in_args[i]; i++) {
		if (aaa_author_debug) {
		    buginf("\nAAA/AUTHOR/EXEC: Processing AV %s",
			   author.in_args[i]);
		}
		if (aaa_split_AV(author.in_args[i], &attr, &val, &type)) {
		    if (!strcmp(attr, AAA_ATTR_service)) {
			if (strcmp(val, AAA_VAL_SVC_shell)) {
			    if (type == AAA_AV_MAND)
				ret = FALSE;
			    else
				if (aaa_author_debug)
				    buginf("\nAAA/AUTHOR/EXEC: "
					   "tried to change \"service\".  "
					   "Ignoring.");
			}
			continue;
		    }
		    if (!strcmp(attr, AAA_ATTR_autocmd)) {
			if (*val == '\0') {
			    tmp_autocmd = NULL;
			} else {
			    tmp_autocmd = string_getnext();
			    sstrncpy(tmp_autocmd, val,
				     (80 < (strlen(val)+1)) ?
				      80 : (strlen(val)+1));
			}
			continue;
		    }
		    if (!strcmp(attr, AAA_ATTR_priv_lvl)) {
			tmp_priv = atoi(val);
			if ((tmp_priv < PRIV_MIN) || (tmp_priv > PRIV_MAX)) {
			    if (aaa_author_debug)
				buginf("\nAAA/AUTHOR/EXEC: received invalid "
				       "privilege level %d. (Should be %d-%d)",
				       tmp_priv, PRIV_MIN, PRIV_MAX);
			    if (type == AAA_AV_MAND)
				ret = FALSE;
			    tmp_priv = EXEC_INVALID_ATTR;
			}
			continue;
		    }
		    if (!strcmp(attr, AAA_ATTR_noescape) &&
			!strcmp(val, AAA_VAL_true)) {
			tmp_noescape = TRUE;
			continue;
		    }
		    if (!strcmp(attr, AAA_ATTR_nohangup) &&
			!strcmp(val, AAA_VAL_true)) {
			tmp_nohangup = TRUE;
			continue;
		    }
		    if (!strcmp(attr, AAA_ATTR_acl)) {
			tmp_acl = atoi(val);
			if ((tmp_acl < MINFASTACC) || (tmp_acl > MAXSLOWACC)) {
			    if (aaa_author_debug)
				buginf("\nAAA/AUTHOR/EXEC: received invalid "
				       "access-class value %d. "
				       "(Should be %d - %d)", tmp_acl,
				       MINFASTACC, MAXSLOWACC);
			    if (type == AAA_AV_MAND)
				ret = FALSE;
			    tmp_acl = EXEC_INVALID_ATTR;
			}

			/* check that the acl actually exists.
			 * If it is mandatory, then it MUST exist
			 */
			aclp = acl_array[tmp_acl];
			if ( ! (aclp && aclp->list) &&
			     (type == AAA_AV_MAND) ) {
			    if (aaa_author_debug) {
				buginf("\nAAA/AUTHOR/EXEC: acl %s does "
				       "not exist.", val);
			    }
			    ret = FALSE;
			}
			
			continue;
		    }


                    if (!strcmp(attr, AAA_ATTR_callback)) {
 			setstring(&callback_info.dial_string, val);
                        callback_info.flags |= USER_CALLBACK_DIALSTRING;
 			continue;
 		    }
 
                    if (!strcmp(attr, AAA_ATTR_callback_noverify)) { 
                        if (atoi(val) == 1) 
 			   callback_info.flags |= USER_NOCALLBACK_VERIFY;
 			continue;
 		    }
 
                    if (!strcmp(attr, AAA_ATTR_callback_rotary)) {
                         callback_info.callback_rotary_or_line = atoi(val);
 			 callback_info.flags |= USER_CALLBACK_ROTARY;
 			 continue;
 		    } 

                    if (!strcmp(attr, AAA_ATTR_callback_line)) {
                         callback_info.callback_rotary_or_line = atoi(val);
 			 callback_info.flags |= USER_CALLBACK_LINE;
 			 continue;
 		    }
 
		    if (!strcmp(attr, AAA_ATTR_timeout)) {
			tmp_timeout = atoi(val);
			if (tmp_timeout < 0) {
			    if (aaa_author_debug)
				buginf("\nAAA/AUTHOR/EXEC: "
				       "Bad timeout value: \"%s\"", val);
			    if (type == AAA_AV_MAND)
				ret = FALSE;
			}
			continue;
		    }
		    if (!strcmp(attr, AAA_ATTR_idletime)) {
			tmp_idletime = atoi(val);
			if (tmp_idletime < 0) {
			    if (aaa_author_debug)
				buginf("\nAAA/AUTHOR/EXEC: "
				       "Bad idletime value: \"%s\"", val);
			    if (type == AAA_AV_MAND)
				ret = FALSE;
			}
			continue;
		    }

		    /* add in processing for:
		     *	??? (new attrs)
		     */
		    /* if unknown and mandatory, then bail */
		    if (type == AAA_AV_MAND) {
			if (aaa_author_debug)
			    buginf("\nAAA/AUTHOR/EXEC: received unknown "
				   "mandatory AV: %s", author.in_args[i]);
			ret = FALSE;
		    }
		}
	    }
	    break;
	}
    }

    if (ret) {
	if (tmp_autocmd != (char *)EXEC_INVALID_ATTR) {
	    tty->tty_autocommand = tmp_autocmd;
	    if ((tmp_autocmd == NULL) && use_def_ac)
		*use_def_ac = FALSE;
	}
	if (tmp_priv != EXEC_INVALID_ATTR) {
	    SET_PRIV(tty->privilege, tmp_priv);
	    if (tty->csb) {
	      tty->csb->priv = tmp_priv;
	    }
	}
	if (tmp_noescape) {
	    tty_savedefaults(tty);
	    tty->tty_escapechar = -1;
	}
	if (tmp_nohangup)
	    tty->acflags |= ACF_RELOGIN;

	tty->accessout = (tmp_acl == EXEC_INVALID_ATTR) ?
	    tty->accessout_default : tmp_acl;

	if (tmp_timeout >= 0) {
	    tty_savedefaults(tty);
	    if (!TIMER_RUNNING(tty->time_activated)) {
		GET_TIMESTAMP(tty->time_activated);
	    }
	    if (tmp_timeout) {
		tty->tty_absolutetimeout = ELAPSED_TIME(tty->time_activated) +
		    (tmp_timeout * ONEMIN);
	    } else {
		tty->tty_absolutetimeout = 0;
	    }
	}
	if (tmp_idletime >= 0) {
	    tty_savedefaults(tty);
	    tty->tty_exectimeout = tty->tty_sessiontimeout =
		(tmp_idletime * ONEMIN);
	    tty->capabilities |= OUTIDLE;
	}
        /*
         * Initiate callback if necessary 
         * If there is Callback to the caller, initiate it here
         * If return is FALSE
         *    callback is disabled, user does not have callback or registry 
         *    is not registered.
         *    login proceeds as if no callback were in effect. 
         * else if return is TRUE (control actually is not returned)
         *    Callback is in effect or the selected line for callback is 
         *    unavailable. In either case the line is disconnected for
         *    code consistency.
         */
        if (reg_invoke_callback_initiate(&callback_info, tty, 
                                                ACF_CALLBACK_EXEC, FALSE))
           death(tty);

	if (author.server_msg)
	    printf("%s\n", author.server_msg);
    } else {
	printf("%s\n", author.server_msg ?
	       author.server_msg : author_failed_msg);
    }

    aaa_cleanup_author(&author);

    if (aaa_author_debug) {
      buginf("\nAAA/AUTHOR/EXEC: Authorization %s",
             ret ? "successful" : "FAILED");
    }


    return(ret);
}
