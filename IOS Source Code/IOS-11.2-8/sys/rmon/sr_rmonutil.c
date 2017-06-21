/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_rmonutil.c,v 3.2 1995/11/17 18:39:52 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rmon/sr_rmonutil.c,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_rmonutil.c,v $
 * Revision 3.2  1995/11/17  18:39:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:36  shaker
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

/* rmonutil.c was extracted from rmonmain.c */

/*
 * Log: rmonmain.c,v 
 * Revision 1.7  1995/02/21  18:58:03  levi
 * Removed #include "subagent.h", replaced with i_GetMib prototype
 *
 * Revision 1.6  1995/01/12  14:00:00  levi
 * Ifdef'ed the FreeVarbind on PKG after i_GetMib call
 *
 * Revision 1.5  1994/11/14  20:18:23  pearson
 * Removed the unused heap.h file
 *
 * Revision 1.4  1994/10/28  17:30:58  vaughn
 * Placed new copyright in the file
 *
 * Revision 1.3  1994/10/26  21:57:28  vaughn
 * Changed copyrights to the new copyright structure
 *
 * Revision 1.2  1994/08/11  14:56:16  pearson
 * Cleaned up some debugging code, added the SR_SIF, SR_FTC, and SR_FTL
 * compilation switches.
 *
 * Revision 1.1  1994/08/08  19:31:48  pearson
 * First checkin of the RMON sources.
 *
 * Revision 1.2  1993/11/03  14:53:14  battle
 * Put #ifdef SR_SNMPv2 around 2 instances of INTEGER_32_TYPE.
 *
 * Apr-4-94 AMP
 *	Noticed that FreeOID() and FreeOctetString() both check to see if their
 * inputs are NULL pointers, so removed the redundant checking before the call.
 * This was intended to improve readability and reduce code size.  Added a
 * routine, FreePtr(), which frees non-null pointers allocated via malloc().
 *
 */

#include "master.h"
#include "sr_rmonmib.h"
#include "../snmp/sr_tmq.h"
VarBind        *i_GetMib SR_PROTOTYPE((OID * object));

/* this routine monitors transitions in the EntryStatus textual convention
 * for sanity.  It returns 0 if no error, an error status otherwise */
int
CheckEntryStatus(new, old)
    long            new, old;
{
    /*
     * if no change, and not trying to set to create-request more than once,
     * continue checking
     */
    if (new != old) {
	/* the new value must be a new one */
	if (new == ES_UNDERCREATION)
	    return (WRONG_VALUE_ERROR);
	if (new == ES_CREATEREQUEST && old != ES_UNINITIALIZED)
	    return (INCONSISTENT_VALUE_ERROR);
    }

    /* return a good status */
    return (0);
}

/* cancel a timer event and free its associated data */
void
CancelEntryStatusTimeout(id)
    int             id;
{
    DPRINTF((APTRACE, "CancelTimeoutWithInfo: cancelling timer %d\n", id));
    TimerFreeInfo(id, TimerInfoFree);
    CancelTimeout(id);
}

/* this routine creates a one-shot timeout given information about timeout
 * parameters and the interval.  It returns the timer id, or -1 if an error. */
int
SetEntryStatusTimeout(delay, data, callback)
    long            delay;
    void           *data;
    void            (*callback) (TimeOutDescriptor *TimeOut);
{
    TimeOutDescriptor *TimeOut;
    int             id;

    /* allocate the timeout descriptor */
    if ((TimeOut = (TimeOutDescriptor *) malloc(sizeof(TimeOutDescriptor)))
	== NULL) {
	return (-1);
    }

    /* make sure we can set the timeout */
    if (CanSetTimeout() == (-1))
	return (-1);

    /* set up the timeout parameters */
    TimeOut->CallBack = callback;
    TimeOut->UserData1 = 0;
    TimeOut->UserData2 = data;

    /* make the timeout relative to the current time */
    delay += TimerCurTime();

    /* install the timeout */
    id = SetOneshotTimeout(delay, (void *) TimeOut);
    DPRINTF((APTRACE, "CreateOneShotTimeout: id is %d\n", id));
    return (id);
}

/* frees the info field associated with a timer */
void
TimerInfoFree(info)
    void           *info;
{
    free((char *) info);
}
