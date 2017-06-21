/* $Id: exec_show_modem_mgmt.h,v 3.1.68.1 1996/06/16 21:14:24 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/les/exec_show_modem_mgmt.h,v $
 *------------------------------------------------------------------
 * A dummy file for brasil branch.
 * E X E C _ S H O W _ M O D E M _ M G M T . H
 *
 * November, 1995 - Jonathan Chiung-Hsi Lin
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_modem_mgmt.h,v $
 * Revision 3.1.68.1  1996/06/16  21:14:24  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/06  19:12:26  jchlin
 * Placeholders for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(show_modem_mgmt_extend_here, no_alt);

/* forward declarations */
pdecl(show_modem_mgmt_port_num1);
pdecl(show_modem_mgmt_port_num1w);
 
/****************************************************************************
 * slot-group <number> ports <range>
 *
 * <range>      :: <range entry> | <range> ',' <range_entry>
 * <range_entry>:: <number> | <number> '-' <number>
 *
 *      OBJ(int,1) = slot number
 *      OBJ(int,2) = ports (bit field)
 *      OBJ(int,3) - (used as a temp variable)
 *      OBJ(int,4) - (used as a temp variable)
 */
EOLS	(show_modem_mgmt_csm_eol, modem_mgmt_show_command,
	 MODEM_MGMT_SHOW_CSM_MODEM_INFO);
 
CHARACTER(show_modem_mgmt_port_num4, show_modem_mgmt_port_num1w, show_modem_mgmt_csm_eol, ',');

EVAL    (show_modem_mgmt_port_num3a, show_modem_mgmt_port_num4, SETOBJ(int,2) =
         GETOBJ(int,2) | csm_make_bit_field(GETOBJ(int,3), GETOBJ(int,4)));

GENERAL_NUMBER(show_modem_mgmt_port_num3, show_modem_mgmt_port_num3a, no_alt, OBJ(int,4),
               0, 31, "List of ports which comprise the slot",
               NUMBER_DEC| NUMBER_WS_OK | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);

WHITESPACE(show_modem_mgmt_port_num1w, show_modem_mgmt_port_num1, show_modem_mgmt_port_num1);
WHITESPACE(show_modem_mgmt_port_num3w, show_modem_mgmt_port_num3, show_modem_mgmt_port_num3);
CHARACTER(show_modem_mgmt_port_num2a, show_modem_mgmt_port_num1w, show_modem_mgmt_csm_eol, ',');
CHARACTER(show_modem_mgmt_port_num2, show_modem_mgmt_port_num3w, show_modem_mgmt_port_num2a, '-');

EVAL    (show_modem_mgmt_port_num1a, show_modem_mgmt_port_num2,
         SETOBJ(int,2) =
               GETOBJ(int,2) | csm_make_bit_field(GETOBJ(int,3), GETOBJ(int,3)));

GENERAL_NUMBER(show_modem_mgmt_port_num1, show_modem_mgmt_port_num1a, no_alt, OBJ(int,3),
               0, 31, "List of ports which comprise the slot",
               NUMBER_DEC| NUMBER_WS_OK | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);

SET     (show_modem_mgmt_port_num0, show_modem_mgmt_port_num1, OBJ(int,2), 0);

KEYWORD (show_modem_mgmt_port, show_modem_mgmt_port_num0, no_alt, "ports",
         "List of ports in the slot group",
         PRIV_USER);

NUMBER  (show_modem_mgmt_slot_num, show_modem_mgmt_port, no_alt, OBJ(int,1), 0,
         2, "Slot number");

KEYWORD (show_modem_mgmt_slot, show_modem_mgmt_slot_num, show_modem_mgmt_extend_here, 
         "slot-group",
         "Specify the ports to slot-group mapping for an interface",
         PRIV_USER);
 
/******************************************************************
 * show modem-mgmt csm lists
 */
EOLS	(show_modem_mgmt_list_eol, modem_mgmt_show_command,
	 MODEM_MGMT_SHOW_CSM_LIST);

KEYWORD (show_modem_mgmt_csm_list, show_modem_mgmt_list_eol,
	 show_modem_mgmt_slot, "lists",
	 "Show CSM modem info lists", PRIV_USER);


/******************************************************************
 * show modem-mgmt csm
 */
KEYWORD (show_modem_mgmt_csm, show_modem_mgmt_csm_list,
	 show_modem_mgmt_extend_here, "csm",
	 "Show Call Switching Module State", PRIV_USER);

/******************************************************************/
KEYWORD_MM (show_modem_mgmt, show_modem_mgmt_csm, ALTERNATE,
	 MODEM_MGMT_KEYWORD, "Show Modem Management information", PRIV_INTERNAL, 6);

#undef	ALTERNATE
#define	ALTERNATE	show_modem_mgmt

