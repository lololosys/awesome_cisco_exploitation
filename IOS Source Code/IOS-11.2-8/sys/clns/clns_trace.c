/* $Id: clns_trace.c,v 3.5.18.2 1996/04/19 17:29:57 asastry Exp $
 * $Source: /release/112/cvs/Xsys/clns/clns_trace.c,v $
 *------------------------------------------------------------------
 * clns_trace.c -- Traceroute for ISO CLNS 
 *  
 * 28-Sept-1989 by Paulina Knibbe
 *
 * Copyright (c) 1988-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clns_trace.c,v $
 * Revision 3.5.18.2  1996/04/19  17:29:57  asastry
 * CSCdi54904:  Fix extended option for CLNS ping to correctly handle
 * source NSAPs
 * Branch: California_branch
 *
 * Revision 3.5.18.1  1996/03/18  19:03:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.2  1996/03/07  08:40:23  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.12.1  1996/02/20  00:21:10  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/08  20:10:07  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.4  1995/12/22  21:58:05  hampton
 * Pass pointers to timestamps instead of passing the complete timestamp
 * structure.  [CSCdi45966]
 *
 * Revision 3.3  1995/11/17  08:52:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:12:40  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:08:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/08  16:42:53  hampton
 * Convert CLNS to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi38392]
 *
 * Revision 2.1  1995/06/07 20:17:52  hampton
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
#include "ttysrv.h"
#include "address.h"
#include "name.h"
#include "../x25/x25.h"
#include "config.h"
#include "../ui/command.h"
#include "../clns/clns.h"
#include "../clns/clns_route.h"
#include "../clns/clns_externs.h"
#include "../util/random.h"
#include "../ui/common_strings.h"

/*
 * Local Definitions & Storage
 */

#define MAXLINELEN	80

#define CLNS_ECHO_OPT_T enum CLNS_ECHO_OPT_T_

CLNS_ECHO_OPT_T {
    CLNS_ECHO_OPT_PADDING,
    CLNS_ECHO_OPT_QOS_GLOBAL,
    CLNS_ECHO_OPT_SOURCE_ROUTE,
    CLNS_ECHO_OPT_RECORD_ROUTE,
    CLNS_ECHO_OPT_PRIORITY,
    CLNS_ECHO_OPT_VERBOSE,
    CLNS_ECHO_OPT_DATAPACKET,
    CLNS_ECHO_OPT_HELP,
    NCLNS_ECHO_OPTCMDS
  };

/*
 * Forward declarations
 */
static void displaySuccessfulProbe(clns_echo_cb_t *, ushort);


/*
 * clns_trace
 * The trace command for CLNS 
 */

void clns_trace (char *args)
{
    /* the following data structure contains the 'per probe' information */
    clns_echo_cb_t       data;

    /* the following variables control this instance of traceroute */
    NSAP_T     source_nsap, destination_nsap, nsap;
    ushort     probe, nprobes;

    boolean    gotthere, qos, segmentation, padding;
    int        trace_id;
    ulong      seconds;
    ushort     minttl, maxttl;
    boolean    verbose;
    char       throw_away;     /* poll for interrupt */
    sys_timestamp gaptimer;
    ushort     dummy1;
    boolean    dummy2;
    ulong      dummy3;

    /* other useful stuff */
    char buff [MAXLINELEN];
    char  *namep;

    /* set default parameters */
    source_nsap[0] = '\0';
    nprobes = 3;
    minttl = 1;
    maxttl = 30;

    verbose = FALSE;
    seconds = 3;
    trace_id = 1;
    qos = TRUE;
    /*
     * Segmentation part is neccessary for CLNS ECHO packet to work.
     * As the DUID of the segmentation part is used to distinguish
     * between ECHO packet.
     * Setting segmentation to FALSE will break trace.
     */
    segmentation = TRUE;
    padding = FALSE;

    /*
     * If user already gave us destination address, use defaults
     * for all parameters.
     */
    if (!clns_parse_hostname(args, nsap)) {
	printf("\nTarget CLNS address: ");
	if (!rdtty(buff, MAXLINELEN) ||
	    !clns_parse_hostname(buff, nsap)) {
	    printf("\n%% Bad CLNS address or host name");
	    return;
	}
	nsap_copy(nsap, destination_nsap);
	if (!trace_common(buff, &seconds, &nprobes,
			  &minttl, &maxttl, CLNS_MAX_LIFETIME))
	    return;

	/*
	 * Get extended options.  Defaults are set above.
	 */
	clns_get_ping_options(&nsap, source_nsap, &qos, &segmentation,
			      &padding, &dummy1, &dummy2, &dummy2, &dummy2,
			      &dummy2, &dummy3, &dummy3, &dummy3);

	/*
	 * Special trace options?
	 */
#ifdef NOTDEF
	printf("\nExtended commands [n]: ");
	if (rdtty(buff, MAXLINELEN) && toupper(*buff) == 'Y') {
	    while (TRUE) {
		printf("%s", clns_echo_opt_prompt);
		if (!rdtty(buff,MAXLINELEN) || null(buff) )
		    break;
		result = parse_clns_options(buff);
		switch (result) {
		case CLNS_ECHO_OPT_VERBOSE:
		    verbose = (verbose ? FALSE : TRUE);
		    break;	
		case CLNS_ECHO_OPT_DATAPACKET:
		    printf ("\nSorry, can't do this yet");
		    data.data_packet = (data.data_packet ? FALSE : TRUE);
		    break;
		case CLNS_ECHO_OPT_PADDING:
		    data.flags |= CLNS_OPT_HASPAD;
		    break;
		case CLNS_ECHO_OPT_QOS_GLOBAL:
		    data.flags |= (CLNS_OPT_HASQOS | CLNS_OPT_HASGQOS);
		    break;
		case CLNS_ECHO_OPT_SOURCE_ROUTE:
		case CLNS_ECHO_OPT_RECORD_ROUTE:
		case CLNS_ECHO_OPT_PRIORITY:
		    printf("\nSorry, can't do this yet");
		    break;
		case CLNS_ECHO_OPT_HELP:
		default:
		    break;
		}
	    }
	}
#endif
    } else {
	nsap_copy(nsap, destination_nsap);
    }

    /* Lets do it */

    psion(stdio);
    printf("%s%s", type_escape_seq_abort_msg, tracing_the_route_to_msg);
    if ((namep = clns_address_lookup(destination_nsap)) != NULL) {
	printf ("%s (%n)\n", namep, destination_nsap);
    } else {
	printf("%n", destination_nsap);
    }
    flush();
    gotthere = FALSE;
    data.echo_flavor = FALSE; 
    data.data_packet = FALSE;   
    data.flags = 0;
    GET_TIMESTAMP(gaptimer);

    for (data.v.trace.ttl = minttl;
	 data.v.trace.ttl <= maxttl; data.v.trace.ttl++) {
	clear_nsap(data.v.trace.currentGateway);
	printf("\n %-2u ", data.v.trace.ttl);
	for (probe = 0; probe < nprobes; ++probe) {

	    /* Wait for our inter-packet gap to expire before continuing. */

	    process_sleep_until(&gaptimer);
	    TIMER_START(gaptimer, 2*CLNS_DEF_ERPDU_INTERVAL);

	    /* each probe should have its own trace_id so we can 
	     * distinguish responses
	     */
	    data.echo_id = random_gen();
	    GET_TIMESTAMP(data.xmittime);
	    TIMER_START(data.timeout, seconds * ONESEC);

	    /* actually send a packet */

	    if (send_clns_echo(destination_nsap, source_nsap,
                               &data, CLNS_MINECHOSIZE, qos, segmentation,
			       padding, TRUE)) {
		switch (data.how) {
		  case ('&'):    /* time to live exceeded */
		      displaySuccessfulProbe(&data,probe);
		      printf(" !");
		      break;

		    case ('U'):
		      displaySuccessfulProbe(&data,probe);
		      printf(" U");
		      gotthere = TRUE;
		      break;

		    case ('!'):   /* got to the dest - quit now */
		      displaySuccessfulProbe(&data,probe);
		      printf(" !");
		      gotthere = TRUE;
		      break;

		    case ('I'):   /* user interrupted */
		      if (verbose)
			  printf(" I");
		      goto cleanup;

		    case ('T'):   /* timeout */
		      printf(" * ");
		      break;
	  
		    /* Note that in the case of congestion, we continue to
		     * send probes. One packet getting thrown away doesn't
		     * mean the trace should stop. If all packets, get 
		     * thrown away, that is interesting information
		     */
		    case ('C'): /* congestion */
		      displaySuccessfulProbe(&data,probe);
		      printf(" C ");
		      break;

		    default:
		      printf("\nPacket discarded. Reason: %x: %s",
			     data.how, discard_string(data.how));
		      break;
		  } /* switch */

		flush();
		/* the following read of getc is an attempt to get the
		   psipending set if the user types an escape
		   when we are using a vty
		   */
		do
		    ;
		while ((throw_away = getc(stdio)) != -1);
	    } else
		goto cleanup; /* send failed */
	} /* for  probe */
	if (gotthere)
	    goto cleanup;
    } /* for  ttl */

    if (verbose)
	printf("\nDestination not found inside max ttl diameter.");

cleanup:
    flush();
    psioff(stdio);

/*     free(options); */

} /* clns_trace */

/*
 *    display the results of a successful traceroute probe
 */
static void displaySuccessfulProbe (clns_echo_cb_t *data, ushort probe)
{
    char *namep;

    namep = clns_address_lookup(data->v.trace.who);
    if (probe == 0) {
	if (namep)
	    printf("%s(%n)", namep, data->v.trace.who);
	else
	    printf("%n", data->v.trace.who);
    } else {
	if (!nsap_equ(data->v.trace.who, data->v.trace.currentGateway)) {
	    /* first response from this gateway */
	    printf("\n     ");
	    if (namep)
		printf("%s(%n)", namep, data->v.trace.who);
	    else
		printf(" %n", data->v.trace.who);
	}  /* same source as last probe response */
    }
    nsap_copy (data->v.trace.who, data->v.trace.currentGateway);
    printf(" %lu msec", data->deltatime);
    flush();
}

