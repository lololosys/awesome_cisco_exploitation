/* $Id: sr_qllc01mib.h,v 3.2 1995/11/17 18:56:22 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/srt/sr_qllc01mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * June 1994, Duane DeCapite (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_qllc01mib.h,v $
 * Revision 3.2  1995/11/17  18:56:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:32:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:01:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef _qllc01mib_h_
#define _qllc01mib_h_

#include "../snmp/snmp_api.h"
#include "sr_qllc01mibdefs.h"
#include "sr_qllc01mibsupp.h"
#include "sr_qllc01mibpart.h"
#include "sr_qllc01mibtype.h"

extern const ObjectInfo qllc01mib_OidList[];
extern const uint       qllc01mib_OidListNum;

extern void init_qllcmib(void);

#endif  /* _qllc01mib_h_ */
      
