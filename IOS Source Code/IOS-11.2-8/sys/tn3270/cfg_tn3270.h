/* $Id: cfg_tn3270.h,v 3.4.6.2 1996/08/12 16:08:16 widmer Exp $
 * $Source: /release/112/cvs/Xsys/tn3270/cfg_tn3270.h,v $
 *------------------------------------------------------------------
 * C F G _ T N 3 2 7 0 . H
 *
 * February 1993, Nick Thille
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Parse chain for tn3270 config commands
 *------------------------------------------------------------------
 * $Log: cfg_tn3270.h,v $
 * Revision 3.4.6.2  1996/08/12  16:08:16  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4.6.1  1996/03/27  23:27:51  irfan
 * CSCdi51821:  tn3270 should allow typeahead
 * Branch: California_branch
 * Add typeahead buffer to hold keyboard input while keyboard appears
 * locked. Add a TN3270 UI knob to control this behaviour. Reduce the
 * amount
 * of whitespace painted
 *
 * Revision 3.4  1996/03/07  00:37:26  irfan
 * CSCdi48513:  TN3270 sends excessive cursor-move strings
 * Add "[no] tn3270 optimize-cursor-move" & "[no] tn3270 status-message".
 *
 * Revision 3.3  1996/02/01  23:34:38  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  18:50:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:34  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:14:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * [no] tn3270 typeahead
 *
 * OBJ(int,1) = Whether to permit typeahead
 */
EOLS  (cfg_tn3270_type1, tn3270_config_command, TN3270_TYPEAHEAD);

KEYWORD (cfg_tn3270_typeahead, cfg_tn3270_type1, NONE,
	 "typeahead", "Buffer typing while keyboard locked",
	 PRIV_CONF);

/******************************************************************
 * tn3270 status-message
 * no tn3270 status-message
 */

EOLS	(cfg_tn3270_statusmsg_eol, tn3270_config_command, 
	 TN3270_STATUS_MSG);

NVGENS	(cfg_tn3270_statusmsg_nvcheck, cfg_tn3270_statusmsg_eol,
	 tn3270_config_command, TN3270_STATUS_MSG);

KEYWORD (cfg_tn3270_statusmsg, cfg_tn3270_statusmsg_nvcheck,
	 cfg_tn3270_typeahead,
	 "status-message",
	 "Enable Status Messages",
	 PRIV_CONF);

/******************************************************************
 * tn3270 optimize-cursor-move
 * no tn3270 optimize-cursor-move
 */

EOLS	(cfg_tn3270_optimizepos_eol, tn3270_config_command, 
	 TN3270_OPTIMIZE_POSITION);

NVGENS	(cfg_tn3270_optimizepos_nvcheck, cfg_tn3270_optimizepos_eol,
	 tn3270_config_command, TN3270_OPTIMIZE_POSITION);

KEYWORD (cfg_tn3270_optimizepos, cfg_tn3270_optimizepos_nvcheck,
	 cfg_tn3270_statusmsg,
	 "optimize-cursor-move",
	 "Attempt to optimize cursor positioning",
	 PRIV_CONF);

/******************************************************************
 * tn3270 character-map <ebcdic in hex> <ascii in hex>
 * no tn3270 character-map {<ebcdic in hex> | ALL}
 *
 * OBJ(int,1)	= ebcdic value entered
 * OBJ(int,2)	= ascii value entered
 * OBJ(int,3)	= TRUE if ALL was parsed
 *              = FALSE if ALL was not parsed
 */

EOLNS	(cfg_tn3270_char_map_eol, tn3270_char_map_command);

OPT_HEXDIGIT (cfg_tn3270_setasc, cfg_tn3270_char_map_eol, no_alt,
	      OBJ(int, 2), MIN_ASCII, MAX_ASCII, 
	      "hexadecimal value of ASCII character");

NOPREFIX (cfg_tn3270_nocheck, cfg_tn3270_setasc, cfg_tn3270_char_map_eol);

OPT_HEXDIGIT (cfg_tn3270_setebc, cfg_tn3270_nocheck, no_alt,
	      OBJ(int, 1), MIN_EBCDIC, MAX_EBCDIC, 
	      "hexadecimal value of EBCDIC character");

KEYWORD_ID_MM(cfg_tn3270_unsetmap, cfg_tn3270_char_map_eol, cfg_tn3270_setebc,
	      OBJ(int,3), TRUE,
	      "all", "Clear all EBCDIC <=> ASCII mapping", PRIV_CONF, 2);

SPLIT	(cfg_tn3270_setting, cfg_tn3270_setebc, cfg_tn3270_unsetmap,
	 sense, TRUE);

NVGENNS	(cfg_tn3270_nvcheck, cfg_tn3270_setting, tn3270_char_map_command);

KEYWORD (cfg_tn3270_char_map, cfg_tn3270_nvcheck,
	 cfg_tn3270_optimizepos,
	 "character-map", "Change EBCDIC <=> ASCII mapping",
	 PRIV_CONF);

/******************************************************************
 * tn3270 require-reset
 * no tn3270 require-reset
 *
 * OBJ(int,1)	= TRUE if wants to require user reset after input error
 * 		= FALSE if doesn't want to require user reset.
 */

EOLS	(cfg_tn3270_reset_req_eol, tn3270_config_command, 
	 TN3270_RESET_REQUIRED);

NVGENS	(cfg_tn3270_reset_req_nvcheck, cfg_tn3270_reset_req_eol,
	 tn3270_config_command, TN3270_RESET_REQUIRED);

KEYWORD_ID (cfg_tn3270_reset_req, cfg_tn3270_reset_req_nvcheck,
	    cfg_tn3270_char_map, 
	    OBJ(int, 1), TRUE, 	 
	    "reset-required", "Require user to enter reset after input error",
	    PRIV_CONF);

/******************************************************************
 * tn3270 null-processing [3270|7171]
 * no tn3270 null-processing [3270|7171]

 *
 * OBJ(int,1)	= TRUE if wants 3270 style null processing
 * 		= FALSE if wants 7171 style null processing
 */

EOLS	(cfg_tn3270_null_proc_eol, tn3270_config_command,
	 TN3270_NULL_PROCESSING);

NVGENS	(cfg_tn3270_null_proc_nvcheck, cfg_tn3270_null_proc_eol,
	 tn3270_config_command, TN3270_NULL_PROCESSING);

KEYWORD_ID (cfg_tn3270_null_proc_3270, cfg_tn3270_null_proc_nvcheck, 
	    no_alt,
	    OBJ(int, 1), TRUE,
	    "3270", "Use 3270-style null processing", PRIV_CONF);

KEYWORD	(cfg_tn3270_null_proc_improved, cfg_tn3270_null_proc_nvcheck, 
	 cfg_tn3270_null_proc_3270,
	 "7171", "Use 7171-style null processing", 
	 PRIV_CONF | PRIV_NONVGEN);

NOPREFIX (cfg_tn3270_null_proc_noprefix, cfg_tn3270_null_proc_improved,
	  cfg_tn3270_null_proc_eol);

KEYWORD (cfg_tn3270_null_proc, cfg_tn3270_null_proc_noprefix,
	 cfg_tn3270_reset_req, 
	 "null-processing",
	 "Configure processing of nulls in user input.", 
	 PRIV_CONF);

/******************************************************************
 * tn3270 datastream [normal|extended]
 * no tn3270 datastream [normal|extended]
 *
 * OBJ(int,1)	= TRUE if wants extended datastream
 * 		= FALSE if doesn't want extended datastream
 */

EOLS	(cfg_tn3270_datastream_eol, tn3270_config_command,
	 TN3270_DATASTREAM);

NVGENS	(cfg_tn3270_datastream_nvcheck, cfg_tn3270_datastream_eol,
	 tn3270_config_command, TN3270_DATASTREAM); 

KEYWORD_ID (cfg_tn3270_datastream_extended, cfg_tn3270_datastream_nvcheck, 
	    no_alt,
	    OBJ(int, 1), TRUE,
	    "extended", "Use extended TN3270 datastream", PRIV_CONF);

KEYWORD	(cfg_tn3270_datastream_normal, cfg_tn3270_datastream_nvcheck, 
	 cfg_tn3270_datastream_extended,
	 "normal", "Use normal TN3270 datastream", 
	 PRIV_CONF | PRIV_NONVGEN);

NOPREFIX (cfg_tn_datastream_noprefix, cfg_tn3270_datastream_normal,
	  cfg_tn3270_datastream_eol);

KEYWORD (cfg_tn3270_datastream, cfg_tn_datastream_noprefix, 
	 cfg_tn3270_null_proc, 
	 "datastream",
	 "Configure support for normal or extended datastream.", 
	 PRIV_CONF);

/******************************************************************
 * tn3270 8bit [ display | transparent-mode ]
 * no tn3270 8bit [ display | transparent-mode ]
 *
 * OBJ(int,1)	= TRUE if dealing with display characters
 * OBJ(int,2)	= TRUE if dealing with transparent-mode data
 */

EOLS	(cfg_tn3270_8bit_eol, tn3270_config_command, TN3270_8_BIT_CMD);

NVGENS	(cfg_tn3270_8bit_nvcheck, cfg_tn3270_8bit_eol,
	 tn3270_config_command, TN3270_8_BIT_CMD);

KEYWORD_ID (cfg_tn3270_8bit_transparent, cfg_tn3270_8bit_nvcheck, NONE,
	    OBJ(int, 2), TRUE,
	    "transparent-mode", 
	    "Use full 8 bit characters when in transparent mode", PRIV_CONF);

KEYWORD_ID (cfg_tn3270_8bit_display, cfg_tn3270_8bit_nvcheck, 
	    cfg_tn3270_8bit_transparent,
	    OBJ(int, 1), TRUE,
	    "display", "Use full 8 bit characters on display", PRIV_CONF);

KEYWORD (cfg_tn3270_8bit, cfg_tn3270_8bit_display, cfg_tn3270_datastream, 
	 "8bit", "Use full 8 bit characters for display or transparent mode", 
	 PRIV_CONF);

KEYWORD	(cfg_tn3270_command, cfg_tn3270_8bit, ALTERNATE,
	 "tn3270", "tn3270 configuration command", PRIV_CONF);

#undef	ALTERNATE
#define ALTERNATE	cfg_tn3270_command
