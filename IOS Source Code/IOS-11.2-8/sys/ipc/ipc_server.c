/* $Id: ipc_server.c,v 3.10.6.5 1996/08/28 12:50:48 thille Exp $
 * $Source: /release/112/cvs/Xsys/ipc/ipc_server.c,v $
 *------------------------------------------------------------------
 * ipc_server.c - The IPC message handling code.
 *
 * March 1995, Jim Hayes
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipc_server.c,v $
 * Revision 3.10.6.5  1996/08/28  12:50:48  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.10.6.4  1996/06/28  23:18:17  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.10.6.3  1996/06/02  15:32:02  motto
 * CSCdi50208:  Issuing csna, llc or tcp CIP show cmd immed after CIP
 * reset fails
 * Branch: California_branch
 *
 * Revision 3.10.6.2  1996/05/21  09:52:07  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.10.6.1  1996/03/18  20:18:20  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.2  1996/03/16  07:00:36  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.2.1  1996/03/07  09:47:37  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.10  1996/03/04  07:13:00  kao
 * CSCdi49674:  While VIP is cofigured with DFS, OIR event will crash the
 * VIP;
 *
 * Revision 3.9  1996/02/28  09:33:06  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.8  1996/02/24  02:13:26  dkerr
 * CSCdi49787:  IPC rolls over and dies under load
 * Make IPC more robust in an RSP->VIP distributed switching environment
 * (and increase performance from the miserly 100 msgs per minute to
 * something respectable, like 300K msgs per second).
 *
 * Revision 3.7  1996/02/23  08:18:14  dlobete
 * CSCdi47188:  micro reload on RSP halts IPC traffic to VIPs
 * Reset sequence numbers in Master's seat structure when we initialize
 * IPC slaves.
 *
 * Revision 3.6  1996/02/22  14:34:49  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/01/19  01:00:56  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1996/01/13  03:09:53  dlobete
 * CSCdi44696:  OIR and IPC troubles - selective sequence number synch
 * needed
 * Add slot granularity to IPC seat reset.
 *
 * Revision 3.3  1995/12/17  18:30:10  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:28:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:00:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:05:43  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/09/17  20:51:46  motto
 * CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
 *
 * Revision 2.4  1995/07/28  22:56:13  anil
 * CSCdi34114:  thread hung by ipc
 * Don't block on failure to send.
 *
 * CSCdi34182:
 * Ignore return code from ipc_output()
 *
 * CSCdi34112:
 * Don't use dangling pointer
 *
 * Revision 2.3  1995/07/13  00:12:59  dlobete
 * CSCdi34151:  UDP transport non-operational
 *
 * Revision 2.2  1995/06/29  18:24:00  dlobete
 * CSCdi34156:  remove ipc_process_deregister_name_request
 * This function no longer required since the IPC naming convention
 * stipulates name uniqueness within the local seat only ie
 * master doesn't get involved.
 *
 * Revision 2.1  1995/06/07  21:02:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
 
#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "config.h"
#include "subsys.h"
#include "logger.h"
#include "../ipc/msg_ipc.c"		/* Not a typo, see logger.h */
#include "config.h"
#include "parser.h"
#include "packet.h"
#include "sys_registry.h"
#include "mgd_timers.h"

#include "ipc_util.h"
#include "ipc.h"
#include "ipc_private.h"
#include "ipc_message.h"
#include "ipc_seats.h"
#include "ipc_ports.h"
#include "ipc_test.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../ui/debug.h"
#include "ipc_debug.h"
#include "ipc_chain.h"

ipc_global_info *ipc_globals;
ipc_platform_info ipc_platform_struct;
static mgd_timer master_timer;

static parser_extension_request ipc_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(show_ipc_commands) },
    { PARSE_ADD_TEST_CMD, &pname(test_ipc_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(debug_ipc_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * Registration of application delayed functions.
 */

struct ipc_delayed_func_ {
    struct ipc_delayed_func_ *next;	/* points to next entry */
    ipc_level_t level;			/* priority level */
    ipc_delayed_func_vector vector;	/* vector to function */
};

/*
 * Head of the linked list.
 */
static struct ipc_delayed_func_ *delayed_func_head = NULL;

/*
 * Register an application vector into our linked list
 * in sorted order by the level value.
 */
boolean ipc_register_delayed_func (ipc_level_t level,
    ipc_delayed_func_vector vector)
{
    struct ipc_delayed_func_ *fncp;
    struct ipc_delayed_func_ *cur;
    struct ipc_delayed_func_ *pre;

    fncp = malloc(sizeof(struct ipc_delayed_func_));
    if (fncp) {
	/*
	 * Initialize the new entry.
	 */
	fncp->vector = vector;
	fncp->level = level;
	fncp->next = NULL;
	/*
	 * Insert the new entry int linked list
	 * in sorted order.
	 */
	for (cur = delayed_func_head, pre = NULL; ; ) {
	    if (cur == NULL) {
		if (pre == NULL) {
		    delayed_func_head = fncp;
		} else {
		    pre->next = fncp;
		}
		break;
	    } else {
		if (level < cur->level) {
		    if (pre == NULL) {
			delayed_func_head = fncp;
		    } else {
			pre->next = fncp;
		    }
		    fncp->next = cur;
		    break;
		} else {
		    pre = cur;
		    cur = cur->next;
		}
	    }
	}
	return TRUE;
    }
    return FALSE;
}

/*
 * Scan the linked list, invoke the function
 * in each entry, then free all sources.
 */
static void ipc_invoke_delayed_func (void)
{
    struct ipc_delayed_func_ *fncp;

    while (delayed_func_head) {
	fncp = delayed_func_head;
	if (fncp->vector) {
	    (*fncp->vector)();
	} else {
	    errmsg(&msgsym(NODELFUNC, IPC));
	}
	delayed_func_head = fncp->next;
	free(fncp);
    }
}


/*
 * ipc_show_status:
 *
 * Show IPC system status
 */

void ipc_show_status (parseinfo *csb)
{
    printf("\nIPC System Status:\n\n");

    printf("This processor is %s server.\n\n",
	   (ipc_globals->is_ipc_master == TRUE)?"the IPC master":"a slave");

    printf("%d message%s in, %d out, %d delivered to local port,\n",
	   ipc_globals->received,
	   (ipc_globals->received == 1)?"":"s",
	   ipc_globals->sent,
	   ipc_globals->delivered);
    
    printf("%d acknowledgement%s received, %d sent,\n",
	   ipc_globals->acks_in,
	   (ipc_globals->acks_in == 1)?"":"s",
	   ipc_globals->acks_out);
    printf("%d fast ipc msgs sent, %d fast buffers sent.\n",
	   ipc_globals->fast_dfs, ipc_globals->fast_dfs_buffers);
    printf("%d fast ipc msgs received, %d fast buffers received.\n\n",
	   ipc_globals->fast_dfs_rx, ipc_globals->fast_dfs_rx_buffers);

    printf("%d message%s dropped, %d no local port, %d destination unknown,\n",
	   ipc_globals->dropped,
	   (ipc_globals->dropped == 1)?"":"s",
	   ipc_globals->no_port,
	   ipc_globals->no_seat);
    printf("%d missing callback or queue, %d duplicate ACK%s, %d retr%s,\n",
	   ipc_globals->no_delivery,
	   ipc_globals->dup_ack,
	   (ipc_globals->dup_ack == 1)?"":"s",
	   ipc_globals->retries,
	   (ipc_globals->retries == 1)?"y":"ies");
    printf("%d message timeout%s.\n",
	   ipc_globals->timeouts,
	   (ipc_globals->timeouts == 1)?"":"s");
    printf("%d ipc_output failures, %d mtu failures,\n",
	   ipc_globals->ipc_output_fails, ipc_globals->message_too_big);
    printf("%d msg alloc failed, %d pak alloc failed, %d memd alloc failed\n",
	   ipc_globals->no_message_buffer, ipc_globals->no_pak_buffer,
	   ipc_globals->no_memd_buffer);
    printf("%d no hwq, %d no port name, %d cannot bind port\n",
	   ipc_globals->no_hwq, ipc_globals->no_port_name,
	   ipc_globals->port_err);
	   
}

/*
 * SUPPORT ROUTINES:
 *
 * Silly support routines, generally called only once, but broken out to 
 * make the code read better.  These are inlines.
 *
 */

/*
 * ipc_output:
 *
 * Send an ipc message to the destination address by looking up the
 * destination seat and calling the its output vector.
 */

static inline boolean ipc_output (ipc_message *message,
				  ipc_seat_data *dest_seat)
{
    if (ipc_debug_blocking) {
	buginf("\nIPC: Server sent message:");
	ipc_dump_message(message);
    }

    if (dest_seat) {
	return (*dest_seat->send_vector)(message, dest_seat);
    } 

    if (ipc_debug_events) {
	buginf("IPC: ipc_output: destination seat is NULL");
    }
    return TRUE;
}

/*
 * ipc_yank_original_message:
 *
 * An ACK has arrived for a message we sent.  We need to yank the original
 * message off the retransmit queue, because we're done with it, and we
 * won't need to retransmit it.
 *
 * Returns:
 *    The yanked message, or NULL if the orignal message could not be found.
 *    Also prints a warning message about not being able to find the message
 *    if IPC event debugging is turned on.
 */

static inline ipc_message *ipc_yank_original_message(ipc_message *ack_message)
{
    ipc_message *the_original_message;
    ipc_message_header *header;

    header = ack_message->header;
    
    the_original_message = 
	ipc_get_message_by_sequence(ipc_message_table,
	    (ipc_portid_to_seat(header->source_port)) | (header->sequence & 0x03ff));

    if (the_original_message == NULL) {
	if (ipc_debug_events) {
	    buginf("\nIPC: Got bogus ack from %d.%d, sequence=%d",
		   ipc_portid_to_seat(header->source_port),
		   ipc_portid_to_port(header->source_port),
		   header->sequence);
	}
	return NULL;
    } 

    if (ipc_debug_packets) {
	buginf("\nIPC: Got ack from %d.%d, sequence=%d",
	       ipc_portid_to_seat(header->source_port),
	       ipc_portid_to_port(header->source_port),
	       header->sequence);
	buginf("\n     For original message:");
	ipc_dump_message(the_original_message);
    }

    ipc_thread_yank(ipc_message_table, the_original_message);
    return the_original_message;
}

/*
 * ipc_send_ack_reply:
 *
 * Create and send an acknowledgement packet for a message.
 * The ACK packet is basically reflected back to the sender's server.
 */
static inline void ipc_send_ack_reply (ipc_message *message)
{
    ipc_message_header *original_header;
    ipc_message *reply_ack;
    ipc_seat_data *dest_seat;

    original_header = message->header;

    if (message == NULL) return;

    /*
     * Find the destination where the ACK will be sent.
     */

    dest_seat = ipc_get_seat(ipc_portid_to_seat(original_header->source_port));
    if (dest_seat == NULL) return;

    /*
     * Create the ACK message, 0 bytes long, with the destination address
     * pointing back to the source of the original message.
     */

    reply_ack = ipc_get_message(0, original_header->source_port, 0);
    if (reply_ack == NULL) return;

    reply_ack->header->source_port = original_header->dest_port;
    reply_ack->header->flags |= IPC_FLAG_ACK;
    reply_ack->header->sequence = message->header->sequence;

    /*
     * Send it on it's way.
     */

    ipc_output(reply_ack, dest_seat);
    ipc_globals->acks_out++;
    ipc_return_message(reply_ack);
}

/*
 * ipc_notify_sender:
 *
 * Notify the sender of this message that there's been a status change
 * in the message.  Valid status changes are:
 *
 * IPC_OK -        The message arrived at the far end.
 * IPC_TIMEOUT -   The message didn't arrive at the far end.
 * IPC_NO_MEMORY - The message couldn't be sent due to lack of resources.
 *
 */
static void ipc_notify_sender (ipc_message *message, ipc_error_code code)
{
    boolean should_dispose_message;

    should_dispose_message = TRUE;

    /*
     * Save the result code in the message
     */

    message->result = code;

    /*
     * Unblock the process associated with this message, if necessary.
     */

    if (message->flags & IPC_FLAG_BLOCKED) {
	message->flags &= (~IPC_FLAG_BLOCKED);
	should_dispose_message = FALSE;
    }

    /*
     * If the process sending this message elected to use async notification, 
     * call that process' callback routine.
     */

    if (message->notify_callback) {
	(*message->notify_callback)(message, message->notify_context, code);
	should_dispose_message = FALSE;
    }
    
    /*
     * If nobody claimed this message as blocked or callback, then we
     * should get rid of it here.  This should never happen, though
     * anything is possible, especially with all the SPM's we have.
     */

    if (should_dispose_message)
	ipc_return_message(message);
}


/*
 * ipc_process_ack_message:
 *
 * Process an acknowledgement message from a remote or local seat:
 * - Pull the message being ACK'd off the retransmit queue
 * - Call an async notify routine to show that a particular message completed,
 *   and/or unblock the process that may be waiting on this acknowledgement.
 * - Consume the acknowledgement message.
 */

static inline void ipc_process_ack_message (ipc_message *ack_message)
{
    ipc_message *the_original_message;

    /*
     * Fetch the original message saved in the retransmit queue and
     * remove it-- we have the ACK for the message, we don't need it
     * anymore.
     */

    ipc_globals->acks_in++;
    
    the_original_message = ipc_yank_original_message(ack_message);

    ipc_return_message(ack_message);

    if (the_original_message == NULL) return;

    if (the_original_message->header->flags & IPC_FLAG_IS_RPC) {
	/*
	 * If we're an RPC, don't deactivate the message in the queue.  We'll
	 * yank the original RPC request when the reply arrives.  It's kept on
	 * the retransmission queue so we can time out the final reply if
	 * necessary.
	 *
	 * To accommodate applications whose rpc operations may take
	 * a long time, we dequeue the rpc req message from ipc_message_table
	 * and place them in ipc_rpc_req_table with longer timeout value.
	 * We also discard without retry when the these rpc req messages
	 * timeout in ipc_rpc_req_table (see ipc_process_retry_queue).
	 *
	 * The timeout value has been changed to be configurable.
	 *
	 * Also, to prevent too many rpc req messages in the table,
	 * we put a limit on the maximum number of messages in it.
	 */
	the_original_message->flags |= IPC_FLAG_GOT_RPC_ACK;
	if (ipc_thread_get_count(ipc_rpc_req_table) >= IPC_RPC_REQ_TABLE_MAX) {
	    ipc_notify_sender(the_original_message, IPC_INTERNAL);
	} else {
    	    mgd_timer_init_leaf(&the_original_message->rpc_timer,
		&master_timer, 0, the_original_message, FALSE);
	    mgd_timer_start(&the_original_message->rpc_timer,
		the_original_message->rpc_timeout);
	    ipc_thread_append(ipc_rpc_req_table, the_original_message);
	}
    } else {
	ipc_notify_sender(the_original_message, IPC_OK);
    }
}

/*
 * ipc_check_sequence
 *
 * Check an inbound message to see if its sequence number is correct.
 *
 * If this message is "in sequence" we return IS_CURRENT.
 * If this message is one we've already seen (the ACK got lost)
 *    we return IS_FROM_PAST.
 * If this message appears to be from the future (we missed a message
 *    somewhere) we return IS_FROM_FUTURE.
 */

static inline ipc_sequence_report ipc_check_sequence (ipc_message *message)
{
    ipc_message_header *header;
    ipc_seat_data *sender;

    ipc_sequence received;
    ipc_sequence expected;

    header = message->header;

    sender = ipc_get_seat(ipc_portid_to_seat(header->source_port));
    if (sender == NULL) return IS_FROM_FUTURE;

    received = header->sequence;
    expected =
        (sender->last_heard == IPC_MAX_SEQUENCE ? 0 : sender->last_heard + 1);

    if (received == expected) {
        sender->last_heard = received;
        return IS_CURRENT;
    }

    ipc_globals->dup_ack++;

    if (received < expected) {
        if (expected - received < IPC_MAX_SEQUENCE >> 1) 
            return IS_FROM_PAST;
        return IS_FROM_FUTURE;
    } else {
        if (received - expected < IPC_MAX_SEQUENCE >> 1) 
            return IS_FROM_FUTURE;
    } 
    return IS_FROM_PAST;
    
}

/*
 * ipc_dispatch_inbound_message:
 *
 * Take a message for a port on the local system and dispatch it based
 * the receive method set for the port.  Either queue the message or call
 * a callback routine.
 */

static inline void ipc_dispatch_inbound_message (ipc_message *message)
{
    ipc_port_data *port_p = NULL;
    /*
     * CSCdi34112
     * Re-figure the port ptr. Don't use message->port_p (which should
     * be eliminated)
     * Note: 'message' is guaranteed non-NULL
     */
    if ( message->header != NULL ) {
	port_p = ipc_get_port(message->header->dest_port);
    }
    if ( port_p == NULL ) {
	ipc_return_message(message);
	return;
    }
 
    switch(port_p->method) {
    case IPC_CALLBACK:
 	if (port_p->receive_callback) {
	    (*port_p->receive_callback) (message, port_p->receive_context,
		IPC_OK);
	    ipc_globals->delivered++;
	    return;
	} else {
	    if (ipc_debug_events)
		buginf("\nIPC: Message arrived for %d.%d, with NULL "
		       "callback.",
		       ipc_portid_to_seat(message->header->dest_port),
		       ipc_portid_to_port(message->header->dest_port));
	    break;
	}

    default:
	errmsg(&msgsym(NODISPATCH, IPC),
	       ipc_portid_to_seat(message->header->dest_port),
	       ipc_portid_to_port(message->header->dest_port));
    }

    ipc_globals->no_delivery++;
    ipc_return_message(message);
}

/*
 * ipc_dispatch_rcp_message:
 *
 * Do some extra housekeeping for RCP reply messages.  We need to find the
 * original RPC query message on the retransmit queue, stuff the reply in
 * and dispatch the message.  The original sequence number of the RPC request
 * is in the reply's TYPE field-- that's how we pull it out of the retransmit
 * queue to match up the RPC request with the response.
 */
static void ipc_dispatch_rpc_reply(ipc_message *reply_message)
{
    ipc_message *original_message;

    /*
     * Match the original message with the reply.
     */

    original_message = ipc_get_message_by_sequence(
	ipc_rpc_req_table,
	ipc_portid_to_seat(reply_message->header->source_port) | 
	(reply_message->header->type & IPC_CLASS_A_PORT_MASK));

    if (original_message == NULL) {
	buginf("\nIPC: NULL ORIGINAL RPC QUERY!");
	ipc_return_message(reply_message);
	return;
    }

    ipc_thread_yank(ipc_rpc_req_table, original_message);

    mgd_timer_stop(&original_message->rpc_timer);

    /*
     * Tell the routine waiting for the original message to finish
     * where to find the reply.
     */
    
    original_message->notify_context = reply_message;
    original_message->flags &= ~IPC_FLAG_BLOCKED;
}

static inline void ipc_fast_dispatch_inbound_message (ipc_message * message)
{
    ipc_port_data *port_p = message->port_p;

    switch (port_p->method) {
	case IPC_FAST_CALLBACK:
	if (port_p->receive_callback) {
	    (*port_p->receive_callback) (message, port_p->receive_context,
					    IPC_OK);
	    ipc_globals->delivered++;
	    return;
	} else {
	    if (ipc_debug_events)
		buginf("\nIPC: Message arrived for %d.%d, with NULL "
		       "callback.",
		       ipc_portid_to_seat(message->header->dest_port),
		       ipc_portid_to_port(message->header->dest_port));

	    break;
	}
      case IPC_CALLBACK:
	message->port_p = NULL;	/* CSCdi34112: can't allow dangling pointer */
 	p_enqueue(&ipc_globals->inboundQ, message);
 	return;
      case IPC_QUEUE:
	if (port_p->receive_queue != NULL) {
	    p_enqueue(port_p->receive_queue, message);
	    ipc_globals->delivered++;
	    return;
	} else {
	    if (ipc_debug_events)
		buginf("\nIPC: Message arrived for %d.%d, with NULL queue.",
		    ipc_portid_to_seat(message->header->dest_port),
		    ipc_portid_to_port(message->header->dest_port));
	    break;
	}
      default:
	errmsg(&msgsym(NODISPATCH, IPC),
	       ipc_portid_to_seat(message->header->dest_port),
	       ipc_portid_to_port(message->header->dest_port));
    }
    ipc_return_message(message);
}

/*
 * ipc_process_message
 *
 * This is the funnel where ALL inbound messages to the IPC go
 */

static void ipc_process_message (ipc_message *message)
{
    ipc_sequence_report message_status;
    ipc_port_data *port_p;
    ipc_flags flags;

    /*
     * First things first.  If this is an ACK message, process it
     * immediately, otherwise it's data so try to deliver it.
     */
    ipc_globals->received++;

    flags = message->header->flags;

    /*
     * This is for dfs messages.  Don't ACK, and don't check or
     * update sequence numbers.
     */
    if (flags & IPC_FLAG_DO_NOT_ACK) {
	message->port_p = port_p = ipc_get_port(message->header->dest_port);
	if (message->port_p == NULL) {
	    return;
	}
	ipc_fast_dispatch_inbound_message(message);
	return;
    }

    if (flags & IPC_FLAG_ACK) {
	ipc_process_ack_message(message);
	return;
    }

    if (!(flags & IPC_FLAG_IS_RPC_REPLY)) {
	message->port_p = port_p = ipc_get_port(message->header->dest_port);
    
	if (port_p == NULL) {
	    if (ipc_debug_events) {
		buginf("\nIPC: received message for unknown port %d",
		       ipc_portid_to_port(message->header->dest_port));
	    }
	    ipc_return_message(message);
	    ipc_globals->no_port++;
	    return;
	}
    }

    message_status = ipc_check_sequence(message);

    switch (message_status) {
    case IS_CURRENT:			/* Correct sequence, send ACK */
	ipc_send_ack_reply(message);
	break;

    case IS_FROM_PAST:			/* Dup msg. re-ack and dump data */
	ipc_send_ack_reply(message);
	ipc_return_message(message);
	return;

    case IS_FROM_FUTURE:		/* Missed msg. no ACK, dump data */
	ipc_return_message(message);
	return;
    }

    /*
     * Deliver the message based on the method specified by the receiving port
     */
    if (flags & IPC_FLAG_IS_RPC_REPLY) {
	ipc_dispatch_rpc_reply(message);
    } else {
	ipc_fast_dispatch_inbound_message(message);
    }
}


/*
 * ipc_process_raw_pak
 *
 * Pakets generally arrive at the IPC server as "paktypes", so before we can
 * funnel them into the IPC system, we need to set up a message buffer
 * and put pak into it, so its presentable to the IPC.
 *
 * Take a message for a port on the local system and dispatch it based
 * the receive method set for the port.  If its FAST its dispatched here,
 * otherwise its queued on inboundQ for forktype ipc_server_process ()
 * to process.
 *
 * all input must be processed here. this routine may be called at process
 * or interrupt level.  this level does the ack processing.  input for a given
 * ipc_port_data *port_p must be at the same level (process or interrupt
 * level) -  this to preserve proper ack processing.
 */
void ipc_process_raw_pak (paktype *pak)
{
    ipc_message *the_message;

    if (pak == NULL)
	return;
    
    /*
     * Check if it is a permanent port packet
     */
    if (((ipc_message_header *)ipcheadstart(pak))->dest_port == IPC_PERMANENT_PORT) {
	if (ipc_platform_struct.platform_permanent_port_process_vector)
	    (*ipc_platform_struct.platform_permanent_port_process_vector)(pak);
	datagram_done(pak);
	return;
    }

    /*
     * Sew the "pak" into a message and start it on it's way.
     */
    the_message = ipc_make_message_from_pak(pak);

    if (the_message == NULL) {
	if (ipc_debug_events) {
	    buginf("\nIPC: No resources available to process ipc packet.");
	}
	datagram_done(pak);
	return;
    }

    /*de-encapsulate ipc_message_header from pak*/
    pak->datagramstart += IPC_MESSAGE_ENCAPS_SIZE;
    pak->datagramsize = the_message->header->size;

    if (ipc_debug_packets) {
	buginf("\nIPC: Server received message:");
	ipc_dump_message(the_message);
    }

    /*
     * Hand the message to the server so it can chew on it for a bit.
     */

    ipc_process_message(the_message);
}

void ipc_flush_retry_queue (void)
{
    ipc_message *message;

    while ( (message = ipc_thread_yank_head(ipc_message_table)) != NULL) {
	if (ipc_debug_events)
	    buginf("\nIPC: message %x callback %x is flushed",
		message, message->notify_callback);
	ipc_notify_sender(message, IPC_INTERNAL);
    }
}

/*
 * ipc_inc_output_sequence()
 *
 * Increment the output sequence number
 *
 * Inputs:
 * ipc_header - pointer to IPC header
 * output_seat - pointer to output seat info
 *
 * Outputs:
 * ipc_header - IPC header with incremented sequence number
 *
 * Returns:
 * NONE
 */
static inline void ipc_inc_output_sequence (ipc_message_header *ipc_header,
					    ipc_seat_data *output_seat)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    if (++output_seat->last_sent > IPC_CLASS_A_PORT_MASK) {
	output_seat->last_sent = 0;
    }
    ipc_header->sequence = output_seat->last_sent;
    reset_interrupt_level(level);
}

/*
 * ipc_dec_output_sequence()
 *
 * Decrement the output sequence number
 *
 * Inputs:
 * ipc_header - pointer to IPC header
 * output_seat - pointer to output seat info
 *
 * Outputs:
 * ipc_header - IPC header with decremented sequence number
 *
 * Returns:
 * NONE
 */
static inline void ipc_dec_output_sequence (ipc_message_header *ipc_header,
					    ipc_seat_data *output_seat)
{
    leveltype level;
    ipc_sequence new_sequence;

    new_sequence = ipc_header->sequence - 1;
    if (new_sequence > IPC_CLASS_A_PORT_MASK) {
        new_sequence = IPC_CLASS_A_PORT_MASK;
    }

    level = raise_interrupt_level(NETS_DISABLE);
    if (new_sequence <= output_seat->last_sent) {
        output_seat->last_sent = new_sequence;
    } else if ((output_seat->last_sent - new_sequence) < IPC_GRACE_SEQUENCE) {
        output_seat->last_sent = new_sequence;
    }
    reset_interrupt_level(level);
}


/*
 * ipc_process_retry_queue:
 *
 * Scurry through our retry queue to see if any messages in it need 
 * retransmission.  If so, re-transmit.
 *
 * This list is ordered, so running into a message with time remaining means
 * we know that we're done.
 */

static void ipc_process_retry_queue (void)
{
    ipc_message *message;
    ipc_seat_data *dest_seat;
    mgd_timer *expired_timer;

    /*
     * Check if any timers expired in rpc request message queue.
     */
    while (mgd_timer_expired(&master_timer)) {
	expired_timer = mgd_timer_first_expired(&master_timer);
	message = mgd_timer_context(expired_timer);
	if (message) {
	    mgd_timer_stop(&message->rpc_timer);
	    ipc_thread_yank(ipc_rpc_req_table, message);
	    ipc_notify_sender(message, IPC_TIMEOUT);
	}
    }

    while ( (message = ipc_thread_get_head(ipc_message_table)) != NULL) {

	/*
	 * Has this message's retry-timer expired?
	 */

	if (TIMER_RUNNING_AND_AWAKE(message->retry_time)) {
	    /*
	     * Increment the retry count, and pull the message from the
	     * top of the retransmit queue
	     */

	    message->retries++;
	    ipc_thread_yank(ipc_message_table, message);

	    /*
	     * Too many retries?
	     */

	    if (message->retries > IPC_RETRY_MAX) {
		if (!(message->flags & IPC_FLAG_GOT_RPC_ACK)) {
		    dest_seat =	ipc_get_seat(
                              ipc_portid_to_seat(message->header->dest_port));
                    if (dest_seat) {
                        ipc_dec_output_sequence (message->header, dest_seat);
                    }
                }
		ipc_notify_sender(message, IPC_TIMEOUT);
	    } else {

		if (ipc_debug_events) {
		    buginf("\nIPC: Retry attempt %d for this message: %x",
			   message->retries, message);
		    ipc_dump_message(message);
		}

		/*
		 * Reset the retry timer, rehang the message on the
		 * tail of the retransmit queue and re-transmit.
		 */

		TIMER_START(message->retry_time, IPC_RETRY_PERIOD);
		ipc_thread_append(ipc_message_table, message);

		if (!(message->flags & IPC_FLAG_GOT_RPC_ACK)) {
		    dest_seat =	ipc_get_seat(
                              ipc_portid_to_seat(message->header->dest_port));
		    if (dest_seat == NULL) continue;
		    ipc_output(message, dest_seat);
		    ipc_globals->retries++;
		}
	    }
	} else {
	    break;
	}
    }
}

static void ipc_server_periodic (void)
{
    /*
     * We may use this one day to perhaps time out fragments on the reassembly
     * queue, or something like that.   For now we do nothing except reset
     * the timer.
     */

    if (ipc_platform_struct.do_delay_initialize) {
	ipc_flush_retry_queue();
	ipc_change_seat_info();	
	/*
	 * Take care of any delayed functions we should run.
	 */
	ipc_invoke_delayed_func();
	ipc_platform_struct.do_delay_initialize = FALSE;
    }

    /*
     * Reset the periodic timer
     */

     TIMER_START(ipc_globals->periodic, IPC_MAINTENANCE_PERIOD);
}

static boolean ipc_serverBLOCK(void)
{
    ipc_message *the_message;

    /* 
     * Check to see if we have any packets waiting on our inbound queue.
     */
    
    if (ipc_globals->inboundQ.qhead != NULL) return FALSE;

    /*
     * Should we do any periodic tasks?
     */

    if (TIMER_RUNNING_AND_AWAKE(ipc_globals->periodic)) return FALSE;

    /*
     * Should we try to process some message retransmissions?
     */

    the_message = ipc_thread_get_head(ipc_message_table);
    if (the_message != NULL) {
	if (TIMER_RUNNING_AND_AWAKE(the_message->retry_time)) return FALSE;
    }

    /*
     * If we get this far, there's no work to do.
     */

    return TRUE;
}

static forktype ipc_server_process (void)
{
    ipc_message *the_message;
    int switch_count;

    for (;;) {
	edisms((blockproc *)ipc_serverBLOCK, 0);


	/*
	 * Consume some IPC packets
	 */

	switch_count = PAK_SWITCHCOUNT;

	do {
	    the_message = p_dequeue(&ipc_globals->inboundQ);
	    if (the_message) {
		ipc_dispatch_inbound_message(the_message);
	    } else {
		break;
	    }
	    switch_count--;

	} while (switch_count > 0);

	/*
	 * See if we need to retransmit some messages
	 */
	ipc_process_retry_queue();

	/*
	 * See if we need to do some periodic functions
	 */

	if (TIMER_RUNNING_AND_AWAKE(ipc_globals->periodic)) {
	    ipc_server_periodic();
	}
    }
}
static boolean ipc_realmBLOCK(void)
{
    /* 
     * Check to see if we have any packets waiting on our inbound queue.
     */

    if (ipc_globals->realm_inboundQ.qhead != NULL) return FALSE;

    /*
     * If we get this far, there's no work to do.
     */

    return TRUE;
}

static forktype ipc_realm_manager_process (void)
{
    ipc_message *message;

    int switch_count;

    for (;;) {
	edisms((blockproc *)ipc_realmBLOCK, 0);

	/*
	 * Consume some IPC packets
	 */

	switch_count = PAK_SWITCHCOUNT;

	do {
	    message = p_dequeue(&ipc_globals->realm_inboundQ);
	    if (message)
		ipc_return_message(message);
	    else
		break;
	    switch_count--;
	} while (switch_count > 0);
    }
}

static boolean ipc_zoneBLOCK(void)
{
    /* 
     * Check to see if we have any packets waiting on our inbound queue.
     */

    if (ipc_globals->zone_inboundQ.qhead != NULL) return FALSE;

    /*
     * If we get this far, there's no work to do.
     */

    return TRUE;
}

static forktype ipc_zone_manager_process (void)
{
    ipc_message *message;
    ipc_platform_info *ipc_platform = &ipc_platform_struct;
    ipc_slot_init_msg *init_msg;

    int switch_count;

    for (;;) {
	edisms((blockproc *)ipc_zoneBLOCK, 0);

	/*
	 * Consume some IPC packets
	 */

	switch_count = PAK_SWITCHCOUNT;

	do {
	    message = p_dequeue(&ipc_globals->zone_inboundQ);
	    if (message) {
		init_msg = (ipc_slot_init_msg *)message->data;
		if (init_msg->header.type == SLOT_SEAT_INIT) {
                   if (ipc_platform->platform_init_slaves_vector) {
                       ipc_platform->platform_init_slaves_vector(
							    init_msg->slot);
		   }
		}
    		ipc_return_message(message);
	    }
	    else
		break;
	    switch_count--;
	} while (switch_count > 0);
    }
}


/*
 * This is the local transmit vector.  It simply queues the message on the IPC
 * server's input queue.
 */

#ifdef OLD
static boolean ipc_send_local (ipc_message *message, ipc_seat_data *seat)
{
    paktype *pak;

    /*
     * Lock the packet so the lower levels don't free it.  We are using the
     * server's copy of the data, so it would be a mistake to free it.  The
     * server may need it to retransmit...
     */

    pak = message->pak;
    message->pak = NULL;
    ipc_return_message(message);

    pak_lock(pak);
    ipc_process_raw_pak(pak);
    
    return FALSE;
}
#endif
static boolean ipc_send_local (ipc_message *message, ipc_seat_data *seat)
{
    paktype *dup_pak;

    if (message == NULL || message->pak == NULL)
        return TRUE;

    dup_pak = pak_duplicate(message->pak);
    if (dup_pak == NULL)
        return TRUE;

    ipc_process_raw_pak(dup_pak);

    return FALSE;
}

/*
 * BLOCK routine used by library routines to sit and wait for a message
 * to become ready.
 */

static boolean ipc_messageBLOCK(ipc_message *message)
{
    if (message->flags & IPC_FLAG_BLOCKED) return TRUE;

    return FALSE;
}
 
ipc_error_code ipc_send_message_noack (ipc_message *message)
{
    ipc_message_header  *hdr = message->header;
    ipc_seat_data *output_seat;

    /* Validate our message */

    if (message == NULL) {
	errmsg(&msgsym(CONSISTENCY,IPC),"send_mesage_async: NULL message.");
	return IPC_INTERNAL;
    }

    output_seat = ipc_get_seat(ipc_portid_to_seat(message->header->dest_port));

    if (output_seat == NULL) {
	ipc_globals->no_seat++;
        ipc_return_message(message);
	return IPC_NO_SUCH_SEAT;
    }

    /*
     * Set up the message call-back fields, timer and hook into re-xmit queue
     */

    if (++output_seat->last_sent > IPC_CLASS_A_PORT_MASK)
	output_seat->last_sent = 0;
    hdr->flags = IPC_FLAG_DO_NOT_ACK;
    message->header->sequence = output_seat->last_sent;
    message->notify_callback = NULL;
    message->notify_context = NULL;
    message->sequence = 
	ipc_portid_to_seat(message->header->dest_port) | 
			(message->header->sequence & IPC_CLASS_A_PORT_MASK);

    if (ipc_output(message, output_seat) == TRUE)
	ipc_globals->ipc_output_fails++;
    
    ipc_return_message(message);
    ipc_globals->sent++;

    return IPC_OK;
}
/*
 * Send a message to a remote destination and return immediately.
 * When the message arrives at the destination (or times out) call
 * the "callback" routine to notify the caller that something happened.
 *
 * This routine DOES NOT CONSUME the original message.  It is up to
 * the callback routine to consume (or reap) the message when called.
 */
ipc_error_code ipc_send_message_async (ipc_message *message,
				       void *callback_context,
				       ipc_callback routine)
{
    ipc_seat_data *output_seat;

    /* Validate our message */

    if (message == NULL) {
	errmsg(&msgsym(CONSISTENCY,IPC),"send_mesage_async: NULL message.");
	return IPC_INTERNAL;
    }

    output_seat = ipc_get_seat(ipc_portid_to_seat(message->header->dest_port));

    if (output_seat == NULL) {
	ipc_globals->no_seat++;
        ipc_return_message(message);
	return IPC_NO_SUCH_SEAT;
    }

    /*
     * Set up the message call-back fields, timer and hook into re-xmit queue
     */

    message->notify_callback = routine;
    message->notify_context = callback_context;
    
    ipc_inc_output_sequence(message->header, output_seat);
    
    message->sequence = 
	ipc_portid_to_seat(message->header->dest_port) | 
			(message->header->sequence & IPC_CLASS_A_PORT_MASK);

    /*
     * If we're being called from an interrupt routine, figure out if
     * we can actually send the packet or not.  If not, move it to the
     * top of the retransmit queue, so we can send it at process level
     * instead.
     */
    
    if (onintstack() && output_seat->interrupt_ok == FALSE) {
	TIMER_START(message->retry_time, 1);
	ipc_thread_prepend(ipc_message_table, message);
	return IPC_OK;
    } else {
	TIMER_START(message->retry_time, IPC_RETRY_PERIOD);
	ipc_thread_append(ipc_message_table, message);
    }

    /*
     * Frag/Defrag here, at some point  (FIXME)
     */

	/*
	 * CSCdi34182
	 * The message is enqueued for retry. Ignore return code from
	 * ipc_output() since the only relevant return code (under the
	 * mandatory retry policy of ipc) is the return code from the
	 * final retry attempt.
	 */
    if (ipc_output(message, output_seat) == TRUE)
	ipc_globals->ipc_output_fails++;
    
    ipc_globals->sent++;

    return IPC_OK;
}

/*
 * Send a message to a remote destination, and block until the message
 * has been acknowledged or times out.
 *
 * The routine CONSUMES the original message.
 */

ipc_error_code ipc_send_message(ipc_message *message)
{
    ipc_error_code result;

    message->flags |= IPC_FLAG_BLOCKED;

    /*
     * CSCdi34114
     * Don't block on failure to send. You could block forever.
     */
    result = ipc_send_message_async(message, NULL, NULL);
    if (result != IPC_OK) {
	ipc_return_message(message);
	return result;
    }
    
    edisms((blockproc *)ipc_messageBLOCK, (long) message);

    result = message->result;

    ipc_return_message(message);
 
    return result;
}

/*
 * Send a pak to a remote destination.
 */

ipc_error_code ipc_send_pak_async(paktype *pak,
				  ipc_port_id dest_port,
				  ipc_message_type type,
				  void *callback_context,
				  ipc_callback routine)
{
    ipc_message *message;
    ipc_message_header *header;

    pak->datagramstart -= IPC_MESSAGE_ENCAPS_SIZE;

    message = ipc_make_message_from_pak(pak);

    if (message == NULL) return IPC_NO_MEMORY;

    header = message->header;
    header->source_port = ipc_make_portid(ipc_globals->local_seat_addr,0);
    header->dest_port = dest_port;
    header->type = type;

    header->size = pak->datagramsize;
    pak->datagramsize += IPC_MESSAGE_ENCAPS_SIZE;

    header->flags = 0;

    return ipc_send_message_async(message, callback_context, routine);
}


/*
 * Send a message to a remote destination by providing a pointer to the
 * message data and a destination port and message type.  This routine
 * blocks until the message has been acknowledged or times out.
 *
 * This routine CONSUMES the original message.
 * This routine COPIES the message data into a packet before sending.
 */

ipc_error_code ipc_send_opaque(void *message_data,
			       ipc_size message_size,
			       ipc_port_id dest_port,
			       ipc_message_type type)
{
    ipc_message *message;
    
    message = ipc_get_message(message_size, dest_port, type);
    
    if (message == NULL) return IPC_NO_MEMORY;

    bcopy(message_data, message->data, message_size);

    return ipc_send_message(message);
}

/*
 * Send a message to a remote destination by providing a pointer to the
 * message data and a destination port and message type.  This routine
 * returns immedately and notifies the caller of success of failure through
 * a callback routine.
 *
 * This routine DOES NOT CONSUME the original message.  The callback routine
 * must do this.
 *
 * This routine COPIES the message data into a packet before sending.
 */

ipc_error_code ipc_send_opaque_async(void *message_data,
				     ipc_size message_size,
				     ipc_port_id dest_port,
				     ipc_message_type type,
				     void *callback_context,
				     ipc_callback routine)
{
    ipc_message *message;

    message = ipc_get_message(message_size, dest_port, type);
    
    if (message == NULL) return IPC_NO_MEMORY;

    bcopy(message_data, message->data, message_size);

    return ipc_send_message_async(message, callback_context, routine);
}

/*
 * Send a remote procedure call message and block until the a reply is
 * received or the message times out.
 *
 * This routine CONSUMES the original message upon completion.
 */
ipc_message *ipc_send_rpc(ipc_message *message,
			  ipc_error_code *error)
{
    ipc_message *reply = NULL;

    message->flags |= IPC_FLAG_BLOCKED;
    message->header->flags |= IPC_FLAG_IS_RPC;

    /*
     * CSCdi34114
     * Don't block on failure to send. You could block forever.
     */
    *error = ipc_send_message_async(message, NULL, NULL);
    if ( *error != IPC_OK ) {
    	ipc_return_message(message);
	return NULL;
    }

    edisms((blockproc *)ipc_messageBLOCK, (long) message);

    *error = message->result;
    reply = (ipc_message *)message->notify_context;

    ipc_return_message(message);
    
    return reply;
}

/*
 * Reply to a remote procedure call and return immediately.  No need to
 * do much more than fill in the message->data area with your reply, as this
 * routine will "fill in the blanks" based on the original message.
 *
 * This routine immediately CONSUMES the original message, and does not
 * consume the reply message--  The callback routine is reponsible for
 * consuming the reply message when it arrives.
 */
ipc_error_code ipc_send_rpc_reply_async(ipc_message *original_message,
					ipc_message *reply_message,
					void *callback_context,
					ipc_callback routine)
{
    reply_message->header->flags |= IPC_FLAG_IS_RPC_REPLY;
    reply_message->header->dest_port = original_message->header->source_port;
    reply_message->header->type = original_message->header->sequence;
    ipc_return_message(original_message);

    return ipc_send_message_async(reply_message, callback_context, routine);
}

/*
 * Reply to a remote procedure call and block until the reply arrives at the
 * original caller.  No need to do much more than fill in the message->data
 * area with your reply, as this routine will "fill in the blanks" based on
 * the original message.
 *
 * This routine CONSUMES both the original message and the reply message
 * upon completion.
 */
ipc_error_code ipc_send_rpc_reply(ipc_message *original_message,
				  ipc_message *reply_message)
{
    reply_message->flags |= IPC_FLAG_BLOCKED;
    reply_message->header->flags |= IPC_FLAG_IS_RPC_REPLY;
    reply_message->header->dest_port = original_message->header->source_port;
    reply_message->header->type = original_message->header->sequence;

    ipc_return_message(original_message);

    return ipc_send_message(reply_message);
}


static void ipc_process_echo_message_callback (ipc_message *message,
					       void *context,
					       ipc_error_code error)
{
    ipc_return_message(message);
}

/*
 * This message handler supports reflecting RPC style messages back
 * to the sender, and is primarily used to test the RPC processing code.
 */
static void ipc_process_echo_message (ipc_message *message,
				      void *context,
				      ipc_error_code error)
{
    ipc_error_code error_code;
    ipc_message *reply_message;

    if (message->header->flags != IPC_FLAG_IS_RPC) {
	if (ipc_debug_events) {
	    buginf("\nIPC: non RPC message from %d.%d arrived on echo port.",
		   ipc_portid_to_seat(message->header->source_port),
		   ipc_portid_to_port(message->header->source_port));
	}
	ipc_return_message(message);
	return;
    } else {
	/* 
	 * Make a new message of the same size, copy the message and return
	 * it to the sender.
	 */

	reply_message = ipc_get_message(message->header->size, 0, 0);

	if (reply_message == NULL) {
	    ipc_return_message(message);
	    return;
	}
	
	bcopy(message->data, reply_message->data, message->header->size);

	error_code = ipc_send_rpc_reply_async(message, reply_message, NULL,
					  ipc_process_echo_message_callback);

	if (error_code != IPC_OK) {
	    if (ipc_debug_events)
		buginf("\nIPC: echo port couldn't reply "
		       "to echo request: (%s).\n",
		       ipc_decode_error(error_code));
	    ipc_return_message(reply_message);
	}
    }
}

static void ipc_process_register_name_request (ipc_message *message)
{
    ipc_register_name_request *request;
    ipc_register_name_response *response;
    ipc_message *response_message;
    ipc_error_code error;
    ipc_port_data *the_port;
    ipc_name *the_name, template;

    char *name;

    request = message->data;
    error = IPC_OK;

    name = ipc_extract_string_copy(request->name_length, request->name);
    if (name == NULL) error = IPC_NO_MEMORY;

    /*
     * Check for duplicates.
     */
    template.name = name;
    the_name = ipc_thread_lookup(ipc_name_table, &template);
    
    if (the_name == NULL) {

	the_port = ipc_create_port_entry(name, request->port_id);

	if (the_port == NULL) error = IPC_NO_MEMORY;

	the_port->flags &= (~IPC_PORT_IS_LOCAL);
    }
    free(name);

    response_message = ipc_get_message(sizeof(ipc_register_name_response),
				       0, 0);

    if (response_message == NULL) return;

    response = response_message->data;
    response->error_code = error;

    ipc_send_rpc_reply_async(message, response_message, NULL, NULL);
}

static void ipc_process_lookup_name_request (ipc_message *message)
{
    ipc_lookup_name_request *request_data;
    ipc_lookup_name_response *response_data;
    ipc_message *response;
    ipc_port_id result;
    char *name;

    
    request_data = message->data;

    name = ipc_extract_string_copy(request_data->name_length,
				   request_data->name);
    if (name == NULL) {
	result = 0;
    } else {
	result = ipc_locate_port(name);
    }

    if (ipc_debug_ports)
	buginf("\nIPC: received name lookup for '%s'.", name);

    if (name) free(name);

    response = ipc_get_message(sizeof(ipc_lookup_name_response), 0, 0);

    if (response == NULL) return;

    response_data = response->data;
    response_data->port_id = result;

    ipc_send_rpc_reply_async(message, response, NULL, NULL);
}

static void ipc_process_zone_man_message (ipc_message *message,
					void *context,
					ipc_error_code error)
{
    p_enqueue(&ipc_globals->zone_inboundQ, message);
}

static void ipc_process_server_message (ipc_message *message,
					void *context,
					ipc_error_code error)
{
    switch (message->header->type) {
    case IPC_TYPE_REGISTER_NAME_REQUEST:
	ipc_process_register_name_request(message);
	break;

    case IPC_TYPE_LOOKUP_NAME_REQUEST:
	ipc_process_lookup_name_request(message);
	break;
    default:
	ipc_return_message(message);
    }
}

static boolean ipc_initialize_server_ports (void)
{
    ipc_create_named_port("IPC Master:Echo",
			  &ipc_globals->my_echo_port, IPC_PORT_OOB);

    ipc_create_named_port("IPC Master:Control",
			  &ipc_globals->my_server_port, IPC_PORT_OOB);

    ipc_set_receive_callback(ipc_globals->my_server_port,
			     NULL,
			     ipc_process_server_message);

    ipc_set_receive_callback(ipc_globals->my_echo_port,
			     NULL,
			     ipc_process_echo_message);

    return FALSE;
}
/*
 * Get the seat id from the zone manager.
 * For now, we are the zone manager.
 */
static void get_seat_id(void)
{
    ipc_globals->local_seat_addr = IPC_MASTER_SEAT;
}
/*
 * Get the zone id from the realm manager.
 * This mechanism should not use the IPC as a vehicle.
 * For now, do nothing.
 */
static void get_zone_id(void)
{
}
static void register_seat_with_peers(void)
{
}
/*
 * Using the zone id we picked up from the realm manager
 * connect to zone.
 * For now, do nothing.
 */
static boolean connect_to_zone(void)
{
    return(FALSE);
}

static void ipc_init_global_entities(void)
{
    ipc_seat_data *tmp_seat;
    int pid;

    get_seat_id();

    get_zone_id();

    register_seat_with_peers();

    /*
     * Attempt to connect to a "higher" being;
     * If we can't then we're the highest being around .....
     * Startup all the right processes.
     */
    if (connect_to_zone() == FALSE) {
		/* 
		 * Fork the zone manager 
		 */
		ipc_globals->is_ipc_zone_manager = TRUE;
		ipc_create_named_port("IPC Master:Zone",
				  &ipc_globals->zone_manager_port, IPC_PORT_OOB);
		ipc_set_receive_callback(ipc_globals->zone_manager_port,
				     NULL, ipc_process_zone_man_message);
	   	pid = cfork((forkproc *)ipc_zone_manager_process, 0 /* No Context */,
			0 /* Default Stack */,
			"IPC Zone Manager",
			0 /* No TTY */);
	
	   	if (pid == NO_PROCESS)
		    return;
	
		/* 
		 * Create the local realm manager's seat 
		 */
		tmp_seat = ipc_create_seat(IPC_REALM_MANAGER_SEAT, 
						IPC_REALM_MANAGER_NAME);
		if (tmp_seat == NULL) 
			return;
		
		tmp_seat->type = IPC_SEAT_LOCAL;
		tmp_seat->send_vector = ipc_send_local;
		tmp_seat->interrupt_ok = FALSE;
		tmp_seat->seat = IPC_REALM_MANAGER_SEAT;
		ipc_globals->last_seat_addr = IPC_REALM_MANAGER_SEAT;
		ipc_globals->is_ipc_realm_manager = TRUE;
		ipc_add_seat(tmp_seat);
	
		pid = cfork((forkproc *)ipc_realm_manager_process, 0 /* No Context */,
			0 /* Default Stack */,
			"IPC Realm Manager",
			0 /* No TTY */);
	
	   	if (pid == NO_PROCESS)
		    return;
		ipc_globals->realm_manager_pid = pid;
    }
}

static boolean ipc_create_local_seat(void)
{
    ipc_seat_data *master;
    ipc_seat local_seat = ipc_globals->local_seat_addr;

    /* Create the local seat manager's seat */
    master = ipc_create_seat(local_seat, IPC_MASTER_NAME);
    if (master == NULL) return TRUE;

    master->type = IPC_SEAT_LOCAL;
    master->send_vector = ipc_send_local;
    master->interrupt_ok = FALSE;
    master->seat = local_seat;
    ipc_globals->last_seat_addr = local_seat;
    ipc_add_seat(master);
    
    return FALSE;
}

/*
 * Allocate and initialize our global variables.
 */

static boolean ipc_initialize_globals (void)
{
    boolean failure;

    ipc_globals = malloc(sizeof (ipc_global_info));
    if (ipc_globals == NULL) return TRUE;

    ipc_globals->is_ipc_master = TRUE;
    queue_init(&ipc_globals->inboundQ,0);
    ipc_globals->local_seat_addr = 0;
    ipc_globals->last_port = 0;


    failure = ipc_init_port_table() |
	      ipc_init_seat_table() |
	      ipc_init_message_system();

    if (failure) return failure;

    ipc_init_global_entities();

    ipc_create_local_seat();

    TIMER_START(ipc_globals->periodic, IPC_MAINTENANCE_PERIOD);

    mgd_timer_init_parent(&master_timer, NULL);

    return FALSE;
}

/*
 * Callback for eoir notice routine.
 * Note: use this callback because ipc_send_message()
 * blocks until it is acked.
 */
static void ipc_send_eoir_notice_callback (ipc_message *message,
                               void *context,
                               ipc_error_code error)
{
    if (error != IPC_OK)
	errmsg(&msgsym(CANT_SEND,IPC),  ipc_decode_error(error));

    ipc_return_message(message);
}

/*
 * Reset seat info in the case of an OIR event.
 */
static void ipc_send_eoir_notice (ulong slot_num)
{
    ipc_error_code error;
    ipc_slot_init_msg slot_init_msg;

    slot_init_msg.header.type = SLOT_SEAT_INIT;
    slot_init_msg.slot 	      = slot_num;
    error = ipc_send_opaque_async(&slot_init_msg, 
					sizeof(ipc_slot_init_msg),
					ipc_globals->zone_manager_port,
					(ipc_message_type) 0, NULL,
					ipc_send_eoir_notice_callback);
    if (error != IPC_OK)
	errmsg(&msgsym(CANT_SEND,IPC),  ipc_decode_error(error));
}

void ipc_reset_seq_nums (ulong slot_num)
{
    ipc_platform_info *ipc_platform = &ipc_platform_struct;
 
    if (ipc_platform->platform_init_slaves_vector)
        ipc_platform->platform_init_slaves_vector(slot_num);
}

void ipc_change_seat_info (void)
{
    ipc_seat_data *our_seat;
    ipc_seat_data *new_master;
    ipc_seat master_seat;
    char control_port_name[IPC_PORT_NAME_MAX];
    ipc_platform_info *ipc_platform = &ipc_platform_struct;
 
    our_seat = ipc_get_seat_info();
    if (our_seat == NULL) {
        return;
    }
 
    ipc_renumber_seat(our_seat, ipc_platform->platform_seat,
                                ipc_platform->platform_seat_name);
 
    /*
     * Make a new master seat
     */
    master_seat = ipc_portid_to_seat(ipc_platform->master_control_pid);
    new_master = ipc_create_seat(master_seat,
                                 IPC_MASTER_NAME);
    if (new_master == NULL) {
        return;
    }
 
    new_master->type = ipc_platform->type;
    new_master->transport = ipc_platform->transport;
    new_master->interrupt_ok = TRUE;
    new_master->send_vector = ipc_platform->platform_tx_vector;
 
    ipc_add_seat(new_master);
 
    sprintf(control_port_name,"%s%c%s", IPC_MASTER_NAME,
            IPC_PORT_NAME_SEPARATOR, IPC_CONTROL_PORT_NAME);
 
    ipc_create_port_entry(control_port_name, ipc_platform->master_control_pid);

    ipc_globals->local_seat_addr = ipc_platform->platform_seat;
    ipc_globals->last_seat_addr = ipc_platform->platform_seat;
    ipc_globals->is_ipc_master = FALSE;
}

static void ipc_subsys_init (subsystype *subsys)
{
    boolean failure;
    int pid;

	/* 
	 * Init IPC API 
	 */
	reg_add_ipc_send_message_async(ipc_send_message_async,
									"ipc_send_message_async");
	reg_add_ipc_send_message(ipc_send_message,"ipc_send_message");
	reg_add_ipc_send_pak_async(ipc_send_pak_async,"ipc_send_pak_async");
	reg_add_ipc_send_opaque(ipc_send_opaque,"ipc_send_opaque");
	reg_add_ipc_send_opaque_async(ipc_send_opaque_async,
									"ipc_send_opaque_async");
	reg_add_ipc_send_rpc(ipc_send_rpc,"ipc_send_rpc");
	reg_add_ipc_send_rpc_reply_async(ipc_send_rpc_reply_async,
									"ipc_send_rpc_reply_async");
	reg_add_ipc_send_rpc_reply(ipc_send_rpc_reply,"ipc_send_rpc_reply");
	reg_add_ipc_locate_port(ipc_locate_port,"ipc_locate_port");
	reg_add_ipc_create_named_port(ipc_create_named_port,
									"ipc_create_named_port");
	reg_add_ipc_register_port(ipc_register_port,"ipc_register_port");
	reg_add_ipc_close_port(ipc_close_port,"ipc_close_port");
	reg_add_ipc_set_receive_callback(ipc_set_receive_callback,
									"ipc_set_receive_callback");
	reg_add_ipc_set_receive_fast_callback(ipc_set_receive_fast_callback,
									"ipc_set_receive_fast_callback");
	reg_add_ipc_get_message(ipc_get_message,"ipc_get_message");
	reg_add_ipc_return_message(ipc_return_message,"ipc_return_message");
        reg_add_ipc_set_rpc_timeout(ipc_set_rpc_timeout, "ipc_set_rpc_timeout");
    
	failure = ipc_initialize_globals();
    
    if (failure) {
	errmsg(&msgsym(NOMEM,IPC),"IPC system initialization");
    }

    /* Registry setup */
    reg_add_raw_enqueue(LINK_IPC, ipc_process_raw_pak, "ipc_process_raw_pak");
    reg_add_send_ipc_eoir_signal(ipc_send_eoir_notice, "ipc_send_eoir_notice");
    reg_add_ipc_reset_seq_nums(ipc_reset_seq_nums, "ipc_reset_seq_nums");


    /* Hook in parse chains */
    parser_add_command_list(ipc_chain_init_table,"ipc service commands");
    ipc_init_debug();

    pid = cfork((forkproc *)ipc_server_process, 0 /* No Context */,
		0 /* Default Stack */,
		"IPC Seat Manager",
		0 /* No TTY */);

    if (pid == NO_PROCESS)
	return;

    ipc_initialize_server_ports();

}

#define IPC_MAJOR_VERSION 1
#define IPC_MINOR_VERSION 0
#define IPC_EDIT_VERSION 0

SUBSYS_HEADER(ipc,
	      IPC_MAJOR_VERSION, IPC_MINOR_VERSION, IPC_EDIT_VERSION,
	      ipc_subsys_init, SUBSYS_CLASS_KERNEL,
	      NULL, NULL);
