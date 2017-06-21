/* $Id: macip_fsm.h,v 3.2 1995/11/17 08:44:59 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/atalk/macip_fsm.h,v $
 *------------------------------------------------------------------
 * macip_fsm.h	- information for users of macip finite state machine
 *
 * September 1991, Russ Tremain, Tremain Laboratories, under contract
 * 	for cisco Systems, Inc.
 *
 * Copyright (c) 1991-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: macip_fsm.h,v $
 * Revision 3.2  1995/11/17  08:44:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:58:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:51:08  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/13  06:50:26  hampton
 * Convert Appletalk to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37037]
 *
 * Revision 2.1  1995/06/07 20:10:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__macip_fsm_h__
#define	__macip_fsm_h__

typedef enum {
    MACIP_FSM_NILEVENT,
    MACIP_FSM_ADD_SERVER,
    MACIP_FSM_TIMEOUT,
    MACIP_FSM_ADD_RESOURCE,
    MACIP_FSM_ZONE_SEEDED,
    MACIP_FSM_START_OK,
    MACIP_FSM_START_FAIL,
    MACIP_FSM_REG_OK,
    MACIP_FSM_REG_FAIL,
    MACIP_FSM_CONFIRM_OK,
    MACIP_FSM_ZONE_DOWN,
    MACIP_FSM_DEL_SERVER,
    MACIP_FSM_DEL_RESOURCE,
    MACIP_FSM_YES_RESOURCES,
    MACIP_FSM_NO_RESOURCES
} macipFsmEvent;

/* structure of elements on server stateQ's: */
typedef struct macipServerEvent_ {
	struct macipServerEvent_ *link;		/* required by queue mgt. */
	macipFsmEvent event;			/* the state transition event */
	ulong	arg;		/* misc. arg used for:
				 * -resource id for {ADD,DEL}RESOURCE event
				 */
} macipServerEvent;

#define	MACIP_SET_DELAY(SS,DELAY)\
    { GET_TIMESTAMP((SS)->timer); (SS)->delay = DELAY; }

#define	MACIP_DELAY_UP(SS)\
	(CLOCK_OUTSIDE_INTERVAL((SS)->timer, (SS)->delay))

#define	MACIP_NO_TIMEOUT(SS)	MACIP_SET_DELAY(SS,0)

extern boolean macip_halt_state(PFV sfunc);
extern PFV macip_next_state(server_rec *ss, macipFsmEvent evt);
extern void macip_jam_event(server_rec *ss, macipFsmEvent ee, ulong arg);
extern void macip_add_event(server_rec *ss, macipFsmEvent ee, ulong arg);
extern char *macip_eventStr(macipFsmEvent ee);
extern char *macip_stateStr(PFV statefunc);
extern void macip_init_state(server_rec *ss);

#endif
