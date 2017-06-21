/* $Id: novecho.c,v 3.5.18.4 1996/07/26 00:08:39 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/novecho.c,v $
 *------------------------------------------------------------------
 * novecho.c -- Perform XNS echo functions
 *
 * January 1990, Greg Satz, Vicki Ralls
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: novecho.c,v $
 * Revision 3.5.18.4  1996/07/26  00:08:39  hampton
 * Various minor IPX cleanups.  [CSCdi64087]
 * Branch: California_branch
 *
 * Revision 3.5.18.3  1996/05/13  17:54:49  john
 * CSCdi35609:  IPX ping to local low end ethernet does not work
 *              make IPX ping to local interface do what IP does.
 * Branch: California_branch
 *
 * Revision 3.5.18.2  1996/05/11  16:52:16  thille
 * CSCdi51398:  Duplicate strings in ping / trace
 * Branch: California_branch
 * Every 116 bytes counts.
 *
 * Revision 3.5.18.1  1996/03/18  22:52:55  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.18.2  1996/03/07  11:21:48  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.18.1  1996/02/20  22:00:04  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/08  20:15:45  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.4  1995/12/01  06:25:04  mschaef
 * CSCdi37234:  Allow fastswitching of certain directed broadcast packets
 *
 * Revision 3.3  1995/11/21  23:42:57  john
 * CSCdi44009:  Test ipx echo command doesnt use ipx internal network
 *              address
 *
 * Revision 3.2  1995/11/17  19:22:27  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:58:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:38:52  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/09/11  23:58:31  hampton
 * Convert XNS and Apollo to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi39966]
 *
 * Revision 2.4  1995/08/25 14:11:26  hampton
 * Minor Novell IPX Cleanups.  Remove some unused variables and routines.
 * Eliminate direct references to the system clock.  [CSCdi39328]
 *
 * Revision 2.3  1995/07/01 03:38:36  hampton
 * Minor Novell IPX Cleanups.  Remove some unused variables and routines.
 * Eliminate direct references to the system clock.  [CSCdi36668]
 *
 * Revision 2.2  1995/06/09 13:25:48  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:28:00  hampton
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
#include "sys_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "ttysrv.h"
#include "config.h"
#include "../if/ether.h"
#include "../if/network.h"
#include "../ui/command.h"
#include "xns.h"
#include "novell.h"
#include "../snmp/sr_pingmib.h"
#include "../snmp/snmp_registry.h"
#include "../util/random.h"
#include "../ui/common_strings.h"

static int snmp_ipx_echo_validate (ciscoPingEntry_t *entry);
static void snmp_ipx_echo_activate (ciscoPingEntry_t *entry);

static const char IPX_str[] = "IPX";

#define MAXLEN	80


/*
 * novell_ping_parse
 * Parse a ping command line.  Called by SERVICE_PING_PARSE.
 * Return TRUE if positive identification of a Novell address.
 */

boolean novell_ping_parse (char *args, int hint, int *index,
			   char **cp, boolean *running)
{
    ulong address;
    char tmpbuff[MAXLEN];

    *running = novell_running;
    *cp = "IPX";
    *index = LINK_NOVELL;

    if(!novell_running)
	return(FALSE);

    if (hint < 0)
	return(strmatch(args, "ipx", FALSE) || strmatch(args, "novell", FALSE));
    else if (hint == LINK_ILLEGAL) {
	if (parse_novelladdress(args, &address, tmpbuff))
	    return(TRUE);
    } else if ((hint == LINK_NOVELL) &&
	       (!args || !*args || parse_novelladdress(args, &address,
						       tmpbuff)))
	return(TRUE);
    return(FALSE);
}

/*
 * nov_echoBLOCK
 * Wait for an Echo Reply or timeout.
 */

static boolean nov_echoBLOCK (xnsechotype *data)
{
    if (!data->active)			  /* if block flag reset, wakeup */
	return(FALSE);
    if (psipending2((tt_soc *)data->tty)) /* interrupted? */
	return(FALSE);
    if (SLEEPING(data->timeout))	  /* keep blocking if timer ticking */
	return(TRUE);
    return(FALSE);
}

/*
 * nov_ping
 * The echo command for NOVELL
 */

void nov_ping (char *args)
{
    long size, repeat, seconds, hits;
    ulong net, i;
    uchar host[IEEEBYTES];
    boolean verbose;
    long tmin = 999999999, tmax = 0, tsum = 0;
    char buff[MAXLEN];
    xnsechotype data;
    enum IPX_ECHO_TYPE novecho_cisco;   /* cisco echo by default */

    size = 100;				/* default datagram size */
    repeat = 5;				/* default repeat count */
    net = 0L;				/* default net not set */
    seconds = 2;			/* default timeout */
    verbose = FALSE;			/* not verbose */
    novecho_cisco = novell_ping_deflt;  /* default to global setting */

    /*
     * Get Novell address
     */
    if (!parse_novelladdress(args, &net, host)) {
	printf("Target IPX address: ");
	if (!rdtty(buff, MAXLEN) || !parse_novelladdress(buff, &net, host)) {
	    printf("\n%% Bad IPX address");
	    return;
	}

	/*
	 * Ask for repeat count, size, and timeout interval
	 */
	if (!ping_common(&repeat, &size, XNS_MINECHOSIZE, NOVELL_MAXSIZE,
			 &seconds))
	    return;

	/*
	 * Ask for verbosity
	 */
	printf("\nVerbose [n]: ");
	if (rdtty(buff, MAXLEN) && toupper(*buff) == 'Y') 
	    verbose = TRUE;

        /*
         * Novell, Inc. Standard Echo
         */

        printf("%s Echo [n]: ", novell_ping_deflt ?
	       "cisco" : "Novell Standard");
        if (rdtty(buff, MAXLEN) && toupper(*buff) == 'Y') {
	  novecho_cisco = novell_ping_deflt ?
	    IPX_ECHO_TYPE_CISCO : IPX_ECHO_TYPE_NOVELL;
	}
	if ((novecho_cisco == IPX_ECHO_TYPE_NOVELL) &&
	    size < NOVELL_ECHO_STD_SIZE) {
	  printf("\n%% Novell Standard Echoes must be at least %d bytes",
		 NOVELL_ECHO_STD_SIZE);
	  return;
	}
    }
    /*
     * Go for it.
     */
    psion(stdio);
    printf(type_escape_seq_abort_msg);
    printf(ping_sending_msg, repeat, size, IPX_str,
	   novecho_cisco ? " Novell" : " cisco");
    printf("%x.%e", net, host);
    printf(ping_timeout_msg, seconds);
    flush();
    data.echo_test = FALSE;
    for (i = 0, hits = 0; (i < repeat) && (psipending() == FALSE); i++) {
	if ((i % 70) == 0 && !verbose) {
	    printc('\n');
	    flush();
	}
	TIMER_START(data.timeout, seconds * ONESEC);
	if (send_novecho(net, host, &data, size, novecho_cisco)) {
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
		process_sleep_for(TIME_LEFT_SLEEPING(data.timeout));
	    }
	    (void)process_may_suspend();
	}
	flush();

	/*
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
}

/*
 * send_novecho
 * Send a NOV echo to a host and block for a reply.
 * Returns TRUE if echo reply received. FALSE otherwise.
 *
 * echo_type tells us if we are doing cisco or Novell, Inc. echo.
 */

boolean send_novecho (long net, uchar *host, xnsechotype *data, int bytes,
		      enum IPX_ECHO_TYPE echo_type)
{
    paktype *pak;
    ipxhdrtype *ipx;
    novndbtype *entry;
    idbtype *idb;
    nidbtype *nidb;
    ulong flags;
    ipx_stnd_ping *sp;
    ipx_cisco_ping *cp;
    sys_timestamp now;

    pak = getbuffer(bytes);
    if (!pak)
	return(FALSE);

    ipx = (ipxhdrtype *)ipxheadstart(pak);
    memset(ipxheadstart(pak), 0, bytes);
    ipx->len = bytes;
    ipx->tc = 0;
    PUT2WD(ipx->ddnet, net);
    ieee_copy(host, ipx->dhost);
    GET_TIMESTAMP(now);
    if (echo_type == IPX_ECHO_TYPE_CISCO) {
      ipx->pt = NOVELL_ECHO_REQUEST;
      data->socket = ipx->dsock = ipx->ssock = NOVELL_SOCK_ECHO;
      cp = (ipx_cisco_ping *)ipxdatastart(pak);
      PUTSHORT(&cp->type, NOVELL_ECHO_REQUEST);
      PAK_PUT_TS(&cp->time_sent, now);
    } else {
      ipx->pt = NOVELL_TYPE_ECHO_STND;
      data->socket = ipx->dsock = ipx->ssock = NOVELL_SOCK_ECHO_STND;
      sp = (ipx_stnd_ping *)ipxdatastart(pak);
      PUTLONG((uchar *)&sp->signature , NOVELL_ECHO_SIGNATURE);
      sp->version_byte = NOVELL_ECHO_VERSION;
      sp->type_byte = NOVELL_TYPE_ECHO_RCV;
      PUTSHORT(&sp->ping_id, (random_gen() & 0x0000FFFF));
      sp->result_byte = 0;   /* when sending always 0 */
      sp->reserved = 0;      /* when sending always 0 (padding) */
      PAK_PUT_TS(&sp->time_sent, now);
    }
    pak->linktype = LINK_NOVELL;
    ipx->cksum = 0xFFFF;
    if (net == -1L)
	return(nov_allnets_test(pak));
    entry = (*novell_table)[NOV_NETHASH(net)];
    if (novrip_first_hop(net, &(ipx->dhost[0]),
			 &(nidbtype *)nidb, &flags, entry,
			 IPX_ROUND_ROBIN_NO, IPX_DEFAULT_ROUTE_YES) == NULL) {
	retbuffer(pak);
	return(FALSE);
    }
    idb = nidb->idb;
    if (ipx_internal_net != 0) {
      PUT2WD(ipx->ssnet, ipx_internal_net);
      ieee_copy(&ipx_internal_hostid[0], ipx->shost);
    } else {      
      PUT2WD(ipx->ssnet, nidb->novellnet);
      ieee_copy(idb->novell_node, ipx->shost);
    }
    if (!nov_send(pak)) {
	if (novell_debug)
	    buginf("\nIPX: Can't send echo");
	return (FALSE);
    } else
	novell_stat[NOVELL_ECHOREQOUT]++;
    data->next = NULL;
    data->active = TRUE;
    data->tty = stdio;
    p_enqueue(&novechoQ, data);
    edisms((blockproc *)nov_echoBLOCK, (long)data);
    if (data->active)
	p_unqueue(&novechoQ, data);
    return (data->active ? FALSE : TRUE);
}

/*
 * novecho_test_driver
 */

void novecho_test_driver (long innet, uchar *inhost, ulong lower,
			  ulong upper, ulong count, ulong interval,
			  ushort csum, uchar method, ulong seed,
			  ulong tc)
{
    long size, repeat, hits;
    long i, ii;
    ulong net;
    uchar host[IEEEBYTES];
    boolean verbose;
    long tmin = 999999999, tmax = 0, tsum = 0;
    xnsechotype data;
    ulong seq;

    size = lower;			/* datagram size lower limit */
    if (upper < lower)
	upper = lower;
    seq = 0;
    repeat = 5;
    net = 0L;				/* default net not set */
    verbose = FALSE;			/* not verbose */

    /*
     * Get Novell address
     */
    if (innet == 0L)
	return;
    
    net = innet;
    ieee_copy(inhost, host);

    /*
     * Go for it.
     */
    psion(stdio);
    printf(type_escape_seq_abort_msg);
    printf("\nSending %d, %d..%d byte IPX Echo sets to %x.%e, interval is %d mSec:",
		count, lower, upper, net, host, interval);
    flush();

    data.echo_test = TRUE;
    data.method = method;

    for (i = 0, hits = 0, ii = 0;
	 (i < (count*((upper-lower)+1))) && (psipending() == FALSE); i++, ii++) {
	if ((ii % 70) == 0 && !verbose) {
	    printc('\n');
	    flush();
	}

	size = lower + seq++;
	
	TIMER_START(data.timeout, (ulong)interval);
	data.len = size;
	data.csum = csum;
	data.seed = seed;

	if (novecho_test(net, host, &data, size, csum, method, &seed, tc)) {
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
	    case 'b':
		if (verbose)
		    printf("\nbad echo received");
		break;
	    case 'c':
		if (verbose)
		    printf("\nbad checksum received");
		break;
	    case 'e':
		if (verbose)
		    printf("\nsend (encap?) fail");
		break;
	    case '?':
		if (verbose)
		    printf("\nno route");
		break;
	    default:
		if (verbose)
		    printf("\nhow = %#x", data.how);
		break;
	    }
	    if (!verbose)
		printc(data.how);
	    process_sleep_for(interval);
        } else {
	    if (!verbose)
		printc('.');
	    else
		printf("\n%d timed out", i);
	    (void)process_may_suspend();
	}
	flush();
	if (size >= upper) {
	    printc(' ');
	    ii++;
	    seq = 0;
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
 * test_novell_echo
 */

void test_novell_echo (parseinfo *csb)
{
    ulong net;
    char host[IEEEBYTES];

    /* Get the network number and host address. */
    net = GETOBJ(paddr,1)->nov_addr.net;
    ieee_copy(GETOBJ(paddr,1)->nov_addr.host, host);
    novecho_test_driver(net, host, GETOBJ(int,3),
			GETOBJ(int,4), GETOBJ(int,1), GETOBJ(int,2),
			GETOBJ(int,5), GETOBJ(int,6), GETOBJ(int,7),
			GETOBJ(int,8));

}

/*
 * test_novecho
 * Send a NOV echo to a host and block for a reply.
 * Returns TRUE if echo reply received. FALSE otherwise.
 */

boolean novecho_test (long net, uchar *host, xnsechotype *data,
		      int bytes, ushort csum, uchar method,
		      ulong *seed, ulong tc)
{
    paktype *pak;
    ipxhdrtype *ipx;
    ipx_cisco_ping *cp;
    novndbtype *entry;
    idbtype *idb;
    nidbtype *nidb;
    ulong flags;
    int i;
    sys_timestamp now;

    pak = getbuffer(bytes);
    if (!pak)
	return(FALSE);

    ipx = (ipxhdrtype *)ipxheadstart(pak);

    memset(ipxheadstart(pak), 0, bytes);
    ipx->len = bytes;
    ipx->tc = tc;
    ipx->pt = NOVELL_ECHO_REQUEST;
    PUT2WD(ipx->ddnet, net);
    ieee_copy(host, ipx->dhost);
    data->socket = ipx->dsock = ipx->ssock = NOVELL_SOCK_ECHO;
    cp = (ipx_cisco_ping *)ipxdatastart(pak);
    PUTSHORT(&cp->type, NOVELL_ECHO_REQUEST);
    GET_TIMESTAMP(now);
    PAK_PUT_TS(&cp->time_sent, now);
    bytes -= XNS_MINECHOSIZE;
    for (i = 0; i < bytes; i++)
	cp->data[i] = novecho_byte(method, seed);
    pak->linktype = LINK_NOVELL;
    ipx->cksum = csum;
    (*data).how = '?';

    if (net == -1L) {
	(*data).how = 't';
	return(nov_allnets_test(pak));
    }

    entry = (*novell_table)[NOV_NETHASH(net)];
    if (novrip_first_hop(net, &(ipx->dhost[0]),
			 &(nidbtype *)nidb, &flags, entry,
			 IPX_ROUND_ROBIN_NO, IPX_DEFAULT_ROUTE_YES) == NULL) {
	retbuffer(pak);
	return(TRUE);
    }
    idb = nidb->idb;
    if (ipx_internal_net != 0) {
      PUT2WD(ipx->ssnet, ipx_internal_net);
      ieee_copy(&ipx_internal_hostid[0], ipx->shost);
    } else {
      PUT2WD(ipx->ssnet, nidb->novellnet);
      ieee_copy(idb->novell_node, ipx->shost);
    }
    if (!nov_send(pak)) {
	if (novell_debug)
	    buginf("\nIPX: Can't send echo");
	(*data).how = 'e'; /* Show as encapsulation error and timeout */
    } else
	novell_stat[NOVELL_ECHOREQOUT]++;
    data->next = NULL;
    data->active = TRUE;
    data->tty = stdio;
    p_enqueue(&novechoQ, data);
    edisms((blockproc *)nov_echoBLOCK, (long)data);
    if (data->active)
	p_unqueue(&novechoQ, data);
    return (data->active ? FALSE : TRUE);
}

boolean novecho_testreply (paktype *pak, xnsechotype *data)
{
    ipxhdrtype *ipx;
    ipx_cisco_ping *cp;
    int i;
    ushort bytes;

    ipx = (ipxhdrtype *)ipxheadstart(pak);

    if (!(*data).echo_test) {
	(*data).how = '!';
	return(TRUE);
    }

    if (novell_debug)
	buginf("\nnovecho_testreply:  csum out = %-04x, in = %-04x",
	       (*data).csum, ipx->cksum);

    if (ipx->cksum != (*data).csum) {
	(*data).how = 'c';
	return(TRUE);
    }

    if (novell_debug)
	buginf("\nnovecho_testreply:  data.len = %d, ipx->len = %d, pak->dgsize = %d",
	       (*data).len, ipx->len, pak->datagramsize);

    if (((*data).len != ipx->len) ||
	(ipx->len <= (XNSOVERHEADBYTES + 2)) ||
	(ipx->len > pak->datagramsize)) {
	(*data).how = 'L';
	return(TRUE);
    }
    
    if (pak->if_input->hwptr->status & IDB_ETHER)
	if ((pak->datagramsize < MINETHERSIZE) ||
	    (pak->datagramsize > (MAXETHERSIZE+ETHER_ARPA_ENCAPBYTES))) {
	    (*data).how = 'E';
	    return(TRUE);
	}


    cp = (ipx_cisco_ping *)XNSDATASTART(pak);
    if (cp->type != NOVELL_ECHO_REPLY) {
	(*data).how = 'f';
	return(TRUE);
    }
    bytes = (*data).len - XNS_MINECHOSIZE;

    for (i = 0; i < bytes; i++) {
	if (cp->data[i] != novecho_byte((*data).method, &(*data).seed)) {
	    (*data).how = 'b';
	    return(TRUE);
	}
    }
    (*data).how = '!';
    return(TRUE);
}

uchar novecho_byte (uchar method, ulong *seed)
{
    uchar result;

    switch (method) {
      case 1:
	result = (uchar)(*seed)++;
	break;
      case 2:
	result = (uchar)novecho_rand(seed);
	break;
      case 3:
	result = (uchar)*seed;
	break;
      default:
	result = (uchar)0;
	break;
    }
    return(result);
}

ushort novecho_rand (ulong *seed)
{

    *seed = *seed * 1103515245 + 12345;
    return((ushort)((*seed/65536)%32768));
}

/*
 * nov_allnets_test
 * If the destination net is -1 then this is a test of your
 * all nets broadcast facility (this is only a test).
 */

boolean nov_allnets_test (paktype *pak)
{
    ipxhdrtype *ipx, *newpak_ipx;
    paktype *newpak;
    nidbtype *nidb = NULL;

    ipx = (ipxhdrtype *)ipxheadstart(pak);

    if (!novell_debug) {
	buginf("\nIPX: Can't send echo");
	return (FALSE);
    }
    ipx->pt = ALLNETS_TEST;
    ipx->dsock = ALLNETS_TEST;
    pak->flags |= PAK_BROADCAST;

    FOR_ALL_NIDBS(nidb) {
	if (interface_up(nidb->idb)) { 
	    newpak = pak_duplicate(pak);
	    if (newpak == NULL)
		return (FALSE);
	    newpak_ipx = (ipxhdrtype *)ipxheadstart(newpak);

	    PUT2WD(newpak_ipx->ssnet, nidb->novellnet);
	    ieee_copy(nidb->idb->novell_node, newpak_ipx->shost);
	    newpak->nif_output = nidb;
	    newpak->if_output = nidb->idb;
	    clear_if_input(newpak);
	    if (nov_send1(newpak, &(newpak_ipx->dhost[0]), newpak_ipx->len)) {
		novell_stat[NOVELL_ECHOREQOUT]++;
	    }
	}
    }
    retbuffer(pak);
    return (TRUE);
}


/*
 * snmp_ipx_echo_validate
 * Validate a IPX echo for SNMP
 * Called via the snmp_ping_validate registry
 */

static int snmp_ipx_echo_validate (ciscoPingEntry_t *entry)
{
    /*
     * there is a real problem if the protocol isn't set, or if it
     * isn't set to the same value as the case registry switch value
     */
    if (!VALID(I_ciscoPingProtocol, entry->valid) ||
	(entry->ciscoPingProtocol != D_ciscoPingProtocol_novell))
	return (SNMP_PING_INVALID_PROTOCOL);

    /*
     * If the address isn't set, we simply continue
     * (the activate routine won't be called by snmp until an address has
     * been set and validated)
     */
    if (VALID(I_ciscoPingAddress, entry->valid) && 
	entry->ciscoPingAddress->length != ADDRLEN_NOVELL)
	return (SNMP_PING_INVALID_ADDRESS);

    /*
     * if the packet size is set, it better be within the legal range.
     * if the packet size is not set, we simply continue (the activation
     * routine won't be called by snmp until a packet size has been set
     * and validated)
     */
    if (VALID(I_ciscoPingPacketSize, entry->valid) &&
	((entry->ciscoPingPacketSize < XNS_MINECHOSIZE) ||
	(entry->ciscoPingPacketSize > NOVELL_MAXSIZE)))
        return (SNMP_PING_INVALID_SIZE);

    return (SNMP_PING_VALID);
}


/*
 * snmp_ipx_echo_activate
 * Send a IPX echo for SNMP
 * Called via the snmp_ping_activate registry
 */
static void snmp_ipx_echo_activate (ciscoPingEntry_t *entry)
{
    int netlen = ADDRLEN_NOVELL-IEEEBYTES, i;
    ulong net = 0L;
    uchar host[IEEEBYTES];
    ulong total_rtt, idx;
    xnsechotype data;

    /* 
     * Recover the address and net numbers from the OctetString
     * The address is the last IEEEBYTES bytes, and the preceeding is the net.
     */
    ieee_copy((char *)entry->ciscoPingAddress->octet_ptr + netlen, host);
    for (i = (netlen-1); i >= 0; i--) {
	net |= entry->ciscoPingAddress->octet_ptr[i] << 
		( (netlen - 1 - i) * 8);
    }

    data.echo_test = FALSE;

    entry->ciscoPingSentPackets = 0;
    SET_VALID(I_ciscoPingSentPackets, entry->valid);
    entry->ciscoPingReceivedPackets = 0;
    SET_VALID(I_ciscoPingReceivedPackets, entry->valid);
    entry->ciscoPingCompleted = D_ciscoPingCompleted_false;
    SET_VALID(I_ciscoPingCompleted, entry->valid);
    total_rtt = 0;

    for (idx = 1; idx <= entry->ciscoPingPacketCount; idx++) {
	entry->ciscoPingSentPackets++;
	TIMER_START(data.timeout, entry->ciscoPingPacketTimeout);
	if (send_novecho(net, host, &data, entry->ciscoPingPacketSize, 
		IPX_ECHO_TYPE_CISCO)) {
	    if (data.how=='!') {
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
	if (idx < entry->ciscoPingPacketCount) {
	    process_sleep_for(entry->ciscoPingDelay);
	}
    }
    entry->ciscoPingCompleted = D_ciscoPingCompleted_true;
}


void register_snmp_ipx_echo(void)
{
    reg_add_snmp_ping_validate(D_ciscoPingProtocol_novell,
		snmp_ipx_echo_validate,
		"snmp_ipx_echo_validate");

    reg_add_snmp_ping_activate(D_ciscoPingProtocol_novell,
		snmp_ipx_echo_activate,
		"snmp_ipx_echo_activate");
}
