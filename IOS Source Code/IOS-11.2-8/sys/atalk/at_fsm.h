/* $Id: at_fsm.h,v 3.2 1995/11/17 08:42:34 hampton Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/atalk/at_fsm.h,v $
 *------------------------------------------------------------------
 * Finite state machine for Appletalk.
 *
 * September 1993, Steven Lin
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: at_fsm.h,v $
 * Revision 3.2  1995/11/17  08:42:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:56:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/24  22:30:28  hampton
 * Further work to parts of Appletalk to make it use the passive timer
 * macros for all its timers.  It no longer have any direct references to
 * the system clock.  [CSCdi37581]
 *
 * Revision 2.2  1995/07/13 06:49:38  hampton
 * Convert Appletalk to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37037]
 *
 * Revision 2.1  1995/06/07 20:08:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __AT_FSM_H__
#define __AT_FSM_H__
/* events numbers can be reused by different state machine classes */
typedef int atfsmEvent;

/* special events: */
#define	ATFSM_NILEVENT	-1
#define	ATFSM_TIMEOUT	-2

/* structure of elements on fsm stateQ's: */
typedef struct atfsmEventRec_ {
	struct atfsmEventRec_ *link;	/* required by queue mgt. */
	atfsmEvent event;		/* the state transition event */
	u_long	arg;			/* misc. arg */
} atfsmEventRec;

/* structure of the FSM state variables: */
typedef struct at_fsm_ {
	queuetype	stateQ;		/* state event queue */
	char		*name;		/* name of FSM */
	boolean		traceInfo;	/* if TRUE if, trace fsm events */
	boolean		traceDebug;	/* if TRUE if, verbose trace */
	boolean		timeout;	/* TRUE if TIMEOUT event */
	boolean		new_state;	/* TRUE if just entered this state */
	sys_timestamp	timer;		/* timer */
	u_long		delay;		/* delay before TIMEOUT
					 * zero ==> no timeout events.
					 */
	char		*userArg;	/* user arg. passed thru fsm */
	void	(*initialState)(struct at_fsm_ *);	/* INITIAL state */
	void	(*haltState)(struct at_fsm_ *);		/* HALT state */
	void	(*stateFunc)(struct at_fsm_ *);	/* current state routine */
	void	(*prevStateFunc)(struct at_fsm_ *);
					/* "any state" return to function */
	char	*(*stateStr)(void (*xx)(struct at_fsm_ *));
					/* returns name of state func <xx> */
	char	*(*eventStr)(atfsmEvent);	/* returns name of fsm event */
	struct	atfsm_table_ {		/* declare state table struct... */
	   void (*curr)(struct at_fsm_ *);
	   atfsmEvent event;
	   void (*next)(struct at_fsm_ *);
	}	const	*stab;		/* ... and a pointer to one. */
} at_fsm;

typedef void    (*FSMFUNC)(struct at_fsm_ *);
#define NILFSMFUNC  ((FSMFUNC) NILVAL)

typedef struct atfsm_table_ atfsm_table;

/* note - the timer is stopped by atfsm_exec_fsm() to cause the
 * state routine to reset the start time.
 */
#define	ATFSM_SET_DELAY(FSM,DELAY)\
	{GET_TIMESTAMP((FSM)->timer); (FSM)->delay=DELAY;}

#define	ATFSM_DELAY_UP(FSM)\
	!CLOCK_IN_INTERVAL((FSM)->timer, (FSM)->delay)

#define	ATFSM_NO_TIMEOUT(FSM)\
	{TIMER_STOP((FSM)->timer);  (FSM)->delay = 0;}

#define	ATFSM_TRACE_INFO(FSM)	((FSM)->traceInfo)
#define	ATFSM_TRACE_DEBUG(FSM)	((FSM)->traceDebug)

#define	ATFSM_SET_TRACE_OFF(FSM)\
	{(FSM)->traceInfo=FALSE; (FSM)->traceDebug=FALSE;}
#define	ATFSM_SET_TRACE_INFO(FSM)\
	{(FSM)->traceInfo=TRUE; (FSM)->traceDebug=FALSE;}
#define	ATFSM_SET_TRACE_DEBUG(FSM)\
	{(FSM)->traceInfo=TRUE; (FSM)->traceDebug=TRUE;}

typedef enum {
    /* returned by atfsm_exec_fsm(): */
    ATFSM_EXEC_OK,
    ATFSM_HALTED,
    ATFSM_BAD_EVENT
} atfsmResult;

extern void atfsm_check_timer(at_fsm *fsm);
extern void atfsm_add_event(at_fsm *fsm, atfsmEvent ee, u_long arg);
extern FSMFUNC atfsm_next_state(at_fsm *fsm, atfsmEvent ee);
extern atfsmResult atfsm_exec_fsm(at_fsm *fsm);
extern void atfsm_clear_stateQ(at_fsm *fsm);
extern FSMFUNC atfsm_get_state(at_fsm *);

#endif
