/* $Id: sr_newfddimib-mib.h,v 3.1 1996/02/15 17:30:14 bstewart Exp $
 * $Source: /release/111/cvs/Xsys/xdi/sr_newfddimib-mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1996, Bob Stewart (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_newfddimib-mib.h,v $
 * Revision 3.1  1996/02/15  17:30:14  bstewart
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

#ifndef _NEWFDDIMIBMIB_H_
#define _NEWFDDIMIBMIB_H_

/* SNMP-MIB translation table.  Subitem on left gets
 * translated into subitem on right.
 */

static const struct MIB_OID newfddimib_oid_table[] =
{
{    "fddi",                            "1.3.6.1.2.1.10.15",},
{    "fddimib",                         "1.3.6.1.2.1.10.15.73",},
{    "fddimibSMT",                      "1.3.6.1.2.1.10.15.73.1",},
{    "fddimibSMTEntry",                 "1.3.6.1.2.1.10.15.73.1.2.1",},
{    "fddimibMAC",                      "1.3.6.1.2.1.10.15.73.2",},
{    "fddimibMACEntry",                 "1.3.6.1.2.1.10.15.73.2.2.1",},
{    "fddimibMACCounters",              "1.3.6.1.2.1.10.15.73.3",},
{    "fddimibMACCountersEntry",         "1.3.6.1.2.1.10.15.73.3.1.1",},
{    "fddimibPATH",                     "1.3.6.1.2.1.10.15.73.4",},
{    "fddimibPATHNumber",               "1.3.6.1.2.1.10.15.73.4.1",},
{    "fddimibPATHEntry",                "1.3.6.1.2.1.10.15.73.4.2.1",},
{    "fddimibPATHConfigEntry",          "1.3.6.1.2.1.10.15.73.4.3.1",},
{    "fddimibPORT",                     "1.3.6.1.2.1.10.15.73.5",},
{    "fddimibPORTNumber",               "1.3.6.1.2.1.10.15.73.5.1",},
{    "fddimibPORTEntry",                "1.3.6.1.2.1.10.15.73.5.2.1",},
{    0, 0},
};



/* Don't put anything after this #endif */ 
#endif /* _NEWFDDIMIBMIB_H_ */
