/* $Id: trsrb.h,v 3.2 1995/11/17 18:57:41 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/srt/trsrb.h,v $
 *------------------------------------------------------------------
 * Token ring specific SRB support
 *
 * August 1995, Bob O'Grady
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: trsrb.h,v $
 * Revision 3.2  1995/11/17  18:57:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:32:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:30:49  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/01  16:14:00  ogrady
 * Placeholder for FDDI/SRB development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TRSRB_H__
#define __TRSRB_H__


/*
 * return true if token ring hdr is srb
 */
static inline boolean
is_tr_srb (tring_hdr *trh)
{
    return(trh->saddr[0] & TR_RII);
}


/*
 * return true if token ring pak is srb
 */
static inline boolean
is_tr_srb_pak (paktype *pak)
{
    return(is_tr_srb((tring_hdr *)pak->datagramstart));
}


/*
 * return riflen from token ring srb hdr
 * for use in fast paths when pak is known to be srb
 * Do NOT add RII checks to this inline
 * Do NOT delete this inline
 */
static inline int
tr_riflen (trrif_t *trh)
{
    return(trh->rc_blth & RC_LTHMASK);
}


/*
 * return riflen for token ring srb pak
 * for use in fast paths when pak is known to be srb
 * Do NOT add RII checks to this inline
 * Do NOT delete this inline
 */
static inline int
tr_riflen_pak (paktype *pak)
{
    return(tr_riflen((trrif_t *)(pak->datagramstart)));
}


/*
 * return riflen if any or 0 for token ring header
 */
static inline int
get_tr_riflen (tring_hdr *trh)
{
    return(is_tr_srb(trh) ? tr_riflen((trrif_t *)trh) : 0);
}


/*
 * return riflen if any or 0 for token ring pak
 */
static inline int
get_tr_riflen_pak (paktype *pak)
{
    return(get_tr_riflen((tring_hdr *)pak->datagramstart));
}

/*
 * return true if srb explorer 
 *  ASSUMES rif is present
 */
static inline boolean
is_tr_explorer (trrif_t *trh)
{
	return((trh->rc_blth & (RC_BROADBIT|RC_SPANBIT|RC_TRACE_ORIG)));
}

static inline boolean
is_tr_explorer_pak (paktype *pak)
{
    return(is_tr_explorer((trrif_t *)pak->datagramstart));
}

#endif /* __TRSRB_H__ */
