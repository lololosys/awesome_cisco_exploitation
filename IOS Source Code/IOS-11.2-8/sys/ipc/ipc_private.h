/* $Id: ipc_private.h,v 3.6.6.3 1996/08/28 12:50:45 thille Exp $
 * $Source: /release/112/cvs/Xsys/ipc/ipc_private.h,v $
 *------------------------------------------------------------------
 * ipc_private.h - Non-public definitions for IPC server system.
 *
 * March 1995, Jim Hayes
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipc_private.h,v $
 * Revision 3.6.6.3  1996/08/28  12:50:45  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.6.2  1996/05/12  23:15:41  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.6.6.1  1996/04/10  03:38:55  dlobete
 * CSCdi53121:  Distributed IP Access Lists need to run at interrupt-level
 * Branch: California_branch
 *
 * Revision 3.6  1996/03/04  07:12:58  kao
 * CSCdi49674:  While VIP is cofigured with DFS, OIR event will crash the
 * VIP;
 *
 * Revision 3.5  1996/02/24  02:13:21  dkerr
 * CSCdi49787:  IPC rolls over and dies under load
 * Make IPC more robust in an RSP->VIP distributed switching environment
 * (and increase performance from the miserly 100 msgs per minute to
 * something respectable, like 300K msgs per second).
 *
 * Revision 3.4  1996/02/09  18:37:49  mbandi
 * CSCdi44416:  IPC RPC timeout is too long - Router hangs.
 * Increase the timeout to 25 minutes, for the slave devices
 * squeeze operation.
 *
 * Revision 3.3  1996/01/13  03:09:51  dlobete
 * CSCdi44696:  OIR and IPC troubles - selective sequence number synch
 * needed
 * Add slot granularity to IPC seat reset.
 *
 * Revision 3.2  1995/11/17  17:28:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:59:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:05:39  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/09/17  20:51:44  motto
 * CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
 *
 * Revision 2.1  1995/06/07  21:02:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
 
#ifndef __IPC_PRIVATE_H__
#define __IPC_PRIVATE_H__

/*
 * IPC Server global variables.
 */

typedef struct ipc_global_info_ {
    boolean   is_ipc_master;		/* TRUE if we're a master server */
    boolean   is_ipc_realm_manager;	/* TRUE if we're a realm manager */
    boolean   is_ipc_zone_manager;	/* TRUE if we're a zone manager */
    queuetype inboundQ;			/* Raw Input IPC packets */
    queuetype realm_inboundQ;		/* Raw Input IPC packets */
    queuetype zone_inboundQ;		/* Raw Input IPC packets */
    sys_timestamp periodic;		/* Next periodic server check */

    ipc_seat  local_seat_addr;		/* Our local address */
    ipc_seat  last_seat_addr;		/* last address we assigned */
    ipc_port  last_port;		/* last port we assigned */
    ipc_port_id  zone_manager_port;	/* address for zone manager PortID */

    int	      zone_manager_pid;
    int	      realm_manager_pid;
    int	      seat_manager_pid;

    ipc_port_id my_echo_port;		/* Responds to messages */
    ipc_port_id my_server_port;		/* Where other servers reach me */

    void *slave_list_ptr;		/* points to list of slave IPs */

    ipc_port_id rx_oob_port0;		/* Out-of-band port for slot 0 */
    ipc_port_id rx_oob_port1;		/* Out-of-band port for slot 1 */
    ipc_port_id rx_oob_port2;		/* Out-of-band port for slot 2 */
    ipc_port_id rx_oob_port3;		/* Out-of-band port for slot 3 */
    ipc_port_id rx_oob_port4;		/* Out-of-band port for slot 4 */

    ipc_port_id tx_oob_port0;		/* Out-of-band port for slot 0 */
    ipc_port_id tx_oob_port1;		/* Out-of-band port for slot 1 */
    ipc_port_id tx_oob_port2;		/* Out-of-band port for slot 2 */
    ipc_port_id tx_oob_port3;		/* Out-of-band port for slot 3 */
    ipc_port_id tx_oob_port4;		/* Out-of-band port for slot 4 */

    /*
     * Good Statistics
     */

    uint      received;			/* Inbound messages received */
    uint      sent;			/* Outbound messages sent */
    uint      delivered;		/* Messages delivered to local ports */
    uint      acks_in;			/* Acknowledgements received */
    uint      acks_out;			/* Acknowledgements sent */
    uint      fast_dfs;			/* Fast-dfs ipc messages sent */
    uint      fast_dfs_buffers;	        /* Fast-dfs memd buffers sent */
    uint      fast_dfs_rx;		/* Fast-dfs ipc messages rcvd */
    uint      fast_dfs_rx_buffers;	/* Fast-dfs memd buffers rcvd */

    /*
     * Bad Statistics
     */

    uint      dropped;			/* Dropped IPC frames */
    uint      no_port;			/* Packets for invalid ports */
    uint      no_seat;			/* Packets for invalid seat */
    uint      no_delivery;		/* No queue or No callback installed */
    uint      dup_ack;			/* Duplicate ACK messages received */
    uint      retries;			/* Retry attempts */
    uint      timeouts;			/* Message timeouts */
    uint      ipc_output_fails;  	/* ipc_output returned bad sts */
    uint      message_too_big;          /* */
    uint      no_message_buffer;        /* message cache empty */
    uint      no_pak_buffer;            /* couldn't allocate message pak */
    uint      no_memd_buffer;  	        /* couldn't allocate memd buffer */
    uint      no_hwq;     	        /* slot doesn't have hwq configed */
    uint      no_port_name;    	        /* couldn't find control port name */
    uint      port_err;    	        /* couldn't find control port */

} ipc_global_info;

extern ipc_global_info *ipc_globals;


/*
 * Sequence numbers.  (Wrap to 0 from 0xffff)
 */

#define IPC_MAX_SEQUENCE 0x03ff
#define IPC_GRACE_SEQUENCE (IPC_MAX_SEQUENCE >> 1) 

/*
 * Misc enumerations
 */

typedef enum ipc_sequence_report_ {
    IS_CURRENT = 0,
    IS_FROM_PAST,
    IS_FROM_FUTURE,
} ipc_sequence_report;

/*
 * Retry Timers
 */

#define IPC_RETRY_MAX 5
#define IPC_RETRY_PERIOD (5*ONESEC)
#define IPC_MAINTENANCE_PERIOD (20*ONESEC)
#define IPC_RPC_DEFAULT_PERIOD (25*60*ONESEC) /* 25 minutes */

/*
 * Server message types
 * Maximum number for type is 0x02ff.
 */

#define IPC_TYPE_SERVER_ECHO 1
#define IPC_TYPE_REGISTER_NAME_REQUEST 2
#define IPC_TYPE_REGISTER_NAME_RESPONSE 3
#define IPC_TYPE_DEREGISTER_NAME_REQUEST 4
#define IPC_TYPE_LOOKUP_NAME_REQUEST 5

/*
 * PVC port message types
 * Maximum number for type is 0x03ff.
 */
#define IPC_TYPE_IP_CACHE 0x300		/* RSP download IP cache to VIP */
#define IPC_TYPE_IP_STATS 0x380		/* VIP send statistics to RSP */
#define IPC_TYPE_IP_DFSCFG 0x381      /* VIP request DFS config to RSP */
#define IPC_TYPE_IP_ACLCFG 0x382      /* VIP request ACL config to RSP */
#define IPC_TYPE_CRYPTO    0x383      /* VIP/RSP crypto request        */


/*
 * Bootstrap message types
 */

#define IPC_BOOTSTRAP_REGISTER_REQUEST_V1 1
#define IPC_BOOTSTRAP_REGISTER_RESPONSE_V1 2

typedef struct ipc_boot_request_v1 {
    ushort transport_type;
    ushort name_length;
    uchar name[0];
/*  transport specific data, if any, follows name */
} ipc_boot_request_v1;

typedef struct ipc_boot_response_v1 {
    ushort        error_code;
    ipc_seat      seat_id;
    ipc_port_id   master_control_port;
} ipc_boot_response_v1;

/*
 * Used to pass context to callback handler
 * via new process.
 */
typedef struct ipc_callback_arg_ {
    ipc_message *msg;
    ipc_port_data *port;
    ipc_error_code flag;
} ipc_callback_arg_t;

/*
 * Constructs for private IPC messages sent within
 * the same seat.
 */
typedef enum ipc_private_msg_type_ {
    SLOT_SEAT_INIT = 1,
} ipc_private_msg_type;

typedef struct ipc_private_msg_ {
    ipc_private_msg_type type;
    uchar 		 data[0];
} ipc_private_msg;

typedef struct ipc_slot_init_msg_ {
    ipc_private_msg header;
    ulong	    slot;
} ipc_slot_init_msg;

/*
 * Some string message defines.
 */
#define IPC_SLOT_STR        	"SLOT"
#define IPC_SLOT_STR_SIZE   	(sizeof(IPC_SLOT_STR) + 6)
#define IPC_STARTUP_MESSAGE 	"SLAVECORE_OPEN"
#define IPC_TERMINATOR_MESSAGE 	"SLAVECORE_END"
#define IPC_OIR_MESSAGE 	"EOIR_EVENT"

extern void ipc_process_raw_pak(paktype *pak);
extern boolean ipc_renumber_as_slave(ipc_seat new_seat_id);

#endif __IPC_PRIVATE_H__
