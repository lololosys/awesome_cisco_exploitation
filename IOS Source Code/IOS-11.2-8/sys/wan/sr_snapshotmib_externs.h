/* $Id: sr_snapshotmib_externs.h,v 3.2 1995/11/17 18:05:14 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/wan/sr_snapshotmib_externs.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * March 20, 1995  Sandra Durham
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_snapshotmib_externs.h,v $
 * Revision 3.2  1995/11/17  18:05:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:51:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:20:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _snapshotmib_externs_h_
#define _snapshotmib_externs_h_

/* these need to be accessed from snapshot.h, but can't use snapshot.h
 * because of all the extra stuff in there dependent on other .h files.
 * Can't add to snapshotmib.h because sr_snapshotmiboid.c won't have
 * the added snapshot.h and snapshot_private.h.  So here it is...
 */
  
  
extern int snapshot_last_forced_if_index;
extern int snapshot_interface_info_index;

extern void snapshot_delete_usage (snapshot_interface_info *);
extern void snapshot_delete_all_activity_blocks (snapshot_interface_info *);
extern void snapshot_create_initial_activity_blocks (snapshot_interface_info *);
extern inline snapshot_interface_info *snapshot_find_idb (idbtype *idb);


#endif /* _snapshotmib_externs_h_ */






