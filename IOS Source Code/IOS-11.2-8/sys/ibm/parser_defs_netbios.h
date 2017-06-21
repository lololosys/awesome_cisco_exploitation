/*
 *	P A R S E R _ D E F S _ N E T B I O S . H
 *
 * $Id: parser_defs_netbios.h,v 3.3 1996/03/07 06:34:11 kchiu Exp $
 * $Source: /release/111/cvs/Xsys/ibm/parser_defs_netbios.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_netbios.h,v $
 * Revision 3.3  1996/03/07  06:34:11  kchiu
 * CSCdi40165:  Add netbios session level byte filtering support
 *
 * Revision 3.2  1996/03/04  07:13:56  kchiu
 * CSCdi49101:  make netbios name recognized filtering optional
 *
 * Revision 3.1  1996/02/01  04:58:16  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:51:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:20:43  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:30:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*  Netbios parsing
 */
#define NETBIOS_ACCESS_FILTER    0
#define NETBIOS_ACCESS_BYTES     1
#define NETBIOS_ACCESS_HOST      2
#define NETB_IAFILTER            3
#define NETB_OAFILTER            4
#define NETBIOS_ENABLE_NAMECACHE 5
#define NETBIOS_NAMECACHE_TIMEOUT 6
#define NETBIOS_NAMECACHE_QUERYTIME 7
#define NETBIOS_NAMECACHE_RECOGTIME 8
#define NETBIOS_NAMECACHE_NAME   9
#define NETBIOS_NAMECACHE_CACHE_ADD_NAME 10
#define NETB_NOV_IAFILTER	11
#define NETB_NOV_OAFILTER	12
#define NETBIOS_PROXY_DATAGRAM  13
#define NETBIOS_NAME_LENGTH     14
#define NETBIOS_BROADCAST_NAME_RECOGNIZED    15
#define NETBIOS_ENABLE_NETBEUI  16
#define NETBIOS_ACCESS_FILTER_NAME_RECOGNIZED    17
#define NETBIOS_HOST_FILTER   18
#define NETBIOS_BYTES_FILTER  19
#define NETBIOS_SESSION_BYTES_FILTER  20
