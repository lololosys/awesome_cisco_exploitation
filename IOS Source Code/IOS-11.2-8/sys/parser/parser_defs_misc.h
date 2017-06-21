/*
 *	P A R S E R _ D E F S _ M I S C . H
 * $Id: parser_defs_misc.h,v 3.3 1995/11/30 00:02:54 hampton Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/parser/parser_defs_misc.h,v $
 * 
 * Copyright (c) 1985-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_misc.h,v $
 * Revision 3.3  1995/11/30  00:02:54  hampton
 * Print a stack traceback before attempting to dump core.  [CSCdi44670]
 *
 * Revision 3.2  1995/11/17  18:51:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:30:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Defines for commands returning char or decimal-number */
#define CHAR_NONE               0
#define CHAR_CHAR               1
#define CHAR_NUM                2

/*
 *  Transport commands
 */
#define TELNET_TRANSPORT_PREFERRED      1
#define TELNET_TRANSPORT_IN             2
#define TELNET_TRANSPORT_OUT            3

/* TFTP server defines */
#define TFTP_SYSTEM              0
#define TFTP_ROM		 1
#define TFTP_FLASH		 2

/*
 * Exception handling
 */
enum {
    EXCEPTION_CORE,
    EXCEPTION_DUMP,
    EXCEPTION_SIMULATE,
    EXCEPTION_MEMMIN,
    EXCEPTION_MEMFRG,
    EXCEPTION_PROTO,
    EXCEPTION_SPURIOUS,
    EXCEPTION_CRASHDUMP,
    EXCEPTION_REGIONSIZE
};

/*
 * For logging synchronous line subcommand
 */
#define LOGSYNC_SEVER_START     0
#define LOGSYNC_SEVER_END       7
