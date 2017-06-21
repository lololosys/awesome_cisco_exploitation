/* $Id: isis_ipx.h,v 3.4.6.1 1996/05/03 01:51:05 sluong Exp $
 * $Source: /release/112/cvs/Xsys/clns/isis_ipx.h,v $
 *------------------------------------------------------------------
 * Externals for IPX-specific IS-IS stuff
 *
 * May 1994, Dave Katz
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: isis_ipx.h,v $
 * Revision 3.4.6.1  1996/05/03  01:51:05  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.4  1996/03/01  07:31:50  mschaef
 * CSCdi50473:  NLSP route aggregation conformance (phase 1)
 *
 * Fix assorted nits and bugs
 *
 * Revision 3.3  1995/12/11  04:45:34  sluong
 * CSCdi45316:  Add route-aggregation command for nlsp
 * change to not automatically generate aggregate routes.
 * up the maximum number of nlsp instances to 28.
 *
 * Revision 3.2  1995/11/17  08:54:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:09:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:52:33  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/10/21  19:03:12  dkatz
 * CSCdi42558:  ISIS needs more debugging hooks
 * Provide a means to look at LSP database internal information.
 * Provide a means to flush the LSP database.
 *
 * Revision 2.2  1995/08/25  14:10:34  hampton
 * Eliminate unnecessary includes of novell.h.  [CSCdi39328]
 *
 * Revision 2.1  1995/06/07 20:18:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __ISIS_IPX_H__
#define __ISIS_IPX_H__

#include "../xns/novell.h"
#include "../xns/novroute.h"

/* Fragment allocation when lsp-data-hashing is in effect */

extern void isis_add_ipx_route (clns_pdbtype *pdb, ulong net_number,
				ulong netmask, isis_lspdbtype *lsp_zero, 
				isis_lspdbtype *current_lsp,
				isis_lsp_parent *next_hops,
				isis_lsp_parent *leaf_params,
				int metric,
				boolean external);
extern void isis_ipx_restart(novpdbtype *pdb);
extern void isis_ipx_cleanup(novpdbtype *pdb);
extern void isis_ipx_area_cmd(clns_pdbtype *, boolean, boolean, boolean,
			      char *, int, int);
extern void isis_insert_ipx_lsp_areas(isis_lspcontext *context);
extern isis_info_type *isis_insert_ipx_hello(clns_pdbtype *pdb, 
					     isis_info_type *item,
					     idbtype *idb, int bytes_left);
extern novpdbtype *isis_ipx_init(clns_pdbtype *clns_pdb, 
				 isis_proc_type proc_type);
extern novpdbtype *nlsp_pdb_init(char *, novpdbtype *);
extern void isis_insert_ipx_lsp(isis_lspcontext *context);
extern void isis_insert_ipx_lsp_hashed(isis_lspcontext *context);
extern void isis_ipx_redist_callback(novpdbtype *pdb,
				     novndbtype *ndb,
				     int event);
extern void isis_ipx_sap_redist_callback(novpdbtype *pdb,
					 novell_sapentrytype *sap,
					 int event);
extern boolean isis_ipx_xroute_callback(novpdbtype *pdb,
					novndbtype *ndb,
					ushort ticks,
					ushort hops);
extern boolean isis_ipx_xsap_callback(novpdbtype *pdb,
				      novell_sapentrytype *sap,
				      ushort hops);
extern boolean nlsp_ipx_sched_spf(clns_pdbtype *);
extern boolean nlsp_ipx_send_pkt(clns_pdbtype *pdb, paktype *pak,
				 int length, idbtype *swidb);
extern void ipx_nlsp_bw_delay_changed(nidbtype *nidb);
extern void ipx_nlsp_interface_init(nidbtype *, novpdbtype *, 
				    boolean, boolean);
extern void isis_add_one_nlsp_link_ad(isis_info_type *item, uchar *id,
				      idbtype *idb, ulong metric, 
				      uchar pseudo_id);
extern void isis_add_ipx_service(clns_pdbtype *pdb, isis_nlsp_svcs_info *svc,
				 isis_lspdbtype *lsp_zero, 
				 isis_lspdbtype *current_lsp,
				 isis_lsp_parent *next_hops,
				 isis_lsp_parent *leaf_params,
				 int name_length);
extern void isis_ipx_system_init(void);
extern void isis_ipx_rdb_changed_callback(novpdbtype *, novndbtype *,
					  novrdbtype *, enum IPX_ROUTE_EVENT,
					  ulong);
extern void isis_ipx_srdb_changed_callback(novpdbtype *, novell_sapentrytype *,
					  novell_sapentry_rdb *,
					  enum IPX_ROUTE_EVENT, ulong);
extern void ipx_nlsp_set_cost(nidbtype *, clns_pdbtype *);
extern void ipx_mark_dr_info_change(novpdbtype *, idbtype *);
extern void ipx_mark_non_dr_info_change(novpdbtype *);
extern boolean ipx_isis_pseudonode_needed(clns_pdbtype *, idbtype *);
extern void isis_clear_nlsp_neighbors(clns_pdbtype *);
extern void ipx_nlsp_clear_all(clns_pdbtype *);
extern void ipx_nlsp_set_default_redist(novpdbtype *pdb);
extern void isis_ipx_enable_summary_command(parseinfo *, clns_pdbtype *);
extern void isis_ipx_enable_lsp_summary_command(parseinfo *, clns_pdbtype *);
extern clns_pdbtype *find_nlsp_pdb_by_instance(int);
extern void isis_ipx_lsp_hash_enable_command(parseinfo *, clns_pdbtype *);

#endif __ISIS_IPX_H__

