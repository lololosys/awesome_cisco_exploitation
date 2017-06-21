/* $Id: msg_datalink.c,v 3.3.60.2 1996/09/13 23:49:25 hampton Exp $
 * $Source: /release/112/cvs/Xsys/if/msg_datalink.c,v $
 *------------------------------------------------------------------
 * msg_datalink.c - Message file for datalink facility
 *
 * July 17, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_datalink.c,v $
 * Revision 3.3.60.2  1996/09/13  23:49:25  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.60.1  1996/08/28  12:48:17  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3  1995/11/17  09:30:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:38:22  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:50:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/19  16:13:23  ahh
 * CSCdi37281:  NULL dereference when compression not in image
 * Print the name of the proper interface and rate-limit the error message.
 *
 * Revision 2.1  1995/06/07  20:53:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"
#include "../os/msgtxt_sys.h"


facdef(LINK);				/* Define datalink facility */
msgdef_section("Data link error messages");


msgdef(NOMAC, LINK, LOG_WARNING, 0, 
       "A random default MAC address of %e has\n"
       "  been chosen.  Ensure that this address is unique, or specify MAC\n"
       "  addresses for commands (such as 'novell routing') that allow the\n"
       "  use of this address as a default.");
msgdef_explanation(
	"An addressing inconsistency occurred.");
msgdef_recommended_action(
	"Make sure that this address is unique, or specify MAC addresses for
	commands such as novell routing that allow the use of this address as a
	default.");
msgdef_ddts_component("");


msgdef(NOSOURCE, LINK, LOG_CRIT, MSG_TRACEBACK|MSG_PROCESS,
       "Source idb not set");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(LINEST, LINK, LOG_CRIT, MSG_TRACEBACK|MSG_PROCESS,
       "No linestate vector for %s");
msgdef_explanation(
	"An internal software inconsistency occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(CHANGED, LINK, LOG_NOTICE, 0, "Interface %s, changed state to %s");
msgdef_explanation(
	"The interface hardware changed state.");
msgdef_recommended_action(
	"Notification message only. No action required.");
msgdef_ddts_component("");


msgdef(UPDOWN, LINK, LOG_ERR, 0, "Interface %s, changed state to %s");
msgdef_explanation(
	"The interface hardware went either up or down.");
msgdef_recommended_action(
	"If the state change was unexpected, confirm the configuration settings
	for the interface.");
msgdef_ddts_component("");


msgdef_limit(TOOBIG, LINK, LOG_ERR, MSG_TRACEBACK,
	     "Interface %s, Output packet size of %d bytes too big",
	     MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(BADMACREG, LINK, LOG_ERR, MSG_TRACEBACK|MSG_PROCESS,
       "Interface %s, non-existent MACADDR registry for link %d");
msgdef_explanation(
	"An attempt to map a network level address to a MAC level address
	failed.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(BADENCAP, LINK, LOG_ERR, MSG_TRACEBACK|MSG_PROCESS,
	     "Interface %s, Bad encapsulation code (%d)",
	     MSGDEF_LIMIT_GLACIAL);
msgdef_explanation(
	"The lower level software was unable to write a MAC header for a
	datagram. A probable cause is configuration error.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADVCALL, LINK, LOG_CRIT, MSG_TRACEBACK|MSG_PROCESS,
       "Interface %s, undefined entry point");
msgdef_explanation(
	"An internal software error occurred. The high-level system code tried
	to use an unimplemented entry point with the virtual interface
	descriptor block (IDB) driver.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(BOOTP, LINK, LOG_NOTICE, 0, "%s address %i, resolved by %i");
msgdef_explanation(
	"An interface's IP address was successfully learned dynamically through
	BootP. The first address is the learned IP address. The second IP
	address is the IP address of the BootP server that provided the
	information.");
msgdef_recommended_action(
	"No action required.");
msgdef_ddts_component("");


msgdef(RARP,  LINK, LOG_NOTICE, 0, "%s address %i, resolved by %i");
msgdef_explanation(
	"The Reverse Address Resolution Protocol (RARP) resolved an IP
	address.");
msgdef_recommended_action(
	"Notification message only. No action required.");
msgdef_ddts_component("");


msgdef(SLARP, LINK, LOG_NOTICE, 0, "%s address %i, resolved by %i");
msgdef_explanation(
	"The Serial Line Address Resolution Protocol (SLARP) resolved an IP
	address.");
msgdef_recommended_action(
	"Notification message only. No action required.");
msgdef_ddts_component("");


msgdef(FDDISTAT, LINK, LOG_WARNING, 0,
       "Interface %s, FDDI state %s detected");
msgdef_explanation(
	"There was a state change on the FDDI ring.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BRIDGECONFLICT, LINK, LOG_WARNING, 0,
       "Interface %s, encapsulated BPDU recvd from %e");
msgdef_explanation(
	"An FCIT running in nonencapsulating transparent mode detected an FDDI
	bridge running in encapsulation mode on the same fiber. This is an
	unstable situation and should be corrected. The incoming interface is
	displayed, along with the FDDI MAC address in standard form.");
msgdef_recommended_action(
	"Upgrade the errant interface to full transparent mode. Copy the error
	message exactly as it appears, and report it to your technical support
	representative.");
msgdef_ddts_component("");


msgdef(REMLOOP, LINK, LOG_ERR, 0, "Interface %s, remote loop %s %s");
msgdef_explanation(
	"The interface entered or exited a remote loopback.");
msgdef_recommended_action(
	"Notification message only. No action is required.");
msgdef_ddts_component("");


msgdef_limit(COMPREG, LINK, LOG_ERR, 0,
	     "Interface %s, non-existent (de)compression registry. "
	     "Packet dropped", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BOGUSENCAP, LINK, LOG_ERR, 0, msgtxt_badencap);
msgdef_explanation(
	"A serial interface has been configured with an unknown
	encapsulation.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


/*
 * Error messages for IDB list insertion, creation and destruction errors.
 */
msgdef(IDBLISINS, LINK, LOG_ERR, MSG_TRACEBACK|MSG_PROCESS,
       "Not able to insert interface %s into %s list");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(IDBLISCRE, LINK, LOG_ERR, MSG_TRACEBACK|MSG_PROCESS,
       "Unable to create %s interface list");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
