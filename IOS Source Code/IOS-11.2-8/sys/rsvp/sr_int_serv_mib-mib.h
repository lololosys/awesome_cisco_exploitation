/* $Id: sr_int_serv_mib-mib.h,v 1.1.2.1 1996/04/18 10:48:56 fred Exp $
 * $Source: /release/112/cvs/Xsys/rsvp/sr_int_serv_mib-mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1996, Fred Baker (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_int_serv_mib-mib.h,v $
 * Revision 1.1.2.1  1996/04/18  10:48:56  fred
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

#ifndef _INT_SERV_MIBMIB_H_
#define _INT_SERV_MIBMIB_H_

/* SNMP-MIB translation table.  Subitem on left gets
 * translated into subitem on right.
 */

static const struct MIB_OID int_serv_mib_oid_table[] =
{
{    "experimental",                    "1.3.6.1.3",},
{    "intSrv",                          "1.3.6.1.3.72",},
{    "intSrvObjects",                   "1.3.6.1.3.72.1",},
{    "intSrvIfAttribTable",             "1.3.6.1.3.72.1.2",},
{    "intSrvIfAttribEntry",             "1.3.6.1.3.72.1.2.1",},
{    "intSrvIfAttribAllocatedBits",     "1.3.6.1.3.72.1.2.1.1",},
{    "intSrvIfAttribMaxAllocatedBits",  "1.3.6.1.3.72.1.2.1.2",},
{    "intSrvIfAttribAllocatedBuffer",   "1.3.6.1.3.72.1.2.1.3",},
{    "intSrvIfAttribFlows",             "1.3.6.1.3.72.1.2.1.4",},
{    "intSrvIfAttribPropagationDelay",  "1.3.6.1.3.72.1.2.1.5",},
{    "intSrvIfAttribStatus",            "1.3.6.1.3.72.1.2.1.6",},
{    "intSrvNotifications",             "1.3.6.1.3.72.2",},
{    "intSrvConformance",               "1.3.6.1.3.72.3",},
{    "intSrvGroups",                    "1.3.6.1.3.72.3.1",},
{    "intSrvIfAttribGroup",             "1.3.6.1.3.72.3.1.2",},
{    "intSrvCompliances",               "1.3.6.1.3.72.3.2",},
{    "intSrvCompliance",                "1.3.6.1.3.72.3.2.1",},
{    0, 0},
};



/* Don't put anything after this #endif */ 
#endif /* _INT_SERV_MIBMIB_H_ */
