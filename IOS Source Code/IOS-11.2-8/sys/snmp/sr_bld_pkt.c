/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_bld_pkt.c,v 3.2.62.2 1996/08/28 13:09:59 thille Exp $
 * $Source: /release/112/cvs/Xsys/snmp/sr_bld_pkt.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, SNMP Research
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_bld_pkt.c,v $
 * Revision 3.2.62.2  1996/08/28  13:09:59  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.62.1  1996/05/01  13:39:33  jjohnson
 * CSCdi40034:  spurious access in SNMP - AddOID
 * Issue an errmsg and abort if an attempt is made to encode an invalid OID
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:58:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:16:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:48:47  hampton
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
static char     rcsid[] = "bld_pkt.c,v 1.11 1995/02/06 18:45:36 vaughn Exp";
#endif				/* (! ( defined(lint) ) && defined(SR_RCSID)) */

/*
 * Revision History:
 *
 * bld_pkt.c,v
 * Revision 1.11  1995/02/06  18:45:36  vaughn
 * Replaced the following recursive functions with non-recursive versions: FindLenVarBind(), AddVarBind(), FreeVarBindList() and NormalizeVarBindListLengths()
 *
 * Revision 1.10  1995/02/06  18:11:07  battle
 * Changes for vrtx.
 *
 * Revision 1.9  1994/10/28  16:37:24  vaughn
 * Placed new copyright in the file
 *
 * Revision 1.8  1994/10/26  21:51:31  vaughn
 * Changed copyrights to the new copyright structure
 *
 * Revision 1.7  1994/09/30  16:10:34  battle
 * hprt changes
 *
 * Revision 1.6  1994/04/18  18:28:22  battle
 * pSOS changes
 *
 * Revision 1.5  1994/02/14  16:38:54  pearson
 * Changed copyright notice to be current for 1994, and added an
 * #ifdef SR_RCSID
 * around the static  declarations of rcsid in each file.
 *
 * AMP
 *
 * Revision 1.4  1994/01/12  23:06:48  key
 * Fixed NormalizeVarBindListLengths by actually implementing the
 * recursive traversal of the list.
 *
 * Revision 1.3  1994/01/12  21:45:13  key
 * Added NormalizeVarBindListLengths().  This routine is used when literally
 * moving a varbind list from the incoming PDU to the outgoing PDU.
 *
 * Revision 1.2  1993/12/23  16:53:51  battle
 * Added cvs headers to all .c files.
 *
 *
 *  2/6/89 JDC
 *  Amended copyright notice
 *  Professionalized error messages a bit
 *  Expanded comments and documentation
 *
 *  4/24/89 JDC
 *  Fix bug in add_unsigned_integer which caused incorrect packets
 *  to be generated if the value was >= 0x80000000 as reported by
 *  simon keen 4/22/89
 *
 *  8/3/89 JDC
 *  Shorten filename to 8 characters so that can have same name for DOS and
 *    UNIX
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
 *  02-Jan-91 JDC
 *  Seal bullethole leading to agent coredump
 *  Test for null pointer octetstring in FindLenOctetString
 *
 *  18-Feb-91 JDC
 *  Ditched LIB_PERROR
 *
 *  20-May-91 JDC
 *  More output to LIBERROR to go with bug fix of 02-Jan-91
 *
 *  13-June-92 JDC
 *  SMP
 *
 *  27-Aug-92 MGH
 *  Added SMP_TRAP_TYPE to the switch statements in BuildPdu().
 *
 *  3-Sept-92 MGH
 *  Added INFORM_REQUEST_TYPE to the switch statements in BuildPdu().
 *
 *  8-Dec-1992 DLB
 *    Lint nits.
 *
 *  6-Jan-93 MGH
 *  Added U_INTEGER_32_TYPE to the switch statement in AddVarBind().
 *
 *  5-May-93 MGH
 *  Changed all LIB_ERROR* messages to DPRINTF messages.
 *
 * 10-Jun-1993 DLB
 *   Changed all short parameters to int.  Short parameters are a bad idea
 *   for numerous reasons I won't go into here.
 */

#include "master.h"


#include "sr_snmp.h"
#include "sr_diag.h"
#include "logger.h"
#include "msg_snmp.c"		/* Not a typo, see logger.h */

/*
 * BuildPdu:
 *
 *   This routine is called with the PDU pointer being used to
 *   create the PDU.  It traces down the structure of varbinds
 *   that has been added to it and builds the ASN.1 packet in the
 *   *packlet pointer of the PDU pointer's data structure.  At
 *   this point, PDU processing is complete and the structure is
 *   ready to be passed onto the authentication layers.
 *
 *   It is the main part of the packet builder.  It traverses the
 *   varbind list to determine the packet size and then calls the
 *   add_* routines to bind the item's constructs into the packet.
 *   The 'packlet' is an OctetString in pdu-ptr->packlet.
 *
 *   The former name (pre-snmp12.1.0.0 release) was build_pdu().
 */

short
BuildPdu(pdu_ptr)
    Pdu            *pdu_ptr;
{
    long            varbindlen;
    long            varbind_tot_len;
    long            varbindlenlen;
    long            datalen, temp_data_len;
    long            lenlen;
    unsigned char  *working_ptr;

    /* find out length of whole PDU */
    if ((varbindlen = FindLenVarBind(pdu_ptr->var_bind_list)) == -1) {
	DPRINTF((APTRACE, "BuildPdu, varbindlen:\n"));
	return (-1);		/* abort */
    }

    if ((varbindlenlen = DoLenLen(varbindlen)) == -1) {
	DPRINTF((APTRACE, "BuildPdu,varbindlenlen:\n"));
	return (-1);
    }
    varbind_tot_len = 1 + varbindlenlen + varbindlen;

    switch (pdu_ptr->type) {
      case GET_REQUEST_TYPE:
      case GET_NEXT_REQUEST_TYPE:
      case GET_RESPONSE_TYPE:
      case SET_REQUEST_TYPE:

#ifdef SR_SNMPv2
      case GET_BULK_REQUEST_TYPE:
      case INFORM_REQUEST_TYPE:
      case SNMPv2_TRAP_TYPE:
	/* since unions are the same shape, can get away with the following: */
#endif				/* SR_SNMPv2 */

	datalen = FindLenInt(pdu_ptr->u.normpdu.request_id) +
	    FindLenInt(pdu_ptr->u.normpdu.error_status) +
	    FindLenInt(pdu_ptr->u.normpdu.error_index) +
	    varbind_tot_len;
	break;

#ifdef	SR_SNMPv1
      case TRAP_TYPE:
	datalen = FindLenInt(pdu_ptr->u.trappdu.generic_trap) +
	    FindLenInt(pdu_ptr->u.trappdu.specific_trap) +
	    FindLenUInt(pdu_ptr->u.trappdu.time_ticks) +
	    varbind_tot_len;
	if ((temp_data_len = FindLenOID(pdu_ptr->u.trappdu.enterprise)) == -1) {
	    DPRINTF((APTRACE, "BuildPdu, temp_data_len (1)\n"));
	    return (-1);
	}
	datalen = datalen + temp_data_len;
	if ((temp_data_len = FindLenOctetString(pdu_ptr->u.trappdu.agent_addr)) == -1) {
	    DPRINTF((APTRACE, "BuildPdu, temp_data_len (2)\n"));
	    return (-1);
	}
	datalen = datalen + temp_data_len;
	break;
#endif				/* SR_SNMPv1 */

      default:
	DPRINTF((APTRACE, "BuildPdu, bad pdu type: %x\n", pdu_ptr->type));
	return (-1);
	break;
    };				/* end of switch */

    if ((lenlen = DoLenLen(datalen)) == -1) {
	DPRINTF((APTRACE, "BuildPdu, lenlen:\n"));
	return (-1);
    }

    /* now allocate memory for PDU packlet */
    if ((pdu_ptr->packlet = (OctetString *) malloc(sizeof(OctetString))) == NULL) {
	DPRINTF((APWARN, "BuildPdu, pdu_ptr->packlet malloc\n"));
	return (-1);
    }

    pdu_ptr->packlet->length = 1 + lenlen + datalen;

    if ((pdu_ptr->packlet->octet_ptr =
    (unsigned char *) malloc((unsigned) pdu_ptr->packlet->length)) == NULL) {
	DPRINTF((APWARN, "BuildPdu, pdu_ptr->packlet->octet_ptr malloc\n"));
	free((char *) pdu_ptr->packlet);
	NULLIT(pdu_ptr->packlet);
	return (-1);
    }
    working_ptr = pdu_ptr->packlet->octet_ptr;

    *working_ptr++ = (unsigned char) (0xff & pdu_ptr->type);
    AddLen(&working_ptr, lenlen, datalen);

    switch (pdu_ptr->type) {
      case GET_REQUEST_TYPE:
      case GET_NEXT_REQUEST_TYPE:
      case GET_RESPONSE_TYPE:
      case SET_REQUEST_TYPE:

#ifdef	SR_SNMPv2
      case INFORM_REQUEST_TYPE:
      case SNMPv2_TRAP_TYPE:
      case GET_BULK_REQUEST_TYPE:
	/* since unions are the same shape, can get away with the following: */
#endif				/* SR_SNMPv2 */

	(void) AddInt(&working_ptr, INTEGER_TYPE, pdu_ptr->u.normpdu.request_id);
	(void) AddInt(&working_ptr, INTEGER_TYPE, pdu_ptr->u.normpdu.error_status);
	(void) AddInt(&working_ptr, INTEGER_TYPE, pdu_ptr->u.normpdu.error_index);
	break;

#ifdef	SR_SNMPv1
      case TRAP_TYPE:
	if (AddOID(&working_ptr, OBJECT_ID_TYPE,
		   pdu_ptr->u.trappdu.enterprise) == -1) {
	    DPRINTF((APTRACE, "AddVarBind, AddOID\n"));
	    return (-1);
	}

	if (AddOctetString(&working_ptr, IP_ADDR_PRIM_TYPE, pdu_ptr->u.trappdu.agent_addr) == -1) {
	    DPRINTF((APTRACE, "AddVarBind, AddOctetString\n"));
	    return -1;
	}
	(void) AddInt(&working_ptr, INTEGER_TYPE, pdu_ptr->u.trappdu.generic_trap);
	(void) AddInt(&working_ptr, INTEGER_TYPE, pdu_ptr->u.trappdu.specific_trap);
	(void) AddUInt(&working_ptr, TIME_TICKS_TYPE, pdu_ptr->u.trappdu.time_ticks);
	break;
#endif				/* SR_SNMPv1 */

      default:
	DPRINTF((APTRACE, "BuildPdu, bad pdu_ptr->type - II. Should not happen:%x\n",
		 pdu_ptr->type));
	FreeOctetString(pdu_ptr->packlet);
	NULLIT(pdu_ptr->packlet);
	return (-1);
	break;
    };				/* end of switch II */


    *working_ptr++ = SEQUENCE_TYPE;
    AddLen(&working_ptr, varbindlenlen, varbindlen);

    if (AddVarBind(&working_ptr, pdu_ptr->var_bind_list, pdu_ptr) == -1) {
	FreeOctetString(pdu_ptr->packlet);
	NULLIT(pdu_ptr->packlet);
	return (-1);
    }

    return (0);
}				/* end of BuildPdu() */

/*
 * FindLenVarBind:
 *
 * The former name (pre-snmp12.1.0.0 release) was find_len_varbind().
 */
long
FindLenVarBind(vb_ptr)
    VarBind        *vb_ptr;
{
    long            lenlen;
    long            total = 0;

    while (vb_ptr != NULL) {
	vb_ptr->data_length = vb_ptr->name_size + vb_ptr->value_size;
	lenlen = DoLenLen(vb_ptr->data_length);
	if (lenlen == -1) {
	    DPRINTF((APTRACE, "FindLenVarBind, lenlen\n"));
	    return (-1);
	}

	total += 1 + lenlen + vb_ptr->data_length;
	vb_ptr = vb_ptr->next_var;
    }
    return (total);
}


#ifdef	SR_SNMPv2
/*
 * FindLenBitString:
 *
 * The former name (pre-snmp12.1.0.0 release) was find_len_bitstring().
 */
short
FindLenBitString(bitstring_ptr)
    BitString      *bitstring_ptr;
{
    long            lenlen;

    if (bitstring_ptr == NULL) {
	DPRINTF((APTRACE, "FindLenBitString, NULL input\n"));
	return (-1);
    }
    if ((lenlen = DoLenLen(bitstring_ptr->length + 1)) == -1) {
	DPRINTF((APTRACE, "FindLenBitString, lenlen\n"));
	return (-1);
    }
    return (1 + lenlen + 1 + bitstring_ptr->length);
}

#endif				/* SR_SNMPv2 */

/*
 * FindLenOctetString:
 *
 * The former name (pre-snmp12.1.0.0 release) was find_len_octetstring().
 */
long
FindLenOctetString(os_ptr)
    OctetString    *os_ptr;
{
    long            lenlen;

    /* begin fix 02-Jan-91 */
    if (os_ptr == NULL) {
	DPRINTF((APTRACE, "FindLenOctetString, NULL input\n"));
	return (-1);
    }
    /* end fix 02-Jan-91 */

    if ((lenlen = DoLenLen(os_ptr->length)) == -1) {
	DPRINTF((APTRACE, "FindLenOctetString, lenlen\n"));
	return (-1);
    }
    return (1 + lenlen + os_ptr->length);
}

/*
 * FindLenOID:
 *
 * The former name (pre-snmp12.1.0.0 release) was find_len_oid().
 */
short
FindLenOID(oid_ptr)
    OID            *oid_ptr;
{
    long            lenlen;
    short           i;
    long            encoded_len;

    encoded_len = 1;		/* for first two SID's */

    for (i = 2; i < oid_ptr->length; i++) {
	if (oid_ptr->oid_ptr[i] < 0x80)	/* 0 - 0x7f */
	    encoded_len += 1;
	else if (oid_ptr->oid_ptr[i] < 0x4000)	/* 0x80 - 0x3fff */
	    encoded_len += 2;
	else if (oid_ptr->oid_ptr[i] < 0x200000)	/* 0x4000 - 0x1FFFFF */
	    encoded_len += 3;
	else if (oid_ptr->oid_ptr[i] < 0x10000000)	/* 0x200000 - 0x0fffffff */
	    encoded_len += 4;
	else
	    encoded_len += 5;
    }

    if ((lenlen = DoLenLen(encoded_len)) == -1) {
	DPRINTF((APTRACE, "FindLenOID, lenlen\n"));
	return (-1);
    }
    return (short) (1 + lenlen + encoded_len);
}

/*
 * FindLenUInt:
 *
 * The former name (pre-snmp12.1.0.0 release) was find_len_unsignedinteger().
 */
short
FindLenUInt(value)
    unsigned long   value;
{
    long            datalen;

    /* if high bit one, must use 5 octets (first with 00) */
    if (((value >> 24) & 0x0ff) != 0)
	datalen = 4;
    else if (((value >> 16) & 0x0ff) != 0)
	datalen = 3;
    else if (((value >> 8) & 0x0ff) != 0)
	datalen = 2;
    else
	datalen = 1;

    if (((value >> (8 * (datalen - 1))) & 0x080) != 0)
	datalen++;

    /* length of length  < 127 octets */

    return (short) (1 + 1 + datalen);
}

/*
 * FindLenInt:
 *
 * The former name (pre-snmp12.1.0.0 release) was find_len_signedinteger().
 */
short
FindLenInt(value)
    long            value;
{
    long            datalen;

    switch ((unsigned char) ((value >> 24) & 0x0ff)) {
      case 0x00:
	if (((value >> 16) & 0x0ff) != 0)
	    datalen = 3;
	else if (((value >> 8) & 0x0ff) != 0)
	    datalen = 2;
	else
	    datalen = 1;
	if (((value >> (8 * (datalen - 1))) & 0x080) != 0)
	    datalen++;
	break;
      case 0xff:
	if (((value >> 16) & 0x0ff) != 0xFF)
	    datalen = 3;
	else if (((value >> 8) & 0x0ff) != 0xFF)
	    datalen = 2;
	else
	    datalen = 1;
	if (((value >> (8 * (datalen - 1))) & 0x080) == 0)
	    datalen++;
	break;
      default:
	datalen = 4;
    };				/* end of switch */

    return (short) (1 + 1 + datalen);
}

#ifdef	SR_SNMPv2
/*
 * FindLenUInt64:
 *
 * The former name (pre-snmp12.1.0.0 release) was find_len_uint64().
 */
short
FindLenUInt64(value)
    UInt64         *value;
{
    long            datalen;

    /* if high bit one, must use 5 octets (first with 00) */
    if (((value->big_end >> 24) & 0x0ff) != 0) {
	datalen = 8;
	if (((value->big_end >> 24) & 0x080) != 0) {
	    datalen++;
	}
    }
    else if (((value->big_end >> 16) & 0x0ff) != 0) {
	datalen = 7;
	if (((value->big_end >> 16) & 0x080) != 0) {
	    datalen++;
	}
    }
    else if (((value->big_end >> 8) & 0x0ff) != 0) {
	datalen = 6;
	if (((value->big_end >> 8) & 0x080) != 0) {
	    datalen++;
	}
    }
    else if (((value->big_end) & 0x0ff) != 0) {
	datalen = 5;
	if (((value->big_end) & 0x080) != 0) {
	    datalen++;
	}
    }
    else if (((value->little_end >> 24) & 0x0ff) != 0) {
	datalen = 4;
	if (((value->little_end >> 24) & 0x080) != 0) {
	    datalen++;
	}
    }
    else if (((value->little_end >> 16) & 0x0ff) != 0) {
	datalen = 3;
	if (((value->little_end >> 16) & 0x080) != 0) {
	    datalen++;
	}
    }
    else if (((value->little_end >> 8) & 0x0ff) != 0) {
	datalen = 2;
	if (((value->little_end >> 8) & 0x080) != 0) {
	    datalen++;
	}
    }
    else {
	datalen = 1;
	if (((value->little_end) & 0x080) != 0) {
	    datalen++;
	}
    }

    /* length of length  < 127 octets */

    return (short) (1 + 1 + datalen);
}

#endif				/* SR_SNMPv2 */

/*
 * DoLenLen:
 *
 * The former name (pre-snmp12.1.0.0 release) was dolenlen().
 */
short
DoLenLen(len)
    long            len;
{
    /* short form? */
    if (len < 128)
	return (1);
    if (len < 0x0100)
	return (2);
    if (len < 0x010000)
	return (3);
    if (len < 0x01000000)
	return (4);
    DPRINTF((APTRACE, "Lenlen: Length greater than 0x01000000???\n"));
    return (-1);
}

/*
 * AddLen:
 *
 * The former name (pre-snmp12.1.0.0 release) was add_len().
 */
void
AddLen(working_ptr, lenlen, data_len)
    unsigned char **working_ptr;
    long            lenlen, data_len;
{
    long            i;

    if (lenlen == 1) {		/* short form? */
	*(*working_ptr)++ = (unsigned char) data_len;
	return;
    }
    /* oh well, long form time */
    *(*working_ptr)++ = ((unsigned char) 0x80 + (unsigned char) lenlen - 1);
    for (i = 1; i < lenlen; i++)
	*(*working_ptr)++ = (unsigned char) ((data_len >> (8 * (lenlen - i - 1))) & 0x0FF);
}

/*
 * AddVarBind:
 *
 * The former name (pre-snmp12.1.0.0 release) was add_varbind().
 */
short
AddVarBind(working_ptr, vb_ptr, pdu_ptr)
    unsigned char **working_ptr;
    VarBind        *vb_ptr;
    Pdu            *pdu_ptr;
{
    long            lenlen;

    if (pdu_ptr == NULL)
	return (0);

    while (vb_ptr != NULL) {

	if ((lenlen = DoLenLen(vb_ptr->data_length)) == -1) {
	    DPRINTF((APTRACE, "AddVarBind, lenlen:\n"));
	    return (-1);
	}

	*(*working_ptr)++ = SEQUENCE_TYPE;
	AddLen(working_ptr, lenlen, vb_ptr->data_length);

	if (AddOID(working_ptr, OBJECT_ID_TYPE, vb_ptr->name) == -1) {
	    DPRINTF((APTRACE, "AddVarBind, AddOID\n"));
	    return (-1);
	}

	switch (vb_ptr->value.type) {
	  case COUNTER_TYPE:	/* handle unsigned integers including
				 * COUNTER_32 */
	  case GAUGE_TYPE:	/* includes GAUGE_32 */
	  case TIME_TICKS_TYPE:

#ifdef SR_SNMPv2
	  case U_INTEGER_32_TYPE:
#endif				/* SR_SNMPv2 */

	    (void) AddUInt(working_ptr, vb_ptr->value.type, vb_ptr->value.ul_value);
	    break;
	  case INTEGER_TYPE:	/* handle signed integers including
				 * INTEGER_32 */
	    (void) AddInt(working_ptr, vb_ptr->value.type, vb_ptr->value.sl_value);
	    break;
	  case OBJECT_ID_TYPE:	/* handle quasi-octet strings */
	    if (AddOID(working_ptr, vb_ptr->value.type,
		       vb_ptr->value.oid_value) == -1) {
		DPRINTF((APTRACE, "AddVarBind, AddOID\n"));
		return (-1);
	    }

	    break;

#ifdef	SR_SNMPv2
	  case COUNTER_64_TYPE:/* handle big unsigned integers */
	    (void) AddUInt64(working_ptr, vb_ptr->value.type, vb_ptr->value.uint64_value);
	    break;
	  case BIT_STRING_TYPE:
	    if (AddBitString(working_ptr, vb_ptr->value.type, vb_ptr->value.bitstring_value) == -1) {
		DPRINTF((APTRACE, "AddVarBind, AddBitString\n"));
		return -1;
	    }
	    break;
#endif				/* SR_SNMPv2 */

	  case OCTET_PRIM_TYPE:
	  case IP_ADDR_PRIM_TYPE:
	  case OPAQUE_PRIM_TYPE:

#ifdef  SR_SNMPv2
	  case NSAP_ADDR_TYPE:
#endif				/* SR_SNMPv2 */

	    if (AddOctetString(working_ptr, vb_ptr->value.type, vb_ptr->value.os_value) == -1) {
		DPRINTF((APTRACE, "AddVarBind, AddOctetString\n"));
		return -1;
	    }
	    break;
	  case NULL_TYPE:

#ifdef	SR_SNMPv2
	  case NO_SUCH_OBJECT_EXCEPTION:
	  case NO_SUCH_INSTANCE_EXCEPTION:
	  case END_OF_MIB_VIEW_EXCEPTION:
#endif				/* SR_SNMPv2 */

	    AddNull(working_ptr, vb_ptr->value.type);
	    break;
	  default:
	    DPRINTF((APTRACE, "AddVarBind, value: Illegal type: 0x%x\n", vb_ptr->value.type));
	    return (-1);
	    break;
	};			/* end of switch */

	pdu_ptr->num_varbinds++;
	vb_ptr = vb_ptr->next_var;
    }
    return (0);
}


/*
 * AddOctetString:
 *
 * The former name (pre-snmp12.1.0.0 release) was add_octetstring().
 */
short
AddOctetString(working_ptr, type, os_ptr)
    unsigned char **working_ptr;
    int             type;
    OctetString    *os_ptr;
{
    long            i;
    long            lenlen;

    if ((lenlen = DoLenLen(os_ptr->length)) == -1) {
	DPRINTF((APTRACE, "AddOctetString,lenlen\n"));
	return (-1);
    }


    *(*working_ptr)++ = (unsigned char) (0xff & type);
    AddLen(working_ptr, lenlen, os_ptr->length);

    for (i = 0; i < os_ptr->length; i++)
	*(*working_ptr)++ = os_ptr->octet_ptr[i];

    return (0);
}

/*
 * AddOID:
 *
 * The former name (pre-snmp12.1.0.0 release) was add_oid().
 */
short
AddOID(working_ptr, type, oid_ptr)
    unsigned char **working_ptr;
    int             type;
    OID            *oid_ptr;
{
    short           i;
    long            lenlen;
    long            encoded_len;

    if (oid_ptr->length < 2) {
	errmsg(&msgsym(BADOID, SNMP));
	return (-1);
    }

    encoded_len = 1;		/* for first two SID's */
    for (i = 2; i < oid_ptr->length; i++) {
	if (oid_ptr->oid_ptr[i] < 0x80)	/* 0 - 0x7f */
	    encoded_len += 1;
	else if (oid_ptr->oid_ptr[i] < 0x4000)	/* 0x80 - 0x3fff */
	    encoded_len += 2;
	else if (oid_ptr->oid_ptr[i] < 0x200000)	/* 0x4000 - 0x1FFFFF */
	    encoded_len += 3;
	else if (oid_ptr->oid_ptr[i] < 0x10000000)	/* 0x200000 - 0x0fffffff */
	    encoded_len += 4;
	else
	    encoded_len += 5;
    }

    if ((lenlen = DoLenLen(encoded_len)) == -1) {
	DPRINTF((APTRACE, "AddOID,lenlen\n"));
	return (-1);
    }

    *(*working_ptr)++ = (unsigned char) (0xff & type);
    AddLen(working_ptr, lenlen, encoded_len);

    if (oid_ptr->length < 2)
	*(*working_ptr)++ = (unsigned char) (oid_ptr->oid_ptr[0] * 40);
    else
	*(*working_ptr)++ = (unsigned char) ((oid_ptr->oid_ptr[0] * 40) +
					     oid_ptr->oid_ptr[1]);

    for (i = 2; i < oid_ptr->length; i++) {
	if (oid_ptr->oid_ptr[i] < 0x80) {	/* 0 - 0x7f */
	    *(*working_ptr)++ = (unsigned char) oid_ptr->oid_ptr[i];
	}
	else if (oid_ptr->oid_ptr[i] < 0x4000) {	/* 0x80 - 0x3fff */
	    *(*working_ptr)++ = (unsigned char)
		(((oid_ptr->oid_ptr[i]) >> 7) | 0x80);	/* set high bit */
	    *(*working_ptr)++ = (unsigned char) (oid_ptr->oid_ptr[i] & 0x7f);
	}
	else if (oid_ptr->oid_ptr[i] < 0x200000) {	/* 0x4000 - 0x1FFFFF */
	    *(*working_ptr)++ = (unsigned char)
		(((oid_ptr->oid_ptr[i]) >> 14) | 0x80);	/* set high bit */
	    *(*working_ptr)++ = (unsigned char)
		(((oid_ptr->oid_ptr[i]) >> 7) | 0x80);	/* set high bit */
	    *(*working_ptr)++ = (unsigned char) (oid_ptr->oid_ptr[i] & 0x7f);
	}
	else if (oid_ptr->oid_ptr[i] < 0x10000000) {	/* 0x200000 - 0x0fffffff */
	    *(*working_ptr)++ = (unsigned char)
		(((oid_ptr->oid_ptr[i]) >> 21) | 0x80);	/* set high bit */
	    *(*working_ptr)++ = (unsigned char)
		(((oid_ptr->oid_ptr[i]) >> 14) | 0x80);	/* set high bit */
	    *(*working_ptr)++ = (unsigned char)
		(((oid_ptr->oid_ptr[i]) >> 7) | 0x80);	/* set high bit */
	    *(*working_ptr)++ = (unsigned char) (oid_ptr->oid_ptr[i] & 0x7f);
	}
	else {
	    *(*working_ptr)++ = (unsigned char)
		(((oid_ptr->oid_ptr[i]) >> 28) | 0x80);	/* set high bit */
	    *(*working_ptr)++ = (unsigned char)
		(((oid_ptr->oid_ptr[i]) >> 21) | 0x80);	/* set high bit */
	    *(*working_ptr)++ = (unsigned char)
		(((oid_ptr->oid_ptr[i]) >> 14) | 0x80);	/* set high bit */
	    *(*working_ptr)++ = (unsigned char)
		(((oid_ptr->oid_ptr[i]) >> 7) | 0x80);	/* set high bit */
	    *(*working_ptr)++ = (unsigned char) (oid_ptr->oid_ptr[i] & 0x7f);
	}
    }				/* end of second for */

    return (0);
}				/* end of AddOID */

/*
 * AddUInt:
 *
 * The former name (pre-snmp12.1.0.0 release) was add_unsignedinteger().
 */
short
AddUInt(working_ptr, type, value)
    unsigned char **working_ptr;
    int             type;
    unsigned long   value;
{
    long            i;
    long            datalen;
    long            lenlen;


    /* if high bit one, must use 5 octets (first with 00) */
    if (((value >> 24) & 0x0ff) != 0)
	datalen = 4;

    else if (((value >> 16) & 0x0ff) != 0)
	datalen = 3;
    else if (((value >> 8) & 0x0ff) != 0)
	datalen = 2;
    else
	datalen = 1;

    if (((value >> (8 * (datalen - 1))) & 0x080) != 0)
	datalen++;

    lenlen = 1;			/* < 127 octets */

    *(*working_ptr)++ = (unsigned char) (0xff & type);
    AddLen(working_ptr, lenlen, datalen);

    if (datalen == 5) {		/* gotta put a 00 in first octet */
	*(*working_ptr)++ = (unsigned char) 0;
	for (i = 1; i < datalen; i++)	/* bug fix 4/24/89, change 0 -> 1 */
	    *(*working_ptr)++
		= (unsigned char) (value >> (8 * ((datalen - 1) - i) & 0x0ff));
    }
    else {
	for (i = 0; i < datalen; i++)
	    *(*working_ptr)++
		= (unsigned char) (value >> (8 * ((datalen - 1) - i) & 0x0ff));
    }

    return (0);
}

/*
 * AddInt:
 *
 * The former name (pre-snmp12.1.0.0 release) was add_signedinteger().
 */
short
AddInt(working_ptr, type, value)
    unsigned char **working_ptr;
    int             type;
    long            value;
{
    long            i;
    long            datalen;
    long            lenlen;

    switch ((unsigned char) ((value >> 24) & 0x0ff)) {
      case 0x00:
	if (((value >> 16) & 0x0ff) != 0)
	    datalen = 3;
	else if (((value >> 8) & 0x0ff) != 0)
	    datalen = 2;
	else
	    datalen = 1;
	if (((value >> (8 * (datalen - 1))) & 0x080) != 0)
	    datalen++;

	break;
      case 0xff:
	if (((value >> 16) & 0x0ff) != 0xFF)
	    datalen = 3;
	else if (((value >> 8) & 0x0ff) != 0xFF)
	    datalen = 2;
	else
	    datalen = 1;
	if (((value >> (8 * (datalen - 1))) & 0x080) == 0)
	    datalen++;
	break;

      default:
	datalen = 4;
    };				/* end of switch */

    lenlen = 1;			/* < 127 octets */

    *(*working_ptr)++ = (unsigned char) (0xff & type);
    AddLen(working_ptr, lenlen, datalen);

    for (i = 0; i < datalen; i++)
	*(*working_ptr)++
	    = (unsigned char) (value >> (8 * ((datalen - 1) - i) & 0x0ff));

    return (0);
}

#ifdef	SR_SNMPv2
/*
 * AddUInt64:
 *
 * The former name (pre-snmp12.1.0.0 release) was add_uint64().
 */
short
AddUInt64(working_ptr, type, value)
    unsigned char **working_ptr;
    int             type;
    UInt64         *value;
{
    long            i;
    long            datalen;
    long            lenlen;


    datalen = FindLenUInt64(value) - 2;

    lenlen = 1;			/* < 127 octets */

    *(*working_ptr)++ = (unsigned char) (0xff & type);
    AddLen(working_ptr, lenlen, datalen);

    if (datalen == 9) {		/* gotta put a 00 in first octet */
	*(*working_ptr)++ = (unsigned char) 0;
	datalen--;
    }

    for (i = datalen; i > 4; i--) {
	*(*working_ptr)++ =
	    (unsigned char) (value->big_end >> (8 * (i - 5) & 0x0ff));
    }
    for (; i > 0; i--) {
	*(*working_ptr)++ =
	    (unsigned char) (value->little_end >> (8 * (i - 1) & 0x0ff));
    }

    return (0);
}

/*
 * AddBitString:
 *
 * The former name (pre-snmp12.1.0.0 release) was add_bitstring().
 */
short
AddBitString(working_ptr, type, bitstring_ptr)
    unsigned char **working_ptr;
    int             type;
    BitString      *bitstring_ptr;
{
    long            i;
    long            lenlen;

    if ((lenlen = DoLenLen(bitstring_ptr->length + 1)) == -1) {
	DPRINTF((APTRACE, "AddBitString,lenlen\n"));
	return (-1);
    }


    *(*working_ptr)++ = (unsigned char) (0xff & type);
    AddLen(working_ptr, lenlen, bitstring_ptr->length + 1);

    *(*working_ptr)++ = (unsigned char) (0xff & bitstring_ptr->unused);

    for (i = 0; i < bitstring_ptr->length; i++)
	*(*working_ptr)++ = bitstring_ptr->octet_ptr[i];

    return (0);
}

#endif				/* SR_SNMPv2 */

/*
 * AddNull:
 *
 * The former name (pre-snmp12.1.0.0 release) was add_null().
 */
void
AddNull(working_ptr, type)
    unsigned char **working_ptr;
    int             type;
{
    *(*working_ptr)++ = (unsigned char) (0xff & type);
    *(*working_ptr)++ = 0x00;
}

/*
 * get-responses based on Set requests and errors that turn the PDU's
 * around may have improper length values in name_size and value_size.
 *
 * On parsing incoming PDU's,the agent may have to deal with broken
 * managers may have encoded lengths with non-minimal values. These
 * non-minimal values are parsed and put in name_size and value_size.
 * These are used to determine the number of octets to skip to find the
 * next element to parse.
 *
 * On building an outgoing PDU, these values assume that length encoding was
 * done using minimal length encoding.  They are normally set to this value
 * by way of the MakeVarBind() call.  However, when a PDU is simply turned
 * around, the VarBinds could have the erroneous lengths in them.  This
 * routine should be called by any function that turns the packet around
 * to normalize those lengths.
 */

void
NormalizeVarBindListLengths(vb_ptr)
    VarBind        *vb_ptr;
{
    while (vb_ptr != NULL) {

	/* Fix up name size */
	vb_ptr->name_size = FindLenOID(vb_ptr->name);


	/* Now fix up value size */
	/* combine the Choices from simple and application */
	switch (vb_ptr->value.type) {
	  case COUNTER_TYPE:	/* includes COUNTER_32_TYPE */
	  case GAUGE_TYPE:	/* includes GAUGE_32_TYPE */
	  case TIME_TICKS_TYPE:

#ifdef SR_SNMPv2
	  case U_INTEGER_32_TYPE:
#endif				/* SR_SNMPv2 */

	    vb_ptr->value_size = FindLenUInt(vb_ptr->value.ul_value);
	    break;
	  case INTEGER_TYPE:	/* includes INTEGER_32_TYPE */
	    vb_ptr->value_size = FindLenInt(vb_ptr->value.sl_value);
	    break;
	  case OBJECT_ID_TYPE:
	    vb_ptr->value_size = FindLenOID(vb_ptr->value.oid_value);
	    break;
	  case OCTET_PRIM_TYPE:
	  case IP_ADDR_PRIM_TYPE:

#ifdef	SR_SNMPv2
	  case NSAP_ADDR_TYPE:
	  case BIT_STRING_TYPE:
#endif				/* SR_SNMPv2 */

	  case OPAQUE_PRIM_TYPE:
	    vb_ptr->value_size = FindLenOctetString(vb_ptr->value.os_value);
	    break;
	  case NULL_TYPE:

#ifdef	SR_SNMPv2
	  case NO_SUCH_OBJECT_EXCEPTION:
	  case NO_SUCH_INSTANCE_EXCEPTION:
	  case END_OF_MIB_VIEW_EXCEPTION:
#endif				/* SR_SNMPv2 */

	    vb_ptr->value_size = 2;

	    break;
	}
	vb_ptr->data_length = vb_ptr->name_size + vb_ptr->value_size;
	vb_ptr = vb_ptr->next_var;
    }
}
