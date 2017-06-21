/* $Id: msg_dma.c,v 3.2.62.2 1996/09/14 00:02:07 hampton Exp $
 * $Source: /release/112/cvs/Xsys/src-rvip/msg_dma.c,v $
 *------------------------------------------------------------------
 * msg_dma.c - Declaration of RVIP DMA messages
 *
 * February 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_dma.c,v $
 * Revision 3.2.62.2  1996/09/14  00:02:07  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/08/28  13:14:42  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  18:50:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:02  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:58:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(DMA);				/* Define the facility */
msgdef_section("Direct memory access error messages");


msgdef(BADOWN, DMA, LOG_ALERT, 0, 
       "Own bit not set before end of packet reached");
msgdef_explanation(
	"This message indicates that an unexpected own bit was found in a DMA
	entry.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADLENGTH, DMA, LOG_ALERT, 0, 
       "Zero length memd packet in buffer header %x");
msgdef_explanation(
	"VIP found that the memory device (MEMD) buffer header indicated a zero
	length transmit.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADICBPTR, DMA, LOG_ALERT, 0, 
       "No icb assigned to port %d, interface %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this error message recurs, copy it exactly as it appears and report
	it to your technical support representative.");
msgdef_ddts_component("");


msgdef(BADJUMPSTART, DMA, LOG_ALERT, 0, "DMA jumpstart failed, %d");
msgdef_explanation(
	"An anomalous situation where the direct memory access (DMA) jumpstart
	mechanism failed to find the DMA to start.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");
