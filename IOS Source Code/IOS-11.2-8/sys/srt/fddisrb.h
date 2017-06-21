/* $Id: fddisrb.h,v 3.2 1995/11/17 18:54:11 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/srt/fddisrb.h,v $
 *------------------------------------------------------------------
 * FDDI specific SRB support
 *
 * August 1995, Bob O'Grady
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fddisrb.h,v $
 * Revision 3.2  1995/11/17  18:54:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:31:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:30:09  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/01  16:14:01  ogrady
 * Placeholder for FDDI/SRB development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __FDDISRB_H__
#define __FDDISRB_H__


/*
 * return true if hdr is fddi srb
 * FDDI and FDDIT use different rii_masks
 */
static inline boolean
is_fddi_srb (fddi_hdr *fddi, uchar rii_mask)
{
    return(fddi->saddr[0] & rii_mask);
}


/*
 * return true if pak is fddi srb
 */
static inline boolean
is_fddi_srb_pak (paktype *pak, uchar rii_mask)
{
    return(is_fddi_srb((fddi_hdr *)pak->datagramstart, rii_mask));
}


/*
 * return riflen from fddi srb hdr
 * for use in fast paths when pak is known to be srb
 * Do NOT add RII checks to this inline
 * Do NOT delete this inline
 */
static inline int
fddi_riflen (fddirif_t *fddi)
{
    return(fddi->rc_blth & RC_LTHMASK);
}


/*
 * return riflen for fddi srb pak
 * for use in fast paths when pak is known to be srb
 * Do NOT add RII checks to this inline
 * Do NOT delete this inline
 */
static inline int
fddi_riflen_pak (paktype *pak)
{
    return(fddi_riflen((fddirif_t *)pak->datagramstart));
}


/*
 * return riflen if any or 0 for fddi header
 */
static inline int
get_fddi_riflen (fddi_hdr *fddi, uchar rii_mask)
{
    return(is_fddi_srb(fddi, rii_mask) ? fddi_riflen((fddirif_t *)fddi) : 0);
}


/*
 * return riflen if any or 0 for fddi pak
 */
static inline int
get_fddi_riflen_pak (paktype *pak, uchar rii_mask)
{
    return(get_fddi_riflen((fddi_hdr *)pak->datagramstart, rii_mask));
}


/*
 * return true if srb explorer 
 *  ASSUMES rif is present
 */
static inline boolean
is_fddi_explorer (fddirif_t *fddi)
{
	return((fddi->rc_blth & (RC_BROADBIT|RC_SPANBIT|RC_TRACE_ORIG)));
}

static inline boolean
is_fddi_explorer_pak (paktype *pak)
{
    return(is_fddi_explorer((fddirif_t *)pak->datagramstart));
}

#endif /* __FDDISRB_H__ */
