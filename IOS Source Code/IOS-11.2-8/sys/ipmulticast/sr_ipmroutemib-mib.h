/* $Id: sr_ipmroutemib-mib.h,v 3.1 1996/02/29 21:00:36 bstewart Exp $
 * $Source: /release/111/cvs/Xsys/ipmulticast/sr_ipmroutemib-mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1996, Bob Stewart (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_ipmroutemib-mib.h,v $
 * Revision 3.1  1996/02/29  21:00:36  bstewart
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

#ifndef _IPMROUTEMIBMIB_H_
#define _IPMROUTEMIBMIB_H_

/* SNMP-MIB translation table.  Subitem on left gets
 * translated into subitem on right.
 */

static const struct MIB_OID ipmroutemib_oid_table[] =
{
{    "ipMRoute",                        "1.3.6.1.3.60.1.1",},
{    "ipMRouteEntry",                   "1.3.6.1.3.60.1.1.2.1",},
{    "ipMRouteNextHopEntry",            "1.3.6.1.3.60.1.1.3.1",},
{    "ipMRouteInterfaceEntry",          "1.3.6.1.3.60.1.1.4.1",},
{    "ciscoIpMRoute",                   "1.3.6.1.4.1.9.10.2.1.1",},
{    "ciscoIpMRouteEntry",              "1.3.6.1.4.1.9.10.2.1.1.2.1",},
{    "ciscoIpMRouteNextHopEntry",       "1.3.6.1.4.1.9.10.2.1.1.3.1",},
{    0, 0},
};



/* Don't put anything after this #endif */ 
#endif /* _IPMROUTEMIBMIB_H_ */
