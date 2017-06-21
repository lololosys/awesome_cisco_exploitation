/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_oidtran.c,v 3.3.20.1 1996/03/18 21:48:43 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/snmp/sr_oidtran.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1995, SNMP Research
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_oidtran.c,v $
 * Revision 3.3.20.1  1996/03/18  21:48:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  12:33:46  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  17:14:35  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/01/31  23:26:45  jjohnson
 * CSCdi47892:  SNMP oid translation table needs to be modularized
 *
 * Revision 3.2  1995/11/17  19:00:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:17:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:49:44  hampton
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
static char     rcsid[] = "oidtran.c,v 1.19 1995/02/06 18:11:18 battle Exp";
#endif				/* (! ( defined(lint) ) && defined(SR_RCSID)) */

/*
 * Revision History:
 *
 * oidtran.c,v
 * Revision 1.19  1995/02/06  18:11:18  battle
 * Changes for vrtx.
 *
 * Revision 1.18  1995/02/03  19:12:54  levi
 * Reorganization of seclib.
 *   Stuff for command line utilities -> mgrutils.c
 *   OID structure manipulation -> oid_lib.c
 *   OID <--> English translation -> oidtran.c
 *   Type and access constant <--> string conversion -> convrsn.c
 *   Finding nodes in the MIB tree -> mib_lib.c
 *   Reading MIB files and building MIB structures -> mibread[m].c
 *   Parsing SIDs of an OID stored in a char* -> prse_sid.c
 *
 * Revision 1.17  1995/01/18  20:32:43  levi
 * Really fix hash lookup (see prev message)
 *
 * Revision 1.16  1995/01/18  20:22:31  reid
 * Changed > to != in MakeOIDFragFromDot for searching hash table
 *
 * Revision 1.15  1995/01/17  16:38:41  levi
 * The hash array is now an array of pointers into the mib_oid_table.  Conflicts
 * within the table linked using the hash_next member of struct MIB_OID.  The
 * changes in this file reflect these changes.
 *
 * Revision 1.14  1994/11/16  18:53:26  pearson
 * Fixed a bug in which FindType() was printing a long value as an int
 *
 * Revision 1.13  1994/11/09  20:22:48  pearson
 * Fixed a prototyping warning for the various ParseSubId functions
 *
 * Revision 1.12  1994/11/07  20:19:57  levi
 * Fixed MakeOIDFromFromDot to ensure the input string is left intact.
 *
 * Revision 1.11  1994/11/07  18:58:17  levi
 * Eliminated use of char buffer
 *
 * Revision 1.10  1994/11/04  19:07:49  pearson
 * Removed an __far which caused a syntax error
 *
 * Revision 1.9  1994/11/02  15:17:32  levi
 * Update for merging libsecure.a into libsnmp.a.
 * Separated sprint stuff in prnt_lib.c out into frmt_lib.c
 *
 * Revision 1.8  1994/10/28  16:37:54  vaughn
 * Placed new copyright in the file
 *
 * Revision 1.7  1994/10/26  21:51:56  vaughn
 * Changed copyrights to the new copyright structure
 *
 * Revision 1.6  1994/10/12  16:01:44  levi
 * Removed #include "snmp-mib.h"
 *
 * Revision 1.5  1994/09/30  16:10:53  battle
 * hprt changes
 *
 * Revision 1.4  1994/08/10  14:53:22  reid
 * Removed #ifdef LIGHT from oidtran.c; this allows name-to-OID translations
 * to work on the reduced snmp-mib.h even when LIGHT is defined.
 *
 * Revision 1.3  1994/08/04  15:37:30  battle
 * pSOS 2.0 changes
 *
 * Revision 1.2  1994/06/27  15:58:49  levi
 * Fixed to use mib_oid_table changes
 *
 * Revision 1.1  1994/06/27  15:52:25  levi
 * Moved oid translation stuff into oidtran.c
 *
 *
 */
#include "master.h"
#include <ctype.h>


#include "sr_diag.h"


#include "sr_snmp.h"
#include "sr_snmpuser.h"

extern int      global_InitNewOIDRoutines;


#ifdef NEW_OID_LIB
extern OID_TREE_ELE *oid_tree_ele_root;
struct MIB_OID *name_oid_hash_array[NAME_OID_HASH_SIZE];
#endif				/* NEW_OID_LIB */


/*
 * cisco additions to support dynamic loading of translation tables
 */

struct MIB_OID_LINKAGE {
    struct MIB_OID *oid_table;
    struct MIB_OID_LINKAGE *next;
};

#define OID_RECORD_SIZE 10
static struct MIB_OID_LINKAGE *oid_list_head;
static struct MIB_OID_LINKAGE *oid_list_tail;
static struct MIB_OID_LINKAGE *oid_record;
static int oid_index;


void
init_oid (void)
{
    oid_record = malloc(sizeof(struct MIB_OID_LINKAGE) * OID_RECORD_SIZE);
    if (oid_record == NULL)
	return;
    oid_record[0].oid_table = mib_oid_table;
    oid_record[0].next = NULL;
    oid_list_head = oid_list_tail = &oid_record[0];
    oid_index = 1;
}

void
load_oid (const struct MIB_OID *oid_table)
{
    if (oid_index == OID_RECORD_SIZE) {
	/*
	 * need to malloc a new block of records
	 */
	oid_record = malloc(sizeof(struct MIB_OID_LINKAGE) * OID_RECORD_SIZE);
	oid_index = 0;
    }
    
    if (oid_record == NULL)
	return;

    oid_record[oid_index].oid_table = (struct MIB_OID *)oid_table;
    oid_record[oid_index].next = NULL;
    oid_list_tail->next = &oid_record[oid_index];
    oid_list_tail = oid_list_tail->next;
    oid_index++;
}



/*
 * Routines to create an object identifer in an OID from
 * dot notation input.
 */

/*
 * MakeOIDFragFromDot:
 *
 *   This routine is called to create a library form object identifier
 *   from an character string.  The string input is usually in the
 *   format "integer.integer.integer...."  (i.e.,
 *   "1.3.6.1.2.1.1.1.0"), but can be shortened by using the names as
 *   they appear in RFC 1157 (i.e., "sysDescr.0").  It returns a
 *   pointer to a malloc'ed data structure containing the internal
 *   library representation for an object identifier.  This identifier
 *   can then be used in calls to make_varbind() and MakePdu() (in the
 *   case of traps).  This malloc'ed data structure will be free'ed by
 *   calls to FreePdu() after the pointer has been used.
 *
 */
OID            *
MakeOIDFragFromDot(text_str)
    char           *text_str;
{
    OID            *oid_ptr;
    char           *temp_ptr, *dot_ptr, *dot_restore, *english_dot;
    int             i, cc;
    int             dot_count;

#ifdef NEW_OID_LIB
    struct MIB_OID *hash_ptr;
    int             hash;
#endif				/* NEW_OID_LIB */

    struct MIB_OID_LINKAGE *node;
    struct MIB_OID * mib_oid_subtable;

    /*
     * replicate the oid string since it may be in rom and we want
     * to play with it
     */
    text_str = strdup(text_str);
    if (text_str == NULL) {
	/* no memory to duplicate it -- we are hosed */
	return(NULL);
    }

    dot_ptr = text_str;
    dot_restore = NULL;
    english_dot = NULL;

    /* see if there is an alpha descriptor at begining */
    if (((*text_str >= 'a') && (*text_str <= 'z')) ||
	((*text_str >= 'A') && (*text_str <= 'Z'))) {
	dot_ptr = strchr(text_str, '.');
	if (dot_ptr != NULL) {
	    *dot_ptr = '\0';
	    dot_restore = dot_ptr;
	    dot_ptr++;
	}

#ifndef NEW_OID_LIB
	english_dot = NULL;

	for (node = oid_list_head; node; node = node->next) {
	    mib_oid_subtable = node->oid_table;
	    for (i = 0; mib_oid_subtable[i].name != NULL; i++) {
		if (!strcmp(mib_oid_subtable[i].name, text_str)) {
		    english_dot = mib_oid_subtable[i].number;
		    break;
		}
	    }
	    if (english_dot)
		break;
	}

	if (english_dot == NULL) {
	    if (dot_restore != NULL) {
		*dot_restore = '.';
	    }
	    DPRINTF((APTRACE, "MakeOIDFragFromDot, table lookup failed: %s\n",
		     text_str));
	    free(text_str);
	    return (NULL);
	}
#else				/* NEW_OID_LIB */
	hash = 0;
	temp_ptr = text_str;
	while (*temp_ptr != '\0') {
	    hash = (hash + (int) *temp_ptr) % (int) NAME_OID_HASH_SIZE;
	    temp_ptr++;
	}

	if (global_InitNewOIDRoutines == 0) {
	    InitNewOIDRoutines();
	}

	for (hash_ptr = name_oid_hash_array[hash];
	     hash_ptr != NULL; hash_ptr = hash_ptr->hash_next) {
	    if (!strcmp(hash_ptr->name, text_str)) {
		break;
	    }
	}
	if (hash_ptr == NULL) {
	    if (dot_restore != NULL) {
		*dot_restore = '.';
	    }
	    DPRINTF((APTRACE,
		     "MakeOIDFragFromDot, hash table lookup failed: %s\n",
		     text_str));
	    free(text_str);
	    return (NULL);
	}

	english_dot = hash_ptr->number;
#endif				/* NEW_OID_LIB */

    }

    /* Ensure the input string is left intact */
    if (dot_restore != NULL) {
	*dot_restore = '.';
    }

    /* count the dots.  num +1 is the number of SIDs */
    dot_count = 0;
    if (english_dot && dot_ptr) {
	dot_count = 1;
    }
    if (english_dot) {
	for (i = 0; english_dot[i] != '\0'; i++) {
	    if (english_dot[i] == '.') {
		dot_count++;
	    }
	}
    }
    if (dot_ptr) {
	for (i = 0; dot_ptr[i] != '\0'; i++) {
	    if (dot_ptr[i] == '.') {
		dot_count++;
	    }
	}
    }
    if ((oid_ptr = (OID *) malloc(sizeof(OID))) == NULL) {
	DPRINTF((APTRACE, "MakeOIDFragFromDot, oid_ptr malloc\n"));
	free(text_str);
	return (NULL);
    }

    oid_ptr->oid_ptr = (unsigned long *) malloc((unsigned) ((dot_count + 1) * sizeof(unsigned long)));
    if (oid_ptr->oid_ptr == NULL) {
	DPRINTF((APTRACE, "MakeOIDFragFromDot, oid_ptr->oid_ptr malloc\n"));
	free(text_str);
	FreeOID(oid_ptr);
	NULLIT(oid_ptr);
	return (NULL);
    }

    oid_ptr->length = dot_count + 1;

    /* now we convert number.number.... strings */
    if (english_dot) {
	temp_ptr = english_dot;
	cc = 0;
    }
    else {
	temp_ptr = dot_ptr;
	cc = 1;
    }
    for (i = 0; i < dot_count + 1; i++) {

	if (*temp_ptr == '0') {
	    if ((temp_ptr[1] == 'x') || (temp_ptr[1] == 'X')) {
		if ((ParseSubIdHex((unsigned char **) &temp_ptr, &oid_ptr->oid_ptr[i])) == ~(unsigned) 0) {
		    DPRINTF((APTRACE, "MakeOIDFragFromDot, hex:\n"));
		    free(text_str);
		    FreeOID(oid_ptr);
		    NULLIT(oid_ptr);
		    return (NULL);
		}
	    }
	    else {
		if ((ParseSubIdOctal((unsigned char **) &temp_ptr, &oid_ptr->oid_ptr[i])) == ~(unsigned) 0) {
		    DPRINTF((APTRACE, "MakeOIDFragFromDot, octal:\n"));
		    free(text_str);
		    FreeOID(oid_ptr);
		    NULLIT(oid_ptr);
		    return (NULL);
		}
	    }
	}
	else {
	    if (*temp_ptr >= '0' && *temp_ptr <= '9') {
		if ((ParseSubIdDecimal((unsigned char **) &temp_ptr, &oid_ptr->oid_ptr[i])) == ~(unsigned) 0) {
		    DPRINTF((APTRACE, "MakeOIDFragFromDot, decimal:\n"));
		    free(text_str);
		    FreeOID(oid_ptr);
		    NULLIT(oid_ptr);
		    return (NULL);
		}
	    }
	    else {
		DPRINTF((APTRACE, "MakeOIDFragFromDot, bad character: %d, %s\n",
			 *temp_ptr, temp_ptr));
		free(text_str);
		FreeOID(oid_ptr);
		NULLIT(oid_ptr);
		return (NULL);
	    }
	}
	if (*temp_ptr == '.')
	    temp_ptr++;		/* to skip over dot */
	else if (*temp_ptr != '\0') {
	    DPRINTF((APTRACE, "MakeOIDFragFromDot, expected dot: %d %s\n",
		     *temp_ptr, temp_ptr));
	    free(text_str);
	    FreeOID(oid_ptr);
	    NULLIT(oid_ptr);
	    return (NULL);
	}
	else {
	    if ((cc == 0) && dot_ptr) {
		temp_ptr = dot_ptr;
	    }
	}
    }				/* end of for loop */

    free(text_str);
    return (oid_ptr);
}				/* MakeOIDFragFromDot() */


/*
 * MakeOIDFromDot:
 *
 *   This routine is called to create a library form object identifier
 *   from an character string.  The string input is usually in the
 *   format "integer.integer.integer...."  (i.e.,
 *   "1.3.6.1.2.1.1.1.0"), but can be shortened by using the names as
 *   they appear in RFC 1157 (i.e., "sysDescr.0").  It returns a
 *   pointer to a malloc'ed data structure containing the internal
 *   library representation for an object identifier.  This identifier
 *   can then be used in calls to make_varbind() and MakePdu() (in the
 *   case of traps).  This malloc'ed data structure will be free'ed by
 *   calls to FreePdu() after the pointer has been used.
 *
 * The former name (pre-snmp12.1.0.0 release) was make_obj_id_from_dot().
 */

OID            *
MakeOIDFromDot(s)
    char           *s;
{
    OID            *oid_ptr;
    oid_ptr = MakeOIDFragFromDot(s);
    if (oid_ptr == NULL) {
	DPRINTF((APTRACE, "MakeOIDFromDot: MakeOIDFragFromDot(%s) failed\n",
		 s));
	return (NULL);
    }
    if (oid_ptr->oid_ptr[0] >= 4) {	/* we have a bogus OID */
	DPRINTF((APTRACE, "MakeOIDFromDot, illegal OID value %lu\n",
		 oid_ptr->oid_ptr[0]));
	FreeOID(oid_ptr);
	NULLIT(oid_ptr);
	return (NULL);
    }
    return (oid_ptr);
}				/* MakeOIDFromDot() */


/*
 * MakeDotFromOID:
 *
 *   This routine is called to convert an Object Identifier library
 *   construct into a dot notation character string, usually for us in
 *   a human interface.  The dot-notation output is the usual form
 *   (1.2.3.4.1.2.1.1) with the a MIB name subsituted for the most
 *   possible sub-identifiers starting from the left
 *   (1.3.6.1.2.1.1.1.0 becomes sysDescr.0).  The MIB names included
 *   in the library are found in the mib_oid_table in snmp-mib.h.
 *   This include file is used in the compilation of oid_lib.c, part
 *   of the libsnmpuser.a library and need not be included in
 *   applications.
 *
 * The former name (pre-snmp12.1.0.0 release) was make_dot_from_obj_id().
 */

short
MakeDotFromOID(oid_ptr, buffer)
    OID            *oid_ptr;
    char            buffer[];
{
    buffer[0] = '\0';
    if ((oid_ptr == NULL) || (oid_ptr->length == 0) ||
	(oid_ptr->oid_ptr == NULL)) {
	DPRINTF((APTRACE, "MakeDotFromOID, bad (NULL) OID\n"));
	return (-1);
    }

#ifdef NEW_OID_LIB
    if (global_InitNewOIDRoutines == 0)
	InitNewOIDRoutines();
#endif				/* NEW_OID_LIB */

    if (GetStringFromSubIds(buffer, oid_ptr->oid_ptr,
			    (short) oid_ptr->length) == -1) {
	DPRINTF((APTRACE, "MakeDotFromOID, GetStringFromSubIds\n"));
	return (-1);
    }

    return (0);
}


#ifdef NEW_OID_LIB
/*
 * OIDTreeDive:
 *
 * The former name (pre-snmp12.1.0.0 release) was oid_tree_dive().
 *
 * Rewritten 11/22/94 DBL
 * This function takes the equivalent of an OID as arguments and a starting
 * point in a MIB tree (i.e. a sub-tree), and locates the child node
 * specified by the OID information.
 */
OID_TREE_ELE   *
OIDTreeDive(oid_tree_ptr, sid, sid_counter)
    OID_TREE_ELE   *oid_tree_ptr;
    unsigned long  *sid;
    int             sid_counter;
{
    OID_TREE_ELE   *ote;
    int             i;

    /* Just to be safe, and to preserve old functionality */
    if (sid_counter < 0) {
	DPRINTF((APERROR, "OIDTreeDive: sid_counter too low\n"));
	return (NULL);
    }
    if (sid_counter == 0) {
	return (NULL);
    }

    /* First check if the oid starts at our root or any sibling root */
    for (ote = oid_tree_ptr; ote != NULL; ote = ote->next) {
	if (ote->sid_value == sid[0]) {
	    break;
	}
    }
    if (ote == NULL) {
	return (NULL);
    }
    oid_tree_ptr = ote;

    /*
     * At this point, we should NOT return NULL, because we've found at least
     * one OTE node that matches.
     */
    /*
     * Walk down the tree.  If an SID names a child that doesn't exist, the
     * inner for loop will complete and ote will be NULL, and the while loop
     * will terminate.  If all SIDs name children that exist, the loop will
     * terminate, and oid_tree_ptr will point to the appropriate node.
     */
    i = 1;
    while ((oid_tree_ptr != NULL) && (i < sid_counter)) {
	for (ote = oid_tree_ptr->first_descendent; ote != NULL; ote = ote->next) {
	    if (ote->sid_value == sid[i]) {
		break;
	    }
	}
	if (ote == NULL) {
	    break;
	}
	i++;
	oid_tree_ptr = ote;
    }

    return (oid_tree_ptr);
}				/* end of OIDTreeDive */

#endif				/* NEW_OID_LIB */

#ifndef NEW_OID_LIB
/*
 * MatchGame:
 *
 * The former name (pre-snmp12.1.0.0 release) was match_game().
 */
short
MatchGame(ptr1, ptr2)
    char           *ptr1;
    char           *ptr2;
{
    short           count = 0;

    while (*ptr1++ == *ptr2++) {
	count++;
    }
    return (count);
}

#endif				/* NEW_OID_LIB */

/*
 * GetStringFromSubIds:
 *
 * The former name (pre-snmp12.1.0.0 release) was get_str_from_sub_ids().
 */
short
GetStringFromSubIds(string_buffer, sid, sid_counter)
    char            string_buffer[];
    unsigned long  *sid;
    int             sid_counter;
{
    int             i;
    char            temp_buffer[20];
    int             dot_count;

#ifdef NEW_OID_LIB
    OID_TREE_ELE   *oid_tree_ele_ptr;
#else
    struct MIB_OID *best;
    int             best_len, test_len;
#endif				/* NEW_OID_LIB */

    struct MIB_OID_LINKAGE *node;
    struct MIB_OID * mib_oid_subtable;

    /* build test string ... put dots between each entry but not after last */
    for (i = 0; i < sid_counter; i++) {
	(void) sprintf(temp_buffer, "%lu", sid[i]);
	if ((i + 1) != sid_counter) {
	    (void) strcat(string_buffer, temp_buffer);
	    (void) strcat(string_buffer, ".");
	}
	else
	    (void) strcat(string_buffer, temp_buffer);
    }

    /*
     * Now, let us try to convert the dotted decimal header of the string in
     * temp_buffer into a more reasonable ASCII string.
     */

#ifdef NEW_OID_LIB
    oid_tree_ele_ptr = OIDTreeDive(oid_tree_ele_root, sid, sid_counter);
#else				/* NEW_OID_LIB */
    best = NULL;
    best_len = -1;
    /* OK, now find the best fit to this string */

    for (node = oid_list_head; node; node = node->next) {
	mib_oid_subtable = node->oid_table;

    for (i = 0; mib_oid_subtable[i].name != NULL; i++) {
	test_len = MatchGame(mib_oid_subtable[i].number, string_buffer);

	/*
	 * test_len is a count of the number successfully matched i.e.,
	 * mib_oid_table[i].number[test_len] is the first mismatch; i.e.,
	 * mib_oid_table[i].number[test_len-1] is the last good match want to
	 * back up to the first character before the last . or null
	 */

	/**
	 * this is what the equation works out to be just looking at it but i
	 * simply could not sleep with such a thing so i applied demorgans
	 * theorem and got the resulting code below which should be
	 * equivalent but run faster
         * while ((test_len > 0)
         *        &&
         *        (!((mib_oid_table[i].number[test_len] == '.') ||
	 *           (mib_oid_table[i].number[test_len] == '\0'))
         *        ||
         *        !((string_buffer[test_len] == '.') ||
         *          (string_buffer[test_len] == '\0'))
         *        )
         *       ) {
	 *
	 **/

	while ((test_len > 0)
	       &&
	       (
		((mib_oid_subtable[i].number[test_len] != '.') &&
		 (mib_oid_subtable[i].number[test_len] != '\0'))
		||
		((string_buffer[test_len] != '.') &&
		 (string_buffer[test_len] != '\0'))
		)
	    ) {
	    test_len--;
	}			/* while */
	if ((test_len > best_len) && (test_len == strlen(mib_oid_subtable[i].number))) {
	    best = &mib_oid_subtable[i];
	    best_len = test_len;
	}			/* if test_len > */
    }				/* for i */
    }
#endif				/* NEW_OID_LIB */

    /*
     * Did we get something?  If so, then replace dot-digit string with the
     * name.
     */

#ifdef NEW_OID_LIB
    if ((oid_tree_ele_ptr != NULL) && (strcmp(oid_tree_ele_ptr->oid_name, unassigned_text) != 0)) {

	/* blow away our old string */
	(void) strcpy(string_buffer, oid_tree_ele_ptr->oid_name);

	/* count the dots.  num +1 is the number of SIDs */
	dot_count = 0;
	for (i = 0; oid_tree_ele_ptr->oid_number_str[i] != '\0'; i++) {
	    if (oid_tree_ele_ptr->oid_number_str[i] == '.')
		dot_count++;
	}
#else				/* NEW_OID_LIB */
    if (best) {
	(void) strcpy(string_buffer, best->name);

	/* count the dots.  num +1 is the number of SIDs */
	dot_count = 0;
	for (i = 0; best->number[i] != '\0'; i++) {
	    if (best->number[i] == '.')
		dot_count++;
	}
#endif				/* NEW_OID_LIB */

	/* now flesh out from where the name leaves off */

	for (i = dot_count + 1; i < sid_counter; i++) {
	    (void) sprintf(temp_buffer, ".%lu", sid[i]);
	    (void) strcat(string_buffer, temp_buffer);
	}
    }
    /* else, just return the dot string we already filled in */
    return (1);
}
