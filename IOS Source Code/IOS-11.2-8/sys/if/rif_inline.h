/* $Id: rif_inline.h,v 3.2.60.5 1996/08/26 15:09:14 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/if/rif_inline.h,v $
 *------------------------------------------------------------------
 * rif_inline.h - inlines for rif table functions
 *
 * May 1995, Richard Livingston
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rif_inline.h,v $
 * Revision 3.2.60.5  1996/08/26  15:09:14  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.2.60.4  1996/07/23  18:45:58  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.2.60.3  1996/07/09  05:58:56  ppearce
 * CSCdi59527:  inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.2.60.2  1996/05/17  11:23:33  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/26  15:20:34  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.48.1  1996/04/03  14:38:48  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.60.1  1996/03/18  20:14:38  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  22:19:01  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  09:30:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:52:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:03:24  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/07  05:11:07  richl
 * CSCdi38315:  makefile rework
 *              add new files for libraries
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "rif.h"
#include "../srt/srb_multiring.h"
#include "../srt/srt_registry.h"

static inline ulong determine_rif (idbtype *swidb, ulong bit)
{
    ulong   rif_flags;

    if (srb_get_multiring_bits(swidb) & bit) {
	rif_flags = RIF_PRESENT;
	if (srb_get_allroutes_bits(swidb) & bit)
	    rif_flags |= RIF_USE_ALLROUTES;
    } else {
	rif_flags = RIF_NOT_PRESENT;
    }
    return(rif_flags);
}

static inline int getRifLenPak (void *ptr, uchar rii_mask)
{
    srbrif_t *shdr;
    shdr = ptr;
    return((shdr->saddr[0] & rii_mask) ? shdr->rc_blth & RC_LTHMASK : 0);
}

static inline riftype *rif_lookup (uchar *address,
				   hwidbtype *interface,
				   uchar *sa,
				   int flags)
{
    return(rif_lookup_callback(address, interface, sa, flags, LINK_ILLEGAL,
			       NULL, 0));
}

static inline void rif_update (hwidbtype *idb, ushort vrn, uchar *addr,
			       uchar *sa, srbroute_t *route, int flags)
{
    rif_update_llc(idb, vrn, addr, sa, route, NULL, flags);
}

static inline
riftype *get_rif_inline (hwidbtype *hwidb, uchar *address)
{
    idbtype *swidb = firstsw_or_null(hwidb);
    uchar addr[IEEEBYTES];

    if (srb_multiring_none_enabled(swidb)) {
	return(NULL);
    }

    if (hwidb->type == IDBTYPE_FDDIT) {
	ieee_swap(address, addr);
	return(rif_lookup(addr, hwidb, NULL, 0));
    } else {
	return(rif_lookup(address, hwidb, NULL, 0));
    }
}


