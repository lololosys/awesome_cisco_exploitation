/* $Id: msg_lat.c,v 3.2.62.2 1996/09/13 23:50:17 hampton Exp $
 * $Source: /release/112/cvs/Xsys/lat/msg_lat.c,v $
 *------------------------------------------------------------------
 * msg_lat.c - Message file for lat facility
 *
 * July 17, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_lat.c,v $
 * Revision 3.2.62.2  1996/09/13  23:50:17  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/08/28  12:56:13  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  17:34:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:27:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:26:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(LAT);				/* Define the LAT facility */
msgdef_section("DEC Local Area Transport error messages");


msgdef(NOSYNC, LAT, LOG_ERR, 0,
       "Tty%t, Output data ptrs out of sync with byte count");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(BADDATA, LAT, LOG_ERR, 0,
       "Tty%t, Data pointer does not correspond to current packet");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(REUSE, LAT, LOG_ERR, 0,
       "Tty%t, Attempt to re-use slot array, empty = %d, fill = %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(QBSPACED, LAT, LOG_ERR, 0,
       "Queue block at %x not found for HI connection");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(BUFFULL, LAT, LOG_ERR, 0,
       "Tty%t, data buffer full with count %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this messages recurs, call your technical support representative
	for assistance.");
msgdef_ddts_component("");


msgdef(NULLIDB, LAT, LOG_ERR, 0,
       "Null IDB pointer with destination %e");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this messages recurs, call your technical support representative
	for assistance.");
msgdef_ddts_component("");
