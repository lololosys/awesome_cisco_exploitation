/* $Id: x29.c,v 3.13.18.8 1996/08/28 13:23:55 thille Exp $
 * $Source: /release/112/cvs/Xsys/x25/x29.c,v $
 *------------------------------------------------------------------
 * x29.c -- X25 Packet assembler/disassembler code.
 *
 * November 1987,  Bill Westfield
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x29.c,v $
 * Revision 3.13.18.8  1996/08/28  13:23:55  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.13.18.7  1996/08/07  09:04:56  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.13.18.6  1996/07/26  19:13:31  swenson
 * CSCdi64070:  crash in x29_command if attempt to delete undefined profile
 * Branch: California_branch
 *
 * Revision 3.13.18.5  1996/06/28  23:33:05  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.13.18.4  1996/05/30  23:47:07  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.13.18.3  1996/04/19  15:49:07  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.2.6.9  1996/04/16  22:25:24  che
 * Branch: IosSec_branch
 * More code changes mandated by code review comments.
 *
 * Revision 2.2.6.8  1996/04/16  03:58:52  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 2.2.6.7  1996/04/09  22:18:05  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.2.6.6  1996/03/05  04:45:50  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 2.2.6.5  1996/02/21  04:25:27  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.2.6.4  1996/01/23  07:10:42  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 2.2.6.3  1995/12/19  04:28:28  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 2.2.6.2  1995/12/17  02:46:35  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.2.6.1  1995/11/17  18:55:20  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.13.18.2  1996/04/02  19:25:01  swenson
 * CSCdi52237:  pad doesnt respond to set or set and read with no
 * parameters
 * Branch: California_branch
 *
 * Revision 3.13.18.1  1996/03/18  22:50:50  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.10.2.3  1996/03/14  01:37:44  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.10.2.2  1996/03/07  11:20:09  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.10.2.1  1996/02/20  21:57:02  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.13  1996/02/07  16:18:03  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.12  1996/02/01  06:13:16  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.11  1996/01/26  18:32:02  eschaffe
 * CSCdi46665:  bus error at PC 0x328FAEE, address 0xD0D0D89
 * (_pad_putbyte).
 * Check to make sure that we are still in data transfer or in flow
 * control ready state before continuing the write.
 *
 * Revision 3.10  1996/01/22  07:46:50  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.9  1996/01/12  02:41:37  billw
 * CSCdi44858:  use #define for AAA attribute values
 * Bring the AV pairs used up to spec level...
 *
 * Revision 3.8  1995/12/15  23:01:10  billw
 * CSCdi45506:  Save 124 bytes per tty by handling padding differently
 * and a in statbits, and maybe an instruction or two as well!
 *
 * Revision 3.7  1995/12/13  00:33:36  billw
 * CSCdi44685:  hack is inadequately documented
 *
 * Revision 3.6  1995/12/12  00:09:53  billw
 * CSCdi44078:  AAA Accounting packets for PAD connection lack protocol
 * attribute
 *
 * Revision 3.5  1995/12/07  06:00:25  eschaffe
 * CSCdi43048:  x29 profile not reliably sent to calling pad
 *
 * Revision 3.4  1995/11/20  21:00:35  eschaffe
 * CSCdi14369:  Cannot set a PAD profile for incoming calls to a PT
 *
 * Revision 3.3  1995/11/20  20:50:25  eschaffe
 * CSCdi40878:  Spurious access in pad_putsetup at reconnection
 *
 * Revision 3.2  1995/11/17  18:08:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:54:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.16  1995/11/08  21:37:30  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.15  1995/10/21  07:43:16  billw
 * CSCdi42559:  exec stack size wrong on incoming PAD vtys.
 *
 * Revision 2.14  1995/10/17  22:38:14  eschaffe
 * CSCdi42345:  False log error message is generated if the packet is full
 *
 * Revision 2.13  1995/10/13  22:13:40  eschaffe
 * CSCdi41961:  Crash in pad_putbyte()
 *
 * Revision 2.12  1995/09/11  21:43:15  tkolar
 * CSCdi40022:  Telnet speed is retained even after the session is closed
 * Move passspeed and maxpassspeed into the userdata struct.
 *
 * Revision 2.11  1995/08/08  16:51:34  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.10  1995/08/06 01:15:21  eschaffe
 * CSCdi36149:  ppp data stuck in virtual interface output queue
 *
 * Revision 2.9  1995/08/03  23:29:13  billw
 * CSCdi37114:  access class on translate checks destination address not
 * source
 *
 * Revision 2.8  1995/07/24  23:55:59  eschaffe
 * CSCdi36312:  Memory leak in Middle and Big buffers
 *
 * Revision 2.7  1995/07/20  07:38:36  hampton
 * Convert X.25 to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37392]
 *
 * Revision 2.6  1995/07/14 07:03:38  hampton
 * Convert Terminal Server to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi37093]
 *
 * Revision 2.5  1995/06/20 03:45:22  ahh
 * CSCdi36095:  Connection routines use msclock
 * Remove more msclock references.
 *
 * Revision 2.4  1995/06/17  06:27:04  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.3  1995/06/13  16:38:46  syiu
 * CSCdi35685:  Async callback rotary should be in reverse oder
 *
 * Revision 2.2  1995/06/10  00:05:13  eschaffe
 * CSCdi35298:  Make sure all buginf messages are enabled by
 * "translate_debug" or "pad_debug" boolean.
 *
 * Revision 2.1  1995/06/07  23:23:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "subsys.h"
#include "address.h"
#include "packet.h"
#include "config.h"
#include "media_registry.h"
#include "../pt/pt_registry.h"
#include "parser.h"
#include "parser_defs_x25.h"
#include "ttysrv.h"
#include "logger.h"
#include "../x25/msg_pad.c"		/* Not a typo, see logger.h */
#include "../ui/debug.h"
#include "name.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "../tcp/telnet.h"
#include "connect.h"
#include "x25.h"
#include "x25_switch.h"
#include "pad.h"
#include "pad_debug.h"
#include "../pt/translate.h"
#include "../pt/pt_debug.h"
#include "../os/async.h"
#include "../snmp/sr_old_ltsmib.h"
#include "aaa.h"
#include "stacks.h"
#include "sys_registry.h"
#include "../h/auth_registry.regh"
#include "../ui/command.h"
#include <ciscolib.h>
#include <string.h>

/* external declarations */
extern regexpacctype *x29_accesslists[];
extern int regexpaccess_command(parseinfo *, regexpacctype *array[]);

/* local storage */
vectortype PADvector;
queuetype profileQ;
connvector pad_connvector;		/* X.25 connection vector */
int pad_encapsfailed = 0;
int x29_inviteclear_time = X29_INVITECLEAR_DEFAULT;

static char *const messtype[8] = {
	"Parameter indication",
	"Inv to clear",
	"Set X.3 param",
	"Break indication",
	"Read X.3 param",
	"Error",
	"Set and Read",
	"" };

char *padques = "(??)";

/*
 * Forward references
 */
void pad_conn_acct (conntype *, acctinfo *);

/*
 * This code supports communications between two PADs (X.25 terminal server),
 * and handles the interface between the PAD and X.25 and other things at that
 * logical level.  It interprets "pad messages" (x25 Q bit set to 1),
 * preforming appropriate actions on the terminal data base, and sending
 * appropriate responses.  See also X28.c
 */

/*

   Terminal	cisco Box	X25network	cisco Box	Terminal
	|- X28.c -|- X29.c -|		     |- X29.c -|-- X28.c --|
     exec or X.28 |  X.29   | X.25	X.25 |  X.29   | virtual tty |
    local editing | listen to and
    pad_telnet	  | form control packets
    user interface| create and listen for (?) connections.

*/



/*
 * X.3 parameters, and how they map onto features provided by the ASM.
 *
 *  Parameter			ASM mapping
 *  1	(recall character)	always returns not possible.  Local escape
 *				is set via exec commands.
 *  2	(echo)			controlls local echo (handled here)
 *  3	(data forwarding)	changes DISPATCH-CHAR array (done)
 *  4	(idle timer)		changes DISPATCH-TIME (done)
 *  5	(ancillary device)	changes FLOWCONTROL ? (done)
 *  6	(PAD service control)	always 0 (sort of controlled by debug option)
 *  7	(break handling)	Fully supported herein
 *  8	(discard output)	Fully supported herein
 *  9	(CR padding)		changes PADDING array.
 * 10	(Line folding)		???
 * 11	(speed)			Fully supported herein
 * 12	(Flow control)		Changes FLOWCONTROL ?
 * 13	(Linefeed insertion)	???
 * 14	(Linefeed padding)	changes PADDING array
 * 15	(Editing)		Controls local editing ???
 * 16	(delete character)
 * 17	(Delete line)
 * 18	(display line)
 * 19	(editing service signals)	???
 * 20	(echo mask)		???
 * 21	(parity)		???
 * 22	(Page wait)		???
 */

uchar ciscoX3defaults[NX3PARAMETERS] = {
0,
1,
1,
2,
1,
0,		/* fill in  (software flow control) */
0,
4,
0,
0,		/* fill in (padding) */
0,
14,		/* fill in (speed) */
0,		/* fill in (software flowcontrol) */
0,
0,		/* padding */
0,CH_DEL,CH_NAK,CH_DC2,	/* editing  (del, ctrl-U, ctrl-R) */
0,0,0,0
};

/*
 * PAD subsystem header
 */

#define PAD_MAJVERSION 1
#define PAD_MINVERSION 0
#define PAD_EDITVERSION  1

SUBSYS_HEADER(pad, PAD_MAJVERSION, PAD_MINVERSION, PAD_EDITVERSION,
              pad_init, SUBSYS_CLASS_PROTOCOL,
              "seq: x25",
              "req: x25");

/*
 * pad_init
 * Set up global X.29 stuff
 */
void pad_init (subsystype *subsys)
{
    /*
     * Set up debugging flags
     */
    pad_debug_init();
    padname_init();
    padvty_init();
    pad_parser_init();
    pad_enable = TRUE;
}

/*
 * pad_x3init
 * Set up the initial (outgoing) X3 parameters.  Some of these come from
 * the current terminal configuration, and some are defaulted.
 */
void pad_x3init (pcbtype *pcb, tt_soc *tty)
{
    bcopy(ciscoX3defaults,pcb->paramsout,sizeof(ciscoX3defaults));
    if (tty->capabilities & SFLOWOUT)
	pcb->paramsout[X3_FLOWOUT] = 1;
    if (tty->capabilities & SFLOWIN)
	pcb->paramsout[X3_FLOWIN] = 1;
    if (tty->tty_dispatchtimeout != 0) {
	register int time = (tty->tty_dispatchtimeout + (1000/20)) / (1000/20);
	if (time > 255)
	    time = 0;
	pcb->paramsout[X3_IDLETIME] = time;
	/* representative alphanumeric */
	if (tty->tty_editinfo.dispatch['A'] & TTY_DISP_DISP)  
	    pcb->paramsout[X3_DISPMASK] |= 1;
	if (tty->tty_editinfo.dispatch[RETURN] & TTY_DISP_DISP)
	    pcb->paramsout[X3_DISPMASK] |= 2;
	if (tty->tty_editinfo.dispatch[CH_ESC] & TTY_DISP_DISP)
	    pcb->paramsout[X3_DISPMASK] |= 4;
	if (tty->tty_editinfo.dispatch[CH_DEL] & TTY_DISP_DISP)
	    pcb->paramsout[X3_DISPMASK] |= 8;
	if (tty->tty_editinfo.dispatch[CH_ETX] & TTY_DISP_DISP)
	    pcb->paramsout[X3_DISPMASK] |= 16;
	if (tty->tty_editinfo.dispatch[CH_HT] & TTY_DISP_DISP)
	    pcb->paramsout[X3_DISPMASK] |= 32;
	/* representitive All other ctrls */
	if (tty->tty_editinfo.dispatch[CH_SOH] & TTY_DISP_DISP)	
	    pcb->paramsout[X3_DISPMASK] |= 64;
    }
    if (tty->current.padding_ptr) {
	/*
	 * if there isn't any padding defined, skip this and leave the values
	 * at zero.  Otherwise, fill in the correct numbers.
	 */
	pcb->paramsout[X3_CRPAD] = tty->current.padding_ptr[RETURN];
	pcb->paramsout[X3_LFPAD] = tty->current.padding_ptr[LINEFEED];
    }
    /* save parameters in case we need to reset to initial values */
    bcopy(pcb->paramsout,pcb->paramsorig,sizeof(pcb->paramsout));
}

/*
 * do_x3list
 * Parse and act on a list of X.3 parameter number/value pairs.
 * Return a list suitable for use in a response.
 */
void do_x3list (conntype *conn, uchar *inlist, int len, int flags)
{
    int param, value;
    boolean success;
    pcbtype *pcb = (pcbtype *) conn->tcb;

    if (conn->debug)
	printf("\n[%s, len %d:",messtype[flags & 7],len);
    for (; len >0; len-=2) {
	param = *inlist++;
	value = *inlist++;
	if (conn->debug)
	    printf(" %d:%d",param,value);

	if (param == X3_PRIVATE) {	/* Check for setting "private" X.3 */
	    if (conn->debug)		/*  parameters, and quit if so */
		printf("(code set %d, aborting)", value);
	    len = 0;
	    success = FALSE;
	    do_x3response(pcb,param,value,success,flags);
	    break;	
	}
	success = TRUE;			/* assume OK if no check */
	if (value == pcb->paramsout[param]) {
	    if (conn->debug) printf("=");
	} else if (conn->proto.pad.statechange_check) {
	    success = (*(conn->proto.pad.statechange_check)) 
		(conn, param, value);
	}

	if (success == TRUE)
	    pcb->paramsout[param] = value;
	do_x3response(pcb,param,value,success,flags);
    }
    if (conn->debug)
	printf("]");
}

/*
 * do_x3response
 * Insert a single value pair inn the the response list
 */
void do_x3response (
    pcbtype *pcb,
    int param,
    int value,          /* whether we changed the parameter value */
    boolean changed,
    int flags)		/* type of outlist */
{
    switch (flags) {
      case X3_READ:
      case X3_SETANDREAD:
	pad_option_emit(pcb, X3_INDICATE, param, value);
	break;
      case X3_SET:
	if (!changed)
	    pad_option_emit(pcb, X3_INDICATE, param|X3_ERRORBIT, 0);
	break;
      case X3_SETQUIETLY:
      default:
	break;
    }
}

/*
 * pad_option_emit
 * emit a given X.3 option into the data stream.
 */
void pad_option_emit (register pcbtype *pcb, int type, int param, int value)
{
    if ((pcb->flags & PCB_CTLREADY) == 0) {
	pad_putpush(pcb, FALSE);
	(void)pad_putbyte(pcb,type,TRUE);
	pcb->flags |= PCB_CTLREADY;
    }
    (void) pad_putbyte(pcb,param,TRUE);
    (void) pad_putbyte(pcb,value,TRUE);
}

/*
 * pad_enqueue
 * Enqueue an arriving X25 packet for the appropriate PAD control block.
 * This is logically equivilent to raw_enqueue, from the interrupt level's
 * point of view.
 */
void pad_enqueue (
    hwidbtype *idb,
    paktype *pak,
    lcitype *lci,
    boolean qbit)
{
    pcbtype *pcb;

    pcb = lci->lci_pcb;
    if (!pcb) {
	datagram_done(pak);
	return;
    }
    GET_TIMESTAMP(idb->lastinput);	/* remember time of last input */
    pak->inputtime = idb->lastinput;	/* save it in packet too */
    pak->length = pak->datagramsize;
    pak->dataptr = pak->network_start;
    pcb->qinput++;
    pcb->pakincount++;
    pcb->unreadbytes += pak->length;
    if (qbit) {
	pcb->pakincontrol++;
	pak->flags |= PAK_CONTROL;	/* control packet */
	if (pad_debug)
	    buginf("\nPAD%t: Control packet received.",pcb->ttynum);
    }
    if (QUEUEFULL(&pcb->inqueue)) {
	if (pad_debug)
	    buginf("\nPAD%t: input packet discarded.",pcb->ttynum);
	datagram_done(pak);
	return;
    }
#ifdef DEBUG1
    if (pad_debug)
	buginf("\nPAD%t: packet %lx received, length %d, datastart %lx",
	       pcb->ttynum,pak,pak->length,pak->dataptr);
#endif
    p_enqueue(&pcb->inqueue, pak);
    clear_if_input(pak);
}


/*
 * This table is from RFC1053.  Somebody should check it against the X3 spec.
 */

#define NUMX3SPEEDS 19
int x3speeds[NUMX3SPEEDS] = { 110, 134, 300, 1200, 600, 75, 150, 1800,
200, 100, 50, 1200, 2400, 4800, 9600, 19200, 48000, 56000, 64000 };

/*
 * Index 11 is 75 in, 1200 out.  It's cheaper to handle it as a special
 * case in the code than to have separate tables for input and
 * output speed.
 */

/*
 * pad_control
 * Process a pad control packet (one with the Q-bit set).
 */
void pad_control (paktype *pak, pcbtype *pcb, conntype *conn)
{
    int code = *pak->dataptr;
    register int i;

    if (pad_debug) {
	buginf("\nPAD%t: Input ",pcb->ttynum);
	print_x29pak(pak,pak->length,buginf);
    }

    (void) pad_putpush(pcb,FALSE);    /* Send accumulated output data */

    switch (code) {

      case X3_INDICATE:
	{      
	    register uchar *p = pak->network_start + 1;
	    register int l = pak->length - 1;
	    register int param;
	    if (pad_debug)
		buginf("\nPAD%t: Setting ParamsIn, length %d",
		       pcb->ttynum,l);
	    for (;l > 0; l -= 2) {
		param = *p++;
		if (param == 0) {  /* some vendor specific params ? */
		    if (pad_debug)
			buginf(", Ignoring vendor %d specific params",*p);
		    break;
		}
		if (param < NX3PARAMETERS) {
		    pcb->paramsin[param] = *p;
		}
		p++;
	    }
	}
	if (pcb->flags & PCB_GETPARAM) {
	    int speed = pcb->paramsin[X3_SPEED];
	    tt_soc *tty = (tt_soc *)pcb->tty;
	    int newtxspeed, newrxspeed;
	    /*
	     * We want to set speeds in two different cases:
	     * On a vty, simply to record the speed, so it gets passed
	     * through to the host by rlogin or telnet.
	     * On other ports it actually changes the hardware speed.
	     * Do this only if passspeed is set (a bit unlikely, but
	     * might as well get it right.
	     * Setspeed is called for the passspeed case.
	     * On a vty, it's a noop.
	     */
	    if (((tty->type == VTY_LINE) || tty->tty_passspeed) &&
		(speed >= 0) && speed < NUMX3SPEEDS) {
		newtxspeed = x3speeds[speed];
		if (speed == 11)  /* 11 is 75 in, 1200 out */
		    newrxspeed = 75;
		else
		    newrxspeed = x3speeds[speed];
		setspeed(tty, newtxspeed, newrxspeed);
	    }
	    pcb->flags &= ~PCB_GETPARAM;
	    pad_defaults2(pcb);	/* Maybe change parameters */
	}
	if ((pcb->flags & PCB_SETPARAM) == 0)
	    break;
	else {
	    if (pad_debug)
		buginf("\nPAD%t: Treating Parameter Indication as SET",
		       pcb->ttynum);
	    code = X3_SET;
	    /* Fall through to SET code */
	}		
	
      case X3_SET:
	pcb->flags &= ~PCB_SETPARAM;
	if (pak->length !=1 )
	    do_x3list(conn, pak->dataptr+1, (int)pak->length-1, code);
	else  /* quietly set our parameters back to initial values */
	    bcopy(pcb->paramsorig,pcb->paramsout,sizeof(pcb->paramsout));
	break;
      case X3_SETANDREAD:
	pcb->flags &= ~PCB_SETPARAM;
	if (pak->length !=1 )
	    do_x3list(conn, pak->dataptr+1, (int)pak->length-1, code);
	else  /* set all parameters back to initial values */
	    for (i=1; i < NX3PARAMETERS; i++){
		pcb->paramsout[i] = pcb->paramsorig[i];
		pad_option_emit(pcb,X3_INDICATE,i,pcb->paramsout[i]);
	    }
	break;
	
      case X3_CLEAR:
	if (conn->debug)
	    printf("[Invitation to CLEAR]");
	pcb->flags |= PCB_CLEAR;
	break;

      case X3_BREAKRECEIVED:
	if (conn->debug)
	    printf("[Break Indication]");
	pad_option_emit(pcb, X3_SET, X3_DISCARD,0);
	pad_putcontrol(pcb);
	/*
	 *  Process the break.  If a VTY connection, process as if input.
	 *  Otherwise, output it to the terminal.
	 */	       
	if (conn == stdio->conntop)
	    stdio->isavedchar = special_chars(stdio, BREAK_CHAR);
	else
	    putc(stdio,BREAK_CHAR);
	break;
	
      case X3_READ:
	{
	    register uchar *p = pak->network_start + 1;
	    register int l = pak->length - 1;
	    register int param;
	    
	    pcb->flags &= ~PCB_SETPARAM;	/* don't IND as SET */
	    if (l > 0) {
		while (l > 0) {
		    param = *p;
		    pad_option_emit(pcb,X3_INDICATE,param,
				    pcb->paramsout[param]);
		    p += 2;
		    l -= 2;
		}
		break;
	    }
	}
	for (i=1; i < NX3PARAMETERS; i++)
	    pad_option_emit(pcb,X3_INDICATE,i,pcb->paramsout[i]);
	break;
	
      case X3_ERROR:
	break;

    }


    /* set x.3 options according to data in packet */

    /* send response packet, if necessary */

    pad_putcontrol(pcb);

    /* free the packet */
    datagram_done(pak);
}

/*
 * pad_accesscheck
 * check whether a given x.121 source may use a particular translate
 * command, using the regular expression access list code.
 */

boolean pad_accesscheck (translatetype *trans, char *source,
			 int len,  regexpacctype *list[])
{
    char savechar;
    int result;

    savechar = source[len];
    source[len] = '\0';
    result = regexp_accesscheck((uchar *)source, trans->accessclass, list);
    source[len] = savechar;
    if (!result) {
	trans->totaluses++;
	trans->failures++;
    }
    return(result);
}

/*
 * pad_getstring
 * Get a string of data from the X.25 connection.  Process the incoming
 * packet queue, including any control packets that are there.
 * Return a status code.
 */
int pad_getstring (
    register pcbtype *pcb,
    uchar **datastart,
    long *datacount,
    register long datamax)
{
    paktype *packet;
    lcitype *lci;
    leveltype level;
 
    /* 
     * Get current input buffer.  Discard it if it has been emptied
     * (this is the normal case)
     */
    packet = pcb->curinputpak;
    if (packet && (packet->length <= 0)) {
#ifdef DEBUG1
	if (pad_debug)
	    buginf("\nPAD%t: Done with current input packet",pcb->ttynum);
#endif
	datagram_done(packet);
	pcb->curinputpak = NULL;
	packet = NULL;
    }

    /*
     * If no partial buffer, attempt to get something off the input queue.
     */
    while (packet == NULL) {
	packet = p_dequeue(&pcb->inqueue);
	if (packet == NULL) {
	    if (pcb->flags & PCB_CLEAR) {
		pcb->state = PAD_CLOSING;
		return(END_OF_FILE);
	    }
	    return ((pcb->state == PAD_ESTABLISHED) ? NO_DATA : END_OF_FILE);
	}
#ifdef DEBUG1
	if (pad_debug) {
	    char buffer[50];
	    char *bufptr = buffer;
	    int i;
	    for (i=0; i< (min(16, packet->length)); i++)
		bufptr += sprintf(bufptr, " %02x", 
				  ((uchar *)packet->dataptr)[i]);
	    buginf("\nPAD%t: new input packet %lx length %d %s",pcb->ttynum, 
		   packet, packet->length, buffer);
	}
#endif
	pcb->qinput--;
	/*
	 * An x25 clear request might come in at interrupt level and clear 
	 * the lci, so make an attempt to check for a valid lci before we
	 * use it.  The call to pad_control is a bit of a concern.  Since it
	 * can suspend, we cannot have a NETS_DISABLE around it, so hopefully
	 * it correctly checks the lci should it need to use it.
	 */
	if (pcb->pcb_lci)
	    pcb->pcb_lci->lci_ack =
		++pcb->pcb_lci->lci_ack % 
		    pcb->pcb_lci->lci_idb->hwptr->x25_modulo;
	if (packet->flags & PAK_CONTROL) {	/* Process control packets */
	    if (pcb->unreadbytes)
		pcb->unreadbytes -= packet->length;
	    pcb->bytesincount += packet->length;
	    pad_control(packet,pcb,pcb->pcb_conn);
	    packet = NULL;
	} else
	    pcb->curinputpak = packet;
	/*
	 * An x25 clear request might come in at interrupt level and clear 
	 * the lci, so make an attempt to check for a valid lci before we
	 * use it.  x25_sendflow uses the lci in one of the routines it calls.
	 * This won't suspend, so we can disable around it.  
	 */
	level = raise_interrupt_level(NETS_DISABLE);
	if ((lci = pcb->pcb_lci) != NULL) {
	    /* maybe update our window */
	    if (lci)
	        x25_sendflow(lci->lci_idb->hwptr, lci, X25_RR);
        }
	reset_interrupt_level(level);
    }

    /*
     * return all or part of the data to the caller.
     * return NO_DATA if we didn't get any data even though
     * we did get a data packet. This prevents our callers
     * from calling putpkt with an empty packet, which some
     * of the putpkt vectors can't handle.
     */

    if (packet->length == 0)
	return(NO_DATA);

    if (datamax > packet->length)
	datamax = packet->length;
    *datastart = (uchar *) packet->dataptr;
    *datacount = datamax;
    packet->length -= datamax;
    packet->dataptr += datamax;
    if (pcb->unreadbytes)
	pcb->unreadbytes -= datamax;
    pcb->bytesincount += datamax;
    if (pcb->flags & PCB_CLEAR)
	pcb->state = PAD_CLOSING;
    return(pcb->flags & PCB_CLEAR ? END_OF_FILE : OK);
}

/*
 * pad_putback - Mark some data read as unread
 *
 * This routine is used to mark data read as unread. This can be done
 * after a call to pad_getstring() if not all of the data was read. Note
 * that the count must never exceed the amount of data returned in the
 * very last pad read.
 *
 */
void pad_putback (pcbtype *pcb, int count)
{
    if (pcb->curinputpak) {	/* Session still active, data there? */
	pcb->curinputpak->length += count; /* Update length */
	pcb->curinputpak->dataptr -= count; /* And count */
	pcb->unreadbytes += count;
	pcb->bytesincount -= count;
    }
}

/*
 * pad_getbyte
 * Return next byte from a PAD data stream, or -1 if none available.
 * Jacket for more general pad_getstring() routine.
 */
int pad_getbyte (pcbtype *pcb, register int *status)
{
    uchar *ptr;
    long count;
 
    *status = pad_getstring(pcb, &ptr, &count, 1);
    if (*status == OK)
	return((int) *ptr);
    else
	return(-1);
}	

/*
 * pad_putbyte
 * Write a byte to a net connection.  Returns TRUE on success, FALSE if
 * there is no buffer room and the user did not specify blocking, or if
 * the connection is dead.  Note that we send a full packet when the NEXT
 * byte is sent, so that we can accurately set the M bit...
 */
boolean pad_putbyte (
    register pcbtype *pcb, 
    register uchar ch,
    register int blockflag)
{
    register paktype *pak = pcb->curoutputpak;

    if (pak && pak->length == 0) {
#ifdef DEBUG0
	if (pad_debug) {
	    buginf("\n pad_putbyte: Pak is full, set the MBIT");
	}
#endif
	pak->flags |= PAK_MBIT;	/* packet full, and more to send... */
	(void) pad_putpush(pcb,FALSE);
	pak = pcb->curoutputpak;
    }
    while (pak == NULL) {	
	if (pad_putsetup(pcb) == TRUE)
	    pak = pcb->curoutputpak;
	else {
	    if (!blockflag || (pcb->state != PAD_ESTABLISHED) || psipending())
		return(FALSE);
	    else {
		TIMER_START(pcb->dallytime, 2*ONESEC);
		edisms((blockproc *)padputBLOCK, (ulong)pcb->pcb_lci);
		pak = pcb->curoutputpak;
	    }
	}
    }
    *pak->dataptr++ = ch;
    pak->length--;
    return(TRUE);
}

/*
 * pad_putclear
 * This is mostly like pad_putbyte with an arg of X3_CLEAR, but it doesn't
 * call psipending, which will cause the function to fail if an incoming
 *  connection has closed (due to the modemcheck()).  This function is used
 *  ONLY by pad_close(), and is very specific to that function
 */
boolean pad_putclear (pcbtype *pcb)
{
    register paktype *pak = pcb->curoutputpak;

    while (pak == NULL) {	
	if (pad_putsetup(pcb) == TRUE)
	    pak = pcb->curoutputpak;
	else {
	    if ((pcb->state != PAD_ESTABLISHED))
		return(FALSE);
	    else {
		TIMER_START(pcb->dallytime, 2*ONESEC);
		edisms((blockproc *)padputBLOCK, (ulong)pcb->pcb_lci);
		if (pcb->pcb_lci) {
		    /* After we wakeup from padputBLOCK(), make sure that
		     * we are still in data transfer or in flow control ready
		     * state.  If not, return.  The serial line must have 
		     * restarted and therfore we should not conitnue the write.
		     */
		    if ((pcb->pcb_lci->lci_state != X25_P4) && 
			(pcb->pcb_lci->lci_state != X25_D1))
			return(FALSE);
		}
		pak = pcb->curoutputpak;
	    }
	}
    }
    *pak->dataptr++ = X3_CLEAR;
    pak->length--;
    return(TRUE);
}

/*
 * padputBLOCK
 * Event dismiss for PAD put byte operation.  We blocked because X25
 * told us to (eg window closed, etc), or because we couldn't get a buffer.
 * Since the window is in Packets, we can either send the current packet,
 * or we can't.  There is no silly window stuff to do.  Wake up when the
 * X25 hold queue for the LCI is empty (which guarantees that X25 will
 * accept the packet).
 */
boolean padputBLOCK (register lcitype *lci)
{
    if (lci->lci_state != X25_P4 && lci->lci_state != X25_D1)
	return(FALSE);			/* activate if connection is dead */
    if (QUEUEEMPTY(&lci->lci_holdQ))	/* activate if window open */
	return(FALSE);
    if (!SLEEPING(lci->lci_pcb->dallytime))
	return(FALSE);			/* activate if timer has expired */
    return(TRUE);			/* keep on blocking */
}

/*
 * pad_putsetup
 * get a PAD connection ready to send data.
 */
boolean pad_putsetup (pcbtype *pcb)
{
    register paktype *pak;
    register idbtype *idb;
    register int size;

    if (pcb->state != PAD_ESTABLISHED)
	return(FALSE);

    /*
     * If we have a packet and the packet is not full (length > 0) then
     * send an error message because this should not happen.
     * But if we have a full packet, we just need to return TRUE 
     * so pad_putbyte() can set the MBIT.
     */
    if (pcb->curoutputpak) {
	if (pcb->curoutputpak->length)
	    errmsg(&msgsym(PUTSETUP, PAD), pcb->ttynum);
	return(TRUE);
    }
    if (pcb->pcb_lci == NULL)
	return(FALSE);
    if (!QUEUEEMPTY(&pcb->pcb_lci->lci_holdQ))	/* activate if window open */
	return(FALSE);

    size =  1<<pcb->pcb_lci->lci_pout;
    pak = getbuffer(size);
    if (pak == NULL)
	return(FALSE);
    pcb->curoutputpak = pak;
    pak->length = pak->unspecified[0] = size;
    pak->dataptr = pak->network_start;
    idb = pcb->pcb_lci->lci_idb;
    pak->enctype = idb->hwptr->enctype;
    pak->if_output = idb;
#ifdef DEBUG0
    if (pad_debug)
	buginf("\npad%t: putsetsetup %d bytes", pcb->ttynum,pak->length);
#endif
    return(TRUE);
}

/*
 * pad_putpush
 * Finish up current output buffer and initiate packet transmission.
 */
void pad_putpush (
    register pcbtype *pcb,
    register boolean qbit)
{
    register paktype *pak;
    register int bytes;

    pak = pcb->curoutputpak;
    if (pak == NULL)
	return;
    bytes = pak->unspecified[0] - pak->length;
    if (bytes == 0)
        return;
#ifdef DEBUG0
    if (pad_debug)
	buginf("\npad%t: putpush %d bytes Q=%d", pcb->ttynum,bytes,qbit);
#endif
    /*
     * We have a buffer with data to send.  Remove from PCB and initialize
     * the X.25 related fields. 
     */
    pcb->curoutputpak = NULL;
    if (x25_pad_encaps(pak,pcb->pcb_lci,qbit,bytes)) {
	x25_output(pcb->pcb_lci->lci_idb->hwptr, pak, TAIL);
	pcb->qoutput++;
	pcb->pakoutcount++;
	pcb->bytesoutcount+=bytes;
    } else {
	datagram_done(pak);
	pad_encapsfailed++;
    }
}

void pad_putcontrol (pcbtype *pcb)
{
    if (pcb->flags & PCB_CTLREADY) {  /* Is there a packet to send ? */
	if (pad_debug && (pcb->curoutputpak)) {
	    paktype *p = pcb->curoutputpak;
	    buginf("\nPAD%t: Output ",pcb->ttynum);
	    print_x29pak(p, p->unspecified[0] - p->length,buginf);
	}
	pad_putpush(pcb,TRUE);
	pcb->pakoutcontrol++;
	pcb->flags &= ~PCB_CTLREADY;
    }
}

/*
 * pad_completecall
 * called when we get back a call accepted from someone we have sent a call
 * to.  Update the pcb, so that openblock will return.
 */
void pad_completecall (lcitype *lci)
{
    pcbtype *pcb = lci->lci_pcb;
    if (pcb) {
	pcb->state = PAD_ESTABLISHED;
	if (pad_debug) {
	    buginf("\nPAD%t: Outgoing call completed",pcb->ttynum);
	}
    } else {
	if (pad_debug) {
	    buginf("\nPAD: Unable to complete outgoing call");
	}
    }
}

/*
 * pad_clearcall
 * when X25 recieves a clear request on a PAD lcn, call this routine to
 * pass the failure data back to the user.
 */
void pad_clearcall (lcitype *lci, int reason, int diag)
{
    pcbtype *pcb;

    pcb = lci->lci_pcb;
    if (pcb) {
	pcb->state = PAD_CLEARED;
	pcb->pcb_lci = NULL;
    }
    if (pad_debug)
	buginf("\nPAD%t: call cleared. Reason %d, Diag %d.",
		pcb ? pcb->ttynum : 0, reason, diag);
}

/*
 * pad_lci_delete
 * called here from x25_lci_delete, since our lci is being deleted.
 * NULL our lci so we don't try to use it anymore
 */
void pad_lci_delete (lcitype *lci)
{
    pcbtype *pcb = lci->lci_pcb;

    if (pcb) 
	pcb->pcb_lci = NULL;
}

/*
 * pad_restart
 * call here from X25_state when the DCE sends a restart request
 */
void pad_restart (lcitype *lci)
{
    pcbtype *pcb = lci->lci_pcb;

    if (pcb) {
	pcb->state = PAD_RESTART;
	pcb->pcb_lci = NULL;
    }
	
    if (pad_debug)
	buginf("\nPAD%t: X25 restarted.", pcb ? pcb->ttynum : 0);
}

/* 
 * pad_reset
 * Called when X.25 issues a reset request.
 * Resume PAD connection after the reset confirmation.
 */
void pad_reset (lcitype *lci)
{
    pcbtype *pcb = lci->lci_pcb;

    if (pcb) {
	if (pcb->state != PAD_RESTART) {
	    pcb->flags &= ~PCB_DISCARD;
	}
    }
    if (pad_debug)
	buginf("\nPAD%t: X25 VC reset.", pcb ? pcb->ttynum : 0);
}

/*
 * pad_openBLOCK
 * Wait for a pad connection to open.  We have sent the call request
 * packet, and are waiting for a response from the remote host -
 * either a call accepted, or a call clear.
 */
boolean pad_openBLOCK (pcbtype *pcb)
{
    if (pcb->state != PAD_CALLING)	/* Still waiting for response ? */
	return(FALSE);		/* connection open, refused, or timed out */
    if (!pcb->pcb_lci)		/* our lci is gone, probably deleted  */
	return(FALSE);		
    if (psiset((tt_soc *)pcb->tty))
	return(FALSE);		/* user aborted the open */
    return(TRUE);
}

/*
 * padBLOCK
 * Event dismiss for the pad routine.  Check for I/O after checking
 * all the special conditions.  This prevents large amounts of output
 * from locking out recognition of the interrupt sequence, carrier drop, etc.
 */
boolean padBLOCK (register conntype *conn)
{
    register tt_soc *tty = conn->tty;

    conn->status |= CONN_CHECKALL;
    if (TIMER_RUNNING_AND_AWAKE(conn->dispatchtimer))
	return(FALSE);
    if (psiset (tty) || modemcheck (tty))
	return (FALSE);
    if (!SLEEPING(conn->notetime))
	return (FALSE);
    if (tty->tty_sessiontimeout && conn_session(conn, TRUE) == FALSE)
	return(FALSE);
    if (TIMER_RUNNING_AND_AWAKE(conn->logintimer))
	return(FALSE);
    conn->status &= ~CONN_CHECKALL;
    if (inputpending(tty))
	return (FALSE);
    if (pad_activity(conn) && !outputblock(tty))
	return (FALSE);
    return (TRUE);
}

/*
 * pad_activity
 * check for activity on a PAD connection
 */
boolean pad_activity (conntype *conn)
{
    if (pad_unread(conn) != 0)
	return(TRUE);

    if (((pcbtype *)(conn->tcb))->state != PAD_ESTABLISHED)
	return(TRUE);

    return(FALSE);
}

/*
 * x25_pad_encapsulate
 * Fill in encapsulation for a packet containing pad data.
 */
boolean x25_pad_encaps (
    paktype *pak, 
    lcitype *lci,
    boolean qbit,		/* should we set the q bit ? */
    int length)			/* length of data */
{
#ifdef DEBUG0
    if (pad_debug)
	buginf("\nPAD: encaps(pak,%d,%d,%d)",lci->lci_lci,qbit,length);
#endif
    pak->linktype = LINK_PAD;
    pak->datagramstart = pak->network_start;
    pak->datagramsize = length;
    pak->x25_flags = qbit ? PAK_X25_QBIT : 0;
    if (!x25_encaps(pak, (long)lci, NULL)) {
	if (pad_debug)
	    buginf("\nPAD: encaps failed on LCN%d",lci->lci_lci); 
	return(FALSE);
    }
    return(TRUE);
}

/*
 * pad_interrupt
 * Transmit an X.25 interrupt packet.
 */
void pad_interrupt (pcbtype *pcb)
{
    uchar foo = 0;

    if (pcb && pcb->pcb_lci && pcb->pcb_lci->lci_allow_int)
	x25_sendint(pcb->pcb_lci, NULL, &foo, 1);
}

/*
 * Print_x29pak
 * print out an X.29 packet
 */
void print_x29pak (
    paktype *pak,
    int len,
    void (*proc) (const char *, ...))
{
    uchar *c;
    int code;

    c = pak->network_start;
    code = *c++;

    proc("X29 packet type %d (%s) len %d",code,messtype[code&0x7],len--);
    switch (code) {
      case X3_INDICATE:
      case X3_SET:
      case X3_BREAKRECEIVED:
      case X3_READ:
      case X3_SETANDREAD:
	print_paramlist(c,len,proc);
	break;
      case X3_CLEAR:
	proc("Invite Clear");
	break;
      case X3_ERROR:
	proc("(reason %d on code %d)", c[0], c[1]);
	break;
      default:
	break;
	
    }
}

void print_paramlist (
    uchar *p,
    int l,
    void (*proc) (const char *, ...))
{
    char buffer[80];
    int i,len = 0;

    buffer[0] = '\0';
    
    if (l > 8)		/* might fit on same line */
	len = sprintf(buffer,"\n   ");

    for (i= 1; i <= l>>1; i+=1) {
	if ((i & 7) == 0) {
	    buffer[len] = '\0';
	    proc("%s",buffer);
	    len = sprintf(buffer,"\n   ");
	}
	len += sprintf(buffer+len," %d:%d,", p[0], p[1]);
	p += 2;
    }
    buffer[len] = '\0';
    if (len > 5 || l < 4)	/* is there more than the newline ? */
	proc("%s",buffer);
}

/***********************************************************************
 ****  Now we deal with incoming connections.
 ***********************************************************************/

/*
 * pad_acceptcall
 * decide whether we want to accept a call.  The call user data indicates
 * that it is a pad call.  Return TRUE if we accept the call (having set
 * up a pcb, a connection, and a VTY), or FALSE if we refuse. 
 */
int pad_acceptcall (
    lcitype *lci,
    uchar *cud,            /* pointer to call user data */
    int cudlen)	           /* Length of call user data */
{
    int dummy;
    int line = -1;
    x25routeentrytype *pt = NULL;
    translatetype *trans = NULL;
    boolean parsed;

    dummy = get_interrupt_level();
    if (dummy) {
	errmsg(&msgsym(INTR, PAD), "pad_acceptcall", dummy);
    }

    /*
     * check for a call routed to an x.25 protocol translator address
     */
    if (lci->lci_rtp) {
	pt = lci->lci_rtp;
	trans = (translatetype *) pt->data;

	if (lci->lci_reverse && 
	    ((trans->in.x25.x25flags & TRANSF_X25_ACCEPTREVERSE) == 0)) {
	        if (pad_debug)
		    buginf("\nPADTCP: Reverse charge not accepted");
                return(PAD_FAILED);
	}
	if (pt->type != X25_ROUTE_IPPT) {
	    if (pad_debug)
		buginf("\nPADTCP: unknown x25 route type %d", pt->type);
	    return(PAD_FAILED);
	}
	/* get a vty */
	line = get_vtyline(TELNET_PORT, &pad_connvector);
	if (line <= 0)
	    return(PAD_FAILED);
	if (pad_debug || translate_debug)
	    buginf("\nPAD: translate call to %*s CUD %x %x %x %x \"%*s\"",
		   lci->lci_dst_addr.length, lci->lci_dst_addr.x121_addr,
		   cud[0], cud[1], cud[2],
		   cud[3], cudlen-4, &cud[4]);
	if (!pad_accesscheck(trans,
			     lci->lci_src_addr.x121_addr,
			     lci->lci_src_addr.length,
			     x29_accesslists)) {
	    if (pad_debug || translate_debug)
		buginf(" Access denied");
	    return(PAD_FAILED);
	}
    } else {    
          if (lci->lci_reverse && 
            ((lci->lci_idb->hwptr->x25_flags & X25_ACCEPTREVERSE) == 0))
	    	return(PAD_FAILED);
	/*
	 * If incoming address is larger then our input interface
	 * address then we have a subaddress we should parse.
	 * Make sure that address matches before trying to parse.
	 */
	if ((lci->lci_dst_addr.length >
	     lci->lci_idb->hwptr->x25_address->length) &&
	    ((lci->lci_dst_addr.length -
	      lci->lci_idb->hwptr->x25_address->length) <= 2)) {
	    line = parse_unsigned(
		    &lci->lci_dst_addr.x121_addr[
			lci->lci_idb->hwptr->x25_address->length],
		    &parsed);
	}
	if (pad_debug)
	    buginf("\nPAD: incoming call to %*s(%d) CUD length %d",
		    lci->lci_dst_addr.length, lci->lci_dst_addr.x121_addr,
		   line == -1 ? 0 : line,
		   cudlen - 4);
	/*
	 * If a sub-address was specified, use it as a rotary group number.
	 * Otherwise, get a VTY, if possible.
	 */
	if ((line > 0) && (line < PORTINTERVAL))
	    line = select_rotary(line, &pad_connvector, NULL, 
                                 ROTARY_ASCENDING);
	else
	    line = get_vtyline(TELNET_PORT, &pad_connvector);
	if (line <= 0)
	    return(PAD_FAILED);

	if (!regexp_accesscheck(lci->lci_src_addr.x121_addr,
				MODEMS[line]->accessin, x29_accesslists)) {
	    if (pad_debug || translate_debug)
		buginf(" Access denied");
	    return(PAD_FAILED);
	}
    }
    if (vty_logging) {
	buginf("\nPAD%t: call from %*s on LCI %d PID %x %x %x %x CUD \"%*s\"",
	       line, lci->lci_src_addr.length, lci->lci_src_addr.x121_addr,
	       lci->lci_lci, cud[0], cud[1], cud[2], cud[3], 
	       cudlen-4, &cud[4]);
    }
    return(pad_makecall(lci, line, pt ? pt->data : NULL));
}

/*
 * x25translate_cfork 
 * Create an approriate type of process for performing protocol translation
 * on an incoming X.25 connection.  Create special processes if they exist,
 * otherwise create a normal exec with the appropriate "autocommand".
 */

static int x25translate_cfork (tt_soc *tty, translatetype *trans)
{
    trans->totaluses++;
    if (trans->maxallowed && (trans->active >= trans->maxallowed)) {
	trans->failures++;
	return(-1);
    }
    return(reg_invoke_pt_x25input(OUTPROT(trans->type), trans, tty));
}

/*
 * pad_makecall
 * Worker routine that makes call
 * Handles PVCs and SVCs
 */
int pad_makecall (
    lcitype *lci,
    int line,
    translatetype *trans)
{
    tt_soc *tty;
    conntype *conn;
    pcbtype *pcb;
    ulong stacksize = 0;

    tty = (tt_soc *) tty_allocate(line,TRUE);
    if (tty == NULL) {
	if (pad_debug)
	    buginf("\nPAD: no free VTYs");
	return(PAD_FAILED);
    }
    conn = getconnection(tty, &pad_connvector);
    if (conn == NULL) {
	if (pad_debug)
	    buginf("\nPAD: can't get Connection");
	tty_deallocate(tty);
	return(PAD_FAILED);
    }
    pcb = getpcb(tty);
    if (pcb == NULL) {
	if (pad_debug)
	    buginf("\nPAD: can't get PCB");
	tty_deallocate(tty);
	freeconnection(conn);
	return(PAD_FAILED);
    }

    conn->tcb = pcb;			/* point to PCB */
    bcopy(lci->lci_src_addr.x121_addr, conn->hostname,
	  lci->lci_src_addr.length);
    pcb->state = PAD_ESTABLISHED;
    pcb->pcb_lci = lci;
    pcb->pcb_conn = conn;
    lci->lci_pcb = pcb;
    unqueue(&tty->connhead, conn);	/* remove from general q */
    tty->conntop = conn;		/* remember the driver conn */
    tty->statbits &= ~(CARDROP+HANGUP+IDLE);	/* reincarnated again! */
    tty->statbits |= READY;
    tty->conncount++;				/* another use */
    tty->ivector = & PADvector;			/* this is a PAD vty */
    switch (tty->type) {
      case AUX_LINE:
      case TTY_LINE:
	if (pad_debug)
	    buginf("\nPAD: Creating TTY daemon on tty%t for lci%d",
		   tty->ttynum, lci->lci_lci);
	if (reg_invoke_kerberos_is_running()) {
	    stacksize = (ulong) KERBEROS_STACK_SIZE;
	}
	tty->tiptop = cfork((forkproc *)tty_daemon, (long)tty,
			    stacksize, "TTY Daemon", tty->ttynum);
	break;
      case VTY_LINE:
	tty->ovector = & PADvector;
	if (trans) {
	    if (pad_debug)
		buginf("\nPAD: Creating protocol translator on tty%t for lci%d",
		       tty->ttynum, lci->lci_lci);
	    tty->tiptop = x25translate_cfork(tty, trans);
	} else {
	    if (pad_debug)
		buginf("\nPAD: Creating exec on tty%t for lci%d",
		       tty->ttynum, lci->lci_lci);
	    if (reg_invoke_kerberos_is_running()) {
		stacksize = (ulong) KERBEROS_STACK_SIZE;
	    } else {
		stacksize = (ulong) EXEC_STACK_SIZE;
	    }
	    tty->tiptop = cfork((forkproc *) exec, (long)tty, stacksize, 
				"Virtual Exec",
				tty->ttynum);
	}
	break;
      default:
	tty->tiptop = -1;
	break;
    }
    if (tty->tiptop < 0) {
	freeconnection(conn);
	death(tty);
	return(PAD_FAILED);
    }
    return((trans && (trans->in.x25.x25flags & TRANSF_X25_PRINTER)) ?
	   PAD_IN_PROGRESS : PAD_OK);
}

/*
 * pad_outputblock - Return TRUE if output would block
 */
boolean pad_outputblock (tt_soc *tty)
{
    pcbtype *pcb = tty2pcb(tty);

    /* Don't block, let the caller handle it */
    if (!pcb || pcb->state != PAD_ESTABLISHED)
	return(FALSE);

    /* Don't block if we don't have a full packet to output */
    if (pcb->curoutputpak && pcb->curoutputpak->length)
	return(FALSE);

    return(!pad_putsetup(pcb));
}

/*
 * pad_putc, pad_getc
 * Write and read from a PAD connection
 */
void pad_putc (tt_soc *tty, int ch)
{
    pcbtype *pcb = tty2pcb(tty);

    if (!pcb || pcb->state != PAD_ESTABLISHED)
	return;
    if (pcb->flags & PCB_CTLREADY) {
	/* if there is a control packet waiting to be sent,
	 * we should send it now before we try to collect data
	 */
	pad_putcontrol(pcb);
    }
    if (!(pcb->flags & PCB_INITED))
	pad_defaults(pcb);
    ch &= tty->tty_datamask;
    (void) pad_putbyte(pcb,ch,TRUE);
}

int pad_getc (tt_soc *tty)
{
    pcbtype *pcb = tty2pcb(tty);
    int status;
    int ch;

    if ((!pcb) || (pcb->state != PAD_ESTABLISHED))
	return(-1);
    if (!(pcb->flags & PCB_INITED))
	pad_defaults(pcb);
    if (tty->isavedchar != -1) {
	ch = tty->isavedchar;
	tty->isavedchar = -1;
	/*
	 * note that BREAK is handled here via isavedchar rather than
	 * by the LASTWASBREAK flag.  In fact, we have to explicitly
	 * clear that flag to prevent two breaks from being seen.
	 */
	if (ch == BREAK_CHAR)
	    tty->statbits &= ~LASTWASBREAK;
	return(ch);
    }
    if (pcb->unreadbytes == 0)
	/* Special case used when echoing characters */
	pad_putpush(pcb,FALSE);	/* force output buffer if necessary */
    ch = pad_getbyte(pcb,&status);
    if (ch == -1)
	return(-1);
    ch &= tty->tty_datamask;
    if (tty->type == VTY_LINE) {
	GET_TIMESTAMP(tty->conntop->idletime); /* 0 idle time */
	if (special_chars(tty,ch) < 0) {
	    return(-1);
	} else {
	    if (tty->statbits & LASTWASSPEC) {
		tty->statbits &= ~LASTWASSPEC;
		return(ch | ESCAPED_FLAG);
	    }
	}
    }
    return(ch);
}

/*
 * pad_setsflow
 * Set software flow control
 */
void pad_setsflow (tt_soc *tty)
{
    pcbtype *pcb = tty2pcb(tty);

    if (!pcb || (pcb->state != PAD_ESTABLISHED))
	return;
    /* Return if its not a valid value */
    if (pcb->paramsin[X3_FLOWIN] > 1 && pcb->paramsin[X3_FLOWOUT] > 1)
	return;
    /* Return if nothing had changed avoiding unsolicted X.29 messages */
    if (pcb->paramsin[X3_FLOWIN] == pcb->paramsin[X3_FLOWOUT])
	return;
    pad_putpush(pcb,FALSE);			/* flush data */
    pad_option_emit(pcb,X3_SETANDREAD, X3_FLOWOUT, 
		    tty->capabilities & SFLOWOUT ? 1 : 0);
    pad_option_emit(pcb,X3_SETANDREAD, X3_FLOWIN,
		    tty->capabilities & SFLOWIN ? 1 : 0);
    pad_putcontrol(pcb);
}

/*
 * pad_clearinput
 * Clear input buffers
 */
void pad_clearinput (tt_soc *tty)
{
    pcbtype *pcb = tty2pcb(tty);
    lcitype *lci;

    if (pcb && pcb->state == PAD_ESTABLISHED) {
	lci = pcb->pcb_lci;
	datagram_done(pcb->curinputpak); /* flush current input buffer */
	pcb->curinputpak = NULL;	/* .... */
	while (!QUEUEEMPTY(&pcb->inqueue))
	    datagram_done(dequeue(&pcb->inqueue));
	pcb->qinput = 0;		/* .... */
	pcb->unreadbytes = 0;		/* no unread bytes */
	lci->lci_ack = lci->lci_pr;	/* update offered window */
	/* maybe tell foreign host */
	x25_sendflow(lci->lci_idb->hwptr, lci, X25_RR);
    }
}

/*
 * pad_clearoutput
 * Clear tty output buffers
 */
void pad_clearoutput (tt_soc *tty)
{
    pcbtype *pcb = tty2pcb(tty);

    if (pcb && pcb->state == PAD_ESTABLISHED) {
	datagram_done(pcb->curoutputpak);
	pcb->curoutputpak = NULL;
	while (!QUEUEEMPTY(&pcb->pcb_lci->lci_holdQ))
	    datagram_done(pak_dequeue(&pcb->pcb_lci->lci_holdQ));
	pcb->qoutput = 0;
    }
}

/*
 * pad_inputpending
 * Returns count of bytes pending
 */
int pad_inputpending (tt_soc *tty)
{
    pcbtype *pcb = tty2pcb(tty); 
    int count;

    if (!pcb || pcb->state != PAD_ESTABLISHED)
	return(0);
    count = pad_unread(tty->conntop);

    if (tty->isavedchar != -1)
	count++;
    return(count);
}

/*
 * pad_startoutput
 * Send some PAD data
 */
boolean pad_startoutput (tt_soc *tty)
{
    pcbtype *pcb = tty2pcb(tty);

    if (!pcb || pcb->state != PAD_ESTABLISHED)
	return(FALSE);

    /*
     * Signal helper process.
     */
    if (tty_signal_helper(tty))
	return(TRUE);
 
    /* 
     * unreadbytes is the count of unread data bytes from X.25.
     * Note that a call to startoutput can fail if pending input. 
     */
    /*
     * there follows a hack designed to provide more efficient packetization
     * of echoing of characters typed at an exec prompt (and etc.)  The exec
     * will do "startoutput" of each echoed character, which would normally
     * send x.25 data, which some users get charged for on a per-packet basis.
     * Instead, if there is still input to be read, don't REALLY start the
     * output yet.  This can have certain detrimental effects if the unread
     * data doesn't cause additional startoutputs (as in the case where it
     * is actually x.29 packets instead of data.)
     */
    if (pcb->unreadbytes == 0)
  	pad_putpush(pcb,FALSE);

    return(TRUE);
}

/*
 * pad_outputpending
 * Return count of output bytes pending
 */
int pad_outputpending (tt_soc *tty)
{
    register paktype *pak;
    pcbtype *pcb = tty2pcb(tty);

    if (!pcb || pcb->state != PAD_ESTABLISHED)
	return(0);
    pak = pcb->curoutputpak;
    if (pak == NULL)
	return(0);
    else
	return(pak->unspecified[0] - pak->length);
}

/*
 * tty2pcb
 * Obtain a PCB pointer given a terminal pointer.
 * Returns NULL if connection is dead or dying.
 */

pcbtype *tty2pcb (tt_soc *tty)
{

    if (tty->statbits & (CARDROP|IDLE))
	return(NULL);
    /* 
     * on vtys, set cardrop if connection dies 
     */
    if (tty->type == VTY_LINE) { 
	if (tty->conntop == NULL)
	    tty->statbits |= CARDROP;
    }
    if (tty->conntop)
	return((pcbtype *) tty->conntop->tcb);
    else return (NULL);
}

/*
 * padvty_init
 * Set up the vector table for PAD Virtual Terminals
 */
void padvty_init (void)
{
    register vectortype *vector;

    vector = & PADvector;
    vector_init(vector);		/* initialize vector structure */

    vector->vgetc = pad_getc;		/* character input */
    vector->vclearinput = pad_clearinput;/* clear buffered input */
    vector->vinputpending = pad_inputpending;

    vector->vputc = pad_putc;			/* character output */
    vector->vputpkt = generic_putpkt;
    vector->voutputblock = pad_outputblock;

    vector->vstartoutput = pad_startoutput;	/* start character output */
    vector->voutputpending = pad_outputpending;
    vector->vclearoutput = pad_clearoutput;
    vector->vsetsflow = pad_setsflow;

    vector->vservicehook = helper_servicehook;
    vector->vnoservicehook = helper_noservicehook;

    conn_vector_init(&pad_connvector, "pad", "X.3 PAD");
				 /* Set up the connection vector */

    pad_connvector.snmp_sesstype = D_tslineSesType_pad;

    pad_connvector.valive = pad_alive; /* Set alive routine pointer */
    pad_connvector.vrun = pad_telnet; /* Set run routine pointer */
    pad_connvector.vclose = pad_close; /* Set close routine pointer */
    pad_connvector.vunread = pad_unread; /* Set unread count pointer */
    pad_connvector.vopen = pad_multiproto_open; /* Parse connect command */
    pad_connvector.vresume = pad_parse_resume; /* Parse resume command */
    pad_connvector.vaccounting = pad_conn_acct;

    reg_add_media_x25_macaddr(LINK_PAD, x25_extract_lci, "x25_extract_lci");
}

/*
 * pad_alive
 */
boolean pad_alive (conntype *conn)
{
    pcbtype *pcb;

    if (!conn)
	return(FALSE);
    pcb = (pcbtype *) conn->tcb;
    if (!pcb || pcb->state != PAD_ESTABLISHED)
	return(FALSE);
    return(TRUE);
}

/*
 * pad_defaults
 * Setting up pad parameters is a two step process.  First we send a
 * message to the remote pad to see what it's current parameters are.
 * Then, and perhaps based on the answers we get, we send some modifcations.
 *
 * pad_defaults
 * Ask the remote pad parameters, and remember to change them later.
 */
void pad_defaults (register pcbtype *pcb)
{
    pad_putpush(pcb,FALSE);			/* flush data */
    pcb->flags |= PCB_GETPARAM|PCB_CTLREADY;
    (void) pad_putbyte(pcb,X3_READ,TRUE);	/* read other pads params */
    pad_putcontrol(pcb);
    pcb->flags |= PCB_INITED;
}

/*
 * pad_defaults2
 * Send appropriate modifications to X.3 state, based on terminal config.
 */
void pad_defaults2 (register pcbtype *pcb)
{
    tt_soc *tty;
    profiletype *profile;
    int n;

    tty = (tt_soc *) pcb->tty;
    pad_putpush(pcb,FALSE);	/* Send any data pending */
    profile = pad_find_profile(DEFAULT_PROFILE);
    if (profile) {
       if (pad_debug) {
	   buginf("\nPAD%t: Loading profile %s", tty->ttynum, profile->name); 
       }
       for (n=0; n < profile->length; ) {
	   pad_option_emit(pcb, X3_SETANDREAD, profile->params[n],
			   profile->params[n+1]);
	   n+=2;
       }
       pad_putcontrol(pcb);
       do_x3list(tty->conntop, profile->params, profile->length, X3_SET);
   }
    else {
	pad_option_emit(pcb,X3_SETANDREAD, X3_LOCALECHO, 0);
	pad_option_emit(pcb,X3_SETANDREAD, X3_IDLETIME, 1);
	pad_option_emit(pcb,X3_SETANDREAD, X3_EDIT, 0);
	pad_option_emit(pcb,X3_SETANDREAD, X3_BREAK, 4+16+1);
	pad_putcontrol(pcb);
    }
}

/*
 * pad_find_profile
 * Find a profile with the given name
 */

profiletype *pad_find_profile (char *name)
{
    profiletype *profile;

    for (profile = (profiletype *) profileQ.qhead; profile;
	 profile = profile->next)
	if (!strcasecmp(profile->name, name))
	    return(profile);
    return(NULL);
}

/*
 * pad_flush_profile
 * Search the translation queue, removing any pointers matching
 * the profile we are deleting
 */ 

static void pad_flush_profile (profiletype *profile)
{
    translatetype *trans;

    for (trans = (translatetype *)translateQ->qhead;trans; trans = trans->next){
	if (INPROT(trans->type) == TRANS_X25)
	    if (trans->in.x25.profile == profile)
		trans->in.x25.profile = NULL;
	if (OUTPROT(trans->type) == TRANS_X25)
	    if (trans->out.x25.profile == profile)
		trans->out.x25.profile = NULL;
    }
}

/*
 * x29_command
 * Allow various PAD related things to be configured
 * **WARNING**: replace partialmatch comparisons with something else if
 *	        you ever use two commands that start with the same letter.
 */
void x29_command (parseinfo *csb)
{
    profiletype *profile;
    int i;

    if (csb->nvgen) {
	switch (csb->which) {
	  case X29_ACCESS_LIST:
	    regexpaccess_command(csb, x29_accesslists);
	    break;
	  case X29_PROFILE:
	    nv_write((boolean)profileQ.qhead,"!");
            for (profile = (profiletype *)profileQ.qhead; profile;
                 profile = profile->next) {
                nv_write(TRUE, "%s %s", csb->nv_command,
                         profile->name);
                for (i = 0; i < profile->length; i+=2){
                    nv_add(TRUE, " %d:%d",profile->params[i],
			   profile->params[i+1]);
                }
            }
            break;
	  case X29_INVITECLEAR_TIME:
	    if (x29_inviteclear_time == 0)
		nv_write(TRUE, "%s %s", csb->nv_command, "none");
	    else
		nv_write(x29_inviteclear_time != X29_INVITECLEAR_DEFAULT,
			 "%s %d", csb->nv_command, x29_inviteclear_time);
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
      case X29_ACCESS_LIST:
	regexpaccess_command(csb, x29_accesslists);
	break;
      case X29_PROFILE:
	if (GETOBJ(int,11)) {
	    sstrncpy(GETOBJ(string,1), DEFAULT_PROFILE, PROFILE_NAMELEN+1);
	}
	profile = pad_find_profile(GETOBJ(string,1));
	if (!profile && csb->sense) {
	    /* add a new profile */
	    profile = malloc_named(sizeof(profiletype), 
						   "X29 PROFILE");
	    if (!profile) {
		printf(nomemory);
		return;
            }
	    sstrncpy(profile->name, GETOBJ(string,1), PROFILE_NAMELEN+1);
	    enqueue(&profileQ, profile);
        } else if (!csb->sense) {
	    if (profile) {
		/* delete an existing profile */
		unqueue(&profileQ, profile);
		pad_flush_profile(profile);
                free(profile);
	    }
	    break;
        }
	/* modify an existing profile */
        i = 0;
        for (i = 0; i < GETOBJ(int,10); i += 2) {
            profile->params[i] = GETOBJ(string,2)[i];
            profile->params[i+1] = GETOBJ(string,2)[i+1];
        }
        profile->length = i;
        break;
      case X29_INVITECLEAR_TIME:
	if (csb->sense)
	    x29_inviteclear_time = GETOBJ(int,1);
	else
	    x29_inviteclear_time = X29_INVITECLEAR_DEFAULT;
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * show_x29
 *
 * Display X.29 information.
 */
void show_x29 (parseinfo *csb)
{
    show_regexpacl(x29_accesslists, GETOBJ(int,1), "X29");
}

/*
 * pad_conn_acct
 * update accounting info (packet and byte counts) in conn block by
 * looking at the guts of the pcb and lci.
 */
void pad_conn_acct (conntype *conn, acctinfo *acct)
{
#if 0
    pcbtype *pcb = (pcbtype *)conn->tcb;
    if (pcb) {
	acct->bytes_in = pcb->bytesincount - acct->bytes_in;
	acct->bytes_out = pcb->bytesoutcount - acct->bytes_out;
	acct->packets_in = pcb->pakincount - acct->packets_in;
	acct->packets_out = pcb->pakoutcount - acct->packets_out;
    }
#endif
    if (!(acct->flags & ACCT_F_FIRSTDONE)) {
	reg_invoke_aaa_acct_attr_add(acct, AAA_ATTR_PROTO, AAA_VAL_PROT_pad,
			     AAA_TYPE_STR, AAA_STRSIZE(AAA_VAL_PROT_pad));
	reg_invoke_aaa_acct_attr_add(acct, AAA_ATTR_ADDR, conn->hostname,
				     AAA_TYPE_STR, strlen(conn->hostname));
	acct->flags |= ACCT_F_FIRSTDONE;
    }
}
