/* $Id: lat_public.h,v 3.4.32.1 1996/08/03 23:32:20 fox Exp $
 * $Source: /release/112/cvs/Xsys/lat/lat_public.h,v $
 *------------------------------------------------------------------
 * Prototypes for public routines
 *
 * January 1993, Dave Katz
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: lat_public.h,v $
 * Revision 3.4.32.1  1996/08/03  23:32:20  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.4  1996/01/10  06:49:33  syiu
 * CSCdi41302:  Concurrent LAT & bridging for HDLC and PPP
 *
 * Revision 3.3  1995/12/17  18:32:02  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:33:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:25:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern void lat_init(subsystype *subsys);
extern void lat_sethost(void);
extern void lat_media_registry(void);
extern void lat_mass_macaddrs_update(boolean);
extern boolean match_lat_group_name(char *buf, int *pi, char *num, char *name);
