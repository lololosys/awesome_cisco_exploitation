/* $Id: cfg_line_datamask.h,v 3.2 1995/11/17 18:44:05 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/cfg_line_datamask.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ D A T A M A S K . H
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_datamask.h,v $
 * Revision 3.2  1995/11/17  18:44:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:09:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************
 * data-character-bits <num>
 * no data-character-bits
 */

PARAMS_KEYONLY(cl_datamask, ALTERNATE, "data-character-bits",
	       OBJ(int,1), 7, 8,
	       line_command, PARSER_LINE_DATAMASK,
	       "Size of characters being handled", "Bits per character",
	       PRIV_USER );

#undef	ALTERNATE
#define	ALTERNATE	cl_datamask
