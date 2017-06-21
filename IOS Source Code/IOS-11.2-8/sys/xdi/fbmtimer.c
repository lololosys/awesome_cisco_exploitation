/* $Id: fbmtimer.c,v 3.2 1995/11/17 18:10:15 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/fbmtimer.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: fbmtimer.c,v $
 * Revision 3.2  1995/11/17  18:10:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:25:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_fbmtimer.c xm7_v3.1.1:cset.0166:6:6 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Frame Based Management Module

		Process FBM Timer Module

		File:			xm7_fbmtimer.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
		
		This module is responsible for processing the timer tick for
		FSP that occurs every FBM_CLOCK_TICK seconds. For every clock
		tick, FSP handles the various periodic tasks that need to be
		performed for the different protocols done in FSP.

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
	FBM Receive Routines
*********************************************************************/

void
#ifdef __STDC__
ProcessFBMTimerMulti (uInt16 stnID)
#else
ProcessFBMTimerMulti (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	This function processes the timer tick received
			every FBM_CLOCK_TICK seconds. Each of the timer based
			protocol routines are called from here.
Parameters:	stnID	= index of station to use.
Input:		None.
Output:		Calls processing routine for timer-based protocols.
Return:		No value is returned.
Modification History:
*********************************************************************/
{

	/*
	 * Processing only when intialized.
	 */
	if (FBMDATA (stnID, fbmStationData.FBMInitialized))
	{
		/* Neighbor Notification protocol */
		ProcessNNTick (stnID);

		/* Status Reporting protocol */
		ProcessSRPTick (stnID);

		/* any local timed processing */
		ProcessLocalTickMulti (stnID);
	}

	return;
}

/*
*	If the constant MULTIPLE_STATION is not defined, then the system
*	is being compiled for single station use. In this case, the following
*	routine(s) need to be defined for the older single station calling
*	sequence.
*/
#ifndef MULTIPLE_STATION

void
#ifdef __STDC__
ProcessFBMTimer (void)
#else
ProcessFBMTimer ()
#endif
{

	ProcessFBMTimerMulti (FIRST_STATION);
	return;
}

#endif							/* MULTIPLE_STATION */
