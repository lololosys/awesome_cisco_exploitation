/* $Id: sr_lesmib.h,v 3.2 1995/11/17 17:52:12 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lane/sr_lesmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1995, Christopher Young (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_lesmib.h,v $
 * Revision 3.2  1995/11/17  17:52:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:24:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:13:05  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/13  03:05:31  cyoung
 * place holder file for LANE Service MIB access routines.
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

#ifndef _lesmib_h_
#define _lesmib_h_

#include "../snmp/snmp_api.h"
#include "sr_lesmibdefs.h"
#include "sr_lesmibsupp.h"
#include "sr_lesmibpart.h"
#include "sr_lesmibtype.h"

extern const ObjectInfo lesmib_OidList[];
extern const uint       lesmib_OidListNum;

#define LES_VCC_INFO_INVALID -1
extern void* get_lesbus(OctetString* elan_name, 
			      long index, 
			      int searchType); 
#endif  /* _lesmib_h_ */









      
