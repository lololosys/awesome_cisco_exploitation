/* $Id: filter.h,v 3.2 1995/11/17 18:47:24 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/filter.h,v $
 *------------------------------------------------------------------
 * Data structures for generic filtering
 *
 * July, 1992, Dave Katz
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: filter.h,v $
 * Revision 3.2  1995/11/17  18:47:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:54:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Symbols used in parsing
 */

/*
 * The following values are stored when recognizing a filter expression
 * operator.
 */
typedef enum filter_op_ {FILTER_OP_AND = 1, FILTER_OP_OR, FILTER_OP_XOR,
			     FILTER_OP_NOT, FILTER_OP_NONE}
filter_op;

/*
 * The following values are stored when recognizing a filter expression
 * term.  They are mutually exclusive.
 */
typedef enum filter_type_ {FILTER_TERM_SRC = 1, FILTER_TERM_DST, 
				    FILTER_TERM_EXPR, FILTER_UNKNOWN}
filter_type;

/*
 * The following values are stored when recognizing a filter object.
 */

typedef enum filter_protocol_type_ {FILTER_NO_PROTO = 0, FILTER_PROTO_CLNS, 
				     FILTER_PROTO_IP}
filter_protocol_type;


/*
 * Common substructure for filter leaves
 *
 * This structure must appear first in any filter leaf.
 */
typedef struct filter_leaf_hdr_ {
    ulong index;		/* Object index of this leaf */
} filter_leaf_hdr;


/*
 * Generic leaf structure
 */
typedef struct generic_leaf_ {
    filter_leaf_hdr generic_hdr; /* Leaf header */
} generic_leaf;


/*
 * A union to carry filter leaves and filter expressions together
 */
typedef union filter_element_ {
    struct filter_expr_ *expr;
    generic_leaf *generic;
} filter_element;


/*
 * Filter expression.  A binary tree of these describes any arbitrary
 * filter.  The left and right children are either filter sets (applied
 * to either source or destination addresses) or further filter expressions.
 * The operation says what to do with the children.
 */

typedef struct filter_expr_ {
    filter_element left;	/* Left child/object */
    filter_element right;	/* Right child/object */
    ulong leftindex;	/* Index of left child */
    ulong rightindex;	/* Index of right child */
    filter_type lefttype;	/* Type of left child */
    filter_type righttype;	/* Type of right child */
    filter_op operator;		/* Operator */
    ulong index;		/* object index if root of tree, or 0 */
} filter_expr;


/*
 * Filter object structure.  A linear array of these structures is used
 * to bind filter structures to processes that reference them--the index into
 * the table is used.  Indices are always greater than zero;  index 0
 * is unused.
 *
 * This indirection allows filter structures to be destroyed without destroying
 * the references to them (e.g. "no clns filter-set foo" when a "clns
 * access-group foo" has already been done).
 *
 * If the "name" field is null, the entry is free.
 *
 * If the pointer is NULL but the type is defined, this means that the
 * object has been deleted, but not all references have been removed.
 * This allows the contents of an object to be replaced without removing
 * its references.  A type of FILTER_UNDEF says that the object has just
 * been allocated
 *
 * The reference count lets us know when it is OK to deallocate the name;
 * when the referenced object and all references to the name are deleted,
 * the name object can be deleted.
 */

#define MAX_FILTER_NAME_LENGTH 30 /* Max length of filter object name */

typedef struct filter_object_ {
    char *name;			/* object name */
    filter_protocol_type protocol; /* Protocol type */
    filter_element filptr;	/* pointer to filter expression/leaf */
    int refcount;		/* reference count */
    enum {FILTER_UNDEF = 0, FILTER_LEAF, FILTER_EXPR} type; /* object type */
} filter_object;

/*
 * Define the symbol DEBUG_TREE to enable consistency checking.
 */

#ifdef DEBUG_TREE
#define CONSISTENCY_CHECK() filter_consistency_check()
#else
#define CONSISTENCY_CHECK()
#endif

/*
 * Define the symbol PRINT_TREE to enable tree dumping.
 */

#ifdef PRINT_TREE
#define DUMP_TREE() print_filters()
#else
#define DUMP_TREE()
#endif


/*
 * A macro for walking the filter object array.
 */

#define FOR_ALL_FILTERS(ix,ptr) \
for((ix)=1,(ptr)=filter_objects+1; (ix)<filter_objects_count; (ix)++,(ptr)++)
