/* $Id: cfg_x29.h,v 3.3 1995/11/20 21:00:29 eschaffe Exp $
 * $Source: /swtal/cherf.111/ios/sys/x25/cfg_x29.h,v $
 *------------------------------------------------------------------
 * C F G _ X 2 9 . H
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_x29.h,v $
 * Revision 3.3  1995/11/20  21:00:29  eschaffe
 * CSCdi14369:  Cannot set a PAD profile for incoming calls to a PT
 *
 * Revision 3.2  1995/11/17  18:05:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:03  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:14:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************************
 * x29 profile <default | profile-name> <num>:<num> [<num>:<num>] ...
 * no x29 profile <default | profile-name>
 */
EOLS	(x29_profile_eol, x29_command, X29_PROFILE);
pdecl(x29_profile_x3_pairs);
EVAL	(x29_profile_x3_eval, x29_profile_x3_pairs,
	 (GETOBJ(string,2)[GETOBJ(int,10)++] = GETOBJ(int,4),
	  GETOBJ(string,2)[GETOBJ(int,10)++] = GETOBJ(int,5)));
X3PAIR  (x29_profile_x3_pair, x29_profile_x3_eval, NONE,
	 OBJ(int,4), OBJ(int,5), "X.3 PAD parameters and values");
TESTVAR	(x29_profile_x3_pairs, no_alt, NONE,
	 x29_profile_x3_pair, no_alt, x29_profile_eol, 
	 OBJ(int,10), PROFILE_NPARAMS*2);

X3PAIR	(x29_profile_first_x3_pair, x29_profile_x3_eval, no_alt,
	 OBJ(int,4), OBJ(int,5), "X.3 PAD parameters and values");

NOPREFIX(x29_profile_noprefix, x29_profile_first_x3_pair, x29_profile_eol);
STRING	(x29_profile_name, x29_profile_noprefix, no_alt,
	 OBJ(string,1), "Name of the profile");
TEST_MULTIPLE_FUNCS (x29_profile_test, x29_profile_name, NONE, no_alt);
KEYWORD_ID (x29_profile_name_default, x29_profile_noprefix, x29_profile_test,
	    OBJ(int,11), TRUE, "default", "Create a default profile", 
	    PRIV_CONF);
NVGENS	(x29_profile_nvgen, x29_profile_name_default, x29_command, 
	 X29_PROFILE);
KEYWORD	(x29_profile, x29_profile_nvgen, no_alt,
	 "profile", "Create an X.3 profile", PRIV_CONF);

/******************************************************************************
 * x29 inviteclear-time <num>
 * no x29 inviteclear-time
 */

EOLS	(x29_inviteclr_eol, x29_command, X29_INVITECLEAR_TIME);
KEYWORD_ID (x29_inviteclr_none, x29_inviteclr_eol, no_alt,
	    OBJ(int,1), FALSE,
	    "none", "do not send X.29 invite clear", PRIV_CONF|PRIV_HIDDEN);
NUMBER	(x29_inviteclr_time, x29_inviteclr_eol, x29_inviteclr_none,
	 OBJ(int,1), 5, MAXINT/ONESEC, "Time in seconds");
NVGENS	(x29_inviteclr_nvgen, x29_inviteclr_time, x29_command,
	 X29_INVITECLEAR_TIME);
KEYWORD	(x29_inviteclr, x29_inviteclr_nvgen, x29_profile,
	 "inviteclear-time", "Wait for response to X.29 Invite Clear message",
	 PRIV_CONF);

/******************************************************************************
 * x29 access-list <1-199> { permit | deny } <pattern>
 * no x29 access-list <1-199>
 *
 * note that there are 200 rergular expression access lists, since they
 * must cover the same range as both regular and complex IP accesslists.
 */
EOLS	(x29_access_eol, x29_command, X29_ACCESS_LIST);
STRING	(x29_access_pattern, x29_access_eol, no_alt,
	 OBJ(string,1),
	 "X.121 address with or without regular-expression pattern matching");
KEYWORD_ID(x29_access_deny, x29_access_pattern, no_alt,
	   OBJ(int,2), FALSE,
	   "deny",
	   "Deny matching source X.121 address access the terminal server",
	   PRIV_CONF);
KEYWORD_ID(x29_access_permit, x29_access_pattern, x29_access_deny,
	   OBJ(int,2), TRUE,
	   "permit",
	   "Allow matching source X.121 address to access the terminal server",
	   PRIV_CONF);
NOPREFIX(x29_access_noprefix, x29_access_permit, x29_access_eol);
NUMBER	(x29_access_num, x29_access_noprefix, no_alt,
	 OBJ(int,1), MINREGEXPACC+1, MAXREGEXPACC, "Access list number");
NVGENS	(x29_access_nvgen, x29_access_num, x29_command, X29_ACCESS_LIST);
KEYWORD	(x29_access, x29_access_nvgen, x29_inviteclr,
	 "access-list", "Define an X.29 access list", PRIV_CONF);

/******************************************************************************/
KEYWORD	(x29, x29_access, NONE,
	 "x29", "X29 commands", PRIV_CONF);

TEST_BOOLEAN(x29_test, x29, NONE, ALTERNATE, pad_enable);

#undef	ALTERNATE
#define	ALTERNATE	x29_test
