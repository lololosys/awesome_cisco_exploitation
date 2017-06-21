/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_objectdb.c,v 3.3 1995/12/28 22:35:45 mordock Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/snmp/sr_objectdb.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, SNMP Research
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_objectdb.c,v $
 * Revision 3.3  1995/12/28  22:35:45  mordock
 * CSCdi46168:  improve snmp modularity
 * move some code and globals out of the kernel subsystem and back into
 * the snmp subsystem
 *
 * Revision 3.2  1995/11/17  18:59:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:17:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:49:38  hampton
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
static char     rcsid[] = "objectdb.c,v 1.20 1995/02/06 18:11:37 battle Exp";
#endif				/* (! ( defined(lint) ) && defined(SR_RCSID)) */

/*
 *
 * objectdb.c,v
 * Revision 1.20  1995/02/06  18:11:37  battle
 * Changes for vrtx.
 *
 * Revision 1.19  1994/11/04  16:33:21  battle
 * lint nits
 *
 * Revision 1.18  1994/10/28  17:32:15  vaughn
 * Placed new copyright in the file
 *
 * Revision 1.17  1994/10/26  21:58:37  vaughn
 * Changed copyrights to the new copyright structure
 *
 * Revision 1.16  1994/10/20  14:36:33  battle
 * added check for null contextInfo
 *
 * Revision 1.15  1994/10/06  15:23:31  pearson
 * Made the OidList[] array FAR for Packaged Agent customers, fixed the type of 'domain' in i_GetVar()
 *
 * Revision 1.14  1994/09/30  16:11:24  battle
 * hprt changes
 *
 * Revision 1.13  1994/09/09  14:14:49  battle
 * added #include "pdu.h"
 *
 * Revision 1.12  1994/08/25  15:49:51  reid
 * added i_GetVar
 *
 * Revision 1.11  1994/08/04  15:37:38  battle
 * pSOS 2.0 changes
 *
 * Revision 1.10  1994/06/13  14:58:19  battle
 * Added async library, GetObject, and byteswapping code.
 *
 * Revision 1.9  1994/03/03  17:58:42  levi
 * Fixed ifdef/endif matching
 *
 * Revision 1.8  1994/03/03  17:43:30  levi
 * Changed #if !defined to #ifndef
 *
 * Revision 1.7  1994/02/22  19:24:58  battle
 * Put it back like it was before Karl's update.
 *
 * Revision 1.5  1994/02/21  17:27:54  battle
 * Fixed log message.
 *
 * Revision 1.3  1994/02/14  16:42:17  pearson
 * Updated copyright notices for 1994.
 *
 * AMP
 *
 * Revision 1.2  1993/12/23  18:38:40  battle
 * Added cvs headers to all .c and .h files.
 *
 *
 * 1-Oct-1992 DLB
 *    Rewritten to get rid of in-place binary trees; now uses binary search in
 *    the OidList array.
 *
 * 2-Dec-1992 DLB
 *    Added code for handling comressed ObjectInfo structures.  Doesn't work
 *    yet.
 *
 * 7-Dec-1992 DLB
 *    Finished up work on compressed_str_to_oid() and expand_oi().
 *
 * 9-Dec-1992 DLB
 *   Lint nits.
 *
 * 10-Jun-1993 DLB
 *   Added #undef STdeclare and #undef _SNMPPART_H_ for SCO's benefit.
 *
 * 14-Jun-1993 DLB
 *   Removed old compressed object info structure stuff.  Reorganized
 *   findObject a bit.
 *
 * 15-Jun-1993 DLB
 *   Changed OidList from static to external and changed NumOIDelem from
 *   a #define to an external int so that they can be accessed from sv1resp.c,
 *   sv2resp.c, and sets.c.
 *
 * 23-Jun-1993 DLB
 *   Added NextObjectClass and FindObjectClass.
 *
 * 22-Jul-1993 DLB
 *   Added emanate support.
 */





/* Includes and definitions for CISCO */

#include "master.h"

#include "sr_snmpd.h"
#include "sr_diag.h"
#include "sr_objectdb.h"
#include "snmp_registry.h"

#define OidElem(i) (OidIndex[i])
#define TotalObjects NumObjects



/*
 * Log from the emanate version before the merge:
 *
 * Revision 1.5  1994/02/15  19:55:51  kallen
 * Getting copyright notices where they belong KIA
 *
 * Revision 1.4  1994/01/31  15:30:00  battle
 * Resolved rcsid conflict.
 *
 * Revision 1.3  1994/01/19  17:24:44  levi
 * Added #if 0 around rcsid to prevent compiler warnings when objectdb.c
 * is #include'd in another source file.
 *
 * Revision 1.2  1993/11/10  19:27:09  battle
 * Additions to emanate to add shared library support and cvs headers.
 *
 *
 * Revision History:
 *
 * 1-Oct-1992 DLB
 *    Rewritten to get rid of AVL trees; now uses binary search in
 *    a realloced array of pointers.
 */

/*
 * This file contains the dispatch table and the routines used to
 * access it.
 */



ObjectInfo    **OidIndex = NULL;
long            NumObjects = 0;


ObjectInfo    **
object_info_array_to_pointer_array(ObjectInfo *varList, uint count)
{
    int             i;
    ObjectInfo    **parray = NULL;

    /* allocate and zero the array of pointers to ObjectInfo structures */
    parray = malloc((count + 1) * sizeof(ObjectInfo *));

    if (parray == NULL) {
	DPRINTF((APERROR, "Can't malloc pointer array\n"));
	return NULL;
    }
    memset((char *)parray, 0, (count + 1) * sizeof(ObjectInfo *));

    /* fill in the pointers to the ObjectInfo struct in the array */
    for (i = 0; i < count; i++) {
	parray[i] = &(varList[i]);
    }

    parray[count] = NULL;

    return parray;

}

void
load_mib (const ObjectInfo *mib, const uint num)
{
    append_var(object_info_array_to_pointer_array((ObjectInfo *) mib,
						  (uint) num));
}


void
init_var()
{
    /*
     * create the empty malloced array of pointers to ObjectInfo structures.
     */
    OidIndex = malloc(sizeof(ObjectInfo *));
    NumObjects = 0;
}


int
append_var(parray)
    ObjectInfo    **parray;
{
    /*
     * merge parray with the OidIndex array.  If there are duplicate objects,
     * just combine method descriptors in the existing object.  Don't throw
     * away the one in parray, though, because it will be needed when the
     * subagent disconnects. Initially allocate an array of pointers to merge
     * into which is the sum of the existing size and the size of parray (not
     * sizeof).  After the merge, realloc to the actual size of the merged
     * list.
     * 
     * %%% could free redundant parray structs and set the parray pointers %%%
     * to point to the structs pointed to by OidIndex array, or, just pass
     * %%% in the raw OidList and duplicate only those that don't already %%%
     * exist.
     */
    long            count;
    ObjectInfo    **marray;
    long            pi, mi, oi;
    int             cmp;

    for (count = 0; parray[count] != NULL; count++) {
	;
    }
    if ((marray = (ObjectInfo **)
	 malloc((NumObjects + count + 1) * sizeof(ObjectInfo *))) == NULL) {
	DPRINTF((APERROR, "append_var: can't malloc pointer array\n"));
	return 0;
    }
    pi = 0;
    oi = 0;
    mi = 0;
    while (parray[pi] != NULL && OidIndex[oi] != NULL) {
	cmp = CmpOID(&(parray[pi]->oid), &(OidIndex[oi]->oid));
	if (cmp < 0) {
	    marray[mi++] = parray[pi++];
	}
	else if (cmp == 0) {
	    DPRINTF((APERROR, "DUPLICATE OBJECT!!\n"));

	    /* remember to skip to the next thing in parray also!! */
	    pi++;
	}
	else {			/* cmp > 0 */
	    marray[mi++] = OidIndex[oi++];
	}
    }
    /* One of them has run out, finish off the other one. */
    while (parray[pi] != NULL) {
	marray[mi++] = parray[pi++];
    }
    while (OidIndex[oi] != NULL) {
	marray[mi++] = OidIndex[oi++];
    }
    marray[mi] = NULL;

    free((char *) OidIndex);
    if ((OidIndex = (ObjectInfo **)
       realloc((char *) marray, (mi + 1) * sizeof(ObjectInfo *))) == NULL) {
	DPRINTF((APERROR,
		 "append_var: realloc from %d to %d bytes failed!\n",
		 (NumObjects + count + 1) * sizeof(ObjectInfo *),
		 (mi + 1) * sizeof(ObjectInfo *)));
    }
    if (mi > (count + NumObjects + 1)) {
	DPRINTF((APWARN, "append_var: something fishy going on here\n"));
    }
    NumObjects = mi;

    free((char *) parray);

    return 1;
}




int
find_object(oid, searchType)
    OID            *oid;
    int             searchType;
{
    /*
     * Do a binary search in the OidList ObjectInfo array for an object
     * matching oid.
     */
    long            low, high, mid = 0;
    int             cmp = -1;
    ObjectInfo     *oi;


    low = 0;
    high = TotalObjects - 1;

    while (low <= high) {
	mid = (low + high) / 2;

	oi = OidElem(mid);
	cmp = CmpNOID(oid, &(oi->oid), oi->oid.length);

	if (cmp < 0) {
	    high = mid - 1;
	}
	else if (cmp > 0) {
	    low = mid + 1;
	}
	else {
	    break;
	}
    }

    if (searchType == EXACT) {
	if (cmp == 0) {
	    return mid;
	}
    }
    else {			/* if(searchType == NEXT) */
	if (cmp > 0) {
	    mid++;
	}

	if (mid < TotalObjects) {
	    return mid;
	}
    }

    return -1;
}

#ifdef SR_SNMPv2
static int IsV2Type SR_PROTOTYPE((int type));

static int
IsV2Type(type)
    int             type;
{
    if (type == BIT_STRING_TYPE || type == COUNTER_64_TYPE ||
	type == NSAP_ADDR_TYPE || type == U_INTEGER_32_TYPE) {
	return 1;
    }
    return 0;
}

#else
#define IsV2Type(x) 0
#endif				/* SR_SNMPv2 */

int
NextObjectClass(i, domain)
    int             i, domain;
{
    i++;

#ifdef SR_SNMPv1
    if (domain == SNMPv1_DOMAIN) {
	while (i < TotalObjects &&
	       (IsV2Type((int) (OidElem(i)->oidtype)))) {
	    i++;
	}
    }
#endif				/* SR_SNMPv1 */

    if (i < TotalObjects) {
	return i;
    }
    return -1;
}


#define FNAME "FindObjectClass"
int
FindObjectClass(oid, searchType, domain)
    OID            *oid;
    int             searchType, domain;
{
    int             i;

    if ((i = find_object(oid, searchType)) == -1) {
	DPRINTF((APTRACE, "%s: findObject failed\n", FNAME));
	return -1;
    }

#ifdef SR_SNMPv1
    if (domain == SNMPv1_DOMAIN) {
	if (searchType == EXACT && IsV2Type((int) (OidElem(i)->oidtype))) {
	    DPRINTF((APTRACE, "%s: searchType exact on non v1 object\n", FNAME));
	    return -1;
	}
	while (i < TotalObjects &&
	       IsV2Type((int) (OidElem(i)->oidtype))) {
	    i++;
	}
	if (i > TotalObjects) {
	    DPRINTF((APTRACE, "%s: ran off end of mib\n", FNAME));
	    return -1;
	}
    }
#endif				/* SR_SNMPv1 */

    return i;
}

#undef FNAME


VarBind        *
GetExactObjectInstance(incoming, view, contextInfo, serialNum, domain)
    OID            *incoming;
    long            view;
    ContextInfo    *contextInfo;
    int             serialNum, domain;
{
    VarBind        *outvb;
    int             i;

    if ((i = FindObjectClass(incoming, EXACT, domain)) == -1 ||
	(view != -1 && (CheckClassMIBView(&(OidElem(i)->oid), view) < 0))) {
	DPRINTF((APTRACE, "No such object.\n"));
	return exception(NO_SUCH_OBJECT_EXCEPTION, incoming, domain);
    }

    outvb = NULL;
    if ((view == -1 || (CheckMIBView(incoming, view) > 0))) {

#ifndef LIGHT
	DPRINTF((APTRACE, "Searching for requested instance of %s\n",
		 OidElem(i)->oidname));
#endif				/* LIGHT */

	outvb = (*(OidElem(i)->get)) (incoming, OidElem(i), EXACT,
				      contextInfo, serialNum);
    }

    if (outvb != NULL) {
	return outvb;
    }

    DPRINTF((APTRACE, "No such instance.\n"));
    return exception(NO_SUCH_INSTANCE_EXCEPTION, incoming, domain);
}

/*
 * This function returns the next instace of any mib object which is greater
 * than incoming relative to the view identified by view.  contextInfo,
 * and serialNum are passed as parameters to any method routine call(s).
 * if domain is SNMPv1_DOMAIN then all v2 type objects (bitstrings,
 * counter64's) are skipped over.  Returns a varbind containing the next
 * accessible object, or an exception or NULL (depending on domain) if end of
 * mib is encountered.
 */

VarBind        *
GetNextObjectInstance(incoming, view, contextInfo, serialNum, domain)
    OID            *incoming;
    long            view;
    ContextInfo    *contextInfo;
    int             serialNum, domain;
{
    VarBind        *outvb;
    int             dopost = 0;
    OID            *local_incoming = incoming;
    int             i;

    /* we do one search, from then on we just increment to the next object. */
    if ((i = FindObjectClass(local_incoming, NEXT, domain)) == -1) {
	goto end_of_mib;
    }

    if (CmpOID(&(OidElem(i)->oid), local_incoming) > 0) {
	/*
	 * Found something bigger than what we were looking for, the instance
	 * information is meaningless, set local_incoming to OidElem(i)'s oid
	 * so that we ignore the instance info in incoming.
	 */
	local_incoming = &(OidElem(i)->oid);
    }

    /*
     * Outer loop until we find a variable class we can access or run out of
     * variable classes.
     */
    do {
	outvb = NULL;

	if (view == -1 || ((dopost = CheckClassMIBView(&(OidElem(i)->oid), view)) >= 0)) {

	    /*
	     * Inner loop until we find an instance we can access or run out
	     * of instances (get method returns NULL).
	     */
	    do {

#ifndef LIGHT
		DPRINTF((APTRACE, "Searching for next instance of %s\n",
			 OidElem(i)->oidname));
#endif				/* LIGHT */

		outvb = (*(OidElem(i)->get)) (local_incoming, OidElem(i),
					      NEXT, contextInfo, serialNum);

		if (outvb != NULL)
		    local_incoming = outvb->name;
	    } while (outvb != NULL &&
		     (dopost != 1 &&
		    (view != -1 && (CheckMIBView(outvb->name, view) < 0))));
	}

	if (outvb != NULL)
	    break;

	i = NextObjectClass(i, domain);

	if (i != -1)
	    local_incoming = &(OidElem(i)->oid);

    } while (i != -1);

    if (outvb != NULL) {
	return outvb;
    }

    /* fall through */

end_of_mib:
    DPRINTF((APTRACE, "End of mib.\n"));
    return exception(END_OF_MIB_VIEW_EXCEPTION, incoming, domain);
}


#ifdef SR_SNMPv2
/* Construct an exception VarBind.  If domain is SNMPv1_DOMAIN, returns NULL
   since SNMPv1 did not use exceptions.

   When only SR_SNMPv1 is defined this function is replaced by a macro in
   snmpd.h which expands to NULL regardless of parameters. */

VarBind        *
exception(type, name, domain)
    int             type;
    OID            *name;
    int             domain;
{
    VarBind        *vb;

    if (domain == SNMPv1_DOMAIN) {
	return NULL;
    }

    if ((vb = (VarBind *) malloc(sizeof(VarBind))) == NULL) {
	DPRINTF((APERROR, "exception: vb malloc\n"));
	return (NULL);
    }

    vb->name = CloneOID(name);	/* name */
    vb->name_size = FindLenOID(vb->name);
    vb->value.type = type;
    vb->value.ul_value = 0;
    vb->value.sl_value = 0;
    vb->value.oid_value = NULL;
    vb->value.os_value = NULL;
    vb->value.uint64_value = NULL;
    vb->value.bitstring_value = NULL;
    vb->next_var = NULL;
    vb->data_length = 0;
    vb->value_size = 2;
    return (vb);
}

#endif				/* SR_SNMPv2 */

ObjectInfo     *
GetOidElem(i)
    int             i;
{
    return OidElem(i);
}

VarBind        *
i_GetVar(contextInfo, searchType, var)
    ContextInfo    *contextInfo;
    int             searchType;
    OID            *var;
{
    long            view;
    int             domain;

#ifdef SR_SNMPv2
    domain = SNMPv2_DOMAIN;
#else
    domain = SNMPv1_DOMAIN;
#endif				/* SR_SNMPv2 */

    if (contextInfo != NULL) {
	view = GetViewIndex(contextInfo->u.local.context);
    }
    else {
	view = -1;
    }

    if (searchType == EXACT) {
	return GetExactObjectInstance(var, view, contextInfo, -1, domain);
    }
    else {
	return GetNextObjectInstance(var, view, contextInfo, -1, domain);
    }
}
