/* $Id: des0.h,v 3.2 1995/11/17 17:46:22 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/csflibs/des/des0.h,v $
 *------------------------------------------------------------------
 * des0.h - DES definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: des0.h,v $
 * Revision 3.2  1995/11/17  17:46:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSF_LIB_DES_H_
#define _CSF_LIB_DES_H_ "%W% %G%"
/*.original des0.h 3.2 5/11/94*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1989,1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 *
 * Export of this software from the United States of America is assumed
 * to require a specific license from the United States Government.
 * It is the responsibility of any person or organization contemplating
 * export to obtain such a license before exporting.
 */

#include <csf/header.h>

/*
#if (CSF_CFG0_M_WORDSIZE != 32) && (CSF_CFG0_M_WORDSIZE != 16)
  # error "WORDSIZE not 32 or 16 -- Don't know how to do this machine type."
#endif
*/

/*
 * number of iterations of the inner oop of the DES algorithm.  The
 * standard is 16, but in case that is too slow, we might do less.  Of
 * course, less also means less security.
 */
#define AUTH_DES_ITER	16 /*TBD/JNK*/


/* Control flag passed to routines (yuck).
 */
#define CSF_CRS_DES_ENCRYPT		TRUE		/* Encrypt the data. */
#define CSF_CRS_DES_DECRYPT		FALSE		/* Decrypt the data. */

#endif /* _CSF_LIB_DES_H_ */
