/* $Id: decnet_echo.c,v 3.3.18.1 1996/03/18 19:11:31 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/decnet/decnet_echo.c,v $
 *------------------------------------------------------------------
 * D E C n e t _ e c h o . c
 *
 * July 1992
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This module contains all the routines for the DECnet echo features.
 *------------------------------------------------------------------
 * $Log: decnet_echo.c,v $
 * Revision 3.3.18.1  1996/03/18  19:11:31  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/14  00:53:07  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.2.26.2  1996/03/07  08:42:30  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  00:43:28  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/08  20:10:18  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.2  1995/11/17  08:58:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:15:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/07/10  20:37:21  asastry
 * CSCdi36667: Allow DECnet pings to make use of 'size' in extended mode.
 *
 * Revision 2.4  1995/07/07  22:46:33  hampton
 * Convert DECNET to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi36830]
 *
 * Revision 2.3  1995/06/18 19:40:20  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  12:59:42  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:22:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <ciscolib.h>
#include "packet.h"
#include "address.h"
#include "decnet.h"
#include "ttysrv.h"
#include "../ui/debug.h"
#include "decnet_echo.h"
#include "decnet_externs.h"
#include "decnet_debug.h"
#include "../clns/clns.h"
#include "sys_registry.h"
#include "subsys.h"
#include "../snmp/sr_pingmib.h"
#include "../snmp/snmp_registry.h"
#include "../ui/common_strings.h"
#include "../ui/command.h"

static int snmp_decnet_echo_validate (ciscoPingEntry_t *entry);
static void snmp_decnet_echo_activate (ciscoPingEntry_t *entry);


/*
 * decnet_ping_parse
 * Parse a ping command line.  Called by SERVICE_PING_PARSE.
 * Return TRUE if positive identification of a DECnet address.
 */

boolean decnet_ping_parse (char *args, int hint, int *index,
			   char **cp, boolean *running)
{
    int dn_area, dn_node;

    *running = dn_running;
    *cp = "DECnet";
    *index = LINK_DECNET;

    if (!dn_running)
	return (FALSE);

    if (hint < 0)
	return(strmatch(args, "decnet", FALSE));
    else if (hint == LINK_ILLEGAL) {
	if (parse_decnetaddr(args, &dn_area, &dn_node))
	    return(TRUE);
    } else if ((hint == LINK_DECNET) &&
	       (!args || !*args || parse_decnetaddr(args, &dn_area,
						    &dn_node)))
	return(TRUE);
    return(FALSE);
}

/*
 * dn_ping
 * Perform DECnet ping
 */
void dn_ping (char *args)
{
    char buff[MAX_BUFLEN];
    int i, area, node;
    long size, repeat, seconds, hits;
    dn_echo_cb_t *data;
    boolean verbose;
    long tmin = 999999999, tmax = 0, tsum = 0;
    repeat = 5;				/* default repeat count */
    seconds = 5;			/* default timeout */
    verbose = FALSE;			/* verbose */
    size = 100;                         /* default datagram size */                

    /*
     * get DECnet address
     */
    if (!parse_decnetaddr(args, &area, &node)) {
	printf("Target DECnet address: ");
	if (!rdtty(buff, MAX_BUFLEN)) {
	    printf("\n%% Bad DECnet address");
	    return;
	} else if (!parse_decnetaddr(buff, &area, &node)) {
	    printf("\n%% Bad DECnet address");
	    return;
	}

	/*
	 * Ask for repeat count, size, and timeout interval
	 */
	if (!ping_common(&repeat, &size, DN_MINECHOSIZE, DN_MAXECHOSIZE, 
			 &seconds))
	    return;
	
	/*
	 * Ask for verbosity
	 */
	printf("\nVerbose [n]: ");
	if (rdtty(buff, MAX_BUFLEN) && toupper(*buff) == 'Y') 
	    verbose = TRUE;
    }

    /* Let's do it */
    psion(stdio);
    printf(type_escape_seq_abort_msg);
    printf("\nSending %d, %d-byte DECnet echos to %d.%d, timeout is %d seconds:",
		repeat, size, area, node, seconds);
    flush();

    for (i = 0, hits = 0; (i < repeat) && (psipending() == FALSE); i++) {
	data = malloc(sizeof(dn_echo_cb_t));
	if (!data)
	    return;
	if ((i % 70) == 0 && !verbose) {
	    printc('\n');
	    flush();
	}

	data->flags = 0;
	GET_TIMESTAMP(data->xmittime);
	TIMER_START(data->timeout, seconds * ONESEC);
	data->echo_flavor = TRUE;
	if (dn_send_echo(area, node, data, size)){
	    switch(data->how) {
	      case '!':		/* good reply */
		hits++;
		/*
		 * If we got a reply before the clocked ticked, turn a zero
		 * into a one to avoid dumb questions from customers.
		 */
		if (data->deltatime == 0L)
		    data->deltatime = 1L;
		tsum += data->deltatime;
		if (data->deltatime < tmin)
		    tmin = data->deltatime;
		if (data->deltatime > tmax)
		    tmax = data->deltatime;
		if (verbose)
		    printf("\n%d in %d ms",i,data->deltatime);
		break;
	      case '+':
		printf("\nHost unreachable");
		break;
	      case 'I':
	      case 'T':
		p_unqueue(&DNechoQ, data);		
		break;
	      case '&':
	      case 'U':
	      case 'C':
		break;
	      default:
		if (verbose)
		    printf("\nunknown packet type %#x", data->how);
		else
		    data->how = '?';
		break;
	    }
	    if (!verbose)
		printc(data->how);
        } else {
	    if (!verbose)
		printc('.');
	    else
		printf("\n%d timed out", i);
	}
	flush();
	free(data);
    }
    if (i > 0) {
	printf("\nSuccess rate is %d percent (%d/%d)",
	    (hits*100)/i, hits, i);
	if (hits) {
	    /*
	     * If we got a reply before the clocked ticked, turn a zero
	     * into a one to avoid dumb questions from customers.
	     */
	    if (tmin == 0)
		tmin = 1;
	    printf(", round-trip min/avg/max = %d/%d/%d ms",
		tmin, tsum/hits, tmax);
	}
    }

    return;
}

/*
 * dn_send_echo
 *
 */

boolean dn_send_echo (
    int area,
    int node,
    dn_echo_cb_t *echo_packet,
    long data_size)
{
    dnnsplinkpkt *nsp;
    paktype *pak;
    dnhdrtype *dn;
    dnnet *net;
    dndatapacket *routing_data;
    int i;

    /*
     * Find first non-zero network number.
     */

    net = NULL;
    for (i = 0; i < DN_MAXNETS; i++) {
	if (dn_nets[i]) {
	    net = dn_nets[i];
	    break;
	}
    }

    if (!net)
	return (FALSE);

    /* Special case if you are pinging yourself */
    if ((area == net->my_dn_area) && (node == net->my_dn_node)) {
	echo_packet->deltatime = ELAPSED_TIME(echo_packet->xmittime);
	echo_packet->how = '!';
	return (TRUE);
    }

    pak = getbuffer(data_size);
    if (!pak)
      return (FALSE);

    dn = (dnhdrtype *)dnheadstart(pak);

    routing_data = (dndatapacket *)&dn->pakstart[0];
    nsp = (dnnsplinkpkt *) &routing_data->databyte;
    
    bcopy(dn_e_baseaddr, routing_data->source_hi, 4);
    dn_addr(routing_data->source, net->my_dn_area, net->my_dn_node);
    bcopy(dn_e_baseaddr, routing_data->dest_hi, 4);
    dn_addr(routing_data->dest, area, node);
    routing_data->typecode = 0x2e;	
    routing_data->visits = 0;

    nsp->msgflg = NSP_LINK_SERVICE;

    dn_addr(nsp->srcaddr, net->my_dn_area, net->my_dn_node);
    dn_addr(nsp->dstaddr, area, node); 

    nsp->acknum[0] = 0x00;
    nsp->acknum[1] = 0x00;
    nsp->ackdat[0] = 0x00;
    nsp->ackdat[1] = 0x00;
    nsp->segnum[0] = 0x00;
    nsp->segnum[1] = 0x00;
    nsp->lsflags = 0;
    nsp->fcval = 0;

    /*
     * Set the packet size.  Size of the headers plus the 
     * faked up connect data.
     */
    put16(dn->paksize, data_size);
    pak->if_input = NULL;
    pak->clns_flags = 0;
    
    echo_packet->next = NULL;
    echo_packet->active = TRUE;
    echo_packet->tty = stdio;
    dn_addr(echo_packet->echo_dest, area, node);

    /* 
     * enqueue the echo packet in the DECnet echo queue, and
     * forward the packet we created.
     */
    p_enqueue(&DNechoQ, echo_packet);
    dn_forward(pak, routing_data, routing_data->typecode);

    edisms((blockproc *)dn_echoBLOCK, (ulong)echo_packet);
    if (echo_packet->active){
	p_unqueue(&DNechoQ, echo_packet);
    }
    if (echo_packet->echo_flavor){
	return (echo_packet->active ? FALSE : TRUE);
    }		
    else
	return (TRUE);
}

/*
 * dn_disconnect
 *
 * If the router receives a Connect Initiate addressed to us, 
 * then send back a Disconnect Initiate with reason "Destination
 * end user does not exist".  Discard anything else addressed to us.
 * Return TRUE if we should reroute the packet.
 */

boolean dn_disconnect (
    register paktype *pak,
    register dndatapacket *data,
    register dnnet *net)
{
    int len, padsize;
    unsigned char t1, t2;
    dn_echo_cb_t *echo_packet, *next;
    boolean connect_initiate_pkt;

    dnnspdispkt *nsp;
    dnhdrtype *dn;

    dn  = (dnhdrtype *)dnheadstart(pak);
    nsp = (dnnspdispkt *) &data->databyte;

    padsize = (dn->pakstart[0] & DN_PAD) ? (dn->pakstart[0] & 0x7f) : 0;
    len = get16(dn->paksize) - padsize;

    /* 
     * Make sure packet is large enough 
     */
    if (len < (sizeof(dndatapacket) + sizeof(dnnspdispkt) - 
               sizeof(nsp->data) - sizeof(data->databyte))) {
        retbuffer(pak);
	return(FALSE);
    }

    dnet_pktbuginf("\nSource address: %d.%d", 
		 dn_area(data->source), dn_node(data->source));

    /* If we got an NSP packet with DISCONNECT INITIATE, it means we sent an
     * echo packet and we are getting a response. If we got one with a
     * CONNECT request, then we are either the target of the echo or an
     * intermediate hop. In the case of the CONNECT, we need to build a 
     * response packet with a DISCONNECT flag and send it back. If we 
     * received a DISCONNECT, we either heard from the node we were trying
     * to 'ping' or from a router en-route.
     */
    if ((nsp->msgflg == NSP_DISC_INITIATE) || 
	(nsp->msgflg == NSP_DISC_CONFIRM)) {
        dnet_pktbuginf("\nDNET: echo response from %d.%d \n",
	       	    dn_area(data->source), dn_node(data->source));

       /*
        * Make sure that someone is waiting for this echo packet.
        */
        if (QUEUEEMPTY(&DNechoQ)) {
            dnet_pktbuginf("\n<<<<<< Q empty >>>>>>>");
            retbuffer(pak);
	    return(FALSE);
        }

	echo_packet = (dn_echo_cb_t *)DNechoQ.qhead;
        while (echo_packet) {
 	    next = (dn_echo_cb_t *)echo_packet->next;
	    if (get16(echo_packet->echo_dest) == get16(nsp->srcaddr)) {
	        echo_packet->deltatime = ELAPSED_TIME(echo_packet->xmittime);
	        echo_packet->how = '!';
	        echo_packet->active = FALSE;
	        p_unqueue(&DNechoQ, echo_packet);
	    }
	    echo_packet = next;
        }
	retbuffer(pak);
	return(FALSE);
    } /* if disconnect request */
    
    connect_initiate_pkt = (nsp->msgflg == NSP_CONNECT) ||
	                   (nsp->msgflg == NSP_RECONNECT) ||
	                   (nsp->msgflg == NSP_CON_CONFIRM);

    /*
     * Verify that we have a link service packet or connect initiate packet.
     */
    if ((nsp->msgflg != NSP_LINK_SERVICE) && !connect_initiate_pkt) {
        retbuffer(pak);
	return(FALSE);
    }

    /*
     * If the packet was a connect initiate packet, just return a DISCONNECT
     * and do not bother handling the echo stuff.
     *
     * If a packet was returned because the destination was not
     * reachable, it means that the router that decided to return it
     * just flipped the source & destination addresses and sent back the
     * packet - with the NSP type still LINK SERVICE. In this case,
     * we need to discard the packet and flag the destination as
     * unreachable. We can do this by checking the echo queue - if there
     * is an echo packet with a destination the same as the source of the 
     * packet, and the incoming packet DN_RET bit is set, then discard the 
     * packet. Note that if the destination WAS reachable, we would have seen 
     * a DISCONNECT message and handled it elsewhere.
     */
     if (!connect_initiate_pkt) {
         echo_packet = (dn_echo_cb_t *)DNechoQ.qhead;
         while (echo_packet) {
             next = (dn_echo_cb_t *)echo_packet->next;
	     if (get16(echo_packet->echo_dest) == get16(nsp->dstaddr)) {
	         if (data->typecode & DN_RET) {
	             echo_packet->how = '+';
	             echo_packet->active = FALSE;
		     retbuffer(pak);
	             p_unqueue(&DNechoQ, echo_packet);
		     return (FALSE);
	        }
             }
	     echo_packet = next;
         }
     }

    t1 = data->dest[0];			/* flip packet src, dst */
    t2 = data->dest[1];
    data->dest[0] = data->source[0];
    data->dest[1] = data->source[1];
    data->source[0] = t1;
    data->source[1] = t2;
    data->typecode &= ~DN_RETREQ;	/* don't want returns */
    data->visits = 0;
    
    /*
     * Flip the logical links.  
     */
    t1 = nsp->dstaddr[0];		 
    t2 = nsp->dstaddr[1];
    nsp->dstaddr[0] = nsp->srcaddr[0];	
    nsp->dstaddr[1] = nsp->srcaddr[1];
    nsp->srcaddr[0] = t1;
    nsp->srcaddr[1] = t2; 
    
    nsp->msgflg = NSP_DISC_INITIATE;
    nsp->reason[0] = 0x04;		/* End user does not exist */
    nsp->reason[1] = 0x00;
    
    /* Since we reuse the pak, we must clear the flag as the original 
     * pak may already be converted from clns.
     */
    pak->clns_flags &= ~CLNS_CONVERTED;

    /*
     * Set the packet size.  Account for the extra byte in dndatapacket
     * which is really a byte in the nsp packet (data->databyte).  Also
     * account for the pad bytes in the original packet.
     */
    put16(dn->paksize, sizeof(dndatapacket) + sizeof(dnnspdispkt) +
	  padsize - sizeof(data->databyte));
    
    return(TRUE);
}

/*
 * dn_echoBLOCK
 * Wait for an DECnet Echo Reply or timeout.
 */

boolean dn_echoBLOCK (ulong arg)
{
    register dn_echo_cb_t *data;

    data = (dn_echo_cb_t *) arg;

    if (!data->active)			/* if block flag reset, wakeup */
	return(FALSE);
    if (psipending2(data->tty)) {	/* interrupted? */
	data->how = 'I';
	return(FALSE);
    }
    if (TIMER_RUNNING_AND_AWAKE(data->timeout)) {
	/* keep blocking if timer ticking */
	data->how = 'T';
	return(FALSE);
    }
    return(TRUE);
}


/*
 * snmp_decnet_echo_validate
 * Validate a decnet echo for SNMP
 * Called via the snmp_ping_validate registry
 */

static int snmp_decnet_echo_validate (ciscoPingEntry_t *entry)
{
    /*
     * there is a real problem if the protocol isn't set, or if it
     * isn't set to the same value as the case registry switch value
     */
    if (!VALID(I_ciscoPingProtocol, entry->valid) ||
	(entry->ciscoPingProtocol != D_ciscoPingProtocol_decnet))
	return (SNMP_PING_INVALID_PROTOCOL);

    /*
     * If the address isn't set, we simply continue
     * (the activate routine won't be called by snmp until an address has
     * been set and validated)
     */
    if (VALID(I_ciscoPingAddress, entry->valid) &&
	entry->ciscoPingAddress->length != ADDRLEN_DECNET)
	return (SNMP_PING_INVALID_ADDRESS);

    /*
     * if the packet size is set, it better be within the legal range.
     * if the packet size is not set, we simply continue (the activation
     * routine won't be called by snmp until a packet size has been set
     * and validated)
     */
    if (VALID(I_ciscoPingPacketSize, entry->valid) &&
	((entry->ciscoPingPacketSize < DN_MINECHOSIZE) ||
	(entry->ciscoPingPacketSize > DN_MAXECHOSIZE)))
	return (SNMP_PING_INVALID_SIZE);

    return (SNMP_PING_VALID);
}


/*
 * snmp_decnet_echo_activate
 * Send a decnet echo for SNMP
 * Called via the snmp_ping_activate registry
 */
static void snmp_decnet_echo_activate (ciscoPingEntry_t *entry)
{
    int area, node;
    dn_echo_cb_t *data;
    ulong total_rtt, idx;

    /*
     * Recover the area and node from the OctetString.
     * The address is only 2 bytes, the first 6 bits are the area,
     * and the last 10 bits are the node.  Remember to ping '58.186'
     * user must use 'e8 ba' octet string.
     */
    area = (entry->ciscoPingAddress->octet_ptr[0] >> 2);
    node = ( ((entry->ciscoPingAddress->octet_ptr[0] & 0x03) << 8) +
       entry->ciscoPingAddress->octet_ptr[1]);

    entry->ciscoPingSentPackets = 0;
    SET_VALID(I_ciscoPingSentPackets, entry->valid);
    entry->ciscoPingReceivedPackets = 0;
    SET_VALID(I_ciscoPingReceivedPackets, entry->valid);
    entry->ciscoPingCompleted = D_ciscoPingCompleted_false;
    SET_VALID(I_ciscoPingCompleted, entry->valid);
    total_rtt = 0;

    for (idx = 1; idx <= entry->ciscoPingPacketCount; idx++) {
	entry->ciscoPingSentPackets++;
	data = malloc(sizeof(dn_echo_cb_t));
	if (!data)
	    return;

	data->flags = 0;
	GET_TIMESTAMP(data->xmittime);
	TIMER_START(data->timeout, entry->ciscoPingPacketTimeout);
	data->echo_flavor = TRUE;
	if (dn_send_echo(area, node, data, DN_MINECHOSIZE)){
	    if (data->how == '!') {
		entry->ciscoPingReceivedPackets++;
		if (data->deltatime == 0L)
		    data->deltatime = 1L;
		total_rtt += data->deltatime;
		if ((!VALID(I_ciscoPingMinRtt, entry->valid)) ||
			(data->deltatime < entry->ciscoPingMinRtt)) {
		    entry->ciscoPingMinRtt = data->deltatime;
		    SET_VALID(I_ciscoPingMinRtt, entry->valid);
		}
		if ((!VALID(I_ciscoPingMaxRtt, entry->valid)) ||
			(data->deltatime > entry->ciscoPingMaxRtt)) {
		    entry->ciscoPingMaxRtt = data->deltatime;
		    SET_VALID(I_ciscoPingMaxRtt, entry->valid);
		}
		entry->ciscoPingAvgRtt =
		    total_rtt / entry->ciscoPingReceivedPackets;
		SET_VALID(I_ciscoPingAvgRtt, entry->valid);
	    }
	}
	free(data);
	if (idx < entry->ciscoPingPacketCount) {
	    process_sleep_for(entry->ciscoPingDelay);
	}
    }
    entry->ciscoPingCompleted = D_ciscoPingCompleted_true;
    return;
}


void register_snmp_decnet_echo(void)
{
    reg_add_snmp_ping_validate(D_ciscoPingProtocol_decnet,
		snmp_decnet_echo_validate,
		"snmp_decnet_echo_validate");

    reg_add_snmp_ping_activate(D_ciscoPingProtocol_decnet,
		snmp_decnet_echo_activate,
		"snmp_decnet_echo_activate");
}

