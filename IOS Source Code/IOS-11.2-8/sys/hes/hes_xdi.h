/* $Id: hes_xdi.h,v 3.2.60.1 1996/04/30 07:00:48 fsunaval Exp $
 * $Source: /release/112/cvs/Xsys/hes/hes_xdi.h,v $
 *------------------------------------------------------------------
 * High-End platforms common FDDI SMT definitions.
 *
 * June 1994, Walter R. Friedrich
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: hes_xdi.h,v $
 * Revision 3.2.60.1  1996/04/30  07:00:48  fsunaval
 * CSCdi51250:  SW-VIP1: Host MIB updating needs to be completed
 * Branch: California_branch
 * 1.Code changes for formation and transfer of SMT Messages to RSP
 *   in form of love letters.
 *
 * Revision 3.2  1995/11/17  09:16:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:39:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:40:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
@(#) if7_mibvend.h if7_v2.2.1:cset.0009:2:2 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED
*/
/*********************************************************************
	Management Services Header File
	
	Vendor MIB Attributes Header File

	THIS FILE IS TO BE PORTED BY THE IMPLEMENTOR.

	File:			if7_mibvend.h
	Created:		05/06/92

	Version:		if7_v2.2.1		Mon Mar  1 16:49:14 1993
	Last Modified:	cset.0009		10/23/92
	
	Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED
	
	This header file is used by the implementor to provide additional
	attributes in the MIB.

	Modification History:
	*** SMT 7.2 ***
*********************************************************************/


/*
*	Define the constant USE_VENDOR_xxx_ATTRIBUTES to include the vendor
*	attriute data structures in the MIB.
*/

/*
#define	USE_VENDOR_SMT_ATTRIBUTES
#define	USE_VENDOR_MAC_ATTRIBUTES
#define	USE_VENDOR_MACHW_ATTRIBUTES
#define	USE_VENDOR_PATH_ATTRIBUTES
#define	USE_VENDOR_PORT_ATTRIBUTES
#define	USE_VENDOR_PORTHW_ATTRIBUTES
*/

/*
*	Define the following data structures to include data space in the
*	MIB data base for the vendor attributes.
*/
#ifdef	USE_VENDOR_SMT_ATTRIBUTES
typedef struct VendorSMTAttrStruct
{
} VendorSMTAttrType;
#endif

#ifdef	USE_VENDOR_MAC_ATTRIBUTES
typedef struct VendorMACAttrStruct
{
} VendorMACAttrType;
#endif

#ifdef	USE_VENDOR_MACHW_ATTRIBUTES
typedef struct VendorMACHWStruct
{
} VendorMACHWType;
#endif

#ifdef	USE_VENDOR_PATH_ATTRIBUTES
typedef struct VendorPATHAttrStruct
{
} VendorPATHAttrType;
#endif

#ifdef	USE_VENDOR_PORT_ATTRIBUTES
typedef struct VendorPORTAttrStruct
{
} VendorPORTAttrType;
#endif

#ifdef	USE_VENDOR_PORTHW_ATTRIBUTES
typedef struct VendorPORTHWStruct
{
} VendorPORTHWType;
#endif


/*
*	Define the attribute names here. Refer to file mibdefs.h for examples
*	of vendor-specific attribute definitions.
*/

/*
*	REMEMBER: Define the COUNT constants in smtdefs.h for the number of
*	vendor-specific notifications for each object:
*		VENDOR_SMT_EC_COUNT, VENDOR_MAC_EC_COUNT,
*		VENDOR_PATH_EC_COUNT and VENDOR_PORT_EC_COUNT
*/

/*********************************************************************
*       Cisco High-End Data structures access definitions.
*********************************************************************/
#define CSPPCSTATE	(STADATA (stnID, hwIDB[0])->status & IDB_VIP) ? \
			 portHW->PCMState :				\
			 get_pc_state (STADATA (stnID, hwIDB[0]),	\
				      portPtr->baseData.xdi.xdiIndex)
#define CSPPCNBR  	(STADATA (stnID, hwIDB[0])->status & IDB_VIP) ? \
			 portHW->NeighborType :				\
			 get_pc_neighbor (STADATA (stnID, hwIDB[0]),	\
				      portPtr->baseData.xdi.xdiIndex)
#define CSPPCWITHHOLD   (STADATA (stnID, hwIDB[0])->status & IDB_VIP) ? \
			 portHW->PC_Withhold : 				\
			 get_pc_withhold (STADATA (stnID, hwIDB[0]),	\
				      portPtr->baseData.xdi.xdiIndex)
#define CSPPCCONNST	(STADATA (stnID, hwIDB[0])->status & IDB_VIP) ? \
			 portHW->ConnectState :				\
			 get_pc_conn_state (STADATA (stnID, hwIDB[0]),	\
				      portPtr->baseData.xdi.xdiIndex)
#define EXTRA_FC_BYTES  3
