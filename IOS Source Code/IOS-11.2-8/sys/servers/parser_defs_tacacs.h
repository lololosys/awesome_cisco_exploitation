/*
 *    P A R S E R _ D E F S _ T A C A S . H
 *
 * $Id: parser_defs_tacacs.h,v 3.1.40.1 1996/04/19 15:29:22 che Exp $
 * $Source: /release/112/cvs/Xsys/servers/parser_defs_tacacs.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_tacacs.h,v $
 * Revision 3.1.40.1  1996/04/19  15:29:22  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.1.58.1  1996/04/04  19:14:47  carrel
 * cvs fixes
 *
 * Revision 3.1.16.1  1996/02/28  06:14:58  lol
 * Branch: IosSec_branch
 * TACACS+ processing.
 *
 * Revision 3.1  1996/02/01  04:59:10  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1995/12/08  00:54:41  irfan
 * CSCdi45224:  tacacs could use a specified source address
 * allow tacacs (TA, XTA, TA+) to use a specific (up) interface's IP
 * address.
 *
 * Revision 3.2  1995/11/17  18:52:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:31:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* TACACS parameters */
#define TACACS_ASK               0
#define TACACS_ATTEMPTS          1
#define TACACS_EXTENDED          2
#define TACACS_HOST              3
#define TACACS_LAST              4
#define TACACS_NOTE              5
#define TACACS_RETRANSMIT        6
#define TACACS_TIMEOUTVALUE      7
#define TACACS_PASSWORDOPT       8
#define TACACS_SLIP              9
#define TACACS_LOGOUT            10
#define TACACS_ENABLE            11
#define TACACS_CONNECT           12
#define TACACS_DIRECTED_REQUEST  13
#define TACACS_KEY               14
#define TACACS_SOURCE_INTERFACE  15
#define TACACS_SINGLE_CONNECTION 16
#define TACACS_LOGIN_TIMEOUT	 17
#define TACACS_ALIAS_LOOKUP      18
