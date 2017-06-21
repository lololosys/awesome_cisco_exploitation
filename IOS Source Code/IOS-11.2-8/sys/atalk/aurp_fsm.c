/* $Id: aurp_fsm.c,v 3.2.60.3 1996/07/19 22:44:23 snyder Exp $
 * $Source: /release/112/cvs/Xsys/atalk/aurp_fsm.c,v $
 *------------------------------------------------------------------
 * Dummy file for Aurp_branch
 *
 * February 1994, Steven Lin
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: aurp_fsm.c,v $
 * Revision 3.2.60.3  1996/07/19  22:44:23  snyder
 * CSCdi63560:  move fsm tables to text space from data
 *              gets 53,716 bytes of data back for run from flash platforms
 *              gets 68 bytes of image for all
 *
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/05/02  08:41:16  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.2.60.1  1996/03/23  01:28:17  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.2  1995/11/17  08:44:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:58:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/24  22:30:41  hampton
 * Further work to parts of Appletalk to make it use the passive timer
 * macros for all its timers.  It no longer have any direct references to
 * the system clock.  [CSCdi37581]
 *
 * Revision 2.1  1995/06/07 20:10:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef SUNTEST
#include "master.h"
#endif
#include <interface.h>
#include "packet.h"
#include "macip_util.h"
#include "at_fsm.h"
#include "aurp_fsm.h"
#include "atalk_private.h"
#include "at_aurp.h"
#include "ataurp_debug.h"

#define    FSM_NAME    "aurp"

static const ulong TickleIntervals[] = { 10, 5, 5, 5 };

static const ulong OpenReqIntervals[] = { 10, 10, 10, 20, 35, 65, 130, 260, 520 };

static const ulong NullRIIntervals[] = { 10, 5, 5, 5 };

const ulong RIUpdIntervals [] = { 10, 5, 5, 5 };

static const ulong RDIntervals[] = { 10, 5, 5, 5 };

static const ulong RIReqIntervals[] = { 10, 15, 25, 30, 30, 30, 30, 30, 30 };

const ulong RIRspIntervals[] = { 10, 5, 5, 5 };

/*************************  PRIVATE FUNCTIONS  ************************/

void
aurp_fsm_DR_Unconnected (at_fsm *fsm)
/*
| DR_Unconnected      | INITIATE        | DR_Open_Rsp_Wait    | 
*/
{
    at_aurp_info *aurp_info = (at_aurp_info *) fsm->userArg;

    if (ATFSM_TRACE_DEBUG(fsm))
	buginf("\nFSM '%s.%lx', state '%s'",
		fsm->name, fsm, fsm->stateStr(fsm->stateFunc));

    /* set timer when state first entered: */
    if (fsm->new_state) {
	fsm->new_state = FALSE;
	if (at_debug(atalkaurp_connection_debug, ATALK_INTERFACE_MASK,
		     aurp_info->idb->phys_idb))
	    buginf("\nAT: %s: Data Receiver state changed -> Unconnected",
		   idb_get_namestring(aurp_info->idb->phys_idb));
		   
	ATFSM_NO_TIMEOUT(fsm);	/* state has no TIMEOUT's */

	aurp_info->r_connid = aurp_next_connid();
	aurp_info->r_sui = DEFAULT_SUI;
	aurp_info->r_sequence = 1;
	aurp_info->r_remapping = FALSE;
	aurp_info->r_hopreduce = FALSE;
	aurp_info->r_informed = FALSE;
	TIMER_STOP(aurp_info->r_lastheard);
    }
    /* Prod data sender to check his connection */
    if (atfsm_get_state(&aurp_info->sender_fsm) == aurp_fsm_DS_Connected) {
	aurp_fsm(&aurp_info->sender_fsm, AURP_FSM_OPENREQBAD);
    }
    atfsm_add_event(fsm, AURP_FSM_INITIATE, 0);
}

void
aurp_fsm_DR_Open_Rsp_Wait (at_fsm *fsm)
/*
| DR_Open_Rsp_Wait    | TIMEOUT2        | DR_Unconnected      | 
| DR_Open_Rsp_Wait    | OPEN_RSPBAD     | DR_Unconnected      | 
| DR_Open_Rsp_Wait    | OPEN_RSPGOOD    | DR_Connected        | 
*/
{
    at_aurp_info *aurp_info = (at_aurp_info *) fsm->userArg;

    if (ATFSM_TRACE_DEBUG(fsm))
	buginf("\nFSM '%s.%lx', state '%s'",
		fsm->name, fsm, fsm->stateStr(fsm->stateFunc));

    /* set timer when state first entered: */
    if (fsm->new_state) {
	fsm->new_state = FALSE;
	if (at_debug(atalkaurp_connection_debug, ATALK_INTERFACE_MASK,
		     aurp_info->idb->phys_idb))
	    buginf("\nAT: %s: Data Receiver state changed -> Open_Rsp_Wait",
		   idb_get_namestring(aurp_info->idb->phys_idb));
	ATFSM_NO_TIMEOUT(fsm);
	aurp_info->r_retry_count = 0;
	TIMER_START(aurp_info->r_retry_timer, OpenReqIntervals[0] * ONESEC);
	aurp_Send_Open_Req(aurp_info);	/* Send Open-Req packet */
    }

    if (AWAKE(aurp_info->r_retry_timer)) {
	if (++aurp_info->r_retry_count
	    < sizeof(OpenReqIntervals) / sizeof(ulong)) {
	    TIMER_START(aurp_info->r_retry_timer,
			OpenReqIntervals[aurp_info->r_retry_count] * ONESEC);
	    aurp_Send_Open_Req(aurp_info);
	} else {
	    atfsm_add_event(fsm, AURP_FSM_TIMEOUT2, 0);
	}
    }
}

void
aurp_fsm_DR_Connected (at_fsm *fsm)
/*
| DR_Connected        | GET_RI          | DR_RI_Rsp_Wait      | 
| DR_Connected        | TIMEOUTLHFT     | DR_Tickle_Ack_Wait  | 
| DR_Connected        | RD              | DR_Unconnected      |
*/
{
    at_aurp_info *aurp_info = (at_aurp_info *) fsm->userArg;

    if (ATFSM_TRACE_DEBUG(fsm))
	buginf("\nFSM '%s.%lx', state '%s'",
		fsm->name, fsm, fsm->stateStr(fsm->stateFunc));

    /* set timer when state first entered: */
    if (fsm->new_state) {
	fsm->new_state = FALSE;
	if (at_debug(atalkaurp_connection_debug, ATALK_INTERFACE_MASK,
		     aurp_info->idb->phys_idb))
	    buginf("\nAT: %s: Data Receiver state changed -> Connected",
		   idb_get_namestring(aurp_info->idb->phys_idb));
	ATFSM_NO_TIMEOUT(fsm);
    }

    /* Do we need to get our peer's routing table? */
    if (!aurp_info->r_informed) {
	atfsm_add_event(fsm, AURP_FSM_GET_RI, 0);
	return;
    }

    /* Last-Heard-From Timeout? */
    if (ELAPSED_TIME(aurp_info->r_lastheard) > aurp_info->r_tickletime) {
	atfsm_add_event(fsm, AURP_FSM_TIMEOUTLHFT, 0);
	return;
    }
}

void
aurp_fsm_DR_RI_Rsp_Wait (at_fsm *fsm)
/*
| DR_RI_Rsp_Wait      | TIMEOUT2        | DR_Unconnected      | 
| DR_RI_Rsp_Wait      | RI_RSP          | DR_Connected        | 
| DR_RI_Rsp_Wait      | RD              | DR_Unconnected      | 
*/
{
    at_aurp_info *aurp_info = (at_aurp_info *) fsm->userArg;

    if (ATFSM_TRACE_DEBUG(fsm))
	buginf("\nFSM '%s.%lx', state '%s'",
		fsm->name, fsm, fsm->stateStr(fsm->stateFunc));

    /* set timer when state first entered: */
    if (fsm->new_state) {
	fsm->new_state = FALSE;
	if (at_debug(atalkaurp_connection_debug, ATALK_INTERFACE_MASK,
		     aurp_info->idb->phys_idb))
	    buginf("\nAT: %s: Data Receiver state changed -> RI_Rsp_Wait",
		   idb_get_namestring(aurp_info->idb->phys_idb));
	ATFSM_NO_TIMEOUT(fsm);	/* state has no TIMEOUT's */
	aurp_info->r_retry_count = 0;
	TIMER_START(aurp_info->r_retry_timer, RIReqIntervals[0]);
	aurp_Send_RI_Req(aurp_info);
    }

    if (AWAKE(aurp_info->r_retry_timer)) {
	if (++aurp_info->r_retry_count
	    < sizeof(RIReqIntervals) / sizeof(ulong)) {
	    TIMER_START(aurp_info->r_retry_timer,
			RIReqIntervals[aurp_info->r_retry_count] * ONESEC);
	    aurp_Send_RI_Req(aurp_info);
	} else {
	    aurp_ClearPathsFor(aurp_info->idb);
	    atfsm_add_event(fsm, AURP_FSM_TIMEOUT2, 0);
	}
    }
}

void
aurp_fsm_DR_Tickle_Ack_Wait (at_fsm *fsm)
/*
| DR_Tickle_Ack_Wait  | TIMEOUT2        | DR_Unconnected      | 
| DR_Tickle_Ack_Wait  | RD              | DR_Unconnected      | 
| DR_Tickle_Ack_Wait  | RI_RSP          | DR_Connected        | 
*/
{
    at_aurp_info *aurp_info = (at_aurp_info *) fsm->userArg;

    if (ATFSM_TRACE_DEBUG(fsm))
	buginf("\nFSM '%s.%lx', state '%s'",
		fsm->name, fsm, fsm->stateStr(fsm->stateFunc));

    /* set timer when state first entered: */
    if (fsm->new_state) {
	fsm->new_state = FALSE;
	if (at_debug(atalkaurp_connection_debug, ATALK_INTERFACE_MASK,
		     aurp_info->idb->phys_idb))
	    buginf("\nAT: %s: Data Receiver state changed -> Tickle_Ack_Wait",
		   idb_get_namestring(aurp_info->idb->phys_idb));
	ATFSM_NO_TIMEOUT(fsm);
	aurp_info->r_retry_count = 0;
	TIMER_START(aurp_info->r_retry_timer, TickleIntervals[0] * ONESEC);
	aurp_Send_Tickle(aurp_info);
    }

    if (AWAKE(aurp_info->r_retry_timer)) {
	if (++aurp_info->r_retry_count
	    < sizeof(TickleIntervals) / sizeof(ulong)) {
	    TIMER_START(aurp_info->r_retry_timer,
			TickleIntervals[aurp_info->r_retry_count] * ONESEC);
	    aurp_Send_Tickle(aurp_info);
	} else {
	    aurp_ClearPathsFor(aurp_info->idb);
	    aurp_UpdateNeighborEntry(aurp_info, neighborDown);
	    atfsm_add_event(fsm, AURP_FSM_TIMEOUT2, 0);
	}
    }
}

static void 
aurp_fsm_DR_RI_Ack_Wait (at_fsm *fsm)
/*
| DR_RI_Ack_Wait      | RI_ACK_DONE     | DR_Unconnected      |
*/
{
    at_aurp_info *aurp_info = (at_aurp_info *) fsm->userArg;

    if (ATFSM_TRACE_DEBUG(fsm))
	buginf("\nFSM '%s.%lx', state '%s'",
		fsm->name, fsm, fsm->stateStr(fsm->stateFunc));

    /* set timer when state first entered: */
    if (fsm->new_state) {
	fsm->new_state = FALSE;
	if (at_debug(atalkaurp_connection_debug, ATALK_INTERFACE_MASK,
		     aurp_info->idb->phys_idb))
	    buginf("\nAT: %s: Data Receiver state changed -> RI_Ack_Wait",
		   idb_get_namestring(aurp_info->idb->phys_idb));
	GET_TIMESTAMP(fsm->timer);
	atfsm_add_event(fsm, AURP_FSM_RI_ACK_DONE, 0);
	return;
    }


}
    
void
aurp_fsm_DS_Unconnected (at_fsm *fsm)
/*
| DS_Unconnected      | INITIATE        | DS_Connected        | 
*/
{
    at_aurp_info *aurp_info = (at_aurp_info *) fsm->userArg;

    if (ATFSM_TRACE_DEBUG(fsm))
	buginf("\nFSM '%s.%lx', state '%s'",
		fsm->name, fsm, fsm->stateStr(fsm->stateFunc));

    /* set timer when state first entered: */
    if (fsm->new_state) {
	fsm->new_state = FALSE;
	if (at_debug(atalkaurp_connection_debug, ATALK_INTERFACE_MASK,
		     aurp_info->idb->phys_idb))
	    buginf("\nAT: %s: Data Sender state changed -> Unconnected",
		   idb_get_namestring(aurp_info->idb->phys_idb));
	ATFSM_NO_TIMEOUT(fsm);	/* state has no TIMEOUT's */
	
	aurp_FreeResources(aurp_info);
	aurp_info->s_retry_count = 0;
	aurp_info->s_sequence = 1;
	aurp_info->s_informed = FALSE;
	aurp_info->s_last_sent_update = 0;
    }

    /* Do nothing */
}

void
aurp_fsm_DS_Connected (at_fsm *fsm)
/*
| DS_Connected        | RI_REQ          | DS_RI_Ack_Wait1     | 
| DS_Connected        | TIMEOUTUPDATE   | DS_RI_Ack_Wait2     | 
| DS_Connected        | BREAK           | DS_Send_RD          |
| DS_Connected        | OPENREQBAD      | DS_RI_Ack_Wait4     | 
| DS_Connected	      | RD		| DS_Unconnected      |
*/
{
    at_aurp_info *aurp_info = (at_aurp_info *) fsm->userArg;

    if (ATFSM_TRACE_DEBUG(fsm))
	buginf("\nFSM '%s.%lx', state '%s'",
		fsm->name, fsm, fsm->stateStr(fsm->stateFunc));

    /* set timer when state first entered: */
    if (fsm->new_state) {
	fsm->new_state = FALSE;
	if (at_debug(atalkaurp_connection_debug, ATALK_INTERFACE_MASK,
		     aurp_info->idb->phys_idb))
	    buginf("\nAT: %s: Data Sender state changed -> Connected",
		   idb_get_namestring(aurp_info->idb->phys_idb));
	ATFSM_NO_TIMEOUT(fsm);	/* state has no TIMEOUT's */
    }

#if 0
    if (AWAKE(aurp_info->s_update_expiration_timer)) {
	if (aurp_Build_RIUpd(aurp_info))
	    atfsm_add_event(fsm, AURP_FSM_TIMEOUTUPDATE, 0);
	else
	    TIMER_START(aurp_info->s_update_expiration_timer,
			aurp_info->s_update_rate * 10 * ONESEC);
    }
#endif
}

void
aurp_fsm_DS_RI_Ack_Wait1 (at_fsm *fsm)
/*
| DS_RI_Ack_Wait1     | TIMEOUT2        | DS_Unconnected      | 
| DS_RI_Ack_Wait1     | RI_ACK_DONE     | DS_Connected        | 
*/
{
    at_aurp_info *aurp_info = (at_aurp_info *) fsm->userArg;

    if (ATFSM_TRACE_DEBUG(fsm))
	buginf("\nFSM '%s.%lx', state '%s'",
		fsm->name, fsm, fsm->stateStr(fsm->stateFunc));

    /* set timer when state first entered: */
    if (fsm->new_state) {
	fsm->new_state = FALSE;
	if (at_debug(atalkaurp_connection_debug, ATALK_INTERFACE_MASK,
		     aurp_info->idb->phys_idb))
	    buginf("\nAT: %s: Data Sender state changed -> RI_Ack_Wait1",
		   idb_get_namestring(aurp_info->idb->phys_idb));
	ATFSM_NO_TIMEOUT(fsm);	/* state has no TIMEOUT's */
	aurp_info->s_retry_count = 0;
	TIMER_START(aurp_info->s_retry_timer, RIRspIntervals[0] * ONESEC);
	aurp_Build_RIRsps(aurp_info);
	aurp_Send_RI_Rsp(aurp_info);
    }

    if (AWAKE(aurp_info->s_retry_timer)) {
	if (++aurp_info->s_retry_count
	    < sizeof(RIRspIntervals) / sizeof(ulong)) {
	    TIMER_START(aurp_info->s_retry_timer,
			RIRspIntervals[aurp_info->s_retry_count] * ONESEC);
	    aurp_Send_RI_Rsp(aurp_info);
	} else {
	    atfsm_add_event(fsm, AURP_FSM_TIMEOUT2, 0);
	}
    }
}

void
aurp_fsm_DS_RI_Ack_Wait2 (at_fsm *fsm)
/*
| DS_RI_Ack_Wait2     | TIMEOUT2        | DS_Unconnected      | 
| DS_RI_Ack_Wait2     | RI_ACK_DONE     | DS_Connected        | 
*/
{
    at_aurp_info *aurp_info = (at_aurp_info *) fsm->userArg;

    if (ATFSM_TRACE_DEBUG(fsm))
	buginf("\nFSM '%s.%lx', state '%s'",
		fsm->name, fsm, fsm->stateStr(fsm->stateFunc));

    /* set timer when state first entered: */
    if (fsm->new_state) {
	fsm->new_state = FALSE;
	if (at_debug(atalkaurp_connection_debug, ATALK_INTERFACE_MASK,
		     aurp_info->idb->phys_idb))
	    buginf("\nAT: %s: Data Sender state changed -> RI_Ack_Wait2",
		   idb_get_namestring(aurp_info->idb->phys_idb));
	ATFSM_NO_TIMEOUT(fsm);	/* state has no TIMEOUT's */
	aurp_info->s_retry_count = 0;
	TIMER_START(aurp_info->s_retry_timer, RIUpdIntervals[0] * ONESEC);
	aurp_Send_RI_Upd(aurp_info);
    }

    if (AWAKE(aurp_info->s_retry_timer)) {
	if (++aurp_info->s_retry_count
	    < sizeof(RIUpdIntervals) / sizeof(ulong)) {
	    TIMER_START(aurp_info->s_retry_timer,
			RIUpdIntervals[aurp_info->s_retry_count] * ONESEC);
	    aurp_Send_RI_Upd(aurp_info);
	} else {
	    atfsm_add_event(fsm, AURP_FSM_TIMEOUT2, 0);
	}
    }
}

void
aurp_fsm_DS_Send_RD (at_fsm *fsm)
/*
| DS_Send_RD	      | SEND_RD		| DS_RI_Ack_Wait3     |
*/
{
    if (ATFSM_TRACE_DEBUG(fsm))
	buginf("\nFSM '%s.%lx', state '%s'",
		fsm->name, fsm, fsm->stateStr(fsm->stateFunc));

    /* set timer when state first entered: */
    if (fsm->new_state) {
	fsm->new_state = FALSE;
	ATFSM_NO_TIMEOUT(fsm);	/* state has no TIMEOUT's */
    }

    /* EDIT code to implement state DS_Send_RD here:*/
}

void
aurp_fsm_DS_RI_Ack_Wait4 (at_fsm *fsm)
/*
| DS_RI_Ack_Wait4     | TIMEOUT2	| DS_Unconnected      |
| DS_RI_Ack_Wait4     | RI_ACK_DONE	| DS_Connected        |
*/
{
    at_aurp_info *aurp_info = (at_aurp_info *) fsm->userArg;

    if (ATFSM_TRACE_DEBUG(fsm))
	buginf("\nFSM '%s.%lx', state '%s'",
		fsm->name, fsm, fsm->stateStr(fsm->stateFunc));

    /* set timer when state first entered: */
    if (fsm->new_state) {
	fsm->new_state = FALSE;
	if (at_debug(atalkaurp_connection_debug, ATALK_INTERFACE_MASK,
		     aurp_info->idb->phys_idb))
	    buginf("\nAT: %s: Data Sender state changed -> RI_Ack_Wait4",
		   idb_get_namestring(aurp_info->idb->phys_idb));
	ATFSM_NO_TIMEOUT(fsm);	/* state has no TIMEOUT's */
	aurp_info->s_retry_count = 0;
	TIMER_START(aurp_info->s_retry_timer, NullRIIntervals[0] * ONESEC);
	aurp_Send_nullRI(aurp_info);
    }

    if (AWAKE(aurp_info->s_retry_timer)) {
	if (++aurp_info->s_retry_count
	    < sizeof(NullRIIntervals) / sizeof(ulong)) {
	    TIMER_START(aurp_info->s_retry_timer,
			NullRIIntervals[aurp_info->s_retry_count] * ONESEC);
	    aurp_Send_nullRI(aurp_info);
	} else {
	    atfsm_add_event(fsm, AURP_FSM_TIMEOUT2, 0);
	}
    }
}

void
aurp_fsm_DS_RI_Ack_Wait3 (at_fsm *fsm)
/*
| DS_RI_Ack_Wait3     | TIMEOUT2        | DS_Unconnected      | 
| DS_RI_Ack_Wait3     | RI_ACK_DONE     | DS_Unconnected      | 
*/
{
    at_aurp_info *aurp_info = (at_aurp_info *) fsm->userArg;

    if (ATFSM_TRACE_DEBUG(fsm))
	buginf("\nFSM '%s.%lx', state '%s'",
		fsm->name, fsm, fsm->stateStr(fsm->stateFunc));

    /* set timer when state first entered: */
    if (fsm->new_state) {
	fsm->new_state = FALSE;
	if (at_debug(atalkaurp_connection_debug, ATALK_INTERFACE_MASK,
		     aurp_info->idb->phys_idb))
	    buginf("\nAT: %s: Data Sender state changed -> RI_Ack_Wait3",
		   idb_get_namestring(aurp_info->idb->phys_idb));
	ATFSM_NO_TIMEOUT(fsm);	/* state has no TIMEOUT's */
	aurp_info->s_retry_count = 0;
	TIMER_START(aurp_info->s_retry_timer, RDIntervals[0] * ONESEC);
	aurp_Send_RD(aurp_info, aurp_info->s_error, FALSE);
    }

    if (AWAKE(aurp_info->s_retry_timer)) {
	if (++aurp_info->s_retry_count
	    < sizeof(RDIntervals) / sizeof(ulong)) {
	    TIMER_START(aurp_info->s_retry_timer,
			RDIntervals[aurp_info->s_retry_count] * ONESEC);
	    aurp_Send_RD(aurp_info, aurp_info->s_error, FALSE);
	} else {
	    atfsm_add_event(fsm, AURP_FSM_TIMEOUT2, 0);
	}
    }
}

void
aurp_fsm_initial (at_fsm *fsm)
/*
| initial             | DS_START        | DS_Unconnected      | 
| initial             | DR_START        | DR_Unconnected      | 
*/
{
    at_aurp_info *aurp_info = (at_aurp_info *) fsm->userArg;

    if (ATFSM_TRACE_DEBUG(fsm))
	buginf("\nFSM '%s.%lx', state '%s'",
		fsm->name, fsm, fsm->stateStr(fsm->stateFunc));

    /* set timer when state first entered: */
    if (fsm->new_state) {
	fsm->new_state = FALSE;
	ATFSM_NO_TIMEOUT(fsm);	/* state has no TIMEOUT's */
    }

    if (!interface_up(aurp_info->idb->phys_idb))
	return;

    if (fsm == &aurp_info->sender_fsm) {
#if 0
	ATFSM_SET_TRACE_DEBUG(fsm);
#endif
	atfsm_add_event(fsm, AURP_FSM_DS_START, 0);
    } else if (fsm == &aurp_info->receiver_fsm) {
#if 0
	ATFSM_SET_TRACE_DEBUG(fsm);
#endif
	atfsm_add_event(fsm, AURP_FSM_DR_START, 0);
    } else
	printf("\nError, cannot identify state machine");
}

void
aurp_fsm_halt (at_fsm *fsm)
/*
*/
{
    if (ATFSM_TRACE_DEBUG(fsm))
	buginf("\nFSM '%s.%lx', state '%s'",
		fsm->name, fsm, fsm->stateStr(fsm->stateFunc));

    /* set timer when state first entered: */
    if (fsm->new_state) {
	fsm->new_state = FALSE;
	ATFSM_NO_TIMEOUT(fsm);	/* state has no TIMEOUT's */
    }

    /* EDIT code to implement state halt here:*/
}

const static atfsm_table stab[] = {
    { aurp_fsm_DR_Unconnected, AURP_FSM_INITIATE, aurp_fsm_DR_Open_Rsp_Wait },
    { aurp_fsm_DR_Unconnected, AURP_FSM_DISCONNECT, aurp_fsm_DR_Unconnected },
    { aurp_fsm_DR_Open_Rsp_Wait, AURP_FSM_TIMEOUT2, aurp_fsm_DR_Unconnected },
    { aurp_fsm_DR_Open_Rsp_Wait, AURP_FSM_OPEN_RSPBAD, aurp_fsm_DR_Unconnected },
    { aurp_fsm_DR_Open_Rsp_Wait, AURP_FSM_OPEN_RSPGOOD, aurp_fsm_DR_Connected },
    { aurp_fsm_DR_Open_Rsp_Wait, AURP_FSM_DISCONNECT, aurp_fsm_DR_Unconnected },
    { aurp_fsm_DR_Connected, AURP_FSM_GET_RI, aurp_fsm_DR_RI_Rsp_Wait },
    { aurp_fsm_DR_Connected, AURP_FSM_TIMEOUTLHFT, aurp_fsm_DR_Tickle_Ack_Wait },
    { aurp_fsm_DR_Connected, AURP_FSM_RD, aurp_fsm_DR_Unconnected },
    { aurp_fsm_DR_Connected, AURP_FSM_DISCONNECT, aurp_fsm_DR_RI_Ack_Wait },
    { aurp_fsm_DR_Connected, AURP_FSM_RI_RSP, aurp_fsm_DR_Connected },
    { aurp_fsm_DR_RI_Rsp_Wait, AURP_FSM_TIMEOUT2, aurp_fsm_DR_Unconnected },
    { aurp_fsm_DR_RI_Rsp_Wait, AURP_FSM_RI_RSP, aurp_fsm_DR_Connected },
    { aurp_fsm_DR_RI_Rsp_Wait, AURP_FSM_RD, aurp_fsm_DR_Unconnected },
    { aurp_fsm_DR_RI_Rsp_Wait, AURP_FSM_DISCONNECT, aurp_fsm_DR_RI_Ack_Wait },
    { aurp_fsm_DR_Tickle_Ack_Wait, AURP_FSM_TIMEOUT2, aurp_fsm_DR_Unconnected },
    { aurp_fsm_DR_Tickle_Ack_Wait, AURP_FSM_RD, aurp_fsm_DR_Unconnected },
    { aurp_fsm_DR_Tickle_Ack_Wait, AURP_FSM_RI_RSP, aurp_fsm_DR_Connected },
    { aurp_fsm_DR_Tickle_Ack_Wait, AURP_FSM_DISCONNECT, aurp_fsm_DR_RI_Ack_Wait },
    { aurp_fsm_DR_Tickle_Ack_Wait, AURP_FSM_TICKLE_ACK, aurp_fsm_DR_Connected },
    { aurp_fsm_DR_RI_Ack_Wait, AURP_FSM_RI_ACK_DONE, aurp_fsm_DR_Unconnected },
    { aurp_fsm_DS_Unconnected, AURP_FSM_INITIATE, aurp_fsm_DS_Connected },
    { aurp_fsm_DS_Unconnected, AURP_FSM_DISCONNECT, aurp_fsm_DS_Unconnected },
    { aurp_fsm_DS_Connected, AURP_FSM_RI_REQ, aurp_fsm_DS_RI_Ack_Wait1 },
    { aurp_fsm_DS_Connected, AURP_FSM_TIMEOUTUPDATE, aurp_fsm_DS_RI_Ack_Wait2 },
    { aurp_fsm_DS_Connected, AURP_FSM_OPENREQBAD, aurp_fsm_DS_RI_Ack_Wait4 },
    { aurp_fsm_DS_Connected, AURP_FSM_RD, aurp_fsm_DS_Unconnected },
    { aurp_fsm_DS_Connected, AURP_FSM_DISCONNECT, aurp_fsm_DS_RI_Ack_Wait3 },
    { aurp_fsm_DS_RI_Ack_Wait1, AURP_FSM_TIMEOUT2, aurp_fsm_DS_Unconnected },
    { aurp_fsm_DS_RI_Ack_Wait1, AURP_FSM_RI_ACK_DONE, aurp_fsm_DS_Connected },

    { aurp_fsm_DS_RI_Ack_Wait1, AURP_FSM_DISCONNECT, aurp_fsm_DS_RI_Ack_Wait3 },
    { aurp_fsm_DS_RI_Ack_Wait2, AURP_FSM_TIMEOUT2, aurp_fsm_DS_Unconnected },
    { aurp_fsm_DS_RI_Ack_Wait2, AURP_FSM_RI_ACK_DONE, aurp_fsm_DS_Connected },
    { aurp_fsm_DS_RI_Ack_Wait2, AURP_FSM_DISCONNECT, aurp_fsm_DS_RI_Ack_Wait3 },
    { aurp_fsm_DS_RI_Ack_Wait3, AURP_FSM_TIMEOUT2, aurp_fsm_DS_Unconnected },
    { aurp_fsm_DS_RI_Ack_Wait3, AURP_FSM_RI_ACK_DONE, aurp_fsm_DS_Unconnected },
    { aurp_fsm_DS_RI_Ack_Wait3, AURP_FSM_DISCONNECT, aurp_fsm_DS_RI_Ack_Wait3 },
    { aurp_fsm_DS_RI_Ack_Wait4, AURP_FSM_TIMEOUT2, aurp_fsm_DS_Unconnected },
    { aurp_fsm_DS_RI_Ack_Wait4, AURP_FSM_RI_ACK_DONE, aurp_fsm_DS_Connected },
    { aurp_fsm_DS_RI_Ack_Wait4, AURP_FSM_DISCONNECT, aurp_fsm_DS_Unconnected },
    { aurp_fsm_initial, AURP_FSM_DS_START, aurp_fsm_DS_Unconnected },
    { aurp_fsm_initial, AURP_FSM_DR_START, aurp_fsm_DR_Unconnected },
    { aurp_fsm_initial, AURP_FSM_DISCONNECT, aurp_fsm_initial },
    { NILFSMFUNC, AURP_FSM_RESET, aurp_fsm_initial },
    { NILFSMFUNC, AURP_FSM_FSM_HALT, aurp_fsm_halt },
    { NILFSMFUNC, ATFSM_NILEVENT, NILFSMFUNC }
};


/*************************  EXPORTED FUNCTIONS  ***********************/

char *
aurp_eventStr(atfsmEvent ee)
{
    switch(ee) {
    /* special events common to all state machines: */
    case ATFSM_TIMEOUT:		return("ATFSM_TIMEOUT");
    case ATFSM_NILEVENT:	return("ATFSM_NILEVENT");
    case AURP_FSM_INITIATE:	return("AURP_FSM_INITIATE");
    case AURP_FSM_TIMEOUT2:	return("AURP_FSM_TIMEOUT2");
    case AURP_FSM_OPEN_RSPBAD:	return("AURP_FSM_OPEN_RSPBAD");
    case AURP_FSM_OPEN_RSPGOOD:	return("AURP_FSM_OPEN_RSPGOOD");
    case AURP_FSM_GET_RI:	return("AURP_FSM_GET_RI");
    case AURP_FSM_TIMEOUTLHFT:	return("AURP_FSM_TIMEOUTLHFT");
    case AURP_FSM_RD:	return("AURP_FSM_RD");
    case AURP_FSM_RI_RSP:	return("AURP_FSM_RI_RSP");
    case AURP_FSM_RI_REQ:	return("AURP_FSM_RI_REQ");
    case AURP_FSM_TIMEOUTUPDATE:	return("AURP_FSM_TIMEOUTUPDATE");
    case AURP_FSM_TICKLE_ACK:	return("AURP_FSM_TICKLE_ACK");
    case AURP_FSM_OPENREQBAD:	return("AURP_FSM_OPENREQBAD");
    case AURP_FSM_SEND_RD:	return("AURP_FSM_SEND_RD");
    case AURP_FSM_RI_ACK_DONE:	return("AURP_FSM_RI_ACK_DONE");
    case AURP_FSM_DS_START:	return("AURP_FSM_DS_START");
    case AURP_FSM_DR_START:	return("AURP_FSM_DR_START");
    case AURP_FSM_FSM_HALT:	return("AURP_FSM_FSM_HALT");
    case AURP_FSM_DISCONNECT:   return("AURP_FSM_DISCONNECT");
    case AURP_FSM_RESET:	return("AURP_FSM_RESET");
    default:	return("");
    }
}

char *
aurp_stateStr(FSMFUNC func)
{
    if (func == aurp_fsm_DR_Unconnected)
	return("aurp_fsm_DR_Unconnected");
    else if (func == aurp_fsm_DR_Open_Rsp_Wait)
	return("aurp_fsm_DR_Open_Rsp_Wait");
    else if (func == aurp_fsm_DR_Connected)
	return("aurp_fsm_DR_Connected");
    else if (func == aurp_fsm_DR_RI_Rsp_Wait)
	return("aurp_fsm_DR_RI_Rsp_Wait");
    else if (func == aurp_fsm_DR_Tickle_Ack_Wait)
	return("aurp_fsm_DR_Tickle_Ack_Wait");
    else if (func == aurp_fsm_DR_RI_Ack_Wait)
	return("aurp_fsm_DR_RI_Ack_Wait");
    else if (func == aurp_fsm_DS_Unconnected)
	return("aurp_fsm_DS_Unconnected");
    else if (func == aurp_fsm_DS_Connected)
	return("aurp_fsm_DS_Connected");
    else if (func == aurp_fsm_DS_RI_Ack_Wait1)
	return("aurp_fsm_DS_RI_Ack_Wait1");
    else if (func == aurp_fsm_DS_RI_Ack_Wait2)
	return("aurp_fsm_DS_RI_Ack_Wait2");
    else if (func == aurp_fsm_DS_Send_RD)
	return("aurp_fsm_DS_Send_RD");
    else if (func == aurp_fsm_DS_RI_Ack_Wait4)
	return("aurp_fsm_DS_RI_Ack_Wait4");
    else if (func == aurp_fsm_DS_RI_Ack_Wait3)
	return("aurp_fsm_DS_RI_Ack_Wait3");
    else if (func == aurp_fsm_initial)
	return("aurp_fsm_initial");
    else if (func == aurp_fsm_halt)
	return("aurp_fsm_halt");
    else if (func == NILFSMFUNC)
	return("*");
    else    
	return("aurp_stateStr:  bad arg");
}

char const *
aurp_showstatusStr(FSMFUNC func)
{
    if (func == aurp_fsm_DR_Unconnected)
	return("Unconnected");
    else if (func == aurp_fsm_DR_Open_Rsp_Wait)
	return("Waiting for Open Response");
    else if (func == aurp_fsm_DR_Connected)
	return("Connected");
    else if (func == aurp_fsm_DR_RI_Rsp_Wait)
	return("Waiting for Routing Information Response");
    else if (func == aurp_fsm_DR_Tickle_Ack_Wait)
	return("Waiting for Tickle Acknowledgement");
    else if (func == aurp_fsm_DR_RI_Ack_Wait)
	return("Waiting for Routing Information Acknowledgement");
    else if (func == aurp_fsm_DS_Unconnected)
	return("Unconnected");
    else if (func == aurp_fsm_DS_Connected)
	return("Connected");
    else if (func == aurp_fsm_DS_RI_Ack_Wait1)
	return("Waiting for Routing Information Acknowledgement");
    else if (func == aurp_fsm_DS_RI_Ack_Wait2)
	return("Waiting for Routing Information Acknowledgement");
    else if (func == aurp_fsm_DS_Send_RD)
	return("aurp_fsm_DS_Send_RD");
    else if (func == aurp_fsm_DS_RI_Ack_Wait4)
	return("Waiting for Routing Information Acknowledgement");
    else if (func == aurp_fsm_DS_RI_Ack_Wait3)
	return("Waiting for Routing Information Acknowledgement");
    else if (func == aurp_fsm_initial)
	return("Initializing");
    else if (func == aurp_fsm_halt)
	return("Halted");
    else if (func == NILFSMFUNC)
	return("Unknown");
    else    
	return("aurp_showstatusStr:  bad arg");
}

void
aurp_fsm_init (at_fsm *fsm, char *uarg)
/* initialize a aurp FSM */
{
    queue_init(&fsm->stateQ, 0);	/* init state event queue */
    fsm->name = FSM_NAME;		/* init state machine name */
    fsm->traceInfo = FALSE;		/* state transitions only */
    fsm->traceDebug = FALSE;		/* verbose trace */
    fsm->timeout = FALSE;		/* TRUE if TIMEOUT event */
    fsm->new_state = TRUE;		/* This is a new state */
    TIMER_STOP(fsm->timer);		/* timer */
    fsm->delay = 0;			/* delay before TIMEOUT */
    fsm->stab = stab;			/* pointer to state table */
    fsm->userArg = uarg;		/* user arg. passed thru fsm */
    fsm->initialState = aurp_fsm_initial;	/* INITIAL state */
    fsm->haltState = aurp_fsm_halt;		/* HALT state */
    fsm->stateFunc = aurp_fsm_initial;		/* current state routine */
    fsm->prevStateFunc = aurp_fsm_initial;	/* for "any state" events */
    fsm->stateStr = aurp_stateStr;	/* returns name of state func */
    fsm->eventStr = aurp_eventStr;	/* returns name of fsm event */
}

