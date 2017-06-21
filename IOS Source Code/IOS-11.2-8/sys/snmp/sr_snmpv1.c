/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_snmpv1.c,v 3.3 1996/01/20 02:36:10 jjohnson Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/snmp/sr_snmpv1.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, SNMP Research
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_snmpv1.c,v $
 * Revision 3.3  1996/01/20  02:36:10  jjohnson
 * CSCdi47220:  need snmp packet debugging
 *
 * Revision 3.2  1995/11/17  19:01:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:18:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:50:56  hampton
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
static char     rcsid[] = "snmpv1.c,v 1.12 1994/11/02 18:19:57 battle Exp";
#endif				/* (! ( defined(lint) ) && defined(SR_RCSID)) */

/*
 * Revision History:
 *
 * snmpv1.c,v
 * Revision 1.12  1994/11/02  18:19:57  battle
 * added an ARGSUSED
 *
 * Revision 1.11  1994/10/28  17:32:21  vaughn
 * Placed new copyright in the file
 *
 * Revision 1.10  1994/10/26  21:58:43  vaughn
 * Changed copyrights to the new copyright structure
 *
 * Revision 1.9  1994/08/04  15:37:42  battle
 * pSOS 2.0 changes
 *
 * Revision 1.8  1994/03/03  15:42:05  vaughn
 * Changed 0's in DPRINTF to APALWAYS.
 *
 * Revision 1.7  1994/02/14  16:42:24  pearson
 * Updated copyright notices for 1994.
 *
 * AMP
 *
 * Revision 1.6  1994/02/09  20:49:14  hecht
 * Changed the calling interface to do_trap() by adding a new parameter,
 * entV2Trap, to the end of the parameter list. All calls to do_trap() in this
 * file are standard traps, so this parameter has a value of (char *) NULL.
 *
 * Revision 1.5  1994/02/04  20:01:05  hecht
 * Changed the interface to do_snmpv1() by adding the new parameter, outLen.
 * Added the parameter, outLen, to the call of findContextInfo.
 *
 * Revision 1.4  1994/01/12  21:44:06  key
 * Added call to NormalizeVarBindListLengths() to fix up VarBind lists
 * that where taken literally from the incoming PDU to the outgoing PDU.
 * This affected sets processing and make_error_pdu().
 *
 * Revision 1.3  1994/01/11  23:09:39  key
 * Fixed SET bug where were we not clearing error_index and error_status
 * after moving the incoming PDU onto the outgoing PDU pointer.
 *
 * Revision 1.2  1993/12/23  18:38:44  battle
 * Added cvs headers to all .c and .h files.
 *
 *
 * 2/4/89 JDC
 *     Amended copyright notice Revised references from "gotone" to
 *     "snmpd" Added support for ifLastChange ... initialize values to zero,
 *     thereby implying ifLastChange = time of startup changed all references
 *     from "session" to "community" etc
 *
 * 2/6/89 JDC
 *     Made daemon load initial value of sysObjectID from snmpd.config
 *     with default value and altered trap sending code to use that value rather
 *     than a hardcoded value Added documentation and comments Professionalized
 *     error messages a bit
 *
 * 4/24/89 JDC
 *     Turned off some verbose debugging output to quiet some of the noise
 *
 * 5/17/89 KWK
 *     converted all conditional compilation from SUN3 and SUN4 to
 *     SUNOS35, SUNOS40, and added support for SUN386i
 *
 * 6/3/89 JDC
 *     commented KWK's changes above of 5/17/89
 *
 * 6/3/89 JDC
 *     added -v qualifier to command line -v makes it verbose ... not
 *     verbose is the default -v amends the value of LogLevel routed all error
 *     messages to stderr
 *
 * 7/13/89 JDC
 *     got superfluous variables out of namelist from NSFnet backbone
 *     project converted botched thashsize to rthashsize in namelist (how'd it
 *     break?)
 *
 * 7/15/89 JDC
 *     fixed up error messages a bit references to sysObjectID are now
 *     symbolic
 *
 * 11/1/89 JDC
 *     add frees and ptrs to NULL on error exit from do_trap fixed
 *     serious problem with double free of malloc'd space set a few ptrs to
 *     NULL after free-ing them
 *
 * 11/8/89 JDC
 *     Make it print pretty via tgrind
 *
 * 03/1/90 JDC
 *     inet_ntoa() core dumps on sparc
 *
 * 03/11/90 JDC
 *     got rid of superfluous 0 argument on call to bind()
 *
 * 09/22/90 KWK
 *     Added MIB-II support
 *
 * 25-Jun-91 AMP
 *     Added ip_forwarding to the nlist structure array; this is used
 *     to read the kernel's gateway/host switch.
 *
 * 27-Jun-91 AMP
 *     Added a check to see if we ran out of tries on binding to our
 *     socket.  If we did, print an error message and exit.
 *
 * 30-Jul-91 AMP
 *     Merged the MOT/PKGAGENT and UNIX trees
 *
 * 05-Aug-91 AMP and JDC
 *     Get rid of all references to remote_port in trap_send
 *     and related code
 *
 * 12-Feb-92
 *     Added DPRINTF support
 *
 * 27-Aug-92 MGH
 * Modified do_trivial_snmp() to allow access_control if the community
 * string appears in the party table (asod->partyIdentity != NULL)
 *
 * 9-Dec-1992 DLB
 * Lint nits.
 *
 * 18-Dec-92 MGH
 * Changed calls to MakeAuthentication (when SECURE is defined)
 *
 * 19-Jan-93 MGH
 * Added code to increment the snmpV1BadCommunityNames counter when needed.
 * Rewrote code dealing with community strings in the party table.
 *
 * 15-Feb-93 MGH
 * Moved packaging of return pdu up one level to do_mgmt().
 *
 * 12-Apr-93 MGH
 * Added code to GET_RESPONSE_TYPE to handle proxy responses.
 *
 * 11-May-93 MGH
 * Changed the name of the inPdu parameter to inPkt because the parameter
 * is still a packet and not a pdu.
 *
 * 18-May-93 MGH
 * Changed #include <diag.h> to #include "diag.h"
 *
 * 8-June-93 MGH
 * Made sure snmpInBadCommunityUses was incremented correctly.
 *
 * 10-Jun-1993 DLB
 * Changed all short parameters to int.  Short parameters are a bad idea
 * for numerous reasons I won't go into here.
 *
 * 24-June-1993 MGH
 * Moved check of version from do_snmpv1() to
 * ParseAuthentication() (auth_lib.c). Removed check for partyIdentity
 * not equal to NULL before setting acl_privs.  If this is a bilingual
 * agent, all community strings that are available are in the party table.
 *
 * 28-June-1993 MGH
 * Changed the call to findAsod() to findContextInfo().  Made appropriate
 * changes to the if block.
 *
 * 1-July-1993 MGH
 * Moved the check for a community string having read-write privileges from
 * do_sets() to before the call to do_sets().  This only affects v1 agents.
 */

#include "master.h"
#include "sr_diag.h"


#include "sr_snmpd.h"
#include "snmp_registry.h"
#include "sr_sysmib2.h"
#include "sr_snmpmib2.h"
#include "sr_snmpv2mib.h"
#include "snmp_debug.h"

/*------------------------------------------------------------
 * Process a trivial (rfc1157) snmp packet.
 *------------------------------------------------------------*/
/*ARGSUSED*/
Pdu            *
do_snmpv1(inPkt, inLen, srcIpAddr, srcUdpPort, in_auth_ptr, outLen)
    unsigned char   inPkt[];
    unsigned int    inLen;
    unsigned long   srcIpAddr;
    unsigned int    srcUdpPort;
    AuthHeader     *in_auth_ptr;
    unsigned int    outLen;
{
    Pdu            *out_pdu_ptr, *in_pdu_ptr;

#ifdef SR_SNMPv2
    int             do_acl_check = FALSE;
    int             acl_privs, pos;
    int             context_index;
    long            req;
#endif				/* SR_SNMPv2 */
    int             error_code;
    ContextInfo    *contextInfo;
    short           limit;

    out_pdu_ptr = in_pdu_ptr = (Pdu *) NULL;
    error_code = 0;


    if ((contextInfo = findContextInfo(srcIpAddr, in_auth_ptr, &limit,
				       &error_code, outLen)) == NULL) {

#ifdef SR_SNMPv2
	if (in_auth_ptr->authType == TRIVIAL_AUTHENTICATION) {
	    snmpData.snmpInBadCommunityNames++;
	    snmpV1Data.snmpV1BadCommunityNames++;
	}
#else				/* !SR_SNMPv2 */
	snmpData.snmpInBadCommunityNames++;
#endif				/* SR_SNMPv2 */

	if (error_code == -1) {
	    DPRINTF((APTRACE, "do_snmpv1: Authentication failure\n"));
	    snmp_authentication_trap(SNMPv1_DOMAIN, (addrtype *)srcIpAddr);
	}
	if (snmp_packet_debug)
	    buginf("\nSNMP: authentication failure");
	goto cleanup;
    }

    if ((in_pdu_ptr = ParsePdu(in_auth_ptr)) == NULL) {
	snmpData.snmpInASNParseErrs++;

#ifdef SR_SNMPv2
	snmpStatsData.snmpStatsEncodingErrors++;
#endif				/* SR_SNMPv2 */

	DPRINTF((APTRACE, "do_snmpv1: Error parsing pdu packlet\n"));
	if (snmp_packet_debug)
	    buginf("\nSNMP: Error parsing packet");
	goto cleanup;
    }

    if (snmp_packet_debug)
	snmp_print_message(in_pdu_ptr);

#ifdef SR_SNMPv2
    do_acl_check = TRUE;

    acl_privs = 0;

    if (do_acl_check == TRUE) {

	/*
	 * Find an entry in the acl table that matches the community string.
	 * There should only be one.  There is no incoming context because
	 * this is a SNMPv1 packet.
	 */

	if ((pos = GetAclEntry(contextInfo->u.PartyCommunity->partyIndex,
			contextInfo->u.PartyCommunity->partyIndex)) == -1) {
	    DPRINTF((APTRACE, "do_snmpv1: No acl entry for community string\n"));
	    if (snmp_packet_debug)
		buginf("\nSNMP: No acl for community string");

	    goto cleanup;
	}
	else {
	    acl_privs = (int) access_control[pos].acl->aclPrivileges;
	}

	/* Make sure context exists in context table */
	if (((context_index =
	   GetContextEntry(access_control[pos].acl->aclResources)) == -1) ||
	    (contexts[context_index].context->contextLocal == SR_REMOTE)) {
	    DPRINTF((APTRACE, "do_snmpv1: No context for the community string\n"));
	    if (snmp_packet_debug)
		buginf("\nSNMP: No context for community string");

	    goto cleanup;
	}
    }
    req = in_pdu_ptr->u.normpdu.request_id;
#endif				/* SR_SNMPv2 */

    switch (in_pdu_ptr->type) {
      case GET_RESPONSE_TYPE:
	snmpData.snmpInGetResponses++;
	reg_invoke_process_snmp_management_pdu(in_pdu_ptr);

#ifdef SR_SNMPv2
#endif				/* SR_SNMPv2 */

	goto cleanup;

      case TRAP_TYPE:
	snmpData.snmpInTraps++;
	reg_invoke_process_snmp_management_pdu(in_pdu_ptr);
	goto cleanup;

      case SET_REQUEST_TYPE:
	snmpData.snmpInSetRequests++;

#ifndef SR_SNMPv2
	if (contextInfo->u.Community->privs == SR_READ_WRITE) {
	    out_pdu_ptr = do_sets(in_pdu_ptr, contextInfo, (OID *) NULL,
				  SNMPv1_DOMAIN);
	}
	else {
	    snmpData.snmpInBadCommunityUses++;
	    out_pdu_ptr = make_error_pdu(GET_RESPONSE_TYPE,
					 in_pdu_ptr->u.normpdu.request_id,
					 NO_ACCESS_ERROR, 0, in_pdu_ptr,
					 SNMPv1_DOMAIN);
	}
#else				/* SR_SNMPv2 */
	if ((do_acl_check == TRUE) && ((acl_privs & SET_MASK) != SET_MASK)) {
	    snmpData.snmpInBadCommunityUses++;
	    snmpV1Data.snmpV1BadCommunityUses++;
	    if ((out_pdu_ptr = make_error_pdu(GET_RESPONSE_TYPE, req,
		  NO_ACCESS_ERROR, 0, in_pdu_ptr, SNMPv1_DOMAIN)) == NULL) {
		goto cleanup;
	    }
	    break;
	}
	if (do_acl_check == TRUE) {
	    out_pdu_ptr = do_sets(in_pdu_ptr, contextInfo,
			   contexts[context_index].context->contextIdentity,
				  SNMPv1_DOMAIN);
	}
	else {
	    out_pdu_ptr = do_sets(in_pdu_ptr, contextInfo, (OID *) NULL,
				  SNMPv1_DOMAIN);
	}
#endif				/* SR_SNMPv2 */

	if (out_pdu_ptr == NULL) {
	    out_pdu_ptr = in_pdu_ptr;
	    out_pdu_ptr->type = GET_RESPONSE_TYPE;
	    out_pdu_ptr->u.normpdu.error_status = 0;
	    out_pdu_ptr->u.normpdu.error_index = 0;
	    NormalizeVarBindListLengths(out_pdu_ptr->var_bind_list);
	    in_pdu_ptr = NULL;
	}
	break;

      case GET_REQUEST_TYPE:
	snmpData.snmpInGetRequests++;

#ifndef SR_SNMPv2
	out_pdu_ptr = do_response(in_pdu_ptr, contextInfo, limit, (OID *) NULL,
				  SNMPv1_DOMAIN);
#else				/* SR_SNMPv2 */
	if ((do_acl_check == TRUE) && ((acl_privs & GET_MASK) != GET_MASK)) {
	    snmpData.snmpInBadCommunityUses++;
	    snmpV1Data.snmpV1BadCommunityUses++;
	    if ((out_pdu_ptr = make_error_pdu(GET_RESPONSE_TYPE, req,
		  NO_ACCESS_ERROR, 0, in_pdu_ptr, SNMPv1_DOMAIN)) == NULL) {
		goto cleanup;
	    }
	    break;
	}

	if (do_acl_check == TRUE) {
	    out_pdu_ptr = do_response(in_pdu_ptr, contextInfo, limit,
			   contexts[context_index].context->contextIdentity,
				      SNMPv1_DOMAIN);
	}

	else {
	    out_pdu_ptr = do_response(in_pdu_ptr, contextInfo, limit,
				      (OID *) NULL, SNMPv1_DOMAIN);
	}
#endif				/* SR_SNMPv2 */

	if (out_pdu_ptr == NULL) {
	    DPRINTF((APTRACE, "do_snmpv1: Request failed\n"));
	    if (snmp_packet_debug)
		buginf("\nSNMP: Request failed");
	}
	break;

      case GET_NEXT_REQUEST_TYPE:
	snmpData.snmpInGetNexts++;

#ifndef SR_SNMPv2
	out_pdu_ptr = do_response(in_pdu_ptr, contextInfo, limit,
				  (OID *) NULL, SNMPv1_DOMAIN);
#else				/* SR_SNMPv2 */
	if ((do_acl_check == TRUE) && ((acl_privs & GET_NEXT_MASK) !=
				       GET_NEXT_MASK)) {
	    snmpData.snmpInBadCommunityUses++;
	    snmpV1Data.snmpV1BadCommunityUses++;
	    if ((out_pdu_ptr = make_error_pdu(GET_RESPONSE_TYPE, req,
		  NO_ACCESS_ERROR, 0, in_pdu_ptr, SNMPv1_DOMAIN)) == NULL) {
		goto cleanup;
	    }
	    break;
	}

	if (do_acl_check == TRUE) {
	    out_pdu_ptr = do_response(in_pdu_ptr, contextInfo, limit,
			   contexts[context_index].context->contextIdentity,
				      SNMPv1_DOMAIN);
	}
	else {
	    out_pdu_ptr = do_response(in_pdu_ptr, contextInfo, limit,
				      (OID *) NULL, SNMPv1_DOMAIN);
	}
#endif				/* SR_SNMPv2 */

	if (out_pdu_ptr == NULL) {
	    DPRINTF((APTRACE, "do_snmpv1: Request failed\n"));
	    if (snmp_packet_debug)
		buginf("\nSNMP: Request failed");
	}
	break;
      default:
	DPRINTF((APALWAYS, "do_snmpv1: Internal error. (Invalid packet type)\n"));
	goto cleanup;
    }				/* switch */


cleanup:
    if (in_pdu_ptr != NULL) {
	FreePdu(in_pdu_ptr);
	in_pdu_ptr = NULL;
    }

    return (out_pdu_ptr);
}
