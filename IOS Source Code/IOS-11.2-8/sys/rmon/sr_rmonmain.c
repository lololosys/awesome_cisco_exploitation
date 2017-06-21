/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_rmonmain.c,v 3.3 1996/02/19 02:24:36 jjohnson Exp $
 * $Source: /release/111/cvs/Xsys/rmon/sr_rmonmain.c,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_rmonmain.c,v $
 * Revision 3.3  1996/02/19  02:24:36  jjohnson
 * CSCdi49291:  Memory leak in rmon
 * Callers of function i_GetMib() must free the returned VarBind
 *
 * Revision 3.2  1995/11/17  18:39:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:30  shaker
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

/* parts of the original rmonmain.c have been extracted into rmonutil.c */

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

/* this routine verifies that a data source exists.  It returns 0 if not,
 * nonzero otherwise */
int
CheckEthernetDataSource(source)
    OID            *source;
{
    VarBind        *vb;
    OID            *type;
    int             status;

    /* assume no match */
    status = 0;

    /* make sure the interface exists */
    vb = i_GetMib(source);

    /* got an interface, make sure it's ethernet */
    if (vb != 0) {
	FreeVarBind(vb);

	/* make an iftype inquiry */
	type = CloneOID(source);
	if (type != 0) {
	    /*
	     * convert to ifType.X and get the variable (the -2 skips back
	     * over the instance number)
	     */
	    type->oid_ptr[type->length - 2] = 3;
	    vb = i_GetMib(type);
	    FreeOID(type);

	    /* make sure we got a response */
	    if (vb != 0) {
		/* the oid exists, check its value */
		if (vb->value.sl_value == IFT_CSMACD
		    || vb->value.sl_value == IFT_ISO8023) {
		    status = 1;
		}
		FreeVarBind(vb);
	    }
	}
    }

    /* return the status */
    return (status);
}

#ifndef SR_SIF			/* single interface */
/* this routine compares data source oids.  It relies on the fact that the
 * data source is the oid form of ifindex.xxx, where xxx is the interface
 * number of an interface.  It returns 0 if the sources are identical, non-
 * zero otherwise. */
int
CmpDataSource(s1, s2)
    OID            *s1, *s2;
{
    int             length;

    /*
     * compare the last long in the oids, since all data sources should have
     * the same length
     */
    length = s1->length - 1;
    return (s1->oid_ptr[length] == s2->oid_ptr[length] ? 0 : 1);
}

#endif				/* SR_SIF */

/* this routine returns true if a packet is a broadcast packet */
int
IsBroadcast(p)
    unsigned char  *p;
{
    int             i;

    /* look for non-FF bytes */
    i = 0;
    while (*p == 0xFF && i < 6) {
	p++;
	i++;
    }

    return (i == 6 ? 1 : 0);
}

/* this routine maintains the rmon tables */
void
UpdateTables(psp)
    PacketState    *psp;
{
    /* update the various tables */
    UpdatePacketStatistics(psp);
    UpdateHistoryStatistics(psp);

#ifndef	SR_FTC			/* fast timer-cancel */
    CheckAlarmTable();
#endif				/* SR_FTC */

    /*
     * make sure this is a real packet, not a notification of drops or a
     * collision.
     */
    if (psp->drops != 0)
	NotifyBuffersOfError();
    if (psp->size != 0) {
	ProcessHostEntry(psp);
	ProcessMatrixEntry(psp);
	ProcessFilters(psp);
	ProcessChannels(psp);
	ProcessPacketCapture(psp);
    }
}
