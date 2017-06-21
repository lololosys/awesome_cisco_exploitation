/* $Id: sr_mempoolmib-mib.h,v 3.2 1996/02/20 06:15:32 jjohnson Exp $
 * $Source: /release/111/cvs/Xsys/os/sr_mempoolmib-mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1996, Jeffrey T. Johnson (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_mempoolmib-mib.h,v $
 * Revision 3.2  1996/02/20  06:15:32  jjohnson
 * CSCdi49356:  Need MIB variable for total DRAM
 * Fully instrument the memory pools
 *
 * Revision 3.1  1996/02/13  19:35:20  jjohnson
 * Initial placeholder
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

#ifndef _MEMPOOLMIBMIB_H_
#define _MEMPOOLMIBMIB_H_

/* SNMP-MIB translation table.  Subitem on left gets
 * translated into subitem on right.
 */

static const struct MIB_OID mempoolmib_oid_table[] =
{
{    "ciscoMemoryPoolMIB",              "1.3.6.1.4.1.9.9.48",},
{    "ciscoMemoryPoolEntry",            "1.3.6.1.4.1.9.9.48.1.1.1",},
{    0, 0},
};



/* Don't put anything after this #endif */ 
#endif /* _MEMPOOLMIBMIB_H_ */
