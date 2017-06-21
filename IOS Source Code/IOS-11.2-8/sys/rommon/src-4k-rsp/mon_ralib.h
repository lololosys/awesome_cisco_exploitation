/* $Id: mon_ralib.h,v 3.2 1995/11/17 18:46:45 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src-4k-rsp/mon_ralib.h,v $
 *------------------------------------------------------------------
 * mon_ralib.h
 *
 * Mar 95, Steve J. Zhang
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * The module defines constants and inline function which are 
 * missed in the RSP ROM Monitor .h files.
 *------------------------------------------------------------------
 * $Log: mon_ralib.h,v $
 * Revision 3.2  1995/11/17  18:46:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/09  13:18:37  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:41:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



/*-----------------------------------------------------------------------------*
 * Inline function for short and long manipulation
 *-----------------------------------------------------------------------------*/

#define u_char unsigned char

/*
 * support protected writes to dbus registers
 */

#define ALL_DISABLE        all_disable
extern const uint all_disable;


#define GETSHORT(ptr)	    getshort_inline(ptr)
#define GETLONG(ptr)        getlong_inline(ptr)

void createlevel(int level, void (*fn)(void), char *str);
void process_sleep_for(ulong msecs);
/*
extern struct mon_iface m_iface; 
*/

#define TRUE        1
#define FALSE       0


