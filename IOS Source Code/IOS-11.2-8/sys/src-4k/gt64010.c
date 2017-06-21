/* $Id: gt64010.c,v 3.1.68.6 1996/08/28 13:10:56 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-4k/gt64010.c,v $
 *------------------------------------------------------------------
 * gt64010.c - Galileo system control for R4600 systems
 *
 * October 1995, Michael Beesley
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: gt64010.c,v $
 * Revision 3.1.68.6  1996/08/28  13:10:56  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.68.5  1996/07/18  17:57:52  mbeesley
 * CSCdi63193:  Need support for rev2 gt64010. Teach the monlib and
 * the IOS system images the incompatible differences between rev1 and
 * rev2 gt64010 chips :
 *         o Addressing flip on bytes accesses thru PCI IO space
 *         o Internal PCI config register addressing change
 * Branch: California_branch
 *
 * Revision 3.1.68.4  1996/06/18  17:52:16  ssangiah
 * CSCdi59308:  PCMCIA removal/insertion crashes the system
 * Branch: California_branch
 *
 * Revision 3.1.68.3  1996/05/24  00:17:06  mbeesley
 * CSCdi57936:  GT64010 Errors should reload the box
 * Branch: California_branch
 *
 * Revision 3.1.68.2  1996/05/21  10:03:08  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.68.1  1996/03/21  23:46:34  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.12  1996/03/18  23:32:55  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.1.2.11  1996/02/19  23:24:08  mbeesley
 * CSCdi49335:  Predator needs support for DMA driven coalesce
 * Branch: ELC_branch
 *
 * Revision 3.1.2.10  1996/01/26  22:10:28  mbeesley
 * Branch: ELC_branch
 *         o Debug GT64010 timers and add support for counters
 *         o Increase GT64010 cache line size
 *         o Finish support for fine granularity throttling
 *
 * Revision 3.1.2.9  1996/01/23  01:06:54  mbeesley
 * Branch: ELC_branch
 * Dynamically configure PCI access to DRAM. Also make output of
 * show commands more usefull.
 *
 * Revision 3.1.2.8  1996/01/16  02:16:20  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.1.2.7  1996/01/09  21:00:51  mbeesley
 * Branch: ELC_branch
 * Fix naming of PCI memory address config registers.
 *
 * Revision 3.1.2.6  1996/01/03  06:14:57  wfried
 * Branch: ELC_branch
 * Introduce required extra Galileo error address register read and prevent
 * Galileo errors from crashing the box for now (during Predator bring-up)
 * for debugging purposes.
 *
 * Revision 3.1.2.5  1995/12/12  04:59:29  mbeesley
 * Branch: ELC_branch
 * Add parallel bridge protection on PCI config reads.
 *
 * Revision 3.1.2.4  1995/12/08  05:00:59  mbeesley
 * Branch: ELC_branch
 * Get NVRAM bank config correct. Fix some defines for PCI registers.
 *
 * Revision 3.1.2.3  1995/12/05  19:58:42  mbeesley
 * Branch: ELC_branch
 * Correct some register initialization.
 *
 * Revision 3.1.2.2  1995/11/30  02:09:19  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.1.2.1  1995/11/22  21:42:49  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/11/20  21:36:47  mbeesley
 * Add placeholder files for future development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "logger.h"
#include "../pas/msg_pas.c"		/* Not a typo, see logger.h */
#include "../src-4k/msg_gt64010.c"	/* Not a typo, see logger.h */
#include "stacks.h"
#include "ttysrv.h"
#include "subsys.h"
#include "config.h"
#include "parser.h"
#include "packet.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"

#include "../dev/monitor1.h"
#include "../os/gdb.h"

#include "../pas/if_pas.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"

#include "exec_show_gt64010.h"
#include "gt64010.h"
#include "gt64010_dma.h"

/*
 * Global storage
 */
gt64010_dma0_cntl_t 	*dma0;
volatile tinybool 	gt64010_post_coalesce;
uchar			gt64010_rev_level;

/*
 * Local storage
 */
static gt64010_timer_cntl_t gt64010_timers[GT64010_MAX_TIMERS];

/*
 * gt64010 parser extensions
 */
static parser_extension_request gt64010_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(show_gt64010_command) },
    { PARSE_LIST_END, NULL }
};

/*
 * gt64010_control_cpu_intr:
 * Control GT64010 cpu interrupt mask. This is controlled via 
 * "pci_cpu_mask". It is used for DMA and Timer interrupts only.
 */
static void gt64010_control_cpu_intr (boolean onoff)
{
    if (onoff) {
	/*
	 * Enabling cpu interrupts. We enable all DMA interrupts.
	 */
	GT64010_REGS->pci_cpu_mask = GT64010_CPU_INTR_MASK;
    } else {
	/*
	 * Disabling all cpu interrupts
	 */
	GT64010_REGS->pci_cpu_mask = 0;
    }
    flush_io_wbuffer();
}

/*
 * gt64010_control_error_intr:
 * Control GT64010 error interrupt mask. This is controlled via 
 * "pci_pci_mask" as errors are fed into the CPU from the
 * GT64010 PCI interrupt, thru the external interrupt controller.
 */
static void gt64010_control_error_intr (boolean onoff)
{
    if (onoff) {
	/*
	 * Enabling error interrupts. We enable all the timer interrupts
	 * as they should not fire unless used. All the DMA/Timer interrupts
	 * are masked as they are taken via the "cpu" interrupt from
	 * the gt64010.
	 *
	 * Note: Rev 2 gt64010 has a problem with DRAM out of range. So turn
	 * this error interrupt off.
	 */
	if (gt64010_rev_level == GT64010_REV1) {
	    GT64010_REGS->pci_pci_mask = GT64010_ERR_INTR_MASK;
	} else {
	    GT64010_REGS->pci_pci_mask = 
		(GT64010_ERR_INTR_MASK & ~INT_DRAM_OUT_OF_RANGE);
	}
    } else {
	/*
	 * Disabling all error interrupts
	 */
	GT64010_REGS->pci_pci_mask = 0;
    }
    flush_io_wbuffer();
}

/*
 * gt64010_init_dma0_channel:
 * Initialize everything for DMA0 (coalesce channel)
 */
static void gt64010_init_dma0_channel (void)
{
    gt64010_dma_ring_t *ring;
    gt64010_dma0_entry_t *dma;
    int i;

    /*
     * Get some memory
     */
    dma0 = malloc_fast(sizeof(gt64010_dma0_cntl_t));
    dma = malloc_fast(sizeof(gt64010_dma0_entry_t) * GT64010_DMA0_ENTRIES);
    if (!dma0 || !dma) {
	crashdump(0);			/* no memory */
    }

    /*
     * Allocate the shared ring data structure
     */
    ring = gt64010_dma_allocate_ring(dma0);

    /*
     * Init the list of stuff
     */
    dma0->dma_list = dma;
    dma0->dma_free = dma;
    dma0->dma_reqt = dma;
    dma0->dma_done = dma;
    dma0->dma_ring = ring;
    for (i=0; i<GT64010_DMA0_ENTRIES; i++) {
	if (i == (GT64010_DMA0_ENTRIES-1)) {
	    dma[i].next = &dma[0];
	} else {
	    dma[i].next = &dma[i+1];
	}
	dma[i].ring_ptr = &ring[i];
	ring[i].next_ptr = 0;
    }
}

/*
 * gt64010_pak_coalesce:
 * Initiate a DMA to coalesce a packet
 */
void gt64010_pak_coalesce (paktype *pak,
			   uchar *dest,
			   int count,
			   gt64010_dma0_entry_t *dma)
{
    gt64010_dma0_entry_t *dma_save = dma;
    gt64010_dma_ring_t *r;
    particletype *mp;
    int i, length;

    /*
     * Setup the entries
     */
    dma->particle_count = count;

    for (i=count-1; i>=0; i--) {

	mp = particle_dequeue_inline(pak);
	dma->particle = mp;

	r = dma->ring_ptr;
	length = max(mp->data_bytes, GT64010_MIN_DMA_BYTES);

	r->source = GT64010_DMA_DATA(mp->data_start);
	r->dest = GT64010_DMA_DATA(dest);
	r->byte_cnt = GT64010_DMA_DATA(length);

	dest += mp->data_bytes;

	/*
	 * Move to the next entry and link into the chain
	 */
	if (i) {
	    dma = dma->next;
	    r->next_ptr = GT64010_DMA_ADDR(dma->ring_ptr);
	} else {
	    r->next_ptr = (ulong)NULL;
	}
    }

    /*
     * Move the free pointer
     */
    dma0->dma_free = dma->next;

    /*
     * Commit this chain to the DMA engine
     */
    gt64010_dma0_commit(dma_save, dma, count);
}

/*
 * gt64010_dma0_complete:
 * Process a DMA channel 0 interrupt, coalesce channel
 */
static void gt64010_dma0_complete (void) 
{
    /*
     * Update the done pointer and let the network layer handler
     * know there is something to do.
     */
    dma0->dma_complete += 1;
    dma0->dma_done = dma0->thread_end->next;
    gt64010_post_coalesce = TRUE;
    
    /*
     * See if there is anything in the backup thread
     */
    if (dma0->backup_thread) {
	dma0->thread = dma0->backup_thread;
	dma0->thread_end = dma0->backup_thread_end;
	dma0->backup_thread = NULL;
	
	/*
	 * Start the engine
	 */
	gt64010_dma0_start(dma0->thread);
    } else {
	dma0->dma_working = FALSE;
    }

    /*
     * Issue a sw interrupt to make sure network layer thread runs
     */
    assert_sw_interrupt(NETWORK_SW_INTERRUPT);
}

/*
 * gt64010_ack_timers:
 * Acknowledge gt64010 timer interrupts
 */
static inline void gt64010_ack_timers (void)
{
    volatile uchar *ptr;

    /*
     * There is a bug in the gt64010 where by a write cycle to the 
     * pci_int_cause register to acknowledge an interrupt can inadvertently
     * acknowledge other interrupts (apart from the interrupt causes with
     * a zero bit in the write data). This can cause a missed interrupt.
     * The workarround is to use 8 bit write cycles to the pci_int_cause
     * register, thus not effecting the other three bytes in the register.
     */
    ptr = (volatile uchar *)(&GT64010_REGS->pci_int_cause);

    /*
     * Timer interrupts are in bits 23-16, which is at offset 1
     */
    *(ptr + 1) = (uchar)(~GT64010_TIMER_MASK >> 16);
}

/*
 * gt64010_ack_dma:
 * Acknowledge gt64010 dma interrupts
 */
static inline void gt64010_ack_dma (void)
{
    volatile uchar *ptr;

    /*
     * There is a bug in the gt64010 where by a write cycle to the 
     * pci_int_cause register to acknowledge an interrupt can inadvertently
     * acknowledge other interrupts (apart from the interrupt causes with
     * a zero bit in the write data). This can cause a missed interrupt.
     * The workarround is to use 8 bit write cycles to the pci_int_cause
     * register, thus not effecting the other three bytes in the register.
     */
    ptr = (volatile uchar *)(&GT64010_REGS->pci_int_cause);

    /*
     * DMA interrupts are in bits 31-24, which is at offset 0
     */
    *(ptr) = (uchar)(~GT64010_DMA_MASK >> 24);
}

/*
 * gt64010_ack_master_abort:
 * Acknowledge gt64010 master abort interrupt
 */
static inline void gt64010_ack_master_abort (void)
{
    volatile uchar *ptr;

    /*
     * There is a bug in the gt64010 where by a write cycle to the 
     * pci_int_cause register to acknowledge an interrupt can inadvertently
     * acknowledge other interrupts (apart from the interrupt causes with
     * a zero bit in the write data). This can cause a missed interrupt.
     * The workarround is to use 8 bit write cycles to the pci_int_cause
     * register, thus not effecting the other three bytes in the register.
     */
    ptr = (volatile uchar *)(&GT64010_REGS->pci_int_cause);

    /*
     * Master abort is in bit 11, which is at offset 2
     */
    *(ptr + 2) = (uchar)(~INT_MASTER_ABORT >> 8);
}

/*
 * gt64010_interrupt:
 * Interrupt handler for GT64010 DMA/Timer
 */
static void gt64010_interrupt (void)
{
    gt64010_timer_cntl_t *ptr;
    ulong cause;
    int timers, handled, i;

    /*
     * Handle Timer interrupts
     */
    cause = GT64010_REGS->pci_int_cause;
    if (cause & GT64010_TIMER_MASK) {

	/*
	 * Acknowledge the timer interrupts
	 */
	cause = cause & GT64010_TIMER_MASK;
	gt64010_ack_timers();
	timers = handled = 0;
	ptr = &gt64010_timers[0];

	/*
	 * Loop thru the four timers
	 */
	for (i=0; i<GT64010_MAX_TIMERS; i++, ptr++) {
	    if (cause & (INT_T0_EXP << i)) {
		timers += 1;
		if (ptr->func) {
		    (*ptr->func)();
		    handled += 1;
		}
	    }
	}
	if (timers != handled) {
	    errmsg(&msgsym(TIMER, GT64010), cause, GT64010_TIMER_MASK, 
		   cause);
	}
    }
     
    /*
     * Handle DMA interrupts
     */
    cause = GT64010_REGS->pci_int_cause;
    if (cause & GT64010_DMA_MASK) {

	/*
	 * Acknowledge and process any dma interrupts
	 */
	gt64010_ack_dma();
	if (cause & INT_DMA0_COMP) {
	    gt64010_dma0_complete();
	    cause &= ~INT_DMA0_COMP;
	}
	if (cause & GT64010_DMA_MASK) {
	    errmsg(&msgsym(DMA, GT64010), cause, GT64010_DMA_MASK, 
		   cause);
	}
    }
}

/*
 * gt64010_post_coalesce_dispatch:
 * Process post-coalesced packets. Gets called from the network IO
 * interrupt handler. Scans thru the DMA0 list, dispatching the
 * installed callback vector.
 */
void gt64010_post_coalesce_dispatch (void)
{
    gt64010_dma0_entry_t *ptr;
    int i, count;

    /*
     * Clear the flag and count the call
     */
    gt64010_post_coalesce = FALSE;
    dma0->post_coalesce_callback += 1;

    /*
     * Handle any completed packets
     */
    while (dma0->dma_reqt != dma0->dma_done) {

	/*
	 * Call the callback for this packet
	 */
	ptr = dma0->dma_reqt;
	(*ptr->func)(ptr->idb, ptr->newpak, ptr->iqueue);

	/*
	 * Return the particles for this packet
	 */
	count = ptr->particle_count;
	for (i=0; i<count; i++) {
	    retparticle_inline(ptr->particle);
	    ptr = ptr->next;
	}

	/*
	 * Move the request pointer
	 */
	dma0->dma_reqt = ptr;
	dma0->post_coalesce_frames += 1;
    }
}

/*
 * gt64010_fatal_errors:
 * Check for fatal error interrupts from the gt64010
 */
static char *gt64010_fatal_errors (ulong cause)
{
    if (cause & INT_DRAM_OUT_OF_RANGE) {
	return("Memory out of range error");
    }
    if (cause & INT_DMA_OUT_OF_RANGE) {
	return("DMA out of range error");
    }
    if (cause & INT_CPU_OUT_OF_RANGE) {
	return("CPU out of range error");
    }
    if (cause & INT_MASTER_READ_PAR) {
	return("Parity error on master read");
    }
    if (cause & INT_SLAVE_WRITE_PAR) {
	return("Parity error on slave write");
    }
    if (cause & INT_MASTER_WRITE_PAR) {
	return("Parity error on master write");
    }
    if (cause & INT_SLAVE_READ_PAR) {
	return("Parity error on slave read");
    }
    if (cause & INT_ADDRESS_PARITY) {
	return("Address parity error");
    }
    if (cause & INT_DRAM_ERROR) {
	return("Memory parity error (external)");
    }
    if (cause & INT_MASTER_ABORT) {
	return("PCI Master abort");
    }
    if (cause & INT_TARGET_ABORT) {
	return("PCI Target abort");
    }
    if (cause & INT_RETRY_CNT_EXP) {
	return("PCI retry counter expired");
    }
    return(NULL);
}

/*
 * gt64010_error_interrupt:
 * Error interrupt handler for GT64010. Return TRUE if we are to crash
 * the box.
 */
boolean gt64010_error_interrupt (void *context)
{
    char *str;
    ulong cause, mask, real_cause;
    volatile ulong dummy_reg;

    /*
     * Read the GT64010 interrupt cause register. It is assumed that all
     * systems will have the error interrupt hooked up thru the "pci"
     * interrupt pin on the GT64010, leaving the "cpu" interrupt
     * pin free for DMA interrupts only.
     */
    cause = GT64010_REGS->pci_int_cause;
    mask = GT64010_REGS->pci_pci_mask;
    real_cause = cause & mask;

    /*
     * If this is a PCI Master Abort because of a read access to a 
     * PCMCIA flash card that got removed, then recover gracefully from 
     * this error interrupt.
     *
     * Note: there is some very small risk that acknowledging the active
     * interrupt(s) will accidentally ack an outstanding timer and/or
     * dma interrupt due to a bug in the gt64010 chip. If this error interrupt
     * is not going to reset the box (IE master abort), it is unsafe to
     * write to bits 31..16 as this could accidentally ack a dma or
     * timer interrupt. Otherwise, the box is going to reset so it does
     * not matter
     */
    if ((real_cause == INT_MASTER_ABORT) && pcmcia_read_flag) {
	gt64010_ack_master_abort();
        restart_count_compare(TRUE);
        return(FALSE);
    }

    /*
     * Clear the interrupt. Note, do not have to worry about accidentally
     * acking a timer or dma interrupt as the box is going down.
     */
    GT64010_REGS->pci_int_cause = (~real_cause) | GT64010_CPU_INTR_MASK;

    /*
     * Start checking the causes
     */
    str = gt64010_fatal_errors(real_cause);
    if (str) {
	/*
	 * Fatal gt64010 error. Print some stuff out, disable
	 * all gt64010 "cpu" interrupts and return TRUE so we crash.
	 * Note: The gt64010 erratta says that the registers 
	 * with addr 0x000 to 0x0FF need to be read twice.
	 */
	dummy_reg = GT64010_REGS->bus_err_high;
	dummy_reg = GT64010_REGS->bus_err_low;
	ttyprintf(CONTTY, "\n%%ERR-1-GT64010: Fatal error, %s\n", str);
	ttyprintf(CONTTY, " cause=0x%08x, mask=0x%08x, real_cause=0x%08x\n", 
		  cause, mask, real_cause);
	ttyprintf(CONTTY, " bus_err_high=0x%08x, bus_err_low=0x%08x, "
		  "addr_decode_err=0x%08x\n",
		  vaxorder(GT64010_REGS->bus_err_high), 
		  vaxorder(GT64010_REGS->bus_err_low),
		  vaxorder(GT64010_REGS->addr_decode_err));
	if (gt64010_errors_are_fatal()) {
	    gt64010_control_error_intr(FALSE);
	    return(TRUE);
	} else {
	    restart_count_compare(TRUE);
	    return(FALSE);
	}
    }

    /*
     * So we have no fatal errors. Just print out a message and crash the box
     */
    ttyprintf(CONTTY, "%%UNKNOWN-1-GT64010: Unknown fatal interrupt(s)\n");
    ttyprintf(CONTTY, " cause=0x%08x, mask=0x%08x, real_cause=0x%08x\n", 
	      cause, mask, real_cause);

    if (gt64010_errors_are_fatal()) {
	gt64010_control_error_intr(FALSE);
	return(TRUE);
    } else {
	restart_count_compare(TRUE);
	return(FALSE);
    }
}

/*
 * gt64010_rev_check:
 * Check which revision of gt64010 is being used
 */
static void gt64010_rev_check (void)
{
    ulong sreg;
    uchar bus, dev, func, reg;
    ulong temp = 0;

    /*
     * Lock out interrupts 
     */
    sreg = getcp0_sreg();
    setcp0_sreg(sreg & ~SREG_IE);

    bus =  dev = func = 0;
    reg = GT64010_CLASS_REV_REG_OFFSET;

    /* 
     * OR in bus, device and func 
     */
    temp |= GT64010_CONFIG_ENABLE;
    temp |= (bus << GT64010_BUSNO_SHIFT);
    temp |= ((dev & 0x1f) << GT64010_DEVNUM_SHIFT);
    temp |= ((func & 0x07) << GT64010_FUNC_SHIFT);
    temp |= (reg << GT64010_REGNUM_SHIFT);

    /*
     * Write the config register
     */
    KSEG1_GT64010->pci_conf_addr = temp;
    flush_io_wbuffer();

    /*
     * Read the data and check the revision ID. For the rev 2 Galileo 
     * (GT64010A), the read will access the REV ID register. For the rev 1 
     * Galileo, which requires the register offset to be shifted left 2 
     * bits during a PCI config access, this read will access the VENDOR ID 
     * register instead of the REV ID register.
     * If bit 7-0 of the read value is 0xab, this is a rev 1 Galileo.
     */
    temp = ((KSEG1_GT64010->pci_conf_data & REVISION_ID) >>
	    REVISION_ID_SHIFT);
    if (temp == (GT64010_VENDOR_ID >> 8)) {
	gt64010_rev_level = GT64010_REV1;
    } else {
	gt64010_rev_level = GT64010_REV2;
    }

    /*
     * Restore interrupt mask
     */
    setcp0_sreg(sreg);
}

/*
 * pci_config_write:
 * Perform a PCI config write cycle
 */
void pci_config_write (uchar bus, uchar dev, uchar func, 
		       uchar reg, ulong value)
{
    leveltype level;
    boolean internal = FALSE;
    ulong temp = 0;

    /*
     * Lock out interrupts 
     */
    level = raise_interrupt_level(ALL_DISABLE);

    /*
     * Setup parallel bridge access work-arround
     */
    gt64010_setup_parallel_bridge(bus);

    /* 
     * OR in bus, device and func 
     */
    temp |= GT64010_CONFIG_ENABLE;
    temp |= (bus << GT64010_BUSNO_SHIFT);
    temp |= ((dev & 0x1f) << GT64010_DEVNUM_SHIFT);
    temp |= ((func & 0x07) << GT64010_FUNC_SHIFT);
    if (!bus && !dev) {
	if (gt64010_rev_level != GT64010_REV1) {
	    /*
	     * Rev 2 or more GT64010
	     */
	    temp |= (reg << GT64010_REGNUM_SHIFT);
	} else {
	    /*
	     * Rev 1 GT64010
	     */
	    temp |= (reg << (GT64010_REGNUM_SHIFT+2));
	}
	internal = TRUE;
    } else {
	/*
	 * This is an access to an external device
	 */
	temp |= (reg << GT64010_REGNUM_SHIFT);
    }

    /*
     * Write the config register
     */
    GT64010_REGS->pci_conf_addr = temp;
    flush_io_wbuffer();

    /*
     * Write the data. If it is not internal, then we must byte swap the 
     * data as the GT64010 will byte swap the data going out the PCI bus.
     */
    if (!internal) {
	GT64010_REGS->pci_conf_data = vaxorder(value);
    } else {
	GT64010_REGS->pci_conf_data = value;
    }

    /*
     * Restore parallel bridge primary bus setting
     */
    gt64010_restore_parallel_bridge(bus);

    /*
     * Restore interrupt mask
     */
    reset_interrupt_level(level);
}

/*
 * pci_config_read:
 * Perform a PCI config read cycle, returning the data
 */
ulong pci_config_read (uchar bus, uchar dev, uchar func, uchar reg)
{
    leveltype level;
    boolean internal = FALSE;
    ulong temp = 0;

    /*
     * Lock out interrupts 
     */
    level = raise_interrupt_level(ALL_DISABLE);

    /*
     * Setup parallel bridge access work-arround
     */
    gt64010_setup_parallel_bridge(bus);

    /* 
     * OR in bus, device and func 
     */
    temp |= GT64010_CONFIG_ENABLE;
    temp |= (bus << GT64010_BUSNO_SHIFT);
    temp |= ((dev & 0x1f) << GT64010_DEVNUM_SHIFT);
    temp |= ((func & 0x07) << GT64010_FUNC_SHIFT);
    if (!bus && !dev) {
	if (gt64010_rev_level != GT64010_REV1) {
	    /*
	     * Rev 2 or more GT64010
	     */
	    temp |= (reg << GT64010_REGNUM_SHIFT);
	} else {
	    /*
	     * Rev 1 GT64010
	     */
	    temp |= (reg << (GT64010_REGNUM_SHIFT+2));
	}
	internal = TRUE;
    } else {
	/*
	 * This is an access to an external device
	 */
	temp |= (reg << GT64010_REGNUM_SHIFT);
    }

    /*
     * Write the config register
     */
    GT64010_REGS->pci_conf_addr = temp;
    flush_io_wbuffer();

    /*
     * Read the data. If it is not internal, we need to byte swap the
     * data as the GT64010 will byte swap the data as it comes off the
     * PCI bus.
     */
    temp = GT64010_REGS->pci_conf_data;

    /*
     * Restore parallel bridge primary bus setting
     */
    gt64010_restore_parallel_bridge(bus);

    /*
     * Restore interrupt mask
     */
    reset_interrupt_level(level);

    if (!internal) {
	return(vaxorder(temp));
    } else {
	return(temp);
    }
}

/*
 * gt64010_check_vendor_id:
 * Check the Vendor/Device ID
 */
static void gt64010_check_vendor_id (void)
{
    ulong temp;
    ushort vendor_id, device_id;

    /*
     * Perform a config read for the vendor/device register
     * Bus 0, Device 0, Function 0, GT64010_VENDOR_OFFSET.
     */
    temp = pci_config_read(0, 0, 0, GT64010_VENDOR_REG_OFFSET);
    vendor_id = temp >> 16;
    device_id = temp & 0xffff;

    if (vendor_id != GT64010_VENDOR_ID) {
	ttyprintf(CONTTY, "GT64010: Vendor ID error, got %#x\n", vendor_id);
    }
    if (device_id != GT64010_DEVICE_ID) {
	ttyprintf(CONTTY, "GT64010: Device ID error, got %#x\n", device_id);
    }	
}

/*
 * gt64010_halfbank_size:
 * Return the size of a half of one DRAM bank
 */
static ulong gt64010_halfbank_size (volatile ulong *ptr)
{
    ulong ras_low, ras_high;

    ras_low = vaxorder(*ptr++);
    ras_high = vaxorder(*ptr);
    if (ras_low <= ras_high) {
	return((ras_high + 1 - ras_low) << 20);
    } else {
	return(0);
    }
}

/*
 * gt64010_config_ras_banksize:
 * Configure the PCI banksize registers, RAS1:0 and RAS3:2
 * Note: This routine really needs to look at the DRAM configuration
 * registers within the GT64010 to figure out the size of each bank.
 * For the time being, everything is hardcoded for 8Meg banks.
 */
static ulong gt64010_config_ras_banksize (void)
{
    ulong bank10;
    ulong banksize10, banksize32;
    ulong temp;

    /*
     * Configure pci_ras10_size
     */
    banksize10 = gt64010_halfbank_size(&GT64010_REGS->ras0_low);
    banksize10 += gt64010_halfbank_size(&GT64010_REGS->ras1_low);
    bank10 = banksize10;
    banksize10 >>= 13;
    temp = 0;
    while (banksize10) {
	temp = (temp << 1) | 0x1000;
	banksize10 >>= 1;
    }
    GT64010_REGS->pci_ras10_size = vaxorder(temp);

    /*
     * Configure pci_ras32_size
     */
    banksize32 = gt64010_halfbank_size(&GT64010_REGS->ras2_low);
    banksize32 += gt64010_halfbank_size(&GT64010_REGS->ras3_low);
    banksize32 >>= 13;
    temp = 0;
    while (banksize32) {
	temp = (temp << 1) | 0x1000;
	banksize32 >>= 1;
    }
    GT64010_REGS->pci_ras32_size = vaxorder(temp);
    return(bank10);
}

/*
 * gt64010_config_ras_bar:
 * Configure the PCI RAS1:0 and RAS3:2 base address registers.
 */ 
static void gt64010_config_ras_bar (ulong bank10)
{
    /*
     * RAS1:0 Base Address is always 0. And this bank is always
     * required.
     */
    pci_config_write(0, 0, 0, GT64010_RAS10_BASE_REG_OFFSET, 0x0);

    /*
     * RAS3:2 Base Address. Set it to bank10.
     */
    pci_config_write(0, 0, 0, GT64010_RAS32_BASE_REG_OFFSET,
		     vaxorder(bank10));
}

/*
 * gt64010_init:
 * Initialize the gt64010
 */
void gt64010_init (void)
{
    ulong bank10;
    int i;

    /*
     * Check the revision level of the chip
     */
    gt64010_rev_check();

    /*
     * Make sure this is a GT64010
     */
    gt64010_check_vendor_id();

    /* 
     * Configure internal registers for PCI subsystem
     */
    GT64010_REGS->pci_io_low = GT64010_PCI_IO_LOW_INIT;
    GT64010_REGS->pci_io_high = GT64010_PCI_IO_HIGH_INIT;
    GT64010_REGS->pci_mem_low = GT64010_PCI_MEM_LOW_INIT;
    GT64010_REGS->pci_mem_high = GT64010_PCI_MEM_HIGH_INIT;
    GT64010_REGS->pci_cs20_size = GT64010_PCI_CS20_INIT;
    GT64010_REGS->pci_cs3_boot_size = GT64010_PCI_CS3_BOOT_INIT;
    GT64010_REGS->pci_cmd = GT64010_PCI_CMD_INIT;
    if (gt64010_rev_level != GT64010_REV1) {
	GT64010_REGS->pci_timeout = GT64010_PCI_TIMEOUT_INIT;
    } else {
	GT64010_REGS->pci_timeout = GT64010_PCI_TIMEOUT_INIT_REV1;
    }

    /*
     * Configure pci ras1:0 and ras3:2 banksize
     */
    bank10 = gt64010_config_ras_banksize();

    /*
     * Configure PCI configuration registers
     * Status / Command
     */
    pci_config_write(0, 0, 0, GT64010_STAT_CMD_REG_OFFSET,
		     GT64010_STAT_CMD_INIT);

    /*
     * RAS[1:0], RAS[3:2] base address
     */
    gt64010_config_ras_bar(bank10);

    /*
     * CS[2:0]. Not used, set to zero.
     */
    pci_config_write(0, 0, 0, GT64010_CS20_BASE_REG_OFFSET, 0);

    /*
     * CS[3]. Not used, set to zero.
     */
    pci_config_write(0, 0, 0, GT64010_CS3_BASE_REG_OFFSET, 0);

    /*
     * Latency, Cache Line and Header type.
     */
    pci_config_write(0, 0, 0, GT64010_LATENCY_REG_OFFSET, GT64010_CACHE_LINE);

    /*
     * MemMapBase, IOMapBase, Interrupt / Pin line
     * Leave all these registers at their default values.
     */

    /*
     * Create a DMA/Timer interrupt stack
     */
    createlevel(LEVEL_DMA, gt64010_interrupt, "DMA/Timer Interrupt");

    /*
     * Init the timer/counter support
     */
    for (i=0; i<GT64010_MAX_TIMERS; i++) {
	gt64010_timers[i].tenable_mask = (0x01000000 << (i*2));
	gt64010_timers[i].cenable_mask = (0x03000000 << (i*2));
    }
    gt64010_timers[0].counter = &GT64010_REGS->ct0;
    gt64010_timers[1].counter = &GT64010_REGS->ct1;
    gt64010_timers[2].counter = &GT64010_REGS->ct2;
    gt64010_timers[3].counter = &GT64010_REGS->ct3;
    GT64010_REGS->counter_ctrl = 0;

    /*
     * Setup the interrupt masks
     */
    gt64010_control_cpu_intr(TRUE);
    gt64010_control_error_intr(TRUE);
}

/*
 * gt64010_install_timer:
 * Install a timer
 */
void gt64010_install_timer (int timer, gt64010_timer_vector_t func)
{
    leveltype level;

    if (timer >= GT64010_MAX_TIMERS) {
	return;
    }
    level = raise_interrupt_level(ALL_DISABLE);
    gt64010_timers[timer].func = func;
    reset_interrupt_level(level);
}

/*
 * gt64010_start_timer:
 * Start a timer for a particluar delay. Note, no interrupt protection.
 * Timers are one shots, parameter is in (usec*50 format).
 */
void gt64010_start_timer (int timer, ulong gt_usecs)
{
    gt64010_timer_cntl_t *ptr;

    if (timer >= GT64010_MAX_TIMERS) {
	return;
    }
    ptr = &gt64010_timers[timer];

    /*
     * Disable the timer, program it and start it up
     */
    GT64010_REGS->counter_ctrl &= ~(ptr->tenable_mask);
    *(ptr->counter) = gt_usecs;
    GT64010_REGS->counter_ctrl |= ptr->tenable_mask;
}

/*
 * gt64010_start_counter:
 * Start a timer for a particluar delay. Note, no interrupt protection.
 * Counters loop forever, parameter is in (usec*50 format).
 */
void gt64010_start_counter (int counter, ulong gt_usecs)
{
    gt64010_timer_cntl_t *ptr;

    if (counter >= GT64010_MAX_TIMERS) {
	return;
    }
    ptr = &gt64010_timers[counter];

    /*
     * Disable the counter, program it and start it up
     */
    GT64010_REGS->counter_ctrl &= ~(ptr->cenable_mask);
    *(ptr->counter) = gt_usecs;
    GT64010_REGS->counter_ctrl |= ptr->cenable_mask;
}

/*
 * gt64010_stop_timer:
 * Stop a timer.
 */
void gt64010_stop_timer (int timer)
{
    gt64010_timer_cntl_t *ptr;

    if (timer >= GT64010_MAX_TIMERS) {
	return;
    }
    ptr = &gt64010_timers[timer];

    /*
     * Disable the timer
     */
    GT64010_REGS->counter_ctrl &= ~(ptr->tenable_mask);
}

/*
 * gt64010_stop_counter:
 * Stop a counter.
 */
void gt64010_stop_counter (int counter)
{
    gt64010_timer_cntl_t *ptr;

    if (counter >= GT64010_MAX_TIMERS) {
	return;
    }
    ptr = &gt64010_timers[counter];

    /*
     * Disable the counter
     */
    GT64010_REGS->counter_ctrl &= ~(ptr->cenable_mask);
}

/*
 * gt64010_show_dma0:
 * Display some info wrt the DMA 0 channel
 */
static void gt64010_show_dma0 (void)
{
    gt64010_dma0_entry_t *t_free, *t_reqt, *t_done,
    			 *t_thread, *t_thread_end,
    			 *t_backup_thread, *t_backup_thread_end;
    ulong 		 t_working, t_complete, t_exhausted, t_callback, 
    			 t_coalesce_frames;
    leveltype 		 level;

    /*
     * Take a snap shot of the info
     */
    level = raise_interrupt_level(LEVEL_DMA);
    t_free = dma0->dma_free;
    t_reqt = dma0->dma_reqt;
    t_done = dma0->dma_done;
    t_thread = dma0->thread;
    t_thread_end = dma0->thread_end;
    t_backup_thread = dma0->backup_thread;
    t_backup_thread_end = dma0->backup_thread_end;
    t_working = dma0->dma_working;
    t_complete = dma0->dma_complete;
    t_exhausted = dma0->exhausted_dma_entries;
    t_callback = dma0->post_coalesce_callback;
    t_coalesce_frames = dma0->post_coalesce_frames;
    reset_interrupt_level(level);

    /*
     * And print it all out
     */
    printf("\nGT64010 Rev level     : %d\n", gt64010_rev_level);
    printf("\nGT64010 Channel 0 DMA :\n");
    printf(" dma_list=%#x, dma_ring=%#x, dma_entries=%d\n",
	   dma0->dma_list, dma0->dma_ring, GT64010_DMA0_ENTRIES);
    printf(" dma_free=%#x, dma_reqt=%#x, dma_done=%#x\n",
	   t_free, t_reqt, t_done);
    printf(" thread=%#x, thread_end=%#x\n", t_thread, t_thread_end);
    printf(" backup_thread=%#x, backup_thread_end=%#x\n",
	   t_backup_thread, t_backup_thread_end);
    printf(" dma_working=%u, dma_complete=%u, post_coalesce_frames=%u\n", 
	   t_working, t_complete, t_coalesce_frames);
    printf(" exhausted_dma_entries=%u, post_coalesce_callback=%u\n\n",
	   t_exhausted, t_callback);
}

/*
 * Macro for displaying a register
 */
static char gt_show_fmt[] = " %20s : 0x%08x (b/s 0x%08x)\n";

#define GT64010_SHOW_REG(x, y)		\
    printf(gt_show_fmt, "" # y, x->y, vaxorder(x->y))

/*
 * show_gt64010:
 * Print out dma info and the register set of the GT64010
 */
void show_gt64010 (parseinfo *csb)
{
    gt64010_regs_t *p;

    automore_enable(NULL);
    gt64010_show_dma0();

    p = GT64010_REGS;
    printf("GT64010 Register Dump : Registers at 0x%08x\n\n", p);

    /*
     * Print out CPU interface configuration
     */
    printf("CPU Interface :\n");
    GT64010_SHOW_REG(p, cpu_interface_conf);
    GT64010_SHOW_REG(p, addr_decode_err);
    
    /*
     * Processor Address space
     */
    printf("Processor Address Space :\n");
    GT64010_SHOW_REG(p, ras10_low);
    GT64010_SHOW_REG(p, ras10_high);
    GT64010_SHOW_REG(p, ras32_low);
    GT64010_SHOW_REG(p, ras32_high);
    GT64010_SHOW_REG(p, cs20_low);
    GT64010_SHOW_REG(p, cs20_high);
    GT64010_SHOW_REG(p, cs3_boot_low);
    GT64010_SHOW_REG(p, cs3_boot_high);
    GT64010_SHOW_REG(p, pci_io_low);
    GT64010_SHOW_REG(p, pci_io_high);
    GT64010_SHOW_REG(p, pci_mem_low);
    GT64010_SHOW_REG(p, pci_mem_high);
    GT64010_SHOW_REG(p, internal_spc_decode);
    GT64010_SHOW_REG(p, bus_err_low);
    GT64010_SHOW_REG(p, bus_err_high);

    /*
     * DRAM and Device Address Space
     */
    printf("DRAM/Device Address Space :\n");
    GT64010_SHOW_REG(p, ras0_low);
    GT64010_SHOW_REG(p, ras0_high);
    GT64010_SHOW_REG(p, ras1_low);
    GT64010_SHOW_REG(p, ras1_high);
    GT64010_SHOW_REG(p, ras2_low);
    GT64010_SHOW_REG(p, ras2_high);
    GT64010_SHOW_REG(p, ras3_low);
    GT64010_SHOW_REG(p, ras3_high);
    GT64010_SHOW_REG(p, cs0_low);
    GT64010_SHOW_REG(p, cs0_high);
    GT64010_SHOW_REG(p, cs1_low);
    GT64010_SHOW_REG(p, cs1_high);
    GT64010_SHOW_REG(p, cs2_low);
    GT64010_SHOW_REG(p, cs2_high);
    GT64010_SHOW_REG(p, cs3_low);
    GT64010_SHOW_REG(p, cs3_high);
    GT64010_SHOW_REG(p, boot_cs_low);
    GT64010_SHOW_REG(p, boot_cs_high);
    GT64010_SHOW_REG(p, dram_config);
    GT64010_SHOW_REG(p, dram_bank0_param);
    GT64010_SHOW_REG(p, dram_bank1_param);
    GT64010_SHOW_REG(p, dram_bank2_param);
    GT64010_SHOW_REG(p, dram_bank3_param);

    /*
     * Device Parameters
     */
    printf("Device Parameters :\n");
    GT64010_SHOW_REG(p, device_bank0_param);
    GT64010_SHOW_REG(p, device_bank1_param);
    GT64010_SHOW_REG(p, device_bank2_param);
    GT64010_SHOW_REG(p, device_bank3_param);
    GT64010_SHOW_REG(p, boot_bank_param);

    /*
     * DMA record
     */
    printf("DMA Channels :\n");
    GT64010_SHOW_REG(p, ch0_dma_count);
    GT64010_SHOW_REG(p, ch1_dma_count);
    GT64010_SHOW_REG(p, ch2_dma_count);
    GT64010_SHOW_REG(p, ch3_dma_count);
    GT64010_SHOW_REG(p, ch0_dma_src);
    GT64010_SHOW_REG(p, ch1_dma_src);
    GT64010_SHOW_REG(p, ch2_dma_src);
    GT64010_SHOW_REG(p, ch3_dma_src);
    GT64010_SHOW_REG(p, ch0_dma_dst);
    GT64010_SHOW_REG(p, ch1_dma_dst);
    GT64010_SHOW_REG(p, ch2_dma_dst);
    GT64010_SHOW_REG(p, ch3_dma_dst);
    GT64010_SHOW_REG(p, ch0_next_ptr);
    GT64010_SHOW_REG(p, ch1_next_ptr);
    GT64010_SHOW_REG(p, ch2_next_ptr);
    GT64010_SHOW_REG(p, ch3_next_ptr);
    GT64010_SHOW_REG(p, ch0_ctrl);
    GT64010_SHOW_REG(p, ch1_ctrl);
    GT64010_SHOW_REG(p, ch2_ctrl);
    GT64010_SHOW_REG(p, ch3_ctrl);
    GT64010_SHOW_REG(p, arbiter_ctrl);

    /*
     * Timer/Counters
     */
    printf("Timer/Counters :\n");
    GT64010_SHOW_REG(p, ct0);
    GT64010_SHOW_REG(p, ct1);
    GT64010_SHOW_REG(p, ct2);
    GT64010_SHOW_REG(p, ct3);
    GT64010_SHOW_REG(p, counter_ctrl);

    /*
     * PCI Internals
     */
    printf("PCI Internal Registers :\n");
    GT64010_SHOW_REG(p, pci_cmd);
    GT64010_SHOW_REG(p, pci_timeout);
    GT64010_SHOW_REG(p, pci_ras10_size);
    GT64010_SHOW_REG(p, pci_ras32_size);
    GT64010_SHOW_REG(p, pci_cs20_size);
    GT64010_SHOW_REG(p, pci_cs3_boot_size);
    GT64010_SHOW_REG(p, pci_serr_mask);

    /*
     * Interrupt Control
     */
    printf("Interrupt Control :\n");
    GT64010_SHOW_REG(p, pci_int_cause);
    GT64010_SHOW_REG(p, pci_cpu_mask);
    GT64010_SHOW_REG(p, pci_pci_mask);

    /*
     * We are finished
     */
    automore_disable();
}

/*
 * gt64010_show_pci_registers:
 * Print out the PCI registers from the GT64010
 */
static char gt_pci_fmt[] = " %20s : 0x%08x (b/s 0x%08x)\n";

void gt64010_show_pci_registers (void)
{
    ulong temp;

    printf("GT64010 External PCI Configuration registers:\n");

    /*
     * Vendor / Device ID
     */
    temp = pci_config_read(0, 0, 0, GT64010_VENDOR_REG_OFFSET);
    printf(gt_pci_fmt, "Vendor / Device ID", temp, vaxorder(temp));

    /*
     * Status / Cmd
     */
    temp = pci_config_read(0, 0, 0, GT64010_STAT_CMD_REG_OFFSET);
    printf(gt_pci_fmt, "Status / Command", temp, vaxorder(temp));

    /*
     * Class / Revision
     */
    temp = pci_config_read(0, 0, 0, GT64010_CLASS_REV_REG_OFFSET);
    printf(gt_pci_fmt, "Class / Revision", temp, vaxorder(temp));

    /*
     * Latency timer
     */
    temp = pci_config_read(0, 0, 0, GT64010_LATENCY_REG_OFFSET);
    printf(gt_pci_fmt, "Latency", temp, vaxorder(temp)); 

    /*
     * RAS[1:0] Base
     */
    temp = pci_config_read(0, 0, 0, GT64010_RAS10_BASE_REG_OFFSET);
    printf(gt_pci_fmt, "RAS[1:0] Base", temp, vaxorder(temp)); 

    /*
     * RAS[3:2] Base
     */
    temp = pci_config_read(0, 0, 0, GT64010_RAS32_BASE_REG_OFFSET);
    printf(gt_pci_fmt, "RAS[3:2] Base", temp, vaxorder(temp)); 

    /*
     * CS[2:0] Base
     */
    temp = pci_config_read(0, 0, 0, GT64010_CS20_BASE_REG_OFFSET);
    printf(gt_pci_fmt, "CS[2:0] Base", temp, vaxorder(temp)); 

    /*
     * CS[3] Base
     */
    temp = pci_config_read(0, 0, 0, GT64010_CS3_BASE_REG_OFFSET);
    printf(gt_pci_fmt, "CS[3] Base", temp, vaxorder(temp)); 

    /*
     * MemMap Base
     */
    temp = pci_config_read(0, 0, 0, GT64010_MEMMAPBASE_REG_OFFSET);
    printf(gt_pci_fmt, "Mem Map Base", temp, vaxorder(temp));

    /*
     * IOMap Base
     */
    temp = pci_config_read(0, 0, 0, GT64010_IOMAPBASE_REG_OFFSET);
    printf(gt_pci_fmt, "IO Map Base", temp, vaxorder(temp)); 

    /*
     * Int Pin / Line
     */
    temp = pci_config_read(0, 0, 0, GT64010_INTPIN_LINE_REG_OFFSET);
    printf(gt_pci_fmt, "Int Pin / Line", temp, vaxorder(temp)); 
}

/*
 * gt64010_subsys_init:
 * Init the gt64010 subsystem
 */
static void gt64010_subsys_init (subsystype *subsys)
{
    /*
     * Add the parser support
     */
    parser_add_command_list(gt64010_chain_init_table, "gt64010");
    
    /*
     * Init DMA0 channel
     */
    gt64010_init_dma0_channel();
}

/*
 * GT64010 subsystem header
 */
#define GT64010_MAJVERSION   1
#define GT64010_MINVERSION   0
#define GT64010_EDITVERSION  1

SUBSYS_HEADER(gt64010, GT64010_MAJVERSION, GT64010_MINVERSION,
	      GT64010_EDITVERSION,
	      gt64010_subsys_init, SUBSYS_CLASS_DRIVER,
	      NULL,
	      NULL);

/* end of file */
