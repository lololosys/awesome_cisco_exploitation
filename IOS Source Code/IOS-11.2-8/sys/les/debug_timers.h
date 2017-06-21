/* $Id: debug_timers.h,v 3.2 1995/11/17 17:36:52 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/debug_timers.h,v $
 *------------------------------------------------------------------
 * debug_timers.h - some trival defs for logging times
 *
 * February 1993, Dit Morse
 *
 * Copyright (c) 1991-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: debug_timers.h,v $
 * Revision 3.2  1995/11/17  17:36:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/08  16:48:03  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.1  1995/06/07 21:30:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 *------------------------------------------------------------------
 */

/* debug times */

/* #define DEBUG_TIMES */

#ifdef DEBUG_TIMES

#define TIME_BASE sys_timestamp time_base = 0; ulong time_delta;
#define SET_TIME GET_TIMESTAMP(time_base);

extern sys_timestamp time_base;
extern ulong time_delta;

#define BEG_TIME(a) \
    time_delta = ELAPSED_TIME(time_base); \
    buginf("\nbeg sec %d.%3d: event: %s", \
	   (time_delta)/1000,\
	   (time_delta)%1000,\
	   a)
#define END_TIME(a) \
    time_delta = ELAPSED_TIME(time_base); \
    buginf("\n    end %d.%3d: event: %s", \
	   (time_delta)/1000,\
	   (time_delta)%1000,\
	   a)

#else

#define TIME_BASE
#define SET_TIME
#define BEG_TIME(a) 
#define END_TIME(a) 

#endif DEBUG_TIMES
