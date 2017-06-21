/* $Id: sr_tn3270servermib.h,v 3.1.2.1 1996/07/15 07:19:24 bchan Exp $
 * $Source: /release/112/cvs/Xsys/hes/sr_tn3270servermib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * July 1996, Barry Chan (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_tn3270servermib.h,v $
 * Revision 3.1.2.1  1996/07/15  07:19:24  bchan
 * CSCdi62509:  Change tn3270 server mib
 * Branch: California_branch
 *
 * Revision 3.1  1996/07/12  02:39:33  bchan
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 * Copyright (C) 1992-1996 by SNMP Research, Incorporated.
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

#ifndef _tn3270servermib_h_
#define _tn3270servermib_h_

#include "../snmp/snmp_api.h"
#include "sr_tn3270servermibdefs.h"
#include "sr_tn3270servermibsupp.h"
#undef SETS
#include "sr_tn3270servermibpart.h"
#include "sr_tn3270servermibtype.h"

extern const ObjectInfo tn3270servermib_OidList[];
extern const uint       tn3270servermib_OidListNum;

#endif  /* _tn3270servermib_h_ */
      
