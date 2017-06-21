/* $Id: sr_rmonlitemib.c,v 3.4.18.1 1996/03/26 21:21:19 jjohnson Exp $
 * $Source: /release/111/cvs/Xsys/rmon/sr_rmonlitemib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * August 1995, Jeffrey T. Johnson (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_rmonlitemib.c,v $
 * Revision 3.4.18.1  1996/03/26  21:21:19  jjohnson
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 * Finish bstewart's commits which didn't make the branch pull
 * Branch: California_branch
 *
 * Revision 3.4  1996/02/09  07:57:31  jjohnson
 * CSCdi48524:  Need mechanism to display RMON tables
 * parse chains are in place.  checkpoint work so that anke can take over.
 *
 * Revision 3.3  1996/01/24  21:18:31  anke
 * CSCdi47169:  Remove snmp from mibs as a required subsystem
 *
 * Revision 3.2  1995/11/17  18:39:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:27  shaker
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
#include "../snmp/snmp_api.h"
#include "sr_rmonlitemib.h"
#include "sr_rmonlitemib-mib.h"
#include "rmon_registry.h"
#include "rmon_lite.h"

/*
 * all the method routines are supplied in separate files by SNMP Research
 */


static void
init_rmonlite (subsystype *subsys)
{
    load_mib(rmonlitemib_OidList, rmonlitemib_OidListNum);
    load_oid(rmonlitemib_oid_table);
    k_alarmEntry_initialize();
    k_eventEntry_initialize();
    rmon_lite_parser_init();
    init_rmon_registry();
}

/*
 * rmonlite subsystem header
 */


#define MAJVERSION_rmonlite 1
#define MINVERSION_rmonlite 0
#define EDITVERSION_rmonlite 0

SUBSYS_HEADER(rmonlite,
              MAJVERSION_rmonlite,
              MINVERSION_rmonlite,
              EDITVERSION_rmonlite,
              init_rmonlite,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              NULL);
      
