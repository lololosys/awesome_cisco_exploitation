/* $Id: sr_callmib.h,v 3.3.12.1 1996/04/12 02:29:48 bdas Exp $
 * $Source: /release/112/cvs/Xsys/isdn/sr_callmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * January 1995, Bibek A Das (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_callmib.h,v $
 * Revision 3.3.12.1  1996/04/12  02:29:48  bdas
 * CSCdi53500:  SNMP output byte counts differ ISDN Primary Rate interface
 * stats
 * Branch: California_branch
 *
 * Revision 3.3  1996/02/12  19:23:34  bdas
 * CSCdi48372:  ciscoCallHistoryRetainTimer need not be started for active
 * calls
 *
 * Revision 3.2  1995/11/17  17:42:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:15:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:17:33  hampton
 * Bump version numbers from 1.x to 2.x.
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

#ifndef _callmib_h_
#define _callmib_h_
#include "../snmp/snmp_api.h"
#include "sr_callmibdefs.h"
#include "sr_callmibsupp.h"
#include "sr_callmibpart.h"
#include "sr_callmibtype.h"
#include "tune.h"

#define FIRST_HISTORY_INDEX 1   /* Index of the History Entry */
#define HISTORY_TABLE_LENGTH_MAX 500   /* maximum at anytime */
#if defined(RP1) || defined(RSP)        /* the 7xxx platforms with PRI */
#define HISTORY_TABLE_LENGTH_DEFAULT (32 * MAX_DSL)   /* default value */
#else
#define HISTORY_TABLE_LENGTH_DEFAULT 100   /* default value */
#endif
#define HISTORY_RETAIN_TIMER_MAX  500 
#define HISTORY_RETAIN_TIMER_DEFAULT 15 
#define MAX_HISTORY_INDEX  2147483647 /* wraps around */
#define MAX_HISTORY_TIME   ((unsigned long) 0xffffffff)
#define FIRST_DISC_CAUSE   0x90
#define LAST_DISC_CAUSE    0x96
#define DISC_CAUSE_OFFSET  0x8e
extern const ObjectInfo callmib_OidList[];
extern const uint       callmib_OidListNum;


#endif  /* _callmib_h_ */
      
