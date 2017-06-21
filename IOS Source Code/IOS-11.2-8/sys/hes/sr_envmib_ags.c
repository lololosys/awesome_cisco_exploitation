/* $Id: sr_envmib_ags.c,v 3.3.52.2 1996/04/29 09:45:34 jjohnson Exp $
 * $Source: /release/112/cvs/Xsys/hes/sr_envmib_ags.c,v $
 *------------------------------------------------------------------
 *   ags-specific functions for Environmental Monitor Mib 
 *
 * Nov-1994,   Sandra C. Durham
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_envmib_ags.c,v $
 * Revision 3.3.52.2  1996/04/29  09:45:34  jjohnson
 * CSCdi55599:  System restart when querying CISCO-ENVMON-MIB
 * Branch: California_branch
 *
 * Revision 3.3.52.1  1996/03/18  19:43:23  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.20.2  1996/03/07  09:40:37  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.20.1  1996/02/20  14:20:05  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/12/01  19:03:42  sdurham
 * CSCdi44362:  envmon is non-modular w.r.t envmon mib.
 *   Remove direct calls from envmon code to mib code. Use service points.
 *
 * Revision 3.2  1995/11/17  09:20:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:42:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:44:09  hampton
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

/*  ags system build
*/

#include "ec.h"
#include "sr_envmib_ags.h"


ciscoEnvMonObjects_t *
k_ciscoEnvMonObjects_get (
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator)
{
    static ciscoEnvMonObjects_t ciscoEnvMonObjectsData;

    if (ecp->ec_state < EC_STATE_IDLE)
	ciscoEnvMonObjectsData.ciscoEnvMonPresent = 
	    D_ciscoEnvMonPresent_oldAgs;
    else
	ciscoEnvMonObjectsData.ciscoEnvMonPresent = D_ciscoEnvMonPresent_ags;

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
                 {ECTEST_V_P12, ECTEST_V_N12, ECTEST_V_P5, ECTEST_V_N5};
    static const int voltage [VOLT_MAX_ENTRIES] =
                 { VOLTS_P12, VOLTS_N12, VOLTS_P5, VOLTS_N5};
    int level, percent, envidx;
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

/*  If this is an old ags, the tables are empty!
 *
 */
    if (ecp->ec_state < EC_STATE_IDLE)
	return(NULL); 
   

    voltStatIndex = (searchType == NEXT && voltageStatusIndex ==0) ?
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
		getenvvoltage(dp->t_measured);

    percent = getenvnumber(dp->t_shut_marg,0);
    ciscoEnvMonVoltageStatusEntryData.ciscoEnvMonVoltageThresholdLow =
	(voltage[voltStatIndex-1] * ( 100 - percent))/100; 
    ciscoEnvMonVoltageStatusEntryData.ciscoEnvMonVoltageThresholdHigh =
	(voltage[voltStatIndex-1] * ( 100 + percent))/100; 

    ciscoEnvMonVoltageStatusEntryData.ciscoEnvMonVoltageLastShutdown =
		getenvvoltage(dp->t_shut_last);

    ciscoEnvMonVoltageStatusEntryData.ciscoEnvMonVoltageState =
		dp->t_warning[0] ? D_ciscoEnvMonVoltageState_warning : 
		D_ciscoEnvMonVoltageState_normal; 

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
    static unsigned int snmpToEnvmT [TEMP_MAX_ENTRIES] =
	                   {ECTEST_T_AMB, ECTEST_T_AIR};
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

/*  If this is an old ags, the tables are empty!
 *
 */
    if (ecp->ec_state < EC_STATE_IDLE)
	return(NULL); 
    

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
		getenvtemp(dp->t_measured);

   ciscoEnvMonTemperatureStatusEntryData.ciscoEnvMonTemperatureThreshold =
       getenvtemp(dp->t_shut_marg);


   ciscoEnvMonTemperatureStatusEntryData.ciscoEnvMonTemperatureLastShutdown =
		getenvtemp(dp->t_shut_last);

    ciscoEnvMonTemperatureStatusEntryData.ciscoEnvMonTemperatureState =
		dp->t_warning[0] ?
		D_ciscoEnvMonTemperatureState_warning : 
		D_ciscoEnvMonTemperatureState_normal; 

    return (&ciscoEnvMonTemperatureStatusEntryData);
}

ciscoEnvMonFanStatusEntry_t *
k_ciscoEnvMonFanStatusEntry_get (
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    long            ciscoEnvMonFanStatusIndex)
{

/*   this is an ags, the tables are empty!
 *
 */
	return(NULL); 

}

ciscoEnvMonSupplyStatusEntry_t *
k_ciscoEnvMonSupplyStatusEntry_get (
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    long            ciscoEnvMonSupplyStatusIndex)
{
/*   this is an ags, the tables are empty!
 *
 */
	return(NULL); 

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

    case ECTEST_V_N5:
	return 4;
	
    default :
	return 0;
    }
}

int getEnvTempToSnmpIndex (int indx)
{

    switch (indx) {

    case ECTEST_T_AMB:
	return 1;

    case ECTEST_T_AIR:
	return 2;

    default :
	return 0;
    }
}

void envmib_platform_specific_init (void)
{
    /* nothing needed for ags. */
}
      
