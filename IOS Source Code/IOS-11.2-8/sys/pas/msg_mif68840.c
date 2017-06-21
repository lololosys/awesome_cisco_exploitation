/* $Id: msg_mif68840.c,v 3.1.60.4 1996/09/14 00:01:25 hampton Exp $
 * $Source: /release/112/cvs/Xsys/pas/msg_mif68840.c,v $
 *------------------------------------------------------------------
 * msg_mif68840.c - 1F MC68840 based PCI port adaptor support
 *                  (Device driver messages)
 *
 * August 1995, Farhad Sunavala
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_mif68840.c,v $
 * Revision 3.1.60.4  1996/09/14  00:01:25  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.60.3  1996/08/28  13:07:23  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.60.2  1996/08/22  18:47:20  dujihara
 * CSCdi65826:  Error messages do not specify slot/subunit
 * Branch: California_branch
 * Added slot (bay) and unit to error messages.
 *
 * Revision 3.1.60.1  1996/03/21  23:24:18  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.4.1  1995/12/23  04:41:32  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.1  1995/11/09  12:58:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/24  19:46:31  fsunaval
 * Placeholder files for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(MIF68840);			/* Define the facility */
msgdef_section("");


msgdef(DISCOVER, MIF68840, LOG_ALERT, 0,
       "MIF68840, Only found %d interfaces on bay %d, shutting down bay");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(NOTMIF68840, MIF68840, LOG_ERR, 0,
       "MIF68840 (bay %d), Device reported id %#x");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(OWNERR, MIF68840, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS,
       "MIF68840 (%d/%d), Buffer ownership error, pak=0x%x.");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
