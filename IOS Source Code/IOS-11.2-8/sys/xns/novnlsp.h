/* $Id: novnlsp.h,v 3.3.6.3 1996/08/08 05:19:25 mschaef Exp $
 * $Source: /release/112/cvs/Xsys/xns/novnlsp.h,v $
 *------------------------------------------------------------------
 * novnlsp.h -- Novell IPX NLSP support
 *
 * June 1994, Mark Schaefer
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: novnlsp.h,v $
 * Revision 3.3.6.3  1996/08/08  05:19:25  mschaef
 * CSCdi31506:  NLSP Ager process remains when NLSP is disabled
 * Branch: California_branch
 *
 * Also:
 *
 *   - Convert to use new scheduler primitives
 *   - Remove cruft including unused variables/params
 *
 * Revision 3.3.6.2  1996/06/03  20:23:58  john
 * CSCdi58748:  ipxwan and nlsp command nvgen order wrong and subsys issues
 * Branch: California_branch
 *
 * Revision 3.3.6.1  1996/03/18  22:54:00  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/16  08:00:52  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2.26.2  1996/03/07  11:22:35  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  22:01:14  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/03/05  23:12:38  sluong
 * CSCdi50715:  NLSP route aggregation conformance (phase 2)
 *
 * Revision 3.2  1995/11/17  19:24:26  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:59:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:39:47  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/09/23  21:43:20  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Generalize NLSP's interface to the ISIS LSP structure so code and
 * structures can be shared with ISIS-IP.
 *
 * Revision 2.2  1995/08/23  18:27:35  dkatz
 * CSCdi37901:  ISIS first SPF is unpredictable
 * Disentangle SPF triggering and holddown calculations.  Insert a short
 * delay before doing the first SPF run.  Add logging of SPF runs for
 * diagnosability.
 *
 * Revision 2.1  1995/06/07  23:29:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __NOVNLSP_H__
#define __NOVNLSP_H__

/*
 * Macros:
 */

#define NOVNLSP_DEBUG(net, pdb) \
(novellnlsp_debug ? (pdb->debug_set ? (pdb->debug_net == net) : TRUE) : FALSE)

static inline ulong udiff_inline (ulong v1, ulong v2)
{
    return((ulong)(v2 - v1));
}

#define UDIFF(v1, v2) (udiff_inline((v2) - (v1)))
#define SPF_LATER(spf1, spf2) (UDIFF((spf1), (spf2)) < 0)
#define SPF_EQUAL(spf1, spf2) ((spf1) == (spf2))
#define SPF_EARLIER(spf1, spf2) (UDIFF((spf1), (spf2)) > 0)
#define ZERO_PREFIX(net, mask) (net == 0 && mask == 0)


#include <string.h>
#include <ciscolib.h>

/*
 * Functions:
 */
extern void novnlsp_ager_create(void);
extern void novnlsp_ager_kill(void);
extern void novnlsp_validate(void);
extern void novnlsp_newage_routes(novpdbtype *pdb,
				  isis_lsp_linkage **lh,
				  ushort lsp_version,
				  ushort lsp_index);

extern void novnlsp_newage_services(novpdbtype *pdb,
				    isis_lsp_linkage **lh,
				    ushort lsp_version,
				    ushort lsp_index);

extern void novnlsp_ager_schedule(void);
extern void novnlsp_delete_rdb_from_lsp_list(novrdbtype *rdb);
extern void novnlsp_delete_srdb_from_lsp_list(novell_sapentry_rdb *srdb);
extern void novnlsp_dr_callback(boolean suspend_ok, novpdbtype *);
extern void novnlsp_process_redistribute(novpdbtype *pdb, novpdbtype *dist,
					 boolean sense);
extern void novnlsp_free_rdb_list(novndbtype *ndb,
				  enum IPX_ROUTE_FREE tablestatus);
extern void novnlsp_insert_srdb_in_lsp_list(novell_sapentry_rdb *srdb,
					    isis_lsp_linkage **lh);
extern void novnlsp_command(parseinfo *csb);

extern int novnlsp_cost_compare(nov_cost_type *cost1,
				nov_cost_type *cost2);

extern void show_nlsp_database(parseinfo *csb);
extern void show_nlsp_spf_log(parseinfo *csb);
extern void show_nlsp_neigh(parseinfo *csb);

extern int novnlsp_cost_compare (nov_cost_type *cost1,
				 nov_cost_type *cost2);

extern void novnlsp_svc_update(novpdbtype *pdb,
			       idbtype *idb,
			       ushort svc_type,
			       uchar *svc_name,
			       int svc_name_len,
			       ulong svc_net,
			       uchar *svc_host,
			       ushort svc_sock,
			       nov_cost_param_type *cost,
			       isis_lsp_linkage **lh,
			       ushort lsp_version,
			       ushort lsp_index);

extern novrdbtype *novnlsp_update(novpdbtype *pdb,
				  ulong dstnet,
				  ulong netmask,
				  ulong snet,
				  uchar *shost,
				  nidbtype *nidb,
				  nov_cost_param_type *cost,
				  ulong flags,
				  isis_lsp_linkage **lh,
				  ushort lsp_version,
				  ushort lsp_index);

extern void nlsp_add_all_ifs(novpdbtype *pdb);
extern void novnlsp_process_packet(paktype *);
extern void novnlsp_change_interface(novpdbtype *pdb, nidbtype *nidb,
				     boolean sense, boolean active);
extern int age_route_patricia(rntype *, va_list);
extern void ipx_nlsp_parser_init(void);
extern ulong ipx_nlsp_get_peer_network(paktype *);

static inline novpdbtype *novnlsp_find_pdb (char *tag)
{
    novpdbtype *pdb;

    if (!novell_nlsp_enabled) {
	return(NULL);
    }
    for (pdb = (novpdbtype *) novpdbQ.qhead; pdb; pdb = pdb->next) {
	if ((pdb->proctype == NET_NLSP) && 
	    !strcasecmp(pdb->pname, strlen(tag) ? tag : NULL))
	    return(pdb);
    }
    return(NULL);
}

static inline uchar ipx_bitsinmask(ulong mask)
{
    ulong masklen = 0;

    for (;mask; mask = mask << 1)
	masklen++;

    return masklen;
}

#endif __NOVNLSP_H__
