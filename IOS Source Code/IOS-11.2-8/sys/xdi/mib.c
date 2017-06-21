/* $Id: mib.c,v 3.2 1995/11/17 18:10:23 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/mib.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: mib.c,v $
 * Revision 3.2  1995/11/17  18:10:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:25:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_mib.c xm7_v3.1.1:cset.0166:6:6 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/**********************************************************************
		Management Information Base Module

		Main processing module.

		File:			xm7_mib.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This module is the initialization sequence for the Management
		Information Base (MIB). The basic mode of operation:
			initialize process
			create and initialize MIB data
			initialize hardware (optional, may occur before MIB init)
			notify FSP and CSP (optional, see manual)
			wait for requests and events
		Requests and events come in the form of messages from other
		processes.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

#include        "xdihdrsif.h"

#include	"mibglbl.h"
#include	"mibhdr.h"
#include	"smtmsg.h"
#include	"mibproto.h"
#include	"multistn.h"
#include	"fddihdr.h"


/*********************************************************************
	MIB Functions.
*********************************************************************/

uInt32
#ifdef __STDC__
MIBMainMulti (uInt16 stnID)
#else
MIBMainMulti (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	Main processing loop for MIB. The MIB initializes
			itself and then waits for requests from other processes.
			When a request comes in, this function will route it
			to the appropriate processing function.
Parameters:	None.
Input:		None.
Output:		Initializes MIB and processes MIB requests.
Return:		0 returned if MIB process is terminated properly.
			Initialization error code returned if process terminates
			abnormally or cannot initialize.
Modification History:
*********************************************************************/
{
uInt32           result;				/* initialization results */

	/*
	 * Initialize system.
	 */
	result = InitTimeStampMulti (stnID);

	if (!result)
		result = InitMIBMessageMulti (stnID);

	if (!result)
		result = InitMIB (stnID);

	if (!result)
		result = InitMIBInterruptsMulti (stnID);

	if (!result)
		result = InitMIBLocalMulti (stnID);

	return (result);
}

uInt32
#ifdef __STDC__
MIBStartup (void)
#else
MIBStartup ()
#endif
/*********************************************************************
Function:	Start-up code for the MSP system.
Parameters:	None.
Input:		None.
Output:		Initializes single use MSP structures
Return:		0 returned if MIB process is started properly.
			Otherwise an error code is returned.
Notes:		In multistation implementations, THIS ROUTINE MUST BE
			CALLED ONLY ** ONCE **. Normally, this routine will be
			called as part of the start-of-day sequence.
			In single station implementations, this routine will be
			called prior to MIBMain() as the start-up sequence.
Modification History:
*********************************************************************/
{
uInt32           result = 0;

	result = InitMIBBuffer (&mibBufferPtr, &mibBufferLen);

	/*
	 * Adjust mibBufferLen to leave room for a zero marker at the end of a
	 * Get operation. This marks the end of the request.
	 */
	mibBufferLen -= SIZE_DEFAULT;

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
MIBMain (void)
#else
MIBMain ()
#endif
{
	return (MIBMainMulti (FIRST_STATION));
}

#endif							/* MULTIPLE_STATION */
