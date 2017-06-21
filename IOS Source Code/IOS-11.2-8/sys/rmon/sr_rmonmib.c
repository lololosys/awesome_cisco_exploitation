/* $Id: sr_rmonmib.c,v 3.4.18.1 1996/03/26 21:21:34 jjohnson Exp $
 * $Source: /release/111/cvs/Xsys/rmon/sr_rmonmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * August 1995, Jeffrey T. Johnson (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_rmonmib.c,v $
 * Revision 3.4.18.1  1996/03/26  21:21:34  jjohnson
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 * Finish bstewart's commits which didn't make the branch pull
 * Branch: California_branch
 *
 * Revision 3.4  1996/02/09  07:57:31  jjohnson
 * CSCdi48524:  Need mechanism to display RMON tables
 * parse chains are in place.  checkpoint work so that anke can take over.
 *
 * Revision 3.3  1996/01/19  00:23:44  jjohnson
 * CSCdi45754:  snmp subsystem should not contain non-snmp mibs
 * the snmp timer task is now part of the snmp subsystem
 *
 * Revision 3.2  1995/11/17  18:39:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:31  shaker
 * Bump version numbers from 2.x to 3.x.
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

#include "master.h"
#include "subsys.h"
#include "sr_rmonmib.h"
#include "sr_rmonmib-mib.h"

/*
 * all the method routines are supplied in separate files by SNMP Research
 */

void
init_rmonmib (subsystype *subsys)
{
    load_mib(rmonmib_OidList, rmonmib_OidListNum);
    load_oid(rmonmib_oid_table);

    k_etherStatsEntry_initialize();
    k_historyControlEntry_initialize();
    k_hostControlEntry_initialize();
    k_hostTopNControlEntry_initialize();
    k_matrixControlEntry_initialize();
    k_filterEntry_initialize();
    k_channelEntry_initialize();
    k_bufferControlEntry_initialize();

}

/*
 * rmonmib subsystem header
 */

#define MAJVERSION_rmonmib 1
#define MINVERSION_rmonmib 0
#define EDITVERSION_rmonmib 0

SUBSYS_HEADER(rmonmib,
              MAJVERSION_rmonmib,
              MINVERSION_rmonmib,
              EDITVERSION_rmonmib,
              init_rmonmib,
              SUBSYS_CLASS_MANAGEMENT,
              "req: rmonlite",
              "seq: rmonlite");
