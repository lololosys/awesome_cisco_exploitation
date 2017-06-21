/* $Id: crypto_swepa_msg.c,v 1.1.66.3 1996/09/14 00:11:30 hampton Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/core/crypto_swepa_msg.c,v $
 *------------------------------------------------------------------
 * crypto_swepa_msg.c
 * 
 * This files defines the software encryption and key mgt messages. 
 *
 * 11, 1995, xliu
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_swepa_msg.c,v $
 * Revision 1.1.66.3  1996/09/14  00:11:30  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 1.1.66.2  1996/08/28  12:39:51  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 1.1.66.1  1996/04/19  14:56:39  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.1  1995/11/30  04:59:49  xliu
 * Branch: IosSec_branch
 *
 * Moved the crypto engine core files into dir CORE.
 *
 * Revision 1.1  1995/11/30  04:20:54  xliu
 * Place holder files.
 *
 * Revision 1.7  1995/06/07 12:03:03  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(SWEPA);
msgdef_section("");


msgdef(FAILED_TO_START_DES_PROCESS, SWEPA, LOG_INFO, 0, 
       "Failed to start DES process");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(START_DES_PROCESS, SWEPA, LOG_INFO, 0, "DES process start ...");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(FAILED_TO_START_EXP_PROCESS, SWEPA, LOG_INFO, 0, 
       "Failed to start EXP process");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(START_EXP_PROCESS, SWEPA, LOG_INFO, 0, "EXP process start ...");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(INIT_SW_EPA_IDB, SWEPA, LOG_INFO, 0, "Init swepa idb start ...");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(EPA_EXP_TIMER_EXPIRED, SWEPA, LOG_INFO, 0, "Exp Timer Expired!");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(EPA_EXP_RUNNING, SWEPA, LOG_INFO, 0, "Exp Running ....");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(FAILED_TO_ALLOCATE_ENCRYPT_QUE, SWEPA, LOG_INFO, 0,
       "Failed to allocate the encrypt queue.");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
