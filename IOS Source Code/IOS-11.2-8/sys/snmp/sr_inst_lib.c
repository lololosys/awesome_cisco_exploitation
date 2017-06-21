/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_inst_lib.c,v 3.2 1995/11/17 18:59:40 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/snmp/sr_inst_lib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, SNMP Research
 *
 * Copyright (c) 1994,1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_inst_lib.c,v $
 * Revision 3.2  1995/11/17  18:59:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:17:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:49:23  hampton
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
static char     rcsid[] = "inst_lib.c,v 1.29 1995/02/07 17:04:53 vaughn Exp";
#endif				/* (! ( defined(lint) ) && defined(SR_RCSID)) */

/*
 *  Revision History
 *
 * inst_lib.c,v
 * Revision 1.29  1995/02/07  17:04:53  vaughn
 * Changed a ifdef SR_PSOS to an ifndef SR_PSOS
 *
 * Revision 1.28  1995/02/06  18:11:34  battle
 * Changes for vrtx.
 *
 * Revision 1.27  1994/11/09  17:50:59  pearson
 * Fixed a but in which 16-bit (intel) systems were losing the top two octets of IP addresses in OctetStringToIP() because of implicit casts in the arithmetic shift
 *
 * Revision 1.26  1994/11/02  15:03:13  battle
 * removed unused variable length in InstToImplied functinos
 *
 * Revision 1.25  1994/10/28  17:32:08  vaughn
 * Placed new copyright in the file
 *
 * Revision 1.24  1994/10/27  15:00:52  reid
 * make sure *os is always either NULL or a valid pointer on return from
 * InstTo*OctetString
 *
 * Revision 1.23  1994/10/26  21:58:34  vaughn
 * Changed copyrights to the new copyright structure
 *
 * Revision 1.22  1994/10/24  18:58:38  reid
 * changed a = to == in InstToUInt
 *
 * Revision 1.21  1994/10/06  15:21:39  pearson
 * Made some functions available on the PA platform
 *
 * Revision 1.20  1994/09/30  16:11:22  battle
 * hprt changes
 *
 * Revision 1.19  1994/08/04  15:37:36  battle
 * pSOS 2.0 changes
 *
 * Revision 1.18  1994/06/30  19:03:10  reid
 * fixed a bug in InstToImpliedOctetString where carry was not getting reset
 * to 0
 *
 * Revision 1.17  1994/06/30  16:51:03  levi
 * Added #include "snmp.h"
 *
 * Revision 1.16  1994/06/30  15:59:26  reid
 * removed AssignValue and BuildReturnVarBind. These will be put in a
 * different file.
 *
 * Revision 1.15  1994/06/29  16:57:36  reid
 * fixed a bug in InstToImpliedOctetString
 * added BuildReturnVarBind and AssignValue
 *
 * Revision 1.14  1994/06/14  18:39:29  reid
 * fixed errors in InstToVariableOctetString where getnext foo.2.255.255 returned
 * the wrong value. (should return foo.3.0.0.0).
 *
 * Revision 1.13  1994/05/16  20:13:30  reid
 * modified InstToOctetString to return only the OctetString rather than
 * including the length as part of the OctetString.
 *
 * Revision 1.12  1994/03/28  17:40:19  reid
 * put unsigned long cast in front of 0xFFFFFFFF for some older compilers.
 *
 * Revision 1.11  1994/02/28  21:40:11  reid
 * added OctetStringToIP()
 *
 * Revision 1.10  1994/02/14  16:42:15  pearson
 * Updated copyright notices for 1994.
 *
 * AMP
 *
 * Revision 1.9  1994/02/08  00:10:07  pearson
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
 * Revision 1.8  1994/02/02  15:42:20  levi
 * Various fixes to InstToImpliedOID and InstToVariableOID.
 *
 * Revision 1.7  1994/01/31  19:51:16  levi
 * Added InstToImpliedOID and InstToVariableOID to inst_lib.c for use in
 * Manager-To-Manager MIB.
 *
 * Revision 1.6  1994/01/13  16:13:00  battle
 * Dynix changes and tester bug fixes.
 *
 * Revision 1.5  1994/01/06  15:08:46  levi
 * Bug fixes for SCO agent.
 *
 * Revision 1.4  1994/01/05  15:06:22  reid
 * Added IPToOctetString
 *
 * Revision 1.3  1993/12/23  18:38:38  battle
 * Added cvs headers to all .c and .h files.
 *
 *
 *  6-11-93 DAR
 *  initial coding
 *
 *  09-Aug-1993 DLB
 *    Added #undef EMANATE
 *
 */


#include "master.h"

#include "sr_snmpd.h"
#include "sr_snmpuser.h"
#include "sr_diag.h"
#include "sr_proto.h"

#include "sr_sitedefs.h"

#define MAX_LONG 0x7FFFFFFF
#define MAX_ULONG ((unsigned long) 0xFFFFFFFF)

/*
 * InstToInt:
 *
 * Extract an integer index from the incoming OID instance starting
 * at the postion indicated by the index parameter. returns 1 on success,
 * 0 on failure.
 */
int
InstToInt(inst, index, val, searchType, carry)
    OID            *inst;
    int             index;
    long           *val;
    int             searchType;
    int            *carry;
{
    unsigned long   tmp;

    if (val == NULL) {
	return -1;
    }
    if ((searchType == GET_REQUEST_TYPE) && *carry) {
	return -1;
    }
    if (index >= inst->length) {
	if (searchType == GET_REQUEST_TYPE) {
	    return -1;
	}
    }

    if (index < inst->length) {
	tmp = inst->oid_ptr[index];
	if ((tmp > (unsigned long) MAX_LONG) || ((tmp + *carry) > (unsigned long) MAX_LONG)) {
	    if (searchType == GET_REQUEST_TYPE) {
		return -1;
	    }
	    tmp = 0L;
	    *carry = 1;
	}
	else {
	    tmp += *carry;
	    *carry = 0;
	}
	*val = (long) tmp;
    }
    else {
	*val = 0L;
	*carry = 0;
    }
    return 1;
}

/*
 * InstToIP:
 *
 * Extract an IP address index from the incoming OID instance starting
 * at the postion indicated by the index parameter. returns 1 on success,
 * 0 on failure.
 */
int
InstToIP(inst, index, ipAddr, searchType, carry)
    OID            *inst;
    int             index;
    unsigned long  *ipAddr;
    int             searchType;
    int            *carry;
{
    int             i;
    int             length;
    unsigned long   tmp;

    if (ipAddr == NULL) {
	return -1;
    }
    *ipAddr = 0L;
    length = inst->length - index;

    if (searchType == GET_REQUEST_TYPE) {
	if ((length < 4) || *carry) {
	    return -1;
	}

	*ipAddr = 0L;
	for (i = 3; i >= 0; i--) {
	    if (inst->oid_ptr[index + i] > 255) {
		return -1;
	    }

	    *ipAddr |= inst->oid_ptr[index + i] << ((3 - i) * 8);
	}
	return 1;
    }
    else {			/* must be a NEXT */
	if (length < 4)
	    *carry = 0;
	*ipAddr = 0L;
	for (i = ((length > 4) ? 3 : length - 1); i >= 0; i--) {
	    tmp = inst->oid_ptr[index + i];
	    if ((tmp > 255) || ((tmp + *carry) > 255)) {
		*ipAddr = 0L;
		*carry = 1;
	    }
	    else {
		tmp += *carry;
		*ipAddr |= tmp << ((3 - i) * 8);
		*carry = 0;
	    }
	}
	return 1;
    }
}

/*
 * InstToUInt:
 *
 * Extract an unsigned integer index from the incoming OID instance starting
 * at the postion indicated by the index parameter. returns 1 on success,
 * 0 on failure.
 */
int
InstToUInt(inst, index, val, searchType, carry)
    OID            *inst;
    int             index;
    unsigned long  *val;
    int             searchType;
    int            *carry;
{
    unsigned long   tmp;

    if (val == NULL) {
	return -1;
    }
    if ((searchType == GET_REQUEST_TYPE) && *carry) {
	return -1;
    }
    if (index >= inst->length) {
	if (searchType == GET_REQUEST_TYPE) {
	    return -1;
	}
    }

    if (index < inst->length) {
	tmp = inst->oid_ptr[index];
	if ((tmp == MAX_ULONG) && *carry) {
	    tmp = 0L;
	    *carry = 1;
	}
	else {
	    tmp += *carry;
	    *carry = 0;
	}

	*val = tmp;
    }
    else {
	*val = (unsigned long) 0;
	*carry = 0;
    }
    return 1;
}

/*
 * InstToFixedOctetString:
 *
 * Extract an OctetString index from the incoming OID instance starting
 * at the postion indicated by the index parameter. returns 1 on success,
 * 0 on failure.
 */

int
InstToFixedOctetString(inst, index, os, searchType, carry, size)
    OID            *inst;
    int             index;
    OctetString   **os;
    int             searchType;
    int            *carry;
    int             size;
{
    int             length;
    int             i, j;
    unsigned long   tmp;

    if (os == NULL) {
	goto fail;
    }
    if (searchType == GET_REQUEST_TYPE && *carry) {
	*os = NULL;
	goto fail;
    }

    if ((*os = (OctetString *) malloc(sizeof(OctetString))) == NULL) {
	DPRINTF((APWARN, "InstToFixedOctetString: cannot malloc OctetString\n"));
	goto fail;
    }
    (*os)->octet_ptr = NULL;
    (*os)->length = 0;
    if (index >= inst->length) {
	if (searchType == GET_REQUEST_TYPE) {
	    goto fail;
	}
    }

    length = inst->length - index;

    if (((*os)->octet_ptr = (unsigned char *)
	 malloc((unsigned) (size) * sizeof(char))) == NULL) {
	DPRINTF((APWARN, "InstToFixedOctetString: cannot malloc octet_ptr\n"));
	goto fail;
    }

    if (searchType == GET_REQUEST_TYPE) {
	if ((length < size) || *carry) {
	    goto fail;
	}

	for (i = size - 1; i >= 0; i--) {
	    if (inst->oid_ptr[index + i] > 255) {
		goto fail;
	    }
	    (*os)->octet_ptr[i] = (unsigned char) inst->oid_ptr[index + i];
	}
	(*os)->length = size;
	return 1;
    }
    else {			/* must be a NEXT */
	(*os)->length = size;
	for (i = size - 1; i >= 0; i--) {
	    if ((index + i) < inst->length) {
		tmp = inst->oid_ptr[index + i];
	    }
	    else {
		tmp = 0L;
		*carry = 0;
	    }
	    if ((tmp > 255) || ((tmp + *carry) > 255)) {
		for (j = i; j < size; j++) {
		    (*os)->octet_ptr[j] = (unsigned char) 0;
		}
		*carry = 1;
	    }
	    else {
		(*os)->octet_ptr[i] = (unsigned char) (tmp + *carry);
		*carry = 0;
	    }
	}
    }

    return 1;
fail:
    if (os != NULL) {
	FreeOctetString(*os);
	*os = NULL;
    }
    return -1;
}

/*
 * InstToImpliedOctetString:
 *
 * Extract an OctetString index from the incoming OID instance starting
 * at the postion indicated by the index parameter. returns 1 on success,
 * 0 on failure.
 */
int
InstToImpliedOctetString(inst, index, os, searchType, carry)
    OID            *inst;
    int             index;
    OctetString   **os;
    int             searchType;
    int            *carry;
{
    int             expected_length;
    int             i;
    unsigned long   tmp;

    if (os == NULL) {
	goto fail;
    }
    if (searchType == GET_REQUEST_TYPE && *carry) {
	*os = NULL;
	goto fail;
    }

    if ((*os = (OctetString *) malloc(sizeof(OctetString))) == NULL) {
	DPRINTF((APWARN, "InstToImpliedOctetString: cannot malloc OctetString\n"));
	goto fail;
    }
    (*os)->octet_ptr = NULL;
    (*os)->length = 0;

    if (index >= inst->length) {
	if (searchType == GET_REQUEST_TYPE) {
	    goto fail;
	}
	*carry = 0;
	return 1;
    }

    expected_length = inst->length - index;

    if (((*os)->octet_ptr = (unsigned char *)
	 malloc((unsigned) (expected_length + 1) * sizeof(unsigned char))) == NULL) {
	DPRINTF((APWARN, "InstToImpliedOctetString: cannot malloc octet_ptr\n"));
	goto fail;
    }

    if (searchType == GET_REQUEST_TYPE) {
	for (i = expected_length - 1; i >= 0; i--) {
	    if (inst->oid_ptr[index + i] > 255) {
		goto fail;
	    }
	    (*os)->octet_ptr[i] = (unsigned char) inst->oid_ptr[index + i];
	}
	(*os)->length = expected_length;
	return 1;
    }
    else {			/* must be a NEXT */
	(*os)->octet_ptr[expected_length] = (unsigned char) 0;
	*carry = 0;

	((*os)->length) = expected_length + 1;
	for (i = expected_length - 1; i >= 0; i--) {
	    if ((index + i) < inst->length) {
		tmp = inst->oid_ptr[index + i];
	    }
	    else {
		tmp = 0L;
		*carry = 0;
	    }
	    if ((tmp > 255) || ((tmp + *carry) > 255)) {
		((*os)->length) = i;
		*carry = 1;
	    }
	    else {
		(*os)->octet_ptr[i] = (unsigned char) (tmp + *carry);
		*carry = 0;
	    }
	}
    }

    return 1;
fail:
    if (os != NULL) {
	FreeOctetString(*os);
	*os = NULL;
    }
    return -1;
}


/*
 * InstToVariableOctetString:
 *
 * Extract an OctetString index from the incoming OID instance starting
 * at the postion indicated by the index parameter. returns 1 on success,
 * 0 on failure.
 */
int
InstToVariableOctetString(inst, index, os, searchType, carry)
    OID            *inst;
    int             index;
    OctetString   **os;
    int             searchType;
    int            *carry;
{
    int             length, expected_length;
    int             i, j;
    unsigned long   tmp;

    if (os == NULL) {
	goto fail;
    }
    if (searchType == GET_REQUEST_TYPE && *carry) {
	*os = NULL;
	goto fail;
    }

    if ((*os = (OctetString *) malloc(sizeof(OctetString))) == NULL) {
	DPRINTF((APWARN, "InstToVariableOctetString: cannot malloc OctetString\n"));
	goto fail;
    }
    (*os)->octet_ptr = NULL;
    (*os)->length = 0;

    if (index >= inst->length) {
	if (searchType == GET_REQUEST_TYPE) {
	    goto fail;
	}
	*carry = 0;
	return 1;
    }

    expected_length = inst->oid_ptr[index];

    if ((expected_length < 0) || (expected_length > MAX_OCTET_STRING_SIZE)) {
	if (searchType == GET_REQUEST_TYPE) {
	    goto fail;
	} else {
	    expected_length = MAX_OCTET_STRING_SIZE;
	}
    }

    length = inst->length - index - 1;

    if (((*os)->octet_ptr = (unsigned char *)
	 malloc((unsigned) (expected_length + 1) * sizeof(unsigned char))) == NULL) {
	DPRINTF((APWARN, "InstToVariableOctetString: cannot malloc octet_ptr\n"));
	goto fail;
    }
    ((*os)->length) = expected_length;

    if (searchType == GET_REQUEST_TYPE) {
	if ((length < expected_length) || *carry) {
	    goto fail;
	}

	for (i = expected_length; i > 0; i--) {
	    if (inst->oid_ptr[index + i] > 255) {
		goto fail;
	    }
	    (*os)->octet_ptr[i - 1] = (unsigned char) inst->oid_ptr[index + i];
	}
	return 1;
    }
    else {			/* must be a NEXT */
	for (i = expected_length; i > 0; i--) {
	    if ((index + i) < inst->length) {
		tmp = inst->oid_ptr[index + i];
	    }
	    else {
		tmp = 0L;
		*carry = 0;
	    }

	    if ((tmp > 255) || ((tmp + *carry) > 255)) {
		for (j = i; j <= expected_length; j++) {
		    (*os)->octet_ptr[j - 1] = (unsigned char) 0;
		}
		*carry = 1;
	    }
	    else {
		(*os)->octet_ptr[i - 1] = (unsigned char) (tmp + *carry);
		*carry = 0;
	    }
	}
	if (*carry) {
	    ((*os)->length)++;
	    if (((*os)->length) > 128) {
		((*os)->length) = 0L;
	    }
	    else {
		*carry = 0;
	    }
	    for (j = 0; j < ((*os)->length); j++) {
		(*os)->octet_ptr[j] = (unsigned char) 0;
	    }
	}
    }

    return 1;
fail:
    if (os != NULL) {
	FreeOctetString(*os);
	*os = NULL;
    }
    return -1;
}


/*
 * InstToImpliedOID:
 *
 * Extract an OID index from the incoming OID instance starting
 * at the postion indicated by the index parameter. returns 1 on success,
 * 0 on failure.
 */
int
InstToImpliedOID(inst, index, os, searchType, carry)
    OID            *inst;
    int             index;
    OID           **os;
    int             searchType;
    int            *carry;
{
    int             expected_length;
    int             i;
    unsigned long   tmp;

    if (os == NULL) {
	goto fail;
    }
    if (searchType == GET_REQUEST_TYPE && *carry) {
	goto fail;
    }

    if ((*os = (OID *) malloc(sizeof(OID))) == NULL) {
	DPRINTF((APWARN, "InstToImpliedOID: cannot malloc OID\n"));
	goto fail;
    }
    (*os)->oid_ptr = NULL;
    (*os)->length = 0;

    if (index >= inst->length) {
	if (searchType == GET_REQUEST_TYPE) {
	    goto fail;
	}
	return 1;
    }

    expected_length = inst->length - index;

    if (((*os)->oid_ptr = (unsigned long *)
	 malloc((unsigned) (expected_length + 1) * sizeof(unsigned long))) == NULL) {
	DPRINTF((APWARN, "InstToImpliedOID: cannot malloc oid_ptr\n"));
	goto fail;
    }

    if (searchType == GET_REQUEST_TYPE) {
	for (i = expected_length - 1; i >= 0; i--) {
	    (*os)->oid_ptr[i] = inst->oid_ptr[index + i];
	}
	(*os)->length = expected_length;
	return 1;
    }
    else {			/* must be a NEXT */
	(*os)->oid_ptr[expected_length] = 0L;

	((*os)->length) = expected_length + 1;
	for (i = expected_length - 1; i >= 0; i--) {
	    tmp = ((index + i) < inst->length) ? inst->oid_ptr[index + i] : 0L;
	    if ((tmp == MAX_ULONG) && (*carry)) {
		((*os)->length) = i;
		*carry = 1;
	    }
	    else {
		(*os)->oid_ptr[i] = tmp + *carry;
		*carry = 0;
	    }
	}
    }

    return 1;
fail:
    if (os != NULL) {
	FreeOID(*os);
	*os = NULL;
    }
    return -1;
}


/*
 * InstToVariableOID:
 *
 * Extract an OID index from the incoming OID instance starting
 * at the postion indicated by the index parameter. returns 1 on success,
 * 0 on failure.
 */
int
InstToVariableOID(inst, index, os, searchType, carry)
    OID            *inst;
    int             index;
    OID           **os;
    int             searchType;
    int            *carry;
{
    int             length, expected_length;
    int             i, j;
    unsigned long   tmp;

    if (os == NULL) {
	goto fail;
    }
    if (searchType == GET_REQUEST_TYPE && *carry) {
	goto fail;
    }

    if ((*os = (OID *) malloc(sizeof(OID))) == NULL) {
	DPRINTF((APWARN, "InstToVariableOID: cannot malloc OID\n"));
	goto fail;
    }
    (*os)->oid_ptr = NULL;
    (*os)->length = 0;

    if (index >= inst->length) {
	if (searchType == GET_REQUEST_TYPE) {
	    goto fail;
	}
	*carry = 0;
	return 1;
    }

    expected_length = inst->oid_ptr[index] + 1;
    length = inst->length - index;

    if (((*os)->oid_ptr = (unsigned long *)
	 malloc((unsigned) (expected_length + 1) * sizeof(unsigned long))) == NULL) {
	DPRINTF((APWARN, "InstToVariableOID: cannot malloc oid_ptr\n"));
	goto fail;
    }

    if (searchType == GET_REQUEST_TYPE) {
	if ((length < expected_length) || *carry) {
	    goto fail;
	}

	((*os)->length) = expected_length;
	for (i = expected_length - 1; i >= 0; i--) {
	    (*os)->oid_ptr[i] = inst->oid_ptr[index + i];
	}
	return 1;
    }
    else {			/* must be a NEXT */
	((*os)->length) = expected_length;
	for (i = expected_length - 1; i >= 0; i--) {
	    tmp = ((index + i) < inst->length) ? inst->oid_ptr[index + i] : 0L;
	    if ((i == 0) && (*carry)) {
		((*os)->length)++;
	    }
	    if ((tmp == MAX_ULONG) && (*carry)) {
		for (j = i; j <= expected_length; j++) {
		    (*os)->oid_ptr[j] = (unsigned long) 0;
		}
		*carry = 1;
		if (i == 0) {
		    ((*os)->length) = 1L;
		}
	    }
	    else {
		(*os)->oid_ptr[i] = (unsigned long) (tmp + *carry);
		*carry = 0;
	    }
	}
    }

    return 1;
fail:
    if (os != NULL) {
	FreeOID(*os);
	*os = NULL;
    }
    return -1;
}


/*
 * GetVariableIndexLength:
 *
 * return the length of a variable length OctetString
 */
int
GetVariableIndexLength(inst, index)
    OID            *inst;
    int             index;
{
    if (index >= inst->length) {
	return 0;
    }

    return (inst->oid_ptr[index] + 1);
}

/*
 * GetImpliedIndexLength:
 *
 * return the length of an IMPLIED OctetString
 */
int
GetImpliedIndexLength(inst, index)
    OID            *inst;
    int             index;
{
    if (index >= inst->length) {
	return 0;
    }

    return (inst->length - index);
}

/*
 * CmpOIDInst: compares instance information (from INDEX clause) of ptr1 and
 *             ptr2 where the instance starts at index. Returns 0 if instance
 *             of ptr1 have the same length and values as the instance of ptr2,
 *             < 0 if instance of ptr1 is less than (or "smaller") than the
 *             instance of ptr2, otherwise > 0.
 */
int
CmpOIDInst(ptr1, ptr2, index)
    OID            *ptr1, *ptr2;
    int             index;
{
    int             i;
    int             min;

    if (!ptr1 && !ptr2) {
	return 0;
    }
    else if (!ptr1) {
	return -1;
    }
    else if (!ptr2) {
	return 1;
    }

    min = (int) ((ptr1->length < ptr2->length) ? ptr1->length : ptr2->length);

    /* check for mismatched values */
    for (i = index; i < min; i++) {
	if (ptr1->oid_ptr[i] != ptr2->oid_ptr[i])
	    return ((int) (ptr1->oid_ptr[i] - ptr2->oid_ptr[i]));
    }

    /*
     * equal for as long as the shorter one holds out.  The longer should be
     * considered bigger
     */
    if (ptr1->length > ptr2->length)
	return (1);
    if (ptr1->length < ptr2->length)
	return (-1);

    /* they are equal for their entire mutual lengths */
    return (0);
}

/*
 * Convert an IP Address (in the form of an unsigned long) to
 * an OctetString.
 */
OctetString    *
IPToOctetString(ipAddr)
    unsigned long   ipAddr;
{
    unsigned char   buff[4];

    buff[0] = (ipAddr >> 24) & 0xff;
    buff[1] = (ipAddr >> 16) & 0xff;
    buff[2] = (ipAddr >> 8) & 0xff;
    buff[3] = (ipAddr >> 0) & 0xff;

    return (MakeOctetString(buff, 4L));
}

/*
 * Convert an OctetString to an IP Address (in the form of an unsigned long).
 */
unsigned long
OctetStringToIP(os)
    OctetString    *os;
{
    int             i;
    unsigned long   ipAddr = 0L;

    for (i = 3; i >= 0; i--) {
	ipAddr |= (unsigned long) os->octet_ptr[i] << ((3 - i) * 8);
    }
    return ipAddr;
}
