/*
 *	P A R S E R _ D E F S _ B O O T P . H
 *
 * $Id: parser_defs_bootp.h,v 3.2 1996/02/14 19:36:50 asb Exp $
 * $Source: /release/111/cvs/Xsys/servers/parser_defs_bootp.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_bootp.h,v $
 * Revision 3.2  1996/02/14  19:36:50  asb
 * CSCdi48113:  Should deliver NetBios NS [implement rest of RFC1877]
 *
 * Revision 3.1  1996/02/01  04:59:08  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:49:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:28:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * async-bootp command items
 */
#define BOOTP_FILE	    0
#define BOOTP_SUBNETMASK    1
#define BOOTP_TIMEOFFSET    2
#define BOOTP_GATEWAY	    3
#define BOOTP_TIME	    4
#define BOOTP_DNS	    6
#define BOOTP_LOG	    7
#define BOOTP_QUOTE	    8
#define BOOTP_LPR	    9
#define BOOTP_IMPRESS	    10
#define BOOTP_RLP	    11
#define BOOTP_HOSTNAME	    12
#define BOOTP_FILESIZE	    13
#define	BOOTP_NBNS	    14
#define BOOTP_DELETE	    15	/* Must be the last item */

