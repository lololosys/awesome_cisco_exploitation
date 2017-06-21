/* $Id: mibconf.h,v 3.1 1995/11/09 11:35:47 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/h/mibconf.h,v $
 *------------------------------------------------------------------
 * xdi SMT fddi mib header file
 *
 * january 1993, steve elias
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
	Management Information Base Header File

	Basic MIB initialization definitions.
	THIS FILE IS TO BE PORTED BY THE IMPLEMENTOR.

	File:		mibconf.h
	Created:	12/01/89

	SID:		1.2
	Last Modified:	12/18/90
	
	

	Configuration information for the Management Information
	Base.  The MIB is the repository for information about a 
	station.  It contains such information as the number of MACs
	and PHYs, the type of station (station or concentrator), and
	operational parameters such as T-Requested.  This file is modified
	for each implementation to contain values which are constant
	throughout the particular implementation.  For example the 
	number of MACs contained in a particular station should remain
	constant, so that value is defined here. 

	Modification History:
*********************************************************************/

/*********************************************************************
*
*	The standard XLNT Designs Inc. (XDI) SMT product as shipped 
*	is configured in a complete software emulation of a FDDI station.
*	This software emulation is setup to look like a station which is a 
*	dual attach, single MAC station.  This software emulation is run
*	on an Intel 80x86 machine and thus hardware configuration info is
*	based on this architecture.  Each implementor should modify this file
*	to reflect his/her particular station type and the hardware 
*	architecture of their host.
*
*********************************************************************/

/*
*	The manufacturers Organizationally Unique Identifier (OUI) is an IEEE
*	assigned value which should come from the block of IEEE 48 bit
*	addresses assigned to this manufacturer.
*/
#define MYMANUFOUI "\x00\x00\x0c"

/*
*	The manufacturers Data field is left up to the vendor to supply.
*/
#define MYMANUFDATA "       cisco Systems"

/*
*	The user data field is left up to each implementor to define
*	as they see fit.
*/
#define MYUSERDATA ""

/*
*	The configuration policy enforced by this station is defined
*	below.  Currently we enforce no policies.
*/
#define MYCONFIGPOLICY 0

/*
*	The connection policy enforced by this station is defined
*	below.  Currently we enforce no policies.
*/
#define MYCONNECTPOLICY 0

#define	RMT_DUP1_POLICY		RMT_DUP_LEAVE
#define	RMT_DUP2_POLICY		RMT_DUP_LEAVE

#define	PHYCONNECTPOLICY	0

#define	LERCUTOFF		7
#define	LERALARM		8

/*
*	Default station values.
*	These values are given as FddiTimerTwosComplements.
*/
#define	DEFAULT_TMAX_LOWERBOUND	0xFF000000
#define	DEFAULT_TVX_LOWERBOUND	0xFF000000
#define	DEFAULT_TREQ		0xFFE00000
#define	DEFAULT_TMAX		0xFFE00000
#define	DEFAULT_TVX		0xFFFF8000
#define	DEFAULT_TMIN		0xFFFFC350
#define	DEFAULT_TPRI0		0
#define	DEFAULT_TPRI1		0
#define	DEFAULT_TPRI2		0
#define	DEFAULT_TPRI3		0
#define	DEFAULT_TPRI4		0
#define	DEFAULT_TPRI5		0
#define	DEFAULT_TPRI6		0
