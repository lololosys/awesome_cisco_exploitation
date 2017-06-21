/* $Id: sr_ibmappnmib-mib.h,v 3.1 1996/02/29 19:47:45 bstewart Exp $
 * $Source: /release/111/cvs/Xsys/appn/sr_ibmappnmib-mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1996, Bob Stewart (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_ibmappnmib-mib.h,v $
 * Revision 3.1  1996/02/29  19:47:45  bstewart
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

#ifndef _IBMAPPNMIBMIB_H_
#define _IBMAPPNMIBMIB_H_

/* SNMP-MIB translation table.  Subitem on left gets
 * translated into subitem on right.
 */

static const struct MIB_OID ibmappnmib_oid_table[] =
{
{    "ibm6611",                         "1.3.6.1.4.1.2.6.2",},
{    "ibmappn",                         "1.3.6.1.4.1.2.6.2.13",},
{    "ibmappnNode",                     "1.3.6.1.4.1.2.6.2.13.1",},
{    "ibmappnGeneralInfoAndCaps",       "1.3.6.1.4.1.2.6.2.13.1.1",},
{    "ibmappnNnUniqueInfoAndCaps",      "1.3.6.1.4.1.2.6.2.13.1.2",},
{    "ibmappnEnUniqueCaps",             "1.3.6.1.4.1.2.6.2.13.1.3",},
{    "ibmappnPortInformation",          "1.3.6.1.4.1.2.6.2.13.1.4",},
{    "ibmappnNodePortEntry",            "1.3.6.1.4.1.2.6.2.13.1.4.1.1",},
{    "ibmappnNodePortIpEntry",          "1.3.6.1.4.1.2.6.2.13.1.4.2.1",},
{    "ibmappnNodePortDlsEntry",         "1.3.6.1.4.1.2.6.2.13.1.4.3.1",},
{    "ibmappnNodePortTrEntry",          "1.3.6.1.4.1.2.6.2.13.1.4.4.1",},
{    "ibmappnNodePortDlcTraceEntry",    "1.3.6.1.4.1.2.6.2.13.1.4.5.1",},
{    "ibmappnLinkStationInformation",   "1.3.6.1.4.1.2.6.2.13.1.5",},
{    "ibmappnNodeLsEntry",              "1.3.6.1.4.1.2.6.2.13.1.5.1.1",},
{    "ibmappnNodeLsIpEntry",            "1.3.6.1.4.1.2.6.2.13.1.5.2.1",},
{    "ibmappnNodeLsDlsEntry",           "1.3.6.1.4.1.2.6.2.13.1.5.3.1",},
{    "ibmappnNodeLsTrEntry",            "1.3.6.1.4.1.2.6.2.13.1.5.4.1",},
{    "ibmappnNodeLsStatusEntry",        "1.3.6.1.4.1.2.6.2.13.1.5.5.1",},
{    "ibmappnSnmpInformation",          "1.3.6.1.4.1.2.6.2.13.1.6",},
{    "ibmappnMemoryUse",                "1.3.6.1.4.1.2.6.2.13.1.7",},
{    "ibmappnXidInformation",           "1.3.6.1.4.1.2.6.2.13.1.8",},
{    "ibmappnNn",                       "1.3.6.1.4.1.2.6.2.13.2",},
{    "ibmappnNnTopo",                   "1.3.6.1.4.1.2.6.2.13.2.1",},
{    "ibmappnNnTopoRouteEntry",         "1.3.6.1.4.1.2.6.2.13.2.1.23.1",},
{    "ibmappnNnAdjNodeEntry",           "1.3.6.1.4.1.2.6.2.13.2.2.1",},
{    "ibmappnNnTopology",               "1.3.6.1.4.1.2.6.2.13.2.3",},
{    "ibmappnNnTopologyEntry",          "1.3.6.1.4.1.2.6.2.13.2.3.1.1",},
{    "ibmappnNnTgTopologyEntry",        "1.3.6.1.4.1.2.6.2.13.2.3.2.1",},
{    "ibmappnNnTopologyFREntry",        "1.3.6.1.4.1.2.6.2.13.2.3.3.1",},
{    "ibmappnNnTgTopologyFREntry",      "1.3.6.1.4.1.2.6.2.13.2.3.4.1",},
{    "ibmappnLocalTopology",            "1.3.6.1.4.1.2.6.2.13.3",},
{    "ibmappnLocalThisNode",            "1.3.6.1.4.1.2.6.2.13.3.1",},
{    "ibmappnLocalGeneral",             "1.3.6.1.4.1.2.6.2.13.3.1.1",},
{    "ibmappnLocalNnSpecific",          "1.3.6.1.4.1.2.6.2.13.3.1.2",},
{    "ibmappnLocalTg",                  "1.3.6.1.4.1.2.6.2.13.3.1.3",},
{    "ibmappnLocalTgEntry",             "1.3.6.1.4.1.2.6.2.13.3.1.3.1.1",},
{    "ibmappnLocalEnTopology",          "1.3.6.1.4.1.2.6.2.13.3.2",},
{    "ibmappnLocalEnEntry",             "1.3.6.1.4.1.2.6.2.13.3.2.1.1",},
{    "ibmappnLocalEnTgEntry",           "1.3.6.1.4.1.2.6.2.13.3.2.2.1",},
{    "ibmappnDir",                      "1.3.6.1.4.1.2.6.2.13.5",},
{    "ibmappnDirPerf",                  "1.3.6.1.4.1.2.6.2.13.5.1",},
{    "ibmappnDirEntry",                 "1.3.6.1.4.1.2.6.2.13.5.2.1",},
{    "ibmappnCos",                      "1.3.6.1.4.1.2.6.2.13.6",},
{    "ibmappnCosModeEntry",             "1.3.6.1.4.1.2.6.2.13.6.1.1",},
{    "ibmappnCosNameEntry",             "1.3.6.1.4.1.2.6.2.13.6.2.1",},
{    "ibmappnCosNodeRowEntry",          "1.3.6.1.4.1.2.6.2.13.6.3.1",},
{    "ibmappnCosTgRowEntry",            "1.3.6.1.4.1.2.6.2.13.6.4.1",},
{    "ibmappnSession",                  "1.3.6.1.4.1.2.6.2.13.7",},
{    "ibmappnSessIntermediate",         "1.3.6.1.4.1.2.6.2.13.7.3",},
{    "ibmappnIsInformation",            "1.3.6.1.4.1.2.6.2.13.7.3.1",},
{    "ibmappnIsInGlobal",               "1.3.6.1.4.1.2.6.2.13.7.3.1.1",},
{    "ibmappnIsInEntry",                "1.3.6.1.4.1.2.6.2.13.7.3.1.2.1",},
{    "ibmappnIsAccounting",             "1.3.6.1.4.1.2.6.2.13.7.3.2",},
{    0, 0},
};



/* Don't put anything after this #endif */ 
#endif /* _IBMAPPNMIBMIB_H_ */
