/* $Id: sr_ospfmib.h,v 3.2 1995/11/17 17:37:33 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/sr_ospfmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * June 1994, Robert Snyder (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_ospfmib.h,v $
 * Revision 3.2  1995/11/17  17:37:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:10:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:28:09  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  21:11:19  hampton
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

#ifndef _ospfmib_h_
#define _ospfmib_h_

#include "../snmp/snmp_api.h"
#include "sr_ospfmibdefs.h"
#include "sr_ospfmibsupp.h"
#undef SETS
#include "sr_ospfmibpart.h"
#include "sr_ospfmibtype.h"

/*
 * Type of interface
 */
#define OSPF_MIB_IFT_BROADCAST	1
#define OSPF_MIB_IFT_NBMA	2
#define OSPF_MIB_IFT_P_TO_P	3
#define OSPF_MIB_IFT_P_TO_MP	5

/*
 * State of interface
 */
#define	OSPF_MIB_IFS_DOWN	1
#define	OSPF_MIB_IFS_LOOPBACK	2
#define	OSPF_MIB_IFS_WAITING	3
#define	OSPF_MIB_IFS_P_TO_P	4
#define	OSPF_MIB_IFS_DR		5
#define	OSPF_MIB_IFS_BACKUP	6
#define	OSPF_MIB_IFS_OTHER	7

extern const ObjectInfo ospfmib_OidList[];
extern const uint       ospfmib_OidListNum;

extern void init_ospfmib(subsystype *);
extern ushort ospf_running;
#endif  /* _ospfmib_h_ */
      
