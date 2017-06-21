/* $Id: fr_svc_misc.c,v 3.1.64.6 1996/07/03 01:57:50 chifei Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_svc_misc.c,v $
 *------------------------------------------------------------------
 * fr_svc_misc.c : Miscellaneous support routines 
 *
 * May 1995, Shankar Natarajan
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fr_svc_misc.c,v $
 * Revision 3.1.64.6  1996/07/03  01:57:50  chifei
 * CSCdi58333:  Some frame relay SVC commands are available in
 * non-enterprise images
 * Branch: California_branch
 * Modularized "frame svc" and "frame lapf" commands to go with FR SVC
 * subsystem; fixed/clean-up LAPF command format; consolidated DL
 * primitives define's.
 *
 * Revision 3.1.64.5  1996/06/30  22:23:39  shankar
 * CSCdi59475:  Change units for Be/Bc from bytes to bits
 * Branch: California_branch
 * Images that do not include SVCs fail to link.
 *
 * Revision 3.1.64.4  1996/06/27  21:38:20  skoh
 * CSCdi59475:  Change units for Be/Bc from bytes to bits
 * Branch: California_branch
 *
 * Revision 3.1.64.3  1996/06/04  00:25:16  snyder
 * CSCdi59389:  spellink errors
 * Branch: California_branch
 *
 * awareded -> awarded
 * Interworking,unspecified -> Interworking, unspecified
 * Requestor -> Requester
 *
 * Revision 3.1.64.2  1996/05/07  00:28:06  skoh
 * CSCdi56827:  fr_svc_select_next_call_ref picks wrong call ref. id
 * Branch: California_branch
 *
 * Revision 3.1.64.1  1996/04/25  23:23:46  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.68.1  1996/04/24  04:11:32  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.1  1995/11/09  13:49:11  shaker
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
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "frame_relay.h"
#include "fr_debug.h"
#include "../parser/parser_defs_exec.h"
#include "../wan/parser_defs_frame.h"
#include "fr_switch.h"
#include "../h/types.h"
#include "../wan/fr_svc.h"
#include "fr_l2.h"
#include "../wan/fr_svc_registry.h"


static generic_def l3cc_event_help[] = {
	{MSG_CALL_PROCEEDING, 	"MSG_CALL_PROCEEDING"},
	{MSG_SETUP,		"MSG_SETUP"},
	{MSG_CONNECT,		"MSG_CONNECT"},
	{MSG_DISCONNECT,	"MSG_DISCONNECT"},
	{MSG_RELEASE,		"MSG_RELEASE"},
	{MSG_RELEASE_COMPLETE,	"MSG_RELEASE_COMPLETE"},
	{MSG_STATUS_ENQUIRY,	"MSG_STATUS_ENQUIRY"},
	{MSG_STATUS,		"MSG_STATUS"},
	{T303_TIMEOUT,		"T303_TIMEOUT"},
	{T305_TIMEOUT,		"T305_TIMEOUT"},
	{T308_TIMEOUT,		"T308_TIMEOUT"},
	{T310_TIMEOUT,		"T310_TIMEOUT"},
	{T322_TIMEOUT,		"T322_TIMEOUT"},
	{FR_DL_REL_CNF,		"DL_REL_CONF"}, 
	{FR_DL_REL_IND,		"DL_REL_IND"},
	{FR_DL_REL_REQ,		"DL_REL_REQ"},
	{FR_DL_EST_REQ,		"DL_EST_REQ"},
	{FR_DL_EST_IND,		"DL_EST_IND"},
	{FR_DL_DATA_REQ,	"DL_DATA_REQ"},
	{FR_DL_DATA_IND,	"DL_DATA_IND"},
	{FR_DL_EST_CNF,		"DL_EST_CONF"},
	{SETUP_REQUEST,		"SETUP_REQUEST"},
	{TIDL_TIMEOUT, 		"IDLE TIMER EXPIRED"},
	{RELEASE_REQ, 		"RELEASE REQUEST"},
	{SPL_RELEASE_REQ, 	"INTERNAL CALL CLEANUP"},
	{0xFF,			" "}
};

static generic_def l3cc_state_help[] = {
	{STATE_NULL,		"STATE_NULL"},
	{STATE_CALL_INITIATED,	"STATE_CALL_INITIATED"},
	{STATE_CALL_PROCEEDING,	"STATE_CALL_PROCEEDING"},
	{STATE_CALL_PRESENT,	"STATE_CALL_PRESENT"},
	{STATE_INC_CALL_PROC,	"STATE_INCOMING_CALL_PROC"},
	{STATE_ACTIVE,		"STATE_ACTIVE"},
	{STATE_DISC_REQUEST,	"STATE_DISC_REQUEST"},
	{STATE_DISC_IND, 	"STATE_DISCONNECT_INDICATION"},
	{STATE_RELEASE_REQUEST,  "STATE_RELEASE_REQUEST"}, 
	{0xFF,			" "}

};
/* Cause value decimal number and definition. */
generic_def cause_value_def[] = {
    {UNALLOC_NUMBER,                "Unassigned number"},
    {NO_ROUTE_TO_TRANSIT_NET,       "No route to specified transit network"},
    {NO_ROUTE_TO_DESTINATION,       "No route to destination"},
    {CHANNEL_UNACCEPTABLE,          "Channel unacceptable"},
    {CALL_AWARDED_OVER_CHANNEL,     "Call awarded and delivered in a channel"},
    {NORMAL_CALL_CLEARING,          "Normal call clearing"},
    {USER_BUSY,                     "User busy"},
    {NO_USER_RESPONDING,            "No user responding"},
    {NO_ANSWER_FROM_USER,           "User alerting, no answer"},
    {CALL_REJECTED,                 "Call rejected"},
    {NUMBER_CHANGED,                "Number changed"},
    {NON_SELECTED_USER_CLEARING,    "Non-selected user clearing"},
    {DEST_OUT_OF_ORDER,             "Destination out of order"},
    {INVALID_NUMBER_FORMAT,         "Invalid number format/Incomplete number"},
    {FACILITY_REJECTED,             "Facility rejected"},
    {RESP_TO_STATUS_ENQ,            "Response to STATUS ENQUIRY"},
    {NORMAL_UNSPECIFIED,            "Normal, unspecified"},
    {NO_CIRCUIT_AVAILABLE,          "No circuit/channel available"},
    {NETWORK_OUT_OF_ORDER,          "Network out of order"},
    {TEMPORARY_FAILURE,             "Temporary failure"},
    {SWITCH_CONGESTION,             "Switching equipment congestion"},
    {ACCESS_INFO_DISCARDED,         "Access information discarded"},
    {REQ_CIRCUIT_NOT_AVAILABLE,     "Requested circuit/channel not available"},
    {RESOURCE_UNAVAILABLE,          "Resource unavailable, unspecified"},
    {BEARER_CAP_NOT_AUTHORIZED,     "Bearer cap. not authorized"},
    {BEARER_CAP_NOT_AVAILABLE,      "Bearer cap. not presently available"},
    {SERVICE_NOT_AVAILABLE,         "Service or option not available"},
    {BEARER_CAP_NOT_IMPLEMENTED,    "Bearer cap. not implemented"},
    {CHANNEL_TYPE_NOT_IMPLEMENTED,  "Channel type not implemented"},
    {REQ_FACILITY_NOT_IMPLEMENTED,  "Requested facility not implemented"},
    {ONLY_DIG_INFO_IS_AVAILABLE,    "Only digital info. Bearer Cap. is available"},
    {SERVICE_OPTION_NOT_IMPLEMENTED, "Service or option not implemented"},
    {INVALID_CALL_REF_VALUE,        "Invalid call reference value"},
    {CHANNEL_DOES_NOT_EXIST,        "Identified channel does not exist"},
    {CALL_ID_IN_USE,                "Call identity in use"},
    {NO_CALL_SUSPENDED,             "No call suspended"},
    {CALL_HAS_BEEN_CLEARED,         "Call with the requested call ID cleared"},
    {INCOMPATIBLE_DESTINATION,      "Incompatible destination"},
    {INVALID_TRANSIT_NET_SELECTION, "Invalid transit network selection"},
    {INVALID_MESSAGE,               "Invalid message, unspecified"},
    {MANDATORY_IE_MISSING,          "Mandatory information element is missing"},
    {MESSAGE_TYPE_NOT_IMPLEMENTED,  "Message type not implemented"},
    {MESSAGE_INCOMPATIBLE_WITH_CALL,"Message Incompatible with call state"},
    {IE_NOT_IMPLEMENTED,            "IE non-existent or not implemented"},
    {INVALID_IE_CONTENTS,           "Invalid information element contents"},
    {MSG_NOT_COMPATIBLE_CALL_STATE, "Message not compatible with call state"},
    {RECOVERY_ON_TIMER_EXPIRY,      "Recovery on time expiry"},
    {PROT_ERROR_UNSPECIFIED,        "Protocol error, unspecified"},
    {INTERWORKING_UNSPECIFIED,      "Interworking, unspecified"},
    {GLOBAL_CALL_REF,		    "Global call reference"},
    {INVALID_CALL_REF_LENGTH,	    "Invalid call reference length"},
    {INVALID_PROT_DISC,		    "Invalid protocol discrimination IE"},
    {INVALID_IE_TYPE,		    "Invalid IE type"},
    {INVALID_IE_LENGTH,		    "Invalid IE length"},
    {UNKNOWN_BEARER_CAP_BYTE3,	    "Unknown bearer capability byte3"},
    {UNKNOWN_BEARER_CAP_BYTE4,	    "Unknown bearer capability byte4"},
    {UNKNOWN_BEARER_CAP_BYTE5,	    "Unknown bearer capability byte5"},
    {NO_MEMORY_TO_ASSIGN,	    "No memory to assign"},
    {NO_FR_MEMORY,		    "No Frame-Relay memory"},
    {END_OF_IE,			    "End of IE"},
    {ILLEGAL_EXTENSION_BIT,	    "Illegal extension bit"},
    {INVALID_CONTENTS_IN_FIELD,	    "Invalid contents in field"},
    {IE_TOO_LONG,		    "IE too long"},
    {IE_TOO_SHORT,		    "IE too short"},
    {PROBLEM_WITH_CALL_PARAMETERS,  "Parameter Negotiation failed"},
    {0xFF,			    " "}
};

/*
 * l3cc_st
 * Utility event for printing debugs
 */
char *l3cc_st(uchar state)
{
    generic_def         *ptr;

    ptr = l3cc_state_help;
    return(fr_svc_table_search(state, ptr));
}

/*
 * cause_def
 * Utility printing definition(message) of cause value
 */
char *cause_def(uchar value)
{
    generic_def         *ptr;

    ptr = cause_value_def;
    return(fr_svc_table_search(value, ptr));
}

/*
 * l3cc_ev
 * Utility event for printing debugs
 */
char *l3cc_ev(uchar event)
{
    generic_def         *ptr;

    ptr = l3cc_event_help;
    return(fr_svc_table_search(event, ptr));
}

/* 
 * fr_svc_table_search
 * The central point for l3cc_st, cause_def and l3cc_ev
 */
char *fr_svc_table_search ( uchar event, generic_def *ptr)
{
    for (; ptr->type != 0xFF; ptr++) {
	if (ptr->type == event)
 	    return(ptr->help);
    }
    return("Unknown event");
}
/*
 * svc_ie_print
 * Print bytes of the IE.
 */
void svc_ie_print (
	char *name, 
	char *ie_ptr, 
	char length, 
	uint direction)
{
    int         iz;
    ushort      *cptr;

    cptr = (ushort *)ie_ptr;
    buginf("\n\tDir: %s, Type: %s, length: %d, ", 
		(direction == IN ? "U <-- N" : "U --> N"), 
		name, length);
    if ((length %2))
	length++;
    length = length >> 1;
    for (iz = 0; iz < length; iz++, cptr++) {
	buginf(" 0x%04x", GETSHORT(cptr));
	if ( (iz>0) && !((iz+1) % 4))
	    buginf("\n\t\t\t\t\t");
    }
}
/*
 * fr_svc_init_ie_mgmt_area
 * In the fr_svc_info_type structure, a section is used for managing
 * incoming messages. Initialize this area 
 */
void fr_svc_init_ie_mgmt_area (fr_svc_info_type *svc)
{
    svc->bearer_cap_ie_present = FALSE;
    svc->dlci_ie_present = FALSE;
    svc->cg_pty_num_ie_present = FALSE;
    svc->cg_subaddr_ie_present = FALSE;
    svc->cd_pty_ie_present = FALSE;
    svc->cd_subaddr_ie_present = FALSE;
    svc->cn_number_ie_present = FALSE;
    svc->cn_subaddr_ie_present = FALSE;
    svc->xit_nwk_ie_present = FALSE;
    svc->ll_comp_ie_present = FALSE;
    svc->user_user_ie_present = FALSE;
    svc->cause_ie_present = FALSE;
    svc->call_state_ie_present = FALSE;
}
/*
 * fr_svc_lookup
 * Given a call_ref, search through the svc linked list
 * to find the appropriate data structure.
 */
fr_svc_info_type *fr_svc_lookup (
	hwidbtype *hwidb,
	uint call_ref)
{
    fr_svc_info_type       *svc;
    fr_idb_struct_type	*fr_idb;

    fr_idb = hwidb->frame_relay_stuff;

    for (svc = fr_idb->svc_head; svc; svc = svc->next) {
	if (svc->params.call_ref == call_ref) {
	    return(svc);
	}
    }
    return(NULL);
}
/*
 * fr_svc_select_next_call_ref
 * A brain-dead scheme to select the next free call ref.
 * Walk the svc queue within the fr_idb. Pick the first
 * number that is not taken.
 */
uint fr_svc_select_next_call_ref (hwidbtype *hwidb)
{
    fr_svc_info_type       *svc;
    uint                     ix;
    fr_idb_struct_type  *fr_idb;
    boolean               inuse;

    fr_idb = hwidb->frame_relay_stuff;

    for (ix = 1; ix < MAX_CALL_ID_PER_IDB; ix++) {
	inuse = FALSE;
        for (svc = fr_idb->svc_head; svc; svc = svc->next) {
            if (svc->params.call_ref == ix) {
                inuse = TRUE;
		break;
	    }
	}
        if (!inuse) 
	    break;
    }
    if (L3_DBG)
        buginf("\nNLI: Call Reference for new call: %d", ix);

    return(ix);

}
