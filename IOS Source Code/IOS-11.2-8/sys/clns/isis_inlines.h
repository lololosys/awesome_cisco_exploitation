/* $Id: isis_inlines.h,v 3.3.46.1 1996/09/12 00:17:16 hsmit Exp $
 * $Source: /release/112/cvs/Xsys/clns/isis_inlines.h,v $
 *------------------------------------------------------------------
 * Inline procedures for IS-IS
 *
 * May 1994, Dave Katz
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: isis_inlines.h,v $
 * Revision 3.3.46.1  1996/09/12  00:17:16  hsmit
 * CSCdi68705:  ISIS improvement in handling duplicate copies of the same
 * LSP
 * Branch: California_branch
 *
 * Revision 3.3  1995/12/11  04:45:26  sluong
 * CSCdi45316:  Add route-aggregation command for nlsp
 * change to not automatically generate aggregate routes.
 * up the maximum number of nlsp instances to 28.
 *
 * Revision 3.2  1995/11/17  08:53:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:09:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:52:25  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/08/25  14:10:31  hampton
 * Eliminate unnecessary includes of novell.h.  [CSCdi39328]
 *
 * Revision 2.1  1995/06/07 20:18:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _ISIS_INLINES_H_
#define _ISIS_INLINES_H_

#include "mgd_timers.h"
#include "../xns/novell.h"


/*
 * isis_system_is_dr - returns TRUE if a system is DR on this circuit
 */
static inline boolean isis_system_is_dr (clns_pdbtype *pdb, uchar *system,
					 isisidbtype *isisidb, ulong level)
{
    switch (level) {
      case ISIS_CIRCUIT_L1:
	return(station_match(isisidb->isis_L1_dr_id, system));
	
      case ISIS_CIRCUIT_L2:
	return(station_match(isisidb->isis_L2_dr_id, system));
      default:
	return(FALSE);
    }
}

/*
 * isis_pak_start
 *
 * Returns a pointer to the packet header, given a pak structure and
 * the PDB (so we know whether it's ISIS or NLSP)
 */
static inline void *isis_pak_start (paktype *pak, clns_pdbtype *pdb)
{
    if (pdb->proc_type == PROC_TYPE_NLSP)
	return(ipxdatastart(pak));
    else
	return(isisheadstart(pak));
}

/*
 * isis_pak_length
 *
 * Returns the length of a packet, given a pak structure and
 * the PDB (so we know whether it's ISIS or NLSP).
 *
 * Assumes that the length fields are valid (for ISIS they're set
 * by the drivers, and for NLSP they're validated by the IPX
 * receive code).
 */
static inline int isis_pak_length (paktype *pak, clns_pdbtype *pdb)
{
    ipxhdrtype *ipx;
    int length;

    if (pdb->proc_type == PROC_TYPE_NLSP) {
	ipx = (ipxhdrtype *) ipxheadstart(pak);
	length = GETSHORT(&ipx->len) - XNSOVERHEADBYTES;
    } else {
	length = pak->datagramsize - 
	    ((ulong) isisheadstart(pak) - (ulong) pak->datagramstart);
    }
    return(length);
}

/*
 * isis_lspdb_private_length
 *
 * Returns the length of the private portion of the LSP database (i.e.,
 * not including the core or TLV portion).
 */
static inline ulong isis_lspdb_private_length (void)
{
    isis_lspdbtype *dummy;
    return (((uchar *) dummy->data) - ((uchar *) dummy));
}

/*
 * isis_lspdb_data_length
 *
 * Returns the length of the data portion (core + TLV) of an LSP database
 * entry.
 */
static inline ulong isis_lspdb_data_length (isis_lspdbtype *lspdb)
{
    return (lspdb->db_length - isis_lspdb_private_length());
}

/*
 * isis_lspdb_tlv_length
 *
 * Returns the length of the TLV portion of an LSP database entry.
 */
static inline ulong isis_lspdb_tlv_length (isis_lspdbtype *lspdb)
{
    return (isis_lspdb_data_length(lspdb) - ISIS_LSP_CORE_SIZE);
}

/*
 * isis_lspid_is_pseudonode
 *
 * Returns TRUE if the LSPID is that of a pseudonode
 */
static inline boolean isis_lspid_is_pseudonode (isis_lspid *lspid)
{
    return(lspid->pseudo_id != 0);
}

/*
 * isis_lsp_index_in_range
 *
 * Returns TRUE if the index is valid (in range), or FALSE otherwise.
 */
static inline boolean isis_lsp_index_in_range (clns_pdbtype *pdb, ushort index)
{
    if (pdb == NULL)
	return(FALSE);
    return(index > 0 && index < pdb->lspix_count);
}

/*
 * isis_lsp_index_to_lspix_entry
 *
 * Returns the LSP index entry corresponding to this index, or NULL
 * if the index value is invalid.
 */
static inline isis_lspix_type
    *isis_lsp_index_to_lspix_entry (clns_pdbtype *pdb, ushort index)
{
    if (!isis_lsp_index_in_range(pdb, index))
	return(NULL);
    return(&pdb->lspix_table[index]);
}

/*
 * isis_lspix_to_lsp
 * 
 * Returns the LSPDB entry corresponding to an index, or NULL if not there
 * or index value is invalid.
 */
static inline isis_lspdbtype *isis_lspix_to_lsp (clns_pdbtype *pdb,
						 ushort index)
{
    isis_lspix_type *entry;
    entry = isis_lsp_index_to_lspix_entry(pdb, index);
    if (entry == NULL)
	return(NULL);
    return(entry->lsp);
}

/*
 * isis_start_lsp_transmission
 *
 * Kick the LSP transmitter on this interface if it isn't already running.
 *
 * Assumes that the IDB pointers are valid.
 */
static inline void isis_start_lsp_transmission (clns_pdbtype *pdb,
						idbtype *idb,
						isisidbtype *isisidb)
{
    if (isisidb->isis_running && !isisidb->isis_lsp_send_requested) {
	isisidb->isis_lsp_send_requested = TRUE; /* Flag it. */
	if (mgd_timer_running(&isisidb->isis_lsp_pacing_timer))
	    return;
	isis_really_start_lsp_transmission(pdb, idb, isisidb);
    }
}


/*
 * isis_dummy_lsp
 *
 * Returns TRUE if the LSP is a dummy.
 */
static inline boolean isis_dummy_lsp (isis_lspdbtype *lsp_db)
{
    return(GETLONG(&lsp_db->data->sequence_number) == 0);
}


/*
 * isis_fastpsnp_hash
 *
 * Xor the two bytes of an LSP checksum, to use this as an index in the
 * FastPSNP cache.
 */
static inline ulong isis_fastpsnp_hash (uchar *checksum)
{
    return(checksum[0] ^ checksum[1]);
}


/*
 * isis_is_L2_attached
 *
 * Check to see if we should set the ATTached bit in L1 LSPs.
 * Based on whether we know other areas dynamically (L2) or
 * statically (via redistributed static/iso-igrp routes).
 */
static inline boolean isis_is_L2_attached (clns_pdbtype *pdb)
{
    return(pdb->num_L2_routes || pdb->redistributed_L2_routes);
}


/*
 * isis_pdbindex_to_procindex
 *
 * Converts a PDB index into an offset into tables of ISIS/NLSP processes.
 */
static inline ulong isis_pdbindex_to_procindex (ulong pdbindex)
{
    return(pdbindex - CLNS_PDBINDEX_ISISNLSP_MIN);
}


/*
 * isis_pdbindex_valid
 *
 * Returns TRUE if the pdbindex is in the valid range for ISIS/NLSP.
 */
static inline boolean isis_pdbindex_valid (ulong index)
{
    return (index >= CLNS_PDBINDEX_ISISNLSP_MIN &&
	    index <= CLNS_PDBINDEX_ISISNLSP_MAX);
}

#endif
