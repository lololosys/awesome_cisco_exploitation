/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_make_lib.c,v 3.2.62.3 1996/04/19 15:30:25 che Exp $
 * $Source: /release/112/cvs/Xsys/snmp/sr_make_lib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, SNMP Research
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_make_lib.c,v $
 * Revision 3.2.62.3  1996/04/19  15:30:25  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.2.72.2  1996/04/10  23:08:53  lol
 * Branch: IosSec_branch
 *         Incorporate code review comments.
 *
 * Revision 3.2.72.1  1996/04/09  21:47:08  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 3.2.62.2  1996/04/02  22:09:13  jjohnson
 * CSCdi53431:  Need mechanism to create empty octet strings
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/03/18  21:48:36  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  12:33:39  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  17:14:25  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:59:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:17:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:49:30  hampton
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
static char     rcsid[] = "make_lib.c,v 1.19 1995/02/06 18:11:16 battle Exp";
#endif				/* (! ( defined(lint) ) && defined(SR_RCSID)) */

/*
 * Revision History:
 *
 * make_lib.c,v
 * Revision 1.19  1995/02/06  18:11:16  battle
 * Changes for vrtx.
 *
 * Revision 1.18  1995/02/03  19:09:11  levi
 * Moved OID manipulation functions into oid_lib.c
 *
 * Revision 1.17  1994/10/28  16:37:37  vaughn
 * Placed new copyright in the file
 *
 * Revision 1.16  1994/10/26  21:51:40  vaughn
 * Changed copyrights to the new copyright structure
 *
 * Revision 1.15  1994/09/30  16:10:48  battle
 * hprt changes
 *
 * Revision 1.14  1994/09/30  14:05:46  pearson
 * added CmpOctetStrings()
 *
 * Revision 1.13  1994/09/19  13:39:15  levi
 * Removed #include "snmpuser.h" from files that don't need it.
 *
 * Revision 1.12  1994/09/19  13:28:44  levi
 * Moved CatOID from oid_lib.c
 *
 * Revision 1.11  1994/08/10  14:52:20  reid
 * removed #ifndef LIGHT from around MakeOctetStringFromHex and MakeOIDFromHex
 *
 * Revision 1.10  1994/08/04  15:37:27  battle
 * pSOS 2.0 changes
 *
 * Revision 1.9  1994/05/23  20:58:24  hecht
 * Added definition of isxdigit for Liebert agent. Also moved #ifndef LIGHT from
 * before MakeOctetStringFromHex routine to after it. It was needed by the new
 * SNMPv2 code that automatically generates SNMPv2 tables for embedded systems.
 *
 * Revision 1.8  1994/04/18  18:28:32  battle
 * pSOS changes
 *
 * Revision 1.7  1994/02/25  19:29:58  battle
 * Added (char *) in free() call.
 *
 * Revision 1.6  1994/02/24  15:55:23  reid
 * remove #ifndef SR_AGENT from around MakeVarBindWithValue
 *
 * Revision 1.5  1994/02/14  16:39:06  pearson
 * Changed copyright notice to be current for 1994, and added an
 * #ifdef SR_RCSID
 * around the static  declarations of rcsid in each file.
 *
 * AMP
 *
 * Revision 1.4  1994/02/11  22:24:49  levi
 * Added CloneObjectSyntax function.
 *
 * Revision 1.3  1993/12/23  16:54:00  battle
 * Added cvs headers to all .c files.
 *
 *
 *  2/6/89 JDC
 *  Amended copyright notice
 *  Professionalized error messages a bit
 *  Expanded comments and documentation
 *
 *  8/8/89 JDC
 *  calloc --> malloc for compat with MSC on PC
 *
 *  11/2/89 JDC
 *  set oid_ptr to NULL after freeing it in MakeOIDFromHex
 *
 *  11/8/89 JDC
 *  Make it print pretty via tgrind
 *
 *  01/08/90 JDC
 *  Added SILENT
 *
 *  09/03/90 JDC
 *  Port to FTP Software, Incorporated Version 2.04
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
 *  25-Apr-92 MRD
 *  Merged in new version of MakeVarBind for Version 11
 *
 *  13-June-92 JDC
 *  SMP
 *
 *  05-Jul-1992 JDC
 *  CloneOctetString()
 *
 *  31-Jul-1992 DLB
 *      Fixed malloc(0) returns NULL problem in MakeOctetStringFromText().
 *
 *  18-Aug-1992 DLB
 *      Fixed some more malloc(0) problems.
 *
 *  22-Sept-92 MGH
 *  Added the following routines: CloneVarBind(), CloneUInt64(),
 *  and CloneBitString().
 *
 *  23-Sept-92 MGH
 *  Changed the name of CloneVarBind() to CloneVarBindList().  Added a
 *  routine called CloneVarBind() that clones one varbind instead of a
 *  varbind_list. (i.e., like CloneVarBindList())
 *
 * 9-Nov-92 KWK
 *  Cleaned up MAX expression in CloneOID() malloc and null-ing of
 *  free'ed pointer in CloneOID() (latter has no real effect).
 *
 * 12-Nov-92 DLB
 *  Changed parameter names which conflicted with #defines for fields in
 *  ObjectSyntax union by prepending _.
 *
 * 8-Dec-1992 DLB
 *   Lint nits.
 *
 * 10-Dec-1992 DLB
 *   Fixed up mis-parening in MAX call in CloneOID().
 *
 * 6-Jan-1993 MGH
 * Added the U_INTEGER_32_TYPE to switch statements in MakeVarBind,
 * CloneVarBindList, and CloneVarBind, and make_varbind.
 *
 * 25-Mar-1993 DLB
 *   Added call to memset() after malloc of holey VarBind structure to
 *   shut purify up.
 *
 * 5-May-1993 MGH
 * Changed all LIB_ERROR* messages to DPRINTF messages.
 *
 * 28-May-1993 DAR
 * chaged parameter to MakeOctetStringFromText and MakeOctetStringFromHex from
 * unsigned char * to char *
 *
 * 01-Jun-1993 DLB
 * Added #include <memory.h>.
 *
 * 10-Jun-1993 DLB
 * Changed all short parameters to int.  Short parameters are a bad idea
 * for numerous reasons I won't go into here.
 *
 * 24-June-1993 MGH
 * Changed CloneVarBindList() to call CloneVarBind() for each varbind
 * in the list instead of having the same code in two routines.
 *
 * 19-July-93 MGH
 * Changed all references to SNMPD to SR_AGENT. Put a #ifndef SR_MGR block
 * around MakeVarBind() since it is only used by the agent.  Added the
 * MakeVarBindWithNull() and MakeVarBindWithValue routines, which will be
 * used by the command-line utilities instead of MakeVarBind().  Put a
 * #ifndef SR_AGENT block around the two new routines.  Made MakeVarBind()
 * check to see if instance, object, or object->oid is NULL.  Also had
 * MakeVarBindWithNull() and MakeVarBindWithValue routines check to see if
 * the object or instance parameter is NULL.
 *
 * 7-Oct-1993 KWK
 * Moved inet_addr() from make_lib.c to pkg\lib\internet\in_util.c
 * Add it's prototype to pkg\include\ip.h
 */

#include "master.h"
#include <ctype.h>




#include "sr_snmp.h"
#include "sr_diag.h"

/* Temporary redefinition of isxdigit - MGH 4/25/94 */

/*
 * MakeOctetString: This routine produces a library OctetString
 *          construct from a passed byte-string and length. This
 *          construct is usually passed to other library calls, such
 *          as MakeVarBindWithNull() or MakeAuthentication(), and
 *          linked into a larger library construct of an ASN.1 entity.
 *          FreeOctetString() recovers all memory malloc'ed by
 *          MakeOctetString, but should not be used if the octetstring
 *          is passed to another library routine. The free_*()
 *          counterparts to those routine free up all memory that has
 *          been linked to the higher level ASN.1 structure.
 *
 *   The former name (pre-snmp12.1.0.0 release) was make_octetstring().
 */

OctetString    *
MakeOctetString(string, length)
    unsigned char  *string;
    long            length;
{
    OctetString    *os_ptr;

    if (length > MAX_OCTET_STRING_SIZE) {
	DPRINTF((APTRACE, "MakeOctetString: Bad octetstring length: %d\n", length));
	return (NULL);
    }

    if ((os_ptr = (OctetString *) malloc(sizeof(OctetString))) == NULL) {
	DPRINTF((APWARN, "MakeOctetString, os_ptr malloc\n"));
	return (NULL);
    }

    os_ptr->octet_ptr = NULL;

    if ((os_ptr->octet_ptr = (unsigned char *) malloc((unsigned) MAX(length, 1)))
	== NULL) {
	DPRINTF((APWARN, "MakeOctetString, octet_ptr malloc\n"));
	FreeOctetString(os_ptr);
	NULLIT(os_ptr);
	return (NULL);
    }

    if (string)
	memcpy((char *) os_ptr->octet_ptr, (char *) string, (int) length);
    os_ptr->length = length;

    return (os_ptr);
}				/* end of MakeOctetString */

/*
 * CloneObjectSyntax:
 *
 * Make a copy of an ObjectSyntax structure.
 */
ObjectSyntax   *
CloneObjectSyntax(os)
    ObjectSyntax   *os;
{
    ObjectSyntax   *new_os;

    if (os == NULL) {
	return (NULL);
    }

    new_os = (ObjectSyntax *) malloc(sizeof(ObjectSyntax));
    if (new_os == NULL) {
	return (NULL);
    }
    new_os->type = os->type;

    switch (os->type) {
      case INTEGER_TYPE:
	new_os->sl_value = os->sl_value;
	break;
      case COUNTER_TYPE:
      case GAUGE_TYPE:
      case TIME_TICKS_TYPE:
	new_os->ul_value = os->ul_value;
	break;
      case OCTET_PRIM_TYPE:
      case IP_ADDR_PRIM_TYPE:
      case OPAQUE_PRIM_TYPE:
	new_os->os_value = CloneOctetString(os->os_value);
	break;
      case NULL_TYPE:
	break;
      case OBJECT_ID_TYPE:
	new_os->oid_value = CloneOID(os->oid_value);
	break;
      case SEQUENCE_TYPE:
	break;

#ifdef SR_SNMPv2
      case BIT_STRING_TYPE:
	new_os->bitstring_value = CloneBitString(os->bitstring_value);
	break;
      case NSAP_ADDR_TYPE:
	new_os->ul_value = os->ul_value;
	break;
      case COUNTER_64_TYPE:
	new_os->uint64_value = CloneUInt64(os->uint64_value);
	break;
#endif				/* SR_SNMPv2 */

      default:
	free((char *) new_os);
	new_os = NULL;
    }

    return (new_os);
}

/* this routine compares two octet strings (which cannot be NULL pointers)
 * and returns zero if they are equal in length and values, and a "difference"
 * value similar to strcmp()'s if they are not. */
int
CmpOctetStrings(o1, o2)
    OctetString    *o1, *o2;
{
    long            i, n;

    n = MIN(o1->length, o2->length);
    for (i = 0; i < n; i++) {
	if (o1->octet_ptr[i] != o2->octet_ptr[i]) {
	    return o1->octet_ptr[i] - o2->octet_ptr[i];
	}
    }
    return o1->length - o2->length;
}

/*
 * CloneOctetString:
 *
 * The former name (pre-snmp12.1.0.0 release) was clone_octetstring().
 */
OctetString    *
CloneOctetString(os_ptr)
    OctetString    *os_ptr;
{
    return (MakeOctetString(os_ptr->octet_ptr, os_ptr->length));
}

#ifdef	SR_SNMPv2
/*
 * MakeBitString:
 *
 * The former name (pre-snmp12.1.0.0 release) was make_bitstring().
 */
BitString      *
MakeBitString(string, length, unused)
    unsigned char  *string;
    long            length;
    int             unused;
{
    BitString      *bitstring_ptr;

    if ((length > MAX_OCTET_STRING_SIZE - 1) || (length < 0)) {
	DPRINTF((APTRACE, "MakeBitString: Bad bitstring length: %d\n", length));
	return (NULL);
    }

    if ((unused < 0) || (unused > 7)) {
	DPRINTF((APTRACE, "MakeBitString: Bad bitstring unused count: %d\n", unused));
	return (NULL);
    }

    if ((bitstring_ptr = (BitString *) malloc(sizeof(BitString))) == NULL) {
	DPRINTF((APWARN, "MakeBitString, bitstring_ptr malloc\n"));
	return (NULL);
    }

    bitstring_ptr->octet_ptr = NULL;

    if ((bitstring_ptr->octet_ptr = (unsigned char *)
	 malloc((unsigned) MAX(length, 1))) == NULL) {
	DPRINTF((APWARN, "MakeBitString, octet_ptr malloc\n"));
	FreeBitString(bitstring_ptr);
	NULLIT(bitstring_ptr);
	return (NULL);
    }

    bitstring_ptr->unused = unused;
    bitstring_ptr->length = length;

    memcpy((char *) bitstring_ptr->octet_ptr, (char *) string, (int) length);

    return (bitstring_ptr);
}				/* end of MakeBitString */

#endif				/* SR_SNMPv2 */


/*
 * MakeOID: This routine produces a library Object Identifier
 *          construct from a passed sub-identifier array and length.
 *          The sub-identifier array is an array of unsigned long
 *          integers, with each element of the array corresponding to
 *          the value of each integer at each position in the
 *          dot-notation display of an object identifier. For example,
 *          the Object Identifier 1.3.6.1.2.1.1 would have a value 1
 *          in sid_array[0], 3 in sid_array[1], etc. length is the
 *          number of sub-identifier elements present in the array.
 *          The construct returned can be free'ed by passing the
 *          pointer to FreeOID(). This is usually unnecessary as the
 *          construct is most often passed to another library routine
 *          for inclusion in a large ASN.1 construct and that library
 *          routine's free'ing counterpart will do the memory
 *          recovery.
 *
 * The former name (pre-snmp12.1.0.0 release) was make_oid().
 */

OID            *
MakeOID(sid, length)
    unsigned long   sid[];
    int             length;
{
    OID            *oid_ptr;
    int             i;

    if (length > MAX_OID_SIZE) {
	DPRINTF((APTRACE, "MakeOID: Bad sid string length: %d\n", length));
	return (NULL);
    }

    if ((oid_ptr = (OID *) malloc(sizeof(OID))) == NULL) {
	DPRINTF((APWARN, "MakeOID, oid_ptr malloc\n"));
	return (NULL);
    }

    oid_ptr->oid_ptr = NULL;

    if ((oid_ptr->oid_ptr = (unsigned long *)
       malloc((unsigned) MAX(length * sizeof(unsigned long), 1))) == NULL) {
	DPRINTF((APWARN, "MakeOID, oid_ptr->oid_ptr malloc\n"));
	FreeOID(oid_ptr);
	NULLIT(oid_ptr);
	return (NULL);
    }

    for (i = 0; i < length; i++)
	oid_ptr->oid_ptr[i] = (unsigned long) sid[i];
    oid_ptr->length = length;

    return (oid_ptr);
}				/* end of MakeOID */


#ifndef SR_MGR
/*
 * MakeVarBind:
 *
 * NOTE: This is the "new" version of make_varbind, MakeVarBind. It
 * differs in the parameters passed to it but performs the same
 * task. The "old" version will be maintained for a short time to
 * allow for conversion.
 *
 * This MakeVarBind is used by the agent method routines, while
 * MakeVarBindWithNull and MakeVarBindWithValue are used by the
 * manager routines.
 *
 * The former name (pre-snmp12.1.0.0 release) was MakeVarbind().
 */
VarBind        *
MakeVarBind(object, instance, value)
    ObjectInfo     *object;
    OID            *instance;
    void           *value;
{
    VarBind        *vb;

    if (object == NULL) {
	DPRINTF((APWARN, "MakeVarBind: Null object parameter.\n"));
	return (NULL);
    }
    if (&object->oid == (OID *) NULL) {
	DPRINTF((APWARN, "MakeVarBind: object->oid is NULL.\n"));
	return (NULL);
    }
    if ((vb = (VarBind *) malloc(sizeof(VarBind))) == NULL) {
	DPRINTF((APWARN, "MakeVarBind, vb malloc\n"));
	return (NULL);
    }
    memset((char *) vb, 0, sizeof(VarBind));

    if (instance != NULL) {
	vb->name = CatOID(&object->oid, instance);	/* name */
    }
    else {
	vb->name = CloneOID(&object->oid);	/* name */
    }
    if (vb->name == NULL) {
	DPRINTF((APWARN, "MakeVarBind: cannot malloc vb->name.\n"));
	return (NULL);
    }
    vb->name_size = FindLenOID(vb->name);
    vb->value.type = object->oidtype;

    /* combine the Choices from simple and application */
    switch (object->oidtype) {
      case COUNTER_TYPE:	/* includes COUNTER_32_TYPE */
      case GAUGE_TYPE:		/* includes GAUGE_32_TYPE */
      case TIME_TICKS_TYPE:

#ifdef SR_SNMPv2
      case U_INTEGER_32_TYPE:
#endif				/* SR_SNMPv2 */

	vb->value.ul_value = *((unsigned long *) value);
	vb->value_size = FindLenUInt(vb->value.ul_value);
	break;
      case INTEGER_TYPE:	/* includes INTEGER_32_TYPE */
	vb->value.sl_value = *((long *) value);
	vb->value_size = FindLenInt(vb->value.sl_value);
	break;
      case OBJECT_ID_TYPE:
	vb->value.oid_value = (OID *) value;
	vb->value_size = FindLenOID(vb->value.oid_value);
	break;
      case OCTET_PRIM_TYPE:
      case IP_ADDR_PRIM_TYPE:
      case OPAQUE_PRIM_TYPE:

#ifdef	SR_SNMPv2
      case NSAP_ADDR_TYPE:
#endif				/* SR_SNMPv2 */

	vb->value.os_value = (OctetString *) value;
	vb->value_size = FindLenOctetString(vb->value.os_value);
	break;

#ifdef	SR_SNMPv2
      case BIT_STRING_TYPE:
	vb->value.bitstring_value = (BitString *) value;
	vb->value_size = FindLenBitString(vb->value.bitstring_value);
	break;
#endif				/* SR_SNMPv2 */

      case NULL_TYPE:

#ifdef	SR_SNMPv2
      case NO_SUCH_OBJECT_EXCEPTION:
      case NO_SUCH_INSTANCE_EXCEPTION:
      case END_OF_MIB_VIEW_EXCEPTION:
#endif				/* SR_SNMPv2 */

	vb->value_size = 2;
	break;

#ifdef	SR_SNMPv2
      case COUNTER_64_TYPE:
	vb->value.uint64_value = (UInt64 *) value;
	vb->value_size = FindLenUInt64(vb->value.uint64_value);
	break;
#endif				/* SR_SNMPv2 */

      default:
	DPRINTF((APWARN, "MakeVarBind: Illegal type: 0x%x\n", object->oidtype));
	FreeVarBind(vb);
	return (NULL);
	break;
    };				/* end of switch */

    return (vb);
}				/* MakeVarBind() */

#endif				/* SR_MGR */

/*
 * MakeVarBindWithValue: This routine is used by a manager (ex: command-line
 *                       utilities) to make a varbind given the name and
 *                       value.  This routine should return a pointer to the
 *                       new varbind on success and NULL on failure.
 *
 * arguments: object - the object class portion of the variable name.
 *            instance - the object instance portion of the variable name.
 *            type - the ASN.1 type of this variable.
 *            value - the value for this variable.
 *
 * returns: the newly created varbind on success, NULL on failure.
 *
 */
VarBind        *
MakeVarBindWithValue(object, instance, type, value)
    OID            *object;
    OID            *instance;
    int             type;
    void           *value;
{
    VarBind        *vb;

    if (object == NULL) {
	DPRINTF((APWARN, "MakeVarBindWithValue: Null object OID.\n"));
	return (NULL);
    }

    if ((vb = (VarBind *) malloc(sizeof(VarBind))) == NULL) {
	DPRINTF((APWARN, "MakeVarBindWithValue, vb malloc\n"));
	return (NULL);
    }
    memset((char *) vb, 0, sizeof(VarBind));

    if (instance != NULL) {
	vb->name = CatOID(object, instance);	/* name */
    }
    else {
	vb->name = CloneOID(object);	/* name */
    }
    if (vb->name == NULL) {
	DPRINTF((APWARN, "MakeVarBindWithValue: cannot malloc vb->name.\n"));
	return (NULL);
    }
    vb->name_size = FindLenOID(vb->name);
    vb->value.type = (short) type;

    /* combine the Choices from simple and application */
    switch (type) {
      case COUNTER_TYPE:	/* includes COUNTER_32_TYPE */
      case GAUGE_TYPE:		/* includes GAUGE_32_TYPE */
      case TIME_TICKS_TYPE:

#ifdef SR_SNMPv2
      case U_INTEGER_32_TYPE:
#endif				/* SR_SNMPv2 */

	vb->value.ul_value = *((unsigned long *) value);
	vb->value_size = FindLenUInt(vb->value.ul_value);
	break;
      case INTEGER_TYPE:	/* includes INTEGER_32_TYPE */
	vb->value.sl_value = *((long *) value);
	vb->value_size = FindLenInt(vb->value.sl_value);
	break;
      case OBJECT_ID_TYPE:
	vb->value.oid_value = (OID *) value;
	vb->value_size = FindLenOID(vb->value.oid_value);
	break;
      case OCTET_PRIM_TYPE:
      case IP_ADDR_PRIM_TYPE:
      case OPAQUE_PRIM_TYPE:

#ifdef	SR_SNMPv2
      case NSAP_ADDR_TYPE:
#endif				/* SR_SNMPv2 */

	vb->value.os_value = (OctetString *) value;
	vb->value_size = FindLenOctetString(vb->value.os_value);
	break;

#ifdef	SR_SNMPv2
      case BIT_STRING_TYPE:
	vb->value.bitstring_value = (BitString *) value;
	vb->value_size = FindLenBitString(vb->value.bitstring_value);
	break;
#endif				/* SR_SNMPv2 */

      case NULL_TYPE:

#ifdef	SR_SNMPv2
      case NO_SUCH_OBJECT_EXCEPTION:
      case NO_SUCH_INSTANCE_EXCEPTION:
      case END_OF_MIB_VIEW_EXCEPTION:
#endif				/* SR_SNMPv2 */

	vb->value_size = 2;
	break;

#ifdef	SR_SNMPv2
      case COUNTER_64_TYPE:
	vb->value.uint64_value = (UInt64 *) value;
	vb->value_size = FindLenUInt64(vb->value.uint64_value);
	break;
#endif				/* SR_SNMPv2 */

      default:
	DPRINTF((APWARN, "MakeVarBindWithValue: Illegal type: 0x%x\n", type));
	FreeVarBind(vb);
	return (NULL);
	break;
    };				/* end of switch */

    return (vb);
}				/* MakeVarBindWithValue() */


/*
 * MakeVarBindWithNull: This routine is used by a manager (ex: command-line
 *                      utilities) to make a varbind given the name
 *                      The type field is set to NULL_TYPE and the value
 *                      field is set to NULL.  This should only be called
 *                      when doing a get, get-next, or get-bulk request.
 *                      This routine should return a pointer to the
 *                      new varbind on success and NULL on failure.
 *
 * arguments: object - the object class portion of the variable name.
 *            instance - the object instance portion of the variable name.
 *
 * returns: the newly created varbind on success,
 *          NULL on failure.
 *
 */
VarBind        *
MakeVarBindWithNull(object, instance)
    OID            *object;
    OID            *instance;
{
    VarBind        *vb;

    if (object == NULL) {
	DPRINTF((APWARN, "MakeVarBindWithNull: Null object OID.\n"));
	return (NULL);
    }

    if ((vb = (VarBind *) malloc(sizeof(VarBind))) == NULL) {
	DPRINTF((APWARN, "MakeVarBindWithNull, vb malloc\n"));
	return (NULL);
    }
    memset((char *) vb, 0, sizeof(VarBind));

    if (instance != NULL) {
	vb->name = CatOID(object, instance);	/* name */
    }
    else {
	vb->name = CloneOID(object);	/* name */
    }
    if (vb->name == NULL) {
	DPRINTF((APWARN, "MakeVarBindWithNull: cannot malloc vb->name.\n"));
	return (NULL);
    }
    vb->name_size = FindLenOID(vb->name);
    vb->value.type = NULL_TYPE;

    vb->value_size = 2;		/* one for the type 0x05, and */
    /* one for the length 0x00 */

    return (vb);
}				/* MakeVarBindWithNull() */

/*
 * make_varbind
 *
 *   NOTE: This has been replaced by MakeVarBind for agent method
 *   routines and MakeVarBindWithNull and MakeVarBindWithValue for
 *   manager routines.  This routine will be supported for the
 *   present, but will eventually be deprecated with a later release.
 *
 *   This routine is called to create a var_bind entry to be strung
 *   onto a PDU.  It returns a pointer to a malloc'ed data structure
 *   of the type VarBind.  This pointer is usually then used in a call
 *   to LinkVarBind() to associate this var_bind with a PDU.  The
 *   structure is free'ed when the PDU is free'ed with a call to
 *   FreePdu().
 *
 *   A var_bind is a variable-value binding.  It binds the name, the
 *   type, and the value of a variable into one construct.  The
 *   current list of SNMP types are and which of the calling variables
 *   to set the value of:
 *
 *   COUNTER_TYPE:  uses ul_value
 *   GAUGE_TYPE:  uses ul_value
 *   INTEGER_TYPE: uses sl_value
 *   TIME_TICKS_TYPE: uses ul_value
 *   OBJECT_ID_TYPE: uses oid_value
 *   OCTET_PRIM_TYPE: uses os_value
 *   IP_ADDR_PRIM_TYPE: uses os_value
 *   OPAQUE_PRIM_TYPE: uses os_value
 *   NULL_TYPE:  no passed value needed
 *
 */


VarBind        *
make_varbind(oid, type, _ul_value, _sl_value, _os_value, _oid_value)
    OID            *oid;
    int             type;
    unsigned long   _ul_value;
    long            _sl_value;
    OctetString    *_os_value;
    OID            *_oid_value;
{
    VarBind        *vb_ptr;

    if ((vb_ptr = (VarBind *) malloc(sizeof(VarBind))) == NULL) {
	DPRINTF((APWARN, "make_varbind, vb_ptr malloc\n"));
	return (NULL);
    }

    vb_ptr->name = oid;		/* name */
    vb_ptr->name_size = FindLenOID(vb_ptr->name);
    vb_ptr->value.type = type;
    vb_ptr->value.ul_value = 0L;
    vb_ptr->value.sl_value = 0L;
    vb_ptr->value.oid_value = NULL;
    vb_ptr->value.os_value = NULL;

#ifdef SR_SNMPv2
    vb_ptr->value.uint64_value = NULL;
    vb_ptr->value.bitstring_value = NULL;
#endif				/* SR_SNMPv2 */

    vb_ptr->next_var = NULL;
    vb_ptr->data_length = 0L;

    /* combine the Choices from simple and application */
    switch (type) {
      case COUNTER_TYPE:	/* includes COUNTER_32_TYPE */
      case GAUGE_TYPE:		/* includes GAUGE_32_TYPE */
      case TIME_TICKS_TYPE:

#ifdef SR_SNMPv2
      case U_INTEGER_32_TYPE:
#endif				/* SR_SNMPv2 */

	vb_ptr->value.ul_value = _ul_value;
	vb_ptr->value_size = FindLenUInt(vb_ptr->value.ul_value);
	break;
      case INTEGER_TYPE:	/* includes INTEGER_32_TYPE */
	vb_ptr->value.sl_value = _sl_value;
	vb_ptr->value_size = FindLenInt(vb_ptr->value.sl_value);
	break;
      case OBJECT_ID_TYPE:
	vb_ptr->value.oid_value = _oid_value;
	vb_ptr->value_size = FindLenOID(vb_ptr->value.oid_value);
	break;
      case OCTET_PRIM_TYPE:
      case IP_ADDR_PRIM_TYPE:

#ifdef	SR_SNMPv2
      case NSAP_ADDR_TYPE:
      case BIT_STRING_TYPE:
#endif				/* SR_SNMPv2 */

      case OPAQUE_PRIM_TYPE:
	vb_ptr->value.os_value = _os_value;
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

      default:
	DPRINTF((APTRACE, "make_varbind: Illegal type: 0x%x\n", type));
	FreeVarBind(vb_ptr);
	NULLIT(vb_ptr);
	return (NULL);
	break;
    };				/* end of switch */

    return (vb_ptr);
}				/* end of  make_varbind() */


/*
 * MakePdu:
 *
 *   This routine is called to create the initial header block for
 *   building the SNMP ASN.1 data structure, which upon completion is
 *   used to build the actual SNMP packet.  It returns a pointer to a
 *   malloc'ed data structure of type Pdu:
 *
 *   typedef struct _Pdu {
 *     OctetString *packlet;
 *     short type;
 *     union {
 *       NormPdu normpdu;
 *       TrapPdu trappdu;
 *     } u;
 *     VarBind *var_bind_list;
 *     VarBind *var_bind_end_ptr;
 *   } Pdu;
 *
 *   The type is one of GET_REQUEST_TYPE, GET_NEXT_REQUEST_TYPE,
 *   GET_RESPONSE_TYPE, SET_REQUEST_TYPE, GET_BULK_REQUEST,
 *   INFORM_REQUEST_TYPE, SNMPv2_TRAP_TYPE, or TRAP_TYPE.  The request
 *   ID is the id number assigned to the particular packet by the
 *   application.  Since the application is UDP based, retry is solely
 *   controlled by the network management application.  The
 *   error_status is set to other than 0 in GET_RESPONSE_TYPE only to
 *   indicate that this response is in reply to a bad request.  The
 *   error_index is used only by GET_RESPONSE_TYPE and points to the
 *   var-bind entry in the PDU that offends the agent.  The enterprise
 *   is used by the TRAP_TYPE pdu and is an object identifier
 *   associated with the trap generating entity.  The agent_addr is
 *   used by the TRAP_TYPE pdu and consists of an octet string
 *   containing the IP address of the trap generating entity.  The
 *   generic_trap and specific_trap are used by the TRAP_TYPE pdu and
 *   consist of integers that indicate which type of trap this PDU
 *   represents.  The time_ticks is the TRAP_TYPE emiting entity's
 *   sense of time since the agent has restarted.
 *
 *   This routine is called once for each packet to be generated.  The
 *   pdu pointer is then passed to the routine LinkVarBind repeatedly
 *   to string var_binds into the packet.  Build_pdu() is then called
 *   to perform the ASN.1 encoding of the pdu and place that result in
 *   the pdu pointer's *packlet field.  After the packlet has been
 *   wrapped in an authentication envelope, it is free'ed by passing
 *   the pointer to FreePdu().
 *
 *
 *   The former name (pre-snmp12.1.0.0 release) was make_pdu().
 */

Pdu            *
MakePdu(type, request_id, error_status_or_non_repeaters,
	error_index_or_max_repetitions, enterprise,
	agent_addr, generic_trap, specific_trap, time_ticks)
    int             type;
    long            request_id;
    long            error_status_or_non_repeaters;
    long            error_index_or_max_repetitions;
    OID            *enterprise;	/* trap stuff */
    OctetString    *agent_addr;	/* trap stuff */
    long            generic_trap;	/* trap stuff */
    long            specific_trap;	/* trap stuff */
    unsigned long   time_ticks;	/* trap stuff */
{
    Pdu            *pdu_ptr;

    if ((pdu_ptr = (Pdu *) malloc(sizeof(Pdu))) == NULL) {
	DPRINTF((APWARN, "MakePdu, pdu_ptr malloc\n"));
	return (NULL);
    }

    pdu_ptr->type = type;
    pdu_ptr->num_varbinds = 0;
    pdu_ptr->packlet = NULL;

#ifdef SR_SNMPv1
    pdu_ptr->u.trappdu.enterprise = NULL;
    pdu_ptr->u.trappdu.agent_addr = NULL;
#endif				/* SR_SNMPv1 */

    pdu_ptr->var_bind_list = NULL;
    pdu_ptr->var_bind_end_ptr = NULL;

    /* sanity check type */
    switch (type) {
      case GET_REQUEST_TYPE:
      case GET_NEXT_REQUEST_TYPE:
      case GET_RESPONSE_TYPE:
      case SET_REQUEST_TYPE:

#ifdef	SR_SNMPv2
      case GET_BULK_REQUEST_TYPE:
      case INFORM_REQUEST_TYPE:
      case SNMPv2_TRAP_TYPE:
#endif				/* SR_SNMPv2 */

	pdu_ptr->u.normpdu.request_id = request_id;
	/* since unions are the same shape, can get away with the following: */
	pdu_ptr->u.normpdu.error_status = error_status_or_non_repeaters;
	pdu_ptr->u.normpdu.error_index = error_index_or_max_repetitions;

	break;

#ifdef	SR_SNMPv1
      case TRAP_TYPE:
	/* sanity check octet strings */
	if (enterprise == NULL) {
	    DPRINTF((APTRACE, "MakePdu: Trap type but no Enterprise.\n"));
	    FreePdu(pdu_ptr);
	    NULLIT(pdu_ptr);
	    return (NULL);
	}
	if (agent_addr == NULL) {
	    DPRINTF((APTRACE, "MakePdu: Trap type but no Agent_addr.\n"));
	    FreePdu(pdu_ptr);
	    NULLIT(pdu_ptr);
	    return (NULL);
	}
	pdu_ptr->u.trappdu.enterprise = enterprise;
	pdu_ptr->u.trappdu.agent_addr = agent_addr;
	pdu_ptr->u.trappdu.generic_trap = generic_trap;
	pdu_ptr->u.trappdu.specific_trap = specific_trap;
	pdu_ptr->u.trappdu.time_ticks = time_ticks;
	break;
#endif				/* SR_SNMPv1 */

      default:
	DPRINTF((APTRACE, "MakePdu: illegal type: 0x%x\n", type));
	FreePdu(pdu_ptr);
	NULLIT(pdu_ptr);
	return (NULL);
	break;
    };				/* end of switch */

    return (pdu_ptr);
}				/* end of make_normpdu */


/*
 * LinkVarBind:
 *
 *   This routine adds the varbind entry created by a call to
 *   make_varbind() to a PDU previously started by a call to MakePdu()
 *   to flesh out the PDU.  This should only be called once for each
 *   varbind_ptr being associated with the PDU as it is this
 *   association that is used to reclaim memory when the PDU is
 *   free'ed.
 *
 *   The former name (pre-snmp12.1.0.0 release) was link_varbind().
 */

/*
 * Thread a vb_ptr onto the end of the list
 */
short
LinkVarBind(pdu_ptr, vb_ptr)
    Pdu            *pdu_ptr;
    VarBind        *vb_ptr;
{
    if (vb_ptr == NULL) {
	DPRINTF((APTRACE, "LinkVarBind: Trying to add Null Variable\n"));
	return (-1);
    }

    /* first variable? */
    if (pdu_ptr->var_bind_end_ptr == NULL) {
	pdu_ptr->var_bind_list = vb_ptr;
	pdu_ptr->var_bind_end_ptr = vb_ptr;
	return (0);
    }

    /* nope, so just add to end of list and update end ptr */
    pdu_ptr->var_bind_end_ptr->next_var = vb_ptr;
    pdu_ptr->var_bind_end_ptr = vb_ptr;
    return (0);
}				/* end of LinkVarBind */

/*
 * MakeOctetStringFromText:
 *
 *   This routine is used to create a library format octet string data
 *   structure for use by calls to make_varbind() and
 *   MakeAuthentication() from text strings.
 *
 * The former name (pre-snmp12.1.0.0 release) was make_octet_from_text().
 */

OctetString    *
MakeOctetStringFromText(text_str)
    char           *text_str;
{
    long            i;
    OctetString    *os_ptr;

    if ((os_ptr = (OctetString *) malloc(sizeof(OctetString))) == NULL) {
	DPRINTF((APWARN, "MakeOctetStringFromText, os_ptr malloc\n"));
	return (NULL);
    }

    os_ptr->octet_ptr = NULL;
    os_ptr->length = strlen((char *) text_str);

    if ((os_ptr->octet_ptr = (unsigned char *)
	 malloc((unsigned) MAX(os_ptr->length, 1))) == NULL) {
	DPRINTF((APWARN, "MakeOctetStringFromText, octet_ptr malloc\n"));
	FreeOctetString(os_ptr);
	NULLIT(os_ptr);
	return (NULL);
    }

    for (i = 0; i < os_ptr->length; i++) {
	os_ptr->octet_ptr[i] = text_str[i];
    }
    return (os_ptr);
}


 /*
  * MakeLenOID: MakeLenOID allocates space for the incoming oid plus one to
  * contain the length. The length is the first element of the oid, and the
  * rest is copied from the incoming oid.  The new oid is then returned.
  * 
  * The former name (pre-snmp12.1.0.0 release) was make_len_oid().
  */
OID            *
MakeLenOID(oida)
    OID            *oida;
{
    OID            *oid;
    int             i, length;

    if (oida == (OID *) NULL) {
	DPRINTF((APWARN, "MakeLenOID: passed in null oid\n"));
	return (NULL);
    }

    if (((length = oida->length) + 1) > MAX_OID_SIZE) {
	DPRINTF((APTRACE, "MakeLenOID: Too long: %d\n", length + 1));
	return (NULL);
    }

    if (length == 0) {
	DPRINTF((APTRACE, "MakeLenOID: Zero length OID\n"));
	return (NULL);
    }

    if ((oid = (OID *) malloc(sizeof(OID))) == NULL) {
	DPRINTF((APWARN, "MakeLenOID: cannot malloc oid\n"));
	return (NULL);
    }

    if (!(oid->oid_ptr =
	  (unsigned long *) malloc((unsigned) ((length + 1) * sizeof(unsigned long))))) {
	DPRINTF((APWARN, "MakeLenOID: cannot malloc oid_ptr\n"));
	free((char *) oid);
	return (NULL);
    }

    oid->oid_ptr[0] = length;

    for (i = 0; i < length; i++) {
	oid->oid_ptr[i + 1] = oida->oid_ptr[i];
    }

    oid->length = length + 1;

    return (oid);
}

 /*
  * MakeOIDSubLen: allocates space for the incoming oid minus one.  The first
  * element is ignored, and a new oid is made equal to all the remaining
  * values. This new oid is then returned.
  * 
  * The former name (pre-snmp12.1.0.0 release) was make_oid_sub_len().
  */
OID            *
MakeOIDSubLen(oida)
    OID            *oida;
{
    OID            *oid;
    int             i, length;

    if (oida == (OID *) NULL) {
	DPRINTF((APTRACE, "MakeOIDSubLen:  A null OID was passed in\n"));
	return (NULL);
    }
    if (((length = oida->length) - 1) > MAX_OID_SIZE) {
	DPRINTF((APTRACE, "MakeOIDSubLen: Too long %d\n", length - 1));
	return (NULL);
    }

    if (length <= 1) {
	DPRINTF((APTRACE, "MakeOIDSubLen: Cannot make Zero-Length OID\n"));
	return (NULL);
    }

    if ((oid = (OID *) malloc(sizeof(OID))) == NULL) {
	DPRINTF((APWARN, "MakeOIDSubLen: cannot malloc oid\n"));
	return (NULL);
    }

    if ((oid->oid_ptr =
	 (unsigned long *)
	 malloc((unsigned) (MAX((length - 1) * sizeof(unsigned long), 1)))) == NULL) {
	DPRINTF((APWARN, "MakeOIDSubLen: cannot malloc oid_ptr\n"));
	free((char *) oid);
	return (NULL);
    }

    for (i = 1; i < length; i++) {
	oid->oid_ptr[i - 1] = oida->oid_ptr[i];
    }

    oid->length = length - 1;
    return (oid);
}

/*
 * CloneVarBindList: clones the incoming varbind list and returns a pointer
 *                   to the new copy of the varbind list.
 *
 * Arguments: in_vb - The varbind list to be cloned.
 *
 * Returns: Pointer to the new copy of the varbind list on success,
 *          NULL on failure.
 *
 * The former name (pre-snmp12.1.0.0 release) was clone_varbind_list().
 */
VarBind        *
CloneVarBindList(in_vb)
    VarBind        *in_vb;
{

    VarBind        *out_vb;
    VarBind        *return_vb;
    VarBind        *cur_vb;
    VarBind        *vb;

    return_vb = NULL;
    out_vb = NULL;
    cur_vb = NULL;
    vb = NULL;

    if (in_vb == NULL) {
	DPRINTF((APTRACE, "CloneVarBindList: value passed in is NULL\n"));
	return (NULL);
    }
    vb = in_vb;

    while (vb != NULL) {
	if ((out_vb = CloneVarBind(vb)) == NULL) {
	    DPRINTF((APWARN, "CloneVarBindList:  NULL return from CloneVarBind.\n"));
	    FreeVarBindList(return_vb);
	    return_vb = NULL;
	    return (NULL);
	}
	/*
	 * We want return_vb to point to the beginning of the varbind list.
	 */
	if (return_vb == NULL) {
	    return_vb = out_vb;
	}
	if (cur_vb == NULL) {
	    cur_vb = out_vb;
	}
	else {
	    cur_vb->next_var = out_vb;
	    cur_vb = out_vb;
	}

	vb = vb->next_var;
    }				/* while (vb != NULL) */

    return (return_vb);

}				/* CloneVarBindList() */

/*
 * CloneVarBind: clones the incoming varbind and returns a pointer to
 *               the new copy of the varbind.
 *
 * Arguments: in_vb - The varbind to be cloned.
 *
 * Returns: Pointer to the new copy of the varbind on success,
 *          NULL on failure.
 *
 * The former name (pre-snmp12.1.0.0 release) was clone_varbind().
 */
VarBind        *
CloneVarBind(in_vb)
    VarBind        *in_vb;
{

    VarBind        *out_vb;
    VarBind        *vb;


    if (in_vb == NULL) {
	DPRINTF((APTRACE, "CloneVarBind: value passed in is NULL\n"));
	return (NULL);
    }
    vb = in_vb;

    if ((out_vb = (VarBind *) malloc(sizeof(VarBind))) == NULL) {
	DPRINTF((APWARN, "CloneVarBind: Cannot malloc out_vb\n"));
	return (NULL);
    }

    out_vb->data_length = vb->data_length;
    out_vb->name_size = vb->name_size;
    out_vb->value_size = vb->value_size;
    out_vb->value.type = vb->value.type;
    out_vb->value.ul_value = 0L;
    out_vb->value.sl_value = 0L;
    out_vb->value.oid_value = NULL;
    out_vb->value.os_value = NULL;

#ifdef SR_SNMPv2
    out_vb->value.uint64_value = NULL;
    out_vb->value.bitstring_value = NULL;
#endif				/* SR_SNMPv2 */

    out_vb->next_var = NULL;

    if ((out_vb->name = (OID *) CloneOID(vb->name)) == NULL) {
	DPRINTF((APWARN, "CloneVarBind: Unable to clone vb->name\n"));
	FreeVarBind(out_vb);
	out_vb = NULL;
	return (NULL);
    }

    /*
     * Assign value for the varbind
     */

    switch (vb->value.type) {

      case COUNTER_TYPE:
      case GAUGE_TYPE:
      case TIME_TICKS_TYPE:	/* unsigned integers */

#ifdef SR_SNMPv2
      case U_INTEGER_32_TYPE:
#endif				/* SR_SNMPv2 */

	out_vb->value.ul_value = vb->value.ul_value;
	break;
      case INTEGER_TYPE:	/* signed integers, including INTEGER_32 */
	out_vb->value.sl_value = vb->value.sl_value;
	break;
      case OBJECT_ID_TYPE:	/* OIDs */
	if ((out_vb->value.oid_value =
	     (OID *) CloneOID(vb->value.oid_value)) == (OID *) NULL) {
	    DPRINTF((APWARN,
		     "CloneVarBind: Unable to clone vb->value.oid_value\n"));
	    FreeVarBind(out_vb);
	    out_vb = NULL;
	    return (NULL);
	}
	break;

#ifdef SR_SNMPv2
      case COUNTER_64_TYPE:	/* big unsigned integer */
	if ((out_vb->value.uint64_value =
	     CloneUInt64(vb->value.uint64_value)) ==
	    (UInt64 *) NULL) {
	    DPRINTF((APWARN,
		 "CloneVarBind: Unable to clone vb->value.uint64_value\n"));
	    FreeVarBind(out_vb);
	    out_vb = NULL;
	    return (NULL);
	}
	break;
      case BIT_STRING_TYPE:
	if ((out_vb->value.bitstring_value =
	     CloneBitString(vb->value.bitstring_value)) ==
	    (BitString *) NULL) {
	    DPRINTF((APWARN,
	      "CloneVarBind: Unable to clone vb->value.bitstring_value\n"));
	    FreeVarBind(out_vb);
	    out_vb = NULL;
	    return (NULL);
	}
	break;
#endif				/* SR_SNMPv2 */

      case OCTET_PRIM_TYPE:
      case IP_ADDR_PRIM_TYPE:
      case OPAQUE_PRIM_TYPE:

#ifdef SR_SNMPv2
      case NSAP_ADDR_TYPE:
#endif				/* SR_SNMPv2 */

	if ((out_vb->value.os_value =
	     (OctetString *) CloneOctetString(vb->value.os_value)) ==
	    (OctetString *) NULL) {
	    DPRINTF((APWARN,
		     "CloneVarBind: Unable to clone vb->value.os_value\n"));
	    FreeVarBind(out_vb);
	    out_vb = NULL;
	    return (NULL);
	}
	break;
      case NULL_TYPE:

#ifdef SR_SNMPv2
      case NO_SUCH_OBJECT_EXCEPTION:
      case NO_SUCH_INSTANCE_EXCEPTION:
      case END_OF_MIB_VIEW_EXCEPTION:
#endif				/* SR_SNMPv2 */

	break;
      default:
	DPRINTF((APWARN,
		 "CloneVarBind: Illegal type: 0x%x\n", out_vb->value.type));
	FreeVarBind(out_vb);
	out_vb = NULL;
	return (NULL);
	break;
    }				/* end of switch */

    return (out_vb);

}				/* CloneVarBind() */

#ifdef SR_SNMPv2
/*
 * CloneUInt64:
 *
 *   The former name (pre-snmp12.1.0.0 release) was clone_uint64().
 */
UInt64         *
CloneUInt64(value)
    UInt64         *value;
{

    UInt64         *return_val;

    return_val = (UInt64 *) NULL;

    if (value == (UInt64 *) NULL) {
	DPRINTF((APTRACE, "CloneUInt64: value passed in is NULL\n"));
	return (NULL);
    }

    if ((return_val = (UInt64 *) malloc(sizeof(UInt64))) == (UInt64 *) NULL) {
	DPRINTF((APWARN, "CloneUInt64: Cannot malloc return_val\n"));
	return (NULL);
    }

    return_val->big_end = value->big_end;
    return_val->little_end = value->little_end;

    return (return_val);

}				/* CloneUInt64() */

/*
 * CloneBitString:
 *
 *   The former name (pre-snmp12.1.0.0 release) was clone_bitstring().
 */
BitString      *
CloneBitString(_bitstring_value)
    BitString      *_bitstring_value;
{

    BitString      *return_string;

    return_string = (BitString *) NULL;

    if (_bitstring_value == (BitString *) NULL) {
	DPRINTF((APTRACE, "CloneBitString: value passed in is NULL\n"));
	return (NULL);
    }

    if ((return_string = (BitString *) malloc(sizeof(BitString))) ==
	(BitString *) NULL) {
	DPRINTF((APWARN, "CloneBitString: Cannot malloc return_string\n"));
	return (NULL);
    }

    return_string->length = _bitstring_value->length;
    return_string->unused = _bitstring_value->unused;
    return_string->octet_ptr = (unsigned char *) NULL;

    /*
     * Malloc space for the octet_ptr and copy over the value from
     * _bitstring_value->octet_ptr.
     */
    if ((return_string->octet_ptr =
	 (unsigned char *) malloc((unsigned) (MAX((int) _bitstring_value->length, 1)))) ==
	(unsigned char *) NULL) {
	DPRINTF((APWARN,
	       "CloneBitString: Cannot malloc return_string->octet_ptr\n"));
	FreeBitString(return_string);
	return_string = (BitString *) NULL;
	return (NULL);
    }

    memcpy((char *) return_string->octet_ptr,
	   (char *) _bitstring_value->octet_ptr,
	   (int) _bitstring_value->length);

    return (return_string);


}				/* CloneBitString() */

#endif				/* SR_SNMPv2 */
