/* $Id: msg_les_fddi.c,v 3.1.2.2 1996/09/13 23:50:37 hampton Exp $
 * $Source: /release/112/cvs/Xsys/les/msg_les_fddi.c,v $
 *------------------------------------------------------------------
 * M S G  _ L E S _ F D D I . C
 *
 * August 1996, Nick Thille
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_les_fddi.c,v $
 * Revision 3.1.2.2  1996/09/13  23:50:37  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/08/28  12:59:39  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1  1996/08/25  01:17:32  thille
 * Placeholders for cleanup of message files and externs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"
#include "../src-c4000/c4000_bsi_public.h"


facdef(LES_FDDI);
msgdef_section("");


msgdef(ILLEGAL, LES_FDDI, LOG_CRIT, 0, msgtxt_fddi_illegal);
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
