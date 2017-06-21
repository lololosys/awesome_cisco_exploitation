/* $Id: if_xdi.h,v 3.3.46.2 1996/08/03 23:27:02 fox Exp $
 * $Source: /release/112/cvs/Xsys/if/if_xdi.h,v $
 *------------------------------------------------------------------
 * Platform independent XDI SMT interface include files.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Longer description here.
 *------------------------------------------------------------------
 * $Log: if_xdi.h,v $
 * Revision 3.3.46.2  1996/08/03  23:27:02  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.3.46.1  1996/03/18  20:13:56  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.16.2  1996/03/07  09:44:04  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.16.1  1996/02/20  14:23:34  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/12/07  22:41:52  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.2  1995/11/17  09:29:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:50:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:53:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__IF_XDI_H__
#define __IF_XDI_H__

#include	"../xdi/smttypes.h"
#include	"../xdi/mibdefs.h"
#include	"../xdi/mibtypes.h"

/*********************************************************************
        Cisco Software options
*********************************************************************/
#define         XM_NO_M_PORT
#define         MFSPFrameLimit(count)     (process_would_suspend())

/*
@(#) if7_smtmacro.h if7_v2.2.1:cset.0009:2:2 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED
*/
/*********************************************************************
	Station Management Header File

	SMT Macro File

	THIS FILE IS TO BE PORTED BY THE IMPLEMENTOR.
	
	File:			if7_smtmacro.h
	Created:		12/01/89

	Version:		if7_v2.2.1		Mon Mar  1 16:49:14 1993
	Last Modified:	cset.0009		10/23/92
	
	Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED
	
	This is the header file used throughout the SMT modules. This
	file defines C macros used to make the SMT source code portable.
	The macros should be reviewed by the implementor to insure that
	the target C and operating system environment will perform as
	expected.

	Modification History:
	*** SMT 7.2 ***
		Based on 6.2.3 multistation code.
*********************************************************************/

/*********************************************************************
	Standard C Macros
*********************************************************************/

/*********************************************************************
macro		MSetBit (variable, bit, state)

Function:	Set or clear a bit in an integer variable.
Parameters:	variable	= the variable to change.
			bit			= bit number 0-based from right to left.
			state		= SET to set bit to 1, CLEAR to clear bit to 0.
Input:		None.
Output:		variable contains bit changed to new setting.
Return:		No value returned.
Notes:		None.
Modification History:
*********************************************************************/

#define	MSetBit(variable, bit, state)\
		((variable) =\
			(state) ? (variable) | (1 << (bit))\
				: (variable) & ~(1 << (bit)))


/*********************************************************************
macro		MReadBit (variable, bit)

Function:	Read setting of a bit in an integer.
Parameters:	variable	= integer value to read.
			bit			= bit number 0-based from right to left.
Input:		None.
Output:		None.
Return:		TRUE if bit is set to 1, FALSE if bit is set to 0.
Notes:		None.
Modification History:
*********************************************************************/

#define	MReadBit(variable, bit)\
		(((variable) & (1 << (bit))) != 0)


/*********************************************************************
macro		MEMCOPY (destination, source, length)

Function:	Copy chars in memory.
Parameters:	destination	= address of target buffer.
			source		= address of source buffer.
			length		= number of chars to transfer.
Input:		None.
Output:		Sets destination contents to contain same contents as source.
Return:		No value returned.
Notes:		None.
Modification History:
*********************************************************************/

#define	MEMCOPY(destination, source, length)\
		bcopy (source, destination, length)


/*********************************************************************
macro		MEMZERO (destination, length)

Function:	Clear length chars in memory.
Parameters:	destination	= address of target buffer.
			length		= number of chars to clear to zero.
Input:		None.
Output:		Sets length chars starting at destination to zero.
Return:		No value returned.
Notes:		None.
Modification History:
*********************************************************************/

#define	MEMZERO(destination, length)\
		memset(destination, 0, length)


/*********************************************************************
macro		GETMEMORY (size)

Function:	Get a block of memory size bytes in length.
Parameters:	size	= number of chars in memory block.
Input:		None.
Output:		None.
Return:		Pointer to contiguous block of memory containing at least
			size chars.
			NULL returned if memory allocation fails.
Notes:		None.
Modification History:
*********************************************************************/

#define	GETMEMORY(size) malloc_named((unsigned) size, "XDI data")


/*********************************************************************
macro		FREEMEMORY (buf)

Function:	Free a block of memory allocated by GETMEMORY().
Parameters:	buf	= address of memory block to free.
Input:		None.
Output:		None.
Return:		None.
Notes:		None.
Modification History:
*********************************************************************/

#define	FREEMEMORY(buf) free ((char *) buf)


/*********************************************************************
macro		MCopyAddress (destination, source)

Function:	Copy an FDDI address.
Parameters:	destination	= address of target buffer.
			source		= address of source buffer.
Input:		None.
Output:		Sets destination to contain same address as source.
Return:		No value returned.
Notes:		None.
Modification History:
*********************************************************************/

#define	MCopyAddress(destination, source)\
		MEMCOPY (destination, source, sizeof (FDDILongAddressType))


/*********************************************************************
macro		MCompareAddress (a1, a2, result)

Function:	Compare two FDDI addresses.
Parameters:	a1	= address of buffer containing first MAC addresses to compare.
			a2	= address of buffer containing second MAC addresses to compare.
Input:		None.
Output:		None.
Return:		Same value as strcmp().
Notes:		The sample implementation is for a generic, unsigned
			comparison of bytes. Alternate implementations for
			faster processing are recommended. These tend to be
			environment specific and have not been used for the
			example.
Modification History:
*********************************************************************/

#define	Mptr(p,n)	(*(((uChar *)(p)) + n))

#define	MCompareAddress(a1, a2, result)\
	(result) = ((Mptr(a1, 0) != Mptr(a2,0))\
			? (Int16) Mptr(a1,0) - (Int16) Mptr(a2,0) :\
		((Mptr(a1,1) != Mptr(a2,1))\
			? (Int16) Mptr(a1,1) - (Int16) Mptr(a2,1) :\
		((Mptr(a1,2) != Mptr(a2,2))\
			? (Int16) Mptr(a1,2) - (Int16) Mptr(a2,2) :\
		((Mptr(a1,3) != Mptr(a2,3))\
			? (Int16) Mptr(a1,3) - (Int16) Mptr(a2,3) :\
		((Mptr(a1,4) != Mptr(a2,4))\
			? (Int16) Mptr(a1,4) - (Int16) Mptr(a2,4) :\
		((Mptr(a1,5) != Mptr(a2,5))\
			? (Int16) Mptr(a1,5) - (Int16) Mptr(a2,5) :\
		0 ))))))


/*********************************************************************
macro		MFddi2Usecs (t)

Function:	Convert an FDDI time value (80ns interval count) to
			microseconds.
Parameters:	t	= FDDI time value to convert.
Input:		None.
Output:		None.
Return:		Value converted to microseconds.
Notes:		None.
Modification History:
*********************************************************************/

#define MFddi2Usecs(t)  (((Int32) (t << 1)) / 25)


/*********************************************************************
macro		MUsecs2Fddi (t)

Function:	Convert microseconds into an FDDI time value (80ns
			interval count).
Parameters:	t	= time value in microseconds to convert.
Input:		None.
Output:		None.
Return:		Value converted to 80ns FDDI time.
Notes:		None.
Modification History:
*********************************************************************/

#define MUsecs2Fddi(t)  ((((Int32) t) * 25) >> 1)


/*********************************************************************
macro		Mhtons (x)

Function:	This function converts a 16 bit half word quantity from
			litte endian format to network byte order.  On machines such
			as Intel processors half words are stored "backwards".  On the
			network we always send most significant byte first, so we must
			convert half words to the proper order.  If this machine is a
			big endian machine then we do nothing.
Parameters:	value   = Half word to be converted.
Input:		Defines macro if constant LITTLE_ENDIAN is defined in smtdefs.h
Output:		None.
Return:		Converted half word.
*********************************************************************/
#ifndef LITTLE_ENDIAN
#define Mhtons(x)       (x)
#else
#define Mhtons(x)       (((((uInt16) (x)) << 8) & 0xff00) | \
                         ((((uInt16) (x)) >> 8) & 0x00ff))
#endif


/*********************************************************************
macro		Mhtonl (x)

Function:	This function converts a 32 bit half word quantity from
			litte endian format to network byte order.
			If this machine is a big endian machine then we do nothing.
Parameters:	value   = 32 bit value to be converted.
Input:		Defines macro if constant LITTLE_ENDIAN is defined in smtdefs.h
Output:		None.
Return:		Converted 32 bit value.
*********************************************************************/
#ifndef LITTLE_ENDIAN
#define Mhtonl(x)       (x)
#else
#define Mhtonl(x)       (((((uInt32) (x)) << 24) & 0xff000000L) | \
                         ((((uInt32) (x)) <<  8) & 0x00ff0000L) | \
                         ((((uInt32) (x)) >>  8) & 0x0000ff00L) | \
                         ((((uInt32) (x)) >> 24) & 0x000000ffL))
#endif


/*********************************************************************
macro		Mntohl (x)

Function:	This function converts a 32 bit value from network byte order
			to host byte order.  The only time any conversion is needed is
			if the host is a little endian machine.
Parameters:	value   = 32-bit value to convert.
Input:		Defines macro if constant LITTLE_ENDIAN is defined in smtdefs.h
Output:		None.
Return:		Converted 32-bit value.
*********************************************************************/
#ifndef LITTLE_ENDIAN
#define Mntohl(x)       (x)
#else
#define Mntohl(x)       (((((uInt32) (x)) << 24) & 0xff000000L) | \
                         ((((uInt32) (x)) <<  8) & 0x00ff0000L) | \
                         ((((uInt32) (x)) >>  8) & 0x0000ff00L) | \
                         ((((uInt32) (x)) >> 24) & 0x000000ffL))
#endif


/*********************************************************************
macro		Mntohs (x)

Function:	This function converts a 16 bit half word quantity from
			litte endian format to network byte order.  On machines such
			as Intel processors half words are stored "backwards".  On the
			network we always send most significant byte first, so we must
			convert half words to the proper order.  If this machine is a
			big endian machine then we do nothing.
Parameters:	value   = Half word to be converted.
Input:		Defines macro if constant LITTLE_ENDIAN is defined in smtdefs.h
Output:		None.
Return:		Converted half word.
*********************************************************************/
#ifndef LITTLE_ENDIAN
#define Mntohs(x)       (x)
#else
#define Mntohs(x)       (((((uInt16) (x)) << 8) & 0xff00) | \
                         ((((uInt16) (x)) >> 8) & 0x00ff))
#endif

/*********************************************************************
macro		Mntohs (x)

Function:	This function converts a 16 bit half word quantity from
			litte endian format to network byte order.  On machines such
			as Intel processors half words are stored "backwards".  On the
			network we always send most significant byte first, so we must
			convert half words to the proper order.  If this machine is a
			big endian machine then we do nothing.
Parameters:	value   = Half word to be converted.
Input:		Defines macro if constant LITTLE_ENDIAN is defined in smtdefs.h
Output:		None.
Return:		Converted half word.
*********************************************************************/
#define        MChangeBits(i, bits, setting)	i = (((i) & ~(bits)) | (setting))

/*
@(#) if7_smtmsgim.h if7_v2.2.1:cset.0009:2:2 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED
*/
/*********************************************************************
	Station Management Header File
	
	Implementor Messages Header File

	THIS FILE IS TO BE PORTED BY THE IMPLEMENTOR.

	File:			if7_smtmsgim.h
	Created:		12/01/89

	Version:		if7_v2.2.1		Mon Mar  1 16:49:14 1993
	Last Modified:	cset.0009		10/23/92
	
	Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED
	
	This header file is used by the implementor to provide additional
	message types and structures as required for implementor provided
	features.

	Modification History:
	*** SMT 7.2 ***
		Based on 6.2.3 multistation code.
*********************************************************************/


/*
*	Add any additional message types here.
*/

/*********************************************************************
	Implementor Defined Message Types
*********************************************************************/

/* change state of frame trace in FBM */
#define	FBM_ACTION_FRAME_TRACE			(ACTION_MSG | FBM_MSG | 0xA1)

/* change state of PMF authorization in FBM */
#define	FBM_ACTION_PMF_AUTHORIZATION	(ACTION_MSG | FBM_MSG | 0xA2)

/* notify MAP of a received frame during tracing */
#define	FBM_EVENT_FRAME_TRACE			(EVENT_MSG | FBM_MSG | 0xA1)

/*
*       Customization to add Cisco required capabilities
*/

/* change fddiMACSMTAddress and its copies */
#define MIB_ACTION_CHANGE_MACADDR		(ACTION_MSG | MIB_MSG | 0xA0)

/*
@(#) if7_mib.h if7_v2.2.1:cset.0009:2:2 Mon Mar  1 16:49:14 1993 Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED
*/
/*********************************************************************
	Management Information Base Header File

	MIB attribute and structure definition.

	File:			if7_mib.h
	Created:		12/01/89

	Version:		if7_v2.2.1		Mon Mar  1 16:49:14 1993
	Last Modified:	cset.0009		10/23/92

	Copyright 1990-1993 XLNT Designs, Inc., ALL RIGHTS RESERVED

	This file contains types and constants required for accessing
	the Management Information Base (MIB).  The MIB is accessed thru
	get, set, add and change routines.  It contains or has access to all
	SMT attributes within a particular station.

	Modification History:
	*** SMT 7.2 ***
		Based on 6.2.3 multistation code.
*********************************************************************/

/*
#include	"mibdefs.h"
#include	"mibvend.h"
#include	"mibtypes.h"
*/


#endif __IF_XDI_H__
