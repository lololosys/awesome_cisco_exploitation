/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_k_host.c,v 3.4.12.1 1996/05/17 21:26:30 anke Exp $
 * $Source: /release/112/cvs/Xsys/rmon/sr_k_host.c,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_k_host.c,v $
 * Revision 3.4.12.1  1996/05/17  21:26:30  anke
 * CSCdi48524:  Need mechanism to display RMON tables
 *              Fill in guts of show rmon commands
 * Branch: California_branch
 *
 * Revision 3.4  1996/02/19  06:06:02  jjohnson
 * CSCdi48853:  rmon can consume all available memory
 * As an interim measure, don't allow rmon to consume memory once the
 * heap's low-water mark has been reached.  While we're at it, name
 * all the malloc'ed memory so that we can tell what is used where
 *
 * Revision 3.3  1996/02/09  07:57:27  jjohnson
 * CSCdi48524:  Need mechanism to display RMON tables
 * parse chains are in place.  checkpoint work so that anke can take over.
 *
 * Revision 3.2  1995/11/17  18:39:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:18  shaker
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
#include "ttysrv.h"

/* global data describing the hostControlTable entries */
static SnmpType        hostControlEntryTypeTable[] = {
    {INTEGER_TYPE, SR_READ_ONLY, 0},	/* hostControlIndex */
    {OBJECT_ID_TYPE, SR_READ_WRITE, -1},	/* hostControlDataSource */
    {INTEGER_TYPE, SR_READ_ONLY, -1},	/* hostControlTableSize */
    {TIME_TICKS_TYPE, SR_READ_ONLY, -1},	/* hostControlLastDeleteTime */
    {OCTET_PRIM_TYPE, SR_READ_WRITE, -1},	/* hostControlOwner */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* hostControlStatus */
};

static Index_t         hostControlEntryIndex[1];
RmonTable	       hostControlTable;
static unsigned long   hostControlTableId;

static Index_t         hostEntryIndex[1];	/* was an array of size 2 */

void DeletehostControlTableEntry SR_PROTOTYPE((int index));

/* this routine frees a host entry */
void
HostEntry_free(hep)
    hostEntry_t    *hep;
{
    if (hep != 0) {

#ifndef SR_FTL
	FreeOctetString(hep->hostAddress);
#endif				/* SR_FTL */

	free(hep);
    }
}

/* this routine deletes the entire host entry table associated with a
 * control table entry */
void
DeleteHostEntryTable(hcp)
    hostControlEntry_t *hcp;
{
    RmonTable      *rtp;

    /* make sure there's something to free */
    if ((rtp = hcp->rtp) != 0) {
	/* get rid of all entries */
	while (rtp->nitems != 0) {
	    HostEntry_free((hostEntry_t *) rtp->tp[0]);
	    RemoveTableEntry(rtp, 0);
	}

	/* now free the table */
	free(rtp->tp);
	free(rtp);
    }

}

/* this routine deletes the least recently accessed host table entries */
void
DeleteLRUHostEntry(hcp)
    hostControlEntry_t *hcp;
{
    int             i, minindex, hostindex;
    unsigned long   minseqno;
    hostEntry_t    *hep;
    OctetString    *savetip;

    /* scan the list to see which is the least recently accessed */
    minseqno = 0xFFFFFFFF;
    minindex = (-1);
    for (i = 0; i < hcp->hostControlTableSize; i++) {
	hep = (hostEntry_t *) hcp->htp[i];
	if (hep->seqno < minseqno) {
	    minindex = i;
	    minseqno = hep->seqno;
	}
    }

    /* make sure we got one */
    if (minindex != (-1)) {
	/* get a pointer to the entry */
	hep = (hostEntry_t *) hcp->htp[minindex];

	/* remove it from the host table */
	savetip = hcp->rtp->tip[0].value.octet_val;
	hcp->rtp->tip[0].value.octet_val = hep->hostAddress;
	hostindex = SearchTable(hcp->rtp, EXACT);
	hcp->rtp->tip[0].value.octet_val = savetip;
	if ((hostindex) == (-1)) {
	    DPRINTF((APERROR, "DeleteLRUHostEntry: can't find entry\n"));
	    return;
	}
	RemoveTableEntry(hcp->rtp, hostindex);

	/* update the control entry */
	hcp->hostControlTableSize--;
	hcp->hostControlLastDeleteTime = TimerCurTime();

	/* remove it from the host time table */
	memcpy(hcp->htp + minindex, hcp->htp + (minindex + 1),
	       ((int) hcp->hostControlTableSize - minindex) * sizeof(hostEntry_t *));

	/* free the memory */
	HostEntry_free(hep);
    }
}

/* this routine is responsible for adding new host table entries to the
 * table maintained by the host control entry.  Since the rfc says that
 * 	"The host group discovers new hosts on the network by
 *	 keeping a list of source and destination MAC Addresses seen
 *	 in good packets"
 * this routine checks the packet status and size before adding the new
 * entry.  It creates a corresponding entry in the hostTimeTable as well.
 * This routine returns a pointer to the new host entry, or 0 if it fails. */
hostEntry_t    *
AddNewHostEntry(hcp, psp)
    hostControlEntry_t *hcp;
    PacketState    *psp;
{
    hostEntry_t    *hep = 0;
    int             index, table_size;
    hostTimeEntry_t **newHostTimeTable;

    /* make sure this is a good packet */
    if (psp->error == 0 && psp->size >= 64 && psp->size <= 1518) {
	/* if we're low on memory free an old entry */
	if (HeapLowMem() != 0)
	    DeleteLRUHostEntry(hcp);

	/* attempt to allocate a new table array for the hostTimeTable */
	table_size = (hcp->hostControlTableSize + 1) *
	    sizeof(hostTimeEntry_t *);
	newHostTimeTable = malloc_named(table_size, "RMON Host Ptrs");
	if (newHostTimeTable != 0) {
	    /* try to insert the new table entry */
	    if ((index = NewTableEntry(hcp->rtp)) != -1) {
		/* get a pointer to this entry */
		hep = (hostEntry_t *) hcp->rtp->tp[index];
		name_memory(hep, "RMON Host Data");
#ifdef SR_FTL
		/* install the index value */
		hep->hostAddress = &hep->addrOS;
		hep->hostAddress->length = ETHERMACADDRLEN;
		hep->hostAddress->octet_ptr = hep->addr;
		memcpy((char *) hep->hostAddress->octet_ptr,
		       (char *) hcp->rtp->tip[0].value.octet_val->octet_ptr,
		       ETHERMACADDRLEN);
#endif				/* SR_FTL */

		/* update the new host time table */
		memcpy(newHostTimeTable, hcp->htp,
		(int) hcp->hostControlTableSize * sizeof(hostTimeEntry_t *));
		newHostTimeTable[hcp->hostControlTableSize] =
		    (hostTimeEntry_t *) hep;
		free(hcp->htp);
		hcp->htp = newHostTimeTable;

		/* initialize the entry */
		hcp->hostControlTableSize++;
		hep->hostIndex = hcp->hostControlIndex;
		hep->hostCreationOrder = hcp->hostControlTableSize;
	    }
	    else {
		/* free the new table */
		free(newHostTimeTable);
	    }
	}
    }

    /* return the new pointer */
    return (hep);
}

/* this routine resequences a host table to avoid problems with sequence
 * number wrapping before lru deletion.  This is an n-squared algorithm,
 * but it's called so infrequently that the overhead is not a problem. */
void
ResequenceHostTable(hcp)
    hostControlEntry_t *hcp;
{
    hostEntry_t    *hep, *minhep;
    unsigned long   new, seqno;
    long            i, j;

    /* go through the entire host table looking for the minimum */
    new = 0;
    for (i = 0; i < hcp->hostControlTableSize; i++) {
	/* look for a minimum greater than the new seqence we'll assign */
	seqno = 0xFFFFFFFF;
	minhep = 0;
	for (j = 0; j < hcp->hostControlTableSize; j++) {
	    hep = (hostEntry_t *) hcp->rtp->tp[j];
	    if (hep->seqno >= new && hep->seqno <= seqno) {
		/* save the new minimum */
		minhep = hep;
		seqno = minhep->seqno;
	    }
	}

	/* if we found one, update it */
	if (minhep != 0) {
	    minhep->seqno = new;
	}

	/* on to the next sequence number */
	new++;
    }

    /* reset the current seqence number for the table */
    hcp->curseqno = new;
}

/* this routine does host table processing for this packet */
void
ProcessHostEntry(psp)
    PacketState    *psp;
{
    int             hostcontrolindex, index;
    hostControlEntry_t *hcp;
    hostEntry_t    *hep;
    OctetString     os;

    /* process all host control entries */
    for (hostcontrolindex = 0; hostcontrolindex < hostControlTable.nitems;
	 hostcontrolindex++) {
	hcp = ((hostControlEntry_t **) hostControlTable.tp)[hostcontrolindex];
	if (hcp->hostControlStatus == D_hostControlStatus_valid
	    && CmpDataSource(hcp->hostControlDataSource, psp->source) == 0) {
	    /* get a pointer to the octet string used in the index */
	    hcp->rtp->tip[0].value.octet_val = &os;
	    os.length = ETHERMACADDRLEN;

	    /* look for the table entry for the source */
	    if (psp->size >= (2 * ETHERMACADDRLEN)) {
		os.octet_ptr = psp->p + ETHERMACADDRLEN;	/* source addr */
		if ((index = SearchTable(hcp->rtp, EXACT)) == -1) {
		    /* add a new entry for the source */
		    hep = AddNewHostEntry(hcp, psp);
		}
		else {
		    /* get a pointer to the existing entry */
		    hep = (hostEntry_t *) hcp->rtp->tp[index];
		}
	    }
	    else {
		/* not a valid source address */
		hep = 0;
	    }

	    /* update source stats only if we have an entry pointer */
	    if (hep != 0) {
		hep->hostOutPkts++;
		hep->hostOutOctets += psp->size;
		if (psp->error != 0)
		    hep->hostOutErrors++;
		if (IsBroadcast(psp->p))
		    hep->hostOutBroadcastPkts++;
		else if (*psp->p & 0x01)
		    hep->hostOutMulticastPkts++;

		/* update sequence counter for lru processing */
		hep->seqno = hcp->curseqno++;
	    }

	    /* look for the table entry for the destination */
	    if (psp->size >= ETHERMACADDRLEN) {
		os.octet_ptr = psp->p;	/* dest addr */
		if ((index = SearchTable(hcp->rtp, EXACT)) == -1) {
		    /* add a new entry for the destination */
		    hep = AddNewHostEntry(hcp, psp);
		}
		else {
		    /* get a pointer to the existing entry */
		    hep = (hostEntry_t *) hcp->rtp->tp[index];
		}
	    }
	    else {
		/* not a valid destination address */
		hep = 0;
	    }

	    /* update destination stats only if we have an entry pointer */
	    if (hep != 0) {
		hep->hostInPkts++;
		hep->hostInOctets += psp->size;

		/* update sequence counter for lru processing */
		hep->seqno = hcp->curseqno++;
	    }

	    /* resequence the table if necessary to prevent wrapping */
	    if (hcp->curseqno >= 0xFFFFFF00)
		ResequenceHostTable(hcp);
	}
    }
}

/* --------------------- MIB COMPILER GENERATED ------------------------- */

int
k_hostControlEntry_initialize()
{
    hostControlEntryIndex[0].offset = I_hostControlIndex;
    hostControlEntryIndex[0].type = T_uint;

    hostControlTable.nindices = 1;
    hostControlTable.tip = hostControlEntryIndex;
    hostControlTable.nitems = 0;
    hostControlTable.rowsize = sizeof(hostControlEntry_t);
    if ((hostControlTable.tp = (void **) malloc(sizeof(hostControlEntry_t *))) == NULL) {
	return 0;
    }

    /* init the host table index */
    hostEntryIndex[0].offset = I_hostAddress;
    hostEntryIndex[0].type = T_ieeeMacAddr;

    /* init the host table sequence id */
    hostControlTableId = 0;

    return 1;
}

int
k_hostControlEntry_terminate()
{
    while (hostControlTable.nitems > 0) {
	DeletehostControlTableEntry(0);
    }

    free(hostControlTable.tp);
    return 1;
}

/* This routine is called by the timeout code to
 * delete a pending creation of a Table entry */
void
hostControlTableDeleteCallback(tdp)
    TimeOutDescriptor *tdp;
{
    hostControlEntry_t *data;

    /* dummy up an entry to delete */
    data = (hostControlEntry_t *) tdp->UserData2;
    data->hostControlStatus = D_hostControlStatus_invalid;
    data->TimerId = -1;

    /* free the timeout descriptor */
    free(tdp);

    /* call the set method */
    k_hostControlEntry_set(data, (ContextInfo *) NULL, 0);
}

/* This routine deletes an entry from the hostControlTable */
void
DeletehostControlTableEntry(index)
    int             index;
{
    hostControlEntry_t *data;

    /* get a pointer to the old entry */
    data = (hostControlEntry_t *) hostControlTable.tp[index];

    /* free the old entry and remove it from the table */
    hostControlEntry_free(data);
    RemoveTableEntry(&hostControlTable, index);
}

/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_hostControlEntryFreeUserpartData(data)
    hostControlEntry_t *data;
{
    /* nothing to free by default */
}

hostControlEntry_t *
k_hostControlEntry_get(serialNum, contextInfo, nominator, searchType, hostControlIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            hostControlIndex;
{
    int             index;

    hostControlTable.tip[0].value.uint_val = hostControlIndex;
    if ((index = SearchTable(&hostControlTable, searchType)) == -1) {
	return NULL;
    }

    return (hostControlEntry_t *) hostControlTable.tp[index];

}

#ifdef SETS
int
k_hostControlEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

#ifdef SR_SNMPv2
    hostControlEntry_t *undodata = (hostControlEntry_t *) dp->undodata;
#else				/* SR_SNMPv2 */
    hostControlEntry_t *undodata = NULL;
#endif				/* SR_SNMPv2 */
    int             error_status = NO_ERROR;

    /*
     * make sure that the data source isn't being changed if this is a valid
     * entry
     */
    if (undodata != 0 && undodata->hostControlStatus
	== D_hostControlStatus_valid) {
	switch (object->nominator) {
	  case I_hostControlDataSource:
	    if (CmpOID(value->oid_value, undodata->hostControlDataSource) != 0) {
		error_status = INCONSISTENT_VALUE_ERROR;
	    }
	    break;
	}
    }

    return error_status;
}

int
k_hostControlEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    hostControlEntry_t *data = (hostControlEntry_t *) dp->data;

    if (data->hostControlStatus == D_hostControlStatus_invalid) {
	dp->state = DELETE;
    }
    else {
	dp->state = ADD_MODIFY;

	/* if setting to valid, make sure we have all the information we need */
	if (data->hostControlStatus == D_hostControlStatus_valid) {
	    if (!VALID(I_hostControlDataSource, data->valid)
	     || CheckEthernetDataSource(data->hostControlDataSource) == 0) {
		/* can't do it, don't have a valid data source */
		dp->state = UNKNOWN;
	    }
	}
    }

    return NO_ERROR;
}

int
k_hostControlEntry_set_defaults(dp)
    doList_t       *dp;
{

    hostControlEntry_t *data = (hostControlEntry_t *) dp->data;

    /*
     * if we're low on memory, don't allow the entry to be created
     */
    if (HeapLowMem())
	return RESOURCE_UNAVAILABLE_ERROR;

    if ((data->hostControlDataSource = MakeOIDFromDot("0.0")) == 0) {
	return RESOURCE_UNAVAILABLE_ERROR;
    }

    if ((data->hostControlOwner = MakeOctetStringFromText("")) == 0) {
	return RESOURCE_UNAVAILABLE_ERROR;
    }

    data->hostControlStatus = D_hostControlStatus_createRequest;
    data->TimerId = -1;

    /* set up private state data */
    if ((data->htp = (hostTimeEntry_t **) malloc(sizeof(hostTimeEntry_t *)))
	== NULL) {
	return RESOURCE_UNAVAILABLE_ERROR;
    }

    /* get the hostTimeTable array */
    if ((data->rtp = (RmonTable *) malloc(sizeof(RmonTable))) == 0) {
	free(data->htp);
	return RESOURCE_UNAVAILABLE_ERROR;
    }

    /* initialize the table */
    if ((data->rtp->tp = (void **) malloc(sizeof(hostEntry_t *))) == NULL) {
	free(data->htp);
	free(data->rtp);
	return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->rtp->tip = hostEntryIndex;
    data->rtp->nitems = 0;
    data->rtp->nindices = 1;
    data->rtp->rowsize = sizeof(hostEntry_t);

    SET_ALL_VALID(data->valid);

    return NO_ERROR;
}

int
k_hostControlEntry_set(data, contextInfo, function)
    hostControlEntry_t *data;
    ContextInfo    *contextInfo;
    int             function;
{

    int             index;
    hostControlEntry_t *new = NULL;

    /* find this entry in the table */
    hostControlTable.tip[0].value.uint_val = data->hostControlIndex;
    if ((index = SearchTable(&hostControlTable, EXACT)) != -1) {
	new = (hostControlEntry_t *) hostControlTable.tp[index];
    }

    /* perform the table entry operation on it */
    if (data->hostControlStatus == D_hostControlStatus_invalid) {
	if (data->TimerId != -1) {
	    CancelEntryStatusTimeout((int) data->TimerId);
	}

	if (index == -1) {
	    return NO_ERROR;
	}
	else {
	    /* get rid of the host entry table */
	    DeleteHostEntryTable(new);

	    /* get rid of the host time table */
	    if (new->htp != 0)
		free(new->htp);

	    /* delete the table entry */
	    DeletehostControlTableEntry(index);
	    return NO_ERROR;
	}
    }
    else if (index == -1) {
	/* add the entry */
	hostControlTable.tip[0].value.uint_val = data->hostControlIndex;
	if ((index = NewTableEntry(&hostControlTable)) == -1) {
	    return GEN_ERROR;
	}
	new = (hostControlEntry_t *) hostControlTable.tp[index];
	name_memory(new, "RMON Host Ctrl");

	/* set a timeout */
	if ((new->TimerId = SetEntryStatusTimeout(MAX_ROW_CREATION_TIME, (void *) new, hostControlTableDeleteCallback)) == -1) {
	    DeletehostControlTableEntry(index);
	    return GEN_ERROR;
	}
    }

    TransferEntries(I_hostControlStatus, hostControlEntryTypeTable,
		    (unsigned long *)new, (unsigned long *)data);
    SET_ALL_VALID(new->valid);

    /* transfer private state information */
    new->rtp = data->rtp;
    new->htp = data->htp;
    new->curseqno = data->curseqno;

    /* if the new row entry is now valid, cancel the creation timeout */
    if (new->hostControlStatus == D_hostControlStatus_valid && new->TimerId != -1) {
	CancelEntryStatusTimeout((int) new->TimerId);
	new->TimerId = -1;

	/* set the table id for top n processing */
	new->tableid = hostControlTableId++;
    }
    else {
	if (new->hostControlStatus == D_hostControlStatus_createRequest) {
	    new->hostControlStatus = D_hostControlStatus_underCreation;
	}
    }

    return NO_ERROR;
}

#endif				/* SETS */

#ifdef SR_SNMPv2
int
hostControlEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return UNDO_FAILED_ERROR;
}

#endif				/* SR_SNMPv2 */

hostEntry_t    *
k_hostEntry_get(serialNum, contextInfo, nominator, searchType, hostIndex, hostAddress)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            hostIndex;
    OctetString    *hostAddress;
{
    hostControlEntry_t *hcp;
    hostEntry_t    *data;
    int             index, hostindex;
    OctetString     os;
    unsigned char   macaddr[ETHERMACADDRLEN];

    /* if the address is too long, start with the next table */
    memset((char *) macaddr, 0, ETHERMACADDRLEN);
    if (hostAddress->length > ETHERMACADDRLEN) {
	hostIndex++;
	os.length = 0;
	os.octet_ptr = macaddr;
    }
    else {
	/* install the address in our scratch copy */
	os.length = hostAddress->length;
	memcpy(macaddr, hostAddress->octet_ptr, (int) hostAddress->length);
	os.octet_ptr = macaddr;
    }
    hostAddress = &os;

    /* repeat until we find an appropriate match, or there is no more data */
    data = NULL;
    do {
	/* get the host control entry */
	hostControlTable.tip[0].value.uint_val = hostIndex;
	if ((index = SearchTable(&hostControlTable, searchType)) == -1) {
	    /* no such control table */
	    break;
	}
	hcp = (hostControlEntry_t *) hostControlTable.tp[index];

	/* search the host entry table */
	hcp->rtp->tip[0].value.octet_val = hostAddress;
	if ((hostindex = SearchTable(hcp->rtp, searchType)) != -1) {
	    data = (hostEntry_t *) hcp->rtp->tp[hostindex];
	}

	/* did we find it? */
	if (data == NULL && searchType == NEXT) {
	    /* no entries, look in the next table */
	    hostIndex++;
	    os.length = 0;
	    memset((char *) macaddr, 0, ETHERMACADDRLEN);
	    os.octet_ptr = macaddr;
	    hostAddress = &os;
	}
    } while (data == NULL && searchType == NEXT);

    /* return the data */
    return (data);
}

hostTimeEntry_t *
k_hostTimeEntry_get(serialNum, contextInfo, nominator, searchType, hostTimeIndex, hostTimeCreationOrder)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            hostTimeIndex;
    long            hostTimeCreationOrder;
{
    hostControlEntry_t *hcp;
    hostTimeEntry_t *data;
    int             index;

    /* repeat until we find an appropriate match, or there is no more data */
    data = NULL;
    do {
	/* get the host control entry */
	hostControlTable.tip[0].value.uint_val = hostTimeIndex;
	if ((index = SearchTable(&hostControlTable, searchType)) == -1) {
	    /* no such control table */
	    break;
	}
	hcp = (hostControlEntry_t *) hostControlTable.tp[index];

	/* does the proper entry exist in the hostTimeTable? */
	if (hostTimeCreationOrder <= hcp->hostControlTableSize) {
	    /* yes, get a pointer to the entry */
	    data = hcp->htp[hostTimeCreationOrder - 1];
	}

	/* did we find it? */
	if (data == NULL && searchType == NEXT) {
	    /* no entries, look in the next table */
	    hostTimeIndex++;
	    hostTimeCreationOrder = 1;
	}
    } while (data == NULL && searchType == NEXT);

    /* return the data */
    return (data);
}

/******************** cisco supplied routines follow ********************/

/*
 * show rmon hosts
 * output a string similiar to the following for every host:
 *    Host 12345 is valid, and owned by foo
 *     Monitors oid.oid.oid.oid.oid.oid
 *     Table size is xxxxxx, last time an entry was deleted was xxxx
 *      Creation Order number is xxxxxx
 *       Physical address is xxxxxxxxxxxxxx
 *       Packets: rcvd xxxxxx, transmitted xxxxxx
 *       Octets: rcvd  xxxxxx, transmitted xxxxxx
 *       # of packets transmitted: broadcast xxxxxx, multicast xxxxxx 
 *       # of bad packets transmitted is xxxxxx
 */

void
show_rmon_hosts (void)
{
    hostControlEntry_t    *controlp;
    hostTimeEntry_t *timep;
    int             i, j, status;
    OctetString    *tempOS;
    char	   *owner;
    char           *object;
    unsigned char           *timeaddr;

    automore_enable(NULL);
    if (hostControlTable.nitems == 0) {
	printf("\nHost Control table is empty");
	automore_disable();
	return;
    }

    for (i = 0; i < hostControlTable.nitems; i++) {
        controlp = hostControlTable.tp[i];
	mem_lock(controlp);

	/* turn the owner string from an octet string to a real string */
	owner = malloc(controlp->hostControlOwner->length + 1);
	if (owner) {
	    tempOS = controlp->hostControlOwner;
	    memcpy(owner, tempOS->octet_ptr, tempOS->length);
	}
	else
	    return;

	/* get a textual representation of the object */
	object = malloc(MAX_OCTET_STRING_SIZE);
	if (object) {
	    status =
		MakeDotFromOID(controlp->hostControlDataSource, object); 
	    if (status == -1) {
		free(object);
		object = NULL;
	    }
	}

        printf("\nHost Control Entry %d is %s, and owned by %s",
	       controlp->hostControlIndex,
	       rmon_status_string(controlp->hostControlStatus),
	       owner ? owner : "unknown");
        printf("\n Monitors host %s", object ? object : "unknown");
        printf("\n Table size is %d, last time an entry was deleted was %TC",
	       controlp->hostControlTableSize,
	       controlp->hostControlLastDeleteTime);
	free(owner);
	free(object);

	for (j = 0; j < controlp->hostControlTableSize; j++) {
	    timep = (hostTimeEntry_t *) controlp->htp[j];
            mem_lock(timep);

	    /* turn the address string from an octet string to a real string */
	    /* not sure if this step is absolutely needed */
	    timeaddr = malloc(timep->hostTimeAddress->length + 1);
	    if (timeaddr) {
		tempOS = timep->hostTimeAddress; 
		memcpy(timeaddr, tempOS->octet_ptr, tempOS->length);
	    }
	    else
		return;

	    printf("\n  Creation Order number is %d",
		   timep->hostTimeCreationOrder);
	    printf("\n   Physical address is %e", timeaddr);
	    printf("\n   Packets: rcvd %d, transmitted %d",
		   timep->hostTimeInPkts, timep->hostTimeOutPkts);
	    printf("\n   Octets: rcvd %d, transmitted %d",
		   timep->hostTimeInOctets, timep->hostTimeOutOctets);
	    printf("\n   # of packets transmitted: broadcast %d, multicast %d",
		   timep->hostTimeOutBroadcastPkts,
		   timep->hostTimeOutMulticastPkts);
	    printf("\n   # of bad packets transmitted is %d",
		   timep->hostTimeOutErrors);

	    free(timeaddr);
            free(timep);
	}

	free(controlp);
    }
    automore_disable();

}
