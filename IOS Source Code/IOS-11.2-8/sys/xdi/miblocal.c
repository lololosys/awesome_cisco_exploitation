/* $Id: miblocal.c,v 3.2 1995/11/17 18:10:43 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/miblocal.c,v $
 *------------------------------------------------------------------
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: miblocal.c,v $
 * Revision 3.2  1995/11/17  18:10:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:56:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:25:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*********************************************************************
	Management Information Base Module

	Local Processing Module

	THIS FILE IS TO BE PORTED BY THE IMPLEMENTOR.

	File:		miblocal.c
	Created:	12/01/89

	SID:		1.3
	Last Modified:	1/28/91
	
	Copyright 1990,1991 XLNT Designs Inc.

	This module contains all implementor initialization and 
	enhancment routines.

	Modification History:

	*** Updated to SMT 6.2 ***

*********************************************************************/

#include	"smtdefs.h"
#include	"smttypes.h"
#include	"smtmsg.h"


/*********************************************************************
	Defined Values
*********************************************************************/


/*********************************************************************
	Global Data
*********************************************************************/


/*********************************************************************
	External Functions
*********************************************************************/


/*********************************************************************
	FBM Processing Routines
*********************************************************************/

uInt32
InitMIBLocal ()
/*********************************************************************
Function:	Provide local initialization support. Such support can
			include hardware initialization, local data
			initialization, and system interfaces.
Parameters:	None.
Input:
Output:
Return:		0 if initialization is successful, otherwise an error
		code is returned.
*********************************************************************/
{
	return (0);
}

uInt32
LocalMIBMessages (smtmsg)
	SMTMessage	*smtmsg;
/*********************************************************************
Function:	Process messages not handled through standard MIB
		message processing.
Parameters:	smtmsg	= address of SMT message buffer.
Input:		smtmsg	= contains message to process.
Output:		Implementor defined.
Return:		Return values are reserved. Routine should always
		return 0.
*********************************************************************/
{
	return (0);
}
