/* $Id: sr_ilmimib.h,v 3.2 1995/11/17 08:47:12 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/atm/sr_ilmimib.h,v $
 *------------------------------------------------------------------
 * Placeholder for ILMI support
 *
 * March, 1995   Kartik Chandrasekhar
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_ilmimib.h,v $
 * Revision 3.2  1995/11/17  08:47:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:01:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:22:26  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:12:49  hampton
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

#ifndef _ilmimib_h_
#define _ilmimib_h_

#include "../snmp/snmp_api.h"
#include "sr_ilmimibdefs.h"
#include "sr_ilmimibsupp.h"

#define N_atmForum
/* List of objects not supported. */
#undef I_atmfPortAddress     /* Object Deprecated. See Addr. Reg. MIB. */
#undef I_atmfMyOsiNmNsapAddress
#undef I_atmfVpcQoSCategory
#undef I_atmfVccQoSCategory
#undef I_atmfAtmLayerMaxVPCs
#undef I_atmfAtmLayerConfiguredVPCs

#define N_enterprises
#define N_atmForum
#define N_atmForumAdmin
#define N_atmfTransmissionTypes
#define N_atmfUnknownType
#define N_atmfSonetSTS3c
#define N_atmfDs3
#define N_atmf4B5B
#define N_atmf8B10B
#define N_atmfMediaTypes
#define N_atmfMediaUnknownType
#define N_atmfMediaCoaxCable
#define N_atmfMediaSingleMode
#define N_atmfMediaMultiMode
#define N_atmfMediaStp
#define N_atmfMediaUtp
#define N_atmfTrafficDescrTypes
#define N_atmfNoDescriptor
#define N_atmfPeakRate
#define N_atmfNoClpNoScr
#define N_atmfClpNoTaggingNoScr
#define N_atmfClpTaggingNoScr
#define N_atmfNoClpScr
#define N_atmfClpNoTaggingScr
#define N_atmfClpTaggingScr
#define N_atmForumUni
#define N_atmfPhysicalGroup

#include "sr_ilmimibpart.h"
#include "sr_ilmimibtype.h"

extern const ObjectInfo ilmimib_OidList[];
extern const uint       ilmimib_OidListNum;

extern void init_ilmimib(subsystype *);
#endif  /* _ilmimib_h_ */
      
