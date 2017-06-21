/* $Id: cfg_line_parity.h,v 3.2 1995/11/17 18:45:21 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/cfg_line_parity.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ P A R I T Y . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_parity.h,v $
 * Revision 3.2  1995/11/17  18:45:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:50:59  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:10:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * parity { none, odd, even, space, mark }
 *
 */
EOLS	(parity_eol, line_command, PARSER_LINE_PARITY);
KEYWORD_ID(parity_mark, parity_eol, no_alt, OBJ(int,1), PARITY_MARK,
	   "mark", "Mark parity", PRIV_USER);
KEYWORD_ID(parity_space, parity_eol, parity_mark, OBJ(int,1), PARITY_SPACE,
	   "space", "Space parity", PRIV_USER);
KEYWORD_ID(parity_even, parity_eol, parity_space, OBJ(int,1), PARITY_EVEN,
	   "even", "Even parity", PRIV_USER);
KEYWORD_ID(parity_odd, parity_eol, parity_even, OBJ(int,1), PARITY_ODD,
	   "odd", "Odd parity", PRIV_USER);
KEYWORD_ID(parity_none, parity_eol, parity_odd, OBJ(int,1), PARITY_NO,
	   "none", "No parity", PRIV_USER);

SET	(parity_no_set, parity_eol, OBJ(int,1), PARITY_NO);
TESTVAR	(parity_no_test, parity_none, parity_no_set,
	 NONE, NONE, NONE, sense, TRUE);

KEYWORD	(parity, parity_no_test, ALTERNATE,
	 "parity", "Set terminal parity", PRIV_USER);


#undef	ALTERNATE
#define	ALTERNATE	parity
