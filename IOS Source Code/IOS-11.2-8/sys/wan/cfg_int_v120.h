/* $Id: cfg_int_v120.h,v 3.2.62.1 1996/08/12 16:09:35 widmer Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_int_v120.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ V 1 2 0 . H
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_v120.h,v $
 * Revision 3.2.62.1  1996/08/12  16:09:35  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:01:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:35:04  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  23:30:20  wmay
 * Placeholder for CS_Arkansas_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
EOLNS (ci_isdn_answer_eol, v120_command);

KEYWORD	(ci_isdn_answer, ci_isdn_answer_eol, no_alt, "all-incoming-calls-v120",
	  "Answer all incoming calls as V.120", PRIV_CONF);

ASSERT (ci_isdn_answer_test, ci_isdn_answer, ALTERNATE,
	csb->interface->hwptr->type == IDBTYPE_BRI ||
	csb->interface->hwptr->type == IDBTYPE_T1 ||
	csb->interface->hwptr->type == IDBTYPE_E1);

#undef	ALTERNATE
#define	ALTERNATE	ci_isdn_answer_test
