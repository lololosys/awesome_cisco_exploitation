/* $Id: sr_envmib_c7100.c,v 3.2.56.2 1996/04/22 16:40:19 dcarroll Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c7100/sr_envmib_c7100.c,v $
 *------------------------------------------------------------------
 * sr_envmib_c7100.c - Predator Environmental Monitor MIB support
 *
 * August 1995, Dave Carroll
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_envmib_c7100.c,v $
 * Revision 3.2.56.2  1996/04/22  16:40:19  dcarroll
 * CSCdi55259:  ciscoEnvMonPresent needs comma removed
 * Branch: California_branch
 * Move comma out of production code.
 *
 * Revision 3.2.56.1  1996/03/21  23:48:28  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1995/12/15  18:42:58  dcarroll
 * Branch: ELC_branch
 * Remove envmon code from boot image. Use common
 * envmon parser code.
 *
 * Revision 3.2.2.1  1995/11/30  02:15:05  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  18:41:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:23:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../snmp/snmp_api.h"
#include "../hes/envmib_registry.h"
#include "../hes/sr_envmib.h"
#include "c7100_envm.h"

ciscoEnvMonObjects_t *
k_ciscoEnvMonObjects_get (int             serialNum,
			  ContextInfo    *contextInfo,
			  int             nominator)
{
    static ciscoEnvMonObjects_t ciscoEnvMonObjectsData;

    ciscoEnvMonObjectsData.ciscoEnvMonPresent = D_ciscoEnvMonPresent_c7200;

    return &ciscoEnvMonObjectsData;
}

ciscoEnvMonVoltageStatusEntry_t *
k_ciscoEnvMonVoltageStatusEntry_get (int             serialNum,
				     ContextInfo    *contextInfo,
				     int             nominator,
				     int             searchType,
				     long            voltageStatusIndex)
{
    static ciscoEnvMonVoltageStatusEntry_t ciscoEnvMonVoltageStatusEntryData;
    static OctetString envVoltStatusDescr;

    if ((voltageStatusIndex == 0) && (searchType == NEXT))
	voltageStatusIndex = 1;
    if (!c7100_volt_valid(voltageStatusIndex))
	return NULL;

    ciscoEnvMonVoltageStatusEntryData.ciscoEnvMonVoltageStatusIndex = 
	voltageStatusIndex;

    envVoltStatusDescr.octet_ptr = c7100_volt_descr(voltageStatusIndex);
    envVoltStatusDescr.length = strlen(envVoltStatusDescr.octet_ptr);
    ciscoEnvMonVoltageStatusEntryData.ciscoEnvMonVoltageStatusDescr =
	&envVoltStatusDescr;

    ciscoEnvMonVoltageStatusEntryData.ciscoEnvMonVoltageStatusValue =
	c7100_volt_value(voltageStatusIndex);

    ciscoEnvMonVoltageStatusEntryData.ciscoEnvMonVoltageThresholdLow =
	c7100_volt_lowthresh(voltageStatusIndex);

    ciscoEnvMonVoltageStatusEntryData.ciscoEnvMonVoltageThresholdHigh =
	c7100_volt_highthresh(voltageStatusIndex);

    ciscoEnvMonVoltageStatusEntryData.ciscoEnvMonVoltageLastShutdown =
	c7100_volt_lastvalue(voltageStatusIndex);

    ciscoEnvMonVoltageStatusEntryData.ciscoEnvMonVoltageState =
	c7100_volt_state(voltageStatusIndex);

    return &ciscoEnvMonVoltageStatusEntryData;
}

ciscoEnvMonTemperatureStatusEntry_t *
k_ciscoEnvMonTemperatureStatusEntry_get (int           serialNum,
					 ContextInfo   *contextInfo,
					 int           nominator,
					 int           searchType,
					 long          temperatureStatusIndex)
{
    static ciscoEnvMonTemperatureStatusEntry_t
	ciscoEnvMonTemperatureStatusEntryData;
    static OctetString envTempStatusDescr;

    if ((temperatureStatusIndex == 0) && (searchType == NEXT))
	temperatureStatusIndex = 1;
    if (!c7100_temp_valid(temperatureStatusIndex))
	return NULL;

    ciscoEnvMonTemperatureStatusEntryData.ciscoEnvMonTemperatureStatusIndex = 
	temperatureStatusIndex;

    envTempStatusDescr.octet_ptr = c7100_temp_descr(temperatureStatusIndex);
    envTempStatusDescr.length = strlen(envTempStatusDescr.octet_ptr);
    ciscoEnvMonTemperatureStatusEntryData.ciscoEnvMonTemperatureStatusDescr =
	&envTempStatusDescr;

    ciscoEnvMonTemperatureStatusEntryData.ciscoEnvMonTemperatureStatusValue =
	c7100_temp_value(temperatureStatusIndex);

    ciscoEnvMonTemperatureStatusEntryData.ciscoEnvMonTemperatureThreshold =
	c7100_temp_thresh(temperatureStatusIndex);

    ciscoEnvMonTemperatureStatusEntryData.ciscoEnvMonTemperatureLastShutdown =
	c7100_temp_lastvalue(temperatureStatusIndex);

    ciscoEnvMonTemperatureStatusEntryData.ciscoEnvMonTemperatureState =
	c7100_temp_state(temperatureStatusIndex);

    return &ciscoEnvMonTemperatureStatusEntryData;
}

ciscoEnvMonSupplyStatusEntry_t *
k_ciscoEnvMonSupplyStatusEntry_get (int             serialNum,
				    ContextInfo    *contextInfo,
				    int             nominator,
				    int             searchType,
				    long            supplyStatusIndex)
{
    static ciscoEnvMonSupplyStatusEntry_t ciscoEnvMonSupplyStatusEntryData;
    static OctetString envSupplyStatusDescr;

    if ((supplyStatusIndex == 0) && (searchType == NEXT))
	supplyStatusIndex = 1;
    if (!c7100_ps_valid(supplyStatusIndex))
	return NULL;

    ciscoEnvMonSupplyStatusEntryData.ciscoEnvMonSupplyStatusIndex =
	supplyStatusIndex;

    envSupplyStatusDescr.octet_ptr = c7100_ps_descr(supplyStatusIndex);
    envSupplyStatusDescr.length = strlen(envSupplyStatusDescr.octet_ptr);
    ciscoEnvMonSupplyStatusEntryData.ciscoEnvMonSupplyStatusDescr =
	&envSupplyStatusDescr;

    ciscoEnvMonSupplyStatusEntryData.ciscoEnvMonSupplyState =
	c7100_ps_state(supplyStatusIndex);

    return &ciscoEnvMonSupplyStatusEntryData;
}

ciscoEnvMonFanStatusEntry_t *
k_ciscoEnvMonFanStatusEntry_get (int             serialNum,
				 ContextInfo    *contextInfo,
				 int             nominator,
				 int             searchType,
				 long            fanStatusIndex)
{
    static ciscoEnvMonFanStatusEntry_t ciscoEnvMonFanStatusEntryData;
    static OctetString envFanStatusDescr;

    if ((fanStatusIndex == 0) && (searchType == NEXT))
	fanStatusIndex = 1;
    if (!c7100_fan_valid(fanStatusIndex))
	return NULL;

    ciscoEnvMonFanStatusEntryData.ciscoEnvMonFanStatusIndex = fanStatusIndex;

    envFanStatusDescr.octet_ptr = c7100_fan_descr(fanStatusIndex);
    envFanStatusDescr.length = strlen(envFanStatusDescr.octet_ptr);
    ciscoEnvMonFanStatusEntryData.ciscoEnvMonFanStatusDescr =
	&envFanStatusDescr;

    ciscoEnvMonFanStatusEntryData.ciscoEnvMonFanState =
	c7100_fan_state(fanStatusIndex);

    return &ciscoEnvMonFanStatusEntryData;
}

/*
 * these routines translate a platform-dependant index supplied by the
 * trap-sending code into an index to the particular table
 */

int
getEnvVoltToSnmpIndex (int indx)
{
    return 1 + indx;
}

int
getEnvTempToSnmpIndex (int indx)
{
    return 1 + indx;
}

int
getEnvFanToSnmpIndex (int indx)
{
    return indx;
}
      
