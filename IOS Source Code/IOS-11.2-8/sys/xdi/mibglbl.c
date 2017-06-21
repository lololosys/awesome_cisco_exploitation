/* $Id: mibglbl.c,v 3.2 1995/11/17 18:10:34 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/mibglbl.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: mibglbl.c,v $
 * Revision 3.2  1995/11/17  18:10:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:25:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_mibglbl.c xm7_v3.1.1:cset.0166:6:6 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Management Information Base Module
		
		Global Variables Module
		
		File:			xm7_mibglbl.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
		
		This module defines the global variables used throughout the
		MIB.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

#include        "xdihdrsif.h"

#include	"smtmsg.h"
#include	"fddihdr.h"
#include	"mibhdr.h"


#ifdef MULTIPLE_STATION

struct MIBAllInfo MIBInfo[MAX_STATION_COUNT];

#else							/* MULTIPLE_STATION */
/*
*	MIB Data
*/
MIBType         *mib;

#endif							/* MULTIPLE_STATION */

/*
*	Global Work Buffer.
*/
Void_p           mibBufferPtr;
uInt16           mibBufferLen;

/*
* The SMT-defined addresses are global variables because all
* compilers do not handle string constants in the same way.
* Making the addresses constants gives them a type definition
* and provides a portable method for initializing the address
* values.
*/
FDDILongAddressType mibBroadcastAddr = SMT_Broadcast_INIT;
FDDILongAddressType mibNullAddr = SMT_NULL_INIT;
FDDILongAddressType mibUnknownAddr = SMT_Unknown_INIT;
FDDILongAddressType mibDirBcnDA = SMT_Directed_Beacon_INIT;
FDDILongAddressType mibSrfDA = SMT_SRF_INIT;
FDDILongAddressType mibSbaDA = SMT_SBA_INIT;
