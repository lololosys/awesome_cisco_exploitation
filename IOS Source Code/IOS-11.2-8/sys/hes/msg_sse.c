/* $Id: msg_sse.c,v 3.3.52.3 1996/09/13 23:46:41 hampton Exp $
 * $Source: /release/112/cvs/Xsys/hes/msg_sse.c,v $
 *------------------------------------------------------------------
 * msg_sse.c - Messages from the SSE
 *
 * Sept., 1993 Tony Li
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_sse.c,v $
 * Revision 3.3.52.3  1996/09/13  23:46:41  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.52.2  1996/08/28  12:45:51  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.52.1  1996/03/18  19:42:54  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.20.1  1996/01/24  21:47:50  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/12/01  09:07:22  tli
 * CSCdi44709:  GETBUF problems in SSE fragmentation
 * Clean up error messages.
 *
 * Revision 3.2  1995/11/17  09:19:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:41:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/28  21:57:58  gchristy
 * CSCdi41169:  SSE: always issue an errmsg before hanging
 *
 * Revision 2.2  1995/09/25  08:33:46  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.1  1995/06/07  20:43:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(SSE);
msgdef_section("Silicon switching engine error messages");


msgdef_limit(BADMEMORY, SSE, LOG_ERR, 0, "SSE memory failure detected,"
	     " orig %#x %#x, tested %#x %#x ", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"A memory error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(BOGUSEQ, SSE, LOG_CRIT, 0, "SSE bogus equal branch "
	     "%#x, %#x, %d %x", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"An internal inconsistency in the data structures used by the SSE
	occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(COMPERR, SSE, LOG_INFO, 0,
	     "SSE compilation failure -- %s", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"A software failure occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(HUNG, SSE, LOG_CRIT, 0, "SSE hung -- %s", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"The SSE is hung.");
msgdef_recommended_action(
	"Reboot the SSE.");
msgdef_ddts_component("");


msgdef_limit(HWFAILURE, SSE, LOG_CRIT, 0, "SSE hardware failure -- %s code "
	     "%#x", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"A hardware error occurred, indicating an unexpected condition on the
	silicon switch processor (SSP) board.");
msgdef_recommended_action(
	"It might be necessary to replace the SSP. Copy the error message
	exactly as it appears, and report it to your technical support
	representative.");
msgdef_ddts_component("");


msgdef_limit(MEMERROR, SSE, LOG_CRIT, 0, "SSE memory failure, address %#x,\n"
	     "                 expected %#x, got %#x", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"A hardware error occurred, indicating memory failure on the silicon
	switch processor (SSP) board.");
msgdef_recommended_action(
	"It might be necessary to replace the SSP. Copy the error message
	exactly as it appears, and report it to your technical support
	representative.");
msgdef_ddts_component("");


msgdef_limit(NESTADDR, SSE, LOG_CRIT, MSG_TRACEBACK,
	     "SSE nested addresses detected, protocol %d %i",
	     MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"An internal inconsistency in the data structures used by the SSE
	occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(NOMATCH, SSE, LOG_CRIT, MSG_TRACEBACK,
	     "SSE delete didn't end at a match node, type %c,"
	     "value %d, address %i, bytecount %d, high %d, state %d",
	     MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"An internal inconsistency in the data structures used by the SSE
	occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(NOMEMORY, SSE, LOG_ERR, 0,
	     "No memory available for %s", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"An operation could not be accomplished because of a low memory
	condition.");
msgdef_recommended_action(
	"Reduce other system activity to ease memory demands. If conditions
	warrant, upgrade to a larger memory configuration.");
msgdef_ddts_component("");


msgdef_limit(NOTZERO, SSE, LOG_CRIT, 0, "program memory does "
	     "not start at zero", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"A hardware error occurred, indicating an unexpected condition on the
	silicon switch processor (SSP) board.");
msgdef_recommended_action(
	"It might be necessary to replace the SSP. Copy the error message
	exactly as it appears, and report it to your technical support
	representative.");
msgdef_ddts_component("");


msgdef_limit(SWFAILURE, SSE, LOG_CRIT, MSG_TRACEBACK,
	     "SSE software failure -- %s %#x", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"A software failure occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");
