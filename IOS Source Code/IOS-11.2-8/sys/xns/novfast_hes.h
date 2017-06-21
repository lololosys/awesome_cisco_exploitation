/* $Id: novfast_hes.h,v 3.4.10.2 1996/08/17 02:19:30 hou Exp $
 * $Source: /release/112/cvs/Xsys/xns/novfast_hes.h,v $
 *------------------------------------------------------------------
 * novfast_hes.h -- IPX Fast Switching module providing MCI/CBUS specific
 *                  Support to write headers by interface
 *
 * Bill May - January, 1995 (from novfast_mci.c and novfast_sbe.c)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: novfast_hes.h,v $
 * Revision 3.4.10.2  1996/08/17  02:19:30  hou
 * CSCdi65648:  7k send out erroneous isl vlan packets
 * Branch: California_branch
 * - add ISL vLAN code to handle IPX SAP input packet in fastswitching path.
 *
 * Revision 3.4.10.1  1996/03/18  22:53:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/16  07:59:58  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.26.2  1996/03/13  02:14:47  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.1  1996/02/20  22:00:50  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/25  03:53:25  mmcneali
 * CSCdi49686:  IPX non-functional in gs7 images on ISL encapsulating
 *              subinterfaces.
 *
 * Revision 3.3  1995/11/20  17:22:24  rbadri
 * CSCdi42852:  IPX fastswitching over ATM fails in setup with vlan switch
 * For LANE, choose the offset based on encaps. Write the SAP encapsulation
 * with correct values.
 *
 * Revision 3.2  1995/11/17  19:23:35  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:59:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:39:21  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/07/18  04:00:54  mschaef
 * CSCdi37175:  IPX fastswitching (high end ethernet) is broken
 *
 * Revision 2.2  1995/07/07  04:30:33  bbenson
 * CSCdi35019:  Fast/SSE switching of LANE IPX SAP encaps not working.
 * - Split out writing of lane encaps from nov_write_hes_aip_header.
 * - Allow same enctype on subinterfaces of non-broadcast media (AIP).
 * - Set uninitialized pak->encsize in novell_et_novell_ether.
 *
 * Revision 2.1  1995/06/07  23:29:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __NOVFAST_HES_H__
#define __NOVFAST_HES_H__


#include "../vlan/vlan.h"
#include "../tbridge/tbridge_sde.h"


/*****************************************************************************
 *                      ETHERNET ROUTINES                                    *
 *****************************************************************************/

static inline
ushort nov_write_hes_ether_header (int enctype,
				   ushort total_length,
				   mciregtype *outreg,
				   novpathtype *path,
				   ushort ipxlen,
				   ushort csum,
				   ulong len_hops)
{
    ulong lw;

    if (total_length < MINETHERSIZE) {
	total_length = MINETHERSIZE;
    } else {
	total_length = ROUND(total_length);
    }

    if (enctype == ET_SAP) {

	total_length += path->extra_bytes;
	total_length |= MCI_TX_ODDALIGN;
	outreg->write1long = path->novp_mh_oa.mac_longs[0];
	outreg->write1long = path->novp_mh_oa.mac_longs[1];
	outreg->write1long = path->novp_mh_oa.mac_longs[2];
	lw = path->novp_mh_oa.mac_longs[3];
	lw = ((lw & 0xff0000ff) | (ROUND(ipxlen + 3) << 8));
	outreg->write1long = lw;
	outreg->write1long = (0xe0030000 | csum);

    } else if (enctype == ET_ISL) {

	/*
	 * Explicitly lay down the 26 byte ISL header, then the inner MAC.
	 *
	 * Nota Bene: assumes "raw-ether" for the
	 *            interior encapsulation for now ...
	 */

	ulong *cache_word = path->novp_mh.mac_longs;

        outreg->write1long = *cache_word++;
        outreg->write1long = *cache_word++;
        outreg->write1long = *cache_word++;
        outreg->write1long = *cache_word++;
        outreg->write1long = *cache_word++;
        outreg->write1long = *cache_word++;
        outreg->write1long = *cache_word++;
        outreg->write1long = *cache_word++;
        outreg->write1long = *cache_word++;

        lw = path->novp_mh.mac_longs[9];
        lw = ((lw & 0xffff0000) | (ROUND(ipxlen)));
        outreg->write1long = lw;
	outreg->write1short = (NOVELL_NO_CHECKSUM);

	/*
         * Nota Bene: The microcode determines on a packet by packet basis
         * whether or not to generate a software CRC (ISL requires a CRC for
         * the interior packet, then an appended exterior CRC covering the
         * entire packet - ugly!). So the mechanism to indicate this is to
         * set bit 13 in the "len" field passed down to the SP; note the
         * actual length on Fast Ethernet can be amply encoded in the first
         * 10 bits!
         */

	total_length |= ISL_DO_CRC_MASK;

        path->novp_idb_ptr->isl_vlan->
	egressing_isl_packets[VLAN_ROUTE_NOVELL]++;

    } else if (enctype == ET_SDE) {

	/*
	 * Lay down a dot10 vLAN header. 38 bytes including SNAP.
	 */

        outreg->write1long = path->novp_mh.mac_longs[0];
        outreg->write1long = path->novp_mh.mac_longs[1];
        outreg->write1long = path->novp_mh.mac_longs[2];
        outreg->write1long = path->novp_mh.mac_longs[3];
        outreg->write1long = path->novp_mh.mac_longs[4];
        outreg->write1long = path->novp_mh.mac_longs[5];
        outreg->write1long = path->novp_mh.mac_longs[6];
        outreg->write1long = path->novp_mh.mac_longs[7];
        outreg->write1long = path->novp_mh.mac_longs[8];
        outreg->write1short = path->novp_mh.mac_shorts[9];

        path->novp_idb_ptr->sde_said_db->
	egressing_dot10_packets[VLAN_ROUTE_NOVELL]++;

    } else {
	outreg->write1long = path->novp_mh.mac_longs[0];
	outreg->write1long = path->novp_mh.mac_longs[1];
	outreg->write1long = path->novp_mh.mac_longs[2];
	if (enctype == ET_NOVELL_ETHER) {
	    outreg->write1long = ((ROUND(ipxlen) << 16) | 0xffff);
	} else if (enctype == ET_ARPA) {
	    outreg->write1long = ((TYPE_NOVELL1 << 16) | csum);
	} else { /* ET_SNAP */
	    outreg->write1long = ((ROUND(ipxlen + 8) << 16) | 0xaaaa);
	    outreg->write1long =  0x03000000;
	    outreg->write1long = ((TYPE_NOVELL1 << 16) | csum);
	}
    }
    outreg->write1long = len_hops;
    return(total_length);
}


static inline
ushort nov_write_hes_ether_oa_header (int enctype,
				      ushort total_length,
				      mciregtype *outreg,
				      novpathtype *path,
				      ushort ipxlen,
				      ushort csum,
				      ulong len_hops)
{
    ulong lw;

    if (total_length < MINETHERSIZE) {
	total_length = MINETHERSIZE;
    } else {
	/* Round up total output bytes if was ODD and > 60 bytes */
	total_length = ROUND(total_length);
    }
    if (enctype == ET_SAP) {
	outreg->write1long = path->novp_mh.mac_longs[0];
	outreg->write1long = path->novp_mh.mac_longs[1];
	outreg->write1long = path->novp_mh.mac_longs[2];
	outreg->write1short = ROUND(ipxlen + 3);
	outreg->write1long = (0xe0e00300 | (uchar)csum);
    } else if (enctype == ET_ISL) {
      /*
       * write down 1 byte alignment, 26 bytes 
       * ISL header and 9 bytes mac
       */
        outreg->write1long = path->novp_mh_oa.mac_longs[0];
	outreg->write1long = path->novp_mh_oa.mac_longs[1];
	outreg->write1long = path->novp_mh_oa.mac_longs[2];
	outreg->write1long = path->novp_mh_oa.mac_longs[3];
	outreg->write1long = path->novp_mh_oa.mac_longs[4];
	outreg->write1long = path->novp_mh_oa.mac_longs[5];
	outreg->write1long = path->novp_mh_oa.mac_longs[6];
	outreg->write1long = path->novp_mh_oa.mac_longs[7];
	outreg->write1long = path->novp_mh_oa.mac_longs[8];

	/* write down 3 bytes mac and 1 byte length field */
	lw = path->novp_mh_oa.mac_longs[9];
	outreg->write1long = ((lw & 0xffffff00) | (ROUND(ipxlen) >>
						   8));
	/* write down 1 byte length field and 1 byte checksum */
	outreg->write1short = ((ROUND(ipxlen) << 8) | 0xff);

	/* another byte checksum */
	len_hops |= 0xff000000;
	total_length += path->extra_bytes;
	total_length |= (MCI_TX_ODDALIGN | ISL_DO_CRC_MASK);
	path->novp_idb_ptr->isl_vlan->
	  egressing_isl_packets[VLAN_ROUTE_NOVELL]++;
	
    } else {
	outreg->write1long = path->novp_mh_oa.mac_longs[0];
	outreg->write1long = path->novp_mh_oa.mac_longs[1];
	outreg->write1long = path->novp_mh_oa.mac_longs[2];
	lw = path->novp_mh_oa.mac_longs[3];
	if (enctype == ET_ARPA) {
	    outreg->write1long = ((lw & 0xffffff00) | (uchar)csum);
	} else {
	    if (enctype == ET_NOVELL_ETHER) {
		lw = ((lw & 0xff0000ff) | (ROUND(ipxlen) << 8));
		outreg->write1long = (lw | 0xff);
		len_hops |= 0xff000000;
	    } else { /* ET_SNAP */
		lw = ((lw & 0xff0000ff) | (ROUND(ipxlen + 8) << 8));
		outreg->write1long = lw;
		outreg->write1long = 0xaa030000;
		outreg->write1long = ((TYPE_NOVELL1 << 8) | (uchar)csum);
	    }
	}
	total_length += path->extra_bytes;
	total_length |= MCI_TX_ODDALIGN;
    }
    outreg->write1long = len_hops;
    return(total_length);
}


/************************************************************************
 *                      FDDI ROUTINES					*
 ************************************************************************/
static inline
ushort nov_write_hes_fddi_header (int enctype,
				  mciregtype *outreg,
				  novpathtype *path,
				  ushort total_length,
				  ushort csum,
				  ulong len_hops)
{
    int ix;
    ushort *ptr;
    int rifwords;

    if (enctype == ET_SNAP) {
	total_length = (total_length + FDDI_SNAP_ENCAPBYTES +
			path->rifwords*2) | MCI_TX_ODDALIGN;
	outreg->write1short = FDDI_LLC_FC_BYTE;
	outreg->write1long = path->novp_mh.mac_longs[0];
	outreg->write1long = path->novp_mh.mac_longs[1];
	outreg->write1long = path->novp_mh.mac_longs[2];
	outreg->write1long = path->novp_mh.mac_longs[3];
	outreg->write1long = path->novp_mh.mac_longs[4];
	if ((rifwords = path->rifwords) != 0) {
	    ptr = (ushort *)&path->novp_mh.mac_longs[5];
	    for (ix = 0; ix < rifwords; ix++)
		outreg->write1short = *ptr++;
	}
    } else if (enctype == ET_NOVELL_FDDI) {
	total_length = (total_length + FDDI_ENCAPBYTES +
			path->rifwords*2) | MCI_TX_ODDALIGN;
	outreg->write1short = FDDI_LLC_FC_BYTE;
	outreg->write1long = path->novp_mh.mac_longs[0];
	outreg->write1long = path->novp_mh.mac_longs[1];
	outreg->write1long = path->novp_mh.mac_longs[2];
	if ((rifwords = path->rifwords) != 0) {
	    ptr = (ushort *)&path->novp_mh.mac_longs[3];
	    for (ix = 0; ix < rifwords; ix++)
		outreg->write1short = *ptr++;
	}	
    } else { /* ET_SAP */
	total_length = (total_length + FDDI_SAP_ENCAPBYTES +
			path->rifwords*2);
	/*path->novp_mh_oa.mac_bytes[0] = FDDI_LLC_FC_BYTE;*//* PRESET */
	outreg->write1long = path->novp_mh_oa.mac_longs[0];
	outreg->write1long = path->novp_mh_oa.mac_longs[1];
	outreg->write1long = path->novp_mh_oa.mac_longs[2];
	outreg->write1long = path->novp_mh_oa.mac_longs[3];
	if ((rifwords = path->rifwords) != 0) {
	    ptr = (ushort *)&path->novp_mh_oa.mac_longs[4];
	    for (ix = 0; ix < rifwords; ix++)
		outreg->write1short = *ptr++;
	}
    }
    outreg->write1short = csum;
    outreg->write1long = len_hops;
    return(total_length);
}


static inline
ushort nov_write_hes_fddi_oa_header (int enctype,
				     mciregtype *outreg,
				     novpathtype *path,
				     ushort total_length,
				     ushort csum,
				     ulong len_hops)
{
    int i;
    ushort *ptr;
    int rifwords;
    ulong sw;

    if (enctype == ET_SNAP) {
	total_length = (total_length + FDDI_SNAP_ENCAPBYTES +
			path->rifwords*2);
	/*path->novp_mh_oa.mac_bytes[0] = FDDI_LLC_FC_BYTE;*/
	outreg->write1long = path->novp_mh_oa.mac_longs[0];
	outreg->write1long = path->novp_mh_oa.mac_longs[1];
	outreg->write1long = path->novp_mh_oa.mac_longs[2];
	ptr = (ushort *)&path->novp_mh_oa.mac_shorts[6];
	if ((rifwords = path->rifwords) != 0) {
	    for (i = 0; i < rifwords; i++)
		outreg->write1short = *ptr++;
	}
	outreg->write1long  = *((ulong *)ptr)++;
	outreg->write1long = (TYPE_NOVELL1 >> 8);
	outreg->write1short = ((TYPE_NOVELL1 << 8) | (uchar)csum);
    } else if (enctype == ET_NOVELL_FDDI) {
	total_length = (total_length + FDDI_ENCAPBYTES +
			path->rifwords*2);
	/*path->novp_mh_oa.mac_bytes[0] = FDDI_LLC_FC_BYTE;*/
	outreg->write1long = path->novp_mh_oa.mac_longs[0];
	outreg->write1long = path->novp_mh_oa.mac_longs[1];
	outreg->write1long = path->novp_mh_oa.mac_longs[2];
	ptr = (ushort *)&path->novp_mh_oa.mac_shorts[6];
	if ((rifwords = path->rifwords) != 0) {
	    for (i = 0; i < rifwords; i++)
		outreg->write1short = *ptr++;
	}
	sw = *((ushort *)ptr)++;
	sw = ((sw & 0xff00) | 0x00ff);
	outreg->write1short = sw;
    } else { /* ET_SAP */
	total_length = (total_length + FDDI_SAP_ENCAPBYTES +
			path->rifwords*2) | MCI_TX_ODDALIGN;
	outreg->write1short = FDDI_LLC_FC_BYTE;
	outreg->write1long = path->novp_mh.mac_longs[0];
	outreg->write1long = path->novp_mh.mac_longs[1];
	outreg->write1long = path->novp_mh.mac_longs[2];
	ptr = (ushort *)&path->novp_mh.mac_shorts[6];
	if ((rifwords = path->rifwords) != 0) {
	    for (i = 0; i < rifwords; i++)
		outreg->write1short = *ptr++;
	}
	outreg->write1long = (0xe0e00300 | (uchar)csum);
    }
    outreg->write1long =  len_hops;
    return(total_length);
}

/************************************************************************
 *			TOKEN RING CACHE ROUTINES			*
 ************************************************************************/
static inline
ushort nov_write_hes_token_header (int enctype,
				   mciregtype *outreg,
				   novpathtype *path,
				   ushort bytes,
				   ushort csum,
				   ulong len_hops)
{
    int ix;
    ushort *ptr;
    int rifwords;

    if (path->enctype == ET_SAP) {
	outreg->write1short = AC_PRI0;
	/*path->novp_mh_oa.mac_bytes[0] = FC_TYPE_LLC;*/
	outreg->write1long  = path->novp_mh_oa.mac_longs[0];
	outreg->write1long  = path->novp_mh_oa.mac_longs[1];
	outreg->write1long  = path->novp_mh_oa.mac_longs[2];
	outreg->write1long  = path->novp_mh_oa.mac_longs[3];
	if ((rifwords = path->rifwords) != 0) {
	    ptr = (ushort *)&path->novp_mh_oa.mac_longs[4];
	    for (ix = 0; ix < rifwords; ix++)
		outreg->write1short = *ptr++;
	}
	bytes |= MCI_TX_ODDALIGN;
    } else { /* ET_SNAP */
	outreg->write1short = ((AC_PRI0 << 8) | FC_TYPE_LLC);
	outreg->write1long  = path->novp_mh.mac_longs[0];
	outreg->write1long  = path->novp_mh.mac_longs[1];
	outreg->write1long  = path->novp_mh.mac_longs[2];
	outreg->write1long  = path->novp_mh.mac_longs[3];
	outreg->write1long  = path->novp_mh.mac_longs[4];
	if ((rifwords = path->rifwords) != 0) {
	    ptr = (ushort *)&path->novp_mh.mac_longs[5];
	    for (ix = 0; ix < rifwords; ix++)
		outreg->write1short = *ptr++;
	}
    }
    outreg->write1short = csum;
    outreg->write1long = len_hops;
    return( bytes + (path->total_encap_bytes) );
}


static inline
ushort nov_write_hes_token_oa_header (int enctype,
				      mciregtype *outreg,
				      novpathtype *path,
				      ushort bytes,
				      ushort csum,
				      ulong len_hops)
{
    int i;
    ushort *ptr;
    int rifwords;

    if (enctype == ET_SAP) {
	outreg->write1short = (AC_PRI0 << 8) | FC_TYPE_LLC;
	outreg->write1long = path->novp_mh.mac_longs[0];
	outreg->write1long = path->novp_mh.mac_longs[1];
	outreg->write1long = path->novp_mh.mac_longs[2];
	ptr = (ushort *)&path->novp_mh.mac_shorts[6];
	if ((rifwords = path->rifwords) != 0) {
	    for (i = 0; i < rifwords; i++)
		outreg->write1short = *ptr++;
	}
	outreg->write1short = *ptr++;
    } else { /* ET_SNAP */
	outreg->write1short = AC_PRI0;
	/*path->novp_mh_oa.mac_bytes[0] = FC_TYPE_LLC;*/
	outreg->write1long  = path->novp_mh_oa.mac_longs[0];
	outreg->write1long  = path->novp_mh_oa.mac_longs[1];
	outreg->write1long  = path->novp_mh_oa.mac_longs[2];
	ptr = (ushort *)&path->novp_mh_oa.mac_shorts[6];
	if ((rifwords = path->rifwords) != 0) {
	    for (i = 0; i < rifwords; i++)
		outreg->write1short = *ptr++;
	}
	outreg->write1long  = *((ulong *)ptr)++;
	outreg->write1long  = *((ulong *)ptr)++;
	bytes |= MCI_TX_ODDALIGN;
    }
    outreg->write1short = ((*ptr & 0xff00) | (uchar)csum);
    outreg->write1long = len_hops;			
    return(bytes + path->total_encap_bytes);
}				     


/************************************************************************
 *			SERIAL CACHE ROUTINES				*
 ************************************************************************/
static inline
ushort nov_write_hes_serial_header (hwidbtype *output,
				    mciregtype *outreg,
				    novpathtype *path,
				    ushort total_length,
				    ushort csum,
				    ulong len_hops)
{
    int ix;
    ulong *maclong;

    maclong = path->novp_mh.mac_longs;

    for (ix = path->total_encap_bytes;
	 ix >= (int) sizeof(ulong);
	 ix -= sizeof(ulong))
	outreg->write1long = *maclong++;
    
    if (ix > 0)
	outreg->write1short = *(ushort *)maclong;
    outreg->write1short = csum;
    outreg->write1long = len_hops;
    total_length += path->total_encap_bytes;
    if (is_frame_relay(output)) {
	INC_PVC_FAST_OUTPUT(HWIDB_TO_FR_PVC(output,
					    path->subint_reference_num),
			    total_length);
    } else if (is_atm_dxi(output)) {
	INC_DXI_PVC_FAST_OUTPUT(HWIDB_TO_DXI_PVC(output,
						 path->subint_reference_num),
				total_length);
    }
    return(total_length);
}

static inline
ushort nov_write_hes_serial_oa_header (hwidbtype *output,
				       mciregtype *outreg,
				       novpathtype *path,
				       ushort total_length,
				       ushort csum,
				       ulong len_hops)
{
    int ix;
    ulong *maclong;
    ushort *macshort;

    maclong = path->novp_mh_oa.mac_longs;
    
    for (ix = path->total_encap_bytes;
	 ix >= (int) sizeof(ulong);
	 ix -= sizeof(ulong)) {
	outreg->write1long = *maclong++;
    }
    macshort = (ushort *)maclong;
    if (ix >= (int) sizeof(ushort)) {
	outreg->write1short = *macshort++;
    }
    outreg->write1short = ((*macshort & 0xff00) | (uchar)csum);
    outreg->write1long = len_hops;
    total_length += path->total_encap_bytes;
    if (is_frame_relay(output)) {
	INC_PVC_FAST_OUTPUT(HWIDB_TO_FR_PVC(output,
					    path->subint_reference_num),
			    total_length);
    } else if (is_atm_dxi(output)) {
	INC_DXI_PVC_FAST_OUTPUT(HWIDB_TO_DXI_PVC(output,
						 path->subint_reference_num),
				total_length);
    }
    return(total_length | MCI_TX_ODDALIGN);
}

/************************************************************************
 *			ATM CACHE ROUTINES				*
 ************************************************************************/
/*
 * Deal with ATM headers now
 */
static inline
ushort nov_write_hes_atm_header (mciregtype *inreg,
				 novpathtype *path,
				 ushort total_length,
				 ushort csum,
				 ulong len_hops)
{
    ushort *macsptr;
    ushort encsize;

    encsize = path->total_encap_bytes;
    total_length = (total_length + encsize);
    macsptr = path->novp_mh.mac_shorts;
    while(encsize > 0) {
	inreg->write1short = *macsptr++;
	encsize -= sizeof(ushort);
    }
    inreg->write1short = csum;
    inreg->write1long = len_hops;
    return(total_length);
}

static inline
ushort nov_write_hes_atm_oa_header (mciregtype *inreg,
				    novpathtype *path,
				    ushort total_length,
				    ushort csum,
				    ulong len_hops)
{
    ushort *macsptr;
    ushort encsize;

    encsize = path->total_encap_bytes;
    total_length = (total_length + encsize) | MCI_TX_ODDALIGN;
    macsptr = path->novp_mh_oa.mac_shorts;
    while(encsize > 0) {
	inreg->write1short = *macsptr++;
	encsize -= sizeof(ushort);
    }
    inreg->write1short = (*macsptr++ & 0xFF00) | (uchar)csum;
    inreg->write1long = len_hops;
    return(total_length);
}

static inline
ushort nov_write_hes_lane_header (int enctype,
				  ushort total_length,
				  mciregtype *outreg,
				  novpathtype *path,
				  ushort ipxlen,
				  ushort csum,
				  ulong len_hops)
{
    ulong lw;
    ushort encsize;
    ushort *macsptr;

    if ((total_length - (ATM_ENCAP_HDRBYTES + ATM_LANE_LECID_ENCAPBYTES)) < MINETHERSIZE) {
	total_length = MINETHERSIZE + ATM_ENCAP_HDRBYTES + ATM_LANE_LECID_ENCAPBYTES;
    }
    total_length = ROUND(total_length);
    encsize = btow(ATM_ENCAP_HDRBYTES + ATM_LANE_LECID_ENCAPBYTES + IEEEBYTES*2);
    if (enctype == ET_SAP) {
	total_length |= MCI_TX_ODDALIGN;
	macsptr = path->novp_mh_oa.mac_shorts;
	while(encsize-- > 0) {
	    outreg->write1short = *macsptr++;
	}
	lw = *macsptr++;
	lw = ((lw << 16) | *macsptr++);
        lw = ((lw & 0xff0000ff) | (ROUND(ipxlen + 3) << 8));
	outreg->write1long = lw;
	outreg->write1long = (0xe0030000 | csum);
    } else {
	macsptr = path->novp_mh.mac_shorts;
	while(encsize-- > 0) {
	    outreg->write1short = *macsptr++;
	}
	if (enctype == ET_NOVELL_ETHER) {
	    outreg->write1long = ((ROUND(ipxlen) << 16) | 0xffff);
	} else if (enctype == ET_ARPA) {
	    outreg->write1long = ((TYPE_NOVELL1 << 16) | csum);
	} else { /* ET_SNAP */
	    outreg->write1long = ((ROUND(ipxlen + 8) << 16) | 0xaaaa);
	    outreg->write1long =  0x03000000;
	    outreg->write1long = ((TYPE_NOVELL1 << 16) | csum);
	}
    }
    outreg->write1long = len_hops;
    return(total_length);
}

static inline
ushort nov_write_hes_lane_oa_header (int enctype,
				     ushort total_length,
				     mciregtype *outreg,
				     novpathtype *path,
				     ushort ipxlen,
				     ushort csum,
				     ulong len_hops)
{
    ulong lw;
    ushort encsize;
    ushort *macsptr;

    if ((total_length - (ATM_ENCAP_HDRBYTES + ATM_LANE_LECID_ENCAPBYTES)) < MINETHERSIZE) {
	total_length = MINETHERSIZE + ATM_ENCAP_HDRBYTES + ATM_LANE_LECID_ENCAPBYTES;
    } 
    total_length = ROUND(total_length);
    encsize = btow(ATM_ENCAP_HDRBYTES + ATM_LANE_LECID_ENCAPBYTES + IEEEBYTES*2);
    if (enctype == ET_SAP) {
	macsptr = path->novp_mh.mac_shorts;
	while(encsize-- > 0) {
	    outreg->write1short = *macsptr++;
	}
	outreg->write1short = ROUND(ipxlen + 3);
	outreg->write1long = (0xe0e00300 | (uchar)csum);
    } else {
	total_length |= MCI_TX_ODDALIGN;
	macsptr = path->novp_mh_oa.mac_shorts;
	while(encsize-- > 0) {
	    outreg->write1short = *macsptr++;
	}
	lw = *macsptr++;
	lw = ((lw << 16) | *macsptr++);
	if (enctype == ET_ARPA) {
	    outreg->write1long = ((lw & 0xffffff00) | (uchar)csum);
	} else if (enctype == ET_NOVELL_ETHER) {
	    lw = ((lw & 0xff0000ff) | (ROUND(ipxlen) << 8));
	    outreg->write1long = (lw | 0xff);
	    len_hops |= 0xff000000;
	} else { /* ET_SNAP */
	    lw = ((lw & 0xff0000ff) | (ROUND(ipxlen + 8) << 8));
	    outreg->write1long = lw;
	    outreg->write1long = 0xaa030000;
	    outreg->write1long = ((TYPE_NOVELL1 << 8) | (uchar)csum);
	}
    }
    outreg->write1long = len_hops;
    return(total_length);
}


/************************************************************************
 *			LEX CACHE ROUTINES				*
 ************************************************************************/
static inline
ushort nov_write_hes_lex_header (int enctype,
				 mciregtype *outreg,
				 novpathtype *path,
				 ushort ipxlen,
				 ushort csum,
				 ulong len_hops)
{
    uint len = (2 * IEEEBYTES);
    ushort *macsptr;
    ushort sw;
    ushort total_length;

    total_length = ROUND(ipxlen);
    total_length += path->total_encap_bytes;
    if (enctype == ET_SAP) {
	total_length += path->extra_bytes;
	total_length |= MCI_TX_ODDALIGN;
	len += path->length - ETHER_SAP_ENCAPBYTES;
	macsptr = path->novp_mh_oa.mac_shorts;
	while (len > 0) {
	    outreg->write1short = *macsptr++;
	    len -= sizeof(ushort);
	}
	sw = (*macsptr++ & 0xFF00);
	outreg->write1short = (sw | ((ROUND(ipxlen + 3) & 0xFF00) >> 8));
	outreg->write1short = (0xE0 | ((ROUND(ipxlen + 3) & 0x00FF) << 8));
	outreg->write1short = 0xE003;
	outreg->write1short = csum;
    } else {
	if (enctype == ET_NOVELL_ETHER) {
	    len += path->length - ETHER_ARPA_ENCAPBYTES;
	    macsptr = path->novp_mh.mac_shorts;
	    while (len > 0) {
		outreg->write1short = *macsptr++;
		len -= sizeof(ushort);
	    }
	    outreg->write1short = ROUND(ipxlen);
	    outreg->write1short = 0xffff;
	} else if (enctype == ET_ARPA) {
	    /*
	     * Write type from cache, thus + 2
	     */
	    len += path->length - ETHER_ARPA_ENCAPBYTES + 2;
	    macsptr = path->novp_mh.mac_shorts;
	    while (len > 0) {
		outreg->write1short = *macsptr++;
		len -= sizeof(ushort);
	    }
	    outreg->write1short = csum;
	} else { /* ET_SNAP */
	    len += path->length - ETHER_SNAP_ENCAPBYTES;
	    macsptr = path->novp_mh.mac_shorts;
	    while (len > 0) {
		outreg->write1short = *macsptr++;
		len -= sizeof(ushort);
	    }
	    outreg->write1short = ROUND(ipxlen + 8);
	    outreg->write1short = 0xAAAA;
	    outreg->write1short = 0x0300;
	    outreg->write1short = 0x0000;
	    outreg->write1short = TYPE_NOVELL1;
	    outreg->write1short = csum;
	}
    }
    outreg->write1short = ((len_hops & 0xFFFF0000) >> 16);
    outreg->write1short = (len_hops & 0x0000FFFF);
    return(total_length);
}

static inline
ushort nov_write_hes_lex_oa_header (int enctype,
				    mciregtype *outreg,
				    novpathtype *path,
				    ushort ipxlen,
				    ushort csum,
				    ulong len_hops)
{
    ushort total_length;
    uint len = (2 * IEEEBYTES);
    ushort *macsptr;
    ushort sw;

    total_length = ROUND(ipxlen);
    total_length += path->total_encap_bytes;
    if (enctype == ET_SAP) {
	len += path->length - ETHER_SAP_ENCAPBYTES;
	macsptr = path->novp_mh.mac_shorts;
	while (len > 0) {
	    outreg->write1short = *macsptr++;
	    len -= sizeof(ushort);
	}
	outreg->write1short = ROUND(ipxlen + 3);
	outreg->write1short = 0xE0E0;
	outreg->write1short = (0x0300 | (uchar)csum);
    } else {
	total_length += path->extra_bytes;
	total_length |= MCI_TX_ODDALIGN;
	if (enctype == ET_NOVELL_ETHER) {
	    len += path->length - ETHER_ARPA_ENCAPBYTES;
	    macsptr = path->novp_mh_oa.mac_shorts;
	    while (len > 0) {
		outreg->write1short = *macsptr++;
		len -= sizeof(ushort);
	    }
	    sw = (*macsptr++ & 0xFF00);
	    outreg->write1short = (sw | ((ROUND(ipxlen) & 0xFF00) >> 8));
	    outreg->write1short = (0xFF | ((ROUND(ipxlen) & 0x00FF) << 8));
	} else if (enctype == ET_ARPA) {
	    /*
	     * Write type from cache, thus + 2
	     */
	    len += path->length - ETHER_ARPA_ENCAPBYTES + 2;
	    macsptr = path->novp_mh_oa.mac_shorts;
	    while (len > 0) {
		outreg->write1short = *macsptr++;
		len -= sizeof(ushort);
	    }
	    sw = (*macsptr++ & 0xFF00);
	    outreg->write1short = (sw | (uchar)csum);
	} else { /* ET_SNAP */
	    len += path->length - ETHER_SNAP_ENCAPBYTES;
	    macsptr = path->novp_mh_oa.mac_shorts;
	    while (len > 0) {
		outreg->write1short = *macsptr++;
		len -= sizeof(ushort);
	    }
	    sw = (*macsptr++ & 0xFF00);
	    outreg->write1short = (sw | ((ROUND(ipxlen) & 0xFF00) >> 8));
	    outreg->write1short = (0xAA | ((ROUND(ipxlen) & 0x00FF) << 8));
	    outreg->write1short = 0xAA03;
	    outreg->write1short = 0x0000;
	    outreg->write1short = ((TYPE_NOVELL1 & 0xFF00) >> 8);
	    outreg->write1short = (((TYPE_NOVELL1 & 0x00FF) << 8) | (uchar)csum);
	}
    }
    outreg->write1short = ((len_hops & 0xFFFF0000) >> 16);
    outreg->write1short = (len_hops & 0x0000FFFF);
    return(total_length);
}

#endif __NOVFAST_HES_H__
