/* $Id: msg_dsx1.c,v 3.3.62.3 1996/09/13 23:50:27 hampton Exp $
 * $Source: /release/112/cvs/Xsys/les/msg_dsx1.c,v $
 *------------------------------------------------------------------
 * msg_dsx1.c -  Message file for an E1/T1 facility.
 *
 * Jan 1995, Manoj Leelanivas
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_dsx1.c,v $
 * Revision 3.3.62.3  1996/09/13  23:50:27  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.62.2  1996/08/28  12:59:30  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.62.1  1996/03/18  20:43:05  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.1  1996/01/24  22:24:49  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  17:42:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:00:28  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:33:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/09  17:43:53  jliu
 * CSCdi35546:  One second update message may lose under heavy loading
 *
 * Revision 2.1  1995/06/07  21:38:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"
#include "../os/msgtxt_sys.h"


facdef(DSX1);
msgdef_section("");


msgdef(NOMEMORY, DSX1, LOG_ALERT, 0, msgtxt_nomemory);
msgdef_explanation(
	"The requested operation could not be accomplished because of a low
	memory condition.");
msgdef_recommended_action(
	"Reduce other system activity to ease memory demands. If conditions
	warrant, upgrade to a larger memory configuration.");
msgdef_ddts_component("");


msgdef(M32_NO_TIMESLOT, DSX1, LOG_ALERT, 0, "Munich 32 no timeslot available");
msgdef_explanation(
        "");
msgdef_recommended_action(
        "");
msgdef_ddts_component("");


msgdef(M32_TIMESLOT_IN_USE, DSX1, LOG_ALERT, 0, "timeslot already configured");
msgdef_explanation(
        "");
msgdef_recommended_action(
        "");
msgdef_ddts_component("");


msgdef(M32_ILLEGAL_CHANNEL, DSX1, LOG_ALERT, 0, "Munich 32 channel invalid");
msgdef_explanation(
        "");
msgdef_recommended_action(
        "");
msgdef_ddts_component("");


msgdef(M32_AR_TIMEOUT, DSX1, LOG_ALERT, 0, "Munich 32 action request timeout");
msgdef_explanation(
        "");
msgdef_recommended_action(
        "");
msgdef_ddts_component("");


msgdef(M32_AR_FAILURE, DSX1, LOG_ALERT, 0, "Munich 32 action request failure");
msgdef_explanation(
        "");
msgdef_recommended_action(
        "");
msgdef_ddts_component("");


msgdef(M32_BAD_STATE, DSX1, LOG_ALERT, 0, "Munich 32 in unknown state");
msgdef_explanation(
        "");
msgdef_recommended_action(
        "");
msgdef_ddts_component("");


msgdef(M32_BAD_BITRATE, DSX1, LOG_ALERT, 0, "Munich 32 bit rate not match");
msgdef_explanation(
        "");
msgdef_recommended_action(
        "");
msgdef_ddts_component("");


msgdef(M32_BAD_INTQ, DSX1, LOG_ALERT, 0, "bad interrupt queue");
msgdef_explanation(
        "");
msgdef_recommended_action(
        "");
msgdef_ddts_component("");


msgdef(M32_MEM_ALLOC, DSX1, LOG_ALERT, 0, "memory allocation error for Munich 32 channel");
msgdef_explanation(
        "");
msgdef_recommended_action(
        "");
msgdef_ddts_component("");



msgdef(M32_ERR, DSX1, LOG_ALERT, 0, "Munich 32 error configuring channelized interface");
msgdef_explanation(
        "");
msgdef_recommended_action(
        "");
msgdef_ddts_component("");
