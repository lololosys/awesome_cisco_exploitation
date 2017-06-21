/* $Id: qa_mgr.c,v 3.3.4.4 1996/07/08 22:52:25 mwu Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/qa_mgr.c,v $
 *------------------------------------------------------------------
 * Hardware Queue and Accumulator allocation/deallocation
 * interface routines for IRSP
 *
 * April 1994, Chris Shaker
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: qa_mgr.c,v $
 * Revision 3.3.4.4  1996/07/08  22:52:25  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.3.4.3  1996/04/06  22:16:22  ondiaye
 * CSCdi52397:  fixed a minor bug: Changed && from & in qa_magr.c
 *                 (next_free_Q). ondiaye
 *
 * Branch: California_branch
 *
 * Revision 3.3.4.2  1996/04/06  08:43:51  ondiaye
 * CSCdi49252:  VIPER needs 16 tx queues in one status word: This is fixed
 *              by breaking the QAasic into two pools of txq and
 *              accumulators
 *              (poo1 and pool2). VIP2 txq's get allocated from pool2.
 *              VIP2
 *              txq's need to be continuous and their accumulators must at
 *              2 byte offset from their queues.
 *              This is temporary in a sense that it won't work well for
 *              the
 *              master/slave rsp model. Marty is working on that version.
 * Branch: California_branch
 *
 * Revision 3.3.4.1  1996/03/21  23:49:37  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/03/18  23:34:28  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.1  1996/02/17  00:04:06  getchell
 * CSCdi49252:  VIPER needs 16 tx queues in one status word
 * These mods constitute a temporary hack to allow a 16E's transmit queues
 * to be represented by one RSP QA status word.  Oumar is going to "do
 * this right" as soon as he finishes up QE ucode development.
 * Branch: ELC_branch
 *
 * Revision 3.3  1996/03/11  11:09:23  ogrady
 * CSCdi51224:  RSP emulation code is obsolete
 *              Expunge code.
 *
 * Revision 3.2  1995/11/17  18:46:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:57:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "rsp_bufhdr.h"
#include "rsp_qasic.h"
#include "qa_hw.h"
#include "rsp_if.h"
#include "logger.h"
#include "../hes/cbus_debug.h"

/*
 * Private queue definitions:
 */
enum {
    RAW_Q_INDEX = SLOT2CMDQ(16), /* start allocating after last cmdq (15) */
    RETURN_Q_INDEX,
    EVENT_Q_INDEX,
    FIRST_FREE_Q_INDEX,
};

static int      pool1_next_free_Q_index;
static int      pool2_next_free_Q_index;
static int	next_free_ACC_index;
static int	base_skipped_Q;            /* all svip txQ's must be in the same status reg */
static int	num_skipped_Q;             /* this allows us to recycle the skipped Q's */
static int      skipped_Q[QA_LAST_QNUM];

static queuetype	element_Q, acc_Q;

typedef struct acc_q_t_ {
  struct acc_q_t_	*next;
  qa_acc_t		*acc_addr;
} acc_q_t;

static acc_q_t acc_q_pool[QA_ACCNUM_LIMIT];

/*
 * Initialize the queue and accumulator allocators:
 */

void qa_mgr_init (void)
{
    int ii;
    acc_q_t *q_pool_ptr = acc_q_pool;
  
    queue_init(&acc_Q, 0);
    queue_init(&element_Q, 0);
    for (ii = 0; ii < QA_ACCNUM_LIMIT ; ii++) {
      q_pool_ptr->next = NULL;
      enqueue(&element_Q, (void *) q_pool_ptr++);
    }
    pool1_next_free_Q_index = FIRST_FREE_Q_INDEX;
    pool2_next_free_Q_index = POOL2_FIRST_TXQNUM;
    next_free_ACC_index = QA_FIRST_ACCNUM;
    base_skipped_Q = 0;
    num_skipped_Q = 0;
}


static int next_free_Q (void)
{
    if (pool1_next_free_Q_index <= POOL1_LAST_TXQNUM)
        return (pool1_next_free_Q_index++); /* if in 1st pool just return the next free index */
    if (num_skipped_Q && (base_skipped_Q < POOL2_LAST_TXQNUM)) { 
        num_skipped_Q--;           /* recycle those skipped buffers */
	return (skipped_Q[base_skipped_Q++]);
    }
    if (pool2_next_free_Q_index >= POOL2_LAST_TXQNUM)
      	return(QA_ALLOC_ERROR);
    else
	return(pool2_next_free_Q_index++);
}

/*
 * Reserve arg num of hwq's such that all arg hwq's will share the same 
 * status register. This is currently used for the svip txq allocation.
 */
int reserve_txhwq (int arg)
{
    int index, i;
    int skipped_q_base_index;
    int skipped_q_num_index;
    int temp;

    index = QA_ALLOC_ERROR;
    skipped_q_base_index = pool2_next_free_Q_index;
    skipped_q_num_index = 0;
    temp = ((pool2_next_free_Q_index + 15) & ~15);
    
    if (arg > 16) 
        return (QA_ALLOC_ERROR);
    if ((temp - pool2_next_free_Q_index) >= arg) { 
        pool2_next_free_Q_index += arg;
	return (skipped_q_base_index);
    }
    if ((temp + arg - 1) <= QA_LAST_QNUM) {
        pool2_next_free_Q_index = temp + arg;
	skipped_q_num_index = temp - skipped_q_base_index;
        for (i=0; i < skipped_q_num_index; i++){
	    skipped_Q[base_skipped_Q + i]= skipped_q_base_index + i;
	    num_skipped_Q++;
	}
	return (temp);
    }
    return (index);    
}



/*
 * get_hwq - Allocate a queue of the specified type
 *
 * Queue types:
 *  QT_CMD   - Command Queue. The second argument is the chassis slot number
 *          so that we can return the appropriate command queue.
 *  QT_LFREE - Local Free Queue. The second argument is not used.
 *  QT_GFREE - Global Free Queue. The second argument is not used.
 *  QT_TX    - Transmit Free Queue. The second argument specifies the number
 *          of contiguous queues which should be allocated. We will allocate
 *          the requested number of hardware queues such that the queue status
 *	    bits will be in the same queue status word(s). This feature is
 *          intended to be used for priority queuing implemented with hardware
 *          queues. THIS FEATURE IS NOT YET IMPLEMENTED. NUMBERS LARGER THAN
 *          ONE WILL RESULT IN AN ERROR BEING RETURNED.
 * QT_RAW    - Raw Queue. The second argument is not used.
 * QT_RETURN - Return Queue. The second argument is not used.
 * QT_EVENT  - Event Queue. The second argument is not used.
 *
 * Returns the index of the first allocated hardware queue.
 * In case of an error, QA_ALLOC_ERROR is returned.
 *
 * For the hacked SP hardware queue/accumulator simulation,
 * this routine will communicate with the emulation layer to
 * inform it of which type each allocated queue is.
 *
 * Currently, this routine uses a simple queue allocation strategy
 * that does not support the freeing of queues.
 */

int get_hwq (queue_t type, int arg)
{
    int index;

    index = QA_ALLOC_ERROR;

    switch (type) {

    case QT_CMD:
	/*
	 * Allocate the correct command queue index for this slot number:
	 */
	if ((arg >= MAX_SLOTS) || (arg < 0)){
	    index = QA_ALLOC_ERROR;	/* Return error if bad slot number */
	} else {
	    index = SLOT2CMDQ(arg);
	}
	break;

    case QT_LFREE:
	/*
	 * Allocate a local free queue pointer such that we can quickly
	 * convert from a local free queue pointer to a hardware IDB pointer:
	 */
	index = next_free_Q();
	if ((index << LOCAL_QUEUE_SHIFT) & ~LOCAL_QUEUE_MASK)
	    index = QA_ALLOC_ERROR;
	break;

    case QT_GFREE:
	/*
	 * Allocate a global free queue pointer:
	 */
	index = next_free_Q();
	break;

    case QT_TX:
	/*
	 * Allocate the specified number of contiguous transmit queues.
	 * Return a pointer to the first allocated queue.
	 */
	if (arg > 1) {
	    index = QA_ALLOC_ERROR;	/* Return error if more than one
					   queue requested */
	} else {
	    index = next_free_Q();
	}
	break;

    case QT_RAW:
	/*
	 * Allocate the Raw queue:
	 */
	index = RAW_Q_INDEX;
	break;

    case QT_RETURN:
	/*
	 * Allocate the Return queue:
	 */
	index = RETURN_Q_INDEX;
	break;

    case QT_EVENT:
	/*
	 * Allocate the event queue:
	 */
	index = EVENT_Q_INDEX;
	break;
    }


    return(index);
}

/*
 * Allocate a hardware accumulator
 */

qa_acc_t *get_hwacc ()
{
    acc_q_t *q_ptr;
    int free;

   /* Only assign acc's from the first pool */
    free = next_free_ACC_index;
    if (POOL2_FIRST_TXQNUM == free) {
        /* 
	 * Try the recycle bin
	 */
        if (QUEUEEMPTY(&acc_Q) == FALSE) {
	    q_ptr = (acc_q_t *) dequeue(&acc_Q);
	    if (cbus_debug) {
	        buginf("\nget_hwacc %x", q_ptr->acc_addr);
	    }
	    q_ptr->next = NULL;
	    enqueue(&element_Q, (void *) q_ptr);
	    return (q_ptr->acc_addr);
	} 
	if (cbus_debug) {
	    buginf("QA: ran out accumulator");
	}

	return (NULL);
    }

    next_free_ACC_index++;
    if (cbus_debug) {
        buginf("\nget_hwacc %x index %d", &qasic_base->qa[free].acc_prim, free);
    }
    return((qa_acc_t *) &qasic_base->qa[free].acc_prim);
}

/*
 * Return a hardware accumulator
 */

void ret_hwacc (qa_acc_t *acc_ptr)
{
    acc_q_t *q_ptr;

    if (QUEUEEMPTY(&element_Q) == FALSE) {
        q_ptr = (acc_q_t *) dequeue(&element_Q);
	q_ptr->acc_addr = acc_ptr;
	q_ptr->next = NULL;
	enqueue(&acc_Q, (void *) q_ptr);
	if (cbus_debug) {
	    buginf("\nret_hwacc %x", acc_ptr);
	}
    }
    else
        if (cbus_debug) {
	    buginf("QA: ran out acc element"); 
	}

}
