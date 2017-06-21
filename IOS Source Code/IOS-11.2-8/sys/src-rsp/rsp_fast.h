/* $Id: rsp_fast.h,v 3.5.4.2 1996/06/04 01:48:38 shaker Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_fast.h,v $
 *------------------------------------------------------------------
 * Inline functions for fast access to QA ASIC for IRSP
 *
 * Chris Shaker, June, 1994
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_fast.h,v $
 * Revision 3.5.4.2  1996/06/04  01:48:38  shaker
 * CSCdi59403:  Better explain hwacc_read_dec_inline()
 * Branch: California_branch
 * Note that the hardware won't read & decrement below 0.
 *
 * Revision 3.5.4.1  1996/05/07  00:14:38  mbeesley
 * CSCdi55679:  Add IP flow/optimum switching engine
 * Branch: California_branch
 *
 * Revision 3.5  1996/03/11  11:09:30  ogrady
 * CSCdi51224:  RSP emulation code is obsolete
 *              Expunge code.
 *
 * Revision 3.4  1996/02/04  02:29:30  shaker
 * CSCdi47971:  RSP-3-NORESTART error message
 * Prevent this error from happening. Also enhance error logging
 * for some hard to reproduce error cases.
 *
 * Revision 3.3  1995/12/15  11:57:39  ogrady
 * CSCdi43520:  custom queueing locks up serial interface on 7513
 * Send buffers that restart interface output queues to the return queue
 * instead of the original free queue.  Add a few sanity checks and error
 * messages.
 *
 * Revision 3.2  1995/11/17  18:48:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/01  06:52:37  dkerr
 * CSCdi36127:  IP with options gets dropped as checksum error
 * Don't call flow/optimum switching code if IP has options.
 *
 * Revision 2.3  1995/07/27  22:16:01  schaefer
 * CSCdi37633: uninitialized free queue ptr in returned buffers
 *
 * Revision 2.2  1995/07/19  06:47:42  shaker
 * CSCdi36264:  fast and optimum switching dont prime returnq
 * Bring 11.0 code up to date with solution in 10.3 code.
 *
 * Revision 2.1  1995/06/07  22:57:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __RSP_FAST_H__
#define __RSP_FAST_H__

/*
 * hwq_dequeue_inline
 * Remove the next buffer header number from the head of the specified
 * hardware queue. Input argument is a pointer to the desired hardware
 * queue. Returns a 32 bit pointer to the dequeued buffer header.
 */

static inline
bufhdr_t *hwq_dequeue_inline (qa_hwq_t *hwq_ptr)
{
    return( (bufhdr_t *) (*( (qa_hwq_read_reg_t *) (&hwq_ptr->primary) ) ) );
}




/*
 * hwq_enqueue_inline
 * Add this buffer header number to the tail of the specified hardware queue.
 * Input arguments are a pointer to the desired hardware queue and a 32 bit
 * pointer to the buffer header to enqueue. Returns nothing.
 */

static inline
void hwq_enqueue_inline (qa_hwq_t *hwq_ptr, bufhdr_t *bufptr)
{
    hwq_ptr->primary = ADDR_TO_MEMD_OFFSET_16(bufptr);
    return;
}




/*
 * hwq_peek_inline
 * Read the buffer header number from the head of the specified hardware
 * queue without. The input argument is a pointer to the desired hardware
 * queue.
 * Returns a 32 bit pointer to the buffer header on the head of the queue:
 */

static inline
bufhdr_t *hwq_peek_inline (qa_hwq_t *q_ptr)
{
    return( (bufhdr_t *) (*( (qa_hwq_read_reg_t *) (&q_ptr->alternate) ) ) );
}

/*
 * hwacc_read_dec_inline
 * Read and decrement the specified hardware accumulator.
 * Input argument is a pointer to the desired hardware
 * accumulator.
 * Returns the value that was in the accumulator BEFORE it was decremented.
 *
 * Note that the hardware will not decrement the accumulator below 0
 * when doing a read and decrement!
 */

static inline
short hwacc_read_dec_inline (qa_acc_t *acc_ptr)
{
    return(acc_ptr->primary);
}




/*
 * hwacc_peek_inline
 * Read the specified hardware accumulator.
 * Input argument is a pointer to the desired accumulator to read.
 * Returns the previous accumulator value.
 */

static inline
short hwacc_peek_inline (qa_acc_t *acc_ptr)
{
    return(acc_ptr->alternate);
}




/*
 * hwacc_sub_inline
 * Subtract the supplied value from the specified hardware accumulator
 * Input arguments are a pointer to the accumulator and the value to
 * subtract.
 * Returns nothing.
 */

static inline
void hwacc_sub_inline (qa_acc_t *acc_ptr, short value)
{
    acc_ptr->primary = value;
}





/*
 * hwacc_set_inline
 * Set the specified accumulator to the supplied value.
 * Returns nothing.
 */

static inline
void hwacc_set_inline (qa_acc_t *acc_ptr, short value)
{
    acc_ptr->alternate = value;
}



static inline void copy_16_aligned_bytes_inline (void *src, void *dest)
{
    ulong data1, data2;

    asm volatile (".set mips3");
    asm volatile (".set noreorder");

    asm volatile ("ld %0, 0(%1);"
		  : "=&r" (data1) : "r" (src) );

    asm volatile ("ld %0, 8(%1);"
		  : "=&r" (data2) : "r" (src) );

    asm volatile ("sd %0, 0(%1);" 
		  : : "r" (data1), "r" (dest) );

    asm volatile ("sd %0, 8(%1);" 
		  : : "r" (data2), "r" (dest) );

    asm volatile (".set reorder");
    asm volatile (".set mips2");
}



#define hwacc_inc_inline(a)	hwacc_sub_inline(a, (-1))





/*
 * Returns a buffer header to the appropriate queue via the queue
 * pointer in the buffer header. Use this to free the input buffer
 * header when you are done with it.
 */

static inline void
retbufhdr_inline (bufhdr_t *bp)
{
    hwq_enqueue_inline(QASIC_OFFSET_TO_ADDR(bp->bf_queue_ptr), bp);
}



extern void rsp_tx_restart_via_returnq(hwidbtype *hwidb, bufhdr_t *bufptr);

extern paktype *fs_pak;
extern void stuff_fs_pak(bufhdr_t *bufptr, hwidbtype *input, paktype *pak);
#endif /* __RSP_FAST_H__ */


