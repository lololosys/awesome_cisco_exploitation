/* $Id: sr_rfc1243-mib.h,v 3.1 1996/02/23 20:33:40 bstewart Exp $
 * $Source: /release/111/cvs/Xsys/atalk/sr_rfc1243-mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1996, Bob Stewart (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_rfc1243-mib.h,v $
 * Revision 3.1  1996/02/23  20:33:40  bstewart
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

#ifndef _RFC1243MIB_H_
#define _RFC1243MIB_H_

/* SNMP-MIB translation table.  Subitem on left gets
 * translated into subitem on right.
 */

static const struct MIB_OID rfc1243_oid_table[] =
{
{    "appletalk",                       "1.3.6.1.2.1.13",},
{    "llap",                            "1.3.6.1.2.1.13.1",},
{    "llapEntry",                       "1.3.6.1.2.1.13.1.1.1",},
{    "aarp",                            "1.3.6.1.2.1.13.2",},
{    "aarpEntry",                       "1.3.6.1.2.1.13.2.1.1",},
{    "atport",                          "1.3.6.1.2.1.13.3",},
{    "atportEntry",                     "1.3.6.1.2.1.13.3.1.1",},
{    "ddp",                             "1.3.6.1.2.1.13.4",},
{    "rtmp",                            "1.3.6.1.2.1.13.5",},
{    "rtmpEntry",                       "1.3.6.1.2.1.13.5.1.1",},
{    "kip",                             "1.3.6.1.2.1.13.6",},
{    "kipEntry",                        "1.3.6.1.2.1.13.6.1.1",},
{    "zip",                             "1.3.6.1.2.1.13.7",},
{    "zipEntry",                        "1.3.6.1.2.1.13.7.1.1",},
{    "nbp",                             "1.3.6.1.2.1.13.8",},
{    "nbpEntry",                        "1.3.6.1.2.1.13.8.1.1",},
{    "atecho",                          "1.3.6.1.2.1.13.9",},
{    0, 0},
};



/* Don't put anything after this #endif */ 
#endif /* _RFC1243MIB_H_ */
