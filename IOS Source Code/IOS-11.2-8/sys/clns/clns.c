/* $Id: clns.c,v 3.9.22.22 1996/08/28 12:38:16 thille Exp $
 * $Source: /release/112/cvs/Xsys/clns/clns.c,v $
 *------------------------------------------------------------------
 * clns.c -- Basic Services for ISO Connectionless Network Protocol
 * 
 * 25-August-1988 by Jim Forster
 * Jan-27-1989 by Jim Forster
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clns.c,v $
 * Revision 3.9.22.22  1996/08/28  12:38:16  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.9.22.21  1996/08/08  14:50:05  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Use new scheduler primitives.  Get rid of gratuitous type coercion.
 *
 * Revision 3.9.22.20  1996/08/04  21:34:07  dkatz
 * CSCdi64354:  ESIS bonus ISHs quashed
 * Branch: California_branch
 * Remove arbitrarily complex logic around the sending of bonus ISHs;
 * rather, always send a rate-limited unicast ISH whenever a new
 * adjacency (ES or IS) is detected.  Send an ISH when the line
 * protocol comes up as well.
 *
 * Revision 3.9.22.19  1996/08/03  23:20:11  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.9.22.18  1996/07/26  18:13:57  tylin
 * CSCdi59808:  clns ping fails between 7000 ethernet interfaces
 *    Fix dialer i/f checking by calling is_ddr().
 *    Update driver's filter table for all ES/IS multicast addresses
 *    whenever enable/disable CLNS routing.
 * Branch: California_branch
 *
 * Revision 3.9.22.17  1996/07/13  11:37:46  fred
 * CSCdi62873:  NULL dereference in fair-queue routines when called by
 *         traffic shaping - make hash_mask independent of caller
 * Branch: California_branch
 *
 * Revision 3.9.22.16  1996/07/11  18:29:04  asastry
 * CSCdi60438:  CLNS fails to work over DDR unless dialer strings are
 * used. Use next hop NSAP to do the dialer look-up.
 *
 * Branch: California_branch
 *
 * Revision 3.9.22.15  1996/07/06  05:52:23  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.9.22.14  1996/07/04  00:58:10  asastry
 * CSCdi60841:  DECnet conversion causes SegV exception, PC
 * ether_extract_addr
 * Branch: California_branch
 *
 * Revision 3.9.22.13  1996/07/02  23:10:30  wmay
 * CSCdi34302:  Protocol accounting not done correctly for dialer ints
 * do receive accounting for dialer ints on the actual input interface
 * Branch: California_branch
 *
 * Revision 3.9.22.12  1996/06/24  09:18:24  fox
 * CSCdi59550:  PPP causes router to crash
 * Branch: California_branch
 * Slim down and speed up linktype to NCP index to PPP protocol lookups.
 * Extend NCP protocheck functions to pass original IDB as well as config
 * IDB.  Change NCP names to upper case.  Display NCP name in trace of
 * PPP negotiation.
 *
 * Revision 3.9.22.11  1996/06/19  16:04:42  asastry
 * CSCdi52326:  Missing CLNS redirects during DECnet IV to Cluster alias
 * communication.
 * Branch: California_branch
 *
 * Revision 3.9.22.10  1996/05/30  01:40:43  asastry
 * CSCdi57629:  7000 router crashes with CLNS fast Switching errors
 * Branch: California_branch
 *
 * Revision 3.9.22.9  1996/05/22  17:03:53  rchandra
 * CSCdi55055:  Route not found with clns isis (2500, 4000, 4500 and 7500
 *              platforms)
 * Branch: California_branch
 * - be careful about resetting the interface number during isis interface
 *   reset
 *
 * Revision 3.9.22.8  1996/05/17  10:41:32  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.9.8.3  1996/05/05  22:50:08  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.9.8.2  1996/04/26  07:32:57  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.9.8.1  1996/04/03  13:51:53  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.9.22.7  1996/05/10  01:36:43  wmay
 * CPP and ppp half bridge commit
 * Branch: California_branch
 *
 * Revision 3.9.22.6  1996/05/04  20:03:47  asastry
 * CSCdi56559:  CLNS cache invalidations are too frequent, causing high
 * CPU utilization.
 * Branch: California_branch
 *
 * Revision 3.9.22.5  1996/05/04  00:35:32  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Bridge-group Virtual Interface smf support.
 * - Set up bridge/route indicators based on CRB/IRB is enabled or not.
 *
 * Revision 3.9.22.4  1996/04/29  09:35:53  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.9.22.3  1996/04/29  02:42:03  bcole
 * CSCdi55590:  configuring ipx routing  breaks IP ping
 * Branch: California_branch
 *
 * Add 'ipdiag' to the list of subsystem dependencies for subsystems which
 * support the ping command.
 *
 * Revision 3.9.22.2  1996/04/17  13:35:58  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.9.22.1  1996/03/18  19:02:20  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.6.3  1996/03/13  01:12:44  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.8.6.2  1996/03/07  08:39:43  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.8.6.1  1996/02/20  00:20:36  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/01/24  20:11:53  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed tuba.c, tuba_default.c and tuba.h
 * . removed all includes for tuba.h.
 * . removed more tuba related code.
 *
 * Revision 3.8  1996/01/11  02:20:49  jenny
 * CSCdi46513:  TCP/IP need code clean up (part 2)
 * . removed clns_tuba.c.
 * . removed all registry calls to TUBA code in CLNS subsystem.
 *
 * Revision 3.7  1995/12/07  22:27:51  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.6  1995/12/01  02:48:13  dkatz
 * CSCdi39906:  CPUHOG Process = IS-IS Update
 *
 * Revision 3.5  1995/11/27  21:45:56  rchandra
 * CSCdi43854:  Decnet: converted packet with exceeded lifetime cause crash
 * - if lifetime is exceeded for the converted packet, do not try to
 *   generate clns error pdu
 * - allow the converted packet marked for return (DN_RET) to have twice
 *   the lifetime than the normal converted packet.
 *
 * Revision 3.4  1995/11/21  23:56:45  raj
 * CSCdi39109:  CLNS generates noise when interface is down
 * Added interface_up calls in various needed places and changed one
 * errmsg to a buginf instead.
 *
 * Revision 3.3  1995/11/17  08:51:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:12:07  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:07:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.12  1995/11/08  20:51:49  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.11  1995/10/11  23:57:25  dkatz
 * CSCdi41968:  CLNS ER PDUs all messed up
 * Calculate data length properly.  Copy the data from the right place.
 * Fix the debug routine so that it properly displays the packet.
 *
 * Revision 2.10  1995/09/23  21:37:16  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Calculate routes from only changed LSPs if the inter-router topology
 * stays constant.  Don't gratuitously install IP routes that will
 * be immediately replaced, as this causes large-scale cache invalidation.
 * Eliminate multiple IP forwarding table traversals.
 *
 * Revision 2.9  1995/09/01  23:05:27  asastry
 * CSCdi39698:  WFQ: Display info (in "show queue") for non-IP protocols
 * as well
 *
 * Revision 2.8  1995/08/23  18:02:45  dkatz
 * CSCdi37692:  LSP checksum errors can melt network
 * Add a knob to discard incoming LSPs with checksum errors, rather
 * than purging them.
 *
 * Add additional debugging capabilities.
 *
 * Revision 2.7  1995/08/08  16:42:32  hampton
 * Convert CLNS to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi38392]
 *
 * Revision 2.6  1995/07/28 21:59:21  rchandra
 * CSCdi37236:  ALIGN-1-FATAL: Illegal access to a low address
 * - do not send redirects if it is a Phase IV to Phase V conversion
 *
 * Revision 2.5  1995/07/09  21:08:51  speakman
 * CSCdi36848:  Configuration of CRB proves unpopular
 * Derive the CRB route/bridge behaviour from explicit, per-interface
 * route/bridge indicators rather than inferring it from protocol-
 * specific configuration.
 *
 * Revision 2.4  1995/06/21  08:53:38  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.3  1995/06/19  04:04:09  speakman
 * CSCdi35697:  General TB and SRB regression failures
 * Make protocol-specific route/bridge test explicit rather than relying
 * on per-interface rxtypes indicators to provide backward compatible
 * SRB and TB behaviour.
 *
 * Revision 2.2  1995/06/18  06:18:05  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.1  1995/06/07 20:16:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../clns/msg_clns.c"		/* Not a typo, see logger.h */
#include "../ui/common_strings.h"
#include <stdarg.h>
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "linktype.h"
#include "address.h"
#include "name.h"
#include "mgd_timers.h"
#include "../iprouting/route.h"
#include "../if/network.h"
#include "../if/priority_private.h"
#include "../x25/x25.h"
#include "../wan/dialer.h"
#include "config.h"
#include "../ui/debug.h"
#include "../decnet/dnconversion.h"
#include "../clns/clns.h"
#include "../clns/clns_route.h"
#include "../clns/clns_adj.h"
#include "../clns/isis.h"
#include "../clns/clns_externs.h"
#include "../clns/isis_externs.h"
#include "../clns/clns_debug.h"
#include "../ip/ip_registry.h"
#include "../os/filter.h"
#include "../os/filter_externs.h"
#include "isis_nlsp_debug.h"
#include "../clns/clns_registry.h"
#include "../clns/isis_registry.h"
#include "../tarp/tarp.h"
#include "../wan/ppp.h"
#include "../wan/ppp_registry.h"


/*
 * Local Definitions & Storage
 */
boolean        clns_running;	          /* currently running clns */
boolean        clns_router_running;       /* currently routing clns (as IS) */
boolean        clns_processes_setup;      /* clns processes are running */
boolean	       clns_security_passthru;    /* clns security options - packets
					     with security options NOT allowed
					     to pass thru', by default */
ulong          clns_nnets;	          /* # of if configured for clns if 
					     clns_nnets >1 then we are an IS */
watched_queue  *clnsQ;
queuetype      esisQ;
queuetype      clns_echoQ;		  /* clns echo queue */
ulong          clns_traffic[CLNS_MAXTRAFFIC];	/* statistics */
ulong          clns_config_timer;	  /* all time in milliseconds */
ulong          clns_holding_timer;
ulong          clns_esct_timer;           /* used in ES mode only */
ushort         clns_packet_lifetime;      /* in seconds */
hwaddrtype     all_ess;			  /* all end systems */
clns_ddbtype   *clns_routes[CLNS_NETHASHLEN]; /* prefix routing table */
clns_ddbtype   *clns_default_prefix;          /* Default prefix route */
clns_cachetype *clns_cache[CLNS_CACHE_SIZE];
int	       clns_cache_changes;	  /* count of cache changes */
clns_pdbtype   *clns_staticpdb;           /* pdb for static routes */
char           clns_STATICpname[] = " ";  /* pname for router static */

/*
 * ES specific variables - if this system is configured as an ES.
 */
clns_adjtype dummy_adj;         /* for 'configuration requests' */
boolean      clns_want_erpdus;  /* do we want erpdu's back on locally
				   generated packets ? */

/*
 * ES-IS Variables
 */
mgd_timer esis_timer;			/* Global timer for ES-IS */
mgd_timer esis_ager_timer;		/* Timer for esis_ager() */
mgd_timer esis_hello_timers;		/* Head of hello timer chain */


/* 
 * Static routes stored here.
 */
queuetype clns_staticQ;

/*
 * Local NETs configured are stored here.
 */
queuetype clns_local_nets;

/*
 * Cache invalidation delay information.
 */
static boolean clns_cache_delay_inval;       /* cache inv delay ON/OFF ? */
static boolean clns_cache_inval_pending;     /* is a cache inv pending ? */
static sys_timestamp clns_cache_inval_req;   /* when inv req was received */
static ulong clns_cache_inval_mintime;	     /* milliseconds */
static ulong clns_cache_inval_maxtime;	     /* milliseconds */
static unsigned clns_cache_inval_quiettime;  /* seconds */
static unsigned clns_cache_inval_quietthresh;/* count of requests */
static ulong *clns_cache_inval_buffer;	    /* circular buffer of req counts */
static ulong clns_cache_inval_recent;	     /* total of all buffer entries */
static ulong *clns_cache_inval_current;	/* current pointer into buffer  */
static boolean clns_cache_purge_immediate;   /* if TRUE, purge cache 
						immediately */
/*
 * Special case handling for intra-cluster traffic. All NSAP's with
 * NSEL of 41 or 81 (hex) are treated as inta-cluster traffic, and are
 * handled specially when we need to send redirects.
 */
#define CLNS_CLUSTER_ALIAS_NSEL1 0x41
#define CLNS_CLUSTER_ALIAS_NSEL2 0x81


/*
 * clns_parse_hostname
 * Convert an NET or NSAP string into a hostname with an internet address.
 * result FALSE if the parse fails, TRUE otherwise with "result" updated.
 */
boolean clns_parse_hostname (char *ptr, NSAP_T nsap)
{
    int parsed, dummy;
    nametype *cp;

    parsed = parse_nsap(ptr, nsap);
    if (!parsed) {
	cp = name_lookup(ptr, &dummy, NAM_CLNS);
	if (cp) {
	    nsap_copy(*(NSAP_T*)cp->v.clns.adr[0].addr.clns_address, nsap);
	    parsed = TRUE;
	}
    }
    return(parsed);
}
 
/*
 * nsap_printf
 * N/n prints a CLNS NSAP address.  Takes NSAP_T argument.
 */

static int nsap_printf (const char **fmt, char **cp, tt_soc *tty, va_list *argp,
			boolean rjust, int width, int size, int hash,
			int precision)
{
    char *str;
    int length;
    int d, charcount;

    charcount = 0;
    str = nsap_addr_string(NSAP_T_to_nsap_addr(va_arg(*argp, NSAP_T *)));
    length = (int) strlen(str);
    if (rjust && width != -1 && (length < width)) {
	for (d = 0; d < (width - length); d++) {
	    doprintc(cp, tty, ' '); /* Leading fill - spaces */
	    charcount++;
	}
    }
    if (*cp) {
	*cp += sprintf (*cp, "%s", str);
    } else {
	_ttyprintf(tty, "%s", str);
    }
    charcount += length;
    if (!rjust && width != -1 && (length < width)) {
	for (d = 0; d < (width-length); d++) {
	    doprintc(cp,tty,' '); /* trailing fill - spaces*/
	    charcount++;
	}
    }
    return(charcount);
}

/*
 * clns_hellostring
 */
char *clns_hellostring (int htype)
{
    switch (htype) {
      case CLNS_OURNSAP_ENTRY: return("Our NSAP");
      case CLNS_OURNET_ENTRY: return("Our NET");
      case CLNS_ES_ENTRY: return("ES");
      case CLNS_IS_ENTRY: return("IS");
      case CLNS_ROUTE_ENTRY: return("Route");
      case CLNS_NULL_ENTRY: return("Null");
      case CLNS_REDIRECT_ENTRY: return("Redirect");
      case CLNS_DISCARD_ENTRY: return("Discard");
      default: return("**Bogus**");
    }
}

/*
 * discard_string
 * Return a string describing a ER PDU Reason for discard
 */

char *discard_string (int code)
{
    switch (code) {
      case CLNS_DISC_UNSPEC: return("unspecified reason");
      case CLNS_DISC_PROTOCOL: return("protocol procedure error");
      case CLNS_DISC_CHECKSUM: return("bad checksum");
      case CLNS_DISC_CONGEST: return("dropped because of congestion");
      case CLNS_DISC_HDRSYNTAX: return("header syntax error");
      case CLNS_DISC_SEGMENT: return("segmentation was forbidden");
      case CLNS_DISC_INCOMPL: return("incomplete PDU received");
      case CLNS_DISC_DUPLICATE: return("duplicate option");
      case CLNS_DISC_UNREACHDST: return("destination unreachable");
      case CLNS_DISC_UNKNOWNDST: return("destination unknown");
      case CLNS_DISC_SROUTEUNSPEC: return("unspecified source route error");
      case CLNS_DISC_SRSYNTAX: return("source route syntax error");
      case CLNS_DISC_UNKNOWNADDR: return("source route unknown address");
      case CLNS_DISC_BADPATH: return("source route unacceptable path");
      case CLNS_DISC_HOPS: return("lifetime exceeded in transit");
      case CLNS_DISC_REASSEM: return("lifetime exceeded while reassembling");
      case CLNS_DISC_OPTION: return("unsupported option");
      case CLNS_DISC_VERSION: return("unsupported protocol version");
      case CLNS_DISC_SECURITY: return("unsupported security option");
      case CLNS_DISC_SROUTE: return("unsupported source route option");
      case CLNS_DISC_RROUTE: return("unsupported record route option");
      case CLNS_DISC_INTERF: return("reassembly interference");
      default: return("unknown reason for discard");
    }
}

/*
 * option_string
 * Convert an option into a string
 */

char *clns_option_string (uchar *opt)
{
    char *ptr;

    ptr = next_buf();
    switch (*opt) {
      case CLNS_OPT_DISCARD_REASON:
	(void) sprintf(ptr, "Discard Reason: %s", 
		       discard_string((int)opt[2]));
	return(ptr);
      case CLNS_OPT_PADDING:		return("Padding");
      case CLNS_OPT_PRIORITY:		return("Priority");
      case CLNS_OPT_QOS:		return("Quality of Service");
      case CLNS_OPT_RROUTE:		return("Record Route");
      case CLNS_OPT_SROUTE:		return("Source Route");
      case CLNS_OPT_SECURITY:		return("Security");
      default:				return("Unknown");
    }
}


/*
 * clns_print
 * Print a CLNS datagram in gory detail
 */

void clns_print (clnshdrtype *clns)
{
    uchar *ptr;
    clns_segpart_t *segptr;
    int size;
    int i;

    buginf("\nCLNS: pid=%#x, hlen=%d, ver=%d, life=%d, sp=%d, er=%d, ms=%d",
	clns->pid, clns->hlen, clns->version, clns->lifetime,
	clns->sp, clns->er, clns->ms);
    buginf("\n      type=%#x, seglen=%d, checksum=%#x", clns->type,
	GETSHORT(clns->clns_seglen), GETSHORT(clns->checksum));
    if (clns->pid != NLP_ISO8473) {
	return;
    }
    size = CLNS_FIXEDHDR_SIZE;
    /*
     * Sanity check: is sum of address lengths contained within header?
     */
    ptr = ((uchar *) clns) + CLNS_FIXEDHDR_SIZE;
    size += ptr[0] + 1;	/* small bug: if packet is small, we're off the end */
    if (!NSAP_LENGTH_CHECK (ptr) || size > clns->hlen) {
	buginf("\n        ***Invalid dal %d***", ptr[0]);
	return;
    }
    buginf("\n      dal=%d, da=%n", ptr[0], ptr);
    ptr += ptr[0] + 1;

    size += ptr[0] + 1;	/* small bug: if packet is small, we're off the end */
    if (!NSAP_LENGTH_CHECK (ptr) || size > clns->hlen) {
	buginf("\n        ***Invalid sal %d***", ptr[0]);
	return;
    }
    buginf(", sal=%d, sa=%n", ptr[0], ptr);
    ptr += ptr[0] + 1;
    /*
     * Segmentation Part
     * Sanity check: is presence of segment part consistent with header len?
     * Are the sp and ms fields consistent?
     */
    if (clns->sp == TRUE) {
	size += sizeof(clns_segpart_t);
	if (size > clns->hlen) {
	    buginf("\n        ***Segpart won't fit***");
	    return;
	}
	segptr = (clns_segpart_t *) ptr;
	buginf("\n       duid=%d, segoff=%d, length=%d",
		GETSHORT(segptr->duid), GETSHORT(segptr->segoffset),
		GETSHORT(segptr->length));
	ptr += sizeof(clns_segpart_t);
    }
    buginf(", data bytes=%d", GETSHORT(clns->clns_seglen) - clns->hlen);
    /*
     * Options
     * Sanity check: is option length consistent with header length?
     */
    while (clns->hlen > (size + 1)) {
	buginf("\n      optcode=%#x, optlen=%d (%s)", ptr[0], ptr[1],
	       clns_option_string(ptr));
        for (i = 0; i < ptr[1]; i++) {
 	    buginf(" %x", ptr[i + 2]);
	}
	size += ptr[1] + 2;
	ptr += ptr[1] + 2;
    }
}

/*
 * clns_get_local_npai
 * Return our NET if we're an IS, else one of our NSAPs
 */

NSAP_T *clns_get_local_npai (clns_pdbtype *pdb)
{
    if (pdb) {
	return(&pdb->net);
    }
    return(&clns_myNET);

}

/*
 * clns_getbuffer
 * Small wrapper for getbuffer that initializes the clns fields in the
 * the packet buffer, and zeros out the data portion.  We don't mind
 * the overhead of zeroing the buffer because the high-traffic case
 * of forwarding a packet does not call this routine.
 */

paktype *clns_getbuffer (int size)
{
    paktype *pak;

    pak = getbuffer(size);
    if (!pak) {
	return(NULL);
    }
    memset(pak->network_start, 0, size);
    pak->clns_nexthopaddr = NULL;
    pak->clns_dstaddr = NULL;
    pak->clns_srcaddr = NULL;
    pak->clns_segpart = NULL;
    pak->clns_optpart = NULL;
    pak->clns_datapart = NULL;
    pak->clns_qos = NULL;
    pak->clns_flags = 0;
    pak->if_input = NULL;
    pak->linktype = LINK_CLNS;
    return(pak);
}

/*
 * domain_string
 *
 */
char *domain_string (char domain_length, uchar domain_id[MAX_DOMAIN_LENGTH])
{
    int i;
    char *ptr, *p2;
    uchar nibble;

    ptr = next_buf();
    p2 = ptr;
    for (i = 0; i < domain_length; i++) {
	nibble = domain_id[i] >> 4;
	*ptr++ = nibble + (nibble < 10 ? '0' : '7');
	nibble = domain_id[i] & 0x0f;
	*ptr++ = nibble + (nibble < 10 ? '0' : '7');
	if (((i % 2) == 0) && ((i + 1) < domain_length)) {
	    *ptr++ = '.';
	}
    }
    *ptr = '\0';
    return(p2);
}

/*
 * clns_network_fixup
 *    this routine is called when an interface encapsulation changes.
 *    if we have clns neighbors on this interface - delete them.
 * NOTE that this is done regardless of whether the
 * encapsulation actually changed. This should be
 * changed to a more general mechanism
 *
 */

static void clns_network_fixup (idbtype *idb, int old_encap)
{
    clns_rdbtype *nr;
    boolean changed;
    clns_ddbtype *dest;
    int i, hash_index, bucket;
    clns_pdbtype *pdb;
    clns_adjtype *adj, *next;

    changed = FALSE;
 
    /*
     * If the encapsulation is changed, blow away all the adjacencies
     * pointing to that inteface.  If the old adjacency was X25 or
     * SMDS, then remove all the static adjacencies pointing to that
     * interface. This is because all those interface static
     * adjacencies will be pointing to X121 addresses or SMDS address.
     */
     for (bucket = 0; bucket < CLNS_ADJ_DB_SIZE; bucket++) {
       for (adj = clns_adjacency_db[bucket]; adj; adj = next) {
           next = adj->next;
           if (adj->idb != idb)
               continue;
           if ((adj->protocol_source & ISO_STATIC) && 
               (old_encap != ET_X25) && (old_encap != ET_SMDS)) {
               continue;
           }
           delete_adjacency(clns_adjacency_db, adj);
           changed = TRUE;
        }
      }

    /*
     * Do nothing more if CLNS not enabled on this interface.
     */
    if (!idb->clns_enabled) {
	return;
    }

    /*
     * now look for conforming clns neighbors. In case of ISIS,
     * remove all the isis related informatiom from the idb and start
     * isis on that interface again. This is because, the interface
     * might have moved from non_p2p to p2p or vice versa due to
     * change in encapsulation.
     */
    for (pdb = clns_protoQ.qhead; pdb; pdb = pdb->next) {
        if ((pdb->typeflag & ISO_ISISMASK) &&
            (isis_checkqueue(&pdb->osi_idbq, idb))) {
            isis_reset_idb_params(pdb, idb, FALSE);
            isis_start_interface(pdb, idb);
            changed = TRUE;
        }
	if (itf_in_pdb(idb, pdb)) {
	    dest = (clns_ddbtype *)NULL;
	    hash_index = 0;
	    while ((dest = clns_walk_ddb(pdb, &hash_index, dest))) {
		for (i = 0; i < dest->routecount; i++) {
		    nr = dest->rdb[i];
#ifdef 0
		    if (((nr->entry_type == NRIB_IS) ||
			 (nr->entry_type == NRIB_ES)) &&
			(nr->idb == idb)) {
			iso_igrp_route_delete(pdb, dest, i);
			changed = TRUE;
		    }
#endif
		}
	    }
	}
    }
    if (changed) {
	clns_invalidate_cache ("clns_network_fixup", TRUE);
    }

    /*
     * Change default setting only for split horizon when encapsulation 
     * changes if different than current setting.
     */
    if (idb->clns_split_enabled == idb->clns_split_enabled_def) {
	idb->clns_split_enabled_def = (is_wan(idb->hwptr)) ? FALSE : TRUE;
	idb->clns_split_enabled = idb->clns_split_enabled_def;
    } else {
	idb->clns_split_enabled_def = (is_wan(idb->hwptr)) ? FALSE : TRUE;
    }
}

/*
 * clns_router_reload
 *
 * CLNS shutdown (send out messages saying we are dying)
 */

static void clns_router_reload (void)
{
    clns_pdbtype *pdb;
    idbqueuetype *idbq, *idbq_next;

    for (pdb = clns_protoQ.qhead; pdb; pdb = pdb->next) {
	if (clns_pdb_running(pdb)) {
	    for (idbq = pdb->idbqueue; idbq; idbq = idbq_next) {
		idbq_next = idbq->next;
		clns_delete_intfc_router(pdb, idbq->idb);
	    }
	}
    }
}	

/*
 * clns_staticQ_update
 *
 * Add or delete from the static route queue.
 */
void clns_staticQ_update (
    NSAP_T     prefix,
    NSAP_T     next_hop,
    idbtype    *idb,
    hwaddrtype *snpa,
    boolean    sense)
{
    
    clns_staticQtype *entry;

    for (entry = clns_staticQ.qhead; entry; entry = entry->next) {
	if (!nsap_equ(prefix, entry->prefix)) {
	    continue;
	}
	if (!idb) {
	    if (!nsap_equ(next_hop, entry->next_hop)) {
		continue;
	    }
	} else {
	    if (idb != entry->idb) {
		continue;
	    }
	    if (snpa->length != entry->snpa.length) {
		continue;
	    }
	    if (bcmp(snpa->addr, entry->snpa.addr, snpa->length)) {
		continue;
	    }
	}
	break;
    }

    /*
     * Add new entry or update existing entry.
     */
    if (sense) {
	if (!entry) {
	    entry = malloc(sizeof(clns_staticQtype));
	    if (!entry) {
		return;
	    }
	    enqueue(&clns_staticQ, entry);
	}

	/*
	 * Fill in data.
	 */
	entry->idb = idb;
	nsap_copy(prefix, entry->prefix);
	if (!idb) {
	    nsap_copy(next_hop, entry->next_hop);
	} else {
	    bcopy(snpa, &entry->snpa, sizeof(hwaddrtype));
	}
    } else {

	/*
	 * Delete existing entry. If entry does not exist, return quietly.
	 */
	if (!entry) {
	    return;
	}
	unqueue(&clns_staticQ, entry);
	free(entry);
    }
}

/*
 * clns_install_static_route
 *
 * Determine if static route exists for prefix. If so, add to
 * CLNS prefix routing table.
 */
static void clns_install_static_route (NSAP_T prefix)
{
    clns_staticQtype *entry;
    int              route_entry;

    for (entry = clns_staticQ.qhead; entry; entry = entry->next) {
	if (!nsap_equ(prefix, entry->prefix)) {
	    continue;
	}

	route_entry = (!entry->next_hop[0] && !entry->idb) ?
	    CLNS_DISCARD_ENTRY : CLNS_ROUTE_ENTRY;
	(void) clns_route_update(prefix, entry->next_hop, entry->idb,
				 route_entry, ISO_STATIC, ISO_STATIC_DIST,
				 0, timer_no_time, 0,
				 *clns_get_local_npai(NULL));
	return;
    }
}

/*
 * clns_nsap_is_local
 *
 * Returns TRUE, if the nsap address is one for this system.
 */
#ifdef NOBODY_CALLS_ME
boolean clns_nsap_is_local (uchar *nsap)
{
    clns_ddbtype *ddb;
    NSAP_T       net;

    /*
     * Compare NETs with 0 N-selectors.
     */
    nsap_copy(nsap, net);
    net[net[0]] = 0;

    ddb = nsap_lookup(net, MATCH_EXACT);
    if (ddb) {
	if ((ddb->entry_type == CLNS_OURNSAP_ENTRY) ||
	    (ddb->entry_type == CLNS_OURNET_ENTRY)) {
	    return(TRUE);
	}
    }
    return(FALSE);
}
#endif


/*
 * clns_manage_all_prefix_nbrs
 *
 * For each ISO-IGRP created prefix routes, add each prefix to the
 * adjacency database, so IS-IS can include them in its level-2 LSPs.
 *
 * Returns FALSE if no routes were found.
 */
boolean clns_manage_all_prefix_nbrs (
    clns_pdbtype *pdb,
    clns_pdbtype *rpdb,
    boolean      sense)
{
    clns_ddbtype *ddb;
    clns_rdbtype *rdb;
    clns_adjtype *adj, *next;
    int          i;
    boolean      change = FALSE;

    for (i = 0; i < CLNS_NETHASHLEN; i++) {
	for (ddb = clns_routes[i]; ddb; ddb = ddb->next) {
	    if (ddb->entry_type != CLNS_ROUTE_ENTRY) {
		continue;
	    }
	    rdb = ddb->rdb[0];
	    if (!rdb) {
		continue;
	    }

	    /*
	     * Only considered routes created by this (rpdb) ISO-IGRP
	     * process.
	     */
	    if (!(rdb->protocol_source & rpdb->typeflag)) {
		continue;
	    }
	    
	    /*
	     * Don't include ISO-IGRP metric 0 entries. This has already
	     * been done in clns_route_update().
	     */
	    if (!ddb->metric) {
		continue;
	    }

	    if (sense) {
		adj = add_adjacency(clns_adjacency_db, ddb->destination,
				    NULL, NULL, -1, TRUE);
		if (!adj) {
		    return(change);
		}
		isis_mark_L2_changed(pdb);
		adj->protocol_source |= rpdb->typeflag;
		adj->state = CLNS_ADJ_UP_STATE;
		adj->adjacencyType = CLNS_PREFIX_ADJ_TYPE;
		adj->neighbor_pak_fmt = CLNS_PHASE_V_FORMAT;
		if (clns_pdb_routemap(pdb, ddb)) {
		    change = TRUE;
		}
	    } else {
		adj = find_adjacency(clns_adjacency_db, ddb->destination,
				     FALSE, NULL, 0);

		/*
		 * There might exist the same prefix from different routing
		 * processes.
		 */
		for ( ; adj; adj = next) {
		    next = adj->next;
		    if (nsap_equ(adj->key, ddb->destination)) {
			if (adj->protocol_source & rpdb->typeflag) {

			    /*
			     * Don't worry, this doesn't call IS-IS for
			     * each prefix deleted.
			     */
			    delete_adjacency(clns_adjacency_db, adj);
			    isis_mark_L2_changed(pdb);
			    change = TRUE;
			}
		    }
		}
	    }
	}
    }
    return(change);
}

/*
 * clns_refresh_my_net
 *
 * Ensure that clns_myNET is consistent with the set of local NETs.
 */
void clns_refresh_my_net (void)
{
    clns_local_nettype *local_net;

    /* If clns_myNET matches one of the locals, leave it alone. */

    for (local_net = clns_local_nets.qhead;
	 local_net; local_net = local_net->next) {
	if (nsap_equ(clns_myNET, local_net->net))
	    return;
    }

    /* Not there.  Set it to the first one, or blank it if there are none. */

    local_net = clns_local_nets.qhead;
    if (local_net)
	nsap_copy(local_net->net, clns_myNET);
    else
	clns_myNET[0] = 0;
}

/*
 * clns_manage_prefix_nbrs
 *
 * This function determines if a given route/pdb pair should be
 * redistributed into IS-IS. If so, a prefix neighbor adjacency
 * entry is added to the adjacency database. This function also
 * does the delete operation.
 */
void clns_manage_prefix_nbrs (
    clns_ddbtype *ddb,
    int          protocol_source,
    boolean      sense)
{
    clns_pdbtype *pdb;
    clns_adjtype *adj, *next;

    for (pdb = clns_protoQ.qhead; pdb; pdb = pdb->next) {
	if ((pdb->typeflag == protocol_source) ||
	    (!(pdb->typeflag & ISO_ISISMASK)) ||
	    (!(pdb->redistallowed & protocol_source))) {
	    continue;
	}

	if (sense) {

	    /*
	     * Check route-map.
	     */
	    if (!clns_pdb_routemap(pdb, ddb)) {
		continue;
	    }

	    adj = add_adjacency(clns_adjacency_db, ddb->destination, NULL,
				NULL, -1, TRUE);
	    if (!adj) return;
	    isis_mark_L2_changed(pdb);
	    adj->protocol_source |= protocol_source;
	    adj->state = CLNS_ADJ_UP_STATE;
	    adj->adjacencyType = CLNS_PREFIX_ADJ_TYPE;
	    adj->neighbor_pak_fmt = CLNS_PHASE_V_FORMAT;
	    break;
	} else {
	    adj = find_adjacency(clns_adjacency_db, ddb->destination,
				 FALSE, NULL, 0);

	    /*
	     * There might exist the same prefix from different routing
	     * processes.
	     */
	    for ( ; adj; adj = next) {
		next = adj->next;
		if (nsap_equ(adj->key, ddb->destination)) {
		    if (adj->protocol_source & protocol_source) {
			delete_adjacency(clns_adjacency_db, adj);
			isis_mark_L2_changed(pdb);
			break;
		    }
		}
	    }
	}
    }
}

/*
 * fair conversation generator
 *
 * sum of the bytes in the NSAPS
 */
static
short clns_fair_queue_flow_id (paktype *pak)
{
    clnshdrtype  *clns;
    uchar 	*nsap;
    int 	 sum;

    clns = (clnshdrtype *)clnsheadstart(pak);

    if (clns->pid != NLP_ISO8473) {
	return(linktype_is_fq_conversation(pak));
    } else {
	pak->fair_weight = FAIR_QUEUE_BASE_WEIGHT;
	nsap = PAK2ADDR(pak);
	sum = xnethash(nsap, 2 + nsap[0] + nsap[1 + *nsap]);
	return(fair_queue_sum(pak, sum));
    }
}

/*
 * clns_print_network_header
 *
 * Print CLNS header.
 */
static void clns_print_network_header (paktype *pak, ushort **data)
{
    
    clnshdrtype *clns;
    uchar *nsap1;
    uchar *nsap2;

    clns = (clnshdrtype *) clnsheadstart(pak);
    if (clns->pid == NLP_ISO8473) {
	nsap1 = PAK2ADDR(pak);
	nsap2 = nsap1 + *nsap1 + 1;
	printf ("\n  source: %n destination %n", nsap2, nsap1);
    }
}

/*
 * clns_cache_invalidate_caller
 *
 * Structure used to track cache invalidation callers
 *
 * We allocate N+1 of these;  the final is to carry unknown callers.
 */
typedef struct clns_cache_invalidation_caller_ {
    const char *caller_name;		/* Name of caller */
    ulong count;			/* Number of times from this caller */
    sys_timestamp when;			/* Timestamp of last call */
} clns_cache_invalidation_caller;

static clns_cache_invalidation_caller *clns_inval_caller_array;

#define MAX_CLNS_CACHE_INVAL_CALLERS 20	/* Max number of callers we save */

void clns_cache_inval_init (ulong mintime, ulong maxtime,
			    ulong quiettime, ulong quietthresh)
{
    /* Free any existing memory blocks. */
    if (clns_cache_inval_buffer) {
	free(clns_cache_inval_buffer);
	clns_cache_inval_buffer = NULL;
    }

    clns_cache_inval_pending = FALSE;
    clns_cache_inval_recent = 0;
    clns_cache_inval_current = NULL;

    if (maxtime) {		/* Enabled */
	clns_cache_inval_mintime = mintime;
	clns_cache_inval_maxtime = maxtime;
	clns_cache_inval_quiettime = quiettime;
	clns_cache_inval_quietthresh = quietthresh;

	/* Note that things run (albeit crippled) if the malloc fails. */
	clns_cache_inval_buffer = 
	    malloc(clns_cache_inval_quiettime * sizeof(ulong));

	clns_cache_inval_current = clns_cache_inval_buffer;
	clns_cache_inval_recent = 0;
	if (!clns_cache_inval_buffer) {
	    printf("\nCLNS:  Insufficient memory");
	}
	clns_cache_delay_inval = TRUE;
    } else {			/* disabled */
	clns_cache_inval_mintime = clns_cache_inval_maxtime = 0;
	clns_cache_inval_quiettime = clns_cache_inval_quietthresh = 0;
	clns_cache_delay_inval = FALSE;
    }
}

/*
 * nv_clns_cache_write
 *
 * Generate the proper cache invalidation commands in NVRAM
 */
void nv_clns_cache_write (parseinfo *csb)
{
    if (clns_cache_delay_inval) {
	/* If not using defaults, issue the command. */
	if (clns_cache_inval_mintime != CLNS_CACHE_MIN_INVAL_DEFAULT ||
	    clns_cache_inval_maxtime != CLNS_CACHE_MAX_INVAL_DEFAULT ||
	    clns_cache_inval_quiettime != CLNS_CACHE_QUIET_IVL_DEFAULT ||
	    clns_cache_inval_quietthresh != CLNS_CACHE_QUIET_THRESH_DEFAULT){
	    nv_write(TRUE, "%s %d %d %d %d",
		     csb->nv_command,
		     clns_cache_inval_mintime / ONESEC,
		     clns_cache_inval_maxtime / ONESEC,
		     clns_cache_inval_quiettime,
		     clns_cache_inval_quietthresh);
	}
    }

    return;
}

/*
 * clns_cache_is_quiet
 *
 * Return TRUE if CLNS cache has been "quiet" (not too many invalidation
 * requests recently).
 */

static boolean clns_cache_is_quiet (void)
{
    /* Say it's quiet if we're not tracking activity. */
    if (clns_cache_inval_buffer == NULL)
	return(TRUE);

    return (clns_cache_inval_recent <= clns_cache_inval_quietthresh);
}

/*
 * clns_perform_cache_flush
 *
 * Check if we need to purge the CLNS cache, return TRUE if needed. This
 * is called periodically from esis_ager().
 * 
 * The cache is flushed only if no deferred invalidation is in effect.
 * Otherwise, we note the fact that an invalidation request has been made
 * and note the current time if no previous request was pending.  Eventually,
 * the cache will actually be flushed when (a) the minimum interval has
 * passed, and (b) the number of cache invalidation requests has quieted down
 * (defined as a certain number of requests over a certain interval).
 * The invalidation is guaranteed to happen by the time the maximum interval
 * has passed.
 */

boolean clns_perform_cache_flush (void)
{
    /* 
     * If no cache invalidation has been specified, just behave normally.
     */
    if (!clns_cache_delay_inval) {
	return (TRUE);
    }
    /* If we need to blow the cache right away, do it.
     */
    if (clns_cache_purge_immediate) {
	clns_cache_purge_immediate = FALSE;
	return (TRUE);
    }

    /* 
     * No request pending - tell the ager it needn't do anything 
     */
    if (!clns_cache_inval_pending) {
	return (FALSE);
    }
    else {
	/* 
	 * If the max interval has been exceeded, do it now. 
	 */
	if (CLOCK_OUTSIDE_INTERVAL(clns_cache_inval_req, 
				   clns_cache_inval_maxtime)) {
	    clns_cache_inval_pending = FALSE;
	    clns_rbuginf("\nMax time exceeded - cache flush OK");
	    clns_cache_version++;
	    return (TRUE);
	    
	/*
	 * If things have quieted down and the min interval has been
	 * exceeded, do it now.
	 */
	} else
	    if (CLOCK_OUTSIDE_INTERVAL(clns_cache_inval_req,
				       clns_cache_inval_mintime) && 
	        clns_cache_is_quiet()) {
	    clns_cache_inval_pending = FALSE;
	    clns_cache_version++;
	    clns_rbuginf("\nMin time exceeded and cache is quiet "
			  "- cache flush OK");
	    return (TRUE);
	}

    }

    /* Circularly advance the current pointer around the buffer. */
    if (clns_cache_inval_current != NULL) {
	clns_cache_inval_current++;
	if (clns_cache_inval_current >= 
	    (clns_cache_inval_buffer + clns_cache_inval_quiettime)) {
	    clns_cache_inval_current = clns_cache_inval_buffer;
	}

	/* Adjust the running total appropriately. */
	clns_cache_inval_recent -= *clns_cache_inval_current;
	*clns_cache_inval_current = 0;
    }
    
    return (FALSE);
}

/*
 * clns_cache_delay_show
 *
 * Display info about CLNS cache invalidation delay parameters.
 */
void show_clns_cache_delay (void)
{
    if (!clns_cache_delay_inval) {
	printf("\n\nCache invalidation delay feature is disabled\n");
	return;
    }

    printf("\n\nMinimum invalidation interval %d seconds, "
	   "\nMaximum interval %d seconds,",
	    clns_cache_inval_mintime / ONESEC, 
	    clns_cache_inval_maxtime / ONESEC);
    printf("\nQuiet interval %d seconds, \nThreshold %d requests",
	    clns_cache_inval_quiettime,
	    clns_cache_inval_quietthresh);

    if (clns_cache_inval_buffer != NULL) {
        printf("\nInvalidation rate %d in last second",   
	       *clns_cache_inval_current);
	if (clns_cache_inval_quiettime > 1)
	    printf(", %d in last %d seconds", clns_cache_inval_recent,
	           clns_cache_inval_quiettime);
	    printf("\n");
    }

    if (clns_cache_inval_pending) {
	printf("Cache invalidation pending for %d seconds\n",
	       ELAPSED_TIME(clns_cache_inval_req) / ONESEC);
    }
}

/*
 * clns_invalidate_cache
 *
 * Invalidate the CLNS route cache.  Keep track of who's calling, and how
 * often, and when. If caller wants to purge the cache, blow the cache.
 */
void clns_invalidate_cache (const char *caller_name, boolean purge)
{
    int callers_left;
    clns_cache_invalidation_caller *cur_caller;

    /* If there's no caller array, get one. */

    if (!clns_inval_caller_array) {
	clns_inval_caller_array = 
	    malloc_named((MAX_CLNS_CACHE_INVAL_CALLERS + 1) *
			 sizeof(clns_cache_invalidation_caller),
			 "CLNS cache inval");
    }

    /* Track it if the array is there. */

    cur_caller = clns_inval_caller_array;
    if (cur_caller) {
	callers_left = MAX_CLNS_CACHE_INVAL_CALLERS;
	while (callers_left) {
	    if (!cur_caller->caller_name) /* Empty slot */
		break;
	    if (cur_caller->caller_name == caller_name)	/* Name match */
		break;
	    cur_caller++;
	    callers_left--;
	}

	/*
	 * Bump the appropriate slot.  If we didn't find a name match and
	 * the list is full, we will bump the "overflow" slot.
	 */
	if (!callers_left) {		/* Full and no match */
	    caller_name = "Overflow entries";
	}
	cur_caller->caller_name = caller_name;
	cur_caller->count++;
	GET_TIMESTAMP(cur_caller->when);
    }

    /*
     * If we are not doing delayed cache invalidation or if the caller
     * wants to purge the cache, blow the cache away.
     *
     * If we're doing delayed cache invalidation, then just set the 'pending' 
     * flag to TRUE, so that the ager will purge the cache when the time 
     * is right. If we are not doing delayed cache invalidation, the ager
     * will be told that it can purge the cache as soon as the version
     * number is changed. 
     */
    if (!clns_cache_delay_inval || purge) {
	clns_cache_purge_immediate = TRUE;
	clns_cache_version++;
	if (clns_routing_debug)
	    buginf("\nCLNS-RT: cache purged from %s", caller_name);

    } else {
	/* Count the activity. */
	if (clns_cache_inval_current) {
	    (*clns_cache_inval_current)++;
	    clns_cache_inval_recent++;
	}

	/* If no request is pending, note the time. */
	if (!clns_cache_inval_pending) {
	    GET_TIMESTAMP(clns_cache_inval_req);
	    clns_cache_inval_pending = TRUE;
	}

	if (clns_routing_debug)
	    buginf("\nCLNS-RT: delayed cache inval from %s", caller_name);
    }
}

/*
 * clns_bump_cache
 *
 * A wrapper to call clns_invalidate_cache for ATM interfaces. 
 * When VCs go down, the cache entries are out-of-date and hence
 * should be nuked. 
 */
void clns_bump_cache (hwidbtype *hwidb, idbtype *swidb, boolean immediate)
{
    if (is_atm(hwidb) || (swidb && is_atm(swidb->hwptr))) {
	clns_invalidate_cache("unknown", TRUE);
    }
}

/*
 * show_clns_cache_invalidations
 *
 * Displays cache invalidation information.
 */
void show_clns_cache_invalidations (void)
{
    int callers_left;
    clns_cache_invalidation_caller *cur_caller;

    /* Display it if the array is there. */

    cur_caller = clns_inval_caller_array; 
    if (cur_caller) {
	printf("\nCaller              Count    Last Invalidation");
	callers_left = MAX_CLNS_CACHE_INVAL_CALLERS + 1; /* Print overflow */
	while (callers_left) {
	    if (!cur_caller->caller_name) /* Empty slot, all done. */
		break;
	    printf("\n%20s %5d    ", cur_caller->caller_name,
		   cur_caller->count);
	    print_dhms(cur_caller->when);
	    cur_caller++;
	    callers_left--;
	}
    }
}

/*
 * clns_staticpdb_init
 * Allocate static pdb and put on protocol queue.
 */
static clns_pdbtype *clns_staticpdb_init (void)
{
    clns_staticpdb = clns_find_router_name(clns_STATICpname);
    if (clns_staticpdb == NULL) {
        clns_staticpdb = malloc_named(sizeof(clns_pdbtype), "CLNS Static PDB");
        if (clns_staticpdb) {
            clns_staticpdb->name = "static";
            clns_staticpdb->family = PDB_OSI;
            setstring(&clns_staticpdb->pname, clns_STATICpname);
            clns_staticpdb->clns_pid = NO_PROCESS;
            clns_staticpdb->typeflag = ISO_STATIC;
            clns_staticpdb->index = CLNS_PDBINDEX_STATIC;
            enqueue(&clns_protoQ, clns_staticpdb);
        }
    }
    return(clns_staticpdb);
}


/*
 * clns_enable_test
 * Return TRUE if we're running CLNS
 * Called by SERVICE_PROTO_RUNNING.
 */

static boolean clns_enable_test (void)
{
    return(clns_running);
}

/*
 * clns_showproto
 * Interface specific portion of "show protocols" commands
 * Called by SERVICE_SHOWPROTO
 */

static void clns_showproto (idbtype *idb)
{
    if (idb == NULL) {
	if (RUNNING(LINK_CLNS))
	    printf("\n  CLNS routing is enabled (address %n)", clns_myNET);
	return;
    }
    if (RUNNING_IF(LINK_CLNS, idb))
	printf("\n  CLNS enabled");
}

/*
 * clns_routing_idb
 * Return TRUE iff CLNS packets inbound on this interface should be
 * routed.
 */

static boolean clns_routing_idb (idbtype *swidb)
{
    if ((global_crb_enable || global_irb_enable) && 
	swidb->tbridge_on_swidb) {
	/*
	 * For CRB, just return the clns route/bridge indicator
	 * if this interface is in a bridge group.  This is
	 * set by the "bridge <n> route clns" command.
	 */
	return(swidb->tbridge_route_linktypes & LINK_CLNS_RBI);
    } else {
	/*
	 * Otherwise, just verify that clns routing is enabled,
	 * and this interface is enabled for clns.  This latter
	 * is set by the "clns net" command.
	 */
	return(clns_router_running && swidb->clns_enabled);
    }
}

/*
 * clns_bridging_idb
 * Return TRUE iff CLNS packets inbound on this interface should be
 * bridged.
 */

static boolean clns_bridging_idb (idbtype *swidb)
{
    if (global_crb_enable) {
	/*
	 * For CRB, just return the complement of the clns
	 * route/bridge indicator if this interface is in
	 * a bridge group.  This is cleared by the
	 * "no bridge <n> route clns" command (the default).
	 */
	return(swidb->tbridge_on_swidb &&
	       (!(swidb->tbridge_route_linktypes & LINK_CLNS_RBI)));
    } else if (global_irb_enable) {
	return(swidb->tbridge_on_swidb &&
	       !(swidb->tbridge_bridge_linktypes & LINK_CLNS_RBI));
    } else {
	/*
	 * To support SRB and the archaic transparent bridging
	 * behaviour just check for the absence of clns routing.
	 */
	return(!clns_router_running);
    }
}
/*
 * clns_rxtypes_update
 *
 * Maintain the route/bridge state of a protocol's rxtypes.
 * Called whenever protocol-specific configuration changes
 * or transparent-bridging-specific configuration changes
 * are made.  Updates a swidb's rxtype_flags to reflect the
 * new configuration.
 *
 * Determines whether the protocol should be routed and sets/
 * clears rxtypes_flags appropriately.  If Concurrent Routing
 * and Bridging is enabled, the decision is based on the
 * protocol configuration for the given swidb.  Otherwise,
 * the decision is based on the protocol configuration for
 * the router as a whole.
 */

void clns_rxtypes_update (idbtype *swidb)
{
    boolean rte_cn, br_cn;

    if (global_crb_enable || global_irb_enable) {
	rte_cn = clns_routing_idb(swidb);
	br_cn = clns_bridging_idb(swidb);
    } else {
	rte_cn = clns_router_running;
	br_cn = !rte_cn;
    }

    if (rte_cn) {
	SET_ROUTING_RXTYPE(swidb, RXTYPE_CLNS);
	SET_ROUTING_RXTYPE(swidb, RXTYPE_SAP_CLNS);
	SET_ROUTING_RXTYPE(swidb, RXTYPE_JUMBO_CLNP);
    } else {
	CLEAR_ROUTING_RXTYPE(swidb, RXTYPE_CLNS);
	CLEAR_ROUTING_RXTYPE(swidb, RXTYPE_SAP_CLNS);
	CLEAR_ROUTING_RXTYPE(swidb, RXTYPE_JUMBO_CLNP);
    }

    if (br_cn) {
	CLEAR_BRIDGING_RXTYPE(swidb, RXTYPE_CLNS);
	CLEAR_BRIDGING_RXTYPE(swidb, RXTYPE_SAP_CLNS);
	CLEAR_BRIDGING_RXTYPE(swidb, RXTYPE_JUMBO_CLNP);
    } else {
	SET_BRIDGING_RXTYPE(swidb, RXTYPE_CLNS);
	SET_BRIDGING_RXTYPE(swidb, RXTYPE_SAP_CLNS);
	SET_BRIDGING_RXTYPE(swidb, RXTYPE_JUMBO_CLNP);
    }

    if (swidb->hwptr->setup_bridging)
	(*(swidb->hwptr->setup_bridging))(swidb->hwptr);

    reg_invoke_bridge_setup();	/* sse_bridging_setup_enqueue() */
}

/*
 * clnsname_lookup_number
 * Jacket for clns_address_lookup() that converts addrtype to NSAP.
 * Called by SERVICE_NAME_LOOKUP_NUMBER, index is ADDR_CLNS
 */

static nametype *clnsname_lookup_number (addrtype *address)
{
    return (name_cache_lookup(NULL, address, NAM_CLNS));
}

/*
 * clns_parse_address
 * Given a string, parse as a CLNS address
 * Returns TRUE if parse succeeded, addrtype argument setup.
 * Called by SERVICE_PARSE_ADDRESS
 */

static boolean clns_parse_address (char *str, addrtype *address)
{
    if (parse_nsap(str, *((NSAP_T *) &address->clns_addr))) {
	address->type = ADDR_CLNS;
	return(TRUE);
    }
    return(FALSE);
}

/*
 * clnsname_show_hosts
 * Display CLNS host information
 * Called by SERVICE_NAME_SHOW_HOSTS with NAM_CLNS index
 */

static void clnsname_show_hosts (nametype *ptr, char *continue_text)
{
    int i;


    printf(" CLNS ");

    /* 
     * Print out the CLNS style address(es).
     */
    for (i = 0; i < MAXCLNSADDRESSES; i++) {
	if (ptr->v.clns.adr[i].length == 0)
	    break;
	if (ptr->v.clns.adr[i].type != ADDR_CLNS)
	    continue;
	if ((i != 0) && !(i & 1)) {
	    if (automore_quit()) {
		ptr->sharecount--;
		return;
	    }
	    printf(continue_text);
	}
	printf("\n   %n", ptr->v.clns.adr[i].addr.clns_address);
    }
}

/*
 * clnsname_match_number
 * Check if address matches one of addresses in this name cache entry
 * Returns TRUE if match succeeds
 * Called by SERVICE_NAME_MATCH_NUMBER, index is ADDR_CLNS
 */

static boolean clnsname_match_number (nametype *ptr, addrtype *address)
{
    int i;

    for (i = 0; i < MAXCLNSADDRESSES; i++) {
	if (ptr->v.clns.adr[i].length == 0)
	    break;

	if (address->length == STATION_LENGTH) {
	    return(station_match(get_station(
		     ptr->v.clns.adr[i].addr.clns_address),
		     &address->addr.clns_address[0]));
	} else if (nsap_equ(ptr->v.clns.adr[i].addr.clns_address,
			    &address->addr.clns_address[0])) {
	    return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * clns_init_idb
 * initialize the clns fields of the idb
 */

static void clns_init_idb (idbtype *idb, boolean setup_hwidb)
{
    idb->clns_enabled = FALSE;
    clns_rxtypes_update(idb);
    idb->clns_net = NULL;
    idb->clns_cluster_alias = FALSE;
    idb->clns_lineup = TRUE;
    idb->clns_mtu = def_proto_mtu(idb, LINK_CLNS);
    idb->clns_checksum = TRUE;
    mgd_timer_init_leaf(&idb->clns_hello_timer, &esis_hello_timers, 
			ESIS_HELLO_TIMER, idb, FALSE);
    TIMER_STOP(idb->clns_sched_hello_time);
    idb->clns_hello_addr = NULL;
    idb->clns_hello_addrlen = 0;
    idb->clns_hello_type = 0;
    idb->clns_send_rdpdus = TRUE;
    idb->clns_send_rdpdumasks = TRUE;
    idb->clns_rd_interval = CLNS_DEF_RDPDU_INTERVAL;
    TIMER_STOP(idb->clns_rd_timer);
    idb->clns_send_erpdus = TRUE;
    idb->clns_er_interval = CLNS_DEF_ERPDU_INTERVAL;
    TIMER_STOP(idb->clns_er_timer);
    idb->clns_esct_time = 0;
    if (setup_hwidb) {
	idb->hwptr->clns_congestion = CLNS_DEF_CONGESTION;
	idb->hwptr->clns_fastokay = FALSE;
	idb->hwptr->clns_routecache = CLNS_FAST_ROUTECACHE;
    }
    if (is_wan(idb->hwptr)) {
	idb->clns_split_enabled = FALSE;
	idb->clns_split_enabled_def = FALSE;
    } else {
	idb->clns_split_enabled = TRUE;
	idb->clns_split_enabled_def = TRUE;
    }
}

/*
 * clns_register_thyself
 */
static void clns_register_thyself (void)
{
    clns_parser_init();
    clns_media_registry();

    /*
     * Register some function calls
     */
    reg_add_route_adjust(clns_route_adjust, "clns_route_adjust");
    reg_add_clns_discard_route_adjust(clns_discard_route_adjust,
				 "clns_discard_route_adjust");
    reg_add_clns_route_search(clns_route_search, "clns_route_search");
    reg_add_fast_setup(clns_fastsetup, "clns_fastsetup");
    reg_add_encap_fixup(clns_network_fixup, "clns_network_fixup");
    reg_add_send_unreach(LINK_CLNS, clns_send_unreach, "clns_send_unreach");
    reg_add_sw_idb_init(clns_init_idb, "clns_init_idb");
    reg_add_proto_on_swidb(LINK_CLNS, clns_on_idb, "clns_on_idb");
    reg_add_routed_proto_on_swidb(LINK_CLNS, clns_on_idb, "clns_on_idb");
    reg_add_ping_run(LINK_CLNS, clns_ping, "clns_ping");
    reg_add_ping_parse(clns_ping_parse, "clns_ping_parse");
    reg_add_trace_run(LINK_CLNS, clns_trace, "clns_trace");
    reg_add_clear_routes(clear_clns_routecommand, "clear_clns_routecommand");
    reg_add_proto_running(LINK_CLNS, clns_enable_test, "clns_enable_test");
    reg_add_proto_bridging_on_swidb(LINK_CLNS, clns_bridging_idb,
				    "clns_bridging_idb"); 
    reg_add_proto_routing_on_swidb(LINK_CLNS, clns_routing_idb,
				   "clns_routing_idb");
    reg_add_rxtypes_update(clns_rxtypes_update,"clns_rxtypes_update");
    reg_add_showproto(clns_showproto, "clns_showproto");
    reg_add_printf( 'n', nsap_printf, "nsap_printf");
    reg_add_setup_global(clns_setup_global, "clns_setup_global");
    reg_add_setup_int(clns_setup_int, "clns_setup_int");
    reg_add_setup_end(clns_setup_end, "clns_setup_end");
    reg_add_parse_address(ADDR_CLNS, clns_parse_address, "clns_parse_address");
    reg_add_name_lookup_number(ADDR_CLNS,
		      clnsname_lookup_number, "clnsname_lookup_number");
    reg_add_name_show_hosts(NAM_CLNS,
		      clnsname_show_hosts, "clnsname_show_hosts");
    reg_add_name_match_number(ADDR_CLNS,
		      clnsname_match_number, "clnsname_match_number");
    reg_add_ip_clns_filter_create(clns_filter_create, "clns_filter_create");
    reg_add_ip_get_filter_index(get_filter_index, "get_filter_index");
    reg_add_ip_clns_filter_destroy(clns_filter_destroy, "clns_filter_destroy");
    reg_add_ip_filter_name(filter_name, "filter_name");
    reg_add_ip_route_map_inform(clns_route_map_inform,
				"clns_route_map_inform");
    reg_add_ip_clns_find_router_name(clns_find_router_name,
				     "clns_find_router_name");
    reg_add_ip_clns_igrp_pdb_initialize(clns_igrp_pdb_initialize,
					"clns_igrp_pdb_initialize");
    reg_add_ip_clns_staticpdb(clns_staticpdb_init, "clns_staticpdb_init");
    reg_add_ip_clns_delete_router(clns_delete_router, "clns_delete_router");
    reg_add_ip_clns_distance(clns_distance, "clns_distance");
    reg_add_ip_clns_nv_write_redist(clns_nv_write_redist,
				    "clns_nv_write_redist");
    reg_add_ip_clns_redistribute(clns_redistribute, "clns_redistribute");
    reg_add_ip_clns_basic_timers(clns_basic_timers, "clns_basic_timers");
    reg_add_ip_clns_metric_command(clns_metric_command, "clns_metric_command");
    reg_add_ip_clns_redistribute_level_nvgen(ip_clns_redistribute_level_nvgen,
					     "ip_clns_redistribute_level_nvgen"
					     );
    reg_add_determine_fair_queue_flow_id (LINK_CLNS, clns_fair_queue_flow_id,
				   "clns_fair_queue_flow_id");
    reg_add_print_network_header(LINK_CLNS, clns_print_network_header,
				 "clns_print_network_header");
    reg_add_ip_clns_redistribute_level_printf(
	ip_clns_redistribute_level_printf,
       	"ip_clns_redistribute_level_printf");
    reg_add_ip_clns_redistribute_metric_nvgen(
	ip_clns_redistribute_metric_nvgen,
	"ip_clns_redistribute_metric_nvgen");
    reg_add_ip_clns_redistribute_metric_printf(
	ip_clns_redistribute_metric_printf,
	"ip_clns_redistribute_metric_printf");
    reg_add_fs_cache_update(LINK_CLNS, clns_add_cache_rewrite,
			    "clns_add_cache_rewrite");
    reg_add_will_reload(clns_router_reload, "clns_router_reload");
    reg_add_clns_delete_igrp_adj(clns_delete_igrp_adj, "clns_delete_igrp_adj");
    reg_add_clns_adjust_static_routes(clns_adjust_static_routes,
				      "clns_adjust_static_routes");
    reg_add_clns_show_protocol_source(clns_show_protocol_source,
				      "clns_show_protocol_source");
    reg_add_clns_filter_addr(clns_filter_addr, "clns_filter_addr");
    reg_add_clns_clear_neighbors(clear_clns_neighbors, "clear_clns_neighbors");
    reg_add_clns_invalidate_cache(clns_invalidate_cache,
				  "clns_invalidate_cache");
    reg_add_bump_cache_version(clns_bump_cache, "clns_bump_cache");
    reg_add_clns_pak_duplicate(clns_pak_duplicate, "clns_pak_duplicate");
    reg_add_isis_mark_L1_changed(isis_mark_L1_changed, "isis_mark_L1_changed");
    register_snmp_clns_echo();
}

/*
 * clns_init
 * Initialize ISO stuff.
 */

static void clns_init (subsystype *subsys)
{
    idbtype *idb;
    int     i;

    /*
     * Create the adjacency database.  If there isn't enough memory,
     * don't even bother running CLNS.
     */
    if (!clns_adjacency_db) {
	clns_adjacency_db = 
	    clns_create_adjacency_db("CLNS adjacency database");
	if (!clns_adjacency_db)
	    return;
    }
    clns_register_thyself();

    /*
     * Register OSICP with PPP
     */
    reg_invoke_ncp_register(LINK_CLNS, TYPE_PPP_OSICP,
			    NULL, NULL, "OSICP");
    reg_invoke_ncp_register_link(LINK_CLNS_ALL_ES, 0, LINK_CLNS);
    reg_invoke_ncp_register_link(LINK_CLNS_ALL_IS, 0, LINK_CLNS);
    reg_invoke_ncp_register_link(LINK_ISIS_ALL_L1_IS, 0, LINK_CLNS);
    reg_invoke_ncp_register_link(LINK_ISIS_ALL_L2_IS, 0, LINK_CLNS);
    reg_invoke_ncp_register_link(LINK_CLNS_BCAST, 0, LINK_CLNS);

    /* Set up debugging flags */
    clns_debug_init();

    clns_running = FALSE;
    clns_router_running = FALSE;
    clns_processes_setup = FALSE;
    clns_security_passthru = FALSE;     /* by default, do not forward packets
					   with security options */
    clns_nnets = 0;			/* no interfaces yet */
    clear_nsap(clns_myNET);
    for (i = 0; i < CLNS_MAXTRAFFIC; i++) {
	clns_traffic[i] = 0L;
    }
    clnsQ = NULL;
    queue_init(&esisQ, 0);
    queue_init(&clns_echoQ, 0);
    mgd_timer_init_parent(&esis_timer, NULL);
    mgd_timer_init_leaf(&esis_ager_timer, &esis_timer, ESIS_AGER_TIMER, NULL,
			FALSE);
    mgd_timer_init_parent(&esis_hello_timers, &esis_timer);
    clns_config_timer = CLNS_DEF_CONFIG_TIMER;
    clns_holding_timer = CLNS_DEF_HOLDING_TIMER;
    clns_esct_timer = 0;
    clns_packet_lifetime = CLNS_DEF_LIFETIME;
    FOR_ALL_SWIDBS(idb) {
	clns_rxtypes_update(idb);
        clns_init_idb(idb, TRUE);
    }

    /*
     * Create a dummy adjacency that we use as an ES to multicast out our
     * interface (when we haven't seen any IS's, and we don't have an
     * ESH from the guy).
     */
    dummy_adj.adjacencyType = CLNS_ES_ADJ_TYPE;
    dummy_adj.snpa.type = STATION_IEEE48;	/* this is just a guess */
    dummy_adj.snpa.length = 0;
    dummy_adj.snpa.target_type = TARGET_OSI_ALL_ES;

    /* 
     * dummy_adj.idb filled in when an interface is enabled, and
     * also refreshed whenever we hear an ESH
     */
    all_ess.type = STATION_ILLEGAL;
    all_ess.length = 0;
    all_ess.target_type = TARGET_OSI_ALL_ES;
    all_iss.type = STATION_ILLEGAL;
    all_iss.length = 0;
    all_iss.target_type = TARGET_OSI_ALL_IS;
    clns_want_erpdus = TRUE;

    /*
     * esis_timer initialized when esis process started
     */
    clns_cache_version = 1;
    clns_cache_changes = 0;
    for (i = 0; i < CLNS_NETHASHLEN; i++) {
	clns_routes[i] = NULL;
    }
    
    /*
     * Initialize static pdb
     */
    clns_staticpdb_init();
    
    /*
     * Initialize cache delay invalidation stuff..
     */
    clns_cache_inval_buffer = NULL; /* Make sure it's clean */
    clns_cache_inval_init(CLNS_CACHE_MIN_INVAL_DEFAULT, 
		          CLNS_CACHE_MAX_INVAL_DEFAULT,
		          CLNS_CACHE_QUIET_IVL_DEFAULT, 
		          CLNS_CACHE_QUIET_THRESH_DEFAULT);
    clns_cache_purge_immediate = FALSE;
}

/*
 * clns_route_adjust
 *
 * An interface has gone down or up. Inform OSI routing protocols.
 */
void clns_route_adjust (idbtype *idb)
{

    boolean was_up, now_up;
    clns_pdbtype *isis_pdb;

    /* Inform ISIS regardless of previous state--ISIS tracks this itself. */

    isis_pdb = find_isis_pdb();
    if (isis_pdb) {
	isis_track_interface_state(isis_pdb, idb);
    }

    was_up = idb->clns_lineup;
    now_up = interface_up(idb);

    if (!idb->clns_enabled) {
	idb->clns_lineup = now_up;
	return;
    }

    /*
     * No state change.
     */
    if (was_up && now_up)
	return;
    if (!was_up && !now_up)
	return;

    /*
     * Record new state.
     */
    idb->clns_lineup = now_up;

    clns_rbuginf("\nCLNS-RT: Interface %s is %s", idb->namestring, 
		 (now_up) ? "up" : "down");

    /*
     * Inform routing protocols.
     */
    clns_remove_idb_depend(idb);
    esis_route_adjust(idb);

    /*
     * Adjust the static routes pointing to that interface.
     *
     * If ISIS-IP is running, delay the building of LSP to avoid
     * generating the LSPs twice. (That would result in the
     * ratelimiting of the 2nd LSP and delayed convergence).
     *
     * If there is a need to rebuild the LSP, set the appropriate
     * flag.  if the int_state_change_timer is not running set it to
     * run after a second. 
     */

    if (isis_pdb && isis_pdb->ip_pdb) {
	if (clns_adjust_static_routes(NULL, idb, now_up, FALSE)) {
	    isis_schedule_lsp_build(isis_pdb, CLNS_LEVEL2,
				    ISIS_IP_LSP_BUILD_DELAY, FALSE);
	    locupdate_buginf(isis_pdb, "L2 route adjust");
	}
    } else {
	clns_adjust_static_routes(NULL, idb, now_up, TRUE);
    }
    if (isis_spf_statistics_debug)
	buginf("\nISIS-Stats: CLNS int down");
    clns_invalidate_cache("clns_route_adjust", TRUE);
}

/*
 * clns_xs_sendnet
 * code common to ES and IS which sends a datagram.  
 *	idb is the interface
 *	snpa is the snpa type, length, & address, (NULL if broadcast)
 * 	next_hop_addr is the NET we're sending it to.
 * return FALSE and send an ER PDU if we believe we couldn't send it
 */

boolean clns_xs_sendnet (paktype *pak, clns_adjtype *adj)
{
    clnshdrtype *clns;
    idbtype *idb;
    paktype *copy;
    NSAP_T nsap;

    clns = (clnshdrtype *)clnsheadstart(pak);
    idb = pak->if_output;
    if (!idb) {
	errmsg(&msgsym(NULLIDB, SYS), "clns_xs_sendnet");
	goto sendfailed;
    }

    /*
     * Filter the packet if appropriate.
     */

    if (idb->clns_filter_out) {
	if (!clns_filter_addr(idb->clns_filter_out, 
			      (nsap_addr *) PAK2ADDR(pak), /* Dest addr */
			      (nsap_addr *) pak->clns_srcaddr)) {
	    goto sendfailed;
	}
    }

    /*
     * If the neighbor wants DECnet formatted packets, hand to DECnet.
     * Do not verify that destination address is valid.
     */
    if (adj->neighbor_pak_fmt == CLNS_PHASE_IV_FORMAT) {
	if (reg_invoke_decnet_convert(LINK_CLNS, pak, NULL, FALSE))
	    return(TRUE);
	datagram_done(pak);
	return(FALSE);
     }

 again:
    if (GETSHORT(clns->clns_seglen) <= (idb->clns_mtu)) { /* need to segment? */
	
	if (!interface_up(idb))
	    goto sendfailed;

	/*
	 * Total length less than interface MTU.  No segmentation needed.
	 */

	/*
	 * Special case for P2P interface. Use the next hop
	 * NSAP instead of an SNPA, so that the dialer map look up
	 * will succeed.
	 */
	if (is_p2p(idb))
	    BUILD_NSAP(adj, nsap);
	    
	if (!clns_encaps(pak, GETSHORT(clns->clns_seglen), idb,
			 &adj->snpa, (is_p2p(idb) ? nsap : NULL))) {

	    /*
	     * Encapsulation failed.
	     */
	    clns_traffic[CLNS_ENCAPSFAILED]++; /* encapsulation failed */
	    clns_pbuginf("\nCLNS encaps failed on %s for dst= %s",
			 idb->namestring,
			 nsap_string(pak->clns_nexthopaddr));
	    goto sendfailed;
	}

	/*
	 * Count the packets we are generating
	 */	
	if (!pak->if_input)
	    clns_traffic[CLNS_OUTPUT]++;
	if (!clns->ms && pak->clns_segpart &&
	    GETSHORT(((clns_segpart_t *)pak->clns_segpart)->segoffset)) {
	    clns_traffic[CLNS_SEGMENTED]++;
	}

	datagram_out(pak);
	clns_traffic[CLNS_OUTPUT]++;

	return(TRUE);
    }
     
    /*
     * The packet is larger than the interfaces mtu.  We must frag it.
     */
    if (clns->sp && pak->clns_segpart) {	/** are we allowed to segment? */
	pak->if_output = idb;
	copy = clns_segment(pak, adj); /* yes, do so */
	if (copy) {
	    datagram_done(pak);
	    pak = copy;
	    clns = (clnshdrtype *)clnsheadstart(pak);
	    goto again;
	} else {
	    clns_traffic[CLNS_SEGFAILED]++;
	}
    } else {
        clns_traffic[CLNS_SEGFAILED]++;	/* no, count a failure */
    }
     
    /*
     * Come here when something went wrong.
     */
sendfailed:
    datagram_done(pak);
    return(FALSE);

}

/*
 * clns_route
 *
 * This routine is the heart of the routing (forwarding) algorithm.
 *
 */
clns_adjtype *clns_route (NSAP_T destination, int depth)
{
    clns_ddbtype       *ddb;
    clns_adjtype       *adj;
    clns_pdbtype       *pdb;
    clns_cache_adjtype *cache_adj;

    /*
     * Limit amount of routing table lookup recursion.
     */
    if (!depth) {
	return(NULL);
    }
    depth--;

    /*
     * If destination is in cache, don't go through slower route
     * lookup.
     */
    cache_adj = nsap_cache_search(destination);
    if (cache_adj) {
	cache_adj->uses++;
	GET_TIMESTAMP(cache_adj->last_used);
	return(cache_adj->adj);
    }

    /*
     * If the destination is in an area configured for one of the
     * routing processes, we're routing via level-1. Call the respective
     * routing protocol to lookup route in it's level-1 forwarding table.
     */
    adj = NULL;
    pdb = clns_find_pdb(destination);
    if (pdb) {
	if ((pdb->typeflag & ISO_ISISMASK) && (pdb->level & ISIS_CIRCUIT_L1)) {
	    adj = isis_route(pdb, destination);
	    if ((pdb->level == ISIS_CIRCUIT_L1) || adj)
		return(adj);
	} else if (pdb->typeflag & ISO_IGRPMASK) {
	    adj = iso_igrp_route(pdb, destination, depth);

	    /*
	     * If not in ISO-IGRP routing tables, try prefix routes.
	     */
	    if (adj) {
		return(adj);
	    }
	}
    } else {
	
	/*
	 * Destination is not in any of our areas. If IS-IS is running,
	 * get closest level-2 router. The closest level-2 router is built 
	 * only when the system is a level-1 only router.
	 *
	 * NOTE: this will have to change when multiple IS-IS processes are 
	 * supported.
	 */
	pdb = find_isis_pdb();
	if (pdb && pdb->isis_best_L2) {
	    adj = isis_make_route_cache(destination, pdb->isis_best_L2);
	    if (adj) {
		return(adj);
	    }
	}
    }

    /*
     * Do level-2 routing by performing a best match lookup in
     * the CLNS prefix routing table.
     */
    ddb = nsap_lookup(destination, MATCH_CLOSEST);
    if (ddb) {
	adj = clns_get_route(destination, ddb, depth);
    }

    /*
     * Last shot, try the adjacency database, the destination might
     * be directly connected.
     */
    if (!adj) {
	adj = find_adjacency(clns_adjacency_db, destination, TRUE, NULL, 0);
	if (adj) {

	    /*
	     * If adjacency found, build cache entry.
	     */
	    clns_make_route_cache(destination, adj);
	}
    }

    /*
     * Well, one more shot.  If we are an ES, try picking up the default IS
     * as the next hop.
     */
    if (!adj && !clns_router_running)
	adj = clns_get_default_is_adj();

    return(adj);
} 

/*
 * clns_get_route
 *
 */
clns_adjtype *clns_get_route (NSAP_T destination, clns_ddbtype *ddb, int depth)
{
    clns_rdbtype   *rdb;
    clns_adjtype   *adj, *save_adj;
    clns_pdbtype   *pdb;
    clns_cachetype *cache;
    int            i;
    boolean        nsap_format;

    if (ddb->entry_type == CLNS_DISCARD_ENTRY) {
	return(NULL);
    }
    if (ddb->entry_type == CLNS_OURNSAP_ENTRY ||
	ddb->entry_type == CLNS_OURNET_ENTRY) {
	return((clns_adjtype *) -1);
    }

    cache = NULL;
    save_adj = NULL;
    nsap_format = FALSE;

    for (i= 0; i < ddb->routecount; i++) {
	rdb = ddb->rdb[i];
        if (!rdb)
	    continue;

	if (rdb->idb && !interface_up(rdb->idb))
	    continue;

	if (rdb->protocol_source & ISO_ISISMASK) {
	    pdb = find_isis_pdb();
	    if (!pdb) {
		continue;
	    }
	    if (pdb->current_L2_version != rdb->route_version) {
		continue;
	    }
	} else if (rdb->protocol_source & ISO_IGRPMASK) {
	    if (TIMER_RUNNING_AND_AWAKE(rdb->expiration_time)) {
		continue;
	    }
	} else if (rdb->protocol_source & ISO_STATIC) {
	    if (!nsap_equ(ddb->destination, rdb->first_hop)) {

		/*
		 * This check allows for static routes to use dynamically 
		 * derived adjacencies. 
		 */
		if (rdb->first_hop[0] != STATION_LENGTH) {
		    nsap_format = TRUE;
		}
	    }
	}

	adj = find_adjacency(clns_adjacency_db, rdb->first_hop,
			     nsap_format, rdb->idb, 0);

	/*
	 * If next-hop is not directly connected, do a routing table
	 * lookup on next-hop (recursively).
	 */
	if (!adj) {
	    adj = clns_route(rdb->first_hop, depth);
	}

	/*
	 * Adjacency found, build cache.
	 */
	if (adj) {
	    if ((int) adj != -1) {
		if (!save_adj) {
		    save_adj = adj;
		}
		if (!i) {
		    cache = clns_make_route_cache(destination, adj);
		} else {
		    clns_make_cache_adj(cache, adj);
		}
	    }
	}
    }

    return(save_adj);
}

/*
 * clns_write
 *
 * 'send a clns datagram' (that we're sourcing). Call clns_route to figure 
 * out the first hop.
 *
 * Note that the dstaddr field need to be filled in BEFORE clns_write is 
 * called.
 *
 * (this should be generalized eventually to handle the same stuff
 * as ip_write: re-used packets, security, broadcast, post-route options)
 */

boolean clns_write (paktype *pak)
{
    clns_adjtype *adj;
    clnshdrtype *clns;

    clns = (clnshdrtype *)clnsheadstart(pak);

    adj = clns_route(pak->clns_dstaddr, ROUTINGDEPTH);

    if (adj) {
	if ((int) adj == -1) {
	    clns_local_enqueue(pak);
	    return(TRUE);
	}
	pak->clns_nexthopaddr = adj->key;
	pak->if_output = adj->idb;
	clns_pbuginf("\nCLNS: Originating packet, size %d", 
		     GETSHORT(clns->clns_seglen));
	clns_pbuginf("\n      from %n\n      to %n\n      via %s (%s %s)",
		     pak->clns_srcaddr, pak->clns_dstaddr,
		     (*pak->clns_nexthopaddr == STATION_LENGTH) ?
		     station_string(&pak->clns_nexthopaddr[1]) :
		     nsap_string(*(NSAP_T *)pak->clns_nexthopaddr),
		     pak->if_output->namestring,
		     snpa_string(&adj->snpa));

	/*
	 * Checksum the PDU header if configured to generate checksums for
	 * locally originated packets.
	 */
	if (adj->idb->clns_checksum)
	    compute_iso_checksum(clnsheadstart(pak),
				 clns->hlen,
				 clns->checksum);
	else {
	    clns->checksum[0] = 0;
	    clns->checksum[1] = 0;
	}	

	return(clns_xs_sendnet(pak, adj));
    } else {
	/*
	 * Destination was not in routing table.
	 * Does DECnet want this?  Verify destination address
	 */
	if (reg_invoke_decnet_convert(LINK_CLNS, pak, NULL, TRUE))
	    return(TRUE);
	clns_dispose(pak, CLNS_DISC_UNKNOWNDST,0);
	datagram_done(pak);
	return(FALSE);
    }
}

/*
 * clns_local_data_enqueue
 * Find a home for a local data PDU.
 */

static
void clns_local_data_enqueue (paktype *pak)
{
    int selector;

    selector = get_selector(*(NSAP_T *) pak->clns_dstaddr);
    switch (selector) {
    case NSP:
	reg_invoke_decnet_convert(LINK_CLNS, pak, NULL, FALSE);
	break;
    case TARP_DEF_NSEL_TYPE:
	/*
	 * Discard packet if TARP is not running.
	 */
	if (reg_invoke_tarp_running_status())
	    reg_invoke_tarp_enqueue(pak);
	else {
	    clns_pbuginf("\nCLNS: TARP packet received, but TARP not "
	                 "running - discarding packet");
	    datagram_done(pak);
	}
	break;
    default:
	clns_ebuginf("\nCLNS: non-supported N-selector = %d, received",
		     selector);
	datagram_done(pak);
    }	
}

/*
 * clns_local_enqueue
 * PDU has local destination, find it a home or drop it
 */

void clns_local_enqueue (paktype *pak)
{
    clnshdrtype *clns;

    clns = (clnshdrtype *)clnsheadstart(pak);

/* *** LATER *** do packet reassembly */

    switch ((int)clns->type) {
      case CLNS_TYPE_ERROR:
	clns_traffic[CLNS_ERSRCVD]++;
	if (pak->if_input) {
	    clns_pbuginf("\nCLNS: ER PDU received on %s!",
			 pak->if_input->namestring);
	}
	if (clns_packet_debug || clns_event_debug)
	    clns_print(clns);

	clns_error_pdu(pak);
	datagram_done(pak);
	break;
      case CLNS_TYPE_DATA:
	clns_traffic[CLNS_LOCAL]++;
	clns_ebuginf("\nCLNS: DT PDU received on %s address to us!",
		    pak->if_input->namestring);
	if (clns_packet_debug || clns_event_debug)
	    clns_print(clns);
	clns_local_data_enqueue(pak);
	break;
      case CLNS_TYPE_ERQ:
	clns_traffic[CLNS_ECHOREQIN]++;
	clns_echo(pak);
	datagram_done(pak);
	break;
      case CLNS_TYPE_ERP:
	clns_traffic[CLNS_ECHOREPIN]++;
	clns_echo_rep(pak);
	datagram_done(pak);
	break;
      default:
	clns_ebuginf("\nCLNS: PDU for unknown ULP: %i", (int)clns->type);
	clns_dispose(pak, CLNS_DISC_UNKNOWNDST, 5);
	datagram_done(pak);
	break;
    }
}

/*
 * clns_forward
 * Forward a CLNS datagram to the next hop.
 * 
 * Note that this routine is only called for CLNS packets which we
 * are switching. (Not any packets which we source).
 */

void clns_forward (paktype *pak, clns_adjtype *adj)
{
    clnshdrtype *clns;
    paktype     *rdpak;
    char        *str;
    uchar       *ptr;
    uchar       nsel_byte;
    clns_adjtype *src_adj;
    boolean handle_cluster_alias;

    clns = (clnshdrtype *)clnsheadstart(pak);
    src_adj = find_adjacency(clns_adjacency_db, pak->clns_srcaddr, TRUE,
			     pak->if_input, 0);

    /* 
     * If packet needs to be segmented, but segmentation is not permitted
     * send out an ERPDU. This applies only if the adjacency is not a Phase IV
     * one.
     */
    if (GETSHORT(clns->clns_seglen) > ((adj->idb)->clns_mtu)) {
        if ((adj->neighbor_pak_fmt != CLNS_PHASE_IV_FORMAT) && !clns->sp) {
	    clns_dispose(pak,CLNS_DISC_SEGMENT,0);
	    datagram_done(pak);
	    return;
	}
    }
    clns_traffic[CLNS_FORWARD]++;
    if (clns->lifetime <= 1) {
	if (!src_adj || (src_adj->neighbor_pak_fmt != CLNS_PHASE_IV_FORMAT)) {
	    clns_traffic[CLNS_LIFETIME]++;
	    clns_dispose(pak,CLNS_DISC_HOPS,4);
	    if (clns_packet_debug)
		buginf("\nCLNS: Life time exceeded (%u)", clns->lifetime);
	} else {
	    if (clns_packet_debug)
		buginf("\nCLNS: Life time exceeded for converted packet(%u)",
		       clns->lifetime);
	}
	datagram_done(pak);
	return;
    }
    clns->lifetime--;

    pak->if_output = adj->idb;
    if (pak->clns_flags & (CLNS_OPT_HASRRT | CLNS_OPT_HASGQOS)){
	if (!clns_opt_postroute(pak)) {
	    datagram_done(pak);
	    return;
	}
    }

    /*
     * Adjust checksum if one was supplied.
     */
    if (GETSHORT(clns->checksum)) {
	ptr = clns->checksum;
	ADD_MOD255(*ptr, 5, 0);
	ptr++;
	SUB_MOD255(*ptr, 4, 0);
    }
    
    /*
     * Print "clns host" names if applicable - only executed when 
     * "debug clns-packets".
     */
    if (clns_packet_debug) {
	buginf("\nCLNS: Forwarding packet size %d", pak->datagramsize);

	str = clns_address_lookup(*(NSAP_T *)pak->clns_srcaddr);
        buginf("\n      from %n %s", pak->clns_srcaddr, str);

	str = clns_address_lookup(*(NSAP_T *)pak->clns_dstaddr);
	buginf("\n      to %n %s", pak->clns_dstaddr, str);

	if (*pak->clns_nexthopaddr == STATION_LENGTH) {
	    str = clns_stationid_lookup(NULL, &pak->clns_nexthopaddr[1]);
	} else {
	    str = clns_address_lookup(*(NSAP_T *)pak->clns_nexthopaddr);
	    if (!str) {
		str = nsap_string(*(NSAP_T *)pak->clns_nexthopaddr);
	    }
	}
	buginf("\n      via %s (%s %s)", str,
	       pak->if_output->namestring, snpa_string(&adj->snpa));
    }

    /*
     * If we are sending this packet out the same interface
     * it came in, send a redirect to the source only if attached
     * to interface. Note that the redirect will always be sent AFTER
     * the forwarded packet.
     *
     * If converting a Phase V packet to Phase IV or Phase IV to Phase V
     * do not send ES-IS redirects.
     */
    rdpak = NULL;
    if ((pak->if_output == pak->if_input) && pak->addr_start &&
	(adj->neighbor_pak_fmt != CLNS_PHASE_IV_FORMAT)) {
	nsel_byte = pak->clns_dstaddr[pak->clns_dstaddr[0]];
	handle_cluster_alias = (nsel_byte == CLNS_CLUSTER_ALIAS_NSEL1) ||
			       (nsel_byte == CLNS_CLUSTER_ALIAS_NSEL2);
	if (handle_cluster_alias) {
	    clns_pbuginf("\nReceived packet with cluster alias NSEL (%x)",
                         nsel_byte);
	}
	if (!src_adj || 
	    (src_adj->neighbor_pak_fmt != CLNS_PHASE_IV_FORMAT) ||
	     handle_cluster_alias)
	    rdpak = clns_send_redirect(pak, adj);
    }

    clns_xs_sendnet(pak, adj);

    if (rdpak) {
	datagram_out(rdpak);
	clns_traffic[CLNS_OUTPUT]++;
	clns_traffic[ESIS_RDUSSENT]++;
    }

}


/*
 * clns_switch
 *
 *    This routine is called whenever a clns packet comes in
 *    It decides if the packet is for us or if we need to switch it
 *    Note that all of our NETs and NSAPs must be stored in the
 *    appropriate database so that clns_route can find them.
 *    
 *    pak is always consumed by this routine
 */
static void clns_switch (paktype *pak)
{
    clnshdrtype  *clns;
    clns_adjtype *adj;
    NSAP_T       dest_nsap;
    int          nsap_length;
  
    clns = (clnshdrtype *)clnsheadstart(pak);

    /*
     * Filter the packet, if appropriate.
     */
    if (pak->if_input->clns_filter_in) {
	if (!clns_filter_addr(pak->if_input->clns_filter_in, 
			      (nsap_addr *) PAK2ADDR(pak), /* Dest addr */
			      (nsap_addr *) pak->clns_srcaddr)) {
	    datagram_done(pak);
	    return;
	}
    }
	    
    /*
     * Coerce the selector field to be zero before looking in the database.
     */
    nsap_length = pak->clns_dstaddr[0];
    nsap_copy(pak->clns_dstaddr, dest_nsap);
    dest_nsap[nsap_length] = 0;
 
    adj = clns_route(dest_nsap, ROUTINGDEPTH);
    if (adj) {
	if ((int) adj == -1) {

	    /*
	     * This packet is for us. Most of the error checking
	     * will be done later but check the lifetime field here.
	     */
	    if (clns->lifetime == 0){
		clns_traffic[CLNS_LIFETIME]++;
		clns_dispose(pak,CLNS_DISC_HOPS,4);
		datagram_done(pak);
	    } else {
		clns_local_enqueue(pak);
	    }
	} else {

	    /*
	     * This packet is for someone else.
	     * If we are a router and we know the route, forward it.
	     */
	    if (clns_router_running) {
		pak->clns_nexthopaddr = adj->key;
		if (adj->idb->clns_enabled ||
		    adj->neighbor_pak_fmt == CLNS_PHASE_IV_FORMAT) {
		    clns_forward(pak, adj);
		} else {
		    /*
		     * if interface not enabled notify user and discard.
		     */
 		    clns_traffic[CLNS_NOROUTE]++;
 		    clns_dispose(pak,CLNS_DISC_UNKNOWNDST,0);
 		    datagram_done(pak);
 		}
	    } else {
		datagram_done(pak);
	    }
	}
    } else {
	/*
	 * Does DECnet want this?  Verify destination address
	 */
	if (reg_invoke_decnet_convert(LINK_CLNS, pak, NULL, TRUE))
	    return;
	clns_traffic[CLNS_NOROUTE]++;
	clns_dispose(pak, CLNS_DISC_UNKNOWNDST, 0);
	datagram_done(pak);
    }
}

/*
 * clns_input
 * Handle CLNS (ISO IP) input
 */

process clns_input (void)
{
    paktype          *pak;
    clnshdrtype      *clns;
    clns_pdbtype     *pdb;
    iso_igrp_hdrtype *iso_igrp;
    int              spot;	/* spot in header of error */
    int              pak_count;

    process_wait_on_system_init();

    /*
     * Set up this process' world.
     */
    clnsQ = create_watched_queue("CLNS Input", 0, 0);
    process_watch_queue(clnsQ, ENABLE, RECURRING);
    reg_add_raw_enqueue(LINK_CLNS, clns_enqueue, "clns_enqueue");

    while (TRUE) {
	process_wait_for_event();

	pak_count = PAK_SWITCHCOUNT;

	while (TRUE) {
            /*
             * Process only a limited number of packets per pass
             */
            if (pak_count-- == 0) {
                break;
            }

	    /*
	     * Dequeue a CLNS (ISO IP) datagram
	     * Note that all ISO packets are actually queued here.
	     * This includes ESIS, ISO_IGRP and ISO8473 (clns) pids
	     */
	    pak = process_dequeue(clnsQ);
	    if (!pak) {
		break;
	    }

	    clns = (clnshdrtype *)clnsheadstart(pak);

	    if (!interface_up(pak->if_input)) {
		datagram_done(pak);
		continue;
	    }

	    pak->clns_nexthopaddr = pak->clns_dstaddr = 
		pak->clns_srcaddr = pak->clns_segpart = 
		    pak->clns_optpart =  pak->clns_datapart = 
			pak->clns_qos = NULL;

	    clns_traffic[CLNS_INPUT]++;

	    idb_increment_rx_proto_counts(pak, ACCT_PROTO_CLNS);
	    
	    if (pak->if_input->hwptr->enctype == ET_PPP) {
		clns_ppp_pad(pak);
	    }

	    switch (clns->pid) {
	    case NLP_ESIS:

		/*
		 * Note that ESIS does not call set_pointers
		 * this may cause grief elsewhere ???
		 * Note also that esis must check the packet
		 * for validity
		 */
		enqueue(&esisQ, pak);
		break;

	    case NLP_ISO_IGRP:
		iso_igrp = (iso_igrp_hdrtype *)iso_igrp_headstart(pak);
		pdb = clns_find_pdb (iso_igrp->source_net);
		if (pdb && (pdb->typeflag & ISO_IGRPMASK)) {
		    if (itf_in_pdb (pak->if_input, pdb)) {
			enqueue(&pdb->clns_inputq, pak);
			break;
		    } else {

			/*
			 * Try another pdb, for the same domain, to handle 
			 * the case were a level-1 pdb is not for this 
			 * interface but a level-2 might be.
			 */
			if (pdb->level == CLNS_LEVEL1) {
			    pdb = clns_find_level2_pdb(iso_igrp->source_net);
			    if (pdb && itf_in_pdb (pak->if_input, pdb)) {
				enqueue(&pdb->clns_inputq, pak);
				break;
			    }
			}
		    }
		}
		datagram_done(pak);
		break;

	    case NLP_ISO8473:	/* clns */
		spot = clns_set_pointers(pak);
		if (spot) {
		    clns_traffic[CLNS_HDRSYNTAX]++;
		    clns_dispose(pak,CLNS_HDRSYNTAX,spot+1);
		    datagram_done(pak);
		} else {
		    if (clns_check_packet(pak)) {
			clns_switch(pak);
		    }
		}
		break;

	    case NLP_ISIS:	/* ISO ISIS */
		pdb = find_isis_pdb();
		if (pdb && clns_pdb_running(pdb)) {
		    if (itf_in_pdb(pak->if_input, pdb)) {
			process_enqueue(pdb->clns_watched_inputq, pak);
			break;
		    }
		}
		datagram_done(pak);
		break;
		
	    default:
		clns_traffic[CLNS_NLPUNKNOWN]++;
		datagram_done(pak);
		break;
	    }
	}
    }
}

/* 
 * clns_ppp_pad
 *
 * Skip past any padding bytes between the PPP header and the CLNS header
 *
 */

void clns_ppp_pad (paktype *pak)
{
    uchar *fudgepacker;
    int    padlen;

    /*
     * CLNS over PPP may have 0 to 3 bytes of padding
     * between the PPP header and the CLNS header as
     * per RFC-1377.
     */

    for (fudgepacker = pak->network_start, padlen = 0;
	 padlen < 4 && *fudgepacker == 0;
	 fudgepacker++, padlen++) {
    }

    if (clns_packet_debug) {
	if (!padlen) {
	    buginf("\n PPP packet for CLNS with no padding");
	} else {
            buginf("\n PPP packet for CLNS with %d byte padding",padlen);
	}
    }

    pak->network_start += padlen;
    pak->encsize       += padlen;
}

/*
 * clns_ager
 *
 * Delete invalid entries from the CLNS prefix routing table. This
 * function will delete IS-IS routing entries that do not match the
 * current version number. This function will delete ISO-IGRP routing
 * entries that have expired.
 */
void clns_ager (void)
{

    clns_ddbtype *ddb, *prev, *next;
    clns_rdbtype *rdb;
    clns_pdbtype *pdb;
    int          i, j, del_count, free_flag, protocol_source = 0;

    free_flag = FALSE;
    pdb = find_isis_pdb();

    for (i = 0; i < CLNS_NETHASHLEN; i++) {
	prev = NULL;
	for (ddb = clns_routes[i]; ddb; prev = ddb, ddb = next) {
 	    protocol_source = 0;
	    next = ddb->next;
	    if (ddb->entry_type != CLNS_ROUTE_ENTRY)
		continue;

	    del_count = 0;
	    for (j = 0; j < ddb->routecount; j++) {
		rdb = ddb->rdb[j];
		if (rdb->protocol_source == ISO_STATIC)
		    continue;

		if (rdb->protocol_source == DECNET_DISCARD)
		    continue;
		/*
		 * For IS-IS routes, if they have just expired,
		 * set the metric to infinity and flash ISO-IGRP
		 * and wait another round for IGRP to poison the route.
		 */
		if ((rdb->protocol_source & ISO_ISISMASK) && pdb) {
		    int rdb_num;
		    if (pdb->current_L2_version == rdb->route_version) 
			continue;
		    if (rdb->metric < CLNS_METRIC_INACCESSIBLE) {
			rdb->metric = CLNS_METRIC_INACCESSIBLE;
			for (rdb_num = 0; rdb_num < ddb->routecount;
			     rdb_num++) {
			    if (ddb->rdb[rdb_num]->metric != 
				CLNS_METRIC_INACCESSIBLE)
				break;
			}
			if (rdb_num == ddb->routecount)	{ /* All are gone */
			    ddb->metric = CLNS_METRIC_INACCESSIBLE;
			    clns_flash_update(rdb->protocol_source);
			    clns_rbuginf("\nCLNS_RT: %s entering holddown",
					 nsap_string(ddb->destination));
			    TIMER_START(ddb->blindtimer, pdb->blindtime);
			}
			continue;
		    }
		}
		if ((rdb->protocol_source & ISO_IGRPMASK) &&
		    (!TIMER_RUNNING(rdb->expiration_time) || 
		     SLEEPING(rdb->expiration_time))) {
		    continue;
		} 
		
		/*
		 * Delete rdb.
		 */
 		protocol_source = rdb->protocol_source; /* Gets last one! */
 		if (clns_delete_rdb(ddb, j)) {
 		    j--;	/* We slid them down... */
 		    free_flag = TRUE;
 		    clns_rbuginf ("\nCLNS-RT: Delete %s from prefix table",
 				  nsap_string (ddb->destination));
 		}
	    }
	    if (!ddb->routecount) {

		/*
		 * Determine if prefix neighbor entries in the adjacency
		 * database should be deleted.
		 */
		clns_manage_prefix_nbrs(ddb, protocol_source, FALSE);


		if (!ddb->destination[0])
		    clns_default_prefix = NULL;
		ddb->next = NULL;
		if (!prev) {
		    clns_routes[i] = next;
		} else {
		    prev->next = next;
		}

		/*
		 * If distance better than static, might have to
		 * reinstall static routes.
		 */
		if (ddb->distance < ISO_STATIC_DIST) {
		    clns_install_static_route(ddb->destination);
		}

		/*
		 * Inform DECnet, if we are doing conversion
		 */
		if (reg_invoke_dnconversion_report())
		    reg_invoke_decnet_trigger_L2_routes();

		free(ddb);
		ddb = prev;

	    }
	}
    }

    /*
     * Increment CLNS cache version number to invalid cache entries that 
     * have timed-out here.
     */
    if (free_flag) {
	clns_invalidate_cache("clns_ager", FALSE);
    }
}

/*
 * clns_on_idb
 *
 * Return TRUE if CLNS is enabled on the given interface
 */
boolean clns_on_idb (idbtype *idb)
{
    if ((!clns_running) || (idb->clns_enabled == FALSE)) {
	return(FALSE);
    } else {
	return(TRUE);
    }
}

/*
 * CLNS subsystem header
 */


#define CLNS_MAJVERSION 1
#define CLNS_MINVERSION 0
#define CLNS_EDITVERSION  1

SUBSYS_HEADER(clns, CLNS_MAJVERSION, CLNS_MINVERSION, CLNS_EDITVERSION,
	      clns_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: iphost, ipdiag, decnet, iprouting, ppp",
	      subsys_req_iphost);
