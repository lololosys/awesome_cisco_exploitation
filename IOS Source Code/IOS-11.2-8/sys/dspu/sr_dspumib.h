/* $Id: sr_dspumib.h,v 3.2 1995/11/17 09:06:33 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/sr_dspumib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * December 1994, Paul Pearce (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_dspumib.h,v $
 * Revision 3.2  1995/11/17  09:06:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:30:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:56:45  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:31:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 * Copyright (C) 1994 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 52.227-7013
 *  and in similar clauses in the FAR and NASA FAR Supplement.
 *
 */

#ifndef _dspumib_h_
#define _dspumib_h_
 
#define N_dspuPuTrapsPrefix
#define N_dspuLuTrapsPrefix

#include "../snmp/snmp_api.h"
#include "sr_dspumibdefs.h"
#include "sr_dspumibsupp.h"
#undef SETS
#include "sr_dspumibpart.h"
#include "sr_dspumibtype.h"

extern const ObjectInfo dspumib_OidList[];
extern const uint       dspumib_OidListNum;

extern void init_dspumib(void);

#define D_dspumibDlcType_undefined   1
#define D_dspumibDlcType_sdlc        2
#define D_dspumibDlcType_ethernet    5
#define D_dspumibDlcType_tokenRing   6
#define D_dspumibDlcType_rsrb        8
#define D_dspumibDlcType_framerelay  9
#define D_dspumibDlcType_fddi       10
#define D_dspumibDlcType_qllc       11

#endif  /* _dspumib_h_ */
      
