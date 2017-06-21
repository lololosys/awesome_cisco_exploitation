/* $Id: iptrace.c,v 3.6.18.5 1996/07/06 05:53:54 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/iptrace.c,v $
 *------------------------------------------------------------------
 * iptrace.c -- IP trace command
 *
 * August 1989, Paulina Knibbe
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: iptrace.c,v $
 * Revision 3.6.18.5  1996/07/06  05:53:54  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.6.18.4  1996/06/03  22:28:42  lmetzger
 * CSCdi59001:  Traceroute is not re-entrant
 * Branch: California_branch
 * Moved all Globals to the tracetype structure.
 *
 * Revision 3.6.18.3  1996/05/17  11:25:06  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.4.4  1996/05/14  06:19:25  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 3.6.4.3  1996/05/05  23:13:27  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.6.4.2  1996/04/03  14:43:10  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.6.4.1  1996/03/22  18:53:58  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.6.18.2  1996/04/15  21:16:43  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.6.18.1  1996/03/18  20:17:39  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.6.2  1996/03/07  09:47:00  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.6.1  1996/02/20  01:00:24  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/08  20:11:41  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.5  1996/02/02  01:10:47  widmer
 * CSCdi48042:  Change functions in ip/iptrace.c to static
 *
 * Revision 3.4  1996/01/24  20:15:10  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.3  1996/01/11  02:26:50  jenny
 * CSCdi46513:  TCP/IP need code clean up (part 2)
 * . removed ip_tuba.c and clns_tuba.c.
 * . removed lots of tuba related registry stubs and code.
 *
 * Revision 3.2  1995/11/17  09:36:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:57:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/11  23:49:29  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.3  1995/06/30 05:21:11  hampton
 * Move externs for the random number generator to their own file.  Remove
 * redundant initialization of the random number generator.  [CSCdi36588]
 *
 * Revision 2.2  1995/06/13 21:48:06  gchristy
 * CSCdi35750:  IP: Trace doesnt understand ICMP admin prohibited
 * unreachables
 *  - add support for dealing with ICMP admin prohibited unreachables
 *
 * Revision 2.1  1995/06/07  21:00:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "ip.h"
#include "ttysrv.h"
#include "config.h"
#include "../ip/ip_registry.h"
#include "../ui/command.h"
#include "ipoptions.h"
#include "address.h"
#include "name.h"
#include "../util/random.h"
#include "../ui/common_strings.h"
#include "subsys.h"
#include "sys_registry.h"
#include "rtt_ipecho_ext.h"


#define MAXLEN 80       /* command buffer length */

/*
 * Forward declarations
 */
static void ip_trace_showprobe(boolean, boolean, tracetype *, ushort);
static boolean traceBLOCK(tracetype *);

static queuetype traceQ;			/* IP Echo queue */


/*
 * ip_trace
 * Execute the "trace" command for IP
 */

void ip_trace (char *args) 
{
    tracetype *data;
    ipaddrtype source;		/* source IP address */
    ipaddrtype destination;     /* destination of trace route */
    ushort probe, nprobes;      /* current probe; number at each level */
    boolean gotthere;           /* got to destination */
    ushort minttl, maxttl;      /* time to live range */
    ushort port;                /* start of dest port range */
    ulong seconds;              /* time to wait */
    boolean numericonly;        /* don't ask name server for symbolic name */
    boolean verbose;            /* print extra output */
    char buff [MAXLEN];         /* input buffer */
    boolean parsed;	        /* parsing status */
    nametype *namep;
    char throw_away;
    ipoptiontype ipoption;      /* long term storage for options */
    ipoptiontype *options;      /* per packet storage for options */
    idbtype *idb;
    ulong sport, whichoptions;
    int ttl;

    /*
     * Set default parameters
     */
    source = 0;
    nprobes = 3;
    minttl = 1;
    maxttl = 30;
    numericonly = FALSE;
    verbose = FALSE;
    port = 33434;
    seconds = 3;
    options = NULL;
    ipoption.length = 0;
    ipoption.paddedlength = 0;
    sport = random_gen() | 0x8000;
    whichoptions = 0;

    /*
     * Get IP address or collect extended paramters.
     */
    if (!parse_hostname(args, &destination)) {
        printf("\nTarget IP address: ");
	if (!rdtty(buff, MAXLEN) || !parse_hostname(buff, &destination)) {
	    printf("\n%% Bad IP address or host name");
	    return;
	}
	printf("\nSource address: ");
	parsed = rdtty(buff, MAXLEN);
	if (parsed) {
	    if (!null(buff)) {
		/*
		 * Confirm that the address is one of ours by scanning
		 * all interfaces.
		 */
		if (parse_hostname(buff, &source)) {
		    FOR_ALL_SWIDBS(idb){
			parsed = ip_ifaddress(idb, source);
			if (parsed)
			    break;
		    }
		} else {
		    parsed = FALSE;
		}
	    }
	}
	if (!parsed) {
	    printf("\n%% Invalid source address");
	    return;
	}
	printf("\nNumeric display [n]: ");
	parsed = rdtty(buff, MAXLEN);
	if (parsed) {
	    if (!null(buff)) {
		if (toupper(*buff) == 'Y')
		    numericonly = TRUE;
	    }
	}
	if (!parsed) {
	    printf("\n%% Y or N");
	    return;
	}
	if (!trace_common(buff, &seconds, &nprobes,
			  &minttl, &maxttl, TTL_DEFAULT))
	    return;
	printf("\nPort Number [%u]: ", port);
	parsed = rdtty(buff, MAXLEN);
	if (parsed) {
	    if (!null(buff))
	        port = parse_unsigned(buff, &parsed);
	    parsed = parsed && (port != 0);
	}
	if (!parsed)
	    printf("\nBad Port Number");

	/*
	 * If the user wants options, allocate memory for them 
	 * return a pointer to the options in 'ipoption'
	 * Note that the accumulated options are in ipoption.
         */
	options = malloc(sizeof(ipoptiontype));
        if (options == NULL)
	    printf(nomemory);
	else {
	    whichoptions = get_options(&ipoption, options, buff, &verbose,
				       destination); 
            /* 
             * The user may have said let's do options but then did not
             * actually select any. Return the allocated memory.
            */
            if (ipoption.paddedlength == 0 && options) {
	        free(options);
		options = NULL;
	    }
	  }
    }
  
    /*
     * All parsing is done!  Let's actually start sending some packets.
     */
    psion(stdio);
    printf("%s%s", type_escape_seq_abort_msg, tracing_the_route_to_msg);
    if (numericonly){
        printf("%i\n", destination);
    } else {
	namep = reg_invoke_ip_address_lookup(destination);
	if (namep) {
	    printf("%s (%i)\n", namep->name, destination);
	} else {
	    printf("%i\n", destination);
	}
    }
    flush();
    gotthere = FALSE;
    data = malloc(sizeof (tracetype));
    if (!data)
	return;
    data->currentGateway = 0;
    data->interactive = TRUE; /* This function is always interactive */
    
    for (ttl = minttl; ttl <= maxttl; ttl++){
	printf("\n %-2u ", ttl);
	for (probe = 0; probe < nprobes; ++probe) {
	    data->sport = random_gen() | 0x8000; /* make sure port is not low */
	    data->ttl = ttl;
	    /*
	     * Each probe should have its own dport so we can distinguish
	     * responses. Also, copy a fresh version of the user
	     * specified options.
             */
	    data->dport = port++;
	    TIMER_START(data->timeout, seconds * ONESEC);
	    if (options != NULL)
	  	bcopy(&ipoption, options, sizeof(ipoptiontype));
	    data->options = options;
	    data->whichoptions = whichoptions;

	    /*
	     * Actually send a packet
	     */
	    ip_trace_sendprobe(source, destination, data);
	    switch(data->how) {
	    case '!':    /* success ! */
		ip_trace_showprobe(verbose, numericonly, data, probe);
		break;
	    case 'U':   /* got to the dest - quit now */
		ip_trace_showprobe(verbose, numericonly, data, probe);
		gotthere = TRUE;
		break;
	    case 'A':
	    case 'N':
	    case 'H':
	    case 'P':
		ip_trace_showprobe(verbose, numericonly, data, probe);
		gotthere = TRUE;
		break;
	    case 'Q':   /* source quench received */
		ip_trace_showprobe(verbose, numericonly, data, probe);
		break;
	    case 'I':   /* user interrupted */
		goto cleanup;
	    case 'T':   /* timeout */
		printf(" * ");
		break;
	    default:
		printf(" ? ");
		break;
	    } /* switch */
	    flush();
	    /*
	     * The following read of getc is an attempt to get the PSIPENDING
	     * set if the user types an escape when we are using a vty
	     */
	    while ((throw_away = getc(stdio)) != -1)
	        ;
	} /* for  probe */
	if (gotthere)
	     goto cleanup;
    } /* for  ttl */
    if (verbose)
	printf("\nDestination not found inside max TTL diameter.");

cleanup:
    flush();
    free(data);
    psioff(stdio);
    free(options);
}

/*
 * ip_trace_showorigin
 * Display the AS number of a given respondent (if available)
 */
static void ip_trace_showorigin (ipaddrtype who)
{
    ushort buff[60];
    int i, buflen;

    buflen = reg_invoke_ip_origin(who, 0, buff, sizeof(buff)/sizeof(ushort));
    if (buflen) {
	printf(" [AS ");
	for (i = 0; i < buflen-1; i++)
	    printf("%d,", buff[i]);
	printf("%d]", buff[i]);
    }
}

/* 
 * ip_trace_showprobe
 * Display the results of a successful traceroute probe
 */

static void ip_trace_showprobe (boolean verbose, boolean numericonly, 
				tracetype *data, ushort probe)
{
    nametype *namep;

    if ((probe != 0) && (data->who != data->currentGateway)) {
        /* first response from this gateway */
	printf("\n    ");
	if (!numericonly) {
	    /* look for symbolic name for this node */
	    if ((namep = reg_invoke_ip_address_lookup(data->who)) != NULL)
	        printf("%s (%i)", namep->name, data->who);
	    else
		printf("%i", data->who);
        } else
	    printf("%i", data->who);

	ip_trace_showorigin(data->who);
    }
    data->currentGateway = data->who;
    if (probe == 0) {
        if (!numericonly) {
	    /* look for symbolic name for this node */
	    if ((namep = reg_invoke_ip_address_lookup(data->who)) != NULL)
	        printf("%s (%i)", namep->name, data->who);
       	    else
		printf("%i", data->who);
        } else
	    printf("%i", data->who);

	ip_trace_showorigin(data->who);
    }
    if ((data->how == '!') || (data->how == 'U'))
	printf(" %lu msec", data->deltatime);
    else
	printf (" !%c ", data->how);
    if (verbose && validmem(data->options) && data->options->paddedlength) {
        printf("\nReceived packet has options\n");
	print_ipoptions(data->options);
    }
}

/*
 * ip_trace_sendprobe
 * Send a probe packet to the traceroute destination
 */

void ip_trace_sendprobe (ipaddrtype source, ipaddrtype destination, 
				tracetype *data)
{
    paktype *pak;
    udptype *udp;
    ipparamtype params;
    long length;

    length = IPHEADERBYTES(data->options) + UDPHEADERBYTES;
    pak = getbuffer(IPHEADERBYTES(data->options) + UDPHEADERBYTES);
    if (!pak)
	return;

    /*
     * Fill in the UDP and IP fields, then send the probe.
     * If user hasn't already chosen a source address, choose one now.
     */
    udp = (udptype *)((uchar *)ipheadstart(pak) + IPHEADERBYTES(data->options));
    udp->sport =  data->sport;
    udp->dport = data->dport;
    udp->length = UDPHEADERBYTES;
    if (source == 0)
        source = ip_best_local_address(destination, FALSE);
    udp->checksum = 0;
    /*
     * Don't compute the checksum if there is a source route.  Along the
     * path, the IP destination field will be changed, throwing the
     * checksum off.
     */
    if (!(data->whichoptions & (PAKOPT_HASSSR|PAKOPT_HASLSR)))
	udp->checksum = CheckSum((uchar *)udp, udp->length, source,
				 destination, UDP_PROT); 
    params = ipdefaultparams;
    params.ttl = data->ttl;
    GET_TIMESTAMP(data->timesent);
    if (ip_aliasaddress(destination))
	pak->flags |= PAK_ALIAS;
    ipwrite(pak, length, UDP_PROT, source, destination, NULL,
	    data->options, &params);

    /*
     * Put self on queue to wait for response, then block.
     */
    data->next = NULL;
    data->active = TRUE;
    data->tty = stdio;
    data->conntop = stdio->conntop;
    enqueue(&traceQ, data);
    edisms((blockproc *)traceBLOCK, (ulong) data);
    if (data->active)
        unqueue(&traceQ, data);
}

/*
 * traceBLOCK
 * Wait for ICMP "time exceeded" message
 */

static boolean traceBLOCK (tracetype *data)
{
    if (!data->active)
	return(FALSE);              /* if block flag reset, wakeup */
    if (data->interactive &&        /* if interactive */
        psipending2(data->tty)) {   /* interrupted? */
        data->how = 'I';
	return(FALSE);
    }
    if (!SLEEPING(data->timeout)) {  /* if we haven't heard yet, give up */
        data->how = 'T';
	return(FALSE);               
    }
    return(TRUE);                 /* wait for one of the conditions above */
}

/*
 * udp_unreachable
 * A UDP packet has been returned as unreachable.
 * Check if it was sent in response to a traceroute probe.
 */

static void udp_unreachable (void *netstart, ipaddrtype source, void *icmpstart)
{
    icmptype *icmp;
    uchar code;
    iphdrtype *ip;
    tracetype *tp;
    udptype *udp;	   
    tt_soc *socket;
    ipsocktype *soc;
    ushort sport, dport;
    addrtype faddr;

    icmp = icmpstart;
    ip = netstart;
    code = icmp->code;
    udp = (udptype *)ipdatastart_iph(ip);
    sport = n2h(udp->dport);
    dport = n2h(udp->sport);
    faddr.type = ADDR_IP;
    faddr.ip_addr = ip->dstadr;

    soc = match_udpsocket(sport, dport, &faddr, NULL); 
    if (soc && (soc->status & SOCK_NETWORK_ERROR)) {
	process_set_boolean(soc->errflag, TRUE);
	soc->error = code;
	return;
    }
    for (tp = (tracetype *) traceQ.qhead; tp; tp = tp->next) {
	/*
	 * check if the originating connection has gone away
	 */
	socket = tp->tty;
	if (((long)socket->tiptop == -1) || (socket->conntop != tp->conntop)){
	    unqueue(&traceQ, tp);
	    /* 
	     * Tell the process that put tp on the TraceQ that it
	     *   is inactive, and they are responsible to free tp.
	     *   If the free is done here the edism and caller will
	     *   use tp after it has been freed
	     */
	    tp->active = FALSE;
	    return;
	}
        if ((udp->dport == tp->dport) && (udp->sport == tp->sport)) {
	    tp->deltatime = ELAPSED_TIME(tp->timesent);
	    switch (code) {
	    case ICMP_UNRNET:
		tp->how = 'N';
	        break;
	    case ICMP_UNRHOST:
		tp->how = 'H';
	        break;
	    case ICMP_UNRPROTOCOL:
		tp->how = 'P';
	        break;
	    case ICMP_UNRPORT:
		tp->how = 'U';
	        break;
	    case ICMP_UNRADMIN:
		tp->how = 'A';
		break;
	    default:
		tp->how = '?';
	        break;
	    }
	    tp->who = source;
	    tp->active = FALSE;
	    if (tp->options)
		extract_ipoptions(ip, tp->options);
	    unqueue(&traceQ, tp);
	    break;
	} /* if */
    } /* for */
}

/*
 * udp_quench
 * What to do when UDP protocol gets a source quench.
 * Take action only if returned packet was sent by traceroute
 */

static void udp_quench (void *netstart, ipaddrtype source)
{
    iphdrtype *ip;
    tracetype *tp;
    udptype *udp;
    tt_soc *socket;

    ip = netstart;
    udp = (udptype *)ipdatastart_iph(ip);
    for (tp = (tracetype *) traceQ.qhead; tp; tp = tp->next) {
	/*
	 * check if the originating connection has gone away
	 */
	socket = tp->tty;
	if ((socket->tiptop == -1) || (socket->conntop != tp->conntop)){
	    unqueue(&traceQ, tp);
	    /* 
	     * Tell the process that put tp on the TraceQ that it
	     *   is inactive, and they are responsible to free tp.
	     *   If the free is done here the edism and caller will
	     *   use tp after it has been freed
	     */
	    tp->active = FALSE;
	    return;
	}
	if ((udp->dport == tp->dport) && (udp->sport == tp->sport)) {
	    tp->deltatime = ELAPSED_TIME(tp->timesent);
	    tp->how = 'Q';
	    tp->who = source;
	    tp->active = FALSE;
	    if (tp->options)
		extract_ipoptions(ip, tp->options);
	    unqueue(&traceQ, tp);
	    break;
	}
    }
}

/*
 * rcv_timeexceeded 
 * We got a Time Exceeded ICMP packet.
 * Check if there is a traceroute packet outstanding.
 */

static void rcv_timeexceeded (paktype *pak, void *icmpstart)
{
    icmptype *icmp;
    iphdrtype *ip;
    tracetype *tp;	       /* data for each outstanding packet */
    iphdrtype *orig_ip;
    udptype *udp;              /* udp header of original packet */
    tt_soc *socket;

    icmp = icmpstart;
    ip = (iphdrtype *)ipheadstart(pak);

    /*
     * If the embedded packet is not a udp packet - return
     */
    orig_ip = (iphdrtype *)(icmp->data);
    if (orig_ip->prot != UDP_PROT) {
	retbuffer(pak);
	return;
    }

    for (tp = (tracetype *)traceQ.qhead; tp; tp=tp->next) {
	/*
	 * check if the originating connection has gone away
	 */
	socket = tp->tty;
	if ((socket->tiptop == -1) || (socket->conntop != tp->conntop)){
	    unqueue(&traceQ, tp);
	    /* 
	     * Tell the process that put tp on the TraceQ that it
	     *   is inactive, and they are responsible to free tp.
	     *   If the free is done here the edism and caller will
	     *   use tp after it has been freed
	     */
	    tp->active = FALSE;
	    retbuffer(pak);
	    return;
	}
        /* 
	 * Get the address of the UDP header
	 */
        udp = (udptype *)((char *)icmp->data + (orig_ip->ihl << 2));

        /* 
         * Confirm that:
         * 1) the destination port of the original
         *       packet == our destination port
         * 2) the source port of the original packet 
         *       == our source port (pid)
         */
 	if ((udp->dport == tp->dport) && (udp->sport == tp->sport)) {
	    tp->deltatime = ELAPSED_TIME(tp->timesent);
	    tp->how = '!';
	    tp->who = ip->srcadr;
	    tp->active = FALSE; 	/* active waiting process */
	    if (tp->options)
		extract_ipoptions(orig_ip, tp->options);
	    unqueue(&traceQ, tp);
	    break;
        }
    }
    retbuffer(pak);
}

/*
 * ipdiag_init
 * Initialize IP diagnostic support
 */

static void ipdiag_init (subsystype *dummy)
{
    reg_add_trace_run(LINK_IP, ip_trace, "ip_trace");
    reg_add_ping_parse(ip_ping_parse, "ip_ping_parse");
    reg_add_ping_run(LINK_IP, ip_ping, "ip_ping");
    reg_add_ip_trace_rcvexceed(rcv_timeexceeded, "rcv_timeexceeded");
    reg_add_ip_icmp_rcvquench(UDP_PROT, udp_quench, "udp_quench");
    reg_add_ip_icmp_rcvunreach(UDP_PROT, udp_unreachable, "udp_unreachable");
    register_snmp_ip_echo();	/* register snmp ping functions */
    register_rtt_mon();         /* register rtr collection */
}

/*
 * IPDIAG subsystem header
 */

#define IPDIAG_MAJVERSION 1
#define IPDIAG_MINVERSION 0
#define IPDIAG_EDITVERSION  1

SUBSYS_HEADER(ipdiag, IPDIAG_MAJVERSION, IPDIAG_MINVERSION, IPDIAG_EDITVERSION,
	      ipdiag_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
	      subsys_req_iphost);

