/* $Id: mibmasic.c,v 3.2 1995/11/17 18:10:45 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/mibmasic.c,v $
 *------------------------------------------------------------------
 *
 *
 * Mar 1994, Walter Friedrich
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: mibmasic.c,v $
 * Revision 3.2  1995/11/17  18:10:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:56:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:25:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_mibmasic.c xm7_v3.1.1:cset.0166:4:4 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/**********************************************************************
		Management Information Base Module

		MIB MASIC Processing.

		File:			xm7_mibmasic.c
		Created:		10/29/93

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94

		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		OPTIONAL MODULE

		This module is responsible for handling the Management
		Information Base (MIB) for the MASIC object. The module
		includes initialization support routines, as well as get and
		change operation routines. As with the FDDI MIB objects, the
		get and change operations call interface routines to retrieve
		or instantiate values.

		With release 7.3, this file contains the following conditional
		compilation options:

			XM_USE_FLEX_CONF	include code for flexible configuration.

		If any of the constants listed above are defined, then the
		associated portion of code is included in the compilation.

		The constants XM_USE_VSO and XM_USE_VSO_MASIC control conditional
		compilation within the other core files. If and only if BOTH
		constants are defined during compilation will the MASIC object
		be available. Otherwise, this file does not have to linked into
		the system. This allows systems without MASIC capabilities to
		remove unused code.

		The constant XM_USE_FLEX_CONF controls conditional compilation
		over routines to handle flexible configuration. If this constant
		is NOT defined, then the associated routines are NOT included in
		the code and the system will not support flexible configurations.

		The flexible configuration routines affected are those that remove
		and delete entities from the MIB and those that notify the MAP of
		any configuration change. The addition and insertion routines
		remain in the code as these are needed to build the MIB during
		initialization.

		Modification History:
		*** SMT 7.3 ***
		Added in Release 7.3.1.
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
*	MASIC Group Definitions
*
*********************************************************************/

MCONST
static           uInt16
                 MASICConfigurationGrpList[] =
{
	calmMASICVersion, calmMASICCapabilities,
	calmMASICStartResource, calmMASICEndResource,
	calmMASICUpstreamResource, calmMASICDownstreamResource,
	calmMASICSliceLatency, calmMASICIndex,
	calmMASICHardwarePresent,
	0},

                 MASICOperationsGrpList[] =
{
	calmMASICCurrentPath, calmMASICRingState,
	calmMASICATHT, calmMASICBeaconInit_Ct,
	calmMASICClaimInit_Ct, calmMASICTokenLost_Ct,
	calmMASICPUNA,
	0};


/*********************************************************************
*
*	MASIC Initialization Routines
*
*********************************************************************/

uInt32
#ifdef __STDC__
InitMASICObject (uInt16 stnID)
#else
InitMASICObject (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	This routine initializes the database for the MASIC object
			by clearing the index lookup tables and inUse flags.
Parameters:	stnID	= index of station to initialize.
Input:		mib		= MIB database for this station.
Output:		mib		= MASIC data within MIB is set to starting state.
Return:		0		if successful.
Modification History:
*********************************************************************/
{
Int16            i;						/* loop counter */
MIBType         *mibPtr;				/* MIB to use */

	/* get MIB */
	mibPtr = MIBDATA (stnID, mib);

	/* clear MASIC tables */
	for (i = 0; i < MAX_MASIC_COUNT; i++)
	{
		mibPtr->masicTable[i].resourceID = 0;
		mibPtr->masicTable[i].index = 0;
		mibPtr->masicBase[i].inUse = FALSE;
	}

	return (0);
}


/*********************************************************************
*
*	MASIC Maintenance Routines
*
*********************************************************************/

uInt32
#ifdef __STDC__
AddMASICObjectMulti (uInt16 stnID, uInt16 rsrcID, MASICDataPtr * masicPtr)
#else
AddMASICObjectMulti (stnID, rsrcID, masicPtr)
		uInt16           stnID;
		uInt16           rsrcID;
		MASICDataPtr    *masicPtr;
#endif
/*********************************************************************
Function:	This routine is called by the implementer at any time a
			new MASIC instance is to be added to the resource list. This
			occurs during initialization processing and when new
			devices are added. The routine places the MASIC's MIB
			information into the MIB.
Parameters:	stnID		= index of station to use.
			rsrcID		= the value for this MASIC's xdiMASICIndex attribute.
			masicPtr	= returned address to MIB entry for MASIC.
Input:		masicPtr	= pointer to hold address of MIB entry for MASIC.
Output:		Adds resource ID into MASIC resource table and initializes
			base attributes.
			masicPtr	= pointer to entry in MIB for the MASIC.
				Implementer must fill in user-specified values
				(see manual and BuildMASICMIBMulti()).
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
MASICDataPtr     base;					/* MASIC base element */
uInt32           result = 0;			/* return value */

	/* get MIB */
	mibPtr = MIBDATA (stnID, mib);
	*masicPtr = (MASICDataPtr) NULL;

	/* check for room in table */
	if (mibPtr->smtData.xdi.Masic_Ct >= MAX_MASIC_COUNT)
		result = EMIB_TABLE_FULL;

	/* check for valid ID */
	else if (rsrcID == 0)
		result = EMIB_INVALID_ID;

	else
	{
		/* check if resource ID is in the table */
		index = MFindResourceID (rsrcID, mibPtr->masicTable,
			mibPtr->smtData.xdi.Masic_Ct);
		if (index >= 0)
			/* index found so ID is in table */
			result = EMIB_DUP_ID;

		else
		{
			/* add ID to table */
			index = MAddResourceID (rsrcID, mibPtr->masicTable,
				mibPtr->smtData.xdi.Masic_Ct);

			/* locate open space in data list */
			for (i = 0, base = &mibPtr->masicBase[0];
				i < MAX_MASIC_COUNT;
				i++, base++)
			{
				if (!base->inUse)
				{
					base->inUse = TRUE;
					mibPtr->masicTable[index].index = i;
					break;
				}
			}

			/* set index to data table entry */
			index = i;

			/* increment MASIC count */
			mibPtr->smtData.xdi.Masic_Ct++;

			/* initialize base attributes */
			base->baseData.HardwarePresent = NOTPRESENT;
			base->baseData.Index = rsrcID;
			base->baseData.xdiIndex = index;

			/* optional attributes */

			/* force vendor specific attributes for XDI */

			/* set packages */

			*masicPtr = base;

			/* reset TimeStamp and SetCount */
			MarkDiscontinuity (&mibPtr->smtData);
		}
	}

	return (result);
}

uInt32
#ifdef __STDC__
InsertMASICObjectMulti (uInt16 stnID, uInt16 rsrcID, MASICDataPtr * masicPtr)
#else
InsertMASICObjectMulti (stnID, rsrcID, masicPtr)
		uInt16           stnID;
		uInt16           rsrcID;
		MASICDataPtr    *masicPtr;
#endif
/*********************************************************************
Function:	This routine is called by the implementer at any time a
			MASIC instance is physically available to the station. This
			occurs after adding a new resource or when a resource is
			re-inserted after it had been removed.
Parameters:	stnID		= index of station to use
			rsrcID		= the value for this MASIC's xdiMASICIndex attribute
			masicPtr	= address of the memory space for the MASIC's
							MIB table entry.
Input:		None.
Output:		Set HardwarePresent to PRESENT for this resource.
			masicPtr	= address of MIB table entry.
Return:		0					if successful.
			EMIB_ID_NOT_FOUND	Resource ID is not in MIB.
Modification History:
*********************************************************************/
{
Int16            index;					/* array index */
MIBType         *mibPtr;				/* MIB data base */
MASICBaseAttrType *base;				/* base attributes */
MASICHWAttrType *hw;
uInt32           result = 0;			/* return value */

	/* get MIB */
	mibPtr = MIBDATA (stnID, mib);

	/* check if resource ID is in the table */
	index = MFindResourceID (rsrcID, mibPtr->masicTable,
		mibPtr->smtData.xdi.Masic_Ct);
	if (index < 0)
		/* index not found */
		result = EMIB_ID_NOT_FOUND;

	else
	{
		/* map index to MASIC attributes */
		index = mibPtr->masicTable[index].index;

		/* initialize values */
		base = &mibPtr->masicBase[index].baseData;
		hw = &mibPtr->masicBase[index].hwData;
		base->HardwarePresent = PRESENT;

		/* set return pointer value */
		*masicPtr = &mibPtr->masicBase[index];
	}

	return (result);
}

/*
 * Only include the routines to remove and delete entities from the MIB
 * if the XM_USE_FLEX_CONF constant is defined. The routines affected
 * are: RemoveMASICObjectMulti() and DeleteMASICObjectMulti().
 */
#ifdef XM_USE_FLEX_CONF

uInt32
#ifdef __STDC__
RemoveMASICObjectMulti (uInt16 stnID, uInt16 rsrcID)
#else
RemoveMASICObjectMulti (stnID, rsrcID)
		uInt16           stnID;
		uInt16           rsrcID;
#endif
/*********************************************************************
Function:	This routine is called by the implementer at any time a
			MASIC instance is physically unavailable to the station, but
			will remain manageable.
Parameters:	stnID	= index of station to use
			rsrcID	= the value for this MASIC's xdiMASICIndex attribute
Input:		None.
Output:		Set HardwarePresent to NOTPRESENT for this resource.
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
	index = MFindResourceID (rsrcID, mibPtr->masicTable,
		mibPtr->smtData.xdi.Masic_Ct);
	if (index < 0)
		/* index not found */
		result = EMIB_ID_NOT_FOUND;

	else
	{
		/* map index to MASIC attributes */
		index = mibPtr->masicTable[index].index;

		/* set HW not present */
		mibPtr->masicBase[index].baseData.HardwarePresent = NOTPRESENT;
	}

	return (result);
}

uInt32
#ifdef __STDC__
DeleteMASICObjectMulti (uInt16 stnID, uInt16 rsrcID)
#else
DeleteMASICObjectMulti (stnID, rsrcID)
		uInt16           stnID;
		uInt16           rsrcID;
#endif
/*********************************************************************
Function:	This routine is called by the implementer at any time a
			MASIC instance is to be removed from the MIB.
Parameters:	stnID	= index of station to use
			rsrcID	= the value for this MASIC's xdiMASICIndex attribute
Input:		None.
Output:		MASIC table has resource ID removed.
Return:		0					if successful.
			EMIB_ID_NOT_FOUND	Resource ID is not in MIB.
Modification History:
*********************************************************************/
{
Int16            index;					/* array index */
MIBType         *mibPtr;				/* MIB data base */
MASICDataPtr     masicPtr;				/* MASIC entry */
uInt32           result = 0;			/* return value */

	/* get MIB */
	mibPtr = MIBDATA (stnID, mib);

	/* check if resource ID is in the table */
	index = MFindResourceID (rsrcID, mibPtr->masicTable,
		mibPtr->smtData.xdi.Masic_Ct);
	if (index < 0)
		/* index not found */
		result = EMIB_ID_NOT_FOUND;

	else
	{
		/* map index to MASIC attributes */
		index = mibPtr->masicTable[index].index;
		masicPtr = &mibPtr->masicBase[index];

		/* remove resource from table */
		MDeleteResourceID (rsrcID, mibPtr->masicTable,
			mibPtr->smtData.xdi.Masic_Ct);
		mibPtr->smtData.xdi.Masic_Ct--;
		masicPtr->inUse = FALSE;

		/* reset TimeStamp and SetCount */
		MarkDiscontinuity (&mibPtr->smtData);
	}

	return (result);
}

void
#ifdef __STDC__
NotifyAddMASICMulti (uInt16 stnID, MASICDataPtr masicPtr)
#else
NotifyAddMASICMulti (stnID, masicPtr)
		uInt16           stnID;
		MASICDataPtr     masicPtr;
#endif
/*********************************************************************
Function:	This routine is called by the implementer to cause
			notification messages to be sent to the MAP when
			a MASIC instance is added to the MIB. This routine is usually
			called after calling AddMASICObjectMulti().
Parameters:	stnID		= index of station to use
			masicPtr	= pointer to the MASIC element in the MIB database.
Input:		masicPtr	= element contains current instance data.
Output:		Messages sent to CSP and MSP.
Return:		None.
Modification History:
*********************************************************************/
{
SMTMessage       msg;					/* notification message */

	/* set up message */
	msg.source = MIB_MSG_ID;
	msg.type = MIB_EVENT_ADD_MASIC;
	msg.typeInfo = 0;
	msg.localID = 0;
	msg.entity = masicPtr->baseData.xdiIndex;
	msg.stnID = stnID;
	msg.data.b16 = masicPtr->baseData.Index;
	msg.len1 = 0;
	msg.len2 = 0;

	/* send message to MAP */
	msg.destination = MAP_MSG_ID;
	SendMIBMessage (&msg);

	return;
}

void
#ifdef __STDC__
NotifyInsertMASICMulti (uInt16 stnID, MASICDataPtr masicPtr)
#else
NotifyInsertMASICMulti (stnID, masicPtr)
		uInt16           stnID;
		MASICDataPtr     masicPtr;
#endif
/*********************************************************************
Function:	This routine is called by the implementer to cause
			notification messages to be sent to the MAP when
			a MASIC instance is inserted to the MIB. This routine is usually
			called after calling InsertMASICObjectMulti().
Parameters:	stnID		= index of station to use
			masicPtr	= pointer to the MASIC element in the MIB database.
Input:		masicPtr	= element contains current instance data.
Output:		Messages sent to CSP and MSP.
Return:		None.
Modification History:
*********************************************************************/
{
SMTMessage       msg;					/* notification message */

	/* set up message */
	msg.source = MIB_MSG_ID;
	msg.type = MIB_EVENT_INSERT_MASIC;
	msg.typeInfo = 0;
	msg.localID = 0;
	msg.entity = masicPtr->baseData.xdiIndex;
	msg.stnID = stnID;
	msg.data.b16 = masicPtr->baseData.Index;
	msg.len1 = 0;
	msg.len2 = 0;

	/* send message to MAP */
	msg.destination = MAP_MSG_ID;
	SendMIBMessage (&msg);

	return;
}

void
#ifdef __STDC__
NotifyRemoveMASICMulti (uInt16 stnID, MASICDataPtr masicPtr)
#else
NotifyRemoveMASICMulti (stnID, masicPtr)
		uInt16           stnID;
		MASICDataPtr     masicPtr;
#endif
/*********************************************************************
Function:	This routine is called by the implementer to cause
			notification messages to be sent to the MAP when
			a MASIC object is removed from the MIB. This routine is usually
			called after calling RemoveMASICObjectMulti().
Parameters:	stnID		= index of station to use
			masicPtr	= pointer to the MASIC element in the MIB database.
Input:		masicPtr	= element contains current instance data.
Output:		Messages sent to CSP and MSP.
Return:		None.
Modification History:
*********************************************************************/
{
SMTMessage       msg;					/* notification message */

	/* set up message */
	msg.source = MIB_MSG_ID;
	msg.type = MIB_EVENT_REMOVE_MASIC;
	msg.typeInfo = 0;
	msg.localID = 0;
	msg.entity = masicPtr->baseData.xdiIndex;
	msg.stnID = stnID;
	msg.data.b16 = masicPtr->baseData.Index;
	msg.len1 = 0;
	msg.len2 = 0;

	/* send message to MAP */
	msg.destination = MAP_MSG_ID;
	SendMIBMessage (&msg);

	return;
}

void
#ifdef __STDC__
NotifyDeleteMASICMulti (uInt16 stnID, MASICDataPtr masicPtr)
#else
NotifyDeleteMASICMulti (stnID, masicPtr)
		uInt16           stnID;
		MASICDataPtr     masicPtr;
#endif
/*********************************************************************
Function:	This routine is called by the implementer to cause
			notification messages to be sent to the MAP when
			a MASIC object is deleted from the MIB. This routine is usually
			called after calling DeleteMASICObjectMulti().
Parameters:	stnID		= index of station to use
			masicPtr	= pointer to the MASIC element in the MIB database.
Input:		masicPtr	= element contains current instance data.
Output:		Messages sent to CSP and MSP.
Return:		None.
Modification History:
*********************************************************************/
{
SMTMessage       msg;					/* notification message */

	/* set up message */
	msg.source = MIB_MSG_ID;
	msg.type = MIB_EVENT_DELETE_MASIC;
	msg.typeInfo = 0;
	msg.localID = 0;
	msg.entity = masicPtr->baseData.xdiIndex;
	msg.stnID = stnID;
	msg.data.b16 = masicPtr->baseData.Index;
	msg.len1 = 0;
	msg.len2 = 0;

	/* send message to MAP */
	msg.destination = MAP_MSG_ID;
	SendMIBMessage (&msg);

	return;
}

#endif							/* XM_USE_FLEX_CONF */


/*********************************************************************
*
*	Vendor-specific MASIC-related Get and Set Routines
*
*********************************************************************/

uInt32
#ifdef __STDC__
GetSMTMASICAttr (uInt16 stnID, TLVParamType * p, uInt16 bufSize)
#else
GetSMTMASICAttr (stnID, p, bufSize)
		uInt16           stnID;
		TLVParamType    *p;
		uInt16           bufSize;
#endif
/*********************************************************************
Function:	Get vendor-specific SMT attributes related to the MASIC.
Parameters:	stnID		= index of station to use.
			p			= pointer to buffer containing the
							requested attribute information
							and used to hold attribute value
							returned.
			bufSize		= maximum length of buffer p measured in chars.
Input:		p			= contains the requested attribute ID and
							object index using the MIB TLV encoding for a Get
							Request (see SMT document).
Output:		p			= holds the attribute value requested
							using the MIB TLV encoding specified
							in the SMT document and the attribute contents
							defined in the CALM MASIC MIB.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol (see 8.4.2.2.1):
			RC_Success
				attribute retrieved successfully
			RC_NoParameter
				attribute not supported or unrecognized
			RC_IllegalOperation
				this operation is not valid for the requested attr
			RC_BUFFER_ERROR
				if the buffer is not large enough to hold the
				requested attribute
Notes:		Entry to the MIB must be from GetXDISMTAttr() in mibxdi.c to
			insure correct attribute values and a clean response buffer.
Modification History:
*********************************************************************/
{
	return (RC_NoParameter);
}

uInt32
#ifdef __STDC__
ChangeSMTMASICAttr (uInt16 stnID, TLVParamType * p)
#else
ChangeSMTMASICAttr (stnID, p)
		uInt16           stnID;
		TLVParamType    *p;
#endif
/*********************************************************************
Function:	Change vendor-specific SMT attributes related to the MASIC.
Parameters:	stnID		= index of station to use.
			p			= pointer to buffer containing the
							requested attribute information
							and used to hold attribute value
							returned.
			bufSize		= maximum length of buffer p measured in chars.
Input:		p			= contains the requested attribute ID, object
							index and new value using the MIB TLV encoding
							for a Get Request (see SMT document).
Output:		p			= holds the attribute value set
							using the MIB TLV encoding specified
							in the SMT document and the attribute contents
							defined in the CALM MASIC MIB.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol (see 8.4.2.2.1):
			RC_Success
				attribute retrieved successfully
			RC_NoParameter
				attribute not supported or unrecognized
			RC_IllegalOperation
				this operation is not valid for the requested attr
			RC_BUFFER_ERROR
				if the buffer is not large enough to hold the
				requested attribute
Notes:		Entry to the MIB must be from ChangeXDISMTAttr() in mibxdi.c to
			insure correct attribute values and a clean response buffer.
Modification History:
*********************************************************************/
{
	return (RC_NoParameter);
}

uInt32
#ifdef __STDC__
GetPATHMASICAttr (uInt16 stnID, TLVParamType * p, uInt16 bufSize)
#else
GetPATHMASICAttr (stnID, p, bufSize)
		uInt16           stnID;
		TLVParamType    *p;
		uInt16           bufSize;
#endif
/*********************************************************************
Function:	Get vendor-specific PATH attributes related to the MASIC.
Parameters:	stnID		= index of station to use.
			p			= pointer to buffer containing the
							requested attribute information
							and used to hold attribute value
							returned.
			bufSize		= maximum length of buffer p measured in chars.
Input:		p			= contains the requested attribute ID and
							object index using the MIB TLV encoding for a Get
							Request (see SMT document).
Output:		p			= holds the attribute value requested
							using the MIB TLV encoding specified
							in the SMT document and the attribute contents
							defined in the CALM MASIC MIB.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol (see 8.4.2.2.1):
			RC_Success
				attribute retrieved successfully
			RC_NoParameter
				attribute not supported or unrecognized
			RC_IllegalOperation
				this operation is not valid for the requested attr
			RC_BUFFER_ERROR
				if the buffer is not large enough to hold the
				requested attribute
Notes:		Entry to the MIB must be from GetXDIPATHAttr() in mibxdi.c to
			insure correct attribute values and a clean response buffer.
Modification History:
*********************************************************************/
{
	return (RC_NoParameter);
}


/*********************************************************************
*
*	MASIC Get and Set Routines
*
*********************************************************************/

uInt32
#ifdef __STDC__
GetMIBMASICAttr (uInt16 stnID, uInt16 bufSize, TLVParamType * p,
	MASICDataPtr masicPtr)
#else
GetMIBMASICAttr (stnID, bufSize, p, masicPtr)
		uInt16           stnID;
		uInt16           bufSize;
		TLVParamType    *p;
		MASICDataPtr     masicPtr;
#endif
/*********************************************************************
Function:	MIB access routine to get a MASIC object attribute.
Parameters:	stnID		= index of station to use.
			bufSize		= maximum length of buffer p measured in chars.
			p			= pointer to buffer containing the
							requested attribute information
							and used to hold attribute value
							returned.
			masicPtr	= pointer to MASIC's attribute structure in
							the MIB.
Input:		p			= contains the requested attribute ID and
							object index using the MIB TLV encoding for a Get
							Request (see SMT document).
Output:		p			= holds the attribute value requested
							using the MIB TLV encoding specified
							in the SMT document and the attribute contents
							defined in the XLNT Manager MIB appendix.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol (see 8.4.2.2.1):
			RC_Success
				attribute retrieved successfully
			RC_NoParameter
				attribute not supported or unrecognized
			RC_IllegalOperation
				this operation is not valid for the requested attr
			RC_BUFFER_ERROR
				if the buffer is not large enough to hold the
				requested attribute
Notes:		Entry to the MIB must be from GetMIBAttr() in mibproc.c to
			insure correct attribute values and a clean response buffer.
Modification History:
*********************************************************************/
{
uInt32           result = RC_Success;	/* return code */
MASICHWAttrType *masicHW;				/* pointer to hardware info */

	/*
	 * Check if buffer is minimum size. Header (4) + VSO index (4) + object
	 * header [attrib type + object index] (4) + value. Min value length is
	 * the same for 8, 16, and 32 bit data types.
	 */
	if (bufSize < (SIZE_TLVHdrType + (3 * SIZE_DEFAULT)))
		return (RC_BUFFER_ERROR);

	/*
	 * Reduce buffer size to get size of data portion.
	 */
	bufSize -= (SIZE_TLVHdrType + (2 * SIZE_DEFAULT));

	/*
	 * Check size of the request to see if it has enough data. The length
	 * should be vendor ID + (param type and resource index).
	 */
	if (p->paramLen < (2 * SIZE_DEFAULT))
		return (RC_LengthError);

	/*
	 * Set length to object ID + object header + most common data size.
	 */
	p->paramLen = 3 * SIZE_DEFAULT;

	/*
	 * Get value from MIB.
	 */
	switch (p->VSOTYPE)
	{
	case calmMASICIndex:
		p->VSOPARAM16 = masicPtr->baseData.Index;
		break;

	case calmMASICHardwarePresent:
		p->VSOPARAM16 = masicPtr->baseData.HardwarePresent;
		break;

	case calmMASICXDIIndex:
		p->VSOPARAM16 = masicPtr->baseData.xdiIndex;
		break;

		/*
		 * Hardware Attributes
		 */
	default:
		/*
		 * The following attributes are only available when the underlying
		 * hardware is available.
		 */
		if (masicPtr->baseData.HardwarePresent)
		{
			/* get info from local source */
			result = GetMASICDataMulti (stnID, p->VSOTYPE, masicPtr,
				(uChar *) & p->VSOGENERAL);
		}

		else
		{
			/* hardware not present */
			result = RC_NoParameter;
		}
	}

	return (result);
}

uInt32
#ifdef __STDC__
ChangeMIBMASICAttr (uInt16 stnID, Void_p bufData)
#else
ChangeMIBMASICAttr (stnID, bufData)
		uInt16           stnID;
		Void_p           bufData;
#endif
/*********************************************************************
Function:	MIB access routine to change a MASIC object attribute.
Parameters:	stnID		= index of station to use.
			bufData		= pointer to buffer containing the
							new attribute information and used to hold
							attribute value returned.
Input:		bufData		= contains the new attribute value using
							the MIB TLV encoding for a Get
							Request (see SMT document).
Output:		bufData		= holds the attribute value requested
							using the MIB TLV encoding specified
							in the SMT document.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol:
			RC_Success
				attribute changed successfully
			RC_NoParameter
				attribute or object ID not supported
			RC_LengthError
				parameter length specified is not valid
			RC_OutOfRange
				value given is out of bounds
			RC_IllegalOperation
				parameter may not be changed
*********************************************************************/
{
TLVParamType    *p;						/* parameter structure */
uInt32           result = RC_Success;	/* return value */
MIBType         *mibPtr;				/* MIB database */
Int16            index;					/* array index */
MASICBaseAttrType *masicBase;			/* MASIC attribute data */
MASICDataPtr     masicPtr;				/* MIB MASIC element */

	/*
	 * Set MIB database to use.
	 */
	mibPtr = MIBDATA (stnID, mib);

	/*
	 * Get format for buffer.
	 */
	p = (TLVParamType *) bufData;

	/*
	 * Check size of the request to see if it has enough data. The length
	 * should be vendor ID + (param type and resource index) + data.
	 */
	if (p->paramLen < (3 * SIZE_DEFAULT))
		return (RC_LengthError);

	/* check if resource ID is in the table */
	index = MFindResourceID (p->VSOINDEX, mibPtr->masicTable,
		mibPtr->smtData.xdi.Masic_Ct);
	if (index < 0)
		/* index not found */
		result = RC_NoParameter;

	else
	{
		/* map index to MASIC attributes */
		index = mibPtr->masicTable[index].index;
		masicPtr = &mibPtr->masicBase[index];
		masicBase = &masicPtr->baseData;

		/*
		 * Change value in MIB.
		 */
		switch (p->VSOTYPE)
		{
			/*****
			*	Filter GET-ONLY attributes
			*****/
		case 0:						/**** PLACEHOLDER ****/
			result = RC_IllegalOperation;
			break;

			/*****
			*	GET-REPLACE attributes
			*****/
		case 1:						/**** PLACEHOLDER ****/
			/* set attribute */

			/* call routine to store value in NV memory */
			SetMASICDataMulti (stnID, p->VSOTYPE, masicPtr, p);

			break;

		default:
			result = RC_NoParameter;
			break;
		}
	}

	return (result);
}

uInt32
#ifdef __STDC__
GetMASICValue (uInt16 stnID, uInt16 bufSize, Void_p bufData, uInt16 * bufUsed)
#else
GetMASICValue (stnID, bufSize, bufData, bufUsed)
		uInt16           stnID;
		uInt16           bufSize;
		Void_p           bufData;
		uInt16          *bufUsed;
#endif
/*********************************************************************
Function:	This routine is called by the GetMIBValue() routine
			when it has identified an MASIC vendor-specific object
			request.
Parameters:	stnID		= index of station to use.
			bufSize		= maximum length of buffer bufData
							measured in chars.
			bufData		= pointer to buffer containing the
							requested attribute information.
			bufUsed		= pointer to buffer to hold the length
							of all attributes returned.
Input:		None.
Output:		bufData		= holds the attribute value requested
							using the MIB TLV encoding specified
							in the SMT document.
			bufUsed		= length, in bytes, of the TLV encoding of
							all attributes returned in bufData.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol (see 8.4.2.2.1):
			RC_Success
				attribute retrieved successfully
			RC_NoParameter
				attribute not supported or unrecognized
			RC_IllegalOperation
				this operation is not valid for the requested attr
			RC_BUFFER_ERROR
				if the buffer is not large enough to hold the
				requested attribute. In this case, as much as
				has been processed is returned.
Modification History:
*********************************************************************/
{
TLVParamType    *param;					/* pointer to current param position */
TLVParamType    *startParam;			/* pointer to initial parameter */
uInt16           startParamLen;			/* initial parameter len */
MASICDataType   *masicPtr;				/* pointer into MASIC base table */
Int16            index;					/* loop index */
Int16            startindex,
                 endindex;				/* loop limits */
uInt16           len;					/* parameter length */
MIBType         *mibPtr;				/* MIB database */
uInt32           result;				/* return value */

	/*
	 * Set MIB database to use.
	 */
	mibPtr = MIBDATA (stnID, mib);

	/*
	 * Set startParam pointer and save initial request length.
	 */
	param = (TLVParamType *) bufData;
	startParam = param;
	startParamLen = param->paramLen;
	*bufUsed = 0;

	/*
	 * If looking for all objects, then set loop limits to the entire table.
	 * Otherwise, find the one table entry and set the limits to include only
	 * that one element.
	 */
	if (param->VSOINDEX == 0)
	{
		startindex = 0;
		endindex = MAX_MASIC_COUNT;
	}

	else
	{
		/* find index of resource ID */
		index = MFindResourceID (param->VSOINDEX,
			mibPtr->masicTable, mibPtr->smtData.xdi.Masic_Ct);
		if (index >= 0)
		{
			/*
			 * Set startindex to table entry, not just index.
			 */
			startindex = mibPtr->masicTable[index].index;
			endindex = startindex + 1;
		}

		else
		{
			/* ID not in MIB so force loop to skip it */
			startindex = MAX_MASIC_COUNT;
			endindex = 0;
		}
	}

	/* Loop through list of objects to retrieve */
	for (index = startindex, masicPtr = &mibPtr->masicBase[startindex];
		index < endindex;
		index++, masicPtr++)
	{
		if (masicPtr->inUse)
		{
			/*
			 * If there is not enough space to write the parameter, then
			 * exit. Use initial parameter length.
			 */
			if (bufSize < (startParamLen + SIZE_TLVHdrType))
			{
				result = RC_BUFFER_ERROR;
				break;
			}

			/*
			 * Copy initial info for this request. It isn't enough to have
			 * just the parameter type and vendor ID because vendor
			 * attributes may have special formats that require additional
			 * information. Copy data from first position in buffer to
			 * current location in buffer.
			 */
			MEMCOPY (param, startParam,
				SIZE_TLVHdrType + startParamLen);

			/*
			 * Process standard attribute. Vendor-specific attributes are not
			 * supported for the MASIC object.
			 */
			result = GetMIBMASICAttr (stnID, bufSize, param, masicPtr);

			if (result == RC_Success)
			{
				/* set MASIC resource ID */
				param->VSOINDEX = masicPtr->baseData.Index;
				len = param->paramLen + SIZE_TLVHdrType;
				*bufUsed += len;
				bufData = MADDPTR (Void_t, bufData, len);
				bufSize -= len;
				param = MADDPTR (TLVParamType, param, len);
			}

			else if (result != RC_NoParameter)
				break;
		}
	}

	/*
	 * If something was retrieved and last Get returned NoParameter, then
	 * change result to Success.
	 */
	if ((*bufUsed > 0) && (result == RC_NoParameter))
		/* set to Success */
		result = RC_Success;

	return (result);
}

uInt32
#ifdef __STDC__
GetMASICParams (uInt16 stnID, uInt16 bufSize, Void_p bufData, uInt16 * bufUsed)
#else
GetMASICParams (stnID, bufSize, bufData, bufUsed)
		uInt16           stnID;
		uInt16           bufSize;
		Void_p           bufData;
		uInt16          *bufUsed;
#endif
/*********************************************************************
Function:	This routine is called by the GetMIBValue() routine
			when it has identified a MASIC vendor-specific object
			request.
Parameters:	stnID		= index of station to use.
			bufSize		= maximum length of buffer bufData
							measured in chars.
			bufData		= pointer to buffer containing the
							requested attribute information.
			bufUsed		= pointer to buffer to hold the length
							of all attributes returned.
Input:		None.
Output:		bufData		= holds the attribute value requested
							using the MIB TLV encoding specified
							in the SMT document.
			bufUsed		= length, in bytes, of the TLV encoding of
							all attributes returned in bufData.
Return:		The value returned corresponds to the results codes used
			by the Parameter Manager Protocol (see 8.4.2.2.1):
			RC_Success
				attribute retrieved successfully
			RC_NoParameter
				attribute not supported or unrecognized
			RC_IllegalOperation
				this operation is not valid for the requested attr
			RC_BUFFER_ERROR
				if the buffer is not large enough to hold the
				requested attribute. In this case, as much as
				has been processed is returned.
Modification History:
*********************************************************************/
{
TLVParamType    *p;						/* request attribute pointer */
uInt16          *groupItem;				/* ptr to item in group list */
uInt16           attrLen;				/* length of attribute */
uInt16           rsrcID;				/* entity resource ID */
uInt32           result = RC_NoParameter;	/* return value */
uInt16           attrCount;				/* # of attributes in group */


	/* set initial attribute pointer */
	p = (TLVParamType *) bufData;

	/*
	 * Similar to standard FDDI MIB attributes, this routine processes
	 * attribute groups separately from single attribute requests. However,
	 * because this routine only has to deal with one object at a time, it
	 * can combine some of the processing into a single routine. Single
	 * attribute requests are sent to GetMASICValue() directly. Group
	 * requests set the group list and then process the list by successively
	 * calling GetMASICValue().
	 */
	switch (p->VSOTYPE)
	{
	case calmMASICConfigurationGrp:
		groupItem = MASICConfigurationGrpList;
		break;

	case calmMASICOperationsGrp:
		groupItem = MASICOperationsGrpList;
		break;

	default:
		groupItem = (uInt16 *) NULL;
		result = GetMASICValue (stnID, bufSize, bufData, bufUsed);
		break;
	}

	/*
	 * If groupItem is set, then an attribute group was requested.
	 */
	if (groupItem)
	{
		rsrcID = p->VSOINDEX;

		/*
		 * Loop through attribute list
		 */
		*bufUsed = 0;
		for (attrCount = 0; *groupItem != 0; groupItem++)
		{
			/*
			 * If there is not enough space to write the parameter type and
			 * length header, then exit. In this case, the header includes
			 * the FDDI VSO type, length, VSO ID, MASIC param type and MASIC
			 * index.
			 */
			if (bufSize < (SIZE_TLVHdrType + (2 * SIZE_DEFAULT)))
			{
				result = RC_BUFFER_ERROR;
				break;
			}

			/* get attribute */
			p->paramType = fddiVSOVendorAttrib;
			p->paramLen = 0;
			p->VSOOBJECT = VSO_calmMASIC;
			p->VSOTYPE = *groupItem;
			p->VSOINDEX = rsrcID;

			/* get the attribute's value */
			result = GetMIBValue (stnID, bufSize, bufData, &attrLen);

			/* if buffer is full or invalid attribute */
			if ((result == RC_BUFFER_ERROR) || (result == RC_IllegalOperation))
				/* then exit */
				break;

			/* else if get was successful */
			else if (result == RC_Success)
			{
				/* move to the next position in the buffer */
				attrCount++;
				*bufUsed += attrLen;
				bufData = MADDPTR (Void_t, bufData, attrLen);
				bufSize -= attrLen;
				p = (TLVParamType *) bufData;
			}

			/* else return value is OK, just skip this parameter */
			else
			{
				p->paramType = 0;
				p->paramLen = 0;
				result = RC_NoParameter;
			}
		}

		/*
		 * If result is RC_NoParameter, check if group actually has no
		 * attributes or just the last attribute was not available.
		 */
		if ((result == RC_NoParameter) && (attrCount > 0))
			/* group has some attributes in it */
			result = RC_Success;
	}

	return (result);
}

/*********************************************************************
*
*	MASIC Operational Routines
*
*********************************************************************/

void
#ifdef __STDC__
DetectMASICNotifications (uInt16 stnID, MIBType * mibPtr)
#else
DetectMASICNotifications (stnID, mibPtr)
		uInt16           stnID;
		MIBType         *mibPtr;
#endif
/*********************************************************************
Function:	Check all MASIC objects and determine what related
			notifications must be sent.
Parameters:	stnID		= index of station to use.
			mibPtr		= pointer to MIB data base.
Input:		None.
Output:		Sends appropriate event messages to FSP and MAP.
Return:		None.
Modification History:
*********************************************************************/
{
	return;
}


/*********************************************************************
*	If the constant MULTIPLE_STATION is not defined, then the system
*	is being compiled for single station use. In this case, the following
*	routine(s) need to be defined for the older single station calling
*	sequence.
*********************************************************************/
#ifndef MULTIPLE_STATION

uInt32
#ifdef __STDC__
AddMASICObject (uInt16 rsrcID, MASICDataPtr * masicPtr)
#else
AddMASICObject (rsrcID, masicPtr)
		uInt16           rsrcID;
		MASICDataPtr    *masicPtr;
#endif
{
	return (AddMASICObjectMulti (FIRST_STATION, rsrcID, masicPtr));
}

uInt32
#ifdef __STDC__
InsertMASICObject (uInt16 rsrcID, MASICDataPtr * masicPtr)
#else
InsertMASICObject (rsrcID, masicPtr)
		uInt16           rsrcID;
		MASICDataPtr    *masicPtr;
#endif
{
	return (InsertMASICObjectMulti (FIRST_STATION, rsrcID, masicPtr));
}

/*
 * Only include the routines to handle flexible configurations
 * if the XM_USE_FLEX_CONF constant is defined.
 */
#ifdef XM_USE_FLEX_CONF

uInt32
#ifdef __STDC__
RemoveMASICObject (uInt16 rsrcID)
#else
RemoveMASICObject (rsrcID)
		uInt16           rsrcID;
#endif
{
	return (RemoveMASICObjectMulti (FIRST_STATION, rsrcID));
}

uInt32
#ifdef __STDC__
DeleteMASICObject (uInt16 rsrcID)
#else
DeleteMASICObject (rsrcID)
		uInt16           rsrcID;
#endif
{
	return (DeleteMASICObjectMulti (FIRST_STATION, rsrcID));
}

void
#ifdef __STDC__
NotifyAddMASIC (MASICDataPtr masicPtr)
#else
NotifyAddMASIC (masicPtr)
		MASICDataPtr     masicPtr;
#endif
{
	NotifyAddMASICMulti (FIRST_STATION, masicPtr);
	return;
}

void
#ifdef __STDC__
NotifyInsertMASIC (MASICDataPtr masicPtr)
#else
NotifyInsertMASIC (masicPtr)
		MASICDataPtr     masicPtr;
#endif
{
	NotifyInsertMASICMulti (FIRST_STATION, masicPtr);
	return;
}

void
#ifdef __STDC__
NotifyRemoveMASIC (MASICDataPtr masicPtr)
#else
NotifyRemoveMASIC (masicPtr)
		MASICDataPtr     masicPtr;
#endif
{
	NotifyRemoveMASICMulti (FIRST_STATION, masicPtr);
	return;
}

void
#ifdef __STDC__
NotifyDeleteMASIC (MASICDataPtr masicPtr)
#else
NotifyDeleteMASIC (masicPtr)
		MASICDataPtr     masicPtr;
#endif
{
	NotifyDeleteMASICMulti (FIRST_STATION, masicPtr);
	return;
}

#endif							/* XM_USE_FLEX_CONF */

#endif							/* MULTIPLE_STATION */
