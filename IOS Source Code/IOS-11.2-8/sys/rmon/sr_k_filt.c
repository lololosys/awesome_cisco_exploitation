/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_k_filt.c,v 3.4.12.2 1996/07/04 17:03:15 sdurham Exp $
 * $Source: /release/112/cvs/Xsys/rmon/sr_k_filt.c,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_k_filt.c,v $
 * Revision 3.4.12.2  1996/07/04  17:03:15  sdurham
 * CSCdi61957:  show rmon filter can cause core dump
 * Branch: California_branch
 * search for correct channel to associate with filter on show command
 *
 * Revision 3.4.12.1  1996/05/17  21:26:28  anke
 * CSCdi48524:  Need mechanism to display RMON tables
 *              Fill in guts of show rmon commands
 * Branch: California_branch
 *
 * Revision 3.4  1996/02/19  06:06:00  jjohnson
 * CSCdi48853:  rmon can consume all available memory
 * As an interim measure, don't allow rmon to consume memory once the
 * heap's low-water mark has been reached.  While we're at it, name
 * all the malloc'ed memory so that we can tell what is used where
 *
 * Revision 3.3  1996/02/09  07:57:26  jjohnson
 * CSCdi48524:  Need mechanism to display RMON tables
 * parse chains are in place.  checkpoint work so that anke can take over.
 *
 * Revision 3.2  1995/11/17  18:39:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:16  shaker
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

/* packet status bits */
#define	PS_TOOLONG		0x01
#define	PS_TOOSHORT		0x02
#define	PS_CRCALIGNERROR	0x04

/* For show_rmon_filter, print 16 chars per line plus the new line char */
#define NUM_CHARS_PER_LINE 16

/* number of bytes used to pretty-print one hex char in show_rmon_filt */
#define BYTES_PER_UNIT 8

static char     ifIndexString[] = "1.3.6.1.2.1.2.2.1.1";

/* global data describing the filterTable entries */
static SnmpType        filterEntryTypeTable[] = {
    {INTEGER_TYPE, SR_READ_ONLY, 0},	/* filterIndex */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* filterChannelIndex */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* filterPktDataOffset */
    {OCTET_PRIM_TYPE, SR_READ_WRITE, -1},	/* filterPktData */
    {OCTET_PRIM_TYPE, SR_READ_WRITE, -1},	/* filterPktDataMask */
    {OCTET_PRIM_TYPE, SR_READ_WRITE, -1},	/* filterPktDataNotMask */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* filterPktStatus */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* filterPktStatusMask */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* filterPktStatusNotMask */
    {OCTET_PRIM_TYPE, SR_READ_WRITE, -1},	/* filterOwner */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* filterStatus */
};

static Index_t         filterEntryIndex[1];
static RmonTable       filterTable;

/* global data describing the channelTable entries */
static SnmpType        channelEntryTypeTable[] = {
    {INTEGER_TYPE, SR_READ_ONLY, 0},	/* channelIndex */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* channelIfIndex */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* channelAcceptType */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* channelDataControl */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* channelTurnOnEventIndex */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* channelTurnOffEventIndex */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* channelEventIndex */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* channelEventStatus */
    {COUNTER_TYPE, SR_READ_ONLY, -1},	/* channelMatches */
    {OCTET_PRIM_TYPE, SR_READ_WRITE, -1},	/* channelDescription */
    {OCTET_PRIM_TYPE, SR_READ_WRITE, -1},	/* channelOwner */
    {INTEGER_TYPE, SR_READ_WRITE, -1},	/* channelStatus */
};

static Index_t         channelEntryIndex[1];
static RmonTable       channelTable;

void DeletefilterTableEntry SR_PROTOTYPE((int index));
static char *GetFilterAcceptType(int type);
static char *GetFilterControl(int type);
static char *GetChannelEventStatus(int type);

#ifdef	PACKETMATCHALARM	/* removed from the latest spec */
/* this routine sends a packet match alarm, which is described as follows
 * in the relevant draft rfc:
 *
 *        packetMatch TRAP-TYPE
 *            ENTERPRISE { rmon }
 *            VARIABLES { channelIndex, channelMatches, channelDescription }
 *            DESCRIPTION
 *                "The SNMP trap that is generated when a packet
 *                is captured by a channel that is configured for
 *                sending SNMP traps."
 *            ::= 3
 */
/*ARGSUSED*/
void
SendPacketMatchAlarm(evp, dummyval, cp)
    eventEntry_t   *evp;
    unsigned long   dummyval;
    channelEntry_t *cp;
{
    VarBind        *vb[3];
    OID            *name;
    OctetString    *osval;
    char            oidName[64];
    extern unsigned long alarmFails;

    /* clear the varbind list */
    memset(vb, 0, sizeof(vb));

    /* channelIndex */
    sprintf(oidName, "%s.%ld", channelIndexString, cp->channelIndex);
    if ((name = MakeOIDFromDot(oidName)) == 0) {
	goto fail;
    }
    if ((vb[0] = MakeVarBindWithValue(name, (OID *) NULL, INTEGER_TYPE,
				      (void *) &cp->channelIndex)) == 0) {
	goto fail;
    }
    FreeOID(name);

    /* channelMatches */
    sprintf(oidName, "%s.%ld", channelMatchesString, cp->channelIndex);
    if ((name = MakeOIDFromDot(oidName)) == 0) {
	goto fail;
    }
    if ((vb[1] = MakeVarBindWithValue(name, (OID *) NULL, COUNTER_TYPE,
				      (void *) &cp->channelMatches)) == 0) {
	goto fail;
    }
    FreeOID(name);

    /* channelDescription */
    sprintf(oidName, "%s.%ld", channelDescriptionString, cp->channelIndex);
    if ((name = MakeOIDFromDot(oidName)) == 0) {
	goto fail;
    }
    if ((osval = CloneOctetString(cp->channelDescription)) == 0) {
	goto fail;
    }
    if ((vb[2] = MakeVarBindWithValue(name, (OID *) NULL, OCTET_PRIM_TYPE,
				      (void *) osval)) == 0) {
	goto fail;
    }
    FreeOID(name);

    /* get the name of the trap to send */
    if ((name = MakeOIDFromDot(rmonString)) == 0) {
	goto fail;
    }

    /* link the varbinds together */
    vb[0]->next_var = vb[1];
    vb[1]->next_var = vb[2];
    vb[2]->next_var = 0;

    /* do the trap */
    RmonTrap(ENTERPRISE_TRAP, (long) 3, vb[0], name, (char *) NULL, evp);

    /* free the enterprise OID */
    FreeOID(name);
    return;

fail:
    alarmFails++;
    FreeOID(name);
    FreeVarBind(vb[0]);
    FreeVarBind(vb[1]);
    FreeVarBind(vb[2]);
}

#endif				/* PACKETMATCHALARM */

/* this routine activates a channel */
void
ActivateChannel(channelIndex)
    long            channelIndex;
{
    channelEntry_t *cp;
    int             index;

    channelTable.tip[0].value.uint_val = channelIndex;
    if ((index = SearchTable(&channelTable, EXACT)) != -1) {
	/* get a pointer to the channel */
	cp = (channelEntry_t *) channelTable.tp[index];
	if (cp->channelStatus == D_channelStatus_valid) {
	    cp->active = 1;
	}
    }
}

/* this routine checks to see if a channel has been activated.  The return
 * values are as follows:
 *	1	-- channel is active
 *	0	-- channel is inactive
 *	-1	-- channel doesn't exist, isn't in a valid state, or doesn't
 *		   respond to packets from the current interface.
 */
int
IsChannelActive(channelIndex, psp)
    long            channelIndex;
    PacketState    *psp;
{
    channelEntry_t *cp;
    int             index, status;

    status = (-1);
    channelTable.tip[0].value.uint_val = channelIndex;
    if ((index = SearchTable(&channelTable, EXACT)) != -1) {
	/* get a pointer to the channel */
	cp = (channelEntry_t *) channelTable.tp[index];

	/* make sure it's valid */
	if (cp->channelStatus == D_channelStatus_valid) {
	    /* check the data source */
	    if ((unsigned long) cp->channelIfIndex
		== psp->source->oid_ptr[psp->source->length - 1]) {
		/* get the activity state */
		if (cp->active != 0) {
		    status = 1;
		}
		else {
		    status = 0;
		}
	    }
	}
    }
    return (status);
}

/* this routine enables or disables channels depending on the event type */
void
SetChannelDataControl(eventIndex)
    long            eventIndex;
{
    int             channelindex;
    channelEntry_t *cp;

    /* process all channels */
    for (channelindex = 0; channelindex < channelTable.nitems; channelindex++) {
	/* get a pointer to the channel and make sure it's still active */
	cp = (channelEntry_t *) channelTable.tp[channelindex];
	if (cp == 0 || cp->channelStatus != D_channelStatus_valid) {
	    /* on to the next channel */
	    continue;
	}

	/* check turning it on */
	if (eventIndex == cp->channelTurnOnEventIndex) {
	    cp->channelDataControl = D_channelDataControl_on;
	}

	/* check turning it off */
	if (eventIndex == cp->channelTurnOffEventIndex) {
	    cp->channelDataControl = D_channelDataControl_off;
	}
    }
}

/* this routine performs channel processing */
void
ProcessChannels(psp)
    PacketState    *psp;
{
    int             channelindex;
    long            active;
    channelEntry_t *cp;
    void            (*alarmFunction) (eventEntry_t   *evp,
				      unsigned long   dummyval,
				      alarmEntry_t   *alarmp);

    /* process all channels */
    for (channelindex = 0; channelindex < channelTable.nitems; channelindex++) {
	/*
	 * get a pointer to the channel and make sure it's still active,
	 * refers to data from this interface, and is on
	 */
	cp = (channelEntry_t *) channelTable.tp[channelindex];
	if (cp == 0
	    || cp->channelStatus != D_channelStatus_valid
	    || (unsigned long) cp->channelIfIndex
	    != psp->source->oid_ptr[psp->source->length - 1]) {
	    /* on to the next channel */
	    continue;
	}

	/* save and clear the active state */
	active = cp->active;
	cp->active = 0;

	/* check for a match */
	if ((active != 0 && cp->channelAcceptType == D_channelAcceptType_acceptMatched)
	    || (active == 0 && cp->channelAcceptType == D_channelAcceptType_acceptFailed)) {
	    /* this is updated regardless of the dataControl value */
	    cp->channelMatches++;

	    /* only continue processing if data control is set to on */
	    if (cp->channelDataControl == D_channelDataControl_off)
		continue;

	    /* check whether to set off an event */
	    if (cp->channelEventStatus != D_channelEventStatus_eventFired) {

#ifdef	PACKETMATCHALARM	/* removed from the latest spec */
		alarmFunction = SendPacketMatchAlarm;
#else				/* PACKETMATCHALARM */
		alarmFunction = 0;
#endif				/* PACKETMATCHALARM */

		FireEvent(cp->channelEventIndex, alarmFunction,
			  (unsigned long) 0, (void *) cp);
		if (cp->channelEventStatus != D_channelEventStatus_eventAlwaysReady) {
		    cp->channelEventStatus = D_channelEventStatus_eventFired;
		}
	    }

	    /* notify buffers that they must capture this packet */
	    NotifyBuffers(cp->channelIndex);
	}
    }
}

/* this routine does filter processing on packets */
void
ProcessFilters(psp)
    PacketState    *psp;
{
    filterEntry_t  *fp;
    int             filterindex, i, len, dataMaskStart, dataNotMaskStart;
    unsigned char  *packetp, *datap, *dataMaskp, *dataNotMaskp;
    register unsigned char packetByte, dataByte, dataMask, dataNotMask;

    /* process all active filters */
    for (filterindex = 0; filterindex < filterTable.nitems; filterindex++) {
	/* get a pointer to the filter and make sure it's active */
	fp = (filterEntry_t *) filterTable.tp[filterindex];
	if (fp == 0 || fp->filterStatus != D_filterStatus_valid)
	    continue;

	/*
	 * don't bother with further tests if the channel has already been
	 * activated
	 */
	if (IsChannelActive(fp->filterChannelIndex, psp) == 1) {
	    break;
	}

	/* data match step 1 -- make sure the packet can be filtered */
	len = (int) fp->filterPktData->length;
	if (psp->size < ((int) fp->filterPktDataOffset + len)) {
	    /* packet is too short */
	    continue;
	}

	/* get the offset within packet data to apply the data mask */
	dataMaskStart = (int) fp->filterPktDataMask->length;
	if (dataMaskStart > len) {
	    dataMaskStart = 0;
	}
	else {
	    dataMaskStart = len - dataMaskStart;
	}

	/* get the offset within packet data to apply the data not mask */
	dataNotMaskStart = (int) fp->filterPktDataNotMask->length;
	if (dataNotMaskStart > len) {
	    dataNotMaskStart = 0;
	}
	else {
	    dataNotMaskStart = len - dataNotMaskStart;
	}

	/* get pointers to the mask data */
	dataMaskp = fp->filterPktDataMask->octet_ptr;
	dataNotMaskp = fp->filterPktDataNotMask->octet_ptr;

	/*
	 * data match steps 2 and 3 combined -- see if the packet matches
	 * bits in the dataNotMask
	 */
	packetp = psp->p + fp->filterPktDataOffset;
	datap = fp->filterPktData->octet_ptr;
	for (i = 0; i < len; i++) {
	    /* get the bytes from the different arrays */
	    dataMask = (i < dataMaskStart ? 0xFF : *dataMaskp++);
	    if (i < dataNotMaskStart) {
		dataNotMask = 0x00;
	    }
	    else {
		/* only mask relevant bits */
		dataNotMask = *dataNotMaskp & dataMask;
		dataNotMaskp++;
	    }

	    /* if these bits are not relevant, don't compare them */
	    if (dataMask != 0) {
		/*
		 * do comparisons on relevant (not masked) bits.  If the
		 * dataNotMask is zero, there are no bits being tested for
		 * inequality and we can skip that comparison.
		 */
		packetByte = *packetp & dataMask;
		dataByte = *datap & dataMask;
		if ((packetByte & ~dataNotMask) != (dataByte & ~dataNotMask)
		    || (dataNotMask != 0
		&& (packetByte & dataNotMask) == (dataByte & dataNotMask))) {
		    /* match failed */
		    break;
		}
	    }

	    /* advance pointers */
	    packetp++;
	    datap++;
	}
	if (i != len)
	    continue;

	/* do packet status checks */
	if (fp->filterPktStatus != 0) {
	    if (fp->filterPktStatusMask == 0) {
		dataMask = 0xFF;
	    }
	    else {
		dataMask = (int) fp->filterPktStatusMask;
	    }

	    /* only mask relevant bits */
	    if (dataMask != 0) {
		/* get the packet status */
		packetByte = 0;
		if (psp->size > 1518)
		    packetByte += PS_TOOLONG;
		else if (psp->size < 64)
		    packetByte += PS_TOOSHORT;
		if (psp->error != 0)
		    packetByte += PS_CRCALIGNERROR;

		/*
		 * perform checks; if dataNotMask is zero, there are no bits
		 * being tested for inequality
		 */
		packetByte &= dataMask;
		dataByte = (unsigned char) fp->filterPktStatus & dataMask;
		dataNotMask = (unsigned char) fp->filterPktStatusNotMask;
		if ((packetByte & ~dataNotMask) != (dataByte & ~dataNotMask)
		|| (packetByte & dataNotMask) == (dataByte & dataNotMask)) {
		    /* match failed */
		    continue;
		}
	    }
	}

	/* passed all tests, activate the channel */
	ActivateChannel(fp->filterChannelIndex);
    }
}

/* --------------------- MIB COMPILER GENERATED ------------------------- */

int
k_filterEntry_initialize()
{
    filterEntryIndex[0].offset = I_filterIndex;
    filterEntryIndex[0].type = T_uint;

    filterTable.nindices = 1;
    filterTable.tip = filterEntryIndex;
    filterTable.nitems = 0;
    filterTable.rowsize = sizeof(filterEntry_t);
    if ((filterTable.tp = (void **) malloc(sizeof(filterEntry_t *))) == NULL) {
	return 0;
    }
    reg_add_rmon_event(SetChannelDataControl,
		       "SetChannelDataControl");

    return 1;
}

int
k_filterEntry_terminate()
{
    while (filterTable.nitems > 0) {
	DeletefilterTableEntry(0);
    }

    free(filterTable.tp);
    return 1;
}

void DeletechannelTableEntry SR_PROTOTYPE((int index));

int
k_channelEntry_initialize()
{
    channelEntryIndex[0].offset = I_channelIndex;
    channelEntryIndex[0].type = T_uint;

    channelTable.nindices = 1;
    channelTable.tip = channelEntryIndex;
    channelTable.nitems = 0;
    channelTable.rowsize = sizeof(channelEntry_t);
    if ((channelTable.tp = (void **) malloc(sizeof(channelEntry_t *))) == NULL) {
	return 0;
    }

    return 1;
}

int
k_channelEntry_terminate()
{
    while (channelTable.nitems > 0) {
	DeletechannelTableEntry(0);
    }

    free(channelTable.tp);
    return 1;
}

/* This routine is called by the timeout code to
 * delete a pending creation of a Table entry */
void
filterTableDeleteCallback(tdp)
    TimeOutDescriptor *tdp;
{
    filterEntry_t  *data;

    /* dummy up an entry to delete */
    data = (filterEntry_t *) tdp->UserData2;
    data->filterStatus = D_filterStatus_invalid;
    data->TimerId = -1;

    /* free the timeout descriptor */
    free(tdp);

    /* call the set method */
    k_filterEntry_set(data, (ContextInfo *) NULL, 0);
}

/* This routine deletes an entry from the filterTable */
void
DeletefilterTableEntry(index)
    int             index;
{
    filterEntry_t  *data;

    /* get a pointer to the old entry */
    data = (filterEntry_t *) filterTable.tp[index];

    /* free the old entry and remove it from the table */
    filterEntry_free(data);
    RemoveTableEntry(&filterTable, index);
}

/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_filterEntryFreeUserpartData(data)
    filterEntry_t  *data;
{
    /* nothing to free by default */
}

filterEntry_t  *
k_filterEntry_get(serialNum, contextInfo, nominator, searchType, filterIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            filterIndex;
{
    int             index;

    filterTable.tip[0].value.uint_val = filterIndex;
    if ((index = SearchTable(&filterTable, searchType)) == -1) {
	return NULL;
    }

    return (filterEntry_t *) filterTable.tp[index];

}

#ifdef SETS
int
k_filterEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

#ifdef SR_SNMPv2
    filterEntry_t  *undodata = (filterEntry_t *) dp->undodata;
#else				/* SR_SNMPv2 */
    filterEntry_t  *undodata = NULL;
#endif				/* SR_SNMPv2 */
    int             error_status = NO_ERROR;

    /*
     * make sure that the data source isn't being changed if this is a valid
     * entry
     */
    if (undodata != 0 && undodata->filterStatus == D_filterStatus_valid) {
	switch (object->nominator) {
	  case I_filterChannelIndex:
	  case I_filterOwner:
	  case I_filterStatus:
	    /* these can be modified while the filter is valid */
	    break;

	  default:
	    if (CmpValues(object->nominator, filterEntryTypeTable,
			(unsigned long *) undodata, value->ul_value) != 0) {
		/*
		 * can't change the value for this variable once the filter
		 * is valid
		 */
		error_status = INCONSISTENT_VALUE_ERROR;
	    }
	    break;
	}
    }

    return (error_status);
}

int
k_filterEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    filterEntry_t  *data = (filterEntry_t *) dp->data;

    if (data->filterStatus == D_filterStatus_invalid) {
	dp->state = DELETE;
    }
    else {
	dp->state = ADD_MODIFY;
    }

    return NO_ERROR;
}

int
k_filterEntry_set_defaults(dp)
    doList_t       *dp;
{

    filterEntry_t  *data = (filterEntry_t *) dp->data;

    /*
     * if we're low on memory, don't allow the entry to be created
     */
    if (HeapLowMem())
	return RESOURCE_UNAVAILABLE_ERROR;

    /* assume defaults */
    data->filterChannelIndex = 65535;	/* arbitrary but invalid */
    data->filterPktDataOffset = 0;	/* defval from mib */

    if ((data->filterPktData = MakeOctetStringFromText("")) == 0) {
	return RESOURCE_UNAVAILABLE_ERROR;
    }

    if ((data->filterPktDataMask = MakeOctetStringFromText("")) == 0) {
	return RESOURCE_UNAVAILABLE_ERROR;
    }

    if ((data->filterPktDataNotMask = MakeOctetStringFromText("")) == 0) {
	return RESOURCE_UNAVAILABLE_ERROR;
    }

    if ((data->filterOwner = MakeOctetStringFromText("")) == 0) {
	return RESOURCE_UNAVAILABLE_ERROR;
    }

    data->filterStatus = D_filterStatus_createRequest;
    data->TimerId = -1;

    SET_ALL_VALID(data->valid);

    return NO_ERROR;
}

int
k_filterEntry_set(data, contextInfo, function)
    filterEntry_t  *data;
    ContextInfo    *contextInfo;
    int             function;
{

    int             index;
    filterEntry_t  *new = NULL;

    /* find this entry in the table */
    filterTable.tip[0].value.uint_val = data->filterIndex;
    if ((index = SearchTable(&filterTable, EXACT)) != -1) {
	new = (filterEntry_t *) filterTable.tp[index];
    }

    /* perform the table entry operation on it */
    if (data->filterStatus == D_filterStatus_invalid) {
	if (data->TimerId != -1) {
	    CancelEntryStatusTimeout((int) data->TimerId);
	}

	if (index == -1) {
	    return NO_ERROR;
	}
	else {
	    /* delete the table entry */
	    DeletefilterTableEntry(index);
	    return NO_ERROR;
	}
    }
    else if (index == -1) {
	/* add the entry */
	filterTable.tip[0].value.uint_val = data->filterIndex;
	if ((index = NewTableEntry(&filterTable)) == -1) {
	    return GEN_ERROR;
	}
	new = (filterEntry_t *) filterTable.tp[index];
	name_memory(new, "RMON Filter");

	/* set a timeout */
	if ((new->TimerId = SetEntryStatusTimeout(MAX_ROW_CREATION_TIME, (void *) new, filterTableDeleteCallback)) == -1) {
	    DeletefilterTableEntry(index);
	    return GEN_ERROR;
	}
    }

    TransferEntries(I_filterStatus, filterEntryTypeTable, 
		    (unsigned long *)new, (unsigned long *)data);
    SET_ALL_VALID(new->valid);

    /* if the new row entry is now valid, cancel the creation timeout */
    if (new->filterStatus == D_filterStatus_valid && new->TimerId != -1) {
	CancelEntryStatusTimeout((int) new->TimerId);
	new->TimerId = -1;
    }
    else {
	if (new->filterStatus == D_filterStatus_createRequest) {
	    new->filterStatus = D_filterStatus_underCreation;
	}
    }

    return NO_ERROR;

}

#endif				/* SETS */

#ifdef SR_SNMPv2
int
filterEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    filterEntry_t  *data = (filterEntry_t *) doCur->data;

#ifdef SR_SNMPv2
    filterEntry_t  *undodata = (filterEntry_t *) doCur->undodata;
#else				/* SR_SNMPv2 */
    filterEntry_t  *undodata = NULL;
#endif				/* SR_SNMPv2 */
    filterEntry_t  *setdata = NULL;
    int             function;

    /* we are either trying to undo an add, a delete, or a modify */
    if (undodata == NULL) {
	/* undoing an add, so delete */
	data->filterStatus = D_filterStatus_invalid;
	setdata = data;
	function = DELETE;
    }
    else {
	/* undoing a delete or modify, replace the original data */
	if (undodata->filterStatus == D_filterStatus_underCreation) {
	    undodata->filterStatus = D_filterStatus_createRequest;
	}
	setdata = undodata;
	function = ADD_MODIFY;
    }

    /* use the set method for the undo */
    if ((setdata != NULL) && k_filterEntry_set(setdata, contextInfo, function) == NO_ERROR) {
	return NO_ERROR;
    }

    return UNDO_FAILED_ERROR;
}

#endif				/* SR_SNMPv2 */

/* This routine is called by the timeout code to
 * delete a pending creation of a Table entry */
void
channelTableDeleteCallback(
    TimeOutDescriptor *tdp)
{
    channelEntry_t *data;

    /* dummy up an entry to delete */
    data = (channelEntry_t *) tdp->UserData2;
    data->channelStatus = D_channelStatus_invalid;
    data->TimerId = -1;

    /* free the timeout descriptor */
    free(tdp);

    /* call the set method */
    k_channelEntry_set(data, (ContextInfo *) NULL, 0);
}

/* This routine deletes an entry from the channelTable */
void
DeletechannelTableEntry(index)
    int             index;
{
    channelEntry_t *data;

    /* get a pointer to the old entry */
    data = (channelEntry_t *) channelTable.tp[index];

    /* free the old entry and remove it from the table */
    channelEntry_free(data);
    RemoveTableEntry(&channelTable, index);
}

/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_channelEntryFreeUserpartData(
    channelEntry_t *data)
{
    /* nothing to free by default */
}

channelEntry_t *
k_channelEntry_get(serialNum, contextInfo, nominator, searchType, channelIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            channelIndex;
{
    int             index;

    channelTable.tip[0].value.uint_val = channelIndex;
    if ((index = SearchTable(&channelTable, searchType)) == -1) {
	return NULL;
    }

    return (channelEntry_t *) channelTable.tp[index];

}

#ifdef SETS
int
k_channelEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

#ifdef SR_SNMPv2
    channelEntry_t *undodata = (channelEntry_t *) dp->undodata;
#else				/* SR_SNMPv2 */
    channelEntry_t *undodata = NULL;
#endif				/* SR_SNMPv2 */
    int             error_status = NO_ERROR;

    /*
     * make sure that the data source isn't being changed if this is a valid
     * entry
     */
    if (undodata != 0 && undodata->channelStatus == D_channelStatus_valid) {
	switch (object->nominator) {
	  case I_channelDataControl:
	  case I_channelEventStatus:
	  case I_channelDescription:
	  case I_channelOwner:
	  case I_channelStatus:
	    /* these can be modified while the channel is valid */
	    break;

	  default:
	    if (CmpValues(object->nominator, channelEntryTypeTable,
			(unsigned long *) undodata, value->ul_value) != 0) {
		/*
		 * can't change the value for this variable once the channel
		 * is valid
		 */
		error_status = INCONSISTENT_VALUE_ERROR;
	    }
	    break;
	}
    }

    return (error_status);
}

int
k_channelEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    channelEntry_t *data = (channelEntry_t *) dp->data;
    char            buf[64];
    OID            *ifIndex;

    if (data->channelStatus == D_channelStatus_invalid) {
	dp->state = DELETE;
    }
    else {
	dp->state = ADD_MODIFY;

	/* if setting to valid, make sure we have all the information we need */
	if (data->channelStatus == D_channelStatus_valid) {
	    /* make sure we have a valid data source */
	    if (!VALID(I_channelIfIndex, data->valid)) {
		dp->state = UNKNOWN;
	    }
	    else {
		/* make an oid with the channel interface index */
		sprintf(buf, "%s.%ld", ifIndexString, data->channelIfIndex);
		if ((ifIndex = MakeOIDFromDot(buf)) == 0) {
		    return (RESOURCE_UNAVAILABLE_ERROR);
		}

		/* check the data source for validity */
		if (CheckEthernetDataSource(ifIndex) == 0) {
		    /* can't do it, don't have a valid data source */
		    dp->state = UNKNOWN;
		}

		/* free the oid */
		FreeOID(ifIndex);
	    }
	}
    }

    return NO_ERROR;
}

int
k_channelEntry_set_defaults(dp)
    doList_t       *dp;
{

    channelEntry_t *data = (channelEntry_t *) dp->data;

    /* set defaults */
    data->channelIfIndex = 65536;	/* arbitrary but invalid */
    data->channelAcceptType = D_channelAcceptType_acceptMatched;
    data->channelDataControl = D_channelDataControl_off;
    data->channelTurnOnEventIndex = 0;
    data->channelTurnOffEventIndex = 0;
    data->channelEventIndex = 0;
    data->channelEventStatus = D_channelEventStatus_eventReady;

    if ((data->channelDescription = MakeOctetStringFromText("")) == 0) {
	return RESOURCE_UNAVAILABLE_ERROR;
    }

    if ((data->channelOwner = MakeOctetStringFromText("")) == 0) {
	return RESOURCE_UNAVAILABLE_ERROR;
    }

    data->channelStatus = D_channelStatus_createRequest;
    data->TimerId = -1;

    SET_ALL_VALID(data->valid);

    return NO_ERROR;
}

int
k_channelEntry_set(data, contextInfo, function)
    channelEntry_t *data;
    ContextInfo    *contextInfo;
    int             function;
{

    int             index;
    channelEntry_t *new = NULL;

    /* find this entry in the table */
    channelTable.tip[0].value.uint_val = data->channelIndex;
    if ((index = SearchTable(&channelTable, EXACT)) != -1) {
	new = (channelEntry_t *) channelTable.tp[index];
    }

    /* perform the table entry operation on it */
    if (data->channelStatus == D_channelStatus_invalid) {
	if (data->TimerId != -1) {
	    CancelEntryStatusTimeout((int) data->TimerId);
	}

	if (index == -1) {
	    return NO_ERROR;
	}
	else {
	    /* delete the table entry */
	    DeletechannelTableEntry(index);
	    return NO_ERROR;
	}
    }
    else if (index == -1) {
	/* add the entry */
	channelTable.tip[0].value.uint_val = data->channelIndex;
	if ((index = NewTableEntry(&channelTable)) == -1) {
	    return GEN_ERROR;
	}
	new = (channelEntry_t *) channelTable.tp[index];
	name_memory(new, "RMON Channel");

	/* set a timeout */
	if ((new->TimerId = SetEntryStatusTimeout(MAX_ROW_CREATION_TIME, (void *) new, channelTableDeleteCallback)) == -1) {
	    DeletechannelTableEntry(index);
	    return GEN_ERROR;
	}
    }

    TransferEntries(I_channelStatus, channelEntryTypeTable,
		    (unsigned long *)new, (unsigned long *)data);
    SET_ALL_VALID(new->valid);

    /* transfer private data fields too */
    new->active = data->active;

    /* if the new row entry is now valid, cancel the creation timeout */
    if (new->channelStatus == D_channelStatus_valid && new->TimerId != -1) {
	CancelEntryStatusTimeout((int) new->TimerId);
	new->TimerId = -1;
    }
    else {
	if (new->channelStatus == D_channelStatus_createRequest) {
	    new->channelStatus = D_channelStatus_underCreation;
	}
    }

    return NO_ERROR;

}

#endif				/* SETS */

#ifdef SR_SNMPv2
int
channelEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    channelEntry_t *data = (channelEntry_t *) doCur->data;

#ifdef SR_SNMPv2
    channelEntry_t *undodata = (channelEntry_t *) doCur->undodata;
#else				/* SR_SNMPv2 */
    channelEntry_t *undodata = NULL;
#endif				/* SR_SNMPv2 */
    channelEntry_t *setdata = NULL;
    int             function;

    /* we are either trying to undo an add, a delete, or a modify */
    if (undodata == NULL) {
	/* undoing an add, so delete */
	data->channelStatus = D_channelStatus_invalid;
	setdata = data;
	function = DELETE;
    }
    else {
	/* undoing a delete or modify, replace the original data */
	if (undodata->channelStatus == D_channelStatus_underCreation) {
	    undodata->channelStatus = D_channelStatus_createRequest;
	}
	setdata = undodata;
	function = ADD_MODIFY;
    }

    /* use the set method for the undo */
    if ((setdata != NULL) && k_channelEntry_set(setdata, contextInfo, function) == NO_ERROR) {
	return NO_ERROR;
    }

    return UNDO_FAILED_ERROR;
}

#endif				/* SR_SNMPv2 */

/******************** cisco supplied routines follow ********************/

/* show rmon filters
 * output a string similiar to the following for every filter/channel
 *    Filter 12345 is valid, and owned by foo
 *       Data offset is xxxxxx, with 
 *       Data of xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
 *       Data Mask is xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
 *       Data Not Mask is xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
 *       Pkt status is xxxxxxx, status mask is xxxxxxx, not mask is xxxxxxx
 *    Associated channel 67890 is valid, and owned by bar
 *       Type of channel is acceptMatched, data control is on
 *       Generate event index xxxxxxx
 *       Event status is eventReady, # of matches is xxxxxx
 *       Turn on event index is xxxxxxx, turn off event index is xxxxxxx
 *       Description: xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
 */

void
show_rmon_filters (void)
{
    filterEntry_t  *filterp;
    channelEntry_t *channelp;
    int             i, bufferSize;
    char           *buffer;
    long            channel_index;

    automore_enable(NULL);
    if (filterTable.nitems == 0) {
	printf("\nFilter table is empty");
	automore_disable();
	return;
    }

    for (i = 0; i < filterTable.nitems; i++) {
        /*
         * If the user terminates show_rmon_filter by typing "q",
         * break out of for loop.
         */
        if (automore_quit()) {
            break;
        }

        filterp = filterTable.tp[i];
        if (filterp == 0 || filterp->filterStatus != D_filterStatus_valid)
             continue;
	mem_lock(filterp);

        /* 
         * Find out the largest octet string, and malloc only one buffer,
         * capable of holding this OctetString. 
         * NOTE: each byte from the OctetString, when translated into
         * hexadecimal printable format, and padded with beautifying
         * spaces and newline feeds, takes up BYTES_PER_UNIT bytes of
         * the buffer being allocated
         */

        bufferSize = filterp->filterPktData->length;

        if (filterp->filterPktDataMask->length > bufferSize) {
            bufferSize = filterp->filterPktDataMask->length;
        }
        if (filterp->filterPktDataNotMask->length > bufferSize) {
            bufferSize = filterp->filterPktDataNotMask->length;
        }

        /* 
         * Check if bufferSize is big enough to hold the filterOwner 
         * string. Note that the bufferSize computed above is the
         * maximum unformatted string size - the actual malloced buffer
         * for the formatted output is BYTES_PER_UNIT times bigger.
         * filterOwner points to a printable char string - it needs
         * no fancy formatting.
         */
        if ((filterp->filterOwner->length + 1) > 
            (BYTES_PER_UNIT * bufferSize)) {
            bufferSize = filterp->filterOwner->length + 1;
        }

        buffer = (char*) malloc(BYTES_PER_UNIT * bufferSize);

        /* 
         * If mallocing memory failed, you will need to
         * undo the "mem_lock" by calling "free" and return
         */
        if (!buffer) {
            free(filterp);
            automore_disable();
            return;
        }

        memcpy(buffer, filterp->filterOwner->octet_ptr,
		   filterp->filterOwner->length);
        printf("\nFilter %d is %s, and owned by %s",
               filterp->filterIndex,
               rmon_status_string(filterp->filterStatus), 
               filterp->filterOwner->length ? buffer : "unknown");
        printf("\n Data offset is %d, with ",
	       filterp->filterPktDataOffset); 

        SPrintOctetString(filterp->filterPktData, NUM_CHARS_PER_LINE,
                          buffer);
        printf("\n Data of %s", buffer);

        SPrintOctetString(filterp->filterPktDataMask, NUM_CHARS_PER_LINE,
                          buffer);
        printf("\n Data Mask is %s", buffer);

        SPrintOctetString(filterp->filterPktDataNotMask, NUM_CHARS_PER_LINE,
                          buffer);
        printf("\n Data Not Mask is %s", buffer);

        printf("\n Pkt status is %d, status mask is %d, not mask is %d", 
               filterp->filterPktStatus, filterp->filterPktStatusMask,
	       filterp->filterPktStatusNotMask);
       
        free(buffer);
        free(filterp);

        channel_index = filterp->filterChannelIndex;

	if (channel_index == 0) {
	    continue;
	}
        channelp = k_channelEntry_get(0, NULL, 0, EXACT, channel_index);
        if ((channelp == NULL) || (channelp->channelStatus !=
				   D_channelStatus_valid)) {
             continue;
	 }

       	mem_lock(channelp);
        if (channelp->channelDescription->length >
            channelp->channelOwner->length) {
            bufferSize = channelp->channelDescription->length;
        } else {
            bufferSize = channelp->channelOwner->length;
        }

        buffer = (char *) malloc(bufferSize + 1);
        if (!buffer) {
            free(channelp);
            automore_disable();
            return;
        }

        memcpy(buffer, channelp->channelOwner->octet_ptr,
               channelp->channelOwner->length);
        printf("\n Associated channel %d is %s, and owned by %s",
	           channelp->channelIndex,
	           rmon_status_string(channelp->channelStatus), 
                   buffer ? buffer : "unknown");
        printf("\n Type of channel is %s, data control is %s",
	       GetFilterAcceptType(channelp->channelAcceptType),
	       GetFilterControl(channelp->channelDataControl));
        printf("\n Generate event index %d", channelp->channelEventIndex);
        printf("\n Event status is %s, # of matches is %d",
	       GetChannelEventStatus(channelp->channelEventStatus),
	       channelp->channelMatches);
        printf("\n Turn on event index is %d, turn off event index is %d", 
               channelp->channelTurnOnEventIndex,
	       channelp->channelTurnOffEventIndex);

        strcpy(buffer, "");
        memcpy(buffer, channelp->channelDescription->octet_ptr,
               channelp->channelDescription->length);
        printf("\n Description: %s", buffer ? buffer : "unknown");

        free(buffer);
        free(channelp);
    }
    automore_disable();
}

static char *
GetFilterAcceptType (int type) 
{
    switch(type) {
      case D_channelAcceptType_acceptMatched:
        return("acceptMatched");
        break;
      case D_channelAcceptType_acceptFailed:
        return("acceptFailed");
        break;
      default:
        return("unknown");
    }
}

static char *
GetFilterControl (int type) 
{
    switch(type) {
      case D_channelDataControl_on:
        return("on");
        break;
      case D_channelDataControl_off:
        return("off");
        break;
      default:
        return("unknown");
    }
}

static char *
GetChannelEventStatus (int type)
{
    switch(type) {
      case D_channelEventStatus_eventReady:
        return("eventReady");
        break;
      case D_channelEventStatus_eventFired:
        return("eventFired");
        break;
      case D_channelEventStatus_eventAlwaysReady:
        return("eventAlwaysReady");
        break;
      default:
        return("unknown");
    }
}
