/* $Id: fbmglbl.c,v 3.2 1995/11/17 18:09:36 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/fbmglbl.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: fbmglbl.c,v $
 * Revision 3.2  1995/11/17  18:09:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:24:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_fbmglbl.c xm7_v3.1.1:cset.0166:8:8 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Frame Based Services Process Module

		Global Variables Module

		File:			xm7_fbmglbl.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94

		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This module defines the global variables used throughout the FBM.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

#include        "xdihdrsif.h"

#include	"smterror.h"
#include	"fddihdr.h"
#include	"fbmframe.h"
#include	"fbmhdr.h"

#ifdef MULTIPLE_STATION

struct FBMAllInfo FBMInfo[MAX_STATION_COUNT];

#else							/* MULTIPLE_STATION */

/* Station Information */
FBMStationInfoType fbmStationData;

/* MAC Information */
FBMMACInfoType   fbmMACData[MAX_MAC_COUNT];

/* PATH Information */
FBMPATHInfoType  fbmPATHData[MAX_PATH_COUNT];

/* PORT Information */
FBMPORTInfoType  fbmPORTData[MAX_PORT_COUNT];

/* Status Report Information */
FBMSRPInfoType   fbmSRPData;

#endif							/* MULTIPLE_STATION */

/*
*	Global Frame Buffer Pointer
*/
Void_p           fbmFrameBuffer;
SMTFrameHdrType *fbmFrameHeader;

/*
* The SMT-defined addresses are global variables because all
* compilers do not handle string constants in the same way.
* Making the addresses constants gives them a type definition
* and provides a portable method for initializing the address
* values.
*/
FDDILongAddressType fbmBroadcastAddr = SMT_Broadcast_INIT;
FDDILongAddressType fbmNullAddr = SMT_NULL_INIT;
FDDILongAddressType fbmUnknownAddr = SMT_Unknown_INIT;
FDDILongAddressType fbmDirBcnDA = SMT_Directed_Beacon_INIT;
FDDILongAddressType fbmSrfDA = SMT_SRF_INIT;
FDDILongAddressType fbmSbaDA = SMT_SBA_INIT;
