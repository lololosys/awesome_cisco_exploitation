/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_k_capt.c,v 3.5.12.1 1996/05/17 21:26:25 anke Exp $
 * $Source: /release/112/cvs/Xsys/rmon/sr_k_capt.c,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_k_capt.c,v $
 * Revision 3.5.12.1  1996/05/17  21:26:25  anke
 * CSCdi48524:  Need mechanism to display RMON tables
 *              Fill in guts of show rmon commands
 * Branch: California_branch
 *
 * Revision 3.5  1996/02/19  06:05:57  jjohnson
 * CSCdi48853:  rmon can consume all available memory
 * As an interim measure, don't allow rmon to consume memory once the
 * heap's low-water mark has been reached.  While we're at it, name
 * all the malloc'ed memory so that we can tell what is used where
 *
 * Revision 3.4  1996/02/09  07:57:24  jjohnson
 * CSCdi48524:  Need mechanism to display RMON tables
 * parse chains are in place.  checkpoint work so that anke can take over.
 *
 * Revision 3.3  1995/11/17  18:39:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:22:18  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:03:13  shaker
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
#include "rmon_registry.h"
#include "rmon_util.h"
#include "logger.h"
#include "../os/free.h"

/* macros for the capture group */
#define	MAXCAPTURESLICE		1518
#define	BS_TOOLONG		0x01
#define	BS_TOOSHORT		0x02
#define	BS_CRCALIGNERROR	0x04
#define	BS_ERRORSEEN		0x08
#define	BS_ORDERAPPROXIMATE	0x10

/* macros for printing captured packets */
#define NUM_LINES_PER_PRINTF 2
#define UNFORMATTED_CHARS_PER_LINE 16
#define BYTES_PER_UNFORMATTED_CHAR 8
#define BUFFER_SIZE NUM_LINES_PER_PRINTF * \
                    UNFORMATTED_CHARS_PER_LINE * \
                    BYTES_PER_UNFORMATTED_CHAR

/* global data describing the bufferControlTable entries */
static SnmpType        bufferControlEntryTypeTable[] = {
    {INTEGER_TYPE, SR_READ_ONLY, 0},	/* bufferControlIndex */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* bufferControlChannelIndex */
    {INTEGER_TYPE, SR_READ_ONLY, -1},	/* bufferControlFullStatus */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* bufferControlFullAction */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* bufferControlCaptureSliceSize */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* bufferControlDownloadSliceSize */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* bufferControlDownloadOffset */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* bufferControlMaxOctetsRequested */
    {INTEGER_TYPE, SR_READ_ONLY, -1},	/* bufferControlMaxOctetsGranted */
    {INTEGER_TYPE, SR_READ_ONLY, -1},	/* bufferControlCapturedPackets */
    {TIME_TICKS_TYPE, SR_READ_ONLY, -1},	/* bufferControlTurnOnTime */
    {OCTET_PRIM_TYPE, SR_READ_WRITE, -1},	/* bufferControlOwner */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* bufferControlStatus */
};

static Index_t         bufferControlEntryIndex[1];
static RmonTable       bufferControlTable;

static const char malloc_owner[] = "RMON PktCap Buf";

void DeletebufferControlTableEntry SR_PROTOTYPE((int index));

/* this routine notifies all buffer control table entries whose
 * bufferControlChannelIndex matches this one that this packet should
 * be captured. */
void
NotifyBuffers(channelIndex)
    long            channelIndex;
{
    bufferControlEntry_t *bcp;
    int             bufindex;

    /*
     * go through the buffer control table and set the active flag on
     * appropriate entries
     */
    for (bufindex = 0; bufindex < bufferControlTable.nitems; bufindex++) {
	/* get a pointer to the entry */
	bcp = (bufferControlEntry_t *) bufferControlTable.tp[bufindex];
	if (bcp->bufferControlStatus != D_bufferControlStatus_valid)
	    continue;

	/* set the active flag */
	if (bcp->bufferControlChannelIndex == channelIndex) {
	    /* activate this entry */
	    bcp->active = 1;
	}
    }
}

/* this routine notifies all buffer valid control table entries that some
 * number of packets have not been processed correctly */
void
NotifyBuffersOfError()
{
    bufferControlEntry_t *bcp;
    int             bufindex;

    /*
     * go through the buffer control table and set the active flag on
     * appropriate entries
     */
    for (bufindex = 0; bufindex < bufferControlTable.nitems; bufindex++) {
	/* get a pointer to the entry */
	bcp = (bufferControlEntry_t *) bufferControlTable.tp[bufindex];
	if (bcp->bufferControlStatus != D_bufferControlStatus_valid)
	    continue;

	/* set the error flag */
	bcp->errorSeen++;
    }
}

/* this routine allocates a packet capture buffer of the appropriate size,
 * along with the associated octet string if necessary. */
captureBufferEntry_t *
AllocateCaptureBuffer(bcp)
    bufferControlEntry_t *bcp;
{
    captureBufferEntry_t *new, *retval = 0;

    /* see if we have memory to play with */
    if (HeapLowMem())
	return(NULL);

    /* get the buffer */
    new = malloc_named(sizeof(captureBufferEntry_t), malloc_owner);

    if (new) {
	/* get the octet string */
	if ((new->captureBufferPacketData =
	     (OctetString *) malloc(sizeof(OctetString))) != 0) {
	    /* null out the octet pointer */
	    new->captureBufferPacketData->octet_ptr = 0;

	    /* get the packet buffer */
	    new->data = malloc_named(bcp->bufferSize, malloc_owner);
	    if (new->data) {
		/* set the control index */
		new->captureBufferControlIndex = bcp->bufferControlIndex;
		bcp->bufferCount++;
		retval = new;
	    }
	}
    }

    /* clean up after problems */
    if (retval == 0) {
	if (new != 0) {
	    if (new->captureBufferPacketData != 0) {
		free(new->captureBufferPacketData);
	    }
	    free(new);
	}
    }

    return (retval);
}

/* this routine frees packet capture buffers */
void
FreeCaptureBuffer(bp, bcp)
    captureBufferEntry_t *bp;
    bufferControlEntry_t *bcp;
{
    /* free things as safely as possible */
    if (bp != 0) {
	if (bcp->bufferCount == 0) {
	    DPRINTF((APERROR,
		     "FreeCaptureBuffer: count is 0 for %ld\n",
		     bcp->bufferControlIndex));
	}
	bcp->bufferCount--;

	/* free the octet string structure */
	if (bp->captureBufferPacketData != 0) {
	    free(bp->captureBufferPacketData);
	}

	/* free the data buffer */
	if (bp->data != 0) {
	    free(bp->data);
	}

	/* free the pointer */
	free(bp);
    }
}

/* this routine gets packet capture buffers for the buffer control entry,
 * and sets the granted value appropriately */
void
GetCaptureBuffers(bcp)
    bufferControlEntry_t *bcp;
{
    long            i, count;
    captureBufferEntry_t *new, *bp;

    /* if we're supposed to save "as many as possible" this is easy */
    if (bcp->bufferControlMaxOctetsRequested == (-1)) {
	/* we'll do our best */
	bcp->bufferControlMaxOctetsGranted = (-1);
	bcp->bufferControlFullStatus = D_bufferControlFullStatus_spaceAvailable;
	return;
    }

    /* get the real number of octets currently in the system */
    bcp->bufferControlMaxOctetsGranted = bcp->bufferCount * bcp->bufferSize;

    /* determine whether to add or subtract buffers */
    count = (bcp->bufferControlMaxOctetsRequested / bcp->bufferSize)
	- bcp->bufferCount;
    if (count > 0) {
	/* start allocating buffers */
	for (i = 0; i < count; i++) {
	    /* get a buffer */
	    if ((new = AllocateCaptureBuffer(bcp)) == 0) {
		/* can't get more */
		break;
	    }

	    /* add it to the list */
	    new->next = bcp->freelist;
	    bcp->freelist = new;

	    /* update the granted count */
	    bcp->bufferControlMaxOctetsGranted += bcp->bufferSize;
	    bcp->bufferControlFullStatus =
		D_bufferControlFullStatus_spaceAvailable;
	}
    }
    else if (count < 0) {
	/* convert to a positive count */
	count = 0 - count;

	/* free buffers from the free list, if possible */
	i = 0;
	while (bcp->freelist != 0 && i < count) {
	    bp = bcp->freelist;
	    bcp->freelist = bp->next;
	    FreeCaptureBuffer(bp, bcp);
	    i++;
	}

	/* free them oldest first, otherwise */
	while (bcp->oldest != 0 && i < count) {
	    bp = bcp->oldest;
	    bcp->oldest = bp->prev;
	    if (bcp->oldest == 0) {
		bcp->newest = 0;
	    }
	    else {
		bcp->oldest->next = 0;
	    }
	    FreeCaptureBuffer(bp, bcp);
	    bcp->bufferControlCapturedPackets--;
	    i++;
	}

	/* make sure we freed them all */
	if (i != count) {
	    DPRINTF((APERROR, "GetCaptureBuffers: couldn't free %ld buffers\n",
		     count));
	}

	/* check the status */
	if (bcp->freelist == 0) {
	    /* no more free buffers */
	    bcp->bufferControlFullStatus = D_bufferControlFullStatus_full;
	}

	/* adjust the granted count */
	bcp->bufferControlMaxOctetsGranted -= (count * bcp->bufferSize);
    }
}

/* this routine gets a buffer to put the data in.  The packet should already
 * be part of the table maintained by the control entry */
captureBufferEntry_t *
GetNextCaptureBuffer(bcp)
    bufferControlEntry_t *bcp;
{
    captureBufferEntry_t *bp;

    /* try to get it from the free list */
    if ((bp = bcp->freelist) != 0) {
	/* get it from the free list */
	bp = bcp->freelist;
	bcp->freelist = bp->next;
	if (bcp->freelist == 0) {
	    /* no more space */
	    bcp->bufferControlFullStatus = D_bufferControlFullStatus_full;
	}
    }
    else if (bcp->bufferControlMaxOctetsRequested == (-1)) {
	/* try to allocate a new one */
	bp = AllocateCaptureBuffer(bcp);
    }
    else if ((bp = bcp->oldest) != 0) {
	/* wrap buffers -- extract from the current table */
	bcp->oldest = bp->prev;
	if (bcp->oldest != 0) {
	    bcp->oldest->next = 0;
	}
	else {
	    bcp->newest = 0;
	}
	bcp->bufferControlCapturedPackets--;
    }
    return (bp);
}

/* this routine adds a capture buffer to the table maintained by the control
 * entry */
void
AddCaptureBuffer(bp, bcp)
    captureBufferEntry_t *bp;
    bufferControlEntry_t *bcp;
{
    /* get the index for the entry */
    bcp->currentIndex++;
    bp->captureBufferIndex = bcp->currentIndex;

    /* add it as the newest entry */
    bp->next = bcp->newest;
    bp->prev = 0;
    if (bcp->newest != 0) {
	bcp->newest->prev = bp;
    }
    bcp->newest = bp;
    if (bcp->oldest == 0) {
	bcp->oldest = bcp->newest;
    }

    /* update the captured count */
    bcp->bufferControlCapturedPackets++;
}

/* this routine handles packet capture */
void
ProcessPacketCapture(psp)
    PacketState    *psp;
{
    bufferControlEntry_t *bcp;
    captureBufferEntry_t *bp;
    int             bufindex, len;

    /*
     * go through the buffer control table and set the active flag on
     * appropriate entries
     */
    for (bufindex = 0; bufindex < bufferControlTable.nitems; bufindex++) {
	/* get a pointer to the entry */
	bcp = (bufferControlEntry_t *) bufferControlTable.tp[bufindex];
	if (bcp->bufferControlStatus != D_bufferControlStatus_valid
	    || bcp->active == 0)
	    continue;

	/* this packet must be destined for this buffer table */
	bcp->active = 0;	/* clear the flag */

	/* all done if this buffer is full */
	if (bcp->bufferControlFullStatus == D_bufferControlFullStatus_full
	    && bcp->bufferControlFullAction
	    == D_bufferControlFullAction_lockWhenFull) {
	    /* on to the next control entry */
	    continue;
	}

	/* get a buffer for the data */
	if ((bp = GetNextCaptureBuffer(bcp)) == 0) {
	    continue;
	}

	/* copy the packet data */
	if (psp->size > (int) bcp->bufferSize) {
	    len = (int) bcp->bufferSize;
	}
	else {
	    len = psp->size;
	}
	memcpy(bp->data, psp->p, len);

	/* fill in the rest of the buffer */
	bp->captureBufferPacketLength = (long) psp->size;
	bp->captureBufferPacketID = psp->id;
	bp->captureBufferPacketTime =
	    CLOCK_DIFF_SIGNED(bcp->startTime, psp->timestamp);;
	bp->captureBufferPacketStatus = 0;
	if (psp->size > 1518) {
	    bp->captureBufferPacketStatus += BS_TOOLONG;
	}
	else if (psp->size < 64) {
	    bp->captureBufferPacketStatus += BS_TOOSHORT;
	}
	if (psp->error != 0)
	    bp->captureBufferPacketStatus += BS_CRCALIGNERROR;
	if (bcp->errorSeen != 0) {
	    bp->captureBufferPacketStatus += BS_ERRORSEEN;
	    bcp->errorSeen = 0;
	}
	if (psp->orderapprox != 0)
	    bp->captureBufferPacketStatus +=
		BS_ORDERAPPROXIMATE;

	/* add the buffer to the table */
	AddCaptureBuffer(bp, bcp);
    }
}

/* --------------------- MIB COMPILER GENERATED ------------------------- */

int
k_bufferControlEntry_initialize()
{
    bufferControlEntryIndex[0].offset = I_bufferControlIndex;
    bufferControlEntryIndex[0].type = T_uint;

    bufferControlTable.nindices = 1;
    bufferControlTable.tip = bufferControlEntryIndex;
    bufferControlTable.nitems = 0;
    bufferControlTable.rowsize = sizeof(bufferControlEntry_t);
    if ((bufferControlTable.tp = (void **) malloc(sizeof(bufferControlEntry_t *))) == NULL) {
	return 0;
    }

    return 1;
}

int
k_bufferControlEntry_terminate()
{
    while (bufferControlTable.nitems > 0) {
	DeletebufferControlTableEntry(0);
    }

    free(bufferControlTable.tp);
    return 1;
}

/* This routine is called by the timeout code to
 * delete a pending creation of a Table entry */
void
bufferControlTableDeleteCallback(tdp)
    TimeOutDescriptor *tdp;
{
    bufferControlEntry_t *data;

    /* dummy up an entry to delete */
    data = (bufferControlEntry_t *) tdp->UserData2;
    data->bufferControlStatus = D_bufferControlStatus_invalid;
    data->TimerId = -1;

    /* free the timeout descriptor */
    free(tdp);

    /* call the set method */
    k_bufferControlEntry_set(data, (ContextInfo *) NULL, 0);
}

/* This routine deletes an entry from the bufferControlTable */
void
DeletebufferControlTableEntry(index)
    int             index;
{
    bufferControlEntry_t *data;

    /* get a pointer to the old entry */
    data = (bufferControlEntry_t *) bufferControlTable.tp[index];

    /* free the old entry and remove it from the table */
    bufferControlEntry_free(data);
    RemoveTableEntry(&bufferControlTable, index);
}

/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_bufferControlEntryFreeUserpartData(data)
    bufferControlEntry_t *data;
{
    /* nothing to free by default */
}

bufferControlEntry_t *
k_bufferControlEntry_get(serialNum, contextInfo, nominator, searchType, bufferControlIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            bufferControlIndex;
{
    int             index;

    bufferControlTable.tip[0].value.uint_val = bufferControlIndex;
    if ((index = SearchTable(&bufferControlTable, searchType)) == -1) {
	return NULL;
    }

    return (bufferControlEntry_t *) bufferControlTable.tp[index];

}

#ifdef SETS
int
k_bufferControlEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

#ifdef SR_SNMPv2
    bufferControlEntry_t *undodata = (bufferControlEntry_t *) dp->undodata;
#else				/* SR_SNMPv2 */
    bufferControlEntry_t *undodata = NULL;
#endif				/* SR_SNMPv2 */
    int             error_status = NO_ERROR;

    /* sanity check the values */
    if ((object->nominator == I_bufferControlCaptureSliceSize
	 && value->sl_value > 65536)) {
	/*
	 * the slice size can't be larger than 64K on some systems because
	 * it's stored as an integer
	 */
	return (INCONSISTENT_VALUE_ERROR);
    }

    /*
     * make sure that the data source isn't being changed if this is a valid
     * entry
     */
    if (undodata != 0 && undodata->bufferControlStatus
	== D_bufferControlStatus_valid) {
	if ((object->nominator == I_bufferControlChannelIndex
	     && value->sl_value != undodata->bufferControlChannelIndex)
	    || (object->nominator == I_bufferControlCaptureSliceSize
	   && value->sl_value != undodata->bufferControlCaptureSliceSize)) {
	    /* can't modify these objects once the entry is made valid */
	    error_status = INCONSISTENT_VALUE_ERROR;
	}
    }

    return (error_status);
}

int
k_bufferControlEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    bufferControlEntry_t *data = (bufferControlEntry_t *) dp->data;

    if (data->bufferControlStatus == D_bufferControlStatus_invalid) {
	dp->state = DELETE;
    }
    else {
	dp->state = ADD_MODIFY;
    }

    return NO_ERROR;
}

int
k_bufferControlEntry_set_defaults(dp)
    doList_t       *dp;
{

    bufferControlEntry_t *data = (bufferControlEntry_t *) dp->data;

    /*
     * if we're low on memory, don't allow the entry to be created
     */
    if (HeapLowMem())
	return RESOURCE_UNAVAILABLE_ERROR;

    /* set default values */
    data->bufferControlChannelIndex = 65535;	/* valid, but not likely */
    data->bufferControlFullStatus = D_bufferControlFullStatus_spaceAvailable;
    data->bufferControlFullAction = D_bufferControlFullAction_lockWhenFull;
    data->bufferControlCaptureSliceSize = 100;	/* from the mib */
    data->bufferControlDownloadSliceSize = 100;	/* from the mib */
    data->bufferControlDownloadOffset = 0;	/* from the mib */
    data->bufferControlMaxOctetsRequested = (-1);	/* from the mib */

    if ((data->bufferControlOwner = MakeOctetStringFromText("")) == 0) {
	return RESOURCE_UNAVAILABLE_ERROR;
    }

    data->bufferControlStatus = D_bufferControlStatus_createRequest;
    data->TimerId = -1;

    SET_ALL_VALID(data->valid);

    return NO_ERROR;
}

int
k_bufferControlEntry_set(data, contextInfo, function)
    bufferControlEntry_t *data;
    ContextInfo    *contextInfo;
    int             function;
{

    int             index;
    long            bufferControlMaxOctetsRequested;
    bufferControlEntry_t *new = NULL;

    /* find this entry in the table */
    bufferControlTable.tip[0].value.uint_val = data->bufferControlIndex;
    if ((index = SearchTable(&bufferControlTable, EXACT)) != -1) {
	new = (bufferControlEntry_t *) bufferControlTable.tp[index];
    }

    /* perform the table entry operation on it */
    if (data->bufferControlStatus == D_bufferControlStatus_invalid) {
	if (data->TimerId != -1) {
	    CancelEntryStatusTimeout((int) data->TimerId);
	}

	if (index == -1) {
	    return NO_ERROR;
	}
	else {
	    /* get rid of allocated buckets */
	    data->bufferControlMaxOctetsRequested = 0;
	    GetCaptureBuffers(data);

	    /* delete the table entry */
	    DeletebufferControlTableEntry(index);
	    return NO_ERROR;
	}
    }
    else if (index == -1) {
	/* add the entry */
	bufferControlTable.tip[0].value.uint_val = data->bufferControlIndex;
	if ((index = NewTableEntry(&bufferControlTable)) == -1) {
	    return GEN_ERROR;
	}
	new = (bufferControlEntry_t *) bufferControlTable.tp[index];
	name_memory(new, "RMON Buf Ctrl");

	/* set a timeout */
	if ((new->TimerId = SetEntryStatusTimeout(MAX_ROW_CREATION_TIME, (void *) new, bufferControlTableDeleteCallback)) == -1) {
	    DeletebufferControlTableEntry(index);
	    return GEN_ERROR;
	}
    }

    TransferEntries(I_bufferControlStatus,
		    bufferControlEntryTypeTable, (unsigned long *)new,
		    (unsigned long *)data);
    SET_ALL_VALID(new->valid);

    /* transfer private state data */
    new->active = data->active;
    if (new->bufferControlCaptureSliceSize == 0) {
	new->bufferSize = MAXCAPTURESLICE;
    }
    else {
	new->bufferSize = (int) new->bufferControlCaptureSliceSize;
    }
    new->bufferCount = data->bufferCount;
    new->oldest = data->oldest;
    new->newest = data->newest;
    new->freelist = data->freelist;
    new->currentIndex = data->currentIndex;

    /* determine whether to free the old buffer table */
    if (new->bufferSize != data->bufferSize && data->bufferSize != 0) {
	/* bufferControlCaptureSliceSize is being changed on the fly */
	bufferControlMaxOctetsRequested = new->bufferControlMaxOctetsRequested;
	new->bufferControlMaxOctetsRequested = 0;
	GetCaptureBuffers(new);
	new->bufferControlMaxOctetsRequested = bufferControlMaxOctetsRequested;
    }

    /* now attempt to allocate the new capture buffer table */
    GetCaptureBuffers(new);

    /* if the new row entry is now valid, cancel the creation timeout */
    if (new->bufferControlStatus == D_bufferControlStatus_valid && new->TimerId != -1) {
	CancelEntryStatusTimeout((int) new->TimerId);
	new->TimerId = -1;

	/* get the start time for this buffer control entry */
	new->bufferControlTurnOnTime = TimerCurTime();
	GET_TIMESTAMP(new->startTime);
    }
    else {
	if (new->bufferControlStatus == D_bufferControlStatus_createRequest) {
	    new->bufferControlStatus = D_bufferControlStatus_underCreation;
	}
    }

    return NO_ERROR;

}

#endif				/* SETS */

#ifdef SR_SNMPv2
int
bufferControlEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return UNDO_FAILED_ERROR;
}

#endif				/* SR_SNMPv2 */

captureBufferEntry_t *
k_captureBufferEntry_get(serialNum, contextInfo, nominator, searchType, captureBufferControlIndex, captureBufferIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            captureBufferControlIndex;
    long            captureBufferIndex;
{
    bufferControlEntry_t *bcp = NULL;
    captureBufferEntry_t *bp, *prevbp;
    captureBufferEntry_t *data;
    long            maxLen;
    int             index;

    /* repeat until we find an appropriate match, or there is no more data */
    data = NULL;
    do {
	/* get the history control entry */
	bufferControlTable.tip[0].value.uint_val = captureBufferControlIndex;
	if ((index = SearchTable(&bufferControlTable, searchType)) == -1) {
	    /* no such control table */
	    break;
	}
	bcp = (bufferControlEntry_t *) bufferControlTable.tp[index];

	/* search the history, newest first, for the entry */
	prevbp = 0;
	for (bp = bcp->newest; bp != 0; bp = bp->next) {
	    if (bp->captureBufferIndex <= captureBufferIndex) {
		break;
	    }
	    prevbp = bp;
	}

	/* check for an exact match */
	if (bp != 0) {
	    if (bp->captureBufferIndex == captureBufferIndex) {
		data = bp;
	    }
	}

	/* did we find it? */
	if (data == NULL && searchType == NEXT) {
	    if (prevbp != 0) {
		/* the preceding entry was the next larger */
		data = prevbp;
	    }
	    else {
		/* no entries, look in the next table */
		captureBufferControlIndex++;
		captureBufferIndex = 0;
	    }
	}
    } while (data == NULL && searchType == NEXT);

    /* if we got something, set up the octet string structure */
    if (data != NULL) {
	/* determine max number of bytes we can access */
	if (data->captureBufferPacketLength > bcp->bufferSize) {
	    maxLen = bcp->bufferSize;
	}
	else {
	    maxLen = data->captureBufferPacketLength;
	}

	/* see how much data is left to download */
	if (bcp->bufferControlDownloadOffset >= maxLen) {
	    /* nothing left to download */
	    data->captureBufferPacketData->length = 0;
	    data->captureBufferPacketData->octet_ptr = 0;
	}
	else {
	    data->captureBufferPacketData->length = maxLen -
		bcp->bufferControlDownloadOffset;
	    data->captureBufferPacketData->octet_ptr = data->data +
		bcp->bufferControlDownloadOffset;
	}

	/* make sure we don't exceed the download size */
	if (data->captureBufferPacketData->length
	    > bcp->bufferControlDownloadSliceSize) {
	    data->captureBufferPacketData->length =
		bcp->bufferControlDownloadSliceSize;
	}
    }

    /* return the data */
    return (data);
}

/******************** cisco supplied routines follow ********************/

/* printPacketData
 * print the contents of a packet NUM_LINES_PER_PRINTF lines at a time.
 * Each output line contains NUM_CHARS_PER_LINE of unformatted input.
 * Each byte of the unformatted input to printPacketData corresponds to 
 * BYTES_PER_UNFORMATTED_CHAR bytes of formatted output
 */

void printPacketData (bufferControlEntry_t *bufferp,
                      captureBufferEntry_t *capturep,
                      char *dest)
{
    OctetString tmp_src;
    char *src; 
    long len;
    long index = 0;
    const long input_per_printf = NUM_LINES_PER_PRINTF * 
                                  UNFORMATTED_CHARS_PER_LINE;

    /* Calculate total octets of the packet saved (off the wire) */
    if (capturep->captureBufferPacketLength > bufferp->bufferSize) {
        len = bufferp->bufferSize;
    } else {
        len = capturep->captureBufferPacketLength;
    }

    /* point to the beginning of captured data */
    src = capturep->data;

    /* 
     * len contains number of available unprinted input octets in the packet.
     * print a maximum of "input_per_printf" octets at a time
     */
    while (len > 0) {
        /* compute start and length of input octets to be printed this time */
        tmp_src.octet_ptr = &src[index];
        tmp_src.length = (len < input_per_printf) ? len : input_per_printf;

        /* store formatted output in dest */
        SPrintOctetString(&tmp_src, UNFORMATTED_CHARS_PER_LINE, dest);

        /*
         * If this is the last set of octets being printed, 
         * the number of octets left in tmp_src may be <= 8,
         * SPrintOctetString does not print the octets on a new line
         * when the number of input octets is <= 8.
         * Test for this condition and insert a "\n" if necessary.
         */
        if (tmp_src.length > 8) {
            printf("%s",dest);
        } else {
            printf("\n%s", dest);
        }

        /* update index and len to reflect new offset and remaining input */
        index += tmp_src.length;
        len -= input_per_printf;
    }
}

/*
 * show rmon capture
 * output a string similiar to the following for every set of captured packets
 *    Buffer 12345 is valid, and owned by foo
 *       Captured data is from channel xxxxxx
 *       Slice size is xxxxxx, download size is xxxxxx
 *       Download offset is xxxxxx, 
 *       Full Satus is spaceAvailable, full action is lockWhenFull
 *       Granted xxxxxxxx octets out of xxxxxxxxx requested
 *       Buffer has been on since xxxxxx, and has captured xxxxxx packets
 *         Packet xxxxxxx was captured xxxxxxx ms since buffer was turned on
 *         It's length is xxxxxx octets and has a status type of xx
 *         Packet ID is xxxxxxx, and contains the following:
 *          xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
 */

void
show_rmon_capture (void)
{
    bufferControlEntry_t   *bufferp;
    captureBufferEntry_t   *capturep;
    int             i;
    OctetString    *tempOS;
    char           *owner, *buffer;

    automore_enable(NULL);
    if (bufferControlTable.nitems == 0) {
        printf("\nBuffer Control table is empty");
        automore_disable();
        return;
    }

    /* 
     * Allocate memory for printing the packet contents 
     * We only print BUFFER_SIZE bytes of formatted output
     * per printf invocation, so we do not need to know the
     * exact size of the captureBufferPacketData
     * Note that the space for "\n" or "\0" is already
     * accounted for in the macro BYTES_PER_UNFORMATTED_CHAR
     * which is used in computing BUFFER_SIZE
     */
    buffer = (char *) malloc(BUFFER_SIZE);
    if (!buffer) {
        automore_disable();
        return;
    }


    for (i = 0; i < bufferControlTable.nitems; i++) {
        /*
         * If the user terminates show_rmon_capture by typing "q",
         * relinquish CPU immediately
         */
        if (automore_quit()) {
            free(buffer);
            automore_disable();
            return;
        }

        bufferp = bufferControlTable.tp[i];
        if (bufferp == 0 ||
	    bufferp->bufferControlStatus != D_bufferControlStatus_valid)
	    continue;
        mem_lock(bufferp);

        /* turn the owner string from an octet string to a real string */
        owner = malloc(bufferp->bufferControlOwner->length + 1);
        if (owner) {
            tempOS = bufferp->bufferControlOwner;
            memcpy(owner, tempOS->octet_ptr, tempOS->length);
        } 
	else {
            free(buffer);
            automore_disable();
	    return;
        }

        printf("\nBuffer %d is %s, owned by %s",
               bufferp->bufferControlIndex,
               rmon_status_string(bufferp->bufferControlStatus),
               owner ? owner : "unknown");
        printf("\n Captured data is from channel %d",
	       bufferp->bufferControlChannelIndex);
        printf("\n Slice size is %d, download size is %d",
	       bufferp->bufferControlCaptureSliceSize,
	       bufferp->bufferControlDownloadSliceSize); 
        printf("\n Download offset is %d",
	       bufferp->bufferControlDownloadOffset); 
        printf("\n Full Status is %s, full action is %s",
	       bufferp->bufferControlFullStatus == 1 
                                   ? "spaceAvailable" : "full",
               bufferp->bufferControlFullAction == 1
                                   ? "lockWhenFull" : "wrapWhenFull");
        printf("\n Granted %d octets out of %d requested",
	       bufferp->bufferControlMaxOctetsGranted,
	       bufferp->bufferControlMaxOctetsRequested);  
        printf("\n Buffer has been on since %TC, and has captured %d packets",
	       bufferp->bufferControlTurnOnTime,
	       bufferp->bufferControlCapturedPackets);

        free(owner);

        if (bufferp->oldest) {
            printf("\n  Current capture buffer entries:");
            for (capturep = bufferp->oldest; capturep; 
                 capturep = capturep->prev) {
                /*
                 * If the user terminates show_rmon_capture by typing "q",
                 * relinquish CPU immediately.
                 */
                if (automore_quit()) {
                    free(bufferp);
                    free(buffer);
                    automore_disable();
                    return;
                }

                mem_lock(capturep);

                printf("\n   Packet %d was captured %d ms ",
                       capturep->captureBufferIndex,
                       capturep->captureBufferPacketTime);
                printf("since buffer was turned on");
                printf("\n   It's length is %d octets ",
                       capturep->captureBufferPacketLength);
                printf("and has a status type of %d",
                       capturep->captureBufferPacketStatus); 
                printf("\n   Packet ID is %d, ",
                       capturep->captureBufferPacketID);
                printf("and contains the following data:");
                printPacketData(bufferp, capturep, buffer);
                free(capturep);
            }
        free(bufferp);
        }
    }
    free(buffer);
    automore_disable();
}
