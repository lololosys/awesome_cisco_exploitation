/* $Id: pgen_hex_mode.h,v 3.2 1995/11/17 18:56:34 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/pgen/pgen_hex_mode.h,v $
 *------------------------------------------------------------------
 * P G E N _ H E X _ M O D E . H
 *
 * Parse chains for the hex input mode of the packet generator. In
 * hex mode, we read in successive lines of text up to a "quit" command,
 * and call a routine that accumulates the data into a buffer for later
 * use.
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pgen_hex_mode.h,v $
 * Revision 3.2  1995/11/17  18:56:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:59:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:34:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

/* There are two "commands": lines of data and the "quit" command. */
EOLS (pgen_hex_quit_eol, pgen_hexmode, PGEN_HEX_QUIT);
KEYWORD (pgen_hex_quit, pgen_hex_quit_eol, ALTERNATE,
	 "quit", "Exit data entry mode", PRIV_USER);

/* Hex data are returned in string 1, with a length in int 1 */
EOLS (pgen_hex_data_eol, pgen_hexmode, PGEN_HEX_DATA);
HEXDATA (pgen_hex_data, pgen_hex_data_eol, pgen_hex_quit,
	 OBJ(string,1), OBJ(int,1), "Hex data to append to parse buffer",
	 PARSEBUF);

#undef ALTERNATE
#define ALTERNATE pgen_hex_data

