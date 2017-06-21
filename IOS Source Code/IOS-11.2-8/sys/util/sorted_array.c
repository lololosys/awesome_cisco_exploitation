/* $Id: sorted_array.c,v 3.2.62.1 1996/03/18 22:31:02 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/util/sorted_array.c,v $
 *------------------------------------------------------------------
 * sorted_array.c -- manipulation of sorted arrays
 *
 * January 1995, Ravishankar Chandrasekaran
 *
 * Copyright (c) 1987-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sorted_array.c,v $
 * Revision 3.2.62.1  1996/03/18  22:31:02  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  11:00:15  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:45:26  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  17:56:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:43:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/06  14:23:55  jacobt
 * CSCdi36607:  need show memory summary command
 *
 * Revision 2.1  1995/06/07  23:13:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "sorted_array.h"

/*
 * sorted_array_add
 *
 * add an entry to a sorted array.
 * Bounds checking on the array is not performed.
 */
void
sorted_array_add (ulong *array, int *count, ulong number)
{
    int index, j;

    for (index = 0; index < *count; ++index) {
	if (array[index] < number)
	    continue;
	if (array[index] == number)
	    return;
	for (j = *count; j > index; --j)
	    array[j] = array[j-1];
	break;
    }
    
    array[index] = number;
    (*count)++;
}

/*
 * sorted_array_get_index
 * Find the FIRST occurence of  an entry in an array.
 *
 * If an entry (number) is found in array
 *         set *pindex to the index of the entry in the array and return TRUE.
 * If the entry  is not found
 *         set *pindex to the index of the insertion  and return FALSE.
 */

#define MIN_BINARY_SEARCH 4

boolean
sorted_array_get_index (ulong *array, int count, int *pindex, ulong number)
{
    int i,j, k;

    i = 0; 
    j = count - 1;
    while (TRUE) {
	if (j - i < MIN_BINARY_SEARCH) {
	    for (; i <= j; i++) {
		if (array[i] < number){
		    continue;
		}
		if (array[i] > number) {
		    *pindex = i;
		    return (FALSE);
		}
		*pindex = i;
		return(TRUE);
	    }	    
	    *pindex = i;
	    return (FALSE);
	}
	k = (i + j) >> 1;
	if (number > array[k]) {
	    i = k + 1;
	    continue;
	} else if (number < array[k]) {
	    j = k - 1;
	    continue;
	}
	while (k > 0 && array[k - 1] == number) {
	    k--;
	}
	*pindex = k;
	return(TRUE);
    }
}

/*
 * sorted_array_get_index_2
 *
 * If an entry (number_1, number_2) is found in (array_1, array_2)
 *         set *pindex to the index of the entry in the arrays and return TRUE.
 * If the entry  is not found
 *         set *pindex to the index of insertion  and return FALSE.
 */

boolean
sorted_array_get_index_2 (ulong *array_1, ulong *array_2, int count, int *pindex,
			  ulong number_1, ulong number_2)
{

    if (!sorted_array_get_index(array_1, count, pindex, number_1)) {
	return(FALSE);
    }
    for (; array_1[*pindex] == number_1 && *pindex < count; ++*pindex) {
	if (array_2[*pindex] < number_2)
	    continue;
	if (array_2[*pindex] > number_2) {
	    return (FALSE);
	} 
	return (TRUE);
    }
    return (FALSE);
}

/*
 * sorted_array_add_2
 *
 * add an entry(number_1, number_2)
 *  to a  pair of sorted arrays (array_1, array2).
 * Bounds checking on the array is not performed.
 */
void
sorted_array_add_2 (ulong *array_1, ulong *array_2, int *pcount,
		    ulong number_1, ulong number_2)
{
    int index, j;

    if (sorted_array_get_index_2(array_1, array_2, *pcount, &index, number_1, number_2)) {
	return;
    }

    for (j = *pcount; j > index; --j) {
	array_1[j] = array_1[j-1];
	array_2[j] = array_2[j-1];
    }
    array_1[index] = number_1;
    array_2[index] = number_2;
    (*pcount)++;
}


/*
 * sorted_array_partial_compare
 *
 * Compare if elements of 1st array appear in the second.
 * Note: It is assumed that both arrays are sorted.
 */
boolean
sorted_array_partial_compare (ulong *array1, int count1,
			      ulong *array2, int count2) 
{
    int i, j;

    /*
     * First is larger than the second.
     */
    if (count1 > count2)
	return(FALSE);

    /*
     * Equal length, as they are sorted just compare.
     */
    if (count1 == count2) {
	if (bcmp(array1, array2, ltob(count1)))
	    return(FALSE);
	return(TRUE);
    }

    /*
     * No choice, brute force.
     */
    for (i = 0; i < count1; ++i) {
	for (j = 0; j < count2; ++j)
	    if (array1[i] == array2[j])
		break;
	if (j >= count2)
	    return(FALSE);
    }
    return(TRUE);
}
