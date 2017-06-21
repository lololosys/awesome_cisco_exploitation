/* $Id: fastsrb_les.h,v 3.3.62.1 1996/03/18 20:40:35 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/les/fastsrb_les.h,v $
 *------------------------------------------------------------------
 * fastsrb_les.h - if spec for fastsrb_les.c
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: fastsrb_les.h,v $
 * Revision 3.3.62.1  1996/03/18  20:40:35  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.1  1996/01/24  22:22:23  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  17:37:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:57:05  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:30:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:31:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __FASTSRB_LES_H__
#define __FASTSRB_LES_H__

/*
 * Common Macros for fast SRB and fast DLSw.
 *
 * PACKET FORMAT FOR FST PACKETS
 *
 * Token Ring
 * ----------
 *      ACFC     acfc bits
 * DADA DADA    mac destination addr
 * DADA SASA    mac destination addr / mac source addr
 * SASA SASA    mac source addr
 *  RIF ....    rif information, variable length
 * AAAA LLC1    #define SNAPSNAP 0xAAAA / llc1
 * 0000 0800    zero's / #define TYPE_IP10MB 0x0800 (IP on 10MB net)
 *                      munged ip header (20 bytes worth)
 *  FST  FST    version:4 bits, ip header length:4 bits, type of service: 8 bits
 *                      / total length
 *  FST  FST    identifier / ipreserved: 1 bit, dontfragment:1 bit,
 *                      more fragments:1 bit, frame offset: 13 bits
 *                      (identifier is really sequence no for fst)
 *  FST  FST    time to live:1 uchar, protocol type: 1 uchar / checksum:ushort
 *  FST  FST    source ip address
 *  FST  FST    destination ip address
 *  RSRB or DLSW Header
 *
 * Serial
 * ------
 * DADA 0800    2 bytes dest addr / #define TYPE_IP10MB 0x0800 (IP on 10MB net)
 *                      munged ip header (20 bytes worth)
 *  FST  FST    version:4 bits, ip header length:4 bits, type of service: 8 bits
 *                      / total length
 *  FST  FST    identifier / ipreserved: 1 bit, dontfragment:1 bit,
 *                      more fragments:1 bit, frame offset: 13 bits
 *                      (identifier is really sequence no for fst)
 *  FST  FST    time to live:1 uchar, protocol type: 1 uchar / checksum:ushort
 *  FST  FST    source ip address
 *  FST  FST    destination ip address
 *  RSRB or DLSW Header
 *
 * Ethernet
 * --------
 * DADA DADA    mac destination addr
 * DADA SASA    mac destination addr / mac source addr
 * SASA SASA    mac source addr
 *                      munged ip header (20 bytes worth)
 * 0800  FST    #define TYPE_IP10MB 0x0800 (IP on 10MB net), version:4 bits,
 *                      ip header length:4 bits, type of service: 8 bits
 *  FST  FST    total length / identifier
 *                      (identifier is really sequence no for fst)
 *  FST  FST    ipreserved: 1 bit, dontfragment:1 bit, more fragments:1 bit,
 *                      frame offset: 13 bits / time to live:1 uchar,
 *                      protocol type: 1 uchar
 *  FST  FST    checksum / source ip address
 *  FST  FST    source ip address / destination ip address
 *  FST  VER    destination ip address / rsrb version
 *  RSRB or DLSW Header
 */

#define FST_WRITE_ETHER_HEADER(data, cptr, fst_hdr)     \
{                                                       \
    PUTLONG(((ulong *)data)++, cptr->m.maclong[0]);     \
    PUTLONG(((ulong *)data)++, cptr->m.maclong[1]);     \
    PUTLONG(((ulong *)data)++, cptr->m.maclong[2]);     \
    PUTSHORT(data++, TYPE_IP10MB);                      \
    PUTLONG(((ulong *)data)++, fst_hdr[0]);             \
    PUTLONG(((ulong *)data)++, fst_hdr[1]);             \
    PUTLONG(((ulong *)data)++, fst_hdr[2]);             \
    PUTLONG(((ulong *)data)++, fst_hdr[3]);             \
    PUTLONG(((ulong *)data)++, fst_hdr[4]);             \
}


static inline ushort *fst_write_fddi_header (ushort *data,
                                             ipcache_rntype *cptr,
                                             ulong *fst_hdr)
{
    ushort i;

    *((uchar *)data)++ = FDDI_ASYNCHRONOUS_FRAME     /* fc */
        | FDDI_48BIT_ADDRESS
            | FDDI_LLC_FRAME
                | FDDI_PRIORITY_ZERO;

    for (i = 0; i < cptr->length/2; i++) {
	PUTSHORT(((ushort *)data)++, cptr->m.macshort[i]);
    }

    PUTLONG(((ulong *)data)++, fst_hdr[0]);
    PUTLONG(((ulong *)data)++, fst_hdr[1]);
    PUTLONG(((ulong *)data)++, fst_hdr[2]);
    PUTLONG(((ulong *)data)++, fst_hdr[3]);
    PUTLONG(((ulong *)data)++, fst_hdr[4]);
    return(data);
}

#define FST_WRITE_SERIAL_HEADER(data, cptr, fst_hdr)    \
{                                                       \
    PUTLONG(((ulong *)data)++, ((cptr->m.macshort[0] << 16) | TYPE_IP10MB)); \
    PUTLONG(((ulong *)data)++, fst_hdr[0]);             \
    PUTLONG(((ulong *)data)++, fst_hdr[1]);             \
    PUTLONG(((ulong *)data)++, fst_hdr[2]);             \
    PUTLONG(((ulong *)data)++, fst_hdr[3]);             \
    PUTLONG(((ulong *)data)++, fst_hdr[4]);             \
}

#endif /* !__FASTSRB_LES_H__ */
