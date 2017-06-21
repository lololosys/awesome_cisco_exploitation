/* $Id: sr_novell_ripsap-mib.h,v 3.1 1996/02/29 22:21:13 bstewart Exp $
 * $Source: /release/111/cvs/Xsys/xns/sr_novell_ripsap-mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1996, Bob Stewart (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_novell_ripsap-mib.h,v $
 * Revision 3.1  1996/02/29  22:21:13  bstewart
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

#ifndef _NOVELL_RIPSAPMIB_H_
#define _NOVELL_RIPSAPMIB_H_

/* SNMP-MIB translation table.  Subitem on left gets
 * translated into subitem on right.
 */

static const struct MIB_OID novell_ripsap_oid_table[] =
{
{    "ripsap",                          "1.3.6.1.4.1.23.2.20",},
{    "ripsapSystem",                    "1.3.6.1.4.1.23.2.20.1",},
{    "ripSysEntry",                     "1.3.6.1.4.1.23.2.20.1.1.1",},
{    "sapSysEntry",                     "1.3.6.1.4.1.23.2.20.1.2.1",},
{    "ripsapCircuit",                   "1.3.6.1.4.1.23.2.20.2",},
{    "ripCircEntry",                    "1.3.6.1.4.1.23.2.20.2.1.1",},
{    "sapCircEntry",                    "1.3.6.1.4.1.23.2.20.2.2.1",},
{    0, 0},
};



/* Don't put anything after this #endif */ 
#endif /* _NOVELL_RIPSAPMIB_H_ */
