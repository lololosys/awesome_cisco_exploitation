/* $Id: ipc_test.h,v 3.2 1995/11/17 17:28:59 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ipc/ipc_test.h,v $
 *------------------------------------------------------------------
 * ipc_test.h - Header file for IPC test code.
 *
 * March 1995, Jim Hayes
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipc_test.h,v $
 * Revision 3.2  1995/11/17  17:28:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:00:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:03:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define IPC_CBUS_TEST_SEAT 	0x000A0000
#define IPC_CBUS_TEST_PORT_ID 	0x000A0001
#define IPC_CBUS_TEST_SEAT_NAME "IPC CBUS Test"

extern void ipc_test_api(parseinfo *csb);
extern void ipc_test_rpc(parseinfo *csb);
extern void ipc_test_queue(parseinfo *csb);
extern void ipc_test_callback(parseinfo *csb);
extern void ipc_test_misc(parseinfo *csb);
