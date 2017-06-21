/* $Id: sr_confmanmib.h,v 3.2 1995/11/17 18:59:04 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/snmp/sr_confmanmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * May 1995, Bob Stewart (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_confmanmib.h,v $
 * Revision 3.2  1995/11/17  18:59:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:16:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:25:44  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/15  22:40:11  thille
 * Add placeholders for Bob's config history feature and MIB.
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

#ifndef _confmanmib_h_
#define _confmanmib_h_

#define N_ciscoConfigManEvent

#include "../snmp/snmp_api.h"
#include "sr_confmanmibdefs.h"
#include "sr_confmanmibsupp.h"
#include "sr_confmanmibpart.h"
#include "sr_confmanmibtype.h"

extern const ObjectInfo confmanmib_OidList[];
extern const uint       confmanmib_OidListNum;

#endif  /* _confmanmib_h_ */
      
