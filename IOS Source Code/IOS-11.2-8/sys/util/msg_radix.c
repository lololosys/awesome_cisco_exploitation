/* $Id: msg_radix.c,v 3.2.62.2 1996/09/14 00:09:54 hampton Exp $
 * $Source: /release/112/cvs/Xsys/util/msg_radix.c,v $
 *------------------------------------------------------------------
 * msg_radix.c - Message file for radix trie facility
 *
 * August 1994, Paul Traina
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_radix.c,v $
 * Revision 3.2.62.2  1996/09/14  00:09:54  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/08/28  13:18:58  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  17:56:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:43:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:34:02  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  23:13:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(RADIX);
msgdef_section("RADIX Error Messages");


msgdef_limit(NOMEMORY, RADIX, LOG_ERR, MSG_TRACEBACK+MSG_PROCESS,
	     "No memory available %s", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"The system is out of memory.");
msgdef_recommended_action(
	"Try one of these actions to correct the problem:

	Reduce the number of routes accepted by this router.

	Upgrade hardware.

	Use a smaller subset image on run-from-RAM platforms.");
msgdef_ddts_component("");


msgdef_limit(DELETE, RADIX, LOG_ERR, MSG_TRACEBACK+MSG_PROCESS,
	     "Error deleting trie entry, %s", ONESEC);
msgdef_explanation(
	"A software programming error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(ADDMASK, RADIX, LOG_ERR, MSG_TRACEBACK+MSG_PROCESS,
	     "Error adding mask entry, %s", ONESEC);
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(BADTREE, RADIX, LOG_ERR, MSG_TRACEBACK+MSG_PROCESS,
	     "Invalid pointer to head of tree, 0x%x", ONESEC);
msgdef_explanation(
	"A software programming error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(ORPHAN, RADIX, LOG_WARNING, MSG_TRACEBACK+MSG_PROCESS,
	     "Orphaned mask %#x, refcount=%d at %#x, next=%#x", 5*ONESEC);
msgdef_explanation(
	"A software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(INIT, RADIX, LOG_CRIT, MSG_TRACEBACK+MSG_PROCESS,
       "No memory for radix initialization: %s");
msgdef_explanation(
	"The system ran out of memory during initialization.  This should only
	occur if an image is too large for the existing dynamic memory.");
msgdef_recommended_action(
	"Use a smaller subset image or upgrade hardware.");
msgdef_ddts_component("");
