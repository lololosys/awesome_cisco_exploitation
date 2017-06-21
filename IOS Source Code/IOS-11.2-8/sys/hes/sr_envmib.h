/* $Id: sr_envmib.h,v 3.4 1995/12/01 19:03:40 sdurham Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/sr_envmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * October 1994, Sandra Durham (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_envmib.h,v $
 * Revision 3.4  1995/12/01  19:03:40  sdurham
 * CSCdi44362:  envmon is non-modular w.r.t envmon mib.
 *   Remove direct calls from envmon code to mib code. Use service points.
 *
 * Revision 3.3  1995/11/22  23:42:00  sdurham
 * CSCdi35282:  Need command to configure envmon trap enables
 *      add individual trap options in separate parser-mib file
 *      envmon_mib_chain.
 *
 * Revision 3.2  1995/11/17  09:20:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:42:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:24:54  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:44:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 * Copyright (C) 1995 by SNMP Research, Incorporated.
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

#ifndef _envmib_h_
#define _envmib_h_

#include "../snmp/snmp_api.h"
#include "sr_envmibdefs.h"
#include "sr_envmibsupp.h"

/* force "ciscoEnvMonShutdownNotification" OID to be generated
   for use by trap generator */
#define N_ciscoEnvMonShutdownNotification

/* force "ciscoEnvMonVoltageNotification" OID to be generated
   for use by trap generator */
#define N_ciscoEnvMonVoltageNotification

/* force "ciscoEnvMonTemperatureNotification" OID to be generated
   for use by trap generator */
#define N_ciscoEnvMonTemperatureNotification

/* force "ciscoEnvMonFanNotification" OID to be generated
   for use by trap generator */
#define N_ciscoEnvMonFanNotification

/* force "ciscoEnvMonRedundantSupplyNotification" OID to be generated
   for use by trap generator */
#define N_ciscoEnvMonRedundantSupplyNotification

/* Individual traps for envmon */
enum { 
    TRAP_ENVMON_SHUTDOWN,
    TRAP_ENVMON_VOLT,
    TRAP_ENVMON_TEMP,
    TRAP_ENVMON_FAN,
    TRAP_ENVMON_SUPPLY
};

#include "sr_envmibpart.h"
#include "sr_envmibtype.h"


extern const ObjectInfo envmib_OidList[];
extern const uint       envmib_OidListNum;
extern ciscoEnvMonMIBNotificationEnables_t envmon_globalsData;



/* 
 *  function prototypes
 *
 */
extern void init_envmib(subsystype *);

extern int getEnvTempToSnmpIndex(int);
extern int getEnvVoltToSnmpIndex(int);

/* 
 *  function prototypes - 7000 & RSP only
 */

extern void envmib_platform_specific_init(void);
extern int getEnvFanToSnmpIndex(int);

extern void envmon_mib_parser_init(void);

#endif  /* _envmib_h_ */

      
