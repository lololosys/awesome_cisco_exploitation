/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_k_topn.c,v 3.5.12.1 1996/05/17 21:26:34 anke Exp $
 * $Source: /release/112/cvs/Xsys/rmon/sr_k_topn.c,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_k_topn.c,v $
 * Revision 3.5.12.1  1996/05/17  21:26:34  anke
 * CSCdi48524:  Need mechanism to display RMON tables
 *              Fill in guts of show rmon commands
 * Branch: California_branch
 *
 * Revision 3.5  1996/02/19  06:06:05  jjohnson
 * CSCdi48853:  rmon can consume all available memory
 * As an interim measure, don't allow rmon to consume memory once the
 * heap's low-water mark has been reached.  While we're at it, name
 * all the malloc'ed memory so that we can tell what is used where
 *
 * Revision 3.4  1996/02/09  07:57:30  jjohnson
 * CSCdi48524:  Need mechanism to display RMON tables
 * parse chains are in place.  checkpoint work so that anke can take over.
 *
 * Revision 3.3  1996/01/17  06:47:05  jjohnson
 * CSCdi46612:  Router crashes while using NETscout for RMON
 * When deleting a hostTopNControlEntry, make sure that any outstanding
 * timer is cancelled.
 *
 * Revision 3.2  1995/11/17  18:39:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:22  shaker
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

/* global data describing the hostTopNControlTable entries */
static SnmpType        hostTopNControlEntryTypeTable[] = {
    {INTEGER_TYPE, SR_READ_ONLY, 0},	/* hostTopNControlIndex */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* hostTopNHostIndex */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* hostTopNRateBase */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* hostTopNTimeRemaining */
    {INTEGER_TYPE, SR_READ_ONLY, -1},	/* hostTopNDuration */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* hostTopNRequestedSize */
    {INTEGER_TYPE, SR_READ_ONLY, -1},	/* hostTopNGrantedSize */
    {TIME_TICKS_TYPE, SR_READ_ONLY, -1},	/* hostTopNStartTime */
    {OCTET_PRIM_TYPE, SR_READ_WRITE, -1},	/* hostTopNOwner */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* hostTopNStatus */
};

static Index_t         hostTopNControlEntryIndex[1];
static RmonTable       hostTopNControlTable;

void DeletehostTopNControlTableEntry SR_PROTOTYPE((int index));

static char *GetTopNRateBaseStr(int);

/* this routine returns a pointer to a valid host control entry */
hostControlEntry_t *
GetValidHCP(index)
    long            index;
{
    int             hostindex;
    hostControlEntry_t *hcp;
    extern RmonTable hostControlTable;

    /* get the host control entry */
    hostControlTable.tip[0].value.uint_val = index;
    if ((hostindex = SearchTable(&hostControlTable, EXACT)) == -1) {
	/* no such control table */
	hcp = 0;
    }
    else {
	/* get a pointer to the entry */
	hcp = (hostControlEntry_t *) hostControlTable.tp[hostindex];
	if (hcp->hostControlStatus != D_hostControlStatus_valid) {
	    /* not a valid entry */
	    hcp = 0;
	}
    }

    /* return the pointer */
    return (hcp);
}

/* this routine gets top n buffers for a particular topn control entry,
 * and sets the granted value appropriately */
void
GetTopNTable(tncp)
    hostTopNControlEntry_t *tncp;
{
    long            i, count, table_size;
    hostTopNEntry_t *new, *tnp, **newtntp;
    unsigned char   dummyaddr[ETHERMACADDRLEN];

    /* determine whether to add or subtract buckets */
    count = tncp->hostTopNRequestedSize - tncp->hostTopNGrantedSize;
    if (count > 0) {

	/* don't proceed any further if we're low on memory */
	if (HeapLowMem())
	    return;

	/* allocate a new array of pointers, and install it */
	table_size = sizeof(hostTopNEntry_t *) * tncp->hostTopNRequestedSize;
	newtntp = malloc_named(table_size, "RMON TopN Ptrs");
	if (newtntp == 0) {
	    /* couldn't allocate the array */
	    return;
	}
	memcpy(newtntp, tncp->tntp,
	       (int) tncp->hostTopNGrantedSize * sizeof(hostTopNEntry_t *));
	if (tncp->tntp != 0)
	    free(tncp->tntp);
	tncp->tntp = newtntp;

	/* start allocating entries */
	for (i = 0; i < count; i++) {
	    new = malloc_named(sizeof(hostTopNEntry_t), "RMON TopN Data");
	    if (new == 0) {
		break;
	    }
	    new->validData = 0;
	    new->hostTopNReport = tncp->hostTopNControlIndex;

	    /* make a place holder for the mac address */
	    new->hostTopNAddress = MakeOctetString(dummyaddr, ETHERMACADDRLEN);
	    if (new->hostTopNAddress == 0) {
		free(new);
		break;
	    }

	    /* add it to the list */
	    tncp->tntp[tncp->hostTopNGrantedSize] = new;

	    /* update the granted count */
	    tncp->hostTopNGrantedSize++;
	    new->hostTopNIndex = tncp->hostTopNGrantedSize;
	}
    }
    else if (count < 0) {
	/* convert to a positive count */
	count = 0 - count;

	for (i = 0; i < count; i++) {
	    tncp->hostTopNGrantedSize--;
	    tnp = tncp->tntp[tncp->hostTopNGrantedSize];
	    tncp->tntp[tncp->hostTopNGrantedSize] = 0;
	    FreeOctetString(tnp->hostTopNAddress);
	    free(tnp);
	}

    }
}

/* this routine clears all of the validData fields in the topN table, so that
 * a new aquisition can start with a clean slate. */
void
ClearTopNValidData(tncp)
    hostTopNControlEntry_t *tncp;
{
    long            i;

    /* clear the validData fields of the table */
    for (i = 0; i < tncp->hostTopNGrantedSize; i++) {
	tncp->tntp[i]->validData = 0;
    }
}


/* this routine frees the copy of the host table associated with a topn
 * control entry */
void
FreeTopNHostTable(tncp)
    hostTopNControlEntry_t *tncp;
{
    long            i;

    /* make sure there is something to free */
    if (tncp->numhosts != 0) {
	/* free all entries */
	for (i = 0; i < tncp->numhosts; i++) {
	    HostEntry_free(tncp->htp[i]);
	}

	/* free the table */
	free(tncp->htp);

	/* clear the counter */
	tncp->numhosts = 0;
    }
}

/* make a copy of the host table associated with this topn table entry */
int
CloneHostTable(tncp)
    hostTopNControlEntry_t *tncp;
{
    int             i, status = 0;
    hostControlEntry_t *hcp;
    hostEntry_t    *new;

    /* get the host control index for this top n table */
    if ((hcp = GetValidHCP(tncp->hostTopNHostIndex)) != 0) {
	/* try to copy the host table */
	if ((tncp->htp = (hostEntry_t **)
	     malloc_named(
	         sizeof(hostEntry_t *) * (int) hcp->hostControlTableSize,
	         "RMON TNH Ptrs"))
	    != 0) {
	    /* copy the table id */
	    tncp->hostTableId = hcp->tableid;

	    /* copy as many entries as possible */
	    tncp->numhosts = 0;
	    for (i = 0; i < hcp->hostControlTableSize; i++) {
		/* copy the entry */
		new = malloc_named(sizeof(hostEntry_t), "RMON TNH Data");
		if (new == 0) {
		    break;
		}
		memcpy(new, hcp->htp[i], sizeof(hostEntry_t));

		/* clone the address field */
		if ((new->hostAddress =
		     CloneOctetString(hcp->htp[i]->hostTimeAddress)) == 0) {
		    /* out of memory */
		    free(new);
		    break;
		}

		/* insert the new entry */
		tncp->htp[i] = new;
		tncp->numhosts++;
		status = 1;	/* got at least some hosts */
	    }
	}
    }

    /* return the status */
    return (status);
}

/* this routine returns the rate value for this entry */
unsigned long
GetHostEntryRate(base, hep)
    long            base;
    hostEntry_t    *hep;
{
    unsigned long   rate;

    switch (base) {
      case D_hostTopNRateBase_hostTopNInPkts:
	rate = hep->hostInPkts;
	break;
      case D_hostTopNRateBase_hostTopNOutPkts:
	rate = hep->hostOutPkts;
	break;
      case D_hostTopNRateBase_hostTopNInOctets:
	rate = hep->hostInOctets;
	break;
      case D_hostTopNRateBase_hostTopNOutOctets:
	rate = hep->hostOutOctets;
	break;
      case D_hostTopNRateBase_hostTopNOutErrors:
	rate = hep->hostOutErrors;
	break;
      case D_hostTopNRateBase_hostTopNOutBroadcastPkts:
	rate = hep->hostOutBroadcastPkts;
	break;
      default:			/* must be
				 * D_hostTopNRateBase_hostTopNOutMulticastPkts
				 *  */
	rate = hep->hostOutMulticastPkts;
	break;
    }

    return (rate);
}

/* this routine implements the topn processing. */
void
TopNCallback(tdp)
    TimeOutDescriptor *tdp;
{
    int             i, j, maxindex;
    hostTopNControlEntry_t *tncp;
    hostControlEntry_t *hcp;
    hostTopNEntry_t *tnp;
    hostEntry_t    *hep, *clonedhep = NULL;
    unsigned long   rate, elapsed;

    /*
     * get a pointer to the topn control entry that registered this timer,
     * and free the associated data
     */
    tncp = (hostTopNControlEntry_t *) tdp->UserData2;
    free(tdp);
    tncp->topNCallbackTimer = 0;

    /* update the elapsed clock */
    elapsed = (TimerCurTime() - tncp->lastCheck) / 100;
    tncp->hostTopNTimeRemaining = 0;
    tncp->hostTopNDuration += elapsed;

    /* start with a valid index value in case we jump to "done" */
    i = 0;

    /* sanity check -- get a pointer to the host control entry */
    if ((hcp = GetValidHCP(tncp->hostTopNHostIndex)) == 0
	|| hcp->tableid != tncp->hostTableId) {
	/* no such host table, any more */
	goto done;
    }

    /*
     * assign values for all of the host entries in the host table.  Use the
     * topNRate field for a scratch area in the hosts table
     */
    for (i = 0; i < hcp->hostControlTableSize; i++) {
	/* get a pointer to the host entry */
	hep = (hostEntry_t *) hcp->rtp->tp[i];
	hep->topNTaken = 0;

	/* look for a match with our cached data */
	rate = 0;
	for (j = 0; j < tncp->numhosts; j++) {
	    clonedhep = tncp->htp[j];
	    if (CmpOctetStrings(hep->hostAddress, clonedhep->hostAddress) == 0) {
		/* got a match */
		rate = GetHostEntryRate(tncp->hostTopNRateBase, clonedhep);
		break;
	    }
	}

	/*
	 * calculate the rate, factoring in the cached value.  If the cached
	 * value is greater than the current value, this host must have been
	 * deleted (via LRU) and then re-entered.  Use the current rate in
	 * that case.
	 */
	hep->topNRate = GetHostEntryRate(tncp->hostTopNRateBase, hep);
	if (rate < hep->topNRate)
	    hep->topNRate -= rate;
    }

    /* determine how many topN values to find */
    if (hcp->hostControlTableSize > tncp->hostTopNGrantedSize) {
	maxindex = (int) tncp->hostTopNGrantedSize;
    }
    else {
	maxindex = (int) hcp->hostControlTableSize;
    }

    /* now prioritize the topN rates */
    for (i = 0; i < maxindex; i++) {
	/* look for the best non-matched rate */
	rate = 0;
	for (j = 0; j < hcp->hostControlTableSize; j++) {
	    /* get a pointer to the host entry */
	    hep = (hostEntry_t *) hcp->rtp->tp[j];

	    /* if this is a new or improved match, remember it */
	    if (hep->topNRate >= rate && hep->topNTaken == 0) {
		rate = hep->topNRate;
		clonedhep = hep;
	    }
	}

	/* if no more entries had a count for the statistic, get out */
	if (rate == 0)
	    break;

	/* install the new value */
	tnp = tncp->tntp[i];
	tnp->hostTopNRate = clonedhep->topNRate;
	memcpy(tnp->hostTopNAddress->octet_ptr,
	       clonedhep->hostAddress->octet_ptr, ETHERMACADDRLEN);
	tnp->validData = 1;

	/* indicate that this entry has been taken */
	clonedhep->topNTaken = 1;
    }

done:
    /* mark unused topN entries as invalid */
    while (i < tncp->hostTopNGrantedSize) {
	tnp = tncp->tntp[i];
	tnp->validData = 0;
	i++;
    }

    /* free the cloned host table */
    FreeTopNHostTable(tncp);
}

/* this routine cancels a timer currently waiting to go off */
void
CancelTopNTimer(tncp)
    hostTopNControlEntry_t *tncp;
{
    TimeOutDescriptor *tdp;

    /* make sure the timer is set */
    if (tncp->topNCallbackTimer != 0) {
	tdp = (TimeOutDescriptor *) tncp->topNCallbackTimer;
	if (tdp->TimerId != -1) {
	    /* cancel the timer */
	    CancelTimeout((int) tdp->TimerId);
	}
	free(tdp);
	tncp->topNCallbackTimer = 0;
    }

}

/* this routine sets a timer to go off when the report is ready */
void           *
SetTopNTimer(tncp)
    hostTopNControlEntry_t *tncp;
{
    TimeOutDescriptor *tdp;

    /* allocate data for the new timer */
    tdp = malloc_named(sizeof(TimeOutDescriptor), "RMON TopN Timer");
    if (tdp) {
	/* fill in the descriptor */
	tdp->CallBack = TopNCallback;
	tdp->UserData2 = (void *) tncp;

	/* set the timer */
	if ((tdp->TimerId = SetOneshotTimeout(TimerCurTime() +
			  (unsigned long) tncp->hostTopNTimeRemaining * 100,
					      (void *) tdp)) == (-1)) {
	    /* free the descriptor */
	    free(tdp);
	    tdp = 0;
	}

	/* record the time */
	tncp->hostTopNStartTime = TimerCurTime();
	tncp->lastCheck = tncp->hostTopNStartTime;
    }

    return ((void *) tdp);
}

/* --------------------- MIB COMPILER GENERATED ------------------------- */

int
k_hostTopNControlEntry_initialize()
{
    hostTopNControlEntryIndex[0].offset = I_hostTopNControlIndex;
    hostTopNControlEntryIndex[0].type = T_uint;

    hostTopNControlTable.nindices = 1;
    hostTopNControlTable.tip = hostTopNControlEntryIndex;
    hostTopNControlTable.nitems = 0;
    hostTopNControlTable.rowsize = sizeof(hostTopNControlEntry_t);
    if ((hostTopNControlTable.tp = (void **) malloc(sizeof(hostTopNControlEntry_t *))) == NULL) {
	return 0;
    }

    return 1;
}

int
k_hostTopNControlEntry_terminate()
{
    while (hostTopNControlTable.nitems > 0) {
	DeletehostTopNControlTableEntry(0);
    }

    free(hostTopNControlTable.tp);
    return 1;
}

/* This routine is called by the timeout code to
 * delete a pending creation of a Table entry */
void
hostTopNControlTableDeleteCallback(tdp)
    TimeOutDescriptor *tdp;
{
    hostTopNControlEntry_t *data;

    /* dummy up an entry to delete */
    data = (hostTopNControlEntry_t *) tdp->UserData2;
    data->hostTopNStatus = D_hostTopNStatus_invalid;
    data->TimerId = -1;

    /* free the timeout descriptor */
    free(tdp);

    /* call the set method */
    k_hostTopNControlEntry_set(data, (ContextInfo *) NULL, 0);
}

/* This routine deletes an entry from the hostTopNControlTable */
void
DeletehostTopNControlTableEntry(index)
    int             index;
{
    hostTopNControlEntry_t *data;

    /* get a pointer to the old entry */
    data = (hostTopNControlEntry_t *) hostTopNControlTable.tp[index];
    data->hostTopNRequestedSize = 0;

    /* cancel any outstanding timer */
    CancelTopNTimer(data);

    /* free the topn table */
    GetTopNTable(data);

    /* free the host table copy */
    FreeTopNHostTable(data);

    /* free the old entry and remove it from the table */
    hostTopNControlEntry_free(data);
    RemoveTableEntry(&hostTopNControlTable, index);
}

/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_hostTopNControlEntryFreeUserpartData(data)
    hostTopNControlEntry_t *data;
{
    /* nothing to free by default */
}

hostTopNControlEntry_t *
k_hostTopNControlEntry_get(serialNum, contextInfo, nominator, searchType, hostTopNControlIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            hostTopNControlIndex;
{
    int             index;
    unsigned long   elapsed, now;
    hostTopNControlEntry_t *data;

    hostTopNControlTable.tip[0].value.uint_val = hostTopNControlIndex;
    if ((index = SearchTable(&hostTopNControlTable, searchType)) == -1) {
	return NULL;
    }
    data = (hostTopNControlEntry_t *) hostTopNControlTable.tp[index];

    /*
     * adjust the time remaining if a timer has actually been set and this is
     * a valid entry.
     */
    if (data->hostTopNStatus == D_hostTopNStatus_valid
	&& data->hostTopNTimeRemaining != 0
	&& data->topNCallbackTimer != 0) {
	/* get the elapsed time since the timer was started */
	now = TimerCurTime();
	elapsed = (now - data->lastCheck) / 100;
	data->lastCheck = now;
	data->hostTopNDuration += elapsed;
	data->hostTopNTimeRemaining -= elapsed;
	if (data->hostTopNTimeRemaining < 0)
	    data->hostTopNTimeRemaining = 0;
    }

    return (data);

}

#ifdef SETS
int
k_hostTopNControlEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

#ifdef SR_SNMPv2
    hostTopNControlEntry_t *undodata = (hostTopNControlEntry_t *) dp->undodata;
#else				/* SR_SNMPv2 */
    hostTopNControlEntry_t *undodata = NULL;
#endif				/* SR_SNMPv2 */
    int             error_status = NO_ERROR;

    /*
     * make sure that the values for duration and time remaining are not
     * negative
     */
    switch (object->nominator) {
      case I_hostTopNTimeRemaining:
      case I_hostTopNRequestedSize:
	if (value->sl_value < 0) {
	    /* this value can't be negative */
	    error_status = INCONSISTENT_VALUE_ERROR;
	}
	break;
    }

    /*
     * make sure that the data source isn't being changed if this is a valid
     * entry
     */
    if (undodata != 0 && undodata->hostTopNStatus == D_hostTopNStatus_valid) {
	switch (object->nominator) {
	  case I_hostTopNHostIndex:
	    if (value->sl_value != undodata->hostTopNHostIndex) {
		error_status = INCONSISTENT_VALUE_ERROR;
	    }
	  case I_hostTopNRateBase:
	    if (value->sl_value != undodata->hostTopNRateBase) {
		error_status = INCONSISTENT_VALUE_ERROR;
	    }
	    break;
	}
    }

    return error_status;
}

int
k_hostTopNControlEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    hostTopNControlEntry_t *data = (hostTopNControlEntry_t *) dp->data;

#ifdef SR_SNMPv2
    hostTopNControlEntry_t *undodata = (hostTopNControlEntry_t *) dp->undodata;
#else				/* SR_SNMPv2 */
    hostTopNControlEntry_t *undodata = NULL;
#endif				/* SR_SNMPv2 */

    if (data->hostTopNStatus == D_hostTopNStatus_invalid) {
	dp->state = DELETE;
    }
    else {
	dp->state = ADD_MODIFY;

	/* if setting to valid, make sure we have all the information we need */
	if (data->hostTopNStatus == D_hostTopNStatus_valid) {
	    if (!VALID(I_hostTopNHostIndex, data->valid)
		|| GetValidHCP(data->hostTopNHostIndex) == 0) {
		/* can't do it, don't have a valid host control entry */
		dp->state = UNKNOWN;
	    }

	    /* check to see if we are starting a new report */
	    if (data->hostTopNTimeRemaining != 0) {
		if (undodata == 0
		    || data->hostTopNTimeRemaining
		    != undodata->hostTopNTimeRemaining
		    || data->hostTopNStatus != undodata->hostTopNStatus) {
		    /*
		     * we are setting hostTopNTimeRemaining to a new value,
		     * or we are "starting the clock" by setting the
		     * entryStatus to valid.  Either way, this is a new
		     * report.
		     */
		    data->newReport = 1;
		}
		else {
		    /*
		     * hostTopNTimeRemaining is nonzero, but we're not
		     * starting a new report
		     */
		    data->newReport = 0;
		}
	    }
	}
    }

    return NO_ERROR;
}

int
k_hostTopNControlEntry_set_defaults(dp)
    doList_t       *dp;
{

    hostTopNControlEntry_t *data = (hostTopNControlEntry_t *) dp->data;

    /*
     * if we're low on memory, don't allow the entry to be created
     */
    if (HeapLowMem())
	return RESOURCE_UNAVAILABLE_ERROR;

    /* set defaults required by the mib */
    data->hostTopNHostIndex = 65535;	/* valid but unlikely */
    data->hostTopNTimeRemaining = 0;
    data->hostTopNRequestedSize = 10;
    data->hostTopNRateBase = D_hostTopNRateBase_hostTopNInPkts;

    if ((data->hostTopNOwner = MakeOctetStringFromText("")) == 0) {
	return RESOURCE_UNAVAILABLE_ERROR;
    }

    data->hostTopNStatus = D_hostTopNStatus_createRequest;
    data->TimerId = -1;
    data->topNCallbackTimer = 0;

    SET_ALL_VALID(data->valid);

    return NO_ERROR;
}

int
k_hostTopNControlEntry_set(data, contextInfo, function)
    hostTopNControlEntry_t *data;
    ContextInfo    *contextInfo;
    int             function;
{

    int             index;
    hostTopNControlEntry_t *new = NULL;

    /* find this entry in the table */
    hostTopNControlTable.tip[0].value.uint_val = data->hostTopNControlIndex;
    if ((index = SearchTable(&hostTopNControlTable, EXACT)) != -1) {
	new = (hostTopNControlEntry_t *) hostTopNControlTable.tp[index];
    }

    /* perform the table entry operation on it */
    if (data->hostTopNStatus == D_hostTopNStatus_invalid) {
	if (data->TimerId != -1) {
	    CancelEntryStatusTimeout((int) data->TimerId);
	}

	if (index == -1) {
	    return NO_ERROR;
	}
	else {
	    /* delete the table entry */
	    DeletehostTopNControlTableEntry(index);
	    return NO_ERROR;
	}
    }
    else if (index == -1) {
	/* add the entry */
	hostTopNControlTable.tip[0].value.uint_val = data->hostTopNControlIndex;
	if ((index = NewTableEntry(&hostTopNControlTable)) == -1) {
	    return GEN_ERROR;
	}
	new = (hostTopNControlEntry_t *) hostTopNControlTable.tp[index];
	name_memory(new, "RMON TopN Ctrl");

	/* set a timeout */
	if ((new->TimerId = SetEntryStatusTimeout(MAX_ROW_CREATION_TIME, (void *) new, hostTopNControlTableDeleteCallback)) == -1) {
	    DeletehostTopNControlTableEntry(index);
	    return GEN_ERROR;
	}
    }

    TransferEntries(I_hostTopNStatus, hostTopNControlEntryTypeTable,
		    (unsigned long *)new, (unsigned long *)data);
    SET_ALL_VALID(new->valid);

    /* transfer private state data */
    new->tntp = data->tntp;
    new->htp = data->htp;
    new->numhosts = data->numhosts;
    new->newReport = data->newReport;
    new->topNCallbackTimer = data->topNCallbackTimer;

    /* update the top n table count */
    GetTopNTable(new);

    /* if the new row entry is now valid, cancel the creation timeout */
    if (new->hostTopNStatus == D_hostTopNStatus_valid) {
	if (new->TimerId != -1) {
	    CancelEntryStatusTimeout((int) new->TimerId);
	    new->TimerId = -1;
	}

	/* if we're starting a new report, try to clone the host table */
	if (new->newReport != 0) {
	    new->newReport = 0;	/* clear the flag */
	    new->hostTopNDuration = 0;
	    FreeTopNHostTable(new);
	    CancelTopNTimer(new);
	    ClearTopNValidData(new);

	    /* now clone the host table and set the callback timer */
	    if (CloneHostTable(new) == 0
		|| (new->topNCallbackTimer = SetTopNTimer(new)) == 0) {
		/* couldn't do it */
		DeletehostTopNControlTableEntry(index);
	    }
	}
    }
    else {
	if (new->hostTopNStatus == D_hostTopNStatus_createRequest) {
	    new->hostTopNStatus = D_hostTopNStatus_underCreation;
	}
    }

    return NO_ERROR;

}

#endif				/* SETS */

#ifdef SR_SNMPv2
int
hostTopNControlEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    return UNDO_FAILED_ERROR;
}

#endif				/* SR_SNMPv2 */

hostTopNEntry_t *
k_hostTopNEntry_get(serialNum, contextInfo, nominator, searchType, hostTopNReport, hostTopNIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            hostTopNReport;
    long            hostTopNIndex;
{
    hostTopNControlEntry_t *tncp;
    hostTopNEntry_t *data;
    int             topnindex;

    /* repeat until we find an appropriate match, or there is no more data */
    data = NULL;
    do {
	/* get the host control entry */
	hostTopNControlTable.tip[0].value.uint_val = hostTopNReport;
	if ((topnindex = SearchTable(&hostTopNControlTable, searchType))
	    == -1) {
	    /* no such control table */
	    break;
	}
	tncp = (hostTopNControlEntry_t *) hostTopNControlTable.tp[topnindex];

	/*
	 * does the proper entry exist in the hostTopNControlTable?  If so,
	 * make sure that the report has been collected.
	 */
	if (tncp->hostTopNTimeRemaining == 0
	    && hostTopNIndex <= tncp->hostTopNGrantedSize) {
	    /* yes, get a pointer to the entry */
	    data = tncp->tntp[hostTopNIndex - 1];
	    if (data->validData == 0) {
		/* not an entry with valid data */
		data = 0;
	    }
	}

	/* did we find it? */
	if (data == NULL && searchType == NEXT) {
	    /* no entries, look in the next table */
	    hostTopNReport++;
	    hostTopNIndex = 1;
	}
    } while (data == NULL && searchType == NEXT);

    /* return the data */
    return (data);
}

/******************** cisco supplied routines follow ********************/

/*
 * show rmon topn
 * output a string similiar to the following for every top # of hosts
 *    Host Entry 12345 of report 12345 is valid, owned by foo
 *       The rate of change is xxxxxx based on xxxxxxxxxxxx
 *       This report was last started at xxxxxxxx
 *       Time remaining in this report is xxxxxxxxx out of xxxxxx
 *       Hosts physical address is xxxxxxxxxxxxxxxxxxxx
 *       Requested # of hosts: xxxxxx, # of hosts granted: xxxxxx
 *        Report # xxxxx of Top N hosts entry xxxxxx is recording
 *        Host foobar at a rate of xxxxx
 */
                    
void
show_rmon_topn (void)
{
    hostTopNControlEntry_t   *hostTopNControlp;
    hostTopNEntry_t *hostTopNp;
    int             i, j;
    OctetString    *tempOS;
    char	   *owner;
    char           *topnaddr;
    char           *htnaddr;

    automore_enable(NULL);
    if (hostTopNControlTable.nitems == 0) {
	printf("\nHost Top N Control table is empty");
	automore_disable();
	return;
    }

    for (i = 0; i < hostTopNControlTable.nitems; i++) {
        hostTopNControlp = hostTopNControlTable.tp[i]; 
	mem_lock(hostTopNControlp);

	/* turn the owner string from an octet string to a real string */
	owner = malloc(hostTopNControlp->hostTopNOwner->length + 1);
	if (owner) {
	    tempOS = hostTopNControlp->hostTopNOwner;
	    memcpy(owner, tempOS->octet_ptr, tempOS->length);
	}
        else
	    return;

	/* turn host address string from an octet string to a real string */
	topnaddr = malloc(hostTopNControlp->hostTopNOwner->length + 1);
	if (topnaddr) {
	    tempOS = hostTopNControlp->hostTopNOwner;
	    memcpy(topnaddr, tempOS->octet_ptr, tempOS->length);
	}
	else
	    return;

        printf("\nHost Entry %d of report %d is %s, owned by %s",
	       hostTopNControlp->hostTopNControlIndex,
	       hostTopNControlp->hostTopNHostIndex,
	       rmon_status_string(hostTopNControlp->hostTopNStatus),
	       owner ? owner : "unknown");
        printf("\n The rate of change is based on %s",
	       GetTopNRateBaseStr(hostTopNControlp->hostTopNRateBase));
        printf("\n This report was last started at %TC",
	       hostTopNControlp->hostTopNStartTime); 
        printf("\n Time remaining in this report is %d out of %d",
	       hostTopNControlp->hostTopNTimeRemaining,
	       hostTopNControlp->hostTopNDuration);
        printf("\n Hosts physical address is %e", topnaddr);
        printf("\n Requested # of hosts: %d, # of hosts granted: %d",
	       hostTopNControlp->hostTopNRequestedSize,
	       hostTopNControlp->hostTopNGrantedSize);

	free(owner);
        free(topnaddr);

	for (j = 0; j < hostTopNControlp->numhosts; j++) {
	    hostTopNp = (hostTopNEntry_t *)hostTopNControlp->htp[j];

	    /* turn the address string from an octet string to a real string */
	    htnaddr = malloc(hostTopNp->hostTopNAddress->length + 1);
	    if (htnaddr) {
		tempOS = hostTopNp->hostTopNAddress;
		memcpy(htnaddr, tempOS->octet_ptr, tempOS->length);
	    }
	    else
		return;

	    printf("\nReport # %d of Top N hosts entry %d is recording", 
		   hostTopNp->hostTopNReport, hostTopNp->hostTopNIndex);
	    printf("\nHost %s at a rate of %d", htnaddr, 
		   hostTopNp->hostTopNRate);

	    free(htnaddr);
	}
	free(hostTopNControlp);
    }
    automore_disable();
}

static char *
GetTopNRateBaseStr (base)
    int            base;
{

    switch (base) {
      case D_hostTopNRateBase_hostTopNInPkts:
	return("hostTopNInPkts");
	break;
      case D_hostTopNRateBase_hostTopNOutPkts:
	return("hostTopNOutPkts");
	break;
      case D_hostTopNRateBase_hostTopNInOctets:
	return("hostTopNInOctets");
	break;
      case D_hostTopNRateBase_hostTopNOutOctets:
	return("hostTopNOutOctets");
	break;
      case D_hostTopNRateBase_hostTopNOutErrors:
	return("hostTopNOutErrors");
	break;
      case D_hostTopNRateBase_hostTopNOutBroadcastPkts:
	return("hostTopNOutBroadcastPkts");
	break;
      case D_hostTopNRateBase_hostTopNOutMulticastPkts:
	return("hostTopNOutMulticastPkts");
        break;
      default:
        return("Unknown type");
    }
}
