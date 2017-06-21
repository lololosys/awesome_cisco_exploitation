/* $Id: icmpping.c,v 3.8.18.4 1996/05/21 09:51:01 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/icmpping.c,v $
 *------------------------------------------------------------------
 * icmpping.c -- ICMP echo command
 * 
 * June 1985, Kirk Lougheed
 * July 1987, Bill Westfield  add IP option support
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: icmpping.c,v $
 * Revision 3.8.18.4  1996/05/21  09:51:01  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8.18.3  1996/05/11  02:30:09  tylin
 * CSCdi56413:  Multicast ping should have a ttl option
 *              Added for extended command only.
 * Branch: California_branch
 *
 * Revision 3.8.18.2  1996/04/15  21:15:37  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.8.18.1  1996/03/18  20:15:57  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.6.2  1996/03/07  09:45:33  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.6.1  1996/02/20  00:58:45  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/08  20:11:36  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.7  1996/01/25  11:19:14  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.6  1996/01/24  20:14:47  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.5  1996/01/11  02:26:25  jenny
 * CSCdi46513:  TCP/IP need code clean up (part 2)
 * . removed ip_tuba.c and clns_tuba.c.
 * . removed lots of tuba related registry stubs and code.
 *
 * Revision 3.4  1995/12/17  18:29:24  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/22  17:46:59  albright
 * CSCdi43791:  Helper addresses dont work over unnumbered interfaces
 *
 * Revision 3.2  1995/11/17  09:33:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:55:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/10/20  18:05:47  albright
 * CSCdi42154:  ip helper-address fails on unnumbered interfaces
 *
 * Revision 2.5  1995/09/25  08:31:49  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.4  1995/09/11  23:49:02  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.3  1995/06/30 05:21:06  hampton
 * Move externs for the random number generator to their own file.  Remove
 * redundant initialization of the random number generator.  [CSCdi36588]
 *
 * Revision 2.2  1995/06/09 13:05:21  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:56:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <ciscolib.h>
#include "interface_private.h"
#include "sys_registry.h"
#include "packet.h"
#include "ip.h"
#include "ttysrv.h"
#include "config.h"
#include "name.h"
#include "../ip/ip_registry.h"
#include "../ui/command.h"
#include "ipoptions.h"
#include "../os/buffers.h"
#include "../snmp/sr_pingmib.h"
#include "../snmp/snmp_registry.h"
#include "../parser/parser_actions.h"
#include "../util/random.h"
#include "../ui/common_strings.h"

/*
 * Definitions
 */
#define	MAXLEN	  80			/* command buffer length */
#define PING_PATTERN 0xABCD		/* default data pattern */ 

/* Minimum packet size is a little bit larger than just a standard ICMP
   echo with no data.  We need at least the sizeof(sys_timestamp) data bytes
   so we can store timing information inside the packet.  */

#define MIN_ICMP_PKT	IPHEADERBYTES(NOPT) + sizeof(icmptype) + sizeof(sys_timestamp)
#define MAX_ICMP_PKT	MAXDATAGRAMSIZE

/*
 * ip_ping_parse
 * Parse a ping command line.  Called by SERVICE_PING_PARSE.
 * Return TRUE if positive identification of an IP address.
 */

boolean ip_ping_parse (char *args, int hint, int *index,
                       char **cp, boolean *running)
{
    ulong address;
    
    /*
     * IP ping is allowed only if IP is running in box.
     * No usable IP if we can't come up with a default interface.
     */
    *running = (boolean) pick_defaultidb(0L);
    *cp = "IP";
    *index = LINK_IP;

    if (!(*running))
	return(FALSE);

    if (hint < 0) {
	if (null(args))		/* IP is special case of null matching */
	    return(TRUE);
	return(strmatch(args, "ip", FALSE));
    } else if (hint == LINK_ILLEGAL) {
	if (parse_hostname(args, &address))
	    return(TRUE);
    } else if ((hint == LINK_IP) &&
	       (!args || !*args || parse_hostname(args, &address)))
	return(TRUE);
    return(FALSE);
}

/*
 * printf_ipaddr_and_name
 *
 * Print DNS name as well as IP address. If DNS name not available, just
 * print address.
 */
static void printf_ipaddr_and_name (ipaddrtype addr)
{

    nametype *name;

    if ((name = reg_invoke_ip_address_lookup(addr))) {
	printf("%s (%i)", name->name, addr);
    } else {
	printf("%i", addr);
    }
}

/*
 * ip_print_bcast_ping
 *
 * Print the results of a broadcast ping.
 */
static void ip_print_bcast_ping (echotype *echo, int request_number)
{
    if (echo->deltatime == 0L)
	echo->deltatime = 1L;
    if (echo->how == '!') {
	printf("\nReply");
    } else if (echo->how == 'C') {
	printf("\nCorrupted reply (pattern %-0#6x, offset %d, %d corruptions)",
	       echo->corruption, echo->corruption_offset,
	       echo->corrupted_words);
    } else {
	printf("\nUnknown reply '%c'", echo->how);
    }
    printf(" to request %d from ", request_number);
    printf_ipaddr_and_name(echo->who);
    printf(", %d ms", echo->deltatime);
}

/*
 * ip_ping
 * The ping command for sending ICMP Echo messages
 */

void ip_ping (char *args)
{
    long size, repeat, seconds;
    ulong i, hits;
    boolean verbose;
    ulong tmin = 999999999, tmax = 0, tsum = 0;
    echotype data, *dp, *dp_child;
    char buff[MAXLEN];
    int tos = TOS_PREC_ROUTINE;
    int incaddr;
    ipoptiontype ipoption;
    ipoptiontype *options;
    ushort pattern;
    int throw_away;
    ulong sweepmin, sweepmax, sweepint, sweeprepeat;
    boolean sweep;
    ipaddrtype source, ipaddr, requested_addr;
    boolean nofrag, broadcast, source_specified, validate, rotate_pattern;
    idbtype *idb, *broad_idb;
    ushort ttl;

    /*
     * Set default parameters.
     */
    size = 100;				/* default datagram size */
    repeat = 5;				/* default repeat count */
    seconds = 2;			/* delay up to two seconds */
    verbose = FALSE;			/* not verbose */
    validate = FALSE;			/* Don't validate */
    rotate_pattern = FALSE;		/* No rotating pattern */
    ipoption.paddedlength = 0;		/* no options */
    ipoption.length = 0;
    options = NULL;
    pattern = PING_PATTERN;		/* default pattern */
    nofrag = FALSE;
    sweep = FALSE;			/* not sweeping by default */
    sweepmin = MIN_ICMP_PKT;
    sweepmax = MAX_ICMP_PKT;
    sweepint = 1;			/* use each size */
    sweeprepeat = 1;
    broadcast = FALSE;
    source_specified = TRUE;
    broad_idb = NULL;
    incaddr = 0;
    ttl = TTL_DEFAULT;			/* default time-to-live is 255 */

    /*
     * If we were passed an IP address, don't go through dialog, use defaults.
     */
    source = 0;
    if (!parse_hostname(args, &ipaddr)) {
	printf("Target IP address: ");
	if (!rdtty(buff, MAXLEN) || !parse_hostname(buff, &ipaddr)) {
	    printf("\n%% Bad IP address");
	    return;
	}

	/*
	 * Send a single packet to multicast groups.
	 */
	if (IPMULTICAST(ipaddr)) repeat = 1;

	/*
         * Ask for repeat count, size, and timeout interval
	 */
	if (!ping_common(&repeat, &size, sweepmin, sweepmax, &seconds))
	    return;
	/* 
	 * Get extended commands from user
	 */
	get_ping_options(buff,&pattern,&options,&ipoption,&tos,&verbose,
			 &nofrag,&source,ipaddr,&incaddr,&broad_idb,&validate,
			 &rotate_pattern, &ttl);
	/*
	 * Recompute minimum packet size now that options have been selected
	 */
	sweepmin += ipoption.paddedlength;
	if (size < sweepmin) {
	    /*
	     * User selected packet size which does not leave enough room
	     * for IP options.
	     */
	    printf("\nSelected packet size of %d too small for packet"
		   " headers.  %d bytes required.", size, sweepmin);
	    return;
	}
	/*
	 * "sweep" mode?
	 */
	printf("Sweep range of sizes [n]: ");
	if (rdtty(buff, MAXLEN) && toupper(*buff) == 'Y') {
	    sweep = TRUE;
	    if (!ping_get_sweep(&sweepmin,&sweepmax,&sweepint))
		return;
	    size = sweepmin;
	    sweeprepeat = ((sweepmax - sweepmin) / sweepint) + 1;
	}
    } else {

	/*
	 * Send a single packet to multicast groups.
	 */
	if (IPMULTICAST(ipaddr)) repeat = 1;
    }

    requested_addr = ipaddr;		/* Save to print header later */
    if (IPMULTICAST(ipaddr) || ipaddr == -1L)
	broadcast = TRUE;
    else 
	FOR_ALL_SWIDBS(idb)
	    if (ip_broadcast(ipaddr, idb)) {
		broadcast = TRUE;
		broad_idb = idb;
		ipaddr = -1L;
		break;
	    }

    if (broadcast && (!(CUR_PRIV(stdio->privilege) > PRIV_USER))) {
	printf("IP broadcast ping disallowed from EXEC user level");
	return;
    }

    /*
     * Go for it.
     */
    if (source == 0 && !broadcast) {
	source_specified = FALSE;
	source = ip_best_local_address(ipaddr, FALSE);
    }

    psion(stdio);
    data.sequence = random_gen();
    data.broadcast = broadcast;
    data.interactive = TRUE;
    data.pattern = pattern;
    data.rotating_pattern = rotate_pattern;
    data.validate = validate;
    data.ttl = ttl;
    printf(type_escape_seq_abort_msg);
    if (sweep == TRUE)
      printf("\nSending %d, [%d..%d]-byte ICMP Echos to %i,"
	     " timeout is %d seconds:", repeat*sweeprepeat, sweepmin,
	     sweepmax, requested_addr, seconds);
    else
      printf("\nSending %d, %d-byte ICMP Echos to %i, timeout is %d seconds:",
	     repeat, size, requested_addr, seconds);
    if (rotate_pattern) {
	printf("\nPacket has rotating data pattern starting with %-0#6x",
	       data.sequence);
    } else {
    if (pattern != PING_PATTERN)
	    printf("\nPacket has data pattern %-0#6x", pattern);
    }
    if (ipoption.paddedlength != 0 ) {
	printf("\nPacket has IP options:  ");
	print_ipoptions(&ipoption);
    }
    flush();
    for (i = 0, hits = 0; (i < repeat*sweeprepeat) &&
	 (psipending() == FALSE); i++) {
	if ((i % 70) == 0 && !verbose) {
	    printc('\n');
	    flush();
	}
	data.how = ' ';
	data.child = NULL;
	data.id = i;
	TIMER_START(data.timeout, seconds * ONESEC);
	data.tos = tos;
	if (options != NULL)			/* copy fresh options */
	    bcopy(&ipoption, options, sizeof(ipoptiontype));
	data.options = options;
	if (send_echo(ipaddr, &data, size, pattern, source, nofrag,
		      broad_idb)) {
	    switch(data.how) {
	    case '!':		/* good reply */
		hits++;
		/*
		 * If we got a reply before the clocked ticked, turn a zero
		 * into a one to avoid dumb questions from customers.
		 */
		if (data.deltatime == 0L)
		    data.deltatime = 1L;
		tsum += data.deltatime;
		if (data.deltatime < tmin)
		    tmin = data.deltatime;
		if (data.deltatime > tmax)
		    tmax = data.deltatime;
		if (verbose)
		    printf("\nReply to request %d (%d ms)", i, data.deltatime);
		break;
	    case 'Q':		/* source quenched */
		if (verbose)
		    printf("\nSource quench from %i", data.who);
		break;
	    case 'U':		/* unreachable */
		if (verbose)
		    printf("\nUnreachable from %i", data.who);
		break;
	    case 'M':		/* unreachable type code 4 */
		if (verbose)
		    printf("\nUnreachable from %i, maximum MTU %u",
			   data.who, data.mtu);
		break;
	    case 'C':		/* corrupted data */
		if (verbose)
		    printf("\nCorrupted reply to request %d (%d ms), "
			   "pattern = %-0#6x, offset %d, %d words corrupted",
			   i, data.deltatime, data.corruption,
			   data.corruption_offset, data.corrupted_words);
		break;
	    default:
		if (verbose)
		    printf("\nUnknown echo response type %#x", data.how);
		else
		    data.how = '?';
		break;
	    }
	    if (verbose) {
		if (sweep)
		    printf(" (size %u)", size);
		if (data.options && data.options->paddedlength) {
		    printf(".  Received packet has options\n ");
		    print_ipoptions(data.options);
		} else {
		    if (ipoption.paddedlength != 0 )
			printf(".  Received packet has no options");
		}
	    } else
		printc(data.how);
        } else {

	    /*
	     * If we were doing a broadcast ping, we deliberately run
	     * at slow rate and time out, so let's see if that was the
	     * case.
	     */
	    if (data.broadcast && (data.how == '!' || data.how == 'C')) {
		ip_print_bcast_ping(&data, i);

		for (dp = data.child; dp; dp = dp->child) {
		    ip_print_bcast_ping(dp, i);
		}
	    } else {	/* nope, it was a legit failure */
		if (!verbose)
		    printc('.');
		else {
		    printf("\nRequest %d timed out", i);
		    if (sweep)
			printf(" (size %u)", size);
		}
	    }
	    (void)process_may_suspend();
	}
	flush();

	/*
	 * If we had any hanging buffers, clean them up now.
	 */

	for (dp = data.child; dp; dp = dp_child) {
	    dp_child = dp->child;
	    dp->child = NULL;
	    free(dp);
	}

	/*
	 * The following getc() is an attempt to get PSIPENDING flag
	 * set if the user types an escape when using a vty.
	 */
	do
	    throw_away = getc(stdio);
	while (throw_away != -1);

	if (sweep == TRUE) {
	    size += sweepint;
	    if (size > sweepmax)
		size = sweepmin;
	}
	/*
	 * If ping failed and the source was picked by us then
	 * try picking source again as the source might have gone
	 * down.
	 */
	if (!source_specified && data.how != '!')
	    source = ip_best_local_address(ipaddr, FALSE);

	ipaddr += incaddr;
    }
    if (i > 0 && !broadcast) {
	printf("\nSuccess rate is %d percent (%u/%u)",
	    hits > MAXULONG/100 ? hits / (i/100) : (hits*100) / i, hits, i);
	if (hits) {
	    /*
	     * If we got a reply before the clocked ticked, turn a zero
	     * into a one to avoid dumb questions from customers.
	     */
	    if (tmin == 0)
		tmin = 1;
	    printf(", round-trip min/avg/max = %u/%u/%u ms",
		tmin, tsum/hits, tmax);
	}
    }
    psioff(stdio);
    free(options);
}

/*
 * snmp_ip_echo_validate
 * Validate a ICMP echo for SNMP
 * Called via the snmp_ping_validate registry
 */

static int snmp_ip_echo_validate (ciscoPingEntry_t *entry)
{
    /*
     * there is a real problem if the protocol isn't set, or if it
     * isn't set to the same value as the case registry switch value
     */
    if (!VALID(I_ciscoPingProtocol, entry->valid) ||
	(entry->ciscoPingProtocol != D_ciscoPingProtocol_ip))
	return (SNMP_PING_INVALID_PROTOCOL);

    /*
     * if the address is set, it better be 4 octets in length, and we don't
     * care about the value.  If the address isn't set, we simply continue
     * (the activate routine won't be called by snmp until an address has
     * been set and validated)
     */
    if (VALID(I_ciscoPingAddress, entry->valid) &&
	(entry->ciscoPingAddress->length != ADDRLEN_IP))
	return (SNMP_PING_INVALID_ADDRESS);

    /*
     * if the packet size is set, it better be within the legal range.
     * if the packet size is not set, we simply continue (the activation
     * routine won't be called by snmp until a packet size has been set
     * and validated)
     */
    if (VALID(I_ciscoPingPacketSize, entry->valid) &&
	((entry->ciscoPingPacketSize < MIN_ICMP_PKT) ||
	 (entry->ciscoPingPacketSize > MAX_ICMP_PKT)))
	return (SNMP_PING_INVALID_SIZE);

    /*
     * all the validation criteria have passed
     */
    return (SNMP_PING_VALID);
}

/*
 * snmp_ip_echo_activate
 * Send a ICMP echo for SNMP
 * Called via the snmp_ping_activate registry
 */

static void snmp_ip_echo_activate (ciscoPingEntry_t *entry)
{
    ipaddrtype	source;
    ipaddrtype	address;
    echotype	data;
    ulong	total_rtt;
    ulong	idx;

    data.sequence = 0;
    data.tos = 0;
    data.options = NULL;
    data.interactive = FALSE;
    data.broadcast = FALSE;
    data.validate = FALSE;
    data.rotating_pattern = FALSE;
    data.ttl = TTL_DEFAULT;
    address = reg_invoke_OctetStringToIP(entry->ciscoPingAddress);
    source = ip_best_local_address(address, FALSE);
    entry->ciscoPingSentPackets = 0;
    SET_VALID(I_ciscoPingSentPackets, entry->valid);
    entry->ciscoPingReceivedPackets = 0;
    SET_VALID(I_ciscoPingReceivedPackets, entry->valid);
    entry->ciscoPingCompleted = D_ciscoPingCompleted_false;
    SET_VALID(I_ciscoPingCompleted, entry->valid);
    total_rtt = 0;
    for (idx = 1; idx <= entry->ciscoPingPacketCount; idx++) {
	entry->ciscoPingSentPackets++;
	data.id = idx;
	TIMER_START(data.timeout, entry->ciscoPingPacketTimeout);
	if (send_echo(address, &data, entry->ciscoPingPacketSize,
		      PING_PATTERN, source, FALSE, NULL)) {
	    if (data.how == '!') {

		entry->ciscoPingReceivedPackets++;

		/*
		 * If we got a reply before the clocked ticked, turn a zero
		 * into a one to avoid dumb questions from customers.
		 */
		if (data.deltatime == 0L)
		    data.deltatime = 1L;
		total_rtt += data.deltatime;
		if ((!VALID(I_ciscoPingMinRtt, entry->valid)) ||
		    (data.deltatime < entry->ciscoPingMinRtt)) {
		    entry->ciscoPingMinRtt = data.deltatime;
		    SET_VALID(I_ciscoPingMinRtt, entry->valid);
		}

		if ((!VALID(I_ciscoPingMaxRtt, entry->valid)) ||
		    (data.deltatime > entry->ciscoPingMaxRtt)) {
		    entry->ciscoPingMaxRtt = data.deltatime;
		    SET_VALID(I_ciscoPingMaxRtt, entry->valid);
		}
		entry->ciscoPingAvgRtt =
		    total_rtt / entry->ciscoPingReceivedPackets;
		SET_VALID(I_ciscoPingAvgRtt, entry->valid);

	    }
	}
	/*
	 * sleep if there are more packets to send
	 */
	if (idx < entry->ciscoPingPacketCount) {
	    process_sleep_for(entry->ciscoPingDelay);
	}
	
    }

    entry->ciscoPingCompleted = D_ciscoPingCompleted_true;
}

void register_snmp_ip_echo(void)
{
    if (registry_populated(REG_SNMP)) {
	reg_add_snmp_ping_validate(D_ciscoPingProtocol_ip,
				   snmp_ip_echo_validate,
				   "snmp_ip_echo_validate");
	
	reg_add_snmp_ping_activate(D_ciscoPingProtocol_ip,
				   snmp_ip_echo_activate,
				   "snmp_ip_echo_activate");
    }
}
