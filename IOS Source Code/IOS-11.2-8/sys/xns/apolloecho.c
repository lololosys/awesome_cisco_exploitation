/* $Id: apolloecho.c,v 3.5.18.2 1996/05/11 16:52:15 thille Exp $
 * $Source: /release/112/cvs/Xsys/xns/apolloecho.c,v $
 *------------------------------------------------------------------
 * apolloecho.c - Add ping for apollo.
 *
 * July 1991, Vicki Ralls
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: apolloecho.c,v $
 * Revision 3.5.18.2  1996/05/11  16:52:15  thille
 * CSCdi51398:  Duplicate strings in ping / trace
 * Branch: California_branch
 * Every 116 bytes counts.
 *
 * Revision 3.5.18.1  1996/03/18  22:52:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.2  1996/03/07  11:21:27  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.12.1  1996/02/20  21:58:26  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/08  20:15:42  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.4  1995/12/22  21:59:01  hampton
 * Pass pointers to timestamps instead of passing the complete timestamp
 * structure.  [CSCdi45966]
 *
 * Revision 3.3  1995/11/17  19:20:46  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:53:09  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:57:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/11  23:58:25  hampton
 * Convert XNS and Apollo to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi39966]
 *
 * Revision 2.2  1995/06/09 13:25:42  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:26:56  hampton
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
#include "ieee.h"
#include "config.h"
#include "xns.h"
#include "apollo.h"
#include "../if/network.h"
#include "../ui/command.h"
#include "../ip/ip.h"
#include "../ui/common_strings.h"

#define MAXLEN	80

extern ulong parse_unsigned(char *, boolean *);

static const char APOLLO_str[] = "APOLLO";


/*
 * apolloechoBLOCK
 * Wait for an Echo Reply or timeout.
 */

static boolean apolloechoBLOCK (register apolloechotype *data)
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
 * apollo_ping_parse
 * Parse a ping command line.  Called by SERVICE_PING_PARSE.
 * Return TRUE if positive identification of an Apollo address.
 */

boolean apollo_ping_parse (char *args, int hint, int *index,
			   char **cp, boolean *running)
{
    char tmpbuff[MAXLEN];
    ulong address;

    *running = apollo_running;
    *cp = "Apollo";
    *index = LINK_APOLLO;

    if (!apollo_running)
	return(FALSE);

    if (hint < 0)
	return(strmatch(args, "apollo", FALSE));
    else if (hint == LINK_ILLEGAL) {
	if (parse_apolloaddress(args, &address, tmpbuff))
	    return(TRUE);
    } else if ((hint == LINK_APOLLO) &&
	       (!args || !*args || parse_apolloaddress(args, &address,
						       tmpbuff)))
	return(TRUE);
    return(FALSE);
}

/*
 * apollo_ping
 * The echo command for APOLLO
 */

void apollo_ping (char *args)
{
    long size, repeat, seconds, hits;
    register long i;
    ulong net;
    uchar host[IEEEBYTES];
    boolean verbose;
    long tmin = 999999999, tmax = 0, tsum = 0;
    char buff[MAXLEN];
    apolloechotype data;

    size = 100;				/* default datagram size */
    repeat = 5;				/* default repeat count */
    net = 0L;				/* default net not set */
    seconds = 2;			/* default timeout */
    verbose = FALSE;			/* not verbose */

    /*
     * Get APOLLO address
     */
    if (!parse_apolloaddress(args, &net, host)) {
	printf("Target APOLLO address: ");
	if (!rdtty(buff, MAXLEN) || !parse_apolloaddress(buff, &net, host)) {
	    printf("\n%% Bad APOLLO address");
	    return;
	}
	
	/*
	 * Ask for repeat count, size, and timeout interval
	 */
	if (!ping_common(&repeat, &size, APOLLO_MINECHOSIZE, APOLLO_MAXSIZE,
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
    printf(ping_sending_msg, repeat, size, APOLLO_str, common_str_empty_str);
    printf("%q.%b", APOLLO_TYPE, net, APOLLO_TYPE, host);
    printf(ping_timeout_msg, seconds);
    flush();
    for (i = 0, hits = 0; (i < repeat) && (psipending() == FALSE); i++) {
	if ((i % 70) == 0 && !verbose) {
	    printc('\n');
	    flush();
	}
	TIMER_START(data.timeout, seconds * ONESEC);
	if (send_apolloecho(net, host, &data, size)) {
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
 * send_apolloecho
 * Send a APOLLO echo to a host and block for a reply.
 * Returns TRUE if echo reply received. False otherwise.
 */

boolean send_apolloecho (
    ulong net,
    uchar *host,
    apolloechotype *data,
    int bytes)
{
    paktype	*pak;
    apollohdrtype *apollo;
    apollo_cisco_ping *cp;
    xns_ripentrytype *entry;
    apollosb	*asb;
    sys_timestamp now;

    pak = getbuffer(APOLLOOVERHEADBYTES + bytes);
    if (!pak)
	return(FALSE);

    apollo = (apollohdrtype *)apolloheadstart(pak);

    memset(apolloheadstart(pak), 0, APOLLOOVERHEADBYTES + bytes);
    apollo->len = bytes;
    apollo->pkttype = SETECHO_REQUEST(apollo->pkttype);
    apollo->apollolink = SETECHO_REQUEST(apollo->apollolink);
    apollo->srcnode = apollo_address;
    apollo->version = 2;
    apollo->transid = apollo_transid++;
    apollo->routelen = 40;
    apollo->curpos = 4;
    apollo->tc = 0;
    apollo->pt = SETECHO_REQUEST(apollo->pt);
    PUTLONG(&apollo->dnet, net);
    ieee_copy(host, apollo->dhost);
    apollo->dsock = apollo->ssock = APOLLO_SOCK_ECHO;
    entry = (*apollo_table)[nethash(GETLONG(&apollo->dnet))];
    
    if (grip_first_hop(GETLONG(&apollo->dnet), &(apollo->dhost[0]),
		       &(pak->if_output), entry) == NULL) {
	if (apollo_debug)
	    buginf("\nApollo: src=%q.%b, dst=%q.%b, no route",
		  APOLLO_TYPE, GETLONG(&apollo->snet), APOLLO_TYPE, 
		   apollo->shost, APOLLO_TYPE, GETLONG(&apollo->dnet),
		   APOLLO_TYPE, apollo->dhost);
	apollo_dispose(APOLLO_NOROUTE, pak);
	return(FALSE);
    }
    cp = (apollo_cisco_ping *)APOLLODATASTART(pak);
    PUTSHORT(&cp->type, APOLLO_ECHO_REQUEST);
    GET_TIMESTAMP(now);
    PAK_PUT_TS(&cp->time_sent, now);
    asb = idb_get_swsb_inline(pak->if_output, SWIDB_SB_APOLLO);
    if (asb == NULL) {
	datagram_done(pak);
	return (FALSE);
    }
    PUTLONG(&apollo->snet, asb->apollonet);
    apollo->cksum = 0xFFFF;
    apollo->hdrdatalen = 2 + bytes;
    apollo->hdrlen = apollo->hdrdatalen + XNSOVERHEADBYTES +
	APOLLOOVERHEADBYTES;
    apollo->datalen = 0;
    apollo->len = XNSOVERHEADBYTES + apollo->hdrdatalen;
    pak->linktype = LINK_APOLLO;
    ieee_copy(apollo_etheraddr, apollo->shost);
    if (!apollo_send(pak)) {
	if (apollo_debug)
	    buginf("\nAPOLLO: Can't send echo");
	return(FALSE);
    } else
	apollo_stat[APOLLO_ECHOREQOUT]++;
    data->socket = APOLLO_SOCK_ECHO;
    data->next = NULL;
    data->active = TRUE;
    data->tty = stdio;
    p_enqueue(&apolloechoQ, data);
    edisms((blockproc *)apolloechoBLOCK, (long)data);
    if (data->active)
	p_unqueue(&apolloechoQ, data);
    return (data->active ? FALSE : TRUE);
}
