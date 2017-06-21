/* $Id: c4000_xdi.h,v 3.2 1995/11/17 18:45:50 hampton Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/src-c4000/c4000_xdi.h,v $
 *------------------------------------------------------------------
 * C4000 platforms XDI definitions.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c4000_xdi.h,v $
 * Revision 3.2  1995/11/17  18:45:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:26:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:56:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __C4000_XDI_H__
#define __C4000_XDI_H__

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
#define CSPPCWITHHOLD			portHW->PC_Withhold
#define CSPPCCONNST			portHW->ConnectState
#define PLAYERREG(stnID,portID,reg)     (((player_t *)cscStnDCB[stnID].portRegs[portID])->reg)
#define BMACREG(stnID,macID,reg)        (((bmac_t *)cscStnDCB[stnID].macRegs[macID])->reg)
#define PLAYERDATA(stnID,portID,var)    (((player_instance_t *)cscStnDCB[stnID].portData[0])->var[portID])
#define BMACDATA(stnID,macID,var)       (((bmac_instance_t *)cscStnDCB[stnID].macData[macID])->var)
#define BSIDATA(stnID,macID,var)        (((bsi_instance_t *)cscStnDCB[stnID].macData[macID])->var)

#define EXTRA_FC_BYTES                  NATL_EXTRA_FC_BYTES

extern unsigned long			CSPInterruptsDisabled;

#endif __C4000_XDI_H__

