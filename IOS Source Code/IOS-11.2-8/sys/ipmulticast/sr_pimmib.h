/* $Id: sr_pimmib.h,v 3.2 1995/11/17 17:30:34 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ipmulticast/sr_pimmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * November 1994, Dino Farinacci (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_pimmib.h,v $
 * Revision 3.2  1995/11/17  17:30:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:02:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:27:19  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  21:04:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef _pimmib_h_
#define _pimmib_h_

#include "../snmp/snmp_api.h"
#include "sr_pimmibdefs.h"
#include "sr_pimmibsupp.h"
#undef SETS
#include "sr_pimmibpart.h"
#include "sr_pimmibtype.h"

extern const ObjectInfo pimmib_OidList[];
extern const uint       pimmib_OidListNum;

extern void init_pimmib(subsystype *);

#endif  /* _pimmib_h_ */
      
