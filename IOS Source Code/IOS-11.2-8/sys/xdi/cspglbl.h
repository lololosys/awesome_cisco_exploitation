
/* $Id: cspglbl.h,v 3.2 1995/11/17 18:08:52 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/cspglbl.h,v $
 *------------------------------------------------------------------
 * XDI include file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cspglbl.h,v $
 * Revision 3.2  1995/11/17  18:08:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:54:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:23:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
@(#) xm7_cspglbl.h xm7_v3.1.1:cset.0166:6:6 Fri Jan 21 10:51:29 1994
Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
*/
/*********************************************************************
		Connection Services Header File
		
		Global Variables Header File
		
		File:			xm7_cspglbl.h
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
		
		This module declares the global variables used throughout the
		CSP.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

#ifdef MULTIPLE_STATION
extern struct CSPAllInfo CSPInfo[];

#else							/* MULTIPLE_STATION */

/*
*	Station Data
*/
extern CSPStationInfoType stationData;

/*
*	MAC Data
*/
extern CSPMACInfoType macData[];

/*
*	PORT Data
*/
extern CSPPORTInfoType portData[];

/*
*	Path Data
*/
extern CSPPATHInfoType pathData[];

#endif							/* MULTIPLE_STATION */

/*
* The SMT-defined addresses are global variables because all
* compilers do not handle string constants in the same way.
* Making the addresses constants gives them a type definition
* and provides a portable method for initializing the address
* values.
*/
extern FDDILongAddressType cspBroadcastAddr;
extern FDDILongAddressType cspNullAddr;
extern FDDILongAddressType cspUnknownAddr;
extern FDDILongAddressType cspDirBcnDA;
extern FDDILongAddressType cspSrfDA;
extern FDDILongAddressType cspSbaDA;
