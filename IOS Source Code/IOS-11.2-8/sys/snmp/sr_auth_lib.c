/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_auth_lib.c,v 3.2 1995/11/17 18:58:51 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/snmp/sr_auth_lib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, SNMP Research
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_auth_lib.c,v $
 * Revision 3.2  1995/11/17  18:58:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:16:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:48:45  hampton
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

#ifndef lint

#ifdef	SR_RCSID
static char     rcsid[] = "auth_lib.c,v 1.20 1995/02/06 18:11:06 battle Exp";
#endif				/* SR_RCSID */

#endif				/* lint */

/*
 * Revision History:
 *
 * auth_lib.c,v
 * Revision 1.20  1995/02/06  18:11:06  battle
 * Changes for vrtx.
 *
 * Revision 1.19  1994/12/09  19:43:17  reid
 * removed extern global_start_time. This only affects FTP && SR_AGENT
 *
 * Revision 1.18  1994/11/07  13:50:11  pearson
 * Converted APWARN to APTRACE in auth_lib.c
 *
 * Revision 1.17  1994/10/28  16:37:23  vaughn
 * Placed new copyright in the file
 *
 * Revision 1.16  1994/10/26  21:51:29  vaughn
 * Changed copyrights to the new copyright structure
 *
 * Revision 1.15  1994/09/30  16:10:31  battle
 * hprt changes
 *
 * Revision 1.14  1994/09/19  13:39:00  levi
 * Removed #include "snmpuser.h" from files that don't need it.
 *
 * Revision 1.13  1994/06/03  14:27:08  kallen
 * changed #if to #ifdef to get around a bug in sox's unifdef
 *
 * Revision 1.12  1994/05/13  16:43:02  hecht
 * Removed the use of the clock_vals array. It's now an element in the
 * PARTY_TABLE structure (the parties array). So, clock_vals[index] becomes
 * parties[index].clockVal.
 *
 * Revision 1.11  1994/04/18  18:28:20  battle
 * pSOS changes
 *
 * Revision 1.10  1994/03/03  15:39:07  vaughn
 * Changed 0's in DPRINTF to APALWAYS.
 *
 * Revision 1.9  1994/02/16  15:03:51  reid
 * added #include snmpuser.h to get prototypes
 *
 * Revision 1.8  1994/02/14  16:38:53  pearson
 * Changed copyright notice to be current for 1994, and added an
 * #ifdef SR_RCSID
 * around the static  declarations of rcsid in each file.
 *
 * AMP
 *
 * Revision 1.7  1994/02/08  18:43:29  reid
 * added support for WinSock
 *
 * Revision 1.6  1994/02/08  00:09:18  pearson
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
 * Revision 1.5  1994/02/04  16:26:07  battle
 * Deleted unused (and un-mentioned) variable cloned_message from CheckDigest.
 *
 * Revision 1.4  1993/12/23  16:53:49  battle
 * Added cvs headers to all .c files.
 *
 *
 *  2/6/89 JDC
 *  Amended copyright notice
 *
 *  Expanded comments and documentation
 *
 *  Cleanup some memory leaks under error conditions
 *
 *  8/3/89 JDC
 *  Cleanup some lint nits
 *
 *  11/8/89 JDC
 *  Make it print pretty via tgrind
 *
 *  01/08/90 JDC
 *  Added SILENT
 *
 *  01-Jan-91 JDC
 *  Changed SILENT to LIB_ERROR and LIB_PERROR
 *
 *  01-Jan-91 JDC
 *  Substituted NULLIT(pointer) for LIGHT
 *
 *  18-Feb-91 JDC
 *  Ditched LIB_PERROR
 *
 *  27-Jun-92 JDC
 *  SMP changes
 *
 *  27-July-92 MRD
 *  Added FreeOctetString() calls in MakeAuthentication for dummy
 *  variable. This was a memory leak.
 *
 * 15-Oct-1992 DLB
 * Added a call to bzero to zero out auth_ptr->privData->octet_ptr before it is
 * used since length is increased to a multiple of 8 later and those extra
 * bytes are never initialized, and are encrypted in with other data.
 *
 * 8-Dec-1992 DLB
 *   Cleaned up some lint nits.
 *
 * 16-Dec-1992 MGH
 *   Made minor changes to MakeAuthentication(), BuildAuthentication(),
 *   FreeAuthentication(), and ParseAuthentication().
 *
 * 6-Jan-1993 MGH
 * Changed all references of "target" to "context".
 *
 * 7-Jan-1993 MGH
 * Changed the src_timestamp and dst_timestamp parameters of
 * MakeAuthentication() to unsigned long.  Changed all occurrences of
 * src_timestamp and dst_timestamp (from the AuthHeader) to unsigned long
 * operations.
 *
 * 8-Feb-1993 MGH
 * Added a parameter, error_code, to ParseAuthentication() and CheckDigest().
 * This parameter will be set to an error code if an error occurs in either
 * one of the routines, and is used by the agent to increment the correct
 * snmpStats counter.
 *
 * 24-Mar-1993 MGH
 * Added code in CheckDigest() to check for clock overflow and whether the
 * party clocks need to be written to disk.
 *
 * 5-May-1993 MGH
 * Changed all LIB_ERROR* messages to DPRINTF messages.
 *
 * 10-May-1993 MGH
 * Changed all references to EXPORT to match the new compliance levels in
 * RFC 1447.
 *
 * 2-June-93 MGH
 * Changed call to WriteParties() so it has no arguments passed to it.
 *
 * 10-June-93 MGH
 * Changed method of accessing the party table.  See readv2.c and secure.h
 * Changed bzero calls to memset calls.
 *
 * 24-June-1993 MGH
 * Moved call of CheckDigest from findAsod() (in snmpv2.c) to
 * ParseAuthentication().  Moved the dstParty/privDst check from do_snmpv2()
 * (in snmpv2.c) to ParseAuthentication(). Fleshed out comments in
 * ParseAuthentication() and BuildAuthentication().  Moved check of version
 * from do_snmpv1 (snmpv1.c) to ParseAuthentication().
 *
 * 26-July-1993 MGH
 * Changed ParseAuthentication() to fill in the authPrivate and privPrivate
 * fields of the AuthHeader structure when needed. It also fills in the
 * privType field of the AuthHeader structure for the response packet.
 * The CheckDigest() routine fills in the src_timestamp and dst_timestamp
 * fields for the response packet.
 *
 * 4-Aug-1993 MGH
 * Redid header blocks for many routines.  Added header blocks for other
 * routines. Removed cloning of SNMP message before doing digest computation
 * in MakeDigest() and CheckDigest().
 */


#include "master.h"


#include "sr_snmp.h"
#include "sr_diag.h"

#ifdef SR_SNMPv2

#ifndef SR_UNSECURABLE
#include "../util/md5.h"
#endif				/* SR_UNSECURABLE */

#include "sr_secure.h"
#include "sr_snmpuser.h"

extern unsigned long basetime;
#endif				/* SR_SNMPv2 */

/*
 * Authentication is now an ASN.1 entity, so make it here.
 */

#ifndef SR_SNMPv2
/*
 * MakeAuthentication: This routine is used to create a library format
 *                     authentication header data structure for use by
 *                     BuildAuthentication().  This particular
 *                     implementation of the library creates an
 *                     authentication header based on the 'trivial'
 *                     authentication put forth by RFC1155-56-57, which is
 *                     a community octet string ususally based on text.
 *                     The header and the octet string associated with the
 *                     header are free'ed when FreeAuthentication() is
 *                     called with the authentication pointer.
 *
 * Arguments: community - the community string to be put in the newly
 *                        malloc'ed AuthHeader structure which is returned
 *                        by this routine.
 *
 * Returns: The newly created AuthHeader structure on success,
 *          NULL on failure.
 *
 * The former name (pre-snmp12.1.0.0 release) was make_authentication().
 */
AuthHeader     *
MakeAuthentication(community)
    OctetString    *community;
{
    AuthHeader     *auth_ptr;

    if ((auth_ptr = (AuthHeader *) malloc(sizeof(AuthHeader))) == NULL) {
	DPRINTF((APWARN, "MakeAuthentication, auth_ptr malloc\n"));
	return (NULL);
    }
    /* NULL out the structure */
    memset((char *) auth_ptr, 0, sizeof(AuthHeader));

#ifdef	SR_SNMPv1
    auth_ptr->version = VERSION;
#endif				/* SR_SNMPv1 */

    auth_ptr->community = community;

    return (auth_ptr);
}				/* end of MakeAuthentication() */

#else				/* SR_SNMPv2 */
/*
 * MakeAuthentication: This routine is used to create a library format
 *                     authentication header data structure for use by
 *                     BuildAuthentication().  This particular implementation
 *                     of the library creates an authentication header based
 *                     on RFC1445-46-47. The header and the octet string
 *                     associated with the header are free'ed when
 *                     FreeAuthentication() is called with the authentication
 *                     pointer. All of the parameters except for community
 *                     and authType are ignored on a SNMPv1 packet. The
 *                     community parameter is ignored on a SNMPv2 packet.
 *
 *
 * Arguments: community     - the community string to be put in the newly
 *                            created AuthHeader structure which is returned
 *                            by this routine (only if SNMPv1 packet).
 *
 *            privDst       - The value for the privDst field of the SNMPv2
 *                            AuthHeader structure returned by this routine.
 *
 *            dstParty      - The value for the dstParty field of the SNMPv2
 *                            AuthHeader structure returned by this routine.
 *
 *            srcParty      - The value for the srcParty field of the SNMPv2
 *                            AuthHeader structure returned by this routine.
 *
 *            context       - The value for the context field of the SNMPv2
 *                            AuthHeader structure returned by this routine.
 *
 *            authType      - This is the value for the authType field of the
 *                            SNMPv2 AuthHeader structure returned by this
 *                            routine. This field indicates the authentication
 *                            type for this packet.
 *
 *            authPrivate   - The value for the authPrivate field of the
 *                            SNMPv2 AuthHeader structure returned by this
 *                            routine.
 *
 *            src_timestamp - The source party timestamp for this packet.
 *
 *            dst_timestamp - The destination party timestamp for this packet.
 *
 *            privType      - The value for the privType field of the SNMPv2
 *                            AuthHeader structure returned by this routine.
 *                            This field indicates the privacy type for this
 *                            packet.
 *
 *            privPrivate   - Put this value in the privPrivate field of the
 *                            SNMPv2 AuthHeader structure returned by this
 *                            routine.
 *
 * Returns: The newly created AuthHeader structure on success,
 *          NULL on failure.
 *
 * The former name (pre-snmp12.1.0.0 release) was make_authentication().
 */

AuthHeader     *
MakeAuthentication(community,
		   privDst, dstParty, srcParty, context,
		   authType, authPrivate, src_timestamp, dst_timestamp,
		   privType, privPrivate)
    OctetString    *community;
    OID            *privDst;
    OID            *dstParty;
    OID            *srcParty;
    OID            *context;
    int             authType;
    OctetString    *authPrivate;
    unsigned long   src_timestamp;
    unsigned long   dst_timestamp;
    int             privType;
    OctetString    *privPrivate;
{
    AuthHeader     *auth_ptr;

    if ((auth_ptr = (AuthHeader *) malloc(sizeof(AuthHeader))) == NULL) {
	DPRINTF((APWARN, "MakeAuthentication, auth_ptr malloc\n"));
	return (NULL);
    }
    /* NULL out the structure */
    memset((char *) auth_ptr, 0, sizeof(AuthHeader));

#ifdef	SR_SNMPv1
    auth_ptr->version = VERSION;
#endif				/* SR_SNMPv1 */

    auth_ptr->authType = authType;
    auth_ptr->srcPartyIndex = -1;
    auth_ptr->dstPartyIndex = -1;
    auth_ptr->contextIndex = -1;

    switch (authType) {

#ifdef	SR_SNMPv1
      case TRIVIAL_AUTHENTICATION:
	auth_ptr->community = community;
	break;
#endif				/* SR_SNMPv1 */

      case NOAUTH_AUTHENTICATION:

#ifndef SR_UNSECURABLE
      case MD5_SNMPv2_AUTHENTICATION:
#endif				/* SR_UNSECURABLE */

	auth_ptr->privDst = privDst;
	auth_ptr->dstParty = dstParty;
	auth_ptr->srcParty = srcParty;
	auth_ptr->context = context;
	auth_ptr->authPrivate = authPrivate;
	auth_ptr->privType = privType;

#ifndef SR_UNSECURABLE

#ifndef SR_NO_PRIVACY
	if (privType == DESPRIV) {
	    auth_ptr->privPrivate = privPrivate;
	}
#endif				/* SR_NO_PRIVACY */

#endif				/* SR_UNSECURABLE */

	auth_ptr->src_timestamp = src_timestamp;
	auth_ptr->dst_timestamp = dst_timestamp;

	break;
      default:
	DPRINTF((APTRACE, "MakeAuthentication, invalid authentication type (%d) (0x%x)\n", authType, authType));
	FreeAuthentication(auth_ptr);
	NULLIT(auth_ptr);
	return (NULL);
	break;
    }

    return (auth_ptr);
}				/* end of MakeAuthentication() */

#endif				/* SR_SNMPv2 */


/*
 * BuildAuthentication: This routine takes the Pdu (pdu_ptr) and the
 *                      authentication information (auth_ptr) and builds the
 *                      actual SNMP packet in the authentication data
 *                      structure's *packlet octet string area.
 *
 *                      If this is a SNMPv1 packet, it uses the procedures
 *                      described in RFC1155-56-57.  If this is a SNMPv2
 *                      packet, it uses the procedures described in
 *                      RFC1445-46-47.
 *
 *                      The 'packlet' is free'ed when FreeAuthentication()
 *                      is called, so it should be copied to a holding area
 *                      or the authentication should not be free'ed until the
 *                      packet is actually sent. Once this has been done, the
 *                      authentication structure (auth_ptr) can be free'ed
 *                      with a call to FreeAuthentication().
 *
 * Arguments: auth_ptr - The packlet field of this structure will contain the
 *                       message to be sent out when this routine completes
 *                       successfully in the packlet field of the structure.
 *                       The packlet field should contain the serialized SNMP
 *                       Pdu, which is stored in the incoming parameter,
 *                       pdu_ptr, and it should contain the serialized
 *                       information for the SNMP wrapper.  Other elements of
 *                       auth_ptr are filled in in this routine.
 *
 *            pdu_ptr  - Contains the serialized SNMP packet to be sent.
 *
 * Returns:  0 - on success
 *          -1 - on failure.
 *
 * The former name (pre-snmp12.1.0.0 release) was build_authentication().
 */

short
BuildAuthentication(auth_ptr, pdu_ptr)
    AuthHeader     *auth_ptr;
    Pdu            *pdu_ptr;
{
    long            i;
    long            lenlen;

    long            pdudatalen;
    long            datalen;
    unsigned char  *working_ptr;

#ifdef SR_SNMPv2
    long            mgmt_com_len, len_mgmt_com_len;
    long            auth_info_len, len_auth_info_len;
    long            auth_msg_len, len_auth_msg_len;
    long            priv_data_len;
    long            temp_data_len;

#ifndef SR_UNSECURABLE

#ifndef SR_NO_PRIVACY
    char            key[8];
    char            iv[8];
    char           *cp, *cp1;
    int             cnt;
    char           *work;
#endif				/* SR_NO_PRIVACY */

#endif				/* SR_UNSECURABLE */

#endif				/* SR_SNMPv2 */

    /* make sure auth_ptr->packlet is clean and ready to roll */
    FreeOctetString(auth_ptr->packlet);
    NULLIT(auth_ptr->packlet);

    if ((auth_ptr->packlet = (OctetString *) malloc(sizeof(OctetString))) == NULL) {
	DPRINTF((APWARN, "BuildAuthentication, packlet malloc\n"));
	return (-1);
    }
    auth_ptr->packlet->octet_ptr = NULL;

    if (pdu_ptr == NULL) {
	DPRINTF((APTRACE, "BuildAuthentication, NO PDU.\n"));
	return (-1);
    }

    pdudatalen = pdu_ptr->packlet->length;

#ifndef	SR_SNMPv2
    datalen = FindLenUInt(auth_ptr->version) +
	FindLenOctetString(auth_ptr->community) + pdudatalen;
    lenlen = DoLenLen(datalen);

    auth_ptr->packlet->length = 1 + lenlen + datalen;

    if ((auth_ptr->packlet->octet_ptr =
       (unsigned char *) malloc((int) auth_ptr->packlet->length)) == NULL) {
	DPRINTF((APWARN, "BuildAuthentication malloc\n"));
	return (-1);
    }

    working_ptr = auth_ptr->packlet->octet_ptr;

    *working_ptr++ = SEQUENCE_TYPE;
    AddLen(&working_ptr, lenlen, datalen);

    (void) AddUInt(&working_ptr, INTEGER_TYPE, auth_ptr->version);
    /* %%% DLB should check return valud of AddOctetString */
    (void) AddOctetString(&working_ptr, OCTET_PRIM_TYPE, auth_ptr->community);

    for (i = 0; i < pdudatalen; i++) {
	*working_ptr++ = pdu_ptr->packlet->octet_ptr[i];
    }
    return (0);
#else				/* SR_SNMPv2 */
    switch (auth_ptr->authType) {

#ifdef	SR_SNMPv1
      case TRIVIAL_AUTHENTICATION:
	datalen = FindLenUInt(auth_ptr->version) +
	    FindLenOctetString(auth_ptr->community) + pdudatalen;
	lenlen = DoLenLen(datalen);

	auth_ptr->packlet->length = 1 + lenlen + datalen;

	if ((auth_ptr->packlet->octet_ptr =
	     (unsigned char *) malloc((unsigned) auth_ptr->packlet->length)) == NULL) {
	    DPRINTF((APWARN, "BuildAuthentication malloc\n"));
	    return (-1);
	}

	working_ptr = auth_ptr->packlet->octet_ptr;

	*working_ptr++ = SEQUENCE_TYPE;
	AddLen(&working_ptr, lenlen, datalen);

	(void) AddUInt(&working_ptr, INTEGER_TYPE, auth_ptr->version);
	/* %%% DLB should check return valud of AddOctetString */
	(void) AddOctetString(&working_ptr, OCTET_PRIM_TYPE, auth_ptr->community);

	for (i = 0; i < pdudatalen; i++) {
	    *working_ptr++ = pdu_ptr->packlet->octet_ptr[i];
	}
	return (0);
	break;
#endif				/* SR_SNMPv1 */

	/*
	 * From RFC1445, page 17, section 3.1:
	 * 
	 * (1) A SnmpMgmtCom value is constructed for which the srcParty
	 * component identifies the originating party, for which the dstParty
	 * component identifies the receiving party, for which the context
	 * component identifies the desired SNMPv2 context, and for which the
	 * pdu component represents the desired management operation.
	 * 
	 * (2) The local database of party information is consulted to determine
	 * the authentication protocol and other relevant information for the
	 * originating and receiving SNMPv2 parties.
	 * 
	 * (3) A SnmpAuthMsg value is constructed with the following properties:
	 * 
	 * Its authInfo component is constructed according to the authentication
	 * protocol specified for the originating party.
	 * 
	 * In particular, if the authentication protocol for the originating
	 * SNMPv2 party is identified as noAuth, then this component
	 * corresponds to the OCTET STRING value of zero length.
	 * 
	 * Its authData component is the constructed SnmpMgmtCom value.
	 * 
	 * In RFC1446, page 17, section 3.1:
	 * 
	 * According to Section 3.1 of [1], a SnmpAuthMsg value is constructed
	 * during Step 3 of generic processing.  In particular, it states the
	 * authInfo component is constructed according to the authentication
	 * protocol identified for the SNMPv2 party originating the message.
	 * When the relevant authentication protocol is the Digest
	 * Authentication Protocol, the procedure performed by a SNMPv2
	 * entity whenever a management communication is to be transmitted by
	 * a SNMPv2 party is as follows.
	 * 
	 * (1) The local database is consulted to determine the authentication
	 * clock and private authentication key (extracted, for example,
	 * according to the conventions defined in Section 1.5.1) of the
	 * SNMPv2 party originating the message.  The local database is also
	 * consulted to determine the authentication clock of the receiving
	 * SNMPv2 party.
	 * 
	 * (2) The authSrcTimestamp component is set to the retrieved
	 * authentication clock value of the message's source.  The
	 * authDstTimestamp component is set to the retrieved authentication
	 * clock value of the message's intended recipient.
	 * 
	 * (3) The authentication digest is temporarily set to the private
	 * authentication key of the SNMPv2 party originating the message.
	 * The SnmpAuthMsg value is serialized according to the conventions
	 * of [13] and [12]. A digest is computed over the octet sequence
	 * representing that serialized value using, for example, the
	 * algorithm specified in Section 1.5.1.  The authDigest component is
	 * set to the computed digest value.
	 * 
	 * Steps 1-2 are done in the call to MakeAuthentication(). The digest is
	 * computed in the call to MakeDigest().
	 */

#ifndef SR_UNSECURABLE
      case MD5_SNMPv2_AUTHENTICATION:

	/*
	 * First, fill in the digest with the appropriate private auth. key
	 */
	if ((auth_ptr->digest = MakeOctetString(auth_ptr->authPrivate->octet_ptr,
			   (long) auth_ptr->authPrivate->length)) == NULL) {
	    DPRINTF((APWARN, "BuildAuthentication, make octet string (digest)\n"));
	    return (-1);
	}

	/*
	 * No break here.  This is a continuation of the above case
	 */
#endif				/* SR_UNSECURABLE */

      case NOAUTH_AUTHENTICATION:
	mgmt_com_len = 0;
	if ((temp_data_len = FindLenOID(auth_ptr->dstParty)) == -1) {
	    DPRINTF((APTRACE, "BuildAuthentication, temp_data_len (dstParty)\n"));
	    return (-1);
	}
	mgmt_com_len += temp_data_len;
	if ((temp_data_len = FindLenOID(auth_ptr->srcParty)) == -1) {
	    DPRINTF((APTRACE, "BuildAuthentication, temp_data_len (srcParty)\n"));
	    return (-1);
	}
	mgmt_com_len += temp_data_len;

	if ((temp_data_len = FindLenOID(auth_ptr->context)) == -1) {
	    DPRINTF((APTRACE, "BuildAuthentication, temp_data_len (context)\n"));
	    return (-1);
	}
	mgmt_com_len += temp_data_len;

	mgmt_com_len += pdudatalen;
	len_mgmt_com_len = DoLenLen(mgmt_com_len);
	mgmt_com_len = 1 /* implicit seq */ + len_mgmt_com_len + mgmt_com_len;

	switch (auth_ptr->authType) {

#ifndef SR_UNSECURABLE
	  case MD5_SNMPv2_AUTHENTICATION:
	    if ((auth_info_len = FindLenOctetString(auth_ptr->digest)) == -1) {
		DPRINTF((APTRACE, "BuildAuthentication, auth_info_len (digest)\n"));
		return (-1);
	    }
	    auth_info_len += FindLenUInt(auth_ptr->dst_timestamp) +
		FindLenUInt(auth_ptr->src_timestamp);
	    len_auth_info_len = DoLenLen(auth_info_len);
	    auth_info_len = 1 /* implicit seq */ + len_auth_info_len + auth_info_len;
	    break;
#endif				/* SR_UNSECURABLE */

	  case NOAUTH_AUTHENTICATION:
	    auth_info_len = 2;
	    len_auth_info_len = 2;
	    break;
	  default:
	    DPRINTF((APTRACE, "BuildAuthentication, invalid authentication type\n"));
	    FreeAuthentication(auth_ptr);
	    NULLIT(auth_ptr);
	    return (-1);
	    break;
	}			/* inner switch  (auth_ptr->authType) */
	auth_msg_len = auth_info_len + mgmt_com_len;
	len_auth_msg_len = DoLenLen(auth_msg_len);
	auth_msg_len = 1 /* implicit seq */ + len_auth_msg_len + auth_msg_len;

	if ((auth_ptr->privData = (OctetString *) malloc(sizeof(OctetString))) == (OctetString *) NULL) {
	    DPRINTF((APWARN, "BuildAuthentication:"));
	    return (-1);
	}

#ifndef SR_UNSECURABLE

#ifndef SR_NO_PRIVACY
	/* malloc an extra 8 bytes in case des needs it for padding */
#endif				/* SR_NO_PRIVACY */

#endif				/* SR_UNSECURABLE */

	if ((auth_ptr->privData->octet_ptr =
	 (unsigned char *) malloc((unsigned) (auth_msg_len + 8))) == NULL) {
	    DPRINTF((APWARN, "BuildAuthentication:"));
	    return (-1);
	}

	/*
	 * Message length is padded to multiple of 8 later, so make sure it
	 * starts out all zeros.
	 */
	/*
	 * Changed it to only zero out the last eight bytes.  The rest will
	 * be used.  Performance issue.
	 */
	memset((char *) (auth_ptr->privData->octet_ptr + auth_msg_len), 0, 8);

	working_ptr = auth_ptr->privData->octet_ptr;
	*working_ptr++ = AUTH_MSG_TYPE;
	AddLen(&working_ptr, len_auth_msg_len, auth_msg_len - 1 - len_auth_msg_len);

#ifndef SR_UNSECURABLE
	if (auth_ptr->authType == MD5_SNMPv2_AUTHENTICATION) {
	    /*
	     * Only MD5 is used for authentication right now.
	     */
	    *working_ptr++ = SNMPv2_MD5_AUTHINFO_TYPE;
	    AddLen(&working_ptr, len_auth_info_len, auth_info_len - 1 - len_auth_info_len);

	    auth_ptr->offset_to_digest = working_ptr - auth_ptr->privData->octet_ptr + 2;
	    /* %%% DLB should check return valud of AddOctetString */
	    (void) AddOctetString(&working_ptr, OCTET_PRIM_TYPE, auth_ptr->digest);

	    (void) AddUInt(&working_ptr, U_INTEGER_32_TYPE, auth_ptr->dst_timestamp);
	    (void) AddUInt(&working_ptr, U_INTEGER_32_TYPE, auth_ptr->src_timestamp);

	}
#endif				/* SR_UNSECURABLE */

	if (auth_ptr->authType == NOAUTH_AUTHENTICATION) {
	    /* NOAUTH_AUTHENTICATION --> zero length octet string */
	    *working_ptr++ = OCTET_PRIM_TYPE;
	    *working_ptr++ = '\0';
	}

	*working_ptr++ = MGMT_COM_TYPE;
	AddLen(&working_ptr, len_mgmt_com_len, mgmt_com_len - 1 - len_mgmt_com_len);

	(void) AddOID(&working_ptr, OBJECT_ID_TYPE, auth_ptr->dstParty);
	(void) AddOID(&working_ptr, OBJECT_ID_TYPE, auth_ptr->srcParty);
	(void) AddOID(&working_ptr, OBJECT_ID_TYPE, auth_ptr->context);

	for (i = 0; i < pdudatalen; i++) {
	    *working_ptr++ = pdu_ptr->packlet->octet_ptr[i];
	}
	auth_ptr->privData->length = auth_msg_len;

#ifndef SR_UNSECURABLE
	/*
	 * Now that the privData is built up, recompute the digest here
	 */
	if (auth_ptr->authType == MD5_SNMPv2_AUTHENTICATION) {

	    auth_ptr->bytecount = auth_msg_len;

	    if (MakeDigest(auth_ptr) == FALSE) {
		DPRINTF((APTRACE, "BuildAuthentication, MakeDigest\n"));
		return (-1);
	    }
	}
	else {			/* NOAUTH_AUTHENTICATION */
	    /* do nothing */
	}

	/*
	 * From RFC1445, pages 17-18, section 3.1:
	 * 
	 * (4) The local database of party information is consulted to determine
	 * the privacy protocol and other relevant information for the
	 * receiving SNMPv2 party.
	 * 
	 * (5) A SnmpPrivMsg value is constructed with the following properties:
	 * 
	 * Its privDst component identifies the receiving SNMPv2 party.
	 * 
	 * Its privData component is the (possibly encrypted) serialization of
	 * the SnmpAuthMsg value according to the conventions of [5].
	 * 
	 * In particular, if the privacy protocol for the receiving SNMPv2 party
	 * is identified as noPriv, then the privData component is
	 * unencrypted. Otherwise, the privData component is processed
	 * according to the privacy protocol.
	 * 
	 * (6) The constructed SnmpPrivMsg value is serialized according to the
	 * conventions of [5].
	 */

#ifndef SR_NO_PRIVACY
	/*
	 * From RFC1446, page 22, section 4.1:
	 * 
	 * When the relevant privacy protocol is the Symmetric Privacy Protocol,
	 * the procedure performed by a SNMPv2 entity whenever a management
	 * communication is to be transmitted by a SNMPv2 party is as
	 * follows.
	 * 
	 * (1) If the SnmpAuthMsg value is not authenticated according to the
	 * conventions of the Digest Authentication Protocol, the generation
	 * of the private management communication fails according to a local
	 * procedure, without further processing.
	 * 
	 * (2) The local database is consulted to determine the private privacy
	 * key of the SNMPv2 party receiving the message (represented, for
	 * example, according to the conventions defined in Section 1.5.2).
	 * 
	 * (3) The SnmpAuthMsg value is serialized according to the conventions
	 * of [13] and [12].
	 * 
	 * (4) The octet sequence representing the serialized SnmpAuthMsg value
	 * is encrypted using, for example, the algorithm specified in
	 * Section 1.5.2 and the extracted private privacy key.
	 * 
	 * (5) The privData component is set to the encrypted value.
	 */


	/*
	 * Next, generate the DES for auth_ptr->privData->octet_ptr
	 */
	if (auth_ptr->privType == DESPRIV) {
	    memcpy((char *) key, (char *) auth_ptr->privPrivate->octet_ptr, 8);
	    memcpy((char *) iv, (char *) &auth_ptr->privPrivate->octet_ptr[8], 8);

	    dessetkey(key);
	    /*
	     * Make length of privData an integer multiple of 8
	     */
	    if ((auth_ptr->privData->length % 8) != 0) {
		auth_ptr->privData->length += 8 - (auth_ptr->privData->length % 8);
	    }

	    work = (char *) auth_ptr->privData->octet_ptr;
	    for (cnt = auth_ptr->privData->length; cnt > 0; cnt -= 8) {
		cp = work;
		cp1 = iv;
		for (i = 8; i != 0; i--) {
		    *cp++ ^= *cp1++;
		}
		endes(work);
		(void) memcpy(iv, work, 8);
		work += 8;
	    }
	}			/* if (auth_ptr->privType == DESPRIV) */
#endif				/* SR_NO_PRIVACY */

#endif				/* SR_UNSECURABLE */

	priv_data_len = 1 + DoLenLen(auth_ptr->privData->length) +
	    auth_ptr->privData->length;

	if ((temp_data_len = FindLenOID(auth_ptr->privDst)) == -1) {
	    DPRINTF((APTRACE, "BuildAuthentication, temp_data_len (privDst)\n"));
	    return (-1);
	}

	datalen = temp_data_len + priv_data_len;
	lenlen = DoLenLen(datalen);

	auth_ptr->packlet->length = 1 + lenlen + datalen;

	if ((auth_ptr->packlet->octet_ptr =
	     (unsigned char *) malloc((unsigned) auth_ptr->packlet->length)) == NULL) {
	    DPRINTF((APTRACE, "BuildAuthentication:"));
	    return (-1);
	}

	working_ptr = auth_ptr->packlet->octet_ptr;

	*working_ptr++ = PRIV_MSG_TYPE;
	AddLen(&working_ptr, lenlen, datalen);

	/* %%% DLB should check return valud of AddOID */
	(void) AddOID(&working_ptr, OBJECT_ID_TYPE, auth_ptr->privDst);
	/* %%% DLB should check return valud of AddOctetString */
	(void) AddOctetString(&working_ptr, PRIV_DATA_TYPE, auth_ptr->privData);

	return (0);
	break;


      default:
	DPRINTF((APTRACE, "BuildAuthentication, invalid authentication type (%d) (0x%x)\n", auth_ptr->authType, auth_ptr->authType));
	FreeAuthentication(auth_ptr);
	NULLIT(auth_ptr);
	break;
    }				/* switch */
    return (-1);
#endif				/* SR_SNMPv2 */
}				/* BuildAuthentication() */

/*
 * FreeAuthentication: If compiled SNMPv1 only, this routine frees all memory
 *                     associated with a TRIVIAL_AUTHENTICATION header data
 *                     structure, including the actual SNMP packet that
 *                     BuildAuthentication() creates and the octet string
 *                     associated with MakeAuthentication(). The Pdu structure
 *                     pointed to by the pdu_ptr passed to MakeAuthentication()
 *                     is NOT touched.
 *
 *                     If compiled with SNMPv2, this routine frees all memory
 *                     associated with a SNMPv2 authentication header data
 *                     structure, including the actual packet that
 *                     BuildAuthentication() creates. The Pdu structure pointed
 *                     to by the pdu_ptr passed to MakeAuthentication() is NOT
 *                     touched.
 *
 * Arguments: auth_ptr - the AuthHeader structure that needs to be freed.
 *
 * Returns: nothing
 *
 * The former name (pre-snmp12.1.0.0 release) was free_authentication().
 */

void
FreeAuthentication(auth_ptr)
    AuthHeader     *auth_ptr;
{
    if (auth_ptr != NULL) {

#ifdef SR_SNMPv1
	FreeOctetString(auth_ptr->community);
#endif				/* SR_SNMPv1 */

	FreeOctetString(auth_ptr->packlet);

#ifdef SR_SNMPv2
	FreeOctetString(auth_ptr->privData);
	FreeOctetString(auth_ptr->digest);
	FreeOID(auth_ptr->privDst);
	FreeOID(auth_ptr->dstParty);
	FreeOID(auth_ptr->srcParty);
	FreeOID(auth_ptr->context);

	FreeOctetString(auth_ptr->authPrivate);
	FreeOctetString(auth_ptr->privPrivate);
#endif				/* SR_SNMPv2 */

	free((char *) auth_ptr);
    }
}

/*
 * ParseAuthentication: This routine is used to create a library format
 *                      authentication header data structure from an incoming
 *                      SNMP packet. If parsing errors occur, a message is
 *                      printed if DEBUG is defined, the error_code parameter
 *                      is set to the correct error code and the routine
 *                      returns  NULL.
 *
 *                      It should be noted that the state of the authentication
 *                      header created during the building phase after a call
 *                      to BuildAuthentication() is nearly identical to the
 *                      state of the authentication header after this call on
 *                      the parsing side.
 *
 *                      If this is a SNMPv1 packet, the procedures for parsing
 *                      a packet are described in RFC1155-56-57.  If this is a
 *                      SNMPv2 packet, the procedures for parsing a packet are
 *                      described in RFC1445-46-47.
 *
 * Arguments: packet_ptr - Pointer to the incoming serialized message (SNMP
 *                         packet).
 *
 *            length     - Should be the length of the packet.
 *            error_code - Will contain the specific error that occurred if
 *                         this routine returns NULL.
 *

 * Returns: On success: A pointer to the filled in AuthHeader structure. All
 *            of the authentication information should be parsed at this
 *            point.  A call to ParsePdu() will be needed to retrieve the
 *            values in the SNMP Pdu.
 *          On failure: Returns NULL.
 *
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_authentication().
 */

AuthHeader     *
ParseAuthentication(packet_ptr, length, error_code)
    unsigned char  *packet_ptr;
    long            length;
    int            *error_code;
{
    AuthHeader     *auth_ptr;
    unsigned char  *working_ptr;
    unsigned char  *end_ptr;
    unsigned char  *new_end_ptr;
    short           type;
    long            seq_length;

#ifdef SR_SNMPv2
    OctetString    *dummy;	/* temporary place to put nothing */
    int             priv_prot;
    int             return_code;

#ifndef SR_UNSECURABLE

#ifndef SR_NO_PRIVACY
    char            key[8];
    char            iv[8];
    char           *cp, *cp1;
    int             i;
    int             cnt;
    char            ivtmp[8];
    char           *work;
#endif				/* SR_NO_PRIVACY */

#endif				/* SR_UNSECURABLE */

#endif				/* SR_SNMPv2 */

    *error_code = 0;
    working_ptr = (unsigned char *) packet_ptr;
    end_ptr = working_ptr + length;

    if ((auth_ptr = (AuthHeader *) malloc((unsigned) sizeof(AuthHeader))) == NULL) {
	DPRINTF((APWARN, "ParseAuthentication, auth_ptr malloc\n"));
	return (NULL);
    }
    memset((char *) auth_ptr, 0, sizeof(AuthHeader));

#ifdef	SR_SNMPv1
    auth_ptr->version = VERSION;
#endif				/* SR_SNMPv1 */

#ifdef SR_SNMPv2		/* 30-Apr-92 */
    auth_ptr->authType = UNKNOWN_AUTHENTICATION;
#endif				/* SR_SNMPv2 */

#ifndef	SR_SNMPv2
    if ((seq_length = ParseSequence(&working_ptr, end_ptr, &type)) == -1) {
	DPRINTF((APTRACE, "ParseAuthentication, ParseSequence:\n"));
	FreeAuthentication(auth_ptr);
	NULLIT(auth_ptr);
	return (NULL);
    }

    /*
     * Re-calculate end_ptr, based on ASN.1 structure's internal length. If
     * they do not match up, use the minimum
     */

    new_end_ptr = working_ptr + seq_length;

    if (new_end_ptr < end_ptr) {
	end_ptr = MIN(end_ptr, new_end_ptr);
    }
    else if (new_end_ptr > end_ptr) {
	DPRINTF((APTRACE, "ParseAuthentication, bad BER length:\n"));
	FreeAuthentication(auth_ptr);
	NULLIT(auth_ptr);
	return (NULL);
    }

    auth_ptr->version = ParseUInt(&working_ptr, end_ptr, &type);
    if (type == -1) {
	DPRINTF((APTRACE, "ParseAuthentication, version:\n"));
	FreeAuthentication(auth_ptr);
	NULLIT(auth_ptr);
	return (NULL);
    }

    if (auth_ptr->version != VERSION) {
	DPRINTF((APTRACE, "ParseAuthentication: version failure.\n"));
	FreeAuthentication(auth_ptr);
	NULLIT(auth_ptr);
	*error_code = SNMP_BAD_VERSION_ERROR;
	return (NULL);
    }

    if ((auth_ptr->community = ParseOctetString(&working_ptr, end_ptr, &type)) ==
	NULL) {
	DPRINTF((APTRACE, "ParseAuthentication, community\n"));
	FreeAuthentication(auth_ptr);
	NULLIT(auth_ptr);
	return (NULL);
    }

    /* remember how big the wrapper is/was */
    auth_ptr->wrapper_size = working_ptr - packet_ptr;

#else				/* SR_SNMPv2 */

    switch (*working_ptr) {

	/*
	 * From RFC1445, page 18, Section 3.2:
	 * 
	 * (1) If the received message is not the serialization (according to
	 * the conventions of [5]) of an SnmpPrivMsg value, then that message
	 * is discarded without further processing. (If the first octet of
	 * the packet has the value hexadecimal 30, then the
	 * snmpStats30Something counter [7] is incremented prior to
	 * discarding the message; otherwise the snmpStatsEncodingErrors
	 * counter [7] is incremented.)
	 * 
	 * The snmpStats30Something counter [7] is only incremented if this is a
	 * SNMPv2 only entity.
	 */

#ifndef SR_SNMPv1
      case SEQUENCE_TYPE:
	*error_code = THIRTY_SOMETHING_ERROR;
	DPRINTF((APTRACE, "ParseAuthentication: Triv packet received on SNMPv2 only agent.\n"));
	FreeAuthentication(auth_ptr);
	NULLIT(auth_ptr);
	return (NULL);
	break;
#endif				/* SR_SNMPv1 */

#ifdef	SR_SNMPv1
      case SEQUENCE_TYPE:
	auth_ptr->authType = TRIVIAL_AUTHENTICATION;

	if ((seq_length = ParseSequence(&working_ptr, end_ptr, &type)) == -1) {
	    DPRINTF((APTRACE, "ParseAuthentication, ParseSequence\n"));
	    FreeAuthentication(auth_ptr);
	    NULLIT(auth_ptr);
	    *error_code = ENCODING_ERROR;
	    return (NULL);
	}

	/*
	 * Re-calculate end_ptr, based on ASN.1 structure's internal length.
	 * If they do not match up, set the length to the minimum of the two
	 * lengths.
	 */

	new_end_ptr = working_ptr + seq_length;

	if (new_end_ptr < end_ptr) {
	    end_ptr = MIN(end_ptr, new_end_ptr);
	}
	else if (new_end_ptr > end_ptr) {
	    DPRINTF((APTRACE, "ParseAuthentication, bad BER length:\n"));
	    FreeAuthentication(auth_ptr);
	    NULLIT(auth_ptr);
	    return (NULL);
	}

	auth_ptr->version = ParseUInt(&working_ptr, end_ptr, &type);
	if (type == -1) {
	    DPRINTF((APTRACE, "ParseAuthentication, version\n"));
	    FreeAuthentication(auth_ptr);
	    NULLIT(auth_ptr);
	    *error_code = ENCODING_ERROR;
	    return (NULL);
	}

	if (auth_ptr->version != VERSION) {
	    DPRINTF((APTRACE, "ParseAuthentication: version failure.\n"));
	    FreeAuthentication(auth_ptr);
	    NULLIT(auth_ptr);
	    *error_code = SNMP_BAD_VERSION_ERROR;
	    return (NULL);
	}

	if ((auth_ptr->community = ParseOctetString(&working_ptr, end_ptr, &type)) == NULL) {
	    DPRINTF((APTRACE, "ParseAuthentication, community\n"));
	    FreeAuthentication(auth_ptr);
	    NULLIT(auth_ptr);
	    *error_code = ENCODING_ERROR;
	    return (NULL);
	}

	/* remember how big the wrapper is/was */
	auth_ptr->wrapper_size = working_ptr - packet_ptr;

	break;
#endif				/* SR_SNMPv1 */

      case PRIV_MSG_TYPE:
	auth_ptr->authType = DIGEST_UNKNOWN_AUTHENTICATION;
	type = PRIV_MSG_TYPE;
	if ((seq_length =
	     ParseImplicitSequence(&working_ptr, end_ptr, &type)) == -1) {
	    DPRINTF((APTRACE, "ParseAuthentication, parse priv message type\n"));
	    FreeAuthentication(auth_ptr);
	    NULLIT(auth_ptr);
	    *error_code = ENCODING_ERROR;
	    return (NULL);
	}

#ifndef SR_UNSECURABLE

#ifndef SR_NO_PRIVACY
	/*
	 * Re-calculate end_ptr, based on ASN.1 structure's internal length.
	 * If they do not match up, and they often do not because of the
	 * padding added to make des message an integral multiple of 8 bytes
	 * in length then set the length to the minimum of the two lengths
	 */
#endif				/* SR_NO_PRIVACY */

#endif				/* SR_UNSECURABLE */

	new_end_ptr = working_ptr + seq_length;

	if (new_end_ptr < end_ptr) {
	    end_ptr = MIN(end_ptr, new_end_ptr);
	}
	else if (new_end_ptr > end_ptr) {
	    DPRINTF((APTRACE, "ParseAuthentication, bad BER length:\n"));
	    FreeAuthentication(auth_ptr);
	    NULLIT(auth_ptr);
	    return (NULL);
	}

	/*
	 * From RFC1445, pages 18-19, Section 3.2:
	 * 
	 * (2)   The local database of party information is consulted for
	 * information about the receiving SNMPv2 party identified by the
	 * privDst component of the SnmpPrivMsg value.
	 * 
	 * (3)  If information about the receiving SNMPv2 party is absent from
	 * the local database of party information, or indicates that the
	 * receiving party's operation is not realized by the local SNMPv2
	 * entity, then the received message is discarded without further
	 * processing, after the snmpStatsUnknownDstParties counter [7] is
	 * incremented.
	 */

	auth_ptr->privDst = ParseOID(&working_ptr, end_ptr);
	if (type == -1) {
	    DPRINTF((APTRACE, "ParseAuthentication, privDst\n"));
	    FreeAuthentication(auth_ptr);
	    NULLIT(auth_ptr);
	    *error_code = ENCODING_ERROR;
	    return (NULL);
	}

	/*
	 * parties and num_parties are globals declared in secure.h and
	 * assigned their initial values in secure.c
	 */
	auth_ptr->dstPartyIndex = GetPartyIndex(auth_ptr->privDst, SR_LOCAL);
	if (auth_ptr->dstPartyIndex == -1) {
	    DPRINTF((APTRACE,
		     "Received packet with unknown destination party ... dropping\n"));
	    FreeAuthentication(auth_ptr);
	    NULLIT(auth_ptr);
	    *error_code = DST_PARTY_ERROR;
	    return (NULL);
	}

	if (parties[auth_ptr->dstPartyIndex].party->domain == SNMPv2_DOMAIN) {
	    auth_ptr->domain = SNMPv2_DOMAIN;

#ifndef SR_UNSECURABLE
	    auth_ptr->authType = MD5_SNMPv2_AUTHENTICATION;
#else				/* SR_UNSECURABLE */
	    auth_ptr->authType = NOAUTH_AUTHENTICATION;
#endif				/* SR_UNSECURABLE */
	}

	else {
	    DPRINTF((APTRACE, "ParseAuthentication, unknown domain (%d)\n",
		     parties[auth_ptr->dstPartyIndex].party->domain));
	    FreeAuthentication(auth_ptr);
	    NULLIT(auth_ptr);
	    return (NULL);
	}


	/*
	 * This will not get the length of the tag and the length of the
	 * length but we add in the worst case values later anyway, so it is
	 * fine
	 */
	auth_ptr->wrapper_size = working_ptr - packet_ptr;

	/*
	 * From RFC1445, page 19, Section 3.2:
	 * 
	 * (4)  An ASN.1 OCTET STRING value is constructed (possibly by
	 * decryption, according to the privacy protocol in use) from the
	 * privData component of said SnmpPrivMsg value.
	 * 
	 * In particular, if the privacy protocol recorded for the party is
	 * noPriv, then the OCTET STRING value corresponds exactly to the
	 * privData component of the SnmpPrivMsg value.
	 */

	if ((auth_ptr->privData = ParseOctetString(&working_ptr, end_ptr, &type)) == NULL) {
	    DPRINTF((APTRACE, "ParseAuthentication, privData\n"));
	    FreeAuthentication(auth_ptr);
	    NULLIT(auth_ptr);
	    *error_code = ENCODING_ERROR;
	    return (NULL);
	}

#ifndef SR_UNSECURABLE

#ifndef SR_NO_PRIVACY
	/*
	 * Call decryption routine here as appropriate
	 */
#endif				/* SR_NO_PRIVACY */

#endif				/* SR_UNSECURABLE */

	/*
	 * From RFC1446, page 22-23, "Receiving a Message" (Privacy message):
	 * 
	 * This section describes the behavior of a SNMPv2 entity when it acts
	 * as a SNMPv2 party for which the privacy protocol is
	 * administratively specified as the Symmetric Privacy Protocol.
	 * Insofar as the behavior of a SNMPv2 entity when receiving a
	 * protocol message is defined generically in [1], only those aspects
	 * of that behavior that are specific to the Symmetric Privacy
	 * Protocol are described below.
	 * 
	 * According to Section 3.2 of [1], the privData component of a received
	 * SnmpPrivMsg value is evaluated during Step 4 of generic
	 * processing.  In particular, it states the privData component is
	 * evaluated according to the privacy protocol identified for the
	 * SNMPv2 party receiving the message.
	 */

	priv_prot = parties[auth_ptr->dstPartyIndex].party->partyPrivProtocol;
	auth_ptr->privType = priv_prot;

	switch (priv_prot) {
	  case ABSENT:
	    FreeAuthentication(auth_ptr);
	    NULLIT(auth_ptr);
	    return (NULL);
	    break;
	  case NOPRIV:
	    /* nothing to be done */
	    break;

#ifndef SR_UNSECURABLE

#ifndef SR_NO_PRIVACY
	    /*
	     * From RFC1446, page 23, section 4.2:
	     * 
	     * When the relevant privacy protocol is the Symmetric Privacy
	     * Protocol, the procedure performed by a SNMPv2 entity whenever
	     * a management communication is received by a SNMPv2 party is as
	     * follows.
	     * 
	     * (1) The local database is consulted to determine the private
	     * privacy key of the SNMPv2 party receiving the message
	     * (represented, for example, according to the conventions
	     * defined in Section 1.5.2).
	     * 
	     * (2) The contents octets of the privData component are decrypted
	     * using, for example, the algorithm specified in Section 1.5.2
	     * and the extracted private privacy key.
	     * 
	     * Processing of the received message continues as specified in [1].
	     * 
	     */
	  case DESPRIV:
	    memcpy((char *) key, (char *) parties[auth_ptr->dstPartyIndex].party->partyPrivPrivate->octet_ptr, 8);
	    memcpy((char *) iv,
		   (char *) &(parties[auth_ptr->dstPartyIndex].party->partyPrivPrivate->octet_ptr[8]), 8);

	    dessetkey(key);
	    /*
	     * The length of the privData should be a multiple of 8 because
	     * DES works on messages that are a multiple of 8
	     */
	    if ((auth_ptr->privData->length % 8) != 0) {
		DPRINTF((APTRACE, "ParseAuthentication: priv data length not\
 an integer multiple of 8.\n"));
		FreeAuthentication(auth_ptr);
		NULLIT(auth_ptr);
		*error_code = ENCODING_ERROR;
		return (NULL);
	    }

	    work = (char *) auth_ptr->privData->octet_ptr;
	    for (cnt = auth_ptr->privData->length; cnt > 0; cnt -= 8) {
		/*
		 * First, save incoming ciphertext for chain
		 */
		(void) memcpy(ivtmp, work, 8);
		dedes(work);
		/*
		 * Unchain block, save ciphertext for next
		 */
		cp = work;
		cp1 = iv;
		for (i = 8; i != 0; i--) {
		    *cp++ ^= *cp1++;
		}
		(void) memcpy(iv, ivtmp, 8);
		work += 8;
	    }
	    break;
#endif				/* SR_NO_PRIVACY */

#endif				/* SR_UNSECURABLE */

	  default:
	    DPRINTF((APTRACE, "ParseAuthentication, unknown privacy protocol\n"));
	    FreeAuthentication(auth_ptr);
	    NULLIT(auth_ptr);
	    *error_code = ENCODING_ERROR;
	    return (NULL);
	}

	working_ptr = (unsigned char *) auth_ptr->privData->octet_ptr;
	end_ptr = working_ptr + auth_ptr->privData->length;


	/*
	 * Parsing auth msg wrapper
	 */

	/*
	 * From RFC1445, page 19, Section 3.2:
	 * 
	 * (5)   If the OCTET STRING value is not the serialization (according
	 * to the conventions of [5]) of an SnmpAuthMsg value, then the
	 * received message is discarded without further processing, after
	 * the snmpStatsEncodingErrors counter [7] is incremented.
	 * 
	 */

	type = AUTH_MSG_TYPE;
	if ((seq_length = ParseImplicitSequence(&working_ptr, end_ptr, &type)) == -1) {
	    DPRINTF((APTRACE, "ParseAuthentication, parse auth message type sequence\n"));
	    FreeAuthentication(auth_ptr);
	    NULLIT(auth_ptr);
	    *error_code = ENCODING_ERROR;
	    return (NULL);
	}

	new_end_ptr = working_ptr + seq_length;
	if (new_end_ptr < end_ptr) {
	    end_ptr = MIN(end_ptr, new_end_ptr);
	}
	else if (new_end_ptr > end_ptr) {
	    DPRINTF((APTRACE, "ParseAuthentication, bad BER length:\n"));
	    FreeAuthentication(auth_ptr);
	    NULLIT(auth_ptr);
	    return (NULL);
	}

	switch (*working_ptr) {
	  case NO_AUTHINFO_TYPE:
	    auth_ptr->authType = NOAUTH_AUTHENTICATION;
	    type = NO_AUTHINFO_TYPE;
	    if ((dummy = ParseOctetString(&working_ptr, end_ptr, &type))
		== NULL) {
		DPRINTF((APTRACE, "ParseAuthentication, no authentication message type\n"));
		FreeAuthentication(auth_ptr);
		NULLIT(auth_ptr);
		*error_code = BAD_AUTHS_ERROR;
		return (NULL);
	    }

	    if (dummy->length != 0) {
		DPRINTF((APTRACE, "ParseAuthentication, non zero octetstring in noauthinfo message type\n"));
		FreeOctetString(dummy);
		FreeAuthentication(auth_ptr);
		NULLIT(auth_ptr);
		*error_code = BAD_AUTHS_ERROR;
		return (NULL);
	    }

	    FreeOctetString(dummy);
	    break;

#ifndef SR_UNSECURABLE
	  case SNMPv2_MD5_AUTHINFO_TYPE:

	    if (auth_ptr->domain != SNMPv2_DOMAIN) {
		DPRINTF((APTRACE, "ParseAuthentication:  received domain (SNMPv2) not equal to configured domain(%d)\n", auth_ptr->domain));
		FreeAuthentication(auth_ptr);
		NULLIT(auth_ptr);
		return (NULL);
	    }


	    auth_ptr->offset_to_auth_message = working_ptr - auth_ptr->privData->octet_ptr;
	    auth_ptr->bytecount = auth_ptr->offset_to_auth_message + end_ptr - working_ptr;

	    /*
	     * Start parsing MD5 authinfo type
	     */
	    type = SNMPv2_MD5_AUTHINFO_TYPE;
	    if ((seq_length = ParseImplicitSequence(&working_ptr, end_ptr, &type)) == -1) {
		DPRINTF((APTRACE, "ParseAuthentication, parse implicit auth message type sequence\n"));
		FreeAuthentication(auth_ptr);
		NULLIT(auth_ptr);
		*error_code = BAD_AUTHS_ERROR;
		return (NULL);
	    }

	    auth_ptr->offset_to_digest = working_ptr - auth_ptr->privData->octet_ptr + 2;

	    /*
	     * From RFC1446, page 18, receiving an authenticated message:
	     * 
	     * (1) If the ASN.1 type of the authInfo component is not
	     * AuthInformation, the message is evaluated as unauthentic, and
	     * the snmpStatsBadAuths counter [14] is incremented. Otherwise,
	     * the authSrcTimestamp, authDstTimestamp, and authDigest
	     * components are extracted from the SnmpAuthMsg value.
	     */

	    if ((auth_ptr->digest =
		 ParseOctetString(&working_ptr, end_ptr, &type)) == NULL) {

		DPRINTF((APTRACE, "ParseAuthentication, digest\n"));
		FreeAuthentication(auth_ptr);
		NULLIT(auth_ptr);
		*error_code = BAD_AUTHS_ERROR;
		return (NULL);
	    }

	    /*
	     * The digest size should be equal to MD5_DIGEST_SIZE.
	     */

	    if (auth_ptr->digest->length != MD5_DIGEST_SIZE) {
		DPRINTF((APTRACE, "ParseAuthentication: digest length wrong.\n"));
		FreeAuthentication(auth_ptr);
		NULLIT(auth_ptr);
		*error_code = BAD_AUTHS_ERROR;
		return (NULL);
	    }

	    auth_ptr->dst_timestamp = ParseUInt(&working_ptr, end_ptr, &type);
	    if (type == -1) {
		DPRINTF((APTRACE, "ParseAuthentication, dest timestamp\n"));
		FreeAuthentication(auth_ptr);
		NULLIT(auth_ptr);
		*error_code = BAD_AUTHS_ERROR;
		return (NULL);
	    }

	    auth_ptr->src_timestamp = ParseUInt(&working_ptr, end_ptr, &type);

	    if (type == -1) {
		DPRINTF((APTRACE, "ParseAuthentication, source timestamp\n"));
		FreeAuthentication(auth_ptr);
		NULLIT(auth_ptr);
		*error_code = BAD_AUTHS_ERROR;
		return (NULL);
	    }
	    break;
#endif				/* SR_UNSECURABLE */

	  default:
	    DPRINTF((APTRACE, "ParseAuthentication, unexpected auth message type %x\n",
		     *working_ptr));
	    FreeAuthentication(auth_ptr);
	    NULLIT(auth_ptr);
	    *error_code = ENCODING_ERROR;
	    return (NULL);
	    break;
	}			/* inner switch */


	type = MGMT_COM_TYPE;
	if ((seq_length = ParseImplicitSequence(&working_ptr, end_ptr, &type)) == -1) {
	    DPRINTF((APTRACE, "ParseAuthentication, parse implicit mgmt comm type sequence\n"));
	    FreeAuthentication(auth_ptr);
	    NULLIT(auth_ptr);
	    *error_code = ENCODING_ERROR;
	    return (NULL);
	}

	/*
	 * From RFC1445, page 19, Section 3.2:
	 * 
	 * (6) If the dstParty component of the authData component of the
	 * obtained SnmpAuthMsg value is not the same as the privDst
	 * component of the SnmpPrivMsg value, then the received message is
	 * discarded without further processing, after the
	 * snmpStatsDstPartyMismatches counter [7] is incremented.
	 */
	if ((auth_ptr->dstParty = ParseOID(&working_ptr, end_ptr)) == NULL) {
	    DPRINTF((APTRACE, "ParseAuthentication, dstParty\n"));
	    FreeAuthentication(auth_ptr);
	    NULLIT(auth_ptr);
	    *error_code = ENCODING_ERROR;
	    return (NULL);
	}

	if (CmpOID(auth_ptr->dstParty, auth_ptr->privDst) != 0) {
	    DPRINTF((APTRACE, "ParseAuthentication: dstParty/privDst mismatch.\n"));
	    FreeAuthentication(auth_ptr);
	    NULLIT(auth_ptr);
	    *error_code = DST_PARTY_MISMATCH_ERROR;
	    return (NULL);
	}

	/*
	 * From RFC1445, page 19, Section 3.2:
	 * 
	 * (7)  The local database of party information is consulted for
	 * information about the originating SNMPv2 party identified by the
	 * srcParty component of the authData component of the SnmpAuthMsg
	 * value.
	 * 
	 * (8) If information about the originating SNMPv2 party is absent from
	 * the local database of party information, then the received message
	 * is discarded without further processing, after the
	 * snmpStatsUnknownSrcParties counter [7] is incremented.
	 */

	if ((auth_ptr->srcParty = ParseOID(&working_ptr, end_ptr)) == NULL) {
	    DPRINTF((APTRACE, "ParseAuthentication, srcParty\n"));
	    FreeAuthentication(auth_ptr);
	    NULLIT(auth_ptr);
	    *error_code = ENCODING_ERROR;
	    return (NULL);
	}
	auth_ptr->srcPartyIndex = GetPartyIndex(auth_ptr->srcParty, SR_REMOTE);
	if (auth_ptr->srcPartyIndex == -1) {
	    DPRINTF((APTRACE,
	       "Received packet with unknown source party ... dropping\n"));
	    FreeAuthentication(auth_ptr);
	    NULLIT(auth_ptr);
	    *error_code = SRC_PARTY_ERROR;
	    return (NULL);
	}

	/*
	 * From RFC1445, pages 19-20, Section 3.2:
	 * 
	 * (9) The obtained SnmpAuthMsg value is evaluated according to the
	 * authentication protocol and other relevant information associated
	 * with the originating and receiving SNMPv2 parties in the local
	 * database of party information.
	 * 
	 * In particular, if the authentication protocol is identified as
	 * noAuth, then the SnmpAuthMsg value is always evaluated as
	 * authentic.
	 * 
	 * (10) If the SnmpAuthMsg value is evaluated as unauthentic, then the
	 * received message is discarded without further processing, and if
	 * the snmpV2EnableAuthenTraps object [7] is enabled, then the SNMPv2
	 * entity sends authorizationFailure traps [7] according to its
	 * configuration (Section 4.2.6 of[2]).
	 */

#ifndef SR_UNSECURABLE
	if (auth_ptr->authType == MD5_SNMPv2_AUTHENTICATION) {
	    /*
	     * Fill in the authPrivate field with the key the agent will need
	     * on a response packet.
	     */
	    if ((auth_ptr->authPrivate =
		 CloneOctetString(parties[auth_ptr->dstPartyIndex].party->partyAuthPrivate)) == NULL) {
		DPRINTF((APWARN,
		       "ParseAuthentication: cannot clone authPrivate.\n"));
		FreeAuthentication(auth_ptr);
		NULLIT(auth_ptr);
		return (NULL);
	    }

#ifndef SR_NO_PRIVACY
	    /*
	     * Fill in the privPrivate field with the key the agent will need
	     * on a response packet.
	     */
	    if (parties[auth_ptr->srcPartyIndex].party->partyPrivProtocol == DESPRIV) {
		if ((auth_ptr->privPrivate =
		     CloneOctetString(parties[auth_ptr->srcPartyIndex].party->partyPrivPrivate)) == NULL) {
		    DPRINTF((APWARN,
		       "ParseAuthentication: cannot clone privPrivate.\n"));
		    FreeAuthentication(auth_ptr);
		    NULLIT(auth_ptr);
		    return (NULL);
		}
	    }
#endif				/* SR_NO_PRIVACY */

	    if (CheckDigest(auth_ptr, (int) auth_ptr->srcPartyIndex,
		    (int) auth_ptr->dstPartyIndex, &return_code) == FALSE) {
		DPRINTF((APTRACE, "ParseAuthentication: Invalid message.\n"));

#ifndef TESTAGENT
		FreeAuthentication(auth_ptr);
		NULLIT(auth_ptr);
		*error_code = return_code;
		return (NULL);
#else				/* TESTAGENT */
		DPRINTF((APALWAYS, "Cooked version for marshall clock sync trouble.\n"));
#endif				/* TESTAGENT */
	    }
	}
#endif				/* SR_UNSECURABLE */

	/*
	 * From RFC1445, page 20, Section 3.2:
	 * 
	 * (11) The SnmpMgmtCom value is extracted from the authData component
	 * of the SnmpAuthMsg value.
	 * 
	 * (12) The local database of context information is consulted for
	 * information about the SNMPv2 context identified by the context
	 * component of the SnmpMgmtCom value.
	 * 
	 * (13) If information about the SNMPv2 context is absent from the local
	 * database of context information, then the received message is
	 * discarded without further processing, after the
	 * snmpStatsUnknownContexts counter [7] is incremented.
	 */

	if ((auth_ptr->context = ParseOID(&working_ptr, end_ptr)) == NULL) {
	    DPRINTF((APTRACE, "ParseAuthentication, context\n"));
	    FreeAuthentication(auth_ptr);
	    NULLIT(auth_ptr);
	    *error_code = ENCODING_ERROR;
	    return (NULL);
	}
	auth_ptr->contextIndex = GetContextIndex(auth_ptr->context);

	if (auth_ptr->contextIndex == -1) {
	    DPRINTF((APTRACE,
		     "Received packet with unknown context ... dropping\n"));
	    FreeAuthentication(auth_ptr);
	    NULLIT(auth_ptr);
	    *error_code = UNKNOWN_CONTEXT_ERROR;
	    return (NULL);
	}


	/* remember how big the wrapper is/was */
	auth_ptr->wrapper_size += working_ptr - auth_ptr->privData->octet_ptr;
	break;

      default:
	DPRINTF((APTRACE, "ParseAuthentication, first byte= 0x%x\n", *working_ptr));
        FreeAuthentication(auth_ptr);
        NULLIT(auth_ptr);

	return (NULL);
	break;
    }				/* outer switch old or new auth as indicated
				 * by 1st byte of msg */
    if (auth_ptr->authType != TRIVIAL_AUTHENTICATION) {
	auth_ptr->privType = parties[auth_ptr->srcPartyIndex].party->partyPrivProtocol;
    }
#endif				/* SR_SNMPv2 */


    /* copy what is left of data into auth_ptr->packlet */
    auth_ptr->packlet = MakeOctetString(working_ptr,
					(long) (end_ptr - working_ptr));

    return (auth_ptr);
}				/* end of ParseAuthentication() */

#ifndef SR_UNSECURABLE

#ifdef SR_SNMPv2
/*
 * CheckDigest: This routine checks to see if the incoming packet
 *              has a valid MD5 digest and the timestamp is within
 *              the correct time frame.
 *
 * Arguments: auth_ptr      - Contains the message and V2 header information.
 *            srcPartyIndex - index (for srcParty) of row in the party table.
 *            dstPartyIndex - index (for dstParty) of row in the party table.
 *            error_code    - will contain the proper error code if an error
 *                            occurs.
 *
 * Returns:  TRUE  - on success.
 *           FALSE - on failure.
 *
 * The former name (pre-snmp12.1.0.0 release) was check_digest().
 */
int
CheckDigest(auth_ptr, srcPartyIndex, dstPartyIndex, error_code)
    AuthHeader     *auth_ptr;
    int             srcPartyIndex;
    int             dstPartyIndex;
    int            *error_code;
{
    int             i;
    MD5_CTX         MD;
    unsigned char   digest[MD5_DIGEST_SIZE];
    unsigned long   now, dst_now, max, val, delta;
    unsigned long   dstTimestamp, srcTimestamp;

    *error_code = 0;
    dstTimestamp = parties[srcPartyIndex].party->partyAuthClock;
    srcTimestamp = parties[dstPartyIndex].party->partyAuthClock;

    /*
     * From: RFC1446 (Section 3.2, page 18)
     * 
     * (2) The local database is consulted to determine the authentication
     * clock, private authentication key (extracted, for example, according
     * to the conventions defined in Section 1.5.1), and lifetime of the
     * SNMPv2 party that originated the message.
     * 
     * (3) If the authSrcTimestamp component plus the lifetime is less than the
     * authentication clock, the message is evaluated as unauthentic, and the
     * snmpStatsNotInLifeTimes counter [14] is incremented.
     */

    /* Check for clock overflow */

    now = GetTimeNow() / 100;
    dst_now = now;

    if (basetime > (MAX_CLOCK_VALUE - now)) {
	now = MAX_CLOCK_VALUE;
	dst_now = now;
    }
    else {
	now = now + basetime;
	dst_now = now;
	if (parties[srcPartyIndex].party->partyAuthClock > (MAX_CLOCK_VALUE - now)) {
	    now = MAX_CLOCK_VALUE;
	}
	else {
	    now = now + parties[srcPartyIndex].party->partyAuthClock;
	}
	if (parties[dstPartyIndex].party->partyAuthClock > (MAX_CLOCK_VALUE - dst_now)) {
	    dst_now = MAX_CLOCK_VALUE;
	}
	else {
	    dst_now = dst_now + parties[dstPartyIndex].party->partyAuthClock;
	}
    }

    if (now == MAX_CLOCK_VALUE) {	/* Clock at max value */
	DPRINTF((APTRACE, "CheckDigest: Clock at maximum value.\n"));

	if (auth_ptr->src_timestamp < now) {
	    DPRINTF((APTRACE, "CheckDigest: Timestamp (%lu) < current time (%lu)\n",
		     auth_ptr->src_timestamp, now));
	    *error_code = NOT_IN_LIFETIME_ERROR;
	    return (FALSE);
	}
    }
    else {
	if (now <= parties[srcPartyIndex].party->partyAuthLifetime) {
	    /* always authentic */
	}
	else {
	    if (auth_ptr->src_timestamp < now -
		parties[srcPartyIndex].party->partyAuthLifetime) {
		DPRINTF((APTRACE, "CheckDigest: Timestamp (%lu) < current time (%lu)\n",
			 auth_ptr->src_timestamp, now));
		*error_code = NOT_IN_LIFETIME_ERROR;
		return (FALSE);
	    }
	}
    }

    /*
     * From: RFC1446 (Section 3.2, pages 18-19)
     * 
     * (4) The authDigest component is extracted and temporarily recorded.
     * 
     * (5) A new SnmpAuthMsg value is constructed such that its authDigest
     * component is set to the private authentication key and its other
     * components are set to the value of the corresponding components in the
     * received SnmpAuthMsg value.  This new SnmpAuthMsg value is serialized
     * according to the conventions of [13] and [12].  A digest is computed
     * over the octet sequence representing that serialized value using, for
     * example, the algorithm specified in Section 1.5.1.
     * 
     * NOTE Because serialization rules are unambiguous but may not be unique,
     * great care must be taken in reconstructing the serialized value prior
     * to computing the digest.  Implementations may find it useful to keep a
     * copy of the original serialized value and then simply modify the
     * octets which directly correspond to the placement of the authDigest
     * component, rather than re-applying the serialization algorithm to the
     * new SnmpAuthMsg value.
     * 
     */

    /*
     * Fill in digest with the private key
     */
    memcpy((char *) &auth_ptr->privData->octet_ptr[auth_ptr->offset_to_digest],
	 (char *) parties[srcPartyIndex].party->partyAuthPrivate->octet_ptr,
	   MD5_DIGEST_SIZE);

    /*
     * This new SnmpAuthMsg value is serialized according to the conventions
     * of [12] and [1]. -- its already serialized.
     */

    /*
     * An MD5 digest is computed over this octet sequence according to [3].
     */
    MD5Init(&MD);

    MD5Update(&MD, (unsigned char *) auth_ptr->privData->octet_ptr,
	      (unsigned) auth_ptr->bytecount);

    MD5Final(digest, &MD);

    /*
     * From: RFC1446 (Section 3.2, page 19)
     * 
     * (6) If the computed digest value is not equal to the digest value
     * temporarily recorded in step 4 above, the message is evaluated as
     * unauthentic, and the snmpStatsWrongDigestValues counter [14] is
     * incremented.
     */

    for (i = 0; i < MD5_DIGEST_SIZE; i++) {
/* printf("%lx %lx\n", digest[i], auth_ptr->digest->octet_ptr[i]); */
	if (digest[i] != auth_ptr->digest->octet_ptr[i]) {
	    DPRINTF((APTRACE, "key mismatch, we assume the message to be unauthentic\n"));
	    *error_code = WRONG_DIGEST_VALUES_ERROR;
	    return (FALSE);
	}
    }


    /*
     * From: RFC1446 (Section 3.2, page 19)
     * 
     * (7) The message is evaluated as authentic.
     * 
     * (8) The local database is consulted for access privileges permitted by
     * the local access policy to the originating SNMPv2 party with respect
     * to the receiving SNMPv2 party. If any level of access is permitted,
     * then:
     * 
     * the authentication clock value locally recorded for the originating
     * SNMPv2 party is advanced to the authSrcTimestamp value if this latter
     * exceeds the recorded value; and,
     * 
     * the authentication clock value locally recorded for the receiving SNMPv2
     * party is advanced to the authDstTimestamp value if this latter exceeds
     * the recorded value.
     * 
     * (Note that this step is conceptually independent from Steps 15-17 of
     * Section 3.2 in [1]).
     * 
     * If the SnmpAuthMsg value is evaluated as unauthentic, an authentication
     * failure is noted and the received message is discarded without further
     * processing.  Otherwise, processing of the received message continues
     * as specified in [1].
     */

    /*
     * If a partyAuthClock is accelerated, see if the clocks need to be
     * written to storage:
     * 
     * TIME_TO_WRITE = TIME_WARP - sysUpTime(delta value) - greatest clock
     * offset from previous write.
     */

    if (auth_ptr->authType == MD5_SNMPv2_AUTHENTICATION) {
	if (now < auth_ptr->src_timestamp) {
	    /*
	     * Just checking to see whether the srcParty and dstParty have an
	     * acl entry.  If they do, the clocks can be updated. The context
	     * is not checked because it has not been parsed yet.  It is not
	     * relevant in this test anyway.
	     */
	    if (GetAclEntry(parties[srcPartyIndex].party->partyIndex,
			  parties[dstPartyIndex].party->partyIndex) != -1) {
		if (auth_ptr->src_timestamp <= (MAX_CLOCK_VALUE -
			 parties[srcPartyIndex].party->partyAuthLifetime)) {
		    parties[srcPartyIndex].party->partyAuthClock =
			(auth_ptr->src_timestamp -
		      (now - parties[srcPartyIndex].party->partyAuthClock));

		    dstTimestamp = parties[srcPartyIndex].party->partyAuthClock;
		    max = FindMaxClockOffset();
		    delta = (GetTimeNow() / 100) - last_clock_write;
		    if (TIME_WARP > delta) {
			val = TIME_WARP - delta;
			if (val > max) {
			    /* no write necessary */
			}
			else {
			    (void) WriteParties();
			}
		    }
		    else {
			(void) WriteParties();
		    }
		}
	    }
	}

	if (dst_now < auth_ptr->dst_timestamp) {
	    /*
	     * Just checking to see whether the srcParty and dstParty have an
	     * acl entry.  If they do, the clocks can be updated. The context
	     * is not checked because it has not been parsed yet.  It is not
	     * relevant in this test anyway.
	     */
	    if (GetAclEntry(parties[srcPartyIndex].party->partyIndex,
			  parties[dstPartyIndex].party->partyIndex) != -1) {
		if (auth_ptr->dst_timestamp <= (MAX_CLOCK_VALUE -
			 parties[dstPartyIndex].party->partyAuthLifetime)) {
		    parties[dstPartyIndex].party->partyAuthClock =
			(auth_ptr->dst_timestamp -
			 (dst_now - parties[dstPartyIndex].party->partyAuthClock));

		    srcTimestamp = parties[dstPartyIndex].party->partyAuthClock;
		    max = FindMaxClockOffset();
		    delta = (GetTimeNow() / 100) - last_clock_write;
		    if (TIME_WARP > delta) {
			val = TIME_WARP - delta;
			if (val > max) {
			    /* no write necessary */
			}
			else {
			    WriteParties();
			}
		    }
		    else {
			WriteParties();
		    }
		}		/* if (auth_ptr->dst_timestamp ...) */
	    }			/* if (GetAclEntry(....) */
	}			/* if (dst_now < ...) */
    }				/* if (authType == MD5_SNMPv2_AUTHENTICATION) */

    auth_ptr->dst_timestamp = dstTimestamp;
    auth_ptr->src_timestamp = srcTimestamp;
    return (TRUE);
}				/* CheckDigest() */

/*
 * MakeDigest: This routine takes the message contained in
 *             auth_ptr->privData->octet_ptr (the message), computes an MD5
 *             digest over it, and places that value in the proper place
 *             in the message (auth_ptr->PrivData->octet_ptr).
 *
 * Arguments:  auth_ptr - contains message and authentication information.
 *
 * returns:    TRUE  - on success.
 *             FALSE - on failure.
 *
 * The former name (pre-snmp12.1.0.0 release) was make_digest().
 */
int
MakeDigest(auth_ptr)
    AuthHeader     *auth_ptr;
{
    int             i;
    MD5_CTX         MD;
    unsigned char   digest[MD5_DIGEST_SIZE];


    MD5Init(&MD);
    MD5Update(&MD, (unsigned char *) auth_ptr->privData->octet_ptr,
	      (unsigned) auth_ptr->bytecount);
    MD5Final(digest, &MD);

    /* Then fill in the digest with the computed digest */

    for (i = 0; i < MD5_DIGEST_SIZE; i++) {
	auth_ptr->privData->octet_ptr[i + auth_ptr->offset_to_digest] = digest[i];
    }

    return (TRUE);
}				/* MakeDigest() */

/*
 * FindMaxClockOffset: This routine finds the biggest difference between
 *                     the party clock values at the last write to
 *                     storage (clockVal value in parties array) and the
 *                     current party clock values (partyAuthClock) and returns
 *                     that value.
 *
 * Arguments: none
 *
 * Returns: Largest difference between current party clock values and the
 *          party clock values on the last write to storage.
 *
 *   The former name (pre-snmp12.1.0.0 release) was
 *   find_max_clock_offset().
 */
unsigned long
FindMaxClockOffset()
{

    int             i;
    unsigned long   max;

    max = 0;

    for (i = 0; i < num_parties; i++) {
	if (parties[i].party == NULL) {
	    continue;
	}
	if (parties[i].party->partyStatus == ACTIVE) {
	    if ((parties[i].party->partyAuthClock > parties[i].clockVal) &&
		((parties[i].party->partyAuthClock - parties[i].clockVal) > max)) {
		max = parties[i].party->partyAuthClock - parties[i].clockVal;
	    }
	}
    }

    return (max);

}				/* FindMaxClockOffset() */

#endif				/* SR_SNMPv2 */

#endif				/* SR_UNSECURABLE */
