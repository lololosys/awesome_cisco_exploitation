/*
 *	P A R S E R _ D E F S _ P T . H
 *
 * $Id: parser_defs_pt.h,v 3.2 1996/02/22 05:20:25 fox Exp $
 * $Source: /release/111/cvs/Xsys/pt/parser_defs_pt.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_pt.h,v $
 * Revision 3.2  1996/02/22  05:20:25  fox
 * CSCdi35908:  Should allow CHAP and PAP clients to same dialin pool
 * Add support for multiple acceptable inbound PPP authentication methods
 *
 * Revision 3.1  1996/02/01  04:59:06  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:52:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:30:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * translate commands
 */
#define TRANS_PROT_OLD           0
#define TRANS_PROT_LAT           1
#define TRANS_PROT_TCP           2
#define TRANS_PROT_X25           3
#define TRANS_PROT_SLIP_PPP      4
#define PT_VTY_ASYNC_ENABLED     5
#define PT_HEADER_COMP           6
#define PT_DYNAMIC_ROUTING       7
#define PT_KEEPALIVE             8
#define PT_PPP_AUTHENTICATION   11
#define PT_PPP_USE_TACACS       12
#define PT_INT_MTU              13
#define TRANS_PROT_AUTOCOMMAND  14
#define PT_IPX_PPPCLIENT        15
