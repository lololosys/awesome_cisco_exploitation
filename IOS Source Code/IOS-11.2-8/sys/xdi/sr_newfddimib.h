/* $Id: sr_newfddimib.h,v 3.2 1995/11/17 19:20:02 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/sr_newfddimib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * RFC 1512 FDDI SMT 7.3 MIB Support
 * June 1994, Chifei W. Cheng (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_newfddimib.h,v $
 * Revision 3.2  1995/11/17  19:20:02  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:56:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:33:35  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:26:16  hampton
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

#ifndef _newfddimib_h_
#define _newfddimib_h_

#include "../snmp/snmp_api.h"
#include "sr_newfddimibdefs.h"
#include "sr_newfddimibsupp.h"
#include "sr_newfddimibpart.h"
#include "sr_newfddimibtype.h"

#define SIZE_SMTStationIdType 8
#define SIZE_SMTUserDataType 32
#define SIZE_FDDITimeStamp 8
#define SIZE_FddiMibPortReqPaths 3

/* some define's that should have picked up by the -defs.h for integer range */
#define D_fddiTime_LowBound	0
#define D_fddiTime_HighBound	2147483647
#define D_fddimibSMTCfgPlcy_LowBound	0
#define D_fddimibSMTCfgPlcy_HighBound	1
#define D_fddimibSMTConnPlcy_LowBound	32768
#define D_fddimibSMTConnPlcy_HighBound	65535
#define D_fddimibSMTNotify_LowBound	2
#define D_fddimibSMTNotify_HighBound	30
#define D_fddimibSMTaction_LowBound	1
#define D_fddimibSMTaction_HighBound	8
#define D_fddimibMacReqPath_LowBound	0
#define D_fddimibMacReqPath_HighBound	255
#define D_fddimibMacErrThrshd_LowBound	0
#define D_fddimibMacErrThrshd_HighBound	65535
#define D_fddimibPathMaxLowBound_min	10000000
#define D_fddimibPortConnPlcy_LowBound	0
#define D_fddimibPortConnPlcy_HighBound	3
#define D_fddimibPortLnkErr_LowBound	4
#define D_fddimibPortLnkErr_HighBound	15

extern const ObjectInfo newfddimib_OidList[];
extern const uint       newfddimib_OidListNum;

extern void init_newfddimib(subsystype *);

#endif  /* _newfddimib_h_ */
      
