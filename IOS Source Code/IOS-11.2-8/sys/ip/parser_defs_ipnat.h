/* $Id: parser_defs_ipnat.h,v 3.1.6.2 1996/06/03 23:23:22 ahh Exp $
 * $Source: /release/112/cvs/Xsys/ip/parser_defs_ipnat.h,v $
 *------------------------------------------------------------------
 * IP Network Address Translation parser definitions
 *
 * October 1995, Andy Heffernan
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_ipnat.h,v $
 * Revision 3.1.6.2  1996/06/03  23:23:22  ahh
 * CSCdi59119:  IP: NAT should support named access-lists
 * And extended access-lists and non-contiguous address pools.
 * Branch: California_branch
 *
 * Revision 3.1.6.1  1996/04/16  18:53:30  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/07  06:00:02  ahh
 * Placeholder...
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * NAT interface command
 */

enum {
    IPNAT_IF_INSIDE,
    IPNAT_IF_OUTSIDE
};

/*
 * NAT global ip command
 */

enum {
    IPNAT_TIMEOUT_CMD,
    IPNAT_ISOURCE,
    IPNAT_OSOURCE,
    IPNAT_IDESTINATION,
    IPNAT_ODESTINATION,
    IPNAT_POOL,
    IPNAT_MAX_ENTRIES
};

/*
 * NAT pool subcommands
 */

enum {
    IPNAT_POOL_ADDRESS
};

/*
 * NAT clear and/or show keywords
 */

enum {
    IPNAT_DYNAMIC,
    IPNAT_STATS
};
