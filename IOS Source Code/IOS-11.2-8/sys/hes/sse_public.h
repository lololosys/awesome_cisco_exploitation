/* $Id: sse_public.h,v 3.2.60.1 1996/03/18 19:43:58 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/hes/sse_public.h,v $
 *------------------------------------------------------------------
 * sse_public.h -- public declarations for sse.c
 *
 * Feb 1993 Tony Li
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sse_public.h,v $
 * Revision 3.2.60.1  1996/03/18  19:43:58  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  21:48:26  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  09:21:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:43:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:45:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern void sse_init(mcitype *cbus);
extern void sse_error(mcitype *cbus);
extern boolean sse_real(ssetype *sse);
extern void sse_generic_consistency(ssetype *sse, int protocol);
