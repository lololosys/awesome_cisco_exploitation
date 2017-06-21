/* $Id: powerfail.h,v 3.3 1995/11/17 09:19:45 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/powerfail.h,v $
 *------------------------------------------------------------------
 * Powerfail info. to be shared between system and rom monitor.
 *
 * Sept. 1992, Gary Kramling
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Longer description here.
 *------------------------------------------------------------------
 * $Log: powerfail.h,v $
 * Revision 3.3  1995/11/17  09:19:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:29:15  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:41:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/16  04:27:39  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  20:43:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

#ifndef __POWERFAIL_H__
#define __POWERFAIL_H__

#define SUPPLY1_PF        0x00040000
#define SUPPLY2_PF        0x00080000

typedef struct {
   ulong pf_addr;
   ulong pf_addr_save;
   uint  intermittent_pf_count;
   ulong time_of_last_pf___old_no_longer_used;
   char  pf_date_time_snap[CURRENT_TIME_STRING_MINBUF];
   sys_timestamp time_of_last_pf;
   ulong reserved_3;
   ulong reserved_4;
   ulong reserved_5;
} powerfail_info_t;

#endif
