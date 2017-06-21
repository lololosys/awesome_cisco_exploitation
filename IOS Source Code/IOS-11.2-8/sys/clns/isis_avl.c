/* $Id: isis_avl.c,v 3.3.52.1 1996/08/04 23:35:08 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/clns/isis_avl.c,v $
 *------------------------------------------------------------------
 * isis_avl.c -- AVL tree functions for the IS-IS link state database.
 *
 * October 1991, Dino Farinacci
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: isis_avl.c,v $
 * Revision 3.3.52.1  1996/08/04  23:35:08  dkatz
 * CSCdi63547:  ISIS database search is expensive
 * Branch: California_branch
 * Rewrite the dreaded isis_recursive_walk so that it no longer uses
 * recursion.  Fix "show isis tree" so that it works properly.  Clean up
 * references to isis_recursive_walk to make type coercion unnecessary.
 *
 * Revision 3.3  1995/12/01  02:48:25  dkatz
 * CSCdi39906:  CPUHOG Process = IS-IS Update
 *
 * Revision 3.2  1995/11/17  08:53:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:09:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/23  21:37:36  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Calculate routes from only changed LSPs if the inter-router topology
 * stays constant.  Don't gratuitously install IP routes that will
 * be immediately replaced, as this causes large-scale cache invalidation.
 * Eliminate multiple IP forwarding table traversals.
 *
 * Revision 2.2  1995/06/21  08:53:46  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.1  1995/06/07  20:18:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "config.h"
#include "logger.h"
#include "../os/free.h"
#include "../clns/clns.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../clns/clns_externs.h"
#include "../clns/isis_externs.h"
#include "../clns/isis_inlines.h"

/*
 * isis_recursive_walk
 *
 * This routine will walk the avl tree, calling the specified procedure
 * at each node.
 *    
 * If the procedure returns false - the walk will stop.
 *
 * We lock the elements of the tree as we descend to avoid crashing
 * in "show" routines due to suspending.
 *
 * The walk type indicates what we want to look for:
 *
 *   ALL--Walk the entire LSP database
 *
 *   MATCH_SYSTEM--Walk only the LSPs that match the system ID, starting
 *      with the fragment number specified in the start_id
 *
 *   MATCHED_SYSTEM--an internal value for continuing MATCH_SYSTEM
 *
 *   FIND_STARTID--Walk all LSPs starting with the specified start_id
 */

#define AVL_MAX_RECURSION 16		/* Works with 65,536 nodes! */
typedef struct recurse_context_ {
    isis_tree_type *element;		/* Element at this level */
    boolean did_right;			/* TRUE if we've done rightt child */
} recurse_context;

boolean isis_recursive_walk (isis_lspid *start_id, isis_walk_type walk_type,
			     isis_tree_type *element, isis_walker_type proc,
			     void *parm1, void *parm2, void *parm3,
			     void *parm4)
{
    boolean result;
    isis_lspdbtype *lsp;
    boolean process_this_lsp;
    recurse_context recurse_stack[AVL_MAX_RECURSION];
    recurse_context *context;
    ulong recurse_level;
    boolean did_left, did_right;
    boolean show_tree;

    recurse_level = 0;
    context = &recurse_stack[0];
    show_tree = (walk_type == ISIS_WALK_SHOW_ALL);

    result = TRUE;
    did_left = FALSE;
    if (!element)
	return(result);
    inline_mem_lock(element);

    /* Top of the loop.  Process the current element. */

    while (result) {
	if (show_tree) {
	    printf("\nCurrent node = %s, depth = %d, bal = %d", 
		   lspid_string(&element->lspid), recurse_level,
		   element->balance);
	}
	
	/* If there is a left child, recurse for it. */

	if (element->llink && !did_left) {
	    if (recurse_level >= AVL_MAX_RECURSION)
		goto get_out;		/* Whatta mess */

	    if (show_tree)
		printf("\n  Go down left");
	    
	    context->element = element;
	    context->did_right = FALSE;
	    recurse_level++;
	    context++;
	    element = element->llink;
	    inline_mem_lock(element);
	    did_left = FALSE;		/* We haven't done the child's left */
	    continue;			/* Go recurse. */
	}
	
	process_this_lsp = FALSE;
	switch (walk_type) {
	    
	    /*
	     * Match system.  Process if we're past the start ID but still in
	     * the matching system ID.
	     */
	  case ISIS_WALK_MATCH_SYSTEM:
	    if (isis_match_lspid(&element->lspid, start_id) >= 0) {
		walk_type = ISIS_WALK_MATCHED_SYSTEM;
	    } else {
		break;			/* Continue but don't process. */
	    }
	    
	    /* FALL THROUGH! */
	    
	    /*
	     * Already matched system.  Bail if system ID doesn't match.
	     * Otherwise, accept the LSP.
	     */
	  case ISIS_WALK_MATCHED_SYSTEM:
	    if (!systemid_match(start_id->system_id,
				element->lspid.system_id)) {
		result = FALSE;
		goto get_out;
	    }
	    process_this_lsp = TRUE;
	    break;
	    
	    /* Find start ID.  Process if we're past the start ID. */
	    
	  case ISIS_WALK_FIND_STARTID:
	    if (isis_match_lspid(&element->lspid, start_id) >= 0) {
		process_this_lsp = TRUE;
		walk_type = ISIS_WALK_ALL;	/* Do all LSPs from here on. */
	    }
	    break;

	    /* A walk all with debugging. */

	  case ISIS_WALK_SHOW_ALL:
	    process_this_lsp = TRUE;
	    break;
	    
	    
	    /* Otherwise, just do it. */
	    
	  case ISIS_WALK_ALL:
	    process_this_lsp = TRUE;
	    break;
	}
	
	if (process_this_lsp) {
	    lsp = element->lsp;
	    if (lsp)
		inline_mem_lock(lsp);
	    result = (*proc) (element, parm1, parm2, parm3, parm4);
	    if (lsp)
		inline_mem_unlock_or_free(lsp);
	    if (!result)
		goto get_out;
	    if (lsp != element->lsp) {	/* Bail if LSP changed beneath us */
		result = FALSE;
		goto get_out;
	    }
	}
	
	if (element->rlink) {
	    if (recurse_level >= AVL_MAX_RECURSION)
		goto get_out;		/* Whatta mess */

	    if (show_tree)
		printf("\n  Go down right");

	    context->element = element;
	    context->did_right = TRUE;
	    recurse_level++;
	    context++;
	    element = element->rlink;
	    inline_mem_lock(element);
	    did_left = FALSE;		/* We haven't done the child's left */
	    continue;			/* Go recurse. */
	}
	did_right = TRUE;

	/* OK, time to come up a level. */

	while (did_right) {
	    inline_mem_unlock_or_free(element);
	    element = NULL;		/* Don't free it twice! */
	    if (recurse_level == 0)	/* All done! */
		goto get_out;
	    
	    recurse_level--;
	    context--;
	    did_left = TRUE;		/* We've always done left by now. */
	    did_right = context->did_right;
	    element = context->element;
	    if (show_tree) {
		printf("\n  Back up to %s", lspid_string(&element->lspid));
	    }
	}
    }

    /* Unwind the stack and unlock the elements if we're bailing out. */

  get_out:
    if (element) {
	inline_mem_unlock_or_free(element);
    }
    while (recurse_level) {
	recurse_level--;
	context--;
	inline_mem_unlock_or_free(context->element);
    }
    return(result);
}


/*
 * isis_rw_next_avl
 *
 * Routine for the bottom of the tree walker for isis_next_avl.
 */
static boolean isis_rw_next_avl(isis_tree_type *element, void *key_parm,
				void *result_parm, void *dummy1, void *dummy2)
{
    isis_lspid *key;
    isis_tree_type **next_element;

    key = key_parm;
    next_element = result_parm;

    /*
     * System ID matches.  Do a simple comparison of the fragment
     * number.  If greater than our key, we're there.
     */
    if (element->lspid.lsp_number > key->lsp_number) {
	*next_element = element;
	return(FALSE);		/* Terminate the search */
    }
    return(TRUE);
}


/*
 * isis_next_avl
 *
 * Get the next LSP number for the system specified by key. Example,
 * if key 0000.0c00.ffff.00-01 is the LSPID, return 0000.0c00.ffff.00-02 
 * if it exists.
 *
 * We leverage isis_recursive_walk to find the right system ID, so
 * isis_rw_next_avl need only look at the fragment number.
 *
 * Updates next_element to point at the new element, if there.
 */
void isis_next_avl (isis_lspid *key, isis_tree_type *element,
		    isis_tree_type **next_element)
{
    *next_element = NULL;
    isis_recursive_walk(key, ISIS_WALK_MATCH_SYSTEM, element, isis_rw_next_avl,
			key, next_element, NULL, NULL);
}


/* 
 * isis_match_lspid
 *
 * Returns -1 if lsp1 < lsp2, 1 if lsp1 > lsp2, 0 if equal.
 */
int isis_match_lspid (isis_lspid *lspid1, isis_lspid *lspid2)
{
    int i;
    uchar *lsp1, *lsp2;

    lsp1 = (uchar *) lspid1;
    lsp2 = (uchar *) lspid2;

    i = ISIS_LSPID_LENGTH;
    while (i--) {
	if (*lsp1 < *lsp2)
	    return (-1);
	if (*(lsp1++) > *(lsp2++))
	    return (1);
    }
    return(0);
}
/*
 * isis_free_avl_lsp
 *
 * Free an LSP from the clutches of the AVL tree.  Clean up other
 * gunk in its wake first, though.
 *
 */
static void isis_free_avl_lsp (clns_pdbtype *pdb, isis_lspdbtype *lsp)
{
    isis_lspix_type *lspix_entry;

    if (lsp->lsp_index) {
	lspix_entry = isis_lsp_index_to_lspix_entry(pdb, lsp->lsp_index);
	lspix_entry->lsp = NULL;
	lsp->lsp_index = 0;
    }
    isis_free_lsp(pdb, lsp);
}

/*
 * isis_avl_insert
 *
 *    This is the default avl insertion routine.
 *    It will return the address of the new avl element (*element)
 *    and the balance condition after the insertion (*h)
 *
 *    The LSP index table entry is also updated.
 *
 *    We are supposed to dispose properly of the LSP, so free it if
 *    we run out of memory.
 */
void isis_avl_insert (clns_pdbtype *pdb, int *h, isis_lspid *key,
		      isis_lspdbtype *lsp, isis_tree_type **element)
{
    isis_lspix_type *lspix_entry;

    if (!*element) {
	*element = malloc_named(sizeof(isis_tree_type), "ISIS AVL");
	if (!*element) {
	    isis_free_avl_lsp(pdb, lsp);
	    return;
	}

	(*element)->llink = NULL;
	(*element)->rlink = NULL;

	*h = TRUE;
	(*element)->balance = 0;
	lspid_copy(key, &(*element)->lspid);
	(*element)->lsp = lsp;
    } else {

	/* There already was an element - just replace the data pointer. */

	isis_free_avl_lsp(pdb, (*element)->lsp);
	(*element)->lsp = lsp;
    }
    lspix_entry = isis_lsp_index_to_lspix_entry(pdb, lsp->lsp_index);
    lspix_entry->lsp = lsp;
    lspix_entry->level = lsp->level;
    lspix_entry->lspix_in_use = TRUE;
}

/* 
 * isis_avl_bal_left
 *
 */
static void isis_avl_bal_left (isis_tree_type **element, int *h)
{

    isis_tree_type *left, *right;

    switch ((*element)->balance) {
    case -1:
	(*element)->balance = 0;
	break;
    case 0:
	(*element)->balance = 1;
	*h = FALSE;
	break;
    case 1:
	right = (*element)->rlink;
	if (!right) {
	    *h = FALSE;
	    return;
	}

	/*
	 * Single rotation.
	 */
	if (right->balance >= 0) {
	    (*element)->rlink = right->llink;
	    right->llink = *element;
	    if (!right->balance) {
		(*element)->balance = 1;
		right->balance = -1;
		*h = FALSE;
	    } else {
		(*element)->balance = 0;
		right->balance = 0;
	    }
	    *element = right;
	} else {

	    /* 
	     * Double rotation.
	     */
	    left = right->llink;
	    right->llink = left->rlink;
	    left->rlink = right;
	    (*element)->rlink = left->llink;
	    left->llink = *element;
	    (*element)->balance = (left->balance == 1) ? -1 : 0;
	    right->balance = (left->balance == -1) ? 1 : 0;
	    *element = left;
	    left->balance = 0;
	}
	break;
    }
}

/* 
 * isis_avl_bal_right
 *
 */
static void isis_avl_bal_right (isis_tree_type **element, int *h)
{

    isis_tree_type *left, *right;

    switch ((*element)->balance) {
    case 1:
	(*element)->balance = 0;
	break;
    case 0:
	(*element)->balance = -1;
	*h = FALSE;
	break;
    case -1:
	left = (*element)->llink;
	if (!left) {
	    *h = FALSE;
	    return;
	}

	/*
	 * Single rotation.
	 */
	if (left->balance <= 0) {
	    (*element)->llink = left->rlink;
	    left->rlink = *element;
	    if (!left->balance) {
		(*element)->balance = -1;
		left->balance = 1;
		*h = FALSE;
	    } else {
		(*element)->balance = 0;
		left->balance = 0;
	    }
	    *element = left;
	} else {
	
	    /*
	     * Double rotation.
	     */
	    right = left->rlink;
	    left->rlink = right->llink;
	    right->llink = left;
	    (*element)->llink = right->rlink;
	    right->rlink = *element;
	    (*element)->balance = (right->balance == -1) ? 1 : 0;
	    left->balance = (right->balance == 1) ? -1 : 0;
	    *element = right;
	    right->balance = 0;
	}
	break;
    }
}

/*
 * isis_avl_delete
 *
 */
static void isis_avl_delete (isis_tree_type **link, 
			     isis_tree_type **del_node, int *h)
{

    if ((*link)->rlink) {
	isis_avl_delete(&(*link)->rlink, del_node, h);
	if (*h) isis_avl_bal_right(link, h);
    } else {
	(*del_node)->lsp = (*link)->lsp;
	lspid_copy(&(*link)->lspid, &(*del_node)->lspid);
	*del_node = *link;
	*link = (*link)->llink;
	*h = TRUE;
    }
}

/*
 * isis_delink_one_lsp
 *
 * Free an LSP given an AVL entry, and clear the pointer to it.  We
 * need to be careful about NULLing out pointers, since there may
 * be memlocks and shows going on...
 */
void isis_delink_one_lsp (clns_pdbtype *pdb, isis_tree_type *element)
{
    if (element->lsp) {
	isis_free_avl_lsp(pdb, element->lsp);
	element->lsp = NULL;
    }
}

/*
 * isis_free_avl_element
 *
 * Frees an AVL element, NULLing pointers as appropriate.
 */
void isis_free_avl_element (isis_tree_type *element)
{
    if (element) {
	element->llink = NULL;
	element->rlink = NULL;
	free(element);
    }
}

/* 
 * isis_lsp_avl_delete
 *
 * Delete LSP from database, and rebalance tree if necessary.
 *
 */
void isis_lsp_avl_delete (clns_pdbtype *pdb, isis_lspid *key,
			  isis_tree_type **element, int *h)
{

    isis_tree_type *del_node;
    int            match;

    /*
     * Did not find LSP in tree.
     */
    if (!*element) {
	*h = FALSE;
	return;
    }

    match = isis_match_lspid(key, &(*element)->lspid);

    switch (match) {
    case -1:
	
	/*
	 * key is less than node in tree, traverse left link.
	 */
	isis_lsp_avl_delete(pdb, key, &(*element)->llink, h);
	if (*h) isis_avl_bal_left (element, h);
	break;

    case 1:

	/*
	 * key is greater than node in tree, traverse right link.
	 */
	isis_lsp_avl_delete(pdb, key, &(*element)->rlink, h);
	if (*h) isis_avl_bal_right(element, h);
	break;
	
    case 0:

	/*
	 * Found entry in tree, delete it.  Free index table entry.
	 */
	del_node = *element;
	isis_delink_one_lsp(pdb, del_node);

	if (!del_node->rlink) {
	    *element = del_node->llink;
	    *h = TRUE;
	} else if (!del_node->llink) {
	    *element = del_node->rlink;
	    *h = TRUE;
	} else {
	    isis_avl_delete(&del_node->llink, &del_node, h);
	    if (*h) isis_avl_bal_left (element, h);
	}
	isis_free_avl_element(del_node);
    }
}

/*
 *   isis_get_avl_element
 *
 * Search the LSP_tree for key 'k'
 * Note that this returns the tree element. An encapsulating routine
 * will need to return the lsp_pdu.
 *
 * If this routine finds a match, it will return:
 *      element - pointer to the element
 *
 * If it doesn't find a match, it will call the insert_proc (if defined)
 * It will return:
 *      element - pointer to new element
 */
isis_tree_type *isis_get_avl_element (isis_lspid *k, isis_tree_type **element,
				      int *h, avl_proc_type insert_proc,
				      isis_lspdbtype *data,
				      idbtype *insert_param1,
				      int insert_param2,
				      clns_pdbtype *insert_param3,
				      isis_TLV_parse *insert_param4)
{

    isis_tree_type *n1, *n2;
    isis_tree_type *return_element = NULL;
    int            match;
    
    /* Initialize the search variables */

    if (!*element) {
	/* new element - insert it here */
	*h = FALSE;
	if (insert_proc) {
	    insert_proc(h, k, data, element, insert_param1, insert_param2, 
			insert_param3, insert_param4);
	}
	return(NULL);
    }

    match = isis_match_lspid(k, &(*element)->lspid);

    switch (match) {
    case -1:

	/* 
	 * Walk left. 
	 */
	return_element = isis_get_avl_element(k, &(*element)->llink, h, 
	      insert_proc, data, insert_param1, insert_param2, 
	      insert_param3, insert_param4);
	if (!*h) break;

	switch((*element)->balance) {
	case 1: 

	    /*
	     * Node was right heavy, now is balanced.
	     */
	    (*element)->balance = 0;
	    *h = FALSE;
	    break;

	case 0:

	    /*
	     * Node was balanced, now is left heavy.
	     */
	    (*element)->balance = -1;
	    break;

	case -1:

	    /*
	     * Node was left heavy, need to rebalance.
	     */
	    n1 = (*element)->llink;
	    
	    /*
	     * Single LL rotation.
	     */
	    if (n1->balance == -1){
		(*element)->llink = n1->rlink;
		n1->rlink = *element;
		(*element)->balance = 0;
		*element = n1;
	    } else {

		/*
		 * Double LR rotation
		 */
		n2 = n1->rlink;
		n1->rlink = n2->llink;
		n2->llink = n1;
		(*element)->llink = n2->rlink;
		n2->rlink = *element;
		(*element)->balance = (n2->balance == -1) ? 1 : 0;
		n1->balance = (n2->balance == 1) ? -1 : 0;
		*element = n2;
	    }
	    (*element)->balance = 0;
	    *h = FALSE;
	    break;
	}
	break;

    case 1:

	/*
	 * Walk right.
	 */
	return_element = isis_get_avl_element(k, &(*element)->rlink, h, 
	      insert_proc, data, insert_param1, insert_param2, 
	      insert_param3, insert_param4);
	if (!*h) break;

	switch ((*element)->balance) {
	case -1:

	    /*
	     * Node was left heavy, is now balanced.
	     */
	    (*element)->balance = 0;
	    *h = FALSE;
	    break;

	case 0:
	    
	    /*
	     * Node was balanced, is now right heavy.
	     */
	    (*element)->balance = 1;
	    break;

	case 1:

	    /*
	     * Node was right heavy, now needs rebalancing.
	     */
	    n1 = (*element)->rlink;

	    /*
	     * Single RR rotation.
	     */
	    if (n1->balance == 1) {
		(*element)->rlink = n1->llink;
		n1->llink = *element;
		(*element)->balance = 0;
		*element = n1;
	    } else {

		/*
		 * Double RL rotation.
		 */
		n2 = n1->llink;
		n1->llink = n2->rlink;
		n2->rlink = n1;
		(*element)->rlink = n2->llink;
		n2->llink = *element;
		(*element)->balance = (n2->balance == 1) ? -1 : 0;
		n1->balance = (n2->balance == -1) ? 1 : 0;
		*element = n2;
	    }
	    (*element)->balance = 0;
	    *h = FALSE;
	    break;
	}
	break;

    case 0:
	
	/* 
	 * Found existing entry.
	 */
	return_element = *element;
	*h = FALSE;
	if (insert_proc) {
	    insert_proc(h, k, data, element, insert_param1, insert_param2,
		insert_param3, insert_param4);
	}
	break;
    }

    return(return_element);
}
