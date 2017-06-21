/* $Id: time.h,v 3.2 1995/11/17 18:52:20 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/time.h,v $
 *------------------------------------------------------------------
 * time.h - Time structure
 *
 * February 1991, David Hampton
 *
 * Copyright (c) 1991-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: time.h,v $
 * Revision 3.2  1995/11/17  18:52:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:48:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:59:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TIME_H__
#define __TIME_H__

/*
 * Definitions for printing times
 */

#define TIME_ABSOLUTE 0 
#define TIME_ELAPSED  1
#define TIME_RELATIVE 2
#define TIME_ELAPSED_CENTISEC  3

typedef struct timetype_ {
    ulong weeks;
    ulong days;
    ulong hours;
    ulong minutes;	
    ulong seconds;
} timetype;

#endif __TIME_H__
