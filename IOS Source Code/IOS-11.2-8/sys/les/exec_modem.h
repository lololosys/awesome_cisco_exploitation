/* $Id: exec_modem.h,v 3.1.64.1 1996/06/16 21:14:18 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/les/exec_modem.h,v $
 *------------------------------------------------------------------
 *  E X E C _ M O D E M . H
 *
 * Nov 1995, Viet Nguyen
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_modem.h,v $
 * Revision 3.1.64.1  1996/06/16  21:14:18  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/21  23:39:21  vnguyen
 * Placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * modem at-mode <slot/port #>
 *
 * OBJ(int,1) = slot #
 * OBJ(int,2) = port #
 *
 */
EOLS       (modem_at_mode_eol, modem_at_mode_command, MODEM_AT_MODE);
GENERAL_NUMBER (modem_port_num, modem_at_mode_eol,
            NONE, OBJ(int,2), 0, 23, "",
            NUMBER_DEC | NUMBER_WS_OK );
CHARACTER  (modem_slot_separator, modem_port_num, NONE, '/');
GENERAL_NUMBER (modem_slot_num, modem_slot_separator,
            NONE, OBJ(int,1), 0, 2, "Slot/Port number",
            NUMBER_DEC | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);
KEYWORD    (modem_at_mode, modem_slot_num, NONE,
            "at-mode", "Enter AT mode for modem", PRIV_CONF);
KEYWORD    (modem_kwd, modem_at_mode, ALTERNATE,
            "modem", "Modem configuration", PRIV_CONF);

/***************************************************************
 * check to see if it is manageable modem image, only certain
 * management commands are valid for manageable image
 */
IFELSE  (modem_mgmt_manageable, modem_kwd, ALTERNATE, 
         reg_invoke_modem_mgmt_exists());


#undef	ALTERNATE
#define	ALTERNATE	modem_mgmt_manageable
