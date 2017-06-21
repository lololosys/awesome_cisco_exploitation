/* $Id: boxtypes.h,v 3.3 1996/02/21 23:39:02 lcheung Exp $
 * $Source: /release/111/cvs/Xsys/h/boxtypes.h,v $
 *------------------------------------------------------------------
 * boxtypes.h  -- The various sorts of boxes that a cisco can be.
 *
 * December 1991, Bill Westfield
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: boxtypes.h,v $
 * Revision 3.3  1996/02/21  23:39:02  lcheung
 * add a new Boxtype. Switch cannot perform routing
 * CSCdi49518:  Add new BOXTYPE_SW type
 *
 * Revision 3.2  1995/11/17  09:11:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:34:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:34:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * System types
 * In addition to the many features that a system can have, it is also
 * a specific type of box to begin with.  This can change which features
 * are turned on by default, and which must be explicitly configured.
 * For example, a Terminal Server may have the ability to route over SLIP
 * links, but unless that feature is turned on, the box should NOT run IP
 * routing.
 *
 * Note that a trouter is defined as a router with TS capability, and a
 * igs/pt is a router with PT capability.
 */

#define BOXTYPE_GS    1		/* Router */
#define BOXTYPE_TS    2		/* Terminal Server */
#define BOXTYPE_PT    3		/* Protocol Translator */
#define BOXTYPE_SW    4 	/* Switch (cannot perform routing) */

extern int boxtype;
