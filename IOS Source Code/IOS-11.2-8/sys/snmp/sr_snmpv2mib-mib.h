/* $Id: sr_snmpv2mib-mib.h,v 3.1 1996/02/26 22:27:34 bstewart Exp $
 * $Source: /release/111/cvs/Xsys/snmp/sr_snmpv2mib-mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1996, Bob Stewart (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_snmpv2mib-mib.h,v $
 * Revision 3.1  1996/02/26  22:27:34  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
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

#ifndef _SNMPV2MIBMIB_H_
#define _SNMPV2MIBMIB_H_

/* SNMP-MIB translation table.  Subitem on left gets
 * translated into subitem on right.
 */

static const struct MIB_OID snmpv2mib_oid_table[] =
{
{    "snmpMIB",                         "1.3.6.1.6.3.1",},
/*   "snmpStats",                       "1.3.6.1.6.3.1.1.1"
     kept in sr_snmp-mib.h due to initialization order */
{    "snmpV1",                          "1.3.6.1.6.3.1.1.2",},
{    "snmpOR",                          "1.3.6.1.6.3.1.1.3",},
{    "snmpOREntry",                     "1.3.6.1.6.3.1.1.3.2.1",},
{    "snmpSet",                         "1.3.6.1.6.3.1.1.6",},
{    "partyMIB",                        "1.3.6.1.6.3.3",},
/*   "snmpParties",                     "1.3.6.1.6.3.3.2.1"
     kept in sr_snmp-mib.h due to initialization order */
{    "initialPartyId",                  "1.3.6.1.6.3.3.1.3",},
{    "initialContextId",                "1.3.6.1.6.3.3.1.4",},
{    "partyEntry",                      "1.3.6.1.6.3.3.2.1.1.1",},
{    "snmpContexts",                    "1.3.6.1.6.3.3.2.2",},
{    "contextEntry",                    "1.3.6.1.6.3.3.2.2.1.1",},
{    "snmpAccess",                      "1.3.6.1.6.3.3.2.3",},
{    "aclEntry",                        "1.3.6.1.6.3.3.2.3.1.1",},
{    "snmpViews",                       "1.3.6.1.6.3.3.2.4",},
{    "viewEntry",                       "1.3.6.1.6.3.3.2.4.1.1",},
{    0, 0},
};



/* Don't put anything after this #endif */ 
#endif /* _SNMPV2MIBMIB_H_ */
