/* $Id: sr_cdlswmib.h,v 3.2 1995/11/17 09:04:14 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dlsw/sr_cdlswmib.h,v $
 *------------------------------------------------------------------
 * Cisco DLSw MIB Header
 *
 * October 1995, Ravi Periasamy
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_cdlswmib.h,v $
 * Revision 3.2  1995/11/17  09:04:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:25:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:56:10  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/05  19:46:54  ravip
 * Placeholder for Cisco DLSw MIB Files
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

#ifndef _cdlswmib_h_
#define _cdlswmib_h_

#include "../snmp/snmp_api.h"
#include "sr_cdlswmibdefs.h"
#include "sr_cdlswmibsupp.h"
#include "sr_cdlswmibpart.h"
#include "sr_cdlswmibtype.h"

extern const ObjectInfo cdlswmib_OidList[];
extern const uint       cdlswmib_OidListNum;

#endif  /* _cdlswmib_h_ */
      
