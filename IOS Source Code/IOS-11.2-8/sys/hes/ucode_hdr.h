/* $Id: ucode_hdr.h,v 3.2 1995/11/17 09:21:35 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/ucode_hdr.h,v $
 *------------------------------------------------------------------ 
 *  ucode_hdr.h - downloadable microcode header definitions
 *
 *  William H. Palmer, Thu Oct  8 16:08:50 PDT 1992
 *
 *  Copyright (c) 1992-1994 by cisco Systems, Inc.  
 *  All rights reserved.  
 *------------------------------------------------------------------
 * $Log: ucode_hdr.h,v $
 * Revision 3.2  1995/11/17  09:21:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:43:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:46:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * microcode file header definition
 */
typedef struct {
    uchar skip;			/* should be DLU_HDR_SKIP */
    uchar hdr_ver;		/* DLU_HDR_VER */
    ushort fw_rev;		/* major:8, minor:8 */
    ushort req_hw_ver;		/* major:8, minor:8 */
    uchar hw_type;		/* controller type */
    uchar desc[25];		/* ascii description */
} dlu_hdr_t;			/* followed by microcode, then IP checksum */

#define DLU_NO_HDR		0x24
#define DLU_HDR_SKIP		0x85
#define DLU_HDR_VER		1

typedef struct {
    ushort crc;			/* IP crc over dlu_hdr, ucode, trailer */
} dlu_trlr_t;
