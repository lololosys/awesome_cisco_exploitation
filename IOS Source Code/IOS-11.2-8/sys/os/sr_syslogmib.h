/* $Id: sr_syslogmib.h,v 3.1.48.1 1996/05/21 06:36:41 mordock Exp $
 * $Source: /release/112/cvs/Xsys/os/sr_syslogmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * August 1995, Anke Dosedal (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_syslogmib.h,v $
 * Revision 3.1.48.1  1996/05/21  06:36:41  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.1  1996/01/19  03:24:22  anke
 * Placeholder for Syslog MIB
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

#ifndef _syslogmib_h_
#define _syslogmib_h_

#define N_clogMessageGenerated

#include "../snmp/snmp_api.h"
#include "sr_syslogmibdefs.h"
#include "sr_syslogmibsupp.h"
#include "sr_syslogmibpart.h"
#include "sr_syslogmibtype.h"

extern const ObjectInfo syslogmib_OidList[];
extern const uint       syslogmib_OidListNum;
extern unsigned long syslog_table_size; /* user defined history table size */
extern unsigned long new_table_size;
extern boolean  syslog_notification_enabled; /* notification enabled flag */
extern unsigned long max_severity;    /* user defined maximum severity level */

extern void syslogmib_parser_init (void);

#endif  /* _syslogmib_h_ */
      
