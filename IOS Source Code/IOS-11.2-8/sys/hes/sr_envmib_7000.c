/* $Id: sr_envmib_7000.c,v 3.3.54.4 1996/08/24 04:50:26 ramk Exp $
 * $Source: /release/112/cvs/Xsys/hes/sr_envmib_7000.c,v $
 *------------------------------------------------------------------
 *   7000-specific functions for Environmental Monitor Mib 
 *
 * Nov 1994,   Sandra C. Durham
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_envmib_7000.c,v $
 * Revision 3.3.54.4  1996/08/24  04:50:26  ramk
 * CSCdi63025:  ciscoEnvMonFanState stays stuck at warning(1) when
 * triggered
 * Branch: California_branch
 *
 * Revision 3.3.54.3  1996/05/21  06:29:29  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.3.54.2  1996/04/29  09:45:33  jjohnson
 * CSCdi55599:  System restart when querying CISCO-ENVMON-MIB
 * Branch: California_branch
 *
 * Revision 3.3.54.1  1996/03/18  19:43:21  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.22.2  1996/03/07  09:40:35  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.22.1  1996/02/20  14:20:01  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/24  16:38:33  jjohnson
 * CSCdi42637:  platform-specific code should be moved out of sub_snmp
 * Install an API for accessing platform-specific SNMP objects
 *
 * Revision 3.2  1995/11/17  09:20:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:42:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/16  04:27:40  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  20:44:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "../snmp/snmp_api.h"
#include "sr_envmib.h"
#include "../snmp/snmp_registry.h"
#include "../snmp/snmp_platform_api.h"
#include "../snmp/chassismib_registry.h"
#include "sr_envmib_7000.h"
#include "../os/clock.h"

#include "envm.h"


ciscoEnvMonObjects_t *
k_ciscoEnvMonObjects_get (
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator)
{
    static ciscoEnvMonObjects_t ciscoEnvMonObjectsData;

    ciscoEnvMonObjectsData.ciscoEnvMonPresent = D_ciscoEnvMonPresent_c7000;

    return (&ciscoEnvMonObjectsData);
}



ciscoEnvMonVoltageStatusEntry_t *
k_ciscoEnvMonVoltageStatusEntry_get (
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    long            voltageStatusIndex)
{
    static ciscoEnvMonVoltageStatusEntry_t ciscoEnvMonVoltageStatusEntryData;
    unsigned int voltStatIndex;
    static const unsigned int snmpToEnvmV [VOLT_MAX_ENTRIES] = 
                 {ECTEST_V_P12, ECTEST_V_N12, ECTEST_V_P5, ECTEST_V_P24};
    int level,threshold,envidx;
    ectest_t  *dp;
    static ecshow_t snmp_env_buf;
    ecshow_t *show = &snmp_env_buf;
    static OctetString envVoltStatusDescr;

    /* check if envmon has filled in show struct yet */
    if (!ecp->ec_show)
	return(NULL);

    level = raise_interrupt_level(ALL_DISABLE);
    bcopy(ecp->ec_show, show, sizeof(ecshow_t));
    reset_interrupt_level(level);


    voltStatIndex = (searchType == NEXT && voltageStatusIndex == 0) ?
	1 : (unsigned int)voltageStatusIndex;

    if ((voltStatIndex < 1) ||
	(voltStatIndex > VOLT_MAX_ENTRIES))
	return(NULL);


    envidx = snmpToEnvmV[voltStatIndex-1];
    dp = &show->s_test[envidx];

    ciscoEnvMonVoltageStatusEntryData.ciscoEnvMonVoltageStatusIndex = 
	voltStatIndex;

    ciscoEnvMonVoltageStatusEntryData.ciscoEnvMonVoltageStatusDescr =
	&envVoltStatusDescr;

    envVoltStatusDescr.octet_ptr = dp->t_description;

    envVoltStatusDescr.length = strlen(dp->t_description);

    ciscoEnvMonVoltageStatusEntryData.ciscoEnvMonVoltageStatusValue =
		convert_millivolts(dp->t_measured,envidx);

    threshold = getenvVoltThreshold(envidx,ENV_VOLT_LOW);
    ciscoEnvMonVoltageStatusEntryData.ciscoEnvMonVoltageThresholdLow =
	convert_millivolts(threshold,envidx);

    threshold = getenvVoltThreshold(envidx,ENV_VOLT_HIGH);
    ciscoEnvMonVoltageStatusEntryData.ciscoEnvMonVoltageThresholdHigh =
	convert_millivolts(threshold,envidx);

    ciscoEnvMonVoltageStatusEntryData.ciscoEnvMonVoltageLastShutdown =
       last_showp ?
	  convert_millivolts(last_showp->s_test[envidx].t_measured,envidx) : 0;

    ciscoEnvMonVoltageStatusEntryData.ciscoEnvMonVoltageState =	
	dp->t_state + 1;

    return (&ciscoEnvMonVoltageStatusEntryData);
}


ciscoEnvMonTemperatureStatusEntry_t *
k_ciscoEnvMonTemperatureStatusEntry_get (
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    long            temperatureStatusIndex)
{
    static ciscoEnvMonTemperatureStatusEntry_t 
	ciscoEnvMonTemperatureStatusEntryData;
    unsigned int tempStatIndex;
    static const unsigned int snmpToEnvmT [TEMP_MAX_ENTRIES] =
	                   {ECTEST_T_INLET, ECTEST_T_EXHAUST};
    int level, envidx;
    static ecshow_t snmp_env_buf;
    ecshow_t *show = &snmp_env_buf;
    static OctetString envTempStatusDescr;
    ectest_t *dp;

    /* check if envmon has filled in show struct yet */
    if (!ecp->ec_show)
	return(NULL);

    level = raise_interrupt_level(ALL_DISABLE);
    bcopy(ecp->ec_show, show, sizeof(ecshow_t));
    reset_interrupt_level(level);


    tempStatIndex = (searchType == NEXT && temperatureStatusIndex ==0) ?
	1 : (unsigned int)temperatureStatusIndex;

    if ((tempStatIndex < 1) ||
	(tempStatIndex > TEMP_MAX_ENTRIES))
	return(NULL);

    envidx = snmpToEnvmT[tempStatIndex-1];
    dp = &show->s_test[envidx];

    ciscoEnvMonTemperatureStatusEntryData.ciscoEnvMonTemperatureStatusIndex = 
	tempStatIndex;


    ciscoEnvMonTemperatureStatusEntryData.ciscoEnvMonTemperatureStatusDescr =
	&envTempStatusDescr;

    envTempStatusDescr.octet_ptr = dp->t_description;
    envTempStatusDescr.length = strlen(dp->t_description);

    ciscoEnvMonTemperatureStatusEntryData.ciscoEnvMonTemperatureStatusValue =
	CELSIUS(dp->t_measured);

   ciscoEnvMonTemperatureStatusEntryData.ciscoEnvMonTemperatureThreshold =
	CELSIUS(env_temp_bounds(envidx,SHUTDOWN));

   ciscoEnvMonTemperatureStatusEntryData.ciscoEnvMonTemperatureLastShutdown =
       last_showp ?
		CELSIUS(last_showp->s_test[envidx].t_measured) : 0;

    ciscoEnvMonTemperatureStatusEntryData.ciscoEnvMonTemperatureState =
		dp->t_state + 1;

    return (&ciscoEnvMonTemperatureStatusEntryData);
}

ciscoEnvMonFanStatusEntry_t *
k_ciscoEnvMonFanStatusEntry_get (
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    long            fanStatusIndex)
{
    static ciscoEnvMonFanStatusEntry_t ciscoEnvMonFanStatusEntryData;
    unsigned int fanStatIndex;
    static const unsigned int snmpToEnvmF [NUM_FANS_7010] =
                   {ECTEST_FAN_0, ECTEST_FAN_1, ECTEST_FAN_2,
			ECTEST_FAN_3, ECTEST_FAN_4, ECTEST_FAN_5};
    int level, envidx;
    static ecshow_t snmp_env_buf;
    ecshow_t *show = &snmp_env_buf;
    static OctetString envFanStatusDescr;
    ectest_t *dp;
    uint chassis;                  /* 7000 or 7010 */

    /* check if envmon has filled in show struct yet */
    if (!ecp->ec_show)
	return(NULL);

    level = raise_interrupt_level(ALL_DISABLE);
    bcopy(ecp->ec_show, show, sizeof(ecshow_t));
    reset_interrupt_level(level);

    chassis = snmp_platform_get_value(SNMP_PLATFORM_CHASSIS_TYPE);

    fanStatIndex = (searchType == NEXT && fanStatusIndex ==0) ?
	1 : (unsigned int)fanStatusIndex;

    if (fanStatIndex < 1)
	return(NULL);

    if (chassis == D_chassisType_c7010) {
	if (fanStatIndex > NUM_FANS_7010)
	    return(NULL);
    } else
	/*  7000 */
	if (fanStatIndex > NUM_FANS_7000)
	    return(NULL); 

    envidx = snmpToEnvmF[fanStatIndex-1];
    dp = &show->s_test[envidx];

    ciscoEnvMonFanStatusEntryData.ciscoEnvMonFanStatusIndex =
	fanStatIndex;

    ciscoEnvMonFanStatusEntryData.ciscoEnvMonFanStatusDescr =
	&envFanStatusDescr;

    envFanStatusDescr.octet_ptr = dp->t_description;
    envFanStatusDescr.length = strlen(dp->t_description);

    ciscoEnvMonFanStatusEntryData.ciscoEnvMonFanState = dp->t_state + 1;

    return (&ciscoEnvMonFanStatusEntryData);
}

ciscoEnvMonSupplyStatusEntry_t *
k_ciscoEnvMonSupplyStatusEntry_get (
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    long            supplyStatusIndex)
{

    unsigned int supplyStatIndex;
    static const unsigned int snmpToEnvmS [NUM_PWR_7000] =
	                   {1, 0}; 
    int level, envidx;
    static ecshow_t snmp_env_buf;
    ecshow_t *show = &snmp_env_buf;
    static OctetString envSupplyStatusDescr;
    static ciscoEnvMonSupplyStatusEntry_t ciscoEnvMonSupplyStatusEntryData;
    ecps_t *dp;
    chassis_type_t  chassis;

    /* check if envmon has filled in show struct yet */
    if (!ecp->ec_show)
	return(NULL);

    level = raise_interrupt_level(ALL_DISABLE);
    bcopy(ecp->ec_show, show, sizeof(ecshow_t));
    reset_interrupt_level(level);

    chassis = snmp_platform_get_value(SNMP_PLATFORM_CHASSIS_TYPE);
    supplyStatIndex = (searchType == NEXT && supplyStatusIndex ==0) ?
	1 : (unsigned int)supplyStatusIndex;

    if (supplyStatIndex < 1)
	return(NULL);

    if (chassis == D_chassisType_c7010) {
	if (supplyStatIndex > NUM_PWR_7010)
	    return(NULL); 
    } else
	/* 7000 */
	if (supplyStatIndex > NUM_PWR_7000)
	    return(NULL); 


    envidx = snmpToEnvmS[supplyStatIndex-1]; 
    dp = &show->ps[envidx];
	
    ciscoEnvMonSupplyStatusEntryData.ciscoEnvMonSupplyStatusIndex =
	supplyStatIndex;

    ciscoEnvMonSupplyStatusEntryData.ciscoEnvMonSupplyStatusDescr =
	&envSupplyStatusDescr;


    if (chassis == D_chassisType_c7000) {
	envSupplyStatusDescr.octet_ptr = dp->type_str;
	envSupplyStatusDescr.length = strlen(dp->type_str);
	
    } else {
	/* 7010 */
	dp = &show->ps[envidx-1];  
	envSupplyStatusDescr.octet_ptr = dp->type_str;
	envSupplyStatusDescr.length = strlen(dp->type_str);
    }

    ciscoEnvMonSupplyStatusEntryData.ciscoEnvMonSupplyState =
	(getenvSupplyState(envidx)+1);

    ciscoEnvMonSupplyStatusEntryData.ciscoEnvMonSupplySource =
	getenvSupplySource(envidx);

    return (&ciscoEnvMonSupplyStatusEntryData);
}

int getEnvVoltToSnmpIndex (int indx)
{

    switch (indx) {

    case ECTEST_V_P12:
	return 1;

    case ECTEST_V_N12:
	return 2;

    case ECTEST_V_P5:
	return 3;

    case ECTEST_V_P24:
	return 4;
	
    default :
	return 0;
    }
}

int getEnvTempToSnmpIndex (int indx)
{

    switch (indx) {

    case ECTEST_T_INLET:
	return 1;

    case ECTEST_T_EXHAUST:
	return 2;

    default :
	return 0;
    }
}

int getEnvFanToSnmpIndex (int indx)
{

    switch (indx) {

    case ECTEST_FAN_0:
	return 1;

    case ECTEST_FAN_1:
	return 2;

    case ECTEST_FAN_2:
	return 3;

    case ECTEST_FAN_3:
	return 4;

    case ECTEST_FAN_4:
	return 5;

    case ECTEST_FAN_5:
	return 6;
	
    default :
	return 0;
    }
}





      
