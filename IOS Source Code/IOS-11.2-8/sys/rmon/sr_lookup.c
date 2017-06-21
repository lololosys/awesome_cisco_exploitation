/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_lookup.c,v 3.2 1995/11/17 18:39:37 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rmon/sr_lookup.c,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_lookup.c,v $
 * Revision 3.2  1995/11/17  18:39:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 * Copyright (C) 1994 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 52.227-7013
 *  and in similar clauses in the FAR and NASA FAR Supplement.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

#include "master.h"
#include "../snmp/snmp_api.h"
#include "sr_lookup.h"

#define CmpInts(a,b) ((a)<(b)?-1:((a)>(b)))

/*
  CmpIndices

  Return Values:

  -1 : Index is less than indicated table item
   0 : Index is equal to indicated table item
   1 : Index is greater than indicated table item
 */
static int
CmpIndices(
    anytype_t      *T_cur,
    Index_t        *I,
    int             Isize)
{
    int             cmp = 0, i;

    for (i = 0; i < Isize; i++) {
	if (I[i].type == T_uint) {
	    cmp = CmpInts(I[i].value.uint_val, T_cur[I[i].offset].uint_val);
	}
	else if (I[i].type == T_octet) {
	    cmp = CmpOctetStrings(I[i].value.octet_val,
				  T_cur[I[i].offset].octet_val);
	}
	else if (I[i].type == T_oid) {
	    cmp = CmpOID(I[i].value.oid_val,
			 T_cur[I[i].offset].oid_val);
	}

#ifdef SR_FTL
	else if (I[i].type == T_ieeeMacAddr) {
	    cmp = CmpIEEEMacAddrs(I[i].value.octet_val,
				  T_cur[I[i].offset].octet_val);
	}
#endif				/* SR_FTL */

	if (cmp != 0) {
	    break;
	}
    }
    return cmp;
}

static void
InstallIndices(
    anytype_t      *New,
    Index_t        *I,
    int             Isize)
{
    int             i;

    /* do nothing if the index type is unknown */
    for (i = 0; i < Isize; i++) {
	if (I[i].type == T_uint) {
	    New[I[i].offset].uint_val = I[i].value.uint_val;
	}
	else if (I[i].type == T_octet) {
	    New[I[i].offset].octet_val = CloneOctetString(I[i].value.octet_val);
	}
	else if (I[i].type == T_oid) {
	    New[I[i].offset].oid_val = CloneOID(I[i].value.oid_val);
	}
    }
}

/*
  TableInsert

  Return Values:

  0..(Tsize-1) : indicates index of existing or newly inserted table item
       -1      : indicates error during insertion
 */
#define FNAME "TableInsert"
int
TableInsert(pT, pTsize, I, Isize, RowSize)
    anytype_t    ***pT;
    int            *pTsize;
    Index_t        *I;
    int             Isize;
    int             RowSize;
{
    int             pos;
    anytype_t     **NewTab = NULL;
    anytype_t      *NewRow = NULL;

    if (*pTsize < 0) {
	*pTsize = 0;
    }
    if ((pos = TableLookup(*pT, *pTsize, I, Isize, NEXT)) == -1) {
	pos = *pTsize;
    }
    else if (CmpIndices((*pT)[pos], I, Isize) == 0) {
	return pos;
    }
    if ((NewTab = (anytype_t **)
	 malloc((unsigned) ((*pTsize + 1) * sizeof(anytype_t *)))) == NULL) {
	goto fail;
    }
    if ((NewRow = (anytype_t *)
	 malloc((unsigned) RowSize)) == NULL) {
	goto fail;
    }
    memset(NewRow, 0, RowSize);
    InstallIndices(NewRow, I, Isize);

    memcpy(NewTab, *pT, pos * sizeof(anytype_t *));
    NewTab[pos] = NewRow;
    memcpy(&(NewTab[pos + 1]), &((*pT)[pos]),
	   (*pTsize - pos) * sizeof(anytype_t *));

    free((char *) *pT);

    *pT = NewTab;
    (*pTsize)++;

    return pos;
fail:
    DPRINTF((APERROR, "%s: malloc error adding row\n", FNAME));
    if (NewTab != NULL) {
	free((char *) NewTab);
    }
    if (NewRow != NULL) {
	free((char *) NewRow);
    }
    return -1;
}

#undef FNAME

/*
  TableLookup

  Return Values:

  0..(Tsize-1) : indicates index of first table item which is >= to index
       -1      : indicates there is no table item >= index
 */
int
TableLookup(T, Tsize, I, Isize, searchType)
    anytype_t     **T;
    int             Tsize;
    Index_t        *I;
    int             Isize;
    int             searchType;
{
    int             low, high, mid = 0;
    int             cmp = -1;

    low = 0;
    high = Tsize - 1;
    while (low <= high) {
	mid = (low + high) / 2;

	cmp = CmpIndices(T[mid], I, Isize);

	if (cmp < 0) {
	    high = mid - 1;
	}
	else if (cmp > 0) {
	    low = mid + 1;
	}
	else {
	    break;
	}
    }
    if (searchType == EXACT && cmp != 0) {
	return -1;
    }

    if (cmp > 0) {
	mid++;
    }

    if (mid < Tsize) {
	return mid;
    }
    return -1;
}
      
