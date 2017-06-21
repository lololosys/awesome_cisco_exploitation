/* $Id: clns_echo.c,v 3.4.4.4 1996/08/08 14:50:13 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/clns/clns_echo.c,v $
 *------------------------------------------------------------------
 * clns_echo.c -- Support for the CLNS Echo facility
 * 
 * 25-August-1988 by Jim Forster
 * Jan-27-1989 by Jim Forster
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clns_echo.c,v $
 * Revision 3.4.4.4  1996/08/08  14:50:13  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Use new scheduler primitives.  Get rid of gratuitous type coercion.
 *
 * Revision 3.4.4.3  1996/08/07  08:56:57  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.4.4.2  1996/04/19  17:29:49  asastry
 * CSCdi54904:  Fix extended option for CLNS ping to correctly handle
 * source NSAPs
 * Branch: California_branch
 *
 * Revision 3.4.4.1  1996/03/18  19:02:34  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/16  06:30:38  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2.26.2  1996/03/07  08:39:53  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  00:20:48  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/03/08  22:45:31  asastry
 * CSCdi50789:  CLNS ping to local address causes crash if debug is turned
 * on.
 *
 * Revision 3.3  1996/02/08  20:10:02  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.2  1995/11/17  08:51:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:07:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/08  16:42:37  hampton
 * Convert CLNS to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi38392]
 *
 * Revision 2.3  1995/06/18 19:39:34  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  12:58:53  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:17:04  hampton
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
#include "address.h"
#include "ttysrv.h"
#include "../x25/x25.h"
#include "config.h"
#include "../ui/command.h"
#include "../clns/clns.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../clns/clns_externs.h"
#include "sys_registry.h"
#include "subsys.h"
#include "../snmp/sr_pingmib.h"
#include "../snmp/snmp_registry.h"
#include "../util/random.h"
#include "../ui/common_strings.h"

#define CLNS_PING_PATTERN 0xABCD	/* Data pattern to send */

static int snmp_clns_echo_validate (ciscoPingEntry_t *entry);
static void snmp_clns_echo_activate (ciscoPingEntry_t *entry);

/*
 * Local Definitions & Storage
 */

#define MAXLINELEN	80

#define CLNS_ECHO_OPT_T enum CLNS_ECHO_OPT_T_

CLNS_ECHO_OPT_T {
    CLNS_ECHO_OPT_PADDING,
    CLNS_ECHO_OPT_QOS_GLOBAL,
/*
    CLNS_ECHO_OPT_SOURCE_ROUTE,
    CLNS_ECHO_OPT_RECORD_ROUTE,
    CLNS_ECHO_OPT_PRIORITY,
*/
    CLNS_ECHO_OPT_VERBOSE,
    CLNS_ECHO_OPT_DATAPACKET,
    CLNS_ECHO_OPT_HELP,
    NCLNS_ECHO_OPTCMDS
  };

static char *const clns_echo_opt_cmds[NCLNS_ECHO_OPTCMDS] = {
    "padding",
    "qos-global",
/*
    "source-route",
    "record-route",
    "priority",
*/
    "verbose",
    "data-packet",
    "?"
  };

const char clns_echo_opt_prompt[] =
/*
  "Padding, QOS-Global, Source-route, Record-route, Priority, \n   Verbose, Data-packet, or ? : "; 
*/
  "Padding, QOS-Global, Verbose, Data-packet, or ? : ";

static const ushort clns_echo_opt_flags[NCLNS_ECHO_OPTCMDS] = {
    CMD_PRV, 
    CMD_PRV,
/*
    CMD_PRV,
    CMD_PRV,
    CMD_PRV,
*/
    CMD_PRV,
    CMD_PRV,
    CMD_INV+CMD_PRV
};

static char *const clns_echo_opt_help[NCLNS_ECHO_OPTCMDS] = {
    "Padding                          Padding Option",
    "QOS-Global                       Allows Congestion Bit",
/*
    "Source-route <hop1> <hop2> ...   Partial source route",
    "Record-route <nhops>             Partial record route",
    "Priority <n>                     Sets Priority",
*/
    "Verbose                          Toggle verbose reporting",
    "Data-packet                      Send Data Packet (not Echo)",
    NULL
};

/*
 * clns_echoBLOCK
 * Wait for an CLNS Echo Reply or timeout.
 */

boolean clns_echoBLOCK (ulong arg)
{
    register clns_echo_cb_t *data;

    data = (clns_echo_cb_t *) arg;

    if (!data->active)			/* if block flag reset, wakeup */
	return(FALSE);
    if (psipending2(data->tty)){	/* interrupted? */
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
 * send_clns_echo
 * Send a CLNS_ echo to a host and block for a reply.
 * Returns TRUE if echo reply received. False otherwise.
 */

boolean send_clns_echo (NSAP_T nsap, NSAP_T source_nsap, clns_echo_cb_t
			*data, int bytes, boolean qos, boolean
			segmentation, boolean padding, boolean verbose)
{
    paktype *pak;
    clnshdrtype *clns;
    uchar *ptr;
    ushort *dataptr;
    clns_segpart_t *segptr;
    NSAP_T *my_nsap;
    ushort pattern;
    int bytesleft;

    /*
     * Get an appropriate sized buffer, and clear the data portion of
     * it and some clns fields
     */
    pak = clns_getbuffer(bytes);
    if (!(pak))
	return(FALSE);

    clns = (clnshdrtype *)clnsheadstart(pak);

    /*
     * Fixed portion of the packet header
     */
    clns->pid = NLP_ISO8473;

    /*
     * Skip header length for now
     */
    clns->version = CLNS_VERSION;
    if (data->echo_flavor) 
	clns->lifetime = clns_packet_lifetime;
    else
	clns->lifetime = data->v.trace.ttl;
    clns->sp = 0;
    clns->ms = 0;	/* this is the last */
    clns->er = 1;	/* send us error reports */
    clns->type = (data->data_packet) ? CLNS_TYPE_DATA : CLNS_TYPE_ERQ;
    PUTSHORT(clns->clns_seglen, bytes);

    /*
     * address part
     */
    ptr = PAK2ADDR(pak);
    pak->clns_dstaddr = (uchar *)ptr;
    nsap_copy(nsap, *(NSAP_T *)ptr);
    ptr += ptr[0] + 1;
    pak->clns_srcaddr = (uchar *)ptr;
    my_nsap = clns_get_local_npai(clns_find_pdb(pak->clns_dstaddr));
    
    /*
     * If source NSAP was specifed, use it as the source address in the
     * outgoing packet.
     */ 
    if (source_nsap[0] != '\0') 
        nsap_copy(source_nsap, *(NSAP_T*)ptr);
    
    /*
     * No NET or NSAP configured.
     */
    else if (!*my_nsap) {
	if (verbose)
	    printf ("\nCLNS: No NET or NSAP configured");
	return (FALSE);
    }
    else
        nsap_copy(*my_nsap, *(NSAP_T*)ptr);

    ptr += ptr[0] + 1;

    /*
     * segmentation part
     */
    if (segmentation) {
	clns->sp = 1;			/* allow segmentation */
	pak->clns_segpart = ptr;
	segptr = (clns_segpart_t*) ptr;
	PUTSHORT(segptr->duid, data->echo_id);
	segptr->segoffset[0] = 0;	/* this is the first segment */
	segptr->segoffset[1] = 0;
	PUTSHORT(segptr->length, bytes); /* total length */
	ptr = ((uchar *) segptr) + sizeof(clns_segpart_t);
    }

    /*
     *  options
     */
    if (qos) {
	*ptr++ = CLNS_OPT_QOS;
	*ptr++ = CLNS_OPT_GQOS_LEN;
	*ptr++ = CLNS_QOS_GLOBAL;
    }
    if (padding) {
	*ptr++ = CLNS_OPT_PADDING;
	*ptr++ = 1;
	*ptr++ = 0;
    }

    /*
     * Header done -- fill in the header length 
     */
    clns->hlen = ptr - (uchar *)clnsheadstart(pak);

    /*
     * Fill in the packet with the data pattern, or a rotating pattern
     * if so specified.  If odd length, we don't bother with the last byte.
     */
    pattern = data->pattern;
    if (data->rotating_pattern)
	pattern = data->echo_id;
    bytesleft = bytes - clns->hlen;
    dataptr = (ushort *) ptr;
    while (bytesleft > 1) {
	PUTSHORT(dataptr, pattern);
	dataptr++;
	bytesleft -= sizeof(ushort);
	if (data->rotating_pattern)
	    pattern++;
    }
    data->corrupted_words = 0;

    /*
     * Send the packet already
     */
    pak->if_input = pak->if_output = NULL;
    data->next = NULL;
    data->active = TRUE;
    data->tty = stdio;
    enqueue(&clns_echoQ, data);
    if (!clns_write(pak)){
	if (verbose)
	    printf("\nCLNS: cannot send ECHO");
	p_unqueue(&clns_echoQ, data);
	return(FALSE);
    }
    clns_traffic[CLNS_ECHOREQOUT]++;
    edisms((blockproc *)clns_echoBLOCK, (ulong) data);
    if (data->active)
	p_unqueue(&clns_echoQ, data);
    if (data->echo_flavor)
	return (data->active ? FALSE : TRUE);
    else
	return (TRUE);
}

/*
 * parse_clns_options
 * Parse the IP option keywords of the ping options subcommand
 * *** LATER -- generalize parse_clns_options & parse_ipoptions
 */

int parse_clns_options (char *buff)
{
    int index, matches;

    if (!null(buff)) {
	matches = nmatch(buff, clns_echo_opt_cmds,
			 clns_echo_opt_flags, NCLNS_ECHO_OPTCMDS,&index);
	if (matches > 1) {
	    printf("%% Ambiguous - \"%s\"",buff);
	    return(-1);
	}
	if (matches == 0) {
	    printf("%% No such option - \"%s\"",buff);
	    return(-1);
	}
    } else
	return(-1);
    if (index == (int) CLNS_ECHO_OPT_HELP) {
	help_command(clns_echo_opt_help, clns_echo_opt_flags, 
		     NCLNS_ECHO_OPTCMDS);
	printc('\n');
    	return(-1);
    }
    return(index);
}

/*
 * clns_ping_parse
 * Parse a ping command line.  Called by SERVICE_PING_PARSE.
 * Return TRUE if positive identification of a CLNS address.
 */

boolean clns_ping_parse (char *args, int hint, int *index,
			 char **cp, boolean *running)
{
    NSAP_T nsap;

    *running = clns_running;
    *cp = "CLNS";
    *index = LINK_CLNS;

    if (!clns_running)
	return(FALSE);

    if (hint < 0)
	return(strmatch(args, "clns", FALSE));
    else if (hint == LINK_ILLEGAL) {
	if (clns_parse_hostname(args, nsap))
	    return(TRUE);
    } else if ((hint == LINK_CLNS) &&
	       (!args || !*args || clns_parse_hostname(args, nsap)))
	return(TRUE);
    return(FALSE);
}

/*
 * clns_get_ping_options
 * Find out if we want something funky to play with.
 */

void clns_get_ping_options (NSAP_T *nsap, NSAP_T source_nsap, boolean *qos,
			    boolean *segmentation, boolean *padding,
			    ushort *pattern, boolean *validate,
			    boolean *rotate_pattern, boolean *verbose,
			    boolean *sweep, ulong *sweepmin, ulong *sweepmax,
			    ulong *sweepint)
{
    NSAP_T *my_nsap;
    boolean retry;
    char buff[MAXLINELEN];
    boolean parsed;

    /*
     * Special ping options?
     */
    printf("\nExtended commands [n]: ");
    if (rdtty(buff, MAXLINELEN) && toupper(*buff) != 'Y')
	return;

    /* Optional source address to be used as the return address for the
     * echo packet. If nothing is typed, use default source address - this
     * is done in 'send_clns_echo'.
     */
    my_nsap = clns_get_local_npai(clns_find_pdb(*nsap));
    retry = TRUE; 
    while (retry) {
	printf("Source CLNS address [%s]: ", nsap_string(*my_nsap));

	if (rdtty(buff, MAXLINELEN) && 
 		  (clns_parse_hostname(buff, source_nsap) || null(buff)))
 	    break;
 	else 
 	    printf("\n%% Bad source CLNS address - %s\n", buff);
    }

    /*
     * Allowing the user to disable segmentation breaks both ping and
     * trace.  But it's really useful for testing.
     * *segmentation = yes_or_no("Include segmentation information",
     *                           *segmentation, TRUE);
     */
    *qos = yes_or_no("Include global QOS option", *qos, TRUE);
    *padding = yes_or_no("Pad packet", *padding, TRUE);
    *validate = yes_or_no("Validate reply data", FALSE, TRUE);
    parsed = TRUE;
    do {
	printf("\nData pattern [0x%-04x]: ", *pattern);
	if (rdtty(buff, MAXLINELEN) && !null(buff)) {
	    if (buff[0] == 'r' || buff[0] == 'R') {
		*rotate_pattern = TRUE;
		parsed = TRUE;
	    } else {
		*pattern = parse_unsigned(buff, &parsed);
		if (!parsed) {
		    printf("\nInvalid pattern, try again.");
		}
	    }
	}
    } while (!parsed);
    printf("Sweep range of sizes [n]: ");
    if (rdtty(buff, MAXLINELEN) && toupper(*buff) == 'Y') {
	if (ping_get_sweep(sweepmin, sweepmax, sweepint))
	    *sweep = TRUE;
    }
    *verbose = yes_or_no("Verbose reply", *verbose, TRUE);
}

/*
 * clns_ping
 * The echo command for CLNS 
 */

void clns_ping (char *args)
{
    long size, repeat, seconds, hits;
    long i;
    boolean verbose, qos, padding, segmentation;
    long tmin = 999999999, tmax = 0, tsum = 0;
    char buff[MAXLINELEN];
    NSAP_T source_nsap;
    clns_echo_cb_t *data;
    boolean data_packet;
    NSAP_T nsap;
    ushort pattern, echo_id;
    boolean validate, rotating_pattern, sweep;
    ulong sweepmin, sweepmax, sweepint;

    size = 100;				/* default datagram size */
    repeat = 5;				/* default repeat count */
    seconds = 2;			/* default timeout */
    verbose = FALSE;			/* not verbose */
    source_nsap[0] = '\0';
    data = NULL;
    data_packet = FALSE;
    qos = TRUE;
    pattern = CLNS_PING_PATTERN;
    validate = FALSE;
    rotating_pattern = FALSE;
    sweep = FALSE;
    sweepmin = CLNS_MINECHOSIZE;
    sweepmax = CLNS_MAXECHOSIZE;
    sweepint = 1;

    /*
     * Segmentation part is neccessary for CLNS ECHO packet to work.
     * As the DUID of the segmentation part is used to distinguish
     * between ECHO packet.
     * Setting segmentation to FALSE will break ping.
     */
    segmentation = TRUE;
    padding = FALSE;

    /*
     * Get CLNS address
     */
    if (!clns_parse_hostname(args, nsap)) {
	printf("Target CLNS address: ");
	if (!rdtty(buff, MAXLINELEN) ||
 	    !clns_parse_hostname(buff, nsap)) {
	    printf("\n%% Bad CLNS address - %s", buff);
	    return;
	}

	/*
	 * Ask for repeat count, size, and timeout interval
	 */
	if (!ping_common(&repeat, &size, CLNS_MINECHOSIZE, CLNS_MAXECHOSIZE, 
			 &seconds))
	    return;

	/*
	 * Get extended options.  Defaults are set above.
	 */
	clns_get_ping_options(&nsap, source_nsap, &qos, &segmentation,
			      &padding, &pattern, &validate, &rotating_pattern,
			      &verbose, &sweep, &sweepmin, &sweepmax,
			      &sweepint);
	if (sweep) {
	    size = sweepmin;
	}
    }
        
    /*
     * Let's do it
     */
    psion(stdio);
    printf(type_escape_seq_abort_msg);
    if (sweep) {
	printf("\nSending %d, [%d..%d]", repeat, sweepmin, sweepmax);
    } else {
	printf("\nSending %d, %d", repeat, size);
    }
    printf("-byte CLNS Echos with timeout %d seconds", seconds);
    flush();
    echo_id = random_gen();
    if (rotating_pattern) {
	printf("\nPacket has rotating data pattern starting with %-0#6x",
	       echo_id);
    } else {
	if (pattern != CLNS_PING_PATTERN) {
	    printf("\nPacket has data pattern %-0#6x", pattern);
	}
    }
    for (i = 0, hits = 0; (i < repeat) && (psipending() == FALSE); i++) {
	data = malloc(sizeof(clns_echo_cb_t));
	if (!data)
	    return;
	data->echo_id = echo_id++;
	if ((i % 70) == 0 && !verbose) {
	    printc('\n');
	    flush();
	}

	data->data_packet = data_packet;
	GET_TIMESTAMP(data->xmittime);
	TIMER_START(data->timeout, seconds * ONESEC);
	data->echo_flavor = TRUE;
	data->validate = validate;
	data->pattern = pattern;
	data->rotating_pattern = rotating_pattern;
	if (send_clns_echo(nsap, source_nsap, data, size, qos,
			   segmentation, padding, TRUE)){
	    switch (data->how) {
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
	      case 'I':
	      case 'T':
		unqueue(&clns_echoQ, data);		
		break;
	      case '&':
	      case 'U':
		break;
	      case 'C':
		if (verbose)
		    printf("\nCorrupted reply to request %d (%d ms), "
			   "pattern = %-0#6x, offset %d, %d words corrupted",
			   i, data->deltatime, data->corruption,
			   data->corruption_offset, data->corrupted_words);
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
	/*
	 * read the input so that we could recoganize the escape 
	 * charcter from the vty.
	 */
	while (getc(stdio) != -1) {
	    /* do nothing */
	}

	/* Dink with size if sweeping. */

	if (sweep) {
	    size += sweepint;
	    if (size > sweepmax)
		size = sweepmin;
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
 * clns_echo
 * handle a clns echo request packet
 */

void clns_echo (paktype *pak)
{
    clnshdrtype *clns;
    paktype *rpak;
    clnshdrtype *rpak_clns;
    uchar *ptr;
    clns_segpart_t *segptr;
    int rep_pdu_len;

    clns = (clnshdrtype *)clnsheadstart(pak);

    if (clns_packet_debug) {
        if (pak->if_input) {
	    buginf("\nCLNS: Echo PDU received on %s len %d\n      from %n",
		   pak->if_input->namestring, pak->datagramsize,
		   pak->clns_srcaddr);
        }
        else {
	    buginf("\nCLNS: Locally generated echo PDU received, len %d"
		   "\n      from %n",
		   pak->datagramsize, pak->clns_srcaddr);
        }
    }

    /*
     * Get an appropriate sized buffer, and clear the data portion of
     * it and some clns fields
     */
    rpak = clns_getbuffer(GETSHORT(clns->clns_seglen) + 
			  CLNS_MAXHEADERBYTES);
    if (!(rpak))
	return;

    rpak_clns = (clnshdrtype *)clnsheadstart(rpak);
    
    /*
     * Fixed portion of the packet header
     */
    rpak_clns->pid = NLP_ISO8473;
    /* skip header length for now */
    rpak_clns->version = CLNS_VERSION;
    rpak_clns->lifetime = clns_packet_lifetime;
    rpak_clns->sp = 1;	/* allow segmentation */
    rpak_clns->ms = 0;	/* this is the last */
    rpak_clns->er = 1;	/* send us error reports */
    rpak_clns->type = CLNS_TYPE_ERP;
    /* skip seg length for now also */
    /*
     * address part
     */
    ptr = PAK2ADDR(rpak);
    rpak->clns_dstaddr = (uchar *)ptr;
    nsap_copy(pak->clns_srcaddr, *(NSAP_T *)ptr);
    ptr += ptr[0] + 1;
    rpak->clns_srcaddr = (uchar *)ptr;
    nsap_copy(pak->clns_dstaddr, *(NSAP_T*)ptr);
    ptr += ptr[0] + 1;
    /*
     * segmentation part (except for length)
     */
    rpak->clns_segpart = ptr;
    segptr = (clns_segpart_t*) ptr;
    PUTSHORT(segptr->duid, random_gen());/* change LATER */
    segptr->segoffset[0] = 0;		/* this is the first segment */
    segptr->segoffset[1] = 0;
    ptr = ((uchar *) segptr) + sizeof(clns_segpart_t);
    /*
     *  options -- copy Global QOS & Padding if present
     */
    if (pak->clns_flags & CLNS_OPT_HASGQOS) {
	rpak->clns_optpart = ptr;
	*ptr++ = CLNS_OPT_QOS;
	*ptr++ = CLNS_OPT_GQOS_LEN;
	*ptr++ = CLNS_QOS_GLOBAL;
    }
    if (pak->clns_flags & CLNS_OPT_HASPAD) {
	if (!rpak->clns_optpart) {
	    rpak->clns_optpart = ptr;
	}
	*ptr++ = CLNS_OPT_PADDING;
	*ptr++ = 1;
	*ptr++ = 0;
    }
    /*
     * Header done -- fill in the header length, copy the entire request
     * packet, and then fill in the segment length
     */
    rpak_clns->hlen = ptr - (uchar *)clnsheadstart(rpak); 
    rpak->clns_datapart = ptr;
    bcopy(clnsheadstart(pak), ptr, GETSHORT(clns->clns_seglen));
    rep_pdu_len = (ptr + GETSHORT(clns->clns_seglen)) - (uchar *)rpak_clns;
    PUTSHORT(rpak_clns->clns_seglen, rep_pdu_len);
    PUTSHORT(segptr->length, rep_pdu_len);
    /*
     * send the packet
     */
    rpak->if_input = rpak->if_output = NULL;
    if (clns_write(rpak)) {
	clns_traffic[CLNS_ECHOREPOUT]++;
    }
}


/*
 * clns_echo_rep
 * process a clns echo reply by searching the queue of outstanding
 * echo requests and matching on the segment id.  Note that the entire echo
 * request packet is contained within the echo reply packet, so we have two
 * headers to skip past in order to look at the payload.
 */
void clns_echo_rep (paktype *pak)
{
    clnshdrtype    *clns;
    clnshdrtype    *erq;
    clns_segpart_t *sp;
    clns_echo_cb_t *ep, *next;
    uchar *p, *endp;
    ushort *dataptr;
    ushort data;
    int bytesleft;

    if (clns_packet_debug) {
        if (pak->if_input) {
	    buginf("\nCLNS: Echo Reply PDU received on %s!",
		   pak->if_input->namestring);
	}
	else {
	    buginf("\nCLNS: Echo Reply PDU received (local address)");
        }
    }
    if (QUEUEEMPTY(&clns_echoQ))
      return;
    p = pak->clns_datapart;
    if (!p)
      return;

    clns = (clnshdrtype *)clnsheadstart(pak);
    bytesleft = GETSHORT(clns->clns_seglen);
    endp = clnsheadstart(pak) + bytesleft;	/* 1 past end */
    erq = (clnshdrtype *)pak->clns_datapart;
    if (!erq->sp)
	return;			/* original was not segmented, return */
    /* original had seg. part. Find seg id, use as echo id */
    if ((p += CLNS_FIXEDHDR_SIZE) >= endp)	/* go past fixed part */
      return;
    if ((p += *p + 1) >= endp)		/* go past dst addr */
      return;
    if ((p += *p + 1) >= endp)		/* go past src addr */
      return;
    if (p + sizeof(clns_segpart_t) > endp)
      return;
    sp = (clns_segpart_t *)p;		/* segmentation part */

    ep = clns_echoQ.qhead;
    while (ep) {
	next = (clns_echo_cb_t *)ep->next;
	if (ep->echo_id == GETSHORT(sp->duid)) {
	    ep->deltatime = ELAPSED_TIME(ep->xmittime);
	    ep->how = '!';
	    ep->active = FALSE;
 	    /*
 	     * If requested, validate the received packet.  If a rotating
 	     * pattern is in use, duplicate it here.
 	     */
 	    if (ep->validate) {
 		dataptr = (ushort *) (((uchar *) clns) + clns->hlen); /* Skip first header */
		bytesleft -= clns->hlen;
		clns = (clnshdrtype *) dataptr;	/* Position at second header */
 		dataptr = (ushort *) (((uchar *) clns) + clns->hlen); /* Skip second header */
		bytesleft -= clns->hlen;
 		if (ep->rotating_pattern)
 		    ep->pattern = ep->echo_id;
		while (bytesleft > 1) {
		    data = GETSHORT(dataptr);
 		    if (data != ep->pattern) {
 			if (ep->corrupted_words == 0) {
 			    ep->corruption = data;
 			    ep->how = 'C';
 			    ep->corruption_offset =
 				(uchar *) dataptr - (uchar *) clns;
 			}
 			ep->corrupted_words++;
 		    }
 		    dataptr++;
		    bytesleft -= sizeof(ushort);
 		    if (ep->rotating_pattern)
 			ep->pattern++;
 		}
 		if (clns_packet_debug && ep->corrupted_words) {
 		    buginf("\ncorrupted, pattern %-0#6x, offset %d, count %d",
 			   ep->corruption,
 			   ep->corruption_offset,
 			   ep->corrupted_words);
 		}
	    }
	    if (!ep->echo_flavor) {
		nsap_copy (pak->clns_srcaddr, ep->v.trace.who);
	    }
	    unqueue(&clns_echoQ, ep);
	    break;
	}
	ep = next;
    }
}


/*
 * snmp_clns_echo_validate
 * Validate a clns echo for SNMP
 * Called via the snmp_ping_validate registry
 */

static int snmp_clns_echo_validate (ciscoPingEntry_t *entry)
{
    /*
     * there is a real problem if the protocol isn't set, or if it
     * isn't set to the same value as the case registry switch value
     */
    if (!VALID(I_ciscoPingProtocol, entry->valid) ||
	(entry->ciscoPingProtocol != D_ciscoPingProtocol_clns))
	return (SNMP_PING_INVALID_PROTOCOL);

    /*
     * If the address isn't set, we simply continue
     * (the activate routine won't be called by snmp until an address has
     * been set and validated)
     */
    if (VALID(I_ciscoPingAddress, entry->valid) &&
	(entry->ciscoPingAddress->length < 4 || 
	entry->ciscoPingAddress->length > ADDRLEN_CLNS ||
	entry->ciscoPingAddress->octet_ptr[0] !=
	entry->ciscoPingAddress->length - 1))
	return (SNMP_PING_INVALID_ADDRESS);

    /*
     * if the packet size is set, it better be within the legal range.
     * if the packet size is not set, we simply continue (the activation
     * routine won't be called by snmp until a packet size has been set
     * and validated)
     */
    if (VALID(I_ciscoPingPacketSize, entry->valid) &&
	((entry->ciscoPingPacketSize < CLNS_MINECHOSIZE) ||
	(entry->ciscoPingPacketSize > CLNS_MAXECHOSIZE)))
	return (SNMP_PING_INVALID_SIZE);

    return (SNMP_PING_VALID);
}


/*
 * snmp_clns_echo_activate
 * Send a clns echo for SNMP
 * Called via the snmp_ping_activate registry
 */
static void snmp_clns_echo_activate (ciscoPingEntry_t *entry)
{
    ulong total_rtt, idx;
    NSAP_T source_nsap;
    clns_echo_cb_t *data;
    NSAP_T nsap;

    source_nsap[0] = '\0';
    /*
     * Recover the NSAP address from the OctetString. 
     * This is a snap as a NSAP is just NSAP_T[CLNS_MAX_NSAP_LENGTH]
     * The first byte is the length of the address. 
     * To ping 45.2300.01 use '04 45 23 00 01'
     */
    for (idx = 0; idx < entry->ciscoPingAddress->length; idx++)
	nsap[idx] = entry->ciscoPingAddress->octet_ptr[idx];

    entry->ciscoPingSentPackets = 0;
    SET_VALID(I_ciscoPingSentPackets, entry->valid);
    entry->ciscoPingReceivedPackets = 0;
    SET_VALID(I_ciscoPingReceivedPackets, entry->valid);
    entry->ciscoPingCompleted = D_ciscoPingCompleted_false;
    SET_VALID(I_ciscoPingCompleted, entry->valid);
    total_rtt = 0;
        
    /*
     * Let's do it
     */
    for (idx = 1; idx <= entry->ciscoPingPacketCount; idx++) {
	entry->ciscoPingSentPackets++;
	data = malloc(sizeof(clns_echo_cb_t));
	if (!data)
	    return;
	data->data_packet = FALSE;
	data->echo_id = random_gen();
	GET_TIMESTAMP(data->xmittime);
	TIMER_START(data->timeout, entry->ciscoPingPacketTimeout);
	data->echo_flavor = TRUE;
	if (send_clns_echo(nsap, source_nsap, data, entry->ciscoPingPacketSize,
		 TRUE, TRUE, FALSE, FALSE)) {
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
}



void register_snmp_clns_echo(void)
{
    reg_add_snmp_ping_validate(D_ciscoPingProtocol_clns,
                snmp_clns_echo_validate,
                "snmp_clns_echo_validate");

    reg_add_snmp_ping_activate(D_ciscoPingProtocol_clns,
                snmp_clns_echo_activate,
                "snmp_clns_echo_activate");
}

