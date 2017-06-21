/* $Id: cfg_line_stopbits.h,v 3.2 1995/11/17 18:45:30 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/cfg_line_stopbits.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ S T O P B I T S . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_stopbits.h,v $
 * Revision 3.2  1995/11/17  18:45:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:01  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:10:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * stopbits { 1 | 1.5 | 2 }
 *
 * OBJ(int,1) = STOPBITS1, STOPBITS15, STOPBITS2
 */

EOLS	(cfg_line_stopbits_eol, line_command, LINE_STOPBITS);
KEYWORD_ID (cfg_line_stopbits_2, cfg_line_stopbits_eol, no_alt,
	 OBJ(int,1), STOPBITS2,
	 "2", "Two stop bits", PRIV_USER);
KEYWORD_ID (cfg_line_stopbits_15, cfg_line_stopbits_eol, cfg_line_stopbits_2,
	 OBJ(int,1), STOPBITS15,
	 "1.5", "One and one-half stop bits", PRIV_USER);
/* Don't check '1.5' if '1' has already matched. */
TEST_MULTIPLE_FUNCS(cfg_line_stopbits_test, cfg_line_stopbits_15,
		    cfg_line_stopbits_2, NONE);
KEYWORD_ID (cfg_line_stopbits_1, cfg_line_stopbits_eol, cfg_line_stopbits_test,
	    OBJ(int,1), STOPBITS1,
	 "1", "One stop bit", PRIV_USER);
NOPREFIX (cfg_line_stopbits_noprefix, cfg_line_stopbits_1,
	  cfg_line_stopbits_eol);
NVGENS	(cfg_line_stopbits_nvgen, cfg_line_stopbits_noprefix,
	 line_command, LINE_STOPBITS);
KEYWORD	(cfg_line_stopbits, cfg_line_stopbits_nvgen, ALTERNATE, 
	 "stopbits", "Set async line stop bits", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	cfg_line_stopbits
