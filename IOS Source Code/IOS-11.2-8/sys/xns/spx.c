/* $Id: spx.c,v 3.6.20.3 1996/06/03 20:24:00 john Exp $
 * $Source: /release/112/cvs/Xsys/xns/spx.c,v $
 *------------------------------------------------------------------
 * spx.c   Sequenced Packet Exchange Protocol  (C code)
 *
 * September 1995, Syed Mohammad Irfan Ashraf
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: spx.c,v $
 * Revision 3.6.20.3  1996/06/03  20:24:00  john
 * CSCdi58748:  ipxwan and nlsp command nvgen order wrong and subsys issues
 * Branch: California_branch
 *
 * Revision 3.6.20.2  1996/05/21  10:11:50  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.20.1  1996/03/18  22:54:10  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.2  1996/03/07  11:22:45  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.1  1996/02/20  22:01:27  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/01/29  07:32:02  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.5  1996/01/22  07:53:13  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1996/01/18  15:58:42  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.3  1995/12/17  18:42:40  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  19:24:44  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:00:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:39:58  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/26  11:57:13  irfan
 * Sequenced Packet Exchange Protocol: holding area
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Implement the SPX protocol.
 * Doesn't recognize any of SPX-II protocol
 *
 * This is a dirty and minimal SPX implementation. We expect to
 * replace it with an improved one in near future, or may be we
 * might just fix whatever problems we find. At the moment this
 * is not cisco standard code, but it works.
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "subsys.h"
#include "sys_registry.h"
#include "../h/cs_registry.regh"
#include "address.h"
#include "interface_private.h"
#include "packet.h"
#include "ttysrv.h"
#include "logger.h"
#include "ieee.h"
#include "../os/free.h"
#include "xns.h"
#include "novell.h"
#include "novell_public.h"
#include "xnsfast.h"
#include "novfast.h"
#include "novigrp.h"
#include "novisis.h"
#include "novnlsp.h"
#include "config.h"
#include "access.h"
#include "../if/network.h"
#include "parser.h"
#include "../os/buffers.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../clns/clns.h"
#include "../clns/isis_externs.h"
#include "../clns/clns_externs.h"
#include "../clns/isis_ipx.h"
#include "../wan/snapshot.h"
#include "../if/if_vidb.h"
#include "../xns/ipx_registry.h"
#include "../xns/spx.h"
#include "../ui/debug.h"
#include "../xns/spx_debug.h"


/*
 * Local Variables
 */
boolean spx_running;                          /* Is SPX installed ? */
static queuetype novell_spxQ;                 /* SPX packets queue */
queuetype spxsockQ;                           /* Current known sockets */
static int spx_pid = 0;                       /* spx_input process id */
sys_timestamp spx_wake_time;                  /* nearest spx processing */
static ushort spx_new_cur_con = 0x2000;

/*
 * Forwared declarations
 */
static boolean spx_inputpak(paktype *);
static forktype spx_input(void);
static void spx_process_packet(paktype *, spxsocktype *);
static spxcontype *get_spxcon(spxsocktype *, ushort);
static void spx_new_connection(paktype *, spxsocktype *);
static ushort spx_gen_new_con(void);
static boolean spx_send_pak(spxcontype *, paktype *, uchar, uchar, int,
		     uchar *, paktype *);
static void spx_abort_con(spxcontype *, paktype *);
static void spx_close_con(spxcontype *, paktype *);
static void spx_process_ack(spxcontype *, spxhdr *);
static void spx_rcvd_data(spxcontype *, paktype *);
void spx_send_watchdog_pak(spxcontype *);
static void spx_send_repeat_ack(spxcontype *);
static void spx_new_timer(sys_timestamp *, ulong);
static void spx_purge_con(spxcontype *);

void spx_setup_parser(void);


#define spx_send_ack(spxcon, pak) spx_send_pak(spxcon, pak, \
					       SPX_CONCTL_SYS, \
					       0, 0, NULL, NULL)


static void spx_init (subsystype *subsys)
{

    reg_add_ipx_input_paktype(NOVELL_TYPE_SPX, spx_inputpak,
			      "SPX input pak");
    spx_setup_parser();
    spx_debug_init();
    queue_init(&novell_spxQ, 0);
    queue_init(&spxsockQ, 0);
    TIMER_STOP(spx_wake_time);
    if (spx_pid <= 0) {
	spx_pid = cfork(spx_input, 0L, 0, "SPX Input", 0);
    }
    if (spx_pid > 0) {
	spx_running = TRUE;
    }
}

static spxsocktype *my_localspxsock (ushort socket)
{
    spxsocktype *spxsock;

    FOR_ALL_SPX_SOCKETS(spxsock) {
	if (spxsock->sock_num == socket) {
	    return (spxsock);
	}
    }
    return NULL;
}

static boolean spx_inputpak (paktype *pak)
{

    spxhdr *spx;

    if (!spx_running) {
	if (spx_debug) {
	    buginf("\nSPX: input pkt dropped. SPX not available");
	}
	return (FALSE);
    }
    spx = (spxhdr *)ipxheadstart(pak);
    /*
     * Are we listening on this IPX socket
     */
    if (!my_localspxsock(spx->dsock)) {
	return (FALSE);
    }
    clear_if_input(pak);
    pak_enqueue(&novell_spxQ, pak);
    return (TRUE);
}


static boolean spxBLOCK (ulong dummy)
{
    if ((QUEUESIZE(&novell_spxQ)) ||
	(TIMER_RUNNING_AND_AWAKE(spx_wake_time))) {
	return (FALSE);
    }
    return (TRUE);
}

/*
 * spx_input
 * SPX background processing
 */
static forktype spx_input (void)
{
    int count, len;
    paktype *pak;
    spxhdr *spx;
    spxsocktype *spxsock;
    spxcontype *spxcon;

    /* wait until system initialized */
    process_wait_on_system_init();

    while (TRUE) {
	edisms((blockproc *)spxBLOCK, 0);
	if (TIMER_RUNNING_AND_AWAKE(spx_wake_time)) {
	    FOR_ALL_SPX_SOCKETS(spxsock) {
		FOR_ALL_SPXCON_ON_SOCK(spxsock, spxcon) {
		    if ((SPXCON_ST_PURGING == spxcon->state) &&
			TIMER_RUNNING_AND_AWAKE(spxcon->purge_timer)) {
			if (spx_debug) {
			    buginf("\nSPX: purge timer fired. Cleaning up %x",
				   spxcon);
			}
			spx_purge_con(spxcon);
			/*
			 * connection queue manipulated. Go back to head
			 */
			if (NULL == (spxcon = spxsock->spxconQ.qhead)) {
			    break;
			}
		    } else if ((SPXCON_ST_OPEN == spxcon->state) &&
			       TIMER_RUNNING_AND_AWAKE(spxcon->send_verify_watchdog)) {
			spx_send_repeat_ack(spxcon);
		    }
		}
	    }
	}

	count = PAK_SWITCHCOUNT;
	while (TRUE) {
	    /*
	     * Switch only a limited number of packets per pass
	     */
	    if (--count == 0) {
		break;
	    }
	    /*
	     * Dequeue the next SPX packet
	     */
	    pak = pak_dequeue(&novell_spxQ);
	    if (!pak) {
		break;
	    }
	    spx = (spxhdr *)ipxheadstart(pak);

	    len = spx->len;
	    /*
	     * Sanity check length 
	     */
	    if ((len < SPX_MINSIZE)) {
		if (spx_debug) {
		    buginf("\nSPX: packet too short");
		}
		datagram_done(pak);
		continue;
	    }
	    /*
	     * Are we listening on this IPX socket
	     */
	    spxsock = my_localspxsock(spx->dsock);
	    if (spxsock) {
		/*
		 * should be control processing now.
		 */
		spx_process_packet(pak, spxsock);
	    } else {
		if (spx_debug) {
		    buginf("\nSPX: no local socket ******");
		}
	    }
	    datagram_done(pak);
	}
    }
}


spxsocktype *spx_api_listen_socket (ushort sock_number, ulong flags,
				    spx_listen_cb open_callback,
				    spx_close_cb close_callback)
{
    spxsocktype *spxsock;

    if (!spx_running) {
	return (NULL);
    }
    if (my_localspxsock(sock_number)) {
	/*
	 * don't recreate the socket if it exists
	 */
	return (NULL);
    }
    spxsock = malloc_named(sizeof(spxsocktype), "SPX socket entry");
    if (spxsock) {
	spxsock->sock_num = sock_number;
	spxsock->flags = flags;
	spxsock->open_cb = (long) open_callback;
	spxsock->close_cb = (long) close_callback;
	queue_init(&spxsock->spxconQ, 0);
	p_enqueue(&spxsockQ, spxsock);
	return (spxsock);
    }
    return (NULL);
}


static void spx_process_packet (paktype *pak, spxsocktype *spxsock)
{

    spxhdr *spx;
    spxcontype *spxcon;

    spx = (spxhdr *)ipxheadstart(pak);

    if (spx_debug) {
	buginf("\nSPX: I Con Src/Dst %x/%x"
	       "  d-strm %x  con-ctl %x",
	       spx->sourceconnectid, spx->destconnectid,
	       spx->datastreamtype, spx->connectionctl);
    }

    if (spx->len < sizeof(spxhdr)) {
	if (spx_debug) {
	    buginf("\nSPX: incoming packet too short");
	}
	return;
    }
    spxsock = my_localspxsock(spx->dsock);
    if (!spxsock) {    /* Whoops! */
	return;        /*
			* Probably the sock was deleted b/w the
			* time we enqueued the pack, and the time
			* this process took a peek.
			*/
    }
    spxcon = get_spxcon(spxsock, spx->destconnectid);
    if (spxcon) {
	/*
	 * restart the abort timer
	 */

	spx_new_timer(&spxcon->abort_timer, SPX_ABORT_WATCHDOG_TIMER);

	spx_process_ack(spxcon, spx);

	switch (spxcon->state) {

	case SPXCON_ST_INITIAL:         /* ???? */
	    break;

	case SPXCON_ST_OPEN:
	    if (SPX_DATASTREAM_EOM_REQ == spx->datastreamtype) {
		spx_close_con(spxcon, pak);
		break;
	    } else if ((!(spx->connectionctl & SPX_CONCTL_SYS)) &&
		       (sizeof(spxhdr) < spx->len)) {
		spx_rcvd_data(spxcon, pak);
	    }
	    if (spx->connectionctl & SPX_CONCTL_NEED_ACK) {
		spx_send_ack(spxcon, NULL);
	    }
	    break;

	case SPXCON_ST_CLOSING:         /* We want to close this con */
	    if (SPX_DATASTREAM_EOM_REQ == spx->datastreamtype) {
		if (spx_debug) {
		    buginf("\nSPX: EOM req received. Informed orderly close");
		}
		spx_abort_con(spxcon, pak);
	    } else {
		if (spx_debug) {
		    buginf("\nSPX: incoming pkt ignored. Closing state");
		}
	    }
	    break;

	case SPXCON_ST_PURGING:         /* Mandatory T/O before deleting con */
	    if (SPX_DATASTREAM_EOM_ACK == spx->datastreamtype) {
		if (spx_debug) {
		    buginf("\nSPX: EOM ack received. Orderly Informed"
			   " Disconnect");
		}
		spx_purge_con(spxcon);
	    } else {
		if (spx_debug) {
		    buginf("\nSPX: packets received in purging state");
		}
		spx_rcvd_data(spxcon, pak);
		if (spx->connectionctl & SPX_CONCTL_NEED_ACK) {
		    spx_send_ack(spxcon, NULL);
		}
	    }
	    break;

	default:
	    if (spx_debug) {
		buginf("\nSPX: Bad connection state");
	    }
	    break;
	}

    } else {
	if (SPX_CONID_CONREQUEST == spx->destconnectid) {
	    if (SPX_CONCTL_NEWCON == spx->connectionctl) {
		/*
		 * do the open handshake for the new connection
		 * --> only if we never processed a prev one on this
		 *     connection.
		 */
		FOR_ALL_SPXCON_ON_SOCK(spxsock, spxcon) {
		    if (spxcon->remote_con == spx->sourceconnectid) {
			return;
		    }
		}
		spx_new_connection(pak, spxsock);
	    }
	}
    }
}

static spxcontype *get_spxcon (spxsocktype *spxsock, ushort con)
{

    spxcontype *spxcon;

    for (spxcon = ((spxcontype *) spxsock->spxconQ.qhead); spxcon;
	 spxcon = spxcon->next) {
	if (spxcon->local_con == con) {
	    return (spxcon);
	}
    }
    return (NULL);
}

static boolean spxcon_number_inuse (ushort con_number)
{
    /*
     * scan the list of all connections
     * and see if this number is in use.
     *
     * I will rather wait for SATYA to fix this.
     */
    return (FALSE);
}

static ushort spx_gen_new_con (void)
{
    spx_new_cur_con += 0x10;
    while (spxcon_number_inuse(spx_new_cur_con))
	spx_new_cur_con += 0x10;
    return (spx_new_cur_con);
}

static void spx_new_connection (paktype *pak, spxsocktype *spxsock)
{
    spxcontype *spxcon;
    spxhdr *spx;

    if (spx_debug) {
	buginf("\nSPX: new connection request for listening socket");
    }
    spxcon = malloc_named(sizeof(spxcontype), "SPX CON");
    if (!spxcon) {
	return;    /* can't do much */
    }

    if (!((spx_listen_cb)(spxsock->open_cb))(spxsock, spxcon)) {
	if (spx_debug) {
	    buginf("\nSPX: socket connection refused  ****");
	}
	free(spxcon);
	return;
    }
    spx = (spxhdr *)ipxheadstart(pak);

    spxcon->remote_con = spx->sourceconnectid;
    spxcon->remote_net = GET2WD(spx->ssnet);
    ieee_copy(spx->shost, spxcon->remote_host);
    spxcon->remote_sock = spx->ssock;

    spxcon->local_con = spx_gen_new_con();
    spxcon->local_net = GET2WD(spx->ddnet);
    ieee_copy(spx->dhost, spxcon->local_host);
    spxcon->local_sock = spx->dsock;

    spxcon->local_next_seq = 0;
    spxcon->remote_next_seq = spx->sequencenumber;
    spxcon->local_alloc_num = SPX_ALLOCATION_UNITS;

    queue_init(&spxcon->inQ, 0);
    queue_init(&spxcon->outQ, 0);
    queue_init(&spxcon->unackedQ, 0);
    spxcon->flags = spxsock->flags;
    spxcon->idb = pak->if_input;
    spxcon->nidb = pak->nif_input;
    spxcon->spxsock = spxsock;
    p_enqueue(&spxsock->spxconQ, spxcon);
    spxcon->state = SPXCON_ST_OPEN;
    spx_send_ack(spxcon, pak);
}


static boolean spx_send_pak (spxcontype *spxcon, paktype *pak,
		      uchar spxconctl, uchar spxdatastream,
		      int data_size, uchar *data_bytes,
		      paktype *out_pak)
{
    spxhdr *spx;

    /*
     * we are acking something
     * let's update our acknumber
     */
    if (pak) {
	spx = (spxhdr *)ipxheadstart(pak);
	if (spx->len > sizeof(spxhdr)) {
	    if (spx_debug) {
		buginf("\nSPX: sending ack");
	    }
	    spxcon->remote_next_seq = spx->sequencenumber + 1;
	}
    }

    if (data_size) {
	if (SPXCON_MAX_OUT_PENDING <= (QUEUESIZE(&spxcon->unackedQ) +
				       QUEUESIZE(&spxcon->outQ))) {
	    if (spx_debug) {
		buginf("\nSPX: spx_send packet dropped. Queue overflow");
	    }
	    return (FALSE);
	}
    }

    pak = out_pak;
    if (!out_pak) {
	pak = getbuffer(sizeof(spxhdr)+data_size);
    }
    if (!pak) {
	return (FALSE);
    }

    /*
     * fill in IPX info
     */
    spx = (spxhdr *)ipxheadstart(pak);
    spx->pt = NOVELL_TYPE_SPX;
    PUT2WD(spx->ddnet, spxcon->remote_net);
    ieee_copy(spxcon->remote_host, spx->dhost);
    spx->dsock = spxcon->remote_sock;
    PUT2WD(spx->ssnet, spxcon->local_net);
    ieee_copy(spxcon->local_host, spx->shost);
    spx->ssock = h2n(spxcon->local_sock);
    spx->cksum = h2n(NOVELL_NO_CHECKSUM);
    spx->len = h2n(sizeof(spxhdr) + data_size);
    spx->tc = 0;

    /*
     *
     * fill in SPX info
     */
    spx->connectionctl = spxconctl | ((data_size)?SPX_CONCTL_NEED_ACK:0);
    spx->datastreamtype = spxdatastream;
    spx->sourceconnectid = spxcon->local_con;
    spx->destconnectid = spxcon->remote_con;
    spx->sequencenumber = spxcon->local_next_seq;
    spx->acknumber = spxcon->remote_next_seq;
    spx->allocnumber = spxcon->local_alloc_num + spx->acknumber;
    if (data_bytes && !out_pak) {
	int i2;

	spxcon->local_next_seq++;
	bcopy(data_bytes, ((uchar *)spx) + (sizeof(spxhdr)), data_size);
	if (spx_debug) {
	    buginf("\nSPX %d bytes: ", data_size);
	    for (i2=0;i2<data_size;i2++) {
		buginf(" %2x ", data_bytes[i2]);
	    }
	}
    }

    /*
     * fill in some cisco pak fields
     */
    pak->if_output = spxcon->idb;
    pak->nif_output = spxcon->nidb;
    pak->linktype = LINK_NOVELL;

    /*
     *  don't send anymore data packets, if
     *  our sequence number would overrun the
     *  remote's allocation number.
     */
    if (data_size) {
	if (spxcon->remote_alloc_num <
	    spx->sequencenumber) {
	    if (spx_debug) {
		buginf("\nSPX: %x Send Queued. No window", spxcon);
	    }
	    pak_enqueue(&spxcon->outQ, pak);
	    return (TRUE);
	}
	if (QUEUESIZE(&spxcon->unackedQ)) {
	    if (spx_debug) {
		buginf("\nSPX: %x Send Queued. Previous unacked", spxcon);
	    }
	    pak_enqueue(&spxcon->outQ, pak);
	    return (TRUE);
	}
	/*
	 * put this packet on the unackedQ
	 * will be taken off and free'd, when it is acked.
	 */
	pak_lock(pak);
	pak_enqueue(&spxcon->unackedQ, pak);
    }

    /*
     * now try to send it
     */
    if (nov_send(pak)) {
	spx_new_timer(&spxcon->send_verify_watchdog,
		      SPX_VERIFY_WATCHDOG_TIMER);
	if (spx_debug) {
	    buginf("\nSPX: Sent an SPX packet");
	}
	return(TRUE);
    }
    if (data_size) {
	/*
	 * free it twice if it was locked.
	 */
	pak_unqueue(&spxcon->unackedQ, pak);
	datagram_done(pak);
    }

    if (spx_debug) {
	buginf("\nSPX: couldn't send an SPX packet");
    }
    return (FALSE);
}

void spx_send_watchdog_pak (spxcontype *spxcon)
{

    spx_new_timer(&spxcon->send_verify_watchdog, SPX_VERIFY_WATCHDOG_TIMER);
    spx_send_pak(spxcon, NULL, SPX_CONCTL_WATCHDOG, 0, 0, NULL, NULL);
}

static void spx_send_repeat_ack (spxcontype *spxcon)
{
    spx_send_pak(spxcon, NULL, SPX_CONCTL_SYS, 0, 0, NULL, NULL);
}

static void spx_abort_con (spxcontype *spxcon, paktype *pak)
{

}

static void spx_close_con (spxcontype *spxcon, paktype *pak)
{
    if (spx_debug) {
	buginf("\nSPX: %x Connection close requested by peer", spxcon);
    }
    spxcon->state = SPXCON_ST_PURGING;
    if (((spx_close_cb)(spxcon->spxsock->close_cb))(spxcon)) {
	/*
	 * kill it quick
	 */
	spx_new_timer(&spxcon->purge_timer, 3);
    } else {
	/*
	 * let application processes notice and
	 * clean up
	 */
	spx_new_timer(&spxcon->purge_timer, SPX_PURGE_TIMER);
    }
    spx_send_pak(spxcon, NULL, SPX_CONCTL_EOM, SPX_DATASTREAM_EOM_ACK,
		 0, NULL, NULL);
}


static void spx_process_ack (spxcontype *spxcon, spxhdr *spx)
{

    paktype *pak;
    spxhdr *spx_old;

    /*
     * look at incoming ack number....does it ack any of our
     * unacked packets ?  free if any such. No need to look
     * beyond the head, if it doesn't ack.
     */
    while (NULL != (pak = pak_queue_get_head(&spxcon->unackedQ))) {
	spx_old = (spxhdr *)ipxheadstart(pak);
	if (spx->acknumber < spx_old->sequencenumber) {
	    break;
	}
	/*
	 * we have been acked. Release this packet.
	 */
	pak = pak_dequeue(&spxcon->unackedQ);
	datagram_done(pak);
    }
    /*
     * look at the incoming allocation number...does it extend
     * remote's offered window ?  If it changes, try to send more
     * packets.
     */
    if (spx->allocnumber > spxcon->remote_alloc_num) {
	spxcon->remote_alloc_num = spx->allocnumber;
	/*
	 * our window has changed. Now what. Pull out packets
	 * from outq and try to send them, along with enqueuing
	 * them to the unackedQ.
	 */
	while (NULL != (pak = pak_queue_get_head(&spxcon->outQ))) {
	    spx_old = (spxhdr *)ipxheadstart(pak);
	    if (spxcon->remote_alloc_num < spx_old->sequencenumber) {
		break;
	    }
	    /*
	     * this packet can be sent.
	     */
	    if (spx_debug) {
		buginf("\nSPX: %x remote window opened..sending Q'd"
		       " pkt", spxcon);
	    }
	    pak = pak_dequeue(&spxcon->outQ);
	    if (spx_debug) {
		buginf("\nSPX: sending packet from outQ");
	    }
	    pak_lock(pak);
	    pak_enqueue(&spxcon->unackedQ, pak);
	    if (!nov_send(pak)) {
		pak_unqueue(&spxcon->unackedQ, pak);
		datagram_done(pak);
		if (spx_debug) {
		    buginf("\nSPX: couldn't send the SPX packet");
		}
		break;
	    }
	}
    }


    /*
     * do nothing if it doesn't ack
     * anything new.
     */
    if ((spx->len > sizeof(spxhdr)) ||
	(SPX_DATASTREAM_EOM_REQ == spx->datastreamtype)) {
	spxcon->remote_next_seq = spx->sequencenumber + 1;
    }
}


static void spx_rcvd_data (spxcontype *spxcon, paktype *pak)
{

    spxhdr *spx = (spxhdr *)ipxheadstart(pak);

    /*
     * incoming data
     * do something with it.
     * like move it to our inQ's, set our
     * ack number, ack it. etc.
     */
    if (spx_debug) {
	buginf("\nSPX: %u bytes data recvd", spx->len - sizeof(spxhdr));
    }
    /*
     * do we want to pass this data to the application in
     * chunks as received, or as a running stream ?
     */
    if (spxcon->flags & SPXCON_PACKET_IO) {
	pak_lock(pak);
	pak_enqueue(&spxcon->inQ, pak);
    } else {
	/*
	 * fill up the rcvd_data queue
	 * and set pointers
	 */
      }
}

int spx_api_read_chunk (spxcontype *spxcon, paktype **pak, int *size)
{


    *pak = NULL;
    *size = 0;

    if (!validmem(spxcon)) {
	return (SPX_RET_BADCON);
    }

    switch (spxcon->state) {

    case SPXCON_ST_OPEN:

	if (QUEUESIZE(&spxcon->inQ)) {
	    *pak = pak_dequeue(&spxcon->inQ);
	    if (pak) {
		spxhdr *spx = (spxhdr *)ipxheadstart(*pak);
		*size = (int) (spx->len - sizeof(spxhdr));
		return (SPX_RET_DATA);
	    }
	}
	return (SPX_RET_NODATA);

    case SPXCON_ST_CLOSING:
    case SPXCON_ST_PURGING:
    case SPXCON_ST_INITIAL:
	return (SPX_RET_CONCLOSED);

    default:
	return (SPX_RET_BADCON);
    }

    return SPX_RET_NODATA;
}

int spx_api_send_data (spxcontype *spxcon, uchar *data, int size)
{

    spx_send_pak(spxcon, NULL, 0, 0, size, data, NULL);
    return SPX_RET_OK;
}


#ifdef NOBODY_CALLS_ME
int spx_api_setup_pak (spxcontype *spxcon, paktype **pak, int size,
		       uchar **offset)
{

    *pak = NULL;
    *offset = 0;

    if (!validmem(spxcon)) {
	return (SPX_RET_BADCON);
    }

    switch (spxcon->state) {

    case SPXCON_ST_OPEN:

	*pak = getbuffer(sizeof(spxhdr)+size);
	if (!*pak) {
	    return (SPX_RET_NOMEM);
	}
	*offset = ((uchar *)(ipxheadstart(*pak))) + sizeof(spxhdr);
	return (SPX_RET_OK);

    case SPXCON_ST_CLOSING:
    case SPXCON_ST_PURGING:
    case SPXCON_ST_INITIAL:
	return (SPX_RET_CONCLOSED);

    default:
	return (SPX_RET_BADCON);
    }

    return (SPX_RET_BADCON);
}
#endif
#ifdef NOBODY_CALLS_ME
int spx_api_send_pak (spxcontype *spxcon, paktype *pak, int size)
{
    if (!validmem(spxcon)) {
	datagram_done(pak);
	return (SPX_RET_BADCON);
    }
    switch (spxcon->state) {

    case SPXCON_ST_OPEN:
	spx_send_pak(spxcon, NULL, 0, 0, size, NULL, pak);
	return SPX_RET_OK;

    case SPXCON_ST_CLOSING:
    case SPXCON_ST_PURGING:
    case SPXCON_ST_INITIAL:
	datagram_done(pak);
	return (SPX_RET_CONCLOSED);

    default:
	datagram_done(pak);
	return (SPX_RET_BADCON);
    }
    datagram_done(pak);
    return (SPX_RET_FAILED);
}
#endif

#ifdef NOBODY_CALLS_ME
void spx_api_flush_input (spxcontype *spxcon)
{
}
#endif

int spx_api_inputsize (spxcontype *spxcon)
{
    return (QUEUESIZE(&spxcon->inQ));
}

int spx_api_outputsize (spxcontype *spxcon)
{
    return (QUEUESIZE(&spxcon->outQ) + QUEUESIZE(&spxcon->unackedQ));
}

void spx_api_push (spxcontype *spxcon, boolean fl1, boolean fl2)
{
}

boolean spx_api_con_alive (spxcontype *spxcon)
{
    return (SPXCON_ST_OPEN == spxcon->state);
}

void spx_api_closecon (spxcontype *spxcon)
{
    /*
     * send spx-informed-close-request pak
     * mark the connection as almost closed
     * start the purge timer.
     */
    if (SPXCON_ST_PURGING != spxcon->state) {
	spx_send_pak(spxcon, NULL, SPX_CONCTL_NEED_ACK,
		     SPX_DATASTREAM_EOM_REQ,
		     0, NULL, NULL);
	spxcon->state = SPXCON_ST_PURGING;
	spx_new_timer(&spxcon->purge_timer, SPX_PURGE_TIMER);
    } else {
	/*
	 * kill this connection now
	 */
	spx_new_timer(&spxcon->purge_timer, 3);
    }
}


boolean spx_api_con_closing (spxcontype *spxcon)
{
    return (SPXCON_ST_OPEN != spxcon->state);
}

static void spx_new_timer (sys_timestamp *new_timer, ulong ticks)
{

    TIMER_START(*new_timer, ticks);
    if ((!TIMER_RUNNING(spx_wake_time)) ||
	TIMER_EARLIER(*new_timer, spx_wake_time)) {
	spx_wake_time = *new_timer;
    }
}

static void spx_purge_con (spxcontype *spxcon)
{

    paktype *pak;
    /*
     * flush pending input, output
     * remove it from conQ of the socket.
     * reduce the connections count. etc.
     */
    if (spx_debug) {
	buginf("\nSPX: purging spxcon %x from conQ", spxcon);
    }
    p_unqueue(&spxcon->spxsock->spxconQ, spxcon);
    if (spx_debug) {
	buginf("\nSPX: returning inQ buffers");
    }
    while(NULL != (pak = pak_dequeue(&spxcon->inQ)))
	datagram_done(pak);
    if (spx_debug) {
	buginf("\nSPX: returning outQ buffers");
    }
    while(NULL != (pak = pak_dequeue(&spxcon->outQ)))
	datagram_done(pak);
    if (spx_debug) {
	buginf("\nSPX: returning unackedQ buffers");
    }
    while(NULL != (pak = pak_dequeue(&spxcon->unackedQ)))
	datagram_done(pak);
    if (spx_debug) {
	buginf("\nSPX: returning spxcon");
    }
    free(spxcon);
}

/*
 * SPX subsystem header
 */
#define SPX_SERVER_MAJVERSION 1
#define SPX_SERVER_MINVERSION 0
#define SPX_SERVER_EDITVERSION  1

SUBSYS_HEADER(spx, SPX_SERVER_MAJVERSION, SPX_SERVER_MINVERSION,
	      SPX_SERVER_EDITVERSION,
	      spx_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: ipx", "req: ipx");
