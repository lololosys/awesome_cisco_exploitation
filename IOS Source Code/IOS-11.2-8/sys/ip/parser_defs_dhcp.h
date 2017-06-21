/* $Id: parser_defs_dhcp.h,v 3.1 1996/02/01 04:58:34 hampton Exp $
 * $Source: /release/111/cvs/Xsys/ip/parser_defs_dhcp.h,v $
 *-------------------------------------------------------------------
 * parser_defs_dhcp.h - DHCP proxy client   DHCP constants for parser
 *
 * Mar 10, 1994, Syed Irfan Ashraf
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_dhcp.h,v $
 * Revision 3.1  1996/02/01  04:58:34  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1996/01/29  07:00:51  hampton
 * Move parser command defines from the common "exec" file to their
 * respective parser_defs_xxx.h files.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:50:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:29:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *	P A R S E R _ D E F S _ D H C P . H
 */

/*
 * DHCP defines
 */

/*
 * DHCP Proxy Client:  Interface configuration commands
 */

enum DHCP_COMMANDS {
    PEER_DEFAULT_IP_ADDRESS_NONE,     /* No peer ip addr needed */
    PEER_DEFAULT_IP_ADDRESS_POOL,     /* Use pool mechanism for peer */
    PEER_DEFAULT_IP_ADDRESS_ADDRESS   /* Actual ip addr for peer */
};

/* DHCP show commands */
enum {
    SHOW_DHCP_SERVER,
    SHOW_DHCP_LEASE,
};
