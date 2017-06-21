/* $Id: sr_lecmib-mib.h,v 3.1 1996/02/29 21:22:28 bstewart Exp $
 * $Source: /release/111/cvs/Xsys/lane/sr_lecmib-mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1996, Bob Stewart (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_lecmib-mib.h,v $
 * Revision 3.1  1996/02/29  21:22:28  bstewart
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

#ifndef _LECMIBMIB_H_
#define _LECMIBMIB_H_

/* SNMP-MIB translation table.  Subitem on left gets
 * translated into subitem on right.
 */

static const struct MIB_OID lecmib_oid_table[] =
{
{    "atmfLanEmulation",                "1.3.6.1.4.1.353.5.3",},
{    "leClientMIB",                     "1.3.6.1.4.1.353.5.3.1",},
{    "lecConfigEntry",                  "1.3.6.1.4.1.353.5.3.1.1.1.1",},
{    "lecStatusEntry",                  "1.3.6.1.4.1.353.5.3.1.1.2.1",},
{    "lecMappingEntry",                 "1.3.6.1.4.1.353.5.3.1.1.3.1",},
{    "lecStatisticsEntry",              "1.3.6.1.4.1.353.5.3.1.1.4.1",},
{    "lecServerVccEntry",               "1.3.6.1.4.1.353.5.3.1.1.5.1",},
{    "lecAtmAddressEntry",              "1.3.6.1.4.1.353.5.3.1.1.6.1",},
{    "lecMacAddressEntry",              "1.3.6.1.4.1.353.5.3.1.1.7.1",},
{    "lecRouteDescrEntry",              "1.3.6.1.4.1.353.5.3.1.1.8.1",},
{    "leArpEntry",                      "1.3.6.1.4.1.353.5.3.1.1.9.1",},
{    "leRDArpEntry",                    "1.3.6.1.4.1.353.5.3.1.1.10.1",},
{    0, 0},
};



/* Don't put anything after this #endif */ 
#endif /* _LECMIBMIB_H_ */
