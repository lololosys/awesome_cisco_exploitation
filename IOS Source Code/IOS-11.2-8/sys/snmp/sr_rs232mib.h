/* $Id: sr_rs232mib.h,v 3.3 1996/01/18 16:37:59 jjohnson Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/snmp/sr_rs232mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * June 1994, Nicholas Thille (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_rs232mib.h,v $
 * Revision 3.3  1996/01/18  16:37:59  jjohnson
 * CSCdi45754:  snmp subsystem should not contain non-snmp mibs
 * move the rs232 mib, image mib, and terminal services mib into separate
 * subsystems.  move the cisco interfaces mib into the interfaces mib
 * subsystem.  consolidate the snmp timer subsystem into the snmp
 * subsystem since it really is core snmp technology.
 *
 * Revision 3.2  1995/11/17  19:00:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:17:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:50:22  hampton
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

#ifndef _rs232mib_h_
#define _rs232mib_h_

#include "../snmp/snmp_api.h"
#include "sr_rs232mibdefs.h"
#include "sr_rs232mibsupp.h"

/* all #undef's should be done here for unsupported variables */
#undef I_rs232InSigChanges
#undef I_rs232OutSigChanges
#undef SETS

#include "sr_rs232mibpart.h"
#include "sr_rs232mibtype.h"

extern const ObjectInfo rs232mib_OidList[];
extern const uint       rs232mib_OidListNum;

#endif  /* _rs232mib_h_ */
      
