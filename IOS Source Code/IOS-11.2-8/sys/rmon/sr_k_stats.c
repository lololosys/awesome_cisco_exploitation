/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_k_stats.c,v 3.4.12.1 1996/05/17 21:26:33 anke Exp $
 * $Source: /release/112/cvs/Xsys/rmon/sr_k_stats.c,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_k_stats.c,v $
 * Revision 3.4.12.1  1996/05/17  21:26:33  anke
 * CSCdi48524:  Need mechanism to display RMON tables
 *              Fill in guts of show rmon commands
 * Branch: California_branch
 *
 * Revision 3.4  1996/02/19  06:06:04  jjohnson
 * CSCdi48853:  rmon can consume all available memory
 * As an interim measure, don't allow rmon to consume memory once the
 * heap's low-water mark has been reached.  While we're at it, name
 * all the malloc'ed memory so that we can tell what is used where
 *
 * Revision 3.3  1996/02/09  07:57:29  jjohnson
 * CSCdi48524:  Need mechanism to display RMON tables
 * parse chains are in place.  checkpoint work so that anke can take over.
 *
 * Revision 3.2  1995/11/17  18:39:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:21  shaker
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
#include "rmon_util.h"
#include "logger.h"
#include "../os/free.h"

/* global data describing the etherStatsTable entries */
static SnmpType        etherStatsEntryTypeTable[] = {
    {INTEGER_TYPE, SR_READ_ONLY, 0},	/* etherStatsIndex */
    {OBJECT_ID_TYPE, SR_READ_WRITE, -1},	/* etherStatsDataSource */
    {COUNTER_TYPE, SR_READ_ONLY, -1},	/* etherStatsDropEvents */
    {COUNTER_TYPE, SR_READ_ONLY, -1},	/* etherStatsOctets */
    {COUNTER_TYPE, SR_READ_ONLY, -1},	/* etherStatsPkts */
    {COUNTER_TYPE, SR_READ_ONLY, -1},	/* etherStatsBroadcastPkts */
    {COUNTER_TYPE, SR_READ_ONLY, -1},	/* etherStatsMulticastPkts */
    {COUNTER_TYPE, SR_READ_ONLY, -1},	/* etherStatsCRCAlignErrors */
    {COUNTER_TYPE, SR_READ_ONLY, -1},	/* etherStatsUndersizePkts */
    {COUNTER_TYPE, SR_READ_ONLY, -1},	/* etherStatsOversizePkts */
    {COUNTER_TYPE, SR_READ_ONLY, -1},	/* etherStatsFragments */
    {COUNTER_TYPE, SR_READ_ONLY, -1},	/* etherStatsJabbers */
    {COUNTER_TYPE, SR_READ_ONLY, -1},	/* etherStatsCollisions */
    {COUNTER_TYPE, SR_READ_ONLY, -1},	/* etherStatsPkts64Octets */
    {COUNTER_TYPE, SR_READ_ONLY, -1},	/* etherStatsPkts65to127Octets */
    {COUNTER_TYPE, SR_READ_ONLY, -1},	/* etherStatsPkts128to255Octets */
    {COUNTER_TYPE, SR_READ_ONLY, -1},	/* etherStatsPkts256to511Octets */
    {COUNTER_TYPE, SR_READ_ONLY, -1},	/* etherStatsPkts512to1023Octets */
    {COUNTER_TYPE, SR_READ_ONLY, -1},	/* etherStatsPkts1024to1518Octets */
    {OCTET_PRIM_TYPE, SR_READ_WRITE, -1},	/* etherStatsOwner */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* etherStatsStatus */
};

static Index_t         etherStatsEntryIndex[1];
static RmonTable       etherStatsTable;

void DeleteetherStatsTableEntry SR_PROTOTYPE((int index));

/* this routine is not generated by the mib compiler.  It is responsible for
 * updating the statistics for appropriate control table entries. */
void
UpdatePacketStatistics(psp)
    PacketState    *psp;
{
    int             i;
    etherStatsEntry_t *esp;

    /* look for etherstats table entries with this data source */
    for (i = 0; i < etherStatsTable.nitems; i++, esp++) {
	esp = ((etherStatsEntry_t **) etherStatsTable.tp)[i];
	if (esp->etherStatsStatus == D_etherStatsStatus_valid
	    && CmpDataSource(esp->etherStatsDataSource, psp->source) == 0) {
	    /* update non-packet stats */
	    esp->etherStatsCollisions += psp->collisions;
	    esp->etherStatsDropEvents += psp->drops;
	    if (psp->size == 0)
		continue;

	    /* update statistics */
	    if (psp->error != 0) {
		/* crc or alignment error */
		if (psp->size < 64)
		    esp->etherStatsFragments++;
		else if (psp->size > 1516)
		    esp->etherStatsJabbers++;
		else
		    esp->etherStatsCRCAlignErrors++;
	    }
	    else {
		/* no errors in the packet */
		if (psp->size < 64)
		    esp->etherStatsUndersizePkts++;
		else if (psp->size > 1518)
		    esp->etherStatsOversizePkts++;
		else {
		    /* check for destination */
		    if (IsBroadcast(psp->p))
			esp->etherStatsBroadcastPkts++;
		    else if (*psp->p & 0x01)
			esp->etherStatsMulticastPkts++;

		    /* check sizes */
		    if (psp->size == 64)
			esp->etherStatsPkts64Octets++;
		    else if (psp->size > 64 && psp->size <= 127)
			esp->etherStatsPkts65to127Octets++;
		    else if (psp->size >= 128 && psp->size <= 255)
			esp->etherStatsPkts128to255Octets++;
		    else if (psp->size >= 256 && psp->size <= 511)
			esp->etherStatsPkts256to511Octets++;
		    else if (psp->size >= 512 && psp->size <= 1023)
			esp->etherStatsPkts512to1023Octets++;
		    else if (psp->size >= 1024 && psp->size <= 1518)
			esp->etherStatsPkts1024to1518Octets++;
		}
	    }

	    /* update all-packet statistics */
	    esp->etherStatsPkts++;
	    esp->etherStatsOctets += psp->size;
	}
    }
}

/* --------------------- MIB COMPILER GENERATED ------------------------- */

int
k_etherStatsEntry_initialize()
{
    etherStatsEntryIndex[0].offset = I_etherStatsIndex;
    etherStatsEntryIndex[0].type = T_uint;

    etherStatsTable.nindices = 1;
    etherStatsTable.tip = etherStatsEntryIndex;
    etherStatsTable.nitems = 0;
    etherStatsTable.rowsize = sizeof(etherStatsEntry_t);
    if ((etherStatsTable.tp = (void **) malloc(sizeof(etherStatsEntry_t *))) == NULL) {
	return 0;
    }

    return 1;
}

int
k_etherStatsEntry_terminate()
{
    while (etherStatsTable.nitems > 0) {
	DeleteetherStatsTableEntry(0);
    }

    free(etherStatsTable.tp);
    return 1;
}

/* This routine is called by the timeout code to
 * delete a pending creation of a Table entry */
void
etherStatsTableDeleteCallback(tdp)
    TimeOutDescriptor *tdp;
{
    etherStatsEntry_t *data;

    /* dummy up an entry to delete */
    data = (etherStatsEntry_t *) tdp->UserData2;
    data->etherStatsStatus = D_etherStatsStatus_invalid;
    data->TimerId = -1;

    /* free the timeout descriptor */
    free(tdp);

    /* call the set method */
    k_etherStatsEntry_set(data, (ContextInfo *) NULL, 0);
}

/* This routine deletes an entry from the etherStatsTable */
void
DeleteetherStatsTableEntry(index)
    int             index;
{
    etherStatsEntry_t *data;

    /* get a pointer to the old entry */
    data = (etherStatsEntry_t *) etherStatsTable.tp[index];

    /* free the old entry and remove it from the table */
    etherStatsEntry_free(data);
    RemoveTableEntry(&etherStatsTable, index);
}

/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_etherStatsEntryFreeUserpartData(data)
    etherStatsEntry_t *data;
{
    /* nothing to free by default */
}

etherStatsEntry_t *
k_etherStatsEntry_get(serialNum, contextInfo, nominator, searchType, etherStatsIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            etherStatsIndex;
{
    int             index;

    etherStatsTable.tip[0].value.uint_val = etherStatsIndex;
    if ((index = SearchTable(&etherStatsTable, searchType)) == -1) {
	return NULL;
    }

    return (etherStatsEntry_t *) etherStatsTable.tp[index];

}

#ifdef SETS
int
k_etherStatsEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{
#ifdef SR_SNMPv2
    etherStatsEntry_t *undodata = (etherStatsEntry_t *) dp->undodata;
#else				/* SR_SNMPv2 */
    etherStatsEntry_t *undodata = NULL;
#endif				/* SR_SNMPv2 */
    int             error_status = NO_ERROR;

    /*
     * make sure that the data source isn't being changed if this is a valid
     * entry
     */
    if (undodata != 0 && undodata->etherStatsStatus == D_etherStatsStatus_valid) {
	switch (object->nominator) {
	  case I_etherStatsDataSource:
	    if (CmpOID(value->oid_value, undodata->etherStatsDataSource) != 0) {
		error_status = INCONSISTENT_VALUE_ERROR;
	    }
	    break;
	}
    }

    return error_status;
}

int
k_etherStatsEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    etherStatsEntry_t *data = (etherStatsEntry_t *) dp->data;

    if (data->etherStatsStatus == D_etherStatsStatus_invalid) {
	dp->state = DELETE;
    }
    else {
	dp->state = ADD_MODIFY;

	/* if setting to valid, make sure we have all the information we need */
	if (data->etherStatsStatus == D_etherStatsStatus_valid) {
	    if (!VALID(I_etherStatsDataSource, data->valid)
	      || CheckEthernetDataSource(data->etherStatsDataSource) == 0) {
		/* can't do it, don't have a valid data source */
		dp->state = UNKNOWN;
	    }
	}
    }

    return NO_ERROR;
}

int
k_etherStatsEntry_set_defaults(dp)
    doList_t       *dp;
{

    etherStatsEntry_t *data = (etherStatsEntry_t *) dp->data;

    /*
     * if we're low on memory, don't allow the entry to be created
     */
    if (HeapLowMem())
	return RESOURCE_UNAVAILABLE_ERROR;

    if ((data->etherStatsDataSource = MakeOIDFromDot("0.0")) == 0) {
	return RESOURCE_UNAVAILABLE_ERROR;
    }

    if ((data->etherStatsOwner = MakeOctetStringFromText("")) == 0) {
	return RESOURCE_UNAVAILABLE_ERROR;
    }

    data->etherStatsStatus = D_etherStatsStatus_createRequest;
    data->TimerId = -1;

    SET_ALL_VALID(data->valid);

    return NO_ERROR;
}

int
k_etherStatsEntry_set(data, contextInfo, function)
    etherStatsEntry_t *data;
    ContextInfo    *contextInfo;
    int             function;
{

    int             index;
    etherStatsEntry_t *new = NULL;

    /* find this entry in the table */
    etherStatsTable.tip[0].value.uint_val = data->etherStatsIndex;
    if ((index = SearchTable(&etherStatsTable, EXACT)) != -1) {
	new = (etherStatsEntry_t *) etherStatsTable.tp[index];
    }

    /* perform the table entry operation on it */
    if (data->etherStatsStatus == D_etherStatsStatus_invalid) {
	if (data->TimerId != -1) {
	    CancelEntryStatusTimeout((int) data->TimerId);
	}

	if (index == -1) {
	    return NO_ERROR;
	}
	else {
	    /* delete the table entry */
	    DeleteetherStatsTableEntry(index);
	    return NO_ERROR;
	}
    }
    else if (index == -1) {
	/* add the entry */
	etherStatsTable.tip[0].value.uint_val = data->etherStatsIndex;
	if ((index = NewTableEntry(&etherStatsTable)) == -1) {
	    return GEN_ERROR;
	}
	new = (etherStatsEntry_t *) etherStatsTable.tp[index];
	name_memory(new, "RMON Stat Data");

	/* set a timeout */
	if ((new->TimerId = SetEntryStatusTimeout(MAX_ROW_CREATION_TIME, (void *) new, etherStatsTableDeleteCallback)) == -1) {
	    DeleteetherStatsTableEntry(index);
	    return GEN_ERROR;
	}
    }

    TransferEntries(I_etherStatsStatus, etherStatsEntryTypeTable,
		    (unsigned long *)new, (unsigned long *)data);
    SET_ALL_VALID(new->valid);

    /* if the new row entry is now valid, cancel the creation timeout */
    if (new->etherStatsStatus == D_etherStatsStatus_valid && new->TimerId != -1) {
	CancelEntryStatusTimeout((int) new->TimerId);
	new->TimerId = -1;
    }
    else {
	if (new->etherStatsStatus == D_etherStatsStatus_createRequest) {
	    new->etherStatsStatus = D_etherStatsStatus_underCreation;
	}
    }

    return NO_ERROR;
}

#endif				/* SETS */

#ifdef SR_SNMPv2
int
etherStatsEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    etherStatsEntry_t *data = (etherStatsEntry_t *) doCur->data;

#ifdef SR_SNMPv2
    etherStatsEntry_t *undodata = (etherStatsEntry_t *) doCur->undodata;
#else				/* SR_SNMPv2 */
    etherStatsEntry_t *undodata = NULL;
#endif				/* SR_SNMPv2 */
    etherStatsEntry_t *setdata = NULL;
    int             function;

    /* we are either trying to undo an add, a delete, or a modify */
    if (undodata == NULL) {
	/* undoing an add, so delete */
	data->etherStatsStatus = D_etherStatsStatus_invalid;
	setdata = data;
	function = DELETE;
    }
    else {
	/* undoing a delete or modify, replace the original data */
	if (undodata->etherStatsStatus == D_etherStatsStatus_underCreation) {
	    undodata->etherStatsStatus = D_etherStatsStatus_createRequest;
	}
	setdata = undodata;
	function = ADD_MODIFY;
    }

    /* use the set method for the undo */
    if ((setdata != NULL) && k_etherStatsEntry_set(setdata, contextInfo, function) == NO_ERROR) {
	return NO_ERROR;
    }

    return UNDO_FAILED_ERROR;
}

#endif				/* SR_SNMPv2 */

/******************** cisco supplied routines follow ********************/

/*
 * show rmon statistics
 * output a string similiar to the following for every ethernet interface:
 *    Interface 12345 is valid, and owned by foo
 *      Monitors oid.oid.oid.oid.oid.oid which has
 *      Received xxxxxx octets, xxxxxx packets,
 *      xxxxx broadcast and xxxxx multicast packets,
 *      xxxxx undersized and xxxxxx oversized packets,
 *      xxxxx fragments and xxxxxx jabbers,
 *      xxxxxx CRC alignment errors and xxxxxx collisions.
 *      # of dropped packet events is (due to lack of resources) xxxxxx
 *      # of packets received of length (in octets):
 *      64: xxxxxx, 65-127: xxxxxx, 128-255: xxxxxx, 
 *      256-511: xxxxxx, 512-1023: xxxxxx, 1024-1518: xxxxxx
 */

void
show_rmon_etherstats (void)
{
    etherStatsEntry_t   *etherStatsp;
    int             i, status;
    OctetString    *ownerOS;
    char	   *owner;
    char           *object;

    automore_enable(NULL);
    if (etherStatsTable.nitems == 0) {
	printf("\nEthernet Statistics table is empty");
	automore_disable();
	return;
    }

    for (i = 0; i < etherStatsTable.nitems; i++) {
        etherStatsp = etherStatsTable.tp[i];
	mem_lock(etherStatsp);

	/* turn the owner string from an octet string to a real string */
	owner = malloc(etherStatsp->etherStatsOwner->length + 1);
	if (owner) {
	    ownerOS = etherStatsp->etherStatsOwner;
	    memcpy(owner, ownerOS->octet_ptr, ownerOS->length);
	}
	else
	    return;

	/* get a textual representation of the object */
	object = malloc(MAX_OCTET_STRING_SIZE);
	if (object) {
	    status = MakeDotFromOID(etherStatsp->etherStatsDataSource, object);
	    if (status == -1) {
		free(object);
		object = NULL;
	    }
	}

        printf("\nInterface %d is %s, and owned by %s",
	       etherStatsp->etherStatsIndex,
	       rmon_status_string(etherStatsp->etherStatsStatus),
	       owner ? owner : "unknown");
        printf("\n Monitors %s which has", object ? object : "unknown");
        printf("\n Received %d octets, %d packets,",
	       etherStatsp->etherStatsOctets, etherStatsp->etherStatsPkts); 
        printf("\n %d broadcast and %d multicast packets,",
	       etherStatsp->etherStatsBroadcastPkts,
	       etherStatsp->etherStatsMulticastPkts); 
        printf("\n %d undersized and %d oversized packets,",
	       etherStatsp->etherStatsUndersizePkts,
	       etherStatsp->etherStatsOversizePkts); 
        printf("\n %d fragments and %d jabbers,",
	       etherStatsp->etherStatsFragments,
	       etherStatsp->etherStatsJabbers); 
        printf("\n %d CRC alignment errors and %d collisions.",
	       etherStatsp->etherStatsCRCAlignErrors,
	       etherStatsp->etherStatsCollisions);
        printf("\n # of dropped packet events (due to lack of resources): %d", 
	       etherStatsp->etherStatsDropEvents);
        printf("\n # of packets received of length (in octets):");
        printf("\n  64: %d, 65-127: %d, 128-255: %d,",
	       etherStatsp->etherStatsPkts64Octets,
	       etherStatsp->etherStatsPkts65to127Octets,
	       etherStatsp->etherStatsPkts128to255Octets);
	printf("\n  256-511: %d, 512-1023: %d, 1024-1518:%d",
	       etherStatsp->etherStatsPkts256to511Octets,
	       etherStatsp->etherStatsPkts512to1023Octets,
	       etherStatsp->etherStatsPkts1024to1518Octets);
	free(owner);
	free(object);
	free(etherStatsp);
    }
    automore_disable();
}
