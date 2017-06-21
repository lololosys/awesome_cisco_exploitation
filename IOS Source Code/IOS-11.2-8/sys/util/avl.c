/* $Id: avl.c,v 3.2.62.2 1996/06/05 21:11:53 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/util/avl.c,v $
 *------------------------------------------------------------------
 * avl.c -- AVL tree functions 
 *
 * October 1991, Dino Farinacci
 * January 1995, Tony Li
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: avl.c,v $
 * Revision 3.2.62.2  1996/06/05  21:11:53  ronnie
 * CSCdi39527:  wavl_walk calls avl_walk with wrong parameter
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/03/18  22:30:38  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:59:45  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:44:48  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  17:56:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:43:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/15  13:41:14  ppearce
 * CSCdi41826:  Crash on 2nd QLLC end-station connectIn to DSPU/DLSw+
 *   Add wrapper around avl_walk() to keep argptr intact for recursive call
 *
 * Revision 2.1  1995/06/07  23:12:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include "../util/avl.h"

#ifdef DEBUG 
boolean avl_debug = FALSE;
#endif


#if 0
/*
 * avl_sanity
 *
 * Crash if the node is insane.
 */

void avl_sanity (avl_node_type *node)
{
    if (!node)
	return;
    switch(node->balance) {
    case LEFT_HEAVY:
	if (!node->left)
	    crashdump(3);
	break;
    case BALANCED:
	if ((node->left && !node->right) ||
	    (!node->right && node->left))
	    crashdump(4);
	break;
    case RIGHT_HEAVY:
	if (!node->right)
	    crashdump(5);
	break;
    }
}
#else
#define avl_sanity(x)
#endif

/*
 * avl_do_walk
 *
 * This routine will walk the avl tree, calling the specified procedure
 * at each node.  The walk is done in order.
 *    
 * If the passed procedure returns FALSE - the walk will stop.  Returns
 * TRUE if the walk completed successfully, FALSE if it was aborted.
 *
 * NB: this routine is essentially duplicated in wavl.c.  If you make changes
 * here, you will almost certainly need to make them there as well.
 * 
 * Note: It is possible that the node being processed could be freed
 * while a subtree is being traversed or while "proc" is processing
 * the node. For example, if the contents of the node are printed the
 * calling task could block allowing another task to alter the tree.
 * For this reason the memory block associated with the specified tree
 * node is mem_locked() while it it processed.
 */

static boolean avl_do_walk (avl_node_type *element, 
			    avl_walker_type proc, 
			    va_list argptr)
{
    if (NULL == element) {
	return(TRUE);
    }

#ifdef DEBUG
    if (avl_debug) {
	buginf("\nAVL: Current node = %x, bal = %d", 
		element, element->balance);
    }
#endif

    mem_lock(element);

    if (element->left) {
#ifdef DEBUG
	if (avl_debug) {
            buginf("\nAVL: Go down left");
        }
#endif

        if (!avl_do_walk(element->left, proc, argptr) ||
            (mem_refcount(element) == 1)) {

            free(element);
            return(FALSE);
        }

#ifdef DEBUG
	if (avl_debug) {
	    buginf("\nAVL: Back up to %s", element);
	}
#endif
    }

    if (!(*proc)(element, argptr) || (mem_refcount(element) == 1)) {
        free(element);
        return(FALSE);
    }

    if (element->right) {
#ifdef DEBUG
	if (avl_debug) {
            buginf("\nAVL: Go down right");
        }
#endif

        if (!avl_do_walk(element->right, proc, argptr) ||
            (mem_refcount(element) == 1)) {
            free(element);
            return(FALSE);
        }

#ifdef DEBUG
	if (avl_debug) {
	    buginf("\nAVL: Back up to %x", element);
	}
#endif
    }

    free(element);
    return(TRUE);
}

/*
 * avl_walk
 *
 * Wrapper for avl_do_walk to prepare the arglist
 *   so that it doesn't get recursively recomputed.
 *
 */
boolean avl_walk (avl_node_type *element, avl_walker_type proc, ...)
{
    va_list argptr;
    boolean retval;

    va_start(argptr, proc);		/* Get remaining args */
    retval = avl_do_walk(element, proc, argptr);
    va_end(argptr);
    return(retval);
}


/*
 * avl_search
 *
 * Search the tree using the comparison function and return the matching
 * node.  If no matching node exists, then return NULL.  Goal is a dummy
 * node which the caller must allocate and salt with the desired search
 * value.
 */

avl_node_type *avl_search (avl_node_type *top, avl_node_type *goal,
			   avl_compare_type compare_func)
{
    while (TRUE) {

	if (!top)
	    return(NULL);
	   
	switch((*compare_func)(top, goal)) {
	case AVL_EQ:			/* Found it */
	    return(top);
	case AVL_LT:			/* top < goal */
	    top = top->right;		/* Search right */
	    break;
	case AVL_GT:			/* top > goal */
	    top = top->left;		/* Search left */
	    break;
	default:			/* This should NEVER happen */
	    return(NULL);
	}
    }
}

/*
 * avl_get_first
 *
 * Return the least node in the tree, or NULL if there is no tree.
 */

avl_node_type *avl_get_first (avl_node_type *top)
{
    if (!top)
	return(NULL);

    while (top->left)			/* Leftmost node is least */
	top = top->left;
    
    return(top);
}

/*
 * avl_get_next
 *
 * Get the least node greater than the value passed in 'element'.  Yes,
 * that's right SNMP fans, it's the all powerful GET NEXT operator!  Note
 * that 'element' can be a real node or a dummy node (i.e., not really in
 * the tree).
 *
 * Return values: NULL if there is no such element, or the next element.
 */

avl_node_type *avl_get_next (avl_node_type *top, avl_node_type *element,
			     avl_compare_type compare_func)
{
    avl_node_type *subtree;

    if (!top)
	return(NULL);

    switch((*compare_func)(top, element)) {

    case AVL_EQ:			/* Found it */
    default:				/* Return the least member of the */
	return(avl_get_first(top->right)); /* right subtree */

    case AVL_LT:			/* top < element */
	/*
	 * If top < element, then there can be nothing greater than element
	 * in the left subtree, AND top is uninteresting.
	 */
	return(avl_get_next(top->right, element, compare_func));

    case AVL_GT:			/* top > element */
	/*
	 * If top > element, then the right subtree is uninteresting.
	 * Get the next least next element from the left subtree.  If it's
	 * non-NULL, then it's less than top, so return that.  Otherwise,
	 * return top.
	 */
	subtree = avl_get_next(top->left, element, compare_func);
	return(subtree ? subtree : top);
    }
}

/* 
 * avl_balance_left
 *
 * Called when we've just deleted an element from the left subtree, and we
 * may want to balance by moving nodes to the left. 
 */
static void avl_balance_left (avl_node_type **element,
			      boolean *balancing_needed)
{

    avl_node_type *left, *right;

    switch ((*element)->balance) {
    case LEFT_HEAVY:
	(*element)->balance = BALANCED;
	break;
    case BALANCED:
	(*element)->balance = RIGHT_HEAVY;
	*balancing_needed = FALSE;
	break;
    case RIGHT_HEAVY:
	right = (*element)->right;
#ifdef BOGUS
/*
 *	This can't possibly happen.  How can the depth of the right subtree
 *	be greater than the left subtree if the right subtree is NULL?
 */
	if (!right) {
	    *balancing_needed = FALSE;
	    return;
	}
#endif

	if (right->balance >= BALANCED) { /* BALANCED or RIGHT_HEAVY */
	    /*
	     * Single rotation.
	     */
	    (*element)->right = right->left;
	    right->left = *element;
	    if (right->balance == BALANCED) {
		/* (*element)->balance = RIGHT_HEAVY; */
		right->balance = LEFT_HEAVY;
		*balancing_needed = FALSE;
	    } else {			/* right->balance == RIGHT_HEAVY */
		(*element)->balance = BALANCED;
		right->balance = BALANCED;
	    }
	    *element = right;
	} else {			/* LEFT_HEAVY */

	    /* 
	     * Double rotation.
	     */
	    left = right->left;
	    right->left = left->right;
	    left->right = right;
	    (*element)->right = left->left;
	    left->left = *element;
	    (*element)->balance =
		(left->balance == RIGHT_HEAVY) ? LEFT_HEAVY : BALANCED;
	    right->balance =
		(left->balance == LEFT_HEAVY) ? RIGHT_HEAVY : BALANCED;
	    *element = left;
	    left->balance = BALANCED;
	}
	break;
    }
}

/* 
 * avl_balance_right
 *
 * Called when we've just deleted an element from the right hand subtree,
 * and we may want to balance by moving nodes to the left. 
 */

static void avl_balance_right (avl_node_type **element,
			       boolean *balancing_needed)
{

    avl_node_type *left, *right;

    switch ((*element)->balance) {
    case RIGHT_HEAVY:
	(*element)->balance = BALANCED;
	break;
    case BALANCED:
	(*element)->balance = LEFT_HEAVY;
	*balancing_needed = FALSE;
	break;
    case LEFT_HEAVY:
	left = (*element)->left;
#ifdef BOGUS
	if (!left) {
	    *balancing_needed = FALSE;
	    return;
	}
#endif

	/*
	 * Single rotation.
	 */
	if (left->balance <= BALANCED) { /* BALANCED or LEFT_HEAVY */
	    (*element)->left = left->right;
	    left->right = *element;
	    if (left->balance == BALANCED) {
		/* (*element)->balance = LEFT_HEAVY; */
		left->balance = RIGHT_HEAVY;
		*balancing_needed = FALSE;
	    } else {			/* left->balanced == LEFT_HEAVY */
		(*element)->balance = BALANCED;
		left->balance = BALANCED;
	    }
	    *element = left;
	} else {			/* RIGHT_HEAVY */
	
	    /*
	     * Double rotation.
	     */
	    right = left->right;
	    left->right = right->left;
	    right->left = left;
	    (*element)->left = right->right;
	    right->right = *element;
	    (*element)->balance =
		(right->balance == LEFT_HEAVY) ? RIGHT_HEAVY : BALANCED;
	    left->balance =
		(right->balance == RIGHT_HEAVY) ? LEFT_HEAVY : BALANCED;
	    *element = right;
	    right->balance = BALANCED;
	}
	break;
    }
}

/*
 * avl_delete_replacement
 *
 * We've deleted one node, and it has both left and right children.  Scroll
 * down and find the maximum child less than the deleted node.  Return the
 * node that we extracted.
 */
static avl_node_type *avl_delete_replacement (avl_node_type **link,
					  boolean *balancing_needed)
{
    avl_node_type *retnode;

    if ((*link)->right) {
	retnode = avl_delete_replacement(&(*link)->right, balancing_needed);
	if (*balancing_needed)
	    avl_balance_right(link, balancing_needed);
	avl_sanity(*link);
    } else {
	retnode = *link;
	*link = (*link)->left;
	*balancing_needed = TRUE;
	avl_sanity(retnode);
    }
    return(retnode);
}

/* 
 * avl_delete
 *
 * Delete the target node from database, and rebalance tree if necessary.
 * Note that the target node may be a dummy node (i.e., not really in the
 * tree).  After extracting the (real) node from the tree, return it.
 * Return NULL if we can't find anything.
 */

avl_node_type *avl_delete (avl_node_type **top, avl_node_type *target, 
			   boolean *balancing_needed,
			   avl_compare_type compare_func) 
{
    avl_node_type *del_node;

    /*
     * Did not find node in tree.
     */
    del_node = NULL;
    if (!*top) {
	*balancing_needed = FALSE;
	return(NULL);
    }

    switch ((*compare_func)(target, *top)) {
    case AVL_LT:
	
	/*
	 * Target is less than top, traverse the left link.
	 */
	del_node = avl_delete(&(*top)->left, target, balancing_needed,
			      compare_func);
	if (*balancing_needed)
	    avl_balance_left(top, balancing_needed);
	break;

    case AVL_GT:

	/*
	 * Target is more than top, traverse the right link.
	 */
	del_node = avl_delete(&(*top)->right, target, balancing_needed,
			      compare_func);
	if (*balancing_needed)
	    avl_balance_right(top, balancing_needed);
	break;
	
    case AVL_EQ:

	/*
	 * Found entry in tree, delete it.
	 */
	del_node = *top;

	if (!del_node->right) {
	    *top = del_node->left;
	    *balancing_needed = TRUE;
	    del_node->left = NULL;
	    del_node->right = NULL;
	    del_node->balance = BALANCED;
	} else if (!del_node->left) {
	    *top = del_node->right;
	    *balancing_needed = TRUE;
	    del_node->left = NULL;
	    del_node->right = NULL;
	    del_node->balance = BALANCED;
	} else {
	    avl_node_type *new_node;

	    new_node = avl_delete_replacement(&del_node->left,
					  balancing_needed);
	    new_node->left = del_node->left;
	    new_node->right = del_node->right;
	    new_node->balance = del_node->balance;
	    *top = new_node;
	    del_node->left = NULL;
	    del_node->right = NULL;
	    del_node->balance = BALANCED;
	    if (*balancing_needed)
		avl_balance_left(top, balancing_needed);
	}
    }
    avl_sanity(del_node);
    return(del_node);
}

/*
 * avl_insert
 *
 * Given an initialized node, which is NOT in the tree, insert it into the
 * tree.  Return a pointer to the node or NULL if there's a duplicate.
 */
avl_node_type *avl_insert (avl_node_type **top,
			   avl_node_type *new,
			   boolean *balancing_needed,
			   avl_compare_type compare_func)
{
    avl_node_type *left, *right;
    avl_node_type *return_node = NULL;
    
    /* Initialize the search variables */

    if (!*top) {
	/* new element - insert it here */
	*balancing_needed = TRUE;
	*top = new;
	new->left = NULL;
	new->right = NULL;
	new->balance = BALANCED;
	return(new);
    }

    switch((*compare_func)(new, *top)) {
    case AVL_LT:

	/* 
	 * Walk left. 
	 */
	return_node = avl_insert(&(*top)->left, new, balancing_needed,
				 compare_func);
	if (!*balancing_needed)
	    break;

	switch((*top)->balance) {
	case RIGHT_HEAVY: 

	    /*
	     * Node was right heavy, now is balanced.
	     */
	    (*top)->balance = BALANCED;
	    *balancing_needed = FALSE;
	    break;

	case BALANCED:

	    /*
	     * Node was balanced, now is left heavy.
	     */
	    (*top)->balance = LEFT_HEAVY;
	    break;

	case LEFT_HEAVY:

	    /*
	     * Node was left heavy, need to rebalance.
	     */
	    left = (*top)->left;
	    
	    /*
	     * Single LL rotation.
	     */
	    if (left->balance == LEFT_HEAVY) {
		(*top)->left = left->right;
		left->right = *top;
		(*top)->balance = BALANCED;
		*top = left;
	    } else {

		/*
		 * Double LR rotation
		 */
		right = left->right;
		left->right = right->left;
		right->left = left;
		(*top)->left = right->right;
		right->right = *top;
		(*top)->balance =
		    (right->balance == LEFT_HEAVY) ? RIGHT_HEAVY : BALANCED;
		left->balance =
		    (right->balance == RIGHT_HEAVY) ? LEFT_HEAVY : BALANCED;
		*top = right;
	    }
	    (*top)->balance = BALANCED;
	    *balancing_needed = FALSE;
	    break;
	}
	break;

    case AVL_GT:

	/*
	 * Walk right.
	 */
	return_node = avl_insert(&(*top)->right, new, balancing_needed,
				 compare_func);

	if (!*balancing_needed) 
	    break;

	switch ((*top)->balance) {
	case LEFT_HEAVY:

	    /*
	     * Node was left heavy, is now balanced.
	     */
	    (*top)->balance = BALANCED;
	    *balancing_needed = FALSE;
	    break;

	case BALANCED:
	    
	    /*
	     * Node was balanced, is now right heavy.
	     */
	    (*top)->balance = RIGHT_HEAVY;
	    break;

	case RIGHT_HEAVY:

	    /*
	     * Node was right heavy, now needs rebalancing.
	     */
	    right = (*top)->right;

	    /*
	     * Single RR rotation.
	     */
	    if (right->balance == RIGHT_HEAVY) {
		(*top)->right = right->left;
		right->left = *top;
		(*top)->balance = BALANCED;
		*top = right;
	    } else {

		/*
		 * Double RL rotation.
		 */
		left = right->left;
		right->left = left->right;
		left->right = right;
		(*top)->right = left->left;
		left->left = *top;
		(*top)->balance =
		    (left->balance == RIGHT_HEAVY) ? LEFT_HEAVY : BALANCED;
		right->balance =
		    (left->balance == LEFT_HEAVY) ? RIGHT_HEAVY : BALANCED;
		*top = left;
	    }
	    (*top)->balance = BALANCED;
	    *balancing_needed = FALSE;
	    break;
	}
	break;

    case AVL_EQ:
	
	/* 
	 * Found existing entry.
	 */
	return_node = NULL;
	*balancing_needed = FALSE;
	break;
    }
    avl_sanity(*top);
    return(return_node);
}

#if 0
/*
 * avl_balance_string
 *
 * Return a balance string representing the balance state.
 */

char *avl_balance_string (balance_type bal)
{
    switch(bal) {
    case LEFT_HEAVY:
	return("Left heavy");
    case BALANCED:
	return("Balanced");
    case RIGHT_HEAVY:
	return("Right heavy");
    default:
	return("Confused");
    }
}

/*
 * show_avl_tree
 *
 * Display an AVL tree via printfs.
 * XXX Not memlock protected yet, tho it needs it.
 */

void show_avl_tree (avl_node_type *top, avl_compare_type compare_func)
{
    avl_node_type *curr;

    if (!top)
	return;
    printf("\nNode     Left     Right    Balance");
    curr = avl_get_first(top);
    for ( ; curr; curr = avl_get_next(top, curr, compare_func)) {
	printf("\n%8x %8x %8x %s", curr, curr->left, curr->right,
	       avl_balance_string(curr->balance));
    }
}
#endif
