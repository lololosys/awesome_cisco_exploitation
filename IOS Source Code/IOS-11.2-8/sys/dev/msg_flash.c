/* $Id: msg_flash.c,v 3.2.60.2 1996/09/13 23:46:05 hampton Exp $
 * $Source: /release/112/cvs/Xsys/dev/msg_flash.c,v $
 *------------------------------------------------------------------
 * msg_flash.c - Message file for flash facility
 *
 * September 1994, Stig Thormodsrud
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_flash.c,v $
 * Revision 3.2.60.2  1996/09/13  23:46:05  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/08/28  12:40:30  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  09:01:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:20:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:25:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"
#include "../dev/msgtxt_flash.h"


facdef(FLASH);				/* Define flash facility */
msgdef_section("Flash nonvolatile memory error messages");


msgdef(WRITEFAIL, FLASH, LOG_ERR, 0, flash_writefail_msg);
msgdef_explanation(
	"The write operation to Flash failed.");
msgdef_recommended_action(
	"Try erasing the device and trying the write again. If that fails, call
	your technical support representative for assistance.");
msgdef_ddts_component("");


msgdef(ERASEFAIL, FLASH, LOG_ERR, 0, flash_erasefail_msg);
msgdef_explanation(
	"The system was unable to erase a Flash device.");
msgdef_recommended_action(
	"Try to erase again. If that fails, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(ACCESSATTRIBONRP, FLASH, LOG_ERR, MSG_TRACEBACK, "%s");
msgdef_explanation(
	"The route processor cannot access the Flash memory card's attribute
	memory. The Flash memory card should continue to operate normally.");
msgdef_recommended_action(
	"If there is still a problem with the Flash memory card, copy the error
	message exactly as it appears and report it to your technical support
	representative.");
msgdef_ddts_component("");
