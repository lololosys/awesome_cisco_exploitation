/* $Id: sr_tcpmib2.h,v 3.2 1995/11/17 18:48:24 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/tcp/sr_tcpmib2.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * March 1994, Jeffrey T. Johnson (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_tcpmib2.h,v $
 * Revision 3.2  1995/11/17  18:48:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:35:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:31:55  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:05:34  hampton
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

#ifndef _tcpmib2_h_
#define _tcpmib2_h_

#include "../snmp/snmp_api.h"
#include "sr_tcpmib2defs.h"
#include "sr_tcpmib2supp.h"
#include "sr_tcpmib2part.h"
#include "sr_tcpmib2type.h"

extern const ObjectInfo tcpmib2_OidList[];
extern const uint       tcpmib2_OidListNum;

extern void init_tcpmib2(subsystype *);
extern tcbtype *find_tcp_connection(
    int             serialNum,
    int             searchType,
    unsigned long   tcpConnLocalAddress,
    long            tcpConnLocalPort,
    unsigned long   tcpConnRemAddress,
    long            tcpConnRemPort);


#endif  /* _tcpmib2_h_ */
      
