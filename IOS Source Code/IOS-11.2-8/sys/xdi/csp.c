/* $Id: csp.c,v 3.2 1995/11/17 18:08:42 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/csp.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: csp.c,v $
 * Revision 3.2  1995/11/17  18:08:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:54:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:23:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_csp.c xm7_v3.1.1:cset.0166:7:7 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Connection Services Process Module
		
		CSP Entry Module
		
		File:			xm7_csp.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
		
		This module is the CSP entry point for starting connection
		processing. The CSP system initialization is directed from here.

		At start of day processing, the routine CSPStartup() is called.
		This initializes all global variables accessible to all stations.

		For each station, the routine CSPMainMulti() is called (call
		CSPMain() just once in single station models). This routine
		initializes all global variables associated with that particular
		station instance.

		Modification History:
        *** SMT 7.3 ***
*********************************************************************/

#include	"cschdrsif.h"
#include	"xdihdrsif.h"

#include	"smterror.h"
#include	"fddihdr.h"
#include	"csphdr.h"
#include	"cspglbl.h"
#include	"smtmsg.h"
#include	"cspproto.h"
#include	"multistn.h"
#include	"cspgrace.h"


/*********************************************************************
		CSP Start-up Routines
*********************************************************************/

uInt32
#ifdef __STDC__
CSPMainMulti (uInt16 stnID)
#else
CSPMainMulti (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	Initialization sequence for CSP in a station.
Parameters:	stnID	= the id of the station to be initialized.
Input:		None.
Output:		Initializes CSP system by calling initialization routines.
Return:		0 is returned if the initialization was successful. Otherwise,
			the initialization error is returned.
Modification History:
*********************************************************************/
{
uInt32           result;

	/*
	 * Initialize system internal structures.
	 */
	result = InitSignalQueue (stnID);

	if (result == 0)
		result = InitCSPTimer (stnID);

	if (result == 0)
		result = InitCSPData (stnID);

	if (result == 0)
		result = InitCSPInterruptsMulti (stnID);

	if (result == 0)
		result = InitCSPMessageMulti (stnID);

	if (result == 0)
		result = MINIT_SCHED_INS (stnID);

	if (result == 0)
		/* XLNT Entry support */
		result = MINIT_GI_INS (stnID);

	/*
	 * Initialize local interfaces.
	 */
	if (result == 0)
		result = InitCSPLocalMulti (stnID);

	return (result);
}

uInt32
#ifdef __STDC__
CSPStartup (void)
#else
CSPStartup ()
#endif
/*********************************************************************
Function:	Start-up code for the CSP system.
Parameters:	None.
Input:		None.
Output:		Initializes CSP semaphores and control variables.
Return:		0 is returned if the initialization was successful. Otherwise,
			the initialization error is returned.
Notes:		THIS ROUTINE MUST BE CALLED ONLY ** ONCE **. Normally,
			this routine will be called as part of the start-of-day
			sequence prior to calling CSPMainMulti() (or CSPMain()
			for single station implementations).
Modification History:
*********************************************************************/
{
uInt32           result;
uInt16           i;

	result = InitProcessCSP ();

	/* clear all station global data */
	for (i = 0; result == 0 && i < MAX_STATION_COUNT; i++)
		result = InitCSPData (i);

	if (result == 0)
		result = StartupSignalQueue ();

	if (result == 0)
		result = StartupCSPTimer ();

	return (result);
}


/*
 * If the constant MULTIPLE_STATION is not defined, then the system is being
 * compiled for single station use. In this case, the following routine(s)
 * need to be defined for the older single station calling sequence.
 */
#ifndef MULTIPLE_STATION

uInt32
#ifdef __STDC__
CSPMain (void)
#else
CSPMain ()
#endif
{

	return (CSPMainMulti (FIRST_STATION));
}

#endif							/* MULTIPLE_STATION */
