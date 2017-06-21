/* $Id: exec_clear_modem.h,v 3.1.64.1 1996/06/16 21:14:07 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/les/exec_clear_modem.h,v $
 *------------------------------------------------------------------
 *  E X E C _ C L E A R  _ M O D E M . H
 *
 * Nov 1995, Viet Nguyen
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_clear_modem.h,v $
 * Revision 3.1.64.1  1996/06/16  21:14:07  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/21  23:39:17  vnguyen
 * Placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * clear modem [counters] [<slot/port #> | group [<group #>] ] 
 * 
 * OBJ(int,1) = CLEAR_MODEM_HW | CLEAR_MODEM_COUNTERS
 * OBJ(int,2) = slot #
 * OBJ(int,3) = port #
 * OBJ(int,4) = group #
 * OBJ(int,5) = TRUE iff slot/port # is entered
 * OBJ(int,6) = TRUE iff group # is entered
 *
 */
EOLS       (clear_modem_eol, clear_modem_command, CLEAR_MODEM);
EOLS       (clear_modem_group_eol, clear_modem_command,
            CLEAR_MODEM_GROUP);
SET        (clear_modem_group_num_set, clear_modem_group_eol,
            OBJ(int,6), TRUE);
NUMBER     (clear_modem_group_num, clear_modem_group_num_set,
            clear_modem_group_eol, OBJ(int,4), 1, PLATFORM_MAXLINES, 
            "Group number");
KEYWORD    (clear_modem_group, clear_modem_group_num,
            no_alt, "group", "Modem group",
            PRIV_OPR);
SET        (clear_modem_slot_port_num_set, clear_modem_eol,
            OBJ(int,5), TRUE);
GENERAL_NUMBER (clear_modem_port_num, clear_modem_slot_port_num_set,
            NONE, OBJ(int,3), 0, 23, "Port number",
            NUMBER_DEC | NUMBER_WS_OK );
CHARACTER  (clear_modem_slot_separator, clear_modem_port_num,
            NONE, '/');
GENERAL_NUMBER (clear_modem_slot_num, clear_modem_slot_separator,
            clear_modem_group, OBJ(int,2), 0, 2, "Slot/Port number (i.e. 1/1)",
            NUMBER_DEC | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);
SET        (clear_modem, clear_modem_slot_num, OBJ(int,1), CLEAR_MODEM_HW);

/**************************************************************
 * clear modem counters
 */
EOLS       (clear_modem_counters_group_eol, clear_modem_command,
            CLEAR_MODEM_GROUP);
SET        (clear_modem_counters_group_num_set, clear_modem_counters_group_eol,
            OBJ(int,6), TRUE);
NUMBER     (clear_modem_counters_group_num, clear_modem_counters_group_num_set,
            clear_modem_counters_group_eol, OBJ(int,4), 1, PLATFORM_MAXLINES,
            "Group number");
KEYWORD    (clear_modem_counters_group, clear_modem_counters_group_num,
            clear_modem_eol, "group", "Modem group",
            PRIV_OPR);
SET        (clear_modem_counters_slot_port_num_set, clear_modem_eol,
            OBJ(int,5), TRUE);
GENERAL_NUMBER (clear_modem_counters_port_num,
            clear_modem_counters_slot_port_num_set, NONE, OBJ(int,3), 0, 23,
            "Port number", NUMBER_DEC | NUMBER_WS_OK );
CHARACTER  (clear_modem_counters_slot_separator, clear_modem_counters_port_num,
            NONE, '/');
GENERAL_NUMBER (clear_modem_counters_slot_num,
            clear_modem_counters_slot_separator,clear_modem_counters_group,
            OBJ(int,2), 0, 2, "Slot/Port number (i.e. 1/1)",
            NUMBER_DEC | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);
KEYWORD_ID (clear_modem_counters, clear_modem_counters_slot_num,
            clear_modem,OBJ(int,1), CLEAR_MODEM_COUNTERS,
            "counters", "Clear modem counters", PRIV_OPR);
 
/**************************************************************
 * clear modem all
 */
EOLS       (clear_modem_all_eol, clear_modem_command,CLEAR_MODEM);
SET        (clear_modem_all_set, clear_modem_all_eol, OBJ(int,1),
            CLEAR_MODEM_HW);
KEYWORD    (clear_modem_all, clear_modem_all_set,
            clear_modem_counters, "all", "Clear all modems",
            PRIV_OPR);

/***************************************************************
 * clear modem at-mode <slot/port #>
 *
 * OBJ(int,1) = slot #
 * OBJ(int,2) = port #
 *
 */
EOLS       (clear_modem_at_mode_eol, clear_modem_at_mode_command, 
            CLEAR_MODEM_AT_MODE);
GENERAL_NUMBER (clear_modem_at_mode_port_num, clear_modem_at_mode_eol,
            NONE, OBJ(int,2), 0, 23, "Port number",
            NUMBER_DEC | NUMBER_WS_OK );
CHARACTER  (clear_modem_at_mode_slot_separator, clear_modem_at_mode_port_num, 
            NONE, '/');
GENERAL_NUMBER (clear_modem_at_mode_slot_num, 
            clear_modem_at_mode_slot_separator,
            NONE, OBJ(int,1), 0, 2, "Slot/Port number (i.e. 1/1)",
            NUMBER_DEC | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);
KEYWORD    (clear_modem_at_mode, clear_modem_at_mode_slot_num,
            NONE,
            "at-mode", "Clear modem AT-session", PRIV_OPR);

/***************************************************************
 * check to see if it is manageable modem image, only certain
 * management commands are valid for manageable image
 */
ASSERT     (clear_modem_manageable, clear_modem_at_mode, clear_modem_all, 
            reg_invoke_modem_mgmt_exists());

KEYWORD    (clear_modem_kwd, clear_modem_manageable, ALTERNATE,
            "modem", "Reset modem hardware or clear modem counters", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	clear_modem_kwd
