/* $Id: msg_dec21140.c,v 3.4.10.4 1996/09/14 00:01:20 hampton Exp $
 * $Source: /release/112/cvs/Xsys/pas/msg_dec21140.c,v $
 *------------------------------------------------------------------
 * msg_dec21140.c - Error messages for DEC21140 100mb ethernet
 *		    PCI port adaptor driver
 *
 * February 1995, Steve Jacobson
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_dec21140.c,v $
 * Revision 3.4.10.4  1996/09/14  00:01:20  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.4.10.3  1996/08/28  13:07:14  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.10.2  1996/05/07  19:24:07  mbeesley
 * CSCdi56937:  Update DEC21140 errmsgs to include slot/subunit
 * Branch: California_branch
 *
 * Revision 3.4.10.1  1996/03/21  23:24:15  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/03/18  23:29:15  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/03/07  00:19:19  mbeesley
 * CSCdi50649:  DEC21140 in Store-and-Forward mode
 *         o Put device in tx cut-thru, 512 byte tx fifo threshold
 *         o Increase rx pool, eliminate use of fallback pool
 *         o Handle rx fifo overrun and error interrupt correctly
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1995/11/22  21:42:14  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 * Revision 3.4  1996/02/23  20:02:19  feisal
 * CSCdi45142:  Support VLAN ISL for new 1FE port adapter
 *
 * Revision 3.3  1995/12/01  02:00:12  mbeesley
 * CSCdi44698:  PCI subsystem needs improvement
 *
 * Revision 3.2  1995/11/17  18:55:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:47  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:33:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(DEC21140);				/* Define the facility */
msgdef_section("");


msgdef(JABBER, DEC21140, LOG_NOTICE, 0,
       "DEC21140(%d/%d), jabber timeout");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(LOSTCARR, DEC21140, LOG_NOTICE, 0,
       "DEC21140(%d/%d), lost carrier");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(COLL, DEC21140, LOG_NOTICE, 0,
       "DEC21140(%d/%d), excessive collisions");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(UNDERFLO, DEC21140, LOG_ERR, 0,
       "DEC21140(%d/%d), transmit FIFO underflow");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(LATECOLL, DEC21140, LOG_NOTICE, 0,
       "DEC21140(%d/%d), late collision error");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(WATCHDOG, DEC21140, LOG_NOTICE, 0,
       "DEC21140(%d/%d), enormous packet received");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(RXFIFO, DEC21140, LOG_ERR, 0,
       "DEC21140(%d/%d), receive FIFO overflow");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(INITFAIL, DEC21140, LOG_ALERT, 0,
       "DEC21140(%d/%d), initialization timeout failure, csr5=%#x");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(ERRINT, DEC21140, LOG_ERR, 0,
       "DEC21140(%d/%d), error interrupt, csr0=%#x, csr5=%#x");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(OWNERR, DEC21140, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS, 
       "DEC21140(%d/%d), buffer ownership error, pak=0x%x");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(NOTDEC21140, DEC21140, LOG_ERR, 0, "Device reported %#x");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(NOISL, DEC21140, LOG_CRIT, 0, 
       "Interface %s does not support ISL");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(DISCOVER, DEC21140, LOG_ALERT, 0, 
       "Only found %d interfaces on bay %d, shutting down bay");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef_limit(NOCARR, DEC21140, LOG_NOTICE, 0, 
	     "DEC21140(%d/%d), no carrier", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
