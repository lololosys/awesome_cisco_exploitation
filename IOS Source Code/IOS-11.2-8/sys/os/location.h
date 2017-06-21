/* $Id: location.h,v 3.1.2.1 1996/04/16 19:09:59 rchandra Exp $
 * $Source: /release/112/cvs/Xsys/os/location.h,v $
 *------------------------------------------------------------------
 * Declarations for system location
 *
 * December 1995, Dave Katz
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: location.h,v $
 * Revision 3.1.2.1  1996/04/16  19:09:59  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/15  09:39:36  dkatz
 * Add placeholder.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define MINUTES_PER_DEGREE 60
#define ARC_SECONDS_PER_MINUTE 60
#define CENTISECS_PER_SECOND 100

extern void location_set(int longitude, int latitude, int altitude);
extern void location_get(int *longitude, int *latitude, int *altitude);
extern void location_centisecs_to_dmsf(int location, int *degrees,
				       ulong *minutes, ulong *seconds,
				       ulong *hundredths);
extern void show_location(parseinfo *csb);
