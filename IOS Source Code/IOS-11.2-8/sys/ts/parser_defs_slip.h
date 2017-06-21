/*
 *	P A R S E R _ D E F S _ S L I P . H
 *
 * $Id: parser_defs_slip.h,v 3.1.40.1 1996/08/14 06:25:52 irfan Exp $
 * $Source: /release/112/cvs/Xsys/ts/parser_defs_slip.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_slip.h,v $
 * Revision 3.1.40.1  1996/08/14  06:25:52  irfan
 * CSCdi65960:  slip line command should be removed from IOS
 * Branch: California_branch
 * Remove code for obsolete UI.
 *
 * Revision 3.1  1996/02/01  05:14:08  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:52:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/18  19:37:17  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.1  1995/06/07  22:31:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * SLIP defines
 */
#define SLIP_NONE	0
#define SLIP_STATIC	1	/* Not attempting dynamic slip */
#define SLIP_DEFAULT	2
#define SLIP_IPADDR	3
#define SLIP_HOST	4

/*
 * Options found on the SLIP command (in object int 8)
 */

#define SLIP_OPTION_COMPRESSED 	0x1
#define SLIP_OPTION_ROUTING	0x2
#define SLIP_OPTION_DEFAULT	0x4
#define SLIP_OPTION_IPADDR	0x8
#define SLIP_OPTION_NEGOTIATE	0x10


/*
 * Async line configuration commands
 */

enum ASYNC_MODE {
    ASYNC_MODE_INT,
    ASYNC_MODE_DED,
    ASYNC_MODE_NONE,
    ASYNC_DEFAULT_ROUTING,
    ASYNC_DYNAMIC_ROUTING,
    ASYNC_DYNAMIC_ADDRESS,
    ASYNC_DEFAULT_IP_ADDRESS,
    ASYNC_GROUP_RANGE,
    ASYNC_GROUP,
    GROUP_DESCRIPTION,
    GROUP_PEER_DEFAULT_IP_ADDRESS
};
