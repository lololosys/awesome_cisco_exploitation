/*
 *	P A R S E R _ D E F S _ S M D S . H
 *
 * $Id: parser_defs_smds.h,v 3.1 1996/02/01 05:14:36 hampton Exp $
 * $Source: /release/111/cvs/Xsys/wan/parser_defs_smds.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_smds.h,v $
 * Revision 3.1  1996/02/01  05:14:36  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1996/01/29  07:01:03  hampton
 * Move parser command defines from the common "exec" file to their
 * respective parser_defs_xxx.h files.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:52:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/15  19:23:31  lmercili
 * CSCdi34816:  add dynamic mapping for ipx/smds
 *
 * Revision 2.1  1995/06/07  22:31:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* SMDS defines */

#define SMDS_PROTO_AARP		 0
#define SMDS_PROTO_ATALK	 1
#define SMDS_PROTO_ARP		 2
#define SMDS_PROTO_CLNS		 3
#define SMDS_PROTO_CLNSES	 4
#define SMDS_PROTO_CLNSIS	 5
#define SMDS_PROTO_DECNET	 6
#define SMDS_PROTO_DNNODE	 7
#define SMDS_PROTO_DNROUTER	 8
#define SMDS_PROTO_IP		 9
#define SMDS_PROTO_NOVELL	10
#define SMDS_PROTO_XNS		11

#define SMDS_HARDWARE           0
#define SMDS_ARP_ON             1
#define SMDS_D15                2	/* OBSOLETE */
#define SMDS_MAP                3
#define SMDS_BA                 4
#define SMDS_NEC                5
#define SMDS_DXI		6
#define IFNSMDSKEY              7
#define SMDS_GLEAN              8

/* SMDS show commands */
enum {
    SHOW_SMDS_ADDRESSES,
    SHOW_SMDS_MAP,
    SHOW_SMDS_TRAFFIC,
};
