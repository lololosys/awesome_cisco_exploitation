/* $Id: vip_isl.h,v 3.1.14.2 1996/05/31 22:34:05 feisal Exp $
 * $Source: /release/112/cvs/Xsys/src-vip/vip_isl.h,v $
 *------------------------------------------------------------------
 * vip_isl.h - VIP software ISL/VLAN support
 *
 * Feisal Yusuf Daruwalla, Feb 1996
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vip_isl.h,v $
 * Revision 3.1.14.2  1996/05/31  22:34:05  feisal
 * CSCdi56695:  ISL Broken on certain card
 * Branch: California_branch
 *
 * Revision 3.1.14.1  1996/03/21  23:52:26  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.18.1  1996/03/18  23:36:45  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.1  1996/02/23  20:03:02  feisal
 * CSCdi45142:  Support VLAN ISL for new 1FE port adapter
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Definitions
 */
#define ISL_MIN_INNER_PKT		64
#define ISL_INNER_CRC_LEN		4
#define ISL_MIN_INNER_PKT_LESS_CRC 	60

#define ISL_ENCAPS			30
#define ISL_ENCAPS_FRONT		26
#define ISL_ENCAPS_FRONT_AND_INNER_MIN 	86
#define ISL_ENCAPS_DA			0x01000c00
#define ISL_ENCAPS_LEN_OFFSET		12
#define ISL_ENCAPS_DA_LEN		6
#define ISL_ENCAPS_SA_LEN		6
#define ISL_ENCAPS_LEN_LEN		2

/*
 * Define some control values for the vip_pak_is_isl() inline
 */
#define ISL_COMPLETE			1	/* complete frame */
#define ISL_PARTIAL			2	/* partial frame */

/*
 * vip_pak_is_isl: 
 * Look at a frame and decide if it is ISL
 */
static inline boolean vip_pak_is_isl (paktype *pak, 
				      particletype *mp,
				      int mode)
{
    uchar *ptr;

    if (mode == ISL_COMPLETE) {
	ptr = pak->datagramstart;
    } else {
	if (!pak_has_particles(pak)) {
	    ptr = mp->data_start;
	} else {
	    ptr = pak->datagramstart;
	}
    }

    if (*(ulong *)(ptr) != ISL_ENCAPS_DA) {
	return(FALSE);
    }

#ifdef NOTYET
    /*
     * Looks like this is not needed
     */
    if (!(*(ushort *)(ptr+4) & 0xfff0)) {
	return(FALSE);
    }
#endif

    return(TRUE);
}

/* end of file */

