/* $Id: sr_atmaddregmib.h,v 3.2 1995/11/17 08:47:09 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/atm/sr_atmaddregmib.h,v $
 *------------------------------------------------------------------
 * Placeholder for ILMI support
 *
 * March, 1995   Kartik Chandrasekhar
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_atmaddregmib.h,v $
 * Revision 3.2  1995/11/17  08:47:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:01:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:22:22  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:12:45  hampton
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

#ifndef _atmaddregmib_h_
#define _atmaddregmib_h_

#include "../snmp/snmp_api.h"
#include "sr_atmaddregmibdefs.h"
#include "sr_atmaddregmibsupp.h"
#include "sr_atmaddregmibpart.h"
#include "sr_atmaddregmibtype.h"

#define atmfAddressEntry_READ_CREATE
#define atmfNetPrefixEntry_READ_CREATE

extern const ObjectInfo atmaddregmib_OidList[];
extern const uint       atmaddregmib_OidListNum;

extern void init_atmaddregmib(subsystype *);

#endif  /* _atmaddregmib_h_ */
      
