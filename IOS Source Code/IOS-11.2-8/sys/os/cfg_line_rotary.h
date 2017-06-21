/* $Id: cfg_line_rotary.h,v 3.2 1995/11/17 18:45:24 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/cfg_line_rotary.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ R O T A R Y . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_rotary.h,v $
 * Revision 3.2  1995/11/17  18:45:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:00  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:10:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * rotary <num>
 * no rotary
 */
PARAMS_KEYONLY(rotary, ALTERNATE, "rotary",
	       OBJ(int,1), 0, 100,
	       line_command, PARSER_LINE_ROTARY,
	       "Add line to a rotary group",
	       "Rotary group to add line to", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	rotary
