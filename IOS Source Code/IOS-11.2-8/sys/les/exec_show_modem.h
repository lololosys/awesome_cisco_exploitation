/* $Id: exec_show_modem.h,v 3.1.64.2 1996/09/02 08:36:59 styang Exp $
 * $Source: /release/112/cvs/Xsys/les/exec_show_modem.h,v $
 *------------------------------------------------------------------
 *  E X E C _ S H O W _ M O D E M . H
 *
 * Nov 1995, Viet Nguyen
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_modem.h,v $
 * Revision 3.1.64.2  1996/09/02  08:36:59  styang
 * CSCdi61183:  Cannot tell hardware revision of cards from command line
 * Branch: California_branch
 * To show hardware version info for the modem card and T1/E1 card.
 *
 * Revision 3.1.64.1  1996/06/16  21:14:22  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/21  23:39:23  vnguyen
 * Placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * show modem [csm | log] [<slot/port #> | group [<group #>] ] 
 * 
 * OBJ(int,1) = SHOW_MODEM_STATS | SHOW_MODEM_CSM | SHOW_MODEM_LOG |
 *              SHOW_MODEM_SUMMARY | SHOW_MODEM_AT_MODE |SHOW_MODEM_VERSION
 * OBJ(int,2) = slot #
 * OBJ(int,3) = port #
 * OBJ(int,4) = group #
 * OBJ(int,5) = TRUE iff slot/port # is entered
 * OBJ(int,6) = TRUE iff group # is entered
 *
 */
EOLS       (show_modem_eol, show_modem_command, SHOW_MODEM);
EOLS       (show_modem_group_eol, show_modem_command,
            SHOW_MODEM_GROUP);
SET        (show_modem_group_num_set, show_modem_group_eol,
            OBJ(int,6), TRUE);
NUMBER     (show_modem_group_num, show_modem_group_num_set,
            show_modem_group_eol, OBJ(int,4), 1, PLATFORM_MAXLINES, 
            "Group number");
SET        (show_modem_group_num_clear, show_modem_group_num,
            OBJ(int,6), FALSE);
KEYWORD    (show_modem_group, show_modem_group_num_clear,
            show_modem_eol, "group", "Modem group information",
            PRIV_USER);
SET        (show_modem_slot_port_num_set, show_modem_eol,
            OBJ(int,5), TRUE);
GENERAL_NUMBER (show_modem_port_num, show_modem_slot_port_num_set,
            NONE, OBJ(int,3), 0, 23, "Port number",
            NUMBER_DEC | NUMBER_WS_OK );
CHARACTER  (show_modem_slot_separator, show_modem_port_num,
            NONE, '/');
GENERAL_NUMBER (show_modem_slot_num, show_modem_slot_separator,
            show_modem_group, OBJ(int,2), 0, 2, "Slot/Port number (i.e. 1/1)",
            NUMBER_DEC | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);

/***************************************************************
 * show modem
 *
 */
SET        (show_modem, show_modem_slot_num, OBJ(int,1), SHOW_MODEM_STATS);

/***************************************************************
 * show modem csm
 *
 */
KEYWORD_ID (show_modem_csm, show_modem_slot_num,
            show_modem,
            OBJ(int,1), SHOW_MODEM_CSM,
            "csm", "CSM modem information", PRIV_USER);

/***************************************************************
 * show modem version
 *
 */
KEYWORD_ID (show_modem_version, show_modem_eol,
            show_modem_csm,
            OBJ(int,1), SHOW_MODEM_VERSION,
            "version", "Version information for all system modems",
            PRIV_USER);

/***************************************************************
 * show modem summary
 *
 */
KEYWORD_ID (show_modem_summary, show_modem_eol,
            show_modem_version,
            OBJ(int,1), SHOW_MODEM_SUMMARY,
            "summary", "Summary statistics for all system modems",
            PRIV_USER);

/***************************************************************
 * show modem log
 *
 */
KEYWORD_ID (show_modem_log, show_modem_slot_num,
            NONE,
            OBJ(int,1), SHOW_MODEM_LOG,
            "log", "Modem event log", PRIV_USER);


/***************************************************************
 * show modem at-mode
 *
 */
KEYWORD_ID (show_modem_at_mode, show_modem_eol,
            show_modem_log,
            OBJ(int,1), SHOW_MODEM_AT_MODE,
            "at-mode", "AT session connections",
            PRIV_USER);

/***************************************************************
 * check to see if it is manageable modem image, only certain
 * management commands are valid for manageable image
 */
ASSERT  (show_modem_manageable, show_modem_at_mode, show_modem_summary, 
         reg_invoke_modem_mgmt_exists());

KEYWORD    (show_modem_mgmt, show_modem_manageable, ALTERNATE,
            "modem", "Modem Management or CSM information", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_modem_mgmt
