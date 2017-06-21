/* $Id: msg_oir.c,v 3.2.56.3 1996/09/13 23:46:38 hampton Exp $
 * $Source: /release/112/cvs/Xsys/hes/msg_oir.c,v $
 *------------------------------------------------------------------
 * msg_oir.c - Online Insertion/Removal message facility
 *
 * October 1992, William H. Palmer
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: msg_oir.c,v $
 * Revision 3.2.56.3  1996/09/13  23:46:38  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.56.2  1996/08/28  12:45:49  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.56.1  1996/05/28  20:36:49  shaker
 * CSCdi58549:  Notify users of non EOIR capable hardware
 * Branch: California_branch
 * Let users know why we're resetting the interface.
 *
 * Revision 3.2  1995/11/17  09:19:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:41:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:43:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(OIR);				/* define the OIR facility */

msgdef_section("Online insertion and removal error messages");


msgdef(REMCARD, OIR, LOG_INFO, 0,
       "Card removed from slot %d, interfaces disabled");
msgdef_explanation(
	"The OIR facility detected the removal of a processor from slot [n].
	The interfaces on that processor will be administratively shut down and
	marked as removed. The routing table will be flushed of any routes
	through the removed interfaces.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef(INSCARD, OIR, LOG_INFO, 0,
       "Card inserted in slot %d, interfaces administratively shut down");
msgdef_explanation(
	"The OIR facility detected a newly inserted processor in slot dec. The
	inserted interface will be shut down until it is configured by the user
	or, if an interface of that type was previously configured, it will be
	restored to its previous state.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef(LONGSTALL, OIR, LOG_ERR, 0,
       "Long bus stall (%u ms), check for improperly seated cards");
msgdef_explanation(
	"Something is preventing the CxBus or CyBus from operating. The usual
	reason is an improperly seated interface processor.");
msgdef_recommended_action(
	"Make sure all interface processor cards are firmly seated. If the
	problem reoccurs, it may indicate a hardware failure. Copy the error
	message exactly as it appears, and report it to your technical support
	representative.");
msgdef_ddts_component("");


msgdef(NOEOIR, OIR, LOG_INFO, 0,
       "%s %s version %u.%u not capable of EOIR");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(SEATED, OIR, LOG_ERR, 0,
       "Insert/removal failed (slot %d), check card seating");
msgdef_explanation(
	"The OIR facility detected an incorrectly seated card in
        slot [n], causing the insertion or removal of this card to fail.");
msgdef_recommended_action(
	"Reseat the card in the indicated slot.");
msgdef_ddts_component("");


