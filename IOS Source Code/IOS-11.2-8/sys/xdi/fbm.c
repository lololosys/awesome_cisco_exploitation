/* $Id: fbm.c,v 3.2 1995/11/17 18:09:27 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/fbm.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: fbm.c,v $
 * Revision 3.2  1995/11/17  18:09:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:24:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_fbm.c xm7_v3.1.1:cset.0166:9:9 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Frame Based Management Module

		Main Processing Module

		File:			xm7_fbm.c
		Created:		12/01/89

		Version:		xm7_v3.1.1	Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This module is responsible for initializing the basic FBM
		processing.

		With release 7.3, this file contains the following conditional
		compilation options:

			XM_USE_RAF		include code for RAF processing.
			XM_USE_ESF		include code for ESF processing.

		If any of the constants listed above are defined, then the
		associated portion of code is included in the compilation.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

#include        "xdihdrsif.h"

#include	"smterror.h"
#include	"fddihdr.h"
#include	"smtmsg.h"
#include	"fbmframe.h"
#include	"fbmhdr.h"
#include	"fbmglbl.h"
#include	"fbmproto.h"
#include	"multistn.h"


/*********************************************************************
	FBM Processing Routines
*********************************************************************/

uInt32
#ifdef __STDC__
FBMMainMulti (uInt16 stnID)
#else
FBMMainMulti (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	Begin FBM processing.
Parameters:	stnID	= index of station to use.
Input:		None.
Output:		Initializes FBM.
Return:		0 if successful, or error code.
Modification History:
*********************************************************************/
{
uInt32           result;				/* init result code */

	/*
	 * Initialize values and interfaces.
	 */
	result = InitFBMQueue (stnID);

	if (result == 0)
		result = InitFBMData (stnID);

	if (result == 0)
		result = InitFBMTransId (stnID);

	if (result == 0)
		result = InitFBMMessageMulti (stnID);

	if (result == 0)
		result = InitFBMInterruptsMulti (stnID);

	if (result == 0)
		result = InitFBMFrameMulti (stnID);

	if (result == 0)
		result = InitFBMTimerMulti (stnID);

	if (result == 0)
		result = InitSRP (stnID);

	if (result == 0)
		result = InitFBMLocalMulti (stnID);

	/*
	 * If XM_USE_RAF is defined, then initialize processing for and protocols
	 * using RAFs.
	 */
#ifdef XM_USE_RAF
	if (result == 0)
		result = InitRAFLocalMulti (stnID);
#endif							/* XM_USE_RAF */

	/*
	 * If XM_USE_ESF is defined, then initialize processing for and protocols
	 * using ESFs.
	 */
#ifdef XM_USE_ESF
	if (result == 0)
		result = InitESFLocalMulti (stnID);
#endif							/* XM_USE_ESF */

	return (result);
}

uInt32
#ifdef __STDC__
FBMStartup (void)
#else
FBMStartup ()
#endif
/*********************************************************************
Function:	Start-up code for the FSP system.
Parameters:	None.
Input:		None.
Output:		Initializes FSP semaphores and control variables.
Return:		0 is returned if the initialization was successful. Otherwise,
			the initialization error is returned.
Notes:		In multistation implementations, THIS ROUTINE MUST BE
			CALLED ONLY ** ONCE **. Normally, this routine will be
			called as part of the start-of-day sequence.
			In single station implementations, this routine will be
			called prior to FBMMain() as the start-up sequence.
Modification History:
*********************************************************************/
{
uInt32           result;				/* init result code */
uInt16           i;						/* loop counter */

	result = InitProcessFBM ();

	/* clear all station global data */
	for (i = 0; result == 0 && i < MAX_STATION_COUNT; i++)
		result = InitFBMData (i);

	if (result == 0)
		result = StartupFBMQueue ();

	if (result == 0)
	{
		result = InitFBMFrameBuffer (&fbmFrameBuffer);
		fbmFrameHeader = (SMTFrameHdrType *) fbmFrameBuffer;
	}

	return (result);
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
FBMMain (void)
#else
FBMMain ()
#endif
{

	return (FBMMainMulti (FIRST_STATION));
}

#endif							/* MULTIPLE_STATION */
