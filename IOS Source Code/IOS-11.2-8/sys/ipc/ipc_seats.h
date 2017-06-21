/* $Id: ipc_seats.h,v 3.3.6.2 1996/08/07 09:00:26 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ipc/ipc_seats.h,v $
 *------------------------------------------------------------------
 * ipc_seats.h - Header file for IPC seat (processor) management
 *
 * March 1995, Jim Hayes
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipc_seats.h,v $
 * Revision 3.3.6.2  1996/08/07  09:00:26  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.3.6.1  1996/04/03  22:48:49  jwelder
 * CSCdi47348:  lingering references to rvip
 * Branch: California_branch
 *
 * Revision 3.3  1996/03/04  07:12:59  kao
 * CSCdi49674:  While VIP is cofigured with DFS, OIR event will crash the
 * VIP;
 *
 * Revision 3.2  1995/11/17  17:28:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:00:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:05:42  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/09/17  20:51:45  motto
 * CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
 *
 * Revision 2.1  1995/06/07  21:02:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IPC_SEATS_H__
#define __IPC_SEATS_H__

/*
 * Here are the anticipated transport types that may be used by the IPC
 * for future projects.  Please be sure to update "ipc_transport_names[]" 
 * in ipc_seats.c if you update this list.
 */

typedef enum ipc_transport_type_ {
    IPC_SEAT_LOCAL=0,
    IPC_SEAT_CXBUS_CIP,
    IPC_SEAT_CYBUS_CIP,
    IPC_SEAT_CXBUS_VIP,
    IPC_SEAT_CYBUS_VIP,
    IPC_SEAT_CXBUS_VIP2,
    IPC_SEAT_CYBUS_VIP2,
    IPC_SEAT_CYBUS_RSP,
    IPC_SEAT_UDP,
    IPC_SEAT_ATM_VC,
} ipc_transport_type;

extern char *const ipc_transport_names[];
    
/*
 * These structures detail the various transport types 
 * that the IPC can use.  When registering the seat,
 * the registering user fills in some parameters, and the
 * the seat registration code fills in the rest.
 *
 * This is still pretty much up-in-the-air on how it will actually work.
 */

/*
 * CBUS IPC Transport
 */

typedef struct ipc_transport_cbus {
    
    /* Provided by user: */

    uint slot_number;			/* Which physical slot? */
    uint bus_number;			/* Which bus?, 0 = default */

} ipc_transport_cbus;

/*
 * UDP IPC Transport
 */ 

typedef struct ipc_transport_udp_ {

    /* Provided by user: */

    ipaddrtype dest_udp_addr;		/* Where we're going. */

    /* Filled in by IPC mechanism: */

    ipsocktype *socket;

} ipc_transport_udp;


/*
 * The MASTER transport structure built for each seat.
 */


/*
 * IPC Transmit vector:
 *
 * This vector is called whenever data needs to be transmitted to
 * the remote seat described below. If the message is successfully
 * encapsulated and sent via the transpart, FALSE is returned.
 *
 * If an error occurs and the message cannot be sent, return TRUE.
 * 
 * This routine should never, ever, block.
 * This routine should never consume the message or paktype structure
 * inside-- it may be reused by the server.
 */

STRUCTURE_REF(ipc_seat_data);

typedef boolean (*ipc_send_vector)(ipc_message *, ipc_seat_data *);

/*
 * SEAT TREE (Table key = 16 bit seat address)
 */

typedef union ipc_transport_ {
	ipc_transport_cbus cbus;
	ipc_transport_udp udp;
	void *ipc_rsp_transport;	/* pointer to RSP transport structure */
} ipc_transport;

struct ipc_seat_data_ {
    thread_linkage links;
    ipc_seat seat;			/* The seat address */
    char *name;
    ipc_transport_type type;		/* Method used to get there */
    ipc_send_vector send_vector;
    boolean interrupt_ok;
    ipc_transport transport;

    ipc_sequence last_sent;		/* Last sequence number assigned */
    ipc_sequence last_heard;		/* Last valid sequence number heard
					   from this seat */
};

/*
 * Constants
 */ 

#define IPC_SEAT_TABLE_SIZE 43		/* Nice prime number for hash table */
#define IPC_SEAT_NAME_MAX 64		/* Supports even the most A-R users */
#define IPC_SIZEOF_FUNC_ARRAY 20	/* Number of delayed functions */

typedef struct ipc_platform_info_ {
    boolean platform_initialized;
    boolean do_delay_initialize;
    ipc_send_vector platform_tx_vector;     /* transmit to CBUS complex */
    ipc_transport_type type;		    /* Method used to get there */
    ipc_transport transport;
    ipc_init_slaves_vector platform_init_slaves_vector; /* init slaves */
    ipc_port_id master_control_pid;
    ipc_seat platform_seat;
    char * platform_seat_name;
    ipc_permanent_port_process_vector platform_permanent_port_process_vector;
} ipc_platform_info;

extern ipc_platform_info ipc_platform_struct;

/*
 * Function Prototypes
 */

extern boolean ipc_init_seat_table(void);

extern ipc_seat_data *ipc_create_seat(ipc_seat seat_addr,
				      char *seat_name);

extern boolean ipc_add_named_seat(uchar *seat_name, ipc_seat new_seat,
                                        ipc_transport_type transport,
                                        ipc_send_vector send_routine,
                                        uint slot);

extern ipc_seat_data *ipc_assign_seat(char *seat_name,
				      ipc_error_code *error);

extern ipc_error_code ipc_unassign_seat(ipc_seat id);

extern void ipc_add_seat(ipc_seat_data *seat);

extern char *ipc_get_seat_name(ipc_seat seat_addr);

extern ipc_seat_data *ipc_get_seat_info(void);
extern ipc_seat_data *ipc_get_seat(ipc_seat seat_addr);

extern ipc_seat_data *ipc_get_seat_by_name(char *name);

extern void ipc_renumber_seat(ipc_seat_data *old_seat, ipc_seat new_seat_id,
			      char *seat_name);

extern void ipc_reset_seat(ipc_seat_data *seat_data);

extern void ipc_show_seats(parseinfo *csb);

#endif __IPC_SEATS_H__
