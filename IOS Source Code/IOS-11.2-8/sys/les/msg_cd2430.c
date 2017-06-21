/* $Id: msg_cd2430.c,v 3.1.24.5 1996/09/13 23:50:23 hampton Exp $
 * $Source: /release/112/cvs/Xsys/les/msg_cd2430.c,v $
 *------------------------------------------------------------------
 * msg_cd2430.c - Message file for cd2430 facility
 *
 * February 1996, Suresh Sangiah
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_cd2430.c,v $
 * Revision 3.1.24.5  1996/09/13  23:50:23  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.24.4  1996/08/28  12:59:26  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.24.3  1996/07/05  19:31:38  jhernand
 * CSCdi61429:  Modify Cirrus Logic CD2430 initialization by adding Timeout
 * Branch: California_branch
 *
 * Revision 3.1.24.2  1996/07/04  02:06:59  jhernand
 * CSCdi61640:  Cobra code does not conform to Cisco guidelines
 * Branch: California_branch
 *
 * Revision 3.1.24.1  1996/06/17  08:34:06  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.1  1996/02/13  07:14:27  ssangiah
 * Placeholder files for porting new platform code from 11.0.
 *
 *
 * Port the Cobra platform into 11.1
 *
 * Revision 3.1  1996/02/13  07:14:27  ssangiah
 * Placeholder files for porting new platform code from 11.0.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"
#include "../os/msgtxt_sys.h"


facdef(CIRRUS);				/* Define the facility */
msgdef_section("");


msgdef_limit(NOMEMORY, CIRRUS, LOG_ALERT, 0, msgtxt_nomemory, 
                                                           MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(BADENCAP, CIRRUS, LOG_ERR, 0, msgtxt_badencap);
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(BADHDXFSM, CIRRUS, LOG_ERR, 0, 
       "Interface serial %d, Unexpected HDX state %d, event %d");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(BADRXISR, CIRRUS, LOG_ERR, 0,
       "Interface serial %d, Unexpected situation in receive ISR");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(BADTXISR, CIRRUS, LOG_ERR, 0,
       "Interface serial %d, Unexpected situation in transmit ISR");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(BADMODE, CIRRUS, LOG_ERR, 0,
       "Interface serial %d, Bad mode encountered in init");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(LINEFLAP, CIRRUS, LOG_NOTICE, 0,
       "Unit %d excessive modem control changes");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(RESETERR, CIRRUS, LOG_ERR, 0,
       "CD2430 serial controller %d, Reset timeout error");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");

msgdef(SETCHAN, CIRRUS, LOG_ERR, MSG_PROCESS | MSG_TRACEBACK,
       "Serial%d: setchan called in CD2430 interrupt context");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");

/*
 * Message facility for async software.
 */
facdef(TTYDRIVER);
msgdef_section("");


msgdef(NOBUF, TTYDRIVER, LOG_CRIT, MSG_PROCESS | MSG_TRACEBACK,
       "Unable to allocate %d I/O buffers");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(NOMEM, TTYDRIVER, LOG_CRIT, MSG_PROCESS | MSG_TRACEBACK,
       "Unable to allocate %d byte status block");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(NOBRKPAK, TTYDRIVER, LOG_CRIT, MSG_PROCESS | MSG_TRACEBACK,
       "Unable to allocate break block from I/O mem");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
