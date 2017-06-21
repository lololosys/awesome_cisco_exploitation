/* $Id: parser_defs_lex.h,v 3.1 1996/02/01 04:58:54 hampton Exp $
 * $Source: /release/111/cvs/Xsys/lex/parser_defs_lex.h,v $
 *------------------------------------------------------------------
 * PARSER_DEFS_LEX.H
 *
 * Copyright (c) 1994,1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: parser_defs_lex.h,v $
 * Revision 3.1  1996/02/01  04:58:54  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:51:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:30:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __PARSER_DEFS_LEX_H__
#define __PARSER_DEFS_LEX_H__

#define LEX_PRIVATE		('L' << 24 | 'E' << 16 | 'X' << 8)
#define	LEX_ENABLE		(LEX_PRIVATE | 0x01)
#define LEX_INTYPE		(LEX_PRIVATE | 0x02)
#define LEX_INADDRESS		(LEX_PRIVATE | 0x03)
#define LEX_PRIORITY		(LEX_PRIVATE | 0x04)
#define LEX_BIND		(LEX_PRIVATE | 0x05)
#define LEX_RETRY		(LEX_PRIVATE | 0x06)
#define LEX_TIMEOUT		(LEX_PRIVATE | 0x07)

/*
 * Definitions for the "lex retry-count <count>" config command.
 */
#define LEX_MIN_RETRY_COUNT        0
#define LEX_MAX_RETRY_COUNT      100

/*
 * Definitions for the "lex timeout <milliseconds>" config command.
 */
#define LEX_MIN_TIMEOUT           500  /* 1/2 second timeout is minimum */
#define LEX_MAX_TIMEOUT         60000  /* 60 second timeout is maximum  */

/*
 * Parser defines for Lex copy commands
 */
#define COPY_TFTP_LEX           0  /* "copy tftp lex <unit>" */
#define COPY_FLASH_LEX          1  /* "copy flash lex <unit>" */

/*
 * Parser defines for "clear controller lex <unit> [prom]" command.
 */
#define LEX_RELOAD_DEFAULT  0      /* use "normal" reload algorithm (flash) */
#define LEX_RELOAD_PROM     1      /* reload prom image  */

#endif
