/* $Id: command2.c,v 3.10.4.9 1996/08/28 13:18:36 thille Exp $
 * $Source: /release/112/cvs/Xsys/ui/command2.c,v $
 *------------------------------------------------------------------
 * command2.c  -- privileged commands for software monitoring and configuring
 *  
 * June 1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: command2.c,v $
 * Revision 3.10.4.9  1996/08/28  13:18:36  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.10.4.8  1996/07/30  01:41:59  billw
 * CSCdi59105:  AAA authenthenication errors during continue login cause
 * failure
 * Branch: California_branch
 *
 * Revision 3.10.4.7  1996/07/23  13:30:11  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.10.4.6  1996/06/17  08:36:24  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.10.4.5  1996/06/05  15:05:56  widmer
 * CSCdi57346:  Add printf style formatting to readline prompt
 * Branch: California_branch
 *
 * Revision 3.10.4.4  1996/05/21  10:07:38  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.10.4.3  1996/05/09  14:47:50  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.10.4.2.6.2  1996/05/02  20:29:33  rlowe
 * Fix a sync bug regarding telnet enable password for Cat5k ATM line card.
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.10.4.2.6.1  1996/04/27  07:37:51  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.10.4.2  1996/04/15  15:00:35  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.10.4.1.8.1  1996/04/08  02:10:45  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.10.4.1  1996/03/18  22:28:48  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.10.6.3  1996/04/04  02:34:41  rlowe
 * Reviewer suggested making auxiliary function an inline.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.10.6.2  1996/03/22  23:04:15  rlowe
 * Use cpu_type global variable instead of platform_get_value function
 * call.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.10.6.1  1996/03/22  09:44:34  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.10  1996/03/09  22:57:56  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.9.8.1  1996/03/03  21:25:09  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.9.6.1  1996/03/02  01:21:26  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.9  1996/02/08  20:13:36  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.8  1996/02/07  16:16:37  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.7  1996/01/24  20:42:30  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.6.18.1  1996/02/29  07:50:30  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.6  1995/12/17  18:40:11  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.5  1995/12/10  21:13:22  widmer
 * CSCdi45105:  Running-configs line exec-timeout and history change at
 * login
 * Add argument to readline() to specify whether command should be added
 * to history, change existing readline() calls to use TRUE, change
 * askstring() to call readline() with FALSE.
 *
 * Revision 3.4  1995/11/18  01:56:52  ahh
 * CSCdi42918:  spurious access in write_memory
 * Make the informational message more general.
 *
 * Revision 3.3  1995/11/17  17:47:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:45:56  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:40:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  21:33:45  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6.36.3.2.1  1996/02/03  07:08:47  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.6.28.1.2.1  1996/01/19  21:57:31  sgarcia
 * CSCdi47203:  sync 11.1 catchup branch to 11.0 ATMFLASH_V11_951214 tag
 * of Synalc2_
 * Branch: Synalc_catchup_branch
 * Patch 11.0 Synalc2_LE2_V110_1_0_3_branch diffs between tags
 * Synalc2_LE2_V110_1_0_3_arizona_arkansas_split_951103 and
 * ATMFLASH_V11_951214
 * into 11.1 branch.
 *
 * Revision 2.6.28.2  1995/11/07  01:16:07  bbenson
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Caught a K&Rism...UI_CheckStartupTTY was defined without a void parm.
 *
 * Revision 2.6.28.1  1995/10/26  04:01:37  rlowe
 * Fix to allow enable command during NMPs VTY session to Synergy ATM line
 * card even if no enable password is configured.
 * CSCdi39518:  Default enable password cannot be changed
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.6  1995/07/22  06:05:29  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Fix over-zealousness in limiting output of "Building Configuration..."
 *
 * Revision 2.5  1995/07/14  07:03:31  hampton
 * Convert Terminal Server to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi37093]
 *
 * Revision 2.4  1995/07/13 06:45:51  billw
 * CSCdi36954:  AAA accounting done improperly when exec login command is
 * used
 *
 * aaa_dup_user no longer duplicates acctQ.  New registry point for
 * changing from one username to another.
 *
 * Revision 2.3  1995/06/20  07:19:40  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/14  19:23:04  vandys
 * CSCdi35265:  Should use intfc description if no line description
 * Don't reference tty->location directly; pass tt_soc to a
 * function, let it take from either tt_soc or IDB.
 *
 * Revision 2.1  1995/06/07  23:10:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "../../boot/cpu.h"
#include <string.h>
#include <ctype.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "interface_private.h"
#include "../h/auth_registry.regh"
#include "packet.h"
#include "ttysrv.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "file.h"
#include "command.h"
#include "name.h"
#include "../parser/parser_actions.h"
#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_breakset.h"
#include "../parser/parser_defs_config.h"
#include "../parser/parser_defs_autoselect.h"
#include "../parser/parser_defs_flash.h"
#include "../os/texti.h"
#include "../os/async.h"
#include "login.h"
#include "../if/network.h"
#include "connect.h"
#include "../os/nv.h"
#include "../parser/msg_parser.c"	/* Not a typo, see logger.h */
#include "../ip/ip_registry.h"
#include "aaa.h"
#include "../util/md5_crypt.h"
#include "../snmp/config_history.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "common_strings.h"
#include "../tcp/http.h"

static ulong config_dirty = FALSE;
uchar *write_terminal_nv_fallback = NULL;


/*
 * UI_CheckStartupTTY
 *	For most platforms, verify that current TTY is the startup TTY.
 *	This is for checking when no enable password is configured, and
 *	we want to limit enable commands to the startup TTY.
 *
 *	For Synergy (SYNALC) all our VTYs are connected to the NMP, so
 *	always allow enable command if no enable password configured.
 */
static inline boolean UI_CheckStartupTTY (void)
{
	if (cpu_type == CPU_SYNALC) {
		return (TRUE) ;
	} else {
		return (stdio->ttynum == startup_ttynum) ;
	}
}

/*
 * enable_command
 */

void enable_command (parseinfo *csb)
{
    char *reason;
    boolean userresp;
    char user[100];
    uint level = GETOBJ(int,1);
    boolean password_good = FALSE;

    /*
     * If this was really the disable command, turn off wizardliness
     * and return.
     */
    if (csb->which == CMD_DISABLE) {
	if (level <= csb->priv) {
	    csb->priv = level;
	    SET_PRIV(stdio->privilege, level);
	    if (parser_priv_debug) {
		printf("\nPrivilege level set to %d",
		       CUR_PRIV(stdio->privilege));
	    }
	} else {
	    printf("\nNew privilege level must be "
		   "less than current privilege level");
	}
	return;
    }

    /*
     * If already godlike, quit
     */
    if (CUR_PRIV(stdio->privilege) >= level) {
	SET_PRIV(stdio->privilege, level);
	csb->priv = level;
	if (parser_priv_debug) {
	    printf("\nPrivilege level set to %d", CUR_PRIV(stdio->privilege));
	}
	return;
    }

    /*
     * Check if we're using TACACS or something similar for authentication
     */
    if (old_access_control) {
	if (enable_use_login_server) {
	    if (!login_have_servers) {
		printf("%% No authentication server running");
		return;
	    }
	    do {
		userresp = askstring(stdio, user, sizeof user, "Username: ",
				     30 * ONESEC, 1, FALSE);
	    } while (userresp && null(user));
	    if (stdio->statbits & (IDLETIMEOUT|CARDROP|HANGUP))
		return;
	    if (!userresp)
		return;
	    reason = NULL;
	    if (reg_invoke_login_enable(stdio, user, &reason, level)) {
		password_good = TRUE;
	    }
	    if (reason != NULL)
		printf("%% %s\n", reason);
	} else { /* !enable_use_login_server */
  
	    /*
	     * If there is an enable secret, we always us that.  Otherwise ...
	     *
	     * If there is no enable password configured, we will allow the
	     * enable command from the startup terminal. If the startup
	     * terminal is the console terminal, we can use the console
	     * terminal password as the enable password. This is a hack for
	     * compatibility with old configurations which use a console
	     * password and no enable password.
	     *
	     * If there is no enable password configured for SYNALC platform
	     * always allow the enable command, even from VTYs.
	     */
	    if ((level == PRIV_MAX) && (enable_secret[PRIV_MAX].pw == NULL) &&
		(enable_password[PRIV_MAX].pw == NULL)) {
		if ((UI_CheckStartupTTY()) || /* Startup terminal */
		    askpassword(stdio,
				console->password ?
				console->password->pw : NULL,
				console->password ?
				console->password->encrypt_type :
				ENCRYPT_NONE)) {
		    password_good = TRUE;
		}
	    } else {
		if (enable_secret[level].pw) {
		    if (ask_md5secret(stdio, enable_secret[level].pw))
			password_good = TRUE;
		} else {
		    if (askpassword(stdio, enable_password[level].pw,
				    enable_password[level].encrypt_type))
			password_good = TRUE;
		}
	    }
	} /* !enable_use_login_server */
    } else {	/* ! old_access_control */
	userstruct *ustruct;
	authenstruct authen;
	boolean ret;
	char buf[512], port[8];

	memset(&authen, 0, sizeof(authen));
	authen.tty = stdio;
	sprintf(&port[0], "tty%d", stdio->ttynum);

	if (aaa_get_cred(stdio->ustruct, NULL, NULL, NULL, NULL, NULL)) {
	    ret = aaa_dup_user(&stdio->ustruct, &ustruct, AAA_SVC_ENABLE,
			       level);
	} else {
	    char *rem = tty_get_remote_addr(stdio);
	    ret = aaa_create_user(&ustruct, NULL, NULL, port, rem,
				  AAA_AUTHEN_TYPE_ASCII, AAA_SVC_ENABLE,
				  level);
	}
	if (!ret) {
	    buginf("%%AAA: enable: Internal error.");
	    aaa_free_user(&ustruct);
	    return;
	}
	ret = aaa_start_login(ustruct, &authen, ACC_LIST_DEFAULT,
			      AAA_AUTHEN_ACT_LOGIN, AAA_SVC_ENABLE);
	/* Now if we can't find any authen mechanism, then fail right away. */
	if (!ret) {
	    authen.status = AAA_AUTHEN_STATUS_FAIL;
	}
	while ((authen.status == AAA_AUTHEN_STATUS_GETDATA) ||
	       (authen.status == AAA_AUTHEN_STATUS_GETUSER) ||
	       (authen.status == AAA_AUTHEN_STATUS_GETPASS)) {

	    ret = askstring(stdio, buf, sizeof(buf), authen.server_msg,
			    30*ONESEC, 1,
			    (authen.flags & AAA_RESP_FLAG_NOECHO));
	    if (!ret) {
		authen.status = AAA_AUTHEN_STATUS_ERROR;
		if (authen.server_msg) {
		    *authen.server_msg = '\0';
		}
		continue;
	    }
	    authen.user_reply = &buf[0];
	    aaa_cont_login(ustruct, &authen);
	    memset(&buf[0], 0, sizeof(buf));
	    authen.user_reply = NULL;
	}
	switch (authen.status) {
	case AAA_AUTHEN_STATUS_PASS:
	    if (authen.server_msg && *authen.server_msg)
		printf("%s\n", authen.server_msg);
	    password_good = TRUE;
	    break;
	case AAA_AUTHEN_STATUS_ERROR:
	    if (authen.server_data_len && authen.server_data &&
		isprint(*authen.server_data)) {
		char *str = malloc(authen.server_data_len + 1);
		if (str) {
		    sstrncpy(str, authen.server_data,
			     authen.server_data_len+1);
		    buginf("\n%% Enable: TACACS+ Server msg: %s", str);
		    free(str);
		}
	    }
	    /* FALLTHRU */
	default:
	    if (authen.server_msg && *authen.server_msg)
		printf("\n%s\n", authen.server_msg);
	    else
		printf("\n%% Error in authentication.\n");
	    break;
	}
	aaa_cleanup_login(ustruct, &authen);
	aaa_free_user(&ustruct);
    }

    /*
     * If we succeeded in enabling, send a notification
     */
    if (password_good) {
/* DAVE -- add proper accounting here */
	(void) reg_invoke_login_enable_note(stdio, level);
	SET_PRIV(stdio->privilege, level);
	csb->priv = level;
	if (parser_priv_debug) {
	    printf("\nPrivilege level set to %d", CUR_PRIV(stdio->privilege));
	}
    }
}

/*
 * sendmessage
 * Send a terminal message to a specified line, or all lines.
 * Send a message to all direct tty lines and any active modems or vty's.
 * Never send to a printer.
 */

void sendmessage (int dest)
{
    tt_soc *line;
    char last_char;
    int i, firstline, lastline;
    paktype *pak;
    int len;

    pak = getbuffer(MEDDATA);
    if (!pak) {
	return;
    }

    pak->datagramstart = pak->network_start;

    len = sprintf((char *)&pak->datagramstart[0],
		  "\n\n\n\07***\n***"
		  "\n*** Message from tty%t",stdio->ttynum);
    if (get_tty_location(stdio))
	len += sprintf((char *)&pak->datagramstart[len],
		       ", %s,", get_tty_location(stdio));
    if (dest == -1)
	len += sprintf((char *)&pak->datagramstart[len],
		       " to all terminals:\n***\n");
    else
	len += sprintf((char *)&pak->datagramstart[len],
		       " to tty%t:\n***\n", dest);


    printf("Enter message, end with CTRL/Z; abort with CTRL/C:\n");
    i = len;
    last_char = '\0';
    /* set the dataptr to point to the real message, so we can skip the 
	header garbage later if we want. */
    pak->transport_start = &pak->datagramstart[i];

    while ((last_char != '') &&
	   ((MEDDATA - (i+5)) > 0)) {
	if (!readline((char *)(&pak->datagramstart[i]), MEDDATA - (i+3),
		      "\r\n", &last_char, TRUE, "")) {
	    datagram_done(pak);
	    return;
	}
	i = strlen((char *)pak->datagramstart);
	pak->datagramstart[i++] = '\n';
	pak->datagramstart[i] = '\0';
    }
    /* set length to be the length of the actual message */
    pak->atalk_datalen = i - len;
    if (!confirm("\nSend message? ")) {
	datagram_done(pak);
	return;
    }

    len += strlen((char *)&pak->datagramstart[len]);
    len += sprintf((char *)&pak->datagramstart[len], "\n");
    pak->datagramsize = len;

    if (dest == -1) {
	firstline = 0; lastline = ALLlines;
    }
    else {
	firstline = dest; lastline = dest + 1;
    }

    for (i = firstline; i < lastline; ++i) {
        line = MODEMS[i]; 
	if ((line) && (line->statbits & READY))
	    tty_sendmessage(line, pak);
    }
    datagram_done(pak);
}

/*
 * askpassword
 * Ask for a password and check it
 */

boolean askpassword (tt_soc *tty, char *target, uint encryption)
{
    int i;
    char buff[80];

    if (target == NULL) {
	printf("%% No password set");
	return(FALSE);
    }
    for (i = 0; i < 3; i++) {
	if (askstring(tty, buff, sizeof buff, "Password: ", 30 * ONESEC, 1,
		      TRUE)) {
	    if (passwdcheck(buff, target, encryption))
		return(TRUE);
	}
	if (tty->autoselectmask & AUTOSELECTED) 
	    return(FALSE);
    }

    printf("\n%% Bad passwords\n");
    return(FALSE);
}

/*
 * askstring
 * Ask for a given string with prompt, timeout and retries
 */

boolean askstring (tt_soc *tty, char *buffer, int length, char *prompt,
		   int timeout, int retries, boolean noecho)
{
    uint savetimeout;
    boolean ok, savedecho;
    char dummy;

    savetimeout = tty->tty_exectimeout;
    tty->tty_exectimeout = timeout;
    savedecho = tty->tty_editinfo.noecho;
    if (noecho)
	tty->tty_editinfo.noecho = TRUE;
    ok = FALSE;
    buffer[0] = '\0';
    while (retries-- > 0) {
	if (!tty->conntop)
	    clearinput(tty);
	if (readline(buffer, length, "\r\n", &dummy, FALSE, "%s", prompt)) {
	    ok = TRUE;
	    break;
	} else if (tty->statbits & (IDLETIMEOUT|CARDROP|HANGUP)) {
	    printf("\n%% %s timeout expired!", prompt);
	    tty->statbits &= ~IDLETIMEOUT;  /* not whole line */
	    break;
	}
	if (tty->autoselectmask & AUTOSELECTED)
	    break;
    }
    tty->tty_exectimeout = savetimeout;
    TIMER_STOP(tty->dallytimer);	/* reset dally time */
    tty->tty_editinfo.noecho = savedecho;
    return(ok);
}

/*
 * terminate_line
 * Got a valid tty, terminate it.  Called by zap_line and snmp_clear_line. 
 * Note: if the line is a VTY_LINE running SLIPMODE, don't reset it.  Just
 * CARDROP and let the vty-async process do the line reset and termination.
 */
void terminate_line (tt_soc *target)
{
    if ((target->statbits & SLIPMODE) && (target->type != VTY_LINE)) {
	if (target->capabilities & MODEMMASK) {
	    target->statbits |= HANGUP;
	    target->statbits &= ~(READY + IDLE + CARDROP + RINGIN +
				  AUTOBAUDING + CALLBACK_IN_PROGRESS);
	} else {
	    tty_if_reset(target);  /* apply reset function */
	}
    } else if (target->statbits & HASPROC) {
	target->statbits |= CARDROP;
    } else {
	if (target->type != VTY_LINE)
	    tty_clearbuffers(target);
	death(target);
    }
}

/*
 * zap_line
 * Terminate another EXEC process
 */

void zap_line (parseinfo *csb)
{
    tt_soc *target;
    int line;

    line = GETOBJ(int,1);
    if (line >= ALLlines) {
	printf("%% %t is an illegal line number", line);
	return;
    }
    
    if (reg_invoke_invalid_line_specified(line, FALSE)) {
        printf("%% Line %d not available for clearing", line);
        return;
    }

    target = MODEMS[line];
    if (target == NULL)
	return;
    if (target == stdio) {
	printf("%% Not allowed to clear current line");
	return;
    }
    if (!confirm(NULL))
	return;
    terminate_line(target);
    printf(" [OK]");
}

/*
 * user_on_stdio_str()
 * Build a tempstr of the from "XXX on YYY". XXX is the user name
 * known through tacacs. YYY is the terminal name (stdio). If user name is
 * not available, only "YYY" is returned.
 * Warning : The return string is allocated from the tempstr array.
 *           Don't do any suspends between getting and using the return string.
 */
char *user_on_stdio_str (void)
{
    char *temp, *cp;
    int len = 0;

    /* prepare the "USER on TTY" string */
    temp = string_getnext();

    if (stdio->user) 
       len = sprintf(temp, "%s on ", stdio->user);
    if (stdio->type == VTY_LINE) {
	len += sprintf(&temp[len], "vty%t", stdio->ttynum - VTYBase);
	if (stdio->conntop)
	    cp = stdio->conntop->hostname[0] ? stdio->conntop->hostname : 
		name_addr2numeric(&stdio->conntop->address);
	else
	    cp = NULL;
    } else {
	if (stdio->ttynum == 0)
	    len += sprintf(&temp[len], "console");
	else
	    len += sprintf(&temp[len], "tty%t", stdio->ttynum);
	cp = get_tty_location(stdio);
    }
    if (cp && *cp) {
	int slen;
	
	slen = strlen(cp);
	if (slen > (sizeof(temp) - 1 - len - 3)) /* NUL + " ()" */
	    slen = (sizeof(temp) - 1 - len - 3);
	len += sprintf(&temp[len], " (%*s)", slen, cp);
    }
    return (temp);
}

/*
 * notify_config
 * Send messages (syslog, etc.) that a configure took place.
 */

void notify_config (char *mess)
{
    errmsg(&msgsym(CONFIG_I, SYS), mess, user_on_stdio_str());
}


/*
 * manual_configure
 * Query user for configuration file
 */

void manual_configure (parseinfo *csb)
{
    char buff[PARSEBUF];
    boolean direct_to_nv = FALSE;
    char *old_uri;

    if (!nv_valid)		/* then service config was default, stop now */
	netconfig_enable = FALSE;

    if (csb->which == PARSER_CONF_NONE) {
	printf("Configuring from terminal%s or network [terminal]? ",
	       nvsize ? ", memory," : NULL);
	rdtty(buff,sizeof(buff));
	if (null(buff) || partialmatch("terminal", buff)) {
	    csb->which = PARSER_CONF_TERM;
	} else if (partialmatch("memory", buff)) {
	    csb->which = PARSER_CONF_MEM;
	} else if (partialmatch("overwrite-network", buff)) {
	    csb->which = PARSER_CONF_OVERWRITE_NET;
	    SETOBJ(int,1) = COPY_TFTP;
	} else if (partialmatch("network", buff)) {
	    csb->which = PARSER_CONF_NET;
	    SETOBJ(int,1) = COPY_TFTP;
	} else {
	    printf("\n?Must be \"terminal\"%s or \"network\"",
		   nvsize ? ", \"memory\"" : NULL);
	    return;
	}
    }

    /*
     * If we are not doing a "conf term" ("conf net" or "conf mem"),
     * and we are not root, dont' let through
     */
    if ((csb->which != PARSER_CONF_TERM) &&
	(csb->priv != PRIV_ROOT)) {
	printf("\nInvalid privileges");
	return;
    }

    switch (csb->which) {

      case PARSER_CONF_TERM:
	reg_invoke_config_history_event(CONFIG_COMMAND_SOURCE_COMMANDLINE, 
					CONFIG_MEDIUM_COMMANDSOURCE, 
					CONFIG_MEDIUM_RUNNING, 
					0, NULL, NULL);
	csb->mode = config_mode;
	printf("\nEnter configuration commands, one per line.  "
	       "End with CNTL/Z.");
	if (!config_prompt) {
	    printc('\n');
	}
	csb->break_chars = CONFIG_BREAKSET;
	csb->batch_buf = NULL;
	csb->resolvemethod = RES_MANUAL;
	csb->permanent = TRUE;
	csb->flags = CONFIG_TERM;
	stdio->texti_info.history = &(stdio->texti_info.config_history);
	config_dirty = TRUE;
	return;

      case PARSER_CONF_HTTP:
	old_uri = csb->http_state->uri;

	/* Setup new URI */
	csb->http_state->uri = "/configure";

	/* Call parser and generate help for new URI */
	reg_invoke_http_parser(csb->http_state, config_mode, FALSE);

	/* Put back old URI */
	csb->http_state->uri = old_uri;
	return;

      case PARSER_CONF_MEM:
	reg_invoke_config_history_event(CONFIG_COMMAND_SOURCE_COMMANDLINE, 
					CONFIG_MEDIUM_STARTUP, 
					CONFIG_MEDIUM_RUNNING, 
					0, NULL, NULL);
	printc('\n');
	flush();
	nv_configure(CUR_PRIV(stdio->privilege));
	return;

      case PARSER_CONF_OVERWRITE_NET:
	direct_to_nv = TRUE;
	/* Drop through to next case */

      case PARSER_CONF_NET:


	/* 
	 * Obtain the network configuration information, load the
	 * configuration file, and parse it.
	 */

	/* config_history_event handled in config_read after getting more
	 * info from user
	 */

	if (GETOBJ(int,1) == COPY_TFTP)
            reg_invoke_config_read(FILE_ACCESS_TFTP, csb, direct_to_nv);
        else if (GETOBJ(int,1) == COPY_RCP)
            reg_invoke_config_read(FILE_ACCESS_RCP, csb, direct_to_nv);
	else
	    reg_invoke_config_read(FILE_ACCESS_UNKNOWN, csb, direct_to_nv);
	if (!direct_to_nv)
	    config_dirty = TRUE;
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * write_network
 * Write configuration information to a host on the network
 */

void write_network (parseinfo *csb)
{
    int file_access;

    if (GETOBJ(int,1) == COPY_TFTP)
	file_access = FILE_ACCESS_TFTP;
    else if (GETOBJ(int,1) == COPY_RCP)
	file_access = FILE_ACCESS_RCP;
    else
	file_access = FILE_ACCESS_UNKNOWN;

    /* config_history_event handled in config_write after getting more
     * info from user
     */
    reg_invoke_config_write(file_access, csb);
}

/*
 * write_terminal
 * Show the current configuration on the terminal
 */

void write_terminal (parseinfo *csb)
{
    char *curcfg;
    uint buffsize;
    uint nv_size;
    uint cursize;
    boolean fallback = FALSE;

    buffsize = nv_get_config_size();
    nv_size = buffsize;

    while (((curcfg = malloc(nv_size)) == NULL) && (nv_size > 0x3fff))
	nv_size >>= 1;

    /*
     * If we don't have a buffer to display the running configuration
     * in, do we have fallback space available?
     */
    if (!curcfg) {
        if (!write_terminal_nv_fallback)
            return;

        if (nv_check_nvptr())
            return;

        curcfg = write_terminal_nv_fallback;
        fallback = TRUE;
    }

    /*
     * Dump the current configuration to our buffer
     */
    cursize = nv_current_to_buffer(curcfg, nv_size, TRUE,
                             CUR_PRIV(stdio->privilege),
                             (csb->flags & HIDE_PASSWORDS));

    /*
     * If we're using the fallback buffer, lock nvram so that other
     * clients won't use it
     */
    if (fallback) {
        nv_set_nvptr(write_terminal_nv_fallback);    /* Lock it */
        if (!nvptr) {
            printf("\n%%Failed to lock write terminal buffer.");
            return;
        }
    }

    if (cursize > 0) {
	reg_invoke_config_history_event(CONFIG_COMMAND_SOURCE_COMMANDLINE, 
					CONFIG_MEDIUM_RUNNING, 
					CONFIG_MEDIUM_COMMANDSOURCE, 
					0, NULL, NULL);
        /*  Print out only as much as the size of the current config file */
        *(curcfg + cursize) = '\0';
	automore_enable(NULL);
	printf("\nCurrent configuration:");
	printf("%s", curcfg);
	automore_disable();
    }

    /*
     * If we used the fallback, free nvram. Else, free the buffer
     */
    if (fallback)
        nv_clear_nvptr();    /* Free lock */
    else
        free(curcfg);

}

/*
 * write_erase
 * Jacket for EXEC "write erase" command
 */

void write_erase (parseinfo *csb)
{
    reg_invoke_config_history_event(CONFIG_COMMAND_SOURCE_COMMANDLINE, 
				    CONFIG_MEDIUM_ERASE, 
				    CONFIG_MEDIUM_STARTUP, 
				    0, NULL, NULL);
    (void) nv_erase(TRUE);
}

/*
 * write_memory
 * Jacket for EXEC "write memory" command
 */

void write_memory (parseinfo *csb)
{
    if (ok_to_write_nv()) {
	reg_invoke_config_history_event(CONFIG_COMMAND_SOURCE_COMMANDLINE, 
					CONFIG_MEDIUM_RUNNING, 
					CONFIG_MEDIUM_STARTUP, 
					0, NULL, NULL);
	(void) nv_current_to_nv(TRUE);
	config_dirty = FALSE;
    } else
	printf("\nNo action taken because command was not confirmed");
}

/*
 * ping_command
 * Handle the various network level echoing protocols
 */

#define PING_CHARS 40

void ping_command (parseinfo *csb)
{
    char *args;
    boolean host_supplied, running;
    int index, hint;
    uchar buff[PING_CHARS];
    char *cp;

    /*
     * First see if something was specified on the command line.
     * If the parser did not recognize the hint, it returns LINK_ILLEGAL.
     */
    args = GETOBJ(string,1);
    hint = GETOBJ(int,1);
    host_supplied = (args && *args);
    if (host_supplied || (hint != LINK_ILLEGAL)) {
	/*
	 * SERVICE_PING_PARSE figures out what protocol and address may
	 * have been specified on the command line.  It returns true on
	 * the first positive match, false if no match at all.
	 */
	psion(stdio);
	if (!reg_invoke_ping_parse(args, hint,
			   &index, &cp, &running)) {
	    psioff(stdio);
	    printf("%% Unrecognized host or address, or protocol not running.\n");
	    return;
	}
	psioff(stdio);
    } else {

	/*
	 * No arguments supplied, so prompt the user.
	 * This time a negative hint indicates that "buff" should contain
	 * a protocol name instead of a protocol address.
	 */
	printf("Protocol [ip]: ");
	if (rdtty((char *)buff, PING_CHARS) == FALSE)
	    return;
	psion(stdio);
	if (!reg_invoke_ping_parse(buff, -1,
			   &index, &cp, &running)) {
	    psioff(stdio);
	    printf("%% Unknown protocol - \"%s\", type \"ping ?\" for help",
		   buff);
	    return;
	}
	psioff(stdio);
	args = "";
    }

    /*
     * We have figured out at least which protocol (in "index")
     * and if we have an address, it is in "args"
     */
    if (!running) {
	printf("%% %s protocol not running", cp);
	return;
    }
    reg_invoke_ping_run(index, args);
}

/*
 * ping_common
 * Common ping parameter parsing.
 * Returns TRUE if all successfully parsed, FALSE otherwise.
 */

boolean ping_common (long *repeat, long *size,
		     long minsize, long maxsize, long *seconds)
{
    *repeat = get_num_in_range("Repeat count", 10, 1, 0x7FFFFFFF, *repeat);
    *size = get_num_in_range("Datagram size", 10, minsize, maxsize, *size);
    *seconds = get_num_in_range("Timeout in seconds", 10, 0, 3600, *seconds);
    return(TRUE);
}

/*
 * ping_get_sweep
 * gets range of packets sizes to sweep.
 */
boolean ping_get_sweep (ulong *minn, ulong *maxx, ulong *intvl)
{
    char buff[PARSEBUF];
    boolean parsed;
    ulong sysmin, sysmax;
    
    sysmin = *minn;
    sysmax = *maxx;
    printf("\nSweep min size [%d]: ", *minn);
    parsed = rdtty(buff, PARSEBUF);
    if (parsed) {
	if (!null(buff))
	    *minn = parse_unsigned(buff, &parsed);
	parsed = parsed && (*minn > 0);
    }
    if (!parsed || (*minn < sysmin)) {
	printf("\n%% Bad minimum size");
	return(FALSE);
    }
    printf("\nSweep max size [%d]: ", *maxx);
    parsed = rdtty(buff, PARSEBUF);
    if (parsed) {
	if (!null(buff))
	    *maxx = parse_unsigned(buff, &parsed);
	parsed = parsed && (*maxx > 0);
    }
    if (!parsed || (*maxx > sysmax) || (*maxx <= *minn)) {
	printf("\n%% Bad maximum size");
	return(FALSE);
    }
    printf("\nSweep interval [%d]: ", *intvl);
    parsed = rdtty(buff, PARSEBUF);
    if (parsed) {
	if (!null(buff))
	    *intvl = parse_unsigned(buff, &parsed);
	parsed = parsed && (*intvl > 0);
    }
    if (!parsed || (*intvl == 0)) {
	printf("\n%% Bad interval size");
	return(FALSE);
    }
    return (TRUE);
}

/*
 * show_idbprotocols
 * Show all protocols running on a particular interface.
 */

static void show_idbprotocols (idbtype *idb)
{
    printf(if_is_line_proto_is_msg, idb->namestring,
	      print_idbstate(idb), interface_up(idb) ? "up" : "down");
    reg_invoke_showproto(idb);
}

/*
 * show_protocol
 * Display a list of all protocols running in the system, and their major
 * idb specific values.
 */

void show_protocol (parseinfo *csb)
{
    idbtype *target;

    target = GETOBJ(idb,1);
    automore_enable(NULL);
    if (target == NULL) {
	printf("\nGlobal values:");
	reg_invoke_showproto(NULL);
	automore_conditional(0);
    }
    show_swidb_chain(target, show_idbprotocols, 0);
    automore_disable();
}

/* 
 * cfg_dirty -- return status of config.  If user has modified
 * config and not saved then return true else return false
 */
static boolean cfg_dirty (void)
{
    return(config_dirty);
}

void nvram_config_dirty_init (void)
{
    reg_add_is_config_dirty(cfg_dirty,"cfg_dirty");
}
