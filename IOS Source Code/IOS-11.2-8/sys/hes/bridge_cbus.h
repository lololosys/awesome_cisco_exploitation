/* $Id: bridge_cbus.h,v 3.2.60.2 1996/05/04 00:44:07 wilber Exp $
 * $Source: /release/112/cvs/Xsys/hes/bridge_cbus.h,v $
 *------------------------------------------------------------------
 * cBus bridging external declarations
 *
 * March 1993, Tony Speakman
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * External function declarations for cBus-bridging specific functions.
 *
 *------------------------------------------------------------------
 * $Log: bridge_cbus.h,v $
 * Revision 3.2.60.2  1996/05/04  00:44:07  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Rip out MTR transparent bridging support.
 *
 * Constrained Multicast Flooding
 * - Convert tbridge flood array to a linked list of tbifd's.
 *
 * Revision 3.2.60.1  1996/03/18  19:39:24  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  21:44:38  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  09:14:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:37:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:37:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern boolean cbus_flood_all(idbtype *, tbifd_type *, boolean *);
extern boolean cbus_flood_oncomplex(hwidbtype *input);
