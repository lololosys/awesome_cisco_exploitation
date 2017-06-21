/* $Id: sr_ipmroutemib.h,v 3.2.58.1 1996/06/12 03:56:14 mleelani Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/sr_ipmroutemib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * November 1994, Dino Farinacci (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_ipmroutemib.h,v $
 * Revision 3.2.58.1  1996/06/12  03:56:14  mleelani
 * CSCdi58720:  Multicast fastswitching does not do fast drop.
 * Branch: California_branch
 * DO fast drop.
 *
 * Revision 3.2  1995/11/17  17:30:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:02:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:27:17  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  21:04:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef _ipmroutemib_h_
#define _ipmroutemib_h_

#include <ciscolib.h>
#include "../snmp/snmp_api.h"
#include "sr_ipmroutemibdefs.h"
#include "sr_ipmroutemibsupp.h"
#undef SETS
#include "sr_ipmroutemibpart.h"
#include "sr_ipmroutemibtype.h"
#include "../h/interface_private.h"
#include "../ipmulticast/mroute.h"

/*
 * Radix tree data structure for IP multicast groups.
 */
typedef struct snmp_radixtype_ {
    rntype      radix[2];
    ipradixtype group;
    ipradixtype mask;
} snmp_radixtype;

extern const ObjectInfo ipmroutemib_OidList[];
extern const uint       ipmroutemib_OidListNum;

extern void init_ipmroutemib(subsystype *);
extern gdbtype *snmp_get_ip_group(int, ipaddrtype);
extern void snmp_add_ip_group(ipaddrtype);
extern void snmp_delete_ip_group(ipaddrtype);
extern mdbtype *snmp_best_ip_mroute(int, ulong, ulong, ulong, gdbtype **);
extern midbtype *snmp_best_ip_midb(int, mdbtype *, uint *, ipaddrtype);
extern boolean snmp_exact_ip_boundary(idbtype *, ipaddrtype, ipaddrtype);
extern boolean snmp_best_ip_boundary(idbtype *, ipaddrtype *, ipaddrtype *,
				     boolean);

#endif  /* _ipmroutemib_h_ */
      
