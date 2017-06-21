/* $Id: msg_clns.c,v 3.5.12.4 1996/09/13 23:45:57 hampton Exp $
 * $Source: /release/112/cvs/Xsys/clns/msg_clns.c,v $
 *------------------------------------------------------------------
 * msg_clns.c - Message file for clns facility
 *
 * July 17, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_clns.c,v $
 * Revision 3.5.12.4  1996/09/13  23:45:57  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.5.12.3  1996/08/28  12:39:01  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.12.2  1996/05/10  19:45:50  dkatz
 * CSCdi57308:  ISIS could detect duplicate system IDs
 * Branch: California_branch
 * Instrument non-pseudonode LSP churn, and use this as a heuristic to
 * detect duplicate System IDs (at long last!).
 *
 * Revision 3.5.12.1  1996/05/07  17:19:02  dkatz
 * CSCdi55147:  NLSP doesnt detect duplicate system IDs
 * Branch: California_branch
 * Detect duplicates in LSPs and IIHs.  Back off the irritating messages
 * to once per minute.
 *
 * Revision 3.5  1996/02/20  18:06:48  dkatz
 * CSCdi47916:  ISIS/NLSP LSP checksum errors
 * Improve diagnostics.  Ignore errors if the holdtime is zero
 * (workaround).
 *
 * Revision 3.4  1995/12/01  17:48:56  dkatz
 * CSCdi44093:  ISIS crashes on purpose
 *
 * Revision 3.3  1995/12/01  02:48:39  dkatz
 * CSCdi39906:  CPUHOG Process = IS-IS Update
 *
 * Revision 3.2  1995/11/17  08:54:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:10:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/09/23  22:13:52  dkatz
 * CSCdi39388:  ISIS validation too harsh
 * Crank out an error message with all available info and try not to crash.
 *
 * Revision 2.4  1995/08/25  05:40:03  dkatz
 * CSCdi38138:  ISIS needs adjacency state tracing
 *
 * Revision 2.3  1995/08/23  18:03:29  dkatz
 * CSCdi37692:  LSP checksum errors can melt network
 * Add a knob to discard incoming LSPs with checksum errors, rather
 * than purging them.
 *
 * Add additional debugging capabilities.
 *
 * Revision 2.2  1995/08/23  17:22:16  dkatz
 * CSCdi37612:  Received ESH flaps ISIS adjacency
 * Ignore received ESH if an ISIS adjacency is present.  Don't
 * syslog encaps failures.
 *
 * Revision 2.1  1995/06/07  20:19:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(CLNS);
msgdef_section("OSI Connectionless Network Service error messages");


msgdef_limit(LSPCKSUM, CLNS, LOG_WARNING, 0,
	     "%s: Bad L%d LSP %s checksum %04x received on %s, length %d, "
	     "should be %04x, bad count %d", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"A link-state packet (LSP) was received for either
	Intermediate System-to-Intermediate System (ISIS) or NetWare
	Link Services Protocol (NLSP), which failed an internal data
	checksum, but which passed the data-link checksum. The
	interface on which the LSP was received is indicated, along
	with the total count of checksum errors received, the LSP ID,
	and the level of the failing LSP. This message usually
	indicates data corruption occurring on a link, but might also
	indicate data corruption in memory in a neighboring router.");
msgdef_recommended_action(
	"If the error is in a neighboring router's memory, the problem
	will correct itself within 15 minutes. That router will detect
	its internal error and regenerate the corrupted data. This
	case is usually indicated by a single LSP ID appearing
	repeatedly in these messages. The same LSP ID might appear in
	this message on other routers as well.

	If data-link corruption has caused the error, check the link
	for errors and deal with them appropriately. This case is
	usually indicated by a number of different LSP IDs appearing
	in these messages.");
msgdef_ddts_component("");


msgdef(REDIRECT, CLNS, LOG_WARNING, 0,
       "Redirect found for non-route entry, dst= %s, next-hop= %s");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If the message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(NSAPIS, CLNS, LOG_WARNING, 0, "Invalid NSAP type in IS table: %x %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If the message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(EDATFAIL, CLNS, LOG_WARNING, 0, "Encapsulation failed, dst= %s");
msgdef_explanation(
	"This message can occur when an interface is down and a static
	neighbor entry exists in the system's CLNS routing table. If this is
	not the case, the message indicates an internal software error.");
msgdef_recommended_action(
	"Check the interface. If the interface is not down and no static
	neighbor entry exists for that interface, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(NSAPES, CLNS, LOG_ERR, MSG_TRACEBACK,
       "Invalid NSAP type in ES table: %x for %s");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If the message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(AREALOST, CLNS, LOG_WARNING, 0,
       "Too many IS-IS areas--can\'t route to area %s");
msgdef_explanation(
	"Intermediate System-to-Intermediate system (IS-IS) allows only three
	area addresses per area. Multiple routers with at least one area
	address in common merge all of their configured area addresses into a
	single set. If the resulting set includes more than three area
	addresses, this message appears and indicates that there will
	be a loss of connectivity to the specified area.");
msgdef_recommended_action(
	"Reconfigure the network so that the area has fewer area addresses, or
	split the area into multiple areas.");
msgdef_ddts_component("");


msgdef_limit(BADPACKET, CLNS, LOG_WARNING, 0,
	     "%s: %s, %s%s from %s (%s)", MSGDEF_LIMIT_GLACIAL);
msgdef_explanation(
	"An invalid packet was received from a neighboring router.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(CKSUMERR, CLNS, LOG_WARNING, 0,
	     "%s: L%d LSP %s database checksum error, restarting",
	     MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(LSPFULL, CLNS, LOG_WARNING, 0,
	     "%s: L%d LSP filled to capacity, dropping routes",
	     10*ONEMIN);
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(LSPTOOBIG, CLNS, LOG_WARNING, 0,
	     "%s: L%d LSP %s size %d bigger than interface %s MTU %d, "
	     "LSP dropped", 10*ONEMIN);
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(ADJCHANGE, CLNS, LOG_NOTICE, 0, "%s: Adjacency to %s (%s) %s, %s");
msgdef_explanation(
	"An Intermediate System-to-Intermediate System (ISIS) or NetWare Link
	Services Protocol (NLSP) adjacency with a neighboring router has either
	come up or gone down. The message describes the change and the reason
	for it. This message appears only if log-adjacency-changes command is
	configured for the ISIS or NLSP process.");
msgdef_recommended_action(
	"This message is primarily informational and appears normally as
	routers and links go up or down. Unexpected adjacency loss might
	indicate high error rates or high packet loss in the network and should
	be investigated.");
msgdef_ddts_component("");


msgdef_limit(BKUPERR, CLNS, LOG_ALERT, MSG_TRACEBACK,
	     "ISIS: %s in %#x, ndb_next %#x, ndb_prev %#x, lsp_next %#x, "
	     "lsp_prev %#x, metric %#x, index %d, %s %#x",
	     MSGDEF_LIMIT_MEDIUM);
msgdef_explanation(
	"An internal error occurred in the maintenance of the Intermediate
	System-to-Intermediate System (ISIS) backup route table.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears and copy down the
	router's configuration along with any other relevant information.
	Contact your technical support representative for assistance.");
msgdef_ddts_component("");


msgdef_limit(LINKERR, CLNS, LOG_ALERT, MSG_TRACEBACK,
	     "ISIS: %s in %#x, lsp_next %#x, lsp_prev %#x, index %d, ver %d, "
	     "%s %#x", MSGDEF_LIMIT_MEDIUM);
msgdef_explanation(
	"An internal error occurred in the maintenance of Intermediate
	System-to-Intermediate System (ISIS) or NetWare Link Services Protocol
	(NLSP) link-state packet (LSP) linkages.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears and copy down the
	router's configuration along with any other relevant information.
	Contact your technical support representative for assistance.");
msgdef_ddts_component("");


msgdef_limit(TENT, CLNS, LOG_WARNING, MSG_TRACEBACK, "%s: %s",
	     MSGDEF_LIMIT_MEDIUM);
msgdef_explanation(
	"An internal error occurred in the Intermediate System-to-Intermediate
	System (ISIS) or NetWare Link Services Protocol (NLSP).");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(BADIFNUM, CLNS, LOG_ERR, MSG_TRACEBACK,
	     "ISIS: Bad if number, pdb %#x, idb %#x, if %#x",
	     MSGDEF_LIMIT_MEDIUM);
msgdef_explanation(
	"An internal error occurred in the Intermediate System-to-Intermediate
	System (ISIS) or NetWare Link Services Protocol (NLSP). An incon-
	sistency was detected in an internal data structure related to
	tracking protocol/interface state, possibly related to enable/
	disable of the protocol on an interface.")
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
	to your technical support representative. If possible, include
	a current configuration, and any information on configuration
	commands recently issued related to ISIS and/or NLSP.");
msgdef_ddts_component("");
msgdef_tac_details(
	"An interface bit mask structure is associated with each
	instance of ISIS or NLSP. This mask tracks interface state,
	specificially whether the interface is up and this protocol
	instance is configured. The mask is the starting point for
	other bit mask operations (for example, what SRM bits should
	be set, given an LSP from this instance that must be flooded).
	More specifically, there is no point in flooding out a down
	interface or out an interface not configured for this instance.
	The LSP will have set only those SRM bits which are set in the
	interface bit mask for the instance. (Note however, that there
	may also be additional reasons to clear LSP SRM bits.)
	
	This error represents a situation where a request has been made
	to set or clear the interface mask bit for a particular protocol
	instance. However, the data structure for the interface (if) is
	inconsistent. Usually this is because the isis_if_number parameter
	is set to -1, indicating that the interface has been or is being
	deallocated by the protocol instance (for example, the \"no ipx
	nlsp enable\" command was issued for the interface). Note that
	the bit operation is ignored and a crash or memory corruption is
	prevented (depending on the platform).

	Report the error log message together with its traceback so that
	development engineering can attempt to find and fix the underlying
	code logic problem.

	Use Topic to search for a similar DDTS. If you find none, write a
	DDTS for this problem.");


msgdef_limit(LSPDB, CLNS, LOG_ERR, MSG_TRACEBACK,
	     "%s: Inconsistent LSPIX entry, lsp %#x, index %d, ixentry %#x, "
	     "ixentry->lsp %#x", MSGDEF_LIMIT_MEDIUM);
msgdef_explanation(
	"An internal error occurred in the Intermediate System-to-Intermediate
	System (ISIS) or NetWare Link Services Protocol (NLSP).");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(LSPCHAIN, CLNS, LOG_ERR, MSG_TRACEBACK,
	     "%s: non-null %s pointer %#x on freed LSP, index %d",
	     MSGDEF_LIMIT_MEDIUM);
msgdef_explanation(
	"An internal error occurred in the Intermediate System-to-Intermediate
	System (ISIS) or NetWare Link Services Protocol (NLSP).");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(LSPIXARRNULL, CLNS, LOG_ERR, MSG_TRACEBACK,
	     "%s: LSPix entry %d should be null, is %#x", MSGDEF_LIMIT_MEDIUM);
msgdef_explanation(
	"An internal error occurred in the Intermediate System-to-Intermediate
	System (ISIS) or NetWare Link Services Protocol (NLSP).");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(LSPIXARRMATCH, CLNS, LOG_ERR, MSG_TRACEBACK,
	     "%s: LSPix entry %d mismatch with LSP %#x, LSP %s is %d, "
	     "entry %d", MSGDEF_LIMIT_MEDIUM);
msgdef_explanation(
	"An internal error occurred in the Intermediate System-to-Intermediate
	System (ISIS) or NetWare Link Services Protocol (NLSP).");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(LSPIXHEAD, CLNS, LOG_ERR, MSG_TRACEBACK,
	     "%s: LSPix %s %d head %#x doesn't match first prev %#x",
	     MSGDEF_LIMIT_MEDIUM);
msgdef_explanation(
	"An internal error occurred in the Intermediate System-to-Intermediate
	System (ISIS) or NetWare Link Services Protocol (NLSP).");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(ONLIST, CLNS, LOG_ERR, MSG_TRACEBACK,
	     "ISIS: entry already threaded, prev %#x, next %#x, index %d, "
	     "ver %d", MSGDEF_LIMIT_MEDIUM);
msgdef_explanation(
	"An internal error occurred in the Intermediate System-to-Intermediate
	System (ISIS) or NetWare Link Services Protocol (NLSP).");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(DUPSYSTEM, CLNS, LOG_WARNING, 0,
	     "%s: possible duplicate system ID %s detected",
	     MSGDEF_LIMIT_GLACIAL);
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
