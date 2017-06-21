/* $Id: sr_snasdlcmib.h,v 3.2 1995/11/17 09:24:21 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ibm/sr_snasdlcmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * October 1994, Barry Chan (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_snasdlcmib.h,v $
 * Revision 3.2  1995/11/17  09:24:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:46:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:48:48  hampton
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

#ifndef _snasdlcmib_h_
#define _snasdlcmib_h_

/* SETS not supported yet */
#undef SETS

#define N_sdlc          /* define OID parts for SDLC */

#include "../snmp/snmp_api.h"
#include "sr_snasdlcmibdefs.h"
#include "sr_snasdlcmibsupp.h"
#undef SETS
#include "sr_snasdlcmibpart.h"
#include "sr_snasdlcmibtype.h"


extern const ObjectInfo snasdlcmib_OidList[];
extern const uint       snasdlcmib_OidListNum;

#endif  /* _snasdlcmib_h_ */
      
