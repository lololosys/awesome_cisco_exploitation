/* $Id: cspinit.c,v 3.2.62.1 1996/03/18 22:51:22 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/xdi/cspinit.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cspinit.c,v $
 * Revision 3.2.62.1  1996/03/18  22:51:22  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/03/07  11:20:26  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2  1995/11/17  18:08:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:54:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:23:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_cspinit.c xm7_v3.1.1:cset.0166:18:18 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Connection Services Process Module
		
		System Initialization Routines
		
		File:			xm7_cspinit.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This module initializes the CSP global data structures.
		This initialization occurs after the MIB has been initialized.
		CSP will not execute until this initialization has been
		completed.

		Modification History:
        *** SMT 7.3 ***
*********************************************************************/

#include	<ciscolib.h>
#include        "cschdrsif.h"
#include        "xdihdrsif.h"

#include	"smterror.h"
#include	"fddihdr.h"
#include	"csphdr.h"
#include	"cspglbl.h"
#include	"smtmsg.h"
#include	"cspproto.h"
#include	"multistn.h"


/*********************************************************************
		Clear Routines

		These routines clear the given MAC and PORT entries. They are
		used during start-up and when an entity is deleted from the
		MIB database. The SMT and PATH objects do not need clear
		routines because they cannot be deleted.
*********************************************************************/

void
#ifdef __STDC__
InitCSPMACData (CSPMACInfoType * macPtr)
#else
InitCSPMACData (macPtr)
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Initialize CSP global data structure for a MAC.
Parameters:	macPtr	= MAC array element
Input:		Requires macPtr->ID member to be set.
Output:		All global values not requiring hardware present for this
			MAC set to clear or default state.
Return:		None.
Notes:		None.
Modification History:
*********************************************************************/
{
uInt16           pathID;				/* path loop counter */
#if USE_FULL_GRACE
Int16            i;						/* GI port counter */
GIPortNode      *p;						/* GI port pointer */
#endif							/* USE_FULL_GRACE */

	macPtr->hwPresent = NOTPRESENT;
	macPtr->unitdataEnable = FALSE;
	macPtr->index = 0;
	macPtr->localPathIndex = PI_local;
	macPtr->requestedPaths = PP_None;
	macPtr->rmtDupPolicy = RMT_DUP_LEAVE;

#if USE_FULL_GRACE
	/* XLNT Entry support. */
	macPtr->Run_Diagnostics = FALSE;
	macPtr->Loop_Req = FALSE;
	macPtr->GI_Flag = FALSE;
	macPtr->Claim_Started = FALSE;
	macPtr->lmState = LM_IDLE;
	macPtr->TGI = 0;

	/*
	 * Initialize the GI port to insert list.  It is empty at initialization.
	 */
	macPtr->portListHead = NULL;
	macPtr->portListTail = NULL;

	/*
	 * Initialize the GI free list of ports.
	 */
	macPtr->freeListHead = &macPtr->portListGI[0];
	for (i = 0, p = &macPtr->portListGI[0]; i < MAX_PORT_GI - 1; ++p, ++i)
		p->nextNode = p + 1;
	macPtr->portListGI[MAX_PORT_GI - 1].nextNode = NULL;
#endif							/* USE_FULL_GRACE */

	for (pathID = 0; pathID < MAX_PATH_COUNT; pathID++)
	{
		macPtr->configList[pathID].nextIndex = macPtr->ID;
		macPtr->configList[pathID].nextType = RT_mac;
		macPtr->configList[pathID].prevIndex = macPtr->ID;
		macPtr->configList[pathID].prevType = RT_mac;
	}

	return;
}

void
#ifdef __STDC__
InitCSPMACHWData (CSPMACInfoType * macPtr)
#else
InitCSPMACHWData (macPtr)
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Initialize CSP global data structure for a MAC.
Parameters:	macPtr	= MAC array element
Input:		None.
Output:		All global values requiring hardware present for this MAC
			set clear or default state.
Return:		None.
Notes:		None.
Modification History:
*********************************************************************/
{
	macPtr->T_MaxCap = 0;
	macPtr->TVXCap = 0;
	MCopyAddress (macPtr->SMTAddress, SMT_Unknown_Address);
	MCopyAddress (macPtr->upstreamNbr, SMT_Unknown_Address);
	macPtr->availablePaths = AP_None;

	/* RMT Information */
	macPtr->RM_Loop = CLEAR;
	macPtr->RM_Join = CLEAR;
	macPtr->JM_Flag = CLEAR;
	macPtr->DA_Flag = CLEAR;
	macPtr->BN_Flag = CLEAR;
	macPtr->ringOP = CLEAR;
	macPtr->claimTimed = CLEAR;
	macPtr->beaconTimed = CLEAR;
	macPtr->Rmode = CLEAR;
	macPtr->SM_MA_Avail = CLEAR;
	macPtr->TRM = 0;
	macPtr->TRC = 0;
	macPtr->TRB = 0;

	/* CFM Information */
	macPtr->Attach_P = CLEAR;
	macPtr->Attach_S = CLEAR;
	macPtr->Attach_L = CLEAR;
	macPtr->PrimaryEnabled = CLEAR;
	macPtr->SecondaryEnabled = CLEAR;
	macPtr->LocalEnabled = CLEAR;
	macPtr->PrimaryBackup = CLEAR;
	macPtr->SecondaryBackup = CLEAR;
	macPtr->currentPath = CP_isolated;
	macPtr->cpIndex = 0;
	macPtr->cmState = CM_ISOLATED;

	/* MAC Operational Values */
	macPtr->T_Max = 0;
	macPtr->TVX = 0;
	macPtr->T_Req = 0;

	/* MAC RMT and Configuration */
	macPtr->configStatus = XDIMAC_configstatus_success;
	macPtr->Loop_Avail = CLEAR;
	macPtr->MAC_Avail = CLEAR;
	macPtr->RM_Available = CLEAR;
	macPtr->rmState = RM_ISOLATED;

	return;
}

void
#ifdef __STDC__
InitCSPPORTData (CSPPORTInfoType * portPtr)
#else
InitCSPPORTData (portPtr)
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Initialize CSP PORT global data structures.
Parameters:	portPtr	= port element to use.
Input:		Requires portPtr->ID to be set.
Output:		All global values not requiring hardware present for the
			PORT set to a default state.
Return:		None.
Notes:		None.
Modification History:
*********************************************************************/
{
uInt16           pathID;				/* path loop counter */

	/* General PORT Information */
	portPtr->MACLoop_Time = 0;
	portPtr->Maint_LS = QLS;
	portPtr->PC_Type = PT_none;
	portPtr->index = 0;
	portPtr->localPathIndex = PI_local;
	portPtr->connectionPolicies = CP_None;
	portPtr->hwPresent = NOTPRESENT;
	portPtr->lerCutoff = LER_CUTOFF_DEFAULT;
	portPtr->lerAlarm = LER_ALARM_DEFAULT;
	portPtr->requestedPaths[RP_none] = PP_None;
	portPtr->requestedPaths[RP_tree] = PP_None;
	portPtr->requestedPaths[RP_peer] = PP_None;
	portPtr->GI_Pending = FALSE;		/* XLNT Entry */
	for (pathID = 0; pathID < MAX_PATH_COUNT; pathID++)
	{
		portPtr->configList[pathID].nextIndex = portPtr->ID;
		portPtr->configList[pathID].nextType = RT_port;
		portPtr->configList[pathID].prevIndex = portPtr->ID;
		portPtr->configList[pathID].prevType = RT_port;
	}

	portPtr->lerFlag = CLEAR;

	return;
}

void
#ifdef __STDC__
InitCSPPORTHWData (CSPPORTInfoType * portPtr)
#else
InitCSPPORTHWData (portPtr)
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Initialize CSP PORT global data structures.
Parameters:	portPtr	= port element to use.
Input:		None.
Output:		All global values requiring hardware present for the PORT
			set to a default state.
Return:		None.
Notes:		None.
Modification History:
*********************************************************************/
{
	portPtr->availablePaths = AP_None;
	portPtr->conCapabilities = CP_None;

	/* PCM Information */
	portPtr->LC_Test = 0;
	portPtr->PC_Neighbor = PT_none;
	portPtr->PC_Mode = PC_Mode_None;
	portPtr->PC_Withhold = PCW_none;
	portPtr->Connect_State = CS_disabled;
	portPtr->PC_LS = 0;
	portPtr->PC_LCT_Fail = 0;
	portPtr->n = 0;
	portPtr->LS_Flag = CLEAR;
	portPtr->RC_Flag = CLEAR;
	portPtr->TC_Flag = CLEAR;
	portPtr->TD_Flag = CLEAR;
	portPtr->CF_Loop = CLEAR;
	portPtr->CF_Join = CLEAR;
	portPtr->TR_Flag = CLEAR;
	portPtr->WC_Flag = CLEAR;
	portPtr->PC_MAC_LCT = CLEAR;
	portPtr->PC_MAC_Loop = CLEAR;
	portPtr->BS_Flag = CLEAR;
	portPtr->LEM_Fail = CLEAR;
	portPtr->LCT_Passed = CLEAR;
	portPtr->T_Val = 0;
	portPtr->R_Val = 0;
	portPtr->TPC = 0;
	portPtr->TNE = 0;
	portPtr->TScrub = 0;
	portPtr->pcState = PC_OFF;
	portPtr->intrState = PORT_INT_NONE;
	portPtr->isConnecting = FALSE;
	portPtr->isWaiting = FALSE;

	/* CFM Information */
	portPtr->CF_Wrap = CLEAR;
	portPtr->CF_C_Wrap = CLEAR;
	portPtr->CF_Insert_P = CLEAR;
	portPtr->CF_Insert_S = CLEAR;
	portPtr->ConcatEnabled = CLEAR;
	portPtr->PrimaryEnabled = CLEAR;
	portPtr->SecondaryEnabled = CLEAR;
	portPtr->LocalEnabled = CLEAR;
	portPtr->ConcatBackup = CLEAR;
	portPtr->PrimaryBackup = CLEAR;
	portPtr->SecondaryBackup = CLEAR;
	portPtr->currentPath = CP_isolated;
	portPtr->cpIndex = 0;
	portPtr->cmState = CM_ISOLATED;

	/* LEM Information */
	portPtr->LEM_Reject_Ct = 0;
	portPtr->lemCount = 0;
	portPtr->TLE = 0;
	portPtr->threshhold = 0;
	portPtr->estimate = 0;
	portPtr->leState = LE_OFF;
	/* rateEst[] is cleared by LEM */

	portPtr->CF_Thru = CLEAR;

	return;
}


/*********************************************************************
		MIB Initialization Routines

		The following routines initialize CSP global data with the
		values present in the MIB. Each object has its own initialization
		routines. Once all of the MIB values are retrieved, the routines
		initialize any local information based on the MIB data. The
		MAC and PORT objects have separate initialization routines
		for always present and present with hardware attributes to allow
		for flexible configurations that allow management of attributes
		without the hardware being present.
*********************************************************************/

uInt32
#ifdef __STDC__
InitCSPMIBSMTMulti (uInt16 stnID)
#else
InitCSPMIBSMTMulti (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	Initialize CSP global data with SMT MIB attribute
			information.
Parameters:	stnID	= index of station to initialize.
Input:		None.
Output:		The stationData MIB-based values are initialized with the
			current settings from the MIB.
Return:		0 if successful, otherwise an error value is returned.
Modification History:
*********************************************************************/
{
uInt32           result;				/* Get result */
TLVParamType     buffer;				/* MIB request buffer */
CSPStationInfoType *stationPtr;			/* station data */

	/*
	 * Initialize station data
	 */
	stationPtr = &(CSPDATA (stnID, stationData));

	buffer.paramType = fddiSMTMac_Ct;
	buffer.paramLen = 0;
	CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
	stationPtr->macCount = buffer.SMTPARAM8;

	buffer.paramType = fddiSMTNonMaster_Ct;
	buffer.paramLen = 0;
	CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
	stationPtr->attachCount = buffer.SMTPARAM8;

	buffer.paramType = fddiSMTMaster_Ct;
	buffer.paramLen = 0;
	CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
	stationPtr->masterCount = buffer.SMTPARAM8;

	buffer.paramType = fddiSMTAvailablePaths;
	buffer.paramLen = 0;
	CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
	stationPtr->availablePaths = buffer.SMTPARAM8;

	buffer.paramType = fddiSMTConfigPolicy;
	buffer.paramLen = 0;
	CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
	stationPtr->configPolicy = buffer.SMTPARAM16;

	buffer.paramType = fddiSMTConnectionPolicy;
	buffer.paramLen = 0;
	CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
	stationPtr->connectionPolicy = buffer.SMTPARAM16;

	buffer.paramType = fddiSMTBypassPresent;
	buffer.paramLen = 0;
	CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
	stationPtr->Bypass = buffer.SMTPARAM8;

	buffer.paramType = fddiSMTTrace_MaxExpiration;
	buffer.paramLen = 0;
	CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
	/* convert from FddiTimer */
	stationPtr->Trace_Max = MFddi2Usecs (buffer.SMTPARAM32);


	/*
	 * Initialize PCM port management variables and graceful insertion
	 * options.
	 */
	CSPDATA (stnID, stationData.numPortsConnecting) = 0;

	/* get port management value */
	MEMZERO (&buffer, sizeof (buffer));
	buffer.paramType = fddiSMTVendorAttrib;
	buffer.paramLen = 0;
	buffer.SMTVENDORID = xdiSMTMaxMasterCanConnect;
	result = CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
	CSPDATA (stnID, stationData.maxMasterCanConnect) =
		(result == RC_Success) ? buffer.SMTVEND8 : 0;
	CSPDATA (stnID, stationData.shadowmaxMasterCanConnect) =
		(result == RC_Success) ? buffer.SMTVEND8 : 0;

#if (USE_SCHED_GRACE || USE_FULL_GRACE)
	/* get port management value */
	MEMZERO (&buffer, sizeof (buffer));
	buffer.paramType = fddiSMTVendorAttrib;
	buffer.paramLen = 0;
	buffer.SMTVENDORID = xdiSMTGracefulMax;
	result = CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
	CSPDATA (stnID, stationData.gracefulMax) =
		(result == RC_Success) ? buffer.SMTVEND8 : 0;

	MEMZERO (&buffer, sizeof (buffer));
	buffer.paramType = fddiSMTVendorAttrib;
	buffer.paramLen = 0;
	buffer.SMTVENDORID = xdiSMTGracefulInsMode;
	result = CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
	if (result == RC_Success)
		CSPDATA (stnID, stationData.gracefulMode) = buffer.SMTVEND8;
	else
		/* not supported */
		CSPDATA (stnID, stationData.gracefulMode) = GIM_off;
#else							/* (USE_SCHED_GRACE || USE_FULL_GRACE) */
	CSPDATA (stnID, stationData.gracefulMode) = GIM_off;
#endif							/* (USE_SCHED_GRACE || USE_FULL_GRACE) */

	/* XLNT Entry support. */
	if (CSPDATA (stnID, stationData.gracefulMode) != GIM_off)
	{
		if (CSPDATA (stnID, stationData.gracefulMax)
			&& (!CSPDATA (stnID, stationData.maxMasterCanConnect)
				|| (CSPDATA (stnID, stationData.maxMasterCanConnect)
					> CSPDATA (stnID, stationData.gracefulMax))))
		{
			CSPDATA (stnID, stationData.maxMasterCanConnect) =
				CSPDATA (stnID, stationData.gracefulMax);
		}
	}

	/*
	 * Set local values.
	 */
	stationPtr->CF_MAC = CLEAR;

	return (0);
}

uInt32
#ifdef __STDC__
InitCSPMIBMACMulti (uInt16 stnID, CSPMACInfoType * macPtr)
#else
InitCSPMIBMACMulti (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Initialize CSP global data with MAC MIB attribute
			information.
Parameters:	stnID	= index of station to initialize.
			macPtr	= MAC element to initialize
Input:		None.
Output:		The macData MIB-based values are initialized with the
			current settings from the MIB.
Return:		0 if successful, otherwise an error value is returned.
Note:		The buffer of type TLVParamType is assumed to have enough
			data space in it to hold the requested attribute. Since it
			will have at least 4 bytes of header and 32 bytes of data,
			this should be enough for all attributes specied here.
Modification History:
*********************************************************************/
{
TLVParamType     buffer;				/* MIB request buffer */
uInt32           result;				/* return value from MIB get */
Int16            pathID;				/* path loop counter */
TLVPathConfigType *pathConfig;			/* path configuration format */

	/* get manageable attributes first */
	buffer.paramType = fddiMACRequestedPaths;
	buffer.paramLen = 4;
	buffer.MACINDEX = macPtr->ID;
	result = CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
	if (result == RC_Success)
	{
		/* MAC exists at this location */
		macPtr->requestedPaths = buffer.MACPARAM8 & RP_MAC_ALLOWED;

		buffer.paramType = fddiMACIndex;
		buffer.paramLen = 4;
		buffer.MACINDEX = macPtr->ID;
		CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
		macPtr->index = buffer.MACPARAM16;

		buffer.paramType = fddiMACHardwarePresent;
		buffer.paramLen = 4;
		buffer.MACINDEX = macPtr->ID;
		CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
		macPtr->hwPresent = buffer.MACPARAM16;

		buffer.paramType = fddiMACMA_UnitdataEnable;
		buffer.paramLen = 4;
		buffer.MACINDEX = macPtr->ID;
		CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
		macPtr->unitdataEnable = buffer.MACPARAM8;

		MEMZERO (&buffer, sizeof (buffer));
		buffer.paramType = fddiMACVendorAttrib;
		buffer.paramLen = 8;
		buffer.MACINDEX = macPtr->ID;
		buffer.MACVENDORID = xdiMACRMTDupPolicy;
		CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
		/* 731-003 LJP Changed access from MACVEND8 to MACVEND16 */
		macPtr->rmtDupPolicy = buffer.MACVEND16;

		MEMZERO (&buffer, sizeof (buffer));
		buffer.paramType = fddiMACVendorAttrib;
		buffer.paramLen = 8;
		buffer.MACINDEX = macPtr->ID;
		buffer.MACVENDORID = xdiMACLocalPath;
		CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
		macPtr->localPathIndex = buffer.MACVEND16;

		MEMZERO (&buffer, sizeof (buffer));
		buffer.paramType = fddiMACVendorAttrib;
		buffer.MACINDEX = macPtr->ID;
		buffer.MACVENDORID = xdiMACPathConfiguration;

		/* loop on CSP path IDs */
		pathConfig = (TLVPathConfigType *) buffer.MACVENDORDATA;
		for (pathID = 0; pathID < MAX_PATH_COUNT; pathID++)
		{
			/*
			 * Set index to array index. The Get routine returns the actual
			 * resource index.
			 */
			buffer.MACINDEX = macPtr->ID;
			/* 731-003 LJP Changed length from 8 to 12 */
			buffer.paramLen = 12;
			pathConfig->pathID = pathID + 1;	/* MIB ID */
			result = CSPGetMIBAttrMulti (stnID, sizeof (buffer),
				(uChar *) & buffer);
			if (result == RC_Success)
			{
				macPtr->configList[pathID].nextIndex = pathConfig->nextIndex;
				macPtr->configList[pathID].nextType = pathConfig->nextType;
				macPtr->configList[pathID].prevIndex = pathConfig->prevIndex;
				macPtr->configList[pathID].prevType = pathConfig->prevType;
			}
		}

		/* set return value */
		result = 0;
	}
	return (result);
}

uInt32
#ifdef __STDC__
InitCSPMIBMACHWMulti (uInt16 stnID, CSPMACInfoType * macPtr)
#else
InitCSPMIBMACHWMulti (stnID, macPtr)
		uInt16           stnID;
		CSPMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Initialize CSP global data with MAC MIB attribute
			information when HW is present.
Parameters:	stnID	= index of station to initialize.
			macPtr	= MAC element to initialize
Input:		None.
Output:		The macData MIB-based values are initialized with the
			current settings from the MIB.
Return:		0 if successful, otherwise an error value is returned.
Modification History:
*********************************************************************/
{
TLVParamType     buffer;				/* MIB request buffer */
uInt32           result;				/* Get return value */

	buffer.paramType = fddiMACT_MaxCapability;
	buffer.paramLen = 4;
	buffer.MACINDEX = macPtr->ID;
	result = CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);

	/* make sure entity is in MIB */
	if (result == RC_Success)
	{
		macPtr->T_MaxCap = buffer.MACPARAM32;

		buffer.paramType = fddiMACTVXCapability;
		buffer.paramLen = 4;
		buffer.MACINDEX = macPtr->ID;
		CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
		macPtr->TVXCap = buffer.MACPARAM32;

		buffer.paramType = fddiMACAvailablePaths;
		buffer.paramLen = 4;
		buffer.MACINDEX = macPtr->ID;
		CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
		macPtr->availablePaths = buffer.MACPARAM8;

		buffer.paramType = fddiMACSMTAddress;
		buffer.paramLen = 4;
		buffer.MACINDEX = macPtr->ID;
		CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
		MCopyAddress (macPtr->SMTAddress, buffer.MACADDR);

		/* set return value */
		result = 0;
	}
	return (result);
}

uInt32
#ifdef __STDC__
InitCSPMIBPATHMulti (uInt16 stnID, CSPPATHInfoType * pathPtr)
#else
InitCSPMIBPATHMulti (stnID, pathPtr)
		uInt16           stnID;
		CSPPATHInfoType *pathPtr;
#endif
/*********************************************************************
Function:	Initialize CSP global data with PATH MIB attribute
			information.
Parameters:	stnID	= index of station to initialize.
			pathPtr	= PATH element to initialize
Input:		None.
Output:		The pathData MIB-based values are initialized with the
			current settings from the MIB.
Return:		0 if successful, otherwise an error value is returned.
Modification History:
*********************************************************************/
{
TLVParamType     buffer;				/* MIB request buffer */
uInt32           result;				/* result of MIB get */

	/* MIB PATH Attributes */
	buffer.paramType = fddiPATHTVXLowerBound;
	buffer.paramLen = 4;
	buffer.PATHINDEX = pathPtr->ID;
	result = CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);

	/* check if path at this location */
	if (result == RC_Success)
	{
		pathPtr->TVXLB = buffer.PATHPARAM32;

		buffer.paramType = fddiPATHT_MaxLowerBound;
		buffer.paramLen = 4;
		buffer.PATHINDEX = pathPtr->ID;
		CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
		pathPtr->T_MaxLB = buffer.PATHPARAM32;

		buffer.paramType = fddiPATHMaxT_Req;
		buffer.paramLen = 4;
		buffer.PATHINDEX = pathPtr->ID;
		CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
		pathPtr->MaxT_Req = buffer.PATHPARAM32;

		buffer.paramType = fddiPATHT_Rmode;
		buffer.paramLen = 4;
		buffer.PATHINDEX = pathPtr->ID;
		result = CSPGetMIBAttrMulti (stnID, sizeof (buffer),
			(uChar *) & buffer);
		/* if optional attribute is present */
		if (result == RC_Success)
			/* use specified value */
			pathPtr->T_Rmode = buffer.PATHPARAM32;
		else
			/* not supported */
			pathPtr->T_Rmode = 0;

		/* set return value */
		result = 0;
	}
	return (result);
}

uInt32
#ifdef __STDC__
InitCSPMIBPORTMulti (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
InitCSPMIBPORTMulti (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Initialize CSP global data with PORT MIB attribute
			information.
Parameters:	stnID	= index of station to initialize.
			portPtr	= PORT element to initialize
Input:		None.
Output:		The portData MIB-based values are initialized with the
			current settings from the MIB.
Return:		0 if successful, otherwise an error value is returned.
Note:		The buffer of type TLVParamType is assumed to have enough
			data space in it to hold the requested attribute. Since it
			will have at least 4 bytes of header and 32 bytes of data,
			this should be enough for all attributes specied here.
Modification History:
*********************************************************************/
{
TLVParamType     buffer;				/* MIB request buffer */
TLVPathConfigType *pathConfig;			/* path config format */
PORTRequestedPathsType *rpPtr;			/* requested paths format */
Int16            pathID;				/* path loop index */
uInt32           result;				/* result of MIB get */

	/* PORT data */
	buffer.paramType = fddiPORTMy_Type;
	buffer.paramLen = 4;
	buffer.PORTINDEX = portPtr->ID;
	result = CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
	if (result == RC_Success)
	{
		/* port at this entry */
		portPtr->PC_Type = buffer.PORTPARAM16;

		buffer.paramType = fddiPORTConnectionPolicies;
		buffer.paramLen = 4;
		buffer.PORTINDEX = portPtr->ID;
		CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
		portPtr->connectionPolicies = buffer.PORTPARAM8;

		buffer.paramType = fddiPORTRequestedPaths;
		buffer.paramLen = 4;
		buffer.PORTINDEX = portPtr->ID;
		CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
		rpPtr = (PORTRequestedPathsType *) & buffer.PORTPARAM32;

		/* set requested values based on valid bits */
		switch (portPtr->PC_Type)
		{
		case PT_a:
			portPtr->requestedPaths[RP_none]
				= rpPtr->reqPaths[RP_none] & RP_A_NONE_ALLOWED;
			portPtr->requestedPaths[RP_tree]
				= rpPtr->reqPaths[RP_tree] & RP_A_TREE_ALLOWED;
			portPtr->requestedPaths[RP_peer]
				= rpPtr->reqPaths[RP_peer] & RP_A_PEER_ALLOWED;

			/* set station index value as well */
			CSPDATA (stnID, stationData.A_Index) = portPtr->ID;
			break;

		case PT_b:
			portPtr->requestedPaths[RP_none]
				= rpPtr->reqPaths[RP_none] & RP_B_NONE_ALLOWED;
			portPtr->requestedPaths[RP_tree]
				= rpPtr->reqPaths[RP_tree] & RP_B_TREE_ALLOWED;
			portPtr->requestedPaths[RP_peer]
				= rpPtr->reqPaths[RP_peer] & RP_B_PEER_ALLOWED;

			/* set station index value as well */
			CSPDATA (stnID, stationData.B_Index) = portPtr->ID;
			break;

		case PT_s:
			portPtr->requestedPaths[RP_none]
				= rpPtr->reqPaths[RP_none] & RP_S_NONE_ALLOWED;
			portPtr->requestedPaths[RP_tree]
				= rpPtr->reqPaths[RP_tree] & RP_S_TREE_ALLOWED;
			portPtr->requestedPaths[RP_peer]
				= rpPtr->reqPaths[RP_peer] & RP_S_PEER_ALLOWED;

			/* set station index value as well */
			CSPDATA (stnID, stationData.S_Index) = portPtr->ID;
			break;

		case PT_m:
			portPtr->requestedPaths[RP_none]
				= rpPtr->reqPaths[RP_none] & RP_M_NONE_ALLOWED;
			portPtr->requestedPaths[RP_tree]
				= rpPtr->reqPaths[RP_tree] & RP_M_TREE_ALLOWED;
			break;
		}

		buffer.paramType = fddiPORTIndex;
		buffer.paramLen = 4;
		buffer.PORTINDEX = portPtr->ID;
		CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
		portPtr->index = buffer.PORTPARAM16;

		buffer.paramType = fddiPORTLer_Cutoff;
		buffer.paramLen = 4;
		buffer.PORTINDEX = portPtr->ID;
		CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
		portPtr->lerCutoff = buffer.PORTPARAM8;

		buffer.paramType = fddiPORTLer_Alarm;
		buffer.paramLen = 4;
		buffer.PORTINDEX = portPtr->ID;
		CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
		portPtr->lerAlarm = buffer.PORTPARAM8;

		buffer.paramType = fddiPORTHardwarePresent;
		buffer.paramLen = 4;
		buffer.PORTINDEX = portPtr->ID;
		CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
		portPtr->hwPresent = buffer.PORTPARAM16;

		/* get XDI attributes */
		MEMZERO (&buffer, sizeof (buffer));
		buffer.paramType = fddiPORTVendorAttrib;
		buffer.paramLen = 8;
		buffer.PORTINDEX = portPtr->ID;
		buffer.PORTVENDORID = xdiPORTLocalPath;
		CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
		portPtr->localPathIndex = buffer.PORTVEND16;

		MEMZERO (&buffer, sizeof (buffer));
		buffer.paramType = fddiPORTVendorAttrib;
		buffer.PORTINDEX = portPtr->ID;
		buffer.PORTVENDORID = xdiPORTPathConfiguration;
		pathConfig = (TLVPathConfigType *) buffer.PORTVENDORDATA;

		/* loop on CSP path IDs */
		for (pathID = 0; pathID < MAX_PATH_COUNT; pathID++)
		{
			/*
			 * Set index to array index. The Get routine returns the actual
			 * resource index.
			 */
			buffer.PORTINDEX = portPtr->ID;
			/* 731-003 LJP  Changed length from 8 to 12 */
			buffer.paramLen = 12;
			pathConfig->pathID = pathID + 1;	/* MIB ID */
			result = CSPGetMIBAttrMulti (stnID, sizeof (buffer),
				(uChar *) & buffer);
			if (result == RC_Success)
			{
				portPtr->configList[pathID].nextIndex = pathConfig->nextIndex;
				portPtr->configList[pathID].nextType = pathConfig->nextType;
				portPtr->configList[pathID].prevIndex = pathConfig->prevIndex;
				portPtr->configList[pathID].prevType = pathConfig->prevType;
			}
		}

		/* optional parameters */
		buffer.paramType = fddiPORTMACLoop_Time;
		buffer.paramLen = 4;
		buffer.PORTINDEX = portPtr->ID;
		result = CSPGetMIBAttrMulti (stnID, sizeof (buffer),
			(uChar *) & buffer);
		if (result == RC_Success)
			/* use specified value */
			portPtr->MACLoop_Time = buffer.PORTPARAM32;
		else
		{
			/* not supported */
			portPtr->MACLoop_Time = 0;
			portPtr->connectionPolicies &= ~CP_pc_mac_loop;
		}

		buffer.paramType = fddiPORTMaint_LS;
		buffer.paramLen = 4;
		buffer.PORTINDEX = portPtr->ID;
		result = CSPGetMIBAttrMulti (stnID, sizeof (buffer),
			(uChar *) & buffer);
		if (result == RC_Success)
		{
			switch (buffer.PORTPARAM8)
			{
			case LS_qls:
				portPtr->Maint_LS = QLS;
				break;

			case LS_ils:
				portPtr->Maint_LS = ILS;
				break;

			case LS_mls:
				portPtr->Maint_LS = MLS;
				break;

			case LS_hls:
				portPtr->Maint_LS = HLS;
				break;

			case LS_pdr:
				portPtr->Maint_LS = PDRLS;
				break;
			}
		}
		else
			/* not supported */
			portPtr->Maint_LS = QLS;

		/* set return value */
		result = 0;
	}
	return (result);
}

uInt32
#ifdef __STDC__
InitCSPMIBPORTHWMulti (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
InitCSPMIBPORTHWMulti (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Initialize CSP global data with PORT MIB attribute
				information when HW is present.
Parameters:	stnID	= index of station to initialize.
				portPtr	= PORT element to initialize
Input:		None.
Output:		The portData MIB-based values are initialized with the
				current settings from the MIB.
Return:		0 if successful, otherwise an error value is returned.
Modification History:
*********************************************************************/
{
TLVParamType     buffer;				/* MIB request buffer */
uInt32           result;				/* return value */

	buffer.paramType = fddiPORTAvailablePaths;
	buffer.paramLen = 4;
	buffer.PORTINDEX = portPtr->ID;
	result = CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);

	/* check if entity is present */
	if (result == RC_Success)
	{
		portPtr->availablePaths = buffer.PORTPARAM8;

		buffer.paramType = fddiPORTConnectionCapabilities;
		buffer.paramLen = 4;
		buffer.PORTINDEX = portPtr->ID;
		CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
		portPtr->conCapabilities = buffer.PORTPARAM8;

		result = 0;
	}
	return (result);
}


/*********************************************************************
		General Initialization Routines
*********************************************************************/

uInt32
#ifdef __STDC__
InitCSPData (uInt16 stnID)
#else
InitCSPData (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	Clear CSP global data structures.
Parameters:	stnID	= index of station to use.
Input:		None.
Output:		All global values cleared. No hardware is affected.
Return:		0 if successful, otherwise an error value is returned.
Notes:		None.
Modification History:
*********************************************************************/
{
Int16            i;						/* loop counter */
CSPStationInfoType *stationPtr;			/* MIB station data */
CSPMACInfoType  *macPtr;				/* MIB MAC entry */
CSPPATHInfoType *pathPtr;				/* MIB PATH entry */
CSPPORTInfoType *portPtr;				/* MIB PORT entry */

	/*
	 * Initialize station data
	 */
	stationPtr = &(CSPDATA (stnID, stationData));

	/* General station information */
	stationPtr->Trace_Max = Trace_Max_Default;
	stationPtr->configPolicy = CFP_None;
	stationPtr->connectionPolicy = 0xFF;/* reject all */
	stationPtr->attachCount = 0;
	stationPtr->masterCount = 0;
	stationPtr->macCount = 0;
	stationPtr->availablePaths = AP_None;

	stationPtr->CSPInitialized = CLEAR;
	stationPtr->CF_MAC = CLEAR;
	stationPtr->A_Index = EMPTY_PORT;
	stationPtr->B_Index = EMPTY_PORT;
	stationPtr->S_Index = EMPTY_PORT;

	/* ECM Information */
	stationPtr->Bypass = CLEAR;
	stationPtr->SB_Flag = CLEAR;
	stationPtr->TEC = 0;
	stationPtr->Path_Test = PT_None;
	stationPtr->ecState = EC_OUT;
	stationPtr->cfState = CM_ISOLATED;

	/* CFM Information */
	stationPtr->PrimaryUseless = SET;
	stationPtr->SecondaryUseless = SET;
	stationPtr->ThruEnabled = CLEAR;
	stationPtr->PeerWrapFlag = CLEAR;
	stationPtr->Pri_MPort_Active = 0;
	stationPtr->Sec_MPort_Active = 0;
	stationPtr->Pri_MAC_Avail = 0;
	stationPtr->Sec_MAC_Avail = 0;


	/*
	 * Initialize MAC data
	 */
	for (i = 0, macPtr = CSPDATA (stnID, macData);
		i < MAX_MAC_COUNT;
		i++, macPtr++)
	{
		macPtr->ID = i;					/* array index fixed */
		InitCSPMACData (macPtr);
		InitCSPMACHWData (macPtr);
	}

	/*
	 * Initialize all PATH data
	 */
	for (i = 0, pathPtr = CSPDATA (stnID, pathData);
		i < MAX_PATH_COUNT;
		i++, pathPtr++)
	{
		pathPtr->T_Rmode = 0;
		pathPtr->MaxT_Req = 0;
		pathPtr->T_MaxLB = 0;
		pathPtr->TVXLB = 0;

		pathPtr->ID = i;
		pathPtr->traceStatus = TS_None;
	}

	/*
	 * Initialize all PORT data
	 */
	for (i = 0, portPtr = CSPDATA (stnID, portData);
		i < CSP_PORT_COUNT;				/* includes EMPTY_PORT */
		i++, portPtr++)
	{
		portPtr->ID = i;				/* array index fixed */
		InitCSPPORTData (portPtr);
		InitCSPPORTHWData (portPtr);
	}

	return (0);
}

uInt32
#ifdef __STDC__
InitCSPMIBDataMulti (uInt16 stnID)
#else
InitCSPMIBDataMulti (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	Initialize CSP global data after MIB has been initialized.
parameters:	None.
Input:		None.
Output:		All global values set to current MIB settings.
Return:		0 if successful, otherwise an error value is returned.
Notes:		This function is called when the MIB initialization
			message is received.
Modification History:
*********************************************************************/
{
CSPMACInfoType  *macPtr;				/* MAC element */
CSPPATHInfoType *pathPtr;				/* PATH element */
CSPPORTInfoType *portPtr;				/* PORT element */
uInt32           result;				/* result of init operations */
uInt16           i;						/* loop counter */
#if USE_FULL_GRACE
TLVParamType     buffer;				/* MIB Get buffer */
ProcState        pState;				/* interrupt state */
uInt16           rscindexGIMAC;			/* index of graceful insertion MAC */
#endif							/* USE_FULL_GRACE */

	/* Initialize station data */
	result = InitCSPMIBSMTMulti (stnID);

	/*
	 * Initialize MAC data
	 * 
	 * All MAC entries are read since CSP does not know where the MACs are in
	 * the MIB. Note: MAC IDs refer to the MIB array element, not the actual
	 * resource ID.
	 * 
	 * MSP fills the MIB arrays sequentially. This routine should only be called
	 * after MSP has been initialized. Therefore, the first element that
	 * returns an error (i.e., RC_NoParameter) must indicate the end of the
	 * active elements in the MIB array. This is determined by the
	 * InitCSPMIBxxxMulti() routine returning a non-zero value.
	 */
	for (i = 0, macPtr = CSPDATA (stnID, macData);
		i < MAX_MAC_COUNT;
		i++, macPtr++)
	{
		result = InitCSPMIBMACMulti (stnID, macPtr);
		if (result != 0)
			/* assume end of array */
			break;

		/* if MAC is not present, skip remaining attributes */
		if (macPtr->hwPresent == NOTPRESENT)
			continue;

		else
			result = InitCSPMIBMACHWMulti (stnID, macPtr);
	}

#if USE_FULL_GRACE
	/* XLNT Entry support */
	MEMZERO (&buffer, sizeof (buffer));
	buffer.paramType = fddiSMTVendorAttrib;
	buffer.paramLen = 0;
	buffer.SMTVENDORID = xdiSMTGIMACID;
	result = CSPGetMIBAttrMulti (stnID, sizeof (buffer), (uChar *) & buffer);
	rscindexGIMAC = buffer.SMTVEND16;

	CSPDATA (stnID, stationData.giMACID) = NO_GIMAC;

	for (i = 0, macPtr = CSPDATA (stnID, macData);
		i < MAX_MAC_COUNT; i++, macPtr++)
	{
		if (macPtr->index == rscindexGIMAC)
		{
			CSPDATA (stnID, stationData.giMACID) = macPtr->ID;
			break;
		}
	}

	if ((CSPDATA (stnID, stationData.gracefulMode) == GIM_gracefulinsertion)
		&& (CSPDATA (stnID, stationData.giMACID) != NO_GIMAC))
	{
		CSPDATA (stnID, macData[CSPDATA (stnID, stationData.giMACID)].GI_Flag)
			= TRUE;
	}
#endif							/* USE_FULL_GRACE */

	/*
	 * Initialize path data
	 */
	for (i = 0, pathPtr = CSPDATA (stnID, pathData);
		i < MAX_PATH_COUNT;
		i++, pathPtr++)
	{
		result = InitCSPMIBPATHMulti (stnID, pathPtr);
		if (result != 0)
			/* assume end of array */
			break;
	}

	/*
	 * Initialize port data
	 */
	for (i = 0, portPtr = CSPDATA (stnID, portData);
		i < MAX_PORT_COUNT;
		i++, portPtr++)
	{
		result = InitCSPMIBPORTMulti (stnID, portPtr);
		if (result != 0)
			/* assume end of array */
			break;

		/* if PORT is not present, skip remaining attributes */
		if (portPtr->hwPresent == NOTPRESENT)
			continue;

		/* if PORT is not present, skip remaining attributes */
		if (portPtr->hwPresent == NOTPRESENT)
			continue;

		else
			result = InitCSPMIBPORTHWMulti (stnID, portPtr);
	}

	/*
	 * Complete initialization
	 */
	CSPDATA (stnID, stationData.CSPInitialized) = SET;

	/*
	 * Note, initial configuration selection criteria will be run by ECM.
	 */
	return (0);
}


/*
 * If the constant MULTIPLE_STATION is not defined, then the system is being
 * compiled for single station use. In this case, the following routine(s)
 * need to be defined for the older single station calling sequence.
 */
#ifndef MULTIPLE_STATION

uInt32
#ifdef __STDC__
InitCSPMIBData (void)
#else
InitCSPMIBData ()
#endif
{
	return (InitCSPMIBDataMulti (FIRST_STATION));
}

#endif							/* MULTIPLE_STATION */
