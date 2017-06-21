/* $Id: sna_util.h,v 3.3.42.1 1996/05/17 11:21:34 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/ibm/sna_util.h,v $
 *------------------------------------------------------------------
 * sna_util.h
 *
 * 12/6/94, Anna O. Lam
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sna_util.h,v $
 * Revision 3.3.42.1  1996/05/17  11:21:34  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.30.2  1996/04/25  23:58:19  bchan
 * CSCdi55817:  gs7-s-mz broken
 * Branch: IbuMod_Calif_branch
 * move ConvertAsciiToEbcdic from dspu/snautil.c to ibm/sna_util.c
 *
 * Revision 3.3.30.1  1996/04/03  14:34:50  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3  1995/12/23  00:54:28  kpatel
 * CSCdi46079:  DLSw+ does not support non-activation XID
 *
 * Revision 3.2  1995/11/17  09:24:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:46:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/19  08:09:37  bchan
 * CSCdi27730:  SNA Fragmentation break if no buffers available
 *
 * Revision 2.2  1995/08/07  05:35:27  richl
 * CSCdi38315:  makefile rework
 *              move utility routines to libibm_util.a
 *
 * Revision 2.1  1995/06/07  20:48:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SNA_UTIL_H__
#define __SNA_UTIL_H__

#define TH_HEADER_SIZE    (6)
/*
 * SNA Transmission headers. We can only "break up" FID2 headers
 */
#define SNA_TH_FID_MASK (0xF0)
#define SNA_TH_FID2_VALUE (0x20)
/* Mapping field -- two bits Begin-BIU at 0x08 and End-BIU at 0x04 */
#define SNA_TH_MPF_MASK (0x0C)
#define SNA_TH_FIRST_BIU (0x08)
#define SNA_TH_MIDDLE_BIU (0x00)
#define SNA_TH_LAST_BIU (0x04)
#define SNA_TH_WHOLE_BIU (0x0C)
#define SNA_BIU(x) (((x) & SNA_TH_MPF_MASK))

#define SNA_IS_FIRST_BIU(x) (((x) & SNA_TH_MPF_MASK) == SNA_TH_FIRST_BIU)
#define SNA_IS_MIDDLE_BIU(x) (((x) & SNA_TH_MPF_MASK) == SNA_TH_MIDDLE_BIU)
#define SNA_IS_LAST_BIU(x) (((x) & SNA_TH_MPF_MASK) == SNA_TH_LAST_BIU)
#define SNA_IS_WHOLE_BIU(x) (((x) & SNA_TH_MPF_MASK) == SNA_TH_WHOLE_BIU)
#define SNA_SET_FIRST_BIU(x) x = (((x) & ~(SNA_TH_MPF_MASK)) | (SNA_TH_FIRST_BIU))
#define SNA_SET_MIDDLE_BIU(x) x = (((x) & ~(SNA_TH_MPF_MASK)) | (SNA_TH_MIDDLE_BIU))
#define SNA_SET_LAST_BIU(x) x = (((x) & ~(SNA_TH_MPF_MASK)) | (SNA_TH_LAST_BIU))
#define SNA_SET_WHOLE_BIU(x) x = (((x) & ~(SNA_TH_MPF_MASK)) | (SNA_TH_WHOLE_BIU))


/* NT 2.1 Support
 *
 * To marry an NRM protocol such as SDLC or QLLC to an ABM protocol
 * such as LLC2 it's necessary to spoof the XID3 ABM bit.
 */

#define		XID_SIZE	4	/* IDBLK, IDNUM */
#define         XID_XID3        0x30    /* XID format 3 */
#define         XID3_ABM        0x40    /* bit setting */
#define		XID3_ROLE	0x30	/* XID3 role mask */
#define		XID3_ABM_MASK	0xBF	/* XID3 ABM mask */
#define         XID3_NONACTIVATION 0x0C /* Non activation bits */

#define         XID0_LENGTH     6	/* Total length of XID format 0 */
#define         XID_LEN_OFFSET  1	/* Offset to length field */
#define		XID3_NONACTIVATION_OFFSET 9 /* Offset to non-activation exchange */
#define         XID3_ABM_OFFSET 19	/* Offset to ABM indicator */
#define         XID3_MAXBTU_OFFSET 21	/* Offset to maxbtu 15 bits */
#define         XID3_MAXIFRAME_OFFSET 27	/* Offset to maxiframe byte */

struct XID3type_ {
    uchar       xid3_fmt;        /* 0: 0x32 for XID-3    */
    uchar       xid3_len;        /* 1: length            */
    uchar       xid3_idblk_idnum[XID_SIZE];          /* 2-5  */
 
    /* xid 3 continuation */
 
    uchar       xid3_fill1[2];   /* 6,7:                 */
    uchar       xid3_properties[2]; /* 8,9: init_self, binds, xchg_state etc */
    uchar       xid3_bind_pace;  /* 10:                  */
    uchar       xid3_fill2[6];   /* 11-16                */
    uchar       xid3_dlc_type;   /* 17: always 1 'SDLC'  */
    uchar       xid3_dlc_length; /* 18: always 11        */
 
    /* SDLC  (and QLLC, LLC2) continuation */
 
    uchar       xid3_lsflags;    /* 19: ABM, role        */
    uchar       xid3_fill3;      /* 20:                  */
    uchar       xid3_maxbtu[2];  /* 21,22 max btu size the ls can receive */
    uchar       xid3_profile;    /* 23: command and response set always 0 */
    uchar       xid3_sim_rim;    /* 24: SIM and RIM support */
    uchar       xid3_fill4[2];   /* 25,26:               */
    uchar       xid3_maxiframe;  /* max number of i-frames in the window */
} ;

void ConvertAsciiToEbcdic(const uchar * ascii, uchar* ebcdic ,int length);

#endif __SNA_UTIL_H__

