/* $Id: isis_externs.h,v 3.5.12.11 1996/08/11 15:13:13 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/clns/isis_externs.h,v $
 *------------------------------------------------------------------
 * isis_externs.h -- External definitions for the IS-IS module.
 *
 * October 1991, Dino Farinacci
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: isis_externs.h,v $
 * Revision 3.5.12.11  1996/08/11  15:13:13  dkatz
 * CSCdi58739:  ISIS needs to be able to limit flooding
 * Branch: California_branch
 * Add phase I of mesh group support.  Allow an interface to be
 * configured to be part of a mesh group, or to be blocked for
 * flooding.  Allow CSNP transmission to be configured on point-to-
 * point interfaces as a minimal robustness mechanism.  Redefine
 * the SRM, etc., bits as an array of ulongs rather than bytes (to
 * speed up masking operations).  Change the interface number field
 * from a short to a long in order to shrink the code size by a few
 * hundred bytes.
 *
 * Revision 3.5.12.10  1996/08/08  14:50:47  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Use new scheduler primitives.  Get rid of gratuitous type coercion.
 *
 * Revision 3.5.12.9  1996/08/07  08:57:23  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.5.12.8  1996/08/04  23:35:10  dkatz
 * CSCdi63547:  ISIS database search is expensive
 * Branch: California_branch
 * Rewrite the dreaded isis_recursive_walk so that it no longer uses
 * recursion.  Fix "show isis tree" so that it works properly.  Clean up
 * references to isis_recursive_walk to make type coercion unnecessary.
 *
 * Revision 3.5.12.7  1996/06/18  16:50:49  dkatz
 * CSCdi59965:  ISIS retransmits too quickly
 * Branch: California_branch
 * Add a retransmission throttling mechanism.
 *
 * Revision 3.5.12.6  1996/06/18  16:31:31  dkatz
 * CSCdi58953:  cannot change ISIS type from L12 to L1
 * Branch: California_branch
 * Force the protocol to go to sleep long enough for all neighbor
 * adjacencies to time out when the level is changed.  While we're
 * in there, do it for NET changes as well (since these can change
 * adjacency levels).
 *
 * Revision 3.5.12.5  1996/05/26  06:32:17  dkatz
 * CSCdi58433:  ISIS/NLSP can starve input queue
 * Branch: California_branch
 * Do a clear_if_input on incoming packets before requeueing them for the
 * Update process.  Depth-limit the secondary queue to avoid buffer
 * starvation.
 *
 * Revision 3.5.12.4  1996/05/22  17:04:08  rchandra
 * CSCdi55055:  Route not found with clns isis (2500, 4000, 4500 and 7500
 *              platforms)
 * Branch: California_branch
 * - be careful about resetting the interface number during isis interface
 *   reset
 *
 * Revision 3.5.12.3  1996/05/07  17:39:58  dkatz
 * CSCdi56547:  ISIS/NLSP have problems with more than 42 neighbors
 * Branch: California_branch
 * Allow neighbors to be spread across multiple TLVs (on both send
 * and receive.)
 *
 * Revision 3.5.12.2  1996/05/03  01:50:50  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.5.12.1  1996/04/12  00:36:32  sluong
 * CSCdi52812:  show ipx nlsp database/neighbor needs to display all
 * instances
 * Branch: California_branch
 *
 * Revision 3.5  1996/02/19  21:32:34  dkatz
 * CSCdi48130:  ISIS timer debugging info needed
 * Add the ability to display managed timer chains.
 *
 * Revision 3.4  1995/12/01  17:01:43  dkatz
 * CSCdi43236:  ES adjacencies may not appear in DR LSP
 *
 * Revision 3.3  1995/12/01  02:48:28  dkatz
 * CSCdi39906:  CPUHOG Process = IS-IS Update
 *
 * Revision 3.2  1995/11/17  08:53:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:09:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  20:52:20  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/10/21  19:03:05  dkatz
 * CSCdi42558:  ISIS needs more debugging hooks
 * Provide a means to look at LSP database internal information.
 * Provide a means to flush the LSP database.
 *
 * Revision 2.7  1995/09/23  22:48:30  dkatz
 * CSCdi39102:  NLSP doesnt do numbered RIP if NLSP configured on i/f
 * Generate a pseudonode LSP for point-to-point links even if NLSP is
 * configured, so long as there are no active adjacencies.  Purge the
 * pseudonode LSP if an adjacency comes up.  Clean up some more stuff
 * out of the update process' block routine.  Reset the circuit ID to
 * the system ID, rather than zeroing it, when an adjacency goes down
 * on a point-to-point link.
 *
 * Revision 2.6  1995/09/23  21:37:39  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Calculate routes from only changed LSPs if the inter-router topology
 * stays constant.  Don't gratuitously install IP routes that will
 * be immediately replaced, as this causes large-scale cache invalidation.
 * Eliminate multiple IP forwarding table traversals.
 *
 * Revision 2.5  1995/08/25  15:24:26  dkatz
 * CSCdi38313:  ISIS may keep corrupted LSPs
 * Validate the LSP checksum before refreshing local LSPs.
 *
 * Revision 2.4  1995/08/25  05:40:02  dkatz
 * CSCdi38138:  ISIS needs adjacency state tracing
 *
 * Revision 2.3  1995/08/23  18:26:03  dkatz
 * CSCdi37901:  ISIS first SPF is unpredictable
 * Disentangle SPF triggering and holddown calculations.  Insert a short
 * delay before doing the first SPF run.  Add logging of SPF runs for
 * diagnosability.
 *
 * Revision 2.2  1995/08/23  18:03:09  dkatz
 * CSCdi37692:  LSP checksum errors can melt network
 * Add a knob to discard incoming LSPs with checksum errors, rather
 * than purging them.
 *
 * Add additional debugging capabilities.
 *
 * Revision 2.1  1995/06/07  20:18:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "config.h"
#include "../iprouting/route_map.h"

#ifndef __ISIS_EXTERNS_H__
#define __ISIS_EXTERNS_H__

/*
 * ---------- External Variable Definitions ----------
 */

/* Adjacency data structures. */
extern clns_adjtype **clns_adjacency_db;

/*
 * Level related character strings.
 */
extern const char isis_L1_str[];
extern const char isis_L1L2_str[];
extern const char isis_L2_str[];
extern const char isis_L2only_str[];

/*
 * ---------- External Function Definitions ----------
 */

/* 
 * isis.c
 */
extern clns_pdbtype *isis_pdb_init (char *, isis_proc_type);
extern void isis_fork_processes(clns_pdbtype *, isis_proc_type);
extern void isis_purge_lsp (clns_pdbtype *, isis_lspdbtype *, int, ulong);
extern isis_info_type *isis_insert_auth (isis_info_type *, clns_pdbtype *,
					 isisidbtype *, int, int);
extern boolean isis_check_auth (isis_info_type *, char *, clns_pdbtype *, 
				isisidbtype *, int);
extern void isis_reset_circuit_id (clns_pdbtype *, idbtype *);
extern void isis_reset_idb_params (clns_pdbtype *, idbtype *, boolean);
extern void isis_start_interface (clns_pdbtype *, idbtype *);
extern void isis_add_static_nbrs (clns_pdbtype *, idbtype *);
extern void isis_checksum_lsp (clns_pdbtype *, isis_lspdbtype *);
extern ushort isis_compute_checksum (uchar *, int, uchar *, int, int);
extern void isis_ager(clns_pdbtype *);
extern void isis_free_resources(clns_pdbtype *);
extern void isis_clear_database(clns_pdbtype *, isis_tree_type *);
extern void isis_summary_address_command(parseinfo *);
extern void isis_send_iih(clns_pdbtype *pdb, idbtype *idb, int level);
extern void isis_rtr_commands(parseinfo *);
extern void isis_intf_commands(parseinfo *);
extern void isis_prc_ivl_command(parseinfo *, clns_pdbtype *);
extern void isis_spf_ivl_command(parseinfo *, clns_pdbtype *);
extern void isis_ignore_attached_bit_command(parseinfo *, clns_pdbtype *);
extern void isis_log_adj_changes_command(parseinfo *, clns_pdbtype *);
extern void isis_lspmtu_command(boolean, boolean, boolean, char *, int,
				 clns_pdbtype *);
extern void isis_upq_depth_command(parseinfo *, clns_pdbtype *);
extern void isis_lsp_gen_command(boolean, boolean, boolean, char *, int, int,
				 clns_pdbtype *);

extern isisidbtype *isis_alloc_isisidb(idbtype *, isis_proc_type, ulong);
extern void isis_purge_dr(int, clns_pdbtype *, isisidbtype *);
extern void isis_schedule_hello(clns_pdbtype *, isisidbtype *, int);
extern void isis_lsp_lifetime_command(boolean, boolean, boolean, char *, int,
				      clns_pdbtype *);
extern void isis_lsp_refresh_command(boolean, boolean, boolean, char*, int,
				     clns_pdbtype *);
extern ulong isis_lsp_refresh_interval(clns_pdbtype *);
extern boolean isis_send_packet(clns_pdbtype *, paktype *, ulong, idbtype *,
				hwaddrtype *);
extern void lspid_copy(const isis_lspid *, isis_lspid *);
extern char *lspid_name_string(clns_pdbtype *, isis_lspid *);
extern char *lspid_string(isis_lspid *);
extern void isis_sysid_display_command(parseinfo *, clns_pdbtype *);
extern boolean isis_we_are_dr(clns_pdbtype *, idbtype *, ulong);
extern void isis_mark_lsp_purged(isis_lspdbtype *, int);
extern boolean isis_rw_purge_lsp(isis_tree_type *,
				 void *, void *, void *, void *);
extern boolean isis_verify_database_checksum(clns_pdbtype *, isis_lspdbtype *);
extern void isis_restart (clns_pdbtype *);
extern void isis_new_es_adjacency(clns_pdbtype *pdb, clns_adjtype *adj);
extern boolean isis_rw_display_lsp (isis_tree_type *, void *, void *, void *,
				    void *);
extern void isis_cleanup_pdb(clns_pdbtype *);
extern void isis_trigger_dr_election(clns_pdbtype *, isisidbtype *, ulong);
extern ulong isis_default_csnp_timer(idbtype *, isis_proc_type);
extern isisidbtype *get_isis_idb(clns_pdbtype *, idbtype *);
extern uchar *isis_adj_addr(clns_pdbtype *, idbtype *, uchar *, uchar *,
			    uchar *);
extern void isis_release_idb_number(clns_pdbtype *, isisidbtype *);

/*
 * isis2.c
 */
extern boolean isis_encaps (paktype *, int, idbtype *, hwaddrtype *);
extern void isis_union_areas (clns_pdbtype *, uchar *);
extern isis_TLV_parse *isis_valid_packet(paktype *, void *, clns_pdbtype *);
extern void isis_dump_packet (paktype *);

/*
 * isis_update.c
 */
extern process isis_update(void);
extern isis_lspdbtype *isis_find_L1_lsp (clns_pdbtype *, isis_lspid *);
extern void isis_build_L1_pdu (clns_pdbtype *);
extern boolean isis_construct_areaAddress(isis_lspcontext *);
extern isis_info_type *isis_get_next_item(clns_pdbtype *, isis_lspdbtype **, 
					  isis_info_type *, int);
extern isis_generic_ad *isis_walk_lsp (clns_pdbtype *, isis_lspdbtype **, 
				       isis_info_type **, isis_generic_ad *, 
				       int, int);
extern void isis_refresh_lsps(clns_pdbtype *);
extern void isis_add_one_es_ad (isis_info_type *, uchar *, ulong);
extern isis_lspdbtype *isis_find_L2_lsp (clns_pdbtype *, isis_lspid *);
extern void isis_build_L2_pdu (clns_pdbtype *);
extern void isis_flush_lsp_fragments (clns_pdbtype *, isis_lspid *, int);
extern boolean isis_area_in_pdb(clns_pdbtype *, clns_adjtype *);
extern void clns_route_map_inform(char *, int);
extern boolean isis_build_is_neighbor (isis_lspcontext *, int, uchar *,
				       uchar);
extern void isis_trigger_update_process(clns_pdbtype *);

/*
 * isis_update2.c
 */
extern void isis_link_lsp (clns_pdbtype *, isis_lspdbtype *, int, idbtype *,
			   isis_TLV_parse *);
extern void isis_delink_lsp (clns_pdbtype *, isis_lspid *, int);
extern void isis_build_L1_dr_pdu (clns_pdbtype *, idbtype *);
extern void isis_build_L2_dr_pdu (clns_pdbtype *, idbtype *);
extern void isis_rcv_lsp (clns_pdbtype *, paktype *, int);
extern void isis_rcv_csnp(clns_pdbtype *, paktype *, int);
extern void isis_rcv_psnp(clns_pdbtype *, paktype *, int);
extern boolean isis_valid_adjacency (clns_pdbtype *, paktype *, int);
extern isis_lspdbtype *isis_malloc_lspdb(clns_pdbtype *, ulong);
extern void isis_schedule_lsp_build(clns_pdbtype *, int, ulong, boolean);
extern void isis_mark_L1_changed_delayed(clns_pdbtype *);
extern void isis_mark_L2_changed_delayed(clns_pdbtype *);
extern void isis_mark_L1_changed(clns_pdbtype *);
extern void isis_mark_L2_changed(clns_pdbtype *);
extern void isis_mark_L1_dr_changed(clns_pdbtype *, isisidbtype *);
extern void isis_mark_L2_dr_changed(clns_pdbtype *, isisidbtype *);
extern void isis_trigger_spf(clns_pdbtype *, int, int, isis_lspid *, boolean);
extern void isis_trigger_partial_route_calculation(clns_pdbtype *);
extern void show_isis_spf_log(parseinfo *);
extern void show_isis_timers(parseinfo *);

/*
 * isis_flood.c
 */
extern boolean isis_rw_set_srm(isis_tree_type *,
			       void *, void *, void *, void *);
extern void isis_rexmit_packet(clns_pdbtype *, idbtype *);
extern void isis_increment_seq (clns_pdbtype *, ulong *);
extern void isis_psnp(clns_pdbtype *, idbtype *);
extern void isis_send_psnps(clns_pdbtype *, idbtype *, long, 
			    isis_tree_type *, hwaddrtype *);
extern void isis_set_all_srm(clns_pdbtype *, isis_lspdbtype *, isisidbtype *);
extern void isis_set_srm(clns_pdbtype *, idbtype *, isis_lspdbtype *);
extern void isis_really_start_lsp_transmission(clns_pdbtype *, idbtype *,
					       isisidbtype *);
extern void isis_lsp_send_timer_expiry(clns_pdbtype *, idbtype *,
				       isisidbtype *);
extern void isis_start_all_transmitters(clns_pdbtype *);
extern void isis_csnp_timer_expired(clns_pdbtype *, idbtype *,
				    isisidbtype *, int);
extern void isis_manipulate_srm_mask(clns_pdbtype *, isisidbtype *);
extern boolean isis_configure_mesh_group(clns_pdbtype *, isisidbtype *, ulong);
extern void isis_deconfigure_mesh_group(clns_pdbtype *, isisidbtype *);
extern void isis_display_mesh_groups(clns_pdbtype *);

/*
 * isis_spf.c
 */
extern void isis_compute_spt (clns_pdbtype *, int);
extern isis_ddbtype *isis_get_L1_route (clns_pdbtype *, uchar *);
extern clns_adjtype *isis_route (clns_pdbtype *, NSAP_T);
extern clns_adjtype *isis_make_route_cache (NSAP_T, isis_ddbtype *);
extern void isis_compute_some_routes(clns_pdbtype *);
extern void isis_delete_parents(isis_lspdbtype *);
extern void isis_age_one_lsp_chain(clns_pdbtype *, ushort);
extern void isis_empty_cleanup_queue(clns_pdbtype *);
extern void isis_flush_lsp_backup_chains(clns_pdbtype *, isis_lspix_type *);
extern void isis_delink_from_tent(clns_pdbtype *, isis_lspdbtype *);

/*
 * isis_adj.c
 */
extern void isis_rcv_serial_iih(paktype *, clns_pdbtype *, idbtype *,
				isishdrtype *);
extern void isis_rcv_lan_iih(paktype *, clns_pdbtype *, idbtype *, 
			     isishdrtype *, int);
extern void isis_update_adj_count (clns_pdbtype *, clns_adjtype *, boolean);
extern void isis_log_adjacency_change (clns_pdbtype *, clns_adjtype *,
				       boolean, const char *);


/* 
 * isis_avl.c
 */
typedef boolean (*isis_walker_type)(isis_tree_type *,
				    void *, void *, void *, void *);
/*
 * Recursive walk types, giving context to the "start_id" parameter.
 */
typedef enum isis_walk_type_ {
    ISIS_WALK_ALL,			/* Walk all LSPs */
    ISIS_WALK_MATCH_SYSTEM,		/* Process all LSPs matching sysID */
    ISIS_WALK_MATCHED_SYSTEM,		/* Internal, not for callers! */
    ISIS_WALK_FIND_STARTID,		/* Process all starting with LSPID */
    ISIS_WALK_SHOW_ALL			/* Display all LSPs */
    } isis_walk_type;
extern boolean isis_recursive_walk(isis_lspid *, isis_walk_type,
				   isis_tree_type *, isis_walker_type,
				   void *, void *, void *, void *);
extern void isis_next_avl(isis_lspid *, isis_tree_type *, isis_tree_type **);
extern int isis_match_lspid(isis_lspid *, isis_lspid *);
extern void isis_lsp_avl_delete(clns_pdbtype *, isis_lspid *, 
				isis_tree_type **, int *);
extern void isis_avl_insert(clns_pdbtype *, int *, isis_lspid *,
			    isis_lspdbtype *, isis_tree_type **);
typedef void (*avl_proc_type)(int *, isis_lspid *, isis_lspdbtype *,
			      isis_tree_type **, idbtype *, int,
			      clns_pdbtype *, isis_TLV_parse *);
extern isis_tree_type *isis_get_avl_element (isis_lspid *, isis_tree_type **, 
					     int *, avl_proc_type,
					     isis_lspdbtype *, idbtype *, 
					     int, clns_pdbtype *,
					     isis_TLV_parse *);
extern void isis_delink_one_lsp (clns_pdbtype *, isis_tree_type *);
extern void isis_free_avl_element (isis_tree_type *);

/*
 * isis_integ.c
 */
extern isis_info_type *isis_insert_nlpid (clns_pdbtype *, 
					  isis_info_type *, int);
extern void isis_insert_lsp_nlpid (isis_lspcontext *);
extern void isis_print_idr_info (isis_info_type *);
extern idbqueuetype *isis_checkqueue (queuetype *, idbtype *);

/*
 * isis_clns.c
 */
extern void isis_add_osi_route(clns_pdbtype *pdb, uchar *dest,
			       isis_lspdbtype *isis_paths,
			       isis_lsp_parent *next_hops,
			       int metric, int level);
extern void isis_add_prefix_neighbors(isis_lspcontext *context);
extern void isis_add_one_prefix_ad(isis_info_type *, uchar *, ulong);
extern void isis_age_L1_routing_table(clns_pdbtype *);
extern void isis_do_partition_avoidance(clns_pdbtype *);
extern void isis_track_interface_state(clns_pdbtype *, idbtype *);
extern void isis_update_interface_state(idbtype *);
extern void isis_adjacency_holddown(clns_pdbtype *);


/*
 * isis_lsp.c
 */
extern void isis_lsp_done(isis_lspcontext *);
extern isis_info_type *isis_new_item(isis_lspcontext *, int, int,
				     void **, int *, boolean);
extern void isis_item_done(isis_lspcontext *, void *);
extern isis_lspcontext *isis_create_lsp(clns_pdbtype *, int, int,
					idbtype *);
extern void isis_destroy_context(isis_lspcontext *);
extern void *isis_add_item_entry(isis_lspcontext *, int, boolean,
				 boolean *);
extern boolean isis_next_lsp (isis_lspcontext *, boolean);
extern boolean isis_create_specific_lsp (isis_lspcontext *, uchar);
extern void show_isis_database(parseinfo *);
extern ushort isis_alloc_lspix(clns_pdbtype *);
extern void isis_validate_lspix_array(clns_pdbtype *);
extern boolean isis_valid_lsp_version(clns_pdbtype *, ushort index,
				      ushort version);
extern void isis_free_lsp(clns_pdbtype *pdb, isis_lspdbtype *);
extern void isis_add_entry_to_lsp_chain(isis_lsp_linkage *,
					isis_lsp_linkage **);
extern void isis_delete_entry_from_lsp_chain(isis_lsp_linkage *);
extern void isis_delete_backup_entry(clns_pdbtype *, isis_backuptype *);
extern void isis_delete_backup_chain(clns_pdbtype *, isis_backuptype **);
extern void isis_add_backup_entry(isis_backuptype *, isis_backuptype **,
				  isis_backuptype **);
extern void isis_install_backup(clns_pdbtype *, ulong, ushort, ulong,
				isis_backuptype **);
extern void isis_attach_backup_chain(isis_backuptype**, isis_backuptype**);
extern void isis_mark_lsp_to_recalculate(clns_pdbtype *, ushort);
extern isis_cleanup_queue *isis_create_cleanup_queue_entry(clns_pdbtype *);
extern void isis_validate_ndb_backup_chain(isis_backuptype **);
extern boolean isis_display_lsp (isis_tree_type *, int, boolean, boolean,
				 clns_pdbtype *);

/*
 * isis_ipx.c
 */
extern clns_pdbtype *find_nlsp_pdb(char *);

#endif __ISIS_EXTERNS_H__
