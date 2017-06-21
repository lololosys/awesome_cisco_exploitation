/* $Id: exec.c,v 3.8.4.11 1996/09/06 21:01:15 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ui/exec.c,v $
 *------------------------------------------------------------------
 * exec.c  -- EXEC command processor
 *  
 * 30-June-1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec.c,v $
 * Revision 3.8.4.11  1996/09/06  21:01:15  snyder
 * CSCdi68334:  dump unused spinner support, save memory
 *              412 image, 4 data
 * Branch: California_branch
 *
 * Revision 3.8.4.10  1996/09/06  01:12:31  snyder
 * CSCdi68277:  make shr_core, shr_ukernel, sub_cxxxx sub_core_platform
 *              things const, save 712 out of data, 208 from image
 * Branch: California_branch
 *
 * Revision 3.8.4.9  1996/08/07  09:03:23  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.8.4.8  1996/07/13  02:26:23  widmer
 * CSCdi59942:  Privilege level not set to username when connecting to
 * router
 * Branch: California_branch
 *
 * Revision 3.8.4.7  1996/07/08  22:55:09  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.8.4.6  1996/06/05  15:05:59  widmer
 * CSCdi57346:  Add printf style formatting to readline prompt
 * Branch: California_branch
 *
 * Revision 3.8.4.5  1996/05/21  10:07:44  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8.4.4  1996/05/09  14:47:56  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.8.4.2.4.1  1996/04/27  07:37:56  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.8.4.3  1996/05/06  16:33:37  widmer
 * CSCdi55211:  ^C in config mode resumes connection
 * Branch: California_branch
 *
 * Revision 3.8.4.2  1996/04/19  15:37:28  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.8.4.1.8.1  1996/04/08  02:10:51  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.8.4.1  1996/03/18  22:29:03  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.6.1  1996/04/05  03:19:57  cakyol
 * started using the %Cx Cisco extensions to printf.  These are:
 *
 *   j: uchar*, atm RAW bytes.
 *
 *   h: hwaddrtype*, where type is either STATION_IEEE48 or
 *      STATION_AIPNSAP.
 *
 *   w: MaskedAddress*, where type is either STATION_IEEE48 or
 *      STATION_AIPNSAP.
 *
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.8  1996/03/09  22:57:59  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.7  1996/02/01  20:50:34  jenny
 * CSCdi47575:  crash when rsh command from host has wildcard
 *
 * Revision 3.6  1996/01/22  07:30:17  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1995/12/17  18:40:16  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.4  1995/12/10  21:13:26  widmer
 * CSCdi45105:  Running-configs line exec-timeout and history change at
 * login
 * Add argument to readline() to specify whether command should be added
 * to history, change existing readline() calls to use TRUE, change
 * askstring() to call readline() with FALSE.
 *
 * Revision 3.3  1995/12/03  18:00:28  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.2  1995/11/17  17:47:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:40:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.11  1995/11/08  21:33:48  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.10  1995/09/25  19:06:14  dkatz
 * CSCdi40898:  Synchronized reload needed
 * Code review nits.
 *
 * Revision 2.9  1995/09/24  00:54:58  dkatz
 * CSCdi40898:  Synchronized reload needed
 *
 * Revision 2.8  1995/09/22  04:36:52  enf
 * CSCdi40869:  Add frankenstorm to 11.0
 *
 * Revision 2.7  1995/08/03  19:18:38  irfan
 * CSCdi38178:  memory leak when using autoselect during-login
 * When allocating new CSB blocks, clean up tty's last CSB block
 * if not cleaned earlier.
 *
 * Revision 2.6  1995/07/04  01:59:20  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.5  1995/06/20  07:19:44  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.4  1995/06/15  22:58:02  syiu
 * CSCdi35686:  Async callback AAA authorization and accounting phase not
 * working
 *
 * Revision 2.3  1995/06/14  19:23:06  vandys
 * CSCdi35265:  Should use intfc description if no line description
 * Don't reference tty->location directly; pass tt_soc to a
 * function, let it take from either tt_soc or IDB.
 *
 * Revision 2.2  1995/06/09  13:22:18  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:11:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "config_register.h"
#include "sys_registry.h"
#include "../h/cs_registry.regh"
#include "interface_private.h"
#include "../dev/flash_registry.h"
#include "ttysrv.h"
#include "connect.h"
#include "logger.h"
#include "command.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../os/texti.h"
#include "../os/async.h"
#include "exec_parser.h"
#include "../dev/monitor1.h"
#include "setup.h"
#include "../if/network.h"
#include "login.h"
#include "aaa.h"
#include "../ui/exec.h"
#include "../os/region.h"
#include "../os/hostname.h"
#include "../h/auth_registry.regh"
#include "../ui/parser_defs_reload.h"
#include "../os/clock.h"
#include "../os/reload.h"
#include "file.h"


/*
 * help_command
 * Print a list of command help strings when "help" is typed.
 */

void help_command (char *const strings[], 
                   const ushort flags[], 
                   const int stringlen)
{
    int i;

    automore_enable(NULL);
    for (i = 0; i < stringlen; i++) {
	if ((!(CUR_PRIV(stdio->privilege) > PRIV_USER)) &&
	    ((flags[i] & CMD_PRV) || (flags[i] & CMD_HPRV)))
	    continue;
	printf("\n%s",strings[i]);
    }
    automore_disable();
}

/* BuildPrompt
 * Build promt at csb->prompt.
 *   This routine is global as it is used by SNA network management
 */
void BuildPrompt (parseinfo *csb, tt_soc *tty)
{
    int prompt_length;		/* leftover length of prompt */

    /* Build the prompt. */
    if (config_prompt || (csb->mode == exec_mode) ||
	!(csb->flags & CONFIG_TERM)) {
	const char *src;
	char *dst, trailer;
	
	trailer = (CUR_PRIV(tty->privilege) > PRIV_USER) ? '#' : '>';
	dst = csb->prompt;
	src = ((CUR_PRIV(tty->privilege) > PRIV_USER) ? Pprompt : Eprompt);
	prompt_length = MAXPROMPTLEN;
	while (*src && prompt_length--) {
	    *dst++ = *src++;
	}
	dst += reg_invoke_parse_cli_prompt(tty->parse_cli_type,
					   csb, dst, &prompt_length);
	src = get_mode_prompt(csb);
	if (src && (prompt_length > 3)) {
	    prompt_length -= 3;
	    *dst++ = '(';
	    while (*src && prompt_length--) {
		*dst++ = *src++;
	    }
	    *dst++ = ')';
	} else {
	    /*
	     * Prompt is too long, so cut off end
	     */
	    if (prompt_length <= 0) {
		dst = &csb->prompt[MAXPROMPTLEN-1];
	    }
	}
	*dst++ = trailer;
	*dst = '\0';
	
	/*
	 * This provides a hook for a routine to globally rewrite
	 * the prompt now that all of its parts have been assembled.
	 */
	dst += reg_invoke_parse_cli_prompt(PARSER_CLI_OVERRIDE, csb,
					   dst, &prompt_length);
    } else {
	csb->prompt[0] = '\0';
    }
}

/*
 * exec
 * The EXEC command process
 */

forktype exec (tt_soc *tty)
{
    int error;
    int i;
    boolean running;		/* TRUE while we are wanted */
    boolean autoconnect;	/* have we connected yet ? */
    boolean autoconnect_do_readline;	/* If true, do readline despite	*/
					/* value of autoconnect		*/
    parseinfo *csb;		/* local ptr to csb of this exec session */
    char lastchar;		/* Last character of user input */
    boolean use_def_autocommand = TRUE; /* ok to use tty_def_autocommand */
    int ch = 0;


    if (modem_debug) {
	buginf("\nTTY%t: EXEC creation",tty->ttynum);
    }

    /*
     * If this is a modem line, wait for the junk to quiet down
     */

    if (execwait_enable && (tty->capabilities & MODEMMASK) &&
	!(tty->capabilities & AUTOBAUD)) {
	i = 20;			/* Wait up to 60 seconds */
	while (i--) {
	    clearinput(tty);
	    process_sleep_for(3*ONESEC);
	    if (modemcheck(tty) || !inputpending(tty))
	      break;
	}
    }

    /* Set privilege level to default */
    if (NEED_TO_SET_PRIV(tty->privilege)) {
	SET_PRIV(tty->privilege, CUR_DEFPRIV(tty->privilege));
    }

    /*
     * free any previous
     * csb blocks & allocate the CSB
     */
    if (tty->csb != NULL)
	free_csb(&tty->csb);
    tty->csb = get_csb("Exec Parser CSB");
    if (tty->csb == NULL) {
	death(tty);
	return;       /* return to hari-kari. Better debugging support */
    }

    reg_invoke_tty_birth(TTYP_EXEC, tty);

    /*
     * Print message of the day
     */
    if (banner_motd && !((tty->capabilities & NOEXECBANNER) ||
                         (tty->capabilities2 & NOMOTDBANNER_2))) {
	printf("%r", banner_motd);
    }
    if (banner_temp) {
        printf("%r", banner_temp);
    }
    if (linenumber_enable && ! (tty->tty_autocommand || 
				tty->tty_def_autocommand ||
				(tty->capabilities & NOEXECBANNER))) {
	printf("\n%s line %t %s\n", hostname, tty->ttynum,
	    get_tty_location(tty));
    }
  dologin:
    /*
     * Free up user string except when this is a callback re-connect 
     * and does not require a re-Authentication to enter exec
     */
    if ((tty->user) && !(tty->acflags & ACF_CALLBACK_NOVERIFY)) { 
	free(tty->user);
	tty->user = NULL;
    }
    if ((tty->ustruct) && !(tty->acflags & ACF_CALLBACK_NOVERIFY))
	aaa_free_user(&tty->ustruct);

    /* Do telnet options negotiation (if any) */
    while ((tty) && (tty->conntop) 
	   && (tty->conntop->proto.tn.do_kerb_auth)) {
	if (tty->statbits & (CARDROP+IDLETIMEOUT+HANGUP)) /* dead line? */
	    break;
	edisms((blockproc *)ttyioBLOCK, (long)tty);     /* dismiss for input */
	ch = getc(tty);
    }

    if (!old_access_control || (tty->actype != AC_NONE)) {/*check line passwd*/
	if (!check_login(tty)) {
	    logout_banner(tty);
	    ttyflush(tty);
	    process_sleep_for(2*ONESEC);
	    death(tty);
	    return;    /* return to hari-kari. Better debugging support */
	}
    }

    /*
     * Now check exec authorization.
     * Don't do it if tty_autocommand is set because that could be from
     * autoselecting.  And besides, we don't authenticate if
     * tty_autocommand is set.
     */
    if (!old_access_control && !tty->tty_autocommand &&
        !(tty->acflags & ACF_CALLBACK_NOVERIFY) &&
	!do_exec_author(tty, &use_def_autocommand)) {
	logout_banner(tty);
	ttyflush(tty);
	process_sleep_for(2*ONESEC);
	death(tty);
	return;
    }

    /*
     * Hack automatic commands.  Setting tty_autocommand tty_def_autocommand
     * simplifies a lot of checks later on in the code.  Do this after login,
     * since login skips lines with "temporary" autocommands only.
     */
    if (tty->tty_autocommand == NULL && use_def_autocommand == TRUE)
	tty->tty_autocommand = tty->tty_def_autocommand;

    if (banner_exec && ! (tty->tty_autocommand ||
			  (tty->capabilities & NOEXECBANNER))) {
	printf("%r", banner_exec);
    }
    reg_invoke_emergency_message(EMERGENCY_LINE_BANNER);
    tty->currentcon = NULL;		/* no connections yet */
    if (parser_priv_debug) {
	printf("\nChanging privilege level to %d default %d",
	       CUR_PRIV(tty->privilege), CUR_DEFPRIV(tty->privilege));
    }
    running = TRUE;			/* until the end */
    autoconnect = FALSE;		/* never connected anywhere */
    csb = tty->csb;			/* local copy for efficiency */
    csb->mode = exec_mode;
    csb->flags = CONFIG_TERM;

    /* authorization may have changed privilege level */
    csb->priv = CUR_PRIV(tty->privilege);

    reg_invoke_aaa_acct_cmd(tty->ustruct, NULL); /* Accounting */

    while (running) {
	lastchar = '\0';
	autoconnect_do_readline = FALSE;
	automore_disable();		/* Ensure no old more processing */
	stdio->statbits &= ~CONTROLC;

	/*
	 * If we got a hotstall interrupt, wait until the process level
	 * hotswap code gets to run and corrects our view of reality
	 * before continuing
	 */
	while (hotstall_flag)
	    process_suspend();

	if (tty->tty_autocommand) {
	    if (!(tty->capabilities & NO_FLUSH_ACTIVATE))
		clearinput(stdio);
	    if (autoconnect) {
		/*
		 * We have an autocommand, but there is already an
		 * open connection, so we must have escaped from
		 * that session and should accept commands until
		 * that session is resumed.
		 */
		if (tty->currentcon)
		    autoconnect_do_readline = TRUE;
		else if (tty->acflags & ACF_RELOGIN) {
		    tty->tty_autocommand = NULL;
		    goto dologin;
		} else
		    strcpy(csb->line, "exit");
	    } else {
		/*
		 * Use sstrncpy() to copy PARSEBUF bytes to csb->line.
		 * tty_autocommand may be pointing to a string with size
		 * larger than PARSEBUF, use strcpy() therefore may cause
		 * buffer overrun. Note that sstrncpy() will alway NULL 
		 * terminate the new string.
		 */
		sstrncpy(csb->line, tty->tty_autocommand, PARSEBUF);
		autoconnect = TRUE;	/* Only let one command through */
	    }
	} else {			/* Allow autocommand to be undone */
	    autoconnect = FALSE;
	    csb->flags |= COMMAND_AUTHORIZE; /* enable command authorization */
	}

        BuildPrompt(csb, tty);
	if ((autoconnect && !autoconnect_do_readline) ||
	    readline(csb->line, PARSEBUF-1, csb->break_chars,
		     &lastchar, TRUE, "%s", csb->prompt)) {
	    if (lastchar == '?') {
		csb->in_help = PARSER_HELP;
	    } else {
		if (lastchar == '\t') {
		    csb->in_help = PARSER_COMMAND_COMPLETION;
		} else {
		    csb->in_help = PARSER_NO_HELP;
		}
	    }

	    csb->lastchar = lastchar;

	    /* Parse the command & check for errors.  If the command 
	     * did not match and we are handling exec commands, check 
	     * for an invalid autoconnect command or treat it as an 
	     * implicit connect command.  Otherwise it was ambiguous
	     * and we report it as an error.
	     *
	     * Hostname connection strings which match exec commands
	     * must use the 'connect' exec command.
	     */
	    flush();
	    error = reg_invoke_parse_cli_cmd(tty->parse_cli_type, csb);
	    if ((error != 0) &&  
		(error != PARSE_TCL_INCOMPLETE_COMMAND)) {
		if ((error & PARSE_ERROR_NOMATCH) &&
		    (csb->mode == exec_mode)) {
		    if (autoconnect && !autoconnect_do_readline) {
			printf("\nLine has invalid autocommand \"%s\"",
			       tty->tty_autocommand);
		    } else {
			if ((tty->tty_transport_pref == 0) ||
			    (csb->flags & KEYWORD_MATCHED)) {
			    report_error(csb, error, FALSE);
			} else {
			    int old_error_index = csb->error_index;
			    int old_error = error;
			    int len;

			    if (parser_ambig_debug) {
				buginf("\nTrying implicit connect command");
			    }
			    len = strlen(csb->line);
			    if (len <= (PARSEBUF-8-1)) {
				sprintf(csb->line, "connect %s",
					tty->texti_info.buffer);
				error = parse_cmd(csb);
				if (error) {
				    /* restore command line */
				    strcpy(csb->line,
					   tty->texti_info.buffer);
				}
			    }
			    if (error != 0) {
				/* Always report the original command error
				 * instead of the implicit connect command
				 * error.
				 */
				csb->error_index = old_error_index;
				report_error(csb, old_error, FALSE);
			    }
			    csb->mode = exec_mode;
			}
		    }
		} else {
		    /* Report the error, but don't require line re-print */
		    report_error(csb, error, FALSE);
		}
	    }

	    /* If ^Z was input or CONFIG_ABORT or CONFIG_DONE is set
	     * and we're in config mode,
	     * force a pop to the Exec command level.  Any input on the
	     * same line as the ^Z has already been processed.
	     * Reset the parse tree to the top level and clear all
	     * active variables used in config mode.
	     */

	    if ((lastchar == CH_SUB || 
		 (csb->flags & (CONFIG_ABORT | CONFIG_DONE))) &&
		((csb->mode == config_mode) || 
		 test_mode_config_subcommand(csb))) {
		exit_config_command(csb);
	    }
	} else {     /* if (autoconnect || readline()) */
	    if ((lastchar == '\003') &&
		((csb->mode == config_mode) ||
		 test_mode_config_subcommand(csb))) {
		exit_config_command(csb);
	    }
	}

	if (csb->exit_all) {
	    running = FALSE;
	    continue;
	}
	if (modemcheck(tty)) {
	    if (tty->type != VTY_LINE)
		tty_clearbuffers(tty);
	    (void) exit_command(FALSE);		/* carrier drop, forced quit */
	    running = FALSE;
	    continue;
	}
	if (tty->statbits & IDLETIMEOUT) {
	    /* The current session timed out.  If there are no more
	     * connections, exit.  Otherwise simulate an empty input line
	     * so that we will resume the open connection.
	     */
	    *csb->line = '\0';
	    if (tty->currentcon == NULL) {
		exit_command(FALSE);
		running = FALSE;
		continue;
	    }
	}
	/* Check if we're at the exec level
	 * and there was no input
	 * and the last character isn't ^Z (i.e., we just exited config mode)
	 * and the user wasn't asking for help.
	 * If this is true, then we should resume the most recently active
	 * connection.  Otherwise, clear the input buffer and get more input.
	 */
	if ((*csb->line == '\0') &&
	    (csb->mode == exec_mode) &&
	    (lastchar != CH_SUB) &&
	    (lastchar != '\003') &&
	    (lastchar != '?')) {
	    /* Resume a connection */
            if (conn_check_idles() &&
                (talk(tty, tty->currentcon, TRUE) == FALSE)) {
		tty->currentcon = conn_leastidle(tty);
		if ((tty->capabilities & AUTOHANGUP) &&
		    !tty->currentcon && !tty->xrcb) {
		    exit_command(FALSE);
		    running = FALSE;
		    continue;
		}
	    }
	}
	/* Clear the input if we were not doing help or command completion */
	if (csb->in_help == PARSER_NO_HELP) {
	    *csb->line = '\0';
	}
    }   /* while (running) */
    
    death(tty);					/* we die now */
}

/*
 * Used for reg_invoke_parse_cli_prompt which does nothing but return
 * 0.
 */
int default_parse_cli_prompt (parseinfo *csb, char *dst, int *prompt_length)
{
    return 0;
}

/*
 * clear_intcommand
 * Worker routine for the clear interface command
 */

void clear_intcommand (parseinfo *csb)
{
    hwidbtype *idb;

    idb = hwidb_or_null(GETOBJ(idb,1));
    reset_if(idb);
}

/*
 * reload_print_hms
 *
 * Print the remaining hours, minutes and possibly seconds until reload,
 * given the number of seconds.
 */
static void reload_print_hms (ulong seconds)
{
    ulong hours, minutes;

    hours = seconds / SECONDS_PER_HOUR;
    seconds -= hours * SECONDS_PER_HOUR;
    minutes = seconds / SECONDS_PER_MINUTE;
    seconds -= minutes * SECONDS_PER_MINUTE;
    if (hours) {
	printf("%d hour%s", hours, (hours > 1) ? "s" : "");
    }
    if (hours || minutes) {
	if (hours && minutes) {
	    printf(" and ");
	}
	if (minutes) {
	    printf("%d minute%s", minutes, (minutes > 1) ? "s" : "");
	}
    }
    if (hours == 0 && minutes < 5) {
	if (minutes && seconds) {
	    printf(" and ");
	}
	if (seconds) {
	    printf("%d second%s", seconds, (seconds > 1) ? "s" : "");
	}
    }
}

/*
 * reload_print_expiration
 *
 * Print the amount of time before (and perhaps the time of day of) the
 * scheduled reload.  Assumes that a reload has been scheduled.
 *
 * The parameter is the number of seconds until the reload.
 */
static void reload_print_expiration (long delay_seconds)
{
    clock_epoch next_reload_epoch;

    if (delay_seconds < 0) {
	printf("\nA reload is imminent.");
    } else {
	if (clock_is_probably_valid()) {
	    clock_get_time(&next_reload_epoch);
	    next_reload_epoch.epoch_secs += delay_seconds;
	    printf("\nReload scheduled for %CC (in ", "%G", &next_reload_epoch);
	    reload_print_hms(delay_seconds);
	    printf(")");
	} else {			/* Clock isn't set */
	    printf("\nReload scheduled in ");
	    reload_print_hms(delay_seconds);
	}
    }
}

/*
 * reload_show
 * 
 * Display the current scheduled reload state.
 */
void reload_show (parseinfo *csb)
{
    if (reload_in_progress()) {
	reload_print_expiration(reload_get_delay());
    } else {
	printf("\nNo reload is scheduled.");
    }
}

/******************************************************************
 * reload {cancel | [{in [hh:]mm | at hh:mm [{Mon dd | dd Mon}] }] [text]}
 *
 * OBJ(string,1) = reload reason
 * OBJ(int,1) = RELOAD_TYPE_IMMEDIATE if no special parameters
 *		RELOAD_TYPE_IN if "in"
 *		RELOAD_TYPE_AT if "at"
 *		RELOAD_TYPE_CANCEL if "cancel"
 * OBJ(int,2) = hours (at);  hours or minutes (in) depending on (int,3)
 * OBJ(int,3) = minutes (at); minutes, or -1 (in) if no hours specified
 * OBJ(int,4) = month (at) (or 0 if date not specified)
 * OBJ(int,5) = day (at)
 */

#define MAX_DELAY_MINUTES (MAXINT/ONEMIN) /* Max reload delay in minutes */
#define MAX_DELAY_SECONDS (MAX_DELAY_MINUTES * SECONDS_PER_MINUTE)
						/* Max delay in seconds */

void reload_command (parseinfo *csb)
{
    ushort config_reg;
    ulong reload_delay;
    timeval reload_time;
    clock_epoch current_epoch;
    ulong current_seconds;
    clock_epoch last_midnight;
    ulong reload_seconds;
    clock_epoch reload_epoch;
    boolean delayed_reload;

    /* Initialize. */

    delayed_reload = FALSE;
    reload_seconds = 0;
    clock_get_time(&current_epoch);
    current_seconds = system_uptime_seconds();

    /* Figure out what kind of reload it is. */

    switch (GETOBJ(int,1)) {
      case RELOAD_TYPE_IN:

	/*
	 * Get the hours and minutes.  Note that if the minutes are -1,
	 * the minutes are really in the hours field.
	 */
	if (GETOBJ(int,3) == -1) {	/* No hours */
	    reload_delay = GETOBJ(int,2); /* In minutes */
	} else {			/* Hours and minutes */
	    reload_delay = (GETOBJ(int,2) * MINUTES_PER_HOUR) + GETOBJ(int,3);
	}

	/* Bail if delay is too long. */

	if (reload_delay > MAX_DELAY_MINUTES) {
	    printf("\n%%Reload delay cannot exceed %d hours and %d minutes",
		   MAX_DELAY_MINUTES / MINUTES_PER_HOUR,
		   MAX_DELAY_MINUTES % MINUTES_PER_HOUR);
	    return;
	}
	reload_seconds = current_seconds + (reload_delay * SECONDS_PER_MINUTE);
	delayed_reload = TRUE;
	break;


      case RELOAD_TYPE_AT:

	/* Squeal if the clock hasn't been set. */

	if (!clock_is_probably_valid()) {
	    printf("\n%%The date and time must be set first.");
	    return;
	}

	/* Calculate the epoch of the midnight just past. */

	clock_epoch_to_timeval(&current_epoch, &reload_time,
			       clock_timezone_offset());
	reload_time.hour = 0;
	reload_time.minute = 0;
	reload_time.second = 0;
	reload_time.millisecond = 0;
	clock_timeval_to_epoch(&reload_time, &last_midnight);

	/*
	 * If the date has been specified, update the timeval with it,
	 * and if it's before today, bump the year.
	 */
	if (GETOBJ(int,4)) {
	    reload_time.month = GETOBJ(int,4);
	    reload_time.day = GETOBJ(int,5);
	    clock_timeval_to_epoch(&reload_time, &reload_epoch);
	    if (reload_epoch.epoch_secs < last_midnight.epoch_secs) {
		reload_time.year++;
	    }
	    reload_time.hour = GETOBJ(int,2);
	    reload_time.minute = GETOBJ(int,3);
	} else {			/* No date */

	    /*
	     * No date was specified.  If the time is before right now,
	     * bump the date.
	     */
	    reload_time.hour = GETOBJ(int,2);
	    reload_time.minute = GETOBJ(int,3);
	    clock_timeval_to_epoch(&reload_time, &reload_epoch);
	    if (reload_epoch.epoch_secs < current_epoch.epoch_secs) {
		reload_time.day++;
	    }
	}

	/*
	 * If the specified time has already passed, complain.  If it's too
	 * far in the future, do likewise.
	 */
	clock_timeval_to_epoch(&reload_time, &reload_epoch);
	if (reload_epoch.epoch_secs < current_epoch.epoch_secs) {
	    printf("\n%%Command ignored--the specified time is in the past.");
	    return;
	}
	reload_delay = reload_epoch.epoch_secs - current_epoch.epoch_secs;
	if (reload_delay > MAX_DELAY_SECONDS) {
	    printf("\n%%Command ignored--the specified time is too far"
		   " in the future.");
	    return;
	}
	delayed_reload = TRUE;
	break;


      case RELOAD_TYPE_CANCEL:

	/* Cancel a previously scheduled reload. */

	if (!reload_in_progress()) {
	    printf("\n%%No reload is scheduled.");
	    return;
	}

	reload_cancel();
	return;

      case RELOAD_TYPE_IMMEDIATE:
	break;

    }

    config_reg = ~mon_getconfig();
    if (stdio->type != CTY_LINE) {  /* Not on the console */
        if ((config_reg & CFG_FILE) == 0) { /* boot bits are zero */
            printf("Reload to the ROM monitor disallowed from a telnet line.\n");
            printf("Set the configuration register boot bits to be non-zero.\n");
            return;
        }
    }
    if (reg_invoke_is_config_dirty() == TRUE) {
	if (csb->flags & CONFIG_HTTP) {
	    printf("\nSystem configuration has been modified."
		   "\nSave configuration and resubmit reload command.");
	    return;
	}

	if (yes_or_no("\nSystem configuration has been modified. Save",
		      FALSE, FALSE)) {
	    write_memory(NULL); /* save config to NVRAM */
	}
    }

    if (reg_invoke_abort_reload() == TRUE)
	return;

    /* If this is a delayed reload, squawk the time. */

    if (delayed_reload) {
	if (reload_seconds) {	/* "reload in" */
	    reload_print_expiration(reload_seconds - system_uptime_seconds());
	} else {			/* "reload at" */
	    clock_get_time(&current_epoch);
	    reload_print_expiration(reload_epoch.epoch_secs -
				    current_epoch.epoch_secs);
	}
    }

    if (confirm("\nProceed with reload? ")) {

	/* Schedule the reload, or do it right now. */

	if (delayed_reload) {

	    /* Call the big honking routine to do it. */

	    reload_schedule_with_message(reload_seconds ?
					 NULL : &reload_epoch,
					 reload_seconds -
					 system_uptime_seconds(),
					 NULL, 0, -1, NULL, GETOBJ(string,1),
					 FALSE, TRUE);

	} else {			/* Immediate reload */
	    boot_reason = GETOBJ(string,1);
	    reload_system(RELOAD_NORMAL);
	}
    }
}

/*
 * reload_emergency_message
 *
 * Squawk a message in the banner and "show version" when a reload has been
 * scheduled.
 */
void reload_emergency_message (emergency_msg_time_type msg_type)
{
    switch (msg_type) {
      case EMERGENCY_SHOW_VERSION:
      case EMERGENCY_LINE_BANNER:
	if (reload_in_progress()) {
	    reload_print_expiration(reload_get_delay());
	}
	break;
      default:
	break;
    }
}

/*
 * exec_help_command
 *
 * Provide the user with an introduction to the interactive
 * help mechanism of the new parser.
 */
void exec_help_command (parseinfo *csb)
{
    if (csb->nvgen) {
        return;
    }

    printf("\nHelp may be requested at any point in a command by entering"
           "\na question mark '?'.  If nothing matches, the help list will"
           "\nbe empty and you must backup until entering a '?' shows the"
           "\navailable options."
           "\nTwo styles of help are provided:"
           "\n1. Full help is available when you are ready to enter a"
           "\n   command argument (e.g. 'show ?') and describes each possible"
           "\n   argument."
           "\n2. Partial help is provided when an abbreviated argument is entered"
           "\n   and you want to know what arguments match the input"
           "\n   (e.g. 'show pr?'.)\n");
}

void show_help_command (parseinfo *csb)
{
    printf("\n%% Type \"%s ?\" for a list of subcommands", "show");
}

void clear_help_command (parseinfo *csb)
{
    printf("\n%% Type \"%s ?\" for a list of subcommands", "clear");
}

void debug_help_command (parseinfo *csb)
{
    printf("\n%% Type \"%s ?\" for a list of subcommands", "debug");
}

void setup_command (parseinfo *csb)
{
    resolve_all_protocols();
}

void disconnect_command (parseinfo *csb)
{
    tt_soc *tty;

    tty = stdio;
    if (close_command(tty, GETOBJ(int,1), GETOBJ(string,1),
		      tty->currentcon) == TRUE) {
	tty->currentcon = conn_leastidle(tty);
    }
}

void hangup_exec_command (parseinfo *csb)
{
    (void) exit_command(FALSE);
    csb->exit_all = TRUE;
}

void send_command (parseinfo *csb)
{
    sendmessage(GETOBJ(int,1));
}

/*
 * show_version
 * Display hardware and software configuration of system
 */

void show_version (parseinfo *csb)
{
    ushort new_configreg;

    automore_enable(NULL);
    host_banner();
    printf("\n\n");
    Print_Hardware();
    printf("\nConfiguration register is %#x", (ushort)configregister);
    new_configreg = (ushort) ~mon_getconfig();
    if (configregister != new_configreg) {
	printf(" (will be %#x at next reload)\n", new_configreg);
    } else {
	printf("\n");
    }
    reg_invoke_emergency_message(EMERGENCY_SHOW_VERSION);
    automore_disable();
}


/*
 * Print_Hardware
 *
 * Our old friend. Print out all the information we have on the current
 * hardware configuration.
 */

void Print_Hardware (void)
{
    uint mainsize, iosize;
    char *hardware_rev, *board_id, *board_rework, *processor_rev, *last_reset;

    printf("%s %s (%s) processor",
	   platform_get_string(PLATFORM_STRING_VENDOR),
	   platform_get_string(PLATFORM_STRING_NOM_DU_JOUR),
	   platform_get_string(PLATFORM_STRING_PROCESSOR));

    hardware_rev = platform_get_string(PLATFORM_STRING_HARDWARE_REVISION);
    if (hardware_rev)
	printf(" (revision %s)", hardware_rev);

    mainsize = region_get_size_by_attributes(REGION_CLASS_LOCAL,
					     REGION_STATUS_PARENT,
					     REGION_MEDIA_ANY);
    iosize = region_get_size_by_attributes(REGION_CLASS_IOMEM,
					   REGION_STATUS_PARENT,
					   REGION_MEDIA_ANY);

    printf(" with %dK", mainsize/1024);
    if (iosize)
	printf("/%dK", iosize/1024);
    printf(" bytes of memory.\n");

    board_id = platform_get_string(PLATFORM_STRING_HARDWARE_SERIAL);
    if (board_id) {
	printf("Processor board ID %s", board_id);
	board_rework = platform_get_string(PLATFORM_STRING_HARDWARE_REWORK);
	if (board_rework)
	    printf(", with hardware revision %s", board_rework);
	printf("\n");
    }

    processor_rev = platform_get_string(PLATFORM_STRING_PROCESSOR_REVISION);
    if (processor_rev)
	printf("%s\n", processor_rev);

    last_reset = platform_get_string(PLATFORM_STRING_LAST_RESET);
    if (last_reset)
	printf("Last reset from %s\n", last_reset);

    reg_invoke_print_copyright();
    reg_invoke_print_features();
    reg_invoke_print_controllers();
    idb_print_summary();
    reg_invoke_show_int_enhanced();
    reg_invoke_print_memory();
    reg_invoke_flash_print_hardware();
    reg_invoke_print_slave_info();
}


/*
 *
 */
void show_controllers (parseinfo *csb)
{
    hwidbtype *idb = NULL;
    boolean first_time = TRUE;

    if (GETOBJ(idb,1)) {
	idb = GETOBJ(idb,1)->hwptr;
    }

    automore_enable(NULL);
    if (idb) {
	if (idb->show_controller) {
	    (*idb->show_controller)(idb, csb);
	}
    } else {
	FOR_ALL_HWIDBS(idb) {
	    if ((GETOBJ(int,1) && !(GETOBJ(int,1) & idb->status))||
                (idb->status & IDB_DELETED))  {
		continue;
	    }

	    if (idb->show_controller) {
		if (first_time) {
		    first_time = FALSE;
		} else {
		    automore_conditional(0);
		}
		if ((*idb->show_controller)(idb, csb)) {
		    break;
		}
	    }
	}
    }
    automore_disable();
}

/*
 * file_xfer_progress
 *
 * Show file transfer progress "!!!!!!!!!!..."
 *
 */

void file_xfer_progress (int num, int divisor)
{

    if (((num - 1) % divisor) == 0) {
        printc('!');
        flush();
    }
}
