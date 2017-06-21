/* $Id: msg_filesys.c,v 3.1.2.4 1996/09/13 23:46:18 hampton Exp $
 * $Source: /release/112/cvs/Xsys/filesys/msg_filesys.c,v $
 *------------------------------------------------------------------
 * msg_filesys.c : Message file for the flash file system.
 *
 * April 1996, Suresh Sangiah
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_filesys.c,v $
 * Revision 3.1.2.4  1996/09/13  23:46:18  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/08/28  12:42:39  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.2.2  1996/04/29  19:22:40  ssangiah
 * CSCdi55648:  The boot flash command does not work.
 *   o Get the "boot flash" command to work again on RSP and c7200.
 *   o Fix an erroneous log message.
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/04/22  18:58:18  ssangiah
 * CSCdi54920:  Need to add support for the flash MIB on c7200.
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/20  03:13:21  ssangiah
 * Placeholder files for new platform flash MIB support.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(FILESYS);			/* Define the Filesys facility */
msgdef_section("");


msgdef(DEV, FILESYS, LOG_NOTICE, 0,
       "PCMCIA flash card %s %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(FLASH, FILESYS, LOG_ERR, 0, 
       "%s %s error %d");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
