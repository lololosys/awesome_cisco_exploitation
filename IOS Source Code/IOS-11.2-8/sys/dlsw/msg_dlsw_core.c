/* $Id: msg_dlsw_core.c,v 3.2.60.3 1996/09/13 23:46:08 hampton Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/msg_dlsw_core.c,v $
 *------------------------------------------------------------------
 * ALl the error message definitions for the DLSw core
 * DLSw subsystem 
 *
 * Fri July 29, 1994 G. Pandian
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * ALl the error message definitions for the DLSw core
 *------------------------------------------------------------------
 * $Log: msg_dlsw_core.c,v $
 * Revision 3.2.60.3  1996/09/13  23:46:08  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/08/28  12:41:30  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.60.1  1996/07/23  13:21:53  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.2  1995/11/17  09:04:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:25:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/18  21:21:39  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:27:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(DLSWC);	
msgdef_section("Data-link switching error messages");


/*
 * error messages for core in general
 */
msgdef(NODLSW, DLSWC, LOG_ERR, MSG_TRACEBACK, 
       "no dlsw, %s");
#define DLSWC_ERR_NODLSW		&msgsym(NODLSW, DLSWC)  
msgdef_explanation(
	"A data-link switching system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(NOPARAM, DLSWC, LOG_ERR, MSG_TRACEBACK, 
       "dlsw_action, null param blk");
#define DLSWC_ERR_NOPARAM	&msgsym(NOPARAM, DLSWC)  
msgdef_explanation(
	"A data-link switching system error occurred.");
msgdef_recommended_action(
	"If this message recurs, copy the error message exactly as it appears
	and report it to your technical support representative.");
msgdef_ddts_component("");


msgdef(FORKFAIL, DLSWC, LOG_ERR, MSG_TRACEBACK, 
	"Can't fork : %s process");
msgdef_explanation(
	"DLSw process cannot run.");
msgdef_recommended_action(
	"Reduce other system activity to ease memory demands. If conditions
	warrant, upgrade to a larger memory. If this message recurs, copy the
	error message exactly as it appears and report it to your technical
	support representative.");
msgdef_ddts_component("");


/*
 * SSP and peer related error messages
 */
msgdef(NOPEER, DLSWC, LOG_ERR, MSG_TRACEBACK, 
       "no peer, %s");
#define DLSWC_ERR_NOPEER		&msgsym(NOPEER, DLSWC)  
msgdef_explanation(
	"A data-link switching system error occurred.");
msgdef_recommended_action(
	"If this message recurs, copy the error message exactly as it appears
	and report it to your technical support representative.");
msgdef_ddts_component("");


msgdef(BADSSPEVENT, DLSWC, LOG_ERR, MSG_TRACEBACK, 
       "dlsw_action, bad ssp event %d, %s");
#define DLSWC_ERR_BADSSPEVENT	&msgsym(BADSSPEVENT, DLSWC)  
msgdef_explanation(
	"A data-link switching system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADSSPOP, DLSWC, LOG_ERR, MSG_TRACEBACK, 
       "bad ssp op in proc ssp %d");
#define DLSWC_ERR_BADSSPOP	&msgsym(BADSSPOP, DLSWC)  
msgdef_explanation(
	"A Silicon Switch Processor (SSP) frame with an invalid command was
	received from the remote peer.");
msgdef_recommended_action(
	"If the remote peer is a nonCisco router, confirm that it supports DLSw
	RFC 1795 standard.");
msgdef_ddts_component("");


msgdef(BADPEEROP, DLSWC, LOG_ERR, MSG_TRACEBACK, 
       "bad peer op in peer_to_core %d");
#define DLSWC_ERR_BADPEEROP	&msgsym(BADPEEROP, DLSWC)  
msgdef_explanation(
	"A data-link switching system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADSSPHDR, DLSWC, LOG_ERR, MSG_TRACEBACK, 
       "bad ssp hdr in proc ssp - %s = 0x%x ");
#define DLSWC_ERR_BADSSPHDR	&msgsym(BADSSPHDR, DLSWC)  
msgdef_explanation(
	"A Silicon Switch Processor (SSP) header received from the remote peer
	contains errors in one of the fields.");
msgdef_recommended_action(
	"If the remote peer is a nonCisco router, confirm that it supports the
	DLSw RFC 1795 standard.");
msgdef_ddts_component("");


/*
 * state machine related error messages
 */
msgdef(INVALIDTIMER, DLSWC, LOG_ERR, MSG_TRACEBACK, 
       "bad timer pop for %s, new state %s");
#define DLSWC_ERR_INVALIDTIMER	&msgsym(INVALIDTIMER, DLSWC)  
msgdef_explanation(
	"A data-link switching system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(INVALIDSTATE, DLSWC, LOG_ERR, MSG_TRACEBACK, 
       "finite state machine in invalid state");
#define DLSWC_ERR_INVALIDSTATE	&msgsym(INVALIDSTATE, DLSWC)  
msgdef_explanation(
	"A data-link switching system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(FAILASSERT, DLSWC, LOG_ERR, MSG_TRACEBACK, 
       "Assertion failed: %s");
#define DLSWC_ERR_FAILASSERT	&msgsym(FAILASSERT, DLSWC)  
msgdef_explanation(
	"An internal software inconsistency occurred. The router continues to
	operate.");
msgdef_recommended_action(
	"If this continues to recur, record the entire message and note any
	DLSw+ problems that you experience. Provide this information to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(FSMCALL, DLSWC, LOG_ERR, 0, 
       "%s state %s, event: %s%s");
#define DLSWC_ERR_FSMCALL	&msgsym(FSMCALL, DLSWC)  
msgdef_explanation(
	"A data-link switching system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(FSMACTION, DLSWC, LOG_ERR, 0, 
       " %s ");
#define DLSWC_ERR_FSMACTION	&msgsym(FSMACTION, DLSWC)  
msgdef_explanation(
	"Indicates frames sent by DLSw on LAN or WAN side. This is good,
	expected behavior.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef(FSMERR, DLSWC, LOG_ERR, MSG_TRACEBACK, 
       "fsm error %s %s %s event: %s%s");
#define DLSWC_ERR_FSMERR		&msgsym(FSMERR, DLSWC)  
msgdef_explanation(
	"A data-link switching system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(FSMINTERR, DLSWC, LOG_ERR, MSG_TRACEBACK, 
       "fsm internal error - %s");
#define DLSWC_ERR_FSMINTERR	&msgsym(FSMINTERR, DLSWC)  
msgdef_explanation(
	"A data-link switching system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(FSMUNKERR, DLSWC, LOG_ERR, MSG_TRACEBACK, 
       "%s %d: %s %s event:%s%s input: %d %d  states: %d %d");
#define DLSWC_ERR_FSMUNKERR	&msgsym(FSMUNKERR, DLSWC)  
msgdef_explanation(
	"A data-link switching system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADDLCOP, DLSWC, LOG_ERR, MSG_TRACEBACK, 
       "dlsw_action, bad dlc event %d, %s");
#define DLSWC_ERR_BADDLCOP	&msgsym(BADDLCOP, DLSWC)  
msgdef_explanation(
	"A data-link switching system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADTIMEROP, DLSWC, LOG_ERR, MSG_TRACEBACK, 
       "dlsw_action, bad timer event %d, %s");
#define DLSWC_ERR_BADTIMEROP	&msgsym(BADTIMEROP, DLSWC)  
msgdef_explanation(
	"A data-link switching system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


/*
 * CLSI related error messages
 */
msgdef(BADCLSIIND, DLSWC, LOG_ERR, MSG_TRACEBACK, 
       "%s Invalid indication %s, %s");
#define DLSWC_ERR_BADCLSIIND	&msgsym(BADCLSIIND, DLSWC)  
msgdef_explanation(
	"A data-link switching system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADCLSIRET, DLSWC, LOG_ERR, MSG_TRACEBACK, 
       "%s Invalid ret code (0x%x) %s, %s");
#define DLSWC_ERR_BADCLSIRET	&msgsym(BADCLSIRET, DLSWC)  
msgdef_explanation(
	"A data-link switching system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADCLSICNF, DLSWC, LOG_ERR, MSG_TRACEBACK, 
       "%s Invalid confirm %s %s");
#define DLSWC_ERR_BADCLSICNF	&msgsym(BADCLSICNF, DLSWC)  
msgdef_explanation(
	"A data-link switching system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADCLSI, DLSWC, LOG_ERR, MSG_TRACEBACK, 
       "%s primitive not valid for dlsw %s %s");
#define DLSWC_ERR_BADCLSI	&msgsym(BADCLSI, DLSWC)  
msgdef_explanation(
	"A data-link switching system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADCLSIIDTYPE, DLSWC, LOG_ERR, MSG_TRACEBACK, 
       "%s Invalid ID type = 0x%x %s");
#define DLSWC_ERR_BADCLSIIDTYPE	&msgsym(BADCLSIIDTYPE, DLSWC)  
msgdef_explanation(
	"A data-link switching system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADCLSIPRIMTYPE, DLSWC, LOG_ERR, MSG_TRACEBACK, 
       "%s Invalid primitive type = 0x%x %s");
#define DLSWC_ERR_BADCLSIPRIMTYPE	&msgsym(BADCLSIPRIMTYPE, DLSWC)  
msgdef_explanation(
	"A data-link switching system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADCLSIHDRLEN, DLSWC, LOG_ERR, MSG_TRACEBACK, 
       "%s Bad header len = 0x%x %s");
#define DLSWC_ERR_BADCLSIHDRLEN	&msgsym(BADCLSIHDRLEN, DLSWC)  
msgdef_explanation(
	"A data-link switching system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADCLSIDATALEN, DLSWC, LOG_ERR, MSG_TRACEBACK, 
       "%s Bad data len = %d %s ");
#define DLSWC_ERR_BADCLSIDATALEN	&msgsym(BADCLSIDATALEN, DLSWC)  
msgdef_explanation(
	"A data-link switching system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADCLSISAP, DLSWC, LOG_ERR, MSG_TRACEBACK, 
       "%s Bad clsi SAP id = %x %s ");
#define DLSWC_ERR_BADCLSISAP	&msgsym(BADCLSISAP, DLSWC)  
msgdef_explanation(
	"A data-link switching system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(SENDSSP, DLSWC, LOG_ERR, 0,
       "SSP OP = %d(%s) %s to %s %s");
#define DLSWC_ERR_SENDSSP	&msgsym(SENDSSP, DLSWC)  
msgdef_explanation(
	"A Silicon Switch Program (SSP) frame was sent to the remote peer.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef(RECVSSP, DLSWC, LOG_ERR, 0,
       "SSP OP = %d(%s) %s from %s");
#define DLSWC_ERR_RECVSSP	&msgsym(RECVSSP, DLSWC)  
msgdef_explanation(
	"A Silicon Switch Program (SSP) frame was received from the remote
	peer.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");
