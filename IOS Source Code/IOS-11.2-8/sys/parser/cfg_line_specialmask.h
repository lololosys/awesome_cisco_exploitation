/* $Id: cfg_line_specialmask.h,v 3.2 1995/11/17 18:44:37 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/cfg_line_specialmask.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ S P E C I A L M A S K . H
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_specialmask.h,v $
 * Revision 3.2  1995/11/17  18:44:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:10:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************
 * special-character-bits <num>
 * no special-character-bits
 */

PARAMS_KEYONLY(cl_specialmask, ALTERNATE, "special-character-bits",
	       OBJ(int,1), 7, 8,
	       line_command, PARSER_LINE_SPECIALMASK,
	       "Size of the escape (and other special) characters",
	       "Bits per character", PRIV_USER );

#undef	ALTERNATE
#define	ALTERNATE	cl_specialmask
