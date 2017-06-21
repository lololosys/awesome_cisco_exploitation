/* $Id: sr_snmpv2mib.c,v 3.3 1996/02/26 22:27:34 bstewart Exp $
 * $Source: /release/111/cvs/Xsys/snmp/sr_snmpv2mib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, Jeffrey T. Johnson (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_snmpv2mib.c,v $
 * Revision 3.3  1996/02/26  22:27:34  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.2  1995/11/17  19:01:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:18:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:51:00  hampton
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

#include "master.h"
#include "../snmp/snmp_api.h"
#include "sys_registry.h"
#include "sr_snmpv2mib.h"
#include "sr_snmpv2mib-mib.h"
#include "registry.h"
#include "../snmp/snmp_registry.regh"

#ifdef        SR_SNMPv2

snmpStats_t   snmpStatsData;
snmpV1_t      snmpV1Data;
snmpTrap_t    snmpTrapData;
snmpSet_t     snmpSetData;


void
init_snmpv2mib(void)
{
    load_mib(snmpv2mib_OidList, snmpv2mib_OidListNum);
    load_oid(snmpv2mib_oid_table);
}

#endif
      
