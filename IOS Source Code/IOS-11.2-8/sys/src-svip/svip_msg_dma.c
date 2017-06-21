/* $Id: svip_msg_dma.c,v 3.1.62.3 1996/09/14 00:02:09 hampton Exp $
 * $Source: /release/112/cvs/Xsys/src-svip/svip_msg_dma.c,v $
 *------------------------------------------------------------------
 * svip_msg_dma.c - Declaration of SVIP DMA messages
 *
 * October 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_msg_dma.c,v $
 * Revision 3.1.62.3  1996/09/14  00:02:09  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.62.2  1996/08/28  13:15:08  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.62.1  1996/03/21  23:51:31  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.10.1  1996/02/10  00:22:05  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/12/26  21:18:41  getchell
 * Placeholder files for Viper development
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* 
 * Include files
 */
#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


/*
 * Define the facility
 */
facdef(DMA);
msgdef_section("");


/* 
 * Message strings
 */
static const char msg_esr[] = "Exceeded number of memd tx status registers in QE";

/*
 * Messages
 */
msgdef(EXCEEDED_STATUS_REG, DMA, LOG_ALERT, 0, msg_esr);
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");

/* end of file */
