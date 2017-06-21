/* $Id: msg_isdn.c,v 3.1.68.2 1996/09/13 23:50:12 hampton Exp $
 * $Source: /release/112/cvs/Xsys/isdn/msg_isdn.c,v $
 *------------------------------------------------------------------
 * msg_isdn.c - Message file for isdn facility
 *
 * August 9, 1995, Bibek Das
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_isdn.c,v $
 * Revision 3.1.68.2  1996/09/13  23:50:12  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.68.1  1996/08/28  12:54:11  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1  1995/11/09  12:14:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/21  07:32:37  bdas
 * CSCdi38453:  ISDN traps need corresponding syslog messages
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"
#include "../os/msgtxt_sys.h"

facdef(ISDN);
msgdef_section("Integrated Services Digital Network (ISDN) error messages");


msgdef(CONNECT, ISDN, LOG_INFO, 0, 
       "Interface %s is now connected to %s %s");
msgdef_explanation(
	"This is an informational message. It is sent when a call is connected.
	The message displays the remote name and number, which it receives from
	the Point-to-Point Protocol (PPP) authentication or from where the
	dialer placed the call to.");
msgdef_recommended_action(
	"No action is required.");
msgdef_ddts_component("");


msgdef(DISCONNECT, ISDN, LOG_INFO, 0,
       "Interface %s  disconnected from %s %s, call lasted %u seconds");
msgdef_explanation(
	"This is an informational message. It is sent when a call is
	disconnected. The message displays the remote name and number, which it
	receives from the Point-to-Point Protocol (PPP) authentication or from
	where the dialer placed the call to. It also shows how long the call
	lasted.");
msgdef_recommended_action(
	"No action is required.");
msgdef_ddts_component("");

msgdef(NOMEMORY, ISDN, LOG_ALERT, 0, msgtxt_nomemory);
msgdef_explanation(
        "The requested operation could not be accomplished because of a low
        memory condition.");
msgdef_recommended_action(
        "Reduce other system activity to ease memory demands. If conditions
        warrant, upgrade to a larger memory configuration.");
msgdef_ddts_component("");

msgdef(INVALID_SPID, ISDN, LOG_WARNING, 0, "Interface %s, Spid%d was rejected");
msgdef_explanation(
        "The Spid configured in the router has been rejected by the switch.");
msgdef_recommended_action(
        "Determine correct spid, reenter spid and clear the BRI interface.");
msgdef_ddts_component("");

msgdef(INVALID_CALLEDNUMBER, ISDN, LOG_WARNING, 0,
       "Interface %s, Ignoring call,
        LDN and Called Party Number mismatch");
msgdef_explanation(
        "The incoming ISDN call is ignored because the Called Party Number does
         not match the LDN numbers configured in the router.");
msgdef_recommended_action(
        "Correct the LDN in the router to match the Called Party Number delivered
         in the incoming Setup message.");
msgdef_ddts_component("");

msgdef(INVALID_EID, ISDN, LOG_WARNING, 0,
       "Interface %s, Ignoring call,
        EndPoint Identifier (EID) mismatch");
msgdef_explanation(
        "The incoming ISDN call is ignored because the EID delivered in the
         incoming Setup message cannot be matched.");
msgdef_recommended_action(
        "If EID is delivered in the Setup message, use the isdn debugs and
         show isdn status to determine the current EID values. May have to
         consult the service provider.");
msgdef_ddts_component("");

msgdef(LAYER2UP, ISDN, LOG_INFO, 0,
       "Layer 2 for Interface %s, TEI %u changed to up");
msgdef_explanation(
      "This is an informational message. It is sent when an ISDN Layer2
      logical link is up. It also shows the TEI associated with this
      logical link.");
msgdef_recommended_action(
      "No action is required.");
msgdef_ddts_component("");

msgdef(LAYER2DOWN, ISDN, LOG_INFO, 0,
       "Layer 2 for Interface %s, TEI %u changed to down");
msgdef_explanation(
      "This is an informational message. It is sent when an ISDN Layer2
      logical link is down. It also shows the TEI associated with this
      logical link.");
msgdef_recommended_action(
      "No action is required.");
msgdef_ddts_component("");

