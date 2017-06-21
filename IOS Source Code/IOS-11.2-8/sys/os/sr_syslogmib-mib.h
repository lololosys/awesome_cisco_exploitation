/* $Id: sr_syslogmib-mib.h,v 3.1.2.1 1996/05/21 06:36:40 mordock Exp $
 * $Source: /release/112/cvs/Xsys/os/sr_syslogmib-mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1996, Anke Dosedal (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_syslogmib-mib.h,v $
 * Revision 3.1.2.1  1996/05/21  06:36:40  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.1  1996/04/18  21:52:49  anke
 * initial placeholder
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

#ifndef _SYSLOGMIBMIB_H_
#define _SYSLOGMIBMIB_H_

/* SNMP-MIB translation table.  Subitem on left gets
 * translated into subitem on right.
 */

static const struct MIB_OID syslogmib_oid_table[] =
{
{    "ciscoSyslogMIB",                  "1.3.6.1.4.1.9.9.41",},
{    "clogBasic",                       "1.3.6.1.4.1.9.9.41.1.1",},
{    "clogHistoryEntry",                "1.3.6.1.4.1.9.9.41.1.2.3.1",},
{    0, 0},
};



/* Don't put anything after this #endif */ 
#endif /* _SYSLOGMIBMIB_H_ */
