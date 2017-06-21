/* $Id: generic_objects.c,v 3.2.58.1 1996/05/09 14:32:07 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/lane/generic_objects.c,v $
 *------------------------------------------------------------------
 * Index and linked list objects library
 *
 * November 1994, Gee Akyol
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: generic_objects.c,v $
 * Revision 3.2.58.1  1996/05/09  14:32:07  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.86.1  1996/04/27  07:06:02  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.76.1  1996/04/08  01:56:28  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.64.1  1996/03/22  22:53:43  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.48.1  1996/03/05  06:42:29  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.36.1  1996/02/27  21:09:51  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.14.2  1996/01/17  20:26:18  cakyol
 * - Index_add now returns the inserted element or the one already
 *   present.  More convenient for handling duplicates.
 * - Tidy up the parser help strings that involve atm address
 *   template.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.14.1  1996/01/11  01:45:58  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.10.1  1995/12/29  01:06:22  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2  1995/11/17  17:49:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:22:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:12:13  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/07/14  19:10:36  cakyol
 * CSCdi37119:  lane generic_objects.c should use master.h instead of
 * externs
 * - done.
 *
 * Revision 2.2  1995/07/13  21:39:28  cakyol
 * CSCdi37017:  LECS should advertise its coming up and going down
 * fixed.
 *
 * CSCdi37071:  lecs should migrate to "malloc_named"
 * fixed.
 *
 * Revision 2.1  1995/06/07  21:18:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "generic_objects.h"

/* by how much to grow, every time something needs to grow */
#define GROWTH_SIZE			128

/* check NULL before assignment */
#define setPointer(ptr, value) \
    if (ptr) *(unsigned long *) ptr = (unsigned long) (value)

/* copy full words; handles overlapping regions */
static void copyFullWords (void **src, void **dst, int size)
{
    if (dst < src)
	while (size-- > 0) *dst++ = *src++;
    else
    {
	src += size;
	dst += size;
	while (size-- > 0) *(--dst) = *(--src);
    }
}

/* size is in bytes */
static void *MALLOC (void *ptr, int newSize)
{
    void *newPtr;

    if (newSize <= 0)
    {
	if (ptr) free(ptr);
	return NULL;
    }
    if (ptr == NULL)
	newPtr = malloc_named(newSize, "newSize");
    else
	newPtr = realloc(ptr, newSize);
    return newPtr;
}

/***************************************************************
****************************************************************
****************************************************************
****************************************************************
**				INDEX OBJECT
****************************************************************
****************************************************************
****************************************************************
***************************************************************/

typedef struct Index
{
    int n;			/* # of elements in index */
    int max;			/* maximum capacity */
    int fixedSize;		/* fixed size if set */
    void **elements;		/* user data pointers */
    CMP compareFunction;	/* comparison function */

} Index;

/*
** THIS IS THE GUTS OF THE WHOLE OBJECT.  
** IT HAS TO BE HIGHLY OPTIMAL
**
** searches an EXACT match in the index that matches the 
** key "key".  Returns the index if found, otherwise returns 
** -1.  If the element is NOT found, the routine returns the 
** insertion point of the element in "insertionPoint".  This 
** is the place that the element should be inserted into if 
** it was to be put into the index object.  On the other hand, 
** if the element is found, the value in "insertionPoint"
** is NOT changed.  Note that "insertionPoint" can be specified 
** as NULL.
**
** If "cmpOverride" is non NULL, then it is used as the 
** comparison function instead of the normal one.  This is 
** only for the duration of this single call however.
*/
static int Index_search (IndexPtr ind, void *key, 
    int *insertionPoint, CMP cmpOverride)
{
    void **elems;
    int mid, diff, lo, hi;
    CMP cmp;

    lo = mid = diff = 0;
    hi = (ind->n)-1;
    elems = ind->elements;
    cmp = cmpOverride ? cmpOverride : ind->compareFunction;

    /* binary search */
    while (lo <= hi)
    {
	/* integer divide by 2, but shift right by 1 is faster */
	mid = (hi+lo) >> 1;

	if ((diff = (*cmp) (key, elems [mid])) >= 0)
	{
	    if (diff)
		lo = mid + 1;
	    else
		return mid;
	}
	else
	    hi = mid - 1;
    }

    /*
    ** not found, but record where the element should be 
    ** inserted, in case it was required to be put into 
    ** the index.
    */
    setPointer(insertionPoint, (diff>0 ? mid+1 : mid));
    return (-1);
}

/*
** create an index object.  If size == 0, size
** is dynamic, the index will grow until end of
** memory, otherwise it is artificially limited in size.
*/
IndexPtr Index_alloc (CMP cmp, int size)
{
    IndexPtr ind;
    int initialSize;
    int bytesRequired;

    /* sanity */
    if ((size < 0) || (cmp == NULL)) return NULL;

    /* allocate the required things */
    if ((ind = malloc_named(sizeof(Index), "Index")) == NULL) 
	return NULL;

    /* ok checks have passed, fill the stuff now */
    initialSize = size ? size : GROWTH_SIZE;
    bytesRequired = sizeof(void*) * initialSize;
    ind->elements = malloc_named(bytesRequired, "bytesRequired");
    if (ind->elements == NULL)
    {
	free(ind);
	return NULL;
    }

    /* set the other stuff */
    ind->n = 0;
    ind->max = initialSize;
    ind->fixedSize = size;
    ind->compareFunction = cmp;

    return ind;
}

/*
** special case ulong index routines
*/
static int ulongCompare (void *key, void *elem)
{ return ((unsigned long) key) - ((unsigned long) elem); }

IndexPtr Ulong_Index_alloc (void)
{ return Index_alloc(ulongCompare, 0); }

/*
** delete an index object
*/
void Index_destroy (IndexPtr ind)
{
    if (ind)
    {
	if (ind->elements) free(ind->elements);
	free(ind);
    }
}

/*
** how many elements in the index ?
*/
int Index_numberOfElements (IndexPtr ind)
{ return ind->n; }

/*
** get the pointer to the element stored at n'th entry
*/
void *Index_getElementPointer (IndexPtr ind, int n)
{
    if ((n < 0) || (n >= ind->n))
	return NULL;
    else
	return(ind->elements [n]);
}

/*
** add a new element to our index.  The "userData" is the POINTER
** to the actual element we are trying to add to the index.
** If the element is successfully added, element pointer is
** returned.  If a duplicate already exists, its pointer is
** returned.  If memory deprivation occurs, NULL is returned.
*/
void *Index_add (IndexPtr ind, void *key, void *userData)
{
    int insertionPoint;
    int size, idx;
    void **source;

    /*
    ** see if element is already there and if not,
    ** note the insertion point in "insertionPoint".
    */
    idx = Index_search(ind, key, &insertionPoint, NULL);
    if (idx != (-1)) return ind->elements [idx];

    /* if ran out of space see what we can do */
    if (ind->n >= ind->max)
    {
	/* if fixed size, nothing we can do */
	if (ind->fixedSize)
	    return NULL;
	/* try and expand */
	else
	{
	    void *temp;
	    int newMax, newSize;

	    newMax = ind->max + GROWTH_SIZE;
	    newSize = newMax * sizeof(void*);
	    if ((temp = MALLOC(ind->elements, newSize)) == NULL)
		return NULL;
	    ind->elements = temp;
	    ind->max = newMax;
	}
    }

    /*
    ** shift all of the pointers after 
    ** "insertionPoint" right by one 
    */
    source = &(ind->elements [insertionPoint]);
    if ((size = ind->n - insertionPoint) > 0)
	copyFullWords(source, (source+1), size);
    
    /* insert the new element pointer into its rightful place */
    *source = userData;

    /* increment element count */
   (ind->n)++;

    /* successfully completed */
    return userData;
}

/*
** first search the element that matches the parameter "key".
** If found, remove the entry from the index.  Note that the
** user data itself is NOT removed, only its representation from
** the index itself is removed.
**
** The return value is the actual user data that was previously
** stored there before. It is anticipated that if an element is
** removed from the index, the actual data itself is likely to be
** removed soon as well, and hence the return value of the 
** original user supplied data, in case it is needed.  If the 
** element cannot be found in the index, NULL is returned.
*/
void *Index_remove (IndexPtr ind, void *key)
{
    int index;
    int size;
    void *userData;

    /* first see if it is there */
    if ((index = Index_search(ind, key, NULL, NULL)) == (-1))
	return NULL;

    /* found the element, remember the user data for returning */
    userData = ind->elements [index];

    /* decrement count; one less element */
   (ind->n)--;

    /* pull the elements AFTER "index" to the left by one */
    if ((size = ind->n - index) > 0)
    {
	void **source = &(ind->elements [index+1]);
	copyFullWords(source, (source-1), size);
    }

    /* if a lot of free space remaining, shrink to save memory */
    if (! ind->fixedSize)
    {
	if (ind->max - ind->n >= GROWTH_SIZE)
	{
	    int newSize;

	    ind->max -= GROWTH_SIZE;
	    newSize = ind->max * sizeof(void*);

	    /* this can NOT fail since we are shrinking */
	    ind->elements = MALLOC(ind->elements, newSize);
	}
    }

    return userData;
}

/*
** variations on getting element pointers from the index.  
** If found, the pointer to the element will be returned, 
** otherwise NULL.
*/
void *Index_searchElement (IndexPtr ind, void *key, CMP cmpOverride)
{
    int index;

    if ((index = Index_search(ind, key, NULL, cmpOverride)) == -1)
	return NULL;
    else
	return ind->elements [index];
}

/* 
** This routine is used for the SNMP get-next table traversal.  
** The element whose index immediately proceeds the key will 
** be returned.  NULL is return if there is no successor.
*/
void *Index_searchNextElement (IndexPtr ind, void *key)
{
    int index, next;
    index = Index_search (ind, key, &next, NULL);
    if (index == -1)
    {
	/* we ran off the end of the table */
	if (next == Index_numberOfElements(ind))
	    return NULL;
	else 
	    return ind->elements [next];
    }
    
    /* this was the last entry in the table */
    if (index == (Index_numberOfElements(ind) - 1))
	return NULL; 
		
    return ind->elements [index + 1];
}

/*
** performs a linear search thru the index, using the specified 
** comparison function.  This routine searches for EQUALITY
** only.  It starts searching at the specified index.  If an
** entry is found, the index is returned and userPointer is set 
** to the stored value.  Otherwise, -1 is returned and userPointer
** is set to NULL.
**
** The cmpFunction function is expected to return a true(non 0)
** value for equal and 0 for non equal.
*/
int Index_linearSearch (IndexPtr ind, int startIndex, void *key, 
     CMP cmpFunction, void *userPointer)
{
    register int i;

    if (startIndex >= 0)
    {
	for (i = startIndex; i < ind->n; i++)
	    if ((*cmpFunction)(key, ind->elements [i]))
	    {
		setPointer(userPointer, ind->elements [i]);
		return i;
	    }
    }

    /* not found or bad index */
    setPointer(userPointer, NULL);
    return -1;
}

/*****************************************************************
******************************************************************
******************************************************************
******************************************************************
**			DOUBLY LINKED LIST OBJECT
******************************************************************
******************************************************************
******************************************************************
******************************************************************/

/*
** this MUST be in the .h file.
** Even though it exposes the object itself in the .h file, 
** it has to be there since List object users have to directly 
** include it in their C structures.  I could have made a pointer 
** out of this, but including it directly is less wasteful of 
** memory.
**
** The elements are internally represented as a doubly linked 
** list.
**
** include ListEntry as the VERY FIRST component of ANY structure
** that you want to make part of a list object.
**
**	typedef struct ListEntry
**	{
**	    struct ListEntry *next;
**	    struct ListEntry *prev;
**
**	} ListEntry;
**
*/

/*
** the doubly linked list management structure
*/
typedef struct List
{
    int count;
    ListEntry *head;
    ListEntry *tail;

} List;

/*
** allocate a doubly linked list management structure
*/
ListPtr List_alloc ()
{
    ListPtr lptr;

    lptr = malloc_named(sizeof(List), "List");
    if (lptr == NULL) return NULL;
    lptr->count = 0;
    lptr->head = lptr->tail = NULL;
    return lptr;
}

/*
** free a doubly linked list management structure
*/
void List_destroy (ListPtr lptr)
{ if (lptr) free(lptr); }

/*
** how many elements are there in a doubly linked 
** list management structure at the moment
*/
int List_size (ListPtr lptr)
{ return lptr ? lptr->count : 0; }

/*
** read (pop) an element from the FRONT of the list
*/
void *List_read (ListPtr lptr)
{
    void *elem;
    
    if (lptr == NULL) return NULL;
    elem = lptr->head;
    if (elem == NULL)
	return NULL;
    else
    {
	List_remove(lptr, elem);
	return elem;
    }
}

/*
** APPEND an element to the END of a doubly linked list
** management structure.  Always added to the END.
*/
void List_addToEnd (ListPtr lptr, void *element)
{
    ListEntryPtr elem = element;

    if ((lptr == NULL) || (elem == NULL)) return;
    if (lptr->count <= 0)
    {
	lptr->head = lptr->tail = elem;
    }
    else
    {
	elem->prev = lptr->tail;
	lptr->tail->next = elem;
	lptr->tail = elem;
    }
    (lptr->count)++;
}

/*
** INSERT an element to the FRONT of a doubly linked list
** management structure.  Always added to the FRONT.
*/
void List_addToFront (ListPtr lptr, void *element)
{
    ListEntryPtr elem = element;

    if ((lptr == NULL) || (elem == NULL)) return;
    if (lptr->count <= 0)
    {
	lptr->head = lptr->tail = elem;
    }
    else
    {
	elem->next = lptr->head;
	lptr->head->prev = elem;
	lptr->head = elem;
    }
    (lptr->count)++;
}

/*
** inserts the element specified by "pentry" AFTER the entry
** identified by "pafter".  If "after" is NULL, the entry will
** be inserted to the START of the list.  The user MUST ensure 
** that the element specified by "pafter" is in fact in the 
** list specified by "lptr".  Also note that "pentry" SHOULD 
** NOT already be in the list specified by "lptr".
*/
void List_addAfter (ListPtr lptr, void *pafter, void *pentry)
{
    ListEntryPtr after = pafter, entry = pentry;

    if ((lptr == NULL) || (pentry == NULL)) return;
    if (after == NULL)
	List_addToFront(lptr, entry);
    else
    if (after == lptr->tail)
	List_addToEnd(lptr, entry);
    else
    {
	entry->next = after->next;
	entry->prev = after;
	after->next->prev = entry;
	after->next = entry;
	(lptr->count)++;
    }
}

/*
** inserts the element specified by "pentry" BEFORE the entry
** identified by "pbefore".  If "pbefore" is NULL, the entry will
** be inserted to the END of the list.  The user MUST ensure that 
** the element specified by "pbefore" is in fact in the list 
** specified by "lptr".  Also note that "pentry" SHOULD NOT 
** already be in the list specified by "lptr".
*/
void List_addBefore (ListPtr lptr, void *pbefore, void *pentry)
{
    ListEntryPtr before = pbefore, entry = pentry;

    if ((lptr == NULL) || (pentry == NULL)) return;
    if (before == NULL)
	List_addToEnd(lptr, entry);
    else
    if (before == lptr->head)
	List_addToFront(lptr, entry);
    else
    {
	before->prev->next = entry;
	entry->prev = before->prev;
	entry->next = before;
	before->prev = entry;
	(lptr->count)++;
    }
}

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
boolean List_insertOrdered (ListPtr lptr, CMP cmpFunction, 
    void *elem, boolean allowDuplicates)
{
    register void *listElem;
    int cmpValue;

    /* sanity */
    if ((lptr == NULL) || (elem == NULL)) return FALSE;

    /* if no elements in the list so far, just insert */
    if (lptr->count <= 0)
    {
	List_addToEnd(lptr, elem);
	return TRUE;
    }

    /* search the appropriate place */
    listElem = lptr->head;
    while (listElem != NULL)
    {
	cmpValue = (*cmpFunction) (elem, listElem);

	/* if duplicate rank, check if we are allowed */
	if (cmpValue == 0)
	{
	    if (allowDuplicates)
	    {
		List_addBefore(lptr, listElem, elem);
		return TRUE;
	    }
	    else
		return FALSE;
	}
	else
	if (cmpValue < 0)
	{
	    List_addBefore(lptr, listElem, elem);
	    return TRUE;
	}

	listElem = ((ListEntry*) listElem)->next;
    }

    /*
    ** if we are here, we reached the end of the list, 
    ** so insert it right to the end.
    */
    List_addToEnd(lptr, elem);
    return TRUE;
}

/*
** returns the "rank" of the element in the list, like
** an array index.  First entry returns 0, last entry
** returns List_size(list)-1.  If NOT found in list,
** returns -1.
*/
int List_inList (ListPtr lptr, void *elem)
{
    register void *listElem;
    register int count = 0;

    /* sanity */
    if ((lptr == NULL) || (elem == NULL)) return (-1);

    /* check */
    listElem = lptr->head;
    while (listElem != NULL)
    {
	if (listElem == elem) return count;
	listElem = ((ListEntry*) listElem)->next;
	count++;
    }
    return (-1);
}

/*
** remove an element pointed to by "element" from the list
** adjusting the rest of the list.
*/
void List_remove (ListPtr lptr, void *element)
{
    ListEntryPtr elem = element;

    if ((lptr == NULL) || (elem == NULL)) return;
    if (elem->next == NULL)
    {
	/* only one in list; count=1 and elem is it */
	if (elem->prev == NULL)
	{
	    lptr->head = lptr->tail = NULL;
	}
	else
	/* at the end of list and count>1 */
	{
	    elem->prev->next = NULL;
	    lptr->tail = elem->prev;
	}
    }
    else
    {
	/* at the beginning of list and count>1 */
	if (elem->prev == NULL)
	{
	    lptr->head = elem->next;
	    elem->next->prev = NULL;
	}
	/* in the middle of list and count>1 */
	else
	{
	    elem->prev->next = elem->next;
	    elem->next->prev = elem->prev;
	}
    }

    /* clean out the pointers */
    elem->next = elem->prev = NULL;

    (lptr->count)--;
}

void List_clearLinks (void *listElem)
{
    ListEntryPtr elem = listElem;

    if (elem == NULL) return;
    elem->next = elem->prev = NULL;
}

void *List_head (ListPtr lptr)
{
    if (lptr == NULL) return NULL;
    return lptr->count <= 0 ? NULL : lptr->head; 
}

void *List_tail (ListPtr lptr)
{
    if (lptr == NULL) return NULL;
    return lptr->count <= 0 ? NULL : lptr->tail; 
}

void *List_next (void *elem)
{
    if (elem == NULL) return NULL;
    return((ListEntryPtr) elem)->next; 
}

