/* $Id: clock_format_string_lengths.h,v 3.1 1995/11/09 12:44:40 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/clock_format_string_lengths.h,v $
 *------------------------------------------------------------------
 * Definitions for min string buffer lengths for clock formatting
 *
 * June 1995, Dave Katz
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This file exists so that data structures generated to hold clock
 * strings can be declared without pulling in all of clock.h.  This
 * was done to fix problems compiling boot ROMs.  Gag.
 *------------------------------------------------------------------
 * $Log: clock_format_string_lengths.h,v $
 * Revision 3.1  1995/11/09  12:44:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/21  07:45:23  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 * Ensure 7000 boot roms compile.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CLOCK_FORMAT_STRING_LENGTHS_H_
#define _CLOCK_FORMAT_STRING_LENGTHS_H_

#define CURRENT_TIME_STRING_MINBUF 33	/* For current_time_string() */
#define UNIX_TIME_STRING_MINBUF 29	/* For unix_time_string() */
#define UNIX_TIME_STRING_2_MINBUF 21	/* For unix_time_string_2() */
#define LOCAL_TIME_STRING_MINBUF 37	/* For local_time_string() */

#endif /* _CLOCK_FORMAT_STRING_LENGTHS_H_ */
