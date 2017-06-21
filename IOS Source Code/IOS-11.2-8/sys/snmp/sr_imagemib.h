/* $Id: sr_imagemib.h,v 3.3 1996/01/18 16:37:55 jjohnson Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/snmp/sr_imagemib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * January 1995, Yunsen Wang (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_imagemib.h,v $
 * Revision 3.3  1996/01/18  16:37:55  jjohnson
 * CSCdi45754:  snmp subsystem should not contain non-snmp mibs
 * move the rs232 mib, image mib, and terminal services mib into separate
 * subsystems.  move the cisco interfaces mib into the interfaces mib
 * subsystem.  consolidate the snmp timer subsystem into the snmp
 * subsystem since it really is core snmp technology.
 *
 * Revision 3.2  1995/11/17  18:59:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:17:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:49:21  hampton
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

#ifndef _imagemib_h_
#define _imagemib_h_

#include "../snmp/snmp_api.h"
#include "sr_imagemibdefs.h"
#include "sr_imagemibsupp.h"
#include "sr_imagemibpart.h"
#include "sr_imagemibtype.h"

extern const ObjectInfo imagemib_OidList[];
extern const uint       imagemib_OidListNum;

#endif  /* _imagemib_h_ */
      
