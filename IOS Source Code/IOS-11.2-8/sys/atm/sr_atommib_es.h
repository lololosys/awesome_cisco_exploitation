/* $Id: sr_atommib_es.h,v 3.1.4.2 1996/05/23 20:00:20 awu Exp $
 * $Source: /release/112/cvs/Xsys/atm/sr_atommib_es.h,v $
 *------------------------------------------------------------------
 * ATOMMIB support for the router products.
 *
 * April 1996, Andrew Wu 
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_atommib_es.h,v $
 * Revision 3.1.4.2  1996/05/23  20:00:20  awu
 * CSCdi56245:  agent returned non-enumeration value for row status in
 *              AToM MIB.
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/04/25  23:03:01  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/10  08:46:01  awu
 * Placeholder file.
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

#ifndef _atommib_es_h_
#define _atommib_es_h_

#include "../snmp/snmp_api.h"
#include "sr_atommib_esdefs.h"
#include "sr_atommib_essupp.h"
#include "sr_atommib_espart.h"
#include "sr_atommib_estype.h"

extern const ObjectInfo atommib_es_OidList[];
extern const uint       atommib_es_OidListNum;

/*
 * some constants
 */

#define ATM_ATOMMIB_ES_MIN_INDEX 1
#define ATM_ATOMMIB_ES_MAX_INDEX 2048
#define ATM_ATOMMIB_ES_BAD_INDEX -1

#define TR_PARAM_FAILED_INDEX 0

#define I_ifIndex 0



#endif  /* _atommib_es_h_ */
      
