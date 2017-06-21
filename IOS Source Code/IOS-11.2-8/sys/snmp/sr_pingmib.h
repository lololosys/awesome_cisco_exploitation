/* $Id: sr_pingmib.h,v 3.2 1995/11/17 19:00:39 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/snmp/sr_pingmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1994, Jeffrey T. Johnson (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_pingmib.h,v $
 * Revision 3.2  1995/11/17  19:00:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:17:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:50:14  hampton
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

#ifndef _pingmib_h_
#define _pingmib_h_

/* allow OID structures to be defined for the ciscoPingCompletion trap OID */
#define N_ciscoPingCompletion

#include "../snmp/snmp_api.h"
#include "sr_pingmibdefs.h"
#include "sr_pingmibsupp.h"
#include "sr_pingmibpart.h"
#include "sr_pingmibtype.h"

/*
 * The following error codes may be returned by a snmp_ping_validate function
 */
#define	SNMP_PING_VALID	0
#define SNMP_PING_INVALID_PROTOCOL 1
#define SNMP_PING_INVALID_ADDRESS 2
#define SNMP_PING_INVALID_SIZE 3

extern const ObjectInfo pingmib_OidList[];
extern const uint       pingmib_OidListNum;

extern void init_pingmib(void);

#endif  /* _pingmib_h_ */
      
