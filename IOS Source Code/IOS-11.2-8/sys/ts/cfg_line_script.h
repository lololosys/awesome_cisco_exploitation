/* $Id: cfg_line_script.h,v 3.1 1996/02/01 04:59:32 hampton Exp $
 * $Source: /release/111/cvs/Xsys/ts/cfg_line_script.h,v $
 *------------------------------------------------------------------
 * Line Scripts commands
 *
 * May'94, Syed Irfan Ashraf
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * 
 *------------------------------------------------------------------
 * $Log: cfg_line_script.h,v $
 * Revision 3.1  1996/02/01  04:59:32  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:44:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/21  18:17:08  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.2  1995/06/13  16:36:23  syiu
 * CSCdi35685:  Async callback rotary should be in reverse oder
 *
 * Revision 2.1  1995/06/07  22:10:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Line subcommands:
 * Script { startup | connection | dialer | reset | activation | callback |
 *          arap-callback | change }\
 *        <script-name>
 *
 * no Script { startup | connection | dialer | reset | activation | callback |
 *             arap-callback | change } <script-name>
 *
 *   * Please note: Script line-change is out of the chain at this time.
 * 
 *    script startup    <script-name>
 *    script reset      <script-name>
 *    script connection <script-name>
 *    script dialer     <script-name>
 *    script activation <script-name>
 *    script callback   <script-name>
 *    script arap-callback <script-name>
 */


/*--------------------------------------------------------------------------*/
/*
 ***************************************************************
 * Line subcommand:
 *     script change     <script-name>
 *  no script change     <script-name>
 ***************************************************************
 *  this command is out of the parser chain at the moment.
 */

EOLS    (line_script_change_eol, script_line_commands,
	 PARSER_LINE_SCRIPT_LINE_CHANGE);

STRING  (line_script_change_name, line_script_change_eol, no_alt,
	OBJ(string,1), "chat script name");

NOPREFIX(line_script_change_noprefix, line_script_change_name,
	 line_script_change_eol);

NVGENS  (line_script_change_nvgen, line_script_change_noprefix,
	 script_line_commands, PARSER_LINE_SCRIPT_LINE_CHANGE);
	 
KEYWORD (line_script_change, line_script_change_nvgen, no_alt,
	 "line-change", "chat script to run whenever line"
	 " configuration is changed", PRIV_CONF);


/*--------------------------------------------------------------------------*/

/* 
 ***************************************************************
 * Line subcommand:
 *     script callback <script-name>
 *  no script callback <script-name>
 ***************************************************************
 * Please note: Script line-change is out of the chain at this time.
 */

EOLS    (line_script_callback_eol, script_line_commands,
	 PARSER_LINE_SCRIPT_CALLBACK);

STRING  (line_script_callback_name, line_script_callback_eol, no_alt,
	 OBJ(string,1), "chat script name");

NOPREFIX(line_script_callback_noprefix, line_script_callback_name,
	 line_script_callback_eol);

NVGENS  (line_script_callback_nvgen, line_script_callback_noprefix,
	 script_line_commands, PARSER_LINE_SCRIPT_CALLBACK);
	 
KEYWORD (line_script_callback, line_script_callback_nvgen,
	 no_alt,
	 "callback", "chat script to run on line whenever callback is"\
         " initiated", PRIV_CONF);

/*--------------------------------------------------------------------------*/

/* 
 ***************************************************************
 * Line subcommand:
 *     script arap-callback <script-name>
 *  no script arap-callback <script-name>
 ***************************************************************
 */

EOLS    (line_script_arapcallback_eol, script_line_commands,
	 PARSER_LINE_SCRIPT_ARAPCALLBACK);

STRING  (line_script_arapcallback_name, line_script_arapcallback_eol, no_alt,
	 OBJ(string,1), "chat script name");

NOPREFIX(line_script_arapcallback_noprefix, line_script_arapcallback_name,
	 line_script_arapcallback_eol);

NVGENS  (line_script_arapcallback_nvgen, line_script_arapcallback_noprefix,
	 script_line_commands, PARSER_LINE_SCRIPT_ARAPCALLBACK);
	 
KEYWORD (line_script_arapcallback, line_script_arapcallback_nvgen,
	 line_script_callback,
	 "arap-callback", "chat script to run on line whenever ARAP callback is"\
         " initiated", PRIV_CONF);

/*--------------------------------------------------------------------------*/

/* 
 ***************************************************************
 * Line subcommand:
 *     script modem-off-hook <script-name>
 *  no script modem-off-hook <script-name>
 ***************************************************************
 */

EOLS    (line_script_modemoffhook_eol, script_line_commands,
	 PARSER_LINE_SCRIPT_OFFHOOK);

STRING  (line_script_modemoffhook_name, line_script_modemoffhook_eol, no_alt,
	 OBJ(string,1), "chat script name");

NOPREFIX(line_script_modemoffhook_noprefix, line_script_modemoffhook_name,
	 line_script_modemoffhook_eol);

NVGENS  (line_script_modemoffhook_nvgen, line_script_modemoffhook_noprefix,
	 script_line_commands, PARSER_LINE_SCRIPT_OFFHOOK);
	 
KEYWORD (line_script_modemoffhook, line_script_modemoffhook_nvgen,
	 line_script_arapcallback,
	 "modem-off-hook", "chat script to run on line when callback "\
         "off-hooks(reserves) modem before callback is initiated", 
         PRIV_CONF|PRIV_HIDDEN);

/*--------------------------------------------------------------------------*/

/* 
 ***************************************************************
 * Line subcommand:
 *     script activation <script-name>
 *  no script activation <script-name>
 ***************************************************************
 */

EOLS    (line_script_activation_eol, script_line_commands,
	 PARSER_LINE_SCRIPT_ACTIVATION);

STRING  (line_script_activation_name, line_script_activation_eol, no_alt,
	 OBJ(string,1), "chat script name");

NOPREFIX(line_script_activation_noprefix, line_script_activation_name,
	 line_script_activation_eol);

NVGENS  (line_script_activation_nvgen, line_script_activation_noprefix,
	 script_line_commands, PARSER_LINE_SCRIPT_ACTIVATION);
	 
KEYWORD (line_script_activation, line_script_activation_nvgen,
	 line_script_modemoffhook,
	 "activation", "chat script to run whenever line is"\
	 " activated", PRIV_CONF);


/*--------------------------------------------------------------------------*/
/*
 ***************************************************************
 * Line subcommand:
 *     script reset      <script-name>
 *  no script reset      <script-name>
 ***************************************************************
 */

EOLS    (line_script_reset_eol, script_line_commands,
	 PARSER_LINE_SCRIPT_RESET);

STRING  (line_script_reset_name, line_script_reset_eol, no_alt,
	 OBJ(string,1), "chat script name");

NOPREFIX(line_script_reset_noprefix, line_script_reset_name,
	 line_script_reset_eol);

NVGENS  (line_script_reset_nvgen, line_script_reset_noprefix,
	 script_line_commands, PARSER_LINE_SCRIPT_RESET);
	 
KEYWORD (line_script_reset, line_script_reset_nvgen, line_script_activation,
	 "reset", "chat script to run whenever line is reset", PRIV_CONF);


/*--------------------------------------------------------------------------*/
/*
 ***************************************************************
 * Line subcommand:
 *     script dialer     <script-name>
 *  no script dialer     <script-name>
 ***************************************************************
 */

EOLS    (line_script_dialer_eol, script_line_commands,
	 PARSER_LINE_SCRIPT_DIALER);

STRING  (line_script_dialer_name, line_script_dialer_eol, no_alt,
	OBJ(string,1), "chat script name");

NOPREFIX(line_script_dialer_noprefix, line_script_dialer_name,
	 line_script_dialer_eol);

NVGENS  (line_script_dialer_nvgen, line_script_dialer_noprefix,
	 script_line_commands, PARSER_LINE_SCRIPT_DIALER);
	 
KEYWORD (line_script_dialer, line_script_dialer_nvgen, line_script_reset,
	 "dialer", "chat script to run whenever dialer makes an"
	 " outgoing call", PRIV_CONF);


/*--------------------------------------------------------------------------*/
/*
 ***************************************************************
 * Line subcommand:
 *     script connection <script-name>
 *  no script connection <script-name>
 ***************************************************************
 */

EOLS    (line_script_connection_eol, script_line_commands,
	 PARSER_LINE_SCRIPT_CONNECTION);

STRING  (line_script_connection_name, line_script_connection_eol, no_alt,
	OBJ(string,1), "chat script name");

NOPREFIX(line_script_connection_noprefix, line_script_connection_name,
	 line_script_connection_eol);

NVGENS  (line_script_connection_nvgen, line_script_connection_noprefix,
	 script_line_commands, PARSER_LINE_SCRIPT_CONNECTION);
	 
KEYWORD (line_script_connection, line_script_connection_nvgen,
	 line_script_dialer,
	 "connection", "chat script to run whenever connection is made"
	 " to the line", PRIV_CONF);


/*--------------------------------------------------------------------------*/
/*
 ***************************************************************
 * Line subcommand:
 *     script startup    <script-name>
 *  no script startup    <script-name>
 ***************************************************************
 */

EOLS    (line_script_startup_eol, script_line_commands,
	 PARSER_LINE_SCRIPT_STARTUP);

STRING  (line_script_startup_name, line_script_startup_eol, no_alt,
	OBJ(string,1), "chat script name");

NOPREFIX(line_script_startup_noprefix, line_script_startup_name,
	 line_script_startup_eol);

NVGENS  (line_script_startup_nvgen, line_script_startup_noprefix,
	 script_line_commands, PARSER_LINE_SCRIPT_STARTUP);
	 
KEYWORD (line_script_startup, line_script_startup_nvgen,
	 line_script_connection,
	 "startup", "chat script to run at system startup", PRIV_CONF);

/**************************************************************/

KEYWORD	(line_script_cmds, line_script_startup, ALTERNATE,
	 "script", "specify event related chat scripts to run on the line",
         PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	line_script_cmds
