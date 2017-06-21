/* $Id: sr_stunmib.h,v 3.2 1995/11/17 09:24:24 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ibm/sr_stunmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code. One line description here.
 *
 * April 1995, Bob Clouston (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_stunmib.h,v $
 * Revision 3.2  1995/11/17  09:24:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:46:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:25:56  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:48:53  hampton
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
#define N_ciscoStunMIB          /* define OID parts for STUN */

#ifndef _stunmib_h_
#define _stunmib_h_

#include "../snmp/snmp_api.h"
#include "sr_stunmibdefs.h"
#include "sr_stunmibsupp.h"
#include "sr_stunmibpart.h"
#include "sr_stunmibtype.h"
#include "../snmp/snmp_trap_fe.h"

extern const ObjectInfo stunmib_OidList[];
extern const uint       stunmib_OidListNum;
ipaddrtype get_stun_name(void);
void get_stun_pname(uchar stungroup, char name[NAMEDTHINGNAMELEN+1]);
extern void init_stunmib(subsystype *);

extern void GenerateSTUNPeerStateChangeTrap(stunpeertype *stp);

#endif  /* _stunmib_h_ */
      
