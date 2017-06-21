/* $Id: sr_queuemib.h,v 3.1 1995/11/09 11:52:38 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/if/sr_queuemib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * June 1995, Fred Baker (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_queuemib.h,v $
 * Revision 3.1  1995/11/09  11:52:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/23  18:13:53  fred
 * CSCdi36295:  Commit Queue MIB to 11.0
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

#ifndef _queuemib_h_
#define _queuemib_h_

#include "../snmp/snmp_api.h"
#include "sr_queuemibdefs.h"
#include "sr_queuemibsupp.h"
#include "sr_queuemibpart.h"
#include "sr_queuemibtype.h"

extern const ObjectInfo queuemib_OidList[];
extern const uint       queuemib_OidListNum;

#endif  /* _queuemib_h_ */
      
