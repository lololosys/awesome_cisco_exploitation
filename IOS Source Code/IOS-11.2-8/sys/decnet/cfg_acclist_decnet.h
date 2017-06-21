/* $Id: cfg_acclist_decnet.h,v 3.4.52.2 1996/06/11 20:28:25 hampton Exp $
 * $Source: /release/112/cvs/Xsys/decnet/cfg_acclist_decnet.h,v $
 *------------------------------------------------------------------
 * DECnet access list commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_acclist_decnet.h,v $
 * Revision 3.4.52.2  1996/06/11  20:28:25  hampton
 * New access-list commands should set the NONVGEN flag. [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.4.52.1  1996/05/30  23:42:29  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.4  1995/11/28  19:50:25  widmer
 * CSCdi44251:  Replace Ifelse(csb->in_help)/Eval with Func
 *
 * Revision 3.3  1995/11/27  21:24:39  widmer
 * CSCdi43607:  Add keyword_option macro to parse multiple keywords
 * Also create permit_deny macro using keyword_option macro to parse
 * permit/deny keywords.
 * Change "ip security multilevel" command to use keyword_option macro.
 *
 * Revision 3.2  1995/11/17  08:58:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:15:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:44:22  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:00:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*****************************************************************
 * DECnet access list
 *
 * access-list <300-399> { permit | deny } <addr> <mask> [<addr2)> <mask2>]
 *    [{ EQ | NEQ }
 *   {
 *    account <acc-reg-exp> |
 *    any |
 *    dst {eq <dnum> | neq <dnum> | lt <dnum> | gt <dnum> |
 *		exp <dexp-reg-exp> | uic <uic>} |
 *    id <id-reg-exp> |
 *    password <pw-reg-exp> |
 *    src {eq <snum> | neq <snum> | lt <snum> | gt <snum> |
 *		exp <sexp-reg-exp> | uic <uic>}
 *   }
 *
 *
 * no access-list <300-399> [{ permit | deny } <addr> <mask> [<addr2)> <mask2>]
 *	[{ EQ | NEQ } <connect-items>]]
 *
 * OBJ(int,1) = access list number
 * OBJ(int,2) = TRUE for permit; FALSE for deny
 * OBJ(int,3) = Union of DNO_AC, DNO_ANY, DNO_DON, DNO_DOE, DNO_DUIC,
 *		DNO_ID, DNO_PW, DNO_SON, DNO_SOE, DNO_SUIC, DNO_EQ
 * OBJ(int,4) = <dnum>
 * OBJ(int,5) = <dgrp>
 * OBJ(int,6) = <dusr>
 * OBJ(int,7) = <sgrp>
 * OBJ(int,8) = <susr>
 * OBJ(string,1) = <acc-reg-exp>
 * OBJ(string,2) = <dexp-reg-exp>
 * OBJ(string,3) = <id-reg-exp>
 * OBJ(string,4) = <pw-reg-exp>
 * OBJ(string,5) = <sexp-reg-exp>
 * (*OBJ(paddr,1))= <addr>
 * (*OBJ(paddr,2))= <mask>
 * (*OBJ(paddr,3))= <OBJ(paddr,2)>  (OBJ(paddr,3).type == ADDR_DECNET if set)
 * (*OBJ(paddr,4))= <mask2>
 *
 * A UIC can be either decimal or octal, depending on the DEC OS in use
 * at the time.  We must use a function which can accept either, but
 * during NVGEN, we only generate octal.
 */

pdecl(aldec_account);
pdecl (aldec_src_eq);
pdecl (aldec_dst_eq);

EOLS	(aldec_eol, dnaccess_command, LIST_DECNET);

/* src ... */
HELP	(aldec_src_help, aldec_account, "Primary keywords:\n");
DNUIC	(aldec_src_uic_val, aldec_src_eq, no_alt, OBJ(int,7), OBJ(int,8));
KEYWORD_OR(aldec_src_uic, aldec_src_uic_val, aldec_src_help,
	   OBJ(int,3), DNO_SUIC,
	   "uic", "Filter on User Identification Code", PRIV_CONF);
STRING	(aldec_src_regexp, aldec_src_eq, no_alt,
	 OBJ(string,5), "Regular expression");
KEYWORD_OR(aldec_src_exp, aldec_src_regexp, aldec_src_uic,
	   OBJ(int,3), DNO_SOE,
	   "exp", "Filter on regular expression", PRIV_CONF);
NUMBER	(aldec_src_num, aldec_src_eq, no_alt,
	OBJ(int,7), 0, -1, "DECnet object number");
SET	(aldec_src_gt_set, aldec_src_num, OBJ(int,10), DNID_GT);
KEYWORD_OR(aldec_src_gt, aldec_src_gt_set, aldec_src_exp,
	   OBJ(int,3), DNO_SON,
	   DNSTR_GT, "Greater than", PRIV_CONF);
SET	(aldec_src_lt_set, aldec_src_num, OBJ(int,10), DNID_LT);
KEYWORD_OR(aldec_src_lt, aldec_src_lt_set, aldec_src_gt,
	   OBJ(int,3), DNO_SON,
	   DNSTR_LT, "Les than", PRIV_CONF);
SET	(aldec_src_neq_set, aldec_src_num, OBJ(int,10), DNID_NEQ);
KEYWORD_OR(aldec_src_neq, aldec_src_neq_set, aldec_src_lt,
	   OBJ(int,3), DNO_SON,
	   DNSTR_NEQ, "Not equal to", PRIV_CONF);
SET	(aldec_src_eq_set, aldec_src_num, OBJ(int,10), DNID_EQ);
KEYWORD_OR(aldec_src_eq, aldec_src_eq_set, aldec_src_neq,
	   OBJ(int,3), DNO_SON,
	   DNSTR_EQ, "Equal to", PRIV_CONF);
KEYWORD (aldec_src, aldec_src_eq, aldec_eol,
	"src", "Filter on source", PRIV_CONF);

/* password <pw-reg-exp> */
STRING	(aldec_pw_regexp, aldec_account, no_alt,
	 OBJ(string,4), "Password regular expression");
KEYWORD_OR(aldec_pw, aldec_pw_regexp, aldec_src,
	   OBJ(int,3), DNO_PW,
	   "password",
	   "Filter when access password matches the regular expression",
	   PRIV_CONF);

/* id <id-reg-exp> */
STRING	(aldec_id_regexp, aldec_account, no_alt,
	 OBJ(string,3), "Access id regular expression");
KEYWORD_OR(aldec_id, aldec_id_regexp, aldec_pw,
	   OBJ(int,3), DNO_ID,
	   "id",
	   "Filter when access id matches the regular expression",
	   PRIV_CONF);

/* dst ... */
HELP	(aldec_dst_help, aldec_account, "Primary keywords:");
DNUIC	(aldec_dst_uic_val, aldec_dst_eq, no_alt, OBJ(int,5), OBJ(int,6));
KEYWORD_OR(aldec_dst_uic, aldec_dst_uic_val, aldec_dst_help,
	   OBJ(int,3), DNO_DUIC,
	   "uic", "User Identification Code", PRIV_CONF);
STRING	(aldec_dst_regexp, aldec_dst_eq, no_alt,
	 OBJ(string,2), "Regular expression");
KEYWORD_OR(aldec_dst_exp, aldec_dst_regexp, aldec_dst_uic,
	   OBJ(int,3), DNO_DOE,
	   "exp", "Filter on a regular expression", PRIV_CONF);
NUMBER	(aldec_dst_num, aldec_dst_eq, no_alt,
	OBJ(int,4), 0, -1, "DECnet object number");
SET	(aldec_dst_gt_set, aldec_dst_num, OBJ(int,9), DNID_GT);
KEYWORD_OR(aldec_dst_gt, aldec_dst_gt_set, aldec_dst_exp,
	   OBJ(int,3), DNO_DON,
	   DNSTR_GT, "Greater than", PRIV_CONF);
SET	(aldec_dst_lt_set, aldec_dst_num, OBJ(int,9), DNID_LT);
KEYWORD_OR(aldec_dst_lt, aldec_dst_lt_set, aldec_dst_gt,
	   OBJ(int,3), DNO_DON,
	   DNSTR_LT, "Less than", PRIV_CONF);
SET	(aldec_dst_neq_set, aldec_dst_num, OBJ(int,9), DNID_NEQ);
KEYWORD_OR(aldec_dst_neq, aldec_dst_neq_set, aldec_dst_lt,
	   OBJ(int,3), DNO_DON,
	   DNSTR_NEQ, "Not equal to", PRIV_CONF);
SET	(aldec_dst_eq_set, aldec_dst_num, OBJ(int,9), DNID_EQ);
KEYWORD_OR(aldec_dst_eq, aldec_dst_eq_set, aldec_dst_neq,
	   OBJ(int,3), DNO_DON,
	   DNSTR_EQ, "Equal to", PRIV_CONF);
KEYWORD (aldec_dst, aldec_dst_eq, aldec_id,
	"dst", "Filter on destination", PRIV_CONF);

/* any */
KEYWORD_OR(aldec_any, aldec_account, aldec_dst,
	   OBJ(int,3), DNO_ANY,
	   "any", "Match any connect packet", PRIV_CONF);

/* account <reg-exp> */
STRING	(aldec_account_exp, aldec_account, no_alt,
	 OBJ(string,1), "Account regular expression");
KEYWORD_OR(aldec_account, aldec_account_exp, aldec_any,
	   OBJ(int,3), DNO_AC,
	   "account",
	   "Filter when the access account matches the regular expression",
	   PRIV_CONF);

/* {EQ | NEQ} */
EVAL	(aldec_neq_set, aldec_any, GETOBJ(int,3) &= ~DNO_EQ);
KEYWORD (aldec_neq, aldec_neq_set, aldec_eol,
	     "neq", "Non-matching connect items", PRIV_CONF);
KEYWORD_OR(aldec_eq, aldec_any, aldec_neq,
	   OBJ(int,3), DNO_EQ,
	   "eq", "Matching connect items", PRIV_CONF);

DNADDR	(aldec_2mask, aldec_eq, no_alt,
	 OBJ(paddr,4), "Destination mask");
DNADDR	(aldec_2addr, aldec_2mask, aldec_eq,
	 OBJ(paddr,3), "Decnet destination address to match");

DNADDR	(aldec_1mask, aldec_2addr, no_alt,
	 OBJ(paddr,2), "Source mask");
DNADDR	(aldec_1addr, aldec_1mask, no_alt,
	 OBJ(paddr,1), "Decnet source address to match");

PERMIT_DENY(aldec_permit_deny, aldec_1addr, no_alt,
	    OBJ(int,2), PRIV_CONF);

NOPREFIX(aldec_noprefix, aldec_permit_deny, aldec_eol);

EVAL	(aldec_modify, aldec_noprefix,
	 GETOBJ(int,1) = GETOBJ(int,1) - DN_MIN_ACL + MINDECACC);
NUMBER	(aldec_num, aldec_modify, NONE, OBJ(int,1),
	 DN_MIN_ACL, DN_MAX_ACL, "DECnet access list");
NVGENS	(alist_dec_nvgen, aldec_num, dnaccess_command, LIST_DECNET);
KEYWORD (access_list_dec, alist_dec_nvgen, ALTERNATE,
	 "access-list", "Decnet Access lists",
	 PRIV_CONF|PRIV_HIDDEN|PRIV_NONVGEN);

#undef	ALTERNATE
#define	ALTERNATE	access_list_dec

/*
 * Old style "[no] access-list ..."
 *
 * These remaining lines should be deleted in a future release.
 */
NUMBER	(oaldec_num, aldec_noprefix, NONE,
	 OBJ(int,1), MINDECACC, MAXDECACC, "DECnet access list");
NVGENS	(oalist_dec_nvgen, oaldec_num, dnaccess_command, LIST_DECNET);
NOP	(oaccess_list_dec, oalist_dec_nvgen, OALTERNATE);
#undef	OALTERNATE
#define	OALTERNATE	oaccess_list_dec

