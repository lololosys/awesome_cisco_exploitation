/* $Id: remif.c,v 3.6.8.3 1996/08/28 12:50:22 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/remif.c,v $
 *------------------------------------------------------------------
 * Remote Interfaces code
 *
 * October 1993, Carl Schaefer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Remote interfaces allow individual physical interfaces to be
 * virtually attached to a remote entity via UDP encapsulation.
 *------------------------------------------------------------------
 * $Log: remif.c,v $
 * Revision 3.6.8.3  1996/08/28  12:50:22  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.8.2  1996/05/09  14:31:07  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.6.8.1.18.1  1996/04/27  07:04:56  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.6.8.1.8.1  1996/04/08  01:55:41  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.6.8.1  1996/03/18  20:17:47  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.12.1  1996/03/22  09:40:19  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.6  1996/02/28  09:33:03  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.5.12.1  1996/03/05  06:14:33  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.5.6.1  1996/02/27  21:09:07  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.5  1996/01/24  20:15:12  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.4  1996/01/19  00:57:48  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1996/01/16  03:37:23  ahh
 * CSCdi46921:  TCP: remove unused second parameter to tcp_close
 *
 * Revision 3.2.14.1  1996/02/15  21:50:43  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additonal swidb parameter to the registry bump_cache_version
 *
 * Revision 3.2  1995/11/17  09:36:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:58:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/10/20  20:16:10  phwang
 * fix ip mulcast error
 *
 * Revision 2.3  1995/06/19  01:06:43  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:05:39  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:00:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "msg_remif.c"			/* Not a typo, see logger.h */
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "ieee.h"
#include "ip.h"
#include "ip_registry.h"
#include "../if/network.h"
#include "../tcp/tcp.h"
#include "remif_proto.h"

/*
 * there's one of these per remote interface
 * it reads from the queue of packets handed up from interrupt level,
 * encapsulates them, and then fires them off.
 */

static forktype remif_tunnel (remif_t* remif)
{
    for (;;) {
	edisms(queueBLOCK, (ulong) &remif->pakQ);
	for (;;) {
	    paktype *pak, *socket_pak;
	    int paklen, datalen, offset;
	    leveltype level;

	    level = raise_interrupt_level(NETS_DISABLE);
	    pak = dequeue(&remif->pakQ);
	    reset_interrupt_level(level);
	    if (pak == NULL)
		break;

	    paklen = pak->datagramsize;
	    datalen = sizeof(remif_pakhdr) + paklen;
	    socket_pak = setup_ipsocket(remif->soc, datalen, &offset);
	    if (socket_pak == NULL) {
		errmsg(&msgsym(NOMEM, REMIF), "remote if data");
	    } else {
		remif_pakhdr *rphdr;

		rphdr = (remif_pakhdr *)(socket_pak->data_area + offset);
		rphdr->if_id = (ulong) remif;
		rphdr->bytes = (ulong) paklen;
		bcopy(pak->datagramstart, rphdr->data, paklen);
		write_ipsocket(remif->soc, socket_pak, datalen);
	    }
	    datagram_done(pak);
	}
    }
}


/*
 * replaces interface's iqueue routine (e.g. raw_enqueue)
 * enqueues incoming packets if there's a place for them to go
 */

static void remif_iqueue (hwidbtype *hwidb, paktype *pak)
{
    remif_t *remif;

    remif = hwidb->remif;

    if (remif == NULL) {
	errmsg(&msgsym(IQUEUE, REMIF), hwidb->hw_namestring);
    } else {
	if (remif->soc && !QUEUEFULL(&remif->pakQ)) {
	    enqueue(&remif->pakQ, pak);
	    return;
	}
    }

    /* If there's no socket, we don't have anywhere to send
     * the packet, so just return it.
     */
    datagram_done(pak);
    return;
}


/*
 * UDP frames to the remote interface port are handled here.
 * they're de-encapsulated and transmitted.
 */

static boolean remif_udp_input (paktype *pak, udptype *udp)
{
    remif_t *remif;
    remif_pakhdr *rphdr;
    hwidbtype *hwidb;

    if (udp->dport != REMIF_PORT)
	return FALSE;

    rphdr = (remif_pakhdr *)udp->udpdata;
    remif = (remif_t *)rphdr->if_id;
    pak->datagramstart = rphdr->data;
    pak->datagramsize = rphdr->bytes;
    clear_if_input(pak);
    hwidb = remif->hwidb;
    pak->if_output = hwidb->firstsw;
    if (remif->oqueue(hwidb, pak, TAIL))
	hwidb->soutput(hwidb);
    return TRUE;
}


/*
 * replaces interface's oqueue routine (e.g. holdq_enqueue)
 * complain if somebody tries to transmit on the interface while
 * it's being used remotely
 */

static boolean stub_oqueue (hwidbtype *hwidb, paktype *pak, enum HEADTAIL ht)
{
    errmsg(&msgsym(OQUEUE, REMIF), hwidb->hw_namestring);
    datagram_done(pak);
    return TRUE;
}

/*
 * write a string to a TCP connection, handling partial success
 * and blocking
 */

static void remif_write (tcbtype *tcb, uchar *buf, uint bytes)
{
    int cc;

    for (;;) {
	cc = tcp_putstring(tcb, buf, bytes);
	bytes -= cc;
	if (bytes == 0)
	    break;
	tcp_idle_put(tcb);
	if (tcp_closing(tcb))
	    break;
	buf += cc;
    }
    return;
}

/*
 * return response to open command
 * Response is:
 * 4 bytes of interface ID
 * 1 byte of MAC address size, or zero if there is no MAC address
 * n bytes of MAC address, or no bytes if there isn't a MAC address
 */

static void rr_opened (remif_t *remif)
{
    uchar cmd;
    tcbtype *tcb;
    uchar zero = 0;
    uchar addr_len = 0;
    uchar size = 0;

    tcb = remif->tcb;
    cmd = REMIF_RESP_OPENED;

    /* Compute the size of our response packet. */

    size = sizeof(remif);

    if (is_ieee_zero(remif->hwidb->hardware)) {
	addr_len = 0;
	size++;
    } else {

	/* This looks stupid, 'cuz it will always be ALWAYS be six bytes,
	   but we may need to pass other address information for some 
	   serial links, or other interfaces...  At which point this code
	   will change... */

	addr_len = sizeof(remif->hwidb->hardware);
	size += addr_len + 1;
    }

    /* Send the command and length of our response. */

    remif_write(tcb, &cmd, sizeof(cmd));
    remif_write(tcb, &size, sizeof(size));

    /* Send the interface ID */

    remif_write(tcb, (uchar*) &remif, sizeof(remif));

    /* If we don't have a mac address defined, mark that fact. */

    if (is_ieee_zero(remif->hwidb->hardware)) {
	remif_write(tcb, (uchar *)&zero, sizeof(zero));
    } else {
	remif_write(tcb, (uchar *)&addr_len, sizeof(uchar));
	remif_write(tcb, remif->hwidb->hardware, addr_len);
    }
    tcp_putpush(tcb, FALSE, FALSE);
    errmsg(&msgsym(OPEN, REMIF), remif->owner, remif->hwidb->hw_namestring);
    return;
}

/*
 * rr_open_failed
 * 
 * Signal the failure that occurred while opening a remote interface.
 * If the cause requires additional data, pass it in "message"
 */

static void rr_open_failed (remif_t *remif, int cause, uchar *message)
{
    uchar cmd;
    tcbtype *tcb;
    uchar size = 0;
    uchar zero = 0;

    tcb = remif->tcb;
    cmd = (uchar) cause;
   
    remif_write(tcb, &cmd, sizeof(cmd));

    switch(cause) {
    case REMIF_RESP_IN_USE:
	size = (uchar)strlen(message) + 1;
	remif_write(tcb, &size, sizeof(size));
	remif_write(tcb, message, size);
	break;
    default:
	remif_write(tcb, &zero, sizeof(zero));
    }
}

/*
 * turn an interface into a remote interface
 */

static boolean rc_open (remif_t *remif, char *ifname, char *owner)
{
    hwidbtype *hwidb;

    remif->hwidb = NULL;

    FOR_ALL_HWIDBS(hwidb)
	if (strcasecmp(hwidb->hw_namestring, ifname) == 0)
	    break;

    if (hwidb == NULL) {
	errmsg(&msgsym(IFNONE, REMIF), ifname, owner);
	rr_open_failed(remif, REMIF_RESP_NO_SUCH, NULL);
	return FALSE;
    }
    if (hwidb->remif) {
	errmsg(&msgsym(DUPOPEN, REMIF), hwidb->hw_namestring,
	       hwidb->remif->owner);
	rr_open_failed(remif, REMIF_RESP_IN_USE, hwidb->remif->owner);
	return FALSE;
    }

    if (hwidb->state != IDBS_UP) {
	rr_open_failed(remif, REMIF_RESP_DOWN, NULL);
	return FALSE;
    }

    /* save stuff we'll muck with */
    remif->idb_state = hwidb->state;
    remif->hwidb = hwidb;
    remif->oqueue = hwidb->oqueue;
    remif->iqueue = hwidb->iqueue;
    remif->periodic = hwidb->periodic;
    remif->ip_routecache = hwidb->ip_routecache;
    strcpy(remif->owner,owner);

    /*
     * Set up local vectors
     */
    remif->tring_bridge_setup = (tring_bridge_setup_t)return_false;


    hwidb->remif = remif;		/* set this before stealing vectors */
    hwidb->iqueue = remif_iqueue;	/* start discarding input */
    hwidb->periodic = NULL;

    hwidb->ip_routecache = 0;
    hwidb->state = IDBS_ADMINDOWN;	/* may need to be smarter here */
    reg_invoke_bump_cache_version(hwidb, NULL, FALSE);

    hwidb->oqueue = stub_oqueue;	/* allow output until cache bump */

    rr_opened(remif);

    return TRUE;
}

/*
 * For each address in the listeners list add them to the interface.
 */

static void remif_addresses (idbtype *idb)
{
    remif_listener *listener;

    if (idb->hwptr->remif == NULL) return;

    listener = idb->hwptr->remif->listeners;
    
    while (listener) {
	if_multi_add(idb->hwptr, idb, listener->addr);
	listener = listener->next;
    }
}

static boolean rc_new_mac (remif_t *remif, int size, uchar *data)
{
    coerce_hwaddr(data, remif->hwidb);
    return (TRUE);
}

static boolean rc_listen (remif_t *remif, int size, uchar *data)
{
    remif_listener *listener;

    /*
     * size param isn't currently used, but is provided for possible
     * non IEEE conformant addresses, if we ever have to handle a different
     * sized address, this will have to be re-written. :-(
     */

    /*
     * Traverse the listeners list to see if we've seen this address already.
     * If not, add the bugger. 
     */

    listener = remif->listeners;

    while(listener) {
	if (ieee_equal(data, listener->addr)) return TRUE;
	listener = listener->next;
    }

    /* We havn't seen the address, so add it. */

    listener = malloc(sizeof(remif_listener));

    if (!listener) {
	return FALSE;
    }

    ieee_copy(data, listener->addr);
    listener->size = 6;

    /* Prepend it to our list of listeners. */

    listener->next = remif->listeners;
    remif->listeners = listener;

    return TRUE;
}

static boolean rc_reset (remif_t *remif)
{
    remif->hwidb->state = IDBS_UP;
    (*remif->hwidb->reset)(remif->hwidb);
    remif->hwidb->state = IDBS_ADMINDOWN;
    return TRUE;
}

/*
 * handle going into promiscuous mode on the interface
 */
static boolean rc_promiscuous (remif_t *remif)
{
    remif->hwidb->state = IDBS_RESET;
    remif->bridging_enabled = TRUE;

    (*remif->hwidb->reset)(remif->hwidb);
    remif->hwidb->state = IDBS_ADMINDOWN;

    return TRUE;

}

/*
 * handle going into chaste mode on the interface
 */
static boolean rc_chaste (remif_t *remif)
{
    remif->hwidb->state = IDBS_RESET;
    remif->bridging_enabled = FALSE;

    (*remif->hwidb->reset)(remif->hwidb);
    remif->hwidb->state = IDBS_ADMINDOWN;

    return TRUE;

}

/*
 * handle putting token ring into bridge mode
 */
static boolean rc_tbridge (remif_t *remif)
{
    hwidbtype* hwidb;

    hwidb = remif->hwidb;
    (*remif->tring_bridge_setup)(hwidb);
    return(TRUE);

}

/*
 * handle remote interface close command (or protocol error, TCP error, etc.)
 */

static void rc_close (remif_t *remif)
{
    paktype *pak;
    hwidbtype *hwidb;
    remif_listener *listener, *next;

    tcp_close(remif->tcb);
    if (remif->soc)
	close_ipsocket(remif->soc);
    process_kill(remif->tunnelpid);

    hwidb = remif->hwidb;
    if (hwidb) {
	hwidb->ip_routecache = remif->ip_routecache;
	hwidb->state = remif->idb_state;
	hwidb->oqueue = remif->oqueue;
	hwidb->iqueue = remif->iqueue;
	hwidb->periodic = remif->periodic;

	if (remif->listeners) {
	    /* Remove the chain of listeners, if any... */
	    
	    listener = remif->listeners;
	    while (listener) {
		next = listener->next;
		free(listener);
		listener = next;
	    }
	}

	hwidb->remif = NULL;
	
	reg_invoke_bump_cache_version(hwidb, NULL, FALSE);

	while ((pak = dequeue(&remif->pakQ))) {
	    datagram_done(pak);
	}
	errmsg(&msgsym(CLOSED, REMIF), remif->owner,
	       remif->hwidb->hw_namestring);
    }

    free(remif);
    
    return;
}


/*
 * handle remote endpoint telling us what UDP port to send
 * encapsulated packets to.
 */

static boolean ri_pakport (remif_t *remif, ushort fport)
{
    addrtype faddr;

    faddr.type = ADDR_IP;
    faddr.ip_addr = remif->ip_remote;
    remif->soc = open_ipsocket(UDP_PROT, NULL /* any local address */,
			       &faddr, REMIF_PORT, fport, 
			       0 /* no status flags */);
    if (remif->soc == NULL) {
	errmsg(&msgsym(NOMEM, REMIF), "tunnel UDP endpoint");
	return FALSE;
    }
    return TRUE;
}


/*
 * read a command and maybe data from the TCP control connection
 */

static remif_cmd remif_getcmd (tcbtype *tcb, uchar *data, uint data_len)
{
    int status;
    uchar *tcp_data;
    int tcp_bytes_left;
    int data_bytes_left = -1;
    int cmd = 0;

    /* Repeatedly read from our socket until we have a complete command */

    for (;;) {
	status = tcp_getstring(tcb, &tcp_data, &tcp_bytes_left, MAXINT);
	switch (status) {
	  case OK:
	    break;

	  case NO_DATA:
	    edisms((blockproc *)tcpmiscBLOCK, (ulong) tcb);
	    continue;

	  case END_OF_FILE:		/* normal termination */
	    return REMIF_CMD_CLOSE;

	  default:
	    errmsg(&msgsym(TCPERR, REMIF), status);
	    return REMIF_RESP_ERR;
	}

	/*
	 * We have a slab of tcp data, so we need to extract the command
	 * and length information from it, and possible extra data.
	 *
	 * We may even not get a complete command, so we may have to
	 * read another slab-o-data, which is why this is in a loop...
	 */

	while (tcp_bytes_left) {
	    if (cmd == 0) {
		cmd = *tcp_data++;
		tcp_bytes_left--;
		continue;
	    }
	    if (data_bytes_left < 0) {
		if (cmd == REMIF_CMD_LISTEN) {
		    *data++ = *tcp_data;
		}
		data_bytes_left = *tcp_data++;
		tcp_bytes_left--;
		if (data_bytes_left == 0) goto done;
		continue;
	    }
	    
	    *data++ = *tcp_data++;
	    data_bytes_left--;
	    tcp_bytes_left--;

	    if (data_bytes_left == 0) goto done;
	    
	}
    }

   /*
    * Since we may have read *MORE* than just our command, we need to 
    * put any leftover data back into the stream...
    */

done:
    if (tcp_bytes_left) 
	tcp_putback(tcb, tcp_bytes_left);

    return cmd;
}


/*
 * there's one of these per remote interface.
 * handle the control connection.
 */

static forktype remif_control (remif_t *remif)
{
    int cmd;
    char data[REMIF_MAXDATA];
    char *owner;
    int size;
    hwidbtype *hwidb;

    for (;;) {
	cmd = remif_getcmd(remif->tcb, data, sizeof(data));
	switch (cmd) {
	  case REMIF_CMD_OPEN:
	    owner = data + strlen(data) + 1;
	    if (!rc_open(remif, data, owner))
		break;
	    continue;

	  case REMIF_CMD_PAKPORT:
	    if (!ri_pakport(remif, *(ushort*)data))
		break;
	    continue;
	    
	  case REMIF_CMD_NEW_MAC:
	    size = *data;
	    if (!rc_new_mac(remif, size, &data[1]))
		break;
	    continue;

	  case REMIF_CMD_LISTEN:
	    size = *data;
	    if (!rc_listen(remif, size, &data[1]))
		break;
	    continue;

	  case REMIF_CMD_RESET:
	    if (!rc_reset(remif))
		break;
	    continue;
	    
	  case REMIF_CMD_SHUTDOWN:
	    continue;

	  case REMIF_CMD_NOSHUTDOWN:
	    hwidb = remif->hwidb;
	    if (hwidb->type == IDBTYPE_TMS380) {
		hwidb->reset(hwidb);
	    }
	    continue;

	  case REMIF_CMD_PROMISCUOUS:
	    if (!rc_promiscuous(remif))
		break;
	    continue;

	  case REMIF_CMD_CHASTE:
	    if (!rc_chaste(remif))
		break;
	    continue;

	  case REMIF_CMD_CLOSE:
	    /* all done */
	    break;

	  case REMIF_RESP_ERR:
	    /* error message should be emitted by remif_getcmd() */
	    break;

	  case REMIF_CMD_TBRIDGE:
	    if (!rc_tbridge(remif))
		break;
	    continue;

	  default:
	    errmsg(&msgsym(BADCMD, REMIF), cmd, remif->ip_remote);
	    break;
	}
	break;
    }
    rc_close(remif);
    process_kill(THIS_PROCESS);
}


/*
 * handle most of the memory allocation, etc. involved in
 * starting a remote interface.
 */

static void remif_open (tcbtype *tcb)
{
    int pid;
    char *errstr;
    remif_t *remif;

    remif = malloc(sizeof(remif_t));
    errstr = "remif_open";
    while (remif) {
	remif->tcb = tcb;
	remif->ip_remote = tcb->foreignhost.ip_addr;
	remif->listeners = NULL;

	remif->tunnelpid = cfork((forkproc *)remif_tunnel, (long) remif,
				 0 /* default stack */, "REMIF Tunnel",
				 0 /* no tty */);
	if (remif->tunnelpid == NO_PROCESS) {
	    errstr = NULL;
	    break;
	}

	pid = cfork((forkproc *)remif_control, (long) remif,
		    0 /* default stack */, "REMIF Control",
		    0 /* no tty */);
	if (pid == NO_PROCESS) {
	    errstr = NULL;
	    break;
	}
	queue_init(&remif->pakQ, 0);	/* may need a queue limit here */
	return;				/* successful return */
    }
    if (errstr)
	errmsg(&msgsym(NOMEM, REMIF), errstr);
    if (remif) {
	if (remif->tunnelpid > 0)
	    process_kill(remif->tunnelpid);
	free(remif);
    }
    tcp_abort(tcb);
    return;				/* error return */
}


/*
 * okay to accept this TCP connection?
 */

static boolean remif_netaccess (ulong sport, ulong dport, ipaddrtype addr)
{
    return TRUE;
}


static void remif_init (subsystype *subsys)
{
    reg_add_ip_udp_input(remif_udp_input, "remif_udp_input");
    reg_add_tcp_listen(REMIF_PORT, remif_open, "remif_open");
    reg_add_tcp_access_check(REMIF_PORT, remif_netaccess, "remif_netaccess");
    reg_add_media_interesting_addresses(remif_addresses, "remif_addresses");
    return;
}


SUBSYS_HEADER(remif,			/* name */
	      1,			/* major version */
	      0,			/* minor version */
	      1,			/* edit version */
	      remif_init,		/* init function */
	      SUBSYS_CLASS_PROTOCOL,	/* subsystem class */
	      "seq: iphost ipservice",
	      "req: iphost ipservice");
