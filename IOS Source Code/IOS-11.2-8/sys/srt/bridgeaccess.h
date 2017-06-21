/* $Id: bridgeaccess.h,v 3.2.62.1 1996/05/30 23:45:29 hampton Exp $
 * $Source: /release/112/cvs/Xsys/srt/bridgeaccess.h,v $
 *------------------------------------------------------------------
 * Bridging Extended Access Lists (pattern matches)
 *
 * March 1992, Paul Traina
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: bridgeaccess.h,v $
 * Revision 3.2.62.1  1996/05/30  23:45:29  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:52:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:00:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if defined(__STDC__) || defined(__cplusplus)
# define P_(s) s
#else
# define P_(s) ()
#endif


/* bridgeaccess.c */
boolean xmacaccesscheck P_((hwidbtype *idb, int list, uchar *datagram, 
			    boolean mci_read));

boolean xmacaccesscheck_gen P_((int list,
			mac_address_type *dest, mac_address_type *source,
			uchar *data, int data_offset, int data_size));


void show_xmacaccess P_((int));
void show_xmacaccess_shell P_((int));

#undef P_

