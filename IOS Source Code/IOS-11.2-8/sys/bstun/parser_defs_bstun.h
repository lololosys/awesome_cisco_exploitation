/* $Id: parser_defs_bstun.h,v 3.1.40.1 1996/05/17 10:41:15 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/bstun/parser_defs_bstun.h,v $
 *------------------------------------------------------------------
 * parser_defs_bstun.h - BSTUN configuration command constants
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_bstun.h,v $
 * Revision 3.1.40.1  1996/05/17  10:41:15  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.26.1  1996/04/11  08:19:55  jbalestr
 * CSCdi51807:  need a test command to transmit raw bisync frames
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1  1996/02/01  04:57:17  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1995/11/21  08:52:53  jbalestr
 * CSCdi42960:  bstun keepalive count
 * Added bstun keepalive interval and count
 *
 * Revision 3.2  1995/11/17  18:49:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:29:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* BSTUN command defines */
#define BSTUN_PEER		 0
#define BSTUN_PROTOCOL		 1
#define BSTUN_KEEPALIVE          2
#define BSTUN_KEEPALIVE_COUNT    3

/* BSTUN Interface subcommand defines */
#define BSTUNIF_GROUP		 0
#define BSTUNIF_ROUTE		 1
#define	NO_BSTUN		 2

/* BSTUN route */
#define BSTUN_RTALL		 1
#define BSTUN_RTADDR		 2
#define BSTUN_TCP		 3
#define BSTUN_INTF		 4
#define BSTUN_TEST		 5

