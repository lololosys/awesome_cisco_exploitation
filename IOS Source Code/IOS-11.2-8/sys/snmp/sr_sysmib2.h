/* $Id: sr_sysmib2.h,v 3.2 1995/11/17 19:01:43 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/snmp/sr_sysmib2.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, Jeffrey T. Johnson (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_sysmib2.h,v $
 * Revision 3.2  1995/11/17  19:01:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:18:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:51:08  hampton
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

#ifndef _sysmib2_h_
#define _sysmib2_h_

#include "../snmp/snmp_api.h"
#include "sr_sysmib2defs.h"
#include "sr_sysmib2supp.h"
#include "sr_sysmib2part.h"
#include "sr_sysmib2type.h"

extern const ObjectInfo sysmib2_OidList[];
extern const uint       sysmib2_OidListNum;
extern       system_t   systemData;

extern char	       *snmp_contact_string;
extern char	       *snmp_location_string;

extern void  init_sysmib2(void);

#endif  /* _sysmib2_h_ */
      
