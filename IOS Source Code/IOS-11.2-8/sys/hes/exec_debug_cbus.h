/* $Id: exec_debug_cbus.h,v 3.3 1995/11/27 21:31:49 yanke Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/exec_debug_cbus.h,v $
 *------------------------------------------------------------------
 * exec_debug_cbus.h - debug cbus events
 *
 * July 1993, William H. Palmer
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_debug_cbus.h,v $
 * Revision 3.3  1995/11/27  21:31:49  yanke
 * Add 'debug cbus monitor' command, add cbus protection
 * registry functions.
 *
 * CSCdi44478:  slave rsp crashes or resets during ips eoir events
 *
 * Revision 3.2  1995/11/17  09:15:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:38:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/13  01:20:30  fsunaval
 * CSCdi36847:  FCICMDFAILs put under debug control (debug cbus
 * cmd-timeout)
 *
 * Revision 2.1  1995/07/02  10:51:53  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/19  14:47:32  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:17:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLS	(debug_cbus_eol, debug_command, DEBUG_CBUS);
/******************************************************************
 * debug cbus cmdretry
 */
EOLS	(debug_cbus_cmdretry_eol, debug_command, DEBUG_CBUS_CMDRETRY);
KEYWORD	(debug_cbus_cmdretry, debug_cbus_cmdretry_eol, debug_cbus_eol,
	"cmd-retry", "ciscoBus command retries", PRIV_OPR);

/******************************************************************
 * debug cbus cmdtimeout
 */
EOLS    (debug_cbus_cmdtimeout_eol, debug_command, DEBUG_CBUS_CMDTIMEOUT);
KEYWORD (debug_cbus_cmdtimeout, debug_cbus_cmdtimeout_eol, debug_cbus_cmdretry,
	"cmd-timeout", "ciscoBus command timeouts", PRIV_OPR);

/******************************************************************
 * debug cbus monitor
 */
EOLS    (debug_cbus_monitor_eol, debug_command, DEBUG_CBUS_MONITOR);
KEYWORD (debug_cbus_monitor, debug_cbus_monitor_eol, debug_cbus_cmdtimeout,
	"monitor", "ciscoBus monitor", PRIV_OPR);

/******************************************************************
 * debug cbus
 *
 */
KEYWORD_DEBUG(debug_cbus, debug_cbus_monitor, ALTERNATE,
	      OBJ(pdb,1), cbus_debug_arr,
	      "cBus", "ciscoBus events", PRIV_OPR);

#undef ALTERNATE
#define ALTERNATE	debug_cbus
