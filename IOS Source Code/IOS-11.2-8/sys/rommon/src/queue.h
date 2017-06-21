/* $Id: queue.h,v 3.5 1996/02/09 00:01:49 tkam Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/queue.h,v $
 *------------------------------------------------------------------
 * queue.h
 *
 * Oct, 95, Steve J. Zhang
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: queue.h,v $
 * Revision 3.5  1996/02/09  00:01:49  tkam
 * CSCdi47873:  HSA ROMMON needs to add flow control to dbus download
 * Add download flow control to HSA ROMMON.
 *
 * Revision 3.4  1996/02/01  02:27:13  mansonw
 * CSCdi47569:  HSA slave refuses to switch to master state
 * Fixed the HSA slave to gain master during hot insertion/removal
 *
 * Revision 3.3  1996/01/22  09:06:47  mdb
 * CSCdi47263:  change definition of NULL to ((void *) 0) from 0
 *
 * Revision 3.2  1995/11/17  18:42:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:23:52  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  23:11:25  szhang
 * placeholder for DBUS handlers in RSP2
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __QUEUE_H__
#define __QUEUE_H__
#include "../../../sys/src-4k/signal_4k.h"
#define NULL ((void *) 0)
#define MAX_DBUS_CHARS          1024
#define MAX_DBUS_CHARS_MASK     (MAX_DBUS_CHARS-1)

typedef struct queue_ {
    volatile char tx_q[MAX_DBUS_CHARS];  /* Queue for transmit */
    volatile char rx_q[MAX_DBUS_CHARS];  /* Queue for Receive */
    uint tx_head;     /* Head of transmit character queue */
    uint tx_tail;     /* Tail of transmit character queue */
    uint rx_head;     /* Head of receive  character queue */
    uint rx_tail;     /* Tail of receive  character queue */
    volatile boolean stop_out;
    volatile rx_state;
} queue_t;

typedef struct vc_ {
    int (*getc)(void);
    void (*putc)(uchar);
    void (*flush)(void);
    void (*rom_general_exception)(r4k_context_t *frame);
    void (*rom_handle_interrupt)(r4k_context_t *frame, char *str);
    void (*rom_dcl_interrupt)(r4k_context_t *frame, uint intr_level);
    void (*rom_dbus_handler)(r4k_context_t *frame);
    long memsize; /* for passing memory size to dbus */
} vc_t;
    
#endif __QUEUE_H__


