/* $Id: cfg_defaultval.h,v 3.2.62.1 1996/06/17 23:35:38 hampton Exp $
 * $Source: /release/112/cvs/Xsys/parser/cfg_defaultval.h,v $
 *------------------------------------------------------------------
 * C F G _ D E F A U L T V A L . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_defaultval.h,v $
 * Revision 3.2.62.1  1996/06/17  23:35:38  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:42:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:02:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*********************************************************************
 * default-value { exec-character-bits | special-character-bits |
 *		   data-character-bits }  { 7 | 8 }
 * default-value modem-interval <mSecs>
 */

PARAMS_KEYONLY(cfg_default_modint, no_alt, "modem-interval",
	       OBJ(int,1), 0, 1000,
	       defaultval_command, PARSER_DEFAULTCMD_MODINT,
	       "Frequency to scan MODEM signals", "milli-seconds",
	       PRIV_CONF);
PARAMS_KEYONLY(cfg_default_datacb, cfg_default_modint, "data-character-bits",
	       OBJ(int,1), 7, 8,
	       defaultval_command, PARSER_DEFAULTCMD_DATAMASK,
	       "Size of characters being handled", "Bits per character",
	       PRIV_CONF);
PARAMS_KEYONLY(cfg_default_speccb, cfg_default_datacb, "special-character-bits",
	       OBJ(int,1), 7, 8,
	       defaultval_command, PARSER_DEFAULTCMD_SPECIALMASK,
	       "Size of the escape (and other special) characters", 
	       "Bits per character", PRIV_CONF);
PARAMS_KEYONLY(cfg_default_execcb, cfg_default_speccb, "exec-character-bits",
	       OBJ(int,1), 7, 8,
	       defaultval_command, PARSER_DEFAULTCMD_EXECMASK,
	       "Size of characters to the command exec", "Bits per character",
	       PRIV_CONF);

KEYWORD_MM(cfg_default, cfg_default_execcb, ALTERNATE,
	 "default-value", "Default character-bits values", PRIV_CONF, 8);


#undef	ALTERNATE
#define	ALTERNATE	cfg_default
