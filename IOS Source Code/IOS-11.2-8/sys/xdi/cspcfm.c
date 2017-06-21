/* $Id: cspcfm.c,v 3.2 1995/11/17 18:08:45 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/cspcfm.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cspcfm.c,v $
 * Revision 3.2  1995/11/17  18:08:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:54:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:23:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_cspcfm.c xm7_v3.1.1:cset.0166:22:22 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Connection Services Process Module

		CFM State Machine

		File:			xm7_cspcfm.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94

		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
		
		This module implements the CFM state machine listed
		in the ANSI X3T9.5 SMT document.

		With release 7.3, this file contains the following conditional
		compilation options:

			XM_NO_S_PORT	do not include code for S port CFM.
			XM_NO_AB_PORT	do not include code for A/B port CFM.
			XM_NO_M_PORT	do not include code for M port CFM.

		If any of the constants listed above are defined, then the
		associated portion of code is NOT included in the compilation.
		This "reverse option" allows the code to remain backward
		compatible with existing interface files and build procedures.

		Modification History:
        *** SMT 7.3 ***
*********************************************************************/

#include        "cschdrsif.h"
#include        "xdihdrsif.h"

#include	"smterror.h"
#include	"fddihdr.h"
#include	"smtmsg.h"
#include	"csphdr.h"
#include	"cspglbl.h"
#include	"cspproto.h"
#include	"multistn.h"
#include	"cspgrace.h"


/*********************************************************************
		CFM Static Variables
*********************************************************************/

#ifndef XM_NO_S_PORT

/*
 * The Config_Table array allows for quick conversion from the S or A and B
 * port CEM states to a station's CFM state. The single version is for single
 * attachment station. The dual version is for dual attachment stations where
 * the rows represent the B Port CM state and the columns are the A Port CM
 * state.
 */
MCONST
static uInt16    Single_Config_Table[6] =
{
	CF_isolated,						/* S:Isolated */
	CF_wrap_s,							/* S:Primary */
	0,									/* S:Secondary */
	0,									/* S:Thru */
	CF_local_s,							/* S:Local */
	CF_c_wrap_s							/* S:Concatenated */
};

#endif							/* XM_NO_S_PORT */

#ifndef XM_NO_AB_PORT

MCONST
static uInt16    Dual_Config_Table[6][6] =
{
/**
* B:			A:isolated	primary	secondary	thru	local		concatenated
* isolated		CF_isolated	0		CF_wrap_a	0		CF_local_a	CF_c_wrap_a
* primary		CF_wrap_b	0		CF_wrap_ab	0		CF_wrap_b	0
* secondary		0			0		0			0		0			0
* thru			0			0		0			CF_thru	0			0
* local			CF_local_b	0		CF_wrap_a	0		CF_local_ab	CF_c_wrap_a
* concatentated	CF_c_wrap_b	0		0			0		CF_c_wrap_b	0
**/
	{CF_isolated, 0, CF_wrap_a, 0, CF_local_a, CF_c_wrap_a},
	{CF_wrap_b, 0, CF_wrap_ab, 0, CF_wrap_b, 0},
	{0, 0, 0, 0, 0, 0},
	{0, 0, 0, CF_thru, 0, 0},
	{CF_local_b, 0, CF_wrap_a, 0, CF_local_ab, CF_c_wrap_a},
	{CF_c_wrap_b, 0, 0, 0, CF_c_wrap_b, 0}
};
#endif							/* XM_NO_AB_PORT */

/*
* Global variables for determining which paths have had a MAC added
* to them.
*/
static uChar     claimRequired;			/* bits indicate which paths need
										 * claim */
static uInt16    claimLocal;			/* index of local path if claim req */

/*
* Bit definitions for claimRequired.
*/
#define	CR_primary		0x01			/* claim required on primary */
#define	CR_secondary	0x02			/* claim required on secondary */
#define	CR_concat		0x04			/* claim on either pri or sec */
#define	CR_local		0x08			/* claim required on local */


/*********************************************************************
		CFM State Machine Support Routines
*********************************************************************/

void
#ifdef __STDC__
CheckScrubbing (uInt16 stnID)
#else
CheckScrubbing (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	Check all paths to see what paths need scrubbing.
Parameters:	stnID	= index of station to check.
Input:		Uses pathData.
Output:		Clears scrubRequired in pathData if SET.
Return:		No value returned.
Modification History:
*********************************************************************/
{
CSPPATHInfoType *pathPtr;				/* path element pointer */
uInt16           i;						/* loop counter */

	/* check all paths */
	for (i = 0, pathPtr = CSPDATA (stnID, pathData);
		i < MAX_PATH_COUNT;
		i++, pathPtr++)
	{
		/* if scrubbing required */
		if (pathPtr->scrubRequired)
		{
			pathPtr->scrubRequired = CLEAR;
			StartScrubbingMulti (stnID, i);
		}
	}

	return;
}

void
#ifdef __STDC__
CheckClaim (uInt16 stnID)
#else
CheckClaim (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	Check all paths to see what paths need a claim process.
Parameters:	stnID	= index of station to check.
Input:		Uses global claim flags and macData.
Output:		Clears claim flags.
Return:		No value returned.
Modification History:
*********************************************************************/
{
CSPMACInfoType  *macPtr;				/* mac element pointer */
uInt16           i;						/* loop counter */

	/* locate a MAC on the path that needs claim */
	for (i = 0, macPtr = CSPDATA (stnID, macData);
		claimRequired && (i < MAX_MAC_COUNT);
		i++, macPtr++)
	{
		/* check for match on primary */
		if ((claimRequired & (CR_primary | CR_concat))
			&& (macPtr->currentPath == CP_primary))
		{
			/* force claim */
			MACClaimRequestMulti (stnID, macPtr->ID);
			claimRequired &= ~(CR_primary | CR_concat);
		}

		/* check for match on secondary */
		if ((claimRequired & (CR_secondary | CR_concat))
			&& (macPtr->currentPath == CP_secondary))
		{
			/* force claim */
			MACClaimRequestMulti (stnID, macPtr->ID);
			claimRequired &= ~(CR_secondary | CR_concat);
		}

		/* check for match on local */
		if ((claimRequired & CR_local)
			&& (macPtr->currentPath == CP_local))
		{
			/* if MAC is on this local */
			if (macPtr->cpIndex == claimLocal)
			{
				/* force claim */
				MACClaimRequestMulti (stnID, macPtr->ID);
				claimRequired &= ~CR_local;
				claimLocal = 0;
			}
		}
	}

	return;
}


/*********************************************************************
		Port CFM State Machine Actions
*********************************************************************/

void
#ifdef __STDC__
PortIsolated_Actions (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
PortIsolated_Actions (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Perform Isolated_Actions for the Port CFM state machines.
Parameters:	stnID	= index of station to use.
			portPtr	= port element.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	/*
	 * Configuration of the port is done after the state machine completes
	 * execution.
	 */
	portPtr->cmState = CM_ISOLATED;
	portPtr->currentPath = CP_isolated;
	SendCSPEvent (stnID, fddiPORTCurrentPath, portPtr->ID);

	return;
}

void
#ifdef __STDC__
PortPrimary_Actions (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
PortPrimary_Actions (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Perform Primary_Actions for the port CFM state machines.
Parameters:	stnID	= index of station to use.
			portPtr	= port element to use.
Input:		Uses portData.
Output:		Changes portData and stationData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	/*
	 * Configuration of the port is done after the state machine completes
	 * execution.
	 */
	portPtr->cmState = CM_PRIMARY;
	portPtr->currentPath = CP_primary;
	portPtr->cpIndex = PII_primary;
	SendCSPEvent (stnID, fddiPORTCurrentPath, portPtr->ID);

#ifndef XM_NO_M_PORT

	/* check for change to Pri_MPort_Active */
	if (portPtr->PC_Type == PT_m)
	{
		/*
		 * Increment M port count then test if this is the first M port
		 * activated on this path.
		 */
		CSPDATA (stnID, stationData.Pri_MPort_Active)++;
		if (CSPDATA (stnID, stationData.Pri_MPort_Active) == 1)
			ReevaluateAttachment (stnID);
	}

#endif							/* XM_NO_M_PORT */

	return;
}

void
#ifdef __STDC__
PortSecondary_Actions (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
PortSecondary_Actions (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Perform Secondary_Actions for the port CFM state machine.
Parameters:	stnID	= index of station to use.
			portPtr	= port element to use.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	/*
	 * Configuration of the port is done after the state machine completes
	 * execution.
	 */
	portPtr->cmState = CM_SECONDARY;
	portPtr->currentPath = CP_secondary;
	portPtr->cpIndex = PII_secondary;
	SendCSPEvent (stnID, fddiPORTCurrentPath, portPtr->ID);

#ifndef XM_NO_M_PORT

	/* check for change to Sec_MPort_Active */
	if (portPtr->PC_Type == PT_m)
	{
		/*
		 * Increment M port count then test if this is the first M port
		 * activated on this path.
		 */
		CSPDATA (stnID, stationData.Sec_MPort_Active)++;
		if (CSPDATA (stnID, stationData.Sec_MPort_Active) == 1)
			ReevaluateAttachment (stnID);
	}

#endif							/* XM_NO_M_PORT */

	return;
}

#ifndef XM_NO_AB_PORT

static void
#ifdef __STDC__
PortThru_Actions (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
PortThru_Actions (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Perform Thru_Actions for the port CFM state machine.
Parameters:	stnID	= index of station to use.
			portPtr	= port element to use.
Input:		Uses portData and stationData.
Output:		Changes portData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	/*
	 * Configuration of the port is done after the state machine completes
	 * execution.
	 */
	portPtr->cmState = CM_THRU;
	portPtr->currentPath = CP_thru;

	/*
	 * Use path that exits this port as current index.
	 */
	portPtr->cpIndex = (uInt16) (portPtr->PC_Type == PT_a)
		? PII_secondary
		: PII_primary;

	SendCSPEvent (stnID, fddiPORTCurrentPath, portPtr->ID);

	return;
}

#endif							/* XM_NO_AB_PORT */

void
#ifdef __STDC__
PortLocal_Actions (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
PortLocal_Actions (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Perform Local_Actions for the port CFM state machines.
Parameters:	stnID	= index of station to use.
			portPtr	= port element to use.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	/*
	 * Configuration of the port is done after the state machine completes
	 * execution.
	 */
	portPtr->cmState = CM_LOCAL;
	portPtr->currentPath = CP_local;
	portPtr->cpIndex = portPtr->localPathIndex - 1;
	SendCSPEvent (stnID, fddiPORTCurrentPath, portPtr->ID);

	return;
}

/*
 * NOTE: The PortConcat_Actions() could be elminated through conditional
 * compilation if there are no A, B and S ports. However, doing so is
 * cumbersome and such a configuration is unlikely in most situations.
 */

static void
#ifdef __STDC__
PortConcat_Actions (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
PortConcat_Actions (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Perform Concat_Actions for the port CFM state machines.
Parameters:	stnID	= index of station to use.
			portPtr	= port element to use.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	/*
	 * Configuration of the port is done after the state machine completes
	 * execution.
	 */
	portPtr->cmState = CM_CONCAT;
	portPtr->currentPath = CP_concatenated;

	/* use path that exits this port as current index */
	portPtr->cpIndex = (uInt16) (portPtr->PC_Type == PT_a)
		? PII_secondary
		: PII_primary;

	SendCSPEvent (stnID, fddiPORTCurrentPath, portPtr->ID);

	return;
}


/*********************************************************************
		MAC CFM State Machine Actions
*********************************************************************/

static void
#ifdef __STDC__
SetMACConfiguration (uInt16 stnID, CSPMACInfoType * macPtr)
#else
SetMACConfiguration (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	This routine determines the MAC's configuration. This
			includes the values for T_Max, TVX, and T_Req based
			upon the currentPath. The routine also validates
			the MAC's capabilities against the path's requested
			parameter values. Actual MAC configuration is done by RMT.
Parameters:	stnID	= index of station to use
			macPtr	= MAC element.
Input:		Uses macData.
Output:		Changes macData.
Return:		0	= successful configuration
			XDIMAC_configstatus_tvx
				= MAC TVX capabilities did not match requested
						parameter values for path.
			XDIMAC_configstatus_tmax
				= MAC T_Max capabilities did not match requested
						parameter values for path.

			When the path parameters do not match the MAC's
			capabilities, the MAC is put into an isolated state and an
			event message is issued.
Modification History:
*********************************************************************/
{
CSPPATHInfoType *pathPtr;				/* requested PATH element */
uInt16           oldConfig;				/* previous configStatus */

	/* get path element */
	pathPtr = &CSPDATA (stnID, pathData[macPtr->cpIndex]);

	/* get MAC parameters */
	macPtr->TVX = pathPtr->TVXLB;
	macPtr->T_Max = pathPtr->T_MaxLB;
	macPtr->T_Req = pathPtr->MaxT_Req;

	/* if TVX is longer than MAC's capability (values are 2s comp) */
	if (macPtr->TVX < macPtr->TVXCap)
		macPtr->configStatus = XDIMAC_configstatus_tvx;

	else
	{
		/* T_Max longer than MAC's capability */
		if (macPtr->T_Max < macPtr->T_MaxCap)
			macPtr->configStatus = XDIMAC_configstatus_tmax;

		else
		{
			/* save configStatus */
			oldConfig = macPtr->configStatus;

			/* set configStatus */
			macPtr->configStatus = XDIMAC_configstatus_success;

			/* send notification if previous status was not OK */
			if (oldConfig != XDIMAC_configstatus_success)
			{
				/* send event notification */
				SendCSPVendorEvent (stnID, fddiMACVendorAttrib,
					xdiMACConfigurationStatus, macPtr->ID);
			}
		}
	}

	if (macPtr->configStatus != XDIMAC_configstatus_success)
	{
		SendSignalMulti (stnID, SIG_RM_Join, macPtr->ID, (uInt32) CLEAR);
		SendSignalMulti (stnID, SIG_RM_Loop, macPtr->ID, (uInt32) CLEAR);

		macPtr->cmState = CM_ISOLATED;
		macPtr->currentPath = CP_isolated;

		/* send event notification */
		SendCSPVendorEvent (stnID, fddiMACVendorAttrib,
			xdiMACConfigurationStatus, macPtr->ID);
	}
	return;
}

static void
#ifdef __STDC__
MACIsolated_Actions (uInt16 stnID, CSPMACInfoType * macPtr)
#else
MACIsolated_Actions (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Perform Isolated_Actions for the MAC CFM state machines.
Parameters:	stnID	= index of station to use
			macPtr	= MAC element.
Input:		Uses macData.
Output:		Changes macData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	SendSignalMulti (stnID, SIG_RM_Join, macPtr->ID, (uInt32) CLEAR);
	SendSignalMulti (stnID, SIG_RM_Loop, macPtr->ID, (uInt32) CLEAR);

	macPtr->cmState = CM_ISOLATED;
	macPtr->currentPath = CP_isolated;

	if (macPtr->configStatus != XDIMAC_configstatus_success)
	{
		/* send event notification */
		macPtr->configStatus = XDIMAC_configstatus_success;
		SendCSPVendorEvent (stnID, fddiMACVendorAttrib,
			xdiMACConfigurationStatus, macPtr->ID);
	}
	SendCSPEvent (stnID, fddiMACCurrentPath, macPtr->ID);

	return;
}

static void
#ifdef __STDC__
MACPrimary_Actions (uInt16 stnID, CSPMACInfoType * macPtr)
#else
MACPrimary_Actions (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Perform Primary_Actions for the MAC CFM state machines.
Parameters:	stnID	= index of station to use.
			macPtr	= MAC element to use.
Input:		Uses macData.
Output:		Changes macData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	macPtr->cmState = CM_PRIMARY;
	macPtr->currentPath = CP_primary;
	macPtr->cpIndex = PII_primary;

	/* Configure MAC for operation */
	SetMACConfiguration (stnID, macPtr);
	SendCSPEvent (stnID, fddiMACCurrentPath, macPtr->ID);

	/* if configuration successful */
	if (macPtr->currentPath == CP_primary)
	{
		/*
		 * Only send RM_Join if successful.
		 */
		SendSignalMulti (stnID, SIG_RM_Join, macPtr->ID, (uInt32) SET);

		/*
		 * Increment MAC count then test if this is the first MAC activated
		 * on this path.
		 */
		CSPDATA (stnID, stationData.Pri_MAC_Avail)++;
		if (CSPDATA (stnID, stationData.Pri_MAC_Avail) == 1)
		{
			/* reevaluate attachment ports */
			ReevaluateAttachment (stnID);
		}
	}
	return;
}

static void
#ifdef __STDC__
MACSecondary_Actions (uInt16 stnID, CSPMACInfoType * macPtr)
#else
MACSecondary_Actions (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Perform Secondary_Actions for the MAC CFM state machine.
Parameters:	stnID	= index of station to use.
			macPtr	= MAC element to use.
Input:		Uses macData.
Output:		Changes macData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	macPtr->cmState = CM_SECONDARY;
	macPtr->currentPath = CP_secondary;
	macPtr->cpIndex = PII_secondary;

	/* Configure MAC for operation */
	SetMACConfiguration (stnID, macPtr);
	SendCSPEvent (stnID, fddiMACCurrentPath, macPtr->ID);

	/* if configuration successful */
	if (macPtr->currentPath == CP_secondary)
	{
		/*
		 * Only send RM_Join if successful.
		 */
		SendSignalMulti (stnID, SIG_RM_Join, macPtr->ID, (uInt32) SET);

		/*
		 * Increment MAC count then test if this is the first MAC activated
		 * on this path.
		 */
		CSPDATA (stnID, stationData.Sec_MAC_Avail)++;
		if (CSPDATA (stnID, stationData.Sec_MAC_Avail) == 1)
		{
			/* reevaluate attachment ports */
			ReevaluateAttachment (stnID);
		}
	}
	return;
}

static void
#ifdef __STDC__
MACLocal_Actions (uInt16 stnID, CSPMACInfoType * macPtr)
#else
MACLocal_Actions (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Perform Local_Actions for the MAC CFM state machines.
Parameters:	stnID	= index of station to use.
			macPtr	= MAC element to use.
Input:		Uses macData.
Output:		Changes macData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	macPtr->cmState = CM_LOCAL;
	macPtr->currentPath = CP_local;
	macPtr->cpIndex = macPtr->localPathIndex - 1;

	/* Configure MAC for operation */
	SetMACConfiguration (stnID, macPtr);
	SendCSPEvent (stnID, fddiMACCurrentPath, macPtr->ID);

	/*
	 * Only send RM_Loop if successful.
	 */
	if (macPtr->currentPath == CP_local)
		SendSignalMulti (stnID, SIG_RM_Loop, macPtr->ID, (uInt32) SET);

	return;
}


/*********************************************************************
		Attachment Configuration Routines
*********************************************************************/

#ifndef XM_NO_AB_PORT

static void
#ifdef __STDC__
ConfigureABPorts (uInt16 stnID, CSPPORTInfoType * AportPtr,
	CSPPORTInfoType * BportPtr)
#else
ConfigureABPorts (stnID, AportPtr, BportPtr)
		uInt16           stnID;
		CSPPORTInfoType *AportPtr;
		CSPPORTInfoType *BportPtr;
#endif
/*********************************************************************
Function:	Perform the configuration for the A and B ports.
Parameters:	stnID		= index of station to use.
			AportPtr	= A port element to use.
			BportPtr	= B port element to use.
Input:		Uses portData and Dual_Config_Table.
Output:		Changes stationData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
Flag             oldFlag;				/* previous PeerWrapFlag state */
uInt16           cfState;				/* station's configuration */

	/* set new configuration */
	cfState = Dual_Config_Table[BportPtr->cmState][AportPtr->cmState];
	CSPDATA (stnID, stationData.cfState) = cfState;
	SendCSPEvent (stnID, fddiSMTCF_State, 0);

	/* check for PeerWrap */
	oldFlag = (
		((AportPtr->PC_Mode == PC_Mode_Peer)
			&& ((cfState == CF_wrap_a)
				|| (cfState == CF_c_wrap_a)
				|| (cfState == CF_wrap_ab)))
		|| ((BportPtr->PC_Mode == PC_Mode_Peer)
			&& ((cfState == CF_wrap_b)
				|| (cfState == CF_c_wrap_b)
				|| (cfState == CF_wrap_ab)))
		);

	/* if PeerWrapFlag changed */
	if (oldFlag != CSPDATA (stnID, stationData.PeerWrapFlag))
	{
		/* notify MIB */
		CSPDATA (stnID, stationData.PeerWrapFlag) = oldFlag;
		SendCSPEvent (stnID, fddiSMTPeerWrapFlag, 0);
	}

	return;
}

#endif							/* XM_NO_AB_PORT */

#ifndef XM_NO_S_PORT

static void
#ifdef __STDC__
ConfigureSPort (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
ConfigureSPort (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Perform the configuration for the A and B ports.
Parameters:	stnID	= index of station to use.
			portPtr	= S port element to use.
Input:		Uses portData and Single_Config_Table.
Output:		Changes stationData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
Flag             oldFlag;				/* previous PeerWrapFlag state */
uInt16           cfState;				/* station's configuration */

	/* set configuration */
	cfState = Single_Config_Table[portPtr->cmState];
	CSPDATA (stnID, stationData.cfState) = cfState;
	SendCSPEvent (stnID, fddiSMTCF_State, 0);

	/* check for PeerWrap */
	oldFlag = ((portPtr->PC_Mode == PC_Mode_Peer)
		&& (portPtr->PC_Neighbor != PT_s)
		&& ((cfState == CF_wrap_s) || (cfState == CF_c_wrap_s)));

	/* if PeerWrapFlag changed */
	if (oldFlag != CSPDATA (stnID, stationData.PeerWrapFlag))
	{
		/* notify MIB */
		CSPDATA (stnID, stationData.PeerWrapFlag) = oldFlag;
		SendCSPEvent (stnID, fddiSMTPeerWrapFlag, 0);
	}
	return;
}

#endif							/* XM_NO_S_PORT */


/*********************************************************************
		CFM State Machines
*********************************************************************/

#ifndef XM_NO_AB_PORT

static void
#ifdef __STDC__
ProcessABCFM (uInt16 stnID, CSPPORTInfoType * AportPtr,
	CSPPORTInfoType * BportPtr)
#else
ProcessABCFM (stnID, AportPtr, BportPtr)
		uInt16           stnID;
		CSPPORTInfoType *AportPtr;
		CSPPORTInfoType *BportPtr;
#endif
/*********************************************************************
Function:	Process the A and B port CFM state machines. As in
			earlier SMT implementation, the A port is processed
			before the B port.
Parameters:	stnID		= the station's ID
			AportPtr	= A port element.
			BportPtr	= B port element.
Input:		Uses portData and stationData.
Output:		None.
Return:		No value returned.
Modification History:
*********************************************************************/
{

	/*
	 * Select state for Port A CFM.
	 */
	switch (AportPtr->cmState)
	{

	case CM_ISOLATED:
		/* CM(02) */
		if (AportPtr->CF_Join && AportPtr->CF_Wrap)
		{
			PortSecondary_Actions (stnID, AportPtr);

			/* Claim secondary */
			claimRequired |= CR_secondary;
		}
		/* CM(03) */
		else if (AportPtr->CF_Join && BportPtr->CF_Join && AportPtr->CF_Thru)
		{
			PortThru_Actions (stnID, AportPtr);

			/* Claim secondary */
			claimRequired |= CR_secondary;
		}
		/* CM(04) */
		else if (AportPtr->CF_Loop && AportPtr->LocalEnabled)
		{
			PortLocal_Actions (stnID, AportPtr);

			/* Claim local */
			claimRequired |= CR_local;
			claimLocal = AportPtr->localPathIndex;
		}
		/* CM(05) */
		else if (AportPtr->CF_Join && AportPtr->CF_C_Wrap)
		{
			PortConcat_Actions (stnID, AportPtr);

			/* Claim concat */
			claimRequired |= CR_concat;
		}
		break;

	case CM_SECONDARY:
		/* CM(20) */
		if (!AportPtr->CF_Join || AportPtr->WC_Flag)
		{
			CSPDATA (stnID, pathData[PII_secondary].scrubRequired) = SET;
			PortIsolated_Actions (stnID, AportPtr);
		}
		/* CM(23) */
		else if (AportPtr->CF_Join && BportPtr->CF_Join && AportPtr->CF_Thru)
		{
			CSPDATA (stnID, pathData[PII_secondary].scrubRequired) = SET;
			PortThru_Actions (stnID, AportPtr);
		}
		/* CM(25) */
		else if (AportPtr->CF_Join && AportPtr->CF_C_Wrap)
		{
			PortConcat_Actions (stnID, AportPtr);

			/* Claim concat */
			claimRequired |= CR_concat;
		}
		break;

	case CM_THRU:
		/* CM(30) */
		if (!AportPtr->CF_Join || AportPtr->WC_Flag)
		{
			CSPDATA (stnID, pathData[PII_secondary].scrubRequired) = SET;
			PortIsolated_Actions (stnID, AportPtr);
		}
		/* CM(32) */
		else if (AportPtr->CF_Join && AportPtr->CF_Wrap)
		{
			CSPDATA (stnID, pathData[PII_secondary].scrubRequired) = SET;
			PortSecondary_Actions (stnID, AportPtr);
		}
		/* CM(35) */
		else if (AportPtr->CF_Join && AportPtr->CF_C_Wrap)
		{
			CSPDATA (stnID, pathData[PII_secondary].scrubRequired) = SET;
			PortConcat_Actions (stnID, AportPtr);
		}
		break;

	case CM_LOCAL:
		/* CM(40) */
		if (!AportPtr->CF_Loop || !AportPtr->LocalEnabled)
		{
			CSPDATA (stnID,
				pathData[AportPtr->localPathIndex - 1].scrubRequired) = SET;
			PortIsolated_Actions (stnID, AportPtr);
		}
		break;

	case CM_CONCAT:
		/* CM(50) */
		if (!AportPtr->CF_Join || AportPtr->WC_Flag)
		{
			CSPDATA (stnID, pathData[PII_primary].scrubRequired) = SET;
			CSPDATA (stnID, pathData[PII_secondary].scrubRequired) = SET;
			PortIsolated_Actions (stnID, AportPtr);
		}
		/* CM(52) */
		else if (AportPtr->CF_Join && AportPtr->CF_Wrap)
		{
			CSPDATA (stnID, pathData[PII_primary].scrubRequired) = SET;
			CSPDATA (stnID, pathData[PII_secondary].scrubRequired) = SET;
			PortSecondary_Actions (stnID, AportPtr);
		}
		/* CM(53) */
		else if (AportPtr->CF_Join && BportPtr->CF_Join && AportPtr->CF_Thru)
		{
			CSPDATA (stnID, pathData[PII_primary].scrubRequired) = SET;
			CSPDATA (stnID, pathData[PII_secondary].scrubRequired) = SET;
			PortThru_Actions (stnID, AportPtr);
		}
		break;
	}

	/*
	 * Select state for Port B CFM.
	 */
	switch (BportPtr->cmState)
	{

	case CM_ISOLATED:
		/* CM(01) */
		if (BportPtr->CF_Join && BportPtr->CF_Wrap)
		{
			PortPrimary_Actions (stnID, BportPtr);

			/* Claim primary */
			claimRequired |= CR_primary;
		}
		/* CM(03) */
		else if (BportPtr->CF_Join && AportPtr->CF_Join && BportPtr->CF_Thru)
		{
			PortThru_Actions (stnID, BportPtr);

			/* Claim primary */
			claimRequired |= CR_primary;
		}
		/* CM(04) */
		else if (BportPtr->CF_Loop && BportPtr->LocalEnabled)
		{
			PortLocal_Actions (stnID, BportPtr);

			/* Claim local */
			claimRequired |= CR_local;
			claimLocal = BportPtr->localPathIndex;
		}
		/* CM(05) */
		else if (BportPtr->CF_Join && BportPtr->CF_C_Wrap)
		{
			PortConcat_Actions (stnID, BportPtr);

			/* Claim concat */
			claimRequired |= CR_concat;
		}
		break;

	case CM_PRIMARY:
		/* CM(10) */
		if (!BportPtr->CF_Join || BportPtr->WC_Flag)
		{
			CSPDATA (stnID, pathData[PII_primary].scrubRequired) = SET;
			PortIsolated_Actions (stnID, BportPtr);
		}
		/* CM(13) */
		else if (BportPtr->CF_Join && AportPtr->CF_Join && BportPtr->CF_Thru)
		{
			CSPDATA (stnID, pathData[PII_primary].scrubRequired) = SET;
			PortThru_Actions (stnID, BportPtr);
		}
		/* CM(15) */
		else if (BportPtr->CF_Join && BportPtr->CF_C_Wrap)
		{
			PortConcat_Actions (stnID, BportPtr);

			/* Claim concat */
			claimRequired |= CR_concat;
		}
		break;

	case CM_THRU:
		/* CM(30) */
		if (!BportPtr->CF_Join || BportPtr->WC_Flag)
		{
			CSPDATA (stnID, pathData[PII_primary].scrubRequired) = SET;
			PortIsolated_Actions (stnID, BportPtr);
		}
		/* CM(31) */
		else if (BportPtr->CF_Join && BportPtr->CF_Wrap)
		{
			CSPDATA (stnID, pathData[PII_primary].scrubRequired) = SET;
			PortPrimary_Actions (stnID, BportPtr);
		}
		/* CM(35) */
		else if (BportPtr->CF_Join && BportPtr->CF_C_Wrap)
		{
			CSPDATA (stnID, pathData[PII_primary].scrubRequired) = SET;
			PortConcat_Actions (stnID, BportPtr);
		}
		break;

	case CM_LOCAL:
		/* CM(40) */
		if (!BportPtr->CF_Loop || !BportPtr->LocalEnabled)
		{
			CSPDATA (stnID,
				pathData[BportPtr->localPathIndex - 1].scrubRequired) = SET;
			PortIsolated_Actions (stnID, BportPtr);
		}
		break;

	case CM_CONCAT:
		/* CM(50) */
		if (!BportPtr->CF_Join || BportPtr->WC_Flag)
		{
			CSPDATA (stnID, pathData[PII_primary].scrubRequired) = SET;
			CSPDATA (stnID, pathData[PII_secondary].scrubRequired) = SET;
			PortIsolated_Actions (stnID, BportPtr);
		}
		/* CM(51) */
		else if (BportPtr->CF_Join && BportPtr->CF_Wrap)
		{
			CSPDATA (stnID, pathData[PII_primary].scrubRequired) = SET;
			CSPDATA (stnID, pathData[PII_secondary].scrubRequired) = SET;
			PortPrimary_Actions (stnID, BportPtr);
		}
		/* CM(53) */
		else if (BportPtr->CF_Join && AportPtr->CF_Join
			&& BportPtr->CF_Thru)
		{
			CSPDATA (stnID, pathData[PII_primary].scrubRequired) = SET;
			CSPDATA (stnID, pathData[PII_secondary].scrubRequired) = SET;
			PortThru_Actions (stnID, BportPtr);
		}
		break;
	}

	return;
}

#endif							/* XM_NO_AB_PORT */

#ifndef XM_NO_M_PORT

static void
#ifdef __STDC__
ProcessMCFM (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
ProcessMCFM (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Process Master Port CFM state machine.
Parameters:	stnID	= index of station to use.
			portPtr	= port element to use.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	/*
	 * Select CFM state.
	 */
	switch (portPtr->cmState)
	{
	case CM_ISOLATED:
		/* CM(01) */
		if (portPtr->CF_Join && portPtr->CF_Insert_P)
		{
			PortPrimary_Actions (stnID, portPtr);

			/* Claim primary */
			claimRequired |= CR_primary;
		}
		/* CM(02) */
		else if (portPtr->CF_Join && portPtr->CF_Insert_S)
		{
			PortSecondary_Actions (stnID, portPtr);

			/* Claim secondary */
			claimRequired |= CR_secondary;
		}
		/* CM(04) */
		else if (portPtr->CF_Loop && portPtr->LocalEnabled)
		{
			PortLocal_Actions (stnID, portPtr);

			/* Claim local */
			claimRequired |= CR_local;
			claimLocal = portPtr->localPathIndex;
		}
		break;

	case CM_PRIMARY:
		/* CM(10) */
		if (!portPtr->CF_Join || portPtr->WC_Flag)
		{
			CSPDATA (stnID, pathData[PII_primary].scrubRequired) = SET;
			PortIsolated_Actions (stnID, portPtr);

			/*
			 * Decrement M port count then test if this is the last M port
			 * activated on this path.
			 */
			CSPDATA (stnID, stationData.Pri_MPort_Active)--;
			if (CSPDATA (stnID, stationData.Pri_MPort_Active) == 0)
			{
				/* no more, so reevaluate attachment ports */
				ReevaluateAttachment (stnID);
			}
		}
		/* CM(12) */
		else if (portPtr->CF_Join && portPtr->CF_Insert_S)
		{
			CSPDATA (stnID, pathData[PII_primary].scrubRequired) = SET;
			PortSecondary_Actions (stnID, portPtr);

			/* Claim secondary */
			claimRequired |= CR_secondary;

			/*
			 * Decrement M port count then test if this is the last M port
			 * activated on this path.
			 */
			CSPDATA (stnID, stationData.Pri_MPort_Active)--;
			if (CSPDATA (stnID, stationData.Pri_MPort_Active) == 0)
			{
				/* no more, so reevaluate attachment ports */
				ReevaluateAttachment (stnID);
			}
		}
		break;

	case CM_SECONDARY:
		/* CM(20) */
		if (!portPtr->CF_Join || portPtr->WC_Flag)
		{
			CSPDATA (stnID, pathData[PII_secondary].scrubRequired) = SET;
			PortIsolated_Actions (stnID, portPtr);

			/*
			 * Decrement M port count then test if this is the last M port
			 * activated on this path.
			 */
			CSPDATA (stnID, stationData.Sec_MPort_Active)--;
			if (CSPDATA (stnID, stationData.Sec_MPort_Active) == 0)
			{
				/* no more, so reevaluate attachment ports */
				ReevaluateAttachment (stnID);
			}
		}
		/* CM(21) */
		else if (portPtr->CF_Join && portPtr->CF_Insert_P)
		{
			CSPDATA (stnID, pathData[PII_secondary].scrubRequired) = SET;
			PortPrimary_Actions (stnID, portPtr);

			/* Claim primary */
			claimRequired |= CR_primary;

			/*
			 * Decrement M port count then test if this is the last M port
			 * activated on this path.
			 */
			CSPDATA (stnID, stationData.Sec_MPort_Active)--;
			if (CSPDATA (stnID, stationData.Sec_MPort_Active) == 0)
			{
				/* no more, so reevaluate attachment ports */
				ReevaluateAttachment (stnID);
			}
		}
		break;

	case CM_LOCAL:
		/* CM(40) */
		if (!portPtr->CF_Loop || !portPtr->LocalEnabled)
		{
			CSPDATA (stnID, pathData[portPtr->localPathIndex - 1].scrubRequired)
				= SET;
			PortIsolated_Actions (stnID, portPtr);
		}
		break;
	}

	return;
}

#endif							/* XM_NO_M_PORT */

#ifndef XM_NO_S_PORT

static void
#ifdef __STDC__
ProcessSCFM (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
ProcessSCFM (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Process the S Port CFM state machine.
Parameters:	stnID	= index of station to use.
			portPtr	= port element to use.
Input:		Uses portData.
Output:		Changes portData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
	/* select CFM state */
	switch (portPtr->cmState)
	{
	case CM_ISOLATED:
		/* CM(01) */
		if (portPtr->CF_Join && portPtr->CF_Wrap)
		{
			PortPrimary_Actions (stnID, portPtr);

			/* Claim primary */
			claimRequired |= CR_primary;
		}
		/* CM(04) */
		else if (portPtr->CF_Loop && portPtr->LocalEnabled)
		{
			PortLocal_Actions (stnID, portPtr);

			/* Claim local */
			claimRequired |= CR_local;
			claimLocal = portPtr->localPathIndex;
		}
		/* CM(05) */
		else if (portPtr->CF_Join && portPtr->CF_C_Wrap)
		{
			PortConcat_Actions (stnID, portPtr);

			/* Claim concat */
			claimRequired |= CR_concat;
		}
		break;

	case CM_PRIMARY:
		/* CM(10) */
		if (!portPtr->CF_Join || portPtr->WC_Flag)
		{
			CSPDATA (stnID, pathData[PII_primary].scrubRequired) = SET;
			PortIsolated_Actions (stnID, portPtr);
		}
		/* CM(15) */
		else if (portPtr->CF_Join && portPtr->CF_C_Wrap)
		{
			PortConcat_Actions (stnID, portPtr);

			/* Claim primary */
			claimRequired |= CR_primary;
		}
		break;

	case CM_LOCAL:
		/* CM(40) */
		if (!portPtr->CF_Loop || !portPtr->LocalEnabled)
		{
			CSPDATA (stnID, pathData[portPtr->localPathIndex - 1].scrubRequired)
				= SET;
			PortIsolated_Actions (stnID, portPtr);
		}
		break;

	case CM_CONCAT:
		/* CM(50) */
		if (!portPtr->CF_Join || portPtr->WC_Flag)
		{
			CSPDATA (stnID, pathData[PII_primary].scrubRequired) = SET;
			CSPDATA (stnID, pathData[PII_secondary].scrubRequired) = SET;
			PortIsolated_Actions (stnID, portPtr);
		}
		/* CM(51) */
		else if (portPtr->CF_Join && portPtr->CF_Wrap)
		{
			CSPDATA (stnID, pathData[PII_primary].scrubRequired) = SET;
			CSPDATA (stnID, pathData[PII_secondary].scrubRequired) = SET;
			PortPrimary_Actions (stnID, portPtr);
		}
		break;
	}

	return;
}

#endif							/* XM_NO_S_PORT */

static void
#ifdef __STDC__
ProcessMACCFM (uInt16 stnID, CSPMACInfoType * macPtr)
#else
ProcessMACCFM (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Process the MAC CFM state machine.
Parameters:	stnID	= index of station to use.
			macPtr	= MAC element to use.
Input:		Uses macData.
Output:		Changes macData.
Return:		No value returned.
Notes:		Claim process is invoked by RMT if necessary so this routine
			does not perform any claim checking.
Modification History:
*********************************************************************/
{
	/* select CFM state */
	switch (macPtr->cmState)
	{
	case CM_ISOLATED:
		/* CM(01) */
		if (macPtr->Attach_P)
		{
			MACPrimary_Actions (stnID, macPtr);
		}
		/* CM(02) */
		else if (macPtr->Attach_S)
		{
			MACSecondary_Actions (stnID, macPtr);
		}
		/* CM(04) */
		else if (macPtr->Attach_L)
		{
			MACLocal_Actions (stnID, macPtr);
		}
		break;

	case CM_PRIMARY:
		/* CM(10) */
		if (!(macPtr->Attach_P || macPtr->Attach_S))
		{
			CSPDATA (stnID, pathData[PII_primary].scrubRequired) = SET;
			MACIsolated_Actions (stnID, macPtr);

			/*
			 * Decrement MAC count then test if this is the last MAC
			 * activated on this path.
			 */
			CSPDATA (stnID, stationData.Pri_MAC_Avail)--;
			if (CSPDATA (stnID, stationData.Pri_MAC_Avail) == 0)
			{
				/* no more, so reevaluate attachment ports */
				ReevaluateAttachment (stnID);
			}
		}
		/* CM(12) */
		else if (macPtr->Attach_S)
		{
			/* when changing paths, clear RM_Join first to reset RMT */
			SendSignalMulti (stnID, SIG_RM_Join, macPtr->ID, (uInt32) CLEAR);

			CSPDATA (stnID, pathData[PII_primary].scrubRequired) = SET;
			MACSecondary_Actions (stnID, macPtr);

			/*
			 * Decrement MAC count then test if this is the last MAC
			 * activated on this path.
			 */
			CSPDATA (stnID, stationData.Pri_MAC_Avail)--;
			if (CSPDATA (stnID, stationData.Pri_MAC_Avail) == 0)
			{
				/* no more, so reevaluate attachment ports */
				ReevaluateAttachment (stnID);
			}
		}
		break;

	case CM_SECONDARY:
		/* CM(20) */
		if (!(macPtr->Attach_P || macPtr->Attach_S))
		{
			CSPDATA (stnID, pathData[PII_secondary].scrubRequired) = SET;
			MACIsolated_Actions (stnID, macPtr);

			/*
			 * Decrement MAC count then test if this is the last MAC
			 * activated on this path.
			 */
			CSPDATA (stnID, stationData.Sec_MAC_Avail)--;
			if (CSPDATA (stnID, stationData.Sec_MAC_Avail) == 0)
			{
				/* no more, so reevaluate attachment ports */
				ReevaluateAttachment (stnID);
			}
		}
		/* CM(21) */
		else if (macPtr->Attach_P)
		{
			/* when changing paths, clear RM_Join first to reset RMT */
			SendSignalMulti (stnID, SIG_RM_Join, macPtr->ID, (uInt32) CLEAR);

			CSPDATA (stnID, pathData[PII_secondary].scrubRequired) = SET;
			MACPrimary_Actions (stnID, macPtr);

			/*
			 * Decrement MAC count then test if this is the last MAC
			 * activated on this path.
			 */
			CSPDATA (stnID, stationData.Sec_MAC_Avail)--;
			if (CSPDATA (stnID, stationData.Sec_MAC_Avail) == 0)
			{
				/* no more, so reevaluate attachment ports */
				ReevaluateAttachment (stnID);
			}
		}
		break;

	case CM_LOCAL:
		/* CM(40) */
		if (!macPtr->Attach_L)
		{
			CSPDATA (stnID, pathData[macPtr->localPathIndex - 1].scrubRequired)
				= SET;
			MACIsolated_Actions (stnID, macPtr);
		}
		break;
	}

	return;
}

/*********************************************************************
		CFM Selection Criteria
*********************************************************************/

void
#ifdef __STDC__
CFMPortSelectionCriteria (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
CFMPortSelectionCriteria (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Calculate the current selection criteria flags for
			a port.
Parameters:	stnID	= index of station to use.
			portPtr	= port element to use.
Input:		Uses portData and stationData.
Output:		Changes portData and stationData.
Return:		No value returned.
Notes:		The A and B port changes are assumed to occur together.
			Whenever one port's input changes, both ports are
			reevaluated.
Modification History:
*********************************************************************/
{
CSPPORTInfoType *AportPtr;				/* A port element */
CSPPORTInfoType *BportPtr;				/* B port element */
CSPStationInfoType *stnPtr;				/* station pointer */
Flag             oldFlag;				/* old value of a flag */
uInt16           cmState;				/* old CM state */
uInt16           BcmState;				/* old CM state for B port */

	/**
	 * WRF
	 * Initialize AportPtr, BportPtr to avoid
	 * compiler warnings.
	 **/
	AportPtr = BportPtr = NULL;

	/*
	 * Clear claim flags.
	 */
	claimRequired = 0;
	claimLocal = 0;

	/* get station */
	stnPtr = &(CSPDATA (stnID, stationData));

	/*
	 * The A/B and S port selection criteria are handled separately, a case
	 * statement can be used in for each port type.
	 */
	switch (portPtr->PC_Type)
	{

#ifndef XM_NO_S_PORT

		/*
		 * The S port configuration is based upon ConcatEnabled,
		 * ConcatBackup, PrimaryEnabled and PrimaryBackup. These are derived
		 * from ThruEnabled (always FALSE), PrimaryUseless SecondaryUseless,
		 * Pri_MAC_Avail, Sec_MAC_Avail, Pri_MPort_Avail and Sec_MPort_Avail.
		 */
	case PT_s:
		stnPtr->ThruEnabled = FALSE;

		/* set PrimaryUseless */
		stnPtr->PrimaryUseless = (stnPtr->Pri_MAC_Avail == 0)
			&& (stnPtr->Pri_MPort_Active == 0);

		/* set Secondary Useless */
		stnPtr->SecondaryUseless = (stnPtr->Sec_MAC_Avail == 0)
			&& (stnPtr->Sec_MPort_Active == 0);

		/* set ConcatEnabled */
		portPtr->ConcatEnabled = (portPtr->availablePaths & AP_primary)
			&& (portPtr->availablePaths & AP_secondary)
			&& (portPtr->requestedPaths[portPtr->PC_Mode]
			& PP_concatenated_pref)
			&& !(stnPtr->PrimaryUseless
			&& stnPtr->SecondaryUseless);

		/* set ConcatBackup */
		portPtr->ConcatBackup = (portPtr->availablePaths & AP_primary)
			&& (portPtr->availablePaths & AP_secondary)
			&& (portPtr->requestedPaths[portPtr->PC_Mode]
			& PP_concatenated_alt)
			&& !(stnPtr->PrimaryUseless
			&& stnPtr->SecondaryUseless);

		/* set PrimaryEnabled */
		portPtr->PrimaryEnabled = (portPtr->availablePaths & AP_primary)
			&& (portPtr->requestedPaths[portPtr->PC_Mode] & PP_primary_pref)
			&& !(stnPtr->PrimaryUseless);

		/* set PrimaryBackup */
		portPtr->PrimaryBackup = (portPtr->availablePaths & AP_primary)
			&& (portPtr->requestedPaths[portPtr->PC_Mode] & PP_primary_alt)
			&& !(stnPtr->PrimaryUseless);

		/* set LocalEnabled */
		portPtr->LocalEnabled = (portPtr->availablePaths & AP_local);

		break;

#endif							/* XM_NO_S_PORT */

#ifndef XM_NO_AB_PORT

	case PT_a:
	case PT_b:
		/* for A/B, get other port's info */
		if (portPtr->PC_Type == PT_a)
		{
			AportPtr = portPtr;
			BportPtr = &(CSPDATA (stnID, portData[stnPtr->B_Index]));
		}
		else
		{
			AportPtr = &(CSPDATA (stnID, portData[stnPtr->A_Index]));
			BportPtr = portPtr;
		}

		/* recalc ThruEnabled */
		stnPtr->ThruEnabled =
		/* primary available to A & B */
			((AportPtr->availablePaths & AP_primary)
			&& (BportPtr->availablePaths & AP_primary))
		/* secondary available to A & B */
			&& ((AportPtr->availablePaths & AP_secondary)
			&& (BportPtr->availablePaths & AP_secondary))
		/* A & B are both in peer mode */
			&& ((AportPtr->PC_Mode == PC_Mode_Peer)
			&& (BportPtr->PC_Mode == PC_Mode_Peer))
		/* A & B can both go to thru */
			&& ((AportPtr->requestedPaths[PC_Mode_Peer] & PP_thru)
			&& (BportPtr->requestedPaths[PC_Mode_Peer] & PP_thru));

		/* set PrimaryUseless */
		stnPtr->PrimaryUseless = (stnPtr->Pri_MAC_Avail == 0)
			&& (stnPtr->Pri_MPort_Active == 0)
			&& !stnPtr->ThruEnabled;

		/* set Secondary Useless */
		stnPtr->SecondaryUseless = (stnPtr->Sec_MAC_Avail == 0)
			&& (stnPtr->Sec_MPort_Active == 0)
			&& !stnPtr->ThruEnabled;

		/*
		 * Process A port.
		 */
		/* set ConcatEnabled */
		AportPtr->ConcatEnabled = (AportPtr->availablePaths & AP_primary)
			&& (AportPtr->availablePaths & AP_secondary)
			&& (AportPtr->requestedPaths[AportPtr->PC_Mode]
			& PP_concatenated_pref)
			&& !(stnPtr->PrimaryUseless
			&& stnPtr->SecondaryUseless);

		/* set ConcatBackup */
		AportPtr->ConcatBackup = (AportPtr->availablePaths & AP_primary)
			&& (AportPtr->availablePaths & AP_secondary)
			&& (AportPtr->requestedPaths[AportPtr->PC_Mode]
			& PP_concatenated_alt)
			&& !(stnPtr->PrimaryUseless
			&& stnPtr->SecondaryUseless);

		/* set PrimaryEnabled */
		AportPtr->PrimaryEnabled = (AportPtr->availablePaths & AP_primary)
			&& (AportPtr->requestedPaths[AportPtr->PC_Mode]
			& PP_primary_pref)
			&& !(stnPtr->PrimaryUseless);

		/* set PrimaryBackup */
		AportPtr->PrimaryBackup = (AportPtr->availablePaths & AP_primary)
			&& (AportPtr->requestedPaths[AportPtr->PC_Mode]
			& PP_primary_alt)
			&& !(stnPtr->PrimaryUseless);

		/* set SecondaryEnabled */
		AportPtr->SecondaryEnabled = (AportPtr->availablePaths
			& AP_secondary)
			&& (AportPtr->requestedPaths[AportPtr->PC_Mode]
			& PP_secondary_pref)
			&& !(stnPtr->SecondaryUseless);

		/* set SecondaryBackup */
		AportPtr->SecondaryBackup = (AportPtr->availablePaths
			& AP_secondary)
			&& (AportPtr->requestedPaths[AportPtr->PC_Mode]
			& PP_secondary_alt)
			&& !(stnPtr->SecondaryUseless);

		/* set LocalEnabled */
		AportPtr->LocalEnabled = (AportPtr->availablePaths & AP_local);

		/*
		 * Process B port.
		 */
		/* set ConcatEnabled */
		BportPtr->ConcatEnabled = (BportPtr->availablePaths & AP_primary)
			&& (BportPtr->availablePaths & AP_secondary)
			&& (BportPtr->requestedPaths[BportPtr->PC_Mode]
			& PP_concatenated_pref)
			&& !(stnPtr->PrimaryUseless
			&& stnPtr->SecondaryUseless);

		/* set ConcatBackup */
		BportPtr->ConcatBackup = (BportPtr->availablePaths & AP_primary)
			&& (BportPtr->availablePaths & AP_secondary)
			&& (BportPtr->requestedPaths[BportPtr->PC_Mode]
			& PP_concatenated_alt)
			&& !(stnPtr->PrimaryUseless
			&& stnPtr->SecondaryUseless);

		/* set PrimaryEnabled */
		BportPtr->PrimaryEnabled = (BportPtr->availablePaths & AP_primary)
			&& (BportPtr->requestedPaths[BportPtr->PC_Mode]
			& PP_primary_pref)
			&& !(stnPtr->PrimaryUseless);

		/* set PrimaryBackup */
		BportPtr->PrimaryBackup = (BportPtr->availablePaths & AP_primary)
			&& (BportPtr->requestedPaths[BportPtr->PC_Mode]
			& PP_primary_alt)
			&& !(stnPtr->PrimaryUseless);

		/* set SecondaryEnabled */
		BportPtr->SecondaryEnabled = (BportPtr->availablePaths
			& AP_secondary)
			&& (BportPtr->requestedPaths[BportPtr->PC_Mode]
			& PP_secondary_pref)
			&& !(stnPtr->SecondaryUseless);

		/* set SecondaryBackup */
		BportPtr->SecondaryBackup = (BportPtr->availablePaths
			& AP_secondary)
			&& (BportPtr->requestedPaths[BportPtr->PC_Mode]
			& PP_secondary_alt)
			&& !(stnPtr->SecondaryUseless);

		/* set LocalEnabled */
		BportPtr->LocalEnabled = (BportPtr->availablePaths & AP_local);

		break;

#endif							/* XM_NO_AB_PORT */

#ifndef XM_NO_M_PORT

	case PT_m:
		/*
		 * The PrimaryUseless and SecondaryUseless flags do not apply to M
		 * ports and are always FALSE. They are not included in the
		 * evaluation.
		 */

		/* set PrimaryEnabled */
		portPtr->PrimaryEnabled = (portPtr->availablePaths & AP_primary)
			&& (portPtr->requestedPaths[portPtr->PC_Mode] & PP_primary_pref);

		/* set PrimaryBackup */
		portPtr->PrimaryBackup = (portPtr->availablePaths & AP_primary)
			&& (portPtr->requestedPaths[portPtr->PC_Mode] & PP_primary_alt);

		/* set SecondaryEnabled */
		portPtr->SecondaryEnabled = (portPtr->availablePaths & AP_secondary)
			&& (portPtr->requestedPaths[portPtr->PC_Mode] & PP_secondary_pref);

		/* set SecondaryBackup */
		portPtr->SecondaryBackup = (portPtr->availablePaths & AP_secondary)
			&& (portPtr->requestedPaths[portPtr->PC_Mode] & PP_secondary_alt);

		/* set LocalEnabled */
		portPtr->LocalEnabled = (portPtr->availablePaths & AP_local);

		break;

#endif							/* XM_NO_M_PORT */

	}

	/* set CFM Flags */
	switch (portPtr->PC_Type)
	{

#ifndef XM_NO_AB_PORT

	case PT_a:
	case PT_b:
		/*
		 * A and B ports act as a pair and are always evaluated together.
		 * First port A is done.
		 */
		if (stnPtr->ThruEnabled && BportPtr->CF_Join)
		{
			/* set CF_Thru and clear CF_C_Wrap, CF_Wrap */
			AportPtr->CF_Thru = SET;
			AportPtr->CF_C_Wrap = CLEAR;
			AportPtr->CF_Wrap = CLEAR;
		}
		else if (!(BportPtr->ConcatEnabled && BportPtr->CF_Join)
				&& (AportPtr->ConcatEnabled || (AportPtr->ConcatBackup
						&& !(BportPtr->ConcatBackup && BportPtr->CF_Join)
					&& !(BportPtr->PrimaryEnabled && BportPtr->CF_Join))))
		{
			/* set CF_C_Wrap and clear CF_Thru, CF_Wrap */
			AportPtr->CF_Thru = CLEAR;
			AportPtr->CF_C_Wrap = SET;
			AportPtr->CF_Wrap = CLEAR;
		}
		else if (!(BportPtr->ConcatEnabled && BportPtr->CF_Join)
				&& (AportPtr->SecondaryEnabled
					|| (AportPtr->SecondaryBackup && !(BportPtr->ConcatBackup
						&& BportPtr->CF_Join))))
		{
			/* set CF_Wrap and clear CF_Thru, CF_C_Wrap */
			AportPtr->CF_Thru = CLEAR;
			AportPtr->CF_C_Wrap = CLEAR;
			AportPtr->CF_Wrap = SET;
		}
		else
		{
			/* clear all flags */
			AportPtr->CF_Thru = CLEAR;
			AportPtr->CF_C_Wrap = CLEAR;
			AportPtr->CF_Wrap = CLEAR;
		}

		/* set WC_Flag */
		oldFlag = AportPtr->WC_Flag;

		/* clear WC flag if any CFM flag is set */
		AportPtr->WC_Flag = !(AportPtr->CF_Thru || AportPtr->CF_Wrap
			|| AportPtr->CF_C_Wrap);

		/* if WC_Flag changed */
		if (AportPtr->WC_Flag != oldFlag)
			/* notify PCM */
			SendSignalMulti (stnID, SIG_PC_WC_Flag,
				AportPtr->ID, (uInt32) AportPtr->WC_Flag);

		/*
		 * Now do B port.
		 */
		if (stnPtr->ThruEnabled && AportPtr->CF_Join)
		{
			/* set CF_Thru and clear CF_C_Wrap, CF_Wrap */
			BportPtr->CF_Thru = SET;
			BportPtr->CF_C_Wrap = CLEAR;
			BportPtr->CF_Wrap = CLEAR;
		}
		else if (BportPtr->ConcatEnabled || (BportPtr->ConcatBackup
					&& !(AportPtr->ConcatEnabled && AportPtr->CF_Join)
				&& !(AportPtr->SecondaryEnabled && AportPtr->CF_Join)))
		{
			/* set CF_C_Wrap and clear CF_Thru, CF_Wrap */
			BportPtr->CF_Thru = CLEAR;
			BportPtr->CF_C_Wrap = SET;
			BportPtr->CF_Wrap = CLEAR;
		}
		else if (!(AportPtr->ConcatEnabled && AportPtr->CF_Join)
				&& (BportPtr->PrimaryEnabled
					|| (BportPtr->PrimaryBackup && !(AportPtr->ConcatBackup
						&& AportPtr->CF_Join))))
		{
			/* set CF_Wrap and clear CF_Thru, CF_C_Wrap */
			BportPtr->CF_Thru = CLEAR;
			BportPtr->CF_C_Wrap = CLEAR;
			BportPtr->CF_Wrap = SET;
		}
		else
		{
			/* clear all flags */
			BportPtr->CF_Thru = CLEAR;
			BportPtr->CF_C_Wrap = CLEAR;
			BportPtr->CF_Wrap = CLEAR;
		}

		/* set WC_Flag */
		oldFlag = BportPtr->WC_Flag;

		/* clear WC flag if any CFM flag is set */
		BportPtr->WC_Flag = !(BportPtr->CF_Thru || BportPtr->CF_Wrap
			|| BportPtr->CF_C_Wrap);

		/* if WC_Flag changed */
		if (BportPtr->WC_Flag != oldFlag)
			/* notify PCM */
			SendSignalMulti (stnID, SIG_PC_WC_Flag,
				BportPtr->ID, (uInt32) BportPtr->WC_Flag);

		/* Save current CM state */
		cmState = AportPtr->cmState;
		BcmState = BportPtr->cmState;

		/* run CM state machines for A/B ports */
		ProcessABCFM (stnID, AportPtr, BportPtr);

		/*
		 * If CM state has changed, then do config change.
		 */
		if ((cmState != AportPtr->cmState)
			|| (BcmState != BportPtr->cmState))
		{
			/* set new configuration */
			ConfigureABPorts (stnID, AportPtr, BportPtr);

			/* start scrubbing on required paths before config change */
			CheckScrubbing (stnID);

			/* reconfigure ports */
			ConfigureAttachMulti (stnID, stnPtr->cfState);

			/* Check if a claim process is required. */
			CheckClaim (stnID);
		}

		break;

#endif							/* XM_NO_AB_PORT */

#ifndef XM_NO_S_PORT

	case PT_s:
		if (portPtr->ConcatEnabled || portPtr->ConcatBackup)
		{
			/* set CF_C_Wrap and clear CF_Wrap */
			portPtr->CF_C_Wrap = SET;
			portPtr->CF_Wrap = CLEAR;
		}
		else if (portPtr->PrimaryEnabled || portPtr->PrimaryBackup)
		{
			/* set CF_Wrap and clear CF_C_Wrap */
			portPtr->CF_C_Wrap = CLEAR;
			portPtr->CF_Wrap = SET;
		}
		else
		{
			/* clear all flags */
			portPtr->CF_C_Wrap = CLEAR;
			portPtr->CF_Wrap = CLEAR;
		}

		/* set WC_Flag */
		oldFlag = portPtr->WC_Flag;

		/* clear WC flag if any CFM flag is set */
		portPtr->WC_Flag = !(portPtr->CF_Wrap || portPtr->CF_C_Wrap);

		/* if WC_Flag changed */
		if (portPtr->WC_Flag != oldFlag)
			/* notify PCM */
			SendSignalMulti (stnID, SIG_PC_WC_Flag,
				portPtr->ID, (uInt32) portPtr->WC_Flag);

		/* Save current CM state */
		cmState = portPtr->cmState;

		/* run CM state machines for S port */
		ProcessSCFM (stnID, portPtr);

		/*
		 * If CM state has changed, then do config change.
		 */
		if (cmState != portPtr->cmState)
		{
			/* configure S port */
			ConfigureSPort (stnID, portPtr);

			/* start scrubbing on required paths before config change */
			CheckScrubbing (stnID);

			/* reconfigure ports */
			ConfigureAttachMulti (stnID, stnPtr->cfState);

			/* Check if a claim process is required. */
			CheckClaim (stnID);
		}

		break;

#endif							/* XM_NO_S_PORT */

#ifndef XM_NO_M_PORT

	case PT_m:
		/*
		 * The desired flags are not used in this implemenation. They default
		 * to FALSE and have been removed from the evaluation criteria where
		 * appropriate.
		 */
		if (portPtr->PrimaryEnabled || (portPtr->PrimaryBackup
				&& !(portPtr->SecondaryEnabled)))
		{
			/* set CF_Insert_P and clear CF_Insert_S */
			portPtr->CF_Insert_P = SET;
			portPtr->CF_Insert_S = CLEAR;
		}
		else if (portPtr->SecondaryEnabled || portPtr->SecondaryBackup)
		{
			/* clear CF_Insert_P and set CF_Insert_S */
			portPtr->CF_Insert_P = CLEAR;
			portPtr->CF_Insert_S = SET;
		}
		else
		{
			/* clear all flags */
			portPtr->CF_Insert_P = CLEAR;
			portPtr->CF_Insert_S = CLEAR;
		}

		/* set WC_Flag */
		oldFlag = portPtr->WC_Flag;

		/* clear WC flag if any CFM flag is set */
		portPtr->WC_Flag =
			!(portPtr->CF_Insert_P || portPtr->CF_Insert_S);

		/* if WC_Flag changed */
		if (portPtr->WC_Flag != oldFlag)
			/* notify PCM */
			SendSignalMulti (stnID, SIG_PC_WC_Flag,
				portPtr->ID, (uInt32) portPtr->WC_Flag);

#if USE_FULL_GRACE
		/* XLNT Entry support */
		if (CSPDATA (stnID, stationData.gracefulMode) == GIM_gracefulinsertion)
			ProcessGIMCFM (stnID, portPtr, (uInt16) 0, (uInt32) 0);
		else
#endif							/* USE_FULL_GRACE */
		{
			/* Save current CM state */
			cmState = portPtr->cmState;

			/* run CM state machines for M port */
			ProcessMCFM (stnID, portPtr);

			/*
			 * If CM state has changed, then do config change.
			 */
			if (cmState != portPtr->cmState)
			{
				/* start scrubbing on required paths */
				CheckScrubbing (stnID);

				/* configure port */
				MCONFIGURE_M_PORT (stnID, portPtr, (uInt32) 0, (uInt32) 0);

				/* Check if a claim process is required. */
				CheckClaim (stnID);
			}
		}

		break;

#endif							/* XM_NO_M_PORT */

	}

	return;
}

void
#ifdef __STDC__
CFMMACSelectionCriteria (uInt16 stnID, CSPMACInfoType * macPtr)
#else
CFMMACSelectionCriteria (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Calculate the current selection criteria flags for
				a MAC.
Parameters:	stnId	= index of station to use.
				macPtr	= MAC element to use.
Input:		Uses macData and stationData.
Output:		Changes macData and stationData.
Return:		No value returned.
Notes:		For a MAC, PrimaryUseless and SecondaryUseless do
				not apply and are taken to be FALSE in all instances.
Modification History:
*********************************************************************/
{
	/*
	 * Set selection criteria.
	 */

	/* PrimaryEnabled */
	macPtr->PrimaryEnabled = (macPtr->availablePaths & AP_primary)
		&& (macPtr->requestedPaths & PP_primary_pref);

	/* PrimaryBackup */
	macPtr->PrimaryBackup = (macPtr->availablePaths & AP_primary)
		&& (macPtr->requestedPaths & PP_primary_alt);

	/* SecondaryEnabled */
	macPtr->SecondaryEnabled = (macPtr->availablePaths & AP_secondary)
		&& (macPtr->requestedPaths & PP_secondary_pref);

	/* SecondaryBackup */
	macPtr->SecondaryBackup = (macPtr->availablePaths & AP_secondary)
		&& (macPtr->requestedPaths & PP_secondary_alt);

	/* LocalEnabled */
	macPtr->LocalEnabled = (macPtr->availablePaths & AP_local)
		&& (macPtr->requestedPaths & PP_local);

	/**
     * Set CFM Flags.
     * The desired flags are not used in this implemenation. They default to
     * FALSE and have been removed from the evaluation criteria where
     * appropriate.
     **/
	if (macPtr->RM_Available
		&& (macPtr->PrimaryEnabled || (macPtr->PrimaryBackup
				&& !macPtr->SecondaryEnabled)))
	{
		macPtr->Attach_P = SET;
		macPtr->Attach_S = CLEAR;
		macPtr->Attach_L = CLEAR;
	}
	else if (macPtr->RM_Available
		&& (macPtr->SecondaryEnabled || macPtr->SecondaryBackup))
	{
		macPtr->Attach_P = CLEAR;
		macPtr->Attach_S = SET;
		macPtr->Attach_L = CLEAR;
	}
	else if (macPtr->RM_Available && macPtr->LocalEnabled)
	{
		macPtr->Attach_P = CLEAR;
		macPtr->Attach_S = CLEAR;
		macPtr->Attach_L = SET;
	}
	else
	{
		macPtr->Attach_P = CLEAR;
		macPtr->Attach_S = CLEAR;
		macPtr->Attach_L = CLEAR;
	}

	/* run CM state machine for this MAC */
	ProcessMACCFM (stnID, macPtr);

	/*
	 * If MAC is now isolated, check if MAC CFM should reevaluate exit
	 * conditions. The state machine does not check to see if MAC is moving
	 * from primary/secondary to local through isolated.
	 */
	if ((macPtr->cmState == CM_ISOLATED) &&
		(macPtr->Attach_P || macPtr->Attach_S || macPtr->Attach_L))
	{
		ProcessMACCFM (stnID, macPtr);
	}

	/* scrubbing done by ReevaluateAttachment() */

	return;
}

void
#ifdef __STDC__
ReevaluateAttachment (uInt16 stnID)
#else
ReevaluateAttachment (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	Select the attachment port(s) to re-evaluate selection
			criteria.
Parameters:	stnID	= index of station to use.
Input:		Uses portData and stationData.
Output:		Calls routine to reevaluate selection criteria.
Return:		No value returned.
Notes:		This routine selects the attachment port based upon the
			number of attachment ports and whether the attachment]
			port exists in the database. If no attachment port can
			be found, then no change to the criteria can be made.
Modification History:
*********************************************************************/
{
	/* reevaluate A/B or S port */
	switch (CSPDATA (stnID, stationData.attachCount))
	{

#ifndef XM_NO_S_PORT

	case 1:
		/* single attachment */
		if (CSPDATA (stnID, stationData.S_Index) != EMPTY_PORT)
			CFMPortSelectionCriteria (stnID,
				&CSPDATA (stnID, portData[CSPDATA (stnID,
							stationData.S_Index)]));

		break;

#endif							/* XM_NO_S_PORT */

#ifndef XM_NO_AB_PORT

	case 2:
		/* dual attachment */
		if (CSPDATA (stnID, stationData.A_Index != EMPTY_PORT))
			CFMPortSelectionCriteria (stnID,
				&CSPDATA (stnID, portData[CSPDATA (stnID,
							stationData.A_Index)]));

		else if (CSPDATA (stnID, stationData.B_Index != EMPTY_PORT))
			CFMPortSelectionCriteria (stnID,
				&CSPDATA (stnID, portData[CSPDATA (stnID,
							stationData.B_Index)]));

		break;

#endif							/* XM_NO_AB_PORT */

	}

	return;
}


/*********************************************************************
		CFM State Machine
*********************************************************************/

void
#ifdef __STDC__
SignalCFM (uInt16 stnID, uInt16 sigType, uInt16 sigEntity, uInt32 sigData)
#else
SignalCFM (stnID, sigType, sigEntity, sigData)
		uInt16           stnID;
		uInt16           sigType;
		uInt16           sigEntity;
		uInt32           sigData;
#endif
/*********************************************************************
Function:	Process a CFM signal.
Parameters:	stnID	= index of station to use.
			sigType		= the signal ID.
			sigEntity	= entity (PHY) to process.
			sigData		= any data associated with the signal.
Input:		Uses portData, cemData and stationData.
Output:		Changes stationData.
Return:		No value returned.
Modification History:
*********************************************************************/
{
CSPPORTInfoType *portPtr;				/* signal port element */
CSPPORTInfoType *BportPtr;				/* A/B pair additional element */
uInt16           cmState;				/* old CM state */
uInt16           BcmState;				/* old CM state for B port */

	/*
	 * Clear claim flags.
	 */
	claimRequired = 0;
	claimLocal = 0;

	/* get port element */
	portPtr = &(CSPDATA (stnID, portData[sigEntity]));

	/*
	 * Preprocess signal for handling by CEM state machine.
	 */
	switch (sigType)
	{
	case SIG_CF_Join:
		portPtr->CF_Join = (Flag) sigData;
		break;

	case SIG_CF_Loop:
		portPtr->CF_Loop = (Flag) sigData;
		break;
	}


	/*
	 * Process through appropriate CFM state machine based on Port type.
	 */
	switch (portPtr->PC_Type)
	{

#ifndef XM_NO_M_PORT

	case PT_m:
#if USE_FULL_GRACE
		/* XLNT Entry support */
		if (CSPDATA (stnID, stationData.gracefulMode) == GIM_gracefulinsertion)
			ProcessGIMCFM (stnID, portPtr, sigType, sigData);
		else
#endif							/* USE_FULL_GRACE */
		{
			/* Save current CM state */
			cmState = portPtr->cmState;

			/* Process the M Port CFM */
			ProcessMCFM (stnID, portPtr);

			/*
			 * If CM state has changed, then do config change.
			 */
			if (cmState != portPtr->cmState)
			{
				/* start scrubbing on required paths */
				CheckScrubbing (stnID);

				/* Configure M Port */
				MCONFIGURE_M_PORT (stnID, portPtr, sigType, sigData);

				/* Check if a claim process is required. */
				CheckClaim (stnID);
			}
		}

		break;

#endif							/* XM_NO_M_PORT */

#ifndef XM_NO_S_PORT

	case PT_s:
		/* Save current CM state */
		cmState = portPtr->cmState;

		/* Process the S Port CEM */
		ProcessSCFM (stnID, portPtr);

		/*
		 * If CM state has changed, then do config change.
		 */
		if (cmState != portPtr->cmState)
		{
			/* configure the S port */
			ConfigureSPort (stnID, portPtr);

			/* start scrubbing on required paths */
			CheckScrubbing (stnID);

			/* reconfigure ports */
			ConfigureAttachMulti (stnID, CSPDATA (stnID, stationData.cfState));

			/* Check if a claim process is required. */
			CheckClaim (stnID);
		}

		break;

#endif							/* XM_NO_S_PORT */

#ifndef XM_NO_AB_PORT

	case PT_a:
	case PT_b:
		/* Change to CF_Join requires reevaluation (A/B only) */
		if (sigType == SIG_CF_Join)
			CFMPortSelectionCriteria (stnID, portPtr);

		else
		{
			/* process both A and B port CFMs */
			if (portPtr->PC_Type == PT_a)
				BportPtr = &(CSPDATA (stnID,
						portData[CSPDATA (stnID, stationData.B_Index)]));
			else
			{
				/* put B in BportPtr and A in portPtr */
				BportPtr = portPtr;
				portPtr = &(CSPDATA (stnID,
						portData[CSPDATA (stnID, stationData.A_Index)]));
			}

			/* Save current CM state */
			cmState = portPtr->cmState;
			BcmState = BportPtr->cmState;

			/* process the CFM state machines */
			ProcessABCFM (stnID, portPtr, BportPtr);

			/*
			 * If CM state has changed, then do config change.
			 */
			if ((cmState != portPtr->cmState)
				|| (BcmState != BportPtr->cmState))
			{
				/* configure the attachment ports */
				ConfigureABPorts (stnID, portPtr, BportPtr);

				/* start scrubbing on required paths */
				CheckScrubbing (stnID);

				/* reconfigure ports */
				ConfigureAttachMulti (stnID,
					CSPDATA (stnID, stationData.cfState));

				/* Check if a claim process is required. */
				CheckClaim (stnID);
			}
		}

		break;

#endif							/* XM_NO_AB_PORT */

	}

	return;
}
