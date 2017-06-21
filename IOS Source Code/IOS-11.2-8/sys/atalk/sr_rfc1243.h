/* $Id: sr_rfc1243.h,v 3.3 1995/11/17 08:45:27 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/atalk/sr_rfc1243.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * March 1994, Yunsen Wang (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_rfc1243.h,v $
 * Revision 3.3  1995/11/17  08:45:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:06:37  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:59:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:21:49  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:11:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 * Copyright (C) 1993 by SNMP Research, Incorporated.
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

#ifndef _rfc1243_h_
#define _rfc1243_h_

#include "../snmp/snmp_api.h"
#include "sr_rfc1243defs.h"
#include "sr_rfc1243supp.h"

#undef I_llapIfIndex
#undef I_llapInPkts
#undef I_llapOutPkts
#undef I_llapInNoHandlers
#undef I_llapInLengthErrors
#undef I_llapInErrors
#undef I_llapCollisions
#undef I_llapDefers
#undef I_llapNoDataErrors
#undef I_llapRandomCTSErrors
#undef I_llapFCSErrors

#undef I_kipNetStart
#undef I_kipNetEnd
#undef I_kipNextHop
#undef I_kipHopCount
#undef I_kipBCastAddr
#undef I_kipCore
#undef I_kipType
#undef I_kipState
#undef I_kipShare

#include "sr_rfc1243part.h"
#include "sr_rfc1243type.h"

extern const ObjectInfo rfc1243_OidList[];
extern const uint       rfc1243_OidListNum;

extern void init_rfc1243(subsystype *);

extern ulong atalk_pakCreated;

#endif  /* _rfc1243_h_ */
      
