/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_table.c,v 3.2 1995/11/17 18:39:55 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rmon/sr_table.c,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_table.c,v $
 * Revision 3.2  1995/11/17  18:39:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:38  shaker
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
#include "sr_rmonmib.h"

#define exit(val)

/* this routine compares values to see if they are equal */
int
CmpValues(nominator, typep, olddata, newval)
    int             nominator;
    SnmpType       *typep;
    unsigned long  *olddata;
    unsigned long   newval;
{
    int             retval;

    /* compare based on type */
    switch (typep[nominator].type) {
	/* signed 32-bit values */
      case INTEGER_TYPE:
	if ((long) olddata[nominator] > (long) newval) {
	    retval = 1;
	}
	else if ((long) olddata[nominator] < (long) newval) {
	    retval = (-1);
	}
	else {
	    retval = 0;
	}
	break;

	/* unsigned 32-bit values */
      case COUNTER_TYPE:
      case GAUGE_TYPE:
      case TIME_TICKS_TYPE:

#ifdef SR_SNMPv2
      case U_INTEGER_32_TYPE:
#endif				/* SR_SNMPv2 */

	if (olddata[nominator] > newval) {
	    retval = 1;
	}
	else if (olddata[nominator] < newval) {
	    retval = (-1);
	}
	else {
	    retval = 0;
	}
	break;

	/* compound values */
      case OCTET_PRIM_TYPE:
	retval = CmpOctetStrings((OctetString *) olddata[nominator],
				 (OctetString *) newval);
	break;
      case OBJECT_ID_TYPE:
	retval = CmpOID((OID *) olddata[nominator], (OID *) newval);
	break;

      default:
	DPRINTF((APERROR, "CmpValues: can't handle type %d\n",
		 typep[nominator].type));
	exit(1);
	retval = 0;
    }
    return (retval);
}

/* this routine writes a new value into a data structure */
int
CopyValues(nominator, typep, new, old)
    int             nominator;
    SnmpType       *typep;
    unsigned long  *new;
    unsigned long  *old;
{
    int             retval = 1;
    OctetString    *newosp, *oldosp;
    OID            *newoidp, *oldoidp;

    /* compare based on type */
    switch (typep[nominator].type) {
	/* 32-bit values */
      case INTEGER_TYPE:
      case COUNTER_TYPE:
      case GAUGE_TYPE:
      case TIME_TICKS_TYPE:

#ifdef SR_SNMPv2
      case U_INTEGER_32_TYPE:
#endif				/* SR_SNMPv2 */

	new[nominator] = old[nominator];
	break;

	/* compound values */
      case OCTET_PRIM_TYPE:
	newosp = (OctetString *) new[nominator];
	oldosp = (OctetString *) old[nominator];
	if (newosp == 0 || CmpOctetStrings(newosp, oldosp) != 0) {
	    FreeOctetString(newosp);
	    if ((newosp = CloneOctetString(oldosp)) != 0) {
		new[nominator] = (unsigned long) newosp;
	    }
	    else {
		retval = 0;
	    }
	}
	break;
      case OBJECT_ID_TYPE:
	newoidp = (OID *) new[nominator];
	oldoidp = (OID *) old[nominator];
	if (newoidp == 0 || CmpOID(newoidp, oldoidp) != 0) {
	    FreeOID(newoidp);
	    if ((newoidp = CloneOID(oldoidp)) != 0) {
		new[nominator] = (unsigned long) newoidp;
	    }
	    else {
		retval = 0;
	    }
	}
	break;

      default:
	DPRINTF((APERROR, "CopyValues: can't handle type %d\n",
		 typep[nominator].type));
	exit(1);
    }

    return (retval);
}

/* this routine moves data from the old structure to the new one, clearing
 * the pointer to aggregate structures in the old data */
void
TransferValues(nominator, typep, new, old)
    int             nominator;
    SnmpType       *typep;
    unsigned long  *new;
    unsigned long  *old;
{
    OctetString    *newosp, *oldosp;
    OID            *newoidp, *oldoidp;

    /* compare based on type */
    switch (typep[nominator].type) {
	/* 32-bit values */
      case INTEGER_TYPE:
      case COUNTER_TYPE:
      case GAUGE_TYPE:
      case TIME_TICKS_TYPE:

#ifdef SR_SNMPv2
      case U_INTEGER_32_TYPE:
#endif				/* SR_SNMPv2 */

	new[nominator] = old[nominator];
	break;

	/* compound values */
      case OCTET_PRIM_TYPE:
	newosp = (OctetString *) new[nominator];
	oldosp = (OctetString *) old[nominator];
	if (newosp == 0 || CmpOctetStrings(newosp, oldosp) != 0) {
	    FreeOctetString(newosp);
	    new[nominator] = (unsigned long) oldosp;
	    old[nominator] = 0;
	}
	break;
      case OBJECT_ID_TYPE:
	newoidp = (OID *) new[nominator];
	oldoidp = (OID *) old[nominator];
	if (newoidp == 0 || CmpOID(newoidp, oldoidp) != 0) {
	    FreeOID(newoidp);
	    new[nominator] = (unsigned long) oldoidp;
	    old[nominator] = 0;
	}
	break;

      default:
	DPRINTF((APERROR, "TransferValues: can't handle type %d\n",
		 typep[nominator].type));
	exit(1);
    }
}

/* this routine copies all entries in a structure */
int
CopyEntries(maxNominator, typep, new, old)
    int             maxNominator;
    SnmpType       *typep;
    unsigned long  *new;
    unsigned long  *old;
{
    int             i;

    /* copy all data up to and including the max nominator */
    for (i = 0; i <= maxNominator; i++) {
	if (CopyValues(i, typep, new, old) == 0)
	    break;
    }
    return (i <= maxNominator ? 0 : 1);
}

/* this routine transfers all non-index entries in a structure */
void
TransferEntries(maxNominator, typep, new, old)
    int             maxNominator;
    SnmpType       *typep;
    unsigned long  *new;
    unsigned long  *old;
{
    int             i;

    /* transfer all data up to and including the max nominator */
    for (i = 0; i <= maxNominator; i++) {
	if (typep[i].indextype == (-1)) {
	    TransferValues(i, typep, new, old);
	}
    }
}

/* calls a routine to search the table */
int
SearchTable(rtp, st)
    RmonTable      *rtp;
    int             st;
{
    return (TableLookup((anytype_t **) rtp->tp, rtp->nitems, rtp->tip, rtp->nindices, st));
}

/* this routine creates a new table entry and clears it out */
int
NewTableEntry(rtp)
    RmonTable      *rtp;
{
    return (TableInsert((anytype_t ***) & rtp->tp, &rtp->nitems, rtp->tip, rtp->nindices,
			rtp->rowsize));
}

/* dumps an rmon table */
void
PrintTable(rtp)
    RmonTable      *rtp;
{
    int             i;
    for (i = 0; i < rtp->nitems; i++) {
	printf("%08lx ", rtp->tp[i]);
    }
    printf("\n");
}

/* this routine deletes an entry from a table */
void
RemoveTableEntry(rtp, index)
    RmonTable      *rtp;
    int             index;
{
    /* make sure it's a valid index */
    if (index < rtp->nitems) {
	/* copy over the entry with the rest of the table */
	memcpy(rtp->tp + index, rtp->tp + (index + 1),
	       (rtp->nitems - (index + 1)) * sizeof(void *));
	rtp->nitems--;
    }
}
