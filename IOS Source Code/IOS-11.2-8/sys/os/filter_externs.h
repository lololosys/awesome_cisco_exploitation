/* $Id: filter_externs.h,v 3.2 1995/11/17 18:47:26 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/filter_externs.h,v $
 *------------------------------------------------------------------
 * filter_externs.h--Generic filter extern definitions
 *
 * July 1992, Dave Katz
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: filter_externs.h,v $
 * Revision 3.2  1995/11/17  18:47:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:54:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern filter_object *filter_objects;
extern int filter_objects_count;

extern void show_filter_expr(char *name, boolean name_present, boolean detail,
			     filter_protocol_type protocol);
extern char *filter_name(ulong index);
extern void filter_consistency_check(void);
extern void print_filters(void);
extern void clean_up_filter_objects(void);
extern ulong create_filter_object(char *name);
extern boolean filter_recursive_walk_execute(filter_expr *root, 
					     boolean (*leafrtn)(generic_leaf *, filter_type, void *, void *), 
					     void *parm1, void *parm2);
extern void reference_filter(ulong index);
extern void unreference_filter(ulong index);
extern filter_object *filter_obj_ptr(ulong index);
extern void delete_filter_pointers(ulong index);
extern void update_filter_pointers(ulong index);
extern ulong get_filter_index(char *name);
