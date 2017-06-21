/* $Id: sr_atmib2.h,v 3.2 1995/11/17 09:36:25 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ip/sr_atmib2.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, Jeffrey T. Johnson (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_atmib2.h,v $
 * Revision 3.2  1995/11/17  09:36:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:58:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:26:52  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  21:00:52  hampton
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

#ifndef _atmib2_h_
#define _atmib2_h_

#include "../snmp/snmp_api.h"
#include "sr_atmib2defs.h"
#include "sr_atmib2supp.h"
#include "sr_atmib2part.h"
#include "sr_atmib2type.h"

#include "../if/arp.h"

extern const ObjectInfo atmib2_OidList[];
extern const uint       atmib2_OidListNum;

extern void init_atmib2(subsystype *);

extern arptype *find_best_arp(long ifIndex,
			      ulong netaddr,
			      long linktype,
			      boolean locktype);

extern arptype *find_exact_arp(long ifIndex,
			       ulong netaddr,
			       long linktype);

extern void remove_exact_arp(long ifIndex,
			     ulong netaddr,
			     long linktype);

/*
 * The base type for the atNteAddress field is NetworkAddress is defined
 * in RFC 1155.  This type is defined as an ASN.1 CHOICE, for which the
 * only allowed choice is of type IpAddress.  Unfortunately, the Cisco
 * atTable may also have AppleTalk entries in it.  Therefore, Cisco has
 * expanded the definition of NetwokAddress to include a second choice,
 * namely an AppleTalk address.  Since the atTable is deprecated and
 * is superceeded by per-protocol "arp" tables, this shouldn't be a
 * problem in the long tem
 */

#define SNMP_MIN_NET 1
#define SNMP_IP_NET  1
#define SNMP_AT_NET  2
#define SNMP_MAX_NET 2

#endif  /* _atmib2_h_ */
      
