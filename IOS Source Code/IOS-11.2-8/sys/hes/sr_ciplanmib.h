/* $Id: sr_ciplanmib.h,v 3.1 1995/11/09 11:42:09 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/sr_ciplanmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1995, Michael Otto (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_ciplanmib.h,v $
 * Revision 3.1  1995/11/09  11:42:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/27  20:46:18  motto
 * Code Review updates:
 *   - Make use of snmp_if_index instead of hw_if_index
 *     using snmp functions in sr_core.c.
 *   - Source line length adjustments.
 *   - Change VLAN to LAN for CIP internal LANs.
 *     This was basically renaming the MIBS,
 *     mib source files, and makefiles.
 *     This also involved changing the
 *     CIPCSNA mib references to virtual
 *     LANs on the CIP.
 *   - Fix query bug in sr_ciplanmib.c for
 *     CIP internal LAN adapters.
 * CSCdi33408:  CIP SNA SNMP MIB support
 *
 * Revision 2.1  1995/06/25  16:40:30  motto
 * Placeholder for Arizona
 *
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

#ifndef _ciplanmib_h_
#define _ciplanmib_h_

#include "../snmp/snmp_api.h"
#include "sr_ciplanmibdefs.h"
#include "sr_ciplanmibsupp.h"
#undef SETS
#include "sr_ciplanmibpart.h"
#include "sr_ciplanmibtype.h"

extern const ObjectInfo ciplanmib_OidList[];
extern const uint       ciplanmib_OidListNum;

#endif  /* _ciplanmib_h_ */
      
