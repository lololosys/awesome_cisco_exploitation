/* $Id: msg_dlsw_peer.c,v 3.2.58.2 1996/09/13 23:46:11 hampton Exp $
 *------------------------------------------------------------------
 * Message File for the DLSw Peer Module
 *
 * August 1994, Frank Bordonaro 
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: msg_dlsw_peer.c,v $
 * Revision 3.2.58.2  1996/09/13  23:46:11  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.58.1  1996/08/28  12:41:32  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  09:04:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:25:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  18:52:05  fbordona
 * CSCdi36524:  CLS SDLC multidrop broken
 *              Clean up DLSw header files
 *
 * Revision 2.1  1995/06/07  20:27:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(DLSWP);
msgdef_section("Data-link switching error messages");


msgdef(PGENERAL, DLSWP, LOG_ERR, MSG_TRACEBACK, 
       "DLSw: %s %s");
#define PEER_GENERAL    &msgsym(PGENERAL, DLSWP)
msgdef_explanation(
	"A general error condition was detected as described in the message
	text. This error should not cause any network problems unless it
	happens repeatedly.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears and report it to your
	technical support representative as a case problem.");
msgdef_ddts_component("");


msgdef(PNOMEM, DLSWP, LOG_ERR, MSG_TRACEBACK, 
       "DLSw: No memory to %s");
#define PEER_NOMEM      &msgsym(PNOMEM, DLSWP)
msgdef_explanation(
	"The router ran out of system memory for operation, as described in the
	message text. Most likely, the router image requires more system memory
	(RAM) than the router contains.");
msgdef_recommended_action(
	"Consult with your technical support representative about memory
	requirements for a specific image.");
msgdef_ddts_component("");


msgdef(PPASSIVE, DLSWP, LOG_ERR, MSG_TRACEBACK, 
       "DLSw: passive open failed from %i(%d) -> %d");
#define PEER_PASSIVE    &msgsym(PPASSIVE, DLSWP)
msgdef_explanation(
	"The system does not have enough main memory (RAM) for TCP to allocate
	the data structures required to accept an incoming TCP connection. Most
	likely, the router image requires more system memory (RAM) than the
	router contains.");
msgdef_recommended_action(
	"Consult with your technical support representative about memory
	requirements for a specific image.");
msgdef_ddts_component("");


msgdef(PBADVALUE, DLSWP, LOG_ERR, MSG_TRACEBACK,
       "DLSw: %s: invalid %s %d");
#define PEER_BADVALUE   &msgsym(PBADVALUE, DLSWP)
msgdef_explanation(
	"A bad parameter value was passed to function chars1 - invalid
	parameter chars2 with value. This error should not cause any network
	problems unless it happens repeatedly.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears and report it to your
	technical support representative as a case problem.");
msgdef_ddts_component("");


msgdef(PCAPMISMATCH, DLSWP, LOG_ERR, 0,
       "DLSw: %s: %s");
#define PEER_BADCAP   &msgsym(PCAPMISMATCH, DLSWP)
msgdef_explanation(
	"Priority keyword was used on one router but not on the other. The
	priority keyword is an option on the DLSW remote peer definition. If
	priority keyword is used one router, it must be used on the other.");
msgdef_recommended_action(
	"If you wish to use prioritization, use it on both routers.");
msgdef_ddts_component("");

msgdef(PLFCHANGE, DLSWP, LOG_ERR, 0, 
       "%s %d; %s");
#define PEER_LFCHANGE &msgsym(PLFCHANGE, DLSWP)
msgdef_explanation(
	"The Largest frame size configured in the DLSW remote peer definition,
	is being changed. This is due to the fact that the MTU size configured
        on the interface, used by DLSW, is less then the configured value of
        largest frame on DLSW remote peer definition. The largest frame size
        is being changed to that configured on the interface.");
msgdef_recommended_action(
	"If you wish to use this largest frame size for DLSW, increase the
        MTU on the interface to a value larger then the configured largest
	frame size.");
msgdef_ddts_component("");
