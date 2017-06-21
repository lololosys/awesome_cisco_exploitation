/* $Id: mibinit.c,v 3.2 1995/11/17 18:10:40 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/mibinit.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: mibinit.c,v $
 * Revision 3.2  1995/11/17  18:10:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:56:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:25:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_mibinit.c xm7_v3.1.1:cset.0169:15:15 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/**********************************************************************
		Management Information Base Module

		MIB Initialization Processing.

		File:			xm7_mibinit.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0169		01/12/94

		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This module is responsible for initializing the Management
		Information Base (MIB).  The module initializes the MIB and
		calls implementer initialization routines to initialize
		the platform-specific MIB attributes.

		With release 7.3, this file contains the following conditional
		compilation options:

			XM_USE_FLEX_CONF	include code for flexible configuration.
			XM_USE_VSO			include code for vendor-specific object support.
			XM_USE_VSO_MASIC	include code for MASIC object support.
			XM_USE_VSO_OTHER	include code for other vendor objects support.

		If any of the constants listed above are defined, then the
		associated portion of code is included in the compilation.

		The routine affected are those that remove and delete entities
		from the MIB and those that notify FSP and CSP of any configuration
		change. The addition and insertion routines remain in the code
		as these are needed to build the MIB during initialization.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

#include        "cschdrsif.h"
#include        "xdihdrsif.h"

#include	"fddihdr.h"
#include	"smterror.h"
#include	"mibglbl.h"
#include	"mibhdr.h"
#include	"smtmsg.h"
#include	"mibproto.h"
#include	"multistn.h"


/*********************************************************************
*
*	Initialization Routines
*
*********************************************************************/

void
#ifdef __STDC__
MarkDiscontinuity (SMTBaseAttrType * smtPtr)
#else
MarkDiscontinuity (smtPtr)
		SMTBaseAttrType *smtPtr;
#endif
/*********************************************************************
Function:	This routine clears the SetCount and TimeStamp in the MIB
			to indicate a discontinuity in the station's state. This occurs
			for flexible configurations when a MAC or PORT is added or
			deleted.
Parameters:	smtPtr	= pointer to SMT object.
Input:		smtPtr	= contains SMT object attribute values.
Output:		SetCount and TimeStamp attributes are reset to 0
Return:		None.
Modification History:
*********************************************************************/
{
	/* reset TimeStamp and SetCount */
	smtPtr->TimeStamp.hiword = 0;
	smtPtr->TimeStamp.loword = 0;
	smtPtr->SetCount.count = 0;
	smtPtr->SetCount.settimestamp.hiword = 0;
	smtPtr->SetCount.settimestamp.loword = 0;

	return;
}

uInt32
#ifdef __STDC__
InitMIB (uInt16 stnID)
#else
InitMIB (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	This routine is called during initialization processing
			to fill in the default values in the Management Information
			Base (MIB).
Parameters:	stnID	= identifier of station to initialize.
Input:		None.
Output:		Allocate MIB data base memory then initialize
			base attributes according to implementer configuration
			provided.
Return:		0				if successful.
			EMIB_INIT_MIB	if MIB memory could not be allocated.
			optionally, implementer may choose a unique value
			returned by interface rotuines to indicate a specific
			initialization fault.
Modification History:
*********************************************************************/
{
MIBType         *mibPtr;				/* MIB database */
uInt32           result;				/* return value */

	/*
	 * Allocate space for MIB through implementer routine.
	 */
	result = EMIB_INIT_MIB;
	mibPtr = CreateMIBMulti (stnID);
	if (mibPtr != NULL)
	{
		/* set up MIB pointer */
		MIBDATA (stnID, mib) = mibPtr;

		/* clear table entries */
		result = InitMIBTables (stnID);

#ifdef	XM_USE_VSO
		/*
		 * If including vendor-specific objects in the MIB, then initialize
		 * their databases.
		 */

#ifdef	XM_USE_VSO_MASIC
		if (result == 0)
			result = InitMASICObject (stnID);
#endif							/* XM_USE_VSO_MASIC */

#ifdef	XM_USE_VSO_OTHER
		if (result == 0)
			result = InitVSOMulti (stnID);
#endif							/* XM_USE_VSO_OTHER */

#endif							/* XM_USE_VSO */

		if (result)
			return (result);

		/*
		 * Initialize default SMT attribute values.
		 */
		mibPtr->smtData.AvailablePaths = AP_primary;
		mibPtr->smtData.ConfigCapabilities = Cap_None;
		mibPtr->smtData.ConfigPolicy = CFP_None;
		mibPtr->smtData.ConnectionPolicy = Con_XDI_Default;
		mibPtr->smtData.T_Notify = T_Notify_Default;
		mibPtr->smtData.StatRptPolicy = TRUE;
		mibPtr->smtData.Trace_MaxExpiration = MUsecs2Fddi (Trace_Max_Default);
		mibPtr->smtData.BypassPresent = FALSE;
		mibPtr->smtData.xdi.RMTDupCapabilities = RMT_DAC_LEAVE;
		mibPtr->smtData.xdi.ConnectMax = 0;
		mibPtr->smtData.xdi.MaxMasterCanConnect = 0;
		mibPtr->smtData.xdi.GracefulMax = 0;
		mibPtr->smtData.xdi.GracefulInsCapabilities = GIC_None;
		mibPtr->smtData.xdi.GracefulInsMode = GIM_off;
		mibPtr->smtData.xdi.GracefulInsMACID = 0;

		/* set default packages */
		mibPtr->smtData.xdi.packages = (
			PKG_manufacturerdata |
			PKG_parametermanagement |
			PKG_smtvendorspecific);

		/*
		 * Get implementation dependent values for SMT attributes. User adds
		 * all manageable objects to MIB later.
		 */
		result = BuildSMTMIBMulti (stnID, mibPtr);
		if (!result)
		{
			/* initialize base SMT attributes */
			mibPtr->smtData.OpVersionId = SMTOPVERSION;
			mibPtr->smtData.HiVersionId = SMTHIVERSION;
			mibPtr->smtData.LoVersionId = SMTLOVERSION;
			mibPtr->smtData.MIBVersionId = SMTMIBVERSIONID;
			mibPtr->smtData.Mac_Ct = 0;
			mibPtr->smtData.NonMaster_Ct = 0;
			mibPtr->smtData.Master_Ct = 0;
			mibPtr->smtData.ECMState = EC_OUT;
			mibPtr->smtData.CF_State = CF_isolated;
			mibPtr->smtData.RemoteDisconnectFlag = FALSE;
			mibPtr->smtData.StationStatus = Status_separated;
			mibPtr->smtData.PeerWrapFlag = FALSE;
			mibPtr->smtData.TimeStamp.hiword = 0;
			mibPtr->smtData.TimeStamp.loword = 0;
			mibPtr->smtData.TransitionTimeStamp.hiword = 0;
			mibPtr->smtData.TransitionTimeStamp.loword = 0;

			/* Set first six bytes of Manufacturer Data field. */
			MEMCOPY (mibPtr->smtData.ManufacturerData.manufacturerData,
				XDIMANUFDATA, 6);

			/* SetCount */
			mibPtr->smtData.SetCount.count = 0;
			mibPtr->smtData.SetCount.settimestamp.hiword = 0;
			mibPtr->smtData.SetCount.settimestamp.loword = 0;
			MEMCOPY (&mibPtr->smtData.LastSetStationId,
				&mibPtr->smtData.StationId, SIZE_SMTStationIdType);

			/* force XDI SMT attribute values */
			mibPtr->smtData.xdi.Port_Ct = 0;
			mibPtr->smtData.xdi.ABS_present = NONE_PRESENT;
			mibPtr->smtData.xdi.syncBandwidth = 0;
			mibPtr->smtData.xdi.Masic_Ct = 0;

			/*
			 * If MULTIPLE_STATION is defined, then use the value of
			 * MAX_STATION_COUNT. Otherwise, force station count to 1.
			 */
#ifdef	MULTIPLE_STATION
			mibPtr->smtData.xdi.Station_Ct = MAX_STATION_COUNT;
#else							/* MULTIPLE_STATION */
			mibPtr->smtData.xdi.Station_Ct = 1;
#endif							/* MULTIPLE_STATION */

			/* force vendor specific attributes for XDI */
			mibPtr->smtData.xdi.packages |= PKG_smtvendorspecific;

			/* complete MIB initialization */
			result = BuildMACMIBMulti (stnID);

			if (result == 0)
				result = BuildPORTMIBMulti (stnID);

			if (result == 0)
				result = BuildPATHMIBMulti (stnID);

#ifdef	XM_USE_VSO
			/*
			 * If including vendor-specific objects in the MIB, then perform
			 * their installation.
			 */

#ifdef	XM_USE_VSO_MASIC
			if (result == 0)
				result = BuildMASICMIBMulti (stnID);
#endif							/* XM_USE_VSO_MASIC */

#ifdef	XM_USE_VSO_OTHER
			if (result == 0)
				result = BuildVSOMIBMulti (stnID);
#endif							/* XM_USE_VSO_OTHER */

#endif							/* XM_USE_VSO */

			if (result == 0)
				result = BuildConfigMulti (stnID);
		}
	}

	return (result);
}


/*********************************************************************
*
*	MIB Maintenance Routines
*
*********************************************************************/

uInt32
#ifdef __STDC__
AddMACObjectMulti (uInt16 stnID, uInt16 rsrcID, MACDataPtr * macPtr)
#else
AddMACObjectMulti (stnID, rsrcID, macPtr)
		uInt16           stnID;
		uInt16           rsrcID;
		MACDataPtr      *macPtr;
#endif
/*********************************************************************
Function:	This routine is called by the implementer at any time a
			new MAC object is to be added to the resource list. This
			occurs during initialization processing and when new
			devices are added. The routine places the MAC's MIB
			information into the MIB.
Parameters:	stnID	= index of station to use.
			rsrcID	= the value for this MAC's fddiMACIndex attribute.
			macPtr	= returned address to MIB entry for MAC.
Input:		macPtr	= pointer to hold address of MIB entry for MAC.
Output:		Adds resource ID into MAC resource table and initializes
			base attributes.
			macPtr	= pointer to entry in MIB for the MAC.
				Implementor must fill in user-specified values
				(see manual and BuildMACMIBMulti()).
Return:		0				if successful.
			EMIB_TABLE_FULL	if resource table is full.
			EMIB_DUP_ID		if resource ID is already in use.
			EMIB_INVALID_ID	if resource ID is 0.
Modification History:
*********************************************************************/
{
Int16            index;					/* table index */
uInt16           i;						/* loop counter */
MIBType         *mibPtr;				/* MIB data base */
MACDataPtr       base;					/* MAC base element */
uInt32           result = 0;			/* return value */

	/* get MIB */
	mibPtr = MIBDATA (stnID, mib);
	*macPtr = (MACDataPtr) NULL;

	/* check for room in table */
	if (mibPtr->smtData.Mac_Ct >= MAX_MAC_COUNT)
		result = EMIB_TABLE_FULL;

	/* check for valid ID */
	else if (rsrcID == 0)
		result = EMIB_INVALID_ID;

	else
	{
		/* check if resource ID is in the table */
		index = MFindResourceID (rsrcID, mibPtr->macTable,
			mibPtr->smtData.Mac_Ct);
		if (index >= 0)
			/* index found so ID is in table */
			result = EMIB_DUP_ID;

		else
		{
			/* add ID to table */
			index = MAddResourceID (rsrcID, mibPtr->macTable,
				mibPtr->smtData.Mac_Ct);

			/* locate open space in data list */
			for (i = 0, base = &mibPtr->macBase[0];
				i < MAX_MAC_COUNT;
				i++, base++)
			{
				if (!base->inUse)
				{
					base->inUse = TRUE;
					mibPtr->macTable[index].index = i;
					break;
				}
			}

			/* set index to data table entry */
			index = i;

			/* increment MAC count */
			mibPtr->smtData.Mac_Ct++;

			/* initialize base attributes */
			base->baseData.RequestedPaths.reqPaths = RP_MAC_DEFAULT;
			base->baseData.FrameErrorThreshold = 0;
			base->baseData.HardwarePresent = NOTPRESENT;
			base->baseData.MA_UnitdataEnable = TRUE;
			base->baseData.Index = rsrcID;

			/* optional attributes */
			base->baseData.NotCopiedThreshold = 0;

			/* XDI attributes */
			base->baseData.xdi.xdiIndex = index;
			base->baseData.xdi.RMTDupPolicy = RMT_DUP_LEAVE;
			base->baseData.xdi.localPath = PI_local;
			for (i = 0; i < MAX_PATH_COUNT; i++)
			{
				base->baseData.xdi.configList[i].nextIndex = index;
				base->baseData.xdi.configList[i].nextType = RT_mac;
				base->baseData.xdi.configList[i].prevIndex = index;
				base->baseData.xdi.configList[i].prevType = RT_mac;
			}

			/* force vendor specific attributes for XDI */
			base->baseData.xdi.packages |=
				PKG_macvendorspecific;

			*macPtr = base;

			/* reset TimeStamp and SetCount */
			MarkDiscontinuity (&mibPtr->smtData);
		}
	}

	return (result);
}

uInt32
#ifdef __STDC__
AddPATHObjectMulti (uInt16 stnID, uInt16 rsrcID, PATHDataPtr * pathPtr)
#else
AddPATHObjectMulti (stnID, rsrcID, pathPtr)
		uInt16           stnID;
		uInt16           rsrcID;
		PATHDataPtr     *pathPtr;
#endif
/*********************************************************************
Function:	This routine is called by the implementer at any time a
			new PATH object is to be added to the resource list. This
			occurs during initialization processing and when new
			devices are added. The routine places the PATH's MIB
			information into the MIB.
Parameters:	stnID	= index of station to use.
			rsrcID	= the value for this PATH's fddiPATHIndex attribute.
			pathPtr	= returned address to MIB entry for PATH.
Input:		pathPtr	= pointer to hold address of MIB entry for PATH.
Output:		Adds resource ID into PATH resource table and initializes
			base attributes.
			pathPtr	= pointer to MIB entry for this PATH.
				Implementor must fill in user-specified values
				(see manual).
Return:		0				if successful.
			EMIB_TABLE_FULL	if resource table is full.
			EMIB_DUP_ID		if resource ID is already in use.
			EMIB_INVALID_ID	if resource ID cannot be used.
Notes:		Multiple local paths as allowed by the SMT document
			may be supported by the index numbering
			scheme defined by the standard. The primary path MUST
			use index 1 and the secondary path MUST use index 2.
			Local paths may use any index of 3 or greater.
				All paths must be added during initialization.
			The XLNT Manager(tm) does NOT manage addition, insertion,
			removal, or deletion of paths during SMT operation. To
			change the path resources, the station must be reset and
			reinitialized.
				Path indexes may range from 1 to MAX_PATH_COUNT.
Modification History:
*********************************************************************/
{
Int16            index;					/* table index */
MIBType         *mibPtr;				/* MIB data base */
PATHDataPtr      base;					/* PATH base element */
uInt32           result = 0;			/* return value */

	/* get MIB */
	mibPtr = MIBDATA (stnID, mib);

	/* get index into table */
	if ((rsrcID < 1) || (rsrcID > MAX_PATH_COUNT))
		/* index found so ID is in table */
		result = EMIB_INVALID_ID;

	else
	{
		/* get table element */
		index = rsrcID - 1;
		base = &mibPtr->pathBase[index];

		/* if already in use */
		if (base->inUse)
			/* duplicate PATH ID */
			result = EMIB_DUP_ID;

		else
		{
			/* initialize base attributes */
			base->baseData.Index = rsrcID;
			/* configuration is built when requested */
			base->baseData.TVXLowerBound = 0;
			base->baseData.T_MaxLowerBound = 0;
			base->baseData.MaxT_Req = 0;

			/* optional attributes */
			base->baseData.RingLatency = 0;
			base->baseData.TraceStatus = TS_None;
			base->baseData.SbaPayload = 0;
			base->baseData.SbaOverhead = 0;
			base->baseData.T_Rmode = 0;
			base->baseData.SbaAvailable = 0;

			/* XDI attributes */
			base->baseData.xdi.xdiIndex = index;
			base->inUse = TRUE;

			/* force vendor specific attributes for XDI */
			base->baseData.xdi.packages |= (
				PKG_tracestatus |
				PKG_restrictedtoken |
				PKG_pathvendorspecific);

			*pathPtr = base;
		}
	}

	return (result);
}

uInt32
#ifdef __STDC__
AddPORTObjectMulti (uInt16 stnID, uInt16 rsrcID, FDDIPortType type,
	PORTDataPtr * portPtr)
#else
AddPORTObjectMulti (stnID, rsrcID, type, portPtr)
		uInt16           stnID;
		uInt16           rsrcID;
		FDDIPortType     type;
		PORTDataPtr     *portPtr;
#endif
/*********************************************************************
Function:	This routine is called by the implementer at any time a
			new PORT object is to be added to the resource list. This
			occurs during initialization processing and when new
			devices are added. The routine places the PORT's MIB
			information into the MIB.
Parameters:	stnID	= index of station to use
			rsrcID	= the value for this PORT's fddiPORTIndex attribute
			portPtr	= returned address to MIB entry for PORT.
Input:		portPtr	= pointer to hold address of MIB entry for PORT.
Output:		Adds resource ID into PORT resource table and initializes
			base attributes.
			portPtr	= pointer to MIB entry for this PORT.
						Implementor must fill in user-specified values
						(see manual).
Return:		0						if successful.
			EMIB_TABLE_FULL			if resource table is full.
			EMIB_DUP_ID				if resource ID is already in use.
			EMIB_INVALID_PORT_TYPE	if port type is not A, B, S or M
									OR if too many NonMaster port types
									would exist in the MIB.
			EMIB_INVALID_ID			if resource ID is 0.
Modification History:
*********************************************************************/
{
Int16            index;					/* table index */
uInt16           i;						/* loop counter */
MIBType         *mibPtr;				/* MIB data base */
PORTDataPtr      base;					/* PORT base element */
uInt32           result = 0;			/* return value */

	/* get MIB */
	mibPtr = MIBDATA (stnID, mib);

	/* check for room in table */
	if (mibPtr->smtData.xdi.Port_Ct >= MAX_PORT_COUNT)
		result = EMIB_TABLE_FULL;

	/* check for valid ID */
	else if (rsrcID == 0)
		result = EMIB_INVALID_ID;

	else
	{
		/* check if resource ID is in the table */
		index = MFindResourceID (rsrcID, mibPtr->portTable,
			mibPtr->smtData.xdi.Port_Ct);
		if (index >= 0)
			/* index found so ID is in table */
			result = EMIB_DUP_ID;

		else
		{
			/* validate port type */
			if (((type == PT_a) && (mibPtr->smtData.xdi.ABS_present
						& (A_PRESENT | S_PRESENT)))
				|| ((type == PT_b)
					&& (mibPtr->smtData.xdi.ABS_present
						& (B_PRESENT | S_PRESENT)))
				|| ((type == PT_s)
					&& (mibPtr->smtData.xdi.ABS_present
						& (A_PRESENT | B_PRESENT | S_PRESENT))))
				result = EMIB_INVALID_PORT_TYPE;
		}
	}

	if (!result)
	{
		/* add ID to table */
		index = MAddResourceID (rsrcID, mibPtr->portTable,
			mibPtr->smtData.xdi.Port_Ct);

		/* locate open space in data list */
		for (i = 0, base = &mibPtr->portBase[0];
			i < MAX_PORT_COUNT;
			i++, base++)
		{
			if (!base->inUse)
			{
				base->inUse = TRUE;
				mibPtr->portTable[index].index = i;
				break;
			}
		}

		/* set index to data table entry */
		index = i;

		/* initialize base attributes */
		base->baseData.My_Type = type;
		base->baseData.ConnectionPolicies = CP_None;
		base->baseData.Index = rsrcID;
		base->baseData.Ler_Cutoff = LER_CUTOFF_DEFAULT;
		base->baseData.Ler_Alarm = LER_ALARM_DEFAULT;
		base->baseData.HardwarePresent = NOTPRESENT;

		/* set default requested paths */
		switch (type)
		{
		case PT_a:
			base->baseData.RequestedPaths.reqPaths[RP_none]
				= RP_A_NONE_DEFAULT;
			base->baseData.RequestedPaths.reqPaths[RP_tree]
				= RP_A_TREE_DEFAULT;
			base->baseData.RequestedPaths.reqPaths[RP_peer]
				= RP_A_PEER_DEFAULT;
			mibPtr->smtData.xdi.ABS_present |= A_PRESENT;
			mibPtr->smtData.NonMaster_Ct++;
			break;

		case PT_b:
			base->baseData.RequestedPaths.reqPaths[RP_none]
				= RP_B_NONE_DEFAULT;
			base->baseData.RequestedPaths.reqPaths[RP_tree]
				= RP_B_TREE_DEFAULT;
			base->baseData.RequestedPaths.reqPaths[RP_peer]
				= RP_B_PEER_DEFAULT_SM;
			mibPtr->smtData.xdi.ABS_present |= B_PRESENT;
			mibPtr->smtData.NonMaster_Ct++;
			break;

		case PT_s:
			base->baseData.RequestedPaths.reqPaths[RP_none]
				= RP_S_NONE_DEFAULT;
			base->baseData.RequestedPaths.reqPaths[RP_tree]
				= RP_S_TREE_DEFAULT;
			base->baseData.RequestedPaths.reqPaths[RP_peer]
				= RP_S_PEER_DEFAULT;
			mibPtr->smtData.xdi.ABS_present |= S_PRESENT;
			mibPtr->smtData.NonMaster_Ct++;
			break;

		case PT_m:
			base->baseData.RequestedPaths.reqPaths[RP_none]
				= RP_M_NONE_DEFAULT;
			base->baseData.RequestedPaths.reqPaths[RP_tree]
				= RP_M_TREE_DEFAULT;
			base->baseData.RequestedPaths.reqPaths[RP_peer] = 0;
			mibPtr->smtData.Master_Ct++;
			break;
		}

		/* optional attributes */
		base->baseData.Maint_LS = LS_qls;
		base->baseData.MACLoop_Time = 0;

		/* XDI attributes */
		base->baseData.xdi.xdiIndex = index;
		base->baseData.xdi.localPath = PI_local;
		for (i = 0; i < MAX_PATH_COUNT; i++)
		{
			base->baseData.xdi.configList[i].nextIndex = index;
			base->baseData.xdi.configList[i].nextType = RT_port;
			base->baseData.xdi.configList[i].prevIndex = index;
			base->baseData.xdi.configList[i].prevType = RT_port;
		}

		/* force vendor specific attributes for XDI */
		base->baseData.xdi.packages |= (
			PKG_maint_LS |
			PKG_maint_LSmanagement |
			PKG_portvendorspecific);

		/* increment PORT count */
		mibPtr->smtData.xdi.Port_Ct++;

		*portPtr = base;

		/* reset TimeStamp and SetCount */
		MarkDiscontinuity (&mibPtr->smtData);
	}

	return (result);
}

uInt32
#ifdef __STDC__
InsertMACObjectMulti (uInt16 stnID, uInt16 rsrcID, MACDataPtr * macPtr)
#else
InsertMACObjectMulti (stnID, rsrcID, macPtr)
		uInt16           stnID;
		uInt16           rsrcID;
		MACDataPtr      *macPtr;
#endif
/*********************************************************************
Function:	This routine is called by the implementer at any time a
			MAC object is physically available to the station. This
			occurs after adding a new resource or when a resource is
			re-inserted after it had been removed.
Parameters:	stnID	= index of station to use
			rsrcID	= the value for this MAC's fddiMACIndex attribute
			macPtr	= address of the memory space for the MAC's
						MIB table entry.
Input:		None.
Output:		Set HardwarePresent to PRESENT for this resource.
			macPtr	= address of MIB table entry.
Return:		0					if successful.
			EMIB_ID_NOT_FOUND	Resource ID is not in MIB.
Modification History:
*********************************************************************/
{
Int16            index;					/* array index */
MIBType         *mibPtr;				/* MIB data base */
MACBaseAttrType *base;					/* base attributes */
MACHWAttrType   *hw;
uInt32           result = 0;			/* return value */

	/* get MIB */
	mibPtr = MIBDATA (stnID, mib);

	/* check if resource ID is in the table */
	index = MFindResourceID (rsrcID, mibPtr->macTable,
		mibPtr->smtData.Mac_Ct);
	if (index < 0)
		/* index not found */
		result = EMIB_ID_NOT_FOUND;

	else
	{
		/* map index to MAC attributes */
		index = mibPtr->macTable[index].index;

		/* initialize values */
		base = &mibPtr->macBase[index].baseData;
		hw = &mibPtr->macBase[index].hwData;
		hw->FrameStatusFunctions = FS_None;
		hw->AvailablePaths = AP_primary;
		hw->CurrentPath = CP_isolated;
		MCopyAddress (hw->UpstreamNbr, SMT_Unknown_Address);
		MCopyAddress (hw->DownstreamNbr, SMT_Unknown_Address);
		MCopyAddress (hw->OldUpstreamNbr, SMT_Unknown_Address);
		MCopyAddress (hw->OldDownstreamNbr, SMT_Unknown_Address);
		hw->DupAddressTest = DAT_none;
		hw->DownstreamPORTType = PT_none;
		hw->T_Req = 0;
		hw->T_Neg = 0;
		hw->T_Max = 0;
		hw->TvxValue = 0;
		hw->Frame_Ct = 0;
		hw->Copied_Ct = 0;
		hw->Transmit_Ct = 0;
		hw->Error_Ct = 0;
		hw->Lost_Ct = 0;
		hw->FrameErrorRatio = 0;
		hw->RMTState = RM_ISOLATED;
		hw->DA_Flag = FALSE;
		hw->UNDA_Flag = FALSE;
		hw->FrameErrorFlag = FALSE;
		hw->MA_UnitdataAvailable = FALSE;
		hw->BridgeFunctions = BT_None;
		hw->T_Pri0 = 0;
		hw->T_Pri1 = 0;
		hw->T_Pri2 = 0;
		hw->T_Pri3 = 0;
		hw->T_Pri4 = 0;
		hw->T_Pri5 = 0;
		hw->T_Pri6 = 0;
		hw->Token_Ct = 0;
		hw->TvxExpired_Ct = 0;
		hw->NotCopied_Ct = 0;
		hw->Late_Ct = 0;
		hw->RingOp_Ct = 0;
		hw->NotCopiedRatio = 0;
		hw->NotCopiedFlag = FALSE;
		base->HardwarePresent = PRESENT;

		/* set XDI values */
		hw->xdi.ConfigurationStatus = XDIMAC_configstatus_success;
		hw->xdi.SM_MA_AvailFlag = FALSE;
		hw->xdi.neighborFlags = NAC_None;
		hw->xdi.lastLost = 0;
		hw->xdi.lastError = 0;
		hw->xdi.lastFrame = 0;
		hw->xdi.lastNotCopied = 0;
		hw->xdi.lastCopied = 0;
		MCopyAddress (hw->xdi.oldSMTAddress, SMT_Unknown_Address);

		/* reset TimeStamp and SetCount */
		MarkDiscontinuity (&mibPtr->smtData);

		/* set return pointer value */
		*macPtr = &mibPtr->macBase[index];
	}

	return (result);
}

uInt32
#ifdef __STDC__
InsertPORTObjectMulti (uInt16 stnID, uInt16 rsrcID, PORTDataPtr * portPtr)
#else
InsertPORTObjectMulti (stnID, rsrcID, portPtr)
		uInt16           stnID;
		uInt16           rsrcID;
		PORTDataPtr     *portPtr;
#endif
/*********************************************************************
Function:	This routine is called by the implementer at any time a
			PORT object is physically available to the station. This
			occurs after adding a new resource or when a resource is
			re-inserted after it had been removed.
Parameters:	stnID	= index of station to use
			rsrcID	= the value for this PORT's fddiPORTIndex attribute
			portPtr	= address of pointer to hold location of PORT's
					MIB table entry.
Input:		None.
Output:		Set HardwarePresent to NOTPRESENT for this resource.
			portPtr	= address of PORT's MIB table entry.
Return:		0					if successful.
			EMIB_ID_NOT_FOUND	Resource ID is not in MIB.
Modification History:
*********************************************************************/
{
Int16            index;					/* array index */
MIBType         *mibPtr;				/* MIB data base */
PORTBaseAttrType *base;					/* base attributes */
PORTHWAttrType  *hw;					/* hw attributes */
uInt32           result = 0;			/* return value */

	/* get MIB */
	mibPtr = MIBDATA (stnID, mib);

	/* check if resource ID is in the table */
	index = MFindResourceID (rsrcID, mibPtr->portTable,
		mibPtr->smtData.xdi.Port_Ct);
	if (index < 0)
		/* index not found */
		result = EMIB_ID_NOT_FOUND;

	else
	{
		/* map index to PORT attributes */
		index = mibPtr->portTable[index].index;

		/* initialize if locally accessible */
		base = &mibPtr->portBase[index].baseData;
		hw = &mibPtr->portBase[index].hwData;
		hw->NeighborType = PT_none;
		hw->MACIndicated.t_val9 = FALSE;
		hw->MACIndicated.r_val9 = FALSE;
		hw->CurrentPath = CP_isolated;
		hw->MACPlacement = 0;
		hw->AvailablePaths =
			((base->My_Type == PT_a) || (base->My_Type == PT_b))
		/* primary and secondary needed for A and B */
			? AP_primary | AP_secondary
		/* otherwise, just primary */
			: AP_primary;
		hw->PMDClass = PMD_multimode;
		hw->ConnectionCapabilities = CP_None;
		hw->BS_Flag = FALSE;
		hw->LCTFail_Ct = 0;
		/* limit estimate to constraints */
		hw->Ler_Estimate = (base->Ler_Alarm < LER_MAX)
			? base->Ler_Alarm + 1 : base->Ler_Alarm;
		hw->Lem_Reject_Ct = 0;
		hw->Lem_Ct = 0;
		hw->ConnectState = CS_disabled;
		hw->PCMState = PC_OFF;
		hw->PC_Withhold = PCW_none;
		hw->LerFlag = FALSE;
		hw->EBError_Ct = 0;
		hw->PC_LS = LS_qls;
		base->HardwarePresent = PRESENT;

		/* XDI attributes */
		hw->xdi.EBErrorFlag = FALSE;
		MCopyAddress (hw->xdi.UpstreamMACNeighbor, SMT_Unknown_Address);
		MCopyAddress (hw->xdi.OldUpstreamMACNeighbor, SMT_Unknown_Address);
		hw->xdi.lastEB = 0;

		/* reset TimeStamp and SetCount */
		MarkDiscontinuity (&mibPtr->smtData);

		/* set return value */
		*portPtr = &mibPtr->portBase[index];
	}

	return (result);
}

/*
 * Only include the routines to remove and delete entities from the MIB
 * if the XM_USE_FLEX_CONF constant is defined. The routines affected
 * are: RemoveMACObjectMulti(), RemovePORTObjectMulti(),
 * DeleteMACObjectMulti() and DeletePORTObjectMulti().
 */
#ifdef XM_USE_FLEX_CONF

uInt32
#ifdef __STDC__
RemoveMACObjectMulti (uInt16 stnID, uInt16 rsrcID, Int16 * xdiID)
#else
RemoveMACObjectMulti (stnID, rsrcID, xdiID)
		uInt16           stnID;
		uInt16           rsrcID;
		Int16           *xdiID;
#endif
/*********************************************************************
Function:	This routine is called by the implementer at any time a
			MAC object is physically unavailable to the station, but
			will remain manageable.
Parameters:	stnID	= index of station to use
			rsrcID	= the value for this MAC's fddiMACIndex attribute
			xdiID	= buffer for the internal index
Input:		None.
Output:		Set HardwarePresent to NOTPRESENT for this resource.
			xdiID	= set to the internal table array index. This value
						is used in the subsequent call to
						NotifyRemoveMACMulti().
Return:		0					if successful.
			EMIB_ID_NOT_FOUND	Resource ID is not in MIB.
Modification History:
*********************************************************************/
{
Int16            index;					/* array index */
MIBType         *mibPtr;				/* MIB data base */
uInt32           result = 0;			/* return value */

	/* get MIB */
	mibPtr = MIBDATA (stnID, mib);

	/* check if resource ID is in the table */
	index = MFindResourceID (rsrcID, mibPtr->macTable,
		mibPtr->smtData.Mac_Ct);
	if (index < 0)
		/* index not found */
		result = EMIB_ID_NOT_FOUND;

	else
	{
		/* map index to MAC attributes */
		index = mibPtr->macTable[index].index;

		/* set return index value */
		*xdiID = index;

		/* set HW not present */
		mibPtr->macBase[index].baseData.HardwarePresent = NOTPRESENT;

		/* reset TimeStamp and SetCount */
		MarkDiscontinuity (&mibPtr->smtData);
	}

	return (result);
}

uInt32
#ifdef __STDC__
RemovePORTObjectMulti (uInt16 stnID, uInt16 rsrcID, Int16 * xdiID)
#else
RemovePORTObjectMulti (stnID, rsrcID, xdiID)
		uInt16           stnID;
		uInt16           rsrcID;
		Int16           *xdiID;
#endif
/*********************************************************************
Function:	This routine is called by the implementer at any time a
			PORT object is physically unavailable to the station, but
			will remain manageable.
Parameters:	stnID	= index of station to use
			rsrcID	= the value for this PORT's fddiPORTIndex attribute
			xdiID	= buffer for the internal index
Input:		None.
Output:		Set HardwarePresent to NOTPRESENT for this resource.
			xdiID	= set to the internal table array index. This value
						is used in the subsequent call to
						NotifyRemovePORTMulti().
Return:		0					if successful.
			EMIB_ID_NOT_FOUND	Resource ID is not in MIB.
Modification History:
*********************************************************************/
{
Int16            index;					/* array index */
MIBType         *mibPtr;				/* MIB data base */
uInt32           result = 0;			/* return value */

	/* get MIB */
	mibPtr = MIBDATA (stnID, mib);

	/* check if resource ID is in the table */
	index = MFindResourceID (rsrcID, mibPtr->portTable,
		mibPtr->smtData.xdi.Port_Ct);
	if (index < 0)
		/* index not found */
		result = EMIB_ID_NOT_FOUND;

	else
	{
		/* map index to PORT attributes */
		index = mibPtr->portTable[index].index;

		/* set return index value */
		*xdiID = index;

		/* set HW not present */
		mibPtr->portBase[index].baseData.HardwarePresent = NOTPRESENT;

		/* reset TimeStamp and SetCount */
		MarkDiscontinuity (&mibPtr->smtData);
	}

	return (result);
}

uInt32
#ifdef __STDC__
DeleteMACObjectMulti (uInt16 stnID, uInt16 rsrcID, Int16 * xdiID)
#else
DeleteMACObjectMulti (stnID, rsrcID, xdiID)
		uInt16           stnID;
		uInt16           rsrcID;
		Int16           *xdiID;
#endif
/*********************************************************************
Function:	This routine is called by the implementer at any time a
			MAC object is to be removed from the MIB.
Parameters:	stnID	= index of station to use
			rsrcID	= the value for this MAC's fddiMACIndex attribute
			xdiID	= buffer for the internal index
Input:		None.
Output:		MAC table has resource ID removed.
			xdiID	= set to the internal table array index. This value
						is used in the subsequent call to
						NotifyDeleteMACMulti().
Return:		0					if successful.
			EMIB_ID_NOT_FOUND	Resource ID is not in MIB.
Modification History:
*********************************************************************/
{
Int16            index;					/* array index */
Int16            i;						/* loop control */
MIBType         *mibPtr;				/* MIB data base */
MACDataPtr       macPtr;				/* MAC entry */
PathConfigType  *cfgPtr;				/* config list */
uInt32           result = 0;			/* return value */

	/* get MIB */
	mibPtr = MIBDATA (stnID, mib);

	/* check if resource ID is in the table */
	index = MFindResourceID (rsrcID, mibPtr->macTable,
		mibPtr->smtData.Mac_Ct);
	if (index < 0)
		/* index not found */
		result = EMIB_ID_NOT_FOUND;

	else
	{
		/* map index to MAC attributes */
		index = mibPtr->macTable[index].index;
		macPtr = &mibPtr->macBase[index];

		/* set return index value */
		*xdiID = index;

		/* remove resource from paths */
		for (i = 0; i < MAX_PATH_COUNT; i++)
		{
			/* next's prev = prev */
			index = macPtr->baseData.xdi.configList[i].nextIndex;
			cfgPtr = (macPtr->baseData.xdi.configList[i].nextType == RT_mac)
				? &mibPtr->macBase[index].baseData.xdi.configList[i]
				: &mibPtr->portBase[index].baseData.xdi.configList[i];
			cfgPtr->prevIndex = macPtr->baseData.xdi.configList[i].prevIndex;
			cfgPtr->prevType = macPtr->baseData.xdi.configList[i].prevType;

			/* prev's next = next */
			index = macPtr->baseData.xdi.configList[i].prevIndex;
			cfgPtr = (macPtr->baseData.xdi.configList[i].prevType == RT_mac)
				? &mibPtr->macBase[index].baseData.xdi.configList[i]
				: &mibPtr->portBase[index].baseData.xdi.configList[i];
			cfgPtr->nextIndex = macPtr->baseData.xdi.configList[i].nextIndex;
			cfgPtr->nextType = macPtr->baseData.xdi.configList[i].nextType;
		}

		/* remove resource from table */
		MDeleteResourceID (rsrcID, mibPtr->macTable,
			mibPtr->smtData.Mac_Ct);
		mibPtr->smtData.Mac_Ct--;
		macPtr->inUse = FALSE;

		/* reset TimeStamp and SetCount */
		MarkDiscontinuity (&mibPtr->smtData);
	}

	return (result);
}

uInt32
#ifdef __STDC__
DeletePORTObjectMulti (uInt16 stnID, uInt16 rsrcID, Int16 * xdiID)
#else
DeletePORTObjectMulti (stnID, rsrcID, xdiID)
		uInt16           stnID;
		uInt16           rsrcID;
		Int16           *xdiID;
#endif
/*********************************************************************
Function:	This routine is called by the implementer at any time a
			PORT object is physically unavailable to the station, but
			will remain manageable.
Parameters:	stnID	= index of station to use
			rsrcID	= the value for this PORT's fddiPORTIndex attribute
			xdiID	= buffer for the internal index
Input:		None.
Output:		PORT table has resource ID removed.
			xdiID	= set to the internal table array index. This value
						is used in the subsequent call to
						NotifyDeletePORTMulti().
Return:		0					if successful.
			EMIB_ID_NOT_FOUND	Resource ID is not in MIB.
Modification History:
*********************************************************************/
{
Int16            index;					/* array index */
Int16            i;						/* loop control */
MIBType         *mibPtr;				/* MIB data base */
PORTDataPtr      portPtr;				/* PORT entry */
PathConfigType  *cfgPtr;				/* config list */
uInt32           result = 0;			/* return value */

	/* get MIB */
	mibPtr = MIBDATA (stnID, mib);

	/* check if resource ID is in the table */
	index = MFindResourceID (rsrcID, mibPtr->portTable,
		mibPtr->smtData.xdi.Port_Ct);
	if (index < 0)
		/* index not found */
		result = EMIB_ID_NOT_FOUND;

	else
	{
		/* map index to PORT attributes */
		index = mibPtr->portTable[index].index;
		portPtr = &mibPtr->portBase[index];

		/* set return index value */
		*xdiID = index;

		/* delete resource */
		switch (portPtr->baseData.My_Type)
		{
		case PT_a:
			mibPtr->smtData.NonMaster_Ct--;
			mibPtr->smtData.xdi.ABS_present &= (~A_PRESENT);
			break;

		case PT_b:
			mibPtr->smtData.NonMaster_Ct--;
			mibPtr->smtData.xdi.ABS_present &= (~B_PRESENT);
			break;

		case PT_s:
			mibPtr->smtData.NonMaster_Ct--;
			mibPtr->smtData.xdi.ABS_present &= (~S_PRESENT);
			break;

		default:
			mibPtr->smtData.Master_Ct--;
			break;
		}

		/* remove resource from paths */
		for (i = 0; i < MAX_PATH_COUNT; i++)
		{
			/* next's prev = prev */
			index = portPtr->baseData.xdi.configList[i].nextIndex;
			cfgPtr = (portPtr->baseData.xdi.configList[i].nextType == RT_mac)
				? &mibPtr->macBase[index].baseData.xdi.configList[i]
				: &mibPtr->portBase[index].baseData.xdi.configList[i];
			cfgPtr->prevIndex = portPtr->baseData.xdi.configList[i].prevIndex;
			cfgPtr->prevType = portPtr->baseData.xdi.configList[i].prevType;

			/* prev's next = next */
			index = portPtr->baseData.xdi.configList[i].prevIndex;
			cfgPtr = (portPtr->baseData.xdi.configList[i].prevType == RT_mac)
				? &mibPtr->macBase[index].baseData.xdi.configList[i]
				: &mibPtr->portBase[index].baseData.xdi.configList[i];
			cfgPtr->nextIndex = portPtr->baseData.xdi.configList[i].nextIndex;
			cfgPtr->nextType = portPtr->baseData.xdi.configList[i].nextType;
		}

		MDeleteResourceID (rsrcID, mibPtr->portTable,
			mibPtr->smtData.xdi.Port_Ct);
		mibPtr->smtData.xdi.Port_Ct--;
		portPtr->inUse = FALSE;

		/* reset TimeStamp and SetCount */
		MarkDiscontinuity (&mibPtr->smtData);
	}

	return (result);
}

#endif							/* XM_USE_FLEX_CONF */

uInt32
#ifdef __STDC__
AddObjectPATHMulti (uInt16 stnID, uInt16 pathID, uInt16 rsrcID, uInt16 rsrcType,
	uInt16 nextRsrc, uInt16 nextType,
	uInt16 prevRsrc, uInt16 prevType)
#else
AddObjectPATHMulti (stnID, pathID, rsrcID, rsrcType, nextRsrc, nextType,
	prevRsrc, prevType)
		uInt16           stnID;
		uInt16           pathID;
		uInt16           rsrcID;
		uInt16           rsrcType;
		uInt16           nextRsrc;
		uInt16           nextType;
		uInt16           prevRsrc;
		uInt16           prevType;
#endif
/*********************************************************************
Function:	This routine is called by the implementer at any time
			to add an object into a PATH.
Parameters:	stnID		= index of station to use
			pathID		= the resource ID of the PATH
			rsrcID		= the resource ID to add
			rsrcType	= type of object to add
			nextRsrc	= resource ID of next object on path
			nextType	= type of next object
			prevRsrc	= resource ID of previous object on path
			prevType	= type of previos object
Input:		Uses current MIB PORT and MAC table entries.
Output:		Updates previous and next pointers in affected PORT and
			MAC table entries.
Return:		0					if successful.
			EMIB_ID_NOT_FOUND	Resource ID is not in MIB.
Modification History:
*********************************************************************/
{
Int16            index;					/* array index */
MIBType         *mibPtr;				/* MIB data base */
uInt32           result = 0;			/* return value */
PATHDataPtr      base;					/* PATH base element */
PathConfigType  *rsrcConfig;			/* resource object config element */
PathConfigType  *newPrev;				/* requested previous object */
PathConfigType  *newNext;				/* requested next object */
PathConfigType  *oldPrev;				/* old previous object */
PathConfigType  *oldNext;				/* old next object */
Int16            next;					/* next object index */
Int16            prev;					/* previous object index */
Int16            rsrc;					/* resource object index */

	/**
	 * WRF
	 * Initialize some local variables to avoid compiler warnings.
	 **/
	index = 0;
	rsrcConfig = NULL;
	newNext = NULL;
	newPrev = NULL;

	/* get MIB */
	mibPtr = MIBDATA (stnID, mib);

	/* get index into table */
	if ((pathID < 1) || (pathID > MAX_PATH_COUNT))
		/* index found so ID is in table */
		result = EMIB_ID_NOT_FOUND;

	else
	{
		/* get table element */
		index = pathID - 1;
		base = &mibPtr->pathBase[index];
		if (!base->inUse)
			/* PATH not installed */
			result = EMIB_ID_NOT_FOUND;
	}

	if (!result)
	{
		/* get resource index */
		rsrc = (rsrcType == RT_mac)
			? MFindResourceID (rsrcID, mibPtr->macTable, mibPtr->smtData.Mac_Ct)
			: MFindResourceID (rsrcID, mibPtr->portTable,
			mibPtr->smtData.xdi.Port_Ct);
		if (rsrc < 0)
			/* resource not found */
			result = EMIB_ID_NOT_FOUND;
		else
		{
			/* get resource info */
			if (rsrcType == RT_mac)
			{
				rsrc = mibPtr->macTable[rsrc].index;
				rsrcConfig =
					&mibPtr->macBase[rsrc].baseData.xdi.configList[index];
			}

			else
			{
				rsrc = mibPtr->portTable[rsrc].index;
				rsrcConfig =
					&mibPtr->portBase[rsrc].baseData.xdi.configList[index];
			}
		}


		/* get next index */
		next = (nextType == RT_mac)
			? MFindResourceID (nextRsrc, mibPtr->macTable,
			mibPtr->smtData.Mac_Ct)
			: MFindResourceID (nextRsrc, mibPtr->portTable,
			mibPtr->smtData.xdi.Port_Ct);
		if (next < 0)
			/* resource not found */
			result = EMIB_ID_NOT_FOUND;
		else
		{
			/* get resource info */
			if (nextType == RT_mac)
			{
				next = mibPtr->macTable[next].index;
				newNext =
					&mibPtr->macBase[next].baseData.xdi.configList[index];
			}

			else
			{
				next = mibPtr->portTable[next].index;
				newNext =
					&mibPtr->portBase[next].baseData.xdi.configList[index];
			}
		}

		/* get previous index */
		prev = (prevType == RT_mac)
			? MFindResourceID (prevRsrc, mibPtr->macTable,
			mibPtr->smtData.Mac_Ct)
			: MFindResourceID (prevRsrc, mibPtr->portTable,
			mibPtr->smtData.xdi.Port_Ct);
		if (prev < 0)
			/* resource not found */
			result = EMIB_ID_NOT_FOUND;
		else
		{
			/* get resource info */
			if (prevType == RT_mac)
			{
				prev = mibPtr->macTable[prev].index;
				newPrev =
					&mibPtr->macBase[prev].baseData.xdi.configList[index];
			}

			else
			{
				prev = mibPtr->portTable[prev].index;
				newPrev =
					&mibPtr->portBase[prev].baseData.xdi.configList[index];
			}
		}
	}

	if (!result)
	{
		/* get config element */
		oldPrev = (rsrcConfig->prevType == RT_mac)
			? &mibPtr->macBase[rsrcConfig->prevIndex].baseData.xdi.configList[index]
			: &mibPtr->portBase[rsrcConfig->prevIndex].baseData.xdi.configList[index];

		/* get config element */
		oldNext = (rsrcConfig->nextType == RT_mac)
			? &mibPtr->macBase[rsrcConfig->nextIndex].baseData.xdi.configList[index]
			: &mibPtr->portBase[rsrcConfig->nextIndex].baseData.xdi.configList[index];

		/* remove object from config list */
		oldPrev->nextIndex = rsrcConfig->nextIndex;
		oldPrev->nextType = rsrcConfig->nextType;
		oldNext->prevIndex = rsrcConfig->prevIndex;
		oldNext->prevType = rsrcConfig->prevType;

		/* set object's new position */
		rsrcConfig->nextIndex = next;
		rsrcConfig->nextType = nextType;
		rsrcConfig->prevIndex = prev;
		rsrcConfig->prevType = prevType;

		/* insert object into list */
		newNext->prevIndex = rsrc;
		newNext->prevType = rsrcType;
		newPrev->nextIndex = rsrc;
		newPrev->nextType = rsrcType;
	}

	return (result);
}

/*
 * Only include the notification routines to issue flexible configuration
 * messages to FSP and CSP if the XM_USE_FLEX_CONF constant is defined.
 */
#ifdef XM_USE_FLEX_CONF

void
#ifdef __STDC__
NotifyAddMACMulti (uInt16 stnID, MACDataPtr macPtr)
#else
NotifyAddMACMulti (stnID, macPtr)
		uInt16           stnID;
		MACDataPtr       macPtr;
#endif
/*********************************************************************
Function:	This routine is called by the implementer to cause
			notification messages to be sent to CSP, FSP and MAP when
			a MAC object is added to the MIB. This routine is usually
			called after calling AddMACObjectMulti().
Parameters:	stnID	= index of station to use
			macPtr	= pointer to the MAC element in the MIB database.
Input:		macPtr	= element has been initialized with all base
						attributes.
Output:		Messages sent to CSP, FSP and MAP.
Return:		None.
Modification History:
*********************************************************************/
{
SMTMessage       msg;					/* notification message */
MIBMACAddType    addInfo;				/* base info */
uInt16           pathID;				/* loop counter */

	/* set up message */
	msg.source = MIB_MSG_ID;
	msg.type = MIB_EVENT_ADD_MAC;
	msg.typeInfo = 0;
	msg.localID = 0;
	msg.entity = macPtr->baseData.xdi.xdiIndex;
	msg.stnID = stnID;
	msg.len1 = SIZE_MIBMACAddType;
	msg.len2 = 0;
	msg.p1.MACAddBuf = &addInfo;

	/* fill in MAC info */
	addInfo.requestedPaths = macPtr->baseData.RequestedPaths.reqPaths;
	addInfo.index = macPtr->baseData.Index;
	addInfo.unitdataEnable = macPtr->baseData.MA_UnitdataEnable;
	addInfo.rmtDupPolicy = macPtr->baseData.xdi.RMTDupPolicy;
	addInfo.localPathIndex = macPtr->baseData.xdi.localPath;
	for (pathID = 0; pathID < MAX_PATH_COUNT; pathID++)
	{
		addInfo.configList[pathID].nextIndex =
			macPtr->baseData.xdi.configList[pathID].nextIndex;
		addInfo.configList[pathID].nextType =
			macPtr->baseData.xdi.configList[pathID].nextType;
		addInfo.configList[pathID].prevIndex =
			macPtr->baseData.xdi.configList[pathID].prevIndex;
		addInfo.configList[pathID].prevType =
			macPtr->baseData.xdi.configList[pathID].prevType;
	}

	/* send message to CSP */
	msg.destination = CSP_MSG_ID;
	SendMIBMessage (&msg);

	/* send message to FSP */
	msg.destination = FBM_MSG_ID;
	SendMIBMessage (&msg);

	/* remove internal info and send message to MAP */
	msg.destination = MAP_MSG_ID;
	msg.entity = macPtr->baseData.Index;
	msg.len1 = 0;
	msg.p1.MACAddBuf = (MIBMACAddType *) NULL;
	SendMIBMessage (&msg);

	return;
}

void
#ifdef __STDC__
NotifyInsertMACMulti (uInt16 stnID, MACDataPtr macPtr)
#else
NotifyInsertMACMulti (stnID, macPtr)
		uInt16           stnID;
		MACDataPtr       macPtr;
#endif
/*********************************************************************
Function:	This routine is called by the implementer to cause
			notification messages to be sent to CSP,FSP and MAP when
			a MAC object is inserted into the MIB. This routine is usually
			called after calling InsertMACObjectMulti().
Parameters:	stnID	= index of station to use
			macPtr	= pointer to the MAC element in the MIB database.
Input:		macPtr	= element has been initialized with all base
						attributes.
Output:		Messages sent to CSP, FSP and MAP.
Return:		None.
Modification History:
*********************************************************************/
{
SMTMessage       msg;					/* notification message */
MIBMACInsertType insertInfo;			/* hardware-based info */

	/* set up message */
	msg.source = MIB_MSG_ID;
	msg.type = MIB_EVENT_INSERT_MAC;
	msg.typeInfo = 0;
	msg.localID = 0;
	msg.entity = macPtr->baseData.xdi.xdiIndex;
	msg.stnID = stnID;
	msg.len1 = SIZE_MIBMACInsertType;
	msg.len2 = 0;
	msg.p1.MACInsertBuf = &insertInfo;

	/* fill in MAC info */
	insertInfo.T_MaxCap = macPtr->hwData.T_MaxCapability;
	insertInfo.TVXCap = macPtr->hwData.TVXCapability;
	insertInfo.availablePaths = macPtr->hwData.AvailablePaths;
	MCopyAddress (insertInfo.SMTAddress, macPtr->hwData.SMTAddress);

	/* send message to CSP */
	msg.destination = CSP_MSG_ID;
	SendMIBMessage (&msg);

	/* send message to FSP */
	msg.destination = FBM_MSG_ID;
	SendMIBMessage (&msg);

	/* remove internal info and send message to MAP */
	msg.destination = MAP_MSG_ID;
	msg.entity = macPtr->baseData.Index;
	msg.len1 = 0;
	msg.p1.MACInsertBuf = (MIBMACInsertType *) NULL;
	SendMIBMessage (&msg);

	return;
}

void
#ifdef __STDC__
NotifyRemoveMACMulti (uInt16 stnID, uInt16 rsrcID, Int16 xdiID)
#else
NotifyRemoveMACMulti (stnID, rsrcID, xdiID)
		uInt16           stnID;
		uInt16           rsrcID;
		Int16            xdiID;
#endif
/*********************************************************************
Function:	This routine is called by the implementer to cause
			notification messages to be sent to CSP, FSP and MAP when
			a MAC object is removed from the MIB. This routine is usually
			called after RemoveMACObjectMulti() which returns the
			table array index, xdiID.
Parameters:	stnID	= index of station to use
			rsrcID	= MAC's MIB resource index value
			xdiID	= internal table array index
Input:		None.
Output:		Messages sent to CSP, FSP and MAP.
Return:		None.
Modification History:
*********************************************************************/
{
SMTMessage       msg;					/* notification message */

	/* set up message */
	msg.source = MIB_MSG_ID;
	msg.type = MIB_EVENT_REMOVE_MAC;
	msg.typeInfo = 0;
	msg.localID = 0;
	msg.entity = (uInt16) xdiID;
	msg.stnID = stnID;
	msg.len1 = 0;
	msg.len2 = 0;

	/* send message to CSP */
	msg.destination = CSP_MSG_ID;
	SendMIBMessage (&msg);

	/* send message to FSP */
	msg.destination = FBM_MSG_ID;
	SendMIBMessage (&msg);

	/* send message to MAP */
	msg.destination = MAP_MSG_ID;
	msg.entity = rsrcID;
	SendMIBMessage (&msg);

	return;
}

void
#ifdef __STDC__
NotifyDeleteMACMulti (uInt16 stnID, uInt16 rsrcID, Int16 xdiID)
#else
NotifyDeleteMACMulti (stnID, rsrcID, xdiID)
		uInt16           stnID;
		uInt16           rsrcID;
		Int16            xdiID;
#endif
/*********************************************************************
Function:	This routine is called by the implementer to cause
			notification messages to be sent to CSP, FSP and MAP when
			a MAC object is deleted from the MIB. This routine is usually
			called after DeleteMACObjectMulti() which returns the
			internal table array index, xdiID.
Parameters:	stnID	= index of station to use
			rsrcID	= MAC's MIB resource index value
			xdiID	= internal table array index
Input:		None.
Output:		Messages sent to CSP, FSP and MAP.
Return:		None.
Modification History:
*********************************************************************/
{
SMTMessage       msg;					/* notification message */

	/* set up message */
	msg.source = MIB_MSG_ID;
	msg.type = MIB_EVENT_DELETE_MAC;
	msg.typeInfo = 0;
	msg.localID = 0;
	msg.entity = (uInt16) xdiID;
	msg.stnID = stnID;
	msg.len1 = 0;
	msg.len2 = 0;

	/* send message to CSP */
	msg.destination = CSP_MSG_ID;
	SendMIBMessage (&msg);

	/* send message to FSP */
	msg.destination = FBM_MSG_ID;
	SendMIBMessage (&msg);

	/* send message to MAP */
	msg.destination = MAP_MSG_ID;
	msg.entity = rsrcID;
	SendMIBMessage (&msg);

	return;
}

void
#ifdef __STDC__
NotifyAddPORTMulti (uInt16 stnID, PORTDataPtr portPtr)
#else
NotifyAddPORTMulti (stnID, portPtr)
		uInt16           stnID;
		PORTDataPtr      portPtr;
#endif
/*********************************************************************
Function:	This routine is called by the implementer to cause
			notification messages to be sent to CSP, FSP and MAP when
			a PORT object is added to the MIB. This routine is usually
			called after calling AddPORTObjectMulti().
Parameters:	stnID	= index of station to use
			portPtr	= pointer to the PORT element in the MIB database.
Input:		portPtr	= element has been initialized with all base
						attributes.
Output:		Messages sent to CSP, FSP and MAP.
Return:		None.
Modification History:
*********************************************************************/
{
SMTMessage       msg;					/* notification message */
MIBPORTAddType   addInfo;				/* base data */
uInt16           pathID;				/* loop counter */

	/* set up message */
	msg.source = MIB_MSG_ID;
	msg.type = MIB_EVENT_ADD_PORT;
	msg.typeInfo = 0;
	msg.localID = 0;
	msg.entity = portPtr->baseData.xdi.xdiIndex;
	msg.stnID = stnID;
	msg.len1 = SIZE_MIBPORTAddType;
	msg.len2 = 0;
	msg.p1.PORTAddBuf = &addInfo;

	/* fill in PORT info */
	addInfo.PC_Type = portPtr->baseData.My_Type;
	addInfo.connectionPolicies = portPtr->baseData.ConnectionPolicies;
	addInfo.requestedPaths[RP_none]
		= portPtr->baseData.RequestedPaths.reqPaths[RP_none];
	addInfo.requestedPaths[RP_tree]
		= portPtr->baseData.RequestedPaths.reqPaths[RP_tree];
	addInfo.requestedPaths[RP_peer]
		= portPtr->baseData.RequestedPaths.reqPaths[RP_peer];
	addInfo.index = portPtr->baseData.Index;
	addInfo.lerCutoff = portPtr->baseData.Ler_Cutoff;
	addInfo.lerAlarm = portPtr->baseData.Ler_Alarm;
	addInfo.localPathIndex = portPtr->baseData.xdi.localPath;
	for (pathID = 0; pathID < MAX_PATH_COUNT; pathID++)
	{
		addInfo.configList[pathID].nextIndex =
			portPtr->baseData.xdi.configList[pathID].nextIndex;
		addInfo.configList[pathID].nextType =
			portPtr->baseData.xdi.configList[pathID].nextType;
		addInfo.configList[pathID].prevIndex =
			portPtr->baseData.xdi.configList[pathID].prevIndex;
		addInfo.configList[pathID].prevType =
			portPtr->baseData.xdi.configList[pathID].prevType;
	}
	addInfo.MACLoop_Time = portPtr->baseData.MACLoop_Time;
	addInfo.Maint_LS = portPtr->baseData.Maint_LS;

	/* send message to CSP */
	msg.destination = CSP_MSG_ID;
	SendMIBMessage (&msg);

	/* send message to FSP */
	msg.destination = FBM_MSG_ID;
	SendMIBMessage (&msg);

	/* remove interal info and send message to MAP */
	msg.destination = MAP_MSG_ID;
	msg.entity = portPtr->baseData.Index;
	msg.len1 = 0;
	msg.p1.PORTAddBuf = (MIBPORTAddType *) NULL;
	SendMIBMessage (&msg);

	return;
}

void
#ifdef __STDC__
NotifyInsertPORTMulti (uInt16 stnID, PORTDataPtr portPtr)
#else
NotifyInsertPORTMulti (stnID, portPtr)
		uInt16           stnID;
		PORTDataPtr      portPtr;
#endif
/*********************************************************************
Function:	This routine is called by the implementer to cause
			notification messages to be sent to CSP, FSP and MAP when
			a PORT object is inserted into the MIB. This routine is usually
			called after calling InsertPORTObjectMulti().
Parameters:	stnID	= index of station to use
			portPtr	= pointer to the PORT element in the MIB database.
Input:		portPtr	= element has been initialized with all base
						attributes.
Output:		Messages sent to CSP, FSP and MAP.
Return:		None.
Modification History:
*********************************************************************/
{
SMTMessage       msg;					/* notification message */
MIBPORTInsertType insertInfo;			/* hardware-based data */

	/* set up message */
	msg.source = MIB_MSG_ID;
	msg.type = MIB_EVENT_INSERT_PORT;
	msg.typeInfo = 0;
	msg.localID = 0;
	msg.entity = portPtr->baseData.xdi.xdiIndex;
	msg.stnID = stnID;
	msg.len1 = SIZE_MIBPORTInsertType;
	msg.len2 = 0;
	msg.p1.PORTInsertBuf = &insertInfo;

	/* fill in PORT info */
	insertInfo.availablePaths = portPtr->hwData.AvailablePaths;
	insertInfo.conCapabilities = portPtr->hwData.ConnectionCapabilities;

	/* send message to CSP */
	msg.destination = CSP_MSG_ID;
	SendMIBMessage (&msg);

	/* send message to FSP */
	msg.destination = FBM_MSG_ID;
	SendMIBMessage (&msg);

	/* send message to MAP */
	msg.destination = MAP_MSG_ID;
	msg.entity = portPtr->baseData.Index;
	msg.len1 = 0;
	msg.p1.PORTInsertBuf = (MIBPORTInsertType *) NULL;
	SendMIBMessage (&msg);

	return;
}

void
#ifdef __STDC__
NotifyRemovePORTMulti (uInt16 stnID, uInt16 rsrcID, Int16 xdiID)
#else
NotifyRemovePORTMulti (stnID, rsrcID, xdiID)
		uInt16           stnID;
		uInt16           rsrcID;
		Int16            xdiID;
#endif
/*********************************************************************
Function:	This routine is called by the implementer to cause
			notification messages to be sent to CSP, FSP and MAP when
			a PORT object is removed from the MIB. This routine is usually
			called after calling RemovePORTObjectMulti() which returns
			the internal table index, xdiID.
Parameters:	stnID	= index of station to use
			rsrcID	= PORT's MIB resource index value
			xdiID	= internal table array index
Input:		None.
Output:		Messages sent to CSP, FSP and MAP.
Return:		None.
Modification History:
*********************************************************************/
{
SMTMessage       msg;					/* notification message */

	/* set up message */
	msg.source = MIB_MSG_ID;
	msg.type = MIB_EVENT_REMOVE_PORT;
	msg.typeInfo = 0;
	msg.localID = 0;
	msg.entity = xdiID;
	msg.stnID = stnID;
	msg.len1 = 0;
	msg.len2 = 0;

	/* send message to CSP */
	msg.destination = CSP_MSG_ID;
	SendMIBMessage (&msg);

	/* send message to FSP */
	msg.destination = FBM_MSG_ID;
	SendMIBMessage (&msg);

	/* send message to MAP */
	msg.destination = MAP_MSG_ID;
	msg.entity = rsrcID;
	SendMIBMessage (&msg);

	return;
}

void
#ifdef __STDC__
NotifyDeletePORTMulti (uInt16 stnID, uInt16 rsrcID, Int16 xdiID)
#else
NotifyDeletePORTMulti (stnID, rsrcID, xdiID)
		uInt16           stnID;
		uInt16           rsrcID;
		Int16            xdiID;
#endif
/*********************************************************************
Function:	This routine is called by the implementer to cause
			notification messages to be sent to CSP, FSP and MAP when
			a PORT object is deleted from the MIB. This routine is usually
			called after DeletePORTObjectMulti() which provides the
			internal table array index, xdiID.
Parameters:	stnID	= index of station to use
			rsrcID	= PORT's MIB resource index value
			xdiID	= internal table array index
Input:		None.
Output:		Messages sent to CSP, FSP and MAP.
Return:		None.
Modification History:
*********************************************************************/
{
SMTMessage       msg;					/* notification message */

	/* set up message */
	msg.source = MIB_MSG_ID;
	msg.type = MIB_EVENT_DELETE_PORT;
	msg.typeInfo = 0;
	msg.localID = 0;
	msg.entity = xdiID;
	msg.stnID = stnID;
	msg.len1 = 0;
	msg.len2 = 0;

	/* send message to CSP */
	msg.destination = CSP_MSG_ID;
	SendMIBMessage (&msg);

	/* send message to FSP */
	msg.destination = FBM_MSG_ID;
	SendMIBMessage (&msg);

	/* send message to MAP */
	msg.destination = MAP_MSG_ID;
	msg.entity = rsrcID;
	SendMIBMessage (&msg);

	return;
}

#endif							/* XM_USE_FLEX_CONF */


/*
*	If the constant MULTIPLE_STATION is not defined, then the system
*	is being compiled for single station use. In this case, the following
*	routine(s) need to be defined for the older single station calling
*	sequence.
*/
#ifndef MULTIPLE_STATION

uInt32
#ifdef __STDC__
AddMACObject (uInt16 rsrcID, MACDataPtr * macPtr)
#else
AddMACObject (rsrcID, macPtr)
		uInt16           rsrcID;
		MACDataPtr      *macPtr;
#endif
{
	return (AddMACObjectMulti (FIRST_STATION, rsrcID, macPtr));
}

uInt32
#ifdef __STDC__
AddPATHObject (uInt16 rsrcID, PATHDataPtr * pathPtr)
#else
AddPATHObject (rsrcID, pathPtr)
		uInt16           rsrcID;
		PATHDataPtr     *pathPtr;
#endif
{
	return (AddPATHObjectMulti (FIRST_STATION, rsrcID, pathPtr));
}

uInt32
#ifdef __STDC__
AddPORTObject (uInt16 rsrcID, FDDIPortType type,
	PORTDataPtr * portPtr)
#else
AddPORTObject (rsrcID, type, portPtr)
		uInt16           rsrcID;
		FDDIPortType     type;
		PORTDataPtr     *portPtr;
#endif
{
	return (AddPORTObjectMulti (FIRST_STATION, rsrcID, type, portPtr));
}

uInt32
#ifdef __STDC__
InsertMACObject (uInt16 rsrcID, MACDataPtr * macPtr)
#else
InsertMACObject (rsrcID, macPtr)
		uInt16           rsrcID;
		MACDataPtr      *macPtr;
#endif
{
	return (InsertMACObjectMulti (FIRST_STATION, rsrcID, macPtr));
}

uInt32
#ifdef __STDC__
InsertPORTObject (uInt16 rsrcID, PORTDataPtr * portPtr)
#else
InsertPORTObject (rsrcID, portPtr)
		uInt16           rsrcID;
		PORTDataPtr     *portPtr;
#endif
{
	return (InsertPORTObjectMulti (FIRST_STATION, rsrcID, portPtr));
}

/*
 * Only include the routines to handle flexible configurations
 * if the XM_USE_FLEX_CONF constant is defined.
 */
#ifdef XM_USE_FLEX_CONF

uInt32
#ifdef __STDC__
RemoveMACObject (uInt16 rsrcID, Int16 * xdiID)
#else
RemoveMACObject (rsrcID, xdiID)
		uInt16           rsrcID;
		Int16           *xdiID;
#endif
{
	return (RemoveMACObjectMulti (FIRST_STATION, rsrcID, xdiID));
}

uInt32
#ifdef __STDC__
RemovePORTObject (uInt16 rsrcID, Int16 * xdiID)
#else
RemovePORTObject (rsrcID, xdiID)
		uInt16           rsrcID;
		Int16           *xdiID;
#endif
{
	return (RemovePORTObjectMulti (FIRST_STATION, rsrcID, xdiID));
}

uInt32
#ifdef __STDC__
DeleteMACObject (uInt16 rsrcID, Int16 * xdiID)
#else
DeleteMACObject (rsrcID, xdiID)
		uInt16           rsrcID;
		Int16           *xdiID;
#endif
{
	return (DeleteMACObjectMulti (FIRST_STATION, rsrcID, xdiID));
}

uInt32
#ifdef __STDC__
DeletePORTObject (stnID, uInt16 rsrcID, Int16 * xdiID)
#else
DeletePORTObject (rsrcID, xdiID)
		uInt16           rsrcID;
		Int16           *xdiID;
#endif
{
	return (DeletePORTObjectMulti (FIRST_STATION, rsrcID, xdiID));
}

#endif							/* XM_USE_FLEX_CONF */

uInt32
#ifdef __STDC__
AddObjectPATH (uInt16 pathID, uInt16 rsrcID, uInt16 rsrcType,
	uInt16 nextRsrc, uInt16 nextType,
	uInt16 prevRsrc, uInt16 prevType)
#else
AddObjectPATH (pathID, rsrcID, rsrcType, nextRsrc, nextType,
	prevRsrc, prevType)
		uInt16           pathID;
		uInt16           rsrcID;
		uInt16           rsrcType;
		uInt16           nextRsrc;
		uInt16           nextType;
		uInt16           prevRsrc;
		uInt16           prevType;
#endif
{
	return (AddObjectPATHMulti (FIRST_STATION, pathID, rsrcID, rsrcType,
			nextRsrc, nextType, prevRsrc, prevType));
}

/*
 * Only include the routines to handle flexible configurations
 * if the XM_USE_FLEX_CONF constant is defined.
 */
#ifdef XM_USE_FLEX_CONF

void
#ifdef __STDC__
NotifyAddMAC (MACDataPtr macPtr)
#else
NotifyAddMAC (macPtr)
		MACDataPtr       macPtr;
#endif
{
	NotifyAddMACMulti (FIRST_STATION, macPtr);
	return;
}

void
#ifdef __STDC__
NotifyInsertMAC (MACDataPtr macPtr)
#else
NotifyInsertMAC (macPtr)
		MACDataPtr       macPtr;
#endif
{
	NotifyInsertMACMulti (FIRST_STATION, macPtr);
	return;
}

void
#ifdef __STDC__
NotifyRemoveMAC (uInt16 rsrcID, Int16 xdiID)
#else
NotifyRemoveMAC (rsrcID, xdiID)
		uInt16           rsrcID;
		Int16            xdiID;
#endif
{
	NotifyRemoveMACMulti (FIRST_STATION, rsrcID, xdiID);
	return;
}

void
#ifdef __STDC__
NotifyDeleteMAC (uInt16 rsrcID, Int16 xdiID)
#else
NotifyDeleteMAC (rsrcID, xdiID)
		uInt16           rsrcID;
		Int16            xdiID;
#endif
{
	NotifyDeleteMACMulti (FIRST_STATION, rsrcID, xdiID);
	return;
}

void
#ifdef __STDC__
NotifyAddPORT (PORTDataPtr portPtr)
#else
NotifyAddPORT (portPtr)
		PORTDataPtr      portPtr;
#endif
{
	NotifyAddPORTMulti (FIRST_STATION, portPtr);
	return;
}

void
#ifdef __STDC__
NotifyInsertPORT (PORTDataPtr portPtr)
#else
NotifyInsertPORT (portPtr)
		PORTDataPtr      portPtr;
#endif
{
	NotifyInsertPORTMulti (FIRST_STATION, portPtr);
	return;
}

void
#ifdef __STDC__
NotifyRemovePORT (uInt16 rsrcID, Int16 xdiID)
#else
NotifyRemovePORT (rsrcID, xdiID)
		uInt16           rsrcID;
		Int16            xdiID;
#endif
{
	NotifyRemovePORTMulti (FIRST_STATION, rsrcID, xdiID);
	return;
}

void
#ifdef __STDC__
NotifyDeletePORT (uInt16 rsrcID, Int16 xdiID)
#else
NotifyDeletePORT (rsrcID, xdiID)
		uInt16           rsrcID;
		Int16            xdiID;
#endif
{
	NotifyDeletePORTMulti (FIRST_STATION, rsrcID, xdiID);
	return;
}

#endif							/* XM_USE_FLEX_CONF */

#endif							/* MULTIPLE_STATION */
