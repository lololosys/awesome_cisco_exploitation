/* $Id: msg_tdm.c,v 3.1.72.4 1996/09/13 23:50:46 hampton Exp $
 * $Source: /release/112/cvs/Xsys/les/msg_tdm.c,v $
 *------------------------------------------------------------------
 * msg_tdm.c -- Message file for lance facility
 *
 * December 1995, Joe Turner
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_tdm.c,v $
 * Revision 3.1.72.4  1996/09/13  23:50:46  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.72.3  1996/08/28  12:59:48  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.72.2  1996/07/02  23:55:20  jturner
 * CSCdi61176:  post-brasil-commit code cleanup in tdm, asm, src and
 * makefile
 * Branch: California_branch
 *
 * Revision 3.1.72.1  1996/06/16  21:16:38  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/03  21:26:49  jturner
 * Placeholder for Brasil.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(TDM);
msgdef_section("");
 

msgdef(TDM_MB_INITFAIL, TDM, LOG_ALERT, 0,
       "Motherboard TDM init failure:\nstream %d, channel %d, channel register 0x%x, control register 0x%x.");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(TDM_INITFAIL, TDM, LOG_ALERT, 0,
       "Slot %d TDM init failure:\nstream %d, channel %d, channel register 0x%x, control register 0x%x.");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(TDM_BADUNIT, TDM, LOG_ERR, 0, 
       "Slot %d bad board ID 0x%x detected.");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(TDM_BAD_CONNECT, TDM, LOG_ALERT, 0,
       "Bad connection requested: slot %d, stream %d, channel %d to slot %d, stream %d, channel %d.");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
