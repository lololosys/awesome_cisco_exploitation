/* $Id: ipfast_flow.c,v 3.3.4.8 1996/09/11 20:09:34 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipfast_flow.c,v $
 *------------------------------------------------------------------
 * <One line description here.>
 *
 * <Month> 1996 Darren Kerr
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipfast_flow.c,v $
 * Revision 3.3.4.8  1996/09/11  20:09:34  snyder
 * CSCdi68837:  more things const
 *              156 out of data, 64 image
 * Branch: California_branch
 *
 * Revision 3.3.4.7  1996/05/21  09:51:20  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3.4.6  1996/05/12  23:09:30  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.3.4.5  1996/05/08  14:36:39  xliu
 * CSCdi56777:  memd buffers are queued to process level for
 * encryption/decryption
 * Branch: California_branch
 *
 * Revision 3.3.4.4  1996/05/07  00:14:11  mbeesley
 * CSCdi55679:  Add IP flow/optimum switching engine
 * Branch: California_branch
 *
 * Revision 3.3.4.3  1996/05/04  01:03:29  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - ARP, IP protocol, fast-switching and smf support for the Bridge-group
 *   Virtual Interface.
 * - Set up bridge/route indicators based on CRB/IRB is enabled or not.
 *
 * Constrained Multicast Flooding
 * - Convert tbridge flood array to a linked list of tbifd's.
 *
 * Name Access List
 *
 * Revision 3.3.4.2  1996/04/27  05:51:36  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.3.28.2  1996/04/26  01:22:42  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.3.28.1  1996/04/17  02:05:47  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.3.4.1  1996/04/19  15:41:42  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.1.18.4  1996/04/11  23:14:53  carrel
 * Branch: IosSec_branch
 * IP code review comments #1
 *
 * Revision 3.1.18.3  1996/03/21  10:12:39  xliu
 * Branch: IosSec_branch
 * Modified comments.
 *
 * Revision 3.1.18.2  1996/03/21  10:04:10  xliu
 * Branch: IosSec_branch
 * Migrated to V111_1_3.
 *
 * Revision 3.1.18.1  1996/03/19  08:09:00  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 3.3  1996/03/07  12:46:12  dkerr
 * CSCdi50623:  IP flow cache needs better accounting granularity
 * Fix alignment errors in access list checks for inbound tokenring.
 *
 * Revision 3.2  1996/03/04  15:31:02  dkerr
 * CSCdi50623:  IP flow cache needs better accounting granularity
 * Better show output.  Also allow flow stats to be exported.
 *
 * Revision 3.1  1996/01/26  00:00:59  dkerr
 * Placeholder
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "../ui/common_strings.h"
#include "../os/clock.h"
#include "registry.h"
#include "logger.h"
#include "../os/free.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "../if/ether.h"
#include "../if/fddi.h"
#include "config.h"
#include "../ip/ip.h"
#include "../ipmulticast/igmp.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "../ip/ipaccount.h"
#include "../ip/ipfast.h"
#include "../if/tring.h"
#include "../les/if_les.h"
#include "../wan/frame_relay.h"
#include "../wan/atm_dxi.h"
#include "../wan/smds.h"
#include "../wan/dialer.h"
#include "../if/network.h"
#include "../if/atm.h"
#include "../iprouting/route.h"
#include "sys_registry.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_tcpip.h"
#include "ip_registry.h"
#include "../ui/debug.h"
#include "../ip/ipfast_flow.h"
#include "../crypto/crypto_public.h"
#include "../crypto/crypto_registry.h"

ipflowtbl_t ipflowtbl;
ipflow_t bogon_flow;
paktype pak_access;

static ulong flowstat_seconds;

/*
 * These puppies are used to export flow statistics to an
 * accounting machine (RMON2, unix application, whatever...).
 */
static ipsocktype *flow_export_socket;
static IPStatMsg  *flow_export_msg;
static paktype    *flow_export_pak;
static queuetype  flow_export_pakQ;
ipaddrtype        flow_export_address;
ushort            flow_export_port;
  
/*
 * Break up flow statistics into common groups.
 */
ipflow_stats_t ipflowstats[IPFLOWSTAT_TOTAL + 1] = {
    { 0, 0, 0, 0, 0, "TCP-Telnet"},
    { 0, 0, 0, 0, 0, "TCP-FTP   "},
    { 0, 0, 0, 0, 0, "TCP-FTPD  "},
    { 0, 0, 0, 0, 0, "TCP-WWW   "},
    { 0, 0, 0, 0, 0, "TCP-SMTP  "},
    { 0, 0, 0, 0, 0, "TCP-X     "},
    { 0, 0, 0, 0, 0, "TCP-BGP   "},
    { 0, 0, 0, 0, 0, "TCP-Frag  "},
    { 0, 0, 0, 0, 0, "TCP-other "},
    { 0, 0, 0, 0, 0, "UDP-DNS   "},
    { 0, 0, 0, 0, 0, "UDP-NTP   "},
    { 0, 0, 0, 0, 0, "UDP-TFTP  "},
    { 0, 0, 0, 0, 0, "UDP-Frag  "},
    { 0, 0, 0, 0, 0, "UDP-other "},
    { 0, 0, 0, 0, 0, "ICMP      "},
    { 0, 0, 0, 0, 0, "IGMP      "},
    { 0, 0, 0, 0, 0, "IPINIP    "},
    { 0, 0, 0, 0, 0, "GRE       "},
    { 0, 0, 0, 0, 0, "IP-other  "},
    { 0, 0, 0, 0, 0, "Total:    "}
};

static char *first_line_str =
" 1-32   64   96  128  160  192  224  256  288  320  352  384  416  448  480";
static char *second_line_str =
"  512  544  576 1024 1536 2048 2560 3072 3584 4096 4608";

ulonglong ip_total_pkts[TOTAL_PKT_SLOTS];

static void show_ip_pkt_sizes (void)
{
    int idx;
    ulonglong dtot;
    ulong ptot;
    char *total_units;

    for (idx = 0, dtot = 0; idx < TOTAL_PKT_SLOTS; idx++)
      dtot += ip_total_pkts[idx];
    if ((dtot / 1000000000) > 0) {
	ptot = dtot / 1000000;
	total_units = "M";
    } else {
	ptot = dtot;
	total_units = "";
    }
    if (!dtot)
      dtot = 1; /* avoid div0 */
    
    printf("\nIP packet size distribution (%lu%s total packets):\n",
	   ptot, total_units);
    printf("  %s\n  ", first_line_str);
    for (idx = 0; idx < FIRST_LINE_SLOTS; idx++) {
	ptot = (ip_total_pkts[idx] * 1000) / dtot;
	if (ptot == 1000)
	    printf(" 1.00");
	else
	    printf(" .%03d", ptot);
    }

    printf("\n\n  %s\n  ", second_line_str);
    for (idx = FIRST_LINE_SLOTS; idx < TOTAL_LINE_SLOTS; idx++) {
	ptot = (ip_total_pkts[idx] * 1000) / dtot;
	if (ptot == 1000)
	    printf(" 1.00");
	else
	    printf(" .%03d", ptot);
    }
}

#ifdef DEBUG
void flow_tcp_debug (iphdrtype *ip)
{
    if (ip->prot == 6) {
	tcptype *tcp = (tcptype *)(ip + 1);
	buginf("\nF:%i/%04x %i/%04x %08x %08x",
	       GETLONG(&ip->srcadr), GETSHORT(&tcp->sourceport),
	       GETLONG(&ip->dstadr), GETSHORT(&tcp->destinationport),
	       GETLONG(&tcp->sequencenumber),
	       GETLONG(&tcp->acknowledgementnumber));
	buginf(" %s%s%s%s%s%s len=%d ",
	       tcp->syn ? "S" : "",
	       tcp->ack ? "A" : "",
	       tcp->fin ? "F" : "",
	       tcp->urg ? "U" : "",
	       tcp->rst ? "R" : "",
	       tcp->psh ? "P" : "",
	       ip->tl - 20 - tcp->dataoffset*4);
    }
}
#endif

/*
 * ipflow_acess_check:
 * Perform access checks and IP accounting as per configuration. This
 * inline is the flow access check engine. It is inlined because the
 * interface is different between Predator and RSP.
 */
static inline boolean ipflow_access_check (paktype *pak,
					   iphdrtype *ip, 
					   hwidbtype *hwin, 
					   idbtype *swout,
					   ushort iplen, 
					   int pkts,
					   crypto_tag_t *crypto_tag)
{
    hwidbtype *hwout = swout->hwptr;
    idbtype *cfg_swidb;
    acl_headertype *acl;

    /*
     * Is IP policy routing enabled on some interface?
     */
    if ((hwout->ip_fast_flags & IP_FAST_POLICY) &&
	(hwin->fast_rcvidb->ip_info &&
	 hwin->fast_rcvidb->ip_info->policy_enabled))
	return(FALSE);

    /* 
     * Check crypto map for input decrption and output encryption. 
     */ 
    if (hwin->fast_rcvidb->crypto_map) {
        if ((!reg_invoke_crypto_map_check(pak, hwin->firstsw->crypto_map,
            CRYPTO_DECRYPT, TRUE)) || crypto_deny_packet(pak)) { 
            return(FALSE);
        }
        crypto_tag->cryptoflags = pak->cryptoflags;
        crypto_tag->crypto_conn_id = pak->crypto_dec_conn_id;
    }

    if (swout->crypto_map) {
        if ((!reg_invoke_crypto_map_check(pak, swout->crypto_map,
             CRYPTO_ENCRYPT, TRUE)) || crypto_deny_packet(pak)) { 
            return(FALSE);
        }
        crypto_tag->cryptoflags = pak->cryptoflags;
        crypto_tag->crypto_conn_id = pak->crypto_enc_conn_id;
    }

    /*
     * If the flow should be encrypted or decrypted and the
     * connection has not been established, the flow should not 
     * be created.
     */
    if ((pak->cryptoflags & CRYPTO_ENCRYPT) || 
       (pak->cryptoflags & CRYPTO_DECRYPT)) {
	if (pak->cryptoflags & CRYPTO_DECRYPT) {
	    if (pak->crypto_dec_conn_id <= 0) {
	   	return (FALSE);
	    } else {
        	reg_invoke_crypto_add_connection_idb(swout->hwptr,
                                             	     pak->crypto_dec_conn_id);
	    }
	}
	if (pak->cryptoflags & CRYPTO_ENCRYPT) {
	    if (pak->crypto_enc_conn_id <= 0) {
	   	return (FALSE);
	    } else {
        	reg_invoke_crypto_add_connection_idb(swout->hwptr,
                                             	     pak->crypto_enc_conn_id);
	    }
	} 

    }

    if (hwout->ip_fast_flags & IP_FAST_ACCESSLISTS) {
	if (!ip_accesscheck_wrapper(pak, swout->ip_output_accesslist,
				    NULL, TRUE))
	    return(FALSE);
    }
    if (hwout->ip_fast_flags & IP_FAST_INPUTACCESSLISTS) {
	/*
	 * For DDR interfaces, get the access list from the
	 * rotary group lead.
	 */
	if (is_ddr(hwin)) {
            cfg_swidb = get_netcfg_idb(hwin->firstsw);
            if (!cfg_swidb)
               return(FALSE);
	    acl = cfg_swidb->ip_input_accesslist;
	} else
	    acl = hwin->fast_rcvidb->ip_input_accesslist;       
	if (acl && !ip_accesscheck_wrapper(pak, acl, NULL, TRUE))  
	    return(FALSE);                                  
    }

    /*
     * If flow switching is configured, we do the odd thing of
     * incrementing the src/dst accounting entry by *zero* pkts and bytes.
     * This is just done to make sure that we have a pre-allocated
     * accounting entry.  When the flow is later reaped, the aggregate
     * packets and bytes will be added en masse to the accounting
     * entry.
     * 
     * XXX - Since we reap flows at process level, maybe we ought to
     * skip the null accounting here, and instead just allocate entries
     * from the flow code if needed.
     */
    if ((hwout->ip_fast_flags & IP_FAST_ACCOUNTING) &&
	!ip_fast_accumulate_acctg(GETLONG(&ip->srcadr), GETLONG(&ip->dstadr),
				  iplen, pkts))
      return(FALSE);

    return(TRUE);
}

/*
 * flow_access_check:
 * Perform access checks and IP accounting as per config. Does not
 * get passed a paktype pointer, so needs surrogate.
 */
boolean flow_access_check (iphdrtype *ip, hwidbtype *hwin, idbtype *swout,
			   ushort iplen, crypto_tag_t *crypto_tag)
{
    paktype *pak = &pak_access;
    int pkts = iplen ? 1 : 0;
    
    /*
     * Create a dummy pak struct for use by access checker.
     * Make sure that pak->network_start is only field used...
     * Also, if the types of fields checked by the access lists
     * is expanded, then we may need to update the switch checks.
     */
    pak->network_start = (uchar *)ip;
    pak->cryptoflags = 0;

    /*
     * Do the checking
     */
    return(ipflow_access_check(pak, ip, hwin, swout, iplen, pkts, crypto_tag));
}

/*
 * pak_flow_access_check:
 * Perform access checks and IP accounting as per config. Gets passed 
 * a paktype pointer, so no need for surrogate.
 */
boolean pak_flow_access_check (paktype *pak, hwidbtype *hwin, idbtype *swout,
			       ushort iplen, crypto_tag_t *crypto_tag)
{
    iphdrtype *ip = (iphdrtype *)ipheadstart(pak);
    int pkts = iplen ? 1 : 0;

    /*
     * Do the checking
     */
    pak->cryptoflags = 0;
    return(ipflow_access_check(pak, ip, hwin, swout, iplen, pkts, crypto_tag));
}

/*
 * If configured to do so, copy expired flow's stats to successive
 * fields in udp packet.  When the packet fills up, send out to
 * configured socket.
 */
static inline void flow_export_stats (ipflow_t *flow)
{
    IPFlowStat *record;
    uint offset;
    
    if (!flow_export_socket)
	return;
    
    if (!flow_export_pak) {
	flow_export_pak = setup_ipsocket(flow_export_socket, FLOW_PAK_SIZE,
					 &offset);
	if (!flow_export_pak) {
	    ipflowtbl.export_no_pak++;
	    return;
	}

	ipflowtbl.export_paks++;
	flow_export_msg = (IPStatMsg *)(flow_export_pak->data_area + offset);
	flow_export_msg->header.version = FLOWSTATHDR_VERSION;
	flow_export_msg->header.count = 0;
	GET_TIMESTAMP32(flow_export_msg->header.SysUptime);
	secs_and_nsecs_since_jan_1_1970((secs_and_nsecs *)
					&flow_export_msg->header.unix_secs);
    }
    record = &flow_export_msg->records[flow_export_msg->header.count++];
    record->srcaddr   = flow->key.fl_src;
    record->dstaddr   = flow->key.fl_dst;
    record->prot      = flow->key.fl_prot;
    record->First     = flow->first_switched;
    record->Last      = flow->last_switched;
    record->dPkts     = flow->pkts;
    record->dOctets   = flow->bytes;
    record->srcport   = flow->key.fl_sport;
    record->dstport   = flow->key.fl_dport;
    *(ulong *)&record->flags = *(ulong *)&flow->tcp_flags;
    
    record->input = flow->fl_input->mci_index;
    record->output = flow->fl_output->mci_index;
    record->nexthop = flow->nexthop;
    ipflowtbl.export_records++;

    /*
     * As soon as we fill up a pak with flow stats, enqueue for
     * later process-level sending.  This is needed to do route
     * lookup.
     */
    if (flow_export_msg->header.count >= FLOWS_PER_PAK) {
	p_enqueue(&flow_export_pakQ, flow_export_pak);
	flow_export_pak = NULL;
	flow_export_msg = NULL;
    }
}


/*
 * Update global ip flow stats.
 * This might be made more efficient by expanding the classifier
 * in the flow creation routine and saving the global statistics index
 * in somewhere in the flow descriptor.
 */
static inline void update_global_stats (ipflow_t *flow)
{
    uchar protocol = flow->key.fl_prot;
    ushort dport = flow->key.fl_dport;
    ushort sport = flow->key.fl_sport;
    ushort known_port;
    int stat_index;
    ipflow_stats_t *statp;
    ulong now;

    stat_index = IPFLOWSTAT_IP_OTHER;

    if (protocol == TCP_PROT) {
	if (dport <= BGP_PORT)
	  known_port = dport;
	else 
	  known_port = sport;

	if (known_port == WWW_PORT)
	  stat_index = IPFLOWSTAT_TCP_WWW;
	else if (known_port == TELNET_PORT)
	  stat_index = IPFLOWSTAT_TCP_TELNET;
	else if (known_port == FTP_PORT)
	  stat_index = IPFLOWSTAT_TCP_FTP;
	else if (known_port == FTPD_PORT)
	  stat_index = IPFLOWSTAT_TCP_FTPD;
	else if (known_port == SMTP_PORT)
	  stat_index = IPFLOWSTAT_TCP_SMTP;
	else if (known_port == BGP_PORT)
	  stat_index = IPFLOWSTAT_TCP_BGP;
	else if ((dport == 0) && (sport == 0))
	  stat_index = IPFLOWSTAT_TCP_FRAG;
	else if ((dport == X_PORT) || (sport == X_PORT))
	  stat_index = IPFLOWSTAT_TCP_X;
	else
	  stat_index = IPFLOWSTAT_TCP_OTHER;
    } else if (protocol == UDP_PROT) {
	if (dport == DNS_PORT)
	    stat_index = IPFLOWSTAT_UDP_DNS;
	else if (dport == NTP_PORT)
	    stat_index = IPFLOWSTAT_UDP_NTP;
	else if ((dport == TFTP_PORT) || (sport == TFTP_PORT))
	    stat_index = IPFLOWSTAT_UDP_TFTP;
	else if ((dport == 0) && (sport == 0))
	  stat_index = IPFLOWSTAT_UDP_FRAG;
	else
	    stat_index = IPFLOWSTAT_UDP_OTHER;
    } else if (protocol == ICMP_PROT) {
	stat_index = IPFLOWSTAT_ICMP;
    } else if (protocol == IGMP_PROT) {
	stat_index = IPFLOWSTAT_IGMP;
    } else if (protocol == IPINIP_PROT) {
	stat_index = IPFLOWSTAT_IPINIP;
    } else if (protocol == GRE_PROT) {
	stat_index = IPFLOWSTAT_GRE;
    }

    statp = &ipflowstats[stat_index];
    statp->flows++;
    statp->active += clock_diff32(flow->first_switched, flow->last_switched);
    GET_TIMESTAMP32(now);
    statp->idle += clock_diff32(flow->last_switched, now);
    statp->pkts += flow->pkts;
    statp->bytes += flow->bytes;
}


/*
 * Free flow descriptor and propogate stats to global and export
 * counters.
 */
static inline void ipflow_free (ipflow_t *flow)
{
    flow->hash_next = ipflowtbl.free;
    ipflowtbl.free = flow;
    ipflowtbl.free_flows++;
    ipflowtbl.active_flows--;

    if (flow->cptr)
      ipcache_adjust_refcount(flow->cptr, -1);
    
    /*
     * Send stats to some configured address.
     */
    flow_export_stats(flow);

    /*
     * Flush total flow byte/packet/duration counters to global
     * per-protocol statistics.
     */
    update_global_stats(flow);

    if (flow->fl_output->ip_fast_flags & IP_FAST_ACCOUNTING) {
	/*
	 * Update src/dst extended accounting database if configured.
	 * LATER: figure out how to handle FALSE return (no accounting entry
	 * available).  Most of the time there should be one, since we
	 * wouldn't have created a flow if an accounting entry for this
	 * src/dst wasn't already allocated.
	 */
	ip_fast_accumulate_acctg(flow->key.fl_src, flow->key.fl_dst,
				 flow->bytes, flow->pkts);
    }
}

/*
 * The fast flow lookup failed to find a valid flow.
 * If we can find a valid entry in the destination network cache,
 * and the packet passes access checks, then try to allocate
 * a new descriptor for this flow and insert it into the lookup table.
 */
ipflow_t *
ipflow_add (iphdrtype *ip, hwidbtype *hwin, hwidbtype *hwout,
	    ipflow_key_t *key, crypto_tag_t *crypto_tag)
{
    ipflow_t *flow, *bucket;
    int hash;

    flow = ipflowtbl.free;
    if (flow) {
	ipflowtbl.free = flow->hash_next;
	flow->hash_next = NULL;
	flow->key = *key;
	GET_TIMESTAMP32(flow->first_switched);
	GET_TIMESTAMP32(flow->last_switched);
	flow->fl_output = hwout;
	flow->fl_input = hwin;
	if (flow->key.fl_prot == TCP_PROT) {
	    tcptype *tcp = (tcptype *)(ip + 1);
	    flow->next_sequence = GETLONG(&tcp->sequencenumber);
	} else
	  flow->next_sequence = 0;

	flow->bytes = ip->tl;
	flow->pkts = 1;
	*(ulong *)&flow->tcp_flags = 0;

        flow->crypto_tag.cryptoflags = crypto_tag->cryptoflags;
        flow->crypto_tag.crypto_conn_id = crypto_tag->crypto_conn_id;

	hash = ipflow_hash(key->fl_dst, key->fl_src, key->fl_ports,
			   key->fl_misc, ipflowtbl.hash_mask);
	bucket = (ipflow_t *)&ipflowtbl.hashtbl->bucket[hash];
	flow->hash_next = bucket->hash_next;
	bucket->hash_next = flow;
	
	ipflowtbl.added++;
	ipflowtbl.active_flows++;
	ipflowtbl.free_flows--;
    } else {
	flow = &bogon_flow;
    
	/*
	 * Add these instead of setting, so that we can keep track
	 * of the number of packets/bytes not being accounted for
	 * via flow entries.
	 */
	ipflowtbl.no_flow_pkts++;
    }
    return(flow);
}

static inline boolean
flow_expired (ipflow_t *flow, ulong now)
{
    if ((clock_diff32(flow->last_switched, now) > IDLE_TIMEOUT) ||
	(clock_diff32(flow->first_switched, now) > ACTIVE_TIMEOUT))
      return(TRUE);
    else
      return(FALSE);
}

/*
 * We have to do this at process level (for now), since write_ipsocket
 * does route lookup.  With fib, we should be able to do this at
 * interrupt level.
 *
 * XXX - this may cause bursts of flow stat packets once per second
 * in a busy network.  Should re-implement at interrupt level to
 * space stuff out...
 */
static void send_export_paks (void)
{
    paktype *pak;
    int size;
    
    if (flow_export_socket) {
	while ((pak = p_dequeue(&flow_export_pakQ)) != NULL)
	    write_ipsocket(flow_export_socket, pak, FLOW_PAK_SIZE);
	
	if (flow_export_pak != NULL) {
	    size = sizeof(IPStatMsg);
	    size += sizeof(IPFlowStat) * flow_export_msg->header.count;
	    write_ipsocket(flow_export_socket, flow_export_pak, size);
	    flow_export_pak = NULL;
	    flow_export_msg = NULL;
	}
    }
}

static void scan_for_expired_flows (void)
{
    ipflow_t *prev, *flow, **bp, **periodp, **setlevelp;
    static ulong bkt = 0, valid_buckets = 0;
    ulong now, hashlen, maxhashlen = 0;
    leveltype level;

   /*
     * Look at only a portion (1/8th) of the table each second.
     */
    if ((bkt + BKTS_PER_PERIOD) > ipflowtbl.hash_size) {
	ipflowtbl.valid_buckets = valid_buckets;
	bkt = valid_buckets = 0;
    }
    bp = &ipflowtbl.hashtbl->bucket[bkt];
    periodp = bp + BKTS_PER_PERIOD;
    bkt += BKTS_PER_PERIOD;
    GET_TIMESTAMP32(now); /* this doesn't need to be very accurate */
    while (bp < periodp) {
	level = raise_interrupt_level(NETS_DISABLE);
	setlevelp = bp + BKTS_PER_SETLEVEL;
	while (bp < setlevelp) {
	    prev = (ipflow_t *)(bp++); /* Set prev to the bucket location */
	    flow = prev->hash_next;    /* First flow pointed to by bucket */
	    if (!flow)
		continue;
	    hashlen = 0;
	    valid_buckets++;
	    do {
		hashlen++;
		if (flow_expired(flow, now)) {
		    prev->hash_next = flow->hash_next;
		    ipflow_free(flow);
		} else {
		    prev = flow;
		}
	    } while ((flow = prev->hash_next) != NULL);
	    
	    if (hashlen > maxhashlen)
		maxhashlen = hashlen;
	}
	reset_interrupt_level(level);
    }
    /*
     * Keep track of hash depths for tracking worst case performance.
     */
    ipflowtbl.cur_hash_max = maxhashlen;
    if (ipflowtbl.max_hash_max < maxhashlen)
      ipflowtbl.max_hash_max = maxhashlen;

}


/*
 * Periodically flush stats from hash entries to interface
 *   and ip accounting structs.
 */
void ipflow_periodic (void)
{
    if (!ipflowtbl.active_flows)
      return;

    flowstat_seconds++;
    if (flow_export_address && !flow_export_socket) {
	addrtype addr;

	addr.type = ADDR_IP;
	addr.length = ADDRLEN_IP;
	addr.ip_addr = flow_export_address;
	flow_export_socket = open_ipsocket(UDP_PROT, NULL, &addr, 0,
					   flow_export_port, SOCK_ANYSRC);
    }

    scan_for_expired_flows();
 
    send_export_paks();
}


/*
Protocol         Total  Flows   Packets Bytes  Packets Active(Sec) Idle(Sec)
--------         Flows   /Sec     /Flow  /Pkt     /Sec     /Flow     /Flow
ICMP                 8    0.0   5464806    95    914.8    5970.4       0.0
Total:               8    0.0   5464806    21    914.8    5970.4       0.0
*/

static void show_protocol_stats (void)
{
    ipflow_stats_t *statp, *tstatp;
    ulong Fps, Pps, Ppf, Bpp, Apf, Ipf;
    ulong tensecs, idx, flows;

    printf("\n  statistics cleared %d seconds ago\n", flowstat_seconds);
    printf("\nProtocol         Total  Flows   Packets Bytes  Packets Active(Sec) Idle(Sec)");
    printf("\n--------         Flows   /Sec     /Flow  /Pkt     /Sec     /Flow     /Flow");

    tensecs = flowstat_seconds / 10;
    if (!tensecs)
	tensecs = 1;
    
    /*
     * Last entry in stat table is for summing previous totals.
     */
    tstatp = &ipflowstats[IPFLOWSTAT_TOTAL];
    tstatp->flows = 0;
    tstatp->pkts = 0;
    tstatp->bytes = 0;
    tstatp->active = 0;
    tstatp->idle = 0;
    
    for (idx = 0; idx <= IPFLOWSTAT_TOTAL; idx++) {
	statp = &ipflowstats[idx];
	
	flows = statp->flows;
	
	if (flows && statp->pkts) {

	    Fps = flows / tensecs;
	    Pps = statp->pkts / tensecs;
	    Bpp = statp->bytes / statp->pkts;
	    Ppf = statp->pkts / flows;
	    Apf = statp->active / flows / (ONESEC/10);
	    Ipf = statp->idle / flows / (ONESEC/10);
	    
	    printf("\n%s %-11d %-4d.%d %-9d %-5d %-6d.%d",
		   statp->proto, flows, Fps / 10, Fps % 10,
		   Ppf, Bpp, Pps / 10, Pps % 10);
	    
	    printf(" %-7d.%d %-7d.%d", Apf / 10, Apf % 10, Ipf / 10, Ipf % 10);

	    tstatp->pkts += statp->pkts;
	    tstatp->flows += flows;
	    tstatp->bytes += statp->bytes;
	    tstatp->active += statp->active;
	    tstatp->idle += statp->idle;
	}
    }
}

static const char rstring[] =
  "\nSrcIf    SrcIPaddress    DstIf    DstIPaddress    Pr SrcP DstP Pkts B/Pk Active";
/*
SrcIf    SrcIPaddress    DstIf    DstIPaddress    Pr SrcP DstP Pkts B/Pk Active
Fd10/0/0 151.101.100.141 Et10/1/0 171.168.132.111 06 0017 5608   10  100   10.3
Et2/0/1  15.1.100.41     Et3/0    15.1.16.1       06 0017 5608   22   60    2.1
*/

/*
 * rsp_ipflow_show
 */
void ipflow_show (ipaddrtype prefix, ipaddrtype mask, idbtype *idb)
{
    ipaddrtype dest, src;
    hwidbtype *hwidb = (idb) ? idb->hwptr : NULL;
    ipflow_t *flow, *show, *showbuf;
    ipflowtbl_t *tp = &ipflowtbl;
    leveltype level;
    int bkt, idx;
    ulong pkts, bytes;
    char pktC;
    
    automore_enable(NULL);
    
	
    show_ip_pkt_sizes();
    
    printf("\n\nIP Flow Switching Cache, %d active, %d inactive, %d added",
	   tp->active_flows, tp->free_flows, tp->added);
    printf("\n  %d flows exported, %lu not exported, %lu export msgs sent",
	   tp->export_records, tp->export_no_pak, tp->export_paks);
    printf("\n  %d cur max hash, %d worst max hash, %d valid buckets",
	   tp->cur_hash_max, tp->max_hash_max, tp->valid_buckets);
    printf("\n  %lu flow alloc failures", tp->no_flow_pkts);

    show_protocol_stats();
    
    if (!(showbuf = malloc(sizeof(ipflow_t) * IPFLOWSHOWMAX)))
	return;
    
    automore_header(rstring);
    for (bkt=0; bkt<tp->hash_size; bkt++) {
	level = raise_interrupt_level(NETS_DISABLE);
	flow = (ipflow_t *)ipflowtbl.hashtbl->bucket[bkt];
	for (idx=0; flow && (idx<IPFLOWSHOWMAX); flow = flow->hash_next) {
	    if (!flow->pkts)
		continue; /* avoid div0 error, though shouldn't happen */
	    dest = flow->key.fl_dst;
	    src = flow->key.fl_src;
	    if (((dest & mask) != prefix) && ((src & mask) != prefix))
	      continue;
	    if (hwidb && hwidb != flow->fl_output && hwidb != flow->fl_input)
	      continue;
	
	    showbuf[idx++] = *flow;
	}
	reset_interrupt_level(level);

	show = showbuf;
	for ( ;idx-- > 0; show++) {
	    ulong active;
	    
	    pkts = show->pkts;
	    bytes = show->bytes / pkts;
	    active = clock_diff32(show->first_switched, show->last_switched);
	    active /= (ONESEC / 10);

	    if (pkts > 9999999) {
		pkts /= 1000000;
		pktC = 'M';
	    } else if (pkts > 9999) {
		pkts /= 1000;
		pktC = 'K';
	    } else
	      pktC = ' ';
	    
	    printf("\n%8s %15i %8s %15i %02x %04x %04x",
		   show->fl_input->hw_short_namestring, show->key.fl_src,
		   show->fl_output->hw_short_namestring, show->key.fl_dst,
		   show->key.fl_prot, show->key.fl_sport, show->key.fl_dport);
	    printf(" %-4d%c%-4d %-4d.%d",
		   pkts, pktC, bytes, active / 10, active % 10);

	}
 	if (automore_quit())
	    break;
    }
    free(showbuf);
    automore_disable();
}

/*
 * Configure sending of expired flow stats to a host via
 * an IP address and UDP port.
 */
boolean ip_flow_cache_commands (parseinfo *csb)
{
    ipaddrtype address;
    ulong      port;

    /*
     * Set flow_export ip address and port numbers.
     */
    if (csb->nvgen) {
        /* print out your set of flow entries */
	if (flow_export_address && flow_export_port)
	    nv_write(TRUE, "%s %i %d", csb->nv_command,
               flow_export_address, flow_export_port);
    } else {
        if (csb->sense) {
	    address = GETOBJ(paddr, 1)->ip_addr;
	    port = GETOBJ(int, 1);
	} else {
	    address = 0;
	    port = 0;
	}
	if ((flow_export_address != address) || flow_export_port != port) {
	    close_ipsocket(flow_export_socket);
	    flow_export_address = address;
	    flow_export_port = port;
	    flow_export_socket = NULL;
	}
    }
    return(TRUE);
}


/*
 * carve_ip_flows
 *
 * This routine is passed a chunk of memory big enough for N buckets
 * and N flow cache entries.  It is imperitive that N is a power of 2
 * in order that the hash mask rounds properly.
 */
static void carve_ip_flows (void *memptr, ulong cnt)
{
    ipflow_t *flow;
    
    /*
     * Temporary dealy for queueing flow stat packets
     * for process level sending.
     */
    queue_init(&flow_export_pakQ, 0);

    /*
     * The first chunk of memory is reserved for the hash table.
     * The next contiguous chunk of memory is for the flow entries.
     */
    ipflowtbl.base = memptr;
    ipflowtbl.free_flows = cnt;
    ipflowtbl.hash_size = cnt;
    ipflowtbl.hash_mask = cnt - 1;
    ipflowtbl.hashtbl = memptr;
    flow = (ipflow_t *)&ipflowtbl.hashtbl->bucket[cnt];
    ipflowtbl.free = flow;
    while (--cnt) {
	flow->hash_next = flow + 1;
	flow++;
    }
    flow->hash_next = NULL;
}


boolean ipflow_table_init (void)
{
    ulong size, avail, entries;
    void *memptr;

    /*
     * If we already have flow memory available, bail now.
     */
    if (ipflowtbl.base)
      return(TRUE);
    
    /*
     * Let's not hog *all* of free memory!  How about limiting the
     * flow switching cache to one fourth of available free memory.
     */
    avail = mempool_get_free_bytes(MEMPOOL_CLASS_LOCAL,
				   MEMPOOL_ALTERNATE_INCLUDE) / 4;

    entries = 1 << 16; /* Start with max of 64K entries */
    size = entries * (sizeof(ipflow_t) + sizeof(ipflow_t *));
    for (; entries > BKTS_PER_PERIOD; entries >>= 1, size >>= 1) {
	if (size < avail) {
	    memptr = malloc_named_aligned(size, "IP flow cache", 32);
	    if (memptr) {
		carve_ip_flows(memptr, entries);
		return(TRUE);
	    }
	}
    }
    return(FALSE);
}

static void clear_ipflow_stats (void)
{
    ipflowtbl_t *tp = &ipflowtbl;
    ipflow_stats_t *stats;
    int idx;

    for (idx = 0; idx < IPFLOWSTAT_TOTAL; idx++) {
	stats = &ipflowstats[idx];
	stats->pkts = 0;
	stats->bytes = 0;
	stats->active = 0;
	stats->idle = 0;
	stats->flows = 0;
    }
    
    tp->valid_buckets = 0;
    tp->added = 0;
    tp->cur_hash_max = 0;
    tp->max_hash_max = 0;
    tp->no_flow_pkts = 0;
    tp->export_records = 0;
    tp->export_no_pak = 0;
    tp->export_paks = 0;
    flowstat_seconds = 0;
}

static void free_resources (void)
{
    ipflow_t *flow, *next;
    ulong bkt;
    
    if (!ipflowtbl.base)
      return;

    /*
     * We shouldn't need to raise interrupt level, since we aren't
     * relinquishing context and no flow switching is enabled on
     * any interface (otherwise we wouldn't be here!).
     */
    for (bkt = 0; bkt < ipflowtbl.hash_size; bkt++) {
	flow = ipflowtbl.hashtbl->bucket[bkt];
	ipflowtbl.hashtbl->bucket[bkt] = NULL;
	while (flow) {
	    next = flow->hash_next;
	    ipflow_free(flow);
	    flow = next;
	}
    }
    ipflowtbl.hash_mask = 0;
    ipflowtbl.hash_size = 0;
    ipflowtbl.hashtbl = NULL;
    ipflowtbl.free = NULL;
    ipflowtbl.free_flows = 0;
    ipflowtbl.active_flows = 0;
    
    free(ipflowtbl.base);
    ipflowtbl.base = NULL;
    clear_ipflow_stats();
}


/*
 * If flow switching was once configured, but is no longer configured
 * on any interface, free up its resources.
 */
void ipflow_check_free_resources (void)
{
    hwidbtype *tmpidb;

    if (!ipflowtbl.base)
      return;
    
    FOR_ALL_HWIDBS(tmpidb)
      if (tmpidb->ip_routecache & IP_FLOW_ROUTECACHE)
	return;

    free_resources();
}

/* end of file */







