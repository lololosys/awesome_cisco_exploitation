/* $Id: sr_rptrmib-mib.h,v 3.1 1996/02/13 23:27:10 bstewart Exp $
 * $Source: /release/111/cvs/Xsys/les/sr_rptrmib-mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1996, Bob Stewart (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_rptrmib-mib.h,v $
 * Revision 3.1  1996/02/13  23:27:10  bstewart
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

#ifndef _RPTRMIBMIB_H_
#define _RPTRMIBMIB_H_

/* SNMP-MIB translation table.  Subitem on left gets
 * translated into subitem on right.
 */

static const struct MIB_OID rptrmib_oid_table[] =
{
{    "snmpDot3RptrMgt",                 "1.3.6.1.2.1.22",},
{    "rptrBasicPackage",                "1.3.6.1.2.1.22.1",},
{    "rptrRptrInfo",                    "1.3.6.1.2.1.22.1.1",},
{    "rptrGroupInfo",                   "1.3.6.1.2.1.22.1.2",},
{    "rptrGroupEntry",                  "1.3.6.1.2.1.22.1.2.1.1",},
{    "rptrPortInfo",                    "1.3.6.1.2.1.22.1.3",},
{    "rptrPortEntry",                   "1.3.6.1.2.1.22.1.3.1.1",},
{    "rptrMonitorPackage",              "1.3.6.1.2.1.22.2",},
{    "rptrMonitorRptrInfo",             "1.3.6.1.2.1.22.2.1",},
{    "rptrMonitorGroupInfo",            "1.3.6.1.2.1.22.2.2",},
{    "rptrMonitorGroupEntry",           "1.3.6.1.2.1.22.2.2.1.1",},
{    "rptrMonitorPortInfo",             "1.3.6.1.2.1.22.2.3",},
{    "rptrMonitorPortEntry",            "1.3.6.1.2.1.22.2.3.1.1",},
{    "rptrAddrTrackPackage",            "1.3.6.1.2.1.22.3",},
{    "rptrAddrTrackRptrInfo",           "1.3.6.1.2.1.22.3.1",},
{    "rptrAddrTrackGroupInfo",          "1.3.6.1.2.1.22.3.2",},
{    "rptrAddrTrackPortInfo",           "1.3.6.1.2.1.22.3.3",},
{    "rptrAddrTrackEntry",              "1.3.6.1.2.1.22.3.3.1.1",},
{    "ciscoRptrMIB",                    "1.3.6.1.4.1.9.9.22",},
{    "ciscoRptrPortEntry",              "1.3.6.1.4.1.9.9.22.1.1.1",},
{    "ciscoRptrMIBglobal",              "1.3.6.1.4.1.9.9.22.1.2",},
{    0, 0},
};



/* Don't put anything after this #endif */ 
#endif /* _RPTRMIBMIB_H_ */
