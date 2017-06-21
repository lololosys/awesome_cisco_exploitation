/* $Id: filter.c,v 3.3.12.2 1996/06/28 23:24:42 hampton Exp $
 * $Source: /release/112/cvs/Xsys/os/filter.c,v $
 *------------------------------------------------------------------
 * filter.c--Support for CLNS address filters
 *
 * July 1992, Dave Katz
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: filter.c,v $
 * Revision 3.3.12.2  1996/06/28  23:24:42  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.3.12.1  1996/03/18  21:29:33  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/07  10:18:18  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.24.1  1996/02/20  16:39:02  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/13  20:49:39  hampton
 * Clean up includes of gdb.h and references to breakpoint.  [CSCdi48844]
 *
 * Revision 3.2  1995/11/17  18:47:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/19  06:48:43  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:54:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <ciscolib.h>
#include "packet.h"
#include "filter.h"
#include "config.h"
#include "filter_externs.h"

/*
 * Variables for recursive walks without true recursion
 *
 * The following variables hold the necessary state for doing recursive
 * walks of a filter tree without having to do true recursion (and risk
 * stack overflow).
 */

#define MAX_RECURSION 10 /* Don't go more than 10 levels deep */
static int rec_level;		/* Current recursion level */
static boolean rec_didleft[MAX_RECURSION]; /* TRUE if we already did the 
					      left child */
static filter_expr *rec_node[MAX_RECURSION]; /* Node stack */
static boolean rec_value[MAX_RECURSION]; /* Value stack */


/*
 * Pointer to the filter object array, and its size.  Remember, entry 0 is not
 * used (so we can assume that index 0 means "nada").  Waste a few bytes.
 */
filter_object *filter_objects = NULL;
int filter_objects_count = 0;
#define FILTER_CREATION_INCREMENT 10 /* Add objects in chunks of this size */

/*
 * Return the address of the indexed filter object, or NULL if the index
 * is invalid.
 *
 * *** Note--filter object pointers are not valid across calls to ***
 * *** make_filter_object() !!                                    ***
 */

filter_object *filter_obj_ptr (ulong index)
{
    if (!index || index >= filter_objects_count)
	return(NULL);

    return(filter_objects + index);
}


/*
 * Find a filter object given the object name.  Returns the object index,
 * or 0 if it isn't there.
 */

ulong get_filter_index (char *name)
{
    ulong index;
    filter_object *obj;

    /* Walk the list looking for the desired entry. */

    FOR_ALL_FILTERS(index, obj) {
	if (!strcasecmp(name, obj->name))
	    return(index);
    }

    /* Not here. */

    return(0);
}

/*
 * Create a filter object of the given name.  Assumes that there
 * is no existing object with the same name!
 *
 * Returns the index assigned, or 0 if we ran out of memory.
 *
 * This routine will fill in the "name" field;  it is up to the caller
 * to fill in the rest of the fields.
 *
 * *** Note--this routine invalidates all existing filter object pointers! ***
 */

static ulong make_filter_object (char *name)
{
    ulong index;
    int count;
    filter_object *new_filter_array;
    filter_object *ptr;
    char *namestr;

    /*
     * Walk the filter array.  We'll fall out of the loop if there are no
     * free entries or if there's no array at all.
     */
    FOR_ALL_FILTERS(index, ptr) {
	if (ptr->name == NULL) {
	    break;
	}
    }

    if (index >= filter_objects_count) {

	/* Didn't find a free entry.  Create a new list with more entries. */

	new_filter_array = (filter_object *) 
	    malloc((filter_objects_count + FILTER_CREATION_INCREMENT) * 
		   sizeof(filter_object));
	if (new_filter_array == NULL)
	    return(0);
	
	/* Copy over the old information. */
	
	if (filter_objects_count) {
	    bcopy((uchar *) filter_objects, (uchar *) new_filter_array,
		  filter_objects_count * sizeof(filter_object));
	    free(filter_objects);
	}
	filter_objects = new_filter_array;
	
	/* Initialize the new entries. */
	
	ptr = filter_objects + filter_objects_count;
	for (count = 0; count < FILTER_CREATION_INCREMENT; count++, ptr++) {
	    ptr->name = NULL;
	    ptr->protocol = FILTER_NO_PROTO;
	    ptr->filptr.generic = NULL;
	    ptr->refcount = 0;
	    ptr->type = FILTER_UNDEF;
	}

	/* Update the count. */
	
	filter_objects_count += FILTER_CREATION_INCREMENT;

	/* Adjust to skip the zero index. */

	if (index == 0)
	    index = 1;

	ptr = filter_obj_ptr(index);
    }

    /* Now update the entry.  "index" and "ptr" should be valid. */

    ptr->filptr.generic = NULL;
    ptr->refcount = 0;
    ptr->type = FILTER_UNDEF;
    namestr = strdup(name);
    if (namestr == NULL)
	return(0);		/* Out of memory!  */
    ptr->name = namestr;	/* It's all set now. */

    return(index);
}


/*
 * Create a filter object with the given name if it does not already
 * exist.
 *
 * It is up to the caller to deal with the situation where the named
 * filter object already exists.
 *
 * Returns the index of the object, or 0 if there is no memory.
 *
 * *** Note--this routine invalidates all existing filter object pointers! ***
 */
ulong create_filter_object (char *name)
{
    ulong index;
    index = get_filter_index(name);
    if (index == 0) {
	index = make_filter_object(name);
    }
    return(index);
}

/*
 * Free a filter object if its reference count has gone to zero.
 * We assume that the caller has already freed the object associated
 * with this name.
 */

static void delete_filter_object (ulong index)
{
    filter_object *fltrptr = filter_obj_ptr(index);

    if (!fltrptr)
	return;

    /* If there are references to the name object, don't delete it. */
    
    if (fltrptr->refcount)
	return;

    /* If the object is unused, bail out. */

    if (!fltrptr->name)
	return;

    free(fltrptr->name);
    fltrptr->name = NULL;
    fltrptr->type = FILTER_UNDEF;
}


/*
 * Clean up the filter object table by deleting all unreferenced
 * placeholder objects.
 */

void clean_up_filter_objects (void)
{
    ulong i;
    filter_object *obj;

    FOR_ALL_FILTERS(i, obj) {
	if (!obj->name)
	    continue;
	if (obj->filptr.generic)
	    continue;
	delete_filter_object(i);
    }
}


/*
 * Perform a recursive walk on a filter expression tree.  The tree is
 * walked, calling "leafrtn" for each leaf of the tree (filter_set).
 * This routine is called to actually run a filter.
 *
 * The routine should be declared as follows:
 *  boolean leaf_routine(void *leaf, enum filter_type,
 *                       void *parm1, void *parm2);
 *
 * The return value *must* be 1 or 0, or things won't work.
 *
 * We return the boolean result of the tree walk.
 *
 * We do not walk unnecessary right children.
 *
 * Note that if we recurse too deeply, we just don't bother going deeper.
 * The routines that create the tree must ensure that the tree isn't too
 * deep.
 *
 * Note also that we overload the global rec_level with our own register
 * variable for speed purposes.  The leaf routine thus does not know the
 * recursion depth.
 *
 * If there are null pointers in the tree (subtrees have been deleted),
 * they are given a value of TRUE.
 */

boolean filter_recursive_walk_execute (
filter_expr *root,
boolean (*leafrtn)(generic_leaf *, filter_type, void *, void *),
void *parm1,
void *parm2)
{
    boolean leftresult, rightresult, didleft, didright;
    filter_expr *cur;
    int rec_level;

    /* Initialize. */

    rec_level = 0;
    cur = root;
    leftresult = rightresult = TRUE;
    didleft = didright = FALSE;

    /* Go to it. */

    for (;;) {

	/* Process the left side if we haven't already done it. */

	if (!didleft) {

	    /* If the left child is an expression, recurse. */
	    
	    if (cur->lefttype == FILTER_TERM_EXPR) {

		if (rec_level >= MAX_RECURSION)
		    break;
		if (cur->left.expr) {
		    rec_node[rec_level] = cur;
		    rec_didleft[rec_level] = FALSE;
		    cur = cur->left.expr;
		    rec_level++;
		    continue;
		} else {
		    leftresult = TRUE;
		}

	    } else {
		
		/* The left child is a leaf.  Process it. */
		
		if (cur->left.generic) {
		    leftresult = (*leafrtn)(cur->left.generic, cur->lefttype, 
					    parm1, parm2);
		} else {
		    leftresult = TRUE;
		}
	    }
	}

	/* Process the right side if we haven't already done it. */

	if (!didright) {

	    /* If the opcode is dyadic, process the right child. */
	    
	    if (cur->right.expr) {

		/* Do the right side only if we have to. */

		if ((!(cur->operator == FILTER_OP_AND && !leftresult) && 
		     !(cur->operator == FILTER_OP_OR && leftresult))) {

		    /* If the right child is an expression, recurse. */
		    
		    if (cur->righttype == FILTER_TERM_EXPR) {
			
			if (rec_level >= MAX_RECURSION)
			    break;
			rec_node[rec_level] = cur;
			rec_didleft[rec_level] = TRUE;
			rec_value[rec_level] = leftresult;
			cur = cur->right.expr;
			didleft = didright = FALSE;
			rec_level++;
			continue;
			
		    } else {
			
			/* The right child is a leaf.  Process it. */
			
			if (cur->right.generic) {
			    rightresult = (*leafrtn)(cur->right.generic, 
						     cur->righttype, 
						     parm1, parm2);
			} else {
			    rightresult = TRUE;
			}
		    }
		}
	    }
	}

	/* Perform the operation. */

	switch (cur->operator) {

	case FILTER_OP_AND:
	    leftresult &= rightresult;
	    break;
	    
	case FILTER_OP_OR:
	    leftresult |= rightresult;
	    break;

	case FILTER_OP_NOT:
	    leftresult = (~leftresult) & 1;
	    break;

	case FILTER_OP_XOR:
	    leftresult ^= rightresult;
	    break;

	case FILTER_OP_NONE:
	    break;
	}
	
	/* Now pop the stack. */
	
	if (rec_level == 0)	/* All done? */
	    break;
	
	rec_level--;
	didright = rec_didleft[rec_level];
	cur = rec_node[rec_level];
	if (didright) {
	    rightresult = leftresult;
	    leftresult = rec_value[rec_level];
	}
	didleft = TRUE;
    }
    return(leftresult);
}


/*
 * Perform a recursive walk on a filter expression tree.  The tree is
 * walked, calling "startrtn" when entering a node, "middlertn" after
 * processing the left child and before processing the right child,
 * "endrtn" after processing the right child, and "leafrtn" for each
 * leaf of the tree (filter_set).  This is intended for use with
 * auxiliary routines, and is not intended to be real fast.
 *
 * The routines should be declared as follows:
 *  boolean leafrtn(void *, filter_type);
 *  boolean startrtn(filter_expr *);
 *  boolean middlertn(filter_expr *);
 *  boolean endrtn(filter_expr *);
 *
 * The return value of each routine should be TRUE to continue recursion,
 * or FALSE to stop recursion.
 *
 * Any routine pointer may be NULL.
 *
 * We return TRUE if the recursion depth is too great, or FALSE otherwise.
 */

static boolean recursive_walk_aux (
filter_expr *root,
boolean (*leafrtn)(void *, filter_type),
boolean (*startrtn)(filter_expr *),
boolean (*middlertn)(filter_expr *),
boolean (*endrtn)(filter_expr *))
{
    boolean didleft, didright;
    filter_expr *cur;
    boolean toodeep;
    boolean keepgoing;

    /* Initialize. */

    rec_level = 0;
    cur = root;
    didleft = didright = toodeep = FALSE;
    keepgoing = TRUE;
 
    /* Go to it. */

    for (;;) {

	/* Process the left side if we haven't already done it. */

	if (!didleft) {

	    /* Entering the node.  Call the start routine. */

	    if (startrtn) {
		keepgoing = (*startrtn)(cur);
		if (!keepgoing) goto popstack;
	    }

	    /* If the left child is an expression, recurse. */
	    
	    if (cur->lefttype == FILTER_TERM_EXPR) {

		if (rec_level >= MAX_RECURSION) {
		    toodeep = TRUE;
		    break;
		}

		/* Skip the recursion if the expression is gone. */

		if (cur->left.expr) {
		    rec_node[rec_level] = cur;
		    rec_didleft[rec_level] = FALSE;
		    cur = cur->left.expr;
		    rec_level++;
		    continue;
		}

	    } else {
		
		/* The left child is a leaf.  Process it. */
		
		if (leafrtn && cur->left.generic) {
		    keepgoing = (*leafrtn)(cur->left.generic, cur->lefttype);
		    if (!keepgoing) goto popstack;
		}
	    }
	}

	/* Process the right side if we haven't already done it. */

	if (!didright) {

	    /* If the opcode is dyadic, process the right child. */
	    
	    if (cur->rightindex) {

		/* Middle of the node.  Call the middle routine. */

		if (middlertn) {
		    keepgoing = (*middlertn)(cur);
		    if (!keepgoing) goto popstack;
		}

		/* If the right child is an expression, recurse. */
		
		if (cur->righttype == FILTER_TERM_EXPR) {
		    
		    if (rec_level >= MAX_RECURSION) {
			toodeep = TRUE;
			break;
		    }

		    /* Skip the recursion if the expression is gone. */

		    if (cur->right.expr) {
			rec_node[rec_level] = cur;
			rec_didleft[rec_level] = TRUE;
			cur = cur->right.expr;
			didleft = didright = FALSE;
			rec_level++;
			continue;
		    }
		    
		} else {
		    
		    /* The right child is a leaf.  Process it. */
		    
		    if (leafrtn && cur->right.generic) {
			keepgoing = (*leafrtn)(cur->right.generic, 
					       cur->righttype);
			if (!keepgoing) goto popstack;
		    }
		}
	    }
	}

	/* Perform the end routine. */
	
	if (endrtn) {
	    keepgoing = (*endrtn)(cur);
	    if (!keepgoing) goto popstack;
	}
	
	/* Now pop the stack. */
	
    popstack:
	if (rec_level == 0)	/* All done? */
	    break;
	
	rec_level--;
	didright = rec_didleft[rec_level];
	cur = rec_node[rec_level];
	didleft = TRUE;
	keepgoing = TRUE;
    }
    return(toodeep);
}


/*
 * Bump the reference count for a filter.
 */
void reference_filter (ulong index)
{
    filter_object *ptr = filter_obj_ptr(index);

    if (ptr) 
	ptr->refcount++;
}


/*
 * Decrement the reference count for a filter.
 */
void unreference_filter (ulong index)
{
    filter_object *ptr = filter_obj_ptr(index);

    if (ptr)
	if (ptr->refcount > 0)
	    ptr->refcount--;
}

/*
 * Node start routine for use when freeing a tree.
 */
static boolean free_start (filter_expr *node)
{
    /*
     * If we're at the root of a tree and we're not just starting out,
     * decrement the node's reference count and stop recursing.
     */
    if (node->index && rec_level != 0) {
	unreference_filter(node->index);
	return(FALSE);	/* Stop recursing. */
    }

    /*
     * Decrement the reference count of the children of the tree if they
     * are leaves, or if they are placeholders.
     */

    if (node->leftindex && 
	(node->lefttype != FILTER_TERM_EXPR || !node->left.expr))
	unreference_filter(node->leftindex);
    if (node->rightindex && 
	(node->righttype != FILTER_TERM_EXPR || !node->right.expr))
	unreference_filter(node->rightindex);

    return(TRUE);
}

/*
 * Node end routine for use when freeing a tree.
 */
static boolean free_end (filter_expr *node)
{
    /* We've already cleaned out the node, so just release it now. */

    free(node);
    return(TRUE);
}

/*
 * Free a tree.
 */
static void free_tree (filter_expr *tree)
{
    /* Call the recursion routine to make it happen. */

    (void) recursive_walk_aux(tree, NULL, free_start, NULL, free_end);
}



/*
 * Formatting routines.  A recursive walk is done on a tree both to
 * do nvram command generation and to handle the show command.
 */

static char tree_text[MAX_FILTER_NAME_LENGTH + 20]; /* Buffer to build text */

/*
 * Node start routine for formatting.
 */
static boolean format_start (filter_expr *node, boolean recurse)
{
    char *source_dest = "";
    char *leftname = "";

    /*
     * If we're running into the root of a tree and we're not just starting
     * out, print the name of the subtree and stop recursing.
     */
    if (!recurse) {
	if (rec_level != 0 && node->index) {
	    strcpy(tree_text, filter_obj_ptr(node->index)->name);
	    return(FALSE);
	}
    }

    /* If the left child is a set, note the source or destination. */

     if (node->lefttype != FILTER_TERM_EXPR) {
	if (node->lefttype == FILTER_TERM_SRC)
	    source_dest = "src ";
	else
	    source_dest = "dest ";
    }

    /*
     * Print the left child's node name if the child is not an expression,
     * or the child is a placeholder.
     */

    if (node->lefttype != FILTER_TERM_EXPR || !node->left.expr)
	leftname = filter_obj_ptr(node->leftindex)->name;

    /*
     * Format the text, noting a new nesting level, and issuing the NOT 
     * operator (if necessary).
     */
    sprintf(tree_text, "%s%s%s%s", 
	    (rec_level != 0) ? "(" : "",
	    (node->operator == FILTER_OP_NOT) ? "not " : "",
	    source_dest, leftname);

    return(TRUE);
}

/*
 * Node middle routine for formatting.
 */
static boolean format_middle (filter_expr *node, boolean recurse)
{
    /* Print the operator. */

    switch (node->operator) {
	
    case FILTER_OP_AND:
	strcpy(tree_text, " and ");
	break;
	
    case FILTER_OP_OR:
	strcpy(tree_text, " or ");
	break;
	
    case FILTER_OP_XOR:
	strcpy(tree_text, " xor ");
	break;
	
    default:
	break;
    }
    return(TRUE);
}


/*
 * Node end routine for formatting.
 */
static boolean format_end (filter_expr *node, boolean recurse)
{
    char *source_dest = "";
    char *rightname = "";

    /* If the right child is a set, note the source or destination. */

     if (node->rightindex && node->righttype != FILTER_TERM_EXPR) {
	if (node->righttype == FILTER_TERM_SRC)
	    source_dest = "src ";
	else
	    source_dest = "dest ";
    }

    /*
     * Print the name of the right child if it isn't an expression or
     * it is a placeholder.
     */

    if (node->rightindex && 
	(node->righttype != FILTER_TERM_EXPR || !node->right.expr))
	rightname = filter_obj_ptr(node->rightindex)->name;

    /* Format the text. */

    sprintf(tree_text, "%s%s%s",
	    source_dest, rightname,
	    (rec_level != 0) ? ")" : "");

    return(TRUE);
}


/*
 * Variants for the show command.
 */

static boolean print_detail;	/* True if we should recurse */

static boolean print_start (filter_expr *node)
{
    boolean ret = format_start(node, print_detail);
    printf("%s", tree_text);
    return(ret);
}

static boolean print_middle (filter_expr *node)
{
    boolean ret = format_middle(node, print_detail);
    printf("%s", tree_text);
    return(ret);
}

static boolean print_end (filter_expr *node)
{
    boolean ret = format_end(node, print_detail);
    printf("%s", tree_text);
    return(ret);
}

static void print_filter_expr (filter_expr *tree, boolean detail)
{
    print_detail = detail;
    (void) recursive_walk_aux(tree, NULL, print_start, print_middle, 
			      print_end);
}


/*
 * Variants for nvram generation.
 */
static boolean nvgen_start (filter_expr *node)
{
    boolean ret = format_start(node, FALSE);
    nv_add(TRUE, "%s", tree_text);
    return(ret);
}

static boolean nvgen_middle (filter_expr *node)
{
    boolean ret = format_middle(node, FALSE);
    nv_add(TRUE, "%s", tree_text);
    return(ret);
}

static boolean nvgen_end (filter_expr *node)
{
    boolean ret = format_end(node, FALSE);
    nv_add(TRUE, "%s", tree_text);
    return(ret);
}

static void nvgen_tree (filter_expr *tree)
{
        (void) recursive_walk_aux(tree, NULL, nvgen_start, nvgen_middle, 
			      nvgen_end);
}



/*
 * Walk the entire set of filter expressions looking for excessive tree depth.
 * This can occur because entire trees can be linked into other trees as
 * subtrees (and circular references can be formed).
 *
 * Returns the index of the first filter expression to be found to be
 * excessively deep, or 0 if no overly deep expression tree is found.
 */
static ulong check_recursion_depth (void)
{
    ulong index;
    filter_object *ptr;

    /* Look down the list of filters, checking each filter expression found. */

    FOR_ALL_FILTERS(index, ptr) {
	if (ptr->name == NULL)
	    continue;		/* Skip unused ones */
	if (ptr->type != FILTER_EXPR)
	    continue;		/* Skip non-expressions */
	if (recursive_walk_aux(ptr->filptr.expr, NULL, NULL, NULL, NULL))
	    return(index);	/* Woops--too deep! */
    }
    return(0);
}


/*
 * Another use of recursive walking is to check for loops.  The following
 * routines implement a check for potential loops in the tree.  The check
 * is done before linking in a node.
 */

/* Helper variables for circularity check */

static boolean its_circular;	/* Set TRUE if we find a circularity */
static ulong circular_index;	/* Index we are checking for */

/*
 * Node start routine for circularity check.
 */

static boolean circ_start (filter_expr *node)
{
    /* If the left or right index matches the test index, note it and quit. */

    if (node->leftindex == circular_index || 
	node->rightindex == circular_index) {
	its_circular = TRUE;
	return(FALSE);
    }
    return(TRUE);
}

/*
 * Recursively descend a tree looking for references to a particular
 * object.  Returns TRUE if a reference is found.
 */

static boolean check_circular (ulong index, ulong testindex)
{
    filter_object *obj = filter_obj_ptr(testindex);
    filter_expr *node = obj->filptr.expr;

    /* Catch references to ourselves! */

    if (index == testindex)
	return(TRUE);

    /*
     * Skip it if there's no node to check, or if the node to be tested is
     * not an expression.
     */

    if (obj->type != FILTER_EXPR || !node)
	return(FALSE);

    /* Let the recursion do the walking. */

    its_circular = FALSE;
    circular_index = index;
    (void) recursive_walk_aux(node, NULL, circ_start, NULL, NULL);
    return(its_circular);
}


/*
 * Recursively walk all trees, updating all pointer references to
 * an object.  This is done when a referenced object is deleted and then
 * recreated.
 */

static filter_element update_ptr; /* Pointer to insert */
static ulong update_index; /* Index of object being updated */

/* Node start routine */

static boolean update_start (filter_expr *node)
{
    /* If we've hit a subtree, bug out. */

    if (rec_level && node->index)
	return(FALSE);

    /*
     * If the index matches and the pointer is null, update the node.  Update
     * the type as well if we've changed the type of a placeholder from
     * FILTER_EXPR to FILTER_LEAF.
     */

    if (node->leftindex == update_index &&
	node->left.generic == NULL) {
	node->left = update_ptr;
	if (filter_obj_ptr(update_index)->type == FILTER_LEAF &&
	    node->lefttype == FILTER_TERM_EXPR)
	    node->lefttype = FILTER_TERM_DST;
    }
    if (node->rightindex == update_index &&
	node->right.generic == NULL) {
	node->right = update_ptr;
	if (filter_obj_ptr(update_index)->type == FILTER_LEAF &&
	    node->righttype == FILTER_TERM_EXPR)
	    node->righttype = FILTER_TERM_DST;
    }
    return(TRUE);
}

void update_filter_pointers (ulong index)
{
    ulong i;
    filter_object *obj;

    /* Walk each filter object, recursively descending each expresion. */

    update_index = index;
    update_ptr = filter_obj_ptr(index)->filptr;

    FOR_ALL_FILTERS(i, obj) {
	if (i == index)		/* It's us! */
	    continue;
	if (!obj->name)		/* Empty entry */
	    continue;
	if (obj->type != FILTER_EXPR) /* Not an expression */
	    continue;
	if (!obj->filptr.expr)	/* Placeholder */
	    continue;
	(void) recursive_walk_aux(obj->filptr.expr, NULL, update_start,
				  NULL, NULL);
    }
}


/*
 * Recursively walk all trees, deleting all pointer references to
 * an object.  This is done when a referenced object is deleted.
 */

static ulong delete_index;	/* Index of object being deleted */

/* Node start routine */

static boolean delete_start (filter_expr *node)
{
    /* If we've hit a subtree, bug out. */

    if (rec_level && node->index)
	return(FALSE);

    /*
     * If the index matches, zero the pointer.
     */

    if (node->leftindex == delete_index) {
	node->left.generic = NULL;
    }
    if (node->rightindex == delete_index) {
	node->right.generic = NULL;
    }
    return(TRUE);
}

void delete_filter_pointers (ulong index)
{
    ulong i;
    filter_object *obj;

    /* Walk each filter object, recursively descending each expresion. */

    delete_index = index;

    FOR_ALL_FILTERS(i, obj) {
	if (i == index)		/* It's us! */
	    continue;
	if (!obj->name)		/* Empty entry */
	    continue;
	if (obj->type != FILTER_EXPR) /* Not an expression */
	    continue;
	if (!obj->filptr.expr)	/* Placeholder */
	    continue;
	(void) recursive_walk_aux(obj->filptr.expr, NULL, delete_start,
				  NULL, NULL);
    }
}

#ifdef DEBUG_TREE

/*
 * Consistency checking routine.  Call this whenever anything gets changed.
 */
static boolean con_check_start (filter_expr *node)
{
    generic_leaf *leaf;
    filter_expr *expr;
    
    /* If we've hit a new subtree, back off. */
    
    if (node->index && rec_level)
	return(FALSE);
    
    /* Operator must be consistent with children. */
    
    switch(node->operator) {
    case FILTER_OP_AND:
    case FILTER_OP_OR:
    case FILTER_OP_XOR:
	/* Dyadic operator.  Check the righthand side. */
	
	if (!node->rightindex)
	    (void) breakpoint();
	
	/* Type must be valid. */
	
	switch (node->righttype) {
	case FILTER_TERM_SRC:
	case FILTER_TERM_DST:
	    
	    /* Right side is a leaf. If it's there, the index must match. */
	    
	    leaf = node->right.generic;
	    if (leaf) {
		if (leaf->generic_hdr.index != node->rightindex)
		    (void) breakpoint();
	    }
	    if (filter_obj_ptr(node->rightindex)->type != FILTER_LEAF)
		(void) breakpoint();
	    break;
	case FILTER_TERM_EXPR:
	    
	    /* Right side is an expression.  If there, the index must match.*/
	    
	    expr = node->right.expr;
	    if (expr) {
		if (expr->index != node->rightindex)
		    (void) breakpoint();
	    }
	    if (filter_obj_ptr(node->rightindex)->type != FILTER_EXPR)
		(void) breakpoint();
	    break;
	default:
	    (void) breakpoint();
	    break;
	}
	break;
    case FILTER_OP_NOT:
    case FILTER_OP_NONE:
	/* Monadic operator.  Must not have a right side. */
	
	if (node->rightindex || node->right.generic)
	    (void) breakpoint();
	break;
    default:
	(void) breakpoint();
	break;
    }
    
    /* Check the left side. */
    
    if (!node->leftindex)
	(void) breakpoint();
    
    /* Type must be valid. */
    
    switch (node->lefttype) {
    case FILTER_TERM_SRC:
    case FILTER_TERM_DST:
	
	/* Left side is a leaf. If it's there, the index must match. */
	
	leaf = node->left.generic;
	if (leaf) {
	    if (leaf->generic_hdr.index != node->leftindex)
		(void) breakpoint();
	}
	if (filter_obj_ptr(node->leftindex)->type != FILTER_LEAF)
	    (void) breakpoint();
	break;
    case FILTER_TERM_EXPR:
	
	/* Left side is an expression.  If there, the index must match.*/
	
	expr = node->left.expr;
	if (expr) {
	    if (expr->index != node->leftindex)
		(void) breakpoint();
	}
	if (filter_obj_ptr(node->leftindex)->type != FILTER_EXPR)
	    (void) breakpoint();
	break;
    default:
	(void) breakpoint();
	break;
    }
    
    /* Now check the index values for consistency. */
    
    if (node->leftindex) {
	if (!filter_obj_ptr(node->leftindex)->refcount)
	    (void) breakpoint();
	if (filter_obj_ptr(node->leftindex)->filptr.generic != 
	    node->left.generic)
	    (void) breakpoint();
    }
    if (node->rightindex) {
	if (!filter_obj_ptr(node->rightindex)->refcount)
	    (void) breakpoint();
	if (filter_obj_ptr(node->rightindex)->filptr.generic != 
	    node->right.generic)
	    (void) breakpoint();
    }
    return(TRUE);
}


void filter_consistency_check (void) {
    
    ulong i;
    filter_object *obj;

    /* Nothing to do if no objects. */

    if (!filter_objects_count)
	return;

    /* Ensure index zero is unused. */

    if (filter_objects->name)
	(void) breakpoint();

    /* Walk the object table. */
    
    FOR_ALL_FILTERS(i, obj) {
	if (!obj->name)
	    continue;

	/* Should have a valid protocol type. */

	switch (obj->protocol) {
	case FILTER_PROTO_CLNS:
	case FILTER_PROTO_IP:
	    break;
	default:
	    (void) breakpoint();
	}

	/* Should have a valid type. */
	
	switch(obj->type) {
	case FILTER_LEAF:
	    
	    /* Filter leaf.  Ensure index is correct if object is there. */
	    
	    if (obj->filptr.generic) {
		if (obj->filptr.generic->generic_hdr.index != i) {
		    (void) breakpoint();
		}
	    }
	    break;
	    
	case FILTER_EXPR:
	    
	    /* Filter expression.  Do recursive descent. */
	    
	    if (obj->filptr.expr) {
		if (obj->filptr.expr->index != i)
		    (void) breakpoint();
		(void) recursive_walk_aux(obj->filptr.expr, NULL,
					  con_check_start, NULL, NULL);
	    }
	    break;

	case FILTER_UNDEF:

	    /* Undefined--it better not have anything attached. */

	    if (obj->refcount)
		(void) breakpoint();
	    if (obj->filptr.generic)
		(void) breakpoint();
	    break;

	default:
	    (void) breakpoint();
	}
    }
 }

#endif /* DEBUG_TREE */

#ifdef PRINT_TREE
static void print_filter_type (filter_type type)
{
    switch (type) {
    case FILTER_TERM_SRC:
	printf("src");
	break;
    case FILTER_TERM_DST:
	printf("dest");
	break;
    case FILTER_TERM_EXPR:
	printf("expr");
	break;
    default:
	break;
    }
}

void print_filters (void) {
    ulong i;
    filter_object *obj;
    filter_expr *expr;
    char *proto_name;

    FOR_ALL_FILTERS(i, obj) {
	if (!obj->name)
	    continue;
	switch (obj->protocol) {
	case FILTER_PROTO_CLNS:
	    proto_name = "CLNS";
	    break;
	case FILTER_PROTO_IP:
	    proto_name = "IP";
	    break;
	default:
	    proto_name = "UNKNOWN";
	}

	printf("\n(%d) %s (%s):  %d refs, ", i, obj->name, proto_name, 
	       obj->refcount);
	switch (obj->type) {
	case FILTER_UNDEF:
	    printf("UNDEF");
	    break;
	case FILTER_LEAF:
	    printf("LEAF (%x)", obj->filptr.generic);
	    if (obj->filptr.generic)
		printf(", ix %d", obj->filptr.generic->generic_hdr.index);
	    break;
	case FILTER_EXPR:
	    expr = obj->filptr.expr;
	    printf("EXPR (%x)", expr);
	    if (expr) {
		printf(", left %d (%x) ", expr->leftindex, expr->left.expr);
		print_filter_type(expr->lefttype);
		printf(", right %d (%x) ", expr->rightindex, expr->right.expr);
		print_filter_type(expr->righttype);
		printf("\n  ");
		print_filter_expr(expr, TRUE);
	    }
	    break;
	}
    }
}
#endif /* PRINT_TREE */


/*
 * proto filter-expression name not term
 * proto filter-expression name term and term
 * proto filter-expression name term or term
 * proto filter-expression name term xor term
 * no proto filter-expression name
 *
 * term := name
 *         source name
 *         destination name
 *
 * OBJ(string,1) = filter name
 * OBJ(string,2) = term 1 name
 * OBJ(string,3) = term 2 name
 * OBJ(int,1) = operator
 * OBJ(int,2) = term 1 qualifier
 * OBJ(int,3) = term 2 qualifier, or 0 if no term 2
 * OBJ(int,4) = protocol type (FILTER_PROTO_...)
 */
void filter_expr_cmd (parseinfo *csb)
{
    filter_object *fltrptr;
    ulong index;
    ulong term1index = 0;
    ulong term2index = 0;
    filter_object *term1ptr = NULL;
    filter_object *term2ptr = NULL;
    filter_expr *expr;

    if (!csb->sense) {

	/* no proto filter-expression name */

	/* Get the object; complain if it's not there. */

	index = get_filter_index(GETOBJ(string,1));
	if (!index) {
	    printf("\n%%FILTER: No such expression");
	    return;
	}
	fltrptr = filter_obj_ptr(index);

	/* Complain if it's the wrong type. */

	if (fltrptr->type != FILTER_EXPR) {
	    printf("\n%%FILTER: Not a filter-expression");
	    return;
	}

	/* Complain if it's the wrong protocol ID. */

	if (fltrptr->protocol != GETOBJ(int,4)) {
	    printf("\n%%FILTER: Wrong protocol type");
	    return;
	}

	/* Skip most stuff if we're just a placeholder. */

	if (fltrptr->filptr.expr) {
	    
	    /* Zero out all pointer references to this expression. */
	    
	    delete_filter_pointers(index);
	    
	    /* Detach the expression from the filter object. */
	    
	    expr = fltrptr->filptr.expr;
	    fltrptr->filptr.expr = NULL;
	    
	    /* Recursively descend the chain, freeing everything we can. */
	    
	    free_tree(expr);
	}
	
	/* Clean up any orphaned objects we might be leaving behind. */

	clean_up_filter_objects();
	CONSISTENCY_CHECK();
	DUMP_TREE();

    } else if (csb->nvgen) {

	/*
	 * NVGEN.  Scan the list of filter objects, and call the nvgen_tree
	 * routine for each filter expression found.
	 */

	FOR_ALL_FILTERS(index, fltrptr) {
	    if (!fltrptr->name)
		continue;
	    if (fltrptr->type != FILTER_EXPR)
		continue;
	    if (fltrptr->protocol != GETOBJ(int,4))
		continue;
	    if (fltrptr->filptr.expr) {
		nv_write(TRUE, "%s %s ", csb->nv_command, fltrptr->name);
		nvgen_tree(fltrptr->filptr.expr);
	    }
	}

    } else {

	/*
	 * It's the real thing.  Basic syntax checking has already been
	 * done by the parser.
	 */

	/* Ensure the object name isn't too long. */

	if (strlen(GETOBJ(string,1)) > MAX_FILTER_NAME_LENGTH) {
	    printf("\n%%FILTER: Filter name too long");
	    return;
	}

	/* Get/create the first term. */

	term1index = create_filter_object(GETOBJ(string,2));
	if (!term1index) {
	nomem:
	    printf(nomemory);
	    goto cleanup;
	}

	/*
	 * If we just created it, make an educated guess as to the 
	 * object type, based on context.
	 */
	term1ptr = filter_obj_ptr(term1index);
	if (term1ptr->type == FILTER_UNDEF) {
	    if (GETOBJ(int,2) == FILTER_UNKNOWN)
		term1ptr->type = FILTER_EXPR;
	    else
		term1ptr->type = FILTER_LEAF;
	    term1ptr->protocol = GETOBJ(int,4);

	} else {

	    /*
	     * Didn't just create it--ensure that the protocol type matches.
	     */
	    if (term1ptr->protocol != GETOBJ(int,4)) {
		printf("\n%%FILTER: Conflicting protocol type");
		goto cleanup;
	    }

	    /*
	     * If the term is an expression and the current expression
	     * implies that it is a set, bitch if the term really exists,
	     * or convert it to a set if it's a placeholder.
	     */
	    if (term1ptr->type == FILTER_EXPR &&
		GETOBJ(int,2) != FILTER_UNKNOWN) {
		if (term1ptr->filptr.expr) {
		    printf("\n%%FILTER: Can't qualify filter-expr with src/dest");
		    goto cleanup;
		} else {
		    term1ptr->type = FILTER_LEAF;
		    update_filter_pointers(term1index);
		}
	    }
	}
	
	if (GETOBJ(int,3)) { /* A second term? */
	    
	    /* Get/create the second term, as above. */
	    
	    term2index = create_filter_object(GETOBJ(string,3));
	    if (!term2index) {
		goto nomem;
	    }
	    term2ptr = filter_obj_ptr(term2index);
	    if (term2ptr->type == FILTER_UNDEF) {
		if (GETOBJ(int,3) == FILTER_UNKNOWN)
		    term2ptr->type = FILTER_EXPR;
		else
		    term2ptr->type = FILTER_LEAF;
		term2ptr->protocol = GETOBJ(int,4);

	    } else {

		if (term2ptr->protocol != GETOBJ(int,4)) {
		    printf("\n%%FILTER: Conflicting protocol type");
		    goto cleanup;
		}
		if (term2ptr->type == FILTER_EXPR &&
		    GETOBJ(int,3) != FILTER_UNKNOWN) {
		    if (term2ptr->filptr.expr) {
			printf("\n%%FILTER: Can't qualify filter-expr with src/dest");
			goto cleanup;
		    } else {
			term2ptr->type = FILTER_LEAF;
			update_filter_pointers(term2index);
		    }
		}
	    }
	    
	} else if (GETOBJ(int,1) == 0) { /* No operator */
	    /*
	     * No second term and no operator.  This can only happen if
	     * the term is a filter set.
	     */
	    if (term1ptr->type != FILTER_LEAF) {
		printf("\n%%FILTER: Parameter must be a filter-set");
		goto cleanup;
	    }
	    
	    /* No second term, but there's an operator.  Sanity check. */
	    
	} else if (GETOBJ(int,1) != FILTER_OP_NOT) {
	    printf("\n%%FILTER: No 2nd term and operator isn't NOT");
	    goto cleanup;
	}

	/* Everything checks out.  Start grabbing memory. */

	index = create_filter_object(GETOBJ(string,1));
	if (!index)
	    goto nomem;

	fltrptr = filter_obj_ptr(index);
	term1ptr = filter_obj_ptr(term1index); /* We invalidated it! */
	term2ptr = filter_obj_ptr(term2index); /* Ditto! */

	/* If the filter object is already in use, complain. */

	if (fltrptr->filptr.expr) {
	    printf("\n%%FILTER: Filter object is in use");
	    goto cleanup;
	}

	/*
	 * If the filter object has no type yet, assign it.  If it has a type,
	 * ensure that it's proper, and that the protocol ID matches.
	 */

	if (fltrptr->type == FILTER_UNDEF) {
	    fltrptr->type = FILTER_EXPR;
	    fltrptr->protocol = GETOBJ(int,4);
	} else {

	    if (fltrptr->type != FILTER_EXPR) {
		printf("\n%%FILTER: Conflicting filter object name");
		goto cleanup;
	    }

	    if (fltrptr->protocol != GETOBJ(int,4)) {
		printf("\n%%FILTER: Conflicting protocol type");
		goto cleanup;
	    }
	}
	
	/* Check for circularity. */
	
	if (term1ptr->type == FILTER_EXPR) {
	    if (check_circular(index, term1index)) {
		printf("\n%%FILTER: Can't create looping filters");
		goto cleanup;
	    }
	}
	if (term2index) {
	    if (term2ptr->type == FILTER_EXPR) {
		if (check_circular(index, term2index)) {
		    printf("\n%%FILTER: Can't create looping filters");
		    goto cleanup;
		}		    
	    }
	}
	
	/* Allocate the filter_expression. */
	
	expr = malloc(sizeof(filter_expr));
	if (!expr)
	    goto nomem;
	expr->index = index;
	expr->operator = GETOBJ(int,1);
	if (!expr->operator)
	    expr->operator = FILTER_OP_NONE;
	expr->leftindex = term1index;
	expr->rightindex = term2index;
	
	/*
	 * Link the terms into the filter expression.
	 */
	if (term1ptr->type == FILTER_EXPR) {
	    expr->lefttype = FILTER_TERM_EXPR;
	} else if (GETOBJ(int,2) == FILTER_TERM_SRC) {
	    expr->lefttype = FILTER_TERM_SRC;
	} else {
	    expr->lefttype = FILTER_TERM_DST;
	}
	expr->left = term1ptr->filptr;
	reference_filter(term1index);
	
	if (term2index) {
	    if (term2ptr->type == FILTER_EXPR) {
		expr->righttype = FILTER_TERM_EXPR;
	    } else if (GETOBJ(int,3) == FILTER_TERM_SRC) {
		expr->righttype = FILTER_TERM_SRC;
	    } else {
		expr->righttype = FILTER_TERM_DST;
	    }
	    expr->right = term2ptr->filptr;
	    reference_filter(term2index);
	}
	
	/* All set.  Link the expression into the object. */
	
	fltrptr->filptr.expr = expr;

	/* Update any objects that might point to us. */

	update_filter_pointers(index);

	/* Do a recursion depth check.  Complain if it's too deep. */

	index = check_recursion_depth();
	if (index)
	    printf("\n%%FILTER: Warning--object '%s' is too complex",
		   filter_obj_ptr(index)->name);
	DUMP_TREE();
    }
    CONSISTENCY_CHECK();
    return;

 cleanup:
    clean_up_filter_objects();
}


/*
 * Show filter expressions.
 */
void show_filter_expr (
char *name,
boolean name_present,
boolean detail,
filter_protocol_type protocol)
{
    ulong first, last;
    filter_object *ptr;
    ulong i;

    /* If there are no filter objects, don't bother. */

    if (filter_objects_count == 0)
	return;

    /* If a name is present, ensure it exists. */

    if (name_present) {
	first = last = get_filter_index(name);
	ptr = filter_obj_ptr(first);
	if (!first || !ptr->filptr.generic) {
	    printf("\n%%FILTER: No such object");
	    return;
	}
	if (ptr->type != FILTER_EXPR) {
	    printf("\n%%FILTER: Object is not a filter expression");
	    return;
	}
	if (ptr->protocol != protocol) {
	    printf("\n%%FILTER: Conflicting protocol type");
	    return;
	}
    } else {
	first = 1;
	last = filter_objects_count - 1;
	ptr = filter_obj_ptr(first);
    }

    /* Run the specified range. */

    automore_enable(NULL);
    for (i=first; i<=last; i++, ptr++) {
	if (!ptr->name)		/* Unused */
	    continue;
	if (ptr->type != FILTER_EXPR) /* Not an expression */
	    continue;
	if (ptr->protocol != protocol) /* Wrong protocol */
	    continue;
	if (!ptr->filptr.expr)	/* Placeholder */
	    continue;
	printf("\n%s = ", ptr->name);
	print_filter_expr(ptr->filptr.expr, detail);
    }
    automore_disable();
}

/*
 * Return the name of a given filter object.
 */
char *filter_name (ulong index)
{
    filter_object *obj = filter_obj_ptr(index);
    if (!obj)
	return("");
    return(obj->name);
}
