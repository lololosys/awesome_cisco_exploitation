/* $Id: termqueue.h,v 3.2 1995/11/17 09:13:43 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/h/termqueue.h,v $
 *------------------------------------------------------------------
 * termqueue.h -- Definitions for inbound port queueing
 *
 * 20-Jun-1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: termqueue.h,v $
 * Revision 3.2  1995/11/17  09:13:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:36:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/14  07:02:28  hampton
 * Convert Terminal Server to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi37093]
 *
 * Revision 2.1  1995/06/07 20:36:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TERMQUEUE_H__
#define __TERMQUEUE_H__

typedef struct queuevector_ {
    void (*vshow)(void *);			/* Show this queue entry */
    void (*vdelete)(void *);			/* Delete this queue entry */
    boolean (*vstart)(void *);		/* Attempt to start this entry */
} queuevector;

typedef struct termqueue_ {
    struct termqueue_ *next;		/* Link to next block on queue */
    sys_timestamp time;				/* Time entered in queue */
    ushort id;				/* Entry ID number */
    void *handle;			/* Queue-specfic data */
    queuevector *v;			/* Function vector */
} termqueue;

/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/
extern termqueue *termqueue_queuehead;
extern termqueue *termqueue_queuetail;


#define TERMQUEUE_INTERVAL 60
/*
 * ../ts/termqueue.c
 */
extern termqueue *termqueue_enqueue(void *, queuevector *);
extern void termqueue_check(void);
extern void termqueue_delete(termqueue *);
extern void termqueue_init(void);
extern void termqueue_periodic(void);
extern void termqueue_remove(void *);

/*
 * ../ts/termqueue_chain.c
 */
extern void termqueue_parser_init(void);

#endif __TERMQUEUE_H__
