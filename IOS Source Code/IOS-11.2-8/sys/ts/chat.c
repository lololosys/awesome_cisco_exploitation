/* $Id: chat.c,v 3.11.4.8 1996/08/28 13:17:58 thille Exp $
 * $Source: /release/112/cvs/Xsys/ts/chat.c,v $
 *------------------------------------------------------------------
 * chat.c - Process chat scripts
 *
 * April 28, 1992, Kevin Paul Herbert
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This module handles the processing of chat scripts, used for establishing
 * dialup connections on asynchronous lines.
 *------------------------------------------------------------------
 * $Log: chat.c,v $
 * Revision 3.11.4.8  1996/08/28  13:17:58  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.11.4.7  1996/08/15  21:54:45  irfan
 * CSCdi62960:  chat scripts mishandle control chars in nvgen
 * Branch: California_branch
 * translate non-printable characters correctly when nvgen'ing.
 * use same translations for debug output.
 * don't trim non-blank characters while looking for delimiters.
 * use malloc_named instead of malloc.
 *
 * Revision 3.11.4.6  1996/08/11  00:13:47  irfan
 * CSCdi64886:  r in chat script doesnt send CR
 * Branch: California_branch
 * Implement \r \n \s \t in chat send strings.
 *
 * Revision 3.11.4.5  1996/08/07  09:03:09  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.11.4.4  1996/07/17  23:03:52  irfan
 * CSCdi62510:  PPP modularity break-requires chat
 * Branch: California_branch
 * kill the deprecated "modem chat-script" line subcommand.
 *
 * Revision 3.11.4.3  1996/05/21  10:07:05  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.11.4.2  1996/04/27  06:28:11  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.4.10.3  1996/04/25  23:50:59  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.4.10.2  1996/04/16  23:45:48  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.4.10.1  1996/02/23  20:59:15  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.11.4.1  1996/03/18  22:19:42  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.3  1996/03/16  07:44:40  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.2.2  1996/03/07  10:57:46  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.1  1996/02/20  21:24:12  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.11  1996/03/09  01:18:06  perryl
 * CSCdi47460:  Crash in bad call from chat_dial to free/validblock
 * use setstring() to free memory for tty->dial_string instead of free.
 * setstring() free the memory and set tty->dial_string to NULL
 *
 * Revision 3.10  1996/02/22  14:39:15  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.9  1996/02/07  16:16:20  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.8  1996/02/03  04:13:10  tkolar
 * CSCdi47986:  IOS modem integration isnt quite there yet
 * Add a way for the IOS to know if the modem switch has reserved the
 * modem.
 *
 * Revision 3.7  1996/02/01  06:10:58  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/29  07:30:35  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.5  1996/01/22  07:29:51  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1995/12/17  18:39:45  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/12/09  00:43:55  tkolar
 * CSCdi45302:  Need a clean interface between modem hardware and
 * commserver
 * Make it easy to claim a tty for exclusive use by a process, and add a
 * modem_busyout registry.
 *
 * Revision 3.2  1995/11/17  18:54:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.11  1995/11/08  21:33:02  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.10  1995/07/14  07:03:12  hampton
 * Convert Terminal Server to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi37093]
 *
 * Revision 2.9  1995/06/28 09:32:03  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.8  1995/06/27  19:21:35  irfan
 * CSCdi30390:  chat scripts should honor exec-character-bits
 * will do henceforth.
 *
 * Revision 2.7  1995/06/27  13:04:17  irfan
 * CSCdi29917:  special chars in chat script names should be discouraged
 * Check for illegal chars in script names. Fix a couple of printf'()s.
 *
 * Revision 2.6  1995/06/20  20:10:48  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.5  1995/06/14  18:32:52  irfan
 * CSCdi35773:  Async line scripts incorrectly hang up the line
 * Not all scripts should be hanging up the async line after
 * completion. Let the script type specific code decide if would
 * want to death().
 *
 * Revision 2.4  1995/06/14  08:07:50  irfan
 * CSCdi35821:  CSCdi29975 reintroduced in arizona (regress)
 * Don't death() a line if we just now ended running a reset_script. Else
 * infinite mutual recursion will result. Besides also fix a memory leak,
 * introduced by arizona.
 *
 * Revision 2.3  1995/06/13  16:37:27  syiu
 * CSCdi35685:  Async callback rotary should be in reverse oder
 *
 * Revision 2.2  1995/06/09  13:21:54  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:09:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#define DEFINE_MESSAGES TRUE
#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ctype.h>
#include <ciscolib.h>
#include "subsys.h"
#include "sys_registry.h"
#include "../h/cs_registry.regh"
#include "interface_private.h"
#include "address.h"
#include "config.h"
#include "parser.h"
#include "ttysrv.h"
#include "../ts/chat.h"
#include "logger.h"
#include "aaa.h"
#include "../ts/if_slip.h"
#include "../util/regexp.h"
#include <string.h>
#include "../os/async.h"
#include "../tcp/tcp.h"
#include "../wan/dialer.h"
#include "../wan/dialer_private.h"
#include "../ui/debug.h"
#include "../ts/chat_debug.h"
#include "../parser/parser_defs_config.h"
#include "../ts/modemcap_public.h"


/*
 * String constants
 */
static const char CHAT_illegal_chars[] = { "^$_.|?+*/{}[](),\\'`~%" };

/*
 * Function prototypes for functions in this module
 */

static char *chat_extract(char *, char *);
static char *chat_extract_substring(char *, char);
static void chat_extract_fail(char *, char **, char **);
static void chat_free_matchlist(chat_match *);
static boolean chat_add_match(chat_match **, char *);
static chat_match *chat_match_char(chat_match *, char);
static int chat_expect(tt_soc *, char *, chat_match *, int);
static void chat_send(tt_soc *, char *);
static int chat_run_script(tt_soc *, char *, char *);
static forktype chat_process(chat_session *);
static forktype chat_sys_startup_process(tt_soc *);
static chat_session *get_chat_session_block(tt_soc *);
static inline void free_chat_session_block(chat_session *);
static inline void report_chat_expect(tt_soc *, char *);
static inline void report_chat_send(tt_soc *, char *);
static inline void report_chat_start(tt_soc *, chat_script *);
static inline void report_chat_finish(tt_soc *, chat_script *, int);
static int chat_line_setup(tt_soc *, int);


/*
 * External references
 */

extern boolean dialer_debug;

/*
 * Local data
 */

facdef(CHAT)
msgdef_section("Chat script errror message");
msgdef(CANTRUN, CHAT, LOG_ERR, 0,
       "%t in use, can't run startup script");
msgdef_explanation(
    "When the startup script attempted to run, it found another process had
     Already been started on the line");
msgdef_recommended_action(
    "Remove startup script from line configuration if it's inappropriate.
     Report to cisco.");
msgdef_ddts_component("chat");


chat_script *chat_script_head;

/*
 * Utility routines
 */

static char *chat_extract (char *script, char *keyword)
{
    char c;			/* Character being processed */
    int i = 0;			/* Output offset */
    boolean quoted = FALSE;	/* Assume not a quoted string */

    keyword[0] = '\0';		/* Ensure returned keyword is zero-filled */

    if (!script)
      return(NULL);		/* Handle end of script */

    while (*script && (' ' == *script))
      script++;			/* Point past leading punctuation */

    if (!*script)
      return(NULL);		/* End of script */

    if (*script == '\"') {
	quoted = TRUE;
	script++;		/* Point past starting quote */
    }

    while ((c = *script)) {
	script++;		/* Point past character just read */

	if (quoted) {
	    if (c == '\"')
	      break;		/* Get out on a quote */	
	    if (c == '\n')	/* Newline in the stream? */
	      keyword[i++] = '\r'; /* Insert leading CR */
	} else {
	    if (' ' == c)
	      break;
	}

	if (c == '^') {		/* Control character? */
	    c = *script;	/* Get the next character */
	    if (!c)
	      break;		/* Get out if the end */
	    c = c & 0x1f;	/* Make into control character */
	    script++;

	} else if (c == '\\') { /* An escaped character? */
	    c = *script;	/* Get the next character */
	    if (!c)
	      break;		/* Get out if the end */

	    /*
	     * Handle special escaped characters.
	     */

	    script++;		/* Point past escaped parameter */

	    switch (c) {	/* Dispatch based on character */
	      case 'n':		/* Insert a newline */
		c = LINEFEED;
		
		break;

	      case 'r':
		c = RETURN;
		break;

	      case 's':
		c = ' ';	/* Use a space */
		break;

	      case 't':
		c = CH_HT;	/* Insert a tab */
		break;

	      default:

		/*
		 * This is not a special escaped character. Store an escape
		 * in the keyword buffer so that our caller can handle the
		 * sequence specially.
		 */

		keyword[i++] = '\\'; /* Store an escape */
		break;
	    }
	}

	keyword[i++] = c;	/* Store the character */
    }
    keyword[i] = '\0';		/* Zero-terminate the string */

    while (*script && (' ' == *script))
      script++;			/* Point past any ending stuff */

    return(*script ? script : NULL); /* Return pointer to next or null */
}

/*
 * chat_extract_substring - Extract a substring
 */

static char 
  *chat_extract_substring (
			   char *expect,
			   char term)
{
    char c;

    /*
     * Search for an un-escaped terminator character
     */

    while (TRUE) {
	c = *expect;
	if (!c)
	  return(NULL);		/* No failure string found */

	if (c == term)
	  break;		/* Found a terminator, get out */

	expect++;		/* Point past the character */
	if (c == '\\')		/* Is this an escape? */
	  expect++;		/* Yes, point past the escaped character */
    }

    return(expect);		/* Return pointer to terminator */
}

/*
 * chat_extract_fail - Parse the failure clause of an expect string
 *
 * This routine is used to parse an expect string for a failure clause.
 * If a non-escaped "-" is found in the string, then there is a failure
 * clause to extract.
 *
 * We will replace the hyphen with a null to properly terminate the
 * string, and then set pointers into the expect buffer at the
 * appropriate points for the failure send and expect strings.
 */

static void chat_extract_fail (
     char *expect,
     char **fail_send,
     char **fail_expect)
{
    /*
     * Clear out pointers in case we don't find anything
     */

    *fail_send = NULL;
    *fail_expect = NULL;

    /*
     * Search for a hyphen in the string
     */

    expect = chat_extract_substring(expect, '-'); /* Search for hyphen */
    if (!expect)
      return;			/* Nothing found */

    /*
     * At this point, we have found a "-", so we have a failure
     * string.
     */

    *expect++ = '\0';		/* Zero-terminate expect string */

    *fail_send = expect;	/* Set pointer to start of send string */

    /*
     * Now search for another hyphen for the start of the failure expect
     * string.
     */

    expect = chat_extract_substring(expect, '-'); /* Search for hyphen */
    if (!expect)
      return;			/* Didn't find one */

    *expect++ = '\0';		/* Zero-terminate fail send string */

    *fail_expect = expect;	/* Set pointer to fail expect string */
}

/*
 * chat_add_match - Add a match to a list of match strings
 */

static boolean chat_add_match (chat_match **list, char *string)
{
    chat_match *ptr;
    char c;

    ptr = malloc_named(sizeof(chat_match) + strlen(string), "chat_match");
    if (!ptr)
	return(FALSE);

    ptr->len = 0;		/* Initialize length */
    ptr->match = 0;		/* No match initially */
    
    while ((c = *string++)) {
	if (c == '\\') {
	    c = *string++;	/* Point past escapes */
	}
	ptr->pattern[ptr->len++] = c; /* Store character */
    }

    ptr->next = *list;		/* Set link to next */
    *list = ptr;		/* And make this the head of the list */
    return(TRUE);		/* Indicate success */
}

/*
 * chat_free_matchlist - Free a match list
 */

static void chat_free_matchlist (chat_match *list)
{
    chat_match *ptr;
    chat_match *next;

    ptr = list;			/* Get list head */

    while (ptr) {
	next = ptr->next;	/* Save link  */
	free(ptr);		/* Dump structure */
	ptr = next;		/* Get link to next */
    }
}

/*
 * chat_match_char - Match an incoming character to active strings
 *
 * This routine is used to look for a match of an incoming character
 * against all of our search strings.
 */

static chat_match
  *chat_match_char (
		    chat_match *list, 
		    char ch)
{
    chat_match *ptr;	/* chat context in for loop */

    for (ptr = list; ptr; ptr = ptr->next) {
	if (ch != ptr->pattern[ptr->match]) { /* If a mismatch */
	    ptr->match = 0;	/* Reset match counter */
	} else {
	    ptr->match++;	/* Count another matched character */
	    if (ptr->match == ptr->len) { /* Complete match? */
		return(ptr);	/* Return this item */
	    }
	}
    }
    return(NULL);		/* No match so far */
}

/*
 * formatted_chat_string
 *
 * for purposes of writing config and information strings
 * we will use our own formatter
 *
 * returns a pointer which must be used immediately in the
 * same thread. this pointer points to a static string
 */
static inline uchar *formatted_chat_string (char *chat_string)
{
    int read_ptr, write_ptr;
    uchar ch;
    static uchar permanent_chat_string[CHAT_MAX_STR];

    for (read_ptr = 0, write_ptr = 0;
	 chat_string[read_ptr]; read_ptr++) {

	ch = (uchar) chat_string[read_ptr];

	if (isprint(ch) && isascii(ch) && !(iscntrl(ch))) {
	    permanent_chat_string[write_ptr++] = ch;
	} else {
	    write_ptr += sprintf(&permanent_chat_string[write_ptr],
				 "\\%#3o", ch);
	}
    }
    permanent_chat_string[write_ptr] = '\0';
    return (permanent_chat_string);
}

static boolean chat_expectBLOCK (
     tt_soc *tty)
{
    if (AWAKE(tty->dallytimer) || modemcheck(tty) || inputpending(tty))
      return(FALSE);

    return(TRUE);
}

/*
 * chat_expect - Wait for a string, abort list, or timeout
 */

static int chat_expect (
     tt_soc *tty,
     char *expect,
     chat_match *abort,
     int timeout)
{
    int c;
    int match = 0;
    chat_match *abort_match;
    int chat_char_match = -1;   /* 1: matches, 0: mismatch, -1: init */

    report_chat_expect(tty, expect);

    TIMER_START(tty->dallytimer, (timeout * ONESEC));     /* Set timeout */

    while (TRUE) {
	edisms((blockproc *)chat_expectBLOCK, (int)tty);
				 /* Wait for a character or timeout */
	if (!SLEEPING(tty->dallytimer)) {
	    if (chat_debug && ((!chatlineno) || (chatlineno == tty->ttynum)))
		buginf("\nCHAT%t: Timeout expecting: %s",
		       tty->ttynum, formatted_chat_string(expect));
	    return(TIMEDOUT);	/* Say a timeout happened */
	}

	if (modemcheck(tty)) {
	    if (chat_debug && ((!chatlineno) || (chatlineno == tty->ttynum)))
		buginf("\nCHAT%t: Carrier drop expecting %",
		       tty->ttynum, formatted_chat_string(expect));
	    return(CARRIER_DROP);	/* Say we were disconnected */
	}

	c = getc(tty);		/* Get a character */

	if (-1 == c)
	    continue;		/* No input right now */

	c &= tty->tty_execmask; /* Clear (possibly) parity bit */

	/*
	 * We have an input character, check for any aborts
	 */

	abort_match = chat_match_char(abort, c);
	if (abort_match) {
	    if (dialer_debug ||
		(chat_debug && ((!chatlineno) || (chatlineno == tty->ttynum))))
		buginf("\nCHAT%t: Found abort string %*s expecting %s",
		       tty->ttynum, abort_match->len, abort_match->pattern,
		       formatted_chat_string(expect));
	    return(ABORTED);	/* We found an abort string */
	}

	/*
	 * Skip any escapes in the expect string
	 */

	if ('\\' == expect[match])
	  match++;

	/*
	 * Check for matching on our string
	 */

	if (expect[match++] == c) {
	    if (chat_debug && chat_debug_granular && ((!chatlineno) ||
				      (chatlineno == tty->ttynum))) {
		if (chat_char_match != 1 ) {
		    buginf("\nCHAT%t: Input match for: %s:: ",
			   tty->ttynum, formatted_chat_string(expect));
		    chat_char_match = 1;
		}
		if ((iscntrl(c)) || !(isprint(c))) {

		    buginf("\\%#3o", c);
		} else {
		    buginf("%c", c);
		}
	    }
	    if (!expect[match]) {
		if (chat_debug && ((!chatlineno) ||
				   (chatlineno == tty->ttynum)))
		    buginf("\nCHAT%t: Completed match for expect: %s",
			   tty->ttynum, formatted_chat_string(expect));
		return(OK);	/* Found a match */
	    }
	}
	else {
	    if (chat_debug && chat_debug_granular && ((!chatlineno) ||
			     (chatlineno == tty->ttynum))) {
		if (chat_char_match) {
		    buginf("\nCHAT%t: Input mismatch expecting: %s :: ",
			   tty->ttynum, formatted_chat_string(expect));
		    chat_char_match = 0;
		}
		if ((iscntrl(c)) || !(isprint(c))) {
		    buginf("\\%#3o", c);
		} else {
		    buginf("%c", c);
		}
	    }
	    match = 0;		/* Reset match pointer */
	}
    }
}

static void chat_send (tt_soc *tty, char *send)
{
    char ch;
    char buf[MODEMCAP_MAXLEN], *ptr;
    int count;

    report_chat_send(tty, send);

    TIMER_START(tty->dallytimer, (CHAT_OUTPUT_TIMEOUT * ONESEC));

    while ((ch = *send++)) {

	startoutput(tty);	/* Get output going */

	/*
	 * Check for the escape character. There are several special
	 * functions to execute.
	 */
	if ('\\' == ch) {

	    ch = *send++;	/* Get the special character */

	    switch (ch) {

	    case 'p':
		process_sleep_for(ONESEC/4); /* Wait for .25 seconds */
		continue;

	    case 'd':
		process_sleep_for(ONESEC * 2); /* Wait for two seconds */
		continue;

	    case 'K':
		sendbreak(tty);	/* Send a break */
		continue;

	    case 'N':
		ch = CH_NUL;	/* Send a NULL */
		break;

	    case 'c':
		return;		/* Get out without trailing CR */

	    case 'q':
		continue;	/* Quiet mode - NYI */

	    case 'T':		/* Insert telephone number */
		ttyprintf(tty, "%s", tty->dial_string);
		continue;

	    case 'M':		/* insert modem configuration string */
		count = 0;
		memset(buf, 0, MODEMCAP_MAXLEN);
		ch = *send++;
		if (ch != ':') {
		    if (chat_debug) {
			buginf("\nCHAT%t:Bad modemcap escape.  Should start"
				" with ':'. ", tty->ttynum);
		    }
		    return;
		}
		while ((ch = *send++)) {
		    if (count >= MODEMCAP_MAXLEN) {
			break;
		    }
		    if (ch == ':') {
			break;
		    } else {
			if (islower(ch))
				buf[count++] = _toupper(ch);
			    else
				buf[count++] = ch;

		    }
		}
		if (ch != ':') {
		    if (chat_debug) {
			buginf("\nCHAT%t:Bad modemcap escape.  Should end"
				" with ':'. ", tty->ttynum);
		    }
		    return;
		}
		buf[count] = 0;
		ptr = reg_invoke_modemcap_get_string(tty->modem_name, buf,0);
		if (ptr)
		    ttyprintf(tty, "%s", ptr);
		continue;
	    }
	}
	ch &= tty->tty_execmask; /* Clear possibly unneeded bit */
	putc(tty, ch);		/* Output the character */
    }
    putc(tty, RETURN);		/* End with a return */
    startoutput(tty);		/* Start the output */
}

/*
 * chat_run_script - Run a chat script
 *
 * This routine is used to actually run a chat script on a specified
 * terminal. It returns when the script has completed execution, or
 * on an error. If last_expect is non-NULL, it will also copy the last 
 * expected string to this pointer.
 */

static int chat_run_script (tt_soc *tty, char *script, char *last_expect)
{
    char expect[CHAT_MAXEXPECT]; /* Expect buffer */
    char send[CHAT_MAXEXPECT];	/* Send buffer */
    char *alt_send;		/* String to send on failure */
    char *alt_expect;		/* String to expect after alt_send */
    int timeout = CHAT_DEFAULT_TIMEOUT;	/* Timeout for data read */
    chat_match *abort = NULL;	/* Abort list */
    int status = OK;
    boolean valid_send;
    boolean parsed;

    while (script) {

	/*
	 * Fetch expect keyword
	 */

	script = chat_extract(script, expect);

	/*
	 * Fetch the send keyword if any. Note that the send keyword
	 * is optional at the end of the script. In this case, we get out
	 * after the final expect, instead of sending an extra CR at
	 * the end.
	 */

	if (script) {
	    valid_send = TRUE;
	    script = chat_extract(script, send);
	} else
	    valid_send = FALSE;

	/*
	 * If we have a non-null expect string, process it.
	 */

	if (expect[0]) {

	    /*
	     * If the send string is non-null, check for special keywords
	     */

	    if (valid_send) {

		/*
		 * Check for the abort keyword. If we've found it, the
		 * send string here must be added to our abort queue.
		 */
		
		if (0 == strcmp(expect, "ABORT")) {
		    chat_add_match(&abort, send); /* Add the abort string */
		    continue;
		}
	
		/*
		 * Check for the timeout keyword. If we've found it, the
		 * send string here is actually a new timeout to set.
		 */

		if (0 == strcmp(expect, "TIMEOUT")) {
		    timeout = parse_unsigned(send, &parsed);
		    if (!parsed) {
	/*		errmsg(&msgsym(BADTIMEOUT, TS), tty->ttynum, send); */
			timeout = CHAT_DEFAULT_TIMEOUT;
		    }
		    continue;
		}
	    }



	    /*
	     * Parse the expect string to see if we have an else clause
	     */

	    chat_extract_fail(expect, &alt_send, &alt_expect);

	    /*
	     * Wait for the string
	     */

	    status = chat_expect(tty, expect, abort, timeout); 

	    /*
	     * If we have a timeout and an alternate string, handle
	     * that now.
	     */

	    if (status == TIMEDOUT && alt_send) {
		chat_send(tty, alt_send);
		status = chat_expect(tty, alt_expect, abort, timeout);
	    }

	    if (status != OK)
		break;
	    
	}

	/*
	 * If there is a send string, send it now
	 */
	
	if (valid_send)
	    chat_send(tty, send);
    }

    /*
     * Return all of the abort structures
     */

    chat_free_matchlist(abort);

    /*
     * Copy the last expected string to pass back to caller
     */
    if ((last_expect) && (strlen(last_expect) <= CHAT_MAXEXPECT))
       strcpy(last_expect, expect);

    return(status);		/* Return completion status */
}

/*
 * chat_process - Run a chat session on a tty
 */

static forktype chat_process (chat_session *session)
{
    int status;
    tt_soc *tty = session->tty;

    if (dialer_debug || modem_debug || (chat_debug &&
			 ((!chatlineno) || (chatlineno == tty->ttynum))))
	buginf("\nCHAT%t: process started", tty->ttynum);

    /*
     * If we are running modem control, assert DTR and wait for CTS
     * to come up.
     */

    tty_portenable(tty);	/* Ensure that the port is enabled */
    tty_dallytimer(tty);	/* Set a timeout */

    status = chat_line_setup(tty, SCRIPT_DIALER_MODEM);

    if ((OK == status) && session->modem_script) {

	report_chat_start(tty, session->modem_script);
	status = chat_run_script(tty, session->modem_script->script, NULL);
	report_chat_finish(tty, session->modem_script, status);
    }

    if ((OK == status) && session->system_script) {

	report_chat_start(tty, session->system_script);
	status = chat_run_script(tty, session->system_script->script, NULL);
	report_chat_finish(tty, session->system_script, status);
    }

    if (OK == status) {
	/* Make sure last bunch of output has probably been received */
	/* on the other end, before protocol takes over the line */

	startoutput(tty);               /* ensure output is running */
	tty_wait_output(tty, CHAT_DEFAULT_TIMEOUT);

	if (FALSE == reg_invoke_async_protocol_mode(tty, &session->addr)) {
	    if (dialer_debug || (chat_debug && ((!chatlineno) ||
						(chatlineno == tty->ttynum))))
		buginf("\nCHAT%t: Unable to set line into async"
		       " interface mode", tty->ttynum);
	    status = ABORTED;
	}
    }

    free_chat_session_block(session); /* Free chat context block */

    if (OK != status)
	death(tty);             /* Embrace it gracefully */

    tty->statbits &= ~DIALOUT;	/* Completed successful dialing */
    tty->tiptop = -1;		/* Forget process number */
    process_kill(THIS_PROCESS);	/* No where to return; Kill me now */
}

/*
 * chat_dial - Start a dialing process
 *
 * This routine is called by the dial-on-demand processing to start up
 * a chat script on a line.
 */
boolean chat_dial (
     hwidbtype *idb,		/* IDB of async interface */
     char *number,		/* Pointer to number to dial */
     dialermaptype *addr)	/* Dialer map entry for this destination */
{
    tt_soc *tty;
    chat_script *modem_script;	/* Pointer to modem script structure */
    chat_script *system_script;	/* Pointer to system script structure */
    regexp *modem_chat = NULL;	/* Compiled regular expression for
				   modem chat script name */
    regexp *system_chat = NULL;	/* Compiled regular expression for
				   system chat script name */
    chat_session *session;	/* Session context */

    tty = idb->idb_tty;		/* Get TTY pointer */
    if (!tty)
      return(FALSE);		/* No TTY block, how strange */

    if (modem_debug || chat_debug || dialer_debug)
	buginf("\nCHAT%t: Attempting async line dialer script",
	       tty->ttynum);

    if ((tty->statbits & HASPROC) 
	    || reg_invoke_modem_hardware_in_use(tty)) {
	if (dialer_debug || (chat_debug &&
			     ((!chatlineno) || (chatlineno == tty->ttynum))))
	  buginf("\nCHAT%t: dial attempt to %s and line is in use",
		 tty->ttynum, number);
	return(FALSE);
    }

    /*
     * Pick up specified chat scripts from the dialer map
     */

    if (addr) {
	modem_chat = (addr->cip)->modem_chat_regcomp;
	system_chat = (addr->cip)->system_chat_regcomp;
    }

    /*
     * Search for a matching modem chat script
     *
     * For a modem script, the script name must match both the regular
     * expression set for the terminal line modem chat-script parameter,
     * and the script name passed to us. Either one or both of the
     * script names may be ommited; in this case, the respective match
     * is always treated as TRUE.
     */

    if (tty->scr_dialer_comp || modem_chat) {
	FOR_ALL_CHAT_SCRIPTS(modem_script)
	    if (((!tty->scr_dialer_comp) ||
		 regexec(tty->scr_dialer_comp, modem_script->name)) &&
		((!modem_chat) ||
		 regexec(modem_chat, modem_script->name)))
		break;		/* Found one */
    } else {
	modem_script = NULL;	/* Not running a modem script */
    }

    /*
     * Search for a matching system chat script
     *
     * We are passed a regular expression of a system script to look
     * for. Search for it.
     */

    if (system_chat) {
	FOR_ALL_CHAT_SCRIPTS(system_script)
	  if (regexec(system_chat, system_script->name))
	    break;
    } else {
	system_script = NULL;	/* No system script */
    }

    /*
     * We must have at least a modem chat script or a system chat script
     * at this point. It is also acceptable to have both.
     */

    if (!modem_script && !system_script) {
	if (dialer_debug || (chat_debug &&
			     ((!chatlineno) || (chatlineno == tty->ttynum))))
	    buginf("\nCHAT%t: no matching chat script found for %s",
		   tty->ttynum, number);
	return(FALSE);
    }

    if (dialer_debug || (chat_debug &&
			 ((!chatlineno) || (chatlineno == tty->ttynum))))
	buginf("\nCHAT%t: Dialing using Modem script: %s  & " \
	       "System script: %s", tty->ttynum,
	       (modem_script) ? modem_script->name : "none",
	       (system_script) ? system_script->name : "none");

    /*
     * Attempt to allocate a context block
     */

    session = malloc_named(sizeof(chat_session), "chat session");
    if (!session) {
	return(FALSE);
    }

    session->tty = tty;		/* Save TTY poitner */

    if (modem_script) {
	mem_lock(modem_script);	/* Place a lock on the buffer */
	session->modem_script = modem_script; /* Remember buffer pointer */
    }

    if (system_script) {
	mem_lock(system_script);	/* Place a lock on the buffer */
	session->system_script = system_script;	/* Remember buffer pointer */
    }

    /*
     * Save the target IP address, if any
     */

    session->addr.type = ADDR_ILLEGAL;

    if (addr && (LINK_IP == addr->link)) {
	session->addr.type = ADDR_IP;
	session->addr.length = ADDRLEN_IP;
	session->addr.ip_addr = addr->proto_addr.ip_addr;
    }
    setstring(&tty->dial_string, number);
    CLAIM_TTY(tty);
    tty->statbits |= DIALOUT;	/* Assign the line */

    tty->tiptop = cfork((forkproc *)chat_process, (long)session,
			0, "Async dialer", tty->ttynum);

    if (tty->tiptop < 0) {
	if (dialer_debug || (chat_debug &&
			     ((!chatlineno) || (chatlineno == tty->ttynum))))
	    buginf("\nCHAT%t: process creation failure calling %s",
		   tty->ttynum, number);
	RELEASE_TTY(tty);
	tty->statbits &= ~DIALOUT; /* Line isn't in use anymore */
	free_chat_session_block(session);    /* Release context block */
	if (tty->dial_string)
	    setstring(&tty->dial_string,NULL);
	return(FALSE);
    }

    return(TRUE);
}

/*
 * Start_Chat  Privilged command to run a chat script on a line
 *
 * Called from the parser
 *
 */
void start_chat(parseinfo *csb)
{

    regexp *script_exp_comp;
    tt_soc *tty = MODEMS[GETOBJ(int,1)];
    int status;

    if (csb->nvgen || !csb->sense)       /* sanity checks */
	return;

    if (!GETOBJ(int,1))
	tty = stdio;
    else
	setstring(&tty->dial_string, GETOBJ(string,2));

    if ((CTY_LINE == tty->type) || (VTY_LINE == tty->type)) {
	printf("\n%% Chat scripts can not be run on the Console or"
	       " Virtual lines");
	return;
    }

    if ((!GETOBJ(string,1)) || \
	(NULL == (script_exp_comp = regcomp(GETOBJ(string,1))))) {
	printf("\n%% Bad chat-script expression: %s", GETOBJ(string,1));
	return;
    }

    if ((tty != stdio) && (tty->statbits & HASPROC)) {
	printf("\n%% Chat script can not be run on line running other "
	       "process");
	return;
    }

    if (reg_invoke_modem_hardware_in_use(tty)) {
	printf("\n%% Modem is in use.");
	return;
    }

    if (tty != stdio) {
	CLAIM_TTY(tty);
    }

    status = do_chat_script(tty, GETOBJ(string,1), script_exp_comp,
			    SCRIPT_USER, NULL);

    if (tty != stdio) {
	RELEASE_TTY(tty);
	if (OK != status) {
	    death(tty);
	}
    }

    if (tty->dial_string) {
	free (tty->dial_string);
	tty->dial_string = NULL;
    }
}

/*
 * do_chat_script - Start a Chat script on a line
 *
 * This routine is called by external modules to execute a chat script on
 * the given tty line
 *
 */
int do_chat_script (tt_soc *tty, char *script_exp,
		    regexp *script_exp_comp, int chat_call_type,
		    char *last_expect)
{

    int status;

    chat_script *temp_script = NULL;     /* Temp pointer to chat_script */
    chat_session *session;               /* Session context */
    vectortype *save_ivector = NULL;     /* Temporary save vector */
    
    if ((!tty) || (!script_exp_comp) || (VTY_LINE == tty->type)) {
	if (chat_debug && ((!chatlineno) || (chatlineno == tty->ttynum)))
	    buginf("\nCHAT%t: bad call to do_chat_script",
		   (tty) ? tty->ttynum : 0);
	return (ABORTED);            /* No TTY block, no script regexp */
    }                                /* or wrong line type! how strange */

    /*
     * Search for a matching chat script
     *
     * We are passed a regular expression of a chat script to look
     * for. Search for it.
     */

    temp_script = find_chat_script(script_exp_comp);

    if (temp_script) {
	if (chat_debug && ((!chatlineno) || (chatlineno == tty->ttynum)))
	    buginf("\nCHAT%t: Matched chat script %s to string %s",
		   tty->ttynum, temp_script->name, script_exp);
    } else {
	if (chat_debug && ((!chatlineno) || (chatlineno == tty->ttynum)))
	    buginf("\nCHAT%t: No matching chat script found for %s",
		   tty->ttynum, script_exp);
	return(NO_DATA);
    }

    /*
     * Attempt to allocate a context block
     */

    if (NULL == (session = get_chat_session_block(tty)))
	return (NO_MEMORY);

    session->tty = tty;		         /* Save TTY poitner */
    session->system_script = NULL;
    session->chat_type = chat_call_type;
    mem_lock(temp_script);               /* Place a lock on the buffer */
    session->modem_script = temp_script;
    if (SCRIPT_CONNECTION == chat_call_type) {
	save_ivector = tty->ivector;	 /* Temporarily save ivector */
	tty->ivector = tty->nvector;
    }

    if (OK == (status = chat_line_setup(tty, chat_call_type))) {

	tty->statbits |= DIALOUT;           /* Say that we are dialing */
					    /*     or chatting         */
	report_chat_start(tty, session->modem_script);

	status = chat_run_script(tty, session->modem_script->script, last_expect);

	startoutput(tty);                   /* ensure output is running */
	tty_wait_output(tty, CHAT_DEFAULT_TIMEOUT); /* Let the trail */
	                                            /* get through */

        /* 
         * Callbacks require this flag to remain set
         */
	if (chat_call_type != SCRIPT_CALLBACK)
           tty->statbits &= ~DIALOUT;
    }


    report_chat_finish(tty, session->modem_script, status);

    if (SCRIPT_CONNECTION == chat_call_type) {
	tty->ivector = save_ivector;     /* Restore ivector */
    }

    free_chat_session_block(session);

    return status;

}

static forktype chat_sys_startup_process(tt_soc *tty)
{

    if (modem_debug)
	buginf("\nCHAT%t: system startup script started", tty->ttynum);
    do_chat_script(tty, tty->scr_startup, tty->scr_startup_comp,
		   SCRIPT_STARTUP, NULL);
    /*
     * Note that we are killing the line after running a chat script.
     * This prevents the Reset script from running.
     */
    tty->statbits |= DIALOUT;
    death(tty);
}

/*
 * chat_sys_startup
 *
 * System coming up. Run any startup scripts as needed..
 * Need to run a chat process asynchronously so that init doesn't wait.
 * Called by init_process through the restarted service point.
 */
static void chat_sys_startup (void)
{
    int i;
    tt_soc *tty;

    for (i = 0; i <= (nttylines+nauxlines); i++) {
	if (!(tty = MODEMS[i]))
	    continue;

	/* Run any startup scripts specified on the lines */

	if (tty->scr_startup && tty->scr_startup_comp) {
	    if ((tty->statbits & HASPROC) 
		|| reg_invoke_modem_hardware_in_use(tty)) {
		errmsg(&msgsym(CANTRUN, CHAT), tty->ttynum);
		continue;
	    }
	    tty->tiptop = cfork((forkproc *)chat_sys_startup_process,
				(long)tty, 0, "Script Startup", tty->ttynum);
	    if (-1 != tty->tiptop) {
		CLAIM_TTY(tty);
		tty->statbits |= DIALOUT;  /* Mark the line in use */
	    }
	}
    }
}

/*
 * chat_tty_death
 *
 * Check to see if there is a script to run on line resets ?
 * We don't make the check for vty's as vty will not have this
 * parameter in the first place
 * Called through death service point
 */
static void chat_tty_death (tt_soc *tty)
{

/*
 * It's easy to do it now, but in future when other areas need to do
 * I/O on the line at the time of death, we need to make sure that
 * chat_tty_death is the last.
 */

    if ((!(tty->statbits & DIALOUT)) && tty->scr_reset &&
	tty->scr_reset_comp) {
	if (modem_debug || chat_debug )
		buginf("\nCHAT%t: Attempting line reset script", tty->ttynum);
	do_chat_script(tty, tty->scr_reset, tty->scr_reset_comp,
		       SCRIPT_RESET, NULL);

    }
}

/* chat_nw_connection
 *
 * Check to see if we have any script connection specified on the
 * line for network connections ? In case we do, run the script
 * Called through tty_birth service point
 */
static void chat_nw_connection (tt_soc *tty)
{

    if (tty->scr_connection) {
	if (modem_debug || chat_debug )
		buginf("\nCHAT%t: Attempting line remote connection script",
		       tty->ttynum);
	do_chat_script(tty, tty->scr_connection, tty->scr_connection_comp,
		       SCRIPT_CONNECTION, NULL);
    }
}

/* chat_exec
 *
 * Check to see if we have any script connection specified on the
 * line for activation ? In case we do, run the script
 * Called through tty_birth service point
 */
static void chat_exec (tt_soc *tty)
{

    if (tty->scr_activation) {
	if (modem_debug || chat_debug )
		buginf("\nCHAT%t: Attempting line activation script",
		       tty->ttynum);
	do_chat_script(tty, tty->scr_activation,
		       tty->scr_activation_comp,
		       SCRIPT_ACTIVATION, NULL);
    }
}

int chat_line_setup(tt_soc *tty, int chattype)

{

    int status = OK;

    tty_portenable(tty);	/* Ensure that the port is enabled */
    tty_dallytimer(tty);	/* Set a timeout */

    while (TRUE) {
	tty->statbits &= ~IDLETIMEOUT;	/* no idle timeout yet */
	TIMER_STOP(tty->dallytimer);	/* no timer ticking yet */
	if ((tty->capabilities & MODEMMASK) == MODEMOUT) {
	    tty->answercountdown = tty->answertimeout; /* init the timeout */
	    tty->statbits &= ~(IDLE + CARDROP + READY + HANGUP);
	    tty->statbits |= RINGIN;	/* waiting for CTS to come up */
	    /*
	     * Clear the I/O of the serial line.  It may have
	     *  data left over from a previous connection.
	     * Avoid flushing line for line reset. This can flush
	     *  logout banner as well. We might as well wait for the
	     *  the output to clear in that case.
	     */
	    if (chattype != SCRIPT_RESET) {
		clearinput(tty);
		clearoutput(tty);
	    }
	    if (modem_debug || dialer_debug || (chat_debug && ((!chatlineno) ||
						(chatlineno == tty->ttynum))))
		buginf("\nCHAT%t: Asserting DTR", tty->ttynum);

	    dtrstate(tty,TRUE);		/* assert DTR */
	    edisms((blockproc *)readyBLOCK,(long)tty); /* block until */
						       /* READY or IDLE */
            if (tty->statbits & (IDLE+CARDROP)) { /* must have timed out */
		if (dialer_debug || (chat_debug && ((!chatlineno) ||
				    (chatlineno == tty->ttynum))))
		    buginf("\nCHAT%t: Timed out waiting for CTS", tty->ttynum);
		status = TIMEDOUT;
	    }
	    break;
	}
	else {
	    /*
	     * Set the line state to be READY
	     */
	    tty->statbits &= ~(IDLE + CARDROP + RINGIN + HANGUP);
	    tty->statbits |= READY;
	    /*
	     * Clear the I/O of the serial line.  It may have
	     * data left over from a previous connection.
	     * Avoid flushing line for line reset. This can flush
	     *  logout banner as well. We might as well wait for
	     *  line to finish output in that case.
	     */
	    if (chattype != SCRIPT_RESET) {
		clearinput(tty);
		clearoutput(tty);
	    }
	    if (modem_debug || dialer_debug || (chat_debug && ((!chatlineno) ||
						(chatlineno == tty->ttynum))))
		buginf("\nCHAT%t: Asserting DTR", tty->ttynum);
	    dtrstate(tty,TRUE);		/* always assert DTR */
	    break;
	}
    }
    return status;
}

/*
 * chat_script_command - Handle the "chat-script" configuration command
 *
 * The format of this command is:
 *
 *	chat-script name script
 *
 * Where name is a single-word token and script is the rest of the line.
 */

void chat_script_command (parseinfo *csb)
{
    chat_script *script = NULL;

    if (csb->nvgen) {
	FOR_ALL_CHAT_SCRIPTS(script) 
	    nv_write(TRUE, "%s %s %s", csb->nv_command, script->name,
		     formatted_chat_string(script->script));
	return;
    }


    /*
     * Create the chat script, if one already does not exist
     */
    chat_script_add_or_delete(csb->sense, TRUE, GETOBJ(string,1),
			      GETOBJ(string,2));
}

/*
 * chat_script_add_or_delete - Add or delete a chat-script
 */
int chat_script_add_or_delete(boolean add, boolean print_msg, 
                              char *script_name, char *script_contents)
{
    chat_script *script;
    chat_script *previous;
    char *bad_char;
    int script_name_len;
    int script_len;

    /*
     * See if we can find a matching chat script name
     */
    previous = NULL;
    FOR_ALL_CHAT_SCRIPTS(script) {
	if (strcmp(script_name, script->name) == 0)
	    break;
	previous = script;
    }

    /*
     * If the caller intends to remove the script from the script 
     * chain, do so and get out (e.g. 'no chat-script foo' command)
     */
    if (!add) {
	if (script) {
	    if (previous)
	      previous->next = script->next; 
	    else
	      chat_script_head = script->next; 
	    free(script);		
	}
	return(OK);			
    }

    /*
     * Return if this script is already defined
     */
    if (script){
	if (print_msg && strcmp(script->script, script_contents))
	    /* Print message unless scripts are identical */
	    printf("\n%% Chat script %s already defined as %s",
		   script->name, script->script);
	return(INUSE);
    }

    /*
     * disallow special chars in the script name
     */
    bad_char = strpbrk(script_name, CHAT_illegal_chars);
    if (NULL != bad_char) {
	printf("\n%% Chat script name has illegal character:  %c",
	       *bad_char);
	return (BADCHARACTERS);
    }

    /*
     * Create the script, and add it to appropriate position in script
     * chain
     */
    script_len = strlen(script_contents) + 1;
    script_name_len = strlen(script_name) + 1; /* Get script name */

    script = malloc_named(sizeof(chat_script) + script_name_len
			  + script_len, "chat script");
    if (!script) {
	return(NO_MEMORY);
    }

    /* 
     * Copy script name and setup the script name pointer
     */
    strcpy(&script->data[0], script_name); 
    script->name = &script->data[0]; 

    /*
     * Copy script contents setup the script contents pointer
     */     
    strcpy(&script->data[script_name_len], script_contents);
    script->script = &script->data[script_name_len]; 

    /*
     * Add script to appropriate position in script chain
     */
    script->next = NULL;
    if (previous)
      previous->next = script;	
    else
      chat_script_head = script; 

    return(OK);
}

/*
 * SCRIPT Line Subcommands
 *
 */
void script_line_commands (parseinfo *csb)
{
    int nvgen = csb->nvgen;
    tt_soc *tty = MODEMS[csb->line1];
    int i;
    char *script_exp = NULL;
    regexp *script_exp_comp = NULL;

    /* If we're doing NV generation, determine the correct config structs,
     * then fall through to process the selected function.
     */
    if (nvgen) {
	if ((csb->line1 < 0) || (csb->line1 >= ALLlines) || !tty) {
	    buginf("%% CHAT: Invalid line in NV generation: %t\n", csb->line1);
	    return;
	}
    }

    switch (csb->which) {

      case PARSER_LINE_SCRIPT_STARTUP:

	if (nvgen) {
	    nv_write((int) tty->scr_startup,
		     "%s %s", csb->nv_command,
		     tty->scr_startup);
	    return;
	}

	if (!get_script(csb->sense, GETOBJ(string,1), &script_exp,
		       &script_exp_comp))
	    return;

	for (i = csb->line1; i < csb->line2; i++) {
	    tty = MODEMS[i];

	    /* Scripts to be run only out to the physical line */
	    if (VTY_LINE == tty->type)
		continue;
	    if (tty->scr_startup)
		free(tty->scr_startup);
	    if (tty->scr_startup_comp)
		free(tty->scr_startup_comp);
	    if (script_exp) {
		mem_lock(script_exp);
		mem_lock(script_exp_comp);
	    }
	    tty->scr_startup = script_exp;
	    tty->scr_startup_comp = script_exp_comp;
	}

	/*
	 * Free script string & compiled reg exp pointers once to
	 * compensate for an extra mem_lock. Quite costly approach
	 */
	if (script_exp) {
	    free(script_exp);
	    free(script_exp_comp);
	}
	break;
	
      case PARSER_LINE_SCRIPT_CONNECTION:
	if (nvgen) {
	     nv_write((int) tty->scr_connection,
		      "%s %s", csb->nv_command,
		      tty->scr_connection);
	     return;
	 }

	if (!get_script(csb->sense, GETOBJ(string,1), &script_exp,
		       &script_exp_comp))
	    return;

	for (i = csb->line1; i < csb->line2; i++) {
	    tty = MODEMS[i];

	    /* Scripts to be run only out to the physical line */
	    if (VTY_LINE == tty->type)
		continue;
	    if (tty->scr_connection)
		free(tty->scr_connection);
	    if (tty->scr_connection_comp)
		free(tty->scr_connection_comp);
	    if (script_exp) {
		mem_lock(script_exp);
		mem_lock(script_exp_comp);
	    }
	    tty->scr_connection = script_exp;
	    tty->scr_connection_comp = script_exp_comp;
	}
	/*
	 * Free script string & compiled reg exp pointers once to
	 * compensate for an extra mem_lock. Quite costly approach
	 */
	if (script_exp) {
	    free(script_exp);
	    free(script_exp_comp);
	}

	break;

      case PARSER_LINE_SCRIPT_DIALER:
	if (nvgen) {
	     nv_write((int) tty->scr_dialer,
		      "%s %s", csb->nv_command,
		      tty->scr_dialer);
	     return;
	 }

	if (!get_script(csb->sense, GETOBJ(string,1), &script_exp,
		       &script_exp_comp))
	    return;

	for (i = csb->line1; i < csb->line2; i++) {
	    tty = MODEMS[i];
	    
	    /* Scripts to be run only out to the physical line */
	    if (VTY_LINE == tty->type)
		continue;
	    if (tty->scr_dialer)
		free(tty->scr_dialer);
	    if (tty->scr_dialer_comp)
		free(tty->scr_dialer_comp);
	    if (script_exp) {
		mem_lock(script_exp);
		mem_lock(script_exp_comp);
	    }
	    tty->scr_dialer = script_exp;
	    tty->scr_dialer_comp = script_exp_comp;
	}
	/*
	 * Free script string & compiled reg exp pointers once to
	 * compensate for an extra mem_lock. Quite costly approach
	 */
	if (script_exp) {
	    free(script_exp);
	    free(script_exp_comp);
	}

	break;

      case PARSER_LINE_SCRIPT_RESET:
	if (nvgen) {
	     nv_write((int) tty->scr_reset,
		      "%s %s", csb->nv_command,
		      tty->scr_reset);
	     return;
	 }

	if (!get_script(csb->sense, GETOBJ(string,1), &script_exp,
		       &script_exp_comp))
	    return;

	for (i = csb->line1; i < csb->line2; i++) {
	    tty = MODEMS[i];
	    
	    /* Scripts to be run only out to the physical line */
	    if (VTY_LINE == tty->type)
		continue;
	    if (tty->scr_reset)
		free(tty->scr_reset);
	    if (tty->scr_reset_comp)
		free(tty->scr_reset_comp);
	    if (script_exp) {
		mem_lock(script_exp);
		mem_lock(script_exp_comp);
	    }
	    tty->scr_reset = script_exp;
	    tty->scr_reset_comp = script_exp_comp;
	}
	/*
	 * Free script string & compiled reg exp pointers once to
	 * compensate for an extra mem_lock. Quite costly approach
	 */
	if (script_exp) {
	    free(script_exp);
	    free(script_exp_comp);
	}

	break;

      case PARSER_LINE_SCRIPT_ACTIVATION:
	if (nvgen) {
	     nv_write((int) tty->scr_activation,
		      "%s %s", csb->nv_command,
		      tty->scr_activation);
	     return;
	 }

	if (!get_script(csb->sense, GETOBJ(string,1), &script_exp,
		       &script_exp_comp))
	    return;

	for (i = csb->line1; i < csb->line2; i++) {
	    tty = MODEMS[i];
	    
	    /* Scripts to be run only out to the physical line */
	    if (VTY_LINE == tty->type)
		continue;
	    if (tty->scr_activation)
		free(tty->scr_activation);
	    if (tty->scr_activation_comp)
		free(tty->scr_activation_comp);
	    if (script_exp) {
		mem_lock(script_exp);
		mem_lock(script_exp_comp);
	    }
	    tty->scr_activation = script_exp;
	    tty->scr_activation_comp = script_exp_comp;
	}
	/*
	 * Free script string & compiled reg exp pointers once to
	 * compensate for an extra mem_lock. Quite costly approach
	 */
	if (script_exp) {
	    free(script_exp);
	    free(script_exp_comp);
	}

	break;

      case PARSER_LINE_SCRIPT_CALLBACK:
	if (nvgen) {
	     nv_write((int) tty->scr_callback,
		      "%s %s", csb->nv_command,
		      tty->scr_callback);
	     return;
	 }

	if (!get_script(csb->sense, GETOBJ(string,1), &script_exp,
		       &script_exp_comp))
	    return;

	for (i = csb->line1; i < csb->line2; i++) {
	    tty = MODEMS[i];
	    
	    /* Scripts to be run only out to the physical line */
	    if (VTY_LINE == tty->type)
		continue;
	    if (tty->scr_callback)
		free(tty->scr_callback);
	    if (tty->scr_callback_comp)
		free(tty->scr_callback_comp);
	    if (script_exp) {
		mem_lock(script_exp);
		mem_lock(script_exp_comp);
	    }
	    tty->scr_callback = script_exp;
	    tty->scr_callback_comp = script_exp_comp;
	}
	/*
	 * Free script string & compiled reg exp pointers once to
	 * compensate for an extra mem_lock. Quite costly approach
	 */
	if (script_exp) {
	    free(script_exp);
	    free(script_exp_comp);
	}

	break;
      case PARSER_LINE_SCRIPT_ARAPCALLBACK:
	if (nvgen) {
	     nv_write((int) tty->scr_arapcallback,
		      "%s %s", csb->nv_command,
		      tty->scr_arapcallback);
	     return;
	 }

	if (!get_script(csb->sense, GETOBJ(string,1), &script_exp,
		       &script_exp_comp))
	    return;

	for (i = csb->line1; i < csb->line2; i++) {
	    tty = MODEMS[i];
	    
	    /* Scripts to be run only out to the physical line */
	    if (VTY_LINE == tty->type)
		continue;
	    if (tty->scr_arapcallback)
		free(tty->scr_arapcallback);
	    if (tty->scr_arapcallback_comp)
		free(tty->scr_arapcallback_comp);
	    if (script_exp) {
		mem_lock(script_exp);
		mem_lock(script_exp_comp);
	    }
	    tty->scr_arapcallback = script_exp;
	    tty->scr_arapcallback_comp = script_exp_comp;
	}
	/*
	 * Free script string & compiled reg exp pointers once to
	 * compensate for an extra mem_lock. Quite costly approach
	 */
	if (script_exp) {
	    free(script_exp);
	    free(script_exp_comp);
	}

	break;

      case PARSER_LINE_SCRIPT_OFFHOOK:
	if (nvgen) {
	     nv_write((int) tty->scr_offhook,
		      "%s %s", csb->nv_command,
		      tty->scr_offhook);
	     return;
	 }

	if (!get_script(csb->sense, GETOBJ(string,1), &script_exp,
		       &script_exp_comp))
	    return;

	for (i = csb->line1; i < csb->line2; i++) {
	    tty = MODEMS[i];
	    
	    /* Scripts to be run only out to the physical line */
	    if (VTY_LINE == tty->type)
		continue;
	    if (tty->scr_offhook)
		free(tty->scr_offhook);
	    if (tty->scr_offhook_comp)
		free(tty->scr_offhook_comp);
	    if (script_exp) {
		mem_lock(script_exp);
		mem_lock(script_exp_comp);
	    }
	    tty->scr_offhook = script_exp;
	    tty->scr_offhook_comp = script_exp_comp;
	}
	/*
	 * Free script string & compiled reg exp pointers once to
	 * compensate for an extra mem_lock. Quite costly approach
	 */
	if (script_exp) {
	    free(script_exp);
	    free(script_exp_comp);
	}

	break;

      case PARSER_LINE_SCRIPT_LINE_CHANGE:
	if (nvgen) {
	    nv_write((int) tty->scr_line_change,
		     "%s %s", csb->nv_command,
		     tty->scr_line_change);
	    return;
	}

	if (!get_script(csb->sense, GETOBJ(string,1), &script_exp,
		       &script_exp_comp))
	    return;

	for (i = csb->line1; i < csb->line2; i++) {
	    tty = MODEMS[i];

	    /* Scripts to be run only out to the physical line */
	    if (VTY_LINE == tty->type)
		continue;
	    if (tty->scr_line_change)
		free(tty->scr_line_change);
	    if (tty->scr_line_change_comp)
		free(tty->scr_line_change_comp);
	    if (script_exp) {
		mem_lock(script_exp);
		mem_lock(script_exp_comp);
	    }
	    tty->scr_line_change = script_exp;
	    tty->scr_line_change_comp = script_exp_comp;
	}
	/*
	 * Free script string & compiled reg exp pointers once to
	 * compensate for an extra mem_lock. Quite costly approach
	 */
	if (script_exp) {
	    free(script_exp);
	    free(script_exp_comp);
	}

	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

boolean get_script (int sense, char *scr, char **script_exp,
	       regexp **script_exp_comp)
{

    *script_exp = NULL;
   
    if (sense) {
	if (NULL == (*script_exp_comp = regcomp(scr))) {
	    printf("\n%% Unable to set compile script: %s", scr);
	    return FALSE;
	}

	setstring(script_exp, scr);
	if (NULL == (*script_exp)) {
	    printf(nomemory);
	    free(script_exp_comp);
	    return FALSE;
	}
    }
    else{
	*script_exp = NULL;
	*script_exp_comp = NULL;
    }
    return TRUE;
}

chat_script *find_chat_script(regexp *comp_script_r_exp)
{

    chat_script* temp_script = NULL;

    FOR_ALL_CHAT_SCRIPTS(temp_script)
	if (regexec(comp_script_r_exp, temp_script->name))
	    break;

    return temp_script;

}

static chat_session *get_chat_session_block(tt_soc *tty)
{

    chat_session *temp_chat_session = NULL;

    temp_chat_session = malloc_named(sizeof(chat_session), "chat sessn 1");

    return temp_chat_session;

}

static inline void free_chat_session_block (chat_session *session)
{
    if (session->modem_script)
	free(session->modem_script);
    if (session->system_script)
	free(session->system_script);
    free(session);
}

static inline void report_chat_expect (tt_soc *tty, char *expect)
{
    if (chat_debug && ((!chatlineno) || (chatlineno == tty->ttynum)))
	buginf("\nCHAT%t: Expecting string: %s", tty->ttynum,
	       formatted_chat_string(expect));
}

static inline void report_chat_send (tt_soc *tty, char *send)
{
    if (chat_debug && ((!chatlineno) || (chatlineno == tty->ttynum)))
	buginf("\nCHAT%t: Sending string: %s", tty->ttynum,
	       formatted_chat_string(send));
}

static inline void report_chat_start (tt_soc *tty, chat_script *script)
{
    if (dialer_debug || (chat_debug &&
			 ((!chatlineno) || (chatlineno == tty->ttynum))))
	buginf("\nCHAT%t: Chat script %s started", tty->ttynum,
	       script->name);
}

static inline void report_chat_finish (tt_soc *tty, chat_script *script,
				       int status)
{
    if (dialer_debug || (chat_debug &&
			 ((!chatlineno) || (chatlineno == tty->ttynum))))
	buginf("\nCHAT%t: Chat script %s finished, status = %s",
	       tty->ttynum, script->name,
	       (OK == status) ? "Success" : tcperror2string(status));
}

/*
 * Parser chains for chat scripts.
 * Include these in menus.c to simplify makefile changes necessary to
 * add chat to an image.
 */

#include "../parser/parser_actions.h"
#include "../parser/macros.h"

/*
 * Parser chains and definitions for chat scripts subsystem
 */

/*
 * Parse chains for Chat Scripts Line config subcommands
 */
#define	ALTERNATE	NONE
#include "cfg_line_script.h"
LINK_POINT(line_script_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Chat config command
 */
#define	ALTERNATE	NONE
#include "cfg_chat.h"
LINK_POINT(chat_config_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for Debug Chat Scripts commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_chat.h"
LINK_POINT(debug_chat_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for Start-Chat Scripts command
 */
#define	ALTERNATE	NONE
#include "exec_start_chat.h"
LINK_POINT(exec_start_chat_command, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for Chat scripts
 */
static parser_extension_request chat_chain_init_table[] = {
    { PARSE_ADD_EXEC_CMD, &pname(exec_start_chat_command) },
    { PARSE_ADD_CFG_LINE_CMD, &pname(line_script_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(chat_config_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(debug_chat_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * Chat subsystem header and initialization.
 */
#define CHAT_MAJVERSION 1
#define CHAT_MINVERSION 0
#define CHAT_EDITVERSION 1

/*
 * chat_scripts_init
 * Initialize the chat user interface.  This consists primarilly of
 * tying the commands into the parser at the appropriate points.
 */
void chat_scripts_init (subsystype *subsys)
{
    chat_script_head = NULL;
    chat_debug_init();
    callback_init();
    parser_add_command_list(chat_chain_init_table, "Chat Scripts");

    reg_add_chat_dial(chat_dial, "chat_dial");
    reg_add_restarted(chat_sys_startup, "chat_sys_startup");
    reg_add_death(chat_tty_death, "chat_tty_death");
    reg_add_tty_birth(TTYP_NW_DAEMON, chat_nw_connection,
				 "chat_nw_connection");
    reg_add_tty_birth(TTYP_EXEC, chat_exec, "chat_exec");
    reg_add_default_tty_birth(chat_exec, "chat_exec");
}

/*
 * CHAT SCRIPTS subsystem header
 */
#define CHAT_SCRIPT_MAJVERSION 1
#define CHAT_SCRIPT_MINVERSION 0
#define CHAT_SCRIPT_EDITVERSION  1

SUBSYS_HEADER(chat, CHAT_SCRIPT_MAJVERSION, CHAT_SCRIPT_MINVERSION,
	      CHAT_SCRIPT_EDITVERSION,
	      chat_scripts_init, SUBSYS_CLASS_LIBRARY,
	      NULL,
	      NULL);
