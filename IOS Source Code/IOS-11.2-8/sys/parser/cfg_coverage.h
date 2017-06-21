/* $Id: cfg_coverage.h,v 3.2 1995/11/17 18:42:39 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/cfg_coverage.h,v $
 *------------------------------------------------------------------
 * cfg_coverage.h - Definitions for coverage analysis
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_coverage.h,v $
 * Revision 3.2  1995/11/17  18:42:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:49:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:02:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "parser_defs_coverage.h"

/******************************************************************
 * [no] coverage history <count>
 *
 * OBJ(int,1) = trace size
 */

EOLS    (cfg_coverage_history_eol, coverage_config_command, COVERAGE_HISTORY);

TESTVAR (cfg_coverage_notest, cfg_coverage_history_eol, no_alt,
	 NONE, NONE, NONE, sense, FALSE);

NUMBER (cfg_coverage_history_count, cfg_coverage_history_eol,
	 cfg_coverage_notest, OBJ(int,1), 1,
	 COVERAGE_MAX_HISTORY, "Size of history ring");

NVGENS  (cfg_coverage_history_coverage_nvgen,
	 cfg_coverage_history_count,
	 coverage_config_command, COVERAGE_HISTORY);

KEYWORD (cfg_coverage_history, cfg_coverage_history_coverage_nvgen, no_alt,
	 "history", "Record PC history",
	 PRIV_CONF);

KEYWORD	(cfg_coverage, cfg_coverage_history, ALTERNATE, "coverage",
	 "Coverage analysis tools",
	 PRIV_CONF|PRIV_INTERNAL);

#undef	ALTERNATE
#define	ALTERNATE	cfg_coverage
