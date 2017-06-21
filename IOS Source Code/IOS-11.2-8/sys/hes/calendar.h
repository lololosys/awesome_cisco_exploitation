/* $Id: calendar.h,v 3.2.60.1 1996/03/18 19:39:38 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/hes/calendar.h,v $
 *------------------------------------------------------------------
 * calendar.h - Real-Time Calendar private definitions 
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: calendar.h,v $
 * Revision 3.2.60.1  1996/03/18  19:39:38  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  21:44:51  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  09:14:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:37:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/16  04:27:26  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  20:37:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

#define YEAR_MASK       0xFF
#define MONTH_MASK      0x1F
#define DATE_MASK       0x3F
#define DAY_MASK        0x7

#define HOURS_MASK      0x3F
#define MINUTES_MASK    0x7F
#define SECONDS_MASK    0x7F
