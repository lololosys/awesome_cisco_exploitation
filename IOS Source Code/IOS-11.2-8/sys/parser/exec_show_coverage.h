/* $Id: exec_show_coverage.h,v 3.2 1995/11/17 18:47:14 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/exec_show_coverage.h,v $
 *------------------------------------------------------------------
 * exec_show_coverage.h - Definitions for coverage analysis "show" commands
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_coverage.h,v $
 * Revision 3.2  1995/11/17  18:47:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:23:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "parser_defs_coverage.h"

/******************************************************************
 * show coverage command
 *
 */

/*
 * show coverage history {status}
 */

EOLS (show_coverage_history_status_eol, coverage_show_command,
      COVERAGE_STATUS);
KEYWORD (show_coverage_history_status, show_coverage_history_status_eol,
	 show_coverage_history_status_eol,
	 "status", "Status of history system", PRIV_CONF);

/* show coverage history last */
/* OBJ(int, 1) = count */

EOLS (show_coverage_history_last_eol, coverage_show_command, COVERAGE_LAST);
NUMBER (show_coverage_history_last_n, show_coverage_history_last_eol,
	no_alt, OBJ(int, 1), 1,
	COVERAGE_MAX_HISTORY, "Number of entries to show");

KEYWORD (show_coverage_history_last, show_coverage_history_last_n,
	 show_coverage_history_status,
	 "last", "Show last (most recent) entries in history table",
	 PRIV_CONF);

/* show coverage history first */
/* OBJ(int, 1) = count */

EOLS (show_coverage_history_first_eol, coverage_show_command, COVERAGE_FIRST);
NUMBER (show_coverage_history_first_n, show_coverage_history_first_eol,
	no_alt, OBJ(int, 1), 1,
	COVERAGE_MAX_HISTORY, "Number of entries to show");

KEYWORD (show_coverage_history_first, show_coverage_history_first_n,
	 show_coverage_history_last,
	 "first", "Show first (least recent) entries in history table",
	 PRIV_CONF);

/* show coverage history all */

EOLS (show_coverage_history_all_eol, coverage_show_command, COVERAGE_ALL);
KEYWORD (show_coverage_history_all, show_coverage_history_all_eol,
	 show_coverage_history_first,
	 "all", "Show entire history table", PRIV_CONF);

/* show coverage history */

KEYWORD (show_coverage_history, show_coverage_history_all, no_alt,
	 "history", "Function call history", PRIV_CONF);

/* show coverage */

KEYWORD	(show_coverage, show_coverage_history, ALTERNATE, "coverage",
	 "Coverage analysis tools",
	 PRIV_CONF|PRIV_INTERNAL);

#undef	ALTERNATE
#define	ALTERNATE	show_coverage
