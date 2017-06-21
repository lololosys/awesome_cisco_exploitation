/* $Id: ipc.h,v 3.6.6.2 1996/06/10 04:51:58 dlobete Exp $
 * $Source: /release/112/cvs/Xsys/ipc/ipc.h,v $
 *------------------------------------------------------------------
 * ipc.h - Master header file for IPC system and user primatives.
 *
 * March 1995, Jim Hayes
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipc.h,v $
 * Revision 3.6.6.2  1996/06/10  04:51:58  dlobete
 * CSCdi45148:  Error messages on VIP not reported on RP/RSP
 * Branch: California_branch
 *   - Use a more efficient scheme for slave-log port-name generation at
 *           both RP/RSP and VIP
 *   - Pull slave application details out of IPC CORE files
 *
 * Revision 3.6.6.1  1996/04/10  03:38:51  dlobete
 * CSCdi53121:  Distributed IP Access Lists need to run at interrupt-level
 * Branch: California_branch
 *
 * Revision 3.6  1996/03/04  07:12:54  kao
 * CSCdi49674:  While VIP is cofigured with DFS, OIR event will crash the
 * VIP;
 *
 * Revision 3.5  1996/02/24  02:13:13  dkerr
 * CSCdi49787:  IPC rolls over and dies under load
 * Make IPC more robust in an RSP->VIP distributed switching environment
 * (and increase performance from the miserly 100 msgs per minute to
 * something respectable, like 300K msgs per second).
 *
 * Revision 3.4  1996/01/19  01:00:54  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1996/01/13  03:09:47  dlobete
 * CSCdi44696:  OIR and IPC troubles - selective sequence number synch
 * needed
 * Add slot granularity to IPC seat reset.
 *
 * Revision 3.2  1995/11/17  09:37:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:59:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:05:28  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/13  00:12:52  dlobete
 * CSCdi34151:  UDP transport non-operational
 *
 * Revision 2.1  1995/06/07  21:02:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
 
#ifndef __IPC_H__
#define __IPC_H__

#define IPC_REALM_MANAGER_SEAT 0x00010000
#define IPC_REALM_MANAGER_NAME "IPC Realm Manager"
#define IPC_MASTER_SEAT 	0x00010000
#define IPC_MASTER_NAME 	"IPC Master"


/**********************************************************************
 * Memory overlay of IPC message header.
 */

#define ipcheadstart(pkt) ((pkt)->datagramstart)
#define ipcdatastart(pkt) ((uchar *)ipcheadstart(pkt) + IPC_MESSAGE_ENCAPS_SIZE)

typedef struct ipc_message_header_ {
    ipc_port_id dest_port;
    ipc_port_id source_port;
    ipc_sequence sequence;
    ipc_message_type type;
    ipc_size size;
    ipc_flags flags;
    uchar data[0];
} ipc_message_header;

#define IPC_MESSAGE_ENCAPS_SIZE 16
#define IPC_MESSAGE_MAX_MTU 18000
#define IPC_MAX_SEAT_NAME	50

#define IPC_CLASS_TYPE_MASK	0xE0000000
#define IPC_CLASS_A_SEAT_MASK 	0xFFFFFC00
#define IPC_CLASS_A_PORT_MASK 	0x03FF
#define IPC_CLASS_B_SEAT_MASK 	0xFFFF0000
#define IPC_CLASS_B_PORT_MASK 	0xFFFF
#define IPC_SEAT_INCREMENT	0x00010000

#define IPC_PERMANENT_PORT	-1	/* permanent port numnber, i.e. -1 */

#define is_ipcclassA(portid) ((ushort)portid & IPC_CLASS_TYPE_MASK)
#define is_ipcclassB(portid) (((ushort)portid & IPC_CLASS_TYPE_MASK) == 0)

#define IPC_PORT_IS_LOCAL  0x0001	/* This is a local port */
#define IPC_PORT_OOB	   0x0002	/* Out of band port */
#define IPC_PORT_IB	   0x0004	/* In band port */

#define IPC_FLAG_BOOTSTRAP     0x0001	/* This is a bootstrap message */
#define IPC_FLAG_ACK           0x0002	/* This is an ACK message */
#define IPC_FLAG_IS_RPC        0x0004   /* This is an RPC request. */
#define IPC_FLAG_IS_RPC_REPLY  0x0008	/* This is an RPC reply. */

#define IPC_RX_NAME_0			"SLOT_RX_0"
#define IPC_RX_NAME_1			"SLOT_RX_1"
#define IPC_RX_NAME_2			"SLOT_RX_2"
#define IPC_RX_NAME_3			"SLOT_RX_3"
#define IPC_RX_NAME_4			"SLOT_RX_4"

#define IPC_TX_NAME_0			"SLOT_TX_0"
#define IPC_TX_NAME_1			"SLOT_TX_1"
#define IPC_TX_NAME_2			"SLOT_TX_2"
#define IPC_TX_NAME_3			"SLOT_TX_3"
#define IPC_TX_NAME_4			"SLOT_TX_4"

/* This stuff isn't well defined yet. */

#define IPC_FLAG_IS_FRAGMENT   0x0010
#define IPC_FLAG_LAST_FRAGMENT 0x0020
#define IPC_FLAG_DO_NOT_ACK    0x0040	/* ack not needed on cybus */
#define IPC_FRAG_PAGE_MASK     0xff00


/**********************************************************************
 * Housekeeping information used by the IPC system when carting
 * messages around.
 *
 * BTW, the messages are kept in the retransmission table, keyed off
 * off of (dest_seat << 16) | sequence #)).  This allows us to receive
 * an ACK from a particular seat, for a particular sequence, and immediately
 * pull the original message off the retransmit queue.
 */
struct ipc_message_ {
    thread_linkage links;

    ipc_message_header *header;		/* Pointer to the message header */
    void *data;				/* Pointer to the message data */
    ipc_port_data *port_p;		/* Pointer to the received dest port */

    /* For use by server: */

    sys_timestamp retry_time;		/* Time at which we retransmit */
    mgd_timer rpc_timer;		/* managed timer used by rpc */
    uint rpc_timeout;			/* rpc timeout value in seconds */
    uint flags;				/* Message status */
    ipc_error_code result;		/* What happened to this message */
    uint sequence;			/* Unique sequence number of this
					   message [TABLE KEY] */
    uint retries;			/* How many times we've resent this */
    paktype *pak;			/* The message itself. */

    ipc_callback notify_callback;
    void *notify_context;
}; 

#define IPC_FLAG_BLOCKED      0x0001	/* Process is blocked on this message*/
#define IPC_FLAG_GOT_RPC_ACK  0x0002	/* Received ACK for RPC query */

#define IPC_FLAG_IN_CACHE     0x8000	/* So we can tell if someone is using
					   a message that has already been
					   returned... */

/*
 * Function prototypes for application's permanent port processing function
 */
typedef void (*ipc_permanent_port_process_vector)(paktype *pak);

/*
 * Function prototypes for application's delayed init function.
 */
typedef void (*ipc_delayed_func_vector)(void);
typedef void (*ipc_init_slaves_vector)(ulong);

/*
 * Priority level of the delayed init function,
 * functions with less numeric value are executed
 * before those with bigger values.
 */
typedef enum {
    IPC_LEVEL_FILESYS = 1,
    IPC_LEVEL_OTHERS
} ipc_level_t;


/**********************************************************************
 * Function prototypes
 */

/*
 * PORTS
 */

extern ipc_port_id ipc_locate_port(char *name);
extern ipc_error_code ipc_create_named_port(char *name, ipc_port_id *port, 
					    ushort band);
extern ipc_error_code ipc_register_port(ipc_port_id port);
extern ipc_error_code ipc_close_port(ipc_port_id port);

extern ipc_error_code ipc_create_multicast_port(char *name, ipc_port *port);
extern ipc_error_code
	ipc_subscribe_multicast_port(ipc_port_id port,
				     ipc_port_id new_subscriber_port);
extern ipc_error_code
	ipc_unsubscribe_multicast_port(ipc_port_id port,
				       ipc_port_id old_subscriber_port);

extern ipc_error_code
	ipc_set_receive_callback(ipc_port_id port, void *callback_context,
				 ipc_callback callback_routine);

extern ipc_error_code
	ipc_set_receive_fast_callback(ipc_port_id port, void *callback_context,
				      ipc_callback callback_routine);

extern ipc_error_code ipc_set_receive_queue(ipc_port_id port,
					    queuetype *receive_queue);

/*
 * MESSAGE SENDING
 */

extern boolean ipc_send_message_permanent_port(ipc_message *message, ipc_message_type type);
extern ipc_error_code ipc_send_message_noack (ipc_message *message);

extern ipc_error_code ipc_send_message(ipc_message *message);
extern ipc_error_code ipc_send_message_async(ipc_message *message,
					     void *callback_context,
					     ipc_callback routine);

extern ipc_error_code ipc_send_opaque(void *message_data,
				      ipc_size message_size,
				      ipc_port_id dest_port,
				      ipc_message_type type);

extern ipc_error_code ipc_send_opaque_async(void *message_data,
					    ipc_size message_size,
					    ipc_port_id dest_port,
					    ipc_message_type type,
					    void *callback_context,
					    ipc_callback routine);

extern ipc_error_code ipc_send_pak_async(paktype *pak,
					 ipc_port_id dest_port,
					 ipc_message_type type,
					 void *callback_context,
					 ipc_callback routine);

/*
 * RPC MESSAGE SENDING
 */

extern ipc_message *ipc_send_rpc(ipc_message *message,
				 ipc_error_code *error);

extern ipc_error_code ipc_send_rpc_reply(ipc_message *original_message,
					 ipc_message *reply_message);

extern ipc_error_code ipc_send_rpc_reply_async(ipc_message *original_message,
					       ipc_message *reply_message,
					       void *callback_context,
					       ipc_callback routine);
/*
 * MESSAGE BUFFERS
 */

extern ipc_message *ipc_get_message(ipc_size size,
				    ipc_port_id dest_port,
				    ipc_message_type type);

extern void ipc_set_rpc_timeout(ipc_message *message, int seconds);

extern void ipc_return_message(ipc_message *message);

/*
 * MISC ROUTINES
 */

extern void ipc_process_permanent_port_pak(paktype *pak);
extern void   ipc_flush_retry_queue(void);
extern void ipc_error_debug(char *routine, ipc_error_code code);
extern char *ipc_decode_error(ipc_error_code code);
extern void ipc_dump_message(ipc_message *message);
extern void ipc_change_seat_info(void);
extern boolean ipc_register_delayed_func(ipc_level_t level,
    ipc_delayed_func_vector vector);


/**********************************************************************
 * Inlines for manipulating port identifiers.
 */

static inline ipc_seat ipc_portid_to_seat (ipc_port_id port)
{
    if (port & IPC_CLASS_TYPE_MASK) 	/* class A address */
	return (port & IPC_CLASS_A_SEAT_MASK);
    else				/* class B address */
	return (port & IPC_CLASS_B_SEAT_MASK);
}

static inline ipc_port ipc_portid_to_port (ipc_port_id port) 
{
    if (port & IPC_CLASS_TYPE_MASK) 	/* class A address */
    	return (port & IPC_CLASS_A_PORT_MASK);
    else				/* class B address */
    	return (port & IPC_CLASS_B_PORT_MASK);

}

static inline ipc_port_id ipc_make_portid (ipc_seat seat, ipc_port port)
{
    if (seat & IPC_CLASS_TYPE_MASK) 	/* class A address */
	return ((seat & IPC_CLASS_A_SEAT_MASK) | 
					(port & IPC_CLASS_A_PORT_MASK));
    else				/* class B address */
	return ((seat & IPC_CLASS_B_SEAT_MASK) | 
					(port & IPC_CLASS_B_PORT_MASK));
}


#endif __IPC_H__
