/* $Id: sr_rsvp_mib-mib.h,v 1.1.2.1 1996/04/18 10:48:59 fred Exp $
 * $Source: /release/112/cvs/Xsys/rsvp/sr_rsvp_mib-mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1996, Fred Baker (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_rsvp_mib-mib.h,v $
 * Revision 1.1.2.1  1996/04/18  10:48:59  fred
 * CSCdi55001:  compiles; MIB implementation upon IETF consensus
 * Branch: California_branch
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

#ifndef _RSVP_MIBMIB_H_
#define _RSVP_MIBMIB_H_

/* SNMP-MIB translation table.  Subitem on left gets
 * translated into subitem on right.
 */

static const struct MIB_OID rsvp_mib_oid_table[] =
{
{    "experimental",                    "1.3.6.1.3",},
{    "rsvp",                            "1.3.6.1.3.71",},
{    "rsvpObjects",                     "1.3.6.1.3.71.1",},
{    "rsvpSenderTable",                 "1.3.6.1.3.71.1.1",},
{    "rsvpSenderEntry",                 "1.3.6.1.3.71.1.1.1",},
{    "rsvpSenderSessionIndex",          "1.3.6.1.3.71.1.1.1.1",},
{    "rsvpSenderTSpecRate",             "1.3.6.1.3.71.1.1.1.2",},
{    "rsvpSenderTSpecPeakRate",         "1.3.6.1.3.71.1.1.1.3",},
{    "rsvpSenderTSpecBurst",            "1.3.6.1.3.71.1.1.1.4",},
{    "rsvpSenderTSpecMinTU",            "1.3.6.1.3.71.1.1.1.5",},
{    "rsvpSenderTSpecMaxTU",            "1.3.6.1.3.71.1.1.1.6",},
{    "rsvpSenderInterval",              "1.3.6.1.3.71.1.1.1.7",},
{    "rsvpSenderRSVPHop",               "1.3.6.1.3.71.1.1.1.8",},
{    "rsvpSenderLastChange",            "1.3.6.1.3.71.1.1.1.9",},
{    "rsvpSenderStatus",                "1.3.6.1.3.71.1.1.1.10",},
{    "rsvpResvTable",                   "1.3.6.1.3.71.1.2",},
{    "rsvpResvEntry",                   "1.3.6.1.3.71.1.2.1",},
{    "rsvpResvSessionIndex",            "1.3.6.1.3.71.1.2.1.1",},
{    "rsvpResvService",                 "1.3.6.1.3.71.1.2.1.8",},
{    "rsvpResvTSpecRate",               "1.3.6.1.3.71.1.2.1.9",},
{    "rsvpResvTSpecPeakRate",           "1.3.6.1.3.71.1.2.1.10",},
{    "rsvpResvTSpecBurst",              "1.3.6.1.3.71.1.2.1.11",},
{    "rsvpResvTSpecLevel",              "1.3.6.1.3.71.1.2.1.12",},
{    "rsvpResvTSpecMinTU",              "1.3.6.1.3.71.1.2.1.13",},
{    "rsvpResvTSpecMaxTU",              "1.3.6.1.3.71.1.2.1.14",},
{    "rsvpResvInterval",                "1.3.6.1.3.71.1.2.1.15",},
{    "rsvpResvScope",                   "1.3.6.1.3.71.1.2.1.16",},
{    "rsvpResvShared",                  "1.3.6.1.3.71.1.2.1.17",},
{    "rsvpResvExplicit",                "1.3.6.1.3.71.1.2.1.18",},
{    "rsvpResvRSVPHop",                 "1.3.6.1.3.71.1.2.1.19",},
{    "rsvpResvLastChange",              "1.3.6.1.3.71.1.2.1.20",},
{    "rsvpResvStatus",                  "1.3.6.1.3.71.1.2.1.21",},
{    "rsvpResvFwdTable",                "1.3.6.1.3.71.1.3",},
{    "rsvpResvFwdEntry",                "1.3.6.1.3.71.1.3.1",},
{    "rsvpResvFwdSessionIndex",         "1.3.6.1.3.71.1.3.1.1",},
{    "rsvpResvFwdService",              "1.3.6.1.3.71.1.3.1.6",},
{    "rsvpResvFwdTSpecRate",            "1.3.6.1.3.71.1.3.1.7",},
{    "rsvpResvFwdTSpecPeakRate",        "1.3.6.1.3.71.1.3.1.8",},
{    "rsvpResvFwdTSpecBurst",           "1.3.6.1.3.71.1.3.1.9",},
{    "rsvpResvFwdTSpecLevel",           "1.3.6.1.3.71.1.3.1.10",},
{    "rsvpResvFwdTSpecMinTU",           "1.3.6.1.3.71.1.3.1.11",},
{    "rsvpResvFwdTSpecMaxTU",           "1.3.6.1.3.71.1.3.1.12",},
{    "rsvpResvFwdInterval",             "1.3.6.1.3.71.1.3.1.13",},
{    "rsvpResvFwdScope",                "1.3.6.1.3.71.1.3.1.14",},
{    "rsvpResvFwdShared",               "1.3.6.1.3.71.1.3.1.15",},
{    "rsvpResvFwdExplicit",             "1.3.6.1.3.71.1.3.1.16",},
{    "rsvpResvFwdRSVPHop",              "1.3.6.1.3.71.1.3.1.17",},
{    "rsvpResvFwdLastChange",           "1.3.6.1.3.71.1.3.1.18",},
{    "rsvpResvFwdStatus",               "1.3.6.1.3.71.1.3.1.19",},
{    "rsvpIfAttribTable",               "1.3.6.1.3.71.1.4",},
{    "rsvpIfAttribEntry",               "1.3.6.1.3.71.1.4.1",},
{    "rsvpIfAttribUdpNbrs",             "1.3.6.1.3.71.1.4.1.1",},
{    "rsvpIfAttribIpNbrs",              "1.3.6.1.3.71.1.4.1.2",},
{    "rsvpIfAttribNbrs",                "1.3.6.1.3.71.1.4.1.3",},
{    "rsvpIfAttribEnabled",             "1.3.6.1.3.71.1.4.1.4",},
{    "rsvpIfAttribUdpRequired",         "1.3.6.1.3.71.1.4.1.5",},
{    "rsvpIfAttribStatus",              "1.3.6.1.3.71.1.4.1.6",},
{    "rsvpNbrTable",                    "1.3.6.1.3.71.1.5",},
{    "rsvpNbrEntry",                    "1.3.6.1.3.71.1.5.1",},
{    "rsvpNbrAddress",                  "1.3.6.1.3.71.1.5.1.1",},
{    "rsvpNbrProtocol",                 "1.3.6.1.3.71.1.5.1.2",},
{    "rsvpNbrStatus",                   "1.3.6.1.3.71.1.5.1.3",},
{    "rsvpFlowTable",                   "1.3.6.1.3.71.1.6",},
{    "rsvpFlowEntry",                   "1.3.6.1.3.71.1.6.1",},
{    "rsvpFlowIndex",                   "1.3.6.1.3.71.1.6.1.1",},
{    "rsvpFlowRate",                    "1.3.6.1.3.71.1.6.1.2",},
{    "rsvpFlowBurst",                   "1.3.6.1.3.71.1.6.1.3",},
{    "rsvpFlowWeight",                  "1.3.6.1.3.71.1.6.1.4",},
{    "rsvpFlowQueue",                   "1.3.6.1.3.71.1.6.1.5",},
{    "rsvpFlowMinTU",                   "1.3.6.1.3.71.1.6.1.6",},
{    "rsvpFlowStatus",                  "1.3.6.1.3.71.1.6.1.7",},
{    "rsvpSessionTable",                "1.3.6.1.3.71.1.7",},
{    "rsvpSessionEntry",                "1.3.6.1.3.71.1.7.1",},
{    "rsvpSessionAddress",              "1.3.6.1.3.71.1.7.1.1",},
{    "rsvpSessionProtocol",             "1.3.6.1.3.71.1.7.1.2",},
{    "rsvpSessionPort",                 "1.3.6.1.3.71.1.7.1.3",},
{    "rsvpSessionSenders",              "1.3.6.1.3.71.1.7.1.4",},
{    "rsvpSessionReceivers",            "1.3.6.1.3.71.1.7.1.5",},
{    "rsvpSessionRequests",             "1.3.6.1.3.71.1.7.1.6",},
{    "rsvpNotifications",               "1.3.6.1.3.71.2",},
{    "newFlow",                         "1.3.6.1.3.71.2.1",},
{    "lostFlow",                        "1.3.6.1.3.71.2.2",},
{    "rsvpConformance",                 "1.3.6.1.3.71.3",},
{    "rsvpGroups",                      "1.3.6.1.3.71.3.1",},
{    "rsvpSessionGroup",                "1.3.6.1.3.71.3.1.1",},
{    "rsvpSenderGroup",                 "1.3.6.1.3.71.3.1.2",},
{    "rsvpResvGroup",                   "1.3.6.1.3.71.3.1.3",},
{    "rsvpResvFwdGroup",                "1.3.6.1.3.71.3.1.4",},
{    "rsvpFlowGroup",                   "1.3.6.1.3.71.3.1.5",},
{    "rsvpIfAttribGroup",               "1.3.6.1.3.71.3.1.6",},
{    "rsvpNbrGroup",                    "1.3.6.1.3.71.3.1.7",},
{    "rsvpCompliances",                 "1.3.6.1.3.71.3.2",},
{    "rsvpCompliance",                  "1.3.6.1.3.71.3.2.1",},
{    0, 0},
};



/* Don't put anything after this #endif */ 
#endif /* _RSVP_MIBMIB_H_ */
