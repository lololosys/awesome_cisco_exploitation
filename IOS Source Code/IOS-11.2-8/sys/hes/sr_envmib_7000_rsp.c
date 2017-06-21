/* $Id: sr_envmib_7000_rsp.c,v 3.3 1995/12/01 19:03:41 sdurham Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/sr_envmib_7000_rsp.c,v $
 *------------------------------------------------------------------
 * ENVMON MIB code shared between 7000 and 7500 platforms
 *
 * February 1995, Carl Schaefer
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_envmib_7000_rsp.c,v $
 * Revision 3.3  1995/12/01  19:03:41  sdurham
 * CSCdi44362:  envmon is non-modular w.r.t envmon mib.
 *   Remove direct calls from envmon code to mib code. Use service points.
 *
 * Revision 3.2  1995/11/17  09:20:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:42:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:44:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../snmp/snmp_api.h"
#include "sr_envmib.h"
#include "envmib_registry.h"

/*
ciscoEnvMonFanNotification NOTIFICATION-TYPE
	OBJECTS	{
		ciscoEnvMonFanStatusDescr,
		ciscoEnvMonFanState
		}
	STATUS	current
	DESCRIPTION
		"A ciscoEnvMonFanNotification is sent if any fan in the 
		fan array (where extant) fails. Since such a Notification is
		usually generated before the shutdown state is reached,
		it can convey more data and has a better chance of being 
		sent than does the ciscoEnvMonShutdownNotification."
	::= { ciscoEnvMonMIBNotifications 4 }
*/



#undef  ENV_TRAP_NUMBER
#define ENV_TRAP_NUMBER 4

#undef  TRAP_VARBIND_COUNT 
#define TRAP_VARBIND_COUNT 2

static const OID envFanOID = {
    LNciscoEnvMonFanNotification,
    (ulong *)IDciscoEnvMonFanNotification
};

/*
 * The OID that identifies the SNMPv1 enterprise. This is identical to the
 * SNMPv2 Notification OID with the last two subidentifiers removed
 */
static const OID enterpriseFanOID = {
    LNciscoEnvMonFanNotification - 2, 
    (ulong *)IDciscoEnvMonFanNotification
};

static const OID envFan_varbinds[TRAP_VARBIND_COUNT] = {
    { LNciscoEnvMonFanStatusDescr, (ulong *)IDciscoEnvMonFanStatusDescr },
    { LNciscoEnvMonFanState, (ulong *)IDciscoEnvMonFanState },
};


static void
envmib_fan_trap (int indx)
{
    int		vbIndex;
    OID	       *vbList[TRAP_VARBIND_COUNT+1];
    OID         instanceOID;
    ulong       instance[1];
    char	trapoid_str[80];

    /*
     * For some unknown reason, SNMP Research made all the object
     * identifier parameters (OID *) *except* for the SNMPv2
     * Notification identifier which is (char *).  It is much easier for
     * us to use the mib-compiler generate OID structure, so
     * we'll just convert that into a string.
     */

    if (MakeDotFromOID((OID *)&envFanOID, trapoid_str) == -1) {
	return;
    }	

    /*
     * All the trap varbind objects have an instance described by
     * INDEX { ciscoEnvMonFanIndex }
     */
    instance[0] = getEnvFanToSnmpIndex(indx);
    instanceOID.oid_ptr = instance;
    instanceOID.length = 1;


    /*
     * don't do anything if envmon traps are disabled
     */
    if (envmon_globalsData.ciscoEnvMonEnableFanNotification ==
	D_ciscoEnvMonEnableFanNotification_false) {
	return;
    }

    /*
     * create the varbind list
     */
    for (vbIndex = 0; vbIndex < TRAP_VARBIND_COUNT; vbIndex++) {
        vbList[vbIndex] = CatOID((OID *) &envFan_varbinds[vbIndex],
					    &instanceOID);

    }
    vbList[vbIndex] = NULL;

    /*
     * tell snmp to do something
     */
    snmp_trap(ENTERPRISE_TRAP, ENV_TRAP_NUMBER, vbList,
		 (OID *)&enterpriseFanOID, trapoid_str);

    /*
     * free data allocated by the CatOID service
     */
    for (vbIndex = 0; vbIndex < TRAP_VARBIND_COUNT; vbIndex++) {
	FreeOID(vbList[vbIndex]);
    }

}


/*
ciscoEnvMonRedundantSupplyNotification NOTIFICATION-TYPE
	OBJECTS	{
		ciscoEnvMonSupplyStatusDescr,
		ciscoEnvMonSupplyState
		}
	STATUS	current
	DESCRIPTION
		"A ciscoEnvMonRedundantSupplyNotification is sent if
		the redundant power supply (where extant) fails. 
		Since such a notification is usually generated before 
		the shutdown state is reached, it can convey more 
		data and has a better chance of being sent
		than does the ciscoEnvMonShutdownNotification."
	::= { ciscoEnvMonMIBNotifications 5 }
*/



#undef  ENV_TRAP_NUMBER
#define ENV_TRAP_NUMBER 5

#undef  TRAP_VARBIND_COUNT 
#define TRAP_VARBIND_COUNT 2

static const OID envRedundantSupplyOID = {
    LNciscoEnvMonRedundantSupplyNotification,
    (ulong *)IDciscoEnvMonRedundantSupplyNotification
};

/*
 * The OID that identifies the SNMPv1 enterprise. This is identical to the
 * SNMPv2 Notification OID with the last two subidentifiers removed
 */
static const OID enterpriseSupplyOID = {
    LNciscoEnvMonRedundantSupplyNotification - 2, 
    (ulong *)IDciscoEnvMonRedundantSupplyNotification
};

static const OID envSupply_varbinds[TRAP_VARBIND_COUNT] = {
   { LNciscoEnvMonSupplyStatusDescr, (ulong *)IDciscoEnvMonSupplyStatusDescr },
   { LNciscoEnvMonSupplyState, (ulong *)IDciscoEnvMonSupplyState },
};


static void
envmib_redundant_supply_trap (int indx)
{
    int		vbIndex;
    OID	       *vbList[TRAP_VARBIND_COUNT+1];
    OID         instanceOID;
    ulong       instance[1];
    char	trapoid_str[80];

    /*
     * For some unknown reason, SNMP Research made all the object
     * identifier parameters (OID *) *except* for the SNMPv2
     * trap identifier which is (char *).  It is much easier for
     * us to use the mib-compiler generate OID structure, so
     * we'll just convert that into a string.
     */

    if (MakeDotFromOID((OID *)&envRedundantSupplyOID,
				  trapoid_str) == -1) {
	return;
    }	

    /*
     * All the trap varbind objects have an instance described by
     * INDEX { ciscoEnvMonSupplyIndex }
     */
    instance[0] = indx;
    instanceOID.oid_ptr = instance;
    instanceOID.length = 1;


    /*
     * don't do anything if envmon traps are disabled
     */
    if (envmon_globalsData.ciscoEnvMonEnableRedundantSupplyNotification ==
	D_ciscoEnvMonEnableRedundantSupplyNotification_false) {
	return;
    }

    /*
     * create the varbind list
     */
    for (vbIndex = 0; vbIndex < TRAP_VARBIND_COUNT; vbIndex++) {
        vbList[vbIndex] = 
	    CatOID((OID *) &envSupply_varbinds[vbIndex],
			      &instanceOID);
    }
    vbList[vbIndex] = NULL;

    /*
     * tell snmp to do something
     */
    snmp_trap(ENTERPRISE_TRAP, ENV_TRAP_NUMBER, vbList,
			 (OID *)&enterpriseSupplyOID, trapoid_str);

    /*
     * free data allocated by the CatOID service
     */
    for (vbIndex = 0; vbIndex < TRAP_VARBIND_COUNT; vbIndex++) {
	FreeOID(vbList[vbIndex]);
    }

}


void envmib_platform_specific_init (void)
{
    reg_add_envmib_fan_trap(envmib_fan_trap,"envmib_fan_trap");
    reg_add_envmib_redundant_supply_trap(envmib_redundant_supply_trap,
					 "envmib_redundant_supply_trap");

}
      
