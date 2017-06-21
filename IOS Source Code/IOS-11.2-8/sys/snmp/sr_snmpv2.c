/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_snmpv2.c,v 3.2 1995/11/17 19:01:34 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/snmp/sr_snmpv2.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, SNMP Research
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_snmpv2.c,v $
 * Revision 3.2  1995/11/17  19:01:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:18:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:50:58  hampton
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
static char     rcsid[] = "snmpv2.c,v 1.13 1994/11/02 18:20:11 battle Exp";
#endif				/* (! ( defined(lint) ) && defined(SR_RCSID)) */

/*
 * Revision History:
 *
 * snmpv2.c,v
 * Revision 1.13  1994/11/02  18:20:11  battle
 * added an ARGSUSED
 *
 * Revision 1.12  1994/10/28  17:32:22  vaughn
 * Placed new copyright in the file
 *
 * Revision 1.11  1994/10/26  21:58:44  vaughn
 * Changed copyrights to the new copyright structure
 *
 * Revision 1.10  1994/08/04  15:37:43  battle
 * pSOS 2.0 changes
 *
 * Revision 1.9  1994/03/03  15:42:07  vaughn
 * Changed 0's in DPRINTF to APALWAYS.
 *
 * Revision 1.8  1994/02/14  16:42:25  pearson
 * Updated copyright notices for 1994.
 *
 * AMP
 *
 * Revision 1.7  1994/02/09  20:50:42  hecht
 * Changed the calling interface to do_trap() by adding a new parameter,
 * entV2Trap, to the end of the parameter list. All calls to do_trap() in this
 * file are standard traps, so this parameter has a value of (char *) NULL.
 *
 * Revision 1.6  1994/02/04  20:01:52  hecht
 * Changed the interface to do_snmpv2() by adding the new parameter, outLen.
 * Added the parameter, outLen, to the call of findContextInfo.
 *
 * Revision 1.5  1994/01/12  21:44:07  key
 * Added call to NormalizeVarBindListLengths() to fix up VarBind lists
 * that where taken literally from the incoming PDU to the outgoing PDU.
 * This affected sets processing and make_error_pdu().
 *
 * Revision 1.4  1994/01/11  23:10:28  key
 * Fixed SET bug where we were not clearing error_status and error_index after
 * moving the incoming PDU onto the outgoing PDU pointer.
 *
 * Revision 1.3  1993/12/23  18:38:45  battle
 * Added cvs headers to all .c and .h files.
 *
 *
 * 04-Jul-1992 JDC
 * Split out SMP support from snmp.c for clarity and maintainability
 *
 * 9-Dec-1992 DLB
 *   Lint nits.
 *
 * 18-Dec-92 MGH
 *   Changing do_smp() to conform to the new security drafts. Changed the
 *   number of parameters in the call to MakeAuthentication(), and added a
 *   targetIndex to the auth_ptr(AuthHeader structure).
 *
 *   Changed calls to CmpNOID to CmpOIDValues.
 *
 * 7-Jan-1992 MGH
 * Changed targetIndex(in the AuthHeader structure) to contextIndex.
 * The variable, basetime, is now an unsigned long.  dst_timestamp and
 * src_timestamp are now unsigned longs.
 *
 * 18-Jan-1993 MGH
 * Added snmpStats counters.
 *
 * 15-Feb-1993 MGH
 * Moved packaging of return pdu up one level to do_mgmt().
 *
 * 6-Apr-1993 MGH
 * Started adding SNMPv2->SNMPv2 and SNMPv2->SNMPv1 proxy support.
 *
 * 11-May-1993 MGH
 * Changed the name of the parameter, inPdu, to inPkt since the parameter
 * is still a packet and not a pdu.
 *
 * 18-May-93 MGH
 * Changed #include <diag.h> to #include "diag.h"
 *
 * 8-June-93 MGH
 * Made sure that the snmpInGetRequests, snmpInGetNexts, snmpInSetRequests,
 * and snmpInGetResponses counters are incremented when that type of pdu
 * comes in.
 *
 * 10-Jun-1993 DLB
 * Changed all short parameters to int.  Short parameters are a bad idea
 * for numerous reasons I won't go into here.
 *
 * 25-June-1993 MGH
 * Moved comparison of dstParty and privDst to ParseAuthentication() (in
 * auth_lib.c).  Changed call of findAsod() to findContextInfo(), and
 * changed call references to asod to contextInfo.
 */

#include "master.h"

#include "sr_diag.h"


#include "sr_snmpd.h"
#include "sr_sysmib2.h"
#include "sr_snmpmib2.h"
#include "sr_snmpv2mib.h"

extern unsigned long basetime;

#ifdef SR_SNMPv2

/*------------------------------------------------------------
 * Process an snmpv2 packet.
 *------------------------------------------------------------*/
/*ARGSUSED*/
Pdu            *
do_snmpv2(inPkt, inLen, srcIpAddr, srcUdpPort, dstIpAddr, dstUdpPort,
	  in_auth_ptr, outLen)
    unsigned char   inPkt[];
    unsigned int    inLen;
    unsigned long   srcIpAddr;
    unsigned int    srcUdpPort;
    unsigned long   dstIpAddr;
    unsigned int    dstUdpPort;
    AuthHeader     *in_auth_ptr;
    unsigned int    outLen;
{
    Pdu            *out_pdu_ptr, *in_pdu_ptr;
    ContextInfo    *contextInfo;
    int             srcPartyIndex;
    int             dstPartyIndex;
    int             contextIndex;
    int             acl_privs = 0;	/* MGH 12-Jun-92 */
    int             pos;	/* MGH 12-Jun-92 */
    int             error_code;
    long            req;	/* MGH 12-Jun-92 */
    short           limit;

    out_pdu_ptr = in_pdu_ptr = (Pdu *) NULL;
    error_code = 0;

/*****************************************************************************/
/* From RFC1445, pages 18-19, section 3.2:                                   */
/*    								             */
/*     (1)  (handled in do_mgmt() in layer above).                           */
/*                                                                           */
/*     (2)  (handled in ParseAuthentication() call in layer above).         */
/*                                                                           */
/*     (3)  (handled in ParseAuthentication() call in layer above).         */
/*    								             */
/*****************************************************************************/
    dstPartyIndex = in_auth_ptr->dstPartyIndex;

    /* see if domain is ok */
    if (parties[dstPartyIndex].party->domain != SNMPv2_DOMAIN) {
	DPRINTF((APTRACE, "do_snmpv2: received query or command with unknown destination domain -- dropping packet\n"));
	goto cleanup;
    }

/* we know the address and port are ok or we would not have been upcalled */

/*****************************************************************************/
/*    								             */
/*     (4) (handled in call to ParseAuthentication in layer above)          */
/*    								             */
/*     (5) (handled in call to ParseAuthentication in layer above)          */
/*    								             */
/*     (6) (handled in call to ParseAuthentication in layer above)          */
/*    								             */
/*     (7) (handled in call to ParseAuthentication in layer above)          */
/*    								             */
/*     (8) (handled in call to ParseAuthentication in layer above)          */
/*    								             */
/*     (9) (handled in call to ParseAuthentication in layer above)          */
/*    								             */
/*     (9) (handled in call to ParseAuthentication in layer above)          */
/*    								             */
/*     10. If the SnmpAuthMsg value is evaluated as unauthentic,             */
/*         then the received message is discarded without further            */
/*         processing, and if the snmpV2EnableAuthenTraps object [7]         */
/*         is enabled, then the SNMPv2 entity sends                          */
/*         authorizationFailure traps [7] according to its                   */
/*         configuration (Section 4.2.6 of[2]).                              */
/*    								             */
/*****************************************************************************/

    srcPartyIndex = in_auth_ptr->srcPartyIndex;

    if ((contextInfo = findContextInfo(srcIpAddr, in_auth_ptr, &limit,
				       &error_code, outLen)) == NULL) {

#ifdef SR_SNMPv1
	if (in_auth_ptr->authType == TRIVIAL_AUTHENTICATION) {
	    snmpV1Data.snmpV1BadCommunityNames++;
	}
#endif				/* SR_SNMPv1 */

	if (error_code == -1) {
	    DPRINTF((APTRACE, "do_snmpv2: Authentication failure\n"));
	    snmp_authentication_trap(SNMPv2_DOMAIN, (addrtype *)srcIpAddr);
	}
	goto cleanup;
    }

/*****************************************************************************/
/*    (11) The SnmpMgmtCom value is extracted from                           */
/*         the authData component of the SnmpAuthMsg                         */
/*         value.                                                            */
/*****************************************************************************/
    if ((in_pdu_ptr = ParsePdu(in_auth_ptr)) == NULL) {
	snmpData.snmpInASNParseErrs++;
	snmpStatsData.snmpStatsEncodingErrors++;
	DPRINTF((APTRACE, "do_snmpv2: Error parsing pdu packlet\n"));
	goto cleanup;
    }
/*****************************************************************************/
/*									     */
/*    (12) (handled in call to ParseAuthentication in layer above)          */
/*									     */
/*    (13) (handled in call to ParseAuthentication in layer above)          */
/*									     */
/*****************************************************************************/

    contextIndex = in_auth_ptr->contextIndex;

/*****************************************************************************/
/*									     */
/*    (14) The local database of access policy information is                */
/*         consulted for access privileges permitted by the local            */
/*         access policy to the originating SNMPv2 party with                */
/*         respect to the receiving SNMPv2 party and the indicated           */
/*         SNMPv2 context.                                                   */
/*****************************************************************************/
/* The view tree privileges will be checked in do_response or do_sets */

    if ((pos = GetAclIndex(contexts[contextIndex].context->contextIndex,
			   parties[srcPartyIndex].party->partyIndex,
			 parties[dstPartyIndex].party->partyIndex)) == -1) {
	DPRINTF((APTRACE, "do_snmpv2: No acl entry for context, source and dest party\n"));
	goto cleanup;
    }
    else {			/* Found acl entry */
	acl_privs = (int) access_control[pos].acl->aclPrivileges;
    }

/*****************************************************************************/
/*									     */
/*    (15) The management communication class is determined from the         */
/*         ASN.1 tag value associated with the PDUs component of the         */
/*         SnmpMgmtCom value.  If the management information class           */
/*         of the received message is either 32, 8, 2, or 1 (i.e.,           */
/*         GetBulk, Set, GetNext or Get) and the SNMPv2 context is           */
/*         not realized by the local SNMPv2 entity, then the                 */
/*         received message is discarded without further processing,         */
/*         after the snmpStatsUnknownContexts counter [7] is                 */
/*         incremented.                                                      */
/*									     */
/*    (16) If the management communication class of the received             */
/*         message is either 128 or 4 (i.e., SNMPv2-Trap or                  */
/*         Response) and this class is not among the access                  */
/*         privileges, then the received message is discarded                */
/*         without further processing, after the                             */
/*         snmpStatsBadOperations counter [7] is incremented.                */
/*									     */
/*    (17) If the management communication class of the received             */
/*         message is not among the access privileges, then the              */
/*         received message is discarded without further processing          */
/*         after generation and transmission of a response message.          */
/*         This response message is directed to the originating              */
/*         SNMPv2 party on behalf of the receiving SNMPv2 party.             */
/*         Its context, var-bind-list and request-id components are          */
/*         identical to those of the received request.  Its error-           */
/*         index component is zero and its error-status component is         */
/*         authorizationError [2].                                           */
/*									     */
/*    (18) If the SNMPv2 context refers to local object resources,           */
/*         then the management operation represented by the 		     */
/*         SnmpMgmtCom value is performed by the receiving SNMPv2  	     */
/*         entity with respect to the MIB view identified by the             */
/*         SNMPv2 context according to the procedures set forth in           */
/*         [2]. 							     */
/*									     */
/*     19. If the SNMPv2 context refers to remote object resources,          */
/*         then the management operation represented by the                  */
/*         SnmpMgmtCom value is performed through the appropriate            */
/*         proxy relationship. 						     */
/*****************************************************************************/

    req = in_pdu_ptr->u.normpdu.request_id;

    switch (in_pdu_ptr->type) {

      case GET_REQUEST_TYPE:
	snmpData.snmpInGetRequests++;
	if (contexts[contextIndex].context->contextLocal != SR_LOCAL) {
	    snmpStatsData.snmpStatsUnknownContexts++;
	    DPRINTF((APWARN, "do_snmpv2: The context is not local for get request\n"));
	    goto cleanup;
	}

	if ((acl_privs & GET_MASK) != GET_MASK) {
	    DPRINTF((APTRACE, "do_snmpv2: Not allowed to do get requests\n"));
	    if ((out_pdu_ptr = make_error_pdu(GET_RESPONSE_TYPE,
					      req, AUTHORIZATION_ERROR, 0, in_pdu_ptr, SNMPv2_DOMAIN)) == NULL) {
		goto cleanup;
	    }
	    break;
	}
	if (contexts[contextIndex].context->contextViewIndex != 0) {
	    if ((out_pdu_ptr = do_response(in_pdu_ptr, contextInfo, limit,
					   in_auth_ptr->context,
					   SNMPv2_DOMAIN)) == NULL) {
		goto cleanup;
	    }
	}


	break;

      case GET_NEXT_REQUEST_TYPE:
	snmpData.snmpInGetNexts++;
	if (contexts[contextIndex].context->contextLocal != SR_LOCAL) {
	    snmpStatsData.snmpStatsUnknownContexts++;
	    DPRINTF((APWARN, "do_snmpv2: The context is not local for get-next request\n"));
	    goto cleanup;
	}

	if ((acl_privs & GET_NEXT_MASK) != GET_NEXT_MASK) {
	    DPRINTF((APTRACE, "do_snmpv2: Not allowed to do get-next requests\n"));
	    if ((out_pdu_ptr = make_error_pdu(GET_RESPONSE_TYPE,
					      req, AUTHORIZATION_ERROR, 0, in_pdu_ptr, SNMPv2_DOMAIN)) == NULL) {
		goto cleanup;
	    }
	    break;
	}
	if (contexts[contextIndex].context->contextViewIndex != 0) {
	    if ((out_pdu_ptr = do_response(in_pdu_ptr, contextInfo, limit,
					   in_auth_ptr->context,
					   SNMPv2_DOMAIN)) == NULL) {
		goto cleanup;
	    }
	}


	break;

      case GET_BULK_REQUEST_TYPE:
	if (contexts[contextIndex].context->contextLocal != SR_LOCAL) {
	    snmpStatsData.snmpStatsUnknownContexts++;
	    DPRINTF((APWARN, "do_snmpv2: The context is not local for get bulk request\n"));
	    goto cleanup;
	}

	if ((acl_privs & BULK_MASK) != BULK_MASK) {
	    DPRINTF((APTRACE, "do_snmpv2: Not allowed to do getbulk requests\n"));
	    if ((out_pdu_ptr = make_error_pdu(GET_RESPONSE_TYPE,
					      req, AUTHORIZATION_ERROR, 0, in_pdu_ptr, SNMPv2_DOMAIN)) == NULL) {
		goto cleanup;
	    }
	    break;
	}
	if (contexts[contextIndex].context->contextViewIndex != 0) {
	    if ((out_pdu_ptr = do_response(in_pdu_ptr, contextInfo, limit,
					   in_auth_ptr->context,
					   SNMPv2_DOMAIN)) == NULL) {
		goto cleanup;
	    }
	}


	break;

      case SET_REQUEST_TYPE:
	snmpData.snmpInSetRequests++;
	if (limit <= 0) {
	    out_pdu_ptr = NULL;	/* too big */
	    snmpStatsData.snmpStatsSilentDrops++;
	    goto cleanup;	/* silently drop it */
	}

#ifdef SETS
	if (contexts[contextIndex].context->contextLocal != SR_LOCAL) {
	    snmpStatsData.snmpStatsUnknownContexts++;
	    DPRINTF((APWARN, "do_snmpv2: The context is not local for set request\n"));
	    goto cleanup;
	}
	if ((acl_privs & SET_MASK) != SET_MASK) {
	    DPRINTF((APTRACE, "do_snmpv2: Not allowed to do set requests\n"));
	    if ((out_pdu_ptr = make_error_pdu(GET_RESPONSE_TYPE,
					      req, AUTHORIZATION_ERROR, 0, in_pdu_ptr, SNMPv2_DOMAIN)) == NULL) {
		goto cleanup;
	    }
	    break;
	}
	if (limit < in_auth_ptr->packlet->length - in_auth_ptr->wrapper_size) {
	    if ((out_pdu_ptr = make_error_pdu(GET_RESPONSE_TYPE,
	       req, TOO_BIG_ERROR, 0, in_pdu_ptr, SNMPv2_DOMAIN)) == NULL) {
		goto cleanup;
	    }
	    break;
	}
	if (contexts[contextIndex].context->contextViewIndex != 0) {
	    if ((out_pdu_ptr = do_sets(in_pdu_ptr, contextInfo,
			    in_auth_ptr->context, SNMPv2_DOMAIN)) == NULL) {
		out_pdu_ptr = in_pdu_ptr;
		out_pdu_ptr->type = GET_RESPONSE_TYPE;
		out_pdu_ptr->u.normpdu.error_status = 0;
		out_pdu_ptr->u.normpdu.error_index = 0;
		NormalizeVarBindListLengths(out_pdu_ptr->var_bind_list);
		in_pdu_ptr = NULL;
	    }
	}


	break;
#endif				/* SETS */

      case GET_RESPONSE_TYPE:
	snmpData.snmpInGetResponses++;
	if ((acl_privs & GET_RESPONSE_MASK) != GET_RESPONSE_MASK) {
	    DPRINTF((APTRACE, "do_snmpv2: Not allowed to do get responses\n"));
	    snmpStatsData.snmpStatsBadOperations++;
	    goto cleanup;
	}


	break;

      case INFORM_REQUEST_TYPE:
	if ((acl_privs & INFORM_MASK) != INFORM_MASK) {
	    DPRINTF((APTRACE, "do_snmpv2: Not allowed to do inform requests\n"));
	    snmpStatsData.snmpStatsBadOperations++;
	}
	if ((out_pdu_ptr = make_error_pdu(GET_RESPONSE_TYPE,
	 req, AUTHORIZATION_ERROR, 0, in_pdu_ptr, SNMPv2_DOMAIN)) == NULL) {
	    goto cleanup;
	}
	break;

      case SNMPv2_TRAP_TYPE:
	if ((acl_privs & SNMPv2_TRAP_MASK) != SNMPv2_TRAP_MASK) {
	    DPRINTF((APTRACE, "do_snmpv2: Not allowed to do SNMPv2 traps.\n"));
	    snmpStatsData.snmpStatsBadOperations++;
	}
	break;

      default:
	DPRINTF((APALWAYS, "do_snmpv2: Internal error. (Invalid packet type)\n"));
	goto cleanup;
    }				/* switch */

cleanup:
    if (in_pdu_ptr != NULL) {
	FreePdu(in_pdu_ptr);
    }

    return (out_pdu_ptr);
}				/* do_snmpv2() */

#endif				/* SR_SNMPv2 */
