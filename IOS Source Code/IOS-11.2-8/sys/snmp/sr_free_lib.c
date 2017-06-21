/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_free_lib.c,v 3.2 1995/11/17 18:59:17 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/snmp/sr_free_lib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, SNMP Research
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_free_lib.c,v $
 * Revision 3.2  1995/11/17  18:59:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:16:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:49:09  hampton
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
static char     rcsid[] = "free_lib.c,v 1.11 1995/02/06 18:45:38 vaughn Exp";
#endif				/* (! ( defined(lint) ) && defined(SR_RCSID)) */

/*
 * Revision History:
 *
 * free_lib.c,v
 * Revision 1.11  1995/02/06  18:45:38  vaughn
 * Replaced the following recursive functions with non-recursive versions: FindLenVarBind(), AddVarBind(), FreeVarBindList() and NormalizeVarBindListLengths()
 *
 * Revision 1.10  1995/02/06  18:11:10  battle
 * Changes for vrtx.
 *
 * Revision 1.9  1994/10/28  16:37:27  vaughn
 * Placed new copyright in the file
 *
 * Revision 1.8  1994/10/26  21:51:33  vaughn
 * Changed copyrights to the new copyright structure
 *
 * Revision 1.7  1994/09/30  19:16:57  battle
 * Fixed typo in ifdef SR_HPRT.
 *
 * Revision 1.6  1994/09/30  16:10:39  battle
 * hprt changes
 *
 * Revision 1.5  1994/04/18  18:28:25  battle
 * pSOS changes
 *
 * Revision 1.4  1994/02/14  20:40:39  levi
 * Added FreeObjectSyntax.
 *
 * Revision 1.3  1994/02/14  16:38:58  pearson
 * Changed copyright notice to be current for 1994, and added an
 * #ifdef SR_RCSID
 * around the static  declarations of rcsid in each file.
 *
 * AMP
 *
 * Revision 1.2  1993/12/23  16:53:55  battle
 * Added cvs headers to all .c files.
 *
 *
 *  2/6/89 JDC
 *  Amended copyright notice
 *  Expanded comments and documentation
 *
 *  11/2/89 JDC
 *  Add missing braces in FreeOID() and FreeOctetString
 *
 *  11/8/89 JDC
 *  Make it print pretty via tgrind
 *
 *  01-Jan-91 JDC
 *  Substituted NULLIT(pointer) for LIGHT
 *
 *  13-June-92 JDC
 *  SMP
 *
 *  12-Nov-1992 DLB
 *   Changed FreeVarBind() to take into account the fact that ObjectSyntax
 *   structure may be a union.
 *
 *  13-Nov-1992 DLB
 *   Added #ifdef SMP around NSAP_ADDR_TYPE in FreeVarBind().
 *
 *  8-Dec-1992 DLB
 *    Lint nits.
 */

#include "master.h"


#include "sr_snmp.h"

/*
 * FreeOctetString: free the data structures allocated and built by
 *                  MakeOctetString().
 *
 * The former name (pre-snmp12.1.0.0 release) was free_octetstring().
 */
void
FreeOctetString(os_ptr)
    OctetString    *os_ptr;
{
    if (os_ptr != NULL) {
	if (os_ptr->octet_ptr != NULL) {
	    free((char *) os_ptr->octet_ptr);
	}
	free((char *) os_ptr);
    }
}

/*
 * FreeObjectSyntax:
 *
 */
void
FreeObjectSyntax(os)
    ObjectSyntax   *os;
{
    if (os != NULL) {
	switch (os->type) {
	  case OCTET_PRIM_TYPE:
	  case IP_ADDR_PRIM_TYPE:
	  case OPAQUE_PRIM_TYPE:
	    FreeOctetString(os->os_value);
	    break;
	  case OBJECT_ID_TYPE:
	    FreeOID(os->oid_value);
	    break;

#ifdef SR_SNMPv2
	  case BIT_STRING_TYPE:
	    FreeBitString(os->bitstring_value);
	    break;
	  case COUNTER_64_TYPE:
	    FreeUInt64(os->uint64_value);
	    break;
#endif				/* SR_SNMPv2 */
	}

	free((char *) os);
    }
}

/*
 * FreeOID: free the data structures allocated and built by MakeOIDFromDot,
 *          etc.
 *
 * The former name (pre-snmp12.1.0.0 release) was free_oid().
 */
void
FreeOID(oid_ptr)
    OID            *oid_ptr;
{
    if (oid_ptr != NULL) {
	if (oid_ptr->oid_ptr != NULL) {
	    free((char *) oid_ptr->oid_ptr);
	}
	free((char *) oid_ptr);
    }
}

#ifdef	SR_SNMPv2
/*
 * FreeUInt64:
 *
 * The former name (pre-snmp12.1.0.0 release) was free_uint64().
 */
void
FreeUInt64(uint64_ptr)
    UInt64         *uint64_ptr;
{
    if (uint64_ptr != NULL) {
	free((char *) uint64_ptr);
    }
}

/*
 * FreeBitString:
 *
 * The former name (pre-snmp12.1.0.0 release) was free_bitstring().
 */
void
FreeBitString(bitstring_ptr)
    BitString      *bitstring_ptr;
{
    if (bitstring_ptr != NULL) {
	if (bitstring_ptr->octet_ptr != NULL) {
	    free((char *) bitstring_ptr->octet_ptr);
	}
	free((char *) bitstring_ptr);
    }
}

#endif				/* SR_SNMPv2 */

/*
 * FreeVarBind: free the data structures allocated and built by calls to
 *              MakeVarBindWithNull(), MakeVarBindWithValue(), etc.
 *
 * The former name (pre-snmp12.1.0.0 release) was free_varbind().
 */
void
FreeVarBind(vb_ptr)
    VarBind        *vb_ptr;
{
    if (vb_ptr != NULL) {
	FreeOID(vb_ptr->name);
	switch (vb_ptr->value.type) {
	  case OBJECT_ID_TYPE:
	    FreeOID(vb_ptr->value.oid_value);
	    break;
	  case OCTET_PRIM_TYPE:
	  case IP_ADDR_PRIM_TYPE:
	  case OPAQUE_PRIM_TYPE:

#ifdef SR_SNMPv2
	  case NSAP_ADDR_TYPE:
#endif				/* SR_SNMPv2 */

	    FreeOctetString(vb_ptr->value.os_value);
	    break;

#ifdef	SR_SNMPv2
	  case COUNTER_64_TYPE:
	    FreeUInt64(vb_ptr->value.uint64_value);
	    break;
	  case BIT_STRING_TYPE:
	    FreeBitString(vb_ptr->value.bitstring_value);
	    break;
#endif				/* SR_SNMPv2 */
	}
	free((char *) vb_ptr);
    }
}

/*
 * FreePdu: free the data structures allocated and built by calls to
 *          MakePdu().
 *
 * The former name (pre-snmp12.1.0.0 release) was free_pdu().
 */
void
FreePdu(pdu_ptr)
    Pdu            *pdu_ptr;
{
    if (pdu_ptr != NULL) {
	FreeOctetString(pdu_ptr->packlet);

#ifdef	SR_SNMPv1
	if (pdu_ptr->type == TRAP_TYPE) {
	    FreeOID(pdu_ptr->u.trappdu.enterprise);
	    FreeOctetString(pdu_ptr->u.trappdu.agent_addr);
	}
#endif				/* SR_SNMPv1 */

	FreeVarBindList(pdu_ptr->var_bind_list);
	free((char *) pdu_ptr);
    }
}

/*
 * FreeVarBindList: free the data structures allocated and built by calls to
 *                  MakeVarBindWithNull(), MakeVarBindWithValue(), etc.
 *
 * The former name (pre-snmp12.1.0.0 release) was free_varbind_list().
 */
void
FreeVarBindList(vb_ptr)
    VarBind        *vb_ptr;
{
    VarBind        *next;

    while (vb_ptr != NULL) {
	next = vb_ptr->next_var;
	FreeVarBind(vb_ptr);
	vb_ptr = next;
    }
}
