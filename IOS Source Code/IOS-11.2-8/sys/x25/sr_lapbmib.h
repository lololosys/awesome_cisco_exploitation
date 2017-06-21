/* $Id: sr_lapbmib.h,v 3.2 1995/11/17 18:06:53 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/x25/sr_lapbmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1994, Nicholas Thille (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_lapbmib.h,v $
 * Revision 3.2  1995/11/17  18:06:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:53:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:33:19  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:21:36  hampton
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

#ifndef _lapbmib_h_
#define _lapbmib_h_

#define N_lapbProtocolCcittV1984

#include "../snmp/snmp_api.h"
#include "sr_lapbmibdefs.h"
#include "sr_lapbmibsupp.h"
#undef SETS

#include "sr_lapbmibpart.h"
#include "sr_lapbmibtype.h"

/* RFC 1382 */

#define MAX_POSITIVE_32 0x7fffffff

extern const ObjectInfo lapbmib_OidList[];
extern const uint       lapbmib_OidListNum;

extern void init_lapbmib(subsystype *);

#endif  /* _lapbmib_h_ */
      
