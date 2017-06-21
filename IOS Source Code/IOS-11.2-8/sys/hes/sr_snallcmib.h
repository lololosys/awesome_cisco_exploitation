/* $Id: sr_snallcmib.h,v 3.2.60.2 1996/04/04 01:54:15 mordock Exp $
 * $Source: /release/112/cvs/Xsys/hes/sr_snallcmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1995, Michael Otto (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_snallcmib.h,v $
 * Revision 3.2.60.2  1996/04/04  01:54:15  mordock
 * CSCdi53463:  Invalid access to the snmp mib dispatch table
 * Branch: California_branch
 * more than one source needs access to do_snallc_trap().  callers of this
 * routine need to use new type/value struct for passing in varbind values.
 *
 * Revision 3.2.60.1  1996/04/03  00:12:59  jjohnson
 * CSCdi53463:  Invalid access to the snmp mib dispatch table
 * Implement a different mechanism for creating trap varbind lists
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:20:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:42:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/09  19:29:26  rbatz
 * CSCdi38250:  CIP CSNA SNMP traps do not work
 *
 * Revision 2.1  1995/06/07  20:44:32  hampton
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

#ifndef _snallcmib_h_
#define _snallcmib_h_

#define N_snaLlcMIBNotificationPrefix

#include "../snmp/snmp_api.h"
#include "sr_snallcmibdefs.h"
#include "sr_snallcmibsupp.h"
#undef SETS
#include "sr_snallcmibpart.h"
#include "sr_snallcmibtype.h"

extern const ObjectInfo snallcmib_OidList[];
extern const uint       snallcmib_OidListNum;

/* trap varbind list - for do_snallc_trap() */
typedef struct _vb_value {
    int type;
    void *value;
} vb_value;

extern void *get_sap_entry( int searchType, long *ifIndex, long *llcPortVirtualIndex,
                            long llcSapNumber, void *sap_data, ushort size, 
                            ushort table_id);

void snaLlcStatusChangeTrap( hwidbtype * hwidb, 
                             cip_llctype *csna_data,
                             ushort code);

extern void do_snallc_trap(const OID* vbList, const OID* entOid,
			   char* v2TrapOID, OID* indexOID,
			   int vbLength, int genericTrapNum, 
			   int specificTrapNum, vb_value dataP[]);

#endif  /* _snallcmib_h_ */
      
