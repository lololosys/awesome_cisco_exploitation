/* $Id: sr_rmonlitemib.h,v 3.2 1995/11/17 18:39:43 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rmon/sr_rmonlitemib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * August 1995, Jeffrey T. Johnson (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_rmonlitemib.h,v $
 * Revision 3.2  1995/11/17  18:39:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:28  shaker
 * Bump version numbers from 2.x to 3.x.
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

#ifndef _rmonlitemib_h_
#define _rmonlitemib_h_

#include "../snmp/snmp_api.h"
#include "sr_rmonlitemibdefs.h"
#include "sr_rmonlitemibsupp.h"
#include "sr_rmonlitemibpart.h"
#include "sr_rmonlitemibtype.h"

extern void k_alarmEntry_initialize(void);
extern void k_eventEntry_initialize(void);

extern const ObjectInfo rmonlitemib_OidList[];
extern const uint       rmonlitemib_OidListNum;

#endif  /* _rmonlitemib_h_ */
      
