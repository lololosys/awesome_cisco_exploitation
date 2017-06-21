/* $Id: list.h,v 3.3 1995/11/21 07:30:44 lpereira Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/list.h,v $
 *------------------------------------------------------------------
 * List toolkit definitions
 *
 * February 1995 Scott Mackie
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: list.h,v $
 * Revision 3.3  1995/11/21  07:30:44  lpereira
 * CSCdi44255:  APPN build is broken with the PortReady changes
 * Removed the forward reference to list_header since it is now
 * in types.h.
 *
 * Revision 3.2  1995/11/17  18:48:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:46:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:18:57  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/08/18  02:57:09  ahh
 * CSCdi38935:  TCP: TCP Listener should use new process primitives
 * Add LIST_ELEMENT_QUEUED() for determining whether an element
 * is on a queue.
 *
 * Revision 2.1  1995/06/07  21:56:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __LIST_H__
#define __LIST_H__


/*
 * Structures
 *
 * List toolkit structures
 */

STRUCTURE_REF(list_element);

struct list_element_ {
    list_element  *next;
    list_element  *prev;
    void          *data;
    list_header   *list;
};

typedef int (*list_insert_func_t)(void *, void *);
typedef int (*list_lookup_func_t)(void *, void *);

typedef boolean (*list_enqueue_t)(list_header *,
				  list_element *,
				  void *);
typedef void * (*list_dequeue_t)(list_header *);
typedef boolean (*list_requeue_t)(list_header *,
                                  list_element *,
                                  void *);
typedef boolean (*list_insert_t)(list_header *,
				 list_element *,
				 void *,
				 list_insert_func_t func); 
typedef void * (*list_remove_t)(list_header *,
				list_element *,
				void *); 
typedef boolean (*list_info_t)(list_header *,
			       list_element *,
			       char *); 
typedef void * (*list_lookup_t)(list_header *,
				 list_element *,
				 void *,
				 list_lookup_func_t func); 

typedef struct list_action_ {
    list_enqueue_t     enqueue;
    list_dequeue_t     dequeue;
    list_requeue_t     requeue;
    list_insert_t      insert;
    list_remove_t      remove;
    list_lookup_t      lookup;
} list_action_t;

struct list_header_ {
    list_element        *head;
    list_element        *tail;
    list_element         element;
    ushort               ID;
    ushort               flags;
    ushort               count;
    ushort               maximum;
    uchar               *name;
    list_info_t          info;
    list_action_t       *action;
    void                *context;
};


/*
 * Defines
 *
 * List toolkit definitions
 */

/*
 * Size of the biggest info textual field
 */
#define LIST_MAX_INFO    80

/*
 * Flag defines for list
 */
#define LIST_FLAGS_VALID          0x0001
#define LIST_FLAGS_MALLOC         0x0002
#define LIST_FLAGS_AUTOMATIC      0x0004
#define LIST_FLAGS_INTERRUPT_SAFE 0x0008

#define LIST_FLAGS_USER_MASK     (LIST_FLAGS_AUTOMATIC | \
				  LIST_FLAGS_INTERRUPT_SAFE)

/*
 * List status macros
 */
#define LIST_VALID(list) ((list)->flags & LIST_FLAGS_VALID)
#define LIST_SIZE(list)  ((list)->count)
#define	LIST_FULL(list)	 (((list)->maximum != 0) && \
                          ((list)->count >= (list)->maximum))
#define LIST_EMPTY(list) ((list)->head == NULL)

/*
 * List element manipulation macros
 */
#define LIST_NEXT_ELEMENT(element) ((element) ? (element)->next : NULL)
#define LIST_PREV_ELEMENT(element) ((element) ? (element)->prev : NULL)
#define LIST_HEAD_ELEMENT(list)    (list)->head
#define LIST_TAIL_ELEMENT(list)    (list)->tail

/*
 * Element status macros
 */
#define LIST_ELEMENT_QUEUED(element) ((element)->list != NULL)

#define FOR_ALL_ELEMENTS_IN_LIST(__list, __element) \
  for (__element  = LIST_HEAD_ELEMENT(__list);      \
       __element != NULL;                           \
       __element  = LIST_NEXT_ELEMENT(__element))

/*
 * List data manipulation macros
 */
#define LIST_GET_DATA(element) ((element) ? (element)->data : NULL)
#define LIST_SET_DATA(element) (element)->data

#define LIST_NEXT_DATA(element)      \
    (LIST_NEXT_ELEMENT(element)) ? LIST_NEXT_ELEMENT(element)->data : NULL
#define LIST_HEAD_DATA(list)      \
    (LIST_HEAD_ELEMENT(list)) ? LIST_HEAD_ELEMENT(list)->data : NULL
#define LIST_TAIL_DATA(list)      \
    (LIST_TAIL_ELEMENT(list)) ? LIST_TAIL_ELEMENT(list)->data : NULL

#define ELEMENT_GET_LIST(element) ((element)->list)

#define FOR_ALL_DATA_IN_LIST(__list, __element, __data) \
  for (__element = LIST_HEAD_ELEMENT(__list),           \
       __data = LIST_GET_DATA(__element);               \
       __element != NULL;                               \
       __element = LIST_NEXT_ELEMENT(__element),        \
       __data = LIST_GET_DATA(__element))

#define FOR_ALL_DATA_IN_LIST_W_NEXT(__list, __element, __nextelem, __data) \
  for (__element = LIST_HEAD_ELEMENT(__list),           \
       __data = LIST_GET_DATA(__element),               \
       __nextelem = LIST_NEXT_ELEMENT(__element);  \
       __element != NULL;                               \
       __element = __nextelem,                          \
       __nextelem = LIST_NEXT_ELEMENT(__element),       \
       __data = LIST_GET_DATA(__element))

#define FOR_ALL_DATA_IN_LIST_LOCKED(__list, __element, __nextelem, __data) \
  for (__data = LIST_HEAD_DATA(__list),                 \
       __element = LIST_HEAD_ELEMENT(__list);           \
       __element != NULL;                               \
       __element = __nextelem,                          \
       __data = LIST_GET_DATA(__element))

/*
 * List toolkit inline wrappers for function vectors
 */

static inline boolean list_enqueue (list_header    *list,
				    list_element   *element,
				    void             *data)
{
    return((*list->action->enqueue)(list, element, data));
}

static inline void *list_dequeue (list_header *list)
{
    return (*list->action->dequeue)(list);
}

static inline boolean list_requeue (list_header    *list,
				    list_element   *element,
				    void           *data)
{
    return ((*list->action->requeue)(list, element, data));
}

static inline boolean list_insert (list_header    *list,
				   list_element   *element,
				   void           *data,
				   list_insert_func_t func)
{
    return ((*list->action->insert)(list, element, data, func));
}

static inline void *list_remove (list_header   *list,
				 list_element  *element,
				 void          *data)
{
    return ((*list->action->remove)(list, element, data));
}

static inline boolean list_info (list_header    *list,
				 list_element   *element,
				 char           *buffer)
{
    if (list->info)
	return ((*list->info)(list, element, buffer));
    else
	return(FALSE);
}
static inline void *list_lookup (list_header    *list,
				   list_element   *element,
				   void           *data,
				   list_lookup_func_t func)
{
    return ((*list->action->lookup)(list, element, data, func));
}


/*
 * Prototypes
 */

extern void list_init(void);
extern void list_memory_init(void);
extern void list_move(list_header *new_list,
		      list_element *element);
extern void list_insert_element(list_header *list,
				list_element *previous,
				list_element *element);
extern void list_remove_element(list_header *list,
				list_element *element);
extern list_header *list_create(list_header *list,
				ushort maximum,
				char *const name, ushort flags);
extern void list_destroy(list_header *list);
extern void *list_lookup(list_header *list, 
			list_element *element,
			void *data,
			list_lookup_func_t func);
extern void list_set_automatic(list_header *list, boolean enabled);
extern void list_set_interrupt_safe(list_header *list, boolean enabled);
extern boolean list_set_info(list_header *list, list_info_t info);
extern list_info_t list_get_info(list_header *list);
extern boolean list_set_action(list_header *list, list_action_t *action);
extern list_action_t *list_get_action(list_header *list);
extern void list_display(parseinfo *csb);
extern void list_display_summary(void);
extern void list_display_detail(int listID);
extern void list_excise(list_header *list, void *corrupted);

#endif
