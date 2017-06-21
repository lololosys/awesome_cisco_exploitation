/* $Id: sr_envmib_ags.h,v 3.3 1995/12/01 19:03:42 sdurham Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/sr_envmib_ags.h,v $
 *------------------------------------------------------------------
 * sr_envmib.h - Environmental Monitor ags mib definitions 
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_envmib_ags.h,v $
 * Revision 3.3  1995/12/01  19:03:42  sdurham
 * CSCdi44362:  envmon is non-modular w.r.t envmon mib.
 *   Remove direct calls from envmon code to mib code. Use service points.
 *
 * Revision 3.2  1995/11/17  09:20:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:42:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:44:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef _envmib_ags_h_
#define _envmib_ags_h_

/*
 *  Defines for voltage levels in millivolts
 *
*/
#define VOLTS_P12   12000
#define VOLTS_N12  -12000
#define VOLTS_P5     5000
#define VOLTS_N5    -5000


/*
 *  Defines for voltage table size
 *
*/
#define VOLT_MAX_ENTRIES 4



/*
 *  Defines for Temperature table size
 *
*/
#define TEMP_MAX_ENTRIES 2

#endif  /* _envmib_ags_h_ */

