/* $Id: sr_novell_nlsp-mib.h,v 3.1 1996/02/29 22:06:57 bstewart Exp $
 * $Source: /release/111/cvs/Xsys/xns/sr_novell_nlsp-mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1996, Bob Stewart (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_novell_nlsp-mib.h,v $
 * Revision 3.1  1996/02/29  22:06:57  bstewart
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

#ifndef _NOVELL_NLSPMIB_H_
#define _NOVELL_NLSPMIB_H_

/* SNMP-MIB translation table.  Subitem on left gets
 * translated into subitem on right.
 */

static const struct MIB_OID novell_nlsp_oid_table[] =
{
{    "nlsp",                            "1.3.6.1.4.1.23.2.19",},
{    "nlspSysEntry",                    "1.3.6.1.4.1.23.2.19.1.1.1",},
{    "nlspSysAreaEntry",                "1.3.6.1.4.1.23.2.19.1.2.1",},
{    "nlspActAreaEntry",                "1.3.6.1.4.1.23.2.19.1.3.1",},
{    "nlspCircuit",                     "1.3.6.1.4.1.23.2.19.2",},
{    "nlspCircEntry",                   "1.3.6.1.4.1.23.2.19.2.1.1",},
{    "nlspForwarding",                  "1.3.6.1.4.1.23.2.19.3",},
{    "nlspDestEntry",                   "1.3.6.1.4.1.23.2.19.3.1.1",},
{    "nlspNeighbors",                   "1.3.6.1.4.1.23.2.19.4",},
{    "nlspNeighEntry",                  "1.3.6.1.4.1.23.2.19.4.1.1",},
{    "nlspTranslation",                 "1.3.6.1.4.1.23.2.19.5",},
{    "nlspIDMapEntry",                  "1.3.6.1.4.1.23.2.19.5.1.1",},
{    "nlspNetMapEntry",                 "1.3.6.1.4.1.23.2.19.5.2.1",},
{    "nlspNameMapEntry",                "1.3.6.1.4.1.23.2.19.5.3.1",},
{    "nlspGraph",                       "1.3.6.1.4.1.23.2.19.6",},
{    "nlspNodeEntry",                   "1.3.6.1.4.1.23.2.19.6.1.1",},
{    "nlspLinkEntry",                   "1.3.6.1.4.1.23.2.19.6.2.1",},
{    "nlspPathEntry",                   "1.3.6.1.4.1.23.2.19.6.3.1",},
{    "nlspGraphXRouteEntry",            "1.3.6.1.4.1.23.2.19.6.4.1",},
{    "nlspGraphServEntry",              "1.3.6.1.4.1.23.2.19.6.5.1",},
{    "nlspLSP",                         "1.3.6.1.4.1.23.2.19.7",},
{    "nlspLSPEntry",                    "1.3.6.1.4.1.23.2.19.7.1.1",},
{    "nlspLSPOptEntry",                 "1.3.6.1.4.1.23.2.19.7.2.1",},
{    0, 0},
};



/* Don't put anything after this #endif */ 
#endif /* _NOVELL_NLSPMIB_H_ */
