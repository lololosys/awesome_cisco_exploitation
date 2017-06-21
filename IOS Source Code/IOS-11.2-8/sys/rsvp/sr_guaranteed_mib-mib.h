/* $Id: sr_guaranteed_mib-mib.h,v 1.1.2.1 1996/04/18 10:48:52 fred Exp $
 * $Source: /release/112/cvs/Xsys/rsvp/sr_guaranteed_mib-mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1996, Fred Baker (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_guaranteed_mib-mib.h,v $
 * Revision 1.1.2.1  1996/04/18  10:48:52  fred
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

#ifndef _GUARANTEED_MIBMIB_H_
#define _GUARANTEED_MIBMIB_H_

/* SNMP-MIB translation table.  Subitem on left gets
 * translated into subitem on right.
 */

static const struct MIB_OID guaranteed_mib_oid_table[] =
{
{    "intSrv",                          "1.3.6.1.3.72",},
{    "intSrvGuaranteed",                "1.3.6.1.3.72.4",},
{    "intSrvGuaranteedObjects",         "1.3.6.1.3.72.4.1",},
{    "intSrvGuaranteedIfTable",         "1.3.6.1.3.72.4.1.1",},
{    "intSrvGuaranteedIfEntry",         "1.3.6.1.3.72.4.1.1.1",},
{    "intSrvGuaranteedIfC",             "1.3.6.1.3.72.4.1.1.1.1",},
{    "intSrvGuaranteedIfD",             "1.3.6.1.3.72.4.1.1.1.2",},
{    "intSrvGuaranteedIfSlack",         "1.3.6.1.3.72.4.1.1.1.3",},
{    "intSrvGuaranteedIfStatus",        "1.3.6.1.3.72.4.1.1.1.4",},
{    "intSrvGuaranteedNotifications",   "1.3.6.1.3.72.4.2",},
{    "intSrvGuaranteedConformance",     "1.3.6.1.3.72.4.3",},
{    "intSrvGuaranteedGroups",          "1.3.6.1.3.72.4.3.1",},
{    "intSrvGuaranteedIfAttribGroup",   "1.3.6.1.3.72.4.3.1.2",},
{    "intSrvGuaranteedCompliances",     "1.3.6.1.3.72.4.3.2",},
{    "intSrvGuaranteedCompliance",      "1.3.6.1.3.72.4.3.2.1",},
{    0, 0},
};



/* Don't put anything after this #endif */ 
#endif /* _GUARANTEED_MIBMIB_H_ */
