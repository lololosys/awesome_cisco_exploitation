/* $Id: sr_envmib_7000.h,v 3.2 1995/11/17 09:20:13 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/sr_envmib_7000.h,v $
 *------------------------------------------------------------------
 *   7000-specific header for Environmental Monitor Mib 
 *
 * Nov 1994,   Sandra C. Durham
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_envmib_7000.h,v $
 * Revision 3.2  1995/11/17  09:20:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:42:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:44:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _envmib_7000_h_
#define _envmib_7000_h_


/*
 *  Defines for voltage levels in millivolts
 *
*/
#define VOLTS_P12   12000
#define VOLTS_N12  -12000
#define VOLTS_P5     5000
#define VOLTS_P24   24000


/*
 *  Defines for table size
 *
*/
#define VOLT_MAX_ENTRIES 4
#define TEMP_MAX_ENTRIES 2
#define NUM_FANS_7010 6
#define NUM_FANS_7000 1
#define NUM_PWR_7010 1
#define NUM_PWR_7000 2

#endif  /* _envmib_7000_h_ */



