/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_prse_pkt.c,v 3.2 1995/11/17 19:00:44 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/snmp/sr_prse_pkt.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, SNMP Research
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_prse_pkt.c,v $
 * Revision 3.2  1995/11/17  19:00:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:17:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:50:17  hampton
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
static char     rcsid[] = "prse_pkt.c,v 1.14 1995/02/06 18:11:20 battle Exp";
#endif				/* (! ( defined(lint) ) && defined(SR_RCSID)) */

/*
 * Revision History:
 *
 * prse_pkt.c,v
 * Revision 1.14  1995/02/06  18:11:20  battle
 * Changes for vrtx.
 *
 * Revision 1.13  1994/10/28  16:37:58  vaughn
 * Placed new copyright in the file
 *
 * Revision 1.12  1994/10/26  21:52:00  vaughn
 * Changed copyrights to the new copyright structure
 *
 * Revision 1.11  1994/09/30  16:10:55  battle
 * hprt changes
 *
 * Revision 1.10  1994/08/08  18:01:42  battle
 * moved inet_ntoa to a file where it will be in both v1 and v2 for pSOS
 *
 * Revision 1.9  1994/08/04  15:37:32  battle
 * pSOS 2.0 changes
 *
 * Revision 1.8  1994/04/18  18:28:42  battle
 * pSOS changes
 *
 * Revision 1.7  1994/02/25  14:58:12  battle
 * Removed an errant = in ParseOID().
 *
 * Revision 1.6  1994/02/14  16:39:19  pearson
 * Changed copyright notice to be current for 1994, and added an
 * #ifdef SR_RCSID
 * around the static  declarations of rcsid in each file.
 *
 * AMP
 *
 * Revision 1.5  1994/02/14  15:44:34  battle
 * Figured out the ParseOID bug.  Basicly ParseOID assumed that any oid in an
 * incoming packet is at least 3 object identifier components long!  We also
 * assumed that the more bit is not set in the first subidentifier.  I put in
 * a check to return early for 2 component (one octet) oids.  I'd really like
 * to just re-write ParseOID altogether.
 *
 * Revision 1.4  1993/12/23  16:54:06  battle
 * Added cvs headers to all .c files.
 *
 *
 *  2/6/89 JDC
 *  Amended copyright notice
 *  Professionalized error messages a bit
 *  Expanded comments and documentation
 *  Plugged a very small memory leak under error conditions
 *
 *  8/3/89 JDC
 *  clean up some lint nits
 *  fix diagnostic messages some
 *
 *  8/8/89 JDC
 *  calloc --> malloc for compat with MSC on PC
 *
 *  11/8/89 JDC
 *  free vb_ptr before error exit in ParseVarBind
 *  set pointer to null after freeing it to be bullet proof in ParseOID
 *
 *  11/8/89 JDC
 *  Make it print pretty via tgrind
 *
 *  01/08/90 JDC
 *  Added SILENT
 *
 *  01-Jan-91 JDC
 *  Changed SILENT to  LIB_ERROR and LIB_PERROR
 *
 *  01-Jan-91 JDC
 *  Substituted NULLIT(pointer) for LIGHT
 *
 *  18-Feb-91 JDC
 *  Ditched LIB_PERROR
 *
 *  24-Feb-92 DSM
 *  Checks, if the length is zero
 *
 *  13-June-92 JDC
 *  SMP
 *
 *  8-Dec-1992 DLB
 *    Lint nits.
 *
 *  6-Jan-1993 MGH
 *  Added the U_INTEGER_32_TYPE to switch statements in ParseVarBind() and
 *  ParseType().
 *
 *  5-May-1993 MGH
 *  Changed all LIB_ERROR* messages to DPRINTF messages.
 */

#include "master.h"


#include "sr_snmp.h"
#include "sr_diag.h"

/*
 * ParsePdu:
 *
 *   This routine takes a PDU from fully populated Authheader
 *   structure and parses the information into the library's internal
 *   PDU format, including all varbind instances.  This routine is
 *   usually called with the authentication header pointer returned by
 *   ParseAuthentication, which is the same state as the header
 *   pointer after BuildAuthentication is called.
 *
 *   The PDU pointer returned from this call is the same state as the
 *   PDU pointer on a building phase after BuildPdu() has been called.
 *   If this routine fails, it returns a NULL.
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_pdu().
 */

Pdu            *
ParsePdu(auth_ptr)
    AuthHeader     *auth_ptr;
{
    unsigned char  *working_ptr, *end_ptr;
    Pdu            *pdu_ptr;
    VarBind        *vb_ptr;
    short           type, pdu_type;
    long            length;

    if ((auth_ptr->packlet == NULL) || (auth_ptr->packlet->octet_ptr == NULL) ||
	(auth_ptr->packlet->length == 0)) {
	DPRINTF((APTRACE, "ParsePdu, bad packlet in auth_ptr->packlet:\n"));
	return (NULL);
    }

    working_ptr = auth_ptr->packlet->octet_ptr;
    end_ptr = working_ptr + auth_ptr->packlet->length;

    if ((pdu_ptr = (Pdu *) malloc(sizeof(Pdu))) == NULL) {
	DPRINTF((APWARN, "ParsePdu, pdu_ptr malloc\n"));
	return (NULL);
    }
    pdu_ptr->packlet = NULL;
    pdu_ptr->num_varbinds = 0L;

#ifdef	SR_SNMPv1
    pdu_ptr->u.trappdu.enterprise = NULL;
    pdu_ptr->u.trappdu.agent_addr = NULL;
#endif				/* SR_SNMPv1 */

    pdu_ptr->var_bind_list = NULL;
    pdu_ptr->var_bind_end_ptr = NULL;

    if ((pdu_type = ParseType(&working_ptr, end_ptr)) == -1) {
	DPRINTF((APTRACE, "ParsePdu, pdu_type\n"));
	FreePdu(pdu_ptr);
	NULLIT(pdu_ptr);
	return (NULL);
    }

    pdu_ptr->type = pdu_type;

    if ((length = ParseLength(&working_ptr, end_ptr)) == -1) {
	DPRINTF((APTRACE, "ParsePdu, length\n"));
	FreePdu(pdu_ptr);
	NULLIT(pdu_ptr);
	return (NULL);
    }

    if (working_ptr + length > end_ptr) {
	DPRINTF((APTRACE, "ParsePdu, bad length:\n"));
	FreePdu(pdu_ptr);
	NULLIT(pdu_ptr);
	return (NULL);
    }

    switch ((int) pdu_type) {
      case GET_REQUEST_TYPE:
      case GET_NEXT_REQUEST_TYPE:
      case GET_RESPONSE_TYPE:
      case SET_REQUEST_TYPE:

#ifdef	SR_SNMPv2
      case GET_BULK_REQUEST_TYPE:
      case INFORM_REQUEST_TYPE:
      case SNMPv2_TRAP_TYPE:
#endif				/* SR_SNMPv2 */

	pdu_ptr->u.normpdu.request_id =
	    ParseInt(&working_ptr, end_ptr, &type);
	if (type == -1) {
	    DPRINTF((APTRACE, "ParsePdu, request_id\n"));
	    FreePdu(pdu_ptr);
	    NULLIT(pdu_ptr);
	    return (NULL);
	}
	pdu_ptr->u.normpdu.error_status =
	    ParseInt(&working_ptr, end_ptr, &type);
	if (type == -1) {
	    DPRINTF((APTRACE, "ParsePdu, error_status\n"));
	    FreePdu(pdu_ptr);
	    NULLIT(pdu_ptr);
	    return (NULL);
	}
	pdu_ptr->u.normpdu.error_index =
	    ParseInt(&working_ptr, end_ptr, &type);
	if (type == -1) {
	    DPRINTF((APTRACE, "ParsePdu, error_index\n"));
	    FreePdu(pdu_ptr);
	    NULLIT(pdu_ptr);
	    return (NULL);
	}
	break;

#ifdef	SR_SNMPv1
      case TRAP_TYPE:
	if ((pdu_ptr->u.trappdu.enterprise =
	     ParseOID(&working_ptr, end_ptr)) == NULL) {
	    DPRINTF((APTRACE, "ParsePdu, enterprise\n"));
	    FreePdu(pdu_ptr);
	    NULLIT(pdu_ptr);
	    return (NULL);
	}
	if ((pdu_ptr->u.trappdu.agent_addr =
	     ParseOctetString(&working_ptr, end_ptr, &type)) == NULL) {
	    DPRINTF((APTRACE, "ParsePdu, agent_addr\n"));
	    FreePdu(pdu_ptr);
	    NULLIT(pdu_ptr);
	    return (NULL);
	}
	pdu_ptr->u.trappdu.generic_trap =
	    ParseInt(&working_ptr, end_ptr, &type);
	if (type == -1) {
	    DPRINTF((APTRACE, "ParsePdu, generic_trap\n"));
	    FreePdu(pdu_ptr);
	    NULLIT(pdu_ptr);
	    return (NULL);
	}
	pdu_ptr->u.trappdu.specific_trap =
	    ParseInt(&working_ptr, end_ptr, &type);
	if (type == -1) {
	    DPRINTF((APTRACE, "ParsePdu, specific_trap\n"));
	    FreePdu(pdu_ptr);
	    NULLIT(pdu_ptr);
	    return (NULL);
	}
	pdu_ptr->u.trappdu.time_ticks =
	    ParseUInt(&working_ptr, end_ptr, &type);
	if (type == -1) {
	    DPRINTF((APTRACE, "ParsePdu, time-ticks\n"));
	    FreePdu(pdu_ptr);
	    NULLIT(pdu_ptr);
	    return (NULL);
	}
	break;
#endif				/* SR_SNMPv1 */

      default:
	DPRINTF((APTRACE, "ParsePdu, bad pdu_type: %x\n", pdu_type));
	FreePdu(pdu_ptr);
	NULLIT(pdu_ptr);
	return (NULL);
	break;
    };				/* end of switch */

    /* now strip out the sequence of */
    if (ParseSequence(&working_ptr, end_ptr, &type) == -1) {
	DPRINTF((APTRACE, "ParsePdu, ParseSequence failure\n"));
	FreePdu(pdu_ptr);
	NULLIT(pdu_ptr);
	return (NULL);
    }

    /* now parse the varbind list */
    while (working_ptr < end_ptr) {
	if ((vb_ptr = ParseVarBind(&working_ptr, end_ptr)) == NULL) {
	    DPRINTF((APTRACE, "ParsePdu, vb_ptr\n"));
	    FreePdu(pdu_ptr);
	    NULLIT(pdu_ptr);
	    return (NULL);
	}
	pdu_ptr->num_varbinds++;

	/* is this first one? */
	if (pdu_ptr->var_bind_list == NULL) {	/* start list */
	    pdu_ptr->var_bind_list = vb_ptr;
	    pdu_ptr->var_bind_end_ptr = vb_ptr;
	}
	else {			/* tack onto end of list */
	    pdu_ptr->var_bind_end_ptr->next_var = vb_ptr;
	    pdu_ptr->var_bind_end_ptr = vb_ptr;
	}

	/* DO NOT FREE vb_ptr.  Just handed it off to pdu structure */
	vb_ptr = NULL;
    };				/* end of while */

    return (pdu_ptr);
}				/* ParsePdu() */

/*
 * ParseVarBind:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_varbind().
 */
VarBind        *
ParseVarBind(working_ptr, end_ptr)
    unsigned char **working_ptr;
    unsigned char  *end_ptr;
{
    VarBind        *vb_ptr;
    short           type;
    unsigned char  *temp_ptr;

    if (ParseSequence(working_ptr, end_ptr, &type) == -1) {
	DPRINTF((APTRACE, "ParseVarBind, ParseSequence failure\n"));
	return (NULL);
    }

    if ((vb_ptr = (VarBind *) malloc(sizeof(VarBind))) == NULL) {
	DPRINTF((APWARN, "ParseVarBind, vb_ptr\n"));
	return (NULL);
    }
    vb_ptr->name = NULL;
    vb_ptr->value.os_value = NULL;
    vb_ptr->value.oid_value = NULL;

#ifdef SR_SNMPv2
    vb_ptr->value.uint64_value = NULL;
    vb_ptr->value.bitstring_value = NULL;
#endif				/* SR_SNMPv2 */

    vb_ptr->next_var = NULL;

    temp_ptr = *working_ptr;

    if ((vb_ptr->name = ParseOID(working_ptr, end_ptr)) == NULL) {
	DPRINTF((APTRACE, "ParseVarBind, vb_ptr->name\n"));
	FreeVarBind(vb_ptr);
	NULLIT(vb_ptr);
	return (NULL);
    }

    vb_ptr->name_size = *working_ptr - temp_ptr;

    temp_ptr = *working_ptr;

    switch (*(*working_ptr)) {
      case COUNTER_TYPE:	/* handle unsigned integers including
				 * COUNTER_32 */
      case GAUGE_TYPE:		/* includes GAUGE_32 */
      case TIME_TICKS_TYPE:

#ifdef SR_SNMPv2
      case U_INTEGER_32_TYPE:
#endif				/* SR_SNMPv2 */

	vb_ptr->value.ul_value =
	    ParseUInt(working_ptr, end_ptr, &vb_ptr->value.type);
	if (vb_ptr->value.type == -1) {
	    DPRINTF((APTRACE, "ParseVarBind, vb_ptr->value.ul_value\n"));
	    FreeVarBind(vb_ptr);
	    NULLIT(vb_ptr);
	    return (NULL);
	}
	break;
      case INTEGER_TYPE:	/* handle signed integers including
				 * INTEGER_32 */
	vb_ptr->value.sl_value =
	    ParseInt(working_ptr, end_ptr, &vb_ptr->value.type);
	if (vb_ptr->value.type == -1) {
	    DPRINTF((APTRACE, "ParseVarBind, vb_ptr->value.sl_value\n"));
	    FreeVarBind(vb_ptr);
	    NULLIT(vb_ptr);
	    return (NULL);
	}
	break;
      case OBJECT_ID_TYPE:	/* handle quasi-octet strings */
	vb_ptr->value.type = OBJECT_ID_TYPE;
	if ((vb_ptr->value.oid_value = ParseOID(working_ptr, end_ptr)) == NULL) {
	    DPRINTF((APTRACE, "ParseVarBind, vb_ptr->value.oid_value\n"));
	    FreeVarBind(vb_ptr);
	    NULLIT(vb_ptr);
	    return (NULL);
	}
	break;

#ifdef	SR_SNMPv2
      case COUNTER_64_TYPE:	/* handle large unsigned integers */
	vb_ptr->value.type = COUNTER_64_TYPE;
	if ((vb_ptr->value.uint64_value = ParseUInt64(working_ptr, end_ptr)) == NULL) {
	    DPRINTF((APTRACE, "ParseVarBind, vb_ptr->value.uint64_value\n"));
	    FreeVarBind(vb_ptr);
	    NULLIT(vb_ptr);
	    return (NULL);
	}
	break;
#endif				/* SR_SNMPv2 */

      case OCTET_PRIM_TYPE:
      case IP_ADDR_PRIM_TYPE:
      case OPAQUE_PRIM_TYPE:

#ifdef	SR_SNMPv2
      case NSAP_ADDR_TYPE:
#endif				/* SR_SNMPv2 */

	if ((vb_ptr->value.os_value =
	     ParseOctetString(working_ptr, end_ptr, &vb_ptr->value.type)) == NULL) {
	    DPRINTF((APTRACE, "ParseVarBind, vb_ptr->value.os_value\n"));
	    FreeVarBind(vb_ptr);
	    NULLIT(vb_ptr);
	    return (NULL);
	}
	break;

#ifdef	SR_SNMPv2
      case BIT_STRING_TYPE:
	if ((vb_ptr->value.bitstring_value =
	ParseBitString(working_ptr, end_ptr, &vb_ptr->value.type)) == NULL) {
	    DPRINTF((APTRACE, "ParseVarBind, vb_ptr->value.bitstring_value\n"));
	    FreeVarBind(vb_ptr);
	    NULLIT(vb_ptr);
	    return (NULL);
	}
	break;
#endif				/* SR_SNMPv2 */

      case NULL_TYPE:

#ifdef	SR_SNMPv2
      case NO_SUCH_OBJECT_EXCEPTION:
      case NO_SUCH_INSTANCE_EXCEPTION:
      case END_OF_MIB_VIEW_EXCEPTION:
#endif				/* SR_SNMPv2 */

	if (ParseNull(working_ptr, end_ptr, &vb_ptr->value.type) == -1) {
	    DPRINTF((APTRACE, "ParseVarBind, ParseNull\n"));
	    FreeVarBind(vb_ptr);
	    NULLIT(vb_ptr);
	    return (NULL);
	}
	break;
      default:
	DPRINTF((APTRACE, "ParseVarBind, value: Illegal type: 0x%x\n",
		 vb_ptr->value.type));
	FreeVarBind(vb_ptr);
	NULLIT(vb_ptr);
	return (NULL);
	break;
    };				/* end of switch */

    vb_ptr->value_size = *working_ptr - temp_ptr;

    if (*working_ptr > end_ptr) {
	DPRINTF((APWARN, "ParseVarBind, past end of packet.\n"));
	return (NULL);
    }

    return (vb_ptr);
}				/* ParseVarBind() */

/*
 * ParseOctetString:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_octet_string().
 */
OctetString    *
ParseOctetString(working_ptr, end_ptr, type)
    unsigned char **working_ptr;
    unsigned char  *end_ptr;
    short          *type;
{
    OctetString    *os_ptr;
    long            length;
    long            i;

    if ((*type = ParseType(working_ptr, end_ptr)) == -1) {
	DPRINTF((APTRACE, "ParseOctetString, type error.\n"));
	return (NULL);
    }

    if ((length = ParseLength(working_ptr, end_ptr)) == -1) {
	DPRINTF((APTRACE, "ParseOctetString, length error.\n"));
	return (NULL);
    }

    if ((os_ptr = MakeOctetString(*working_ptr, length)) == NULL) {
	DPRINTF((APTRACE, "ParseOctetString, os_ptr\n"));
	*type = -1;
	return (NULL);
    }

    os_ptr->length = length;

    /*
     * call to MakeOctetString just copied - did not advance working_ptr.
     * Earlier versions of MSC did not like += and did some weird castings of
     * types (at least that is what codeview showed).
     */
    for (i = 0; i < length; i++) {
	(*working_ptr)++;
    }

    if (*working_ptr > end_ptr) {
	DPRINTF((APWARN, "ParseOctetString, past end of packet.\n"));
	*type = -1;
	return (NULL);
    }

    return (os_ptr);
}				/* end of ParseOctetString() */

/*
 * ParseOID:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_oid().
 */
OID            *
ParseOID(working_ptr, end_ptr)
    unsigned char **working_ptr;
    unsigned char  *end_ptr;
{
    OID            *oid_ptr;
    long            i;
    long            length;
    short           type;
    register int    oidlen, count;
    unsigned char   first;

    if ((type = ParseType(working_ptr, end_ptr)) == -1) {
	DPRINTF((APERROR, "ParseOID, type error.\n"));
	return (NULL);
    }

    if (type != OBJECT_ID_TYPE) {
	DPRINTF((APERROR, "ParseOID, type %x not OBJECT_ID_TYPE\n", type));
	return (NULL);
    }

    if ((length = ParseLength(working_ptr, end_ptr)) < 1) {
	DPRINTF((APERROR, "ParseOID, length error.\n"));
	return (NULL);
    }

    if ((*(*working_ptr) & 0x80) != 0) {
	DPRINTF((APERROR, "ParseOID, more bit set in first subidentifier.\n"));
	return (NULL);
    }

    if ((oid_ptr = (OID *) malloc(sizeof(OID))) == NULL) {
	DPRINTF((APWARN, "ParseOID, oid_ptr malloc\n"));
	return (NULL);
    }

    oid_ptr->oid_ptr = NULL;

    /*
     * we can be assured that this will at LEAST be long enough (and saves
     * stack space in machines with small stacks).
     */
    if ((oid_ptr->oid_ptr =
	 (unsigned long *) malloc((unsigned) ((length + 2) * sizeof(unsigned long)))) == NULL) {
	DPRINTF((APWARN, "ParseOID, oid_ptr malloc\n"));
	FreeOID(oid_ptr);
	NULLIT(oid_ptr);
	return (NULL);
    }

    oid_ptr->oid_ptr[0] = (unsigned long) (*(*working_ptr) / 40);
    oid_ptr->oid_ptr[1] = (unsigned long) (*(*working_ptr)++
					   - (oid_ptr->oid_ptr[0] * 40));
    oidlen = 2;

    if (length == 1) {
	oid_ptr->length = oidlen;
	return oid_ptr;
    }

    if (*working_ptr >= end_ptr) {
	DPRINTF((APWARN, "ParseOID, past end of packet.\n"));
	FreeOID(oid_ptr);
	NULLIT(oid_ptr);
	return (NULL);
    }

    oid_ptr->oid_ptr[2] = 0;
    count = 0;
    first = *(*working_ptr);
    for (i = 0; i < length - 1; i++) {

	oid_ptr->oid_ptr[oidlen] =
	    (oid_ptr->oid_ptr[oidlen] << 7) + (*(*working_ptr) & 0x7F);

	if ((*(*working_ptr)++ & 0x80) == 0) {	/* is last octet */
	    count = 0;
	    if (*working_ptr < end_ptr) {
		first = *(*working_ptr);
	    }
	    oidlen++;
	    if (i < length - 2) {	/* -1 - i++ (from the loop) */
		oid_ptr->oid_ptr[oidlen] = 0;
	    }
	}
	else {
	    count++;
	    if (count == 5) {
		if (first & 0x70) {
		    DPRINTF((APWARN, "ParseOID, sid value too big.\n"));
		    FreeOID(oid_ptr);
		    NULLIT(oid_ptr);
		    return (NULL);
		}
	    }
	    if (count > 5) {
		DPRINTF((APWARN, "ParseOID, sid value too big.\n"));
		FreeOID(oid_ptr);
		NULLIT(oid_ptr);
		return (NULL);
	    }
	}

	if (*working_ptr > end_ptr) {
	    DPRINTF((APWARN, "ParseOID, past end of packet.\n"));
	    FreeOID(oid_ptr);
	    NULLIT(oid_ptr);
	    return (NULL);
	}
    }
    if (oidlen > MAX_OID_SIZE) {
	DPRINTF((APTRACE, "ParseOID: Bad sid string length: %d\n", oidlen));
	FreeOID(oid_ptr);
	NULLIT(oid_ptr);
	return (NULL);
    }
    oid_ptr->length = oidlen;

    return (oid_ptr);
}				/* end of ParseOID() */

#ifdef	SR_SNMPv2
/*
 * ParseUInt64:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_uint64().
 */
UInt64         *
ParseUInt64(working_ptr, end_ptr)
    unsigned char **working_ptr;
    unsigned char  *end_ptr;
{
    UInt64         *uint64_ptr;
    long            i;
    long            length;
    short           type;

    if ((type = ParseType(working_ptr, end_ptr)) == -1) {
	DPRINTF((APTRACE, "ParseUInt64, type error.\n"));
	return (NULL);
    }

    if (type != COUNTER_64_TYPE) {
	DPRINTF((APTRACE, "ParseUInt64, type %x not COUNTER_64_TYPE\n", type));
	return (NULL);
    }

    if ((length = ParseLength(working_ptr, end_ptr)) == -1) {
	DPRINTF((APTRACE, "ParseUInt64, length error.\n"));
	return (NULL);
    }

    /* check length */
    if ((length > 9) || ((length > 8) && (*(*working_ptr) != 0x00))) {
	DPRINTF((APTRACE, "ParseUInt64, length error: %ld\n", length));
	return (NULL);
    }

    if ((uint64_ptr = (UInt64 *) malloc(sizeof(UInt64))) == NULL) {
	DPRINTF((APWARN, "ParseUInt64, uint64_ptr malloc\n"));
	return (NULL);
    }

    /* do we have that funky leading 0 octet because first bit is 1? */
    if (*(*working_ptr) == 0x00) {	/* if so, skip */
	(*working_ptr)++;
	length--;		/* and adjust length accordingly */
    }

    uint64_ptr->big_end = 0L;
    uint64_ptr->little_end = 0L;

    for (i = 0; i < length; i++) {
	uint64_ptr->big_end = (uint64_ptr->big_end << 8) +
	    (uint64_ptr->little_end >> 24);
	uint64_ptr->little_end = (uint64_ptr->little_end << 8) +
	    (unsigned long) *(*working_ptr)++;
    }

    if (*working_ptr > end_ptr) {
	DPRINTF((APWARN, "ParseUInt64, past end of packet.\n"));
	return (0);
    }

    return (uint64_ptr);
}				/* end of ParseUInt64() */

/*
 * ParseBitString:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_bitstring().
 */
BitString      *
ParseBitString(working_ptr, end_ptr, type)
    unsigned char **working_ptr;
    unsigned char  *end_ptr;
    short          *type;
{
    BitString      *bitstring_ptr;
    long            length;
    long            i;
    short           unused;

    if ((*type = ParseType(working_ptr, end_ptr)) == -1) {
	DPRINTF((APTRACE, "ParseBitString, type error.\n"));
	return (NULL);
    }

    if ((length = ParseLength(working_ptr, end_ptr)) == -1) {
	DPRINTF((APTRACE, "ParseBitString, length error.\n"));
	return (NULL);
    }

    if (length < 1) {
	DPRINTF((APTRACE, "ParseBitString, length range error.\n"));
	return (NULL);
    }
    length = length - 1;

    /* use the same routine to pull out the unused count */
    if ((unused = ParseLength(working_ptr, end_ptr)) == -1) {
	DPRINTF((APTRACE, "ParseBitString, unused count range error.\n"));
	return (NULL);
    }

    if ((length == 0) && (unused != 0)) {
	DPRINTF((APTRACE, "ParseBitString, unused count value error.\n"));
	return (NULL);
    }

    if ((bitstring_ptr = MakeBitString(*working_ptr, length, unused)) == NULL) {
	DPRINTF((APWARN, "ParseBitString, bitstring_ptr\n"));
	*type = -1;
	return (NULL);
    }

    /*
     * call to MakeBitString just copied - did not advance working_ptr.
     * Earlier versions of MSC did not like += and did some weird castings of
     * types (at least that is what codeview showed).
     */

    for (i = 0; i < length; i++) {
	(*working_ptr)++;
    }

    if (*working_ptr > end_ptr) {
	DPRINTF((APWARN, "ParseBitString, past end of packet.\n"));
	*type = -1;
	return (NULL);
    }

    return (bitstring_ptr);
}				/* end of ParseBitString() */

#endif				/* SR_SNMPv2 */

/*
 * ParseUInt:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_unsignedinteger().
 */
unsigned long
ParseUInt(working_ptr, end_ptr, type)
    unsigned char **working_ptr;
    unsigned char  *end_ptr;
    short          *type;
{
    unsigned long   value;
    long            length;
    long            i;

    if ((*type = ParseType(working_ptr, end_ptr)) == -1) {
	DPRINTF((APTRACE, "ParseUInt, type error.\n"));
	return (0);
    }

    if ((length = ParseLength(working_ptr, end_ptr)) == -1) {
	DPRINTF((APTRACE, "ParseUInt, length error.\n"));
	return (0);
    }

    /* check length */
    if ((length > 5) || ((length > 4) && (*(*working_ptr) != 0x00))) {
	DPRINTF((APTRACE, "ParseUInt, length error: %ld\n", length));
	*type = -1;		/* signal error */
	return (0);
    }

    /* do we have that funky leading 0 octet because first bit is 1? */
    if (*(*working_ptr) == 0x00) {	/* if so, skip */
	(*working_ptr)++;
	length--;		/* and adjust length accordingly */
    }

    value = 0L;
    for (i = 0; i < length; i++) {
	value = (value << 8) + (unsigned long) *(*working_ptr)++;
    }

    if (*working_ptr > end_ptr) {
	DPRINTF((APWARN, "ParseUInt, past end of packet.\n"));
	*type = -1;
	return (0);
    }

    return (value);
}				/* end of ParseUInt() */

/*
 * ParseInt:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_signedinteger().
 */
long
ParseInt(working_ptr, end_ptr, type)
    unsigned char **working_ptr;
    unsigned char  *end_ptr;
    short          *type;
{
    long            value;
    long            length;
    long            i;
    long            sign;

    value = 0L;

    if ((*type = ParseType(working_ptr, end_ptr)) == -1) {
	DPRINTF((APTRACE, "parsed_signedinteger, type error.\n"));
	return (0);
    }

    if ((length = ParseLength(working_ptr, end_ptr)) == -1) {
	DPRINTF((APTRACE, "ParseInt, length error.\n"));
	return (0);
    }

    if (length > 4) {
	DPRINTF((APTRACE, "parsed_signedinteger, length error: %ld\n", length));
	*type = -1;
	return (0);
    }

    sign = ((*(*working_ptr) & 0x80) == 0x00) ? 0x00 : 0xff;

    for (i = 0; i < length; i++) {
	value = (value << 8) + (unsigned long) *(*working_ptr)++;
    }

    /* now fill in the upper bits with the appropriate sign extension. */
    for (i = length; i < 4; i++) {
	value = value + (sign << i * 8);
    }

    if (*working_ptr > end_ptr) {
	DPRINTF((APWARN, "ParseInt, past end of packet.\n"));
	*type = -1;
	return (0);
    }

    return (value);
}				/* end of ParseInt() */

/*
 * ParseNull:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_null().
 */
short
ParseNull(working_ptr, end_ptr, type)
    unsigned char **working_ptr;
    unsigned char  *end_ptr;
    short          *type;
{
    long            length;

    *type = ParseType(working_ptr, end_ptr);

#ifndef	SR_SNMPv2
    if (*type != NULL_TYPE) {
#else				/* SR_SNMPv2 */
    if ((*type != NULL_TYPE) &&
	(*type != NO_SUCH_OBJECT_EXCEPTION) &&
	(*type != NO_SUCH_INSTANCE_EXCEPTION) &&
	(*type != END_OF_MIB_VIEW_EXCEPTION)) {
#endif				/* SR_SNMPv2 */

	DPRINTF((APTRACE, "ParseNull, Unexpected type: %x\n", *type));
	*type = -1;
	return (-1);
    }

    if ((length = ParseLength(working_ptr, end_ptr)) == -1) {
	DPRINTF((APTRACE, "ParseNull, length error.\n"));
	return (-1);
    }

    if (length != 0) {
	DPRINTF((APTRACE, "ParseNull, length nonzero.\n"));
	return (-1);
    }

    if (*working_ptr > end_ptr) {
	DPRINTF((APWARN, "ParseNull, past end of packet.\n"));
	return (-1);
    }
    return (0);
}				/* end of ParseNull() */

/*
 * ParseSequence:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_sequence().
 */
long
ParseSequence(working_ptr, end_ptr, type)
    unsigned char **working_ptr;
    unsigned char  *end_ptr;
    short          *type;
{
    long            length;

    if ((*type = ParseType(working_ptr, end_ptr)) != SEQUENCE_TYPE) {
	DPRINTF((APTRACE, "ParseSequence, Unexpected type: %x\n", *type));
	*type = -1;
	return (-1);
    }

    if ((length = ParseLength(working_ptr, end_ptr)) == -1) {
	DPRINTF((APTRACE, "ParseSequence, length error.\n"));
	return (-1);
    }

    if (*working_ptr > end_ptr) {
	DPRINTF((APWARN, "ParseSequence, past end of packet.\n"));
	return (-1);
    }

    return (length);
}				/* end of ParseSequence() */

/*
 * ParseType:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_type().
 */
short
ParseType(working_ptr, end_ptr)
    unsigned char **working_ptr;
    unsigned char  *end_ptr;
{
    short           type;

    type = *(*working_ptr)++;

    if (*working_ptr > end_ptr) {
	DPRINTF((APWARN, "ParseType, past end of packet.\n"));
	return (-1);
    }

    switch (type) {
      case INTEGER_TYPE:
      case OCTET_PRIM_TYPE:
      case NULL_TYPE:
      case OBJECT_ID_TYPE:
      case SEQUENCE_TYPE:
      case IP_ADDR_PRIM_TYPE:
      case COUNTER_TYPE:
      case GAUGE_TYPE:
      case TIME_TICKS_TYPE:
      case OPAQUE_PRIM_TYPE:
      case GET_REQUEST_TYPE:
      case GET_NEXT_REQUEST_TYPE:
      case GET_RESPONSE_TYPE:
      case SET_REQUEST_TYPE:

#ifdef	SR_SNMPv1
      case TRAP_TYPE:
#endif				/* SR_SNMPv1 */

#ifdef	SR_SNMPv2
      case U_INTEGER_32_TYPE:
      case BIT_STRING_TYPE:
      case NSAP_ADDR_TYPE:
      case COUNTER_64_TYPE:
      case GET_BULK_REQUEST_TYPE:
      case INFORM_REQUEST_TYPE:
      case SNMPv2_TRAP_TYPE:
      case NO_SUCH_OBJECT_EXCEPTION:
	/* case NO_SUCH_INSTANCE_EXCEPTION: already defined elsewhere */
      case END_OF_MIB_VIEW_EXCEPTION:
#endif				/* SR_SNMPv2 */

#ifdef SR_SNMPv2
      case PRIV_DATA_TYPE:
#endif				/* SR_SNMPv2 */

	return (type);
	break;
      default:
	DPRINTF((APWARN, "no such type in ParseType (%d) (0x%x)\n", type, type));
	break;
    };				/* switch */
    return (-1);
}				/* end of ParseType() */

/*
 * ParseLength:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_length().
 */
long
ParseLength(working_ptr, end_ptr)
    unsigned char **working_ptr;
    unsigned char  *end_ptr;
{
    long            length;
    long            lenlen;
    long            i;

    length = (long) *(*working_ptr)++;

    if (length < 0x80)
	return (length);

    /* long form. */
    lenlen = length & 0x7f;

    if ((lenlen > 4) || (lenlen < 1)) {	/* not in standard, but I cannot
					 * handle it */
	DPRINTF((APTRACE, "ParseLength: lenlen invalid: (%d).\n", lenlen));
	return (-1);
    }

    length = 0L;
    for (i = 0; i < lenlen; i++) {
	length = (length << 8) + *(*working_ptr)++;
    }

    if (*working_ptr > end_ptr) {
	DPRINTF((APWARN, "ParseLength, past end of packet.\n"));
	return (-1);
    }

    return (length);
}				/* end of ParseLength() */

#ifdef SR_SNMPv2
/*
 * ParseImplicitSequence:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_implicit_sequence().
 */
long
ParseImplicitSequence(working_ptr, end_ptr, type)
    unsigned char **working_ptr;
    unsigned char  *end_ptr;
    short          *type;
{
    long            length;
    short           expected_type;

    expected_type = *type;

    if ((*type = ParseType(working_ptr, end_ptr)) != expected_type) {
	DPRINTF((APTRACE, "ParseImplicitSequence, Unexpected type: %x (expected %x)\n",
		 *(*working_ptr - 1), expected_type));
	*type = -1;
	return (-1);
    }

    if ((length = ParseLength(working_ptr, end_ptr)) == -1) {
	DPRINTF((APTRACE, "ParseImplicitSequence, length error.\n"));
	return (-1);
    }

    if (*working_ptr > end_ptr) {
	DPRINTF((APWARN, "ParseImplicitSequence, past end of packet.\n"));
	return (-1);
    }

    return (length);
}				/* end of ParseImplicitSequence() */

#endif				/* SR_SNMPv2 */
