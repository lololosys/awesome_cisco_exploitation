/* $Id: rsp_init.h,v 3.3 1995/11/17 18:48:44 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-rsp/rsp_init.h,v $
 *------------------------------------------------------------------
 * RSP initialization
 *
 * August 1994, John Foster
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_init.h,v $
 * Revision 3.3  1995/11/17  18:48:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:33:08  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:28:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:28:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/09/17  20:55:35  motto
 * CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
 *
 * Revision 2.1  1995/08/24  20:48:52  foster
 * placeholder for future development
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef	__RSP_INIT_H__
#define	__RSP_INIT_H__

extern uchar get_rsp_peer_slot (void);
extern uchar get_rsp_self_slot (void);
extern boolean open_slave_config (boolean force);
extern boolean close_slave_config (void);
extern boolean write_slave_config (char *buf, int size);
extern void sync_slave_config (boolean force, char *buf, int size);
extern void sync_slave_private_config (boolean force, char *buf, int size);
extern boolean get_slave_auto_sync_mode (void);

extern tinybool rsp2;

#endif
