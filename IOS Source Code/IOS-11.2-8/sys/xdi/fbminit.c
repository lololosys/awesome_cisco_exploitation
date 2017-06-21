/* $Id: fbminit.c,v 3.2.62.1 1996/03/18 22:51:29 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/xdi/fbminit.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: fbminit.c,v $
 * Revision 3.2.62.1  1996/03/18  22:51:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/03/07  11:20:31  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2  1995/11/17  18:09:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:24:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_fbminit.c xm7_v3.1.1:cset.0173:8:8 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Frame Based Management Module

		Initialization Module

		File:			xm7_fbminit.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0173		01/14/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This module is responsible for initializing FBM for
		processing. The basic system services are initialized
		when FBM starts. However, FBM cannot begin its function
		until the MIB has completed its initialization. After
		the MIB has been initialized, FBM completes its initialization
		through a call to InitFBMMIBDataMulti().  After FBM completes
		its initialization and FBM is ready to work.

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
	FBM Processing Routines
*********************************************************************/

void
#ifdef __STDC__
InitFBMMACData (FBMMACInfoType * macPtr)
#else
InitFBMMACData (macPtr)
		FBMMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Reset base MAC FBM data.
Parameters:	macPtr	= pointer to MAC element to initialize.
Input:		macPtr	= macPtr->ID is already set.
Output:		Clears MAC base data to start values.
Return:		None.
*********************************************************************/
{
	macPtr->hwPresent = NOTPRESENT;
	macPtr->Index = 0;

	return;
}

void
#ifdef __STDC__
InitFBMMACHWData (FBMMACInfoType * macPtr)
#else
InitFBMMACHWData (macPtr)
		FBMMACInfoType  *macPtr;
#endif
/*********************************************************************
Function:	Reset hardware-dependent MAC FBM data.
Parameters:	macPtr	= pointer to MAC element to initialize.
Input:		macPtr	= macPtr->ID is already set.
Output:		Clears MAC hardware data to start values.
Return:		None.
*********************************************************************/
{
	MCopyAddress (macPtr->SMTAddress, SMT_Unknown_Address);
	MCopyAddress (macPtr->UNA, SMT_Unknown_Address);
	MCopyAddress (macPtr->DNA, SMT_Unknown_Address);
	macPtr->SM_MA_Avail = FALSE;
	macPtr->myDuplicate = FALSE;
	macPtr->myUNADuplicate = FALSE;
	macPtr->Dup_Addr_Test = DAT_none;
	macPtr->TNN = T_Notify_Max;
	macPtr->TVU = 0;
	macPtr->TVD = 0;
	macPtr->NNTransactionID = 0;
	macPtr->resourceID = 0;

	return;
}

void
#ifdef __STDC__
InitFBMPORTData (FBMPORTInfoType * portPtr)
#else
InitFBMPORTData (portPtr)
		FBMPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Reset base PORT FBM data.
Parameters:	portPtr	= pointer to PORT element to initialize.
Input:		portPtr	= portPtr->ID is already set.
Output:		Clears PORT base data to start values.
Return:		None.
*********************************************************************/
{
	portPtr->Index = 0;
	portPtr->portType = PT_none;
	portPtr->hwPresent = NOTPRESENT;

	return;
}

void
#ifdef __STDC__
InitFBMPORTHWData (FBMPORTInfoType * portPtr)
#else
InitFBMPORTHWData (portPtr)
		FBMPORTInfoType *portPtr;
#endif
/*********************************************************************
Function:	Reset hardware-dependent PORT FBM data.
Parameters:	portPtr	= pointer to PORT element to initialize.
Input:		portPtr	= portPtr->ID is already set.
Output:		Clears PORT hardware-dependent data to start values.
Return:		None.
*********************************************************************/
{
	portPtr->resourceID = 0;

	return;
}

uInt32
#ifdef __STDC__
InitFBMData (uInt16 stnID)
#else
InitFBMData (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	Reset all FBM data.
Parameters:	stnID	= index of station to use.
Input:		None.
Output:		Initializes FBM global data.
Return:		0 returned if initialization was successful.
			Otherwise an error value is returned.
*********************************************************************/
{
uInt16           i;						/* loop counter */
FBMStationInfoType *stnPtr;				/* station info */
FBMMACInfoType  *macPtr;				/* MAC element */
FBMPATHInfoType *pathPtr;				/* PATH element */
FBMPORTInfoType *portPtr;				/* PORT element */

	/* clear station data */
	stnPtr = &(FBMDATA (stnID, fbmStationData));
	MEMZERO (stnPtr, sizeof (FBMStationInfoType));
	stnPtr->S_Index = MAX_PORT_COUNT;
	stnPtr->A_Index = MAX_PORT_COUNT;
	stnPtr->B_Index = MAX_PORT_COUNT;
	stnPtr->monitorMode = FMM_MONITOR_NONE;

	/* clear MAC data */
	for (i = 0, macPtr = &(FBMDATA (stnID, fbmMACData[0]));
		i < MAX_MAC_COUNT;
		i++, macPtr++)
	{
		macPtr->ID = i;
		InitFBMMACData (macPtr);
		InitFBMMACHWData (macPtr);
	}

	/* clear PATH data */
	for (i = 0, pathPtr = &(FBMDATA (stnID, fbmPATHData[0]));
		i < MAX_PATH_COUNT;
		i++, pathPtr++)
	{
		MEMZERO (pathPtr, sizeof (FBMPATHInfoType));
		pathPtr->ID = i;
	}

	/* clear PORT data */
	for (i = 0, portPtr = &(FBMDATA (stnID, fbmPORTData[0]));
		i < MAX_PORT_COUNT;
		i++, portPtr++)
	{
		portPtr->ID = i;
		InitFBMPORTData (portPtr);
		InitFBMPORTHWData (portPtr);
	}

	return (0);
}

uInt32
#ifdef __STDC__
InitFBMMIBDataMulti (uInt16 stnID)
#else
InitFBMMIBDataMulti (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	Initialize FBM global data from MIB after MIB has
			been initialized.
Parameters:	stnID	= index of station to use.
Input:		None.
Output:		All MIB-based global values set to current MIB settings.
Return:		0 if successful, otherwise an error value is returned.
Notes:		This function may only be called AFTER MIB initialization
			has been completed.
Modification History:
*********************************************************************/
{
TLVParamType     buffer;				/* Get request buffer */
uInt16           i;						/* loop counter */
FBMStationInfoType *stnPtr;				/* station info */
FBMMACInfoType  *macPtr;				/* MAC element */
FBMPATHInfoType *pathPtr;				/* PATH element */
FBMPORTInfoType *portPtr;				/* PORT element */
uInt16           resID;					/* counter for resource ID */
uInt32           result;				/* Get request result */

	/*
	 * Initialize station data
	 */
	stnPtr = &(FBMDATA (stnID, fbmStationData));
	resID = 1;

	buffer.paramType = fddiSMTStationId;
	buffer.paramLen = 0;
	FBMGetMIBAttrMulti (stnID, sizeof (buffer), (Void_p) & buffer);
	MEMCOPY (&stnPtr->stationID, buffer.SMTOTHER, SIZE_SMTStationIdType);

	buffer.paramType = fddiSMTMac_Ct;
	buffer.paramLen = 0;
	FBMGetMIBAttrMulti (stnID, sizeof (buffer), (Void_p) & buffer);
	stnPtr->macCount = buffer.SMTPARAM8;

	buffer.paramType = fddiSMTNonMaster_Ct;
	buffer.paramLen = 0;
	FBMGetMIBAttrMulti (stnID, sizeof (buffer), (Void_p) & buffer);
	stnPtr->attachCount = buffer.SMTPARAM8;

	buffer.paramType = fddiSMTMaster_Ct;
	buffer.paramLen = 0;
	FBMGetMIBAttrMulti (stnID, sizeof (buffer), (Void_p) & buffer);
	stnPtr->masterCount = buffer.SMTPARAM8;

	buffer.paramType = fddiSMTOpVersionId;
	buffer.paramLen = 0;
	FBMGetMIBAttrMulti (stnID, sizeof (buffer), (Void_p) & buffer);
	stnPtr->opVersionID = buffer.SMTPARAM16;

	buffer.paramType = fddiSMTHiVersionId;
	buffer.paramLen = 0;
	FBMGetMIBAttrMulti (stnID, sizeof (buffer), (Void_p) & buffer);
	stnPtr->hiVersionID = buffer.SMTPARAM16;

	buffer.paramType = fddiSMTLoVersionId;
	buffer.paramLen = 0;
	FBMGetMIBAttrMulti (stnID, sizeof (buffer), (Void_p) & buffer);
	stnPtr->loVersionID = buffer.SMTPARAM16;

	buffer.paramType = fddiSMTT_Notify;
	buffer.paramLen = 0;
	FBMGetMIBAttrMulti (stnID, sizeof (buffer), (Void_p) & buffer);
	stnPtr->T_Notify = buffer.SMTPARAM16;

	stnPtr->macPresent = 0;
	stnPtr->masterPresent = 0;
	stnPtr->attachPresent = 0;

	/*
	 * Initialize PATH data.
	 */
	for (i = 0, pathPtr = FBMDATA (stnID, fbmPATHData);
		i < MAX_PATH_COUNT;
		i++, pathPtr++)
	{
		buffer.paramType = fddiPATHIndex;
		buffer.paramLen = 4;
		buffer.PATHINDEX = i;
		result = FBMGetMIBAttrMulti (stnID, sizeof (buffer), (Void_p) & buffer);
		pathPtr->Index = buffer.PATHPARAM16;
	}

	/*
	 * Initialize PORT data. PORTs done before MACs to accomodate setting the
	 * resourceID field.
	 */
	for (i = 0, portPtr = FBMDATA (stnID, fbmPORTData);
		i < MAX_PORT_COUNT;
		i++, portPtr++)
	{
		buffer.paramType = fddiPORTIndex;
		buffer.paramLen = 4;
		buffer.PORTINDEX = i;
		result = FBMGetMIBAttrMulti (stnID, sizeof (buffer), (Void_p) & buffer);

		/* if PORT exists at this location */
		if (result == RC_Success)
		{
			/*
			 * Set portPtr->Index after checking result from Get.
			 */
			portPtr->Index = buffer.PORTPARAM16;

			buffer.paramType = fddiPORTMy_Type;
			buffer.paramLen = 4;
			buffer.PORTINDEX = i;
			result = FBMGetMIBAttrMulti (stnID, sizeof (buffer),
				(Void_p) & buffer);
			portPtr->portType = buffer.PORTPARAM16;

			/* set A, B or S index */
			if (portPtr->portType == PT_a)
				stnPtr->A_Index = i;

			else if (portPtr->portType == PT_b)
				stnPtr->B_Index = i;

			else if (portPtr->portType == PT_s)
				stnPtr->S_Index = i;

			buffer.paramType = fddiPORTHardwarePresent;
			buffer.paramLen = 4;
			buffer.PORTINDEX = i;
			result = FBMGetMIBAttrMulti (stnID, sizeof (buffer),
				(Void_p) & buffer);
			portPtr->hwPresent = buffer.PORTPARAM16;

			if (portPtr->hwPresent)
			{
				if (portPtr->portType == PT_m)
					stnPtr->masterPresent++;
				else
					stnPtr->attachPresent++;

				/* set resourceID only for ports present */
				portPtr->resourceID = resID;
				resID++;
			}
		}
	}

	/*
	 * Initialize MAC data.
	 */
	for (i = 0, macPtr = FBMDATA (stnID, fbmMACData);
		i < MAX_MAC_COUNT;
		i++, macPtr++)
	{
		buffer.paramType = fddiMACIndex;
		buffer.paramLen = 4;
		buffer.MACINDEX = i;
		result = FBMGetMIBAttrMulti (stnID, sizeof (buffer), (Void_p) & buffer);

		/* if a MAC exists at this location */
		if (result == RC_Success)
		{
			/*
			 * Set macPtr->Index after checking result from Get.
			 */
			macPtr->Index = buffer.MACPARAM16;

			buffer.paramType = fddiMACHardwarePresent;
			buffer.paramLen = 4;
			buffer.MACINDEX = i;
			FBMGetMIBAttrMulti (stnID, sizeof (buffer), (Void_p) & buffer);
			macPtr->hwPresent = buffer.MACPARAM16;

			if (macPtr->hwPresent)
			{
				stnPtr->macPresent++;

				buffer.paramType = fddiMACSMTAddress;
				buffer.paramLen = 4;
				buffer.MACINDEX = i;
				FBMGetMIBAttrMulti (stnID, sizeof (buffer), (Void_p) & buffer);
				MCopyAddress (macPtr->SMTAddress,
					buffer.MACADDR);

				/* set resourceID only for MACs present */
				macPtr->resourceID = resID;
				resID++;
			}
		}
	}

	/*
	 * Set initialized flag.
	 */
	stnPtr->FBMInitialized = SET;

	return (0);
}


/*
*	If the constant MULTIPLE_STATION is not defined, then the system
*	is being compiled for single station use. In this case, the following
*	routine(s) need to be defined for the older single station calling
*	sequence.
*/
#ifndef MULTIPLE_STATION

uInt32
#ifdef __STDC__
InitFBMMIBData (void)
#else
InitFBMMIBData ()
#endif
{

	return (InitFBMMIBDataMulti (FIRST_STATION));
}

#endif							/* MULTIPLE_STATION */
