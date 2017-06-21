/* $Id: sr_novell_ripsap.h,v 3.2 1995/11/17 19:25:06 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/sr_novell_ripsap.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * May 1994, Yunsen Wang (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_novell_ripsap.h,v $
 * Revision 3.2  1995/11/17  19:25:06  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:00:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:34:12  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:30:16  hampton
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

#ifndef _novell_ripsap_h_
#define _novell_ripsap_h_

#include "../snmp/snmp_api.h"
#include "sr_novell_ripsapdefs.h"
#include "sr_novell_ripsapsupp.h"
#include "sr_novell_ripsappart.h"
#include "sr_novell_ripsaptype.h"

extern const ObjectInfo novell_ripsap_OidList[];
extern const uint       novell_ripsap_OidListNum;

extern void init_novell_ripsap(subsystype *);

#endif  /* _novell_ripsap_h_ */
      
