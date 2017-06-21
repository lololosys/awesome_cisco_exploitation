/* $Id: ipc_ports.h,v 3.2.60.1 1996/06/02 15:31:59 motto Exp $
 * $Source: /release/112/cvs/Xsys/ipc/ipc_ports.h,v $
 *------------------------------------------------------------------
 * ipc_ports.h - Header file for IPC communication endpoint mgmt.
 *
 * March 1995, Jim Hayes
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipc_ports.h,v $
 * Revision 3.2.60.1  1996/06/02  15:31:59  motto
 * CSCdi50208:  Issuing csna, llc or tcp CIP show cmd immed after CIP
 * reset fails
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:28:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:59:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:02:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
 
#ifndef __IPC_PORTS_H__
#define __IPC_PORTS_H__

extern thread_table *ipc_port_table;
extern thread_table *ipc_name_table;

typedef enum ipc_port_type {
    IPC_PORT_UNICAST = 1,
    IPC_PORT_MULTICAST,
} ipc_port_type;

typedef enum ipc_recieve_method {
    IPC_QUEUE=1,
    IPC_CALLBACK,
    IPC_FAST_CALLBACK
} ipc_receive_method;


#define IPC_PORT_NAME_MAX  64		/* Longest IPC name allowed */
#define IPC_PORT_NAME_SEPARATOR ':'	/* Separator between name fields */

/*
 * Port Table (Table key = port ID)
 */

struct ipc_port_data_ {
    thread_linkage links;
    ipc_port_id port;
    char *name;
    ipc_port_type type;
    ipc_receive_method method;

    void *receive_context;
    ipc_callback receive_callback;

    queuetype *receive_queue;
    ushort flags;
};


/*
 * NAME Entry.  (Table Key = ascii name)
 */

typedef struct ipc_name_ {
    thread_linkage links;
    char *name;
    ipc_port_id port;
} ipc_name;

/*
 * Message formats.
 */

#define IPC_CONTROL_PORT_NAME "Control"

typedef struct ipc_register_name_request_ {
    ipc_port_id   port_id;
    ushort        name_length;
    uchar         name[0];
} ipc_register_name_request;

typedef struct ipc_register_name_response_ {
    ushort        error_code;
    /* Maybe more data in the future. */
} ipc_register_name_response;

typedef struct ipc_deregister_name_request_ {
    ipc_port_id   port_id;
    /* Maybe more data in the future. */
} ipc_deregister_name_request;

typedef struct ipc_lookup_name_request_ {
    ushort        name_length;
    uchar         name[0];
} ipc_lookup_name_request;

typedef struct ipc_lookup_name_response_ {
    ipc_port_id   port_id;
} ipc_lookup_name_response;

/*
 * Constants
 */ 

#define IPC_PORT_TABLE_SIZE 97
#define IPC_NAME_TABLE_SIZE 97

#define IPC_PORT_UNICAST 0
#define IPC_PORT_MULTICAST 1

/*
 * Function Prototypes
 */

extern boolean ipc_init_port_table(void);
extern char *ipc_get_name_by_port(ipc_port_id port);
extern ipc_port_data *ipc_get_port(ipc_port_id port);
extern ipc_port_data *ipc_create_port_entry(char *name, ipc_port_id port);
extern void ipc_close_ports_on_seat(ipc_seat seat);

#endif __IPC_PORTS_H__
 
