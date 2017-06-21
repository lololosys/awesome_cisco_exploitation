/* $Id: sr_ds1mib.h,v 3.2 1995/11/17 09:20:07 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/sr_ds1mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1994, Kenny Roberts (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_ds1mib.h,v $
 * Revision 3.2  1995/11/17  09:20:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:42:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:24:50  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:43:57  hampton
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

#ifndef _ds1mib_h_
#define _ds1mib_h_

#include "../snmp/snmp_api.h"
#include "sr_ds1mibdefs.h"
#include "sr_ds1mibsupp.h"
#undef SETS

/* dsx1FarEndCurrentTable not supported, we are a Near End */

#undef I_dsx1FarEndCurrentIndex
#undef I_dsx1FarEndTimeElapsed
#undef I_dsx1FarEndValidIntervals
#undef I_dsx1FarEndCurrentESs
#undef I_dsx1FarEndCurrentSESs
#undef I_dsx1FarEndCurrentSEFSs
#undef I_dsx1FarEndCurrentUASs
#undef I_dsx1FarEndCurrentCSSs
#undef I_dsx1FarEndCurrentLESs
#undef I_dsx1FarEndCurrentPCVs
#undef I_dsx1FarEndCurrentBESs
#undef I_dsx1FarEndCurrentDMs

/* dsx1FarEndIntervalTable not supported, we are a Near End */

#undef I_dsx1FarEndIntervalIndex
#undef I_dsx1FarEndIntervalNumber
#undef I_dsx1FarEndIntervalESs
#undef I_dsx1FarEndIntervalSESs
#undef I_dsx1FarEndIntervalSEFSs
#undef I_dsx1FarEndIntervalUASs
#undef I_dsx1FarEndIntervalCSSs
#undef I_dsx1FarEndIntervalLESs
#undef I_dsx1FarEndIntervalPCVs
#undef I_dsx1FarEndIntervalBESs
#undef I_dsx1FarEndIntervalDMs

/* dsx1FarEndTotalTable not supported, we are a Near End */

#undef I_dsx1FarEndTotalIndex
#undef I_dsx1FarEndTotalESs
#undef I_dsx1FarEndTotalSESs
#undef I_dsx1FarEndTotalSEFSs
#undef I_dsx1FarEndTotalUASs
#undef I_dsx1FarEndTotalCSSs
#undef I_dsx1FarEndTotalLESs
#undef I_dsx1FarEndTotalPCVs
#undef I_dsx1FarEndTotalBESs
#undef I_dsx1FarEndTotalDMs


#include "sr_ds1mibpart.h"
#include "sr_ds1mibtype.h"

extern const ObjectInfo ds1mib_OidList[];
extern const uint       ds1mib_OidListNum;

extern void init_ds1mib(subsystype *);
extern int cardifmib_get_card_index (cdbtype *cdb);
extern long cardifmib_get_slot_number (cdbtype *cdb);
extern long cardifmib_get_port_number (cdbtype *cdb);
extern long cardifmib_get_connector_type (cdbtype *cdb);

#endif  /* _ds1mib_h_ */
      
