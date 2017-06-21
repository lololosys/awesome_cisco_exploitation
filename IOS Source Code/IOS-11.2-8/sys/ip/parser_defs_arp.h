/*
 *	P A R S E R _ D E F S _ A R P . H
 *
 * $Id: parser_defs_arp.h,v 3.2 1996/03/01 20:20:53 rchandra Exp $
 * $Source: /release/111/cvs/Xsys/ip/parser_defs_arp.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_arp.h,v $
 * Revision 3.2  1996/03/01  20:20:53  rchandra
 * CSCdi50381:  IP: show ip arp command is not flexible
 * - make 'show ip arp [<address>] [<hostname>] [<mac>] [<interface>]' work
 *
 * Revision 3.1  1996/02/01  04:58:33  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1996/01/29  07:00:42  hampton
 * Move parser command defines from the common "exec" file to their
 * respective parser_defs_xxx.h files.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:49:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:28:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * ARP types
 */
#define ARP_SAP	        	 0
#define ARP_ARPA        	 1
#define	ARP_SMDS		 2
#define ARP_SNAP       		 3
#define ARP_ULTRA       	 4
#define ARP_PROBE       	 5
#define ARP_ENTRY_TIME  	 6
#define ARP_FR                   7

/* Type of hardware address for 'arp' command */
#define ADDR_ETHER		0
#define ADDR_ULTRA		1

/* 
 * codes used by the 'show ip arp' command 
 */

enum {
    SHOW_ARP,
    SHOW_ARP_INTERFACE,
    SHOW_ARP_ADDR,
    SHOW_ARP_MAC
};
