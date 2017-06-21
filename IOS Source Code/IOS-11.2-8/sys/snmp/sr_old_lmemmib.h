/* $Id: sr_old_lmemmib.h,v 3.2 1995/11/17 19:00:25 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/snmp/sr_old_lmemmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1994, Kenny Roberts (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_old_lmemmib.h,v $
 * Revision 3.2  1995/11/17  19:00:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:17:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:50:01  hampton
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

#ifndef _old_lmemmib_h_
#define _old_lmemmib_h_

#include "../snmp/snmp_api.h"
#include "sr_old_lmemmibdefs.h"
#include "sr_old_lmemmibsupp.h"
#include "sr_old_lmemmibpart.h"
#include "sr_old_lmemmibtype.h"

extern const ObjectInfo old_lmemmib_OidList[];
extern const uint       old_lmemmib_OidListNum;

extern void init_old_lmemmib(void);

#endif  /* _old_lmemmib_h_ */
      
