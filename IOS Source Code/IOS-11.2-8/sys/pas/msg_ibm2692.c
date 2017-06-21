/* $Id: msg_ibm2692.c,v 3.3.32.5 1996/09/14 00:01:23 hampton Exp $
 * $Source: /release/112/cvs/Xsys/pas/msg_ibm2692.c,v $
 *------------------------------------------------------------------
 * msg_ibm2692.c - Message file for 4R IBM Token Ring PCI 
 *	 	   port adaptor
 *
 * May 1995, HC Wang
 * 
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_ibm2692.c,v $
 * Revision 3.3.32.5  1996/09/14  00:01:23  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.32.4  1996/08/28  13:07:20  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.32.3  1996/08/22  18:42:30  dujihara
 * CSCdi65823:  Error messages dont specify slot/subunit
 * Branch: California_branch
 * Added slot (bay) and unit to error messages.
 *
 * Revision 3.3.32.2  1996/06/07  00:17:57  hwang
 * CSCdi59533:  4r driver need to implement srb command queue
 * Branch: California_branch
 *
 * Revision 3.3.32.1  1996/04/23  02:24:01  sthormod
 * CSCdi54994:  4R uses cpu to byte-swap
 * Branch: California_branch
 *
 * Revision 3.3  1996/01/11  19:39:38  hwang
 * CSCdi46480:  4R failed hot standby test
 * vip_token_classify function to return device stat in trip style.
 * Remove some unused error messages.
 *
 * Revision 3.2  1995/11/17  18:55:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:49  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:33:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(IBM2692);			/* Define the facility */
msgdef_section("Error messages which refer to an IBM Token Ring chip set");


msgdef(NOPCIMEMORY, IBM2692, LOG_ALERT, 0,
       "IBM2692 (%d/%d),  PCI memory unavailable.");
msgdef_explanation(
	"The router or access server could not allocate memory for the
	descriptors.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(DISCOVER, IBM2692, LOG_ALERT, 0,
       "IBM2692, Only found %d interfaces on bay %d, shutting down bay");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(SRBQ_OVERFLOW, IBM2692, LOG_ALERT, 0,
       "IBM2692 (%d/%d), SRB queue overflow.");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


/*
 * C7200 specific messages
 */
msgdef(OLDREV, IBM2692, LOG_ALERT, 0,
       "IBM2692 (bay %d), Port adapter requires Rev 2 CPU.");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
