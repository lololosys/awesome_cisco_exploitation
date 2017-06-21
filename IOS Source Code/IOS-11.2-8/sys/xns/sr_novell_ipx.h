/* $Id: sr_novell_ipx.h,v 3.2 1995/11/17 19:24:55 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/sr_novell_ipx.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * May 1994, Yunsen Wang (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_novell_ipx.h,v $
 * Revision 3.2  1995/11/17  19:24:55  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:00:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:34:05  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:30:06  hampton
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

#ifndef _novell_ipx_h_
#define _novell_ipx_h_

#include "../snmp/snmp_api.h"
#include "sr_novell_ipxdefs.h"
#include "sr_novell_ipxsupp.h"
#include "sr_novell_ipxpart.h"
#include "sr_novell_ipxtype.h"

extern const ObjectInfo novell_ipx_OidList[];
extern const uint       novell_ipx_OidListNum;

extern void init_novell_ipx(subsystype *);

/*
 * the following enumerations are not specified in the mib but are
 * specified in the Novell documentation
 */
/* enumerated values for ipxCircMediaType */
#define D_ipxCircMediaType_GenericLan  0x0000
#define D_ipxCircMediaType_GenericWan  0x8000
#define D_ipxCircMediaType_NovellEther 0x0005
#define D_ipxCircMediaType_EtherSap    0x0003
#define D_ipxCircMediaType_TokenSap    0x0004
#define D_ipxCircMediaType_FddiSap     0x0014
#define D_ipxCircMediaType_EtherSnap   0x000a
#define D_ipxCircMediaType_TokenSnap   0x000b
#define D_ipxCircMediaType_FddiSnap    0x0017
#define D_ipxCircMediaType_Isdn        0x800d
#define D_ipxCircMediaType_PPP         0x801c
#define D_ipxCircMediaType_X25         0x801e
#define D_ipxCircMediaType_Fr          0x801f

#endif  /* _novell_ipx_h_ */
      
