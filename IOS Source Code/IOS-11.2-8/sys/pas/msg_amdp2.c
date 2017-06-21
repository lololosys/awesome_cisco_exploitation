/* $Id: msg_amdp2.c,v 3.3.54.4 1996/09/14 00:01:18 hampton Exp $
 * $Source: /release/112/cvs/Xsys/pas/msg_amdp2.c,v $
 *------------------------------------------------------------------
 * msg_amdp2.c - 8e Presidio2 based PCI port adaptor support
 *		 (Device driver messages)
 *
 * April 1995, Dave Turner
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_amdp2.c,v $
 * Revision 3.3.54.4  1996/09/14  00:01:18  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.54.3  1996/08/28  13:07:09  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.54.2  1996/05/07  01:22:43  mbeesley
 * CSCdi56710:  Correct AMDp2 fifo setting, update error messages
 * Branch: California_branch
 *
 * Revision 3.3.54.1  1996/03/21  23:24:13  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/02/10  00:31:03  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1995/11/22  21:42:12  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  18:55:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:45  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:33:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(AMDP2);				/* Define the facility */
msgdef_section("");


msgdef(LOSTCARR, AMDP2, LOG_NOTICE, 0,
       "AMDP2(%d/%d), Lost carrier. Transceiver problem?");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(LATECOLL, AMDP2, LOG_NOTICE, 0,
       "AMDP2(%d/%d), Late collision.");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(UNDERFLO, AMDP2, LOG_ERR, 0,
       "AMDP2(%d/%d), Transmit underflow.");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(OVERFLO, AMDP2, LOG_ERR, 0,
       "AMDP2(%d/%d), Receive overflow.");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(BABBLE, AMDP2, LOG_WARNING, 0,
       "AMDP2(%d/%d), Babble error.");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(SPURIDON, AMDP2, LOG_ERR, 0,
       "AMDP2(%d/%d), Spurious IDON interrupt, CSR%d=0x%04x.");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(MEMERR, AMDP2, LOG_ALERT, 0,
       "AMDP2(%d/%d), Memory error, CSR%d=0x%04x.");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(INITFAIL, AMDP2, LOG_ALERT, 0,
       "AMDP2(%d/%d), Init failed, CSR%d=0x%04x.");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");

msgdef(DISCOVER, AMDP2, LOG_ALERT, 0, 
       "Only found %d interfaces on bay %d, shutting down bay");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(OWNERR, AMDP2, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS, 
       "AMDP2(%d/%d), Buffer ownership error, pak=0x%x.");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(NOTAMDP2, AMDP2, LOG_ERR, 0, "Device reported %#x");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef_limit(COLL, AMDP2, LOG_NOTICE, 0, 
	     "AMDP2(%d/%d), Excessive collisions, TDR=%d, TRC=%d.",
	     MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
