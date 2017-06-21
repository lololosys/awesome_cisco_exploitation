/* $Id: sr_cdpmib-mib.h,v 3.1.12.1 1996/07/11 17:19:39 tylin Exp $
 * $Source: /release/112/cvs/Xsys/cdp/sr_cdpmib-mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1996, Bob Stewart (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_cdpmib-mib.h,v $
 * Revision 3.1.12.1  1996/07/11  17:19:39  tylin
 * CSCdi59144:  setting cdpInterfaceEnable, cdpInterfaceMessageInterval =>
 * noSuchNam
 *    Obsolete wrongly defined object cdpInterfaceMessageInterval and
 *    define new
 *    object group cdpGlobal corresponds to command '[no] cdp run', 'cdp
 *    timer'
 *    and 'cdp holdtime'
 * Branch: California_branch
 *
 * Revision 3.1  1996/02/27  21:36:11  bstewart
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

#ifndef _CDPMIBMIB_H_
#define _CDPMIBMIB_H_

/* SNMP-MIB translation table.  Subitem on left gets
 * translated into subitem on right.
 */

static const struct MIB_OID cdpmib_oid_table[] =
{
{    "ciscoCdpMIB",                     "1.3.6.1.4.1.9.9.23",},
{    "cdpInterface",                    "1.3.6.1.4.1.9.9.23.1.1",},
{    "cdpInterfaceEntry",               "1.3.6.1.4.1.9.9.23.1.1.1.1",},
{    "cdpCache",                        "1.3.6.1.4.1.9.9.23.1.2",},
{    "cdpCacheEntry",                   "1.3.6.1.4.1.9.9.23.1.2.1.1",},
{    "cdpGlobal",                       "1.3.6.1.4.1.9.9.23.1.3",},
{    0, 0},
};



/* Don't put anything after this #endif */ 
#endif /* _CDPMIBMIB_H_ */
