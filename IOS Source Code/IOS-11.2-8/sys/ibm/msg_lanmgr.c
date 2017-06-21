/* $Id: msg_lanmgr.c,v 3.2.60.2 1996/09/13 23:49:09 hampton Exp $
 * $Source: /release/112/cvs/Xsys/ibm/msg_lanmgr.c,v $
 *------------------------------------------------------------------
 * msg_lanmgr.c - Message file for Lan Network Manager support code
 *
 * April 1990, David R. Hampton
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_lanmgr.c,v $
 * Revision 3.2.60.2  1996/09/13  23:49:09  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/08/28  12:46:51  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  09:23:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:45:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:02:00  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:47:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __MSG_LANMGR_C__
#define __MSG_LANMGR_C__

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(LANMGR);				/* Define the Lan Manager facility */
msgdef_section("IBM LAN Network Manager error messages");


msgdef(OLDTRAMON, LANMGR, LOG_WARNING, 0,
       "Cannot run RPS on %s, Old TRAMON code.");
msgdef_explanation(
	"The software on this interface cannot support the functions required
	for LAN Network Manager.");
msgdef_recommended_action(
	"Call your technical support representative about a firmware upgrade
	for this interface card.");
msgdef_ddts_component("");


msgdef(BADRNGNUM, LANMGR, LOG_WARNING, 0,
       "Ring number mismatch on %s, shutting down the interface.");
msgdef_explanation(
	"The router detected a conflict in assigned ring numbers for the
	specified Token Ring. The Ring Parameter Server (RPS) function was shut
	down to prevent potentially incorrect information from being sourced
	onto the ring.");
msgdef_recommended_action(
	"Check all bridges connected to this Token Ring and ensure that they
	are using the same assigned ring number.");
msgdef_ddts_component("");


msgdef(BADUPCALL, LANMGR, LOG_WARNING, 0,
       "Unexpected call (%s) from LLC2 for Bridge %03x-%x-%03x.");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");



/*
 * LNM CLSI related error messages
 */
facdef(LNMC);	
msgdef_section("LAN network manager system error messages");


msgdef(BADCLSIIND, LNMC, LOG_ERR, MSG_TRACEBACK, 
       "%s Invalid indication %s, %s");
#define LNMC_ERR_BADCLSIIND	&msgsym(BADCLSIIND, LNMC)  
msgdef_explanation(
	"A LAN network manager system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADCLSIRET, LNMC, LOG_ERR, MSG_TRACEBACK, 
       "%s Invalid ret code (0x%x) %s, %s");
#define LNMC_ERR_BADCLSIRET	&msgsym(BADCLSIRET, LNMC)  
msgdef_explanation(
	"A LAN network manager system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADCLSICNF, LNMC, LOG_ERR, MSG_TRACEBACK, 
       "%s Invalid confirm %s %s");
#define LNMC_ERR_BADCLSICNF	&msgsym(BADCLSICNF, LNMC)  
msgdef_explanation(
	"A LAN network manager system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADCLSI, LNMC, LOG_ERR, MSG_TRACEBACK, 
       "%s primitive not valid for lnm %s %s");
#define LNMC_ERR_BADCLSI	&msgsym(BADCLSI, LNMC)  
msgdef_explanation(
	"A LAN network manager system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADCLSIIDTYPE, LNMC, LOG_ERR, MSG_TRACEBACK, 
       "%s Invalid ID type = 0x%x %s");
#define LNMC_ERR_BADCLSIIDTYPE	&msgsym(BADCLSIIDTYPE, LNMC)  
msgdef_explanation(
	"A LAN network manager system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADCLSIPRIMTYPE, LNMC, LOG_ERR, MSG_TRACEBACK, 
       "%s Invalid primitive type = 0x%x %s");
#define LNMC_ERR_BADCLSIPRIMTYPE &msgsym(BADCLSIPRIMTYPE, LNMC)  
msgdef_explanation(
	"A LAN network manager system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADCLSIHDRLEN, LNMC, LOG_ERR, MSG_TRACEBACK, 
       "%s Bad header len = 0x%x %s");
#define LNMC_ERR_BADCLSIHDRLEN	&msgsym(BADCLSIHDRLEN, LNMC)  
msgdef_explanation(
	"A LAN network manager system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADCLSIDATALEN, LNMC, LOG_ERR, MSG_TRACEBACK, 
       "%s Bad data len = %d %s ");
#define LNMC_ERR_BADCLSIDATALEN	&msgsym(BADCLSIDATALEN, LNMC)  
msgdef_explanation(
	"A LAN network manager system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADCLSISAP, LNMC, LOG_ERR, MSG_TRACEBACK, 
       "%s Bad clsi SAP id = %x %s ");
#define LNMC_ERR_BADCLSISAP	&msgsym(BADCLSISAP, LNMC)  
msgdef_explanation(
	"A LAN network manager system error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");

#endif /* __MSG_LANMGR_C__ */
