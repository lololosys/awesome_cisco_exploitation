/* $Id: ttysrv.c,v 3.16.4.12 1996/08/28 13:04:36 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/ttysrv.c,v $
 *------------------------------------------------------------------
 * ttysrv.c -- device independent terminal service routines
 *  
 * June 1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ttysrv.c,v $
 * Revision 3.16.4.12  1996/08/28  13:04:36  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.16.4.11  1996/08/19  19:01:29  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.16.4.10  1996/07/29  19:02:23  snyder
 * CSCdi64402:  declare arrays in lat that should be const, const
 *              save 1084 data, 68 out of image
 * Branch: California_branch
 *
 * Revision 3.16.4.9  1996/07/04  00:28:52  irfan
 * CSCdi61001:  Autoselect throws away characters on startup
 * Branch: California_branch
 * Add "flush-at-activation" line subcommand to the ts system.
 *
 * Revision 3.16.4.8  1996/06/17  08:34:54  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.16.4.7  1996/06/12  14:29:58  bjthomas
 * CSCdi38891:  need warning when line session-timeout nears
 * Branch: California_branch
 *
 * Revision 3.16.4.6  1996/05/10  18:20:43  lol
 * CSCdi57133:  TTY_EVENTS are not handled
 * Branch: California_branch
 * Remove call to process_wakeup_w_reason until such time as TTY_EVENT
 * handling
 * is properly architectured.
 *
 * Revision 3.16.4.5  1996/04/23  00:03:22  bjthomas
 * CSCdi53239:  show line should not display speed for vtys
 * Branch: California_branch
 *
 * Revision 3.16.4.4  1996/04/19  15:24:49  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.2.6.9  1996/04/16  03:56:00  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 2.2.6.8  1996/04/09  21:29:54  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.2.6.7  1996/03/19  08:26:05  che
 * Branch: IosSec_branch
 *
 * Revision 2.2.6.6  1996/03/05  04:44:04  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 2.2.6.5  1996/02/21  03:50:01  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.2.6.4  1996/01/23  06:14:19  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 2.2.6.3  1995/12/19  04:09:44  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 2.2.6.2  1995/12/17  02:13:32  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.2.6.1  1995/11/17  17:23:49  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.16.4.3  1996/04/16  19:10:00  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.16.4.2  1996/03/27  10:44:26  irfan
 * CSCdi33144:  telnet should be configurable to refuse some options
 * Branch: California_branch
 * Add a bit in tty->capabilities to allow telnet/rlogin to silently
 * ignore remote host's flow control change requests.
 *
 * Revision 3.16.4.1  1996/03/18  21:31:42  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.11.2.6  1996/03/16  07:21:35  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.11.2.5  1996/03/14  01:21:41  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.11.2.4  1996/03/13  01:51:11  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.11.2.3  1996/03/09  05:10:41  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.11.2.2  1996/03/07  10:20:12  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.11.2.1  1996/02/20  16:42:18  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.16  1996/03/07  12:09:55  billw
 * CSCdi46565:  <uses> in  <show line> doesnt include PPP, Xremote, etc
 *
 * Revision 3.15  1996/02/21  00:33:05  jquan
 * CSCdi45337:  wall() function used by flh is bogus. Added
 * datagram_done() after ttyline_sendmessage(). It is necessary.
 *
 * Revision 3.14  1996/02/13  20:49:46  hampton
 * Clean up includes of gdb.h and references to breakpoint.  [CSCdi48844]
 *
 * Revision 3.13  1996/02/13  19:13:13  jquan
 * CSCdi45337:  wall() function used by flh is bogus. Replace fprintf by
 * ttyline_sendmessage in wall() to avoid hanging system. Change comments
 * regarding wall() function. Remove datagram_done() after
 * ttyline_sendmessage() since the background tty output process will free
 * the buffer.
 *
 * Revision 3.12  1996/02/03  04:13:04  tkolar
 * CSCdi47986:  IOS modem integration isnt quite there yet
 * Add a way for the IOS to know if the modem switch has reserved the
 * modem.
 *
 * Revision 3.11  1996/01/22  06:56:26  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.10  1996/01/12  20:03:53  sjackson
 * CSCdi44704:  modem busyout command is needed
 * Command added as an extension of the line modem autoconfigure
 *
 * Revision 3.9  1995/12/15  23:01:35  billw
 * CSCdi45506:  Save 124 bytes per tty by handling padding differently
 * and a in statbits, and maybe an instruction or two as well!
 *
 * Revision 3.8  1995/12/09  00:43:50  tkolar
 * CSCdi45302:  Need a clean interface between modem hardware and
 * commserver
 * Make it easy to claim a tty for exclusive use by a process, and add a
 * modem_busyout registry.
 *
 * Revision 3.7  1995/12/02  02:31:03  eschaffe
 * CSCdi44755:  Few of the statbits flags in show line N command are not
 * interpreted
 *
 * Revision 3.6  1995/11/28  03:58:54  widmer
 * CSCdi44175:  Add setup_printf to replace sprintf in setup code
 *
 * Revision 3.5  1995/11/22  03:19:42  irfan
 * CSCdi41430:  Async lines may reset their configuration
 * Obituary: tty_reset_line_defaults() died today. All time favourite pain
 * in the neck.
 *
 * Revision 3.4  1995/11/20  22:27:06  sdurham
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 *        fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 *
 * Revision 3.3  1995/11/17  18:52:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:19:20  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:48:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.15  1995/11/08  21:19:30  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.14  1995/10/15  18:54:25  eschaffe
 * CSCdi40294:  Increase the number of Protocol Translation sessions
 *
 * Revision 2.13  1995/10/10  03:30:32  eschaffe
 * CSCdi41378:  System crashed in pad_close()
 *         Incorporate code review comments from BillW.
 *
 * Revision 2.12  1995/10/09  23:41:23  eschaffe
 * CSCdi41378:  System crashed in pad_close()
 *
 * Revision 2.11  1995/10/03  00:55:56  irfan
 * CSCdi37688:  garbage in show lines output
 * sanity check parameters for destroy_vtylines
 *
 * Revision 2.10  1995/09/20  18:56:42  billw
 * CSCdi39673:  format for dispatch-timeout in SHOW LINE bogus
 *
 * Revision 2.9  1995/09/18  17:12:17  ahh
 * CSCdi39085:  tcp connections accepted in HANGUP state w/ modem cts-...
 * Don't do that.
 *
 * Revision 2.8  1995/09/11  21:42:56  tkolar
 * CSCdi40022:  Telnet speed is retained even after the session is closed
 * Move passspeed and maxpassspeed into the userdata struct.
 *
 * Revision 2.7  1995/07/14  07:02:54  hampton
 * Convert Terminal Server to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi37093]
 *
 * Revision 2.6  1995/07/12 23:45:14  hampton
 * Convert TCP to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37015]
 *
 * Revision 2.5  1995/06/19 06:49:05  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.4  1995/06/14  19:22:50  vandys
 * CSCdi35265:  Should use intfc description if no line description
 * Don't reference tty->location directly; pass tt_soc to a
 * function, let it take from either tt_soc or IDB.
 *
 * Revision 2.3  1995/06/13  16:37:48  syiu
 * CSCdi35685:  Async callback rotary should be in reverse oder
 *
 * Revision 2.2  1995/06/09  23:40:06  achopra
 * CSCdi04836:  Provide ip-on-break functionality on TS
 *
 * Revision 2.1  1995/06/07  21:59:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "sys_registry.h"
#include "packet.h"
#include "config.h"
#include "ttysrv.h"
#include "stacks.h"
#include "connect.h"
#include "logger.h"
#include "login.h"
#include "../ui/command.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../ui/setup.h"
#include "../h/cs_registry.regh"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../h/auth_registry.regh"

extern void ttyline_sendmessage (int , paktype *);

/*
 * Forward references
 */
static char *paritystring(int code);
static int null_getc(tt_soc *);
static void display_padding(tt_soc *);
static void print_line(tt_soc *);
static void display_dispatch(tt_soc *);


/*
 * Local storage
 */
static queuetype translate_queue;
queuetype *translateQ = &translate_queue;

int AuxBase;			/* Auxilliary line (console port b) */
int nvtylines;
int maxvtylines;		/* max number of VTYs */
int defvtylines;		/* Default number of VTYs */
int VTYBase;
int freeLineBase;		/* first free line, updated by drivers */
int ALLlines;			/* Sum of all types of lines */

/*
 * NULLvector holds the default tty i/o routine vectors.
 * It saves a bunch of space.  Unless a tty needs different
 * processing for a given function it should point its ivector
 * and ovectors at this one.
 *
 * It is initialized by vty_init which is called by the start up
 * sequence.  It is assumed in various places that NULLvector
 * is initialized.
 */
vectortype NULLvector;			/* default vector dispatch */

/*
 * vty_init
 * Initialize the vanilla virtual terminals
 */

void vty_init (void)
{
    vectortype *vector;
    int i;

    vector = & NULLvector;
    vector_init(vector);
    vector->vgetc = null_getc;
    /* 
     * Set service hook
     */
    vector->vservicehook = generic_servicehook;
    vector->vnoservicehook = generic_noservicehook;
    for (i = VTYBase; i < VTYBase + nvtylines; i++) {
    	(void) tty_init(i,(IDLE+NOBANNER),VTY_LINE,vector);
	MODEMS[i]->actype = AC_PASSWORD;
	MODEMS[i]->tty_txspeed = ASYNC_DEFSPEED;
	MODEMS[i]->tty_rxspeed = ASYNC_DEFSPEED;
    }

    /*
     * Initialize tty parser support
     */
    async_parser_init();

    /*
     * Init the protocol translation queue
     */
    queue_init(translateQ, 0);
}

/*
 * vty_add
 * Add some number of vty lines for use by protocol conversion.
 */

static void vty_add (int n)
{
    register vectortype *vector;
    register int i;

    if (nvtylines == 0)		/* If no vtys, we can't add any either! */
	return;
    vector = & NULLvector; 	/* Already initialized ! */
    for (i = ALLlines; i < ALLlines + n; i++) {
	if (MODEMS[i])
	    printf("\n%% Line %t already exists!", i);
	else {
	    (void) tty_init(i,(IDLE+NOBANNER),VTY_LINE,vector);
	    MODEMS[i]->actype = AC_PASSWORD;
	    MODEMS[i]->tty_txspeed = ASYNC_DEFSPEED;
	    MODEMS[i]->tty_rxspeed = ASYNC_DEFSPEED;
	}
    }
    nvtylines += n;
    ALLlines += n;
}

/*
 * create_vtylines
 * Create some extra vty lines, because the customer has explicitly asked
 * for them with a "line vty 0 bignum" command.  The parser enforces the
 * maximum number of lines that can be created.
 */

void create_vtylines (int maxline)
{
    vty_add(1 + maxline - (nvtylines+VTYBase));
    /* Nofity anyone else who cares, that we have created extra vty lines. 
     */
    reg_invoke_extra_vtylines(maxline + 1);
}

/*
 * destroy some VTY lines because we don't want them any more.  Only
 * VTYs can be destroyed, and only if they are not in use, and there
 * is a minimum that are always around.
 */
void destroy_vtylines (int minline)
{
    int i;

    /*
     * sanity check: do nothing if minline
     * was never there to begin with!
     */
    if (minline > (VTYBase + nvtylines)) {
	printf("\n%% Vty line %t doesn't exist", minline);
	return;
    }

    nvtylines = minline - VTYBase;
    if (nvtylines < defvtylines) {
	printf("\n%% Can't delete last %d VTY lines", defvtylines);
	nvtylines = defvtylines;
    }
    ALLlines = nconlines + nttylines + nauxlines + nvtylines;
    for (i = ALLlines; i < MAXLINES; i++) {
	if (MODEMS[i]) {
	    if ((MODEMS[i]->statbits & HASPROC) || (MODEMS[i]->tiptop > 0))
		printf("\n%% Can't delete line %t, line in use.", i);
	    else {
		free(MODEMS[i]);
		MODEMS[i] = NULL;
	    }
	}
	else break;
    }
}

/*
 * get_vtyline
 * Given a port number, return next free terminal line or 0 if can't.
 */
 
int get_vtyline (register ulong port, connvector *connv)
{
    register int i, j;

    j = VTYBase + nvtylines;
    for (i = VTYBase; i < j; i++) {

	/* 
	 * Get a free line - No process or a connection attached to it.
	 */

	if ((MODEMS[i]->statbits & HASPROC) || (MODEMS[i]->tiptop > 0))
	    continue;

	if (!connv || (MODEMS[i]->transport_allowin & connv->type))
	    return(i);
    }
    return(0);
}

/*
 * select_line
 * Given a line number, see if we can assign it for a server process.
 * Line must not be in use, and if it requires CTS, the carrier drop bit
 * must be off. Helper routine for tcp_getline().
 * Returns line number on success, 0 on failure, -linenumber if port busy
 */

int select_line (int port, connvector *connv)
{
    tt_soc *tty;

    if ((port > 0) && (port < VTYBase)) {
        tty = MODEMS[port];
        if (connv && ! (tty->transport_allowin & connv->type))
            return(0);
        if ((tty->statbits & HASPROC) || (tty->tiptop > 0)
		|| reg_invoke_modem_hardware_in_use(tty))
            return(-port);
        if ((tty->capabilities & MODEMCTS) && (tty->statbits & (IDLE|HANGUP)))
            return(-port);
        return(port);
    }
    return(0);
}

/*
 * select_rotary
 * Given a rotary number, see if we can assign a port for a server process.
 * Line must not be in use, and if it requires CTS, the carrier drop bit
 * must be off.  Helper routine for tcp_getline().
 * Returns line number on success, 0 on failure, -linenumber if port busy
 * Line search can be in ascending or descending order. 
 * Optional previous tty to allow rotary selection of a next line (in 
 * ascending or descending order of line numbers) other than the previous one 
 */

int select_rotary (int port, connvector *connv, tt_soc *prev_tty, 
                   int search_order)
{
    register tt_soc *tty;
    register int i, first;

    first = 0;

    switch (search_order) {
      /*
       * Search by ascending order of tty number
       */
      case ROTARY_ASCENDING:
       for (i = 1; i < ALLlines; i++) {
           tty = MODEMS[i];
           if (tty->rotary == port) {
               if (first == 0) first = i;
               if ((tty->statbits & HASPROC) 
		 || tty->answercountdown 
		 || (tty->tiptop > 0) 
		 || (connv && ! (tty->transport_allowin & connv->type)) 
		 || ((tty->capabilities & MODEMCTS) && (tty->statbits & IDLE))
		 || reg_invoke_modem_hardware_in_use(tty))
                  continue;
               else if (prev_tty){
                   if (tty->ttynum  > prev_tty->ttynum)
                     return(i);
                   else 
                     continue;
	       }
               return(i);
           }
       }
       break;

      /*
       * Search by descending order of tty number
       */
      case ROTARY_DESCENDING:
       for (i = (ALLlines-1); i > 0; i--) {
           tty = MODEMS[i];
           if (tty->rotary == port) {
               if (first == 0) first = i;
               if ((tty->statbits & HASPROC) 
		 || tty->answercountdown 
		 || (tty->tiptop > 0) 
		 || (connv && ! (tty->transport_allowin & connv->type)) 
		 || ((tty->capabilities & MODEMCTS) && (tty->statbits & IDLE))
		 || reg_invoke_modem_hardware_in_use(tty))
                  continue;
               else if (prev_tty){
                   if (tty->ttynum  < prev_tty->ttynum)
                     return(i);
                   else 
                     continue;
	       }
               return(i);
           }
       }
       break;
     }
     return(-first);
}

/*
 * ttybug
 * Print out debugging information
 */

void ttybug (tt_soc *tty, char *str)
{
    buginf("\nTTY%t: %s", tty->ttynum, str);
}

/*
 * tty_allocate
 * Allocate a terminal if possible.
 * Line number zero indicates that we want the next free vty.
 * Return a pointer to the terminal socket, or NULL if line is busy
 * or if all vty's in use.
 */

tt_soc *tty_allocate (register int line, int assignflag)
{
    register tt_soc *tty;

    if (line == 0) {
	for (line = VTYBase; line < VTYBase + nvtylines; line++) {
	    tty = MODEMS[line];

	/* 
	 * Allocate a line only if the line is free and there is no
	 * process id (tiptop) or a connection associated with the line.
	 */
	    if ((tty->statbits & HASPROC) 
			|| (tty->tiptop > 0)
			|| reg_invoke_modem_hardware_in_use(tty))
		continue;
	    if (assignflag) {
		CLAIM_TTY(tty);
	    }
	    return(tty);
	}
	return(NULL);
    }
    tty = MODEMS[line];
    if ((tty->statbits & HASPROC)
		|| (tty->tiptop > 0)
		|| reg_invoke_modem_hardware_in_use(tty))
	return(NULL);
    if (assignflag) {
	CLAIM_TTY(tty);
    }
    return(tty);
}

/*
 * tty_deallocate
 * Given a tty pointer, deallocate a terminal.  This entails resetting
 * I/O vectors to their defaults and clearing the "hasproc" bit.
 * Also clear the CARDROP bit, as nothing is listening to it any more.
 */

void tty_deallocate (register tt_soc *tty)
{
    switch (tty->type) {
	case VTY_LINE:
	    tty->ivector = & NULLvector;	/* no more input */
	    tty->ovector = & NULLvector;	/* no more output */
	    break;
	default:
	    /*
	     * restore to "native" vectors
	     */
	    tty->ivector = tty->ovector = tty->nvector;
	    break;
    }
    RELEASE_TTY(tty);
    tty->statbits &= ~(CARDROP|LASTWASBREAK);	/* say no process */
    TIMER_STOP(tty->dallytimer);
}

/*
 * ungetc
 * Back up the input stream one character.
 */

void ungetc (tt_soc *tty, int ch)
{
    tty->isavedchar = ch;
}

/*
 * null_getc
 * Return -1 always for the NULL device
 */

static int null_getc (tt_soc *t)
{
    return(-1);
}

/*
 * special_chars
 * Handle special escape sequences embedded within a byte stream.
 *	tty->tty_escapechar
 *	tty->logoutchar
 *	tty->tty_holdchar
 * A value of -1 for any of the special characters indicates no such
 * characters are set.	The BREAK_CHAR is represented by an integer
 * not in the set of char.  We return -1 if the character has been
 * absorbed, otherwise we return the character.
 */

int special_chars (register tt_soc *tty, register int ch)
{
    /*
     * Represent the break character specially, mask ordinary characters.
     */
    if (ch == BREAK_CHAR)
      tty->statbits |= LASTWASBREAK;
    else
      ch &= tty->tty_specialmask;

    /*
     * Handle the breakpoint character. On the console terminal, typing
     * the breakpoint character will enter the debugger if debugging has
     * already been enabled.
     */
    if (tty->type == CTY_LINE) {
	if (ch == CH_BREAKPOINT) {
	    if (breakpoint()) {
		return(-1);
	    }
	}
    }

    /*
     * Handle software flowcontrol.  Don't return stop/start characters.
     */
    if (tty->capabilities & SFLOWIN) {
	if (ch == tty->tty_startchar) {
	    tty->statbits &= ~WANTXON;
	    startoutput(tty);
	    return(-1);
        }
        if (ch == tty->tty_stopchar) {
	    tty->statbits |= WANTXON;
	    tty_stopoutput(tty);
	    return(-1);
	}
    }

    /*
     * Other special handling only if PSI enabled.	
     */
    if ((tty->statbits & PSIENA) == 0)
	return(ch);

    /*
     * If we've received an escape character, see what we should do now.
     * Allow quoting of hold, escape, and logout characters.
     * Exit if the second character of sequence is 'x' or 'X'.
     * Flag as special if second character something else.  Used by Telnet.
     */	  
    if (tty->statbits & FIRSTESC) {
	tty->statbits &= ~FIRSTESC;
	if ((ch == (int) 'X') || (ch == (int) 'x')) {
	    tty->statbits &= ~WANTXON;		/* clear XOFF */
	    tty->statbits |= ESCAPED;
	    tty_clearbuffers(tty);
	    return(-1);
	} else if ((ch != tty->tty_holdchar) && (ch != tty->tty_escapechar) &&
	    (ch != (int) tty->logoutchar))
		tty->statbits |= LASTWASSPEC;
	return(ch);
    }	 

    /*
     * Check local hold.  If holding, re-enable transmitter.  If the release
     * character was the hold character, aborb it, else pass it on.
     */
    if (tty->statbits & HOLDING) {
	tty->statbits &= ~HOLDING;
	startoutput(tty);
	if (ch == tty->tty_holdchar)
	    return(-1);
    }
    if (ch == tty->tty_holdchar) {
	tty->statbits |= HOLDING;
	tty_stopoutput(tty);
	return(-1);
    }

    /*
     * Look for the escape character
     */
    if (ch == tty->tty_escapechar) {
	if (ch != BREAK_CHAR)
	    tty->statbits |= FIRSTESC;		/* we may escape */
	else {
	    tty->statbits |= ESCAPED;     /* we have escaped */
	    tty->statbits &= ~(LASTWASBREAK+WANTXON); /* clear break, XOFF */
	    tty_clearbuffers(tty);
	    if (ch == (int) tty->logoutchar)	/* let break be logout too */
		tty->statbits |= CARDROP;
	}
	return(-1);
    }	

    /*
     *  Look for the logout character
     */
    if (ch == (int) tty->logoutchar) {
	tty->statbits |= CARDROP;
	return(-1);
    }

    /*
     * Anything else is just data
     */	 
    return(ch);
}

/*
 * psion
 * Turn on the pseudo-interrupt system.
 */

void psion (tt_soc *tty)
{
    tty->statbits |= PSIENA;
}

/*
 * psioff
 * Disable pseudo-interrupts, release any flow control hold
 */

void psioff (register tt_soc *tty)
{
    leveltype status;

    if (tty->statbits & SENTXOFF) {
	if (!reg_invoke_tty_xon(tty)) {
	    putc(tty,tty->tty_startchar);
	}
    }
    status = raise_interrupt_level(TTY_DISABLE);
    tty->statbits &= ~PSIFLAGS;
    reset_interrupt_level(status);
}

/*
 * psiset
 * Return TRUE if escape char received
 */

boolean psiset (tt_soc *tty)
{
    return((tty->statbits & ESCAPED) ? TRUE : FALSE || modemcheck(tty));
}

/*
 * psipending
 * Return TRUE if escape char is being received or has been recieved
 */

boolean psipending (void)
{
    return((stdio->statbits & (FIRSTESC + ESCAPED)) ? TRUE : FALSE ||
	   modemcheck(stdio));
}

boolean psipending2 (tt_soc *tty)
{
    return((tty->statbits & (FIRSTESC + ESCAPED)) ? TRUE : FALSE ||
	   modemcheck(tty));
}

/*
 * modemcheck
 * Returns TRUE if carrier has dropped or if vty connection is going away.
 * Also clears ready and idle to force the tiptop to exit.
 */

boolean modemcheck (register tt_soc *tty)
{
    register long status;

    if (tty->conntop)
	if (!conn_alive(tty->conntop)) {
	    /*
	     * This is a little obscure.  Normally, we don't want to set
	     * CARDROP on connection failures except on VTYs.  However, in
	     * a tty_daemon during the login process, the line is essentially
	     * behaving like a VTY anyway (I/O vectors BOTH to TCP connection)
	     * Note that both vtys (always) and tty_daemons (during this phase)
	     * have ivector == ovector, and only these will also have conntop.
	     */
	    if (tty->ovector == tty->ivector)
		tty->statbits |= CARDROP;	    
	    return(TRUE);
	}
    status = tty->statbits;
    if ((status & (CARDROP + HANGUP)) &&
	 (tty->capabilities & (MODEMIN+MODEMOUT+MODEMRI))) {
	return(TRUE);
    }
    if (status & CARDROP)
	return(TRUE);
    if (status & HANGUP) {		/* never true here ? */
	tty->statbits |= CARDROP;
	return(TRUE);
    }
    return(FALSE);
}

/*
 * Routines to clean TTY input and output buffers
 */

void tty_clearbuffers (register tt_soc *tty)
{
    clearoutput(tty);		/* clear output first... */
    clearinput(tty);		/* because we might sent XON now */
}

/*
 * tt2number
 * Given a tt_soc pointer, return a terminal number
 */

int tt2number (tt_soc *tty)
{
    if (tty == NULL)
	return(0);
    else
	return(tty->ttynum);
}

/*
 * restoreflow
 * Put back software flow control from saved capabilities.
 * Intended for use by rlogin and telnet
 */

void restoreflow (tt_soc *tty, ulong oldcaps)
{
    if (tty->capabilities == oldcaps)
	return;
    tty->capabilities = oldcaps;
    setsflow(tty);
}

/*
 * terminal_command
 * Change terminal parameters by invoking the configuration mechanism.
 */

#define BUFFLEN 160


/*
 * ttystatbits
 * Description of the status bits
 */

#define NSTATBITS 32

static char *const ttystatbits[NSTATBITS] = {
"Dialing out",			/* DIALOUT   0x1 */
"Escape Started",		/* FIRSTESC  0x2 */
"Escape Typed",			/* ESCAPED   0x4 */
"PSI Enabled",			/* PSIENA    0x8 */
"DSR Transition",		/* RINGIN    0x10 */
"Idle",				/* IDLE	     0x20 */
"Ready",			/* READY     0x40 */
"Hanging up",			/* HANGUP    0x80 */
"Connected",			/* CONNECTED 0x100 */
"Carrier Dropped",		/* CARDROP   0x200 */
"Active",		        /* HASPROC   0x400 */
"Rcvd CR",			/* LASTWASCR 0x800 */
"Hold Typed",			/* HOLDING   0x1000 */
"Waiting for XON",		/* WANTXON   0x2000 */
"Sent XOFF",			/* SENTXOFF  0x4000 */
"Autobauding",			/* AUTOBAUDING 0x8000 */
"No Exit Banner",		/* NOBANNER  0x10000 */
"Idle Timeout",			/* IDLETIMEOUT 0x20000 */
"Rcvd BREAK",			/* LASTWASBREAK 0x40000 */
"Async Interface Active",	/* SLIPMODE 0x80000 */
"Sending Break",		/* SENDINGBREAK 0x100000 */
"Callback",		        /* CALLBACK_BYPASS 0x200000 */
"Rcvd Command",			/* LASTWASSPEC 0x400000 */
"Send XOFF Soon",		/* SENDXOFF    0x800000 */	
"Card Console",  		/* --Unused--  0x1000000 */
"Input Stopped",		/* STOPPEDINPUT 0x2000000 */
"Send Break Soon",		/* SENDBREAK   0x4000000 */
"Control-c Typed",		/* CONTROLC  0x8000000 */
"Ctrl-c Enabled",		/* CTRLC_ENBL  0x10000000 */
"HW PPP Support Active",	/* PPPMODE 0x20000000 */
"Vty-async Interface Up",	/* VTYASYNC_LINE_STATE  0x40000000 */
"Modem Detected"		/* MODEM_DETECTED 0x80000000 */
};


/*
 * ttystatbits2
 * Description of the status2 bits
 */

#define NSTATBITS_2 5

static char *const ttystatbits2[NSTATBITS_2] = {
"CTS Raised",			/* CTS_MODEM_DETECT_2 0x1 */
"Modem Configured",		/* MODEM_CONFIGURED_2 0x2 */
"Modem Configuring",		/* MODEM_CONFIGURING_2 0x4 */
"Modem Speed Locked",		/* MODEM_SPEED_LOCKEd_2 0x8 */
"Busyout"			/* BUSYOUT_2 0x10 */
};


/*
 * ttycapbits
 * Descriptions of the capabilities bits
 */

#define NCAPBITS 32

static char *const ttycapbits[NCAPBITS] = {
"No Flush-at-Activation",	/* NO_FLUSH_ACTIVATE 0x1 */
"Permit LAT Remote Modify",	/* REMOTE_MOD  0x2 */
"Notification Set",		/* NOTIFY      0x4 */
"EXEC Suppressed",		/* NOEXEC      0x8 */
"Private Line",			/* PRIVATE_TTY     0x10 */
"Telnet Transparent Mode",	/* TRANSPARENT 0x20 */
"Software Flowcontrol In",	/* SFLOWIN     0x40 */
"Software Flowcontrol Out",	/* SFLOWOUT    0x80 */
"Autobaud Full Range",		/* AUTOBAUD    0x100 */
"Receives Logging Output",	/* LOGGING     0x200 */
"Hardware Flowcontrol In",	/* HFLOWIN     0x400 */
"Hardware Flowcontrol Out",	/* HFLOWOUT    0x800 */
"Modem Callin",			/* MODEMIN     0x1000 */
"Modem Callout",		/* MODEMOUT    0x2000 */
"Modem CTS-Required",		/* MODEMCTS    0x4000 */
"Modem RI is CD",		/* MODEMRI     0x8000 */
"Line usable as async interface", /* MAYSLIP    0x10000 */
"Line is permanent async interface", /* MUSTSLIP   0x20000 */
"Character padding",		/* PADDED     0x40000 */
"Hangup on Last Close",		/* AUTOHANGUP 0x80000 */
"Refuse Telnet Echo",		/* REFUSEECHO 0x100000 */
"Refuse Suppress-GA",		/* REFUSESGA  0x200000 */
"Send SYNC on BREAK",		/* BREAKSYNC  0x400000 */
"Modem DTR-Active",		/* MODEMDTR   0x800000 */
"Lockable",			/* LOCKABLE  0x1000000 */
"Send BREAK on IP",		/* BREAKONIP 0x2000000 */
"Output non-idle",		/* OUTIDLE   0x4000000 */
"No login banner",		/* NOEXECBANNER 0x8000000 */
"Insecure",			/* INSECURE  0x10000000 */
"FastAuto",			/* AUTOHIGH  0x20000000 */
"ARAP Permitted",		/* MAYARAP   0x40000000 */
"ARAP Always"			/* MUSTARAP  0x80000000 */
};


/*
 * ttycapbits2
 * Descriptions of the capabilities2 bits
 */

#define NCAPBITS_2 11

static char *const ttycapbits2[NCAPBITS_2] = {
"Send IP on BREAK",             /* IPONBREAK_2  0x1 */
"Modem Discovery",		/* MA_DISCOVERY_2 0x2 */
"Modem Autoconfigure",		/* MA_FIXED_2	0x4 */
"Modem No Error Control",	/* NO_MODEM_ERRCNTRL_2 0x8 */
"Modem No Compression",		/* NO_MODEM_COMPRESS_2 0x10 */
"Modem Busyout",		/* MODEM_BUSYOUT_2 0x20 */
"",				/* MODEM_BUSYOUT_OVERRIDE_2 0x40 */
"Flow Control Locked",          /* FLOWPERMANENT_2 0x80 */
"NTP Reference Clock",		/* NTP_CLOCK_2 0x100 */
"PPS Reference Clock",		/* PPS_CLOCK_2 0x200 */
"NTP Receive Suppressed",	/* RCV_SUPPR_CLOCK_2 0x400 */
};


static const char dishead[] = {" "
"Tty Typ     Tx/Rx     A Modem  Roty AccO AccI  Uses    Noise   Overruns"
"\n"};

extern ulong irate,orate,totalinput, totaloutput, sorate, sirate;

void display_lines (parseinfo *csb)
{
    int i, target;

    target = GETOBJ(int,1);
    automore_enable(NULL);
#if DEBUG
    if (modem_debug) {
	printf("\nTotal characters input %lu, output %lu",
	       totalinput, totaloutput);
	printf("\nInstantaneous input rate %lu, output rate %lu",
	       irate, orate);
	printf("\nSmoothed (10 S) input rate %lu, output rate %lu",
	       sirate, sorate);
    }
#endif
    if (target == -1) {
	i = 0;			/* Scan through all lines */
	automore_header(dishead);
    } else {
	i = target;
	printf(dishead);
    }
    for (/* i Assigned above */; i < ALLlines; i++) {
	register tt_soc *t = MODEMS[i];
	if (automore_quit())
	    break;
	if ((target != -1) && (target != i))
	    continue;
        if (reg_invoke_invalid_line_specified(i, FALSE)) {
            printf("Line %d is currently invalid\n", i);
            continue;
        }
	printf("%c%-3t ",
	       ((t->statbits & SLIPMODE) ? 'A' :  /* A for async active */
		((t->statbits & CARDCONSOLE) ? 'C' : 
		 ((t->statbits & HASPROC) ? '*' : /* * means in use */
		  ((t->capabilities & MAYSLIP) ? 'I' : ' ')))),
	                                          /* I for interactive */
	       i);

	switch (t->type) {
	  case CTY_LINE:
	    printf("CTY%-15c",' '); break;
	  case TTY_LINE:
	    printf("TTY %-6u/%6u ",t->tty_txspeed,t->tty_rxspeed); break;
	  case VTY_LINE:
	    printf("VTY%-15c",' ');
	    break;
	  case AUX_LINE:
	    printf("AUX %-6u/%6u ",t->tty_txspeed,t->tty_rxspeed); break;
	  default:
	    printf("???%-15c",' '); break;
	}
	if (t->capabilities & AUTOBAUD) 
	    printf("F ");
	else
	    printf("- ");
	printf("%s", print_modemtype(t));
	if (t->rotary)
	    printf("%-4d ",t->rotary);
	else
	    printf("   - ");
	if (t->accessout)
	    printf("%-4d ",t->accessout);
	else
	    printf("   - ");
	if (t->accessin)
	    printf("%-4d",t->accessin);
	else
	    printf("   -");
	printf(" %-5d %-8d %-8d/%d", t->usagecount, t->linenoise,
	       t->overrun, t->overflow);
#ifdef DEBUG
	if (modem_debug)
	    printf("%-8d%-8d", t->totalin, t->totalout);
#endif
	printf("\n");
	if (target == i) {
	    print_line(t);
	    break;
	}
    }
    automore_disable();
}

/*
 * show_terminal
 * Support for the EXEC "show terminal" command
 */

void show_terminal (parseinfo *csb)
{
    automore_enable(NULL);
    print_line(stdio);
    automore_disable();
}

/*
 * wall
 * write to open tty line connections except the stdio for this process.
 * If display string is a NULL, assume this call is a test to
 * see if there are open connections.
 * Return back count of open connections found.
 */
int wall (char *strng)
{
    int i, count = 0;
    paktype *pak;

    for (i = 0; i < ALLlines; i++) {
	register tt_soc *t = MODEMS[i];
	if (t->statbits & HASPROC) {
	    switch (t->type) {
	      case VTY_LINE:
	      case CTY_LINE:
	      case AUX_LINE:
	      case TTY_LINE:
		if (stdio != t) { /* only print out warning messages to */
		    /* other tty connections. */
                    count++;
                    if (strng) {

			pak = getbuffer(strlen(strng) + 1); /* Get a buffer */
			if (!pak)	/* If no memory, skip this one */
			    continue;
			strcpy(pak->datagramstart, strng);
			pak->datagramsize = strlen(pak->datagramstart);
			ttyline_sendmessage(i, pak); 
			datagram_done(pak);

                    }
		}
		break;
	      default:
		break;
	    }
	}
    }
    if (count && strng)  /* we actually put out the msg */
        process_sleep_for(10*ONESEC);
    return (count);
}

/*
 * get_tty_location
 * Pick up location string, either from tt_soc or idb
 */
char *
get_tty_location(tt_soc *tty)
{
    idbtype *idb;

    if (tty != NULL) {
	if (tty->location && tty->location[0]) {
	    return(tty->location);
	}
	idb = firstsw_or_null(tty->tty_idb);
	if (idb && idb->description && idb->description[0]) {
	    return(idb->description);
	}
    }
    return(NULL);
}

/*
 * print_line
 * Display interesting portions of the tt_soc data structure
 */

static void print_line (register tt_soc *tty)
{
    long printlen = 0;
    int time_left;

    printf("\nLine %t, Location: \"%s\", Type: \"%s\"\nLength: %d lines, Width: %d columns",
		tty->ttynum,
		get_tty_location(tty),
		tty->termtype,
		tty->tty_length,
		tty->tty_width);
    if (tty->tty_txspeed && tty->tty_rxspeed) {
	printf("\nBaud rate (TX/RX) is %d/%d", tty->tty_txspeed, tty->tty_rxspeed);
	if (tty->type != VTY_LINE)
	    printf(", %s parity, %s stopbits, %d databits",
		paritystring(tty->tty_parity),
  		stopstring(tty->tty_stopbits), tty->tty_databits);
    }
    if (tty->tty_passspeed)
	printf("\nAccept speed from network, default %d, maximum %d",
 	       tty->tty_passspeed, tty->tty_maxpassspeed);
    printbits("\nStatus: ", tty->statbits & (~(LINESTATE-READY)), ttystatbits,
	      NSTATBITS, NULL);
    if ( tty->capabilities ) {
        printbits("\nCapabilities: ", tty->capabilities, ttycapbits, NCAPBITS,
            &printlen);
        if ( tty->capabilities2 )
            printbits(NULL, tty->capabilities2, ttycapbits2, NCAPBITS_2,
                &printlen);
    }
    else
        printbits("\nCapabilities: ", tty->capabilities2, ttycapbits2,
		  NCAPBITS_2, NULL);
    printlen = 0;
    printbits("\nModem state: ", tty->statbits & (LINESTATE), ttystatbits, 
	      NSTATBITS, &printlen);
    if (tty->statbits2) {
	printbits(NULL, tty->statbits2 & (LINESTATE_2), ttystatbits2,
		  NSTATBITS_2, &printlen);
    }
    printf("\n"
	     "Special Chars: Escape  Hold  Stop  Start  Disconnect  Activation"
	   "\n               ");
    { /* costmetic block */
	if (tty->tty_escapechar == BREAK_CHAR)
	    printf("BREAK");
	else if (tty->tty_escapechar < 0)
	    printf(" none");
	else
	    printf(" %cx ", tty->tty_escapechar);
	if (tty->tty_holdchar < 0) printf("   none");
	else
	    printf("    %c ", tty->tty_holdchar);
	if (tty->capabilities & (SFLOWIN+SFLOWOUT))
	    printf("   %c     %c ", tty->tty_stopchar, tty->tty_startchar);
	else printf("   -     -  ");
	if (tty->logoutchar != -1)
	    printf("      %c    ", tty->logoutchar);
	else printf("     none   ");
	if (tty->activatechar == -1)
	    printf("      any");
	else printf("      %c", tty->activatechar);
    }
/*
Timeouts:      Idle EXEC    Idle Session   Modem Answer  Session   Dispatch
               11:11:11      22:22:22       00:00:05     1:00:00   0:00:00.100
012345678901234567890123456789012345678901234567890123456789012345678901234
*/
    printf("\nTimeouts:      Idle EXEC    Idle Session   Modem Answer  "
	   "Session   Dispatch\n               ");
    {
	if (tty->tty_exectimeout)
	    print_ticks_in_dhms(tty->tty_exectimeout);
	else printf(" never ");
	printf("       ");
	if (tty->tty_sessiontimeout)
	    print_ticks_in_dhms(tty->tty_sessiontimeout);
	else printf(" never ");
	printf("        ");

	if (tty->tty_absolutetimeout) {
	    if (!TIMER_RUNNING(tty->time_activated)) {
		printf("             ");
		print_ticks_in_dhms(tty->tty_absolutetimeout);
		printf("    ");
	    } else {
		/* Don't try to print a -ve time. Transient case */
		time_left = tty->tty_absolutetimeout -
		    ELAPSED_TIME(tty->time_activated);
		if (time_left < 0)
		    time_left = 0;
		printf("             ");
		print_ticks_in_dhms(time_left);
		printf("    ");
	    }
	}
	else
	    printf("               none     ");

	if (tty->tty_dispatchtimeout) {
	    print_ticks_in_dhms(tty->tty_dispatchtimeout);
	    if (tty->tty_dispatchtimeout % ONESEC)
		printf(".%03d", tty->tty_dispatchtimeout % ONESEC);
	} else
	    printf("not set");
	if (tty->tty_sessiontimeout && (tty->capabilities & OUTIDLE))
	    printf("\nSession idle time reset by output.");
    }
    printf("\n                            Idle Session Disconnect Warning");
    printf("\n                             ");
    if (tty->sessiontimeout_warning) {
	print_ticks_in_dhms(tty->sessiontimeout_warning);
    } else {
	printf(" never ");
    }
    printf("\nModem type is ");
    if (tty->modem_name) {
	printf("%s.", tty->modem_name);
    } else {
	printf("unknown.");
    }
    printf("\nSession limit is ");
    if (tty->sessionlimit)
      printf("%d sessions.", tty->sessionlimit);
    else
	printf("not set.");
    printf("\nTime since activation: ");
    if (TIMER_RUNNING(tty->time_activated))
	print_dhms(tty->time_activated);
    else
	printf("never");
    printf("\nEditing is %sabled.", tty->tty_editingon ? "en" : "dis");
    printf("\nHistory is ");
    if (tty->tty_historyon) {
	printf("enabled, history size is %d.", tty->tty_historysize);
    } else {
	printf("disabled.");
    }
    printf("\nDNS resolution in show commands is %sabled", tty->tty_dns ?
	   "en" : "dis");
    printf("\nFull user help is %sabled", tty->tty_help ? "en" : "dis");
    show_transport(tty);
    if (tty->tty_def_autocommand)
	printf("\nAutomatically execute command \"%s\"",
	       tty->tty_def_autocommand);
    display_padding(tty);
    display_dispatch(tty);
    reg_invoke_tty_show(tty);
}

/*
 * paritystring
 * Return string describing parity handling
 */

static char *paritystring (int code)
{
    switch (code) {
	case PARITY_NO:	return("no");
	case PARITY_ODD: return("odd");
	case PARITY_EVEN: return("even");
	case PARITY_SPACE: return("space");
	case PARITY_MARK: return("mark");
	default: return("???");
    }
}

/*
 * stopstring
 * Return string describing stopbits
 */

char *stopstring (int code)
{
    switch (code) {
	case STOPBITS1: return("1"); 
	case STOPBITS15: return("1.5");
	case STOPBITS2: return("2");
        default: return("??");
    }
}

/*
 * display_padding
 * Show the padding setup for a terminal line
 */

static void display_padding (register tt_soc *tty)
{
    register int i;

    if (!tty->current.padding_ptr) {
	printf("\nNo output characters are padded");
	return;
    }
    printf("\nCharacters padded with NUL bytes:");
    printf("\n   Char    ASCII    count");
    for (i = 0; i < MAXASCII; i++) {
	if (tty->current.padding_ptr[i]) {
	    printf("\n    ");
	    if (i < 32)
	        printf("^%c", i+64);
	    else
		printf(" %c", i);
	    printf("%-9d%-9d", i, tty->current.padding_ptr[i]);
	}
    }
}

/*
 * printbits
 * Given a long word of bit flags, an array of names, and the length of
 * that array, pretty print the names of the flags that are set.
 */

void printbits (
    char *leader,
    register long wrd,
    char * const *array,
    long arraysize,
    long *printlen)
{
    register long i, len = 0, bit;
    register char *ptr;
    long leaderflag = TRUE;

    if (printlen) {
        len = *printlen;
    }
    if (leader) {
        len = printf("%s", leader);
        if (wrd == 0) {
            printf("none");
            return;
        }
    } else {
        if (wrd != 0)
            len += printf("%s", (len ? ", " : "\n  "));
    }
    for (i = 0; i < 32; i++) {
	bit = wrd & (1 << i);
	if (bit) {
	    if (!leaderflag)
		len += printf("%s", (len ? ", " : "\n  "));
	    else
	        leaderflag = FALSE;
	    ptr = (i >= arraysize) ? NULL : array[i];
	    if (!ptr)
		len += printf("%#x", bit);
	    else {
		if ((len + strlen(ptr)) > 75)
		    len = printf("\n  %s", ptr);
		else
		    len += printf("%s", ptr);
	    }
	    if (len > 60)
		len = 0;
	}
    }
    if (printlen) {
        *printlen = len;
    }
}

/*
 * tty_setedit
 * Set a special tty editing character to a particular value.  This sets
 * it the array of special characters, as well as indicating that it is
 * a dispatch character.  If you unset a character (value = -1), then
 * the old value of that character is removed from the dispatch table.
 */

void tty_setedit (tty_editdata *editinfo, int which, int value)
{
    int oldval = editinfo->special[which];

    if (which >= NSPECIAL) {
	errmsg(&msgsym(SPEC, SYS), which, value);
	return;
    }
    editinfo->special[which] = value;
    if (oldval > 0 && oldval < MAXASCII)
	editinfo->dispatch[oldval] &= ~TTY_DISP_EDIT;
    if (value > 0 && value < MAXASCII)
	editinfo->dispatch[value] = 1;
}

/*
 * tty_create_vty_exec
 * Create a virtual EXEC on the specified (virtual) tty line
 */

void tty_create_vty_exec (tt_soc *tty)
{
    ulong stacksize = EXEC_STACK_SIZE;

    if (reg_invoke_kerberos_is_running()) {
	stacksize = KERBEROS_STACK_SIZE;
    }

    if (tty->type == VTY_LINE) {
	tty->ovector = tty->ivector;
	tty->tiptop = cfork((forkproc *)exec, (long)tty, stacksize,
                            "Virtual Exec", tty->ttynum);
    } else
	tty->tiptop = -1;
}

/*
 * tty_savedefaults
 * When a user changes a tty parameter, save the default information,
 * so that it can be restored later.
 */

void tty_savedefaults (tt_soc *tty)
{
    if (tty->defaults)
	return;			/* already has saved defaults */
    tty->defaults = malloc(sizeof(tty_userdata));
    if (tty->defaults)
	bcopy(&tty->current, tty->defaults, sizeof(tty_userdata));
    if (tty->current.padding_ptr) { /* make copy of padding data */
	tty->defaults->padding_ptr = malloc(MAXASCII);
	if (tty->defaults->padding_ptr)
	    bcopy(tty->current.padding_ptr,
		  tty->defaults->padding_ptr, MAXASCII);
    }
}

/*
 * display_dispatch
 * Show the dispatch character setup for a terminal line
 */

static void display_dispatch (register tt_soc *tty)
{
    register int i, ch;

    for (i = 0, ch = 0; i < MAXASCII; i++)
	ch |= (int) tty->tty_editinfo.dispatch[i];
    if (!(ch  & TTY_DISP_DISP)) {
	printf("\nNo special data dispatching characters");
	return;
    }
    printf("\nCharacters causing immediate data dispatching:");
    printf("\n   Char    ASCII");
    for (i = 0; i < MAXASCII; i++) {
	if (tty->tty_editinfo.dispatch[i] & TTY_DISP_DISP) {
	    printf("\n    ");
	    if (i < 32)
	        printf("^%c%-9d", i+64, i);
	    else if (i == 127)
		printf("^?%-9d", i);
	    else
		printf(" %c%-9d", i, i);
	}
    }
}

/*
 * lock_command
 * Let the user lock all of his sessions with one command.
 */
void lock_command (parseinfo *csb)
{
    char password[50], password2[50];
    int c;

    if ((stdio->capabilities & (LOCKABLE|PRIVATE_TTY)) == 0) {
	printf("\n%% The \"lock\" command is not permitted from this terminal");
	return;
    }

    if (!askstring(stdio, password, sizeof password, "Password: ",
	30*ONESEC, 1, TRUE))
	return;
    if (!askstring(stdio, password2, sizeof password2, "Again: ",
	30*ONESEC, 1, TRUE))
	return;
    if (strcmp(password, password2) == 0) {
	while (TRUE) {
	    printf("\n\n\n\n\n\n\n\n\n\n\n                Locked\n\n\n\n\n\n\n");
	    c = getcwait(stdio);
	    if (c == -1) {
		if ((stdio->statbits & IDLETIMEOUT) && !modemcheck(stdio)) {
		    stdio->statbits &= ~IDLETIMEOUT;
		    continue;
		} else
		    return;
	    }
	    if ((c == RETURN) && askpassword(stdio, password, 0))
		return;
	}
    } else {
	printf("Passwords do not match.\n");
	return;
    }
}

/*
 * tty_if_reset
 * If this tty line is acting like a network interface,
 * apply the idb->reset vector.
 */

void tty_if_reset (tt_soc *tty)
{
    hwidbtype *hwidb;

    if (!(tty->statbits & SLIPMODE))
	return;
    hwidb = tty->tty_idb;
    if (!hwidb)
	return;
    (*(hwidb->reset))(hwidb);
}

/*
 * tty_xon_default
 * Default routine to handle XON
 * See SERVICE_TTY_XON
 */

boolean tty_xon_default (tt_soc *tty)
{
    return(FALSE);
}


void async_setup_global (setup_struct *sp)
{
    char *tempbuff;
    char *s;
    long speed;

    if (!system_loading && nttylines > 0) {
        if (reg_invoke_async_int_base_unit() >= AuxBase)
            return; 
	if (yes_or_no("  Configure Async lines", TRUE, TRUE)) {
	    s = tempbuff = malloc(500);
	    if (!tempbuff)
		return;
	    speed  = get_num_in_range("    Async line speed", 10,
				      ASYNC_MINSPEED, ASYNC_MAXSPEED,
				      ASYNC_DEFSPEED);
	    s += sprintf(s, "\nspeed %d", speed);
	    if (yes_or_no("    Configure for HW flow control", TRUE, TRUE)) {
		s += sprintf(s, "\nflowcontrol hardware");
	    }
	    if (yes_or_no("    Configure for modems", TRUE, FALSE)) {
		s += sprintf(s, "\nmodem inout");
		if (yes_or_no("      Configure for default chat script",
			      TRUE, TRUE)) {
		    s += sprintf(s, "\nscript dialer cisco-default");
		    setup_printf(sp,
				 "\nchat-script cisco-default "
				 "ABORT ERROR \"\" \"AT Z\" OK \"ATDT "
				 "\\T\" TIMEOUT 30 \\c CONNECT \\c");
		}
	    }
	    setup_printf(sp, "\n!\nline 1 %t%s", AuxBase-1, tempbuff);
	    reg_invoke_alllines_setup(sp);
	}
    }
}
