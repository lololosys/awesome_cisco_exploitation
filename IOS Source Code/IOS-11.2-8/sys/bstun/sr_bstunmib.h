/* $Id: sr_bstunmib.h,v 3.2 1995/11/17 08:49:31 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/bstun/sr_bstunmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1995, Silvana Zdravkovski (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_bstunmib.h,v $
 * Revision 3.2  1995/11/17  08:49:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:14:47  hampton
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

#ifndef _bstunmib_h_
#define _bstunmib_h_

#include "../snmp/snmp_api.h"
#include "sr_bstunmibdefs.h"
#include "sr_bstunmibsupp.h"
#include "sr_bstunmibpart.h"
#include "sr_bstunmibtype.h"

extern const ObjectInfo bstunmib_OidList[];
extern const uint       bstunmib_OidListNum;

#endif  /* _bstunmib_h_ */
      
