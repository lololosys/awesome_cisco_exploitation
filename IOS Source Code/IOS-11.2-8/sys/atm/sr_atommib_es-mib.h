/* $Id: sr_atommib_es-mib.h,v 3.1.4.1 1996/04/25 23:02:58 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/atm/sr_atommib_es-mib.h,v $
 *------------------------------------------------------------------
 * ATOMMIB support for router products.
 *
 * April 1996, Andrew Wu (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_atommib_es-mib.h,v $
 * Revision 3.1.4.1  1996/04/25  23:02:58  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/10  09:00:29  awu
 * Placeholder file.
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

#ifndef _ATOMMIB_ESMIB_H_
#define _ATOMMIB_ESMIB_H_

/* SNMP-MIB translation table.  Subitem on left gets
 * translated into subitem on right.
 */

static const struct MIB_OID atommib_es_oid_table[] =
{
{    "atmInterfaceConfTable",           "1.3.6.1.2.1.37.1.2",},
{    "atmInterfaceConfEntry",           "1.3.6.1.2.1.37.1.2.1",},
{    "atmTrafficDescrParamTable",       "1.3.6.1.2.1.37.1.5",},
{    "atmTrafficDescrParamEntry",       "1.3.6.1.2.1.37.1.5.1",},
{    "atmVclTable",                     "1.3.6.1.2.1.37.1.7",},
{    "atmVclEntry",                     "1.3.6.1.2.1.37.1.7.1",},
{    0, 0},
};

/* Don't put anything after this #endif */ 
#endif /* _ATOMMIB_ESMIB_H_ */
