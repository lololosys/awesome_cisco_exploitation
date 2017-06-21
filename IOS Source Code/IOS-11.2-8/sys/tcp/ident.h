/* $Id: ident.h,v 3.4 1996/01/31 23:34:38 tkolar Exp $
 * $Source: /release/111/cvs/Xsys/tcp/ident.h,v $
 *------------------------------------------------------------------
 * ident.h  --  Support for RFC1413, the Ident protocol
 *
 * September 1995, Tim Kolar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ident.h,v $
 * Revision 3.4  1996/01/31  23:34:38  tkolar
 * CSCdi47936:  Customers want additional Ident functionality
 * Add an option to prevent username polling attacks.
 *
 * Revision 3.3  1996/01/29  22:08:58  tkolar
 * CSCdi46928:  Identd does not fully match the RFC
 * Change OS to "IOS", remove whitespace, and fix bounds checking
 *
 * Revision 3.2  1995/11/17  18:47:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:35:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:32:02  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/10  17:46:28  tkolar
 * Add placeholder files to main branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *  Parser constants
 */

#define IP_IDENT		0
#define IP_IDENT_ALWAYSBE	1

/*
 *  Ident constants
 */
#define USER_IDENT_STR  ":USERID:IOS:"
#define ERROR_IDENT_STR ":ERROR:"
#define USER_IDENT_LEN  12
#define ERROR_IDENT_LEN 7


/*
 *  IDENT_MAXREPLY is both the size of the largest response we will send,
 *    and the largest request we will receive.  IDENT_MAXERROR is the largest
 *    error string we will send as part of a reply.  There is no guidance in 
 *    the RFC about these, and the BSD server doesn't do any bounds checking
 *    at all.    In short, I made these numbers up.
 */
#define IDENT_MAXREPLY  150
#define IDENT_MAXERROR  50

/*
 *  If our box has a TCP connection like the one being asked about, but there 
 *   is no username associated with the tty, return this string.
 */
#define IDENT_GUEST "<Guest>"

#define SPACE ' '
#define COMMA ','

/* Ident parsing state machine values */
#define IPARSE_STARTING         0
#define IPARSE_LOCALPORT        1
#define IPARSE_FIND_NEXTDIGIT   2
#define IPARSE_FOREIGNPORT      3
#define IPARSE_DONE             4

extern boolean tcprcmd_debug;

extern process tcpident_daemon(void);
extern void tcp_listen_ident_port(tcbtype *);
extern boolean tcp_access_check_ident(ulong, ulong, ipaddrtype);

