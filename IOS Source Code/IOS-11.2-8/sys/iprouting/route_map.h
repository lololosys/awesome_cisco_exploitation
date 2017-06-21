/* $Id: route_map.h,v 3.3.12.6 1996/05/04 01:22:42 wilber Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/route_map.h,v $
 *------------------------------------------------------------------
 * route_map.h 
 *
 * Route map support data structures.
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: route_map.h,v $
 * Revision 3.3.12.6  1996/05/04  01:22:42  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.3.12.5  1996/04/16  19:02:23  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.3.12.4  1996/04/05  03:46:42  rchandra
 * CSCdi53755:  BGP: flap dampening settings not flexible enough
 * Branch: California_branch
 * - add route-map support to set flap dampening parameters
 *
 * Revision 3.3.12.3  1996/04/02  07:29:32  dkatz
 * CSCdi52277:  Redistribution metric problems with IGRP/EIGRP
 * Branch: California_branch
 * Always store canonical metrics scaled for IGRP, and make EIGRP do
 * its own scaling internally.  Get the scaling right in all three
 * places.
 *
 * Revision 3.3.12.2  1996/03/28  23:59:20  rchandra
 * CSCdi52098:  BGP: exact keyword not accepted by route-map in community
 * match
 * Branch: California_branch
 * - fix my silly bug in handling exact community matching
 * - limit the range of valid community to 0xFFFF0000
 * - fix a cosmetic bug in show ip community-list
 *
 * Revision 3.3.12.1  1996/03/28  22:28:27  rchandra
 * CSCdi52880:  BGP: generate error message for unsupported route-map
 * options
 * Branch: California_branch
 * - generate error message when unsupported route-map option is used
 * - tread confederation EBGP neighbors similar to internal neighbors in
 *   best path selection
 * - fix the bug in bgp_compare_med, so that during MED comparison,
 *   confederation information is skipped properly
 * - confederation EBGP paths should be inserted into the IP table with
 *   internal distance
 * - do not consider common-AS neighbors as confederation neighbors
 * - during 'clear ip bgp dampening' avoid skipping paths
 * - undo the patch to the radix version walker
 *
 * Revision 3.3  1996/02/17  01:23:27  myeung
 * CSCdi49210:  match tag clause does not work with static routes
 *
 * Revision 3.2  1995/11/17  17:37:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:10:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/25  08:31:20  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.3  1995/07/12  07:19:12  tli
 * CSCdi36426:  Policy routing will not disable on an interface
 * - Policy routed packets were creating fastswitching cache entries.
 *   That's not till _next_ release.
 * - Add counters to record packet and byte count on a per route map entry
 *   basis.
 * - Marketing identified a catastrophic deficiency.  Implemented "set ip
 *   tos" and "set ip precedence" to cover this deficiency.
 *
 * Revision 2.2  1995/07/06  23:12:41  rchandra
 * CSCdi36759:  BGP : Add conditional per neighbor default generation
 * - add 'neighbor <x.x.x.x> default-originate route-map <tag>' command.
 *
 * Revision 2.1  1995/06/07  21:10:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __ROUTE_MAP_H__
#define __ROUTE_MAP_H__

#include "types.h"
#include "../clns/clns_route.h"

#define ROUTE_MAP_INIT_SEQ 10
#define ROUTE_MAP_MAX_COMMUNITY 16

/*
 * This is a head of queue pointer for a given route-map.
 */
struct route_map_headtype_ {
    struct route_map_headtype_ *next;
    uchar                      *map_tag;
    queuetype	               map_queue;
    ulong		       match_flags;
    ulong		       set_flags;
};

/*
 * Lists of various data types.
 */

typedef struct route_map_interface_ {
    struct route_map_interface_ *next;
    idbtype                     *idb;
} route_map_interface;

typedef struct route_map_addrtype_ {
    struct route_map_addrtype_ *next;
    addrtype		       address;
} route_map_addrtype;

/*
 * Match criteria related data structures.
 */
typedef struct route_map_community_ {
    ulong	com_array[ROUTE_MAP_MAX_COMMUNITY];
    int		count;
} route_map_community;

typedef struct route_map_match_value_ {
    struct route_map_match_value_ *next;
    ulong                          value;
} route_map_match_value;

typedef struct route_map_match_acl_ {
    struct route_map_match_acl_ *next;
    acl_headertype                *acl;
} route_map_match_acl;

typedef struct route_map_matchtype_ {
    ulong		      match_flags;
    route_map_match_acl       *ip_address;
    route_map_match_value     *clns_address;
    route_map_match_acl       *ip_next_hop;
    route_map_match_value     *clns_next_hop;
    route_map_match_acl       *ip_route_source;
    route_map_match_value     *clns_route_source;
    route_map_match_value     *metric;
    route_map_match_value     *route_type;
    route_map_match_value     *tag;
    route_map_interface       *interface;
    route_map_match_value     *as_path;
    route_map_match_value     *community;
    uint		       min_length;
    uint		       max_length;
} route_map_matchtype;

/*
 * Route type stored in route-maps.
 */
#define ROUTE_MAP_INTERNAL 0
#define ROUTE_MAP_EXTERNAL 1
#define ROUTE_MAP_LEVEL1   2
#define ROUTE_MAP_LEVEL2   3
#define	ROUTE_MAP_LOCAL	   4
#define ROUTE_MAP_EX_TYPE1 5
#define ROUTE_MAP_EX_TYPE2 6
/*
 * Metric supports both scalar and IGRP vector.
 */
typedef struct route_map_metrictype_ {
    boolean igrp_metric;
    union {
	ulong scalar;
	struct {
	    ulong bandwidth;
	    ulong user_bandwidth;
	    ulong delay;
	    ulong reliability;
	    ulong loading;
	    ushort mtu;
	} vector;
    } value;
} route_map_metrictype;

/*
 * Dampening parameters
 */
typedef struct route_map_dampening_ {
    ushort		halflife_time;
    ushort		reuse_limit;
    ushort		suppress_limit;
    ushort		maximum_stime;
}route_map_dampening;		
 
/*
 * Set criteria related data structures.
 */
typedef struct route_map_settype_ {
    ulong                set_flags;
    route_map_metrictype metric;
    ulong                metric_type;
    ulong                tag;
    ulong                level;
    addrtype             destination;
    ulong                mask;
    ulong                weight;
    ulong                local_pref;
    ulong		 destpref;
    ushort		 origin;
    ushort		 origin_as;
    route_map_community	 *community;
    ulong                flags;
    route_map_addrtype	 *default_next_hop;
    route_map_interface	 *default_interface;
    route_map_addrtype   *next_hop;
    route_map_interface  *interface;
    route_map_dampening  damp_value;
    ushort 		 *as_path;
    uchar		 ascount;
    uchar		 qos;
    uchar		 precedence;
} route_map_settype;

/*
 * NOTE : If you add new set, update set_array in route_map.c
 */
#define ROUTE_MAP_METRIC_SET     	0x00000001
#define ROUTE_MAP_METRICTYPE_SET 	0x00000002
#define ROUTE_MAP_TAG_SET        	0x00000004
#define ROUTE_MAP_AUTOTAG_SET    	0x00000008
#define ROUTE_MAP_LEVEL_SET      	0x00000010
#define ROUTE_MAP_DEST_SET       	0x00000020
#define ROUTE_MAP_LOCALPREF_SET  	0x00000040
#define ROUTE_MAP_WEIGHT_SET     	0x00000080
#define ROUTE_MAP_NEXT_HOP_SET    	0x00000100
#define ROUTE_MAP_ORIGIN_SET     	0x00000200
#define ROUTE_MAP_ASPATH_PND_SET 	0x00000400
#define	ROUTE_MAP_COMMUNITY_ADD	 	0x00000800
#define ROUTE_MAP_ASPATH_TAG_SET 	0x00001000
#define ROUTE_MAP_DEFAULT_NEXT_HOP_SET  0x00002000
#define ROUTE_MAP_DEFAULT_INTERFACE_SET 0x00004000
#define ROUTE_MAP_INTERFACE_SET		0x00008000
#define ROUTE_MAP_QOS_SET		0x00010000
#define ROUTE_MAP_PRECEDENCE_SET	0x00020000
#define ROUTE_MAP_COMMUNITY_SET		0x00040000
#define ROUTE_MAP_DAMPENING_SET		0x00080000
#define ROUTE_MAP_DESTPREF_SET          0x00100000

#define ROUTE_MAP_METRIC_ADD	 0x01
#define ROUTE_MAP_METRIC_SUB	 0x02
#define ROUTE_MAP_MAX_ASPATH	 10

/*
 * These values *must* be consistent with the ones in the parse chain.
 */
#define ROUTE_MAP_SET_TYPE1    0
#define ROUTE_MAP_SET_TYPE2    1
#define ROUTE_MAP_SET_INTERNAL 2
#define ROUTE_MAP_SET_EXTERNAL 3
#define ROUTE_MAP_SET_LEVEL1   0
#define ROUTE_MAP_SET_LEVEL2   1
#define ROUTE_MAP_SET_LEVEL12  2
#define ROUTE_MAP_SET_STUB     3
#define ROUTE_MAP_SET_BACKBONE 4

/*
 * Route map data structure. One for each route-map command entry. This
 * data structure is pointed to by route_map_headtype. 
 *
 * Index field is used to number the route-map entries without
 * holes. They are usefull for speedier access of data structures
 * corresponding to each route-map entry.
 */
struct route_maptype_ {
    struct route_maptype_ *next;
    route_map_headtype    *head;
    boolean               grant;
    int                   seq_num;
    int			  index;
    route_map_matchtype   match_list;
    route_map_settype     set_list;
    ulong		  pkt_count;
    ulong		  byte_count;
};

/*
 * Constants for matches/sets supported by caller of route_map_ip_check() or
 * route_map_clns_check().
 * NOTE : If you add new match, update match_array in route_map.c
 */
#define ROUTE_MAP_MATCH_IPADDRESS       	0x0001
#define ROUTE_MAP_MATCH_IPNEXTHOP       	0x0002
#define ROUTE_MAP_MATCH_IPROUTESOURCE   	0x0004
#define ROUTE_MAP_MATCH_CLNSADDRESS     	0x0008
#define ROUTE_MAP_MATCH_CLNSNEXTHOP     	0x0010
#define ROUTE_MAP_MATCH_CLNSROUTESOURCE 	0x0020
#define ROUTE_MAP_MATCH_INTERFACE       	0x0040 
#define ROUTE_MAP_MATCH_METRIC          	0x0080
#define ROUTE_MAP_MATCH_ROUTETYPE              	0x0100
#define ROUTE_MAP_MATCH_TAG             	0x0200
#define ROUTE_MAP_MATCH_ASPATH          	0x0400
#define ROUTE_MAP_MATCH_COMMUNITY	 	0x0800
#define ROUTE_MAP_MATCH_EXACT_COMMUNITY		0x1000
#define ROUTE_MAP_MATCH_LENGTH			0x2000

/*
 * Constants for adding match community arguments.
 */
#define ROUTEMAP_MATCH_COMMARGS_BEGIN	1
#define ROUTEMAP_MATCH_COMMARGS		2


/*
 * Constants for the state change to a route-map
 */
#define ROUTE_MAP_ADD		1
#define ROUTE_MAP_DELETE	2
#define ROUTE_MAP_MODIFY	3

struct route_map_nametype_ {
    char *mapname;
    int userid;
    ulong match_flags;
    ulong set_flags;
    char *username;
    struct route_map_headtype_ *head;
};

/*
 * Macros for callers.
 */
#define ROUTE_MAP_IP_CONNECTED_MATCHES \
    (ROUTE_MAP_MATCH_IPADDRESS | ROUTE_MAP_MATCH_INTERFACE)

#define ROUTE_MAP_IP_STATIC_MATCHES \
    (ROUTE_MAP_MATCH_IPADDRESS | ROUTE_MAP_MATCH_IPNEXTHOP |          \
     ROUTE_MAP_MATCH_INTERFACE | ROUTE_MAP_MATCH_TAG)

#define ROUTE_MAP_OSPF_MATCHES \
    (ROUTE_MAP_MATCH_IPADDRESS | ROUTE_MAP_MATCH_IPNEXTHOP |          \
     ROUTE_MAP_MATCH_IPROUTESOURCE | ROUTE_MAP_MATCH_INTERFACE |      \
     ROUTE_MAP_MATCH_METRIC | ROUTE_MAP_MATCH_ROUTETYPE | ROUTE_MAP_MATCH_TAG)

#define ROUTE_MAP_IGRP1_MATCHES \
    (ROUTE_MAP_MATCH_IPADDRESS | ROUTE_MAP_MATCH_IPNEXTHOP |          \
     ROUTE_MAP_MATCH_IPROUTESOURCE | ROUTE_MAP_MATCH_INTERFACE |      \
     ROUTE_MAP_MATCH_METRIC)

#define ROUTE_MAP_IGRP2_MATCHES \
    (ROUTE_MAP_MATCH_IPADDRESS | ROUTE_MAP_MATCH_IPNEXTHOP |          \
     ROUTE_MAP_MATCH_IPROUTESOURCE | ROUTE_MAP_MATCH_INTERFACE |      \
     ROUTE_MAP_MATCH_METRIC | ROUTE_MAP_MATCH_ROUTETYPE | ROUTE_MAP_MATCH_TAG)

#define ROUTE_MAP_RIP_MATCHES   ROUTE_MAP_IGRP1_MATCHES

#define ROUTE_MAP_HELLO_MATCHES ROUTE_MAP_IGRP1_MATCHES

#define ROUTE_MAP_EGP_MATCHES   ROUTE_MAP_IGRP1_MATCHES

#define ROUTE_MAP_BGP_MATCHES \
    (ROUTE_MAP_IGRP1_MATCHES | ROUTE_MAP_MATCH_TAG | ROUTE_MAP_MATCH_ASPATH | \
     ROUTE_MAP_MATCH_ROUTETYPE | ROUTE_MAP_MATCH_COMMUNITY)

#define ROUTE_MAP_BGP_NETWORK_MATCHES \
    (ROUTE_MAP_MATCH_IPADDRESS | ROUTE_MAP_MATCH_IPNEXTHOP | \
     ROUTE_MAP_MATCH_IPROUTESOURCE | ROUTE_MAP_MATCH_ROUTETYPE)

#define ROUTE_MAP_BGP_ATTRIBUTE_MATCHES \
    (ROUTE_MAP_MATCH_ASPATH | ROUTE_MAP_MATCH_COMMUNITY)

#define ROUTE_MAP_CLNS_STATIC_MATCHES \
    (ROUTE_MAP_MATCH_CLNSADDRESS | ROUTE_MAP_MATCH_CLNSNEXTHOP |      \
     ROUTE_MAP_MATCH_INTERFACE)

#define ROUTE_MAP_ISIS_MATCHES \
    (ROUTE_MAP_MATCH_IPADDRESS | ROUTE_MAP_MATCH_IPNEXTHOP |          \
     ROUTE_MAP_MATCH_IPROUTESOURCE | ROUTE_MAP_MATCH_CLNSADDRESS |    \
     ROUTE_MAP_MATCH_CLNSNEXTHOP | ROUTE_MAP_MATCH_CLNSROUTESOURCE |  \
     ROUTE_MAP_MATCH_INTERFACE | ROUTE_MAP_MATCH_METRIC |             \
     ROUTE_MAP_MATCH_ROUTETYPE | ROUTE_MAP_MATCH_TAG)

#define ROUTE_MAP_ISO_IGRP_MATCHES \
    (ROUTE_MAP_MATCH_CLNSADDRESS | ROUTE_MAP_MATCH_CLNSNEXTHOP |      \
     ROUTE_MAP_MATCH_CLNSROUTESOURCE | ROUTE_MAP_MATCH_INTERFACE |    \
     ROUTE_MAP_MATCH_METRIC)

#define ROUTE_MAP_ODR_MATCHES \
    (ROUTE_MAP_MATCH_IPADDRESS | ROUTE_MAP_MATCH_INTERFACE |	      \
     ROUTE_MAP_MATCH_IPNEXTHOP)

/*
 * structure to hold information about users of route-map.
 */
typedef struct route_map_userinfo_ {
    int		userid;
    ulong	set_flags;
    ulong	match_flags;
    char 	*username;
} route_map_userinfo;

/*
 * External function definitions.
 */
extern void route_map_command(parseinfo *);
extern void route_map_nvgen(parseinfo *);
extern route_map_headtype *route_map_head_add(uchar *);
extern route_maptype *route_map_add(route_map_headtype *, route_maptype *, 
				    boolean, int);
extern boolean route_map_delete(route_map_headtype *, route_maptype *);
extern void route_map_free_data(route_maptype *);
extern void route_map_free_item(route_map_match_value **);
extern void route_map_free_community(parseinfo *);
extern void route_map_match_free_address(route_maptype *, int, int);
extern route_map_headtype *find_route_map_head(uchar *);
extern route_maptype *find_route_map(route_map_headtype *, int);
extern ulong route_map_hash(uchar *);
extern void route_map_match_interface_command(parseinfo *);
extern void route_map_match_metric_command(parseinfo *);
extern void route_map_match_tag_command(parseinfo *);
extern void route_map_match_aspath_command(parseinfo *);
extern void route_map_match_community_command(parseinfo *);
extern void route_map_match_routetype_command(parseinfo *);
extern void route_map_match_address_command(parseinfo *);
extern void route_map_match_nexthop_command(parseinfo *);
extern void route_map_match_routesource_command(parseinfo *);
extern void route_map_process_match_item(route_map_match_value **, ulong,
					 boolean);
extern void route_map_process_match_sitem(route_map_match_value **, char *,
					 boolean);
extern void route_map_process_match_acl_item(route_map_match_acl **,
					     int, char *, boolean, boolean);
extern void route_map_match_nvgen(parseinfo *);
extern void route_map_nvadd_address(route_map_match_value *, boolean);
extern void route_map_nvadd_number(route_map_match_value *);
extern void route_map_set_nvgen(parseinfo *);
extern void route_map_set_dest_command(parseinfo *);
extern void route_map_set_metric_command(parseinfo *);
extern void route_map_set_metrictype_command(parseinfo *);
extern void route_map_set_tag_command(parseinfo *);
extern void route_map_set_autotag_command(parseinfo *);
extern void route_map_set_localpref_command(parseinfo *);
extern void route_map_set_destpref_command(parseinfo *);
extern void route_map_set_community_command(parseinfo *);
extern void route_map_set_weight_command(parseinfo *);
extern void route_map_set_origin_command(parseinfo *);
extern void route_map_set_dampening_command(parseinfo *);
extern void route_map_set_as_path_prepend_command(parseinfo *);
extern void route_map_set_as_path_prepend_fresh_command(parseinfo *);
extern void route_map_set_as_path_tag_command(parseinfo *);
extern void route_map_set_level_command(parseinfo *);
extern void route_map_set_next_hop_command(parseinfo *);
extern void route_map_set_interface_command(parseinfo *);
extern route_maptype *route_map_ip_check(uchar *, ndbtype *, pdbtype *);
extern route_maptype *route_map_default_check (uchar *);
extern ulong route_map_ip_matches_supported(ulong);
extern boolean route_map_match_ipaddress(ipaddrtype, ipaddrtype,
					 route_map_match_acl *);
extern boolean route_map_match_number(ulong, route_map_match_value *);
extern boolean route_map_match_routetype(ndbtype *, route_map_match_value *);
extern boolean route_map_match_aspath(char *, route_map_match_value *);
extern route_maptype *route_map_clns_check(uchar *, clns_ddbtype *, 
					   clns_rdbtype *);
extern ulong route_map_clns_matches_supported(ulong);
extern boolean route_map_match_clnsaddress(nsap_addr *, 
					   route_map_match_value *);
extern void route_map_inform_protocols(char *, int event);
extern void route_map_eols(parseinfo *);
extern void show_route_map(parseinfo *);
extern void display_one_route_map(route_map_headtype *);
extern ndbtype *route_map_find_ndb(uchar *);
extern ndbtype *route_map_use_accesslist(route_map_match_acl *);
extern boolean route_map_config(route_map_nametype **, char *,
				const route_map_userinfo *);
extern void route_map_deconfig(route_map_nametype **);
extern void route_map_sanity_check(route_map_nametype *);

#endif __ROUTE_MAP_H__
