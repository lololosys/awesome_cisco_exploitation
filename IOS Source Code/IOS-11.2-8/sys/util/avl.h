/* $Id: avl.h,v 3.2 1995/11/17 17:56:05 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/util/avl.h,v $
 *------------------------------------------------------------------
 * avl.h - Definitions for AVL trees
 *
 * Jan 1995 Tony Li
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: avl.h,v $
 * Revision 3.2  1995/11/17  17:56:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:43:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:12:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Instructions:
 *
 *			     READ BEFORE USING
 *
 *
 *			       REALLY!!!!!!
 *
 * An AVL node is the basic structure which the tree is built around.  To
 * use these routines, embed the following structure in your applications
 * structure at the very top.  Example:
 *
 * typedef footype_ {
 *	avl_node_type avlnode;
 *	int frobnitz;
 *	widgettype widget;
 * } footype;
 *
 * Next, define a comparison routine which compares two footypes and
 * returns an avl_compare:
 *
 * enum avl_compare foo_compare (footype a, footype b) ....
 *
 * Finally, for the AVL functions that you want to use (avl_insert,
 * avl_delete, avl_walk, etc.), define some static inline wrappers which
 * simply cast arguments around to make everyone happy.  This does not
 * imply that you should not do it carefully.  Example:
 *
 * static inline boolean foo_get_next (footype *current, footype **next) 
 * {
 *	return(avl_get_next((avl_node_type *) current,
 * 			    (avl_node_type **)next,
 *			    (avl_compare_type) foo_compare);
 * }
 *
 * You're all done.
 *
 * NB: The caller is responsible for ALL memory management.
 */

typedef enum balance_type_ {
    LEFT_HEAVY = -1,
    BALANCED = 0,
    RIGHT_HEAVY = 1,
} balance_type;

typedef struct avl_node_type_ {
    struct avl_node_type_ *left;	/* Left child */
    struct avl_node_type_ *right;	/* Right child */
    balance_type balance;		/* Current balance status */
} avl_node_type;

enum avl_compare {AVL_LT, AVL_EQ, AVL_GT};

/*
 * avl_get_quickest
 *
 * Return the easiest possible element in the tree.  In this case, the top.
 * Seems silly, but prevents anyone from reaching inside the
 * implementation.
 */
static inline avl_node_type *
avl_get_quickest (avl_node_type *top)
{
    return(top);
}

/*
 * A walker node is called at each node in the tree.  It returns FALSE to
 * stop the walk.
 */
typedef boolean (*avl_walker_type)(avl_node_type *avl_node, va_list ap);

/*
 * A comparison routine is defined by the called to return comparisons
 * about two elements in the tree.  AVL_LT implies that node_one <
 * node_two, AVL_EQ implies that node_one == node_two, and AVL_GT implies
 * that node_one > node_two.
 */
typedef enum avl_compare (*avl_compare_type)(avl_node_type *node_one,
					     avl_node_type *node_two);

extern boolean avl_walk(avl_node_type *avl_node, avl_walker_type proc,
			...);
extern avl_node_type *avl_get_next(avl_node_type *top,
				   avl_node_type *current, 
				   avl_compare_type compare_func);
extern avl_node_type *avl_get_first(avl_node_type *top);
extern avl_node_type *avl_insert(avl_node_type **top,
				 avl_node_type *new,
				 boolean *balancing_needed,
				 avl_compare_type compare_func);
extern avl_node_type *avl_delete (avl_node_type **top,
				  avl_node_type *target,  
				  boolean *balancing_needed,
				  avl_compare_type compare_func);
extern avl_node_type *avl_search (avl_node_type *top, avl_node_type *goal,
				  avl_compare_type compare_func);
extern void show_avl_tree(avl_node_type *top, avl_compare_type compare_func);

