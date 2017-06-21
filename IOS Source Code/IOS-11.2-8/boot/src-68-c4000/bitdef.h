/* $Id: bitdef.h,v 3.2 1995/11/17 07:42:43 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c4000/bitdef.h,v $
 *------------------------------------------------------------------
 * bitdef.h - printf bitdef support structure
 *
 * April 1992, Rob Clevenger
 *
 * Copyright (c) 1992-1993 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: bitdef.h,v $
 * Revision 3.2  1995/11/17  07:42:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:09:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:15:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


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

