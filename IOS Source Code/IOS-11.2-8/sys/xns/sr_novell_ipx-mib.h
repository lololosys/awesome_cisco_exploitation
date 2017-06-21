/* $Id: sr_novell_ipx-mib.h,v 3.1 1996/02/29 21:45:01 bstewart Exp $
 * $Source: /release/111/cvs/Xsys/xns/sr_novell_ipx-mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1996, Bob Stewart (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_novell_ipx-mib.h,v $
 * Revision 3.1  1996/02/29  21:45:01  bstewart
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

#ifndef _NOVELL_IPXMIB_H_
#define _NOVELL_IPXMIB_H_

/* SNMP-MIB translation table.  Subitem on left gets
 * translated into subitem on right.
 */

static const struct MIB_OID novell_ipx_oid_table[] =
{
{    "ipx",                             "1.3.6.1.4.1.23.2.5",},
{    "ipxSystem",                       "1.3.6.1.4.1.23.2.5.1",},
{    "ipxBasicSysEntry",                "1.3.6.1.4.1.23.2.5.1.1.1",},
{    "ipxAdvSysEntry",                  "1.3.6.1.4.1.23.2.5.1.2.1",},
{    "ipxCircuit",                      "1.3.6.1.4.1.23.2.5.2",},
{    "ipxCircEntry",                    "1.3.6.1.4.1.23.2.5.2.1.1",},
{    "ipxForwarding",                   "1.3.6.1.4.1.23.2.5.3",},
{    "ipxDestEntry",                    "1.3.6.1.4.1.23.2.5.3.1.1",},
{    "ipxStaticRouteEntry",             "1.3.6.1.4.1.23.2.5.3.2.1",},
{    "ipxServices",                     "1.3.6.1.4.1.23.2.5.4",},
{    "ipxServEntry",                    "1.3.6.1.4.1.23.2.5.4.1.1",},
{    "ipxDestServEntry",                "1.3.6.1.4.1.23.2.5.4.2.1",},
{    "ipxStaticServEntry",              "1.3.6.1.4.1.23.2.5.4.3.1",},
{    0, 0},
};



/* Don't put anything after this #endif */ 
#endif /* _NOVELL_IPXMIB_H_ */
