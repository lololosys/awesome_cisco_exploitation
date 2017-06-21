/* $Id: sr_ethermib.h,v 3.2 1995/11/17 18:59:15 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/snmp/sr_ethermib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, Robert Snyder (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_ethermib.h,v $
 * Revision 3.2  1995/11/17  18:59:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:16:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  10:21:54  smackie
 * Subsystem header cleanup. Round up the last few stragglers and rope 'em
 * in with the rest of the herd. (CSCdi23568)
 *
 * Revision 2.1  1995/06/07  22:49:03  hampton
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

#ifndef _ethermib_h_
#define _ethermib_h_

#include "../snmp/snmp_api.h"
#include "sr_ethermibdefs.h"
#include "sr_ethermibsupp.h"

/* all #undef's should be done here for unsupported variables */
#undef I_dot3CollIndex
#undef I_dot3CollFrequencies
#undef I_dot3CollCount

#include "sr_ethermibpart.h"
#include "sr_ethermibtype.h"

extern const ObjectInfo ethermib_OidList[];
extern const uint       ethermib_OidListNum;

extern void init_ethermib(subsystype *);

#endif  /* _ethermib_h_ */
      
