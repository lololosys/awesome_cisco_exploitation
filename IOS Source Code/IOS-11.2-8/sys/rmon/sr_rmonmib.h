/* $Id: sr_rmonmib.h,v 3.3 1996/01/18 01:14:18 jjohnson Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/rmon/sr_rmonmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * August 1995, Jeffrey T. Johnson (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_rmonmib.h,v $
 * Revision 3.3  1996/01/18  01:14:18  jjohnson
 * CSCdi47080:  Enable limited RMON packet capture
 *
 * Revision 3.2  1995/11/17  18:39:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:32  shaker
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

#ifndef _rmonmib_h_
#define _rmonmib_h_

/* allow rmon row creation */
#define alarmEntry_READ_CREATE
#define bufferControlEntry_READ_CREATE 
#define channelEntry_READ_CREATE 
#define etherStatsEntry_READ_CREATE 
#define eventEntry_READ_CREATE 
#define filterEntry_READ_CREATE 
#define historyControlEntry_READ_CREATE 
#define hostControlEntry_READ_CREATE 
#define hostTopNControlEntry_READ_CREATE 
#define matrixControlEntry_READ_CREATE 

#include "../snmp/snmp_api.h"
#include "sr_rmonmibdefs.h"
#include "sr_rmonmibsupp.h"
#include "sr_rmonmibuser.h"

/*
 * the following will prevent oid table entries for the events and
 * alarms groups from being created.  this is desirable because the
 * rmonlitemib subsystem defines the oid table entries for those
 * objects 
 */
#ifdef SNMPPART_DECL
#undef I_alarmIndex
#undef I_alarmInterval
#undef I_alarmVariable
#undef I_alarmSampleType
#undef I_alarmValue
#undef I_alarmStartupAlarm
#undef I_alarmRisingThreshold
#undef I_alarmFallingThreshold
#undef I_alarmRisingEventIndex
#undef I_alarmFallingEventIndex
#undef I_alarmOwner
#undef I_alarmStatus
#undef I_eventIndex
#undef I_eventDescription
#undef I_eventType
#undef I_eventCommunity
#undef I_eventLastTimeSent
#undef I_eventOwner
#undef I_eventStatus
#undef I_logEventIndex
#undef I_logIndex
#undef I_logTime
#undef I_logDescription
#endif /* SNMPPART_DECL */

#include "sr_rmonmibpart.h"
#include "sr_rmonmibtype.h"
#include "sr_rmon.h"

extern const ObjectInfo rmonmib_OidList[];
extern const uint       rmonmib_OidListNum;

#endif  /* _rmonmib_h_ */
      
