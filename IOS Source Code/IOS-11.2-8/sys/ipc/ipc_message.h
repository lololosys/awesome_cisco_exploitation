/* $Id: ipc_message.h,v 3.2 1995/11/17 17:28:44 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ipc/ipc_message.h,v $
 *------------------------------------------------------------------
 * ipc_message.h - Headers for ipc_message allocation primatives
 *
 * March 1995, Jim Hayes
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipc_message.h,v $
 * Revision 3.2  1995/11/17  17:28:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:59:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:05:37  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
 
#ifndef __IPC_MESSAGE_H__
#define __IPC_MESSAGE_H__

extern thread_table *ipc_message_table;
extern thread_table *ipc_rpc_req_table;

/*
 * Constants
 */ 

#define IPC_RPC_REQ_TABLE_SIZE 20
#define IPC_MESSAGE_TABLE_SIZE 97
#define IPC_MESSAGE_CACHE_SIZE 100

#define	IPC_RPC_REQ_TABLE_MAX  20

/*
 * Function Prototypes
 */

extern ipc_message *ipc_get_message_from_cache(void);
extern void ipc_return_message_to_cache(ipc_message *);

extern boolean ipc_init_message_system(void);
extern ipc_message *ipc_make_message_from_pak(paktype *);
extern void ipc_show_message_queue(parseinfo *csb);
extern ipc_message *ipc_get_message_by_sequence(thread_table * ipc_thread,
	uint sequence);

#endif __IPC_MESSAGE_H__
