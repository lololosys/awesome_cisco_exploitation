/* $Id: exec_test_modem.h,v 3.1.64.1 1996/06/16 21:14:27 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/les/exec_test_modem.h,v $
 *------------------------------------------------------------------
 *  E X E C _ T E S T _ M O D E M . H
 *
 * Nov 1995, Viet Nguyen
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_test_modem.h,v $
 * Revision 3.1.64.1  1996/06/16  21:14:27  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/21  23:39:25  vnguyen
 * Placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * test modem back-to-back <first slot/port #> <second slot/port #>
 * 
 * OBJ(int,1) = first slot #
 * OBJ(int,2) = first port #
 * OBJ(int,3) = second slot #
 * OBJ(int,4) = second port #
 *
 */
EOLS       (modem_back2back_eol, modem_back2back_test_command, 
            MODEM_BACK2BACK_TEST);
GENERAL_NUMBER (modem_port_num2, modem_back2back_eol,
            NONE, OBJ(int,4), 0, 23, "",
            NUMBER_DEC | NUMBER_WS_OK );
CHARACTER  (modem_slot_separator2, modem_port_num2, NONE, '/');
GENERAL_NUMBER (modem_slot_num2, modem_slot_separator2,
            NONE, OBJ(int,3), 0, 2, "Second Slot/Port number",
            NUMBER_DEC | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);
GENERAL_NUMBER (modem_port_num1, modem_slot_num2,
            NONE, OBJ(int,2), 0, 23, "Port number",
            NUMBER_DEC | NUMBER_WS_OK );
CHARACTER  (modem_slot_separator1, modem_port_num1, NONE, '/');
GENERAL_NUMBER (modem_slot_num1, modem_slot_separator1,
            NONE, OBJ(int,1), 0, 2, "First Slot/Port number (i.e. 1/1)",
            NUMBER_DEC | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);
KEYWORD    (modem_back2back_test, modem_slot_num1, NONE,
            "back-to-back", "Execute back-to-back modem testing ", PRIV_OPR);
KEYWORD    (test_modem_kwd, modem_back2back_test, NONE,
            "modem", "Modems testing ", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	test_modem_kwd
