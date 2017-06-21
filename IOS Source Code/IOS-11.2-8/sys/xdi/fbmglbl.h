/* $Id: fbmglbl.h,v 3.2 1995/11/17 18:09:38 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/fbmglbl.h,v $
 *------------------------------------------------------------------
 * XDI include file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: fbmglbl.h,v $
 * Revision 3.2  1995/11/17  18:09:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:24:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
@(#) xm7_fbmglbl.h xm7_v3.1.1:cset.0166:6:6 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
*/
/*********************************************************************
		Frame Based Services Header File

		Global Variables Header File

		File:			xm7_fbmglbl.h
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94

		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This module declares the global variables used throughout the
		FBM.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

#ifdef MULTIPLE_STATION

extern struct FBMAllInfo FBMInfo[];

#else							/* MULTIPLE_STATION */

/* Station Information */
extern FBMStationInfoType fbmStationData;

/* MAC Information */
extern FBMMACInfoType fbmMACData[MAX_MAC_COUNT];

/* PATH Information */
extern FBMPATHInfoType fbmPATHData[MAX_PATH_COUNT];

/* PORT Information */
extern FBMPORTInfoType fbmPORTData[MAX_PORT_COUNT];

/* Status Report Information */
extern FBMSRPInfoType fbmSRPData;

#endif							/* MULTIPLE_STATION */

/*
*	Global Frame Buffer Pointer
*/
extern Void_p    fbmFrameBuffer;
extern SMTFrameHdrType *fbmFrameHeader;

/*
* The SMT-defined addresses are global variables because all
* compilers do not handle string constants in the same way.
* Making the addresses constants gives them a type definition
* and provides a portable method for initializing the address
* values.
*/
extern FDDILongAddressType fbmBroadcastAddr;
extern FDDILongAddressType fbmNullAddr;
extern FDDILongAddressType fbmUnknownAddr;
extern FDDILongAddressType fbmDirBcnDA;
extern FDDILongAddressType fbmSrfDA;
extern FDDILongAddressType fbmSbaDA;
