/* $Id: ipc_seats.c,v 3.3.44.3 1996/08/07 09:00:25 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ipc/ipc_seats.c,v $
 *------------------------------------------------------------------
 * ipc_seats.c - Code that manages IPC seats (processors.)
 *
 * March 1995, Jim Hayes
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipc_seats.c,v $
 * Revision 3.3.44.3  1996/08/07  09:00:25  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.3.44.2  1996/06/02  15:32:00  motto
 * CSCdi50208:  Issuing csna, llc or tcp CIP show cmd immed after CIP
 * reset fails
 * Branch: California_branch
 *
 * Revision 3.3.44.1  1996/04/03  22:48:47  jwelder
 * CSCdi47348:  lingering references to rvip
 * Branch: California_branch
 *
 * Revision 3.3  1995/12/07  22:29:15  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.2  1995/11/17  17:28:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:59:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:05:41  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/13  00:12:58  dlobete
 * CSCdi34151:  UDP transport non-operational
 *
 * Revision 2.1  1995/06/07  21:02:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../ip/ip.h"
#include "ipc_util.h"
#include "ipc.h"
#include "ipc_private.h"
#include "ipc_seats.h"
#include "ipc_ports.h"

thread_table *ipc_seat_table;

char *const ipc_transport_names[] = {
    "Local",
    "CIP-CX",
    "CIP-CY",
    "VIP-CX",
    "VIP-CY",
    "VIP2-CX",
    "VIP2-CY",
    "UDP",
    "ATM-VC",
};

/*
 * These routines are used by the hash table routines.
 */

static uint ipc_compute_seat_hash (void *item, int table_size)
{
    return ((ipc_seat_data *)item)->seat % table_size;
}

static int ipc_compare_seats (void *item1, void *item2)
{
    if (((ipc_seat_data *)item1)->seat ==
	((ipc_seat_data *)item2)->seat) return 0;

    if (((ipc_seat_data *)item1)->seat <
	((ipc_seat_data *)item2)->seat) return -1;

    return 1;
}

static void ipc_delete_seat (void *item)
{

    ipc_seat_data *the_seat;

    if (item == NULL) return;

    the_seat = (ipc_seat_data *)item;

    if (the_seat->name) {
	free(the_seat->name);
	the_seat->name = NULL;
    }

    ipc_close_ports_on_seat(the_seat->seat);

    free(the_seat);
}

/*
 * Create and initialize the seat table.
 */

boolean ipc_init_seat_table (void)
{
    ipc_seat_table = ipc_thread_create(IPC_SEAT_TABLE_SIZE,
				       "IPC Seats",
				       ipc_compare_seats,
				       ipc_compute_seat_hash,
				       ipc_delete_seat);

    if (ipc_seat_table == NULL) return TRUE;
    
    return FALSE;
}

void ipc_add_seat (ipc_seat_data *seat)
{
    ipc_thread_append(ipc_seat_table, seat);
}

ipc_seat_data *ipc_create_seat (ipc_seat seat_addr,
				char *seat_name)
{
    ipc_seat_data *the_seat;

    the_seat = malloc_named(sizeof(ipc_seat_data), "IPC Seat");

    if (the_seat == NULL) return NULL;

    the_seat->name = ipc_clone_string(seat_name);

    if (the_seat->name == NULL) {
	free(the_seat);
	return NULL;
    }

    the_seat->seat = seat_addr;
    the_seat->send_vector = NULL;
    the_seat->type = IPC_SEAT_LOCAL;
    the_seat->last_sent = 0;
    the_seat->last_heard = 0;

    return the_seat;
}

/*
 * Given a seat address, return the seat data structure associated with it
 */

ipc_seat_data *ipc_get_seat(ipc_seat seat_addr)
{
    ipc_seat_data template;

    template.seat = seat_addr;

    return ipc_thread_lookup(ipc_seat_table, &template);
}

/*
 * Given a seat address, return the seat name
 */

char *ipc_get_seat_name(ipc_seat seat_addr)
{
    ipc_seat_data *the_seat;

    the_seat = ipc_get_seat(seat_addr);

    if (the_seat == NULL) return NULL;

    return the_seat->name;
}

/*
 * Given a seat name, return the seat data structure associated with it.
 */

ipc_seat_data *ipc_get_seat_by_name (char *name)
{
    ipc_seat_data *the_seat;

    ipc_thread_forward_lookup(ipc_seat_table);

    while ((the_seat =
	    (ipc_seat_data *)ipc_thread_get_next(ipc_seat_table)) != NULL) {

	if (strcmp(the_seat->name, name) == 0) return the_seat;
    }
    return NULL;
}

/*
 * Assign a seat within the IPC system.
 *
 * Allocates all the memory it uses, so no need to worry about
 * passing in copies of the data.  We will copy it for you.
 *
 * "Please do not pull on the safety bar, I will lower it for you."
 *   - Haunted Mansion, Disneyland.
 *
 * If the seat name is already taken, don't assign the seat, return
 * IPC_SEAT_IN_USE in error code.
 * 
 */

ipc_seat_data *ipc_assign_seat (char *seat_name,
				ipc_error_code *error)
{
    ipc_seat_data *the_seat;
    ipc_seat the_seat_id;

    the_seat = ipc_get_seat_by_name(seat_name);

    if (the_seat != NULL) {
	*error = IPC_SEAT_IN_USE;
	return the_seat;
    }

    /*
     * Cool!  This seat isn't taken.  Let's snarf it up and make one!
     */

    the_seat_id = ipc_globals->last_seat_addr + IPC_SEAT_INCREMENT;

    the_seat = ipc_create_seat(the_seat_id, seat_name);

    if (the_seat == NULL) {
	*error = IPC_NO_MEMORY;
	return NULL;
    }

    *error = IPC_OK;
    return the_seat;
}

ipc_error_code ipc_unassign_seat (ipc_seat id)
{
    return IPC_OK;
}

ipc_seat_data *ipc_get_seat_info (void)
{
    return ipc_get_seat(ipc_globals->local_seat_addr);
}

/*
 * Change a seat's identification number and renumber it's ports
 * to use the new id and name.
 */

void ipc_renumber_seat(ipc_seat_data *old_seat, ipc_seat new_seat_id,
		       char *seat_name)
{
    ipc_thread_yank(ipc_seat_table, old_seat);

    free(old_seat->name);
    old_seat->seat = new_seat_id;
    old_seat->name = ipc_clone_string(seat_name);

    ipc_thread_prepend(ipc_seat_table, old_seat);

    ipc_renumber_as_slave(new_seat_id);
}

void ipc_reset_seat (ipc_seat_data *seat_data)
{
    if (seat_data) {
        seat_data->last_heard = 0;
        seat_data->last_sent  = 0;
    }
}

void ipc_show_seats (parseinfo *csb)
{
    ipc_seat_data *the_seat;
    int node_count;

    ipc_thread_forward_lookup(ipc_seat_table);
    node_count = ipc_thread_get_count(ipc_seat_table);

    automore_enable(NULL);

    printf("There %s %d node%s in this IPC realm.\n",
	   (node_count == 1)?"is":"are", node_count,
	   (node_count == 1)?"":"s");
   
    if (node_count == 0) {
	automore_disable();
	return;
    }

    automore_enable(" ID               Type                 Name\n");

    do {
	the_seat = ipc_thread_get_next(ipc_seat_table);
	
	if (the_seat) {
	    ipc_thread_lock(the_seat);
	    printf("%-8x 	    %10s %40s\n",
		   the_seat->seat,
		   ipc_transport_names[the_seat->type],
		   the_seat->name);
	    ipc_thread_unlock(ipc_seat_table,the_seat);
	}
    } while (the_seat != NULL);
    automore_disable();
}

