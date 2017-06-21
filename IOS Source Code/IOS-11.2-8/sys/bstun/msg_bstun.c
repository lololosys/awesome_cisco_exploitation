/* $Id: msg_bstun.c,v 3.2.60.3 1996/09/13 23:45:55 hampton Exp $
 * $Source: /release/112/cvs/Xsys/bstun/msg_bstun.c,v $
 *------------------------------------------------------------------
 * msg_bstun - BSTUN messages
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_bstun.c,v $
 * Revision 3.2.60.3  1996/09/13  23:45:55  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/08/28  12:37:55  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.60.1  1996/05/17  10:41:11  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/11  08:19:53  jbalestr
 * CSCdi51807:  need a test command to transmit raw bisync frames
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.48.1  1996/04/03  13:50:56  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2  1995/11/17  08:49:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/03  01:04:11  arothwel
 * CSCdi39397:  Fix:
 *
 *         o Add specific poll support for local-ack pollee.
 *         o Remove superfluous fields from lcb.
 *         o Generalize protocol main handler.
 *         o Finish off replacing all deprecated process mgnt calls.
 *         o Multiple poll-adds causing hard-on polling.
 *         o Add safety code to ignore 'spurious' Tx completes.
 *
 * Revision 2.2  1995/06/18  21:27:12  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:14:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(BSTUN);				/* Define the BSTUN facility */
msgdef_section("Block serial tunneling error messages");


msgdef(PEERSTATE, BSTUN, LOG_WARNING, 0, "Peer %s, wrong state %d (%d)");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(BADMAGIC, BSTUN, LOG_ERR, 0, 
       "%s: wrong magic, mine %x, theirs %x (%d)");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(BADMAGICTCP, BSTUN, LOG_ERR, 0,
       "%s: peer %s, wrong magic, mine %x, theirs %x");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(BADLENOP, BSTUN, LOG_ERR, 0,
       "%s: bad len or unknown op, op %d, len %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(TCPPEERSHUT, BSTUN, LOG_INFO, 0, "%s %s, %i(%d)");
msgdef_explanation(
	"This route closed a BSTUN connection with a remote peer.");
msgdef_recommended_action(
	"Examine this router to see why it closed this connection with this
	peer. This can be caused by normal events, such as reconfiguration.");
msgdef_ddts_component("");


msgdef(CONNILLSTATE, BSTUN, LOG_ERR, 0, "CONN: Peer %s, illegal state %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(BADCONN, BSTUN, LOG_ERR, 0, "CONN: bad connection (%d), peer: %s");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(ERR, BSTUN, LOG_INFO, 0, "%s: %s: %s, op %x, len %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If either message recurs, call your technical support representative
	for assistance.");
msgdef_ddts_component("");


msgdef(SENDPUNT, BSTUN, LOG_ERR, 0, "%s: sent %s to %s");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If either message recurs, call your technical support representative
	for assistance.");
msgdef_ddts_component("");


msgdef(SENDPUNTTCP, BSTUN, LOG_ERR, 0, "%s: sent %s to ([%u])%i");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If either message recurs, call your technical support representative
	for assistance.");
msgdef_ddts_component("");


msgdef(OPENING, BSTUN, LOG_INFO, 0, "CONN: opening peer %s, %d");
msgdef_explanation(
	"A connection attempt to a remote peer completed successfully (OPENED,
	PASSIVE-OPEN) or is in the process of being opened (OPENING). This is
	good, expected behavior.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef(CONNOPENFAIL, BSTUN, LOG_INFO, 0, "CONN: peer %s open failed, %s [%u]");
msgdef_explanation(
	"An attempt to connect to a remote TCP BSTUN peer failed.");
msgdef_recommended_action(
	"Verify that the remote peer is accessible from this router, that it is
	running software capable of supporting BSTUN, and that it is configured
	correctly.");
msgdef_ddts_component("");


msgdef(BADPASSIVEOPEN, BSTUN, LOG_ERR, 0, 
       "passive open from %i(%d) -> %d failed");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If either message recurs, call your technical support representative
	for assistance.");
msgdef_ddts_component("");


msgdef(PASSIVEOPEN, BSTUN, LOG_INFO, 0, "passive open %i(%d) -> %d");
msgdef_explanation(
	"A connection attempt to a remote peer completed successfully (OPENED,
	PASSIVE-OPEN) or is in the process of being opened (OPENING). This is
	good, expected behavior.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef(TCPFINI, BSTUN, LOG_INFO, 0, "peer %s closed [previous state %s]");
msgdef_explanation(
	"A remote peer closed a BSTUN connection with this router.");
msgdef_recommended_action(
	"Examine the other router to see why it closed this connection with
	this peer. This can be caused by normal events, such as
	reconfiguration.");
msgdef_ddts_component("");


msgdef(PEERSHUTDOWN, BSTUN, LOG_INFO, 0, "shutting down peer %s on %s");
msgdef_explanation(
	"A connection to a remote peer is being shut down. This is typically
	the result of user intervention in BSTUN reconfiguration or disabling.
	This is good, expected behavior.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef(OPENED, BSTUN, LOG_INFO, 0, "%s: peer %s opened, [previous state %s]");
msgdef_explanation(
	"A connection attempt to a remote peer completed successfully (OPENED,
	PASSIVE-OPEN) or is in the process of being opened (OPENING). This is
	good, expected behavior.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef(RECONNECT, BSTUN, LOG_INFO, 0, "PHDR: reconnect from peer %s");
msgdef_explanation(
	"A remote peer reestablished a connection to this router.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef(NOBUF, BSTUN, LOG_CRIT, 0, "Interface %s, no buffer available to %s");
msgdef_explanation(
	"A memory shortage existed at the time that the configuration command
	was issued. This condition is rare and, under normal conditions,
	temporary.");
msgdef_recommended_action(
	"Reconfigure the BSTUN group. If memory shortages persist, call your
	technical support representative for assistance.");
msgdef_ddts_component("");


msgdef(NOGRPDEF, BSTUN, LOG_ERR, 0, "No bstun group defined on interface %s");
msgdef_explanation(
	"An action was taken that requires the previous definition of a BSTUN
	group number.");
msgdef_recommended_action(
	"Make sure "encapsulation bstun" and the "bstun group" have been
	defined for the interface.");
msgdef_ddts_component("");


msgdef(UNKPROTOEV, BSTUN, LOG_ERR, 0, 
       "Unknown protocol event %d on interface %s");
msgdef_explanation(
	"An event passed to the BSTUN layer for processing could not
	be decoded.");
msgdef_recommended_action(
	"Perform a hard restart on the indicated interface.");
msgdef_ddts_component("");


msgdef(PROCEXIT, BSTUN, LOG_INFO, 0,
       "Process terminating. Due to signal %d. Subcode %d. Address %x.");
msgdef_explanation(
	"The BSTUN background process is terminating.");
msgdef_recommended_action(
	"If this message appears while you are unconfiguring BSTUN, no action
	is required. Otherwise, record all the displayed values and any other
	BSTUN error messages, and contact your technical support
	representative.");
msgdef_ddts_component("");
