/* $Id: cfg_translate.h,v 3.4.4.1 1996/08/12 16:06:56 widmer Exp $
 * $Source: /release/112/cvs/Xsys/pt/cfg_translate.h,v $
 *------------------------------------------------------------------
 * C F G _ T R A N S L A T E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_translate.h,v $
 * Revision 3.4.4.1  1996/08/12  16:06:56  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4  1996/03/11  22:01:00  swenson
 * CSCdi51039:  parsing of translate authentication options adds access
 * class
 * change which elements of uiarray[] are used for global translate
 * options to avoid conflicts introduced by patches for CSCdi35908.
 *
 * Revision 3.3  1995/12/20  18:10:24  eschaffe
 * CSCdi44853:  crash in fast_check when access class used on tcp
 * translation
 *
 * Revision 3.2  1995/11/17  18:58:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:00:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:56:24  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:14:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * translate <ipaddr> <x121addr>
 * translate <x121addr> <ipaddr>
 *
 * uiarray[0,1] are used for authentication options
 * uiarray[2] is unused 
 * uiarray[3] is unused
 * uiarray[4] is used for the swap global option
 * uiarray[5] is used for the max-users global option
 * uiarray[6] is used for the quiet global option
 * uiarray[7] is used for the login global option
 * uiarray[8] is used for the local global option
 * uiarray[9] is used for the access-class global option
 */
EOLNS	(translate_eol, translate_command);

pdecl(trans_extra_opts);
KEYWORD_ID(trans_swap, trans_extra_opts, NONE,
	   uiarray[4], TRUE,
	   "swap",
	   "Allow X.3 parameters to be set on the protocol"
	   " translator by the host originating the X.25 call",
	   PRIV_CONF);
ASSERT  (trans_swap_opt, trans_swap, NONE, 
	 ((GETOBJ(int,1) == TRANS_PROT_X25) && 
	 (GETOBJ(int,6) == TRANS_PROT_TCP))
	 || ((GETOBJ(int,1) == TRANS_PROT_TCP) && 
	 (GETOBJ(int,6) == TRANS_PROT_X25)));
NUMBER	(trans_max_num, trans_extra_opts, no_alt,
	 uiarray[5], 0, MAXUINT,
	 "Max number of simultaneous users of the translation");
KEYWORD	(trans_max, trans_max_num, trans_swap_opt,
	 "max-users",
	 "Limit the number of simultaneous users of the translation",
	 PRIV_CONF);
KEYWORD_ID(trans_quiet, trans_extra_opts, trans_max,
	   uiarray[6], TRUE,
	   "quiet", "Suppress printing of user-information messages",
	   PRIV_CONF);
KEYWORD_ID(trans_login, trans_extra_opts, trans_quiet,
	   uiarray[7], TRUE,
	   "login",
	 "Require that the user log in before the outgoing connection is made",
	   PRIV_CONF);
KEYWORD_ID(trans_local, trans_extra_opts, trans_login,
	   uiarray[8], TRUE,
	   "local",
	   "Allow Telnet protocol negotiations not to be translated",
	   PRIV_CONF);
NUMBER	(trans_access_num, trans_extra_opts, no_alt,
	 uiarray[9], MINFASTACC, MAXFASTACC, "Access list number");
KEYWORD	(trans_access, trans_access_num, trans_local,
	 "access-class",
	 "Allow access list parameters to be used by source hosts",
	 PRIV_CONF);

NOP	(trans_extra_opts, trans_access, translate_eol);
LINK_TRANS(trans_out_return, trans_extra_opts);

/***************************************************************
 *  Outgoing protocol information
 */

LINK_TRANS(trans_out_extend_here, no_alt);
TESTVAR	(trans_outgoing_notest, translate_eol, NONE, NONE, NONE, NONE,
	 sense, FALSE);
NOP     (trans_outgoing, trans_out_extend_here, trans_outgoing_notest);
LINK_TRANS(trans_in_return, trans_outgoing);

/***************************************************************
 *  Incoming protocol information
 */

/*
 * Variables use by incoming
 *  OBJ(paddr,1)
 *  OBJ(int,1), OBJ(int,2), OBJ(int,3), OBJ(int,4)
 *  OBJ(string,1), OBJ(string,2), OBJ(string,3)
 */

LINK_TRANS(trans_in_extend_here, no_alt);

NVGENNS	(trans_nvgen, trans_in_extend_here, translate_command);
KEYWORD	(translate, trans_nvgen, NONE,
	 "translate", "Translate global configuration commands", PRIV_CONF);

TEST_BOOLEAN(translate_test, translate, NONE, ALTERNATE, protocolconversion);

#undef	ALTERNATE
#define	ALTERNATE	translate_test
