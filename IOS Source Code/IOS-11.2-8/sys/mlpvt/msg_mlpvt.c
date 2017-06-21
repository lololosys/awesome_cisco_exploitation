/* $Id: msg_mlpvt.c,v 1.1.42.9 1996/09/13 23:50:55 hampton Exp $
 * $Source: /release/112/cvs/Xsys/mlpvt/msg_mlpvt.c,v $
 *------------------------------------------------------------------
 * msg_mlpvt.c -- MLPVT messages
 *
 * January 1996, Ashwin Baindur
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: msg_mlpvt.c,v $
 * Revision 1.1.42.9  1996/09/13  23:50:55  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 1.1.42.8  1996/08/28  13:00:37  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 1.1.42.7  1996/06/04  00:22:27  snyder
 * CSCdi59389:  spellink errors
 * Branch: California_branch
 *
 * misconfigurated -> misconfigured
 * authenication -> authentication
 * authenicated -> authenticated
 *
 * Revision 1.1.42.6  1996/06/03  18:02:54  syiu
 * CSCdi59320:  SGBP has too many errmsg facdefs
 * Branch: California_branch
 *
 * Revision 1.1.42.5  1996/05/22  02:24:13  perryl
 * CSCdi56889:  MLPVT Hellos detection of misconfiguration
 * Branch: California_branch
 * Print the possible misconfigured member name in the
 * error message
 *
 * Revision 1.1.42.4  1996/05/18  22:35:12  perryl
 * CSCdi57939:  MLPVT is a bad name for Stack Group Bidding Protocol
 * Branch: California_branch
 * Rename it to SGBP, Stack Group Bidding Protocol
 *
 * Revision 1.1.42.3  1996/05/06  21:50:00  perryl
 * CSCdi56532:  MLPVT does not properly break tie on equal high bids
 * Branch: California_branch
 * Randomize the tie breaker
 *
 * Revision 1.1.42.2  1996/05/01  02:06:54  perryl
 * CSCdi56162:  MultiChassis MLP and VPN AAA calls mismatched
 * Branch: California_branch
 * Add wrapper functions aaa_chap_rresponse() and aaa_chap_rchallenge(),
 * for L2F and MLPVT to do CHAP style authentication with the new AAA API.
 *
 * Revision 1.1.42.1  1996/04/27  06:34:55  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.1.2.8  1996/04/22  00:31:26  perryl
 * Branch: Dial1_branch
 * Added CHAP style authentication to MLPVT.
 * Used AAA to obtain password, eliminate "mlpvt password"
 * command.
 * Fixed misc bugs and enhanced debug messages.
 *
 * Revision 1.1.2.7  1996/04/03  22:59:02  abaindur
 * Branch: Dial1_branch
 *  Enhance bid generation scheme for MLP bundle mastership. Provide
 * a means of controlling mastership based on either locality of a call
 * or load-sharing.
 *
 * Revision 1.1.2.6  1996/03/09  00:48:00  abaindur
 * Branch: Dial1_branch
 *  Some state m/c fixes, cleanup, and documentation.
 *
 * Revision 1.1.2.5  1996/03/05  02:47:05  agh
 * Branch: Dial1_branch
 * Much cleanup of unwanted code, also added comments.
 *
 * Revision 1.1.2.4  1996/03/01  22:58:14  abaindur
 * Branch: Dial1_branch
 *  Remove config knob for a tie-breaker metric used in case of equal
 *  MLPPP bundle mastership bids.
 *
 * Revision 1.1.2.3  1996/03/01  21:43:19  abaindur
 * Branch: Dial1_branch
 *  o fix the query simulator; a casualty of the integration with MLP/VPN
 *  o enhance query state m/c to handle case where a peer goes down just
 *  when we're sending it a query msg. The TCP write blocks and a peer loss
 *  event blows the peer state from beneath.
 *
 * Revision 1.1.2.2  1996/02/13  18:03:34  agh
 * Branch: Dial1_branch
 * Clean up closing of TCP Pipes.
 *
 * Revision 1.1.2.1  1996/01/26  20:28:51  agh
 * Branch: Dial1_branch
 * Initial Code Commit for MLPVT.
 *
 * Revision 1.1  1996/01/26  19:23:02  abaindur
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"
 

facdef(SGBP);				/* Define the mlpvt group facility */
msgdef_section("");


/*
 * Messages related to queries
 */
msgdef(INVALIDB, SGBP, LOG_ERR, 0,
       "%s for bundle \'%s\' -- %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(NEWL, SGBP, LOG_DEBUG, 0,
       "Local query #%d for %s, count %d, ourbid %d");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(DUPL, SGBP, LOG_DEBUG, 0,
       "Duplicate local query #%d for %s, count %d, ourbid %d");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(NEWP, SGBP, LOG_DEBUG, 0,
       "Peer query #%d for %s, count %d, peerbid %d, ourbid %d");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(CANCEL, SGBP, LOG_DEBUG, 0, "Local query #%d:%d for bundle %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(DONE, SGBP, LOG_DEBUG, 0,
       "Query #%d for bundle %s, count %d, master is %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(TIMEOUT, SGBP, LOG_ERR, 0,
       "Peer %i bidding; state \'PB %s\' deleted");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(MQB, SGBP, LOG_DEBUG, 0, "\tBundle: %s\tState: %s\tOurBid: %03d");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(PB, SGBP, LOG_DEBUG, 0, "\t%i\tState: %s\tBid: %03d  Retry: %d");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(EQUAL, SGBP, LOG_DEBUG, 0,
       "%d equal highest bids, randomly select bid# %d");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");

msgdef(HANGUP, SGBP, LOG_DEBUG, 0,
       "I am a forward-only member, can't forward bundle %s, hangup");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");

/*
 * Messages related to peer-to-peer messages
 */
msgdef(INVFIELD, SGBP, LOG_ERR, 0,
       "%s field type %#x has %s %d (expected %d)");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(RCVD, SGBP, LOG_DEBUG, 0,
       "MQ %s from %i for query %d:%d, bundle %s, bid %d");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(SENT, SGBP, LOG_DEBUG, 0,
       "MQ %s to %i for query %d:%d, bundle %s, bid %d, len %d");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(UNEXP, SGBP, LOG_ERR, 0,
       "MQ %s from %i for query %d, bundle %s, bid %d, state %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(INVALID, SGBP, LOG_ERR, 0, "MQ message with %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(MQ, SGBP, LOG_DEBUG, 0,
       "%s (%#x) for query %d:%d, bundle %s, bid %d, len %d");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


/*
 * Messages for events
 */
msgdef(PEERERROR, SGBP, LOG_ERR, 0,
       "Peer %i %s during \'PB %s\' state for bundle %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(UNKNOWNPEER, SGBP, LOG_ERR, 0, "Event %#x from peer at %i");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(UNKNOWNEVENT, SGBP, LOG_ERR, 0, "Event %#x from peer at %i");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(SHUTDOWN, SGBP, LOG_NOTICE, 0, "%s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(STARTUP, SGBP, LOG_NOTICE, 0, "%s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(OPENFAIL, SGBP, LOG_ERR, 0,
       "Open failed for member %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(CLOSE, SGBP, LOG_DEBUG, 0, "Closing pipe for member %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(ARRIVING, SGBP, LOG_NOTICE, 0,
       "New peer event for member %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(LEAVING, SGBP, LOG_NOTICE, 0, "Member %s leaving group %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(UNKNOWN, SGBP, LOG_ERR, 0, "%i not known");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(STATE, SGBP, LOG_DEBUG, 0,
       "Member %s current State %d Reference %#x udp %#x");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef_limit(AUTHFAILED, SGBP, LOG_ALERT, 0,
             "Member %s failed authentication",
             MSGDEF_LIMIT_GLACIAL);
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(PEERMSG, SGBP, LOG_DEBUG, 0,
       "Type %#x, length %d, from peer %i (%s)");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


/*
 * Messages associated with hello protocol.
 */
msgdef(CHALLENGED, SGBP, LOG_DEBUG, 0,
       "Rcv Hello Challenge message from member %s using %i");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(RESPONSED, SGBP, LOG_DEBUG, 0,
       "Rcv Hello Response message from member %s using %i");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(CHALLENGE, SGBP, LOG_DEBUG, 0,
       "Send Hello Challenge to %s group %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(RESPONSE, SGBP, LOG_DEBUG, 0,
       "Send Hello Response to %s group %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(NORESP, SGBP, LOG_DEBUG, 0,
       "Fail to response to %s group %s, may not have password");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(UNKNOWNHELLO, SGBP, LOG_ALERT, 0,
       "Rcv Hello message from non-group member using %i");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(MISSCONF, SGBP, LOG_ALERT, 0,
       "Possible misconfigured member %s using %i");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(RCVINFO, SGBP, LOG_DEBUG, 0,
       "Received Info Addr = %i Reference = %#x");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");

msgdef(SENDINFO, SGBP, LOG_DEBUG, 0,
       "Send Info Addr to %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");

msgdef(KEEPALIVE, SGBP, LOG_DEBUG, 0,
       "Sending Keepalive to %s, retry=%d");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");

msgdef(KEEPALIVE_TIMEOUT, SGBP, LOG_DEBUG, 0,
       "Keepalive timeout on %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");

msgdef(SENDAUTHOK, SGBP, LOG_DEBUG, 0,
       "Send Hello Authentication OK to member %s using %i");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");

msgdef(AUTHOK, SGBP, LOG_DEBUG, 0,
       "Member %s State -> AuthOK");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");

msgdef(WAITINFO, SGBP, LOG_DEBUG, 0,
       "Member %s State -> WaitInfo");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");

msgdef(ACTIVE, SGBP, LOG_DEBUG, 0,
       "Member %s State -> Active");

msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");

msgdef(DIFFERENT, SGBP, LOG_ALERT, 0,
       "Rcv %s's addr %i is different from the hello's addr %i");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(NOTAUTH, SGBP, LOG_ALERT, 0,
       "Rcv Connection request by non-authenticated member %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(DUPLICATE, SGBP, LOG_ALERT, 0,
       "Rcv Duplicate bundle %s is found on member %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
