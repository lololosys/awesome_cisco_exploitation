/* $Id: ipaccess_common.c,v 3.1.2.4 1996/08/28 12:49:36 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipaccess_common.c,v $
 *------------------------------------------------------------------
 * ipaccess_common.c -- Common IP ACL functions
 *
 * May 1996, Greg Christy
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipaccess_common.c,v $
 * Revision 3.1.2.4  1996/08/28  12:49:36  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.2.3  1996/08/07  09:00:12  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/07/30  23:42:56  ahh
 * CSCdi64604:  NAT: need support for multi-homing
 * Allow the use of route maps for choosing a global address pool.
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/07/29  22:45:47  snyder
 * CSCdi64484:  declare arrays const in sub_ipcore
 *              moves 1344 out of data, 20 out of image
 * Branch: California_branch
 *
 * Revision 3.1  1996/05/08  01:02:44  gchristy
 * Initial commit.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "../ip/ip.h"
#include "access.h"
#include "config.h"
#include "parser.h"
#include "../ip/ipaccess.h"
#include "../ip/ipaccess_private.h"
#include "../crypto/crypto_public.h"
#include "../parser/parser_defs_access.h"
#include "../ipmulticast/parser_defs_igmp.h"
#include "parser_defs_ip.h"
#include "../ipmulticast/igmp.h"
#include "logger.h"
#include "../ip/msg_ipsecure.c"		/* Not a typo, see logger.h */
#include "../ip/ipaccess_inline.h"
#include "globs.h"

static logcache_entry logcache[LOGCACHESIZE];
static ulong	      logcache_version;

const id_string proto_items[] = {
    {IP_PROT, IP_PROT_NAME},
    {TCP_PROT, TCP_PROT_NAME},
    {UDP_PROT, UDP_PROT_NAME},
    {ICMP_PROT, ICMP_PROT_NAME},
    {IGRP_PROT, IGRP_PROT_NAME},
    {IGMP_PROT, IGMP_PROT_NAME},
    {GRE_PROT, GRE_PROT_NAME},
    {NOSIP_PROT, NOSIP_PROT_NAME},
    {NEWIGRP_PROT, EIGRP_PROT_NAME},
    {OSPF_PROT, OSPF_PROT_NAME},
    {IPINIP_PROT, IPINIP_PROT_NAME},
    {0, NULL}
};

const id_string old_proto_items[] = {
    {IP_PROT, IP_PROT_NAME},
    {TCP_PROT, TCP_PROT_NAME},
    {UDP_PROT, UDP_PROT_NAME},
    {ICMP_PROT, ICMP_PROT_NAME},
    {IGRP_PROT, IGRP_PROT_NAME},
    {IGMP_PROT, IGMP_PROT_NAME},
    {GRE_PROT, GRE_PROT_NAME},
    {NOSIP_PROT, NOSIP_PROT_NAME},
    {IPINIP_PROT, DVMRP_PROT_NAME},
    {0, NULL}
};

const id_string ip2access_cmds[] = {
    {OPC_NONE, OPC_NONE_STR},
    {OPC_LT, OPC_LT_STR},
    {OPC_GT, OPC_GT_STR},
    {OPC_EQ, OPC_EQ_STR},
    {OPC_NEQ, OPC_NEQ_STR},
    {OPC_RANGE, OPC_RANGE_STR},
    {0, NULL}
};


const id_string icmptypes[] = {
    {ICMP_ECHOREPLY, ICMP_ECHOREPLY_NAME},
    {ICMP_UNREACHABLE, ICMP_UNREACHABLE_NAME},
    {ICMP_QUENCH, ICMP_QUENCH_NAME},
    {ICMP_REDIRECT, ICMP_REDIRECT_NAME},
    {ICMP_ALTADDRESS, ICMP_ALTADDRESS_NAME},
    {ICMP_ECHO, ICMP_ECHO_NAME},
    {ICMP_RDPADVERT, ICMP_RDPADVERT_NAME},
    {ICMP_RDPSOLICIT, ICMP_RDPSOLICIT_NAME},
    {ICMP_TIMEEXCEEDED, ICMP_TIMEEXCEEDED_NAME},
    {ICMP_PARAMETER, ICMP_PARAMETER_NAME},
    {ICMP_TIMESTAMP, ICMP_TIMESTAMP_NAME},
    {ICMP_TIMESTAMPREPLY, ICMP_TIMESTAMPREPLY_NAME},
    {ICMP_INFOREQUEST, ICMP_INFOREQUEST_NAME},
    {ICMP_INFOREPLY, ICMP_INFOREPLY_NAME},
    {ICMP_MASKREQUEST, ICMP_MASKREQUEST_NAME},
    {ICMP_MASKREPLY, ICMP_MASKREPLY_NAME},
    {ICMP_TRACEROUTE, ICMP_TRACEROUTE_NAME},
    {ICMP_CONVERSION, ICMP_CONVERSION_NAME},
    {ICMP_MOBILEREDIRECT, ICMP_MOBILEREDIRECT_NAME},
    {0, NULL}
};

const id_subid_string icmptypecodes [] = {
    {ICMP_UNREACHABLE, ICMP_UNRNET, ICMP_UNRNET_NAME},
    {ICMP_UNREACHABLE, ICMP_UNRHOST, ICMP_UNRHOST_NAME},
    {ICMP_UNREACHABLE, ICMP_UNRPROTOCOL, ICMP_UNRPROTOCOL_NAME},
    {ICMP_UNREACHABLE, ICMP_UNRPORT, ICMP_UNRPORT_NAME},
    {ICMP_UNREACHABLE, ICMP_UNRFRAG, ICMP_UNRFRAG_NAME},
    {ICMP_UNREACHABLE, ICMP_UNRSRFAIL, ICMP_UNRSRFAIL_NAME},
    {ICMP_UNREACHABLE, ICMP_UNRNONET, ICMP_UNRNONET_NAME},
    {ICMP_UNREACHABLE, ICMP_UNRNOHOST, ICMP_UNRNOHOST_NAME},
    {ICMP_UNREACHABLE, ICMP_UNRHOSTISOLATED, ICMP_UNRHOSTISOLATED_NAME},
    {ICMP_UNREACHABLE, ICMP_UNRNETPROHIB, ICMP_UNRNETPROHIB_NAME},
    {ICMP_UNREACHABLE, ICMP_UNRPROHIB, ICMP_UNRPROHIB_NAME},
    {ICMP_UNREACHABLE, ICMP_UNRNETTOS, ICMP_UNRNETTOS_NAME},
    {ICMP_UNREACHABLE, ICMP_UNRHOSTTOS, ICMP_UNRHOSTTOS_NAME},
    {ICMP_UNREACHABLE, ICMP_UNRADMIN, ICMP_UNRADMIN_NAME},
    {ICMP_UNREACHABLE, ICMP_UNRHOSTPREC, ICMP_UNRHOSTPREC_NAME},
    {ICMP_UNREACHABLE, ICMP_UNRPRECEDENCE, ICMP_UNRPRECEDENCE_NAME},
    {ICMP_REDIRECT, ICMP_REDNET, ICMP_REDNET_NAME},
    {ICMP_REDIRECT, ICMP_REDHOST, ICMP_REDHOST_NAME},
    {ICMP_REDIRECT, ICMP_REDSERVNET, ICMP_REDSERVNET_NAME},
    {ICMP_REDIRECT, ICMP_REDSERVHOST, ICMP_REDSERVHOST_NAME},
    {ICMP_TIMEEXCEEDED, ICMP_TIMTTL, ICMP_TIMTTL_NAME},
    {ICMP_TIMEEXCEEDED, ICMP_TIMFRAG, ICMP_TIMFRAG_NAME},
    {ICMP_PARAMETER, ICMP_PARAMPROB, ICMP_PARAMPROB_NAME},
    {ICMP_PARAMETER, ICMP_PARAMREQUIRED, ICMP_PARAMREQUIRED_NAME},
    {ICMP_PARAMETER, ICMP_PARAMREQNOROOM, ICMP_PARAMREQNOROOM_NAME},
    {0, 0, NULL}
};

const id_string igmptypes[] = {
    {IGMP_QUERY_TYPE, IGMP_QUERY_NAME},
    {IGMP_REPORT_TYPE, IGMP_REPORT_NAME},
    {IGMP_DVMRP_TYPE, IGMP_DVMRP_NAME},
    {IGMP_PIM_TYPE, IGMP_PIM_NAME},
    {IGMP_TRACE_TYPE, IGMP_TRACE_NAME},
    {0, NULL}
};


/*
 * nfast_check
 * Check if this outgoing packet should be allowed to pass through
 * this interface.  Returns TRUE/FALSE.  This version uses the ACL
 * header to find the list.
 */

boolean nfast_check (acl_headertype *acl, ipaddrtype target)
{
    fasttype *grp;
    fasthash *acc;
    fastitem *item;
    ipaddrtype mask;

    /*
     * Find the access list.  Return TRUE (pass the packet)
     * if the group doesn't exist or is illegal.
     */
    if (!acl)
	return(TRUE);

    if ((acl->type != ACL_IP_SIMPLE) &&
	(acl->type != ACL_IP_NAMED_SIMPLE))
	return(TRUE);

    grp = (fasttype *) acl->list;
    if (grp == NULL)
	return(TRUE);

    /*
     * Check the hash table of non-wildcarded entries.
     * Fall through to linear scan of wild entries if no matches.
     */
    acc = grp->hashlist[nethash(target)];
    while (acc) {
	if (target == acc->address)
	    return(acc->grant);
	acc = acc->next;
    }

    /*
     * Scan the permission list and make a decision.
     */
    item = grp->items;
    while (item) {
	mask = ~item->wildmask;
	if ((target & mask) == item->permitmask)
	    return(item->grant);
	item = item->next;
    }
    return(FALSE);
}

/*
 * slow_check
 * This is a wrapper function for ip_slow_check_inline.
 */
boolean slow_check (acl_headertype *acl, boolean interrupt_context,
		    idbtype *input, uint protocol,
		    ipaddrtype source, ipaddrtype destination,
		    ulong sport, ulong dport, 
		    boolean ackrst, uchar tosbyte) 
{
	return(ip_slow_check_inline(acl, interrupt_context,
				    input, NULL, protocol, source,
				    destination, sport, dport, ackrst,
				    tosbyte, 0));
}
/*
 * ip_accesscheck_wrapper
 * This is a wrapper function for ip_accesscheck_inline.
 */

boolean ip_accesscheck_wrapper (paktype *pak, acl_headertype *acl,
				hwaddrtype *src_snpa,
				boolean interrupt_context) 
{
    return(ip_accesscheck_inline(pak, acl, src_snpa,
				 interrupt_context));
}

/*
 * ip_accesscheck
 * This function is for backwards compatibility.
 *
 * Note: When doing access checks in performance critical areas where
 * the interrupt context is fixed, ip_accesscheck_wrapper() should be
 * used as the onintstack() call is expensive. 
 */
boolean ip_accesscheck (paktype *pak, acl_headertype *acl)
{
    return(ip_accesscheck_wrapper(pak, acl, NULL, onintstack()));
}

/*
 * ip_accesscheck_snpa
 * This function is a variation of ip_accesscheck which collects the
 * source mac address using the input hwidb's extract_snpa function
 * before calling the access check. 
 *
 * Note: this function must not be called after the vencap or
 * fastswitch function rewrites the output interface encapsulation.
 */
boolean ip_accesscheck_snpa (paktype *pak, acl_headertype *acl,
			     boolean interrupt_context)
{
    hwaddrtype ipaccess_snpa, *src_snpa;
    idbtype *input;

    input = pak->if_input;

    if (input && input->hwptr->extract_snpa) {
	src_snpa = &ipaccess_snpa;
	(*input->hwptr->extract_snpa)(pak, src_snpa, SOURCE);
    } else
	src_snpa = NULL;

    return(ip_accesscheck_wrapper(pak, acl, src_snpa, interrupt_context));
}

static void
accesslog_msg (logcache_entry *entry)
{
    unsigned count;
    char *plural;
    static const char *const mode[] = { "denied", "permitted" };
    char *input_buf, *snpa_buf;

    if (!entry->acl)
	return;

    count = entry->count;
    if (count == 0)
	count = 1;

    plural = (count == 1) ? "" : "s";

    input_buf = string_getnext();

    if (entry->input) {
	sprintf(input_buf, "(%s ", entry->input->namestring);

	/* Tack on source snpa if available */
	if (snpa_is_valid(&entry->src_snpa)) {
	    snpa_buf = string_getnext();
	    print_snpa_string(snpa_buf, &entry->src_snpa);
	    strcat(input_buf, snpa_buf);
	}
	strcat(input_buf, ") ");
    } else
	input_buf[0] = '\0';	/* No input info */

    switch (entry->protocol) {
    case TCP_PROT:
	errmsg(&msgsym(IPACCESSLOGP,SEC), entry->acl->name,
	       mode[entry->grant != 0],
	       "tcp", entry->source, entry->sport, input_buf,
	       entry->destination, entry->dport,
	       count, plural);
	break;
    case UDP_PROT:
	errmsg(&msgsym(IPACCESSLOGP,SEC), entry->acl->name,
	       mode[entry->grant != 0],
	       "udp", entry->source, entry->sport, input_buf,
	       entry->destination, entry->dport,
	       count, plural);
	break;
    case ICMP_PROT:
	errmsg(&msgsym(IPACCESSLOGDP,SEC), entry->acl->name,
	       mode[entry->grant != 0],
	       "icmp", entry->source, input_buf,
	       entry->destination, entry->sport, entry->dport,
	       count, plural);
	break;
    default:
	errmsg(&msgsym(IPACCESSLOGNP,SEC), entry->acl->name,
	       mode[entry->grant != 0],
	       entry->protocol, entry->source, input_buf,
	       entry->destination, count, plural);
    }
}

/*
 * accesslog_cache_print
 * Print old cache information
 */

void accesslog_cache_print (void)
{
    int i;
    logcache_entry *entry;

    entry = logcache;

    for (i = 0; i < LOGCACHESIZE; i++) {
	if (entry->count &&
	    CLOCK_OUTSIDE_INTERVAL(entry->logtime, LOGPRINTDELAY)) {
	    accesslog_msg(entry);
	    GET_TIMESTAMP(entry->logtime);
	    entry->count = 0;
	}
	entry++;
    }
}

void
accesslog_cache (acl_headertype *acl, boolean grant, idbtype *input,
		 hwaddrtype *src_snpa,
		 ipaddrtype source, ipaddrtype destination, 
		 uchar protocol, ushort sport, ushort dport)
{
    int i;
    logcache_entry *entry, *oldest;

    if (!acl)
	return;

    entry = oldest = logcache;

    for (i = 0; i < LOGCACHESIZE; i++, entry++) {
	if ((long)(entry->age - oldest->age) < 0)	/* if entry < oldest */
	    oldest = entry;

	if (entry->source != source)
	    continue;
	if (entry->destination != destination)
	    continue;
	if (entry->protocol != protocol)
	    continue;
	if (entry->dport != dport)
	    continue;
	if (entry->sport != sport)
	    continue;
	if (entry->grant != grant)
	    continue;
	if (entry->acl != acl)
	    continue;
	if (entry->input != input)
	    continue;
	if (src_snpa && !snpa_equ(&entry->src_snpa, src_snpa))
	    continue;
	entry->age = logcache_version++;
	entry->count++;

	return;
    }

    /* generate summary log record if we delayed anything */
    if (oldest->count)
	accesslog_msg(oldest);

    free(oldest->acl);

    oldest->acl = acl;
    mem_lock(acl);		/* Don't let someone else free the ACL
				   header out from under us */
    oldest->grant = grant;
    oldest->input = input;
    oldest->source = source;
    oldest->destination = destination;
    oldest->protocol = protocol;
    oldest->dport = dport;
    oldest->sport = sport;
    oldest->age   = logcache_version++;
    oldest->count = 0;
    GET_TIMESTAMP(oldest->logtime);
    if (src_snpa)
	snpa_copy(src_snpa, &oldest->src_snpa);
    else
	snpa_clear(&oldest->src_snpa);

    accesslog_msg(oldest);
}

/*
 * accesslog_cache_flush
 * Flush all cache information
 */

void accesslog_cache_flush (acl_headertype *acl)
{
    int i;
    logcache_entry *entry;

    entry = logcache;

    for (i = 0; i < LOGCACHESIZE; i++) {
	if (entry->count && (acl == entry->acl || !acl)) {
	    accesslog_msg(entry);
	    entry->count = 0;
	}
	entry++;
    }
}

/*
 * ipaccess_emptylist
 *
 * Determine if an access list is empty
 */

boolean ipaccess_emptylist (acl_headertype *acl)
{
    if (acl == NULL || acl->list == NULL)
	return(TRUE);
    return(FALSE);
}

/*
 * ipaccess_numeric_acl
 */

boolean ipaccess_numeric_acl (acl_headertype *acl)
{
    return(acl->type == ACL_IP_SIMPLE || acl->type == ACL_IP_EXTENDED);
}

/*
 * ipaccess_willpassfrags
 *
 * Return TRUE if this extended access-list will pass non-zero
 * fragments.  Extended access-lists that need to peek into the
 * transport header will always pass non-zero fragments because it
 * does not know if it did (or will) pass fragment zero since that
 * knowledge depends on information only in fragment zero.
 */

boolean
ipaccess_willpassfrags (acl_headertype *acl)
{
    if ((acl->type == ACL_IP_EXTENDED || acl->type == ACL_IP_NAMED_EXTENDED) &&
	acl->tp_header_required)
	return(TRUE);
    return(FALSE);
}
