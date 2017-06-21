/* $Id: mibhdr.h,v 3.2 1995/11/17 18:10:37 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/mibhdr.h,v $
 *------------------------------------------------------------------
 *
 *
 * Mar 1994, Walter Friedrich
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: mibhdr.h,v $
 * Revision 3.2  1995/11/17  18:10:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:56:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:25:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
@(#) xm7_mibhdr.h xm7_v3.1.1:cset.0166:8:8 Fri Jan 21 10:51:29 1994
Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
*/
/*********************************************************************
		Management Information Base Header File

		MSP General Header File

		File:		xm7_mibhdr.h
		Created:	03/25/92

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94

		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This file contains types and constants used to implement the SMT
		MIB in the XLNT Manager(tm).

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/


/*********************************************************************
	MSP Constants
*********************************************************************/

/*
*	XDI reserves the first 6 bytes of the manufacturer data string.
*	This constant defines the information that goes into that string.
*/
#define XDIMANUFDATA    	"XDI731"

/*
*	The ABS_present bits are used to ensure that only ONE A, B or
*	S port exists in a station's MIB at any given time.
*/
#define	NONE_PRESENT		0x00
#define	A_PRESENT			0x01
#define	B_PRESENT			0x02
#define	S_PRESENT			0x04

/*
*	Resource ID Table Opearations
*/
#define	SEARCH_FIND			1
#define	SEARCH_ADD			2
#define	SEARCH_DELETE		3

/*
*	Resource ID Search Macros
*/
#define	MFindResourceID(id, table, count)\
	SearchResourceID (id, table, count, SEARCH_FIND)
#define	MAddResourceID(id, table, count)\
	SearchResourceID (id, table, count, SEARCH_ADD)
#define	MDeleteResourceID(id, table, count)\
	SearchResourceID (id, table, count, SEARCH_DELETE)


/*
*	Known Addresses
*/
#ifdef	SMT_Broadcast_DA
#undef	SMT_Broadcast_DA
#endif
#ifdef	NULL_ADDRESS
#undef	NULL_ADDRESS
#endif
#ifdef	SMT_Unknown_Address
#undef	SMT_Unknown_Address
#endif
#ifdef	SMT_Directed_Beacon_DA
#undef	SMT_Directed_Beacon_DA
#endif
#ifdef	SMT_SRF_DA
#undef	SMT_SRF_DA
#endif

#define	SMT_Broadcast_DA		mibBroadcastAddr
#define	NULL_ADDRESS			mibNullAddr
#define	SMT_Unknown_Address		mibUnknownAddr
#define	SMT_Directed_Beacon_DA	mibDirBcnDA
#define	SMT_SRF_DA				mibSrfDA

#ifdef	USE_62_DEFS

#ifdef	BROADCAST_ADDRESS
#undef	BROADCAST_ADDRESS
#endif
#ifdef	UNKNOWN_ADDRESS
#undef	UNKNOWN_ADDRESS
#endif
#ifdef	DIR_BCN_ADDRESS
#undef	DIR_BCN_ADDRESS
#endif
#ifdef	SRF_MULTICAST_ADDRESS
#undef	SRF_MULTICAST_ADDRESS
#endif

#define	BROADCAST_ADDRESS		SMT_Broadcast_DA
#define	UNKNOWN_ADDRESS			SMT_Unknown_Address
#define	DIR_BCN_ADDRESS			SMT_Directed_Beacon_DA
#define	SRF_MULTICAST_ADDRESS	SMT_SRF_DA

#endif							/* USE_62_DEFS */
