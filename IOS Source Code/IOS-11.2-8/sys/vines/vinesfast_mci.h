/* $Id: vinesfast_mci.h,v 3.2.62.1 1996/05/17 12:15:52 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/vines/vinesfast_mci.h,v $
 *------------------------------------------------------------------
 * vinesfast_mci.h -- VINES Fast Switching module providing MCI/CBUS
 *                    specific Support.
 *
 * July 1992, David R. Hampton
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vinesfast_mci.h,v $
 * Revision 3.2.62.1  1996/05/17  12:15:52  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.1  1996/05/02  22:08:27  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2  1995/11/17  17:59:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:45:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:15:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/**********************************************************************
 *
 * Small inline functions make the code more readable.  These are used
 * for writing to MCI style interface card
 *
 **********************************************************************/

/*
 * vines_write_ether_header
 *
 * Create a ARPA Ethernet header on a packet in MCI/CBUS memory.
 */
inline static void vines_write_ether_header (
    mciregtype *outreg,
    vinesfastpathtype *path)
{
    outreg->write1long	= path->vinesp_mh.mac_longs[0];
    outreg->write1long	= path->vinesp_mh.mac_longs[1];
    outreg->write1long	= path->vinesp_mh.mac_longs[2];
    outreg->write1short = TYPE_VINES;
}

/*
 * vines_write_ether_snap_header
 *
 * Create a Ethernet SNAP header on a packet in MCI/CBUS memory.
 */
inline static void vines_write_ether_snap_header (
    mciregtype *outreg,
    vinesfastpathtype *path,
    long temp_length)
{
    outreg->write1long	= path->vinesp_mh.mac_longs[0];
    outreg->write1long	= path->vinesp_mh.mac_longs[1];
    outreg->write1long	= path->vinesp_mh.mac_longs[2];
    outreg->write1short = temp_length;
    outreg->write1long	= (SNAPSNAP << 16) | (LLC1_UI << 8);
    outreg->write1long	= TYPE_VINES2;
}

/*
 * vines_write_fddi_header
 *
 * Create a FDDI SNAP header on a packet in CBUS memory.
 */
inline static void vines_write_fddi_header (
    mciregtype *outreg,
    vinesfastpathtype *path)
{
    short i;
    ushort *p;

    outreg->write1short = FDDI_LLC_FC_BYTE;
    outreg->write1long	= path->vinesp_mh.mac_longs[0];
    outreg->write1long	= path->vinesp_mh.mac_longs[1];
    outreg->write1long	= path->vinesp_mh.mac_longs[2];
    p = &path->vinesp_mh.mac_shorts[6];
    for (i = path->rifwordlength; i > 0; i--)
	outreg->write1short = *p++;
    outreg->write1long	= (SNAPSNAP << 16) | (LLC1_UI << 8);
    outreg->write1long	= TYPE_VINES2;
}

/*
 * vines_write_token_header
 *
 * Create a Token Ring SNAP header on a packet in CBUS memory.
 */
inline static void vines_write_token_snap_header (
    mciregtype *outreg,
    vinesfastpathtype *path)
{
    short i;
    ushort *p;

    outreg->write1short = (AC_PRI0 << 8) | FC_TYPE_LLC;
    outreg->write1long	= path->vinesp_mh.mac_longs[0];
    outreg->write1long	= path->vinesp_mh.mac_longs[1];
    outreg->write1long	= path->vinesp_mh.mac_longs[2];
    p = &path->vinesp_mh.mac_shorts[6];
    for (i = path->rifwordlength; i > 0; i--)
	outreg->write1short = *p++;
    outreg->write1long	= (SNAPSNAP << 16) | (LLC1_UI << 8);
    outreg->write1long	= TYPE_VINES2;
}

/*
 * vines_write_token_header
 *
 * Create a Banyan Token Ring header (SAP + demux byte) on a packet in
 * CBUS memory.
 */
inline static void vines_write_token_header (
    mciregtype *outreg,
    vinesfastpathtype *path)
{
    short i;
    ushort *p;

    outreg->write1short = (AC_PRI0 << 8) | FC_TYPE_LLC;
    outreg->write1long	= path->vinesp_mh.mac_longs[0];
    outreg->write1long	= path->vinesp_mh.mac_longs[1];
    outreg->write1long	= path->vinesp_mh.mac_longs[2];
    p = &path->vinesp_mh.mac_shorts[6];
    for (i = path->rifwordlength; i > 0; i--)
	outreg->write1short = *p++;
    outreg->write1long	= (SAP_VINES << 24) | (SAP_VINES << 16) |
	(LLC1_UI << 8) | VINES_DEMUX_IP;
}

/*
 * vines_write_hdlc_header
 *
 * Create a HDLC header on a packet in MCI/SCI/CBUS memory.  This routine
 * makes special allowance for a MIP header.
 */
inline static void vines_write_hdlc_header (
    mciregtype *outreg,
    vinesfastpathtype *path)
{
    if (path->length == HDLC_ENCAPBYTES) {
	outreg->write1long = HDLC_VINESCODE;
    } else {
	outreg->write1short = path->vinesp_mh.mac_shorts[0];
	outreg->write1long = HDLC_VINESCODE;
    }
}

/*
 * vines_write_ppp_header
 *
 * Create a PPP header on a packet in MCI/SCI/CBUS memory.  This routine
 * makes special allowance for a MIP header.
 */
inline static void vines_write_ppp_header (
    mciregtype *outreg,
    vinesfastpathtype *path)
{
    if (path->length == HDLC_ENCAPBYTES) {
	outreg->write1long = ((PPP_STATION << 16) | TYPE_PPP_VINES);
    } else {
	outreg->write1short = path->vinesp_mh.mac_shorts[0];
	outreg->write1long = ((PPP_STATION << 16) | TYPE_PPP_VINES);
    }
}

/*
 * vines_write_frame_header
 *
 * Create a frame relay header on a packet in MCI/SCI/CBUS memory.  This
 * routine makes special allowance for a MIP header.
 */
static inline void vines_write_frame_header (
    mciregtype *outreg,
    vinesfastpathtype *path)
{
    if (path->length == FRAME_RELAY_ENCAPBYTES) {
	outreg->write1long = path->vinesp_mh.mac_longs[0];
    } else {
	outreg->write1long  = path->vinesp_mh.mac_longs[0];
	outreg->write1short = path->vinesp_mh.mac_shorts[2];
    }
}

/*
 * vines_write_frame_header
 *
 * Create an IETF variant of a frame relay header on a packet in
 * MCI/SCI/CBUS memory.  This routine makes special allowance for a MIP
 * header.
 */
static inline void vines_write_frame_ietf_header (
    mciregtype *outreg,
    vinesfastpathtype *path)
{
    if (path->length == FRAME_RELAY_SNAPENCAPBYTES) {
	outreg->write1long  = path->vinesp_mh.mac_longs[0];
	outreg->write1long  = path->vinesp_mh.mac_longs[1];
	outreg->write1short = path->vinesp_mh.mac_shorts[4];
    } else {
	outreg->write1long  = path->vinesp_mh.mac_longs[0];
	outreg->write1long  = path->vinesp_mh.mac_longs[1];
	outreg->write1long  = path->vinesp_mh.mac_longs[2];
    }
}

/*
 * vines_write_smds_header
 *
 * Note that the comments aren't in the order in which the fields appear
 * ih the header.  The order isn't important (since they are only
 * comments), its the amount of data copied that is crucial.  This
 * routine makes special allowance for a MIP header.
 */
static inline void vines_write_smds_header (
    mciregtype *outreg,
    vinesfastpathtype *path)
{
    ulong *uvar = path->vinesp_mh.mac_longs;

    outreg->write1long = *uvar++;	/* rsvd, betag, basize */
    outreg->write1long = *uvar++;	/* Destination Address (part 1) */
    outreg->write1long = *uvar++;	/* Destination Address (part 2) */
    outreg->write1long = *uvar++;	/* Source Address (part 1) */
    outreg->write1long = *uvar++;	/* Source Address (part 2) */
    outreg->write1long = *uvar++;	/* xhlpi, xhel, x */
    outreg->write1long = *uvar++;	/* hel[0] - hel[3] */
    outreg->write1long = *uvar++;	/* hel[4] - hel[7] */
    outreg->write1long = *uvar++;	/* hel[8] - hel[11] */
    outreg->write1long = *uvar++;	/* Dsap, Ssap, Ctl, Org[0] */
    outreg->write1long = *uvar++;	/* Org[1], Org[2], Ethertype */
    if (path->length > (SMDS_CISCOBYTES + SMDS_SNAP_ENCAPBYTES)) {
        outreg->write1long = *uvar++;		/* MIP and Cisco Headers */
    } else {
        outreg->write1short = *(ushort *)uvar;	/* Cisco Header */
    }
}

/*
 * vines_write_smds_dxi_header
 *
 * Note that the comments aren't in the order in which the fields appear
 * ih the header.  The order isn't important (since they are only
 * comments), its the amount of data copied that is crucial.  This
 * routine makes special allowance for a MIP header.
 */
static inline void vines_write_smds_dxi_header (
    mciregtype *outreg,
    vinesfastpathtype *path)
{
    ulong *uvar = path->vinesp_mh.mac_longs;

    outreg->write1long = *uvar++;	/* DXI Headers */
    outreg->write1long = *uvar++;	/* rsvd, betag, basize */
    outreg->write1long = *uvar++;	/* Destination Address (part 1) */
    outreg->write1long = *uvar++;	/* Destination Address (part 2) */
    outreg->write1long = *uvar++;	/* Source Address (part 1) */
    outreg->write1long = *uvar++;	/* Source Address (part 2) */
    outreg->write1long = *uvar++;	/* xhlpi, xhel, x */
    outreg->write1long = *uvar++;	/* hel[0] - hel[3] */
    outreg->write1long = *uvar++;	/* hel[4] - hel[7] */
    outreg->write1long = *uvar++;	/* hel[8] - hel[11] */
    outreg->write1long = *uvar++;	/* Dsap, Ssap, Ctl, Org[0] */
    outreg->write1long = *uvar++;	/* Org[1], Org[2], Ethertype */
    if (path->length > (SMDS_DXIBYTES + SMDS_SNAP_ENCAPBYTES)) {
        outreg->write1short = *(ushort *)uvar;	/* MIP Header */
    }
}

/*
 * vines_write_smds_dxi_header
 *
 * Write SMDS' four byte header extension (aka the trailer).
 */
static inline void vines_write_smds_trailer (
    mciregtype *outreg,
    ulong *src)
{
    outreg->write1long = *src;
}
/*
 * vines_write_atm_dxi_header
 *
 * Puts an atm-dxi header on a packet in
 * MCI/SCI/CBUS memory.  This routine makes special allowance for a MIP
 * header.
 */
static inline void vines_write_atm_dxi_header (
    mciregtype *outreg,
    vinesfastpathtype *path)
{
    ulong len, *maclong;

    maclong = path->vinesp_mh.mac_longs;
    for (len = path->length; len >= 4; len -= 4)
	outreg->write1long  = *maclong++;

    if (len) 
	outreg->write1short = *(ushort *)maclong;
}

/*
 * Writes ATM Mac header.
 */
inline static void vines_write_atm_header (
    mciregtype *outreg,
    vinesfastpathtype *path)
{
    ushort i;

    for(i = 0; i < btow(path->length); i++) {
	outreg->write1short = path->vinesp_mh.mac_shorts[i];
    }
}

/***********************************************************************
 *
 *			Externs and Prototypes
 *			   vinesfast_mci.c
 *
 ***********************************************************************/

/*
 * VINES variables
 */
extern boolean (*vfs_samemci[V_FS_MAX])(hwidbtype *, vinesfastpathtype *);
extern boolean (*vfs_mci2mci[V_FS_MAX])(hwidbtype *, vinesfastpathtype *);

/*
 * VINES Function Declarations
 *     vines/vinesfast_mci.c
 */

/* vinesfast_mci.c */
extern boolean vines_access_check_mci(hwidbtype *, ulong);
extern boolean ether_vines_fastswitch(hwidbtype *);
extern boolean fddi_vines_fastswitch(hwidbtype *);
extern boolean token_snap_vines_fastswitch(hwidbtype *);
extern boolean token_vines_fastswitch(hwidbtype *);
extern boolean hdlc_vines_fastswitch(hwidbtype *);
extern boolean atm_vines_fastswitch(hwidbtype *);
