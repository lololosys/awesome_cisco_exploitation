/* $Id: ipc_message.c,v 3.7.10.3 1996/08/28 12:50:41 thille Exp $
 * $Source: /release/112/cvs/Xsys/ipc/ipc_message.c,v $
 *------------------------------------------------------------------
 * ipc_message.c - Code to manage creation and deletion of ipc msgs.
 *
 * March 1995, Jim Hayes
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipc_message.c,v $
 * Revision 3.7.10.3  1996/08/28  12:50:41  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.10.2  1996/07/23  13:24:44  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.7.10.1  1996/03/18  20:18:17  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.12.2  1996/03/16  07:00:27  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.12.1  1996/03/07  09:47:32  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7  1996/02/24  02:13:16  dkerr
 * CSCdi49787:  IPC rolls over and dies under load
 * Make IPC more robust in an RSP->VIP distributed switching environment
 * (and increase performance from the miserly 100 msgs per minute to
 * something respectable, like 300K msgs per second).
 *
 * Revision 3.6  1996/02/23  10:03:51  feisal
 * CSCdi49009:  Memory leak in IPC under heavy load e.g. DFS
 *
 * Revision 3.5  1996/01/05  23:38:59  mbandi
 * CSCdi44416:  IPC RPC timeout is too long - Router hangs.
 * Default RPC time out is too long, changed to 25sec.
 *
 * Revision 3.4  1995/12/07  22:29:12  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.3  1995/11/17  17:28:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:41:39  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:59:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:05:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/07/18  06:46:34  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.3  1995/07/13 00:12:55  dlobete
 * CSCdi34151:  UDP transport non-operational
 *
 * Revision 2.2  1995/07/12  22:27:08  dlobete
 * CSCdi34149:  Cleanup slave applications
 *
 * Revision 2.1  1995/06/07  21:02:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
 
#include "master.h"
#include "logger.h"
#include "../ipc/msg_ipc.c"		/* Not a typo, see logger.h */
#include "config.h"
#include "parser.h"
#include "packet.h"

#include "ipc_util.h"
#include "ipc.h"
#include "ipc_private.h"
#include "ipc_message.h"
#include "ipc_ports.h"
#include "ipc_seats.h"
#include "ipc_debug.h"

/*
 * Local variables
 */

thread_table *ipc_message_table;
thread_table *ipc_rpc_req_table;
ipc_message **ipc_message_cache;
int ipc_message_cache_index;

/*
 * These routines are used by the hash table routines.
 */

static uint ipc_compute_message_hash (void *item, int table_size)
{
    return ((ipc_message *)item)->sequence % table_size;
}

static int ipc_compare_messages (void *item1, void *item2)
{
    if (((ipc_message *)item1)->sequence ==
	((ipc_message *)item2)->sequence) return 0;

    if (((ipc_message *)item1)->sequence <
	((ipc_message *)item2)->sequence) return -1;

    return 1;
}

static void ipc_delete_message (void *item)
{
    ipc_return_message((ipc_message *)item);
}

/*
 * Find the message in the retransmit queue based on its
 * sequence number.
 */

ipc_message *ipc_get_message_by_sequence (thread_table *ipc_thread,
	uint sequence)
{
    ipc_message template;

    template.sequence = sequence;

    return ipc_thread_lookup(ipc_thread, &template);
}

/*
 * Show all the message fields.  Suitable for debugging.
 */

void ipc_dump_message (ipc_message *message)
{
    ipc_message_header *header;
    int i;

    if (message == NULL) {
	buginf("\n   Null message passed to ipc_dump_message");
	return;
    }

    buginf("\n     control: ");
    
    if (message->sequence != 0) {
	buginf("flags=<%s>, retries=%d, seq_key=0x%x, ",
	       message->flags & IPC_FLAG_BLOCKED ? "BLOCKED":"DONE",
	       message->retries,
	       message->sequence);
	
	if (!TIMER_RUNNING(message->retry_time)) {
	    buginf("no retry set");
	} else {
	    buginf("retry in %TF", message->retry_time);
	}

	buginf("\n              ");

	if (message->notify_callback) 
	    buginf("notify routine=0x%x, notify context=0x%x, ",
		   message->notify_callback, message->notify_context);
	else
	    buginf("no notification set, ");
	
	buginf("\n              ");
    }	

    if (message->pak)
	buginf("pak=0x%x", message->pak);
    else
	buginf("no pak pointer set");

    header = message->header;
    
    buginf(", header=0x%x, data=0x%x",
	   header, message->data);

    buginf("\n     message: flags=<");

    if (header->flags & IPC_FLAG_ACK)
	buginf("ACK");
    else
	buginf("DATA");

    if (header->flags & IPC_FLAG_IS_FRAGMENT)
	buginf(" FRAG");

    if (header->flags & IPC_FLAG_IS_RPC)
	buginf(" RPC-Q");

    if (header->flags & IPC_FLAG_IS_RPC_REPLY)
	buginf(" RPC-R");

    if (header->flags & IPC_FLAG_DO_NOT_ACK)
	buginf(" DO-NOT-ACK");

    buginf(">, source=%d.%d, dest=%d.%d, size=%d,\n",
	   ipc_portid_to_seat(header->source_port),
	   ipc_portid_to_port(header->source_port),
	   ipc_portid_to_seat(header->dest_port),
	   ipc_portid_to_port(header->dest_port),
	   header->size);

    buginf("              sequence=%d, type=%d.", header->sequence,
	   header->type);

    if (header->size && header->data) {
	buginf("\n        data: ");
	for(i = 0; i< min(20,header->size); i++)
	    buginf("%2x ", *(header->data+i));
    }
}


/*
 * Return a message element from the message element cache, or
 * null if the cache is empty.
 */

ipc_message *ipc_get_message_from_cache(void)
{
    leveltype status;

    ipc_message *message;

    status = raise_interrupt_level(NETS_DISABLE);

    if (ipc_message_cache_index == 0) {
        reset_interrupt_level(status);
	if (ipc_debug_events) {
	    buginf("\nIPC: couldn't get cached message block, index = 0.");
	}
        return NULL;
    }
    
    message = ipc_message_cache[--ipc_message_cache_index];

    reset_interrupt_level(status);

    if (message == NULL) {
	if (ipc_debug_events) {
	    buginf("\nIPC: couldn't get cached message block.");
	}
	return NULL;
    }

    message->flags = 0;
    return message;
}


/*
 * Put an element back in the cach.  Scream if it's already there.
 */

void ipc_return_message_to_cache (ipc_message *message)
{
    leveltype status;

    if (message->flags & IPC_FLAG_IN_CACHE) {
	errmsg(&msgsym(DELETED,IPC), message);
	return;
    }

    message->links.next = NULL;
    message->links.prev = NULL;
    message->links.chain = NULL;

    message->header = NULL;
    message->data = NULL;
    message->port_p = NULL;
    TIMER_STOP(message->retry_time);
    message->flags = IPC_FLAG_IN_CACHE;
    message->result = IPC_OK;
    message->sequence = 0;
    message->retries = 0;
    message->pak = NULL;
    message->data = NULL;
    message->notify_callback = NULL;
    message->notify_context = NULL;

    status = raise_interrupt_level(NETS_DISABLE);
    
    ipc_message_cache[ipc_message_cache_index++] = message;

    reset_interrupt_level(status);


    return;
}

boolean ipc_init_message_system (void)
{
    int i;
    ipc_message *message;
    
    /* Initialize the table */

    ipc_message_table = ipc_thread_create(IPC_MESSAGE_TABLE_SIZE,
					  "IPC Message Elements",
					  ipc_compare_messages,
					  ipc_compute_message_hash,
					  ipc_delete_message);

    ipc_rpc_req_table = ipc_thread_create(IPC_RPC_REQ_TABLE_SIZE,
					  "IPC RPC Req Elements",
					  ipc_compare_messages,
					  ipc_compute_message_hash,
					  ipc_delete_message);

    if (ipc_message_table == NULL) return TRUE;

    /* Initialize our queue/list/hash element cache */

    ipc_message_cache = malloc_named(sizeof(ipc_message *) *
				     IPC_MESSAGE_CACHE_SIZE, "IPC Msg Cache");

    if (ipc_message_cache == NULL) {
	ipc_thread_destroy(ipc_message_table);
	return TRUE;
    }

    /* Initialize our cache elements */

    ipc_message_cache_index = 0;

    for (i = 0; i < IPC_MESSAGE_CACHE_SIZE; i++) {
	message = malloc_named(sizeof(ipc_message), "IPC Message");
	ipc_return_message_to_cache(message);
    }

    ipc_message_cache_index = IPC_MESSAGE_CACHE_SIZE;

    return FALSE;
}


/*
 * Get an ipc message element and a pak of the appropriate size, hook
 * them together, initialize the header fields and return it to the user.
 */
ipc_message *ipc_get_message (ipc_size size,
			     ipc_port_id dest_port,
			     ipc_message_type type)
{
    paktype *new_pak;
    ipc_message *new_message;
    ipc_message_header *header;

    /* Is the size reasonable? */

    if (size + IPC_MESSAGE_ENCAPS_SIZE > IPC_MESSAGE_MAX_MTU) {
	if (ipc_debug_events) {
	    buginf("\nIPC: get_buffer: size exceeds maximum IPC MTU.");
	} 
	return NULL;
    }

    /* Fetch a message block from our cache */

    new_message = ipc_get_message_from_cache();
    if (new_message == NULL) {
	ipc_globals->no_message_buffer++;
	return NULL;
    }

    /* Fetch a packet from the buffer pools. */

    new_pak = getbuffer(size + IPC_MESSAGE_ENCAPS_SIZE);

    if (new_pak == NULL) {
	ipc_return_message_to_cache(new_message);
	ipc_globals->no_pak_buffer++;
	return NULL;
    }

    /* do the usual cisco stuff */
    new_pak->encsize = IPC_MESSAGE_ENCAPS_SIZE;
    new_pak->datagramsize = new_pak->encsize + size;
    new_pak->datagramstart = pak_center(new_pak) - new_pak->encsize;

    /* Hook them together */

    header = (ipc_message_header *) new_pak->datagramstart;

    new_message->header = header;
    new_message->data = ipcdatastart(new_pak);
    new_message->pak = new_pak;
    new_message->rpc_timeout = IPC_RPC_DEFAULT_PERIOD;

    header->source_port = ipc_make_portid(ipc_globals->local_seat_addr,0);
    header->dest_port = dest_port;
    header->sequence = 0;
    header->type = type;
    header->size = size;
    header->flags = 0;

    return new_message;
}

void ipc_set_rpc_timeout (ipc_message *message, int seconds)
{
    if (message)
	message->rpc_timeout = seconds*ONESEC;
}


/*
 * Take an inbound packet, set up all the appropriate message fields
 * (data, header, backpointers, etc.) and return a completed message
 * to the caller for further processing.
 */

ipc_message *ipc_make_message_from_pak(paktype *pak)
{
    ipc_message *new_message;

    if (pak == NULL)
        return NULL;

    new_message = ipc_get_message_from_cache();

    if (new_message == NULL) return NULL;

    new_message->header = (ipc_message_header *)ipcheadstart(pak);
    new_message->data = ipcdatastart(pak);
    new_message->pak = pak;
    new_message->rpc_timeout = IPC_RPC_DEFAULT_PERIOD;

    new_message->port_p = NULL;
    new_message->notify_callback = NULL;
    new_message->notify_context = NULL;

    return new_message;
}

void ipc_return_message(ipc_message *message)
{
    if (message->pak == NULL) {
	if (message->data == NULL) {
	    errmsg(&msgsym(CONSISTENCY,IPC),"message pak & data == NULL");
	    return;
	}
    } else {
	datagram_done(message->pak);
    }

    ipc_return_message_to_cache(message);
}

#define NAME_WIDTH 20

void ipc_show_message_queue (parseinfo *csb)
{
    int in_queue, elements;
    ipc_message *message;
    leveltype status;
    
    uint sequence;
    char *source_port_name;
    char chopped_source_port_name[NAME_WIDTH];
    ipc_port_id source_port;
    
    char *dest_port_name;
    char chopped_dest_port_name[NAME_WIDTH];
    ipc_port_id dest_port;

    ipc_size size;
    int tries;

    in_queue = ipc_thread_get_count(ipc_message_table);

    automore_enable(NULL);

    printf("There %s %d IPC message%s waiting for acknowledgement"
	   " in the transmit queue.\n",
	   (in_queue == 1)?"is":"are", in_queue,
	   (in_queue == 1)?"":"s");


    elements = IPC_MESSAGE_CACHE_SIZE - ipc_message_cache_index;

    printf("There %s %d message%s currently in use by the system.\n",
	   (elements == 1)?"is":"are", elements,
	   (elements == 1)?"":"s");

    if (ipc_thread_get_head(ipc_message_table) == NULL) {
	automore_disable();
	return;
    }

    /* Scan the list and print out stuff about each message entry. */

    automore_enable("Source                           "
		    "Destination                      "
		    "Size  Tries\n");

    ipc_thread_forward_lookup(ipc_message_table);

    do {
	status = raise_interrupt_level(NETS_DISABLE);
	message=ipc_thread_get_next(ipc_message_table);

	if (message) {
	    sequence = message->sequence;

	    source_port = message->header->source_port;

	    if (ipc_portid_to_port(source_port) == 0) {
		source_port_name = 
		    ipc_get_seat_name(ipc_portid_to_seat(source_port));
	    } else {
		source_port_name = ipc_get_name_by_port(source_port);
	    }

	    if (source_port_name == NULL) {
		source_port_name = "Unknown";
	    }

	    ipc_chop_copy(source_port_name,
			  chopped_source_port_name, NAME_WIDTH);

	    dest_port = message->header->dest_port;

	    if (ipc_portid_to_port(dest_port) == 0) {
		dest_port_name = 
		    ipc_get_seat_name(ipc_portid_to_seat(dest_port));
	    } else {
		dest_port_name = ipc_get_name_by_port(dest_port);
	    }

	    if (dest_port_name == NULL) {
		dest_port_name = "Unknown";
	    }

	    ipc_chop_copy(dest_port_name,
			  chopped_dest_port_name, NAME_WIDTH);

	    size = message->header->size;
	    tries = message->retries;

	    reset_interrupt_level(status);
	
	    printf("%-5d.%5d %20s %-5d.%5d %20s %5d %2d\n",
		   ipc_portid_to_seat(source_port),
		   ipc_portid_to_port(source_port),
		   chopped_source_port_name,
		   ipc_portid_to_seat(dest_port),
		   ipc_portid_to_port(dest_port),
		   chopped_dest_port_name,
		   size,
		   tries);
	} else {
	    reset_interrupt_level(status);
	}
    } while (message != NULL);
}

