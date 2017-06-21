/* $Id: sr_vinesmib.h,v 3.2 1995/11/17 17:57:21 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/vines/sr_vinesmib.h,v $
 *------------------------------------------------------------------
 * S R _ V I N E S M I B . H
 *
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * September 1994, Bob Stewart (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_vinesmib.h,v $
 * Revision 3.2  1995/11/17  17:57:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:44:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:32:28  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:13:55  hampton
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

#ifndef _vinesmib_h_
#define _vinesmib_h_

#include "../snmp/snmp_api.h"
#include "sr_vinesmibdefs.h"
#include "sr_vinesmibsupp.h"
#include "sr_vinesmibpart.h"
#include "sr_vinesmibtype.h"

extern const ObjectInfo vinesmib_OidList[];
extern const uint       vinesmib_OidListNum;

extern void init_vinesmib(subsystype *);

#endif  /* _vinesmib_h_ */
      
