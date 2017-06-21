/* $Id: sr_snapshotmib.h,v 3.2 1995/11/17 18:05:12 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/wan/sr_snapshotmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * September 1994, Joel P. Bion (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_snapshotmib.h,v $
 * Revision 3.2  1995/11/17  18:05:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:51:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:20:21  hampton
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

#ifndef _snapshotmib_h_
#define _snapshotmib_h_

#include "../snmp/snmp_api.h"
#include "sr_snapshotmibdefs.h"
#include "sr_snapshotmibsupp.h"
#include "sr_snapshotmibpart.h"
#include "sr_snapshotmibtype.h"


extern const ObjectInfo snapshotmib_OidList[];
extern const uint       snapshotmib_OidListNum;
#endif  /* _snapshotmib_h_ */

      
