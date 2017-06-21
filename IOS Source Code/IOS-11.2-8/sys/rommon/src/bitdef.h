/* $Id: bitdef.h,v 3.2 1995/11/17 18:41:01 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src/bitdef.h,v $
 *------------------------------------------------------------------
 * bitdef.h
 *
 * October 1992, Rob Clevenger
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Bit definition file for the monitor's printf function.
 *------------------------------------------------------------------
 * $Log: bitdef.h,v $
 * Revision 3.2  1995/11/17  18:41:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:04:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:36:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __BITDEF_H__
#define __BITDEF_H__
/*
** Support for printf's ability to print a string for
** register bits.
** The actstate (active state) field defines the state of
** the bit when active.
** The string associated with the bit will be printed if
** the bit is active.
*/

struct bitdef {
    int actstate;
    char *string;
};

#endif  /* __BITDEF_H__ */
