/* $Id: xnsecho.c,v 3.4.18.2 1996/05/11 16:52:17 thille Exp $
 * $Source: /release/112/cvs/Xsys/xns/xnsecho.c,v $
 *------------------------------------------------------------------
 * xnsecho.c -- Perform XNS echo functions
 *
 * January 1988, Greg Satz
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xnsecho.c,v $
 * Revision 3.4.18.2  1996/05/11  16:52:17  thille
 * CSCdi51398:  Duplicate strings in ping / trace
 * Branch: California_branch
 * Every 116 bytes counts.
 *
 * Revision 3.4.18.1  1996/03/18  22:54:21  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.12.2  1996/03/07  11:22:55  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.12.1  1996/02/20  22:01:58  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/08  20:15:59  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.3  1995/12/22  21:59:03  hampton
 * Pass pointers to timestamps instead of passing the complete timestamp
 * structure.  [CSCdi45966]
 *
 * Revision 3.2  1995/11/17  19:25:39  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:00:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/11  23:59:00  hampton
 * Convert XNS and Apollo to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi39966]
 *
 * Revision 2.2  1995/06/09 13:26:24  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:30:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "ttysrv.h"
#include "../ui/debug.h"
#include "../if/ether.h"
#include "config.h"
#include "xns.h"
#include "xns_debug.h"
#include "../if/network.h"
#include "../ui/command.h"
#include "../util/random.h"
#include "../ui/common_strings.h"

#define MAXLEN	80

static const char XNS_str[] = "XNS";

/*
 * xnsechoBLOCK
 * Wait for an Echo Reply or timeout.
 */
boolean xnsechoBLOCK (register xnsechotype *data)
{
    if (!data->active)			  /* if block flag reset, wakeup */
	return(FALSE);
    if (psipending2(data->tty))		  /* interrupted? */
	return(FALSE);
    if (SLEEPING(data->timeout))	  /* keep blocking if timer ticking */
	return(TRUE);
    return(FALSE);
}

/*
 * xns_ping_parse
 * Parse a ping command line.  Called by SERVICE_PING_PARSE.
 * Return TRUE if positive identification of an XNS address.
 */

boolean xns_ping_parse (char *args, int hint, int *index,
			char **cp, boolean *running)
{
    ulong address;
    char tmpbuff[MAXLEN];

    *running = xns_running;
    *cp = "XNS";
    *index = LINK_XNS;

    if (!xns_running)
	return(FALSE);

    if (hint < 0)
	return(strmatch(args, "xns", FALSE));
    else if (hint == LINK_ILLEGAL) {
	if (parse_xnsaddress(args, &address, tmpbuff))
	    return(TRUE);
    } else if ((hint == LINK_XNS) &&
	       (!args || !*args || parse_xnsaddress(args, &address, tmpbuff)))
	return(TRUE);
    return(FALSE);
}

/*
 * xns_ping
 * The echo command for XNS
 */

void xns_ping (char *args)
{
    long size, repeat, seconds, hits;
    register long i;
    ulong net;
    uchar host[IEEEBYTES];
    boolean verbose;
    long tmin = 999999999, tmax = 0, tsum = 0;
    char buff[MAXLEN];
    xnsechotype data;

    size = 100;				/* default datagram size */
    repeat = 5;				/* default repeat count */
    net = 0L;				/* default net not set */
    seconds = 2;			/* default timeout */
    verbose = FALSE;			/* not verbose */

    /*
     * Get XNS address
     */
    if (!parse_xnsaddress(args, &net, host)) {
	printf("Target XNS address: ");
	if (!rdtty(buff, MAXLEN) || !parse_xnsaddress(buff, &net, host)) {
	    printf("\n%% Bad XNS address");
	    return;
	}
	
	/*
	 * Ask for repeat count, size, and timeout interval
	 */
	if (!ping_common(&repeat, &size, XNS_MINECHOSIZE, XNS_MAXSIZE,
			 &seconds))
	    return;
	
	/*
	 * Ask for verbosity
	 */
	printf("\nVerbose [n]: ");
	if (rdtty(buff, MAXLEN) && toupper(*buff) == 'Y') 
	    verbose = TRUE;
    }

    /*
     * Go for it.
     */
    psion(stdio);
    printf(type_escape_seq_abort_msg);
    printf(ping_sending_msg, repeat, size, XNS_str, common_str_empty_str);
    printf("%u.%e", net, host);
    printf(ping_timeout_msg, seconds);
    flush();
    data.socket = random_gen();
    for (i = 0, hits = 0; (i < repeat) && (psipending() == FALSE); i++) {
	if ((i % 70) == 0 && !verbose) {
	    printc('\n');
	    flush();
	}
	TIMER_START(data.timeout, seconds * ONESEC);
	if (send_xnsecho(net, host, &data, size)) {
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
		    printf("\n%d in %d ms",i,data.deltatime);
		break;
	    case 'B':
		if (verbose)
		    printf("\nbad echo received");
		break;
	    case 'C':
		if (verbose)
		    printf("\nbad checksum received");
		break;
	    default:
		if (verbose)
		    printf("\nunknown packet type %#x", data.how);
		else
		    data.how = '?';
		break;
	    }
	    if (!verbose)
		printc(data.how);
        } else {
	    boolean timed_out;

	    timed_out = AWAKE(data.timeout);
	    if (!verbose) {
		printc('.');
	    } else {
	      printf("\n%d %s", i,
		     timed_out ? "timed out" : "send failed");
	    }
	    if (!timed_out) {
	      process_sleep_until(&data.timeout);
	    }
	    (void)process_may_suspend();
	}
	flush();
	/*
	 *
	 * We will only see PSIPENDING if we do this
	 */
	while (getc(stdio) != -1) {
	  /* Nothing */
	}
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
    psioff(stdio);
}

/*
 * send_xnsecho
 * Send a XNS echo to a host and block for a reply.
 * Returns TRUE if echo reply received. False otherwise.
 */

boolean send_xnsecho (
    long net,
    uchar *host,
    xnsechotype *data,
    int bytes)
{
    paktype	*pak;
    xnshdrtype	*xns;
    xns_cisco_ping *cp;
    xns_ripentrytype *entry;
    idbtype	*idb;
    xnssb	*xsb;
    sys_timestamp now;

    pak = getbuffer(bytes);
    if (!pak)
	return(FALSE);
    xns = (xnshdrtype *)xnsheadstart(pak);
    memset(xnsheadstart(pak), 0, bytes);
    xns->len = bytes;
    xns->tc = 0;
    xns->pt = XNS_TYPE_ECHO;
    pak->flags |= PAK_HEARSELF;
    PUT2WD(xns->ddnet, net);;
    ieee_copy(host, xns->dhost);
    xns->dsock = XNS_SOCK_ECHO;
    entry = (*xns_table)[nethash(net)];
    if (grip_first_hop(net, &(xns->dhost[0]),
		       &(pak->if_output), entry) == NULL) {
	retbuffer(pak);
	buginf("\nXNSECHO: Can't get first hop");
	return(FALSE);
    }
    xns->ssock = data->socket;
    cp = (xns_cisco_ping *)XNSDATASTART(pak);
    PUTSHORT(&cp->type, XNS_ECHO_REQUEST);
    GET_TIMESTAMP(now);
    PAK_PUT_TS(&cp->time_sent, now);
    /* If all nets test do it */
    if (net == -1L) 
	return(xns_allnets_test(pak));
    idb = pak->if_output;
    xsb = idb_get_swsb_inline(idb, SWIDB_SB_XNS);
    PUT2WD(xns->ssnet, xsb->xnsnet);
    ieee_copy(xsb->xns_node, xns->shost);
    xns_crc(pak);
    if (!xns_send(pak)) {
	if (xnspkt_debug) {
	    buginf("\nXNSECHO: Can't send echo");
	}
    } else {
	xns_stat[XNS_ECHOREQOUT]++;
    }
    data->next = NULL;
    data->active = TRUE;
    data->tty = stdio;
    p_enqueue(&xnsechoQ, data);
    edisms((blockproc *)xnsechoBLOCK, (long)data);
    if (data->active)
	p_unqueue(&xnsechoQ, data);
    return (data->active ? FALSE : TRUE);
}

/*
 * parse_xnsaddress
 * Parse and return an XNS address 
 */

boolean
parse_xnsaddress (char *buf, ulong *net, uchar *host)
{
    char netstring[8*sizeof(ulong) + 10];
    int netindx;
    boolean parsed;

    buf = deblank(buf);
    if (buf[0] == '-' && buf[1] == '1' && buf[2] == '.') {
	*net = -1L;
	buf = &buf[2];
    } else {
	for (netindx = 0;
	     (netindx < sizeof(netstring) - 2) && *buf && (*buf != '.');
	     netindx++)
	    netstring[netindx] = *buf++;
	netstring[netindx++] = '\0';
	*net = parse_unsigned(netstring, &parsed);
    }
    if (*buf++ != '.')
	return(FALSE);
    if (!parse_ether_address(buf, host))
	return(FALSE);
    return(TRUE);
}

/* If the destination net is -1 then this is a test of your
 * all nets broadcast facility (this is only a test).
 */

boolean
xns_allnets_test (paktype *pak)
{
    xnshdrtype	*xns, *newpak_xns;
    idbtype	*idb;
    paktype	*newpak;
    xnssb	*xsb;
    list_element *l_elt;

    if (!xnspkt_debug) {
	buginf("\nXNSECHO: Can't send echo");
	return (FALSE);
    }
    xns = (xnshdrtype *)xnsheadstart(pak);

    xns->pt = ALLNETS_TEST;
    xns->dsock = ALLNETS_TEST;
    pak->flags |= PAK_BROADCAST;

    FOR_ALL_SWIDBS_IN_LIST(SWIDBLIST_XNS, l_elt, idb) {
	xsb = idb_get_swsb(idb, SWIDB_SB_XNS);
	if (xsb && xsb->xnsnet && interface_up(idb)) {
	    newpak = pak_duplicate(pak);
	    if (newpak == NULL)
		return (FALSE);
	    newpak_xns = (xnshdrtype *)xnsheadstart(newpak);
	    PUT2WD(newpak_xns->ssnet, xsb->xnsnet);
	    ieee_copy(xsb->xns_node, newpak_xns->shost);
	    newpak->if_output = idb;
	    xns_crc(newpak);
	    if (xns_send(newpak))
		xns_stat[XNS_ECHOREQOUT]++;
	}
    }
    retbuffer(pak);
    return (TRUE);
}
