/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_mgmt.c,v 3.3.28.3 1996/05/24 00:38:09 jjohnson Exp $
 * $Source: /release/112/cvs/Xsys/snmp/sr_mgmt.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, Jeffrey T. Johnson
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_mgmt.c,v $
 * Revision 3.3.28.3  1996/05/24  00:38:09  jjohnson
 * CSCdi58000:  IbuMod feature commit messes with core snmp engine
 * Back out the IbuMod changes to the core snmp engine until we
 * figure out what the right thing to do is
 *
 * Revision 3.3.28.2  1996/05/17  11:42:59  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.14.2  1996/04/03  21:26:47  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.14.1  1996/03/26  00:22:54  bchan
 * CSCdi52555:  Allow cip index to be passed in the community string
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.28.1  1996/03/18  21:48:38  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.1  1996/03/07  12:33:41  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3  1996/01/20  02:36:06  jjohnson
 * CSCdi47220:  need snmp packet debugging
 *
 * Revision 3.2  1995/11/17  18:59:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:17:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:49:32  hampton
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
static char     rcsid[] = "mgmt.c,v 1.24 1995/02/06 18:11:36 battle Exp";
#endif				/* (! ( defined(lint) ) && defined(SR_RCSID)) */

/*
 * Revision History:
 *
 * mgmt.c,v
 * Revision 1.24  1995/02/06  18:11:36  battle
 * Changes for vrtx.
 *
 * Revision 1.23  1994/11/02  19:40:12  levi
 * Fixed memset of contextInfo
 *
 * Revision 1.22  1994/11/02  17:09:43  levi
 * Fixed lint nits
 *
 * Revision 1.21  1994/11/01  18:00:53  vaughn
 * Added a memset to initialize a ContextInfo struct to 0 in findContextInfo
 *
 * Revision 1.20  1994/10/28  17:32:13  vaughn
 * Placed new copyright in the file
 *
 * Revision 1.19  1994/10/26  21:58:36  vaughn
 * Changed copyrights to the new copyright structure
 *
 * Revision 1.18  1994/10/19  14:19:26  battle
 * removed CloneOID from party party context in ContextInfo
 *
 * Revision 1.17  1994/09/20  20:27:37  hecht
 * Added a #ifdef SR_SNMPv2 block around the partyTAddressToUnsignedLong()
 * routine since it is not used on a SNMPv1 only agent.
 *
 * Revision 1.16  1994/09/01  20:28:25  hecht
 * Added a new routine called partyTAddressToUnsignedLong(). Added code to
 * findContextInfo() to authenticate the source address of SNMPv1 packets on
 * a bilingual agent. This code enables the SNMPv1 authentication portion of a
 * bilingual agent to perform like a SNMPv1 agent.
 *
 * Revision 1.15  1994/08/29  20:32:18  battle
 * modified contextInfo
 *
 * Revision 1.14  1994/08/04  15:37:37  battle
 * pSOS 2.0 changes
 *
 * Revision 1.13  1994/06/13  17:24:16  hecht
 * Fixed a problem in findContextInfo() for SNMPv1 authentication checks on
 * a bilingual agent. The auth_ptr->srcPartyIndex field is not filled in for
 * incoming SNMPv1 packets, so I delayed the calculation of "limit" for
 * SNMPv1 packets on a bilingual agent until the routine has found the entry for
 * the SNMPv1 community string in the party table.
 *
 * Revision 1.12  1994/03/03  15:42:00  vaughn
 * Changed 0's in DPRINTF to APALWAYS.
 *
 * Revision 1.11  1994/02/14  16:42:16  pearson
 * Updated copyright notices for 1994.
 *
 * AMP
 *
 * Revision 1.10  1994/02/14  15:25:49  levi
 * Added code to increment error_index when it is 0 on various errors.
 *
 * Revision 1.9  1994/02/09  20:40:44  hecht
 * Changed the calling interface to do_trap() by adding a new parameter,
 * entV2Trap, to the end of the parameter list. All calls to do_trap() in this
 * file are standard traps, so this parameter has a value of (char *) NULL.
 *
 * Revision 1.8  1994/02/09  20:34:55  pearson
 * Got rid of a bunch of dos linefeeds that crept into this file somehow.  Also
 * cleaned up the
 * 	#ifdef lint
 * so that it now reads
 * 	#if !defined(lint) && !defined(SR_RCSID)
 *
 * AMP
 *
 * Revision 1.7  1994/02/08  18:43:59  reid
 * added support for WinSock
 *
 * Revision 1.6  1994/02/04  19:57:42  hecht
 * Calls of do_snmpv1() and do_snmpv2() in do_mgmt() now require the
 * additional parameter, outLen. Changed the calling interface to
 * findContextInfo() by adding the parameter, outLen. The outLen variable
 * in findContextInfo() replaces the use of PKTBUFSIZ in the MIN computation of
 * the variable, limit.
 *
 * Revision 1.5  1994/01/31  15:24:08  hecht
 * Changed the calls to strncmp() in findContextInfo() to memcmp() calls. This
 * was done because the community strings may have null characters in them.
 * The strncmp() function will not work properly on a string that has null
 * characters in it.
 *
 * Revision 1.4  1994/01/12  21:44:05  key
 * Added call to NormalizeVarBindListLengths() to fix up VarBind lists
 * that where taken literally from the incoming PDU to the outgoing PDU.
 * This affected sets processing and make_error_pdu().
 *
 * Revision 1.3  1993/12/23  18:38:39  battle
 * Added cvs headers to all .c and .h files.
 *
 *
 *  01-Jul-92 JDC
 *  Split out from snmp.c to support SMP
 *
 *  27-Aug-92 MGH
 *  Modified findAsod() to scan for community strings in the party table
 *  when SMP is defined.
 *
 *  18-Sep-92 KWK
 *  Added return((void *) NULL); to NOAUTH_AUTHEN party-check case.
 *
 * 9-Dec-1992 DLB
 * Lint nits.
 *
 * 10-Dec-1992 KWK
 * PKG doesn't have an netinet/in.h
 *
 * 19-Jan-1993 MGH
 * Removed some deadwood code from findAsod() (dealing with community strings).
 * Removed code in make_error_pdu() that was incrementing non-existant
 * counters (smpInOut table).
 *
 * 8-Feb-1993 MGH
 * Added a parameter to the call of ParseAuthentication().  If this is a
 * SNMPv2 agent and ParseAuthentication() returns NULL, the error_code
 * parameter is checked to see if any snmpStats counters need incrementing.
 *
 * 12-Feb-1993 MGH
 * Moved packaging of get response up to this layer from do_snmpv1() and
 * do_snmpv2().
 *
 * 13-Feb-1993 KWK
 * Converted community and trap community ip_addr back to Network Byte Order.
 *
 * 13-Feb-1993 KWK
 * Changed build_snmpv[12]_response declaration to unsigned short to match
 * snmpd.h
 *
 * 10-May-93 MGH
 * Changed all references to EXPORT to match the new compliance levels in
 * RFC 1447.
 *
 * 11-May-93 MGH
 * Changed all references to inPdu to inPkt since the parameter is still
 * a packet and not a pdu.
 *
 * 18-May-93 MGH
 * Changed #include <diag.h> to #include "diag.h"
 *
 * 8-June-93 MGH
 * Change do_mgmt() so snmpInASNParseErrs is not incremented for every
 * possible error_code returned from ParseAuthentication() if this is a
 * bilingual (SNMPv1 and SNMPv2) or a SNMPv2 agent.  It's still incremented
 * every time ParseAuthentication() returns NULL in a SNMPv1 only agent.
 *
 * 10-Jun-1993 DLB
 * Changed all short parameters to int.  Short parameters are a bad idea
 * for numerous reasons I won't go into here.
 *
 * 24-June-1993 MGH
 * Moved call to CheckDigest() from findAsod() to ParseAuthentication()
 * (auth_lib.c).  Added case statements for four more error codes to switch
 * statement after ParseAuthentication() call. Made the comparison of
 * partyTDomain and RFC_1157_DOMAIN the first if statement when checking
 * to see whether a community string is in the party table.  It used to
 * be the last comparison made for a match. This should make the loop
 * more efficient.
 *
 * 28-June-1993 MGH
 * Changed the name of findAsod() to findContextInfo().  An almost total
 * rewrite.
 *
 * 20-July-1993 MGH
 * Changed the mapping of NO_CREATION_ERROR from BAD_VALUE_ERROR to
 * NO_SUCH_NAME_ERROR for a SNMPv1 packet.
 *
 * 26-July-1993 MGH
 * Changed build_snmpv2_response() to use the authPrivate and privPrivate
 * keys from the incoming AuthHeader structure (in_auth_ptr) instead of
 * looking them up in the database (see ParseAuthentication() in auth_lib.c).
 * It also uses the src_timestamp and dst_timestamp fields from the incoming
 * AuthHeader structure (see CheckDigest() in auth_lib.c). Also uses the
 * authType and privType fields from the incoming AuthHeader structure.
 */

#define	WANT_ALL_ERRORS

#include "master.h"
#include <ciscolib.h>


#include "sr_diag.h"
#include "sr_snmpd.h"


#include "snmp_debug.h"
#include "snmp_registry.h"
#include "address.h"
#include "sr_sysmib2.h"
#include "sr_snmpmib2.h"
#include "sr_snmpv2mib.h"

/*------------------------------------------------------------
 * Process one packet.
 *------------------------------------------------------------*/
unsigned short
do_mgmt(inPkt, inLen, srcIpAddr, srcUdpPort, dstIpAddr, dstUdpPort,
	outPkt, outLen)
    unsigned char  *inPkt;
    unsigned int    inLen;
    unsigned long   srcIpAddr;
    unsigned int    srcUdpPort;
    unsigned long   dstIpAddr;
    unsigned int    dstUdpPort;
    unsigned char  *outPkt;
    unsigned int    outLen;
{
    AuthHeader     *in_auth_ptr;
    Pdu            *result_pdu;
    unsigned short  result_len = 0;
    int             error_code;

    in_auth_ptr = NULL;
    result_pdu = NULL;
    snmpData.snmpInPkts++;

#ifdef SR_SNMPv2
/*****************************************************************************/
/* from RFC1445, page 18, Processing a Received Communication;               */
/*          (1)  The snmpStatsPackets counter [7] is incremented.            */
/*****************************************************************************/


    snmpStatsData.snmpStatsPackets++;
#endif				/* SR_SNMPv2 */

    if ((in_auth_ptr = ParseAuthentication(inPkt, (long) inLen,
					   &error_code)) == NULL) {
/*****************************************************************************/
/* from RFC1157:                                                             */
/*      (1)  It performs a rudimentary parse of the incoming datagram        */
/*           to build an ASN.1 object corresponding to an ASN.1              */
/*           Message object. If the parse fails, it discards the             */
/*           datagram and performs no further actions.                       */
/*                                                                           */
/* from SNMPv2 Admin document:                                               */
/*       1. If the received message is not the serialization (according      */
/*          to the conventions of [5]) of an SnmpPrivMsg value, then that    */
/*          message is discarded without further processing.                 */
/*****************************************************************************/

#ifndef SR_SNMPv2
	snmpData.snmpInASNParseErrs++;
#endif				/* SR_SNMPv2 */

	switch (error_code) {

#ifdef SR_SNMPv2
	  case THIRTY_SOMETHING_ERROR:
	    snmpData.snmpInASNParseErrs++;
	    snmpStatsData.snmpStats30Something++;
	    break;
	  case ENCODING_ERROR:
	    snmpData.snmpInASNParseErrs++;
	    snmpStatsData.snmpStatsEncodingErrors++;
	    break;
	  case DST_PARTY_ERROR:
	    snmpStatsData.snmpStatsUnknownDstParties++;
	    break;
	  case DST_PARTY_MISMATCH_ERROR:
	    snmpStatsData.snmpStatsDstPartyMismatches++;
	    break;
	  case SRC_PARTY_ERROR:
	    snmpStatsData.snmpStatsUnknownSrcParties++;
	    break;
	  case BAD_AUTHS_ERROR:
	    snmpData.snmpInASNParseErrs++;
	    snmpStatsData.snmpStatsBadAuths++;
	    break;
	  case NOT_IN_LIFETIME_ERROR:
	    snmpStatsData.snmpStatsNotInLifetimes++;
	    DPRINTF((APWARN, "do_mgmt: authentication failure.\n"));
	    snmp_authentication_trap(SNMPv2_DOMAIN, (addrtype *)srcIpAddr);
	    break;
	  case WRONG_DIGEST_VALUES_ERROR:
	    snmpStatsData.snmpStatsWrongDigestValues++;
	    DPRINTF((APWARN, "do_mgmt: authentication failure.\n"));
	    snmp_authentication_trap(SNMPv2_DOMAIN, (addrtype *)srcIpAddr);
	    break;
	  case UNKNOWN_CONTEXT_ERROR:
	    snmpStatsData.snmpStatsUnknownContexts++;
	    break;
#endif				/* SR_SNMPv2 */

#ifdef SR_SNMPv1
	  case SNMP_BAD_VERSION_ERROR:
	    snmpData.snmpInBadVersions++;
	    break;
#endif				/* SR_SNMPv1 */

	  default:
	    break;
	}
	if (snmp_packet_debug)
	    buginf("\nSNMP: Error parsing packet");
	DPRINTF((APTRACE, "do_mgmt: error parsing packet -- dropping packet\n"));
	goto cleanup;
    }

#ifndef	SR_SNMPv2
    result_pdu = do_snmpv1(inPkt, inLen, srcIpAddr, srcUdpPort, in_auth_ptr,
			   outLen);
#else				/* SR_SNMPv2 */

    switch (in_auth_ptr->authType) {

#ifdef	SR_SNMPv1
      case TRIVIAL_AUTHENTICATION:
	result_pdu = do_snmpv1(inPkt, inLen, srcIpAddr, srcUdpPort, in_auth_ptr,
			       outLen);
	break;
#endif				/* SR_SNMPv1 */

      case NOAUTH_AUTHENTICATION:

	if (in_auth_ptr->domain == SNMPv2_DOMAIN) {
	    result_pdu = do_snmpv2(inPkt, inLen, srcIpAddr, srcUdpPort,
				dstIpAddr, dstUdpPort, in_auth_ptr, outLen);
	}
	else {
	    DPRINTF((APWARN, "do_mgmt: unknown transport.\n"));
	    goto cleanup;
	}
	break;

#ifndef SR_UNSECURABLE
      case MD5_SNMPv2_AUTHENTICATION:
	result_pdu = do_snmpv2(inPkt, inLen, srcIpAddr, srcUdpPort,
			       dstIpAddr, dstUdpPort, in_auth_ptr, outLen);
	break;
#endif				/* SR_UNSECURABLE */

      default:
	DPRINTF((APWARN, "do_mgmt:  unknown authtype: %d\n",
		 in_auth_ptr->authType));
	DPRINTF((APWARN, "dropping packet\n"));
	goto cleanup;
	break;
    }				/* switch authType */
#endif				/* SR_SNMPv2 */

    if (result_pdu != (Pdu *) NULL) {
	if (snmp_packet_debug)
	    snmp_print_message(result_pdu);

#ifndef SR_SNMPv2
	result_len = build_snmpv1_response(in_auth_ptr, result_pdu,
					   outPkt);
#else				/* SR_SNMPv2 */

#ifdef SR_SNMPv1
	if (in_auth_ptr->authType == TRIVIAL_AUTHENTICATION) {
	    result_len = build_snmpv1_response(in_auth_ptr, result_pdu,
					       outPkt);
	}
	else {
	    result_len = build_snmpv2_response(in_auth_ptr, result_pdu,
					       outPkt);
	}
#else				/* !SR_SNMPv1 */
	result_len = build_snmpv2_response(in_auth_ptr, result_pdu,
					   outPkt);
#endif				/* SR_SNMPv1 */

#endif				/* SR_SNMPv2 */
    }				/* if (result_pdu != ...) */

cleanup:
    if (in_auth_ptr != NULL) {
	FreeAuthentication(in_auth_ptr);
	in_auth_ptr = NULL;
    }
    if (result_pdu != NULL) {
	FreePdu(result_pdu);
	result_pdu = NULL;
    }

    return (result_len);	/* signal results to layer above */
}				/* do_mgmt() */

#ifdef SR_SNMPv1
unsigned short
build_snmpv1_response(in_auth_ptr, out_pdu_ptr, outPkt)
    AuthHeader     *in_auth_ptr;
    Pdu            *out_pdu_ptr;
    unsigned char   outPkt[];
{
    unsigned short  outPkt_len = 0;
    AuthHeader     *out_auth_ptr;
    OctetString    *community_ptr;
    SR_FNAME("build_snmpv1_response")

    out_auth_ptr = (AuthHeader *) NULL;
    community_ptr = (OctetString *) NULL;

    if (BuildPdu(out_pdu_ptr) == -1) {
	DPRINTF((APTRACE, "%s: Failed to build PDU.\n", FName));
	goto done;
    }


    if ((community_ptr = MakeOctetString(in_auth_ptr->community->octet_ptr,
				 in_auth_ptr->community->length)) == NULL) {
	DPRINTF((APALWAYS, "%s: Failed to make octetstring.\n", FName));
	goto done;
    }


#ifndef SR_SNMPv2
    if ((out_auth_ptr = MakeAuthentication(community_ptr)) == NULL) {

	DPRINTF((APALWAYS, "%s: Failed to make authentication.\n", FName));
	goto done;
    }
#else				/* SR_SNMPv2 */
    if ((out_auth_ptr = MakeAuthentication(
					   community_ptr,
					   (OID *) NULL,
					   (OID *) NULL,
					   (OID *) NULL,
					   (OID *) NULL,
					   in_auth_ptr->authType,
					   (OctetString *) NULL,
					   (unsigned long) 0,
					   (unsigned long) 0,
					   (int) 0,
					   (OctetString *) NULL)
	 ) == NULL) {
	DPRINTF((APALWAYS, "%s: Failed to make authentication.\n", FName));
	goto done;
    }
#endif				/* SR_SNMPv2 */

    community_ptr = (OctetString *) NULL;

    if (BuildAuthentication(out_auth_ptr, out_pdu_ptr) == -1) {
	DPRINTF((APALWAYS, "%s: Failed to build authentication.\n", FName));
	goto done;
    }

    outPkt_len = (unsigned short) out_auth_ptr->packlet->length;

    bcopy((char *) out_auth_ptr->packlet->octet_ptr, (char *) outPkt,
	  (int) outPkt_len);

    snmpData.snmpOutGetResponses++;

done:
    if (out_auth_ptr != NULL) {
	FreeAuthentication(out_auth_ptr);
	out_auth_ptr = NULL;
    }
    /* community ptr must come after auth_ptr */
    if (community_ptr != NULL) {
	FreeOctetString(community_ptr);
	community_ptr = NULL;
    }

    return (outPkt_len);
}				/* build_snmpv1_response() */

#endif				/* SR_SNMPv1 */


#ifdef SR_SNMPv2
unsigned short
build_snmpv2_response(in_auth_ptr, out_pdu_ptr, outMsg)
    AuthHeader     *in_auth_ptr;
    Pdu            *out_pdu_ptr;
    unsigned char   outMsg[];
{

    unsigned short  outMsg_len;
    AuthHeader     *out_auth_ptr;
    int             privType;
    static OID     *privDst;
    static OID     *dstParty;
    static OID     *srcParty;
    static OID     *context;
    static OctetString *authPrivate;
    static OctetString *privPrivate;
    int             authType;
    unsigned long   src_timestamp;
    unsigned long   dst_timestamp;
    SR_FNAME("build_snmpv2_response")

    outMsg_len = 0;
    out_auth_ptr = (AuthHeader *) NULL;
    privDst = (OID *) NULL;
    dstParty = (OID *) NULL;
    srcParty = (OID *) NULL;
    context = (OID *) NULL;
    authPrivate = (OctetString *) NULL;
    privPrivate = (OctetString *) NULL;

/*****************************************************************************/
/* From RFC1445, pages 21-22, section 3.3:                                   */
/*                                                                           */
/*     The procedure for generating a response to a SNMPv2 management        */
/*     request is identical to the procedure for transmitting a              */
/*     request (see Section 3.1), with these exceptions:                     */
/*                                                                           */
/*    (1)  In Step 1, the dstParty component of the responding               */
/*         SnmpMgmtCom value is taken from the srcParty component of         */
/*         the original SnmpMgmtCom value; the srcParty component of         */
/*         the responding SnmpMgmtCom value is taken from the                */
/*         dstParty component of the original SnmpMgmtCom value; the         */
/*         context component of the responding SnmpMgmtCom value is          */
/*         taken from the context component of the original                  */
/*         SnmpMgmtCom value; and, the pdu component of the                  */
/*         responding SnmpMgmtCom value is the response which                */
/*         results from applying the operation specified in the              */
/*         original SnmpMgmtCom value.                                       */
/*                                                                           */
/*    (2)  In Step 7, the serialized SnmpPrivMsg value is                    */
/*         transmitted using the transport address and transport             */
/*         domain from which its corresponding request originated -          */
/*         even if that is different from the transport information          */
/*         recorded in the local database of party information.              */
/*                                                                           */
/*****************************************************************************/

    if (BuildPdu(out_pdu_ptr) == -1) {
	DPRINTF((APTRACE, "%s: Failed to build PDU.\n", FName));
	goto finished;
    }

    switch (in_auth_ptr->authType) {
/*****************************************************************************/
/* From RFC1445, pages 17-18, section 3.1                                    */
/*                                                                           */
/*     (1)  A SnmpMgmtCom value is constructed for which the srcParty        */
/*          component identifies the originating party, for which the        */
/*          dstParty component identifies the receiving party, for           */
/*          which the context component identifies the desired SNMPv2        */
/*          context, and for which the pdu component represents the          */
/*          desired management operation.                                    */
/*                                                                           */
/*     (2)  The local database of party information is consulted to          */
/*          determine the authentication protocol and other relevant         */
/*          information for the originating and receiving SNMPv2             */
/*          parties.                                                         */
/*                                                                           */
/*     (3)  A SnmpAuthMsg value is constructed with the following            */
/*          properties:                                                      */
/*                                                                           */
/*               Its authInfo component is constructed according to          */
/*               the authentication protocol specified for the               */
/*               originating party.                                          */
/*                                                                           */
/*               In particular, if the authentication protocol for           */
/*               the originating SNMPv2 party is identified as               */
/*               noAuth, then this component corresponds to the              */
/*               OCTET STRING value of zero length.                          */
/*                                                                           */
/*               Its authData component is the constructed SnmpMgmtCom       */
/*               value.                                                      */
/*                                                                           */
/*     (4)  The local database of party information is consulted to          */
/*          determine the privacy protocol and other relevant                */
/*          information for the receiving SNMPv2 party.                      */
/*                                                                           */
/*     (5)  A SnmpPrivMsg value is constructed with the following            */
/*          properties:                                                      */
/*                                                                           */
/*               Its privDst component identifies the receiving              */
/*               SNMPv2 party.                                               */
/*                                                                           */
/*               Its privData component is the (possibly encrypted)          */
/*               serialization of the SnmpAuthMsg value according to         */
/*               the conventions of [5].                                     */
/*                                                                           */
/*                 In particular, if the privacy protocol for the            */
/*                 receiving SNMPv2 party is identified as noPriv,           */
/*                 then the privData component is unencrypted.               */
/*                 Otherwise, the privData component is processed            */
/*                 according to the privacy protocol.                        */
/*                                                                           */
/*****************************************************************************/

      case NOAUTH_AUTHENTICATION:

#ifndef SR_UNSECURABLE
      case MD5_SNMPv2_AUTHENTICATION:
#endif				/* SR_UNSECURABLE */

	/* source and dest are reversed for the reply */
	if ((dstParty = CloneOID(in_auth_ptr->srcParty)) == NULL) {
	    DPRINTF((APTRACE, "%s: clone dstParty OID failure\n", FName));
	    goto finished;
	}

	if ((privDst = CloneOID(in_auth_ptr->srcParty)) == NULL) {
	    DPRINTF((APTRACE, "%s: clone privDst OID failure\n", FName));
	    goto finished;
	}

	if ((srcParty = CloneOID(in_auth_ptr->dstParty)) == NULL) {
	    DPRINTF((APTRACE, "%s: clone dstParty OID failure\n", FName));
	    goto finished;
	}
	if ((context = CloneOID(in_auth_ptr->context)) == (OID *) NULL) {
	    DPRINTF((APTRACE, "%s: make context OID failure\n", FName));
	    goto finished;
	}

	authType = in_auth_ptr->authType;

#ifndef SR_UNSECURABLE
	if (authType == MD5_SNMPv2_AUTHENTICATION) {
	    authPrivate =
		MakeOctetString(in_auth_ptr->authPrivate->octet_ptr,
				in_auth_ptr->authPrivate->length);
	    if (authPrivate == NULL) {
		DPRINTF((APTRACE, "%s: MakeOctetString -- authPrivate\n", FName));
		goto finished;
	    }
	}
	else {
	    authPrivate = NULL;
	}
#else				/* SR_UNSECURABLE */
	authPrivate = NULL;
#endif				/* SR_UNSECURABLE */

	/*
	 * The delta values for the two clocks have been calculated in a call
	 * to CheckDigest() (from ParseAuthentication). Using these
	 * calculated values will keep the agent from failing if a user
	 * deletes the parties used for the request in the request.  The same
	 * goes for the authPrivate, privPrivate, and privType fields.
	 */

	/* Check for clock overflow */
	src_timestamp = GetTimeNow() / 100;
	dst_timestamp = GetTimeNow() / 100;

	if (basetime > (MAX_CLOCK_VALUE - src_timestamp)) {
	    src_timestamp = MAX_CLOCK_VALUE;
	    dst_timestamp = MAX_CLOCK_VALUE;
	}
	else {
	    src_timestamp = src_timestamp + basetime;
	    dst_timestamp = dst_timestamp + basetime;
	    if (in_auth_ptr->src_timestamp > (MAX_CLOCK_VALUE - src_timestamp)) {
		src_timestamp = MAX_CLOCK_VALUE;
	    }
	    else {
		src_timestamp = src_timestamp + in_auth_ptr->src_timestamp;
	    }
	    if (in_auth_ptr->dst_timestamp > (MAX_CLOCK_VALUE - dst_timestamp)) {
		dst_timestamp = MAX_CLOCK_VALUE;
	    }
	    else {
		dst_timestamp = dst_timestamp + in_auth_ptr->dst_timestamp;
	    }
	}

	privType = in_auth_ptr->privType;

#ifndef SR_UNSECURABLE

#ifndef SR_NO_PRIVACY
	if (privType == DESPRIV) {
	    privPrivate =
		MakeOctetString(in_auth_ptr->privPrivate->octet_ptr,
				in_auth_ptr->privPrivate->length);
	    if (privPrivate == NULL) {
		DPRINTF((APTRACE, "%s: MakeOctetString -- privPrivate\n", FName));
		goto finished;
	    }
	}

	else {
	    privPrivate = NULL;
	}
#else				/* SR_NO_PRIVACY */
	privPrivate = NULL;
#endif				/* SR_NO_PRIVACY */

#else				/* SR_UNSECURABLE */
	privPrivate = NULL;
#endif				/* SR_UNSECURABLE */

	if ((out_auth_ptr = MakeAuthentication(
					       (OctetString *) NULL,
					       privDst,
					       dstParty,
					       srcParty,
					       context,
					       authType,
					       authPrivate,
					       src_timestamp,
					       dst_timestamp,
					       privType,
					       privPrivate)) == NULL) {

	    DPRINTF((APTRACE, "%s: MakeAuthentication\n", FName));
	    goto finished;
	}

	privDst = (OID *) NULL;
	dstParty = (OID *) NULL;
	srcParty = (OID *) NULL;
	context = (OID *) NULL;
	authPrivate = (OctetString *) NULL;
	privPrivate = (OctetString *) NULL;
	break;

      default:
	DPRINTF((APTRACE, "%s: unknown authentication type: %x\n", FName, in_auth_ptr->authType));
	goto finished;
	break;
    }

/*****************************************************************************/
/* From RFC1445, page 18, section 3.1                                        */
/*                                                                           */
/*     (6) The constructed SnmpPrivMsg value is serialized                   */
/*         according to the conventions of [5].                              */
/*****************************************************************************/

    if (BuildAuthentication(out_auth_ptr, out_pdu_ptr) == -1) {
	DPRINTF((APALWAYS, "%s: Failed to build authentication.\n", FName));
	goto finished;
    }

/*****************************************************************************/
/* From RFC1445, page 18, section 3.1                                        */
/*                                                                           */
/*     (7) The serialized SnmpPrivMsg value is transmitted                   */
/*         using the transport address and transport domain for              */
/*         the receiving SNMPv2 party.                                       */
/*                                                                           */
/*         In this case, in step 7, the serialized SnmpPrivMsg value is      */
/*         transmitted using the transport address and transport             */
/*         domain from which its corresponding request originated -          */
/*         even if that is different from the transport information          */
/*         recorded in the local database of party information.              */
/*                                                                           */
/* Actual transmission of the message is handled by the layer above.         */
/*****************************************************************************/
    outMsg_len = (unsigned short) out_auth_ptr->packlet->length;

    bcopy((char *) out_auth_ptr->packlet->octet_ptr, (char *) outMsg,
	  (int) outMsg_len);

finished:

    if (out_auth_ptr != NULL) {
	FreeAuthentication(out_auth_ptr);
	out_auth_ptr = NULL;
    }

    if (privDst != NULL) {
	FreeOID(privDst);
	privDst = NULL;
    }

    if (dstParty != NULL) {
	FreeOID(dstParty);
	dstParty = NULL;
    }

    if (srcParty != NULL) {
	FreeOID(srcParty);
	srcParty = NULL;
    }

    if (context != NULL) {
	FreeOID(context);
	context = NULL;
    }

    if (authPrivate != NULL) {
	FreeOctetString(authPrivate);
	authPrivate = NULL;
    }

    if (privPrivate != NULL) {
	FreeOctetString(privPrivate);
	privPrivate = NULL;
    }

    return (outMsg_len);
}				/* build_snmpv2_response() */

#endif				/* SR_SNMPv2 */

/* findContextInfo is needed for TRIVIAL,  and / or SNMPv2 */

/*------------------------------------------------------------
 * Search for the authorization structure associated with
 * this community (or party) name.
 *
 * error_code is set to -1 if authentication failure traps
 * should be generated.
 *------------------------------------------------------------*/
ContextInfo    *
findContextInfo(from_addr, auth_ptr, limit, error_code, outLen)
    unsigned long   from_addr;
    AuthHeader     *auth_ptr;
    short          *limit;
    int            *error_code;
    unsigned int    outLen;
{
    int             i;
    static ContextInfo contextInfo;
    SR_FNAME("findContextInfo")
    addrtype *src_addr;

#define MATCH_IP_ADDR(addr,src_addr) (((addr)==(src_addr)) || ((addr)==0L))

    memset(&contextInfo, 0, sizeof(ContextInfo));

#ifndef	SR_SNMPv2
    *limit = MIN(outLen, 484);
#else				/* SR_SNMPv2 */

#ifndef SR_SNMPv1
    *limit = MIN(outLen, parties[auth_ptr->srcPartyIndex].party->partyMaxMessageSize);
#else				/* SR_SNMPv1 */
    /*
     * Do not compute value for limit here for incoming SNMPv1 packets on a
     * bilingual agent since the srcPartyIndex field in the auth_ptr
     * structure is not filled in for SNMPv1 packets.
     */
    if (auth_ptr->authType != TRIVIAL_AUTHENTICATION) {
	*limit = MIN(outLen, parties[auth_ptr->srcPartyIndex].party->partyMaxMessageSize);
    }
#endif				/* SR_SNMPv1 */

#endif				/* SR_SNMPv2 */

#ifndef SR_SNMPv2
    *limit -= (auth_ptr->wrapper_size + 23);
#else				/* SR_SNMPv2 */

#ifndef SR_SNMPv1
    *limit -= (auth_ptr->wrapper_size + 23);
#else				/* SR_SNMPv1 */
    /*
     * Do not compute value for limit here for incoming SNMPv1 packets on a
     * bilingual agent since the srcPartyIndex field in the auth_ptr
     * structure is not filled in for SNMPv1 packets.
     */
    if (auth_ptr->authType != TRIVIAL_AUTHENTICATION) {
	*limit -= (auth_ptr->wrapper_size + 23);
    }
#endif				/* SR_SNMPv1 */

#endif				/* SR_SNMPv2 */

/* 23 =   4 (worst case pdu type/length)
	+ 6 (worst case request id)
	+ 3 (worst case error status)
	+ 6 (worst case error index)
	+ 4 (worst case varbindlist wrapper)

    note this is an overestimate for the limit on a tooBig because we know
    that it really should be

   16 =   2 (worst case pdu type/length)
        + 6 (worst case request id)
        + 3 (worst case error status)
        + 3 (worst case error index)
        + 2 (worst case varbindlist wrapper)

    so we will have a few more silent drops -- but this is so pathological
    because of the 484 rule that we will not invest any more resources to
    make it perfect
*/

    *error_code = 0;

#ifndef SR_SNMPv2
    for (i = 0; i < num_communities; i++) {
	register int    len = strlen(communities[i].community_name);

	if ((auth_ptr->community->length == len) &&
	    MATCH_IP_ADDR(communities[i].ip_addr, from_addr) &&
	    (memcmp((char *) auth_ptr->community->octet_ptr,
		    communities[i].community_name, len) == 0)) {
	    contextInfo.CIType = CI_COMMUNITY;
	    contextInfo.u.Community = &(communities[i]);
	    return (&(contextInfo));
	}
    }
    *error_code = -1;
    DPRINTF((APTRACE, "%s: No matching community in table.\n", FName));
    return ((ContextInfo *) NULL);
#else				/* SR_SNMPv2 */
    switch (auth_ptr->authType) {

#ifdef SR_SNMPv1
      case TRIVIAL_AUTHENTICATION:

	/*
	 * the from_addr parameter is not really an IP address, but is
	 * really an addrtype * that was cast into an unsigned long
	 * to avoid having to change a bunch of SNMP Research code.
	 * This is the only place the from_addr is used, so now is
	 * the time to resurrect the pointer
	 */
	src_addr = (addrtype *) from_addr;

	/*
	 * If the community string appears in the party table, and the
	 * partyTDomain is equal to rfc1157Domain (trivial), the party table
	 * entry should be returned in the contextInfo structure so the
	 * related acl and view privileges can be used.  This integrates
	 * parties and community strings into one table for a bilingual
	 * agent.
	 */
	for (i = 0; i < num_parties; i++) {
	    if (parties[i].party == NULL) {
		continue;
	    }
	    if (parties[i].party->partyTDomain == RFC_1157_DOMAIN) {
		if (parties[i].party->partyAuthPrivate != NULL) {
		    if (auth_ptr->community->length ==
			parties[i].party->partyAuthPrivate->length) {
			if (memcmp((char *) auth_ptr->community->octet_ptr,
				   (char *) parties[i].party->partyAuthPrivate->octet_ptr,
				   (int) auth_ptr->community->length) == 0) {
			    if (reg_invoke_snmp_access_check(src_addr->type,
							     parties[i].party->access_list,
							     src_addr)) {

				/*
				 * cisco hack to skip traphost parties
				 * see sr_config.c: traphost_partyname() for
				 * the significance of "*tp."
				 */
				if ((strlen(parties[i].party->partyHandle) >= 4) &&
				    (memcmp(parties[i].party->partyHandle, "*tp.", 4) == 0))
				    continue;

				/*
				 * Compute value for limit here for incoming
				 * SNMPv1 packets on a bilingual agent now
				 * that we know the index into the party
				 * table for this community string.
				 */
				*limit = MIN(outLen,
				     parties[i].party->partyMaxMessageSize);
				*limit -= (auth_ptr->wrapper_size + 23);
				contextInfo.CIType = CI_PARTY_COMMUNITY;
				contextInfo.u.PartyCommunity = parties[i].party;
				return (&(contextInfo));
			    }
			    else {	/* refuse packet */
				DPRINTF((APTRACE, "%s: Manager station not authorized to use this community string.\n", FName));
				*error_code = -1;
				return ((ContextInfo *) NULL);
			    }
			}
		    }
		}
	    }
	}

	DPRINTF((APTRACE, "%s: No matching community in party table.\n", FName));
	*error_code = -1;
	return ((ContextInfo *) NULL);
	break;
#endif				/* SR_SNMPv1 */

	/*
	 * Sanity check the party table information.  The incoming auth type
	 * should match the auth type of the source party.
	 */
      case NOAUTH_AUTHENTICATION:
	if (parties[auth_ptr->srcPartyIndex].party->partyAuthProtocol !=
	    NOAUTH_AUTHENTICATION) {
	    DPRINTF((APWARN, "%s: Incoming pkt is noauth/nopriv, but srcParty \
is: 0x%x\n", FName, parties[auth_ptr->srcPartyIndex].party->partyAuthProtocol));
	    *error_code = -1;
	    return ((ContextInfo *) NULL);
	}
	break;

#ifndef SR_UNSECURABLE
      case MD5_SNMPv2_AUTHENTICATION:
	if (parties[auth_ptr->srcPartyIndex].party->partyAuthProtocol !=
	    MD5_SNMPv2_AUTHENTICATION) {
	    DPRINTF((APWARN, "%s: Incoming pkt is md5auth, but srcParty \
is: 0x%x\n", FName, parties[auth_ptr->srcPartyIndex].party->partyAuthProtocol));
	    *error_code = -1;
	    return ((ContextInfo *) NULL);
	}
#endif				/* SR_UNSECURABLE */

	break;

      default:
	DPRINTF((APTRACE, "%s: unknown authtype in process authentication (%d) (0x%x)\n",
		 FName, auth_ptr->authType, auth_ptr->authType));
	return ((ContextInfo *) NULL);
    }				/* end switch */

    /*
     * Sanity check the context table information.  The matching entry must
     * be local and non-proxy or proxy.
     */
    if ((contexts[auth_ptr->contextIndex].context->contextLocal == SR_LOCAL) &&
	(contexts[auth_ptr->contextIndex].context->contextViewIndex > 0)) {
	contextInfo.CIType = CI_LOCAL;
	contextInfo.u.local.Time =
	    contexts[auth_ptr->contextIndex].context->contextLocalTime;
	contextInfo.u.local.Entity =
	    contexts[auth_ptr->contextIndex].context->contextLocalEntity;
	contextInfo.u.local.srcParty =
	    parties[auth_ptr->srcPartyIndex].party->partyIdentity;
	contextInfo.u.local.dstParty =
	    parties[auth_ptr->dstPartyIndex].party->partyIdentity;
	contextInfo.u.local.context =
	    contexts[auth_ptr->contextIndex].context->contextIdentity;
	return (&(contextInfo));
    }


    else {
	DPRINTF((APWARN, "%s: Illegal context entry.\n", FName));
	snmpStatsData.snmpStatsUnknownContexts++;
	return ((ContextInfo *) NULL);
    }
#endif				/* SR_SNMPv2 */

    /* NOTREACHED */
}				/* findContextInfo() */

#ifdef NOT_FOR_CISCO
unsigned long
partyTAddressToUnsignedLong(index)
    int             index;
{

    int             cnt, addr_part;
    char            TAddress[128], part_TAddress[16];
    unsigned long   dstAddr;

    dstAddr = 0;
    strcpy(TAddress, "");
    strcpy(part_TAddress, "");

    for (cnt = 0; cnt < parties[index].party->partyTAddress->length - 3; cnt++) {
	addr_part = parties[index].party->partyTAddress->octet_ptr[cnt];
	(void) sprintf(part_TAddress, "%d.", addr_part);
	(void) strcat(TAddress, part_TAddress);
    }
    addr_part = parties[index].party->partyTAddress->octet_ptr[cnt];
    (void) sprintf(part_TAddress, "%d", addr_part);
    (void) strcat(TAddress, part_TAddress);

    dstAddr = inet_addr(TAddress);

    return (dstAddr);
}				/* partyTAddressToUnsignedLong() */

#endif				/* SR_SNMPv2 */

/*------------------------------------------------------------
 * Copy the input pdu to an error pdu format.
 *------------------------------------------------------------*/
Pdu            *
make_error_pdu(type, req_id, status, counter, in_pdu_ptr, domain)
    int             type;
    long            req_id;
    int             status;
    int             counter;
    Pdu            *in_pdu_ptr;
    int             domain;
{
    Pdu            *out_pdu_ptr;

#ifdef	SR_SNMPv2
    /* first, do any remappings necessary */
    /* and ensure any counters are zero as appropriate */
    switch (status) {
      case TOO_BIG_ERROR:
	counter = 0;
	break;
      case GEN_ERROR:
	/* do nothing */
	break;
      case AUTHORIZATION_ERROR:
	counter = 0;
	/* no break intended here -- fall into next case */
      case NOT_WRITABLE_ERROR:
      case NO_ACCESS_ERROR:
      case NO_CREATION_ERROR:
	if (domain == SNMPv1_DOMAIN) {
	    status = NO_SUCH_NAME_ERROR;
	}
	if (counter == 0) {
	    counter = 1;
	}
	break;
      case WRONG_TYPE_ERROR:
      case WRONG_LENGTH_ERROR:
      case WRONG_ENCODING_ERROR:
      case WRONG_VALUE_ERROR:
      case INCONSISTENT_VALUE_ERROR:
      case INCONSISTENT_NAME_ERROR:
	if (domain == SNMPv1_DOMAIN) {
	    status = BAD_VALUE_ERROR;
	}
	if (counter == 0) {
	    counter = 1;
	}
	break;
      case RESOURCE_UNAVAILABLE_ERROR:
      case COMMIT_FAILED_ERROR:
      case UNDO_FAILED_ERROR:
	if (domain == SNMPv1_DOMAIN) {
	    status = GEN_ERROR;
	}
	if (counter == 0) {
	    counter = 1;
	}
	break;
      default:
	DPRINTF((APWARN, "Internal error: unknown remapping case (%d) in make_error_pdu\n", status));
	break;
    }				/* switch */
#endif				/* SR_SNMPv2 */

    /* now make the pdu */
    out_pdu_ptr = MakePdu(type, req_id, (long) status, (long) counter,
			  (OID *) NULL, (OctetString *) NULL, 0L, 0L, 0L);


    if (out_pdu_ptr == (Pdu *) NULL) {
	DPRINTF((APWARN, "make_error_pdu: Malloc of out_pdu_ptr failed.\n"));
	return ((Pdu *) NULL);
    }

    /* now do the right thing with the varbind lists */

#ifndef	SR_SNMPv2
    out_pdu_ptr->var_bind_list = in_pdu_ptr->var_bind_list;
    out_pdu_ptr->var_bind_end_ptr = in_pdu_ptr->var_bind_end_ptr;
    in_pdu_ptr->var_bind_list = NULL;
    in_pdu_ptr->var_bind_end_ptr = NULL;
    NormalizeVarBindListLengths(out_pdu_ptr->var_bind_list);
#else				/* SR_SNMPv2 */
    if ((domain == SNMPv2_DOMAIN) && (status == TOO_BIG_ERROR)) {
	/* empty varbindlist on reply */
	/* nothing to be done */
    }
    else {
	/* echo the input list to the output list */
	out_pdu_ptr->var_bind_list = in_pdu_ptr->var_bind_list;
	out_pdu_ptr->var_bind_end_ptr = in_pdu_ptr->var_bind_end_ptr;
	in_pdu_ptr->var_bind_list = NULL;
	in_pdu_ptr->var_bind_end_ptr = NULL;
	NormalizeVarBindListLengths(out_pdu_ptr->var_bind_list);
    }
#endif				/* SR_SNMPv2 */



    /* now increment the counters (only get here if successful in making pdu */
    switch (status) {
      case TOO_BIG_ERROR:
	snmpData.snmpOutTooBigs++;
	break;

#ifdef	SR_SNMPv1
      case NO_SUCH_NAME_ERROR:
	snmpData.snmpOutNoSuchNames++;
	break;
      case BAD_VALUE_ERROR:
	snmpData.snmpOutBadValues++;
	break;
#endif				/* SR_SNMPv1 */

      case GEN_ERROR:
	snmpData.snmpOutGenErrs++;
	break;

#ifdef SR_SNMPv2
      case NO_ACCESS_ERROR:
      case WRONG_TYPE_ERROR:
      case WRONG_LENGTH_ERROR:
      case WRONG_ENCODING_ERROR:
      case WRONG_VALUE_ERROR:
      case NO_CREATION_ERROR:
      case INCONSISTENT_VALUE_ERROR:
      case RESOURCE_UNAVAILABLE_ERROR:
      case COMMIT_FAILED_ERROR:
      case UNDO_FAILED_ERROR:
      case AUTHORIZATION_ERROR:
      case NOT_WRITABLE_ERROR:
      case INCONSISTENT_NAME_ERROR:
	break;
#endif				/* SR_SNMPv2 */

      default:
	DPRINTF((APWARN, "Internal error: unknown counter case (%d) in make_error_pdu\n", status));
	break;
    }				/* switch (status) */

    return (out_pdu_ptr);
}				/* make_error_pdu() */
