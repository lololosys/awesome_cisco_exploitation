/* $Id: msg_rsrb.c,v 3.2.62.2 1996/09/14 00:02:16 hampton Exp $
 * $Source: /release/112/cvs/Xsys/srt/msg_rsrb.c,v $
 *------------------------------------------------------------------
 * msg_rsrb.c - Message file for rsrb facility
 *
 * July 17, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_rsrb.c,v $
 * Revision 3.2.62.2  1996/09/14  00:02:16  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/08/28  13:15:52  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  18:54:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:31:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:00:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(RSRB);				/* Define the RSRB facility */
msgdef_section("Remote source-route bridging error messages");


msgdef(OPTNULL, RSRB, LOG_WARNING, 0, "Remopened and t NULL");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(PEERSTAT, RSRB, LOG_WARNING, 0, "Peer %s, wrong state %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(CONNSTAT, RSRB, LOG_WARNING, 0,
       "Peer %s, IFin, bad connection state %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(BADLEN, RSRB, LOG_WARNING, 0,
       "Peer %s, %s, bad length %d, trn %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If either message recurs, call your technical support representative
	for assistance.");
msgdef_ddts_component("");


msgdef(BADVERSIONIF, RSRB, LOG_ERR, 0,
       "IFin: %s: version mismatch, mine %d, theirs %d");
msgdef_explanation(
	"The remote end of a direct serial peer is running the wrong version of
	the system software. Either the local end, the remote end, or both are
	not up to date.");
msgdef_recommended_action(
	"Call your technical support representative for an update.");
msgdef_ddts_component("");


msgdef(BADVERSIONFST, RSRB, LOG_ERR, 0,
       "FSTin: %s: version mismatch, mine %d, theirs %d");
msgdef_explanation(
	"The remote end of a direct serial peer is running the wrong version of
	the system software. Either the local end, the remote end, or both are
	not up to date.");
msgdef_recommended_action(
	"Call your technical support representative for an update.");
msgdef_ddts_component("");


msgdef(BADVERSIONTCP, RSRB, LOG_ERR, 0,
       "%s: %d/%i: version mismatch, mine %d, theirs %d");
msgdef_explanation(
	"The remote end of a TCP remote peer is running the wrong version of
	the system software. Either the local end, the remote end, or both are
	not up to date.");
msgdef_recommended_action(
	"Call your technical support representative for an update.");
msgdef_ddts_component("");


msgdef(IFERR, RSRB, LOG_ERR, 0, "%s: %s: %s, op %x, len %d, trn %d");
msgdef_explanation(
	"The remote end of a direct serial RSRB connection detected a
	configuration problem or traffic that is not recognized by the
	configuration.");
msgdef_recommended_action(
	"Examine the configuration on both sides of the serial connection for
	possible problems. Examine the traffic being offered for propagation
	with respect to the configuration. The destination target ring is
	denoted by the value of trn.");
msgdef_ddts_component("");


msgdef(SENDPUNTIF, RSRB, LOG_ERR, 0, "%s: sent %s to %s");
msgdef_explanation(
	"The local end of a direct serial RSRB connection detected a
	configuration problem or traffic that is not recognized by the
	configuration.");
msgdef_recommended_action(
	"Examine the configuration on both sides of the serial connection for
	possible problems. Examine the traffic being offered for propagation
	with respect to the configuration.");
msgdef_ddts_component("");


msgdef(FSTERR, RSRB, LOG_ERR, 0, "%s: %s: %i, op %x, len %d, trn %d");
msgdef_explanation(
	"The remote end of a direct serial RSRB connection detected a
	configuration problem or traffic that is not recognized by the
	configuration.");
msgdef_recommended_action(
	"Examine the configuration on both sides of the serial connection for
	possible problems. Examine the traffic being offered for propagation
	with respect to the configuration. The destination target ring is
	denoted by the value of trn.");
msgdef_ddts_component("");


msgdef(SENDPUNTFST, RSRB, LOG_ERR, 0, "%s: sent %s to %s");
msgdef_explanation(
	"The local end of a direct serial RSRB connection detected a
	configuration problem or traffic that is not recognized by the
	configuration.");
msgdef_recommended_action(
	"Examine the configuration on both sides of the serial connection for
	possible problems. Examine the traffic being offered for propagation
	with respect to the configuration.");
msgdef_ddts_component("");


msgdef(BADLENIP, RSRB, LOG_WARNING, 0,
       "Peer %d/%i, %s, bad length %d, trn %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If either message recurs, call your technical support representative
	for assistance.");
msgdef_ddts_component("");


msgdef(ILLPEER, RSRB, LOG_WARNING, 0,
       "Peer %s [%-08x], illegal state %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If either of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(CONNILLSTATE, RSRB, LOG_WARNING, 0,
       "Peer %s, CONN, illegal state %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(CONIPST, RSRB, LOG_WARNING, 0,
       "Peer %d/%i, CONN, illegal state %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(HDRNOVRP, RSRB, LOG_ERR, 0, "Peer %i, HDR, no vrp");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(HDRVRP, RSRB, LOG_ERR, 0,
       "Peer %d/%i, HDR, vrp state wrong, %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(HDRRECV, RSRB, LOG_WARNING, 0,
       "Peer %d/%i, HDR, recv state invalid, not empty %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef_limit(NOMEMORY, RSRB, LOG_ERR, 0,
	     "Unit %d, no memory for %s", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"The requested operation could not be accomplished because of a low
	memory condition.");
msgdef_recommended_action(
	"Reduce other system activity to ease memory demands. If conditions
	warrant, upgrade to a larger memory configuration.");
msgdef_ddts_component("");


msgdef(LOCAL, RSRB, LOG_WARNING, 0,
       "Unit %d, local/vring set simultaneously, vrn %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If either of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(NOTREM, RSRB, LOG_ERR, 0, "Null idb and not remote");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(BADVRE, RSRB, LOG_WARNING, 0, "Bad vre type");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(RNGXFAIL, RSRB, LOG_WARNING, 0, "Ring exchange failure, resetting peer: %s");
msgdef_explanation(
	"The RSRB peer initiation logic failed due to a memory shortage or
	congestion condition.");
msgdef_recommended_action(
	"The problem should clear and the peers should re-open without operator
	intervention.");
msgdef_ddts_component("");
