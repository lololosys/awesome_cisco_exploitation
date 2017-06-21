/* $Id: parser_defs_fast_ethernet.h,v 3.2 1996/02/13 21:25:53 rbatz Exp $
 * $Source: /release/111/cvs/Xsys/if/parser_defs_fast_ethernet.h,v $
 *------------------------------------------------------------------
 * parser_defs_fast_ethernet.h:  Definitions for fast ethernet parser commands.
 *
 * November 1994, Gary Kramling
 * Adapted from parser_defs_channel.h by Stig Thormodsrud
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: parser_defs_fast_ethernet.h,v $
 * Revision 3.2  1996/02/13  21:25:53  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.1  1996/02/01  04:58:22  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:50:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/09  00:21:36  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.1  1995/06/07  22:29:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __PARSER_DEFS_FAST_ETHERNET_H__
#define __PARSER_DEFS_FAST_ETHERNET_H__

/*
 * Fast Ethernet platform specific commands
 */
#define PARSER_MEDIATYPE_MII            1
#define PARSER_MEDIATYPE_100BASET       2

/*
 *  FastEthernet commands
 */
typedef enum _fether_parser_cmd_t {
    FULL_DUPLEX_CMD
} fether_parser_cmd_t;

#endif	/* __PARSER_DEFS_FAST_ETHERNET_H__ */
