/* $Id: ttystatem.c,v 3.3.28.2 1996/06/28 23:31:14 hampton Exp $
 * $Source: /release/112/cvs/Xsys/ts/ttystatem.c,v $
 *------------------------------------------------------------------
 * ttystatem.c  -- dispatch state machines for terminal lines.
 *
 * October 1990, Bill Westfield
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ttystatem.c,v $
 * Revision 3.3.28.2  1996/06/28  23:31:14  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.3.28.1  1996/03/18  22:20:15  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.1  1996/03/07  10:58:20  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3  1996/01/22  07:30:03  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  18:56:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:40:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/25  11:45:34  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.3  1995/07/14 07:03:25  hampton
 * Convert Terminal Server to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi37093]
 *
 * Revision 2.2  1995/06/20 03:45:13  ahh
 * CSCdi36095:  Connection routines use msclock
 * Remove more msclock references.
 *
 * Revision 2.1  1995/06/07  23:10:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "ttysrv.h"
#include "interface_private.h"
#include "config.h"
#include "connect.h"
#include "../ui/debug.h"
#include "ttystatem.h"
#include "../os/os_debug.h"
#include <ciscolib.h>
#include <string.h>

#define STATEM_NAMELEN 20
#define MAXBYTE 255
#define MAXSTATE 7

#define TTYSTATE_FLAGMASK ~7
#define TTYSTATE_STATEMASK 7
/*
 * State flags
 */
#define TTYSTATE_XMIT 8
#define TTYSTATE_DELAY 16

struct tty_statemtype_ {
    tty_statemtype *next;
    char name[STATEM_NAMELEN+1];
    uchar states[MAXSTATE+1][MAXBYTE+1];
};
/*
 * local storage
 */
static queuetype tty_statemQUEUE;
queuetype *tty_statemQ = &tty_statemQUEUE;

/*
 * forward references
 */

/*
 * external symbols
 */


/* 
 * statem_command
 * Parse the commands necessary to define a dispatch-character state machine.
 */
void statem_command (parseinfo *csb)
{
    tty_statemtype *statem;
    int thisstate = 0, nextstate = 0;
    int firstchar = 0, lastchar = 0;
    int i;

    if (csb->nvgen) {
	nv_write((boolean)tty_statemQ->qhead, "!");
	for (statem = (tty_statemtype *) tty_statemQ->qhead; statem;
	     statem = statem->next) {
	    for (thisstate = 0; thisstate < MAXSTATE; thisstate++) {
		for (firstchar = 0; firstchar < MAXBYTE; firstchar++) {
		    nextstate = statem->states[thisstate][firstchar];
		    for (i = firstchar+1; i < MAXBYTE; i++) {
			if (statem->states[thisstate][i] != nextstate) {
			    i--;
			    break;
			}
		    }
		    switch (nextstate) {
		      case 0:
			break;
		      case TTYSTATE_XMIT:
			nv_write(TRUE, "state-machine %s %d %d %d transmit",
				 statem->name, thisstate, firstchar, i);
			break;
		      default:
			nv_write(TRUE, "state-machine %s %d %d %d %d",
				 statem->name, thisstate, firstchar, i,
				 nextstate & TTYSTATE_STATEMASK);
			nv_add(nextstate & TTYSTATE_DELAY, " delay");
			break;
		    }
		    firstchar = i;
		}
	    }
	}
	return;
    }

    statem = find_namedthing(tty_statemQ, GETOBJ(string,1));
    if (csb->sense == FALSE) {
	if (statem) {
	    unqueue(tty_statemQ, statem);
	    for (i=0; i < ALLlines; i++){
		if (MODEMS[i]->dispatch_machine == statem){
		    MODEMS[i]->dispatch_machine = NULL;
		}
	    }
	    free(statem);
	}
    } else {
	if (!statem) {
	    statem = malloc(sizeof(tty_statemtype));
	    if (!statem){
		return;
	    }
	    sstrncpy(statem->name, GETOBJ(string,1), STATEM_NAMELEN+1);
	    enqueue(tty_statemQ, statem);
	}
	thisstate = GETOBJ(int,1);
	firstchar = GETOBJ(int,2);
	lastchar = GETOBJ(int,3);
	if (GETOBJ(int,5)) {
	    nextstate = TTYSTATE_XMIT;
	} else {
	    nextstate = GETOBJ(int,4);
	}
	if (GETOBJ(int,6)) {
	    nextstate |= TTYSTATE_DELAY;
	}

	if (nextstate > TTYSTATE_DELAY+MAXSTATE
	    || nextstate < 0
	    || thisstate > MAXSTATE
	    || thisstate < 0
	    || firstchar > MAXBYTE
	    || firstchar < 0
	    || lastchar > MAXBYTE
	    || lastchar < 0
	    || lastchar < firstchar) {
	    printf("\nInvalid dispatch-state command");
	    return;
	}
    }
    
    for (i=firstchar; i <= lastchar; i++){
	statem->states[thisstate][i] = nextstate;
    }
}

void tty_disstate_command (parseinfo *csb)
{
    tty_statemtype *statem;

    if (csb->nvgen) {
	tt_soc *tty = MODEMS[csb->line1];
        nv_write((boolean)tty->dispatch_machine, "%s %s", csb->nv_command,
                ((tty_statemtype *)tty->dispatch_machine)->name);
	return;
    }


    if (csb->sense == FALSE) {
	setinteger(csb, 0, &console->dispatch_machine);
    } else {
	statem = find_namedthing(tty_statemQ, GETOBJ(string,1));
	if (!statem) {
	    printf("\nNo such state machine name");
	    return;
	}
	setinteger(csb, (int)statem, &console->dispatch_machine);
    }
    setinteger(csb, 0, &console->dispatch_state);
}

boolean tty_runstates (conntype *conn, int ch)
{
    tt_soc *tty = conn->tty;

    tty_statemtype *statem = (tty_statemtype *)tty->dispatch_machine;
    int pushflag = FALSE;

    tty->dispatch_state = statem->states[tty->dispatch_state][ch];
    if (tty->dispatch_state >= TTYSTATE_XMIT) {
	switch (tty->dispatch_state & TTYSTATE_FLAGMASK) {
	  case TTYSTATE_XMIT:
	    pushflag = TRUE;
	    tty->dispatch_state = 0;
	    break;
	  case TTYSTATE_DELAY:
	    TIMER_START(conn->dispatchtimer, ONESEC/20);
	    tty->dispatch_state &= TTYSTATE_STATEMASK;
	    break;
	}
    }
    if (modem_debug) {
	buginf("\nTTY%t: switched to state %d%s", tty->ttynum,
	       tty->dispatch_state, pushflag ? " (Transmit)" : "");
	if (TIMER_RUNNING(conn->dispatchtimer))
	    buginf(" (delay %d)", TIME_LEFT_SLEEPING(conn->dispatchtimer));
    }
    return(pushflag);
}
