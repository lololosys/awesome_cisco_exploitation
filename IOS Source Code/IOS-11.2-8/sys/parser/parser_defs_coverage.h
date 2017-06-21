/*
 * $Id: parser_defs_coverage.h,v 3.2 1995/11/17 18:49:59 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/parser_defs_coverage.h,v $
 *------------------------------------------------------------------
 * parser_defs_coverage.h - Parser definitions for coverage commands
 * 
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_coverage.h,v $
 * Revision 3.2  1995/11/17  18:49:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:29:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __PARSER_DEFS_COVERAGE_H__
#define __PARSER_DEFS_COVERAGE_H__

#define COVERAGE_MAX_HISTORY 100000

/* coverage configuration commands */

enum COVERAGE_CFG {
    COVERAGE_HISTORY
};

/* coverage show commands */

enum COVERAGE_SHOW {
    COVERAGE_ALL,
    COVERAGE_FIRST,
    COVERAGE_LAST,
    COVERAGE_STATUS
  };

#endif /* __PARSER_DEFS_COVERAGE_H__ */
