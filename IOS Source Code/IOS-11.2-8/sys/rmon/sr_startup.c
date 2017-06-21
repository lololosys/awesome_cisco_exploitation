/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_startup.c,v 3.2 1995/11/17 18:39:54 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rmon/sr_startup.c,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_startup.c,v $
 * Revision 3.2  1995/11/17  18:39:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:37  shaker
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

/* this routine creates an ether stats entry */
void
SetEtherStatsMonitor(index, source, owner)
    long            index;
    OID            *source;
    char           *owner;
{
    etherStatsEntry_t esp;
    doList_t        de;
    OctetString    *etherStatsOwner = 0;
    OID            *etherStatsDataSource = 0;

    if ((etherStatsOwner = MakeOctetStringFromText(owner)) == 0) {
	DPRINTF((APWARN, "%s: can't init etherStatsOwner\n", fname));
	goto done;
    }
    if ((etherStatsDataSource = CloneOID(source)) == 0) {
	DPRINTF((APWARN, "%s: can't init etherStatsDataSource\n", fname));
	goto done;
    }

    /* set the table entry */
    memset(&de, 0, sizeof(doList_t));
    memset(&esp, 0, sizeof(etherStatsEntry_t));
    de.data = (void *) &esp;
    if (k_etherStatsEntry_set_defaults(&de) != NO_ERROR) {
	DPRINTF((APWARN, "%s: can't set etherStats defaults\n", fname));
	goto done;
    }
    FreeOID(esp.etherStatsDataSource);
    FreeOctetString(esp.etherStatsOwner);
    esp.etherStatsOwner = etherStatsOwner;
    esp.etherStatsDataSource = etherStatsDataSource;
    esp.etherStatsIndex = index;
    esp.etherStatsStatus = D_etherStatsStatus_valid;

    /* clear our parameters */
    etherStatsOwner = 0;
    etherStatsDataSource = 0;

    /* do the set */
    k_etherStatsEntry_set(&esp, (ContextInfo *) 0, ADD_MODIFY);

done:
    FreeOID(etherStatsDataSource);
    FreeOctetString(etherStatsOwner);
}

/* this routine creates a history entry */
void
SetHistoryMonitor(index, source, owner, requested, interval)
    long            index;
    OID            *source;
    char           *owner;
    long            requested;
    long            interval;
{
    historyControlEntry_t hce;
    doList_t        de;
    OctetString    *historyControlOwner = 0;
    OID            *historyControlDataSource = 0;

    if ((historyControlOwner = MakeOctetStringFromText(owner)) == 0) {
	DPRINTF((APWARN, "%s: can't init historyControlOwner\n", fname));
	goto done;
    }
    if ((historyControlDataSource = CloneOID(source)) == 0) {
	DPRINTF((APWARN, "%s: can't init historyControlDataSource\n", fname));
	goto done;
    }

    /* set the first history collection */
    memset(&de, 0, sizeof(doList_t));
    memset(&hce, 0, sizeof(historyControlEntry_t));
    de.data = (void *) &hce;
    if (k_historyControlEntry_set_defaults(&de) != NO_ERROR) {
	DPRINTF((APWARN, "%s: can't set historyControl defaults\n", fname));
	goto done;
    }
    FreeOID(hce.historyControlDataSource);
    FreeOctetString(hce.historyControlOwner);
    hce.historyControlOwner = historyControlOwner;
    hce.historyControlDataSource = historyControlDataSource;
    hce.historyControlIndex = index;
    hce.historyControlBucketsRequested = requested;
    hce.historyControlInterval = interval;
    hce.historyControlStatus = D_historyControlStatus_valid;

    /* clear our parameters */
    historyControlOwner = 0;
    historyControlDataSource = 0;

    /* do the set */
    k_historyControlEntry_set(&hce, (ContextInfo *) 0, ADD_MODIFY);

done:
    FreeOID(historyControlDataSource);
    FreeOctetString(historyControlOwner);
}
