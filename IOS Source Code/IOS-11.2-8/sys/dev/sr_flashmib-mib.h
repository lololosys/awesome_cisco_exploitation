/* $Id: sr_flashmib-mib.h,v 3.1 1996/02/12 19:29:29 bstewart Exp $
 * $Source: /release/111/cvs/Xsys/dev/sr_flashmib-mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1996, Bob Stewart (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_flashmib-mib.h,v $
 * Revision 3.1  1996/02/12  19:29:29  bstewart
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

#ifndef _FLASHMIBMIB_H_
#define _FLASHMIBMIB_H_

/* SNMP-MIB translation table.  Subitem on left gets
 * translated into subitem on right.
 */

static const struct MIB_OID flashmib_oid_table[] =
{
{    "ciscoFlashMIB",                   "1.3.6.1.4.1.9.9.10",},
{    "ciscoFlashDevice",                "1.3.6.1.4.1.9.9.10.1.1",},
{    "ciscoFlashDeviceEntry",           "1.3.6.1.4.1.9.9.10.1.1.2.1",},
{    "ciscoFlashChips",                 "1.3.6.1.4.1.9.9.10.1.1.3",},
{    "ciscoFlashChipEntry",             "1.3.6.1.4.1.9.9.10.1.1.3.1.1",},
{    "ciscoFlashPartitions",            "1.3.6.1.4.1.9.9.10.1.1.4",},
{    "ciscoFlashPartitionEntry",        "1.3.6.1.4.1.9.9.10.1.1.4.1.1",},
{    "ciscoFlashFiles",                 "1.3.6.1.4.1.9.9.10.1.1.4.2",},
{    "ciscoFlashFileEntry",             "1.3.6.1.4.1.9.9.10.1.1.4.2.1.1",},
{    "ciscoFlashOps",                   "1.3.6.1.4.1.9.9.10.1.2",},
{    "ciscoFlashCopyEntry",             "1.3.6.1.4.1.9.9.10.1.2.1.1",},
{    "ciscoFlashPartitioningEntry",     "1.3.6.1.4.1.9.9.10.1.2.2.1",},
{    "ciscoFlashMiscOpEntry",           "1.3.6.1.4.1.9.9.10.1.2.3.1",},
{    "ciscoFlashMIBTrapPrefix",         "1.3.6.1.4.1.9.9.10.1.3",},
{    "ciscoFlashMIBTraps",              "1.3.6.1.4.1.9.9.10.1.3.0",},
{    "ciscoFlashCopyCompletionTrap",    "1.3.6.1.4.1.9.9.10.1.3.0.1",},
{    "ciscoFlashPartitioningCompletionTrap", "1.3.6.1.4.1.9.9.10.1.3.0.2",},
{    "ciscoFlashMiscOpCompletionTrap",  "1.3.6.1.4.1.9.9.10.1.3.0.3",},
{    "ciscoFlashDeviceChangeTrap",      "1.3.6.1.4.1.9.9.10.1.3.0.4",},
{    0, 0},
};



/* Don't put anything after this #endif */ 
#endif /* _FLASHMIBMIB_H_ */
