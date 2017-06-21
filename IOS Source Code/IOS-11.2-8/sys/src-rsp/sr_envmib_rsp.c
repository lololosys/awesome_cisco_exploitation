/* $Id: sr_envmib_rsp.c,v 3.2 1995/11/17 18:50:02 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-rsp/sr_envmib_rsp.c,v $
 *------------------------------------------------------------------
 * RSP-specific support for environmental monitor MIB
 *
 * February 1995, Carl Schaefer
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_envmib_rsp.c,v $
 * Revision 3.2  1995/11/17  18:50:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:28:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:58:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_registry.h"
#include "../hes/sr_envmib.h"
#include "rsp_chasint.h"

ciscoEnvMonObjects_t *
k_ciscoEnvMonObjects_get (int             serialNum,
			  ContextInfo    *contextInfo,
			  int             nominator)
{
    static ciscoEnvMonObjects_t ciscoEnvMonObjectsData;

    if (!ci_present())
	return NULL;

    ciscoEnvMonObjectsData.ciscoEnvMonPresent = D_ciscoEnvMonPresent_ci;

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
    if (!ci_volt_valid(voltageStatusIndex))
	return NULL;

    ciscoEnvMonVoltageStatusEntryData.ciscoEnvMonVoltageStatusIndex = 
	voltageStatusIndex;

    envVoltStatusDescr.octet_ptr = ci_volt_descr(voltageStatusIndex);
    envVoltStatusDescr.length = strlen(envVoltStatusDescr.octet_ptr);
    ciscoEnvMonVoltageStatusEntryData.ciscoEnvMonVoltageStatusDescr =
	&envVoltStatusDescr;

    ciscoEnvMonVoltageStatusEntryData.ciscoEnvMonVoltageStatusValue =
	ci_volt_value(voltageStatusIndex);

    ciscoEnvMonVoltageStatusEntryData.ciscoEnvMonVoltageThresholdLow =
	ci_volt_lowthresh(voltageStatusIndex);

    ciscoEnvMonVoltageStatusEntryData.ciscoEnvMonVoltageThresholdHigh =
	ci_volt_highthresh(voltageStatusIndex);

    ciscoEnvMonVoltageStatusEntryData.ciscoEnvMonVoltageLastShutdown =
	ci_volt_lastvalue(voltageStatusIndex);

    ciscoEnvMonVoltageStatusEntryData.ciscoEnvMonVoltageState =
	ci_volt_state(voltageStatusIndex);

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
    if (!ci_temp_valid(temperatureStatusIndex))
	return NULL;

    ciscoEnvMonTemperatureStatusEntryData.ciscoEnvMonTemperatureStatusIndex = 
	temperatureStatusIndex;

    envTempStatusDescr.octet_ptr = ci_temp_descr(temperatureStatusIndex);
    envTempStatusDescr.length = strlen(envTempStatusDescr.octet_ptr);
    ciscoEnvMonTemperatureStatusEntryData.ciscoEnvMonTemperatureStatusDescr =
	&envTempStatusDescr;

    ciscoEnvMonTemperatureStatusEntryData.ciscoEnvMonTemperatureStatusValue =
	ci_temp_value(temperatureStatusIndex);

    ciscoEnvMonTemperatureStatusEntryData.ciscoEnvMonTemperatureThreshold =
	ci_temp_thresh(temperatureStatusIndex);

    ciscoEnvMonTemperatureStatusEntryData.ciscoEnvMonTemperatureLastShutdown =
	ci_temp_lastvalue(temperatureStatusIndex);

    ciscoEnvMonTemperatureStatusEntryData.ciscoEnvMonTemperatureState =
	ci_temp_state(temperatureStatusIndex);

    return &ciscoEnvMonTemperatureStatusEntryData;
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
    if (!ci_fan_valid(fanStatusIndex))
	return NULL;

    ciscoEnvMonFanStatusEntryData.ciscoEnvMonFanStatusIndex = fanStatusIndex;

    envFanStatusDescr.octet_ptr = ci_fan_descr(fanStatusIndex);
    envFanStatusDescr.length = strlen(envFanStatusDescr.octet_ptr);
    ciscoEnvMonFanStatusEntryData.ciscoEnvMonFanStatusDescr =
	&envFanStatusDescr;

    ciscoEnvMonFanStatusEntryData.ciscoEnvMonFanState =
	ci_fan_state(fanStatusIndex);

    return &ciscoEnvMonFanStatusEntryData;
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
    if (!ci_ps_valid(supplyStatusIndex))
	return NULL;

    ciscoEnvMonSupplyStatusEntryData.ciscoEnvMonSupplyStatusIndex =
	supplyStatusIndex;

    envSupplyStatusDescr.octet_ptr = ci_ps_descr(supplyStatusIndex);
    envSupplyStatusDescr.length = strlen(envSupplyStatusDescr.octet_ptr);
    ciscoEnvMonSupplyStatusEntryData.ciscoEnvMonSupplyStatusDescr =
	&envSupplyStatusDescr;

    ciscoEnvMonSupplyStatusEntryData.ciscoEnvMonSupplyState =
	ci_ps_state(supplyStatusIndex);

    ciscoEnvMonSupplyStatusEntryData.ciscoEnvMonSupplySource =
	ci_ps_source(supplyStatusIndex);
    return &ciscoEnvMonSupplyStatusEntryData;
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
      
