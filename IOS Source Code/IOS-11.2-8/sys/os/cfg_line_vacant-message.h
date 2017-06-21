/* $Id: cfg_line_vacant-message.h,v 3.3 1996/02/01 23:34:12 widmer Exp $
 * $Source: /release/111/cvs/Xsys/os/cfg_line_vacant-message.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ V A C A N T - M E S S A G E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_vacant-message.h,v $
 * Revision 3.3  1996/02/01  23:34:12  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  18:45:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:03  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:10:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * vacant-message <c vacant-text c>
 * vacant-messsage
 * no vacant-message
 *
 * OBJ(int,1) =	TRUE = vacant-text
 *		FALSE = no vacant-text
 */
EOLS	(cl_vacant_eol, vacant_command, LINE_VACANT);
SET	(cl_vacant_text, cl_vacant_eol, OBJ(int,1), TRUE);
TEXT	(cl_vacant_char, cl_vacant_text, cl_vacant_eol,
	 OBJ(string,1),
	 "c vacant-text c, where 'c' is a delimiting character");

NOPREFIX (cl_vacant_noprefix, cl_vacant_char, cl_vacant_eol);
NVGENS	(cl_vacant_nvgen, cl_vacant_noprefix, vacant_command, LINE_VACANT);

KEYWORD	(cl_vacant, cl_vacant_nvgen, ALTERNATE,
	 "vacant-message", "Define a vacant banner", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cl_vacant
