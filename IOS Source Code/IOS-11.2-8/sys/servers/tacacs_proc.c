/* $Id: tacacs_proc.c,v 3.1.14.6 1996/08/28 13:09:02 thille Exp $
 * $Source: /release/112/cvs/Xsys/servers/tacacs_proc.c,v $
 *------------------------------------------------------------------
 * TACACS+ process for reading/writing tacacs+ packets
 *
 * Feb 1996, Lol Grant
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tacacs_proc.c,v $
 * Revision 3.1.14.6  1996/08/28  13:09:02  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.14.5  1996/05/21  10:01:40  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.14.4  1996/05/11  06:23:27  lol
 * CSCdi55924:  show tacacs displays handle(0x0) ??? output with
 * Ciscosecure daemon
 * Branch: California_branch
 *
 * Revision 3.1.14.3  1996/04/29  10:09:17  mdb
 * The 96q1 gcc is much more particular about flagging potentially
 * uninitialized variables [CSCdi56015].
 * Branch: California_branch
 *
 * Revision 3.1.14.2  1996/04/24  03:00:00  lol
 * CSCdi55359:  TACACS events debug output displays handle (0X0) ????
 * Branch: California_branch
 * Cosmetic changes to T+ output formats.
 *
 * Revision 3.1.14.1  1996/04/19  15:29:42  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.1.2.4  1996/04/16  22:05:53  lol
 * Branch: IosSec_branch
 *         Add some comments and do minor cleanups.
 *
 * Revision 3.1.2.3  1996/04/10  23:08:29  lol
 * Branch: IosSec_branch
 *         Incorporate code review comments.
 *
 * Revision 3.1.2.2  1996/04/09  21:45:25  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 3.1.2.1  1996/02/28  06:15:13  lol
 * Branch: IosSec_branch
 * TACACS+ processing.
 *
 * Revision 3.1  1996/02/28  04:44:50  lol
 * Placeholder for IosSec branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "subsys.h"
#include "sys_registry.h"
#include "../h/auth_registry.regh"
#include "../h/cs_registry.regh"
#include "interface_private.h"
#include "address.h"
#include "config.h"
#include "parser.h"
#include "ttysrv.h"
#include "aaa.h"
#include "../servers/tacacs_plus.h"
#include "../servers/tacacs.h"
#include "../util/md5.h"
#include "../tcp/tcp.h"
#include "../ui/debug.h"
#include "../os/os_debug_flags.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_tacacs.h"
#include "../if/network.h"
#include "logger.h"
#include "../util/random.h"
#include "mgd_timers.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../tcp/tcpinternal.h"

/* All tacacs+ plus requests now come through this code. With the
 * advent of Ciscosecure there are now two kinds of mutually exclusive
 * T+ server behaviour. The traditional server behaviour opens and
 * closes separate tcp connections for each T+ session and when
 * quiescent, has no open connections (this is known as a
 * "stay-closed" server). The new behaviour, supported only by
 * CiscoSecure, attempts to maintain a single tcp connection to each
 * daemon. The connection is always open and multiple T+ sessions are
 * multiplexed over them. This is referred to as a "stay-open"
 * server. Note that you can't mix traffic for the two kinds of server
 * indiscriminately, since a stay-closed server must only receive
 * packets belonging to a single T+ session over a given tcp
 * connection. 
 */


typedef struct tac_req_ {
    struct tac_req_ *next;
    watched_boolean *done;	   /* calling process scheduler interface */
    mgd_timer timer;               /* request timeout timer */
    tacacs_plus_pkt_hdr *out_pak;  /* encrypted data to daemon */
    tacacs_plus_pkt_hdr *out_data; /* unencrypted data to daemon */
    tacacs_plus_pkt_hdr *in_data;  /* data from daemon */
    uint allocated;                /* size of in_data area */
    uint bytes_out;                /* bytes written */
    uint bytes_in;                 /* bytes read from handle */
    addrtype *servaddr;		   /* current server for request */
    uint flags;			   /* flags field */
    tcbtype *handle;               /* for stay-closed server requests */
} tac_req;

/*
 * Forward references
 */

/*
 *
 */
#define FOREACH_REQUEST(r) for((r)=tac_plusQ.qhead; (r); (r) = (r)->next)
#define FOREACH_SERVER(s) for((s)=tacacshostQ.qhead; (s); (s) = (s)->next)

/*
 * Local data
 */
static queuetype tac_plusQ; /* queue of reqs from AAA */
static mgd_timer tac_plus_parent_timer; /* timeout parent timer */
static mgd_timer tac_plus_periodic_timer; /* periodic timer */
static pid_t tac_plus_pid = 0;

#define ARBITRARY_BUF_SIZE 2*80
static char dumpreq_buf[ARBITRARY_BUF_SIZE];

#define REQ_TYPE_STRING_SIZE 31
static char req_type_buf[REQ_TYPE_STRING_SIZE];

#define TAC_PLUS_PERIOD (0.1*ONESEC) /* polling interval for handles */

static char *
out_pak_type (tacacs_plus_pkt_hdr *pak)
{
    authen_start_body *start;
    char *act, *type;

    memset(&req_type_buf, '\0', sizeof(req_type_buf));

    switch(pak->type) {
    case TAC_PLUS_AUTHEN:
	if (pak->seq_no == 1) {
	    start = (authen_start_body *)(((char *)pak) 
					  + TACACS_PLUS_HDR_SIZ);
	    switch (start->action) {
	    case AAA_AUTHEN_ACT_LOGIN:
		act = "LOGIN";
		break;
	    case AAA_AUTHEN_ACT_CHPASS:
		act = "CHPASS";
		break;
	    case AAA_AUTHEN_ACT_SENDPASS:
		act = "SENDPASS";
		break;
	    case AAA_AUTHEN_ACT_SENDAUTH:
		act = "SENDAUTH";
		break;
	    default:
		act = "UNKNOWN";
		break;
	    }
	    switch(start->authen_type) {
	    case AAA_AUTHEN_TYPE_NONE: 
		type = "NONE"; 
		break;
	    case AAA_AUTHEN_TYPE_ASCII: 
		type = "ASCII"; 
		break;
	    case AAA_AUTHEN_TYPE_PAP: 
		type = "PAP"; 
		break;
	    case AAA_AUTHEN_TYPE_CHAP: 
		type = "CHAP"; 
		break;
	    case AAA_AUTHEN_TYPE_ARAP: 
		type = "ARAP"; 
		break;
	    default:
		type = "UNKNOWN";
		break;
	    }
	    sprintf(req_type_buf, "AUTHEN/START/%s/%s", act, type);
	} else {
	    if (pak->seq_no % 2) {
		return("AUTHEN/CONT");
	    } else {
		return("AUTHEN/REPLY");
	    }
	}
	break;

    case TAC_PLUS_AUTHOR:
	sprintf(req_type_buf, "AUTHOR/%s", 
		(pak->seq_no == 1) ? "START" : "REPLY");
	break;

    case TAC_PLUS_ACCT:
	if (pak->seq_no == 1) {
	    char *p = "UNKNOWN";
	    acct_request_body *body =
		(acct_request_body *) 
		    (((char *)pak) + TACACS_PLUS_HDR_SIZ);

	    switch(body->flags) {
	    case ACCT_FLAG_START:
		p = "START";
		break;
	    case ACCT_FLAG_STOP:
		p = "STOP";
		break;
	    case ACCT_FLAG_WATCHDOG:
		p = "UPDATE";
		break;
	    default:
		break;
	    }
	    sprintf(req_type_buf, "ACCT/REQUEST/%s", p);
	} else {
	    sprintf(req_type_buf, "ACCT/REPLY");
	}
	break;

    default:
	return("UNKNOWN");
	break;		
    }
    if (req_type_buf[sizeof(req_type_buf)-1] != '\0') {
	buginf("\n%%TAC+: out_pak_type overwrote its internal buffer");
    }
    return(req_type_buf);
}

static char *
dumpreq (tac_req *req)
{
    tacacs_plus_pkt_hdr *data;

    if (!req || !req->out_data) {
	return("NULL request");
    }

    data = req->out_data;
    memset(dumpreq_buf, '\0', sizeof(dumpreq_buf));

    sprintf(dumpreq_buf, "req=%x id=%u ver=%d handle=0x%x (%s) expire=%d %s", 
	    req, data->session_id, data->version, req->handle,
	    (req->handle ? tcp_statestring(req->handle->state) : "NONE"),
	    mgd_timer_left_sleeping(&req->timer)/ONESEC,
	    out_pak_type(data));

    if (dumpreq_buf[sizeof(dumpreq_buf)-1] != '\0') {
	buginf("\n%%TAC+: dumpreq overwrote its internal buffer");
    }
    return(dumpreq_buf);
}

void
show_tacacs(parseinfo *csb)
{
    tac_req *r;
    servtype *server;
    tcbtype *tcb;
    int i;

    for(i=0; i<100; i++) {

	server = queuepick(&tacacshostQ, i);
	if (!server) {
	    break;
	}

	mem_lock(server);
	printf("\n\nServer: %i/%d: ", server->addr, 
	       server->port ? server->port : TACACS_PORT);
	printf("opens=%d closes=%d aborts=%d errors=%d",
	       server->opens, server->closes, server->aborts, server->errors);
	printf("\n\tpackets in=%d packets out=%d",
	       server->paks_in, server->paks_out);
	if (server->handle) {
	    tcb = server->handle;
	    printf("\n\tconnection %x state=%s", tcb, 
		   tcb ? tcp_statestring(tcb->state) : "NULL");
	} else {
	    printf("\n\tno connection");
	}
	free(server);
	
    }

    r = tac_plusQ.qhead; 

    if (r) {
	printf("\n\nTacacs+ request queue:");
    }
    
    for(i=0; i<100; i++) {
	r = queuepick(&tac_plusQ, i);
	if (!r) {
	    break;
	}
	mem_lock(r);
	printf("\n%s", dumpreq(r));
	free(r);
    }
}

/*
 *
 */
tacacs_plus_pkt_hdr *
copy_tac_plus_packet (tacacs_plus_pkt_hdr *pak)
{
    tacacs_plus_pkt_hdr *copy;
    int length;
    
    if (!pak) {
	return(NULL);
    }
    length = pak->length + TACACS_PLUS_HDR_SIZ;
    copy = malloc_named(length, "T+ tmp");
    if (!copy) {
	return(NULL);
    }
    memcpy(copy, pak, length);
    return(copy);
}

/*
 *
 */
static void
req_complete (tac_req *req)
{
    if (tacacs_events_debug) {
	buginf("\nTAC+: %s processed", dumpreq(req));
    }

    if (tacacs_debug) {
	buginf("\nTAC+: (%u) %s processed", 
	       req->out_data->session_id, out_pak_type(req->out_data));
    }

    unqueue(&tac_plusQ, req);

    /* stop its timer */
    mgd_timer_stop(&req->timer);

    /* mark done */
    process_set_boolean(req->done, TRUE);

    /* 
     * If the queue is now empty, stop the periodic timer 
     */
    if (QUEUEEMPTY(&tac_plusQ)) {
	mgd_timer_stop(&tac_plus_periodic_timer);
	if (tacacs_events_debug) {
	    buginf("\nTAC+: periodic timer stopped (queue empty)");
	}
    }
}

servtype *
get_server (addrtype *curraddr)
{
    servtype *server;

    if (!curraddr->ip_addr) {
	buginf("\n%%TAC+: no address for get_server");
	return(NULL);
    }

    FOREACH_SERVER(server) {
	if (curraddr->ip_addr == server->addr) {
	    return(server);
	}
    }
    return(NULL);
}

int 
get_server_port (addrtype *servaddr)
{
    servtype *server = get_server(servaddr);

    if (server && server->port) {
	return(server->port);
    }
    return(TACACS_PORT);
}


char *
get_server_key (addrtype *servaddr)
{
    servtype *server = get_server(servaddr);

    if (server && server->key) {
	return(server->key);
    }
    return(tacacs_plus_key);
}

int
get_server_timeout (addrtype *servaddr)
{
    servtype *server = get_server(servaddr);
    if (server && server->timeout) {
	return(server->timeout * ONESEC);
    }
    return(tacacs_timeout);
}

static tcbtype *
get_server_handle (addrtype *servaddr)
{
    servtype *server = get_server(servaddr);
    return(server->handle);
}

/*
 * Non-blocking packet reading routine.  May be called many
 * times. Tries first to read a header and then to read the body.
 *
 * Read WANTED bytes into hdrp, allocating space if needed, updating
 * READP and ALLOCATEDP accordingly.  READP is a pointer to the total
 * number of bytes read so far.  WANTED is the total number of bytes
 * we want for completion.  Returns the status of the read. 
 */
static int
read_tcb (tcbtype *tcb, tacacs_plus_pkt_hdr **hdrp, 
	     int *readp, int *allocatedp, int wanted)
	    
{
    uchar *datap;
    tacacs_plus_pkt_hdr *hdr, *new_hdr;
    uchar *pak;
    int status;
    int count;

    if (!*hdrp) {
	*hdrp = malloc_named(wanted, "T+ scratch area");
	if (!*hdrp) {
	    return(END_OF_FILE);
	}
	*allocatedp = wanted;
    }

    if (wanted > *allocatedp) {
	new_hdr = realloc(*hdrp, wanted);
	if (!new_hdr) {
	    return(END_OF_FILE);
	}
	*allocatedp = wanted;
	*hdrp = new_hdr;
    }

    hdr = *hdrp;

    status = tcp_getstring(tcb, &datap, &count, wanted - *readp);
    if (status == OK) {
	pak = (uchar *) *hdrp;
	memcpy(&pak[*readp], datap, count);
	*readp += count;
	if (tacacs_events_debug) {
	    buginf("\nTAC+: %i %s read=%d wanted=%d alloc=%d got=%d",
		   tcb->foreignhost.ip_addr, 
		   tcp_statestring(tcb->state),
		   *readp, wanted, *allocatedp, count);
	}
    }
    return(status);
}
/*
 * Allocate space and read a packet header followed by a packet body
 * into in_pak, adjusting readp and allocatedp to track space
 * allocated and total bytes read. 
 */
static int
read_packet (tcbtype *tcb, tacacs_plus_pkt_hdr **in_pak, int *readp,
	    int *allocatedp)
{
    int status = 0;

    if (*readp < TACACS_PLUS_HDR_SIZ) {
	/* read header */
	status = read_tcb(tcb, in_pak, readp, allocatedp, TACACS_PLUS_HDR_SIZ);
    }

    if (*readp >= TACACS_PLUS_HDR_SIZ) {
	/* read body */
	status = read_tcb(tcb, in_pak, readp, allocatedp, 
			  (*in_pak)->length + TACACS_PLUS_HDR_SIZ);
    }

    if (status == END_OF_FILE) {
	if (tacacs_events_debug) {
	    buginf("\nTAC+: %i read END-OF-FILE", tcb->foreignhost.ip_addr);
	}
    }
    return(status);
}

/*
 * Single-connection server's handle has closed.
 */
static void
server_handle_closed (addrtype *servaddr)
{
    tac_req *req, *next;
    tcbtype *saved_handle, *tcb = NULL;
    servtype *server;

    /* 
     * Allocate a new handle.  Any partly written req should be marked
     * done since it may be the cause of the closure. An alternative
     * strategy might be to try and re-send it, but this seems unwise
     * if it's a poison packet. 
     */

    server = get_server(servaddr);
    if (!server) {
	return;
    }

    saved_handle = server->handle;
    tcb = server->handle;
    close_handle((void **)&tcb);

    server = get_server(servaddr);
    if (!server) {
	return; /* deleted ? */
    }
    server->closes++;
    server->handle = NULL;

    /* Can't reconnect to server? All requests for it are doomed */
    if (!open_handle(servaddr, (void **)&tcb)) {
	server = get_server(servaddr);
	if (server) {
	    server->errors++;
	}
	req = tac_plusQ.qhead;
	while(req) {
	    next = req->next;
	    if (req->servaddr->ip_addr == servaddr->ip_addr) {
		req_complete(req);
	    }
	    req = next;
	}
	return;
    }

    server = get_server(servaddr);
    if (!server) {
	return; /* deleted ? */
    }
    server->handle = tcb;
    server->opens++;

    FOREACH_REQUEST(req) {
	/* Find any part-written req for this server */
	if ((req->servaddr->ip_addr == servaddr->ip_addr) && 
	    (req->bytes_out > 0)) {
	    /* If it's a start-packet, just requeue it.
	     * Otherwise, we were in the middle of a conversation
	     * and thus have (presumably) lost the context.
	     *
	     * Also make sure we don't requeue too many times (prevent loops)
	     */
	    if (req->out_data->seq_no == 1) {
		if (tacacs_events_debug) {
		    buginf("\nTAC+: %i partly processed START req %x "
			   "requeued after unexpected handle 0x%x closure.",
			   servaddr->ip_addr, req, saved_handle);
		}
		req->bytes_out = 0;
	    } else {
		if (tacacs_events_debug) {
		    buginf("\nTAC+: %i partly processed req %x aborted"
			   " after unexpected handle 0x%x closure",
			   servaddr->ip_addr, req, saved_handle);
		}
		req_complete(req);
	    }
	    return;
	}
    }
}

/*
 * Do a non-blocking read into the server area keeping track of how
 * much has been read.  If and when a packet is fully read, find and
 * process the matching request. 
 */
static void
server_readable (addrtype *servaddr)
{
    tac_req *req;
    tcbtype *tcb;
    int status;
    boolean ret;
    servtype *server = get_server(servaddr);

    if (!server) {
	return; /* deleted ? */
    }

    tcb = server->handle;
    if (!tcb) {
	ret = open_handle(servaddr, (void **)&tcb);
	server = get_server(servaddr);
	if (!server) {
	    /* server deleted ? */
	    return;
	}
	if (!ret) {
	    server->errors++;
	    buginf("\n%%TAC+: cannot open handle for %i", servaddr);
	    return;
	}
	server->opens++;
	server->handle = tcb;
    }

    status = read_packet(tcb, 
		&server->in_data, 
		&server->bytes_in,
		&server->allocated);

    if (status == END_OF_FILE) {
	/* This can block. Should we throw away any data so far? */
	server_handle_closed(servaddr);
    }

    server = get_server(servaddr);
    if (!server) {
	return; /* deleted ? */
    }

    if (server->bytes_in < TACACS_PLUS_HDR_SIZ) {
	/* Packet header not fully assembled yet */
	return;
    }

    if (server->bytes_in < (server->in_data->length + TACACS_PLUS_HDR_SIZ)) {
	/* Packet body not fully assembled yet */
	return;
    }

    server->paks_in++;

    /* Incoming packet fully assembled. Find matching req */
    FOREACH_REQUEST(req) {
	if (req->out_pak->session_id == server->in_data->session_id) {
	    break;
	}
    }

    if (!req) {
	if (tacacs_events_debug) {
	    buginf("\nTAC+: %i unexpected packet id=%u ignored",
		   server->addr,
		   server->in_data->session_id);
	}

	/* toss data and start over */
	server->bytes_in = 0;
	server->errors++;
	return;
    }

    if (tacacs_events_debug) {
	/* Incoming packet fully assembled */
	buginf("\nTAC+: %i received %d byte reply for %x",
	       tcb->foreignhost.ip_addr, server->bytes_in, req);
    }

    req->in_data = copy_tac_plus_packet(server->in_data);
    /* Get ready for next packet. No point freeing our scratch area */
    server->bytes_in = 0;

    req_complete(req);
}
/*
 * The req for this single-connection server hasn't been fully written.
 * Write as many bytes as possible, updating the req to say how much
 * has been written. 
 */
static void
server_writable (tac_req *req)
{
    tcbtype *tcb;
    int want_out, n;
    uchar *pak;
    servtype *server;
    addrtype *servaddr;
    boolean ret;

    want_out = req->out_pak->length + TACACS_PLUS_HDR_SIZ;
    if (want_out <= req->bytes_out) {
	buginf("\n%%TAC+: no work for %x", req);
	return;
    }

    servaddr = req->servaddr;
    tcb = get_server_handle(servaddr);

    if (!tcb) {
	/* can block */
	ret = open_handle(servaddr, (void **)&tcb);
	server = get_server(servaddr);
	if (!server) {
	    return;
	}
	if (!(ret && tcb)) {
	    server->errors++;
	    buginf("\n%%TAC+: cannot open handle for %x", req);
	    return;
	}
	server->opens++;
	server->handle = tcb;
    }

    pak = (uchar *)req->out_pak;
    n = tcp_putstring(tcb, &pak[req->bytes_out], want_out - req->bytes_out);
    req->bytes_out += n;

    if (tacacs_events_debug) {
	buginf("\nTAC+: %i %s %x wrote %d of %d bytes", 
	       tcb->foreignhost.ip_addr,
	       tcp_statestring(tcb->state), req, n, want_out);
    }
    
    tcp_putpush(tcb, TRUE, FALSE); /* This can block */

    if (req->bytes_out >= want_out) {
	server = get_server(req->servaddr);
	if (server) {
	    server->paks_out++;
	} else {
	    /* Server got deleted. Don't expect any more data from it */
	    req_complete(req);
	    return;
	}
	if (req->flags == TAC_PLUS_EXPECT_NO_REPLY) {
	    req_complete(req);
	}
    }
}

/*
 * Try to write as many bytes as possible of a req to a private
 * handle, updating the req to say how much has been written. If
 * completely written, try reading a reply. 
 */
static void
req_poll (tac_req *req)
{
    tcbtype *tcb;
    int want_out, n;
    uchar *pak;
    int status;
    servtype *server;

    tcb = req->handle;
    if (!tcb) {
	buginf("\n%%TAC+: req_writable -- no handle for %x", req);
	return;
    }
    
    want_out = req->out_pak->length + TACACS_PLUS_HDR_SIZ;
    
    if (req->bytes_out < want_out) {
	pak = (uchar *)req->out_pak;
	n = tcp_putstring(tcb, &pak[req->bytes_out], want_out - req->bytes_out);
	req->bytes_out += n;
	tcp_putpush(tcb, TRUE, FALSE); /* This can block */

	if (tacacs_events_debug) {
	    buginf("\nTAC+: %i %s %x wrote %d of %d bytes", 
		   tcb->foreignhost.ip_addr,
		   tcp_statestring(tcb->state), req, n, want_out);
	}

	if (req->bytes_out >= want_out) {
	    server = get_server(req->servaddr);
	    if (server) {
		server->paks_out++;
	    }
	}
    }

    if (req->bytes_out >= want_out) {
	/* request fully written. Try reading reply */

	if (req->flags == TAC_PLUS_EXPECT_NO_REPLY) {
	    /* no reply wanted/expected */
	    req_complete(req);
	    return;
	}

	status = read_packet(tcb, 
			     &req->in_data, 
			     &req->bytes_in,
			     &req->allocated);
	
	if (status == END_OF_FILE) {
	    req_complete(req);
	    return;
	}

	if (req->bytes_in < TACACS_PLUS_HDR_SIZ) {
	    /* Packet header not fully assembled yet */
	    return;
	}

	if (req->bytes_in < (req->in_data->length + TACACS_PLUS_HDR_SIZ)) {
	    /* Incoming packet not fully assembled yet */
	    return;
	}
	
	server = get_server(req->servaddr);
	if (server) {
	    server->paks_in++;
	}

	if (tacacs_events_debug) {
	    /* Incoming packet fully assembled */
	    buginf("\nTAC+: %i received %d byte reply for %x",
		   tcb->foreignhost.ip_addr, req->bytes_in, req);
	}
	req_complete(req);
    }
}
/*
 * A req is ready to go to a single-connection server if no unwritten
 * request precedes it. 
 */
static boolean
ready_to_go (tac_req *req)
{
    ipaddrtype servipaddr;
    tac_req *r;

    servipaddr = req->servaddr->ip_addr;

    if (req->handle) {
	buginf("\n%%TAC+: bad req for ready_to_go %x", req);
	return(FALSE);
    }

    FOREACH_REQUEST(r) {
	/* which unwritten req is next for this single-connection server? */
	if (r->servaddr->ip_addr == servipaddr &&
	    (req->bytes_out < req->out_data->length + TACACS_PLUS_HDR_SIZ)) {
	    break;
	}
    }

    /* Is it us? */
    if (r == req) {
	return(TRUE);
    }
    return(FALSE);
}

/*
 * This periodic timer drives the whole engine until such time as we
 * can rely on tcp to tell us when we can read/write/close
 * connections.
 *
 * Periodically, and we poll all requests doing as much reading and
 * writing as possible, trying never to block. 
 */
static void
periodic_timer (void)
{
    servtype *server, *next_server;
    tac_req *req, *next_req;
    addrtype servaddr;

    if (!tac_plusQ.qhead) {
	return;
    }

    /* 
     * Try and do non-blocking writes for as many requests as
     * possible. Try non-blocking reads for written reqs which have
     * their own handles.
     */
       
    req = tac_plusQ.qhead;

    while(req) {
	next_req = req->next;

	/* 
	 * Try and complete the request. This may free it.
	 */

	if (req->handle) {
	    req_poll(req);
	} else {
	    /* request is bound for a single-connection server */
	    if (ready_to_go(req)) {
		server_writable(req);
	    }
	}
	req = next_req;
    }

    /* 
     * Lastly, poll all single-connection servers for incoming data 
     */
    server = (servtype *)tacacshostQ.qhead; 
    while (server) {
	next_server = server->next;
	if (server->single_connection) {
	    servaddr.type = ADDR_IP;
	    servaddr.addr = server->addr;
	    server_readable(&servaddr);
	}
	server = next_server;
    }
}

/*
 *
 */
static void
message_event (void)
{
    ulong message, value;
    void *pointer;

    while (process_get_message(&message, &pointer, &value)) {
	errmsg(&msgsym(UNEXPECTEDMESSAGE,SCHED), 
	       message, pointer, value);
    }
}

/*
 * Come here when a req times out
 */
static void
timer_event (void)
{
    mgd_timer *expired_timer;
    tac_req *req;
    servtype *server;

    while (mgd_timer_expired(&tac_plus_parent_timer)) {
	expired_timer = mgd_timer_first_expired(&tac_plus_parent_timer);
	req = mgd_timer_context(expired_timer);

	if (!req) {
	    /* periodic timer */
	    periodic_timer();
	    mgd_timer_update(&tac_plus_periodic_timer, TAC_PLUS_PERIOD);
	    continue;
	}

	/* A req has timed out */
	if (tacacs_debug || tacacs_events_debug) {
	    buginf("\nTAC+: %i (%u) %s -- TIMED OUT", 
		   req->servaddr->ip_addr, 
		   req->out_data->session_id,
		   out_pak_type(req->out_data));
	}

	/* 
	 * If this request is only partly written to a stay-open
	 * server handle, we need to close the handle lest the remote
	 * server gets out of sync. 
	 */

	if (!req->handle && 
	    (req->bytes_out < (req->out_data->length + TACACS_PLUS_HDR_SIZ))) {
	    server = get_server(req->servaddr);
	    server->closes++;
	    server->bytes_in = 0;
	    close_handle((void **)&server->handle);
	}
	req_complete(req);
    } /* while */
}
/*
 * The T+ packet processing "engine"
 */
static process 
tac_plus_process (void)
{
    ulong major, minor;
    
    process_watch_mgd_timer(&tac_plus_parent_timer, ENABLE);

    while (TRUE) {
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {

	    case QUEUE_EVENT:
		errmsg(&msgsym(UNEXPECTEDQUEUE,SCHED), minor);
		break;

	    case TIMER_EVENT:
		timer_event();
		break;

	    case MESSAGE_EVENT:
		message_event();
		break;

	    default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}

/*
 * Enqueue a req for a particular server. Return FALSE if this cannot
 * be done.
 */
static boolean
req_enqueue (tac_req *req, addrtype *servaddr)
{
    tcbtype *tcb;
    boolean ret;
    servtype *server = get_server(servaddr);

    if (!server) {
	/* server deleted? */
	if (tacacs_debug || tacacs_events_debug) {
	    buginf("\nTAC+: %i -- request for nonexistent server", 
		   servaddr->ip_addr);
	}
	return(FALSE);
    }

    if (!(req->handle || server->handle)) {
	ret = open_handle(servaddr, (void **)&tcb);

	server = get_server(servaddr);
	if (!server) {
	    return(FALSE);
	}
	if (!(ret && tcb)) {
	    server->errors++;
	    return(FALSE);
	}
	server->handle = tcb;
	server->opens++;
    }

    /* start request timer */
    mgd_timer_start(&req->timer, get_server_timeout(servaddr));

    req->servaddr = servaddr;

    enqueue(&tac_plusQ, req);

    /* 
     * Ensure the periodic timer is running 
     */
    if (!mgd_timer_running(&tac_plus_periodic_timer)) {
	mgd_timer_start(&tac_plus_periodic_timer, TAC_PLUS_PERIOD);
	if (tacacs_events_debug) {
	    buginf("\nTAC+: periodic timer started");
	}
    }
    
    if (tacacs_events_debug) {
	buginf("\nTAC+: %i %s queued", servaddr->ip_addr, dumpreq(req));
    }

    if (tacacs_debug) {
	buginf("\nTAC+: %i (%u) %s queued", 
	       servaddr->ip_addr,  req->out_data->session_id,
	       out_pak_type(req->out_data));
    }

    return(TRUE);
}

boolean
stay_open_server (addrtype *addr, void *handle)
{
    servtype *server = get_server(addr);
    if (!server) {
	return(FALSE);
    }
    return(server->single_connection);
}


/*
 * Send T+ packet and wait for it to be processed 
 */
boolean
tacacs_plus_proc_send_receive (tcbtype *tcb,
			       tacacs_plus_pkt_hdr *out_data,
			       tacacs_plus_pkt_hdr **in,
			       addrtype *addr,
			       uint reply_wanted)
{
    tac_req *req;
    tacacs_plus_pkt_hdr *out_pak;
    ulong major, minor;

    *in = NULL;

    /* start setting up the req */
    req = malloc_named(sizeof(tac_req), "T+ req");
    if (!req) {
	return(FALSE);
    }

    out_pak = copy_tac_plus_packet(out_data);
    if (!out_pak) {
	return(FALSE);
    }

    if (!tacacs_plus_encrypt(addr, out_pak)) {
	free(out_pak);
	return(FALSE);
    }

    req->out_pak = out_pak;
    req->out_data = out_data;
    req->flags = reply_wanted;
    req->handle = tcb;

    if (tac_plus_pid == 0) {

	queue_init(&tac_plusQ, 0);

	mgd_timer_init_parent(&tac_plus_parent_timer, NULL);

	mgd_timer_init_leaf(&tac_plus_periodic_timer, 
			    &tac_plus_parent_timer, 
			    0, 0, FALSE);

	tac_plus_pid = process_create(tac_plus_process, "TACACS+", 
				      NORMAL_STACK, PRIO_NORMAL);	
	if (tac_plus_pid == NO_PROCESS) {
	    buginf("\n%%TAC+: failed to start T+ process");
	    free(req);
	    free(out_pak);
	    return(FALSE);
	}
	mgd_timer_start(&tac_plus_periodic_timer, TAC_PLUS_PERIOD);
    }

    mgd_timer_init_leaf(&req->timer, &tac_plus_parent_timer, 
			0, req, FALSE);

    req->done = create_watched_boolean("T+ req", 0);
    process_set_boolean(req->done, FALSE);

    process_push_event_list(NULL);

    process_watch_boolean(req->done, ENABLE, ONE_SHOT);

    if (!req_enqueue(req, addr)) {
	/* Couldn't queue -- fatal */
	if (tacacs_events_debug) {
	    buginf("\nTAC+: %i cannot queue %s", addr->ip_addr, dumpreq(req));
	}
	mgd_timer_stop(&req->timer);

	process_watch_boolean(req->done, DISABLE, ONE_SHOT);
	process_pop_event_list(NULL);
	delete_watched_boolean(&req->done);

	free(out_pak);
	free(req);
	return(FALSE);
    }

    process_wait_for_event();
    while (process_get_wakeup(&major, &minor)) {
	switch (major) {
	    
	case BOOLEAN_EVENT:
	    if (!process_get_boolean(req->done)) {
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		continue;
	    }
	    break;
		
	default:
	    errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
	    process_wait_for_event();
	}
    }
    
    process_watch_boolean(req->done, DISABLE, ONE_SHOT);
    process_pop_event_list(NULL);
    delete_watched_boolean(&req->done);

    req->out_data = NULL;

    if (req->in_data) {
	*in = req->in_data;
	req->in_data = NULL;
	free(out_pak);
	free(req);
	return(TRUE);
    }

    if (!req->in_data && (req->flags == TAC_PLUS_EXPECT_NO_REPLY)) {
	free(out_pak);
	free(req);
	return(TRUE);
    }

    free(out_pak);
    free(req);
    return(FALSE);
}
