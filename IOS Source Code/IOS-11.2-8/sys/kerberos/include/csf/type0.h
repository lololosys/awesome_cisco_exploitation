/* $Id: type0.h,v 3.2 1995/11/17 17:47:31 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csf/type0.h,v $
 *------------------------------------------------------------------
 * type0.h - Type definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: type0.h,v $
 * Revision 3.2  1995/11/17  17:47:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSF_TYPE0_H_
#define _CSF_TYPE0_H_ "%W% %G%"

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright 1989,1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/* LSBF & MSBF -- Little (lsb first) or big (msb first).
 */
#if !defined( CSF_CFG0_M_LSBF ) && !defined( CSF_CFG0_M_MSBF )
  # error -- CSF_CFG0_M_...  ONE of LSBF or MSBF must be defined.
#endif
#if defined( CSF_CFG0_M_LSBF ) && defined( CSF_CFG0_M_MSBF )
  # error -- CSF_CFG0_M_... LSBF and MSBF both defined -- pick ONE.
#endif

/* WORDSIZE -- Word size (in bits) of this machine.
 */
#ifndef CSF_CFG0_M_WORDSIZE
  # error -- CSF_CFG0_M_WORDSIZE not defined -- pick 16 or 32.
#endif

/* RALIGN -- Required alignment (in bytes) for this machine.
 */
#ifndef CSF_CFG0_M_RALIGN
  # error -- CSF_CFG0_M_RALIGN not defined -- define one.
#endif


/*
 * The rest of these definitions come from the above.
 * External definition of the following is optional.
 */

/* BALIGN -- Best alignment for this machine.
 */
#ifndef CSF_CFG_M_BALIGN
#define CSF_CFG_M_BALIGN (CSF_CFG0_WORDSIZE / sizeof(long))
#endif


/*
 * Basic types.
 */

#ifndef csf_i8
typedef char csf_i8;
#endif
#ifndef csf_ui8
typedef unsigned char csf_ui8;
#endif

#if CSF_CFG0_M_WORDSIZE == 16
#ifndef csf_i16
typedef int csf_i16;
#endif
#ifndef csf_ui16
typedef unsigned int csf_ui16;
#endif
#ifndef csf_i32
typedef long csf_i32;
#endif
#ifndef csf_ui32
typedef unsigned long csf_ui32;
#endif
#endif /* CSF_CFG0_M_WORDSIZE == 16 */

#if CSF_CFG0_M_WORDSIZE == 32
#ifndef csf_i16
typedef short csf_i16;
#endif
#ifndef csf_ui16
typedef unsigned short csf_ui16;
#endif
#ifndef csf_i32
typedef int csf_i32;
#endif
#ifndef csf_ui32
typedef unsigned int csf_ui32;
#endif
#endif /* CSF_CFG0_M_WORDSIZE == 32 */

/* csf_o - "octet" */
#ifndef csf_o
typedef unsigned char csf_o;
#endif

/* csf_sPad -- Pad required to align structures. */
#ifndef csf_sPad
typedef long csf_sPad;
#endif


/* These types are host-dependent; we assume the ANSI definitions.
 */

#ifndef FALSE
#define FALSE   0
#endif
#ifndef TRUE
#define TRUE    1
#endif

#ifndef csf_size_t
typedef size_t csf_size_t;
#endif

#ifndef csf_time_t
typedef time_t csf_time_t;
#endif

#ifndef csf_tm
typedef struct tm csf_tm;
#endif


/*
 * These types are derived from the basic types above
 * (or are independent of machine-specific types).
 */

#ifndef csf_sts
typedef csf_ui32 csf_sts;
#endif

#ifndef csf_boolean
typedef int csf_boolean;
#endif

#ifndef csf_flags
typedef csf_ui32 csf_flags;
#endif

#ifndef csf_mTypes
typedef union _csf_mTypes {
	csf_o o;
	csf_ui8 ui8; csf_i8 i8;
	csf_ui16 ui16; csf_i16 i16;
	csf_ui32 ui32; csf_i32 i32;
	csf_ui8 ua16[sizeof(csf_ui16)];
	csf_ui8 ua32[sizeof(csf_ui32)];
	void *p;
	csf_size_t sizet;
	csf_sPad spad;
} csf_mTypes;
#endif

#endif /* _CSF_TYPE0_H_ */
