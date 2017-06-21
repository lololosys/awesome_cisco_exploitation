/* $Id: sr_snadlcconv01mib.h,v 3.2 1995/11/17 18:56:40 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/srt/sr_snadlcconv01mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * October 1994, Barry Chan (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_snadlcconv01mib.h,v $
 * Revision 3.2  1995/11/17  18:56:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:32:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:02:08  hampton
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

#ifndef _snadlcconv01mib_h_
#define _snadlcconv01mib_h_

#include "../snmp/snmp_api.h"
#include "sr_snadlcconv01mibdefs.h"
#include "sr_snadlcconv01mibsupp.h"
#include "sr_snadlcconv01mibpart.h"
#include "sr_snadlcconv01mibtype.h"

extern const ObjectInfo snadlcconv01mib_OidList[];
extern const uint       snadlcconv01mib_OidListNum;

extern void init_snadlcconvmib(void);

#endif  /* _snadlcconv01mib_h_ */
      
