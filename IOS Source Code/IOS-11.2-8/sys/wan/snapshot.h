/* $Id: snapshot.h,v 3.2 1995/11/17 18:04:51 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/wan/snapshot.h,v $
 *------------------------------------------------------------------
 * Definitions for snapshot routes.
 *
 * April 1994, Joel P. Bion
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Definitions for snapshot support.
 *------------------------------------------------------------------
 * $Log: snapshot.h,v $
 * Revision 3.2  1995/11/17  18:04:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:51:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:33:01  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:19:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef _SNAPSHOT_H_
#define _SNAPSHOT_H_

#include "../h/address.h"

extern void snapshot_init(subsystype *);
extern void snapshot_parser_init(void);
extern void snapshot_media_registry(void);
extern void show_snapshot(parseinfo *);
extern void snapshotif_command(parseinfo *);
extern void snapshot_command(parseinfo *);
extern void snapshot_build_address(idbtype *, paktype *, addrtype *);
extern void snapshot_force_idb_active(idbtype *, boolean);
extern void snapshot_clear_quiet_period(parseinfo *);
extern forktype snapshot_tasker (void);


/* Snapshot packet flags -- defined in packet.h */

/* Packet is an update on Snapshot enabled interface - dialer should ignore */
#define SNAPSHOT_PAK_IS_UPDATE (0x1)

/* Address used to indicate non-dialer usage. These have to be the same! */
#define SNAPSHOT_NO_DIALER_ADDR (0)
#define SNAPSHOT_SERVER_DIALER_ADDR (SNAPSHOT_NO_DIALER_ADDR)

#endif

