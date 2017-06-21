/* $Id: sr_snmpv2mib.h,v 3.2 1995/11/17 19:01:38 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/snmp/sr_snmpv2mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, Jeffrey T. Johnson (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_snmpv2mib.h,v $
 * Revision 3.2  1995/11/17  19:01:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:18:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:51:02  hampton
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

#ifndef _snmpv2mib_h_
#define _snmpv2mib_h_

#include "../snmp/snmp_api.h"
#include "sr_snmpv2mibdefs.h"
#include "sr_snmpv2mibsupp.h"
#include "sr_snmpv2mibpart.h"
#include "sr_snmpv2mibtype.h"

extern const ObjectInfo snmpv2mib_OidList[];
extern const uint       snmpv2mib_OidListNum;

extern void init_snmpv2mib(void);

extern snmpStats_t   snmpStatsData;
extern snmpV1_t      snmpV1Data;
extern snmpOR_t      snmpORData;
extern snmpOREntry_t snmpOREntryData;
extern snmpTrap_t    snmpTrapData;
extern snmpSet_t     snmpSetData;

#define SNMPv2_TRAPS_ENABLED  D_snmpV2EnableAuthenTraps_true
#define SNMPv2_TRAPS_DISABLED D_snmpV2EnableAuthenTraps_false

#endif  /* _snmpv2mib_h_ */
      
