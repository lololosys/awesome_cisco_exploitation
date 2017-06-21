/* $Id: cfg_line_modem_mgmt.h,v 3.1.64.1 1996/06/16 21:14:00 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/les/cfg_line_modem_mgmt.h,v $
 *------------------------------------------------------------------
 *  C F G _ L I N E _ M O D E M _ M G M T . H
 *
 * Dec 1995, Viet Nguyen
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_line_modem_mgmt.h,v $
 * Revision 3.1.64.1  1996/06/16  21:14:00  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/22  01:27:49  vnguyen
 * Placeholder file for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] modem shutdown
 */
EOLS	(line_modem_shutdown_eol, line_modem_command, MODEM_SHUTDOWN);
KEYWORD (line_modem_shutdown, line_modem_shutdown_eol, ALTERNATE,
         "shutdown", "Immediate busyout of modem", PRIV_CONF);

/***************************************************************
 * [no] modem bad
 */
EOLS	(line_modem_bad_eol, line_modem_command, MODEM_BAD);
KEYWORD (line_modem_bad, line_modem_bad_eol, line_modem_shutdown,
         "bad", "Mark modem as bad", PRIV_CONF);

/***************************************************************
 * [no] modem hold-reset
 */
EOLS	(line_modem_hold_reset_eol, line_modem_command, MODEM_HOLD_RESET);
KEYWORD (line_modem_hold_reset, line_modem_hold_reset_eol, line_modem_bad,
         "hold-reset", "Set modem in reset mode", PRIV_CONF);

/***************************************************************
 * [no] modem status-polling
 */
EOLS	(line_modem_status_poll_eol, line_modem_command, MODEM_STATUS_POLL);
KEYWORD (line_modem_status_poll, line_modem_status_poll_eol, line_modem_hold_reset,
         "status-poll", "Modem Management status polling", PRIV_CONF);

/***************************************************************
 * [no] modem at-mode-permit
 */
EOLS	(line_modem_at_mode_eol, line_modem_command, MODEM_AT_MODE_PERMIT);
KEYWORD (line_modem_at_mode, line_modem_at_mode_eol, line_modem_status_poll,
         "at-mode-permit", "Permit Direct Connect session", PRIV_CONF);

/***************************************************************
 * check to see if it is manageable modem image, only certain
 * management commands are valid for manageable image
 */
IFELSE  (line_modem_manageable, line_modem_at_mode, line_modem_hold_reset, 
         reg_invoke_modem_mgmt_exists());

#undef	ALTERNATE
#define	ALTERNATE	line_modem_manageable
