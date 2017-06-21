/* $Id: ipoptions.c,v 3.5.22.5 1996/08/28 12:50:03 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipoptions.c,v $
 *------------------------------------------------------------------
 * ipoptions.c -- Support for IP level options
 *
 * July 1987, Bill Westfield
 * July 1988, Eric B. Decker (IP Options restructuring and IP Security)
 * June 1989, David R. Hampton (full blown IP Security)
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipoptions.c,v $
 * Revision 3.5.22.5  1996/08/28  12:50:03  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.22.4  1996/08/15  12:49:41  albright
 * CSCdi59020:  Cant forward IP packet (options)
 * Branch: California_branch
 *
 * Revision 3.5.22.3  1996/06/28  23:17:44  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.5.22.2  1996/06/18  01:45:46  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.5.22.1  1996/03/18  20:17:27  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.6.2  1996/03/07  09:46:44  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.6.1  1996/02/20  01:00:12  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/01/24  20:15:06  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.4  1996/01/11  02:26:47  jenny
 * CSCdi46513:  TCP/IP need code clean up (part 2)
 * . removed ip_tuba.c and clns_tuba.c.
 * . removed lots of tuba related registry stubs and code.
 *
 * Revision 3.3  1996/01/10  02:22:06  raj
 * CSCdi40929:  traceroute fails, ping works. port unreachables not sent
 * Allow unreachables within the first 3 minutes and fix timer code to
 * remove ip_icmpokay variable.
 *
 * Revision 3.2  1995/11/17  09:35:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:57:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/09/25  08:32:21  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.7  1995/09/13  07:01:34  hampton
 * All code should use the routine GETSHORT()/PUTSHORT() instead of using
 * the routines getshort()/putshort().  [CSCdi40249]
 *
 * Revision 2.6  1995/09/13 04:07:19  hampton
 * The Fall of the House of Msclock.  [CSCdi40182]
 *
 * Revision 2.5  1995/09/09 00:53:23  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.4  1995/08/31  18:38:54  gchristy
 * CSCdi39471:  IP: hang in IP forwarding
 * - always bail if an option length is bogus
 *
 * Revision 2.3  1995/07/18  21:40:33  ahh
 * CSCdi37213:  Telnet using loose source routing of IP causes hang
 * When copying source route, use sanity-checked length from IP header.
 *
 * Revision 2.2  1995/07/13  23:30:12  mikel
 * CSCdi37031:  DNSIX: eso packet not being stripped properly
 * fix incorrect pointer to extended security option
 *
 * Revision 2.1  1995/06/07  20:59:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "../ui/debug.h"
#include "ip.h"
#include "ipoptions.h"
#include "ip_debug.h"
#include "../tcp/tcp_debug.h"
#include "config.h"
#include "../if/network.h"
#include "../iprouting/route.h"
#include "logger.h"
#include "../ip/msg_ip.c"		/* Not a typo, see logger.h */
#include "../ip/msg_ipsecure.c"		/* Not a typo, see logger.h */
#include "../ip/ip_registry.h"
#include "../parser/parser_defs_tcpip.h"
#include "../os/clock.h"

/* #define DEBUG_OPTS TRUE */

/*
 * Local Storage
 */

ip_sec_compartment_info ip_sec_info[MAX_NLESO];
int ip_sec_info_entries;
uchar ip_secopt_bytes;			/* bytes in default security option */

/*
 * add_ipoption
 * Add the new option to the option list
 */

boolean add_ipoption (ipoptiontype *oldopts, ipoptiontype *newopt)
{
    register int i;

    if ((oldopts->length + newopt->length) > MAXIPOPTIONBYTES)
	return(FALSE);
    bcopy(newopt->ipopts, &oldopts->ipopts[oldopts->length], newopt->length);
    oldopts->length += newopt->length;
    for (i = oldopts->length; i & 3; i++)
	oldopts->ipopts[i] = PADDING;
    oldopts->paddedlength = i;
    return(TRUE);
}

/*
 * extract_ipoptions
 * Extract the options from a packet into an option data structure,
 * setting the length field appropriately.  Note that the padded length
 * and the real length are set to the same thing, since there is no way
 * at this point to tell padding from real options.
 */

void extract_ipoptions (iphdrtype *ip, ipoptiontype *options)
{
    register int length;

    length = ltob(ip->ihl) - MINIPHEADERBYTES;
    if (length > 0) {
	options->length = length;
	options->paddedlength = length;
	bcopy(&ip[1], options->ipopts, length);
    } else {
	options->length = 0;
	options->paddedlength = 0;
    }
}

/*
 * ip_option_len
 * Determine length of this IP header option, and perform basic sanity
 * checking on that length.
 */

static inline ushort ip_option_len (uchar *opts, int optarea_len)
{
    uchar len;
   
    if (IPOPT_TLV(*opts)) {
	len = *(opts + 1);
	if (len < 2 || len > MAXIPOPTIONBYTES || len > optarea_len)
	    return 0;
	return len;
    } else
	return 1;
}

/*
 * print_ipoptions
 * Print out the options in human readable form.
 * Note well:  printing routines we call that detect a bogus length field
 * should return 0 to ensure this routine terminates.
 */

void print_ipoptions (ipoptiontype *options)
{
    int i;
    ushort len;
    boolean optok;

    printf("Total option bytes= %d, padded length=%d\n", 
        options->length, options->paddedlength);
    optok = TRUE;
    for (i = 0; (i < options->length) && optok; i += len) {
	len = ip_option_len(&options->ipopts[i], options->length);
	if (len == 0) {
	    printf(" Bad length field for option %d\n", options->ipopts[i]);
	    break;
	}
	switch (options->ipopts[i]) {
	case IPOPT_END:
	    printf(" End of list");
	    break;
	case IPOPT_NOP:
	    printf(" Nop");
	    break;
	case IPOPT_RRT:
	    printf(" Record route:");
	    optok = print_ipoptroute((ipopt_routetype *)&options->ipopts[i]);
	    break;
	case IPOPT_BASSEC:
	    printf(" Basic security:");
	    optok = print_ipoptbassec((ipopt_bassectype *)&options->ipopts[i]);
	    break;
	case IPOPT_EXTSEC:
	    printf(" Extended security:");
	    optok = print_ipoptextsec((ipopt_extsectype *)&options->ipopts[i]);
	    break;
	case IPOPT_CIPSO:
	    printf(" Commercial security:");
	    optok = print_ipoptcipso((ipopt_cipsotype *)&options->ipopts[i]);
	    break;
	case IPOPT_LSR:
	    printf(" Loose source route:");
	    optok = print_ipoptroute((ipopt_routetype *)&options->ipopts[i]);
	    break;
	case IPOPT_SSR:
	    printf(" Strict source route:");
	    optok = print_ipoptroute((ipopt_routetype *)&options->ipopts[i]);
	    break;
	case IPOPT_TSTMP:
	    printf(" Timestamp:");
	    optok = print_ipopttstmp((ipopt_tstmptype *)&options->ipopts[i]);
	    break;
	case IPOPT_SID:
	    printf(" Stream ID:");
	    optok = print_ipoptsid((ipopt_sidtype *)&options->ipopts[i]);
	    break;
	case IPOPT_ALERT:
	    printf(" Router alert:");
	    optok = print_ipoptalert((ipopt_alerttype *)&options->ipopts[i]);
	    break;
	default:
	    printf(" Unknown option code %#x", options->ipopts[i]);
	    break;
	}
	printf("\n");
    }
}

/*
 * print_ipoptsid
 * Print Stream ID option
 */

boolean print_ipoptsid (ipopt_sidtype *ptr)
{
    if (ptr->length != IPOPT_SIDSIZE) {
	printf(" Bad length field");
	return(FALSE);
    }
    printf("%x", GETSHORT(ptr->streamid));
    return(TRUE);
}

/*
 * print_ipoptalert
 * Print router alert option
 */

boolean print_ipoptalert (ipopt_alerttype *ptr)
{
    printf("%x", GETLONG(ptr->value));
    return(TRUE);
}

/*
 * print_ipoptroute
 * Print out a route (source route or record route) from an IP option list
 */

boolean print_ipoptroute (ipopt_routetype *ptr)
{
    register int i;
    ipaddrtype host;
    int nhops, current;

    nhops = btol(ptr->length - IPOPT_ROUTEHEADERSIZE);
    current = btol(ptr->pointer - IPOPT_ROUTEHEADERSIZE);
    if ((nhops < 1) || (nhops > btol(MAXIPOPTIONBYTES))) {
	printf("\nBad length field %d", nhops);
	return(FALSE);
    }
    for (i = 0; i < nhops; i++) {
	if (i == 4)
	    printf("\n        ");
	if (i == current)
	    printf(" <*>");
	bcopy(ptr->hops[i], &host, sizeof(ipaddrtype));
	printf(" %i",n2hl(host));
    }
    if (current >= i)
	printf(" <*>");
   return(TRUE);
}

/*
 * ipopt_print_time
 *
 * Print out a time string given a pointer to a timestamp.
 */
static void ipopt_print_time (uchar *tsptr)
{
    ulong time, stripped_time;
    timeval tv;
    boolean valid;

    time = n2hl(GETLONG(tsptr));
    valid = ((time & ICMP_TIMESTAMPINVALID) == 0);
    stripped_time = time & (~ICMP_TIMESTAMPINVALID);
    clock_icmp_time_to_timeval(stripped_time, clock_timezone_offset(), &tv);
    printf("\n  Time=%c%02d:%02d:%02d.%03d %s (%08x)", (valid ? ' ' : '*'),
	   tv.hour, tv.minute, tv.second, tv.millisecond,
	   clock_timezone_name(), time);
}
    

/*
 * print_ipopttstmp
 * Print out timestamps from IP options
 */

boolean print_ipopttstmp (ipopt_tstmptype *ptr)
{
    register int i;
    ipaddrtype host;
    int current = (ptr->pointer - IPOPT_TSTMPHEADERSIZE) >> 2;
    int ntimes = (ptr->length - IPOPT_TSTMPHEADERSIZE) >> 2;

    printf(" Type %d.  Overflows: %d length %d, ptr %d",ptr->flg, ptr->oflw,
		ptr->length,ptr->pointer);
    if ((ntimes < 0)||(ntimes > btol(MAXIPHEADERBYTES))||(current > ntimes)) {
	printf(" Bad length field");
	return(FALSE);
    }
    switch (ptr->flg) {
	case IPOPT_TSTMP_TYPE0:		/* Timestamps only */
	    for (i = 0; i < current; i++) {
		ipopt_print_time(ptr->tsdata[i]);
	    }
	    printf("\n  >>Current pointer<<");
	    for (i = current; i < ntimes; i++) {
		ipopt_print_time(ptr->tsdata[i]);
	    }
	    break;
	case IPOPT_TSTMP_TYPE1:	/* internet addresses and time stamps */
	case IPOPT_TSTMP_TYPE2:
	    for (i = 0; i < current; i++) {
		bcopy(ptr->tsdata[i++], &host, sizeof(ipaddrtype));
		ipopt_print_time(ptr->tsdata[i]);
		printf(" at host %i", n2hl(host));
	    }
	    printf("\n  >>Current pointer");
	    for (i = current; i < ntimes; i++) {
		bcopy(ptr->tsdata[i++], &host, sizeof(ipaddrtype));
		ipopt_print_time(ptr->tsdata[i]);
		printf(" at host %i", n2hl(host));
	    }
	    break;
	default:
	    break;
    }
    return(TRUE);
}


/*
 * security_string
 * Return a string describing the security level.  Argument is an internal
 * security code.
 */

char *security_string (uchar security_code)
{
    switch (security_code) {
      case SECLVL_RESERVED4:	return("reserved_4");
      case SECLVL_TOPSECRET:	return("topsecret");
      case SECLVL_SECRET:	return("secret");
      case SECLVL_CONFIDENTIAL:	return("confidential");
      case SECLVL_RESERVED3:	return("reserved_3");
      case SECLVL_RESERVED2:	return("reserved_2");
      case SECLVL_UNCLASSIFIED:	return("unclassified");
      case SECLVL_RESERVED1:	return("reserved_1");
      case SECINSERT:		return("insert");
      default:			return("unknown");
      }
}

/*
 * authority_string
 * Fills a buffer in with the current human readable representation
 * of the interface's ipsecurity authority matrix.
 */

char matrix_buffer[MAX_AUTH_STRING];

char *authority_string (uchar *authptr)
{
    char *ptr = matrix_buffer;

    if (*authptr & AUTH_GENSER)
      ptr += sprintf(ptr, "genser ");
    if (*authptr & AUTH_SIOP_ESI)
      ptr += sprintf(ptr, "siop-esi ");
    if (*authptr & AUTH_SCI)
      ptr += sprintf(ptr, "sci ");
    if (*authptr & AUTH_NSA)
      ptr += sprintf(ptr, "nsa ");
    if (*authptr & AUTH_DOE)
      ptr += sprintf(ptr, "doe ");
    if (*authptr & ~AUTH_MASK)
      ptr += sprintf(ptr, "unknown ");
    *ptr = '\0';

    return(matrix_buffer);
}

/*
 * print_ipoptbassec
 * Print basic security option in English
 */

boolean print_ipoptbassec (ipopt_bassectype *ptr)
{
    int i = 0;

    if (ptr->length < 4) {
	printf(" Bad length field");
	return(FALSE);
    }

    printf(" Class: %s", security_string(map_security(ptr->classification)));
    printf("  Auth: ");
    if ((ptr->authority[0] & ~AUTH_MASK) == 0)
      printf("%s", authority_string(&ptr->authority[0]));
    else
      for (i = 3; i < ptr->length; i++)
	printf(" %#x", ptr->authority[i-3]);
    return(TRUE);
}

/*
 * print_ipoptextsec
 * Print extended IP security options
 */

boolean print_ipoptextsec (ipopt_extsectype *ptr)
{
    int i;

    if (ptr->length < 4) {
	printf(" Bad length field");
	return(FALSE);
    }

    printf(" Source: %#x, Info:", ptr->source);
    for (i = 0; i < (ptr->length - 3); i++)
	printf(" %#x", ptr->info[i]);
    return (TRUE);
}

/*
 * print_ipoptcipso
 * Print commercial IP security options
 */
boolean print_ipoptcipso (ipopt_cipsotype *ptr)
{
    int i;

    if (ptr->length < 4) {
	printf(" Bad length field");
	return (FALSE);
    }
    for (i = 0; i < (ptr->length - 3); i++)
	printf(" %#x", ptr->info[i]);

    return (TRUE);
}

/*
 * parse_ipoptroute
 * Parse an ipoption route.  That is, a list of hostnames/addresses.
 * Store it at PTR in appropriate form (with pointer and length).
 * DOES NOT fill in the actual option type.
 * Returns FALSE if there was an error.
 */

boolean parse_ipoptroute (ipoptiontype *ptr, parseinfo *csb)
{
    ipopt_routetype *optptr;
    int i;
    ipaddrtype  addr, last;
    addrtype    *addrp;

    optptr = (ipopt_routetype *) ptr->ipopts; /* where to put option */
    last = 0;
    addrp = GETOBJ(paddr,1);
    for (i = 0; i < ((MAXIPOPTIONBYTES - IPOPT_ROUTEHEADERSIZE) >> 2); i++) {
	if (!addrp || (addrp->type != ADDR_IP))
	    break;
	addr = n2hl(addrp->ip_addr);
	last = addr;
	bcopy(&addr, optptr->hops[i], sizeof(ipaddrtype));
	switch (i) {
	  case 0: addrp = GETOBJ(paddr,2); break;
	  case 1: addrp = GETOBJ(paddr,3); break;
	  case 2: addrp = GETOBJ(paddr,4); break;
	  case 3: addrp = GETOBJ(paddr,5); break;
	  case 4: addrp = GETOBJ(paddr,6); break;
	  case 5: addrp = GETOBJ(paddr,7); break;
	  case 6: addrp = GETOBJ(paddr,8); break;
	  default: addrp = NULL; break;
	}
    }
    ptr->length = optptr->length = (i << 2) + IPOPT_ROUTEHEADERSIZE;
    ptr->paddedlength = (ptr->length + 3) & (~3);
    optptr->pointer = 4;		/* point to first address */
    return(TRUE);
}

void interact_parse_ipoptroute (parseinfo *csb)
{
    parse_ipoptroute(csb->parse_line_pointer, csb);
    csb->exit_all = TRUE;
}

/*
 * parse_ipoptrecord
 * Parse a number and create a RECORD ROUTE IP option.
 * accepts a maximum number of bytes available for the IP header,
 * which is used to deafault the size if crlf is typed.
 */

boolean parse_ipoptrecord (ipoptiontype *ptr, char *buff, int maxbytes)
{
    ulong num;
    ipopt_routetype *optptr;
    boolean parsed;

    optptr = (ipopt_routetype *) ptr->ipopts;
    if (maxbytes > MAXIPOPTIONBYTES)
	maxbytes =  MAXIPOPTIONBYTES;
    num = parse_unsigned(buff, &parsed);
    if (!parsed) {
	num = (maxbytes - IPOPT_ROUTEHEADERSIZE) >> 2;
    }
    if ((!num) || (num > ((maxbytes - IPOPT_ROUTEHEADERSIZE) >> 2)))
	return(FALSE);

    memset(optptr->hops, 0, (num<<2)+1);
    ptr->length = optptr->length = (num << 2) + IPOPT_ROUTEHEADERSIZE;
    ptr->paddedlength = (ptr->length + 3) & (~3);
    optptr->pointer = 4;		/* point to first address */
    return(TRUE);
}

/*
 * parse_ipopttstmp
 * Parse an IP option timestamp specification
 */

boolean parse_ipopttstmp (ipoptiontype *ptr, char *buff, int maxbytes)
{
    ipopt_tstmptype *optptr;
    register int num;
    boolean parsed;

    optptr = (ipopt_tstmptype *) ptr->ipopts;
    if (maxbytes > MAXIPOPTIONBYTES)
	maxbytes =  MAXIPOPTIONBYTES;
    num = parse_unsigned(buff, &parsed);
    if (!parsed) {
	num = (maxbytes - IPOPT_TSTMPHEADERSIZE) >> 2;
    }
    if (num > ((maxbytes - IPOPT_TSTMPHEADERSIZE) >> 2))
	return(FALSE);
    memset(optptr->tsdata, 0, (num<<2));
    ptr->length = optptr->length = (num << 2) + IPOPT_TSTMPHEADERSIZE;
    ptr->paddedlength = (ptr->length + 3) & (~3);
    optptr->pointer = 5;		/* point to first data */
    optptr->oflw = 0;
    optptr->flg = 0;
    return(TRUE);
}

/*
 * ipopt_findopt
 * Scan IP options starting at ptr, looking for the option given.
 */

ipopt_routetype *ipopt_findopt (uchar *ptr, int opt, int length)
{
    int len;
    
    while (length > 0) {

	if (*ptr == opt)
	    return((ipopt_routetype *)ptr);

	len = ip_option_len(ptr, length);
	if (len == 0)
	    break;

	length -= len;
	ptr    += len;
    }
    return NULL;
}

/*
 * ipopt_findoptpak
 * Jacket for ipopt_findopt
 */

static ipopt_routetype *ipopt_findoptpak (uchar *iphdr, int opt)
{
    return(ipopt_findopt(iphdr + MINIPHEADERBYTES, opt,
			 ipheadsize((iphdrtype *)iphdr) - MINIPHEADERBYTES));
}

/*
 * map_security:
 * map the IP Security Option "Hamming" code into an internal code.
 */

int map_security (int hamming_code)
{
    switch (hamming_code) {
    case SECINSERT:    return (SECINSERT);
    case TOPSECRET:    return (SECLVL_TOPSECRET);
    case SECRET:       return (SECLVL_SECRET);
    case CONFIDENTIAL: return (SECLVL_CONFIDENTIAL);
    case UNCLASSIFIED: return (SECLVL_UNCLASSIFIED);
    case RESERVED4:    return (SECLVL_RESERVED4);
    case RESERVED3:    return (SECLVL_RESERVED3);
    case RESERVED2:    return (SECLVL_RESERVED2);
    case RESERVED1:    return (SECLVL_RESERVED1);
    default:	       return (SECNONE);
    }
}

/*
 * inv_map_security
 * Convert internal security level to option "Hamming" code version.
 */

int inv_map_security (int internal_code)
{
    switch (internal_code) {
    case SECLVL_TOPSECRET:	return (TOPSECRET);
    case SECLVL_SECRET:		return (SECRET);
    case SECLVL_CONFIDENTIAL:	return (CONFIDENTIAL);
    case SECLVL_UNCLASSIFIED:	return (UNCLASSIFIED);
    case SECLVL_RESERVED4:	return (RESERVED4);
    case SECLVL_RESERVED3:	return (RESERVED3);
    case SECLVL_RESERVED2:	return (RESERVED2);
    case SECLVL_RESERVED1:	return (RESERVED1);
    default:			return (UNCLASSIFIED);
    }
}

/*
 * abort_opt:  abort a packet that has option errors.
 *
 * 1) spit out an appropriate message if the needed debugging level
 *    is turned on.
 * 2) send out an ICMP parameter message pointing at the option that
 *    is totally hozed.
 * 3) bump the traffic cell for ip options.
 *
 * This routine is intended to only be called from ipopt_inproc.
 */

void abort_opt (
    paktype *pak,			/* the bozoid packet */
    uchar *opts,			/* the pntr to the option bytes */
    char *msg)				/* message to spew */
{
    if (ippkt_debug)
	ipbug(pak, msg);
    send_parameter(pak, ICMP_PARAMPROB,
		   (uint) (opts - ipheadstart(pak)));
    ip_traffic.badoption++;
}

/*
 * abort_for_security
 *
 * Abort a packet that has option errors, but do it at a specific security
 * classification level and authority.  Also overwrites the packet with ones
 * for those who are paranoid about these things.
 */
void abort_for_security (
    paktype *pak,			/* the bozoid packet */
    uint type,				/* response type or option pointer */
    uchar class,			/* internal classification lavel */
    uchar auth,				/* internal authority label */
    char *msg)				/* message to spew */
{
    iphdrtype *ip;

    ip = (iphdrtype *)ipheadstart(pak);

    if (ippkt_debug)
	ipbug(pak, msg);
    switch (type) {
      case SECRESP_BLACKHOLE:
	break;
      case SECRESP_PROHIB:
	send_unreachable2(pak, ICMP_UNRPROHIB, class, auth);
	break;
      case SECRESP_REQUIRED:
	send_parameter2(pak, ICMP_PARAMREQUIRED, IPOPT_BASSEC,
			class, auth);
	break;
      case SECRESP_REQNOROOM:
	send_parameter2(pak, ICMP_PARAMREQNOROOM, IPOPT_BASSEC,
			class, auth);
	break;
      default:
	send_parameter2(pak, ICMP_PARAMPROB, (type - (uint)ipheadstart(pak)),
			class, auth);
	break;
    }

    /*
     * Write the audit either as failure at send time or receive time,
     * but only if not as a result of a broadcast packet.
     */
    if ((pak->flags & PAK_BROADCAST) == 0 &&
	physical_broadcast(pak) != FUZ_TRUE) {
	if (pak->if_input == NULL)
	    reg_invoke_ip_send_audit(pak, DNSIX_SECERROR_SENDR);
	else
	    reg_invoke_ip_send_audit(pak, DNSIX_SECERROR_RECV);
    }
    /*
     * pak->datagramsize may not yet be initialized, so bone this packet
     * based upon the size of the IP packet.
     */
    memset(ipheadstart(pak), 0xff, ip->tl);
    ip_traffic.badoption++;
    ip_traffic.badsecure++;
    pak->flags |= PAK_ABORT;
}

/*
 * ip_eso_get_defaults
 *
 * Determine bit default and length for a particular NLESO source
 */
static int ip_eso_get_defaults (uchar source, uchar *bit_default)
{
    int max_bytes, i;
    
    *bit_default = 0;
    max_bytes = 0;
    for (i = 0; i < ip_sec_info_entries; i++) {
	if (ip_sec_info[i].source_id == source) {
	    max_bytes = ip_sec_info[i].max_bytes;
	    if (ip_sec_info[i].bit_default == 1)
		*bit_default = 255;
	    break;
	}
    }
    return(max_bytes);
}

/*
 * ip_eso_add_buf
 *
 * Take all the NLESOs defined for a particular interface, and attempt
 * to place maximal accrediation info for those NLESOs into the passed
 * in buffer.
 */
static int ip_eso_add_buf (idbtype *idb, uchar *op, int available)
{
    ip_sec_eso_source *eso;
    int optlen, extlen, pad, max_bytes, i;
    uchar bit_default;

    extlen = 0;
    for (eso = (ip_sec_eso_source *)idb->ip_sec_nleso.qhead; eso;
	 eso = eso->next) {
	if (eso->type != IP_SEC_ESO_MAX)
	    continue;
	max_bytes = ip_eso_get_defaults(eso->source_id, &bit_default);
	if (max_bytes == 0) {
	    errmsg(&msgsym(NOMAX, SEC));
	    continue;
	}
	optlen = max_bytes + 3;
	if ((extlen + optlen) > available) {
	    if (ipsecurity_debug)
		buginf("\nIP Security: No room for %d byte ESO option,"
		       " (%d available)",
		       optlen, available - extlen);
	    break;				/* No more room */
	}
	*op++ = IPOPT_EXTSEC;
	*op++ = optlen;
	*op++ = eso->source_id;
	bcopy(eso->compartment_info, op, eso->compartment_bytes);
	op += eso->compartment_bytes;
	/*
	 * Fill the compartment info so that all possible bytes of NLESO
	 * are transmitted.
	 */
	for (i = 0; i < max_bytes - eso->compartment_bytes; i++)
	    *op++ = bit_default;
	extlen += optlen;
    }
    pad = extlen % 4;
    if (pad) {
	for (pad = 4 - pad; pad; pad--) {
	    *op++ = PADDING;
	    extlen++;
	}
    }
    return(extlen);
}

/*
 * ip_add_eso_opt
 * Add extended security options to ipoptiontype structure.
 */

void ip_add_eso_opt (ipoptiontype *op, ipaddrtype addr)
{
    idbtype *idb;
    int len;

    /*
     * Need to know which interface we are going to route to in order to
     * decide what the ESO options should look like.
     */
    idb = reg_invoke_ip_first_hop(&addr, addr, IP_PATH_DEFFLAGS);
    if (!idb)
	return;
    len = ip_eso_add_buf(idb, &op->ipopts[op->length],
			 MAXIPHEADERBYTES - op->length);
    op->length += len;
    op->paddedlength += len;
}

/*
 * ip_add_eso_opt
 * Add basic security options to ipoptiontype structure.
 */

void ip_add_bso_opt (ipoptiontype *op)
{
    op->ipopts[op->length++] = IPOPT_BASSEC;
    op->ipopts[op->length++] = SECURITY_SIZE;
    op->ipopts[op->length++] = SECINSERT;
    op->ipopts[op->length++] = 0;
    op->paddedlength += SECURITY_SIZE;
}

/*
 * ip_bso_classify_pak
 *
 * Compute the definative (or is it devinative) security label for this
 * packet.  This is where the majority of the rules set forth in RFC1108
 * are checked.
 *
 * 1) How many security options were found?
 *
 *    0a  - internal (no idb), must be reused.  pak->class tells the story.
 *    0b  - Are we accepting packets without a label?
 *          Yes  -  Label at default for xfc.  Done.
 *          No   -  Nuke it.
 *    1   - Continue.
 *    >1  - Nuke it.
 *
 * 2) How does the packet classification match the interface?
 *
 *    Lower  - Nuke it.
 *    Same   - Continue.
 *    Higher - Nuke it.
 *    Unrecognizable - Nuke it.
 *
 * 3) Are we checking authority bits?
 *
 *    No  - Label with the given classification, with the DEFAULT
 *          set of authority bits. Done.
 *    Yes - Continue.
 *
 * 4) Do the authority bits fall within the range allowed?
 *
 *    No  - Nuke it.
 *    Yes - Label at given classification and authority.  Done.
 *
 * Responses to nuked packets are computed as follows.
 *
 * 0) Oddball cases like none or too many options are treated as
 *    Unclassified with the default authority bits.
 * 1) Classification too low - no response.
 * 2) Classification too high - response at highest classification of
 *    xfc. Use all common authority bits.
 * 3) Not enough authority bits - no response.
 * 4) Too many authority bits - respond with all common authorities.
 */
static boolean ip_bso_classify_pak (idbtype *idb, paktype *pak, int basseen,
				    ipopt_bassectype *opt)
{
    iphdrtype *ip;
    uchar level, authority;
    
    ip = (iphdrtype *)ipheadstart(pak);

    if (opt) {
	/*
	 * Assume that the label is correct.  (We'll soon find out.)
	 */
	level = map_security(opt->classification);
	authority = opt->authority[0];
    } else {
	level = idb->ip_sec_def_class;
	authority = idb->ip_sec_def_auth;
    }

    if (ipsecurity_debug) {
	buginf("\nIP Security: src %i dst %i, number of BSO %d", ip->srcadr,
	       ip->dstadr, basseen);
	if (idb) {
	    buginf("\n     idb: %s (%#x) %#x to %s (%#x) %#x, %simplicit",
		   security_string(idb->ip_sec_low),
		   idb->ip_sec_low, idb->ip_sec_auth_low,
		   security_string(idb->ip_sec_high),
		   idb->ip_sec_high, idb->ip_sec_auth_high,
		   idb->ip_sec_implicit ? "" : "no ");
	    buginf("\n          def %s (%#x) %#x",
		   security_string(idb->ip_sec_def_class),
		   idb->ip_sec_def_class, idb->ip_sec_def_auth);
	} else
	    buginf("\n     idb: NULL");
	if (opt)
	    buginf("\n     pak: %s (%#x) %#x", security_string(level),
		   opt->classification, authority);
    }

    if (basseen == 0) {
	/*
	 * NO SECURITY OPTION in incoming packet.
	 * If we already striped security option, then no option is OK.
	 */
	if (pak->ip_flags & PAKOPT_SECSTRIP)
	    return(TRUE);
	if (!idb) {
	    /*
	     * Is this a reused packet?  (E.G. ICMP Mask Reply) When the
	     * system is secured, it always generates real internal
	     * packets with a security option.  To be here and be
	     * internal (idb NULL and no basic security option) this must
	     * be a reused packet.  It is now internal but the incoming
	     * didn't have a security option so the packet doesn't
	     * either.  But note that on the way in the packet was
	     * labelled and passed security or we wouldn't be seeing it
	     * here.  The incoming interface must have been labelled
	     * IMPLICIT.  Look for the old classification as a sanity
	     * check.  We could change this to a SECINSERT (as it should
	     * have come from ICMP), but it makes more sense to leave the
	     * classification.  This prevents things like a mask request
	     * coming in on a Secret interface, and being responded to on
	     * an Unclassified interface.
	     */
	    if ((pak->classification == SECNONE) ||
		(pak->authority == AUTH_NONE)) {
		errmsg(&msgsym(NOOPT, SEC));
		return(FALSE);
	    }
	    return(TRUE);
	}

	/*
	 * If we are doing implicit labelling, then no option is OK.
	 */
	if (idb->ip_sec_implicit) {
	    pak->classification = idb->ip_sec_def_class;
	    pak->authority = idb->ip_sec_def_auth;
	    return(TRUE);
	}

	/*
	 * No label, and we are not implicit.  Consider it unclassified, and
	 * reject it.  We may not be allowed to reply.
	 */
	if (inclass(idb, SECLVL_UNCLASSIFIED) && (idb->ip_sec_auth_low))
	    abort_for_security(pak, SECRESP_REQUIRED, SECLVL_UNCLASSIFIED,
			       idb->ip_sec_auth_low, "no basic");
	else
	    abort_for_security(pak, SECRESP_BLACKHOLE, 0, 0,
			       "no basic, no resp");
	return(FALSE);
    }

    else if (basseen == 1) {
	/*
	 * ONE SECURITY OPTION.  Check the classification.
	 */
	switch (opt->classification) {
	  case RESERVED4:
	  case RESERVED3:
	  case RESERVED2:
	  case RESERVED1:
	    if (idb && !idb->ip_sec_allow_rsrvd) {
		/*
		 * Unrecognizable label.  Treat as Unclassified.
		 */
		if (inclass(idb, SECLVL_UNCLASSIFIED) &&
		    (idb->ip_sec_auth_low))
		    abort_for_security(pak, (uint)&opt->classification,
				       SECLVL_UNCLASSIFIED,
				       idb->ip_sec_auth_low,
				       "reserved class");
		else
		    abort_for_security(pak, SECRESP_BLACKHOLE, 0, 0,
				       "reserved class, no resp");
		return(FALSE);
	    } /* else we are allowing the reserved fields, so fall through */
	  case TOPSECRET:
	  case SECRET:
	  case CONFIDENTIAL:
	  case UNCLASSIFIED:
	    /*
	     * Must allow internal packets to have a security label so
	     * that we may send ICMP messages at a specific level.  These
	     * may be either newly generated or reused packets, so we
	     * must label (or relabel) them in this routine, otherwise
	     * postroutine breaks.
	     */
	    if (!idb)
	      break;

	    if (level < idb->ip_sec_low) {
		/*
		 * Packet labelled too low.  No response permitted.
		 */
		abort_for_security(pak, SECRESP_BLACKHOLE, 0, 0,
				   "class too low, no resp");
		return(FALSE);
	    }
	    if (level > idb->ip_sec_high) {
		/*
		 * Packet too high.  Send error at xfc high.
		 */
		if (inauthl(idb, authority))
		  abort_for_security(pak, (uint)&opt->classification, 
				     idb->ip_sec_high,
				     authority & idb->ip_sec_auth_high,
				     "class too high");
		else
		  abort_for_security(pak, SECRESP_BLACKHOLE, 0, 0,
				     "class too high, bad auths, no resp");
		return(FALSE);
	    }
	    break;
	    
	  case SECINSERT:
	    /*
	     * okay to insert only if internally generated.
	     */
	    if (!idb) {
		pak->classification = SECINSERT;
		return(TRUE);
	    }
	    /* Fall through */

	  default:
	    /*
	     * Unrecognizable label.  Treat as Unclassified.
	     */
	    if (inclass(idb, SECLVL_UNCLASSIFIED) && (idb->ip_sec_auth_low))
	      abort_for_security(pak, (uint)&opt->classification,
				 SECLVL_UNCLASSIFIED,
				 idb->ip_sec_auth_low, "unrecognized class");
	    else
	      abort_for_security(pak, SECRESP_BLACKHOLE, 0, 0,
				 "unrecognized class, no resp");
	    return(FALSE);
	}
    }

    else {
	/*
	 * MORE THAN ONE security option in incoming packet.  Consider it to be
	 * Unclassified, and reject it.  No response to internal packets, or
	 * those with no low authority bits defined.
	 */
	if (!idb)
	    errmsg(&msgsym(TOOMANY, SEC));
	else if (inclass(idb, SECLVL_UNCLASSIFIED) && (idb->ip_sec_auth_low))
	    abort_for_security(pak, (uint)opt, SECLVL_UNCLASSIFIED,
			       idb->ip_sec_auth_low, "multiple basic");
	else
	    abort_for_security(pak, SECRESP_BLACKHOLE, 0, 0,
			       "multiple basic, no resp");
	return(FALSE);
    }
      
    /*
     * Now (maybe) check authority bits
     */
    if (idb) {
	if (!(idb->ip_sec_instyle & SECSTYLE_IGNOREAUTH)) {
	    if (ipsecurity_debug)
		buginf("\nIP Security: checking BSO %#x against [%#x %#x]",
		       authority, idb->ip_sec_auth_low,
		       idb->ip_sec_auth_high);
	    if ((authority == AUTH_NONE) || !inauthl(idb, authority)) {
		/*
		 * Not enough authories present.  Cannot process or complain.
		 */
		abort_for_security(pak, SECRESP_BLACKHOLE, 0, 0,
				   "auth too low, no resp");
		return(FALSE);
	    }
	
	    if (!inauthh(idb, authority)) {
		/*
		 * Too many authorities.  Complain using common auths.
		 */
		abort_for_security(pak, (uint)&opt->authority[0],
				   map_security(opt->classification),
				   authority & idb->ip_sec_auth_high,
				   "auth too high");
		return(FALSE);
	    }
	}
	else
	  authority = idb->ip_sec_def_auth;
    }

    /*
     * now we can definatively label the packet.
     */
    if (ipsecurity_debug)
	buginf("\nIP Security: classified BSO as %s (%#x) %#x",
	       security_string(level), opt->classification, authority);
    pak->classification = level;
    pak->authority = authority;
    return(TRUE);
}

/*
 * ip_eso_valid_max
 * Determine if new length for compartment bytes is sufficiently long
 * enough given the compartment data which may already be configured on
 * some interfaces.
 */

boolean ip_eso_valid_max (ushort max)
{
    idbtype *idb;
    ip_sec_eso_source *eso;

    FOR_ALL_SWIDBS(idb)
	for (eso = (ip_sec_eso_source *)idb->ip_sec_nleso.qhead; eso;
	     eso = eso->next) {
	    if (eso->compartment_bytes > max)
		return(FALSE);
	}
    return(TRUE);
}

/*
 * ip_eso_disable
 * Deconfigure IP extended security info for a particular interface.
 */

void ip_eso_disable (idbtype *idb)
{
    ip_sec_eso_source *p;

    while ((p = dequeue(&idb->ip_sec_nleso)))
	free(p);
    while ((p = dequeue(&idb->ip_sec_aeso)))
	free(p);
}

/*
 * ip_eso_remove_source
 * Remove any knowledge of a particular IP extended security source from
 * all interfaces.
 */

void ip_eso_remove_source (uchar source)
{
    idbtype *idb;
    ip_sec_eso_source *eso, *next;

    FOR_ALL_SWIDBS(idb)
	for (eso = (ip_sec_eso_source *)idb->ip_sec_nleso.qhead; eso;
	     eso = next) {
	    next = eso->next;
	    if (eso->source_id == source) {
		p_unqueue(&idb->ip_sec_nleso, eso);
		free(eso);
	    }
	}
}

/*
 * ip_eso_dominate
 * Return true if min is "dominated by" max.
 */
static inline boolean ip_eso_dominate (uchar min, uchar max)
{
    return((min | max) == max);
}

/*
 * ip_eso_dom_check
 * Perform extended security "domination" checks, as per DNSIX spec.
 */

static boolean ip_eso_dom_check (paktype *pak, uchar *opt,
				 uchar *opt_data, int opt_bytes,
				 ip_sec_eso_source *max_acc,
				 ip_sec_eso_source *min_acc)
{
    int max_bytes, i;
    uchar bit_default, *cp;
    int max_table_bytes, min_table_bytes;
    uchar *max_table_data, *min_table_data, source;

    max_table_data = max_acc->compartment_info;
    max_table_bytes = max_acc->compartment_bytes;
    if (min_acc) {
	min_table_data = min_acc->compartment_info;
	min_table_bytes = min_acc->compartment_bytes;
    } else {
	min_table_data = NULL;
	min_table_bytes = 0;
    }
    source = max_acc->source_id;
    max_bytes = ip_eso_get_defaults(source, &bit_default);
    if (max_bytes == 0)
	return(FALSE);
    /*
     * Calculate and compare against default any extra bits in packet's
     * option field
     */
    if (opt_bytes > max_bytes) {
	for (cp = &opt_data[max_bytes]; cp < &opt_data[opt_bytes]; cp++) {
	    if (*cp != bit_default) {
		return(FALSE);
	    }
	}
    }
    /*
     * Fill unspecified table data with default for comparison purposes
     */
    if (max_table_bytes < max_bytes) {
	for (i = max_table_bytes; i < max_bytes; i++)
	    max_table_data[i] = bit_default;
    }
    if (min_acc) {
	if (min_table_bytes < max_bytes) {
	    for (i = min_table_bytes; i < max_bytes; i++)
		min_table_data[i] = bit_default;
	}
    }
    /*
     * Now make sure bits in ESO are "dominated by" (sic) info from
     * accreditation range table
     */
    for (i = 0; i < min(opt_bytes, max_bytes); i++) {
	if (!ip_eso_dominate(opt_data[i], max_table_data[i])) {
	    abort_for_security(pak, (uint)opt, pak->classification,
			       pak->authority,
			       "Compartment bits not dominated by max"
			       " sensitivity level");
	    return(FALSE);
	}
	if (!min_acc)			/* Need to check min sensitivity? */
	    continue;
	if (ip_eso_dominate(min_table_data[i], opt_data[i]))
	    continue;
	abort_for_security(pak, (uint)opt, pak->classification,
			   pak->authority,
			   "Compartment bits don't dominate min"
			   " sensitivity level");
	return(FALSE);
    }
    return(TRUE);
}

/*
 * ip_eso_classify_pak
 * Process extended IP security option(s) found in packet.
 * Returns FALSE if packet somehow didn't pass security checks.
 */

static boolean ip_eso_classify_pak (idbtype *idb, paktype *pak, uchar *opts,
				    int optleft)
{
    ipopt_extsectype *eopt;
    ip_sec_eso_source *aeso, *nleso;
    uchar *endp;
    boolean found_source, accred_match[MAX_NLESO];
    int nlidx;
    ushort len;
    
    if (!idb)
	return(TRUE);
    if (ipsecurity_debug)
	buginf("\nIP Security: ESO Source %d data %#x (%d) interface %s",
	       *(opts+2), *(opts+3), *(opts+1), idb->namestring);
    switch (idb->ip_sec_extended) {
    case IP_ESO_DENY:
	/*
	 * Extended not allowed
	 */
	abort_for_security(pak, (uint)opts, pak->classification,
			   pak->authority,
			   "Security failure: extended security disallowed");
	return(FALSE);
	break;
    case IP_ESO_IGNORE:
	return(TRUE);
	break;
    default:
	break;
    }
    memset(accred_match, 0, sizeof(boolean) * MAX_NLESO);
    for (endp = &opts[optleft]; opts < endp; opts += len) {
	len = ip_option_len(opts, endp - opts);
	if (len == 0) {
	    abort_for_security(pak, (uint)opts, pak->classification,
			       pak->authority,
			       "Security failure: invalid option length");
	    return(FALSE);
	}
	if (*opts != IPOPT_EXTSEC)
	    continue;		/* Skip non extended IPSO options */
	/*
	 * Process extended IPSO fields
	 */
	eopt = (ipopt_extsectype *)opts;
	found_source = FALSE;
	for (aeso = (ip_sec_eso_source *)idb->ip_sec_aeso.qhead;
	     aeso; aeso = aeso->next) {
	    if (aeso->source_id != eopt->source)
		continue;
	    /*
	     * No need to do further processing on AESO
	     */
	    found_source = TRUE;
	}
	if (found_source)
	    continue;
	for (nlidx = 0, nleso = (ip_sec_eso_source *)idb->ip_sec_nleso.qhead;
	     nleso; nleso = nleso->next, nlidx++) {
	    if (nleso->source_id != eopt->source)
		continue;
	    found_source = TRUE;
	    /*
	     * Find max sensitivity level accreditation information
	     */
	    if (nleso->type != IP_SEC_ESO_MAX)
		continue;
	    if (accred_match[nlidx]) {
		/*
		 * Already matched
		 */
		abort_for_security(pak, (uint)opts, pak->classification,
				   pak->authority,
				   "NLESO source appeared twice");
		return(FALSE);
	    }
	    if (!ip_eso_dom_check(pak, opts, eopt->info, eopt->length - 3,
				  nleso, NULL))
		return(FALSE);
	    accred_match[nlidx] = TRUE;
	}
	if (!found_source) {
	    if (ipsecurity_debug)
		buginf("\nIP Security: No max accred. info for ESO source %d",
		       eopt->source);
	    abort_for_security(pak, (uint)opts, pak->classification,
			       pak->authority, "ESO source not found");
	    return(FALSE);
	}
    }
    return(TRUE);
}

/*
 * ipopt_inproc: scan ip option fields on input of a packet to
 * 	the system.  It handles any processing needed prior to
 * 	making any routing decisions.  It does not change option
 * 	state.
 *
 * Assumptions:
 *   1) when a packet is first allocated pak->desthost and approriate
 * 	pak->flags have been cleared.  Currently this happens
 *      when the packet is first malloc'd during system initilization
 *      or when a packet is returned to the free pool via ret_pool.
 *
 * 	Users that reuse a packet are responsible to ensure that
 * 	these conditions are maintained.  If you use ipwrite this
 * 	is handled there.
 *
 * It is okay to call this routine without any options.  We will see
 * that there are none and not do anything.
 *
 * This function does the following things:
 *
 * 1) Scans across all options looking for errors.  For each option
 *    that is found an appropriate flag is set that denotes that
 *    such an option has been seen.  These flags are in the packet
 *    header.  (pak->ip_flags)  The bit positions are denoted
 *    by PAKOPT_HAS defines.
 * 2) Looks for source routing and returns the host address of the
 *    next hop.  This cell is desthost in the packet header. (pak->desthost)
 * 3) Checks for the security level of the packet and compares it
 *    against the configured level of the incoming interface.  If
 *    inappropriate, the packet will be blown off.
 * 4) Duplicated option fields are checked for and if found an
 *    appropriate error message is generated.  The only options
 *    allowed to be duplicated are END, NOP, and EXTENDED SECURITY.
 * 5) Checks that EXTENDED SECURITY is allowed on this interface. If not
 *    then the packet is nuked.
 *
 * Errors that are detected will result in an appropriate ICMP
 * message being generated to the originator of the packet and
 * the packet nailed to the wall (dropped).  In the event that the
 * packet being checked is from ourselves we will still generate
 * the appropriate ICMP packet.
 *
 * This routine should not cause datagram state changes to occur.
 * Any changes will be handled by ipopt_postroute (formerly
 * ipopt_outproc).  This kind of activity needs to happen
 * after routing decisions have been made.
 *
 * ipopt_inproc will return TRUE if the options were processed
 * without error, FALSE otherwise.
 *
 * The special security code SECINSERT is ignored only if the packet
 * is really internal.  SLIP packets are always nuked when the box is in
 * secured mode.
 */

boolean ipopt_inproc (paktype *pak)
{
    register uchar *opts;
    register int noptions;
    register ulong flags = 0;
    register idbtype *idb;
    iphdrtype *ip;
    ipopt_routetype *route_opts;
    boolean saw_ip_srcroute_option;
    int bassec_seen, extsec_seen, extsec_optleft;
    uchar    *extsec_location;
    ipopt_bassectype *user_sec = NULL;
    ushort len;

    ip = (iphdrtype *)ipheadstart(pak);

    if (pak->ip_flags & PAKOPT_SECADD) {
	/*
	 * If we are responsible for having added the security info
	 * to this packet, then we must not check the security against
	 * the input interface (as the security was added based upon the
	 * output interface).  We should only get here if we have requeued
	 * a packet after adding security info during the postroute procedure.
	 * Such a packet has already been processed by this routine before
	 * being requeued.
	 */
	if (ipsecurity_debug)
	    buginf("\nIP Security: Skipping redundant security check on"
		   " requeued packet.");
	return(TRUE);
    }
    bassec_seen = extsec_seen = extsec_optleft = 0;
    extsec_location = NULL;
    saw_ip_srcroute_option = FALSE;
    noptions = (ip->ihl << 2) - MINIPHEADERBYTES;
#ifdef DEBUG_OPTS
    if (ippkt_debug && ippkt_check(pak, TRUE, ipdebug_acl))
	buginf("\nIPOPT_IN:  s %i d %i dh %i xfc 0x%x ob %d",
	     n2hl(ip->srcadr), n2hl(ip->dstadr), pak->desthost,
	     pak->if_input, noptions);
#endif
    pak->ip_flags = 0;			/* clear what is there */
    pak->desthost = (ipaddrtype) 0;	/* clear out fake source route */

    /*
     *  If we don't have options don't do anything.
     */
    idb = pak->if_input;
    if (!noptions) {
	if (!ip_secopt_bytes)
	  return(TRUE);
	else
	  goto finished_options;
    }
    ip_traffic.options++;

    opts = (uchar *)ip + MINIPHEADERBYTES;
    for (; noptions > 0; noptions -= len, opts += len) {
      len = ip_option_len(opts, noptions);
      if (len == 0) {
	  abort_opt(pak, opts, "Invalid option length");
	  return(FALSE);
      }
      switch (*opts) {
	case IPOPT_BASSEC:
	    /*
	     * Remember first or second basic security option.
	     * First is for us
	     * to use, second is for complaining to the sender.
	     */
	    if (++bassec_seen <= 2)
	      user_sec = (ipopt_bassectype *) (opts);
	    flags |= PAKOPT_HASSEC;
	    ip_traffic.opt_bassec++;
	    break;
	    
	case IPOPT_EXTSEC:
	    /*
	     * These may or may not be nailed, its a local option.  We
	     * must wait until all options have been parsed so we also
	     * know whether there was a basic security option.
	     */
	    if (!extsec_seen) {
		extsec_location = opts;   /* remember first occurrance */
		extsec_optleft = noptions;
		flags |= PAKOPT_HASSEC;
		extsec_seen = TRUE;
	    }
	    ip_traffic.opt_extsec++;
	    break;

        case IPOPT_CIPSO:
	    ip_traffic.opt_cipso++;

	    if (idb->ip_sec_cipso == IP_CIPSO_DENY) {
		abort_opt(pak, opts, "Commercial security option present");
		return(FALSE);
	    }
	    flags |= PAKOPT_HASCIPSO;
	    break;

	case IPOPT_LSR:		/* loose source route */
	case IPOPT_SSR:		/* strict source route */
	    if (flags & (PAKOPT_HASSSR | PAKOPT_HASLSR)) {
		abort_opt(pak, opts, "Duplicate source route option");
		return(FALSE);
	    }
	    /*
	     * Do a few sanity checks on the routing field.
	     */
	    route_opts = (ipopt_routetype *) opts;
	    if ((route_opts->pointer < IPOPT_ROUTEMINPTR) ||
		((route_opts->pointer & 3) != 0) ||
		(route_opts->pointer > MAXIPOPTIONBYTES)) {
		abort_opt(pak, opts, "Bad pointer in source route");
		return(FALSE);
	    }
	    if ((route_opts->length < IPOPT_ROUTEMINLEN)) {
		abort_opt(pak, opts, "Bad length in source route");
		return(FALSE);
	    }
	    saw_ip_srcroute_option = TRUE;
	    pak->desthost = ipopt_srin(pak, route_opts);
	    if (pak->desthost)
		flags |= ((*opts == IPOPT_SSR) ? PAKOPT_HASSSR
			  : PAKOPT_HASLSR);
	    if (*opts == IPOPT_SSR)
		ip_traffic.opt_ssr++;
	    else if (*opts == IPOPT_LSR)
		ip_traffic.opt_lsr++;
	    break;
	    
	case IPOPT_RRT:
	    if (flags & PAKOPT_HASRRT) {
		abort_opt(pak, opts, "Duplicate record route option");
		return(FALSE);
	    }
	    flags |= PAKOPT_HASRRT;
	    ip_traffic.opt_rrt++;
	    break;
	    
	case IPOPT_SID:
	    if (flags & PAKOPT_HASSID) {
		abort_opt(pak, opts, "Duplicate SID option");
		return(FALSE);
	    }
	    flags |= PAKOPT_HASSID;
	    ip_traffic.opt_sid++;
	    break;
	    
	case IPOPT_TSTMP:
	    if (flags & PAKOPT_HASTSTMP) {
		abort_opt(pak, opts, "Duplicate TSTMP option");
		return(FALSE);
	    }
	    flags |= PAKOPT_HASTSTMP;
	    ip_traffic.opt_tstmp++;
	    break;
	    
	case IPOPT_ALERT:
	    flags |= PAKOPT_HASALERT;
	    ip_traffic.opt_alert++;
	    break;

	case IPOPT_NOP:
	    ip_traffic.opt_nop++;
	    break;

	case IPOPT_END:
	    noptions = 1;		/* len = 1, so this ends the loop */
	    ip_traffic.opt_end++;
	    break;
	    
	default:
	    /*
	     * Unknown option type.  Ignore the option and continue
	     * further option processing.  Do not send an ICMP
	     * message to the originator.  Do not return FALSE since
	     * this will throw the packet away and we want to be as
	     * receptive as possible.
	     */
	    if (ippkt_debug)
		ipbug(pak, "unknown option");
	    ip_traffic.opt_unk++;
	    break;
	} /* switch (*opts) */
    }

    if (noptions < 0) {
	/*
	 * The option lengths were hozed.  This means the basic
	 * packet can't be trusted.  Throw the packet away
	 */
	if (ippkt_debug)
	   ipbug(pak, "format error due to options");
	ip_traffic.formaterr++;
	return(FALSE);
      }

finished_options:

#ifdef DEBUG_OPTS
    if (ippkt_debug || ipsecurity_debug)
        buginf("\nIPOPT_IN: at finished_options, flags: %#x", flags);
#endif

    if (ip_secopt_bytes || bassec_seen)
	/*
	 *  Now call the routine to classify this packet.  We also check
	 *  on whether extended security options are legal.
	 */
	if (!ip_bso_classify_pak(idb, pak, bassec_seen, user_sec))
	    return(FALSE);

    if (extsec_seen) {
	if (!ip_eso_classify_pak(idb, pak, extsec_location, extsec_optleft))
	    return(FALSE);
    }
    /*
     * Do we have a source routes are they are turned off?  Abort the option
     * with a source route failed.  This message is output even if
     * icmp messages are throttled back.
     */
    if (!ip_srcroute && saw_ip_srcroute_option) {
	if (ippkt_debug)
	    ipbug(pak, "rejected source route");
	send_unreachable(pak,ICMP_UNRSRFAIL);
	ip_traffic.badoption++;
	pak->flags |= PAK_ABORT;
	return(FALSE);
    }
    pak->ip_flags |= flags;		/* remember permanently */
    return(TRUE);
}

/*
 * ipopt_srin
 * Determine whether an IP source route in this packet should be used
 * or not (whether we are the destination, or original source).  If not,
 * just return the ordinary destination address of the packet.
 *
 * Will also update the bit flags PAKOPT_SRNOTEND if the source route
 * we are processing is not at its end.  We assume that a higher level
 * has done sanity checks on the routing field.
 */

ipaddrtype ipopt_srin (paktype *pak, ipopt_routetype *ptr)
{
    iphdrtype *ip;
    ipaddrtype host = 0L;
    
    ip = (iphdrtype *)ipheadstart(pak);
    if (!pak->if_input) {	/* packet originating here ? */
	if ((int)(ptr->length - ptr->pointer) >= 3) {
	    bcopy((uchar *)ptr + ptr->pointer - 1, &host, 4);
	    host = n2hl(host);
	}
    } else {
	/*
	 * Is the packet aimed at one of our interfaces.  We cannot
	 * simply use ip_forus here, because (in this case) a SLIP
	 * address really isn't for us.  We shouldn't touch the route,
	 * but let the SLIP host handle it.
	 */

	if (ip_ouraddress(ip->dstadr)) {
	    if ((int)(ptr->length - ptr->pointer) >= 3) {
		bcopy((uchar *)ptr + ptr->pointer - 1, &host, 4);
		host = n2hl(host);		
		pak->flags |= PAK_NOREDIRECT;
	    }
	}
    }
    /*
     * Check to see if we are off the end of the source route
     * If not set PAKOPT_SRNOTEND to tell others.
     */
    if (ptr->pointer <= ptr->length)
	pak->ip_flags |= PAKOPT_SRNOTEND;
    return(host);
}	

/*
 * ip_eso_postroute
 * Post-routing extended IP security processing.
 */

static boolean ip_eso_postroute (paktype *pak, uchar *opts)
{
    idbtype *idb;
    ipopt_extsectype *eopt;
    int nlidx;
    ip_sec_eso_source *eso, *max, *min;

    idb = pak->if_output;
    /*
     * Do we even want to check the ESO?
     */
    if (idb->ip_sec_extended == IP_ESO_IGNORE)
	return(TRUE);
    eopt = (ipopt_extsectype *)opts;
    for (eso = (ip_sec_eso_source *)idb->ip_sec_aeso.qhead;
	 eso; eso = eso->next) {
	if (eso->source_id != eopt->source)
	    continue;
	/*
	 * No need to do further processing on AESO
	 */
	return(TRUE);
    }
    max = min = NULL;
    for (nlidx = 0, eso = (ip_sec_eso_source *)idb->ip_sec_nleso.qhead;
	 eso; eso = eso->next, nlidx++) {
	if (eso->source_id != eopt->source)
	    continue;
	/*
	 * Find max/min sensitivity level accreditation information
	 */
	if (eso->type == IP_SEC_ESO_MAX) {
	    max = eso;
	    if (min)
		break;
	} else {
	    min = eso;
	    if (max)
		break;
	}
    }
    if (!max) {
	abort_for_security(pak, (uint)opts, pak->classification,
			   pak->authority, "ESO source not found");
	return(FALSE);
    }
    if (!ip_eso_dom_check(pak, opts, eopt->info, eopt->length - 3,
			  max, min))
	return(FALSE);
    return(TRUE);
}

/*
 * We know the actual security label at this point, as we put it
 * into the packet structure.  Here we must rewrite the label in
 * the packet, as we may have changed it. (e.g. it was labelled
 * U-NSA, but came in on a dedicated U-Genser interface where we
 * were ignoring the authority bits.  We should send it out as
 * U-Genser.
 *
 * ipopt_inproc and ip_bso_classify_pak have guaranteed us that
 * there will only be one basic security option.
 */

static boolean ip_bso_postroute (paktype *pak, uchar *opt,
				 boolean *state_change)
{
    idbtype *idb;
    ipopt_bassectype *user_sec;
    iphdrtype *ip;

    if (!ip_secopt_bytes)
	return(TRUE);

    ip = (iphdrtype *)ipheadstart(pak);
    idb = pak->if_output;
    user_sec = (ipopt_bassectype *)opt;
    switch (pak->classification) {
    case SECINSERT:
	/*
	 *  Set this packet to the high level of the interface.
	 */
	if (ipsecurity_debug)
	    buginf("\nIP Security: BSO postroute: SECINSERT changed"
		   " to %s (%#x) %#x",
		   security_string(idb->ip_sec_high),
		   inv_map_security(idb->ip_sec_high), 
		   idb->ip_sec_auth_high);
	*state_change = TRUE;
	user_sec->classification = inv_map_security(idb->ip_sec_high);
	user_sec->authority[0] = idb->ip_sec_auth_high;
	break;
	    
    case SECLVL_RESERVED4:
    case SECLVL_TOPSECRET:
    case SECLVL_SECRET:
    case SECLVL_CONFIDENTIAL:
    case SECLVL_RESERVED3:
    case SECLVL_RESERVED2:
    case SECLVL_UNCLASSIFIED:
    case SECLVL_RESERVED1:
	/*
	 * Rewrite authority bits if necessary.
	 */
	if (user_sec->authority[0] != pak->authority) {
	    if (ipsecurity_debug)
		buginf("\nIP Security: BSO postroute: rewrite"
		       " %#x %#x to %#x %#x",
		       user_sec->classification, user_sec->authority[0],
		       inv_map_security(pak->classification),
		       pak->authority);
	    user_sec->authority[0] = pak->authority;
	    *state_change = TRUE;
	}
	break;
	    
    default:
	buginf("\npostroute: impossible security class,"
	       " src %i dst %i, code %#x",
	       ip->srcadr, ip->dstadr, user_sec->classification);
	memset(pak->datagramstart, 0xff, pak->datagramsize);
	return(FALSE);
    }
    return(TRUE);
}

/*
 * ip_sec_insert_option
 * Add IP security options to an existing packet.  If there is room,
 * squeeze the options in by ripple copying the packet.  If there is not,
 * make a new packet.  The new packet is then re-enqueued on the input
 * interface.  When the PAKMACRO split is done, we should be able to
 * avoid this re-enqueueing technique by instead getting the extra room
 * out of the space reserved for encapsulation bytes.
 */

static boolean ip_sec_insert_option (paktype *pak, int optlen, uchar *optdata,
			      uchar *where)
{
    paktype *new;
    iphdrtype *ip, *new_ip;
    int lead_bytes;
    uchar *src, *dst;

    ip = (iphdrtype *)ipheadstart(pak);

    if ((ip->tl + optlen) > pak_get_pool_size(pak)) {
	if (ipsecurity_debug)
	    buginf("\nIP Security: growing packet");
	/*
	 * Doesn't fit.  Allocate new pak, and initialize with current
	 * packet data up to the point where we want to add the security
	 * option.  Then construct the rest of the packet.
	 */
	src = ipheadstart(pak);
	lead_bytes = where - src;
	/*
	 * Allocate enough space for the maxmimum IP header size to make
	 * sure we don't get called twice (once for BSO, once for ESO).
	 */
	new = getbuffer(ip->tl - (ip->ihl << 2) + MAXIPHEADERBYTES);
	if (new == NULL)
	    return(FALSE);
	new_ip = (iphdrtype *)ipheadstart(new);
	pak_copy(pak, new, lead_bytes);
	src += lead_bytes;
	dst = ipheadstart(new) + lead_bytes;
	bcopy(optdata, dst, optlen);		/* Bytes of security option */
	dst += optlen;
	bcopy(src, dst, ip->tl - lead_bytes);
	new_ip->tl += optlen;
	new_ip->ihl += (optlen >> 2);
	new->datagramsize = new_ip->tl;
	new->datagramstart = ipheadstart(new);
	/*
	 * Now we must return as if we failed to insert this option.
	 * In actuality we have succeded, but are re-enqueueing the new
	 * packet because we have no way of providing the upper levels
	 * an indication that the packet pointer has moved.
	 * Before enqueueing, fix up the IP checksum.
	 */
	new_ip->checksum = 0;
	new_ip->checksum = ipcrc((ushort *)ipheadstart(new), ltob(new_ip->ihl));
	new->ip_flags |= PAKOPT_SECADD;
	ip_simple_enqueue(new);
	return(FALSE);
    }
    /*
     * Squeeze the option into an existing packet
     */
    src = ipheadstart(pak) + ip->tl;
    dst = src + optlen;
    for ( ; src > where; )
	*(--dst) = *(--src);
    bcopy(optdata, where, optlen);
    ip->tl += optlen;
    ip->ihl += (optlen >> 2);
    pak->datagramsize += optlen;
    return(TRUE);
}

/*
 * ip_sroute_postroute
 *
 * Strict/loose source route option processing.
 */

static void ip_sroute_postroute (paktype *pak, uchar *opts, ipaddrtype laddr,
			  boolean *state_change)
{
    ipopt_routetype *ptr;
    ipaddrtype sroute_g1;
    uchar *sroute;
    iphdrtype *ip;

    ip = (iphdrtype *)ipheadstart(pak);
    ptr = (ipopt_routetype *) opts;
    if (!pak->if_input) {
	/*
	 * Packet is sourced by us.  The source route may have originated
	 * from a remote machine, so it may be in either of the two possible
	 * forms described by rfc1122 sec. 3.2.1.8.  Accept both the correct
	 * and incorrect forms.
	 * Adjust the source route so that destination IP address
	 * is last hop of source route, and first hop of source
	 * route is now the destination IP address.
	 * Ie
	 *     S {>>G1, G2, ... GN} D
	 * becomes
	 *     S {>>G2, ... GN, D} G1
	 *
	 * First make sure we were handed a correctly constructed
	 * option list.
	 */
	if ((ptr->pointer != IPOPT_ROUTEMINPTR) ||
	    (ptr->length < IPOPT_ROUTEMINLEN)) {
	    errmsg(&msgsym(BADSROUTE, IP), ptr->length, ptr->pointer);
	    return;
	}
	if (ippkt_debug)
	    ipbug(pak, "source route");
	sroute = &opts[IPOPT_ROUTEHEADERSIZE];
	bcopy(sroute, &sroute_g1, sizeof(ipaddrtype));
	/*
	 * Ripple copy G2...GN
	 */
	bcopy(&sroute[sizeof(ipaddrtype)], sroute,
	      ptr->length - IPOPT_ROUTEHEADERSIZE - sizeof(ipaddrtype));
	/*
	 * Append D to source route
	 */
	bcopy(&ip->dstadr, &opts[ptr->length - sizeof(ipaddrtype)],
	      sizeof(ipaddrtype));
	ip->dstadr = sroute_g1;
	*state_change = TRUE;
    } else if (ip_ouraddress(ip->dstadr)) {
	/*
	 * The packet's route is aimed directly at us.  If the route is
	 * not complete, put the next hop into the destination address
	 * field and update recorded route with our output interface address
	 */
	if (ptr->pointer < ptr->length) {
	    bcopy(&opts[ptr->pointer - 1], &ip->dstadr, sizeof(ipaddrtype));
	    if (ippkt_debug)
		ipbug(pak, "source route");
	    bcopy(&laddr, &opts[ptr->pointer - 1], sizeof(ipaddrtype));
	    ptr->pointer += sizeof(ipaddrtype);
	    *state_change = TRUE;
	}
    }
    if (ptr->pointer < ptr->length)
	pak->ip_flags |= PAKOPT_SRNOTEND;
}
    
/*
 * ipopt_postroute
 *
 * Do IP option processing that occurs after routing decisions
 * have been made.  This includes inserting the host address into
 * record and/or source routing fields, time stamps, and outgoing
 * security checks.  The latter assumes that pak->if_output contains
 * a pointer to the interface that will be used for output.
 *
 * ipopt_postroute returns true if all options could be processed
 * sucessfully.  False if something went wrong.
 *
 * Security:
 *
 * Internal packets deal with security by inserting a special
 * security code that postroute is responsible for changing to
 * the appropriate level.  When we, postroute, are called routing
 * has been done and we know what level to set the packet to because
 * we now know which interface it is going out.
 *
 * Postroute handles rewriting the security option on packets that
 * arrived on an interface where we ignored the authority bits.  It also
 * makes sure that special interface output conditions are taken care of.
 *
 * Postroute will not flag a security violation if we are broadcasting.
 * It will cause the outgoing packet to be dropped.
 *
 * It is okay to call postroute without options.  A check is made and
 * nothing will be done.  Speed sensitive code should check to see if
 * options are around prior to call.
 *
 * Postroute assumes that inproc has been called for this packet previously.
 * It doesn't check for things like duplicate options that shouldn't be
 * there and similar things that inproc has already done.
 *
 * This routine can be called under one of four conditions:
 *
 * 	1) the main routing loop in a gateway following successful
 * 	   routing.  if_output will generally not equal if_input but
 * 	   if the packet is for us it could be.  under the assumption
 * 	   that we are going out as opposed to up a check against the
 * 	   output interface is made.  If the packet is for us this will
 * 	   be a duplicate check against the input interface.  This
 * 	   assumes that such a check has been done by inproc.
 *
 * 	2) if a packet is for us and routing failed.  Or if we are
 * 	   a terminal server and the packet is for us.  Postroute
 * 	   will be called to process other options other than security.
 * 	   Input will equal output and we will do a duplicate security
 * 	   check.
 *
 * 	3) Internally generated packets must all have postroute called
 * 	   prior to actually sending anything out to make sure we are
 * 	   allowed (security) as well as to update other option state.
 * 	   All real internally generated packets come through ipwrite.
 * 	   This is the only legitimate place where the special security
 * 	   insert code is used (inserted).  if_input should always be
 * 	   null here.
 *
 * 	4) Special cases.  There are a few special cases in the system.
 * 	   One such case is called special forwarding.  In special
 * 	   forwarding a packet is accepted and processed just like in
 * 	   #2 above but it is destined to be forwarded out.  The responsible
 * 	   routine processes it just like ip_forward would.  This makes
 * 	   it look as if ip_forward had forwarded it except that routing
 * 	   has happened twice.  Once to get it in and then once more to
 * 	   get it out.  Ipwrite is not used.  Another special case involves
 * 	   Slip.  Slip generates and routes packets on behalf of external
 * 	   Slip nodes.  It is special because there is no input interface.
 * 	   It handles packets just like special forwarding.
 *
 * The special security code SECINSERT is dangerous from a security stand
 * point.  It is very important to insure that no external packets can come
 * into the system with this code and be accepted.  This is insured in
 * ipopt_inproc which will not accept this code, unless the packet is
 * internally generated traffic.
 */

boolean ipopt_postroute (paktype *pak, ipaddrtype host)
{
    register int noptions;
    register uchar *opts;
    register ipopt_routetype *ptr;
    register idbtype *idb;
    iphdrtype *ip;
    boolean state_change;
    boolean bassec_seen;
    uchar *user_sec, *extsec_location;
    boolean extsec_seen;
    int extsec_optleft, len;

    ip = (iphdrtype *)ipheadstart(pak);

    extsec_optleft = 0;
    bassec_seen = state_change = extsec_seen = FALSE;
    user_sec = NULL;
    extsec_location = NULL;
    noptions = (ip->ihl << 2) - MINIPHEADERBYTES;
#ifdef DEBUG_OPTS
    if (ippkt_debug && ippkt_check(pak, TRUE, ipdebug_acl))
	buginf("\nIPOPT_OUT: s %i d %i dh %i h %i ob %d",
	     ip->srcadr, ip->dstadr, pak->desthost, host, noptions);
#endif
    idb = pak->if_output;
    if (!idb) {				/* never allowed in postroute */
	if (ipsecurity_debug)
	    buginf("\nIP Security: postroute: no interface, src %i dst %i",
		   ip->srcadr, ip->dstadr);
	return(FALSE);
    }

    /* Is there anything to do? */
    if (!noptions)
      goto options_finished;

    opts = ipheadstart(pak) + MINIPHEADERBYTES;
    pak->ip_flags &= ~PAKOPT_SRNOTEND;	/* clear not end */

    for (; noptions > 0; noptions -= len, opts += len) {
	len = ip_option_len(opts, noptions);
	if (len == 0) 
	    return(FALSE);

	switch (*opts) {
	  case IPOPT_BASSEC:
	      bassec_seen = TRUE;
	      user_sec = opts;
	      if (!ip_bso_postroute(pak, user_sec, &state_change))
		  return(FALSE);
	      break;
 	    
	    case IPOPT_EXTSEC:
	      if (!ip_eso_postroute(pak, opts))
		  return(FALSE);
	      if (!extsec_seen) {
		  extsec_location = opts;   /* remember first occurrance */
		  extsec_optleft = noptions;
		  extsec_seen = TRUE;
	      }
	      break;

	    case IPOPT_SSR:		/* strict source route */
	    case IPOPT_LSR:		/* loose source route */
	        ip_sroute_postroute(pak, opts, host, &state_change);
		break;

	    case IPOPT_RRT:		/* record route */
		ptr = (ipopt_routetype *) opts;
	        /*
		 * Only copy the next hop in if it is non-null.
		 * This handles the unnumbered interface problem
		 */
		if ((int)(ptr->length - ptr->pointer) >= 3 && host) {
		    host = h2nl(host);
		    bcopy(&host, opts + ptr->pointer - 1, sizeof(ipaddrtype));
		    ptr->pointer += sizeof(ipaddrtype);
		    state_change = TRUE;
		}
		break;

	    case IPOPT_TSTMP:
		if (ipopt_addtstmp((ipopt_tstmptype *)opts,host))
		  state_change = TRUE;
		break;

	    case IPOPT_END:
		noptions=0;		/* end options processing */
		break;

	    default:
		break;
	}
    }

options_finished:

    if ((ip_secopt_bytes) && ((pak->ip_flags & PAKOPT_SECSTRIP) == 0)) {
	if (!ip_sec_post_checks(pak, bassec_seen, user_sec, extsec_seen,
				extsec_location, extsec_optleft,
				&state_change))
	    return(FALSE);
    }
    /*
     * Option processing complete.  We may have changed option
     * state so we must recalculate the checksum.  We do it
     * here rather than everywhere we call postroute.
     */
    if (state_change) {
	ip->checksum = 0;
	ip->checksum = ipcrc((ushort *)ipheadstart(pak), ltob(ip->ihl));
    }
    return(TRUE);
}

/*
 * ip_eso_fixstyle
 * Perform extended IPSO output style operations (strip/add/insert).
 */

boolean ip_eso_fixstyle (paktype *pak, boolean seen, uchar *opt,
			 uchar *bso_opt, int optleft, boolean *state_change)
{
    idbtype *idb;
    iphdrtype *ip;
    int strip_bytes, optlen, size, pad, extlen;
    uchar *pak_data, *next_op;
    ulong *opt_long;
    uchar ipopts[MAXIPOPTIONBYTES];

    ip =(iphdrtype *)ipheadstart(pak);
    idb = pak->if_output;
    if (idb->ip_sec_outstyle & SECSTYLE_STRIP) {
	if (!seen)
	    return(TRUE);
	/*
	 * May be multiple ESOs to strip, search for them all and try
	 * to be smart about ripple copying the packet so as to not
	 * ripple the entire packet for each stripped option.
	 */
	strip_bytes = 0;
	pak_data = ipdatastart(pak);
	for (; optleft > 0; optleft -= optlen) {
	    optlen = ip_option_len(opt, optleft);
	    if (optlen == 0)
		return(FALSE);
	    if (*opt != IPOPT_EXTSEC) {
		opt += optlen;
		continue;		/* Skip non extended IPSO options */
	    }
	    next_op = opt + optlen;
	    size = (int)(pak_data - next_op) - strip_bytes;
	    if (size > 0)
		bcopy(next_op, opt, size);
	    strip_bytes += optlen;
	}
	if (strip_bytes) {
	    /*
	     * Determine option padding.  As there may already be padding,
	     * make sure we don't make 32 bits worth.
	     */
	    for (pad = strip_bytes % 4; pad; pad--) {
		*opt++ = PADDING;
		strip_bytes--;
	    }
	    opt_long = (ulong *)opt;
	    opt_long--;
	    if (*opt_long == 0) {
		opt -= 4;
		strip_bytes += 4;
	    }
	    ip->tl -= strip_bytes;
	    ip->ihl -= (strip_bytes >> 2);
	    /*
	     * Now ripple the data portion of the packet
	     */
	    size = ip->tl - (ip->ihl << 2);
	    bcopy(pak_data, opt, size);
	    *state_change = TRUE;
	}
	return(TRUE);
    }
    if (idb->ip_sec_outstyle & SECSTYLE_ADD ||
	(pak->classification == SECINSERT)) {
	if (seen)
	    return(TRUE);
	/*
	 * Add extended IPSO option to internally generated packet, or
	 * a packet without extended IPSO destined for an interface
	 * configured for extended IPSO.
	 */
	extlen = ip_eso_add_buf(idb, ipopts,
				MAXIPHEADERBYTES - (ip->ihl << 2));
	if (extlen == 0)
	    return(TRUE);
	if (ipsecurity_debug)
	    buginf("\nIP Security: Adding ESO source %d (%d bytes)",
		   ipopts[2], extlen);
	/*
	 * Stick the extended IPSO options into the packet header, after
	 * the (mandatory) BSO option.
	 */
	if (!ip_sec_insert_option(pak, extlen, ipopts,
				  bso_opt + SECURITY_SIZE))
	    return(FALSE);
	*state_change = TRUE;
	return(TRUE);
    }
    return(TRUE);
}

/*
 * ip_sec_post_checks
 * Perform any additional IPSO post-route work, regardless of whether
 * or not basic/extedned options are already present in the packet.
 */

boolean ip_sec_post_checks (paktype *pak, boolean bassec_seen, uchar *user_sec,
			    boolean extsec_seen, uchar *extsec_location,
			    int extsec_optleft, boolean *state_change)
{
    idbtype *idb;

    idb = pak->if_output;
    /*
     *  Before finishing up, we must test the security option against the
     *  label on the outgoing interface.  If it fails, then we send an
     *  "administratively prohibited" ICMP message, and trash the packet.
     *  There is no need to check if the packet was a SECINSERT, because
     *  it is guaranteed to pass.
     */
    if (pak->classification != SECINSERT) {
	if (!inclass(idb, pak->classification) ||
	    !inauthl(idb, pak->authority) ||
	    !inauthh(idb, pak->authority)) {
	    abort_for_security(pak, SECRESP_PROHIB, pak->classification,
			       pak->authority,
			       "postroute, failed xfc out");
	    return(FALSE);
	}
    }
    /*
     *  Check for any of the special output styles.  Do a quick check
     *  to see if there are any present, then do specific checks.
     *  This can only fail if we are trying to add an option to a
     *  packet that has a full IP header.
     */
    if (idb->ip_sec_outstyle) {
	if (!ip_bso_fixstyle(pak, bassec_seen, &user_sec, state_change, 
			     &extsec_location))
	    return(FALSE);
    }
    if (idb->ip_sec_outstyle || pak->classification == SECINSERT) {
	if (!ip_eso_fixstyle(pak, extsec_seen, extsec_location, user_sec,
			     extsec_optleft, state_change))
	    return(FALSE);
    }
    return(TRUE);
}

/*
 *  ip_bso_fixstyle
 *  This routine handles the set of variations in output style.  Users may
 *  set an interface to always include, or never include a security option.
 *  Also, if one is included, it may be forced to be first.  This will allow
 *  host that support, and hosts that don't support, the security option, to
 *  talk to each other without either breaking.  This routine sets
 *  state_change to TRUE if it has modified the packet.  It returns FALSE
 *  only if it needs to modify a packet, but cannot.
 */
boolean ip_bso_fixstyle (paktype *pak, boolean basic_seen, uchar **optp,
			 boolean *state_change, uchar **extsec_location)
{
    iphdrtype *ip;
    uchar *opts, *src, *dst;
    idbtype *idb = pak->if_output;
    ulong length, sec_offset;
    ipopt_bassectype *sec_opt;
    uchar optdata[SECURITY_SIZE];
    
    ip = (iphdrtype *)ipheadstart(pak);
    sec_opt = (ipopt_bassectype *) *optp;
    if (idb->ip_sec_outstyle & SECSTYLE_STRIP) {
	/*
	 * STRIP a security option if the output is marked such.  We now
	 * do this by shuffling the rest of the packet down over the
	 * security option.  The old method was to overwrite the option
	 * with NOPs. Always returns immediately.
	 */
	if (basic_seen) {
	    if (ipsecurity_debug)
		buginf("\nIP Security: Stripping BSO label: %x %x %x %x",
		       sec_opt->code, sec_opt->length,
		       sec_opt->classification, sec_opt->authority[0]);
	    sec_offset = (uchar *)sec_opt - ipheadstart(pak);
	    length = (ip->tl - SECURITY_SIZE - sec_offset);
	    bcopy((uchar *)(((uchar *)sec_opt) + SECURITY_SIZE),
		  (uchar *)sec_opt, length);
	    ip->tl -= SECURITY_SIZE;
	    ip->ihl--;
	    pak->length -= SECURITY_SIZE;
	    pak->dataptr -= SECURITY_SIZE;
	    *state_change = TRUE;
	    pak->ip_flags |= PAKOPT_SECSTRIP; /* security option stripped */

	    /*
	     *  We've stripped the bso option so we need to change
	     *  extsec_location to reflect that shift.
	     */
	    *extsec_location -= SECURITY_SIZE;
	}
	return(TRUE);
    }

    if (idb->ip_sec_outstyle & SECSTYLE_FIRST) {
 	/*
	 *  Move basic security options to the front.  We do not need to
	 *  mark a state change, because we will be swapping 32-bit
	 *  numbers.  That doesn't affect the checksum.
	 */
 	if (basic_seen) {
	    opts = (uchar *)ip + MINIPHEADERBYTES;
	    if ((uchar *)sec_opt > opts) {
		/*
		 *  Have to move it.
		 */
		if (ipsecurity_debug)
		    buginf("\nIP Security: Moving BSO label"
			   " (%x %x) to front.",
			   sec_opt->classification, sec_opt->authority[0]);
		for (dst=(uchar *)sec_opt+SECURITY_SIZE,
		     src = (uchar *)sec_opt;
		     src > opts; ) {
		    *(--dst) = *(--src);
		}
		opts[0] = IPOPT_BASSEC;
		opts[1] = SECURITY_SIZE;
		opts[2] = pak->classification;
		opts[3] = pak->authority;
	    }
	    *optp = opts;
	    return(TRUE);
	}
 	/*
	 *  If we are 'first' and there isn't a security option present,
	 *  then we must check to see if we are also 'add'.
	 */
    }

    if (idb->ip_sec_outstyle & SECSTYLE_ADD) {
	/*
	 *  ADD a security option if the output is marked such.  It is
	 *  not possible to get here unless the packet came in on a
	 *  implicit interface.  This lets us rewrite the packet
	 *  without worrying about it being on a TCP retransmit queue.
	 */
 	if (!basic_seen) {
	    if (ip->ihl < 15) {
		if (ipsecurity_debug)
		  buginf("\nIP Security: Adding BSO label (%x %x)",
			 inv_map_security(pak->classification),
			 pak->authority);
		opts = ipheadstart(pak) + MINIPHEADERBYTES;
		optdata[0] = IPOPT_BASSEC;
		optdata[1] = SECURITY_SIZE;
		optdata[2] = inv_map_security(pak->classification);
		optdata[3] = pak->authority;
		if (!ip_sec_insert_option(pak, SECURITY_SIZE, optdata, opts))
		    return(FALSE);
		*state_change = TRUE;
		*optp = ipheadstart(pak) + MINIPHEADERBYTES;
		return(TRUE);
	    }
	    abort_for_security(pak, SECRESP_REQNOROOM, pak->classification,
			       pak->authority, "no room to add IPSO");
	    return(FALSE);
 	}
    }
    return (TRUE);
}

/*
 * ipopt_reverseroute(ptr)
 * Reverse an IP source route, for use in sending the packet back
 * to the originator over the same path.  Note that this can be used
 * either on the route inside a packet, or on a route in an ipoptiontype.
 */

void ipopt_reverseroute (ipopt_routetype *ptr)
{
    register int nhops, i;
    ipaddrtype path[10];

    nhops = (ptr->length - IPOPT_ROUTEHEADERSIZE) >> 2;

    for (i=0; i < nhops; i++)
	bcopy(ptr->hops[i], &path[i], sizeof(ipaddrtype)); /* extract route */
    for (i--; i >= 0; i--)
	bcopy(&path[i], ptr->hops[nhops-(i+1)], sizeof(ipaddrtype)); /* insert backward */
    ptr->pointer = 4;				/* reset pointer */
}

/*
 * ipopt_reversepak
 */

void ipopt_reversepak (paktype *pak)
{
    ipopt_routetype *p;
    uchar *iphdr;

    iphdr = ipheadstart(pak);
    p = ipopt_findoptpak(iphdr, IPOPT_SSR);
    if (!p)
	p = ipopt_findoptpak(iphdr,IPOPT_LSR);
    if (p)
	ipopt_reverseroute(p);
}

/*
 * ipopt_addtstmp
 * Add a timestamp to the packet, in one of the 3 possible formats.
 */

boolean ipopt_addtstmp (ipopt_tstmptype *ptr, ipaddrtype nxthost)
{
    ulong time;
    ipaddrtype host;

    /*
     * Check if room for another timestamp.  If not, increment overflow
     * counter and take FALSE return.
     */
    if ((int)(ptr->length - ptr->pointer) < 3) {
	ptr->oflw += 1;
	return(FALSE);
    }
    switch (ptr->flg) {
	case IPOPT_TSTMP_TYPE2:	/* add timestamp if address already there */
	    bcopy((uchar *)ptr+ptr->pointer-1, &host,4);
	    if (n2hl(host) != nxthost)
		break;		/* not us, exit */
	    /* fall through */
	case IPOPT_TSTMP_TYPE1:	/* add address and timestamp */
	    if ((int) (ptr->length - ptr->pointer) < 7 || !nxthost)
		return(FALSE);	/* not enough room for full timestamp */
	    host = h2nl(nxthost);
	    bcopy(&host, (uchar *)ptr+ptr->pointer-1, sizeof(ipaddrtype));
	    ptr->pointer += sizeof(ipaddrtype);
	    /* fall through */
	case IPOPT_TSTMP_TYPE0:	/* Timestamps only */
	    time = h2nl(clock_icmp_time());
	    bcopy(&time, (uchar *)ptr+ptr->pointer-1, sizeof(ipaddrtype));
	    ptr->pointer += sizeof(ipaddrtype);
	    break;
	default:
	    return(FALSE);
    }
    return(TRUE);
}

/*
 * ipopt_fixfrag( pak, length )
 * Remove options that aren't supposed to be copied into a fragment
 * from the packet.  We overwrite the killed options with no-ops, so
 * that the overall header size remains the same.
 */

void ipopt_fixfrag (paktype *pak, register int length)
{
    register uchar *ptr;
    register int len;

    length -= MINIPHEADERBYTES;		/* compute number of option bytes */
    ptr = ipheadstart(pak) + MINIPHEADERBYTES;

    while (length > 0) {

	len = ip_option_len(ptr, length);

	if (len == 0 || *ptr == IPOPT_END)
	    break;

	if (*ptr & IPOPT_FRAGCOPY) {
	    length -= len;
	    ptr    += len;
	} else {
	    while (len-- > 0)
		*ptr++ = IPOPT_NOP;
	}
    }
}

/*
 * ipopt_copytcb( opts, pak )
 * Copy those options relevant to a connection of some kind (not necessarily
 * a TCP connection) into the optiontype array, for use in sending return
 * packets.  This includes SID, source routes, and security.
 *
 * This code does not kick out early like other option processing code.
 * If default security is turned on in the system and someone calls
 * us with out any options then we must insert the single default
 * security option.
 */

boolean ipopt_copytcb (ipoptiontype *opts, paktype *pak)
{
    iphdrtype *ip;
    int cnt = 0;		/* number of options bytes copied */
    int length, len;
    uchar *src, *dst;
    int optlen;
    ipopt_routetype *opt_start;
    boolean bassec_seen = FALSE;

    ip = (iphdrtype *)ipheadstart(pak);

    length = (ip->ihl << 2) - MINIPHEADERBYTES; /* number of option bytes */
    src = ipheadstart(pak) + MINIPHEADERBYTES;

    if (tcp_debug && length > 0)
	buginf("\nTCP: copying IP options from SYN packet");

    while (length > 0) {
	len = ip_option_len(src, length);
	if (len == 0)
	    return(FALSE);
	if (*src == IPOPT_END)
	    break;

	switch (*src) {
	    case IPOPT_BASSEC:
	    	bassec_seen = TRUE;
	    case IPOPT_SSR:		/* strict source route */
	    case IPOPT_LSR:		/* loose source route */
	    case IPOPT_SID:
	    case IPOPT_EXTSEC:
		opt_start = (ipopt_routetype *) &opts->ipopts[cnt];
		for (optlen = len; optlen > 0; optlen--)
		    opts->ipopts[cnt++] = *src++;
		if (opt_start->code == IPOPT_SSR ||
		    opt_start->code == IPOPT_LSR)
		  ipopt_reverseroute(opt_start);
		length -= len;		/* src was already bumped */
		break;

	    case IPOPT_TSTMP:		/* variable length opts not copied */
	    case IPOPT_RRT:
	    case IPOPT_NOP:
	    case IPOPT_ALERT:
		src    += len;
		length -= len;
		break;

	    default:
		if (ippkt_debug)
		    ipbug(pak, "unrecognized option");

		src    += len;
		length -= len;
		break;
	}
    }

    if (!bassec_seen && ip_secopt_bytes) {
	/*
	 * No security in the option field and default security
	 * is turned on.  Check for room and if we've got it insert
	 * the default security option.  The option consists of
	 * IPOPT_BASSEC, SECURITY_SIZE (4), SECINSERT, 0 and is very unlikely
	 * to ever change.
	 *
	 * To deal with our friend BFE we must make sure the security
	 * option is at the start of the options.  This means we must do
	 * a ripple copy.
	 */
	if (cnt > MAXIPOPTIONBYTES - SECURITY_SIZE)
	  return(FALSE);
	dst = &opts->ipopts[cnt-1+SECURITY_SIZE];
	src = &opts->ipopts[cnt-1];
	for (length = cnt; length > 0; length--)
	  *dst-- = *src--;
	opts->ipopts[0] = IPOPT_BASSEC;
	opts->ipopts[1] = SECURITY_SIZE;
	opts->ipopts[2] = inv_map_security(pak->classification);
	opts->ipopts[3] = pak->authority;
	cnt += SECURITY_SIZE;
    }
    opts->length = cnt;
    opts->paddedlength = (opts->length + 3) & ~3;
    for (cnt = opts->paddedlength - opts->length,
	dst = &opts->ipopts[opts->length];
	cnt; cnt--)
      *dst++ = PADDING;
    return(TRUE);
}

/*
 * ipopt_checktcb
 * Check to see if the security label it the tcb is a SECINSERT, and if
 * so, lock it down to the specific label of this packet.  If there are
 * no tcb options, then create a set.
 *
 * This routine exists to allow TCP to open a connection with a
 * SECINSERT, but have that modified to the actual level of the
 * connection before any data begins to flow.
 */
boolean ipopt_checktcb (ipoptiontype **optptr, paktype *pak)
{
    iphdrtype *ip;
    ipoptiontype *opts;
    int length;
    uchar *start;
    ipopt_routetype *basic_seen;

    ip = (iphdrtype *)ipheadstart(pak);

    if (tcp_debug)
	buginf("\nTCP: checking IP options in received SYN packet");

    /*
     * Find a basic security option in the incoming packet, if one is
     * present.  We are guaranteed by the IP input routines that there
     * will be 0 or 1 basic security options present.
     */
    length = (ip->ihl << 2) - MINIPHEADERBYTES; /* number of option bytes */
    start = (uchar *)ip + MINIPHEADERBYTES;
    basic_seen = ipopt_findopt(start, IPOPT_BASSEC, length);

    opts = *optptr;
    if (!opts && (basic_seen || ip_secopt_bytes)) {
	/*
	 * Security may be turned on but our tcb was not set up with
	 * any options.  Or security is not on (and there were no options
	 * sent in our initial SYN), 
	 * but the remote replied with a security options.
	 * We must now create a set of options for this connection.
	 */
	opts = malloc(sizeof (ipoptiontype));
	if (!opts)
	    return(FALSE);
	ip_add_bso_opt(opts);
	*optptr = opts;
	opts->ipopts[2] = inv_map_security(pak->classification);
	opts->ipopts[3] = pak->authority;
	return(TRUE);
    } else if (ip_secopt_bytes) {
	/*
	 * Are we in secure mode?  If so, tcp_open insures that the first
	 * option in the tcb is guaranteed to be security.  We don't care
	 * what the label in the packet says, we have already run the
	 * classifier and rely on it's output for the correct level.
	 */
	if (opts->ipopts[0] != IPOPT_BASSEC) {
	    errmsg(&msgsym(NOTSEC, SEC));
	    return(FALSE);
	}
	if (opts->ipopts[2] != SECINSERT) {
	    errmsg(&msgsym(SECINS, SEC));
	    return(FALSE);
	}
	opts->ipopts[2] = inv_map_security(pak->classification);
	opts->ipopts[3] = pak->authority;
	return(TRUE);
    }
    /*
     * We are not secure, and the remote did not reply with a security
     * options.  We're outta here.
     */
    return(TRUE);
}

/*
 * optionbytesleft
 * Compute number of option bytes left in a packet that already
 * has options in it, and has (paksize) header bytes total.
 */

int optionbytesleft (int maxsize, ipoptiontype *curopts)
{
    int bytesleft;

    if (maxsize < MAXIPHEADERBYTES) 
	bytesleft = maxsize - (curopts->length + MINIPHEADERBYTES);
    else
	bytesleft = MAXIPOPTIONBYTES - curopts->length;
    return((bytesleft < 0) ? 0 : bytesleft);
}

/*
 * send_parameter2
 * Send an ICMP parameter problem message at a particular security level
 */

void send_parameter2 (
    register paktype *pak,
    int code,
    int offset,
    uchar class,
    uchar auth)
{
    iphdrtype *ip;
    register icmptype *icmp;
    ipoptiontype secopt;
    ipaddrtype myself;
    paktype *newp;
    ulong size;

    ip = (iphdrtype *)ipheadstart(pak);

    icmp = (icmptype *) icmp_sendsetup(pak, &newp, &size);

    if (!icmp)
	return;
    secopt.ipopts[0] = IPOPT_BASSEC;
    secopt.ipopts[1] = SECURITY_SIZE;
    secopt.ipopts[2] = inv_map_security(class);
    secopt.ipopts[3] = auth;
    secopt.length = SECURITY_SIZE;
    secopt.paddedlength = SECURITY_SIZE;
    icmp->type = ICMP_PARAMETER;
    icmp->code = code;
    icmp->identifier = 0;
    icmp->sequence = 0;
    *(((uchar *) icmp) + 4) = offset;
    icmp->checksum = 0;
    icmp->checksum = ipcrc((ushort *)icmp, size - IPHEADERBYTES(NOPT));
    myself = pak->if_input ? pak->if_input->ip_address :
	     ip_best_local_address(ip->srcadr, TRUE);
    icmp_traffic.paramsent++;
    if (icmppkt_debug)
	buginf("\nICMP: parameter problem sent to %i (dest was %i)",
	    n2hl(ip->srcadr), n2hl(ip->dstadr));
    ipwrite(newp,size,ICMP_PROT,myself,n2hl(ip->srcadr),
	NULL, ip_secopt_bytes ? &secopt : NULL, &ipdefaultparams);
}

/*
 * send_unreachable2
 * Send an ICMP unreachable at a specific security classification
 */

void send_unreachable2 (register paktype *pak, uchar code, uchar class, uchar auth)
{
    iphdrtype *ip;
    paktype *newp;
    register icmptype *icmp;
    ipaddrtype myself;
    idbtype *idb;
    ulong size;
    ipoptiontype secopt;

    ip = (iphdrtype *)ipheadstart(pak);

    idb = pak->if_input;
    if (!idb ||
	CLOCK_IN_INTERVAL(idb->ip_unreach_timer, idb->ip_unreach_interval))
	return;
    if (!netsareup || !idb->ip_unreach)
	return;
    secopt.paddedlength = SECURITY_SIZE;
    secopt.length = SECURITY_SIZE;
    secopt.ipopts[0] = IPOPT_BASSEC;
    secopt.ipopts[1] = SECURITY_SIZE;
    secopt.ipopts[2] = inv_map_security(class);
    secopt.ipopts[3] = auth;
    myself = pak->if_input->ip_address;
    icmp = icmp_sendsetup(pak, &newp, &size);
    if (!icmp)
	return;
    icmp->type = ICMP_UNREACHABLE;
    icmp->code = code;
    icmp->identifier = 0;
    icmp->sequence = 0;
    icmp->checksum = 0;
    icmp->checksum = ipcrc((ushort *)icmp, size - IPHEADERBYTES(NOPT));
    icmp_traffic.unreachsent++;
    if (icmppkt_debug)
	buginf("\nICMP: dst (%i) %s unreachable sent to %i",
	    ip->dstadr, unreach2string(code), ip->srcadr);
    ipwrite(newp,size,ICMP_PROT,myself,n2hl(ip->srcadr),
	NULL, ip_secopt_bytes ? &secopt : NULL, &ipdefaultparams);
    GET_TIMESTAMP(idb->ip_unreach_timer);
}

/*
 * print_security
 * Display security setting and style for current interface.
 */

void print_security (idbtype *idb)
{
    printf("\n  Security level is ");
    if (idb->ip_sec_default)
        printf("default");
    else {
        if (!idb->ip_sec_multilevel)
	    printf("dedicated %s %s",
		   security_string(idb->ip_sec_low),
		   authority_string(&idb->ip_sec_auth_low));
	else {
	    printf("multilevel %s %s",
		   security_string(idb->ip_sec_low),
		   authority_string(&idb->ip_sec_auth_low));
	    printf("\n                            to %s %s",
		   security_string(idb->ip_sec_high),
		   authority_string(&idb->ip_sec_auth_high));
	  }

	if (idb->ip_sec_implicit)
	  printf("\n  Implicit labelling at %s %s",
		 security_string(idb->ip_sec_def_class),
		 authority_string(&idb->ip_sec_def_auth));

	if (idb->ip_sec_instyle || idb->ip_sec_outstyle)
	  printf("\n  Security style is: %s%s%s%s",
		 (idb->ip_sec_outstyle & SECSTYLE_ADD) ? "add " : "",
		 (idb->ip_sec_outstyle & SECSTYLE_STRIP) ? "strip " : "",
		 (idb->ip_sec_outstyle & SECSTYLE_FIRST) ? "first " : "",
		 (idb->ip_sec_instyle  & SECSTYLE_IGNOREAUTH) ? "ignore-auth " : "");
    }
    if (idb->ip_sec_extended)
	printf("\n  Extended security options are %sed",
	       (idb->ip_sec_extended == IP_ESO_IGNORE) ? "ignor" : "allow");
    if (idb->ip_sec_cipso)
	printf("\n  Commercial security options are %sed",
	       (idb->ip_sec_cipso == IP_CIPSO_IGNORE) ? "ignor" : "deni");
}
