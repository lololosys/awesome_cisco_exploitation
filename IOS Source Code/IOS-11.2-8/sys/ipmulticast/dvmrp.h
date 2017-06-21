/* $Id: dvmrp.h,v 3.8.10.4 1996/09/11 16:54:42 dino Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/dvmrp.h,v $
 *------------------------------------------------------------------
 * dvmrp.h - Structure and Constant defintions for DVMRP.
 *
 * August, 1993.
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: dvmrp.h,v $
 * Revision 3.8.10.4  1996/09/11  16:54:42  dino
 * CSCdi68465:  RP information is too difficult to change in last-hop
 * routers
 * Branch: California_branch
 *
 * Revision 3.8.10.3  1996/06/28  07:48:18  dino
 * CSCdi61612:  Put in DVMRP route-limit, have it default to 7000, avoid
 *              MBONE meltdown.
 * Branch: California_branch
 *
 * Revision 3.8.10.2  1996/06/21  18:43:19  tylin
 * CSCdi58913:  DVMRP control packets are not counted in the tunnel idb
 * stats
 * Branch: California_branch
 *
 * Revision 3.8.10.1  1996/05/14  00:12:27  mleelani
 * CSCdi56461:  European cisco MBONE needs ip dvmrp metric-offset out
 * command
 * Branch: California_branch
 * Support outgoing DVMRP metric offsets.
 *
 * Revision 3.8  1996/02/22  19:27:09  lwei
 * CSCdi49375:  Missing DVMRP graft on PIM/DVMRP border causes join latency
 *
 * Revision 3.7  1996/01/10  22:04:33  dino
 * CSCdi46682:  DVMRP accept-filters cant filter out default route.
 *
 * Revision 3.6  1996/01/09  20:00:25  dino
 * CSCdi46618:  SNMP mrinfo bit is 4 bits to the left from where it is
 *              suppose to be
 *
 * Revision 3.5  1995/12/30  21:16:06  dino
 * CSCdi46259:  mrouted 3.8 sets the S-bit in mrinfo responses, cisco
 *              should too.
 *
 * Revision 3.4  1995/12/03  18:06:28  dino
 * CSCdi44874:  no ip dvmrp debug detail turns on debugging.
 *
 * Revision 3.3  1995/11/21  23:11:58  dino
 * CSCdi43427:  cisco does not process DVMRP prunes.
 *
 * Revision 3.2  1995/11/17  17:29:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:00:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.12  1995/10/10  21:15:54  dino
 * CSCdi41878:  Monitor number of DVMRP routes received and errmsg()
 * during surges.
 *
 * Revision 2.11  1995/10/06  09:00:39  dino
 * CSCdi41679:  mtrace should return non-responding router information
 *
 * Revision 2.10  1995/10/05  14:20:55  widmer
 * CSCdi41469:  More Mtrace bugs found during EFT testing.
 * Missed file in previous commit
 *
 * Revision 2.9  1995/08/22  21:54:22  dino
 * CSCdi39110:  DVMRP version number should be added to all packets, fix
 * style
 *
 * Revision 2.8  1995/08/15  21:22:09  chwhite
 * CSCdi38823:  Code review comments and minor bug fixes
 *
 * Revision 2.7  1995/08/09  23:14:15  asastry
 * CSCdi38504:  Limit DVMRP debug output using access lists for the
 * command 'debug ip dvmrp detail [in | out]'
 *
 * Revision 2.6  1995/08/03  01:12:55  chwhite
 * CSCdi38104:  It would be nice if the router supported mtrace/mrinfo.
 *              Added support for mbone traceroute and mrinfo
 *
 * Revision 2.5  1995/07/24  07:33:31  hampton
 * Transition IP Multicast support to use the passive timer macros for all
 * its timers.  It no longer references the system clock directly.
 * [CSCdi37539]
 *
 * Revision 2.4  1995/07/18 07:32:40  asastry
 * CSCdi37082:  Enhance 'debug ip dvmrp detail' to allow '[in | out]'.
 *
 * Revision 2.3  1995/07/01  07:46:10  dino
 * CSCdi27921:  Border router needs to send Registers to RP. ISPs want PIM
 *              over DVMRP unicast routing.
 *
 * Revision 2.2  1995/06/10  20:39:48  jzwiebel
 * Enabled neighbor lists and all flags for DVMRP ask2_neighbor packets.
 *
 * CSCdi35660:  DVMRP: enable ask2_neighbor flags and neighbor list
 *
 * Revision 2.1  1995/06/07  21:03:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __DVMRP_H__
#define __DVMRP_H__

/*
 * DVMRP constants.
 */
#define DVMRP_MAX_SIZE        576
#define DVMRP_REPORT_INTERVAL (60 * ONESEC)

#define DVMRP_PROBE_CODE      1
#define DVMRP_REPORT_CODE     2
#define DVMRP_ASK_NBR_CODE    3
#define DVMRP_NBR_CODE        4
#define DVMRP_ASK_NBR2_CODE   5
#define DVMRP_NBR2_CODE       6
#define DVMRP_PRUNE_CODE      7
#define DVMRP_GRAFT_CODE      8
#define DVMRP_GRAFT_ACK_CODE  9

#define DVMRP_NBR_EXPIRATION  (DVMRP_REPORT_INTERVAL * 3)
#define DVMRP_ENTRY_SIZE      8
#define DVMRP_INFINITY        32
#define DVMRP_END             0x80
#define DVMRP_MAX_METRIC      32
#define DVMRP_MAX_FILTER      32
#define DVMRP_EXPIRATION_TIME (DVMRP_REPORT_INTERVAL * 3)
#define DVMRP_PRUNE_TIME      (300 * ONESEC)  
#define DVMRP_MIN_PRUNE_TIME  (3 * ONESEC)       
#define DVMRP_GRAFT_FREQUENCY (5 * ONESEC)
#define DVMRP_AGING_RATE      10
#define DVMRP_DEFAULT_METRIC_OFFSET_IN  1
#define DVMRP_DEFAULT_METRIC_OFFSET_OUT 0
#define DVMRP_CLEAR_ROUTES_RCVD (ONEMIN + (5 * ONESEC))
#define DVMRP_DEFAULT_COMPLAINT_COUNT 10000
#define DVMRP_DEFAULT_SUMMARY_METRIC 1

/*
 * DVMRP_ASK_NBR2 flags.
 */
#define DVMRP_TUNNEL_FLAG  0x01
#define DVMRP_TUNNEL_SR    0x02  /* tunnel uses IP source routing */
#define DVMRP_PIM_FLAG     0x04  /* report is from a PIM router */
#define DVMRP_DOWN_FLAG    0x10
#define DVMRP_DISABLE_FLAG 0x20
#define DVMRP_QUERIER_FLAG 0x40
#define DVMRP_LEAF         0x80  /* report is from a leaf router */

/*
 * DVMRP ASK_NBR and ASK_NBR2 packet format.
 */
typedef struct dvmrp_asknbrtype_ {
    ipaddrtype local_address; /* Our address on interface */
    uchar      metric;        /* We always set to 1 */
    uchar      threshold;     /* TTL threshold on tunnel */
    uchar      count_flags;   /* Nbr count for ask-nbr, flags for ask-nbr2 */
    uchar      data[0];       /* Nbr for ask-nbr, nbr count/nbr for ask-nbr2 */
} dvmrp_asknbrtype;

#define DVMRP_ASKNBR_SIZE 11  /* Includes neighbor address but not count */

/*
 * Prune packet type definition.
 */
typedef struct dvmrp_prunetype_ {
    ipaddrtype source;
    ipaddrtype group;
    ulong      prune_timer;
} dvmrp_prunetype;

/*
 * Graft and Graft-Ack packets have same format.
 */
typedef struct dvmrp_grafttype_ {
    ipaddrtype source;
    ipaddrtype group;
} dvmrp_grafttype;

/*
 * DVMRP Report metric structure. Used to configure what unicast destinations
 * should be advertised in DVMRP Reports.
 */
typedef struct dvmrp_metrictype_ {
    ulong   metric;  /* Configured DVMRP for interface */
    idbtype *idb;    /* What interface does it apply to */
    acl_headertype *acl;   /* Access-list for destinations metric applies to */
    pdbtype *pdb;    /* Interrogate routes learned from a specific protocol */
    boolean dvmrp;   /* Use DVMRP routes */
    uchar   *route_map; /* Use route-map to filter routes */
} dvmrp_metrictype;

/*
 * DVMRP filter structure. Defines what DVMRP destinations that are accepted
 * when receiving DVMRP Reports.
 */
typedef struct dvmrp_filter_ {
    idbtype *idb;      /* Which interface filter applies to */
    acl_headertype *acl;       /* access-list for routes to apply filter */
    acl_headertype *nbr_acl;   /* access-list for neigbhors to apply filter */
    ulong   distance;  /* Administrative distance */
} dvmrp_filter;

/*
 * DVMRP summary data structure.
 */
typedef struct dvmrp_sumtype_ {
    struct dvmrp_sumtype_ *next;
    idbtype    *idb;         /* Which interface summary applies to */
    ipaddrtype address;      /* Route's IP address */
    ipaddrtype mask;         /* Route's IP netmask */
    ulong      metric;       /* Configured metric to advertise for summary */
    ulong      count;
} dvmrp_sumtype;
 
/*
 * DVMRP routing table structure. The DVMRP routing table is not used for
 * unicast routing. Its sole purpose is to compute an RPF interface when there
 * are DVMRP tunnels configured.
 */
typedef struct dvmrptype_ {
    rntype        radix[2];
    ipradixtype   dest;     /* IP unicast destination address */
    ipradixtype   mask;     /* IP mask for destination */
    ipaddrtype    next_hop; /* Next-hop router's IP address */
    idbtype       *idb;     /* Next-hop interface to reach next-hop router */
    ulong         metric;   /* Metric in hops to destination */
    ulong         distance; /* Distance to compare with main routing table */
    sys_timestamp uptime;   /* How long entry has existed */
    sys_timestamp expires;  /* When entry will expire if no Report received */
} dvmrptype;

/*
 * DVMRP macro definitions.
 */
#define DVMRP_BUGINF if (dvmrp_debug) buginf
#define DVMRP_IN_BUGINF(Address,Str) \
    if (dvmrp_debug && \
	(dvmrp_debug_detail_flag & DVMRP_INCOMING_DETAIL_DEBUG) && \
	(fast_check(ip_dvmrp_acl, Address))) buginf Str
#define DVMRP_OUT_BUGINF(Address,Str) \
    if (dvmrp_debug && \
	(dvmrp_debug_detail_flag & DVMRP_OUTGOING_DETAIL_DEBUG) && \
	(fast_check(ip_dvmrp_acl, Address))) buginf Str
#define DVMRP_PRUNING_BUGINF \
    if (dvmrp_debug && (dvmrp_debug_detail_flag & DVMRP_PRUNING_DEBUG)) buginf

#define DVMRP_TUNNEL(Idb) (Idb->hwptr->tunnel && \
	   ((tunnel_info *) Idb->hwptr->tunnel)->mode == TUN_MODE_DVMRP_IP)
#define DVMRP_ROUTES (dvmrp_cache)
#define DVMRP_VALID(Route) (Route && SLEEPING(Route->expires))
#define DVMRP_RPF_NBR(mdb) \
	   (DVMRP_ACTIVE(mdb->if_input) && \
		!pim_find_nbr(mdb->rpf_nbr, mdb->if_input))

/*
 * Version number fields for DVMRP. This long is in the group field of the
 * IGMP header. DVMRP_CISCO_VERSION is cisco's supported capabilities.
 */
#define DVMRP_LEAF_CAPABLE   0x00010000
#define DVMRP_PRUNE_CAPABLE  0x00020000
#define DVMRP_GENID_CAPABLE  0x00040000
#define DVMRP_MTRACE_CAPABLE 0x00080000
#define DVMRP_SNMP_CAPABLE   0x00100000
#define DVMRP_AUTORP_CAPABLE 0x00400000  /* Sent only in mrinfo responses */

#define DVMRP_CISCO_VERSION ((SYS_MINORVERSION << 8 | SYS_MAJORVERSION) | \
     DVMRP_PRUNE_CAPABLE | DVMRP_MTRACE_CAPABLE)

#define DVMRP_PRUNER(Idb, Version) \
    ((Idb->dvmrp_nbr_version & DVMRP_PRUNE_CAPABLE) || \
     ((Version & 0xff) >= 0x03 && (Version & 0xff00) <= 0x0400))

#define DVMRP_34(Version) ((Version & 0xffff) == 0x0403)

/*
 * ---------- External data declarations. ----------
 */
extern ulong dvmrp_unicast_routing;
extern sys_timestamp dvmrp_report_timer;
extern dvmrp_metrictype dvmrp_configured_metrics[DVMRP_MAX_METRIC];
extern ulong dvmrp_metric_count;
extern dvmrp_filter dvmrp_filters[DVMRP_MAX_FILTER];
extern ulong dvmrp_filter_count;
extern rnhtype *dvmrp_cache;
extern queuetype ip_mrinfoQ;
extern int ip_dvmrp_acl;
extern ulong dvmrp_skip_aging;
extern ulong dvmrp_route_limit;
extern ulong dvmrp_routehog_count;
extern ulong dvmrp_distance;

/*
 * ---------- External function declarations. ----------
 */

/*
 * dvmrp.c
 */
extern void dvmrp_metric_command(parseinfo *);
extern boolean dvmrp_receive_probe(idbtype *, ipaddrtype, igmptype *);
extern void dvmrp_init_idb(idbtype *);
extern void dvmrp_disable_idb(idbtype *);
extern void dvmrp_send_report(idbtype *, ipaddrtype);
extern void dvmrp_send_tunnel_report(igmptype **, paktype **, char **, 
				     ulong *, ipaddrtype, idbtype *, ulong *,
				     boolean *);
extern int dvmrp_build_report_entry(rntype *, va_list);
extern int dvmrp_setup_metric(ndbtype *, idbtype *, boolean);
extern void dvmrp_send_report_packet(paktype *, igmptype *, idbtype *, 
				     ipaddrtype, ulong);
extern paktype *dvmrp_getbuffer(igmptype **, char **, ulong *);
extern void dvmrp_send_groups(idbtype *);
extern void dvmrp_receive_report(igmptype *, ipaddrtype, idbtype *, int);
extern boolean dvmrp_prefer_unicast_route(dvmrptype *, ndbtype *);
extern void dvmrp_add_route(ipaddrtype, ipaddrtype, ipaddrtype, idbtype *,
			    ulong, ulong);
extern dvmrptype *dvmrp_get_route(ipaddrtype, ipaddrtype, boolean);
extern void dvmrp_age_routes(void);
extern int dvmrp_age_one_route(rntype *, va_list);
extern int dvmrp_delete_route(dvmrptype *);
extern void dvmrp_clear_route(parseinfo *);
extern int dvmrp_clear_one_route(rntype *, va_list);
extern void dvmrp_show_route(parseinfo *);
extern int dvmrp_show_one_route(rntype *, va_list);
extern void dvmrp_accept_filter_command(parseinfo *);
extern boolean dvmrp_accept_filter(ipaddrtype, ipaddrtype, idbtype *, ulong *,
				   boolean);
extern idbtype *dvmrp_get_tunnel(iphdrtype *);
extern ipaddrtype dvmrp_get_tunnel_source(idbtype *);
extern void dvmrp_tunnel_accounting(paktype *, idbtype *);
extern void dvmrp_send_on_tunnel(paktype *, ulong, idbtype *);
extern void dvmrp_receive_ask_nbr(ipaddrtype, ipaddrtype, idbtype *, ulong,
				  boolean);
extern void dvmrp_send_asknbr2_request(ipaddrtype, ipaddrtype);
extern void dvmrp_receive_nbr2(igmptype *, paktype *, int);
extern void mrinfo_process_asknbr2_response(paktype *, ipaddrtype);
extern boolean ipname_match_number(nametype *, addrtype *);
extern void dvmrp_print_version(ulong, boolean);
extern void dvmrp_route_complaint(idbtype *);
extern void dvmrp_send_prune(mdbtype *, idbtype *);
extern void dvmrp_send_graft(gdbtype *, mdbtype *);
extern void dvmrp_send_graft_ack(ipaddrtype, idbtype *, ipaddrtype, 
				 ipaddrtype);
extern void dvmrp_receive_prune(paktype *, ulong, ipaddrtype, idbtype *,
				igmptype *);
extern void dvmrp_receive_graft_or_ack(paktype *, ulong, ipaddrtype, idbtype *,
				       igmptype *);

#endif __DVMRP_H__

