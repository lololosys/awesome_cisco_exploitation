/* $Id: cfg_line_refuse-message.h,v 3.2 1995/11/17 18:45:23 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/cfg_line_refuse-message.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ R E F U S E - M E S S A G E . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_refuse-message.h,v $
 * Revision 3.2  1995/11/17  18:45:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:00  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:10:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * refuse-message <c refuse-text c>
 * no refuse-message
 *
 * OBJ(string,1) = refuse-text
 */
EOLS	(cl_refuse_eol, tty_busymsg, LINE_REFUSE);
TEXT	(cl_refuse_text, cl_refuse_eol, no_alt,
	 OBJ(string,1),
	 "c refuse-text c, where 'c' is a delimiting character");

NOPREFIX (cl_refuse_noprefix, cl_refuse_text, cl_refuse_eol);
NVGENS	(cl_refuse_nvgen, cl_refuse_noprefix, tty_busymsg, LINE_REFUSE);

KEYWORD	(cl_refuse, cl_refuse_nvgen, ALTERNATE,
	 "refuse-message", "Define a refuse banner", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cl_refuse
