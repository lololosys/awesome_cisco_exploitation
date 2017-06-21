/* $Id: mibglbl.h,v 3.2 1995/11/17 18:10:36 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/mibglbl.h,v $
 *------------------------------------------------------------------
 * XDI include file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: mibglbl.h,v $
 * Revision 3.2  1995/11/17  18:10:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:56:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:25:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
@(#) xm7_mibglbl.h xm7_v3.1.1:cset.0166:6:6 Fri Jan 21 10:51:29 1994
Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
*/
/*********************************************************************
		Management Information Base Header File

		MIB global data header file.

		File:			xm7_mibglbl.h
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This file contains the declaration for the global variable
		containing the Management Information Base (MIB).

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

#ifdef MULTIPLE_STATION

extern struct MIBAllInfo MIBInfo[];

#else							/* MULTIPLE_STATION */

extern MIBType  *mib;

#endif							/* MULTIPLE_STATION */

/*
*	Global Work Buffer
*/
extern Void_p    mibBufferPtr;
extern uInt16    mibBufferLen;

/*
* The SMT-defined addresses are global variables because all
* compilers do not handle string constants in the same way.
* Making the addresses constants gives them a type definition
* and provides a portable method for initializing the address
* values.
*/
extern FDDILongAddressType mibBroadcastAddr;
extern FDDILongAddressType mibNullAddr;
extern FDDILongAddressType mibUnknownAddr;
extern FDDILongAddressType mibDirBcnDA;
extern FDDILongAddressType mibSrfDA;
extern FDDILongAddressType mibSbaDA;
