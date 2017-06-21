/*
 *	P A R S E R _ D E F S _ L O G G E R . H
 *
 * $Id: parser_defs_logger.h,v 3.2 1996/02/07 21:15:08 widmer Exp $
 * $Source: /release/111/cvs/Xsys/os/parser_defs_logger.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_logger.h,v $
 * Revision 3.2  1996/02/07  21:15:08  widmer
 * CSCdi48343:  Remove unused #ifdef STANDALONE from parser code
 *
 * Revision 3.1  1996/02/01  04:59:02  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:51:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:30:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Logging Levels */
enum {
    P_LOG_NONE = 0,
    P_LOG_TRAP,
    P_LOG_MONITOR,
    P_LOG_CONSOLE,
    P_LOG_HOST,
    P_LOG_BUFFERED,
    P_LOG_ON,
    P_LOG_LEVEL,
    P_LOG_FACILITY,
    P_LOG_SOURCE
};

/*
 *  Logging Facility commands
 */
#define P_LOG_FAC_KERN                                    0
#define P_LOG_FAC_USER                                    1
#define P_LOG_FAC_MAIL                                    2
#define P_LOG_FAC_DAEMON                                  3
#define P_LOG_FAC_AUTH                                    4
#define P_LOG_FAC_SYSLOG                                  5
#define P_LOG_FAC_LPR                                     6
#define P_LOG_FAC_NEWS                                    7
#define P_LOG_FAC_UUCP                                    8
#define P_LOG_FAC_SYS9                                    9
#define P_LOG_FAC_SYS10                                  10
#define P_LOG_FAC_SYS11                                  11
#define P_LOG_FAC_SYS12                                  12
#define P_LOG_FAC_SYS13                                  13
#define P_LOG_FAC_SYS14                                  14
#define P_LOG_FAC_CRON                                   15
#define P_LOG_FAC_LOCAL0                                 16
#define P_LOG_FAC_LOCAL1                                 17
#define P_LOG_FAC_LOCAL2                                 18
#define P_LOG_FAC_LOCAL3                                 19
#define P_LOG_FAC_LOCAL4                                 20
#define P_LOG_FAC_LOCAL5                                 21
#define P_LOG_FAC_LOCAL6                                 22
#define P_LOG_FAC_LOCAL7                                 23
