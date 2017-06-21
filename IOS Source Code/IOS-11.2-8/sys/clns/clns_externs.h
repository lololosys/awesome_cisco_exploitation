/* $Id: clns_externs.h,v 3.8.12.9 1996/08/08 14:50:15 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/clns/clns_externs.h,v $
 *------------------------------------------------------------------
 * clns_externs.h --  external variables for clns 
 *  
 * February 1989, by Jim Forster
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clns_externs.h,v $
 * Revision 3.8.12.9  1996/08/08  14:50:15  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Use new scheduler primitives.  Get rid of gratuitous type coercion.
 *
 * Revision 3.8.12.8  1996/08/07  08:56:59  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.8.12.7  1996/08/04  21:34:12  dkatz
 * CSCdi64354:  ESIS bonus ISHs quashed
 * Branch: California_branch
 * Remove arbitrarily complex logic around the sending of bonus ISHs;
 * rather, always send a rate-limited unicast ISH whenever a new
 * adjacency (ES or IS) is detected.  Send an ISH when the line
 * protocol comes up as well.
 *
 * Revision 3.8.12.6  1996/08/03  23:20:21  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.8.12.5  1996/07/26  18:13:54  tylin
 * CSCdi59808:  clns ping fails between 7000 ethernet interfaces
 *    Fix dialer i/f checking by calling is_ddr().
 *    Update driver's filter table for all ES/IS multicast addresses
 *    whenever enable/disable CLNS routing.
 * Branch: California_branch
 *
 * Revision 3.8.12.4  1996/07/11  18:29:12  asastry
 * CSCdi60438:  CLNS fails to work over DDR unless dialer strings are
 * used. Use next hop NSAP to do the dialer look-up.
 *
 * Branch: California_branch
 *
 * Revision 3.8.12.3  1996/05/04  20:03:53  asastry
 * CSCdi56559:  CLNS cache invalidations are too frequent, causing high
 * CPU utilization.
 * Branch: California_branch
 *
 * Revision 3.8.12.2  1996/04/19  17:29:53  asastry
 * CSCdi54904:  Fix extended option for CLNS ping to correctly handle
 * source NSAPs
 * Branch: California_branch
 *
 * Revision 3.8.12.1  1996/04/06  00:35:53  asastry
 * CSCdi53023:  ISO-IGRP should poison redistributed routes if the
 * redistribution is disabled.
 *
 * Branch: California_branch
 *
 * Revision 3.8  1996/02/13  19:44:03  hampton
 * Reorder routines to remove platform specific extern statements in
 * fastswitching code.  [CSCdi48844]
 *
 * Revision 3.7  1996/01/25  22:18:53  rchandra
 * CSCdi44688:  ISO-IGRP: 4k based images cannot interoperate with 68k
 * images
 * - do not super-impose structure on the byte stream while generating and
 *   parsing update
 *
 * Revision 3.6  1996/01/23  02:38:07  asastry
 * CSCdi47315:  DECnet IV/V conversion makes bogus entries in ISO-IGRP L2
 * routing table
 *
 * Revision 3.5  1996/01/11  02:20:57  jenny
 * CSCdi46513:  TCP/IP need code clean up (part 2)
 * . removed clns_tuba.c.
 * . removed all registry calls to TUBA code in CLNS subsystem.
 *
 * Revision 3.4  1995/12/01  17:01:32  dkatz
 * CSCdi43236:  ES adjacencies may not appear in DR LSP
 *
 * Revision 3.3  1995/11/17  08:52:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:12:11  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:07:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/10/21  19:02:50  dkatz
 * CSCdi42558:  ISIS needs more debugging hooks
 * Provide a means to look at LSP database internal information.
 * Provide a means to flush the LSP database.
 *
 * Revision 2.4  1995/10/11  23:57:29  dkatz
 * CSCdi41968:  CLNS ER PDUs all messed up
 * Calculate data length properly.  Copy the data from the right place.
 * Fix the debug routine so that it properly displays the packet.
 *
 * Revision 2.3  1995/09/23  21:37:19  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Calculate routes from only changed LSPs if the inter-router topology
 * stays constant.  Don't gratuitously install IP routes that will
 * be immediately replaced, as this causes large-scale cache invalidation.
 * Eliminate multiple IP forwarding table traversals.
 *
 * Revision 2.2  1995/08/08  16:42:41  hampton
 * Convert CLNS to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi38392]
 *
 * Revision 2.1  1995/06/07 20:17:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../decnet/decnet.h"
#include "../clns/clns.h"
#include "../clns/clns_route.h"
#include "../clns/clns_adj.h"
#include "../clns/iso_igrp.h"
#include "config.h"

#ifndef __CLNS_EXTERNS_H__
#define __CLNS_EXTERNS_H__

extern NSAP_T clns_myNET;	/* Network Entity Title 
				   Must have an NET before can bring up
				   the clns processes or an interface */
extern ulong clns_nnets;	/* # of interfaces configured for clns
				   if clns_nnets >1 then we are an IS */
extern const char nbr_hdr[];
extern watched_queue *clnsQ;
extern queuetype esisQ;
extern queuetype clns_echoQ;		/* clns echo queue */
extern queuetype clns_protoQ;           /* protocol processes */
extern queuetype clns_local_nets;       /* Local NETs configured */

extern ulong clns_traffic[CLNS_MAXTRAFFIC];

extern boolean clns_running;
extern boolean clns_router_running;
extern boolean clns_processes_setup;
extern boolean clns_security_passthru;
extern boolean clns_packet_debug;
extern boolean clns_event_debug;
extern boolean clns_filter_debug;
extern boolean esis_packet_debug;
extern boolean esis_event_debug;
extern boolean clns_routing_debug;
extern boolean clns_igrp_debug;

extern ulong clns_config_timer;
extern ulong clns_holding_timer;
extern ulong clns_esct_timer;
extern ushort clns_packet_lifetime;

extern hwaddrtype all_ess;			/* all end systems */
extern hwaddrtype all_iss;			/* all intermediates systems */

extern clns_ddbtype *clns_routes[CLNS_NETHASHLEN]; /* prefix routing table */
extern clns_ddbtype *clns_default_prefix;          /* Default prefix route */
extern int clns_cache_version;			/* cache version number */
extern int clns_cache_changes;			/* cache changes */
extern clns_cachetype *clns_cache[CLNS_CACHE_SIZE];
extern clns_pdbtype *clns_staticpdb;            /* static pdb */
extern queuetype clns_staticQ;                  /* static routes */

/*
 * ES specific variables
 */
extern clns_adjtype dummy_adj;	/* for 'configuration requests', when
				   we broadcast the pdu */
extern boolean clns_want_erpdus;	/* do we want erpdu's back on locally
					 * generated packets ? */

/*
 * ES-IS Variables
 */
extern mgd_timer esis_timer;		/* when to run periodic esis tasks */
extern mgd_timer esis_ager_timer;	/* when to run esis_ager() */
extern mgd_timer esis_hello_timers;	/* when to run send hellos */
extern const char clns_echo_opt_prompt[];

/*
 * External declarations
 */

extern void clns_add_prefixroute(iso_igrp_prefixroute *prefixroute, 
				 idbtype *idb, NSAP_T first_hop, 
				 clns_pdbtype *pdb, hwaddrtype *snpa);
clns_rdbtype *clns_route_update(NSAP_T nsap_prefix, NSAP_T first_hop, 
				idbtype *first_hop_idb, int entry_type, 
				ulong protocol_source, ulong distance, 
				ulong validation_time, 
				sys_timestamp expiration_time,
				int metric, NSAP_T source_NET);
extern void clns_update_prefixroute(clns_ddbtype *dest, 
				    iso_igrp_prefixroute *prefixroute, 
				    clns_pdbtype *pdb, idbtype *idb, 
				    NSAP_T first_hop, hwaddrtype *snpa);
extern void iso_igrp_setup_metric(clns_rdbtype *, idbtype *);
extern char *area_string(uchar station_id[]);
extern clns_pdbtype *clns_find_pdb(NSAP_T nsap);
extern clns_pdbtype *clns_find_level2_pdb(NSAP_T nsap);
extern clns_ddbtype *clns_find_dest(clns_pdbtype *pdb, NSAP_T id);
extern boolean clns_rate_limit_erpdu(paktype *, idbtype *);
extern clns_pdbtype *clns_find_router_name(char *router_name);
extern NSAP_T *clns_get_local_npai(clns_pdbtype *pdb);
extern paktype *clns_getbuffer(int size);
extern char *clns_hellostring(int htype);
extern clns_pdbtype *clns_igrp_pdb_init(NSAP_T nsap, char *router_name, int level, 
					boolean dynamic);
extern clns_pdbtype *clns_igrp_pdb_initialize(char *router_name);
extern char *clns_option_string(uchar *opt);
extern paktype *clns_segment(paktype *old_pak, clns_adjtype *adj);
extern paktype *clns_send_redirect(paktype *pak, clns_adjtype *adj);
extern char *discard_string(int code);
extern char *domain_string(char domain_length, uchar domain_id[]);
extern uchar *get_area(NSAP_T nsap);
extern uchar *get_station(NSAP_T nsap);
extern uchar get_selector(NSAP_T nsap);
extern clns_ddbtype *getnextprefix(clns_ddbtype *dest,
				   idbtype *interface);
extern clns_ddbtype *getnextdest(clns_pdbtype *pdb,int *hash_index, 
				 clns_ddbtype *dest,idbtype *interface,
				 clns_rdbtype **rdb);
extern clns_ddbtype *clns_walk_ddb(clns_pdbtype *pdb,int *hash_index, 
				   clns_ddbtype *dest);
enum {CLEAR_NBRS_ALL, CLEAR_NBRS_IS, CLEAR_NBRS_ES};
extern void clear_clns_neighbors(int);
extern int clns_show_igrp_routes(clns_pdbtype *, int);
extern void clns_metric_command(clns_pdbtype *, parseinfo *);
extern void clns_basic_timers(clns_pdbtype *, parseinfo *);
extern void iso_igrp_sendhello(clns_pdbtype *pdb, idbtype *interface, 
			       hwaddrtype *snpa);
extern void iso_igrp_sendupdate(clns_pdbtype *pdb, idbtype *interface,
				boolean poison_prefix_routes,
				clns_pdbtype *rpdb, 
				clns_ddbtype *specific_prefix);
extern clns_rdbtype *iso_igrp_destination_update(clns_pdbtype *pdb, 
	 ushort type, uchar *id, idbtype *interface, NSAP_T gateway, 
	 hwaddrtype *snpa, ulong metric, ulong flags, int rib_type, 
	 ulong expiration_interval);
extern void iso_igrp_add_static_nbrs(clns_pdbtype *pdb, idbtype *idb);
extern char *next_buf(void);
extern clns_cache_adjtype *nsap_cache_search(NSAP_T nsap);
extern void clns_route_adjust(idbtype *idb);
extern boolean clns_route_search (uchar *prefix);
extern void clns_discard_route_adjust(uchar *, boolean);
extern boolean clns_adjust_static_routes(clns_adjtype *, idbtype *, boolean,
					 boolean);
extern void clns_remove_idb_depend(idbtype *idb);
extern void clns_remove_neighbor_depend(clns_pdbtype *pdb, NSAP_T next_hop);
extern void nsap_cache_delete(NSAP_T nsap);
extern clns_ddbtype *nsap_lookup(NSAP_T nsap, uchar match_type);
extern void clns_make_cache_adj(clns_cachetype *cache, clns_adjtype *adj);
extern clns_cachetype *clns_make_route_cache(NSAP_T dest_nsap, 
					     clns_adjtype *adj);
extern clns_ddbtype *nsap_next(clns_ddbtype *ddb);
extern char *nsap_string(NSAP_T nsap);
extern char *prefix_string(NSAP_T prefix);
extern char *snpa_string(hwaddrtype *snpa);
extern clns_ddbtype *dest_create(clns_pdbtype *pdb,
				 NSAP_T id);
extern char *long_area_string(NSAP_T area_addr);
extern char *station_string(uchar area_id[]);
extern char *systemid_string(uchar system_id []);
extern char *systemid_name_string(clns_pdbtype *, uchar *systemid);
extern void build_nsap(int domain_length, uchar *domain, uchar *area, 
		       uchar *station, uchar *selector, NSAP_T *nsap);
extern void clear_nsap(NSAP_T nsap);
extern void clns_add_net(NSAP_T nsap);
extern char *build_domain_name(char *area_name);
extern boolean clns_add_intfc_router(char *router_name, idbtype *idb, 
				     int protocol, int level);
extern void clns_add_isis_router(clns_pdbtype *pdb);
extern boolean clns_add_router(char *router_name, NSAP_T routing_prefix, 
			       boolean dynamic, int protocol);
extern boolean clns_found_level1_pdb(clns_pdbtype *pdb);
extern void clns_make_common_domain_pdb(clns_pdbtype *domain_pdb, 
					clns_pdbtype *bogus_pdb);
extern void clns_rxtypes_update(idbtype *);
extern boolean clns_build_prefix_routes(clns_pdbtype *pdb, idbtype *interface, 
					iso_igrp_info_type **ptr, int *nbytes, 
					clns_ddbtype **dest,
					boolean poison_prefix_routes,
					clns_pdbtype *rpdb,
					clns_ddbtype *specific_prefix);
extern boolean clns_check_packet(paktype *pak);
extern void clns_nv_write_redist(clns_pdbtype *to_pdb, char *);
extern boolean clns_delete_intfc_router(clns_pdbtype *pdb, idbtype *idb);
extern boolean clns_delete_router(char *router_name);
extern void clns_dispose(paktype *pak, int code, int spot);
extern void clns_echo(paktype *pak);
extern boolean clns_echoBLOCK(ulong arg);
extern void clns_echo_rep(paktype *pak);
extern boolean clns_encaps(paktype *pak, int length, idbtype *idb,
			   hwaddrtype *snpa, uchar *address);
extern boolean isis_encaps(paktype *pak, int length, idbtype *idb,
			   hwaddrtype *snpa);
extern void clns_enqueue(paktype *pak);
extern void clns_error_pdu(paktype *pak);
extern void clns_forward(paktype *pak, clns_adjtype *adj);
extern void clns_igrp_router(clns_pdbtype *pdb);
extern clns_adjtype *iso_igrp_route(clns_pdbtype *pdb, NSAP_T destination,
				    int depth);
extern process clns_input(void);
extern void clns_ppp_pad(paktype *pak);
extern void clns_macaddrs_update(idbtype *idb);
extern void clns_itf_enable(idbtype *idb);
extern void clns_itf_disable(idbtype *idb);
extern void clns_local_enqueue(paktype *pak);
extern boolean clns_opt_input(paktype *pak);
extern boolean clns_opt_postroute(paktype *pak);
extern boolean clns_parse_hostname(char *ptr, NSAP_T nsap);
extern boolean clns_ping_parse(char *, int, int *, char **, boolean *);
extern void clns_ping(char *);
extern void register_snmp_clns_echo(void);
extern void clns_print(clnshdrtype *);
extern void clns_send_unreach(paktype *pak);
extern int clns_set_pointers(paktype *pak);
extern int clns_setupprefix(clns_pdbtype *pdb, char *ptr, clns_ddbtype *dest,
			    boolean poison_prefix_routes);
extern void clns_setupmetric(clns_pdbtype *pdb, iso_igrp_metrictype *ptr, 
			     clns_ddbtype *dest, clns_rdbtype *rdb,
			     boolean poison_prefix_routes);
extern boolean clns_prefix_has_idb_up(clns_ddbtype *ddb);
extern void clns_show_one_neighbor(clns_pdbtype *pdb, clns_adjtype *adj,
				   char kind, boolean long_display,
				   boolean indent);
extern void clns_show_one_route(clns_ddbtype *ddb);
extern void clns_show_pdb_chain(char *router_name, 
				boolean (*rtn)(clns_pdbtype *));
extern void clns_show_protocol_source(ulong protocol_source);
extern boolean clns_show_router(clns_pdbtype *pdb);
extern void clns_trace(char *args);
extern boolean clns_write(paktype *pak);
extern boolean clns_xs_sendnet(paktype *pak, clns_adjtype *adj);
extern clns_adjtype *clns_route(NSAP_T destination, int depth);
extern clns_adjtype *clns_get_route(NSAP_T, clns_ddbtype *ddb, int depth);
extern void compute_iso_checksum(void *startptr, int length, void *cksumptr);
extern void esh_input(paktype *pak);
extern void send_one_is_hello(clns_pdbtype *, idbtype *, NSAP_T, hwaddrtype *,
			      char *);
extern void send_is_hellos(idbtype *, hwaddrtype *, char *);
extern void esis_ager(void);
extern boolean esis_routeBLOCK(/*queuetype * */ ulong q);
extern void esis_route_adjust(idbtype *);
extern void esis_router(void);
extern void force_hellos(void);
extern void ish_input(paktype *pak);
extern boolean iso_igrp_holddown(ulong old_metric, ulong new_metric);
extern ulong iso_igrp_metric(iso_igrp_metrictype *ptr, idbtype *interface,
			     clns_pdbtype *pdb);
extern void iso_igrp_route_delete(clns_pdbtype *pdb,clns_ddbtype *dest,
				  short index);
extern void iso_igrp_route_update(clns_rdbtype *rdb,
				  iso_igrp_metrictype *ptr,
				  idbtype *interface);
extern boolean iso_pdbBLOCK(ulong pdb);
extern boolean itf_in_pdb(idbtype *interface, clns_pdbtype *pdb);
extern void clns_manage_prefix_nbrs(clns_ddbtype *ddb, int protocol_source,
				    boolean sense);
extern void clns_staticQ_update (NSAP_T, NSAP_T, idbtype *, hwaddrtype *,
 				 boolean);
extern boolean clns_nsap_is_local(uchar *);
extern boolean clns_manage_all_prefix_nbrs(clns_pdbtype *, clns_pdbtype *, 
					   boolean);
extern void nsap_copy(NSAP_T ns_src, NSAP_T ns_dst);
extern void prefix_copy(NSAP_T prefix, NSAP_T nsap);
extern boolean nsap_delete(NSAP_T nsap);
extern boolean nsap_delete_rdb(NSAP_T, NSAP_T, idbtype *);
extern boolean nsap_equ(NSAP_T nsap1, NSAP_T nsap2);
extern boolean net_equ(NSAP_T nsap1, NSAP_T nsap2);
extern int nsap_hash(int length, uchar *nsap);
extern boolean nsap_insert(clns_ddbtype *ddb);
extern int nsap_match(NSAP_T nsap_prefix, NSAP_T dst_nsap);
extern boolean station_match(uchar *station1, uchar *station2);
extern boolean clns_domain_match(int length1, uchar *domain1, int length2, 
				 uchar *domain2);
extern boolean area_match(char *, char *);
extern int parse_clns_options(char *buff);
extern boolean parse_nsap(char *ptr, NSAP_T nsap);
extern void rd_input(paktype *pak);
extern void clns_get_ping_options (NSAP_T *nsap, NSAP_T source_nsap,
				   boolean *qos, boolean *segmentation,
				   boolean *padding, ushort *pattern,
				   boolean *validate, boolean *rotate_pattern,
				   boolean *verbose, boolean *sweep,
				   ulong *sweepmin, ulong *sweepmax,
				   ulong *sweepint);
extern boolean send_clns_echo(NSAP_T nsap, NSAP_T source_nsap, 
		clns_echo_cb_t *data, int bytes, boolean qos,
		boolean segmentation, boolean padding, boolean verbose);
extern void clns_redistribute(clns_pdbtype *pdb, clns_pdbtype *rpdb, 
			      int sense, char *map_name);
extern char *clns_redistribute_routemap(clns_pdbtype *, clns_pdbtype *, char *,
					boolean);
extern void clns_distance(clns_pdbtype *pdb, int distance);
extern void nv_clns_route(parseinfo *);
extern void show_clns_cache(void);
extern void show_clns_globals(void);
extern void show_clns_interface(idbtype *idb);
extern void show_clns_neighbors(clns_pdbtype *pdb, clns_adjtype **, char kind,
				idbtype *target, boolean long_display);
extern void show_clns_redirects(void);
extern void show_clns_traffic(void);
extern boolean verify_iso_checksum(void *startptr, int length, void *cksumptr);
extern void clns_pad_net(NSAP_T nsap);
extern char *clns_address_lookup(NSAP_T nsap);
extern char *clns_stationid_lookup(clns_pdbtype *pdb, uchar *stationid);
extern void clns_flash_update(ulong protocol_source);
extern void clns_fastsetup(hwidbtype *idb);
extern clns_pdbtype *find_isis_pdb(void);
extern void add_area_to_adj(NSAP_T area_address, clns_adjtype *adj);
extern boolean area_in_adj(NSAP_T area_address, clns_adjtype *adj);
extern void station_copy(uchar *, uchar *);
extern void systemid_copy(uchar *, uchar *);
extern void area_copy(NSAP_T src, NSAP_T dest);
extern void clns_ager(void);
extern boolean systemid_match(uchar *, uchar *);
extern ulong iso_igrp_compute_metric(clns_pdbtype *pdb, idbtype *interface);
extern boolean get_nsap_addr(char *string, nsap_addr *nsap, int *offset);
extern boolean get_nsap_pfx(char *string, nsap_pfx *prefix, int *offset);
extern boolean get_masked_nsap(char *string, masked_nsap *mskdnsap, 
			       int *offset);
extern int get_nsap_structure(char *string, void **value, int *offset,
			      boolean require_dots);
extern char *nsap_addr_string(nsap_addr *nsap);
extern char *nsap_pfx_string(nsap_pfx *pfx, boolean print_dots);
extern char *masked_nsap_string(masked_nsap *mskd);
extern void clns_show_filter_set(char *name, boolean name_present);
extern void clns_set_access_group(idbtype *idb, uchar *name, boolean in);
extern void clns_set_adj_filter(idbtype *, uchar *, boolean);
extern boolean clns_filter_addr(ulong index, nsap_addr *destaddr,
				nsap_addr *srcaddr);
extern boolean clns_accesscheck(paktype *, int);
extern void clear_clns_command(parseinfo *);
extern void clear_clns_routecommand(void);
extern boolean clns_on_idb(idbtype *);
extern ulong clns_filter_create(char *, ulong);
extern void clns_filter_destroy(ulong);
extern boolean clns_pdb_routemap(clns_pdbtype *, clns_ddbtype *);
extern void ip_clns_redistribute_metric_nvgen(pdbtype *, pdbtype *);
extern void ip_clns_redistribute_level_nvgen(pdbtype *, pdbtype *);
extern void ip_clns_redistribute_metric_printf(pdbtype *, pdbtype *);
extern void ip_clns_redistribute_level_printf(pdbtype *, pdbtype *);
extern void clns_refresh_my_net(void);
extern boolean clns_delete_rdb(clns_ddbtype *, int);
extern void clns_delete_all_rdbs(clns_ddbtype *);
extern idbqueuetype *clns_add_if_to_pdbqueue(idbtype *, clns_pdbtype *);
extern void clns_remove_idb_from_queue(clns_pdbtype *, idbtype *);
extern void clns_invalidate_cache(const char *, boolean);
extern void clns_bump_cache(hwidbtype *, idbtype *, boolean);
extern void show_clns_cache_invalidations(void);
extern void nv_clns_cache_write(parseinfo *);
extern void clns_cache_inval_init(ulong, ulong, ulong, ulong);
extern void show_clns_cache_delay(void);
extern boolean clns_perform_cache_flush(void);
extern void clns_set_pdb_running (clns_pdbtype *, boolean);
extern boolean clns_pdb_running (clns_pdbtype *);
extern void clns_pak_duplicate (paktype *, paktype **, long *);

/*
 * clns_adj.c
 */

extern void adjacency_ager(clns_adjtype **);
extern clns_adjtype *add_adjacency(clns_adjtype **, NSAP_T, idbtype *,
				   hwaddrtype *, long, boolean);
extern int compare_stationid(uchar *, uchar *);
extern int compare_area_address(uchar *, uchar *);
extern boolean delete_adjacency(clns_adjtype **, clns_adjtype *);
extern clns_adjtype *next_neighbor(clns_adjtype **, idbtype *, int,
				   clns_adjtype *, int *);
extern clns_adjtype *find_adjacency(clns_adjtype **, NSAP_T, boolean,
				    idbtype *, int);
extern boolean area_in_adj(NSAP_T, clns_adjtype *);
extern void add_area_to_adj(NSAP_T, clns_adjtype *);
extern void delete_interface_adjacencies(clns_adjtype **, idbtype *);
extern clns_adjtype **clns_create_adjacency_db(const char *);
extern void clns_mark_default_is_adj(clns_adjtype *);
extern clns_adjtype *clns_get_default_is_adj(void);

/*
 * clns_adj2.c
 */
extern void clns_delete_igrp_adj(clns_pdbtype *, clns_adjtype *);

/*
 * clns_chain.c
 */

extern void clns_parser_init(void);

/*
 * clns_media.c
 */

extern void clns_media_registry(void);

/*
 * clns_setup.c
 */
extern void clns_setup_global(setup_struct *);
extern void clns_setup_int(setup_struct *);
extern void clns_setup_end(setup_struct *);

#endif __CLNS_EXTERNS_H__
