/* $Id: sr_ospfmib-mib.h,v 3.1 1996/03/02 03:54:24 bstewart Exp $
 * $Source: /release/111/cvs/Xsys/iprouting/sr_ospfmib-mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * March 1996, Bob Stewart (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_ospfmib-mib.h,v $
 * Revision 3.1  1996/03/02  03:54:24  bstewart
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

#ifndef _OSPFMIBMIB_H_
#define _OSPFMIBMIB_H_

/* SNMP-MIB translation table.  Subitem on left gets
 * translated into subitem on right.
 */

static const struct MIB_OID ospfmib_oid_table[] =
{
{    "ospf",                            "1.3.6.1.2.1.14",},
{    "ospfGeneralGroup",                "1.3.6.1.2.1.14.1",},
{    "ospfAreaEntry",                   "1.3.6.1.2.1.14.2.1",},
{    "ospfStubAreaEntry",               "1.3.6.1.2.1.14.3.1",},
{    "ospfLsdbEntry",                   "1.3.6.1.2.1.14.4.1",},
{    "ospfAreaRangeEntry",              "1.3.6.1.2.1.14.5.1",},
{    "ospfHostEntry",                   "1.3.6.1.2.1.14.6.1",},
{    "ospfIfEntry",                     "1.3.6.1.2.1.14.7.1",},
{    "ospfIfMetricEntry",               "1.3.6.1.2.1.14.8.1",},
{    "ospfVirtIfEntry",                 "1.3.6.1.2.1.14.9.1",},
{    "ospfNbrEntry",                    "1.3.6.1.2.1.14.10.1",},
{    "ospfVirtNbrEntry",                "1.3.6.1.2.1.14.11.1",},
{    0, 0},
};



/* Don't put anything after this #endif */ 
#endif /* _OSPFMIBMIB_H_ */
