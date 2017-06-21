/* $Id: sr_cdpmib.h,v 3.2.60.1 1996/07/11 17:19:41 tylin Exp $
 * $Source: /release/112/cvs/Xsys/cdp/sr_cdpmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * November 1994, Arun Sastry (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_cdpmib.h,v $
 * Revision 3.2.60.1  1996/07/11  17:19:41  tylin
 * CSCdi59144:  setting cdpInterfaceEnable, cdpInterfaceMessageInterval =>
 * noSuchNam
 *    Obsolete wrongly defined object cdpInterfaceMessageInterval and
 *    define new
 *    object group cdpGlobal corresponds to command '[no] cdp run', 'cdp
 *    timer'
 *    and 'cdp holdtime'
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:50:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:05:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/27  07:21:17  smackie
 * Convert CDP to use platform_get_string(). Tidy up subsystem entry
 * points to use the right prototype. (CSCdi36461)
 *
 * Revision 2.1  1995/06/07  20:15:13  hampton
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

#ifndef _cdpmib_h_
#define _cdpmib_h_

#include "../snmp/snmp_api.h"
#include "sr_cdpmibdefs.h"
#include "sr_cdpmibsupp.h"
#ifdef I_cdpInterfaceMessageInterval
#undef I_cdpInterfaceMessageInterval
#endif /* I_cdpInterfaceMessageInterval */
#include "sr_cdpmibpart.h"
#include "sr_cdpmibtype.h"

extern const ObjectInfo cdpmib_OidList[];
extern const uint       cdpmib_OidListNum;

#endif  /* _cdpmib_h_ */
      
