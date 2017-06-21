/* $Id: mif68840_xdi.h,v 3.1.60.1 1996/03/21 23:24:12 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/pas/mif68840_xdi.h,v $
 *------------------------------------------------------------------
 * mif68840_xdi.h - 1F MIF68840 based PCI port adaptor support
 *                  (Port Adapter platforms common FDDI SMT definitions)
 *
 * August 1995, Farhad Sunavala
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mif68840_xdi.h,v $
 * Revision 3.1.60.1  1996/03/21  23:24:12  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.4.1  1995/12/23  04:41:31  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.1  1995/11/09  12:58:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/24  19:46:31  fsunaval
 * Placeholder files for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __MIF68840_XDI_H__
#define __MIF68840_XDI_H__

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
*       Cisco Low-End Data structures access definitions.
*********************************************************************/
#define CSPPCSTATE			portHW->PCMState
#define CSPPCNBR			portHW->NeighborType
#define CSPPCWITHHOLD		portHW->PC_Withhold
#define CSPPCCONNST			portHW->ConnectState

#define EXTRA_FC_BYTES      3          

extern void CSPHandlePHYInterrupts(uInt16 stnID, uInt16 portID);
extern void CSPHandleMACInterrupts(uInt16 stnID, uInt16 macID);
extern uInt16    ReadPHYRegMulti(uInt16 stnID, uInt16 portID, uInt16 reg);
extern uInt16    WritePHYRegMulti(uInt16 stnID, uInt16 portID, uInt16 reg,
	uInt16 val);
extern Flag LEMInterrupt(uInt16 stnID, uInt16 portID);    

#endif __MIF68840_XDI_H__

