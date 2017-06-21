/* $Id: ms_xchng.h,v 3.3 1995/12/05 07:06:28 dbath Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-rsp/ms_xchng.h,v $
 *------------------------------------------------------------------
 * ms_xchng.h - interface provided by master_xchng.c and slave_xchng.c
 *
 * September 1995, David Bath
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ms_xchng.h,v $
 * Revision 3.3  1995/12/05  07:06:28  dbath
 * CSCdi43178:  rommon boot info not kept in sync on slave
 * Ship the three boot related environment variables, and the config
 * register setting, across to the slave whenever autosync is enabled,
 * or whenever the user manually syncs the slave config.
 *
 * Revision 3.2  1995/11/17  18:46:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:27:47  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/14  03:53:10  dbath
 * Placeholders for VIP_branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern void rsp_slave_init(void);
extern void sanitise_ms_settings(void);
extern void slave_parser_init(void);
extern boolean get_slave_monvar(int, boolean*, char*);
extern void manual_set_slave_monvars(boolean);
extern void auto_set_slave_confreg(ushort, boolean);
