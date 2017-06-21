/* $Id: sr_old_lipmib.h,v 3.2 1995/11/17 09:36:38 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ip/sr_old_lipmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * May 1994, Nicholas Thille (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_old_lipmib.h,v $
 * Revision 3.2  1995/11/17  09:36:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:58:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:01:09  hampton
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

#ifndef _old_lipmib_h_
#define _old_lipmib_h_

#include "../snmp/snmp_api.h"
#include "sr_old_lipmibdefs.h"
#include "sr_old_lipmibsupp.h"
#include "sr_old_lipmibpart.h"
#include "sr_old_lipmibtype.h"

extern const ObjectInfo old_lipmib_OidList[];
extern const uint       old_lipmib_OidListNum;

extern void init_old_lipmib(void);

#endif  /* _old_lipmib_h_ */
      
