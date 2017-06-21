/* $Id: mibsup.c,v 3.3 1996/01/22 07:52:16 mdb Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/xdi/mibsup.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: mibsup.c,v $
 * Revision 3.3  1996/01/22  07:52:16  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  18:10:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:56:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:25:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_mibsup.c xm7_v3.1.1:cset.0166:9:9 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/**********************************************************************
		Management Information Base Module

		MIB support module.

		File:			xm7_mibsup.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This module contains functions used to calculate various
		MIB attributes when they are requested.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

#include        "xdihdrsif.h"

#include	"fddihdr.h"
#include	"mibglbl.h"
#include	"mibhdr.h"
#include	"smtmsg.h"
#include	"mibproto.h"
#include	"multistn.h"


/*********************************************************************
	MIB Functions.
*********************************************************************/

void
#ifdef __STDC__
GetMACPATHConnections (MACDataPtr macPtr, uInt16 * inputPath,
	uInt16 * outputPath)
#else
GetMACPATHConnections (macPtr, inputPath, outputPath)
		MACDataPtr       macPtr;
		uInt16          *inputPath;
		uInt16          *outputPath;
#endif
/*********************************************************************
Function:	Find the input and output path indexes for a given MAC.
Parameters:	macPtr		= MAC element to check.
			inputPath	= buffer for input path index.
			outputPath	= buffer for output path index.
Input:		Uses current MIB info.
Output:		inputPath	= path index of path providing input to
							this MAC.
			outputPath	= pathindex of path receiving output
							from this MAC.
Return:		None.
*********************************************************************/
{
	if (macPtr->baseData.HardwarePresent)
	{
		switch (macPtr->hwData.CurrentPath)
		{
		case CP_primary:
			*inputPath = PI_primary;
			*outputPath = PI_primary;
			break;

		case CP_secondary:
			*inputPath = PI_secondary;
			*outputPath = PI_secondary;
			break;

		case CP_local:
			*inputPath = macPtr->baseData.xdi.localPath;
			*outputPath = macPtr->baseData.xdi.localPath;
			break;

		default:
			*inputPath = PI_None;
			*outputPath = PI_None;
			break;
		}
	}

	else
	{
		*inputPath = PI_None;
		*outputPath = PI_None;
	}

	return;
}

void
#ifdef __STDC__
GetPORTPATHConnections (PORTDataPtr portPtr, uInt16 * inputPath,
	uInt16 * outputPath)
#else
GetPORTPATHConnections (portPtr, inputPath, outputPath)
		PORTDataPtr      portPtr;
		uInt16          *inputPath;
		uInt16          *outputPath;
#endif
/*********************************************************************
Function:	Find the input and output path indexes for a given PORT.
Parameters:	portPtr		= PORT element to check.
			inputPath	= buffer for input path index.
			outputPath	= buffer for output path index.
Input:		Uses current MIB info.
Output:		inputPath	= path index of path providing input to
							this PORT.
			outputPath	= pathindex of path receiving output
							from this PORT.
Return:		None.
*********************************************************************/
{
	if (portPtr->baseData.HardwarePresent)
	{
		switch (portPtr->hwData.CurrentPath)
		{
		case CP_primary:
			*inputPath = PI_primary;
			*outputPath = PI_primary;
			break;

		case CP_secondary:
			*inputPath = PI_secondary;
			*outputPath = PI_secondary;
			break;

		case CP_local:
			*inputPath = portPtr->baseData.xdi.localPath;
			*outputPath = portPtr->baseData.xdi.localPath;
			break;

		case CP_concatenated:
		case CP_thru:
			/* these states only apply to A, B and S ports */
			if (portPtr->baseData.My_Type == PT_a)
			{
				*inputPath = PI_secondary;
				*outputPath = PI_primary;
			}

			else
			{
				*inputPath = PI_primary;
				*outputPath = PI_secondary;
			}
			break;

		default:
			*inputPath = PI_None;
			*outputPath = PI_None;
			break;
		}
	}

	else
	{
		*inputPath = PI_None;
		*outputPath = PI_None;
	}

	return;
}

uInt32
#ifdef __STDC__
BuildPATHConfiguration (uInt16 stnID, uInt16 pathIndex,
	uInt16 bufSize, Void_p bufData)
#else
BuildPATHConfiguration (stnID, pathIndex, bufSize, bufData)
		uInt16           stnID;
		uInt16           pathIndex;
		uInt16           bufSize;
		Void_p           bufData;
#endif
/*********************************************************************
Function:	Build a PATH configuration list.
Parameters:	stnID		= station index.
			pathIndex	= index of PATH to search.
			bufSize		= size of parameter buffer less header
							and index.
			bufData		= buffer to build parameter in.
Input:		bufData		= has TLV header for parameter. Remaining
							space is cleared to 0.
Output:		bufData		= TLV encoding of the path configuration.
Return:		RC_Success		if successful.
			RC_NoParameter	if no objects have this path available
			RC_BUFFER_ERROR	if buffer is not large enough to hold the list.
								In this case, as much of the list as possible
								is returned in bufData.
*********************************************************************/
{
uInt32           result;				/* return value */
MIBType         *mibPtr;				/* MIB data base */
MACDataPtr       macPtr;				/* MAC attributes */
PORTDataPtr      portPtr;				/* PORT attributes */
Int16            i;						/* loop index */
uInt16           startIndex;			/* starting index of list */
uInt16           startType;				/* starting resource type */
uInt16           currentIndex;			/* current index in list */
uInt16           currentType;			/* current resource type */
FDDIAvailablePaths searchPath;			/* path to create list for */
TLVParamType    *p;						/* parameter to create */
FDDIResourceTag *tag;					/* current resource element in list */

	/* Get MIB */
	mibPtr = MIBDATA (stnID, mib);
	macPtr = (MACDataPtr) NULL;

	/* set search path */
	if (pathIndex == PI_primary)
		searchPath = AP_primary;

	else if (pathIndex == PI_secondary)
		searchPath = AP_secondary;

	else
		searchPath = AP_local;

	/* find first PORT object on this path */
	pathIndex--;						/* get table index */
	startIndex = 0;
	startType = 0;
	for (i = 0, portPtr = mibPtr->portBase;
		i < MAX_PORT_COUNT;
		i++, portPtr++)
	{
		if (portPtr->inUse &&
			portPtr->baseData.HardwarePresent &&
			(portPtr->hwData.AvailablePaths & searchPath))
		{
			startIndex = i;
			startType = RT_port;
			break;
		}
	}

	/* if no port on path, then search MACs */
	if (startType != RT_port)
	{
		for (i = 0, macPtr = mibPtr->macBase;
			i < MAX_MAC_COUNT;
			i++, macPtr++)
		{
			if (macPtr->inUse &&
				macPtr->baseData.HardwarePresent &&
				(macPtr->hwData.AvailablePaths & searchPath))
			{
				startIndex = i;
				startType = RT_mac;
				break;
			}
		}
	}

	/* set up parameter buffer */
	p = (TLVParamType *) bufData;
	tag = (FDDIResourceTag *) & p->PATHGENERAL;

	/* set param length to 0 */
	p->paramLen = 0;

	/*
	 * If something was found on the path, then the list is traversed to
	 * build the parameter. If no objects are on this path, then an empty
	 * list is returned.
	 */
	result = RC_Success;
	if (startType != 0)
	{
		/* follow list to create attribute */
		currentIndex = startIndex;
		currentType = startType;

		/* loop through list */
		do
		{
			if (bufSize < SIZE_FDDIResourceTag)
			{
				result = RC_BUFFER_ERROR;
				break;
			}

			/* set resource tag values */
			if (currentType == RT_mac)
			{
				macPtr = &mibPtr->macBase[currentIndex];

				/*
				 * 731-002 LJP  Add only if this MAC hardware is present.
				 */
				if (macPtr->baseData.HardwarePresent)
				{
				    tag->resourcetype = RT_mac;
				    tag->resourceindex = macPtr->baseData.Index;
				    tag->currentpath = macPtr->hwData.CurrentPath;

				    /* move to next tag */
				    /* 731-002 LJP  Use MADDPTR() instead of tag++. */
				    tag = MADDPTR (FDDIResourceTag, tag, SIZE_FDDIResourceTag);
				    bufSize -= SIZE_FDDIResourceTag;
				    p->paramLen += SIZE_FDDIResourceTag;
				}

				/* move to next object */
				currentIndex =
					macPtr->baseData.xdi.configList[pathIndex].nextIndex;
				currentType =
					macPtr->baseData.xdi.configList[pathIndex].nextType;
			}

			else
			{
				portPtr = &mibPtr->portBase[currentIndex];

				/*
				 * 731-002 LJP  Add only if this PORT entry hardware is
				 * present.
				 */
				if (portPtr->baseData.HardwarePresent)
				{
				    tag->resourcetype = RT_port;
				    tag->resourceindex = portPtr->baseData.Index;
				    tag->currentpath = portPtr->hwData.CurrentPath;

				    /* move to next tag */
				    /* 731-002 LJP  Use MADDPTR() instead of tag++. */
				    tag = MADDPTR (FDDIResourceTag, tag, SIZE_FDDIResourceTag);
				    bufSize -= SIZE_FDDIResourceTag;
				    p->paramLen += SIZE_FDDIResourceTag;
				}

				/* move to next object */
				currentIndex =
					portPtr->baseData.xdi.configList[pathIndex].nextIndex;
				currentType =
					portPtr->baseData.xdi.configList[pathIndex].nextType;
			}

			/* check for end of list */
		}
		while ((currentIndex != startIndex) || (currentType != startType));

	}

	return (result);
}

void
#ifdef __STDC__
GetConnectedResource (uInt16 stnID, uChar startIndex, uInt16 startType,
	uChar * conIndex, uInt16 * conType)
#else
GetConnectedResource (stnID, startIndex, startType, conIndex, conType)
		uInt16           stnID;
		uChar            startIndex;
		uInt16           startType;
		uChar           *conIndex;
		uInt16          *conType;
#endif
/*********************************************************************
Function:	Find next resource object connected to the given object.
Parameters:	stnID		= station index.
			startIndex	= array index of starting object.
			startType	= resource type of starting object.
			conIndex	= buffer for connected index.
			conType		= buffer for connected type.
Input:		Uses current MIB info.
Output:		conIndex	= array index of downstream resource object.
							Object is physically present and
							connected to the starting object.
			conType		= resource type of downstream resource object.
Return:		None.
Modification History:
*********************************************************************/
{
MIBType         *mibPtr;				/* MIB data base */
MACDataPtr       macPtr;				/* MAC attributes */
PORTDataPtr      portPtr;				/* PORT attributes */
uInt16           temp;					/* temp variable */
uChar            currentIndex;			/* current index in list */
uInt16           currentType;			/* current resource type */
uInt16           currentPath;			/* current resource's output path */
uInt16           nextPath;				/* next resource's input path */
uInt16           startPath;				/* start resource's output path */

	/**
	 * WRF
	 * Initialize local variables to avoid compiler warnings.
	 **/
	macPtr = NULL;
	portPtr = NULL;

	/* Get MIB */
	mibPtr = MIBDATA (stnID, mib);
	currentIndex = startIndex;
	currentType = startType;

	/* get path to search */
	if (currentType == RT_mac)
	{
		macPtr = &mibPtr->macBase[startIndex];
		GetMACPATHConnections (macPtr, &temp, &currentPath);
	}
	else
	{
		portPtr = &mibPtr->portBase[startIndex];
		GetPORTPATHConnections (portPtr, &temp, &currentPath);
	}

	/* if not isolated, */
	if (currentPath != PI_None)
	{
		/* set starting path for loop control */
		startPath = currentPath;

		/* loop until something is found or it gets back to start */
		do
		{
			/* get next resource */
			if (currentType == RT_mac)
			{
				currentIndex =
					macPtr->baseData.xdi.configList[currentPath - 1].nextIndex;
				currentType =
					macPtr->baseData.xdi.configList[currentPath - 1].nextType;
			}

			else
			{
				currentIndex =
					portPtr->baseData.xdi.configList[currentPath - 1].nextIndex;
				currentType =
					portPtr->baseData.xdi.configList[currentPath - 1].nextType;
			}

			/* get next resource on input path */
			if (currentType == RT_mac)
			{
				macPtr = &mibPtr->macBase[currentIndex];
				GetMACPATHConnections (macPtr, &nextPath, &temp);
			}
			else
			{
				portPtr = &mibPtr->portBase[currentIndex];
				GetPORTPATHConnections (portPtr, &nextPath, &temp);
			}

			/* stop if input and output match */
			if (currentPath == nextPath)
				break;

			else
			{
				/* if a port */
				if (currentType == RT_port)
				{
					/* path switches in concatenated */
					if (portPtr->hwData.CurrentPath == CP_concatenated)
					{
						/* concatenated implies A, B or S */
						if (portPtr->baseData.My_Type == PT_a)
						{
							/* A port changes primary */
							if (currentPath == PI_primary)
								currentPath = PI_secondary;
						}

						/* must be B or S */
						else if (currentPath == PI_secondary)
							currentPath = PI_primary;
					}
				}
			}

		}
		while (!((currentIndex == startIndex) && (currentType == startType)
				&& (currentPath == startPath)));
	}

	/* set return values */
	*conIndex = currentIndex;
	*conType = currentType;

	return;
}

uInt16
#ifdef __STDC__
GetMACPlacement (uInt16 stnID, PORTDataPtr startPort)
#else
GetMACPlacement (stnID, startPort)
		uInt16           stnID;
		PORTDataPtr      startPort;
#endif
/*********************************************************************
Function:	Get the MAC entity that is upstream from the given PORT.
Parameters:	stnID		= index of station to use.
			startPort	= PORT to start from.
Input:		Uses MIB database.
Output:		None.
Return:		Index of MAC that exits at this port. If no MAC outputs
			through this port, then 0 is returned.
Modification History:
*********************************************************************/
{
MIBType         *mibPtr;				/* MIB data base */
uInt16           macIndex;				/* index of MAC found */
uInt16           currentIndex;			/* current entity in list */
uInt16           currentType;			/* type of current entity */
uInt16           outPath;				/* current path being searched */
uInt16           pathIndex;				/* internal path index */
PORTDataPtr      portPtr;				/* PORT element */
MACDataPtr       macPtr;				/* MAC element */

	/**
	 * WRF
	 * Initialize variables to avoid compiler warnings.
	 **/
	outPath = 0;

	mibPtr = MIBDATA (stnID, mib);

	/*
	 * Set default return value if no MAC found.
	 */
	macIndex = 0;

	/* determine path to search on */
	switch (startPort->hwData.CurrentPath)
	{
	case CP_isolated:
		macIndex = 0;
		outPath = 0;
		break;

	case CP_local:
		outPath = startPort->baseData.xdi.localPath;
		break;

	case CP_secondary:
		outPath = PI_secondary;
		break;

	case CP_primary:
		outPath = PI_primary;
		break;

	case CP_concatenated:
		if (startPort->baseData.My_Type == PT_a)
			outPath = PI_secondary;

		else if ((startPort->baseData.My_Type == PT_b)
			|| (startPort->baseData.My_Type == PT_s))
			outPath = PI_primary;

		else
			outPath = 0;
		break;

	case CP_thru:
		if (startPort->baseData.My_Type == PT_a)
			outPath = PI_secondary;

		else if (startPort->baseData.My_Type == PT_b)
			outPath = PI_primary;

		else
			outPath = 0;
		break;
	}

	/* locate upstream entity */
	currentType = RT_port;
	currentIndex = startPort->baseData.xdi.xdiIndex;
	portPtr = startPort;
	macPtr = (MACDataPtr) NULL;

	/* continue search until MAC or PORT is found on path */
	while (outPath > 0)
	{
		pathIndex = outPath - 1;

		/* move to upstream entity */
		if (currentType == RT_mac)
		{
			currentIndex = macPtr->baseData.xdi.configList[pathIndex].prevIndex;
			currentType = macPtr->baseData.xdi.configList[pathIndex].prevType;
		}
		else
		{
			currentIndex =
				portPtr->baseData.xdi.configList[pathIndex].prevIndex;
			currentType = portPtr->baseData.xdi.configList[pathIndex].prevType;
		}

		/* check if this entity is on the path */
		if (currentType == RT_mac)
		{
			macPtr = &mibPtr->macBase[currentIndex];

			/* determine if entity is on path */
			switch (macPtr->hwData.CurrentPath)
			{
			case CP_isolated:
				break;

			case CP_local:
				/* use port's local path */
				if (outPath == macPtr->baseData.xdi.localPath)
					outPath = 0;
				break;

			case CP_secondary:
				if (outPath == PI_secondary)
					outPath = 0;
				break;

			case CP_primary:
				if (outPath == PI_primary)
					outPath = 0;
				break;
			}

			/* if MAC was found on path */
			if (outPath == 0)
				/* set MAC's index */
				macIndex = macPtr->baseData.Index;
		}

		else
		{
			portPtr = &mibPtr->portBase[currentIndex];

			/* if entity is on path, then stop search */
			switch (portPtr->hwData.CurrentPath)
			{
			case CP_isolated:
				break;

			case CP_local:
				/* use port's local path */
				if (outPath == portPtr->baseData.xdi.localPath)
					outPath = 0;
				break;

			case CP_secondary:
				if (outPath == PI_secondary)
					outPath = 0;
				break;

			case CP_primary:
				if (outPath == PI_primary)
					outPath = 0;
				break;

			case CP_concatenated:
				if (portPtr->baseData.My_Type == PT_a)
					if (outPath == PI_primary)
						outPath = 0;
					else
						/* switch paths */
						outPath = PI_primary;
				else if (outPath == PI_secondary)
					outPath = 0;
				else
					/* switch path */
					outPath = PI_secondary;
				break;

			case CP_thru:
				/*
				 * Thru case should always stop loop unless path is a local
				 * path.
				 */
				if (outPath < PI_local)
					outPath = 0;
				break;
			}
		}
	}

	return (macIndex);
}
