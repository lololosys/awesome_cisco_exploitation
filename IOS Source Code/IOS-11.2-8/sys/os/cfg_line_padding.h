/* $Id: cfg_line_padding.h,v 3.2 1995/11/17 18:45:19 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/cfg_line_padding.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ P A D D I N G . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_padding.h,v $
 * Revision 3.2  1995/11/17  18:45:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:50:59  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:10:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * padding { <decimal-number> | <char> } <count>
 * no padding { <decimal-number> | <char> } [<count>]
 * no padding
 *
 * OBJ(int,1) = <decimal-number>
 * OBJ(string,1) = <char> (only first char is examined)
 * OBJ(int,2) = <count>
 * OBJ(int,3) =	CHAR_NUM = <decimal-number> was parsed
 *		CHAR_CHAR = <char> was parsed
 *		CHAR_NONE = no <char> or <decimal-number>
 */
EOLS	(cl_padding_eol, pad_command, TERM_PADDING);

/* <count> */
NUMBER	(cl_padding_count, cl_padding_eol, no_alt,
	 OBJ(int,2), 0, 255, "The number of NULLS to output");

/* Handle 'no padding' */
SET	(cl_padding_nonum_set, cl_padding_eol, OBJ(int,3), CHAR_NONE);
TESTVAR	(cl_padding_nonum, cl_padding_nonum_set, no_alt,
	 NONE, NONE, NONE,
	 sense, FALSE);

/* Handle 'no padding <decimal-number>' */
NOPREFIX (cl_padding_noprefix, cl_padding_count, cl_padding_eol);

/* <decimal-number> | <char> */
SET	(cl_padding_num_set, cl_padding_noprefix, OBJ(int,3), CHAR_NUM);
CHAR_NUMBER(cl_padding_num, cl_padding_noprefix, cl_padding_nonum,
	    OBJ(int,1), "Character to be padded");

NVGENS	(cl_padding_nvgen, cl_padding_num, pad_command, TERM_PADDING);
KEYWORD	(cl_padding, cl_padding_nvgen, ALTERNATE,
	 "padding", "Set padding for a specified output character", PRIV_USER);

#undef  ALTERNATE
#define ALTERNATE	cl_padding
