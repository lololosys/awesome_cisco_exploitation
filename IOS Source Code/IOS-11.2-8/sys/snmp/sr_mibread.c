/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_mibread.c,v 3.2.62.1 1996/03/18 21:48:41 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/snmp/sr_mibread.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1995, SNMP Research
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_mibread.c,v $
 * Revision 3.2.62.1  1996/03/18  21:48:41  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  12:33:44  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  17:14:27  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:59:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:17:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:49:34  hampton
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
static char     rcsid[] = "mibread.c,v 1.1 1995/02/03 19:12:45 levi Exp";
#endif				/* (! ( defined(lint) ) && defined(SR_RCSID)) */

/*
 * Revision History:
 *
 * mibread.c,v
 * Revision 1.1  1995/02/03  19:12:45  levi
 * Reorganization of seclib.
 *   Stuff for command line utilities -> mgrutils.c
 *   OID structure manipulation -> oid_lib.c
 *   OID <--> English translation -> oidtran.c
 *   Type and access constant <--> string conversion -> convrsn.c
 *   Finding nodes in the MIB tree -> mib_lib.c
 *   Reading MIB files and building MIB structures -> mibread[m].c
 *   Parsing SIDs of an OID stored in a char* -> prse_sid.c
 *
 */

#include "master.h"
#include <ctype.h>


#include "sr_diag.h"


#define DO_NOT_DECLARE_MIB_OID_TABLE

#include "sr_snmp.h"
#include "sr_snmpuser.h"

#include "sr_snmp-mib.h"
struct MIB_OID *mib_oid_table = (struct MIB_OID *)orig_mib_oid_table;

#ifdef NEW_OID_LIB
OID_TREE_ELE   *oid_tree_ele_root;
OID_TREE_ELE   *oid_tree_node_storage = NULL;
OID_TREE_ELE   *otns_temp;
char           *unassigned_text = "UNASSIGNED";

struct MIB_OID *name_oid_hash_array[NAME_OID_HASH_SIZE];


/*
 * InitNewOIDRoutines:
 *
 *   Routines to initialize the new data structures that will speed
 *   name * to OID and OID to name translations at the expense of
 *   memory and * initialization time.
 *
 * The former name (pre-snmp12.1.0.0 release) was init_new_oid_routines().
 */
int             global_InitNewOIDRoutines = 0;

void
InitNewOIDRoutines()
{
    int             i;

    /* Initialize MIB tree */
    InitOIDTree();

    /* Clear out any old info from hash array */
    for (i = 0; i < NAME_OID_HASH_SIZE; i++) {
	name_oid_hash_array[i] = NULL;
    }

    InitNameOIDHashArray(0);

    global_InitNewOIDRoutines = 1;
}

/*
 * MergeNewOIDRoutines:
 *
 *  Performs same function as InitNewOIDRoutines, except does not
 *  discard previous tree and hash-table contents.
 *
 *  The value of start should be the index of the first of the
 *  newly merged entries.
 */
void
MergeNewOIDRoutines(start)
    int             start;
{
    int             i;

    if (!global_InitNewOIDRoutines) {
	InitNewOIDRoutines();
	return;
    }

    /* Merge latest info into MIB tree */
    MergeOIDTree(start);

    /* Clear out any old info from hash array */
    for (i = 0; i < NAME_OID_HASH_SIZE; i++) {
	name_oid_hash_array[i] = NULL;
    }

    /* Merge latest info into hash array */
    InitNameOIDHashArray(0);
}

/*
 * FreeOIDTree:
 *
 * Free memory used in the OID tree.
 */
void
_FreeOIDTree(node)
    OID_TREE_ELE   *node;
{
    OID_TREE_ELE   *tmp;

    while (node != NULL) {
	tmp = node;
	node = node->next;
	_FreeOIDTree(tmp->first_descendent);
	if (tmp->oid_name == unassigned_text) {
	    free(tmp);
	}
    }
}
void
FreeOIDTree(node)
    OID_TREE_ELE   *node;
{
    _FreeOIDTree(node);
    if (oid_tree_node_storage != NULL) {
	free(oid_tree_node_storage);
    }
}

/*
 * InitOIDTree:
 *
 * The former name (pre-snmp12.1.0.0 release) was init_oid_tree().
 */
void
InitOIDTree()
{
    int             i;

    if (mib_oid_table == NULL) {
	DPRINTF((APWARN, "mib_oid_table is NULL. snmpinfo.dat not loaded?\n"));
	return;
    }

    /* Free old OID tree if it exists. */
    FreeOIDTree(oid_tree_ele_root);

    oid_tree_ele_root = NULL;

    /* First count the number of entries and allocate them */

    for (i = 0; mib_oid_table[i].name != NULL; i++);

    oid_tree_node_storage = (OID_TREE_ELE *) malloc(sizeof(OID_TREE_ELE) * i);
    if (oid_tree_node_storage == NULL) {
	DPRINTF((APWARN, "Error allocating oid_tree_node_storage\n"));
	return;
    }
    otns_temp = oid_tree_node_storage;

    for (i = 0; mib_oid_table[i].name != NULL; i++) {
	AddEleToTree(&(mib_oid_table[i]));
    }
}

/*
 * MergeOIDTree:
 *
 * Same an InitOIDTree, but does not discard old tree.
 */
void
MergeOIDTree(start)
    int             start;
{
/*
 * For now we will just call InitOIDTree to make it work, although it
 * will run slower.  Later we will maintain several oid_tree_node_storage
 * variables (an array), and just add an additional one when we call this.
 * Then the InitOIDTree function would free all the elements of the
 * oid_tree_node_storage array.
 */
    InitOIDTree();
}

OID_TREE_ELE   *
CreateTreeEle(sid, num_str_len, mot)
    unsigned long   sid;
    int             num_str_len;
    struct MIB_OID *mot;
{
    OID_TREE_ELE   *ele;
    if (num_str_len) {
	ele = (OID_TREE_ELE *) malloc(sizeof(OID_TREE_ELE));
	if (ele == NULL) {
	    DPRINTF((APWARN, "malloc error\n"));
	    return (NULL);
	}
	ele->sid_value = sid;
	ele->oid_name = unassigned_text;
	ele->oid_number_str = NULL;

    }
    else {
	ele = otns_temp;
	otns_temp++;
	ele->sid_value = sid;
	ele->oid_name = mot->name;
	ele->oid_number_str = mot->number;

    }
    ele->next = NULL;
    ele->first_descendent = NULL;
    return (ele);
}

int
AddEleToTree(mot)
    struct MIB_OID *mot;
{
    char           *num_str, *sid_str;
    OID_TREE_ELE   *parent, *node, *ele, *prev;
    unsigned long   sid;
    int             num_str_len;

/* STEP 0 */
    parent = NULL;
    node = oid_tree_ele_root;
    prev = NULL;
    num_str = mot->number;

/* STEP 1 */
step_1:
    /* Extract SID from the OID string */
    if (num_str[0] == '\0') {
	DPRINTF((APERROR, "AddEleToTree:  bad entry in mib_oid_table.\n"));
	DPRINTF((APERROR, "  Severe Error.\nName: %s \t Number: %s\n",
		 mot->name, mot->number));
	return (-1);
    }
    if (num_str[0] == '0') {
	if ((num_str[1] == 'x') || (num_str[1] == 'X')) {
	    if ((ParseSubIdHex((unsigned char **) &num_str, &sid)) < 0) {
		DPRINTF((APWARN, "AddEleToTree, hex:\n"));
		return (-1);
	    }
	}
	else {
	    if ((ParseSubIdOctal((unsigned char **) &num_str, &sid)) < 0) {
		DPRINTF((APWARN, "AddEleToTree, octal:\n"));
		return (-1);
	    }
	}
    }
    else if (num_str[0] >= '0' && num_str[0] <= '9') {
	if ((ParseSubIdDecimal((unsigned char **) &num_str, &sid)) < 0) {
	    DPRINTF((APWARN, "AddEleToTree, decimal:\n"));
	    return (-1);
	}
    }
    else {
	DPRINTF((APWARN, "AddEleToTree, bad character: %d, %s\n",
		 num_str[0], num_str));
	return (-1);
    }
    if (num_str[0] == '.') {
	num_str++;
    }
    num_str_len = (num_str[0] == '\0') ? 0 : 1;

    if (node == NULL) {
	ele = CreateTreeEle(sid, num_str_len, mot);
	if (parent == NULL) {
	    oid_tree_ele_root = ele;
	}
	else {
	    parent->first_descendent = ele;
	}


	node = ele;
    }
    else {
	while (1) {
	    if (sid < node->sid_value) {
		ele = CreateTreeEle(sid, num_str_len, mot);
		if (ele == NULL) {
		    return (-1);
		}
		if (parent == NULL) {
		    if (prev == NULL) {
			oid_tree_ele_root = ele;
		    }
		    else {
			prev->next = ele;
		    }
		    ele->next = node;
		}
		else {
		    if (prev == NULL) {
			parent->first_descendent = ele;
		    }
		    else {
			prev->next = ele;
		    }
		    ele->next = node;
		}


		node = ele;
	    }

	    if (sid > node->sid_value) {
		if (node->next == NULL) {
		    ele = CreateTreeEle(sid, num_str_len, mot);
		    node->next = ele;


		    prev = node;
		    node = ele;
		}
		else {
		    prev = node;
		    node = node->next;
		    if (node != NULL) {
			continue;
		    }
		}
	    }
	    break;
	}
    }

    if (num_str_len == 0) {
	if (node != NULL) {
	    if (node->oid_name == unassigned_text) {
		/* We must replace the node with a real node */
		ele = CreateTreeEle(sid, num_str_len, mot);


		ele->next = node->next;
		ele->first_descendent = node->first_descendent;
		if (prev != NULL) {
		    prev->next = ele;
		}
		else {
		    if (parent != NULL) {
			parent->first_descendent = ele;
		    }
		}


		free(node);
	    }
	    else {
		/*
		 * This is a collision.  We should check if the contents of
		 * mot exactly match the contents of node, and if so, we can
		 * simply return.  Otherwise, we should call a hook function
		 * which returns a code indicating the action to take:
		 * MIB_COLLISION_DISCARD (0) discard mot contents
		 * MIB_COLLISION_REPLACE (1) replace node with mot
		 */
		/*
		 * For now we just assume the hook function returns 0.
		 */
	    }
	}
	return (0);
    }
    parent = node;
    node = node->first_descendent;
    prev = NULL;
    goto step_1;
}

/*
 * InitNameOIDHashArray:
 *
 * The former name (pre-snmp12.1.0.0 release) was init_name_oid_hash_array().
 */
void
InitNameOIDHashArray(start)
    int             start;
{
    int             i, j, hash;
    struct MIB_OID *temp_hash_ptr;

    /* for each line in the mib translation table */
    for (i = start; mib_oid_table[i].name != NULL; i++) {
	/* calculate hash */
	hash = 0;
	for (j = (strlen(mib_oid_table[i].name) - 1); j >= 0; j--) {
	    hash = (hash + mib_oid_table[i].name[j]) % NAME_OID_HASH_SIZE;
	}

	/* now add entry to hash array */
	if (name_oid_hash_array[hash] == NULL) {
	    name_oid_hash_array[hash] = &(mib_oid_table[i]);
	    name_oid_hash_array[hash]->hash_next = NULL;
	}
	else {
	    /* Walk to the end of the list, and put it there */
	    temp_hash_ptr = name_oid_hash_array[hash];
	    while (temp_hash_ptr->hash_next != NULL) {
		temp_hash_ptr = temp_hash_ptr->hash_next;
	    }
	    temp_hash_ptr->hash_next = &(mib_oid_table[i]);
	    temp_hash_ptr->hash_next->hash_next = NULL;
	}
    }
}

/*
 * NewMIBFromFile:
 *
 * The former name (pre-snmp12.1.0.0 release) was new_mib_from_file().
 */
int
NewMIBFromFile(filename)
    char           *filename;
{
    int             mib_entry_count, new_mib_entry_count;
    char            in_buffer[1024];
    char            name_buf[255], num_buf[255];
    FILE           *fp;
    struct MIB_OID *new_mib_table;
    int             i;

    /* sanity check file contents first */
    if ((fp = fopen(filename, "r")) == NULL) {
	DPRINTF((APERROR, "NewMIBFromFile: cannot open file: %s\n", filename));
	return (-1);
    }

    mib_entry_count = 0;
    while (fgets(in_buffer, sizeof(in_buffer), fp) != NULL) {
	if (in_buffer[0] != '"')
	    continue;

	if (ParseMIBLine(in_buffer, name_buf, num_buf) < 0) {
	    DPRINTF((APERROR, "NewMIBFromFile: Illegal line: %s\n", in_buffer));
	    (void) fclose(fp);
	    return (-1);
	}

	/* sanity check name characters */
	for (i = strlen(name_buf) - 1; i >= 0; --i)
	    if ((!isprint((int) name_buf[i])) || (name_buf[i] == '.')) {
		DPRINTF((APERROR, "NewMIBFromFile: Bad mib name: %s\n", name_buf));
		(void) fclose(fp);
		return (-1);
	    }

	/* sanity check number characters */
	for (i = strlen(num_buf) - 1; i >= 0; --i)
	    if (!(isxdigit((int) num_buf[i]) || (num_buf[i] == '.') ||
		  (((num_buf[i] == 'x') || (num_buf[i] == 'X')) &&
		   (num_buf[i - 1] == '0')))) {
		DPRINTF((APERROR, "NewMIBFromFile: Bad mib number: %s\n", num_buf));
		(void) fclose(fp);
		return (-1);
	    }

	/* not a comment, so this will be a new mib entry */
	mib_entry_count++;

    }				/* end of read wile loop */

    (void) fclose(fp);		/* close the file and reopen */


    /* allocate a new mib table */
    new_mib_table = (struct MIB_OID *) malloc((unsigned) (sizeof(struct MIB_OID) * (mib_entry_count + 1)));

    /* now initialize the new table */
    if ((fp = fopen(filename, "r")) == NULL) {
	DPRINTF((APERROR, "NewMIBFromFile: cannot open file: %s\n", filename));
	return (-1);
    }

    new_mib_entry_count = 0;
    while (fgets(in_buffer, sizeof(in_buffer), fp) != NULL) {
	if (in_buffer[0] != '"')
	    continue;

	if (ParseMIBLine(in_buffer, name_buf, num_buf) < 0) {
	    DPRINTF((APERROR, "NewMIBFromFile: Illegal line: %s\n", in_buffer));
	    (void) fclose(fp);
	    return (-1);
	}

	/* now allocate memory for these strings and add them to the table */
	new_mib_table[new_mib_entry_count].name = (char *) malloc((unsigned) (strlen(name_buf) + 1));
	(void) strcpy(new_mib_table[new_mib_entry_count].name, name_buf);

	new_mib_table[new_mib_entry_count].number = (char *) malloc((unsigned) (strlen(num_buf) + 1));
	(void) strcpy(new_mib_table[new_mib_entry_count].number, num_buf);

	new_mib_entry_count++;
    }

    /* close mib file */
    (void) fclose(fp);

    /* now terminate table */
    new_mib_table[new_mib_entry_count].name = NULL;
    new_mib_table[new_mib_entry_count].number = NULL;

    /* now blow away the old value for mib_oid_table */
    mib_oid_table = new_mib_table;

    /* and re-initialize the data structures */
    global_InitNewOIDRoutines = 0;
    InitNewOIDRoutines();

    return (0);
}


/*
 * ParseMIBLine:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_mib_line().
 */

int
ParseMIBLine(in_buffer, name_buf, num_buf)
    char           *in_buffer;
    char           *name_buf;
    char           *num_buf;
{
    char           *t1, *t2;

    /* add some error checking later */

    t1 = strchr(in_buffer, '"');
    t2 = strchr(&t1[1], '"');
    *t2 = '\0';
    (void) strcpy(name_buf, &t1[1]);

    t1 = strchr(&t2[1], '"');
    t2 = strchr(&t1[1], '"');
    *t2 = '\0';
    (void) strcpy(num_buf, &t1[1]);

    return (0);
}

/*
 * MergeMIBFromFile:
 *
 * The former name (pre-snmp12.1.0.0 release) was merge_mib_from_file().
 */
int
MergeMIBFromFile(filename)
    char           *filename;
{
    int             orig_mib_entry_count, new_mib_entry_count;
    char            in_buffer[1024];
    char            name_buf[255], num_buf[255];
    FILE           *fp;
    struct MIB_OID *new_mib_table;
    int             i;

    /* sanity check file contents first */
    if ((fp = fopen(filename, "r")) == NULL) {
	DPRINTF((APERROR, "MergeMIBFromFile: cannot open file: %s\n", filename));
	return (-1);
    }

    new_mib_entry_count = 0;
    while (fgets(in_buffer, sizeof(in_buffer), fp) != NULL) {
	if (in_buffer[0] != '"')
	    continue;

	if (ParseMIBLine(in_buffer, name_buf, num_buf) < 0) {
	    DPRINTF((APERROR, "MergeMIBFromFile: Illegal line: %s\n", in_buffer));
	    (void) fclose(fp);
	    return (-1);
	}

	/* sanity check name characters */
	for (i = strlen(name_buf) - 1; i >= 0; --i)
	    if ((!isprint((int) name_buf[i])) || (name_buf[i] == '.')) {
		DPRINTF((APERROR, "MergeMIBFromFile: Bad mib name: %s\n", name_buf));
		(void) fclose(fp);
		return (-1);
	    }

	/* sanity check number characters */
	for (i = strlen(num_buf) - 1; i >= 0; --i)
	    if (!(isxdigit((int) num_buf[i]) || (num_buf[i] == '.') ||
		  (((num_buf[i] == 'x') || (num_buf[i] == 'X')) &&
		   (num_buf[i - 1] == '0')))) {
		DPRINTF((APERROR, "MergeMIBFromFile: Bad mib number: %s\n", num_buf));
		(void) fclose(fp);
		return (-1);
	    }

	/* not a comment, so this will be a new mib entry */
	new_mib_entry_count++;

    }				/* end of read wile loop */

    (void) fclose(fp);		/* close the file and reopen */

    /* Now count how many in current MIB */
    for (orig_mib_entry_count = 0; mib_oid_table[orig_mib_entry_count].name != NULL; orig_mib_entry_count++);

    /* allocate a new mib table */
    new_mib_table = (struct MIB_OID *) malloc((unsigned) (sizeof(struct MIB_OID) * (orig_mib_entry_count + new_mib_entry_count + 1)));


    /* Load new mib with contents of current MIB */
    new_mib_entry_count = 0;
    while (mib_oid_table[new_mib_entry_count].name != NULL) {

	/* now allocate memory for these strings and add them to the table */
	new_mib_table[new_mib_entry_count].name = (char *) malloc((unsigned) (strlen(mib_oid_table[new_mib_entry_count].name) + 1));
	(void) strcpy(new_mib_table[new_mib_entry_count].name, mib_oid_table[new_mib_entry_count].name);

	new_mib_table[new_mib_entry_count].number = (char *) malloc((unsigned) (strlen(mib_oid_table[new_mib_entry_count].number) + 1));
	(void) strcpy(new_mib_table[new_mib_entry_count].number, mib_oid_table[new_mib_entry_count].number);

	new_mib_entry_count++;
    }				/* end of loading new mib with current mib */

    /* now add the new table */
    if ((fp = fopen(filename, "r")) == NULL) {
	DPRINTF((APERROR, "MergeMIBFromFile: cannot open file: %s\n", filename));
	return (-1);
    }

    while (fgets(in_buffer, sizeof(in_buffer), fp) != NULL) {
	if (in_buffer[0] != '"')
	    continue;

	if (ParseMIBLine(in_buffer, name_buf, num_buf) < 0) {
	    DPRINTF((APERROR, "MergeMIBFromFile: Illegal line: %s\n", in_buffer));
	    (void) fclose(fp);
	    return (-1);
	}

	/* now allocate memory for these strings and add them to the table */
	new_mib_table[new_mib_entry_count].name = (char *) malloc((unsigned) (strlen(name_buf) + 1));
	(void) strcpy(new_mib_table[new_mib_entry_count].name, name_buf);

	new_mib_table[new_mib_entry_count].number = (char *) malloc((unsigned) (strlen(num_buf) + 1));
	(void) strcpy(new_mib_table[new_mib_entry_count].number, num_buf);

	new_mib_entry_count++;
    }

    /* close mib file */
    (void) fclose(fp);

    /* now terminate table */
    new_mib_table[new_mib_entry_count].name = NULL;
    new_mib_table[new_mib_entry_count].number = NULL;

    /* now blow away the old value for mib_oid_table */
    mib_oid_table = new_mib_table;

    /* and re-initialize the data structures */
    InitNewOIDRoutines();

    return (0);
}


#endif				/* NEW_OID_LIB */


