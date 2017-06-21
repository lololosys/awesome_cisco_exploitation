/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_k_mtrx.c,v 3.4.12.1 1996/05/17 21:26:32 anke Exp $
 * $Source: /release/112/cvs/Xsys/rmon/sr_k_mtrx.c,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_k_mtrx.c,v $
 * Revision 3.4.12.1  1996/05/17  21:26:32  anke
 * CSCdi48524:  Need mechanism to display RMON tables
 *              Fill in guts of show rmon commands
 * Branch: California_branch
 *
 * Revision 3.4  1996/02/19  06:06:03  jjohnson
 * CSCdi48853:  rmon can consume all available memory
 * As an interim measure, don't allow rmon to consume memory once the
 * heap's low-water mark has been reached.  While we're at it, name
 * all the malloc'ed memory so that we can tell what is used where
 *
 * Revision 3.3  1996/02/09  07:57:28  jjohnson
 * CSCdi48524:  Need mechanism to display RMON tables
 * parse chains are in place.  checkpoint work so that anke can take over.
 *
 * Revision 3.2  1995/11/17  18:39:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:20  shaker
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
#include "../util/qsort.h"
#include "rmon_util.h"
#include "logger.h"
#include "../os/free.h"

/* global data describing the matrixControlTable entries */
static SnmpType        matrixControlEntryTypeTable[] = {
    {INTEGER_TYPE, SR_READ_ONLY, 0},	/* matrixControlIndex */
    {OBJECT_ID_TYPE, SR_READ_WRITE, -1},	/* matrixControlDataSource */
    {INTEGER_TYPE, SR_READ_ONLY, -1},	/* matrixControlTableSize */
    {TIME_TICKS_TYPE, SR_READ_ONLY, -1},	/* matrixControlLastDeleteTime
						 *  */
    {OCTET_PRIM_TYPE, SR_READ_WRITE, -1},	/* matrixControlOwner */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* matrixControlStatus */
};

static Index_t         matrixControlEntryIndex[1];
static RmonTable       matrixControlTable;

static Index_t         matrixSDEntryIndex[2];
static Index_t         matrixDSEntryIndex[2];

void DeletematrixControlTableEntry SR_PROTOTYPE((int index));

/* this routine frees a matrix entry */
void
MatrixEntry_free(mep)
    matrixSDEntry_t *mep;
{
    if (mep != 0) {

#ifndef SR_FTL
	FreeOctetString(mep->matrixSDSourceAddress);
	FreeOctetString(mep->matrixSDDestAddress);
#endif				/* SR_FTL */

	free(mep);
    }
}

/* this routine deletes the entire matrix table associated with a
 * control table entry */
void
DeleteMatrixEntryTable(mcp)
    matrixControlEntry_t *mcp;
{
    RmonTable      *rtp;

    /* make sure there's something to free */
    if ((rtp = mcp->rtp) != 0) {
	/* get rid of all entries */
	while (rtp->nitems != 0) {
	    MatrixEntry_free((matrixSDEntry_t *) rtp->tp[0]);
	    RemoveTableEntry(rtp, 0);
	}

	/* now free the table */
	free(rtp->tp);
	free(rtp);
    }

}

/* this routine deletes the least recently accessed host table entries */
void
DeleteLRUMatrixEntry(mcp)
    matrixControlEntry_t *mcp;
{
    int             i, minindex, sdindex;
    unsigned long   minseqno;
    matrixSDEntry_t *mep;
    OctetString *savetip0, *savetip1;

    /* scan the list to see which is the least recently accessed */
    minseqno = 0xFFFFFFFF;
    minindex = (-1);
    for (i = 0; i < mcp->matrixControlTableSize; i++) {
	mep = (matrixSDEntry_t *) mcp->dstp[i];
	if (mep->seqno < minseqno) {
	    minindex = i;
	    minseqno = mep->seqno;
	}
    }

    /* make sure we got one */
    if (minindex != (-1)) {
	/* get a pointer to the entry */
	mep = (matrixSDEntry_t *) mcp->dstp[minindex];

	/* remove it from the matrix sd table */
	savetip0 = mcp->rtp->tip[0].value.octet_val;
	savetip1 = mcp->rtp->tip[1].value.octet_val;
	mcp->rtp->tip[0].value.octet_val = mep->matrixSDSourceAddress;
	mcp->rtp->tip[1].value.octet_val = mep->matrixSDDestAddress;
	sdindex = SearchTable(mcp->rtp, EXACT);
	mcp->rtp->tip[0].value.octet_val = savetip0;
	mcp->rtp->tip[1].value.octet_val = savetip1;
	if ((sdindex) == (-1)) {
	    DPRINTF((APERROR, "DeleteLRUMatrixEntry: can't find entry\n"));
	    return;
	}
	RemoveTableEntry(mcp->rtp, sdindex);

	/* update the control entry */
	mcp->matrixControlTableSize--;
	mcp->matrixControlLastDeleteTime = TimerCurTime();

	/* remove it from the matrix ds table */
	memcpy(mcp->dstp + minindex, mcp->dstp + (minindex + 1),
	   ((int) mcp->matrixControlTableSize - minindex) * sizeof(void *));

	/* free the memory */
	MatrixEntry_free(mep);
    }
}


int
VCompareMatrixDSEntries(const void *dsp1, const void *dsp2)
{
    return CompareMatrixDSEntries((matrixDSEntry_t **) dsp1,
				  (matrixDSEntry_t **) dsp2);
}
    

/* this routine compares two matrixDSEntries and returns greater than, equal
 * to, or less than 0, depending on their index values. */
int
CompareMatrixDSEntries(dsp1, dsp2)
    matrixDSEntry_t **dsp1, **dsp2;
{
    int             status;

    /* compare the more significant index */
    if ((status = CmpOctetStrings((*dsp1)->matrixDSDestAddress,
				  (*dsp2)->matrixDSDestAddress)) == 0) {
	/* compare the less significant index */
	status = CmpOctetStrings((*dsp1)->matrixDSSourceAddress,
				 (*dsp2)->matrixDSSourceAddress);
    }
    return (status);
}

/* this routine is responsible for adding new matrix table entries to the
 * table maintained by the matrix control entry.  Since the rfc says that
 * 	"It [the probe] must only create new entries based on information
 *	 received in good packets,"
 * this routine checks the packet status and size before adding the new
 * entry.  It creates a corresponding entry in the matrixDSTable as well.
 * This routine returns a pointer to the new matrix entry, or 0 if it fails. */
matrixSDEntry_t *
AddNewMatrixEntry(mcp, psp)
    matrixControlEntry_t *mcp;
    PacketState    *psp;
{
    matrixSDEntry_t *mep = 0;
    int             index;
    matrixDSEntry_t **newMatrixDSTable;

    /* make sure this is a good packet */
    if (psp->error == 0 && psp->size >= 64 && psp->size <= 1518) {
	/* if we're low on memory free up an old entry */
	if (HeapLowMem() != 0)
	    DeleteLRUMatrixEntry(mcp);

	/* attempt to allocate a new table array for the matrixDSTable */
	if ((newMatrixDSTable = (matrixDSEntry_t **)
	     malloc_named(((int) mcp->matrixControlTableSize + 1) *
			  sizeof(matrixDSEntry_t *), "RMON Mtrx Ptrs"))
	    != 0) {
	    /* try to insert the new table entry */
	    if ((index = NewTableEntry(mcp->rtp)) != -1) {
		/* get a pointer to this entry */
		mep = (matrixSDEntry_t *) mcp->rtp->tp[index];
		name_memory(mep, "RMON Mtrx Data");
#ifdef SR_FTL
		/* install the source address */
		mep->matrixSDSourceAddress = &mep->srcAddrOS;
		mep->matrixSDSourceAddress->length = ETHERMACADDRLEN;
		mep->matrixSDSourceAddress->octet_ptr = mep->srcaddr;
		memcpy((char *) mep->matrixSDSourceAddress->octet_ptr,
		       (char *) mcp->rtp->tip[0].value.octet_val->octet_ptr,
		       ETHERMACADDRLEN);

		/* install the destination address */
		mep->matrixSDDestAddress = &mep->dstAddrOS;
		mep->matrixSDDestAddress->length = ETHERMACADDRLEN;
		mep->matrixSDDestAddress->octet_ptr = mep->dstaddr;
		memcpy((char *) mep->matrixSDDestAddress->octet_ptr,
		       (char *) mcp->rtp->tip[1].value.octet_val->octet_ptr,
		       ETHERMACADDRLEN);
#endif				/* SR_FTL */

		/* add this entry to the end of the new DS table */
		memcpy(newMatrixDSTable, mcp->dstp,
		       (int) mcp->matrixControlTableSize * sizeof(matrixDSEntry_t *));
		newMatrixDSTable[mcp->matrixControlTableSize] =
		    (matrixDSEntry_t *) mep;
		free(mcp->dstp);
		mcp->dstp = newMatrixDSTable;

		/* initialize the entry */
		mcp->matrixControlTableSize++;
		mep->matrixSDIndex = mcp->matrixControlIndex;

		/* sort the DS table */
		qsort((char *) mcp->dstp, (int) mcp->matrixControlTableSize,
		      sizeof(matrixDSEntry_t *), VCompareMatrixDSEntries);
	    }
	    else {
		/* free the new table */
		free(newMatrixDSTable);
	    }
	}
    }

    /* return the new pointer */
    return (mep);
}

/* this routine resequences a matrix table to avoid problems with sequence
 * number wrapping before lru deletion.  This is an n-squared algorithm,
 * but it's called so infrequently that the overhead is not a problem. */
void
ResequenceMatrixTable(mcp)
    matrixControlEntry_t *mcp;
{
    matrixSDEntry_t *mep, *minmep;
    unsigned long   new, seqno;
    long            i, j;

    /* go through the entire host table looking for the minimum */
    new = 0;
    for (i = 0; i < mcp->matrixControlTableSize; i++) {
	/* look for a minimum greater than the new seqence we'll assign */
	seqno = 0xFFFFFFFF;
	minmep = 0;
	for (j = 0; j < mcp->matrixControlTableSize; j++) {
	    mep = (matrixSDEntry_t *) mcp->rtp->tp[j];
	    if (mep->seqno >= new && mep->seqno <= seqno) {
		/* save the new minimum */
		minmep = mep;
		seqno = minmep->seqno;
	    }
	}

	/* if we found one, update it */
	if (minmep != 0) {
	    minmep->seqno = new;
	}

	/* on to the next sequence number */
	new++;
    }

    /* reset the current seqence number for the table */
    mcp->curseqno = new;
}

/* this routine does matrix table processing for this packet */
void
ProcessMatrixEntry(psp)
    PacketState    *psp;
{
    int             matrixindex, index;
    matrixControlEntry_t *mcp;
    matrixSDEntry_t *mep;
    OctetString     srcos, dstos;

    /* process all matrix control entries */
    for (matrixindex = 0; matrixindex < matrixControlTable.nitems;
	 matrixindex++) {
	mcp = ((matrixControlEntry_t **) matrixControlTable.tp)[matrixindex];
	if (mcp->matrixControlStatus == D_matrixControlStatus_valid
	 && CmpDataSource(mcp->matrixControlDataSource, psp->source) == 0) {
	    /* get pointers to the octet strings used in the index */
	    mcp->rtp->tip[0].value.octet_val = &srcos;
	    mcp->rtp->tip[1].value.octet_val = &dstos;
	    srcos.length = ETHERMACADDRLEN;
	    dstos.length = ETHERMACADDRLEN;

	    /* look for the table entry for the conversation */
	    if (psp->size >= (2 * ETHERMACADDRLEN)) {
		srcos.octet_ptr = psp->p + ETHERMACADDRLEN;	/* source addr */
		dstos.octet_ptr = psp->p;	/* dest addr */
		if ((index = SearchTable(mcp->rtp, EXACT)) == -1) {
		    /* add a new entry for the source */
		    mep = AddNewMatrixEntry(mcp, psp);
		}
		else {
		    /* get a pointer to the existing entry */
		    mep = (matrixSDEntry_t *) mcp->rtp->tp[index];
		}
	    }
	    else {
		/* not a valid packet */
		mep = 0;
	    }

	    /* update stats only if we have an entry pointer */
	    if (mep != 0) {
		mep->matrixSDPkts++;
		mep->matrixSDOctets += psp->size;
		if (psp->error != 0)
		    mep->matrixSDErrors++;

		/* update sequence counter for lru processing */
		mep->seqno = mcp->curseqno++;

		/* resequence the table if necessary */
		if (mcp->curseqno >= 0xFFFFFF00)
		    ResequenceMatrixTable(mcp);
	    }
	}
    }
}

/* increments an octet string, returns 1 if a carry, 0 if not */
int
IncOctetString(dstosp, srcosp)
    OctetString    *dstosp, *srcosp;
{
    int             carry = 1;
    long            i;

    /* check for a valid length */
    if (srcosp->length > ETHERMACADDRLEN) {
	dstosp->length = 0;
	dstosp->octet_ptr = (unsigned char *) "";
    }
    else {
	/* increment and copy individual bytes */
	dstosp->length = srcosp->length;
	for (i = ETHERMACADDRLEN - 1; i >= 0; i--) {
	    dstosp->octet_ptr[i] = srcosp->octet_ptr[i] + carry;
	    if (dstosp->octet_ptr[i] != 0) {
		carry = 0;
	    }
	}
    }

    /* return the carry  value */
    return (carry);
}

/* --------------------- MIB COMPILER GENERATED ------------------------- */

int
k_matrixControlEntry_initialize()
{
    matrixControlEntryIndex[0].offset = I_matrixControlIndex;
    matrixControlEntryIndex[0].type = T_uint;

    matrixControlTable.nindices = 1;
    matrixControlTable.tip = matrixControlEntryIndex;
    matrixControlTable.nitems = 0;
    matrixControlTable.rowsize = sizeof(matrixControlEntry_t);
    if ((matrixControlTable.tp = (void **) malloc(sizeof(matrixControlEntry_t *))) == NULL) {
	return 0;
    }

    /* initialize the indices for the matrixSDTable */
    matrixSDEntryIndex[0].offset = I_matrixSDSourceAddress;
    matrixSDEntryIndex[0].type = T_ieeeMacAddr;
    matrixSDEntryIndex[1].offset = I_matrixSDDestAddress;
    matrixSDEntryIndex[1].type = T_ieeeMacAddr;

    /* initialize the indices for the matrixDSTable */
    matrixDSEntryIndex[0].offset = I_matrixDSDestAddress;
    matrixDSEntryIndex[0].type = T_ieeeMacAddr;
    matrixDSEntryIndex[1].offset = I_matrixDSSourceAddress;
    matrixDSEntryIndex[1].type = T_ieeeMacAddr;

    return 1;
}

int
k_matrixControlEntry_terminate()
{
    while (matrixControlTable.nitems > 0) {
	DeletematrixControlTableEntry(0);
    }

    free(matrixControlTable.tp);
    return 1;
}

/* This routine is called by the timeout code to
 * delete a pending creation of a Table entry */
void
matrixControlTableDeleteCallback(tdp)
    TimeOutDescriptor *tdp;
{
    matrixControlEntry_t *data;

    /* dummy up an entry to delete */
    data = (matrixControlEntry_t *) tdp->UserData2;
    data->matrixControlStatus = D_matrixControlStatus_invalid;
    data->TimerId = -1;

    /* free the timeout descriptor */
    free(tdp);

    /* call the set method */
    k_matrixControlEntry_set(data, (ContextInfo *) NULL, 0);
}

/* This routine deletes an entry from the matrixControlTable */
void
DeletematrixControlTableEntry(index)
    int             index;
{
    matrixControlEntry_t *data;

    /* get a pointer to the old entry */
    data = (matrixControlEntry_t *) matrixControlTable.tp[index];

    /* free the old entry and remove it from the table */
    matrixControlEntry_free(data);
    RemoveTableEntry(&matrixControlTable, index);
}

/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_matrixControlEntryFreeUserpartData(data)
    matrixControlEntry_t *data;
{
    /* nothing to free by default */
}

matrixControlEntry_t *
k_matrixControlEntry_get(serialNum, contextInfo, nominator, searchType, matrixControlIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            matrixControlIndex;
{
    int             index;

    matrixControlTable.tip[0].value.uint_val = matrixControlIndex;
    if ((index = SearchTable(&matrixControlTable, searchType)) == -1) {
	return NULL;
    }

    return (matrixControlEntry_t *) matrixControlTable.tp[index];

}

#ifdef SETS
int
k_matrixControlEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

#ifdef SR_SNMPv2
    matrixControlEntry_t *undodata = (matrixControlEntry_t *) dp->undodata;
#else				/* SR_SNMPv2 */
    matrixControlEntry_t *undodata = NULL;
#endif				/* SR_SNMPv2 */
    int             error_status = NO_ERROR;

    /*
     * make sure that the data source isn't being changed if this is a valid
     * entry
     */
    if (undodata != 0 && undodata->matrixControlStatus
	== D_matrixControlStatus_valid) {
	switch (object->nominator) {
	  case I_matrixControlDataSource:
	    if (CmpOID(value->oid_value, undodata->matrixControlDataSource)
		!= 0) {
		error_status = INCONSISTENT_VALUE_ERROR;
	    }
	    break;
	}
    }

    return error_status;
}

int
k_matrixControlEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    matrixControlEntry_t *data = (matrixControlEntry_t *) dp->data;

    if (data->matrixControlStatus == D_matrixControlStatus_invalid) {
	dp->state = DELETE;
    }
    else {
	dp->state = ADD_MODIFY;

	/* if setting to valid, make sure we have all the information we need */
	if (data->matrixControlStatus == D_matrixControlStatus_valid) {
	    if (!VALID(I_matrixControlDataSource, data->valid)
	    || CheckEthernetDataSource(data->matrixControlDataSource) == 0) {
		/* can't do it, don't have a valid data source */
		dp->state = UNKNOWN;
	    }
	}
    }

    return NO_ERROR;
}

int
k_matrixControlEntry_set_defaults(dp)
    doList_t       *dp;
{

    matrixControlEntry_t *data = (matrixControlEntry_t *) dp->data;

    /*
     * if we're low on memory, don't allow the entry to be created
     */
    if (HeapLowMem())
	return RESOURCE_UNAVAILABLE_ERROR;

    if ((data->matrixControlDataSource = MakeOIDFromDot("0.0")) == 0) {
	return RESOURCE_UNAVAILABLE_ERROR;
    }

    if ((data->matrixControlOwner = MakeOctetStringFromText("")) == 0) {
	return RESOURCE_UNAVAILABLE_ERROR;
    }

    data->matrixControlStatus = D_matrixControlStatus_createRequest;
    data->TimerId = -1;

    /* set up private state data */
    if ((data->dstp = malloc_named(sizeof(matrixDSEntry_t *),
				   "RMON Mtrx Dstp"))
	== NULL) {
	return RESOURCE_UNAVAILABLE_ERROR;
    }

    /* get the matrixSDEntry array */
    if ((data->rtp = malloc_named(sizeof(RmonTable),
				  "RMON Mtrx Rtp")) == 0) {
	free(data->dstp);
	return RESOURCE_UNAVAILABLE_ERROR;
    }

    /* initialize the table */
    if ((data->rtp->tp = malloc_named(sizeof(matrixSDEntry_t *),
				      "RMON Mtrx Ptrs"))
	== NULL) {
	free(data->dstp);
	free(data->rtp);
	return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->rtp->tip = matrixSDEntryIndex;
    data->rtp->nitems = 0;
    data->rtp->nindices = 2;
    data->rtp->rowsize = sizeof(matrixSDEntry_t);

    SET_ALL_VALID(data->valid);

    return NO_ERROR;
}

int
k_matrixControlEntry_set(data, contextInfo, function)
    matrixControlEntry_t *data;
    ContextInfo    *contextInfo;
    int             function;
{

    int             index;
    matrixControlEntry_t *new = NULL;

    /* find this entry in the table */
    matrixControlTable.tip[0].value.uint_val = data->matrixControlIndex;
    if ((index = SearchTable(&matrixControlTable, EXACT)) != -1) {
	new = (matrixControlEntry_t *) matrixControlTable.tp[index];
    }

    /* perform the table entry operation on it */
    if (data->matrixControlStatus == D_matrixControlStatus_invalid) {
	if (data->TimerId != -1) {
	    CancelEntryStatusTimeout((int) data->TimerId);
	}

	if (index == -1) {
	    return NO_ERROR;
	}
	else {
	    /* delete the sd table */
	    DeleteMatrixEntryTable(new);

	    /* delete the ds table */
	    if (new->dstp != 0)
		free(new->dstp);

	    /* delete the table entry */
	    DeletematrixControlTableEntry(index);
	    return NO_ERROR;
	}
    }
    else if (index == -1) {
	/* add the entry */
	matrixControlTable.tip[0].value.uint_val = data->matrixControlIndex;
	if ((index = NewTableEntry(&matrixControlTable)) == -1) {
	    return GEN_ERROR;
	}
	new = (matrixControlEntry_t *) matrixControlTable.tp[index];
	name_memory(new, "RMON Mtrx Ctrl");

	/* set a timeout */
	if ((new->TimerId = SetEntryStatusTimeout(MAX_ROW_CREATION_TIME, (void *) new, matrixControlTableDeleteCallback)) == -1) {
	    DeletematrixControlTableEntry(index);
	    return GEN_ERROR;
	}
    }

    TransferEntries(I_matrixControlStatus,
		    matrixControlEntryTypeTable,
		    (unsigned long *)new, (unsigned long *)data);
    SET_ALL_VALID(new->valid);

    /* transfer private state information */
    new->rtp = data->rtp;
    new->dstp = data->dstp;
    new->curseqno = data->curseqno;

    /* if the new row entry is now valid, cancel the creation timeout */
    if (new->matrixControlStatus == D_matrixControlStatus_valid && new->TimerId != -1) {
	CancelEntryStatusTimeout((int) new->TimerId);
	new->TimerId = -1;
    }
    else {
	if (new->matrixControlStatus == D_matrixControlStatus_createRequest) {
	    new->matrixControlStatus = D_matrixControlStatus_underCreation;
	}
    }

    return NO_ERROR;
}

#endif				/* SETS */

#ifdef SR_SNMPv2
int
matrixControlEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return UNDO_FAILED_ERROR;
}

#endif				/* SR_SNMPv2 */


matrixSDEntry_t *
k_matrixSDEntry_get(serialNum, contextInfo, nominator, searchType, matrixSDIndex, matrixSDSourceAddress, matrixSDDestAddress)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            matrixSDIndex;
    OctetString    *matrixSDSourceAddress;
    OctetString    *matrixSDDestAddress;
{
    matrixControlEntry_t *mcp;
    matrixSDEntry_t *data;
    int             index, matrixindex, carry;
    OctetString     srcos, dstos, tmpos;
    unsigned char   srcaddr[ETHERMACADDRLEN], dstaddr[ETHERMACADDRLEN];
    unsigned char   tmpaddr[ETHERMACADDRLEN];

    /*
     * make sure we are starting with valid addresses.  It is possible that
     * we will receive a mac address which is 7 octets long; this happens
     * when a get-next pdu is received with a mac address of
     * ff:ff:ff:ff:ff:ff -- this will require nulling this mac address and
     * incrementing the next.
     */
    carry = 0;

    /* set up the destination address */
    memset((char *) dstaddr, 0, ETHERMACADDRLEN);
    dstos.octet_ptr = dstaddr;
    if (matrixSDDestAddress->length > ETHERMACADDRLEN) {
	dstos.length = 0;

	/* increment the next octet string */
	tmpos.length = 0;
	memset((char *) tmpaddr, 0, ETHERMACADDRLEN);
	tmpos.octet_ptr = tmpaddr;
	carry = IncOctetString(&tmpos, matrixSDSourceAddress);
	matrixSDSourceAddress = &tmpos;
    }
    else {
	/* copy the original address to a 6-byte buffer */
	dstos.length = matrixSDDestAddress->length;
	memcpy(dstaddr, matrixSDDestAddress->octet_ptr, (int) dstos.length);
    }
    matrixSDDestAddress = &dstos;

    /* set up the source addr */
    memset((char *) srcaddr, 0, ETHERMACADDRLEN);
    srcos.octet_ptr = srcaddr;
    if (matrixSDSourceAddress->length > ETHERMACADDRLEN) {
	srcos.length = 0;
	carry = 1;
    }
    else {
	/* copy the original address to a 6-byte buffer */
	srcos.length = matrixSDSourceAddress->length;
	memcpy(srcaddr, matrixSDSourceAddress->octet_ptr, (int) srcos.length);
    }
    matrixSDSourceAddress = &srcos;

    /* if there was an overflow, move to the next control table entry */
    if (carry != 0)
	matrixSDIndex++;

    /* repeat until we find an appropriate match, or there is no more data */
    data = NULL;
    do {
	/* get the host control entry */
	matrixControlTable.tip[0].value.uint_val = matrixSDIndex;
	if ((index = SearchTable(&matrixControlTable, searchType)) == -1) {
	    /* no such control table */
	    break;
	}
	mcp = (matrixControlEntry_t *) matrixControlTable.tp[index];

	/* search the host entry table */
	mcp->rtp->tip[0].value.octet_val = matrixSDSourceAddress;
	mcp->rtp->tip[1].value.octet_val = matrixSDDestAddress;
	if ((matrixindex = SearchTable(mcp->rtp, searchType)) != -1) {
	    data = (matrixSDEntry_t *) mcp->rtp->tp[matrixindex];
	}

	/* did we find it? */
	if (data == NULL && searchType == NEXT) {
	    /* no entries, look in the next table */
	    matrixSDIndex++;
	    srcos.length = 0;
	    memset((char *) srcos.octet_ptr, 0, ETHERMACADDRLEN);
	    matrixSDDestAddress = &srcos;
	    matrixSDSourceAddress = &srcos;
	}
    } while (data == NULL && searchType == NEXT);

    /* return the data */
    return (data);
}

matrixDSEntry_t *
k_matrixDSEntry_get(serialNum, contextInfo, nominator, searchType, matrixDSIndex, matrixDSDestAddress, matrixDSSourceAddress)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            matrixDSIndex;
    OctetString    *matrixDSDestAddress;
    OctetString    *matrixDSSourceAddress;
{
    matrixControlEntry_t *mcp;
    matrixDSEntry_t *data;
    int             index, matrixindex, carry;
    OctetString     srcos, dstos, tmpos;
    unsigned char   srcaddr[ETHERMACADDRLEN], dstaddr[ETHERMACADDRLEN];
    unsigned char   tmpaddr[ETHERMACADDRLEN];

    /*
     * make sure we are starting with valid addresses.  It is possible that
     * we will receive a mac address which is 7 octets long; this happens
     * when a get-next pdu is received with a mac address of
     * ff:ff:ff:ff:ff:ff -- this will require nulling this mac address and
     * incrementing the next.
     */
    carry = 0;

    /* set up the source address */
    memset((char *) srcaddr, 0, ETHERMACADDRLEN);
    srcos.octet_ptr = srcaddr;
    if (matrixDSSourceAddress->length > ETHERMACADDRLEN) {
	srcos.length = 0;

	/* increment the next octet string */
	tmpos.length = 0;
	memset((char *) tmpaddr, 0, ETHERMACADDRLEN);
	tmpos.octet_ptr = tmpaddr;
	carry = IncOctetString(&tmpos, matrixDSDestAddress);
	matrixDSDestAddress = &tmpos;
    }
    else {
	/* copy the original address to a 6-byte buffer */
	srcos.length = matrixDSSourceAddress->length;
	memcpy(srcaddr, matrixDSSourceAddress->octet_ptr, (int) srcos.length);
    }
    matrixDSSourceAddress = &srcos;

    /* set up the destination address */
    memset((char *) dstaddr, 0, ETHERMACADDRLEN);
    dstos.octet_ptr = dstaddr;
    if (matrixDSDestAddress->length > ETHERMACADDRLEN) {
	dstos.length = 0;
	carry = 1;
    }
    else {
	/* copy the original address to a 6-byte buffer */
	dstos.length = matrixDSDestAddress->length;
	memcpy(dstaddr, matrixDSDestAddress->octet_ptr, (int) dstos.length);
    }
    matrixDSDestAddress = &dstos;

    /* if there was an overflow, move to the next control table entry */
    if (carry != 0)
	matrixDSIndex++;

    /* repeat until we find an appropriate match, or there is no more data */
    data = NULL;
    do {
	/* get the host control entry */
	matrixControlTable.tip[0].value.uint_val = matrixDSIndex;
	if ((index = SearchTable(&matrixControlTable, searchType)) == -1) {
	    /* no such control table */
	    break;
	}
	mcp = (matrixControlEntry_t *) matrixControlTable.tp[index];

	/*
	 * search the host entry table.  Since DS entries are not maintained
	 * in an RmonTable, we must use TableLookup() directly.
	 */
	matrixDSEntryIndex[0].value.octet_val = matrixDSDestAddress;
	matrixDSEntryIndex[1].value.octet_val = matrixDSSourceAddress;
	if ((matrixindex = TableLookup((anytype_t **) mcp->dstp,
				       (int) mcp->matrixControlTableSize,
				matrixDSEntryIndex, 2, searchType)) != -1) {
	    data = (matrixDSEntry_t *) mcp->dstp[matrixindex];
	}

	/* did we find it? */
	if (data == NULL && searchType == NEXT) {
	    /* no entries, look in the next table */
	    matrixDSIndex++;
	    srcos.length = 0;
	    memset((char *) srcos.octet_ptr, 0, ETHERMACADDRLEN);
	    matrixDSDestAddress = &srcos;
	    matrixDSSourceAddress = &srcos;
	}
    } while (data == NULL && searchType == NEXT);

    /* return the data */
    return (data);
}

/******************** cisco supplied routines follow ********************/

/*
 * show rmon matrix
 * output a string similiar to the following for every traffic matrix
 *    Matrix 12345 is valid, and owned by foo
 *      Monitors oid.oid.oid.oid.oid.oid
 *      Table size is xxxxxx, last time an entry was deleted was xxxx
 *       Source addr is xxxxxxxxxxx, dest addr is xxxxxxxxxxxx
 *       Transmitted xxxxxxxx pkts, xxxxxxxxx octets, xxxxxxxxx errors
 */
void
show_rmon_matrix (void)
{
    matrixControlEntry_t    *matrixControlp;
    matrixSDEntry_t *matrixSDp;
    int             i, j, status;
    OctetString    *tempOS;
    char	   *owner;
    char           *sourceaddr;
    char           *destaddr;
    char           *object;

    automore_enable(NULL);
    if (matrixControlTable.nitems == 0) {
	printf("\nMatrix Control table is empty");
	automore_disable();
	return;
    }

    for (i = 0; i < matrixControlTable.nitems; i++) {
        matrixControlp = matrixControlTable.tp[i];
	mem_lock(matrixControlp);

	/* turn the owner string from an octet string to a real string */
	owner = malloc(matrixControlp->matrixControlOwner->length + 1);
	if (owner) {
	    tempOS = matrixControlp->matrixControlOwner;
	    memcpy(owner, tempOS->octet_ptr, tempOS->length);
	}
	else
	    return;

	/* get a textual representation of the object */
	object = malloc(MAX_OCTET_STRING_SIZE);
	if (object) {
	    status =
		MakeDotFromOID(matrixControlp->matrixControlDataSource,object);
	    if (status == -1) {
		free(object);
		object = NULL;
	    }
	}

        printf("\nMatrix %d is %s, and owned by %s",
	       matrixControlp->matrixControlIndex,
	       rmon_status_string(matrixControlp->matrixControlStatus), 
               owner ? owner : "unknown"); 
        printf("\n Monitors %s", object ? object : "unknown");
        printf("\n Table size is %d,",
	       matrixControlp->matrixControlTableSize);
        printf(" last time an entry was deleted was at %TC",
	       matrixControlp->matrixControlLastDeleteTime); 

        for (j = 0; j < matrixControlp->rtp->nitems; j++) {
	    matrixSDp = matrixControlp->rtp->tp[j]; 
	    mem_lock(matrixSDp);


	    /* turn source address string from octet string to a real string */
	    sourceaddr = malloc(matrixSDp->matrixSDSourceAddress->length + 1);
	    if (sourceaddr) {
		tempOS = matrixSDp->matrixSDSourceAddress;
		memcpy(sourceaddr, tempOS->octet_ptr, tempOS->length);
	    }
	    else
		return;

	    /* turn dest address string from octet string to real string */
	    destaddr = malloc(matrixSDp->matrixSDDestAddress->length + 1);
	    if (destaddr) {
		tempOS = matrixSDp->matrixSDDestAddress;
		memcpy(destaddr, tempOS->octet_ptr, tempOS->length);
	    }
	    else
		return;

	    printf("\n  Source addr is %e,", sourceaddr);
	    printf(" dest addr is %e", destaddr);
	    printf("\n  Transmitted %d pkts, %d octets, %d errors",
		   matrixSDp->matrixSDPkts, matrixSDp->matrixSDOctets,
		   matrixSDp->matrixSDErrors); 
	    free(destaddr);
	    free(sourceaddr);
	    free(matrixSDp);
	}
	free(owner);
	free(object);
	free(matrixControlp);
    }
    automore_disable();
}
