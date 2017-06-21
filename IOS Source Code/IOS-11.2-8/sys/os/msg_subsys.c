/* $Id: msg_subsys.c,v 3.2.62.3 1996/09/14 00:01:01 hampton Exp $
 * $Source: /release/112/cvs/Xsys/os/msg_subsys.c,v $
 *------------------------------------------------------------------
 * System error messages for Subsystem support
 *
 * August 1993, Scott Mackie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: msg_subsys.c,v $
 * Revision 3.2.62.3  1996/09/14  00:01:01  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/08/28  13:03:36  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.62.1  1996/05/24  19:29:49  hampton
 * Improve the granularity of low memory corruption checks during
 * subsystem initialization.  [CSCdi58674]
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:49:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:47:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/11  02:50:45  smackie
 * Add some paranoia to the subsystem sequence recursion and prevent
 * bogusness from causing stack problems. Also throw in support for
 * subsystems in multiple data segments. (CSCdi36893)
 *
 * Revision 2.2  1995/06/28  09:18:46  smackie
 * Finally make subsystem sequencing robust with multiple hierarchy
 * sequences. Tidy up subsystem code into the bargain and split the
 * parser code into a new file. Make the subsystem code far noisier
 * about bogusness. (CSCdi23568)
 *
 * Revision 2.1  1995/06/07  21:57:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(SUBSYS);
msgdef_section("Software subsystem error messages");


msgdef(BADVERSION, SUBSYS, LOG_CRIT, 0,
       "Bad subsystem version number (%d) - ignoring subsystem");
msgdef_explanation(
	"A software consistency check failed.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(MISMATCH, SUBSYS, LOG_CRIT, 0,
       "Kernel and subsystem version differ (%d.%d) - ignoring subsystem");
msgdef_explanation(
	"A software consistency check failed.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADCLASS, SUBSYS, LOG_CRIT, 0,
       "Bad subsystem class (%d) - ignoring subsystem");
msgdef_explanation(
	"A software consistency check failed.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADSEQUENCE, SUBSYS, LOG_CRIT, 0,
       "Subsystem (%s) has cross-class sequence for (%s)");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(RECURSION, SUBSYS, LOG_CRIT, 0,
       "Maximum sequence depth exceeded (%d) by (%s)");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(NOTFOUND, SUBSYS, LOG_CRIT, 0,
       "Subsystem (%s) needs subsystem (%s) to start");
msgdef_explanation(
	"A software consistency check failed.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(PAGEZERO, SUBSYS, LOG_ERR, 0,
       "The %s class subsystem %s was being initialized.");
msgdef_explanation(
	"In all Cisco products the first 256 bytes of memory is unused and off
	limits.  Newer platforms have hardware to immediately trap reads or
	writes to this area.  Older platforms periodically perform checks on
	this memory.  This message appears only on older platforms and
	indicates that this off limits memory area was modified.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
	 to your technical support representative."); 
msgdef_ddts_component(
	"Determine the component from the process name displayed in the
	error message.");
