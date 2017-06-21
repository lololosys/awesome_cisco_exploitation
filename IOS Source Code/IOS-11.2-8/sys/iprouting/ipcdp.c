/* $Id: ipcdp.c,v 3.1.2.3 1996/08/28 12:52:33 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ipcdp.c,v $
 *------------------------------------------------------------------
 * Automatic stub routing for IP via CDP
 *
 * January 1996, Bruce Cole
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipcdp.c,v $
 * Revision 3.1.2.3  1996/08/28  12:52:33  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.2.2  1996/08/27  20:20:46  bcole
 * CSCdi67283:  redistribute odr creates routing process instead of
 * deferring
 * Branch: California_branch
 *
 * Make ODR code manage a process of its own so that the IP routing parse
 * time code can work with it as expected.
 *
 * Revision 3.1.2.1  1996/04/15  21:17:57  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/15  01:40:24  bcole
 * Placeholders for 11.2 features.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "config.h"
#include "logger.h"
#include "../cdp/cdp.h"
#include "subsys.h"
#include "../cdp/cdp_externs.h"
#include "route.h"
#include "../if/network.h"
#include "../ip/ip.h"
#include "../cdp/cdp_debug.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../ip/ip_registry.h"
#include "../cdp/cdp_registry.h"
#include "ipcdp.h"

static sys_timestamp cdp_ip_init_time;
static pdbtype *cdp_pdb;

/*
 * cdp_ip_is_hub
 * Are we acting as a CDP hub?
 */

static boolean cdp_ip_is_hub (void)
{
    if (cdp_pdb && cdp_pdb->running)
	return(TRUE);
    return(FALSE);
}

/*
 * cdp_ip_is_stub
 * Should we consider ourself to be an IP stub?
 * We must:
 *   have no IP routing protocols configured
 *   have been up long enough to detect any CDP neighbors.
 */

static boolean cdp_ip_is_stub (void)
{
    pdbtype *pdb;

    if (cdp_ip_is_hub())
	return(FALSE);
    /*
     * Make sure we don't have any real routing protocols configured.
     */
    for (pdb = protoQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->running &&
	    !(pdb->proctype & (PROC_MOBILE|PROC_NHRP|PROC_ODR)))
	    return(FALSE);
    }
    if (CLOCK_IN_STARTUP_INTERVAL(CDP_ACQUIRETIME))
	return(FALSE);
    return(TRUE);
}

/*
 * cdp_ip_periodic
 * Do periodic housekeeping:
 *   Age out routes from IP routing table that have not been told to us
 *   lately via CDP.
 */

static void cdp_ip_periodic (pdbtype *pdb)
{
    if (!pdb->running)
	return;
    cdp_ipbuginf("\nCDP-IP: Aging routes");
    age_all_routes(pdb);
    process_may_suspend();
}

/*
 * cdp_ip_cleanup
 * Get rid of pdb for CDP.  
 */

static void cdp_ip_cleanup (pdbtype *pdb)
{
    unqueue(&protoQ, pdb);
    free_pdbindex(pdb);
    free(pdb);
    cdp_pdb = NULL;
}

/*
 * cdp_ip_process
 * Lightweight process to age ODR routes.  We are required to have a process
 * in order for the IP routing parse time commands to work correctly
 * (router odr, redistribute odr, etc.)
 */

static void cdp_ip_process (pdbtype *pdb)
{
    if (!process_get_arg_ptr((void**)&pdb))
	process_kill(THIS_PROCESS);
    memory_new_owner(pdb);
    process_wait_on_system_init();
    do {
	TIMER_START_JITTERED(pdb->timer, pdb->broadcasttime,
			     pdb->broadcasttime / 20);
	process_sleep_until(&pdb->timer);
	cdp_ip_periodic(pdb);
    } while (TRUE);
}

/*
 * cdp_ip_create_pdb
 * Create an IP routing pdb for CDP.
 */

static pdbtype *cdp_ip_create_pdb (ushort asystem)
{
    pdbtype *pdb;

    if (cdp_pdb)
	return(cdp_pdb);
    pdb = malloc(sizeof(pdbtype));
    if (!pdb) {
	errmsg(&msgsym(NOMEMORY, SYS), "ODR pdb", sizeof(pdbtype));
	return(NULL);
    }
    if (allocate_pdbindex(pdb) == -1) {
	free(pdb);
	return(NULL);
    }
    pdb->name = "odr";
    pdb->family = PDB_IP;
    pdb->pname = "ODR Router";
    pdb->proctype = PROC_ODR;
    pdb->distance_cmp = iprouting_distcmp_generic;
    pdb->distance = pdb->def_distance = ODR_DISTANCE;
    pdb->multipath = pdb->def_multipath = DEFMAXROUTES;
    pdb->cleanup = cdp_ip_cleanup;

    /*
     * Setup default timers
     */
    pdb->broadcasttime = pdb->def_broadcasttime = CDP_AGETIME;
    pdb->invalidtime = pdb->def_invalidtime = CDP_INVALIDTIME;
    pdb->holdtime = pdb->def_holdtime = 0L;
    pdb->flushtime = pdb->def_flushtime = CDP_FLUSHTIME;
    pdb->process = cdp_ip_process;

    cdp_pdb = pdb;
    enqueue(&protoQ, pdb);
    return(pdb);
}

/*
 * cdp_ip_write_prefix
 * Append another IP prefix to the IP prefix TLV.
 */

static uchar *cdp_ip_write_prefix (ipaddrtype net, ipaddrtype mask,
				   uchar *tlv_stream)
{
    uchar prefixlen;

    prefixlen = ip_bitsinmask(mask);
    cdp_ipbuginf("\nCDP-IP: Writing prefix %i/%d", net, prefixlen);
    PUTLONG(tlv_stream, net);
    tlv_stream += ADDRLEN_IP;
    *tlv_stream = prefixlen;
    tlv_stream++;
    return(tlv_stream);
}

/*
 * cdp_ip_get_stub_prefixes
 * Find IP interfaces with no CDP neighbors that are up.
 * Return a count of the number of prefixes added.
 */

static ushort cdp_ip_get_stub_prefixes (idbtype *ignore, uchar *tlv_stream,
					ushort max)
{
    idbtype *swidb;
    ipaddrtype net, mask;
    ushort count;

    count = 0;
    FOR_ALL_SWIDBS(swidb) {
	/*
	 * Don't consider the interface out which we are sending this CDP
	 * packet.
	 */
	if (swidb == ignore)
	    continue;
	if (!swidb->ip_enabled || !swidb->ip_address)
	    continue;
	if (!interface_up(swidb))
	    continue;
	mask = swidb->ip_nets_mask;
	net = swidb->ip_address & mask;
	tlv_stream = cdp_ip_write_prefix(net, mask, tlv_stream);
	if (++count == max)
	    return(count);
    }
    if (count == 0)
	cdp_ipbuginf("\nCDP-IP: Cannot find stub network");
    return(count);
}

/*
 * cdp_ip_insert_prefixes
 * Build an IP Prefix TLV, space permitting.
 */

static boolean cdp_ip_insert_prefixes (cdp_info_type *item, idbtype *idb,
				       ushort *bytes_left)
{
    ushort tlv_len;
    ushort prefix_count, prefix_room;
    
    if (!cdp_ip_is_stub()) {
	PUTSHORT(&item->length, 0);
	return(TRUE);
    }
    tlv_len = CDP_TLV_OVERHEAD + CDP_IPPREFIX_TLV_LEN;
    /*
     * Check if there is enough space for at least 1 IP prefix
     */
    if (*bytes_left < tlv_len) {
	PUTSHORT(&item->length, 0);
	return(FALSE);
    }
    prefix_room = (*bytes_left - CDP_TLV_OVERHEAD) / CDP_IPPREFIX_TLV_LEN;
    prefix_count = cdp_ip_get_stub_prefixes(idb, item->value, prefix_room);
    if (prefix_count == 0) {
	PUTSHORT(&item->length, 0);
	return(TRUE);
    }
    tlv_len = CDP_TLV_OVERHEAD + CDP_IPPREFIX_TLV_LEN * prefix_count;
    PUTSHORT(&item->code, CDP_IPPREFIX_TYPE);
    PUTSHORT(&item->length, tlv_len);
    *bytes_left -= tlv_len;
    return(TRUE);
}

/*
 * cdp_ip_update_fib
 * Tell the IP routing table about the prefixes we have learned via CDP.
 */

static void cdp_ip_update_fib (ipaddrtype net, ipaddrtype mask,
			       ipaddrtype source, idbtype *swidb)
{
    cdp_ipbuginf("\nCDP-IP: Updating prefix %i%m in routing table",
		 net, mask);
    network_update(cdp_pdb, net, mask, source, swidb, CDP_METRIC,
		   0, source, 0, NULL, 0);
}

/*
 * cdp_ip_source
 * Determine the IP address of the CDP neighbor whose update we are currently
 * processsing.
 */

static ipaddrtype cdp_ip_source (cdp_addr_info *cdp_addr)
{
    uchar i;
    cdp_addr_structure *addr;
    
    if (!cdp_addr)
	return(0);
    addr = (cdp_addr_structure *)cdp_addr->data;
    for (i = 0; i < cdp_addr->num_addr; i++) {
	if (addr->type == ADDR_IP)
	    return(GETLONG(addr->address));
	addr = (cdp_addr_structure *)((uchar *)addr + CDP_ADDRSTRUCT_OVERHEAD +
				      addr->length);
    }
    return(0);
}

/*
 * cdp_handle_ip_info
 * Decode IP prefix TLV from incoming CDP update.
 */

static void cdp_handle_ip_info (cdp_info_type *item,
				cdp_cache_type *cache_entry, boolean new_entry)
{
    ushort data_len;
    ipaddrtype net, mask, source;
    uchar prefixlen, *tlv_stream;
    idbtype *swidb;
    ushort count, i;

    data_len = GETSHORT(&item->length) - CDP_TLV_OVERHEAD;
    count = data_len / CDP_IPPREFIX_TLV_LEN;
    if ((count == 0) || (data_len % CDP_IPPREFIX_TLV_LEN) != 0) {
	cdp_ipbuginf("\nCDP-IP: IP TLV length (%d) invalid", data_len);
	return;
    }
    if (!cdp_ip_is_hub())
	return;
    tlv_stream = item->value;
    swidb = cache_entry->idb;
    source = cdp_ip_source(cache_entry->addr);
    for (i = 0; i < count; i++) {
	net = GETLONG(tlv_stream);
	tlv_stream += ADDRLEN_IP;
	prefixlen = *tlv_stream;
	tlv_stream++;
	cdp_ipbuginf("\nCDP-IP: Reading prefix %i/%d source %i via %s",
		     net, prefixlen, source,
		     swidb ? swidb->namestring : "");
	mask = prefixlen ? ip_bitcount2mask(prefixlen) : 0;
	if (swidb)
	    cdp_ip_update_fib(net, mask, source, swidb);
    }
}

/*
 * cdp_ip_init
 * Register hooks so that CDP calls us during TLV processing.
 */

static void cdp_ip_init (subsystype *subsys)
{
    GET_TIMESTAMP(cdp_ip_init_time);
    add_tlv_func[CDP_IPPREFIX_TYPE] = cdp_ip_insert_prefixes;
    cdp_decode_func[CDP_IPPREFIX_TYPE] = cdp_handle_ip_info;
    reg_add_iprouting_pdb_init(PROC_ODR, cdp_ip_create_pdb,
			       "cdp_ip_create_pdb");
}

/*
 * IPCDP subsystem header
 */

#define IPCDP_MAJVERSION 1
#define IPCDP_MINVERSION 0
#define IPCDP_EDITVERSION  1

SUBSYS_HEADER(ipcdp, IPCDP_MAJVERSION, IPCDP_MINVERSION, IPCDP_EDITVERSION,
	      cdp_ip_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: cdp",
	      "req: cdp, iprouting");
