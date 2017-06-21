/* $Id: exec_show_isdn.h,v 3.4.20.2 1996/05/23 19:28:24 hrobison Exp $
 * $Source: /release/112/cvs/Xsys/wan/exec_show_isdn.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ I S D N . H
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_isdn.h,v $
 * Revision 3.4.20.2  1996/05/23  19:28:24  hrobison
 * CSCdi58513:  show isdn status displays dialer option when dialer int
 * defined
 * Branch: California_branch
 *
 * Revision 3.4.20.1  1996/05/01  14:52:45  hrobison
 * CSCdi46413:  MIP when used as PRI must be in slot 0-4
 * Branch: California_branch
 * Change PRI initialization: for the 7xxx platform the
 * dsl id is now assigned independently of the MIP card
 * slot/subunit location.  The number of PRI interfaces
 * per router is still limited to 10.  The dsl assignment
 * for the 4xxx platform is based on the slot location
 * and corrected to allow only 1 dsl id per slot.
 *
 * Read bug release notes for details on how the ISDN
 * debug output and ISDN show commands have changed.
 * The debug output now uses the hw_short_namestring
 * rather than PRI<dsl #> or BRI<dsl #>.  The show commands
 * now take the interface name or the dsl id as input.
 *
 * Revision 3.4  1996/02/01  23:34:59  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.3  1995/12/07  17:45:39  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.2  1995/11/17  18:02:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/10/25  04:36:07  rbeach
 * CSCdi42110:  Sho isdn status command invalid for MBRI dsl values
 * greater than 9. Fix the parser to allow values up to 15.
 *
 * Revision 2.3  1995/10/21  06:37:40  bdas
 * CSCdi36921:  ISDN MIB needs to contain info about active calls
 *
 * Revision 2.2  1995/08/12  04:21:48  bdas
 * CSCdi36875:  Need to provide a show isdn history command for call
 * history mib
 *
 * Revision 2.1  1995/07/02  01:46:10  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:24:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
 /******************************************************************
  * show isdn active <dsl id 0-15> || <interface name>
  */
EOLS    (show_isdn_active_eol, show_isdn, SHOW_ISDN_ACTIVE);
INTERFACE(show_isdn_active_if, show_isdn_active_eol, show_isdn_active_eol,
           OBJ(idb,1), 
           IFTYPE_SERIAL | IFTYPE_BRI);
GENERAL_NUMBER(show_isdn_active_int, show_isdn_active_eol, show_isdn_active_if,
           OBJ(int,1), 0, 15, "List of DSL's",
           NUMBER_DEC| NUMBER_WS_OK | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);
KEYWORD_ID(show_isdn_active, show_isdn_active_int, no_alt,
	   OBJ(int,1), -1,
           "active", "ISDN active calls ", PRIV_USER | PRIV_HIDDEN);

 /******************************************************************
 * show isdn history <dsl id 0-15> || <interface name>
 */
EOLS    (show_isdn_history_eol, show_isdn, SHOW_ISDN_HISTORY);
INTERFACE(show_isdn_history_if, show_isdn_history_eol, show_isdn_history_eol,
           OBJ(idb,1), 
           IFTYPE_SERIAL | IFTYPE_BRI);
GENERAL_NUMBER(show_isdn_history_int, show_isdn_history_eol, show_isdn_history_if,
           OBJ(int,1), 0, 15, "List of DSL's",
           NUMBER_DEC| NUMBER_WS_OK | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);
KEYWORD_ID(show_isdn_history, show_isdn_history_int, show_isdn_active,
	   OBJ(int,1), -1,
           "history", "ISDN call history", PRIV_USER | PRIV_HIDDEN);

/******************************************************************
 * show isdn status <dsl id 0-15> || <interface name>
 */
EOLS    (show_isdn_status_eol, show_isdn, SHOW_ISDN_STATUS);
INTERFACE(show_isdn_status_if, show_isdn_status_eol, show_isdn_status_eol,
           OBJ(idb,1), 
           IFTYPE_SERIAL | IFTYPE_BRI);
GENERAL_NUMBER(show_isdn_dsl_int, show_isdn_status_eol, show_isdn_status_if,
           OBJ(int,1), 0, 15, "List of DSL's",
           NUMBER_DEC| NUMBER_WS_OK | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);
KEYWORD_ID(show_isdn_status, show_isdn_dsl_int, show_isdn_history,
	   OBJ(int,1), -1,
           "status", "ISDN Line Status", PRIV_USER);

/******************************************************************
 * show isdn timers 
 */
EOLS    (show_isdn_timers_eol, show_isdn, SHOW_ISDN_TIMERS);
KEYWORD (show_isdn_timers, show_isdn_timers_eol, show_isdn_status,
           "timers", "ISDN Timer values", PRIV_USER);

/******************************************************************
 * show isdn memory
 */
EOLS	(show_isdn_memory_eol, show_isdn, SHOW_ISDN_MEMORY);
KEYWORD (show_isdn_memory, show_isdn_memory_eol, show_isdn_timers,
            "memory", "ISDN memory information", PRIV_USER);

/******************************************************************
 * show isdn service <dsl id 0-15> || <interface name>
 */
EOLS	(show_isdn_channels_eol, show_isdn, SHOW_ISDN_CHANNELS);
INTERFACE(show_isdn_channels_if, show_isdn_channels_eol, show_isdn_channels_eol,
           OBJ(idb,1), 
           IFTYPE_SERIAL | IFTYPE_BRI);
GENERAL_NUMBER(show_isdn_int, show_isdn_channels_eol, show_isdn_channels_if,
           OBJ(int,1), 0, 15, "List of DSL's",
           NUMBER_DEC| NUMBER_WS_OK | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);
KEYWORD_ID(show_isdn_channels, show_isdn_int, NONE,
	   OBJ(int,1), -1,
	   "service", "ISDN service information", PRIV_USER);
TEST_INT(show_isdn_test, show_isdn_channels, NONE,
	 show_isdn_memory, nprinets);

/******************************************************************
 * show isdn
 */
KEYWORD (show_isdn_info, show_isdn_test, NONE,
            "isdn", "ISDN information", PRIV_USER);

ASSERT	(show_isdn, show_isdn_info, ALTERNATE,
	 nbrinets || nprinets);

#undef	ALTERNATE
#define	ALTERNATE	show_isdn
