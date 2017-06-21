/* $Id: sr_isdnmib.h,v 3.3 1996/03/05 19:40:06 bdas Exp $
 * $Source: /release/111/cvs/Xsys/isdn/sr_isdnmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * January 1995, Bibek A Das (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_isdnmib.h,v $
 * Revision 3.3  1996/03/05  19:40:06  bdas
 * CSCdi49512:  ISDN MIB trap needs to specify the direction of the call
 *
 * Revision 3.2  1995/11/17  17:42:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:15:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/30  00:20:33  bdas
 * CSCdi36243:  isdn mib trap sent for every failed call retry attempt.
 * Send the trap just once, when the first call attempt fails.
 *
 * Revision 2.1  1995/06/07  21:17:38  hampton
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

#ifndef _isdnmib_h_
#define _isdnmib_h_
#define N_demandNbrCallDetails
#include "../snmp/snmp_api.h"
#include "sr_isdnmibdefs.h"
#include "sr_isdnmibsupp.h"
#include "sr_isdnmibpart.h"
#include "sr_isdnmibtype.h"
#include "tune.h"

#define DEFAULT_NEIGHBOR_ENTRY_AGEOUT (24 * ONEHOUR)
#define MAX_DURATION  2147483647
#define FIRST_NEIGHBOR 1
#define MAX_NEIGHBORS 30
#define MAX_IFINDEX 2147483647
#define MAX_NBR_LENGTH 80
#define FAILED_CALL 1
#define ACCEPTED_CALL 2
#define REFUSED_CALL  3
extern const ObjectInfo isdnmib_OidList[];
extern const uint       isdnmib_OidListNum;
typedef struct {
       char nbr_number[MAX_NBR_LENGTH];
       ulong successful_calls;
       ulong failed_calls;
       ulong accepted_calls;
       ulong refused_calls;
       boolean retrying_call;
} neighbor_info_type;

#endif  /* _isdnmib_h_ */
      
