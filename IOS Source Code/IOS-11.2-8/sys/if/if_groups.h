/* $Id: if_groups.h,v 3.2.60.1 1996/09/03 22:37:18 sjackson Exp $
 * $Source: /release/112/cvs/Xsys/if/if_groups.h,v $
 *------------------------------------------------------------------
 * Group interface abstraction definitions
 *
 * April 1995, Tim Kolar
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_groups.h,v $
 * Revision 3.2.60.1  1996/09/03  22:37:18  sjackson
 * CSCdi66297:  All interfaces reset (all calls dropped) when group-range
 * applied
 * Avoid reconfiguring existing interfaces within the group range
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:28:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:50:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:52:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef _IF_GROUPS_H_
#define _IF_GROUPS_H_

#define NEW_LINE 10   /* ascii New Line */
#define MAX_COMMAND_BUFSIZE 4000 /* a reasonable amount methinks */

/* States for end_of_interface */

#define STATE_NORMAL    1
#define STATE_FOUND_NEW_LINE 2

/* maximum size of an interface name */
#define INTERFACE_NAME_SIZE 50

void unconfigure_interfaces(uchar *, uint, uchar *, int, int, int, int);
void sync_to_master_interface(uchar *, uint, uchar *, int, uchar *,
					int, int, int, int);

#endif /* _IF_GROUPS_H_ */
