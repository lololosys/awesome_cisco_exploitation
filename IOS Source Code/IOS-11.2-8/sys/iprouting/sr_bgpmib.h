/* $Id: sr_bgpmib.h,v 3.4 1995/11/18 01:25:11 thille Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/sr_bgpmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * March 1994, Robert Snyder (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_bgpmib.h,v $
 * Revision 3.4  1995/11/18  01:25:11  thille
 * CSCdi38737:  bgp4PathAttrTable requests improperly handled
 * Fix it.
 *
 * Revision 3.3  1995/11/18  00:59:03  thille
 * CSCdi38736:  BGP snmp trap responses getting lost
 * Fix and modularize the bgp trap support.
 *
 * Revision 3.2  1995/11/17  17:37:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:10:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:27:59  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  21:11:04  hampton
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

#ifndef _bgpmib_h_
#define _bgpmib_h_

#define N_bgp
#define N_bgpPeerLastError
#define N_bgpPeerState
#define N_bgpEstablished
#define N_bgpBackwardTransition

#include "../snmp/snmp_api.h"
#include "sr_bgpmibdefs.h"
#include "sr_bgpmibsupp.h"

#undef SETS
/* no support for bgpRcvdPathAttrTable */

#undef I_bgpPathAttrPeer
#undef I_bgpPathAttrDestNetwork
#undef I_bgpPathAttrOrigin
#undef I_bgpPathAttrASPath
#undef I_bgpPathAttrNextHop
#undef I_bgpPathAttrInterASMetric

#include "sr_bgpmibpart.h"
#include "sr_bgpmibtype.h"

#undef N_bgp
#undef N_bgpPeerLastError
#undef N_bgpPeerState
#undef N_bgpEstablished
#undef N_bgpBackwardTransition

extern const ObjectInfo bgpmib_OidList[];
extern const uint       bgpmib_OidListNum;

extern void init_bgpmib(subsystype *);

#endif  /* _bgpmib_h_ */
      
