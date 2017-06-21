/* $Id: exec_pad.h,v 3.3.46.2 1996/08/12 16:09:45 widmer Exp $
 * $Source: /release/112/cvs/Xsys/x25/exec_pad.h,v $
 *------------------------------------------------------------------
 * E X E C _ P A D . H
 * 
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_pad.h,v $
 * Revision 3.3.46.2  1996/08/12  16:09:45  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.46.1  1996/07/12  18:17:34  swenson
 * CSCdi60860:  Busy-message not working for pad calls
 * provide a switch to make pad calls quiet
 * Branch: California_branch
 *
 * Revision 3.3  1995/12/15  22:28:26  widmer
 * CSCdi45685:  Need priv flag for interactive commands
 * Add Priv_Interactive to flag interactive commands
 *
 * Revision 3.2  1995/11/17  18:06:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:09  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:21:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * pad  { <x121addr> | <hostname> } [/debug] [/quiet] [/reverse] 
 *      [/cud <text>] [/profile <string>]
 *
 * x25 <x121addr>	= OBJ(paddr,1)
 * x25 <hostname>	= OBJ(string,1)
 * cud <text>		= OBJ(string,2)
 * profile <string>	= OBJ(string,3)
 * quiet		= OBJ(int,5)
 * reverse		= OBJ(int,3)
 * debug		= OBJ(int,1)
 */
EOLNS	(exec_pad_eol, pad_connect_command);

pdecl(exec_pad_options);
#define	PAD_DEBUG_SET	1<<0
#define	PAD_CUD_SET	1<<1
#define	PAD_PROFILE_SET	1<<2
#define	PAD_REVERSE_SET	1<<3
#define	PAD_QUIET_SET	1<<4


/*  quiet  */
SET	(exec_pad_quiet_set, exec_pad_options, OBJ(int,5), TRUE);
KEYWORD_ORTEST(exec_pad_quiet, exec_pad_quiet_set, exec_pad_eol,
	       OBJ(int,4), PAD_QUIET_SET ,
	       "/quiet", "Suppress informational messages", PRIV_USER);

/*  reverse  */
SET	(exec_pad_reverse_set, exec_pad_options, OBJ(int,3), TRUE);
KEYWORD_ORTEST(exec_pad_reverse, exec_pad_reverse_set, exec_pad_quiet,
	       OBJ(int,4), PAD_REVERSE_SET,
	       "/reverse", "X25 Address reverse", PRIV_USER);

STRING  (exec_pad_profile_string, exec_pad_options, no_alt,
         OBJ(string,3), "Profile name");
KEYWORD_ORTEST(exec_pad_profile, exec_pad_profile_string, exec_pad_reverse,
	       OBJ(int,4), PAD_PROFILE_SET,
	       "/profile", "Use a defined X.3 profile", PRIV_USER);

/*  cud  */
SET	(exec_pad_cud_set, exec_pad_options, OBJ(int,2), TRUE);
STRING	(exec_pad_cud_string, exec_pad_cud_set, no_alt,
	 OBJ(string,2), "Call user data");
KEYWORD_ORTEST(exec_pad_cud, exec_pad_cud_string, exec_pad_profile,
	       OBJ(int,4), PAD_CUD_SET,
	       "/cud", "Call user data", PRIV_USER);

/*  debug  */
SET	(exec_pad_debug_set, exec_pad_options, OBJ(int,1), TRUE);
KEYWORD_ORTEST(exec_pad_debug, exec_pad_debug_set, exec_pad_cud,
	   OBJ(int,4), PAD_DEBUG_SET,
	   "/debug", "Debugging option", PRIV_USER);

NOP	(exec_pad_options, exec_pad_debug, NONE);

/*  host  */
STRING	 (exec_pad_x121addr, exec_pad_options, no_alt,
	  OBJ(string,1), "X121 address or name of a remote system");

/*  pad  */
KEYWORD	(exec_pad_command, exec_pad_x121addr, NONE, 
	 "pad", "Open a X.29 PAD connection",
	 PRIV_USER | PRIV_INTERACTIVE);

TEST_BOOLEAN(exec_pad, exec_pad_command, NONE, ALTERNATE, pad_enable);


#undef	ALTERNATE
#define	ALTERNATE	exec_pad
