/* $Id: sr_ds1mib-mib.h,v 3.2 1996/02/09 16:04:22 bstewart Exp $
 * $Source: /release/111/cvs/Xsys/hes/sr_ds1mib-mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1996, Jeffrey T. Johnson (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_ds1mib-mib.h,v $
 * Revision 3.2  1996/02/09  16:04:22  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 * Retroactively decided not to keep any xxxTable registration points
 *
 * Revision 3.1  1996/02/02  02:55:00  jjohnson
 * CSCdi47892:  SNMP oid translation table needs to be modularized
 * modularize the envmon and ds1 mib translation tables
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

#ifndef _DS1MIBMIB_H_
#define _DS1MIBMIB_H_

/* SNMP-MIB translation table.  Subitem on left gets
 * translated into subitem on right.
 */

static const struct MIB_OID ds1mib_oid_table[] =
{
{    "ds1",                             "1.3.6.1.2.1.10.18",},
{    "dsx1ConfigEntry",                 "1.3.6.1.2.1.10.18.6.1",},
{    "dsx1CurrentEntry",                "1.3.6.1.2.1.10.18.7.1",},
{    "dsx1IntervalEntry",               "1.3.6.1.2.1.10.18.8.1",},
{    "dsx1TotalEntry",                  "1.3.6.1.2.1.10.18.9.1",},
{    "dsx1FracEntry",                   "1.3.6.1.2.1.10.18.13.1",},
{    0, 0},
};



/* Don't put anything after this #endif */ 
#endif /* _DS1MIBMIB_H_ */
