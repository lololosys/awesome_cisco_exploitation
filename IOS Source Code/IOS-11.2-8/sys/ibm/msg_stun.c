/* $Id: msg_stun.c,v 3.2.60.3 1996/09/13 23:49:13 hampton Exp $
 * $Source: /release/112/cvs/Xsys/ibm/msg_stun.c,v $
 *------------------------------------------------------------------
 * msg_stun.c - Message file for sdlc facility
 *
 * July 17, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_stun.c,v $
 * Revision 3.2.60.3  1996/09/13  23:49:13  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/08/28  12:46:55  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.60.1  1996/05/17  11:20:45  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/11  14:37:48  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.48.1  1996/04/03  14:34:07  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2  1995/11/17  09:23:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:45:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/18  23:36:42  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:47:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(STUN);				/* Define the STUN facility */
msgdef_section("Serial tunneling error messages");


msgdef(PEERSTATE, STUN, LOG_WARNING, 0, "Peer %s, wrong state %d (%d)");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(BADMAGIC, STUN, LOG_ERR, 0, "%s: wrong magic, mine %x, theirs %x (%d)");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(BADMAGICTCP, STUN, LOG_ERR, 0,
       "%s: peer %s, wrong magic, mine %x, theirs %x");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(BADLENOP, STUN, LOG_ERR, 0,
       "%s: bad len or unknown op, op %d, len %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(TCPPEERSHUT, STUN, LOG_INFO, 0, "%s %s, %i(%d)");
msgdef_explanation(
	"This route closed a STUN connection with a remote peer.");
msgdef_recommended_action(
	"Examine this router to see why it closed this connection with this
	peer. (This can be caused by normal events, such as
	reconfiguration.)");
msgdef_ddts_component("");


msgdef(CONNILLSTATE, STUN, LOG_ERR, 0, "CONN: Peer %s, illegal state %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(BADCONN, STUN, LOG_ERR, 0, "CONN: bad connection (%d), peer: %s");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(ERR, STUN, LOG_INFO, 0, "%s: %s: %s, op %x, len %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(SENDPUNT, STUN, LOG_ERR, 0, "%s: sent %s to %s");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If either of these messages recurs, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(SENDPUNTTCP, STUN, LOG_ERR, 0, "%s: sent %s to ([%u])%i");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If either of these messages recurs, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(OPENING, STUN, LOG_INFO, 0, "CONN: opening peer %s, %d");
msgdef_explanation(
	"A connection attempt to a remote peer completed successfully (OPENED,
	PASSIVE-OPEN) or is in the process of being opened (OPENING). This is
	good, expected behavior.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef(CONNOPENFAIL, STUN, LOG_INFO, 0, "CONN: peer %s open failed, %s [%u]");
msgdef_explanation(
	"An attempt to connect to a remote TCP STUN peer failed.");
msgdef_recommended_action(
	"Verify that the remote peer is accessible from this router, that it is
	running software capable of supporting STUN, and that it is configured
	correctly.");
msgdef_ddts_component("");


msgdef(BADPASSIVEOPEN, STUN, LOG_ERR, 0,
       "passive open from %i(%d) -> %d failed");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(PASSIVEOPEN, STUN, LOG_INFO, 0, "passive open %i(%d) -> %d");
msgdef_explanation(
	"A connection attempt to a remote peer completed successfully (OPENED,
	PASSIVE-OPEN) or is in the process of being opened (OPENING). This is
	good, expected behavior.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef(TCPFINI, STUN, LOG_INFO, 0, "peer %s closed [previous state %s]");
msgdef_explanation(
	"A remote peer closed a STUN connection with this router.");
msgdef_recommended_action(
	"Examine the other router to see why it closed this connection with
	this peer. (This can be caused by normal events, such as
	reconfiguration.)");
msgdef_ddts_component("");


msgdef(PEERSHUTDOWN, STUN, LOG_INFO, 0, "shutting down peer %s on %s");
msgdef_explanation(
	"A connection to a remote peer is being shut down. This is typically
	the result of user intervention in STUN reconfiguration or disabling.
	This is good, expected behavior.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef(OPENED, STUN, LOG_INFO, 0, "%s: peer %s opened, [previous state %s]");
msgdef_explanation(
	"A connection attempt to a remote peer completed successfully (OPENED,
	PASSIVE-OPEN) or is in the process of being opened (OPENING). This is
	good, expected behavior.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef_limit(NOINPIDB, STUN, LOG_ERR, MSG_TRACEBACK, "Input idb not set", 10);
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(RECONNECT, STUN, LOG_INFO, 0, "PHDR: reconnect from peer %s");
msgdef_explanation(
	"A remote peer reestablished a connection to this router.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef(NOBUF, STUN, LOG_CRIT, 0, "Interface %s, no buffer available to %s");
msgdef_explanation(
	"A memory shortage existed at the time that the configuration command
	was issued. This condition is rare and, under normal conditions,
	temporary.");
msgdef_recommended_action(
	"Reconfigure the STUN group. If memory shortages persist, call your
	technical support representative for assistance.");
msgdef_ddts_component("");


msgdef(NOTGI, STUN, LOG_CRIT, 0, "Please remove and redefine protocol group %d");
msgdef_explanation(
	"An internal error occurred. The configuration is irregular.");
msgdef_recommended_action(
	"Remove and reconfigure the STUN protocol group. Record the
	configuration and call your technical support representative for
	assistance.");
msgdef_ddts_component("");
