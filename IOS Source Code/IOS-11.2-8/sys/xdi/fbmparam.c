/* $Id: fbmparam.c,v 3.2.60.1 1996/03/18 22:51:34 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/xdi/fbmparam.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: fbmparam.c,v $
 * Revision 3.2.60.1  1996/03/18  22:51:34  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.1  1996/03/07  11:20:36  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2  1995/11/17  18:09:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:24:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_fbmparam.c xm7_v3.1.1:cset.0166:15:15 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Frame Based Services Process Module
		
		Frame Parameter Processing Module
		
		File:			xm7_fbmparam.c
		Created:		08/27/90

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
		
		This module contains routines that format the SMT frame
		parameters.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

#include	<ciscolib.h>
#include        "cschdrsif.h"
#include        "xdihdrsif.h"

#include	"smterror.h"
#include	"fddihdr.h"
#include	"fbmframe.h"
#include	"fbmhdr.h"
#include	"fbmglbl.h"
#include	"smtmsg.h"
#include	"fbmproto.h"
#include	"multistn.h"


/*********************************************************************
	Frame Parameter Support Routines
*********************************************************************/

/*
*	Array of TLV MIB attributes for MIB groups. In this SMT version
*	no attribute group has more than 14 parameters. Setting array
*	size to 15 provides enough room plus a little extra.
*/
TLVParamType     mibGrp[15];


/*********************************************************************
	Frame Parameter Routines
*********************************************************************/

uInt16
#ifdef __STDC__
UNA_Param (UNAParamType * param, FBMMACInfoType * macPtr)
#else
UNA_Param (param, macPtr)
		UNAParamType    *param;
		FBMMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Create the Upstream Neighbor Address parameter.
Parameters:	param	= pointer to location where UNA parameter should start.
			macPtr	= MAC element of MAC sending frame.
Input:		Uses station data.
Output:		param	= contains the UNA SMT parameter.
Return:		Length in characters of the parameter including
			the type and length fields.
Modification History:
*********************************************************************/
{
	/* fill in header */
	param->Type = Mhtons (UNA_PARAM_TYPE);
	param->Length = Mhtons (SIZE_UNAParamType - SIZE_ParamHdrType);

	/* fill in parameter */
	MCopyAddress (param->UNA, macPtr->UNA);

	return (SIZE_UNAParamType);
}

uInt16
#ifdef __STDC__
StationDescriptor_Param (uInt16 stnID, StationDescParamType * param)
#else
StationDescriptor_Param (stnID, param)
		uInt16           stnID;
		StationDescParamType *param;
#endif
/*********************************************************************
Function:	Create the Station Descriptor parameter.
Parameters:	stnID	= index of station to use.
			param	= pointer to location where parameter should start.
Input:		Uses station data.
Output:		param	= contains the Station Descriptor parameter.
Return:		Length in characters of the parameter including
			the type and length fields.
Modification History:
*********************************************************************/
{
	/* fill in header */
	param->Type = Mhtons (STATION_DESC_PARAM_TYPE);
	param->Length = Mhtons (SIZE_StationDescParamType - SIZE_ParamHdrType);

	/* fill in parameter */
	param->NodeClass = (FBMDATA (stnID, fbmStationData.masterCount) == 0)
		? SMT_Type_Station : SMT_Type_Concentrator;
	param->Mac_Ct = FBMDATA (stnID, fbmStationData.macPresent);
	param->NonMaster_Ct = FBMDATA (stnID, fbmStationData.attachPresent);
	param->Master_Ct = FBMDATA (stnID, fbmStationData.masterPresent);

	return (SIZE_StationDescParamType);
}

uInt16
#ifdef __STDC__
StationState_Param (uInt16 stnID, StationStateParamType * param,
	uInt16 macID)
#else
StationState_Param (stnID, param, macID)
		uInt16           stnID;
		StationStateParamType *param;
		uInt16           macID;
#endif
/*********************************************************************
Function:	Create the Station State parameter.
Parameters:	stnID	= index of station to use.
			param	= pointer to location where parameter should start.
			macID	= FBM MAC index sending this frame.
Input:		Uses fbmStationData and MIB information.
Output:		param	= contains the SMT frame parameter.
Return:		Length in characters of the parameter including
			the type and length fields.
Modification History:
*********************************************************************/
{
TLVParamType     mibData;				/* get request buffer */
uInt16           nbrA;					/* A port's neighbor */
uInt16           nbrB;					/* B port's neighbor */
FBMStationInfoType *stnPtr;				/* station data */

	/* get station info */
	stnPtr = &(FBMDATA (stnID, fbmStationData));

	/* fill in header */
	param->Type = Mhtons (STATION_STATE_PARAM_TYPE);
	param->Length = Mhtons (SIZE_StationStateParamType - SIZE_ParamHdrType);

	/* init parameter */
	param->Topology = 0;
	param->DuplAddress = 0;

	/*
	 * Set Topology field
	 */

	/* set PeerWrap */
	mibData.paramType = fddiSMTPeerWrapFlag;
	mibData.paramLen = 0;
	FBMGetMIBAttrMulti (stnID, sizeof (mibData), (Void_p) & mibData);
	param->Topology |= (mibData.SMTPARAM8) ? Topology_Peer_Wrap : Topology_Thru;

	/* get current configuration */
	mibData.paramType = fddiSMTCF_State;
	mibData.paramLen = 0;
	FBMGetMIBAttrMulti (stnID, sizeof (mibData), (Void_p) & mibData);

	/* set unattached concentrator if isolated or in any local state */
	if ((stnPtr->masterCount > 0) && (mibData.SMTPARAM16 <= CF_local_s))
		param->Topology |= Topology_Unattached_Conc;

	/* set rooted station */
	if (stnPtr->attachCount == 1)
	{
		/* get S PC_Neighbor */
		mibData.paramType = fddiPORTNeighborType;
		mibData.paramLen = 4;
		mibData.PORTINDEX = FBMDATA (stnID, fbmStationData.S_Index);
		mibData.PORTPARAM16 = PT_none;	/* in case there's no S port */
		if (mibData.PORTINDEX < MAX_PORT_COUNT)
		{
			/* S port is listed in MIB */
			if (FBMGetMIBAttrMulti (stnID, sizeof (mibData),
					(Void_p) & mibData) != RC_Success)
			{
				/* S port HW may not be present */
				mibData.PORTPARAM16 = PT_none;
			}
		}
		/* if not attached or attached to A, B or S */
		if (mibData.PORTPARAM16 != PT_m)
			/* mark as rooted */
			param->Topology |= Topology_Rooted_Station;
	}

	else
	{
		/*
		 * The value of the neighbor port type needs to be qualified with the
		 * connect state of the port to determine if the ring is twisted. If
		 * the port is no longer connected, then the old neighbor type
		 * information is not applicable for determining the twisted state.
		 */
		nbrA = PT_none;
		/* if A port exists */
		if (stnPtr->A_Index < MAX_PORT_COUNT)
		{
			/* get A port connect state */
			mibData.paramType = fddiPORTConnectState;
			mibData.paramLen = 4;
			mibData.PORTINDEX = stnPtr->A_Index;
			if (FBMGetMIBAttrMulti (stnID, sizeof (mibData), (Void_p) & mibData)
				== RC_Success)
			{
				/* get neighbor if connection is active */
				if (mibData.PORTPARAM16 == CS_active)
				{
					mibData.paramType = fddiPORTNeighborType;
					mibData.paramLen = 4;
					mibData.PORTINDEX = stnPtr->A_Index;
					if (FBMGetMIBAttrMulti (stnID, sizeof (mibData),
							(Void_p) & mibData) == RC_Success)
					{
						/* A port neighber retrieved */
						nbrA = mibData.PORTPARAM16;
					}
				}
			}
		}

		nbrB = PT_none;
		/* if B port exists */
		if (stnPtr->B_Index < MAX_PORT_COUNT)
		{
			/* get B port connect state */
			mibData.paramType = fddiPORTConnectState;
			mibData.paramLen = 4;
			mibData.PORTINDEX = stnPtr->B_Index;
			if (FBMGetMIBAttrMulti (stnID, sizeof (mibData), (Void_p) & mibData)
				== RC_Success)
			{
				/* get neighbor if connection is active */
				if (mibData.PORTPARAM16 == CS_active)
				{
					mibData.paramType = fddiPORTNeighborType;
					mibData.paramLen = 4;
					mibData.PORTINDEX = stnPtr->B_Index;
					if (FBMGetMIBAttrMulti (stnID, sizeof (mibData),
							(Void_p) & mibData) == RC_Success)
					{
						/* B port neighber retrieved */
						nbrB = mibData.PORTPARAM16;
					}
				}
			}
		}

		/* set topology bits */
		if (nbrA == PT_a)
			param->Topology |= Topology_Twisted_AA;

		if (nbrB == PT_b)
			param->Topology |= Topology_Twisted_BB;

		if ((nbrA != PT_m) && (nbrB != PT_m))
			param->Topology |= Topology_Rooted_Station;
	}

	if (FBMDATA (stnID, fbmSRPData.srState) != SR_DISABLED)
		param->Topology |= Topology_SRF;

	/*
	 * Get SBA info on all paths.
	 */
	mibData.paramType = fddiSMTVendorAttrib;
	mibData.paramLen = 4;
	mibData.SMTVENDORID = xdiSMTSynchronousServiceFlag;
	if (FBMGetMIBAttrMulti (stnID, sizeof (mibData), (Void_p) & mibData)
		== RC_Success)
	{
		/* if sync service flag set */
		if (mibData.SMTVEND8)
			/* set bit in topology field */
			param->Topology |= Topology_SBA;
	}

	/*
	 * Set DuplAddress field
	 */
	if (FBMDATA (stnID, fbmMACData[macID].myDuplicate))
		param->DuplAddress |= DuplAddr_My_Duplicate;

	if (FBMDATA (stnID, fbmMACData[macID].myUNADuplicate))
		param->DuplAddress |= DuplAddr_My_UNA_Duplicate;

	return (SIZE_StationStateParamType);
}

uInt16
#ifdef __STDC__
MsgTimeStamp_Param (uInt16 stnID, MsgTimeStampParamType * param)
#else
MsgTimeStamp_Param (stnID, param)
		uInt16           stnID;
		MsgTimeStampParamType *param;
#endif
/*********************************************************************
Function:	Create the Message Time Stamp parameter.
Parameters:	stnID	= index of station to use.
			param	= pointer to location where parameter should start.
Input:		Get data from MIB.
Output:		param	= contains the SMT MsgTimeStamp parameter.
Return:		Length in characters of the parameter including
			the type and length fields.
Modification History:
*********************************************************************/
{
TLVParamType     mibData;				/* get request buffer */

	/* fill in header */
	param->Type = Mhtons (MSG_TIME_STAMP_PARAM_TYPE);
	param->Length = Mhtons (SIZE_MsgTimeStampParamType - SIZE_ParamHdrType);

	/* fill in parameter */
	mibData.paramType = fddiSMTTimeStamp;
	mibData.paramLen = 0;
	FBMGetMIBAttrMulti (stnID, sizeof (mibData), (Void_p) & mibData);
	MEMCOPY (&param->MsgTimeStamp, mibData.SMTOTHER, mibData.paramLen);

	/* Convert to network order */
	param->MsgTimeStamp.hiword = Mhtonl (param->MsgTimeStamp.hiword);
	param->MsgTimeStamp.loword = Mhtonl (param->MsgTimeStamp.loword);

	return (SIZE_MsgTimeStampParamType);
}

uInt16
#ifdef __STDC__
StationPolicies_Param (uInt16 stnID, StationPoliciesParamType * param)
#else
StationPolicies_Param (stnID, param)
		uInt16           stnID;
		StationPoliciesParamType *param;
#endif
/*********************************************************************
Function:	Create the Station Policies parameter.
Parameters:	stnID	= index of station to use.
			param	= pointer to location where parameter should start.
Input:		Get data from MIB.
Output:		param	= contains the SMT Station Policies parameter.
Return:		Length in characters of the parameter including
			the type and length fields.
Modification History:
*********************************************************************/
{
TLVParamType     mibData;				/* get request buffer */

	/* fill in header */
	param->Type = Mhtons (STATION_POLICIES_PARAM_TYPE);
	param->Length = Mhtons (SIZE_StationPoliciesParamType - SIZE_ParamHdrType);

	/* fill in parameter */
	param->ConfigPolicy = 0;
	param->ConnectionPolicy = 0;

	/*
	 * Hold configuration policy not supported by this code
	 */
	param->ConfigPolicy = 0;

	/*
	 * Get connection policy and convert to frame format
	 */
	mibData.paramType = fddiSMTConnectionPolicy;
	mibData.paramLen = 0;
	FBMGetMIBAttrMulti (stnID, sizeof (mibData), (Void_p) & mibData);

	/* S-M and M-* match the bits in fddiSMTConnectionPolicy */
	param->ConnectionPolicy = mibData.SMTPARAM16
		& (FBM_rejectS_M | FBM_rejectM_A | FBM_rejectM_B
		| FBM_rejectM_S | FBM_rejectM_M);

	/* translate other bits */
	if (mibData.SMTPARAM16 & Con_rejectA_A)
		param->ConnectionPolicy |= FBM_rejectA_A;
	if (mibData.SMTPARAM16 & Con_rejectA_B)
		param->ConnectionPolicy |= FBM_rejectA_B;
	if (mibData.SMTPARAM16 & Con_rejectA_S)
		param->ConnectionPolicy |= FBM_rejectA_S;
	if (mibData.SMTPARAM16 & Con_rejectA_M)
		param->ConnectionPolicy |= FBM_rejectA_M;
	if (mibData.SMTPARAM16 & Con_rejectB_A)
		param->ConnectionPolicy |= FBM_rejectB_A;
	if (mibData.SMTPARAM16 & Con_rejectB_B)
		param->ConnectionPolicy |= FBM_rejectB_B;
	if (mibData.SMTPARAM16 & Con_rejectB_S)
		param->ConnectionPolicy |= FBM_rejectB_S;
	if (mibData.SMTPARAM16 & Con_rejectB_M)
		param->ConnectionPolicy |= FBM_rejectB_M;
	if (mibData.SMTPARAM16 & Con_rejectS_A)
		param->ConnectionPolicy |= FBM_rejectS_A;
	if (mibData.SMTPARAM16 & Con_rejectS_B)
		param->ConnectionPolicy |= FBM_rejectS_B;
	if (mibData.SMTPARAM16 & Con_rejectS_S)
		param->ConnectionPolicy |= FBM_rejectS_S;

	/* convert to network order */
	param->ConfigPolicy = Mhtons (param->ConfigPolicy);
	param->ConnectionPolicy = Mhtons (param->ConnectionPolicy);

	return (SIZE_StationPoliciesParamType);
}

uInt16
#ifdef __STDC__
MACNeighbors_Param (MACNeighborsParamType * param, FBMMACInfoType * macPtr)
#else
MACNeighbors_Param (param, macPtr)
		MACNeighborsParamType *param;
		FBMMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Create the MAC Neighbors parameter.
Parameters:	param	= pointer to location where parameter should start.
			macPtr	= MAC element of MAC sending the frame.
Input:		Uses MAC data.
Output:		param	= contains the SMT MAC Neighbors  parameter.
Return:		Length in characters of the parameter including
		the type and length fields.
Modification History:
*********************************************************************/
{
uInt16           length;				/* length returned */

	/* only process for physically present MACs */
	length = 0;
	if (macPtr->hwPresent)
	{
		/* fill in header */
		param->Type = Mhtons (MAC_NEIGHBORS_PARAM_TYPE);
		param->Length = Mhtons (SIZE_MACNeighborsParamType - SIZE_ParamHdrType);

		/* fill in parameter */
		param->MIB_Index = Mhtons (macPtr->Index);
		param->MAC_Index = Mhtons (macPtr->resourceID);
		MCopyAddress (param->UNA, macPtr->UNA);
		MCopyAddress (param->DNA, macPtr->DNA);

		length = SIZE_MACNeighborsParamType;
	}

	return (length);
}

uInt16
#ifdef __STDC__
PathDescriptor_Param (uInt16 stnID, PathDescParamType * param)
#else
PathDescriptor_Param (stnID, param)
		uInt16           stnID;
		PathDescParamType *param;
#endif
/*********************************************************************
Function:	Create the Path Descriptor parameter.
Parameters:	stnID	= index of station to use.
			param	= pointer to location where parameter should start.
Input:		Uses MAC and PORT data and info from MIB.
Output:		param	= contains the SMT Path Descriptor parameter.
Return:		Length in characters of the parameter including
			the type and length fields.
Modification History:
*********************************************************************/
{
TLVParamType     mibData;				/* get request buffer */
PathPORTRecordType *portRecord;			/* parameter PORT entry */
PathMACRecordType *macRecord;			/* parameter MAC entry */
FBMMACInfoType  *macPtr;				/* MAC element */
FBMPORTInfoType *portPtr;				/* PORT element */
PORTMACIndicatedType *remoteMac;		/* remote MAC indicated format */
uInt16           i;						/* loop counter */
XDIConIndexType *conIndex;				/* connected resource format */

	/* fill in header */
	param->Type = Mhtons (PATH_DESC_PARAM_TYPE);
	param->Length = 0;

	/* fill in parameter */
	portRecord = &param->record.port;
	portPtr = &(FBMDATA (stnID, fbmPORTData[0]));
	for (i = 0; i < MAX_PORT_COUNT; i++, portPtr++)
	{
		/* only include physically present entities */
		if (!portPtr->hwPresent)
			continue;

		/* set index */
		portRecord->MIB_Index = Mhtons (portPtr->Index);

		/* get port type */
		mibData.paramType = fddiPORTMy_Type;
		mibData.paramLen = 4;
		mibData.PORTINDEX = i;
		FBMGetMIBAttrMulti (stnID, sizeof (mibData), (Void_p) & mibData);
		portRecord->PORTType = (uChar) mibData.PORTPARAM16;

		/* get connect state */
		mibData.paramType = fddiPORTConnectState;
		mibData.paramLen = 4;
		mibData.PORTINDEX = i;
		FBMGetMIBAttrMulti (stnID, sizeof (mibData), (Void_p) & mibData);
		portRecord->ConnectState = (uChar) mibData.PORTPARAM16;

		/* get remote port type */
		mibData.paramType = fddiPORTNeighborType;
		mibData.paramLen = 4;
		mibData.PORTINDEX = i;
		FBMGetMIBAttrMulti (stnID, sizeof (mibData), (Void_p) & mibData);
		portRecord->RemotePORTType = (uChar) mibData.PORTPARAM16;

		/* get remote MAC */
		mibData.paramType = fddiPORTMACIndicated;
		mibData.paramLen = 4;
		mibData.PORTINDEX = i;
		FBMGetMIBAttrMulti (stnID, sizeof (mibData), (Void_p) & mibData);
		remoteMac = (PORTMACIndicatedType *) & (mibData.PORTPARAM16);
		portRecord->RemoteMAC = (remoteMac->r_val9) ? 1 : 0;

		/* get connected resource */
		mibData.paramType = fddiPORTVendorAttrib;
		mibData.paramLen = 4;
		mibData.PORTINDEX = i;
		mibData.PORTVENDORID = xdiPORTConnectedIndex;
		FBMGetMIBAttrMulti (stnID, sizeof (mibData), (Void_p) & mibData);
		conIndex = (XDIConIndexType *) mibData.PORTVENDORDATA;
		portRecord->ConResourceIndex =
			(conIndex->resourceType == RT_mac)
			? FBMDATA (stnID, fbmMACData[conIndex->xdiIndex].resourceID)
			: FBMDATA (stnID, fbmPORTData[conIndex->xdiIndex].resourceID);
		portRecord->ConResourceIndex = Mhtons (portRecord->ConResourceIndex);

		/* accumulate length */
		param->Length += SIZE_PathPORTRecordType;

		/* move pointer */
		portRecord = MADDPTR (PathPORTRecordType, portRecord,
			SIZE_PathPORTRecordType);
	}

	/* MAC records start immediately following port records */
	macRecord = (PathMACRecordType *) portRecord;
	macPtr = &(FBMDATA (stnID, fbmMACData[0]));
	for (i = 0; i < MAX_MAC_COUNT; i++, macPtr++)
	{
		/* only include physically present entities */
		if (!macPtr->hwPresent)
			continue;

		/* get MAC address */
		mibData.paramType = fddiMACSMTAddress;
		mibData.paramLen = 4;
		mibData.MACINDEX = i;
		FBMGetMIBAttrMulti (stnID, sizeof (mibData), (Void_p) & mibData);
		MCopyAddress (macRecord->MAC_Addr, mibData.MACADDR);

		/* get connected resource */
		mibData.paramType = fddiMACVendorAttrib;
		mibData.paramLen = 4;
		mibData.MACINDEX = i;
		mibData.MACVENDORID = xdiMACConnectedIndex;
		FBMGetMIBAttrMulti (stnID, sizeof (mibData), (Void_p) & mibData);
		conIndex = (XDIConIndexType *) mibData.MACVENDORDATA;
		macRecord->ConResourceIndex =
			(conIndex->resourceType == RT_mac)
			? FBMDATA (stnID, fbmMACData[conIndex->xdiIndex].resourceID)
			: FBMDATA (stnID, fbmPORTData[conIndex->xdiIndex].resourceID);
		macRecord->ConResourceIndex = Mhtons (macRecord->ConResourceIndex);

		/* accumulate length */
		param->Length += SIZE_PathMACRecordType;

		/* move pointer */
		macRecord = MADDPTR (PathMACRecordType, macRecord,
			SIZE_PathMACRecordType);
	}

	/* save unconverted length */
	i = param->Length;

	/* convert length */
	param->Length = Mhtons (param->Length);

	return (i + SIZE_ParamHdrType);
}

uInt16
#ifdef __STDC__
MACStatus_Param (uInt16 stnID, MACStatusParamType * param,
	FBMMACInfoType * macPtr)
#else
MACStatus_Param (stnID, param, macPtr)
		uInt16           stnID;
		MACStatusParamType *param;
		FBMMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Create the MAC Status parameter.
Parameters:	stnID	= index of station to use.
			param	= pointer to location where parameter should start.
			macPtr	= MAC element of MAC to get status.
Input:		Uses MAC data and MIB information.
Output:		param	= contains the SMT MAC Status parameter.
Return:		Length in characters of the parameter including
			the type and length fields.
Modification History:
*********************************************************************/
{
TLVParamType    *mibptr;				/* pointer into group buffer */
uInt16           length;				/* parameter length to return */
uInt16           currentPath;			/* MAC's current path */

	/* only process for physically present MACs */
	length = 0;
	if (macPtr->hwPresent)
	{
		/* fill in header */
		param->Type = Mhtons (MAC_STATUS_PARAM_TYPE);
		param->Length = Mhtons (SIZE_MACStatusParamType - SIZE_ParamHdrType);

		/* fill in parameter */
		param->MIB_Index = Mhtons (macPtr->Index);
		param->MAC_Index = Mhtons (macPtr->resourceID);

		/* get MAC operation group */
		MEMZERO (mibGrp, 4 * SIZE_TLVHdrType);
		mibGrp[0].paramType = fddiMACOperationGrp;
		mibGrp[0].paramLen = 4;
		mibGrp[0].MACINDEX = macPtr->Index;
		FBMMultiGetMIBAttrMulti (stnID, sizeof (mibGrp),
			(Void_p) mibGrp, (TLVSetCountType *) NULL);

		/* T_Req */
		mibptr = (TLVParamType *) FindParam (fddiMACT_Req,
			mibGrp, sizeof (mibGrp), HOST_ORDER);
		if (mibptr)
			param->T_Req = Mhtonl (mibptr->MACPARAM32);

		/* T_Neg */
		mibptr = (TLVParamType *) FindParam (fddiMACT_Neg,
			mibGrp, sizeof (mibGrp), HOST_ORDER);
		if (mibptr)
			param->T_Neg = Mhtonl (mibptr->MACPARAM32);

		/* T_Max */
		mibptr = (TLVParamType *) FindParam (fddiMACT_Max,
			mibGrp, sizeof (mibGrp), HOST_ORDER);
		if (mibptr)
			param->T_Max = Mhtonl (mibptr->MACPARAM32);

		/* TVXValue */
		mibptr = (TLVParamType *) FindParam (fddiMACTvxValue,
			mibGrp, sizeof (mibGrp), HOST_ORDER);
		if (mibptr)
			param->TVXValue = Mhtonl (mibptr->MACPARAM32);

		/*
		 * T-Min was removed from the SMT MIB in rev. 7.0. No specific
		 * replacement value is given except for a reference to the MAC
		 * standard. The MAC standard does not provide any technical
		 * definition of T-Min. A safe value is to use the shortest time
		 * (largest numerical) value: 0xFFFFFFFF.
		 */
		param->T_Min = 0xFFFFFFFF;

		/*
		 * SbaPayload value comes from the path that the MAC is currently on.
		 */
		param->SBA = 0;
		mibGrp[0].paramType = fddiMACCurrentPath;
		mibGrp[0].paramLen = 4;
		mibGrp[0].MACINDEX = macPtr->ID;
		FBMGetMIBAttrMulti (stnID, sizeof (mibGrp), (Void_p) mibGrp);
		currentPath = mibGrp[0].MACPARAM16;

		/* get path index */
		if (currentPath != CP_isolated)
		{
			if (currentPath == CP_local)
			{
				mibGrp[0].paramType = fddiMACVendorAttrib;
				mibGrp[0].paramLen = 8;
				mibGrp[0].MACINDEX = macPtr->ID;
				mibGrp[0].MACVENDORID = xdiMACLocalPath;
				FBMGetMIBAttrMulti (stnID, sizeof (mibGrp), (Void_p) mibGrp);
				currentPath = mibGrp[0].MACVEND16;
			}

			else
				currentPath = (currentPath == CP_secondary)
					? PI_secondary : PI_primary;

			/* adjust path index */
			currentPath--;

			/* get SbaPayload value */
			mibGrp[0].paramType = fddiPATHSbaPayload;
			mibGrp[0].paramLen = 4;
			mibGrp[0].PATHINDEX = currentPath;
			if (FBMGetMIBAttrMulti (stnID, sizeof (mibGrp), (Void_p) mibGrp)
				== RC_Success)
			{
				param->SBA = Mhtonl (mibGrp[0].PATHPARAM32);
			}
		}

		/*
		 * Get frame counters.
		 */
		MEMZERO (mibGrp, 4 * SIZE_TLVHdrType);
		mibGrp[0].paramType = fddiMACCountersGrp;
		mibGrp[0].paramLen = 4;
		mibGrp[0].MACINDEX = macPtr->Index;
		FBMMultiGetMIBAttrMulti (stnID, sizeof (mibGrp),
			(Void_p) mibGrp, (TLVSetCountType *) NULL);

		/* frame count */
		mibptr = (TLVParamType *) FindParam (fddiMACFrame_Ct,
			mibGrp, sizeof (mibGrp), HOST_ORDER);
		if (mibptr)
			param->Frame_Ct = Mhtonl (mibptr->MACPARAM32);

		/* error count */
		mibptr = (TLVParamType *) FindParam (fddiMACError_Ct,
			mibGrp, sizeof (mibGrp), HOST_ORDER);
		if (mibptr)
			param->Error_Ct = Mhtonl (mibptr->MACPARAM32);

		/* lost count */
		mibptr = (TLVParamType *) FindParam (fddiMACLost_Ct,
			mibGrp, sizeof (mibGrp), HOST_ORDER);
		if (mibptr)
			param->Lost_Ct = Mhtonl (mibptr->MACPARAM32);

		length = SIZE_MACStatusParamType;
	}

	return (length);
}

uInt16
#ifdef __STDC__
LERStatus_Param (uInt16 stnID, LERStatusParamType * param,
	FBMPORTInfoType * portPtr)
#else
LERStatus_Param (stnID, param, portPtr)
		uInt16           stnID;
		LERStatusParamType *param;
		FBMPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Create the PORT LER Status parameter.
Parameters:	stnID	= index of station to use.
			param	= pointer to location where parameter should start.
			portPtr	= FBM index of PORT to get LER status.
Input:		Uses PORT data and MIB information.
Output:		param	= contains the SMT LER Status parameter.
Return:		Length in characters of the parameter including
			the type and length fields.
Modification History:
*********************************************************************/
{
TLVParamType    *mibptr;				/* pointer into group buffer */
uInt16           length;				/* return value */

	/* only process of physically present PORTs */
	length = 0;
	if (portPtr->hwPresent)
	{
		/* fill in header */
		param->Type = Mhtons (LER_STATUS_PARAM_TYPE);
		param->Length = Mhtons (SIZE_LERStatusParamType - SIZE_ParamHdrType);

		/* fill in parameter */
		param->MIB_Index = Mhtons (portPtr->Index);
		param->PORT_Index = Mhtons (portPtr->resourceID);

		/* get LER group */
		MEMZERO (mibGrp, 4 * SIZE_TLVHdrType);
		mibGrp[0].paramType = fddiPORTLerGrp;
		mibGrp[0].paramLen = 4;
		mibGrp[0].PORTINDEX = portPtr->Index;
		FBMMultiGetMIBAttrMulti (stnID, sizeof (mibGrp),
			(Void_p) mibGrp, (TLVSetCountType *) NULL);

		/* get LER cutoff */
		mibptr = (TLVParamType *) FindParam (fddiPORTLer_Cutoff,
			mibGrp, sizeof (mibGrp), HOST_ORDER);
		if (mibptr)
			param->Ler_Cutoff = mibptr->PORTPARAM8;

		/* get LER alarm */
		mibptr = (TLVParamType *) FindParam (fddiPORTLer_Alarm,
			mibGrp, sizeof (mibGrp), HOST_ORDER);
		if (mibptr)
			param->Ler_Alarm = mibptr->PORTPARAM8;

		/* get LER estimate */
		mibptr = (TLVParamType *) FindParam (fddiPORTLer_Estimate,
			mibGrp, sizeof (mibGrp), HOST_ORDER);
		if (mibptr)
			param->Ler_Estimate = mibptr->PORTPARAM8;

		/* get LEM reject count */
		mibptr = (TLVParamType *) FindParam (fddiPORTLem_Reject_Ct,
			mibGrp, sizeof (mibGrp), HOST_ORDER);
		if (mibptr)
			param->Lem_Reject_Ct = Mhtonl (mibptr->PORTPARAM32);

		/* get LEM count */
		mibptr = (TLVParamType *) FindParam (fddiPORTLem_Ct,
			mibGrp, sizeof (mibGrp), HOST_ORDER);
		if (mibptr)
			param->Lem_Ct = Mhtonl (mibptr->PORTPARAM32);

		length = SIZE_LERStatusParamType;
	}

	return (length);
}

uInt16
#ifdef __STDC__
FrameCounters_Param (uInt16 stnID, FrameCountersParamType * param,
	FBMMACInfoType * macPtr)
#else
FrameCounters_Param (stnID, param, macPtr)
		uInt16           stnID;
		FrameCountersParamType *param;
		FBMMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Create the MAC Frame Counters parameter.
Parameters:	stnID	= index of station to use.
			param	= pointer to location where parameter should start.
			macPtr	= MAC element of MAC to get counts.
Input:		Uses MAC data and MIB information.
Output:		param	= contains the SMT Frame Counters parameter.
Return:		Length in characters of the parameter including
			the type and length fields.
Modification History:
*********************************************************************/
{
TLVParamType    *mibptr;				/* pointer into group buffer */
uInt16           length;				/* return value */

	/* only process for physically present MACs */
	length = 0;
	if (macPtr->hwPresent)
	{
		/* fill in header */
		param->Type = Mhtons (FRAME_COUNTERS_PARAM_TYPE);
		param->Length = Mhtons (SIZE_FrameCountersParamType
			- SIZE_ParamHdrType);

		/* fill in parameter */
		param->MIB_Index = Mhtons (macPtr->Index);
		param->MAC_Index = Mhtons (macPtr->resourceID);

		/* get MAC counters group */
		MEMZERO (mibGrp, 4 * SIZE_TLVHdrType);
		mibGrp[0].paramType = fddiMACCountersGrp;
		mibGrp[0].paramLen = 4;
		mibGrp[0].MACINDEX = macPtr->Index;
		FBMMultiGetMIBAttrMulti (stnID, sizeof (mibGrp),
			(Void_p) mibGrp, (TLVSetCountType *) NULL);

		/* receive count */
		mibptr = (TLVParamType *) FindParam (fddiMACCopied_Ct,
			mibGrp, sizeof (mibGrp), HOST_ORDER);
		if (mibptr)
			param->Copied_Ct = Mhtonl (mibptr->MACPARAM32);

		/* transmit count */
		mibptr = (TLVParamType *) FindParam (fddiMACTransmit_Ct,
			mibGrp, sizeof (mibGrp), HOST_ORDER);
		if (mibptr)
			param->Transmit_Ct = Mhtonl (mibptr->MACPARAM32);

		length = SIZE_FrameCountersParamType;
	}

	return (length);
}

uInt16
#ifdef __STDC__
FrameNotCopied_Param (uInt16 stnID, FrameNotCopiedParamType * param,
	FBMMACInfoType * macPtr)
#else
FrameNotCopied_Param (stnID, param, macPtr)
		uInt16           stnID;
		FrameNotCopiedParamType *param;
		FBMMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Create the MAC Frame Not Copied Count parameter.
Parameters:	stnID	= index of station to use.
			param	= pointer to location where parameter should start.
			macPtr	= MAC element of MAC to get not copied count.
Input:		Uses MAC data and MIB information.
Output:		param	= contains the SMT Frame Not Copied parameter.
Return:		Length in characters of the parameter including
			the type and length fields.
Modification History:
*********************************************************************/
{
TLVParamType     mibData;				/* get request buffer */
uInt16           length;				/* return value */

	/* only process for physically present MACs */
	length = 0;
	if (macPtr->hwPresent)
	{
		mibData.paramType = fddiMACNotCopied_Ct;
		mibData.paramLen = 4;
		mibData.MACINDEX = macPtr->ID;
		if (FBMGetMIBAttrMulti (stnID, sizeof (mibData), (Void_p) & mibData)
			== RC_Success)
		{
			/* fill in header */
			param->Type = Mhtons (FRAME_NOT_COPIED_PARAM_TYPE);
			param->Length = Mhtons (SIZE_FrameNotCopiedParamType
				- SIZE_ParamHdrType);

			/* fill in parameter */
			param->MIB_Index = Mhtons (macPtr->Index);
			param->MAC_Index = Mhtons (macPtr->resourceID);
			param->NotCopied_Ct = Mhtonl (mibData.MACPARAM32);

			length = SIZE_FrameNotCopiedParamType;
		}
	}

	return (length);
}

uInt16
#ifdef __STDC__
PriorityValues_Param (uInt16 stnID, PriorityValuesParamType * param,
	FBMMACInfoType * macPtr)
#else
PriorityValues_Param (stnID, param, macPtr)
		uInt16           stnID;
		PriorityValuesParamType *param;
		FBMMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Create the MAC Priority Values parameter.
Parameters:	stnID	= index of station to use.
			param	= pointer to location where parameter should start.
			macPtr	= MAC element of MAC to get priority values.
Input:		Uses MAC data and MIB information.
Output:		param	= contains the SMT Priority Values parameter.
Return:		Length in characters of the parameter including
			the type and length fields.
Modification History:
*********************************************************************/
{
TLVParamType    *mibptr;				/* pointer into group buffer */
uInt16           length;				/* return value */

	/* only process for physically present MACs */
	length = 0;
	if (macPtr->hwPresent)
	{
		/* fill in header */
		param->Type = Mhtons (PRIORITY_VALUES_PARAM_TYPE);
		param->Length = Mhtons (SIZE_PriorityValuesParamType
			- SIZE_ParamHdrType);

		/* fill in parameter */
		param->MIB_Index = Mhtons (macPtr->Index);
		param->MAC_Index = Mhtons (macPtr->resourceID);

		/* get Operation group */
		MEMZERO (mibGrp, 4 * SIZE_TLVHdrType);
		mibGrp[0].paramType = fddiMACOperationGrp;
		mibGrp[0].paramLen = 4;
		mibGrp[0].MACINDEX = macPtr->Index;
		FBMMultiGetMIBAttrMulti (stnID, sizeof (mibGrp),
			(Void_p) mibGrp, (TLVSetCountType *) NULL);

		mibptr = (TLVParamType *) FindParam (fddiMACT_Pri0,
			mibGrp, sizeof (mibGrp), HOST_ORDER);
		if (mibptr)
			param->T_Pri0 = Mhtonl (mibptr->MACPARAM32);

		mibptr = (TLVParamType *) FindParam (fddiMACT_Pri1,
			mibGrp, sizeof (mibGrp), HOST_ORDER);
		if (mibptr)
			param->T_Pri1 = Mhtonl (mibptr->MACPARAM32);

		mibptr = (TLVParamType *) FindParam (fddiMACT_Pri2,
			mibGrp, sizeof (mibGrp), HOST_ORDER);
		if (mibptr)
			param->T_Pri2 = Mhtonl (mibptr->MACPARAM32);

		mibptr = (TLVParamType *) FindParam (fddiMACT_Pri3,
			mibGrp, sizeof (mibGrp), HOST_ORDER);
		if (mibptr)
			param->T_Pri3 = Mhtonl (mibptr->MACPARAM32);

		mibptr = (TLVParamType *) FindParam (fddiMACT_Pri4,
			mibGrp, sizeof (mibGrp), HOST_ORDER);
		if (mibptr)
			param->T_Pri4 = Mhtonl (mibptr->MACPARAM32);

		mibptr = (TLVParamType *) FindParam (fddiMACT_Pri5,
			mibGrp, sizeof (mibGrp), HOST_ORDER);
		if (mibptr)
			param->T_Pri5 = Mhtonl (mibptr->MACPARAM32);

		mibptr = (TLVParamType *) FindParam (fddiMACT_Pri6,
			mibGrp, sizeof (mibGrp), HOST_ORDER);
		if (mibptr)
			param->T_Pri6 = Mhtonl (mibptr->MACPARAM32);

		length = SIZE_PriorityValuesParamType;
	}

	return (length);
}

uInt16
#ifdef __STDC__
EBStatus_Param (uInt16 stnID, EBStatusParamType * param,
	FBMPORTInfoType * portPtr)
#else
EBStatus_Param (stnID, param, portPtr)
		uInt16           stnID;
		EBStatusParamType *param;
		FBMPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Create the PORT EB Status parameter.
Parameters:	stnID	= index of station to use.
			param	= pointer to location where parameter should start.
			portPtr	= PORT element of PORT to get EB status.
Input:		Uses PORT data and MIB information.
Output:		param	= contains the SMT EB Status parameter.
Return:		Length in characters of the parameter including
			the type and length fields.
Modification History:
*********************************************************************/
{
TLVParamType     mibData;				/* get request buffer */
uInt16           length;				/* parameter length returned */

	/* only process for physically present PORTs */
	length = 0;
	if (portPtr->hwPresent)
	{
		/*
		 * Get MIB value. If not supported, then skip this parameter.
		 */
		mibData.paramType = fddiPORTEBError_Ct;
		mibData.paramLen = 4;
		mibData.PORTINDEX = portPtr->ID;
		if (FBMGetMIBAttrMulti (stnID, sizeof (mibData), (Void_p) & mibData)
			== RC_Success)
		{
			/* fill in header */
			param->Type = Mhtons (EB_STATUS_PARAM_TYPE);
			param->Length = Mhtons (SIZE_EBStatusParamType
				- SIZE_ParamHdrType);

			/* fill in parameter */
			param->MIB_Index = Mhtons (portPtr->Index);
			param->PORT_Index = Mhtons (portPtr->resourceID);
			param->EbErrorCt = Mhtonl (mibData.PORTPARAM32);

			length = SIZE_EBStatusParamType;
		}
	}

	return (length);
}

uInt16
#ifdef __STDC__
Manufacturer_Param (uInt16 stnID, ManufacturerParamType * param)
#else
Manufacturer_Param (stnID, param)
		uInt16           stnID;
		ManufacturerParamType *param;
#endif
/*********************************************************************
Function:	Create the Manufacturer Field parameter.
Parameters:	stnID	= index of station to use.
			param	= pointer to location where parameter should start.
Input:		Uses MIB information.
Output:		param	= contains the SMT Manufacturer parameter.
Return:		Length in characters of the parameter including
			the type and length fields.
Modification History:
*********************************************************************/
{
TLVParamType     mibData;				/* get request buffer */
uInt16           length;				/* param length returned */

	/* clear length */
	length = 0;

	/*
	 * If attribute is not supported by the MIB, then skip this parameter.
	 */
	mibData.paramType = fddiSMTManufacturerData;
	mibData.paramLen = 0;
	if (FBMGetMIBAttrMulti (stnID, sizeof (mibData), (Void_p) & mibData)
		== RC_Success)
	{
		/* fill in header */
		param->Type = Mhtons (MANUFACTURER_PARAM_TYPE);
		param->Length = Mhtons (SIZE_ManufacturerParamType
			- SIZE_ParamHdrType);

		/* fill in parameter */
		MEMCOPY (param->manuf_OUI, mibData.SMTOTHER,
			SIZE_SMTManufacturerDataType);

		length = SIZE_ManufacturerParamType;
	}

	return (length);
}

uInt16
#ifdef __STDC__
User_Param (uInt16 stnID, UserParamType * param)
#else
User_Param (stnID, param)
		uInt16           stnID;
		UserParamType   *param;
#endif
/*********************************************************************
Function:	Create the User Field parameter.
Parameters:	stnID	= index of station to use.
			param	= pointer to location where parameter should start.
Input:		Uses MIB information.
Output:		param	= contains the SMT User parameter.
Return:		Length in characters of the parameter including
			the type and length fields.
Modification History:
*********************************************************************/
{
TLVParamType     mibData;				/* buffer for data */

	mibData.paramType = fddiSMTUserData;
	mibData.paramLen = 0;
	FBMGetMIBAttrMulti (stnID, sizeof (mibData), (Void_p) & mibData);

	/* fill in header */
	param->Type = Mhtons (USER_PARAM_TYPE);
	param->Length = Mhtons (SIZE_UserParamType - SIZE_ParamHdrType);

	/* fill in parameter */
	MEMCOPY (param->user_data, mibData.SMTOTHER, SIZE_SMTUserDataType);

	return (SIZE_UserParamType);
}

uInt16
#ifdef __STDC__
Echo_Param (EchoParamType * param, Void_p eBuffer, uInt16 eSize)
#else
Echo_Param (param, eBuffer, eSize)
		EchoParamType   *param;
		Void_p           eBuffer;
		uInt16           eSize;
#endif
/*********************************************************************
Function:	Create the Echo Data parameter.
Parameters:	param	= pointer to location where parameter should start.
			eBuffer	= pointer to echo data.
			eSize	= length of echo data.
Input:		eBuffer	= contains echo data to use. This data must be
			in network byte order.
Output:		param	= contains the SMT Echo parameter.
Return:		Length in characters of the parameter including
			the type and length fields.
Modification History:
*********************************************************************/
{
	/* fill in header */
	param->Type = Mhtons (ECHO_PARAM_TYPE);
	param->Length = Mhtons (eSize);

	/* fill in parameter */
	MEMCOPY (param->echo_data, eBuffer, eSize);

	return (eSize + SIZE_ParamHdrType);
}

uInt16
#ifdef __STDC__
ReasonCode_Param (ReasonCodeParamType * param, uInt32 code)
#else
ReasonCode_Param (param, code)
		ReasonCodeParamType *param;
		uInt32           code;
#endif
/*********************************************************************
Function:	Create the Reason Code parameter.
Parameters:	param	= pointer to location where parameter should start.
			code	= reason code to use.
Input:		None.
Output:		param	= contains the SMT Reason Code parameter.
Return:		Length in characters of the parameter including
			the type and length fields.
Modification History:
*********************************************************************/
{
	/* fill in header */
	param->Type = Mhtons (REASON_CODE_PARAM_TYPE);
	param->Length = Mhtons (SIZE_ReasonCodeParamType - SIZE_ParamHdrType);

	/* fill in parameter */
	param->reason = Mhtonl (code);

	return (SIZE_ReasonCodeParamType);
}

uInt16
#ifdef __STDC__
RejectedFrame_Param (RejectedFrameParamType * param,
	SMTFrameHdrType * frame, uInt16 maxLen, uInt16 reqFrameLen)
#else
RejectedFrame_Param (param, frame, maxLen, reqFrameLen)
		RejectedFrameParamType *param;
		SMTFrameHdrType *frame;
		uInt16           maxLen;
		uInt16           reqFrameLen;
#endif
/*********************************************************************
Function:	Create the Rejected Frame Beginning parameter.
Parameters:	param		= pointer to location where parameter should start.
			frame		= pointer to rejected frame.
			maxLen		= maximum size of this parameter (i.e., # of
							bytes left in frame currently being built).
			reqFrameLen	= number of bytes actually received for this frame.
Input:		frame		= contains rejected frame starting with the pad
							bytes before the FC field through the complete
							SMT info field.
Output:		param		= contains the SMT Rejected Frame parameter.
Return:		Length in characters of the parameter including
			the type and length fields.
Modification History:
*********************************************************************/
{
uInt16           frameLen;				/* length of data placed in param */

	/* get length of rejected frame */
	frameLen = Mntohs (frame->smtHdr.InfoField_Length)
		+ SIZE_SMTFrameHdrType - FC_PAD_SIZE;

	/*
	 * If length of InfoField exceeds the number of received bytes, then use
	 * only what was received.
	 */
	if (frameLen > reqFrameLen)
		frameLen = reqFrameLen;

	/*
	 * Reduce amount of space for rejected frame by the size of the parameter
	 * header and pad field.
	 */
	maxLen -= (SIZE_ParamHdrType + REJECTED_PAD_SIZE);

	/* set data size */
	if (frameLen > maxLen)
		frameLen = maxLen;

	/* fill in parameter */
	MEMCOPY (param->rejected_frame_beginning, &frame->macHdr.FC, frameLen);

	/* add padding to parameter length */
	frameLen += REJECTED_PAD_SIZE;

	/* fill in header */
	param->Type = Mhtons (REJECTED_FRAME_PARAM_TYPE);
	param->Length = Mhtons (frameLen);

	return (frameLen + SIZE_ParamHdrType);
}

uInt16
#ifdef __STDC__
SupportedVersions_Param (uInt16 stnID, SupportedVersionsParamType * param)
#else
SupportedVersions_Param (stnID, param)
		uInt16           stnID;
		SupportedVersionsParamType *param;
#endif
/*********************************************************************
Function:	Create the SMT Supported Versions parameter.
Parameters:	stnID	= index of station to use.
			param	= pointer to location where parameter should start.
Input:		Uses station data.
Output:		param	= contains the SMT Supported Versions parameter.
Return:		Length in characters of the parameter including
			the type and length fields.
Modification History:
*********************************************************************/
{
uInt16           count;					/* # of version supported */
uInt16          *versionPtr;			/* version entry in parameter */
FBMStationInfoType *stationPtr;			/* station data */

	/* cast pointer */
	stationPtr = &FBMDATA (stnID, fbmStationData);

	/* get number of versions */
	count = stationPtr->hiVersionID - stationPtr->loVersionID + 1;
	param->numVersions = count;

	/* fill in header */
	param->Type = Mhtons (SUPPORTED_VERSIONS_PARAM_TYPE);

	/* length must end on 4-byte boundary */
	if (count & 1)
		count++;

	/*
	 * length is 2 bytes per version plus pad, numVersions and opVersion
	 */
	param->Length = Mhtons ((count << 1) + 4);

	/* fill in parameter */
	for (versionPtr = param->Version, count = stationPtr->loVersionID;
		count <= stationPtr->hiVersionID;
		count++, versionPtr++)
	{
		*versionPtr = Mhtons (count);
		if (count == stationPtr->opVersionID)
			param->indexOpVersion = count - stationPtr->loVersionID + 1;
	}

	/* if odd number of versions, pad last one to 0s */
	if (param->numVersions & 1)
		*versionPtr = 0;

	return (Mntohs (param->Length) + SIZE_ParamHdrType);
}

uInt16
#ifdef __STDC__
Authorization_ParamMulti (uInt16 stnID, AuthorizationParamType * param,
	SMTFrameHdrType * frameHdr, uInt16 spaceLeft, uInt16 outputMAC)
#else
Authorization_ParamMulti (stnID, param, frameHdr, spaceLeft, outputMAC)
		uInt16           stnID;
		AuthorizationParamType *param;
		SMTFrameHdrType *frameHdr;
		uInt16           spaceLeft;
		uInt16           outputMAC;
#endif
/*********************************************************************
Function:	Create the Authorization parameter.
Parameters:	stnID		= index of station to use.
			param		= pointer to location where parameter should
							start.
			frameHdr	= pointer to start of frame.
			spaceLeft	= amount of space left in SMT InfoField.
			outputMAC	= index of MAC sending frame.
Input:		None.
Output:		param		= contains the SMT Authorization parameter.
Return:		Length in characters of the parameter including
			the type and length fields.
Modification History:
*********************************************************************/
{
uInt16           aLen;					/* length of authorization field */
uInt16           extra;					/* flag if padding is needed */
uInt16           length;				/* length of parameter returned */

	/* check if authorization is used */
	length = 0;
	if (FBMDATA (stnID, fbmStationData.useAuthorization))
	{
		/* fill in parameter */
		aLen = BuildAuthorizationMulti (stnID, param->Authorization,
			frameHdr, spaceLeft - SIZE_TLVHdrType, outputMAC);

		/* adjust length if not multiple of 4 */
		extra = aLen & 0x3;
		if (extra)
			aLen += (4 - extra);

		/* verify length */
		if (aLen > spaceLeft)
			/* invalid length */
			length = 0;

		else
		{
			/* fill in header */
			param->Type = Mhtons (AUTHORIZATION_PARAM_TYPE);
			param->Length = Mhtons (aLen);

			length = aLen + SIZE_ParamHdrType;
		}
	}

	return (length);
}

uInt16
#ifdef __STDC__
Attribute_Param (TLVParamType * param, TLVParamType * attr)
#else
Attribute_Param (param, attr)
		TLVParamType    *param;
		TLVParamType    *attr;
#endif
/*********************************************************************
Function:	Convert a MIB attribute TLV encoding into network order.
Parameters:	param	= pointer to location where parameter
						should start.
			attr	= pointer to attribute.
Input:		attr	= contains parameter in TLV MIB encoding.
Output:		param	= contains the SMT frame parameter.
Return:		Length in characters of the parameter including
			the type and length fields.
Notes:		If the buffer and the attribute have the same address,
			then the routine does not copy the attribute to the
			buffer and converts the attribute in situ.
Modification History:
*********************************************************************/
{
uInt16           len;					/* param length returned */

	/* get length */
	len = attr->paramLen + SIZE_TLVHdrType;

	/* copy if buffer is different location than attribute */
	if (param != attr)
		MEMCOPY (param, attr, len);

	/* convert attribute to network order */
	AttrNetOrder (param, NET_ORDER);

	/* return size of parameter */
	return (len);
}


/*
*	If the constant MULTIPLE_STATION is not defined, then the system
*	is being compiled for single station use. In this case, the following
*	routine(s) need to be defined for the older single station calling
*	sequence.
*/
#ifndef MULTIPLE_STATION

uInt16
#ifdef __STDC__
Authorization_Param (AuthorizationParamType * param,
	SMTFrameHdrType * frameHdr, uInt16 spaceLeft, uInt16 outputMAC)
#else
Authorization_Param (param, frameHdr, spaceLeft, outputMAC)
		AuthorizationParamType *param;
		SMTFrameHdrType *frameHdr;
		uInt16           spaceLeft;
		uInt16           outputMAC;
#endif
{
	return (Authorization_ParamMulti (FIRST_STATION, param, frameHdr,
			spaceLeft, outputMAC));
}

#endif							/* MULTIPLE_STATION */
