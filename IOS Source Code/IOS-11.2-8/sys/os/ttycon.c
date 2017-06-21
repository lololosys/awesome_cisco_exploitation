/* $Id: ttycon.c,v 3.15.6.10 1996/09/05 01:17:54 sjackson Exp $
 * $Source: /release/112/cvs/Xsys/os/ttycon.c,v $
 *------------------------------------------------------------------
 * ttycon.c -- minimum device independent terminal service routines
 *             for console output only
 *  
 * October 10, 1989, Chris Shaker
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ttycon.c,v $
 * Revision 3.15.6.10  1996/09/05  01:17:54  sjackson
 * CSCdi67473:  big buffers created not trimmed (queued on TCP input with
 * rtelnet)
 * Enable coalescing of TCP input packets by default for reverse telnet
 * Branch: California_branch
 *
 * Revision 3.15.6.9  1996/07/11  19:23:57  sjackson
 * CSCdi60440:  Unable to Allocate IP Address From Local (Non-Default) Pool
 * Branch: California_branch
 * Configuration IDB containing pool information determined from physical
 * interface when required
 *
 * Revision 3.15.6.8  1996/07/09  23:50:26  pgoginen
 * CSCdi56342:  A few characters lost during a data transfer over an async
 * tunnel
 * Branch: California_branch
 * Send XON after making sure we can allocate buffers for the incoming
 * data once it starts arriving.
 *
 * Revision 3.15.6.7  1996/06/21  17:01:26  widmer
 * CSCdi53912:  Parser misinterprets second line of int description upon
 * reload
 * Branch: California_branch
 * Don't allow escaping of \r or \n
 *
 * Revision 3.15.6.6  1996/06/12  14:29:52  bjthomas
 * CSCdi38891:  need warning when line session-timeout nears
 * Branch: California_branch
 *
 * Revision 3.15.6.5  1996/06/06  04:25:31  widmer
 * CSCdi59599:  Add printf style formatting to parse_line
 * Branch: California_branch
 *
 * Revision 3.15.6.4  1996/06/05  15:05:49  widmer
 * CSCdi57346:  Add printf style formatting to readline prompt
 * Branch: California_branch
 *
 * Revision 3.15.6.3  1996/05/21  09:58:31  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.15.6.2  1996/04/29  17:32:53  eschaffe
 * CSCdi54905:  vty-async packets going from x25 to ethernet are delayed
 * Branch: California_branch
 *
 * Revision 3.15.6.1  1996/03/18  21:31:39  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.11.12.3  1996/03/16  07:21:32  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.11.12.2  1996/03/07  10:20:08  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.11.12.1  1996/02/20  16:42:15  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.15  1996/03/05  01:19:21  billw
 * CSCdi49461:  autoselect during-login causes premature authorization
 * Additional commenting...
 *
 * Revision 3.14  1996/02/23  10:38:01  billw
 * CSCdi49461:  autoselect during-login causes premature authorization
 * Define a new flag AUTOSELECT_AUTHORIZE that survives across death().
 * Set it at the same time AUTOSELECTED is set, but only clear it when
 * a new autoselect starts.
 *
 * Revision 3.13  1996/02/11  09:21:01  billw
 * CSCdi47099:  not nearly enough processes call process_set_crashblock
 *
 * Revision 3.12  1996/02/07  21:15:13  widmer
 * CSCdi48343:  Remove unused #ifdef STANDALONE from parser code
 *
 * Revision 3.11  1995/12/22  18:14:58  vandys
 * CSCdi45755:  SPurious Accesses in strcmp() called by TI_add_history()
 * Check for zero-length history before comparing against
 * the "previous" history entry.  Also, use a local variable
 * to point at the history data structure.
 *
 * Revision 3.10  1995/12/17  18:34:28  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.9  1995/12/10  21:13:06  widmer
 * CSCdi45105:  Running-configs line exec-timeout and history change at
 * login
 * Add argument to readline() to specify whether command should be added
 * to history, change existing readline() calls to use TRUE, change
 * askstring() to call readline() with FALSE.
 *
 * Revision 3.8  1995/12/09  00:43:48  tkolar
 * CSCdi45302:  Need a clean interface between modem hardware and
 * commserver
 * Make it easy to claim a tty for exclusive use by a process, and add a
 * modem_busyout registry.
 *
 * Revision 3.7  1995/12/01  18:47:47  sjackson
 * CSCdi36114:  async routing dies under heavy load
 *
 * Revision 3.6  1995/11/29  08:11:47  tkolar
 * CSCdi44485:  autoselect during-login debugging is insecure
 * Suppress autoselect debugging info while line is not echoing.
 *
 * Revision 3.5  1995/11/22  05:47:35  irfan
 * CSCdi43970:  low memory overwrite by input helper process
 * Avoid null dereferencing of vty->conntop.
 *
 * Revision 3.4  1995/11/20  10:44:04  billw
 * CSCdi42894:  Performance: AUX port SLIP/PPP uses excessive CPU
 *
 * Revision 3.3  1995/11/17  18:52:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:19:16  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:48:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.11  1995/11/08  21:19:27  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.10  1995/10/24  04:59:49  eschaffe
 * CSCdi42182:  session timeout not reset by ppp packets
 *
 * Revision 2.9  1995/09/22  21:14:04  eschaffe
 * CSCdi40681:  System was restarted by error - Illegal Instruction, PC 0x0
 *
 * Revision 2.8  1995/08/06  01:15:18  eschaffe
 * CSCdi36149:  ppp data stuck in virtual interface output queue
 *
 * Revision 2.7  1995/07/14  07:02:51  hampton
 * Convert Terminal Server to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi37093]
 *
 * Revision 2.6  1995/06/23 18:48:30  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.5  1995/06/21  08:56:21  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.4  1995/06/20  04:14:26  ahh
 * CSCdi36095:  Connection routines use msclock
 * Remove more msclock references.  (Should have been in initial commit,
 * but something got screwed up.)
 *
 * Revision 2.3  1995/06/19  06:49:03  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:12:19  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:59:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ctype.h>
#undef   toupper			/* use library function call */
#undef   tolower			/* use library function call */
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "sys_registry.h"
#include "ttysrv.h"
#include "logger.h"
#include "exception.h"
#include "connect.h"
#include "async.h"
#include "../ts/if_async.h"
#include "config.h"
#include "parser.h"
#include "stdarg.h"

#include "texti.h"
#include "texti_keymaps.h"
#include "../parser/parser_actions.h"

#include "../ui/command.h"
#include "../parser/parser_defs_autoselect.h"
#include "../ts/callback.h"

/*
 * local storage
 */
uchar execmask_default = 0x7F;
uchar datamask_default = 0xFF;
uchar specialmask_default = 0x7F;

/*
 * tty_init
 * Initialize the terminal data structure
 */

tt_soc *tty_init (int n, ulong resetstatus, int ttytype, vectortype *vector)
{
    register tt_soc *tty;

    tty = malloc_named(sizeof(tt_soc), "TTY data");
    MODEMS[n] = tty;
    if (tty == NULL) {
	system_running = FALSE;
	crashdump(0);
	return(NULL);
    }
    tty_init_data(tty, n, resetstatus, ttytype, vector);
    return(tty);
}

/*
 * tty_init_data
 * Fill in data fields in a tty data structures.
 */

void tty_init_data (tt_soc *tty, int n, ulong resetstatus,
		    int ttytype, vectortype *vector)
{
    int i;

    tty->ttynum = n;
    tty->type = ttytype;
    tty->ivector = tty->ovector = tty->nvector = vector;
    tty->statbits = resetstatus;
    tty->def_capabilities = tty->capabilities = 0L;
    tty->tty_exectimeout = IDLE_EXEC_DEFAULT;
    tty->tty_sessiontimeout = IDLE_SESSION_DEFAULT;
    tty->sessiontimeout_warning = SESSIONTIMEOUT_WARNING_DEFAULT*ONESEC;
    tty->sessiontimeout_warning_msg = NULL;
    tty->autoselectmask = 0;
    tty->tty_absolutetimeout = ABSOLUTE_TIMEOUT_DEFAULT * ONEMIN;
    tty->logoutwarning = LOGOUT_WARNING_DEFAULT * ONESEC;
    tty->answertimeout = MODEMWAIT;
    tty->callback_forcewaittime = CALLBACK_DEFAULT_FORCEDWAIT_SEC;
    tty->callback_wait_nodsr = CALLBACK_DEFAULT_WAIT_NODSR;
    tty->isavedchar = -1;		/* no saved input char */
    tty->logoutchar = -1;		/* no logout char */
    tty->activatechar = RETURN;		/* default activation character */
    tty->tty_escapechar =  TTY_ESCAPECHAR; /* escape */
    tty->tty_holdchar = -1;			/* no hold char */
    tty->tty_length = LENGTH_DEFAULT;	/* screen length */
    tty->tty_width = WIDTH_DEFAULT;	/* screen width */
    tty->tty_lat_groupmask[0] = 1;	/* Enable group code 1 */
    tty->tty_lat_groupmask[1] = 1;	/* ... */
    tty->tty_datamask = datamask_default;
    tty->tty_specialmask = specialmask_default;
    tty->tty_execmask = execmask_default;
    tty->tty_historyon = TRUE;
    tty->tty_historysize = HISTORY_SIZE_DEFAULT;
    tty->tty_dns = TRUE;
    tty->tty_editingon = TRUE;
    tty->tty_help = FALSE;
    tty->tty_stopchar = CTRLS;		/* Default STOP character */
    tty->tty_startchar = CTRLQ;		/* Default START character */
    tty->tty_tcp_coalesce_size = DEF_TCP_COALESCE;
    /*
     * Default TTY Async DTR pulse time
     */
    tty->dtr_pulseinterval = ASYNC_DEFAULT_PULSEINTERVAL;              

    if ((tty->type == TTY_LINE) || (tty->type == AUX_LINE)) {
    	tty->transport_allowin = 0;
    } else {
    	tty->transport_allowin = CONN_TRANSPORT_DEFMASK;
    }

    tty->transport_allowout = CONN_TRANSPORT_DEFMASK;
    tty->tty_transport_pref = CONN_TRANSPORT_DEFMASK;

    tty->log_sync = FALSE;	        /* Default to async log message */
    tty->log_sync_lock = LOG_SYNC_IS_LOCK;
    tty->log_sync_level = LOG_SYNC_DEF_LEVEL;
    tty->log_sync_msgsize = LOG_SYNC_DEF_SIZE;
    tty->log_sync_reprint_type = 0;

    for (i = 0; i < NSPECIAL; i++)
	tty->tty_editinfo.special[i] = CH_UNDEFINED;
    tty_setedit(&tty->tty_editinfo, NVT_EC2, CH_BS);	/* BS erases char */
    tty_setedit(&tty->tty_editinfo, NVT_ABORT, CH_EOT);	/* ^C is abort */
    tty_setedit(&tty->tty_editinfo, NVT_EC, CH_DEL);	/* Rubbout erase char */
    tty_setedit(&tty->tty_editinfo, NVT_EL, CH_NAK);	/* ^U erases line */
    tty_setedit(&tty->tty_editinfo, NVT_EL2, CH_CAN);	/* ^X erases line */
    tty_setedit(&tty->tty_editinfo, NVT_EW, CH_ETB);	/* ^W erases word */
    tty_setedit(&tty->tty_editinfo, NVT_RP, CH_DC2);	/* ^R reprints */
    tty->tty_editinfo.noecho = FALSE;

    queue_init(&tty->messageQ, TTY_MESSAGEQ);
    queue_init(&tty->asyncmsgQ, TTY_ASYNCMSGQ);

    tty->automore_flush = FALSE;
    tty->automore_enabled = FALSE;
    tty->automore_supress = 0;

    tty->cfgidb = NULL;

    texti_init(tty);
    tty->parse_cli_type = PARSER_CLI_DEFAULT;
    SET_DEFPRIV(tty->privilege, DEFAULT_PRIV);
    SET_PRIV(tty->privilege, CUR_DEFPRIV(tty->privilege));
    SET_NEED_TO_SET_PRIV(tty->privilege);
}

/*
 * novector
 * Does nothing but return FALSE status.
 */ 

static boolean novector (void)
{
    return(FALSE);
}

/*
 * vector_init
 * Initialize a dispatch vector to the null state.
 * This could be used to create a /dev/null or NUL: device!
 */

void vector_init (vectortype *vector)
{
    int i;
    long *cell;

    cell = (long *) vector;
    for (i = 0; i < NVECTORS; i++)
	*cell++ = (long) novector;
}

/*
 * serial_inputpending
 * Return count of bytes buffered for input
 */

int serial_inputpending (tt_soc *tty)
{
    register int count;

    count = tty->incount;
    if (tty->isavedchar != -1)
	count++;
    if (!count && (tty->statbits & STOPPEDINPUT))
      startinput(tty);		/* Ensure input is started */
    return(count);
}

/*
 * ttyioBLOCK
 * Event dismiss for input from the tty.
 */

boolean ttyioBLOCK (register tt_soc *tty)
{
    if (tty->statbits & CONTROLC)
	return(FALSE);
    if (inputpending(tty))
	return(FALSE);
    if (TIMER_RUNNING_AND_AWAKE(tty->dallytimer)) {
	tty->statbits |= IDLETIMEOUT;
	return(FALSE);
    }
    if (modemcheck(tty))
	return(FALSE);
    if (tty->log_sync && (tty->log_sync_reprint_type & LOG_SYNC_RPRNT_REQ)) {
	tty->log_sync_reprint_type &= ~LOG_SYNC_RPRNT_REQ;
	tty->log_sync_reprint_type |= LOG_SYNC_RPRNT_GRANTED;
	return(FALSE);
    }
    return(TRUE);
}

/*
 * logsyncBLOCK
 * Event dismiss for logging synchronous lock.
 */

static boolean logsyncBLOCK (register tt_soc *tty)
{
    if (tty->log_sync_lock == LOG_SYNC_IS_LOCK)
	return(FALSE);
    if (tty->log_sync && (tty->log_sync_reprint_type & LOG_SYNC_RPRNT_REQ)) {
	tty->log_sync_reprint_type &= ~LOG_SYNC_RPRNT_REQ;
	tty->log_sync_reprint_type |= LOG_SYNC_RPRNT_GRANTED;
	return(FALSE);
    }
    return(TRUE);

}

/*
 * getcwait
 * Wait for a character to arrive.
 * Returns -1 on timeout or carrier drop.
 */

int getcwait (register tt_soc *tty)
{
    register int ch;

    if (tty->statbits & (CARDROP+IDLETIMEOUT+HANGUP)) /* dead line? */
	return(-1);
    if (tty->tty_exectimeout) {
	TIMER_START(tty->dallytimer, tty->tty_exectimeout);
    } else
	TIMER_STOP(tty->dallytimer);
    LOG_SYNC_UNLOCK(tty);
    while (TRUE) {
	(void)startoutput(tty);		/* force any pending output */
	edisms((blockproc *)ttyioBLOCK, (long)tty);	/* dismiss for input */
	if (tty->log_sync && 
	    (tty->log_sync_reprint_type & LOG_SYNC_RPRNT_GRANTED)) {
	    tty->log_sync_reprint_type &= ~LOG_SYNC_RPRNT_GRANTED;
	    reprint_prompt(tty);
	    continue;
	}
	ch = getc(tty);			/* read a character */
        if (tty->statbits & (CARDROP+IDLETIMEOUT+HANGUP+CONTROLC)) {
	    ch = -1;
	    break;
	}
	if (ch == BREAK_CHAR)		/* ignore break at this level */
	    ch = -1;
	if (ch >= 0)	                /* if .gt. zero, we have a char */
	    break;
    }

    LOG_SYNC_REQUEST(tty);
    if (tty->log_sync && tty->log_sync_lock != LOG_SYNC_IS_LOCK) {
	/* dismiss waiting for Tty background proc to release lock */
	edisms((blockproc *)logsyncBLOCK, (long)tty);
    }
    if (tty->log_sync && 
	(tty->log_sync_reprint_type & LOG_SYNC_RPRNT_GRANTED)) {
	tty->log_sync_reprint_type &= ~LOG_SYNC_RPRNT_GRANTED;
	reprint_prompt(tty);
    }
    return(ch);
}

/*
 * ttyin
 * Block waiting to read in a character
 */

int ttyin (tt_soc *tty)
{
    int c;

    c = getcwait(tty);
    c &= 0377;
    printc(c);
    return(c);
}

/* 
 * serial_getc
 * Get a character from a serial line input buffer.
 * Note: We use the atomic_decrement() primitive to decrement the incount
 * to save the overhead of raise_interrupt_level/reset_interrupt_level
 * calls on the terminal server for every character processed.
 * tty->statbits manipulations are unprotected.
 */

int serial_getc (register tt_soc *tty)
{
    register uchar i;

    if (tty->isavedchar != -1) {	/* check for saved input char */
	i = (uchar) tty->isavedchar;
	tty->isavedchar = -1;
	return((int) i);
    }

    /*
     * Check buffered data
     */
    if (tty->incount == 0) {
	/*
	 * If manually stopped, restart
	 */
	if (tty->statbits & STOPPEDINPUT) {
	    startinput(tty);
	    return(-1);
	}

	/*
	 * Else ask driver about underlying state
	 */
	if ((*(tty->nvector->vinputpending))(tty) == 0) {
	    /*
	     * Nope, he doesn't have any data, either
	     */
	    return(-1);
	}

	/*
	 * Make sure we only honor data queued under the tt_soc already
	 */
	if (tty->incount == 0) {
	    return(-1);
	}
    }

    if (tty->statbits & (CARDROP + HANGUP + IDLE))
	return(-1);
    i = *tty->inhead++;
    if (tty->inhead >= tty->inend)
	 tty->inhead = tty->instart;
    atomic_decrement(&tty->incount);

    if ((tty->statbits & SENTXOFF) && (tty->incount < (tty->insize >> 2))) {
	if (reg_invoke_tty_xon(tty) == FALSE) {
	    if (quick_putc(tty, tty->tty_startchar)) {
		if (modem_debug)
		    buginf("\ntty%t: Sending XON", tty->ttynum);
		tty->statbits &= ~SENTXOFF;
	    }
	}
    }
    if (tty->statbits & (LASTWASBREAK|LASTWASSPEC)) {
	if (tty->statbits & LASTWASBREAK) {
	    tty->statbits &= ~LASTWASBREAK;
	    return(BREAK_CHAR);
	}
	if (tty->statbits & LASTWASSPEC) {
	    tty->statbits &= ~LASTWASSPEC;
	    return(ESCAPED_FLAG | (uint)i);
	}
    }
    return((uint) i);
}

/*
 * erase_char
 * Given a character, remove it from the screen
 */

void erase_char (uchar c)
{
     if (stdio->tty_editinfo.noecho)
	return;
    if (c < 040)
	printf("\010 \010\010 \010");
    else
	printf("\010 \010");
}

/*
 * texti
 *	Read a blob of text terminated by the break character
 */


boolean texti (char *input_buffer, int maxline, 
	       char *breakset, char *lastchar)
{
    *input_buffer = '\0';
    return(readline(input_buffer, maxline, breakset, lastchar, TRUE, ""));
}

/* 
 * autoselect_check 
 */

static boolean autoselect_check (int ch, tt_soc *tty)
{
    pid_t pid;

    if ((ch > 0) && (tty->autoselectmask & AUTOSELECTING)) {
	tty->autoselect_sample |= ch & 0xFF;
	if (modem_debug) {
	    if (tty->tty_editinfo.noecho) {
		buginf("\nTTY%t: Autoselect(2) sample [suppressed--line is"
		" not echoing]", tty->ttynum);
	    } else {
		buginf("\nTTY%t: Autoselect(2) sample %x", tty->ttynum,
		tty->autoselect_sample);
	    }
	}
	if (reg_invoke_exec_process(tty, tty->autoselect_sample)) {
	    process_get_pid(&pid);
	    if (tty->tiptop == -1 || tty->tiptop == pid) {
		/*
		 * Failed to create an exec process ?
		 * We'll have to clean up and hang up modem control.
		 */
		process_sleep_for(ONESEC);       /* Wait for messages */
		RELEASE_TTY(tty);
		terminate_line(tty);
	    }
	    tty->autoselectmask &= ~AUTOSELECTING;
	    /*
	     * Set bits indicating we were able to detect a protocol.
	     * AUTOSELECTED is for this process, and indicates that we
	     * should abort the autoselect or login currently in progress.
	     * it is also used as a semaphore to prevent two processes from
	     * running at the same time.  AUTOSELECT_AUTHORIZE is sticky
	     * across process death, and is used by the next process we
	     * start to indicate that it was started because of an autoselect.
	     */
	    tty->autoselectmask |= AUTOSELECTED|AUTOSELECT_AUTHORIZE;
	    return(TRUE);  /* we autoselected something, so bail */
	}
	tty->autoselect_sample <<= 8;
    }
    return(FALSE);
}

/*
 * Readline allows a prompt to be specified in addition to the regular
 * texti things.
 */
boolean	readline (char *input_buffer, int maxline, 
		  const char *breakset, char *lastchar,
		  boolean add_to_history, const char *prompt_fmt, ...)
{
    va_list prompt_args;
    boolean ret;

    va_start(prompt_args, prompt_fmt);
    ret = readline_args(input_buffer, maxline, breakset, lastchar,
			add_to_history, prompt_fmt, prompt_args);
    va_end(prompt_args);
    return(ret);
}

boolean readline_args (char *input_buffer, int maxline,
		       const char *breakset, char *lastchar,
		       boolean add_to_history, const char *prompt_fmt,
		       va_list prompt_args)
{
    texti_struct *edit = &(stdio->texti_info);
    int readinginput = TRUE;	/* true if still accepting input */

    edit->history->used = 0;
    edit->input_buffer = input_buffer;
    if (maxline < 256) {
	edit->max_length = maxline;
    } else {
	edit->max_length = 256;
    }
    TI_texti_init(edit, prompt_fmt, prompt_args);
    if (strlen(input_buffer) > 0) {
	if (!TI_insert_string(edit, input_buffer, STRING_REPLACE)) {
	    BEEP;
	}
	TI_display(edit);
    }

    stdio->log_sync_reprint_type = LOG_SYNC_RPRNT_READLINE;
    while (readinginput == TRUE) {
	if ((edit->input_char == RETURN) &&
	    (!strchr(breakset, RETURN))) {
	    edit->input_char = LINEFEED;
	} else {
	    edit->input_char = getcwait(stdio);	/* wait for input char */
	    if (autoselect_check(edit->input_char, stdio)) {
		return(FALSE);
	    }
	    if (edit->input_char < 0) {		/* timeout or carrier drop */
		*lastchar = '\0';
		if (stdio->statbits & CTRLC_ENBL) {
		    stdio->statbits |= CONTROLC;  /* get out of setup if in */
		}
		LOG_SYNC_REPRINT_END(stdio);
		return(FALSE);
	    }
	}

	edit->input_char &= stdio->tty_execmask;
	if (edit->flags & JUST_KILLED) {
	    edit->flags &= ~JUST_KILLED;
	    edit->flags |= KILLED_BEFORE;
	} else {
	    edit->flags &= ~KILLED_BEFORE;
	}
	if (edit->flags & JUST_YANKED) {
	    edit->flags &= ~JUST_YANKED;
	    edit->flags |= YANKED_BEFORE;
	} else {
	    edit->flags &= ~YANKED_BEFORE;
	}
	if (edit->flags & TERMINAL_ESCAPE) {
	    /*  if it's a printable character, then accept it */
	    if ((edit->input_char < 127) &&
		(edit->input_char != '\r') &&
		(edit->input_char != '\n')) {
		if (!TI_insert_char(edit)) {
		    BEEP;
		}
	    } else {
		BEEP;
	    }
	    edit->flags &= ~TERMINAL_ESCAPE;
	    TI_display(edit);
	    continue;
	}
	if ((!(edit->flags & ESCAPE_CHAR)) &&
	    (!(edit->flags & TERMINAL_ESCAPE))) {

	    /* check for terminating character */
	    if (strchr(breakset, edit->input_char)) {
		readinginput = FALSE;
		if (edit->input_char != TAB) {
		    if (edit->input_char == LINEFEED) {
			Printc(RETURN);
		    } else {
			Printc(edit->input_char);
		    }
		}
	    } else {
		if (edit->input_char == CONTROL_C) {
		    *lastchar = edit->input_char;
		    *input_buffer = '\0';
		    *(edit->input_buffer) = '\0';
		    edit->history->current = edit->history->length;
		    if (stdio->statbits & CTRLC_ENBL) {
			stdio->statbits |= CONTROLC;  /* pop out of setup */
		    }
		    LOG_SYNC_REPRINT_END(stdio);
		    return(FALSE);
		} else {
		    if ((edit->main_functions[edit->input_char])(edit) ==
			FALSE) {
			BEEP;
		    }
		}
	    }
	} else {	/* if !(edit->flags & ESCAPE_CHAR)  */
	    if (edit->flags & ESCAPE_CHAR) {
		if ((edit->escape_functions[edit->input_char])(edit) ==
		    FALSE) {
		    BEEP;
		}
		edit->flags &= ~ESCAPE_CHAR;
	    } else {	/* flags & ESCAPE_CHAR && flags & TERMINAL_ESCAPE */
		/* just in case TERMINAL_ESCAPE was set but not used */
		edit->flags &= ~TERMINAL_ESCAPE;
	    }
	}

	TI_display(edit);

    }/* end while (readinginput == TRUE) */
    *lastchar = edit->input_char;

    strcpy(edit->buffer + strlen(edit->buffer), edit->end);
    strcpy(edit->input_buffer, edit->buffer);

    if (add_to_history && (*lastchar != '?') && (*lastchar != '\t')) {
	TI_add_history(edit, edit->buffer);
    }
    LOG_SYNC_REPRINT_END(stdio);
    return(TRUE);
}

/*
 * reprint_prompt
 * This function is called from "getcwait()".
 *
 * In "tt_soc structure", 
 *    log_sync_reprint_msg contains the message to be printed.
 *
 *    if log_sync_reprint_type ==
 *      LOG_SYNC_RPRNT_READLINE, means use readline() style to reprint prompt.
 *      LOG_SYNC_RPRNT_BUFFER, means string in log_sync_reprint_msg should
 *                             be printed out.
 */
void reprint_prompt (tt_soc *tty)
{

    /* if feature is not turned on, don't do anything */
    if (!tty->log_sync)
        return;
    if (tty->log_sync_reprint_type == LOG_SYNC_RPRNT_BUFFER) {
	ttyprintf(tty, "%s", tty->log_sync_reprint_msg);
	ttyflush(tty);
    } else if (tty->log_sync_reprint_type == LOG_SYNC_RPRNT_READLINE ) {
	texti_struct *edit = &(tty->texti_info);
	tt_soc *curtty = stdio;
	stdio = tty;
	TI_retype_line(edit);
	TI_display(edit);
	ttyflush(tty);
	stdio = curtty;
    }

}
static void TI_texti_init (texti_struct *edit, const char *prompt_fmt,
			   va_list prompt_args)
{
    edit->input_char = 0;
    edit->start = edit->buffer;
    *edit->start = '\0';

    edit->end = edit->buffer + (edit->max_length - 1);
    *edit->end = '\0';

    edit->length = 0;
    edit->prompt_fmt = prompt_fmt;
    edit->prompt_args = prompt_args;
    if (edit->prompt_fmt) {
	edit->prompt_len = _doprnt(NULL, NULLTTY, prompt_fmt,
				   prompt_args, FALSE);
    } else {
	edit->prompt_len = 0;
    }
    edit->flags = 0;
    edit->left_edge = edit->buffer;
    edit->where = 0;
    if (stdio->tty_editingon && SCREEN_WIDTH &&
	(SCREEN_WIDTH > (edit->prompt_len + 1 + 2*SCROLL_JUMP))) {
	edit->width = SCREEN_WIDTH - edit->prompt_len - 1;
    } else {
	/*
	 * prompt is too big, or width is too small, or
	 * editing has been turned off, so turn off scrolling
	 */
	edit->width = MAXINT;
    }

    edit->scroll_jump = SCROLL_JUMP;

    edit->flags &= ~KILLED_BEFORE;
    edit->flags &= ~YANKED_BEFORE;

    edit->old_buffer[0] = '\0';
    edit->old_cursor = 0;

    edit->where = 0;
    
    /*
     * See if history was turned on of off
     */
    if (edit->history->historyon != stdio->tty_historyon) {
	if (stdio->tty_historyon) {
	    history_on();
	} else {
	    history_off();
	}
    }

    /*
     * See if history size changed
     */
    if (edit->history->size != stdio->tty_historysize) {
	history_size(stdio->tty_historysize);
    }

    /*
     * See if editing should be on or off
     */
    if (stdio->tty_editingon) {
	edit->main_functions = editing_functions;
	edit->escape_functions = escape_editing_functions;
    } else {
	edit->main_functions = non_editing_functions;
	edit->escape_functions = escape_non_editing_functions;
    }

    if (edit->prompt_len) {
	printc('\n');
	vprintf(edit->prompt_fmt, edit->prompt_args);
    } else {
	if (edit->input_buffer[0]) {
	    printc('\n');
	}
    }
}

/*****************************************************************************/
/*************************   Editing Primitives  *****************************/
/*****************************************************************************/

static int TI_forward_space_char (register texti_struct *edit)
{
    if (*(edit->end) == '\0') {
	return(FALSE);
    }

    edit->where += TI_char_len(*edit->end);
    *((edit->start)++) = *((edit->end)++);
    *edit->start = '\0';

    TI_scroll(edit);

    return(TRUE);
}

static int TI_back_space_char (register texti_struct *edit)
{
    /* check to see if at beginning of line */
    if (edit->start == edit->buffer) {
	return(FALSE);
    }

    *(--(edit->end)) = *(--(edit->start));
    *edit->start = '\0';
    edit->where -= TI_char_len(*edit->end);

    TI_scroll(edit);

    return(TRUE);
}

static int TI_insert_char (register texti_struct *edit)
{
    /* check for line that is too long */
    if (edit->length >= (edit->max_length - 2)) {
	return(FALSE);
    }
    edit->length++;

    /* insert character */
    *((edit->start)++) = edit->input_char;
    *(edit->start) = '\0';
    edit->where += TI_char_len(edit->input_char);

    TI_scroll(edit);

    return(TRUE);
}

static int TI_delete_char (register texti_struct *edit)
{
    /* check for existence of character to delete */
    if (*edit->end == '\0') {
	return(FALSE);
    }

    /* delete character */
    edit->length--;
    edit->end++;

    return(TRUE);
}

/******************************************************************************/
/******************************    Insertions  ********************************/
/******************************************************************************/


static int TI_insert_string (register texti_struct *edit,
			     register char *line, int flag)
{
    if (flag == STRING_REPLACE) {
	while (TI_back_space_char(edit)) {
	    /* Null body */
	}
	while (TI_delete_char(edit)) {
	    /* Null body */
	}
    } else {
	/* flag == STRING_INSERT */
    }
		
    while (*line != '\0') {
	edit->input_char = *line;
	if (! TI_insert_char(edit)) {
	    return(FALSE);
	}
	line++;
    }
    return(TRUE);
}

static int TI_tab (texti_struct *edit)
{
#ifdef	NO_TAB
    if (!(edit->flags & TERMINAL_ESCAPE)) {
	/* I know, I'm cheating, so sue me */
	edit->input_char = ' ';
    }
#endif	NO_TAB
    return(TI_insert_char(edit));
}

static int TI_retype_line (texti_struct *edit)
{
    if (NOECHO == FALSE) {
	printc('\n');
	if (edit->prompt_len) {
	    vprintf(edit->prompt_fmt, edit->prompt_args);
	}
    }

    edit->old_buffer[0] = '\0';
    edit->old_cursor = 0;

    return(TRUE);
}

/*****************************************************************************/
/*****************************     Movement    *******************************/
/*****************************************************************************/

static int TI_move_to_bol (register texti_struct *edit)
{
    while (edit->start != edit->buffer) {
	TI_back_space_char(edit);
    }
    return(TRUE);
}

static int TI_move_to_eol (register texti_struct *edit)
{
    while (*(edit->end) != '\0') {
	TI_forward_space_char(edit);
    }
    return(TRUE);
}

static int TI_back_space_word (register texti_struct *edit)
{
    while ((edit->start > edit->buffer) &&
	   (strchr(edit->word_sep, *(edit->start-1)) != NULL)) {
	TI_back_space_char(edit);
    }
    while ((edit->start > edit->buffer) &&
	   (strchr(edit->word_sep, *(edit->start-1)) == NULL)) {
	TI_back_space_char(edit);
    }
    return(TRUE);
}

static int TI_forward_space_word (register texti_struct *edit)
{
    while ((*edit->end != '\0') &&
	   (strchr(edit->word_sep, *edit->end) != NULL) &&
	  TI_forward_space_char(edit)) {
	/* Null Body */
    }
    while ((*edit->end != '\0') &&
	   (strchr(edit->word_sep, *edit->end) == NULL) && 
	   TI_forward_space_char(edit)) {
	/* Null Body */
    }
    return(TRUE);
}

/******************************************************************************/
/****************************     Deletions    ********************************/
/******************************************************************************/

static int TI_back_space_delete_char (texti_struct *edit)
{
    if (TI_back_space_char(edit) == FALSE) {
	return(FALSE);
    }
    TI_delete_char(edit);
    return(TRUE);
}

static int TI_kill_to_eol (register texti_struct *edit)
{
    if (edit->flags & KILLED_BEFORE) {
	TI_add_kill(edit, edit->end, APPEND);
    } else {
	TI_add_kill(edit, edit->end, NEW);
    }

    /* erase end from buffer */
    while (TI_delete_char(edit)) {
	/* Null Body */
    }
    return(TRUE);
}

static int TI_kill_to_bol (register texti_struct *edit)
{
    if (edit->start == edit->buffer) {
	return(TRUE);
    }

    if (edit->flags & KILLED_BEFORE) {
	TI_add_kill(edit, edit->buffer, PREPEND);
    } else {
	TI_add_kill(edit, edit->buffer, NEW);
    }
    edit->flags |= JUST_KILLED;
    while (TI_back_space_delete_char(edit)) {
	/* Null Body */
    }
    return(TRUE);
}

static int TI_back_word_delete (register texti_struct *edit)
{
    edit->kill_pointer = edit->kill_buffer + edit->max_length - 1;
    *(edit->kill_pointer) = '\0';

    /* back up over whitespace */
    while ((edit->start > edit->buffer) &&
	   (strchr(edit->word_sep, *(edit->start-1)) != NULL)) {
	TI_back_space_char(edit);
	*(--(edit->kill_pointer)) = *(edit->end);
	TI_delete_char(edit);
    }
    /* back up over word */
    while ((edit->start > edit->buffer) &&
	   (strchr(edit->word_sep, *(edit->start-1)) == NULL)) {
	TI_back_space_char(edit);
	*(--(edit->kill_pointer)) = *(edit->end);
	TI_delete_char(edit);
    }
    if (edit->flags & KILLED_BEFORE) {
	TI_add_kill(edit, edit->kill_pointer, PREPEND);
    } else {
	TI_add_kill(edit, edit->kill_pointer, NEW);
    }
    edit->flags |= JUST_KILLED;

    return(TRUE);
}

static int TI_delete_word (register texti_struct *edit)
{
    edit->kill_pointer = edit->kill_buffer;
    while ((*edit->end != '\0') &&
	   (strchr(edit->word_sep, *edit->end) != NULL)) {
	*(edit->kill_pointer++) = *edit->end;
	TI_delete_char(edit);
    }
    while ((*edit->end != '\0') &&
	   (strchr(edit->word_sep, *edit->end) == NULL)) {
	*(edit->kill_pointer++) = *edit->end;
	TI_delete_char(edit);
    }
    *(edit->kill_pointer) = '\0';
    if (edit->flags & KILLED_BEFORE) {
	TI_add_kill(edit, edit->kill_buffer, APPEND);
    } else {
	TI_add_kill(edit, edit->kill_buffer, NEW);
    }
    edit->flags |= JUST_KILLED;
    return(TRUE);
}

/******************************************************************************/
/**********************     Miscellaneous Editing    **************************/
/******************************************************************************/


static int TI_capitalize_word (register texti_struct *edit)
{
    /* move over whitespace */
    while ((*edit->end != '\0') &&
	   (strchr(edit->word_sep, *edit->end) != NULL) &&
	   TI_forward_space_char(edit)) {
	/* Null Body */
    }

    if (*(edit->end) == '\0') {
	return(TRUE);
    }

    /* Uppercase first letter */
    *(edit->end) = toupper(*(edit->end));
    TI_forward_space_char(edit);

    /* Lowercase remaining letters in word */
    while ((*edit->end != '\0') &&
	   (strchr(edit->word_sep, *edit->end) == NULL)) {
	/* convert to lower case */
	*(edit->end) = tolower(*(edit->end));
	TI_forward_space_char(edit);
    }
    return(TRUE);
}

static int TI_lowercase_word (register texti_struct *edit)
{
    /* move over whitespace */
    while ((*edit->end != '\0') &&
	   (strchr(edit->word_sep, *edit->end) != NULL) &&
	   TI_forward_space_char(edit)) {
	/* Null Body */
    }

    /* while not whitespace */
    while ((*edit->end != '\0') &&
	   (strchr(edit->word_sep, *edit->end) == NULL)) {
	/* convert to lowercase */
	*(edit->end) = tolower(*(edit->end));
	TI_forward_space_char(edit);
    }
    return(TRUE);
}

static int TI_uppercase_word (register texti_struct *edit)
{
    /* move over whitespace */
    while ((*edit->end != '\0') &&
	   (strchr(edit->word_sep, *edit->end) != NULL) &&
	   TI_forward_space_char(edit)) {
	/* Null Body */
    }

    /* while not whitespace */
    while ((*edit->end != '\0') &&
	   (strchr(edit->word_sep, *edit->end) == NULL)) {
	/* convert to lowercase */
	*(edit->end) = toupper(*(edit->end));
	TI_forward_space_char(edit);
    }
    return(TRUE);
}

static int TI_transpose_char (texti_struct *edit)
{
    char ch;

    if (*(edit->end) == '\0') {
	if (TI_back_space_char(edit) == FALSE) {
	    return(FALSE);
	}
    }
    if (TI_back_space_char(edit) == FALSE) {
	if (*(edit->end + 1) == '\0') {
	    TI_forward_space_char(edit);
	}
	return(FALSE);
    }

    ch = *(edit->end);
    *(edit->end) = *(edit->end + 1);
    *(edit->end + 1) = ch;

    TI_forward_space_char(edit);
    TI_forward_space_char(edit);
	
    return(TRUE);
}

static int TI_escape (texti_struct *edit)
{
    edit->flags |= ESCAPE_CHAR;
    /*
     * save the value of edit->flags,
     * because escape sequences are 2 characters
     */
    if (edit->flags & KILLED_BEFORE) {
	edit->flags |= JUST_KILLED;
    }
    if (edit->flags & YANKED_BEFORE) {
	edit->flags |= JUST_YANKED;
    }
    return(TRUE);
}

static int TI_quoted_insert (texti_struct *edit)
{
    edit->flags |= TERMINAL_ESCAPE;
    return(TRUE);
}

static int TI_arrow (texti_struct *edit)
{
    edit->input_char = getcwait(stdio);
    switch (edit->input_char) {
      case 'A':	/* up arrow */
	return((arrow_functions.up_arrow)(edit));
	break;
      case 'B':	/* down arrow */
	return((arrow_functions.down_arrow)(edit));
	break;
      case 'C':	/* right arrow */
	return((arrow_functions.right_arrow)(edit));
	break;
      case 'D':	/* left arrow */
	return((arrow_functions.left_arrow)(edit));
	break;
      default:
	return(FALSE);
    }   /* switch edit->input_char for ansii arrow */
    return(FALSE);
}


static int TI_INVALID_CHAR (texti_struct *edit)
{
    return(FALSE);
}


/******************************************************************************
 ******************************************************************************
 *                                                                            *
 *                       Init & Reset Functions                               *
 *                                                                            *
 ******************************************************************************
 ******************************************************************************/

void texti_init (tt_soc *tty)
{
    texti_struct	*edit = &(tty->texti_info);

    edit->kill_ring = NULL;
    edit->kill_current = 0;
    edit->kill_length = 0;
    edit->kill_max = MAX_KILL;

    edit->exec_history.buffer = NULL;
    edit->exec_history.current = 0;
    edit->exec_history.length = 0;
    edit->exec_history.counter = 0;
    edit->exec_history.historyon = tty->tty_historyon;
    edit->exec_history.size = tty->tty_historysize;

    edit->config_history.buffer = NULL;
    edit->config_history.current = 0;
    edit->config_history.length = 0;
    edit->config_history.counter = 0;
    edit->config_history.historyon = tty->tty_historyon;
    edit->config_history.size = tty->tty_historysize;

    edit->history = &(edit->exec_history);

    edit->old_buffer[0] = '\0';
    edit->old_cursor = 0;


    strcpy(edit->word_sep, "- .");
}

void texti_reset (tt_soc *tty)
{
    TI_free_history(&(tty->texti_info.exec_history));
    TI_free_history(&(tty->texti_info.config_history));
    TI_free_kill(tty);

    texti_init(tty);
}

/******************************************************************************
 ******************************************************************************
 *                                                                            *
 *                       Kill & Yank Functions                                *
 *                                                                            *
 ******************************************************************************
 ******************************************************************************/

static int TI_yank (texti_struct *edit)
{
    register char *cp;

    edit->kill_pointer = (char *) TI_rotate_ring(edit, NEW);
    if (edit->kill_pointer == FALSE) {
	return(FALSE);
    } else {
	strcpy(edit->kill_buffer, edit->kill_pointer);
	cp = edit->kill_pointer;
	edit->flags |= JUST_YANKED;
	while (*cp != '\0') {
	    edit->input_char = *cp;
	    if (! TI_insert_char(edit)) {
		return(FALSE);
	    }
	    cp++;
	}
    }
    return(TRUE);
}

static int TI_rotate_yank (register texti_struct *edit)
{
    if (edit->flags & YANKED_BEFORE) {
	edit->flags |= JUST_YANKED;
    } else {
	return(FALSE);
    }

    edit->kill_pointer = edit->kill_buffer;
    while (*(edit->kill_pointer) != '\0') {
	edit->kill_pointer++;
	TI_back_space_delete_char(edit);
    }

    edit->kill_pointer = (char *) TI_rotate_ring(edit, OLD);
    strcpy(edit->kill_buffer, edit->kill_pointer);
    return(TI_insert_string(edit, edit->kill_pointer, STRING_INSERT));
}

static void TI_add_kill (texti_struct *edit, char *line, int flag)
{
    /* flag == 0			create new kill buffer
       flag == 1			append to end of current kill buffer
          Ctrl-k
	  Esc-d
       flag == 2			prepend to end of current kill buffer
          Ctrl-u
	  Ctrl-w
	  Esc-Delete
     */
    int	counter = 0;
    char	*cp;

    if ((NOECHO == TRUE) || (*line == '\0') || (!stdio->tty_editingon)) {
	return;
    }

    if (edit->kill_ring == NULL) {
	edit->kill_ring = (char **) MALLOC((sizeof(char *) * edit->kill_max));
	if (edit->kill_ring == NULL) {
	    return;
	}
    }

    switch (flag) {
      /* if edit->kill_ring is NEW */
      case NEW:
	/* if edit->kill_ring is full */
	if (edit->kill_length >= edit->kill_max) {
	    /* free first edit->kill_buffer */
	    FREE(edit->kill_ring[0]);

	    for (counter = 1;
		 counter <= edit->kill_max;
		 counter++) {
		edit->kill_ring[counter-1] = edit->kill_ring[counter];
	    }
	    edit->kill_length--;
	}
	edit->kill_ring[edit->kill_length] =
	    (char *) MALLOC(sizeof(char) * (1 + strlen(line)));
	if (edit->kill_ring[edit->kill_length] != NULL) {
	    strcpy(edit->kill_ring[edit->kill_length], line);
	    edit->kill_current = ++edit->kill_length;
	}
	break;

      case APPEND:
	edit->kill_length--;
	cp = (char *) MALLOC(sizeof(char) *
			     (1 + strlen(line) +
			      strlen(edit->kill_ring[edit->kill_length])));
	if (cp == NULL) {
	    return;
	}
	sprintf(cp, "%s%s", edit->kill_ring[edit->kill_length], line);
	FREE(edit->kill_ring[edit->kill_length]);
	edit->kill_ring[edit->kill_length] = cp;
	edit->kill_length++;
	break;

      case PREPEND:
	edit->kill_length--;

	cp = (char *) MALLOC(sizeof(char) *
			     (1 + strlen(line) +
			      strlen(edit->kill_ring[edit->kill_length])));
	if (cp  == NULL) {
	    return;
	}
	sprintf(cp, "%s%s", line, edit->kill_ring[edit->kill_length]);
	FREE(edit->kill_ring[edit->kill_length]);
	edit->kill_ring[edit->kill_length] = cp;
	edit->kill_length++;
	break;
    }
}

static char *TI_rotate_ring (texti_struct *edit, int flag)
{
    if (edit->kill_ring == NULL) {
	return(FALSE);
    } else {
	switch (flag) {
	  case NEW:
	    edit->kill_current = edit->kill_length - 1;
	    return(edit->kill_ring[edit->kill_current]);
	  case OLD:
	    if (edit->kill_current == 0) {
		edit->kill_current = edit->kill_length - 1;
	    } else {
		edit->kill_current--;
	    }
	    return(edit->kill_ring[edit->kill_current]);
	  default:
	    return(FALSE);
	}
    }
}

static void TI_free_kill (tt_soc *tty)
{
    texti_struct *edit = &(tty->texti_info);
    register int counter = 0;

    if (edit->kill_ring != NULL) {
	while (counter < edit->kill_length) {
	    FREE(edit->kill_ring[counter]);
	    counter++;
	}
	FREE(edit->kill_ring);
    }
}


/******************************************************************************
 ******************************************************************************
 *                                                                            *
 *                           History Functions                                *
 *                                                                            *
 ******************************************************************************
 ******************************************************************************/

static int TI_next_history (texti_struct *edit)
{
    char *cp;

    if ((cp = (char *) TI_get_next_history(edit)) != NULL) {
	(edit->history->used)--;
	return(TI_insert_string(edit, cp, STRING_REPLACE));
    } else {
	if (edit->history->used != 0) {
	    (edit->history->used)--;
	    return(TI_insert_string(edit, edit->input_buffer, STRING_REPLACE));
	} else {
	    return(FALSE);
	}
    }
}

static int TI_prev_history (texti_struct *edit)
{
    char *cp;

    if (edit->history->used == 0) {
	strcpy(edit->input_buffer, edit->buffer);
	strcpy(edit->input_buffer + strlen(edit->input_buffer), edit->end);
    }
    if ((cp = (char *) TI_get_prev_history(edit)) != NULL) {
	(edit->history->used)++;
	return(TI_insert_string(edit, cp, STRING_REPLACE));
    } else {
	return(FALSE);
    }
}

static int TI_beginning_of_history (texti_struct *edit)
{
    char *tmp1;
    char *tmp2 = NULL;

    if (edit->history->used == 0) {
	strcpy(edit->input_buffer, edit->buffer);
	strcpy(edit->input_buffer + strlen(edit->input_buffer), edit->end);
    }

    while ((tmp1 = (char *) TI_get_prev_history(edit)) != NULL) {
	tmp2 = tmp1;
	(edit->history->used)++;
    }
    if (tmp2 == NULL) {
	return(FALSE);
    }
    return(TI_insert_string(edit, tmp2, STRING_REPLACE));
}

static int TI_end_of_history (texti_struct *edit)
{
    char *cp;

    if (edit->history->used == 0) {
	return(FALSE);
    }
    while ((cp = (char *) TI_get_next_history(edit)) != NULL) {
	(edit->history->used)--;
    }
    (edit->history->used)--;
    return(TI_insert_string(edit, edit->input_buffer, STRING_REPLACE));
}

static int TI_add_history (texti_struct *edit, register char *line)
{
    int counter = 0;
    texti_history_struct *h = edit->history;

    /* return if history is off */
    if ((!h->historyon) ||
	   (NOECHO == TRUE) ||
	   (h->size <= 0)) {
	return(FALSE);
    }

    /* make sure line isn't blank */
    counter = 0;
    while (line[counter] == ' ') {
	counter++;
    }
    if (line[counter] == '\0') {
	return(FALSE);
    }

    /*
     * if histoy size hasn't been called yet,
     *  malloc history history buffer
     */
    if (h->buffer == NULL) {
	h->buffer = (char **) MALLOC(sizeof(char *) * h->size);
	/* Turn off history and return if this fails */
	if (h->buffer == NULL) {
	    history_off();
	    Printf("\nUnable to malloc history buffer");
	    Printf("\nDisabling history");
	    return(FALSE);
	}
    } else {
	/*
	 * Check to see if this is a duplicate of the previous command
	 */
	if ((h->length > 0) && (strcmp(h->buffer[h->length-1], line) == 0)) {
	    /* Duplicate command, so reset current and return */
	    h->current = h->length;
	    return(TRUE);
	}
    }

    /* if history is full */
    /* history shouldn't be > edit->history->size, but just in case */
    if (h->length >= h->size) {

	/* free first history */
	FREE(h->buffer[0]);
		
	/* move each history pointer down one */
	for (counter = 1; counter <= h->size; counter++) {
	    h->buffer[counter-1] = h->buffer[counter];
	}

	/* subtract 1 from the length */
	h->length--;
    }

    /* increment history counter */
    h->counter++;

    /* malloc buffer and add history to end */
    h->buffer[h->length] = (char *) MALLOC(sizeof(char) * (1 + strlen(line)));
    if (h->buffer[h->length] == NULL) {
	/* can't do malloc, so just forget about it */
	return(FALSE);
    } else {
	strcpy(h->buffer[h->length],line);
	h->length += 1;
	h->current = h->length;
	return(TRUE);
    }
}
		
static char *TI_get_next_history (texti_struct *edit)
{
    edit->history->current++;
    if ((!edit->history->historyon) ||
	(edit->history->current >= edit->history->length)) {
	edit->history->current = edit->history->length;
	return(NULL);
    } else {
	return(edit->history->buffer[edit->history->current]);
    }
}

static char *TI_get_prev_history (texti_struct *edit)
{
    if ((!edit->history->historyon) ||
	(edit->history->current <= 0)) {
	return(NULL);
    } else {
	return(edit->history->buffer[--edit->history->current]);
    }
}

void print_history (parseinfo *csb)
{
    texti_struct *edit=&(stdio->texti_info);
    int counter = 0;

    if (edit->history->historyon) {
	while (counter < edit->history->length) {
	    Printf("\n  %s", edit->history->buffer[counter++]);
	}
    }
}

static void TI_free_history (texti_history_struct *history)
{
    int counter = 0;

    if (history->buffer != NULL) {
	while (counter < history->length) {
	    FREE(history->buffer[counter]);
	    counter++;
	}
	FREE(history->buffer);
    }
}

static void history_off (void)
{
    stdio->texti_info.history->historyon = FALSE;
}

static void history_on (void)
{
    stdio->texti_info.history->historyon = TRUE;
}

static void TI_history_size (texti_history_struct *history, int new_size)
{
    char **new_history_buffer;
    int counter;

    if (new_size < 0) {
	return;
    }

    /* same size */
    if (new_size == history->size) {
	return;
    }

    /* new history buffer */
    if (history->buffer == NULL) {
	history->buffer = (char **) MALLOC(sizeof(char *) * new_size);
	/* if malloc fails, let user know and then just
	   pretend this never happened */
	if (history->buffer == NULL) {
	    history_off();
	    Printf("\nUnable to malloc history buffer");
	    Printf("\nDisabling history");
	} else {
	    history->size = new_size;
	}
	return;
    }

    /* new_size > current length of history, so just add (or delete)
     * onto the end */
    if (new_size >= history->length) {
	new_history_buffer = (char **) MALLOC(sizeof(char *) * new_size);

	if (new_history_buffer != NULL) {
	    bcopy(history->buffer, new_history_buffer,
		  sizeof(char *) * new_size);
	    FREE(history->buffer);
	    history->buffer = new_history_buffer;
	    history->size = new_size;
	}
    } else {		/* new_size < history->length */
	new_history_buffer = (char **) MALLOC(sizeof(char *) * new_size);

	if (new_history_buffer || (new_size == 0)) {
	    for (counter = 0;
		 counter < (history->length-new_size);
		 counter++) {
		FREE(history->buffer[counter]);
	    }
	    for (counter=0; counter < new_size; counter++) {
		history->buffer[counter] = 
		    history->buffer[counter + history->length - new_size];
	    }
	    history->size = new_size;
	    history->length = new_size;
	    history->current = new_size;
	    bcopy(history->buffer, new_history_buffer,
		  sizeof(char *) * new_size);
	    FREE(history->buffer);
	    history->buffer = new_history_buffer;
	}
    }
}

static void history_size (int new_size)
{
    TI_history_size(&(stdio->texti_info.exec_history),new_size);
    TI_history_size(&(stdio->texti_info.config_history),new_size);
}

/*******************************************/
/************* Display Routines ************/
/*******************************************/

#define	SCROLL_CHAR	'$'

static void TI_display (texti_struct *edit)
{
    char *new_line;
    register char *cp;
    char str[9];

    new_line = edit->new_buffer;
    new_line[0] = '\0';
    cp = edit->left_edge;
    while (*cp != '\0') {
	GET_CHAR_STRING(*cp, str);
	strcat(new_line, str);
	cp++;
    }
    cp = edit->end;
    while (*cp != '\0') {
	GET_CHAR_STRING(*cp, str);
	strcat(new_line, str);
	cp++;
    }

    if (strlen(new_line) > edit->width) {
	/* The end is scrolled of the edge of the screen */
	new_line[edit->width] = '\0';
	new_line[edit->width-1] = SCROLL_CHAR;
    }
    if (edit->left_edge != edit->buffer) {
	new_line[0] = SCROLL_CHAR;
    }

    TI_replace_line(edit->old_buffer, new_line, edit->old_cursor, edit->where);

    strcpy(edit->old_buffer, new_line);
    edit->old_cursor = edit->where;
}

static void TI_replace_line (char *old_line, char *new_line,
			     int start_cursor, int end_cursor)
{
    register char *old = old_line;
    register char *new = new_line;
    int from_cursor;
    int to_cursor;

    from_cursor = start_cursor;
    to_cursor = 0;
    while ((*old != '\0')  &&  (*new != '\0')) {
	if (*old != *new) {
	    TI_move_cursor(to_cursor, &from_cursor, new_line);
	    Printc(*new);
	    from_cursor++;
	}
	old++;
	new++;
	to_cursor++;
    }
    while (*new != '\0') {
	TI_move_cursor(to_cursor, &from_cursor, new_line);
	Printc(*new);
	new++;
	to_cursor++;
	from_cursor++;
    }
    while (*old != '\0') {
	TI_move_cursor(to_cursor, &from_cursor, new_line);
	Putc(' ');
	old++;
	to_cursor++;
	from_cursor++;
    }
    TI_move_cursor(end_cursor, &from_cursor, new_line);
}
		

static void TI_move_cursor (register int to, register int *from, char *line)
{
    register char *cp;
    int i;

    if (to == *from) {
	return;
    }
    if (to < *from) {
	while (to < *from) {
	    Putc('\b');
	    (*from)--;
	}
    } else {
	/* to > from */
	cp = line;
	for (i=0; i < *from; i++) {
	    cp++;
	}
	while (to > *from) {
	    Printc(*cp);
	    cp++;
	    (*from)++;
	}
    }
    return;
}

static void TI_scroll (register texti_struct *edit)
{
    char *scroll_line;
    char *left_edge;
    int len;
    register int i;

    if (edit->width == MAXINT) {
	/*
	 *  Scrolling has been turned off
	 */
	return;
    }
    if ((edit->where > 0) && (edit->where < (edit->width - 1))) {
	/*
	 *  We haven't stepped over the boundaries,
	 *  so there's no sense in wasting our time
	 */
	return;
    }
    scroll_line = edit->new_buffer;
    scroll_line[0] = '\0';
    strcpy(scroll_line, edit->buffer);
    strcat(scroll_line, edit->end);
    left_edge = scroll_line + (edit->left_edge - edit->buffer);

    /* scroll left */
    while ((edit->where <= 0)  &&  (left_edge != scroll_line)) {
	i = 0;
	while (i < edit->scroll_jump) {
	    if (left_edge == scroll_line) {
		break;
	    }
	    edit->left_edge--;
	    left_edge--;
	    len = TI_char_len(*left_edge);
	    edit->where += len;
	    i += len;
	}
    }

    /* scroll right */
    while (edit->where >= (edit->width - 1)) {
	i = 0;
	while (i < edit->scroll_jump) {
	    len = TI_char_len(*left_edge);
	    edit->left_edge++;
	    left_edge++;
	    edit->where -= len;
	    i += len;
	}
    }
}

static int TI_char_len (register char ch)
{
    if (ch == TAB) {
	return(8);
    }
    if (((uchar)ch < 32)  ||  (ch == 127)) {
	return(2);
    }
    return(1);
}

static void TI_free (void **pointer)
{
    free(*pointer);
    *pointer = NULL;
}


/*
 * rdtty
 * Read a line with some editing provided
 */

boolean rdtty (char *b, register int maxline)
{
    char dummy;

    *b = '\0';	/* Reset the buffer before calling readline() */

    /* use general routine */
    return(readline(b, maxline, "\r\n", &dummy, TRUE, ""));
}

/*
 * generic_servicehook - Set the service hook
 */

boolean
generic_servicehook(tt_soc *tty, txserv_t txhook, rxserv_t rxhook)
{
    tty->txservice = txhook;
    tty->rxservice = rxhook;
    return(TRUE);
}

/*
 * generic_noservicehook - Clear the service hook
 */

void generic_noservicehook (tt_soc *tty)
{
    tty->txservice = NULL;
    tty->rxservice = NULL;
}

/*
 * serial_setbuffers
 * Perform buffer set up functions. 
 * Can be used to change buffer sizes -- interlocks and resets pointers.
 */

void serial_setbuffers (tt_soc *tty, int insize, int outsize)
{
    leveltype status;

    status = raise_interrupt_level(TTY_DISABLE);

    /*
     * Release current input buffer, if any, and set a new one, if any.
     */
    free(tty->instart);
    tty->instart = NULL;
    tty->insize = insize;
    if (insize > 0)
	tty->instart = malloc_named(insize, "TTY Input Buf");
    serial_clearinput(tty);

    /*
     * Release current output buffer, if any, and set a new one, if any.
     */
    free(tty->outstart);
    tty->outstart = NULL;
    tty->outsize = outsize;
    if (outsize > 0)
	tty->outstart = malloc_named(outsize, "TTY Output Buf");
    clearoutput(tty);
    reset_interrupt_level(status);
}

/*
 * helper_daemonBLOCK - Scheduler predicate for input helper daemon
 */

static boolean helper_daemonBLOCK (tt_soc *tty)
{
    if (inputpending(tty))
      return(FALSE);		/* Don't block if input pending */

    if (tty->helper_outready && !outputblock(tty))
      return(FALSE);		/* Don't block if output pending */

    return(TRUE);		/* Nothing to do right now */
}

/*
 * helper_daemon - Input helper process
 *
 * The helper_daemon process is used to run the rxservice and txservice
 * entry points for drivers which can not implement it for themselves.
 * This is used for telnet, for example, which needs to reply to messages
 * as it receives them. Since a connection could have a full window, it
 * isn't possible to respond to them from the TCP process (the process
 * that delivers the data).
 */

static forktype helper_daemon (tt_soc *tty)
{

    while (TRUE) {

	/*
	 * Wait for something to do
	 */

	edisms((blockproc *)helper_daemonBLOCK, (ulong)tty);
	rx_tx_handler(tty);
    }
}

/*
 * helper_servicehook - Create helper process and set service hook
 */
boolean helper_servicehook (tt_soc *tty, txserv_t txhook, rxserv_t rxhook)
{
    /*
     * If we do not have a helper process, create one. If there already is
     * one, we don't need to create a new one.
     */
    if (!tty->helper) {
	tty->helper = cfork((forkproc *) helper_daemon, (long)tty, 800,
			    "Input Helper", tty->ttynum);
	if (!tty->helper)
	  return(FALSE);
	process_set_crashblock(tty->helper, TRUE);
    }
    return(generic_servicehook(tty, txhook, rxhook));
}

/*
 * helper_noservicehook - Terminate helper process and clear service hook
 */

void helper_noservicehook (tt_soc *tty)
{
    if (tty->helper) {
	process_kill(tty->helper);
	tty->helper = 0;
    }
    generic_noservicehook(tty);
}
#define TX_BYTE_LIMIT 400             /* Limit on number of bytes to send */
/*
 * rx_tx_handler
 * A hanlder function to manage the rxservice and txservice entry points.
 */
void rx_tx_handler (tt_soc *vty)
{
    int c;
    uchar buf[64];
    uint len;
    /*
     * Manage the rxservice entry point. While there is input pending,
     * pass it to the rxservice hook.
     * This should process data from a VTY connection running XRemote, PAD
     * LAT, TCP, ...etc and pass it to the rxservice (SLIP, PPP, ARAP, ..).
     */
    if (vty->rxservice) {
	if (vty->conntop) {
	    GET_TIMESTAMP(vty->conntop->idletime);
	    if (vty->tty_sessiontimeout) {
		start_sessiontimeout(vty->conntop, vty);
	    }
	}
	/*
	 * Gather a buffer full at a time, then feed it through
	 * the handler in bulk.
	 */
	len = 0;
	while ((c = getc(vty)) >= 0) {
	    buf[len++] = c;
	    if (len == sizeof(buf)) {
		(*(vty->rxservice))(vty, buf, sizeof(buf));
		len = 0;
	    }
	}
	if (len > 0) {
	    (*(vty->rxservice))(vty, buf, len);
	}
    }
    
    /*
     * Manage the txservice entry point. Stuff output as long as we
     * can get it from the txservice handler.
     * This will move data from the txservice handler and pass it to a VTY  
     */
    if (vty->txservice) {
	uint x;
	boolean output_to_send = FALSE;
	int limit;

	if (vty->conntop && vty->tty_sessiontimeout &&
	    (vty->capabilities & OUTIDLE)) {
	    start_sessiontimeout(vty->conntop, vty);
	}
	/* 
	 * Call outputblock() only once per output packet, leaving putc()
	 * to block and unblock when necessary.
	 */
	if (!outputblock(vty)) {
	    if (vty->type == AUX_LINE) {
		char c2;
		
		/*
		 * The aux port requires a check to ensure that we will not
		 * be blocked before writing each character.
		 * We stuff output for as long as we can get it from the
		 * txservice handler.
		 */
		do {
		    len = (*(vty->txservice))(vty, &c2, sizeof(c2));
		    if (len == 0) {
			vty->helper_outready = FALSE; /* No more output */
			break;
		    }
		    output_to_send = TRUE;		    
		    putc(vty, c2);
		} while (!outputblock(vty));
	    } else {
		/* Set a limit on the number of bytes to send so we can pause
		 * after approximately 400 bytes of continoues output to 
		 * check and process any pending input.
		 */
		limit = TX_BYTE_LIMIT;
		while (vty->txservice && limit > 0 && 
		       (len = (*(vty->txservice))(vty, buf, sizeof(buf))) >0) {
		    output_to_send = TRUE;
		    limit -= len;
		    for (x = 0; x < len; ++x) {
			putc(vty, buf[x]);
		    }
		}
		vty->helper_outready = FALSE; /* No more output */
	    }
	    /* 
	     * Call startoutput() only when there is output to send
	     */
	    if (output_to_send)
		startoutput(vty);
	}
    }
}

/*
 * tty_signal_helper
 *
 * If we have a transmit service hook, we must signal our helper
 * process. We can not actually do any I/O on the connection here,
 * since that might result in us blocking a shared process.
 *
 * We check to see if the current process is something other than
 * our helper, and if so, just signal the helper.
 */
boolean tty_signal_helper (tt_soc *tty)
{
    pid_t pid;

    process_get_pid(&pid);
    if (tty->txservice && (tty->helper != pid)) {
	tty->helper_outready = TRUE;
	return(TRUE);
    }
    return(FALSE);
}
