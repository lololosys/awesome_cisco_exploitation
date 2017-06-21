/* $Id: sr_ifmib.h,v 3.3 1995/11/21 02:45:12 sdurham Exp $
 * $Source: /swtal/cherf.111/ios/sys/snmp/sr_ifmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1995, Sandra Durham (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_ifmib.h,v $
 * Revision 3.3  1995/11/21  02:45:12  sdurham
 * CSCdi43776:  memory leak in ifmib
 *      removed malloc's from the HC counters...
 *
 * Revision 3.2  1995/11/17  18:59:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:16:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:25:48  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:49:14  hampton
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

#ifndef _ifmib_h_
#define _ifmib_h_

#include "../snmp/snmp_api.h"
#include "sr_ifmibdefs.h"
#include "sr_ifmibsupp.h"
#include "sr_ifmibpart.h"
#include "sr_ifmibtype.h"



extern const ObjectInfo ifmib_OidList[];
extern const uint       ifmib_OidListNum;

extern void init_ifmib(subsystype *subsys);

/* define internal queues */
#define ifRcvAddrQ struct ifRcvAddrQ_
ifRcvAddrQ {
    ifRcvAddrQ	       *next;
    ifRcvAddressEntry_t  *entry;
    long           snmp_if_index;
};

#define ifStackQ struct ifStackQ_
ifStackQ {
    ifStackQ	       *next;
    ifStackEntry_t          *entry;
};

extern  ifStackQ *
ifStackQget (int	searchType,
	       long     ifHighIndex,
	       long     ifLowIndex);

extern ifRcvAddrQ *
ifRcvAddrQget (int	searchType,
	       long     ifIndex,
	       OctetString * ifRcvAddressAddress);

extern void ifStackQinsert (ifStackQ   *newifStack);
extern void ifRcvAddrQinsert (ifRcvAddrQ  *newifRcvAddr);

extern void ifRcvAddressEntry_free (ifRcvAddressEntry_t *data);
extern void ifStackEntry_free (ifStackEntry_t *data);


#endif  /* _ifmib_h_ */



      
