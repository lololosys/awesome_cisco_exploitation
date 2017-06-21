/* $Id: cspmsg.c,v 3.2 1995/11/17 18:09:06 hampton Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/xdi/cspmsg.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cspmsg.c,v $
 * Revision 3.2  1995/11/17  18:09:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:54:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:23:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_cspmsg.c xm7_v3.1.1:cset.0166:26:26 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Connection Services Process Module
		
		Message Processing Routines
		
		File:			xm7_cspmsg.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
		
		This module provides the message processing routines for
		handling messages received by CSP.

		With release 7.3, this file contains the following conditional
		compilation options:

			XM_USE_FLEX_CONF	include code for flexible configuration.

		The constant XM_USE_FLEX_CONF controls conditional compilation
		over routines to process messages handling flexible
		configuration changes. If this constant is NOT defined, then
		the associated routines are NOT included in the code and the
		system will not support flexible configurations.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

#include        "cschdrsif.h"
#include        "xdihdrsif.h"
#include        "../if/if_fddi.h"

#include	"smterror.h"
#include	"fddihdr.h"
#include	"smtmsg.h"
#include	"csphdr.h"
#include	"cspglbl.h"
#include	"cspproto.h"
#include	"multistn.h"
#include	"cspgrace.h"


/*********************************************************************
		PCM Connection Routines

		These routines are located here because they are used by
		the PCM pseudo-code and message processing. They are in this
		file instead of the pseudo-code file (csppcode.c) because
		different pseudo-code files are used by software- and
		hardware-based PCMs, but these routines are the same for
		both versions.
*********************************************************************/

Flag
#ifdef __STDC__
Allow_Connection (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
Allow_Connection (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Determine if this connection topology is allowed by the
			PORT's connection policy.
Parameters:	stnID	= index of station to use,
			portPtr	= PORT's array element.
Input:		Uses portData and stationData.
Output:		None.
Return:		SET if connection is allowed, CLEAR if not allowed.
Modification History:
*********************************************************************/
{
uInt16           connectionType;		/* type of connection */
Flag             result;				/* return value */

	/*
	 * The connection type is determined by setting the bit corresponding to
	 * the connection policy. The way the bits are ordered in
	 * fddiSMTConnectionPolicy so that the policies for A ports are in the
	 * rightmost four bits, the B port policies are in the next four bits,
	 * etc. Each of the four bits represent neighbors of type A, B, S and M
	 * moving right to left.
	 * 
	 * To select the proper bit representing the current connection, set the
	 * neighbors bit, then shift it to the proper nibble.
	 */

	/* Set neighbor bit */
	connectionType = 1 << portPtr->PC_Neighbor;

	/* Shift 4 bits per port type */
	connectionType <<= (portPtr->PC_Type << 2);

	/*
	 * If bit is set in connection policy, then this link is not acceptable
	 * to this station.
	 */
	result = !(CSPDATA (stnID, stationData.connectionPolicy) & connectionType);

	return (result);
}

Flag
#ifdef __STDC__
MACOnOutput (uInt16 stnID, CSPPORTInfoType * portPtr)
#else
MACOnOutput (stnID, portPtr)
		uInt16           stnID;
		CSPPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	This routine determines the setting of T_Val(9), MAC on
			Port Output. This routine relies on a few basic FDDI rules:
				Port A always terminates the secondary path
				Ports B and S always terminate the primary path
Parameters:	stnID	= index of station to use
			portPtr	= information of port to process.
Input:		Uses portData and stationData.
Output:		None.
Return:		SET if T_Val(9) is to be SET, otherwise T_Val(9) is to be CLEAR.
Modification History:
*********************************************************************/
{
uInt16           searchPath;			/* path to search */
CSPMACInfoType  *upmacPtr;				/* upstream MAC information */
uInt16           upIndex;				/* upstream entity index */
uInt16           upType;				/* upstream entity type */

	/* set default value */
	searchPath = PI_None;

	/* select path based on type */
	switch (portPtr->PC_Type)
	{
	case PT_a:
		/* search secondary path */
		searchPath = PI_secondary;
		break;

	case PT_b:
	case PT_s:
		/* search primary path */
		searchPath = PI_primary;
		break;

	case PT_m:
		if (portPtr->requestedPaths[RP_tree] & PP_primary_pref)
			searchPath = PI_primary;

		else if (portPtr->requestedPaths[RP_tree] & PP_secondary_pref)
			searchPath = PI_secondary;

		else if (portPtr->requestedPaths[RP_tree] & PP_primary_alt)
			searchPath = PI_primary;

		else if (portPtr->requestedPaths[RP_tree] & PP_secondary_alt)
			searchPath = PI_secondary;

		else if (portPtr->requestedPaths[RP_tree] & PP_local)
			searchPath = PI_local;

		break;
	}

	/* set default */
	upType = RT_port;

	/* if port has a requested path */
	if (searchPath != PI_None)
	{
		/* convert path into path index */
		if (searchPath == PI_local)
			searchPath = portPtr->localPathIndex - 1;

		else
			searchPath--;

		/* get upstream entity on this path */
		upType = portPtr->configList[searchPath].prevType;
		upIndex = portPtr->configList[searchPath].prevIndex;

		/* check if MAC */
		while (upType == RT_mac)
		{
			/* get MAC info */
			upmacPtr = &(CSPDATA (stnID, macData[upIndex]));

			/* check if MAC is on path */
			if ((searchPath == PII_primary)
				&& (upmacPtr->availablePaths & AP_primary)
				&& ((upmacPtr->requestedPaths
						& (PP_primary_pref | PP_primary_alt))
					&& !(upmacPtr->requestedPaths
						& PP_secondary_pref)))
			{
				/* MAC is requesting primary */
				break;
			}

			else if ((searchPath == PII_secondary)
					&& (upmacPtr->availablePaths & AP_secondary)
					&& (upmacPtr->requestedPaths
					& (PP_secondary_pref | PP_secondary_alt)))
			{
				/* MAC is requesting secondary */
				break;
			}

			else if ((upmacPtr->availablePaths & AP_local)
				&& ((upmacPtr->localPathIndex - 1) == searchPath))
				/* MAC is requesting correct local path */
				break;

			/*
			 * MAC is not on this path. Check upstream for serial MACs.
			 */
			upType = upmacPtr->configList[searchPath].prevType;
			upIndex = upmacPtr->configList[searchPath].prevIndex;
		}
	}

	/* Bit is set if MAC found immediately upstream */
	return (upType == RT_mac);
}


/*********************************************************************
		Message Processing Routines
*********************************************************************/

void
#ifdef __STDC__
SendCSPEvent (uInt16 stnID, uInt16 eventType, uInt16 entity)
#else
SendCSPEvent (stnID, eventType, entity)
		uInt16           stnID;
		uInt16           eventType;
		uInt16           entity;
#endif
/*********************************************************************
Function:	Send a CSP event message.
Parameters:	stnID		= index of station to use.
			eventType	= type of event record.
			entity		= PORT or MAC index if needed, else 0.
Input:		Uses CSP global data.
Output:		None.
Return:		None.
Notes:		This function takes the global data based on the event
			type to reduce processing time.
			This function ONLY processes standard FDDI MIB values.
			Refer to SendCSPVendorEvent() below for vendor-specific
			values.
Modification History:
*********************************************************************/
{
SMTMessage       eventMsg;				/* message to send */

	/*
	 * Create header.
	 */
	eventMsg.source = CSP_MSG_ID;
	eventMsg.destination = MIB_MSG_ID;
	eventMsg.type = CSP_EVENT_NOTIFY_MIB;
	eventMsg.typeInfo = eventType;
	eventMsg.entity = entity;
	eventMsg.localID = 0;
	eventMsg.len1 = 0;
	eventMsg.len2 = 0;
	eventMsg.stnID = stnID;

	/*
	 * Fill in data.
	 */
	switch (eventType)
	{

		/*
		 * SMT Attributes
		 */
	case fddiSMTECMState:
		eventMsg.data.b16 = CSPDATA (stnID, stationData.ecState);
		/**
		 * WRF
		 * Log CMT debugging information.
		 **/
                fddi_cbug_ecm_state (stnID, eventMsg.data.b16);
		break;

	case fddiSMTCF_State:
		eventMsg.data.b16 = CSPDATA (stnID, stationData.cfState);
		/**
		 * WRF
		 * Log CMT debugging information.
		 **/
                fddi_cbug_cfm_state (stnID, eventMsg.data.b16);
		break;

	case fddiSMTPeerWrapFlag:
		eventMsg.data.b8 = CSPDATA (stnID, stationData.PeerWrapFlag);
		break;

		/*
		 * MAC Attributes
		 */
	case fddiMACCurrentPath:
		eventMsg.data.b16 = CSPDATA (stnID, macData[entity].currentPath);
		break;

	case fddiMACRMTState:
		eventMsg.data.b16 = CSPDATA (stnID, macData[entity].rmState);
		/**
		 * WRF
		 * Log CMT debugging information.
		 **/
                fddi_cbug_rmt_state (stnID, entity, eventMsg.data.b16);
		break;

	case fddiMACDA_Flag:
		eventMsg.data.b8 = CSPDATA (stnID, macData[entity].DA_Flag);
		break;

	case fddiMACMA_UnitdataAvailable:
		eventMsg.data.b8 = CSPDATA (stnID, macData[entity].MAC_Avail);
		break;

	case fddiMACSMTAddress:
		/* RMT caused address to change */
		MCopyAddress (eventMsg.data.addr,
			CSPDATA (stnID, macData[entity].SMTAddress));
		break;

		/*
		 * PATH Attributes
		 */
	case fddiPATHTraceStatus:
		eventMsg.data.b16 = CSPDATA (stnID, pathData[entity].traceStatus);
		break;

		/*
		 * PORT Attributes
		 */
	case fddiPORTNeighborType:
		/*
		 * note: the data structure corresponding to PC_Neighbor is a uInt16
		 * in mib.
		 */
		eventMsg.data.b16 = (uInt16) CSPDATA (stnID, portData[entity].PC_Neighbor);
		break;

	case fddiPORTMACIndicated:
		/* set both transmitted and received bits */
		eventMsg.data.b8 = (MReadBit (CSPDATA (stnID,
					portData[entity].R_Val), 9)) ? CSP_RVAL9 : 0;
		eventMsg.data.b8 |= (MReadBit (CSPDATA (stnID,
					portData[entity].T_Val), 9)) ? CSP_TVAL9 : 0;
		break;

	case fddiPORTCurrentPath:
		eventMsg.data.b16 = CSPDATA (stnID, portData[entity].currentPath);
		break;

	case fddiPORTBS_Flag:
		eventMsg.data.b8 = CSPDATA (stnID, portData[entity].BS_Flag);
		break;

	case fddiPORTLCTFail_Ct:
		eventMsg.data.b32 = CSPDATA (stnID, portData[entity].PC_LCT_Fail);
		break;

	case fddiPORTLer_Estimate:
		eventMsg.data.b8 = CSPDATA (stnID, portData[entity].estimate);
		break;

	case fddiPORTLerFlag:
		eventMsg.data.b8 = CSPDATA (stnID, portData[entity].lerFlag);
		break;

	case fddiPORTLem_Reject_Ct:
		eventMsg.data.b32 = CSPDATA (stnID, portData[entity].LEM_Reject_Ct);
		break;

	case fddiPORTLem_Ct:
		eventMsg.data.b32 = CSPDATA (stnID, portData[entity].lemCount);
		break;

	case fddiPORTConnectState:
		eventMsg.data.b16 = CSPDATA (stnID, portData[entity].Connect_State);
		break;

	case fddiPORTPCMState:
		eventMsg.data.b16 = CSPDATA (stnID, portData[entity].pcState);
		/**
		 * WRF
		 * Log CMT debugging information.
		 **/
                fddi_cbug_pcm_state (stnID, entity, eventMsg.data.b16);
		break;

	case fddiPORTPC_Withhold:
		eventMsg.data.b16 = CSPDATA (stnID, portData[entity].PC_Withhold);
		break;

	default:
		return;
	}

	/*
	 * Send message.
	 */
	SendCSPMessage (&eventMsg);

	return;
}

void
#ifdef __STDC__
SendCSPVendorEvent (uInt16 stnID, uInt16 eventType, uInt32 eventID,
	uInt16 entity)
#else
SendCSPVendorEvent (stnID, eventType, eventID, entity)
		uInt16           stnID;
		uInt16           eventType;
		uInt32           eventID;
		uInt16           entity;
#endif
/*********************************************************************
Function:	Send a CSP event message for a vendor-specific attribute value.
Parameters:	stnID		= index of station to use.
			eventType	= vendor-specific MIB parameter type.
			eventID		= vendor-specific parameter ID.
			entity		= PORT or MAC index if needed, else 0.
Input:		Uses CSP global data.
Output:		None.
Return:		None.
Notes:		This function takes the global data based on the event
			type to reduce processing time.
Modification History:
*********************************************************************/
{
SMTMessage       eventMsg;				/* message to send */

	/*
	 * Create header.
	 */
	eventMsg.source = CSP_MSG_ID;
	eventMsg.destination = MIB_MSG_ID;
	eventMsg.type = CSP_EVENT_NOTIFY_MIB;
	eventMsg.typeInfo = eventType;
	eventMsg.entity = entity;
	eventMsg.localID = 0;
	eventMsg.len1 = 0;
	eventMsg.len2 = 0;
	eventMsg.stnID = stnID;
	eventMsg.vendorID = eventID;
	eventMsg.vendorType = 0;

	/*
	 * Fill in data.
	 */
	switch (eventType & MIB_OBJECT_MASK)
	{
	case fddiMAC:
		if (eventID == xdiMACConfigurationStatus)
		{
			/* report new configuration result */
			eventMsg.data.b16 = CSPDATA (stnID, macData[entity].configStatus);
		}

		else if (eventID == xdiMACSM_MA_AvailFlag)
		{
			/* report new flag state */
			eventMsg.data.b8 = CSPDATA (stnID, macData[entity].SM_MA_Avail);
		}

		break;

	case fddiSMT:
	case fddiPATH:
	case fddiPORT:
	default:
		return;
	}

	/*
	 * Send message.
	 */
	SendCSPMessage (&eventMsg);

	return;
}

void
#ifdef __STDC__
ProcessCSPMIBChange (SMTMessage * smtmsg)
#else
ProcessCSPMIBChange (smtmsg)
		SMTMessage      *smtmsg;
#endif
/*********************************************************************
Function:	Process change from the MIB.
Parameters:	smtmsg	= address of message buffer.
Input:		smtmsg	= contains message to process.
Output:		Changes global data and/or posts signals to the signal
			queue depending upon the message.
Return:		None.
Notes:		None.
Modification History:
*********************************************************************/
{
uChar            savePaths;				/* save available paths */
uInt16           stnID;					/* station ID from message */
uInt16           i;						/* loop counter */
CSPStationInfoType *stnPtr;				/* station data pointer */
CSPMACInfoType  *macPtr;				/* MAC element pointer */
CSPPATHInfoType *pathPtr;				/* PATH element pointer */
CSPPORTInfoType *portPtr;				/* PORT element pointer */
PORTRequestedPathsType *rpPtr;			/* format for requested paths */

	stnID = smtmsg->stnID;
	stnPtr = &CSPDATA (stnID, stationData);

	switch (smtmsg->typeInfo)
	{
		/*
		 * SMT Attributes
		 */
	case fddiSMTConfigPolicy:
		/* set new policy value */
		stnPtr->configPolicy = smtmsg->data.b16;
		break;

	case fddiSMTConnectionPolicy:
		stnPtr->connectionPolicy = smtmsg->data.b16;

		/*
		 * Verify that all connections are valid.
		 */
		for (i = 0, portPtr = &CSPDATA (stnID, portData[0]);
			i < MAX_PORT_COUNT;
			i++, portPtr++)
		{
			switch (portPtr->pcState)
			{
				/*
				 * Do not check the current neighbor if the port is in Off,
				 * Break, Trace, Connect or Maint. Otherwise, recheck against
				 * the new connection policy.
				 */
			case PC_OFF:
			case PC_BREAK:
			case PC_TRACE:
			case PC_CONNECT:
			case PC_MAINT:
				break;

			default:
				/*
				 * If the connection policy causes the connection status to
				 * change, then restart the connection.
				 */
				if (MReadBit (portPtr->T_Val, 3)
					!= Allow_Connection (stnID, &CSPDATA (stnID, portData[i])))
				{
					SendSignalMulti (stnID, SIG_PC_Start, i, (uInt32) 0);
				}
				break;
			}
		}
		break;

	case fddiSMTTrace_MaxExpiration:
		stnPtr->Trace_Max = MFddi2Usecs (smtmsg->data.b32);
		break;

	case fddiSMTVendorAttrib:
#if (USE_SCHED_GRACE || USE_FULL_GRACE)
		if (smtmsg->vendorID == xdiSMTGracefulInsMode)
		{
			if (CSPDATA (stnID, stationData.gracefulMode)
				== GIM_gracefulinsertion)
			{
#if USE_FULL_GRACE
				if (CSPDATA (stnID, stationData.giMACID) != NO_GIMAC)
				{
					macPtr = &CSPDATA (stnID,
						macData[CSPDATA (stnID, stationData.giMACID)]);

					while (ReadGIPORTQueue (macPtr, &portPtr)
						!= ECSP_GIPORT_Q_MT)
					{
						AbortGIMulti (stnID, macPtr, portPtr);

						/*
						 * Tell ECM that this port is finished with GI.
						 */
						SendSignal (SIG_EC_PCM_RETRY, portPtr->ID, (uInt32) 0);
					}
				}
#endif							/* USE_FULL_GRACE */
			}
			else if (CSPDATA (stnID, stationData.gracefulMode)
				== GIM_scheduledinsertion)
			{
				MCANCEL_SCHED_INS (stnID, (CSPPORTInfoType *) NULL);
			}
			CSPDATA (stnID, stationData.gracefulMode) = smtmsg->data.b16;
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
			else
			{
				CSPDATA (stnID, stationData.maxMasterCanConnect) =
					CSPDATA (stnID, stationData.shadowmaxMasterCanConnect);
			}
		}
#endif							/* (USE_SCHED_GRACE || USE_FULL_GRACE) */

		if (smtmsg->vendorID == xdiSMTMaxMasterCanConnect)
		{
			CSPDATA (stnID, stationData.shadowmaxMasterCanConnect)
				= smtmsg->data.b8;
			if (CSPDATA (stnID, stationData.gracefulMode) == GIM_off)
			{
				CSPDATA (stnID, stationData.maxMasterCanConnect)
					= smtmsg->data.b8;
			}
			else if (!CSPDATA (stnID, stationData.gracefulMax)
					|| (smtmsg->data.b8
						&& (CSPDATA (stnID, stationData.gracefulMax)
						>= smtmsg->data.b8)))
			{
				CSPDATA (stnID, stationData.maxMasterCanConnect)
					= smtmsg->data.b8;
			}
		}

		else if (smtmsg->vendorID == xdiSMTGIMACID)
		{
			CSPDATA (stnID, stationData.giMACID) = smtmsg->data.b16;
		}

		break;

		/*
		 * MAC Attributes
		 */
	case fddiMACRequestedPaths:
		/* set new requested paths value */
		macPtr = &CSPDATA (stnID, macData[smtmsg->entity]);
		macPtr->requestedPaths = smtmsg->data.b8;

		/* re-evaluate MAC criteria */
		CFMMACSelectionCriteria (stnID, macPtr);

		/* re-evaluate attachment criteria */
		if (stnPtr->S_Index != EMPTY_PORT)
			CFMPortSelectionCriteria (stnID,
				&CSPDATA (stnID, portData[stnPtr->S_Index]));

		else if (stnPtr->A_Index != EMPTY_PORT)
			CFMPortSelectionCriteria (stnID,
				&CSPDATA (stnID, portData[stnPtr->A_Index]));

		else if (stnPtr->B_Index != EMPTY_PORT)
			CFMPortSelectionCriteria (stnID,
				&CSPDATA (stnID, portData[stnPtr->B_Index]));
		break;

	case fddiMACMA_UnitdataEnable:
		/* notify RMT to change setting of flag */
		SendSignalMulti (stnID, SIG_RM_MA_Unitdata, smtmsg->entity,
			(uInt32) smtmsg->data.b8);
		break;

	case fddiMACUpstreamNbr:
		MCopyAddress (CSPDATA (stnID, macData[smtmsg->entity].upstreamNbr),
			smtmsg->data.addr);
		break;

	case fddiMACDupAddressTest:
		/* FSP changed the duplicate address detection status */
		SendSignalMulti (stnID, SIG_Dup_Addr, smtmsg->entity,
			(uInt32) smtmsg->data.b16);
		break;

	case fddiMACVendorAttrib:
		if (smtmsg->vendorID == xdiMACRMTDupPolicy)
		{
			CSPDATA (stnID, macData[smtmsg->entity].rmtDupPolicy)
				= smtmsg->data.b16;
		}

		else if (smtmsg->vendorID == xdiMACLocalPath)
		{
			/* if new path is requested */
			macPtr = &CSPDATA (stnID, macData[smtmsg->entity]);
			if (macPtr->localPathIndex != smtmsg->data.b16)
			{
				/* change value */
				macPtr->localPathIndex = smtmsg->data.b16;

				/* if currently on local path */
				if (macPtr->currentPath >= PI_local)
				{
					/* force MAC to isolated */
					savePaths = macPtr->availablePaths;
					macPtr->availablePaths = AP_None;
					CFMMACSelectionCriteria (stnID, macPtr);

					/* use new local path */
					macPtr->availablePaths = savePaths;
					CFMMACSelectionCriteria (stnID, macPtr);
				}
			}
		}
		break;

		/*
		 * PATH Attributes
		 */
	case fddiPATHTVXLowerBound:
	case fddiPATHT_MaxLowerBound:
	case fddiPATHMaxT_Req:
		/*
		 * Update values in PATH's data structure first.
		 */
		pathPtr = &CSPDATA (stnID, pathData[smtmsg->entity]);
		if (smtmsg->typeInfo == fddiPATHTVXLowerBound)
			pathPtr->TVXLB = smtmsg->data.b32;

		else if (smtmsg->typeInfo == fddiPATHT_MaxLowerBound)
			pathPtr->T_MaxLB = smtmsg->data.b32;

		else if (smtmsg->typeInfo == fddiPATHMaxT_Req)
			pathPtr->MaxT_Req = smtmsg->data.b32;

		/* reconfigure MACs on this path */
		for (i = 0, macPtr = CSPDATA (stnID, macData);
			i < MAX_MAC_COUNT;
			i++, macPtr++)
		{
			/* if MAC is on this path */
			/*
			 * 731-006 LJP  Previously, this section only checked cpIndex to
			 * see if the MAC was on the path. This is insufficient because
			 * cpIndex is 0 for isolated or primary. The test must check
			 * currentPath and cpIndex to determine if the MAC can be
			 * reconfigured. Also, since this loop goes through all possible
			 * MACs, it should check if the MAC is present.
			 */
			if ((macPtr->hwPresent) && (macPtr->currentPath != CP_isolated)
				&& (macPtr->cpIndex == smtmsg->entity))
			{
				/*
				 * Set new MAC operational values and reconfigure. This code
				 * relies on the fact that the MIB validates the new
				 * operational value for all MACs that have this path
				 * requested. Thus, the range of values don't have to be
				 * checked as in CFM.
				 */
				macPtr->TVX = pathPtr->TVXLB;
				macPtr->T_Max = pathPtr->T_MaxLB;
				macPtr->T_Req = pathPtr->MaxT_Req;
				ConfigureMACMulti (stnID, macPtr->ID, (Flag) ENABLE,
					macPtr->cpIndex, macPtr->TVX, macPtr->T_Max, macPtr->T_Req);
			}
		}
		break;

	case fddiPATHT_Rmode:
		/* convert from FddiTime */
		CSPDATA (stnID, pathData[smtmsg->entity].T_Rmode)
			= MFddi2Usecs (smtmsg->data.b32);
		break;

		/*
		 * PORT Attributes
		 */
	case fddiPORTConnectionPolicies:
		CSPDATA (stnID, portData[smtmsg->entity].connectionPolicies)
			= smtmsg->data.b8;
		break;

	case fddiPORTRequestedPaths:
		/* set new requested paths */
		portPtr = &CSPDATA (stnID, portData[smtmsg->entity]);
		rpPtr = (PORTRequestedPathsType *) & smtmsg->data.b32;
		portPtr->requestedPaths[RP_none] = rpPtr->reqPaths[RP_none];
		portPtr->requestedPaths[RP_tree] = rpPtr->reqPaths[RP_tree];
		portPtr->requestedPaths[RP_peer] = rpPtr->reqPaths[RP_peer];

		/* reevaluate port */
		CFMPortSelectionCriteria (stnID, portPtr);
		break;

	case fddiPORTLer_Cutoff:
		CSPDATA (stnID, portData[smtmsg->entity].lerCutoff) = smtmsg->data.b8;
		break;

	case fddiPORTLer_Alarm:
		CSPDATA (stnID, portData[smtmsg->entity].lerAlarm) = smtmsg->data.b8;
		break;

	case fddiPORTMACLoop_Time:
		/* convert from FddiTime */
		CSPDATA (stnID, portData[smtmsg->entity].MACLoop_Time) =
			MFddi2Usecs (smtmsg->data.b32);
		break;

	case fddiPORTMaint_LS:
		/* convert from SMT to CSP line state value */
		portPtr = &CSPDATA (stnID, portData[smtmsg->entity]);
		switch (smtmsg->data.b8)
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

		/* post signal to PCM to affect change */
		SendSignalMulti (stnID, SIG_PC_Maint, smtmsg->entity,
			(uInt32) portPtr->Maint_LS);
		break;

	case fddiPORTVendorAttrib:
		if (smtmsg->vendorID == xdiPORTLocalPath)
		{
			/* if new path is requested */
			portPtr = &CSPDATA (stnID, portData[smtmsg->entity]);
			if (portPtr->localPathIndex != smtmsg->data.b16)
			{
				/* change value */
				portPtr->localPathIndex = smtmsg->data.b16;

				/* if currently on local path */
				if (portPtr->currentPath >= PI_local)
				{
					/* force MAC to isolated */
					savePaths = portPtr->availablePaths;
					portPtr->availablePaths = AP_None;
					CFMPortSelectionCriteria (stnID, portPtr);

					/* use new local path */
					portPtr->availablePaths = savePaths;
					CFMPortSelectionCriteria (stnID, portPtr);
				}
			}
		}
		break;
	}

	return;
}

/*
 * Only include the routines to process flexible configuration message
 * if the XM_USE_FLEX_CONF constant is defined. The two routine affected
 * are: ProcessCSPMACChange() and ProcessCSPPORTChange().
 */
#ifdef XM_USE_FLEX_CONF

void
#ifdef __STDC__
ProcessCSPMACChange (SMTMessage * smtmsg)
#else
ProcessCSPMACChange (smtmsg)
		SMTMessage      *smtmsg;
#endif
/*********************************************************************
Function:	Process a flexible configuration change for a MAC.
Parameters:	smtmsg	= address of message buffer.
Input:		smtmsg	= contains message to process.
Output:		Changes global data and/or posts signals to the signal
			queue depending upon the message.
Return:		None.
Notes:		None.
Modification History:
*********************************************************************/
{
CSPMACInfoType  *macPtr;				/* MAC element pointer */
MIBMACAddType   *addInfo;				/* add msg info */
MIBMACInsertType *insertInfo;			/* insert msg info */
PathConfigType  *macList;				/* MAC's config list */
PathConfigType  *newList;				/* Message config list */
PathConfigType  *nbrList;				/* next/prev config list */
uInt16           pathID;				/* loop counter */
uInt16           stnID;					/* station in msg */

	/* get MAC array element to use */
	stnID = smtmsg->stnID;
	macPtr = &(CSPDATA (stnID, macData[smtmsg->entity]));

	/* select message type */
	switch (smtmsg->type)
	{
	case MIB_EVENT_ADD_MAC:
		addInfo = smtmsg->p1.MACAddBuf;

		/*
		 * If index is set, then this MAC is either still in use or was not
		 * deleted. Ignore this request.
		 */
		if (macPtr->index != 0)
		{
			break;
		}
		else
		{
			/* initialize info */
			macPtr->requestedPaths = addInfo->requestedPaths;
			macPtr->index = addInfo->index;
			macPtr->unitdataEnable = addInfo->unitdataEnable;
			macPtr->rmtDupPolicy = addInfo->rmtDupPolicy;
			macPtr->localPathIndex = addInfo->localPathIndex;
			for (pathID = 0, macList = macPtr->configList,
				newList = addInfo->configList;
				pathID < MAX_PATH_COUNT;
				pathID++, macList++, newList++)
			{
				macList->nextIndex = newList->nextIndex;
				macList->nextType = newList->nextType;
				macList->prevIndex = newList->prevIndex;
				macList->prevType = newList->prevType;

				/* if it doesn't point to itself */
				if (macList->nextIndex != macPtr->ID)
				{
					/* set next object's previous values to this MAC */
					if (macList->nextType == RT_mac)
						nbrList = &CSPDATA (stnID,
							macData[macList->nextIndex].configList[pathID]);
					else
						nbrList = &CSPDATA (stnID,
							portData[macList->nextIndex].configList[pathID]);

					nbrList->prevIndex = macPtr->ID;
					nbrList->prevType = RT_mac;

					/* set previous object's next values to this MAC */
					if (macList->prevType == RT_mac)
						nbrList = &CSPDATA (stnID,
							macData[macList->prevIndex].configList[pathID]);
					else
						nbrList = &CSPDATA (stnID,
							portData[macList->prevIndex].configList[pathID]);

					nbrList->nextIndex = macPtr->ID;
					nbrList->nextType = RT_mac;
				}
			}

			/* add MAC */
			CSPDATA (stnID, stationData.macCount)++;
		}


		break;

	case MIB_EVENT_INSERT_MAC:
		insertInfo = smtmsg->p1.MACInsertBuf;

		/* if index is not set */
		if (macPtr->index == 0)
		{
			/* then this MAC has not been added */
			break;
		}
		else
		{
			/* initialize info */
			macPtr->hwPresent = PRESENT;
			macPtr->T_MaxCap = insertInfo->T_MaxCap;
			macPtr->TVXCap = insertInfo->TVXCap;
			macPtr->availablePaths = insertInfo->availablePaths;
			MCopyAddress (macPtr->SMTAddress,
				insertInfo->SMTAddress);

			/* if station is connecting */
			if (CSPDATA (stnID, stationData.ecState) == EC_IN)
			{
				/* enable RMT */
				SendSignalMulti (stnID, SIG_RM_Available,
					macPtr->ID, (uInt32) SET);

				/* run configuration criteria */
				CFMMACSelectionCriteria (stnID, macPtr);
			}
		}
		break;

	case MIB_EVENT_REMOVE_MAC:
		macPtr->hwPresent = NOTPRESENT;

		/* remove MAC from selection criteria */
		if (macPtr->currentPath == CP_primary)
		{
			/* decrement # MACs on primary path */
			CSPDATA (stnID, stationData.Pri_MAC_Avail)--;
			if (CSPDATA (stnID, stationData.Pri_MAC_Avail) == 0)
			{
				/* no more, so reevaluate attachment ports */
				ReevaluateAttachment (stnID);
			}

			/*
			 * Scrub primary path.
			 */
			CSPDATA (stnID, pathData[PII_primary].scrubRequired) = SET;
		}
		else if (macPtr->currentPath == CP_secondary)
		{
			/* decrement # MACs on secondary path */
			CSPDATA (stnID, stationData.Sec_MAC_Avail)--;
			if (CSPDATA (stnID, stationData.Sec_MAC_Avail) == 0)
			{
				/* no more, so reevaluate attachment ports */
				ReevaluateAttachment (stnID);
			}

			/*
			 * Scrub secondary path.
			 */
			CSPDATA (stnID, pathData[PII_secondary].scrubRequired) = SET;
		}

		/*
		 * If on a local path, then scrub that path.
		 */
		else if (macPtr->currentPath == CP_local)
		{
			CSPDATA (stnID, pathData[macPtr->localPathIndex - 1].scrubRequired)
				= SET;
		}

		/* clear hardware-dependent info */
		InitCSPMACHWData (macPtr);

		/*
		 * Check if scrubbing is needed.
		 */
		CheckScrubbing (stnID);

		break;

	case MIB_EVENT_DELETE_MAC:
		for (pathID = 0, macList = macPtr->configList,
			newList = addInfo->configList;
			pathID < MAX_PATH_COUNT;
			pathID++, macList++, newList++)
		{
			/* if it doesn't point to itself */
			if (macList->nextIndex != macPtr->ID)
			{
				/* delete MAC from path config list */
				if (macList->nextType == RT_mac)
					nbrList = &CSPDATA (stnID,
						macData[macList->nextIndex].configList[pathID]);
				else
					nbrList = &CSPDATA (stnID,
						portData[macList->nextIndex].configList[pathID]);

				nbrList->prevIndex = macList->prevIndex;
				nbrList->prevType = macList->prevType;

				if (macList->prevType == RT_mac)
					nbrList = &CSPDATA (stnID,
						macData[macList->prevIndex].configList[pathID]);
				else
					nbrList = &CSPDATA (stnID,
						portData[macList->prevIndex].configList[pathID]);

				nbrList->nextIndex = macList->nextIndex;
				nbrList->nextType = macList->nextType;
			}
		}

		/* clear base info (clears index) */
		InitCSPMACData (macPtr);

		/* delete MAC */
		CSPDATA (stnID, stationData.macCount)--;
		break;
	}

	return;
}

void
#ifdef __STDC__
ProcessCSPPORTChange (SMTMessage * smtmsg)
#else
ProcessCSPPORTChange (smtmsg)
		SMTMessage      *smtmsg;
#endif
/*********************************************************************
Function:	Process a flexible configuration change for a PORT.
Parameters:	smtmsg	= address of message buffer.
Input:		smtmsg	= contains message to process.
Output:		Changes global data and/or posts signals to the signal
			queue depending upon the message.
Return:		None.
Notes:		None.
Modification History:
*********************************************************************/
{
CSPStationInfoType *stnPtr;				/* Station info */
CSPPORTInfoType *portPtr;				/* PORT element pointer */
MIBPORTAddType  *addInfo;				/* add msg info */
MIBPORTInsertType *insertInfo;			/* insert msg info */
PathConfigType  *portList;				/* PORT's config list */
PathConfigType  *newList;				/* Message config list */
PathConfigType  *nbrList;				/* next/prev config list */
uInt16           pathID;				/* loop counter */
uInt16           stnID;					/* station in msg */

	/* get PORT array element to use */
	stnID = smtmsg->stnID;
	stnPtr = &(CSPDATA (stnID, stationData));
	portPtr = &(CSPDATA (stnID, portData[smtmsg->entity]));

	/* select message type */
	switch (smtmsg->type)
	{
	case MIB_EVENT_ADD_PORT:
		addInfo = smtmsg->p1.PORTAddBuf;

		/*
		 * If index is set, then this PORT is either still in use or was not
		 * deleted. Ignore this request.
		 */
		if (portPtr->index != 0)
		{
			break;
		}
		else
		{
			/* initialize info */
			portPtr->PC_Type = addInfo->PC_Type;
			switch (portPtr->PC_Type)
			{
			case PT_a:
				portPtr->requestedPaths[RP_none]
					= addInfo->requestedPaths[RP_none] & RP_A_NONE_ALLOWED;
				portPtr->requestedPaths[RP_tree]
					= addInfo->requestedPaths[RP_tree] & RP_A_TREE_ALLOWED;
				portPtr->requestedPaths[RP_peer]
					= addInfo->requestedPaths[RP_peer] & RP_A_PEER_ALLOWED;

				/* set station index value as well */
				stnPtr->attachCount++;
				stnPtr->A_Index = portPtr->ID;
				break;

			case PT_b:
				portPtr->requestedPaths[RP_none]
					= addInfo->requestedPaths[RP_none] & RP_B_NONE_ALLOWED;
				portPtr->requestedPaths[RP_tree]
					= addInfo->requestedPaths[RP_tree] & RP_B_TREE_ALLOWED;
				portPtr->requestedPaths[RP_peer]
					= addInfo->requestedPaths[RP_peer] & RP_B_PEER_ALLOWED;

				/* set station index value as well */
				stnPtr->attachCount++;
				stnPtr->B_Index = portPtr->ID;
				break;

			case PT_s:
				portPtr->requestedPaths[RP_none]
					= addInfo->requestedPaths[RP_none] & RP_S_NONE_ALLOWED;
				portPtr->requestedPaths[RP_tree]
					= addInfo->requestedPaths[RP_tree] & RP_S_TREE_ALLOWED;
				portPtr->requestedPaths[RP_peer]
					= addInfo->requestedPaths[RP_peer] & RP_S_PEER_ALLOWED;

				/* set station index value as well */
				stnPtr->attachCount++;
				stnPtr->S_Index = portPtr->ID;
				break;

			case PT_m:
				portPtr->requestedPaths[RP_none]
					= addInfo->requestedPaths[RP_none] & RP_M_NONE_ALLOWED;
				portPtr->requestedPaths[RP_tree]
					= addInfo->requestedPaths[RP_tree] & RP_M_TREE_ALLOWED;
				stnPtr->masterCount++;
				break;
			}
			portPtr->index = addInfo->index;
			portPtr->lerCutoff = addInfo->lerCutoff;
			portPtr->lerAlarm = addInfo->lerAlarm;
			portPtr->localPathIndex = addInfo->localPathIndex;
			for (pathID = 0, portList = portPtr->configList,
				newList = addInfo->configList;
				pathID < MAX_PATH_COUNT;
				pathID++, portList++, newList++)
			{
				portList->nextIndex = newList->nextIndex;
				portList->nextType = newList->nextType;
				portList->prevIndex = newList->prevIndex;
				portList->prevType = newList->prevType;

				/* if it doesn't point to itself */
				if (portList->nextIndex != portPtr->ID)
				{
					/* set next object's previous pointers to this PORT */
					if (portList->nextType == RT_mac)
						nbrList = &CSPDATA (stnID,
							macData[portList->nextIndex].configList[pathID]);
					else
						nbrList = &CSPDATA (stnID,
							portData[portList->nextIndex].configList[pathID]);

					nbrList->prevIndex = portPtr->ID;
					nbrList->prevType = RT_port;

					if (portList->prevType == RT_mac)
						nbrList = &CSPDATA (stnID,
							macData[portList->prevIndex].configList[pathID]);
					else
						nbrList = &CSPDATA (stnID,
							portData[portList->prevIndex].configList[pathID]);

					nbrList->nextIndex = portPtr->ID;
					nbrList->nextType = RT_port;
				}
			}

			portPtr->MACLoop_Time = addInfo->MACLoop_Time;
			portPtr->Maint_LS = addInfo->Maint_LS;
		}
		break;

	case MIB_EVENT_INSERT_PORT:
		insertInfo = smtmsg->p1.PORTInsertBuf;

		/* if index is not set */
		if (portPtr->index == 0)
		{
			/* then this PORT has not been added */
			break;
		}
		else
		{
			/* initialize info */
			portPtr->hwPresent = PRESENT;
			portPtr->availablePaths = insertInfo->availablePaths;
			portPtr->conCapabilities = insertInfo->conCapabilities;

			/*
			 * PC_Start is NOT issued automatically like RM_Available. This
			 * allows the implementer to select when the port should be
			 * activated.
			 */
		}
		break;

	case MIB_EVENT_REMOVE_PORT:
		portPtr->hwPresent = NOTPRESENT;

		/* if M port */
		if (portPtr->PC_Type == PT_m)
		{
			/* remove PORT from selection criteria */
			if (portPtr->currentPath == CP_primary)
			{
				/* decrement # M ports on primary path */
				if ((CSPDATA (stnID,
							stationData.Pri_MPort_Active)--) == 0)
				{
					/* no more, reevaluate attach ports */
					ReevaluateAttachment (stnID);
				}
			}
			else if (portPtr->currentPath == CP_secondary)
			{
				/* decrement # M ports on secondary path */
				if ((CSPDATA (stnID,
							stationData.Sec_MPort_Active)--) == 0)
				{
					/* no more, reevaluate attach ports */
					ReevaluateAttachment (stnID);
				}
			}
		}
		/* clear hardware-dependent info */
		InitCSPPORTHWData (portPtr);

		break;

	case MIB_EVENT_DELETE_PORT:
		/* remove port from station index if A, B or S */
		switch (portPtr->PC_Type)
		{
		case PT_a:
			stnPtr->A_Index = MAX_PORT_COUNT;
			stnPtr->attachCount--;
			break;

		case PT_b:
			stnPtr->B_Index = MAX_PORT_COUNT;
			stnPtr->attachCount--;
			break;

		case PT_s:
			stnPtr->S_Index = MAX_PORT_COUNT;
			stnPtr->attachCount--;
			break;

		case PT_m:
			stnPtr->masterCount--;
			break;
		}

		/* remove PORT from path list */
		for (pathID = 0, portList = portPtr->configList,
			newList = addInfo->configList;
			pathID < MAX_PATH_COUNT;
			pathID++, portList++, newList++)
		{
			/* if it doesn't point to itself */
			if (portList->nextIndex != portPtr->ID)
			{
				/* delete port from path config list */
				if (portList->nextType == RT_mac)
					nbrList = &CSPDATA (stnID,
						macData[portList->nextIndex].configList[pathID]);
				else
					nbrList = &CSPDATA (stnID,
						portData[portList->nextIndex].configList[pathID]);

				nbrList->prevIndex = portList->prevIndex;
				nbrList->prevType = portList->prevType;

				if (portList->prevType == RT_mac)
					nbrList = &CSPDATA (stnID,
						macData[portList->prevIndex].configList[pathID]);
				else
					nbrList = &CSPDATA (stnID,
						portData[portList->prevIndex].configList[pathID]);

				nbrList->nextIndex = portList->nextIndex;
				nbrList->nextType = portList->nextType;
			}
		}

		/* clear base info (clears index) */
		InitCSPPORTData (portPtr);
		break;
	}

	return;
}

#endif							/* XM_USE_FLEX_CONF */

void
#ifdef __STDC__
ProcessCSPMessageMulti (SMTMessage * smtmsg)
#else
ProcessCSPMessageMulti (smtmsg)
		SMTMessage      *smtmsg;
#endif
/*********************************************************************
Function:	Process an SMT message.
Parameters:	smtmsg	= address of message buffer.
Input:		smtmsg	= contains message to process.
Output:		Changes global data and/or posts signals to the signal
			queue depending upon the message.
Return:		None.
Notes:		None.
Modification History:
*********************************************************************/
{
uInt16           stnID;					/* station ID from message */
uInt16           i;						/* loop counter */

	stnID = smtmsg->stnID;

	/*
	 * Select message type.
	 */
	switch (smtmsg->type)
	{

		/*
		 * Event messages.
		 */
	case MIB_EVENT_NOTIFY_CHANGE:
		ProcessCSPMIBChange (smtmsg);
		break;

/*
 * Only call these message processing routines if flexible configuration
 * is supported by the system.
 */
#ifdef XM_USE_FLEX_CONF

	case MIB_EVENT_ADD_MAC:
	case MIB_EVENT_INSERT_MAC:
	case MIB_EVENT_REMOVE_MAC:
	case MIB_EVENT_DELETE_MAC:
		ProcessCSPMACChange (smtmsg);
		break;

	case MIB_EVENT_ADD_PORT:
	case MIB_EVENT_INSERT_PORT:
	case MIB_EVENT_REMOVE_PORT:
	case MIB_EVENT_DELETE_PORT:
		ProcessCSPPORTChange (smtmsg);
		break;

#endif							/* XM_USE_FLEX_CONF */

		/*
		 * Action messages.
		 */
	case CSP_ACTION_FDDI_SMT:

		/*
		 * fddiSMTStationAction
		 */
		switch (smtmsg->typeInfo)
		{
		case SMTAction_connect:
			SendSignalMulti (stnID, SIG_Connect, 0, (uInt32) 0);
			break;

		case SMTAction_disconnect:
			SendSignalMulti (stnID, SIG_Disconnect, 0, (uInt32) 0);
			break;

		case SMTAction_path_Test:
			/* set Path_Test variable in ECM */
			SendSignalMulti (stnID, SIG_Path_Test, 0, (uInt32) PT_Pending);
			break;

		case SMTAction_disable_a:
			/* cause PC_Disable on A if A is in peer */
			i = CSPDATA (stnID, stationData.A_Index);
			if (CSPDATA (stnID, portData[i].PC_Mode) == PC_Mode_Peer)
			{
				SendSignalMulti (stnID, SIG_PC_Disable, i, (uInt32) 0);
			}
			break;

		case SMTAction_disable_b:
			/* cause PC_Disable on B if B is in peer */
			i = CSPDATA (stnID, stationData.B_Index);
			if (CSPDATA (stnID, portData[i].PC_Mode) == PC_Mode_Peer)
			{
				SendSignalMulti (stnID, SIG_PC_Disable, i, (uInt32) 0);
			}
			break;

		case SMTAction_disable_m:
			/* cause PC_Disable on all M ports */
			for (i = 0; i < MAX_PORT_COUNT; i++)
			{
				if (CSPDATA (stnID, portData[i].PC_Type) == PT_m)
				{
					SendSignalMulti (stnID, SIG_PC_Disable, i, (uInt32) 0);
				}
			}
			break;

		default:
			break;
		}

		break;

	case CSP_ACTION_FDDI_MAC:

		/*
		 * fddiMACAction
		 */
		break;

	case CSP_ACTION_FDDI_PORT:

		/*
		 * fddiPORTAction
		 */
		switch (smtmsg->typeInfo)
		{
		case PORTAction_maintport:

			/*
			 * Take maintenance line state from data. Convert from SMT to CSP
			 * line state value.
			 */
			switch (smtmsg->data.b8)
			{
			case LS_qls:
				CSPDATA (stnID, portData[smtmsg->entity].Maint_LS) = QLS;
				break;

			case LS_ils:
				CSPDATA (stnID, portData[smtmsg->entity].Maint_LS) = ILS;
				break;

			case LS_mls:
				CSPDATA (stnID, portData[smtmsg->entity].Maint_LS) = MLS;
				break;

			case LS_hls:
				CSPDATA (stnID, portData[smtmsg->entity].Maint_LS) = HLS;
				break;

			case LS_pdr:
				CSPDATA (stnID, portData[smtmsg->entity].Maint_LS) = PDRLS;
				break;
			}

			SendSignalMulti (stnID, SIG_PC_Maint, smtmsg->entity,
				(uInt32) CSPDATA (stnID, portData[smtmsg->entity].Maint_LS));
			break;

		case PORTAction_enableport:
			SendSignalMulti (stnID, SIG_PC_Enable, smtmsg->entity, (uInt32) 0);
			break;

		case PORTAction_disableport:
			SendSignalMulti (stnID, SIG_PC_Disable, smtmsg->entity, (uInt32) 0);
			break;

		case PORTAction_startport:
			/* allow PC_Start only if ECM is IN */
			if (CSPDATA (stnID, stationData.ecState) == EC_IN)
				SendSignalMulti (stnID, SIG_PC_Start,
					smtmsg->entity, (uInt32) 0);
			break;

		case PORTAction_stopport:
			/* allow PC_Stop only if ECM is IN */
			if (CSPDATA (stnID, stationData.ecState) == EC_IN)
				SendSignalMulti (stnID, SIG_PC_Stop,
					smtmsg->entity, (uInt32) 0);
			break;
		}
		break;


		/*
		 * Response messages.
		 */

		/*
		 * Local messages.
		 */
	default:
		LocalCSPMessages (smtmsg);
		break;
	}

	return;
}


/*
 * If the constant MULTIPLE_STATION is not defined, then the system is being
 * compiled for single station use. In this case, the following routine(s)
 * need to be defined for the older single station calling sequence.
 */
#ifndef MULTIPLE_STATION

void
#ifdef __STDC__
ProcessCSPMessage (SMTMessage * smtmsg)
#else
ProcessCSPMessage (smtmsg)
		SMTMessage      *smtmsg;
#endif
{
	smtmsg->stnID = FIRST_STATION;
	ProcessCSPMessageMulti (smtmsg);
	return;
}

#endif							/* MULTIPLE_STATION */
