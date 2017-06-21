/* $Id: list.h,v 1.2 1996/05/09 20:32:44 jchlin Exp $
 * $Source: /cscka/t1_neat/src/common/list.h,v $
 *------------------------------------------------------------------
 * list.h -- NEAT processor's Link List Management definitions.
 *
 * April, 1996 - Jonathan Chiung-Hsi Lin
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: list.h,v $
 * Revision 1.2  1996/05/09 20:32:44  jchlin
 * The initial commit for double link list utility.
 * Ths double link list utility can be used to make
 * list management faster than the single link list.
 *
 * Revision 1.1  1996/04/05  00:34:38  jchlin
 * Placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __LIST_H__
#define __LIST_H__
 
typedef struct dlink_node_t_ {
    void   *next;
    void   *prev;
} dlink_node_t;

extern void dlink_node_init(dlink_node_t *node);
extern void dlink_node_insert_next(dlink_node_t *curr_node, 
				   dlink_node_t *new_node);
extern void dlink_node_insert_prev(dlink_node_t *curr_node,
				   dlink_node_t *new_node);
extern void dlink_node_delete(dlink_node_t **header, dlink_node_t *node);
extern void dlink_list_delete(dlink_node_t **header, dlink_node_t *first_node,
			      dlink_node_t *last_node);

#endif /* __LIST_H__ */
