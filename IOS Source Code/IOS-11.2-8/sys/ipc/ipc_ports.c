/* $Id: ipc_ports.c,v 3.5.6.4 1996/08/28 12:50:43 thille Exp $
 * $Source: /release/112/cvs/Xsys/ipc/ipc_ports.c,v $
 *------------------------------------------------------------------
 * ipc_ports.c - Code to managage IPC communication endpoints.
 *
 * March 1995, Jim Hayes
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipc_ports.c,v $
 * Revision 3.5.6.4  1996/08/28  12:50:43  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.6.3  1996/06/28  23:18:14  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.5.6.2  1996/06/02  15:31:58  motto
 * CSCdi50208:  Issuing csna, llc or tcp CIP show cmd immed after CIP
 * reset fails
 * Branch: California_branch
 *
 * Revision 3.5.6.1  1996/03/18  20:18:18  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.16.3  1996/03/16  07:00:29  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.16.2  1996/03/07  09:47:34  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.16.1  1996/02/20  14:26:50  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/03/04  07:12:56  kao
 * CSCdi49674:  While VIP is cofigured with DFS, OIR event will crash the
 * VIP;
 *
 * Revision 3.4  1996/02/18  19:41:47  kao
 * CSCdi45136:  EOIR Problems with VIP card
 *
 * Revision 3.3  1995/12/07  22:29:14  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.2  1995/11/17  17:28:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:59:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:05:38  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/13  00:12:56  dlobete
 * CSCdi34151:  UDP transport non-operational
 *
 * Revision 2.1  1995/06/07  21:02:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "logger.h"
#include "../ipc/msg_ipc.c"		/* Not a typo, see logger.h */
#include "ipc_util.h"
#include "ipc.h"
#include "ipc_private.h"
#include "ipc_ports.h"
#include "ipc_debug.h"

thread_table *ipc_port_table;		/* Ports by port id */
thread_table *ipc_name_table;		/* Ports by name */

/*
 * These routines are used by the hash table routines.
 */

static uint ipc_compute_port_hash (void *item, int table_size)
{
    return ((ipc_port_data *)item)->port % table_size;
}

static int ipc_compare_ports (void *item1, void *item2)
{
    if (((ipc_port_data *)item1)->port ==
	((ipc_port_data *)item2)->port) return 0;

    if (((ipc_port_data *)item1)->port <
	((ipc_port_data *)item2)->port) return -1;

    return 1;
}

static void ipc_delete_port (void *item)
{
    ipc_port_data *the_port;

    if (item == NULL) return;

    the_port = (ipc_port_data *)item;

    if (the_port->name) {
	free(the_port->name);
	the_port->name = NULL;
    }
    
    free(the_port);
}

static uint ipc_compute_port_name_hash (void *item, int table_size)
{
    return (ipc_thread_hash_str(((ipc_name *)item)->name) % table_size);
}

static int ipc_compare_port_names (void *item1, void *item2)
{

    int x;

    x = strcmp(((ipc_name *)item1)->name,
	       ((ipc_name *)item2)->name);

    if (x == 0) return 0;

    if (x < 0) return -1;

    return 1;
}

static void ipc_delete_port_name (void *item)
{
    ipc_name *the_name;
    
    if (item == NULL) return;

    the_name = (ipc_name *)item;

    if (the_name->name) {
	free(the_name->name);
	the_name->name = NULL;
    }
    
    free(the_name);
}

/*
 * Create the list of available ports.
 */

boolean ipc_init_port_table (void)
{
    ipc_port_table = ipc_thread_create(IPC_PORT_TABLE_SIZE,
				       "IPC Ports",
				       ipc_compare_ports,
				       ipc_compute_port_hash,
				       ipc_delete_port);

    if (ipc_port_table == NULL) return TRUE;

    ipc_name_table = ipc_thread_create(IPC_NAME_TABLE_SIZE,
				       "IPC Port Names",
				       ipc_compare_port_names,
				       ipc_compute_port_name_hash,
				       ipc_delete_port_name);

    if (ipc_name_table == NULL) {
	ipc_thread_destroy(ipc_port_table);
	return TRUE;
    }
    
    return FALSE;
}

/*
 * Given a port ID, return the port's name.
 */

char *ipc_get_name_by_port (ipc_port_id port)
{
    ipc_port_data template;
    ipc_port_data *result;

    template.port = port;

    result = ipc_thread_lookup(ipc_port_table, &template);

    if (result) return result->name;

    return NULL;
}

/*
 * Given a port ID, return the port data structure 
 */

ipc_port_data *ipc_get_port (ipc_port_id port)
{
    ipc_port_data template;

    template.port = port;

    return ipc_thread_lookup(ipc_port_table, &template);

}

/*
 * Give a port's name, return the port ID.
 * If the port name cannot be found in the local namespace,
 * the IPC Master is asked to resolve the name.
 *
 * If the IPC Master cannot resolve the name, 0 is returned.
 */

ipc_port_id ipc_locate_port (char *name)
{
    ipc_name template;
    ipc_port_id control_port;
    char control_port_name[IPC_PORT_NAME_MAX];
    ipc_name *result;

    ipc_lookup_name_request *request_data;
    ipc_lookup_name_response *response_data;
    ipc_message *request;
    ipc_message *response;
    
    ipc_error_code error;
    ipc_port_id remote_port;

    if (ipc_debug_ports)
        buginf("\nIPC: Looking up port for '%s'", name);

    /*
     * Try the local name first.  If that fails, ask the IPC master.
     */

    template.name = name;
    result = ipc_thread_lookup(ipc_name_table, &template);
    if (result) return result->port;

    if (ipc_globals->is_ipc_master) return 0;

    /*
     * Well, no local port has this name, let's ask big-brother upstairs.
     */

    if (ipc_debug_ports)
	buginf("\nIPC: Local lookup failed on '%s' trying %s.",
	       name, IPC_MASTER_NAME);

    sprintf(control_port_name,"%s%c%s", IPC_MASTER_NAME,
	    IPC_PORT_NAME_SEPARATOR, IPC_CONTROL_PORT_NAME);

    template.name = control_port_name;
    result = ipc_thread_lookup(ipc_name_table, &template);

    if (!result) return 0;

    control_port = result->port;

    request = ipc_get_message(sizeof(ipc_lookup_name_request) + strlen(name),
			      control_port,
			      IPC_TYPE_LOOKUP_NAME_REQUEST);

    if (request == NULL) return 0;

    request_data = request->data;

    request_data->name_length = strlen(name);
    bcopy(name, request_data->name, strlen(name));
    
    response = ipc_send_rpc(request, &error);

    if (error != IPC_OK) return 0;

    response_data = response->data;

    remote_port = response_data->port_id;

    ipc_return_message(response);

    if (ipc_debug_ports)
	buginf("\nIPC: Remote lookup %s, port=%d.",
	       remote_port?"passed":"failed", remote_port);

    return remote_port;
}

void ipc_show_ports (parseinfo *csb)
{
    ipc_port_data *the_port;
    uint port_count;
    char *type;
    const int name_len = 40;

    char port_name[name_len+1];

    port_count = ipc_thread_get_count(ipc_port_table);
    
    automore_enable(NULL);

    printf("There %s %d port%s defined.\n",
	   (port_count == 1)?"is":"are",
	   port_count,
	   (port_count == 1)?"":"s");

    if (port_count) {

	automore_enable("Port ID     Type       Name\n");

	ipc_thread_forward_lookup(ipc_port_table);

	while ( (the_port = ipc_thread_get_next(ipc_port_table)) != NULL) {

	    switch(the_port->type) {
	        case IPC_PORT_UNICAST: type = "unicast"; break;
	        case IPC_PORT_MULTICAST: type = "multicast"; break;
	        default: type = "unknown"; break;
	    }

	    ipc_chop_copy(the_port->name, port_name, name_len);
	    
	    printf("%-8x.%5x %10s %40s\n",
		   ipc_portid_to_seat(the_port->port),
		   ipc_portid_to_port(the_port->port),
		   type,
		   port_name);
	}
    }
    automore_disable();
}

/*
 * Create a port entry out of thin air, add it to the port table
 * and return it.
 */

ipc_port_data *ipc_create_port_entry (char *name, ipc_port_id port)
{
    ipc_port_data *the_port;
    ipc_name *the_name;
    char *cloned_name;

    the_port = malloc_named(sizeof(ipc_port_data), "IPC Port");
    if (the_port == NULL) return NULL;

    the_name = malloc_named(sizeof(ipc_name), "IPC Name");
    if (the_name == NULL) {
	free(the_port);
	return NULL;
    }

    cloned_name = ipc_clone_string(name);
    if (cloned_name == NULL) {
	free(the_port);
	free(the_name);
	return NULL;
    }

    the_port->port = port;
    the_port->name = cloned_name;
    the_port->type = IPC_PORT_UNICAST;

    the_name->port = port;
    the_name->name = cloned_name;

    ipc_thread_append(ipc_port_table, the_port);
    ipc_thread_append(ipc_name_table, the_name);

    if (ipc_debug_ports)
	buginf("\nIPC: Created named port %d (%s)",
	       the_port->port, the_port->name);


    return the_port;
}

/*
 * Create a port in the local namespace and return the port id.
 */

ipc_error_code ipc_create_named_port (char *name, ipc_port_id *port, 
							ushort flags)
{
    ipc_port_data *the_port;
    ipc_name *the_name, template;

    template.name = name;
    the_name = ipc_thread_lookup(ipc_name_table, &template);

    if (the_name != NULL) return IPC_PORT_IN_USE;

    /* Create the unique local port identifier */

    *port = ipc_make_portid(ipc_globals->local_seat_addr, ++ipc_globals->last_port);

    /* Create the database entries for mapping ports <-> names. */
    
    the_port = ipc_create_port_entry(name, *port);

    if (the_port == NULL) return IPC_NO_MEMORY;

    the_port->flags |= flags;

    the_port->flags |= IPC_PORT_IS_LOCAL;

    return IPC_OK;
}

/*
 * Register this port with the IPC Master server
 */

ipc_error_code ipc_register_port (ipc_port_id port)
{
    ipc_port_data *the_port;
    ipc_port_id control_port;
    ipc_error_code error;

    ipc_message *request;
    ipc_message *response;

    int message_size;

    ipc_register_name_request *request_data;
    ipc_register_name_response *response_data;


    /*
     * Fetch the port we wish to register
     */

    the_port = ipc_get_port(port);
    if (the_port == NULL) return IPC_NO_SUCH_PORT;

    if (ipc_debug_ports)
	buginf("\nIPC: Register port %d (%s)", the_port->port, the_port->name);

    /*
     * If we're already the IPC master, don't bother. We already know.
     */

    if (ipc_globals->is_ipc_master) {
	the_port->flags &= (~IPC_PORT_IS_LOCAL);
	return IPC_OK;
    }

    /*
     * Find the port id of the IPC_Master's control port.
     */
    control_port = ipc_globals->zone_manager_pid;

    if (control_port == 0) return IPC_INTERNAL;

    /*
     * Allocate and stuff the message with the registration data
     */

    message_size = sizeof(ipc_register_name_request) + strlen(the_port->name);

    request = ipc_get_message(message_size, control_port,
			      IPC_TYPE_REGISTER_NAME_REQUEST);

    if (request == NULL) return IPC_NO_MEMORY;

    request_data = request->data;

    request_data->port_id = port;
    request_data->name_length = strlen(the_port->name);
    bcopy(the_port->name, request_data->name, request_data->name_length);

    /*
     * Send the RPC request to the server.
     */

    response = ipc_send_rpc(request, &error);
    if (error != IPC_OK) return error;

    response_data = response->data;
 
    error = response_data->error_code;
    if (error == IPC_OK)
        the_port->flags &= (~IPC_PORT_IS_LOCAL);
 
    ipc_return_message(response);
 
    return(error);
}

/*
 * Close this port.  If it was registered with the IPC master server,
 * notify the master server that it's going away too.
 */

ipc_error_code ipc_close_port (ipc_port_id port)
{
    ipc_port_data *port_data;
    ipc_name *name_data, name_template;

    ipc_message *close_request;
    ipc_deregister_name_request *close_data;
    ipc_port_id control_port;
    ipc_error_code error;
    leveltype level;

    if (onintstack()) {
	errmsg(&msgsym(ONINT,IPC),"ipc_close_port() ");
	return (IPC_PROCESS_LEVEL_ONLY);
    }
    port_data = ipc_get_port(port);
    if (port_data == NULL) return IPC_NO_SUCH_PORT;

    if (ipc_debug_ports)
	buginf("\nIPC: Close port request for port %d (%s).",
	       port_data->port, port_data->name);

    name_template.name = port_data->name;
    name_data = ipc_thread_lookup(ipc_name_table, &name_template);

    if (name_data == NULL) {
	errmsg(&msgsym(CONSISTENCY,IPC),"ipc_close_port: missing name.");
	return IPC_NO_SUCH_PORT;
    }

    level = raise_interrupt_level(NETS_DISABLE);
     ipc_thread_yank(ipc_port_table, port_data);
     ipc_thread_yank(ipc_name_table, name_data);
    reset_interrupt_level(level);

    error = IPC_OK;

    if (!(port_data->flags & IPC_PORT_IS_LOCAL) &&
	!(ipc_globals->is_ipc_master)) {

	/*
	 * Port isn't local, so let's deregister it from the master server.
	 * If we're the master server, don't bother.
	 */
	control_port = ipc_globals->zone_manager_pid;

	if (control_port == 0) return IPC_INTERNAL;
	
	close_request = ipc_get_message(sizeof(ipc_deregister_name_request),
					control_port,
					IPC_TYPE_DEREGISTER_NAME_REQUEST);

	if (close_request == NULL) return IPC_NO_MEMORY;
	
	close_data = close_request->data;
	close_data->port_id = port_data->port;
	
	error = ipc_send_message(close_request);
    }

    ipc_delete_port(port_data);
    name_data->name = NULL;
    ipc_delete_port_name(name_data);
    return error;
}

void ipc_close_ports_on_seat (ipc_seat seat) {
    ipc_port_data *the_port;
    uint port_count;
    
    if (onintstack()) {
 	errmsg(&msgsym(ONINT,IPC),"ipc_close_ports_on_seat() ");
 	return;
    }

    port_count = ipc_thread_get_count(ipc_port_table);
    if (port_count) {
	ipc_thread_forward_lookup(ipc_port_table);
	while ((the_port = ipc_thread_get_next(ipc_port_table)) != NULL) {
            if (seat == ipc_portid_to_seat(the_port->port)) {
                ipc_close_port(the_port->port);
            }
	}
    }
}

ipc_error_code ipc_create_multicast_port (char *name, ipc_port *port)
{
    return IPC_OK;
}

ipc_error_code ipc_subscribe_multicast_port (ipc_port_id port,
					     ipc_port_id new_subscriber_port)
{
    return IPC_OK;
}

ipc_error_code ipc_unsubscribe_multicast_port (ipc_port_id port,
					       ipc_port_id old_subscriber_port)
{
    return IPC_OK;

}
ipc_error_code ipc_set_receive_callback (ipc_port_id port,
					 void *callback_context,
					 ipc_callback callback_routine)
{
    ipc_port_data *the_port;

    the_port = ipc_get_port(port);
    
    if (the_port == NULL) {
	ipc_error_debug("set callback", IPC_NO_SUCH_PORT);
	return IPC_NO_SUCH_PORT;
    }

    the_port->method = IPC_CALLBACK;
    the_port->receive_callback = callback_routine;
    the_port->receive_context = callback_context;

    return IPC_OK;
}

ipc_error_code ipc_set_receive_fast_callback (ipc_port_id port,
					      void *callback_context,
					      ipc_callback callback_routine)
{
    ipc_port_data *the_port;

    the_port = ipc_get_port(port);
    
    if (the_port == NULL) {
	ipc_error_debug("set fast callback", IPC_NO_SUCH_PORT);
	return IPC_NO_SUCH_PORT;
    }

    the_port->method = IPC_FAST_CALLBACK;
    the_port->receive_callback = callback_routine;
    the_port->receive_context = callback_context;

    return IPC_OK;
}

ipc_error_code ipc_set_receive_queue (ipc_port_id port,
				      queuetype *receive_queue)
{
    ipc_port_data *the_port;

    the_port = ipc_get_port(port);
    
    if (the_port == NULL) {
	ipc_error_debug("set receive queue", IPC_NO_SUCH_PORT);
	return IPC_NO_SUCH_PORT;
    }

    the_port->method = IPC_QUEUE;
    the_port->receive_queue = receive_queue;

    return IPC_OK;
}

/*
 * ipc_port_repair_name:
 *
 * If given a port name, strip away it's old hostname (beginning to first ':')
 * and change it to the current host name.
 *
 * Returns the new host name (in a malloc'd block) and free's the old name.
 */

static char *ipc_port_repair_name (char *old_name)
{
    char *tail_end;
    char new_name[IPC_PORT_NAME_MAX];

    /*
     * Find the next character colon
     */

    tail_end = strchr(old_name, IPC_PORT_NAME_SEPARATOR) + 1;
    
    if (tail_end == NULL) return old_name;

    sprintf(new_name,"%s:%s", hostname, tail_end);

    tail_end = malloc(strlen(new_name) + 1);

    if (tail_end == NULL) return old_name;

    strcpy(tail_end, new_name);
    free(old_name);

    return tail_end;
}

/*
 * Renumber an IPC port to a new port ID.  If the port ID is no longer
 * an IPC master, renumber the name as well.
 */

static boolean ipc_renumber_port (ipc_port_data *old_port, ipc_port_id new_port_id)
{
    ipc_name template;
    ipc_name *the_name;
    char *new_name;

    template.name = old_port->name;
    the_name = ipc_thread_lookup(ipc_name_table, &template);

    if (the_name == NULL)
	return TRUE;
    
    ipc_thread_yank(ipc_name_table, the_name);

    /*
     * If the name begins with "IPC Master:" and we're NOT the IPC master
     * anymore, we'll rename it to the current hostname.
     */

    if (strncmp(old_port->name, IPC_MASTER_NAME,
		strlen(IPC_MASTER_NAME)) == 0) {

	if (ipc_portid_to_seat(new_port_id) != IPC_MASTER_SEAT) {
	    new_name = ipc_port_repair_name(old_port->name);

	    old_port->name = new_name;
	    the_name->name = new_name;
	}
    }

    old_port->port = new_port_id;
    the_name->port = new_port_id;

    ipc_thread_append(ipc_name_table, the_name);

    return FALSE;
}

/*
 * ipc_renumber_as_slave:
 *
 * We've officially become an IPC slave so we need to renumber our
 * local ports to the new "seat" address and change the names of any
 * ports that might have claimed themselves "IPC Master" in the past.
 */

boolean ipc_renumber_as_slave (ipc_seat new_seat_id)
{
    ipc_port_data *the_port;
    ipc_port_data *first_port;
    ipc_port_id new_port_id;
    boolean failure;


    if (ipc_debug_events)
	buginf("\nIPC: Renumbering slave's local ports with new seat = %x", 
						new_seat_id);

    the_port = NULL;   /* Compiler warns about possible unused variable. */

    first_port = ipc_thread_get_head(ipc_port_table);

    /*
     * When MEMD recarve, seat id is the same, don't need to change
     */
    if (ipc_portid_to_seat(first_port->port) == new_seat_id)
        return FALSE;

    /*
     * Loop through all the ports in the port table looking for
     * ports that aren't converted to the new seat id.
     */
    

    
    do {
	the_port = ipc_thread_yank_head(ipc_port_table);

	if (ipc_debug_events)
	    buginf("\nIPC: port from port_table= %x", the_port->port);

	if (ipc_portid_to_seat(the_port->port) != new_seat_id) {
	    new_port_id = ipc_make_portid(new_seat_id,
					  ipc_portid_to_port(the_port->port));
	    failure = ipc_renumber_port(the_port, new_port_id);
	    ipc_thread_append(ipc_port_table, the_port);

	    if (failure) {
		if (ipc_debug_ports) {
		    buginf("\nIPC: ipc_renumber_as_slave: Couldn't "
			   "renumber IPC ports.");
		}
		return TRUE;
	    }
	}

	/*
	 * Get the next port.  If it's the same as the first one we looked at
	 * we're done.
	 */


    } while (ipc_thread_get_head(ipc_port_table) != first_port);

    return FALSE;
}

