/* $Id: dmdp.c,v 3.6 1996/02/07 16:14:56 widmer Exp $
 * $Source: /release/111/cvs/Xsys/servers/dmdp.c,v $
 *------------------------------------------------------------------
 * dmdp.c - Dnsix-NAT sussport
 *
 * Feb 1993, Phanindra Jujjavarapu
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dmdp.c,v $
 * Revision 3.6  1996/02/07  16:14:56  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.5  1996/02/01  06:08:42  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/22  06:59:06  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1996/01/05  10:18:06  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.2  1995/11/17  18:50:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/03  07:42:46  hampton
 * EIGRP stops processing input.  Convert the IP socket input queue into a
 * watched_queue.  This allows the scheduler to see if there is input
 * waiting when the process dismisses.  [CSCdi38156]
 *
 * Revision 2.3  1995/07/12 04:11:30  mikel
 * CSCdi36107:  DNSIX doesnt check malloc return code
 *
 * Revision 2.2  1995/06/09  13:18:43  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:43:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../os/old_timer_callbacks.h"
#include "interface_private.h"
#include "packet.h"
#include "../ip/ip.h"
#include "config.h"
#include "parser.h"
#include "dmdp.h"
#include "parser_defs_dnsix.h"


/*
 * Global variables
 */
static char retrans_name[] = "Dmdp Retransmit Timer";
dmdpblocktype dmdpblock;

/*
 * dmdp_init
 * Initialize the DMDP service
 */

void dmdp_init (void)
{
    dmdpblock.serving = FALSE;
    dmdpblock.retries = 0;
    dmdpblock.qtime = 0;
    dmdpblock.client_list = NULL;
}/* end of dmdp_init */

/*
 * dmdp_start
 * start DMDP service
 */

void dmdp_start (void)
{
    if (dmdpblock.serving == FALSE) {
	dmdpblock.retries = DMDP_RETRIES;
	dmdpblock.qtime = DMDP_QTIME;
	dmdpblock.client_list = NULL;
	dmdpblock.pid = cfork(dmdp_server, (long)&dmdpblock, 1500,
			       "DMDP Server", 0);
	dmdpblock.serving = TRUE;
    }
} /* end of dmdp_start */

/*
 * dmdp_dump_pak
 * abort all the packets
 * from this client
 */
void dmdp_dump_pak (ipsocktype *socket, paktype *pak)
{
    dmdplisttype *client;

    if (pak == NULL)
	return;
    if ((client = dmdp_find_client(socket)) == NULL)
	return;
    if (client->pak_queue->qhead == NULL)
	return;
    unqueue(client->pak_queue, pak);

} /* end of dmdp_dump_pak */

/*
 * dmdp_chksum
 * calculate the checksum
 * DNSIX. Page 65 Appendix
 * DNSIX interface specifications.
 */

ushort dmdp_chksum(char *buff, ushort length)
{
    char lsb_of_byte;
    char lsb_of_accumulator;
    char byte;
    int i, j;
    ushort accumulator;
    ushort polynomial;

    accumulator = 0177777;
    polynomial =  0120001;

    for (i= 0; i< length; i++) {
	byte = buff[i];
	for (j=1; j <= 8; j++) {
	    lsb_of_byte = byte & 1;
	    lsb_of_accumulator = accumulator & 1;
	    accumulator >>=1;
	    if (lsb_of_byte != lsb_of_accumulator)
		accumulator ^=polynomial;
	    byte >>=1;
	}
    }
    return(accumulator);
} /* end of dmdp_chksum */


/*
 * dmdp_find_client
 * find the client
 */
dmdplisttype * dmdp_find_client (ipsocktype *socket)
{
    dmdplisttype *client;

    client = dmdpblock.client_list;
    for (; client ; client = client->next) {
	if (client->socket == socket)
	    return(client);
    }
    return(NULL);
} /* end of dmdp_find_client */


/*
 * dmdp_delete_client
 * delete the client
 */
void dmdp_delete_client (dmdplisttype *client)
{
    paktype *pak;
    dmdplisttype *temp1, *temp2;

    if (client == dmdpblock.client_list) {
	dmdpblock.client_list = client->next;
    } else {
	temp2 = temp1  = dmdpblock.client_list;
	for (; temp1 && temp1->next ;  temp1 = temp1->next) {
	    if (temp1->next == client) {
		temp2->next = temp1->next->next;
		break;
	    } else {
		temp2 = temp1->next;
	    }
	}
    }
    if (client) {
	destroy_timer(client->retrans_timer);
	while ((pak = dequeue(client->pak_queue))) {
	    retbuffer(pak);
	}
	free(client->pak_queue);
	free(client);
    }
} /* end of dmdp_delete_client */


/*
 * dmdp_register
 * register a client with dmdp
 */

void dmdp_register (ipsocktype *socket, receivetype call_receive,
		    notifytype call_notify)
{
    dmdplisttype *client;

    if (!dmdpblock.serving)
	dmdp_init();
    /*
     * find the socket
     */
    if ((client = dmdp_find_client(socket)) == NULL) {
	client = (dmdplisttype *)(malloc(sizeof(dmdplisttype)));
	if (!client) {
	    printf(nomemory);
	    return;
	}
	client->next = dmdpblock.client_list;
	dmdpblock.client_list = client;
	client->socket = socket;
	client->send_seq[0] = 1;
	client->send_seq[1] = 0;
	client->rcv_seq[0] = 0;
	client->pak_queue = malloc(sizeof(queuetype));
	if (!client->pak_queue) {
	    printf(nomemory);
	    free(client);
	    dmdpblock.client_list = NULL;
	    return;
	}
	client->retrans_timer = create_timer(dmdp_retrans,
						  client,
						  NULL,
						  NULL,
						  retrans_name,
						  TMT_ONCE);
    }
    client->receive = call_receive;
    client->notify = call_notify;
} /* end of dmdp_register */

/*
 * dmdp_deregister
 * register a client with dmdp
 */

void dmdp_deregister (ipsocktype *socket)
{
    dmdplisttype *client;

    if (!dmdpblock.serving)
	dmdp_init();
    /*
     * find the socket
     */
    if ((client = dmdp_find_client(socket)) == NULL)
	return;
    dmdp_delete_client(client);

} /* end of dmdp_deregister */

/*
 * dmdpBLOCK
 * Wait for a input on any packet
 */

boolean dmdpBLOCK (void)
{
    dmdplisttype *client;

    for (client = dmdpblock.client_list; client; client = client->next) { 
	if (!process_is_queue_empty(client->socket->inq))
	    return(FALSE);			/* input available */
    }
    return(TRUE);
} /* end of dmdpBLOCK */

/*
 * dmdp_service_stop
 * stop the dmdp service
 */
void dmdp_service_stop (void)
{
    dmdplisttype *client;
    dmdplisttype *temp;
    paktype *pak;

    if (dmdpblock.serving) {
	process_kill(dmdpblock.pid);
	client = dmdpblock.client_list;
	while (client) {
	    destroy_timer(client->retrans_timer);
	    while ((pak = dequeue(client->pak_queue))) {
		retbuffer(pak);
	    }
	    free(client->pak_queue);
 	    temp = client->next;
	    free(client);
	    client = temp;
	}
	dmdpblock.retries = 0;
	dmdpblock.qtime = 0;
	dmdpblock.client_list = NULL;
	dmdpblock.serving = FALSE;
	dmdpblock.pid = 0;
    }
} /* end of dmdp_service_stop */

/*
 * dmdp_rcv_ack
 * dmdp_rcv_ack
 */
void dmdp_rcv_ack(ipsocktype *socket, paktype *rcv_pak, dmdptype *rcv_dmdp)
{
    dmdpdatatype *data;
    dmdptype *dmdp;
    dmdplisttype *client;
    paktype *pak, *pak_copy;
    int length;
    ushort fport;
    ipaddrtype faddress;
    
    if ((client = dmdp_find_client(socket)) == NULL)
	return;
    pak = (paktype *)client->pak_queue->qhead;
    data = (dmdpdatatype *)pak->datagramstart;
    if ((data->send_seq[0] == rcv_dmdp->seq[0])
	&& (data->send_seq[1] == rcv_dmdp->seq[1])) {
	pak = dequeue(client->pak_queue);
	(client->notify)(data->handle, 
			 data->retries, 
			 data->fport,
			 TRUE);
	datagram_done(pak);
	client->send_seq[0]++;
    }
    if (client->pak_queue->qhead == NULL)
	return;
    pak = (paktype *)client->pak_queue->qhead;
    data = (dmdpdatatype *)pak->datagramstart;
    length = data->send_length;
    faddress = data->faddress;
    fport = data->fport;
    pak_copy = pak_duplicate(pak);
    if (pak_copy == NULL ) 
	return;
    dmdp = (dmdptype *)pak_copy->datagramstart;
    dmdp->control = DMDP_DATA;
    dmdp->version = DMDP_VERSION;
    dmdp->data_offset = DMDP_HDR_SIZE/4;
    length += DMDP_HDR_SIZE;
    data->retries++;
    data->send_seq[0] = dmdp->seq[0] = client->send_seq[0];
    data->send_seq[1] = dmdp->seq[1] = client->send_seq[1];
    dmdp->chksum = 0;
    dmdp->chksum = dmdp_chksum((char *)dmdp, length);
    if (faddress) {
	client->socket->faddress.ip_addr = faddress;
	client->socket->faddress.type = ADDR_IP;
    }
    if (fport)
	client->socket->fport = fport;
    write_ipsocket(client->socket, pak_copy, length);
    set_timer(client->retrans_timer, DMDP_RETRANS_TIME * ONESEC);
} /* end of dmdp_rcv_ack */

/*
 * dmdp_send_ack
 * dmdp_rcv_data
 */
void dmdp_send_ack (dmdplisttype *client)
{
    int offset;
    ipsocktype *socket;
    ulong length;
    paktype *pak;
    dmdptype *dmdp;

    socket = client->socket;
    pak = setup_ipsocket(socket, DMDP_HDR_SIZE, (uint *)&offset);
    if (pak == NULL)
	return;
    dmdp = (dmdptype *)pak->datagramstart;
    dmdp->control = DMDP_ACK;
    dmdp->version = DMDP_VERSION;
    dmdp->data_offset = DMDP_HDR_SIZE/4;
    length = DMDP_HDR_SIZE;
    dmdp->seq[0] = client->rcv_seq[0];
    dmdp->seq[1] = client->rcv_seq[1];
    dmdp->chksum = 0;
    dmdp->chksum = dmdp_chksum((char *)dmdp, length);
    write_ipsocket(client->socket, pak, length);
} /* end of dmdp_send_ack */

/*
 * dmdp_rcv_data
 * dmdp_rcv_data
 */
void dmdp_rcv_data (ipsocktype *socket, paktype *rcv_pak, dmdptype *rcv_dmdp)
{
    dmdplisttype *client;
    
    if ((client = dmdp_find_client(socket)) == NULL)
	return;

    /*
     * check if the rcv sequence is older
     * than what we have.
     */
    if (rcv_dmdp->seq[0] < client->rcv_seq[0])
	return;
    else 
	if (rcv_dmdp->seq[0] == client->rcv_seq[0])
	    dmdp_send_ack(client);
	else {
	    /* it is a new message */
	    client->rcv_seq[0] = rcv_dmdp->seq[0];
	    (client->receive) (rcv_pak,((uchar *)rcv_dmdp + DMDP_HDR_SIZE));
	    dmdp_send_ack(client);
	}

} /* end of dmdp_rcv_data */

/*
 * dmdp_retrans
 * dmdp retransmit timer
 */
void dmdp_retrans (dmdplisttype *client, uchar *ptr1, uchar *ptr2)
{
    paktype *pak;
    paktype *pak_copy;
    dmdptype *dmdp;
    dmdpdatatype *data;
    ulong length;
    ipaddrtype faddress;
    ushort fport;
    
    if ((client = dmdp_find_client(client->socket)) == NULL)
	return;
    if (client->pak_queue->qhead == NULL)
	return;
    
    pak = (paktype *)client->pak_queue->qhead;
    data = (dmdpdatatype *)pak->datagramstart;
    if (data->retries == 0)
	return;
    if (data->retries == dmdpblock.retries) {
	if (((client->notify)(data->handle, 
			 data->retries, 
			 data->faddress,
			 FALSE)) == FALSE) {
	    pak = dequeue(client->pak_queue);
	    datagram_done(pak);
	    return;
	} else
	    data->retries = 0;
    }
    length = data->send_length;
    pak_copy = pak_duplicate(pak);
    if (pak_copy == NULL ) 
	return;
    dmdp = (dmdptype *)pak_copy->datagramstart;
    dmdp->control = DMDP_DATA;
    dmdp->version = DMDP_VERSION;
    dmdp->data_offset = DMDP_HDR_SIZE/4;
    faddress = data->faddress;
    fport = data->fport;
    length += DMDP_HDR_SIZE;
    data->retries++;
    data->send_seq[0] = dmdp->seq[0] = client->send_seq[0];
    data->send_seq[1] = dmdp->seq[1] = client->send_seq[1];
    dmdp->chksum = 0;
    dmdp->chksum = dmdp_chksum((char *)dmdp, length);
    if (faddress) {
	client->socket->faddress.ip_addr = faddress;
	client->socket->faddress.type = ADDR_IP;
    }
    if (fport)
	client->socket->fport = fport;
    write_ipsocket(client->socket, pak_copy, length);
    set_timer(client->retrans_timer, DMDP_RETRANS_TIME * ONESEC);
}

/*
 * dmdp_server
 * dmdp serving process
 */
void dmdp_server (void)
{
    paktype *pak;
    iphdrtype *ip;
    dmdptype *dmdp;
    udptype *udp;
    dmdplisttype *client;
    ipsocktype *socket;
    
    edisms(systeminitBLOCK, 0);
    while (TRUE) {
	/* Block until input on this socket */
	edisms((blockproc *)dmdpBLOCK, 0);
	for (client = dmdpblock.client_list; client; client = client->next) {
	    socket = client->socket;
	    while ((pak = process_dequeue(socket->inq))) {
		--(socket->inallocated);
		ip = (iphdrtype *)ipheadstart(pak);
		udp = (udptype *)((char *)ip + (ip->ihl << 2));
		dmdp = (dmdptype *)((uchar *)udp + UDPHEADERBYTES);
		if (dmdp->version != DMDP_VERSION) {
		    retbuffer(pak);
		    continue;
		}
		switch (dmdp->control) {
		  case DMDP_ACK:
		    dmdp_rcv_ack(socket, pak, dmdp);
		    break;
		  case DMDP_DATA:
		    dmdp_rcv_data(socket, pak, dmdp);
		    break;
		  case DMDP_ENC:
		  case DMDP_NACK:
		  case DMDP_UNROPT:
		  case DMDP_NOENC:
		  default:
		    printf("\nDMDP: Unrecognized DMDP packet from  %i",
			   ip->srcadr);
		    break;
		}
		retbuffer(pak);
	    }
	}
    }
} /* end of dmdp_server */


/*
 * dmdp_send:
 * send the dmdp packet 
 */
boolean dmdp_send (ipsocktype *socket, paktype *pak, ipaddrtype faddress,
		   ushort fport, ulong length)
{
    paktype *pak_copy;
    dmdptype *dmdp;
    dmdpdatatype *data;
    dmdplisttype *client;
    boolean send;
    
    send = FALSE;
    if (dmdpblock.serving == FALSE)
	return(FALSE);
    if ((client = dmdp_find_client(socket)) == NULL)
	return(FALSE);
    if (client->pak_queue->qhead == NULL)
	return(TRUE);
    enqueue(client->pak_queue, pak);
    data = (dmdpdatatype *)pak->datagramstart;
    data->handle = (ulong)pak;
    data->retries = 0;
    data->send_seq[0] = 0;
    data->send_seq[1] = 0;
    data->send_length = length;
    data->faddress = faddress;
    data->fport = fport;
    pak_copy = pak_duplicate(pak);
    if (pak_copy == NULL) 
	return(FALSE);
    dmdp =(dmdptype *)(pak_copy->network_start +
		       IPHEADERBYTES(NOPT) + UDPHEADERBYTES);
    dmdp->control = DMDP_DATA;
    dmdp->version = DMDP_VERSION;
    dmdp->data_offset = DMDP_HDR_SIZE/4;
    length += DMDP_HDR_SIZE;
    data->retries++;
    data->send_seq[0] = dmdp->seq[0] = client->send_seq[0];
    data->send_seq[1] = dmdp->seq[1] = client->send_seq[1];
    dmdp->chksum = 0;
    dmdp->chksum = dmdp_chksum((char *)dmdp, length);
    if (faddress) {
	client->socket->faddress.ip_addr = faddress;
	client->socket->faddress.type = ADDR_IP;
    }
    if (fport)
	client->socket->fport = fport;
    write_ipsocket(client->socket, pak_copy, length);
    set_timer(client->retrans_timer, DMDP_RETRANS_TIME * ONESEC);
    return(TRUE);
} /* end of dmdp_send */


/*
 * dmdp_command:
 * process dmdp commands
 */
void dmdp_command (parseinfo *csb)
{
    int keyword;

    keyword = GETOBJ(int,1);
    if (csb->nvgen) {
	if (dmdpblock.serving) {
	    if (dmdpblock.retries != DMDP_RETRIES) {
		nv_write(TRUE, "%s %d", csb->nv_command, dmdpblock.retries);
	    }
	}
	return;
    }
    switch (keyword) {
      case DNSIX_DMDP_RETRIES:
	if (csb->sense) {
	    if (dmdpblock.serving == FALSE) {
		dmdp_init();
	    }
	    dmdpblock.retries = GETOBJ(int, 2);
	} else  {
	    dmdpblock.retries = DMDP_RETRIES;
	}
	break;
      default:
	bad_parser_subcommand(csb, keyword);
	break;
    }
}
