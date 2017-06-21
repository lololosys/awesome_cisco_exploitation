/* $Id: exec_llc2.h,v 3.3 1996/01/18 02:13:38 tli Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/llc2/exec_llc2.h,v $
 *------------------------------------------------------------------
 * E X E C _ L L C 2 . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_llc2.h,v $
 * Revision 3.3  1996/01/18  02:13:38  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.2  1995/11/17  17:43:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:35:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:54:38  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:21:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * llc2 offset <num>
 */
EOLS	(exec_llc2_offset_eol, llc2_test_command, PARSER_LLC_OFFSET );
GENERAL_NUMBER(exec_llc2_offset_num, exec_llc2_offset_eol, no_alt,
               OBJ(int,1), 0, (2<<16)-1, "Packet offset",
               (NUMBER_HEX | HEX_NO_ZEROX_OK | NUMBER_WS_OK | NUMBER_HELP_CHECK) );
KEYWORD	(exec_llc2_offset, exec_llc2_offset_num, no_alt,
	 "offset", "Set send command packet offset", PRIV_OPR );

/******************************************************************
 * llc2 stat
 */
EOLS	(exec_llc2_stat_eol, llc2_test_command, PARSER_LLC_STAT );
KEYWORD	(exec_llc2_stat, exec_llc2_stat_eol, exec_llc2_offset,
	 "stat", "Status of circuits", PRIV_OPR );


/******************************************************************
 * llc2 close <cookie>
 */
EOLS	(exec_llc2_close_eol, llc2_test_command, PARSER_LLC_CLOSE );
GENERAL_NUMBER(exec_llc2_close_cookie, exec_llc2_close_eol, no_alt,
	       OBJ(int,1), 0, (2<<16)-1, "Cookie number",
	       (NUMBER_HEX | HEX_NO_ZEROX_OK | NUMBER_WS_OK | NUMBER_HELP_CHECK) );
KEYWORD	(exec_llc2_close, exec_llc2_close_cookie, exec_llc2_stat,
	 "close", "Close the circuit", PRIV_OPR );


/******************************************************************
 * llc2 send <cookie> [<hexdata>]
 */
EOLS	(exec_llc2_send_eol, llc2_test_command, PARSER_LLC_SEND );
HEXDATA	(exec_llc2_send_data, exec_llc2_send_eol, exec_llc2_send_eol,
	 OBJ(string,1), OBJ(int,1), "Data to send", 100 );
GENERAL_NUMBER(exec_llc2_send_cookie, exec_llc2_send_data, no_alt,
	       OBJ(int,2), 0, (2<<16)-1, "Cookie number",
	       (NUMBER_HEX | HEX_NO_ZEROX_OK | NUMBER_WS_OK | NUMBER_HELP_CHECK) );
KEYWORD	(exec_llc2_send, exec_llc2_send_cookie, exec_llc2_close,
	 "send", "Send some data", PRIV_OPR );


/******************************************************************
 * llc2 attach <interface> <hexdata>
 */
EOLS	(exec_llc2_attach_eol, llc2_test_command, PARSER_LLC_ATTACH );
HEXDATA	(exec_llc2_attach_mac, exec_llc2_attach_eol, no_alt,
	 OBJ(string,1), OBJ(int,1), "Destination MAC address", IEEEBYTES );
INTERFACE(exec_llc2_attach_int, exec_llc2_attach_mac, no_alt,
	  OBJ(idb,1), IFTYPE_HWIDB );
KEYWORD	(exec_llc2_attach, exec_llc2_attach_int, exec_llc2_send,
	 "attach", "Attach an existing connection", PRIV_OPR );


/******************************************************************
 *  llc2 open <interface> <hexdata> [n]
 */
EOLS	(exec_llc2_open_eol, llc2_test_command, PARSER_LLC_OPEN );
KEYWORD_ID(exec_llc2_open_nonblocking, exec_llc2_open_eol, exec_llc2_open_eol,
	   OBJ(int,2), TRUE,
	   "n", "Non-blocking", PRIV_OPR );
HEXDATA	(exec_llc2_open_mac, exec_llc2_open_nonblocking, no_alt,
	 OBJ(string,1), OBJ(int,1), "Destination MAC address", IEEEBYTES );
INTERFACE(exec_llc2_open_int, exec_llc2_open_mac, no_alt,
	  OBJ(idb,1), IFTYPE_HWIDB     );
KEYWORD	(exec_llc2_open, exec_llc2_open_int, exec_llc2_attach,
	 "open", "Open a circuit", PRIV_OPR );


/******************************************************************/
KEYWORD	(exec_llc2, exec_llc2_open, ALTERNATE,
	 "llc2", "Execute llc2 tests", PRIV_OPR|PRIV_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	exec_llc2
