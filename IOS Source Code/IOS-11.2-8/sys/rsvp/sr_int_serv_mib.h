/* $Id: sr_int_serv_mib.h,v 1.1.2.2 1996/04/18 10:48:58 fred Exp $
 * $Source: /release/112/cvs/Xsys/rsvp/sr_int_serv_mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1996, Fred Baker (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_int_serv_mib.h,v $
 * Revision 1.1.2.2  1996/04/18  10:48:58  fred
 * CSCdi55001:  compiles; MIB implementation upon IETF consensus
 * Branch: California_branch
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

#ifndef _int_serv_mib_h_
#define _int_serv_mib_h_

#include "../snmp/snmp_api.h"
#include "sr_int_serv_mibdefs.h"
#include "sr_int_serv_mibsupp.h"
#include "sr_int_serv_mibpart.h"
#include "sr_int_serv_mibtype.h"

extern const ObjectInfo int_serv_mib_OidList[];
extern const uint       int_serv_mib_OidListNum;

#endif  /* _int_serv_mib_h_ */
      
