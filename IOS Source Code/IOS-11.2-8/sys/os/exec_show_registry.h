/* $Id: exec_show_registry.h,v 3.3.20.1 1996/08/12 16:05:16 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/exec_show_registry.h,v $
 *------------------------------------------------------------------
 * exec_show_registry.h
 *
 * 12-March-1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_registry.h,v $
 * Revision 3.3.20.1  1996/08/12  16:05:16  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/25  11:20:20  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.2  1995/11/17  18:47:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:12  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:25:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * show registry  [<registry> [<service>]] [brief]
 */
EOLNS	(show_registry_eol, registry_show);

KEYWORD_ID (show_registry_brief, show_registry_eol, NONE,
	    OBJ(int,1), TRUE,
	    "brief", "List only a brief service description",
	    PRIV_USER);

NOP	(show_registry_service_nop, show_registry_brief, show_registry_eol);
STRING	(show_registry_service_string, show_registry_service_nop,
	 show_registry_eol,
	 OBJ(string,2), "Name or Number of service to examine");

TEST_MULTIPLE_FUNCS(show_registry_service, show_registry_service_string,
                    NONE, NONE);

NOP (show_registry_nop2, show_registry_brief, show_registry_service);

STRING  (show_registry_regname_string, show_registry_nop2, show_registry_eol,
	 OBJ(string,1), "Name or Number of registry to examine");

TEST_MULTIPLE_FUNCS(show_registry_regname, show_registry_regname_string,
                    NONE, NONE);

NOP (show_registry_nop1, show_registry_brief, show_registry_regname);

/*
 * show registry statistics
 */
EOLNS	(show_registry_statistics_eol, registry_show_statistics);

KEYWORD (show_registry_statistics, show_registry_statistics_eol,
	 show_registry_nop1,
	 "statistics", "Function registry statistics",
	 PRIV_USER);

KEYWORD (registry_show, show_registry_statistics, NONE,
	 "registry", "Function registry information",
	 PRIV_USER|PRIV_USER_HIDDEN);

TEST_BOOLEAN(show_registry_test, registry_show, NONE, ALTERNATE,
	     systeminit_flag);

#undef	ALTERNATE
#define	ALTERNATE	show_registry_test
