/* $Id: srb_error.h,v 3.1.2.5 1996/08/26 15:13:24 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/srt/srb_error.h,v $
 *------------------------------------------------------------------
 * SRB error message identifiers
 *
 * April 1995, Paul Pearce
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Common procedures for printing all SRB error messages
 *
 *------------------------------------------------------------------
 * $Log: srb_error.h,v $
 * Revision 3.1.2.5  1996/08/26  15:13:24  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.1.2.4  1996/08/12  18:33:31  ppearce
 * CSCdi61195:  rsrb performance degraded 40% from 11.1(1)
 * Branch: California_branch
 *   Code review comments -- code convention violation
 *
 * Revision 3.1.2.3  1996/08/11  23:34:19  ppearce
 * CSCdi61195:  RSRB performance degraded 40% from 11.1(1)
 * Branch: California_branch
 *   Erroneous RSRB debug processing -- missing check for debug enabled
 *
 * Revision 3.1.2.2  1996/07/09  06:06:13  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.1.2.1  1996/05/17  12:14:31  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.4.3  1996/04/30  21:44:18  ppearce
 * IBU modularity: build error gs7-j-mz
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.2  1996/04/30  00:59:46  ppearce
 * IBU modularity: correct *-p-* images build problem
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.1  1996/04/26  15:23:50  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1  1996/04/26  12:18:43  ppearce
 * Add placeholders for IbuMod_Calif_branch
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __SRB_ERROR_H_
#define __SRB_ERROR_H_


#include "srb_core.h"


/*
 * SrbDebug_FastForward()
 *
 */
static inline void
SrbDebug_FastForward (hwidbtype *ihwidb)
{
    srb_triplet_t srb_triplet;
    idbtype *iswidb = firstsw_or_null(ihwidb);
    srbV.srb_sb_get_triplet(iswidb, &srb_triplet);
    buginf("\nSRB%d: fast forward (srn %d bn %d trn %d)",
             ihwidb->unit, 
             srb_triplet.localRing,
             srb_triplet.bridgeNum,
             srb_triplet.remoteRing);
}


/*
 * SrbDebug_VrnFastForward()
 *
 */
static inline void
SrbDebug_VrnFastForward (hwidbtype *ihwidb, ushort bridgeNum, ushort ringNum)
{
    idbtype *iswidb = firstsw_or_null(ihwidb);
    srb_triplet_t srb_triplet;
    srbV.srb_sb_get_triplet(iswidb, &srb_triplet);
    buginf("\nSRB%d: fast forward (srn %d bn %d vrn %d bn %d trn %d)",
             ihwidb->unit, 
             srb_triplet.localRing,
             srb_triplet.bridgeNum,
             srb_triplet.remoteRing, 
             bridgeNum, 
             ringNum);
}


/*
 * SrbDebug_DroppedFastForward()
 *
 */
static inline void
SrbDebug_DroppedFastForward (hwidbtype *ihwidb)
{
    idbtype *iswidb = firstsw_or_null(ihwidb);
    srb_triplet_t srb_triplet;
    srbV.srb_sb_get_triplet(iswidb, &srb_triplet);
    buginf("\nSRB%d: fast forward dropped (srn %d bn %d trn %d)",
             ihwidb->unit, 
             srb_triplet.localRing,
             srb_triplet.bridgeNum, 
             srb_triplet.remoteRing);
} 


/*
 * RsrbDebug_ForwardToPeer()
 *
 */
static inline void 
RsrbDebug_ForwardToPeer (hwidbtype *ihwidb, tr_vpeertype *vrp)
{
    char peerid[MAX_PEER_NAME];
    srb_triplet_t srb_triplet;
    idbtype *iswidb = firstsw_or_null(ihwidb);
    srbV.srb_sb_get_triplet(iswidb, &srb_triplet);
    buginf("\nRSRB%d: forward srn %d bn %d vrn %d to peer%s",
             ihwidb->unit, 
             srb_triplet.localRing,
             srb_triplet.bridgeNum, 
             srb_triplet.remoteRing, 
             vrpeer_id(vrp, peerid));
}

#endif  /* __SRB_ERROR_H_ */
