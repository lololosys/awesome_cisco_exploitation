/* $Id: async.c,v 3.20.4.12 1996/08/28 13:02:32 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/async.c,v $
 *------------------------------------------------------------------
 * async.c -- common asynchronous serial line support.
 *  
 * 23-December-1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: async.c,v $
 * Revision 3.20.4.12  1996/08/28  13:02:32  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.20.4.11  1996/08/02  23:37:59  billw
 * CSCdi49270:  login local on line doesnt work for reverse telnet
 * Branch: California_branch
 *
 * Revision 3.20.4.10  1996/07/23  13:26:50  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.20.4.9  1996/07/10  22:21:27  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.20.4.8  1996/07/09  23:50:21  pgoginen
 * CSCdi56342:  A few characters lost during a data transfer over an async
 * tunnel
 * Branch: California_branch
 * Send XON after making sure we can allocate buffers for the incoming
 * data once it starts arriving.
 *
 * Revision 3.20.4.7  1996/07/04  00:28:49  irfan
 * CSCdi61001:  Autoselect throws away characters on startup
 * Branch: California_branch
 * Add "flush-at-activation" line subcommand to the ts system.
 *
 * Revision 3.20.4.6  1996/06/12  14:29:39  bjthomas
 * CSCdi38891:  need warning when line session-timeout nears
 * Branch: California_branch
 *
 * Revision 3.20.4.5  1996/06/06  20:59:05  irfan
 * CSCdi57344:  Line stuck in carrier dropped, funny modem state 10600, no
 * process
 * Branch: California_branch
 * Ensure proper hangup/cleanup if CARDROPs in interface startup.
 *
 * Revision 3.20.4.4  1996/04/19  15:24:23  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.3.6.10  1996/04/16  03:55:44  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 2.3.6.9  1996/04/09  21:25:50  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.3.6.8  1996/03/19  08:24:51  che
 * Branch: IosSec_branch
 *
 * Revision 2.3.6.7  1996/03/05  04:43:54  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 2.3.6.6  1996/02/28  06:14:42  lol
 * Branch: IosSec_branch
 * TACACS+ processing.
 *
 * Revision 2.3.6.5  1996/02/21  03:47:56  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.3.6.4  1996/01/23  06:13:24  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 2.3.6.3  1995/12/19  04:08:48  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 2.3.6.2  1995/12/17  02:11:29  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.3.6.1  1995/11/17  17:21:14  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.20.4.3  1996/04/08  20:11:42  syiu
 * CSCdi53895:  Async Callback broken
 * Branch: California_branch
 *
 * Revision 3.20.4.2  1996/03/27  12:26:00  irfan
 * CSCdi48100:  bus error on restart (bcopy6)
 * Branch: California_branch
 * Code changes to avoid double execs on the same tty: Don't answer
 * new incoming calls while previous process/framing is not cleared
 * up fully. Avoid dropping the line twice when an async interface
 * resets. Never return from kfork(). In effect background
 * Modem process should develop better relationship with foreground
 * process/framing.
 *
 * Revision 3.20.4.1  1996/03/18  21:28:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.11.2.4  1996/03/16  07:20:14  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.11.2.3  1996/03/09  05:08:38  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.11.2.2  1996/03/07  10:17:40  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.11.2.1  1996/02/20  16:36:46  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.20  1996/03/09  22:56:27  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.19  1996/03/05  01:19:13  billw
 * CSCdi49461:  autoselect during-login causes premature authorization
 * Additional commenting...
 *
 * Revision 3.18  1996/03/01  16:51:10  vandys
 * CSCdi49509:  Remove dead code: serial_gets
 *
 * Revision 3.17  1996/02/23  10:37:55  billw
 * CSCdi49461:  autoselect during-login causes premature authorization
 * Define a new flag AUTOSELECT_AUTHORIZE that survives across death().
 * Set it at the same time AUTOSELECTED is set, but only clear it when
 * a new autoselect starts.
 *
 * Revision 3.16  1996/02/09  10:04:09  billw
 * CSCdi48421:  autoselect during-login causes multiple execs on single tty
 *
 * Revision 3.15  1996/02/07  16:14:08  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.14  1996/02/03  04:13:01  tkolar
 * CSCdi47986:  IOS modem integration isnt quite there yet
 * Add a way for the IOS to know if the modem switch has reserved the
 * modem.
 *
 * Revision 3.13  1996/02/01  06:07:30  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.12  1996/01/22  23:42:23  irfan
 * CSCdi47309:  autoselect during-login ignored with aaa new-model
 * review comments for CSCdi44865.
 *
 * Revision 3.11  1996/01/22  06:56:02  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.10  1995/12/22  01:13:26  perryl
 * CSCdi44865:  autoselect during-login should be ignored if login is not
 * set.
 * Avoid the case for "no login" and "autoselect during-login" ending
 * up not firing autoselect on the line. Add warning messages for
 * conflicting configurations.
 *
 * Revision 3.9  1995/12/17  18:34:06  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.8  1995/12/15  23:01:29  billw
 * CSCdi45506:  Save 124 bytes per tty by handling padding differently
 * and a in statbits, and maybe an instruction or two as well!
 *
 * Revision 3.7  1995/12/09  00:43:46  tkolar
 * CSCdi45302:  Need a clean interface between modem hardware and
 * commserver
 * Make it easy to claim a tty for exclusive use by a process, and add a
 * modem_busyout registry.
 *
 * Revision 3.6  1995/11/22  06:37:44  irfan
 * CSCdi41542:  Telnet and TN3270 connection dropping for Inactivity
 * Timeout
 * Ensure tty->dallytimer is correctly initialized in serial_putc before
 * starting a wait for this to expire. Avoid false waits for putpkt
 *
 * Revision 3.5  1995/11/20  22:26:57  sdurham
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 *        fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 *
 * Revision 3.4  1995/11/20  10:46:36  billw
 * CSCdi43781:  AAA Accounting not done for incoming tty daemon connections
 *
 * Revision 3.3  1995/11/17  18:44:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:16:46  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:43:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.14  1995/11/08  21:18:15  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.13  1995/10/13  22:13:18  eschaffe
 * CSCdi41961:  Crash in pad_putbyte()
 *
 * Revision 2.12  1995/09/25  19:04:53  dkatz
 * CSCdi40898:  Synchronized reload needed
 * Code review nits.
 *
 * Revision 2.11  1995/09/24  00:54:40  dkatz
 * CSCdi40898:  Synchronized reload needed
 *
 * Revision 2.10  1995/09/11  21:42:53  tkolar
 * CSCdi40022:  Telnet speed is retained even after the session is closed
 * Move passspeed and maxpassspeed into the userdata struct.
 *
 * Revision 2.9  1995/07/14  07:02:42  hampton
 * Convert Terminal Server to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi37093]
 *
 * Revision 2.8  1995/07/12 23:45:10  hampton
 * Convert TCP to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37015]
 *
 * Revision 2.7  1995/06/20 16:11:11  lol
 * CSCdi34401: Tacacs notif.type 6-ena and 10-slipoff not sent to
 *             nondefault server
 *
 * Revision 2.6  1995/06/20  04:14:22  ahh
 * CSCdi36095:  Connection routines use msclock
 * Remove more msclock references.  (Should have been in initial commit,
 * but something got screwed up.)
 *
 * Revision 2.5  1995/06/14  19:22:46  vandys
 * CSCdi35265:  Should use intfc description if no line description
 * Don't reference tty->location directly; pass tt_soc to a
 * function, let it take from either tt_soc or IDB.
 *
 * Revision 2.4  1995/06/14  06:30:44  billw
 * CSCdi35380:  AAA accounting causes crashes
 * Clean up aaa_dup_user and callers, correct freeing of non-malloced
 * memory, and a couple other things.
 *
 * Revision 2.3  1995/06/09  23:40:03  achopra
 * CSCdi04836:  Provide ip-on-break functionality on TS
 *
 * Revision 2.2  1995/06/09  13:11:59  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:53:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "sys_registry.h"
#include "../dev/flash_registry.h"
#include "../h/cs_registry.regh"
#include "interface_private.h"
#include "ttysrv.h"
#include "packet.h"
#include "connect.h"
#include "../ts/stp.h"
#include "async.h"
#include "stacks.h"
#include "name.h"
#include "config.h"
#include "../ui/debug.h"
#include "logger.h"
#include "login.h"
#include "hostname.h"
#include "parser.h"
#include "aaa.h"
#include "../os/texti.h"
#include "../os/os_debug_flags.h"
#include "../os/free.h"
#include "../ui/command.h"
#include "../ip/ip.h"
#include "../les/flh.h"
#include "../h/auth_registry.regh"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */

#include "../parser/parser_defs_autoselect.h"

/*
 * These routines support UART based serial line drivers.
 * The virtual terminals are handled elsewhere.
 */

/*
 * Forward declarations
 */
static int quick_gets(tt_soc *, char *, int);
static void tty_messages(tt_soc *);
static void check_line_absolute_timeout (tt_soc *);


/*
 * tty_reset
 * Reset user configurable terminal parameters at end of session.
 */

void tty_reset (tt_soc *tty)
{

    TIMER_STOP(tty->time_activated);    /* reset time_activated */
    psioff(tty);			/* clear interrupts */
    tty->statbits &= ~IDLETIMEOUT;	/* no idle timeout yet */
    tty->tty_autocommand = NULL;	/* clear autocommand */
    tty->acflags = 0;			/* clear access flags */
    TIMER_STOP(tty->callbacktimer);	/* reset callback timer */
    texti_reset(tty);

    /* set need to set privilege for next time */
    SET_NEED_TO_SET_PRIV(tty->privilege);


    /*
     * Clear flowcontrol stuff and poke output
     * Wait for output to finish.
     */
    if ((tty->statbits & SLIPMODE) == 0) {
	tty->txservice = NULL;
	tty->statbits &= ~(HOLDING|WANTXON);
	startoutput(tty);
	tty_wait_output(tty, MAX_TTY_OUTPUT_WAIT);
        process_sleep_for(ONESEC/10);
    }

    /*
     * Possibly reset the terminal speed to where we start autobaud detection.
     */ 
    if (tty->capabilities & AUTOBAUD)
	auto_setspeed(tty);
    else if (tty->tty_passspeed)
	setspeed(tty, tty->tty_passspeed, tty->tty_passspeed);

    setstring(&tty->user, NULL);	/* clear user name */
    aaa_free_user(&tty->ustruct);
    tty->accessout = tty->accessout_default;

    tty->automore_enabled = FALSE;
    tty->automore_supress = 0;

    /*
     * For public or virtual terminals, don't maintain environment.
     */
    if ((tty->type == VTY_LINE) || !(tty->capabilities & PRIVATE_TTY)) {
        tty->capabilities = tty->def_capabilities;
        tty->capabilities2 = tty->def_capabilities2;
	free(tty->termtype);
	if (tty->def_termtype)
	  mem_lock(tty->def_termtype);
	tty->termtype = tty->def_termtype;
	free(tty->keymaptype);
	if (tty->def_keymaptype)
	  mem_lock(tty->def_keymaptype);
	tty->keymaptype = tty->def_keymaptype;
	/*
	 * Restore hardware and software flow control only on VTYs that
	 * are not running SLIPMODE.
	 */ 
	if (!(tty->statbits & SLIPMODE)) {
	    setflow(tty);
	    setsflow(tty);
	}
	if (tty->defaults == NULL)
	    return;		/* defaults not changed, we are done */
        if (tty->type == VTY_LINE) {
 	    tty->tty_txspeed = 0;
	    tty->tty_rxspeed = 0;
	} else if (!(tty->capabilities & AUTOBAUD))
	    setspeed(tty, tty->defaults->u_txspeed, tty->defaults->u_rxspeed);
	setparity(tty, tty->defaults->u_parity);
	setdatabits(tty, tty->defaults->u_databits);
	setstopbits(tty, tty->defaults->u_stopbits);
	bcopy(tty->defaults, &tty->current, sizeof(tty_userdata));
	free(tty->defaults);
	tty->defaults = NULL;
    }
}

/*
 * set_dallytimer
 * Set the dallytimer for a line depending on whether we are in
 * a session or at the EXEC.
 */

void tty_dallytimer (tt_soc *tty)
{
    TIMER_STOP(tty->dallytimer);
    if (tty->statbits & CONNECTED) {
	if (tty->tty_sessiontimeout) 
	    TIMER_START(tty->dallytimer, tty->tty_sessiontimeout);
    } else {
	if (tty->tty_exectimeout)
	    TIMER_START(tty->dallytimer, tty->tty_exectimeout);
   }
}

/*
 * serial_outputblock - See if output would block
 */

boolean serial_outputblock (tt_soc *tty)
{
    uchar *t;

    t = tty->outtail;

    if (t++ == NULL)			/* Running putstring or putpkt? */
      return(TRUE);

    if (t == tty->outend)
      t = tty->outstart;

    if (tty->outhead == t) {
	(void)startoutput(tty);
	return(TRUE);
    }
    return(FALSE);
}

/*
 * serial_outputxoffed - See if output is xoffed
 */
boolean serial_outputxoffed (tt_soc *tty)
{
    return (tty->statbits & WANTXON) ? TRUE:FALSE;
}

/*
 * serial_putc
 * Output a character and possible padding on a serial line.
 */

void serial_putc (tt_soc *tty, int c)
{
    uchar *t, padding;
    leveltype status;

    if (tty->statbits & CARDROP)
	return;

    /*
     * if we are running putpkt, we should wait for it to
     * get over. In that case outtail is set to 0
     */
    while (tty->outpak && !tty->outtail) {
	/*
	 * Reset any dallytimer, if left running from past and not
	 * needed now. Also startoutput, just in case.
	 */
	tty_dallytimer(tty);
	if (!startoutput(tty)) {
	    return;
	}
 	/* Wait until packet is out */
	edisms((blockproc *) sputBLOCK, (ulong)tty);
	if (tty->statbits & CARDROP) {
	    /* If carrier "drops" while we are in here, we have to
	     * notice and kill the output packet.  Otherwise, if
	     * we are flowcontroled, we'd get permanantly stuck!
	     */
	    if (modem_debug) {
		ttybug(tty, "CARDROP or IDLETIMEOUT while put'ting pkt");
	    }
	    clearoutput(tty);
	}
    }

    if (c == BREAK_CHAR) {
	sendbreak(tty);			/* generate hardware break */
	return;
    }

    status = raise_interrupt_level(TTY_DISABLE); /* Block TTY interrupts */
    if (bootstrap_enable  && tty->type == CTY_LINE) {
	/* only save characters when no one is logged into the console */
	if (!(tty->statbits & HASPROC)) {
	    reg_invoke_flh_log_putc(FLH_OP,c); /* for rxboot -- log char */
				               /* to flh buffer */
	}
    }

    /* 
     *  Put the char in the buffer in the next open slot
     */
    t = tty->outtail + 1;
    if (t == tty->outend)
	t = tty->outstart;

    /*
     * Check for buffer full, or for overwriting.  Also start an idle
     * timeout in case the hardware is busted.  Use session timeout if
     * software interrupts enabled (indicating we're in a telnet/rlogin/etc
     * session, or use the EXEC timeout.
     */
    if (t == tty->outhead) {
	while (t == tty->outhead) {
	    reset_interrupt_level(status);
	    tty_dallytimer(tty);
	    if (!startoutput(tty)) {
		tty->outcount = 0;
		return;
	    }
            edisms((blockproc *)sputBLOCK, (ulong)tty);
	    if (tty->statbits & (CARDROP|IDLE|IDLETIMEOUT)) {
		if (modem_debug) {
		    ttybug(tty, "CARDROP or IDLETIMEOUT while output wait");
		}
	        return;
	    }
	    raise_interrupt_level(TTY_DISABLE);
            t = tty->outtail + 1;	/* reset t in case of buffer clear */
            if (t == tty->outend)
	        t = tty->outstart;
	}
    }
    *t = (uchar) c;			/* store character */

    /* 
     * See if output has gone idle, possibly restart output.
     * If startoutput() fails, we have an unready modem.
     */

    tty->outtail = t;		/* Set new tail pointer */
    tty->outcount++;		/* Count another character */
    
    reset_interrupt_level(status);		/* Drop priority */

    /*
     * We've sent the character.  Now check if we should be
     * sending padding characters after it.  Note that quick_putc
     * doesn't block and doesn't call serial_putc, eliminating a
     * number of hangs and recursion problems.  We accept the very
     * occasional problem of the buffer filling while sending pad
     * characters.
     */
    if (tty->current.padding_ptr) {
	padding = tty->current.padding_ptr[c & 0x7F];
	while (padding--) 
	    quick_putc(tty, '\0');
    }
}

/*
 * generic_putpkt - Output a packet
 *
 * This is the dumb version of the output from a packet routine. We use
 * putc() to output the string. This routine never blocks, and returns
 * the count actually output.
 *
 * Note that this routine does not effect the share count - the caller
 * must free the packet when it is done with it.
 */

int generic_putpkt (tt_soc *tty, paktype *pak, uchar *datastart, int len)
{
    int count = 0;

    while (len-- && !outputblock(tty)) {
	putc(tty, tty->tty_datamask & (*datastart++));
	count++;
    }
    
    startoutput(tty);
    return(count);
}

/*
 * sputBLOCK
 * Block until tty output buffer is empty.
 * Note that some of our callers *require* the output buffer be empty!
 */

boolean sputBLOCK (tt_soc *tty)
{
    if (tty->statbits & (CARDROP|IDLE))
	return(FALSE);
    if (TIMER_RUNNING_AND_AWAKE(tty->dallytimer)) {
	tty->statbits |= (IDLETIMEOUT+CARDROP);
	return(FALSE);
    }
    return(tty->outcount ? TRUE : FALSE);
}

/*
 * serial_clearinput
 * Clear serial input buffers, i.e. reset pointers
 * Send XON if we sent XOFF before.
 */

void serial_clearinput (tt_soc *tty)
{
    leveltype status;

    if (tty->instart == NULL)
	return;
    status = raise_interrupt_level(TTY_DISABLE);
    tty->inend = tty->instart + tty->insize;
    tty->inhead = tty->instart;
    tty->intail = tty->instart;
    tty->incount = 0; 
    tty->isavedchar = -1;
    reset_interrupt_level(status);
    if (tty->statbits & SENTXOFF) {
	if (reg_invoke_tty_xon(tty) == FALSE) {
	    if(quick_putc(tty,tty->tty_startchar)) {
		tty->statbits &= ~SENTXOFF;
	    }
	}
    }
    if (tty->statbits & STOPPEDINPUT)
      startinput(tty);
}

/*
 * serial_clearoutput
 * Clear serial output buffers, i.e. reset pointers
 */

void serial_clearoutput (tt_soc *tty)
{
    leveltype status;
    paktype *pak;

    if (tty->outstart == NULL)
	return;
    status = raise_interrupt_level(TTY_DISABLE);
    tty->outend = tty->outstart + tty->outsize;
    tty->outhead = tty->outstart;
    tty->outtail = tty->outstart;
    tty->outcount = 0;
    pak = tty->outpak;
    if (pak) {
	tty->outpak = NULL;
	datagram_done(pak);
    }
    reset_interrupt_level(status);
}

/*
 * serial_outputpending
 * Return count of bytes buffered for output
 */

int serial_outputpending (tt_soc *tty)
{
    if ((tty->statbits & CARDROP) || (tty->statbits & SLIPMODE))
	clearoutput(tty);
    return(tty->outcount + (tty->outpak ? 1 : 0));
}

/*
 * Linewatch listens for process activation characters on inactive serial
 * lines.  TipTops are created by this process.  Linewatch is run as part
 * of the TTY Background process. This process also watches for background
 * text messages to be sent to terminal lines.
 */

ulong totalinput, totaloutput, irate, orate, sirate, sorate;
static int check_absolute_timeout = CHECK_TTY_CYCLE;

void linewatch (void)
{
    tt_soc *tty;
    int i, c, totallines;
    ulong itot=0, otot=0;
    uchar receivebuf[20];

    totallines = nconlines + nauxlines + nttylines;

    if(--check_absolute_timeout < 0)
	check_absolute_timeout = CHECK_TTY_CYCLE;
    for (i = 0; i < ALLlines; ++i) {
	tty = MODEMS[i];
	if (!tty)
	  continue;

        /*
         * Poll to see if callback timer has expired.
         */
        if (reg_invoke_callback_expired_tty(tty, FALSE)){
          if (modem_debug)
            buginf("\nTTY%t callback timer expired", tty->ttynum);
          return;
        }

	itot += tty->totalin;
	otot += tty->totalout;
	tty_messages(tty);

	if (tty->statbits & (SLIPMODE | HASPROC | AUTOBAUDING)) {
	    if ((!check_absolute_timeout) && (tty->type != CTY_LINE))
		check_line_absolute_timeout(tty);
	    continue;
	}

	if (tty->capabilities & (MODEMIN+MODEMOUT+MODEMRI))
	    continue;
	if (i >= totallines)
	  continue;

	/*
	 * If an activation character has been specified, look for it.
	 */
	if (tty->activatechar > 0) {
	    if (!inputpending(tty)) {		/* No input at all */
		continue;
	    }
	    c = quick_getc(tty);		/* get a character */
	    if (c < 0) {			/* if nothing ... */
		continue;			/* on to next line */
	    }
	    if (tty->capabilities & AUTOBAUD) {
		if (!(tty->statbits & AUTOBAUDING)) {
		    tty->isavedchar = c;
		    tty->tiptop = cfork((forkproc *)auto_baud(tty), (long)tty,
					0, "SetSpeed", tty->ttynum);
		    if (tty->tiptop > 0) {
			CLAIM_TTY(tty);
		        tty->statbits |= AUTOBAUDING;
			process_set_crashblock(tty->tiptop, TRUE);
		    }
		}
		continue;
	    }
	    c &= tty->tty_execmask;		/* look at ASCII chars */
	    if (c == (int) tty->activatechar) {	/* activation char? */
		start_process(tty);		/* yes, start a process */
	    } else if ((c == 0x4F) && (tty->capabilities & (MAYARAP|MUSTARAP)))
	    {
                memset(receivebuf, 0, 20);
                receivebuf[0] = c;
                quick_gets(tty, &receivebuf[1], 8);
                sprintf(receivebuf, "CONNECT\n");
                quick_puts(tty, receivebuf);
                startoutput(tty);
                process_sleep_for(ONESEC/4);
                quick_gets(tty, receivebuf, 7);
                start_process(tty);
            } else {
		tty_clearbuffers(tty);		/* flush buffers */
		tty->linenoise++;		/* count noise on line */
	    }
	} else {

	    /*
	     * If no activation character, start on any input.
	     */
	    if (inputpending(tty)) {
	        start_process(tty);
	    }
	}
    }
    irate = itot - totalinput;
    totalinput = itot;
    orate = otot - totaloutput;
    totaloutput = otot;
    sirate = (sirate*9 + irate)/10;
    sorate = (sorate*9 + orate)/10;
}

/*
 * start_exec_proc
 * Returns TRUE if we were able to fork the exec process.
 * We want the ability for other functions that call start_exec_proc to
 * know why start_exec_proc did not work and display the approperiate 
 * buginf message.
 */
boolean start_exec_proc (tt_soc * tty)
{
    ulong stacksize = EXEC_STACK_SIZE;

    if (tty->capabilities & NOEXEC)
	return(FALSE);
    if (reg_invoke_kerberos_is_running()) {
	stacksize = KERBEROS_STACK_SIZE;
    }
    tty->tiptop = cfork ((forkproc *)exec, (long)tty, stacksize,
			 "Exec", tty->ttynum);
    if (tty->tiptop == -1) {
	quick_puts(tty,"\r\n%% Unable to create EXEC - no memory or "
		   "too many processes\r\n");
	return(FALSE);
    } else {
	process_set_crashblock(tty->tiptop, TRUE);
	tty->statbits &= ~CARDROP;
	CLAIM_TTY(tty);
    }
    return(TRUE);
}

/*
 * autoselect_process
 * process to run on a line and sniff at traffic till we can make
 * an accurate guess as to what that traffic is.
 */
static forktype autoselect_process(tt_soc *tty)
{
    uint sample = 0;
    int ch;
    int count;
    pid_t pid;

    if (modem_debug)
	ttybug(tty, "Autoselect started");

    /*
     * If this is a ARAP or PPP callback, go straight to forking corresponding 
     * process via registry. Both ARAP and PPP callbacks initiate outgoing 
     * connections.
     */
    process_get_pid(&pid);
    if ((tty->acflags & ACF_CALLBACK_ARAP) || 
        (tty->acflags & ACF_CALLBACK_PPP)) {
        if (reg_invoke_exec_process(tty, 0)) {
	    if (tty->tiptop == -1 || tty->tiptop == pid) {
		/*
		 * Failed to create an ARAP process ?
		 * We'll have to clean up and hang up modem control.
		 */
		process_sleep_for(ONESEC);	/* Wait for messages */
		RELEASE_TTY(tty);
		terminate_line(tty);
	    }
	    tty->autoselectmask &= ~AUTOSELECTED;
	    process_kill(THIS_PROCESS);	/* It worked, stop autoselect */
	}
    }


    TIMER_START(tty->dallytimer, 2*ONEMIN);
    if (!(tty->capabilities & NO_FLUSH_ACTIVATE)) {
	process_sleep_for(ONESEC/2);
	clearinput(tty);
    }
    tty->autoselectmask &= ~(AUTOSELECTED|AUTOSELECT_AUTHORIZE);
    for (count = 0; count < AUTOSELECT_CHARACTERS; count++) {
	/*
	 * Avoid using getcwait, since it resets dallytimer to a timeout
	 * longer than we want.  The rather brief code here is enough.
	 */
	edisms((blockproc *)ttyioBLOCK, (long)tty);
	ch = getc(tty);

	if (ch == -1)		/* carrier drop, etc? */
	    break;		/* ...abort quickly. */
	if (ch == 13) {
	    /* Quick getout to start an Exec. */
	    if (modem_debug)
		ttybug(tty, "Autoselect starting EXEC");
	    start_exec_proc(tty);
	    if (tty->tiptop == -1 || tty->tiptop == pid) {
		/*
		 * Failed to create an exec process ?
		 * We'll have to clean up and hang up modem control.
		 */
		process_sleep_for(ONESEC);	/* Wait for messages */
		RELEASE_TTY(tty);
		terminate_line(tty);
	    }
	    tty->autoselectmask &= ~AUTOSELECTED;
	    process_kill(THIS_PROCESS);	/* It worked, stop autoselect */
	}
	sample |= ch & 0xFF;
	if (modem_debug)
	    buginf("\nTTY%t: Autoselect sample %x", tty->ttynum, sample);
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

	if (reg_invoke_exec_process(tty, sample)) {
	    if (tty->tiptop == -1 || tty->tiptop == pid) {
		/*
		 * Failed to create an exec process ?
		 * We'll have to clean up and hang up modem control.
		 */
		process_sleep_for(ONESEC);	/* Wait for messages */
		RELEASE_TTY(tty);
		terminate_line(tty);
	    }
	    tty->autoselectmask &= ~AUTOSELECTED;
	    process_kill(THIS_PROCESS);	/* It worked, stop autoselect */
	}
	sample <<= 8;
    }
    /*
     * Autoselect failed to find anything interesting.
     * clean up, including handling modem control
     */
    if (modem_debug)
	ttybug(tty, "Autoselect failed");
    death(tty);
}

/*
 * start_process
 * Start an exec on a serial line
 */

void start_process (tt_soc *tty)
{
    /*
     * System too crowded, no more people allowed on.
     */
    if (mempool_is_empty(MEMPOOL_CLASS_LOCAL)) {
	quick_puts(tty,"\r\n%% Low on memory; try again later\r\n");
	return;
    }

    /* 
     * Clear the callback timer to deactivate callback expiry
     */
    if (reg_invoke_callback_expired_tty(tty, TRUE)){
       if (modem_debug)
          buginf("\nTTY%t start_process - callback_expired_tty");
       return;
    }

    GET_TIMESTAMP(tty->time_activated);
    tty->notified = FALSE;

    /* For xtacacs */
    tty->tty_user_server = 0;
    tty->tty_slip_server = 0;

    /*
     * If we want to autoselect something, and we want to do it before 
     *  starting the login, start the autoselect process here.
     *
     * Do not start autoselect  if we have autoselect during-login. However, we
     * will ignore autoselect during-login if we have "no login", so that
     *  we don't end up with no autoselect at all once exec is fired.
     */
    if ((tty->autoselectmask & PARSER_AUTOSELECT_ALL) && 
	((!(tty->autoselectmask & PARSER_AUTOSELECT_DURING_LOGIN))
	 || (old_access_control && (tty->actype == AC_NONE)))) {

	tty->tiptop = cfork((forkproc *)autoselect_process, (long)tty, 0,
			    "AutoSelect", tty->ttynum);
	if (tty->tiptop != -1) {
	    CLAIM_TTY(tty);
	    process_set_crashblock(tty->tiptop, TRUE);
	    return;
	}
    }

    /*
     * See if this line wants to run a special process (SLIP, PPP, ARAP).
     * Return if someone successfully launched a process.
     */
    if (reg_invoke_exec_process(tty, 0))
	return;

    /*
     * No special process, so we want an EXEC, unless we've been
     * configured to not even start one of those (e.g. we're a printer).
     */
    start_exec_proc(tty);
}

/*
 * death
 * Terminate a tty top level process, reset data structures.
 */
void death (tt_soc *tty)
{
    long pid;


    if (modem_debug) {
	const char *procname;
	if (!process_get_name(THIS_PROCESS, &procname))
	    procname = NULL;
	buginf("\nTTY%t: Line reset by \"%s\"", tty->ttynum, procname);
    }
  

    /* Inform other folks, hoping to attend the funeral to get dressed
     */

    reg_invoke_death(tty);

    /*
     * Blow away output if it's more than MAX_TTY_OUTPUT_WAIT secs long
     * This will also take care of Xoff'ed lines
     * Actually we shouldn't wait for Xoff'ed lines, but there is no
     *   generic routine to tell us just that.
     * Re-enable tty..in case we were sitting Xoffed
     */

    startoutput(tty);
    tty_wait_output(tty, MAX_TTY_OUTPUT_WAIT);

    /* On vty's a clearoutput() can clear data just sent out. So clear */
    /*   only if there is more to send */

    if (outputpending(tty))
	clearoutput(tty);
    tty_portenable(tty);

    /*
     * Clean up exec parseblock if there is one
     */

    if (tty->csb != NULL)
	free_csb(&tty->csb);

    /*
     * Possibly remove VTY from logger queue
     */
    if (tty->type == VTY_LINE || tty->capabilities & MODEMMASK) {
	logger_ttydel(tty);
	while (QUEUESIZE(&tty->messageQ))
	  datagram_done(pak_dequeue(&tty->messageQ));
	tty->message_offset = 0;
	while (QUEUESIZE(&tty->asyncmsgQ))
	  datagram_done(pak_dequeue(&tty->asyncmsgQ));
	tty->asyncmsg_offset = 0;
	tty_wait_output(tty, MAX_TTY_OUTPUT_WAIT);/* Wait for logger output */
    }
    /*
     * If we're doing standard modem control, signal a hangup to the line
     * scanning process.  Wait up to MAX_TTY_OUTPUT_WAIT secs for output
     * to finish.
     */

    if (tty->capabilities & (MODEMMASK)) {
	if (!(tty->statbits & NOBANNER))
	    tty_wait_output(tty, MAX_TTY_OUTPUT_WAIT);
	else if (outputpending(tty))
	    process_sleep_for(ONESEC/4);	/* wait for echoing */
	if (modem_debug)
	    buginf("\nTTY%t: Modem: %s->HANGUP",
		   tty->ttynum, modem_statestring(tty));
	tty->statbits |= HANGUP;
	tty->statbits &= ~(READY + IDLE + CARDROP + RINGIN + AUTOBAUDING
			   + CONNECTED + DIALOUT + CALLBACK_IN_PROGRESS);
    } else {
	if (tty->statbits & DIALOUT) {

	/* Special case code for killing a Chat Dial-out line */
	/* Pulse DTR so that the modem kills the dial-out even though */
        /* there is no modem control enabled on the line */

	    if (modem_debug)
		ttybug(tty, "dropping DTR");
	    dtrstate(tty, FALSE);
	    if (!(tty->capabilities & MODEMDTR)) {
		process_sleep_for(ONESEC * 1);
		if (modem_debug)
		    ttybug(tty, "asserting DTR");
		dtrstate(tty, TRUE);
	    }
	}

	if (modem_debug)
	    buginf("\nTTY%t: Modem: %s->READY",
		   tty->ttynum, modem_statestring(tty));
	tty->statbits |= READY;		/* make sure line is ready */
	tty->statbits &= ~(HANGUP + IDLE + CARDROP + RINGIN + AUTOBAUDING
			   + CONNECTED + DIALOUT + CALLBACK_IN_PROGRESS);
    }

    /*
     * If we have a virtual terminal process, close the TCP connection.
     */
    if (tty->conntop) {
	conn_close(tty->conntop);
	tty->conntop = NULL;
    }

    if (tty->ustruct) {		/* clean up user stuff, including accounting */
	aaa_free_user(&tty->ustruct);
    }

    tty->statbits2 &= ~MODEM_CONFIGURED_2;

    pid = tty->tiptop;			/* copy process ID */
    tty->tiptop = -1;			/* say no more process */
    tty->autoselectmask &= ~AUTOSELECTED; /* not autoselected */
    tty_reset(tty);			/* reset terminal parameters */
    tty_deallocate(tty);		/* line is no longer assigned */
    reg_invoke_line_handoff();
    if (pid > 0)			/* if top level process */
	process_kill(pid);		/* then kill it (maybe ourself!) */
}

/*
 * quick_gets
 * A fast, special purpose getc for linewatch().
 * Ignores the carrier drop bit.  Never blocks.
 */

static int quick_gets(tt_soc *tty, char *string, int length)
{
    int count;

    for (count = 0; count < length; count++) {
        string[count] = quick_getc(tty);
    }
    return(length);
}


/*
 * kfork
 * This is like death, only it doesn't hang up phone lines.
 * SLIP uses this to get rid of the exec after setting things up.
 * This function is supposed to never return.
 */
void kfork (tt_soc *tty)
{

    /*
     * Clean up exec parseblock if there is one
     */

    if (tty->csb != NULL)
	free_csb(&tty->csb);
    texti_reset(tty);
    reg_invoke_kfork(tty);
    psioff(tty);			/* clear pseudo interrupts */
    tty->ivector = tty->ovector;	/* make input match output */
    if (tty->tiptop < 0) {
	errmsg(&msgsym(BADPID, SYS), tty->tiptop, tty->ttynum);
    }
    tty->tiptop = -1;			/* say no more process */
    process_kill(THIS_PROCESS);	        /* process destruction */
}

/*
 * quick_puts
 * Print an unformatted string on a terminal, never block.
 */

void quick_puts (tt_soc *tty, char *str)
{
    while (*str)
	quick_putc(tty,*str++);
}

/*
 * quick_getc
 * A fast, special purpose getc for linewatch().
 * Ignores the carrier drop bit.  Never blocks. 
 */

int quick_getc (tt_soc *tty)
{
    int ch;
    leveltype status;

    status = raise_interrupt_level(TTY_DISABLE);	/* lock out tty interrupts */
    if (tty->incount == 0) {
	ch = -1;
    } else {
	tty->statbits &= ~LASTWASBREAK;
	ch = (int) *tty->inhead++;
	if (tty->inhead >= tty->inend)
	    tty->inhead = tty->instart;
	--tty->incount;
	if ((tty->statbits & SENTXOFF) && 
	    (tty->incount < (tty->insize >> 1))) {
	    if (reg_invoke_tty_xon(tty) == FALSE) {
		if (quick_putc(tty,tty->tty_startchar)) {
		    tty->statbits &= ~SENTXOFF;
		}
	    }
	}
    }
    reset_interrupt_level(status);
    return(ch);
}

/*
 * quick_putc
 * Output a character on a serial line, never block.
 */

boolean quick_putc (tt_soc *tty, int c)
{
    uchar *t;
    leveltype status;
    boolean start = FALSE;

    status = raise_interrupt_level(TTY_DISABLE); /* Disable interrupts */

    /* 
     *  Put the char in the buffer in the next open slot
     */
    if (tty->outtail) {
	t = tty->outtail + 1;
	if (t == tty->outend)
	    t = tty->outstart;
    } else if (tty->outpak) {
	/*
	 * If outtail is null, that means that we are currently outputting
	 * directly from a packet buffer.  Packet buffers will almost ALWAYS
	 * have room at the end to add a few characters, so try doing that.
	 * (If it doesn't work, just abort!)
	 */
	if (tty->outcount > (pak_get_pool_size(tty->outpak) + 5))
	    return(FALSE);
	*tty->outend++ = c;	/* outend point past end of buffer */
	tty->outcount++;
	reset_interrupt_level(status);
	return(TRUE);
    } else {
	reset_interrupt_level(status);
	return(FALSE);		/* Who knows what is going on... */
    }

    /*
     * Check for buffer full.  If it is, start output
     * and exit, else store the character.
     */
    if (t == tty->outhead) {
	reset_interrupt_level(status);
        (void)startoutput(tty);
	return(FALSE);
    }
    *t = (uchar) c;

    /* 
     * See if output has gone idle, possibly restart output.
     * If startoutput() fails, we have an unready modem.
     */

    if (tty->outcount++ == 0)
      start = TRUE;		/* If this is the first byte, start output */

    tty->outtail = t;

    reset_interrupt_level(status);

    if (start) {
        if (!startoutput(tty)) {
	    tty->outcount = 0;
	}
    }

    return(TRUE);
}

/*
 * rotary_hunt
 * Given a line in a rotary group, release our hold on it and step to
 * the next free line, claiming it.
 */

tt_soc *rotary_hunt (tt_soc *tty)
{
    int i, rotary, baseline;
    conntype *conn;
    vectortype *oldvector;

    rotary = tty->rotary;		/* rotary group number */
    baseline = tty->ttynum;		/* initial line number */
    conn = tty->conntop;		/* the TCP connection */
    oldvector = tty->ivector;		/* Old input vector */

    /*
     * Now release the current tty line.
     */
    if (modem_debug)
        ttybug(tty,"trying next line in rotary");
    tty->conntop = NULL;		/* no vty driver process */
    if (tty->capabilities & MODEMMASK) {
	tty->statbits &= ~(READY|HANGUP|RINGIN); /* no other states */
	tty->statbits |= IDLE;
    } else
	tty->statbits &= ~(HANGUP|RINGIN);	/* no other states */
    tty->nconns--;			/* one less connect block */
    conn->tty = NULL;			/* erase back pointer */
    tty->tiptop = -1;			/* no top process */
    psioff(tty);			/* clear pseudo interrupts */
    tty_deallocate(tty);		/* line is no longer assigned */

    /*
     * Now find the next free line in the rotary group.
     */
    for (i = baseline+1; i < ALLlines; i++) {
	tty = MODEMS[i];
	if ((tty->rotary != rotary) 
		|| (tty->statbits & HASPROC) 
		|| tty->answercountdown 
		|| reg_invoke_modem_hardware_in_use(tty))
	    continue;
	if ((tty->capabilities & MODEMCTS) && (tty->statbits & IDLE))
	    continue;
	tty = (tt_soc *) tty_allocate(i,TRUE);
	if (!tty)
	    continue;
	if (conn_settty(conn, tty)) {		/* Try to set new tty */
	    tty->conntop = conn;	/* remember the driver conn */
	    conn->tty = tty;			/* set up back pointer */
	    tty->statbits &= ~(CARDROP+HANGUP+IDLE);
	    tty->statbits |= READY;
	    tty->conncount++;			/* another use */
	    tty->nconns++;			/* Count the connect block */
	    tty->ivector = oldvector;		/* set new input source */
	    switch (tty->type) {
	      case VTY_LINE:
	        tty->ovector = oldvector;	/* Set output source */
	        break;
	      default:				/* use standard output */
	        break;
	    }
	    return(tty);			/* now try again */
	}
	tty_deallocate(tty);			/* Failed, free up tty */
    }
    if (modem_debug)
	buginf(" -- no more free lines");
    conn_close(conn);
    return(NULL);
}

void async_daemon_kill (tt_soc *tty)
{
    if (tty) {
	if (!conn_alive(tty->conntop)) /* connection closed? */
	    tty->statbits &= ~CARDROP;	/* yes, reset CARDROP */
	if (tty->tty_sessiontimeout) {
	    TIMER_START(tty->dallytimer, tty->tty_sessiontimeout);
	} else {
	    TIMER_START(tty->dallytimer, 2*ONEMIN);
	}
	/* wait for output to finish */
	edisms((blockproc *)finishoutBLOCK, (ulong)tty);
	death(tty);				/* close down */
    }
    process_kill(THIS_PROCESS);			/* just in case */
}

void async_daemon_attach (tt_soc *tty)
{
    boolean connectflag;
    int status;
    vectortype *tmpvector;

    connectflag = FALSE;		/* not yet connected */
    TIMER_STOP(tty->conntop->dispatchtimer);	/* no dispatch */

    /*
     * If we are doing modem control out (active), then we wait for
     * CTS to come up.  If no CTS after a while, we either close the
     * connection and kill this process, or in the case of a rotary,
     * attempt to step to the next unused line.
     */
    tty_dallytimer(tty);
    while (!connectflag) { 
	tty->statbits &= ~IDLETIMEOUT;	/* no idle timeout yet */
	TIMER_STOP(tty->dallytimer);	/* no timer ticking yet */
	if ((tty->capabilities & MODEMMASK) == MODEMOUT) {
	    tty->answercountdown = tty->answertimeout; /* init the timeout */
	    tty->statbits &= ~(IDLE + CARDROP + READY + HANGUP);
	    tty->statbits |= RINGIN;	/* waiting for CTS to come up */
	    /*
	     * Clear the I/O of the serial line (only!).  It may have
	     * data left over from a previous connection.  The net connection
	     * may already have valid data on it, so leave it alone.
	     */
	    (*(tty->ovector->vclearoutput))(tty);
	    (*(tty->ovector->vclearinput))(tty);

	    if (modem_debug)
	        ttybug(tty,"asserting DTR");
	    dtrstate(tty,TRUE);		/* assert DTR */
	    /* block until READY or IDLE */
	    edisms((blockproc *)readyBLOCK,(long)tty);
            if (tty->statbits & (IDLE+CARDROP)) { /* must have timed out */
		if (tty->rotary && (tty = rotary_hunt(tty)))
		    continue;		/* maybe hunting? */
		async_daemon_kill(tty);
	    }
	    connectflag = TRUE;			/* finally connected */
	    process_set_ttysoc(THIS_PROCESS,tty);/* Change stdio tty */
	} else {
	    /*
	     * Clear the I/O of the serial line (only!).  It may have
	     * data left over from a previous connection.  The net connection
	     * may already have valid data on it, so leave it alone.
	     */
	    (*(tty->ovector->vclearoutput))(tty);
	    (*(tty->ovector->vclearinput))(tty);
	    tty_portenable(tty); /* Make sure port is enabled */
	    if (modem_debug)
	      ttybug(tty,"asserting DTR");
	    dtrstate(tty,TRUE);		/* always assert DTR */
	    connectflag = TRUE;		/* we are connected */
	    process_set_ttysoc(THIS_PROCESS,tty);/* Change stdio tty */
	}
    }
    tty->statbits |= CONNECTED;

    if ((banner_motd || banner_temp || linenumber_enable) &&
	! (tty->conntop->status & CONN_STREAM)) {
	tmpvector = tty->ovector;
	tty->ovector = tty->ivector;
        if (banner_motd && ! ((tty->capabilities & NOEXECBANNER) ||
                              (tty->capabilities2 & NOMOTDBANNER_2)))
	    printf("%r", banner_motd);
	if (banner_temp)
	    printf("%r", banner_temp);
	if (linenumber_enable) {
	    printf("\n%s line %t %s\n", hostname, tty->ttynum,
		get_tty_location(tty));
	}
	startoutput(tty);
	tty->ovector = tmpvector;
    }
    /*
     * Perhaps check for a password.  Diddle the IO vectors and
     * use the EXEC code...
     */
    if (!old_access_control || (tty->actype != AC_NONE) ) {
	status = tty->tty_dispatchtimeout;
	tty->tty_dispatchtimeout = 0;
	tty_clearbuffers(tty);
	tmpvector = tty->ovector;
	tty->ovector = tty->ivector;
	/* Do a login, but not if we have already authenticated (ie. NASI) */
	if (!aaa_get_user_p(tty->ustruct, NULL) && !check_login(tty)) {
	    startoutput(tty);
	    process_sleep_for(2*ONESEC);	/* pause to read the message */
	    tty->ovector = tmpvector;
	    tty->tty_dispatchtimeout = status;
	    async_daemon_kill(tty);
	}
	if (old_access_control) {
	    printf("\nPassword OK\n");
	}
	startoutput(tty);
	tty->ovector = tmpvector;
	tty->tty_dispatchtimeout = status;
    }

    /* Report initial port characteristics */

    conn_report(tty->conntop);

    /*
     * Display the banner
     */

    if ((banner_incoming) && ! (tty->conntop->status & CONN_STREAM)) {
	tmpvector = tty->ovector;
	tty->ovector = tty->ivector;
	if (banner_incoming)
	  printf("%r", banner_incoming);
	reg_invoke_emergency_message(EMERGENCY_LINE_BANNER);
	startoutput(tty);
	tty->ovector = tmpvector;
    }
    reg_invoke_aaa_acct_conn(tty->ustruct, NULL, tty->conntop);
}

/*
 * readyBLOCK
 * Block until READY bit is lit or until a CTS timeout event (IDLE bit)
 */

boolean readyBLOCK (tt_soc *tty)
{
    return((tty->statbits & (IDLE+READY+CARDROP)) ? FALSE : TRUE);
}

boolean finishoutBLOCK (tt_soc *tty)
{
    if (tty->statbits & (CARDROP|IDLE))
	return(FALSE);
    if (TIMER_RUNNING_AND_AWAKE(tty->dallytimer))
	return(FALSE);
    return(outputpending(tty) ? TRUE : FALSE);
}

/*
 * tty_async_display
 * This routine extracts one byte from "pak" and display the character
 * to tty, it is called by tty_messages. If the output is blocked,
 * this routine will return FALSE otherwise it will return TRUE.
 */
static boolean tty_async_display (tt_soc *tty, paktype *pak, int *ptr)
{
    leveltype status;
    uchar ch;

    if (outputblock(tty))
	return(FALSE);

    status = raise_interrupt_level(ALL_DISABLE);

    ch = pak->datagramstart[(*ptr)++];
    ch &= tty->tty_execmask;
    reset_interrupt_level(status);

    if (ch == '\n') {
	if (tty->statbits & LASTWASCR) {
	    tty->statbits &= ~LASTWASCR;
	} else {
	    ch = '\r';
	    tty->statbits |= LASTWASCR;
	    (*ptr)--;
	}
    }
    putc(tty, ch);
    return(TRUE);
}

/*
 * tty_messages - Handle background messages for a terminal
 *
 * This routine is used to display background messages queued to a terminal
 * line. Since we are called as part of the tty_background process, we must
 * never block. To accomplish this, we keep the current offset of the
 * message being output in the tty block. 
 *
 * If logging synchronous feature is turned on, this function will process
 * two queues: asyncmsgQ and messageQ.
 */

static void tty_messages (tt_soc *tty)
{
    elementtype *element;
    paktype *pak;
    uchar ch;
    boolean output = FALSE;
    boolean proceed;
    boolean release_lock = FALSE;
    boolean reprint_prompt = FALSE;
    leveltype status;

    if (tty->statbits & AUTOBAUDING)
      return;

    if ((!(tty->statbits & HASPROC)) &&
	(tty->type == VTY_LINE || tty->capabilities & MODEMMASK))
      return;

    /* if logging synchronous is turned on, then display asyncmsgQ */
    if (tty->log_sync) {
	while (1) {
	    element = (elementtype *)tty->asyncmsgQ.qhead;
	    if (!element) {
		reprint_prompt = TRUE;
		break;
	    }
	    pak = element->pakptr;

	    if (!tty_async_display(tty, pak, &tty->asyncmsg_offset))
	      break;

	    if (pak->datagramsize <= tty->asyncmsg_offset) {
		datagram_done(pak_dequeue(&tty->asyncmsgQ));
		tty->asyncmsg_offset = 0;
		tty->statbits &= ~LASTWASCR;
	    }
	    output = TRUE;
	}
    }

    proceed = !tty->log_sync || tty->log_sync_lock != LOG_SYNC_IS_LOCK;

    while (proceed) {

	status = raise_interrupt_level(ALL_DISABLE);
	element = (elementtype *)tty->messageQ.qhead;
	reset_interrupt_level(status);
	if (!element) {
	    reprint_prompt = TRUE;
	    release_lock = TRUE;
	    break;
	}
	pak = element->pakptr;
	if (!tty_async_display(tty, pak, &tty->message_offset))
	    break;
	reprint_prompt = FALSE;
	output = TRUE;

        /* signal to getcwait() that output is processed */
	if (tty->log_sync && tty->log_sync_lock == LOG_SYNC_IS_UNLOCK)
	    LOG_SYNC_USED(tty);

	if (pak->datagramsize <= tty->message_offset) {
	    /* if lost message is found, format the message and put into
	       the current buffer */
	    if (pak->length > 0) {
		char *cp;
		pak->datagramstart = pak->data_area;
		cp = (char*) pak->datagramstart;
		cp = errfmt(&msgsym(MSGLOST, SYS), cp, MEDDATA + ENCAPBYTES,
			    pak->length);
		pak->datagramsize = (uchar*) cp - pak->datagramstart;
		pak->length = 0;
	    } else
	        datagram_done(pak_dequeue(&tty->messageQ));
	    tty->message_offset = 0;
	    tty->statbits &= ~LASTWASCR;
	    reprint_prompt = TRUE;
	    /* If lock is requested, and buffer is exhausted, release 
	       the lock */
	    if (tty->log_sync && tty->log_sync_lock == LOG_SYNC_IS_REQ) {
		release_lock = TRUE;
		proceed = FALSE;
	    }
	} else {
	    ch = pak->datagramstart[tty->message_offset];
	    if (ch == '\n' && !(tty->statbits & LASTWASCR)) {
		/* if lock is requested and a complete line is output,
		   release the lock */
		reprint_prompt = TRUE;
		if (tty->log_sync && tty->log_sync_lock == LOG_SYNC_IS_REQ) {
		    release_lock = TRUE;
		    proceed = FALSE;
		}
	    }
	}
    }
    if (output) {
	(void)startoutput(tty);
	if (tty->log_sync && reprint_prompt)
	    tty->log_sync_reprint_type |= LOG_SYNC_RPRNT_REQ;
    }

    /* Release the lock only by request */
    if (tty->log_sync && release_lock) {
	if (tty->log_sync_lock == LOG_SYNC_IS_REQ)
	    LOG_SYNC_LOCK(tty);
	else if (tty->log_sync_lock != LOG_SYNC_IS_LOCK)
	    LOG_SYNC_UNLOCK(tty);
    }

}

/*
 * tty_sendmessage - Send a message to a terminal. If logging synchronous
 * is turned on, it will send an asynchronous message to a terminal.
 */

void tty_sendmessage (tt_soc *tty, paktype *pak)
{
    if (tty->statbits & AUTOBAUDING)
      return;

    if ((!(tty->statbits & HASPROC)) &&
	(tty->type == VTY_LINE || tty->capabilities & MODEMMASK))
      return;

    /* see if this tty has any special protocol requirements. */
    if (reg_invoke_proto_send_message(tty, pak))
	return;

    pak_lock(pak);			/* Place a lock on this packet */
    pak->length = 0;
    /* If synchronous is turned on, add the packet to asynchronous queue
       otherwise add it to normal message queue. Note: no error is generated
       if enqueue() fails */
    if (tty->log_sync) {
	if (!pak_enqueue(&tty->asyncmsgQ, pak))
	    datagram_done(pak);		
    } else {
	if (!pak_enqueue(&tty->messageQ, pak))
	    datagram_done(pak);		
    }
}

/*
 * ttyline_sendmessage
 *
 * Send a message to a terminal, given its TTY line number.
 * If the line number is -1, it is sent to all terminals.
 *
 * Returns without comment if the line number is invalid.
 *
 * The caller is expected to free the paktype structure.
 */
void ttyline_sendmessage (int linenum, paktype *pak)
{
    tt_soc *soc;
    int firstline, lastline;

    if (linenum == -1) {		/* Send everywhere? */
	firstline = 0;
	lastline = ALLlines;
    } else {
	if (linenum < 0 || linenum >= ALLlines)	/* Illegal line number */
	    return;
	firstline = linenum;
	lastline = linenum + 1;
    }
    for (linenum = firstline;  linenum< lastline; linenum++) {
        soc = MODEMS[linenum]; 
	if ((soc) && (soc->statbits & READY))
	    tty_sendmessage(soc, pak);
    }
}

/*
 * tty_send_logmessage - Send a log message to a terminal.
 * If logging synchronous is turned on, high severity level (configurable)
 * will be added to async queue by calling tty_sendmessage().
 * Queue overflow will be checked too.
 */

void tty_send_logmessage (tt_soc *tty, paktype *pak, int severity, int count)
{
    if (tty->statbits & AUTOBAUDING)
      return;

    if ((!(tty->statbits & HASPROC)) &&
	(tty->type == VTY_LINE || tty->capabilities & MODEMMASK))
      return;

    if (tty->log_sync && tty->log_sync_level >= severity) {
	tty_sendmessage(tty, pak);
    } else {
	pak_lock(pak);			/* Place a lock on this packet */
	/* if queue overflow, update message lost count */
	if (tty->log_sync && QUEUEFULL(&tty->messageQ) && 
	    tty->log_sync_msgsize != 0) {
	    tty_log_lostcount(tty, count);
	    datagram_done(pak);
	    return;
	}
	pak->length = 0;        	/* Initialize lost messages count */
	if (!pak_enqueue(&tty->messageQ, pak))
	    datagram_done(pak);		/* if the enqueue fails, release */
    }
}

/*
 * tty_log_lostcount
 *
 * This function increment message lost count. The count is accumulated in
 * pak->length. It is used for "logging sychronous" feature.
 *
 */
void tty_log_lostcount (tt_soc *tty, int count)
{
    elementtype *e;

    e = (elementtype*) tty->messageQ.qtail;
    /* if paktype is found on the end of the queue, increment the
       message lost count */
    if (e)
        e->pakptr->length += count;
}

/*
 * start_sessiontimeout
 * start (if possible) session timeout timer, and (if possible) session 
 * timeout warning timer.
 * assumption: tty->tty_sessiontimeout is non zero
 */

void start_sessiontimeout (conntype *conn, tt_soc *tty)
{
    TIMER_START(conn->sessiontimer, tty->tty_sessiontimeout);
    if (tty->sessiontimeout_warning) {
	TIMER_START(conn->sessiontimer_warning, tty->tty_sessiontimeout -
		tty->sessiontimeout_warning);
    }
}


/*
 * tty_wait_output
 * wait for given number of secs to output to finish
 */
void tty_wait_output (tt_soc *tty, int timeout_secs)
{
    while ((!outputxoffed(tty)) && outputpending(tty) && timeout_secs--)
	process_sleep_for(ONESEC);
}

static void check_line_absolute_timeout (tt_soc *tty)
{
    paktype *pak;
    int remain_t;
    int len, size;
    conntype *con;
    char strng[] = "\n[**Session %3d Idle Disconnect Warning**]\n %s \n";

    /* session timeout && session disconnect warning must be config */
    /* can't confuse PPP, SLIP, XREMOTE or ARAP with ASCII messages */
    if (tty && tty->tty_sessiontimeout && tty->sessiontimeout_warning  &&
       (tty->statbits & READY) && !(tty->statbits & SLIPMODE) && 
       !tty->xrcb && !tty->arapptr) {
	con = (conntype *) tty->connhead.qhead;
	while (con) {
	    if (TIMER_RUNNING_AND_SLEEPING(con->sessiontimer) &&
	       TIMER_RUNNING_AND_AWAKE(con->sessiontimer_warning)) {
		TIMER_STOP(con->sessiontimer_warning);
		/*
		 * size has a fudge of + 1
		 *    %s 2 over, %3d even, '\0' 1 under
		 */
		size = strlen(tty->sessiontimeout_warning_msg) + strlen(strng);
		pak = getbuffer(size);
		if (pak == NULL)
		    break;
		len = sprintf(pak->datagramstart, strng, con->number,
				tty->sessiontimeout_warning_msg);
		pak->datagramsize = len;
		tty_sendmessage(tty, pak);
		datagram_done(pak);
	    }
	    con = con->link;
	}
    }

    if (!(tty->tty_absolutetimeout && TIMER_RUNNING(tty->time_activated)))
	return;

    remain_t = tty->tty_absolutetimeout - ELAPSED_TIME(tty->time_activated);

    /* notify user of any impending logout due to absolute timer */
    if (tty->logoutwarning && (!tty->notified) &&
       !(tty->statbits & SLIPMODE) && !tty->xrcb && !tty->arapptr &&
       (remain_t < tty->logoutwarning)) {

	pak = getbuffer(MEDDATA);

	if (pak) {
	    pak->datagramstart = pak_center(pak);

	    strcpy(pak->datagramstart,
		   "\n*\n*\n* Line timeout expired\n*\n*");
	    pak->datagramsize = strlen(pak->datagramstart);
	    tty_sendmessage(tty, pak);
	    datagram_done(pak);
	    if (modem_debug)
		buginf("\nTTY%t: TTY Absolute Logout at T + %u seconds",
		       tty->ttynum, remain_t/ONESEC);
	}
	tty->notified = TRUE;

    } else if (remain_t <= 0){
	if (modem_debug)
	    ttybug(tty, "TTY Absolute Logout");
	if (tty->statbits & SLIPMODE) {
	    if (tty->capabilities & MODEMMASK) {
		tty->statbits |= HANGUP;
		tty->statbits &= ~(READY + IDLE + CARDROP + RINGIN +
				   AUTOBAUDING + CALLBACK_IN_PROGRESS);
	    } else
		tty_if_reset(tty);  /* apply reset function */
	}
	else if (tty->statbits & HASPROC)
	    tty->statbits |= CARDROP;
	else {
	    if (tty->type != VTY_LINE)
		tty_clearbuffers(tty);
	    death(tty);
	}
    }
}

/*
 * tty_background
 *
 * Process spawned by Init to handle events that occur with one
 * second granularity
 */
process tty_background (void)
{
    process_wait_on_system_init();
    /*
     * Certain other processes, notably the "script startup" code, need
     * to get first crack at these lines.  Since system_init lets pretty
     * much everyone start at once, it random order, with no other
     * interlocking, it seems easiest to just wait "long enough".
     */
    process_sleep_for(ONESEC*5);
    while (TRUE) {
	modem_scanner();	/* watch for async line state changes */
	linewatch();		/* start processes on async lines */
	process_sleep_periodic(ONESEC);	/* run once a second */
    }
}

static void process_death_hook (pid_t pid)
{
    tt_soc *tty;

    if (!process_get_ttysoc(pid, &tty))
	return;
    if (tty == NULL)
	return;
    death(tty);
}

void tty_core_init (void)
{
    reg_add_process_death(process_death_hook, "process_death_hook");
}
