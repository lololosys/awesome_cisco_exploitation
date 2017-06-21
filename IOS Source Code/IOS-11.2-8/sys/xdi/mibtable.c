/* $Id: mibtable.c,v 3.2 1995/11/17 18:10:56 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/mibtable.c,v $
 *------------------------------------------------------------------
 *
 *
 * Mar 1994, Walter Friedrich
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: mibtable.c,v $
 * Revision 3.2  1995/11/17  18:10:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:56:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:25:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_mibtable.c xm7_v3.1.1:cset.0166:8:8 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Management Services Process Module

		MSP MIB Resource Table Routines

		File:			xm7_mibtable.c
		Created:		04/02/92

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94

		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This file contains routines for initializing and managing the
		resource ID tables in the MIB. The resource ID table is a sorted
		list of IDs for MACs, PATHS, or PORTs. Each object has its own
		resource ID table.

		An element in the resource ID table consists of the resource ID
		and an index. The index is the array element in the MIB data
		array where the object's attributes are stored.

		Since objects are now referred to by their IDs, a searching
		algorithm is needed to map the IDs to their position in the
		data array. A binary search is used here because it is fast
		and compact for searches of this size (1-255 elements).

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

#include        "cschdrsif.h"
#include        "xdihdrsif.h"

#include	"fddihdr.h"
#include	"smterror.h"
#include	"smtmsg.h"
#include	"mibglbl.h"
#include	"mibhdr.h"
#include	"mibproto.h"
#include	"multistn.h"


/*********************************************************************
*
*	MIB Table Routines
*
*********************************************************************/

uInt32
#ifdef __STDC__
InitMIBTables (uInt16 stnID)
#else
InitMIBTables (stnID)
		uInt16           stnID;
#endif
/*********************************************************************
Function:	This routine is called during initialization processing
			to clear the MIB resource tables.
Parameters:	stnID	= identifier of station to initialize.
Input:		None.
Output:		All resource tables are cleared and all object data
			table entries are open.
Return:		0		if successful.
Modification History:
*********************************************************************/
{
Int16            i;						/* loop counter */
MIBType         *mibPtr;				/* MIB to use */

	/* get MIB */
	mibPtr = MIBDATA (stnID, mib);

	/* clear MAC tables */
	for (i = 0; i < MAX_MAC_COUNT; i++)
	{
		mibPtr->macTable[i].resourceID = 0;
		mibPtr->macTable[i].index = 0;
		mibPtr->macBase[i].inUse = FALSE;
	}

	/* clear PORT tables */
	for (i = 0; i < MAX_PORT_COUNT; i++)
	{
		mibPtr->portTable[i].resourceID = 0;
		mibPtr->portTable[i].index = 0;
		mibPtr->portBase[i].inUse = FALSE;
	}

	return (0);
}

Int16
#ifdef __STDC__
SearchResourceID (uInt16 rsrcID, ResourceTableType * table,
	uChar count, uInt16 op)
#else
SearchResourceID (rsrcID, table, count, op)
		uInt16           rsrcID;
		ResourceTableType table[];
		uChar            count;
		uInt16           op;
#endif
/*********************************************************************
Function:	Search for a resource ID in a resource table. Upon
			completion of the search, perform the operation specified
			by op. This will be either SEARCH_FIND, SEARCH_ADD or
			SEARCH_DELETE.
Parameters:	rsrcID	= object's resource ID to find.
			table	= resource ID table to use.
			count	= # of entries in the table.
			op		= operation to perform on completion of search.
Input:		table	= sorted list of resource IDs.
Output:		Depends upon operation:
			SEARCH_FIND	= No change to table.
			SEARCH_ADD	= Inserts ID into list.
			SEARCH_DELETE	= Deletes ID from list.
Return:		Index of table element if found.
			-1 if not found.
Notes:		When adding, the calling process must ensure that enough
			space is available in the table.
Modification History:
*********************************************************************/
{
Int16            lo,
                 hi,
                 mid;					/* binary search indexes */
uInt16           val;					/* key (rsrcID) value in table */

	lo = 0;
	hi = count - 1;
	/**
	 * WRF
	 * Initializing mid to avoid compiler warning.
	 **/
	mid = 0;

	while (lo <= hi)
	{
		mid = (lo + hi) >> 1;
		val = table[mid].resourceID;
		if (val == rsrcID)
			break;
		if (rsrcID > val)
			lo = mid + 1;
		else
			hi = mid - 1;
	}

	switch (op)
	{
	case SEARCH_FIND:
		if (lo > hi)
			mid = -1;
		break;

	case SEARCH_ADD:
		if (lo <= hi)
			/* key already exists */
			break;

		/*
		 * Perform insertion of new key. As a side effect of the binary
		 * search, lo happens to be the index where the new key is to be
		 * inserted. This copy shifts the elements upward to open a space in
		 * the table.
		 * 
		 * Copy each table entry to open a space in the table to avoid
		 * overlapping memory copy. The for() loop moves from the next open
		 * entry (count) to one short of the entry where the new item is to
		 * be added (lo).
		 */
		for (hi = count; hi > lo; hi--)
			MEMCOPY ((uChar *) (&table[hi]),
				(uChar *) (&table[hi - 1]),
				sizeof (ResourceTableType));

		/* install new key and set return value */
		table[lo].resourceID = rsrcID;
		table[lo].index = 0;
		mid = lo;

		break;

	case SEARCH_DELETE:
		if (lo > hi)
		{
			/* key not found */
			mid = -1;
			break;
		}

		/*
		 * Preform deletion of key. See SEARCH_ADD for description of copy
		 * function.
		 * 
		 * Copy each table entry to close the space in the table to avoid
		 * overlapping memory copy. The for() loop moves from the entry to
		 * delete (mid) to one short of the end of the table (count). Also
		 * added code to clear previous last entry in the table.
		 */
		for (hi = mid; hi < count; hi++)
			MEMCOPY ((uChar *) (&table[hi]),
				(uChar *) (&table[hi + 1]),
				sizeof (ResourceTableType));

		table[count - 1].resourceID = 0;
		table[count - 1].index = 0;

		break;
	}

	return (mid);
}
