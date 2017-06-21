/* $Id: cfg_int_stun.h,v 3.2 1995/11/17 09:21:57 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ibm/cfg_int_stun.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ S T U N . H
 * 
 * STUN interface subcommands.
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_stun.h,v $
 * Revision 3.2  1995/11/17  09:21:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:49  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:07:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/************************************************************************
 * no stun
 */
EOLS	(ci_stun_eol, stunif_command, NO_STUN );
TESTVAR	(ci_stun_noprefix, ci_stun_eol, NONE,
	 NONE, NONE, no_alt, sense, FALSE );

/************************************************************************
 * stun sdlc-role {primary|secondary}
 */
EOLS	(ci_stun_sdlcrole_eol, stunif_command, STUNIF_SDLCROLE );
KEYWORD_ID(ci_stun_sdlcrole_secondary, ci_stun_sdlcrole_eol, no_alt,
	   OBJ(int,1), FALSE,
	   "secondary", "Secondary station", PRIV_CONF );
KEYWORD_ID(ci_stun_sdlcrole_primary, ci_stun_sdlcrole_eol,
	   ci_stun_sdlcrole_secondary,
	   OBJ(int,1), TRUE,
	   "primary", "Primary station", PRIV_CONF );
NOPREFIX(ci_stun_sdlcrole_noprefix, ci_stun_sdlcrole_primary,
	 ci_stun_sdlcrole_eol);
NVGENS	(ci_stun_sdlcrole_nvgen, ci_stun_sdlcrole_noprefix,
	 stunif_command, STUNIF_SDLCROLE);
KEYWORD	(ci_stun_sdlcrole, ci_stun_sdlcrole_nvgen, ci_stun_noprefix,
	 "sdlc-role", "SDLC device behavior", PRIV_CONF );

/************************************************************************
 * [no] stun group <group-number>
 */
PARAMS (ci_stun_grp, ci_stun_sdlcrole, "group",
	     OBJ(int,1), STUN_MIN_GROUP, STUN_MAX_GROUP,
	     stunif_command, STUNIF_GROUP,
	     "Assign an interface to a STUN group",
	     "A STUN group number", PRIV_CONF);

KEYWORD	(ci_stun, ci_stun_grp, NONE, "stun",
	 "STUN interface subcommands", PRIV_CONF);

TEST_IDBSTATUS(ci_stun_test, ci_stun, NONE, ALTERNATE, IDB_SERIAL);

#undef  ALTERNATE
#define ALTERNATE ci_stun_test
