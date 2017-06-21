/* $Id: fr_svc_l3_sdl_u.c,v 3.1.64.5 1996/07/29 22:11:34 snyder Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_svc_l3_sdl_u.c,v $
 *------------------------------------------------------------------
 * fr_svc_l3_sdl_u.c : Layer 3 State Change handling
 *
 * May 1995, Shankar Natarajan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fr_svc_l3_sdl_u.c,v $
 * Revision 3.1.64.5  1996/07/29  22:11:34  snyder
 * CSCdi64472:  declare some arrays const in shr_frame, shr_frame_svc
 *              2380 out of data, 40 image
 * Branch: California_branch
 *
 * Revision 3.1.64.4  1996/06/12  14:09:14  shankar
 * CSCdi57333:  FR SVC Timer handler needs SVC event intelligence
 * Branch: California_branch
 *
 * Revision 3.1.64.3  1996/05/31  00:59:31  ccpoe
 * CSCdi58460:  map-class frame-relay traffic-shaping features dont work
 * on FR SVC
 * Branch: California_branch
 *
 * Revision 3.1.64.2  1996/05/24  21:21:33  ccpoe
 * CSCdi58460:  map-class frame-relay traffic-shaping features dont work
 * on FR SVC
 * Branch: California_branch
 *
 * Revision 3.1.64.1  1996/04/25  23:23:44  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.68.1  1996/04/24  04:11:30  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.1  1995/11/09  13:49:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/01  19:53:34  shankar
 * Files for Arkansas feature
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "passive_timers.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "frame_relay.h"
#include "fr_debug.h"
#include "../parser/parser_defs_exec.h"
#include "../wan/parser_defs_frame.h"
#include "../wan/fr_svc.h"
#include "../wan/fr_svc_registry.h"

/* 
 * Transitions from the Null FRU0 State
 */
const static StateTable StateTable_00[] = {
	{MSG_SETUP,		FRU0_Setup},
	{MSG_CALL_PROCEEDING,	FRU0_Unexpected},
	{MSG_CONNECT,		FRU0_Unexpected},
	{MSG_DISCONNECT,	FRU0_Unexpected},
	{MSG_RELEASE,		FRU0_Release},
	{MSG_RELEASE_COMPLETE,	FRU0_ReleaseComplete},
	{MSG_STATUS_ENQUIRY,	FRU0_Unexpected},
	{MSG_STATUS,		FRU0_Status},
	{RELEASE_REQ,		FRU0_Unexpected},
	{SETUP_REQUEST,		FRU0_SetupRequest}, 
	{MSG_UNKNOWN, 		FRU0_Unknown_Msg}
};

/* 
 * Transitions from the Call Initiated FRU1 State
 * This state is recahed when an internal request for a 
 * call has been received and a SETUP has gone out.
 */
const static StateTable StateTable_01[] = {
	{MSG_SETUP,		FRUx_Setup}, 
	{MSG_CALL_PROCEEDING,	FRU1_CallProceeding},
	{MSG_CONNECT,		FRU1_Connect},
	{MSG_DISCONNECT,	FRU1_Disconnect},		
	{MSG_RELEASE,		FRU1_Release},
	{MSG_RELEASE_COMPLETE,	FRU1_ReleaseComplete},
	{MSG_STATUS_ENQUIRY,	FRUx_StatusEnquiry},
	{MSG_STATUS,		FRUx_Status},
	{DISCONNECT_REQ,	FRU1_DisconnectReq},
	{RELEASE_REQ,		FRUx_ReleaseRequest},
	{SPL_RELEASE_REQ,	FRUx_SplReleaseRequest},
	{MSG_UNKNOWN, 		FRUx_Unknown_Msg}
};

/* 
 * Transitions from the Outgoing Call Proceeding FRU3 State
 * Indication from the network that all information necessary
 * to process the call has been received and that no more
 * call setup information will be accepted.
 */
const static StateTable StateTable_03[] = {
	{MSG_SETUP,		FRUx_Setup},
	{MSG_CALL_PROCEEDING,	FRUx_Unexpected},
	{MSG_CONNECT,		FRU3_Connect},
	{MSG_DISCONNECT,	FRUx_Disconnect},
	{MSG_RELEASE,		FRUx_Release}, 
	{MSG_RELEASE_COMPLETE,	FRUx_ReleaseComplete},
	{MSG_STATUS_ENQUIRY,	FRUx_StatusEnquiry},
	{MSG_STATUS,		FRUx_Status},
	{RELEASE_REQ,		FRU3_ReleaseRequest},
	{SPL_RELEASE_REQ,	FRUx_SplReleaseRequest},
	{TIDL_TIMEOUT, 		FRUx_Idletoolong}, 
	{MSG_UNKNOWN, 		FRUx_Unknown_Msg}
};

/* 
 * Transitions from the Call Present U5 state.
 * Q931 calls this state 6.
 * User has received a call establish request but has not 
 * responded yet. 
 */
const static StateTable StateTable_06[] = {
	{MSG_SETUP,		FRUx_Setup},
	{MSG_CALL_PROCEEDING,	FRUx_Unexpected},
	{MSG_CONNECT,		FRUx_Unexpected},
	{MSG_DISCONNECT,	FRU6_Disconnect},
	{MSG_RELEASE,		FRU6_Release},
	{MSG_RELEASE_COMPLETE,	FRUx_ReleaseComplete},
	{MSG_STATUS_ENQUIRY,	FRUx_StatusEnquiry},
	{MSG_STATUS,		FRUx_Status},
	{REJECT_REQUEST,	FRUx_Unexpected},
	{RELEASE_REQ,		FRUx_ReleaseRequest},
	{SPL_RELEASE_REQ,	FRUx_SplReleaseRequest},
	{TIDL_TIMEOUT, 		FRUx_Idletoolong}, 
	{MSG_UNKNOWN, 		FRUx_Unknown_Msg}
};

/* 
 * Transitions from Incoming Call Proceeding U9 state
 * State for an incoming call when the user has sent 
 * acknowledgement that the user has received all call
 * information necessary to effect call establishment.
 */
const static StateTable StateTable_09[] = {
	{MSG_SETUP,		FRUx_Setup},
	{MSG_CALL_PROCEEDING,	FRUx_Unexpected},
	{MSG_CONNECT,		FRUx_Unexpected},
	{MSG_DISCONNECT,	FRUx_Disconnect},
	{MSG_RELEASE,		FRUx_Release},
	{MSG_RELEASE_COMPLETE,	FRUx_ReleaseComplete},
	{MSG_STATUS_ENQUIRY,	FRUx_StatusEnquiry},
	{MSG_STATUS,		FRUx_Status},
	{SETUP_RESPONSE,	FRUx_Unexpected},
	{RELEASE_REQ,		FRUx_ReleaseRequest},
	{SPL_RELEASE_REQ,	FRUx_SplReleaseRequest},
	{TIDL_TIMEOUT, 		FRUx_Idletoolong}, 
	{MSG_UNKNOWN, 		FRUx_Unknown_Msg}
};

/* 
 * Transitions from the Call Active FRU10 state.
 * This state exists either when the user has been 
 * awarded the call.
 */
const static StateTable StateTable_10[] = {
	{MSG_SETUP,		FRUx_Unexpected},
	{MSG_CALL_PROCEEDING,	FRUx_Unexpected},
	{MSG_CONNECT,		FRUx_Unexpected},
	{MSG_DISCONNECT,	FRUx_Disconnect},
	{MSG_RELEASE,		FRUx_Release},
	{MSG_RELEASE_COMPLETE,	FRUx_ReleaseComplete},
	{MSG_STATUS_ENQUIRY,	FRUx_StatusEnquiry},
	{MSG_STATUS,		FRUx_Status},
	{RELEASE_REQ,		FRUx_ReleaseRequest},
	{SPL_RELEASE_REQ,	FRUx_SplReleaseRequest},
	{TIDL_TIMEOUT, 		FRUx_Idletoolong}, 
	{MSG_UNKNOWN, 		FRUx_Unknown_Msg}
};

/* 
 * Transitions from the Disconnect Request FRU11 state.
 * User has asked the network to clear the call.
 */
const static StateTable StateTable_11[] = {
	{MSG_SETUP,		FRUx_Unexpected},
	{MSG_CALL_PROCEEDING,	FRUx_Unexpected},
	{MSG_CONNECT,		FRUx_Unexpected},
	{MSG_DISCONNECT,	FRU11_Disconnect},
	{MSG_RELEASE,		FRU11_Release},
	{MSG_RELEASE_COMPLETE,	FRUx_ReleaseComplete},
	{MSG_STATUS_ENQUIRY,	FRUx_StatusEnquiry},
	{MSG_STATUS,		FRUx_Status},
	{RELEASE_REQ,		FRUx_ReleaseRequest},
	{SPL_RELEASE_REQ,	FRUx_SplReleaseRequest},
	{TIDL_TIMEOUT, 		FRUx_Idletoolong}, 
	{MSG_UNKNOWN, 		FRUx_Unknown_Msg}
};

/* 
 * Transitions from the Disconnect Indication FRU12 state.
 * Network has asked the user to disconnect the call.
 */
const static StateTable StateTable_12[] = {
	{MSG_SETUP,		FRUx_Unexpected},
	{MSG_CALL_PROCEEDING,	FRUx_Unexpected},
	{MSG_CONNECT,		FRUx_Unexpected},
	{MSG_DISCONNECT,	FRUx_Unexpected},
	{MSG_RELEASE,		FRU12_Release},
	{MSG_RELEASE_COMPLETE,	FRUx_ReleaseComplete},
	{MSG_STATUS_ENQUIRY,	FRUx_StatusEnquiry},
	{MSG_STATUS,		FRUx_Status},
	{RELEASE_REQ,		FRU12_ReleaseRequest},
	{SPL_RELEASE_REQ,	FRUx_SplReleaseRequest},
	{TIDL_TIMEOUT, 		FRUx_Idletoolong}, 
	{MSG_UNKNOWN, 		FRUx_Unknown_Msg}
};

/* 
 * Transitions from the Release Request FRU19 state.
 * User has asked the network to release the call 
 * and is waiting for a response.
 */
const static StateTable StateTable_19[] = {
	{MSG_SETUP,		FRUx_Unexpected},
	{MSG_CALL_PROCEEDING,	FRUx_Unexpected},
	{MSG_CONNECT,		FRUx_Unexpected},
	{MSG_DISCONNECT,	FRUx_Unexpected}, 
	{MSG_RELEASE,		FRU19_Release},
	{MSG_RELEASE_COMPLETE,	FRU19_ReleaseComplete},
	{MSG_STATUS_ENQUIRY,	FRUx_StatusEnquiry},
	{MSG_STATUS,		FRU19_Status},
	{TIDL_TIMEOUT, 		FRUx_Idletoolong}, 
	{MSG_UNKNOWN, 		FRUx_Unknown_Msg}
};


/* 
 * State transition function. For any existing state, 
 * handle a particular message type by indexing into a 
 * table of current states. 
 */
const static StateTable *StateTableFunc[] = {
	StateTable_00, 			/* NULL */
	StateTable_01,			/* Call Initiated */
	0,
	StateTable_03, 			/* Call Proceeding */
	0,
	0,
	StateTable_06,			/* Call Present */
	0,
	0,
	StateTable_09, 			/* Incoming call proc */
	StateTable_10, 			/* Call Active */
	StateTable_11, 			/* Disconnect Request */
	StateTable_12,			/* Disconnect Indication */
	0,
	0,
	0,
	0,
	0,
	0,
	StateTable_19,			/* Release Request */
	NULL				/* End of table */
};

/* 
 * svc_process_l3_event
 * Routine that will handle the dispatching of control to a 
 * relevant function. The state changes will be handled within 
 * each called function. 
 * 
 * svc_process_l3_event can be called in both input and output directions.
 * In the output direction, "params" is derived from the configured
 * maps. In the input direction, "params" is derived from the svc
 * structure. The configured values need not be the same as the 
 * final values.
 * 
 * In a similar fashion, "event" could either be an internal parameter
 * or it could be a layer 3 message. The svc structure does not know
 * about the internal events. 
 *
 * Because of the above reasons,"params" and "event" are always passed
 * as explicit parameters to other underlying functions.
 *
 * Call_id is another parameter that has dual meaning. On the incoming 
 * side it'll hold a call reference, while on the outgoing side, it 
 * holds a call id.
 */
boolean svc_process_l3_event(
	paktype *pak,
	fr_svc_info_type *svc,
	uint call_id, 
	uint event, 
	struct fr_svc_params *svc_params,
	uint direction)
{
    const StateTable 		*state;
    int			iy;
    hwidbtype 		*hwidb;

    iy = 0;			/* Only used for debugging */

    /* 
     * There are times when "IN" direction is used on outgoing packets. 
     * This happens in cases when an incoming call request is 
     * rejected for reasons of misconfiguration. In such a case,
     * only if_input is known. While if_output is the same as 
     * if_input in such cases, I work with just the "IN" flag.
     */
    if (direction == IN)
	hwidb = pak->if_input->hwptr;
    else 
	hwidb = pak->if_output->hwptr;

    if (!hwidb)
	return(FALSE);

    /* 
     * When we come here, we may not have an SVC structure assigned. 
     * A new call, for instance. In which case, an svc structure needs 
     * to be allocated. 
     */
    if (!svc) {
	svc = fr_assign_svc_mem();
	if (!svc)
	    return(FALSE);
    	svc->call_state = STATE_NULL;
    	svc->params.call_ref = fr_svc_select_next_call_ref(hwidb);
	if (!call_id)
    	    svc->call_id = fr_svc_select_next_call_id(hwidb);
	else 
	    svc->call_id = call_id; 
    	add_fr_svc_to_idb_queue(hwidb, svc);

	/* 
	 * In the case of a new call, we must move some configuration
	 * information from the map class into the svc structure. 
	 * These values are then negotiated through the l3 procedures.
	 */
	memcpy((uchar *)&svc->params.svc_addr,
			(uchar *)&svc_params->svc_addr, 
			sizeof(svc_addr_block));
    }
    state = StateTableFunc[svc->call_state];

    /* 
     * Bogusness test. Should never happen.
     */
    if (!state) {
	buginf("\nInvalid Function selected, %d", svc->call_state); 
	return(FALSE);
    }

    /* 
     * Walk the event table. If we reach MSG_UNKNOWN, there was no match.
     * Such an event should never happen. If there is a match, go execute.
     */
    while (TRUE) {
	/* 
	 * debug statement will not live after product matures. 
	 */
	if (fr_svc_detd_debug)
	    buginf("\nsvc_process_l3_event:Walking table i %d CS %d",
					iy++, svc->call_state); 
	if (state->message == event) {
	    break;
	} else {
	    if (state->message == MSG_UNKNOWN) {
		if (fr_svc_detd_debug)
		    buginf("\nL3 Event ERROR: Unknown message is 0x%x", event);
		return(FALSE);
	    } else
		state++;
	}
    }
    (state->EventFunc)(hwidb, svc, pak, call_id, event, svc_params);

return(TRUE);
}

/* 
 * FRU0_Setup
 * Page 197, Figure A.3, of the Q.931. 3/93
 * Incoming SETUP message. svc must be already allocated by now. 
 * If it did not already exist, it would have been created by the 
 * ie_parse routines. Not only do we move into the CALL_PRESENT state 
 * but we also move into the CALL_ACTIVE state if all goes well. 
 */
void FRU0_Setup (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint		init_state;
    boolean 		ret;

    if (L3_DBG)
	buginf("\nFRU0_Setup : %s", hwidb->hw_namestring);

    if (!svc)
	return;

    init_state = svc->call_state;

    svc->call_state = STATE_CALL_PRESENT;

    if (L3_DBG)
	buginf("\nL3SDL : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
	call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));

    ret = fr_svc_send_msg_to_nli(hwidb, pak, svc, NL_CALL_IND);

    /* 
     * We do basic sanity checking when a message is sent to the NLI. 
     * Check maps to see whether a call is legal or not. If the NLI 
     * rejects the call, the layer 3 call control must also reject.
     */
    if (ret) {
    	fr_svc_send_msg(hwidb, svc, MSG_CALL_PROCEEDING, svc->params.call_ref, svc_params);
    } else  {
	/* 
	 * We may have failed call setup. Don't do anything to clear the 
	 * call. The other side will send us a RELEASE shortly.
	 * This failure should not happen if the configurations are okay.
	 */
	return;
    }
    ret = fr_svc_send_msg_to_nli(hwidb, pak, svc, NL_CALL_CNF);

    /* 
     * We do basic sanity checking when a message is sent to the NLI. 
     * Check maps to see whether a call is legal or not. If the NLI 
     * rejects the call, the layer 3 call control must also reject.
     */
    if (ret) {
    	init_state = svc->call_state;
	svc->call_state = STATE_INCOMING_CALL_PROC;
    	if (L3_DBG)
	    buginf("\nL3SDL : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
	    call_ref, l3cc_st(init_state), l3cc_ev(event),  
					l3cc_st(svc->call_state));
    	fr_svc_send_msg(hwidb, svc, MSG_CONNECT, svc->params.call_ref,
			svc_params);
    } else {
	/* 
	 * We may have failed call setup. Don't do anything to clear the 
	 * call. The other side will send us a RELEASE shortly.
	 * This failure should not happen if the configurations are okay.
	 */
	return;
    }
    /* 
     * NLI has accepted the call. Move to the ACTIVE state. 
     */
    init_state = svc->call_state;
    svc->call_state = STATE_ACTIVE;
    if (L3_DBG)
	buginf("\nL3SDL : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
	call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));
}
/* 
 * FRU0_Unexpected
 * Unexpected message for the NULL call state. Page 198, Q.931, A.3, Sheet 2.
 * Send out a RELEASE_COMPLETE and move to the NULL state
 */
void FRU0_Unexpected (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint		init_state;
    boolean 		ret;

    if (L3_DBG)
	buginf("\nFRU0_Unexpected : %s", hwidb->hw_namestring);

    init_state = svc->call_state;

    ret = fr_svc_send_msg_to_nli(hwidb, pak, svc, NL_REL_CNF);

    /* 
     * Tear the call down whatever the value of "ret" above.
     * cause value is INVALID_CALL_REF_VALUE (81).
     */
    svc->params.cause = INVALID_CALL_REF_VALUE;
    fr_svc_send_msg(hwidb, svc, MSG_RELEASE_COMPLETE, svc->params.call_ref,
		    svc_params);
    svc->call_state = STATE_NULL;

    if (L3_DBG)
	buginf("\nL3SDL UNEXPECTED : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
	call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));

    rem_fr_svc_from_idb_queue(hwidb, svc);
    free(svc);
}
/* 
 * FRU0_ReleaseComplete
 * To prevent endless loops, treat the RELEASE message 
 * separately from the RELEASE COMPLETE message. This routine is
 * for RELEASE COMPLETE. Ref: Q.931, 3/93, A.3, Sheet 2.
 */
void FRU0_ReleaseComplete (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint		init_state;
    boolean 		ret;

    if (L3_DBG)
	buginf("\nFRU0_ReleaseComplete : %s", hwidb->hw_namestring);

    init_state = svc->call_state;

    /* 
     * In the NULL state, we received a RELEASE COMPLETE. Nothing to do
     * but stay in the NULL state. Release all allocated NLI and L3 resources.
     */
	
    ret = fr_svc_send_msg_to_nli(hwidb, pak, svc, NL_REL_CNF);

    svc->call_state = STATE_NULL;

    if (L3_DBG)
	buginf("\nL3SDL FRU0_ReleaseComplete: Ref: %d, Init : %s, Rcvd: %s, Next : %s",
	call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));

    rem_fr_svc_from_idb_queue(hwidb, svc);
    free(svc);
}

/* 
 * FRU0_Release
 * In the NULL state, we received a RELEASE message. Send out a 
 * RELEASE COMPLETE and move to the NULL state. 
 * Ref: Q.931, 3/93, pg 198.
 */
void FRU0_Release (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint		init_state;
    boolean 		ret;

    if (L3_DBG)
	buginf("\nFRU0_Release : %s", hwidb->hw_namestring);

    init_state = svc->call_state;

    /* 
     * Release all allocated NLI and L3 resources since call is being 
     * torn down.
     */
    ret = fr_svc_send_msg_to_nli(hwidb, pak, svc, NL_REL_IND);

    /* 
     * Cause value is NORMAL_CALL_CLEARING (16). 
     */
    svc->params.cause = NORMAL_CALL_CLEARING;
    fr_svc_send_msg(hwidb, svc, MSG_RELEASE_COMPLETE, svc->params.call_ref,
		    svc_params);

    /* 
     * Release all allocated NLI and L3 resources since call is being 
     * torn down.
     */
    ret = fr_svc_send_msg_to_nli(hwidb, pak, svc, NL_REL_CNF);

    svc->call_state = STATE_NULL;

    if (L3_DBG)
	buginf("\nL3SDL FRU0_Release: Ref: %d, Init : %s, Rcvd: %s, Next : %s",
	call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));

    rem_fr_svc_from_idb_queue(hwidb, svc);
    free(svc);
}
/* 
 * FRU0_Status
 * Call state is NULL, Incoming STATUS message, Final state is NULL.
 * The status message has a call state IE. If the call state in the IE is not 
 * NULL, we have a call state mismatch. Send a RELEASE COMPLETE. A.3, Sheet 1.
 * If the call state in the incoming message is NULL, all is well.
 */
void FRU0_Status (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint		init_state;
    boolean 		ret;

    if (L3_DBG)
	buginf("\nFRU0_Status : %s", hwidb->hw_namestring);

    init_state = svc->call_state;

    if (svc->call_state_inc_msg != STATE_NULL) {
    	/* 
     	 * By virtue of being within this function, we think that we are in 
     	 * the NULL state but the switch is telling us otherwise. 
     	 */
    	svc->params.cause = MSG_NOT_COMPATIBLE_CALL_STATE;
    	fr_svc_send_msg(hwidb, svc, MSG_RELEASE_COMPLETE,
			svc->params.call_ref, svc_params);

    	ret = fr_svc_send_msg_to_nli(hwidb, pak, svc, NL_REL_CNF);

    	svc->call_state = STATE_NULL;

    	if (L3_DBG)
	buginf("\nL3SDL FRU0_Status : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
	call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));

    	rem_fr_svc_from_idb_queue(hwidb, svc);
    	free(svc);
    } else {
	/* 
	 * We are in the NULL state and the switch knows so. Duh!
	 * Lets force ourselves into the NULL state anyhow.
	 */
    	svc->call_state = STATE_NULL;

    	if (L3_DBG)
	buginf("\nL3SDL FRU0_Status : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
	call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));
    }

} 

/* 
 * FRU0_SetupRequest
 * We are getting a internal request to setup the call.
 * Assign a non-zero call reference and send out a setup message.
 * Ref: Q.931, 3/93, pg. 197.
 */
void FRU0_SetupRequest (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint		init_state;

    if (L3_DBG)
	buginf("\nFRU0_SetupRequest : %s", hwidb->hw_namestring);

    if (!call_ref)
    	call_ref = fr_svc_select_next_call_ref(hwidb);

    /* 
     * Sanity check. Should never happen. 
     */
    if (!call_ref) {
	return;
    }

    init_state = svc->call_state;
    fr_svc_send_msg(hwidb, svc, MSG_SETUP, svc->params.call_ref, svc_params);
    TIMER_START(svc->svc_t303, T303_DEFAULT);
    process_set_boolean(fr_svc_timer_bool, TRUE);
    svc->call_state = STATE_CALL_INITIATED;

    if (L3_DBG)
    	buginf("\nL3SDL : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
    	call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));
}
/* 
 * FRU0_Unknown_Msg
 * For State NULL, Unknown messages need special treatment.
 * Unknown message. See page 198, Q.931, A.3, Sheet 2.
 * Send out a RELEASE COMPLETE and go to the null state.
 */
void FRU0_Unknown_Msg (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint		init_state;

    if (L3_DBG)
	buginf("\nFRU0_Unknown_Msg : %s", hwidb->hw_namestring);

    init_state = svc->call_state;
    svc->params.cause = INVALID_CALL_REF_VALUE;
    fr_svc_send_msg(hwidb, svc, MSG_RELEASE_COMPLETE, svc->params.call_ref,
		    svc_params);
    svc->call_state = STATE_NULL;

    if (L3_DBG)
	buginf("\nL3SDL UNEXPECTED : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
	call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));

    rem_fr_svc_from_idb_queue(hwidb, svc);
    free(svc);
}
/* 
 * FRUx_Unknown_Msg
 * Unknown message. Should never happen. See page 216, Q.931, A.3, Sheet 20.
 * Send out a STATUS ENQUIRY and remain in the same state.
 */
void FRUx_Unknown_Msg (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint		init_state;

    if (L3_DBG)
	buginf("\nFRUx_Unknown_Msg : %s", hwidb->hw_namestring);

    init_state = svc->call_state;

    if (L3_DBG)
    	buginf("\nL3SDL FRUx_Unknown_Msg : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
    	call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));

    fr_svc_send_msg(hwidb, svc, MSG_STATUS_ENQUIRY, svc->params.call_ref,
		    svc_params);

}
/* 
 * FRUx_Unexpected
 * Unexpected message for this call state. 
 * As per A.3, Sheet  20 of Q.931, for unexpected and unrecognized 
 * messages, send out either a STATUS or a STATUS ENQUIRY.
 * Make no state change. 
 */
void FRUx_Unexpected (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint		init_state;

    if (L3_DBG)
	buginf("\nFRUx_Unexpected : %s", hwidb->hw_namestring);

    init_state = svc->call_state;

    /* 
     * As per Q.931, pg. 216, the cause can be either MESSAGE_INCOMPATIBLE_WITH_CALL
     * (98) or MSG_NOT_COMPATIBLE_CALL_STATE (101). 
     */
    svc->params.cause = MESSAGE_INCOMPATIBLE_WITH_CALL;
    fr_svc_send_msg(hwidb, svc, MSG_STATUS, call_ref, svc_params);

    /* 
     * remain in the current state
     */

    if (L3_DBG)
        buginf("\nL3SDL FRUx_Unexpected: Ref: %d, Init : %s, Rcvd: %s, Next : %s",
        call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));
}
/* 
 * FRUx_Setup
 * A SETUP message can only be received in the NULL state. 
 * If we are in any other state, remain in that state. 
 * Ignore the SETUP message. A.3, Sheet 20, Q.931, 03/93.
 */
void FRUx_Setup (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint		init_state;

    if (L3_DBG)
	buginf("\nFRUx_Setup : %s", hwidb->hw_namestring);

    init_state = svc->call_state;

    if (L3_DBG)
        buginf("\nL3SDL : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
        call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));
}
/* 
 * FRU1_CallProceeding
 * When in the STATE_CALL_INITIATED state, we received a MSG_CALL_PROCEEDING. 
 * As per A.3, Sheet 4, of Q.931, we need to stop T303, start T310 and move into 
 * the STATE_CALL_PROCEEDING state. 
 */
void FRU1_CallProceeding (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint		init_state;

    if (L3_DBG)
	buginf("\nFRU1_CallProceeding : %s", hwidb->hw_namestring);

    init_state = svc->call_state;

    TIMER_STOP(svc->svc_t303);

    TIMER_START(svc->svc_t310, T310_DEFAULT);
    process_set_boolean(fr_svc_timer_bool, TRUE);

    svc->call_state = STATE_CALL_PROCEEDING;

    if (L3_DBG)
        buginf("\nL3SDL : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
        call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));
}
/* 
 * FRU1_Connect
 * When in the STATE_CALL_INITIATED state, we received a MSG_CONNECT. 
 * Put the call in the active state and stop T303. A.3, Sheet 4, Q.931 3/93.
 */
void FRU1_Connect (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint		init_state;
    boolean 		ret;

    if (L3_DBG)
	buginf("\nFRU1_Connect : %s", hwidb->hw_namestring);

    init_state = svc->call_state;

    TIMER_STOP(svc->svc_t303);

    ret = fr_svc_send_msg_to_nli(hwidb, pak, svc, NL_CALL_CNF);

    svc->call_state = STATE_ACTIVE;

    if (L3_DBG)
    	buginf("\nL3SDL FRU1_Connect : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
       call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));
}
/* 
 * FRU1_Disconnect
 * When in the STATE_CALL_INITIATED state, we received a MSG_DISCONNECT.
 * send a RELEASE message to the switch and move to the NULL state. 
 * Not documented anywhere. I made this one up. 
 */
void FRU1_Disconnect (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint		init_state;

    if (L3_DBG)
	buginf("\nFRU1_Disconnect : %s", hwidb->hw_namestring);

    init_state = svc->call_state;
    fr_svc_send_msg(hwidb, svc, MSG_RELEASE, call_ref, svc_params);

    /* 
     * Don't bother freeing any resources now. We'll, hopefully,
     * get a RELEASE COMPLETE in response to the RELEASE. That'll
     * free up the NLI and L3 resources. 
     */

    if (L3_DBG)
    	buginf("\nL3SDL FRU1_Disconnect : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
       call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));
}
/* 
 * FRU1_Release
 * When in the STATE_CALL_INITIATED state, we received a MSG_RELEASE. 
 * send back a RELEASE COMPLETE and disconnect the call. 
 * Not documented anywhere. I made this one up. 
 */
void FRU1_Release (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint		init_state;
    boolean 		ret;

    if (L3_DBG)
	buginf("\nFRU1_Release : %s", hwidb->hw_namestring);

    init_state = svc->call_state;

    TIMER_STOP(svc->svc_t303);
    TIMER_STOP(svc->svc_t305);
    TIMER_STOP(svc->svc_t308);
    TIMER_STOP(svc->svc_t310);
    TIMER_STOP(svc->svc_t322);

    if (L3_DBG)
	buginf("\nL3SDL FRU1_Release : Ref: %d, Init : %d, Rcvd: %d, Next : %d",
       call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));

    ret = fr_svc_send_msg_to_nli(hwidb, pak, svc, NL_REL_CNF);

    svc->params.cause = NORMAL_CALL_CLEARING;
    fr_svc_send_msg(hwidb, svc, MSG_RELEASE_COMPLETE, call_ref, svc_params);

    svc->call_state = STATE_NULL;

    rem_fr_svc_from_idb_queue(hwidb, svc);
    free(svc);
}
/* 
 * FRU1_ReleaseComplete
 * When in the STATE_CALL_INITIATED state, we received a MSG_RELEASE_COMPLETE.
 * Stop T303, go back to the NULL state. A.3, Sheet 3, Q.931, 03/93.
 */
void FRU1_ReleaseComplete (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint		init_state;
    boolean 		ret;

    if (L3_DBG)
	buginf("\nFRU1_ReleaseComplete : %s", hwidb->hw_namestring);

    init_state = svc->call_state;
    svc->call_state = STATE_NULL;
    TIMER_STOP(svc->svc_t303);

    if (L3_DBG)
        buginf("\nL3SDL FRU1_ReleaseComplete : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
        call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));

    ret = fr_svc_send_msg_to_nli(hwidb, pak, svc, NL_REL_CNF);

    rem_fr_svc_from_idb_queue(hwidb, svc);
    free(svc);
}
/* 
 * FRUx_StatusEnquiry
 * When in any state, we received a MSG_STATUS_ENQUIRY. 
 * As per A.3, Sheet 17, of Q.931, send back a STATUS, remain in current state. 
 */
void FRUx_StatusEnquiry (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint		init_state;

    if (L3_DBG)
	buginf("\nFRUx_StatusEnquiry : %s", hwidb->hw_namestring);

    init_state = svc->call_state;
    fr_svc_send_msg(hwidb, svc, MSG_STATUS, svc->params.call_ref, svc_params);

    if (L3_DBG)
        buginf("\nL3SDL : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
        call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));
}
/* 
 * FRUx_Status
 * we received a MSG_STATUS. 
 * As per A.3, Sheet 17, of Q.931, check whether call state is 0. If yes, 
 * release the call, go to null state. If no, and the state is not 
 * compatible, send out a STATUS and check with the switch. 
 */
void FRUx_Status (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint		init_state;

    if (L3_DBG)
	buginf("\nFRUx_Status : %s", hwidb->hw_namestring);

    init_state = svc->call_state;

    if (!svc->call_state_inc_msg) {
	if (L3_DBG)
	    buginf("\nFRUx_Status : State: %s, not compatible with incoming state : %s", 
			l3cc_st(init_state), l3cc_st(svc->call_state_inc_msg));
	rem_fr_svc_from_idb_queue(hwidb, svc);
	free(svc);
	return;
    } 

    if (svc->call_state != svc->call_state_inc_msg )
    	fr_svc_send_msg(hwidb, svc, MSG_STATUS, svc->params.call_ref,
			svc_params);

    if (L3_DBG)
        buginf("\nL3SDL : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
        call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));
}

/* 
 * FRU1_DisconnectReq
 * The upper layer says that we are going down. 
 * Someone turned off SVCs. As per A.3, Sheet 3, of Q.931. 
 */
void FRU1_DisconnectReq (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint		init_state;

    if (L3_DBG)
	buginf("\nFRU1_DisconnectReq");

    init_state = svc->call_state;

    TIMER_STOP(svc->svc_t303);

    TIMER_START(svc->svc_t305, T305_DEFAULT);
    process_set_boolean(fr_svc_timer_bool, TRUE);

    svc->call_state = STATE_DISCONNECT_REQUEST;

    if (L3_DBG)
        buginf("\nL3SDL : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
        call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));
}
/* 
 * FRU3_Connect
 * Outgoing call received a CONNECT message. A.3, Sheet 7, Q.931, 03/93.
 */
void FRU3_Connect (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint                init_state;
    boolean		ret;

    if (L3_DBG)
	buginf("\nFRU3_Connect : %s", hwidb->hw_namestring);

    init_state = svc->call_state;

    TIMER_STOP(svc->svc_t310);

    ret = fr_svc_send_msg_to_nli(hwidb, pak, svc, NL_CALL_CNF);

    svc->call_state = STATE_ACTIVE;

    if (L3_DBG)
        buginf("\nL3SDL : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
        call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));
}
/* 
 * FRUx_Disconnect
 * Ref: A.3, Sheet 19, Q.931
 */
void FRUx_Disconnect (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint                init_state;
    boolean 		ret;

    if (L3_DBG)
	buginf("\nFRUx_Disconnect : %s", hwidb->hw_namestring);

    init_state = svc->call_state;

    /* 
     * stop all timers
     */
    TIMER_STOP(svc->svc_t303);
    TIMER_STOP(svc->svc_t305);
    TIMER_STOP(svc->svc_t308);
    TIMER_STOP(svc->svc_t310);
    TIMER_STOP(svc->svc_t322);

    ret = fr_svc_send_msg_to_nli(hwidb, pak, svc, NL_REL_IND);

    svc->call_state = STATE_DISCONNECT_INDICATION;

    if (L3_DBG)
        buginf("\nL3SDL : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
        call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));

    ret = fr_svc_send_msg_to_nli(hwidb, pak, svc, NL_REL_CNF);

    if (L3_DBG)
	buginf("\nL3SDL : DISCONNECT received, Responding with a RELEASE message");

    fr_svc_send_msg(hwidb, svc, MSG_RELEASE, svc->params.call_ref, svc_params);

    TIMER_START(svc->svc_t308, T308_DEFAULT);
    process_set_boolean(fr_svc_timer_bool, TRUE);

    init_state = svc->call_state;
    svc->call_state = STATE_RELEASE_REQUEST;

    if (L3_DBG)
        buginf("\nL3SDL : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
        call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));
}
/* 
 * FRUx_Release
 * A.3. Sheet 19, page 215, Q.931
 */
void FRUx_Release (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint                init_state;
    boolean 		ret;

    if (L3_DBG)
	buginf("\nFRUx_Release : %s", hwidb->hw_namestring);

    init_state = svc->call_state;

    /* 
     * stop all timers
     */
    TIMER_STOP(svc->svc_t303);
    TIMER_STOP(svc->svc_t305);
    TIMER_STOP(svc->svc_t308);
    TIMER_STOP(svc->svc_t310);
    TIMER_STOP(svc->svc_t322);

    ret = fr_svc_send_msg_to_nli(hwidb, pak, svc, NL_REL_IND);

    svc->params.cause = NORMAL_CALL_CLEARING;
    fr_svc_send_msg(hwidb, svc, MSG_RELEASE_COMPLETE, svc->params.call_ref,
		    svc_params);

    ret = fr_svc_send_msg_to_nli(hwidb, pak, svc, NL_REL_CNF);

    svc->call_state = STATE_NULL;

    if (L3_DBG)
        buginf("\nL3SDL : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
        call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));
}
/* 
 * FRU3_ReleaseRequest
 * received a release request internally from upstairs.
 * Page 203, Q.931, A.3, Sheet 7.
 */
void FRU3_ReleaseRequest (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint                init_state;
    boolean 		ret;

    if (L3_DBG)
	buginf("\nFRU3_ReleaseRequest : %s", hwidb->hw_namestring);

    init_state = svc->call_state;

    TIMER_STOP(svc->svc_t310);

    fr_svc_send_msg(hwidb, svc, MSG_RELEASE, svc->params.call_ref, svc_params);

    TIMER_START(svc->svc_t308, T308_DEFAULT);
    process_set_boolean(fr_svc_timer_bool, TRUE);

    ret = fr_svc_send_msg_to_nli(hwidb, pak, svc, NL_REL_IND);

    svc->call_state = STATE_RELEASE_REQUEST;

    if (L3_DBG)
        buginf("\nL3SDL : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
        call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));

}
/* 
 * FRUx_ReleaseRequest
 * received a release request internally from upstairs.
 */
void FRUx_ReleaseRequest (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint                init_state;
    boolean 		ret;

    if (L3_DBG)
	buginf("\nFRUx_ReleaseRequest : %s", hwidb->hw_namestring);

    init_state = svc->call_state;

    TIMER_STOP(svc->svc_t310);

    fr_svc_send_msg(hwidb, svc, MSG_RELEASE, svc->params.call_ref, svc_params);

    TIMER_START(svc->svc_t308, T308_DEFAULT);
    process_set_boolean(fr_svc_timer_bool, TRUE);

    ret = fr_svc_send_msg_to_nli(hwidb, pak, svc, NL_REL_IND);

    svc->call_state = STATE_RELEASE_REQUEST;

    if (L3_DBG)
        buginf("\nL3SDL : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
        call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));
}

/* 
 * FRUx_SplReleaseRequest
 * received a release request internally from upstairs.
 * This is a special case of FRUx_ReleaseRequest where the NLI 
 * is not told. It already knows. By not letting it know again, we 
 * prevent code loops. Used when parameter negotiation 
 * fails during call setup. In such a case we want to clean up the 
 * resources we allocated. Greaceful way is through a RELEASE. 
 */
void FRUx_SplReleaseRequest (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint                init_state;

    if (L3_DBG)
	buginf("\nFRUx_SplReleaseRequest : %s", hwidb->hw_namestring);

    init_state = svc->call_state;

    TIMER_STOP(svc->svc_t310);

    svc->params.cause = PROBLEM_WITH_CALL_PARAMETERS;

    fr_svc_send_msg(hwidb, svc, MSG_RELEASE, svc->params.call_ref, svc_params);

    svc->call_state = STATE_RELEASE_REQUEST;

    if (L3_DBG)
        buginf("\nL3SDL : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
        call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));
}

/* 
 * FRU6_Disconnect
 * In State Call Present, we received a DISCONNECT.
 * Page 204, Q.931, A.3, Sheet 8
 */
void FRU6_Disconnect (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint                init_state;
    boolean 		ret;

    if (L3_DBG)
	buginf("\nFRU6_Disconnect : %s", hwidb->hw_namestring);

    init_state = svc->call_state;

    ret = fr_svc_send_msg_to_nli(hwidb, pak, svc, NL_REL_IND);

    svc->call_state = STATE_DISCONNECT_INDICATION;

    if (L3_DBG)
        buginf("\nL3SDL : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
        call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));
}

/* 
 * FRU6_Release
 * In State Call Present, we received a RELEASE. 
 * A.3, Sheet 8, Q.931, Page 204.
 */
void FRU6_Release (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint                init_state;
    boolean 		ret;

    if (L3_DBG)
	buginf("\nFRU6_Release : %s", hwidb->hw_namestring);

    init_state = svc->call_state;

    ret = fr_svc_send_msg_to_nli(hwidb, pak, svc, NL_REL_IND);

    svc->params.cause = NORMAL_CALL_CLEARING;
    fr_svc_send_msg(hwidb, svc, MSG_RELEASE_COMPLETE, svc->params.call_ref,
		    svc_params);

    ret = fr_svc_send_msg_to_nli(hwidb, pak, svc, NL_REL_CNF);

    svc->call_state = STATE_NULL;

    if (L3_DBG)
        buginf("\nL3SDL : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
        call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));

    rem_fr_svc_from_idb_queue(hwidb, svc);
    free(svc);
}

/* 
 * FRU11_Disconnect
 * In the DISCONNECT_REQ state, we received a DISCONNECT. 
 * Page 207, A.3, Sheet 11, Q.931.
 */
void FRU11_Disconnect (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint                init_state;

    if (L3_DBG)
	buginf("\nFRU11_Disconnect : %s", hwidb->hw_namestring);

    init_state = svc->call_state;

    TIMER_STOP(svc->svc_t305);

    fr_svc_send_msg(hwidb, svc, MSG_RELEASE, svc->params.call_ref, svc_params);

    TIMER_START(svc->svc_t308, T308_DEFAULT);
    process_set_boolean(fr_svc_timer_bool, TRUE);

    svc->call_state = STATE_RELEASE_REQUEST;

    if (L3_DBG)
        buginf("\nL3SDL : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
        call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));
}
/* 
 * FRU11_Release
 * In the DISCONNECT_REQ state, we received a RELEASE. 
 * Page 207, Q.931, A.3, Sheet 11.
 */
void FRU11_Release (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint                init_state;
    boolean 		ret;

    if (L3_DBG)
	buginf("\nFRU11_Release : %s", hwidb->hw_namestring);

    init_state = svc->call_state;

    TIMER_STOP(svc->svc_t305);

    ret = fr_svc_send_msg_to_nli(hwidb, pak, svc, NL_REL_CNF);

    svc->params.cause = NORMAL_CALL_CLEARING;
    fr_svc_send_msg(hwidb, svc, MSG_RELEASE_COMPLETE, svc->params.call_ref,
		    svc_params);

    svc->call_state = STATE_NULL;

    if (L3_DBG)
        buginf("\nL3SDL : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
        call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));

    rem_fr_svc_from_idb_queue(hwidb, svc);
    free(svc);
}
/* 
 * FRU12_Release
 * In the STATE_DISCONNECT_INDICATION state, a RELEASE was received.
 * Page 208, Q.931, A.3, Sheet 12
 */
void FRU12_Release (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint                init_state;
    boolean 		ret;

    if (L3_DBG)
	buginf("\nFRU12_Release %s", hwidb->hw_namestring);

    init_state = svc->call_state;

    ret = fr_svc_send_msg_to_nli(hwidb, pak, svc, NL_REL_IND);

    svc->params.cause = NORMAL_CALL_CLEARING;
    fr_svc_send_msg(hwidb, svc, MSG_RELEASE_COMPLETE, svc->params.call_ref,
		    svc_params);

    ret = fr_svc_send_msg_to_nli(hwidb, pak, svc, NL_REL_CNF);

    svc->call_state = STATE_NULL;

    if (L3_DBG)
        buginf("\nL3SDL : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
        call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));

    rem_fr_svc_from_idb_queue(hwidb, svc);
    free(svc);
}
/* 
 * FRU12_ReleaseRequest
 * In the STATE_DISCONNECT_INDICATION state, a release 
 * request came from upstairs. 
 * Page 208, Q.931, A.3, Sheet 12
 */
void FRU12_ReleaseRequest (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint                init_state;
    boolean 		ret;

    if (L3_DBG)
	buginf("\nFRU12_ReleaseRequest : %s", hwidb->hw_namestring);

    init_state = svc->call_state;

    fr_svc_send_msg(hwidb, svc, MSG_RELEASE, svc->params.call_ref, svc_params);

    TIMER_START(svc->svc_t308, T308_DEFAULT);
    process_set_boolean(fr_svc_timer_bool, TRUE);

    svc->call_state = STATE_RELEASE_REQUEST;

    ret = fr_svc_send_msg_to_nli(hwidb, pak, svc, NL_REL_IND);

    if (L3_DBG)
        buginf("\nL3SDL : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
        call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));
}

/* 
 * FRU19_Release
 * Used for an incoming RELEASE. page 211, Q.931., A.3, Sheet 15
 * page 211, Q.931. 03/93.
 */
void FRU19_Release (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint                init_state;
    boolean 		ret;

    if (L3_DBG)
	buginf("\nFRU19_Release %s", hwidb->hw_namestring);

    init_state = svc->call_state;

    TIMER_STOP(svc->svc_t308);

    ret = fr_svc_send_msg_to_nli(hwidb, pak, svc, NL_REL_CNF);

    svc->call_state = STATE_NULL;

    if (L3_DBG)
        buginf("\nL3SDL : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
        call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));

    rem_fr_svc_from_idb_queue(hwidb, svc);
    free(svc);
}
/* 
 * FRU19_ReleaseComplete
 * Used for an incoming RELEASE COMPLETE. page 211, Q.931., A.3, Sheet 15
 */
void FRU19_ReleaseComplete (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    if (L3_DBG)
	buginf("\nFRU19_ReleaseComplete %s", hwidb->hw_namestring);

    FRU19_Release(hwidb, svc, pak, call_ref, event, svc_params);
}

/* 
 * FRU19_Status
 * In STATE_RELEASE_REQUEST, we received a STATUS 
 * page 211, Q.931, A.3, Sheet 15
 */
void FRU19_Status (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint                init_state;

    if (L3_DBG)
	buginf("\nFRU19_Status %s", hwidb->hw_namestring);

    init_state = svc->call_state;

    if (svc->call_state == 0) {

	/* 
  	 * The below sequence does not conform to Q.931. 
 	 */
	svc->call_state = STATE_RELEASE_REQUEST;

    	if (L3_DBG)
            buginf("\nL3SDL : Ref: %d, Init : %s, Rcvd: %s, Next : %s", call_ref,
            l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));
    } else {
	svc->call_state = STATE_NULL;

    	rem_fr_svc_from_idb_queue(hwidb, svc);

    	if (L3_DBG)
            buginf("\nL3SDL : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
            call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));

    	free(svc);

	return;
    }
}

/* 
 * FRUx_Idletoolong
 * Idle timer has kicked in. We'll need to tear the SVC down. 
 * As per page 141 of Q.931, Sec 3.3.3, we need to use a DISCONNECT message 
 * for user initiated call clearing. We also use the same mechanism when 
 * layer 2 goes down. This can happen due to a "no frame svc" 
 * command. Or when the interface is shut down.
 */
void FRUx_Idletoolong (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint                init_state;

    if (L3_DBG)
	buginf("\nFRUx_Idletoolong: %s", hwidb->hw_namestring);

    init_state = svc->call_state;

    svc->params.cause = NORMAL_CALL_CLEARING;
    fr_svc_send_msg(hwidb, svc, MSG_DISCONNECT, svc->params.call_ref,
		    svc_params);

    /* 
     * If the interface is down, do not start any timers. 
     * This is a case when an interface may have been shut down or a 
     * cable has been pulled. We are not going to get any replies 
     * for the above DISCONNECT. This is an internal clear.
     */
    if (hwidb->state == IDBS_UP) {
    	TIMER_START(svc->svc_t305, T305_DEFAULT);
     	process_set_boolean(fr_svc_timer_bool, TRUE);
    }

    svc->call_state = STATE_DISCONNECT_REQUEST;

    if (L3_DBG)
        buginf("\nL3SDL : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
        call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));

    /* 
     * For abnormal situations, such as someone
     * shutting the interface down, we'll need to clean out things.
     * For normal timeouts, don't do any cleanup here. let the RELEASE/COMPLETE 
     * exchange do it. That will definitely be more graceful.
     */
    if (hwidb->state != IDBS_UP) {
    	rem_fr_svc_from_idb_queue(hwidb, svc);
    	free(svc);
    }
}
/* 
 * FRUx_ReleaseComplete
 * Added 1/26/96 because the NORTEL switch is sending a 
 * RELEASE COMPLETE when we are in the active state. The situation
 * when this happens is not clear. But we need a FRUx_ReleaseComplete
 * anyway. This is to support Page 215 of Q.933, A.3, Sheet 19.
 */
void FRUx_ReleaseComplete (
	hwidbtype *hwidb,
	fr_svc_info_type *svc, 
	paktype *pak,
	uint call_ref, 
	uint event,
	struct fr_svc_params *svc_params)
{
    uint                init_state;
    boolean 		ret; 

    if (L3_DBG)
	buginf("\nFRUx_ReleaseComplete : %s", hwidb->hw_namestring);

    init_state = svc->call_state;

    TIMER_STOP(svc->svc_t303);
    TIMER_STOP(svc->svc_t305);
    TIMER_STOP(svc->svc_t308);
    TIMER_STOP(svc->svc_t310);
    TIMER_STOP(svc->svc_t322);

    svc->call_state = STATE_NULL;

    if (L3_DBG)
        buginf("\nL3SDL FRUx_ReleaseComplete : Ref: %d, Init : %s, Rcvd: %s, Next : %s",
        call_ref, l3cc_st(init_state), l3cc_ev(event),  l3cc_st(svc->call_state));

    ret = fr_svc_send_msg_to_nli(hwidb, pak, svc, NL_REL_CNF);

    rem_fr_svc_from_idb_queue(hwidb, svc);
    free(svc);
}
