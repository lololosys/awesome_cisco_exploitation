/* $Id: cfg_int_apollo.h,v 3.4 1995/12/14 13:08:10 widmer Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/xns/cfg_int_apollo.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ A P O L L O . H
 * 
 * Apollo interface subcommands.
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_apollo.h,v $
 * Revision 3.4  1995/12/14  13:08:10  widmer
 * CSCdi45510:  Add General_String macro to replace other string macros
 * Change Var_String, String, Txt_Line, and Text to use General_String
 * Add fail argument to Priv_Test
 * Change Neg1 to use Keyword_Id
 * Remove Sense macro
 *
 * Revision 3.3  1995/12/07  17:45:43  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.2  1995/11/17  19:20:56  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:37:58  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/02  01:42:57  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:03:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/************************************************************************
 * apollo update-time <seconds>
 * no apollo update-time <seconds>
 */
PARAMS_KEYONLY	(ci_apollo_updtimer, no_alt, 
	 "update-time", OBJ(int,1), 10, -1L, apolloif_command, APOLLOIF_UPDATETIME,
	 "Set Apollo routing update timer", "Routing update timer", PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * apollo network <number>
 * no apollo network [<number>]
 */
EOLS	(ci_apollo_neteol, apolloif_command, APOLLOIF_NETWORK);
HEXNUM	(ci_apollo_netnum, ci_apollo_neteol, no_alt, 
	 OBJ(int,1), "Network number");
NOPREFIX (ci_apollo_nonet, ci_apollo_netnum, ci_apollo_neteol);
NVGENS	(ci_apollo_netnvgen, ci_apollo_nonet, apolloif_command, 
	 APOLLOIF_NETWORK);
KEYWORD	(ci_apollo_network, ci_apollo_netnvgen, ci_apollo_updtimer,
	 "network", "Assign an Apollo network & enable Apollo routing", PRIV_CONF|PRIV_SUBIF);

/*************************************************************************
 * apollo access-group <name>
 * no apollo access-group <name>
 *
 * OBJ(string,1) = <name>
 */
EOLS	(ci_apollo_ageol, apolloif_command, APOLLOIF_ACC);
STRING	(ci_apollo_agname, ci_apollo_ageol, no_alt, OBJ(string,1),
	 "A valid Apollo access list name");
NOPREFIX (ci_apollo_agnoprefix, ci_apollo_agname, ci_apollo_ageol);
NVGENS	(ci_apollo_agnvgen, ci_apollo_agnoprefix,
	 apolloif_command, APOLLOIF_ACC);
KEYWORD	(ci_apollo_accessgrp, ci_apollo_agnvgen, ci_apollo_network,
	 "access-group", "Apply an access list to output packets",
	 PRIV_CONF|PRIV_SUBIF);

KEYWORD	(ci_apollo, ci_apollo_accessgrp, NONE, "apollo",
	 "Apollo interface subcommands", PRIV_CONF|PRIV_SUBIF);

PRIV_TEST(ci_apollo_unsupp, ci_apollo, NONE, NONE, PRIV_UNSUPPORTED);

TEST_IDBSTATUS(ci_apollo_unsupp_test, ci_apollo_unsupp, ci_apollo, NONE,
	       IDB_TUNNEL);

ASSERT  (ci_apollo_test, ci_apollo_unsupp_test, ALTERNATE,
	 main_if_or_vc(csb->interface));

#undef  ALTERNATE
#define ALTERNATE ci_apollo_test
