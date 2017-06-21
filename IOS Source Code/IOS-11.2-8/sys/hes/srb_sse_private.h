/* $Id: srb_sse_private.h,v 3.2.60.2 1996/05/17 11:19:16 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/hes/srb_sse_private.h,v $
 *------------------------------------------------------------------
 * srb_sse_private.h
 *
 * March 1993     Tony Li, Percy P. Khabardar, William H. Palmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: srb_sse_private.h,v $
 * Revision 3.2.60.2  1996/05/17  11:19:16  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/26  07:53:43  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.2.48.1  1996/04/03  14:32:17  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.60.1  1996/03/18  19:43:40  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  21:48:07  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  09:20:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:42:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/21  06:41:54  gchristy
 * CSCdi35154:  SSE: SRB can sometimes fast-switch
 *   - don't assume bridge numbers are symmetric
 *   - only skip RCF bytes on a failure
 *   - restart RCF parsing with node immediately following ICB compare
 *   - allow SSE switching when AST is configured
 *
 * Revision 2.1  1995/06/07  20:44:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__SRB_SSE_PRIVATE_H__
#define	__SRB_SSE_PRIVATE_H__

#define SRB_SSE_DPRIF    4     /* dual-port rif size */
#define SRB_SSE_MPRIF    6     /* multi-port rif size */
#define SRB_SSE_MAXRIF   6     /* max sse srb rif size to match */ 

struct sse_srbinfo_ {
    boolean update_needed;		/* TRUE if we need to do work */
    spintype *tr_dest_mac_tree;		/* Tree of our mac addresses */
};

/*
 * srb_sse.c
 */ 
void srb_sse_chain_init(void);

#endif	__SRB_SSE_PRIVATE_H__
