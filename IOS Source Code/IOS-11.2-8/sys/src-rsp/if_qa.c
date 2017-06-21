/* $Id: if_qa.c,v 3.3 1996/01/22 07:15:46 mdb Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/src-rsp/if_qa.c,v $
 *------------------------------------------------------------------
 * Queue ASIC interface routines for IRSP
 *
 * April 1994, Chris Shaker
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_qa.c,v $
 * Revision 3.3  1996/01/22  07:15:46  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  18:46:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:56:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "rsp_bufhdr.h"
#include "rsp_qasic.h"
#include "rsp_if.h"
#include "qa_hw.h"
#include "rsp_fast.h"
#include "rsp_pcmap.h"
#include "logger.h"
#include "../src-rsp/msg_rsp.c"		/* Not a typo, see logger.h */


/********************* HWQ support *********************/

/*
 * queue_ptr
 * Return a pointer to the queue at the specified queue index
 */

qa_hwq_t *queue_ptr (int index)
{
    return((qa_hwq_t *)&qasic_base->qa[index].hwq_prim);
}

/*
 * acc_ptr
 * Return a pointer to the accumulator at the specified accumulator index
 */

qa_acc_t *acc_ptr (int index)
{
    return((qa_acc_t *)&qasic_base->qa[index].acc_prim);
}

/*
 * hwq_dequeue
 * Remove the next buffer header number from the head of the specified
 * hardware queue. Input argument is a pointer to the desired hardware
 * queue. Returns a 32 bit pointer to the dequeued buffer header.
 */

bufhdr_t *hwq_dequeue (qa_hwq_t *hwq_ptr)
{
    return(hwq_dequeue_inline(hwq_ptr));
}




/*
 * hwq_enqueue
 * Add this buffer header number to the tail of the specified hardware queue.
 * Input arguments are a pointer to the desired hardware queue and a 32 bit
 * pointer to the buffer header to enqueue. Returns nothing.
 */

void hwq_enqueue (qa_hwq_t *hwq_ptr, bufhdr_t *bufptr)
{
    hwq_enqueue_inline(hwq_ptr, bufptr);
}




/*
 * hwq_peek
 * Read the buffer header number from the head of the specified hardware
 * queue without. The input argument is a pointer to the desired hardware
 * queue.
 * Returns a 32 bit pointer to the buffer header on the head of the queue:
 */

bufhdr_t *hwq_peek (qa_hwq_t *q_ptr)
{
    return(hwq_peek_inline(q_ptr));
}

/*
 * hwacc_read_dec
 * Read and decrement the specified hardware accumulator.
 * Input argument is a pointer to the desired hardware
 * accumulator. Returns the previous accumulator value.
 */

short hwacc_read_dec (qa_acc_t *acc_ptr)
{
    return(hwacc_read_dec_inline(acc_ptr));
}




/*
 * hwacc_peek
 * Read the specified hardware accumulator.
 * Input argument is a pointer to the desired accumulator to read.
 * Returns the previous accumulator value.
 */

short hwacc_peek (qa_acc_t *acc_ptr)
{
    return(hwacc_peek_inline(acc_ptr));
}




/*
 * hwacc_sub
 * Subtract the supplied value from the specified hardware accumulator
 * Input arguments are a pointer to the accumulator and the value to
 * subtract.
 * Returns nothing.
 */

void hwacc_sub (qa_acc_t *acc_ptr, short value)
{
    hwacc_sub_inline(acc_ptr, value);
}





/*
 * hwacc_set
 * Set the specified accumulator to the supplied value.
 * Returns nothing.
 */

void hwacc_set (qa_acc_t *acc_ptr, short value)
{
    hwacc_set_inline(acc_ptr, value);
}


/********************* QA Diag space support *********************/

/*
 * rsp_disable_cbuses
 * Disable the CyBuses by disabling bus grant in both of the
 * CBUS 0 and CBUS 1 registers. Also disable read bypass on the memd
 * write buffer.
 * Return the concatenation of the previous CBUS 0 and CBUS 1 register
 * data and the state of the MEMD read bypass write bit
 * NESTABLE
 */

rsp_cbus_state_t rsp_disable_cbuses (void)
{
    volatile uchar *cbus0_ptr, *cbus1_ptr;
    ushort dclreg;
    volatile ushort *dclptr;
    leveltype level;
    rsp_cbus_state_t ret;

    cbus0_ptr = (uchar *) ADRSPC_CBUS0REG;
    cbus1_ptr = (uchar *) ADRSPC_CBUS1REG;
    dclptr = (ushort*) ADRSPC_DCL;

    /*
     * Take away grant enable on both buses:
     */
    level = raise_interrupt_level(ALL_DISABLE);

    ret.cbus0 = *cbus0_ptr;
    if (ret.cbus0 & CBUSX_GRANT_ENABLE)
	*cbus0_ptr = ret.cbus0 & ~CBUSX_GRANT_ENABLE;

    ret.cbus1 = *cbus1_ptr;
    if (ret.cbus1 & CBUSX_GRANT_ENABLE)
	*cbus1_ptr = ret.cbus1 & ~CBUSX_GRANT_ENABLE;

    /*
     * Take away read bypass on the MEMD write buffer:
     */
    dclreg = *dclptr;
    if (dclreg & DCL_MEMD_RDBYPASS) {
	ret.memd_rdbypass = 1;
	*dclptr = (dclreg & ~DCL_MEMD_RDBYPASS);
    } else {
	ret.memd_rdbypass = 0;
    }

    /*
     * Ensure that the write buffers are flushed by reading from
     * the last register that we wrote
     */
    dclreg = *dclptr;

    reset_interrupt_level(level);

    return ret;
}




/*
 * rsp_restore_cbuses
 * Restore the bus grant state of the CyBuses via the CBUS 0
 * and CBUS 1 registers, based on the previous bus state.
 * Inputs the concatenation of the previous CBUS 0 and CBUS 1 register
 * data and the state of the MEMD read bypass write bit
 * NESTABLE
 */

void rsp_restore_cbuses (rsp_cbus_state_t old)
{
    volatile uchar *cbus0_ptr, *cbus1_ptr;
    volatile ushort *dclptr;
    leveltype level;

    cbus0_ptr = (uchar *) ADRSPC_CBUS0REG;
    cbus1_ptr = (uchar *) ADRSPC_CBUS1REG;
    dclptr = (ushort*) ADRSPC_DCL;

    /*
     * Restore bus grant state on both CyBuses:
     */

    level = raise_interrupt_level(ALL_DISABLE);

    /*
     * If bus grant was previously enabled on CBUS 0,
     * reenable it, else disable it:
     */
    if (old.cbus0 & CBUSX_GRANT_ENABLE) {
	*cbus0_ptr |= CBUSX_GRANT_ENABLE;
    } else {
	*cbus0_ptr &= ~CBUSX_GRANT_ENABLE;
    }

    /*
     * If bus grant was previously enabled on CBUS 1,
     * reenable it, else, disable it:
     */
    if (old.cbus1 & CBUSX_GRANT_ENABLE) {
	*cbus1_ptr |= CBUSX_GRANT_ENABLE;
    } else {
	*cbus1_ptr &= ~CBUSX_GRANT_ENABLE;
    }

    /*
     * If read bypass was previously enabled, reenable it, else disable it:
     */
    if (old.memd_rdbypass)
	*dclptr |= DCL_MEMD_RDBYPASS;
    else
	*dclptr &= ~DCL_MEMD_RDBYPASS;

    reset_interrupt_level(level);

    return;
}

/*
 * Insure that the MEMD write buffers in the dcl/dpu are completely flushed 
 *  *** ASSUMES MEMD read-around-write has been disabled ***
 */
static inline 
void flush_memd_write_buffers (void)
{
	/*
	 * Do a read to any address in memd space.
	 *  Avoid QA addresses since we are trying here to disable
	 *   non-diagnostic QA accesses.
	 *  pick something uncached: base of buffer headers.
	 *  note bufhdr_base is not valid during rsp_qa_init();
	 */
    register void *ptr = (void *)ADDR_MEMD_TO_UNCACHED(ADRSPC_MEMD_BUFHDR);

    asm volatile(".set noreorder");

    asm volatile ("lhu $0,0(%0)" :
	/* no outputs */ : "r" (ptr));

    asm volatile(".set reorder");
}

/*
 * delay for 3 QA cycles
 *  on RSP1 that is 3 x 30ns
 *  on RSP2 that is 3 x 20ns
 *  (delay for worst case)
 */
static inline 
void delay_3_QA_cycles (void)
{
        asm volatile(".set noreorder");
 
        asm volatile(" nop");
        asm volatile(" nop");
        asm volatile(" nop");

        asm volatile(" nop");
        asm volatile(" nop");
        asm volatile(" nop");

        asm volatile(" nop");
        asm volatile(" nop");
        asm volatile(" nop");

        asm volatile(".set reorder");
}

/*
 * Read QA diag value
 * Two consecutive reads must be done to return one 32 bit word of data
 * from the QA Diag space.
 *
 * From the QA spec:
 *
 *   it is a requirement that the cycle following a diagnostic operation
 *   must be a dead cycle, and that a non-diagnostic access must
 *   precede a diagnostic access by at least three cycles.
 *
 *   Note - these are QA cycles: 30ns on RSP1 and 20ns on RSP2
 *   Also the RSP can only access the QA once every 60ns (cbus cycle)
 *
 * for this reason network interrupts and cbus activity should be masked
 *  while reading or writing diag space
 */

static ulong get_qa_diag_value (volatile ulong *qa_diag_ptr)
{
	ulong			value;

	flush_memd_write_buffers();
 
	delay_3_QA_cycles();

	value = *qa_diag_ptr;		/* First read starts hardware */

	/*
	 * One QA dead cycle required here
	 * is guaranteed by the fact that RSP can only read the
	 * QA diags once per 60ns
	 */

	value = *qa_diag_ptr;		/* Second read gets read data */

	/*
	 * One QA dead cycle required here
	 * is guaranteed by the fact that RSP can only read the
	 * QA diags once per 60ns and by the fact that we cannot
	 * reenable the cybuses in the next 30ns
	 */

	return(value);
}

/*
 * Read the 32 bit word from QA Diag space at the specified location
 * caller *must* block interrupts and cbus accesses
 */
ulong read_qa_diag_unprotected (volatile ulong *qa_diag_ptr)
{
	ulong v1;
	ulong v2, v3;
	int i;


	/*
	 * kludge alert!
	 *  due to QA Diag HW read flakiness 
	 *  we read till we get 3 equal values in a row.
	 *  This may be overkill but it works 
	 *  See CSCdi46980.
	 */
	v1 = get_qa_diag_value(qa_diag_ptr);

	for(i = 10; ; --i) {

		if (!i) {
                        errmsg(&msgsym(QADIAG, RSP), "read", qa_diag_ptr);
			break;
		}
		v2 = get_qa_diag_value(qa_diag_ptr);
		if (v1 == v2) {
		   v3 = get_qa_diag_value(qa_diag_ptr);
		   if (v1 == v3) {
		       break;
		   } else {
		      v1 = v3;
		   }
		} else {
		    v1 = v2;
		}
	}
	return(v1);
}

static boolean qa_diag_ptr_valid (volatile ulong *qa_diag_ptr)
{
	uint			start, end, ptr;
	/*
	 * Do range check on pointer argument:
	 */
	ptr = (uint) qa_diag_ptr;
        start = (uint) qasic_diag_base;
        end = start + sizeof(qa_diag_t);
        if ((ptr < start) || (ptr >= end)) {
            errmsg(&msgsym(QADIAG, RSP), "address", qa_diag_ptr);
            return(FALSE);
        }
        return(TRUE);
}

/*
 * Read the 32 bit word from QA Diag space at the specified location:
 */
ulong read_qa_diag (volatile ulong *qa_diag_ptr)
{
        ulong                   value;
        rsp_cbus_state_t        old_cbus_state;
        leveltype               level;

        if (!qa_diag_ptr_valid(qa_diag_ptr))
          return(0L);

        /*
         * We must save the state of the CyBus, and stop the Cybus before
         * accessing QA diag space. When we have done our access, we must
         * restore the state of the CyBus.
         */
        level = raise_interrupt_level(ALL_DISABLE);

        old_cbus_state = rsp_disable_cbuses();

        value = read_qa_diag_unprotected(qa_diag_ptr);

        rsp_restore_cbuses(old_cbus_state);

	reset_interrupt_level(level);

	return(value);
}

/*
 * Write the supplied 32 bit data into QA Diag space at the specified location
 * caller *must* block interrupts and cbus accesses
 */

void write_qa_diag_unprotected (volatile ulong *qa_diag_ptr, ulong value)
{
	flush_memd_write_buffers();

	delay_3_QA_cycles();

	*qa_diag_ptr = value;

	flush_memd_write_buffers();
	/*
	 * One QA dead cycle required here 
	 * is guaranteed by the fact that RSP can only read the
	 * QA diags once per 60ns and by the fact that we cannot
	 * reenable the cybuses in the next 30ns
	 */
}


/*
 * write_qa_diag
 * Write the supplied 32 bit data into QA Diag space at the specified location
 *
 * We must save the state of the CyBus, and stop the Cybus before
 * accessing QA diag space. When we have done our access, we must
 * restore the state of the CyBus.
 */

void write_qa_diag (volatile ulong *qa_diag_ptr, ulong value)
{
	rsp_cbus_state_t	old_cbus_state;
	leveltype		level;

        if (!qa_diag_ptr_valid(qa_diag_ptr))
	    return;

	/*
	 * We must save the state of the CyBus, and stop the Cybus before
	 * accessing QA diag space. When we have done our access, we must
	 * restore the state of the CyBus.
	 */
	level = raise_interrupt_level(ALL_DISABLE);

	old_cbus_state = rsp_disable_cbuses();

        write_qa_diag_unprotected(qa_diag_ptr, value);

	rsp_restore_cbuses(old_cbus_state);

	reset_interrupt_level(level);
}

/*
 * Clear QA Errors - Assumes cbus and interrupts disabled
 */
static inline
void qa_clear_error_unprotected (void)
{
	write_qa_diag_unprotected(&qasic_diag_base->control,1);
}


/*
 * Initialize the QA ASIC:
 */

void
rsp_qa_init (void)
{
    uchar		memd_config, memd_diag, memd_page0_7, memd_page8_15;
    volatile ulong	*lptr, *eptr;
    volatile uchar	*reg;
    rsp_cbus_state_t	old_cbus_state;
    leveltype		level;

    /*
     * We must save the state of the CyBus, and stop the Cybus before
     * accessing QA diag space. When we have done our access, we must
     * restore the state of the CyBus.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    old_cbus_state = rsp_disable_cbuses();

    /*
     * Reset the QA & MEMD ASICs
     *
     * we reset the MEMD ASIC because the QA interrupt counters
     * are there, but unfortunately we have to save some registers
     * across the reset.
     */
    reg = (uchar*) ADRSPC_MEMD_CONFIG;	memd_config	= *reg;
    reg = (uchar*) ADRSPC_PAGESIZE0_7;	memd_page0_7	= *reg;
    reg = (uchar*) ADRSPC_PAGESIZE8_15;	memd_page8_15	= *reg;
    reg = (uchar*) ADRSPC_MEMD_DIAG;	memd_diag	= *reg;

    reg = (uchar*) ADRSPC_RESET_PERIPH;
    *reg = ACTIVE_LOW(PERIPH_QA_ASIC | PERIPH_MEMD_ASIC);
    *reg;			/* flush write buffer */
    usecdelay(1);		/* delay 80ns to set-up reset, plus
				 * 80ns for all peripheral resets, except
				 * PCMCIA needs 500ns to reset
				 */

    reg = (uchar*) ADRSPC_MEMD_CONFIG;	*reg = memd_config;
    reg = (uchar*) ADRSPC_PAGESIZE0_7;	*reg = memd_page0_7;
    reg = (uchar*) ADRSPC_PAGESIZE8_15;	*reg = memd_page8_15;
    reg = (uchar*) ADRSPC_MEMD_DIAG;	*reg = memd_diag;


    /*
     * Clear entire Diag space contents, doing 64 bit writes.
     * Writes have to be done on 64 bit boundary.
     */
    lptr = (ulong *) qasic_diag_base;
    eptr = (ulong *) ((uint)lptr + QA_DIAGSIZE);
    flush_memd_write_buffers();
    delay_3_QA_cycles();
    while (lptr < eptr) {
	*lptr++ = 0L;		/* Write 32 bits on 64 bit boundary */
        /*
	 * One QA dead cycle required here 
	 * is guaranteed by the fact that RSP can only read the
	 * QA diags once per 60ns
	 */
	lptr++;
    }
    /*
     * Write buffer base address which is added to the buffer
     * header pointer output by the QA ASIC:
     */
    qasic_diag_base->base_addr = (ulong) bufhdr_base;

    /*
     * Clear any existing QA Error
     * and insure that all writes are flushed out to the QA.
     */
    qa_clear_error_unprotected();

    rsp_restore_cbuses(old_cbus_state);
    reset_interrupt_level(level);
    return;
}

/*
 * make an *atomic* copy of entire diag space
 * Note this will stall the Cybus for a *long* time.
 */
void rsp_save_qadiag (qa_diag_t	*qasic_diag_copy)
{
    rsp_cbus_state_t	old_cbus_state;
    leveltype		level;
    volatile ulong      *src;
    ulong	        *dst;

    src = (ulong*) qasic_diag_base;
    dst = (ulong*) qasic_diag_copy;

    /*
     * Save the state of the CyBus, and stop the Cybus before
     * accessing QA diag space.
     */
    level = raise_interrupt_level(ALL_DISABLE);
    old_cbus_state = rsp_disable_cbuses();

    while (src < (ulong*) &qasic_diag_base[1]) {

	*dst = read_qa_diag_unprotected(src);
	src += 2;
	dst += 2;
    }
    rsp_restore_cbuses(old_cbus_state);
    reset_interrupt_level(level);
    return;
}

