/* $Id: des_ecb.h,v 1.1.14.1 1996/04/19 14:56:48 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/crypto_lib/des_ecb.h,v $
 *------------------------------------------------------------------
 * des_ecb.h  -- Cylink DES C constants and type defintions. 
 *
 * Feb, 1996, xliu 
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: des_ecb.h,v $
 * Revision 1.1.14.1  1996/04/19  14:56:48  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.1  1996/02/28  08:40:08  xliu
 * Branch: IosSec_branch
 * Check in Cylink DES C implementation on MIP.
 *
 * Revision 1.1  1996/02/28  08:35:17  xliu
 * Placeholder.
 *
 * Revision 1.7  1995/06/07 12:03:03  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* file des_ecb.h  */


typedef long long  *des_key_schedule;

#define DES_ENCRYPT	1
#define DES_DECRYPT	0

