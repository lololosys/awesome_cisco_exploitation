/* $Id: sr_cdlswmib-mib.h,v 3.1 1996/02/28 17:45:13 bstewart Exp $
 * $Source: /release/111/cvs/Xsys/dlsw/sr_cdlswmib-mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1996, Bob Stewart (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_cdlswmib-mib.h,v $
 * Revision 3.1  1996/02/28  17:45:13  bstewart
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

#ifndef _CDLSWMIBMIB_H_
#define _CDLSWMIBMIB_H_

/* SNMP-MIB translation table.  Subitem on left gets
 * translated into subitem on right.
 */

static const struct MIB_OID cdlswmib_oid_table[] =
{
{    "ciscoDlswNode",                   "1.3.6.1.4.1.9.10.9.1.1",},
{    "ciscoDlswTrapControl",            "1.3.6.1.4.1.9.10.9.1.1.10",},
{    "ciscoDlswTConn",                  "1.3.6.1.4.1.9.10.9.1.2",},
{    "ciscoDlswTConnStat",              "1.3.6.1.4.1.9.10.9.1.2.1",},
{    "ciscoDlswTConnConfigEntry",       "1.3.6.1.4.1.9.10.9.1.2.2.1",},
{    "ciscoDlswTConnOperEntry",         "1.3.6.1.4.1.9.10.9.1.2.3.1",},
{    "ciscoDlswTConnSpecific",          "1.3.6.1.4.1.9.10.9.1.2.4",},
{    "ciscoDlswTConnTcp",               "1.3.6.1.4.1.9.10.9.1.2.4.1",},
{    "ciscoDlswTConnTcpConfigEntry",    "1.3.6.1.4.1.9.10.9.1.2.4.1.1.1",},
{    "ciscoDlswTConnTcpOperEntry",      "1.3.6.1.4.1.9.10.9.1.2.4.1.2.1",},
{    "ciscoDlswInterface",              "1.3.6.1.4.1.9.10.9.1.3",},
{    "ciscoDlswIfEntry",                "1.3.6.1.4.1.9.10.9.1.3.1.1",},
{    "ciscoDlswCircuit",                "1.3.6.1.4.1.9.10.9.1.5",},
{    "ciscoDlswCircuitStat",            "1.3.6.1.4.1.9.10.9.1.5.1",},
{    "ciscoDlswCircuitEntry",           "1.3.6.1.4.1.9.10.9.1.5.2.1",},
{    0, 0},
};



/* Don't put anything after this #endif */ 
#endif /* _CDLSWMIBMIB_H_ */
