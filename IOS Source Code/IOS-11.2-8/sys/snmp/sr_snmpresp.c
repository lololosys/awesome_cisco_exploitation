/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_snmpresp.c,v 3.2 1995/11/17 19:01:26 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/snmp/sr_snmpresp.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, SNMP Research
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_snmpresp.c,v $
 * Revision 3.2  1995/11/17  19:01:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:18:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:50:51  hampton
 * Bump version numbers from 1.x to 2.x.
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

#if (! ( defined(lint) ) && defined(SR_RCSID))
static char     rcsid[] = "snmpresp.c,v 1.10 1995/02/06 18:11:40 battle Exp";
#endif				/* (! ( defined(lint) ) && defined(SR_RCSID)) */

/*
 * Revision History:
 *
 * snmpresp.c,v
 * Revision 1.10  1995/02/06  18:11:40  battle
 * Changes for vrtx.
 *
 * Revision 1.9  1994/10/28  17:32:20  vaughn
 * Placed new copyright in the file
 *
 * Revision 1.8  1994/10/26  21:58:42  vaughn
 * Changed copyrights to the new copyright structure
 *
 * Revision 1.7  1994/09/30  16:11:27  battle
 * hprt changes
 *
 * Revision 1.6  1994/08/04  15:37:41  battle
 * pSOS 2.0 changes
 *
 * Revision 1.5  1994/02/14  16:42:23  pearson
 * Updated copyright notices for 1994.
 *
 * AMP
 *
 * Revision 1.4  1994/02/08  00:10:10  pearson
 * Here's a brief description of the changes in this commit of the source
 * tree:
 * 	The structure of the packaged tree has changed somewhat.
 * 	Assembly language files and system-specific startup code are in
 * 	the pkg/lib/sys directory.  There is a 'serial' directory which
 * 	contains rs232 code for all of the different platforms.
 *
 * 	Uart code has a more uniform initialization call now.  For
 * 	example, init_uart() is mapped to init code for different uarts
 * 	on different systems.
 *
 * 	The national semiconductor "rib" code is in this release, along
 * 	with support for the debug/rt debugger and paradigm locate.
 * 	That created some new directories in the pkg/cmd tree.  There is
 * 	support now for a sonic driver, the ethernet repeater mib, and
 * 	sonic/ric statistics.
 *
 * 	The dos agent and utilities now configure themselves from an
 * 	ascii text file, rather than a "device driver".
 *
 * 	Various parts of the standard agent have had large stack
 * 	variables moved to malloc-ed structures to save room.
 *
 * 	Some bugs which were exposed by the marshall/levi test tool have
 * 	been fixed.
 *
 * 	Two new hello programs have been created.  Hello3 is essentially
 * 	a ping server, which allows embedded developers to test their
 * 	network stack, and hello4 allows tests of bootp and tftp.
 *
 * 	Some new routines have been added to the pkg/lib/c directory.
 * 	Dmemdump dumps memory in hex and in ascii.  32-bit versions of
 * 	memcpy, memset, etc have been created.
 *
 * Revision 1.3  1993/12/23  18:38:43  battle
 * Added cvs headers to all .c and .h files.
 *
 *
 *  2/4/89 JDC
 *  amended copyright notice
 *  changed references from "gotone" to "snmpd"
 *
 *  7/13/89 JDC
 *  worked on the error messages a bit
 *
 *  11/8/89 JDC
 *  NULL out_pdu_ptr after call to FreePdu two places in do_response
 *
 *  11/8/89 JDC
 *  Make it print pretty via tgrind
 *
 *  27-Jan-91 JDC
 *  Drop the "global" from the name of the MIB variables in the SNMP group
 *
 *  20-May-91 JDC
 *  Fix make_error_pdu to handle all types of octet strings
 *
 *  20-May-91 JDC
 *  SET requests are to return a GetResponse PDU of identical form, i.e.,
 *    copy the input to the output -- not go diving as had been done in
 *    previous releases of this subroutine
 *
 *  27-May-91 JDC
 *  Make make_error_pdu more robust in the face of memory allocation failures
 *
 *  25-Jun-91 JDC/AMP
 *  Removed recursion from find_var.
 *  Put print statements into conditional compiles.
 *
 *  30-Jul-91 AMP
 *  Merged the MROM/PKG and UNIX trees.  For this file, this means
 *  porting the non-recursive version of find_var from the Unix world
 *  to the micro world.
 *
 *  18-Feb-92 MRD
 *  Added DPRINTF support
 *  Changed object lookup method (findObject)
 *  Other minor clean-up
 *
 *  14-Nov-92 KWK
 *  Cleaned up memory leaks in cases where we hit limit during processing.
 *
 * 9-Dec-1992 DLB
 *   Lint nits.
 *
 * 18-Dec-92 MGH
 *   Changed over to new security drafts.  The fourth parameter of
 *   do_smp_response is now target instead of dstParty.
 *
 * 7-May-93 MGH
 *   Changed the name of "session" to "serialNum".
 *
 * 18-May-93 MGH
 * Changed #include <diag.h> to #include "diag.h"
 * Changed #include <snmp.h> to #include "snmp.h"
 *
 * 8-June-93 MGH
 * Made sure snmpInTotalReqVars is incremented on get and getnext requests.
 *
 * 10-Jun-1993 DLB
 *   Changed all short parameters to int.  Short parameters are a bad idea
 *   for numerous reasons I won't go into here.
 *
 * 14-Jun-1993 DLB
 *   Changed serialNum to global scope so it can be shared by sv1resp.c.
 *
 * 18-Jun-1993 DLB
 *   Major reorganization and simplification.  Now uses the two routines
 *   GetNextObjectInstance and GetExactObjectInstance in sv1resp.c.
 *
 * 23-Jun-1993 DLB
 *   Merged sv1resp.c and sv2resp.c into snmpresp.c, made serialNum a
 *   local static variable again.
 *
 * 28-June-1993 MGH
 * Changed the name of asod to contextInfo.  Changed all code dealing with
 * "asod" accordingly.
 *
 * 20-Jul-1993 DLB
 *   Changed version to domain.
 */

#include "master.h"





#include "sr_diag.h"
#include "sr_snmp.h"
#include "sr_snmpd.h"
#include "sr_snmpmib2.h"
#include "logger.h"
#include "msg_snmp.c"

static int      serialNum = 0;
static int	hogSerialNum = 0;

/*
 * snmp cpu hog detector
 */
static void
snmp_cpu_hog_check (char *pduType,
		    OID *oid)
{
    char *oidstr;

    if (check_cpuhog() && (hogSerialNum != serialNum)) {
	oidstr = malloc(MAX_OCTET_STRING_SIZE);
	if (oidstr) {
	    MakeDotFromOID(oid, oidstr);
	    errmsg(&msgsym(CPUHOG, SNMP), pduType, oidstr);
	    hogSerialNum = serialNum;
	    free(oidstr);
	}
    }
}

/*------------------------------------------------------------
 * Process GET and GET-NEXT requests.
 *------------------------------------------------------------*/
Pdu            *
do_response(inpdu, contextInfo, limit, context, domain)
    Pdu            *inpdu;
    ContextInfo    *contextInfo;
    int             limit;
    OID            *context;
    int             domain;
{
    Pdu            *outpdu;
    VarBind        *invb, *outvb;
    int             searchType;
    int             varCounter;
    long            req;
    short           total_so_far = 0;
    int             error;
    long            view;
    long            FailedVarBind = 0;
    OID            *incoming;
#ifdef SR_SNMPv2
    long            non_repeaters;
    long            max_repetitions;
    long            N, M, R;
    long            i, r;
    int             All_endOfMibView;
    VarBind        *LastNonRepeater;

    LastNonRepeater = NULL;
#endif				/* SR_SNMPv2 */


    outpdu = NULL;
    outvb = NULL;
    invb = NULL;
    serialNum++;

    searchType = inpdu->type;

#ifdef SR_SNMPv2
    if (searchType == GET_BULK_REQUEST_TYPE) {
	searchType = NEXT;
    }
#endif				/* SR_SNMPv2 */

    req = inpdu->u.normpdu.request_id;

    /*
     * Allocate the pdu to use for the response if successful
     */
    if ((outpdu = MakePdu(GET_RESPONSE_TYPE, req, (long) NO_ERROR, 0L,
		 (OID *) NULL, (OctetString *) NULL, 0L, 0L, 0L)) == NULL) {

	return (NULL);
    }

    view = GetViewIndex(context);


#ifdef SR_SNMPv2
    if (inpdu->type != GET_BULK_REQUEST_TYPE) {
#endif				/* SR_SNMPv2 */

	varCounter = 0;

	/*
	 * For each variable, look up the object, and retrieve its value.
	 */
	for (invb = inpdu->var_bind_list; invb; invb = invb->next_var) {
	    incoming = invb->name;

	    if (searchType == EXACT) {
		outvb =
		    GetExactObjectInstance(incoming, view, contextInfo,
					   serialNum, domain);
	    }
	    else {		/* if(searchType == NEXT) */
		outvb =
		    GetNextObjectInstance(incoming, view, contextInfo,
					  serialNum, domain);
	    }

	    snmp_cpu_hog_check(searchType == EXACT ? "Get" : "GetNext",
			       incoming);

	    if (outvb == NULL) {
		FailedVarBind = varCounter + 1;
		error = NO_ACCESS_ERROR;

#ifdef SR_SNMPv2
		if (domain == SNMPv2_DOMAIN) {
		    error = GEN_ERROR;
		}
#endif				/* SR_SNMPv2 */

		goto fail;
	    }

	    total_so_far += outvb->name_size + outvb->value_size + 4;
	    /* 4 <-- 1 tag, 1 definite long 2, 2 length (worst case) */
	    if (total_so_far >= limit) {
		FreeVarBind(outvb);
		error = TOO_BIG_ERROR;
		FailedVarBind = varCounter + 1;
		goto fail;
	    }			/* if limit */
	    LinkVarBind(outpdu, outvb);
	    varCounter++;
	}			/* for */

	snmpData.snmpInTotalReqVars += varCounter;

	return outpdu;

#ifdef SR_SNMPv2
    }				/* endof not getbulk */
    else {			/* GET BULK request */

	/*
	 * Upon receipt of a GetBulkRequest-PDU, the receiving SNMPv2 entity
	 * processes each variable binding in the variable-binding list to
	 * produce a Response-PDU with its request-id field having the same
	 * value as in the request.  Processing begins by examining the
	 * values in the non-repeaters and max-repetitions fields.  If the
	 * value in the non-repeaters field is less than zero, then the value
	 * of the field is set to zero.  Similarly, if the value in the
	 * max-repetitions field is less than zero, then the value of the
	 * field is set to zero.
	 */

	non_repeaters = inpdu->u.bulkpdu.non_repeaters;
	max_repetitions = inpdu->u.bulkpdu.max_repetitions;

	if (non_repeaters < 0)
	    non_repeaters = 0;
	if (max_repetitions < 0)
	    max_repetitions = 0;

	/*
	 * The values of the non-repeaters and max-repetitions fields in the
	 * request specify the processing requested.  One variable binding in
	 * the Response-PDU is requested for the first N variable bindings in
	 * the request and M variable bindings are requested for each of the
	 * R remaining variable bindings in the request.  Consequently, the
	 * total number of requested variable bindings communicated by the
	 * request is given by N + (M * R), where N is the minimum of: a) the
	 * value of the non-repeaters field in the request, and b) the number
	 * of variable bindings in the request; M is the value of the
	 * max-repetitions field in the request; and R is the maximum of: a)
	 * number of variable bindings in the request - N, and b)  zero.
	 */

	N = MIN(non_repeaters, inpdu->num_varbinds);
	M = max_repetitions;
	R = MAX(inpdu->num_varbinds - N, 0);

	/*
	 * If N is greater than zero, the first through the (N)-th variable
	 * bindings of the Response-PDU are each produced as follows:
	 * 
	 * (1)  The variable is located which is in the lexicographically
	 * ordered list of the names of all variables which are accessible by
	 * this request and whose name is the first lexicographic successor
	 * of the variable binding's name in the incoming GetBulkRequest-PDU.
	 * The corresponding variable binding's name and value fields in the
	 * Response-PDU are set to the name and value of the located
	 * variable.
	 * 
	 * (2)  If the requested variable binding's name does not
	 * lexicographically precede the name of any variable accessible by
	 * this request, i.e., there is no lexicographic successor, then the
	 * corresponding variable binding produced in the Response-PDU has
	 * its value field set to `endOfMibView', and its name field set to
	 * the variable binding's name in the request.
	 */
	for (i = 1, invb = inpdu->var_bind_list; i <= N; i++, invb = invb->next_var) {
	    outvb =
		GetNextObjectInstance(invb->name, view, contextInfo, serialNum, 2);

	    snmp_cpu_hog_check("GetBulk", invb->name);

	    if (outvb == NULL) {
		FailedVarBind = i;
		error = GEN_ERROR;
		goto fail;
	    }

	    total_so_far += outvb->name_size + outvb->value_size + 4;
	    /* 4 <-- 1 tag, 1 definite long 2, 2 length (worst case) */
	    if (total_so_far >= limit) {
		FreeVarBind(outvb);
		goto done;
	    }			/* if limit */
	    LinkVarBind(outpdu, outvb);
	}
	LastNonRepeater = outvb;

	/*
	 * If M and R are non-zero, the (N + 1)-th and subsequent variable
	 * bindings of the Response-PDU are each produced in a similar
	 * manner.  For each iteration i, such that i is greater than zero
	 * and less than or equal to M, and for each repeated variable, r,
	 * such that r is greater than zero and less than or equal to R, the
	 * (N + ( (i-1) * R ) + r)-th variable binding of the Response-PDU is
	 * produced as follows:
	 * 
	 * (1)  The variable which is in the lexicographically ordered list of
	 * the names of all variables which are accessible by this request
	 * and whose name is the (i)-th lexicographic successor of the (N +
	 * r)-th variable binding's name in the incoming GetBulkRequest-PDU
	 * is located and the variable binding's name and value fields are
	 * set to the name and value of the located variable.
	 * 
	 * (2)  If there is no (i)-th lexicographic successor, then the
	 * corresponding variable binding produced in the Response- PDU has
	 * its value field set to `endOfMibView', and its name field set to
	 * either the last lexicographic successor, or if there are no
	 * lexicographic successors, to the (N + r)-th variable binding's
	 * name in the request.
	 */

	for (i = 1; i <= M; i++) {
	    All_endOfMibView = 1;
	    if (i == 2) {
		if (LastNonRepeater != NULL) {
		    invb = LastNonRepeater->next_var;
		}
		else {
		    invb = outpdu->var_bind_list;
		}
	    }
	    for (r = 1; r <= R; r++, invb = invb->next_var) {
		incoming = invb->name;

		outvb = GetNextObjectInstance(incoming, view, contextInfo,
					      serialNum, 2);

		snmp_cpu_hog_check("GetBulk", invb->name);

		if (outvb == NULL) {
		    FailedVarBind = N + r;
		    error = GEN_ERROR;
		    goto fail;
		}

		if (outvb->value.type != END_OF_MIB_VIEW_EXCEPTION) {
		    All_endOfMibView = 0;
		}

		total_so_far += outvb->name_size + outvb->value_size + 4;
		/* 4 <-- 1 tag, 1 definite long 2, 2 length (worst case) */
		if (total_so_far < limit) {
		    LinkVarBind(outpdu, outvb);
		}		/* if limit */
		else {
		    FreeVarBind(outvb);
		    goto done;
		}
	    }
	    if (All_endOfMibView)
		goto done;
	}

done:

	/*
	 * While the maximum number of variable bindings in the Response-PDU
	 * is bounded by N + (M * R), the response may be generated with a
	 * lesser number of variable bindings (possibly zero) for either of
	 * two reasons.
	 * 
	 * (1)  If the size of the message encapsulating the Response-PDU
	 * containing the requested number of variable bindings would be
	 * greater than either a local constraint or the maximum message size
	 * of the request's source party, then the response is generated with
	 * a lesser number of variable bindings.  This lesser number is the
	 * ordered set of variable bindings with some of the variable
	 * bindings at the end of the set removed, such that the size of the
	 * message encapsulating the Response-PDU is approximately equal to
	 * but no greater than the minimum of the local constraint and the
	 * maximum message size of the request's source party.  Note that the
	 * number of variable bindings removed has no relationship to the
	 * values of N, M, or R.
	 * 
	 * (2)  The response may also be generated with a lesser number of
	 * variable bindings if for some value of iteration i, such that i is
	 * greater than zero and less than or equal to M, that all of the
	 * generated variable bindings have the value field set to the
	 * `endOfMibView'.  In this case, the variable bindings may be
	 * truncated after the (N + (i R))-th variable binding.
	 */

	return outpdu;

	/*
	 * If the processing of any variable binding fails for a reason other
	 * than listed above, then the Response-PDU is re-formatted with the
	 * same values in its request-id and variable-bindings fields as the
	 * received GetBulkRequest-PDU, with the value of its error-status
	 * field set to `genErr', and the value of its error-index field is
	 * set to the index of the failed variable binding.
	 */
    }				/* if BULK */
#endif				/* SR_SNMPv2 */

fail:
    if (outpdu != NULL)
	FreePdu(outpdu);
    return make_error_pdu(GET_RESPONSE_TYPE, req, error,
			  (int) FailedVarBind, inpdu,
			  domain);
}
