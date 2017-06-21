/* $Id: at_fsm.c,v 3.3.60.2 1996/03/23 01:26:28 slin Exp $
 * $Source: /release/111/cvs/Xsys/atalk/at_fsm.c,v $
 *------------------------------------------------------------------
 * Finite state machine for Appletalk.
 *
 * September 1993, Steven Lin
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: at_fsm.c,v $
 * Revision 3.3.60.2  1996/03/23  01:26:28  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.3.60.1  1996/03/18  18:52:16  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  08:28:41  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  13:33:37  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  08:42:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:05:27  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:56:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/24  22:30:26  hampton
 * Further work to parts of Appletalk to make it use the passive timer
 * macros for all its timers.  It no longer have any direct references to
 * the system clock.  [CSCdi37581]
 *
 * Revision 2.2  1995/07/13 06:49:37  hampton
 * Convert Appletalk to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37037]
 *
 * Revision 2.1  1995/06/07 20:08:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef SUNTEST
#include "master.h"
#include <string.h>
#endif

#include "macip_util.h"
#include "at_fsm.h"

#ifndef SUNTEST

/* definitions for at_err(): */
#if 0					/* XXX temporary */
extern atalkdebugtype *appletalk_debug;
#define	DEBUGSWITCH (at_debug(appletalk_debug, 0))
#else
#define DEBUGSWITCH TRUE
#endif
#else  SUNTEST
#define	DEBUGSWITCH TRUE
#endif  SUNTEST

#define	MAX_ATFSM_FREELIST	20	/* how many fsm-event records to
					 * malloc before garbage collection */
#define	MIN_ATFSM_TO_FREE	5	/* minimum beyond max free before
					 * calling garbage collection */
static queuetype atfsm_freeQ;		/* the free list */
/*
 * Forward Declarations
 */
static void atfsm_dump_fsm (at_fsm *fsm);

/*************************  PRIVATE FUNCTIONS  ************************/

static atfsmEventRec *
atfsm_get_fsm_event(void)
{
    register atfsmEventRec *erec;

    if (QUEUEEMPTY(&atfsm_freeQ))
	erec = malloc(sizeof(atfsmEventRec));
    else
	erec = dequeue(&atfsm_freeQ);

    return(erec);
}

/*
 * atfsm_empty_trash -
 *	routine to return excess unused storage to system.
 */
static void
atfsm_empty_trash (void)
{
    register atfsmEventRec *erec;

    while (QUEUESIZE(&atfsm_freeQ) >= MAX_ATFSM_FREELIST) {
	erec = dequeue(&atfsm_freeQ);
	free(erec);
    }
}

/*
 * atfsm_free_fsm_event -
 *	return an fsm event record to free list, perform
 *	garbage collection if needed.
 *	
 */
static void
atfsm_free_fsm_event(atfsmEventRec *erec)
{
    enqueue (&atfsm_freeQ, erec);

    if (QUEUESIZE(&atfsm_freeQ) > (MAX_ATFSM_FREELIST + MIN_ATFSM_TO_FREE))
	atfsm_empty_trash();	/* empty down to MAX */
}


/*************************  EXPORTED FUNCTIONS  ***********************/


/*
 * atfsm_check_timer -
 *		if state has a timer, and timer expired, then
 *		post a ATFSM_TIMEOUT event
 */
void
atfsm_check_timer (at_fsm *fsm)
{
    /* is there a timer running for this state */
    if (!TIMER_RUNNING(fsm->timer))
        return;

    /* if delay expired... */
    if (ATFSM_DELAY_UP(fsm))
        atfsm_add_event(fsm, ATFSM_TIMEOUT, 0);
}


/*
 * atfsm_add_event -
 *	
 *	add a state transition event to the END of the event queue,
 *	execute event queue.
 *	
 */
void
atfsm_add_event (at_fsm *fsm, atfsmEvent evt, u_long arg)
{
    atfsmEventRec *erec;

    /* allocate and enqueue an event record: */
    erec = atfsm_get_fsm_event();
    if (erec != NULL) {
        erec->event = evt;
        erec->arg = arg;
        enqueue(&fsm->stateQ, erec);
    }

    if (ATFSM_TRACE_DEBUG(fsm))
	buginf("\nADD EVENT<%s>", fsm->eventStr(erec->event));
}
 
/*
 * atfsm_next_state -
 *	lookup next state in <fsm> based on current state and event <evt>
 *	returns NILFSMFUNC if not in table.
 */
FSMFUNC
atfsm_next_state (at_fsm *fsm, atfsmEvent evt)
{
    register int ii;
    register atfsm_table const *stab = fsm->stab;

    /* first, look for "fully qualified" state transition: */
    for (ii=0; stab[ii].event != ATFSM_NILEVENT; ii++) {
        if (stab[ii].curr == fsm->stateFunc && stab[ii].event == evt) {
            /* if "to previous" transition... */
            if (stab[ii].next == NILFSMFUNC) {
                FSMFUNC tmpfunc = fsm->prevStateFunc;

                fsm->prevStateFunc = fsm->initialState;
                /* ... then return previous state: */
                return(tmpfunc);
            }
            else    /* "normal" state change: */
                return(stab[ii].next);
        }
    }

    /* not found, look for "from any state" transition: */
    for (ii=0; stab[ii].event != ATFSM_NILEVENT; ii++) {
        if (stab[ii].curr == NILFSMFUNC && stab[ii].event == evt) {
            if (stab[ii].next == NILFSMFUNC)
                return(fsm->stateFunc);  /* ignore */
            else {
                /* save current state to return to: */
                fsm->prevStateFunc = fsm->stateFunc;
                return(stab[ii].next);
            }
        }
    }

    /* illegal state transition event */
    return(NILFSMFUNC);
}

atfsmResult
atfsm_exec_fsm (at_fsm *fsm)
/*
 * execute the state machine <fsm>.
 *
 * Note:  this routine may be called with any granularity.
 * E.g., you may want to execute 1 or 100 times per second, depending on
 * the timing inherent in your state machine.
 *
 * This routine is normally driven by a background process, but
 * can alternately be invoked explicitly after an event has been
 * posted.  However, be warned that event queues are not protected
 * from interrupts, so don't call from an interrupt routine.
 */
{
    register atfsmEvent evt;
    register atfsmEventRec *erec;

    /* if no new fsm events... */
    if (QUEUESIZE(&fsm->stateQ) <= 0) {
	/* has this state's timer expired? Post an event if yes: */
	atfsm_check_timer(fsm);
    }

    /* if still no new fsm events (no TIMEOUT events)... */
    if (QUEUESIZE(&fsm->stateQ) <= 0) {
        (*fsm->stateFunc)(fsm);	/* ...just execute current state */
    }

    /* while state machine events... */
    while (QUEUESIZE(&fsm->stateQ) > 0) {
	FSMFUNC prevstate;

        erec = dequeue(&fsm->stateQ);
        evt = erec->event;
	atfsm_free_fsm_event(erec);

        prevstate = fsm->stateFunc;

        /* lookup new state: */
        fsm->stateFunc = atfsm_next_state(fsm, evt);
        if (fsm->stateFunc == NILFSMFUNC) {
	    fsm->stateFunc = prevstate;		/* error recovery */
	    at_err(MODID,"illegal state transition event, %s(%s)==>??",
		fsm->stateStr(fsm->stateFunc),
		fsm->eventStr(evt));
	    return(ATFSM_BAD_EVENT);
        }

        /* let states know about timeout events: */
        fsm->timeout = (evt == ATFSM_TIMEOUT);

        /* if new state or ATFSM_TIMEOUT... */
        if (fsm->stateFunc != prevstate || fsm->timeout) {
	    fsm->new_state = TRUE;
            fsm->delay = 0;
	    TIMER_STOP(fsm->timer);  /* ... then reset state timers */
	}
	
        if (ATFSM_TRACE_INFO(fsm)) {
            /* print state transition: */
            buginf("\nAT FSM TRANSITION %s@%lx:  %s(%s)==>%s",
                fsm->name,
                fsm,
                fsm->stateStr(prevstate),
                fsm->eventStr(evt),
                fsm->stateStr(fsm->stateFunc));
        }

        /* process event: */
        if (fsm->stateFunc == fsm->haltState) {
	    /* clear state Q: */
	    atfsm_clear_stateQ(fsm);
            (*fsm->haltState)(fsm);	/* execute HALT state */
            return(ATFSM_HALTED);			/* HALT */
        }
        else
            (*fsm->stateFunc)(fsm);
    }

    return(ATFSM_EXEC_OK);
}

/*
 * atfsm_clear_stateQ -
 *	clear the state queue, reset timers
 */
void
atfsm_clear_stateQ (at_fsm *fsm)
{
    if (ATFSM_TRACE_INFO(fsm)) {
        if (QUEUESIZE(&fsm->stateQ) > 0) {
	    atfsm_dump_fsm(fsm);
	    buginf("\n>>WARNING:  UNPROCESSED state events, fsm '%s'@%lx",
		fsm->name, fsm);
        }
    }

    /* don't generate any more timer events: */
    ATFSM_NO_TIMEOUT(fsm);

    /* delete remaining states: */
    while (QUEUESIZE(&fsm->stateQ) > 0) {
	atfsmEventRec *erec = dequeue(&fsm->stateQ);

        if (ATFSM_TRACE_INFO(fsm))
            buginf("\n\tCLEAR EVENT<%s>, fsm '%s'@%lx",
                    fsm->eventStr(erec->event), fsm->name, fsm);

	atfsm_free_fsm_event(erec);
    }
}

/*
 * atfsm_get_state
 *
 * Returns current state.
 */
FSMFUNC
atfsm_get_state (at_fsm *fsm)
{
    return (fsm->stateFunc);
}


static void
atfsm_dump_state_table (at_fsm *fsm)
/* dump the state table */
{
    register atfsm_table const *stab = fsm->stab;
    register int ii;

    printf("\n+--------------------+--------------------------+--------------------+");
    printf("\n| %-18s | %-24s | %-18s |", "State", "Event", "New State");
    printf("\n+--------------------+--------------------------+--------------------+");
    stab = fsm->stab;
    for (ii=0; stab[ii].event != ATFSM_NILEVENT; ii++) {
	printf("\n| %-18s | %-24s | %-18s |",
	    fsm->stateStr(stab[ii].curr),
	    fsm->eventStr(stab[ii].event),
	    fsm->stateStr(stab[ii].next));
    }
    printf("\n+--------------------+--------------------------+--------------------+");
}

/*
 * print contents state machine variable <fsm>
 */
static void
atfsm_dump_fsm (at_fsm *fsm)
{
    printf("\n----------State Machine '%s@%0lx'----------", fsm->name, fsm);
    printf("\nstateQ=%0lx, size=%d", &fsm->stateQ, QUEUESIZE(&fsm->stateQ));
    printf("\ntimeout=%d, timer=%#Ta, delay=%d, userArg=%0lx",
	fsm->timeout, fsm->timer, fsm->delay, fsm->userArg);

    printf("\ninitialState=%s", fsm->stateStr(fsm->initialState));
    printf("\nhaltState=%s", fsm->stateStr(fsm->haltState));
    printf("\nstateFunc (current state)=%s", fsm->stateStr(fsm->stateFunc));
    printf("\nprevStateFunc=%s", fsm->stateStr(fsm->prevStateFunc));

    atfsm_dump_state_table(fsm);	 /* dump the state table */
}
