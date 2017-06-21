/* $Id: generic_objects.h,v 3.2.58.1 1996/05/09 14:32:08 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/lane/generic_objects.h,v $
 *------------------------------------------------------------------
 * index and linked list objects library
 *
 * November 1994, Gee Akyol
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: generic_objects.h,v $
 * Revision 3.2.58.1  1996/05/09  14:32:08  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.86.1  1996/04/27  07:06:03  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.76.1  1996/04/08  01:56:29  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.64.1  1996/03/22  22:53:45  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.48.1  1996/03/05  06:42:31  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.36.1  1996/02/27  21:09:53  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.14.2  1996/01/17  20:26:20  cakyol
 * - Index_add now returns the inserted element or the one already
 *   present.  More convenient for handling duplicates.
 * - Tidy up the parser help strings that involve atm address
 *   template.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.14.1  1996/01/11  01:46:00  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.10.1  1995/12/29  01:06:23  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2  1995/11/17  17:50:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:22:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:12:14  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  21:18:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef generic_objects_h
#define generic_objects_h

typedef int bool;

/***************************************************************
****************************************************************
****************************************************************
****************************************************************
**
**				INDEX OBJECT
**
** This object provides index functionality to any user defined
** structure.  It is very similar in nature to a database index
** concept.  The only thing the user has to do is to allocate the
** index and supply a "comparison function" during the allocation
** process.  This is the critical part of the object.  The user
** supplied comparison function should be implemented as efficiently
** as possible and have NO side effects (in other words, should NOT
** change the object in any way).  The index object then applies
** the user supplied comparison function, by passing the searched
** object "key" and the stored element pointers, in a binary search
** to locate the searched object itself.  The comparison function
** should return 3 types of outputs, 0 for equality, < 0 for 1st 
** argument being less than the second argument and > 0 otherwise.
**
** WARNING:  The comparison routine should be written to accept the
**	     "key" parameter FIRST followed by the "user supplied"
**	     element pointer SECOND.
**
** Note that index object stores an element with the same key ONLY 
** once, ie, duplicate objects are not allowed.
**
****************************************************************
****************************************************************
****************************************************************
****************************************************************/

typedef struct Index *IndexPtr;

/* syntax of comparison function */
typedef int (*CMP) (void *key, void *elem);

/*
** create an Index object.
**
** parameters:
**
**	cmp	pointer to the user supplied comparison 
**		function.
**
**	size	The max number of entries allowed in the index.
**		If this is 0, the index will grow indefinitely
**		automatically as it needs to until it runs out
**		of memory.  If NON 0, it will have this fixed size.
**		In the fixed size case, any further additions to 
**		the index will be rejected.
**
** return value:
**
**	pointer to the Index object if successfull, otherwise NULL.
**
*/
extern IndexPtr Index_alloc (CMP cmp, int size);

/*
** special index for native ulong comparisons.
** The comparison for this is internally provided.
*/
extern IndexPtr Ulong_Index_alloc (void);

/*
** destroy the index object specified by "ind".  Note that this 
** function frees ONLY the index object itself and NOT the 
** individual user specified elements that the index represents.
*/
extern void Index_destroy (IndexPtr ind);

/* 
** at any time, return the number of elements in the index
*/
extern int Index_numberOfElements (IndexPtr ind);

/*
** add the element specified by "key" and identified by 
** "userData", to the index object.  Note that the user 
** specified comparison function is called here by passing 
** the key and a pointer to the user data repeatedly (in 
** that specific order) for a binary search algorithm.  If 
** the list was fixed size AND full, or if the list cannot 
** grow any more as a result of memory exhaustion, NULL is 
** returned.  If the element was ALREADY in the list, the 
** already existing element pointer is returned.  Otherwise 
** the user data specified in parameter "userData" is returned.
*/
extern void *Index_add (IndexPtr ind, void *key, void *userData);

/*
** remove the entry represented by the key "key" from the index.
** Note that ONLY the representation from the index itself is 
** removed, the user object itself is NOT touched in any way.
** A successfull removal returns the "original" user specified 
** data pointer when the object was stored.  This is because it 
** is anticipated that once an entry from the index is removed, 
** this is usually followed by removing the object itself, hence 
** kill two birds with one stone, ie. search only once for two 
** removals.  NULL is returned if object was NOT in the index.
*/
extern void *Index_remove (IndexPtr ind, void *key);

/*
** search and return a pointer to user element identified by "key".
** NULL is returned if object was not in the index.  If cmpOverride
** is non NULL, this will be used for comparisons INSTEAD of the
** function supplied with the Index_alloc call.  This will ONLY 
** occur temporarily during ONLY this single call.
*/
extern void *Index_searchElement (IndexPtr ind, void *key, 
    CMP cmpOverride);

/*
** search and return a pointer to the user element immediately 
** after the given "key" NULL is returned if there is no 
** successor.
*/
extern void *Index_searchNextElement (IndexPtr ind, void *key);

/*
** this routine returns the user pointer at "n'th" value in
** the index.  By successive n's, the user can iterate thru
** all the elements one at a time.  Note that the first valid
** n is 0, and the last valid n is (Index_numberOfElements () -1).
** This routine is good to use to iterate thru all the elements
** in the index object one at a time.
*/
extern void *Index_getElementPointer (IndexPtr ind, int n);

/*
** this routine performs a linear search thru all the elements
** in a n index object, starting from a user specified point.
** It performs an EXACT match policy.  The index value of the
** matching entry is returned and userPointer is set to the correct
** entry if found, otherwise -1 is returned and userPointer is set
** to NULL.  The user specified matching function is used for 
** performing the comparisons.
*/
extern int Index_linearSearch 
    (IndexPtr ind, int startIndex, void *key, 
     CMP cmpFunction, void *userPointer);

/***************************************************************
****************************************************************
****************************************************************
****************************************************************
**
**			DOUBLY LINKED LIST OBJECT
**
** "ListEntry" is a generic template to make any C structure
** part of a doubly linked list.  The requirement is that any C 
** structure, that wants to become part of such a list, MUST have 
** the component "ListEntry" as the FIRST component in that 
** structure.  At the creation time, the ListEntry component MUST 
** be initialized to NULL explicitly by the user.
**
** A "ListEntry" object can be in ONLY ONE list at a time.
**
** "List" is the actual list object management structure, which
** maintains the list of the user specified objects.
**
****************************************************************
****************************************************************
****************************************************************
***************************************************************/

typedef struct List *ListPtr;
typedef struct ListEntry
{
    struct ListEntry *next;
    struct ListEntry *prev;
} ListEntry, *ListEntryPtr;

/*
** allocate a linked list management object.  
** A pointer to the object is returned 
** if successfull, otherwise a NULL is returned.
*/
extern ListPtr List_alloc (void);

/*
** Free the linked list management object.  Note that ONLY the
** management object is removed.  Individual user elements are NOT
** touched in any way
*/
extern void List_destroy (ListPtr lptr);

/*
** returns how many elements are in the linked list
*/
int List_size (ListPtr lptr);

/*
** "pops" out the first entry from the linked list.  This operation
** REMOVES the head element from the list (does NOT remove the 
** element itself, but ONLY removes it from the list management 
** structure).  A NULL is returned if list is empty
*/
extern void *List_read (ListPtr lptr);

/*
** Add the user specified object "entry" to the END of the list
*/
extern void List_addToEnd (ListPtr lptr, void *entry);

/*
** Add the user specified object "entry" to the HEAD of the list
*/
extern void List_addToFront (ListPtr lptr, void *entry);

/*
** remove the user specified object "entry" from the list
*/
extern void List_remove (ListPtr lptr, void *entry);

/*
** Add the user specified object "entry" AFTER the entry 
** identified by "after".  If "after" is NULL, the element
** is inserted to the FRONT of the list.
*/
extern void List_addAfter (ListPtr lptr, void *after, void *entry);

/*
** Add the user specified object "entry" BEFORE the entry 
** identified by "before".  If "before" is NULL, the element
** is inserted to the END of the list.
*/
extern void List_addBefore (ListPtr lptr, void *before, 
    void *entry);

/*
** This routine inserts into the list, depending on the
** ordering specified by the user specified function.
** This function is similar to the one used in the Index
** object and its syntax and semantics should behave
** exactly the same.
**
** If "allowDuplicates" is TRUE, duplicate ranks are allowed
** otherwise, the insertion is not made and FALSE is returned.
*/
extern boolean List_insertOrdered (ListPtr lptr, CMP cmpFunction, 
    void *elem, boolean allowDuplicates);

/*
** confirms or denies depending if the element "elem" is in
** the specified list "lptr".  A serial search is done thru
** the list, so NOT a very efficient implementation, but useful.
** Returns TRUE for "yes in list", FALSE for "NO NOT in list".
*/
extern bool List_inList (ListPtr lptr, void *elem);

/*
** clear out the link pointers at the front of
** the structure
*/
extern void List_clearLinks (void *listElem);

/*
** return a pointer to the FIRST user object in the list.  This
** routine does NOT pop the element from the list, it can be used
** to "inspect" the object WITHOUT removing it from the list.
** Hence it is different from the routine "List_read".  A NULL
** is returned, if list is empty.
*/
extern void *List_head (ListPtr lptr);

/*
** similar to "List_head", this routine returns a pointer to the 
** LAST element in the list.  Again, as in the case of "List_head",
** it does NOT remove the entry from the list.
*/
extern void *List_tail (ListPtr lptr);

/*
** returns the element AFTER "entry".  A NULL is returned if this
** is the last element in the list.
*/
extern void *List_next (void *entry);

#define List_pop		List_read
#define List_get		List_read

#define List_write		List_addToEnd
#define List_append		List_addToEnd

#define List_prepend		List_addToFront
#define List_push		List_addToFront

#define List_first		List_head
#define List_last		List_tail

#define List_moveToFront(lst, element) \
    { \
	List_remove((lst), (element)); \
	List_addToFront((lst), (element)); \
    }

#define List_moveToEnd(lst, element) \
    { \
	List_remove((lst), (element)); \
	List_addToEnd((lst), (element)); \
    }

#define List_moveAfter(lst, after, element) \
    { \
	List_remove((lst), (element)); \
	List_addAfter((lst), (after), (element)); \
    }

#define List_moveBefore(lst, before, element) \
    { \
	List_remove((lst), (element)); \
	List_addBefore((lst), (before), (element)); \
    }

#endif /* generic_objects_h */

