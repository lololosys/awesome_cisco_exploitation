/* $Id: sr_lecmib.h,v 3.2 1995/11/17 17:51:56 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lane/sr_lecmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * June 1995, Andy Bierman (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_lecmib.h,v $
 * Revision 3.2  1995/11/17  17:51:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:23:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:12:55  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/05  21:49:18  abierman
 * Adding placeholder files to support ATMF LEC MIB (atm94-0737R4)
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _lecmib_h_
#define _lecmib_h_

#include "../snmp/snmp_api.h"
#include "sr_lecmibdefs.h"
#include "sr_lecmibsupp.h"
#include "sr_lecmibpart.h"
#include "sr_lecmibtype.h"

#define LEC_VCC_INFO_INVALID 0

extern const ObjectInfo lecmib_OidList[];
extern const uint       lecmib_OidListNum;

#endif  /* _lecmib_h_ */
      
