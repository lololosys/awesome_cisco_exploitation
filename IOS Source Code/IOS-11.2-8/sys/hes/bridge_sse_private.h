/* $Id: bridge_sse_private.h,v 3.2.60.1 1996/03/18 19:39:29 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/hes/bridge_sse_private.h,v $
 *------------------------------------------------------------------
 * bridge_sse_private.h
 *
 * Feb 1993 Tony Li
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bridge_sse_private.h,v $
 * Revision 3.2.60.1  1996/03/18  19:39:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  21:44:43  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  09:14:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:37:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:37:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

enum SSE_BRIDGE_MEDIA {SSE_BRIDGE_ETHER, SSE_BRIDGE_FDDI};

struct sse_tbinfo_ {
    spintype *ether_bridge_tree[MAX_SSE_BRIDGE_GROUPS];	/* Ethernet tree */
    spintype *ether_group_tree[MAX_SSE_BRIDGE_GROUPS];
    spintype *fddi_bridge_tree[MAX_SSE_BRIDGE_GROUPS];  /* FDDI tree */
    spintype *fddi_group_tree[MAX_SSE_BRIDGE_GROUPS];
    ssestattype bridge_stats[MAX_SSE_BRIDGE_GROUPS];    /* Usage stats */
    uint bce_version;
    uint rp_version;
    boolean reset;			/* True if things need to be reset */
    boolean setup;			/* TRUE if things need to be set up */
};

#define FCI_BRIDGE_ON	1
#define FCI_BRIDGE_OFF	0

extern void bridge_sse_chain_init(void);
