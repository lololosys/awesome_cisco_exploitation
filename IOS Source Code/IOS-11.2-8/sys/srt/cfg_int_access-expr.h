/* $Id: cfg_int_access-expr.h,v 3.2.62.1 1996/08/12 16:07:44 widmer Exp $
 * $Source: /release/112/cvs/Xsys/srt/cfg_int_access-expr.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ A C C E S S - E X P R . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_access-expr.h,v $
 * Revision 3.2.62.1  1996/08/12  16:07:44  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:53:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:57:55  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:03:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*****************************************************************
 * access-expression { input | output } <boolean-expression>
 * no access-expression { input | output } <boolean-expression>
 *
 * OBJ(int,1) = TRUE if input, FALSE if output
 * OBJ(string,1) = <boolean-expression>
 */

EOLNS	(access_exp_eol, access_expression_command);

TEXT	(access_exp_expr, access_exp_eol, no_alt,
	OBJ(string,1), "A boolean expression of bridge access lists");

NOPREFIX (access_exp_noprefix, access_exp_expr, access_exp_eol);
NVGENNS	(access_exp_nvgen, access_exp_noprefix, access_expression_command);

KEYWORD_ID (access_exp_out, access_exp_nvgen, no_alt,
	OBJ(int,1), FALSE,
	"output", "Filter output packets", PRIV_CONF);

KEYWORD_ID (access_exp_in, access_exp_nvgen, access_exp_out,
	OBJ(int,1), TRUE,
	"input", "Filter input packets", PRIV_CONF);

/*****************************************************************/
KEYWORD	(access_expression, access_exp_in, NONE, 
	"access-expression", "Build a bridge boolean access expression",
	 PRIV_CONF);

TEST_BOOLEAN(access_expression_test, access_expression, NONE,
	     ALTERNATE, bridge_enable);

#undef	ALTERNATE
#define	ALTERNATE	access_expression_test
